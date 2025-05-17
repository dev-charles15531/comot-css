#include <stdio.h>
#include <stdbool.h>
#include "tokenizer_impl.h"
#include "comot-css/error.h"
#include "comot-css/diag.h"

Token makeToken(TokenType type, TokenKind kind, const DecodedStream *value, size_t length, size_t line, size_t column) {
  Token tok;

  tok.type = type;
  tok.kind = kind;
  tok.value = value->bytePtr;
  tok.length = length;
  tok.line = line;
  tok.column = column;

  return tok;
}

Token emitErrorToken(Tokenizer *t, const char* message, const DecodedStream *value, size_t length, size_t line, size_t column) {
  if(t->shouldLog) {
    logDiagnostic(message, value->bytePtr, line, column);
    t->errorCount ++;

    if(t->errorCount >= t->maxErrors) {
      t->shouldLog = 0;
      fprintf(stderr, "[diagnostic] Logging disabled after %ld errors\n", t->errorCount);
    }
  }

  return makeToken(TOKEN_ERROR, TOKEN_KIND_ERROR, value, length, line, column);
}

bool isNCodePointValidEscape(Tokenizer *t, size_t n) {
  const char *currCodePointPtr = peekPtrAtN(t, n)->bytePtr;
  if(currCodePointPtr && *currCodePointPtr != '\\')
    return false;

  const char *nextCodePointPtr = peekPtrAtN(t, n + 1)->bytePtr;
  if(nextCodePointPtr && *nextCodePointPtr == '\n')
    return false;

  return true;
}

bool isWhitespace(const char *currCodePoint) {
  return (*currCodePoint == ' ' || *currCodePoint == '\t' || *currCodePoint == '\n' || *currCodePoint == '\r' || *currCodePoint == '\f'); 
}

bool isDigit(const char *currCodePoint) {
  // 0-9
  return (*currCodePoint >= '0' && *currCodePoint <= '9');
}

bool isLetter(const char *currCodePoint) {
  // A-Z, a-z
  return ((*currCodePoint >= 'A' && *currCodePoint <= 'Z') || (*currCodePoint >= 'a' && *currCodePoint <= 'z'));
}

bool isNonASCIICodePoint(const char *currCodePoint) {
  // Unicode code point greater than or equal to U+0080
  return ((unsigned char) *currCodePoint >= 0x80);
}

bool isIdentStartCodePoint(const char *currCodePoint) {
  return (isLetter(currCodePoint) || isNonASCIICodePoint(currCodePoint) || (*currCodePoint == '_'));
}

bool isIdentCodePoint(const DecodedStream *currStream) {
  return (isIdentStartCodePoint(currStream->bytePtr) || isDigit(currStream->bytePtr) || *currStream->bytePtr == '-');
}

bool isNextThreeCodePointStartAnIdentSequence(Tokenizer *t) {
  const char *firstCodePoint = t->curr->bytePtr;
  const char *secondCodePoint = peekPtrAtN(t, 1)->bytePtr;
  const char *thirdCodePoint = peekPtrAtN(t, 2)->bytePtr;

  if(*firstCodePoint == '-') {
    if(isIdentStartCodePoint(secondCodePoint)) {
      return true;
    }
    if(*secondCodePoint == '-' && isIdentStartCodePoint(thirdCodePoint)) {
      return true;
    }
    if(*secondCodePoint == '\\' && isNCodePointValidEscape(t, 1)) {
      return true;
    }
  }

  if(isIdentStartCodePoint(firstCodePoint))
    return true;

  if(*firstCodePoint == '\\' && isNCodePointValidEscape(t, 0))
    return true;

  return false;
}

bool isNextThreeCodePointStartNumber(Tokenizer *t) {
  const char *firstCodePoint = t->curr->bytePtr;
  const char *secondCodePoint = peekPtrAtN(t, 1)->bytePtr;
  const char *thirdCodePoint = peekPtrAtN(t, 2)->bytePtr;

  if(*firstCodePoint == '+' || *firstCodePoint == '-') {
    if(isDigit(secondCodePoint))
      return true;
    else if(*secondCodePoint == '.' && isDigit(thirdCodePoint))
      return true;
    else
      return false;
  }

  if(*firstCodePoint == '.') {
    if(isDigit(secondCodePoint))
      return true;
    else 
      return false;
  }

  if(isDigit(firstCodePoint))
    return true;

  return false;
}
