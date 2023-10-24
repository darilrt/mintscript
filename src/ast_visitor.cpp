#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "ast_visitor.h"

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
#define i new ir::Instruction

static sa::Symbol *t_null = nullptr,
                  *t_int = nullptr,
                  *t_float = nullptr,
                  *t_str = nullptr,
                  *t_bool = nullptr;

inline bool IsPrimitive(sa::Symbol* type) {
    return type == t_int || type == t_float || type == t_str || type == t_bool;
}

AstVisitor::AstVisitor() { }

AstVisitor::~AstVisitor() { }

AstVisitor* AstVisitor::Eval(ASTNode *node) {
    AstVisitor* visitor = new AstVisitor();
    
    visitor->stack.push(new ir::Instruction(ir::Type::Scope, { }));

    visitor->table = new sa::SymbolTable();
    
    visitor->table->Set("int", { true, false, nullptr });
    visitor->table->Set("float", { true, false, nullptr });
    visitor->table->Set("str", { true, false, nullptr });
    visitor->table->Set("bool", { true, false, nullptr });
    visitor->table->Set("null", { true, false, nullptr });

    t_null = visitor->table->Get("null");
    t_int = visitor->table->Get("int");
    t_float = visitor->table->Get("float");
    t_str = visitor->table->Get("str");
    t_bool = visitor->table->Get("bool");

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
        PUSH_INST(i(ir::Int, (int) node->value, { }));
        return t_int;
    }
    else {
        PUSH_INST(i(ir::Float, node->value, { }));
        return t_float;
    }
    return t_null;
}

sa::Symbol* AstVisitor::Visit(StringExprAST *node) {
    PUSH_INST(i(ir::String, node->value, { }));
    return t_str;
}

sa::Symbol* AstVisitor::Visit(BoolExprAST *node) {
    PUSH_INST(i(ir::Bool, node->value, { }));
    return t_bool;
}

sa::Symbol* AstVisitor::Visit(NullExprAST *node) {
    PUSH_INST(i(ir::Null, { }));
    return t_null;
}

sa::Symbol* AstVisitor::Visit(PropertyExprAST *node) {
    if (node->name == "print") {
        PUSH_INST(i(ir::String, node->name, { }));
    }
    else {
        PUSH_INST(i(ir::Var, node->name, { }));
        sa::Symbol* sym = table->Get(node->name);
        return sym->type;
    }
    return t_null;
}

sa::Symbol* AstVisitor::Visit(IndexExprAST *node) {
    return t_null;
}

sa::Symbol* AstVisitor::Visit(CallExprAST *node) {
    STACK_PUSH_I(i(ir::Call, { }));

    node->property->Accept(this);
    
    for (auto arg : node->args) {
        arg->Accept(this);
    }

    STACK_POP();

    return {};
}

sa::Symbol* AstVisitor::Visit(UnaryExprAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(BinaryExprAST *node) {
    ir::Instruction* inst = i(ir::AddI, { });

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
                inst->GetArgs()[0] = i(
                    ir::IntToFloat,
                    { inst->GetArgs()[0] }
                );
            }
            else if (lhst == t_bool) {
                inst->GetArgs()[0] = i(
                    ir::BoolToFloat,
                    { inst->GetArgs()[0] }
                );
            }

            if (rhst == t_int) {
                inst->GetArgs()[1] = i(
                    ir::IntToFloat,
                    { inst->GetArgs()[1] }
                );
            }
            else if (rhst == t_bool) {
                inst->GetArgs()[1] = i(
                    ir::BoolToFloat,
                    { inst->GetArgs()[1] }
                );
            }
        }
        else if (type == t_int) {
            if (lhst == t_bool) {
                inst->GetArgs()[0] = i(
                    ir::BoolToInt,
                    { inst->GetArgs()[0] }
                );
            }

            if (rhst == t_bool) {
                inst->GetArgs()[1] = i(
                    ir::BoolToInt,
                    { inst->GetArgs()[1] }
                );
            }
        }
        else {
            inst->GetArgs()[0] = i(
                ir::BoolToInt,
                { inst->GetArgs()[0] }
            );
            inst->GetArgs()[1] = i(
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
    return {};
}

sa::Symbol* AstVisitor::Visit(AssignmentAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(VarDeclarationAST *node) {
    sa::Symbol* type = node->type->Accept(this);
    sa::Symbol* value = node->expression->Accept(this);

    if (type != value) {
        mError::AddError("Type mismatch");
        return {};
    }

    PUSH_INST(i(ir::Decl, node->identifier.value, { }));
    table->Set(node->identifier.value, { false, node->isMutable, type });
    STACK_PUSH_I(i(ir::Set, {
        i(ir::Var, node->identifier.value, { })
    }));
    node->expression->Accept(this);
    STACK_POP();

    return {};
}

sa::Symbol* AstVisitor::Visit(LambdaAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ArgDeclAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(BlockAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ReturnAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(FunctionAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(IfAST* node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(WhileAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ForAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(BreakAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ContinueAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ImportAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ExportAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(ClassAST *node) {
    return {};
}

sa::Symbol* AstVisitor::Visit(TypeSignatureAST *node) {
    return table->Get(node->name.value);
}

sa::Symbol* AstVisitor::Visit(TypeAccessAST *node) {
    return node->lhs->Accept(this);
}