#include <gtest/gtest.h>

#include "mCc/assembler.h"
#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/parser.h"
#include "mCc/symbol_table.h"
#include "mCc/tac.h"
#include <cstdbool>
#include <string>

/*
 * These tests for assembler are onyl for completeness sake, we did the major
 * testing during development with the help of /doc/test_mCc/test1.mC, but this
 * is more a integration than a unit test.
 */

TEST(Assembler, TestAssemblerBasic)
{
	const char input[] = "void main(){"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected = "\t.file\t\"generated.c\"\n"
	                "\t.text\n"
	                "\t.globl\tmain\n"
	                "\t.type\tmain, @function\n"
	                "main:\n"
	                "\tpushl\t%ebp\n"
	                "\tmovl\t%esp, %ebp\n"
	                "\tsubl\t$0, %esp\n"
	                ".LR1:\n"
	                "\tleave\n"
	                "\tret\n"
	                "\t.size\tmain, .-main\n"
	                "\t.ident\t\"mCc compiler\"\n"
	                "\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerIntVar)
{
	const char input[] = "void main(){"
	                     "int a;"
	                     "a = 44;"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected = "\t.file\t\"generated.c\"\n"
	                "\t.text\n"
	                "\t.globl\tmain\n"
	                "\t.type\tmain, @function\n"
	                "main:\n"
	                "\tpushl\t%ebp\n"
	                "\tmovl\t%esp, %ebp\n"
	                "\tsubl\t$16, %esp\n"
	                "\tmovl\t$44, -4(%ebp)\n"
	                "\tmovl\t-4(%ebp), %eax\n"
	                "\tmovl\t%eax, -8(%ebp)\n"
	                ".LR2:\n"
	                "\tleave\n"
	                "\tret\n"
	                "\t.size\tmain, .-main\n"
	                "\t.ident\t\"mCc compiler\"\n"
	                "\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerFloatVar)
{
	const char input[] = "void main(){"
	                     "float a;"
	                     "a = 4.4;"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected = "\t.file\t\"generated.c\"\n"
	                "\t.text\n"
	                "\t.globl\tmain\n"
	                "\t.type\tmain, @function\n"
	                "main:\n"
	                "\tpushl\t%ebp\n"
	                "\tmovl\t%esp, %ebp\n"
	                "\tsubl\t$16, %esp\n"
	                "\tflds\t.LC3\n"
	                "\tfstps\t-4(%ebp)\n"
	                "\tflds\t-4(%ebp)\n"
	                "\tfstps\t-8(%ebp)\n"
	                ".LR4:\n"
	                "\tleave\n"
	                "\tret\n"
	                "\t.size\tmain, .-main\n"
	                "\t.section\t.rodata\n"
	                ".LC3:\n"
	                "\t.float\t4.400000\n"
	                "\t.ident\t\"mCc compiler\"\n"
	                "\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerStringVar)
{
	const char input[] = "void main(){"
	                     "string a;"
	                     "a = \"hallo\";"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected = "\t.file\t\"generated.c\"\n"
	                "\t.section\t.rodata\n"
	                ".LC5:\n"
	                "\t.string\t\"hallo\"\n"
	                "\t.text\n"
	                "\t.globl\tmain\n"
	                "\t.type\tmain, @function\n"
	                "main:\n"
	                "\tpushl\t%ebp\n"
	                "\tmovl\t%esp, %ebp\n"
	                "\tsubl\t$16, %esp\n"
	                "\tmovl\t$.LC5, -4(%ebp)\n"
	                "\tmovl\t-4(%ebp), %eax\n"
	                "\tmovl\t%eax, -8(%ebp)\n"
	                ".LR6:\n"
	                "\tleave\n"
	                "\tret\n"
	                "\t.size\tmain, .-main\n"
	                "\t.ident\t\"mCc compiler\"\n"
	                "\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerIntOps)
{
	const char input[] = "void main(){"
	                     "int a;"
	                     "int b;"
	                     "int c;"
	                     "a = 3;"
	                     "b = 4;"
	                     "c = a + b;"
	                     "c = a - b;"
	                     "c = a * b;"
	                     "c = a / b;"
	                     "c = -a;"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected =
	    "\t.file\t\"generated.c\"\n\t.text\n\t.globl\tmain\n\t.type\tmain, "
	    "@function\nmain:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$80, "
	    "%esp\n\tmovl\t$3, -4(%ebp)\n\tmovl\t-4(%ebp), %eax\n\tmovl\t%eax, "
	    "-8(%ebp)\n\tmovl\t$4, -12(%ebp)\n\tmovl\t-12(%ebp), "
	    "%eax\n\tmovl\t%eax, -16(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-20(%ebp)\n\tmovl\t-16(%ebp), %eax\n\tmovl\t%eax, "
	    "-24(%ebp)\n\tmovl\t-20(%ebp), %edx\n\tmovl\t-24(%ebp), "
	    "%eax\n\taddl\t%edx, %eax\n\tmovl\t%eax, -28(%ebp)\n\tmovl\t-28(%ebp), "
	    "%eax\n\tmovl\t%eax, -32(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-36(%ebp)\n\tmovl\t-16(%ebp), %eax\n\tmovl\t%eax, "
	    "-40(%ebp)\n\tmovl\t-36(%ebp), %eax\n\tsubl\t-40(%ebp), "
	    "%eax\n\tmovl\t%eax, -44(%ebp)\n\tmovl\t-44(%ebp), %eax\n\tmovl\t%eax, "
	    "-32(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-48(%ebp)\n\tmovl\t-16(%ebp), %eax\n\tmovl\t%eax, "
	    "-52(%ebp)\n\tmovl\t-48(%ebp), %eax\n\timull\t-52(%ebp), "
	    "%eax\n\tmovl\t%eax, -56(%ebp)\n\tmovl\t-56(%ebp), %eax\n\tmovl\t%eax, "
	    "-32(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-60(%ebp)\n\tmovl\t-16(%ebp), %eax\n\tmovl\t%eax, "
	    "-64(%ebp)\n\tmovl\t-60(%ebp), "
	    "%eax\n\tcltd\n\tidivl\t-64(%ebp)\n\tmovl\t%eax, "
	    "-68(%ebp)\n\tmovl\t-68(%ebp), %eax\n\tmovl\t%eax, "
	    "-32(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-72(%ebp)\n\tmovl\t-72(%ebp), %eax\n\tnegl\t%eax\n\tmovl\t%eax, "
	    "-76(%ebp)\n\tmovl\t-76(%ebp), %eax\n\tmovl\t%eax, "
	    "-32(%ebp)\n.LR7:\n\tleave\n\tret\n\t.size\tmain, "
	    ".-main\n\t.ident\t\"mCc "
	    "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerFloatOps)
{
	const char input[] = "void main(){"
	                     "float a;"
	                     "float b;"
	                     "float c;"
	                     "a = 3.1;"
	                     "b = 4.3;"
	                     "c = a + b;"
	                     "c = a - b;"
	                     "c = a * b;"
	                     "c = a / b;"
	                     "c = -a;"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected =
	    "\t.file\t\"generated.c\"\n\t.text\n\t.globl\tmain\n\t.type\tmain, "
	    "@function\nmain:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$80, "
	    "%esp\n\tflds\t.LC8\n\tfstps\t-4(%ebp)\n\tflds\t-4(%ebp)\n\tfstps\t-8(%"
	    "ebp)\n\tflds\t.LC9\n\tfstps\t-12(%ebp)\n\tflds\t-12(%ebp)\n\tfstps\t-"
	    "16(%ebp)\n\tflds\t-8(%ebp)\n\tfstps\t-20(%ebp)\n\tflds\t-16(%ebp)"
	    "\n\tfstps\t-24(%ebp)\n\tflds\t-20(%ebp)\n\tfadds\t-24(%ebp)"
	    "\n\tfstps\t-28(%ebp)\n\tflds\t-28(%ebp)\n\tfstps\t-32(%ebp)\n\tflds\t-"
	    "8(%ebp)\n\tfstps\t-36(%ebp)\n\tflds\t-16(%ebp)\n\tfstps\t-40(%ebp)"
	    "\n\tflds\t-36(%ebp)\n\tfsubs\t-40(%ebp)\n\tfstps\t-44(%ebp)\n\tflds\t-"
	    "44(%ebp)\n\tfstps\t-32(%ebp)\n\tflds\t-8(%ebp)\n\tfstps\t-48(%ebp)"
	    "\n\tflds\t-16(%ebp)\n\tfstps\t-52(%ebp)\n\tflds\t-48(%ebp)\n\tfmuls\t-"
	    "52(%ebp)\n\tfstps\t-56(%ebp)\n\tflds\t-56(%ebp)\n\tfstps\t-32(%ebp)"
	    "\n\tflds\t-8(%ebp)\n\tfstps\t-60(%ebp)\n\tflds\t-16(%ebp)\n\tfstps\t-"
	    "64(%ebp)\n\tflds\t-60(%ebp)\n\tfdivs\t-64(%ebp)\n\tfstps\t-68(%ebp)"
	    "\n\tflds\t-68(%ebp)\n\tfstps\t-32(%ebp)\n\tflds\t-8(%ebp)\n\tfstps\t-"
	    "72(%ebp)\n\tflds\t-72(%ebp)\n\tfchs\n\tfstps\t-76(%ebp)\n\tflds\t-76(%"
	    "ebp)\n\tfstps\t-32(%ebp)\n.LR10:\n\tleave\n\tret\n\t.size\tmain, "
	    ".-main\n\t.section\t.rodata\n.LC8:\n\t.float\t3.100000\n.LC9:\n\t."
	    "float\t4.300000\n\t.ident\t\"mCc "
	    "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n";
	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerControlFlow)
{
	const char input[] = "void main(){"
	                     "int a;"
	                     "if(true){"
	                     "a = 3;"
	                     "} else {"
	                     "a = 4;"
	                     "}"
	                     "while (a>0){"
	                     "a = a -1;"
	                     "}"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected =
	    "\t.file\t\"generated.c\"\n\t.text\n\t.globl\tmain\n\t.type\tmain, "
	    "@function\nmain:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$48, "
	    "%esp\n\tmovl\t$1, -4(%ebp)\n\tcmpl\t$0, "
	    "-4(%ebp)\n\tje\t.L1\n\tmovl\t$3, -8(%ebp)\n\tmovl\t-8(%ebp), "
	    "%eax\n\tmovl\t%eax, -12(%ebp)\n\tjmp .L2\n.L1:\n\tmovl\t$4, "
	    "-16(%ebp)\n\tmovl\t-16(%ebp), %eax\n\tmovl\t%eax, "
	    "-12(%ebp)\n.L2:\n.L3:\n\tmovl\t-12(%ebp), %eax\n\tmovl\t%eax, "
	    "-20(%ebp)\n\tmovl\t$0, -24(%ebp)\n\tmovl\t-20(%ebp), "
	    "%eax\n\tcmpl\t-24(%ebp), %eax\n\tsetg\t%al\n\tmovzbl\t%al, "
	    "%eax\n\tmovl\t%eax, -28(%ebp)\n\tcmpl\t$0, "
	    "-28(%ebp)\n\tje\t.L4\n\tmovl\t-12(%ebp), %eax\n\tmovl\t%eax, "
	    "-32(%ebp)\n\tmovl\t$1, -36(%ebp)\n\tmovl\t-32(%ebp), "
	    "%eax\n\tsubl\t-36(%ebp), %eax\n\tmovl\t%eax, "
	    "-40(%ebp)\n\tmovl\t-40(%ebp), %eax\n\tmovl\t%eax, -12(%ebp)\n\tjmp "
	    ".L3\n.L4:\n.LR11:\n\tleave\n\tret\n\t.size\tmain, "
	    ".-main\n\t.ident\t\"mCc "
	    "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n";
	ASSERT_STREQ(buffer, expected);

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerArray)
{
	const char input[] = "void main(){"
	                     "int[3] arr;"
	                     "arr[0] = 4;"
	                     "arr[2] = 5;"
	                     "int b;"
	                     "b = arr[2];"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected =
	    "\t.file\t\"generated.c\"\n\t.text\n\t.globl\tmain\n\t.type\tmain, "
	    "@function\nmain:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$48, "
	    "%esp\n\tmovl\t$4, -4(%ebp)\n\tmovl\t$0, -8(%ebp)\n\tmovl\t-8(%ebp), "
	    "%eax\n\tmovl\t-4(%ebp), %edx\n\tmovl\t%edx, "
	    "-20(%ebp,%eax,4)\n\tmovl\t$5, -24(%ebp)\n\tmovl\t$2, "
	    "-28(%ebp)\n\tmovl\t-28(%ebp), %eax\n\tmovl\t-24(%ebp), "
	    "%edx\n\tmovl\t%edx, -20(%ebp,%eax,4)\n\tmovl\t$2, "
	    "-32(%ebp)\n\tmovl\t-32(%ebp), %eax\n\tmovl\t-20(%ebp,%eax,4), "
	    "%eax\n\tmovl\t%eax, -36(%ebp)\n\tmovl\t-36(%ebp), %eax\n\tmovl\t%eax, "
	    "-40(%ebp)\n.LR12:\n\tleave\n\tret\n\t.size\tmain, "
	    ".-main\n\t.ident\t\"mCc "
	    "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}

TEST(Assembler, TestAssemblerFunc)
{
	const char input[] = "int foo(int b){"
	                     "return b;"
	                     "}"
	                     "void main(){"
	                     "int a;"
	                     "a= foo(4);"
	                     "}";
	auto result = mCc_parser_parse_string(input);
	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *myStream = open_memstream(&buffer, &bufferSize);

	mCc_ass_print_assembler_program(result.program, myStream);

	fclose(myStream);

	auto expected =
	    "\t.file\t\"generated.c\"\n\t.text\n\t.globl\tfoo\n\t.type\tfoo, "
	    "@function\nfoo:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$16, "
	    "%esp\n\tmovl\t8(%ebp), %eax\n\tmovl\t%eax, "
	    "-4(%ebp)\n\tmovl\t-4(%ebp), %eax\n\tmovl\t%eax, "
	    "-8(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tjmp "
	    ".LR13\n.LR13:\n\tleave\n\tret\n\t.size\tfoo, "
	    ".-foo\n\t.globl\tmain\n\t.type\tmain, "
	    "@function\nmain:\n\tpushl\t%ebp\n\tmovl\t%esp, %ebp\n\tsubl\t$16, "
	    "%esp\n\tmovl\t$4, "
	    "-4(%ebp)\n\tpushl\t-4(%ebp)\n\tcall\tfoo\n\taddl\t$4, "
	    "%esp\n\tmovl\t%eax, -8(%ebp)\n\tmovl\t-8(%ebp), %eax\n\tmovl\t%eax, "
	    "-12(%ebp)\n.LR14:\n\tleave\n\tret\n\t.size\tmain, "
	    ".-main\n\t.ident\t\"mCc "
	    "compiler\"\n\t.section\t.note.GNU-stack,\"\",@progbits\n";

	ASSERT_STREQ(buffer, expected);

	free(buffer);
	mCc_parser_delete_result(&result);
}