#pragma once

#include "object.h"
#include "visitor.h"
#include "mlist.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual mList Accept(Visitor* visitor) = 0;
};
