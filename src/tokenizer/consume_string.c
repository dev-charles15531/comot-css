#include <stdio.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/diag.h"

/**
 * Consumes a string literal token from the input stream, including the
 * required opening and closing quotes and any escaped code points.
 *
 * If the value is invalid (e.g. unclosed string, invalid escape sequence),
 * an error token will be returned.
 *
 * @param t  The tokenizer
 * @param endingCodePoint  The character to match for the closing quote
 * @return  A token representing the string literal
 */
Token consumeString(Tokenizer *t, char endingCodePoint) {
  const DecodedStream *startStream = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  advancePtrToN(t, 1); // consume opening quote

  while(!isEof(t)) {
    const char *ptr = t->curr->bytePtr;

    if(*ptr == '\n') {
      logDiagnostic("Unclosed string literal", startStream->bytePtr, startLine, startCol);
      return makeToken(TOKEN_BAD_STRING, TOKEN_KIND_ERROR, startStream, t->curr - startStream, startLine, startCol);
    }

    if(*ptr == '\\') {
      const char *nxt = peekPtrAtN(t, 1)->bytePtr;
      // printf("\nChar after:: %c\n\n", *nxt);

      // If next is EOF — spec says do nothing and fall through
      if(*nxt == '\0') {
        break;
      }

      if(*nxt == '\n') {
        advancePtrToN(t, 2); // skip both backslash and newline
        continue;
      }

      // Valid escape?
      if(isNCodePointValidEscape(t, 1)) {
        advancePtrToN(t, 2); // consume '\'
        consumeEscapedCodePoint(t); // modifies t->curr
        continue;
      }

      // Invalid escape?
      // TODO: is this an invalid escape?
      // logDiagnostic("Invalid escape sequence in string", t->curr->bytePtr, t->line, t->column);
      advancePtrToN(t, 2); // consume '\' and wtv follows 
      continue;
    }

    if(*ptr == endingCodePoint) {
      advancePtrToN(t, 1); // consume closing quote
      return makeToken(TOKEN_STRING, TOKEN_KIND_VALID, startStream, t->curr - startStream, startLine, startCol);
    }

    // Anything else — consume it
    advancePtrToN(t, 1);
  }

  // EOF before closing quote
  logDiagnostic("Unexpected end of file in string", startStream->bytePtr, startLine, startCol);
  return makeToken(TOKEN_STRING, TOKEN_KIND_ERROR, startStream, t->curr - startStream, startLine, startCol);
}
