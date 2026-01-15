#include <stdlib.h>
#include "ast.h"

static ASTNode *optimize_sequence(ASTNode *sequence);
static int is_clear_loop(ASTNode *loop);

/**
 * Optimize an AST by combining consecutive operations
 * 
 * Transformations:
 * - Multiple MOVE_RIGHT/LEFT -> MOVE_PTR(offset)
 * - Multiple INCREMENT/DECREMENT -> MODIFY_CELL(delta)
 * - [-] or [+] loops -> CLEAR_LOOP
 * - Recursively optimize nested loops
 */
ASTNode *ast_optimize(ASTNode *node)
{
    if (!node) {
        return NULL;
    }

    switch (node->type) {
    case AST_SEQUENCE:
        return optimize_sequence(node);
    case AST_LOOP:
        // Check for clear loop pattern: [-] or [+]
        if (is_clear_loop(node)) {
            ASTNode *clear = ast_create_node(AST_CLEAR_LOOP);
            ast_free(node);
            return clear;
        } else {
            // Recursively optimize loop body
            node->data.loop.body = ast_optimize(node->data.loop.body);
            return node;
        }
    default:
        // Leaf nodes - no optimization needed
        return node;
    }
}

/**
 * Optimize a sequence by combining consecutive operations
 */
static ASTNode *optimize_sequence(ASTNode *sequence)
{
    if (!sequence || sequence->data.sequence.count == 0) {
        return sequence;
    }

    ASTNode *optimized = ast_create_node(AST_SEQUENCE);
    int i = 0;

    while (i < sequence->data.sequence.count) {
        ASTNode *child = sequence->data.sequence.children[i];

        // Try to combine consecutive MOVE_RIGHT/MOVE_LEFT
        if (child->type == AST_MOVE_RIGHT || child->type == AST_MOVE_LEFT) {
            int offset = (child->type == AST_MOVE_RIGHT) ? 1 : -1;
            i++;

            // Accumulate consecutive moves
            while (i < sequence->data.sequence.count) {
                ASTNode *next = sequence->data.sequence.children[i];
                if (next->type == AST_MOVE_RIGHT) {
                    offset++;
                    i++;
                } else if (next->type == AST_MOVE_LEFT) {
                    offset--;
                    i++;
                } else {
                    break;
                }
            }

            // Only emit if net movement is non-zero
            if (offset != 0) {
                ASTNode *move = ast_create_node(AST_MOVE_PTR);
                move->data.offset = offset;
                
                optimized->data.sequence.children = realloc(
                    optimized->data.sequence.children,
                    sizeof(ASTNode *) * (optimized->data.sequence.count + 1));
                optimized->data.sequence.children[optimized->data.sequence.count++] = move;
            }
        }
        // Try to combine consecutive INCREMENT/DECREMENT
        else if (child->type == AST_INCREMENT || child->type == AST_DECREMENT) {
            int delta = (child->type == AST_INCREMENT) ? 1 : -1;
            i++;

            // Accumulate consecutive modifications
            while (i < sequence->data.sequence.count) {
                ASTNode *next = sequence->data.sequence.children[i];
                if (next->type == AST_INCREMENT) {
                    delta++;
                    i++;
                } else if (next->type == AST_DECREMENT) {
                    delta--;
                    i++;
                } else {
                    break;
                }
            }

            // Only emit if net change is non-zero
            if (delta != 0) {
                ASTNode *modify = ast_create_node(AST_MODIFY_CELL);
                modify->data.delta = delta;
                
                optimized->data.sequence.children = realloc(
                    optimized->data.sequence.children,
                    sizeof(ASTNode *) * (optimized->data.sequence.count + 1));
                optimized->data.sequence.children[optimized->data.sequence.count++] = modify;
            }
        }
        // Recursively optimize loops and other nodes
        else {
            ASTNode *opt_child = ast_optimize(child);
            optimized->data.sequence.children = realloc(
                optimized->data.sequence.children,
                sizeof(ASTNode *) * (optimized->data.sequence.count + 1));
            optimized->data.sequence.children[optimized->data.sequence.count++] = opt_child;
            i++;
        }
    }

    // Free the old sequence structure (but not the children - we reused them)
    free(sequence->data.sequence.children);
    free(sequence);

    return optimized;
}

/**
 * Check if a loop matches the clear pattern: [-] or [+]
 */
static int is_clear_loop(ASTNode *loop)
{
    if (loop->type != AST_LOOP || !loop->data.loop.body) {
        return 0;
    }

    ASTNode *body = loop->data.loop.body;
    
    // Body must be a sequence with exactly one child
    if (body->type != AST_SEQUENCE || body->data.sequence.count != 1) {
        return 0;
    }

    ASTNode *child = body->data.sequence.children[0];
    
    // Child must be INCREMENT or DECREMENT
    return (child->type == AST_INCREMENT || child->type == AST_DECREMENT);
}
