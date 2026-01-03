#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

ASTNode *parser_parse(Lexer *lexer);

#endif // PARSER_H
