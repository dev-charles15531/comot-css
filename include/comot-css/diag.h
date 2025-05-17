#ifndef DIAG_H
#define DIAG_H

#include "tokenizer_impl.h"

void logDiagnostic(const char* message, const char *ctx, size_t line, size_t column);

#endif

