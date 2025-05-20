#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>
#include "comot-css/tokens.h"
#include "arena_alloc.h"

typedef struct Tokenizer Tokenizer;   // forward dcl

// Create/destroy tokenizer
Tokenizer *tokCreate(const uint8_t *input, size_t len, Arena *arena);

// Get next token
Token tokNext(Tokenizer *t);

#endif
