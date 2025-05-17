#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "comot-css/tokenizer.h"

#define MAX_INPUT_SIZE 1024
#define FIXED_ARENA_SIZE 2048
#define MAX_TOKENS 1024

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if(size == 0 || size > MAX_INPUT_SIZE)
    return 0;

  char *input = malloc(size + 1);
  if(!input)
    return 0;

  memcpy(input, data, size);
  input[size] = '\0';

  Arena arena = arena_create(FIXED_ARENA_SIZE);
  Tokenizer *t = tok_create((const uint8_t *)input, size + 1, &arena); // Include null terminator

  if(t) {
    Token tok;
    size_t token_count = 0;

    do {
      tok = tok_next(t);
      if (token_count++ < 5)

      // Exit loop if max tokens reached or EOF
    } while(tok.type != TOKEN_EOF && token_count < MAX_TOKENS);
  }

  arena_destroy(&arena);
  free(input);
  return 0;
}
