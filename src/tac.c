#include "mCc/tac.h"
#include "mCc/ast_print.h"
#include "mCc/ast_visit.h"
#include <assert.h>
#include <mCc/ast.h>

static void cgen_statement_list(struct mCc_ast_statement_list *, FILE *);

static int identifier = 0;
static int label = 0;

static int newIdentifier()
{
	identifier++;
	return identifier;
}

static int newLabel()
{
	label++;
	return label;
}

static const char *mCc_get_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {
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

	return "unknown_op";
}

/*static const char *mCc_get_unary_op(enum mCc_ast_unary_op op)
{
    switch (op) {
        case MCC_AST_UNARY_OP_NOT: return "!";
        case MCC_AST_UNARY_OP_MINUS: return "-";
    }
    return "unknown_unary_op";
}*/

static int cgen_literal(struct mCc_ast_literal *literal, FILE *out)
{
	int i = newIdentifier();
	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		fprintf(out, "t%d = %d\n", i, literal->b_value);
		break;
	case MCC_AST_LITERAL_TYPE_INT:
		fprintf(out, "t%d = %ld\n", i, literal->i_value);
		break;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		fprintf(out, "t%d = %f\n", i, literal->f_value);
		break;
	case MCC_AST_LITERAL_TYPE_STRING:
		fprintf(out, "t%d = %s\n", i, literal->s_value);
		break;
	}
	return i;
}

static int cgen_identifier(struct mCc_ast_identifier *identifier, FILE *out)
{
	int i = newIdentifier();
	fprintf(out, "t%d = %s\n", i, identifier->name);
	return i;
}

static int cgen_expression(struct mCc_ast_expression *expression, FILE *out)
{
	int i = newIdentifier();
	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP: {
		int t1 = cgen_expression(expression->binary_op.lhs, out);
		int t2 = cgen_expression(expression->binary_op.rhs, out);
		fprintf(out, "t%d = t%d %s t%d\n", i, t1,
		        mCc_get_binary_op(expression->binary_op.op), t2);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_LITERAL: {
		int t = cgen_literal(expression->literal, out);
		fprintf(out, "t%d = t%d\n", i, t);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER: {
		int t = cgen_identifier(expression->identifier, out);
		fprintf(out, "t%d = t%d\n", i, t);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER:
		// TODO
		break;
	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR: {
		struct mCc_ast_argument_list *next = expression->call_expr.arguments;
		while (next != NULL) {
			int t = cgen_expression(next->expression, out);
			fprintf(out, "PushStack(t%d)\n", t);
			next = next->next;
		}
		fprintf(out, "Call (%s)\n", expression->call_expr.identifier->name);
		fprintf(out, "t%d = PopStack()\n", i);

		break;
	}
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP: {
		switch (expression->unary_op.op) {
		case MCC_AST_UNARY_OP_NOT: {
			int t = cgen_expression(expression->unary_op.rhs, out);
			fprintf(out, "t%d = 0 == t%d\n", i, t);
			break;
		}
		case MCC_AST_UNARY_OP_MINUS: {
			int t = cgen_expression(expression->unary_op.rhs, out);
			fprintf(out, "t%d = 0 - t%d\n", i, t);
			break;
		}
		}
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_PARENTH: {
		int t = cgen_expression(expression->expression, out);
		fprintf(out, "t%d = t%d\n", i, t);
		break;
	}
	}
	return i;
}

static void cgen_statement(struct mCc_ast_statement *statement, FILE *out)
{
	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_WHILE: {
		int labelBefore = newLabel();
		int labelAfter = newLabel();
		fprintf(out, "L%d:\n", labelBefore);
		int t = cgen_expression(statement->while_condition, out);
		fprintf(out, "Ifz t%d Goto L%d\n", t, labelAfter);
		cgen_statement(statement->body, out);
		fprintf(out, "Goto L%d\n", labelBefore);
		fprintf(out, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_IF: {
		int labelElse = newLabel();
		int labelAfter = newLabel();
		int t = cgen_expression(statement->if_condition, out);
		fprintf(out, "Ifz t%d Goto L%d\n", t, labelElse);
		cgen_statement(statement->if_branch, out);
		fprintf(out, "Goto L%d\n", labelAfter);
		fprintf(out, "L%d:\n", labelElse);
		cgen_statement(statement->else_branch, out);
		fprintf(out, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_RETURN: {
		int label = cgen_expression(statement->expression, out);
		fprintf(out, "RETURN t%d\n", label);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		// does not matter in our tac
		break;

	case MCC_AST_STATEMENT_TYPE_ASSIGNMENT: {
		int t = cgen_expression(statement->assignment->normal_ass.rhs, out);
		char *identifier = statement->assignment->identifier->name;
		fprintf(out, "%s = t%d\n", identifier, t);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT:
		// TODO
		break;

	case MCC_AST_STATEMENT_TYPE_EXPRESSION: {
		cgen_expression(statement->expression, out);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT:
		cgen_statement_list(statement->statement_list, out);
		break;
	}
}

static void cgen_statement_list(struct mCc_ast_statement_list *statement_list,
                                FILE *out)
{
	struct mCc_ast_statement_list *next = statement_list;
	while (next != NULL) {
		cgen_statement(next->statement, out);
		next = next->next;
	}
}

static void cgen_function_def(struct mCc_ast_function_def *function_def,
                              FILE *out)
{
	fprintf(out, "START FUNC %s:\n", function_def->function_identifier->name);
	// TODO: if stack, change order of pop statements!!
	struct mCc_ast_parameter *next = function_def->parameters;
	while (next != NULL) {
		switch (next->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_DECLARATION:
			fprintf(out, "%s = PopStack(%s)\n",
			        next->declaration->normal_decl.identifier->name,
			        mCc_ast_print_type(next->declaration->identifier_type));
			break;
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION:
			// TODO
			break;
		}
		next = next->next;
	}
	cgen_statement(function_def->compound_stmt, out);
	fprintf(out, "END FUNC %s:\n", function_def->function_identifier->name);
}

static void
cgen_function_def_list(struct mCc_ast_function_def_list *function_def_list,
                       FILE *out)
{
	struct mCc_ast_function_def_list *next = function_def_list;
	while (next != NULL) {
		cgen_function_def(next->function_def, out);
		next = next->next;
	}
}

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *program)
{
	assert(out);
	assert(program);
	identifier = 0;
	label = 0;

	fprintf(out, "start tac\n");

	cgen_function_def_list(program->function_def_list, out);

	fprintf(out, "end tac\n\n");
}
