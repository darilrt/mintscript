#pragma once

#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <functional>

namespace ir {
    enum Type {
        // Test
        IR,

        // Control flow
        Scope,
        Loop, Break, Continue,
        If,

        // Functions
        Call, Return, Arg, Native,

        // Variables
        Decl, Var, Set, Val,

        // Objects
        New, Field,

        // Operators
        AddI, SubI, MulI, DivI, ModI,
        AddF, SubF, MulF, DivF, ModF,
        And, Or, Xor, Not,
        Shl, Shr,
        EqI, NeqI, LtI, GtI, LeqI, GeqI,
        EqF, NeqF, LtF, GtF, LeqF, GeqF,

        // Literals
        Int,
        Float,
        String,
        Bool,
        Null,

        // Casts
        IntToFloat,
        FloatToInt,
        BoolToInt,
        IntToBool,
        FloatToBool,
        BoolToFloat
    };

    class Mainfold;

    class Instruction {
    public:
        union {
            int i;
            float f;
            std::string* s;
            bool b;
            Instruction* ir;
            Mainfold (*native)(std::vector<Mainfold>);
        } value;

        Instruction(Type instruction, std::vector<Instruction*> args);

        Instruction(Type instruction, std::string value, std::vector<Instruction*> args);
        
        Instruction(Type instruction, int value, std::vector<Instruction*> args);

        Instruction(Type instruction, float value, std::vector<Instruction*> args);

        Instruction(Type instruction, Mainfold (*value)(std::vector<Mainfold>), std::vector<Instruction*> args);

        ~Instruction();

        inline Type GetInstruction() { return instruction; }

        inline void SetInstruction(Type instruction) { this->instruction = instruction; }
        
        inline std::vector<Instruction*>& GetArgs() { return args; }

        inline Instruction* GetArg(int index) { return args[index]; }
        
    private:
        Type instruction;

        std::vector<Instruction*> args;
    };

    class Object {
    public:
        std::vector<Mainfold> fields;

        Object(int size) { fields.resize(size); }
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
            Native,
            Null
        } type;

        union {
            bool b;
            int i;
            float f;
            std::string* s;
            Instruction* ir;
            ir::Object* st;
            Mainfold* mf;
            Mainfold (*native)(std::vector<Mainfold>);
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
        Mainfold(Type type, Mainfold (*value)(std::vector<Mainfold>)) { this->type = type; this->value.native = value; }
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
        enum class State {
            None,
            Continue,
            Break,
            Return
        };

        Interpreter();
        ~Interpreter();

        Mainfold Interpret(Instruction* instruction);

        void Print(Instruction* instruction, int indent = 0);
        
    private:
        State state = State::None;
        Context context;
        std::stack<std::vector<Mainfold>> stack;
    };

    extern ir::Instruction* global;
}
