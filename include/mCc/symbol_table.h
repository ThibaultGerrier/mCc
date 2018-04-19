#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include "ast.h"
#include "stddef.h"
#include "uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_sym_table_entry_type {
	MCC_SYM_TABLE_VAR,
	MCC_SYM_TABLE_ARRAY,
};

struct mCc_sym_table_entry {
	enum mCc_sym_table_entry_type var_type;
	enum mCc_ast_type data_type;
	size_t array_size;

	char *name;        /* key of the hashmap */
	UT_hash_handle hh; /* makes this structure hashable */
};

struct mCc_sym_table_tree {
	struct mCc_sym_table_entry *symbol_table;
	struct mCc_sym_table_tree *parent;
	struct mCc_sym_table_tree *next_sibling;
	struct mCc_sym_table_tree *first_child;
};

struct mCc_sym_table_entry *
mCc_sym_table_new_entry(char *name, enum mCc_sym_table_entry_type var_type,
                        enum mCc_ast_type data_type);

struct mCc_sym_table_entry *
mCc_sym_table_new_entry_array(char *name,
                              enum mCc_sym_table_entry_type var_type,
                              enum mCc_ast_type data_type, size_t size);

struct mCc_sym_table_tree *
mCc_sym_table_new_tree(struct mCc_sym_table_entry *entry);

void mCc_sym_table_delete_entry(struct mCc_sym_table_entry *entry);

void mCc_sym_table_delete_symbol_table(struct mCc_sym_table_entry *entry);

void mCc_sym_table_delete_tree(struct mCc_sym_table_tree *tree);

void mCc_sym_table_add_child(struct mCc_sym_table_tree *tree,
                             struct mCc_sym_table_tree *child);

#ifdef __cplusplus
}
#endif

#endif
