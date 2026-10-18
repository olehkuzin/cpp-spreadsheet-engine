#include "OperatorNode.h"

// Clones this node, creating a copy with the same operation settings.
OperatorNode* OperatorNode::clone() const {
    auto newNode = new OperatorNode(op);

    if(isUnaryOperator){
        newNode->setUnary();
        newNode->setOperand(operand);
    }else{
        newNode->setLeft(left);
        newNode->setRight(right);
    }
    return newNode;
}

// Implementation of print method for OperatorNode.
void OperatorNode::print()const{
    std::cout << "Operator : " << op << std::endl;
}

// Evaluate the result based on operator type and operand values.
CValue OperatorNode::evaluate() const {

    if (isUnaryOperator && holds_alternative<double>(operand)) {
        // Handle unary negation for numeric values.
        if (op == "-" ) {
            double value = 0 - std::get<double>(operand);
            return CValue{value};
        }
    }else {
        // Handle binary operations.
        if (op == "+") {
            // Addition for doubles.
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double sum = std::get<double>(left) + std::get<double>(right);
                return CValue{sum};
            }
                // Concatenation for strings.
            else if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
                std::string leftStr = std::holds_alternative<double>(left) ? std::to_string(std::get<double>(left)) : std::get<std::string>(left);
                std::string rightStr = std::holds_alternative<double>(right) ? std::to_string(std::get<double>(right)) : std::get<std::string>(right);
                return CValue{leftStr + rightStr};
            }
            // Subtraction for doubles.
        }else if (op == "-"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double sub = std::get<double>(left) - std::get<double>(right);
                return CValue{sub};
            }
            // Multiplication for doubles.
        }else if(op == "*"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double mul = std::get<double>(left) * std::get<double>(right);
                return CValue{mul};
            }
            // Division for doubles, check for division by zero.
        }else if(op == "/"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                if (std::get<double>(right) == 0){
                    return std::monostate{};
                }
                double div = std::get<double>(left) / std::get<double>(right);
                return CValue{div};
            }
            // Exponentiation for doubles.
        }else if(op == "^"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double pows = pow(std::get<double>(left), std::get<double>(right));
                return CValue{pows};
            }
            // Equality comparison for doubles and strings.
        }else if(op == "="){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double eq = std::get<double>(left) == std::get<double>(right);
                return CValue{eq};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double eq = std::get<std::string>(left) == std::get<std::string>(right);
                return CValue{eq};
            }
            // Inequality comparison for doubles and strings.
        }else if(op == "<>"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double neq = std::get<double>(left) != std::get<double>(right);
                return CValue{neq};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double neq = std::get<std::string>(left) != std::get<std::string>(right);
                return CValue{neq};
            }
            // Less than comparison for doubles and strings.
        }else if(op == "<"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double lt = std::get<double>(left) < std::get<double>(right);
                return CValue{lt};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double lt = std::get<std::string>(left) < std::get<std::string>(right);
                return CValue{lt};
            }
            // Less than or equal to comparison for doubles and strings.
        }else if(op == "<="){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double lte = std::get<double>(left) <= std::get<double>(right);
                return CValue{lte};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double lte = std::get<std::string>(left) <= std::get<std::string>(right);
                return CValue{lte};
            }
            // Greater than comparison for doubles and strings.
        }else if(op == ">"){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double gt = std::get<double>(left) > std::get<double>(right);
                return CValue{gt};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double gt = std::get<std::string>(left) > std::get<std::string>(right);
                return CValue{gt};
            }
            // Greater than or equal to comparison for doubles and strings.
        }else if(op == ">="){
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double gte = std::get<double>(left) >= std::get<double>(right);
                return CValue{gte};
            }else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                double gte = std::get<std::string>(left) >= std::get<std::string>(right);
                return CValue{gte};
            }

        }
    }

    return std::monostate{};
}
