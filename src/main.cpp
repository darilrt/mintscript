#include "MintScript.h"
#include "ir.h"

#include <iostream>

int main(int argc, char** argv) {
    #define i new ir::Instruction

    ir::Interpreter interpreter;
    interpreter.Interpret(std::vector<ir::Instruction*> {
        i(ir::Decl, "person_print", { }),
        i(ir::Set, {
            i(ir::Var, "person_print", { }),
            i(ir::IR, {
                i(ir::Call, {
                    i(ir::String, "print", { }),
                    i(ir::Field, 0, { i(ir::Arg, 0, { }) }),
                })
            })
        }),

        i(ir::Decl, "person", { }),
        i(ir::Set, {
            i(ir::Var, "person", { }),
            i(ir::New, 1, { })
        }),
        i(ir::Set, {
            i(ir::Field, 0, { i(ir::Var, "person", { }) }),
            i(ir::String, "John", { })
        }),
        i(ir::Call, {
            i(ir::Var, "person_print", { }),
            i(ir::Var, "person", { }),
        })
    });

    return 0;
}

