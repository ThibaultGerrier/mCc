%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression** result_e}{struct mCc_ast_program** result}

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
{\
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column; \
	(ast_node)->node.sloc.end_col = (ast_sloc).last_column;\
}


struct mCc_parse_error parse_error;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

/***** identifier ******/
%token <struct mCc_ast_identifier* > ID "identifier"

/***** literals *****/

%token <bool>   BOOL_LITERAL   "bool literal"
%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <char*>   STRING_LITERAL   "string literal"

/***** types *****/

%token BOOL_TYPE "bool type"
%token INT_TYPE "int type"
%token FLOAT_TYPE "float type"
%token STRING_TYPE "string type"
%token VOID_TYPE "void type"

/***** statements *****/

%token IF "if"
%token ELSE "else"

%token WHILE "while"
%token RETURN "return"

/***** special chars *****/

%token LPARENTH "("
%token RPARENTH ")"

%token LBRACKET "["
%token RBRACKET "]"

%token LBRACE "{"
%token RBRACE "}"

%token COMMA ","

%token SEMIKOLON ";"

%token ASSIGNMENT "="

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


%type <enum mCc_ast_unary_op> unary_op

%type <enum mCc_ast_binary_op> binary_op binary_op_level_1 binary_op_level_2

%type <enum mCc_ast_type> type

%type <enum mCc_ast_type> return_type

%type <struct mCc_ast_expression *> expression single_expr call_expr single_expr_level_1 single_expr_level_2
%type <struct mCc_ast_statement *> statement if_stmt compound_stmt
%type <struct mCc_ast_statement_list *> statement_list
%type <struct mCc_ast_literal *> literal literal_num

%type <struct mCc_ast_declaration *> declaration
%type <struct mCc_ast_assignment *> assignment

%type <struct mCc_ast_function_def *> function_def
%type <struct mCc_ast_function_def_list *> function_def_list
%type <struct mCc_ast_program *> program

%type <struct mCc_ast_parameter *> parameters
%type <struct mCc_ast_argument_list *> arguments

%start program

%%

type : BOOL_TYPE           { $$ = MCC_AST_TYPE_BOOL; }
	 | INT_TYPE            { $$ = MCC_AST_TYPE_INT; }
	 | FLOAT_TYPE          { $$ = MCC_AST_TYPE_FLOAT; }
	 | STRING_TYPE         { $$ = MCC_AST_TYPE_STRING; }
	 ;

unary_op : NOT             { $$ = MCC_AST_UNARY_OP_NOT; }
         | MINUS           { $$ = MCC_AST_UNARY_OP_MINUS; }
		 ;

binary_op : LESS           { $$ = MCC_AST_BINARY_OP_LESS; }
		  | GREATER        { $$ = MCC_AST_BINARY_OP_GREATER; }
		  | LESS_EQUALS    { $$ = MCC_AST_BINARY_OP_LESS_EQUALS; }
		  | GREATER_EQUALS { $$ = MCC_AST_BINARY_OP_GREATER_EQUALS; }
		  | AND            { $$ = MCC_AST_BINARY_OP_AND; }
		  | OR             { $$ = MCC_AST_BINARY_OP_OR; }
		  | EQUALS         { $$ = MCC_AST_BINARY_OP_EQUALS; }
		  | NOT_EQUALS     { $$ = MCC_AST_BINARY_OP_NOT_EQUALS; }
		  ;

binary_op_level_1: ASTER		{ $$ = MCC_AST_BINARY_OP_MUL; }
                 | SLASH		{ $$ = MCC_AST_BINARY_OP_DIV; }
                 ;

binary_op_level_2: PLUS 		{ $$ = MCC_AST_BINARY_OP_ADD; }
                 | MINUS		{ $$ = MCC_AST_BINARY_OP_SUB; }
                 ;

expression: single_expr_level_1 binary_op expression { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
          | single_expr_level_1                      { $$ = $1;                                           loc($$, @1); }
          ;

single_expr : literal                                  { $$ = mCc_ast_new_expression_literal($1);                                                            loc($$, @1); }
			| ID                                       { $$ = mCc_ast_new_expression_identifier($1);                                                         loc($$, @1); }
			| ID LBRACKET expression RBRACKET          { $$ = mCc_ast_new_expression_array_identifier($1, $3);                                               loc($$, @1); }
			| call_expr                                { $$ = $1;                                                                                            loc($$, @1); }
            | unary_op literal_num			           { struct mCc_ast_expression * expression =  mCc_ast_new_expression_literal($2);
                                                            $$ = mCc_ast_new_expression_unary_op($1,expression);                        loc($$, @1); loc(expression, @2); }
			| unary_op expression                      { $$ = mCc_ast_new_expression_unary_op($1, $2);                                                       loc($$, @1); }
			| LPARENTH expression RPARENTH             { $$ = mCc_ast_new_expression_parenth($2);                                                            loc($$, @1); }
			;

single_expr_level_1: single_expr_level_2 binary_op_level_2 single_expr_level_1	{ $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
                   | single_expr_level_2								        { $$ = $1;                                           loc($$, @1); }
                   ;

single_expr_level_2: single_expr binary_op_level_1 single_expr_level_2 	{ $$ = mCc_ast_new_expression_binary_op($2, $1, $3); loc($$, @1); }
                   | single_expr									    { $$ = $1;                                           loc($$, @1); }
                   ;


literal_num : INT_LITERAL    { $$ = mCc_ast_new_literal_int($1);    loc($$, @1); }
            | FLOAT_LITERAL  { $$ = mCc_ast_new_literal_float($1);  loc($$, @1); }
            ;

literal : BOOL_LITERAL   { $$ = mCc_ast_new_literal_bool($1);   loc($$, @1); }
        | STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); loc($$, @1); }
        | literal_num    { $$ = $1;                             loc($$, @1); }
		;

statement : if_stmt                                      { $$ = $1;                                     loc($$, @1); }
		  | WHILE LPARENTH expression RPARENTH statement { $$ = mCc_ast_new_statement_while($3, $5);    loc($$, @1); }
		  | RETURN SEMIKOLON                             { $$ = mCc_ast_new_statement_return(NULL);     loc($$, @1); }
		  | RETURN expression SEMIKOLON                  { $$ = mCc_ast_new_statement_return($2);       loc($$, @1); }
		  | declaration SEMIKOLON                        { $$ = mCc_ast_new_statement_declaration($1);  loc($$, @1); }
		  | assignment SEMIKOLON                         { $$ = mCc_ast_new_statement_assignment($1);   loc($$, @1); }
		  | expression SEMIKOLON                         { $$ = mCc_ast_new_statement_expression($1);   loc($$, @1); }
		  | compound_stmt					             { $$ = $1;                                     loc($$, @1); }
		  ;

statement_list : statement statement_list { $$ = mCc_ast_new_statement_list($1); $$->next = $2; loc($$, @1); }
			   | statement                { $$ = mCc_ast_new_statement_list($1);                loc($$, @1); }
			   ;

if_stmt : IF LPARENTH expression RPARENTH statement                { $$ = mCc_ast_new_statement_if($3, $5, NULL); loc($$, @1); }
		| IF LPARENTH expression RPARENTH statement ELSE statement { $$ = mCc_ast_new_statement_if($3, $5, $7);   loc($$, @1); }
		;

return_type : type { $$ = $1; }
			| VOID_TYPE { $$ = MCC_AST_TYPE_VOID;}
			;

declaration : type ID                               { $$ = mCc_ast_new_declaration($1, $2);           loc($$, @1); }
			| type ID LBRACKET INT_LITERAL RBRACKET { $$ = mCc_ast_new_array_declaration($1, $2, $4); loc($$, @1); }
			;

assignment  : ID ASSIGNMENT expression									{ $$ = mCc_ast_new_assignment($1, $3);                 loc($$, @1); }
			| ID LBRACKET expression RBRACKET ASSIGNMENT expression     { $$ = mCc_ast_new_array_assignment($1, $3, $6);       loc($$, @1); }

parameters  : declaration COMMA parameters { $$ = mCc_ast_new_parameter($1); $$->next = $3; loc($$, @1); }
			| declaration                  { $$ = mCc_ast_new_parameter($1);                loc($$, @1); }
			;

call_expr : ID LPARENTH arguments RPARENTH     { $$ = mCc_ast_new_expression_call_expr($1, $3);                loc($$, @1); }
          | ID LPARENTH RPARENTH               { $$ = mCc_ast_new_expression_call_expr($1, NULL);              loc($$, @1); }
		  ;

arguments : expression COMMA arguments         { $$ = mCc_ast_new_argument_list($1); $$->next = $3; loc($$, @1); }
		  | expression                         { $$ = mCc_ast_new_argument_list($1);                loc($$, @1); }
		  ;

compound_stmt   : LBRACE statement_list RBRACE			{ $$ = mCc_ast_new_statement_compound($2);		loc($$, @1);}
				| LBRACE RBRACE		                    { $$ = mCc_ast_new_statement_compound(NULL);	loc($$, @1);}
				;

function_def : return_type ID LPARENTH parameters RPARENTH compound_stmt { $$ = mCc_ast_new_function_def($1, $2, $4, $6);   loc($$, @1);}
			 | return_type ID LPARENTH RPARENTH compound_stmt { $$ = mCc_ast_new_function_def($1, $2, NULL, $5);            loc($$, @1);}
			 ;

function_def_list : function_def function_def_list { $$ = mCc_ast_new_function_def_list($1); $$->next = $2; loc($$, @1); }
				  | function_def                   { $$ = mCc_ast_new_function_def_list($1);                loc($$, @1); }
				  ;


program : function_def_list { *result = mCc_ast_new_program($1); }
		| expression { *result_e = $1; }
		;

%%

#include <assert.h>

#include "scanner.h"

void mCc_parser_error(
    struct MCC_PARSER_LTYPE *yylloc,
    yyscan_t *scanner,
	struct mCc_ast_expression** result_e,
	struct mCc_ast_program** result,
	const char *msg)
{
    // suppress the warning unused parameter
    (void)scanner;
    (void)result_e;
    (void)result;

    // without global variable: save inside program/expression
    /*
    *result = (struct mCc_ast_program*)malloc(sizeof(struct mCc_ast_program));
    memset(*result, 0, sizeof(struct mCc_ast_program));
    (*result)->error_msg = strdup(msg);

    *result_e = (struct mCc_ast_expression*)malloc(sizeof(struct mCc_ast_expression));
    memset(*result_e, 0, sizeof(struct mCc_ast_expression));
    (*result_e)->error_msg = strdup(msg)
    */

    // save error message and location to global parse_error variable.
    parse_error.is_error = 1;
    parse_error.location.first_line = yylloc->first_line;
    parse_error.location.last_line = yylloc->last_line;
    parse_error.location.first_column = yylloc->first_column;
    parse_error.location.last_column = yylloc->last_column;
    parse_error.msg = strdup(msg);
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

    // reset is_error
    parse_error.is_error = 0;

	if (yyparse(scanner, &result.expression, &result.program) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	if (parse_error.is_error) {
	    result.status = MCC_PARSER_STATUS_SYNTAX_ERROR;
	    result.parse_error = parse_error;
	}

	mCc_parser_lex_destroy(scanner);
	return result;
}
