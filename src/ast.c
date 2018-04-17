#include "mCc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------- Identifier */

struct mCc_ast_identifier *mCc_ast_new_identifier(const char *name)
{
	struct mCc_ast_identifier *identifier = malloc(sizeof(*identifier));
	assert(identifier);
	int len = strlen(name) + 1;
	identifier->name = malloc(len);
	assert(identifier->name);
	memcpy(identifier->name, name, len);
	return identifier;
}

void mCc_ast_delete_identifier(struct mCc_ast_identifier *identifier)
{
	assert(identifier);
	assert(identifier->name);
	free(identifier->name);
	free(identifier);
}

/* ------------------------------------------------------------- Declaration */

struct mCc_ast_declaration *
mCc_ast_new_declaration(enum mCc_ast_type type,
                        struct mCc_ast_identifier *identifier)
{
	assert(identifier);

	struct mCc_ast_declaration *decl = malloc(sizeof(*decl));
	assert(decl);

	decl->declaration_type = MCC_AST_DECLARATION_TYPE_DECLARATION;
	decl->identifier_type = type;
	decl->normal_decl.identifier = identifier;
	return decl;
}

struct mCc_ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_type type,
                              struct mCc_ast_identifier *identifier, long size)
{
	assert(identifier);

	struct mCc_ast_declaration *decl = malloc(sizeof(*decl));
	assert(decl);

	decl->declaration_type = MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION;
	decl->identifier_type = type;
	decl->array_decl.identifier = identifier;
	decl->array_decl.literal = mCc_ast_new_literal_int(size);
	return decl;
}

void mCc_ast_delete_declaration(struct mCc_ast_declaration *declaration)
{
	assert(declaration);
	if (declaration->declaration_type ==
	    MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION) {
		mCc_ast_delete_literal(declaration->array_decl.literal);
		mCc_ast_delete_identifier(declaration->array_decl.identifier);
	} else {
		mCc_ast_delete_identifier(declaration->normal_decl.identifier);
	}
	free(declaration);
}

/* ------------------------------------------------------------- Parameter */

struct mCc_ast_parameter *
mCc_ast_new_parameter(struct mCc_ast_declaration *declaration)
{
	assert(declaration);

	struct mCc_ast_parameter *param = malloc(sizeof(*param));
	assert(param);

	param->declaration = declaration;
	param->next = NULL;
	return param;
}

void mCc_ast_delete_parameter(struct mCc_ast_parameter *parameter)
{
	assert(parameter);
	mCc_ast_delete_declaration(parameter->declaration);
	if (parameter->next != NULL) {
		mCc_ast_delete_parameter(parameter->next);
	}
	free(parameter);
}

/* ------------------------------------------------------------- Arguments */

struct mCc_ast_argument_list *
mCc_ast_new_argument_list(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_argument_list *list = malloc(sizeof(*list));
	assert(list);

	list->expression = expression;
	list->next = NULL;
	return list;
}

void mCc_ast_delete_argument_list(struct mCc_ast_argument_list *argument_list)
{
	assert(argument_list);
	mCc_ast_delete_expression(argument_list->expression);
	if (argument_list->next != NULL) {
		mCc_ast_delete_argument_list(argument_list->next);
	}
	free(argument_list);
}

/* ------------------------------------------------------------- Assignment */

struct mCc_ast_assignment *
mCc_ast_new_assignment(struct mCc_ast_identifier *identifier,
                       struct mCc_ast_expression *rhs)
{
	assert(identifier);
	assert(rhs);

	struct mCc_ast_assignment *ass = malloc(sizeof(*ass));
	assert(ass);

	ass->type = MCC_AST_ASSIGNMENT_TYPE_NORMAL;
	ass->identifier = identifier;
	ass->normal_ass.rhs = rhs;
	return ass;
}

struct mCc_ast_assignment *
mCc_ast_new_array_assignment(struct mCc_ast_identifier *identifier,
                             struct mCc_ast_expression *index,
                             struct mCc_ast_expression *rhs)
{
	assert(identifier);
	assert(index);
	assert(rhs);

	struct mCc_ast_assignment *ass = malloc(sizeof(*ass));
	assert(ass);

	ass->type = MCC_AST_ASSIGNMENT_TYPE_ARRAY;
	ass->identifier = identifier;
	ass->array_ass.index = index;
	ass->array_ass.rhs = rhs;
	return ass;
}

void mCc_ast_delete_assignment(struct mCc_ast_assignment *assignment)
{
	assert(assignment);
	mCc_ast_delete_identifier(assignment->identifier);
	if (assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL) {
		mCc_ast_delete_expression(assignment->normal_ass.rhs);
	} else {
		mCc_ast_delete_expression(assignment->array_ass.index);
		mCc_ast_delete_expression(assignment->array_ass.rhs);
	}
	free(assignment);
}

/* ------------------------------------------------------------- Expressions */

struct mCc_ast_expression *
mCc_ast_new_expression_identifier(struct mCc_ast_identifier *identifier)
{
	assert(identifier);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;
	expr->identifier = identifier;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_array_identifier(struct mCc_ast_identifier *identifier,
                                        struct mCc_ast_expression *expression)
{
	assert(identifier);
	assert(expression);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER;
	expr->array_identifier.identifier = identifier;
	expr->array_identifier.expression = expression;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_call_expr(struct mCc_ast_identifier *identifier,
                                 struct mCc_ast_argument_list *arguments)
{
	assert(identifier);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_CALL_EXPR;
	expr->call_expr.identifier = identifier;
	expr->call_expr.arguments = arguments;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_literal(struct mCc_ast_literal *literal)
{
	assert(literal);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_unary_op(enum mCc_ast_unary_op op,
                                struct mCc_ast_expression *rhs)
{
	assert(rhs);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->unary_op.op = op;
	expr->unary_op.rhs = rhs;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_expression *lhs,
                                 struct mCc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY_OP;
	expr->binary_op.op = op;
	expr->binary_op.lhs = lhs;
	expr->binary_op.rhs = rhs;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_parenth(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	assert(expr);

	expr->type = MCC_AST_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

void mCc_ast_delete_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mCc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		mCc_ast_delete_expression(expression->unary_op.rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mCc_ast_delete_expression(expression->binary_op.lhs);
		mCc_ast_delete_expression(expression->binary_op.rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mCc_ast_delete_expression(expression->expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		mCc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER:
		mCc_ast_delete_identifier(expression->array_identifier.identifier);
		mCc_ast_delete_expression(expression->array_identifier.expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		mCc_ast_delete_identifier(expression->call_expr.identifier);
		if (expression->call_expr.arguments != NULL) {
			mCc_ast_delete_argument_list(expression->call_expr.arguments);
		}
		break;
	}

	free(expression);
}

/* ---------------------------------------------------------- Statement List */

struct mCc_ast_statement_list *
mCc_ast_new_statement_list(struct mCc_ast_statement *statement)
{
	assert(statement);
	struct mCc_ast_statement_list *list = malloc(sizeof(*list));
	assert(list);

	list->statement = statement;
	list->next = NULL;
	return list;
}

void mCc_ast_delete_statement_list(
    struct mCc_ast_statement_list *statement_list)
{
	assert(statement_list);
	if (statement_list->next != NULL) {
		mCc_ast_delete_statement_list(statement_list->next);
	}
	mCc_ast_delete_statement(statement_list->statement);
	free(statement_list);
}

/* -------------------------------------------------------------- Statements */

struct mCc_ast_statement *
mCc_ast_new_statement_if(struct mCc_ast_expression *condition,
                         struct mCc_ast_statement *if_branch,
                         struct mCc_ast_statement *else_branch)
{
	assert(condition);
	assert(if_branch);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));

	stmt->type = MCC_AST_STATEMENT_TYPE_IF;
	stmt->if_condition = condition;
	stmt->if_branch = if_branch;
	stmt->else_branch = else_branch;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_while(struct mCc_ast_expression *condition,
                            struct mCc_ast_statement *body)
{
	assert(condition);
	assert(body);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));

	stmt->type = MCC_AST_STATEMENT_TYPE_WHILE;
	stmt->while_condition = condition;
	stmt->body = body;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_return(struct mCc_ast_expression *expression)
{
	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));

	stmt->type = MCC_AST_STATEMENT_TYPE_RETURN;
	stmt->expression = expression;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_assignment(struct mCc_ast_assignment *assignment)
{
	assert(assignment);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	assert(stmt);

	stmt->type = assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL
	                 ? MCC_AST_STATEMENT_TYPE_ASSIGNMENT
	                 : MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT;
	stmt->assignment = assignment;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_declaration(struct mCc_ast_declaration *declaration)
{
	assert(declaration);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	assert(stmt);

	stmt->type = MCC_AST_STATEMENT_TYPE_DECLARATION;
	stmt->declaration = declaration;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	assert(stmt);

	stmt->type = MCC_AST_STATEMENT_TYPE_EXPRESSION;
	stmt->expression = expression;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_compound(struct mCc_ast_statement_list *statement_list)
{
	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	assert(stmt);

	stmt->type = MCC_AST_STATEMENT_TYPE_COMPOUND_STMT;
	stmt->statement_list = statement_list;
	return stmt;
}

void mCc_ast_delete_statement(struct mCc_ast_statement *statement)
{
	assert(statement);

	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_IF:
		mCc_ast_delete_expression(statement->if_condition);
		mCc_ast_delete_statement(statement->if_branch);
		if (statement->else_branch != NULL) {
			mCc_ast_delete_statement(statement->else_branch);
		}
		break;
	case MCC_AST_STATEMENT_TYPE_WHILE:
		mCc_ast_delete_expression(statement->while_condition);
		mCc_ast_delete_statement(statement->body);
		break;
	case MCC_AST_STATEMENT_TYPE_RETURN:
		if (statement->expression != NULL)
			mCc_ast_delete_expression(statement->expression);
		break;
	case MCC_AST_STATEMENT_TYPE_EXPRESSION:
		mCc_ast_delete_expression(statement->expression);
		break;
	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT:
		if (statement->statement_list != NULL) {
			mCc_ast_delete_statement_list(statement->statement_list);
		}
		break;
	case MCC_AST_STATEMENT_TYPE_ASSIGNMENT:
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT:
		mCc_ast_delete_assignment(statement->assignment);
		break;
	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		mCc_ast_delete_declaration(statement->declaration);
		break;
	default: break;
	}

	free(statement);
}

/* ---------------------------------------------------------------- Literals */

struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_int(long value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_float(double value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_string(char *value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	assert(lit);

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->s_value = value;
	return lit;
}

void mCc_ast_delete_literal(struct mCc_ast_literal *literal)
{
	assert(literal);
	if (literal->type == MCC_AST_LITERAL_TYPE_STRING) {
		free(literal->s_value);
	}
	free(literal);
}

/* ----------------------------------------------------- Function Definition */

struct mCc_ast_function_def *
mCc_ast_new_function_def(enum mCc_ast_type return_type,
                         struct mCc_ast_identifier *function_identifier,
                         struct mCc_ast_parameter *parameters,
                         struct mCc_ast_statement *compound_stmt)
{
	assert(function_identifier);
	/* assert(parameters); */
	/* assert(compound_stmt); */

	struct mCc_ast_function_def *function_def = malloc(sizeof(*function_def));
	assert(function_def);

	function_def->return_type = return_type;
	function_def->function_identifier = function_identifier;
	function_def->parameters = parameters;
	function_def->compound_stmt = compound_stmt;
	return function_def;
}

void mCc_ast_delete_function_def(struct mCc_ast_function_def *function_def)
{
	assert(function_def);
	mCc_ast_delete_identifier(function_def->function_identifier);
	if (function_def->parameters != NULL) {
		mCc_ast_delete_parameter(function_def->parameters);
	}
	mCc_ast_delete_statement(function_def->compound_stmt);
	free(function_def);
}

/* ----------------------------------------------- Function Definition List */

struct mCc_ast_function_def_list *
mCc_ast_new_function_def_list(struct mCc_ast_function_def *function_def)
{
	assert(function_def);
	struct mCc_ast_function_def_list *list = malloc(sizeof(*list));
	assert(list);

	list->function_def = function_def;
	list->next = NULL;
	return list;
}

void mCc_ast_delete_function_def_list(
    struct mCc_ast_function_def_list *function_def_list)
{
	assert(function_def_list);
	if (function_def_list->next != NULL) {
		mCc_ast_delete_function_def_list(function_def_list->next);
	}
	mCc_ast_delete_function_def(function_def_list->function_def);
	free(function_def_list);
}

/* ---------------------------------------------------------------- Program */

struct mCc_ast_program *
mCc_ast_new_program(struct mCc_ast_function_def_list *function_def_list)
{
	assert(function_def_list);
	struct mCc_ast_program *program = malloc(sizeof(*program));
	assert(program);

	program->function_def_list = function_def_list;
	return program;
}

void mCc_ast_delete_program(struct mCc_ast_program *program)
{
	assert(program);
	mCc_ast_delete_function_def_list(program->function_def_list);
	free(program);
}
