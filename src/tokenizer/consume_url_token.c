#include <string.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/diag.h"

static void consumeReminantsOfBadUrl(Tokenizer *t) {
  while(advancePtrToN(t, 1)) {
    if(isEof(t))
      return;

    if(!t->curr || !t->curr->bytePtr)
      return;

    if(*t->curr->bytePtr == ')')
      return;

    if(isNCodePointValidEscape(t, 0))
      consumeEscapedCodePoint(t);
  }
}

Token consumeUrlToken(Tokenizer *t) {
  const DecodedStream *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  while(isWhitespace(t->curr->bytePtr)) {
    if(!advancePtrToN(t, 1))
      break;
  }

  while(t->curr && t->curr < t->end) {
    if(!t->curr->bytePtr) {
      logDiagnostic("Null bytePtr in tokenizer stream", t->curr ? t->curr->bytePtr : NULL, t->line, t->column);
      
      return makeToken(TOKEN_BAD_URL, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, startLine, startCol);
    }

    const char *charAtCurrPtr = t->curr->bytePtr;

    if(*charAtCurrPtr == ')')
      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);

    if(isEof(t)) {
      logDiagnostic("Unexpected end of file", tCurr->bytePtr, startLine, startCol);

      return makeToken(TOKEN_URL, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(isWhitespace(charAtCurrPtr)) {
      while(t->curr && t->curr->bytePtr && isWhitespace(t->curr->bytePtr)) {
        if(!advancePtrToN(t, 1))
          break;
      }

      if(!t->curr || !t->curr->bytePtr || *t->curr->bytePtr == ')' || isEof(t)) {
        if(isEof(t)) {
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
      logDiagnostic("Non printable code point", tCurr->bytePtr, startLine, startCol);
      consumeReminantsOfBadUrl(t);
      
      return makeToken(TOKEN_BAD_URL, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, startLine, startCol);
    }

    if(*charAtCurrPtr == '\\') {
      if(isNCodePointValidEscape(t, 0)) {
        consumeEscapedCodePoint(t);
      } 
      else {
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
