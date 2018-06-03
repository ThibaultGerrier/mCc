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
	const char input[] = "void main(){"
	                     "int a;"
	                     "a = -1;"
	                     "bool b;"
	                     "b = false;"
	                     "b = !b;"
	                     "float f;"
	                     "f = -1.3;"
	                     "}";
	auto result = mCc_parser_parse_string(input);

	auto fp = fopen("generated.s", "w");
	mCc_ass_print_assembler_program(result.program, fp);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	ASSERT_EQ(1, 0);
}