#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>
#include "comot-css/tokens.h"
#include "arena_alloc.h"

typedef struct Tokenizer Tokenizer;   // forward dcl

// Create/destroy tokenizer
Tokenizer *tok_create(const uint8_t *input, size_t len, Arena *arena);
void tok_destroy(Tokenizer *t);

// Core functions
Token tok_next(Tokenizer *t);
bool tok_peek(Tokenizer *t, Token *out);

#endif
