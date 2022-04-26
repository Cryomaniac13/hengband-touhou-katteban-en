#include "../angband.h"

#include "../io/io-plain-read-write.h"
#include "player-log.h"

void write_log_object_type_single(FILE *file, log_object_type_single *log_entry)
{
    write_u16b(file, log_entry->k_idx);
    write_u32b(file, log_entry->use_count);
}

void write_log_object_type_group(FILE *file, log_object_type_group *group)
{
    int i;

    write_u16b(file, group->entry_count);
    for (i = 0; i < group->entry_count; i++)
    {
        write_log_object_type_single(file, &group->log_entries[i]);
    }
}

void write_log_spell_single(FILE *file, log_spell_single *log_entry)
{
    write_string(file, log_entry->name);
    write_u32b(file, log_entry->use_count);
}

void write_log_spell_group(FILE *file, log_spell_group *group)
{
    int i;

    write_u16b(file, group->entry_count);
    for (i = 0; i < group->entry_count; i++)
    {
        write_log_spell_single(file, &group->log_entries[i]);
    }
}

bool write_player_log(FILE *file, player_log *log)
{
    write_byte(file, P_LOG_VER_MAJOR);
    write_byte(file, P_LOG_VER_MINOR);
    write_byte(file, P_LOG_VER_PATCH);

    write_log_object_type_group(file, &log->log_food);
    write_log_object_type_group(file, &log->log_potions);
    write_log_object_type_group(file, &log->log_scrolls);
    write_log_object_type_group(file, &log->log_wands);
    write_log_object_type_group(file, &log->log_staves);
    write_log_object_type_group(file, &log->log_rods);

    write_log_spell_group(file, &log->log_spells);
    write_log_spell_group(file, &log->log_race_powers);
    write_log_spell_group(file, &log->log_class_powers);

    if (ferror(file) || (fflush(file) == EOF)) return FALSE;

    return TRUE;
}

static bool save_player_log_aux(char *file_name, player_log *log)
{
    bool success = FALSE;
    int fd  = -1;
    int mode =  0644;
    FILE *log_save_file;

    FILE_TYPE(FILE_TYPE_SAVE);

    fd = fd_make(file_name, mode);

    if (fd >= 0)
    {
        (void)fd_close(fd);

        log_save_file = my_fopen(file_name, "wb");

        if (log_save_file)
        {
            if (write_player_log(log_save_file, log)) success = TRUE;
            if (my_fclose(log_save_file)) success = FALSE;
        }
        else
        {
            msg_print("ERROR! Failed to open save file for player log!");
        }
    }

    if (!success) return (FALSE);

    return (TRUE);
}

bool save_player_log(cptr savefile_name, player_log *log)
{
    bool success = FALSE;

    char    log_file_name[1024];
    char    cur_file_name[1024];

    strcpy(log_file_name, savefile_name);
    strcat(log_file_name, ".plog");

    strcpy(cur_file_name, log_file_name);
    strcat(cur_file_name, ".new");

    fd_kill(cur_file_name);

    if (save_player_log_aux(cur_file_name, log))
    {
        char temp_file_name[1024];

        strcpy(temp_file_name, log_file_name);
        strcat(temp_file_name, ".old");

        fd_kill(temp_file_name);

        fd_move(log_file_name, temp_file_name);
        fd_move(cur_file_name, log_file_name);
        fd_kill(temp_file_name);

        success = TRUE;
    }

    return success;
}
