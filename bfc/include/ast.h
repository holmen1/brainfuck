#ifndef AST_H
#define AST_H

typedef enum {
    AST_SEQUENCE,    // List of commands
    AST_MOVE_RIGHT,  // > (single operation)
    AST_MOVE_LEFT,   // < (single operation)  
    AST_INCREMENT,   // + (single operation)
    AST_DECREMENT,   // - (single operation)
    AST_OUTPUT,      // .
    AST_INPUT,       // ,
    AST_LOOP         // [ ... ]
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
        // MOVE_RIGHT, MOVE_LEFT, INCREMENT, DECREMENT, OUTPUT, INPUT have no data
    } data;
} ASTNode;

// Public API
ASTNode *ast_create_node(ASTNodeType type);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node);

#endif // AST_H