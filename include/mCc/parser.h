#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_parser_status {
	MCC_PARSER_STATUS_OK,
	MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
	MCC_PARSER_STATUS_UNKNOWN_ERROR,
    MCC_PARSER_STATUS_SYNTAX_ERROR,
};

struct mCc_error_location {
    int first_line;
    int last_line;
    int first_column;
    int last_column;
};

struct mCc_parse_error {
    int is_error;
    char* msg;
    struct mCc_error_location location;
};

struct mCc_parser_result {
	enum mCc_parser_status status;
	struct mCc_ast_expression *expression;
	struct mCc_ast_program *program;
	struct mCc_parse_error parse_error;
};

struct mCc_parser_result mCc_parser_parse_string(const char *input);

struct mCc_parser_result mCc_parser_parse_file(FILE *input);

#ifdef __cplusplus
}
#endif

#endif
