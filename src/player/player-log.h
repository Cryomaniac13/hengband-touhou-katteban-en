#ifndef INCLUDED_PLAYER_LOG
#define INCLUDED_PLAYER_LOG

#include "player-log-object.h"
#include "player-log-spell.h"

#define P_LOG_VER_MAJOR 0
#define P_LOG_VER_MINOR 1
#define P_LOG_VER_PATCH 0

typedef struct player_log {
    log_object_type_group log_food;
    log_object_type_group log_potions;
    log_object_type_group log_scrolls;

    log_object_type_group log_wands;
    log_object_type_group log_staves;
    log_object_type_group log_rods;

    log_spell_group log_spells;
    log_spell_group log_race_powers;
    log_spell_group log_class_powers;
} player_log;

extern player_log *p_log_ptr;

extern void init_player_log(player_log *log);
extern void player_log_record_food_use(player_log *log, u16b);
extern void player_log_record_potion_use(player_log *log, u16b);
extern void player_log_record_scroll_use(player_log *log, u16b);
extern void player_log_record_wand_use(player_log *log, u16b);
extern void player_log_record_staff_use(player_log *log, u16b);
extern void player_log_record_rod_use(player_log *log, u16b);
extern void player_log_record_spell_use(player_log *log, cptr);

#endif // INCLUDED_PLAYER_LOG
