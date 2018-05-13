#include "mCc/tac.h"
#include "mCc/ast_print.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_visit.h"
#include <assert.h>
#include <mCc/ast.h>
#include <mCc/symbol_table.h>
#include <stdarg.h>

int mCc_tac_identifier = 0;
int mCc_tac_label = 0;

int mCc_tac_new_identifier()
{
	mCc_tac_identifier++;
	return mCc_tac_identifier;
}

int mCc_tac_new_label()
{
	mCc_tac_label++;
	return mCc_tac_label;
}

struct mCc_ast_parameter* mCc_tac_reverse_parameter(struct mCc_ast_parameter* root) {
    struct mCc_ast_parameter* new_root = 0;
    while (root) {
        struct mCc_ast_parameter* next = root->next;
        root->next = new_root;
        new_root = root;
        root = next;
    }
    return new_root;
}

char mCc_tac_get_ret_type(enum mCc_ast_type t)
{
	switch (t) {
	case MCC_AST_TYPE_BOOL: return 'b';
	case MCC_AST_TYPE_INT: return 'i';
	case MCC_AST_TYPE_FLOAT: return 'f';
	case MCC_AST_TYPE_STRING: return 's';
	case MCC_AST_TYPE_VOID: return 'v';
	default: return '-';
	}
}

char *mCc_tac_get_tac_var(struct mCc_tac_var t)
{
	char *str = malloc(10);
    if(t.array){
        sprintf(str, "t%d_%c_%d", t.num, mCc_tac_get_ret_type(t.type),t.array);
        return str;
    }else{
        sprintf(str, "t%d_%c", t.num, mCc_tac_get_ret_type(t.type));
        return str;
    }

}

char *mCc_tac_get_binary_op(enum mCc_ast_binary_op op, enum mCc_ast_type t)
{
	switch (op) {
	/* math operations */
	case MCC_AST_BINARY_OP_ADD: {
		switch (t) {
		case MCC_AST_TYPE_INT: return "+_i";
		case MCC_AST_TYPE_FLOAT: return "+_f";
		default: return "+";
		}
	}
	case MCC_AST_BINARY_OP_SUB: {
		switch (t) {
		case MCC_AST_TYPE_INT: return "-_i";
		case MCC_AST_TYPE_FLOAT: return "-_f";
		default: return "-";
		}
	}
	case MCC_AST_BINARY_OP_MUL: {
		switch (t) {
		case MCC_AST_TYPE_INT: return "*_i";
		case MCC_AST_TYPE_FLOAT: return "*_f";
		default: return "*";
		}
	}
	case MCC_AST_BINARY_OP_DIV: {
		switch (t) {
		case MCC_AST_TYPE_INT: return "/_i";
		case MCC_AST_TYPE_FLOAT: return "/_f";
		default: return "/";
		}
	}

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

mCc_tac_node mCc_tac_create_node()
{
	mCc_tac_node temp;
	temp = (mCc_tac_node)malloc(sizeof(struct mCc_tac));
	temp->next = NULL;
	return temp;
}

mCc_tac_node mCc_tac_add_node(mCc_tac_node head, char *arg0, char *arg1,
                              char *arg2, char *op, enum mCc_tac_line_type type)
{
	mCc_tac_node temp, p;
	temp = mCc_tac_create_node();
	temp->arg0 = arg0;
	temp->arg1 = arg1;
	temp->arg2 = arg2;
	temp->op = op;
	temp->type = type;
	if (head == NULL) {
		head = temp;
	} else {
		p = head;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = temp;
	}
	return head;
}

void mCc_tac_print_tac(mCc_tac_node head, FILE *out)
{
	mCc_tac_node p;
	p = head;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_SIMPLE:
			fprintf(out, "%s = %s\n", p->arg0, p->arg1);
			break;
		case TAC_LINE_TYPE_DOUBLE:
			fprintf(out, "%s = %s %s %s\n", p->arg0, p->arg1, p->op, p->arg2);
			break;
		case TAC_LINE_TYPE_SPECIAL: fprintf(out, "%s", p->arg0); break;
		}
		p = p->next;
	}
}
void mCc_tac_delete_tac(mCc_tac_node head)
{
	mCc_tac_node curr;
	while ((curr = head) != NULL) {
		head = head->next;
		free(curr->arg0);
		free(curr->arg1);
		free(curr->arg2);
		free(curr);
	}
}

void mCc_tac_emitDouble(mCc_tac_node tac, char *arg0, char *arg1, char *arg2,
                        char *op)
{
	mCc_tac_add_node(tac, arg0, arg1, arg2, op, TAC_LINE_TYPE_DOUBLE);
}

void mCc_tac_emitSimple(mCc_tac_node tac, char *arg0, char *arg1)
{
	mCc_tac_add_node(tac, arg0, arg1, NULL, NULL, TAC_LINE_TYPE_SIMPLE);
}

void mCc_tac_emitSpecial(mCc_tac_node tac, char *str)
{
	mCc_tac_add_node(tac, str, NULL, NULL, NULL, TAC_LINE_TYPE_SPECIAL);
}

void mCc_tac_emitS(mCc_tac_node tac, char *format, ...)
{
	char *str = malloc(sizeof(char) * 100);
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(str, 100, format, argptr);
	va_end(argptr);
	mCc_tac_emitSpecial(tac, str);
}

struct mCc_tac_var mCc_tac_cgen_literal(struct mCc_ast_literal *literal,
                                        mCc_tac_node tac)
{
	char *str;
	struct mCc_tac_var ret = {
		num : mCc_tac_new_identifier(),
		type : MCC_AST_TYPE_VOID,

	};
	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		ret.type = MCC_AST_TYPE_BOOL;
		str = malloc(sizeof(char) * 10);
		sprintf(str, "%d", literal->b_value);
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret), str);
		break;
	case MCC_AST_LITERAL_TYPE_INT:
		ret.type = MCC_AST_TYPE_INT;
		str = malloc(sizeof(char) * 10);
		sprintf(str, "%ld", literal->i_value);
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret), str);
		break;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		ret.type = MCC_AST_TYPE_FLOAT;
		str = malloc(sizeof(char) * 10);
		sprintf(str, "%f", literal->f_value);
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret), str);
		break;
	case MCC_AST_LITERAL_TYPE_STRING:
		ret.type = MCC_AST_TYPE_STRING;
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret), literal->s_value);
		break;
	}
	return ret;
}

struct mCc_tac_var
mCc_tac_cgen_identifier(struct mCc_ast_identifier *identifier, mCc_tac_node tac)
{
	struct mCc_tac_var ret = {
		num : mCc_tac_new_identifier(),
		type : identifier->symbol_table_entry->data_type,
        array : identifier->symbol_table_entry->array_size,
	};
	mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret), identifier->name);
	return ret;
}

struct mCc_tac_var
mCc_tac_cgen_expression(struct mCc_ast_expression *expression, mCc_tac_node tac)
{
	struct mCc_tac_var ret = {
		num : mCc_tac_new_identifier(),
		type : MCC_AST_TYPE_VOID,
	};

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP: {
		struct mCc_tac_var t1 =
		    mCc_tac_cgen_expression(expression->binary_op.lhs, tac);
		struct mCc_tac_var t2 =
		    mCc_tac_cgen_expression(expression->binary_op.rhs, tac);
		ret.type = t1.type; // any of the two
		mCc_tac_emitDouble(
		    tac, mCc_tac_get_tac_var(ret), mCc_tac_get_tac_var(t1),
		    mCc_tac_get_tac_var(t2),
		    mCc_tac_get_binary_op(expression->binary_op.op, t1.type));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_LITERAL: {
		struct mCc_tac_var t = mCc_tac_cgen_literal(expression->literal, tac);
		ret.type = t.type;
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret),
		                   mCc_tac_get_tac_var(t));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_identifier(expression->identifier, tac);
		ret.type = t.type;
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret),
		                   mCc_tac_get_tac_var(t));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER: {
		struct mCc_tac_var expr = mCc_tac_cgen_expression(
		    expression->array_identifier.expression, tac);
		struct mCc_tac_var id = mCc_tac_cgen_identifier(
		    expression->array_identifier.identifier, tac);
		ret.type = expr.type;
		mCc_tac_emitDouble(tac, mCc_tac_get_tac_var(ret),
		                   mCc_tac_get_tac_var(id), mCc_tac_get_tac_var(expr),
		                   "+_i");
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR: {
		struct mCc_ast_argument_list *next = expression->call_expr.arguments;
		while (next != NULL) {
			struct mCc_tac_var t =
			    mCc_tac_cgen_expression(next->expression, tac);
			mCc_tac_emitS(tac, "PushStack(%s)\n", mCc_tac_get_tac_var(t));
			next = next->next;
		}
		struct mCc_sym_table_entry *symbol_table_entry =
		    expression->call_expr.identifier->symbol_table_entry;
		ret.type = symbol_table_entry->data_type;
		mCc_tac_emitS(tac, "Call (%s)\n",
		              expression->call_expr.identifier->name);
		mCc_tac_emitS(tac, "%s = PopStack()\n", mCc_tac_get_tac_var(ret));
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(expression->unary_op.rhs, tac);
		ret.type = t.type; // not sure
		switch (expression->unary_op.op) {
		case MCC_AST_UNARY_OP_NOT: {
			mCc_tac_emitDouble(tac, mCc_tac_get_tac_var(ret), "0",
			                   mCc_tac_get_tac_var(t), "==");
			break;
		}
		case MCC_AST_UNARY_OP_MINUS: {
			mCc_tac_emitDouble(tac, mCc_tac_get_tac_var(ret), "0",
			                   mCc_tac_get_tac_var(t), "-");
			break;
		}
		}
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_PARENTH: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(expression->expression, tac);
		ret.type = t.type;
		mCc_tac_emitSimple(tac, mCc_tac_get_tac_var(ret),
		                   mCc_tac_get_tac_var(t));
		break;
	}
	}
	return ret;
}

void mCc_tac_cgen_statement(struct mCc_ast_statement *statement,
                            mCc_tac_node tac)
{
	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_WHILE: {
		int labelBefore = mCc_tac_new_label();
		int labelAfter = mCc_tac_new_label();
		mCc_tac_emitS(tac, "L%d:\n", labelBefore);
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->while_condition, tac);
		mCc_tac_emitS(tac, "Ifz %s Goto L%d\n", mCc_tac_get_tac_var(t),
		              labelAfter);
		mCc_tac_cgen_statement(statement->body, tac);
		mCc_tac_emitS(tac, "Goto L%d\n", labelBefore);
		mCc_tac_emitS(tac, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_IF: {
		int labelElse = mCc_tac_new_label();
		int labelAfter = mCc_tac_new_label();
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->if_condition, tac);
		mCc_tac_emitS(tac, "Ifz %s Goto L%d\n", mCc_tac_get_tac_var(t),
		              labelElse);
		mCc_tac_cgen_statement(statement->if_branch, tac);
		mCc_tac_emitS(tac, "Goto L%d\n", labelAfter);
		mCc_tac_emitS(tac, "L%d:\n", labelElse);
		mCc_tac_cgen_statement(statement->else_branch, tac);
		mCc_tac_emitS(tac, "L%d:\n", labelAfter);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_RETURN: {
		struct mCc_tac_var label =
		    mCc_tac_cgen_expression(statement->expression, tac);
		mCc_tac_emitS(tac, "RETURN %s\n", mCc_tac_get_tac_var(label));
		break;
	}
	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		switch (statement->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: {
			mCc_tac_emitS(
			    tac, "ARRAY_DECL %s %s (%ld) \n",
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
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->assignment->normal_ass.rhs, tac);
		char *identifier = statement->assignment->identifier->name;
		mCc_tac_emitSimple(tac, identifier, mCc_tac_get_tac_var(t));
		break;
	}
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->assignment->array_ass.rhs, tac);
		struct mCc_tac_var index = mCc_tac_cgen_expression(
		    statement->assignment->array_ass.index, tac);
		mCc_tac_emitS(tac, "ARRAY_ASSIGNMENT %s[%s]=%s\n",
		              statement->assignment->identifier->name,
		              mCc_tac_get_tac_var(index), mCc_tac_get_tac_var(t));

		break;
	}
	case MCC_AST_STATEMENT_TYPE_EXPRESSION: {
		mCc_tac_cgen_expression(statement->expression, tac);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_COMPOUND_STMT:
		mCc_tac_cgen_statement_list(statement->statement_list, tac);
		break;
	}
}

void mCc_tac_cgen_statement_list(struct mCc_ast_statement_list *statement_list,
                                 mCc_tac_node tac)
{
	struct mCc_ast_statement_list *next = statement_list;
	while (next != NULL) {
		mCc_tac_cgen_statement(next->statement, tac);
		next = next->next;
	}
}

void mCc_tac_cgen_function_def(struct mCc_ast_function_def *function_def,
                               mCc_tac_node tac)
{
	mCc_tac_emitS(tac, "START FUNC %s\n",
	              function_def->function_identifier->name);
	//reverse the order
    struct mCc_ast_parameter *next = mCc_tac_reverse_parameter(function_def->parameters);
	while (next != NULL) {
		switch (next->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_DECLARATION: {
			mCc_tac_emitS(
			    tac, "%s = PopStack(%s)\n",
			    next->declaration->normal_decl.identifier->name,
			    mCc_ast_print_type(next->declaration->identifier_type));
			break;
		}
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: {
			mCc_tac_emitS(
			    tac, "%s = PopStackArray(%s_%ld)\n",
			    next->declaration->array_decl.identifier->name,
			    mCc_ast_print_type(next->declaration->identifier_type),
			    next->declaration->array_decl.literal->i_value);
			break;
		}
		}
		next = next->next;
	}
	mCc_tac_cgen_statement(function_def->compound_stmt, tac);
	mCc_tac_emitS(tac, "END FUNC %s\n",
	              function_def->function_identifier->name);
}

mCc_tac_node mCc_tac_cgen_function_def_list(
    struct mCc_ast_function_def_list *function_def_list, mCc_tac_node tac)
{
	struct mCc_ast_function_def_list *next = function_def_list;
	while (next != NULL) {
		mCc_tac_cgen_function_def(next->function_def, tac);
		next = next->next;
	}
	return tac;
}

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *program)
{
	assert(out);
	assert(program);
	mCc_tac_identifier = 0;
	mCc_tac_label = 0;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL, 0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_visitor visitor =
	    symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(program, &visitor);

	fprintf(out, "start tac\n");
	char *start = malloc(sizeof(char) * 25);
	sprintf(start, "start\n");

	mCc_tac_node tac =
	    mCc_tac_add_node(NULL, start, NULL, NULL, NULL, TAC_LINE_TYPE_SPECIAL);

	mCc_tac_cgen_function_def_list(program->function_def_list, tac);
	mCc_tac_print_tac(tac, out);
	mCc_tac_delete_tac(tac);

	fprintf(out, "end tac\n\n");
}

