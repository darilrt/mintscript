#include "MintScript.h"
#include "ir.h"

#include <iostream>

int main(int argc, char** argv) {
    #define i new ir::Instruction

    ir::Interpreter interpreter;
    interpreter.Interpret(std::vector<ir::Instruction*> {
        i(ir::Set, "sum", { 
            i(ir::IR, {
                i(ir::Set, "a", { i(ir::Arg, 0, { }) }),
                i(ir::Set, "b", { i(ir::Arg, 1, { }) }),
                i(ir::Call, "print", { 
                    i(ir::Add, {
                        i(ir::Var, "a", { }),
                        i(ir::Var, "b", { })
                    })
                })
            })
        }),
        i(ir::Call, "sum", { 
            i(ir::Int, 5, { }),
            i(ir::Int, 6, { }),
        }),
    });

    return 0;
}

