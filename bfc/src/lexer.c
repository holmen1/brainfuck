#include <stdlib.h>
#include "lexer.h"

/* Opaque Lexer structure - only visible in implementation */
struct Lexer {
    const char *source;
    int position;
    int length;
    Token current_token; // Cached current token
    int token_valid;     // Whether current_token is valid
};

/**
 * Create a new lexer for tokenizing Brainfuck source code
 *
 * Arguments:
 *   source - Pointer to source code string
 *   length - Length of source code
 *
 * Returns:
 *   Pointer to new Lexer, or NULL on error
 */
Lexer *lexer_create(const char *source, int length)
{
    if (!source || length < 0) {
        return NULL;
    }

    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) {
        return NULL;
    }

    lexer->source = source;
    lexer->position = 0;
    lexer->length = length;
    lexer->token_valid = 0; // No token cached yet

    return lexer;
}

/**
 * Get the next token from the source
 *
 * Skips all non-command characters (whitespace, comments, etc.)
 * Returns the next Brainfuck command as a token
 *
 * Arguments:
 *   lexer - Lexer instance
 *
 * Returns:
 *   Next token with type and position
 */
Token lexer_next_token(Lexer *lexer)
{
    if (!lexer) {
        return (Token){TOKEN_EOF, -1};
    }

    /* Skip non-command characters */
    while (lexer->position < lexer->length) {
        char ch = lexer->source[lexer->position];

        /* Check if character is a valid BF command */
        if (ch == '>' || ch == '<' || ch == '+' || ch == '-' || ch == '.' ||
            ch == ',' || ch == '[' || ch == ']') {
            break;
        }

        /* Skip whitespace, newlines, and comments */
        lexer->position++;
    }

    /* Check for EOF */
    if (lexer->position >= lexer->length) {
        return (Token){TOKEN_EOF, lexer->position};
    }

    /* Get current character and position */
    char ch = lexer->source[lexer->position];
    int pos = lexer->position;
    lexer->position++;

    /* Convert character to token */
    switch (ch) {
    case '>':
        return (Token){TOKEN_RIGHT, pos};
    case '<':
        return (Token){TOKEN_LEFT, pos};
    case '+':
        return (Token){TOKEN_INC, pos};
    case '-':
        return (Token){TOKEN_DEC, pos};
    case '.':
        return (Token){TOKEN_OUTPUT, pos};
    case ',':
        return (Token){TOKEN_INPUT, pos};
    case '[':
        return (Token){TOKEN_LOOP_START, pos};
    case ']':
        return (Token){TOKEN_LOOP_END, pos};
    default:
        /* Should not reach here */
        return (Token){TOKEN_EOF, pos};
    }
}

/**
 * Free lexer resources
 *
 * Arguments:
 *   lexer - Lexer instance to free
 */
void lexer_free(Lexer *lexer)
{
    if (lexer) {
        free(lexer);
    }
}

/**
 * Peek at the current token without consuming it
 *
 * Arguments:
 *   lexer - Lexer instance
 *
 * Returns:
 *   The current token type
 */
TokenType lexer_peek(Lexer *lexer)
{
    if (!lexer) {
        return TOKEN_EOF;
    }

    /* If we don't have a cached token, get the next one */
    if (!lexer->token_valid) {
        lexer->current_token = lexer_next_token(lexer);
        lexer->token_valid = 1;
    }

    return lexer->current_token.type;
}

/**
 * Consume the current token and move to the next
 *
 * Arguments:
 *   lexer - Lexer instance
 */
void lexer_next(Lexer *lexer)
{
    if (!lexer) {
        return;
    }

    /* Invalidate cached token so next peek will get a new one */
    lexer->token_valid = 0;
}
