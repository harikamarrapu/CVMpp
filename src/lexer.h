#pragma once
#include <string>
#include <vector>

enum class TokenType {
    NUMBER, IDENTIFIER, TRUE_KW, FALSE_KW,
    LET, IF, ELSE, WHILE, PRINT, INPUT,
    PLUS, MINUS, STAR, SLASH,
    EQEQ, NEQ, LT, LTE, GT, GTE,
    EQ,
    SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE,
    EOF_TOKEN
};

std::string tokenTypeName(TokenType t);

struct Token {
    TokenType type;
    std::string value;
    int line;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos;
    int line;

    char cur() const;
    char peek(int off = 1) const;
    void adv();
    void skipWS();
    void skipLineComment();
    Token readNumber();
    Token readWord();
};
