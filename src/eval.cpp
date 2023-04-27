#include "eval.h"
#include "debug_visitor.h"
#include "mlist.h"

#include <iostream>

mList Eval::Evaluate() {
    EvalVisitor visitor;
    return ast->Accept(&visitor);
}