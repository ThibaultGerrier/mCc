#include <gtest/gtest.h>

#include "mCc/assembler.h"
#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"
#include "mCc/tac.h"
#include <cstdbool>
#include <string>

TEST(ThreeAdressCode, TestAssembler1)
{
	const char input[] = "void foo(int[3] arr){"
	                     "int x;"
	                     "x = arr[0];"
	                     "print_int(x);"
	                     "arr[1] = 55;"
	                     "print_int(55);"
	                     "}"
	                     "void main(){"
	                     "int[3] arr;"
	                     "arr[0] = 3;"
	                     "arr[2] = 4;"
	                     "foo(arr);"
	                     "print_int(arr[1]);"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto fp = fopen("generated.s", "w");
	mCc_ass_print_assembler_program(result.program, fp);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	// ASSERT_EQ(1, 0);
}
/*
TEST(ThreeAdressCode, TestAssembler)
{
    const char input[] = "void main(){"
                         "int a;"
                         "a = 3;"
                         "print_int(a);"
                         "}";
    auto result = mCc_parser_parse_string(input);
    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    auto fp = fopen("generated.s", "w");
    mCc_ass_print_assembler_program(result.program, fp);
    fclose(fp);
    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    // ASSERT_EQ(1, 0);
}*/