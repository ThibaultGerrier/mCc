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
	MCC_SYM_TABLE_FUNCTION,
};

struct mCc_sym_table_entry {
	enum mCc_sym_table_entry_type var_type;
	enum mCc_ast_type data_type;
	size_t array_size;
	size_t scope_index;

	// back reference for function call expression, NULL if not a function
	struct mCc_ast_function_def *function_def;

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
mCc_sym_table_new_entry(const char *name, size_t scope_index,
                        enum mCc_sym_table_entry_type var_type,
                        enum mCc_ast_type data_type);

struct mCc_sym_table_entry *
mCc_sym_table_new_entry_array(const char *name, size_t scope_index,
                              enum mCc_sym_table_entry_type var_type,
                              enum mCc_ast_type data_type, size_t size);

struct mCc_sym_table_tree *
mCc_sym_table_new_tree(struct mCc_sym_table_entry *entry);

void mCc_sym_table_delete_entry(struct mCc_sym_table_entry *entry);

void mCc_sym_table_delete_symbol_table(struct mCc_sym_table_entry **table);

void mCc_sym_table_delete_tree_recursive(struct mCc_sym_table_tree *tree);

//deletes a symbol table including the built-in functions
void mCc_sym_table_delete_tree(struct mCc_sym_table_tree *tree);

/**
 * returns true if the entry didn't exist otherwise false
 */
bool mCc_sym_table_add_entry(struct mCc_sym_table_entry **table,
                             struct mCc_sym_table_entry *entry);

/**
 * returns NULL if the key doesn't exist otherwise the found entry
 */
struct mCc_sym_table_entry *
mCc_sym_table_lookup_entry(struct mCc_sym_table_entry *table, const char *key);

struct mCc_sym_table_entry *
mCc_sym_table_ascendant_tree_lookup_entry(struct mCc_sym_table_tree *tree,
                                          const char *key);

void mCc_sym_table_add_child(struct mCc_sym_table_tree *tree,
                             struct mCc_sym_table_tree *child);

#ifdef __cplusplus
}
#endif

#endif
