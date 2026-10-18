#include <string>

#ifndef BIGHOMEWORKCPP_CPOS_H
#define BIGHOMEWORKCPP_CPOS_H


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


#endif //BIGHOMEWORKCPP_CPOS_H
