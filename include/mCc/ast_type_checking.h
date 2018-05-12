#ifndef MCC_AST_TYPE_CHECKING_H
#define MCC_AST_TYPE_CHECKING_H

#include "ast_symbol_table.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_ast_visitor
mCc_ast_type_checking_visitor(struct mCc_ast_function_def **cur_function,
                              struct mCc_err_error_manager *error_manager);

#ifdef __cplusplus
}
#endif

#endif
