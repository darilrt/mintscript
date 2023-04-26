#include "types.h"
#include "mfn.h"
#include "mint.h"
#include "mfloat.h"
#include "mstr.h"
#include "mbool.h"
#include "mnull.h"
#include "mlist.h"

#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "symbol.h"

#include "debug_visitor.h"

mObject *EvalVisitor::Visit(ASTNode *node) {
    return node->Accept(this);
}

mObject *EvalVisitor::Visit(NumberExprAST *node) {
    switch (node->type) {
    case NumberExprAST::Type::Int: return new mint((int)node->value);
        case NumberExprAST::Type::Float: return new mfloat((float)node->value);
    }

    return new mint(0);
}

mObject *EvalVisitor::Visit(StringExprAST *node) {
    return new mStr(node->value);
}

mObject *EvalVisitor::Visit(BoolExprAST *node) {
    return node->value ? mbool::True : mbool::False;
}

mObject *EvalVisitor::Visit(NullExprAST *node) {
    return mnull::Null;
}

mObject *EvalVisitor::Visit(LambdaExprAST *node) {
    std::cout << "LambdaExprAST" << std::endl;
    return nullptr;
}

mObject *EvalVisitor::Visit(PropertyExprAST *node) {
    mObject *result = zSymbolTable::locals->Get(node->name);

    if (result == nullptr) {
        std::cout << "Name '" << node->name << "' is not defined" << std::endl;
        std::cout << " but we can define it for you as null" << std::endl;
        
        zSymbolTable::locals->Set(node->name, mnull::Null);
        result = zSymbolTable::locals->Get(node->name); 
    }

    return result;
}

mObject *EvalVisitor::Visit(IndexExprAST *node) {
    mObject *object = node->expr->Accept(this);

    if (object == nullptr) {
        return nullptr; // TODO: Error
    }

    mObject *index = node->index->Accept(this);

    if (index == nullptr) {
        return nullptr; // TODO: Error
    }

    mlist* args = new mlist({ index });

    mObject *result = object->CallMethod("zGet", args, nullptr);

    DECREF(args);
    DECREF(object);
    DECREF(index);

    return nullptr;
}

mObject *EvalVisitor::Visit(CallExprAST *node) {
    mObject * object = node->property->Accept(this);

    if (object == nullptr) {
        return nullptr; // TODO: Error
    }

    mlist* args = new mlist();

    // for (auto arg : node->args) {
    //     zObject *result = arg->Accept(this);

    //     if (result == nullptr) {
    //         return nullptr; // TODO: Error
    //     }

    //     args->Append(result);
    // }
    
    mObject *result = object->CallMethod("zCall", args, nullptr);

    DECREF(args);

    return result;
}

mObject *EvalVisitor::Visit(UnaryExprAST *node) {
    mObject *result = nullptr;
    
    mObject *operand = node->expr->Accept(this);

    if (operand == nullptr) {
        return nullptr;
    }

    mlist* args = new mlist({ operand });

    // Operator overloading

    switch (node->op.type) {
        case Token::Type::Plus:             result = operand->CallMethod("zPos",       args, nullptr); break; // +
        case Token::Type::Minus:            result = operand->CallMethod("zNeg",       args, nullptr); break; // -
        case Token::Type::Not:              result = operand->CallMethod("zNot",       args, nullptr); break; // !

        case Token::Type::PlusPlus: {
            if (node->isPrefix) {           result = operand->CallMethod("zPreInc",    args, nullptr); break; } // ++
            else {                          result = operand->CallMethod("zPostInc",   args, nullptr); break; } // ++
            break;
        }

        case Token::Type::MinusMinus: {
            if (node->isPrefix) {           result = operand->CallMethod("zPreDec",    args, nullptr); break; } // --
            else {                          result = operand->CallMethod("zPostDec",   args, nullptr); break; } // --
            break;
        }
    }

    DECREF(args);
    DECREF(operand);

    return result;
}

mObject *EvalVisitor::Visit(BinaryExprAST *node) {
    mObject *result = nullptr;
    mObject *left = node->lhs->Accept(this);
    mObject *right = node->rhs->Accept(this);

    if (left == nullptr || right == nullptr) { return nullptr; }

    mlist* args = new mlist({ right });

    // Operator overloading
    // TODO: Move this to a separate function
    switch (node->op.type) {
        case Token::Type::Plus:             result = left->CallMethod("zAdd",       args, nullptr); break; // +
        case Token::Type::Minus:            result = left->CallMethod("zSub",       args, nullptr); break; // -
        case Token::Type::Star:             result = left->CallMethod("zMul",       args, nullptr); break; // *
        case Token::Type::Slash:            result = left->CallMethod("zDiv",       args, nullptr); break; // /
        case Token::Type::Percent:          result = left->CallMethod("zMod",       args, nullptr); break; // %
        case Token::Type::EqualEqual:       result = left->CallMethod("zEq",        args, nullptr); break; // ==
        case Token::Type::NotEqual:         result = left->CallMethod("zNeq",       args, nullptr); break; // !=
        case Token::Type::Less:             result = left->CallMethod("zLt",        args, nullptr); break; // <
        case Token::Type::LessEqual:        result = left->CallMethod("zLe",        args, nullptr); break; // <=
        case Token::Type::Greater:          result = left->CallMethod("zGt",        args, nullptr); break; // >
        case Token::Type::GreaterEqual:     result = left->CallMethod("zGe",        args, nullptr); break; // >=
        case Token::Type::Amp:              result = left->CallMethod("zAnd",       args, nullptr); break; // &
        case Token::Type::Pipe:             result = left->CallMethod("zOr",        args, nullptr); break; // |
        case Token::Type::Caret:            result = left->CallMethod("zXor",       args, nullptr); break; // ^ 
        case Token::Type::LessLess:         result = left->CallMethod("zLShift",    args, nullptr); break; // <<
        case Token::Type::GreaterGreater:   result = left->CallMethod("zRShift",    args, nullptr); break; // >>
        case Token::Type::AmpAmp:           result = left->CallMethod("zLAnd",      args, nullptr); break; // &&
        case Token::Type::PipePipe:         result = left->CallMethod("zLOr",       args, nullptr); break; // ||
    }

    if (result == nullptr) {
        std::cout << "Unsupported operator " << node->op.value;
        std::cout << " for types '" << left->type->name << "' and '" << right->type->name << "'" << std::endl;
    }
    
    DECREF(args);
    DECREF(left);
    DECREF(right);

    return result;
}

mObject *EvalVisitor::Visit(TernaryExprAST *node) {
    return nullptr;
}

mObject *EvalVisitor::Visit(ParenExprAST *node) {
    return node->expr->Accept(this);
}

mObject *EvalVisitor::Visit(AssignmentAST *node) {
    mObject *result = nullptr;

    mObject *left = node->declaration->Accept(this);
    INCREF(left);

    mObject *right = node->expression->Accept(this);
    INCREF(right);

    if (left == nullptr || right == nullptr) { return nullptr; }

    if (node->type.type != Token::Type::Equal) {
        mError::AddError("Unsupported operator " + node->type.value);

        DECREF(left);
        DECREF(right);
        return nullptr;
    }

    mlist* args = new mlist({ right });

    left->CallMethod("zAssign", args, nullptr);

    DECREF(args);
    DECREF(left);
    DECREF(right);
    return result;
}