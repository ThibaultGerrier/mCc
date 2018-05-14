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
    char* val;
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
    TAC_LINE_TYPE_POPARRAY,
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
            char* name;
        } type_call;

        struct {
            char* variable_name;
            enum mCc_ast_type variable_type;
        } type_push;

        struct {
            char* variable_name;
        } type_return;

        struct {
            int label_name;
        } type_label;

        struct {
            int jump_name;
        } type_jump;

        struct {
            char* func_name;
        } type_labelfunc;

        struct {
            char* func_name;
        } type_labelfunc_end;

        struct {
            enum mCc_ast_type variable_type;
            char* variable_name;
        } type_pop;

        struct {
            enum mCc_ast_type variable_type;
            int size;
            char* variable_name;
        } type_pop_array;

        struct {
            char* arr_name;
            enum mCc_ast_type variable_type;
            int size;
        } type_decl_array;

        struct {
            char* arr_name;
            enum mCc_ast_type variable_type;
            char* location;
            char* assigned_variable_name;
        } type_assign_array;

        struct {
            int jump_label_name;
            char* condition_variable;
        } type_ifz;

        struct {
            char* string;
        } type_special;

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
            char* arg0_name;
            char* arr_name;
            char* location;
            enum mCc_ast_type arr_type;
        } type_array_iden;

    };
    struct mCc_tac *next;
};

typedef struct mCc_tac *mCc_tac_node;

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *result);

void mCc_tac_cgen_statement_list(struct mCc_ast_statement_list *, mCc_tac_node);

void mCc_tac_print_tac(mCc_tac_node head, FILE *out);

void mCc_tac_delete_tac(mCc_tac_node head);

mCc_tac_node mCc_tac_cgen_function_def_list(
        struct mCc_ast_function_def_list *function_def_list, mCc_tac_node tac);



#ifdef __cplusplus
}
#endif