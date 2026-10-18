#include "CSpreadsheet.h"
#include "myBuilder.h"


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
