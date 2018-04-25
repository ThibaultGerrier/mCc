#include "mCc/ast_visit.h"

#include <assert.h>
#include <stdlib.h>

#define visit(node, callback, visitor, visit_type) \
	do { \
		if (callback) { \
			(callback)(node, visit_type, (visitor)->userdata); \
		} \
	} while (0)

#define visit_if(cond, node, callback, visitor, visit_type) \
	do { \
		if (cond) { \
			visit(node, callback, visitor, visit_type); \
		} \
	} while (0)

#define visit_if_pre_order(node, callback, visitor) \
	visit_if(((visitor)->order == MCC_AST_VISIT_PRE_ORDER || \
	          (visitor)->order == MCC_AST_VISIT_PRE_AND_POST_ORDER), \
	         node, callback, visitor, MCC_AST_VISIT_BEFORE)

#define visit_if_post_order(node, callback, visitor) \
	visit_if(((visitor)->order == MCC_AST_VISIT_POST_ORDER || \
	          (visitor)->order == MCC_AST_VISIT_PRE_AND_POST_ORDER), \
	         node, callback, visitor, MCC_AST_VISIT_BEFORE)

void mCc_ast_visit_program(struct mCc_ast_program *program,
                           struct mCc_ast_visitor *visitor)
{
	assert(program);
	assert(visitor);
	visit_if_pre_order(program, visitor->program, visitor);
	mCc_ast_visit_function_def_list(program->function_def_list, visitor);
	visit_if_post_order(program, visitor->program, visitor);
}

void mCc_ast_visit_function_def_list(
    struct mCc_ast_function_def_list *function_def_list,
    struct mCc_ast_visitor *visitor)
{
	assert(function_def_list);
	assert(visitor);

	struct mCc_ast_function_def_list *next = function_def_list;
	while (next != NULL) {
		visit_if_pre_order(next, visitor->function_def_list, visitor);
		mCc_ast_visit_function_def(next->function_def, visitor);
		visit_if_post_order(function_def_list, visitor->function_def_list,
		                    visitor);
		next = next->next;
	}
}

void mCc_ast_visit_function_def(struct mCc_ast_function_def *function_def,
                                struct mCc_ast_visitor *visitor)
{
	assert(function_def);
	assert(visitor);
	visit_if_pre_order(function_def, visitor->function_def, visitor);
	mCc_ast_visit_statement(function_def->compound_stmt, visitor);
	mCc_ast_visit_type(&function_def->return_type, visitor);
	mCc_ast_visit_identifier(function_def->function_identifier, visitor);
	mCc_ast_visit_parameters(function_def->parameters, visitor);
	visit_if_post_order(function_def, visitor->function_def, visitor);
}

void mCc_ast_visit_parameters(struct mCc_ast_parameter *parameters,
                              struct mCc_ast_visitor *visitor)
{
	assert(visitor);

	struct mCc_ast_parameter *next = parameters;
	while (next != NULL) {
		visit_if_pre_order(next, visitor->parameter, visitor);
		mCc_ast_visit_declaration(next->declaration, visitor);
		visit_if_post_order(next, visitor->parameter, visitor);
		next = next->next;
	}
}

void mCc_ast_visit_arguments(struct mCc_ast_argument_list *arguments,
                             struct mCc_ast_visitor *visitor)
{
	assert(arguments);
	assert(visitor);
	struct mCc_ast_argument_list *next = arguments;
	while (next != NULL) {
		visit_if_pre_order(next, visitor->argument_list, visitor);
		mCc_ast_visit_expression(next->expression, visitor);
		visit_if_post_order(arguments, visitor->argument_list, visitor);
		next = next->next;
	}
}

void mCc_ast_visit_declaration(struct mCc_ast_declaration *declaration,
                               struct mCc_ast_visitor *visitor)
{
	assert(declaration);
	assert(visitor);
	visit_if_pre_order(declaration, visitor->declaration, visitor);
	mCc_ast_visit_type(&declaration->identifier_type, visitor);
	switch (declaration->declaration_type) {
	case MCC_AST_DECLARATION_TYPE_DECLARATION:
		mCc_ast_visit_identifier(declaration->normal_decl.identifier, visitor);
		break;
	case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION:
		mCc_ast_visit_identifier(declaration->array_decl.identifier, visitor);
		mCc_ast_visit_literal(declaration->array_decl.literal, visitor);
		break;
	}
	visit_if_post_order(declaration, visitor->declaration, visitor);
}

void mCc_ast_visit_assignment(struct mCc_ast_assignment *assignment,
                              struct mCc_ast_visitor *visitor)
{
	assert(assignment);
	assert(visitor);
	visit_if_pre_order(assignment, visitor->assignment, visitor);
	switch (assignment->type) {
	case MCC_AST_ASSIGNMENT_TYPE_NORMAL:
		mCc_ast_visit_identifier(assignment->identifier, visitor);
		mCc_ast_visit_expression(assignment->normal_ass.rhs, visitor);
		break;
	case MCC_AST_ASSIGNMENT_TYPE_ARRAY:
		mCc_ast_visit_identifier(assignment->identifier, visitor);
		mCc_ast_visit_expression(assignment->array_ass.index, visitor);
		mCc_ast_visit_expression(assignment->array_ass.rhs, visitor);
		break;
	}
	visit_if_post_order(assignment, visitor->assignment, visitor);
}

void mCc_ast_visit_type(enum mCc_ast_type *type,
                        struct mCc_ast_visitor *visitor)
{
	assert(visitor);

	visit(type, visitor->type, visitor, MCC_VISIT_NO_TYPE);

	/* visit_if_post_order(type, visitor->type, visitor); */
	/* visit_if_pre_order(type, visitor->type, visitor); */
}

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		visit_if_pre_order(expression, visitor->expression_identifier, visitor);
		mCc_ast_visit_identifier(expression->identifier, visitor);
		visit_if_post_order(expression, visitor->expression_identifier,
		                    visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER:
		visit_if_pre_order(expression, visitor->expression_array_identifier,
		                   visitor);
		mCc_ast_visit_identifier(expression->identifier, visitor);
		mCc_ast_visit_expression(expression->array_identifier.expression,
		                         visitor);
		visit_if_post_order(expression, visitor->expression_array_identifier,
		                    visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		visit_if_pre_order(expression, visitor->expression_call_expr, visitor);
		mCc_ast_visit_identifier(expression->call_expr.identifier, visitor);
		if (expression->call_expr.arguments != NULL) {
			mCc_ast_visit_arguments(expression->call_expr.arguments, visitor);
		}
		visit_if_post_order(expression, visitor->expression_call_expr, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mCc_ast_visit_literal(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mCc_ast_visit_expression(expression->binary_op.lhs, visitor);
		mCc_ast_visit_expression(expression->binary_op.rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		visit_if_pre_order(expression, visitor->expression_unary_op, visitor);
		mCc_ast_visit_expression(expression->unary_op.rhs, visitor);
		visit_if_post_order(expression, visitor->expression_unary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mCc_ast_visit_expression(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
		break;
	}

	visit_if_post_order(expression, visitor->expression, visitor);
}

void mCc_ast_visit_statement_list(struct mCc_ast_statement_list *statement_list,
                                  struct mCc_ast_visitor *visitor)
{
	assert(statement_list);
	assert(visitor);
	/* visit_if_pre_order(statement_list, visitor->statement_list,
	 * visitor); */

	struct mCc_ast_statement_list *next = statement_list;
	while (next != NULL) {
		visit_if_pre_order(next, visitor->statement_list, visitor);
		mCc_ast_visit_statement(next->statement, visitor);
		visit_if_post_order(statement_list, visitor->statement_list, visitor);
		next = next->next;
	}
	/* visit_if_post_order(statement_list, visitor->statement_list,
	 * visitor); */
}

void mCc_ast_visit_statement(struct mCc_ast_statement *statement,
                             struct mCc_ast_visitor *visitor)
{
	assert(statement);
	assert(visitor);

	visit_if_pre_order(statement, visitor->statement, visitor);

	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_IF:
		visit_if_pre_order(statement, visitor->statement_if, visitor);
		mCc_ast_visit_expression(statement->if_condition, visitor);
		mCc_ast_visit_statement(statement->if_branch, visitor);
		if (statement->else_branch != NULL) {
			mCc_ast_visit_statement(statement->else_branch, visitor);
		}
		visit_if_post_order(statement, visitor->statement_if, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_WHILE:
		visit_if_pre_order(statement, visitor->statement_while, visitor);
		mCc_ast_visit_expression(statement->while_condition, visitor);
		mCc_ast_visit_statement(statement->body, visitor);
		visit_if_post_order(statement, visitor->statement_while, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_RETURN:
		visit_if_pre_order(statement, visitor->statement_return, visitor);
		if (statement->expression != NULL) {
			mCc_ast_visit_expression(statement->expression, visitor);
		}
		visit_if_post_order(statement, visitor->statement_return, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		visit_if_pre_order(statement, visitor->statement_declaration, visitor);
		if (statement->declaration != NULL) {
			mCc_ast_visit_declaration(statement->declaration, visitor);
		}
		visit_if_post_order(statement, visitor->statement_declaration, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_ASSIGNMENT:
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT:
		visit_if_pre_order(statement, visitor->statement_assignment, visitor);
		if (statement->assignment != NULL) {
			mCc_ast_visit_assignment(statement->assignment, visitor);
		}
		visit_if_post_order(statement, visitor->statement_assignment, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_EXPRESSION:
		visit_if_pre_order(statement, visitor->statement_expression, visitor);
		if (statement->expression != NULL) {
			mCc_ast_visit_expression(statement->expression, visitor);
		}
		visit_if_post_order(statement, visitor->statement_expression, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT:
		visit_if_pre_order(statement, visitor->statement_compound_stmt,
		                   visitor);
		if (statement->statement_list != NULL) {
			mCc_ast_visit_statement_list(statement->statement_list, visitor);
		}
		visit_if_post_order(statement, visitor->statement_compound_stmt,
		                    visitor);
		break;
	default: break;
	}

	visit_if_post_order(statement, visitor->statement, visitor);
}

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	visit_if_pre_order(literal, visitor->literal, visitor);

	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		visit(literal, visitor->literal_bool, visitor, MCC_VISIT_NO_TYPE);
		break;

	case MCC_AST_LITERAL_TYPE_INT:
		visit(literal, visitor->literal_int, visitor, MCC_VISIT_NO_TYPE);
		break;

	case MCC_AST_LITERAL_TYPE_FLOAT:
		visit(literal, visitor->literal_float, visitor, MCC_VISIT_NO_TYPE);
		break;

	case MCC_AST_LITERAL_TYPE_STRING:
		visit(literal, visitor->literal_string, visitor, MCC_VISIT_NO_TYPE);
		break;
	}

	visit_if_post_order(literal, visitor->literal, visitor);
}

void mCc_ast_visit_identifier(struct mCc_ast_identifier *identifier,
                              struct mCc_ast_visitor *visitor)
{
	assert(identifier);
	assert(visitor);

	visit_if_pre_order(identifier, visitor->identifier, visitor);

	visit_if_post_order(identifier, visitor->identifier, visitor);
}
