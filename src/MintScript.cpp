#include "MintScript.h"
#include "parser.h"
#include "ast.h"
#include "error.h"
#include "globals.h"
#include "ast_visitor.h"
#include "builtin.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <windows.h>

ir::Instruction* mLoadFile(const std::string &path) {
    std::filesystem::path filePath(path);
    MAIN_FILE_PATH = std::filesystem::absolute(filePath).parent_path();

    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cout << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Parse the file
    Parser parser(source);

    ASTNode *node = parser.Parse();

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        delete node;
        return nullptr;
    }

    if (node == nullptr) {
        mError::AddError("Failed to parse file: " + path);
        return nullptr;
    }

    // Evaluate the AST
    AstVisitor* ast_visitor = AstVisitor::Eval(node);

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        delete node;
        delete ast_visitor;
        return nullptr;
    }

    ir::Instruction* instruction = ast_visitor->stack.top();

    delete node;
    delete ast_visitor;

    return instruction;
}

void mMain(int argc, char **argv) {
    mInit();

    if (argc > 1) {
        std::vector<std::string> args(argv + 1, argv + argc);
        
        bool printIR = false;
        std::string filePath = "";

        for (std::string arg : args) {
            if (arg == "-i" || arg == "--interactive") {
                mRunInteractive();
                return;
            } else if (arg == "-p" || arg == "--print") {
                printIR = true;
            } else {
                filePath = arg;
            }
        }

        if (filePath == "") {
            std::cout << "No file specified" << std::endl;
            return;
        }

        mRunFile(filePath, printIR);
        
    } else {
        mRunInteractive();
    }

    mShutdown();
}

void mInit() {
    // Initialize the MintScript context

    // Register the built-in functions
    mint_Root();

typedef void (__stdcall *MYPROC)(void); 

    // load function from dll called test.dll
    HINSTANCE hinstLib = LoadLibrary("test.dll");

    if (!hinstLib) {
        std::cout << "Could not load the dynamic library" << std::endl;
        return;
    }

    MYPROC root = (MYPROC) GetProcAddress(hinstLib, "mintroot");

    if (root != NULL) {
        root();
    }
    else {
        std::cout << "Could not load the function" << std::endl;
    }
}

void mShutdown() {
    // Shutdown the MintScript context
}

void mRunFile(const std::string &path, bool printIR) {
    ir::Instruction* irCode = mLoadFile(path);

    if (mError::HasError()) {
        return;
    }

    ir::Interpreter interpreter;
    
    ir::global->GetArgs().push_back(irCode);

    if (printIR) {
        interpreter.Print(ir::global);
    }
    
    interpreter.Interpret(ir::global);
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

void mint::Type(const std::string &name, const std::vector<Field> &fields, const std::vector<Method> &methods) {
    sa::global->SetType(name, { name });
    sa::Type* type = sa::global->GetType(name);

    for (Field field : fields) {
        type->AddField(field.name, { field.isMutable, field.type });
    }

    for (Method method : methods) {
        const std::string &methodName = "m_" + name + "_" + method.name;

        type->SetMethod(method.name, { methodName, t_function->GetVariant(method.args) });

        ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
            new ir::Instruction(ir::Decl, methodName, { }),
            new ir::Instruction(ir::Native, method.value, { })
        }));
    }
}

void mint::Function(const std::string &name, const std::vector<sa::Type *> &args, ir::Mainfold (*value)(std::vector<ir::Mainfold>)) {
    sa::global->SetSymbol(name, { false, "f_" + name, t_function->GetVariant(args) });
    ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
        new ir::Instruction(ir::Decl, "f_" + name, { }),
        new ir::Instruction(ir::Native, value, { })
    }));
}
