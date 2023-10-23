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

    return {};
}

sa::Symbol* AstVisitor::Visit(NumberExprAST *node) {
    if (node->type == NumberExprAST::Type::Int) {
        PUSH_INST(i(ir::Int, (int) node->value, { }));

        return table->Get("int");
    }
    else {
        PUSH_INST(i(ir::Float, node->value, { }));
        
        return table->Get("float");
    }
    return {};
}

sa::Symbol* AstVisitor::Visit(StringExprAST *node) {
    PUSH_INST(i(ir::String, node->value, { }));
    return {};
}

sa::Symbol* AstVisitor::Visit(BoolExprAST *node) {
    PUSH_INST(i(ir::Bool, node->value, { }));
    return {};
}

sa::Symbol* AstVisitor::Visit(NullExprAST *node) {
    PUSH_INST(i(ir::Null, { }));
    return {};
}

sa::Symbol* AstVisitor::Visit(PropertyExprAST *node) {
    if (node->name == "print") {
        PUSH_INST(i(ir::String, node->name, { }));
    }
    else {
        PUSH_INST(i(ir::Var, node->name, { }));
    }
    return {};
}

sa::Symbol* AstVisitor::Visit(IndexExprAST *node) {
    return {};
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
    ir::Instruction* inst = i(ir::Add, { });

    STACK_PUSH_I(inst);
    node->lhs->Accept(this);
    node->rhs->Accept(this);
    STACK_POP();

    switch (node->op.type) {
        case Token::Type::Plus: break;
        case Token::Type::Minus: inst->SetInstruction(ir::Sub); break;
        case Token::Type::Star: inst->SetInstruction(ir::Mul); break;
        case Token::Type::Slash: inst->SetInstruction(ir::Div); break;
        case Token::Type::Percent: inst->SetInstruction(ir::Mod); break;
        case Token::Type::EqualEqual: inst->SetInstruction(ir::Eq); break;
        case Token::Type::NotEqual: inst->SetInstruction(ir::Neq); break;
        case Token::Type::Less: inst->SetInstruction(ir::Lt); break;
        case Token::Type::LessEqual: inst->SetInstruction(ir::Leq); break;
        case Token::Type::Greater: inst->SetInstruction(ir::Gt); break;
        case Token::Type::GreaterEqual: inst->SetInstruction(ir::Geq); break;
        case Token::Type::Amp: inst->SetInstruction(ir::And); break;
        case Token::Type::Pipe: inst->SetInstruction(ir::Or); break;
        case Token::Type::Caret: inst->SetInstruction(ir::Xor); break;
        case Token::Type::LessLess: inst->SetInstruction(ir::Shl); break;
        case Token::Type::GreaterGreater: inst->SetInstruction(ir::Shr); break;
        case Token::Type::AmpAmp: inst->SetInstruction(ir::And); break;
        case Token::Type::PipePipe: inst->SetInstruction(ir::Or); break;
        default: break;
    }

    return {};
}

sa::Symbol* AstVisitor::Visit(TernaryExprAST *node) {
    // TODO: Implement
    return {};
}

sa::Symbol* AstVisitor::Visit(ParenExprAST *node) {
    node->expr->Accept(this);
    return {};
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
    table->Set(node->identifier.value, { false, node->isMutable, nullptr });
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