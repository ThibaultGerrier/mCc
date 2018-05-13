
#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/ast_type_checking.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"

#include <cstdbool>

TEST(TypeChecking, SimpleAssignmentCorrect)
{
	const char input[] = "void main(){int a; a = 123;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleArrayAssignmentCorrect)
{
	const char input[] = "void main(){string[3] b; b[0] = \"Foo\";}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleAdditionCorrect)
{
	const char input[] = "void main(){int a; a = 123; int b; b = 321; a + b;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexAdditionCorrect)
{
	const char input[] = "void main(){int a; a = 123; int b; b = 321; int c; c "
	                     "= a; (a + b) + c;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleReturnAssigmentCorrect)
{
	const char input[] =
	    "int foo() {return 42;} void main(){int a; a = foo();}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, VoidReturnCorrect)
{
	const char input[] = "void main(){return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionCallCorrect)
{
	const char input[] =
	    "int foo(int i, float f, string s, bool b) {} void main(){int i; float "
	    "f; string s; bool b; b = false; i = 1; f = 2.0; s = \"three\"; foo(i, "
	    "f, s, b);}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionReturnAssigmentCorrect)
{
	const char input[] =
	    "float foo(){return 1.0;}void main(){float a; a = foo();}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleIfTypeCheckCorrect)
{
	const char input[] = "main(){bool a; a = true; if (a){return;}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexIfTypeCheckCorrectI)
{
	const char input[] = "main(){if (3 < 1){return;}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexIfTypeCheckCorrectII)
{
	const char input[] = "main(){if (false || true){return;}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, NegationBoolCorrect)
{
	const char input[] = "main(){bool b = false; b = !b;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, NegationIntCorrect)
{
	const char input[] = "main(){int a = 3; a = -a;}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_function_def *cur_function;
	auto symbol_table_visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);
	mCc_ast_visit_program(prog, &symbol_table_visitor);
	auto type_checking_visitor =
	    mCc_ast_type_checking_visitor(&cur_function, error_manager);
	mCc_ast_visit_program(prog, &type_checking_visitor);

	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}