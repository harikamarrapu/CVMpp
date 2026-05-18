#include "lexer.h"
#include <cctype>
#include <stdexcept>

std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::NUMBER:     return "NUMBER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::TRUE_KW:    return "TRUE";
        case TokenType::FALSE_KW:   return "FALSE";
        case TokenType::LET:        return "LET";
        case TokenType::IF:         return "IF";
        case TokenType::ELSE:       return "ELSE";
        case TokenType::WHILE:      return "WHILE";
        case TokenType::PRINT:      return "PRINT";
        case TokenType::INPUT:      return "INPUT";
        case TokenType::PLUS:       return "PLUS";
        case TokenType::MINUS:      return "MINUS";
        case TokenType::STAR:       return "STAR";
        case TokenType::SLASH:      return "SLASH";
        case TokenType::EQEQ:       return "EQEQ";
        case TokenType::NEQ:        return "NEQ";
        case TokenType::LT:         return "LT";
        case TokenType::LTE:        return "LTE";
        case TokenType::GT:         return "GT";
        case TokenType::GTE:        return "GTE";
        case TokenType::EQ:         return "EQ";
        case TokenType::SEMICOLON:  return "SEMICOLON";
        case TokenType::LPAREN:     return "LPAREN";
        case TokenType::RPAREN:     return "RPAREN";
        case TokenType::LBRACE:     return "LBRACE";
        case TokenType::RBRACE:     return "RBRACE";
        case TokenType::EOF_TOKEN:  return "EOF";
        default:                    return "UNKNOWN";
    }
}

Lexer::Lexer(std::string source) : src(std::move(source)), pos(0), line(1) {}

char Lexer::cur() const {
    return pos < src.size() ? src[pos] : '\0';
}

char Lexer::peek(int off) const {
    size_t p = pos + off;
    return p < src.size() ? src[p] : '\0';
}

void Lexer::adv() {
    if (pos < src.size()) {
        if (src[pos] == '\n') line++;
        pos++;
    }
}

void Lexer::skipWS() {
    while (pos < src.size() && std::isspace(cur())) adv();
}

void Lexer::skipLineComment() {
    while (pos < src.size() && cur() != '\n') adv();
}

Token Lexer::readNumber() {
    std::string num;
    int sl = line;
    while (pos < src.size() && std::isdigit(cur())) { num += cur(); adv(); }
    return {TokenType::NUMBER, num, sl};
}

Token Lexer::readWord() {
    std::string word;
    int sl = line;
    while (pos < src.size() && (std::isalnum(cur()) || cur() == '_')) { word += cur(); adv(); }
    if (word == "let")   return {TokenType::LET,      word, sl};
    if (word == "if")    return {TokenType::IF,        word, sl};
    if (word == "else")  return {TokenType::ELSE,      word, sl};
    if (word == "while") return {TokenType::WHILE,     word, sl};
    if (word == "print") return {TokenType::PRINT,     word, sl};
    if (word == "input") return {TokenType::INPUT,     word, sl};
    if (word == "true")  return {TokenType::TRUE_KW,   word, sl};
    if (word == "false") return {TokenType::FALSE_KW,  word, sl};
    return {TokenType::IDENTIFIER, word, sl};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < src.size()) {
        skipWS();
        if (pos >= src.size()) break;

        int sl = line;
        char c = cur();

        if (c == '/' && peek() == '/') { skipLineComment(); continue; }
        if (std::isdigit(c)) { tokens.push_back(readNumber()); continue; }
        if (std::isalpha(c) || c == '_') { tokens.push_back(readWord()); continue; }

        adv();
        switch (c) {
            case '+': tokens.push_back({TokenType::PLUS,      "+", sl}); break;
            case '-': tokens.push_back({TokenType::MINUS,     "-", sl}); break;
            case '*': tokens.push_back({TokenType::STAR,      "*", sl}); break;
            case '/': tokens.push_back({TokenType::SLASH,     "/", sl}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", sl}); break;
            case '(': tokens.push_back({TokenType::LPAREN,    "(", sl}); break;
            case ')': tokens.push_back({TokenType::RPAREN,    ")", sl}); break;
            case '{': tokens.push_back({TokenType::LBRACE,    "{", sl}); break;
            case '}': tokens.push_back({TokenType::RBRACE,    "}", sl}); break;
            case '=':
                if (cur() == '=') { adv(); tokens.push_back({TokenType::EQEQ, "==", sl}); }
                else              {        tokens.push_back({TokenType::EQ,   "=",  sl}); }
                break;
            case '!':
                if (cur() == '=') { adv(); tokens.push_back({TokenType::NEQ, "!=", sl}); }
                else throw std::runtime_error("Unexpected '!' at line " + std::to_string(sl));
                break;
            case '<':
                if (cur() == '=') { adv(); tokens.push_back({TokenType::LTE, "<=", sl}); }
                else              {        tokens.push_back({TokenType::LT,  "<",  sl}); }
                break;
            case '>':
                if (cur() == '=') { adv(); tokens.push_back({TokenType::GTE, ">=", sl}); }
                else              {        tokens.push_back({TokenType::GT,  ">",  sl}); }
                break;
            default:
                throw std::runtime_error(
                    std::string("Unexpected character '") + c + "' at line " + std::to_string(sl));
        }
    }
    tokens.push_back({TokenType::EOF_TOKEN, "", line});
    return tokens;
}
