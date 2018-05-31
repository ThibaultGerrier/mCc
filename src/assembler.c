#include "mCc/ast_symbol_table.h"
#include "mCc/error_manager.h"
#include "mCc/symbol_table.h"
#include "uthash.h"
#include <mCc/assembler.h>
#include <mCc/ast.h>
#include <mCc/tac.h>

int mCc_ass_label;

struct mCc_ass_str {
	char *str;
	int label;
	struct mCc_ass_str *next;
};
typedef struct mCc_ass_str *mCc_ass_str_node;

struct mCc_ass_function {
	char *name;
	int stack_size;
	struct mCc_ass_function_var *data;
	struct mCc_ass_str *next;
};
typedef struct mCc_ass_function *mCc_ass_function_node;

struct mCc_ass_function_var {
	char *name; // key
	int location;
	bool is_reference;
	UT_hash_handle hh;
};
typedef struct mCc_ass_function_var *mCc_ass_function_var_node;

void add_variable(mCc_ass_function_var_node *table, int location,
                  char *var_name, bool is_reference)
{
	//fprintf(stderr, "%s\n", var_name);
	struct mCc_ass_function_var *s;
	HASH_FIND_STR(*table, var_name, s);
	if (s == NULL) {
		s = malloc(sizeof(struct mCc_ass_function_var));
		s->location = location;
		s->name = var_name;
		s->is_reference = is_reference;
		HASH_ADD_STR(*table, name, s);
	}
}

void print_var_table(mCc_ass_function_var_node table)
{
	mCc_ass_function_var_node s;
	for (s = table; s != NULL; s = (mCc_ass_function_var_node)(s->hh.next)) {
		fprintf(stderr, "var id %s: loc %d, ref %d\n", s->name, s->location, s->is_reference);
	}
}

typedef struct list {
	void *data;
	struct list *next;
} List;

struct ass {
	List *strings;
	List *function_data;
};

List *create(void *data, List *next)
{
	List *new_node = (List *)malloc(sizeof(List));
	new_node->data = data;
	new_node->next = next;
	return new_node;
}

List *append(List *head, void *data)
{
	List *new_node = create(data, NULL);
	if (head == NULL)
		return new_node;
	List *cursor = head;
	while (cursor->next != NULL)
		cursor = cursor->next;
	cursor->next = new_node;
	return head;
}

mCc_ass_str_node mCc_ass_create_node_str()
{
	mCc_ass_str_node temp;
	temp = (mCc_ass_str_node)malloc(sizeof(struct mCc_ass_str));
	temp->next = NULL;
	return temp;
}

mCc_ass_function_node mCc_ass_create_node_function()
{
	mCc_ass_function_node temp;
	temp = (mCc_ass_function_node)malloc(sizeof(struct mCc_ass_function));
	temp->next = NULL;
	return temp;
}

int mCc_ass_new_label()
{
	mCc_ass_label++;
	return mCc_ass_label;
}

void print_str_ass(void *v, FILE *out)
{
	mCc_ass_str_node n = (mCc_ass_str_node)v;
	fprintf(out, "LC%d:\n\t.string\t\"%s\"\n", n->label, n->str);
}
void print_str(void *v, FILE *out)
{
	mCc_ass_str_node n = (mCc_ass_str_node)v;
	fprintf(out, "label: %d, str: %s\n", n->label, n->str);
}

void print_func_data(void *v, FILE *out)
{
	mCc_ass_function_node n = (mCc_ass_function_node)v;
	print_var_table(n->data);
	fprintf(stderr, "---\n");
}


void print_func(void *v, FILE *out)
{
	mCc_ass_function_node n = (mCc_ass_function_node)v;
	fprintf(out, "FUNCTION: %s\n", n->name);
}

void print(List *p, void (*f)(void *, FILE *), FILE *out)
{
	while (p) {
		(*f)(p->data, out);
		p = p->next;
	}
}

void analyze(mCc_tac_node head, FILE *out)
{
	struct ass *ass;
	ass = (struct ass *)malloc(sizeof(struct ass));
	ass->strings = NULL;
	ass->function_data = NULL;

	mCc_ass_function_var_node cur_function_data = NULL;
	mCc_tac_node p;
	p = head;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_SIMPLE:
			if (p->type_simple.arg1.type == MCC_AST_TYPE_STRING &&
			    p->type_simple.arg1.literal == true) {
				mCc_ass_str_node str = mCc_ass_create_node_str();
				str->str = p->type_simple.arg1.val;
				str->label = mCc_ass_new_label();
				ass->strings = append(ass->strings, str);
			}
			if (p->type_simple.arg0.literal == false) {
				add_variable(
				    &cur_function_data, 0,
				    p->type_simple.arg0.val, false);
			}
			if (p->type_simple.arg1.literal == false) {
				add_variable(
						&cur_function_data, 0,
				    p->type_simple.arg1.val, false);
			}
			break;
		case TAC_LINE_TYPE_DOUBLE: break;
		case TAC_LINE_TYPE_CALL: break;
		case TAC_LINE_TYPE_POP:
				add_variable(
						&cur_function_data, 0,
						p->type_pop.var.val, p->type_pop.var.array > 0);
			break;
		case TAC_LINE_TYPE_PUSH: break;
		case TAC_LINE_TYPE_RETURN: break;
		case TAC_LINE_TYPE_IFZ: break;
		case TAC_LINE_TYPE_DECL_ARRAY: break;
		case TAC_LINE_TYPE_IDEN_ARRAY: break;
		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY: break;
		case TAC_LINE_TYPE_LABEL: break;
		case TAC_LINE_TYPE_JUMP: break;
		case TAC_LINE_TYPE_LABELFUNC: {
			/*mCc_ass_function_node node = mCc_ass_create_node_function();
			node->name = p->type_label_func.func_name;
			node->stack_size = 0;
			node->data = NULL;
			ass->function_data = append(ass->function_data, node);*/
			//cur_function_data = node->data;
			cur_function_data = NULL;
			break;
		}
		case TAC_LINE_TYPE_LABELFUNC_END: {
			//print_var_table(cur_function_data);
			mCc_ass_function_node node = mCc_ass_create_node_function();
			node->name = p->type_label_func_end.func_name;
			node->stack_size = 0;
			node->data = cur_function_data;
			ass->function_data = append(ass->function_data, node);
			//cur_function_data = node->data;
			//ass->function_data = append(ass->function_data, cur_function_data);
			//fprintf(stderr, "----\n");
			break;
		}

		case TAC_LINE_TYPE_BEGIN: break;
		}
		p = p->next;
	}

	print(ass->function_data, print_func_data, out);
	print(ass->strings, print_str, out);
	fprintf(out, "----\n");


	// start printing assembler code
	fprintf(out, "\t.file\t\"generated.c\"\n\t.section\t.rodata\n");
	print(ass->strings, print_str_ass, out);
	fprintf(out, "\t.text\n");

	int funcRetLabel = 0;
	// mCc_tac_node p;
	p = head;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_LABELFUNC:
			fprintf(out, "\t.globl\t%s\n\t.type\t%s, @function\n%s:\n",
			        p->type_label_func.func_name, p->type_label_func.func_name,
			        p->type_label_func.func_name);
			fprintf(out, "\tpushl\t%%ebp\n\tmovl\t%%esp, %%ebp\n");
			funcRetLabel = mCc_ass_new_label();
			break;
		case TAC_LINE_TYPE_SIMPLE: break;
		case TAC_LINE_TYPE_DOUBLE: break;
		case TAC_LINE_TYPE_CALL: break;
		case TAC_LINE_TYPE_POP: break;
		case TAC_LINE_TYPE_PUSH: break;
		case TAC_LINE_TYPE_RETURN: break;
		case TAC_LINE_TYPE_IFZ: break;
		case TAC_LINE_TYPE_DECL_ARRAY: break;
		case TAC_LINE_TYPE_IDEN_ARRAY: break;
		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY: break;
		case TAC_LINE_TYPE_LABEL: break;
		case TAC_LINE_TYPE_JUMP: break;
		case TAC_LINE_TYPE_LABELFUNC_END:
			fprintf(out, ".L%d:\n", funcRetLabel);
			fprintf(out, "\tleave\n\tret\n");
			fprintf(out, "\t.size\t%s, .-%s\n",
			        p->type_label_func_end.func_name,
			        p->type_label_func.func_name);
			break;
		case TAC_LINE_TYPE_BEGIN: break;
		}
		p = p->next;
	}

	fprintf(out, "\t.ident\t\"mCc "
	             "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void mCc_ast_print_assembler_program(struct mCc_ast_program *program, FILE *out)
{
	assert(program);

	mCc_tac_node tac = mCc_ast_get_tac_program(program);
	mCc_tac_print_tac(tac, out);

	analyze(tac, out);
}
