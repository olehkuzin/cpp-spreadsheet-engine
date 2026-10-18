#include "CellNode.h"

// Implementation of deep copying.
CellNode* CellNode::clone() const {
    return new CellNode(position);
}

// Implementation of print method for CellNode.
void CellNode::print()const{
    std::cout << "CPos: " << position.getColumnStr() << position.getRow() << std::endl;
}
// Overloaded, but unused in my implementation evaluate method for Cell node.
CValue CellNode::evaluate() const {
    return {};
}

