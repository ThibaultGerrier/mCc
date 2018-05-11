#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/tac.h"

void print_error(struct mCc_parse_error parse_error)
{
	struct mCc_error_location location = parse_error.location;
	char line[10], column[10];
	if (location.first_line != location.last_line) {
		sprintf(line, "%d-%d", location.first_line, location.last_line);
	} else {
		sprintf(line, "%d", location.first_line);
	}
	if (location.first_column != location.last_column) {
		sprintf(column, "%d-%d", location.first_column, location.last_column);
	} else {
		sprintf(column, "%d", location.first_column);
	}
	fprintf(stderr, "Error (line %s, column %s): %s\n", line, column,
	        parse_error.msg);
}

TEST(Parser, BinaryOp_1)
{
	const char input[] = "192 + 3.14";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3.14, expr->binary_op.rhs->literal->f_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, NestedExpression_1)
{
	const char input[] = "42 * (192 - 3.14)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(42, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->binary_op.rhs->type);

	auto subexpr = expr->binary_op.rhs->expression;

	// subexpr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_SUB, subexpr->binary_op.op);

	// subexpr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->binary_op.lhs->type);

	// subexpr -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, subexpr->binary_op.lhs->literal->i_value);

	// subexpr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->binary_op.rhs->type);

	// subexpr -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT,
	          subexpr->binary_op.rhs->literal->type);
	ASSERT_EQ(3.14, subexpr->binary_op.rhs->literal->f_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, PrecedenceTest)
{
	const char input[] = "-42 * 192 + 3.14";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->binary_op.lhs->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->binary_op.lhs->binary_op.op);

	// root -> lhs -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP,
	          expr->binary_op.lhs->binary_op.lhs->type);
	ASSERT_EQ(MCC_AST_UNARY_OP_MINUS,
	          expr->binary_op.lhs->binary_op.lhs->unary_op.op);

	// root -> lhs -> lhs -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          expr->binary_op.lhs->binary_op.lhs->unary_op.rhs->type);

	// root -> lhs -> lhs -> rhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          expr->binary_op.lhs->binary_op.lhs->unary_op.rhs->literal->type);
	ASSERT_EQ(
	    42, expr->binary_op.lhs->binary_op.lhs->unary_op.rhs->literal->i_value);

	// root -> lhs ->rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          expr->binary_op.lhs->binary_op.rhs->type);

	// root -> lhs -> rhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          expr->binary_op.lhs->binary_op.rhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->binary_op.rhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3.14, expr->binary_op.rhs->literal->f_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, MissingClosingParenthesis_1)
{
	const char input[] = "(42";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
	mCc_parser_delete_result(&result);
}

TEST(Parser, WrongOperatorTest)
{
	const char input[] = "8 . 8";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
	mCc_parser_delete_result(&result);
}

TEST(Parser, ComplexMathExpressionTest)
{
	const char input[] = "((1 + 8) / (8 == 8))";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root -> paren
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->type);

	// root -> paren -> expr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->expression->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_DIV, expr->expression->binary_op.op);

	// root -> paren -> expr -> lhs
	auto sub_expr_lhs = expr->expression->binary_op.lhs;
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, sub_expr_lhs->type);

	// root -> paren -> expr -> lhs -> paren -> expr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	          sub_expr_lhs->expression->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, sub_expr_lhs->expression->binary_op.op);

	// root -> paren -> expr -> lhs -> paren -> expr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          sub_expr_lhs->expression->binary_op.lhs->type);

	// root -> paren -> expr -> lhs -> paren -> expr -> lhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          sub_expr_lhs->expression->binary_op.lhs->literal->type);
	ASSERT_EQ(1, sub_expr_lhs->expression->binary_op.lhs->literal->i_value);

	// root -> paren -> expr -> lhs -> paren -> expr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          sub_expr_lhs->expression->binary_op.rhs->type);

	// root -> paren -> expr -> lhs -> paren -> expr -> rhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          sub_expr_lhs->expression->binary_op.rhs->literal->type);
	ASSERT_EQ(8, sub_expr_lhs->expression->binary_op.rhs->literal->i_value);

	// root -> paren -> expr -> rhs
	auto sub_expr_rhs = expr->expression->binary_op.rhs;
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, sub_expr_rhs->type);

	// root -> paren -> expr -> rhs -> paren -> expr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	          sub_expr_rhs->expression->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_EQUALS, sub_expr_rhs->expression->binary_op.op);

	// root -> paren -> expr -> rhs -> paren -> expr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          sub_expr_rhs->expression->binary_op.lhs->type);

	// root -> paren -> expr -> rhs -> paren -> expr -> lhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          sub_expr_rhs->expression->binary_op.lhs->literal->type);
	ASSERT_EQ(8, sub_expr_rhs->expression->binary_op.lhs->literal->i_value);

	// root -> paren -> expr -> rhs -> paren -> expr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          sub_expr_rhs->expression->binary_op.rhs->type);

	// root -> paren -> expr -> rhs -> paren -> expr -> rhs ->literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          sub_expr_rhs->expression->binary_op.rhs->literal->type);
	ASSERT_EQ(8, sub_expr_rhs->expression->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, UnaryOperatorExclamationTest)
{
	const char input[] = "!42";
	auto result = mCc_parser_parse_string(input);

	auto expr = result.expression;

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->type);
	ASSERT_EQ(1, expr->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
	ASSERT_EQ(2, expr->unary_op.rhs->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
	ASSERT_EQ(2, expr->unary_op.rhs->literal->node.sloc.start_col);

	mCc_parser_delete_result(&result);
}

TEST(Parser, UnaryOperatorMinusTest)
{
	const char input[] = "-42";
	auto result = mCc_parser_parse_string(input);

	auto expr = result.expression;

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->type);
	ASSERT_EQ(1, expr->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
	ASSERT_EQ(2, expr->unary_op.rhs->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
	ASSERT_EQ(2, expr->unary_op.rhs->literal->node.sloc.start_col);

	mCc_parser_delete_result(&result);
}

TEST(Parser, BoolLiteralTest)
{
	const char valid_bool_input[] = "true";
	const char string_input[] = "\"true\"";

	auto valid_bool_result = mCc_parser_parse_string(valid_bool_input);
	auto string_result = mCc_parser_parse_string(string_input);

	auto valid_bool_expr = valid_bool_result.expression;
	auto string_expr = string_result.expression;

	ASSERT_EQ(MCC_PARSER_STATUS_OK, valid_bool_result.status);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, string_result.status);

	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, valid_bool_expr->type);
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL, valid_bool_expr->literal->type);
	ASSERT_EQ(1, valid_bool_expr->node.sloc.start_col);

	EXPECT_EQ(true, valid_bool_expr->literal->b_value);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, valid_bool_expr->type);
	EXPECT_EQ(0, strcmp(string_expr->literal->s_value, "true"));

	mCc_ast_delete_expression(valid_bool_expr);
	mCc_ast_delete_expression(string_expr);
}

TEST(Parser, SourceLocation_SingleLineColumn)
{
	const char input[] = "(42 + 192)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->type);
	ASSERT_EQ(1, expr->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->expression->type);
	ASSERT_EQ(2, expr->expression->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          expr->expression->binary_op.lhs->literal->type);
	ASSERT_EQ(2, expr->expression->binary_op.lhs->literal->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          expr->expression->binary_op.rhs->literal->type);
	ASSERT_EQ(7, expr->expression->binary_op.rhs->literal->node.sloc.start_col);

	mCc_parser_delete_result(&result);
}

TEST(Parser, IdentifierTest)
{
	const char id_input[] = "some_id123";
	const char array_input[] = "array[id[5]]";

	auto id_result = mCc_parser_parse_string(id_input);
	auto array_result = mCc_parser_parse_string(array_input);

	auto id_expr = id_result.expression;
	auto array_expr = array_result.expression;

	ASSERT_EQ(MCC_PARSER_STATUS_OK, id_result.status);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, array_result.status);

	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, id_expr->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER, array_expr->type);

	ASSERT_EQ(1, id_expr->node.sloc.start_col);
	ASSERT_EQ(10, id_expr->node.sloc.end_col);
	ASSERT_EQ(0, strcmp("some_id123", id_expr->identifier->name));
	ASSERT_EQ(0,
	          strcmp("array", array_expr->array_identifier.identifier->name));
	ASSERT_EQ(0, strcmp("id", array_expr->array_identifier.expression
	                              ->array_identifier.identifier->name));
	ASSERT_EQ(5, array_expr->array_identifier.expression->array_identifier
	                 .expression->literal->i_value);

	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, id_expr->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER, array_expr->type);
	ASSERT_EQ(1, id_expr->node.sloc.start_col);

	mCc_ast_delete_expression(id_expr);
	mCc_ast_delete_expression(array_expr);
}

TEST(Parser, EqualsTest)
{
	const char input[] = "192 == 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_EQUALS, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, NotEqualsTest)
{
	const char input[] = "192 != 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_NOT_EQUALS, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, OrTest)
{
	const char input[] = "192 || 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_OR, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, AndTest)
{
	const char input[] = "192 && 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_AND, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, GreaterEqualsTest)
{
	const char input[] = "192 >= 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_GREATER_EQUALS, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, LessEqualsTest)
{
	const char input[] = "192 <= 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LESS_EQUALS, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, GreaterTest)
{
	const char input[] = "192 > 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_GREATER, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, LessTest)
{
	const char input[] = "192 < 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LESS, expr->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr->binary_op.lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, LocigalPrecedenceTest1)
{
	const char input[] = "192 < 3 || true";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr_top = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr_top->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_OR, expr_top->binary_op.op);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_top->binary_op.rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL,
	          expr_top->binary_op.rhs->literal->type);
	ASSERT_EQ(true, expr_top->binary_op.rhs->literal->b_value);

	auto expr_lhs = expr_top->binary_op.lhs;

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr_lhs->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LESS, expr_lhs->binary_op.op);

	// root -> lhs -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_lhs->binary_op.lhs->type);

	// root -> lhs -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr_lhs->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr_lhs->binary_op.lhs->literal->i_value);

	// root -> lhs -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_lhs->binary_op.rhs->type);

	// root ->lhs -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr_lhs->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr_lhs->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, LocigalPrecedenceTest2)
{
	const char input[] = "true || 192 < 3";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr_top = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr_top->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_OR, expr_top->binary_op.op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_top->binary_op.lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL,
	          expr_top->binary_op.lhs->literal->type);
	ASSERT_EQ(true, expr_top->binary_op.lhs->literal->b_value);

	auto expr_rhs = expr_top->binary_op.rhs;

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr_rhs->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LESS, expr_rhs->binary_op.op);

	// root -> rhs -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_rhs->binary_op.lhs->type);

	// root -> rhs -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr_rhs->binary_op.lhs->literal->type);
	ASSERT_EQ(192, expr_rhs->binary_op.lhs->literal->i_value);

	// root -> rhs -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr_rhs->binary_op.rhs->type);

	// root ->rhs -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr_rhs->binary_op.rhs->literal->type);
	ASSERT_EQ(3, expr_rhs->binary_op.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, DanglingElse)
{
	const char input[] = "int fun() {if (c1) if (c2) f1(); else f2();}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	ASSERT_EQ(MCC_AST_TYPE_INT,
	          prog->function_def_list->function_def->return_type);
	auto if1 = prog->function_def_list->function_def->compound_stmt
	               ->statement_list->statement;
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_IF, if1->type);
	ASSERT_EQ(NULL, if1->else_branch);
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_IF, if1->if_branch->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_CALL_EXPR,
	          if1->if_branch->if_branch->expression->type);
	ASSERT_EQ(
	    0,
	    strcmp("f1", if1->if_branch->if_branch->expression->identifier->name));
	ASSERT_EQ(
	    0, strcmp("f2",
	              if1->if_branch->else_branch->expression->identifier->name));
	mCc_parser_delete_result(&result);
}

TEST(Parser, SyntaxErrorTest)
{
	const char input[] =
	    "int foo() { \n int a; a = 12 1232423; \n a = a + 1; }";
	auto result = mCc_parser_parse_string(input);

	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_SYNTAX_ERROR, result.status);
	ASSERT_EQ(2, result.parse_error.location.first_line);
	ASSERT_EQ(2, result.parse_error.location.last_line);
	ASSERT_LT(1, result.parse_error.location.first_column);
	ASSERT_GT(25, result.parse_error.location.first_column);
	ASSERT_GT(25, result.parse_error.location.last_column);
	mCc_parser_delete_result(&result);
}

TEST(Parser, SyntaxErrorTestWithComment)
{
	const char input[] =
	    "int foo() { \n/*\n */ int a; a = 12 1232423; \n a = a + 1; }";
	auto result = mCc_parser_parse_string(input);

	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_SYNTAX_ERROR, result.status);
	ASSERT_EQ(3, result.parse_error.location.first_line);
	ASSERT_EQ(3, result.parse_error.location.last_line);
	ASSERT_LT(1, result.parse_error.location.first_column);
	ASSERT_GT(25, result.parse_error.location.first_column);
	ASSERT_GT(29, result.parse_error.location.last_column);
	mCc_parser_delete_result(&result);
}

TEST(Parser, Declaration_test)
{
	const char input[] = "int dec(){int a;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto statement = result.program->function_def_list->function_def
	                     ->compound_stmt->statement_list->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_DECLARATION, statement->type);
	ASSERT_EQ(MCC_AST_TYPE_INT, statement->declaration->identifier_type);
	ASSERT_EQ(
	    0, strcmp("a", statement->declaration->normal_decl.identifier->name));

	mCc_parser_delete_result(&result);
}

TEST(Parser, Assignment_test)
{
	const char input[] = "int dec(){a=5;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto statement = result.program->function_def_list->function_def
	                     ->compound_stmt->statement_list->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSIGNMENT, statement->type);
	ASSERT_EQ(0, strcmp("a", statement->assignment->identifier->name));
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          statement->assignment->normal_ass.rhs->literal->type);
	ASSERT_EQ(5, statement->assignment->normal_ass.rhs->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Function_def)
{
	const char input[] = "int def(){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	auto function = prog->function_def_list->function_def;

	ASSERT_EQ(MCC_AST_TYPE_INT, function->return_type);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Function_parameters)
{
	const char input[] = "int def(float f, bool b){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	auto function = prog->function_def_list->function_def;

	auto parameter1 = function->parameters->declaration;
	auto parameter2 = function->parameters->next->declaration;

	ASSERT_EQ(MCC_AST_TYPE_FLOAT, parameter1->identifier_type);
	ASSERT_EQ(0, strcmp("f", parameter1->normal_decl.identifier->name));

	ASSERT_EQ(MCC_AST_TYPE_BOOL, parameter2->identifier_type);
	ASSERT_EQ(0, strcmp("b", parameter2->normal_decl.identifier->name));

	mCc_parser_delete_result(&result);
}

TEST(Parser, Function_call)
{
	const char input[] = "def()";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	ASSERT_EQ(0, strcmp("def", expr->call_expr.identifier->name));

	mCc_parser_delete_result(&result);
}

TEST(Parser, Function_call_arguments)
{
	const char input[] = "def(1.2, true)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;
	auto arguments = expr->call_expr.arguments;

	auto argument1 = arguments->expression;
	auto argument2 = arguments->next->expression;

	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, argument1->literal->type);
	ASSERT_EQ(1.2, argument1->literal->f_value);

	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL, argument2->literal->type);
	ASSERT_EQ(true, argument2->literal->b_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Function_list)
{
	const char input[] = "int first(int a){} int second(int b){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	auto firstFunction = prog->function_def_list->function_def;
	auto secondFunction = prog->function_def_list->next->function_def;

	ASSERT_EQ(MCC_AST_TYPE_INT, firstFunction->return_type);

	auto parameterFirstFunction = firstFunction->parameters->declaration;

	ASSERT_EQ(MCC_AST_TYPE_INT, parameterFirstFunction->identifier_type);
	ASSERT_EQ(
	    0, strcmp("a", parameterFirstFunction->normal_decl.identifier->name));

	ASSERT_EQ(MCC_AST_TYPE_INT, secondFunction->return_type);

	auto parameterSecondFunction = secondFunction->parameters->declaration;

	ASSERT_EQ(MCC_AST_TYPE_INT, parameterSecondFunction->identifier_type);
	ASSERT_EQ(
	    0, strcmp("b", parameterSecondFunction->normal_decl.identifier->name));

	mCc_parser_delete_result(&result);
}

TEST(Parser, If_statement)
{
	const char input[] = "int fun() {if(true){}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto statement = result.program->function_def_list->function_def
	                     ->compound_stmt->statement_list->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_IF, statement->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          statement->if_condition->expression->type);

	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL,
	          statement->if_condition->literal->type);
	ASSERT_EQ(true, statement->if_condition->literal->b_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, While_statement)
{
	const char input[] = "int fun() {while(true){}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto statement = result.program->function_def_list->function_def
	                     ->compound_stmt->statement_list->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_WHILE, statement->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
	          statement->while_condition->expression->type);

	ASSERT_EQ(true, statement->while_condition->literal->b_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Return_statement)
{
	const char input[] = "int fun() {return 1;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto statement = result.program->function_def_list->function_def
	                     ->compound_stmt->statement_list->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_RETURN, statement->type);
	ASSERT_EQ(1, statement->expression->literal->i_value);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Compound_statement)
{
	const char input[] = "int compound(){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	auto function = prog->function_def_list->function_def;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_COMPOUND_STMT,
	          function->compound_stmt->type);

	mCc_parser_delete_result(&result);
}

TEST(Parser, Statement_list)
{
	const char input[] = "int statements(){int a; a=1; float b; b=1.1;}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto prog = result.program;
	auto statementList =
	    prog->function_def_list->function_def->compound_stmt->statement_list;

	auto statement1 = statementList->statement;
	auto statement2 = statementList->next->statement;
	auto statement3 = statementList->next->next->statement;
	auto statement4 = statementList->next->next->next->statement;

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_DECLARATION, statement1->type);
	ASSERT_EQ(
	    0, strcmp("a", statement1->declaration->normal_decl.identifier->name));

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSIGNMENT, statement2->type);
	ASSERT_EQ(0, strcmp("a", statement2->assignment->identifier->name));
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT,
	          statement2->assignment->normal_ass.rhs->literal->type);
	ASSERT_EQ(1, statement2->assignment->normal_ass.rhs->literal->i_value);

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_DECLARATION, statement3->type);
	ASSERT_EQ(
	    0, strcmp("b", statement3->declaration->normal_decl.identifier->name));

	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSIGNMENT, statement4->type);
	ASSERT_EQ(0, strcmp("b", statement4->assignment->identifier->name));
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT,
	          statement4->assignment->normal_ass.rhs->literal->type);
	ASSERT_EQ(1.1, statement4->assignment->normal_ass.rhs->literal->f_value);

	mCc_parser_delete_result(&result);
}