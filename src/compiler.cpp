#include "compiler.h"
#include <stdexcept>

std::string opcodeName(Opcode op) {
    switch (op) {
        case Opcode::PUSH_INT:      return "PUSH_INT";
        case Opcode::PUSH_BOOL:     return "PUSH_BOOL";
        case Opcode::LOAD:          return "LOAD";
        case Opcode::STORE:         return "STORE";
        case Opcode::ADD:           return "ADD";
        case Opcode::SUB:           return "SUB";
        case Opcode::MUL:           return "MUL";
        case Opcode::DIV:           return "DIV";
        case Opcode::EQ:            return "EQ";
        case Opcode::NEQ:           return "NEQ";
        case Opcode::LT:            return "LT";
        case Opcode::LTE:           return "LTE";
        case Opcode::GT:            return "GT";
        case Opcode::GTE:           return "GTE";
        case Opcode::NEGATE:        return "NEGATE";
        case Opcode::JUMP:          return "JUMP";
        case Opcode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case Opcode::PRINT:         return "PRINT";
        case Opcode::INPUT:         return "INPUT";
        case Opcode::HALT:          return "HALT";
        default:                    return "???";
    }
}

int Compiler::emit(Opcode op, int32_t arg) {
    code.push_back({op, arg});
    return (int)code.size() - 1;
}

void Compiler::patch(int idx, int32_t target) {
    code[idx].arg = target;
}

int Compiler::declareVar(const std::string& name) {
    auto it = varMap.find(name);
    if (it != varMap.end()) return it->second;
    int idx = (int)varList.size();
    varMap[name] = idx;
    varList.push_back(name);
    return idx;
}

int Compiler::resolveVar(const std::string& name) {
    auto it = varMap.find(name);
    if (it == varMap.end())
        throw std::runtime_error("Undefined variable '" + name + "'");
    return it->second;
}

std::vector<Instruction> Compiler::compile(const Program& prog) {
    code.clear();
    varMap.clear();
    varList.clear();
    for (auto& stmt : prog.stmts) compileNode(*stmt);
    emit(Opcode::HALT);
    return code;
}

void Compiler::compileNode(const ASTNode& node) {
    if (auto* n = dynamic_cast<const VarDecl*>(&node)) {
        compileExpr(*n->init);
        emit(Opcode::STORE, declareVar(n->name));
        return;
    }
    if (auto* n = dynamic_cast<const Assign*>(&node)) {
        compileExpr(*n->val);
        emit(Opcode::STORE, resolveVar(n->name));
        return;
    }
    if (auto* n = dynamic_cast<const Block*>(&node)) {
        for (auto& s : n->stmts) compileNode(*s);
        return;
    }
    if (auto* n = dynamic_cast<const IfStmt*>(&node)) {
        compileExpr(*n->cond);
        int jmpFalse = emit(Opcode::JUMP_IF_FALSE);
        compileNode(*n->then_);
        if (n->else_) {
            int jmpEnd = emit(Opcode::JUMP);
            patch(jmpFalse, (int32_t)code.size());
            compileNode(*n->else_);
            patch(jmpEnd, (int32_t)code.size());
        } else {
            patch(jmpFalse, (int32_t)code.size());
        }
        return;
    }
    if (auto* n = dynamic_cast<const WhileStmt*>(&node)) {
        int loopStart = (int)code.size();
        compileExpr(*n->cond);
        int jmpFalse = emit(Opcode::JUMP_IF_FALSE);
        compileNode(*n->body);
        emit(Opcode::JUMP, loopStart);
        patch(jmpFalse, (int32_t)code.size());
        return;
    }
    if (auto* n = dynamic_cast<const PrintStmt*>(&node)) {
        compileExpr(*n->expr);
        emit(Opcode::PRINT);
        return;
    }
    if (auto* n = dynamic_cast<const InputStmt*>(&node)) {
        emit(Opcode::INPUT, resolveVar(n->varName));
        return;
    }
    compileExpr(node);
}

void Compiler::compileExpr(const ASTNode& node) {
    if (auto* n = dynamic_cast<const NumberLit*>(&node)) {
        emit(Opcode::PUSH_INT, n->val);
        return;
    }
    if (auto* n = dynamic_cast<const BoolLit*>(&node)) {
        emit(Opcode::PUSH_BOOL, n->val ? 1 : 0);
        return;
    }
    if (auto* n = dynamic_cast<const Ident*>(&node)) {
        emit(Opcode::LOAD, resolveVar(n->name));
        return;
    }
    if (auto* n = dynamic_cast<const UnaryExpr*>(&node)) {
        compileExpr(*n->operand);
        if (n->op == "-") emit(Opcode::NEGATE);
        return;
    }
    if (auto* n = dynamic_cast<const BinExpr*>(&node)) {
        compileExpr(*n->left);
        compileExpr(*n->right);
        if      (n->op == "+")  emit(Opcode::ADD);
        else if (n->op == "-")  emit(Opcode::SUB);
        else if (n->op == "*")  emit(Opcode::MUL);
        else if (n->op == "/")  emit(Opcode::DIV);
        else if (n->op == "==") emit(Opcode::EQ);
        else if (n->op == "!=") emit(Opcode::NEQ);
        else if (n->op == "<")  emit(Opcode::LT);
        else if (n->op == "<=") emit(Opcode::LTE);
        else if (n->op == ">")  emit(Opcode::GT);
        else if (n->op == ">=") emit(Opcode::GTE);
        return;
    }
    throw std::runtime_error("Unknown expression node in compiler");
}
