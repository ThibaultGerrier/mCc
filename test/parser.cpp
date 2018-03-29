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
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, subexpr->binary_op.op);

	// subexpr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->binary_op.lhs->type);

	// subexpr -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->binary_op.lhs->literal->type);
	ASSERT_EQ(192, subexpr->binary_op.lhs->literal->i_value);

	// subexpr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->binary_op.rhs->type);

	// subexpr -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, subexpr->binary_op.rhs->literal->type);
	ASSERT_EQ(3.14, subexpr->binary_op.rhs->literal->f_value);

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

    EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
    ASSERT_EQ(2, expr->unary_op.rhs->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->unary_op.rhs->type);
	ASSERT_EQ(2, expr->unary_op.rhs->literal->node.sloc.start_col);

	mCc_ast_delete_expression(expr);
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

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->expression->binary_op.lhs->literal->type);
	ASSERT_EQ(2, expr->expression->binary_op.lhs->literal->node.sloc.start_col);

	EXPECT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->expression->binary_op.rhs->literal->type);
	ASSERT_EQ(7, expr->expression->binary_op.rhs->literal->node.sloc.start_col);

	mCc_ast_delete_expression(expr);
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
	ASSERT_EQ(11, id_expr->node.sloc.end_col);
	ASSERT_EQ(0, strcmp("some_id123", id_expr->identifier->name));
	ASSERT_EQ(0, strcmp("array", array_expr->array_identifier.identifier->name));
	ASSERT_EQ(0, strcmp("id", array_expr->array_identifier.expression->array_identifier.identifier->name));
	ASSERT_EQ(5, array_expr->array_identifier.expression->array_identifier.expression->literal->i_value);

	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, id_expr->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_ARRAY_IDENTIFIER, array_expr->type);
	ASSERT_EQ(1, id_expr->node.sloc.start_col);

	mCc_ast_delete_expression(id_expr);
	mCc_ast_delete_expression(array_expr);
}
