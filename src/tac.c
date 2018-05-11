#include "mCc/tac.h"
#include "mCc/ast_print.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_visit.h"
#include <assert.h>
#include <mCc/ast.h>
#include <mCc/symbol_table.h>

void cgen_statement_list(struct mCc_ast_statement_list *, FILE *);

int identifier = 0;
int label = 0;

int newIdentifier()
{
	identifier++;
	return identifier;
}

int newLabel()
{
	label++;
	return label;
}

struct tac_var {
	int num;
	enum mCc_ast_type type;
};

char getRetType(enum mCc_ast_type t)
{
	switch (t) {
	case MCC_AST_TYPE_BOOL: return 'b';
	case MCC_AST_TYPE_INT: return 'i';
	case MCC_AST_TYPE_FLOAT: return 'f';
	case MCC_AST_TYPE_STRING: return 's';
	case MCC_AST_TYPE_VOID: return '-';
	default: return '-';
	}
}

char *print_tac_var(struct tac_var t, char *buffer)
{
	// fprintf(stderr, "%d\n", t.num);
	sprintf(buffer, "t%d_%c", t.num, getRetType(t.type));
	return buffer;
}

const char *mCc_get_binary_op(enum mCc_ast_binary_op op)
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

struct tac_var cgen_literal(struct mCc_ast_literal *literal, FILE *out)
{
	char buffer[15] = { 0 };
	struct tac_var ret = {
		num : newIdentifier(),
		type : MCC_AST_TYPE_VOID,
	};
	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		ret.type = MCC_AST_TYPE_BOOL;
		fprintf(out, "%s = %d\n", print_tac_var(ret, buffer), literal->b_value);
		break;
	case MCC_AST_LITERAL_TYPE_INT:
		ret.type = MCC_AST_TYPE_INT;
		fprintf(out, "%s = %ld\n", print_tac_var(ret, buffer),
		        literal->i_value);
		break;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		ret.type = MCC_AST_TYPE_FLOAT;
		fprintf(out, "%s = %f\n", print_tac_var(ret, buffer), literal->f_value);
		break;
	case MCC_AST_LITERAL_TYPE_STRING:
		ret.type = MCC_AST_TYPE_STRING;
		fprintf(out, "%s = %s\n", print_tac_var(ret, buffer), literal->s_value);
		break;
	}
	return ret;
}

struct tac_var cgen_identifier(struct mCc_ast_identifier *identifier, FILE *out)
{
	char buffer[15] = { 0 };
	struct tac_var ret = {
		num : newIdentifier(),
		type : identifier->symbol_table_entry->data_type,
	};
	fprintf(out, "%s = %s\n", print_tac_var(ret, buffer), identifier->name);
	return ret;
}

struct tac_var cgen_expression(struct mCc_ast_expression *expression, FILE *out)
{
	char buffer[15] = { 0 };
	char buffer2[15] = { 0 };
	char buffer3[15] = { 0 };

	struct tac_var ret = {
		num : newIdentifier(),
		type : MCC_AST_TYPE_VOID,
	};

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP: {
		struct tac_var t1 = cgen_expression(expression->binary_op.lhs, out);
		struct tac_var t2 = cgen_expression(expression->binary_op.rhs, out);
		ret.type = t1.type; // any of the two
		fprintf(out, "%s = %s %s %s\n", print_tac_var(ret, buffer),
		        print_tac_var(t1, buffer2),
		        mCc_get_binary_op(expression->binary_op.op),
		        print_tac_var(t2, buffer3));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_LITERAL: {
		struct tac_var t = cgen_literal(expression->literal, out);
		ret.type = t.type;
		fprintf(out, "%s = %s\n", print_tac_var(ret, buffer),
		        print_tac_var(t, buffer2));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER: {
		struct tac_var t = cgen_identifier(expression->identifier, out);
		ret.type = t.type;
		fprintf(out, "%s = %s\n", print_tac_var(ret, buffer),
		        print_tac_var(t, buffer2));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER: {
		struct tac_var t =
		    cgen_expression(expression->array_identifier.expression, out);
		struct tac_var u =
		    cgen_identifier(expression->array_identifier.identifier, out);
		fprintf(out, "%s = %s + %s\n", print_tac_var(ret, buffer),
		        print_tac_var(u, buffer2), print_tac_var(t, buffer3));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR: {
		struct mCc_ast_argument_list *next = expression->call_expr.arguments;
		while (next != NULL) {
			struct tac_var t = cgen_expression(next->expression, out);
			fprintf(out, "PushStack(%s)\n", print_tac_var(t, buffer));
			next = next->next;
		}
		struct mCc_sym_table_entry *symbol_table_entry =
		    expression->call_expr.identifier->symbol_table_entry;
		ret.type = symbol_table_entry->data_type;
		fprintf(out, "Call (%s)\n", expression->call_expr.identifier->name);
		fprintf(out, "%s = PopStack()\n", print_tac_var(ret, buffer));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP: {
		struct tac_var t = cgen_expression(expression->unary_op.rhs, out);
		ret.type = t.type; // not sure
		switch (expression->unary_op.op) {
		case MCC_AST_UNARY_OP_NOT: {
			fprintf(out, "%s = 0 == %s\n", print_tac_var(ret, buffer),
			        print_tac_var(t, buffer2));
			break;
		}
		case MCC_AST_UNARY_OP_MINUS: {
			fprintf(out, "%s = 0 - %s\n", print_tac_var(ret, buffer),
			        print_tac_var(t, buffer2));
			break;
		}
		}
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_PARENTH: {
		struct tac_var t = cgen_expression(expression->expression, out);
		ret.type = t.type;
		fprintf(out, "%s = %s\n", print_tac_var(ret, buffer),
		        print_tac_var(t, buffer2));
		break;
	}
	}
	return ret;
}

void cgen_statement(struct mCc_ast_statement *statement, FILE *out)
{
	char buffer[15] = { 0 };
	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_WHILE: {
		int labelBefore = newLabel();
		int labelAfter = newLabel();
		fprintf(out, "L%d:\n", labelBefore);
		struct tac_var t = cgen_expression(statement->while_condition, out);
		fprintf(out, "Ifz %s Goto L%d\n", print_tac_var(t, buffer), labelAfter);
		cgen_statement(statement->body, out);
		fprintf(out, "Goto L%d\n", labelBefore);
		fprintf(out, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_IF: {
		int labelElse = newLabel();
		int labelAfter = newLabel();
		struct tac_var t = cgen_expression(statement->if_condition, out);
		fprintf(out, "Ifz %s Goto L%d\n", print_tac_var(t, buffer), labelElse);
		cgen_statement(statement->if_branch, out);
		fprintf(out, "Goto L%d\n", labelAfter);
		fprintf(out, "L%d:\n", labelElse);
		cgen_statement(statement->else_branch, out);
		fprintf(out, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_RETURN: {
		struct tac_var label = cgen_expression(statement->expression, out);
		fprintf(out, "RETURN %s\n", print_tac_var(label, buffer));
		break;
	}
	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		switch (statement->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: {
			fprintf(out, "ARRAY_DECL %s %s (%ld) \n",
			        statement->declaration->array_decl.identifier->name,
			        mCc_ast_print_type(statement->declaration->identifier_type),
			        statement->declaration->array_decl.literal->i_value);
			break;
		}
		case MCC_AST_DECLARATION_TYPE_DECLARATION: {
			// does not matter in our tac
			break;
		}
		}
		break;

	case MCC_AST_STATEMENT_TYPE_ASSIGNMENT: {
		struct tac_var t =
		    cgen_expression(statement->assignment->normal_ass.rhs, out);
		char *identifier = statement->assignment->identifier->name;
		fprintf(out, "%s = %s\n", identifier, print_tac_var(t, buffer));
		break;
	}
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT: {
		// TODO: Maybe add type of array assignment?
		struct tac_var t =
		    cgen_expression(statement->assignment->array_ass.rhs, out);
		struct tac_var index =
		    cgen_expression(statement->assignment->array_ass.index, out);
		fprintf(out, "ARRAY_ASSIGNMENT %s[%s]=",
		        statement->assignment->identifier->name,
		        print_tac_var(index, buffer));
		fprintf(out, "%s\n", print_tac_var(t, buffer));
		break;
	}
	case MCC_AST_STATEMENT_TYPE_EXPRESSION: {
		cgen_expression(statement->expression, out);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT:
		cgen_statement_list(statement->statement_list, out);
		break;
	}
}

void cgen_statement_list(struct mCc_ast_statement_list *statement_list,
                         FILE *out)
{
	struct mCc_ast_statement_list *next = statement_list;
	while (next != NULL) {
		cgen_statement(next->statement, out);
		next = next->next;
	}
}

void cgen_function_def(struct mCc_ast_function_def *function_def, FILE *out)
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
			fprintf(out, "%s = PopStackArray(%s) size-%ld\n",
			        next->declaration->array_decl.identifier->name,
			        mCc_ast_print_type(next->declaration->identifier_type),
			        next->declaration->array_decl.literal->i_value);
			break;
		}
		next = next->next;
	}
	cgen_statement(function_def->compound_stmt, out);
	fprintf(out, "END FUNC %s:\n", function_def->function_identifier->name);
}

void cgen_function_def_list(struct mCc_ast_function_def_list *function_def_list,
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

	struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL, 0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_visitor visitor =
	    symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(program, &visitor);

	fprintf(out, "start tac\n");

	cgen_function_def_list(program->function_def_list, out);

	fprintf(out, "end tac\n\n");
}
