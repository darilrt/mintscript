#pragma once

#include "object.h"
#include "visitor.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual mObject* Accept(Visitor* visitor) = 0;
};
