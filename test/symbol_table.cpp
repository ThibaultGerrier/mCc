#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"
#include <cstdbool>

TEST(SymbolTable, Existing_Entry)
{
	struct mCc_sym_table_entry *table = nullptr;

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	ASSERT_EQ(true, mCc_sym_table_add_entry(&table, entry));
	entry = mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_ARRAY,
	                                MCC_AST_TYPE_STRING);

	ASSERT_EQ(false, mCc_sym_table_add_entry(&table, entry));

	mCc_sym_table_delete_symbol_table(&table);
	mCc_sym_table_delete_entry(entry);
}

TEST(SymbolTable, Lookup_Entry)
{
	struct mCc_sym_table_entry *table = nullptr;

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	ASSERT_EQ(true, mCc_sym_table_add_entry(&table, entry));
	struct mCc_sym_table_entry *result_entry =
	    mCc_sym_table_lookup_entry(table, "var1");
	ASSERT_NE(nullptr, result_entry);
	ASSERT_EQ(result_entry->data_type, MCC_AST_TYPE_INT);
	result_entry = mCc_sym_table_lookup_entry(table, "var2");
	ASSERT_EQ(nullptr, result_entry);

	mCc_sym_table_delete_symbol_table(&table);
}

TEST(SymbolTable, Lookup_Ascendant_Entry)
{
	struct mCc_sym_table_tree *tree_grandparent = nullptr;
	struct mCc_sym_table_tree *tree_parent = nullptr;
	struct mCc_sym_table_tree *tree_uncle = nullptr;
	struct mCc_sym_table_tree *tree = nullptr;

	tree_grandparent = mCc_sym_table_new_tree(nullptr);
	tree_parent = mCc_sym_table_new_tree(nullptr);
	tree_uncle = mCc_sym_table_new_tree(nullptr);
	tree = mCc_sym_table_new_tree(nullptr);
	mCc_sym_table_add_child(tree_grandparent, tree_parent);
	mCc_sym_table_add_child(tree_grandparent, tree_uncle);
	mCc_sym_table_add_child(tree_parent, tree);

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 3, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_parent =
	    mCc_sym_table_new_entry("var2", 2, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_uncle =
	    mCc_sym_table_new_entry("var3", 4, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_grandparent =
	    mCc_sym_table_new_entry("var4", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	mCc_sym_table_add_entry(&tree->symbol_table, entry);
	mCc_sym_table_add_entry(&tree_parent->symbol_table, entry_parent);
	mCc_sym_table_add_entry(&tree_uncle->symbol_table, entry_uncle);
	mCc_sym_table_add_entry(&tree_grandparent->symbol_table, entry_grandparent);

	ASSERT_EQ(entry, mCc_sym_table_ascendant_tree_lookup_entry(tree, "var1"));
	ASSERT_EQ(entry_parent,
	          mCc_sym_table_ascendant_tree_lookup_entry(tree, "var2"));
	ASSERT_EQ(entry_grandparent,
	          mCc_sym_table_ascendant_tree_lookup_entry(tree, "var4"));
	ASSERT_EQ(nullptr, mCc_sym_table_ascendant_tree_lookup_entry(tree, "var3"));
	mCc_sym_table_delete_tree(tree_grandparent);
}

TEST(SymbolTable, Visitor_Program_Declaration)
{
	const char input[] = "void main(){int a; string[3] b;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	auto visitor = symbol_table_visitor(&visitor_data, NULL);

	mCc_ast_visit_program(prog, &visitor);

	auto entry_a = mCc_sym_table_lookup_entry(
	    visitor_data.symbol_table_tree->first_child->symbol_table, "a");
	ASSERT_NE(nullptr, entry_a);

	ASSERT_EQ(0, strcmp("a", entry_a->name));
	ASSERT_EQ(1u, entry_a->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_VAR, entry_a->var_type);
	ASSERT_EQ(MCC_AST_TYPE_INT, entry_a->data_type);

	auto entry_b = mCc_sym_table_lookup_entry(
	    visitor_data.symbol_table_tree->first_child->symbol_table, "b");
	ASSERT_NE(nullptr, entry_b);

	ASSERT_EQ(0, strcmp("b", entry_b->name));
	ASSERT_EQ(1u, entry_b->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_ARRAY, entry_b->var_type);
	ASSERT_EQ(3u, entry_b->array_size);
	ASSERT_EQ(MCC_AST_TYPE_STRING, entry_b->data_type);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
}

TEST(SymbolTable, Visitor_Program_MultiScopeShadowing)
{
	const char input[] = "void main(){int a; { string[3] a;}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	auto visitor = symbol_table_visitor(&visitor_data, NULL);

	mCc_ast_visit_program(prog, &visitor);

	auto entry_a = mCc_sym_table_lookup_entry(
	    visitor_data.symbol_table_tree->first_child->symbol_table, "a");
	ASSERT_NE(nullptr, entry_a);

	ASSERT_EQ(0, strcmp("a", entry_a->name));
	ASSERT_EQ(1u, entry_a->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_VAR, entry_a->var_type);
	ASSERT_EQ(MCC_AST_TYPE_INT, entry_a->data_type);

	auto entry_a2 = mCc_sym_table_lookup_entry(
	    visitor_data.symbol_table_tree->first_child->first_child->symbol_table,
	    "a");
	ASSERT_NE(nullptr, entry_a2);

	ASSERT_EQ(0, strcmp("a", entry_a2->name));
	ASSERT_EQ(2u, entry_a2->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_ARRAY, entry_a2->var_type);
	ASSERT_EQ(3u, entry_a2->array_size);
	ASSERT_EQ(MCC_AST_TYPE_STRING, entry_a2->data_type);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
}

TEST(SymbolTable, Visitor_Program_MultiScopeShadowingAssignment)
{
	const char input[] =
	    "void main(){int a; { string a; a = \"str\";} a = 123;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	auto visitor = symbol_table_visitor(&visitor_data, NULL);

	mCc_ast_visit_program(prog, &visitor);

	auto entry_a =
	    prog->function_def_list->function_def->compound_stmt->statement_list
	        ->next->next->statement->assignment->identifier->symbol_table_entry;
	ASSERT_NE(nullptr, entry_a);

	ASSERT_EQ(0, strcmp("a", entry_a->name));
	ASSERT_EQ(1u, entry_a->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_VAR, entry_a->var_type);
	ASSERT_EQ(MCC_AST_TYPE_INT, entry_a->data_type);

	auto entry_a2 = prog->function_def_list->function_def->compound_stmt
	                    ->statement_list->next->statement->statement_list->next
	                    ->statement->assignment->identifier->symbol_table_entry;
	ASSERT_NE(nullptr, entry_a2);

	ASSERT_EQ(0, strcmp("a", entry_a2->name));
	ASSERT_EQ(2u, entry_a2->scope_index);
	ASSERT_EQ(MCC_SYM_TABLE_VAR, entry_a2->var_type);
	ASSERT_EQ(MCC_AST_TYPE_STRING, entry_a2->data_type);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
}

TEST(SymbolTable, Visitor_Program_Redefinition)
{
	const char input[] =
	    "void main(){int a; string[3] a;\n\n  {int b; int b;}\n}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();

	auto visitor = symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(prog, &visitor);

	auto entry_a = mCc_sym_table_lookup_entry(
	    visitor_data.symbol_table_tree->first_child->symbol_table, "a");
	ASSERT_NE(nullptr, entry_a);

	ASSERT_EQ(0, strcmp("a", entry_a->name));
	ASSERT_EQ(MCC_SYM_TABLE_VAR, entry_a->var_type);
	ASSERT_EQ(MCC_AST_TYPE_INT, entry_a->data_type);
	// check the error message
	ASSERT_EQ(2u, error_manager->used);
	ASSERT_EQ(0,
	          strcmp("error in line 1, col: 20: redefinition of variable 'a'",
	                 error_manager->array[0]->msg));
	ASSERT_EQ(1u, error_manager->array[0]->start_line);
	ASSERT_EQ(20u, error_manager->array[0]->start_col);
	ASSERT_EQ(0,
	          strcmp("error in line 3, col: 11: redefinition of variable 'b'",
	                 error_manager->array[1]->msg));
	ASSERT_EQ(3u, error_manager->array[1]->start_line);
	ASSERT_EQ(11u, error_manager->array[1]->start_col);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(SymbolTable, Visitor_Function_Table)
{
	const char input[] = "int foo(int bar){return bar;}\nvoid main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	auto visitor = symbol_table_visitor(&visitor_data, NULL);

	mCc_ast_visit_program(prog, &visitor);

	auto function_table = visitor_data.symbol_table_tree->symbol_table;

	auto entry_foo = mCc_sym_table_lookup_entry(function_table, "foo");
	ASSERT_NE(nullptr, entry_foo);

	ASSERT_EQ(0, strcmp("foo", entry_foo->name));

	auto entry_main = mCc_sym_table_lookup_entry(function_table, "main");
	ASSERT_NE(nullptr, entry_main);

	ASSERT_EQ(0, strcmp("main", entry_main->name));

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
}

TEST(SymbolTable, Visitor_Function_Table_Undefined_Function)
{
	const char input[] = "void main(){int a; foo(a);}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(prog, &visitor);

	auto function_table = visitor_data.symbol_table_tree->symbol_table;

	auto entry_foo = mCc_sym_table_lookup_entry(function_table, "foo");
	ASSERT_EQ(nullptr, entry_foo);

	// check the error message
	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(
	    0, strcmp("error in line 1, col: 20: not defined function call 'foo'",
	              error_manager->array[0]->msg));
	ASSERT_EQ(1u, error_manager->array[0]->start_line);
	ASSERT_EQ(20u, error_manager->array[0]->start_col);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(SymbolTable, Visitor_Function_Table_Redefined_Function)
{
	const char input[] = "void foo(){} void foo(){} void main(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(prog, &visitor);

	auto function_table = visitor_data.symbol_table_tree->symbol_table;

	auto entry_foo = mCc_sym_table_lookup_entry(function_table, "foo");
	ASSERT_NE(nullptr, entry_foo);

	// check the error message
	ASSERT_EQ(1u, error_manager->used);

	ASSERT_EQ(0, strcmp("error in line 1, col: 19: redefined function 'foo'",
	                    error_manager->array[0]->msg));
	ASSERT_EQ(1u, error_manager->array[0]->start_line);
	ASSERT_EQ(19u, error_manager->array[0]->start_col);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(SymbolTable, Visitor_Function_Table_No_Main)
{
	const char input[] = "void foo(){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
		                                                      0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	auto visitor = symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(prog, &visitor);

	auto function_table = visitor_data.symbol_table_tree->symbol_table;

	auto entry_foo = mCc_sym_table_lookup_entry(function_table, "foo");
	ASSERT_NE(nullptr, entry_foo);

	// check the error message
	ASSERT_EQ(1u, error_manager->used);
	
	ASSERT_EQ(0, strcmp("No main function in program",
	                    error_manager->array[0]->msg));
	ASSERT_EQ(0u, error_manager->array[0]->start_line);
	ASSERT_EQ(0u, error_manager->array[0]->start_col);

	mCc_parser_delete_result(&result);
	mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
}

TEST(SymbolTable, Visitor_Function_Table_Built_In)
{
const char input[] = "void main(){print();print_nl();print_int();print_float();read_int();read_float();}";
auto result = mCc_parser_parse_string(input);

ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

auto prog = result.program;

struct mCc_ast_symbol_table_visitor_data visitor_data = { nullptr, nullptr,
                                                          0 };

struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
auto visitor = symbol_table_visitor(&visitor_data, error_manager);

mCc_ast_visit_program(prog, &visitor);

auto function_table = visitor_data.symbol_table_tree->symbol_table;

auto entry_print = mCc_sym_table_lookup_entry(function_table, "print");
ASSERT_NE(nullptr, entry_print);

auto entry_print_nl = mCc_sym_table_lookup_entry(function_table, "print_nl");
ASSERT_NE(nullptr, entry_print_nl);

auto entry_print_int = mCc_sym_table_lookup_entry(function_table, "print_int");
ASSERT_NE(nullptr, entry_print_int);

auto entry_print_float = mCc_sym_table_lookup_entry(function_table, "print_float");
ASSERT_NE(nullptr, entry_print_float);

auto entry_read_int = mCc_sym_table_lookup_entry(function_table, "read_int");
ASSERT_NE(nullptr, entry_read_int);

auto entry_read_float = mCc_sym_table_lookup_entry(function_table, "read_float");
ASSERT_NE(nullptr, entry_read_float);

// check the error message
ASSERT_EQ(0u, error_manager->used);

mCc_parser_delete_result(&result);
mCc_sym_table_delete_tree(visitor_data.symbol_table_tree);
mCc_err_delete_error_manager(error_manager);
}
