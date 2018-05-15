#ifndef MCC_AST_FUNCTION_RETURN_CHECK_H
#define MCC_AST_FUNCTION_RETURN_CHECK_H

#include "mCc/ast.h"
#include "mCc/error_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

void mCc_ast_function_return_checks(
    struct mCc_ast_program *program,
    struct mCc_err_error_manager *error_manager);

#ifdef __cplusplus
}
#endif

#endif // MCC_AST_FUNCTION_RETURN_CHECK_H
