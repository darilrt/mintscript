#include "ir.h"

#include <cmath>

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

ir::Instruction::Instruction(Type instruction, Mainfold (*value)(std::vector<Mainfold>), std::vector<Instruction*> args) {
    this->instruction = instruction;
    this->args = args;
    this->value.native = value;
}

ir::Instruction::~Instruction() {
}

#define ARG(i) Interpret(instruction->GetArg(i))

ir::Mainfold ir::Interpreter::Interpret(Instruction *instruction) {

    switch (instruction->GetInstruction()) {
        case IR: { 
            return { Mainfold::Scope, new Instruction(ir::Scope, 1, instruction->GetArgs()) };
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

        case Loop: {
            Mainfold ret = { Mainfold::Null };

            while (ARG(0).value.b) {
                ret = Interpret(instruction->GetArg(1));
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
                else if (mf->type == Mainfold::Native) {
                    ret = mf->value.native(argv);
                }

                stack.pop();

                return ret;
            }
            
            return { Mainfold::Null };
        }

        case Arg: {
            return stack.top()[instruction->value.i]; 
        }

        case Scope: {
            // Scopes types:
            // 0: no breakable or returnable scope
            // 1: returnable scope
            // 2: breakable scope

            SymbolTable* scope = new SymbolTable(context.GetCurrent());
            context.Add(scope);
            context.SetCurrent(scope);

            Mainfold mf = { Mainfold::Null };

            int scopeType = instruction->value.i;

            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                mf = ARG(i);

                if (state == State::Return) {
                    if (scopeType == 1) {
                        state = State::None;
                    }
                    break;
                }
            }

            context.SetCurrent(scope->GetParent());
            delete scope;

            return mf;
        }

        case Native: {
            return { Mainfold::Native, instruction->value.native };
        }

        case Return: {
            Mainfold mf = { Mainfold::Null };

            if (instruction->GetArgs().size() > 0) {
                mf = ARG(0);

                if (mf.type == Mainfold::Field) {
                    mf = *mf.value.mf;
                }
            }

            state = State::Return;

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

            if (value.type == Mainfold::Field) {
                value = *value.value.mf;
            }
            
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

        case Val: {
            return *ARG(0).value.mf;
        }

        // Objects
        case New: {
            Object* object = new Object(instruction->value.i);
            Mainfold mf = { Mainfold::Object, object };

            // for (int i = 0; i < instruction->GetArgs().size(); i++) {
            //     object->fields.push_back(Interpret(instruction->GetArg(i)));
            // }

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

void ir::Interpreter::Print(Instruction *instruction, int indent) {
    const std::string indentStr = std::string(indent, ' ');

    switch (instruction->GetInstruction()) {
        case IR: {
            std::cout << indentStr << "IR {" << std::endl;
            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                Print(instruction->GetArg(i), indent + 1);
            }
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case If: {
            std::cout << indentStr << "If {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            Print(instruction->GetArg(1), indent + 1);
            if (instruction->GetArgs().size() > 2) {
                Print(instruction->GetArg(2), indent + 1);
            }
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Loop: {
            std::cout << indentStr << "Loop {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            Print(instruction->GetArg(1), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Call: {
            std::cout << indentStr << "Call {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            for (int i = 1; i < instruction->GetArgs().size(); i++) {
                Print(instruction->GetArg(i), indent + 1);
            }
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Arg: {
            std::cout << indentStr << "Arg(" << instruction->value.i << ")" << std::endl;
            break;
        }

        case Scope: {
            std::cout << indentStr << "Scope {" << std::endl;
            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                Print(instruction->GetArg(i), indent + 1);
            }
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Native: {
            std::cout << indentStr << "Native(" << instruction->value.native << ")" << std::endl;
            break;
        }

        case Return: {
            std::cout << indentStr << "Return {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        // Variables

        case Decl: {
            std::cout << indentStr << "Decl(" << *instruction->value.s << ")" << std::endl;
            break;
        }

        case Set: {
            std::cout << indentStr << "Set {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            Print(instruction->GetArg(1), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Var: {
            std::cout << indentStr << "Var(" << *instruction->value.s << ")" << std::endl;
            break;
        }

        case Val: {
            std::cout << indentStr << "Val {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        // Objects

        case New: {
            std::cout << indentStr << "New(" << instruction->value.i << ")" << std::endl;
            break;
        }

        case Field: {
            std::cout << indentStr << "Field(" << instruction->value.i << ") { \n";
            Print(instruction->GetArg(0), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        // Operators

        case AddI: {
            std::cout << indentStr << "AddI {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            Print(instruction->GetArg(1), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case SubI: {
            std::cout << indentStr << "SubI {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            Print(instruction->GetArg(1), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        // Literals
        
        case Int: { std::cout << indentStr << "Int(" << instruction->value.i << ")" << std::endl; break; }
        case Float: { std::cout << indentStr << "Float(" << instruction->value.f << ")" << std::endl; break; }
        case String: { std::cout << indentStr << "String(" << *instruction->value.s << ")" << std::endl; break; }
        case Bool: { std::cout << indentStr << "Bool(" << instruction->value.b << ")" << std::endl; break; }
        case Null: { std::cout << indentStr << "Null" << std::endl; break; }
        
        default: break;
    }
}

inline ir::Mainfold& ir::SymbolTable::Get(std::string name) {
    if (symbols.find(name) == symbols.end()) {
        if (parent != nullptr) {
            return parent->Get(name);
        }
    }

    return symbols[name];
}
