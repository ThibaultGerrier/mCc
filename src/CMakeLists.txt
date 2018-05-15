# find BISON + FLEX targets
# NOTE: they have to be in upper-case
find_package(BISON)
find_package(FLEX)

# build flex- and bison-targets
flex_target(scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h)
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c COMPILE_FLAGS "-Wall" DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h)

add_flex_bison_dependency(scanner parser)

# basic ast
add_library(mCc_ast SHARED  ast.c ast_function_return_check.c ../include/mCc/ast_function_return_check.h)

# visitors
add_library(mCc_visit SHARED ast_visit.c)

# prints
add_library(mCc_print SHARED ast_print.c)

# symbol table
add_library(mCc_symbol SHARED symbol_table.c)

# ast symbol table
add_library(mCc_ast_symbol SHARED ast_symbol_table.c)

# ast symbol table
add_library(mCc_ast_type SHARED ast_type_checking.c)

# intermediate linking of libraries
target_link_libraries(mCc_print mCc_ast mCc_visit mCc_symbol mCc_ast_symbol mCc_ast_type)

# setup mCc_lib
add_library(mCc_lib SHARED ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})

# link remaining
target_link_libraries(mCc_lib mCc_print)

set_property(TARGET mCc_lib PROPERTY COMPILE_FLAGS "-D_POSIX_C_SOURCE=200809L")
