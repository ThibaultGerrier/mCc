#include "mCc/ast_type_checking.h"
#include "mCc/ast_print.h"

static void check_statement_expression_type(
    struct mCc_ast_expression *expression, enum mCc_ast_type expected_type,
    const char *statement_str, struct mCc_ast_function_def *cur_function,
    struct mCc_err_error_manager *error_manager);

void binary_operation_error(enum mCc_ast_type lhs, enum mCc_ast_type rhs,
                            enum mCc_ast_binary_op op, size_t line_nm,
                            size_t col_nm, size_t line_nm_end,
                            size_t col_nm_end,
                            struct mCc_err_error_manager *error_manager)
{
	char msg[256];
	sprintf(msg,
	        "error: the type of the binary expression '%s' '%s' '%s' in line %lu, "
	        "col: %lu is not allowed",
	        mCc_ast_print_type(lhs), mCc_ast_print_binary_op(op),
	        mCc_ast_print_type(rhs), line_nm, col_nm);

	struct mCc_err_error_entry *entry =
	    mCc_err_new_error_entry(msg, line_nm, col_nm, line_nm_end, col_nm_end);
	mCc_err_error_manager_insert_error_entry(error_manager, entry);
}

void unary_operation_error(enum mCc_ast_type rhs, enum mCc_ast_unary_op op,
                           size_t line_nm, size_t col_nm, size_t line_nm_end,
                           size_t col_nm_end,
                           struct mCc_err_error_manager *error_manager)
{
	char msg[256];
	sprintf(msg,
	        "error: the type of the unary expression '%s' '%s' in line %lu, "
	        "col: %lu is not allowed",
	        mCc_ast_print_unary_op(op), mCc_ast_print_type(rhs), line_nm,
	        col_nm);

	struct mCc_err_error_entry *entry =
	    mCc_err_new_error_entry(msg, line_nm, col_nm, line_nm_end, col_nm_end);
	mCc_err_error_manager_insert_error_entry(error_manager, entry);
}

static enum mCc_ast_type
resolve_binary_expression(struct mCc_ast_expression *expression,
                          struct mCc_ast_function_def *cur_function,
                          struct mCc_err_error_manager *error_manager);

static enum mCc_ast_type
resolve_expression(struct mCc_ast_expression *expression,
                   struct mCc_ast_function_def *cur_function,
                   struct mCc_err_error_manager *error_manager)
{

	if (expression == NULL) {
		return MCC_AST_TYPE_VOID;
	}
	size_t line_nm = expression->node.sloc.start_line;
	size_t col_nm = expression->node.sloc.start_col;
	size_t line_nm_end = expression->node.sloc.end_line;
	size_t col_nm_end = expression->node.sloc.end_col;
	enum mCc_ast_type lhs_type;
	enum mCc_ast_type rhs_type;
	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		return mCc_ast_literal_type_convert(expression->literal->type);
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		return expression->identifier->symbol_table_entry->data_type;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER:
		check_statement_expression_type(expression->array_identifier.expression,
		                                MCC_AST_TYPE_INT, "array access",
		                                cur_function, error_manager);
		return expression->array_identifier.identifier->symbol_table_entry
		    ->data_type;
	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		return MCC_AST_TYPE_VOID;
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		rhs_type = resolve_expression(expression->unary_op.rhs, cur_function,
		                              error_manager);
		if ((expression->unary_op.op == MCC_AST_UNARY_OP_NOT &&
		     rhs_type != MCC_AST_TYPE_BOOL) ||
		    (expression->unary_op.op == MCC_AST_UNARY_OP_MINUS &&
		     !(rhs_type == MCC_AST_TYPE_INT ||
		       rhs_type == MCC_AST_TYPE_FLOAT))) {
			unary_operation_error(rhs_type, expression->unary_op.op, line_nm,
			                      col_nm, line_nm_end, col_nm_end,
			                      error_manager);
		}
		return rhs_type;
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		return resolve_binary_expression(expression, cur_function,
		                                 error_manager);

		return MCC_AST_TYPE_VOID;
	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		return resolve_expression(expression->expression, cur_function,
		                          error_manager);
	}
	return MCC_AST_TYPE_VOID;
}

static enum mCc_ast_type
resolve_binary_expression(struct mCc_ast_expression *expression,
                          struct mCc_ast_function_def *cur_function,
                          struct mCc_err_error_manager *error_manager)
{

	size_t line_nm = expression->node.sloc.start_line;
	size_t col_nm = expression->node.sloc.start_col;
	size_t line_nm_end = expression->node.sloc.end_line;
	size_t col_nm_end = expression->node.sloc.end_col;
	enum mCc_ast_type lhs_type = resolve_expression(
	    expression->binary_op.lhs, cur_function, error_manager);
	enum mCc_ast_type rhs_type = resolve_expression(
	    expression->binary_op.rhs, cur_function, error_manager);
	enum mCc_ast_binary_op op = expression->binary_op.op;

	bool equals_comparison =
	    op == MCC_AST_BINARY_OP_EQUALS || op == MCC_AST_BINARY_OP_NOT_EQUALS;
	bool bool_comparison =
	    op == MCC_AST_BINARY_OP_AND || op == MCC_AST_BINARY_OP_OR;
	bool num_comparison = op == MCC_AST_BINARY_OP_LESS ||
	                      op == MCC_AST_BINARY_OP_LESS_EQUALS ||
	                      op == MCC_AST_BINARY_OP_GREATER ||
	                      op == MCC_AST_BINARY_OP_GREATER_EQUALS;
	bool op_calc = op == MCC_AST_BINARY_OP_ADD || op == MCC_AST_BINARY_OP_SUB ||
	               op == MCC_AST_BINARY_OP_MUL || op == MCC_AST_BINARY_OP_DIV;
	bool typesEqual = lhs_type == rhs_type;
	bool bothNum = typesEqual && (lhs_type == MCC_AST_TYPE_INT ||
	                              lhs_type == MCC_AST_TYPE_FLOAT);
	bool bothBool = typesEqual && lhs_type == MCC_AST_TYPE_BOOL;

	if (!(bool_comparison && bothBool) && !(num_comparison && bothNum) &&
	    !(op_calc && bothNum) && !(typesEqual && equals_comparison)) {
		binary_operation_error(lhs_type, rhs_type, op, line_nm, col_nm,
		                       line_nm_end, col_nm_end, error_manager);
	}
	if ((num_comparison && bothNum) || (typesEqual && equals_comparison))
		return MCC_AST_TYPE_BOOL;
	return lhs_type;
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
		        "error: the type of the %s statement in line %lu, "
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
		                   *((struct mCc_ast_function_def **)data), error_manager);
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
		    *((struct mCc_ast_function_def **)data), error_manager);
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
		    *((struct mCc_ast_function_def **)data), error_manager);
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
		    *((struct mCc_ast_function_def **)data);

		enum mCc_ast_type cond_type = resolve_expression(
		    statement->expression, function_def, error_manager);
		if (cond_type != function_def->return_type && error_manager != NULL) {
			char msg[256];
			size_t line_nm = statement->node.sloc.start_line;
			size_t col_nm = statement->node.sloc.start_col;
			size_t line_nm_end = statement->node.sloc.end_line;
			size_t col_nm_end = statement->node.sloc.end_col;
			sprintf(msg,
			        "error: the type of the %s statement in line %lu, "
			        "col: %lu is not %s but %s",
			        mCc_ast_print_statement(statement->type), line_nm, col_nm,
			        mCc_ast_print_type(function_def->return_type),
			        mCc_ast_print_type(cond_type));

			struct mCc_err_error_entry *entry = mCc_err_new_error_entry(
			    msg, line_nm, col_nm, line_nm_end, col_nm_end);
			mCc_err_error_manager_insert_error_entry(error_manager, entry);
		}
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
		    *((struct mCc_ast_function_def **)data);
		enum mCc_ast_type lhs_type =
		    statement->assignment->identifier->symbol_table_entry->data_type;
		struct mCc_ast_expression *rhs_expr;
		if (statement->assignment->type == MCC_AST_ASSIGNMENT_TYPE_ARRAY) {
			check_statement_expression_type(
			    statement->assignment->array_ass.index, MCC_AST_TYPE_INT,
			    "array ass index", function_def, error_manager);
			rhs_expr = statement->assignment->array_ass.rhs;
		} else {
			rhs_expr = statement->assignment->normal_ass.rhs;
		}
		check_statement_expression_type(
		    rhs_expr, lhs_type, mCc_ast_print_statement(statement->type),
		    *((struct mCc_ast_function_def **)data), error_manager);
	}
}

static void
type_checking_function_def(struct mCc_ast_function_def *function_def,
                           enum mCc_ast_visit_type visit_type,
                           struct mCc_err_error_manager *error_manager,
                           void *data)
{
	if (visit_type == MCC_AST_VISIT_BEFORE) {
		*((struct mCc_ast_function_def **)data) = function_def;
	}
}

struct mCc_ast_visitor
mCc_ast_type_checking_visitor(struct mCc_ast_function_def **cur_function,
                              struct mCc_err_error_manager *error_manager)
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
