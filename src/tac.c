#include "mCc/ast_symbol_table.h"
#include "mCc/error_manager.h"
#include "mCc/symbol_table.h"
#include <mCc/tac.h>

int mCc_tac_identifier = 0;
int mCc_tac_label = 0;

char *mCc_tac_new_identifier()
{
	mCc_tac_identifier++;
	char *str = malloc(sizeof(char) * 30);
	sprintf(str, "t%d", mCc_tac_identifier);
	return str;
}

int mCc_tac_new_label()
{
	mCc_tac_label++;
	return mCc_tac_label;
}

struct mCc_ast_parameter *
mCc_tac_reverse_parameter(struct mCc_ast_parameter *root)
{
	struct mCc_ast_parameter *new_root = 0;
	while (root) {
		struct mCc_ast_parameter *next = root->next;
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
	char *str = t.val ? malloc(sizeof(char) * (strlen(t.val) + 10))
	                  : malloc(sizeof(char) * 20); // + type and array

	if (t.array) {
		sprintf(str, "%s_%c_%d", t.val, mCc_tac_get_ret_type(t.type), t.array);
		return str;
	} else {
		sprintf(str, "%s_%c", t.val, mCc_tac_get_ret_type(t.type));
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

mCc_tac_node mCc_tac_add_node(mCc_tac_node node, mCc_tac_node head)
{
	mCc_tac_node p;
	if (head == NULL) {
		head = node;
	} else {
		p = head;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = node;
	}
	return head;
}

void mCc_tac_print_tac(mCc_tac_node head, FILE *out)
{
	mCc_tac_node p;
	p = head;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_SIMPLE: {
			char *var1 = mCc_tac_get_tac_var(p->type_simple.arg0);
			char *var2 = mCc_tac_get_tac_var(p->type_simple.arg1);
			fprintf(out, "\t%s = %s\n", var1, var2);
			free(var1);
			free(var2);
			break;
		}
		case TAC_LINE_TYPE_DOUBLE: {
			char *var1 = mCc_tac_get_tac_var(p->type_double.arg0);
			char *var2 = mCc_tac_get_tac_var(p->type_double.arg1);
			char *var3 = mCc_tac_get_tac_var(p->type_double.arg2);

			fprintf(out, "\t%s = %s %s %s\n", var1, var2,
			        mCc_tac_get_binary_op(p->type_double.op.op,
			                              p->type_double.op.type),
			        var3);

			free(var1);
			free(var2);
			free(var3);
			break;
		}

		case TAC_LINE_TYPE_CALL:
			fprintf(out, "\tCALL(%s)\n", p->type_call.name);
			break;
		case TAC_LINE_TYPE_POP: {
			char *var1 = mCc_tac_get_tac_var(p->type_pop.var);
			fprintf(out, "\t%s = POP\n", var1);
			free(var1);
			break;
		}
		case TAC_LINE_TYPE_PUSH: {
			char *var1 = mCc_tac_get_tac_var(p->type_push.var);
			fprintf(out, "\tPUSH %s\n", var1);
			free(var1);
			break;
		}
		case TAC_LINE_TYPE_RETURN: {
			char *var1 = mCc_tac_get_tac_var(p->type_return.var);
			fprintf(out, "\tRETURN %s\n", var1);
			free(var1);
			break;
		}
		case TAC_LINE_TYPE_IFZ: {
			char *var = mCc_tac_get_tac_var(p->type_ifz.var);
			fprintf(out, "\tIFZ %s GOTO L%d\n", var,
			        p->type_ifz.jump_label_name);
			free(var);
			break;
		}
		case TAC_LINE_TYPE_DECL_ARRAY: {
			char *var1 = mCc_tac_get_tac_var(p->type_decl_array.var);
			fprintf(out, "\t%s[%d]\n", var1, p->type_decl_array.var.array);
			free(var1);
			break;
		}

		case TAC_LINE_TYPE_IDEN_ARRAY: {
			char *var1 = mCc_tac_get_tac_var(p->type_array_iden.arr);
			char *var2 = mCc_tac_get_tac_var(p->type_array_iden.loc);
			char *var3 = mCc_tac_get_tac_var(p->type_array_iden.var);

			fprintf(out, "\t%s = %s[%s]\n", var3, var1, var2);
			free(var1);
			free(var2);
			free(var3);
			break;
		}
		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY: {
			char *var1 = mCc_tac_get_tac_var(p->type_assign_array.arr);
			char *var2 = mCc_tac_get_tac_var(p->type_assign_array.loc);
			char *var3 = mCc_tac_get_tac_var(p->type_assign_array.var);

			fprintf(out, "\t%s[%s] = %s\n", var1, var2, var3);
			free(var1);
			free(var2);
			free(var3);
			break;
		}

		case TAC_LINE_TYPE_LABEL:
			fprintf(out, "\tL%d:\n", p->type_label.label_name);
			break;
		case TAC_LINE_TYPE_JUMP:
			fprintf(out, "\tGOTO L%d\n", p->type_jump.jump_name);
			break;
		case TAC_LINE_TYPE_LABELFUNC:
			fprintf(out, "START FUNC %s\n", p->type_label_func.func_name);
			break;
		case TAC_LINE_TYPE_LABELFUNC_END:
			fprintf(out, "END FUNC %s\n\n", p->type_label_func_end.func_name);
			break;
		case TAC_LINE_TYPE_BEGIN: break;
		default: fprintf(out, "error %d\n", p->type); break;
		}
		p = p->next;
	}
}

void mCc_tac_delete_tac(mCc_tac_node head)
{
	mCc_tac_node p;
	p = head;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_SIMPLE: {
			free(p->type_simple.arg0.val);
			free(p->type_simple.arg1.val);
		} break;
		case TAC_LINE_TYPE_DOUBLE:
			free(p->type_double.arg0.val);
			free(p->type_double.arg1.val);
			free(p->type_double.arg2.val);
			break;
		case TAC_LINE_TYPE_CALL:
			// free(p->type_call.name);
			break;
		case TAC_LINE_TYPE_POP: free(p->type_pop.var.val); break;
		case TAC_LINE_TYPE_PUSH: free(p->type_push.var.val); break;
		case TAC_LINE_TYPE_RETURN: free(p->type_return.var.val); break;
		case TAC_LINE_TYPE_IFZ: free(p->type_ifz.var.val); break;
		case TAC_LINE_TYPE_DECL_ARRAY: free(p->type_decl_array.var.val); break;
		case TAC_LINE_TYPE_IDEN_ARRAY:
			free(p->type_array_iden.var.val);
			free(p->type_array_iden.arr.val);
			free(p->type_array_iden.loc.val);
			break;
		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY:
			free(p->type_assign_array.var.val);
			free(p->type_assign_array.arr.val);
			free(p->type_assign_array.loc.val);
			break;
		case TAC_LINE_TYPE_LABEL: break;
		case TAC_LINE_TYPE_JUMP: break;
		case TAC_LINE_TYPE_LABELFUNC: break;
		case TAC_LINE_TYPE_LABELFUNC_END: break;
		case TAC_LINE_TYPE_BEGIN: break;
		default: break;
		}
		mCc_tac_node temp = p;
		p = p->next;
		free(temp);
	}
}

struct mCc_tac_var mCc_tac_cgen_literal(struct mCc_ast_literal *literal,
                                        mCc_tac_node tac)
{
	char *str;
	struct mCc_tac_var ret = {
		val : mCc_tac_new_identifier(),
		type : MCC_AST_TYPE_VOID,
	};
	mCc_tac_node node = mCc_tac_create_node();
	node->type = TAC_LINE_TYPE_SIMPLE;
	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_BOOL: {
		ret.type = MCC_AST_TYPE_BOOL;
		str = malloc(sizeof(char) * 10);
		sprintf(str, "%d", literal->b_value);
		node->type_simple.arg0 = ret;
		struct mCc_tac_var arg1 = { MCC_AST_TYPE_BOOL, 0, str };
		node->type_simple.arg1 = arg1;
		break;
	}
	case MCC_AST_LITERAL_TYPE_INT: {
		ret.type = MCC_AST_TYPE_INT;
		str = malloc(sizeof(char) * 30);
		sprintf(str, "%ld", literal->i_value);
		node->type_simple.arg0 = ret;
		struct mCc_tac_var arg1 = { MCC_AST_TYPE_INT, 0, str };
		node->type_simple.arg1 = arg1;
		break;
	}
	case MCC_AST_LITERAL_TYPE_FLOAT: {
		ret.type = MCC_AST_TYPE_FLOAT;
		str = malloc(sizeof(char) * 30);
		sprintf(str, "%f", literal->f_value);
		node->type_simple.arg0 = ret;
		struct mCc_tac_var arg1 = { MCC_AST_TYPE_FLOAT, 0, str };
		node->type_simple.arg1 = arg1;
		break;
	}
	case MCC_AST_LITERAL_TYPE_STRING: {
		str = malloc(sizeof(char) *
		             (strlen(literal->s_value) + 1)); // seg faults without + 10
		sprintf(str, "%s", literal->s_value);
		ret.type = MCC_AST_TYPE_STRING;
		node->type_simple.arg0 = ret;
		struct mCc_tac_var arg1 = { MCC_AST_TYPE_STRING, 0, str };
		node->type_simple.arg1 = arg1;
		break;
	}
	}
	mCc_tac_add_node(node, tac);

	char *val = malloc(sizeof(char) * (strlen(ret.val) + 1));
	sprintf(val, "%s", ret.val);
	struct mCc_tac_var temp = { ret.type, ret.array, val };
	return temp;
}

struct mCc_tac_var
mCc_tac_cgen_identifier(struct mCc_ast_identifier *identifier, mCc_tac_node tac)
{
	struct mCc_tac_var ret = {
		val : mCc_tac_new_identifier(),
		type : identifier->symbol_table_entry->data_type,
		array : (int)identifier->symbol_table_entry->array_size,
	};
	mCc_tac_node node = mCc_tac_create_node();
	node->type = TAC_LINE_TYPE_SIMPLE;
	node->type_simple.arg0 = ret;
	char *str = malloc(sizeof(char) * (strlen(identifier->name) + 1));
	sprintf(str, "%s", identifier->name);
	struct mCc_tac_var arg1 = { identifier->symbol_table_entry->data_type,
		                        (int)identifier->symbol_table_entry->array_size,
		                        str };

	node->type_simple.arg1 = arg1;
	mCc_tac_add_node(node, tac);

	char *val = malloc(sizeof(char) * (strlen(ret.val) + 1));
	sprintf(val, "%s", ret.val);
	struct mCc_tac_var temp = { ret.type, ret.array, val };
	return temp;
}

struct mCc_tac_var
mCc_tac_cgen_expression(struct mCc_ast_expression *expression, mCc_tac_node tac)
{
	struct mCc_tac_var ret = {
		val : mCc_tac_new_identifier(),
		type : MCC_AST_TYPE_VOID,
	};

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP: {
		struct mCc_tac_var t1 =
		    mCc_tac_cgen_expression(expression->binary_op.lhs, tac);
		struct mCc_tac_var t2 =
		    mCc_tac_cgen_expression(expression->binary_op.rhs, tac);
		ret.type = t1.type;   // any of the two
		ret.array = t1.array; // any of the two
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_DOUBLE;

		node->type_double.arg0 = ret;
		node->type_double.arg1 = t1;
		node->type_double.arg2 = t2;
		struct mCc_tac_op op = { expression->binary_op.op, t1.type };
		node->type_double.op = op;
		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_LITERAL: {
		struct mCc_tac_var t = mCc_tac_cgen_literal(expression->literal, tac);
		ret.type = t.type;
		ret.array = t.array;
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_SIMPLE;
		node->type_simple.arg0 = ret;
		node->type_simple.arg1 = t;
		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_identifier(expression->identifier, tac);
		ret.type = t.type;
		ret.array = t.array;
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_SIMPLE;
		node->type_simple.arg0 = ret;
		node->type_simple.arg1 = t;
		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER: {
		struct mCc_tac_var expr = mCc_tac_cgen_expression(
		    expression->array_identifier.expression, tac);
		struct mCc_tac_var id = mCc_tac_cgen_identifier(
		    expression->array_identifier.identifier, tac);
		ret.type = id.type;
		ret.array = 0;

		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_IDEN_ARRAY;

		node->type_array_iden.var = ret;
		node->type_array_iden.arr = id;
		node->type_array_iden.loc = expr;

		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR: {
		struct mCc_ast_argument_list *next = expression->call_expr.arguments;
		while (next != NULL) {
			struct mCc_tac_var t =
			    mCc_tac_cgen_expression(next->expression, tac);
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_PUSH;
			node->type_push.var = t;
			mCc_tac_add_node(node, tac);
			next = next->next;
		}
		struct mCc_sym_table_entry *symbol_table_entry =
		    expression->call_expr.identifier->symbol_table_entry;
		ret.type = symbol_table_entry->data_type;
		ret.array = (int)symbol_table_entry->array_size;

		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_CALL;
		node->type_call.name = expression->call_expr.identifier->name;
		mCc_tac_add_node(node, tac);

		mCc_tac_node nodePop = mCc_tac_create_node();
		nodePop->type = TAC_LINE_TYPE_POP;
		nodePop->type_pop.var = ret;
		mCc_tac_add_node(nodePop, tac);
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(expression->unary_op.rhs, tac);
		ret.type = t.type;
		ret.array = t.array;
		switch (expression->unary_op.op) {
		case MCC_AST_UNARY_OP_NOT: {
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_DOUBLE;
			node->type_double.arg0 = ret;
			char *zeros = malloc(sizeof(char) * (strlen("0") + 1));
			sprintf(zeros, "0");
			struct mCc_tac_var zero = { MCC_AST_TYPE_INT, 0, zeros };
			struct mCc_tac_op op = { MCC_AST_BINARY_OP_EQUALS, t.type };

			node->type_double.arg1 = zero;
			node->type_double.arg2 = t;
			node->type_double.op = op;
			mCc_tac_add_node(node, tac);
			break;
		}
		case MCC_AST_UNARY_OP_MINUS: {
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_DOUBLE;
			node->type_double.arg0 = ret;
			char *zeros = malloc(sizeof(char) * (strlen("0") + 1));
			sprintf(zeros, "0");
			struct mCc_tac_var zero = { MCC_AST_TYPE_INT, 0, zeros };
			struct mCc_tac_op op = { MCC_AST_BINARY_OP_SUB, t.type };

			node->type_double.arg1 = zero;
			node->type_double.arg2 = t;
			node->type_double.op = op;
			mCc_tac_add_node(node, tac);
			break;
		}
		}
		break;
	}
	case MCC_AST_EXPRESSION_TYPE_PARENTH: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(expression->expression, tac);
		ret.type = t.type;
		ret.array = t.array;
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_SIMPLE;
		node->type_simple.arg0 = ret;
		node->type_simple.arg1 = t;
		mCc_tac_add_node(node, tac);
		break;
	}
	}

	char *val = malloc(sizeof(char) * (strlen(ret.val) + 1));
	sprintf(val, "%s", ret.val);
	struct mCc_tac_var temp = { ret.type, ret.array, val };
	return temp;
}

void mCc_tac_cgen_statement(struct mCc_ast_statement *statement,
                            mCc_tac_node tac)
{
	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_WHILE: {
		int labelBefore = mCc_tac_new_label();
		int labelAfter = mCc_tac_new_label();
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_LABEL;
		node->type_label.label_name = labelBefore;
		mCc_tac_add_node(node, tac);
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->while_condition, tac);
		mCc_tac_node nodeExpression = mCc_tac_create_node();

		nodeExpression->type = TAC_LINE_TYPE_IFZ;
		nodeExpression->type_ifz.jump_label_name = labelAfter;
		nodeExpression->type_ifz.var = t;

		mCc_tac_add_node(nodeExpression, tac);
		mCc_tac_cgen_statement(statement->body, tac);
		mCc_tac_node nodeJump = mCc_tac_create_node();
		nodeJump->type = TAC_LINE_TYPE_JUMP;
		nodeJump->type_jump.jump_name = labelBefore;
		mCc_tac_add_node(nodeJump, tac);
		mCc_tac_node nodeLabel = mCc_tac_create_node();
		nodeLabel->type = TAC_LINE_TYPE_LABEL;
		nodeLabel->type_label.label_name = labelAfter;
		mCc_tac_add_node(nodeLabel, tac);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_IF: {

		if (!statement->else_branch) {
			int labelAfter = mCc_tac_new_label();

			struct mCc_tac_var t =
			    mCc_tac_cgen_expression(statement->if_condition, tac);

			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_IFZ;
			node->type_ifz.jump_label_name = labelAfter;
			node->type_ifz.var = t;
			mCc_tac_add_node(node, tac);

			mCc_tac_cgen_statement(statement->if_branch, tac);

			mCc_tac_node nodeLabel = mCc_tac_create_node();
			nodeLabel->type = TAC_LINE_TYPE_LABEL;
			nodeLabel->type_label.label_name = labelAfter;
			mCc_tac_add_node(nodeLabel, tac);

		} else {
			int labelElse = mCc_tac_new_label();
			int labelAfter = mCc_tac_new_label();

			struct mCc_tac_var t =
			    mCc_tac_cgen_expression(statement->if_condition, tac);

			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_IFZ;
			node->type_ifz.jump_label_name = labelElse;
			node->type_ifz.var = t;
			mCc_tac_add_node(node, tac);

			mCc_tac_cgen_statement(statement->if_branch, tac);

			mCc_tac_node nodeJump = mCc_tac_create_node();
			nodeJump->type = TAC_LINE_TYPE_JUMP;
			nodeJump->type_jump.jump_name = labelAfter;
			mCc_tac_add_node(nodeJump, tac);

			mCc_tac_node nodeLabel = mCc_tac_create_node();
			nodeLabel->type = TAC_LINE_TYPE_LABEL;
			nodeLabel->type_label.label_name = labelElse;
			mCc_tac_add_node(nodeLabel, tac);

			mCc_tac_cgen_statement(statement->else_branch, tac);

			mCc_tac_node nodeLabel2 = mCc_tac_create_node();
			nodeLabel2->type = TAC_LINE_TYPE_LABEL;
			nodeLabel2->type_label.label_name = labelAfter;
			mCc_tac_add_node(nodeLabel2, tac);
		}
		break;
	}
	case MCC_AST_STATEMENT_TYPE_RETURN: {
		if (!statement->expression) {
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_RETURN;
			struct mCc_tac_var var = { MCC_AST_TYPE_VOID, 0, NULL };
			node->type_return.var = var;
			mCc_tac_add_node(node, tac);
		} else {
			struct mCc_tac_var label =
			    mCc_tac_cgen_expression(statement->expression, tac);
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_RETURN;
			node->type_return.var = label;
			mCc_tac_add_node(node, tac);
		}
		break;
	}
	case MCC_AST_STATEMENT_TYPE_DECLARATION:
		switch (statement->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: {
			mCc_tac_node node = mCc_tac_create_node();
			node->type = TAC_LINE_TYPE_DECL_ARRAY;

			char *str = malloc(
			    sizeof(char) *
			    (strlen(statement->declaration->array_decl.identifier->name) +
			     1));
			sprintf(str, "%s",
			        statement->declaration->array_decl.identifier->name);
			struct mCc_tac_var var = {
				statement->declaration->identifier_type,
				(int)statement->declaration->array_decl.literal->i_value, str
			};
			node->type_decl_array.var = var;
			mCc_tac_add_node(node, tac);
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

		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_SIMPLE;

		char *str = malloc(sizeof(char) * (strlen(identifier) + 1));
		sprintf(str, "%s", identifier);
		struct mCc_tac_var arg0 = {
			statement->assignment->identifier->symbol_table_entry->data_type,
			(int)statement->assignment->identifier->symbol_table_entry
			    ->array_size,
			str
		};

		node->type_simple.arg0 = arg0;

		node->type_simple.arg1 = t;
		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT: {
		struct mCc_tac_var t =
		    mCc_tac_cgen_expression(statement->assignment->array_ass.rhs, tac);
		struct mCc_tac_var index = mCc_tac_cgen_expression(
		    statement->assignment->array_ass.index, tac);
		mCc_tac_node node = mCc_tac_create_node();
		node->type = TAC_LINE_TYPE_ASSIGNMENT_ARRAY;

		char *str =
		    malloc(sizeof(char) *
		           (strlen(statement->assignment->identifier->name) + 1));
		sprintf(str, "%s", statement->assignment->identifier->name);

		struct mCc_tac_var arr = {
			statement->assignment->identifier->symbol_table_entry->data_type,
			(int)statement->assignment->identifier->symbol_table_entry
			    ->array_size,
			str,
		};
		node->type_assign_array.arr = arr;
		node->type_assign_array.loc = index;
		node->type_assign_array.var = t;

		mCc_tac_add_node(node, tac);
		break;
	}
	case MCC_AST_STATEMENT_TYPE_EXPRESSION: {
		struct mCc_tac_var var =
		    mCc_tac_cgen_expression(statement->expression, tac);
		free(var.val);
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
	mCc_tac_node nodeLabel = mCc_tac_create_node();
	nodeLabel->type = TAC_LINE_TYPE_LABELFUNC;
	nodeLabel->type_label_func.func_name =
	    function_def->function_identifier->name;
	mCc_tac_add_node(nodeLabel, tac);

	// reverse the order
	struct mCc_ast_parameter *next =
	    mCc_tac_reverse_parameter(function_def->parameters);
	while (next != NULL) {
		mCc_tac_node nodeParameter = mCc_tac_create_node();
		switch (next->declaration->declaration_type) {
		case MCC_AST_DECLARATION_TYPE_DECLARATION: {
			nodeParameter->type = TAC_LINE_TYPE_POP;
			char *str = malloc(
			    sizeof(char) *
			    (strlen(next->declaration->normal_decl.identifier->name) + 1));
			sprintf(str, "%s", next->declaration->normal_decl.identifier->name);
			struct mCc_tac_var var = { next->declaration->identifier_type, 0,
				                       str };
			nodeParameter->type_pop.var = var;
			mCc_tac_add_node(nodeParameter, tac);
			break;
		}
		case MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION: {
			nodeParameter->type = TAC_LINE_TYPE_POP;
			char *str = malloc(
			    sizeof(char) *
			    (strlen(next->declaration->array_decl.identifier->name) + 1));
			sprintf(str, "%s", next->declaration->array_decl.identifier->name);
			struct mCc_tac_var var = {
				next->declaration->identifier_type,
				(int)next->declaration->array_decl.literal->i_value, str
			};
			nodeParameter->type_pop.var = var;

			mCc_tac_add_node(nodeParameter, tac);
			break;
		}
		}
		next = next->next;
	}
	mCc_tac_cgen_statement(function_def->compound_stmt, tac);
	mCc_tac_node nodeEnd = mCc_tac_create_node();
	nodeEnd->type = TAC_LINE_TYPE_LABELFUNC_END;
	nodeEnd->type_label_func_end.func_name =
	    function_def->function_identifier->name;
	mCc_tac_add_node(nodeEnd, tac);
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

	mCc_tac_node tac = mCc_ast_get_tac_program(program);
	fprintf(out, "start tac\n");
	mCc_tac_print_tac(tac, out);
	fprintf(out, "end tac\n");
	mCc_tac_delete_tac(tac);
}

mCc_tac_node mCc_ast_get_tac_program(struct mCc_ast_program *program)
{
	assert(program);
	mCc_tac_identifier = 0;
	mCc_tac_label = 0;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL, 0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_visitor visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_program(program, &visitor);

	mCc_tac_node tac = mCc_tac_create_node();
	tac->type = TAC_LINE_TYPE_BEGIN;

	mCc_tac_cgen_function_def_list(program->function_def_list, tac);

	return tac;
}

mCc_tac_node mCc_ast_get_tac_expression(struct mCc_ast_expression *expression)
{
	assert(expression);
	mCc_tac_identifier = 0;
	mCc_tac_label = 0;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL, 0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_visitor visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_expression(expression, &visitor);

	mCc_tac_node tac = mCc_tac_create_node();
	tac->type = TAC_LINE_TYPE_BEGIN;

	mCc_tac_cgen_expression(expression, tac);

	return tac;
}

mCc_tac_node mCc_ast_get_tac_statement(struct mCc_ast_statement *statement)
{
	assert(statement);
	mCc_tac_identifier = 0;
	mCc_tac_label = 0;

	struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL, 0 };

	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_ast_visitor visitor =
	    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);

	mCc_ast_visit_statement(statement, &visitor);

	mCc_tac_node tac = mCc_tac_create_node();
	tac->type = TAC_LINE_TYPE_BEGIN;

	mCc_tac_cgen_statement(statement, tac);

	return tac;
}

mCc_tac_node mCc_ast_get_tac(struct mCc_ast_program *program)
{
	assert(program);
	mCc_tac_identifier = 0;
	mCc_tac_label = 0;

	mCc_tac_node tac = mCc_tac_create_node();
	tac->type = TAC_LINE_TYPE_BEGIN;

	mCc_tac_cgen_function_def_list(program->function_def_list, tac);

	return tac;
}
