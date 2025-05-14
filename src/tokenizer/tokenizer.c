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
    //char charAtCurrPtr = *t->curr;

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
        // Track line/column
        if (*t->curr == '\n') {
          t->line++;
          t->column = 1;
        }
        else {
          t->column++;
        }
        t->curr++;
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


    // Example: EOF check (can be improved)
    if (t->curr >= t->end || *t->curr == '\0') {
      return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, line, column);
    }

    // For now, treat anything else as a delim (placeholder logic)
    t->curr++;
    t->column++;
    return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, start, t->curr - start, line, column);
  }

  // If we fall through the loop, return EOF
  return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, t->line, t->column);
}
