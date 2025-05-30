#include <stdio.h>
#include <stdlib.h>
#include <stdalign.h>
#include <string.h>
#include "comot-css/tokens.h"
#include "comot-css/tokenizer.h"
#include "comot-css/diag.h"
#include "decoder.h"
#include "tokenizer_impl.h"

#define ARENA_ALIGNMENT alignof(max_align_t)

/**
 * @brief Create a tokenizer from a raw CSS input.
 *
 * This function initializes a tokenizer by decoding the CSS input and
 * allocating a Tokenizer structure. It will return NULL if the input is
 * invalid or if the arena allocation fails.
 *
 * @param raw The raw CSS input to decode.
 * @param len The length of the input.
 * @param arena The arena to allocate memory from.
 *
 * @return A pointer to a new Tokenizer, or NULL on failure.
 */
Tokenizer *tokCreate(const uint8_t *raw, size_t len, Arena *arena) {
  if(!arena || !raw) 
    return NULL;

  DecodedStream *s = arena_alloc(arena, len * sizeof(DecodedStream), ARENA_ALIGNMENT);
  if(!s)
    return NULL;

  size_t count = decodeCssInput(raw, len, s, len);
  if(count == 0)
    return NULL;

  printf("Decoded %zu code points.\n", count);

  Tokenizer *t = arena_alloc(arena, sizeof(Tokenizer), ARENA_ALIGNMENT);
  if(!t)
    return NULL;

  t->start = s;
  t->curr = s;
  t->end = s + count;
  t->line = 1;
  t->column = 1;
  t->state = DATA_STATE;
  t->shouldLog = 1;
  t->errorCount = 0;
  t->maxErrors = 10;
  t->stringQuote = '\0';
  t->arena = arena;

  return t;
}

/**
 * Retrieves the next token from the tokenizer's input stream.
 *
 * This function processes the current state of the tokenizer's finite
 * state machine (FSM) to determine the type of token present at the
 * current position in the input stream. It handles various token types
 * including identifiers, strings, numbers, delimiters, and whitespace.
 * The tokenizer's state is updated accordingly, and the appropriate
 * token is returned.
 *
 * @param t Pointer to the Tokenizer instance.
 * @return The next Token in the input stream, or an error Token if
 *         the tokenizer is in an invalid state or encounters an
 *         unexpected input.
 */
Token tokNext(Tokenizer *t) {
  if(!t) {
    printf("Invalid or NULL Tokenizer type t");

    exit(1);
  }

  // Skip over any characters already consumed
  while (t->curr < t->end) {
    // Save position
    const DecodedStream *start = t->curr;
    size_t line = t->line;
    size_t column = t->column;

    // NOTE: FSM RUN
    switch(t->state) {
      case DATA_STATE:
        if(start && start->bytePtr && isIdentStartCodePoint(start->bytePtr)) {
          // Possibly start of identifier
          t->state = IDENTIFIER_STATE;
        }
        else if(start && start->bytePtr && (*start->bytePtr == '"' || *start->bytePtr == '\'')) {
          t->state = STRING_STATE;
        }
        else if(start && start->bytePtr && isDigit(start->bytePtr)) {
          t->state = NUMBER_STATE;
        }
        else if (start && start->bytePtr && t->curr < t->end && isWhitespace(start->bytePtr)) {
          t->state = WHITESPACE_STATE;
        }
        else {
            t->state = DELIM_STATE;
        }

        return tokNext(t);
      case IDENTIFIER_STATE: 
        // Identifiers
        t->state = DATA_STATE;
        if(start && start->bytePtr && isIdentCodePoint(start))
          return consumeIdentLikeToken(t);

        break;

      case STRING_STATE:
        // Double or single quotation mark (")
        t->state = DATA_STATE;
        return consumeString(t, *start->bytePtr);

        break;

      case NUMBER_STATE:
        // Digit
        t->state = DATA_STATE;
        return consumeNumericToken(t);

        break;

      case WHITESPACE_STATE:
        // Whitespace
        while (t->curr < t->end && isWhitespace(t->curr->bytePtr)) {
          if(!advancePtrToN(t, 1))
            break;
        }

        t->state = DATA_STATE;
        return makeToken(TOKEN_WHITESPACE, TOKEN_KIND_VALID, start, t->curr - start, line, column);

        break;

      case DELIM_STATE:
        t->state = DATA_STATE;

        // Comments or '/'
        if(start && start->bytePtr && *start->bytePtr == '/') {
          return consumeCommentOrDelim(t, '/');
        }

        // Number sign (#)
        if(start && start->bytePtr && *start->bytePtr == '#') {
          advancePtrToN(t, 1);

          const DecodedStream *nxtPtr = t->curr;
          if(isIdentCodePoint(nxtPtr) || (isNCodePointValidEscape(t, 0) && isNCodePointValidEscape(t, 1))) {
            if(isNextThreeCodePointStartAnIdentSequence(t)) {
              const DecodedStream *currStream = consumeIdentSequence(t);

              return makeToken(TOKEN_HASH, TOKEN_KIND_VALID, start, currStream - start, line, column);
            }
          }
          else {
            advancePtrToN(t, 1);

            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Left parenthesis (()
        if(start && start->bytePtr && *start->bytePtr == '(') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_LEFT_PAREN, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Right parenthesis ())
        if(start && start->bytePtr && *start->bytePtr == ')') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_RIGHT_PAREN, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Plus sign (+)
        if(start && start->bytePtr && *start->bytePtr == '+') {
          if(isNextThreeCodePointStartNumber(t)) {
            return consumeNumericToken(t);
          }
          else {
            advancePtrToN(t, 1);
    
            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Comma (,)
        if(start && start->bytePtr && *start->bytePtr == ',') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_COMMA, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Hyphen/Minus sign (-)
        if(start && start->bytePtr && *start->bytePtr == '-') {
          const DecodedStream *nxtPtr = peekPtrAtN(t, 1);
          const DecodedStream *nxt2Ptr = peekPtrAtN(t, 2);

          // Validate next two pointers and their bytePtrs
          if(nxtPtr && nxt2Ptr) {
            if(isNextThreeCodePointStartNumber(t)) {
              
              return consumeNumericToken(t);
            }
            else if(*nxtPtr->bytePtr == '-' && *nxt2Ptr->bytePtr == '>') {
              advancePtrToN(t, 3);
            
              return makeToken(TOKEN_CDC, TOKEN_KIND_VALID, start, t->curr - start, line, column);
            }
            else if(isNextThreeCodePointStartAnIdentSequence(t)) {
              return consumeIdentLikeToken(t);
            }
            else {
              advancePtrToN(t, 1);
            
              return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
            }
          } 
          else {
            advancePtrToN(t, 1);
          
            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Full stop (.)
        if(start && start->bytePtr && *start->bytePtr == '.') {
          if(isNextThreeCodePointStartNumber(t)) {
            return consumeNumericToken(t);
          }
          else {
            advancePtrToN(t, 1);

            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Semicolon (;)
        if(start && start->bytePtr && *start->bytePtr == ';') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_SEMICOLON, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Colon (:)
        if(start && start->bytePtr && *start->bytePtr == ':') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_COLON, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Less than sign (<)
        if(start && start->bytePtr && *start->bytePtr == '<') {
          const DecodedStream *nxtPtr = peekPtrAtN(t, 1);
          const DecodedStream *nxt2Ptr = peekPtrAtN(t, 2);
          const DecodedStream *nxt3Ptr = peekPtrAtN(t, 3);

          if(nxtPtr && *nxtPtr->bytePtr == '!' && nxt2Ptr && *nxt2Ptr->bytePtr == '-' && nxt3Ptr && *nxt3Ptr->bytePtr == '-') {
            advancePtrToN(t, 3);

            return makeToken(TOKEN_CDO, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
          else {
            advancePtrToN(t, 1);

            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Commercial 'AT' symbol (@)
        if(start && start->bytePtr && *start->bytePtr == '@') {
          advancePtrToN(t, 1);

          if(isNextThreeCodePointStartAnIdentSequence(t)) {
            const DecodedStream *currStream = consumeIdentSequence(t);

            return makeToken(TOKEN_AT_KEYWORD, TOKEN_KIND_VALID, start, currStream - start, line, column);
          }
          else {
            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
          }
        }

        // Left square bracket ([)
        if(start && start->bytePtr && *start->bytePtr == '[') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_LEFT_SQUARE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Reverse solidus (\)
        if(start && start->bytePtr && *start->bytePtr == '\\') {
          if(isNCodePointValidEscape(t, 1)) {
            if(advancePtrToN(t, 1))
              return consumeIdentLikeToken(t);
          }
          else {
            // [PARSE ERR] end of file was reached before the end of string
            logDiagnostic("Invalid escape sequence", start->bytePtr, line, column);

            advancePtrToN(t, 1);
            size_t delta = (t->curr >= start) ? (t->curr - start) : 0;

            return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, delta, line, column);
          }
        }

        // Right square bracket (])
        if(start && start->bytePtr && *start->bytePtr == ']') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_RIGHT_SQUARE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Left curly bracket ({)
        if(start && start->bytePtr && *start->bytePtr == '{') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_LEFT_CURLY, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Right curly bracket (})
        if(start && start->bytePtr && *start->bytePtr == '}') {
          advancePtrToN(t, 1);

          return makeToken(TOKEN_RIGHT_CURLY, TOKEN_KIND_VALID, start, t->curr - start, line, column);
        }

        // Anything else as a delim
        advancePtrToN(t, 1);
        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);

      default:
        printf("Unknown tokenizer state: %d\n", t->state);
        exit(1);
    }

    // EOF
    if(isEof(t)) {
      return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, line, column);
    }

  }

  // If we fall through the loop, return EOF
  return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, t->line, t->column);
}
