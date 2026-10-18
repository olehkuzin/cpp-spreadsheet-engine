#include "ASTAbstractNode.h"

#ifndef BIGHOMEWORKCPP_CELLNODE_H
#define BIGHOMEWORKCPP_CELLNODE_H


class CellNode : public ASTAbstractNode {
    CPos position;

public:
    // Constructs a CellNode from a string representation of the position.
    CellNode(const std::string& pos) : position(CPos(pos)) {}

    // Constructs a CellNode from a CPos object.
    CellNode(const CPos& pos) : position(pos) {}

    // Returns the position of this cell node.
    const CPos & getPos() const { return position; }

    // Prints cell information.
    void print() const override;

    // Overloaded, but unused in my implementation evaluate method for Cell node.
    CValue evaluate() const override;

    // Deep copying for CellNode.
    CellNode* clone() const override;
};


#endif //BIGHOMEWORKCPP_CELLNODE_H
