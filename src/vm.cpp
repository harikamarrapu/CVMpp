#include "vm.h"
#include <iostream>
#include <stdexcept>

std::string valueToString(const Value& v) {
    if (v.isInt())  return std::to_string(v.ival);
    if (v.isBool()) return v.bval ? "true" : "false";
    return "?";
}

void VM::push(Value v) { stack.push_back(v); }

Value VM::pop() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    Value v = stack.back();
    stack.pop_back();
    return v;
}

void VM::execute(const std::vector<Instruction>& code, int numVars, bool debugMode) {
    vars.assign(numVars, Value(0));
    stack.clear();

    int ip = 0;
    while (ip < (int)code.size()) {
        const Instruction& instr = code[ip];

        if (debugMode) {
            std::cout << "  [" << ip << "] " << opcodeName(instr.op);
            switch (instr.op) {
                case Opcode::PUSH_INT: case Opcode::PUSH_BOOL:
                case Opcode::LOAD:     case Opcode::STORE:
                case Opcode::JUMP:     case Opcode::JUMP_IF_FALSE:
                case Opcode::INPUT:
                    std::cout << " " << instr.arg; break;
                default: break;
            }
            std::cout << "\n";
        }

        switch (instr.op) {
            case Opcode::PUSH_INT:
                push(Value(instr.arg)); break;

            case Opcode::PUSH_BOOL:
                push(Value(instr.arg != 0)); break;

            case Opcode::LOAD:
                push(vars[instr.arg]); break;

            case Opcode::STORE:
                vars[instr.arg] = pop(); break;

            case Opcode::ADD: { Value b = pop(), a = pop(); push(Value(a.ival + b.ival)); break; }
            case Opcode::SUB: { Value b = pop(), a = pop(); push(Value(a.ival - b.ival)); break; }
            case Opcode::MUL: { Value b = pop(), a = pop(); push(Value(a.ival * b.ival)); break; }
            case Opcode::DIV: {
                Value b = pop(), a = pop();
                if (b.ival == 0) throw std::runtime_error("Division by zero");
                push(Value(a.ival / b.ival)); break;
            }
            case Opcode::NEGATE: { Value a = pop(); push(Value(-a.ival)); break; }

            case Opcode::EQ: {
                Value b = pop(), a = pop();
                push(Value(a.isInt() ? a.ival == b.ival : a.bval == b.bval)); break;
            }
            case Opcode::NEQ: {
                Value b = pop(), a = pop();
                push(Value(a.isInt() ? a.ival != b.ival : a.bval != b.bval)); break;
            }
            case Opcode::LT:  { Value b = pop(), a = pop(); push(Value(a.ival <  b.ival)); break; }
            case Opcode::LTE: { Value b = pop(), a = pop(); push(Value(a.ival <= b.ival)); break; }
            case Opcode::GT:  { Value b = pop(), a = pop(); push(Value(a.ival >  b.ival)); break; }
            case Opcode::GTE: { Value b = pop(), a = pop(); push(Value(a.ival >= b.ival)); break; }

            case Opcode::JUMP:
                ip = instr.arg; continue;

            case Opcode::JUMP_IF_FALSE: {
                Value cond = pop();
                bool taken = cond.isBool() ? !cond.bval : (cond.ival == 0);
                if (taken) { ip = instr.arg; continue; }
                break;
            }

            case Opcode::PRINT:
                std::cout << valueToString(pop()) << "\n"; break;

            case Opcode::INPUT: {
                std::string line;
                std::getline(std::cin, line);
                try { vars[instr.arg] = Value(std::stoi(line)); }
                catch (...) {
                    if      (line == "true")  vars[instr.arg] = Value(true);
                    else if (line == "false") vars[instr.arg] = Value(false);
                    else throw std::runtime_error("Invalid input: '" + line + "'");
                }
                break;
            }

            case Opcode::HALT: return;

            default:
                throw std::runtime_error("Unknown opcode");
        }
        ip++;
    }
}
