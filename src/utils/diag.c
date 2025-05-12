#include "comot-css/error.h"
#include "comot-css/diag.h"

// const char* lexer_error_message(LexerErrorCode code) {
//   switch (code) {
//       case LEXER_OK: return "No error";
//       case LEXER_ERR_UNTERMINATED_STRING: return "Unterminated string literal";
//       case LEXER_ERR_INVALID_ESCAPE: return "Invalid escape sequence";
//       case LEXER_ERR_UNEXPECTED_CHAR: return "Unexpected character";
//       case LEXER_ERR_UNTERMINATED_COMMENT: return "Unterminated comment";
//       case LEXER_ERR_UNKNOWN_TOKEN: return "Unknown or malformed token";
//       case LEXER_ERR_BAD_NUMBER: return "Malformed number";
//       default: return "Unknown lexer error";
//   }
// }

void lexer_diag_add(LexerDiagnostics* diag, LexerErrorCode code, size_t line, size_t column) {
  if (diag->count < MAX_LEXER_ERRORS) {
      diag->errors[diag->count++] = (LexerError){ code, line, column };
  }
}

