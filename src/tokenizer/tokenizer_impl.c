#include <stdio.h>
#include <stdbool.h>
#include "tokenizer_impl.h"
#include "comot-css/error.h"
#include "comot-css/diag.h"

/**
 * Creates a new token with the specified properties.
 *
 * This function initializes a Token structure with the provided type, kind,
 * value, length, line, and column information. It extracts the byte pointer
 * from the DecodedStream to set as the token's value.
 *
 * @param type The type of the token (e.g., IDENT, FUNCTION, etc.).
 * @param kind The kind of the token (valid or error).
 * @param value The decoded stream representing the token's value.
 * @param length The length of the token.
 * @param line The line number where the token starts.
 * @param column The column number where the token starts.
 * @return A Token structure initialized with the provided properties.
 */
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

/**
 * Emits an error token with the specified properties, and logs a diagnostic
 * message if the tokenizer is set to log errors.
 *
 * @param t The tokenizer instance.
 * @param message A human-readable diagnostic message describing the error.
 * @param value The decoded stream representing the token's value.
 * @param length The length of the token.
 * @param line The line number where the token starts.
 * @param column The column number where the token starts.
 * @return A Token structure initialized with the provided properties, with
 *         its kind set to TOKEN_KIND_ERROR.
 */
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

/**
 * Checks if a backslash followed by a single character is a valid escape sequence at the nth character of the current token.
 *
 * @param t The tokenizer instance.
 * @param n The position in the current token to check.
 *
 * @return true if the escape sequence is valid, false otherwise.
 */
bool isNCodePointValidEscape(Tokenizer *t, size_t n) {
  const DecodedStream *currCodePointPtr = peekPtrAtN(t, n);
  if(!currCodePointPtr || *currCodePointPtr->bytePtr != '\\')
    return false;

  const DecodedStream *nextCodePointPtr = peekPtrAtN(t, n + 1);
  if(!nextCodePointPtr || *nextCodePointPtr->bytePtr == '\n')
    return false;

  return true;
}

/**
 * Checks if a character is a whitespace character.
 *
 * @param currCodePoint The character to check.
 *
 * @return true if the character is a whitespace character, false otherwise.
 */
bool isWhitespace(const char *currCodePoint) {
  return (*currCodePoint == ' ' || *currCodePoint == '\t' || *currCodePoint == '\n' || *currCodePoint == '\r' || *currCodePoint == '\f'); 
}

/**
 * Checks if a character is a digit.
 *
 * @param currCodePoint The character to check.
 *
 * @return true if the character is a digit, false otherwise.
 */
bool isDigit(const char *currCodePoint) {
  // 0-9
  return (*currCodePoint >= '0' && *currCodePoint <= '9');
}

/**
 * Checks if a character is a letter.
 *
 * @param currCodePoint The character to check.
 *
 * @return true if the character is a letter, false otherwise.
 */
bool isLetter(const char *currCodePoint) {
  // A-Z, a-z
  return ((*currCodePoint >= 'A' && *currCodePoint <= 'Z') || (*currCodePoint >= 'a' && *currCodePoint <= 'z'));
}

/**
 * Checks if a character is a non-ASCII Unicode code point.
 *
 * @param currCodePoint The character to check.
 *
 * @return true if the character is a non-ASCII Unicode code point, false otherwise.
 */
bool isNonASCIICodePoint(const char *currCodePoint) {
  // Unicode code point greater than or equal to U+0080
  return ((unsigned char) *currCodePoint >= 0x80);
}

/**
 * Checks if a character is a valid identifier start code point.
 *
 * A valid identifier start code point is a letter, a non-ASCII Unicode code
 * point, or an underscore.
 *
 * @param currCodePoint The character to check.
 *
 * @return true if the character is a valid identifier start code point, false otherwise.
 */
bool isIdentStartCodePoint(const char *currCodePoint) {
  return (isLetter(currCodePoint) || isNonASCIICodePoint(currCodePoint) || (*currCodePoint == '_'));
}

/**
 * Checks if a character is a valid identifier code point.
 *
 * A valid identifier code point is a valid identifier start code point,
 * a digit, or a hyphen.
 *
 * @param currStream The decoded stream representing the character to check.
 *
 * @return true if the character is a valid identifier code point, false otherwise.
 */
bool isIdentCodePoint(const DecodedStream *currStream) {
  return (isIdentStartCodePoint(currStream->bytePtr) || isDigit(currStream->bytePtr) || *currStream->bytePtr == '-');
}

/**
 * Checks if the next three code points in the input stream could possibly be
 * the start of an identifier sequence.
 *
 * @param t The tokenizer instance.
 *
 * @return true if the next three code points could be the start of an
 *         identifier sequence, false otherwise.
 */
bool isNextThreeCodePointStartAnIdentSequence(Tokenizer *t) {
  const char *firstCodePoint = t->curr->bytePtr;
  const DecodedStream *secondCodePoint = peekPtrAtN(t, 1);
  const DecodedStream *thirdCodePoint = peekPtrAtN(t, 2);

  if(*firstCodePoint == '-') {
    if(isIdentStartCodePoint(secondCodePoint->bytePtr)) {
      return true;
    }
    if(secondCodePoint && *secondCodePoint->bytePtr == '-' && thirdCodePoint && isIdentStartCodePoint(thirdCodePoint->bytePtr)) {
      return true;
    }
    if(secondCodePoint && *secondCodePoint->bytePtr == '\\' && isNCodePointValidEscape(t, 1)) {
      return true;
    }
  }

  if(isIdentStartCodePoint(firstCodePoint))
    return true;

  if(*firstCodePoint == '\\' && isNCodePointValidEscape(t, 0))
    return true;

  return false;
}

/**
 * Checks if the next three code points in the input stream could possibly
 * be the start of a number sequence.
 *
 * This function examines the current position and the next two positions
 * in the tokenizer's input stream to determine if they represent the 
 * start of a numeric sequence. A numeric sequence can start with an
 * optional '+' or '-' sign, followed by a digit or a dot followed by a digit.
 *
 * @param t The tokenizer instance.
 *
 * @return true if the next three code points could be the start of a 
 *         number sequence, false otherwise.
 */
bool isNextThreeCodePointStartNumber(Tokenizer *t) {
  const char *firstCodePoint = t->curr->bytePtr;
  const DecodedStream *secondCodePoint = peekPtrAtN(t, 1);
  const DecodedStream *thirdCodePoint = peekPtrAtN(t, 2);

  if(*firstCodePoint == '+' || *firstCodePoint == '-') {
    if(secondCodePoint && isDigit(secondCodePoint->bytePtr))
      return true;
    else if(secondCodePoint && *secondCodePoint->bytePtr == '.' && thirdCodePoint && isDigit(thirdCodePoint->bytePtr))
      return true;
    else
      return false;
  }

  if(*firstCodePoint == '.') {
    if(secondCodePoint && isDigit(secondCodePoint->bytePtr))
      return true;
    else 
      return false;
  }

  if(isDigit(firstCodePoint))
    return true;

  return false;
}
