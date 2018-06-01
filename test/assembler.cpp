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
	const char input[] = "void main(){int[3] arr; arr[0] = 3; arr[2] = 4;float[3] f; f[2] = 4.1; float f1; f1= f[1];}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_assembler_program(result.program, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	ASSERT_EQ(1, 0);
}