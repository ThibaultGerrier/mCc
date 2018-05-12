#include "mCc/ast_type_checking.h"
#include "mCc/ast_print.h"

static enum mCc_ast_type
resolve_expression(struct mCc_ast_expression *expression,
                   struct mCc_ast_function_def *cur_function,
                   struct mCc_err_error_manager *error_manager)
{
	if (expression == NULL)
		return MCC_AST_TYPE_VOID;
	return MCC_AST_TYPE_VOID;
}

static void check_statement_expression_type(
    struct mCc_ast_expression *expression, enum mCc_ast_type expected_type,
    const char *statement_str, struct mCc_ast_function_def *cur_function,
    struct mCc_err_error_manager *error_manager)
{
	enum mCc_ast_type cond_type =
	    resolve_expression(expression, cur_function, error_manager);
	if (cond_type != expected_type && error_manager != NULL) {
		char msg[256];
		size_t line_nm = expression->node.sloc.start_line;
		size_t col_nm = expression->node.sloc.start_col;
		size_t line_nm_end = expression->node.sloc.end_line;
		size_t col_nm_end = expression->node.sloc.end_col;
		sprintf(msg,
		        "error the type of the %s statement in line %lu, "
		        "col: %lu is not %s but %s",
		        statement_str, line_nm, col_nm,
		        mCc_ast_print_type(expected_type),
		        mCc_ast_print_type(cond_type));

		struct mCc_err_error_entry *entry = mCc_err_new_error_entry(
		    msg, line_nm, col_nm, line_nm_end, col_nm_end);
		mCc_err_error_manager_insert_error_entry(error_manager, entry);
	}
}

static void type_checking_statement_expression(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		assert(statement);
		assert(data);
		resolve_expression(statement->expression,
		                   (struct mCc_ast_function_def *)data, error_manager);
	}
}

static void type_checking_statement_while(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		assert(statement);
		assert(data);
		check_statement_expression_type(
		    statement->while_condition, MCC_AST_TYPE_BOOL,
		    mCc_ast_print_statement(statement->type),
		    (struct mCc_ast_function_def *)data, error_manager);
	}
}

static void type_checking_statement_if(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		assert(statement);
		assert(data);
		check_statement_expression_type(
		    statement->if_condition, MCC_AST_TYPE_BOOL,
		    mCc_ast_print_statement(statement->type),
		    (struct mCc_ast_function_def *)data, error_manager);
	}
}

static void type_checking_statement_return(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		assert(statement);
		assert(data);
		struct mCc_ast_function_def *function_def =
		    (struct mCc_ast_function_def *)data;
		check_statement_expression_type(
		    statement->expression, function_def->return_type,
		    mCc_ast_print_statement(statement->type),
		    (struct mCc_ast_function_def *)data, error_manager);
	}
}

static void type_checking_statement_assignment(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		assert(statement);
		assert(statement->assignment);
		assert(data);
		struct mCc_ast_function_def *function_def =
		    (struct mCc_ast_function_def *)data;
		enum mCc_ast_type lhs_type =
		    statement->assignment->identifier->symbol_table_entry->data_type;
		struct mCc_ast_expression* rhs_expr;
		if (statement->assignment->type == MCC_AST_ASSIGNMENT_TYPE_ARRAY) {
			check_statement_expression_type(
			    statement->assignment->array_ass.index, MCC_AST_TYPE_INT,
			    "array ass index", function_def, error_manager);
			rhs_expr = statement->assignment->array_ass.rhs;
		} else {
			rhs_expr = statement->assignment->normal_ass.rhs;
		}
		check_statement_expression_type(
		    rhs_expr, lhs_type,
		    mCc_ast_print_statement(statement->type),
		    (struct mCc_ast_function_def *)data, error_manager);
	}
}

static void type_checking_function_def(
    struct mCc_ast_function_def *function_def, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		data = &function_def;
	}
}

struct mCc_ast_visitor
mCc_ast_type_checking_visitor(struct mCc_ast_function_def** cur_function, struct mCc_err_error_manager *error_manager)
{
	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_AND_POST_ORDER,

		.error_manager = error_manager,

		.userdata = cur_function,

		.program = NULL,

		.type = NULL,
		.function_def_list = NULL,
		.function_def = type_checking_function_def,
		.parameter = NULL,
		.argument_list = NULL,
		.declaration = NULL,

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
		.statement_if = type_checking_statement_if,
		.statement_while = type_checking_statement_while,
		.statement_return = type_checking_statement_return,
		.statement_assignment = type_checking_statement_assignment,
		.statement_declaration = NULL,
		.statement_expression = type_checking_statement_expression,
		.statement_compound_stmt = NULL,

		.identifier = NULL,

		.literal_bool = NULL,
		.literal_int = NULL,
		.literal_float = NULL,
		.literal_string = NULL,
	};
}
