#include <stdio.h>
#include "comot-css/diag.h"

/**
 * @brief Logs a diagnostic message to stderr with details about the parse error.
 *
 * This function prints an error message specifying the line and column where 
 * the error occurred, along with the provided diagnostic message and a snippet 
 * of the context in which the error occurred.
 *
 * @param message A human-readable diagnostic message describing the error.
 * @param ctx A snippet of the source code context where the error occurred.
 * @param line The line number in the source code where the error was detected.
 * @param column The column number in the source code where the error was detected.
 */
void logDiagnostic(const char* message, const char *ctx, size_t line, size_t column) {
  fprintf(stderr, "PARSE ERR at %ld:%ld: %s\nContext: %.20s...\n", line, column, message, ctx);
}

