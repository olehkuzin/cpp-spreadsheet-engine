cmake_minimum_required(VERSION 3.20)
project(cpp_spreadsheet_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Course-provided expression parser (prebuilt, macOS arm64 only)
set(EXPRESSION_PARSER_DIR ${CMAKE_SOURCE_DIR}/third_party/expression_parser)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(EXPRESSION_PARSER_LIB_DIR ${EXPRESSION_PARSER_DIR}/lib/arm64-darwin23-g++)
else()
    set(EXPRESSION_PARSER_LIB_DIR ${EXPRESSION_PARSER_DIR}/lib/arm64-darwin23-clang)
endif()

add_library(expression_parser STATIC IMPORTED)
set_target_properties(expression_parser PROPERTIES
        IMPORTED_LOCATION ${EXPRESSION_PARSER_LIB_DIR}/libexpression_parser.a
        INTERFACE_INCLUDE_DIRECTORIES ${EXPRESSION_PARSER_DIR}/include)

add_library(spreadsheet
        src/CSpreadsheet.cpp
        src/myBuilder.cpp
        src/NumericNode.cpp
        src/StringNode.cpp
        src/CellNode.cpp
        src/OperatorNode.cpp
)
target_include_directories(spreadsheet PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(spreadsheet PUBLIC expression_parser)

add_executable(spreadsheet_tests tests/main.cpp)
target_link_libraries(spreadsheet_tests PRIVATE spreadsheet)

enable_testing()
add_test(NAME spreadsheet_tests COMMAND spreadsheet_tests)
