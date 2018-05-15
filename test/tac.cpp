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

TEST(ThreeAdressCode, Generate_Expression_Bool)
{
	const char input[] = "true || false";
	auto result = mCc_parser_parse_string(input);

	auto tac = mCc_ast_get_tac_expression(result.expression);
	mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next;
	auto line5 = line4->next;

	ASSERT_EQ(line1->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line2->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line3->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line4->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line5->type, TAC_LINE_TYPE_DOUBLE);

	ASSERT_EQ(line1->type_simple.arg0.type, line1->type_simple.arg1.type);
	ASSERT_EQ(line2->type_simple.arg0.type, line2->type_simple.arg1.type);
	ASSERT_EQ(line3->type_simple.arg0.type, line3->type_simple.arg1.type);
	ASSERT_EQ(line4->type_simple.arg0.type, line4->type_simple.arg1.type);

	ASSERT_EQ(line5->type_double.arg0.type, MCC_AST_TYPE_BOOL);
	ASSERT_EQ(line5->type_double.op.op, MCC_AST_BINARY_OP_OR);

	// fprintf(stderr, "\n");
	mCc_tac_delete_tac(tac);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, Generate_Expression_Parenth)
{
	const char input[] = "1.2+2*(1-2)";
	auto result = mCc_parser_parse_string(input);

	auto tac = mCc_ast_get_tac_expression(result.expression);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next;
	auto line5 = line4->next;
	auto line6 = line5->next;
	auto line7 = line6->next;
	auto line8 = line7->next;
	auto line9 = line8->next;
	auto line10 = line9->next;
	auto line11 = line10->next;
	auto line12 = line11->next;

	ASSERT_EQ(line1->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line2->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line3->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line4->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line5->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line6->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line7->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line8->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line9->type, TAC_LINE_TYPE_DOUBLE);
	ASSERT_EQ(line10->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line11->type, TAC_LINE_TYPE_DOUBLE);
	ASSERT_EQ(line12->type, TAC_LINE_TYPE_DOUBLE);

	ASSERT_EQ(line1->type_simple.arg0.type, line1->type_simple.arg1.type);
	ASSERT_EQ(line2->type_simple.arg0.type, line2->type_simple.arg1.type);
	ASSERT_EQ(line3->type_simple.arg0.type, line3->type_simple.arg1.type);
	ASSERT_EQ(line4->type_simple.arg0.type, line4->type_simple.arg1.type);
	ASSERT_EQ(line5->type_simple.arg0.type, line5->type_simple.arg1.type);
	ASSERT_EQ(line6->type_simple.arg0.type, line6->type_simple.arg1.type);
	ASSERT_EQ(line7->type_simple.arg0.type, line7->type_simple.arg1.type);
	ASSERT_EQ(line8->type_simple.arg0.type, line8->type_simple.arg1.type);
	ASSERT_EQ(line10->type_simple.arg0.type, line10->type_simple.arg1.type);

	ASSERT_EQ(line9->type_double.arg0.type, line9->type_double.op.type);
	ASSERT_EQ(line11->type_double.arg0.type, line11->type_double.op.type);
	ASSERT_EQ(line12->type_double.arg0.type, line12->type_double.op.type);

	ASSERT_EQ(line9->type_double.op.op, MCC_AST_BINARY_OP_SUB);
	ASSERT_EQ(line11->type_double.op.op, MCC_AST_BINARY_OP_MUL);
	ASSERT_EQ(line12->type_double.op.op, MCC_AST_BINARY_OP_ADD);

	// fprintf(stderr, "\n");
    mCc_tac_delete_tac(tac);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, Generate_IF)
{
	const char input[] = "void fun(){if(true){}}";
	auto result = mCc_parser_parse_string(input);
	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_statement(
	    result.program->function_def_list->function_def->compound_stmt
	        ->statement_list->statement);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_IFZ, tac->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABEL, tac->next->next->next->next->type);

	auto gotoLabel = tac->next->next->next->type_ifz.jump_label_name;
	auto label = tac->next->next->next->next->type_label.label_name;

	ASSERT_EQ(gotoLabel, label);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_IFELSE)
{
	const char input[] = "void fun(){if(true){}else{}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_statement(
	    result.program->function_def_list->function_def->compound_stmt
	        ->statement_list->statement);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_IFZ, tac->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_JUMP, tac->next->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABEL, tac->next->next->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABEL,
	          tac->next->next->next->next->next->next->type);

	auto gotoIF = tac->next->next->next->type_ifz.jump_label_name;
	auto labelIF = tac->next->next->next->next->next->type_label.label_name;
	auto gotoELSE = tac->next->next->next->next->type_jump.jump_name;
	auto labelELSE =
	    tac->next->next->next->next->next->next->type_label.label_name;

	ASSERT_EQ(gotoIF, labelIF);
	ASSERT_EQ(gotoELSE, labelELSE);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_WHILE)
{
	const char input[] = "void fun(){while(true){}}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_statement(
	    result.program->function_def_list->function_def->compound_stmt
	        ->statement_list->statement);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABEL, tac->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_SIMPLE, tac->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_IFZ, tac->next->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_JUMP, tac->next->next->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABEL,
	          tac->next->next->next->next->next->next->type);

	auto beginLabel = tac->next->type_label.label_name;
	auto endLabel =
	    tac->next->next->next->next->next->next->type_label.label_name;

	auto gotoBeginning = tac->next->next->next->next->next->type_jump.jump_name;
	auto ifZGotoEnd = tac->next->next->next->next->type_ifz.jump_label_name;

	ASSERT_EQ(beginLabel, gotoBeginning);
	ASSERT_EQ(endLabel, ifZGotoEnd);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, FUNCTION_DEF)
{
	const char input[] = "int fun(){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABELFUNC, tac->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_LABELFUNC_END, tac->next->next->type);

	auto funNameStart = tac->next->type_label_func.func_name;
	auto funNameEnd = tac->next->next->type_label_func_end.func_name;

	ASSERT_EQ(funNameStart, funNameEnd);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, FUNCTION_DEF_WITH_PARAMS)
{
	const char input[] = "int fun(int a, string b){}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_POP, tac->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_POP, tac->next->next->next->type);

	auto popStr = tac->next->next->type_pop;
	auto popInt = tac->next->next->next->type_pop;

	ASSERT_EQ(popInt.var.type, MCC_AST_TYPE_INT);
	ASSERT_EQ(popStr.var.type, MCC_AST_TYPE_STRING);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, FUNCTION_CALL)
{
	const char input[] = "void foo(){} int main(){foo();}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);
	ASSERT_EQ(TAC_LINE_TYPE_CALL, tac->next->next->next->next->type);

	auto callFun = tac->next->next->next->next->type_call.name;

	ASSERT_STREQ(callFun, "foo");

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, FUNCTION_RETURN)
{
	const char input[] = "int foo(){return 1;} int main(){int a; a=foo();}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next; // return
	auto line5 = line4->next;
	auto line6 = line5->next;
	auto line7 = line6->next;
	auto line8 = line7->next; // pop return

	ASSERT_EQ(TAC_LINE_TYPE_RETURN, line4->type);
	ASSERT_EQ(TAC_LINE_TYPE_POP, line8->type);

	ASSERT_EQ(line8->type_pop.var.type, line4->type_return.var.type);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, FUNCTION_CALL_WITH_PARAMS)
{
	const char input[] =
	    "void foo(int a, string b){} int main(){foo(1,\"abc\");}";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	ASSERT_EQ(TAC_LINE_TYPE_BEGIN, tac->type);

	ASSERT_EQ(TAC_LINE_TYPE_POP, tac->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_POP, tac->next->next->next->type);

	ASSERT_EQ(TAC_LINE_TYPE_PUSH,
	          tac->next->next->next->next->next->next->next->next->type);
	ASSERT_EQ(TAC_LINE_TYPE_PUSH, tac->next->next->next->next->next->next->next
	                                  ->next->next->next->next->type);

	auto pushI =
	    tac->next->next->next->next->next->next->next->next->type_push.var;
	auto pushS = tac->next->next->next->next->next->next->next->next->next->next
	                 ->next->type_push.var;
	auto popI = tac->next->next->next->type_pop.var;
	auto popS = tac->next->next->type_pop.var;

	ASSERT_EQ(pushI.type, popI.type);
	ASSERT_EQ(pushS.type, popS.type);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_ARRAY_DEF)
{
	const char input[] = "void fun(){int[5] arr;}";
	auto result = mCc_parser_parse_string(input);
	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	auto line1 = tac->next;
	auto line2 = line1->next; // array_def

	ASSERT_EQ(line2->type, TAC_LINE_TYPE_DECL_ARRAY);

	ASSERT_EQ(line2->type_decl_array.var.array, 5);
	ASSERT_EQ(line2->type_decl_array.var.type, MCC_AST_TYPE_INT);
	ASSERT_STREQ(line2->type_decl_array.var.val, "arr");

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_ARRAY_ASS)
{
	const char input[] = "void fun(){int[5] arr; arr[4]=1;}";
	auto result = mCc_parser_parse_string(input);
	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next;
	auto line5 = line4->next;
	auto line6 = line5->next;
	auto line7 = line6->next;

	ASSERT_EQ(line7->type, TAC_LINE_TYPE_ASSIGNMENT_ARRAY);
	ASSERT_EQ(line7->type_assign_array.arr.array, 5);
	ASSERT_EQ(line7->type_assign_array.loc.type, MCC_AST_TYPE_INT);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_ARRAY_IDEN)
{
	const char input[] = "void fun(){int[5] arr; int a; a=arr[4];}";
	auto result = mCc_parser_parse_string(input);
	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next;
	auto line5 = line4->next;
	auto line6 = line5->next;

	ASSERT_EQ(line6->type, TAC_LINE_TYPE_IDEN_ARRAY);
	ASSERT_EQ(line6->type_array_iden.arr.array, 5);
	ASSERT_EQ(line6->type_assign_array.loc.type, MCC_AST_TYPE_INT);
	ASSERT_EQ(line6->type_assign_array.arr.type,
	          line6->type_assign_array.var.type);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
	// fprintf(stderr, "\n");
}

TEST(ThreeAdressCode, Generate_FLOAT_OP)
{
	const char input[] = "void main() {float a; a = 3.0; a = a * 2.0;}";
	auto result = mCc_parser_parse_string(input);
	if (result.parse_error.is_error) {
		print_error(result.parse_error);
	}
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto tac = mCc_ast_get_tac_program(result.program);
	// mCc_tac_print_tac(tac, stderr);

	auto line1 = tac->next;
	auto line2 = line1->next;
	auto line3 = line2->next;
	auto line4 = line3->next;
	auto line5 = line4->next;
	auto line6 = line5->next;
	auto line7 = line6->next;
	auto line8 = line7->next;
	auto line9 = line8->next;

	ASSERT_EQ(line2->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line2->type_simple.arg0.type, MCC_AST_TYPE_FLOAT);
	ASSERT_EQ(line2->type_simple.arg1.type, MCC_AST_TYPE_FLOAT);

	ASSERT_EQ(line3->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line3->type_simple.arg0.type, MCC_AST_TYPE_FLOAT);
	ASSERT_EQ(line3->type_simple.arg1.type, MCC_AST_TYPE_FLOAT);

	ASSERT_EQ(line4->type, TAC_LINE_TYPE_SIMPLE);
	ASSERT_EQ(line4->type_simple.arg0.type, MCC_AST_TYPE_FLOAT);
	ASSERT_EQ(line4->type_simple.arg1.type, MCC_AST_TYPE_FLOAT);

	ASSERT_EQ(line9->type, TAC_LINE_TYPE_DOUBLE);
	ASSERT_EQ(line9->type_double.arg0.type, MCC_AST_TYPE_FLOAT);
	ASSERT_EQ(line9->type_double.arg1.type, MCC_AST_TYPE_FLOAT);
	ASSERT_EQ(line9->type_double.arg2.type, MCC_AST_TYPE_FLOAT);

	mCc_tac_delete_tac(tac);
	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_literal_int)
{
	const char input[] = "3";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto lit = mCc_tac_cgen_literal(result.expression->literal, NULL);
	ASSERT_EQ(lit.type, MCC_AST_TYPE_INT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_literal_float)
{
	const char input[] = "3.2";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto lit = mCc_tac_cgen_literal(result.expression->literal, NULL);
	ASSERT_EQ(lit.type, MCC_AST_TYPE_FLOAT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_literal_string)
{
	const char input[] = "\"hello\"";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto lit = mCc_tac_cgen_literal(result.expression->literal, NULL);
	ASSERT_EQ(lit.type, MCC_AST_TYPE_STRING);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_literal_bool)
{
	const char input[] = "true";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto lit = mCc_tac_cgen_literal(result.expression->literal, NULL);
	ASSERT_EQ(lit.type, MCC_AST_TYPE_BOOL);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_int)
{
	const char input[] = "1 + 2";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_INT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_float)
{
	const char input[] = "1.1 + 2.2";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_FLOAT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_bool)
{
	const char input[] = "true && false";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_BOOL);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_parenth)
{
	const char input[] = "(1)";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_INT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_negation)
{
	const char input[] = "-1";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_INT);

	mCc_parser_delete_result(&result);
}

TEST(ThreeAdressCode, unit_cgen_expression_not)
{
	const char input[] = "!true";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = mCc_tac_cgen_expression(result.expression, NULL);
	ASSERT_EQ(expr.type, MCC_AST_TYPE_BOOL);

	mCc_parser_delete_result(&result);
}
