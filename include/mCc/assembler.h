#ifndef MCC_ASSEMBLER_H
#define MCC_ASSEMBLER_H

#endif // MCC_ASSEMBLER_H
#include "../../src/uthash.h"
#include "mCc/ast.h"
#include "mCc/tac.h"
#include "mCc/parser.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_ass_function {
	char *name;
	int stack_size;
	struct mCc_ass_function_var *data;
	struct mCc_ass_str *next;
};
typedef struct mCc_ass_function *mCc_ass_function_node;

struct mCc_ass_function_var {
	char *name; // key
	int location;
	bool is_reference;
	bool needs_free;
	UT_hash_handle hh;
};
typedef struct mCc_ass_function_var *mCc_ass_function_var_node;

struct mCc_ass_static_data {
	char *name; // key
	int label;
	bool is_string;
	UT_hash_handle hh;
};
typedef struct mCc_ass_static_data *mCc_ass_static_data_node;

typedef struct mCc_ass_list {
	void *data;
	struct mCc_ass_list *next;
} mCc_ass_List;

struct mCc_ass {
	// List *strings;
	mCc_ass_static_data_node strings;
	mCc_ass_static_data_node floats;
	mCc_ass_List *function_data;
};

void mCc_ass_print_assembler_program(struct mCc_ast_program *, FILE *);
void mCc_ass_print_assembler(struct mCc_tac *, FILE *);

#ifdef __cplusplus
}
#endif
