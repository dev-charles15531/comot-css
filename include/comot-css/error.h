#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>
#include "comot-css/tokens.h"

Token emitErrorToken(Tokenizer *t, const char* message, const DecodedStream *value, size_t length, size_t line, size_t column);

#endif
