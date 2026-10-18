#include "myBuilder.h"

// Adds an addition operator to the current expression.
void myBuilder::opAdd(){
    tablePtr->addEl(position, new OperatorNode("+"));
}

// Adds a subtraction operator to the current expression.
void myBuilder::opSub(){
    tablePtr->addEl(position, new OperatorNode("-"));
}

// Adds a multiplication operator to the current expression.
void myBuilder::opMul(){
    tablePtr->addEl(position, new OperatorNode("*"));
}

// Adds a division operator to the current expression.
void myBuilder::opDiv(){
    tablePtr->addEl(position, new OperatorNode("/"));
}

// Adds a power operator to the current expression.
void myBuilder::opPow(){
    tablePtr->addEl(position, new OperatorNode("^"));
}

// Adds a negation operator to the current expression.
void myBuilder::opNeg(){
    tablePtr->addEl(position, new OperatorNode("-", true));
}

// Adds an equality operator to the current expression.
void myBuilder::opEq(){
    tablePtr->addEl(position, new OperatorNode("="));
}

// Adds a not-equal operator to the current expression.
void myBuilder::opNe(){
    tablePtr->addEl(position, new OperatorNode("<>"));
}

// Adds a less-than operator to the current expression.
void myBuilder::opLt(){
    tablePtr->addEl(position, new OperatorNode("<"));
}

// Adds a less-than-or-equal-to operator to the current expression.
void myBuilder::opLe() {
    tablePtr->addEl(position, new OperatorNode("<="));
}

// Adds a greater-than operator to the current expression.
void myBuilder::opGt(){
    tablePtr->addEl(position, new OperatorNode(">"));
}

// Adds a greater-than-or-equal-to operator to the current expression.
void myBuilder::opGe(){
    tablePtr->addEl(position, new OperatorNode(">="));
}

// Adds a numeric value to the current expression.
void myBuilder::valNumber(double val){
    tablePtr->addEl(position, new NumericNode(val));
}

// Adds a string value to the current expression.
void myBuilder::valString(std::string val) {
    tablePtr->addEl(position, new StringNode(val));
}

// Adds a cell reference to the current expression.
void myBuilder::valReference(std::string val){
    tablePtr->addEl(position, new CellNode(val));
}


