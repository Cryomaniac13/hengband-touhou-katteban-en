#ifndef INCLUDED_PLAYER_LOG_OBJECT
#define INCLUDED_PLAYER_LOG_OBJECT

#include "../h-type.h"

typedef struct log_object_type_single {
    u16b k_idx;

    u32b use_count;
} log_object_type_single;

typedef struct log_object_type_group {
    log_object_type_single log_entries[256];
    u16b entry_count;
} log_object_type_group;

extern void init_log_object_type_group(log_object_type_group *group);
extern void record_object_use_in_group(log_object_type_group *group, u16b);

#endif // INCLUDED_PLAYER_LOG_OBJECT
