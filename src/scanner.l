%option prefix="mCc_parser_"

%option batch
%option bison-bridge
%option bison-locations
%option noinput
%option nounput
%option noyywrap
%option reentrant
%option yylineno


%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
#define YYLTYPE MCC_PARSER_LTYPE

/* TODO:
 * - track lines
 * - take line breaks into account
 */

#define YY_USER_ACTION \
	yylloc->first_column = yylloc->last_column; \
	yylloc->last_column += yyleng; \
	yylloc->first_line=yylineno;
%}

int_literal   [0-9]+
float_literal [0-9]+\.[0-9]+
bool_literal  true|false
string_literal \"[^\"]*\"
/* string_literal \"(\\.|[^"\\])*\" */
/* string_literal  \"\.*\" */
identifier   [a-zA-Z_][a-zA-Z0-9_]*


%%

"bool"            { return TK_BOOL_TYPE; }
"int"             { return TK_INT_TYPE; }
"float"           { return TK_FLOAT_TYPE; }
"string"          { return TK_STRING_TYPE; }
"void"          { return TK_VOID_TYPE; }

"if"              { return TK_IF; }
"else"            { return TK_ELSE; }

"!"               { return TK_NOT; }
"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }

"<"               { return TK_LESS; }
">"               { return TK_GREATER; }
"<="              { return TK_LESS_EQUALS; }
">="              { return TK_GREATER_EQUALS; }
"&&"              { return TK_AND; }
"||"              { return TK_OR; }
"=="              { return TK_EQUALS; }
"!="              { return TK_NOT_EQUALS; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }

"["               { return TK_LBRACKET; }
"]"               { return TK_RBRACKET; }

"{"               { return TK_LBRACE; }
"}"               { return TK_RBRACE; }


","               { return TK_COMMA; }
";"               { return TK_SEMIKOLON; }
"="               { return TK_ASSIGNMENT; }

[ \t\r\n]+        {}


{bool_literal}    { yylval->TK_BOOL_LITERAL = strcmp(yytext, "true") == 0; return TK_BOOL_LITERAL; }

{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

{string_literal}  { yylval->TK_STRING_LITERAL = malloc(strlen(yytext) + 1);
                    strcpy(yylval->TK_STRING_LITERAL, yytext);
                    return TK_STRING_LITERAL;
                  }

{identifier}      { yylval->TK_ID = mCc_ast_new_identifier(yytext); return TK_ID; }

<<EOF>>           { return TK_END; }

.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }
