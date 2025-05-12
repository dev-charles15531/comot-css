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

// Shared tokenizer helpers
static inline bool isEof(Tokenizer *t) {
  return t->curr >= t->end;
}

static inline const char *peekPtrAtN(Tokenizer *t, size_t n) {
  return (t->curr + n) >= t->end ? NULL : t->curr + n;
}

static inline const char *advancePtrToN(Tokenizer *t, size_t n) {
  const char *cPtr = ((t->curr + n) < t->end) ? (t->curr + n) : NULL;

  if(*cPtr == '\n') {
    t->line ++;
    t->column = 1;
  }
  else {
    t->column += n;
  }

  t->curr += n;

  return t->curr;
}

Token make_token(TokenType type, TokenKind kind, const char *value, size_t length, size_t line, size_t column);

Token consumeCommentOrDelim(Tokenizer *t, char codePoint);

#endif
