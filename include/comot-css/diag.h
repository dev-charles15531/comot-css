#ifndef DIAG_H
#define DIAG_H

#include "tokenizer_impl.h"

/**
 * @brief Logs a diagnostic message with the given `message`, `ctx`, `line`, and `column`
 *
 * @param message a human-readable diagnostic message
 * @param ctx     a pointer to the original source code
 * @param line    the line number in the source code where the diagnostic occurred
 * @param column  the column number in the source code where the diagnostic occurred
 */
void logDiagnostic(const char* message, const char *ctx, size_t line, size_t column);

#endif

