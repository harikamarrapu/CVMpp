# CVM++ — Stack-Based Virtual Machine & Custom Compiler

A lightweight scripting language written in **C++14** that compiles source code
down to a custom bytecode format, executed by a stack-based Virtual Machine.

---

## Project Structure

```
cvm++/
├── src/
│   ├── ast.h           – AST node class hierarchy
│   ├── lexer.h/.cpp    – Tokeniser
│   ├── parser.h/.cpp   – Recursive descent parser
│   ├── compiler.h/.cpp – Bytecode compiler
│   ├── vm.h/.cpp       – Stack-based Virtual Machine
│   └── main.cpp        – REPL and file runner
├── scripts/
│   ├── hello.cvm
│   ├── fibonacci.cvm
│   ├── counter.cvm
│   ├── booleans.cvm
│   └── input_demo.cvm
├── CMakeLists.txt
└── README.md
```

---

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

## Usage

### Run a script
```bash
cvm scripts/fibonacci.cvm
```

### REPL
```bash
cvm
>>> let x = 5;
>>> print x * 3;
15
>>> exit
```

### Flags
| Flag | Description |
|---|---|
| `--ast` | Print the Abstract Syntax Tree |
| `--bytecode` | Print compiled bytecode listing |
| `--debug` | Trace VM execution step by step |

---

## Language Reference

### Variables
```
let x = 10;
x = x + 1;
```

### Data Types
- Integers: `0`, `42`, `-5`
- Booleans: `true`, `false`

### Operators
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Unary: `-`

### Control Flow
```
if (x < 10) {
    print x;
} else {
    print 0;
}

while (x > 0) {
    x = x - 1;
}
```

### I/O
```
print x + 1;
input y;        // reads from stdin into y (must be declared first)
```

### Comments
```
// single-line comment
```

---

## Instruction Set

| Opcode | Argument | Description |
|---|---|---|
| `PUSH_INT` | value | Push integer onto stack |
| `PUSH_BOOL` | 0 or 1 | Push boolean onto stack |
| `LOAD` | index | Load variable onto stack |
| `STORE` | index | Pop stack into variable |
| `ADD/SUB/MUL/DIV` | — | Arithmetic operations |
| `EQ/NEQ/LT/LTE/GT/GTE` | — | Comparison, pushes bool |
| `NEGATE` | — | Unary minus |
| `JUMP` | address | Unconditional jump |
| `JUMP_IF_FALSE` | address | Jump if top of stack is false |
| `PRINT` | — | Pop and print |
| `INPUT` | index | Read stdin into variable |
| `HALT` | — | End execution |

---

## Architecture

```
Source (.cvm)
    |
    v
  Lexer         -> token stream
    |
    v
  Parser        -> Abstract Syntax Tree
    |
    v
  Compiler      -> bytecode (vector<Instruction>)
    |
    v
  VM            -> output
```
