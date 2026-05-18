#pragma once
#include "ast.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

enum class Opcode : uint8_t {
    PUSH_INT,
    PUSH_BOOL,
    LOAD,
    STORE,
    ADD, SUB, MUL, DIV,
    EQ, NEQ, LT, LTE, GT, GTE,
    NEGATE,
    JUMP,
    JUMP_IF_FALSE,
    PRINT,
    INPUT,
    HALT
};

std::string opcodeName(Opcode op);

struct Instruction {
    Opcode  op;
    int32_t arg = 0;
};

class Compiler {
public:
    std::vector<Instruction> compile(const Program& prog);
    const std::vector<std::string>& varNames() const { return varList; }

private:
    std::vector<Instruction>            code;
    std::unordered_map<std::string,int> varMap;
    std::vector<std::string>            varList;

    int  emit(Opcode op, int32_t arg = 0);
    void patch(int idx, int32_t target);
    int  declareVar(const std::string& name);
    int  resolveVar(const std::string& name);

    void compileNode(const ASTNode& node);
    void compileExpr(const ASTNode& node);
};
