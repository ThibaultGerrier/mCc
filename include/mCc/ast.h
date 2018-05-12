#ifndef MCC_AST_H
#define MCC_AST_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward Declarations */
struct mCc_ast_expression;
struct mCc_ast_statement;
struct mCc_ast_literal;
struct mCc_sym_table_entry;
struct mCc_sym_table_tree;

/* ---------------------------------------------------------------- AST Node */

struct mCc_ast_source_location {
	size_t start_line;
	size_t start_col;
	size_t end_line;
	size_t end_col;
};

/* Data contained by every AST node. */
struct mCc_ast_node {
	struct mCc_ast_source_location sloc;
};

/* --------------------------------------------------------------- Identifier */

struct mCc_ast_identifier {
	struct mCc_ast_node node;

	struct mCc_sym_table_entry *symbol_table_entry;

	char *name;
};

struct mCc_ast_identifier *mCc_ast_new_identifier(const char *name);

void mCc_ast_delete_identifier(struct mCc_ast_identifier *identifier);

/* ---------------------------------------------------------------- Literals */

enum mCc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_BOOL,
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_STRING,
};

struct mCc_ast_literal {
	struct mCc_ast_node node;

	enum mCc_ast_literal_type type;
	union {
		/* MCC_AST_LITERAL_TYPE_BOOL */
		bool b_value;

		/* MCC_AST_LITERAL_TYPE_INT */
		long i_value;

		/* MCC_AST_LITERAL_TYPE_FLOAT */
		double f_value;

		/* MCC_AST_LITERAL_TYPE_STRING */
		char *s_value;
	};
};

struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value);

struct mCc_ast_literal *mCc_ast_new_literal_int(long value);

struct mCc_ast_literal *mCc_ast_new_literal_float(double value);

struct mCc_ast_literal *mCc_ast_new_literal_string(char *value);

void mCc_ast_delete_literal(struct mCc_ast_literal *literal);

/* -------------------------------------------------------------------- Types */

enum mCc_ast_type {
	MCC_AST_TYPE_BOOL,
	MCC_AST_TYPE_INT,
	MCC_AST_TYPE_FLOAT,
	MCC_AST_TYPE_STRING,
	MCC_AST_TYPE_VOID,
};

/* struct mCc_ast_type { */
/*     struct mCc_ast_node node; */
/*     enum mCc_ast_type type; */
/* }; */

/* struct mCc_ast_expression * */
/* mCc_ast_new_type(struct mCc_ast_type *type); */

/* void mCc_ast_delete_type(struct mCc_ast_type *type); */

/* --------------------------------------------------------------- Operators */

enum mCc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,
	MCC_AST_BINARY_OP_SUB,
	MCC_AST_BINARY_OP_MUL,
	MCC_AST_BINARY_OP_DIV,
	MCC_AST_BINARY_OP_LESS,
	MCC_AST_BINARY_OP_GREATER,
	MCC_AST_BINARY_OP_LESS_EQUALS,
	MCC_AST_BINARY_OP_GREATER_EQUALS,
	MCC_AST_BINARY_OP_AND,
	MCC_AST_BINARY_OP_OR,
	MCC_AST_BINARY_OP_EQUALS,
	MCC_AST_BINARY_OP_NOT_EQUALS,
};

enum mCc_ast_unary_op { MCC_AST_UNARY_OP_NOT, MCC_AST_UNARY_OP_MINUS };

/* ------------------------------------------------------------- Declaration */

enum mCc_ast_declaration_type {
	MCC_AST_DECLARATION_TYPE_DECLARATION,
	MCC_AST_DECLARATION_TYPE_ARRAY_DECLARATION
};

struct mCc_ast_declaration {
	struct mCc_ast_node node;
	enum mCc_ast_declaration_type declaration_type;
	enum mCc_ast_type identifier_type;
	union {
		struct {
			struct mCc_ast_identifier *identifier;
		} normal_decl;

		struct {
			struct mCc_ast_identifier *identifier;
			struct mCc_ast_literal *literal;
		} array_decl;
	};
};

struct mCc_ast_declaration *
mCc_ast_new_declaration(enum mCc_ast_type type,
                        struct mCc_ast_identifier *identifier);

struct mCc_ast_declaration *
mCc_ast_new_array_declaration(enum mCc_ast_type type,
                              struct mCc_ast_identifier *identifier, long size);

void mCc_ast_delete_declaration(struct mCc_ast_declaration *declaration);

/* ------------------------------------------------------------- Parameters */

struct mCc_ast_parameter {
	struct mCc_ast_node node;
	struct mCc_ast_parameter *next;
	struct mCc_ast_declaration *declaration;
};

struct mCc_ast_parameter *
mCc_ast_new_parameter(struct mCc_ast_declaration *declaration);

void mCc_ast_delete_parameter(struct mCc_ast_parameter *parameter);

/* ------------------------------------------------------------- Arguments */

struct mCc_ast_argument_list {
	struct mCc_ast_node node;
	struct mCc_ast_argument_list *next;
	struct mCc_ast_expression *expression;
};

struct mCc_ast_argument_list *
mCc_ast_new_argument_list(struct mCc_ast_expression *expression);

void mCc_ast_delete_argument_list(struct mCc_ast_argument_list *argument_list);

/* ------------------------------------------------------------- Assignment */

enum mCc_ast_assignment_type {
	MCC_AST_ASSIGNMENT_TYPE_NORMAL,
	MCC_AST_ASSIGNMENT_TYPE_ARRAY,
};

struct mCc_ast_assignment {
	struct mCc_ast_node node;
	enum mCc_ast_assignment_type type;
	struct mCc_ast_identifier *identifier;
	union {
		struct {
			struct mCc_ast_expression *rhs;
		} normal_ass;

		struct {
			struct mCc_ast_expression *index;
			struct mCc_ast_expression *rhs;
		} array_ass;
	};
};

struct mCc_ast_assignment *
mCc_ast_new_assignment(struct mCc_ast_identifier *identifier,
                       struct mCc_ast_expression *rhs);

struct mCc_ast_assignment *
mCc_ast_new_array_assignment(struct mCc_ast_identifier *identifier,
                             struct mCc_ast_expression *index,
                             struct mCc_ast_expression *rhs);

void mCc_ast_delete_assignment(struct mCc_ast_assignment *assignment);

/* ------------------------------------------------------------- Expressions */

enum mCc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_LITERAL,
	MCC_AST_EXPRESSION_TYPE_IDENTIFIER,
	MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER,
	MCC_AST_EXPRESSION_TYPE_CALL_EXPR,
	MCC_AST_EXPRESSION_TYPE_UNARY_OP,
	MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	MCC_AST_EXPRESSION_TYPE_PARENTH,
};

struct mCc_ast_expression {
	struct mCc_ast_node node;

	enum mCc_ast_expression_type type;
	union {
		/* MCC_AST_EXPRESSION_TYPE_LITERAL */
		struct mCc_ast_literal *literal;

		/* MCC_AST_EXPRESSION_TYPE_BINARY_OP */
		struct {
			enum mCc_ast_binary_op op;
			struct mCc_ast_expression *lhs; /* only binary operation */
			struct mCc_ast_expression *rhs;
		} binary_op;

		/* MCC_AST_EXPRESSION_TYPE_UNARY_OP */
		struct {
			enum mCc_ast_unary_op op;
			struct mCc_ast_expression *rhs; /* only binary operation */
		} unary_op;

		/* MCC_AST_EXPRESSION_TYPE_IDENTIFIER */
		struct mCc_ast_identifier *identifier;

		/* MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER */
		struct {
			struct mCc_ast_identifier *identifier;
			struct mCc_ast_expression *expression;
		} array_identifier;

		/* MCC_AST_EXPRESSION_TYPE_CALL_EXPR */
		struct {
			struct mCc_ast_identifier *identifier;
			struct mCc_ast_argument_list *arguments;
		} call_expr;

		/* MCC_AST_EXPRESSION_TYPE_PARENTH */
		struct mCc_ast_expression *expression;
	};
};

struct mCc_ast_expression *
mCc_ast_new_expression_identifier(struct mCc_ast_identifier *identifier);

struct mCc_ast_expression *
mCc_ast_new_expression_array_identifier(struct mCc_ast_identifier *identifier,
                                        struct mCc_ast_expression *expression);

struct mCc_ast_expression *
mCc_ast_new_expression_call_expr(struct mCc_ast_identifier *identifier,
                                 struct mCc_ast_argument_list *arguments);

struct mCc_ast_expression *
mCc_ast_new_expression_literal(struct mCc_ast_literal *literal);

struct mCc_ast_expression *
mCc_ast_new_expression_unary_op(enum mCc_ast_unary_op op,
                                struct mCc_ast_expression *rhs);

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_expression *lhs,
                                 struct mCc_ast_expression *rhs);

struct mCc_ast_expression *
mCc_ast_new_expression_parenth(struct mCc_ast_expression *expression);

void mCc_ast_delete_expression(struct mCc_ast_expression *expression);

/* ---------------------------------------------------------- Statement List */

struct mCc_ast_statement_list {
	struct mCc_ast_node node;
	struct mCc_ast_statement *statement;
	struct mCc_ast_statement_list *next;
};

struct mCc_ast_statement_list *
mCc_ast_new_statement_list(struct mCc_ast_statement *statement);

void mCc_ast_delete_statement_list(
    struct mCc_ast_statement_list *statement_list);

/* -------------------------------------------------------------- Statements */

enum mCc_ast_statement_type {
	MCC_AST_STATEMENT_TYPE_IF,
	/* MCC_AST_STATEMENT_TYPE_IF_ELSE, */
	MCC_AST_STATEMENT_TYPE_WHILE,
	MCC_AST_STATEMENT_TYPE_RETURN,
	MCC_AST_STATEMENT_TYPE_DECLARATION,
	MCC_AST_STATEMENT_TYPE_ASSIGNMENT,
	MCC_AST_STATEMENT_TYPE_ARRAY_ASSIGNMENT,
	MCC_AST_STATEMENT_TYPE_EXPRESSION,
	MCC_AST_STATEMENT_TYPE_COMPOUND_STMT
};

struct mCc_ast_statement {
	struct mCc_ast_node node;

	enum mCc_ast_statement_type type;

	union {

		/* MCC_AST_STATEMENT_TYPE_IF */
		struct {
			struct mCc_ast_expression *if_condition;
			struct mCc_ast_statement *if_branch;
			struct mCc_ast_statement *else_branch;
		};

		/* MCC_AST_STATEMENT_TYPE_WHILE */
		struct {
			struct mCc_ast_expression *while_condition;
			struct mCc_ast_statement *body;
		};

		/* MCC_AST_STATEMENT_TYPE_EXPRESSION */
		/* MCC_AST_STATEMENT_TYPE_RETURN */
		struct mCc_ast_expression *expression;

		/* MCC_AST_STATEMENT_TYPE_DECLARATION */
		struct mCc_ast_declaration *declaration;

		/* MCC_AST_STATEMENT_TYPE_ASSIGNMENT */
		struct mCc_ast_assignment *assignment;

		/* MCC_AST_STATEMENT_TYPE_COMPOUND_STMT */
		struct mCc_ast_statement_list *statement_list;
	};
};

struct mCc_ast_statement *
mCc_ast_new_statement_if(struct mCc_ast_expression *condition,
                         struct mCc_ast_statement *if_branch,
                         struct mCc_ast_statement *else_branch);

struct mCc_ast_statement *
mCc_ast_new_statement_while(struct mCc_ast_expression *condition,
                            struct mCc_ast_statement *body);

struct mCc_ast_statement *
mCc_ast_new_statement_return(struct mCc_ast_expression *expression);

struct mCc_ast_statement *
mCc_ast_new_statement_expression(struct mCc_ast_expression *expression);

struct mCc_ast_statement *
mCc_ast_new_statement_assignment(struct mCc_ast_assignment *assignment);

struct mCc_ast_statement *
mCc_ast_new_statement_declaration(struct mCc_ast_declaration *declaration);

struct mCc_ast_statement *
mCc_ast_new_statement_compound(struct mCc_ast_statement_list *statement_list);

void mCc_ast_delete_statement(struct mCc_ast_statement *statement);

/* ----------------------------------------------------- Function Definition */

struct mCc_ast_function_def {
	struct mCc_ast_node node;

	enum mCc_ast_type return_type;

	struct mCc_ast_identifier *function_identifier;

	struct mCc_ast_parameter *parameters;

	struct mCc_ast_statement *compound_stmt;
};

struct mCc_ast_function_def *
mCc_ast_new_function_def(enum mCc_ast_type return_type,
                         struct mCc_ast_identifier *function_identifier,
                         struct mCc_ast_parameter *parameters,
                         struct mCc_ast_statement *compound_stmt);

void mCc_ast_delete_function_def(struct mCc_ast_function_def *function_def);

/* ----------------------------------------------- Function Definition List */

struct mCc_ast_function_def_list {
	struct mCc_ast_node node;
	struct mCc_ast_function_def *function_def;
	struct mCc_ast_function_def_list *next;
};

struct mCc_ast_function_def_list *
mCc_ast_new_function_def_list(struct mCc_ast_function_def *function_def);

void mCc_ast_delete_function_def_list(
    struct mCc_ast_function_def_list *function_def_list);

/* ---------------------------------------------------------------- Program */

struct mCc_ast_program {
	struct mCc_ast_node node;

	struct mCc_ast_function_def_list *function_def_list;
};

struct mCc_ast_program *
mCc_ast_new_program(struct mCc_ast_function_def_list *function_def_list);

void mCc_ast_delete_program(struct mCc_ast_program *program);

#ifdef __cplusplus
}
#endif

#endif
