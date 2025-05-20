#include "tokenizer_impl.h"
#include "comot-css/tokens.h"
#include "comot-css/error.h"

/**
 * Consumes a comment or delimiter starting with the given code point.
 *
 * This function will consume a comment if one is present, and otherwise
 * will consume a single delimiter token.  If the end of the file is
 * reached before the start or end of the comment is found, an error is
 * logged and the function will return an error token.
 *
 * @param t The tokenizer
 * @param codePoint The code point to consume as a delimiter if not a comment
 * @return A token representing the comment or delimiter
 */
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

