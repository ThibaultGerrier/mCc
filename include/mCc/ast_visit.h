#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mCc/ast.h"
#include "mCc/error_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_ast_visit_traversal {
	MCC_AST_VISIT_DEPTH_FIRST,
	/* TODO: MCC_AST_VISIT_BREADTH_FIRST, */
};

enum mCc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
	MCC_AST_VISIT_PRE_AND_POST_ORDER,
};

enum mCc_ast_visit_type {
	MCC_AST_VISIT_BEFORE,
	MCC_AST_VISIT_AFTER,
	MCC_VISIT_NO_TYPE
};

/* Callbacks */

typedef void (*mCc_ast_visit_program_cb)(struct mCc_ast_program *,
                                         enum mCc_ast_visit_type,
                                         struct mCc_err_error_manager *,
                                         void *);

typedef void (*mCc_ast_visit_function_def_list_cb)(
    struct mCc_ast_function_def_list *, enum mCc_ast_visit_type,
    struct mCc_err_error_manager *, void *);

typedef void (*mCc_ast_visit_function_def_cb)(struct mCc_ast_function_def *,
                                              enum mCc_ast_visit_type,
                                              struct mCc_err_error_manager *,
                                              void *);

typedef void (*mCc_ast_visit_parameter_cb)(struct mCc_ast_parameter *,
                                           enum mCc_ast_visit_type,
                                           struct mCc_err_error_manager *,
                                           void *);

typedef void (*mCc_ast_visit_argument_list_cb)(struct mCc_ast_argument_list *,
                                               enum mCc_ast_visit_type,
                                               struct mCc_err_error_manager *,
                                               void *);

typedef void (*mCc_ast_visit_declaration_cb)(struct mCc_ast_declaration *,
                                             enum mCc_ast_visit_type,
                                             struct mCc_err_error_manager *,
                                             void *);

typedef void (*mCc_ast_visit_assignment_cb)(struct mCc_ast_assignment *,
                                            enum mCc_ast_visit_type,
                                            struct mCc_err_error_manager *,
                                            void *);

typedef void (*mCc_ast_visit_type_cb)(enum mCc_ast_type *,
                                      enum mCc_ast_visit_type,
                                      struct mCc_err_error_manager *, void *);

typedef void (*mCc_ast_visit_expression_cb)(struct mCc_ast_expression *,
                                            enum mCc_ast_visit_type,
                                            struct mCc_err_error_manager *,
                                            void *);

typedef void (*mCc_ast_visit_statement_list_cb)(struct mCc_ast_statement_list *,
                                                enum mCc_ast_visit_type,
                                                struct mCc_err_error_manager *,
                                                void *);

typedef void (*mCc_ast_visit_statement_cb)(struct mCc_ast_statement *,
                                           enum mCc_ast_visit_type,
                                           struct mCc_err_error_manager *,
                                           void *);

typedef void (*mCc_ast_visit_literal_cb)(struct mCc_ast_literal *,
                                         enum mCc_ast_visit_type,
                                         struct mCc_err_error_manager *,
                                         void *);

typedef void (*mCc_ast_visit_identifier_cb)(struct mCc_ast_identifier *,
                                            enum mCc_ast_visit_type,
                                            struct mCc_err_error_manager *,
                                            void *);

struct mCc_ast_visitor {
	enum mCc_ast_visit_traversal traversal;
	enum mCc_ast_visit_order order;

	struct mCc_err_error_manager *error_manager;

	void *userdata;

	mCc_ast_visit_program_cb program;

	mCc_ast_visit_type_cb type;

	mCc_ast_visit_function_def_list_cb function_def_list;
	mCc_ast_visit_function_def_cb function_def;

	mCc_ast_visit_parameter_cb parameter;

	mCc_ast_visit_argument_list_cb argument_list;

	mCc_ast_visit_declaration_cb declaration;

	mCc_ast_visit_assignment_cb assignment;

	mCc_ast_visit_expression_cb expression;
	mCc_ast_visit_expression_cb expression_identifier;
	mCc_ast_visit_expression_cb expression_array_identifier;
	mCc_ast_visit_expression_cb expression_call_expr;
	mCc_ast_visit_expression_cb expression_literal;
	mCc_ast_visit_expression_cb expression_binary_op;
	mCc_ast_visit_expression_cb expression_unary_op;
	mCc_ast_visit_expression_cb expression_parenth;

	mCc_ast_visit_statement_list_cb statement_list;
	mCc_ast_visit_statement_cb statement;
	mCc_ast_visit_statement_cb statement_if;
	mCc_ast_visit_statement_cb statement_while;
	mCc_ast_visit_statement_cb statement_return;
	mCc_ast_visit_statement_cb statement_expression;
	mCc_ast_visit_statement_cb statement_assignment;
	mCc_ast_visit_statement_cb statement_declaration;
	mCc_ast_visit_statement_cb statement_compound_stmt;

	mCc_ast_visit_identifier_cb identifier;

	mCc_ast_visit_literal_cb literal;
	mCc_ast_visit_literal_cb literal_bool;
	mCc_ast_visit_literal_cb literal_int;
	mCc_ast_visit_literal_cb literal_float;
	mCc_ast_visit_literal_cb literal_string;
};

void mCc_ast_visit_program(struct mCc_ast_program *program,
                           struct mCc_ast_visitor *visitor);

void mCc_ast_visit_type(enum mCc_ast_type *type,
                        struct mCc_ast_visitor *visitor);

void mCc_ast_visit_function_def_list(
    struct mCc_ast_function_def_list *function_def_list,
    struct mCc_ast_visitor *visitor);

void mCc_ast_visit_function_def(struct mCc_ast_function_def *function_def,
                                struct mCc_ast_visitor *visitor);

void mCc_ast_visit_parameters(struct mCc_ast_parameter *parameters,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_argument_list(struct mCc_ast_argument_list *arguments,
                                 struct mCc_ast_visitor *visitor);

void mCc_ast_visit_declaration(struct mCc_ast_declaration *declaration,
                               struct mCc_ast_visitor *visitor);

void mCc_ast_visit_assignment(struct mCc_ast_assignment *assignment,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_statement_list(struct mCc_ast_statement_list *statement_list,
                                  struct mCc_ast_visitor *visitor);

void mCc_ast_visit_statement(struct mCc_ast_statement *statement,
                             struct mCc_ast_visitor *visitor);

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor);

void mCc_ast_visit_identifier(struct mCc_ast_identifier *identifier,
                              struct mCc_ast_visitor *visitor);

#ifdef __cplusplus
}
#endif

#endif
