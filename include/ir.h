#pragma once

#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

namespace ir {
    enum Type {
        // Test
        IR,

        // Control flow
        Scope,
        Branch, While, Break, Continue,

        // Functions
        Call, Return, Arg,

        // Variables
        Decl, Var, Set,

        // Operators
        Add, Sub, Mul, Div, Mod,
        And, Or, Xor, Not,
        Shl, Shr,
        Eq, Neq, Lt, Gt, Leq, Geq,

        // Literals
        Int,
        Float,
        String,
        Bool,
    };

    class Instruction {
    public:
        union {
            int i;
            float f;
            std::string* s;
            bool b;
            Instruction* ir;
        } value;

        Instruction(Type instruction, std::vector<Instruction*> args);

        Instruction(Type instruction, std::string value, std::vector<Instruction*> args);
        
        Instruction(Type instruction, int value, std::vector<Instruction*> args);

        ~Instruction();

        inline Type GetInstruction() const { return instruction; }
        
        inline std::vector<Instruction*> GetArgs() { return args; }

        inline Instruction* GetArg(int index) const { return args[index]; }
        
    private:
        Type instruction;

        std::vector<Instruction*> args;
    };

    class Mainfold {
    public:
        enum Type {
            Int,
            Float,
            String,
            Bool,
            Object,
            Scope,
            None
        } type;

        union {
            int i;
            float f;
            std::string* s;
            bool b;
            Instruction* ir;
        } value;

        Mainfold() { this->type = None; }
        Mainfold(Type type, int value) { this->type = type; this->value.i = value; }
        Mainfold(Type type, float value) { this->type = type; this->value.f = value; }
        Mainfold(Type type, std::string* value) { this->type = type; this->value.s = value; }
        Mainfold(Type type, bool value) { this->type = type; this->value.b = value; }
        Mainfold(Type type, Instruction* value) { this->type = type; this->value.ir = value; }
        Mainfold(Type type) { this->type = type; }
    };

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void Set(std::string name, Mainfold value) { symbols[name] = value; }

        inline Mainfold Get(std::string name);

        inline void SetParent(SymbolTable* parent) { this->parent = parent; }

        inline SymbolTable* GetParent() { return parent; }

    private:
        SymbolTable* parent = nullptr;

        std::unordered_map<std::string, Mainfold> symbols;
    };

    class Context {
    public:
        inline void Add(SymbolTable* symbolTable) { tables.push_back(symbolTable); }

        inline SymbolTable* GetCurrent() { return current; }

        inline void SetCurrent(SymbolTable* symbolTable) { current = symbolTable; }

    private:
        std::vector<SymbolTable*> tables;

        SymbolTable* current;
    };

    class Interpreter {
    public:
        void Interpret(std::vector<Instruction*> instructions);

        Mainfold Interpret(Instruction* instruction);
        
    private:
        Context context;
        std::stack<std::vector<Mainfold>> stack;
    };
}