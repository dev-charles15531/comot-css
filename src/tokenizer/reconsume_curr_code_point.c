#include "tokenizer_impl.h"

const DecodedStream *reconsumeCurrInputCodePoint(Tokenizer *t) {
  const DecodedStream *prev = ptrLookback(t);

  if(prev) {
    if(*prev->bytePtr == '\n') {
      t->line --;
      t->column = 1;
    }
    else {
      t->column --;
    }

    t->curr = prev;
  }

  return prev;
}
