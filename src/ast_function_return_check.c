#include "mCc/ast_function_return_check.h"
#include <mCc/ast.h>
#include <mCc/error_manager.h>
#include <stdio.h>


void mCc_ast_check_function_void_compound (struct mCc_ast_statement *compound_stmt, struct mCc_err_error_manager *error_manager) {
    struct mCc_ast_statement_list *current_statement_list = compound_stmt->statement_list;
    if (current_statement_list == NULL) {
        return;
    }
    
    do {
        struct mCc_ast_statement *current_statement = current_statement_list->statement;
        if (current_statement->type == MCC_AST_STATEMENT_TYPE_RETURN && current_statement->expression != NULL) {
            if (error_manager != NULL) {
                char msg[256];
                sprintf(msg,
                        "error in line %lu, col: %lu: return of a value in a void function",
                        current_statement->node.sloc.start_line,
                        current_statement->node.sloc.start_col);
                mCc_err_error_manager_insert_error_entry(
                        error_manager,
                        mCc_err_new_error_entry(msg, current_statement->node.sloc.start_line,
                                                current_statement->node.sloc.start_col,
                                                current_statement->node.sloc.end_line,
                                                current_statement->node.sloc.end_col));
            }
        } else if (current_statement->type == MCC_AST_STATEMENT_TYPE_IF) {
            mCc_ast_check_function_void_compound(current_statement->if_branch, error_manager);
            // if there is no else branch there can be no return statement
            if (current_statement->else_branch != NULL) {
                mCc_ast_check_function_void_compound(current_statement->else_branch, error_manager);
            }
        } else if (current_statement->type == MCC_AST_STATEMENT_TYPE_WHILE) {
            mCc_ast_check_function_void_compound(current_statement->body, error_manager);
        }

    } while ((current_statement_list = current_statement_list->next) != NULL);
}

// returns false if there is no return statement in a function body
bool mCc_ast_check_function_return_compound(struct mCc_ast_statement *compound_stmt, struct mCc_err_error_manager *error_manager) {
    bool out = false;
    struct mCc_ast_statement_list *current_statement_list = compound_stmt->statement_list;
    if (current_statement_list == NULL) {
        return out;
    }

    do {
        struct mCc_ast_statement *current_statement = current_statement_list->statement;
        if (current_statement->type == MCC_AST_STATEMENT_TYPE_RETURN && current_statement->expression != NULL) {
            return true;
        } else if (current_statement->type == MCC_AST_STATEMENT_TYPE_IF) {
            out =  out || mCc_ast_check_function_return_compound(current_statement->if_branch, error_manager);
            // if there is no else branch there can be no return statement
            if (current_statement->else_branch != NULL) {
                out =  out && mCc_ast_check_function_return_compound(current_statement->else_branch, error_manager);
            }
        }
    } while ((current_statement_list = current_statement_list->next) != NULL);

    return out;
}


void mCc_ast_check_function(struct mCc_ast_function_def *function_def, struct mCc_err_error_manager *error_manager) {
    if (function_def->return_type == MCC_AST_TYPE_VOID) { // check for absence of return statement
        // if there is no compound statement there can be no return statement
        if (function_def->compound_stmt != NULL) {
            mCc_ast_check_function_void_compound(function_def->compound_stmt, error_manager);
        }
    } else { // check for presence  of return statement
        if (!mCc_ast_check_function_return_compound(function_def->compound_stmt, error_manager)) {
            if (error_manager != NULL) {
                char msg[256];
                sprintf(msg,
                        "error in line %lu, col: %lu: no return in a non void function: '%s'",
                        function_def->node.sloc.start_line,
                        function_def->node.sloc.start_col,function_def->function_identifier->name);
                mCc_err_error_manager_insert_error_entry(
                        error_manager,
                        mCc_err_new_error_entry(msg, function_def->node.sloc.start_line,
                                                function_def->node.sloc.start_col,
                                                function_def->node.sloc.end_line,
                                                function_def->node.sloc.end_col));
            }
        }
    }
}

void mCc_ast_function_return_checks(struct mCc_ast_program *program, struct mCc_err_error_manager *error_manager) {
    assert(program);

    struct mCc_ast_function_def_list *current_function_def_list = program->function_def_list;
    struct mCc_ast_function_def *function_def = current_function_def_list->function_def;

    while (current_function_def_list != NULL) {
        mCc_ast_check_function(current_function_def_list->function_def, error_manager);
        current_function_def_list = current_function_def_list->next;
    }
}
