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

/**
 * @brief Checks if the tokenizer has reached the end of the input.
 * 
 * This function determines whether the current position of the tokenizer
 * is at or beyond the end of the input stream, indicating that there
 * are no more tokens to process.
 * 
 * @param t Pointer to the Tokenizer instance.
 * @return true if the tokenizer has reached the end of the input, false otherwise.
 */
static inline bool isEof(Tokenizer *t) {
  return t->curr >= t->end;
}

/**
 * @brief Looks ahead in the input stream by `n` positions.
 *
 * This function is used to peek at the character at a given position in the
 * input stream without advancing the tokenizer's current position. It is
 * used to determine whether a given token is valid or not.
 *
 * @param t   Pointer to the Tokenizer instance.
 * @param n   The number of positions to look ahead.
 *
 * @return A pointer to the character at the desired position in the input
 *         stream if it exists, or `NULL` if the desired position is beyond
 *         the end of the stream.
 */
static inline const DecodedStream *peekPtrAtN(Tokenizer *t, size_t n) {
  // Check if the desired position is beyond the end of the stream
  if (t->curr + n >= t->end)
    return NULL;

  // Return the pointer at the desired position
  return t->curr + n;
}

/**
 * @brief Advances the tokenizer's current position by `n` positions.
 *
 * This function is used to advance the tokenizer's current position in the
 * input stream by a given number of positions. It is used to skip over
 * characters in the input stream that are not relevant to the current
 * token being processed.
 *
 * As the tokenizer advances, it keeps track of the line and column numbers
 * of the current position in the input stream. If a newline character is
 * encountered during the advancement, the line number is incremented and
 * the column number is reset to 1. Otherwise, the column number is simply
 * incremented by 1.
 *
 * If the desired position is beyond the end of the stream, this function
 * returns `NULL`.
 *
 * @param t   Pointer to the Tokenizer instance.
 * @param n   The number of positions to advance in the input stream.
 *
 * @return A pointer to the character at the desired position in the input
 *         stream if it exists, or `NULL` if the desired position is beyond
 *         the end of the stream.
 */
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

/**
 * @brief Moves the tokenizer's position back by one.
 *
 * This function returns a pointer to the character immediately before the
 * current position in the tokenizer's input stream. If the current position
 * is at the start of the input stream, it returns `NULL` to indicate that
 * no further lookback is possible.
 *
 * @param t Pointer to the Tokenizer instance.
 * @return A pointer to the character before the current position, or `NULL`
 *         if the current position is at the start of the input stream.
 */
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
