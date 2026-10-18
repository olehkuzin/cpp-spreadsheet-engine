#include "CPos.h"
#include "ASTAbstractNode.h"
#include "NumericNode.h"
#include "StringNode.h"
#include "CellNode.h"
#include "OperatorNode.h"
#ifndef BIGHOMEWORKCPP_CSPREADSHEET_H
#define BIGHOMEWORKCPP_CSPREADSHEET_H
constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;



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


#endif //BIGHOMEWORKCPP_CSPREADSHEET_H
