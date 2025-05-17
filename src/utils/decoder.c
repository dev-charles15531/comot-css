#include "decoder.h"

void tryExtractCharset(const uint8_t *data, size_t len, char *charset) {
  const char *prefix = "@charset \"";
  size_t prefixLen = strlen(prefix);

  if(len < prefixLen + 1)
    return;

  const uint8_t *p = data;
  const uint8_t *end = data + len - prefixLen - 1;

  while((p = memchr(p, '@', end - p))) {
    if((size_t) (end - p) < prefixLen)
      break;

    if(memcmp(p, prefix, prefixLen) == 0) {
      const char *start = (const char *) (p + prefixLen);
      const uint8_t *limit = data + len;
      const char *endQuote = memchr(start, '"', limit - (const uint8_t *) start);

      if(endQuote && (endQuote - start) < 32) {
        size_t len = endQuote - start;
        memcpy(charset, start, len);
        charset[len] = '\0';

        return;
      }
    }

    ++ p;
  }
}

Encoding detectEncoding(const uint8_t *data, size_t len, char **declaredCharset) {
  *declaredCharset = NULL;

  if(len >= 2) {
    if(data[0] == 0xFF && data[1] == 0xFE)
      return ENCODING_UTF16LE;
    if(data[0] == 0xFE && data[1] == 0xFF)
      return ENCODING_UTF16BE;
  }

  if(len >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
    return ENCODING_UTF8;

  tryExtractCharset(data, len > 1024 ? 1024 : len, *declaredCharset);
  if(*declaredCharset && strcmp(*declaredCharset, "utf-8") == 0)
    return ENCODING_UTF8;

  return ENCODING_UTF8; // fallback
}

size_t normalizeCodePoints(DecodedStream *input, size_t len) {
  size_t write = 0;
  for(size_t read = 0; read < len; ++ read) {
    uint32_t cp = input[read].codePoint;
    if(cp == 0x0000)
      cp = REPLACEMENT_CHAR;
    else if(cp == 0x000D) {
      cp = 0x000A; // CR → LF

      if(read + 1 < len && input[read + 1].codePoint == 0x000A)
        read ++; // Skip LF in CRLF
    }
    else if(cp == 0x000C) {
      cp = 0x000A; // FF → LF
    }

    input[write ++].codePoint = cp;
  }

  return write;
}

int isValidUtf8Cont(uint8_t b) {
  return (b & 0xC0) == 0x80;
}

size_t decodeUtf8(const uint8_t *in, size_t len, DecodedStream *out, size_t cap) {
  size_t i = 0, o = 0;

  while(i < len && o < cap) {
    uint8_t b = in[i];

    out[o].bytePtr = (const char *) (in + i); 

    if(b < 0x80) {
      out[o++].codePoint = b;
      i++;
    }
    else if((b & 0xE0) == 0xC0 && i+1 < len && isValidUtf8Cont(in[i+1])) {
      uint32_t cp = ((b & 0x1F) << 6) | (in[i+1] & 0x3F);
      out[o++].codePoint = cp < 0x80 ? REPLACEMENT_CHAR : cp;
      i += 2;
    }
    else if((b & 0xF0) == 0xE0 && i+2 < len && isValidUtf8Cont(in[i+1]) && isValidUtf8Cont(in[i+2])) {
      uint32_t cp = ((b & 0x0F) << 12) | ((in[i+1] & 0x3F) << 6) | (in[i+2] & 0x3F);
      out[o++].codePoint = (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) ? REPLACEMENT_CHAR : cp;
      i += 3;
    }
    else if((b & 0xF8) == 0xF0 && i+3 < len && isValidUtf8Cont(in[i+1]) && isValidUtf8Cont(in[i+2]) && isValidUtf8Cont(in[i+3])) {
      uint32_t cp = ((b & 0x07) << 18) | ((in[i+1] & 0x3F) << 12) | ((in[i+2] & 0x3F) << 6) | (in[i+3] & 0x3F);
      out[o++].codePoint = cp <= 0x10FFFF ? cp : REPLACEMENT_CHAR;
      i += 4;
    }
    else {
      out[o++].codePoint = REPLACEMENT_CHAR;
      i++;
    }
  }

  return o;
}

size_t decodeUtf16(const uint8_t *in, size_t len, DecodedStream *out, size_t cap, int le) {
  size_t i = 0, o = 0;

  while(i + 1 < len && o < cap) {
    out[o].bytePtr = (const char *) (in + i); 

    uint16_t w1 = le ? (in[i] | (in[i+1] << 8)) : ((in[i] << 8) | in[i+1]);
    i += 2;

    if(w1 >= 0xD800 && w1 <= 0xDBFF && i + 1 < len) {
      uint16_t w2 = le ? (in[i] | (in[i+1] << 8)) : ((in[i] << 8) | in[i+1]);

      if(w2 >= 0xDC00 && w2 <= 0xDFFF) {
        out[o++].codePoint = 0x10000 + ((w1 - 0xD800) << 10) + (w2 - 0xDC00);
        i += 2;
      }
      else
        out[o++].codePoint = REPLACEMENT_CHAR;
    }
    else 
      out[o++].codePoint = w1;
  }

  return o;
}

size_t decodeCssInput(const uint8_t *raw, size_t len, DecodedStream *out, size_t cap) {
  char *declCharset = NULL;
  Encoding enc = detectEncoding(raw, len, &declCharset);

  // Skip BOM
  if(enc == ENCODING_UTF8 && len >= 3 && raw[0] == 0xEF && raw[1] == 0xBB && raw[2] == 0xBF) {
    raw += 3; len -= 3;
  }
  else if((enc == ENCODING_UTF16LE || enc == ENCODING_UTF16BE) && len >= 2) {
    raw += 2; len -= 2;
  }

  size_t count = 0;
  if(enc == ENCODING_UTF8) {
    count = decodeUtf8(raw, len, out, cap);
  }
  else if (enc == ENCODING_UTF16LE || enc == ENCODING_UTF16BE) {
    count = decodeUtf16(raw, len, out, cap, enc == ENCODING_UTF16LE);
  }

  return normalizeCodePoints(out, count);
}
