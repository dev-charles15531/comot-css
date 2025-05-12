#include <stdalign.h>
#include <string.h>
#include "comot-css/tokens.h"
#include "comot-css/tokenizer.h"
#include "tokenizer_impl.h"

#define ARENA_SIZE (64 * 1024) // 64 KB
#define ARENA_ALIGNMENT alignof(max_align_t)

Tokenizer *tok_create(const char *input, Arena *arena) {
  if(!arena)
    return NULL;

  Tokenizer *t = (Tokenizer *) arena_alloc(arena, sizeof(Tokenizer), ARENA_ALIGNMENT);
  if(!t)
    return NULL;

  t->start = input;
  t->curr = input;
  t->end = input + strlen(input);
  t->line = 1;
  t->column = 1;
  t->state = DATA_STATE;
  t->stringQuote = '\0';
  t->arena = arena;

  return t;
}

Token tok_next(Tokenizer *t) {
  // Skip over any characters already consumed
  while (t->curr < t->end) {
    char ch = *t->curr;

    // Save position
    const char *start = t->curr;
    size_t line = t->line;
    size_t column = t->column;

    // Whitespace
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
      while (t->curr < t->end && (*t->curr == ' ' || *t->curr == '\t' ||
                                  *t->curr == '\n' || *t->curr == '\r')) {
        // Track line/column
        if (*t->curr == '\n') {
          t->line++;
          t->column = 1;
        } else {
          t->column++;
        }
        t->curr++;
      }

      return make_token(TOKEN_WHITESPACE, TOKEN_KIND_VALID, start, t->curr - start, line, column);
    }

    // Comments or '/'
    if (ch == '/') {
      return consumeCommentOrDelim(t, '/');
    }

    // Example: EOF check (can be improved)
    if (t->curr >= t->end || *t->curr == '\0') {
      return make_token(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, line, column);
    }

    // For now, treat anything else as a delim (placeholder logic)
    t->curr++;
    t->column++;
    return make_token(TOKEN_DELIM, TOKEN_KIND_VALID, start, 1, line, column);
  }

  // If we fall through the loop, return EOF
  return make_token(TOKEN_EOF, TOKEN_KIND_VALID, t->curr, 0, t->line, t->column);
}
