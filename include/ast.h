#pragma once

#include "visitor.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual sa::Type* Accept(Visitor* visitor) = 0;
};
