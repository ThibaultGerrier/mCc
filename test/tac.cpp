#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/tac.h"

void print_error(struct mCc_parse_error parse_error)
{
	struct mCc_error_location location = parse_error.location;
	char line[10], column[10];
	if (location.first_line != location.last_line) {
		sprintf(line, "%d-%d", location.first_line, location.last_line);
	} else {
		sprintf(line, "%d", location.first_line);
	}
	if (location.first_column != location.last_column) {
		sprintf(column, "%d-%d", location.first_column, location.last_column);
	} else {
		sprintf(column, "%d", location.first_column);
	}
	fprintf(stderr, "Error (line %s, column %s): %s\n", line, column,
	        parse_error.msg);
}

TEST(ThreeAdressCode, Generate_Sample_TAC)
{
	const char input[] = "void main(){ int a; int b; a = 129; a = a - 12; b = "
	                     "0; b = (a + 1) + (b - 2);}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Sample_TAC2)
{
	const char input[] = "void main(){ int a; a = (a + 1) + (12 - 2);}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Sample_IF)
{
	const char input[] = "void main(){ int a; a = 2; if (a == 2){ a = a + 1;} "
	                     "else { a = a - 1; }}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Sample_While)
{
	const char input[] =
	    "void main(){ int a; a = 2; while (a > 0){ a = a - 1;} a = 3;}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Sample_Functions)
{

	const char input[] =
	    "void main(){ int asd; } void foo(){int a; a=2;} void asd(){}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_Calls)
{

	const char input[] = "void foo(){} void main(){int a; a=1; foo();}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_Calls_Push)
{

	const char input[] =
	    "void foo(int a, int b){int c; c=a+b;} void main(){foo(1,2);}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_Return)
{

	const char input[] = "void foo(){return 1;} void main(){foo();}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_CallinCall)
{

	const char input[] =
	    "int fooInt(int i){return i;} void main(){fooInt(fooInt(1));}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_Array)
{

	const char input[] = "void main(){float[5] arr; arr[0]=1.2;arr[4]=1.2;}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_Function_Array_Expr)
{
	const char input[] = "void main(){int[2] arr; arr[0] = 1; arr[1] = 4; "
	                     "int[2] arr2; arr2[0] = arr[0];}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_PushPopOrder)
{
	const char input[] = "int foo(int a, int b){return 1;} void main(){foo(1,2);}";
	auto result = mCc_parser_parse_string(input);

	mCc_ast_print_tac_program(stderr, result.program);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

}

TEST(ThreeAdressCode, Generate_DOUBLE)
{
    const char input[] = "void main(int[5] arr){int[5] avcvv; avcvv[0]=arr[0];}";
    auto result = mCc_parser_parse_string(input);

    mCc_ast_print_tac_program(stderr, result.program);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}

TEST(ThreeAdressCode, Generate_exprearr)
{
    const char input[] = "void main(){int[5] arr; int b; b = 1 + (2 - arr[0]);}";
    auto result = mCc_parser_parse_string(input);

    mCc_ast_print_tac_program(stderr, result.program);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, 1);
}
