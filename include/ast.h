#pragma once

#include "visitor.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual sa::Symbol Accept(Visitor* visitor) = 0;
};
