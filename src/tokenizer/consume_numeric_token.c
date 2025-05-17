#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

void consumeNumber(Tokenizer *t) {
  if(*t->curr->bytePtr == '+' || *t->curr->bytePtr == '-') {
    advancePtrToN(t, 1);
  }

  while(isDigit(t->curr->bytePtr)) {
    advancePtrToN(t, 1);
  }

  if (*t->curr->bytePtr == '.') {
    const char *afterDot = peekPtrAtN(t, 1)->bytePtr;
    if (afterDot && isDigit(afterDot)) {
      advancePtrToN(t, 2); // consume '-' and digits

      while (isDigit(t->curr->bytePtr)) {
        advancePtrToN(t, 1);
      }
    }
  }

  if (*t->curr->bytePtr == 'e' || *t->curr->bytePtr == 'E') {
    const char *afterE = peekPtrAtN(t, 1)->bytePtr;
    const char *afterSign = peekPtrAtN(t, 2)->bytePtr;

    if(afterE && isDigit(afterE)) {
      advancePtrToN(t, 1); // consume 'e'

      while(isDigit(t->curr->bytePtr))
        advancePtrToN(t, 1);
    }
    else if(afterE && (*afterE == '+' || *afterE == '-') && afterSign && isDigit(afterSign)) {
      advancePtrToN(t, 2); // consume 'e' and '+' or '-'

      while(isDigit(t->curr->bytePtr)) 
        advancePtrToN(t, 1);
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
