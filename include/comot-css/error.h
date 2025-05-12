#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

// Lightweight error code enum
typedef enum {
  LEXER_OK = 0,
  LEXER_ERR_UNTERMINATED_STRING,
  LEXER_ERR_INVALID_ESCAPE,
  LEXER_ERR_UNEXPECTED_CHAR,
  LEXER_ERR_UNTERMINATED_COMMENT,
  LEXER_ERR_UNKNOWN_TOKEN,
  LEXER_ERR_BAD_NUMBER
} LexerErrorCode;

// Error structure
typedef struct {
  LexerErrorCode code;
  size_t line;
  size_t column;
} LexerError;

const char* lexer_error_message(LexerErrorCode code);

#endif
