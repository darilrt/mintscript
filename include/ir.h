#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

namespace ir {
    enum Type {
        // Control flow
        Scope,
        Branch, While, Break, Continue,

        // Functions
        Call,
        Return,

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
        
        inline std::vector<Instruction*> GetArgs() const { return args; }

        inline Instruction* GetArg(int index) const { return args[index]; }
            
    private:
        Type instruction;
        std::vector<Instruction*> args;
    };

    class Interpreter {
    public:
        class Mainfold {
        public:
            enum Type {
                Int,
                Float,
                String,
                Bool,
                Object,
                IR,
                None
            } type;

            union {
                int i;
                float f;
                std::string* s;
                bool b;
                Instruction* ir;
            } value;

            Mainfold(Type type, int value) { this->type = type; this->value.i = value; }
            Mainfold(Type type, float value) { this->type = type; this->value.f = value; }
            Mainfold(Type type, std::string* value) { this->type = type; this->value.s = value; }
            Mainfold(Type type, bool value) { this->type = type; this->value.b = value; }
            Mainfold(Type type, Instruction* value) { this->type = type; this->value.ir = value; }
            Mainfold(Type type) { this->type = type; }
        };

        void Interpret(std::vector<Instruction*> instructions);

        Mainfold Interpret(Instruction* instruction);
        
    private:
    };
}