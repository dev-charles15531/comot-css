#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "comot-css/tokenizer.h"

typedef struct {
  TokenType type;
  const char *value;
} ExpectedToken;

static const char* tokenTypeToString(TokenType type) {
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

void test_all_tokens() {
  Arena arena = arena_create(4096);   // 4KB

  const char *css =
  "/* comment */\n"
  "123 45.67 10% 1.5em\n"
  ".cls #id :hover ::after\n"
  "{ margin: 0; }\n"
  "@media all {}\n"
  "\"string\" 'another'\n"
  "url(example.png) url(\"quoted.png\")\n"
  "--custom: value;\n";

  Tokenizer *t = tokCreate((const uint8_t *)css, strlen(css), &arena);
  assert(t);

  ExpectedToken expected[] = {
    {TOKEN_COMMENT, "/* comment */"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_NUMBER, "123"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_NUMBER, "45.67"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_PERCENTAGE, "10%"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_DIMENSION, "1.5em"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_DELIM, "."},
    {TOKEN_IDENT, "cls"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_HASH, "#id"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_COLON, ":"},
    {TOKEN_IDENT, "hover"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_COLON, ":"},
    {TOKEN_COLON, ":"},
    {TOKEN_IDENT, "after"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_LEFT_CURLY, "{"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_IDENT, "margin"},
    {TOKEN_COLON, ":"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_NUMBER, "0"},
    {TOKEN_SEMICOLON, ";"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_RIGHT_CURLY, "}"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_AT_KEYWORD, "@media"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_IDENT, "all"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_LEFT_CURLY, "{"},
    {TOKEN_RIGHT_CURLY, "}"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_STRING, "\"string\""},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_STRING, "'another'"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_URL, "example.png"},
    {TOKEN_RIGHT_PAREN, ")" },
    {TOKEN_WHITESPACE, " "},
    {TOKEN_FUNCTION, "url"},
    {TOKEN_STRING, "\"quoted.png\""},
    {TOKEN_RIGHT_PAREN, ")" },
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_IDENT, "--custom"},
    {TOKEN_COLON, ":"},
    {TOKEN_WHITESPACE, " "},
    {TOKEN_IDENT, "value"},
    {TOKEN_SEMICOLON, ";"},
    {TOKEN_WHITESPACE, "\n"},
    {TOKEN_EOF, ""}
  };

  for(size_t i = 0; i < sizeof(expected) / sizeof(ExpectedToken); ++i) {
    Token tok = tokNext(t);
    
    // printf("Actual Token %2zu: type=%s, value='%.*s', length=%zu\n", i, tokenTypeToString(tok.type), (int)tok.length, tok.value, tok.length);

    // Check if the token type matches
    assert(tok.type == expected[i].type);

    // Check if the token value matches
    int result = strncmp(tok.value, expected[i].value, tok.length);
    // printf("Comparing values: '%.*s' vs '%s' -> cmp_result: %d\n", (int)tok.length, tok.value, expected[i].value, cmp_result);

    // Check the length as well
    assert(result == 0);
    assert(strlen(expected[i].value) == tok.length);
    printf("✅ Token %2zu: type=%s, value='%.*s'\n",
      i,
      tokenTypeToString(tok.type),
      (int)tok.length,
      tok.value
    );
  }

  arena_destroy(&arena);
  printf("\n🎉 test_all_tokens passed\n");
}

int main() {
  test_all_tokens();

  return 0;
}
