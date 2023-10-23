#include "MintScript.h"
#include "parser.h"
#include "ast.h"
#include "error.h"
#include "globals.h"
#include "ast_visitor.h"

#include <iostream>
#include <fstream>
#include <filesystem>

void mInit() {
}

void mShutdown() {
    // Shutdown the MintScript context
}

void mRunFile(const std::string &path) {
    std::filesystem::path filePath(path);
    MAIN_FILE_PATH = std::filesystem::absolute(filePath).parent_path();

    std::ifstream file(filePath);

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

    std::cout << "DEBUG: Parsed file: " << path << std::endl; // DEBUG

    // Evaluate the AST
    AstVisitor* ast_visitor = AstVisitor::Eval(node);

    ir::Interpreter interpreter;
    
    interpreter.Interpret(ast_visitor->stack.top());

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return;
    }
    
    delete node;
}

void mRunString(const std::string &source) {
}

void mRunInteractive() {
    std::cout << "MintScript v" << MINT_VERSION << std::endl;
    std::cout << "Type 'exit()' to exit" << std::endl;

    std::string input;
    
    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, input);

        while (input.size() > 0 && input[input.size() - 1] == '\\') {
            input.pop_back();
            input += '\n';
            std::string nextLine;
            std::cout << ".. ";
            std::getline(std::cin, nextLine);
            input += nextLine;
        }

        if (input == "exit()") {
            break;
        }

        mRunString(input);

        if (mError::HasError()) {
            mError::PrintErrors();
            mError::ClearErrors();
        }
    }
}