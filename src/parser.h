#pragma once
#include "ast.h"
#include "lexer.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<Program> parse();

private:
    std::vector<Token> toks;
    size_t pos;

    Token& cur();
    Token& peekAt(int off = 1);
    Token consume();
    Token expect(TokenType t, const std::string& msg);
    bool check(TokenType t) const;
    bool match(TokenType t);

    ASTNodePtr parseStatement();
    ASTNodePtr parseVarDecl();
    ASTNodePtr parseAssign();
    ASTNodePtr parseIf();
    ASTNodePtr parseWhile();
    ASTNodePtr parsePrint();
    ASTNodePtr parseInput();
    ASTNodePtr parseBlock();

    ASTNodePtr parseExpression();
    ASTNodePtr parseComparison();
    ASTNodePtr parseAddSub();
    ASTNodePtr parseMulDiv();
    ASTNodePtr parseUnary();
    ASTNodePtr parsePrimary();
};
