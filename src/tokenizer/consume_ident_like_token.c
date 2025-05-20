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

  if(!tCurr)
    return makeToken(TOKEN_EOF, TOKEN_KIND_ERROR, tCurr, 0, startLine, startCol);

  const DecodedStream *str = consumeIdentSequence(t);
  if(!str || str < tCurr || !tCurr)
    return makeToken(TOKEN_EOF, TOKEN_KIND_ERROR, tCurr, 0, startLine, startCol);

  if(str->bytePtr < tCurr->bytePtr)
    return makeToken(TOKEN_EOF, TOKEN_KIND_ERROR, tCurr, 0, startLine, startCol);

  size_t len = str->bytePtr - tCurr->bytePtr;
  if(len > (1 << 20)) {
    return makeToken(TOKEN_EOF, TOKEN_KIND_ERROR, tCurr, 0, startLine, startCol);
  }

  char *result = (char *) arena_alloc(t->arena, len + 1, 1);
  if(!result) {
    fprintf(stderr, "Arena alloc failed\n");
    exit(1);
  }

  memcpy(result, tCurr->bytePtr, len);
  result[len] = '\0';

  if(t->curr && *t->curr->bytePtr == '(') {
    if(!advancePtrToN(t, 1))
      return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, tCurr, 0, startLine, startCol);

    if(strcasecmp(result, "url") == 0) {
      while(t->curr && isWhitespace(t->curr->bytePtr)) {
        if(!advancePtrToN(t, 1))
          return makeToken(TOKEN_EOF, TOKEN_KIND_VALID, tCurr, 0, startLine, startCol);
      }

      if(t->curr && (*t->curr->bytePtr == '"' || *t->curr->bytePtr == '\'')) {
        return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
      }

      return consumeUrlToken(t);
    }

    return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
  }

  return makeToken(TOKEN_IDENT, TOKEN_KIND_VALID, tCurr, len, startLine, startCol);
}
