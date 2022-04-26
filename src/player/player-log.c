#include "player-log.h"

/*
 * Static player log data
 */
player_log p_log;

/*
 * Pointer to the player log data
 */
player_log *p_log_ptr = &p_log;

void init_player_log(player_log *log)
{
    init_log_object_type_group(&log->log_food);
    init_log_object_type_group(&log->log_potions);
    init_log_object_type_group(&log->log_scrolls);

    init_log_object_type_group(&log->log_wands);
    init_log_object_type_group(&log->log_staves);
    init_log_object_type_group(&log->log_rods);

    init_log_spell_group(&log->log_spells);
    init_log_spell_group(&log->log_race_powers);
    init_log_spell_group(&log->log_class_powers);
}

void player_log_record_food_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_food, k_idx);
}

void player_log_record_potion_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_potions, k_idx);
}

void player_log_record_scroll_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_scrolls, k_idx);
}

void player_log_record_wand_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_wands, k_idx);
}

void player_log_record_staff_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_staves, k_idx);
}

void player_log_record_rod_use(player_log *log, u16b k_idx)
{
    record_object_use_in_group(&log->log_rods, k_idx);
}

void player_log_record_spell_use(player_log *log, cptr spell_name)
{
    record_spell_use_in_group(&log->log_spells, spell_name);
}

void player_log_record_rac_power_use(player_log *log, cptr power_name)
{
    record_spell_use_in_group(&log->log_race_powers, power_name);
}

void player_log_record_class_power_use(player_log *log, cptr power_name)
{
    record_spell_use_in_group(&log->log_class_powers, power_name);
}
