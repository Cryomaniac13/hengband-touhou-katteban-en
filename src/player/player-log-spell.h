#ifndef INCLUDED_PLAYER_LOG_SPELL
#define INCLUDED_PLAYER_LOG_SPELL

#include "../h-type.h"

typedef struct log_spell_single
{
    cptr name[80];
    u32b use_count;
} log_spell_single;

typedef struct log_spell_group
{
    log_spell_single log_entries[512];
    u16b entry_count;
} log_spell_group;

extern void init_log_spell_group(log_spell_group *group);
extern void record_spell_use_in_group(log_spell_group *group, cptr);

#endif // INCLUDED_PLAYER_LOG_SPELL
