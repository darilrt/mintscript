#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "ast_visitor.h"

#include <sstream>

#define PUSH_INST stack.top()->GetArgs().push_back
#define STACK_TOP stack.top()
#define STACK_POP stack.pop
#define STACK_PUSH stack.push
#define STACK_PUSH_I(X) \
    { \
        ir::Instruction* __inst = X; \
        stack.top()->GetArgs().push_back(__inst); \
        stack.push(__inst); \
    }
#define ins new ir::Instruction

static sa::Symbol *t_null = nullptr,
                  *t_int = nullptr,
                  *t_float = nullptr,
                  *t_str = nullptr,
                  *t_bool = nullptr,
                  *t_type = nullptr;

inline bool IsPrimitive(sa::Symbol* type) {
    return type == t_int || type == t_float || type == t_str || type == t_bool;
}

void PrintMainfold(ir::Mainfold mf) {
    switch (mf.type) {
        case ir::Mainfold::Int: std::cout << mf.value.i; break;
        case ir::Mainfold::Float: std::cout << mf.value.f; break;
        case ir::Mainfold::String: std::cout << *mf.value.s; break;
        case ir::Mainfold::Bool: std::cout << (mf.value.b ? "true" : "false"); break;
        case ir::Mainfold::Null: std::cout << "Null"; break;
        case ir::Mainfold::Field: std::cout << "{ field." << mf.value.mf << " }"; break;
        case ir::Mainfold::Object: std::cout << "{ object." << mf.value.st << " }"; break;
        case ir::Mainfold::Native: std::cout << "{ native }"; break;
        case ir::Mainfold::Scope: std::cout << "{ scope." << mf.value.ir << " }"; break;
        default: break;
    }
}

void LoadBuiltInTypes(sa::SymbolTable* table, std::stack<ir::Instruction*>& stack) {
    auto PUSH_NATIVE = [&](std::string label, ir::Mainfold (*lambda)(std::vector<ir::Mainfold>) ) {
        stack.top()->GetArgs().push_back(new ir::Instruction(ir::Decl, label, { })); \
        stack.top()->GetArgs().push_back(new ir::Instruction(ir::Set, { \
            new ir::Instruction(ir::Var, label, { }), \
            new ir::Instruction(ir::Native, lambda, { }) \
        }));
    };

    table->Set("Type", { true, false, nullptr });
    t_type = table->Get("Type");
    t_type->type = t_type;
    
    table->Set("Function", { true, false, t_type });
    
    table->Set("int", { true, false, t_type });
    t_int = table->Get("int");
    
    table->Set("float", { true, false, t_type });
    t_float = table->Get("float");
    
    table->Set("str", { true, false, t_type });
    t_str = table->Get("str");
    
    table->Set("bool", { true, false, t_type });
    t_bool = table->Get("bool");
    
    table->Set("null", { true, false, t_type });
    t_null = table->Get("null");

    t_float->SetMethod("ToStr", { t_str });
    t_float->GetMethod("ToStr")->name = "mfloat_strToStr";

    PUSH_NATIVE("mfloat_strToStr",
        [](std::vector<ir::Mainfold> args) -> ir::Mainfold {

            std::cout << "asdasdsa" << std::endl;
            ir::Mainfold* self = args.at(0).value.mf;

            std::ostringstream ss;
            ss << self->value.f;
            std::string* str = new std::string(ss.str());
            
            return { ir::Mainfold::String, str };
        }
    );
    
    table->Set("print", { false, false, table->Get("Function") });
    PUSH_NATIVE("print",
        [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            for (ir::Mainfold arg : args) {
                if (arg.type == ir::Mainfold::Field) {
                    PrintMainfold(*arg.value.mf);
                }
                else {
                    PrintMainfold(arg);
                }
            }

            return { ir::Mainfold::Null };
        }
    );
}

AstVisitor::AstVisitor() { }

AstVisitor::~AstVisitor() { }

AstVisitor* AstVisitor::Eval(ASTNode *node) {
    AstVisitor* visitor = new AstVisitor();
    
    visitor->stack.push(ins(ir::Scope, { }));

    visitor->table = new sa::SymbolTable();

    LoadBuiltInTypes(visitor->table, visitor->stack);
    
    sa::Symbol* list = node->Accept(visitor);

    return visitor;
}

sa::Symbol* AstVisitor::Visit(ASTNode *node) {
    std::cout << "ASTNode" << std::endl;
    return {};
}

sa::Symbol* AstVisitor::Visit(ProgramAST *node) {

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        sa::Symbol* res = stmt->Accept(this);

        if (mError::HasError()) { return {}; }
    }

    return t_null;
}

sa::Symbol* AstVisitor::Visit(NumberExprAST *node) {
    if (node->type == NumberExprAST::Type::Int) {
        PUSH_INST(ins(ir::Int, (int) node->value, { }));
        return t_int;
    }
    else {
        PUSH_INST(ins(ir::Float, node->value, { }));
        return t_float;
    }
    return t_null;
}

sa::Symbol* AstVisitor::Visit(StringExprAST *node) {
    PUSH_INST(ins(ir::String, node->value, { }));
    return t_str;
}

sa::Symbol* AstVisitor::Visit(BoolExprAST *node) {
    PUSH_INST(ins(ir::Bool, node->value, { }));
    return t_bool;
}

sa::Symbol* AstVisitor::Visit(NullExprAST *node) {
    PUSH_INST(ins(ir::Null, { }));
    return t_null;
}

sa::Symbol* AstVisitor::Visit(PropertyExprAST *node) {
    PUSH_INST(ins(ir::Var, node->name, { }));
    sa::Symbol* sym = table->Get(node->name);

    if (sym == nullptr) {
        mError::AddError("Symbol '" + node->name + "' not found");
        return t_null;
    }

    return sym->type;
}

sa::Symbol* AstVisitor::Visit(IndexExprAST *node) {
    throw std::runtime_error("IndexExprAST not implemented");
    return t_null;
}

sa::Symbol* AstVisitor::Visit(CallExprAST *node) {
    ir::Instruction* inst = ins(ir::Call, { });

    STACK_PUSH(inst);

    node->property->Accept(this);

    std::vector<ir::Instruction*>& args = inst->GetArg(0)->GetArgs();

    if (args.size() > 0) {
        PUSH_INST(args[0]);
    }

    for (auto arg : node->args) {
        arg->Accept(this);
    }

    STACK_POP();

    PUSH_INST(inst);

    return {};
}

sa::Symbol* AstVisitor::Visit(UnaryExprAST *node) {
    throw std::runtime_error("UnaryExprAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(BinaryExprAST *node) {
    ir::Instruction* inst = ins(ir::AddI, { });

    STACK_PUSH_I(inst);
    sa::Symbol* lhst = node->lhs->Accept(this);
    sa::Symbol* rhst = node->rhs->Accept(this);
    STACK_POP();

    if (
        node->op.type == Token::Type::NotEqual ||
        node->op.type == Token::Type::Less ||
        node->op.type == Token::Type::LessEqual ||
        node->op.type == Token::Type::Greater ||
        node->op.type == Token::Type::GreaterEqual ||
        node->op.type == Token::Type::AmpAmp ||
        node->op.type == Token::Type::PipePipe
    ) { 
        switch (node->op.type) {
            case Token::Type::NotEqual: inst->SetInstruction(ir::Neq); return t_bool;
            case Token::Type::Less: inst->SetInstruction(ir::Lt); return t_bool;
            case Token::Type::LessEqual: inst->SetInstruction(ir::Leq); return t_bool;
            case Token::Type::Greater: inst->SetInstruction(ir::Gt); return t_bool;
            case Token::Type::GreaterEqual: inst->SetInstruction(ir::Geq); return t_bool;
            case Token::Type::AmpAmp: inst->SetInstruction(ir::And); return t_bool;
            case Token::Type::PipePipe: inst->SetInstruction(ir::Or); return t_bool;
            default: break;
        }

        return t_bool;
    }

    sa::Symbol* type = lhst;

    if (IsPrimitive(lhst) && IsPrimitive(rhst)) {
        type = (lhst == t_float || rhst == t_float) ? t_float : t_int;

        if (type == t_float) {
            if (lhst == t_int) {
                inst->GetArgs()[0] = ins(
                    ir::IntToFloat,
                    { inst->GetArgs()[0] }
                );
            }
            else if (lhst == t_bool) {
                inst->GetArgs()[0] = ins(
                    ir::BoolToFloat,
                    { inst->GetArgs()[0] }
                );
            }

            if (rhst == t_int) {
                inst->GetArgs()[1] = ins(
                    ir::IntToFloat,
                    { inst->GetArgs()[1] }
                );
            }
            else if (rhst == t_bool) {
                inst->GetArgs()[1] = ins(
                    ir::BoolToFloat,
                    { inst->GetArgs()[1] }
                );
            }
        }
        else if (type == t_int) {
            if (lhst == t_bool) {
                inst->GetArgs()[0] = ins(
                    ir::BoolToInt,
                    { inst->GetArgs()[0] }
                );
            }

            if (rhst == t_bool) {
                inst->GetArgs()[1] = ins(
                    ir::BoolToInt,
                    { inst->GetArgs()[1] }
                );
            }
        }
        else {
            inst->GetArgs()[0] = ins(
                ir::BoolToInt,
                { inst->GetArgs()[0] }
            );
            inst->GetArgs()[1] = ins(
                ir::BoolToInt,
                { inst->GetArgs()[1] }
            );
        }
    }

    switch (node->op.type) {
        case Token::Type::Plus: inst->SetInstruction(type == t_float ? ir::AddF : ir::AddI); break;
        case Token::Type::Minus: inst->SetInstruction(type == t_float ? ir::SubF : ir::SubI); break;
        case Token::Type::Star: inst->SetInstruction(type == t_float ? ir::MulF : ir::MulI); break;
        case Token::Type::Slash: inst->SetInstruction(type == t_float ? ir::DivF : ir::DivI); break;
        case Token::Type::Percent: inst->SetInstruction(type == t_float ? ir::ModF : ir::ModI); break;
        default: break;
    }

    return type;
}

sa::Symbol* AstVisitor::Visit(TernaryExprAST *node) {
    // TODO: Implement
    return {};
}

sa::Symbol* AstVisitor::Visit(ParenExprAST *node) {
    return node->expr->Accept(this);
}

sa::Symbol* AstVisitor::Visit(ArrayExprAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(AccessExprAST *node) {
    ir::Instruction* inst = ins(ir::Field, { });

    STACK_PUSH(inst);
    sa::Symbol* type = node->expr->Accept(this);
    STACK_POP();
    
    if (type->HasMethod(node->name.value)) {
        const sa::Method* method = type->GetMethod(node->name.value);
        PUSH_INST(ins(ir::Var, method->name, { inst->GetArg(0) }));
        delete inst;
        return method->type;
    }
    else if (type->HasField(node->name.value)) {
        sa::Field* field = type->GetField(node->name.value);
        PUSH_INST(ins(
            ir::Field,
            field->offset, 
            { inst->GetArg(0) }
        ));
        delete inst;
        return field->type;
    }
    else {
        mError::AddError("'" + node->name.value + "' is not a member of '" + type->name + "'");
    }

    delete inst;
    return t_null;
}

sa::Symbol* AstVisitor::Visit(AssignmentAST *node) {
    throw std::runtime_error("AssignmentAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(VarDeclarationAST *node) {
    sa::Symbol* type = node->type->Accept(this);
    sa::Symbol* value = node->expression->Accept(this);

    if (type != value) {
        mError::AddError("Type mismatch");
        return {};
    }

    PUSH_INST(ins(ir::Decl, node->identifier.value, { }));
    table->Set(node->identifier.value, { false, node->isMutable, type });
    STACK_PUSH_I(ins(ir::Set, {
        ins(ir::Var, node->identifier.value, { })
    }));
    node->expression->Accept(this);
    STACK_POP();

    return {};
}

sa::Symbol* AstVisitor::Visit(LambdaAST *node) {
    throw std::runtime_error("LambdaAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ArgDeclAST *node) {
    throw std::runtime_error("ArgDeclAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(BlockAST *node) {
    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        sa::Symbol* res = stmt->Accept(this);

        if (mError::HasError()) { return t_null; }
    }
    return {};
}

sa::Symbol* AstVisitor::Visit(ReturnAST *node) {
    throw std::runtime_error("ReturnAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(FunctionAST *node) {
    throw std::runtime_error("FunctionAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(IfAST* node) {
    throw std::runtime_error("IfAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(WhileAST *node) {
    throw std::runtime_error("WhileAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ForAST *node) {
    throw std::runtime_error("ForAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(BreakAST *node) {
    throw std::runtime_error("BreakAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ContinueAST *node) {
    throw std::runtime_error("ContinueAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ImportAST *node) {
    throw std::runtime_error("ImportAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ExportAST *node) {
    throw std::runtime_error("ExportAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(ClassAST *node) {
    throw std::runtime_error("ClassAST not implemented");
    return {};
}

sa::Symbol* AstVisitor::Visit(TypeSignatureAST *node) {
    return table->Get(node->name.value);
}

sa::Symbol* AstVisitor::Visit(TypeAccessAST *node) {
    return node->lhs->Accept(this);
}