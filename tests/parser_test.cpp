#include "parser.h"
#include "ast.h"
#include "decl.h"
#include "expr.h"
#include "test.h"
#include "visitor.h"

#include <cassert>
#include <iostream>

#define PRINT_INDENT() for (int i = 0; i < indent; i++) std::cout << "  ";

class PrintVisitor : public Visitor {
public:
    int indent = 0;

    mList Visit(ASTNode* node) {
        PRINT_INDENT()
        std::cout << "ASTNode" << std::endl;
        return {};
    }

    mList Visit(NumberExprAST* node) {
        PRINT_INDENT()
        std::cout << "NumberExprAST: " << node->value << std::endl;
        return {};
    }

    mList Visit(StringExprAST* node) {
        PRINT_INDENT()
        std::cout << "StringExprAST: " << node->value << std::endl;
        return {};
    }

    mList Visit(BoolExprAST* node) {
        PRINT_INDENT()
        std::cout << "BoolExprAST: " << (node->value ? "true" : "false") << std::endl;
        return {};
    }

    mList Visit(NullExprAST* node) {
        PRINT_INDENT()
        std::cout << "NullExprAST" << std::endl;
        return {};
    }

    mList Visit(LambdaExprAST* node) {
        return {};
    }

    mList Visit(PropertyExprAST* node) {
        PRINT_INDENT()
        std::cout << "PropertyExprAST: " << node->name << std::endl;
        return {};
    }

    mList Visit(IndexExprAST* node) {
        PRINT_INDENT()
        std::cout << "IndexExprAST" << std::endl;
        indent++;
        node->expr->Accept(this);
        node->index->Accept(this);
        indent--;
        return {};
    }

    mList Visit(CallExprAST* node) {
        PRINT_INDENT()
        std::cout << "CallExprAST" << std::endl;
        indent++;
        node->property->Accept(this);
        for (auto arg : node->args) {
            arg->Accept(this);
        }
        indent--;
        return {};
    }

    mList Visit(UnaryExprAST* node) {
        PRINT_INDENT()
        std::cout << "UnaryExprAST: " << (node->op.TypeToString()) << std::endl;
        indent++;
        node->expr->Accept(this);
        indent--;
        return {};
    }

    mList Visit(BinaryExprAST* node) {
        PRINT_INDENT()
        std::cout << "BinaryExprAST: " << (node->op.TypeToString()) << std::endl;
        indent++;
        node->lhs->Accept(this);
        node->rhs->Accept(this);
        indent--;
        return {};
    }

    mList Visit(TernaryExprAST* node) {
        PRINT_INDENT()
        std::cout << "TernaryExprAST" << std::endl;
        indent++;
        node->condition->Accept(this);
        node->trueExpr->Accept(this);
        node->falseExpr->Accept(this);
        indent--;
        return {};
    }

    mList Visit(ParenExprAST* node) {
        PRINT_INDENT()
        std::cout << "ParenExprAST" << std::endl;
        return {};
    }

    mList Visit(ArrayExprAST* node) {
        PRINT_INDENT()
        std::cout << "ArrayExprAST" << std::endl;
        return {};
    }

    mList Visit(AccessExprAST* node) {
        PRINT_INDENT()
        std::cout << "AccessExprAST" << std::endl;
        return {};
    }

    mList Visit(AssignmentAST* node) {
        PRINT_INDENT()
        std::cout << "AssignmentAST" << std::endl;
        return {};
    }

    mList Visit(VarDeclarationAST* node) {
        PRINT_INDENT()
        std::cout << "VarDeclarationAST: " << std::endl;
        indent++;
        // node->expr->Accept(this);
        indent--;
        return {};
    }

    mList Visit(LambdaAST* node) {
        PRINT_INDENT()
        std::cout << "LambdaAST" << std::endl;
        return {};
    }

    mList Visit(ArgDeclAST* node) {
        PRINT_INDENT()
        std::cout << "ArgDeclAST: " << std::endl;
        return {};
    }

    mList Visit(BlockAST* node) {
        for (auto stmt : node->statements) {
            stmt->Accept(this);
        }
        return {};
    }

    mList Visit(ReturnAST* node) {
        PRINT_INDENT()
            std::cout << "ReturnAST" << std::endl;
        indent++;
        // node->expr->Accept(this);
        indent--;
        return {};
    }

    mList Visit(FunctionAST* node) {
        PRINT_INDENT()
        std::cout << "FunctionAST: " << node->name.value << std::endl;
        indent++;
        // for (auto arg : node->args) {
        //     arg->Accept(this);
        // }
        // node->block->Accept(this);
        indent--;
        return {};
    }

    mList Visit(IfAST* node) {
        PRINT_INDENT()
        std::cout << "IfAST" << std::endl;
        indent++;
        // node->condition->Accept(this);
        // node->trueBlock->Accept(this);
        // if (node->falseBlock) {
        //     node->falseBlock->Accept(this);
        // }
        indent--;
        return {};
    }

    mList Visit(WhileAST* node) {
        PRINT_INDENT()
        std::cout << "WhileAST" << std::endl;
        indent++;
        // node->condition->Accept(this);
        // node->block->Accept(this);
        indent--;
        return {};
    }

    mList Visit(BreakAST* node) {
        std::cout << "BreakAST" << std::endl;
        return {};
    }

    mList Visit(ContinueAST* node) {
        std::cout << "ContinueAST" << std::endl;
        return {};
    }
};

void print_ast(ASTNode* ast) {
    PrintVisitor visitor;
    ast->Accept(&visitor);
}

void TestParser() {
    Parser parser("\na = 10;");

    auto ast = parser.Parse();

    std::cout << "\n===========" << std::endl;
    print_ast(ast);
    std::cout << "\n===========" << std::endl;
}