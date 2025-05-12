#ifndef DIAG_H
#define DIAG_H

#include "error.h"
#include <stddef.h>

// Max errors collected per session
#define MAX_LEXER_ERRORS 32

typedef struct {
  LexerError errors[MAX_LEXER_ERRORS];
  size_t count;
} LexerDiagnostics;

void lexer_diag_add(LexerDiagnostics* diag, LexerErrorCode code, size_t line, size_t column);

#endif
