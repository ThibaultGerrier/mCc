#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include <mCc/tac_print.h>

void print_all_errors(std::string prefix,
                      struct mCc_err_error_manager *error_manager)
{
    for (size_t i = 0; i < error_manager->used; i++) {
        std::cerr << prefix << ": " << error_manager->array[i]->msg
                  << std::endl;
    }
}

TEST(ControllFlowGraph, TestCFG1)
{
    const char input[] = "void main(int x){"
            "while(x>0){"
            "x=x-1;"
            "}"
            "}";

    auto result = mCc_parser_parse_string(input);
    auto tac = mCc_ast_get_tac_program(result.program);
    mCc_tac_print_dot(stderr,tac);
    //mCc_tac_print_tac(tac,stderr);
    ASSERT_EQ(1,2);

}
