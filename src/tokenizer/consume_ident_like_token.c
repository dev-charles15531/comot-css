#include <strings.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token consumeIdentLikeToken(Tokenizer *t) {
  const DecodedStream *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  const DecodedStream *str = consumeIdentSequence(t);
  size_t len = str - tCurr;

  char *result = (char *) arena_alloc(t->arena, len + 1, 1);
  if(result == NULL) {
    printf("Arena alloc failed");
    exit(1);
  }

  memcpy(result, tCurr->bytePtr, len);
  result[len] = '\0'; // Null-terminate

  if(*t->curr->bytePtr == '(') {
    advancePtrToN(t, 1);

    // printf("string:: %s, len:: %zu\n", result, len);
    if(strcasecmp(result, "url") == 0) {
      while(isWhitespace(t->curr->bytePtr)) {
        advancePtrToN(t, 1);
      }

      if (*t->curr->bytePtr == '"' || *t->curr->bytePtr == '\'') {
        return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
      }

      return consumeUrlToken(t);
    }

    return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
  }
 
  return makeToken(TOKEN_IDENT, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
}
