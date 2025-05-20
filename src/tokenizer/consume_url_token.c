#include <string.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/diag.h"

/**
 * Consumes any remaining characters from a bad url token, including
 * whitespace, escaped code points, and the closing ')'.
 *
 * This function will advance the tokenizer until it has consumed all
 * characters up to and including the closing ')', or until it has
 * reached the end of the input.
 *
 * @param t  The tokenizer
 */
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

/**
 * Consumes a URL token from the input stream.  This token is
 * special because it can contain escaped characters, whitespace,
 * and other special characters.  This function will consume all
 * characters up to and including the closing ')', or until it
 * has reached the end of the input.
 *
 * If the URL token is invalid (e.g. no closing ')', invalid escape
 * sequence), an error token will be returned.
 *
 * @param t  The tokenizer
 * @return  A token representing the URL
 */
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
