#ifndef TOKENS_H
#define TOKENS_H

#include <stddef.h>

// Token types
typedef enum {
  TOKEN_IDENT,
  TOKEN_FUNCTION,
  TOKEN_AT_KEYWORD,
  TOKEN_HASH,
  TOKEN_STRING,
  TOKEN_BAD_STRING,
  TOKEN_URL,
  TOKEN_BAD_URL,
  TOKEN_DELIM,
  TOKEN_NUMBER,
  TOKEN_PERCENTAGE,
  TOKEN_DIMENSION,
  TOKEN_WHITESPACE,
  TOKEN_COMMENT,
  TOKEN_COLON,
  TOKEN_SEMICOLON,
  TOKEN_COMMA,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_SQUARE,
  TOKEN_RIGHT_SQUARE,
  TOKEN_LEFT_CURLY,
  TOKEN_RIGHT_CURLY,
  TOKEN_CDO,
  TOKEN_CDC,
  TOKEN_EOF,
  TOKEN_ERROR
} TokenType;

// Token kind (valid or error)
typedef enum {
  TOKEN_KIND_VALID,
  TOKEN_KIND_ERROR
} TokenKind;

// Token structure
typedef struct {
  TokenType type;
  TokenKind kind;
  const char* value;    // pointer to token start in input
  size_t length;        // length of the token
  size_t line;          // line number where token starts
  size_t column;        // column number where token starts
} Token;

#endif  // !TOKENS_H
