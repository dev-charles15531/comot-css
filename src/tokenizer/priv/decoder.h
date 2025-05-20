#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define REPLACEMENT_CHAR 0xFFFD
#define MAX_INPUT_LEN 1048576

typedef enum {
  ENCODING_UTF8,
  ENCODING_UTF16LE,
  ENCODING_UTF16BE,
  ENCODING_UNKNOWN
} Encoding;

typedef struct {
  uint32_t codePoint;
  const char *bytePtr;
} DecodedStream;

size_t decodeCssInput(const uint8_t *raw, size_t len, DecodedStream *out, size_t cap);

size_t normalizeCodePoints(DecodedStream *input, size_t len);

Encoding detectEncoding(const uint8_t *data, size_t len, char *declaredCharset);

int isValidUtf8Cont(uint8_t b);

size_t decodeUtf8(const uint8_t *in, size_t len, DecodedStream *out, size_t cap);

size_t decodeUtf16(const uint8_t *in, size_t len, DecodedStream *out, size_t cap, int le);

void tryExtractCharset(const uint8_t *data, size_t len, char *charset);

#endif
