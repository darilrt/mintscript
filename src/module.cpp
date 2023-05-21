#include <fstream>

#include "module.h"
#include "error.h"
#include "parser.h"
#include "ast.h"
#include "debug_visitor.h"
#include "globals.h"

mType* mModule::Type = new mType(
    "Module",
    []() {
    },
    []() -> mObject* {
        return new mModule();
    }
);

mModule::mModule() : mObject(mModule::Type) { }

mObject *mModule::ImportFile(const std::string &path) {
    std::filesystem::path filePath(MAIN_FILE_PATH / path);

    // Check if the file was already imported
    if (mSymbolTable::GetScope(filePath.string()) != nullptr) {
        // TODO: Return the module
        return nullptr;
    }

    // Read the file
    std::ifstream file(filePath);

    if (!file.is_open()) {
        mError::AddError("Failed to open file: " + path);
        return nullptr;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Parse the file
    Parser parser(source);
    ASTNode *node = parser.Parse();

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return nullptr;
    }
    
    if (node == nullptr) {
        mError::AddError("Failed to parse file: " + path);
        return nullptr;
    }
    
    // Evaluate the AST
    mModule* module = new mModule();
    mSymbolTable* symbolTable = mSymbolTable::NewScope(filePath.string());
    EvalVisitor::Eval(node, symbolTable, module);
    
    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return nullptr;
    }

    return module;
}
