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

    mObjectRef(mObject **ref) : ref(ref), mObject(mType::Type) {}

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
    mObject *result = zSymbolTable::locals->Get(node->name);
    mObjectRef *ref = new mObjectRef(zSymbolTable::locals->GetRef(node->name));

    if (result == nullptr) {
        std::cout << "Name '" << node->name << "' is not defined" << std::endl;
        std::cout << " but we can define it for you as null" << std::endl;
        
        zSymbolTable::locals->Set(node->name, mNull::Null);
        result = zSymbolTable::locals->Get(node->name); 
    }

    return mList({ result, ref });
}

mList EvalVisitor::Visit(IndexExprAST *node) {
    mList list = node->expr->Accept(this);
    mObject* object = list[0];
	INCREF(object);
    list.Clear();

    if (object == nullptr) {
        return {}; // TODO: Error
    }

    list = node->index->Accept(this);
    mObject* index = list[0];
	INCREF(index);
    list.Clear();

    if (index == nullptr) {
        return {}; // TODO: Error
    }

    mList args({ index });
    mObject *result = object->CallMethod("zGet", &args, nullptr);
    
	DECREF(object);

    return mList({ result });
}

mList EvalVisitor::Visit(CallExprAST *node) {
    mList list = node->property->Accept(this);
    mObject* object = list[0];
	INCREF(object);
    list.Clear();

    if (object == nullptr) {
        return {}; // TODO: Error
    }

    mList args;
    mObject *result = object->CallMethod("zCall", nullptr, nullptr);

	DECREF(object);

    return mList({ result });
}

mList EvalVisitor::Visit(UnaryExprAST *node) {
    mObject *result = nullptr;

    mList list = node->expr->Accept(this);
    mObject* operand = list[0];
    INCREF(operand);
    list.Clear();

    if (operand == nullptr) {
        return {};
    }

    mList args({ operand });

    // Operator overloading

    switch (node->op.type) {
        case Token::Type::Plus:             result = operand->CallMethod("zPos",       &args, nullptr); break; // +
        case Token::Type::Minus:            result = operand->CallMethod("zNeg",       &args, nullptr); break; // -
        case Token::Type::Not:              result = operand->CallMethod("zNot",       &args, nullptr); break; // !

        case Token::Type::PlusPlus: {
            if (node->isPrefix) {           result = operand->CallMethod("zPreInc",    &args, nullptr); break; } // ++
            else {                          result = operand->CallMethod("zPostInc",   &args, nullptr); break; } // ++
            break;
        }

        case Token::Type::MinusMinus: {
            if (node->isPrefix) {           result = operand->CallMethod("zPreDec",    &args, nullptr); break; } // --
            else {                          result = operand->CallMethod("zPostDec",   &args, nullptr); break; } // --
            break;
        }
    }

    DECREF(operand);
    
    return mList({ result });
}

mList EvalVisitor::Visit(BinaryExprAST *node) {
    mObject *result = nullptr;

    mList ret = node->lhs->Accept(this);
    if (ret.items.size() == 0) { return {}; }
    mObject *left = ret[0];
	INCREF(left);
    ret.Clear();

    ret = node->rhs->Accept(this);
    if (ret.items.size() == 0) { return {}; }
    mObject *right = ret[0];
	INCREF(right);
    ret.Clear();

    if (left == nullptr || right == nullptr) { return {}; }

    mList args({ right });

    // Operator overloading
    // TODO: Move this to a separate function
    switch (node->op.type) {
        case Token::Type::Plus:             result = left->CallMethod("zAdd",       &args, nullptr); break; // +
        case Token::Type::Minus:            result = left->CallMethod("zSub",       &args, nullptr); break; // -
        case Token::Type::Star:             result = left->CallMethod("zMul",       &args, nullptr); break; // *
        case Token::Type::Slash:            result = left->CallMethod("zDiv",       &args, nullptr); break; // /
        case Token::Type::Percent:          result = left->CallMethod("zMod",       &args, nullptr); break; // %
        case Token::Type::EqualEqual:       result = left->CallMethod("zEq",        &args, nullptr); break; // ==
        case Token::Type::NotEqual:         result = left->CallMethod("zNeq",       &args, nullptr); break; // !=
        case Token::Type::Less:             result = left->CallMethod("zLt",        &args, nullptr); break; // <
        case Token::Type::LessEqual:        result = left->CallMethod("zLe",        &args, nullptr); break; // <=
        case Token::Type::Greater:          result = left->CallMethod("zGt",        &args, nullptr); break; // >
        case Token::Type::GreaterEqual:     result = left->CallMethod("zGe",        &args, nullptr); break; // >=
        case Token::Type::Amp:              result = left->CallMethod("zAnd",       &args, nullptr); break; // &
        case Token::Type::Pipe:             result = left->CallMethod("zOr",        &args, nullptr); break; // |
        case Token::Type::Caret:            result = left->CallMethod("zXor",       &args, nullptr); break; // ^ 
        case Token::Type::LessLess:         result = left->CallMethod("zLShift",    &args, nullptr); break; // <<
        case Token::Type::GreaterGreater:   result = left->CallMethod("zRShift",    &args, nullptr); break; // >>
        case Token::Type::AmpAmp:           result = left->CallMethod("zLAnd",      &args, nullptr); break; // &&
        case Token::Type::PipePipe:         result = left->CallMethod("zLOr",       &args, nullptr); break; // ||
    }

    if (result == nullptr) {
        std::cout << "Unsupported operator " << node->op.value;
        std::cout << " for types '" << left->type->name << "' and '" << right->type->name << "'" << std::endl;
    }
    
    DECREF(left);
    DECREF(right);
    
    return mList({ result });
}

mList EvalVisitor::Visit(TernaryExprAST *node) {
    return {};
}

mList EvalVisitor::Visit(ParenExprAST *node) {
	mList list = node->expr->Accept(this);
	mList ret = mList({ list[0] });
	list.Clear();
	return ret;
}

mList EvalVisitor::Visit(AssignmentAST *node) {
    mObject *result = nullptr;

    mList ret = node->declaration->Accept(this);

    if (ret.items.size() <= 1) { 
        mError::AddError("Invalid assignment");
        return {}; 
    }

	mObject* prev = ret[0];
    INCREF(prev);
    mObjectRef* left = (mObjectRef*) ret[1];
	INCREF(left);
    ret.Clear();
    
    ret = node->expression->Accept(this);
    mObject* right = ret[0];
	INCREF(right);
    ret.Clear();

    if (left == nullptr || right == nullptr) { return {}; }

    if (node->type.type != Token::Type::Equal) {
        
		DECREF(prev);
        (*left->ref) = right;
		INCREF(right);
        
        return {};
    }

	DECREF(prev);
	DECREF(left);
	DECREF(right);

    return mList({ result });
}