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
	    malloc(sizeof(*stack_entry));
	stack_entry->s = s;
	stack_entry->symbol_table_tree = symbol_table_tree;
	stack_entry->cur_index = cur_index;
	return stack_entry;
}

void symbol_table_add_built_in_functions(const struct mCc_ast_symbol_table_visitor_data *visit_data) {

    const char *a[6];
    a[0] = "print";
    a[1] = "print_nl";
    a[2] = "print_int";
    a[3] = "print_float";
    a[4] = "read_int";
    a[5] = "read_float";

    // TODO make sure this gets cleaned up (the new function def ast
    // elements) print
    struct mCc_sym_table_entry *new_entry_print =
		    mCc_sym_table_new_entry(a[0], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_VOID);

    struct mCc_ast_identifier *identifier_print =
		    mCc_ast_new_identifier(a[0]);
    struct mCc_ast_declaration *declaration_print = mCc_ast_new_declaration(
		    MCC_AST_TYPE_STRING, mCc_ast_new_identifier("msg"));
    struct mCc_ast_parameter *parameter_print =
		    mCc_ast_new_parameter(declaration_print);
    new_entry_print->function_def = mCc_ast_new_function_def(
		    MCC_AST_TYPE_VOID, identifier_print, parameter_print, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_print);

    // print_nl
    struct mCc_sym_table_entry *new_entry_print_nl =
		    mCc_sym_table_new_entry(a[1], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_VOID);

    struct mCc_ast_identifier *identifier_nl = mCc_ast_new_identifier(a[1]);
    new_entry_print_nl->function_def = mCc_ast_new_function_def(
		    MCC_AST_TYPE_VOID, identifier_nl, NULL, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_print_nl);

    // print_int
    struct mCc_sym_table_entry *new_entry_print_int =
		    mCc_sym_table_new_entry(a[2], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_VOID);

    struct mCc_ast_identifier *identifier_print_int =
		    mCc_ast_new_identifier(a[2]);
    struct mCc_ast_declaration *declaration_print_int =
		    mCc_ast_new_declaration(MCC_AST_TYPE_INT,
		                            mCc_ast_new_identifier("x"));
    struct mCc_ast_parameter *parameter_print_int =
		    mCc_ast_new_parameter(declaration_print_int);
    new_entry_print_int->function_def = mCc_ast_new_function_def(
		    MCC_AST_TYPE_VOID, identifier_print_int, parameter_print_int, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_print_int);

    // print_float
    struct mCc_sym_table_entry *new_entry_print_float =
		    mCc_sym_table_new_entry(a[3], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_VOID);

    struct mCc_ast_identifier *identifier_print_float =
		    mCc_ast_new_identifier(a[3]);
    struct mCc_ast_declaration *declaration_print_float =
		    mCc_ast_new_declaration(MCC_AST_TYPE_FLOAT,
		                            mCc_ast_new_identifier("x"));
    struct mCc_ast_parameter *parameter_print_float =
		    mCc_ast_new_parameter(declaration_print_float);
    new_entry_print_float->function_def =
		    mCc_ast_new_function_def(MCC_AST_TYPE_VOID, identifier_print_float,
		                             parameter_print_float, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_print_float);

    // read_int
    struct mCc_sym_table_entry *new_entry_read_int =
		    mCc_sym_table_new_entry(a[4], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_INT);
    struct mCc_ast_identifier *identifier_read_int =
		    mCc_ast_new_identifier(a[4]);
    new_entry_read_int->function_def = mCc_ast_new_function_def(
		    MCC_AST_TYPE_INT, identifier_read_int, NULL, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_read_int);

    // read_float
    struct mCc_sym_table_entry *new_entry_read_float =
		    mCc_sym_table_new_entry(a[5], visit_data->stack->cur_index,
		                            MCC_SYM_TABLE_FUNCTION, MCC_AST_TYPE_FLOAT);
    struct mCc_ast_identifier *identifier_read_float =
		    mCc_ast_new_identifier(a[5]);
    new_entry_read_float->function_def = mCc_ast_new_function_def(
		    MCC_AST_TYPE_FLOAT, identifier_read_float, NULL, NULL);
    mCc_sym_table_add_entry(
		    &visit_data->stack->symbol_table_tree->symbol_table,
		    new_entry_read_float);
}

static void symbol_table_program(struct mCc_ast_program *program,
                                 enum mCc_ast_visit_type visit_type,
                                 struct mCc_err_error_manager *error_manager,
                                 void *data)
{
    assert(program);
    assert(data);
    struct mCc_ast_symbol_table_visitor_data *visit_data = data;

    if (visit_type == MCC_AST_VISIT_BEFORE) {
        visit_data->symbol_table_tree = mCc_sym_table_new_tree(NULL);
        visit_data->stack = ast_symbol_table_new_stack_entry(
                NULL, visit_data->symbol_table_tree, 0);
        symbol_table_add_built_in_functions(visit_data);

    } else if (visit_type == MCC_AST_VISIT_AFTER) {
        free(ast_symbol_table_stack_pop(&visit_data->stack));

        // check if there is a main()
        struct mCc_sym_table_entry *result = mCc_sym_table_lookup_entry(
                visit_data->symbol_table_tree->symbol_table, "main");
        if (result == NULL) {
            char msg[256];
            sprintf(msg, "No main function in program");
            mCc_err_error_manager_insert_error_entry(
                    error_manager, mCc_err_new_error_entry(msg, 0, 0, 0, 0));
        }
    }
}

static void
symbol_table_function_identifier(struct mCc_ast_function_def *function_def,
                                 struct mCc_err_error_manager *error_manager,
                                 struct mCc_ast_symbol_table_stack_entry *stack)
{
	assert(function_def);
	assert(stack);

	struct mCc_ast_identifier *iden = function_def->function_identifier;

	struct mCc_sym_table_entry *result =
	    mCc_sym_table_ascendant_tree_lookup_entry(stack->symbol_table_tree,
	                                              iden->name);

	if (result == NULL) {
		struct mCc_sym_table_entry *entry = mCc_sym_table_new_entry(
		    iden->name, stack->cur_index, MCC_SYM_TABLE_FUNCTION,
		    function_def->return_type);
		entry->function_def = function_def;
		mCc_sym_table_add_entry(&stack->symbol_table_tree->symbol_table, entry);
	} else {
		if (error_manager != NULL) {
			char msg[256];
			sprintf(msg,
			        "error in line %lu, col: %lu: redefined function "
			        "'%s'",
			        iden->node.sloc.start_line, iden->node.sloc.start_col,
			        iden->name);
			mCc_err_error_manager_insert_error_entry(
			    error_manager,
			    mCc_err_new_error_entry(
			        msg, iden->node.sloc.start_line, iden->node.sloc.start_col,
			        iden->node.sloc.end_line, iden->node.sloc.end_col));
		}
	}
}

static void
symbol_table_function_def(struct mCc_ast_function_def *function_def,
                          enum mCc_ast_visit_type visit_type,
                          struct mCc_err_error_manager *error_manager,
                          void *data)
{
	assert(function_def);
	assert(data);

	struct mCc_ast_symbol_table_visitor_data *visit_data = data;

	if (visit_type == MCC_AST_VISIT_BEFORE) {
		symbol_table_function_identifier(function_def, error_manager,
		                                 visit_data->stack);
		visit_data->max_index++;

		struct mCc_ast_symbol_table_stack_entry *stack_entry =
		    ast_symbol_table_new_stack_entry(NULL, NULL, 0);

		struct mCc_sym_table_tree *child = mCc_sym_table_new_tree(NULL);

		stack_entry->symbol_table_tree = child;

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

static void symbol_table_statement_compound_stmt(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
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

static void symbol_table_declaration(
    struct mCc_ast_declaration *declaration, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
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
				if (error_manager != NULL) {
					char msg[256];
					sprintf(msg,
					        "error in line %lu, col: %lu: redefinition of "
					        "variable '%s'",
					        declaration->node.sloc.start_line,
					        declaration->node.sloc.start_col,
					        declaration->normal_decl.identifier->name);
					mCc_err_error_manager_insert_error_entry(
					    error_manager,
					    mCc_err_new_error_entry(
					        msg, declaration->node.sloc.start_line,
					        declaration->node.sloc.start_col,
					        declaration->node.sloc.end_line,
					        declaration->node.sloc.end_col));
				}
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
				        declaration->array_decl.literal
				            ->i_value)); // TODO check array value (should not
				                         // be negative)
			} else {
				if (error_manager != NULL) {
					char msg[256];
					sprintf(msg,
					        "error in line %lu, col: %lu: redefinition of "
					        "variable '%s'",
					        declaration->node.sloc.start_line,
					        declaration->node.sloc.start_col,
					        declaration->normal_decl.identifier->name);
					mCc_err_error_manager_insert_error_entry(
					    error_manager,
					    mCc_err_new_error_entry(
					        msg, declaration->node.sloc.start_line,
					        declaration->node.sloc.start_col,
					        declaration->node.sloc.end_line,
					        declaration->node.sloc.end_col));
				}
			}
		}
	}
}

static void symbol_table_identifier(struct mCc_ast_identifier *identifier,
                                    enum mCc_ast_visit_type visit_type,
                                    struct mCc_err_error_manager *error_manager,
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
		if (error_manager != NULL) {
			char msg[256];
			sprintf(msg,
			        "error in line %lu, col: %lu: undefined identifier: '%s'",
			        identifier->node.sloc.start_line,
			        identifier->node.sloc.start_col, identifier->name);
			mCc_err_error_manager_insert_error_entry(
			    error_manager,
			    mCc_err_new_error_entry(msg, identifier->node.sloc.start_line,
			                            identifier->node.sloc.start_col,
			                            identifier->node.sloc.end_line,
			                            identifier->node.sloc.end_col));
		}
	}
}

struct mCc_ast_visitor mCc_ast_symbol_table_visitor(
    struct mCc_ast_symbol_table_visitor_data *visit_data,
    struct mCc_err_error_manager *error_manager)
{
	assert(visit_data);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_AND_POST_ORDER,

		.error_manager = error_manager,

		.userdata = visit_data,

		.program = symbol_table_program,

		.type = NULL,
		.function_def_list = NULL,
		.function_def = symbol_table_function_def,
		.parameter = NULL,
		.argument_list = NULL,
		.declaration = symbol_table_declaration,

		.expression = NULL,
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

		.identifier = symbol_table_identifier,

		.literal_bool = NULL,
		.literal_int = NULL,
		.literal_float = NULL,
		.literal_string = NULL,
	};
}
