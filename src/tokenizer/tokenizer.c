#include <stdio.h>
#include <stdalign.h>
#include <string.h>
#include "comot-css/tokens.h"
#include "comot-css/tokenizer.h"
#include "tokenizer_impl.h"

#define ARENA_SIZE (64 * 1024) // 64 KB
#define ARENA_ALIGNMENT alignof(max_align_t)

Tokenizer *tok_create(const char *input, Arena *arena) {
  if(!arena)
    return NULL;

  Tokenizer *t = (Tokenizer *) arena_alloc(arena, sizeof(Tokenizer), ARENA_ALIGNMENT);
  if(!t)
    return NULL;

  t->start = input;
  t->curr = input;
  t->end = input + strlen(input);
  t->line = 1;
  t->column = 1;
  t->state = DATA_STATE;
  t->stringQuote = '\0';
  t->arena = arena;

  return t;
}

Token tok_next(Tokenizer *t) {
  // Skip over any characters already consumed
  while (t->curr < t->end) {
    // Save position
    const char *start = t->curr;
    size_t line = t->line;
    size_t column = t->column;

    // Comments or '/'
    if(*start == '/') {
      return consumeCommentOrDelim(t, '/');
    }

    // Whitespace
    if(isWhitespace(start)) {
      while (t->curr < t->end && isWhitespace(t->curr)) {
        advancePtrToN(t, 1);
      }

      return makeToken(TOKEN_WHITESPACE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Double quotation mark (")
    if(*start == '"')
      return consumeString(t, '"');

    // Number sign (#)
    if(*start == '#') {
      advancePtrToN(t, 1);

      const char *nxtPtr = t->curr;
      if(isIdentCodePoint(nxtPtr) || (isNCodePointValidEscape(t, 0) && isNCodePointValidEscape(t, 1))) {
        if(isNextThreeCodePointStartAnIdentSequence(t)) {
          const char *currPtr = consumeIdentSequence(t);

          return makeToken(TOKEN_HASH, TOKEN_KIND_VALID, start, currPtr - start, line, column);
        }
      }
      else {
        advancePtrToN(t, 1);

        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Single quotation mark (')
    if(*start == '\'')
      return consumeString(t, '\'');
    
    // Left parenthesis (()
    if(*start == '(') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_LEFT_PAREN, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Right parenthesis ())
    if(*start == ')') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_RIGHT_PAREN, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Plus sign (+)
    if(*start == '+') {
      if(isNextThreeCodePointStartNumber(t)) {
        return consumeNumericToken(t);
      }
      else {
        advancePtrToN(t, 1);
    
        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Comma (,)
    if(*start == ',') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_COMMA, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Identifiers
    if (isIdentStartCodePoint(start)) {
      return consumeIdentLikeToken(t);
    }

    // Digit
    if(isDigit(start)) {
      return consumeNumericToken(t);
    }

    // Hyphen/Minus sign (-)
    if(*start == '-') {
      const char *nxtPtr = peekPtrAtN(t, 1);
      const char *nxt2Ptr = peekPtrAtN(t, 2);

      if(isNextThreeCodePointStartNumber(t)) {
        return consumeNumericToken(t);
      }
      else if(*nxtPtr == '-' && *nxt2Ptr == '>') {
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

    // Full stop (.)
    if(*start == '.') {
      if(isNextThreeCodePointStartNumber(t)) {
        return consumeNumericToken(t);
      }
      else {
        advancePtrToN(t, 1);

        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Semicolon (;)
    if(*start == ';') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_SEMICOLON, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Colon (:)
    if(*start == ':') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_COLON, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Less than sign (<)
    if(*start == '<') {
      const char *nxtPtr = peekPtrAtN(t, 1);
      const char *nxt2Ptr = peekPtrAtN(t, 2);
      const char *nxt3Ptr = peekPtrAtN(t, 3);

      if(*nxtPtr == '!' && *nxt2Ptr == '-' && *nxt3Ptr == '-') {
        advancePtrToN(t, 3);

        return makeToken(TOKEN_CDO, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
      else {
        advancePtrToN(t, 1);

        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Commercial 'AT' symbol (@)
    if(*start == '@') {
      advancePtrToN(t, 1);

      if(isNextThreeCodePointStartAnIdentSequence(t)) {
        const char *currPtr = consumeIdentSequence(t);

        return makeToken(TOKEN_AT_KEYWORD, TOKEN_KIND_VALID, start, currPtr - start, line, column);
      }
      else {
        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Left square bracket ([)
    if(*start == '[') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_LEFT_SQUARE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Reverse solidus (\)
    if(*start == '\\') {
      if(isNCodePointValidEscape(t, 0)) {
        return consumeIdentLikeToken(t);
      }
      else {
        // TODO: [parse err]
        advancePtrToN(t, 1);

        return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
      }
    }

    // Right square bracket (])
    if(*start == ']') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_RIGHT_SQUARE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Left curly bracket ({)
    if(*start == '{') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_LEFT_CURLY, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Right curly bracket (})
    if(*start == '}') {
      advancePtrToN(t, 1);

      return makeToken(TOKEN_RIGHT_CURLY, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }
    
    // Digit
    // if(isDigit(start)) {
    //   printf("Here: %s\n", start);
    //   return consumeNumericToken(t);
    // }

    // EOF
    if(isEof(t)) {
      return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, t->curr - start, line, column);
    }

    // Anything else as a delim
    advancePtrToN(t, 1);
    return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
  }

  // If we fall through the loop, return EOF
  return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, t->curr - t->start, t->line, t->column);
}
