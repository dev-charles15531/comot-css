#include "tokenizer_impl.h"

Token make_token(TokenType type, TokenKind kind, const char *value, size_t length, size_t line, size_t column) {
  Token tok;

  tok.type = type;
  tok.kind = kind;
  tok.value = value;
  tok.length = length;
  tok.line = line;
  tok.column = column;
  
  return tok;
}

