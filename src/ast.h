#pragma once
#include <memory>
#include <vector>
#include <string>

struct ASTNode { virtual ~ASTNode() = default; };
using ASTNodePtr = std::unique_ptr<ASTNode>;

struct NumberLit : ASTNode {
    int val;
    explicit NumberLit(int v) : val(v) {}
};

struct BoolLit : ASTNode {
    bool val;
    explicit BoolLit(bool v) : val(v) {}
};

struct Ident : ASTNode {
    std::string name;
    explicit Ident(std::string n) : name(std::move(n)) {}
};

struct BinExpr : ASTNode {
    std::string op;
    ASTNodePtr left, right;
    BinExpr(std::string o, ASTNodePtr l, ASTNodePtr r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
};

struct UnaryExpr : ASTNode {
    std::string op;
    ASTNodePtr operand;
    UnaryExpr(std::string o, ASTNodePtr x)
        : op(std::move(o)), operand(std::move(x)) {}
};

struct VarDecl : ASTNode {
    std::string name;
    ASTNodePtr init;
    VarDecl(std::string n, ASTNodePtr i) : name(std::move(n)), init(std::move(i)) {}
};

struct Assign : ASTNode {
    std::string name;
    ASTNodePtr val;
    Assign(std::string n, ASTNodePtr v) : name(std::move(n)), val(std::move(v)) {}
};

struct Block : ASTNode {
    std::vector<ASTNodePtr> stmts;
};

struct IfStmt : ASTNode {
    ASTNodePtr cond, then_, else_;
    IfStmt(ASTNodePtr c, ASTNodePtr t, ASTNodePtr e)
        : cond(std::move(c)), then_(std::move(t)), else_(std::move(e)) {}
};

struct WhileStmt : ASTNode {
    ASTNodePtr cond, body;
    WhileStmt(ASTNodePtr c, ASTNodePtr b) : cond(std::move(c)), body(std::move(b)) {}
};

struct PrintStmt : ASTNode {
    ASTNodePtr expr;
    explicit PrintStmt(ASTNodePtr e) : expr(std::move(e)) {}
};

struct InputStmt : ASTNode {
    std::string varName;
    explicit InputStmt(std::string n) : varName(std::move(n)) {}
};

struct Program : ASTNode {
    std::vector<ASTNodePtr> stmts;
};
