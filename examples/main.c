#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "comot-css/tokenizer.h"

int main(void) {
  Arena arena = arena_create(1024 * 1024);

  const char *css = "body { color: red; /* comment anything i want here */ }";
  Tokenizer *t = tok_create(css, &arena);
  if (!t) {
    fprintf(stderr, "Failed to create tokenizer.\n");
    return 1;
  }

  Token tok;
  do {
    tok = tok_next(t);
    printf("Token: type=%d, value='%.*s'\n", tok.type, (int)tok.length, tok.value);
  } while (tok.type != TOKEN_EOF);

  // destron arena
  arena_destroy(&arena);

  return 0;
}
