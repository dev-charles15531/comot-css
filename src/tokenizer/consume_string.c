#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/diag.h"


Token consumeString(Tokenizer *t, char endingCodePoint) {
  const DecodedStream *startStream = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  advancePtrToN(t, 1); // consume opening quote

  while(!isEof(t)) {
    const char *ptr = t->curr->bytePtr;

    if(*ptr == endingCodePoint) {
      advancePtrToN(t, 1); // consume closing quote
      return makeToken(TOKEN_STRING, TOKEN_KIND_VALID, startStream, t->curr - startStream, startLine, startCol);
    }

    if(*ptr == '\n') {
      logDiagnostic("Unclosed string literal", startStream->bytePtr, startLine, startCol);
      return makeToken(TOKEN_BAD_STRING, TOKEN_KIND_ERROR, startStream, t->curr - startStream, startLine, startCol);
    }

    if(*ptr == '\\') {
      const char *nxt = peekPtrAtN(t, 1)->bytePtr;

      // If next is EOF — spec says do nothing and fall through
      if(*nxt == '\0') {
        break;
      }

      if(*nxt == '\n') {
        advancePtrToN(t, 2); // skip both backslash and newline
        continue;
      }

      // Valid escape?
      if(isNCodePointValidEscape(t, 0)) {
        advancePtrToN(t, 1); // consume '\'
        consumeEscapedCodePoint(t); // modifies t->curr
        continue;
      }

      // Invalid escape
      logDiagnostic("Invalid escape sequence in string", t->curr->bytePtr, t->line, t->column);
      advancePtrToN(t, 1); // skip '\'
      continue;
    }

    // Anything else — consume it
    advancePtrToN(t, 1);
  }

  // EOF before closing quote
  logDiagnostic("Unexpected end of file in string", startStream->bytePtr, startLine, startCol);
  return makeToken(TOKEN_STRING, TOKEN_KIND_ERROR, startStream, t->curr - startStream, startLine, startCol);
}
