%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression** result_e}{struct mCc_ast_literal** result_l}{struct mCc_ast_statement** result_s}{struct mCc_ast_function_def** result_f}{struct mCc_ast_declaration** result_d}{struct mCc_ast_program** result}{struct mCc_parse_error* result_error}

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


%token START_PROGRAM START_TEST

/* precedence */

%left OR
%left AND
%left EQUALS NOT_EQUALS
%left GREATER_EQUALS GREATER LESS_EQUALS LESS
%left MINUS PLUS
%left ASTER SLASH
%precedence NEG_PREC
%precedence NOT_PREC

%right THEN_PREC ELSE

/* types */

%type <enum mCc_ast_type> type

%type <enum mCc_ast_type> return_type

%type <struct mCc_ast_expression *> expression single_expr call_expr
%type <struct mCc_ast_statement *> statement if_stmt compound_stmt
%type <struct mCc_ast_statement_list *> statement_list
%type <struct mCc_ast_literal *> literal

%type <struct mCc_ast_declaration *> declaration
%type <struct mCc_ast_assignment *> assignment

%type <struct mCc_ast_function_def *> function_def
%type <struct mCc_ast_function_def_list *> function_def_list
%type <struct mCc_ast_program *> program

%type <struct mCc_ast_parameter *> parameters
%type <struct mCc_ast_argument_list *> arguments
%type <int> meta_start

%start meta_start


%destructor {mCc_ast_delete_identifier($$);} ID
%destructor {mCc_ast_delete_expression($$);} expression call_expr
%destructor {mCc_ast_delete_statement($$);} statement compound_stmt
%destructor {mCc_ast_delete_statement_list($$);} statement_list
%destructor {mCc_ast_delete_literal($$);} literal
%destructor {mCc_ast_delete_declaration($$);} declaration
%destructor {mCc_ast_delete_argument_list($$);} arguments
%destructor {mCc_ast_delete_parameter($$);} parameters
%destructor {mCc_ast_delete_assignment($$);} assignment
%destructor {mCc_ast_delete_function_def($$);} function_def
%destructor {mCc_ast_delete_function_def_list($$);}  function_def_list
%destructor {mCc_ast_delete_program($$);} program

%%

type : BOOL_TYPE           { $$ = MCC_AST_TYPE_BOOL; }
	 | INT_TYPE            { $$ = MCC_AST_TYPE_INT; }
	 | FLOAT_TYPE          { $$ = MCC_AST_TYPE_FLOAT; }
	 | STRING_TYPE         { $$ = MCC_AST_TYPE_STRING; }
	 ;

expression: expression PLUS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3);                      loc($$, @1); }
		  | expression MINUS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3);                     loc($$, @1); }
		  | expression ASTER expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3);                     loc($$, @1); }
		  | expression SLASH expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3);                     loc($$, @1); }
		  | expression LESS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LESS, $1, $3);                     loc($$, @1); }
		  | expression GREATER expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GREATER, $1, $3);               loc($$, @1); }
		  | expression LESS_EQUALS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LESS_EQUALS, $1, $3);       loc($$, @1); }
		  | expression GREATER_EQUALS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GREATER_EQUALS, $1, $3); loc($$, @1); }
		  | expression AND expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_AND, $1, $3);                       loc($$, @1); }
		  | expression OR expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_OR, $1, $3);                         loc($$, @1); }
		  | expression EQUALS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_EQUALS, $1, $3);                 loc($$, @1); }
		  | expression NOT_EQUALS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_NOT_EQUALS, $1, $3);         loc($$, @1); }
          | single_expr                      { $$ = $1;                                                                             loc($$, @1); }

single_expr : literal                                  { $$ = mCc_ast_new_expression_literal($1);                           loc($$, @1); }
			| ID                                       { $$ = mCc_ast_new_expression_identifier($1);                        loc($$, @1); }
			| ID LBRACKET expression RBRACKET          { $$ = mCc_ast_new_expression_array_identifier($1, $3);              loc($$, @1); }
			| call_expr                                { $$ = $1;                                                           loc($$, @1); }
			| MINUS expression %prec NEG_PREC          { $$ = mCc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_MINUS, $2);  loc($$, @1); }
			| NOT expression %prec NOT_PREC            { $$ = mCc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_NOT, $2);    loc($$, @1); }
			| LPARENTH expression RPARENTH             { $$ = mCc_ast_new_expression_parenth($2);                           loc($$, @1); }
			;

literal : BOOL_LITERAL   { $$ = mCc_ast_new_literal_bool($1);   loc($$, @1); }
        | STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); loc($$, @1); }
		| INT_LITERAL    { $$ = mCc_ast_new_literal_int($1);    loc($$, @1); }
        | FLOAT_LITERAL  { $$ = mCc_ast_new_literal_float($1);  loc($$, @1); }
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

if_stmt : IF LPARENTH expression RPARENTH statement %prec THEN_PREC { $$ = mCc_ast_new_statement_if($3, $5, NULL); loc($$, @1); }
		| IF LPARENTH expression RPARENTH statement ELSE statement  { $$ = mCc_ast_new_statement_if($3, $5, $7);   loc($$, @1); }
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

program : function_def_list { $$ = mCc_ast_new_program($1); loc($$, @1);}
		;

meta_start : START_PROGRAM program { *result = $2; }
		   | START_TEST program    { *result = $2; }
		   /*| START_TEST statement  { *result_s = $2; } */
		   | START_TEST expression { *result_e = $2; }
		   ;

%%
// | expression { *result_e = $1; }

#include <assert.h>

#include "scanner.h"

extern int start_token; // kind of a hack to enable different top level rules

void mCc_parser_error(
	struct MCC_PARSER_LTYPE *yylloc,
	yyscan_t *scanner,
	struct mCc_ast_expression** result_e,
	struct mCc_ast_literal** result_l,
	struct mCc_ast_statement** result_s,
	struct mCc_ast_function_def** result_f,
	struct mCc_ast_declaration** result_d,
	struct mCc_ast_program** result,
	struct mCc_parse_error* result_error,
	const char *msg)
{
	 // suppress the warning unused parameter
	(void)scanner;
	(void)result_e;
	(void)result_l;
	(void)result_s;
	(void)result_f;
	(void)result_d;
	(void)result;
	result_error->is_error = true;
	result_error->location.first_line = yylloc->first_line;
	result_error->location.last_line = yylloc->last_line;
	result_error->location.first_column = yylloc->first_column;
	result_error->location.last_column = yylloc->last_column;
	result_error->msg = strdup(msg);
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

void mCc_parser_delete_result(struct mCc_parser_result* result) {
	if (result->expression != NULL) {
		mCc_ast_delete_expression(result->expression);
	}

	if (result->literal != NULL) {
		mCc_ast_delete_literal(result->literal);
	}

	if (result->statement != NULL) {
		mCc_ast_delete_statement(result->statement);
	}

	if (result->declaration != NULL) {
		mCc_ast_delete_declaration(result->declaration);
	}

	if (result->function_def != NULL) {
		mCc_ast_delete_function_def(result->function_def);
	}

	if (result->program != NULL) {
		mCc_ast_delete_program(result->program);
	}

	if (result->parse_error.msg != NULL) {
		free(result->parse_error.msg);
	}
}

struct mCc_parser_result mCc_parser_parse_file(FILE *input)
{
	assert(input);
	#ifdef _UNIT_TESTS__
	start_token = TK_START_TEST;
	#else
	start_token = TK_START_PROGRAM;
	#endif

	yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

	struct mCc_parser_result result = {
		.status = MCC_PARSER_STATUS_OK,
	};

	// reset is_error
	result.parse_error.is_error = false;

	if (yyparse(scanner, &result.expression, &result.literal, &result.statement,
		&result.function_def, &result.declaration, &result.program, &result.parse_error) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	if (result.parse_error.is_error)
		result.status = MCC_PARSER_STATUS_SYNTAX_ERROR;

	mCc_parser_lex_destroy(scanner);
	#ifdef _UNIT_TESTS__
	start_token = TK_START_TEST;
	#else
	start_token = TK_START_PROGRAM;
	#endif
	return result;
}
