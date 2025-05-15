#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

void consumeNumber(Tokenizer *t) {
  if(*t->curr == '+' || *t->curr == '-') {
    advancePtrToN(t, 1);
  }

  while(isDigit(t->curr)) {
    advancePtrToN(t, 1);
  }

  if (*t->curr == '.') {
    const char *afterDot = peekPtrAtN(t, 1);
    if (afterDot && isDigit(afterDot)) {
      advancePtrToN(t, 2); // consume '-' and digits

      while (isDigit(t->curr)) {
        advancePtrToN(t, 1);
      }
    }
  }

  if (*t->curr == 'e' || *t->curr == 'E') {
    const char *afterE = peekPtrAtN(t, 1);
    const char *afterSign = peekPtrAtN(t, 2);

    if(afterE && isDigit(afterE)) {
      advancePtrToN(t, 1); // consume 'e'

      while (isDigit(t->curr))
        advancePtrToN(t, 1);
    }
    else if(afterE && (*afterE == '+' || *afterE == '-') && afterSign && isDigit(afterSign)) {
      advancePtrToN(t, 2); // consume 'e' and '+' or '-'

      while(isDigit(t->curr)) 
        advancePtrToN(t, 1);
    }
  }
}

Token consumeNumericToken(Tokenizer *t) {
  const char *tCurr = t->curr;
  size_t startLine = t->line;
  size_t startCol = t->column;

  consumeNumber(t);

  if(isNextThreeCodePointStartAnIdentSequence(t)) {
    const char *currPtr = consumeIdentSequence(t);

    return makeToken(TOKEN_DIMENSION, TOKEN_KIND_VALID, tCurr, currPtr - tCurr, startLine, startCol);
  }
  else if(*t->curr == '%') {
    advancePtrToN(t, 1);

    return makeToken(TOKEN_PERCENTAGE, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
  else {
    return makeToken(TOKEN_NUMBER, TOKEN_KIND_VALID, tCurr, t->curr - tCurr, startLine, startCol);
  }
}
