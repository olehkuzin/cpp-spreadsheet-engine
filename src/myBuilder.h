#include "expression.h"
#include "CSpreadsheet.h"

#ifndef BIGHOMEWORKCPP_MYBUILDER_H
#define BIGHOMEWORKCPP_MYBUILDER_H


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


#endif //BIGHOMEWORKCPP_MYBUILDER_H
