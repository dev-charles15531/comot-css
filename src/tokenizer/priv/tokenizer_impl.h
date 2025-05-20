#ifndef TOKENIZER_IMPL_H
#define TOKENIZER_IMPL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "comot-css/tokenizer.h"
#include "comot-css/tokens.h"
#include "arena_alloc.h"
#include "decoder.h"

// Tokenizer FSM state
typedef enum {
  DATA_STATE,
  IDENTIFIER_STATE,
  STRING_STATE,
  NUMBER_STATE,
  WHITESPACE_STATE,
  DELIM_STATE
} TokenizerState;

// Tokenizer state structure
typedef struct Tokenizer {
  const DecodedStream *start;
  const DecodedStream *curr;
  const DecodedStream *end;
  size_t line;
  size_t column;
  TokenizerState state;
  size_t shouldLog;     // Current logging status (1 = on, 0 = off)
  size_t errorCount;    // Total number of errors seen
  size_t maxErrors;     // Disable logging after this many errors
  char stringQuote;
  Arena *arena;
} Tokenizer;

// Shared tokenizer helpers
static inline bool isEof(Tokenizer *t) {
  return t->curr >= t->end;
}

static inline const DecodedStream *peekPtrAtN(Tokenizer *t, size_t n) {
  if (t->curr + n >= t->end)
    return NULL;

  return t->curr + n;
}

static inline const DecodedStream *advancePtrToN(Tokenizer *t, size_t n) {
  for(size_t i = 0; i < n && t->curr < t->end; i++) {
    if(t->curr->bytePtr && *t->curr->bytePtr == '\n') {
      t->line++;
      t->column = 1;
    }
    else {
      t->column++;
    }

    t->curr++;
  }

  return t->curr < t->end ? t->curr : NULL;
}

static inline const DecodedStream *ptrLookback(Tokenizer *t) {
  if(t->curr <= t->start)
    return NULL;

  return t->curr - 1;
}

Token makeToken(TokenType type, TokenKind kind, const DecodedStream *value, size_t length, size_t line, size_t column);

bool isNCodePointValidEscape(Tokenizer *t, size_t n);

bool isWhitespace(const char *currCodePoint);

bool isDigit(const char *currCodePoint);

bool isLetter(const char *currCodePoint);

bool isNonASCIICodePoint(const char *currCodePoint);

bool isIdentStartCodePoint(const char *currCodePoint);

bool isIdentCodePoint(const DecodedStream *currStream);

Token consumeIdentLikeToken(Tokenizer *t);

Token consumeCommentOrDelim(Tokenizer *t, char codePoint);

Token consumeString(Tokenizer *t, char codePoint);

void consumeEscapedCodePoint(Tokenizer *t);

const DecodedStream *reconsumeCurrInputCodePoint(Tokenizer *t);

const DecodedStream *consumeIdentSequence(Tokenizer *t);

bool isNextThreeCodePointStartAnIdentSequence(Tokenizer *t);

bool isNextThreeCodePointStartNumber(Tokenizer *t);

void consumeNumber(Tokenizer *t);

Token consumeNumericToken(Tokenizer *t);

Token consumeUrlToken(Tokenizer *t);


#endif
