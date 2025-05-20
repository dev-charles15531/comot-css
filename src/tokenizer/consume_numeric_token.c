#include "tokenizer_impl.h"
#include "comot-css/tokens.h"

/**
 * Consumes a number in the tokenizer's input stream.
 *
 * Advances the tokenizer's current position over a number in the input stream.
 * The number is allowed to have a leading sign, and may be a decimal number
 * (i.e. have a fractional part).  If an 'e' or 'E' appears after the number,
 * an exponent may also be present.
 *
 * @param t   Pointer to the Tokenizer instance.
 */
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

/**
 * Consumes a numeric token from the tokenizer's input stream.
 *
 * This function processes a numeric sequence in the input stream, which
 * may be a number, a percentage, or a dimension. It first consumes the
 * number using the consumeNumber function. Then, it checks if the number
 * is followed by an identifier sequence to determine if it's a dimension.
 * If a '%' character follows the number, it is a percentage. Otherwise, 
 * it is treated as a standalone number.
 *
 * @param t Pointer to the Tokenizer instance.
 * @return A token representing the numeric value, which can be of type
 *         TOKEN_NUMBER, TOKEN_PERCENTAGE, or TOKEN_DIMENSION.
 */
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
