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
    TAC_LINE_TYPE_CALL,
    TAC_LINE_TYPE_POP,
    TAC_LINE_TYPE_POPARRAY,
    TAC_LINE_TYPE_PUSH,
    TAC_LINE_TYPE_RETURN,
    TAC_LINE_TYPE_VOID,
    TAC_LINE_TYPE_IFZ,
    TAC_LINE_TYPE_DECL_ARRAY,
    TAC_LINE_TYPE_ASSIGNMENT_ARRAY,
    TAC_LINE_TYPE_LABEL,
    TAC_LINE_TYPE_JUMP,
    TAC_LINE_TYPE_LABELFUNC,
    TAC_LINE_TYPE_LABELFUNC_END,
};

struct mCc_tac {
    char *arg0;
    char *arg1;
    char *arg2;
    char *op;
    int array_size;
    enum mCc_ast_literal_type literal_type;
    enum mCc_tac_line_type type;
    struct mCc_tac *next;
};
typedef struct mCc_tac *mCc_tac_node;

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *result);

void mCc_tac_cgen_statement_list(struct mCc_ast_statement_list *, mCc_tac_node);

void mCc_tac_print_tac(mCc_tac_node head, FILE *out);

void mCc_tac_delete_tac(mCc_tac_node head);

mCc_tac_node mCc_ast_get_tac_program(struct mCc_ast_program *program);



#ifdef __cplusplus
}
#endif
