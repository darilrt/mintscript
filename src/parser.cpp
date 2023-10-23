#include "parser.h"
#include "token.h"
#include "error.h"
#include "ast.h"
#include "expr.h"
#include "decl.h"

#include "eval.h"

#include <iostream>

#define EXPECT(t) if (scanner.Peek().type != Token::Type::t) { scanner.Reset(); return nullptr; } scanner.Next();
#define GET(var_name, t) Token var_name = scanner.Peek(); if (var_name.type != Token::Type::t) { scanner.Reset(); return nullptr; } scanner.Next();
#define EXPECTF(var, f) if (!(var = f())) { scanner.Reset(); return nullptr; }
#define IS(t) scanner.Peek().type == Token::Type::t
#define GET_OR_ERROR(var, t, error) Token var = scanner.Peek(); if (var.type != Token::Type::t) { ERROR("SyntaxError: " + error + " " + var.location.ToString()); scanner.Reset(); return nullptr; } scanner.Next();
#define EXPECT_OR_ERROR(t, error) if (scanner.Peek().type != Token::Type::t) { ERROR("SyntaxError: " + error + " " + scanner.Peek().location.ToString()); scanner.Reset(); return nullptr; } scanner.Next();

#define LOG_PEEK() std::cout << "Peek: " << scanner.Peek().ToString() << std::endl;
#define RETURN_ON_ERROR() if (mError::HasError()) { scanner.Reset(); return nullptr; }

Parser::Parser(const std::string &source, const std::string& filename) : scanner(source, filename) { }

ASTNode* Parser::Parse() {
    ASTNode *node = nullptr;
    std::vector<ASTNode*> nodes;

    scanner.PushAndSetIgnoreNewLine(true);
    while (scanner.Peek().type != Token::Type::EndFile) {
        node = Statement();
        if (node) { nodes.push_back(node); }

        scanner.SkipNewLine();
        
        if (mError::HasError()) { break; }
    }
    scanner.PopIgnoreNewLine();
    
    return new ProgramAST(nodes);
}

// Statement: NewLine | Declaration | IfStatement | WhileStatement | ForStatement | ReturnStatement | BreakStatement | ContinueStatement | Block | Expression
ASTNode *Parser::Statement() {
    ASTNode *node = nullptr;

    scanner.SkipNewLine();

    if (
        (node = Declaration()) ||
        (node = IfStatement()) ||
        (node = WhileStatement()) ||
        (node = ForStatement()) ||
        (node = ReturnStatement()) ||
        (node = BreakStatement()) ||
        (node = ContinueStatement()) ||
        (node = ImportStatement()) ||
        (node = ExportStatement()) ||
        (node = Block())
    ) { return node; }
    
    if (mError::HasError()) { return nullptr; }

    if (node = Expression()) {
        scanner.Consume();
        return node;
    }

    ERROR("Syntax Error: Unexpected " + scanner.Peek().ToString());
    
    scanner.Reset();
    return node;
}

// Declaration: MutDeclaration | LetDeclaration | FunctionDeclaration | ClassDeclaration | EnumDeclaration | InterfaceDeclaration
ASTNode* Parser::Declaration() {
    ASTNode *node = nullptr;

    if ((node = VarDeclaration()) ||
        (node = FunctionDeclaration()) ||
        (node = ClassDeclaration()) ||
        (node = Assignment())) {
        scanner.Consume();
        return node;
    }
    
    scanner.Reset();
    return node;
}

// Assignment: Expression '=' Expression
ASTNode *Parser::Assignment() {
    ASTNode *node = nullptr;

    EXPECTF(node, Expression);

    if (IS(Equal)) {
        const Token &op = scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Expression);

        node = new AssignmentAST(op, node, right);
    }

    scanner.Consume();
    return node;
}

// MutDeclaration: (Mut || Let) Identifier : Type = Expression
ASTNode *Parser::VarDeclaration() {
    ASTNode *node = nullptr;

    if (!(IS(Mut)) && !(IS(Let))) { return 0; }

    Token mut = scanner.Peek(); 
    scanner.Next();

    if (!(IS(Identifier))) {
        ERROR("SyntaxError: Expected identifier after '" + mut.value + "' " + mut.location.ToString());
        scanner.Reset();
        return 0;
    }

    Token name = scanner.Peek();
    scanner.Next();

    if (!(IS(Colon))) {
        ERROR("SyntaxError: Expected ':' after identifier '" + name.value + "' " + name.location.ToString());
        scanner.Reset();
        return 0;
    }

    scanner.Next();

    ASTNode *type = Type();

    if (type == nullptr) {
        ERROR("SyntaxError: Expected type after ':' " + scanner.Peek().location.ToString());
        scanner.Reset();
        return 0;
    }

    ASTNode *expression = nullptr;

    if (IS(Equal)) {
        scanner.Next();

        expression = Expression();

        if (expression == nullptr) {
            ERROR("SyntaxError: Expected expression after '=' " + scanner.Peek().location.ToString());
            scanner.Reset();
            return 0;
        }
    }

    node = new VarDeclarationAST(mut.type == Token::Type::Mut, name, type, expression);
    
    scanner.Consume();
    return node;
}

// FunctionDeclaration: Identifier Lambda
ASTNode *Parser::FunctionDeclaration() {
    ASTNode *node = nullptr;

    GET(name, Identifier);

    if (!(IS(LParen))) {
        scanner.Reset();
        return 0; 
    }
    
    node = Lambda();

    if (node == nullptr) { 
        scanner.Reset();
        return 0;
    }

    node = new FunctionAST(name, (LambdaAST*) node);
    
    scanner.Consume();
    return node;
}

// ClassDeclaration: Class Identifier "(" (Expression ("," Expression)*)? ")" "{" (VarDeclaration | FunctionDeclaration)* "}"
ASTNode *Parser::ClassDeclaration() {
    ASTNode *node = nullptr;

    EXPECT(Class);

    GET_OR_ERROR(name, Identifier, "Expected identifier after 'class'");

    std::vector<ASTNode*> bases;

    if (IS(LParen)) {
        scanner.Next();

        ASTNode *parent = nullptr;

        EXPECTF(parent, Expression);

        bases.push_back(parent);

        while (IS(Comma)) {
            scanner.Next();

            EXPECTF(parent, Expression);

            bases.push_back(parent);
        }

        if (!(IS(RParen))) {
            ERROR("SyntaxError: Expected ')' after class declaration " + scanner.Peek().location.ToString());
            scanner.Reset();
            return 0;
        }
        scanner.Next();
    }

    EXPECT_OR_ERROR(LBrace, "Expected '{' after identifier '" + name.value + "'");

    std::vector<ASTNode*> statements;

    scanner.PushAndSetIgnoreNewLine(true);

    while (!(IS(RBrace))) {
        ASTNode* statement = nullptr;
        
        if ((statement = VarDeclaration()) ||
            (statement = FunctionDeclaration())) {
            statements.push_back(statement);
        }
        else {
            ERROR("SyntaxError: Expected statement in class body " + scanner.Peek().location.ToString());
            scanner.Reset();

            for (ASTNode *node : statements) {
                delete node;
            }

            return 0;
        }

        if (mError::HasError()) { 
            scanner.Reset();

            for (ASTNode *node : statements) {
                delete node;
            }

            return 0;
        }

        scanner.SkipNewLine();
    }

    scanner.PopIgnoreNewLine();

    if (!(IS(RBrace))) {
        ERROR("SyntaxError: Expected '}' " + scanner.Peek().location.ToString());
        scanner.Reset();
        return 0;
    }

    scanner.Next();

    node = new ClassAST(name, statements, bases);

    scanner.Consume();
    return node;
}

// ExportList: ExportItem (',' ExportItem)*
std::vector<ASTNode *> Parser::ExportList() {
    std::vector<ASTNode *> exports;

    ASTNode *node = nullptr;
    if (node = ExportItem()) {
        exports.push_back(node);

        while (IS(Comma)) {
            scanner.Next();

            ASTNode *exportItem = nullptr;
            if (!(exportItem = ExportItem())) {
                ERROR("SyntaxError: Expected item after ',' " + scanner.Peek().location.ToString());
                scanner.Reset();

                for (ASTNode *node : exports) {
                    delete node;
                }

                exports.clear();
                return exports;
            }

            exports.push_back(exportItem);
        }
    }

    scanner.Consume();
    return exports;
}

// ExportItem: Identifier | Expression 'as' Identifier
ASTNode *Parser::ExportItem() {
    ASTNode *node = nullptr;

    if (IS(Identifier)) {
        Token name = scanner.Peek();
        scanner.Next();

        node = new ExportItemAST(name);
    }
    else {
        EXPECTF(node, Expression);

        if (!(IS(As))) {
            ERROR("SyntaxError: Expected 'as' after expression " + scanner.Peek().location.ToString());
            scanner.Reset();
            return nullptr;
        }

        scanner.Next();

        if (!(IS(Identifier))) {
            ERROR("SyntaxError: Expected identifier after 'as' " + scanner.Peek().location.ToString());
            scanner.Reset();
            return nullptr;
        }

        GET(name, Identifier);
        node = new ExportItemAST(name, node);
    }

    scanner.Consume();
    return node;
}

// ImportList: ImportItem (',' ImportItem)*
std::vector<ASTNode *> Parser::ImportList() {
    std::vector<ASTNode *> imports;

    ASTNode *node = nullptr;
    if (node = ImportItem()) {
        imports.push_back(node);

        while (IS(Comma)) {
            scanner.Next();

            ASTNode *importItem = nullptr;
            if (!(importItem = ImportItem())) {
                ERROR("SyntaxError: Expected item after ',' " + scanner.Peek().location.ToString());
                scanner.Reset();

                for (ASTNode *node : imports) {
                    delete node;
                }

                imports.clear();
                return imports;
            }

            imports.push_back(importItem);
        }
    }

    scanner.Consume();
    return imports;
}

// ImportItem: Identifier | Expression 'as' Identifier
ASTNode* Parser::ImportItem() {
    ASTNode *node = nullptr;

    if (IS(Identifier)) {
        Token name = scanner.Peek();
        scanner.Next();

        if (IS(As)) {
            scanner.Next();

            if (!(IS(Identifier))) {
                ERROR("SyntaxError: Expected identifier after 'as' " + scanner.Peek().location.ToString());
                scanner.Reset();
                return nullptr;
            }

            GET(asName, Identifier);
            node = new ImportItemAST(name, asName);
        }
        else {
            node = new ImportItemAST(name);
        }
    }
    else {
        ERROR("SyntaxError: Expected identifier " + scanner.Peek().location.ToString());
        scanner.Reset();
        return nullptr;
    }

    scanner.Consume();
    return node;
}

// ExprList: (Expression (',' Expression)*)
std::vector<ASTNode*> Parser::ExprList() {
    std::vector<ASTNode*> exprs;

    ASTNode *node = nullptr;
    if (node = Expression()) {
        exprs.push_back(node);

        while (IS(Comma)) {
            scanner.Next();

            ASTNode *expr = nullptr;
            if (!(expr = Expression())) {
                ERROR("SyntaxError: Expected expression after ',' " + scanner.Peek().location.ToString());
                scanner.Reset();

                for (ASTNode *node : exprs) {
                    delete node;
                }

                exprs.clear();
                return exprs;
            }

            exprs.push_back(expr);
        }
    }

    scanner.Consume();
    return exprs;
}

// ArgDeclList: (ArgDecl (',' ArgDecl)*)
std::vector<ASTNode*> Parser::ArgDeclList() {
    std::vector<ASTNode *> args;

    ASTNode *node = nullptr;

    if (node = ArgDecl(false)) {
        args.push_back(node);

        while (IS(Comma)) {
            scanner.Next();

            ASTNode *arg = nullptr;
            if (!(arg = ArgDecl())) {
                ERROR("SyntaxError: Expected argument after ',' " + scanner.Peek().location.ToString());
                scanner.Reset();

                for (ASTNode *node : args) {
                    delete node;
                }

                args.clear();

                args.push_back(nullptr);
                return args;
            }

            args.push_back(arg);
        }
        
        scanner.Consume();
    }
    
    return args;
}

// ArgDecl: Identifier ':' Type ('=' Expression)?
ASTNode *Parser::ArgDecl(bool strict) {
    ASTNode *node = nullptr;

    Token name = scanner.Peek();
    if (!(IS(Identifier))) {
        return 0;
    }
    scanner.Next();

    if (!(IS(Colon))) {
        if (strict) ERROR("SyntaxError: Expected ':' after identifier '" + name.value + "' " + name.location.ToString());
        scanner.Reset();
        return 0;
    }
    scanner.Next();

    ASTNode *type = Expression();

    if (type == nullptr) {
        ERROR("SyntaxError: Expected type after ':' " + scanner.Peek().location.ToString());
        scanner.Reset();
        return 0;
    }

    ASTNode *expression = nullptr;

    if (IS(Equal)) {
        scanner.Next();

        expression = Expression();

        if (expression == nullptr) {
            ERROR("SyntaxError: Expected expression after '=' " + scanner.Peek().location.ToString());
            scanner.Reset();
            return 0;
        }
    }

    node = new ArgDeclAST(true, name, type, expression);

    scanner.Consume();
    return node;
}

// Block: '{' ((Statement NewLine)* Statement)? '}'
ASTNode *Parser::Block() {
    ASTNode* node = nullptr;

    EXPECT(LBrace);

    std::vector<ASTNode*> statements;
    
    scanner.PushAndSetIgnoreNewLine(true);

    while (!(IS(RBrace))) {
        ASTNode* statement = nullptr;

        if (statement = Statement()) {
            statements.push_back(statement);
        }

        if (mError::HasError()) { 
            scanner.Reset();

            for (ASTNode *node : statements) {
                delete node;
            }

            return 0;
        }

        scanner.SkipNewLine();
    }

    scanner.PopIgnoreNewLine();

    if (!(IS(RBrace))) {
        ERROR("SyntaxError: Expected '}' " + scanner.Peek().location.ToString());
        scanner.Reset();
        return 0;
    }

    scanner.Next();
    scanner.Consume();
    return new BlockAST(statements);
}

// Type: TypeSignature ( '.' TypeSignature )*
ASTNode *Parser::Type(bool strict) {
    ASTNode *node = nullptr;
    TypeSignatureAST *lhs = nullptr;

    if (!(lhs = TypeSignature(strict))) {
        if (strict) { scanner.Reset(); }
        return nullptr;
    }

    node = lhs;

    while (IS(Dot)) {
        scanner.Next();

        TypeSignatureAST *rhs = nullptr;
        EXPECTF(rhs, TypeSignature);

        node = new TypeAccessAST(lhs, rhs);
    }

    scanner.Consume();
    return node;
}

// TypeSignature: Identifier ( '[' Type ( ',' Type )* ']' )?
TypeSignatureAST *Parser::TypeSignature(bool strict) {
    Token name;

    if (strict) {
        GET_OR_ERROR(_name, Identifier, "Expected identifier");
        name = _name;
    }
    else {
        name = scanner.Peek();

        if (!(IS(Identifier))) {
            return nullptr;
        }

        scanner.Next();
    }

    std::vector<ASTNode *> types;

    if (IS(LBracket)) {
        scanner.Next();
        
        ASTNode *type = nullptr;
        EXPECTF(type, Type);

        types.push_back(type);

        while (IS(Comma)) {
            scanner.Next();

            ASTNode *type = nullptr;
            EXPECTF(type, Type);

            types.push_back(type);
        }
        
        if (!(IS(RBracket))) {
            ERROR("SyntaxError: Expected ']' " + scanner.Peek().location.ToString());
            scanner.Reset();
            return nullptr;
        }

        scanner.Next();

    }

    return new TypeSignatureAST(name, types);
}

// Expression: Conditional
ASTNode* Parser::Expression() {
    ASTNode *expr = nullptr;
    
    scanner.PushAndSetIgnoreNewLine(true);

    if (expr = Conditional()) {
        scanner.PopIgnoreNewLine();
        return expr;
    }

    scanner.PopIgnoreNewLine();

    return expr;
}

// Conditional: LogicalOr ('?' Expression ':' Expression)?
ASTNode *Parser::Conditional() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, LogicalOr);

    if (IS(Question)) {
        scanner.Next();

        ASTNode *trueExpr = nullptr;
        EXPECTF(trueExpr, Expression);

        EXPECT(Colon);

        ASTNode *falseExpr = nullptr;
        EXPECTF(falseExpr, Expression);

        expr = new TernaryExprAST(expr, trueExpr, falseExpr);
    }

    scanner.Consume();
    return expr;
}

// LogicalOr: LogicalAnd ('||' LogicalAnd)*
ASTNode *Parser::LogicalOr() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, LogicalAnd);

    while (IS(PipePipe)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, LogicalAnd);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// LogicalAnd: BitwiseOr ('&&' BitwiseOr)*
ASTNode *Parser::LogicalAnd() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, BitwiseOr);

    while (IS(AmpAmp)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, BitwiseOr);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// BitwiseOr: BitwiseXor ('|' BitwiseXor)*
ASTNode *Parser::BitwiseOr() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, BitwiseXor);

    while (IS(Pipe)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, BitwiseXor);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// BitwiseXor: BitwiseAnd ('^' BitwiseAnd)*
ASTNode *Parser::BitwiseXor() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, BitwiseAnd);

    while (IS(Caret)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, BitwiseAnd);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// BitwiseAnd: Equality ('&' Equality)*
ASTNode *Parser::BitwiseAnd() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Equality);

    while (IS(Amp)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Equality);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Equality: Comparison (('==' | '!=') Comparison)*
ASTNode *Parser::Equality() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Comparison);

    while (IS(EqualEqual) || IS(NotEqual)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Comparison);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Comparison: Shift (( '<' | '>' | '<=' | '>=' ) Shift)*
ASTNode *Parser::Comparison() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Shift);

    while (IS(Less) || IS(Greater) || IS(LessEqual) || IS(GreaterEqual)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Shift);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Shift: Additive (('<<' | '>>') Additive)*
ASTNode *Parser::Shift() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Additive);

    while (IS(LessLess) || IS(GreaterGreater)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Additive);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Additive: Multiplicative (('+' | '-') Multiplicative)*
ASTNode *Parser::Additive() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Multiplicative);

    while (IS(Plus) || IS(Minus)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = Multiplicative();

        if (right == nullptr) {
            ERROR("SyntaxError: Expected expression after operator '" + token.value + "' " + token.location.ToString());
            scanner.Reset();
            return nullptr;
        }

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Multiplicative: Unary (('*' | '/' | '%') Unary)*
ASTNode *Parser::Multiplicative() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Unary);

    while (IS(Star) || IS(Slash) || IS(Percent)) {
        Token token = scanner.Peek();
        scanner.Next();

        ASTNode *right = nullptr;
        EXPECTF(right, Unary);

        expr = new BinaryExprAST(token, expr, right);
    }

    scanner.Consume();
    return expr;
}

// Unary: Prefix | Postfix
ASTNode *Parser::Unary() {
    ASTNode *expr = nullptr;

    (expr = Prefix()) ||
    (expr = Postfix());

    scanner.Consume();
    return expr;
}

// Prefix: ('+' | '-' | '!' | '~' | '++' | '--') Unary | Postfix
ASTNode *Parser::Prefix() {
    ASTNode *expr = nullptr;

    if (IS(Plus) || IS(Minus) || IS(Not) || IS(Tilde) || IS(PlusPlus) || IS(MinusMinus)) {
        Token token = scanner.Peek();
        scanner.Next();
        scanner.Consume();

        EXPECTF(expr, Unary);

        expr = new UnaryExprAST(token, expr);
    }
    else {
        EXPECTF(expr, Postfix);
    }

    scanner.Consume();
    return expr;
}

// Postfix: Access ('++' | '--' )
ASTNode *Parser::Postfix() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Access);

    if (IS(PlusPlus) || IS(MinusMinus)) {
        Token token = scanner.Peek();
        scanner.Next();
        scanner.Consume();

        expr = new UnaryExprAST(token, expr, false);
    }
    
    scanner.Consume();
    return expr;
}

// Access: Factor ( '.' IDENTIFIER | '[' Expression ']' | '(' ExprList ')' )*
ASTNode *Parser::Access() {
    ASTNode *node = nullptr;

    EXPECTF(node, Factor);
    
    while (true) {
        if (IS(Dot)) {
            scanner.Next();
            
            if (!(IS(Identifier))) {
                ERROR("SyntaxError: Expected identifier after '.'");
                scanner.Reset();
                return 0;
            }

            Token token = scanner.Peek();
            scanner.Next();

            node = new AccessExprAST(node, token);
        }
        else if (IS(LBracket)) {
            scanner.Next();

            ASTNode *index = Expression();

            if (index == nullptr) {
                ERROR("SyntaxError: Expected expression after '['");
                scanner.Reset();
                return 0;
            }
            
            if (!(IS(RBracket))) {
                ERROR("SyntaxError: Expected ']'");
                scanner.Reset();
                return 0;
            }

            scanner.Next();
            
            node = new IndexExprAST(node, index);
        }
        else if (IS(LParen)) {
            scanner.Next();

            if (IS(RParen)) {
                scanner.Next();
                node = new CallExprAST(node);
                continue;
            }

            std::vector<ASTNode*> args = ExprList();

            if (!(IS(RParen))) {
                ERROR("SyntaxError: Expected ')'");
                scanner.Reset();
                return 0;
            }

            scanner.Next();

            node = new CallExprAST(node, args);
        }
        else {
            break;
        }
    }

    scanner.Consume();
    return node;
}

// Factor: INT | FLOAT | STRING | true | false | null | LambdaDecl | '(' Expression ')'
ASTNode* Parser::Factor() {
    ASTNode *expr = nullptr;

    scanner.Consume();

    if (IS(Int)) {
        GET(token, Int);
        expr = new NumberExprAST(std::stoi(token.value));
    }
    else if (IS(Float)) {
        GET(token, Float);
        expr = new NumberExprAST(50.1f);
    }
    else if (IS(String)) {
        GET(token, String);
        expr = new StringExprAST(token.value);
    }
    else if (IS(True)) {
        scanner.Next();
        expr = new BoolExprAST(true);
    }
    else if (IS(False)) {
        scanner.Next();
        expr = new BoolExprAST(false);
    }
    else if (IS(Null)) {
        scanner.Next();
        expr = new NullExprAST();
    }
    else if (IS(LParen)) {
        scanner.Consume();

        if (expr = Lambda()) { return expr; }

        if (mError::HasError()) {
            scanner.Reset();
            return nullptr;
        }
        
        scanner.Next();
        
        expr = Expression();

        if (!(IS(RParen))) {
            std::cout << "Expected ')'" << std::endl;
            scanner.Reset();
            return 0;
        }

        expr = new ParenExprAST(expr);
        scanner.Next();
        scanner.Consume();
    }
    else if (expr = Array()) { }
    else if (expr = Property()) { }
    else { 
        scanner.Reset(); 
        return nullptr;
    }
    
    scanner.Consume();
    return expr;
}

// LambdaDecl: "(" ArgDeclList ")" "->" Type Block
ASTNode *Parser::Lambda() {
    ASTNode* node = nullptr;
    std::vector<ASTNode*> params;
    ASTNode* type = nullptr;
    ASTNode* body = nullptr;

    EXPECT(LParen);

    params = ArgDeclList();
    
    if (params.size() == 0) { }
    else if (params.size() == 1 && params[0] == nullptr) {
        scanner.Reset();
        return nullptr;
    }

    if (!(IS(RParen))) {
        if (params.size() != 0) {
            ERROR("SyntaxError: Expected ')' or parameter");
            scanner.Reset();
            return nullptr;
        }
        scanner.Reset();
        return nullptr;
    }
    
    scanner.Next();
    
    if (IS(Arrow)) {
        scanner.Next();

        type = Expression();

        if (type == nullptr) {
            ERROR("SyntaxError: Expected expression after '->'");
            scanner.Reset();
            return nullptr;
        }

        if (!(IS(LBrace))) {
            ERROR("SyntaxError: Expected '{'");
            scanner.Reset();
            return nullptr;
        }
    }

    if (!(IS(LBrace)) && params.size() == 0) {
        return nullptr;
    }

    body = Block();

    node = new LambdaAST(params, type, body);
    
    scanner.Consume();
    return node;
}

// Property: IDENTIFIER
ASTNode *Parser::Property() {
    PropertyExprAST *node = nullptr;
    
    if (IS(Identifier)) {
        GET(name, Identifier);
        node = new PropertyExprAST(name.value);
    }
    
    scanner.Consume();
    return (ASTNode*) node;
}

// Array: "[" ExprList "]"
ASTNode *Parser::Array() {
    EXPECT(LBracket);

    ArrayExprAST* node = new ArrayExprAST();

    node->values = ExprList();

    if (!(IS(RBracket))) {
        ERROR("SyntaxError: Expected ']'");
        scanner.Reset();
        return nullptr;
    }
    scanner.Next();

    scanner.Consume();
    return node;
}

// IfStatement: "if" Expression Block ("elif" Expression Block)* ("else" Block)?
ASTNode *Parser::IfStatement() {
    IfAST *node = new IfAST();

    EXPECT(If);

    node->condition = Expression();

    if (node->condition == nullptr) {
        ERROR("SyntaxError: Expected expression after 'if'");
        scanner.Reset();
        return nullptr;
    }

    node->body = Block();
    
    if (node->body == nullptr) {
        ERROR("SyntaxError: Expected block after 'if' expression");
        scanner.Reset();
        return nullptr;
    }

    while (IS(Elif)) {
        scanner.Next();

        IfAST* elif = new IfAST();
        elif->condition = Expression();

        if (elif->condition == nullptr) {
            ERROR("SyntaxError: Expected expression after 'elif'");
            scanner.Reset();
            return nullptr;
        }

        elif->body = Block();

        if (elif->body == nullptr) {
            ERROR("SyntaxError: Expected block after 'elif' expression");
            scanner.Reset();
            return nullptr;
        }
        
        node->elseIfs.push_back(elif);
    }

    if (IS(Else)) {
        scanner.Next();

        ASTNode* elseBody = Block();

        if (elseBody == nullptr) {
            ERROR("SyntaxError: Expected block after 'else'");
            scanner.Reset();
            return nullptr;
        }

        node->elseBody = elseBody;
    }
    
    scanner.Consume();
    return (ASTNode*) node;
}

// WhileStatament: "While" Expression Block
ASTNode *Parser::WhileStatement() {
    WhileAST* node = new WhileAST();

    EXPECT(While);

    node->condition = Expression();

    if (node->condition == nullptr) {
        ERROR("SyntaxError: Expected expression after 'while'");
        scanner.Reset();
        return nullptr;
    }

    node->body = Block();

    if (node->body == nullptr) {
        ERROR("SyntaxError: Expected block after 'while' expression");
        scanner.Reset();
        return nullptr;
    }

    scanner.Consume();
    return (ASTNode*) node;
}

// ForStataement: "for" (Mut || Let)? Identifier (":" Type) "in" Expression Block
ASTNode *Parser::ForStatement() {
    EXPECT(For);

    VarDeclarationAST* var = new VarDeclarationAST(false, Token(), nullptr, new NullExprAST());
    
    if (IS(Mut)) { var->isMutable = true; scanner.Next(); }
    else if (IS(Let)) { var->isMutable = false; scanner.Next(); }
    
    if (!(IS(Identifier))) {
        ERROR("SyntaxError: Expected identifier after 'for'");
        scanner.Reset();
        return nullptr;
    }

    GET(name, Identifier);
    var->identifier = name;

    if (IS(Colon)) {
        scanner.Next();

        ASTNode* type = Expression();
        RETURN_ON_ERROR();

        if (type == nullptr) {
            ERROR("SyntaxError: Expected type after ':'");
            scanner.Reset();
            return nullptr;
        }

        var->type = type;
    }

    if (!(IS(In))) {
        ERROR("SyntaxError: Expected 'in' after identifier");
        scanner.Reset();
        return nullptr;
    }
    scanner.Next();

    ASTNode* expr = Expression();
    RETURN_ON_ERROR();

    if (expr == nullptr) {
        ERROR("SyntaxError: Expected expression after 'in'");
        scanner.Reset();
        return nullptr;
    }

    ASTNode* body = Block();

    RETURN_ON_ERROR();

    if (body == nullptr) {
        ERROR("SyntaxError: Expected block after 'for' expression");
        scanner.Reset();
        return nullptr;
    }

    return new ForAST(var, expr, body);
}

// ReturnStatement: "ret" Expression?
ASTNode *Parser::ReturnStatement() {
    ASTNode *expr = nullptr;

    EXPECT(Return);

    if (expr = Expression()) {
        scanner.Consume();
        return new ReturnAST(expr);
    }

    return new ReturnAST(nullptr);
}

// BreakStatement: "break"
ASTNode *Parser::BreakStatement() {
    EXPECT(Break);
    scanner.Consume();
    return new BreakAST();
}

// ContinueStatement: "continue"
ASTNode *Parser::ContinueStatement() {
    EXPECT(Continue);
    scanner.Consume();
    return new ContinueAST();
}

// ImportStatement: "import" String | "import" Identifier ('.' Identifier)* ('{' ExportList '}')? (as Identifier)?
ASTNode *Parser::ImportStatement() {
    EXPECT(Import);

    ImportAST* node = new ImportAST();

    if (IS(String)) {
        Token path = scanner.Peek();
        scanner.Next();

        node->path = path;
        node->isPath = true;
    }
    else {
        std::vector<Token> identifiers;

        Token name = scanner.Peek();

        if (!(IS(Identifier))) {
            ERROR("SyntaxError: Expected identifier after 'import'");
            scanner.Reset();
            return nullptr;
        }
        scanner.Next();

        identifiers.push_back(name);

        while (IS(Dot)) {
            scanner.Next();

            if (!(IS(Identifier))) {
                ERROR("SyntaxError: Expected identifier after '.'");
                scanner.Reset();
                return nullptr;
            }

            GET(name, Identifier);
            identifiers.push_back(name);
        }

        node->identifiers = identifiers;

        if (IS(LBrace)) {
            scanner.Next();

            node->imports = ImportList();

            if (!(IS(RBrace))) {
                ERROR("SyntaxError: Expected '}'");
                scanner.Reset();
                return nullptr;
            }
        }
        else if (IS(As)) {
            scanner.Next();

            if (!(IS(Identifier))) {
                ERROR("SyntaxError: Expected identifier after 'as'");
                scanner.Reset();
                return nullptr;
            }

            GET(name, Identifier);
            node->alias = name;
        }
    }

    scanner.Consume();
    return node;
}

// ExportStatement: "export" '{' Identifier (',' Identifier)* '}'
ASTNode *Parser::ExportStatement() {
    EXPECT(Export);

    if (!(IS(LBrace))) {
        ERROR("SyntaxError: Expected '{'");
        scanner.Reset();
        return nullptr;
    }
    scanner.Next();

    std::vector<ASTNode*> list = ExportList();

    if (!(IS(RBrace))) {
        ERROR("SyntaxError: Expected '}'");
        scanner.Reset();
        return nullptr;
    }

    scanner.Next();

    return new ExportAST(list);
}
