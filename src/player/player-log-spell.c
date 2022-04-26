#include "../angband.h"
#include "player-log-spell.h"

void init_log_spell_group(log_spell_group *group)
{
    (void)C_WIPE(group->log_entries, 512, log_spell_single);
    group->entry_count = 0;
}

void record_spell_use_in_group(log_spell_group *group, cptr spell_name)
{
    int i;
    bool entry_found = FALSE;

    for (i = 0; i < group->entry_count; i++)
    {
        if (!strcmp(group->log_entries[i].name, spell_name))
        {
            group->log_entries[i].use_count++;
            entry_found = TRUE;
            break;
        }
    }

    if (entry_found == FALSE)
    {
        my_strcpy(group->log_entries[i].name, spell_name, sizeof(group->log_entries[i].name));
        group->log_entries[i].use_count = 1;
        group->entry_count++;
    }
}
