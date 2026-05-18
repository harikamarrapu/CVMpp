#include "parser.h"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : toks(std::move(tokens)), pos(0) {}

Token& Parser::cur()           { return toks[pos]; }
Token& Parser::peekAt(int off) { size_t p = pos + off; return toks[p < toks.size() ? p : toks.size()-1]; }
Token  Parser::consume()       { return toks[pos++]; }
bool   Parser::check(TokenType t) const { return toks[pos].type == t; }

bool Parser::match(TokenType t) {
    if (check(t)) { pos++; return true; }
    return false;
}

Token Parser::expect(TokenType t, const std::string& msg) {
    if (!check(t))
        throw std::runtime_error(msg + " at line " + std::to_string(cur().line) +
                                 " (got '" + cur().value + "')");
    return consume();
}

std::unique_ptr<Program> Parser::parse() {
    auto prog = std::make_unique<Program>();
    while (!check(TokenType::EOF_TOKEN))
        prog->stmts.push_back(parseStatement());
    return prog;
}

ASTNodePtr Parser::parseStatement() {
    if (check(TokenType::LET))    return parseVarDecl();
    if (check(TokenType::IF))     return parseIf();
    if (check(TokenType::WHILE))  return parseWhile();
    if (check(TokenType::PRINT))  return parsePrint();
    if (check(TokenType::INPUT))  return parseInput();
    if (check(TokenType::LBRACE)) return parseBlock();
    if (check(TokenType::IDENTIFIER) && peekAt(1).type == TokenType::EQ)
        return parseAssign();
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    return expr;
}

ASTNodePtr Parser::parseVarDecl() {
    expect(TokenType::LET, "Expected 'let'");
    auto name = expect(TokenType::IDENTIFIER, "Expected variable name").value;
    expect(TokenType::EQ, "Expected '='");
    auto init = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<VarDecl>(name, std::move(init));
}

ASTNodePtr Parser::parseAssign() {
    auto name = consume().value;
    expect(TokenType::EQ, "Expected '='");
    auto val = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<Assign>(name, std::move(val));
}

ASTNodePtr Parser::parseIf() {
    expect(TokenType::IF, "Expected 'if'");
    expect(TokenType::LPAREN, "Expected '('");
    auto cond = parseExpression();
    expect(TokenType::RPAREN, "Expected ')'");
    auto then_ = parseBlock();
    ASTNodePtr else_;
    if (match(TokenType::ELSE)) else_ = parseBlock();
    return std::make_unique<IfStmt>(std::move(cond), std::move(then_), std::move(else_));
}

ASTNodePtr Parser::parseWhile() {
    expect(TokenType::WHILE, "Expected 'while'");
    expect(TokenType::LPAREN, "Expected '('");
    auto cond = parseExpression();
    expect(TokenType::RPAREN, "Expected ')'");
    auto body = parseBlock();
    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

ASTNodePtr Parser::parsePrint() {
    expect(TokenType::PRINT, "Expected 'print'");
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<PrintStmt>(std::move(expr));
}

ASTNodePtr Parser::parseInput() {
    expect(TokenType::INPUT, "Expected 'input'");
    auto name = expect(TokenType::IDENTIFIER, "Expected variable name").value;
    expect(TokenType::SEMICOLON, "Expected ';'");
    return std::make_unique<InputStmt>(name);
}

ASTNodePtr Parser::parseBlock() {
    expect(TokenType::LBRACE, "Expected '{'");
    auto block = std::make_unique<Block>();
    while (!check(TokenType::RBRACE) && !check(TokenType::EOF_TOKEN))
        block->stmts.push_back(parseStatement());
    expect(TokenType::RBRACE, "Expected '}'");
    return block;
}

ASTNodePtr Parser::parseExpression() { return parseComparison(); }

ASTNodePtr Parser::parseComparison() {
    auto left = parseAddSub();
    while (check(TokenType::EQEQ) || check(TokenType::NEQ) ||
           check(TokenType::LT)   || check(TokenType::LTE) ||
           check(TokenType::GT)   || check(TokenType::GTE)) {
        auto op = consume().value;
        auto right = parseAddSub();
        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseAddSub() {
    auto left = parseMulDiv();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        auto op = consume().value;
        left = std::make_unique<BinExpr>(op, std::move(left), parseMulDiv());
    }
    return left;
}

ASTNodePtr Parser::parseMulDiv() {
    auto left = parseUnary();
    while (check(TokenType::STAR) || check(TokenType::SLASH)) {
        auto op = consume().value;
        left = std::make_unique<BinExpr>(op, std::move(left), parseUnary());
    }
    return left;
}

ASTNodePtr Parser::parseUnary() {
    if (check(TokenType::MINUS)) {
        auto op = consume().value;
        return std::make_unique<UnaryExpr>(op, parseUnary());
    }
    return parsePrimary();
}

ASTNodePtr Parser::parsePrimary() {
    if (check(TokenType::NUMBER)) {
        return std::make_unique<NumberLit>(std::stoi(consume().value));
    }
    if (check(TokenType::TRUE_KW))  { consume(); return std::make_unique<BoolLit>(true);  }
    if (check(TokenType::FALSE_KW)) { consume(); return std::make_unique<BoolLit>(false); }
    if (check(TokenType::IDENTIFIER)) {
        return std::make_unique<Ident>(consume().value);
    }
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    throw std::runtime_error("Unexpected token '" + cur().value +
                             "' at line " + std::to_string(cur().line));
}
