#include "mCc/symbol_table.h"
#include "uthash.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct mCc_sym_table_entry *
mCc_sym_table_new_entry(char *name, enum mCc_sym_table_entry_type var_type,
                        enum mCc_ast_type data_type)
{
	assert(name);
	struct mCc_sym_table_entry *entry = malloc(sizeof(*entry));
	assert(entry);
	entry->var_type = var_type;
	entry->data_type = data_type;
	entry->name = malloc((strlen(name) + 1) * sizeof(char));
	assert(entry->name);
	strcpy(entry->name, name);
	return entry;
}

struct mCc_sym_table_entry *
mCc_sym_table_new_entry_array(char *name,
                              enum mCc_sym_table_entry_type var_type,
                              enum mCc_ast_type data_type, size_t array_size)
{
	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry(name, var_type, data_type);
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
	return tree;
}

void mCc_sym_table_delete_entry(struct mCc_sym_table_entry *entry)
{
	assert(entry);
	assert(entry->name);
	free(entry->name);
	free(entry);
}

void mCc_sym_table_delete_symbol_table(struct mCc_sym_table_entry *table)
{
	struct mCc_sym_table_entry *current_entry, *tmp;

	HASH_ITER(hh, table, current_entry, tmp)
	{
		HASH_DEL(table, current_entry);
		mCc_sym_table_delete_entry(current_entry);
	}
}

void mCc_sym_table_delete_tree(struct mCc_sym_table_tree *tree)
{
	assert(tree);
	if (tree->symbol_table != NULL) {
		mCc_sym_table_delete_symbol_table(tree->symbol_table);
	}
	if (tree->first_child != NULL) {
		mCc_sym_table_delete_tree(tree->first_child);
	}
	if (tree->next_sibling != NULL) {
		mCc_sym_table_delete_tree(tree->next_sibling);
	}
}

void mCc_sym_table_add_child(struct mCc_sym_table_tree *tree,
                             struct mCc_sym_table_tree *child)
{
	assert(tree);
	assert(child);
	if (tree->first_child == NULL) {
		tree->first_child = child;
		return;
	}
	struct mCc_sym_table_tree* cur_tree = tree->first_child;
	while (cur_tree->next_sibling != NULL) {
		cur_tree = cur_tree->next_sibling;
	}
	cur_tree->next_sibling = child;
}
