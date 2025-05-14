#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

void consumeNumber(Tokenizer *t) {
  const char *nxtPtr = peekPtrAtN(t, 1);
  if(*nxtPtr == '+' || *nxtPtr == '-') {
    advancePtrToN(t, 1);
  }

  nxtPtr = peekPtrAtN(t, 1);
  while(nxtPtr && isDigit(nxtPtr)) {
    advancePtrToN(t, 1);
  }

  nxtPtr = peekPtrAtN(t, 1);
  const char *nxt2Prt = peekPtrAtN(t, 2);

  if(*nxtPtr == '.' && isDigit(nxt2Prt)) {
    advancePtrToN(t, 2);

    nxtPtr = peekPtrAtN(t, 1);
    while(nxtPtr && isDigit(nxtPtr)) {
      advancePtrToN(t, 1);
    }
  }

  nxtPtr = peekPtrAtN(t, 1);
  nxt2Prt = peekPtrAtN(t, 2);
  const char *nxt3Ptr = peekPtrAtN(t, 3);

  if((nxtPtr && (*nxtPtr == 'E' || *nxtPtr == 'e')) &&
      ((nxt2Prt && isDigit(nxt2Prt)) ||
       ((nxt2Prt && (*nxt2Prt == '+' || *nxt2Prt == '-')) &&
        (nxt3Ptr && isDigit(nxt3Ptr))))) {
    
    advancePtrToN(t, 1); // consume 'e' or 'E'

    if(*nxt2Prt == '+' || *nxt2Prt == '-') {
      advancePtrToN(t, 1); // consume '+' or '-'
    }

    // consume digits in exponent
    while((nxtPtr = peekPtrAtN(t, 1)) && isDigit(nxtPtr)) {
      advancePtrToN(t, 1);
    }
  }
}

Token consumeNumericToken(Tokenizer *t) {
  const char *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  const char *nxtPtr = peekPtrAtN(t, 1);

  consumeNumber(t);

  if(isNextThreeCodePointStartAnIdentSequence(t)) {
    consumeIdentSequence(t);

    // update nxtPtr
    nxtPtr = peekPtrAtN(t, 1);
    return makeToken(TOKEN_DIMENSION, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
  else if(nxtPtr && *nxtPtr == '%') {
    advancePtrToN(t, 1);

    return makeToken(TOKEN_PERCENTAGE, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
  else {
    return makeToken(TOKEN_NUMBER, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
}
