#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_err_error_entry {
	char *msg;
	size_t start_line;
	size_t start_col;
	size_t end_line;
	size_t end_col;
};

struct mCc_err_error_manager {
	struct mCc_err_error_entry **array;
	size_t used;
	size_t size;
};

struct mCc_err_error_entry *
mCc_err_new_error_entry(const char *msg, size_t start_line, size_t start_col,
                        size_t end_line, size_t end_col);

void mCc_err_free_error_entry(struct mCc_err_error_entry *entry);

struct mCc_err_error_manager *mCc_err_new_error_manager();

void mCc_err_error_manager_insert_error_entry(
    struct mCc_err_error_manager *err_man, struct mCc_err_error_entry *entry);

void mCc_err_delete_error_manager(struct mCc_err_error_manager *err_man);

#ifdef __cplusplus
}
#endif

#endif
