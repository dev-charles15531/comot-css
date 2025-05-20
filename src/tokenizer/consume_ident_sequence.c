#include <stdio.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

/**
 * Consumes an identifier sequence starting at the current position.
 *
 * This function will consume all valid identifier characters, including escaped
 * characters.  If the end of the file is reached, the function will return the
 * current position; otherwise, it will return the position after the last
 * character of the sequence.
 *
 * @param t The tokenizer
 * @return The position after the last character of the sequence
 */
const DecodedStream *consumeIdentSequence(Tokenizer *t) {
  const DecodedStream *startPtr = t->curr;

  while(true) {
    if(!advancePtrToN(t, 1))
      break;

    if(isIdentCodePoint(t->curr)) {
      continue;
    } 
    else if(isNCodePointValidEscape(t, 0)) {
      consumeEscapedCodePoint(t);  // Does internal validation

      continue;
    } 
    else {
      startPtr = t->curr;

      break;
    }
  }

  return startPtr;
}
