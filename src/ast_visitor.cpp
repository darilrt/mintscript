#include "ast.h"
#include "expr.h"
#include "decl.h"
#include "token.h"
#include "ast_visitor.h"
#include "builtin.h"
#include "MintScript.h"

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

inline bool IsPrimitive(sa::Type* type) {
    return type == t_int || type == t_float || type == t_str || type == t_bool;
}

void AstVisitor::PushScope() {
    sa::SymbolTable* newTable = new sa::SymbolTable(table);
    table = newTable;
}

void AstVisitor::PopScope() {
    sa::SymbolTable* oldTable = table;
    table = table->GetParent();
    delete oldTable;
}

AstVisitor::AstVisitor() { }

AstVisitor::~AstVisitor() { }

AstVisitor* AstVisitor::Eval(ASTNode *node, std::string moduleName) {
    AstVisitor* visitor = new AstVisitor();
    
    visitor->moduleName = moduleName;
    visitor->stack.push(ins(ir::Scope, 0, { }));
    visitor->table = sa::global;

    sa::Type* list = node->Accept(visitor);

    return visitor;
}

sa::Type* AstVisitor::Visit(ASTNode *node) {
    std::cout << "ASTNode" << std::endl;
    return {};
}

sa::Type* AstVisitor::Visit(ProgramAST *node) {

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        sa::Type* res = stmt->Accept(this);

        if (mError::HasError()) { return {}; }
    }

    return t_null;
}

sa::Type* AstVisitor::Visit(NumberExprAST *node) {
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

sa::Type* AstVisitor::Visit(StringExprAST *node) {
    PUSH_INST(ins(ir::String, node->value, { }));
    return t_str;
}

sa::Type* AstVisitor::Visit(BoolExprAST *node) {
    PUSH_INST(ins(ir::Bool, node->value, { }));
    return t_bool;
}

sa::Type* AstVisitor::Visit(NullExprAST *node) {
    PUSH_INST(ins(ir::Null, { }));
    return t_null;
}

sa::Type* AstVisitor::Visit(PropertyExprAST *node) {
    sa::Symbol* sym = table->GetSymbol(node->name);

    if (sym == nullptr) {
        sa::Type* typ = table->GetType(node->name);

        if (typ == nullptr) {
            mError::AddError("Symbol '" + node->name + "' not found");
            return t_null;
        }

        PUSH_INST(ins(ir::Var, node->name, { }));
        return t_type;
    }
    
    PUSH_INST(ins(ir::Var, sym->name, { }));
    return sym->type;
}

sa::Type* AstVisitor::Visit(IndexExprAST *node) {
    throw std::runtime_error("IndexExprAST not implemented");
    return t_null;
}

sa::Type* AstVisitor::Visit(CallExprAST *node) {
    ir::Instruction* inst = ins(ir::Call, { });

    STACK_PUSH_I(inst);

    sa::Type* ptype = node->property->Accept(this);
    sa::Type* type = t_null;

    if (ptype == t_type) {
        const std::string name = *inst->GetArg(0)->value.s;
        type = table->GetType(name);

        if (type == nullptr) {
            mError::AddError("Type '" + name + "' not found");
            return t_null;
        }

        if (type->HasMethod(name)) {
            delete inst->GetArg(0)->value.s;
            inst->GetArg(0)->value.s = new std::string(type->GetMethod(name)->name);
            inst->GetArgs().push_back(ins(ir::New, (int) type->fields.size(), { }));
        }
        else {
            inst->SetInstruction(ir::New);
            inst->value.i = (int) type->fields.size();
        }
    }
    else if (ptype->IsVariantOf(table->GetType("Function"))) {
        type = ptype->typeParameters[0];
    }
    else {
        mError::AddError("Cannot call '" + ptype->name + "'");
        return t_null;
    }

    std::vector<ir::Instruction*>& args = inst->GetArg(0)->GetArgs();

    if (args.size() > 0) {
        if (args[0]->GetInstruction() == ir::Var || args[0]->GetInstruction() == ir::Field) {
            PUSH_INST(ins(ir::Val, { args[0] }));
        }
        else {
            PUSH_INST(args[0]);
        }
    }

    if (ptype->typeParameters.size() == (node->args.size() + 1)) {
        for (int i = 0; i < node->args.size(); i++) {
            sa::Type* type = node->args[i]->Accept(this);
            sa::Type* expected = ptype->typeParameters[i + 1];

            if (type != expected) {
                mError::AddError("Type mismatch expected '" + expected->name + "' got '" + type->name + "'");
                return t_null;
            }

            auto& args = inst->GetArgs();

            if (args[args.size() - 1]->GetInstruction() == ir::Var || args[args.size() - 1]->GetInstruction() == ir::Field) {
                args[args.size() - 1] = ins(ir::Val, { args[args.size() - 1] });
            }

            if (type == t_type) {
                mError::AddError("Cannot pass type as argument");
                return t_null;
            }
        }
    }
    else {
        mError::AddError("Expected " + std::to_string(ptype->typeParameters.size() - 1) + " arguments got " + std::to_string(node->args.size()));
        return t_null;
    }


    STACK_POP();

    return type;
}

sa::Type* AstVisitor::Visit(UnaryExprAST *node) {
    throw std::runtime_error("UnaryExprAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(BinaryExprAST *node) {
    ir::Instruction* inst = ins(ir::AddI, { });

    STACK_PUSH_I(inst);
    sa::Type* lhst = node->lhs->Accept(this);
    sa::Type* rhst = node->rhs->Accept(this);
    STACK_POP();

    if (inst->GetArg(0)->GetInstruction() == ir::Var || inst->GetArg(0)->GetInstruction() == ir::Field) {
        inst->GetArgs()[0] = ins(ir::Val, { inst->GetArg(0) });
    }

    if (inst->GetArg(1)->GetInstruction() == ir::Var || inst->GetArg(1)->GetInstruction() == ir::Field) {
        inst->GetArgs()[1] = ins(ir::Val, { inst->GetArg(1) });
    }

    if (
        node->op.type == Token::Type::AmpAmp ||
        node->op.type == Token::Type::PipePipe
    ) {
        switch (node->op.type) {
            case Token::Type::AmpAmp: inst->SetInstruction(ir::And); return t_bool;
            case Token::Type::PipePipe: inst->SetInstruction(ir::Or); return t_bool;
            default: break;
        }

        return t_bool;
    }

    sa::Type* type = lhst;

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

        case Token::Type::NotEqual: inst->SetInstruction(type == t_float ? ir::NeqF : ir::NeqI); break;
        case Token::Type::EqualEqual: inst->SetInstruction(type == t_float ? ir::EqF : ir::EqI); break;
        case Token::Type::Less: inst->SetInstruction(type == t_float ? ir::LtF : ir::LtI); break;
        case Token::Type::LessEqual: inst->SetInstruction(type == t_float ? ir::LeqF : ir::LeqI); break;
        case Token::Type::Greater: inst->SetInstruction(type == t_float ? ir::GtF : ir::GtI); break;
        case Token::Type::GreaterEqual: inst->SetInstruction(type == t_float ? ir::GeqF : ir::GeqI); break;

        default: break;
    }

    return type;
}

sa::Type* AstVisitor::Visit(TernaryExprAST *node) {
    throw std::runtime_error("TernaryExprAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(ParenExprAST *node) {
    return node->expr->Accept(this);
}

sa::Type* AstVisitor::Visit(ArrayExprAST *node) {
    throw std::runtime_error("ArrayExprAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(AccessExprAST *node) {
    ir::Instruction* inst = ins(ir::Field, { });

    STACK_PUSH(inst);
    sa::Type* type = node->expr->Accept(this);
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

sa::Type* AstVisitor::Visit(AssignmentAST *node) {
    ir::Instruction* inst = ins(ir::Set, { });

    STACK_PUSH_I(inst);
    sa::Type* type = node->declaration->Accept(this);

    sa::Type* expr = node->expression->Accept(this);

    if (type != expr) {
        mError::AddError("Type mismatch expected '" + type->name + "' got '" + expr->name + "'");
        return t_null;
    }
    STACK_POP();

    return t_null;
}

sa::Type* AstVisitor::Visit(VarDeclarationAST *node) {
    const std::string parentName = nameStack.size() > 0 ? nameStack.top() : "";

    sa::Type* clazz = t_null;

    if (parentName != "") {
        clazz = table->GetType(nameStack.top());
        
        if (clazz == nullptr) {
            mError::AddError("Class '" + nameStack.top() + "' not found");
            return t_null;
        }

        clazz->SetField(node->identifier.value, { node->isMutable, node->type->Accept(this) });

        if (node->expression) {
            sa::Type* value = node->expression->Accept(this);

            if (clazz->GetField(node->identifier.value)->type != value) {
                mError::AddError("Type mismatch");
                return t_null;
            }
        }
        else if (!node->isMutable) {
            mError::AddError("Cannot declare immutable variable without value");
            return t_null;
        }

        return t_null;
    }

    sa::Type* type = node->type->Accept(this);

    const std::string vname = "v" + moduleName + node->identifier.value;

    table->SetSymbol(node->identifier.value, { node->isMutable, vname, type });
    STACK_PUSH_I(ins(ir::Set, {
        ins(ir::Decl, vname, { })
    }));
    
    if (node->expression) {
        sa::Type* value = node->expression->Accept(this);

        if (type != value) {
            mError::AddError("Type mismatch");
            return t_null;
        }
    }
    else if (!node->isMutable) {
        mError::AddError("Cannot declare immutable variable without value");
        return t_null;
    }
    else {
        PUSH_INST(ins(ir::Null, { }));
    }

    STACK_POP();

    return t_null;
}

sa::Type* AstVisitor::Visit(LambdaAST *node) {
    throw std::runtime_error("LambdaAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(ArgDeclAST *node) {
    throw std::runtime_error("ArgDeclAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(BlockAST *node) {
    sa::Type* res = t_null;

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        if (dynamic_cast<ReturnAST*>(stmt) != nullptr) {
            res = stmt->Accept(this);
            break;
        }

        res = stmt->Accept(this);

        if (mError::HasError()) { return t_null; }
    }

    return res;
}

sa::Type* AstVisitor::Visit(ReturnAST *node) {   
    STACK_PUSH_I(ins(ir::Return, { }));
    
    sa::Type* type = node->expression ? node->expression->Accept(this) : t_null;

    STACK_POP();

    return type;
}

sa::Type* AstVisitor::Visit(FunctionAST *node) {
    const std::string fname = "f" + moduleName + node->name.value;

    sa::Type* rettype = node->lambda->returnType ? node->lambda->returnType->Accept(this) : t_null;

    PUSH_INST(ins(ir::Decl, fname, { }));

    STACK_PUSH_I(ins(ir::Set, {
        ins(ir::Var, fname, { }),
    }));

    STACK_PUSH_I(ins(ir::IR, { }));

    table->SetSymbol(node->name.value, { 
        false, 
        fname, 
        t_null
    });

    sa::Symbol* sym = table->GetSymbol(node->name.value);

    PushScope();

    int i = 0;

    std::vector<sa::Type*> argtypes = { rettype };

    for (; i < node->lambda->parameters.size(); i++) {
        ArgDeclAST* argDecl = (ArgDeclAST*) node->lambda->parameters[i];
        const std::string argName = "v" + moduleName + argDecl->identifier.value;
        
        PUSH_INST(ins(ir::Decl, argName, { }));
        PUSH_INST(ins(ir::Set, {
            ins(ir::Var, argName, { }),
            ins(ir::Arg, i, { })
        }));

        sa::Type *argType = argDecl->type->Accept(this);

        argtypes.push_back(argType);
        
        table->SetSymbol(argDecl->identifier.value, { 
            false,
            argName, 
            argType
        });
    }
    
    sym->type = table->GetTypeVariant("Function", argtypes);

    sa::Type* retsym = node->lambda->body->Accept(this);
    
    if (retsym != rettype) {
        mError::AddError("Function '" + node->name.value + "' return type mismatch expected '" + rettype->name + "' got '" + retsym->name + "'");
        return t_null;
    }

    PopScope();
    STACK_POP();
    STACK_POP();
 
    return t_null;
}

sa::Type* AstVisitor::Visit(IfAST* node) {
    STACK_PUSH_I(ins(ir::If, { }));

    sa::Type* type = node->condition->Accept(this);

    // Add body {
    STACK_PUSH_I(ins(ir::Scope, { }));
    PushScope();

    sa::Type* res = node->body->Accept(this);

    if (mError::HasError()) { return t_null; }

    PopScope();
    STACK_POP();
    // }

    // Add else {
    if (node->elseIfs.size() == 0 && node->elseBody != nullptr) {
        STACK_PUSH_I(ins(ir::Scope, { }));
        PushScope();

        sa::Type* res = node->elseBody->Accept(this);

        if (mError::HasError()) { return t_null; }

        PopScope();
        STACK_POP();
    }

    if (node->elseIfs.size() > 0) {
        for (auto elif : node->elseIfs) {
            STACK_PUSH_I(ins(ir::If, { }));

            sa::Type* type = elif->condition->Accept(this);

            // Add body {
            STACK_PUSH_I(ins(ir::Scope, { }));
            PushScope();

            sa::Type* res = elif->body->Accept(this);

            if (mError::HasError()) { return t_null; }

            PopScope();
            STACK_POP();
        }
        
        if (node->elseBody) {
            STACK_PUSH_I(ins(ir::Scope, { }));
            PushScope();

            sa::Type* res = node->elseBody->Accept(this);

            if (mError::HasError()) { return t_null; }

            PopScope();
            STACK_POP();
        }

        for (auto elif : node->elseIfs) {
            STACK_POP();
        }
    }

    STACK_POP();
    return t_null;
}

sa::Type* AstVisitor::Visit(WhileAST *node) {

    STACK_PUSH_I(ins(ir::Loop, { }));

    sa::Type* type = node->condition->Accept(this);

    STACK_PUSH_I(ins(ir::Scope, 2, { }));
    PushScope();

    sa::Type* res = node->body->Accept(this);

    if (mError::HasError()) { return t_null; }

    PopScope();
    STACK_POP();

    STACK_POP();

    return res;
}

sa::Type* AstVisitor::Visit(ForAST *node) {
    throw std::runtime_error("ForAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(BreakAST *node) {
    PUSH_INST(ins(ir::Break, { }));
    return t_null;
}

sa::Type* AstVisitor::Visit(ContinueAST *node) {
    PUSH_INST(ins(ir::Continue, { }));
    return t_null;
}

sa::Type* AstVisitor::Visit(ImportAST *node) {
    throw std::runtime_error("ImportAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(ExportAST *node) {
    throw std::runtime_error("ExportAST not implemented");
    return {};
}

sa::Type* AstVisitor::Visit(ClassAST *node) {
    table->SetType(node->name.value, { node->name.value });
    sa::Type* type = table->GetType(node->name.value);

    nameStack.push(node->name.value);

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        VarDeclarationAST* varDecl = dynamic_cast<VarDeclarationAST*>(stmt);

        if (varDecl != nullptr) { // Field
            const std::string fieldName = varDecl->identifier.value;

            type->SetField(fieldName, { varDecl->isMutable, varDecl->type->Accept(this) });

            continue;
        }
        
        FunctionAST* funcDecl = dynamic_cast<FunctionAST*>(stmt);
        
        if (funcDecl != nullptr) { // Method
            const std::string methodName = funcDecl->name.value;

            type->SetMethod(
                methodName, 
                { 
                    "m" + moduleName + type->name + methodName, 
                    table->GetTypeVariant("Function", { 
                        type->name == methodName ? 
                            type :
                            funcDecl->lambda->returnType ? funcDecl->lambda->returnType->Accept(this) : t_null 
                    })
                }
            );

            continue;
        }
        
        mError::AddError("Invalid statement in class '" + node->name.value + "'");
        return t_null;
    }

    for (auto& stmt : node->statements) {
        if (stmt == nullptr) { continue; }

        FunctionAST* funcDecl = dynamic_cast<FunctionAST*>(stmt);
        
        if (funcDecl != nullptr) { // Method
            const std::string methodName = funcDecl->name.value;

            sa::Method* method = type->GetMethod(methodName);

            std::string fname = "f" + moduleName + type->name + methodName;

            PUSH_INST(ins(ir::Decl, fname, { }));
            STACK_PUSH_I(ins(ir::Set, {
                ins(ir::Var, fname, { }),
            }));
            STACK_PUSH_I(ins(ir::IR, { }));
            PushScope();

            int i = 0;

            const std::string vthis = "v" + moduleName + "this";

            table->SetSymbol("this", { 
                false,
                vthis,
                type
            });

            PUSH_INST(ins(ir::Decl, vthis, { }));
            PUSH_INST(ins(ir::Set, {
                ins(ir::Var, vthis, { }),
                ins(ir::Arg, i++, { })
            }));

            for (; i <= funcDecl->lambda->parameters.size(); i++) {
                ArgDeclAST* argDecl = (ArgDeclAST*) funcDecl->lambda->parameters[i - 1];
                const std::string argName = "v" + moduleName + argDecl->identifier.value;
                
                table->SetSymbol(argDecl->identifier.value, { 
                    false,
                    argName, 
                    argDecl->type->Accept(this)
                });

                PUSH_INST(ins(ir::Decl, argName, { }));
                PUSH_INST(ins(ir::Set, {
                    ins(ir::Var, argName, { }),
                    ins(ir::Arg, i, { })
                }));
            }

            sa::Type* retsym = funcDecl->lambda->body->Accept(this);

            if (funcDecl->lambda->returnType) {
                sa::Type* rettype = method->type;

                if (retsym != rettype) {
                    mError::AddError("Function '" + methodName + "' return type mismatch expected '" + rettype->name + "' got '" + retsym->name + "'");
                    return t_null;
                }
            }

            if (type->name == methodName) {
                PUSH_INST(ins(ir::Var, vthis, { }));
            }

            PopScope();
            STACK_POP();
            STACK_POP();

        }
    }

    nameStack.pop();

    return t_null;
}

sa::Type* AstVisitor::Visit(TypeSignatureAST *node) {
    sa::Type* type = table->GetType(node->name.value);

    if (type == nullptr) {
        mError::AddError("Invalid type '" + node->name.value + "'");
        return t_null;
    }
    
    return type;
}

sa::Type* AstVisitor::Visit(TypeAccessAST *node) {
    return node->lhs->Accept(this);
}