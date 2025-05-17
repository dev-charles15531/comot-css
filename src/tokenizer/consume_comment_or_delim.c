#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/error.h"

Token consumeCommentOrDelim(Tokenizer *t, char codePoint) {
  const DecodedStream *tCurr = t->curr;

  const DecodedStream *c = peekPtrAtN(t, 1);
  if(*c->bytePtr == '*') {   // this should be a comment[start]
    advancePtrToN(t, 1);
    while(!isEof(t)) {
      c = peekPtrAtN(t, 1);
      const DecodedStream *cNext = peekPtrAtN(t, 2);

      // if this is the end of the comment
      if(*c->bytePtr == '*' && *cNext->bytePtr == codePoint) {
        advancePtrToN(t, 3);

        return makeToken(TOKEN_COMMENT, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
      }

      // consume next
      advancePtrToN(t, 1);
    }

    // [PARSE ERR] end of file was reached before the end of comment
    return emitErrorToken(t, "Unexpected end of file", tCurr, t->curr - tCurr, t->line, t->column);
  }
  else {  // it's not a comment.
    advancePtrToN(t, 1);

    return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
  }
}
