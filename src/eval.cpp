#include "eval.h"
#include "eval_visitor.h"
#include "mlist.h"

#include <iostream>

mList Eval::Evaluate() {
    EvalVisitor visitor;
    return ast->Accept(&visitor);
}