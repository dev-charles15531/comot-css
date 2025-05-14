#include <string.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

static void consumeReminantsOfBadUrl(Tokenizer *t) {
  while(advancePtrToN(t, 1)) {
    if(*t->curr == ')' || isEof(t)) {
      return;
    }

    if(isNCodePointValidEscape(t, 0)) {
      consumeEscapedCodePoint(t, *t->curr);
    }
  }
}

Token consumeUrlToken(Tokenizer *t) {
  const char *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  while(isWhitespace(t->curr)) {
    advancePtrToN(t, 1);
  }

  while(t->curr) {
    const char *charAtCurrPtr = t->curr;
    if(*charAtCurrPtr == ')') {
      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(isEof(t)) {
      // TODO: [parse err]
      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(isWhitespace(charAtCurrPtr)) {
      while(isWhitespace(t->curr)) {
        advancePtrToN(t, 1);
      }

      if(*t->curr == ')' || isEof(t)) {
        if(isEof(t)) {
          // TODO: [parse err]
        }

        advancePtrToN(t, 1);

        return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
      }
      else {
        consumeReminantsOfBadUrl(t);

        return makeToken(TOKEN_BAD_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
      }
    }

    if(*charAtCurrPtr == '"' || *charAtCurrPtr == '\'' || *charAtCurrPtr == '(') {
      // TODO: [parse err]

      consumeReminantsOfBadUrl(t);

      return makeToken(TOKEN_BAD_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(*charAtCurrPtr == '\\') {
      if(isNCodePointValidEscape(t, 0)) {
        consumeEscapedCodePoint(t, *t->curr);
      }
      else {
        // TODO: [parse err]

        consumeReminantsOfBadUrl(t);

        return makeToken(TOKEN_BAD_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
      }

      continue;
    }

    // advance pointer
    advancePtrToN(t, 1);
  }

  return makeToken(TOKEN_BAD_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
}

