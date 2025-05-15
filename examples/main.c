#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "comot-css/tokenizer.h"

const char* tokenTypeToString(TokenType type) {
  switch (type) {
    case TOKEN_IDENT: return "TOKEN_IDENT";
    case TOKEN_FUNCTION: return "TOKEN_FUNCTION";
    case TOKEN_AT_KEYWORD: return "TOKEN_AT_KEYWORD";
    case TOKEN_HASH: return "TOKEN_HASH";
    case TOKEN_STRING: return "TOKEN_STRING";
    case TOKEN_BAD_STRING: return "TOKEN_BAD_STRING";
    case TOKEN_URL: return "TOKEN_URL";
    case TOKEN_BAD_URL: return "TOKEN_BAD_URL";
    case TOKEN_DELIM: return "TOKEN_DELIM";
    case TOKEN_NUMBER: return "TOKEN_NUMBER";
    case TOKEN_PERCENTAGE: return "TOKEN_PERCENTAGE";
    case TOKEN_DIMENSION: return "TOKEN_DIMENSION";
    case TOKEN_WHITESPACE: return "TOKEN_WHITESPACE";
    case TOKEN_COMMENT: return "TOKEN_COMMENT";
    case TOKEN_COLON: return "TOKEN_COLON";
    case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
    case TOKEN_COMMA: return "TOKEN_COMMA";
    case TOKEN_LEFT_PAREN: return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN: return "TOKEN_RIGHT_PAREN";
    case TOKEN_LEFT_SQUARE: return "TOKEN_LEFT_SQUARE";
    case TOKEN_RIGHT_SQUARE: return "TOKEN_RIGHT_SQUARE";
    case TOKEN_LEFT_CURLY: return "TOKEN_LEFT_CURLY";
    case TOKEN_RIGHT_CURLY: return "TOKEN_RIGHT_CURLY";
    case TOKEN_CDO: return "TOKEN_CDO";
    case TOKEN_CDC: return "TOKEN_CDC";
    case TOKEN_EOF: return "TOKEN_EOF";
    case TOKEN_ERROR: return "TOKEN_ERROR";
    default: return "UNKNOWN_TOKEN";
  }
}

int main(void) {
  Arena arena = arena_create(1024 * 1024);  // 1mb

  // const char *css = "body { color: red; /* comment anything i want here */\ncontent: \"Hello world\" }";
  const char *css =
  "/* comment */\n"
  ".a {\n"
  "  content: \"simple string\";\n"
  "  content: \"escaped quote: \\\"\";\n"
  "  content: \"unicode: \\263A\";\n" // ☺
  "  content: 'single quoted';\n"
  "  /* multiple */ /* comments \\*/\n"
  "}\n"
  "#myId {\n"
  "  color: blue;\n"
  "}\n"
  ".funcTest {\n"
  "  background-image: url(\"image(path).png\");\n" // parentheses in URL
  "  transform: rotate(45deg);\n" // parentheses in function
  "  margin: 10px 5px 10px 5px;\n"
  "  padding: 10px 5px;\n"
  "  font-size: calc(100% + 2em);\n" // plus sign
  "  box-shadow: 0 0 5px 0 rgba(0, 0, 0, 0.3);\n" // comma
  "  border: 1px solid #ccc;\n"
  "  font-weight: bold;\n"
  "  -webkit-user-select: none;\n" // hyphen-minus in vendor prefix
  "}\n";

  Tokenizer *t = tok_create(css, &arena);
  if (!t) {
    fprintf(stderr, "Failed to create tokenizer.\n");
    return 1;
  }

  Token tok;
  do {
    tok = tok_next(t);
    printf("Token: type=%s, value='%.*s'\n", tokenTypeToString(tok.type), (int)tok.length, tok.value);
  } while (tok.type != TOKEN_EOF);

  // destron arena
  arena_destroy(&arena);

  return 0;
}
