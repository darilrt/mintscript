#include "MintScript.h"
#include "ir.h"

#include <iostream>

int main(int argc, char** argv) {
    #define i new ir::Instruction

    ir::Interpreter interpreter;
    interpreter.Interpret(std::vector<ir::Instruction*> {
        i(ir::Set, "a", { i(ir::Int, 10, {} ) }),
        i(ir::Set, "b", { 
            i(ir::If, {
                i(ir::Eq, { i(ir::Var, "a", {}), i(ir::Int, 10, {}) }),
                i(ir::Scope, {
                    i(ir::Set, "b", { i(ir::Int, 20, {}) }),
                    i(ir::Set, "a", { i(ir::Int, 30, {}) }),
                    i(ir::Add, { i(ir::Var, "a", {}), i(ir::Var, "b", {}) }),
                }),
                i(ir::Int, 0, {})
            })
        }),
        
        i(ir::Call, "print", { i(ir::Var, "b", { }), }),
    });

    return 0;
}

