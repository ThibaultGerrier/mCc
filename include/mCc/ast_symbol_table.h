#ifndef MCC_AST_SYMBOL_TABLE
#define MCC_AST_SYMBOL_TABLE
#include <assert.h>
#include <stddef.h>
#include "symbol_table.h"

struct mCc_ast_symbol_table_stack_entry {
	struct mCc_ast_symbol_table_stack_entry *s;
  struct mCc_sym_table_tree* symbol_table_tree;
  size_t cur_index;
};

struct mCc_ast_symbol_table_visitor_data {
  struct mCc_ast_symbol_table_stack_entry* stack;
  struct mCc_sym_table_tree* symbol_table_tree;
  size_t max_index;
};

#endif
