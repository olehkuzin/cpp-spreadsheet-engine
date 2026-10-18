#ifndef BIGHOMEWORKCPP_OPERATORNODE_H
#define BIGHOMEWORKCPP_OPERATORNODE_H

#include "ASTAbstractNode.h"

class OperatorNode : public ASTAbstractNode {
    std::string op; // Stores the operator as a string.
    CValue operand; // Used if the operator is unary.
    CValue left;    // Left operand for binary operations.
    CValue right;   // Right operand for binary operations.
    bool isUnaryOperator = false; // Flag to indicate if the operator is unary.

public:
    // Constructor for binary operators.
    OperatorNode(const std::string & operatorStr)
            : op(operatorStr), operand({}), left({}), right({}), isUnaryOperator(false) {}

    // Constructor for unary operators.
    OperatorNode(const std::string & operatorStr, bool unaryFlag)
            : op(operatorStr), isUnaryOperator(unaryFlag) {}

    // Returns the operator string.
    const std::string & getOp() const { return op; }

    // Checks if the operator is unary.
    bool isUnary() const { return isUnaryOperator; }

    // Sets the operator to unary.
    void setUnary() { isUnaryOperator = true; }

    // Sets the operand for unary operations.
    void setOperand(const CValue &newOperand) { operand = newOperand; }

    // Sets the left operand for binary operations.
    void setLeft(const CValue &lhs) { left = lhs; }

    // Sets the right operand for binary operations.
    void setRight(const CValue &rhs) { right = rhs; }

    // Declaring of deep copying method.
    OperatorNode* clone() const override;

    // Evaluates the operator based on its type and operands.
    CValue evaluate() const override;

    // Prints the operator's details.
    void print() const override;
};


#endif //BIGHOMEWORKCPP_OPERATORNODE_H
