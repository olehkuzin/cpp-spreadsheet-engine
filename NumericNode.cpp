#include "NumericNode.h"

// Implementation of copying method of NumericNode;
NumericNode* NumericNode::clone() const {
    return new NumericNode(*this);
}

// Implementation of evaluate method of NumericNode;
CValue NumericNode::evaluate() const {
    return { value };
}
// Implementation of printing method for NumericNode.
void NumericNode::print()const{
    std::cout << "Double: " << get<double>(evaluate()) << std::endl;
}
