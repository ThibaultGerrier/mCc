#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(Parser, BinaryOp_1)
{
	const char input[] = "192 + 3.14";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	ASSERT_EQ(192, expr->lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, expr->rhs->literal->type);
	ASSERT_EQ(3.14, expr->rhs->literal->f_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, NestedExpression_1)
{
	const char input[] = "42 * (192 + 3.14)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	ASSERT_EQ(42, expr->lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->rhs->type);

	auto subexpr = expr->rhs->expression;

	// subexpr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, subexpr->op);

	// subexpr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->lhs->type);

	// subexpr -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->lhs->literal->type);
	ASSERT_EQ(192, subexpr->lhs->literal->i_value);

	// subexpr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->rhs->type);

	// subexpr -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, subexpr->rhs->literal->type);
	ASSERT_EQ(3.14, subexpr->rhs->literal->f_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, MissingClosingParenthesis_1)
{
	const char input[] = "(42";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}

TEST(Parser, WrongOperatorTest)
{
	const char input[] = "8 . 8";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}

TEST(Parser, ComplexMathExpressionTest)
{
	const char input[] = "((1 + 8) * !(8 == 8))";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
}
TEST(Parser, UnaryOperatorTest)
{
	const char input[] = "!42";
	auto result = mCc_parser_parse_string(input);

	auto expr = result.expression;

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->type);
	ASSERT_EQ(1, expr->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);
	ASSERT_EQ(2, expr->rhs->literal->node.sloc.start_col);

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

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->expression->lhs->literal->type);
	ASSERT_EQ(2, expr->expression->lhs->literal->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->expression->rhs->literal->type);
	ASSERT_EQ(7, expr->expression->rhs->literal->node.sloc.start_col);
}
