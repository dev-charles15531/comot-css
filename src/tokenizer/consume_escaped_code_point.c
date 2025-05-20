#include <stdint.h>
#include <stddef.h>
#include "tokenizer_impl.h"
#include "comot-css/diag.h"

#define REPLACEMENT_CHAR 0xFFFD
#define MAX_CODE_POINT   0x10FFFF

// Returns 1 if the given character c is a valid hexadecimal digit, 0 otherwise.
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

/**
 * Consumes an escaped code point from the input stream, including the
 * required backslash and up to 6 hex digits, and optional whitespace.
 *
 * If the value is invalid (i.e. 0, > MAX_CODE_POINT, or surrogate pair),
 * the value is replaced with the REPLACEMENT_CHAR and a diagnostic is
 * logged.
 *
 * @param t  The tokenizer
 */
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
