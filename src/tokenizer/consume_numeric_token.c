#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

void consumeNumber(Tokenizer *t) {
  if(*t->curr->bytePtr == '+' || *t->curr->bytePtr == '-') {
    if (!advancePtrToN(t, 1))
      return;
  }

  while(t->curr && t->curr->bytePtr && isDigit(t->curr->bytePtr)) {
    if(!advancePtrToN(t, 1))
      return;
  }

  const DecodedStream *afterDotPtr = peekPtrAtN(t, 1);
  if(t->curr && *t->curr->bytePtr == '.' && afterDotPtr && isDigit(afterDotPtr->bytePtr)) {
    if(!advancePtrToN(t, 2)) // consume '.' and digit
      return;

    while(t->curr && isDigit(t->curr->bytePtr)) {
      if(!advancePtrToN(t, 1))
        return;
    }
  }

  if(t->curr && (*t->curr->bytePtr == 'e' || *t->curr->bytePtr == 'E')) {
    const DecodedStream *afterE = peekPtrAtN(t, 1);
    const DecodedStream *afterSign = peekPtrAtN(t, 2);

    if(afterE && isDigit(afterE->bytePtr)) {
      if(!advancePtrToN(t, 1))
        return;

      while(t->curr && isDigit(t->curr->bytePtr)) {
        if(!advancePtrToN(t, 1))
          return;
      }
    }
    else if(afterE && (*afterE->bytePtr == '+' || *afterE->bytePtr == '-') && afterSign && isDigit(afterSign->bytePtr)) {
      if (!advancePtrToN(t, 2))
        return;

      while(t->curr && isDigit(t->curr->bytePtr)) {
        if(!advancePtrToN(t, 1))
          return;
      }
    }
  }
}


Token consumeNumericToken(Tokenizer *t) {
  const DecodedStream *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  consumeNumber(t);

  if(isNextThreeCodePointStartAnIdentSequence(t)) {
    const DecodedStream *currStream = consumeIdentSequence(t);

    return makeToken(TOKEN_DIMENSION, TOKEN_KIND_VALID, tCurr, currStream - tCurr, startLine, startCol);
  }
  else if(*t->curr->bytePtr == '%') {
    advancePtrToN(t, 1);

    return makeToken(TOKEN_PERCENTAGE, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
  else {
    return makeToken(TOKEN_NUMBER, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
}
