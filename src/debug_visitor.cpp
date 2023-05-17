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
    std::cout << "ASTNode" << std::endl;
    return {};
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
    mSymbolTable::Symbol* symbol = mSymbolTable::LocalsGetSymbol(node->name);
    
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

    if (!object->HasMethod("mGet")) {
        mError::AddError("Object does not have method 'mGet'");
        return {};
    }

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

    if (mError::HasError()) { return {}; }
    
    if (list.items.size() == 0) {
		return {};
	}

    mObject* object = list[0];

    if (object == nullptr) {
        return {};
    }

    mList args;

    for (auto arg : node->args) {
        mList list = arg->Accept(this);
        
        if (mError::HasError()) { return {}; }

        if (list.items.size() == 0) {
            mError::AddError("Expected argument");
            return {}; // TODO: Error
        }

        mObject* value = list[0];

        if (value == nullptr) {
            mError::AddError("Expected argument");
            return {}; // TODO: Error
        }
        
        args.items.push_back(value);
    }

    mFunction* func = dynamic_cast<mFunction*>(object);
    mObject* result = nullptr;

    if (func) {
        if (func->ast) {
            // Load symbol table
            mSymbolTable* old = mSymbolTable::locals;
            mSymbolTable::locals = new mSymbolTable(old);

            // Load arguments
            if (args.items.size() != func->args.size()) {
                mError::AddError("Expected " + std::to_string(func->args.size()) + " arguments, got " + std::to_string(args.items.size()));
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return {};
            }
            
            for (int i = 0; i < args.items.size(); i++) {
                mObject* value = args[i];
                
                if (i >= func->args.size()) {
                    std::cout << "Too many arguments" << std::endl;
                    delete mSymbolTable::locals;
                    mSymbolTable::locals = old;
                    return {};
                }

                if (value->type != func->args[i].type) {
                    std::cout << "Argument type mismatch" << std::endl;
                    delete mSymbolTable::locals;
                    mSymbolTable::locals = old;
                    return {};
                }

                mSymbolTable::locals->Set(func->args[i].name, value, func->args[i].type, false);
            }
            
            // Call lambda
            LambdaAST* lambda = (LambdaAST*) func->ast;

            mList ret = lambda->body->Accept(this);

            if (ret.items.size() == 0 || mError::HasError()) {
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return {};
            }

            result = ret[0];

            if (result->type != func->returnType) {
                std::cout << "Return type mismatch" << std::endl;
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return {};
            }

            // Unload symbol table
            delete mSymbolTable::locals;
            mSymbolTable::locals = old;
        }
        else {
            result = func->Call(&args, nullptr, nullptr);
        }
    }
    else {
        result = object->CallMethod("mCall", &args, nullptr);
    }
    
    return result ? mList({ result }) : mList();
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

mList EvalVisitor::Visit(ArrayExprAST *node) {
    mList* array = new mList();

    for (auto& expr : node->values) {
        mList list = expr->Accept(this);
        if (list.items.size() == 0) { return {}; }
        mObject* item = list[0];

        if (item == nullptr) { return {}; }

        array->items.push_back(item);
    }

    return mList({ array });
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

    return {};
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
        
        if (ret.items.size() == 0) {
            value = mNull::Null; 
        }
        else {
            value = ret[0] == nullptr ? mNull::Null : ret[0];

            if (type != value->type) {
                std::cout << "Cannot assign type '" << value->type->name << "' to type '" << ((mType*)type)->name << "'" << std::endl;
                return {};
            }
        }

        INCREF(value);
    }
    
    mSymbolTable::locals->Set(name, value, (mType*)type, node->isMutable);
    
    return {};
}

mList EvalVisitor::Visit(LambdaAST *node) {
    mFunction* func = new mFunction();
    func->ast = node;

    // Resolve return type
    if (node->returnType) {
        mList ret = node->returnType->Accept(this);

        if (ret.items.size() == 0) {
            func->returnType = mNull::Type; // TODO: Change this to void
        }
        else {
            if (ret[0]->type != mType::Type) {
                mError::AddError("Type '" + ret[0]->ToString() + "' is not a valid type");
                return {};
            }

            func->returnType = (mType*) ret[0];
        }
    }
    else {
        func->returnType = mNull::Type; // TODO: Change this to void
    }

    // Resolve arguments
    for (auto& arg : node->parameters) {
        const ArgDeclAST* argDecl = (ArgDeclAST*) arg;

        mList type = argDecl->type->Accept(this);
        if (type.items.size() == 0) { 
            mError::AddError("Invalid type");
            return {}; 
        }

        mType* argType = (mType*) type[0];
        if (argType == nullptr) {
            mError::AddError("Invalid type");
            return {};
        }

        if (argType->type != mType::Type) {
            mError::AddError("Type '" + argType->ToString() + "' is not a valid type");
            return {};
        }

        func->args.push_back({
            argDecl->identifier.value,
            argType
        });
    }
    
    // Generate symbol table
    mSymbolTable* table = new mSymbolTable(mSymbolTable::locals);
    func->symbolTable = table;

    return mList({ func });
}

mList EvalVisitor::Visit(ArgDeclAST *node) {
    return {};
}

mList EvalVisitor::Visit(BlockAST *node) {
    mSymbolTable* old = mSymbolTable::locals;
    mSymbolTable::locals = new mSymbolTable(old);

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        mList res = stmt->Accept(this);

        if (mError::HasError()) { return {}; }

        if (res.items.size() > 0) {
            mObject* result = res[0];

            if (result != nullptr) {
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return mList({ result });
            }
        }
    }

    delete mSymbolTable::locals;
    mSymbolTable::locals = old;
    
    return {};
}

mList EvalVisitor::Visit(ReturnAST *node) {
    mList ret = node->expression->Accept(this);

    if (ret.items.size() == 0) { return {}; }

    mObject* result = ret[0];
    
    if (result == nullptr) { return {}; }

    return mList({ result });
}

mList EvalVisitor::Visit(FunctionAST *node) {
    mObject* func = node->lambda->Accept(this)[0];

    if (func == nullptr) { return {}; }

    mSymbolTable::locals->Set(
        node->name.value,
        func,
        mFunction::Type,
        false
    );

    return mList();
}

mList EvalVisitor::Visit(IfAST* node) {
    mList ret = node->condition->Accept(this);

    if (ret.items.size() == 0) { return {}; }

    mBool* result = (mBool*) ret[0];

    if (result == nullptr) { return {}; }

    if (result->type != mBool::Type) {
        mError::AddError("Condition must be a boolean");
        return {};
    }

    if ((result)->value) {
        return node->body->Accept(this);
    }
    else {
        for (auto& elif : node->elseIfs) {
            ret = elif->condition->Accept(this);

            if (ret.items.size() == 0) { return {}; }

            result = (mBool*) ret[0];

            if (result == nullptr) { return {}; }

            if (result->type != mBool::Type) {
                mError::AddError("Condition must be a boolean");
                return {};
            }

            if ((result)->value) {
                return elif->body->Accept(this);
            }
        }

        if (node->elseBody != nullptr) {
            return node->elseBody->Accept(this);
        }
    }

    return {};
}

static bool breakLoop = false;
static bool continueLoop = false;

mList EvalVisitor::Visit(WhileAST *node) {
    mSymbolTable* old = mSymbolTable::locals;
    mSymbolTable::locals = new mSymbolTable(old);

    do {
        mList ret = node->condition->Accept(this);

        if (ret.items.size() == 0) { 
            delete mSymbolTable::locals;
            mSymbolTable::locals = old;
            return {}; 
        }

        mBool* result = (mBool*) ret[0];

        if (result == nullptr) { 
            delete mSymbolTable::locals;
            mSymbolTable::locals = old;
            return {}; 
        }

        if (result->type != mBool::Type) {
            mError::AddError("Condition must be a boolean");
            delete mSymbolTable::locals;
            mSymbolTable::locals = old;
            return {};
        }

        if (!(result)->value) {
            break;
        }

        ret = node->body->Accept(this);

        if (breakLoop) {
            breakLoop = false;
            break;
        }

        if (continueLoop) {
            continueLoop = false;
            continue;
        }

        if (ret.items.size() != 0) { 
            mObject* retObj = ret[0];

            if (retObj != nullptr) {
                std::cout << retObj->ToString() << std::endl;
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return mList({ retObj });
            }
        }
    } while (true);

    delete mSymbolTable::locals;
    mSymbolTable::locals = old;
    return mList();
}

mList EvalVisitor::Visit(BreakAST *node) {
    breakLoop = true;
    return mList({ new mNull() });
}

mList EvalVisitor::Visit(ContinueAST *node) {
    continueLoop = true;
    return mList({ new mNull() });
}
