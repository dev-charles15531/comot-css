#ifndef TOKENIZER_IMPL_H
#define TOKENIZER_IMPL_H

#include <stddef.h>
#include <stdbool.h>
#include "comot-css/tokenizer.h"
#include "comot-css/tokens.h"
#include "arena_alloc.h"

// Tokenizer FSM state
typedef enum {
  DATA_STATE,
  STRING_STATE,
  BAD_STRING_STATE,
  ESCAPE_STATE
} TokenizerState;

// Tokenizer state structure
typedef struct Tokenizer {
  const char *start;
  const char *curr;
  const char *end;
  size_t line;
  size_t column;
  TokenizerState state;
  char stringQuote;
  Arena *arena;
} Tokenizer;

// Decode one UTF-8 code point from `ptr`
// Returns the code point, sets `*out_len` to the byte length
// Returns 0xFFFD (replacement char) on invalid sequences
static inline uint32_t decodeUtf8(const char *ptr, int *out_len) {
  const unsigned char *bytes = (const unsigned char *)ptr;
  if(bytes[0] < 0x80) {
    *out_len = 1;

    return bytes[0];
  }
  else if((bytes[0] & 0xE0) == 0xC0 && (bytes[1] & 0xC0) == 0x80) {
    *out_len = 2;

    return ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
  }
  else if((bytes[0] & 0xF0) == 0xE0 && (bytes[1] & 0xC0) == 0x80 && (bytes[2] & 0xC0) == 0x80) {
    *out_len = 3;

    return ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
  }
  else if ((bytes[0] & 0xF8) == 0xF0 && (bytes[1] & 0xC0) == 0x80 && (bytes[2] & 0xC0) == 0x80 && (bytes[3] & 0xC0) == 0x80) {
    *out_len = 4;

    return ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
  }
  else {
    *out_len = 1;

    return 0xFFFD;  // Replacement character
  }
}

// Shared tokenizer helpers
static inline bool isEof(Tokenizer *t) {
  return t->curr >= t->end;
}

static inline const char *peekPtrAtN(Tokenizer *t, size_t n) {
  const char *ptr = t->curr;
  
  for(size_t i = 0; i < n; i ++) {
    if(ptr >= t->end)
      return NULL;

    int len;
    decodeUtf8(ptr, &len);

    if((ptr + len) >= t->end)
      return NULL;

    ptr += len;
  }

  return ptr >= t->end ? NULL : ptr;
}

static inline const char *advancePtrToN(Tokenizer *t, size_t n) {
  const char *nextPtr = peekPtrAtN(t, n) ;

  if(nextPtr == NULL) {
    t->curr = t->end;
  }
  else {
    if(*nextPtr == '\n') {
      t->line ++;
      t->column = 1;
    }
    else {
      t->column += n;
    }

    t->curr = nextPtr;
  }

  return t->curr;
}

static inline const char *ptrLookback(Tokenizer *t) {
  if(t->curr <= t->start)
    return NULL;

  const char *ptr = t->curr - 1;
  while(ptr >= t->start && ((*ptr & 0xC0) == 0x80)) {
    ptr --;
  }

  int len;
  decodeUtf8(ptr, &len);
  if(ptr + len == t->curr)
    return ptr;
  else 
    return NULL;
}

Token makeToken(TokenType type, TokenKind kind, const char *value, size_t length, size_t line, size_t column);

bool isNCodePointValidEscape(Tokenizer *t, size_t n);

bool isWhitespace(const char *currCodePoint);

bool isDigit(const char *currCodePoint);

bool isLetter(const char *currCodePoint);

bool isNonASCIICodePoint(const char *currCodePoint);

bool isIdentStartCodePoint(const char *currCodePoint);

bool isIdentCodePoint(const char *currCodePoint);

Token consumeIdentLikeToken(Tokenizer *t);

Token consumeCommentOrDelim(Tokenizer *t, char codePoint);

Token consumeString(Tokenizer *t, char codePoint);

void consumeEscapedCodePoint(Tokenizer *t, char codePoint);

const char *reconsumeCurrInputCodePoint(Tokenizer *t);

const char *consumeIdentSequence(Tokenizer *t);

bool isNextThreeCodePointStartAnIdentSequence(Tokenizer *t);

bool isNextThreeCodePointStartNumber(Tokenizer *t);

void consumeNumber(Tokenizer *t);

Token consumeNumericToken(Tokenizer *t);

Token consumeUrlToken(Tokenizer *t);


#endif
