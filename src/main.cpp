#include "MintScript.h"
#include "ir.h"

#include <iostream>

int main(int argc, char** argv) {
    #define i new ir::Instruction

    ir::Interpreter interpreter;
    interpreter.Interpret(std::vector<ir::Instruction*> {
        i(ir::Set, "a", { 
            i(ir::Add, { 
                i(ir::Int, 1, {}), 
                i(ir::Int, 2, {}) 
            }) 
        }),
        i(ir::Var, "a", {})
    });

    return 0;
}

