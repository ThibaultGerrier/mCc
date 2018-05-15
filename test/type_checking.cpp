
#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/ast_type_checking.h"
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexMathCorrect)
{
	const char input[] = "void main(){int a; a = 123; int b; b = 321; int c; c "
	                     "= a; (a + b) * c;}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionReturnAssigmentCorrect)
{
	const char input[] =
	    "float foo(){return 1.0;}void main(){float a; a = foo() + 3.0;}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionChainReturnAssigmentCorrect)
{
	const char input[] = "float foo(){return 1.0;} int bar(float in){return "
	                     "42;} void main(){int a; a = bar(foo());}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleIfTypeCheckCorrectI)
{
	const char input[] = "void main(){bool a; a = true; if (a){return;}}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexIfTypeCheckCorrectII)
{
	const char input[] = "void main(){if (3 < 1){return;}}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexIfTypeCheckCorrectIII)
{
	const char input[] = "void main(){if (false || true){return;}}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, NegationBoolCorrect)
{
	const char input[] = "void main(){bool b; b = false; b = !b;}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, NegationIntCorrect)
{
	const char input[] = "void main(){int a; a = 3; a = -a;}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleAssignmentIncorrect)
{
	const char input[] = "void main(){int a; a = 1.0;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the ass statement in line 1, col: "
	                    "24 is not int but float",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleArrayAssignmentInorrect)
{
	const char input[] = "void main(){string[3] b; b[0] = 123;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the ass[] statement in line 1, "
	                    "col: 33 is not string but int",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ArrayAssignmentInorrectI)
{
	const char input[] =
	    "void main(){string[3] a; float b; b = 15.33; a[0] = b;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the ass[] statement in line 1, "
	                    "col: 53 is not string but float",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ArrayAssignmentInorrectII)
{
	const char input[] = "void main(){string[3] a; float b; b = 15.33; a = b;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the ass statement in line 1, "
	                    "col: 50 is not string but float",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleAdditionInorrect)
{
	const char input[] =
	    "void main(){int a; a = 123; float b; b = 3.21; a + b;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the binary expression 'int' '+' "
	                    "'float' in line 1, col: 48 is not allowed",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexMathInorrect)
{
	const char input[] =
	    "void main(){int a; a = 123; bool b; b = true; int c; c "
	    "= a; (a * c) / b;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the binary expression 'int' '/' "
	                    "'bool' in line 1, col: 61 is not allowed",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, VoidReturnInorrect)
{
	const char input[] = "void main(){return 1;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the return statement in line 1, "
	                    "col: 13 is not void but int",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, NegationIncorrect)
{
	const char input[] = "void main(){string a; a = \"foo\"; a = -a;}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the unary expression '-' 'string' "
	                    "in line 1, col: 38 is not allowed",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleIfTypeCheckIncorrectI)
{
	const char input[] = "void main(){int a; a = 34; if (a){return;}}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the if statement in line 1, col: "
	                    "32 is not bool but int",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, ComplexIfTypeCheckIncorrectII)
{
	const char input[] = "void main(){if ((3 < 1) || \"foo\"){return;}}";
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

	ASSERT_EQ(1u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the binary expression 'bool' '||' "
	                    "'string' in line 1, col: 17 is not allowed",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, TwoIncorrect)
{
	const char input[] = "void main(){if (false || 3.14){return true;}}";
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

	ASSERT_EQ(2u, error_manager->used);
	ASSERT_EQ(0, strcmp("error: the type of the binary expression 'bool' '||' "
	                    "'float' in line 1, col: 17 is not allowed",
	                    error_manager->array[0]->msg));
	ASSERT_EQ(0, strcmp("error: the type of the return statement in line 1, "
	                    "col: 32 is not void but bool",
	                    error_manager->array[1]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionArrayParamCorrect)
{
	const char input[] =
	    "float foo(int a, string b, float[3] c) {return c[0] + c[1];} void "
	    "main() { float[3] arr; foo(1, \"bar\", arr); }";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionArrayParamInorrect)
{
	const char input[] =
	    "int foo(int a, string b, float[3] c) {return c[0] + c[1];} void "
	    "main() { float[3] arr; foo(1, \"bar\", arr); }";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error: the type of the return statement in line 1, "
	                    "col: 39 is not int but float",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionArrayParamAssigmentCorrect)
{
	const char input[] =
	    "float foo(int a, string b, float[3] c) {return c[0] + c[1];} void "
	    "main() { float[3] arr; float a; a = foo(1, \"bar\", arr); }";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionCallInorrect)
{
	const char input[] =
	    "int foo(int i, float f, string s, bool b) {} void main(){int i; float "
	    "f; string s; bool b; b = false; i = 1; f = 2.0; s = \"three\"; foo(i, "
	    "f, b, s);}";
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

	ASSERT_EQ(2u, error_manager->used);

	ASSERT_EQ(
	    0, strcmp("error: in function 'foo' the 3rd argument expr should be of "
	              "type 'string' but is of type 'bool' in line 1, col: 132",
	              error_manager->array[0]->msg));
	ASSERT_EQ(
	    0, strcmp("error: in function 'foo' the 4th argument expr should be of "
	              "type 'bool' but is of type 'string' in line 1, col: 132",
	              error_manager->array[1]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionReturnAssigmentInorrect)
{
	const char input[] =
	    "int foo(){return 1;}void main(){float a; a = foo() + 3.0;}";
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

	ASSERT_EQ(2u, error_manager->used);

	ASSERT_EQ(0, strcmp("error: the type of the binary expression 'int' '+' "
	                    "'float' in line 1, col: 46 is not allowed",
	                    error_manager->array[0]->msg));
	ASSERT_EQ(0, strcmp("error: the type of the ass statement in line 1, col: "
	                    "46 is not float but int",
	                    error_manager->array[1]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, SimpleFunctionReturnAssigmentInorrect)
{
	const char input[] = "int foo(){return 1;}void main(){float a; a = foo();}";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error: the type of the ass statement in line 1, col: "
	                    "46 is not float but int",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionChainReturnAssigmentIncorrect)
{
	const char input[] = "int foo(){return 1;} int bar(float in){return "
	                     "42;} void main(){int a; a = bar(foo());}";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0,
	          strcmp("error: in function 'bar' the 1st argument expr should be "
	                 "of type 'float' but is of type 'int' in line 1, col: 75",
	                 error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionCallFewArgumentsInorrect)
{
	const char input[] =
	    "int foo(int i, float f, string s, bool b) {} void main(){int i; float "
	    "f; string s; bool b; b = false; i = 1; f = 2.0; s = \"three\"; foo(i, "
	    "f, s);}";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error: in function 'foo' the call argument count is "
	                    "not equal to the function definition parameter count: "
	                    "3 != 4 in line 1, col: 132",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, FunctionCallTooManyArgumentsInorrect)
{
	const char input[] =
	    "int foo(int i, float f, string s, bool b) {} void main(){int i; float "
	    "f; string s; bool b; b = false; i = 1; f = 2.0; s = \"three\"; foo(i, "
	    "f, s, b, b);}";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error: in function 'foo' the call argument count is "
	                    "not equal to the function definition parameter count: "
	                    "5 != 4 in line 1, col: 132",
	                    error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(TypeChecking, BuiltInCorret)
{
	const char input[] =
	    "void main(){int i; float f; string s; bool b; b = "
	    "false; i = 1; f = 2.0; s = \"three\"; print(s); print_nl(); "
	    "print_int(i); print_float(f); i = read_int(); f = read_float();}";
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

	const ::testing::TestInfo *const test_info =
	    ::testing::UnitTest::GetInstance()->current_test_info();
	print_all_errors(test_info->name(), error_manager);
	ASSERT_EQ(0u, error_manager->used);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(SymbolTable, FunctionAndVariableNameConflictIncorrect)
{
	const char input[] = "void foo(){} void main(){int foo; foo();}";
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

	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0,
	          strcmp("error: function 'foo', naming conflict with a varaible "
	                 "in this scope or unknown function, line: 1, col: 35",
	                 error_manager->array[0]->msg));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}
