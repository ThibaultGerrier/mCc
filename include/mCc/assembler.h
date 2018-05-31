//
// Created by philipp on 12.04.18.
//

#ifndef MCC_TAC_H
#define MCC_TAC_H

#endif // MCC_TAC_H
#include "mCc/ast.h"
#include "mCc/parser.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


void mCc_ast_print_assembler_program(struct mCc_ast_program *result, FILE *out);


#ifdef __cplusplus
}
#endif