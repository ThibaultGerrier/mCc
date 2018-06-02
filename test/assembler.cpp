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
	                     "int b;"
	                     "a = 2 + 3;" // 5
	                     "print_int(a);"
	                     "print_nl();"
	                     "b = 4 * a;" // 20
	                     "print_int(b);"
	                     "print_nl();"
	                     "a = b/5 - 2;" // 2
	                     "print_int(a);"
	                     "print_nl();"
						 "a = a * 4 + 3;" // 11
						 "print_int(a);"
						 "print_nl();"
	                     "}";
	auto result = mCc_parser_parse_string(input);

	auto fp = fopen("generated.s", "w");
	mCc_ast_print_assembler_program(result.program, stderr);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	//ASSERT_EQ(1, 0);
}

TEST(ThreeAdressCode, TestAssemblerFunc)
{
	const char input[] = "int add(int a, int b){"
						 "return a + b;"
						 "}"
						 "void main(){"
						 "int a;"
						 "int b;"
						 "int c;"
						 "a = 4;"
						 "b = 5;"
						 "c = add(a,b);" // 9
						 "print_int(c);"
						 "print_nl();"
						 "}";
	auto result = mCc_parser_parse_string(input);

	auto fp = fopen("generated.s", "w");
	mCc_ast_print_assembler_program(result.program, stderr);
	fclose(fp);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	ASSERT_EQ(1, 0);
}