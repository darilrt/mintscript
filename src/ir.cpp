#include "ir.h"

#include <cmath>

ir::Instruction* ir::global = new Instruction(ir::Scope, 3, { });

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

ir::Instruction::Instruction(Type instruction, NewInfo *value, std::vector<Instruction *> args) {
    this->instruction = instruction;
    this->args = args;
    this->value.n = value;
}

ir::Instruction::~Instruction() {
}

#define ARG(i) Interpret(instruction->GetArg(i))

ir::Interpreter::Interpreter() {
    SymbolTable* scope = new SymbolTable(nullptr);
    context.Add(scope);
    context.SetCurrent(scope);
}

ir::Interpreter::~Interpreter() {
}

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
                ret = ARG(1);

                if (state == State::Continue) {
                    state = State::None;
                }

                if (state == State::Break) {
                    state = State::None;
                    break;
                }
            }

            return ret;
        }

        case Call: {
            Mainfold name = ARG(0);

            if (name.type != Mainfold::String) {
                const std::vector<Instruction*> args = instruction->GetArgs();

                std::vector<Mainfold> argv;
                
                if (args.size() > 1) {
                    argv.resize(args.size() - 1);
                    for (int i = 1; i < args.size(); i++) {
                        argv[i - 1] = Interpret(args[i]);
                    }
                }
                
                Mainfold* mf = &name;

                if (name.type == Mainfold::Field) {
                    mf = name.value.mf;
                }

                Mainfold ret = { Mainfold::Null };

                if (mf->type == Mainfold::Scope) {
                    stack.push(argv);
                    ret = Interpret(mf->value.ir);
                    
                    if (state == State::Return) {
                        state = State::None;
                    }
                    stack.pop();
                }
                else if (mf->type == Mainfold::Native) {
                    ret = mf->value.native(argv);
                }


                return ret;
            }
            
            std::cout << "Call End"  << std::endl;
            return { Mainfold::Null };
        }

        case Arg: {
            return stack.top()[instruction->value.i]; 
        }

        case Vtable: {
            Mainfold var = ARG(0);
            return {
                Mainfold::Field,
                &context.GetCurrent()->Get(var.vtables[instruction->value.i])
            };
        }

        case Scope: {
            // Scopes types:
            //      0: no breakable or returnable scope
            //      1: returnable scope
            //      2: breakable scope
            //      3: no symbol table

            int scopeType = instruction->value.i;

            SymbolTable* scope = nullptr;
            if (scopeType != 3) {
                scope = new SymbolTable(context.GetCurrent());
                context.Add(scope);
                context.SetCurrent(scope);
            }

            Mainfold mf = { Mainfold::Null };

            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                mf = ARG(i);

                if (state != State::None) {
                    break;
                }
            }

            if (scopeType != 3) {
                context.SetCurrent(context.GetCurrent()->GetParent());
                delete scope;
            }

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

        case Break: {
            state = State::Break;
            return { Mainfold::Null };
        }

        case Continue: {
            state = State::Continue;
            return { Mainfold::Null };
        }

        // Variables
        case Decl: {
            context.GetCurrent()->Set(*instruction->value.s);
            return { 
                Mainfold::Field,
                &context.GetCurrent()->Get(*instruction->value.s)
            };
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
            Instruction::NewInfo* info = instruction->value.n;
            Object* object = new Object(info->size);

            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                Mainfold mf = ARG(i);

                if (mf.type == Mainfold::Field) {
                    mf = *mf.value.mf;
                }

                object->fields[i] = mf;
            }

            Mainfold mf = { Mainfold::Object, object };
            mf.vtables = info->vtables;
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
        
        case EqI:  { return { Mainfold::Bool, { ARG(0).value.i == ARG(1).value.i } }; }
        case NeqI: { return { Mainfold::Bool, { ARG(0).value.i != ARG(1).value.i } }; }
        case LtI:  { return { Mainfold::Bool, { ARG(0).value.i < ARG(1).value.i } }; }
        case GtI:  { return { Mainfold::Bool, { ARG(0).value.i > ARG(1).value.i } }; }
        case LeqI: { return { Mainfold::Bool, { ARG(0).value.i <= ARG(1).value.i } }; }
        case GeqI: { return { Mainfold::Bool, { ARG(0).value.i >= ARG(1).value.i } }; }

        case EqF:  { return { Mainfold::Bool, { ARG(0).value.f == ARG(1).value.f } }; }
        case NeqF: { return { Mainfold::Bool, { ARG(0).value.f != ARG(1).value.f } }; }
        case LtF:  { return { Mainfold::Bool, { ARG(0).value.f < ARG(1).value.f } }; }
        case GtF:  { return { Mainfold::Bool, { ARG(0).value.f > ARG(1).value.f } }; }
        case LeqF: { return { Mainfold::Bool, { ARG(0).value.f <= ARG(1).value.f } }; }
        case GeqF: { return { Mainfold::Bool, { ARG(0).value.f >= ARG(1).value.f } }; }
        
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
            std::cout << indentStr << "Scope(" << instruction->value.i << ") {" << std::endl;
            for (int i = 0; i < instruction->GetArgs().size(); i++) {
                Print(instruction->GetArg(i), indent + 1);
            }
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Native: {
            std::cout << indentStr << "Native(" << (void*) instruction->value.native << ")" << std::endl;
            break;
        }

        case Return: {
            std::cout << indentStr << "Return {" << std::endl;
            Print(instruction->GetArg(0), indent + 1);
            std::cout << indentStr << "}" << std::endl;
            break;
        }

        case Break: {
            std::cout << indentStr << "Break" << std::endl;
            break;
        }

        case Continue: {
            std::cout << indentStr << "Continue" << std::endl;
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

#define OP(name) { std::cout << indentStr << #name << " {" << std::endl; Print(instruction->GetArg(0), indent + 1); Print(instruction->GetArg(1), indent + 1); std::cout << indentStr << "}" << std::endl; break; }

        case AddI: OP(AddI)
        case SubI: OP(SubI)
        case MulI: OP(MulI)
        case DivI: OP(DivI)
        case ModI: OP(ModI)

        case AddF: OP(AddF)
        case SubF: OP(SubF)
        case MulF: OP(MulF)
        case DivF: OP(DivF)
        case ModF: OP(ModF)

        case Shl: OP(Shl)
        case Shr: OP(Shr)

        case EqI: OP(EqI)
        case NeqI: OP(NeqI)
        case LtI: OP(LtI)
        case GtI: OP(GtI)
        case LeqI: OP(LeqI)
        case GeqI: OP(GeqI)

        case EqF: OP(EqF)
        case NeqF: OP(NeqF)
        case LtF: OP(LtF)
        case GtF: OP(GtF)
        case LeqF: OP(LeqF)
        case GeqF: OP(GeqF)

        // Literals
        
        case Int: { std::cout << indentStr << "Int(" << instruction->value.i << ")" << std::endl; break; }
        case Float: { std::cout << indentStr << "Float(" << instruction->value.f << ")" << std::endl; break; }
        case String: { std::cout << indentStr << "String(" << *instruction->value.s << ")" << std::endl; break; }
        case Bool: { std::cout << indentStr << "Bool(" << instruction->value.b << ")" << std::endl; break; }
        case Null: { std::cout << indentStr << "Null" << std::endl; break; }
        
        default:
            std::cout << indentStr << "Unknown instruction " << instruction->GetInstruction() << std::endl;
            break;
    }
}

inline ir::Mainfold& ir::SymbolTable::Get(std::string name) {
    if (symbols.find(name) == symbols.end()) {
        if (parent != nullptr) {
            return parent->Get(name);
        }

        std::cout << "IR: Symbol " << name << " not found" << std::endl;
    }

    return symbols[name];
}
