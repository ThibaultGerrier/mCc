#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/ast_function_return_check.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"
#include <cstdbool>
#include <string>

void print_all_errors(std::string prefix,
                      struct mCc_err_error_manager *error_manager)
{
	for (size_t i = 0; i < error_manager->used; i++) {
		std::cerr << prefix << ": " << error_manager->array[i]->msg
		          << std::endl;
	}
}

TEST(ReturnCheck, voidFunctionCorrectSingle)
{
	const char input[] = "void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, voidFunctionCorrectSingleWithReturn)
{
	const char input[] = "void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, voidFunctionCorrectMultiple)
{
	const char input[] =
	    "void foo(){} void bar(){float number;} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingle)
{
	const char input[] = "int foo() {return 1;} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectMultiple)
{
	const char input[] = "int foo() {return 1;} float bar() {float b; return "
	                     "b;} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingleIf)
{
	const char input[] = "int foo() {if(true){return 1;}} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingleIfElse)
{
	const char input[] =
	    "int foo() {if(true){return 1;}else{return 2;}} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingleIfElseMultipleI)
{
	const char input[] = "int foo() {if(true){return 1;}else{if(true){return "
	                     "1;}}} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingleIfElseMultipleII)
{
	const char input[] = "int foo() {if(true){return 1;}else{if(true){return "
	                     "1;}else{return 3;}}} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionCorrectSingleWhile)
{
	const char input[] = "int foo(){while(true){int a; return a;} return 1;} "
	                     "void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionMultipleCorrect)
{
	const char input[] = "int foo(){while(true){return 1;} return 2;} float "
	                     "bar(int b){if(b){return "
	                     "1;}else{} return 1.0;}void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionIncorrectSingleWithReturnVoid)
{
	const char input[] = "int foo(){return;} void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 1: no return in a non void function: 'foo'",
	        error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionIncorrectIf)
{
	const char input[] = "int foo(){if(true){}} void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 1: no return in a non void function: 'foo'",
	        error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionIncorrectIfelse)
{
	const char input[] =
	    "int foo(){if(true){return 1;}else{}} void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 1: no return in a non void function: 'foo'",
	        error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionIncorrectWhile)
{
	const char input[] =
	    "int foo(){while(true){return 1;}} void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 1: no return in a non void function: 'foo'",
	        error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, nonVoidFunctionMultipleIncorrect)
{
	const char input[] =
	    "int foo(){while(true){return 1;}} float bar(int b){if(b){return "
	    "1;}else{}}void main(){int a; return;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(2u, error_manager->used);

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 1: no return in a non void function: 'foo'",
	        error_manager->array[0]->msg));

	ASSERT_EQ(
	    0,
	    strcmp(
	        "error in line 1, col: 35: no return in a non void function: 'bar'",
	        error_manager->array[1]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, FunctionCorrectFunctionNonVoidReturnII)
{
	const char input[] =
	    "int ackermann(int m, int n){if (m == 0) {return n + 1;}if (n == 0) "
	    "{return ackermann(m - 1, 1);}return ackermann(m - 1, ackermann(m, n - "
	    "1));} void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfReturnsWithoutCompoundNonVoidCorrect)
{
	const char input[] =
	    "int ackermann(int m, int n){if(m < 2) return 1;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfReturnsWithoutCompoundNonVoidInorrect)
{
	const char input[] =
	    "int ackermann(int m, int n){if(m < 2) int a;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error in line 1, col: 1: no return in a non void "
	                    "function: 'ackermann'",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfElseReturnsWithoutCompoundNonVoidCorrect)
{
	const char input[] = "int ackermann(int m, int n){if(m < 2) return 1; else "
	                     "return 2;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfElseReturnsWithoutCompoundNonVoidInorrect)
{
	const char input[] = "int ackermann(int m, int n){if(m < 2) return 1; else "
	                     "int a;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error in line 1, col: 1: no return in a non void "
	                    "function: 'ackermann'",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfReturnsWithoutCompoundVoidCorrect)
{
	const char input[] =
	    "void ackermann(int m, int n){if(m < 2) return;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfReturnsWithAndWithoutCompoundVoidCorrect)
{
	const char input[] = "void ackermann(int m, int n){if(m < 2) {return;} "
	                     "else return;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithCompoundNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1){return false;} else if (n == "
	    "0){return true;} else {return is_even(n-2);}}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutCompoundIfNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else {return is_even(n-2);}}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutCompoundElseNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0){return true;} else return is_even(n-2);}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutCompoundNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else return is_even(n-2);}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutCompoundNonVoidIfInorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)int a; else return is_even(n-2);}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error in line 1, col: 1: no return in a non void "
	                    "function: 'is_even'",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutCompoundNonVoidElseInorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else int a;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error in line 1, col: 1: no return in a non void "
	                    "function: 'is_even'",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseIfelseIfWithCompundNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else {if (n==2) return true;}}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseIfelseIfWithoutCompundNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else if (n==2) return true;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseIfelseIfelseIfWithoutCompundNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else if (n==2) return true; else if (n == 3) return "
	    "false;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseIfelseIfelseIfWithoutCompundNonVoidInorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1)return false; else if (n == "
	    "0)return true; else if (n==2) return true; else if (n == 3) int "
	    "a;}void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error in line 1, col: 1: no return in a non void "
	                    "function: 'is_even'",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutReturnNonVoidCorrect)
{
	const char input[] =
	    "bool is_even(int n) {if (n == 1) {1+3;} return true;} void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(ReturnCheck, IfelseWithoutReturnNoCompoundNonVoidCorrect)
{
	const char input[] =
	    "bool foo(int n) {if (n == 1) 1 + 3; return true;} void main() {}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = mCc_ast_symbol_table_visitor(&visitor_data, nullptr);

	mCc_ast_visit_program(prog, &visitor);

	mCc_ast_function_return_checks(prog, error_manager);

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}