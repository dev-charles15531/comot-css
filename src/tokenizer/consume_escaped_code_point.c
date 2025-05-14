#include <stdint.h>
#include <stddef.h>
#include "tokenizer_impl.h"
// #include "comot-css/tokenizer.h"

#define REPLACEMENT_CHAR 0xFFFD
#define MAX_CODE_POINT   0x10FFFF

static inline int isHex(const char *c) {
  return (*c >= '0' && *c <= '9') || (*c >= 'a' && *c <= 'f') || (*c >= 'A' && *c <= 'F');
}

static inline int hexValue(const char *c) {
  if (*c >= '0' && *c <= '9')
    return *c - '0';

  if (*c >= 'a' && *c <= 'f')
    return 10 + (*c - 'a');

  if (*c >= 'A' && *c <= 'F')
    return 10 + (*c - 'A');

  return -1;
}


// Consumes an escaped code point starting from the reverse solidus(\).
// Assumes '\' has already been consumed and a valid escape follows.
// Updates *pos to new offset. Input must be null-terminated.
void consumeEscapedCodePoint(Tokenizer *t, char codePoint) {
  // const char *tCurr = t->curr;  // Start of token content
  // size_t startLine = t->line;
  // size_t startCol = t->col;

  uint32_t value = 0;
  size_t digits = 1;

  value = hexValue(&codePoint);
  advancePtrToN(t, 1);

  // Consume up to 5 more hex digits (total max 6)
  while (digits < 6 && isHex(peekPtrAtN(t, 1))) {
    value = (value << 4) | hexValue(advancePtrToN(t, 1));
    digits++;
  }

  // Consume whitespace after hex digits
  if (isWhitespace(peekPtrAtN(t, 1))) {
    advancePtrToN(t, 1);
  }

  // Validate the code point
  if (value == 0 || value > MAX_CODE_POINT || (value >= 0xD800 && value <= 0xDFFF)) {
    value = REPLACEMENT_CHAR;
  }

  // The string content will be pulled from the original source (tCurr to t->curr)
  // return makeToken(TOKEN_STRING, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  return;
}
