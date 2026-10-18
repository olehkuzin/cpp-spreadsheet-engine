cmake_minimum_required(VERSION 3.26)
project(bigHomeworkcpp)

set(CMAKE_CXX_STANDARD 20)
link_directories(${CMAKE_SOURCE_DIR}/arm64-darwin23-clang/)

add_executable(bigHomeworkcpp main.cpp
        CPos.h
        ASTAbstractNode.h
        myBuilder.cpp
        myBuilder.h
        CSpreadsheet.cpp
        CSpreadsheet.h
        NumericNode.cpp
        NumericNode.h
        StringNode.cpp
        StringNode.h
        CellNode.cpp
        CellNode.h
        OperatorNode.cpp
        OperatorNode.h
)
target_link_libraries(bigHomeworkcpp PRIVATE libexpression_parser.a)
target_include_directories(bigHomeworkcpp PRIVATE ${CMAKE_SOURCE_DIR})