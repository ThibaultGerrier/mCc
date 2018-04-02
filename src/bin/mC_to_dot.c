#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"
#include "mCc/ast_print.h"
#include "mCc/parser.h"

void print_error(struct mCc_parse_error parse_error) {
	struct mCc_error_location location = parse_error.location;
	char line[10], column[10];
	if (location.first_line != location.last_line) {
		sprintf(line, "%d-%d", location.first_line, location.last_line );
	} else {
		sprintf(line, "%d", location.first_line);
	}
	if (location.first_column != location.last_column) {
		sprintf(column, "%d-%d", location.first_column, location.last_column );
	} else {
		sprintf(column, "%d", location.first_column);
	}
	fprintf(stderr, "Error (line %s, column %s): %s\n", line, column, parse_error.msg);
}

int main(void)
{
	struct mCc_ast_program *program = NULL;
	struct mCc_ast_statement *stmt = NULL;
	struct mCc_ast_declaration *decl = NULL;
	struct mCc_ast_expression *expr = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(stdin);
		if (result.status != MCC_PARSER_STATUS_OK) {
            if (result.parse_error.is_error) {
                print_error(result.parse_error);
            }
			return EXIT_FAILURE;
		}
		program = result.program;
		stmt = result.statement;
		expr = result.expression;
	}

	if (program != NULL) {
		printf("progr\n");
		mCc_ast_print_dot_program(stdout, program);
		mCc_ast_delete_program(program);
	} else if (decl != NULL) {
		printf("decl\n");
		mCc_ast_print_dot_declaration(stdout, decl);
		mCc_ast_delete_declaration(decl);
	} else if (stmt != NULL) {
		printf("statment\n");
		mCc_ast_print_dot_statement(stdout, stmt);
		mCc_ast_delete_statement(stmt);
	} else if (expr != NULL) {
		mCc_ast_print_dot_expression(stdout, expr);
		mCc_ast_delete_expression(expr);
	}

	/* mCc_ast_delete_expression(expr); */

	return EXIT_SUCCESS;
}
