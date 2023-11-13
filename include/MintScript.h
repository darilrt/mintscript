#pragma once

#if defined(MINT_EXPORTS)
    #define MINT_API __declspec(dllexport)
#else
    #define MINT_API __declspec(dllimport)
#endif

#include "ir.h"
#include "sa_symbol.h"

#include <string>
#include <vector>

extern MINT_API sa::Type *t_null,
                *t_int,
                *t_float,
                *t_str,
                *t_bool,
                *t_type,
                *t_function,
                *t_void;

namespace mint {

    MINT_API void Main(int argc, char** argv);

    void Init();

    void Shutdown();

    void RunFile(const std::string &path, bool printIR=false);

    void RunREPL();

    class Method {
    public:
        std::string name;
        std::vector<sa::Type*> args;
        ir::Mainfold (*value)(std::vector<ir::Mainfold>);

        Method() = default;
    };

    class Field {
    public:
        std::string name;
        sa::Type* type;
        bool isMutable;

        Field() = default;
    };
    
    MINT_API void Type(
        const std::string &name,
        const std::vector<Field> &fields,
        const std::vector<Method> &methods
    );

    MINT_API void Function(
        const std::string &name,
        const std::vector<sa::Type*> &args,
        ir::Mainfold (*value)(std::vector<ir::Mainfold>)
    );

}
