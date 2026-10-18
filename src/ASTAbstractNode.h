#include <cstdlib>
#include <variant>
#include <string>
#include <unordered_map>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include "list"

#include "CPos.h"

using CValue = std::variant<std::monostate, double, std::string>;

#ifndef BIGHOMEWORKCPP_ASTABSTRACTNODE_H
#define BIGHOMEWORKCPP_ASTABSTRACTNODE_H



class ASTAbstractNode {
public:

    // Virtual destructor for proper cleanup of derived classes.
    virtual ~ASTAbstractNode() = default;
    // Method to get a value of node.
    virtual CValue evaluate() const = 0;
    // Method print to print a value of node, useful for debugging.
    virtual void print() const = 0;
    // Clone method to get deep copies of each node.
    virtual ASTAbstractNode* clone() const = 0;
};


#endif //BIGHOMEWORKCPP_ASTABSTRACTNODE_H
