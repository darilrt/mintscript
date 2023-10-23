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
        While, Break, Continue,
        If,

        // Functions
        Call, Return, Arg,

        // Variables
        Decl, Var, Set,

        // Objects
        New, Field,

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
        Null
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

        Instruction(Type instruction, float value, std::vector<Instruction*> args);

        ~Instruction();

        inline Type GetInstruction() { return instruction; }

        inline void SetInstruction(Type instruction) { this->instruction = instruction; }
        
        inline std::vector<Instruction*>& GetArgs() { return args; }

        inline Instruction* GetArg(int index) { return args[index]; }
        
    private:
        Type instruction;

        std::vector<Instruction*> args;
    };

    class Mainfold;

    class Object {
    public:
        std::vector<Mainfold> fields;

        Object(int size) { fields.reserve(size); }
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
            Field,
            Null
        } type;

        union {
            int i;
            float f;
            std::string* s;
            bool b;
            Instruction* ir;
            ir::Object* st;
            Mainfold* mf;
        } value;

        Mainfold() { this->type = Null; }
        Mainfold(Type type) { this->type = type; }
        Mainfold(Type type, int value) { this->type = type; this->value.i = value; }
        Mainfold(Type type, float value) { this->type = type; this->value.f = value; }
        Mainfold(Type type, std::string* value) { this->type = type; this->value.s = value; }
        Mainfold(Type type, bool value) { this->type = type; this->value.b = value; }
        Mainfold(Type type, Instruction* value) { this->type = type; this->value.ir = value; }
        Mainfold(Type type, ir::Object* value) { this->type = type; this->value.st = value; }
        Mainfold(Type type, Mainfold* value) { this->type = type; this->value.mf = value; }
    };

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void Set(std::string name) { symbols[name] = { Mainfold::Null, 0 }; }

        inline Mainfold& Get(std::string name);

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