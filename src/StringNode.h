#ifndef BIGHOMEWORKCPP_STRINGNODE_H
#define BIGHOMEWORKCPP_STRINGNODE_H
#include "ASTAbstractNode.h"

// Represents a node that stores string values.
class StringNode : public ASTAbstractNode {
    std::string value; // Holds the string value for this node.

public:
    // Constructor initializes the node with a string value.
    StringNode(const std::string &val) : value(val) {}

    // Getter for a string inside node.
    const std::string & getStrVal(){return value;}

    // Prints the value of this node.
    void print() const override;

    // Creates a deep copy of this node.
    StringNode* clone() const override;

    // Evaluates and returns the string value as a CValue.
    CValue evaluate() const override;
};


#endif //BIGHOMEWORKCPP_STRINGNODE_H
