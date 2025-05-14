#include "tokenizer_impl.h"

const char *reconsumeCurrInputCodePoint(Tokenizer *t) {
  const char *prev = ptrLookback(t);

  if(prev) {
    if(*prev == '\n') {
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
