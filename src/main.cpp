#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

static void printAST(const ASTNode& node, int indent = 0) {
    std::string pad(indent * 2, ' ');
    if (auto* n = dynamic_cast<const Program*>(&node)) {
        std::cout << pad << "Program\n";
        for (auto& s : n->stmts) printAST(*s, indent + 1);
    } else if (auto* n = dynamic_cast<const VarDecl*>(&node)) {
        std::cout << pad << "VarDecl(" << n->name << ")\n";
        printAST(*n->init, indent + 1);
    } else if (auto* n = dynamic_cast<const Assign*>(&node)) {
        std::cout << pad << "Assign(" << n->name << ")\n";
        printAST(*n->val, indent + 1);
    } else if (auto* n = dynamic_cast<const Block*>(&node)) {
        std::cout << pad << "Block\n";
        for (auto& s : n->stmts) printAST(*s, indent + 1);
    } else if (auto* n = dynamic_cast<const IfStmt*>(&node)) {
        std::cout << pad << "If\n";
        std::cout << pad << "  [cond]\n"; printAST(*n->cond,  indent + 2);
        std::cout << pad << "  [then]\n"; printAST(*n->then_, indent + 2);
        if (n->else_) { std::cout << pad << "  [else]\n"; printAST(*n->else_, indent + 2); }
    } else if (auto* n = dynamic_cast<const WhileStmt*>(&node)) {
        std::cout << pad << "While\n";
        std::cout << pad << "  [cond]\n"; printAST(*n->cond, indent + 2);
        std::cout << pad << "  [body]\n"; printAST(*n->body, indent + 2);
    } else if (auto* n = dynamic_cast<const PrintStmt*>(&node)) {
        std::cout << pad << "Print\n"; printAST(*n->expr, indent + 1);
    } else if (auto* n = dynamic_cast<const InputStmt*>(&node)) {
        std::cout << pad << "Input(" << n->varName << ")\n";
    } else if (auto* n = dynamic_cast<const BinExpr*>(&node)) {
        std::cout << pad << "BinExpr(" << n->op << ")\n";
        printAST(*n->left, indent + 1); printAST(*n->right, indent + 1);
    } else if (auto* n = dynamic_cast<const UnaryExpr*>(&node)) {
        std::cout << pad << "UnaryExpr(" << n->op << ")\n";
        printAST(*n->operand, indent + 1);
    } else if (auto* n = dynamic_cast<const NumberLit*>(&node)) {
        std::cout << pad << "Number(" << n->val << ")\n";
    } else if (auto* n = dynamic_cast<const BoolLit*>(&node)) {
        std::cout << pad << "Bool(" << (n->val ? "true" : "false") << ")\n";
    } else if (auto* n = dynamic_cast<const Ident*>(&node)) {
        std::cout << pad << "Ident(" << n->name << ")\n";
    }
}

static void printBytecode(const std::vector<Instruction>& code,
                           const std::vector<std::string>& varNames) {
    std::cout << "\n--- Bytecode ---\n";
    for (int i = 0; i < (int)code.size(); i++) {
        const auto& ins = code[i];
        std::cout << std::setw(4) << i << "  "
                  << std::left << std::setw(16) << opcodeName(ins.op);
        switch (ins.op) {
            case Opcode::PUSH_INT:
                std::cout << ins.arg; break;
            case Opcode::PUSH_BOOL:
                std::cout << (ins.arg ? "true" : "false"); break;
            case Opcode::LOAD: case Opcode::STORE: case Opcode::INPUT:
                std::cout << ins.arg;
                if (ins.arg < (int)varNames.size())
                    std::cout << "  ; " << varNames[ins.arg];
                break;
            case Opcode::JUMP: case Opcode::JUMP_IF_FALSE:
                std::cout << ins.arg; break;
            default: break;
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n\n";
}

static void runSource(const std::string& src, bool showAST, bool showBytecode, bool debugVM) {
    Lexer lexer(src);
    auto tokens = lexer.tokenize();
    




    Parser parser(tokens);
    auto program = parser.parse();

    if (showAST) {
        std::cout << "\n--- AST ---\n";
        printAST(*program);
        std::cout << "-----------\n\n";
    }

    Compiler compiler;
    auto bytecode = compiler.compile(*program);

    if (showBytecode) printBytecode(bytecode, compiler.varNames());

    VM vm;
    vm.execute(bytecode, (int)compiler.varNames().size(), debugVM);
}

static void runREPL(bool showAST, bool showBytecode, bool debugVM) {
    std::cout << "CVM++ REPL  (type 'exit' to quit)\n";
    std::string line, block;
    int openBraces = 0;

    while (true) {
        std::cout << (openBraces > 0 ? "... " : ">>> ");
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;

        block += line + "\n";
        for (char c : line) {
            if (c == '{') openBraces++;
            if (c == '}') openBraces--;
        }

        if (openBraces <= 0) {
            try { runSource(block, showAST, showBytecode, debugVM); }
            catch (const std::exception& e) { std::cerr << "Error: " << e.what() << "\n"; }
            block.clear();
            openBraces = 0;
        }
    }
}

int main(int argc, char* argv[]) {
    bool showAST = false, showBytecode = false, debugVM = false;
    std::string filename;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if      (arg == "--ast")      showAST      = true;
        else if (arg == "--bytecode") showBytecode = true;
        else if (arg == "--debug")    debugVM      = true;
        else if (arg == "--help") {
            std::cout << "Usage: cvm [options] [script.cvm]\n"
                      << "  --ast       Print the Abstract Syntax Tree\n"
                      << "  --bytecode  Print compiled bytecode\n"
                      << "  --debug     Trace VM execution\n"
                      << "  (no file)   Launch interactive REPL\n";
            return 0;
        } else {
            filename = arg;
        }
    }

    if (filename.empty()) {
        runREPL(showAST, showBytecode, debugVM);
    } else {
        std::ifstream f(filename);
        if (!f) { std::cerr << "Cannot open '" << filename << "'\n"; return 1; }
        std::ostringstream ss; ss << f.rdbuf();
        try { runSource(ss.str(), showAST, showBytecode, debugVM); }
        catch (const std::exception& e) { std::cerr << "Error: " << e.what() << "\n"; return 1; }
    }
    return 0;
}
