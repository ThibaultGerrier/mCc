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
	const char input[] = "int add(int a, int b){\n"
	                     "\treturn a + b;\n"
	                     "}\n"
	                     "void main(){\n"
	                     "int r;\n"
	                     "r = 4;\n"
	                     "int tr;"
	                     "tr = add(r, r);\n"
	                     "print_int(tr);\n"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto fp = fopen("generated.s", "w");
	mCc_ass_print_assembler_program(result.program, fp);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	// ASSERT_EQ(1, 0);
}

TEST(ThreeAdressCode, TestAssembler)
{
	const char input[] = "int add(int a, int b){\n"
	                     "\treturn a + b;\n"
	                     "}\n"
	                     "int inc(int b){\n"
	                     "\treturn b + 1;\n"
	                     "}\n"
	                     "void main(){\n"
	                     "int r;\n"
	                     "r = 4;\n"
	                     "int tr;"
	                     "tr = add(inc(r), r);\n"
	                     "print_int(tr);\n"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto fp = fopen("generated.s", "w");
	mCc_ass_print_assembler_program(result.program, fp);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	// ASSERT_EQ(1, 0);
}