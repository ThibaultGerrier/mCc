#include "mCc/symbol_table.h"
#include "mCc/ast.h"
#include "uthash.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mCc_sym_table_entry *
mCc_sym_table_new_entry(const char *name, size_t scope_index,
                        enum mCc_sym_table_entry_type var_type,
                        enum mCc_ast_type data_type)
{
	assert(name);
	struct mCc_sym_table_entry *entry = malloc(sizeof(*entry));
	assert(entry);
	entry->var_type = var_type;
	entry->data_type = data_type;
	entry->scope_index = scope_index;
	entry->array_size = 0;
	entry->function_def = NULL;
	size_t len = strlen(name) + 1;
	entry->name = malloc(len);
	assert(entry->name);
	memcpy(entry->name, name, len);
	return entry;
}

struct mCc_sym_table_entry *
mCc_sym_table_new_entry_array(const char *name, size_t scope_index,
                              enum mCc_sym_table_entry_type var_type,
                              enum mCc_ast_type data_type, size_t array_size)
{
	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry(name, scope_index, var_type, data_type);
	entry->array_size = array_size;
	return entry;
}

struct mCc_sym_table_tree *
mCc_sym_table_new_tree(struct mCc_sym_table_entry *symbol_table)
{
	struct mCc_sym_table_tree *tree = malloc(sizeof(*tree));
	tree->symbol_table = symbol_table;
	tree->first_child = NULL;
	tree->next_sibling = NULL;
	tree->parent = NULL;
	return tree;
}

void mCc_sym_table_delete_entry(struct mCc_sym_table_entry *entry)
{
	assert(entry);
	assert(entry->name);
	free(entry->name);
	free(entry);
}

void mCc_sym_table_delete_symbol_table(struct mCc_sym_table_entry **table)
{
	struct mCc_sym_table_entry *current_entry, *tmp;

	HASH_ITER(hh, *table, current_entry, tmp)
	{
		HASH_DEL(*table, current_entry);
		mCc_sym_table_delete_entry(current_entry);
	}
	table = NULL;
}

void mCc_sym_table_delete_tree_recursive(struct mCc_sym_table_tree *tree)
{
	assert(tree);
	if (tree->symbol_table != NULL) {
		mCc_sym_table_delete_symbol_table(&tree->symbol_table);
	}
	if (tree->first_child != NULL) {
		mCc_sym_table_delete_tree_recursive(tree->first_child);
	}
	if (tree->next_sibling != NULL) {
		mCc_sym_table_delete_tree_recursive(tree->next_sibling);
	}
	free(tree);
}

void mCc_sym_table_delete_tree(struct mCc_sym_table_tree *tree)
{
	// clean up for built-in functions
	const char *a[6];
	a[0] = "print";
	a[1] = "print_nl";
	a[2] = "print_int";
	a[3] = "print_float";
	a[4] = "read_int";
	a[5] = "read_float";

	for (size_t i = 0; i < 6; i++) {
		struct mCc_sym_table_entry *entry = NULL;
		HASH_FIND_STR(tree->symbol_table, a[i], entry);
		assert(entry);
		mCc_ast_delete_function_def(entry->function_def);
	}

	assert(tree);
	mCc_sym_table_delete_tree_recursive(tree);
}

bool mCc_sym_table_add_entry(struct mCc_sym_table_entry **table,
                             struct mCc_sym_table_entry *entry)
{
	assert(entry);
	struct mCc_sym_table_entry *tmp_entry = NULL;
	HASH_FIND_STR(*table, entry->name, tmp_entry);
	if (tmp_entry == NULL) {
		HASH_ADD_STR(*table, name, entry);
		return true;
	}
	return false;
}

struct mCc_sym_table_entry *
mCc_sym_table_lookup_entry(struct mCc_sym_table_entry *table, const char *key)
{
	struct mCc_sym_table_entry *entry = NULL;
	HASH_FIND_STR(table, key, entry);
	return entry;
}

struct mCc_sym_table_entry *
mCc_sym_table_ascendant_tree_lookup_entry(struct mCc_sym_table_tree *tree,
                                          const char *key)
{
	assert(tree);
	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_lookup_entry(tree->symbol_table, key);
	if (entry != NULL) {
		return entry;
	}
	if (tree->parent == NULL) {
		return NULL;
	}
	return mCc_sym_table_ascendant_tree_lookup_entry(tree->parent, key);
}

void mCc_sym_table_add_child(struct mCc_sym_table_tree *tree,
                             struct mCc_sym_table_tree *child)
{
	assert(tree);
	assert(child);
	if (tree->first_child == NULL) {
		tree->first_child = child;
		child->parent = tree;
		return;
	}
	struct mCc_sym_table_tree *cur_tree = tree->first_child;
	while (cur_tree->next_sibling != NULL) {
		cur_tree = cur_tree->next_sibling;
	}
	cur_tree->next_sibling = child;
	child->parent = tree;
}
