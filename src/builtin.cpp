#include "builtin.h"
#include "ir.h"
#include "MintScript.h" 

#include <iostream>

sa::Type    *t_null = nullptr,
            *t_int = nullptr,
            *t_float = nullptr,
            *t_str = nullptr,
            *t_bool = nullptr,
            *t_type = nullptr,
            *t_function = nullptr,
            *t_void = nullptr;

ir::Mainfold str_Concat(std::vector<ir::Mainfold> args) {
    ir::Mainfold* self = args[0].value.mf;
    
    return {
        ir::Mainfold::String,
        new std::string(*self->value.s + *args[1].value.s)
    };
}

ir::Mainfold builtin_print(std::vector<ir::Mainfold> args) {
    ir::Mainfold mf = args[0];
    
    if (mf.type == ir::Mainfold::Field) mf = *mf.value.mf;
    
    switch (mf.type) {
        case ir::Mainfold::Int: std::cout << mf.value.i; break;
        case ir::Mainfold::Float: std::cout << mf.value.f; break;
        case ir::Mainfold::String: std::cout << *mf.value.s; break;
        case ir::Mainfold::Bool: std::cout << (mf.value.b ? "true" : "false"); break;
        case ir::Mainfold::Null: std::cout << "Null"; break;
        case ir::Mainfold::Field: std::cout << "{ field." << mf.value.mf << " }"; break;
        case ir::Mainfold::Object: std::cout << "{ object." << mf.value.st << " }"; break;
        case ir::Mainfold::Native: std::cout << "{ native." << (void*)mf.value.native << " }"; break;
        case ir::Mainfold::Scope: std::cout << "{ scope." << mf.value.ir << " }"; break;
        default: std::cout << "Unknown Mainfold type " << mf.type; break;
    }

    std::cout << "\n";

    return { ir::Mainfold::Null };
}

void mint_Root() {
    sa::global->SetType("Function", { "Function" });
    t_function = sa::global->GetType("Function");

    sa::global->SetType("null", { "null" });
    t_null = sa::global->GetType("null");

    sa::global->SetType("void", { "void" });
    t_void = sa::global->GetType("void");

    sa::global->SetType("int", { "int" });
    t_int = sa::global->GetType("int");

    sa::global->SetType("float", { "float" });
    t_float = sa::global->GetType("float");

    mint::Type("str",
        { },
        { { "Concat", { t_str, t_str } , str_Concat } }
    );
    t_str = sa::global->GetType("str");

    sa::global->SetType("bool", { "bool" });
    t_bool = sa::global->GetType("bool");

    mint::Function("print", { t_void, t_str } , builtin_print);
}