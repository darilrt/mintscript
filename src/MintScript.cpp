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

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

void* mLoadLib(const std::string& path) {
typedef void (*root_t)(void); 
#if defined(_WIN32) || defined(_WIN64)
    // load function from dll called test.dll
    HINSTANCE hinstLib = LoadLibrary(path.c_str());

    if (!hinstLib) {
        std::cout << "Could not load the dynamic library" << std::endl;
        return nullptr;
    }

    root_t root = (root_t) GetProcAddress(hinstLib, "mint_Root");

    if (root != NULL) {
        root();
    }
    else {
        std::cout << "Could not load the root function" << std::endl;
        FreeLibrary(hinstLib);
        return nullptr;
    }

    // Free the DLL module when you are finished with it:
    // FreeLibrary(hinstLib);
    return hinstLib;
#elif defined(__linux__)
    void* handle = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle) {
        std::cout << "Could not load the dynamic library" << std::endl;
        return nullptr;
    }

    root_t root = (root_t) dlsym(handle, "mint_Root");

    if (root != NULL) {
        root();
    }
    else {
        std::cout << "Could not load the root function" << std::endl;
        dlclose(handle);
        return nullptr;
    }

    return handle;
#endif
}

ir::Instruction* mLoadFile(const std::string &path, const std::string &moduleName) {
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
    AstVisitor* ast_visitor = AstVisitor::Eval(node, moduleName);

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

void mint::Main(int argc, char **argv) {
    
    Init();

    if (argc > 1) {
        std::vector<std::string> args(argv + 1, argv + argc);
        
        bool printIR = false;
        std::string filePath = "";

        for (std::string arg : args) {
            if (arg == "-i" || arg == "--interactive") {
                RunREPL();
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

        RunFile(filePath, printIR);
        
    } else {
        RunREPL();
    }

    Shutdown();
}

void mint::Init() {
    // Initialize the MintScript context

    // Register the built-in functions
    mint_Root();
}

void mint::Shutdown() {
    // Shutdown the MintScript context
}

void mint::RunFile(const std::string &path, bool printIR) {
    ir::Instruction* irCode = mLoadFile(path, "main");

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

void mint::RunREPL() {
    std::cout << "MintScript a" << MINT_VERSION << std::endl;
    std::cout << "Type 'exit()' to exit" << std::endl;

    std::string input;

    Parser parser("");
    ir::Interpreter interpreter;
    interpreter.Interpret(ir::global);

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

        parser = Parser(input);
        ASTNode *node = parser.Parse();

        // Check for errors
        if (mError::HasError()) {
            mError::PrintErrors();
            mError::ClearErrors();
            delete node;
            continue;
        }

        if (node == nullptr) {
            mError::AddError("Failed to parse input");
            mError::PrintErrors();
            mError::ClearErrors();
            continue;
        }

        // Evaluate the AST
        AstVisitor* ast_visitor = AstVisitor::Eval(node);

        // Check for errors
        if (mError::HasError()) {
            mError::PrintErrors();
            mError::ClearErrors();
            delete node;
            delete ast_visitor;
            continue;
        }
        
        ir::Instruction* irCode = ast_visitor->stack.top();
        irCode->value.i = 3;
        interpreter.Interpret(irCode);

        if (mError::HasError()) {
            mError::PrintErrors();
            mError::ClearErrors();
        }

        delete node;
        delete irCode;
        delete ast_visitor;
    }
}

sa::Type *mint::Type(const std::string &name) {
    return sa::global->GetType(name);
}

sa::Type *mint::Type(const std::string &name, const std::vector<Field> &fields, const std::vector<Method> &methods) {
    sa::global->SetType(name, { name });
    sa::Type* type = sa::global->GetType(name);

    for (Field field : fields) {
        type->AddField(field.name, { field.isMutable, field.type });
    }

    for (Method method : methods) {
        const std::string &methodName = "m" + name + "." + method.name;

        type->SetMethod(method.name, { methodName, t_function->GetVariant(method.args) });

        ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
            new ir::Instruction(ir::Decl, methodName, { }),
            new ir::Instruction(ir::Native, method.value, { })
        }));
    }

    return type;
}

void mint::Extend(const std::string &name, const std::vector<Field> &fields, const std::vector<Method> &methods) {
    sa::Type* type = sa::global->GetType(name);

    if (type == nullptr) {
        mError::AddError("Type '" + name + "' does not exist");
        return;
    }

    for (Field field : fields) {
        type->AddField(field.name, { field.isMutable, field.type });
    }

    for (Method method : methods) {
        const std::string &methodName = "m" + name + "_" + method.name;

        type->SetMethod(method.name, { methodName, t_function->GetVariant(method.args) });

        ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
            new ir::Instruction(ir::Decl, methodName, { }),
            new ir::Instruction(ir::Native, method.value, { })
        }));
    }
}

void mint::Function(const std::string &name, const std::vector<sa::Type *> &args, ir::Mainfold (*value)(std::vector<ir::Mainfold>)) {
    sa::global->SetSymbol(name, { false, "f" + name, t_function->GetVariant(args) });
    ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
        new ir::Instruction(ir::Decl, "f" + name, { }),
        new ir::Instruction(ir::Native, value, { })
    }));
}

void mint::Interface(const std::string &name, const std::vector<Method> &methods) {
    sa::global->SetType(name, { name });
    sa::Type* type = sa::global->GetType(name);
    type->isInterface = true;

    for (Method method : methods) {
        type->SetMethod(method.name, { method.name, t_function->GetVariant(method.args) });
    }
}

void mint::Implement(sa::Type *itrfce, sa::Type *type) {
    if (!itrfce->isInterface) {
        mError::AddError("Type " + itrfce->name + " is not an interface");
        return;
    }

    if (type->isInterface) {
        mError::AddError("Type " + type->name + " is an interface");
        return;
    }

    if (!type->Implements(itrfce)) {
        mError::AddError("Type " + type->name + " does not implement interface " + itrfce->name);
        return;
    }
    
    type->implements.insert(itrfce);

    std::vector<sa::Method> methods;

    for (auto p : itrfce->methods) {
        methods.push_back(p.second);
    }

    ir::Instruction* list = new ir::Instruction(ir::New, (int) methods.size(), { });

    std::sort(methods.begin(), methods.end(), [](sa::Method a, sa::Method b) {
        return a.offset < b.offset;
    });

    for (sa::Method method : methods) {
        list->GetArgs().push_back(new ir::Instruction(ir::Var, type->GetMethod(method.name)->name, { }));
    }

    ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
        new ir::Instruction(ir::Decl, "vt" + type->GetFullName() + "#" + itrfce->GetFullName(), { }),
        list
    }));
}

mint::TModule mint::Module(const std::string &name) {
    sa::global->SetModule(name, { name });
    sa::Module* mod = sa::global->GetModule(name);
    return { mod };
}

mint::TModule::TModule(sa::Module *mod) {
    this->mod = mod;
    mod->symbols = new sa::SymbolTable();
}

void mint::TModule::Type(const std::string &name, const std::vector<Field> &fields, const std::vector<Method> &methods) {
    mod->symbols->SetType(name, { name });
    sa::Type* type = mod->symbols->GetType(name);
}

void mint::TModule::Function(const std::string &name, const std::vector<sa::Type *> &args, ir::Mainfold (*value)(std::vector<ir::Mainfold>)) {
    const std::string fname = "f" + mod->name + "." + name;
    
    mod->symbols->SetSymbol(name, { false, fname, t_function->GetVariant(args) });
    ir::global->GetArgs().push_back(new ir::Instruction(ir::Set, {
        new ir::Instruction(ir::Decl, fname, { }),
        new ir::Instruction(ir::Native, value, { })
    }));
}
