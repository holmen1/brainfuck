#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

ASTNode *parser_parse(Lexer *lexer);

#endif // PARSER_H
