#include "ir.h"

ir::Instruction::Instruction(Type instruction, std::vector<Instruction *> args) {
    this->instruction = instruction;
    this->args = args;
}

ir::Instruction::Instruction(Type instruction, std::string value, std::vector<Instruction *> args) {
    this->instruction = instruction;
    this->args = args;
    this->value.s = new std::string(value);
}

ir::Instruction::Instruction(Type instruction, int value, std::vector<Instruction *> args) {
    this->instruction = instruction;
    this->args = args;
    this->value.i = value;
}

ir::Instruction::~Instruction()  {
    for (int i = 0; i < args.size(); i++) {
        delete args[i];
    }
}

void ir::Interpreter::Interpret(std::vector<Instruction *> instructions) {
    for (int i = 0; i < instructions.size(); i++) {
        Mainfold mf = Interpret(instructions[i]);

        switch (mf.type) {
            case Mainfold::Int: std::cout << mf.value.i << std::endl; break;
            case Mainfold::Float: std::cout << mf.value.f << std::endl; break;
            case Mainfold::String: std::cout << *mf.value.s << std::endl; break;
            case Mainfold::Bool: std::cout << (mf.value.b ? "true" : "false") << std::endl; break;
            case Mainfold::Object: std::cout << mf.value.i << std::endl; break;

            default:
                break;
        }
    }
}

ir::Interpreter::Mainfold ir::Interpreter::Interpret(Instruction *instruction) {
    switch (instruction->GetInstruction()) {
        case Add: {
            const int a = instruction->GetArgs()[0]->value.i;
            const int b = instruction->GetArgs()[1]->value.i;

            return { Mainfold::Int, { a + b } };
        }

        case Set: {
            const std::string name = *instruction->value.s;
            const Mainfold value = Interpret(instruction->GetArg(0));

            return { Mainfold::None };
        }

        case Var: {
            return { Mainfold::String, { instruction->value.s } };
        }

        default:
            break;
    }

    return { Mainfold::None };
}