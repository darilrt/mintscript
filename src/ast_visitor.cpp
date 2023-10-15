#include "types.h"
#include "module.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "symbol.h"
#include "ast_visitor.h"

AstVisitor::AstVisitor() { }

AstVisitor::~AstVisitor() { }

AstVisitor* AstVisitor::Eval(ASTNode *node) {
    AstVisitor* visitor = new AstVisitor();
    
    visitor->stack.push(new ir::Instruction(ir::Type::IR, { }));

    mList list = node->Accept(visitor);

    return visitor;
}

mList AstVisitor::Visit(ASTNode *node) {
    std::cout << "ASTNode" << std::endl;
    return {};
}

mList AstVisitor::Visit(ProgramAST *node) {
    
    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        mList res = stmt->Accept(this);

        if (mError::HasError()) { return {}; }

        if (res.items.size() > 0) {
            mObject* result = res[0];

            if (result != nullptr) {
                return {};
            }
        }
    }

    return {};
}

mList AstVisitor::Visit(NumberExprAST *node) {
    std::cout << "NumberExprAST" << std::endl;
    return {};
}

#define PUSH_INST stack.top()->GetArgs().push_back
#define STACK_TOP stack.top()
#define STACK_POP stack.pop
#define STACK_PUSH stack.push
#define i new ir::Instruction

mList AstVisitor::Visit(StringExprAST *node) {
    PUSH_INST(i(ir::String, node->value, { }));
    return {};
}

mList AstVisitor::Visit(BoolExprAST *node) {
    return {};
}

mList AstVisitor::Visit(NullExprAST *node) {
    return {};
}

mList AstVisitor::Visit(PropertyExprAST *node) {
    if (node->name == "print") {
        PUSH_INST(i(ir::String, node->name, { }));
    }
    else {
        PUSH_INST(i(ir::Var, node->name, { }));
    }
    return {};
}

mList AstVisitor::Visit(IndexExprAST *node) {
    return {};
}

mList AstVisitor::Visit(CallExprAST *node) {
    STACK_PUSH(i(ir::Call, { }));

    node->property->Accept(this);
    
    for (auto arg : node->args) {
        arg->Accept(this);
    }

    instructions.push_back(STACK_TOP);

    return {};
}

mList AstVisitor::Visit(UnaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(BinaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(TernaryExprAST *node) {
    return {};
}

mList AstVisitor::Visit(ParenExprAST *node) {
    return {};
}

mList AstVisitor::Visit(ArrayExprAST *node) {
    return {};
}

mList AstVisitor::Visit(AccessExprAST *node) {
    return {};
}

mList AstVisitor::Visit(AssignmentAST *node) {
    return {};
}

mList AstVisitor::Visit(VarDeclarationAST *node) {
    std::cout << "VarDeclarationAST" << std::endl;
    return {};
}

mList AstVisitor::Visit(LambdaAST *node) {
    return {};
}

mList AstVisitor::Visit(ArgDeclAST *node) {
    return {};
}

mList AstVisitor::Visit(BlockAST *node) {
    return {};
}

mList AstVisitor::Visit(ReturnAST *node) {
    return {};
}

mList AstVisitor::Visit(FunctionAST *node) {
    return {};
}

mList AstVisitor::Visit(IfAST* node) {
    return {};
}

mList AstVisitor::Visit(WhileAST *node) {
    return {};
}

mList AstVisitor::Visit(ForAST *node) {
    return {};
}

mList AstVisitor::Visit(BreakAST *node) {
    return {};
}

mList AstVisitor::Visit(ContinueAST *node) {
    return {};
}

mList AstVisitor::Visit(ImportAST *node) {
    return {};
}

mList AstVisitor::Visit(ExportAST *node) {
    return {};
}

mList AstVisitor::Visit(ClassAST *node) {
    return {};
}

mList AstVisitor::Visit(TypeSignatureAST *node) {
    return {};
}

mList AstVisitor::Visit(TypeAccessAST *node) {
    return {};
}