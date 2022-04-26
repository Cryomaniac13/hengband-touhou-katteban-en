#include "../angband.h"
#include "player-log-object.h"

void init_log_object_type_group(log_object_type_group *group)
{
    (void)C_WIPE(group->log_entries, 256, log_object_type_single);
    group->entry_count = 0;
}

void record_object_use_in_group(log_object_type_group *group, u16b k_idx)
{
    int i;
    bool entry_found = FALSE;

    for (i = 0; i < group->entry_count; i++)
    {
        if (group->log_entries[i].k_idx == k_idx)
        {
            group->log_entries[i].use_count++;
            entry_found = TRUE;
            break;
        }
    }

    if (entry_found == FALSE)
    {
        group->log_entries[i].k_idx = k_idx;
        group->log_entries[i].use_count = 1;
        group->entry_count++;
    }
}
