#include "parser.h"
#include "token.h"

#include "ast.h"
#include "expr.h"
#include "decl.h"

#include "eval.h"

#include <iostream>

#define EXPECT(t) if (scanner.Peek().type != Token::Type::t) { scanner.Reset(); return 0; } scanner.Next();
#define GET(var_name, t) Token var_name = scanner.Peek(); if (var_name.type != Token::Type::t) { scanner.Reset(); return 0; } scanner.Next();
#define EXPECTF(var, f) if (!(var = f())) { scanner.Reset(); return 0; }
#define IS(t) scanner.Peek().type == Token::Type::t

#define LOG_PEEK() std::cout << "Peek: " << scanner.Peek().ToString() << std::endl;

Parser::Parser(const std::string &source) : scanner(source) { }

ASTNode *Parser::Parse() {
    ASTNode *node = nullptr;

    scanner.PushAndSetIgnoreNewLine(false);
    while (scanner.Peek().type != Token::Type::EndFile) {
        node = Statement();
    }
    scanner.PopIgnoreNewLine();

    return node;
}

// Statement: NewLine | Declaration | IfStatement | WhileStatement | ForStatement | ReturnStatement | BreakStatement | ContinueStatement | Block | Expression
ASTNode *Parser::Statement() {
    ASTNode *node = nullptr;

    while (IS(NewLine)) {
        scanner.Next();
    }

    // if (
        // (node = Declaration()) // ||
        // (node = IfStatement()) ||
        // (node = WhileStatement()) ||
        // (node = ForStatement()) ||
        // (node = ReturnStatement()) ||
        // (node = BreakStatement()) ||
        // (node = ContinueStatement()) ||
        // (node = BlockStatement()) ||
        // (node = Expression())
    // ) { return true; }
    
    if (node = Expression()) {
        if (!(IS(NewLine) || IS(EndFile))) {
            std::cout << "Syntax Error: Unexpected " << scanner.Peek().ToString() << std::endl;
            return 0;
        }
    }

    return node;
}

// Declaration: MutDeclaration | LetDeclaration | FunctionDeclaration | ClassDeclaration | EnumDeclaration | InterfaceDeclaration
ASTNode* Parser::Declaration() {
    ASTNode *node = nullptr;

    node = Assignment();
    
    // (node = MutDeclaration()) ||
    // (node = LetDeclaration()) ||
    // (node = FunctionDeclaration()) ||
    // (node = ClassDeclaration()) ||
    // (node = EnumDeclaration()) ||
    // (node = InterfaceDeclaration());

    return node;
}

// Assignment: ( MutDeclaration | LetDeclaration | Expression ) '=' Expression
ASTNode *Parser::Assignment() {
    AssignmentAST *node = nullptr;

    if (IS(Mut) || IS(Let)) {
    } else {
        ASTNode *expr = nullptr;
        EXPECTF(expr, Expression);

        if (IS(Equal)) {
            scanner.Next();

            ASTNode *right = nullptr;
            EXPECTF(right, Expression);

            node = new AssignmentAST(Token::Type::Equal, expr, right);
        } else {
            return expr;
        }
    }

    scanner.Consume();
    return node;
}

// Expression: Factor
ASTNode* Parser::Expression() {
    ASTNode *expr = nullptr;
    
    scanner.PushAndSetIgnoreNewLine(true);
    EXPECTF(expr, Conditional);
    scanner.PopIgnoreNewLine();

    scanner.Consume();
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

        ASTNode *right = nullptr;
        EXPECTF(right, Multiplicative);

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

// Postfix: Factor ('++' | '--' | (('[' Expression ']')* | ('(' Expression ')')*)* )
ASTNode *Parser::Postfix() {
    ASTNode *expr = nullptr;

    EXPECTF(expr, Factor);

    if (IS(PlusPlus) || IS(MinusMinus)) {
        Token token = scanner.Peek();
        scanner.Next();
        scanner.Consume();

        expr = new UnaryExprAST(token, expr, false);
    }
    else {
        while (IS(LBracket) || IS(LParen)) {
            while (IS(LBracket)) {
                scanner.Next();

                ASTNode *index = Expression();

                if (!(IS(RBracket))) {
                    std::cout << "Expected ']'" << std::endl;
                    scanner.Reset();
                    return 0;
                }

                expr = new IndexExprAST(expr, index);
                scanner.Next();
            }

            while (IS(LParen)) {
                scanner.Next();

                // TODO: Get arguments

                if (!(IS(RParen))) {
                    std::cout << "Expected ')'" << std::endl;
                    scanner.Reset();
                    return 0;
                }

                expr = new CallExprAST(expr);
                scanner.Next();
            }
        }
    }
    
    scanner.Consume();
    return expr;
}

// Factor: INT | FLOAT | STRING | true | false | null | LambdaDecl | '(' Expression ')'
ASTNode* Parser::Factor() {
    ASTNode *expr = nullptr;

    if (IS(Int)) {
        GET(token, Int);
        expr = new NumberExprAST(std::stoi(token.value));
    }
    else if (IS(Float)) {
        GET(token, Float);
        expr = new NumberExprAST(std::stod(token.value));
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
    else if (expr = Lambda()) { }
    else if (IS(LParen)) {
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
    else if (expr = Property()) { }

    scanner.Consume();
    return expr;
}

// LambdaDecl: "(" ")" "->" Type "{" statement* "}"
ASTNode *Parser::Lambda() {
    return 0;
}

// Property: IDENTIFIER? ("." IDENTIFIER)*
ASTNode *Parser::Property() {
    PropertyExprAST *base = nullptr;
    PropertyExprAST *expr = nullptr;
    
    scanner.PushAndSetIgnoreNewLine(true);

    if (IS(Identifier)) {
        GET(name, Identifier);
        expr = new PropertyExprAST(name.value);

        base = expr;
    }
    
    while (IS(Dot)) {
        scanner.Next();
        
        GET(name, Identifier);

        if (expr) {
            expr->next = new PropertyExprAST(name.value);
            expr = (PropertyExprAST*) expr->next;
        }
        else {
            expr = new PropertyExprAST("." + name.value);
            base = expr;
        }
    }

    scanner.PopIgnoreNewLine();

    scanner.Consume();
    return (ASTNode*) base;
}
