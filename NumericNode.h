#ifndef BIGHOMEWORKCPP_NUMERICNODE_H
#define BIGHOMEWORKCPP_NUMERICNODE_H
#include "ASTAbstractNode.h"
#include "cstdlib"

// Represents a node that stores numeric values.
class NumericNode : public ASTAbstractNode {
    double value; // Holds the numeric value for this node.

public:
    // Constructor initializes the node with a numeric value.
    NumericNode ( double val ) : value ( val ) {}

    // Getter for a number inside node.
    const double getNumVal(){return value;}

    // Prints the value of this node.
    void print () const override;

    // Creates a deep copy of this node.
    NumericNode* clone () const override;

    // Evaluates and returns the numeric value as a CValue.
    CValue evaluate () const override;

};


#endif //BIGHOMEWORKCPP_NUMERICNODE_H
