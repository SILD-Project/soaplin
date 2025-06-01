
// Structure for an executable program loaded into memory.

#include "mm/paging.h"
#include <stddef.h>

typedef enum {
    EXEC_FMT_ELF,
    EXEC_FMT_PE,
    EXEC_FMT_MACHO,
    EXEC_FMT_SCRIPT
} exec_format_t;

typedef struct exec {
    pagemap_t pm;

    void *rwdata_start;
    size_t rwdata_len;

    uint64_t entry;

    int argc;    // 0 for useds
    char **argv; // NULL when the executable is a used

    char *path;
    exec_format_t format;

    size_t usecount; // -1 for users, else, the number of processes sharing the executable's read only sections
    struct exec *used; // The reference executable.
} exec_t;