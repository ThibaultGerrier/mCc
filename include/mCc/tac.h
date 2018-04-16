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

void mCc_ast_print_tac_program(FILE *out, struct mCc_ast_program *result);

#ifdef __cplusplus
}
#endif
