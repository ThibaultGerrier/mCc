#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast_symbol_table.h"
#include "mCc/symbol_table.h"

void ast_symbol_table_stack_push(
    struct mCc_ast_symbol_table_stack_entry **stack,
    struct mCc_ast_symbol_table_stack_entry *s)
{
	assert(stack);
	assert(s);
	s->s = *stack;
	*stack = s;
}

struct mCc_ast_symbol_table_stack_entry *
ast_symbol_table_stack_pop(struct mCc_ast_symbol_table_stack_entry **stack)
{
	assert(stack);
	struct mCc_ast_symbol_table_stack_entry *s = *stack;
	*stack = (*stack)->s;
	return s;
}

struct mCc_ast_symbol_table_stack_entry *
ast_symbol_table_new_stack_entry(struct mCc_ast_symbol_table_stack_entry *s,
                                 struct mCc_sym_table_tree *symbol_table_tree,
                                 size_t cur_index)
{
	struct mCc_ast_symbol_table_stack_entry *stack_entry =
	    malloc(sizeof(stack_entry));
	stack_entry->s = s;
	stack_entry->symbol_table_tree = symbol_table_tree;
	stack_entry->cur_index = cur_index;
	return stack_entry;
}

static void symbol_table_program(struct mCc_ast_program *program,
                                 enum mCc_ast_visit_type visit_type, void *data)
{
	assert(program);
	assert(data);
	struct mCc_ast_symbol_table_visitor_data *visit_data = data;

	if (visit_type == MCC_AST_VISIT_BEFORE) {
		visit_data->symbol_table_tree = mCc_sym_table_new_tree(NULL);
		visit_data->stack = ast_symbol_table_new_stack_entry(NULL, visit_data->symbol_table_tree, 0);
	} else if (visit_type == MCC_AST_VISIT_AFTER) {
		free(ast_symbol_table_stack_pop(&visit_data->stack));
	}
}

static void
symbol_table_statement_compound_stmt(struct mCc_ast_statement *statement,
                                     enum mCc_ast_visit_type visit_type,
                                     void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_COMPOUND_STMT);

	struct mCc_ast_symbol_table_visitor_data *visit_data = data;

	if (visit_type == MCC_AST_VISIT_BEFORE) {
		visit_data->max_index++;

		struct mCc_ast_symbol_table_stack_entry *stack_entry =
		    ast_symbol_table_new_stack_entry(NULL, NULL, 0);

		struct mCc_sym_table_tree *child = mCc_sym_table_new_tree(NULL);

		stack_entry->symbol_table_tree = child;
		stack_entry->cur_index = visit_data->max_index;

		if (visit_data->stack != NULL) {
			mCc_sym_table_add_child(visit_data->stack->symbol_table_tree,
			                        child);
			ast_symbol_table_stack_push(&visit_data->stack, stack_entry);
		} else {
			visit_data->stack = stack_entry;
		}

	} else if (visit_type == MCC_AST_VISIT_AFTER) {
		free(ast_symbol_table_stack_pop(&visit_data->stack));
	}
}

static void symbol_table_declaration(struct mCc_ast_declaration *declaration,
                                     enum mCc_ast_visit_type visit_type,
                                     void *data)
{
	assert(declaration);
	assert(data);
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		struct mCc_ast_symbol_table_visitor_data *visit_data = data;
		if (declaration->declaration_type ==
		    MCC_AST_DECLARATION_TYPE_DECLARATION) {
			struct mCc_sym_table_entry *result = mCc_sym_table_lookup_entry(
			    visit_data->stack->symbol_table_tree->symbol_table,
			    declaration->normal_decl.identifier->name);
			if (result == NULL) {
				mCc_sym_table_add_entry(
				    &visit_data->stack->symbol_table_tree->symbol_table,
				    mCc_sym_table_new_entry(
				        declaration->normal_decl.identifier->name,
				        visit_data->stack->cur_index, MCC_SYM_TABLE_VAR,
				        declaration->identifier_type));
			} else {
				// TODO do not directly print into stderr
				fprintf(stderr, "error: redefinition");
			}
		} else if (declaration->declaration_type ==
		           MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION) {
			struct mCc_sym_table_entry *result = mCc_sym_table_lookup_entry(
			    visit_data->stack->symbol_table_tree->symbol_table,
			    declaration->array_decl.identifier->name);
			if (result == NULL) {
				mCc_sym_table_add_entry(
				    &visit_data->stack->symbol_table_tree->symbol_table,
				    mCc_sym_table_new_entry_array(
				        declaration->array_decl.identifier->name,
				        visit_data->stack->cur_index, MCC_SYM_TABLE_ARRAY,
				        declaration->identifier_type,
				        declaration->array_decl.literal->i_value));
			} else {
				// TODO do not directly print into stderr
				fprintf(stderr, "error: redefinition");
			}
		}
	}
}

static void symbol_table_identifier(struct mCc_ast_identifier *identifier,
                                    enum mCc_ast_visit_type visit_type,
                                    void *data)
{
	assert(identifier);
	assert(data);
	struct mCc_ast_symbol_table_visitor_data *visit_data = data;
	struct mCc_sym_table_entry *result =
	    mCc_sym_table_ascendant_tree_lookup_entry(
	        visit_data->stack->symbol_table_tree, identifier->name);
	if (result != NULL) {
		identifier->symbol_table_entry = result;
	} else {
		// TODO do not directly print into stderr
		fprintf(stderr, "error: not defined identifier");
	}
}

struct mCc_ast_visitor
symbol_table_visitor(struct mCc_ast_symbol_table_visitor_data *visit_data)
{
	assert(visit_data);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_AND_POST_ORDER,

		.userdata = visit_data,

		.program = symbol_table_program,

		.type = NULL,
		.function_def_list = NULL,
		.function_def = NULL,
		.parameter = NULL,
		.argument_list = NULL,
		.declaration = symbol_table_declaration,

		.expression_identifier = NULL,
		.expression_array_identifier = NULL,
		.expression_call_expr = NULL,
		.expression_literal = NULL,
		.expression_binary_op = NULL,
		.expression_unary_op = NULL,
		.expression_parenth = NULL,

		.statement = NULL,
		.statement_list = NULL,
		.statement_if = NULL,
		.statement_while = NULL,
		.statement_return = NULL,
		.statement_assignment = NULL,
		.statement_declaration = NULL,
		.statement_expression = NULL,
		.statement_compound_stmt = symbol_table_statement_compound_stmt,

		.function_identifier = NULL,
		.identifier = symbol_table_identifier,

		.literal_bool = NULL,
		.literal_int = NULL,
		.literal_float = NULL,
		.literal_string = NULL,
	};
}
