#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mCc/ast_function_return_check.h>

#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_type_checking.h"
#include "mCc/error_manager.h"
#include "mCc/parser.h"
#include "mCc/tac.h"

void cleanup_and_exit(struct mCc_parser_result *parser_result,
                      struct mCc_sym_table_tree *symbol_table_tree,
                      struct mCc_err_error_manager *error_manager,
                      int exit_status)
{
	mCc_parser_delete_result(parser_result);
	mCc_sym_table_delete_tree(symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
	exit(exit_status);
}

void print_usage(const char *prg)
{
	printf("usage: %s <FILE>\n\n", prg);
	printf("  <FILE>        Input filepath or - for stdin\n");
}

void print_all_errors(struct mCc_err_error_manager *error_manager)
{
	for (size_t i = 0; i < error_manager->used; i++) {
		printf("%s\n", error_manager->array[i]->msg);
	}
}

void print_errors_and_exit_if_errors(
    struct mCc_parser_result *parser_result,
    struct mCc_sym_table_tree *symbol_table_tree,
    struct mCc_err_error_manager *error_manager)
{
	assert(error_manager);
	if (error_manager->used != 0) {
		print_all_errors(error_manager);
		cleanup_and_exit(parser_result, symbol_table_tree, error_manager,
		                 EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	/* determine input source */
	FILE *in;
	if (strcmp("-", argv[1]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[1], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}
	struct mCc_parser_result result;
	struct mCc_ast_program *program = NULL;
	struct mCc_sym_table_tree *symbol_table_tree = NULL;
	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_tac *tac = NULL;

	/* parsing phase */
	{
		result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status != MCC_PARSER_STATUS_OK) {
			return EXIT_FAILURE;
		}
		program = result.program;
	}
	/* symbol table creation phase */
	{
		struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL,
			                                                      0 };
		struct mCc_ast_visitor symbol_table_visitor =
		    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);

		mCc_ast_visit_program(program, &symbol_table_visitor);

		symbol_table_tree = visitor_data.symbol_table_tree;

		print_errors_and_exit_if_errors(&result, symbol_table_tree,
		                                error_manager);
	}
	/* return check  phase */
	{
		mCc_ast_function_return_checks(program, error_manager);

		print_errors_and_exit_if_errors(&result, symbol_table_tree,
										error_manager);
	}
	/* type checking phase */
	{
		// temporary for each function scope check (return mostly)
		struct mCc_ast_function_def *cur_function;
		struct mCc_ast_visitor type_checking_visitor =
		    mCc_ast_type_checking_visitor(&cur_function, error_manager);

		mCc_ast_visit_program(program, &type_checking_visitor);

		print_errors_and_exit_if_errors(&result, symbol_table_tree,
		                                error_manager);
	}

	/* Three address code generation phase */
	{
		tac = mCc_ast_get_tac(program);
		mCc_tac_print_tac(tac, stdout);
		mCc_tac_delete_tac(tac);
	}

	/*    TODO
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */

	/* cleanup */
	print_all_errors(error_manager);
	cleanup_and_exit(&result, symbol_table_tree, error_manager, EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
