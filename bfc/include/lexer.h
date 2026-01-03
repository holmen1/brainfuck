#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_RIGHT,      // >
    TOKEN_LEFT,       // <
    TOKEN_INC,        // +
    TOKEN_DEC,        // -
    TOKEN_OUTPUT,     // .
    TOKEN_INPUT,      // ,
    TOKEN_LOOP_START, // [
    TOKEN_LOOP_END,   // ]
    TOKEN_EOF         // End of file
} TokenType;

typedef struct {
    TokenType type;
    int position; // Position in source (for error messages)
} Token;

typedef struct Lexer Lexer;

// Public API
Lexer *lexer_create(const char *source, int length);
TokenType lexer_peek(Lexer *lexer);
void lexer_next(Lexer *lexer);
char *lexer_format_tokens(Lexer *lexer);
void lexer_free(Lexer *lexer);

#endif // LEXER_H
