/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_MCC_PARSER_MCC_SHA_PARSER_TAB_H_INCLUDED
# define YY_MCC_PARSER_MCC_SHA_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef MCC_PARSER_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define MCC_PARSER_DEBUG 1
#  else
#   define MCC_PARSER_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define MCC_PARSER_DEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined MCC_PARSER_DEBUG */
#if MCC_PARSER_DEBUG
extern int mCc_parser_debug;
#endif
/* "%code requires" blocks.  */
#line 10 "../src/parser.y" /* yacc.c:1909  */

#include "mCc/parser.h"

#line 56 "mCc@sha/parser.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef MCC_PARSER_TOKENTYPE
# define MCC_PARSER_TOKENTYPE
  enum mCc_parser_tokentype
  {
    TK_END = 0,
    TK_INT_LITERAL = 258,
    TK_FLOAT_LITERAL = 259,
    TK_LPARENTH = 260,
    TK_RPARENTH = 261,
    TK_PLUS = 262,
    TK_MINUS = 263,
    TK_ASTER = 264,
    TK_SLASH = 265
  };
#endif

/* Value type.  */
#if ! defined MCC_PARSER_STYPE && ! defined MCC_PARSER_STYPE_IS_DECLARED

union MCC_PARSER_STYPE
{

  /* "float literal"  */
  double TK_FLOAT_LITERAL;
  /* binary_op  */
  enum mCc_ast_binary_op TK_binary_op;
  /* "integer literal"  */
  long TK_INT_LITERAL;
  /* single_expr  */
  struct mCc_ast_expression * TK_single_expr;
  /* expression  */
  struct mCc_ast_expression * TK_expression;
  /* literal  */
  struct mCc_ast_literal * TK_literal;
#line 93 "mCc@sha/parser.tab.h" /* yacc.c:1909  */
};

typedef union MCC_PARSER_STYPE MCC_PARSER_STYPE;
# define MCC_PARSER_STYPE_IS_TRIVIAL 1
# define MCC_PARSER_STYPE_IS_DECLARED 1
#endif



int mCc_parser_parse (void *scanner, struct mCc_ast_expression** result);

#endif /* !YY_MCC_PARSER_MCC_SHA_PARSER_TAB_H_INCLUDED  */
