#include "mCc/error_manager.h"

#define max(a, b) \
	({ \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a > _b ? _a : _b; \
	})

struct mCc_err_error_entry *
mCc_err_new_error_entry(const char *msg, size_t start_line, size_t start_col,
                        size_t end_line, size_t end_col)
{
	struct mCc_err_error_entry *err_entry =
	    (struct mCc_err_error_entry *)malloc(sizeof(*err_entry));
	assert(err_entry);

	size_t msg_len = strlen(msg) + 1;
	err_entry->msg = (char *)malloc(msg_len);
	assert(err_entry);
	memcpy(err_entry->msg, msg, msg_len);

	err_entry->start_line = start_line;
	err_entry->start_col = start_col;
	err_entry->end_line = end_line;
	err_entry->end_col = end_col;

	return err_entry;
}

void mCc_err_free_error_entry(struct mCc_err_error_entry *entry)
{
	assert(entry);
	free(entry->msg);
	free(entry);
}

struct mCc_err_error_manager *mCc_err_new_error_manager()
{
	struct mCc_err_error_manager *err_man =
	    (struct mCc_err_error_manager *)malloc(sizeof(*err_man));
	assert(err_man);
	err_man->array = NULL;
	err_man->used = 0;
	err_man->size = 0;
	return err_man;
}

void mCc_err_error_manager_insert_error_entry(
    struct mCc_err_error_manager *err_man, struct mCc_err_error_entry *entry)
{
	if (err_man->used == err_man->size) {
		err_man->size = max(err_man->size * 2, 1u);
		err_man->array = (struct mCc_err_error_entry **)realloc(
		    err_man->array,
		    err_man->size * sizeof(struct mCc_err_error_entry *));
		assert(err_man->array);
	}
	err_man->array[err_man->used++] = entry;
}

void mCc_err_delete_error_manager(struct mCc_err_error_manager *err_man)
{
	assert(err_man);
	for (size_t i = 0; i < err_man->used; ++i) {
		mCc_err_free_error_entry(err_man->array[i]);
	}
	free(err_man->array);
	err_man->array = NULL;
	err_man->used = err_man->size = 0;
	free(err_man);
}
