#include <stdlib.h>
#include "lexer.h"
#include "ast.h"
#include "parser.h"

static ASTNode *parse_statement(Lexer *lexer);
static ASTNode *parse_loop(Lexer *lexer);

ASTNode *parser_parse(Lexer *lexer)
{
    if (!lexer) {
        return NULL;
    }

    ASTNode *root = ast_create_node(AST_SEQUENCE);
    if (!root) {
        return NULL;
    }

    TokenType token = lexer_peek(lexer);

    // Parse statements until EOF
    while (token != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(lexer);
        if (!stmt) {
            ast_free(root);
            return NULL;
        }

        // Add statement to sequence
        root->data.sequence.children = (ASTNode **)realloc(
            root->data.sequence.children,
            sizeof(ASTNode *) * (root->data.sequence.count + 1));
        if (!root->data.sequence.children) {
            ast_free(root);
            ast_free(stmt);
            return NULL;
        }

        root->data.sequence.children[root->data.sequence.count++] = stmt;
        token = lexer_peek(lexer);
    }

    return root;
}

static ASTNode *parse_statement(Lexer *lexer)
{
    TokenType token = lexer_peek(lexer);

    switch (token) {
    case TOKEN_RIGHT: {
        // Accumulate consecutive moves right
        int offset = 0;
        while (lexer_peek(lexer) == TOKEN_RIGHT) {
            lexer_next(lexer);
            offset++;
        }
        ASTNode *node = ast_create_node(AST_MOVE_PTR);
        if (node) {
            node->data.move.offset = offset;
        }
        return node;
    }
    case TOKEN_LEFT: {
        // Accumulate consecutive moves left
        int offset = 0;
        while (lexer_peek(lexer) == TOKEN_LEFT) {
            lexer_next(lexer);
            offset--;
        }
        ASTNode *node = ast_create_node(AST_MOVE_PTR);
        if (node) {
            node->data.move.offset = offset;
        }
        return node;
    }
    case TOKEN_INC: {
        // Accumulate consecutive increments
        int delta = 0;
        while (lexer_peek(lexer) == TOKEN_INC) {
            lexer_next(lexer);
            delta++;
        }
        ASTNode *node = ast_create_node(AST_MODIFY_CELL);
        if (node) {
            node->data.modify.delta = delta;
        }
        return node;
    }
    case TOKEN_DEC: {
        // Accumulate consecutive decrements
        int delta = 0;
        while (lexer_peek(lexer) == TOKEN_DEC) {
            lexer_next(lexer);
            delta--;
        }
        ASTNode *node = ast_create_node(AST_MODIFY_CELL);
        if (node) {
            node->data.modify.delta = delta;
        }
        return node;
    }
    case TOKEN_OUTPUT: {
        lexer_next(lexer);
        return ast_create_node(AST_OUTPUT);
    }
    case TOKEN_INPUT: {
        lexer_next(lexer);
        return ast_create_node(AST_INPUT);
    }
    case TOKEN_LOOP_START: {
        return parse_loop(lexer);
    }
    case TOKEN_LOOP_END:
    case TOKEN_EOF:
    default:
        return NULL;
    }
}

static ASTNode *parse_loop(Lexer *lexer)
{
    // Consume the '['
    lexer_next(lexer);

    // Create loop node with sequence body
    ASTNode *loop = ast_create_node(AST_LOOP);
    if (!loop) {
        return NULL;
    }

    ASTNode *body = ast_create_node(AST_SEQUENCE);
    if (!body) {
        ast_free(loop);
        return NULL;
    }

    // Parse statements until ']'
    while (lexer_peek(lexer) != TOKEN_LOOP_END &&
           lexer_peek(lexer) != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(lexer);
        if (!stmt) {
            ast_free(loop);
            ast_free(body);
            return NULL;
        }

        // Add to body sequence
        body->data.sequence.children = (ASTNode **)realloc(
            body->data.sequence.children,
            sizeof(ASTNode *) * (body->data.sequence.count + 1));
        if (!body->data.sequence.children) {
            ast_free(loop);
            ast_free(body);
            ast_free(stmt);
            return NULL;
        }

        body->data.sequence.children[body->data.sequence.count++] = stmt;
    }

    // Consume the ']'
    if (lexer_peek(lexer) == TOKEN_LOOP_END) {
        lexer_next(lexer);
    } else {
        // Unmatched '['
        ast_free(loop);
        ast_free(body);
        return NULL;
    }

    loop->data.loop.body = body;
    return loop;
}
