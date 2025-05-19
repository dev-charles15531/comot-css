#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/error.h"

Token consumeCommentOrDelim(Tokenizer *t, char codePoint) {
  const DecodedStream *tCurr = t->curr;

  const DecodedStream *c = peekPtrAtN(t, 1);
  if (!c) {
    // [PARSE ERR] end of file was reached before the start of comment
    return emitErrorToken(t, "Unexpected end of file", tCurr, t->curr - tCurr, t->line, t->column);
  }

  if (*c->bytePtr == '*') {
    // beginning of a comment: "/*"
    advancePtrToN(t, 1);

    while (1) {
      c = peekPtrAtN(t, 1);
      const DecodedStream *cNext = peekPtrAtN(t, 2);
      if (!c || !cNext) {
        // [PARSE ERR] end of file was reached before the end of comment
        return emitErrorToken(t, "Unexpected end of file", tCurr, t->curr - tCurr, t->line, t->column);
      }

      if (*c->bytePtr == '*' && *cNext->bytePtr == codePoint) {
        advancePtrToN(t, 3);  // advance past the closing `*/`
        
        return makeToken(TOKEN_COMMENT, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
      }

      advancePtrToN(t, 1);
    }
  } else {
    // Not a comment; treat as a delimiter
    advancePtrToN(t, 1);

    return makeToken(TOKEN_DELIM, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, t->line, t->column);
  }
}

