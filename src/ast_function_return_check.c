#include "mCc/ast_function_return_check.h"
#include "mCc/ast.h"
#include "mCc/error_manager.h"
#include <stdio.h>

bool mCc_ast_check_function_return_compound(
    struct mCc_ast_statement *compound_stmt);

bool if_else_case(bool out, struct mCc_ast_statement *current_statement)
{
	// check if this is a if else
	if (current_statement->else_branch != NULL) {
		// check if there is a return statement or not
		if (current_statement->if_branch->type ==
		        MCC_AST_STATEMENT_TYPE_RETURN &&
		    current_statement->if_branch->expression != NULL) {
			out = true;
		} else {
			if (current_statement->if_branch->type ==
			    MCC_AST_STATEMENT_TYPE_COMPOUND_STMT) {
				out = mCc_ast_check_function_return_compound(
				    current_statement->if_branch);
			} else {
				return false;
			}
		}
		// check if there is a return statement or not in the else branch
		if (current_statement->else_branch->type ==
		        MCC_AST_STATEMENT_TYPE_RETURN &&
		    current_statement->else_branch->expression != NULL) {
			out = out && true; // could be simplified to just out

			// check if there is a compound statement
		} else if (current_statement->else_branch->type ==
		           MCC_AST_STATEMENT_TYPE_IF) {
			out = out && if_else_case(out, current_statement->else_branch);
			// check if there is a if statement
		} else if (current_statement->else_branch->type ==
		           MCC_AST_STATEMENT_TYPE_COMPOUND_STMT) {
			out = out && mCc_ast_check_function_return_compound(
			                 current_statement->else_branch);
		} else {
			return false;
		}
	} else {
		return false;
	}
	return out;
}

// returns false if there is no return statement in a function body
bool mCc_ast_check_function_return_compound(
    struct mCc_ast_statement *compound_stmt)
{
	bool out = false;
	struct mCc_ast_statement_list *current_statement_list =
	    compound_stmt->statement_list;
	if (current_statement_list == NULL) {
		return out;
	}

	do {
		struct mCc_ast_statement *current_statement =
		    current_statement_list->statement;
		if (current_statement->type == MCC_AST_STATEMENT_TYPE_RETURN &&
		    current_statement->expression != NULL) {
			return true;
		} else if (current_statement->type == MCC_AST_STATEMENT_TYPE_IF) {
			if (current_statement->else_branch != NULL) {
				out = out || if_else_case(out, current_statement);
			}
		} else if (current_statement->type ==
		           MCC_AST_STATEMENT_TYPE_COMPOUND_STMT) {
			out = out ||
			      mCc_ast_check_function_return_compound(current_statement);
		}
	} while ((current_statement_list = current_statement_list->next) != NULL);

	return out;
}

void mCc_ast_check_function(struct mCc_ast_function_def *function_def,
                            struct mCc_err_error_manager *error_manager)
{
	if (function_def->return_type !=
	    MCC_AST_TYPE_VOID) { // check for presence  of return statement
		if (!mCc_ast_check_function_return_compound(
		        function_def->compound_stmt)) {
			if (error_manager != NULL) {
				char msg[256];
				sprintf(msg,
				        "error in line %lu, col: %lu: no return in a non void "
				        "function: '%s'",
				        function_def->node.sloc.start_line,
				        function_def->node.sloc.start_col,
				        function_def->function_identifier->name);
				mCc_err_error_manager_insert_error_entry(
				    error_manager, mCc_err_new_error_entry(
				                       msg, function_def->node.sloc.start_line,
				                       function_def->node.sloc.start_col,
				                       function_def->node.sloc.end_line,
				                       function_def->node.sloc.end_col));
			}
		}
	}
}

void mCc_ast_function_return_checks(struct mCc_ast_program *program,
                                    struct mCc_err_error_manager *error_manager)
{
	assert(program);

	struct mCc_ast_function_def_list *current_function_def_list =
	    program->function_def_list;

	while (current_function_def_list != NULL) {
		mCc_ast_check_function(current_function_def_list->function_def,
		                       error_manager);
		current_function_def_list = current_function_def_list->next;
	}
}
