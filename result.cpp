#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"
using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

constexpr unsigned                     SPREADSHEET_CYCLIC_DEPS                 = 0x01;
constexpr unsigned                     SPREADSHEET_FUNCTIONS                   = 0x02;
constexpr unsigned                     SPREADSHEET_FILE_IO                     = 0x04;
constexpr unsigned                     SPREADSHEET_SPEED                       = 0x08;
constexpr unsigned                     SPREADSHEET_PARSER                      = 0x10;
#endif /* __PROGTEST__ */

class CSpreadsheet;
class myBuilder;
class ASTAbstractNode;
class NumericNode;
class StringNode;
class CellNode;
class OperatorNode;
class CPos {
public:

    // Construct CPos from a string representation of a cell position.
    CPos(std::string_view str) {
        parsePosition(str);
    };

    // Construct CPos with specified column and row.
    CPos(long long column, int row, bool columnAbs = false, bool rowAbs = false)
            : row(row), column(column), absoluteColumn(columnAbs), absoluteRow(rowAbs) {
        column_str = intToColumnString(column);
    };

    // Getters for column and row properties.
    const long long getColumn() const { return column; }

    const int getRow() const { return row; }

    const std::string &getColumnStr() const { return column_str; }

    const bool isColumnAbsolute() const { return absoluteColumn; }

    const bool isRowAbsolute() const { return absoluteRow; }

    // Operator == for two CPos elements to check if them equals.
    bool operator==(const CPos &other) const {
        return row == other.row && column == other.column;
    }

private:
    int row;
    long long column; // Column representation as a number.
    std::string column_str;
    bool absoluteColumn = false; // Flags know if reference is absolute or not.
    bool absoluteRow = false;

    /**
     * Parsing function for the string representation of a cell position to extract
     * row, column, and absolute flags.
     */

    void parsePosition(std::string_view pos) {
        long long idx = 0;
        column = 0;
        if (pos[idx] == '$') { // Absolute column checking.
            absoluteColumn = true;
            ++idx;
        }
        long long size = pos.size();

        while (idx < size && std::isalpha(pos[idx])) {
            column = column * 26 + (std::toupper(pos[idx]) - 'A' + 1); // Formula of unique number representation for each string.
            column_str += toupper(pos[idx]);
            idx++;
        }

        if (idx < size && pos[idx] == '$') { // Absolute row checking.
            absoluteRow = true;
            ++idx;
        }

        if (idx == size) {
            throw std::invalid_argument("Invalid cell position format");
        }

        std::string_view row_part = pos.substr(idx);
        size_t converted = 0;
        try {
            row = std::stoi(std::string(row_part), &converted);
        } catch (const std::exception & e) {
            throw std::invalid_argument("Invalid cell position format: " + std::string(pos));
        }

        if (converted != row_part.size()) {
            throw std::invalid_argument("Invalid cell position format");
        }
    }
/**
 * Converting function to get a string representation of column, if that was constructed by a column number and row number.
 */

    static std::string intToColumnString(long long columnNumber){
        std::string result;
        int base = 26;

        while (columnNumber > 0) {
            columnNumber--;
            long long rem = columnNumber % base;
            result = char('A' + rem) + result;
            columnNumber /= base;
        }

        return result;
    }
};
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
class myBuilder : public CExprBuilder{
private:
    CSpreadsheet * tablePtr;  // Pointer to associated spreadsheet
    CPos position;            // Current cell position being built
public:
    // Constructor initializes builder with a spreadsheet, position will always be changed.
    myBuilder ( CSpreadsheet * table ): tablePtr( table ), position("A1" ){}

    // Setter for the current table.
    void setTable ( CSpreadsheet * table ){
        tablePtr = table;
    }
    // Setter for the current cell position.
    void setPos ( const CPos & pos ) { position = pos; }

    // Operator setters declaration for building expressions.
    void opAdd () override;
    void opSub () override;
    void opMul () override;
    void opDiv () override;
    void opPow () override;
    void opNeg () override;
    void opEq () override;
    void opNe () override;
    void opLt () override;
    void opLe () override;
    void opGt () override;
    void opGe () override;

    // Value setters declaration for building expressions.
    void valNumber ( double val ) override;
    void valString ( std::string val ) override;
    void valReference ( std::string val ) override;
    void valRange ( std::string val) override { };
    void funcCall ( std::string fnName, int paramCount ) override { };

};

namespace std {
    // Specializes std::hash for using CPos in unordered containers.
    template <>
    // Calculates a hash value for a CPos object using XOR to make this hash unique.
    struct hash<CPos> {
        size_t operator()(const CPos& pos) const {
            return std::hash<int>()(pos.getRow()) ^ std::hash<long long>()(pos.getColumn());
        }
    };
}

class CSpreadsheet {
public:
    // Returns spreadsheet capabilities as a bitmask.
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED;
    }

    // Default constructor initializing builder with this instance.
    CSpreadsheet () {}

    // Copy constructor.
    CSpreadsheet ( const CSpreadsheet &other) ;

    // Copy assignment operator.
    CSpreadsheet & operator = ( const CSpreadsheet &other );

    // Adds a node at a specified position.
    void addEl ( const CPos &pos, ASTAbstractNode *node );

    // Retrieves list of nodes at a specified position.
    std::list<std::shared_ptr<ASTAbstractNode>> getListByPos ( const CPos &pos ) const;

    // Sets the cell at the specified position with provided contents.
    bool setCell ( const CPos &pos, const std::string &contents );

    // Retrieves the value from the cell at the specified position.
    CValue getValue (const CPos &pos) const;

    // Copies a rectangle area from one position to another.
    void copyRect ( CPos dstCell, CPos srcCell, int w = 1, int h = 1 );

    // Saves the spreadsheet to a stream.
    bool save ( std::ostream &os) const;

    // Loads a spreadsheet from a stream.
    bool load ( std::istream &is );

private:
    std::unordered_map<CPos, std::list<std::shared_ptr<ASTAbstractNode>>> table; // Maps positions to node lists.

};

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

// Copy constructor: deep copy each node in the spreadsheet to a new spreadsheet.
CSpreadsheet::CSpreadsheet ( const CSpreadsheet & other ) {
    for ( const auto & pair : other.table ) {
        const CPos & pos = pair.first;
        std::list<std::shared_ptr<ASTAbstractNode>> clonedList;
        for ( const auto & node : pair.second ) {
            clonedList.push_back( std::shared_ptr<ASTAbstractNode>( node->clone() ) );
        }
        table[pos] = clonedList;
    }
}

// Defines the assignment operation for a CSpreadsheet object.
CSpreadsheet& CSpreadsheet::operator=(const CSpreadsheet& other) {
    // Prevent self-assignment.
    if (this != &other) {
        table.clear();  // Clears the current spreadsheet's content.

        // Iterate through each element in the other spreadsheet to copy.
        for (const auto & pair : other.table) {
            const CPos& pos = pair.first;  // The position of the cell.
            const std::list<std::shared_ptr<ASTAbstractNode>>& nodeList = pair.second;  // Nodes at this position.

            std::list<std::shared_ptr<ASTAbstractNode>> clonedList;  // List to hold cloned nodes.
            // Clone each node and add to the list.
            for (const auto & node : nodeList) {
                std::shared_ptr<ASTAbstractNode> clonedNode = std::shared_ptr<ASTAbstractNode>(node->clone());
                clonedList.push_back(clonedNode);
            }
            table[pos] = clonedList;  // Assign the cloned list to the corresponding position in the table.
        }
    }
    return *this;  // Return a reference to the current object.
}

// Retrieve list of AST nodes for the specified position; returns empty list if not found.
std::list<std::shared_ptr<ASTAbstractNode>> CSpreadsheet::getListByPos ( const CPos & pos ) const {
    auto it = table.find(pos);
    if ( it == table.end() ) {
        return {}; // Return an empty list if position is not in the map.
    }
    return it->second;
}

// Adds an element (node) to the spreadsheet at a specified position.
void CSpreadsheet::addEl(const CPos &pos, ASTAbstractNode* node) {
    table[pos].push_back(std::shared_ptr<ASTAbstractNode>(node));
}

bool CSpreadsheet::setCell(const CPos& pos, const std::string& contents) {
    try {
        myBuilder builder(this); // Builder for parsing and constructing expressions.
        table[pos].clear();  // Clears existing cell content.
        builder.setPos(pos);  // Sets the current position in the builder.
        parseExpression(contents, builder);  // Parses and builds the expression tree.
        return true;  // Successfully parsed and set.
    } catch (const std::exception& e) {
        table.erase(pos); // Erase if insert was nor successful;
        return false;
    }
}



std::vector<CPos> check; // Used for cyclic dependency checks.

CValue CSpreadsheet::getValue(const CPos & pos) const {
    auto node_list = getListByPos(pos);
    // Check for cyclic dependencies before processing
    for (auto & it : check) {
        if (it == pos){
            return CValue{}; // Return empty value on cycle detection
        }
    }
    check.push_back(pos); // Mark position as visited

    if (node_list.empty()) {
        check.pop_back(); // Clean up on exit
        return std::monostate{}; // Return uninitialized state if no nodes found
    }
    std::stack<CValue> evalStack;

    for (const auto& node : node_list) {
        // Push values or references onto the stack for evaluation
        if (std::dynamic_pointer_cast<NumericNode>(node) || std::dynamic_pointer_cast<StringNode>(node) || std::dynamic_pointer_cast<CellNode>(node)) {
            if (auto s = std::dynamic_pointer_cast<CellNode>(node)){
                evalStack.push(getValue(s->getPos())); // Recursive call for cell references
            }else{
                evalStack.push(node->evaluate()); // Evaluate and push value nodes
            }
        } else if (auto operatorNode = std::dynamic_pointer_cast<OperatorNode>(node)) {
            // Process operators, considering unary operators separately
            if (operatorNode->isUnary() && operatorNode->getOp() == "-") {
                auto operand = evalStack.top(); evalStack.pop();
                operatorNode->setOperand(operand);
                auto s = operatorNode->evaluate();
                if (std::holds_alternative<std::monostate>(s)){
                    check.pop_back(); // Clean up on error
                    return std::monostate();
                }
                evalStack.push(s);
            } else {
                // Ensure there are enough operands for binary operations
                if (evalStack.size() < 2) {
                    check.pop_back(); // Clean up on error
                    return CValue();
                }
                auto right = evalStack.top(); evalStack.pop();
                auto left = evalStack.top(); evalStack.pop();

                operatorNode->setLeft(left);
                operatorNode->setRight(right);
                auto s = operatorNode->evaluate();
                if (std::holds_alternative<std::monostate>(s)){
                    check.pop_back(); // Clean up on error
                    return std::monostate();
                }
                evalStack.push(s);
            }
        }
    }

    // After processing all nodes, there should be exactly one result on the stack
    if (evalStack.size() != 1 ) {
        check.pop_back(); // Clean up on error
        return std::monostate();
    }
    check.pop_back(); // Clean up after successful evaluation
    CValue resultNode = evalStack.top(); // Get the result from the stack
    return resultNode; // Return the evaluated result
}


void CSpreadsheet::copyRect(CPos dstCell, CPos srcCell, int w, int h) {
    // Temporary storage for the copied cells to handle overlapping copy areas properly
    std::unordered_map<CPos, std::list<std::shared_ptr<ASTAbstractNode>>> tempStorage;

    // Iterate through each cell in the specified rectangle
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            int newSrcRow = srcCell.getRow() + row;
            long long newSrcCol = srcCell.getColumn() + col;
            int newDstRow = dstCell.getRow() + row;
            long long newDstCol = dstCell.getColumn() + col;
            CPos srcPos(newSrcCol, newSrcRow);
            CPos dstPos(newDstCol, newDstRow);

            // Check if the source cell exists in the spreadsheet
            if (table.find(srcPos) != table.end()) {
                const auto& nodeList = table[srcPos];
                std::list<std::shared_ptr<ASTAbstractNode>> adjustedNodeList;

                // Clone each node, adjusting references as needed
                for (const auto& node : nodeList) {
                    if (auto tmp = std::dynamic_pointer_cast<CellNode>(node)) {
                        long long upgradeCol = tmp->getPos().isColumnAbsolute() ? tmp->getPos().getColumn() :
                                               tmp->getPos().getColumn() + (newDstCol - newSrcCol);
                        int upgradeRow = tmp->getPos().isRowAbsolute() ? tmp->getPos().getRow() :
                                         tmp->getPos().getRow() + (newDstRow - newSrcRow);

                        CPos newCellPos(upgradeCol, upgradeRow, tmp->getPos().isColumnAbsolute(), tmp->getPos().isRowAbsolute());
                        auto clonedCellNode = std::make_shared<CellNode>(newCellPos);
                        adjustedNodeList.push_back(clonedCellNode);
                    } else {
                        adjustedNodeList.push_back(std::shared_ptr<ASTAbstractNode>(node->clone()));
                    }
                }
                // Store modified list in temporary storage
                tempStorage[dstPos] = adjustedNodeList;
            }
        }
    }

    // Update the main table with the copied cells
    for (const auto& pair : tempStorage) {
        table[pair.first] = pair.second;
    }
}

bool CSpreadsheet::save(std::ostream &os) const {
    // Check if the output stream is ready to be written to
    if (!os.good()) {
        return false;
    }

    double checkSum = 0;  // Initialize checksum for data validation

    // Iterate through all cells in the spreadsheet
    for (auto & ptr : table) {
        // Start the block for a cell with its position
        os << ptr.first.getColumnStr() + std::to_string(ptr.first.getRow()) + "{\n";

        // Serialize each node in the cell
        for (auto & tabPtr : ptr.second) {
            if (auto n = std::dynamic_pointer_cast<NumericNode>(tabPtr)) {
                os << "Double Value: " << n->getNumVal() << "\n";
                checkSum += n->getNumVal();  // Add to checksum
            } else if (auto s = std::dynamic_pointer_cast<StringNode>(tabPtr)) {
                os << "String value: " << s->getStrVal() << "\n";
            } else if (auto k = std::dynamic_pointer_cast<CellNode>(tabPtr)) {
                // Write cell reference with absolute positioning notation
                os << "Reference: ";
                if (k->getPos().isColumnAbsolute()) {
                    os << "$";
                }
                os << k->getPos().getColumnStr();
                if (k->getPos().isRowAbsolute()) {
                    os << "$";
                }
                os << std::to_string(k->getPos().getRow()) + "\n";
            } else if (auto d = std::dynamic_pointer_cast<OperatorNode>(tabPtr)) {
                // Indicate unary operators with a prefix
                std::string isUn = d->isUnary() ? "-" : "+";
                os << "Operator: " << isUn << d->getOp() << "\n";
            }
        }

        // Close the block for this cell
        os << "}\n";
    }

    // Output the final checksum value
    os << "CheckSum: " << checkSum << "\n";

    return true;
}

bool CSpreadsheet::load(std::istream &is) {
    // Check if input stream is in a good state
    if (!is.good()) {
        return false;
    }

    std::string line;
    CPos currentPos("A1");
    std::list<std::shared_ptr<ASTAbstractNode>> nodeList;
    std::unordered_map<CPos,std::list<std::shared_ptr<ASTAbstractNode>>> tmp;
    double checkSum = 0;  // To verify data integrity

    while (getline(is, line)) {
        // Handle end of cell block or checksum line
        if (line.empty() || line[0] == '}' || line.substr(0, 8) == "CheckSum") {
            if (!nodeList.empty()) {
                tmp[currentPos] = nodeList;  // Store nodes temporarily
                nodeList.clear();
            }
            if (line.substr(0, 8) == "CheckSum") {
                double n = std::stod(line.substr(10));
                if (n != checkSum) {  // Verify checksum
                    return false;
                } else {
                    for (auto & pair : tmp) {  // Confirm temporary storage
                        table[pair.first] = pair.second;
                    }
                }
            }
            continue;
        }

        // Start of a new cell block
        if (line.back() == '{') {
            std::string cellPos = line.substr(0, line.size() - 1);
            currentPos = CPos(cellPos);
        } else {  // Process cell content
            if (line.find("Double Value: ") == 0) {
                double value = std::stod(line.substr(14));
                checkSum += value;  // Update checksum
                nodeList.push_back(std::make_shared<NumericNode>(value));
            } else if (line.find("String value: ") == 0) {
                std::string value = line.substr(14);
                nodeList.push_back(std::make_shared<StringNode>(value));
            } else if (line.find("Reference: ") == 0) {
                std::string value = line.substr(11);
                nodeList.push_back(std::make_shared<CellNode>(value));
            } else if (line.find("Operator: ") == 0) {
                std::string value = line.substr(11);
                bool isUnary = (line[10] == '-');
                nodeList.push_back(std::make_shared<OperatorNode>(value, isUnary));
            }
        }
    }

    return true;
}
#ifndef __PROGTEST__

bool                                   valueMatch                              ( const CValue                        & r,
                                                                                 const CValue                        & s )

{
  if ( r . index () != s . index () )
    return false;
  if ( r . index () == 0 )
    return true;
  if ( r . index () == 2 )
    return std::get<std::string> ( r ) == std::get<std::string> ( s );
  if ( std::isnan ( std::get<double> ( r ) ) && std::isnan ( std::get<double> ( s ) ) )
    return true;
  if ( std::isinf ( std::get<double> ( r ) ) && std::isinf ( std::get<double> ( s ) ) )
    return ( std::get<double> ( r ) < 0 && std::get<double> ( s ) < 0 )
           || ( std::get<double> ( r ) > 0 && std::get<double> ( s ) > 0 );
  return fabs ( std::get<double> ( r ) - std::get<double> ( s ) ) <= 1e8 * DBL_EPSILON * fabs ( std::get<double> ( r ) );
}
int main ()
{
  CSpreadsheet x0, x1;
  std::ostringstream oss;
  std::istringstream iss;
  std::string data;
  assert ( x0 . setCell ( CPos ( "A1" ), "10" ) );
  assert ( x0 . setCell ( CPos ( "A2" ), "20.5" ) );
  assert ( x0 . setCell ( CPos ( "A3" ), "3e1" ) );
  assert ( x0 . setCell ( CPos ( "A4" ), "=40" ) );
  assert ( x0 . setCell ( CPos ( "A5" ), "=5e+1" ) );
  assert ( x0 . setCell ( CPos ( "A6" ), "raw text with any characters, including a quote \" or a newline\n" ) );
  assert ( x0 . setCell ( CPos ( "A7" ), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\"" ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A1" ) ), CValue ( 10.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A2" ) ), CValue ( 20.5 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A3" ) ), CValue ( 30.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A4" ) ), CValue ( 40.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A5" ) ), CValue ( 50.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A6" ) ), CValue ( "raw text with any characters, including a quote \" or a newline\n" ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A7" ) ), CValue ( "quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++." ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "A8" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "AAAA9999" ) ), CValue() ) );
  assert ( x0 . setCell ( CPos ( "B1" ), "=A1+A2*A3" ) );
  assert ( x0 . setCell ( CPos ( "B2" ), "= -A1 ^ 2 - A2 / 2   " ) );
  assert ( x0 . setCell ( CPos ( "B3" ), "= 2 ^ $A$1" ) );
  assert ( x0 . setCell ( CPos ( "B4" ), "=($A1+A$2)^2" ) );
  assert ( x0 . setCell ( CPos ( "B5" ), "=B1+B2+B3+B4" ) );
  assert ( x0 . setCell ( CPos ( "B6" ), "=B1+B2+B3+B4+B5" ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 625.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -110.25 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 1024.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 930.25 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 2469.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 4938.0 ) ) );
  assert ( x0 . setCell ( CPos ( "A1" ), "12" ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 627.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -154.25 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 1056.25 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 5625.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 11250.0 ) ) );
  x1 = x0;
  assert ( x0 . setCell ( CPos ( "A2" ), "100" ) );
  assert ( x1 . setCell ( CPos ( "A2" ), "=A3+A5+A4" ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3612.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -204.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 17424.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 24928.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 49856.0 ) ) );
  oss . clear ();
  oss . str ( "" );
  assert ( x0 . save ( oss ) );
  data = oss . str ();
  iss . clear ();
  iss . str ( data );
  assert ( x1 . load ( iss ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
  assert ( x0 . setCell ( CPos ( "A3" ), "4e1" ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
  assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
  oss . clear ();
  oss . str ( "" );
  assert ( x0 . save ( oss ) );
  data = oss . str ();
  for ( size_t i = 0; i < std::min<size_t> ( data . length (), 10 ); i ++ )
    data[i] ^=0x5a;
  iss . clear ();
  iss . str ( data );
  assert ( ! x1 . load ( iss ) );
  assert ( x0 . setCell ( CPos ( "D0" ), "10" ) );
  assert ( x0 . setCell ( CPos ( "D1" ), "20" ) );
  assert ( x0 . setCell ( CPos ( "D2" ), "30" ) );
  assert ( x0 . setCell ( CPos ( "D3" ), "40" ) );
  assert ( x0 . setCell ( CPos ( "D4" ), "50" ) );
  assert ( x0 . setCell ( CPos ( "E0" ), "60" ) );
  assert ( x0 . setCell ( CPos ( "E1" ), "70" ) );
  assert ( x0 . setCell ( CPos ( "E2" ), "80" ) );
  assert ( x0 . setCell ( CPos ( "E3" ), "90" ) );
  assert ( x0 . setCell ( CPos ( "E4" ), "100" ) );
  assert ( x0 . setCell ( CPos ( "F10" ), "=D0+5" ) );
  assert ( x0 . setCell ( CPos ( "F11" ), "=$D0+5" ) );
  assert ( x0 . setCell ( CPos ( "F12" ), "=D$0+5" ) );
  assert ( x0 . setCell ( CPos ( "F13" ), "=$D$0+5" ) );
  x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 1, 4 );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
  x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 2, 4 );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H10" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H11" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue() ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( 35.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue() ) );
  assert ( x0 . setCell ( CPos ( "F0" ), "-27" ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
  x0 . copyRect ( CPos ( "H12" ), CPos ( "H13" ), 1, 2 );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue ( 25.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( -22.0 ) ) );
  assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
