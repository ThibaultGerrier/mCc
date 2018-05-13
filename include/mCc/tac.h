//
// Created by philipp on 12.04.18.
//

#ifndef MCC_TAC_H
#define MCC_TAC_H

#endif // MCC_TAC_H
#include "mCc/ast.h"
#include "mCc/parser.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif



struct mCc_tac_var {
    int num;
    enum mCc_ast_type type;
    int array;
};

struct mCc_tac_op {
	enum mCc_ast_binary_op op;
	enum mCc_ast_type type;
};

enum mCc_tac_line_type {
	TAC_LINE_TYPE_SIMPLE,
	TAC_LINE_TYPE_DOUBLE,
	TAC_LINE_TYPE_SPECIAL,
};

struct mCc_tac {
	char *arg0;
	char *arg1;
	char *arg2;
	char *op;
	enum mCc_tac_line_type type;
	struct mCc_tac *next;
};
typedef struct mCc_tac *mCc_tac_node;

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *result);

void mCc_tac_cgen_statement_list(struct mCc_ast_statement_list *, mCc_tac_node);

#ifdef __cplusplus
}
#endif
