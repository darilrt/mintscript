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

class mObjectRef : public mObject {
public:
    mObject **ref;
    mType *type;
    bool isMutable;

    mObjectRef(mObject **ref, mType *type, bool isMutable) : ref(ref), type(type), isMutable(isMutable), mObject(mType::Type) { }

    std::string ToString() override {
        return "mObjectRef";
    }
};

mList EvalVisitor::Visit(ASTNode *node) {
    mList list = node->Accept(this);
    mList ret = mList({ list[0] });
	list.Clear();
    return ret;
}

mList EvalVisitor::Visit(NumberExprAST *node) {
    mObject *result = nullptr;

    switch (node->type) {
        case NumberExprAST::Type::Int: result = new mInt((int)node->value); break;
        case NumberExprAST::Type::Float: result = new mFloat((float)node->value); break;
    }

    return result ? mList({ result }) : mList();
}

mList EvalVisitor::Visit(StringExprAST *node) {
    return mList({ new mStr(node->value) });
}

mList EvalVisitor::Visit(BoolExprAST *node) {
    return mList({ node->value ? mBool::True : mBool::False });
}

mList EvalVisitor::Visit(NullExprAST *node) {
    return mList({ mNull::Null });
}

mList EvalVisitor::Visit(LambdaExprAST *node) {
    std::cout << "LambdaExprAST" << std::endl;
    return {};
}

mList EvalVisitor::Visit(PropertyExprAST *node) {
    mSymbolTable::Symbol* symbol = mSymbolTable::globals->GetSymbol(node->name);

    if (symbol == nullptr) {
        std::cout << "Name '" << node->name << "' is not defined" << std::endl;
        return {};
    }

    mObject *result = symbol->value;
    mObjectRef *ref = new mObjectRef(&symbol->value, symbol->type, symbol->isMutable);

    return mList({ result, ref });
}

mList EvalVisitor::Visit(IndexExprAST *node) {
    mList list = node->expr->Accept(this);
    
    mObject* object = list[0];
    if (object == nullptr) { return {}; }

    list = node->index->Accept(this);
    mObject* index = list[0];

    if (index == nullptr) { return {}; }

    mList args({ index });
    mObject *result = object->CallMethod("mGet", &args, nullptr);

	if (result == nullptr) { return {}; }

    return mList({ result });
}

mList EvalVisitor::Visit(CallExprAST *node) {
    mList list = node->property->Accept(this);
    mObject* object = list[0];

    if (object == nullptr) {
        return {}; // TODO: Error
    }

    mList args;

    for (auto arg : node->args) {
        mList list = arg->Accept(this);
        mObject* result = list[0];

        if (result == nullptr) {
            return {}; // TODO: Error
        }

        args.items.push_back(result);
    }

    mObject *result = object->CallMethod("mCall", &args, nullptr);

    return mList({ result });
}

mList EvalVisitor::Visit(UnaryExprAST *node) {
    mObject *result = nullptr;

    mList list = node->expr->Accept(this);
    mObject* operand = list[0];

    if (operand == nullptr) {
        return {};
    }

    mList args({ operand });

    // Operator overloading

    switch (node->op.type) {
        case Token::Type::Plus:             result = operand->CallMethod("mPos",       &args, nullptr); break; // +
        case Token::Type::Minus:            result = operand->CallMethod("mNeg",       &args, nullptr); break; // -
        case Token::Type::Not:              result = operand->CallMethod("mNot",       &args, nullptr); break; // !

        case Token::Type::PlusPlus: {
            if (node->isPrefix) {           result = operand->CallMethod("mPreInc",    &args, nullptr); break; } // ++
            else {                          result = operand->CallMethod("mPostInc",   &args, nullptr); break; } // ++
            break;
        }

        case Token::Type::MinusMinus: {
            if (node->isPrefix) {           result = operand->CallMethod("mPreDec",    &args, nullptr); break; } // --
            else {                          result = operand->CallMethod("mPostDec",   &args, nullptr); break; } // --
            break;
        }
    }
    
    return mList({ result });
}

mList EvalVisitor::Visit(BinaryExprAST *node) {
    mObject *result = nullptr;

    mList ret = node->lhs->Accept(this);
    if (ret.items.size() == 0) { return {}; }
    mObject *left = ret[0];

    ret = node->rhs->Accept(this);
    if (ret.items.size() == 0) { return {}; }
    mObject *right = ret[0];

    if (left == nullptr || right == nullptr) { return {}; }

    mList args({ right });

    // Operator overloading
    // TODO: Move this to a separate function
    switch (node->op.type) {
        case Token::Type::Plus:             result = left->CallMethod("mAdd",       &args, nullptr); break; // +
        case Token::Type::Minus:            result = left->CallMethod("mSub",       &args, nullptr); break; // -
        case Token::Type::Star:             result = left->CallMethod("mMul",       &args, nullptr); break; // *
        case Token::Type::Slash:            result = left->CallMethod("mDiv",       &args, nullptr); break; // /
        case Token::Type::Percent:          result = left->CallMethod("mMod",       &args, nullptr); break; // %
        case Token::Type::EqualEqual:       result = left->CallMethod("mEq",        &args, nullptr); break; // ==
        case Token::Type::NotEqual:         result = left->CallMethod("mNeq",       &args, nullptr); break; // !=
        case Token::Type::Less:             result = left->CallMethod("mLt",        &args, nullptr); break; // <
        case Token::Type::LessEqual:        result = left->CallMethod("mLe",        &args, nullptr); break; // <=
        case Token::Type::Greater:          result = left->CallMethod("mGt",        &args, nullptr); break; // >
        case Token::Type::GreaterEqual:     result = left->CallMethod("mGe",        &args, nullptr); break; // >=
        case Token::Type::Amp:              result = left->CallMethod("mAnd",       &args, nullptr); break; // &
        case Token::Type::Pipe:             result = left->CallMethod("mOr",        &args, nullptr); break; // |
        case Token::Type::Caret:            result = left->CallMethod("mXor",       &args, nullptr); break; // ^ 
        case Token::Type::LessLess:         result = left->CallMethod("mLShift",    &args, nullptr); break; // <<
        case Token::Type::GreaterGreater:   result = left->CallMethod("mRShift",    &args, nullptr); break; // >>
        case Token::Type::AmpAmp:           result = left->CallMethod("mLAnd",      &args, nullptr); break; // &&
        case Token::Type::PipePipe:         result = left->CallMethod("mLOr",       &args, nullptr); break; // ||
    }

    if (result == nullptr) {
        std::cout << "Unsupported operator " << node->op.value;
        std::cout << " for types '" << left->type->name << "' and '" << right->type->name << "'" << std::endl;
    }
    
    return mList({ result });
}

mList EvalVisitor::Visit(TernaryExprAST *node) {
    return {};
}

mList EvalVisitor::Visit(ParenExprAST *node) {
	mList list = node->expr->Accept(this);
	return list;
}

mList EvalVisitor::Visit(AssignmentAST *node) {
    mList leftRet = node->declaration->Accept(this);

    if (leftRet.items.size() <= 1) {
        mError::AddError("Invalid assignment");
        return {}; 
    }

	mObject* prev = leftRet[0];
    mObjectRef* left = (mObjectRef*)leftRet[1];

    if (left == nullptr) { return {}; }
    if (!left->isMutable) {
        std::cout << "Cannot assign to immutable variable" << std::endl;
        return {};
    }
    
    mList rightRet = node->expression->Accept(this);

    if (rightRet.items.size() <= 0) { 
        mError::AddError("Invalid value");
        return {}; 
    }

    mObject* right = rightRet[0];
    
    if (right == nullptr) { return {}; }

    if (node->type.type != Token::Type::Equal) {
        if (left->type != right->type) {
            std::cout << "Cannot assign type '" << right->type->name << "' to type '" << left->type->name << "'" << std::endl;

            return {};
        }

        (*left->ref) = right;
		INCREF(right);
    }

    return mList({ *left->ref });
}

mList EvalVisitor::Visit(VarDeclarationAST *node) {
    mObject *result = nullptr;

    if (!node->isMutable && node->expression == nullptr) {
        mError::AddError("Cannot declare immutable variable without an initial value");
        return {};
    }

    mList ret = node->type->Accept(this);

    if (ret.items.size() == 0) { return {}; }
    
    mObject *type = ret[0];

    if (type == nullptr) { return {}; }

    if (type->type != mType::Type) {
        mError::AddError("Type '" + type->ToString() + "' is not a valid type");
        return {};
    }

    const std::string& name = node->identifier.value;

    if (mSymbolTable::locals->Exists(name, nullptr)) {
        mError::AddError("Name '" + name + "' is already defined");
        return {};
    }

    mObject *value = mNull::Null;

    if (node->expression != nullptr) {
        ret = node->expression->Accept(this);
        
        if (ret.items.size() == 0) { value = mNull::Null; }
        
        INCREF(value);

        value = ret[0] == nullptr ? mNull::Null : ret[0];

        if (type != value->type) {
            std::cout << "Cannot assign type '" << value->type->name << "' to type '" << ((mType*) type)->name << "'" << std::endl;
            return {};
        }
    }
    
    mSymbolTable::locals->Set(name, value, (mType*)type, node->isMutable);
    
    return {};
}

mList EvalVisitor::Visit(LambdaAST *node) {
    mFunction *func = new mFunction([] (mObject *args, mObject *kwargs, mObject *self) -> mObject* {
        std::cout << "Lambda function called" << std::endl;
        return nullptr;
    });


    return mList({ func });
}
