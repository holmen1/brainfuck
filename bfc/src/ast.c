#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

static void ast_print_helper(ASTNode *node, int depth);

ASTNode *ast_create_node(ASTNodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        return NULL;
    }

    node->type = type;

    // Initialize the union based on type
    switch (type) {
    case AST_SEQUENCE:
        node->data.sequence.children = NULL;
        node->data.sequence.count = 0;
        break;
    case AST_LOOP:
        node->data.loop.body = NULL;
        break;
    case AST_MOVE_PTR:
        node->data.offset = 0;
        break;
    case AST_MODIFY_CELL:
        node->data.delta = 0;
        break;
    case AST_MOVE_RIGHT:
    case AST_MOVE_LEFT:
    case AST_INCREMENT:
    case AST_DECREMENT:
    case AST_OUTPUT:
    case AST_INPUT:
    case AST_CLEAR_LOOP:
        // These types don't have data
        break;
    }

    return node;
}

void ast_free(ASTNode *node)
{
    if (!node) {
        return;
    }

    // Free the union data based on type
    switch (node->type) {
    case AST_SEQUENCE:
        // Free all children
        for (int i = 0; i < node->data.sequence.count; i++) {
            ast_free(node->data.sequence.children[i]);
        }
        free(node->data.sequence.children);
        break;
    case AST_LOOP:
        // Free the loop body
        ast_free(node->data.loop.body);
        break;
    case AST_MOVE_RIGHT:
    case AST_MOVE_LEFT:
    case AST_INCREMENT:
    case AST_DECREMENT:
    case AST_OUTPUT:
    case AST_INPUT:
    case AST_MOVE_PTR:
    case AST_MODIFY_CELL:
    case AST_CLEAR_LOOP:
        // No allocated data to free
        break;
    }

    // Free the node itself
    free(node);
}

void ast_print(ASTNode *node)
{
    if (!node) {
        return;
    }

    ast_print_helper(node, 0);
}

static void ast_print_helper(ASTNode *node, int depth)
{
    if (!node) {
        return;
    }

    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Print node type and details
    switch (node->type) {
    case AST_MOVE_RIGHT:
        printf("MOVE_RIGHT\n");
        break;
    case AST_MOVE_LEFT:
        printf("MOVE_LEFT\n");
        break;
    case AST_INCREMENT:
        printf("INCREMENT\n");
        break;
    case AST_DECREMENT:
        printf("DECREMENT\n");
        break;
    case AST_OUTPUT:
        printf("OUTPUT\n");
        break;
    case AST_INPUT:
        printf("INPUT\n");
        break;
    case AST_MOVE_PTR:
        printf("MOVE_PTR(%+d)\n", node->data.offset);
        break;
    case AST_MODIFY_CELL:
        printf("MODIFY_CELL(%+d)\n", node->data.delta);
        break;
    case AST_CLEAR_LOOP:
        printf("CLEAR_LOOP\n");
        break;
    case AST_SEQUENCE:
        printf("SEQUENCE(%d children)\n", node->data.sequence.count);
        for (int i = 0; i < node->data.sequence.count; i++) {
            ast_print_helper(node->data.sequence.children[i], depth + 1);
        }
        break;
    case AST_LOOP:
        printf("LOOP\n");
        ast_print_helper(node->data.loop.body, depth + 1);
        break;
    }
}
