#include <stdio.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

const DecodedStream *consumeIdentSequence(Tokenizer *t) {
  const DecodedStream *startPtr = t->curr;

  while(advancePtrToN(t, 1)) {
    if(isIdentCodePoint(t->curr))
      continue;
    else if(isNCodePointValidEscape(t, 0)) {
      consumeEscapedCodePoint(t); // NOTE: no code point is returned from this function
      
      continue;
    }
    else {
      startPtr = t->curr;
      // reconsumeCurrInputCodePoint(t);

      break;
    }
  }

  return startPtr;
}
