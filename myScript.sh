sed -n '1,45p' "test.cpp" > "result.cpp"
echo "class CSpreadsheet;" >> "result.cpp"
echo "class myBuilder;" >> "result.cpp"
echo "class ASTAbstractNode;" >> "result.cpp"
echo "class NumericNode;" >> "result.cpp"
echo "class StringNode;" >> "result.cpp"
echo "class CellNode;" >> "result.cpp"
echo "class OperatorNode;" >> "result.cpp"

sed -n '7,102p' "Cpos.h" >> "result.cpp"
sed -n '21,32p' "ASTAbstractNode.h" >> "result.cpp"
sed -n '8,45p' "myBuilder.h" >> "result.cpp"
sed -n '17,69p' "CSpreadsheet.h" >> "result.cpp"
sed -n '6,27p' "NumericNode.h" >> "result.cpp"
sed -n '5,25p' "StringNode.h" >> "result.cpp"
sed -n '6,29p' "CellNode.h" >> "result.cpp"
sed -n '6,49p' "OperatorNode.h" >> "result.cpp"

sed -n '2,77p' "myBuilder.cpp" >> "result.cpp"
sed -n '3,16p' "NumericNode.cpp" >> "result.cpp"
sed -n '3,17p' "StringNode.cpp" >> "result.cpp"
sed -n '3,16p' "CellNode.cpp" >> "result.cpp"
sed -n '3,132p' "OperatorNode.cpp" >> "result.cpp"
sed -n '4,290p' "CSpreadSheet.cpp" >> "result.cpp"

sed -n '75,229p' "test.cpp" >> "result.cpp"



