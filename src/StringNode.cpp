#include "StringNode.h"

// Implementation of the clone function.
StringNode* StringNode::clone() const {
    return new StringNode(value);
}

// Implementation of the evaluate function.
CValue StringNode::evaluate() const {
    return {value} ;
}

// Implementation of printing method for StringNode.
void StringNode::print()const{
    std::cout << "String : " << get<std::string>(evaluate()) << std::endl;
}
