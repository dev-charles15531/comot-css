#include <string.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/diag.h"

static void consumeReminantsOfBadUrl(Tokenizer *t) {
  while(advancePtrToN(t, 1)) {
    if(*t->curr->bytePtr == ')' || isEof(t)) {
      return;
    }

    if(isNCodePointValidEscape(t, 0)) {
      consumeEscapedCodePoint(t); // NOTE: no code point is returned from this function
    }
  }
}

Token consumeUrlToken(Tokenizer *t) {
  const DecodedStream *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  while(isWhitespace(t->curr->bytePtr)) {
    advancePtrToN(t, 1);
  }

  while(t->curr) {
    const char *charAtCurrPtr = t->curr->bytePtr;
    if(*charAtCurrPtr == ')') {
      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(isEof(t)) {
      // [PARSE ERR] end of file was reached before the end of string
      logDiagnostic("Unexpected end of file", tCurr->bytePtr, startLine, startCol);

      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(isWhitespace(charAtCurrPtr)) {
      while(isWhitespace(t->curr->bytePtr)) {
        advancePtrToN(t, 1);
      }

      if(*t->curr->bytePtr == ')' || isEof(t)) {
        if(isEof(t)) {
          // [PARSE ERR] end of file was reached before the end of string
          logDiagnostic("Unexpected end of file", tCurr->bytePtr, startLine, startCol);
        }

        advancePtrToN(t, 1);

        return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
      }
      else {
        consumeReminantsOfBadUrl(t);

        return makeToken(TOKEN_BAD_URL, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, startLine, startCol);
      }
    }

    if(*charAtCurrPtr == '"' || *charAtCurrPtr == '\'' || *charAtCurrPtr == '(') {
      // [PARSE ERR] end of file was reached before the end of string
      logDiagnostic("Non printable code point", tCurr->bytePtr, startLine, startCol);

      consumeReminantsOfBadUrl(t);

      return makeToken(TOKEN_BAD_URL, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(*charAtCurrPtr == '\\') {
      if(isNCodePointValidEscape(t, 0)) {
        consumeEscapedCodePoint(t); // NOTE: no code point is returned from this function
      }
      else {
        // [PARSE ERR] end of file was reached before the end of string
        logDiagnostic("Invalid escape sequence", tCurr->bytePtr, startLine, startCol);

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
