#include "types.h"
#include "mfn.h"
#include "mint.h"
#include "mfloat.h"
#include "mstr.h"
#include "mbool.h"
#include "mnull.h"
#include "mlist.h"
#include "module.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "error.h"
#include "symbol.h"
#include "eval_visitor.h"

#define BEGIN_EVAL() \
    mSymbolTable* old = mSymbolTable::locals; \
    mSymbolTable::locals = new mSymbolTable(old);

#define END_EVAL() \
    delete mSymbolTable::locals; \
    mSymbolTable::locals = old;

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

EvalVisitor::EvalVisitor() { }

EvalVisitor::~EvalVisitor() { }

mObject* EvalVisitor::Eval(ASTNode *node, mSymbolTable *symbolTable, mModule *module) {
    EvalVisitor visitor;
    visitor.module = module;
    
    mSymbolTable* old = mSymbolTable::locals;
    mSymbolTable::locals = symbolTable;

    mList list = node->Accept(&visitor);

    mSymbolTable::locals = old;

    return list.items.size() == 0 ? nullptr : list[0];
}

mList EvalVisitor::Visit(ASTNode *node) {
    std::cout << "ASTNode" << std::endl;
    return {};
}

mList EvalVisitor::Visit(ProgramAST *node) {
    
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

mList EvalVisitor::Visit(PropertyExprAST *node) {
    mSymbolTable::Symbol* symbol = mSymbolTable::LocalsGetSymbol(node->name);
    
    if (symbol == nullptr) {
        ERROR("Name '" + node->name + "' is not defined");
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
            mSymbolTable::locals = new mSymbolTable(func->symbolTable);

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
                    ERROR("Expected " + std::to_string(func->args.size()) + " arguments, got " + std::to_string(args.items.size()));
                    delete mSymbolTable::locals;
                    mSymbolTable::locals = old;
                    return {};
                }

                if (value->type != func->args[i].type) {
                    ERROR("Argument " + std::to_string(i) + " must be of type '" + func->args[i].type->name + "'");
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
                ERROR("Expected return type '" + func->returnType->name + "', got '" + result->type->name + "'");
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return {};
            }

            // Unload symbol table
            delete mSymbolTable::locals;
            mSymbolTable::locals = old;
        }
        else {
            mObject* selfObject = nullptr;
            result = func->Call(&args, nullptr, selfObject);
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

    const std::unordered_map<Token::Type, const std::string> methods = {
        { Token::Type::Plus,            "mAdd"      },
        { Token::Type::Minus,           "mSub"      },
        { Token::Type::Star,            "mMul"      },
        { Token::Type::Slash,           "mDiv"      },
        { Token::Type::Percent,         "mMod"      },
        { Token::Type::EqualEqual,      "mEq"       },
        { Token::Type::NotEqual,        "mNeq"      },
        { Token::Type::Less,            "mLt"       },
        { Token::Type::LessEqual,       "mLe"       },
        { Token::Type::Greater,         "mGt"       },
        { Token::Type::GreaterEqual,    "mGe"       },
        { Token::Type::Amp,             "mAnd"      },
        { Token::Type::Pipe,            "mOr"       },
        { Token::Type::Caret,           "mXor"      },
        { Token::Type::LessLess,        "mLShift"   },
        { Token::Type::GreaterGreater,  "mRShift"   },
        { Token::Type::AmpAmp,          "mLAnd"     },
        { Token::Type::PipePipe,        "mLOr"      }
    };

    const std::string method = methods.at(node->op.type);

    if (left->HasMethod(method)) {
        result = left->CallMethod(method, &args, nullptr);

        if (mError::HasError()) { return {}; }
    }
    else {
        ERROR("Unsupported operator " + node->op.value + " for type '" + left->type->name + "'" + " and type '" + right->type->name + "'");
        return {};
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

mList EvalVisitor::Visit(AccessExprAST *node) {
    mObject* obj;
    mObjectRef* ref;
    mList ret = node->expr->Accept(this);

    if (ret.items.size() == 0) { return {}; }

    obj = ret[0];

    if (obj == nullptr) { return {}; }

    if (obj->HasField(node->name.value)) {
        mObject** fRef = obj->GetFieldRef(node->name.value);
        mType::mFieldInfo* fieldInfo = obj->type->GetFieldInfo(node->name.value);
        ref = new mObjectRef(fRef, fieldInfo->ftype, true);
        obj = obj->GetField(node->name.value);
    }
    else if (obj->HasMethod(node->name.value)) {
        obj = obj->GetMethod(node->name.value);
        mObject** fRef = nullptr;
        ref = new mObjectRef(fRef, obj->type, false);
    }
    else {
        ERROR("Object of type '" + obj->type->name + "' has no attribute '" + node->name.value + "'");
        return {};
    }
    
    return mList({ obj, ref });
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
        if (left->type != right->type && right->type != mNull::Type) {
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

    mList ret = node->type ? node->type->Accept(this) : mList({ mNull::Type });

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
            
            if (type != value->type && value->type != mNull::Type) {
                ERROR("Cannot assign type '" + value->type->name + "' to type '" + ((mType*)type)->name + "'");
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
    func->symbolTable = new mSymbolTable(mSymbolTable::locals);
    
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
    
    ((mFunction*) func)->name = node->name.value;

    mSymbolTable::locals->Set(
        node->name.value,
        func,
        mFunction::Type,
        false
    );

    return {};
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

        if (breakLoop) {
            breakLoop = false;
            break;
        }

        if (continueLoop) {
            continueLoop = false;
            continue;
        }

        ret = node->body->Accept(this);

        if (ret.items.size() != 0) { 
            mObject* retObj = ret[0];

            if (retObj != nullptr) {
                delete mSymbolTable::locals;
                mSymbolTable::locals = old;
                return mList({ retObj });
            }
        }
    } while (true);

    delete mSymbolTable::locals;
    mSymbolTable::locals = old;
    return {};
}

mList EvalVisitor::Visit(ForAST *node) {
    BEGIN_EVAL();

    mList ret = node->iterable->Accept(this);

    if (ret.items.size() == 0) { 
        END_EVAL();
        return {}; 
    }

    mList* iterable = (mList*) ret[0];

    if (iterable == nullptr) { 
        ERROR("Iterable cannot be null");
        END_EVAL();
        return {}; 
    }

    if (iterable->type != mList::Type) {
        ERROR("Iterable must be a list");
        END_EVAL();
        return {};
    }

    // Creteate loop variable
    node->variable->Accept(this);
    
    if (mError::HasError()) { 
        END_EVAL();
        return {}; 
    }

    mSymbolTable::Symbol* symbol = mSymbolTable::LocalsGetSymbol(((VarDeclarationAST*) node->variable)->identifier.value);

    for (int i = 0; i < iterable->items.size(); i++) {
        mObject* item = iterable->items[i];

        DECREF(symbol->value);

        if (symbol->type != mNull::Type && item->type != symbol->type) {
            ERROR("Expected type '" + symbol->type->name + "', got '" + item->type->name + "'");
            END_EVAL();
            return {};
        }

        symbol->value = item;
        
        if (breakLoop) {
            breakLoop = false;
            break;
        }

        if (continueLoop) {
            continueLoop = false;
            continue;
        }

        ret = node->body->Accept(this);

        if (ret.items.size() != 0) { 
            mObject* retObj = ret[0];

            if (retObj != nullptr) {
                END_EVAL();
                return mList({ retObj });
            }
        }
    }
    
    END_EVAL();
    return {};
}

mList EvalVisitor::Visit(BreakAST *node) {
    breakLoop = true;
    return mList({ new mNull() });
}

mList EvalVisitor::Visit(ContinueAST *node) {
    continueLoop = true;
    return mList({ new mNull() });
}

mList EvalVisitor::Visit(ImportAST *node) {
    if (node->isPath) {
        const std::string& path = node->path.value;

        mObject* module = mModule::ImportFile(path);

        if (module == nullptr) {
            std::cout << "Failed to import module '" << path << "'" << std::endl;
            return {};
        }

        for (auto& field : module->fields) {
            mSymbolTable::locals->Set(field.first, field.second, field.second->type, false);
        }

    }
    else {
        mObject* module = mModule::Import(node->identifiers[0].value);

        if (module == nullptr) {
            std::cout << "Failed to import module '" << node->identifiers[0].value << "'" << std::endl;
            return {};
        }

        mObject* obj = module;
        Token token = node->identifiers[0];
        
        for (int i = 1; i < node->identifiers.size(); i++) {
            const std::string& name = node->identifiers[i].value;

            if (!obj->HasField(name)) {
                std::cout << "Module '" << obj->ToString() << "' has no attribute '" << name << "'" << std::endl;
                return {};
            }

            obj = obj->GetField(name);
            token = node->identifiers[i];
        }

        if (node->alias.value != "")
            mSymbolTable::locals->Set(node->alias.value, obj, obj->type, false);
        else
            mSymbolTable::locals->Set(token.value, obj, obj->type, false);
    }

    return {};
}

mList EvalVisitor::Visit(ExportAST *node) {
    if (!module) { return {}; }

    for (ASTNode* node : node->list) {
        const ExportItemAST* item = (ExportItemAST*) node;

        if (item->expr) {
            mList ret = item->expr->Accept(this);

            if (ret.items.size() == 0) { return {}; }

            mObject* result = ret[0];

            if (result == nullptr) { return {}; }

            module->fields[item->name.value] = result;
        }
        else {
            mSymbolTable::Symbol* symbol = mSymbolTable::LocalsGetSymbol(item->name.value);

            if (symbol == nullptr) {
                ERROR("Name '" + item->name.value + "' is not defined");
                return {};
            }

            module->fields[item->name.value] = symbol->value;
        }
    }

    return mList();
}

mList EvalVisitor::Visit(ClassAST *node) {
    const std::string& name = node->name.value;

    if (mSymbolTable::locals->Exists(name, nullptr)) {
        mError::AddError("Name '" + name + "' is already defined");
        return {};
    }

    mType* type = new mType(name);

    for (auto& stmt : node->statements) {
        VarDeclarationAST* varDecl = dynamic_cast<VarDeclarationAST*>(stmt);
        
        if (varDecl) {
            mList typeRet = varDecl->type->Accept(this);
            if (typeRet.items.size() == 0) { return {}; }

            mObject* typeVar = typeRet[0];
            if (typeVar == nullptr) { return {}; }

            if (typeVar->type != mType::Type) {
                ERROR("Type '" + typeVar->ToString() + "' is not a valid type");
                return {};
            }

            type->SetFieldInfo(varDecl->identifier.value, (mType*) typeVar);
        }
        else {
            FunctionAST* funcDecl = dynamic_cast<FunctionAST*>(stmt);
            if (funcDecl) {
                mObject* func = funcDecl->lambda->Accept(this)[0];

                if (func == nullptr) { return {}; }
                
                ((mFunction*) func)->name = funcDecl->name.value;
                
                type->SetMethod(funcDecl->name.value, func);
            }
        }
    }

    mSymbolTable::locals->Set(name, type, mType::Type, false);

    return {};
}
