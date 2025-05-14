#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

Token consumeString(Tokenizer *t, char codePoint) {
  const char *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  while(!isEof(t)) {
    // advancePtrToN(t, 1);
    const char *c = peekPtrAtN(t, 1);

    // if this is the end of the string
    if(*c == codePoint) {
      advancePtrToN(t, 2);

      return makeToken(TOKEN_STRING, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    // if this is a new line
    if(*c == '\n') {
      advancePtrToN(t, 1);

      // return bad string
      return makeToken(TOKEN_STRING, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, startLine, startCol);
    }

    // if this is a reverse solidus(\)
    if(*c == '\\') {
      const char *cNext = peekPtrAtN(t, 1);

      if(*cNext == '\n')
        advancePtrToN(t, 1);
      else if(isNCodePointValidEscape(t, 1)) {
        consumeEscapedCodePoint(t, *c);  
      }
    }

    // Anything else? consume!
    advancePtrToN(t, 1);
  }

  // end of file was reached before the end of string TODO: [PARSE ERR]
  return makeToken(TOKEN_STRING, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, t->line, t->column);
}
