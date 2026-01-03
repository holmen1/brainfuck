#ifndef AST_H
#define AST_H

typedef enum {
    AST_SEQUENCE,    // List of commands
    AST_MOVE_PTR,    // > or < (combined)
    AST_MODIFY_CELL, // + or - (combined)
    AST_OUTPUT,      // .
    AST_INPUT,       // ,
    AST_LOOP         // [ ... ]
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            int offset; // For MOVE_PTR: +1 for >, -1 for <
        } move;
        struct {
            int delta; // For MODIFY_CELL: +1 for +, -1 for -
        } modify;
        struct {
            struct ASTNode **children;
            int count;
        } sequence;
        struct {
            struct ASTNode *body; // For LOOP: body of loop
        } loop;
    } data;
} ASTNode;

// Public API
ASTNode *ast_create_node(ASTNodeType type);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node);

#endif // AST_H