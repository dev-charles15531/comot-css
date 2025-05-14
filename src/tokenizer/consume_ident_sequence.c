#include <stdio.h>
#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

const char *consumeIdentSequence(Tokenizer *t) {
  const char *startPtr = t->curr;

  while(advancePtrToN(t, 1)) {
    if(isIdentCodePoint(t->curr))
      continue;
    else if(isNCodePointValidEscape(t, 0)) {
      consumeEscapedCodePoint(t, *t->curr);
      
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
