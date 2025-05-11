#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer Lexer;

// Create/destroy lexer
Lexer* lex_create(const char *input, Arena *arena);
void lex_destroy(Lexer *lexer);

// Core functions
Token lex_next(Lexer *lexer);
bool lex_peek(Lexer *lexer, Token *out);

#endif
