#include "mCc/ast_symbol_table.h"
#include "mCc/error_manager.h"
#include "mCc/symbol_table.h"
#include "uthash.h"
#include <mCc/assembler.h>
#include <mCc/ast.h>
#include <mCc/tac.h>

int mCc_ass_label;

int round_up(double x)
{
	return (int)(x + 0.5);
}

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
	bool needs_free;
	UT_hash_handle hh;
};
typedef struct mCc_ass_function_var *mCc_ass_function_var_node;

struct mCc_ass_static_data {
	char *name; // key
	int label;
	bool is_string;
	UT_hash_handle hh;
};
typedef struct mCc_ass_static_data *mCc_ass_static_data_node;

bool add_variable(mCc_ass_function_var_node *table, int location,
                  char *var_name, bool is_reference, bool needs_free)
{
	// fprintf(stderr, "%s\n", var_name);
	struct mCc_ass_function_var *s;
	HASH_FIND_STR(*table, var_name, s);
	if (s == NULL) {
		s = malloc(sizeof(struct mCc_ass_function_var));
		s->location = location;
		s->name = var_name;
		s->is_reference = is_reference;
		s->needs_free = needs_free;
		HASH_ADD_STR(*table, name, s);
		return true;
	}
	return false;
}

void add_static_data(mCc_ass_static_data_node *table, int label, char *name,
                     bool is_string)
{
	struct mCc_ass_static_data *s;
	HASH_FIND_STR(*table, name, s);
	if (s == NULL) {
		s = malloc(sizeof(struct mCc_ass_static_data));
		s->label = label;
		s->name = name;
		s->is_string = is_string;
		HASH_ADD_STR(*table, name, s);
	}
}

void print_var_table(mCc_ass_function_var_node table)
{
	mCc_ass_function_var_node s;
	for (s = table; s != NULL; s = (mCc_ass_function_var_node)(s->hh.next)) {
		fprintf(stderr, "var id %s: loc %d, ref %d\n", s->name, s->location,
		        s->is_reference);
	}
}

void print_static_data_table(mCc_ass_static_data_node table, FILE *out)
{
	mCc_ass_static_data_node s;
	for (s = table; s != NULL; s = (mCc_ass_static_data_node)(s->hh.next)) {
		// fprintf(stderr, "var id %s: loc %d, ref %d\n", s->name, s->label);
		if (s->is_string) {
			fprintf(out, "LC%d:\n\t.string\t\"%s\"\n", s->label, s->name);
		} else {
			fprintf(out, "LC%d:\n\t.float\t%s\n", s->label, s->name);
		}
	}
}

typedef struct list {
	void *data;
	struct list *next;
} List;

struct ass {
	// List *strings;
	mCc_ass_static_data_node strings;
	mCc_ass_static_data_node floats;
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
	ass->floats = NULL;
	ass->function_data = NULL;

	mCc_ass_function_var_node cur_function_data = NULL;
	mCc_tac_node p;
	p = head;
	int stackSize = 0;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_SIMPLE: {
			if (p->type_simple.arg1.type == MCC_AST_TYPE_STRING &&
			    p->type_simple.arg1.literal == true) {
				add_static_data(&ass->strings, mCc_ass_new_label(),
				                p->type_simple.arg1.val, true);
			}
			if (p->type_simple.arg1.type == MCC_AST_TYPE_FLOAT &&
			    p->type_simple.arg1.literal == true) {
				add_static_data(&ass->floats, mCc_ass_new_label(),
				                p->type_simple.arg1.val, false);
			}
			// if (p->type_simple.arg0.literal == false) {
			bool added;
			if (p->type_simple.arg0.depth != -1) {
				char *buf = malloc(sizeof(char) *
				                   (strlen(p->type_simple.arg0.val) + 5));
				sprintf(buf, "%s_%d", p->type_simple.arg0.val,
				        p->type_simple.arg0.depth);
				added = add_variable(&cur_function_data, stackSize + 4, buf,
				                     false, true);
			} else {
				added = add_variable(&cur_function_data, stackSize + 4,
				                     p->type_simple.arg0.val, false, true);
			}
			if (added) {
				stackSize += 4;
			}
			//}
			break;
		}
		case TAC_LINE_TYPE_DOUBLE: {
			bool added;
			if (p->type_double.arg0.depth != -1) {
				char *buf = malloc(sizeof(char) *
				                   (strlen(p->type_double.arg0.val) + 5));
				sprintf(buf, "%s_%d", p->type_double.arg0.val,
				        p->type_double.arg0.depth);
				added = add_variable(&cur_function_data, stackSize + 4, buf,
				                     false, true);
			} else {
				added = add_variable(&cur_function_data, stackSize + 4,
				                     p->type_double.arg0.val, false, true);
			}
			if (added) {
				stackSize += 4;
			}
			break;
		}
		case TAC_LINE_TYPE_CALL: break;
		case TAC_LINE_TYPE_POP: {
			// TODO pop in function vs return value
			if (p->type_simple.arg0.depth != -1) {
				char *buf =
				    malloc(sizeof(char) * (strlen(p->type_pop.var.val) + 5));
				sprintf(buf, "%s_%d", p->type_pop.var.val,
				        p->type_pop.var.depth);
				add_variable(&cur_function_data, stackSize + 4, buf,
				             p->type_pop.var.array > 0, true);
			} else {
				add_variable(&cur_function_data, stackSize + 4,
				             p->type_pop.var.val, p->type_pop.var.array > 0,
				             false);
			}
			stackSize += 4;
			break;
		}
		case TAC_LINE_TYPE_PUSH: break;
		case TAC_LINE_TYPE_RETURN: break;
		case TAC_LINE_TYPE_IFZ: break;
		case TAC_LINE_TYPE_DECL_ARRAY: break;
		case TAC_LINE_TYPE_IDEN_ARRAY:
			add_variable(&cur_function_data, stackSize + 4,
						 p->type_array_iden.var.val, false,
						 false);
			stackSize += 4;
			break;
		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY: {
			char *buf = malloc(sizeof(char) *
							   (strlen(p->type_assign_array.arr.val) + 5));
			sprintf(buf, "%s_%d", p->type_assign_array.arr.val,
					p->type_assign_array.arr.depth);
			bool added = add_variable(&cur_function_data, stackSize + 4, buf,
									  false, true);

			if (added) {
				stackSize += p->type_assign_array.arr.array * 4;
			}
			break;
		}

		case TAC_LINE_TYPE_LABEL: break;
		case TAC_LINE_TYPE_JUMP: break;
		case TAC_LINE_TYPE_LABELFUNC: {
			cur_function_data = NULL;
			break;
		}
		case TAC_LINE_TYPE_LABELFUNC_END: {
			mCc_ass_function_node node = mCc_ass_create_node_function();
			node->name = p->type_label_func_end.func_name;

			// round up needed size to the next multiple of 16;
			node->stack_size = round_up((double)stackSize / 16) * 16;

			node->data = cur_function_data;
			ass->function_data = append(ass->function_data, node);

			// reset stuff
			stackSize = 0;
			break;
		}

		case TAC_LINE_TYPE_BEGIN: break;
		}
		p = p->next;
	}

	print(ass->function_data, print_func_data, out);
	// print(ass->strings, print_static_data_table, out);
	fprintf(out, "----\n");

	// start printing assembler code
	fprintf(out, "\t.file\t\"generated.c\"\n");
	if (HASH_COUNT(ass->strings) > 0) {
		fprintf(out, "\t.section\t.rodata\n");
	}
	print_static_data_table(ass->strings, out);
	fprintf(out, "\t.text\n");

	int funcRetLabel = 0;
	// mCc_tac_node p;
	p = head;
	List *func_data_temp = ass->function_data;
	mCc_ass_function_node function_data = NULL;
	while (p != NULL) {
		switch (p->type) {
		case TAC_LINE_TYPE_LABELFUNC:
			function_data = (mCc_ass_function_node)func_data_temp->data;

			fprintf(out, "\t.globl\t%s\n\t.type\t%s, @function\n%s:\n",
			        p->type_label_func.func_name, p->type_label_func.func_name,
			        p->type_label_func.func_name);
			fprintf(out, "\tpushl\t%%ebp\n\tmovl\t%%esp, %%ebp\n");

			fprintf(out, "\tsubl\t$%d, %%esp\n", function_data->stack_size);
			funcRetLabel = mCc_ass_new_label();
			break;
		case TAC_LINE_TYPE_SIMPLE: {
			struct mCc_ass_function_var *arg0_var;
			if (p->type_simple.arg0.depth != -1) {
				char buff[20];
				sprintf(buff, "%s_%d", p->type_simple.arg0.val,
				        p->type_simple.arg0.depth);
				HASH_FIND_STR(function_data->data, buff, arg0_var);
			} else {
				HASH_FIND_STR(function_data->data, p->type_simple.arg0.val,
				              arg0_var);
			}
			if (arg0_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG 0 %s\n",
				        p->type_simple.arg0.val);
			}
			if (p->type_simple.arg1.literal == true) {
				switch (p->type_simple.arg1.type) {
				case MCC_AST_TYPE_BOOL:
				case MCC_AST_TYPE_INT:
					fprintf(out, "\tmovl\t$%s, -%d(%%ebp)\n",
					        p->type_simple.arg1.val, arg0_var->location);
					break;
				case MCC_AST_TYPE_FLOAT: {
					struct mCc_ass_static_data *arg1_var;
					HASH_FIND_STR(ass->floats, p->type_simple.arg1.val,
					              arg1_var);
					if (arg1_var == NULL) {
						fprintf(stderr, "SOMETHING WENT WRONG 1F\n");
					}
					fprintf(out, "\tflds\t.LC%d\n\tfstps\t-%d(%%ebp)\n",
					        arg1_var->label, arg0_var->location);
					break;
				}

				case MCC_AST_TYPE_STRING: {
					struct mCc_ass_static_data *arg1_var;
					HASH_FIND_STR(ass->strings, p->type_simple.arg1.val,
					              arg1_var);
					if (arg1_var == NULL) {
						fprintf(stderr, "SOMETHING WENT WRONG 1S\n");
					}
					fprintf(out, "\tmovvl\t$.LC%d, -%d(%%ebp)\n",
					        arg1_var->label, arg0_var->location);
					break;
				}
				case MCC_AST_TYPE_VOID:
					// shouldn't happen ?
					break;
				}
			} else {
				struct mCc_ass_function_var *arg1_var;
				if (p->type_simple.arg1.depth != -1) {
					char buff[20];
					sprintf(buff, "%s_%d", p->type_simple.arg1.val,
					        p->type_simple.arg1.depth);
					HASH_FIND_STR(function_data->data, buff, arg1_var);
				} else {
					HASH_FIND_STR(function_data->data, p->type_simple.arg1.val,
					              arg1_var);
				}
				if (arg0_var == NULL) {
					fprintf(stderr, "SOMETHING WENT WRONG 1 %s\n",
					        p->type_simple.arg0.val);
				}
				switch (
				    p->type_simple.arg0
				        .type) { // arg0 or 1, shouldn't matter (should be same)
				case MCC_AST_TYPE_BOOL:
				case MCC_AST_TYPE_INT:
				case MCC_AST_TYPE_STRING:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_TYPE_FLOAT:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n", arg0_var->location);
					break;
				case MCC_AST_TYPE_VOID:
					// shouldn't happen ?
					break;
				}
			}
			break;
		}
		case TAC_LINE_TYPE_DOUBLE: {
			struct mCc_ass_function_var *arg0_var;
			struct mCc_ass_function_var *arg1_var;
			struct mCc_ass_function_var *arg2_var;
			if (p->type_double.arg0.depth != -1) {
				char buff[20];
				sprintf(buff, "%s_%d", p->type_double.arg0.val,
				        p->type_double.arg0.depth);
				HASH_FIND_STR(function_data->data, buff, arg0_var);
			} else {
				HASH_FIND_STR(function_data->data, p->type_double.arg0.val,
				              arg0_var);
			}
			if (p->type_double.arg1.depth != -1) {
				char buff[20];
				sprintf(buff, "%s_%d", p->type_double.arg1.val,
				        p->type_double.arg1.depth);
				HASH_FIND_STR(function_data->data, buff, arg1_var);
			} else {
				HASH_FIND_STR(function_data->data, p->type_double.arg1.val,
				              arg1_var);
			}
			if (p->type_double.arg2.depth != -1) {
				char buff[20];
				sprintf(buff, "%s_%d", p->type_double.arg2.val,
				        p->type_double.arg2.depth);
				HASH_FIND_STR(function_data->data, buff, arg2_var);
			} else {
				HASH_FIND_STR(function_data->data, p->type_double.arg2.val,
				              arg2_var);
			}
			if (arg0_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG 0 %s\n",
				        p->type_double.arg0.val);
			}
			if (arg1_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG 1 %s\n",
				        p->type_double.arg1.val);
			}
			if (arg2_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG 2 %s\n",
				        p->type_double.arg2.val);
			}

			// fprintf(out, "\tmovl -%d(%%ebp), %%edx\n", arg1_var->location);
			// fprintf(out, "\tmovl -%d(%%ebp), %%eax\n", arg2_var->location);

			// TODO missing floats
			switch (p->type_double.arg1.type) {
			case MCC_AST_TYPE_BOOL:
			case MCC_AST_TYPE_INT:
				switch (p->type_double.op.op) {
				case MCC_AST_BINARY_OP_ADD:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%edx\n",
					        arg1_var->location);
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\taddl\t%%edx, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_SUB:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tsubl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					break;
				case MCC_AST_BINARY_OP_MUL:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\timull\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					break;
				case MCC_AST_BINARY_OP_DIV:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcltd\n");
					fprintf(out, "\tidivl\t-%d(%%ebp)\n", arg2_var->location);
					break;
				case MCC_AST_BINARY_OP_LESS:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsetl\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_GREATER:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsetg\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_LESS_EQUALS:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsetle\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_GREATER_EQUALS:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsetge\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_AND:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tandl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					break;
				case MCC_AST_BINARY_OP_OR:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\torl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					break;
				case MCC_AST_BINARY_OP_EQUALS:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsete\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				case MCC_AST_BINARY_OP_NOT_EQUALS:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n",
					        arg1_var->location);
					fprintf(out, "\tcmpl\t-%d(%%ebp), %%eax\n",
					        arg2_var->location);
					fprintf(out, "\tsetne\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					break;
				}
				fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n", arg0_var->location);
				break;
			case MCC_AST_TYPE_FLOAT: {
				switch (p->type_double.op.op) {
				case MCC_AST_BINARY_OP_ADD:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfadds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n", arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_SUB:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfsubs\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n", arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_MUL:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfmuls\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n", arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_DIV:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfdivs\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n", arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_LESS:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfxch\t%%st(1)\n");
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tseta\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_GREATER:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfxch\t%%st(1)\n");
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tseta\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_LESS_EQUALS:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tfxch\t%%st(1)\n");
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tsetnb\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_GREATER_EQUALS:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfxch\t%%st(1)\n");
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tsetnb\t%%al\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_AND:
				case MCC_AST_BINARY_OP_OR:
					fprintf(stderr, "NO AND/OR on floats\n");
					break;
				case MCC_AST_BINARY_OP_EQUALS:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tsetnp\t%%al\n");
					fprintf(out, "\tmovl\t$0, %%edx\n");
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tcmovne\t%%edx, %%eax\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				case MCC_AST_BINARY_OP_NOT_EQUALS:
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tsetp\t%%al\n");
					fprintf(out, "\tmovl\t$1, %%edx\n");
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg1_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", arg2_var->location);
					fprintf(out, "\tfucomip\t%%st(1), %%st\n");
					fprintf(out, "\tfstp\t%%st(0)\n");
					fprintf(out, "\tcmovne\t%%edx, %%eax\n");
					fprintf(out, "\tmovzbl\t%%al, %%eax\n");
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n",
					        arg0_var->location);
					break;
				}
				break;
			}
			case MCC_AST_TYPE_STRING:
				fprintf(stderr, "NO OP on strings\n");
				break;
			case MCC_AST_TYPE_VOID: fprintf(stderr, "NO OP on voids\n"); break;
			}
			break;
		}

		case TAC_LINE_TYPE_CALL: break;
		case TAC_LINE_TYPE_POP: break;
		case TAC_LINE_TYPE_PUSH: break;
		case TAC_LINE_TYPE_RETURN: break;
		case TAC_LINE_TYPE_IFZ: {
			struct mCc_ass_function_var *arg_var;
			if (p->type_ifz.var.depth != -1) {
				char buff[20];
				sprintf(buff, "%s_%d", p->type_ifz.var.val,
				        p->type_ifz.var.depth);
				HASH_FIND_STR(function_data->data, buff, arg_var);
			} else {
				HASH_FIND_STR(function_data->data, p->type_ifz.var.val,
				              arg_var);
			}
			if (arg_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG 1 %s\n",
				        p->type_ifz.var.val);
			}
			fprintf(out, "\tcmpl\t$0, -%d(%%ebp)\n", arg_var->location);
			fprintf(out, "\tje\t.L%d\n", p->type_label.label_name);
			break;
		}

		case TAC_LINE_TYPE_DECL_ARRAY: break;
		case TAC_LINE_TYPE_IDEN_ARRAY: {
			struct mCc_ass_function_var *arr_var;
			struct mCc_ass_function_var *index_var;
			struct mCc_ass_function_var *var_var;
			char buff[20];
			sprintf(buff, "%s_%d", p->type_array_iden.arr.val,
					p->type_array_iden.arr.depth);
			HASH_FIND_STR(function_data->data, buff, arr_var);

			HASH_FIND_STR(function_data->data, p->type_array_iden.var.val,
						  var_var);

			HASH_FIND_STR(function_data->data, p->type_array_iden.loc.val,
						  index_var);

			if (arr_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG arr %s\n",
						p->type_array_iden.arr.val);
			}
			if (var_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG var %s\n",
						p->type_array_iden.var.val);
			}
			if (index_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG loc %s\n",
						p->type_array_iden.loc.val);
			}


			switch (p->type_array_iden.var.type) {
				case MCC_AST_TYPE_BOOL:
				case MCC_AST_TYPE_INT:
				case MCC_AST_TYPE_STRING:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n", index_var->location);
					fprintf(out, "\tmovl\t-%d(%%ebp,%%eax,4), %%eax\n", arr_var->location);
					fprintf(out, "\tmovl\t%%eax, -%d(%%ebp)\n", var_var->location);
					break;
				case MCC_AST_TYPE_FLOAT:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n", index_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp,%%eax,4)\n", arr_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp)\n",var_var->location);
					break;
				case MCC_AST_TYPE_VOID:
					fprintf(stderr, "VOID ARRAY? \n");
					break;
			}
			break;
		}

		case TAC_LINE_TYPE_ASSIGNMENT_ARRAY:{
			struct mCc_ass_function_var *arr_var;
			struct mCc_ass_function_var *index_var;
			struct mCc_ass_function_var *var_var;
			char buff[20];
			sprintf(buff, "%s_%d", p->type_assign_array.arr.val,
					p->type_assign_array.arr.depth);
			HASH_FIND_STR(function_data->data, buff, arr_var);

			HASH_FIND_STR(function_data->data, p->type_assign_array.var.val,
						  var_var);

			HASH_FIND_STR(function_data->data, p->type_assign_array.loc.val,
						  index_var);

			if (arr_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG arr %s\n",
						p->type_assign_array.arr.val);
			}
			if (var_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG var %s\n",
						p->type_assign_array.var.val);
			}
			if (index_var == NULL) {
				fprintf(stderr, "SOMETHING WENT WRONG loc %s\n",
						p->type_assign_array.loc.val);
			}

			switch (
					p->type_assign_array.arr.type) {
				case MCC_AST_TYPE_BOOL:
				case MCC_AST_TYPE_INT:
				case MCC_AST_TYPE_STRING:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n", index_var->location);
					fprintf(out, "\tmovl\t-%d(%%ebp), %%edx\n", var_var->location);
					fprintf(out, "\tmovl\t%%edx, -%d(%%ebp,%%eax,4)\n",arr_var->location);
					break;
				case MCC_AST_TYPE_FLOAT:
					fprintf(out, "\tmovl\t-%d(%%ebp), %%eax\n", index_var->location);
					fprintf(out, "\tflds\t-%d(%%ebp)\n", var_var->location);
					fprintf(out, "\tfstps\t-%d(%%ebp,%%eax,4)\n",arr_var->location);
					break;
				case MCC_AST_TYPE_VOID:
					fprintf(stderr, "VOID ARRAY? \n");
					break;
			}
			break;
		}

		case TAC_LINE_TYPE_LABEL:
			fprintf(out, ".L%d:\n", p->type_label.label_name);
			break;
		case TAC_LINE_TYPE_JUMP:
			fprintf(out, "\tjmo .L%d\n", p->type_jump.jump_name);
			break;
		case TAC_LINE_TYPE_LABELFUNC_END:
			fprintf(out, ".LR%d:\n", funcRetLabel);
			fprintf(out, "\tleave\n\tret\n");
			fprintf(out, "\t.size\t%s, .-%s\n",
			        p->type_label_func_end.func_name,
			        p->type_label_func.func_name);

			func_data_temp = func_data_temp->next;
			break;
		case TAC_LINE_TYPE_BEGIN: break;
		}
		p = p->next;
	}

	if (HASH_COUNT(ass->floats) > 0) {
		fprintf(out, "\t.section\t.rodata\n");
	}
	print_static_data_table(ass->floats, out);

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
