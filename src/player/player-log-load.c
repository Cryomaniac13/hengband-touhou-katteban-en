#include "../angband.h"

#include "../io/io-plain-read-write.h"
#include "player-log.h"

static void read_log_object_type_single(FILE *file, log_object_type_single *log_entry)
{
    read_u16b(file, &log_entry->k_idx);
    read_u32b(file, &log_entry->use_count);
}

static void read_log_object_type_group(FILE *file, log_object_type_group *group)
{
    int i;

    read_u16b(file, &group->entry_count);
    for (i = 0; i < group->entry_count; i++)
    {
        read_log_object_type_single(file, &group->log_entries[i]);
    }
}

static void read_log_spell_single(FILE *file, log_spell_single *log_entry)
{
    read_string(file, &log_entry->name, sizeof(&log_entry->name));
    read_u32b(file, &log_entry->use_count);
}

static void read_log_spell_group(FILE *file, log_spell_group *group)
{
    int i;

    read_u16b(file, &group->entry_count);
    for (i = 0; i < group->entry_count; i++)
    {
        read_log_spell_single(file, &group->log_entries[i]);
    }
}

static void read_player_log(FILE *file, player_log *log)
{
    byte log_version_major, log_version_minor, log_version_patch;

    read_byte(file, &log_version_major);
    read_byte(file, &log_version_minor);
    read_byte(file, &log_version_patch);

    read_log_object_type_group(file, &log->log_food);
    read_log_object_type_group(file, &log->log_potions);
    read_log_object_type_group(file, &log->log_scrolls);
    read_log_object_type_group(file, &log->log_wands);
    read_log_object_type_group(file, &log->log_staves);
    read_log_object_type_group(file, &log->log_rods);

    read_log_spell_group(file, &log->log_spells);
    read_log_spell_group(file, &log->log_race_powers);
    read_log_spell_group(file, &log->log_class_powers);
}

static bool load_player_log_aux(char *file_name, player_log *log)
{
    FILE *log_save_file;

    log_save_file = my_fopen(file_name, "rb");

    if (!log_save_file) return FALSE;

    read_player_log(log_save_file, log);
    if (ferror(log_save_file)) return FALSE;

    my_fclose(log_save_file);

    return TRUE;
}

bool load_player_log(cptr savefile_name, player_log *log)
{
    bool    success = FALSE;
    int     fd = -1;
    char    log_file_name[1024];

    init_player_log(log);

    if (!savefile_name[0]) return TRUE;

    strcpy(log_file_name, savefile_name);
    strcat(log_file_name, ".plog");

    // Try opening log file; if it doesn't exist, ignore and
    // continue with a cleanly initialized player log
    fd = fd_open(log_file_name, O_RDONLY);
    if (fd < 0) return TRUE;

    (void)fd_close(fd);

    if (load_player_log_aux(log_file_name, log)) success = TRUE;

    return success;
}
