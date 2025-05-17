#include <stdint.h>
#include <stddef.h>
#include "tokenizer_impl.h"
#include "comot-css/diag.h"

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
void consumeEscapedCodePoint(Tokenizer *t) {
  uint32_t value = 0;
  size_t digits = 0;

  // Read up to 6 hex digits
  while(!isEof(t) && isHex(t->curr->bytePtr) && digits < 6) {
    value = (value << 4) | hexValue(t->curr->bytePtr);
    advancePtrToN(t, 1);
    digits++;
  }

  // Optional single whitespace after escape sequence
  if (!isEof(t) && isWhitespace(t->curr->bytePtr)) {
    advancePtrToN(t, 1);
  }

  // Validate the code point
  if (value == 0 || value > MAX_CODE_POINT || (value >= 0xD800 && value <= 0xDFFF)) {
    value = REPLACEMENT_CHAR;

    // TODO: replace in tokenizer input with REPLACEMENT_CHAR
    // NOTE: this char must have been replaced before reaching thsis point
  }

  if(isEof(t)) {
    // [PARSE ERR]  unclosed string
    // return bad string
    // TODO: the replace thing needs to be done here as well.
    logDiagnostic("Unexpected end of file", t->curr->bytePtr, t->line, t->column);
  }

  return;
}
