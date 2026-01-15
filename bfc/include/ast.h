#ifndef AST_H
#define AST_H

typedef enum {
    // Naive nodes (1:1 with tokens)
    AST_SEQUENCE,    // List of commands
    AST_MOVE_RIGHT,  // > (single operation)
    AST_MOVE_LEFT,   // < (single operation)  
    AST_INCREMENT,   // + (single operation)
    AST_DECREMENT,   // - (single operation)
    AST_OUTPUT,      // .
    AST_INPUT,       // ,
    AST_LOOP,        // [ ... ]

    // Optimized nodes (created by optimizer)
    AST_MOVE_PTR,    // >>> becomes MOVE_PTR(offset=3)
    AST_MODIFY_CELL, // +++ becomes MODIFY_CELL(delta=3)
    AST_CLEAR_LOOP   // [-] becomes CLEAR_LOOP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            struct ASTNode **children;
            int count;
        } sequence;
        struct {
            struct ASTNode *body; // For LOOP: body of loop
        } loop;
        int offset; // For AST_MOVE_PTR
        int delta;  // For AST_MODIFY_CELL
        // MOVE_RIGHT, MOVE_LEFT, INCREMENT, DECREMENT, OUTPUT, INPUT, CLEAR_LOOP have no data
    } data;
} ASTNode;

// Public API
ASTNode *ast_create_node(ASTNodeType type);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node);
ASTNode *ast_optimize(ASTNode *node);

#endif // AST_H