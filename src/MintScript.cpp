#include "MintScript.h"
#include "parser.h"
#include "ast.h"
#include "eval.h"
#include "error.h"

#include <iostream>
#include <fstream>

void mInit() {
    // types
    mException::Type->Init();
    mType::Type->Init();
    zFunctionType->Init();
    mint::Type->Init();
    mStr::Type->Init();
    mfloat::Type->Init();
    mbool::Type->Init();
    mnull::Type->Init();
    mlist::Type->Init();
    // zDict::Type->Init();
}

void mShutdown() {
    // Shutdown the MintScript context
}

void mRunFile(const std::string &path) {
    // Read the file
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "Failed to open file: " << path << std::endl;
        return;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Parse the file
    Parser parser(source);

    ASTNode *node = parser.Parse();

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return;
    }

    if (node == nullptr) {
        mError::AddError("Failed to parse file: " + path);
        return;
    }

    // Evaluate the AST

    Eval eval(node);
    eval.Evaluate();
}

void mRunString(const std::string &source) {
    Parser parser(source);

    ASTNode *node = parser.Parse();

    if (mError::HasError()) {
        mError::PrintErrors();
        return;
    }

    if (node == nullptr) { return; }
    
    // Evaluate the AST
    Eval eval(node);
    mlist result = eval.Evaluate();
    
    if (result.items.size() > 0) { 
        std::cout << result[0]->ToString() << std::endl;
    }

    // Cleanup
    delete node;
}

void mRunInteractive() {
    std::cout << "MintScript Interpreter v0.1" << std::endl;
    std::cout << "Type 'exit' to exit" << std::endl;

    std::string input;
    
    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, input);

        if (input == "exit()") {
            break;
        }

        mRunString(input);

        if (mError::HasError()) {
            mError::ClearErrors();
        }
    }
}
