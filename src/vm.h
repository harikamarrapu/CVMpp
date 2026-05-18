#pragma once
#include "compiler.h"
#include <vector>
#include <string>

struct Value {
    enum class Tag { INT, BOOL } tag;
    union { int ival; bool bval; };
    Value() : tag(Tag::INT), ival(0) {}
    explicit Value(int  v) : tag(Tag::INT),  ival(v) {}
    explicit Value(bool v) : tag(Tag::BOOL), bval(v) {}
    bool isInt()  const { return tag == Tag::INT;  }
    bool isBool() const { return tag == Tag::BOOL; }
};

std::string valueToString(const Value& v);

class VM {
public:
    void execute(const std::vector<Instruction>& code,
                 int numVars,
                 bool debugMode = false);

private:
    std::vector<Value> stack;
    std::vector<Value> vars;

    void  push(Value v);
    Value pop();
};
