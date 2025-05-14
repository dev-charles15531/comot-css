#include <stdbool.h>
#include "tokenizer_impl.h"

Token makeToken(TokenType type, TokenKind kind, const char *value, size_t length, size_t line, size_t column) {
  Token tok;

  tok.type = type;
  tok.kind = kind;
  tok.value = value;
  tok.length = length;
  tok.line = line;
  tok.column = column;
  
  return tok;
}

bool isNCodePointValidEscape(Tokenizer *t, size_t n) {
  const char *currCodePointPtr = peekPtrAtN(t, n);
  if(*currCodePointPtr != '\\')
    return false;

  const char *nextCodePointPtr = peekPtrAtN(t, n + 1);
  if(*nextCodePointPtr == '\n')
    return false;

  return true;
}

bool isWhitespace(const char *currCodePoint) {
  return (*currCodePoint == ' ' || *currCodePoint == '\t' || *currCodePoint == '\n' || *currCodePoint == '\r' || *currCodePoint == '\f'); 
}

bool isDigit(const char *currCodePoint) {
  // 0-9
  return (*currCodePoint >= 0 && *currCodePoint <= 9);
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

bool isIdentCodePoint(const char *currCodePoint) {
  return (isIdentStartCodePoint(currCodePoint) || isDigit(currCodePoint) || *currCodePoint == '-');
}

bool isNextThreeCodePointStartAnIdentSequence(Tokenizer *t) {
  const char *firstCodePoint = t->curr;
  const char *secondCodePoint = peekPtrAtN(t, 1);
  const char *thirdCodePoint = peekPtrAtN(t, 2);

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
  const char *firstCodePoint = t->curr;
  const char *secondCodePoint = peekPtrAtN(t, 1);
  const char *thirdCodePoint = peekPtrAtN(t, 2);

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
