//
// Created by philipp on 12.04.18.
//

#ifndef MCC_TAC_H
#define MCC_TAC_H

#endif //MCC_TAC_H
#include <stdio.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void mCc_tac_generate(struct mCc_parser_result *result);

#ifdef __cplusplus
}
#endif

