#include "mCc/ast_print.h"

#include <assert.h>

#include "mCc/ast_visit.h"

#define LABEL_SIZE 64

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {

	/**** binary operations ****/
	/* math operations */
	case MCC_AST_BINARY_OP_ADD: return "+";
	case MCC_AST_BINARY_OP_SUB: return "-";
	case MCC_AST_BINARY_OP_MUL: return "*";
	case MCC_AST_BINARY_OP_DIV: return "/";

	/* compare operations */
	case MCC_AST_BINARY_OP_LESS: return "<";
	case MCC_AST_BINARY_OP_GREATER: return ">";
	case MCC_AST_BINARY_OP_LESS_EQUALS: return "<=";
	case MCC_AST_BINARY_OP_GREATER_EQUALS: return ">=";
	case MCC_AST_BINARY_OP_AND: return "&&";
	case MCC_AST_BINARY_OP_OR: return "||";
	case MCC_AST_BINARY_OP_EQUALS: return "==";
	case MCC_AST_BINARY_OP_NOT_EQUALS: return "!=";
	}

	return "unknown op";
}

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op)
{
	/**** unary operations ****/
	switch (op) {
	case MCC_AST_UNARY_OP_NOT: return "!";
	case MCC_AST_UNARY_OP_MINUS: return "-";
	}
	return "unknown unary op";
}

const char *mCc_ast_print_statement(enum mCc_ast_statement_type stmt)
{
	switch (stmt) {
	case MCC_AST_STATEMENT_TYPE_IF: return "if";
	case MCC_AST_STATEMENT_TYPE_WHILE: return "while";
	case MCC_AST_STATEMENT_TYPE_RETURN: return "return";
	case MCC_AST_STATEMENT_TYPE_DECLARATION: return "decl";
	case MCC_AST_STATEMENT_TYPE_ASSIGNMENT: return "ass";
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT: return "ass[]";
	case MCC_AST_STATEMENT_TYPE_EXPRESSION: return "expr";
	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT: return "{}";
	}

	return "unknown statment";
}

const char *mCc_ast_print_type(enum mCc_ast_type type)
{
	switch (type) {
	case MCC_AST_TYPE_BOOL: return "bool";
	case MCC_AST_TYPE_INT: return "int";
	case MCC_AST_TYPE_FLOAT: return "float";
	case MCC_AST_TYPE_STRING: return "string";
	case MCC_AST_TYPE_VOID: return "void";
	}

	return "unknown type";
}

const char *mCc_ast_print_declaration(enum mCc_ast_declaration_type declaration)
{
	switch (declaration) {
	case MCC_AST_DECLARATION_TYPE_DECLARATION: return "decl";
	case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: return "array decl";
	}

	return "unknown statment";
}

const char *mCc_ast_print_ordinal_suffix(size_t num)
{
	switch (num % 100) {
	case 11:
	case 12:
	case 13: return "th";
	}
	switch (num % 10) {
	case 1: return "st";
	case 2: return "nd";
	case 3: return "rd";
	default: return "th";
	}
}

/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n");
	fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node,
                           const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
	        label);
}

static void print_dot_program(struct mCc_ast_program *program,
                              enum mCc_ast_visit_type visit_type,
                              struct mCc_err_error_manager *error_manager,
                              void *data)
{
	assert(program);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "program");

	FILE *out = data;
	print_dot_node(out, program, label);
	print_dot_edge(out, program, program->function_def_list,
	               "function def list");
}

static void print_dot_type(enum mCc_ast_type *type,
                           enum mCc_ast_visit_type visit_type,
                           struct mCc_err_error_manager *error_manager,
                           void *data)
{
	assert(type);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", mCc_ast_print_type(*type));

	FILE *out = data;
	print_dot_node(out, type, label);
}

static void
print_dot_function_def_list(struct mCc_ast_function_def_list *function_def_list,
                            enum mCc_ast_visit_type visit_type,
                            struct mCc_err_error_manager *error_manager,
                            void *data)
{
	assert(function_def_list);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "function list");

	FILE *out = data;
	print_dot_node(out, function_def_list, label);
	print_dot_edge(out, function_def_list, function_def_list->function_def,
	               "function def");
	if (function_def_list->next != NULL) {
		print_dot_edge(out, function_def_list, function_def_list->next,
		               "next function def");
	}
}

static void print_dot_function_def(struct mCc_ast_function_def *function_def,
                                   enum mCc_ast_visit_type visit_type,
                                   struct mCc_err_error_manager *error_manager,
                                   void *data)
{
	assert(function_def);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", "function");

	FILE *out = data;
	print_dot_node(out, function_def, label);
	print_dot_edge(out, function_def, &function_def->return_type,
	               "return type");
	print_dot_edge(out, function_def, function_def->function_identifier,
	               "function def");
	if (function_def->parameters != NULL) {
		print_dot_edge(out, function_def, function_def->parameters,
		               "parameters");
	}
	print_dot_edge(out, function_def, function_def->compound_stmt,
	               "compound stmt");
}

static void print_dot_parameter(struct mCc_ast_parameter *parameter,
                                enum mCc_ast_visit_type visit_type,
                                struct mCc_err_error_manager *error_manager,
                                void *data)
{
	assert(parameter);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", "parameter");

	FILE *out = data;
	print_dot_node(out, parameter, label);
	print_dot_edge(out, parameter, parameter->declaration, "declaration");
	if (parameter->next != NULL) {
		print_dot_edge(out, parameter, parameter->next, "next parameter");
	}
}

static void print_dot_argument_list(struct mCc_ast_argument_list *argument_list,
                                    enum mCc_ast_visit_type visit_type,
                                    struct mCc_err_error_manager *error_manager,
                                    void *data)
{
	assert(argument_list);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "argument list");

	FILE *out = data;
	print_dot_node(out, argument_list, label);
	print_dot_edge(out, argument_list, argument_list->expression, "expression");
	if (argument_list->next != NULL) {
		print_dot_edge(out, argument_list, argument_list->next,
		               "next argument");
	}
}

static void print_dot_declaration(struct mCc_ast_declaration *declaration,
                                  enum mCc_ast_visit_type visit_type,
                                  struct mCc_err_error_manager *error_manager,
                                  void *data)
{
	assert(declaration);
	assert(data);

	FILE *out = data;

	char label[LABEL_SIZE] = { 0 };
	if (declaration->declaration_type == MCC_AST_DECLARATION_TYPE_DECLARATION) {
		snprintf(label, sizeof(label), "%s", "declaration");
		print_dot_edge(out, declaration, &declaration->identifier_type,
		               "identifier type");
		print_dot_edge(out, declaration, declaration->normal_decl.identifier,
		               "identifier");
	}
	if (declaration->declaration_type ==
	    MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION) {
		snprintf(label, sizeof(label), "%s", "declaration[]");
		print_dot_edge(out, declaration, &declaration->identifier_type,
		               "identifier type");
		print_dot_edge(out, declaration, declaration->array_decl.literal,
		               "size");
		print_dot_edge(out, declaration, declaration->array_decl.identifier,
		               "identifier");
	}
	print_dot_node(out, declaration, label);
}

static void
print_dot_statement_list(struct mCc_ast_statement_list *statement_list,
                         enum mCc_ast_visit_type visit_type,
                         struct mCc_err_error_manager *error_manager,
                         void *data)
{
	assert(statement_list);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "statement list");

	FILE *out = data;
	print_dot_node(out, statement_list, label);
	print_dot_edge(out, statement_list, statement_list->statement, "statement");
	if (statement_list->next != NULL) {
		print_dot_edge(out, statement_list, statement_list->next,
		               "next statement");
	}
}

static void print_dot_statement(struct mCc_ast_statement *statement,
                                enum mCc_ast_visit_type visit_type,
                                struct mCc_err_error_manager *error_manager,
                                void *data)
{
	assert(statement);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "stmt: %s",
	         mCc_ast_print_statement(statement->type));

	FILE *out = data;
	print_dot_node(out, statement, label);
}

static void print_dot_statement_compound_stmt(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_COMPOUND_STMT);

	FILE *out = data;
	if (statement->statement_list != NULL) {
		print_dot_edge(out, statement, statement->statement_list, "stmt list");
	}
}

static void print_dot_statement_expression(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_EXPRESSION);

	FILE *out = data;
	if (statement->statement_list != NULL) {
		print_dot_edge(out, statement, statement->expression, "expr");
	}
}

static void print_dot_statement_if(struct mCc_ast_statement *statement,
                                   enum mCc_ast_visit_type visit_type,
                                   struct mCc_err_error_manager *error_manager,
                                   void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_IF);

	FILE *out = data;
	print_dot_edge(out, statement, statement->if_condition, "condition");
	print_dot_edge(out, statement, statement->if_branch, "if");
	if (statement->else_branch != NULL) {
		print_dot_edge(out, statement, statement->else_branch, "else");
	}
}

static void print_dot_statement_while(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_WHILE);

	FILE *out = data;
	print_dot_edge(out, statement, statement->while_condition, "condition");
	print_dot_edge(out, statement, statement->body, "body");
}

static void print_dot_statement_return(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_RETURN);

	FILE *out = data;
	if (statement->expression != NULL) {
		print_dot_edge(out, statement, statement->expression, "expression");
	}
}

static void print_dot_statement_assignment(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_ASSIGNMENT ||
	       statement->type == MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT);

	FILE *out = data;
	print_dot_edge(out, statement, statement->assignment->identifier,
	               "identifier");
	if (statement->assignment->type == MCC_AST_ASSIGNMENT_TYPE_NORMAL) {
		/* snprintf(label, sizeof(label), "="); */
		print_dot_edge(out, statement, statement->assignment->normal_ass.rhs,
		               "rhs");
	} else {
		/* snprintf(label, sizeof(label), "[]="); */
		print_dot_edge(out, statement, statement->assignment->array_ass.index,
		               "index");
		print_dot_edge(out, statement, statement->assignment->array_ass.rhs,
		               "rhs");
	}
}

static void print_dot_statement_declaration(
    struct mCc_ast_statement *statement, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(statement);
	assert(data);
	assert(statement->type == MCC_AST_STATEMENT_TYPE_DECLARATION);

	FILE *out = data;
	print_dot_edge(out, statement, statement->declaration, "decl");
	/* print_dot_edge(out, statement, &statement->declaration->identifier_type,
	 */
	/*                "type"); */
	/* if (statement->declaration->declaration_type == */
	/*     MCC_AST_DECLARATION_TYPE_DECLARATION) { */
	/* 	/1* snprintf(label, sizeof(label), "="); *1/ */
	/* 	print_dot_edge(out, statement, */
	/* 	               statement->declaration->normal_decl.identifier, */
	/* 	               "identifier"); */
	/* } else { */
	/* 	/1* snprintf(label, sizeof(label), "[]="); *1/ */
	/* 	print_dot_edge(out, statement, */
	/* 	               statement->declaration->array_decl.literal, "size"); */
	/* 	print_dot_edge(out, statement, */
	/* 	               statement->declaration->array_decl.identifier, */
	/* 	               "identifier"); */
	/* } */
}

static void print_dot_identifier(struct mCc_ast_identifier *identifier,
                                 enum mCc_ast_visit_type visit_type,
                                 struct mCc_err_error_manager *error_manager,
                                 void *data)
{
	assert(identifier);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", identifier->name);

	FILE *out = data;
	print_dot_node(out, identifier, label);
}

static void print_dot_expression_identifier(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: id");

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->identifier, "id");
}

static void print_dot_expression_array_identifier(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: id[]");

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->array_identifier.identifier,
	               "index");
	print_dot_edge(out, expression, expression->array_identifier.expression,
	               "expr");
}

static void print_dot_expression_call_expr(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: call");

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->call_expr.identifier, "id");
	if (expression->call_expr.arguments != NULL) {
		print_dot_edge(out, expression, expression->call_expr.arguments,
		               "arguments");
	}
}

static void print_dot_expression_literal(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: lit");
	print_dot_edge(out, expression, expression->literal, "literal");
}

static void print_dot_expression_binary_op(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mCc_ast_print_binary_op(expression->binary_op.op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->binary_op.lhs, "lhs");
	print_dot_edge(out, expression, expression->binary_op.rhs, "rhs");
}

static void print_dot_expression_unary_op(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mCc_ast_print_unary_op(expression->unary_op.op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->unary_op.rhs, "rhs");
}

static void print_dot_expression_parenth(
    struct mCc_ast_expression *expression, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "( )");
	print_dot_edge(out, expression, expression->expression, "expression");
}

static void print_dot_literal_bool(struct mCc_ast_literal *literal,
                                   enum mCc_ast_visit_type visit_type,
                                   struct mCc_err_error_manager *error_manager,
                                   void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), literal->b_value ? "true" : "false");

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_int(struct mCc_ast_literal *literal,
                                  enum mCc_ast_visit_type visit_type,
                                  struct mCc_err_error_manager *error_manager,
                                  void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%ld", literal->i_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mCc_ast_literal *literal,
                                    enum mCc_ast_visit_type visit_type,
                                    struct mCc_err_error_manager *error_manager,
                                    void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%f", literal->f_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(
    struct mCc_ast_literal *literal, enum mCc_ast_visit_type visit_type,
    struct mCc_err_error_manager *error_manager, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", literal->s_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static struct mCc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = out,

		.program = print_dot_program,

		.type = print_dot_type,
		.function_def_list = print_dot_function_def_list,
		.function_def = print_dot_function_def,
		.parameter = print_dot_parameter,
		.argument_list = print_dot_argument_list,
		.declaration = print_dot_declaration,

		.expression = NULL,
		.expression_identifier = print_dot_expression_identifier,
		.expression_array_identifier = print_dot_expression_array_identifier,
		.expression_call_expr = print_dot_expression_call_expr,
		.expression_literal = print_dot_expression_literal,
		.expression_binary_op = print_dot_expression_binary_op,
		.expression_unary_op = print_dot_expression_unary_op,
		.expression_parenth = print_dot_expression_parenth,

		.statement = print_dot_statement,
		.statement_list = print_dot_statement_list,
		.statement_if = print_dot_statement_if,
		.statement_while = print_dot_statement_while,
		.statement_return = print_dot_statement_return,
		.statement_assignment = print_dot_statement_assignment,
		.statement_declaration = print_dot_statement_declaration,
		.statement_expression = print_dot_statement_expression,
		.statement_compound_stmt = print_dot_statement_compound_stmt,

		.identifier = print_dot_identifier,

		.literal_bool = print_dot_literal_bool,
		.literal_int = print_dot_literal_int,
		.literal_float = print_dot_literal_float,
		.literal_string = print_dot_literal_string,
	};
}

void mCc_ast_print_dot_program(FILE *out, struct mCc_ast_program *program)
{
	assert(out);
	assert(program);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_program(program, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression)
{
	assert(out);
	assert(expression);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_expression(expression, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_statement(FILE *out, struct mCc_ast_statement *statement)
{
	assert(out);
	assert(statement);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_statement(statement, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *decl)
{
	assert(out);
	assert(decl);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_declaration(decl, &visitor);

	print_dot_end(out);
}
