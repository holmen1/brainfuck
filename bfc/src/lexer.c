#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

/* Opaque Lexer structure - only visible in implementation */
struct Lexer {
    const char *source;
    int position;
    int length;
    Token current_token; // Cached current token
    int token_valid;     // Whether current_token is valid
};

static Token lexer_next_token(Lexer *lexer);

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

/**
 * Format all tokens as a string for debugging
 *
 * Returns dynamically allocated string: "Output: [TOKEN1, TOKEN2, ..., EOF]"
 * Caller must free the returned string
 *
 * Arguments:
 *   lexer - Lexer instance
 *
 * Returns:
 *   Allocated string with formatted tokens, or NULL on error
 */
char *lexer_format_tokens(Lexer *lexer)
{
    if (!lexer) {
        return NULL;
    }
    
    // Start with reasonable buffer size
    int capacity = 256;
    int length = 0;
    char *buffer = malloc(capacity);
    if (!buffer) return NULL;
    
    // Write header
    length += snprintf(buffer + length, capacity - length, "Output: [");
    
    TokenType token_type;
    int first = 1;
    
    while ((token_type = lexer_peek(lexer)) != TOKEN_EOF) {
        // Add comma separator
        if (!first) {
            if (length + 2 >= capacity) {
                capacity *= 2;
                char *new_buffer = realloc(buffer, capacity);
                if (!new_buffer) {
                    free(buffer);
                    return NULL;
                }
                buffer = new_buffer;
            }
            length += snprintf(buffer + length, capacity - length, ", ");
        }
        first = 0;
        
        // Get token name
        const char *token_name = "";
        switch (token_type) {
        case TOKEN_RIGHT:
            token_name = "RIGHT";
            break;
        case TOKEN_LEFT:
            token_name = "LEFT";
            break;
        case TOKEN_INC:
            token_name = "INC";
            break;
        case TOKEN_DEC:
            token_name = "DEC";
            break;
        case TOKEN_OUTPUT:
            token_name = "OUTPUT";
            break;
        case TOKEN_INPUT:
            token_name = "INPUT";
            break;
        case TOKEN_LOOP_START:
            token_name = "LOOP_START";
            break;
        case TOKEN_LOOP_END:
            token_name = "LOOP_END";
            break;
        case TOKEN_EOF:
            token_name = "EOF";
            break;
        }
        
        // Expand buffer if needed
        if (length + (int)strlen(token_name) + 10 >= capacity) {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        
        length += snprintf(buffer + length, capacity - length, "%s", token_name);
        lexer_next(lexer);
    }
    
    // Add EOF and closing bracket
    if (length + 10 >= capacity) {
        capacity *= 2;
        char *new_buffer = realloc(buffer, capacity);
        if (!new_buffer) {
            free(buffer);
            return NULL;
        }
        buffer = new_buffer;
    }
    snprintf(buffer + length, capacity - length, ", EOF]");
    
    return buffer;
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
static Token lexer_next_token(Lexer *lexer)
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

