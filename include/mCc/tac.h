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
	enum mCc_ast_type type;
	int array;
	char *val;
};

struct mCc_tac_op {
	enum mCc_ast_binary_op op;
	enum mCc_ast_type type;
};

enum mCc_tac_line_type {
	TAC_LINE_TYPE_SIMPLE,
	TAC_LINE_TYPE_DOUBLE,
	TAC_LINE_TYPE_CALL,
	TAC_LINE_TYPE_POP,
	TAC_LINE_TYPE_PUSH,
	TAC_LINE_TYPE_RETURN,
	TAC_LINE_TYPE_IFZ,
	TAC_LINE_TYPE_DECL_ARRAY,
	TAC_LINE_TYPE_IDEN_ARRAY,
	TAC_LINE_TYPE_ASSIGNMENT_ARRAY,
	TAC_LINE_TYPE_LABEL,
	TAC_LINE_TYPE_JUMP,
	TAC_LINE_TYPE_LABELFUNC,
	TAC_LINE_TYPE_LABELFUNC_END,
	TAC_LINE_TYPE_BEGIN,
};

struct mCc_tac {
	enum mCc_tac_line_type type;

	union {

		struct {
			char *name;
		} type_call;

		struct {
			struct mCc_tac_var var;
		} type_push;

		struct {
			struct mCc_tac_var var;
		} type_return;

		struct {
			int label_name;
		} type_label;

		struct {
			int jump_name;
		} type_jump;

		struct {
			char *func_name;
		} type_label_func;

		struct {
			char *func_name;
		} type_label_func_end;

		struct {
			struct mCc_tac_var var;
		} type_pop;

		struct {
			struct mCc_tac_var var;
		} type_decl_array;

		struct {
			struct mCc_tac_var arr;
			struct mCc_tac_var var;
			struct mCc_tac_var loc;
		} type_assign_array;

		struct {
			int jump_label_name;
			struct mCc_tac_var var;
		} type_ifz;

		struct {
			struct mCc_tac_var arg0;
			struct mCc_tac_var arg1;
		} type_simple;

		struct {
			struct mCc_tac_var arg0;
			struct mCc_tac_var arg1;
			struct mCc_tac_var arg2;
			struct mCc_tac_op op;
		} type_double;

		struct {
			struct mCc_tac_var arr;
			struct mCc_tac_var loc;
			struct mCc_tac_var var;
		} type_array_iden;
	};
	struct mCc_tac *next;
};

typedef struct mCc_tac *mCc_tac_node;

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *result);
mCc_tac_node mCc_ast_get_tac_program(struct mCc_ast_program *);

void mCc_tac_cgen_statement_list(struct mCc_ast_statement_list *, mCc_tac_node);

void mCc_tac_print_tac(mCc_tac_node head, FILE *out);

void mCc_tac_delete_tac(mCc_tac_node head);

mCc_tac_node mCc_tac_cgen_function_def_list(
    struct mCc_ast_function_def_list *function_def_list, mCc_tac_node tac);

mCc_tac_node mCc_ast_get_tac_expression(struct mCc_ast_expression *);

mCc_tac_node mCc_ast_get_tac_statement(struct mCc_ast_statement *);

struct mCc_tac_var mCc_tac_cgen_literal(struct mCc_ast_literal *, mCc_tac_node);
struct mCc_tac_var mCc_tac_cgen_expression(struct mCc_ast_expression *,
                                           mCc_tac_node);

#ifdef __cplusplus
}
#endif