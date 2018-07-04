#define _DEFAULT_SOURCE
#include <getopt.h>
#include <mCc/ast_function_return_check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mCc/assembler.h"
#include "mCc/ast.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_type_checking.h"
#include "mCc/buildins_c.h"
#include "mCc/error_manager.h"
#include "mCc/parser.h"
#include "mCc/tac.h"

void cleanup_and_exit(char *outfilename, char *outassfilename,
                      struct mCc_parser_result *parser_result,
                      struct mCc_sym_table_tree *symbol_table_tree,
                      struct mCc_err_error_manager *error_manager,
                      struct mCc_tac *tac, int exit_status)
{
	free(outfilename);
	free(outassfilename);
	mCc_parser_delete_result(parser_result);
	mCc_sym_table_delete_tree(symbol_table_tree);
	mCc_err_delete_error_manager(error_manager);
	mCc_tac_delete_tac(tac);
	exit(exit_status);
}

void print_usage(const char *prg)
{
	printf("usage: %s {OPTIONS} <FILE>\n\n", prg);
	printf("  <FILE>                        Input filepath or - for stdin\n");
	printf("  OPTIONS:\n");
	printf("    --help, -h                  Display this usage information\n");
	printf("    --version, -v               Display version information\n");
	printf("    --output <FILE>, -o <FILE>  Output file (default a.out)\n");
}

/**
 * *infilename and *outfilename is allocated by this function, so they have to
 * be freed
 */
void parse_arguments(int argc, char *argv[], char **infilename,
                     char **outfilename)
{
	int c;
	int help_flag = 0;
	int version_flag = 0;

	*outfilename = NULL;
	*infilename = NULL;

	if (argc < 2) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	struct option long_options[] = { { "help", no_argument, &help_flag, 'h' },
		                             { "version", no_argument, &version_flag,
		                               'v' },
		                             { "output", required_argument, 0, 'o' },
		                             { 0, 0, 0, 0 } };

	for (int i = 0; i < argc - 1; ++i) {
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "hvo:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0) {
				break;
			}
			if (option_index == 2) { // output
				if (optarg) {
					*outfilename =
					    (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
					strcpy(*outfilename, optarg);
					break;
				}
			}
			fprintf(stderr, "error while parsing arguments!\n");
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
			break;
		case 'h': help_flag = 1; break;
		case 'v': version_flag = 1; break;
		case 'o':
			if (optarg) {
				*outfilename =
				    (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(*outfilename, optarg);
				break;
			}
			fprintf(stderr, "error while parsing arguments!\n");
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
			break;

		case '?':
		default:
			fprintf(stderr, "error while parsing arguments!\n");
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
			/* getopt_long already printed an error message. */
			break;
		}
	}

	if (help_flag != 0) {
		free(*outfilename);
		print_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	if (version_flag != 0) {
		free(*outfilename);
		printf("version 0.1.0");
		exit(EXIT_SUCCESS);
	}

	if (*outfilename == NULL) {
		*outfilename = (char *)malloc(sizeof(char) * (strlen("a.out") + 1));
		strcpy(*outfilename, "a.out");
	}
	*infilename = (char *)malloc(sizeof(char) * (strlen(argv[argc - 1]) + 1));

	strcpy(*infilename, argv[argc - 1]);
}

void print_all_errors(struct mCc_err_error_manager *error_manager)
{
	for (size_t i = 0; i < error_manager->used; i++) {
		fprintf(stderr, "%s\n", error_manager->array[i]->msg);
	}
}

void print_errors_and_exit_if_errors(
    char *outfilename, char *outassfilename,
    struct mCc_parser_result *parser_result,
    struct mCc_sym_table_tree *symbol_table_tree,
    struct mCc_err_error_manager *error_manager, struct mCc_tac *tac)
{
	assert(error_manager);
	if (error_manager->used != 0) {
		print_all_errors(error_manager);
		cleanup_and_exit(outfilename, outassfilename, parser_result,
		                 symbol_table_tree, error_manager, tac, EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	FILE *in;
	FILE *outass;
	char *infilename = (char *)NULL;
	char *outfilename = (char *)NULL;
	char *outassfilename = (char *)NULL;

	parse_arguments(argc, argv, &infilename, &outfilename);
	/* determine input source */
	if (strcmp("-", infilename) == 0) {
		in = stdin;
	} else {
		in = fopen(infilename, "r");
	}
	// output files (assembler and executable)
	size_t outassfilename_length = sizeof(char) * (strlen(outfilename) + 3);
	outassfilename = (char *)malloc(outassfilename_length);
	snprintf(outassfilename, outassfilename_length, "%s.S", outfilename);
	outass = fopen(outassfilename, "w");
	free(infilename);

	if (!in || !outass) {
		free(outassfilename);
		if (in) {
			fclose(in);
		}
		if (outass) {
			fclose(outass);
		}
		perror("fopen");
		printf("onononono ");
		return EXIT_FAILURE;
	}

	struct mCc_parser_result result;
	struct mCc_ast_program *program = NULL;
	struct mCc_sym_table_tree *symbol_table_tree = NULL;
	struct mCc_err_error_manager *error_manager = mCc_err_new_error_manager();
	struct mCc_tac *tac = NULL;

	/* parsing phase */
	{
		result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status != MCC_PARSER_STATUS_OK) {
			mCc_parser_print_error(result.parse_error);
			free(outfilename);
			free(outassfilename);
			return EXIT_FAILURE;
		}
		program = result.program;
	}
	/* symbol table creation phase */
	{
		struct mCc_ast_symbol_table_visitor_data visitor_data = { NULL, NULL,
			                                                      0 };
		struct mCc_ast_visitor symbol_table_visitor =
		    mCc_ast_symbol_table_visitor(&visitor_data, error_manager);

		mCc_ast_visit_program(program, &symbol_table_visitor);

		symbol_table_tree = visitor_data.symbol_table_tree;

		print_errors_and_exit_if_errors(outfilename, outassfilename, &result,
		                                symbol_table_tree, error_manager, tac);
	}
	/* return check  phase */
	{
		mCc_ast_function_return_checks(program, error_manager);

		print_errors_and_exit_if_errors(outfilename, outassfilename, &result,
		                                symbol_table_tree, error_manager, tac);
	}
	/* type checking phase */
	{
		// temporary for each function scope check (return mostly)
		struct mCc_ast_function_def *cur_function;
		struct mCc_ast_visitor type_checking_visitor =
		    mCc_ast_type_checking_visitor(&cur_function, error_manager);

		mCc_ast_visit_program(program, &type_checking_visitor);

		print_errors_and_exit_if_errors(outfilename, outassfilename, &result,
		                                symbol_table_tree, error_manager, tac);
	}

	/* Three address code generation phase */
	{
		tac = mCc_ast_get_tac(program);
		// mCc_tac_print_tac(tac, stdout);
	}

	/* assembler code generation phase */
	{
		mCc_ass_print_assembler(tac, outass);
		fclose(outass);
	}

	/* Backend compiler invocation phase */
	{
		// rather dirty, there might be a better way to do it without temporary
		// files and popen

		FILE *p_buildin_gcc =
		    popen("gcc -m32 -S -O0 -fno-stack-protector "
		          "-fno-asynchronous-unwind-tables "
		          "-fsingle-precision-constant -o ./_mCc_buildins.S -xc -",
		          "w");
		if (!p_buildin_gcc) {
			perror("popen");
			print_all_errors(error_manager);
			cleanup_and_exit(outfilename, outassfilename, &result,
			                 symbol_table_tree, error_manager, tac,
			                 EXIT_FAILURE);
		}
		fprintf(p_buildin_gcc, "%s", buildins_c_data);
		pclose(p_buildin_gcc);

		char cmd_buffer[1024];
		snprintf(cmd_buffer, 1024, "gcc -m32 -c %s -o ./%s.o", outassfilename,
		         outassfilename);
		pclose(popen(cmd_buffer, "r"));
		pclose(popen("gcc -m32 -c _mCc_buildins.S -o _mCc_buildins.S.o", "r"));
		snprintf(cmd_buffer, 1024,
		         "gcc -m32 -o ./%s ./_mCc_buildins.S.o ./%s.o", outfilename,
		         outassfilename);
		pclose(popen(cmd_buffer, "r"));

		// cleanup temporary files
		snprintf(cmd_buffer, 1024, "%s.o", outassfilename);
		remove(cmd_buffer);
		snprintf(cmd_buffer, 1024, "%s.o", outfilename);
		remove(cmd_buffer);
		remove(outassfilename);
		remove("_mCc_buildins.S");
		remove("_mCc_buildins.S.o");
	}

	/*    TODO
	 * - do some optimisations
	 */

	/* cleanup */
	print_all_errors(error_manager);
	cleanup_and_exit(outfilename, outassfilename, &result, symbol_table_tree,
	                 error_manager, tac, EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
