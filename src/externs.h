/* File: externs.h */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: extern declarations (variables and functions) */

/*
 * Note that some files have their own header files
 * (z-virt.h, z-util.h, z-form.h, term.h, random.h)
 */


/*
 * Automatically generated "variable" declarations
 */

extern int max_macrotrigger;
extern cptr macro_template;
extern cptr macro_modifier_chr;
extern cptr macro_modifier_name[MAX_MACRO_MOD];
extern cptr macro_trigger_name[MAX_MACRO_TRIG];
extern cptr macro_trigger_keycode[2][MAX_MACRO_TRIG];


/* 日本語版機能追加で使う */
extern int level_up;

/*
 *  List for auto-picker/destroyer entries
 */
extern int max_autopick;
extern int max_max_autopick;
extern autopick_type *autopick_list;

/* tables.c */
///bldg131221
extern const cptr new_store_name[][MAX_STORES];
extern const cptr new_store_owner_name[][MAX_STORES];


extern const s16b ddd[9];
extern const s16b ddx[10];
extern const s16b ddy[10];
extern const s16b ddx_ddd[9];
extern const s16b ddy_ddd[9];
extern const s16b cdd[8];
extern const s16b ddx_cdd[8];
extern const s16b ddy_cdd[8];
extern const char hexsym[16];
extern const char listsym[];
extern const cptr color_char;
extern const byte adj_mag_study[];
extern const byte adj_mag_mana[];
extern const byte adj_mag_fail[];
extern const byte adj_mag_stat[];
extern const byte adj_chr_gold[];
extern const byte adj_int_dev[];
extern const byte adj_wis_sav[];
extern const byte adj_dex_dis[];
extern const byte adj_int_dis[];
extern const byte adj_dex_ta[];
extern const byte adj_str_td[];
extern const byte adj_dex_th[];
extern const byte adj_str_th[];
extern const byte adj_str_wgt[];
extern const byte adj_str_hold[];
extern const byte adj_str_dig[];
extern const s16b adj_str_blow[];
extern const byte adj_dex_blow[];
extern const byte adj_dex_safe[];
extern const byte adj_con_fix[];
extern const byte adj_con_mhp[];
extern const byte adj_chr_chm[];
extern const byte blows_table[12][12];
extern const arena_type arena_info[MAX_ARENA_MONS + 2];

///mod140105 種族変更（準備）
//extern const owner_type owners[MAX_STORES][MAX_OWNERS];
extern const byte extract_energy[200];
extern const s32b player_exp[PY_MAX_LEVEL];
extern const s32b player_exp_a[PY_MAX_LEVEL];
extern const player_sex sex_info[MAX_SEXES];
extern const player_race race_info[MAX_RACES];
extern const player_class class_info[MAX_CLASS];
//extern const magic_type technic_info[NUM_TECHNIC][32];
//extern const magic_type hissatsu_info[32];
extern magic_type hissatsu_info[32];
extern const player_seikaku seikaku_info[MAX_SEIKAKU];
extern const player_race mimic_info[];
extern const u32b fake_spell_flags[4];
//extern const s32b realm_choices1[];
//extern const s32b realm_choices2[];
extern const cptr realm_names[];
#ifdef JP
extern const cptr E_realm_names[];
#endif
//extern const cptr spell_names[VALID_REALM][32];
extern const int chest_traps[64];
//extern const cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5];
extern const cptr color_names[16];
extern const cptr stat_names[6];
extern const cptr stat_names_reduced[6];
extern const cptr window_flag_desc[32];
extern const option_type option_info[];
extern const cptr chaos_patrons[MAX_PATRON];
extern const int chaos_stats[MAX_PATRON];
extern const int chaos_rewards[MAX_PATRON][20];
//extern const martial_arts ma_blows[MAX_MA];
extern const int monk_ave_damage[PY_MAX_LEVEL+1][3];
extern const cptr game_inscriptions[];
extern const kamae kamae_shurui[MAX_KAMAE];
extern const kamae kata_shurui[MAX_KATA];
extern const cptr exp_level_str[5];
extern const cptr silly_attacks[MAX_SILLY_ATTACK];
extern const cptr silly_attacks2[MAX_SILLY_ATTACK];
extern const monster_power monster_powers[MAX_MONSPELLS];
extern const cptr monster_powers_short[MAX_MONSPELLS];
extern const cptr ident_info[];
extern const mbe_info_type mbe_info[];
extern const byte feature_action_flags[FF_FLAG_MAX];
extern const dragonbreath_type dragonbreath_info[];
extern const activation_type activation_info[];

/* variable.c */
extern const cptr copyright[5];
extern byte h_ver_major;
extern byte h_ver_minor;
extern byte h_ver_patch;
extern byte h_ver_extra;
extern byte sf_extra;
extern u32b sf_system;
extern byte z_major;
extern byte z_minor;
extern byte z_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;
extern bool arg_wizard;
extern bool arg_sound;
extern bool arg_music;
extern byte arg_graphics;
extern bool arg_monochrome;
extern bool arg_force_original;
extern bool arg_force_roguelike;
extern bool arg_bigtile;
extern bool character_generated;
extern bool character_dungeon;
extern bool character_loaded;
extern bool character_saved;
extern bool character_icky;
extern bool character_xtra;
extern bool creating_savefile;
extern u32b seed_flavor;
extern u32b seed_town;
extern s16b command_cmd;
extern s16b command_arg;
extern s16b command_rep;/*:::リピートコマンドのカウント*/
extern s16b command_dir;
extern s16b command_see;
extern s16b command_gap;
extern s16b command_wrk;
extern s16b command_new;
extern s16b energy_use;
extern bool msg_flag;
extern s16b running;
extern s16b resting;
extern s16b cur_hgt;
extern s16b cur_wid;
extern s16b dun_level;
extern s16b num_repro;
extern s16b object_level;
extern s16b monster_level;
extern s16b base_level;
extern s32b turn;
extern s32b turn_limit;
extern s32b dungeon_turn;
extern s32b dungeon_turn_limit;
extern s32b old_turn;
extern s32b old_battle;
extern bool use_sound;
extern bool use_music;
extern bool use_graphics;
extern bool use_bigtile;
extern s16b signal_count;
extern bool inkey_base;
extern bool inkey_xtra;
extern bool inkey_scan;
extern bool inkey_flag;
extern bool get_com_no_macros;
extern s16b coin_type;
extern bool opening_chest;
extern bool shimmer_monsters;
extern bool shimmer_objects;
extern bool repair_monsters;
extern bool repair_objects;
extern s16b inven_nxt;
extern s16b inven_cnt;
extern s16b equip_cnt;
extern s16b o_max;
extern s16b o_cnt;
extern s16b m_max;
extern s16b m_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;/*:::詳細不明だが、直前に生成されたモンスターのcave[][].m_idxが入っている？*/
extern int total_friends;
extern s32b friend_align;
extern int leaving_quest;
extern bool reinit_wilderness;
extern bool multi_rew;
extern char summon_kin_type;
extern bool hack_mind;
extern bool hack_startup;


/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/*** Input Options ***/

extern bool rogue_like_commands;	/* Rogue-like commands */
extern bool always_pickup;	/* Pick things up by default */
extern bool carry_query_flag;	/* Prompt before picking things up */
extern bool quick_messages;	/* Activate quick messages */
extern bool auto_more;	/* Automatically clear '-more-' prompts */
extern bool command_menu;	/* Enable command selection menu */
extern bool other_query_flag;	/* Prompt for floor item selection */
extern bool use_old_target;	/* Use old target by default */
extern bool always_repeat;	/* Repeat obvious commands */
extern bool confirm_destroy;	/* Prompt for destruction of known worthless items */
extern bool confirm_wear;	/* Confirm to wear/wield known cursed items */
extern bool confirm_quest;	/* Prompt before exiting a quest level */
extern bool target_pet;	/* Allow targetting pets */

#ifdef ALLOW_EASY_OPEN
extern bool easy_open;	/* Automatically open doors */
#endif

#ifdef ALLOW_EASY_DISARM
extern bool easy_disarm;	/* Automatically disarm traps */
#endif

#ifdef ALLOW_EASY_FLOOR
extern bool easy_floor;	/* Display floor stacks in a list */
#endif

extern bool use_command;	/* Allow unified use command */
extern bool over_exert;	/* Allow casting spells when short of mana */
extern bool numpad_as_cursorkey;	/* Use numpad keys as cursor key in editor mode */


/*** Map Screen Options ***/

extern bool center_player;	/* Center map while walking (*slow*) */
extern bool center_running;	/* Centering even while running */
extern bool view_yellow_lite;	/* Use special colors for torch-lit grids */
extern bool view_bright_lite;	/* Use special colors for 'viewable' grids */
extern bool view_granite_lite;	/* Use special colors for wall grids (slow) */
extern bool view_special_lite;	/* Use special colors for floor grids (slow) */
extern bool view_perma_grids;	/* Map remembers all perma-lit grids */
extern bool view_torch_grids;	/* Map remembers all torch-lit grids */
extern bool view_unsafe_grids;	/* Map marked by detect traps */
extern bool view_reduce_view;	/* Reduce view-radius in town */
extern bool fresh_before;	/* Flush output while continuous command */
extern bool fresh_after;	/* Flush output after monster's move */
extern bool fresh_message;	/* Flush output after every message */
extern bool hilite_player;	/* Hilite the player with the cursor */
extern bool display_path;	/* Display actual path before shooting */


/*** Text Display Options ***/

extern bool plain_descriptions;	/* Plain object descriptions */
extern bool plain_pickup;	/* Plain pickup messages(japanese only) */
extern bool always_show_list;	/* Always show list when choosing items */
extern bool depth_in_feet;	/* Show dungeon level in feet */
extern bool show_labels;	/* Show labels in object listings */
extern bool show_weights;	/* Show weights in object listings */
extern bool show_item_graph;	/* Show items graphics */
extern bool equippy_chars;	/* Display 'equippy' chars */
extern bool display_mutations;	/* Display mutations in 'C'haracter Display */
extern bool compress_savefile;	/* Compress messages in savefiles */
extern bool abbrev_extra;	/* Describe obj's extra resistances by abbreviation */
extern bool abbrev_all;	/* Describe obj's all resistances by abbreviation */
extern bool exp_need;	/* Show the experience needed for next level */
extern bool ignore_unview;	/* Ignore whenever any monster does */
extern bool full_mon_list; //v1.1.66 モンスターリスト自動更新に画面外モンスターも含む
extern bool show_throwing_dam; //新オプション　武器の投擲ダメージを表示する
extern bool show_special_info; //ほか特殊な情報を表記　既読の新聞タイトル
extern bool show_actual_value; //v1.1.81 技能値に実値を表示

/*** Game-Play Options ***/

extern bool stack_force_notes;	/* Merge inscriptions when stacking */
extern bool stack_force_costs;	/* Merge discounts when stacking */
extern bool expand_list;	/* Expand the power of the list commands */
extern bool small_levels;	/* Allow unusually small dungeon levels */
extern bool always_small_levels;	/* Always create unusually small dungeon levels */
extern bool empty_levels;	/* Allow empty 'arena' levels */
extern bool bound_walls_perm;	/* Boundary walls become 'permanent wall' */
extern bool last_words;	/* Leave last words when your character dies */
extern bool ex_attack_msg;
extern bool auto_dump;

#ifdef WORLD_SCORE
extern bool send_score;	/* Send score dump to the world score server */
#endif

extern bool allow_debug_opts;	/* Allow use of debug/cheat options */


/*** Disturbance Options ***/

extern bool find_ignore_stairs;	/* Run past stairs */
extern bool find_ignore_doors;	/* Run through open doors */
extern bool find_cut;	/* Run past known corners */
extern bool check_abort;	/* Check for user abort while continuous command */
extern bool flush_failure;	/* Flush input on various failures */
extern bool flush_disturb;	/* Flush input whenever disturbed */
extern bool disturb_move;	/* Disturb whenever any monster moves */
extern bool disturb_high;	/* Disturb whenever high-level monster moves */
extern bool disturb_near;	/* Disturb whenever viewable monster moves */
extern bool disturb_pets;	/* Disturb when visible pets move */
extern bool disturb_panel;	/* Disturb whenever map panel changes */
extern bool disturb_state;	/* Disturb whenever player state changes */
extern bool disturb_minor;	/* Disturb whenever boring things happen */
extern bool ring_bell;	/* Audible bell (on errors, etc) */
extern bool disturb_trap_detect;	/* Disturb when leaving trap detected area */
extern bool alert_trap_detect;	/* Alert when leaving trap detected area */


/*** Birth Options ***/

extern bool manual_haggle;	/* Manually haggle in stores */
extern bool easy_band;	/* Easy Mode (*) */
extern bool smart_learn;	/* Monsters learn from their mistakes (*) */
extern bool smart_cheat;	/* Monsters exploit players weaknesses (*) */
extern bool vanilla_town;	/* Use 'vanilla' town without quests and wilderness */
extern bool lite_town;	/* Use 'lite' town without a wilderness */
extern bool ironman_shops;	/* Stores are permanently closed (*) */
extern bool ironman_small_levels;	/* Always create unusually small dungeon levels (*) */
extern bool ironman_downward;	/* Disable recall and use of up stairs (*) */
extern bool ironman_empty_levels;	/* Always create empty 'arena' levels (*) */
extern bool ironman_rooms;	/* Always generate very unusual rooms (*) */
extern bool ironman_nightmare;	/* Nightmare mode(it isn't even remotely fair!)(*) */
extern bool left_hander;	/* Left-Hander */
extern bool preserve_mode;	/* Preserve artifacts (*) */
extern bool autoroller;	/* Allow use of autoroller for stats (*) */
extern bool autochara;	/* Autoroll for weight, height and social status */
extern bool powerup_home;	/* Increase capacity of your home (*) */

extern bool ironman_no_fixed_art;
extern bool no_capture_book;
extern bool birth_bonus_20;
extern bool birth_bonus_10;
extern bool ironman_no_bonus;

/*** Easy Object Auto-Destroyer ***/

extern bool destroy_items;	/* Use easy auto-destroyer */
extern bool destroy_feeling;	/* Apply auto-destroy as sense feeling */
extern bool destroy_identify;	/* Apply auto-destroy as identify an item */
extern bool leave_worth;	/* Auto-destroyer leaves known worthy items */
extern bool leave_equip;	/* Auto-destroyer leaves weapons and armour */
extern bool leave_chest;	/* Auto-destroyer leaves closed chests */
extern bool leave_wanted;	/* Auto-destroyer leaves wanted corpses */
extern bool leave_corpse;	/* Auto-destroyer leaves corpses and skeletons */
extern bool leave_junk;	/* Auto-destroyer leaves junk */
extern bool leave_special;	/* Auto-destroyer leaves items your race/class needs */


/*** Play-record Options ***/

extern bool record_fix_art;	/* Record fixed artifacts */
extern bool record_rand_art;	/* Record random artifacts */
extern bool record_destroy_uniq;	/* Record when destroy unique monster */
extern bool record_fix_quest;	/* Record fixed quests */
extern bool record_rand_quest;	/* Record random quests */
extern bool record_maxdepth;	/* Record movements to deepest level */
extern bool record_stair;	/* Record recall and stair movements */
extern bool record_buy;	/* Record purchased items */
extern bool record_sell;	/* Record sold items */
extern bool record_danger;	/* Record hitpoint warning */
extern bool record_arena;	/* Record arena victories */
extern bool record_ident;	/* Record first identified items */
extern bool record_named_pet;	/* Record informations of named pets */


extern bool cheat_peek;
extern bool cheat_hear;
extern bool cheat_room;
extern bool cheat_xtra;
extern bool cheat_know;
extern bool cheat_live;
extern bool cheat_save;

extern char record_o_name[MAX_NLEN];
extern s32b record_turn;
extern byte hitpoint_warn;
extern byte mana_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern bool autosave_t;
extern bool autosave_l;
extern bool closing_flag;
extern s16b panel_row_min, panel_row_max;
extern s16b panel_col_min, panel_col_max;
extern s16b panel_col_prt, panel_row_prt;
extern int py;
extern int px;
extern s16b target_who;
extern s16b target_col;
extern s16b target_row;
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_name[32];
///mod140206 ＠の職業名を追加
extern char player_class_name[32];
extern char player_base[32];
extern char savefile[1024];
extern char savefile_base[40];
extern s16b lite_n;
extern s16b lite_y[LITE_MAX];
extern s16b lite_x[LITE_MAX];
extern s16b mon_lite_n;
extern s16b mon_lite_y[MON_LITE_MAX];
extern s16b mon_lite_x[MON_LITE_MAX];
extern s16b view_n;
extern byte view_y[VIEW_MAX];
extern byte view_x[VIEW_MAX];
extern s16b temp_n;
extern byte temp_y[TEMP_MAX];
extern byte temp_x[TEMP_MAX];
extern s16b redraw_n;
extern byte redraw_y[REDRAW_MAX];
extern byte redraw_x[REDRAW_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern s16b quark__num;
extern cptr *quark__str;
extern u16b message__next;
extern u16b message__last;
extern u16b message__head;
extern u16b message__tail;
extern u16b *message__ptr;
extern char *message__buf;
extern u32b option_flag[8];
extern u32b option_mask[8];
extern u32b window_flag[8];
extern u32b window_mask[8];
extern term *angband_term[8];
extern const char angband_term_name[8][16];
extern byte angband_color_table[256][4];
extern const cptr angband_sound_name[SOUND_MAX];
extern const cptr angband_music_basic_name[MUSIC_BASIC_MAX]; //v1.1.58
extern cave_type *cave[MAX_HGT];
extern saved_floor_type saved_floors[MAX_SAVED_FLOORS];
extern s16b max_floor_id;
extern u32b saved_floor_file_sign;
extern object_type *o_list;
extern monster_type *m_list;
extern s16b *mproc_list[MAX_MTIMED];
extern s16b mproc_max[MAX_MTIMED];
extern u16b max_towns;
extern town_type *town;
extern object_type *inventory;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_race_size;
extern alloc_entry *alloc_race_table;
extern byte misc_to_attr[256];
extern char misc_to_char[256];
extern byte tval_to_attr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern player_type *p_ptr;
extern const player_sex *sp_ptr;
extern const player_race *rp_ptr;
extern const player_class *cp_ptr;
extern const player_seikaku *ap_ptr;
//extern const player_magic *mp_ptr;
extern birther previous_char;
extern vault_type *v_info;
extern char *v_name;
extern char *v_text;
//extern skill_table *s_info;
//extern player_magic *m_info;
extern feature_type *f_info;
extern char *f_name;
extern char *f_tag;
extern object_kind *k_info;
extern char *k_name;
extern char *k_text;
extern artifact_type *a_info;
extern char *a_name;
extern char *a_text;
extern ego_item_type *e_info;
extern char *e_name;
extern char *e_text;
extern monster_race *r_info;
extern char *r_name;
extern char *r_text;
extern dungeon_info_type *d_info;
extern char *d_name;
extern char *d_text;
extern cptr ANGBAND_SYS;
extern cptr ANGBAND_KEYBOARD;
extern cptr ANGBAND_GRAF;
extern cptr ANGBAND_DIR;
extern cptr ANGBAND_DIR_APEX;
extern cptr ANGBAND_DIR_BONE;
extern cptr ANGBAND_DIR_DATA;
extern cptr ANGBAND_DIR_EDIT;
extern cptr ANGBAND_DIR_SCRIPT;
extern cptr ANGBAND_DIR_FILE;
extern cptr ANGBAND_DIR_HELP;
extern cptr ANGBAND_DIR_INFO;
extern cptr ANGBAND_DIR_PREF;
extern cptr ANGBAND_DIR_SAVE;
extern cptr ANGBAND_DIR_USER;
extern cptr ANGBAND_DIR_XTRA;
extern bool item_tester_full;
extern bool item_tester_no_ryoute;
extern byte item_tester_tval;
extern bool (*item_tester_hook)(object_type *o_ptr);
extern bool (*ang_sort_comp)(vptr u, vptr v, int a, int b);
extern void (*ang_sort_swap)(vptr u, vptr v, int a, int b);
extern monster_hook_type get_mon_num_hook;
extern monster_hook_type get_mon_num2_hook;
extern bool (*get_obj_num_hook)(int k_idx);
extern bool monk_armour_aux;
extern bool monk_notify_aux;
extern wilderness_type **wilderness;
extern building_type building[MAX_BLDG];
extern u16b max_quests;
extern u16b max_r_idx;
extern u16b max_k_idx;
extern u16b max_v_idx;
extern u16b max_f_idx;
extern u16b max_a_idx;
extern u16b max_e_idx;
extern u16b max_d_idx;
extern u16b max_o_idx;
extern u16b max_m_idx;
extern s32b max_wild_x;
extern s32b max_wild_y;
extern quest_type *quest;
extern char quest_text[10][80];
extern int quest_text_line;
extern s16b gf_color[MAX_GF];
extern int init_flags;
extern int highscore_fd;
extern int mutant_regenerate_mod;
extern bool can_save;
extern s16b world_monster;
extern bool world_player;
extern int cap_mon;
extern int cap_mspeed;
extern int cap_hp;
extern int cap_maxhp;
extern u16b cap_nickname;

///mod150614 闘技場変更
/*
extern s16b battle_mon[4];
extern int sel_monster;
extern int battle_odds;
extern int kakekin;
extern u32b mon_odds[4];
*/
extern int pet_t_m_idx;
extern int riding_t_m_idx;
extern s16b kubi_r_idx[MAX_KUBI];
extern s16b today_mon;
extern bool write_level;
extern u32b playtime;
extern u32b start_time;
extern int tsuri_dir;
//extern bool sukekaku;
extern bool new_mane;
extern bool mon_fight;
extern bool ambush_flag;
extern bool generate_encounter;
extern cptr screen_dump;

/*** Terrain feature variables ***/
extern s16b feat_none;
extern s16b feat_floor;
extern s16b feat_glyph;
extern s16b feat_explosive_rune;
extern s16b feat_mirror;
extern door_type feat_door[MAX_DOOR_TYPES];
extern s16b feat_up_stair;
extern s16b feat_down_stair;
extern s16b feat_entrance;
extern s16b feat_trap_open;
extern s16b feat_trap_armageddon;
extern s16b feat_trap_piranha;
extern s16b feat_rubble;
extern s16b feat_magma_vein;
extern s16b feat_quartz_vein;
extern s16b feat_granite;
extern s16b feat_permanent;
extern s16b feat_glass_floor;
extern s16b feat_glass_wall;
extern s16b feat_permanent_glass_wall;
extern s16b feat_pattern_start;
extern s16b feat_pattern_1;
extern s16b feat_pattern_2;
extern s16b feat_pattern_3;
extern s16b feat_pattern_4;
extern s16b feat_pattern_end;
extern s16b feat_pattern_old;
extern s16b feat_pattern_exit;
extern s16b feat_pattern_corrupted;
extern s16b feat_black_market;
extern s16b feat_town;
extern s16b feat_deep_water;
extern s16b feat_shallow_water;
extern s16b feat_deep_lava;
extern s16b feat_shallow_lava;
extern s16b feat_dirt;
extern s16b feat_grass;
extern s16b feat_flower;
extern s16b feat_brake;
extern s16b feat_tree;
extern s16b feat_mountain;
extern s16b feat_swamp;
extern s16b feat_undetected;

extern s16b feat_dark_pit;
extern s16b feat_elder_sign;

//v1.1.64 ダンジョン「地獄」用
extern s16b feat_needle_mat;
//v1.1.68 追加
extern s16b feat_ice_wall;

extern s16b feat_heavy_cold_zone;
extern s16b feat_cold_zone;
extern s16b feat_heavy_electrical_zone;
extern s16b feat_electrical_zone;
extern s16b feat_deep_acid_puddle;
extern s16b feat_acid_puddle;
extern s16b feat_deep_poisonous_puddle;
extern s16b feat_poisonous_puddle;
extern s16b feat_deep_miasma;
extern s16b feat_thin_miasma;

//v1.1.91
extern s16b feat_oil_field;



extern byte dungeon_type;
extern s16b *max_dlv;
extern s16b feat_wall_outer;
extern s16b feat_wall_inner;
extern s16b feat_wall_solid;
extern s16b floor_type[100], fill_type[100];
extern bool now_damaged;
extern s16b now_message;
extern bool use_menu;
//v1.1.25
extern byte *flag_dungeon_complete;


/* autopick.c */
extern void autopick_load_pref(bool disp_mes);
extern errr process_autopick_file_command(char *buf);
extern cptr autopick_line_from_entry(autopick_type *entry);
extern int is_autopick(object_type *o_ptr);
extern void autopick_alter_item(int item, bool destroy);
extern void autopick_delayed_alter(void);
extern void autopick_pickup_items(cave_type *c_ptr);
extern bool autopick_autoregister(object_type *o_ptr);
extern void do_cmd_edit_autopick(void);

/* birth.c */
extern void add_history_from_pref_line(cptr t);
extern void player_birth(void);
extern void get_max_stats(void);
extern void get_height_weight(void);
extern void determine_random_questor(quest_type *q_ptr);
extern void player_outfit(void);
extern void dump_yourself(FILE *fff);

/* cave.c */
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(int feat);
extern bool is_known_trap(cave_type *c_ptr);
extern bool is_closed_door(int feat);
extern bool is_hidden_door(cave_type *c_ptr);
extern bool los(int y1, int x1, int y2, int x2);
extern void update_local_illumination(int y, int x);
extern bool player_can_see_bold(int y, int x);
extern bool cave_valid_bold(int y, int x);
extern bool no_lite(void);
extern void apply_default_feat_lighting(byte f_attr[F_LIT_MAX], byte f_char[F_LIT_MAX]);
extern void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp);
extern void move_cursor_relative(int row, int col);
extern void print_rel(char c, byte a, int y, int x);
extern void note_spot(int y, int x);
extern void display_dungeon(void);
extern void lite_spot(int y, int x);
extern void prt_map(void);
extern void prt_path(int y, int x);
extern void display_map(int *cy, int *cx);
extern void do_cmd_view_map(void);
extern void forget_lite(void);
extern void update_lite(void);
extern void forget_view(void);
extern void update_view(void);
extern void update_mon_lite(void);
extern void clear_mon_lite(void);
extern void delayed_visual_update(void);
extern void forget_flow(void);
extern void update_flow(void);
extern void update_smell(void);
extern void map_area(int range);
extern void wiz_lite(bool ninja);
extern void wiz_dark(void);
extern void cave_set_feat(int y, int x, int feat);
extern int conv_dungeon_feat(int newfeat);
extern int feat_state(int feat, int action);
extern void cave_alter_feat(int y, int x, int action);
extern void remove_mirror(int y, int x);
extern bool is_mirror_grid(cave_type *c_ptr);
extern bool is_glyph_grid(cave_type *c_ptr);
extern bool is_explosive_rune_grid(cave_type *c_ptr);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool projectable(int y1, int x1, int y2, int x2);
extern void scatter(int *yp, int *xp, int y, int x, int d, int mode);
extern void health_track(int m_idx);
extern void monster_race_track(int r_idx);
extern void object_kind_track(int k_idx);
extern void disturb(int stop_search, int flush_output);
extern void glow_deep_lava_and_bldg(void);

/* cmd1.c */
extern bool test_hit_fire(int chance, int ac, int vis);
extern bool test_hit_norm(int chance, int ac, int vis);
extern s16b critical_shot(int weight, int plus, int dam);
//extern s16b critical_norm(int weight, int plus, int dam, s16b meichuu, int mode);
extern s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, int mode, bool thrown);
extern void search(void);
extern void py_pickup_aux(int o_idx);
extern void carry(bool pickup);
extern bool py_attack(int y, int x, int mode);
extern bool pattern_seq(int c_y, int c_x, int n_y, int n_x);
extern bool player_can_enter(s16b feature, u16b mode);
extern bool move_player_effect(int ny, int nx, u32b mpe_mode);
extern bool trap_can_be_ignored(int feat);
extern void move_player(int dir, bool do_pickup, bool break_trap, bool activate_trap);
extern void run_step(int dir);
#ifdef TRAVEL
extern void travel_step(void);
#endif

extern void activate_floor_trap(int y, int x, u32b mpe_mode);//v1.1.97

/* cmd2.c */
extern void forget_travel_flow(void);
extern bool confirm_leave_level(bool down_stair);
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void do_cmd_search(void);
extern void do_cmd_open(void);
extern void do_cmd_close(void);
extern void do_cmd_tunnel(void);
extern void do_cmd_disarm(void);
extern void do_cmd_bash(void);
extern void do_cmd_alter(void);
extern void do_cmd_spike(void);
extern void do_cmd_walk(bool pickup);
extern void do_cmd_stay(bool pickup);
extern void do_cmd_run(void);
extern void do_cmd_rest(void);
///mod160603 boolを返すことにした
extern bool do_cmd_fire(void);
extern void do_cmd_fire_aux(int item, object_type *j_ptr);
extern void do_cmd_throw(void);
extern bool do_cmd_throw_aux(int mult, bool boomerang, int shuriken);
#ifdef TRAVEL
extern void do_cmd_travel(void);
#endif

/* cmd3.c */
extern void do_cmd_inven(void);
extern void do_cmd_equip(void);
extern void do_cmd_wield(bool mochikae);
extern void do_cmd_takeoff(void);
extern void do_cmd_drop(void);
extern void do_cmd_destroy(void);
extern void do_cmd_observe(void);
extern void do_cmd_uninscribe(void);
extern void do_cmd_inscribe(void);
extern void do_cmd_refill(void);
extern void do_cmd_target(void);
extern void do_cmd_look(void);
extern void do_cmd_locate(void);
extern void do_cmd_query_symbol(void);
extern void kamaenaoshi(int item);
extern bool ang_sort_comp_hook(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_hook(vptr u, vptr v, int a, int b);

/* cmd4.c */
#ifndef JP
extern cptr get_ordinal_number_suffix(int num);
#endif
extern errr do_cmd_write_nikki(int type, int num, cptr note);
extern void do_cmd_nikki(void);
extern void do_cmd_redraw(void);
extern void do_cmd_change_name(void);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(int num_now);
extern void do_cmd_options_aux(int page, cptr info);
extern void do_cmd_options(void);
extern void do_cmd_pref(void);
extern void do_cmd_reload_autopick(void);
extern void do_cmd_macros(void);
extern void do_cmd_visuals(void);
extern void do_cmd_colors(void);
extern void do_cmd_note(void);
extern void do_cmd_version(void);
extern void do_cmd_feeling(void);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen_html_aux(char *filename, int message);
extern void do_cmd_save_screen(void);
extern void do_cmd_knowledge_quests_completed(FILE *fff, int quest_num[]);
extern void do_cmd_knowledge_quests_failed(FILE *fff, int quest_num[]);
extern bool ang_sort_comp_quest_num(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_quest_num(vptr u, vptr v, int a, int b);
extern void do_cmd_knowledge(void);
extern void plural_aux(char * Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(void);

/* cmd5.c */
extern cptr spell_category_name(int tval);
extern void do_cmd_browse(void);
extern void do_cmd_study(void);
extern void do_cmd_cast(void);
extern bool rakuba(int dam, bool force);
extern bool do_riding(bool force);
extern void check_pets_num_and_align(monster_type *m_ptr, bool inc);
extern int calculate_upkeep(void);
extern void do_cmd_pet_dismiss(void);
extern void do_cmd_pet(void);

/* cmd6.c */
extern void do_cmd_eat_food(void);
extern void do_cmd_quaff_potion(void);
extern void do_cmd_read_scroll(void);
extern void do_cmd_aim_wand(void);
extern void do_cmd_use_staff(void);
extern void do_cmd_zap_rod(void);
extern void do_cmd_activate(void);
extern void do_cmd_rerate_aux(void);
extern void do_cmd_rerate(bool display);
extern void ring_of_power(int dir);
extern void do_cmd_use(void);
extern bool do_cmd_magic_eater(bool only_browse, bool powerful);

/* do-spell.c */
extern void stop_singing(void);
extern cptr do_spell(int realm, int spell, int mode);

/* dungeon.c */
extern void leave_quest_check(void);
extern void leave_tower_check(void);
extern void extract_option_vars(void);
extern void determine_bounty_uniques(void);
//extern void determine_today_mon(bool conv_old);
extern void play_game(bool new_game);
extern bool psychometry(void);
extern void leave_level(int level);
extern void enter_level(int level);
extern s32b turn_real(s32b hoge);
extern void prevent_turn_overflow(void);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_player(int mode);
extern errr make_character_dump(FILE *fff);
extern errr file_character(cptr name);
extern errr process_pref_file_command(char *buf);
extern cptr process_pref_file_expr(char **sp, char *fp);
extern errr process_pref_file(cptr name);
extern errr process_autopick_file(cptr name);
extern errr process_histpref_file(cptr name);
extern void print_equippy(void);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(bool show_version, cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void process_player_name(bool sf);
extern void get_name(void);
extern void get_class_name(void);
extern void do_cmd_suicide(void);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
//extern long total_points(void);
extern void close_game(void);
extern void exit_game_panic(void);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);
extern int	calc_player_score_mult(void);

#ifdef JP
extern errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count);
#endif
extern errr counts_write(int where, u32b count);
extern u32b counts_read(int where);

/* flavor.c */
extern bool object_is_quest_target(object_type *o_ptr);
extern void get_table_name_aux(char *out_string);
extern void get_table_name(char *out_string);
extern void get_table_sindarin_aux(char *out_string);
extern void get_table_sindarin(char *out_string, bool use_brackets);
extern void flavor_init(void);
extern char *object_desc_kosuu(char *t, object_type *o_ptr);
extern void object_desc(char *buf, object_type *o_ptr, u32b mode);

/* floors.c */
extern void init_saved_floors(bool force);
extern void clear_saved_floor_files(void);
extern saved_floor_type *get_sf_ptr(s16b floor_id);
extern s16b get_new_floor_id(void);
extern void prepare_change_floor_mode(u32b mode);
extern void precalc_cur_num_of_pet(void);
extern void leave_floor(void);
extern void change_floor(void);
extern void stair_creation(void);

/* generate.c */
extern bool place_quest_monsters(void);
extern void wipe_generate_cave_flags(void);
extern void clear_cave(void);
extern void generate_cave(void);

/* init1.c */
extern byte color_char_to_attr(char c);
extern s16b f_tag_to_index(cptr str);
extern errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax, int xmax);

/* init2.c */
extern void init_file_paths(char *path);
extern cptr err_str[PARSE_ERROR_MAX];
extern errr init_v_info(void);
extern errr init_buildings(void);
extern s16b f_tag_to_index_in_init(cptr str);
extern void init_angband(void);
extern cptr get_check_sum(void);

/* load.c */
extern errr rd_savefile_new(void);
extern bool load_floor(saved_floor_type *sf_ptr, u32b mode);

/* melee1.c */
/* melee2.c */
extern bool make_attack_normal(int m_idx);
extern void process_monsters(void);
extern int get_mproc_idx(int m_idx, int mproc_type);
extern void mproc_init(void);
extern bool set_monster_csleep(int m_idx, int v);
extern bool set_monster_fast(int m_idx, int v);
extern bool set_monster_slow(int m_idx, int v);
extern bool set_monster_stunned(int m_idx, int v);
extern bool set_monster_confused(int m_idx, int v);
extern bool set_monster_monfear(int m_idx, int v);
extern bool set_monster_invulner(int m_idx, int v, bool energy_need);
extern void process_monsters_mtimed(int mtimed_idx);
extern void dispel_monster_status(int m_idx);
extern u32b get_curse(int power, object_type *o_ptr);
extern void curse_equipment(int chance, int heavy_chance);
extern void mon_take_hit_mon(int m_idx, int dam, bool *fear, cptr note, int who);
extern bool process_the_world(int num, int who, bool vs_player);
extern void monster_gain_exp(int m_idx, int s_idx);

//v1.1.94
extern bool set_monster_timed_status_add(int mtimed_type, int m_idx, int v);

/* monster1.c */
extern void roff_top(int r_idx);
extern void screen_roff(int r_idx, int mode);
extern void display_roff(int r_idx);
extern void output_monster_spoiler(int r_idx, void (*roff_func)(byte attr, cptr str));
extern void create_name(int type, char *name);
extern bool mon_hook_dungeon(int r_idx);

extern monster_hook_type get_monster_hook(void);
extern monster_hook_type get_monster_hook2(int y, int x);
extern void set_friendly(monster_type *m_ptr);
extern void set_pet(monster_type *m_ptr);
extern void set_hostile(monster_type *m_ptr);
extern void anger_monster(monster_type *m_ptr);
extern bool monster_can_cross_terrain(s16b feat, monster_race *r_ptr, u16b mode);
extern bool monster_can_enter(int y, int x, monster_race *r_ptr, u16b mode);
extern bool are_enemies(monster_type *m_ptr1, monster_type *m_ptr2);
extern bool monster_has_hostile_align(monster_type *m_ptr, int pa_good, int pa_evil, monster_race *r_ptr);
extern bool monster_living(monster_race *r_ptr);
extern bool no_questor_or_bounty_uniques(int r_idx);


/* monster2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void set_target(monster_type *m_ptr, int y, int x);
extern void reset_target(monster_type *m_ptr);
extern monster_race *real_r_ptr(monster_type *m_ptr);
extern void delete_monster_idx(int i);
extern void delete_monster(int y, int x);
extern void compact_monsters(int size);
extern void wipe_m_list(void);
extern s16b m_pop(void);
extern errr get_mon_num_prep(monster_hook_type monster_hook, monster_hook_type monster_hook2);
extern s16b get_mon_num(int level);
extern void monster_desc(char *desc, monster_type *m_ptr, int mode);
extern int lore_do_probe(int r_idx);
extern void lore_treasure(int m_idx, int num_item, int num_gold);
extern void sanity_blast(monster_type *m_ptr, bool necro);
extern void update_mon(int m_idx, bool full);
extern void update_monsters(bool full);
extern bool place_monster_aux(int who, int y, int x, int r_idx, u32b mode);
extern bool place_monster(int y, int x, u32b mode);
extern bool alloc_horde(int y, int x);
extern bool alloc_guardian(bool def_val);
extern bool alloc_monster(int dis, u32b mode);
extern bool summon_specific(int who, int y1, int x1, int lev, int type, u32b mode);
extern bool summon_named_creature (int who, int oy, int ox, int r_idx, u32b mode);
extern bool multiply_monster(int m_idx, bool clone, u32b mode);
extern void update_smart_learn(int m_idx, int what);
extern void choose_new_monster(int m_idx, bool born, int r_idx);
extern byte get_mspeed(monster_race *r_ptr);
extern bool player_place(int y, int x);
extern void monster_drop_carried_objects(monster_type *m_ptr);

/* object1.c */
extern s16b m_bonus(int max, int level);

extern void reset_visuals(void);
extern void object_flags(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern void object_flags_known(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern cptr item_activation(object_type *o_ptr);
extern bool screen_object(object_type *o_ptr, u32b mode);
extern char index_to_label(int i);
extern s16b label_to_inven(int c);
extern s16b label_to_equip(int c);
extern s16b wield_slot(object_type *o_ptr);
extern cptr mention_use(int i);
extern cptr describe_use(int i);
extern bool check_book_realm(const byte book_tval, const byte book_sval);
extern bool item_tester_okay(object_type *o_ptr);
extern void display_inven(void);
extern void display_equip(void);
extern int show_inven(int target_item);
extern int show_equip(int target_item);
extern void toggle_inven_equip(void);
extern bool can_get_item(void);
extern bool get_item(int *cp, cptr pmt, cptr str, int mode);

/* object2.c */
extern void excise_object_idx(int o_idx);
extern void delete_object_idx(int o_idx);
extern void delete_object(int y, int x);
extern void compact_objects(int size);
extern void wipe_o_list(void);
extern s16b o_pop(void);
extern s16b get_obj_num(int level);
extern void object_known(object_type *o_ptr);
extern void object_aware(object_type *o_ptr);
extern void object_tried(object_type *o_ptr);
extern s32b object_value(object_type *o_ptr);
extern s32b object_value_real(object_type *o_ptr);
extern bool can_player_destroy_object(object_type *o_ptr);
extern void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt);
extern void reduce_charges(object_type *o_ptr, int amt);
extern int object_similar_part(object_type *o_ptr, object_type *j_ptr);
extern bool object_similar(object_type *o_ptr, object_type *j_ptr);
extern void object_absorb(object_type *o_ptr, object_type *j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type *o_ptr);
extern void object_prep(object_type *o_ptr, int k_idx);
extern void object_copy(object_type *o_ptr, object_type *j_ptr);
extern void apply_magic(object_type *o_ptr, int lev, u32b mode);
extern bool make_object(object_type *j_ptr, u32b mode);
extern void place_object(int y, int x, u32b mode);
extern bool make_gold(object_type *j_ptr);
extern void place_gold(int y, int x);
extern s16b drop_near(object_type *o_ptr, int chance, int y, int x);
extern void acquirement(int y1, int x1, int num, bool great, bool known);
extern void amusement(int y1, int x1, int num, bool known);
extern void init_normal_traps(void);
extern s16b choose_random_trap(void);
extern void disclose_grid(int y, int x);
extern void place_trap(int y, int x);
extern void inven_item_charges(int item);
extern void inven_item_describe(int item);
extern void inven_item_increase(int item, int num);
extern void inven_item_optimize(int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(object_type *o_ptr);
extern bool object_sort_comp(object_type *o_ptr, s32b o_value, object_type *j_ptr);
extern s16b inven_carry(object_type *o_ptr);
extern s16b inven_takeoff(int item, int amt);
extern void inven_drop(int item, int amt);
extern void combine_pack(void);
extern void reorder_pack(void);
extern void display_koff(int k_idx);
extern object_type *choose_warning_item(void);
extern bool process_warning(int xx, int yy, bool sagume_check);
extern void do_cmd_kaji(bool only_browse);
extern void torch_flags(object_type *o_ptr, u32b *flgs);
extern void torch_dice(object_type *o_ptr, int *dd, int *ds);
extern void torch_lost_fuel(object_type *o_ptr);
extern bool item_monster_okay(int r_idx);

/* racial.c */
extern bool gain_magic(void);
///mod140226 info表示を追加
extern void do_cmd_racial_power(bool only_info);

/* save.c */
extern bool save_player(void);
extern bool load_player(void);
extern void remove_loc(void);
extern bool save_floor(saved_floor_type *sf_ptr, u32b mode);

/* spells1.c */
extern bool in_disintegration_range(int y1, int x1, int y2, int x2);
extern void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ,bool masterspark);
extern int take_hit(int damage_type, int damage, cptr kb_str, int monspell);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(u16b *gp, int range, int y1, int x1, int y2, int x2, int flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(int who, int rad, int y, int x, int dam, int typ, int flg, int monspell);
extern int project_length;
extern bool binding_field(int dam);
extern void seal_of_mirror(int dam);

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(void);
extern bool detect_traps(int range, bool known);
extern bool detect_doors(int range);
extern bool detect_stairs(int range);
extern bool detect_treasure(int range);
extern bool detect_objects_gold(int range);
extern bool detect_objects_normal(int range);
extern bool detect_objects_magic(int range);
extern bool detect_monsters_normal(int range);
extern bool detect_monsters_invis(int range);
extern bool detect_monsters_evil(int range);
extern bool detect_monsters_xxx(int range, u32b match_flag);
extern bool detect_monsters_string(int range, cptr);
extern bool detect_monsters_nonliving(int range);
extern bool detect_monsters_mind(int range);
extern bool detect_all(int range);
extern bool wall_stone(void);
extern bool speed_monsters(void);
extern bool slow_monsters(int power);
extern bool sleep_monsters(int power);
///mod140227 悲鳴とアラームに分けた
extern void aggravate_monsters(int who, bool alarm);
extern bool genocide_aux(int m_idx, int power, bool player_cast, int dam_side, cptr spell_name);
extern bool symbol_genocide(int power, bool player_cast, int default_char);
extern bool mass_genocide(int power, bool player_cast);
extern bool mass_genocide_undead(int power, bool player_cast);
extern bool probing(void);
extern bool banish_evil(int dist);
extern bool dispel_evil(int dam);
extern bool dispel_good(int dam);
extern bool dispel_undead(int dam);
extern bool dispel_monsters(int dam);
extern bool dispel_living(int dam);
extern bool dispel_demons(int dam);
extern bool crusade(void);
extern bool turn_undead(void);
extern bool destroy_area(int y1, int x1, int r, bool in_generate, bool force_floor, bool flan);
extern bool earthquake_aux(int cy, int cx, int r, int m_idx);
extern bool earthquake(int cy, int cx, int r);
extern void lite_room(int y1, int x1);
extern void unlite_room(int y1, int x1);
extern bool lite_area(int dam, int rad);
extern bool unlite_area(int dam, int rad);
extern bool fire_ball(int typ, int dir, int dam, int rad);
extern bool fire_rocket(int typ, int dir, int dam, int rad);
extern bool fire_ball_hide(int typ, int dir, int dam, int rad);
extern bool fire_meteor(int who, int typ, int x, int y, int dam, int rad);
extern bool fire_bolt(int typ, int dir, int dam);
///mod140212 スターダストのルーチン　フラグを追加できるようにした
extern bool fire_blast(int typ, int dir, int dd, int ds, int num, int dev, int flg_add);
extern void call_chaos(void);
extern bool fire_beam(int typ, int dir, int dam);
extern bool fire_bolt_or_beam(int prob, int typ, int dir, int dam);
extern bool lite_line(int dir, int dam);
extern bool drain_life(int dir, int dam);
extern bool death_ray(int dir, int plev);
extern bool wall_to_mud(int dir, int dam);
extern bool destroy_door(int dir);
extern bool disarm_trap(int dir);
extern bool wizard_lock(int dir);
extern bool heal_monster(int dir, int dam);
extern bool speed_monster(int dir, int power);
extern bool slow_monster(int dir, int power);
extern bool sleep_monster(int dir, int power);
extern bool stasis_monster(int dir);    /* Like sleep, affects undead as well */
extern bool stasis_evil(int dir);    /* Like sleep, affects undead as well */
extern bool confuse_monster(int dir, int plev);
extern bool stun_monster(int dir, int plev);
extern bool fear_monster(int dir, int plev);
extern bool poly_monster(int dir, int power);
extern bool clone_monster(int dir);
extern bool teleport_monster(int dir, int distance);
extern bool door_creation(int rad);
extern bool trap_creation(int y, int x);
extern bool tree_creation(void);
extern bool glyph_creation(void);
extern bool destroy_doors_touch(void);
extern bool disarm_traps_touch(void);
extern bool animate_dead(int who, int y, int x);
extern bool sleep_monsters_touch(void);
extern bool activate_ty_curse(bool stop_ty, int *count);
extern int activate_hi_summon(int y, int x, bool can_pet);
extern int summon_cyber(int who, int y, int x);
extern void wall_breaker(void);
extern bool confuse_monsters(int dam);
extern bool charm_monsters(int dam);
extern bool charm_animals(int dam);
extern bool charm_undead(int dam);
extern bool stun_monsters(int dam);
extern bool stasis_monsters(int dam);
extern bool banish_monsters(int dist);
extern bool turn_monsters(int dam);
extern bool turn_evil(int dam);
extern bool deathray_monsters(void);
extern bool charm_monster(int dir, int plev);
extern bool control_one_undead(int dir, int plev);
extern bool control_one_demon(int dir, int plev);
extern bool charm_animal(int dir, int plev);
extern bool charm_living(int dir, int plev);
extern bool mindblast_monsters(int dam);
extern s32b flag_cost(object_type *o_ptr, int plusses, bool flag_junko);
extern void report_magics(void);
extern bool teleport_swap(int dir);
extern bool item_tester_hook_recharge(object_type *o_ptr);
extern bool item_tester_hook_staff_rod_wand(object_type *o_ptr);//v1.1.86
extern bool project_hook(int typ, int dir, int dam, int flg);
extern bool project_hack(int typ, int dam);
extern bool eat_magic(int power);
extern void discharge_minion(void);
extern bool kawarimi(bool success);
extern bool rush_attack(bool *mdeath, int len, int mode);
extern void remove_all_mirrors(bool explode);

/* spells3.c */
extern bool teleport_away(int m_idx, int dis, u32b mode);
extern void teleport_monster_to(int m_idx, int ty, int tx, int power, u32b mode);
extern bool cave_player_teleportable_bold(int y, int x, u32b mode);
extern bool teleport_player_aux(int dis, u32b mode);
extern void teleport_player(int dis, u32b mode);
extern void teleport_player_away(int m_idx, int dis);
extern void teleport_player_to(int ny, int nx, u32b mode);
extern void teleport_away_followable(int m_idx);
extern void teleport_level(int m_idx);
extern int choose_dungeon(cptr note, int y, int x);
extern bool recall_player(int turns);
extern bool word_of_recall(void);
extern bool reset_recall(void);
extern bool apply_disenchant(int mode, int slot);
extern void mutate_player(void);
extern void apply_nexus(monster_type *m_ptr);
///del131214
//extern void phlogiston(void);
extern void brand_weapon(int brand_type);
extern void call_the_(void);
extern void fetch(int dir, int wgt, bool require_los);
extern void alter_reality(void);
extern bool warding_glyph(void);
extern bool place_mirror(void);
extern bool explosive_rune(void);
extern void identify_pack(void);
extern bool remove_curse(void);
extern bool remove_all_curse(void);
extern bool alchemy(int mode);
extern bool enchant(object_type *o_ptr, int n, int eflag);
extern bool enchant_spell(int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(void);
extern bool ident_spell(bool only_equip);
extern bool mundane_spell(bool only_equip);
extern bool identify_item(object_type *o_ptr);
extern bool identify_fully(bool only_equip);
extern bool recharge(int num);
extern bool bless_weapon(void);
extern bool pulish_shield(void);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(void);
extern s16b experience_of_spell(int spell, int realm);
extern int mod_need_mana(int need_mana, int spell, int realm);
extern int mod_spell_chance_1(int chance);
extern int mod_spell_chance_2(int chance);
extern s16b spell_chance(int spell,int realm);
extern bool spell_okay(int spell, bool learned, bool study_pray, int realm);
extern void print_spells(int target_spell, byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type *o_ptr);
extern bool hates_elec(object_type *o_ptr);
extern bool hates_fire(object_type *o_ptr);
extern bool hates_cold(object_type *o_ptr);
extern int set_acid_destroy(object_type *o_ptr);
extern int set_elec_destroy(object_type *o_ptr);
extern int set_fire_destroy(object_type *o_ptr);
extern int set_cold_destroy(object_type *o_ptr);
extern int inven_damage(inven_func typ, int perc);
extern int acid_dam(int dam, cptr kb_str, int monspell);
extern int elec_dam(int dam, cptr kb_str, int monspell);
extern int fire_dam(int dam, cptr kb_str, int monspell);
extern int cold_dam(int dam, cptr kb_str, int monspell);
extern bool rustproof(void);
extern bool curse_armor(void);
extern bool curse_weapon_object(bool force, object_type *o_ptr);
extern bool curse_weapon(bool force, int slot);
extern bool brand_bolts(void);
extern bool polymorph_monster(int y, int x);
extern bool dimension_door(int mode);
extern bool mirror_tunnel(void);
extern bool summon_kin_player(int level, int y, int x, u32b mode);

/* store.c */
extern bool combine_and_reorder_home(int store_num);
extern void do_cmd_store(void);
//extern void store_shuffle(int which);
extern void store_maint(int town_num, int store_num);
extern void store_init(int town_num, int store_num);
extern void move_to_black_market(object_type * o_ptr);

/* bldg.c */
extern bool get_nightmare(int r_idx);
extern void have_nightmare(int r_idx);
//extern void battle_monsters(void);
extern void do_cmd_bldg(void);
extern void do_cmd_quest(void);
extern void quest_discovery(int q_idx);
extern int quest_number(int level);
extern int random_quest_number(int level);
extern bool tele_town(bool alltown);
extern s32b calc_expect_crit(int weight, int plus, int dam, s16b meichuu, bool dokubari);

/* util.c */
extern errr path_parse(char *buf, int max, cptr file);
extern errr path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern FILE *my_fopen_temp(char *buf, int max);
extern errr my_fgets(FILE *fff, char *buf, huge n);
extern errr my_fputs(FILE *fff, cptr buf, huge n);
extern errr my_fclose(FILE *fff);
extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern errr fd_copy(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_chop(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);
extern void flush(void);
extern void bell(void);
extern void sound(int num);
extern void move_cursor(int row, int col);
extern void text_to_ascii(char *buf, cptr str);
extern void ascii_to_text(char *buf, cptr str);
extern errr macro_add(cptr pat, cptr act);
extern sint macro_find_exact(cptr pat);
extern char inkey(void);
extern cptr quark_str(s16b num);
extern void quark_init(void);
extern s16b quark_add(cptr str);
extern s16b message_num(void);
extern cptr message_str(int age);
extern void message_add(cptr msg);
extern void msg_print(cptr msg);
#ifndef SWIG
extern void msg_format(cptr fmt, ...);
#endif /* SWIG */
extern void screen_save(void);
extern void screen_load(void);
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(byte attr, cptr str, int row, int col);
extern void prt(cptr str, int row, int col);
extern void c_roff(byte attr, cptr str);
extern void roff(cptr str);
extern void clear_from(int row);
extern bool askfor_aux(char *buf, int len, bool numpad_cursor);
extern bool askfor(char *buf, int len);
extern bool get_string(cptr prompt, char *buf, int len);
extern bool get_check(cptr prompt);
extern bool get_check_strict(cptr prompt, int mode);
extern bool get_com(cptr prompt, char *command, bool z_escape);
extern s16b get_quantity(cptr prompt, int max);
extern void pause_line(int row);
extern void request_command(int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);
extern errr type_string(cptr str, uint len);
extern void roff_to_buf(cptr str, int wlen, char *tbuf, size_t bufsize);
//v1.1.58 本家BGM機能をコピー 演奏開始処理成功で0が返る
extern errr play_music(int type, int num);
extern void select_floor_music(void);


#ifdef SORT_R_INFO
extern void tag_sort(tag_type elements[], int number);
#endif /* SORT_R_INFO */

#ifdef SUPPORT_GAMMA
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
#endif /* SUPPORT_GAMMA */

extern size_t my_strcpy(char *buf, const char *src, size_t bufsize);
extern size_t my_strcat(char *buf, const char *src, size_t bufsize);
extern char *my_strstr(const char *haystack, const char *needle);
extern char *my_strchr(const char *ptr, char ch);
extern void str_tolower(char *str);
extern int inkey_special(bool numpad_cursor);


/* xtra1.c */
extern void cnv_stat(int val, char *out_val);
extern s16b modify_stat_value(int value, int amount);
extern bool is_daytime(void);
extern bool is_midnight(void);//v1.1.85
extern void extract_day_hour_min(int *day, int *hour, int *min);
extern void prt_time(void);
extern cptr map_name(void);
extern u32b weight_limit(void);
extern bool buki_motteruka(int i);
extern void calc_bonuses(void);
extern void notice_stuff(void);
extern void update_stuff(void);
extern void redraw_stuff(void);
extern void window_stuff(void);
extern void handle_stuff(void);
extern s16b empty_hands(bool riding_control);
extern bool heavy_armor(void);
extern void print_monster_list(int x, int y, int max_lines);


/* effects.c */
extern void set_action(int typ);
extern void reset_tim_flags(void);
extern void dispel_player(void);
extern bool set_mimic(int v, int p, bool do_dec);
extern bool set_blind(int v);
extern bool set_confused(int v);
extern bool set_poisoned(int v);
extern bool set_afraid(int v);
extern bool set_paralyzed(int v);
extern bool set_image(int v);
extern bool set_fast(int v, bool do_dec);
extern bool set_slow(int v, bool do_dec);
extern bool set_shield(int v, bool do_dec);
//extern bool set_tsubureru(int v, bool do_dec);
extern bool set_tim_sh_death(int v, bool do_dec);
extern bool set_magicdef(int v, bool do_dec);
extern bool set_blessed(int v, bool do_dec);
extern bool set_hero(int v, bool do_dec);
extern bool set_shero(int v, bool do_dec);
extern bool set_protevil(int v, bool do_dec);
extern bool set_invuln(int v, bool do_dec);
extern bool set_tim_invis(int v, bool do_dec);
extern bool set_tim_infra(int v, bool do_dec);
extern bool set_tim_regen(int v, bool do_dec);
extern bool set_tim_stealth(int v, bool do_dec);
extern bool set_lightspeed(int v, bool do_dec);
extern bool set_tim_levitation(int v, bool do_dec);
extern bool set_tim_sh_touki(int v, bool do_dec);
extern bool set_tim_sh_fire(int v, bool do_dec);
extern bool set_tim_sh_holy(int v, bool do_dec);
extern bool set_tim_eyeeye(int v, bool do_dec);
extern bool set_resist_magic(int v, bool do_dec);
extern bool set_tim_reflect(int v, bool do_dec);
extern bool set_multishadow(int v, bool do_dec);
extern bool set_dustrobe(int v, bool do_dec);
extern bool set_kabenuke(int v, bool do_dec);
extern bool set_tsuyoshi(int v, bool do_dec);
extern bool set_ele_attack(u32b attack_type, int v);
extern bool set_ele_immune(u32b immune_type, int v);
extern bool set_oppose_acid(int v, bool do_dec);
extern bool set_oppose_elec(int v, bool do_dec);
extern bool set_oppose_fire(int v, bool do_dec);
extern bool set_oppose_cold(int v, bool do_dec);
extern bool set_oppose_pois(int v, bool do_dec);
extern bool set_stun(int v);
extern bool set_cut(int v);
extern bool set_food(int v);
extern bool inc_stat(int stat);
extern bool dec_stat(int stat, int amount, int permanent);
extern bool res_stat(int stat);
extern bool hp_player(int num);
extern bool do_dec_stat(int stat);
extern bool do_res_stat(int stat);
extern bool do_inc_stat(int stat);
extern bool restore_level(void);
extern bool lose_all_info(void);
extern void gain_exp_64(s32b amount, u32b amount_frac);
extern void gain_exp(s32b amount);
extern void calc_android_exp(void);
extern void lose_exp(s32b amount);
extern bool drain_exp(s32b drain, s32b slip, int hold_life_prob);
extern void do_poly_self(void);
extern bool set_ultimate_res(int v, bool do_dec);
extern bool set_tim_res_nether(int v, bool do_dec);
extern bool set_tim_res_time(int v, bool do_dec);
extern bool choose_ele_attack(void);
extern bool choose_ele_immune(int turn);
extern bool set_wraith_form(int v, bool do_dec);
extern bool set_tim_esp(int v, bool do_dec);
extern bool set_superstealth(bool set);
extern bool set_tim_res_dark(int v, bool do_dec);


extern bool set_reactive_heal(int v, bool do_dec);

/* xtra2.c */
extern void check_experience(void);
extern void complete_quest(int quest_num);
extern void check_quest_completion(monster_type *m_ptr);
extern void check_find_art_quest_completion(object_type *o_ptr);
extern cptr extract_note_dies(monster_race *r_ptr);
extern void monster_death(int m_idx, bool drop_item);
extern bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note);
extern void get_screen_size(int *wid_p, int *hgt_p);
extern void panel_bounds_center(void);
extern void resize_map(void);
extern void redraw_window(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(void);
extern cptr look_mon_desc(monster_type *m_ptr, u32b mode);
extern void ang_sort_aux(vptr u, vptr v, int p, int q);
extern void ang_sort(vptr u, vptr v, int n);
extern bool target_able(int m_idx);
extern bool target_okay(void);
extern bool target_set(int mode);
extern void target_set_prepare_look();
extern void target_set_prepare_full();
extern bool get_aim_dir(int *dp);
extern bool get_hack_dir(int *dp);
extern bool get_rep_dir(int *dp, bool under);
extern bool get_rep_dir2(int *dp);
extern void gain_level_reward(int chosen_reward);
extern bool tgt_pt (int *x, int *y);
extern void do_poly_wounds(void);
extern void change_race(int new_race, cptr effect_msg);
extern int mon_damage_mod(monster_type *m_ptr, int dam, bool is_psy_spear);
//extern s16b gain_energy(void);
///mod131230 弓のTVAL変更により設定し直し
extern s16b bow_energy(int tval, int sval);
extern int bow_tmul(int tval, int sval);
extern cptr your_alignment(void);
extern int weapon_exp_level(int weapon_exp);
extern int riding_exp_level(int riding_exp);
extern int spell_exp_level(int spell_exp);
extern void display_rumor(bool ex);

/* mspells1.c */
extern bool clean_shot(int y1, int x1, int y2, int x2, bool _friend);
extern bool summon_possible(int y1, int x1);
extern bool raise_possible(monster_type *m_ptr);
extern bool dispel_check(int m_idx);
extern bool spell_is_inate(u16b spell);
extern bool make_attack_spell(int m_idx, int special_flag);

/* mspells2.c */
extern void get_project_point(int sy, int sx, int *ty, int *tx, int flg);
extern bool monst_spell_monst(int m_idx);

/* artifact.c */
extern void one_sustain(object_type *o_ptr);
extern void one_high_resistance(object_type *o_ptr);
extern void one_lordly_high_resistance(object_type *o_ptr);
extern void one_ele_resistance(object_type *o_ptr);
extern void one_dragon_ele_resistance(object_type *o_ptr);
extern void one_low_esp(object_type *o_ptr);
extern void one_resistance(object_type *o_ptr);
extern void one_ability(object_type *o_ptr);
extern void one_activation(object_type *o_ptr);
extern bool create_artifact(object_type *o_ptr, bool a_scroll);
extern int activation_index(object_type *o_ptr);
extern bool activate_random_artifact(object_type * o_ptr, int item);
extern const activation_type* find_activation_info(object_type *o_ptr);
extern void get_bloody_moon_flags(object_type *o_ptr);
extern void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr);
extern bool create_named_art(int a_idx, int y, int x);

/* scores.c */
extern void display_scores_aux(int from, int to, int note, high_score *score);
extern void display_scores(int from, int to);
extern void kingly(void);
extern bool send_world_score(bool do_send);
extern errr top_twenty(void);
extern errr predict_score(void);
extern void race_legends(void);
extern void race_score(int race_num);
extern void show_highclass(void);

/* mind.c */
extern mind_power mind_powers[5];
extern void mindcraft_info(char *p, int use_mind, int power);
extern void do_cmd_mind(void);
extern void do_cmd_mind_browse(void);

/* mane.c */
extern bool do_cmd_mane(bool baigaesi);

/* mspells3.c */
extern bool do_cmd_cast_learned(void);
extern void learn_spell(int monspell);
extern void set_rf_masks(s32b *f4, s32b *f5, s32b *f6, int mode);
//v1.1.82 新職業「弾幕研究家」用
extern void learn_monspell_new(int thrown_spell, monster_type *m_ptr, bool flag_direct, bool flag_look, int caster_dist);
extern bool check_monspell_learned(int monspell_num);

/* hissatsu.c */
//extern void hissatsu_info(char *p, int power);
extern void do_cmd_hissatsu(void);
extern void do_cmd_hissatsu_browse(void);
extern void do_cmd_gain_hissatsu(void);
extern s16b mult_hissatsu(int mult, u32b *flgs, monster_type *m_ptr, int mode);

/* mutation.c */
extern int count_bits(u32b x);
extern bool gain_random_mutation(int choose_mut);
extern bool lose_mutation(int choose_mut);
extern void dump_mutations(FILE *OutFile);
extern void do_cmd_knowledge_mutations(void);
extern int calc_mutant_regenerate_mod(void);
extern bool mutation_power_aux(u32b power);


/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void user_name(char *buf, int id);
#endif

#if 0
#ifndef HAS_STRICMP
/* util.c */
extern int stricmp(cptr a, cptr b);
#endif
#endif

#ifndef HAVE_USLEEP
/* util.c */
extern int usleep(huge usecs);
#endif

#ifdef MACINTOSH
/* main-mac.c */
/* extern void main(void); */
#endif

#if defined(MAC_MPW) || defined(MACH_O_CARBON)
/* Globals needed */
extern  u32b _ftype;
extern  u32b _fcreator;
#endif

#if defined(MAC_MPW) && defined(CARBON)
extern void convert_pathname(char *path);
#endif

#if defined(MACH_O_CARBON)
extern void fsetfileinfo(cptr path, u32b fcreator, u32b ftype);
#endif

#ifdef WINDOWS
/* main-win.c */
/* extern int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, ...); */
#endif


#ifdef ALLOW_REPEAT /* TNB */

/* util.c */
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

#endif /* ALLOW_REPEAT -- TNB */

#ifdef ALLOW_EASY_OPEN /* TNB */

/* variable.c */
extern bool easy_open;

/* cmd2.c */
extern bool easy_open_door(int y, int x);

#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM /* TNB */

/* variable.c */
extern bool easy_disarm;

/* cmd2.c */
extern bool do_cmd_disarm_aux(int y, int x, int dir);

#endif /* ALLOW_EASY_DISARM -- TNB */


#ifdef ALLOW_EASY_FLOOR /* TNB */

/* object1.c */
extern int scan_floor(int *items, int y, int x, int mode);
extern int show_floor(int target_item, int y, int x, int *min_width);
extern bool get_item_floor(int *cp, cptr pmt, cptr str, int mode);
extern void py_pickup_floor(bool pickup);

/* variable.c */
extern bool easy_floor;

#endif /* ALLOW_EASY_FLOOR -- TNB */

/* obj_kind.c */
extern bool object_is_potion(object_type *o_ptr);
///del131221 賞金首
//extern bool object_is_shoukinkubi(object_type *o_ptr);
extern bool object_is_favorite(object_type *o_ptr);
extern bool object_is_rare(object_type *o_ptr);
extern bool object_is_weapon(object_type *o_ptr);
extern bool object_is_weapon_ammo(object_type *o_ptr);
extern bool object_is_ammo(object_type *o_ptr);
extern bool object_is_armour(object_type *o_ptr);
extern bool object_is_weapon_armour_ammo(object_type *o_ptr);
extern bool object_is_melee_weapon(object_type *o_ptr);
extern bool object_is_wearable(object_type *o_ptr);
extern bool object_is_equipment(object_type *o_ptr);
extern bool object_refuse_enchant_weapon(object_type *o_ptr);
extern bool object_allow_enchant_weapon(object_type *o_ptr);
extern bool object_allow_enchant_melee_weapon(object_type *o_ptr);
extern bool object_is_smith(object_type *o_ptr);
extern bool object_is_artifact(object_type *o_ptr);
extern bool object_is_random_artifact(object_type *o_ptr);
extern bool object_is_nameless(object_type *o_ptr);
extern bool object_allow_two_hands_wielding(object_type *o_ptr);

/* wild.c */
extern void set_floor_and_wall(byte type);
extern void wilderness_gen(void);
extern void wilderness_gen_small(void);
extern errr init_wilderness(void);
extern void init_wilderness_terrains(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x);
extern bool change_wild_mode(void);

/* wizard1.c */
extern void spoil_random_artifact(cptr fname);

/* wizard2.c */
extern void strip_name(char *buf, int k_idx);

/* avatar.c */
extern bool compare_virtue(int type, int num, int tekitou);
extern int virtue_number(int type);
extern cptr virtue[MAX_VIRTUE];
//extern void get_virtues(void);
extern void chg_virtue(int virtue, int amount);
extern void set_virtue(int virtue, int amount);
extern void dump_virtues(FILE * OutFile);

#ifdef JP
/* japanese.c */
extern void sindarin_to_kana(char *kana, const char *sindarin);
extern void jverb1( const char *in , char *out);
extern void jverb2( const char *in , char *out);
extern void jverb3( const char *in , char *out);
extern void jverb( const char *in , char *out , int flag);
extern void sjis2euc(char *str);
extern void euc2sjis(char *str);
extern byte codeconv(char *str);
extern bool iskanji2(cptr s, int x);
#endif

#ifdef WORLD_SCORE
/* report.c */
extern errr report_score(void);
extern cptr make_screen_dump(void);
#endif

/* inet.c */
extern int soc_write(int sd, char *buf, size_t sz);
extern void set_proxy(char *default_url, int default_port);
extern int connect_server(int timeout, const char *host, int port);
extern int disconnect_server(int sd);
extern cptr soc_err(void);

#ifdef CHUUKEI
/* chuukei.c */
extern bool chuukei_server;
extern bool chuukei_client;

extern int connect_chuukei_server(char *server_string);
extern void browse_chuukei(void);
extern void flush_ringbuf(void);
extern void prepare_chuukei_hooks(void);
#endif

extern void prepare_movie_hooks(void);
extern void prepare_browse_movie_aux(cptr filename);
extern void prepare_browse_movie(cptr filename);
extern void browse_movie(void);
extern bool browsing_movie;

#ifdef TRAVEL
/* for travel */
extern travel_type travel;
#endif

/* variable.c (for snipers) */
extern int snipe_type;
extern bool reset_concent;   /* Concentration reset flag */
extern bool is_fired;

/* snipe.c */
extern void reset_concentration(bool msg);
extern void display_snipe_list(void);
extern int tot_dam_aux_snipe (int mult, monster_type *m_ptr);
extern void do_cmd_snipe(void);
extern void do_cmd_snipe_browse(void);
extern int boost_concentration_damage(int tdam);

/* hex.c */
extern bool stop_hex_spell_all(void);
extern bool stop_hex_spell(void);
extern void check_hex(void);
extern bool hex_spell_fully(void);
extern void revenge_spell();
extern void revenge_store(int dam);
extern bool teleport_barrier(int m_idx);
extern bool magic_barrier(int m_idx);
extern bool multiply_barrier(int m_idx);


///mod131226 skill 武器技能をTVALごとにまとめ、技能と武器技能の統合に
extern s16b ref_skill_exp(int skill_num);
extern void gain_skill_exp(int skill_num , monster_type* m_ptr);
///mod131229 武器に刃があるかどうか判別用
extern bool object_has_a_blade(object_type *o_ptr);
///mod131229 主にアイテム生成でいいのが出やすくなる判定
///mod140104 数値でなくboolを返すようにした
extern bool weird_luck(void);
///mod131229 ベースアイテムに一定確率で付加されるフラグの処理
extern void add_art_flags(object_type * o_ptr);

///mod140103 破邪攻撃ダメージへの耐性、弱点、免疫処理適用
extern int mod_holy_dam(int dam);
///mod140105 int1.c以外からも使うためexternにした
extern void drop_here(object_type *j_ptr, int y, int x);
///mod140103 破邪攻撃ダメージ計算処理の補助関数
extern int is_hurt_holy(void);

///mod140107 幻想郷の人型ユニーク判別用
extern bool is_gen_unique(int r_idx);

///mod140119 劣化弱点判定処理
extern int mod_disen_dam(int dam);
extern int is_hurt_disen(void);

///mod140122 閃光弱点判定処理
extern int mod_lite_dam(int dam);
extern int is_hurt_lite(void);


///mod140122 装備制限関数
extern bool wield_check( int item_old, int item_new);

///mod140126 狂気免疫判定
extern bool immune_insanity(void);

///mod140126 水弱点判定処理
extern int mod_water_dam(int dam);
extern int is_hurt_water(void);

///mod140202 メイド・執事用報酬
extern void gain_maid_reward(void);

///mod140202 新たな職業ごと特技をまとめてここで使う予定
extern void do_cmd_new_class_power(bool only_browse);

///mod140208 二領域の魔法(書)が同じ大分類に属する場合TRUE
extern bool same_category_realm(int realm1, int realm2);


///mod140208 magic_typeを種族や職業に合わせて計算する関数
extern void calc_spell_info(magic_type *s_ptr, int realm, int spell);

///新魔法領域の難度などの表　領域、魔法番号、適性の順
extern magic_type spell_base_info[MAX_MAGIC][32][6];

///mod160620 オカルト魔法用新テーブル
extern magic_type occult_spell_base_info[32][6];

///mod140208 水一時耐性
extern bool set_tim_res_water(int v, bool do_dec);


///mod140211 他のソースファイルから使うためにstaticを外した。
extern void touch_zap_player(monster_type *m_ptr);

///mod140211 アイテムや地形にも影響を与える視界内攻撃
extern bool project_hack2(int typ_base, int dice, int sides, int base);
extern bool project_m(int who, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg);
extern bool project_o(int who, int r, int y, int x, int dam, int typ);
extern bool project_f(int who, int r, int y, int x, int dam, int typ);
extern bool project_p(int who, cptr who_name, int r, int y, int x, int dam, int typ, int flg, int monspell);

extern byte spell_color(int type);

///mod140215 H&Aを独立,一時カオス耐性、一時超隠密
extern bool hit_and_away(void);

extern bool set_tim_res_chaos(int v, bool do_dec);
extern bool set_tim_superstealth(int v, bool do_dec, int stealth_type);
extern bool set_tim_unite_darkness(int v, bool do_dec);

///mod140216 格闘ルーチン変更関連
extern const martial_arts_new ma_blows_new[];

///mod140217 軌跡を表示しないボール
extern bool fire_ball_jump(int typ, int dir, int dam, int rad, cptr msg);

///mod140222 project()による固定ダメージを与える入身
//v1.1.92 半径指定追加
extern bool rush_attack2(int len, int type, int dam,int rad);
///mod140222 stat_ind[]を放り込んで使う数列
extern const int adj_general[STAT_HYPERMAX-2];
///mod140222 汎用時限処理
extern bool set_tim_general(int v, bool do_dec, int ind, int num);
///mod140222 視界内のランダムなモンスターになにか撃つ
extern bool fire_random_target(int typ, int dam, int method, int rad, int range);
///mod140222 テレポート・バック
extern bool teleport_back(int *idx, u32b mode);
///mod 素手攻撃手段選定
extern int find_martial_arts_method(int findmode);
///mod140228 敵による視界内攻撃
extern bool project_hack3(int who, int cy, int cx, int typ, int dice, int sides, int base);

///mod140302 元素弱点、耐性処理統合
extern int mod_fire_dam(int dam);
extern int is_hurt_fire(void);
extern int mod_cold_dam(int dam);
extern int is_hurt_cold(void);
extern int mod_elec_dam(int dam);
extern int is_hurt_elec(void);
extern int mod_acid_dam(int dam);
extern int is_hurt_acid(void);

///mod140302 小傘専用の傘パラメータ設定ルーチン
extern void apply_kogasa_magic(object_type *o_ptr, int lev, bool birth);

///mod140373 難易度
extern byte difficulty;

///mod140308 弓枠廃止処理関連
extern int shootable(int *tester);
extern bool object_is_shooting_weapon(object_type *o_ptr);

extern errr get_obj_num_prep(void);

extern void display_rumor_new(int town_num);
extern bool set_nennbaku(int v, bool do_dec);

extern u32b total_points_new(bool test);

extern void prt_score(void);

extern bool detect_monsters_living(int range);
///mod140325 一時的能力上昇
extern bool set_tim_addstat(int stat, int amount, int v, bool do_dec);

///mod140326 泥酔度実装
extern bool set_alcohol(int v);

///mod140328 大型レーザー
extern bool fire_spark(int typ, int dir, int dam, int rad);


///mod140329 生来型変異を得る
extern void gain_perma_mutation(void);
extern bool set_radar_sense(int v, bool do_dec);
extern void process_world_aux_alcohol(void);
extern int calc_alcohol_mod(int num, bool check);
extern bool object_is_not_worthless(object_type *o_ptr);
extern int count_mutations(void);

extern bool finish_the_game;

extern bool ang_sort_comp_pet(vptr u, vptr v, int a, int b);

extern bool ignore_summon_align;
extern bool you_are_human_align(void);

extern const  souvenir_type souvenir_table[];

extern void masterspark(int y, int x, int m_idx, int typ, int dam_hp, int monspell, bool learnable, int rad);

extern void fix_floor_item_list(int y,int x);

extern bool set_tim_lucky(int v, bool do_dec);
extern bool set_foresight(int v, bool do_dec);
extern bool set_deportation(int v, bool do_dec);

extern bool spell_is_summon(u16b spell);

extern bool check_invalidate_inventory(int slot);

extern bool set_clawextend(int v, bool do_dec);

///mod140510 一時早駆け
extern bool set_tim_speedster(int v, bool do_dec);

extern bool set_tim_res_insanity(int v, bool do_dec);

extern bool set_kamioroshi(u32b kamioroshi_type, int v);
extern bool dispel_kwai(int dam);
extern bool diehardmind(void);
///mod140512 流星群ルーチンを属性指定可能にした
extern void cast_meteor(int dam, int rad, int typ);

extern bool check_quest_unique_text(void);

///mod140608 インベントリ追加関連
extern object_type *inven_add;
///mod160206 インベントリ追加2
extern object_type *inven_special;

extern void display_list_inven2(void);
extern bool put_item_into_inven2(void);
extern bool takeout_inven2(void);
extern int calc_inven2_num(void);

///mod140608 クエスト無視処理関係
extern bool check_ignoring_quest(int questnum);

extern bool vanish_dungeon(void);

///mod140625 特殊ユニーク関連
extern bool apply_mon_race_reimu(void);
extern bool apply_mon_race_marisa(void);
extern void cast_musou_hu_in(int reimu_idx);

extern byte rakuba_type;

extern bool object_is_melee_weapon_except_strange_kind(object_type *o_ptr);


///mod140714 付喪神使い専用
extern bool apply_mon_race_tsukumoweapon(object_type *o_ptr, int r_idx);
extern void tsukumo_recall(void);
extern void make_evilweapon(object_type *o_ptr, monster_race *r_ptr);


///mod140719 ランダムユニーク実装
extern bool apply_random_unique(int r_idx);
extern char random_unique_name[3][80];
extern s16b random_unique_akills;
extern bool flag_generate_doppelganger;

extern int flag_generate_lilywhite;


extern int special_idol_generate_type;

extern bool check_rest_f50(int r_idx);

extern void search_specific_object(int mode);

extern void mutation_stop_mouth(void); //外から使えるようにする

extern bool eat_rock(void);
extern bool item_tester_hook_make_tsukumo(object_type *o_ptr);

///mod140813 喘息
extern bool set_asthma(int v);

extern bool quick_cast;

///mod140816 赤蛮奇
extern bool apply_mon_race_banki_head(int r_idx);
extern void update_minion_sight(void);

///mod140817 ミスティア
extern bool check_mon_blind(int m_idx);

///mod140819 エンジニア
extern int engineer_guild_supply_table[MAX_MACHINE_SUPPLY][2];
extern machine_type machine_table[MAX_MACHINE];
extern bool rush_attack3(int length ,cptr msg, int dam);
extern bool use_machine(int mode);
extern bool scouter_use;
extern void engineer_malfunction(int typ, int dam);
extern bool engineer_triggerhappy(void);
extern void engineer_guild_price_change(void);
extern int new_class_power_change_energy_need;


extern bool cast_wrath_of_the_god(int dam, int rad, bool aim);

///mod140903 剣術家再実装
extern bool choose_samurai_kata(void);
extern int calc_weapon_dam(int hand);

extern void dragon_resist(object_type *o_ptr);

extern bool monster_is_you(s16b r_idx);

extern const monspells2 monspell_list2[MAX_MONSPELLS2+1];
extern void make_magic_list_satori(bool del);

extern const cptr stat_desc[6];
extern bool cast_monspell_new(void);
extern cptr cast_monspell_new_aux(int num, bool only_info, bool fail, int xtra);

extern bool satori_reading;

extern void kyouko_echo(bool del, int thrown_spell);

///mod141116 キスメ
extern int calc_kisume_bonus(void);
extern bool do_cmd_concentrate(int mode);

extern bool hatate_psychography(void);

extern void make_mask(monster_type *m_ptr);

///mod150103 野良神様
extern const deity_type	deity_table[];
extern void change_deity_type(int rank);

///mod151016 村紗がmap_area()で水地形のみを感知する専用フラグ
extern bool murasa_detect_water;

extern int muta_erasable_count(void);

///mod150129 霊夢
extern u32b osaisen;
extern int osaisen_rank(void);
extern void gain_osaisen(int amount);
extern void throw_osaisen(void);
extern void check_osaisen(void);
//extern int oonusa_dam;

///mod150208 野良神様
extern bool deity_racial_power_aux(s32b command);
extern void deity_racial_power(power_desc_type *power_desc, int *num_return);
extern void apply_deity_ability_flags(int deity_type, u32b flgs[TR_FLAG_SIZE]);
extern void apply_deity_ability(int deity_type, int *new_speed);
extern void set_deity_bias(int type,int num);

extern void summon_material(int m_idx);

extern u32b marisa_magic_power[8];
extern void marisa_gain_power(object_type *o_ptr, int mult);

///mod150315 妖精特技追加
extern bool fairy_racial_power_aux(s32b command);
extern void fairy_racial_power(power_desc_type *power_desc, int *num_return);
extern void get_fairy_type(bool birth);
extern const fairy_type fairy_table[FAIRY_TYPE_MAX+2];

extern bool ident_spell_2(int mode);
extern bool go_west(void);

extern void check_nue_undefined(void);
extern void check_nue_revealed(int m_idx, int chance_mult);

extern bool flag_seija_quest_comp;
extern void restore_seija_item(void);
extern int select_magic_eater(bool only_browse, bool recharge);

///mod150423 魔道具術師改変
extern bool item_tester_hook_wear(object_type *o_ptr);
extern bool do_cmd_recharge_equipment(int power);
extern bool recharge_magic_eater(int power);


///mod150516 天子用
extern bool kanameishi_check(int add);
extern void kanameishi_break(void);

///mod150528 パルスィ
extern bool apply_mon_race_green_eyed(void);


///mod150609 新闘技場
extern void	battle_mon_gambling(void);
///mod150614 新闘技場
extern battle_mon_type battle_mon_list[BATTLE_MON_LIST_MAX];
extern s16b battle_mon_team_type[4];
extern int	battle_mon_wager;
extern int  battle_mon_odds;
extern u32b battle_mon_team_bet;
extern int  battle_mon_base_level;
extern void clear_bldg(int min_row, int max_row);


//投擲アイテム破壊率計算　cmd2.c以外でも使うようになったので移動
extern int breakage_chance(object_type *o_ptr);
//新投擲ルーチン
extern bool do_cmd_throw_aux2(int y, int x, int ty, int tx, int mult, object_type *o_ptr, int mode);

extern bool flag_sumireko_wakeup;

extern const cptr battle_mon_random_team_name_list[BATTLE_MON_RANDOM_TEAM_NAME_NUM+1];
extern const battle_mon_special_team_type	battle_mon_special_team_list[BATTLE_MON_SPECIAL_TEAM_NUM+1];

extern bool unzan_guard(void);

extern int calc_weapon_dam(int hand);

///mod150711 妹紅
extern bool flag_mokou_resurrection;
extern bool flag_mokou_possess;
extern void mokou_resurrection(void);

///mod150712 神奈子
extern void kanako_get_point(monster_type *m_ptr);
extern int kanako_rank(void);
extern cptr kanako_comment(void);

///mod150719 布都
extern s16b feat_plate; //皿
extern bool is_plate_grid(cave_type *c_ptr);
extern bool futo_break_plate(int y, int x);
extern bool futo_determine_target(int cy, int cx, int *ty, int *tx);
extern bool futo_can_place_plate(int y, int x);

extern s16b feat_bomb; //爆弾
extern void bomb_count(int y, int x, int num);

/*:::アリス関係*/
extern cptr alice_doll_name[INVEN_DOLL_NUM_MAX];
extern int calc_doll_blow_num(int doll_num);


///mod150811 魔理沙
extern struct marisa_magic_type marisa_magic_table[MARISA_MAX_MAGIC_KIND];
extern void marisa_make_magic_recipe(void);
extern bool marisa_will_buy(object_type *o_ptr);
extern cptr marisa_essence_name[8];
extern bool marisa_extract_material(bool in_home);
extern int choose_marisa_magic(int mode);
extern bool check_marisa_recipe(void);
extern bool make_marisa_magic(void);
extern bool carry_marisa_magic(void);
extern cptr use_marisa_magic(int num, bool only_info);
extern void marisa_check_essence(void);
extern void activate_moon_vault_trap(void);

///mod150822 プリズムリバー
extern void  prism_change_class(bool urgency);

extern u32b hecatia_hp[3];

extern char extra_mon_name[80];

extern void metamorphose_to_monster(int r_idx, int time);

extern bool player_gain_mana(int mana);
extern bool monplayer_attack_monst(int t_idx);

extern void hina_gain_yaku(int yaku);

extern bool item_recall(int mode);

extern void sakuya_time_stop(bool open);

extern void stop_tsukumo_music(void);
extern void stop_raiko_music(void);

extern bool process_warning2(void);

extern void hina_yakuotoshi(int gain_exp);
extern void gain_random_slay_2(object_type *o_ptr);
extern void building_prt_gold(void);
extern void hina_at_work(void);

extern byte value_check_aux1(object_type *o_ptr);

extern int kaguya_quest_art_list[5];
extern int kaguya_quest_level;

extern void do_cmd_wield_3_fairies(void);
extern s16b wield_slot_3_fairies(object_type *o_ptr);

extern bool check_equip_specific_fixed_art(int a_idx, bool only_equip);
extern int numbering_newspaper(int r_idx);

extern bool rankup_monster(int m_idx, int rank);

extern const drug_compound_type drug_compound_table[];
extern const drug_material_type drug_material_table[];
extern const drug_recipe_type drug_recipe_table[32];
extern void compound_drug(void);
extern void check_drug_recipe_aux(object_type *o_ptr);
extern void check_drug_recipe(void);

extern bool kogasa_smith_aux(object_type *o_ptr);
extern bool object_is_metal(object_type *o_ptr);
extern bool kogasa_smith(void);

extern bool final_strike(int item , bool remain_broken_weapon);

extern void yukari_dec_home_item(int item);
extern bool yukari_send_item_to_home(object_type *o_ptr);

extern bool support_item_test(void);

extern void apply_magic_itemcard(object_type *o_ptr, int lev, int r_idx);
extern const support_item_type support_item_list[];

extern bool set_itemcard(void);
extern bool remove_itemcard(void);
extern bool activate_itemcard(void);
extern void read_itemcard(void);

extern bool flag_spell_consume_book;
extern bool flag_spell_forget;
extern void get_bad_mental_mutation(void);

extern void init_extra_dungeon_buildings();
extern byte building_ex_idx[BLDG_EX_NUM];

extern ex_quest_rew_type ex_quest_reward_table[];

extern byte	ex_buildings_history[EX_BUILDINGS_HISTORY_MAX];
extern byte	ex_buildings_param[EX_BUILDINGS_PARAM_MAX];
extern void exbldg_search_around(void);

extern bool hack_flag_access_home;
extern bool hack_flag_access_home_orin;
extern bool orin_deliver_item(void);
//v1.1.62
extern bool hack_flag_access_home_only_arrow;


extern int calc_itemcard_slot_size(void);

extern int hack_ex_bldg_summon_idx;
extern int hack_ex_bldg_summon_type;
extern u32b hack_ex_bldg_summon_mode;

extern s16b monspell_yamabiko;

extern void show_building(building_type* bldg);

extern void testmsg(cptr msg); //デバッグ用

extern void extra_mode_score_mult(int *mult, int class_idx);

extern u32b parsee_damage_count;

extern u32b score_bonus_kill_unique2;

extern bool hack_flag_calc_museum_score;

extern void berserk_spell_fail_effect(int spell);

extern const gun_base_param_type gun_base_param_table[];
extern const cptr gf_desc_name[];

extern int	calc_gun_fire_chance(int slot);
extern int	calc_gun_charge_mult(int slot);
extern int	calc_gun_timeout(object_type *o_ptr);
extern bool gun_free_world; //銃が出現しない
extern int select_gun_melee_mode(void);
extern bool bm_sells_antique; //店に骨董品が並ぶ(蒐集用アイテム)

extern bool always_check_arms;

extern int calc_gun_load_num(object_type *o_ptr);

extern bool mass_genocide_2(int power, int rad, int mode);

extern int	special_shooting_mode;

extern void do_cmd_fire_gun_aux(int slot, int dir);

//輝夜(永琳プレイ時の特殊モンスター)
extern bool apply_mon_race_master_kaguya(void);
extern void	place_master_kaguya(void);
extern byte kaguya_fight;

extern void raising_game(void);

//秘術
extern s16b hassyakusama_target_idx;
extern occult_contact_type occult_contact_table[];
extern void break_eibon_wheel(void);
extern bool break_eibon_flag;
extern bool is_elder_sign(cave_type *c_ptr);

extern int	check_have_specific_item(int tval, int sval);

extern void travel_flow(int ty, int tx);

extern void hit_trap(bool break_trap);

extern void call_amaterasu(void);

//ヘカーティア
extern bool  hecatia_change_body(void);
extern void get_hecatia_seikaku(void);

extern void print_all_item_param(int idx);

//純狐
extern void	add_junko_flag_val(int flag_num, int add_val);
extern bool purify_equipment(void);
extern bool	junko_make_nameless_arts(void);
extern bool	activate_nameless_art(bool only_info);
extern bool check_activated_nameless_arts(int art_idx);

extern byte dimension_door_distance;
extern bool item_tester_hook_jeweler(object_type *o_ptr);
extern essence_type essence_info[];
extern void junko_msg(void);
extern bool convert_item_to_mana(void);
extern void junko_gain_equipment_flags(object_type *o_ptr, int amt, u32b flgs[TR_FLAG_SIZE]);
//v1.1.56
extern cptr msg_tim_nameless_arts(int art_idx, int counter_idx, bool activate);

//兵士
extern soldier_skill_type soldier_skill_table[SOLDIER_SKILL_ARRAY_MAX][SOLDIER_SKILL_NUM_MAX+1];
extern bool gain_soldier_skill(void);
extern void get_soldier_skillpoint(void);
extern void check_soldier_skill(void);
extern u32b hack_gun_fire_critical_flag;
extern void pass_through_portal(void);
extern int	 hack_gun_fire_critical_hit_bonus;
extern bool get_random_target(int mode, int range);
extern bool hack_flag_gain_gun_skill_exp;
extern soldier_bullet_type soldier_bullet_table[];
extern soldier_bullet_making_type soldier_bullet_making_table[];
extern int soldier_change_bullet(void);
extern int hack_der_freischutz_basenum;

extern	int	hack_project_start_x;
extern	int	hack_project_start_y;
extern	int	hack_project_end_x;
extern	int	hack_project_end_y;

//v1.1.24 急流下りクエスト
extern	u32b	qrkdr_rec_turn;
extern torrent_quest_score_type torrent_quest_score_list[];



extern u32b testcount;
extern u32b testcount2;

//v1.1.25 色々追加
extern byte start_ver[4];
extern s32b gambling_gain;
extern u32b score_count_pinch;
extern u32b score_count_pinch2;
//v1.1.25 スコアサーバ関連
extern char score_server_name[SCORE_SERVER_STR_LEN];
extern char score_server_key[SCORE_SERVER_STR_LEN];
extern void get_score_server_name(void);
//v1.1.27 銃投擲仕様変更
extern void gun_throw_effect(int y,int x,object_type *q_ptr);
//v1.1.27 結界ガード
extern int count_damage_guard_break;
extern bool set_mana_shield(bool set, bool guard_break);
extern void check_mana_shield(int *dam, int damage_type);

extern bool flag_friendly_attack;

extern int teleport_town_mode;

//v1.1.32 特殊性格実装関連
extern const player_seikaku * get_ap_ptr(int race_idx, int class_idx, int seikaku_idx);
extern bool is_special_seikaku(int idx);
extern const player_seikaku seikaku_info_special[MAX_SEIKAKU_SPECIAL];

extern cptr skill_exp_desc[SKILL_EXP_MAX];

extern int mass_genocide_3(int rad, bool geno_friend, bool geno_unique);

extern bool straygod_job_adviser(void);

extern void whirlwind_attack(int mode);

//v1.1.35 ネムノ縄張り関係
extern void make_nemuno_sanctuary(void);
extern void erase_nemuno_sanctuary(bool force, bool redraw);

extern void floor_attack(int typ, int dice, int sides, int base, int mode);

//v1.1.36 リピートフラグ
extern bool command_repeat_flag;

extern void absorb_tsukumo(int m_idx);

extern bool hack_flag_powerup_mutation;

//v1.1.43
extern bool lunatic_torch(int m_idx, int power);

//v1.1.43
extern bool flag_sunny_refraction;
extern void update_flying_dist(void);

//v1.1.44
extern bool	execute_restartable_project(int mode, int dir, int restart_chance, int dice, int sides, int base, int typ, int rad);
extern bool flag_futo_broken_plate;
extern bool	rapid_fire(int shoot_num, int mode);
extern bool evil_undulation(bool activate, bool check);

//v1.1.45
extern void make_magic_list_satono(void);

//v1.1.46
extern cptr jyoon_inven2_finger_name[JYOON_INVEN2_NUM];

//v1.1.46 女苑関係
//extern bool object_is_cheap_to_jyoon(object_type *o_ptr);
extern bool item_tester_hook_activate(object_type *o_ptr);
extern void do_cmd_activate_aux(int item);
extern bool	activate_inven2(void);

//v1.1.46 女苑が浪費した金額をp_ptr->magic_num1[0][1]に記録する。$999,999,999,999が上限
extern void	jyoon_record_money_waste(int sum);

//v1.1.46 アイテムを持っているモンスターを感知する
extern bool detect_monsters_rich(int range);

//v1.1.48 紫苑関係
extern void shion_seizure(int slot);
extern bool shion_begging(void);

//v1.1.51 新アリーナ(夢の世界に行く)関係
extern s16b	nightmare_diary_count;
extern s16b	nightmare_diary_win;
extern u32b	nightmare_total_point;
extern s16b	nightmare_mon_r_idx[NIGHTMARE_DIARY_MONSTER_MAX];
extern u32b	tmp_nightmare_point;
extern s16b	nightmare_mon_base_level;
extern const nightmare_gen_map_type	nightmare_gen_map_table[NIGHTMARE_GEN_MAP_TABLE_SIZE];
//extern bool monster_hook_nightmare_diary(int r_idx);

extern const nightmare_stage_type nightmare_stage_table[];
extern const nightmare_spellname_type	nightmare_spellname_table[];
extern const nightmare_treasure_type nightmare_treasure_table[];
extern bool	nightmare_diary(void);
extern void	make_nightmare_diary_photo(byte x, byte y);

//v1.1.52 菫子新性格
extern bool	okina_make_nameless_arts(int num);
extern const cptr	nameless_skill_type_desc[];


//v1.1.53
extern void clownpiece_likes_fire(int typ);

//v1.1.55
extern s16b	nightmare_record_hp;

//v1.1.56
extern void	bact_buy_strange_object(void);
extern void	bact_marisa_make_spellcard(void);
extern void look_spellcard(object_type *o_ptr);
extern void use_spellcard(object_type *o_ptr);
extern bool set_tim_spellcard_effect(int v, bool do_dec, int art_idx);

//v1.1.57
extern bool generate_seven_star_sword(void);
extern pattern_attack_type pattern_attack_table[];
extern bool pattern_attack(void);

//v1.1.58 本家からBGM機能をコピー
extern const cptr angband_music_basic_name[MUSIC_BASIC_MAX];
extern void init_music_hack(void);
extern int find_mon_music_priority(int r_idx);
extern int music_r_idx;
extern void	check_music_mon_r_idx(int m_idx);

//v1.1.58
extern bool	flag_update_floor_music;
extern bool	flag_unique_class_use_monster_bgm;
extern bool	flag_display_bgm_title;
extern cptr bgm_title_text_ptr;

extern bool teleport_to_specific_dir(int dist, int retry_chance, int mode);

//v1.1.59
extern void update_dungeon_feeling(bool flag_feel_now);
extern void sunny_charge_sunlight(int amt);

//v1.1.64 武器修復テーブルを外部からも使えるよう設定
extern bool item_tester_hook_repair_material(object_type *o_ptr);
extern const struct guild_arts_type repair_weapon_power_table[];

//v1.1.65
extern int calc_martialarts_dam_x100(int hand_idx);

//v1.1.66
extern bool set_broken(int v);
extern bool	self_repair(void);

//v1.1.70
extern const cptr animal_ghost_strife_names[];
extern void	get_animal_ghost_family(bool birth);

//v1.1.71
extern bool throw_up(void);

//v1.1.73
extern	bool	flag_bribe_summon_monsters;

//v1.1.74
extern bool can_do_cmd_cast(void);

//v1.1.76
extern void yuyuko_nehan(void);

//v1.1.77
extern bool orin_escape(int m_idx);

//v1.1.78
extern void	check_suiken(void);

//v1.1.84
extern bool energy_drain(void);
extern void process_over_exert(int sleep_turn_base);

//v1.1.85
extern int apply_new_lake_type(bool flag_vault);

//v1.1.86
extern ability_card_type ability_card_list[ABILITY_CARD_LIST_LEN];
extern int apply_magic_abilitycard(object_type *o_ptr, int card_idx, int card_rank_min, int card_rank_max);
extern cptr use_ability_card_aux(object_type *o_ptr, bool only_info);
extern bool use_ability_card(object_type *o_ptr);
extern int	count_ability_card(int card_idx);
extern int calc_ability_card_mod_param(int card_idx, int card_num);
extern int calc_ability_card_prob(int card_idx, int card_num);
extern bool wall_through_telepo(int mode);
extern s16b	ability_card_trade_count;

//v1.1.87
extern s16b	buy_gacha_box_count;
extern u32b	total_pay_cardshop;
extern void set_abilitycard_price_rate();
extern void make_ability_card_store_list();
extern bool hack_flag_store_examine;
extern int calc_ability_card_price(int card_idx);
extern bool	buy_abilitycard_from_mon(void);

//v1.1.90
extern bool	mon_saving_throw(monster_type *m_ptr, int power);

//v1.1.91
extern int oil_field_damage_mod(int src_dam, int y, int x);
extern int yuma_vacuum_oil(int y, int x, bool check, int rad);

//v1.1.92
extern bool item_tester_hook_cursed(object_type *o_ptr);

//v1.1.93
extern bool set_tim_aggravation(int v, bool do_dec);

//テスト用
extern int	gv_test1;
extern int	gv_test2;
extern int	gv_test3;
extern int	gv_test4;

//v1.1.95
extern int check_player_is_seen(void);

//v1.1.97
extern const int chest_new_traps[CHEST_TRAP_LIST_LENGTH];
extern void activate_chest_trap(int y, int x, s16b o_idx, bool trap_player);
extern void teleporter_trap(void);

extern bool monster_delay(int m_idx, int add_energy);

extern void gain_physical_mutation(void);

extern bool flag_life_explode;

extern bool teleport_walk(int dist);

extern bool rokuro_head_search_item(int v, bool flag_stop);

extern bool set_hirarinuno_card(int v, bool do_dec);

extern void break_market(void);

extern void walk_energy_modify(void);

extern bool eat_jewel(void);
extern bool make_magatama(void);
extern void magatama_effect_desc(char *desc, object_type *o_ptr);
extern bool	activate_magatama(bool only_info);

extern bool	flag_nue_check_undefined;

//v2.0.5
extern bool hatate_search_unique_monster(void);
extern void alloc_hatate_search_mon();

//v2.0.6
extern bool yuma_swallow(int rad);
extern void	yuma_lose_extra_power(int dec_val);
extern const cptr equipment_tr_flag_names[TR_FLAG_MAX + 1];
extern bool do_cmd_eat_food_aux(int item);
extern int yuma_eat_other_things(object_type *o_ptr);
extern int calc_korin_sell_price(object_type *o_ptr);
extern bool check_yuma_ultimate_mode(void);

//v2.0.7
extern int 	chimata_calc_card_rank(void);
extern void chimata_comment_card_rank(void);
extern bool	chimata_activate_ability_card_power(bool only_info);
extern int	chimata_can_copy(int r_idx);
extern void	search_specific_monster(int mode);
