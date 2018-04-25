#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/symbol_table.h"
#include <cstdbool>

TEST(SymbolTable, Existing_Entry)
{
	struct mCc_sym_table_entry *table = nullptr;

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	ASSERT_EQ(true, mCc_sym_table_add_entry(&table, entry));
	entry = mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_ARRAY,
	                                MCC_AST_TYPE_STRING);

	ASSERT_EQ(false, mCc_sym_table_add_entry(&table, entry));

	mCc_sym_table_delete_symbol_table(&table);
	mCc_sym_table_delete_entry(entry);
}

TEST(SymbolTable, Lookup_Entry)
{
	struct mCc_sym_table_entry *table = nullptr;

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	ASSERT_EQ(true, mCc_sym_table_add_entry(&table, entry));
	struct mCc_sym_table_entry *result_entry =
	    mCc_sym_table_lookup_entry(table, "var1");
	ASSERT_NE(nullptr, result_entry);
	ASSERT_EQ(result_entry->data_type, MCC_AST_TYPE_INT);
	result_entry = mCc_sym_table_lookup_entry(table, "var2");
	ASSERT_EQ(nullptr, result_entry);

	mCc_sym_table_delete_symbol_table(&table);
}

TEST(SymbolTable, Lookup_Ascendant_Entry)
{
	struct mCc_sym_table_tree *tree_grandparent = nullptr;
	struct mCc_sym_table_tree *tree_parent = nullptr;
	struct mCc_sym_table_tree *tree_uncle = nullptr;
	struct mCc_sym_table_tree *tree = nullptr;

	tree_grandparent = mCc_sym_table_new_tree(nullptr);
	tree_parent = mCc_sym_table_new_tree(nullptr);
	tree_uncle = mCc_sym_table_new_tree(nullptr);
	tree = mCc_sym_table_new_tree(nullptr);
	mCc_sym_table_add_child(tree_grandparent, tree_parent);
	mCc_sym_table_add_child(tree_grandparent, tree_uncle);
	mCc_sym_table_add_child(tree_parent, tree);

	struct mCc_sym_table_entry *entry =
	    mCc_sym_table_new_entry("var1", 3, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_parent =
	    mCc_sym_table_new_entry("var2", 2, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_uncle =
	    mCc_sym_table_new_entry("var3", 4, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	struct mCc_sym_table_entry *entry_grandparent =
	    mCc_sym_table_new_entry("var4", 1, MCC_SYM_TABLE_VAR, MCC_AST_TYPE_INT);

	mCc_sym_table_add_entry(&tree->symbol_table, entry);
	mCc_sym_table_add_entry(&tree_parent->symbol_table, entry_parent);
	mCc_sym_table_add_entry(&tree_uncle->symbol_table, entry_uncle);
	mCc_sym_table_add_entry(&tree_grandparent->symbol_table, entry_grandparent);

	ASSERT_EQ(entry, mCc_sym_table_ascendant_tree_lookup_entry(tree, "var1"));
	ASSERT_EQ(entry_parent,
	          mCc_sym_table_ascendant_tree_lookup_entry(tree, "var2"));
	ASSERT_EQ(entry_grandparent,
	          mCc_sym_table_ascendant_tree_lookup_entry(tree, "var4"));
	ASSERT_EQ(nullptr, mCc_sym_table_ascendant_tree_lookup_entry(tree, "var3"));
	mCc_sym_table_delete_tree(tree_grandparent);
}
