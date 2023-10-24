#include "ir.h"

#include <cmath>

void PrintMainfold(ir::Mainfold mf) {
    switch (mf.type) {
        case ir::Mainfold::Int: std::cout << mf.value.i; break;
        case ir::Mainfold::Float: std::cout << mf.value.f; break;
        case ir::Mainfold::String: std::cout << *mf.value.s; break;
        case ir::Mainfold::Bool: std::cout << (mf.value.b ? "true" : "false"); break;
        case ir::Mainfold::Null: std::cout << "Null"; break;
        case ir::Mainfold::Field: std::cout << "{field." << mf.value.mf << "}"; break;
        case ir::Mainfold::Object: std::cout << "{object." << mf.value.st << "}"; break;
        default: break;
    }
}

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

ir::Instruction::Instruction(Type instruction, float value, std::vector<Instruction *> args) {
    this->instruction = instruction;
    this->args = args;
    this->value.f = value;
}

ir::Instruction::~Instruction() {
    for (int i = 0; i < args.size(); i++) {
        delete args[i];
    }
}

void ir::Interpreter::Interpret(std::vector<Instruction *> instructions) {
    Mainfold mf = { Mainfold::Null };

    SymbolTable* global = new SymbolTable();
    context.Add(global);
    context.SetCurrent(global);

    for (int i = 0; i < instructions.size(); i++) {
        mf = Interpret(instructions[i]); 
    }

    context.SetCurrent(nullptr);
    delete global;
}

#define ARG(i) Interpret(instruction->GetArg(i))

ir::Mainfold ir::Interpreter::Interpret(Instruction *instruction) {
    switch (instruction->GetInstruction()) {
        case IR: { 
            return { Mainfold::Scope, new Instruction(ir::Scope, instruction->GetArgs()) };
        }

        case If: {
            Mainfold ret = { Mainfold::Null };
            
            if (ARG(0).value.b) {
                ret = Interpret(instruction->GetArg(1));
            }
            else if (instruction->GetArgs().size() > 2) {
                ret = Interpret(instruction->GetArg(2));
            }
            
            return ret;
        }

        case Call: {
            Mainfold name = ARG(0);

            if (name.type != Mainfold::String) {
                const std::vector<Instruction*> args = instruction->GetArgs();

                std::vector<Mainfold> argv;
                argv.resize(args.size() - 1);

                for (int i = 1; i < args.size(); i++) {
                    argv[i - 1] = Interpret(args[i]);
                }

                stack.push(argv);
                
                Mainfold* mf = ARG(0).value.mf;
                Mainfold ret = { Mainfold::Null };

                if (mf->type == Mainfold::Scope) {
                    ret = Interpret(mf->value.ir);
                }

                stack.pop();

                return ret;
            }
            else {
                if (*name.value.s == "print") {
                    if (instruction->GetArgs().size() == 1) {
                        std::cout << "Expected at least one argument to print" << std::endl;

                        return { Mainfold::Null };
                    }

                    for (int i = 1; i < instruction->GetArgs().size(); i++) {
                        Mainfold mf = Interpret(instruction->GetArg(i));
                        
                        if (mf.type == Mainfold::Field) {
                            PrintMainfold(*mf.value.mf);
                        }
                        else {
                            PrintMainfold(mf);
                        }

                        std::cout << " ";
                    }

                    std::cout << std::endl;
                }
            }

            return { Mainfold::Null };
        }

        case Arg: { return stack.top()[instruction->value.i]; }

        case Scope: {
            SymbolTable* scope = new SymbolTable(context.GetCurrent());
            context.Add(scope);
            context.SetCurrent(scope);

            Mainfold mf = { Mainfold::Null };

            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                ir::Instruction *arg = instruction->GetArg(i);

                if (arg->GetInstruction() == ir::Type::Return) {
                    if (arg->GetArgs().size() > 0) {
                        mf = Interpret(instruction->GetArg(0));
                    }
                    break;
                }
                
                mf = Interpret(instruction->GetArg(i));
            }

            context.SetCurrent(scope->GetParent());
            delete scope;

            return mf;
        }

        // Variables
        case Decl: {
            context.GetCurrent()->Set(*instruction->value.s);
            return { Mainfold::Null };
        }

        case Set: {
            Mainfold* mf = ARG(0).value.mf;
            Mainfold value = ARG(1);
            
            mf->type = value.type;
            mf->value = value.value;

            return { Mainfold::Null };
        }

        case Var: {
            return { 
                Mainfold::Field,
                &context.GetCurrent()->Get(*instruction->value.s)
            };
        }

        // Objects
        case New: {
            Object* object = new Object(instruction->value.i);
            Mainfold mf = { Mainfold::Object, object };

            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                object->fields.push_back(Interpret(instruction->GetArg(i)));
            }

            return mf;
        }

        case Field: {
            Object* object = ARG(0).value.mf->value.st;
            return { Mainfold::Field, &object->fields[instruction->value.i] };
        }

        // Operators
        case AddI: { return { Mainfold::Int,  { ARG(0).value.i + ARG(1).value.i } }; }
        case SubI: { return { Mainfold::Int,  { ARG(0).value.i - ARG(1).value.i } }; }
        case MulI: { return { Mainfold::Int,  { ARG(0).value.i * ARG(1).value.i } }; }
        case DivI: { return { Mainfold::Int,  { ARG(0).value.i / ARG(1).value.i } }; }
        case ModI: { return { Mainfold::Int,  { ARG(0).value.i % ARG(1).value.i } }; }

        case AddF: { return { Mainfold::Float,  { ARG(0).value.f + ARG(1).value.f } }; }
        case SubF: { return { Mainfold::Float,  { ARG(0).value.f - ARG(1).value.f } }; }
        case MulF: { return { Mainfold::Float,  { ARG(0).value.f * ARG(1).value.f } }; }
        case DivF: { return { Mainfold::Float,  { ARG(0).value.f / ARG(1).value.f } }; }
        case ModF: { return { Mainfold::Float,  { std::fmod(ARG(0).value.f, ARG(1).value.f) } }; }
        
        case Shl: { return { Mainfold::Int,  { ARG(0).value.i << ARG(1).value.i } }; }
        case Shr: { return { Mainfold::Int,  { ARG(0).value.i >> ARG(1).value.i } }; }
        case Eq:  { return { Mainfold::Bool, { ARG(0).value.i == ARG(1).value.i } }; }
        case Neq: { return { Mainfold::Bool, { ARG(0).value.i != ARG(1).value.i } }; }
        case Lt:  { return { Mainfold::Bool, { ARG(0).value.i < ARG(1).value.i } }; }
        case Gt:  { return { Mainfold::Bool, { ARG(0).value.i > ARG(1).value.i } }; }
        case Leq: { return { Mainfold::Bool, { ARG(0).value.i <= ARG(1).value.i } }; }
        case Geq: { return { Mainfold::Bool, { ARG(0).value.i >= ARG(1).value.i } }; }
        
        case And: { return { Mainfold::Bool, { ARG(0).value.b && ARG(1).value.b } }; }
        case Or:  { return { Mainfold::Bool, { ARG(0).value.b || ARG(1).value.b } }; }
        case Xor: { return { Mainfold::Bool, { ARG(0).value.b ^ ARG(1).value.b } }; }
        case Not: { return { Mainfold::Bool, { !ARG(0).value.b } }; }

        // Casts
        case IntToBool: { return { Mainfold::Bool, { (bool)ARG(0).value.i } }; }
        case IntToFloat: { return { Mainfold::Float, { (float)ARG(0).value.i } }; }
        case FloatToInt: { return { Mainfold::Int, { (int)ARG(0).value.f } }; }
        case FloatToBool: { return { Mainfold::Bool, { (bool)ARG(0).value.f } }; }
        case BoolToInt: { return { Mainfold::Int, { (int)ARG(0).value.b } }; }
        case BoolToFloat: { return { Mainfold::Float, { (float)ARG(0).value.b } }; }
        
        // Literals
        case Int: return { Mainfold::Int, { instruction->value.i } };
        case Float: return { Mainfold::Float, { instruction->value.f } };
        case String: return { Mainfold::String, { instruction->value.s } };
        case Bool: return { Mainfold::Bool, { instruction->value.b } };
        case Null: return { Mainfold::Null };
        default: break;
    }

    return { Mainfold::Null };
}

inline ir::Mainfold& ir::SymbolTable::Get(std::string name) {
    if (symbols.find(name) == symbols.end()) {
        if (parent != nullptr) {
            return parent->Get(name);
        }
    }

    return symbols[name];
}
