#include <gtest/gtest.h>

#include "mCc/assembler.h"
#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"
#include "mCc/tac.h"
#include <cstdbool>
#include <string>

TEST(ThreeAdressCode, TestAssembler)
{
	const char input[] = "void main(){int a; int b; a = 0; b = 1;}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_assembler_program(result.program, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, TestAssemblerStr)
{
	const char input[] = "void main(){float fl; fl = 3.1; string a; a = "
	                     "\"heelo\"; print(\"das\"); print(a);}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_assembler_program(result.program, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, TestAssemblerFunction)
{
	const char input[] =
	    "void foo(int a, int[3] arr){int c; c = 3; int b; b = 4; bool bo; bo = "
	    "true;} void main(){int x; x = 5; int k; k=4;}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_assembler_program(result.program, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, TestAssemblerSimpleAssignemtns)
{
	const char input[] =
	    "void main(){int x; x = 5; int k; k=4; x = k; if(k == 4){k=3;}}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_assembler_program(result.program, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	ASSERT_EQ(1, 0);
}