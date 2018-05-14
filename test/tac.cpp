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

TEST(ThreeAdressCode, Generate_asdaf)
{
	const char input[] =
	    "int fib(int n){if (n < 2) {return n;}return fib(n - 1) + fib(n - "
	    "2);}void main(){print(\"Please enter a number: \");int n;n = "
	    "read_int();print_nl();int result;result = "
	    "fib(n);print(\"fib(\");print_int(n);print(\") = "
	    "\");print_int(result);print_nl();}";
	auto result = mCc_parser_parse_string(input);

	auto tac = mCc_ast_get_tac_program(result.program);
	mCc_tac_print_tac(tac, stderr);
	mCc_tac_delete_tac(tac);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	fprintf(stderr, "\nSUCCESS\n");

	mCc_parser_delete_result(&result);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, 1);
}