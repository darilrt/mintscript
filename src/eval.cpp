#include "eval.h"
#include "debug_visitor.h"

#include <iostream>

mObject* Eval::Evaluate() {
    EvalVisitor visitor;
    return ast->Accept(&visitor);
}