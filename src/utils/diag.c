#include <stdio.h>
#include "comot-css/diag.h"

void logDiagnostic(const char* message, const char *ctx, size_t line, size_t column) {
  fprintf(stderr, "PARSE ERR at %ld:%ld: %s\nContext: %.20s...\n", line, column, message, ctx);
}

