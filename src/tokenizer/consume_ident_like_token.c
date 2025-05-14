#include <strings.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"


Token consumeIdentLikeToken(Tokenizer *t) {
  const char *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  const char *str = consumeIdentSequence(t);

  if(strcasecmp(str, "url") == 0 && *t->curr == '(') {
    advancePtrToN(t, 1);

    while (isWhitespace(peekPtrAtN(t, 0))) {
      advancePtrToN(t, 1);
    }

    if (*t->curr == '"' || *t->curr == '\'') {
      return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
    }

    return consumeUrlToken(t);
  }
 
  if(*t->curr == '(') {
    advancePtrToN(t, 1);

    return makeToken(TOKEN_FUNCTION, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
  
  return makeToken(TOKEN_IDENT, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
}
