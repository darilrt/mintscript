#pragma once

#include "scanner.h"

#include "ast.h"
#include "expr.h"

class Parser {
public:
    Parser(const std::string& source, const std::string& filename="");

    ASTNode *Parse();
    
    // Definitions

    ASTNode* Statement(); // Declaration, If, While, For, Return, Break, Continue, Block, Expression

    ASTNode* Declaration(); // Mut, Let, Function, Class, Enum, Interface

    // Declarations

    ASTNode* Assignment(); // ( MutDeclaration | LetDeclaration | Expression ) = Expression

    ASTNode* MutDeclaration(); // Mut Identifier : Type

    ASTNode* LetDeclaration(); // Let Identifier : Type

    ASTNode* FunctionDeclaration(); // Function Identifier (Parameters) : Type {Statements}

    ASTNode* ClassDeclaration(); // Class Identifier {Statements}

    ASTNode* EnumDeclaration(); // Enum Identifier {Statements}

    ASTNode* InterfaceDeclaration(); // Interface Identifier {Statements}

    // Statements

    ASTNode* IfStatement(); // If (Expression) Statement [Elif (Expression) Statement]* [Else Statement]

    ASTNode* WhileStatement(); // While (Expression) Statement

    ASTNode* ForStatement(); // For (Expression) Statement

    ASTNode* ReturnStatement(); // Return Expression

    ASTNode* BreakStatement(); // Break

    ASTNode* ContinueStatement(); // Continue

    ASTNode* BlockStatement(); // { Statement* }

    // Expression parsing

    ASTNode* Expression(); // Assignment, Conditional

    ASTNode* Conditional(); // ?:

    ASTNode* LogicalOr(); // ||

    ASTNode* LogicalAnd(); // &&

    ASTNode* BitwiseOr(); // |

    ASTNode* BitwiseXor(); // ^

    ASTNode* BitwiseAnd(); // &

    ASTNode* Equality(); // ==, !=

    ASTNode* Comparison(); // <, >, <=, >=

    ASTNode* Shift(); // <<, >>

    ASTNode* Additive(); // +, -

    ASTNode* Multiplicative(); // *, /, %

    ASTNode* Unary(); // !, ~, -, +

    ASTNode* Prefix(); // ++, --

    ASTNode* Postfix(); // ++, --

    ASTNode* Factor(); // Number, String, Bool, Null, Identifier, (Expression), [Expression], {Expression}

    ASTNode* Lambda(); // (Parameters) -> Type {Statements}

    ASTNode* Property(); // .Identifier

private:
    Scanner scanner;
};