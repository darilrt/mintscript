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

MINT_API void mMain(int argc, char** argv);

void mInit();

void mShutdown();

void mRunFile(const std::string &path, bool printIR=false);

void mRunString(const std::string &source);

void mRunInteractive();

namespace mint {

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
