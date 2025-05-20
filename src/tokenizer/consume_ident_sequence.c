#include <stdio.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

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
