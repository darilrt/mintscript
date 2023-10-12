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

ir::Instruction::~Instruction() {
    for (int i = 0; i < args.size(); i++) {
        delete args[i];
    }
}

void ir::Interpreter::Interpret(std::vector<Instruction *> instructions) {
    Mainfold mf = { Mainfold::None };

    SymbolTable* global = new SymbolTable();
    context.Add(global);
    context.SetCurrent(global);

    for (int i = 0; i < instructions.size(); i++) {
        mf = Interpret(instructions[i]); 
    }
    
    switch (mf.type) {
        case Mainfold::Int: std::cout << mf.value.i << std::endl; break;
        case Mainfold::Float: std::cout << mf.value.f << std::endl; break;
        case Mainfold::String: std::cout << *mf.value.s << std::endl; break;
        case Mainfold::Bool: std::cout << (mf.value.b ? "true" : "false") << std::endl; break;
        case Mainfold::Object: std::cout << mf.value.i << std::endl; break;
        default: break;
    }
}

#define ARG(i) Interpret(instruction->GetArg(i))

ir::Mainfold ir::Interpreter::Interpret(Instruction *instruction) {
    switch (instruction->GetInstruction()) {
        case IR: { 
            return { Mainfold::Scope, new Instruction(ir::Scope, instruction->GetArgs()) };
        }

        case Set: {
            context.GetCurrent()->Set(
                *instruction->value.s, 
                ARG(0)
            );
            
            return { Mainfold::None };
        }

        case Var: { return context.GetCurrent()->Get(*instruction->value.s); }

        case If: {
            Mainfold ret = { Mainfold::None };
            
            if (ARG(0).value.b) {
                ret = Interpret(instruction->GetArg(1));
            }
            else if (instruction->GetArgs().size() > 2) {
                ret = Interpret(instruction->GetArg(2));
            }
            
            return ret;
        }

        case Call: {
            const std::string name = *instruction->value.s;

            if (name == "print") {
                const Mainfold value = ARG(0);

                switch (value.type) {
                    case Mainfold::Int: std::cout << value.value.i << std::endl; break;
                    case Mainfold::Float: std::cout << value.value.f << std::endl; break;
                    case Mainfold::String: std::cout << *value.value.s << std::endl; break;
                    case Mainfold::Bool: std::cout << (value.value.b ? "true" : "false") << std::endl; break;
                    case Mainfold::Object: std::cout << value.value.i << std::endl; break;
                    default: break;
                }
            }
            else {
                const std::vector<Instruction*> args = instruction->GetArgs();

                std::vector<Mainfold> argv;
                argv.resize(args.size());

                for (int i = 0; i < args.size(); i++) {
                    argv[i] = Interpret(args[i]);
                }

                stack.push(argv);
                
                Mainfold mf = context.GetCurrent()->Get(name);
                Mainfold ret = { Mainfold::None };

                if (mf.type == Mainfold::Scope) {
                    ret = Interpret(mf.value.ir);
                }

                stack.pop();

                return ret;
            }

            return { Mainfold::None };
        }

        case Arg: { return stack.top()[instruction->value.i]; }

        case Scope: {
            SymbolTable* scope = new SymbolTable(context.GetCurrent());
            context.Add(scope);
            context.SetCurrent(scope);

            Mainfold mf = { Mainfold::None };

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

        // Operators
        case Add: { return { Mainfold::Int,  { ARG(0).value.i + ARG(1).value.i } }; }
        case Sub: { return { Mainfold::Int,  { ARG(0).value.i - ARG(1).value.i } }; }
        case Mul: { return { Mainfold::Int,  { ARG(0).value.i * ARG(1).value.i } }; }
        case Div: { return { Mainfold::Int,  { ARG(0).value.i / ARG(1).value.i } }; }
        case Mod: { return { Mainfold::Int,  { ARG(0).value.i % ARG(1).value.i } }; }
        case And: { return { Mainfold::Bool, { ARG(0).value.b && ARG(1).value.b } }; }
        case Or:  { return { Mainfold::Bool, { ARG(0).value.b || ARG(1).value.b } }; }
        case Xor: { return { Mainfold::Bool, { ARG(0).value.b ^ ARG(1).value.b } }; }
        case Not: { return { Mainfold::Bool, { !ARG(0).value.b } }; }
        case Shl: { return { Mainfold::Int,  { ARG(0).value.i << ARG(1).value.i } }; }
        case Shr: { return { Mainfold::Int,  { ARG(0).value.i >> ARG(1).value.i } }; }
        case Eq:  { return { Mainfold::Bool, { ARG(0).value.i == ARG(1).value.i } }; }
        case Neq: { return { Mainfold::Bool, { ARG(0).value.i != ARG(1).value.i } }; }
        case Lt:  { return { Mainfold::Bool, { ARG(0).value.i < ARG(1).value.i } }; }
        case Gt:  { return { Mainfold::Bool, { ARG(0).value.i > ARG(1).value.i } }; }
        case Leq: { return { Mainfold::Bool, { ARG(0).value.i <= ARG(1).value.i } }; }
        case Geq: { return { Mainfold::Bool, { ARG(0).value.i >= ARG(1).value.i } }; }
        
        // Literals
        case Int: return { Mainfold::Int, { instruction->value.i } };
        case Float: return { Mainfold::Float, { instruction->value.f } };
        case String: return { Mainfold::String, { instruction->value.s } };
        case Bool: return { Mainfold::Bool, { instruction->value.b } };
        default: break;
    }

    return { Mainfold::None };
}

inline ir::Mainfold ir::SymbolTable::Get(std::string name) {
    if (symbols.find(name) == symbols.end()) {
        if (parent != nullptr) {
            return parent->Get(name);
        }
        
        return { Mainfold::None };
    }

    return symbols[name];
}
