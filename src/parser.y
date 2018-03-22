%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mCc/parser.h"
}

%{
#include <string.h>

int mCc_parser_lex();
void mCc_parser_error();

#define loc(ast_node, ast_sloc) \
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

%token <bool>   BOOL_LITERAL   "bool literal"
%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <char*>   STRING_LITERAL   "string literal"

%token LPARENTH "("
%token RPARENTH ")"

/***** unary operations *****/

%token NOT "!"

/***** binary operations *****/

/* math operations */

%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"

/* compare operators */

%token LESS "<"
%token GREATER ">"
%token LESS_EQUALS "<="
%token GREATER_EQUALS ">="
%token AND "&&"
%token OR "||"
%token EQUALS "=="
%token NOT_EQUALS "!="


%type <enum mCc_ast_op> unary_op

%type <enum mCc_ast_op> binary_op

%type <struct mCc_ast_expression *> expression single_expr
%type <struct mCc_ast_literal *> literal

%start toplevel

%%

toplevel : expression { *result = $1; }
		 ;

unary_op :  NOT            { $$ = MCC_AST_UNARY_OP_NOT; }
         ;

binary_op : PLUS           { $$ = MCC_AST_BINARY_OP_ADD; }
		  | MINUS          { $$ = MCC_AST_BINARY_OP_SUB; }
		  | ASTER          { $$ = MCC_AST_BINARY_OP_MUL; }
		  | SLASH          { $$ = MCC_AST_BINARY_OP_DIV; }
		  | LESS           { $$ = MCC_AST_BINARY_OP_LESS; }
		  | GREATER		   { $$ = MCC_AST_BINARY_OP_GREATER; }
		  | LESS_EQUALS    { $$ = MCC_AST_BINARY_OP_LESS_EQUALS; }
		  | GREATER_EQUALS { $$ = MCC_AST_BINARY_OP_GREATER_EQUALS; }
		  | AND            { $$ = MCC_AST_BINARY_OP_AND; }
		  | OR             { $$ = MCC_AST_BINARY_OP_OR; }
		  | EQUALS         { $$ = MCC_AST_BINARY_OP_EQUALS; }
		  | NOT_EQUALS     { $$ = MCC_AST_BINARY_OP_NOT_EQUALS; }
		  ;

single_expr : literal                         { $$ = mCc_ast_new_expression_literal($1); loc($$, @1); }
			| unary_op expression             { $$ = mCc_ast_new_expression_unary_op($1, $2); loc($$, @1); }
			| LPARENTH expression RPARENTH    { $$ = mCc_ast_new_expression_parenth($2); loc($$, @1); }
			;

expression : single_expr                      { $$ = $1;                                           loc($$, @1); }
		   | single_expr binary_op expression { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
		   ;

literal : BOOL_LITERAL   { $$ = mCc_ast_new_literal_bool($1);   loc($$, @1); }
        | INT_LITERAL   { $$ = mCc_ast_new_literal_int($1);   loc($$, @1); }
		| FLOAT_LITERAL { $$ = mCc_ast_new_literal_float($1); loc($$, @1); }
		| STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); loc($$, @1); }
		;

%%

#include <assert.h>

#include "scanner.h"

void mCc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner,
					  const char *msg)
{
}

struct mCc_parser_result mCc_parser_parse_string(const char *input)
{
	assert(input);

	struct mCc_parser_result result = { 0 };

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		result.status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM;
		return result;
	}

	result = mCc_parser_parse_file(in);

	fclose(in);

	return result;
}

struct mCc_parser_result mCc_parser_parse_file(FILE *input)
{
	assert(input);

	yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

	struct mCc_parser_result result = {
		.status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.expression) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);

	return result;
}
