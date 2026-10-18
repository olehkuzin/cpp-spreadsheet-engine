# C++ Spreadsheet Engine

A spreadsheet engine in C++20. Cells hold numbers, strings or formulas. Formulas are
parsed into an abstract syntax tree (AST) and evaluated on demand. Written as the semester
assignment for the C++ programming course at FIT ČVUT (Czech Technical University in
Prague), 2024.

## Features

- Cells addressed as `A1`, with absolute and relative references (`$A$1`, `$A1`, `A$1`)
- Values: numbers, strings, and formulas starting with `=`
- Operators: `+ - * / ^`, unary minus, comparisons `= <> < <= > >=`
- Formula AST built from the course-provided expression parser through a `CExprBuilder`
  implementation (`src/myBuilder.*`)
- Cyclic dependency detection (a cell in a cycle evaluates to an empty value)
- `copyRect` copies a rectangle of cells and shifts relative references
- `save` / `load` the spreadsheet to and from a stream

The capabilities reported by `CSpreadsheet::capabilities()` are cyclic dependencies, file I/O and speed.

## Tech stack

C++20, CMake, and a prebuilt course expression parser (`third_party/expression_parser`).

## Project structure

```
src/                     Spreadsheet, AST nodes, CPos, builder
tests/main.cpp           Assertion-based test driver
third_party/expression_parser/
  include/expression.h   Parser interface (course-provided)
  lib/                   Prebuilt libexpression_parser.a (macOS arm64)
```

## Build and test

The prebuilt parser library exists only for **macOS on arm64** (Clang and GCC builds).

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build      # or ./build/spreadsheet_tests
```

Without CMake:

```sh
clang++ -std=c++20 -Isrc -Ithird_party/expression_parser/include src/*.cpp tests/main.cpp \
  -Lthird_party/expression_parser/lib/arm64-darwin23-clang -lexpression_parser -o spreadsheet_tests
./spreadsheet_tests
```

The test driver exits with code 0 when all assertions pass.

## License

The code in `src/` and `tests/` is under the [MIT license](LICENSE). The files in
`third_party/expression_parser/` were provided by FIT ČVUT and are not covered by it.
