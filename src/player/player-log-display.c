#include "../angband.h"
#include "player-log.h"

/*
 *  Displays information on how many times different objects in a single group
 *  were used by the player
 */

static void log_display_object_type_group(FILE *fff, log_object_type_group *group, cptr title)
{
    int i;
    log_object_type_single *log_object_single;
    object_kind *k_ptr;

    cptr        s, s0;
    char        *t;

    cptr        base_name;
    char        tmp_name[160];

    fprintf(fff, "%s\n", title);
    fprintf(fff, "========================================\n");

    for (i = 0; i < group->entry_count; i++)
    {
        log_object_single = &group->log_entries[i];
        k_ptr = &k_info[log_object_single->k_idx];

        t = tmp_name;
        base_name = k_name + k_ptr->name;
        if (base_name[0] == '&')
        {
            s = base_name + 2;
        }
        else
        {
            s = base_name;
        }

        for (; *s;)
        {
            if (*s == '~')
            {
                s++;
            }
            else
            {
                *t++ = *s++;
            }
        }

        *t = '\0';

        fprintf(fff, "%-20s %6d\n", tmp_name, log_object_single->use_count);
    }

    fprintf(fff, "\n\n");
}

/*
 *  Displays information on how many times different spells in a single group
 *  were cast by the player
 */

static void log_display_spell_group(FILE *fff, log_spell_group *group, cptr title)
{
    int i;
    log_spell_single *single_spell;

    fprintf(fff, "%s\n", title);
    fprintf(fff, "==========================================================\n");

    for (i = 0; i < group->entry_count; i++)
    {
        single_spell = &group->log_entries[i];
        fprintf(fff, "%-50s %7d\n", single_spell->name, single_spell->use_count);
    }

    fprintf(fff, "\n\n");
}

void player_log_display_objects(player_log *log)
{
    FILE            *fff;
	char            file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff)
	{
		msg_format(_("一時ファイル %s を作成できませんでした。",
                    "Failed to create temporary file %s."), file_name);
		msg_print(NULL);
		return;
	}

	log_display_object_type_group(fff, &log->log_food,    "Food");
	log_display_object_type_group(fff, &log->log_potions, "Potions");
	log_display_object_type_group(fff, &log->log_scrolls, "Scrolls");
	log_display_object_type_group(fff, &log->log_wands,   "Wands");
	log_display_object_type_group(fff, &log->log_staves,  "Staves");
	log_display_object_type_group(fff, &log->log_rods,    "Rods");

	my_fclose(fff);
	show_file(TRUE, file_name, "Player Log: Used objects", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}

void player_log_display_spells(player_log *log)
{
    FILE            *fff;
	char            file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff)
	{
		msg_format(_("一時ファイル %s を作成できませんでした。",
                    "Failed to create temporary file %s."), file_name);
		msg_print(NULL);
		return;
	}

	log_display_spell_group(fff, &log->log_spells,       "Spells");
	log_display_spell_group(fff, &log->log_race_powers,  "Race powers");
    log_display_spell_group(fff, &log->log_class_powers, "Class powers");

	my_fclose(fff);
	show_file(TRUE, file_name, "Player Log: Used spells/powers", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}
