#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"
#include "mCc/tac_print.h"
#include "mCc/parser.h"


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
                mCc_parser_print_error(result.parse_error);
            }
            return EXIT_FAILURE;
        }
        program = result.program;
        stmt = result.statement;
        expr = result.expression;
    }

    if (program != NULL) {
        //mCc_tac_node tac=mCc_ast_get_tac(program);
        //mCc_tac_print_dot(stdout,tac);
        //mCc_ast_delete_program(program);
        //mCc_tac_delete_tac(tac);
    } else if (decl != NULL) {
        //mCc_ast_print_dot_declaration(stdout, decl);
        mCc_ast_delete_declaration(decl);
    } else if (stmt != NULL) {
        //mCc_ast_print_dot_statement(stdout, stmt);
        mCc_ast_delete_statement(stmt);
    } else if (expr != NULL) {
        //mCc_ast_print_dot_expression(stdout, expr);
        mCc_ast_delete_expression(expr);
    }

    /* mCc_ast_delete_expression(expr); */

    return EXIT_SUCCESS;
}
