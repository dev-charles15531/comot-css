#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

Token consumeCommentOrDelim(Tokenizer *t, char codePoint) {
  const char *tCurr = t->curr;

  const char *c = peekPtrAtN(t, 1);
  if(*c == '*') {   // this should be a comment[start]
    advancePtrToN(t, 1);
    while(!isEof(t)) {
      c = peekPtrAtN(t, 1);
      const char *cNext = peekPtrAtN(t, 2);

      // if this is the end of the comment
      if(*c == '*' && *cNext == codePoint) {
        advancePtrToN(t, 3);

        return make_token(TOKEN_COMMENT, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
      }

      // consume next
      advancePtrToN(t, 1);
    }

    // end of file was reached before the end of comment[PARSE ERR]
    return make_token(TOKEN_ERROR, TOKEN_KIND_ERROR, tCurr, t->curr - tCurr, t->line, t->column);
  }
  else {  // it's not a comment.
    advancePtrToN(t, 1);

    return make_token(TOKEN_DELIM, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
  }
}
