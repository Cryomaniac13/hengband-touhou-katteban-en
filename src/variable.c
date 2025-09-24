/* File: variable.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband variables */

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
const cptr copyright[5] =
{
	"",
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


int max_macrotrigger = 0;
cptr macro_template = NULL;
cptr macro_modifier_chr;
cptr macro_modifier_name[MAX_MACRO_MOD];
cptr macro_trigger_name[MAX_MACRO_TRIG];
cptr macro_trigger_keycode[2][MAX_MACRO_TRIG];

/* レベルアップの時に上昇量を表示するのに使う */
int level_up = 0;

/*
 *  List for auto-picker/destroyer entries
 */
int max_autopick = 0;
int max_max_autopick = 0;
autopick_type *autopick_list = NULL;

/*
 * Savefile version
 */
byte h_ver_major;       /* Savefile version for Hengband 1.1.1 and later */
byte h_ver_minor;
byte h_ver_patch;
byte h_ver_extra;

byte sf_extra;		/* Savefile's encoding key */

byte z_major;           /* Savefile version for Hengband */
byte z_minor;
byte z_patch;

//v1.1.25 ゲーム開始時のバージョンを記録することにした
byte start_ver[4];

/*
 * Savefile information
 */
u32b sf_system;			/* Operating system info */
u32b sf_when;			/* Time when savefile created */
u16b sf_lives;			/* Number of past "lives" with this file */
u16b sf_saves;			/* Number of "saves" during this life */

/*
 * Run-time arguments
 */
bool arg_fiddle;			/* Command arg -- Request fiddle mode */
bool arg_wizard;			/* Command arg -- Request wizard mode */
bool arg_sound;				/* Command arg -- Request special sounds */
bool arg_music;				/* Command arg -- Request special musics */ //v1.1.58
byte arg_graphics;			/* Command arg -- Request graphics mode */
bool arg_monochrome;		/* Command arg -- Request monochrome mode */
bool arg_force_original;	/* Command arg -- Request original keyset */
bool arg_force_roguelike;	/* Command arg -- Request roguelike keyset */
bool arg_bigtile = FALSE;	/* Command arg -- Request big tile mode */

/*
 * Various things
 */
bool character_generated;	/* The character exists */
bool character_dungeon;		/* The character has a dungeon */
bool character_loaded;		/* The character was loaded from a savefile */
bool character_saved;		/* The character was just saved to a savefile */

/*:::店に入ったときTRUEになって出るときFALSEになっている。*/
//ただ、screen_save/load()や建物出入りでは++や--で演算されている。boolは内部的にbyteのはず？だからこれで階層保持されているのか。
bool character_icky;		/* The game is in an icky full screen mode */
/*:::初期化中フラグ？*/
bool character_xtra;		/* The game is in an icky startup mode */

bool creating_savefile;		/* New savefile is currently created */

u32b seed_flavor;		/* Hack -- consistent object colors */
u32b seed_town;			/* Hack -- consistent town layout */

s16b command_cmd;		/* Current "Angband Command" */

s16b command_arg;		/* Gives argument of current command */
s16b command_rep;		/* Gives repetition of current command *//*:::コマンド繰り返し中の回数*/
s16b command_dir;		/* Gives direction of current command */

s16b command_see;		/* See "object1.c" */
s16b command_wrk;		/* See "object1.c" */

s16b command_gap = 999;         /* See "object1.c" */

s16b command_new;		/* Command chaining from inven/equip view */

s16b energy_use;		/* Energy use this turn */

//v1.1.36 コマンドリピートフラグ　'n'実行中にTRUE
bool command_repeat_flag = FALSE;


bool msg_flag;			/* Used in msg_print() for "buffering" */

s16b running;			/* Current counter for running, if any */
/*:::休憩ターン数　&や*なら-2,-1が入る*/
s16b resting;			/* Current counter for resting, if any */

s16b cur_hgt;			/* Current dungeon height */
s16b cur_wid;			/* Current dungeon width */
/*:::今いる階*/
s16b dun_level;			/* Current dungeon level */
s16b num_repro;			/* Current reproducer count */
s16b object_level;		/* Current object creation level */
s16b monster_level;		/* Current monster creation level */
s16b base_level;        /* Base dungeon level */

s32b turn;				/* Current game turn */
s32b turn_limit;		/* Limit of game turn */
s32b dungeon_turn;			/* Game turn in dungeon */
s32b dungeon_turn_limit;	/* Limit of game turn in dungeon */
s32b old_turn;			/* Turn when level began */
s32b old_battle;

bool use_sound;			/* The "sound" mode is enabled */
bool use_music;			/* The "music" mode is enabled */ //v1.1.58
bool use_graphics;		/* The "graphics" mode is enabled */
bool use_bigtile = FALSE;

s16b signal_count;		/* Hack -- Count interupts */

bool inkey_base;		/* See the "inkey()" function */
bool inkey_xtra;		/* See the "inkey()" function */
bool inkey_scan;		/* See the "inkey()" function */
bool inkey_flag;		/* See the "inkey()" function */
bool get_com_no_macros = FALSE;	/* Expand macros in "get_com" or not */

s16b coin_type;			/* Hack -- force coin type */

/*:::箱を開けるときTRUE 箱を開けた時のアイテム生成でまた箱が生成されないようにする*/
bool opening_chest;		/* Hack -- prevent chest generation */

/*:::詳細不明。多色・シェイプチェンジの敵が生成されたときTRUEになっている*/
bool shimmer_monsters;	/* Hack -- optimize multi-hued monsters */
bool shimmer_objects;	/* Hack -- optimize multi-hued objects */

/*:::「＠行動後にモンスター情報を更新する」フラグ？*/
bool repair_monsters;	/* Hack -- optimize detect monsters */
bool repair_objects;	/* Hack -- optimize detect objects */

s16b inven_nxt;			/* Hack -- unused */
bool hack_mind;
///mod140821 もと獣人開始時変異用のhack_mutationだったが、全＠のクイックスタートに反映されないゲームスタート時限定処理フラグにした
bool hack_startup;

s16b inven_cnt;			/* Number of items in inventory */
s16b equip_cnt;			/* Number of items in equipment */

s16b o_max = 1;			/* Number of allocated objects */
s16b o_cnt = 0;			/* Number of live objects */

/*:::m_list[]の最大添え字+1?*/
s16b m_max = 1;			/* Number of allocated monsters */
s16b m_cnt = 0;			/* Number of live monsters */

s16b hack_m_idx = 0;	/* Hack -- see "process_monsters()" */
s16b hack_m_idx_ii = 0;
/*:::カオスパトロンからすでに報酬をもらったカウント　アンドロイドなどの急なレベルアップでなんども報酬が発生するのを防ぐ*/
bool multi_rew = FALSE;
char summon_kin_type;   /* Hack, by Julian Lighton: summon 'relatives' */

int total_friends = 0;
s32b friend_align = 0;

int leaving_quest = 0;
bool reinit_wilderness = FALSE;


/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/*** Input Options ***/

bool rogue_like_commands;	/* Rogue-like commands */
bool always_pickup;	/* Pick things up by default */
bool carry_query_flag;	/* Prompt before picking things up */
bool quick_messages;	/* Activate quick messages */
bool auto_more;	/* Automatically clear '-more-' prompts */
bool command_menu;	/* Enable command selection menu */
bool other_query_flag;	/* Prompt for floor item selection */
bool use_old_target;	/* Use old target by default */
bool always_repeat;	/* Repeat obvious commands */
bool confirm_destroy;	/* Prompt for destruction of known worthless items */
/*:::呪われたアイテムを装備するとき警告するオプション*/
bool confirm_wear;	/* Confirm to wear/wield known cursed items */
bool confirm_quest;	/* Prompt before exiting a quest level */
bool target_pet;	/* Allow targetting pets */

#ifdef ALLOW_EASY_OPEN
bool easy_open;	/* Automatically open doors */
#endif

#ifdef ALLOW_EASY_DISARM
bool easy_disarm;	/* Automatically disarm traps */
#endif

#ifdef ALLOW_EASY_FLOOR
bool easy_floor;	/* Display floor stacks in a list */
#endif

bool use_command;	/* Allow unified use command */
bool over_exert;	/* Allow casting spells when short of mana */
bool numpad_as_cursorkey;	/* Use numpad keys as cursor key in editor mode */


/*** Map Screen Options ***/

bool center_player;	/* Center map while walking (*slow*) */
bool center_running;	/* Centering even while running */
bool view_yellow_lite;	/* Use special colors for torch-lit grids */
bool view_bright_lite;	/* Use special colors for 'viewable' grids */
bool view_granite_lite;	/* Use special colors for wall grids (slow) */
bool view_special_lite;	/* Use special colors for floor grids (slow) */
bool view_perma_grids;	/* Map remembers all perma-lit grids */
bool view_torch_grids;	/* Map remembers all torch-lit grids */
bool view_unsafe_grids;	/* Map marked by detect traps */
bool view_reduce_view;	/* Reduce view-radius in town */
bool fresh_before;	/* Flush output while continuous command */
bool fresh_after;	/* Flush output after monster's move */
bool fresh_message;	/* Flush output after every message */
bool hilite_player;	/* Hilite the player with the cursor */
bool display_path;	/* Display actual path before shooting */


/*** Text Display Options ***/

bool plain_descriptions;	/* Plain object descriptions */
bool plain_pickup;	/* Plain pickup messages(japanese only) */
bool always_show_list;	/* Always show list when choosing items */
bool depth_in_feet;	/* Show dungeon level in feet */
bool show_labels;	/* Show labels in object listings */
bool show_weights;	/* Show weights in object listings */
bool show_item_graph;	/* Show items graphics */
bool equippy_chars;	/* Display 'equippy' chars */
bool display_mutations;	/* Display mutations in 'C'haracter Display */
bool compress_savefile;	/* Compress messages in savefiles */
bool abbrev_extra;	/* Describe obj's extra resistances by abbreviation */
bool abbrev_all;	/* Describe obj's all resistances by abbreviation */
bool exp_need;	/* Show the experience needed for next level */
bool ignore_unview;	/* Ignore whenever any monster does */
bool full_mon_list;
bool show_throwing_dam;
bool show_special_info; //ほか特殊な情報を表記　既読の新聞タイトル

bool show_actual_value; //v1.1.81 技能値に実値を表示


/*** Game-Play Options ***/

bool stack_force_notes;	/* Merge inscriptions when stacking */
bool stack_force_costs;	/* Merge discounts when stacking */
bool expand_list;	/* Expand the power of the list commands */
bool small_levels;	/* Allow unusually small dungeon levels */
bool always_small_levels;	/* Always create unusually small dungeon levels */
bool empty_levels;	/* Allow empty 'arena' levels */
bool bound_walls_perm;	/* Boundary walls become 'permanent wall' */
bool last_words;	/* Leave last words when your character dies */
bool ex_attack_msg; //「攻撃した」を武器に応じてメッセージ変えてみる
bool auto_dump;		//ゲーム終了時に必ずダンプファイルを出力
bool gun_free_world; //銃が出現しない
bool always_check_arms; //「右手と左手に別々の物を持つとき常に確認する」
bool bm_sells_antique; //店に骨董品が並ぶ(蒐集用アイテム)

#ifdef WORLD_SCORE
bool send_score;	/* Send score dump to the world score server */
#endif

bool allow_debug_opts;	/* Allow use of debug/cheat options */


/*** Disturbance Options ***/

bool find_ignore_stairs;	/* Run past stairs */
bool find_ignore_doors;	/* Run through open doors */
bool find_cut;	/* Run past known corners */
bool check_abort;	/* Check for user abort while continuous command */
bool flush_failure;	/* Flush input on various failures */
bool flush_disturb;	/* Flush input whenever disturbed */
bool disturb_move;	/* Disturb whenever any monster moves */
bool disturb_high;	/* Disturb whenever high-level monster moves */
bool disturb_near;	/* Disturb whenever viewable monster moves */
bool disturb_pets;	/* Disturb when visible pets move */
bool disturb_panel;	/* Disturb whenever map panel changes */
bool disturb_state;	/* Disturb whenever player state changes */
bool disturb_minor;	/* Disturb whenever boring things happen */
bool ring_bell;	/* Audible bell (on errors, etc) */
bool disturb_trap_detect;	/* Disturb when leaving trap detected area */
bool alert_trap_detect;	/* Alert when leaving trap detected area */


/*** Birth Options ***/

bool manual_haggle;	/* Manually haggle in stores *//*:::値切り交渉をするオプション*/
bool easy_band;	/* Easy Mode (*) */
bool smart_learn;	/* Monsters learn from their mistakes (*) */
bool smart_cheat;	/* Monsters exploit players weaknesses (*) */
bool vanilla_town;	/* Use 'vanilla' town without quests and wilderness */
bool lite_town;	/* Use 'lite' town without a wilderness */
bool ironman_shops;	/* Stores are permanently closed (*) */
bool ironman_small_levels;	/* Always create unusually small dungeon levels (*) */
bool ironman_downward;	/* Disable recall and use of up stairs (*) */
bool ironman_empty_levels;	/* Always create empty 'arena' levels (*) */
bool ironman_rooms;	/* Always generate very unusual rooms (*) */
bool ironman_nightmare;	/* Nightmare mode(it isn't even remotely fair!)(*) */
bool left_hander;	/* Left-Hander */
bool preserve_mode;	/* Preserve artifacts (*) */
bool autoroller;	/* Allow use of autoroller for stats (*) */
bool autochara;	/* Autoroll for weight, height and social status */
bool powerup_home;	/* Increase capacity of your home (*) */

bool ironman_no_fixed_art;	/*:::追加オプション ★が出ない*/
bool no_capture_book; /*:::妖魔本が出現しない*/
bool birth_bonus_20;
bool birth_bonus_10;
bool ironman_no_bonus;

/*** Easy Object Auto-Destroyer ***/

bool destroy_items;	/* Use easy auto-destroyer */
bool destroy_feeling;	/* Apply auto-destroy as sense feeling */
bool destroy_identify;	/* Apply auto-destroy as identify an item */
bool leave_worth;	/* Auto-destroyer leaves known worthy items */
bool leave_equip;	/* Auto-destroyer leaves weapons and armour */
bool leave_chest;	/* Auto-destroyer leaves closed chests */
bool leave_wanted;	/* Auto-destroyer leaves wanted corpses */
bool leave_corpse;	/* Auto-destroyer leaves corpses and skeletons */
bool leave_junk;	/* Auto-destroyer leaves junk */
bool leave_special;	/* Auto-destroyer leaves items your race/class needs */


/*** Play-record Options ***/

bool record_fix_art;	/* Record fixed artifacts */
bool record_rand_art;	/* Record random artifacts */
bool record_destroy_uniq;	/* Record when destroy unique monster */
bool record_fix_quest;	/* Record fixed quests */
bool record_rand_quest;	/* Record random quests */
bool record_maxdepth;	/* Record movements to deepest level */
bool record_stair;	/* Record recall and stair movements */
bool record_buy;	/* Record purchased items */
bool record_sell;	/* Record sold items */
bool record_danger;	/* Record hitpoint warning */
bool record_arena;	/* Record arena victories */
bool record_ident;	/* Record first identified items */
bool record_named_pet;	/* Record informations of named pets */


/* Cheating options */

bool cheat_peek;	/* Peek into object creation */
bool cheat_hear;	/* Peek into monster creation */
bool cheat_room;	/* Peek into dungeon creation */
bool cheat_xtra;	/* Peek into something else */
bool cheat_know;	/* Know complete monster info */
bool cheat_live;	/* Allow player to avoid death */
bool cheat_save;	/* Ask for saving death */


/* Special options */

byte hitpoint_warn;	/* Hitpoint warning (0 to 9) */
byte mana_warn;	/* Mana color (0 to 9) */

byte delay_factor;	/* Delay factor (0 to 9) */

bool autosave_l;	/* Autosave before entering new levels */
bool autosave_t;	/* Timed autosave */
s16b autosave_freq;     /* Autosave frequency */


/*
 * Dungeon variables
 */

bool closing_flag;		/* Dungeon is closing */


/*
 * Dungeon size info
 */
/*:::フロアの升目数　cur_hgtとcur_widとは別なのか？*/
/*:::↑たぶんフロアでなく現在の画面の升目数*/
s16b panel_row_min, panel_row_max;
s16b panel_col_min, panel_col_max;
s16b panel_col_prt, panel_row_prt;

/*
 * Player location in dungeon
 */
int py;
int px;

/*
 * Targetting variables
 */
s16b target_who;/*:::ターゲットモンスターのm_list[]番号*/
s16b target_col;/*:::ターゲットモンスターのx位置*/
s16b target_row;/*:::ターゲットモンスターのy位置*/


/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Current player's character name
 */
/*:::＠の名前*/
char player_name[32];

///mod140206 ＠の職業名を追加　男女で分けたり職によっては自分で入力する
char player_class_name[32];


/*
 * Stripped version of "player_name"
 */
char player_base[32];


/*
 * Buffer to hold the current savefile name
 * 'savefile' holds full path name. 'savefile_base' holds only base name.
 */
char savefile[1024];
char savefile_base[40];

/*
 * Array of grids lit by player lite (see "cave.c")
 */
s16b lite_n;
s16b lite_y[LITE_MAX];
s16b lite_x[LITE_MAX];

/*
 * Array of grids lit by player lite (see "cave.c")
 */
s16b mon_lite_n;
s16b mon_lite_y[MON_LITE_MAX];
s16b mon_lite_x[MON_LITE_MAX];

/*
 * Array of grids viewable to the player (see "cave.c")
 */
s16b view_n;
byte view_y[VIEW_MAX];
byte view_x[VIEW_MAX];

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
byte temp_y[TEMP_MAX];
byte temp_x[TEMP_MAX];

/*
 * Array of grids for delayed visual updating (see "cave.c")
 */
s16b redraw_n = 0;
byte redraw_y[REDRAW_MAX];
byte redraw_x[REDRAW_MAX];


/*
 * Number of active macros.
 */
s16b macro__num;

/*
 * Array of macro patterns [MACRO_MAX]
 */
cptr *macro__pat;

/*
 * Array of macro actions [MACRO_MAX]
 */
cptr *macro__act;

/*
 * Array of macro types [MACRO_MAX]
 */
bool *macro__cmd;

/*
 * Current macro action [1024]
 */
char *macro__buf;


/*
 * The number of quarks
 */
s16b quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;


/*
 * The next "free" index to use
 */
u16b message__next;

/*
 * The index of the oldest message (none yet)
 */
u16b message__last;

/*
 * The next "free" offset
 */
u16b message__head;

/*
 * The offset to the oldest used char (none yet)
 */
u16b message__tail;

/*
 * The array of offsets, by index [MESSAGE_MAX]
 */
u16b *message__ptr;

/*
 * The array of chars, by offset [MESSAGE_BUF]
 */
char *message__buf;


/*
 * The array of normal options
 */
u32b option_flag[8];
u32b option_mask[8];


/*
 * The array of window options
 */
u32b window_flag[8];
u32b window_mask[8];


/*
 * The array of window pointers
 */
/*:::ウィンドウに対応しているらしい*/
term *angband_term[8];


/*
 * Standard window names
 */
const char angband_term_name[8][16] =
{
	"Hengband",
	"Term-1",
	"Term-2",
	"Term-3",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Global table of color definitions
 */
byte angband_color_table[256][4] =
{
	{0x00, 0x00, 0x00, 0x00},	/* TERM_DARK */
	{0x00, 0xFF, 0xFF, 0xFF},	/* TERM_WHITE */
	{0x00, 0x80, 0x80, 0x80},	/* TERM_SLATE */
	{0x00, 0xFF, 0x80, 0x00},	/* TERM_ORANGE */
	{0x00, 0xC0, 0x00, 0x00},	/* TERM_RED */
	{0x00, 0x00, 0x80, 0x40},	/* TERM_GREEN */
	{0x00, 0x00, 0x00, 0xFF},	/* TERM_BLUE */
	{0x00, 0x80, 0x40, 0x00},	/* TERM_UMBER */
	{0x00, 0x40, 0x40, 0x40},	/* TERM_L_DARK */
	{0x00, 0xC0, 0xC0, 0xC0},	/* TERM_L_WHITE */
	{0x00, 0xFF, 0x00, 0xFF},	/* TERM_VIOLET */
	{0x00, 0xFF, 0xFF, 0x00},	/* TERM_YELLOW */
	{0x00, 0xFF, 0x00, 0x00},	/* TERM_L_RED */
	{0x00, 0x00, 0xFF, 0x00},	/* TERM_L_GREEN */
	{0x00, 0x00, 0xFF, 0xFF},	/* TERM_L_BLUE */
	{0x00, 0xC0, 0x80, 0x40}	/* TERM_L_UMBER */
};


/*
 * Standard sound names
 */
const cptr angband_sound_name[SOUND_MAX] =
{
	"dummy",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
	"study",
	"teleport",
	"shoot",
	"quaff",
	"zap",
	"walk",
	"tpother",
	"hitwall",
	"eat",
	"store1",
	"store2",
	"store3",
	"store4",
	"dig",
	"opendoor",
	"shutdoor",
	"tplevel",
	"scroll",
	"buy",
	"sell",
	"warn",
	"rocket",
	"n_kill",
	"u_kill",
	"quest",
	"heal",
	"x_heal",
	"bite",
	"claw",
	"m_spell",
	"summon",
	"breath",
	"ball",
	"m_heal",
	"atkspell",
	"evil",
	"touch",
	"sting",
	"crush",
	"slime",
	"wail",
	"winner",
	"fire",
	"acid",
	"elec",
	"cold",
	"illegal",
	"fail",
	"wakeup",
	"invuln",
	"fall",
	"pain",
	"destitem",
	"moan",
	"show",
	"unused",
	"explode",
	"glass",
};


/*
* Standard music names
*/
//v1.1.58 本家BGM機能を改変コピー
const cptr angband_music_basic_name[MUSIC_BASIC_MAX] =
{
	"new_game",
	"gameover",
	"exit",
	"town",
	"field1",
	"field2",
	"field3",
	"dun_low",
	"dun_med",
	"dun_high",
	"feel1",
	"feel2",
	"winner",
	"build",
	"wild",
	"quest",
	"arena",
	"battle",
	"quest_clear",
	"final_quest_clear",
	"ambush",
	"nightmare1",
	"nightmare2",
	"nightmare3",
	"nightmare4",
	"nightmare5",
	"nightmare6",
	"nightmare7",
	"nightmare8",
	"the_world",
	"invulner",
	"lightspeed",
	"wraith",
	"hallucination",
	"confusion",
	"fear",
	"berserk",

	"metamorphosis",
	"m_gigantic",
	"m_demon_lord",
	"m_beast",
	"m_marisa",
	"m_dragon",
	"m_other",
	"bydo",

	"quest_extra10",
	"quest_extra20",
	"quest_extra30",
	"quest_extra40",
	"quest_extra50",
	"quest_extra60",
	"quest_extra70",
	"quest_extra80",
	"quest_extra90",

	"default_floor",
	//v1.1.59追加
	"build_ex",
};


/*
 * The array of "cave grids" [MAX_WID][MAX_HGT].
 * Not completely allocated, that would be inefficient
 * Not completely hardcoded, that would overflow memory
 */
/*:::ダンジョンの今居る階の全てのマスの情報*/
cave_type *cave[MAX_HGT];


/*
 * The array of saved floors
 */
saved_floor_type saved_floors[MAX_SAVED_FLOORS];


/*
 * Number of floor_id used from birth
 */
s16b max_floor_id;


/*
 * Sign for current process used in temporal files.
 * Actually it is the start time of current process.
 */
u32b saved_floor_file_sign;


/*
 * The array of dungeon items [max_o_idx]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [max_m_idx]
 */
monster_type *m_list;

/*
 * The array to process dungeon monsters [max_m_idx]
 */
s16b *mproc_list[MAX_MTIMED];
s16b mproc_max[MAX_MTIMED]; /* Number of monsters to be processed */


/*
 * Maximum number of towns
 */
u16b max_towns;

/*
 * The towns [max_towns]
 */
town_type *town;


/*
 * The player's inventory [INVEN_TOTAL]
 */
/*:::所持アイテム、装備品一覧*/
object_type *inventory;

///mod140608 追加インベントリ
object_type *inven_add;

///mod160205 小傘に鍛冶依頼でアイテムを預けるため特殊スペースを作った。この欄はinven_add[]と違い重量計算がされない。
object_type *inven_special;

/*
 * The size of "alloc_kind_table" (at most max_k_idx * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;


/*
 * The size of "alloc_race_table" (at most max_r_idx)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;


/*
 * Specify attr/char pairs for visual special effects
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte misc_to_attr[256];
char misc_to_char[256];


/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
/*:::アイテム大分類ごとにインベントリに表示する色を指定してある*/
/*:::この変数はprocess_pref_file_command()で初期化されているらしい*/
byte tval_to_attr[128];
char tval_to_char[128];


/*
 * Keymaps for each "mode" associated with each keypress.
 */
cptr keymap_act[KEYMAP_MODES][256];



/*** Player information ***/

/*
 * Static player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;

/*
 * Pointer to the player tables
 * (sex, race, class, magic)
 */
const player_sex *sp_ptr;
const player_race *rp_ptr;
const player_class *cp_ptr;
const player_seikaku *ap_ptr;

/*:::職業決定時にm_info[class]からコピーされてくる。*/
///mod140207 魔法仕様変更で削除
//const player_magic *mp_ptr;


/*
 * The last character rolled,
 * holded for quick start
 */
/*:::クイックスタート用キャラクターメイクデータ*/
birther previous_char;


/*
 * The vault generation arrays
 */
vault_type *v_info;
char *v_name;
/*:::v_info.txtにあるD:行が「全て」入っている。使うときはv_info[].textという32bit値をv_textに足してそれを開始アドレスとする。*/
char *v_text;

/*
 * The skill table
 */
/*:::各職業ごとの武器熟練度と騎乗などのスキルの初期値、最大値*/
//skill_table *s_info;

/*
 * The magic info
 */
/*:::このm_info[]は、キャラメイク時とロード時に参照されているのみ。*/
/*:::その後はmp_ptrが使われる。*/
///mod140208 魔法仕様変更で削除
//player_magic *m_info;

/*
 * The terrain feature arrays
 */
/*:::Q002 このf_info[]はいつ代入されている？*/
/*:::↑起動時にinit_f_info()で*/
feature_type *f_info;
char *f_name;
char *f_tag;

/*
 * The object kind arrays
 */
object_kind *k_info;
char *k_name;
char *k_text;

/*
 * The artifact arrays
 */
artifact_type *a_info;
char *a_name;
char *a_text;

/*
 * The ego-item arrays
 */
ego_item_type *e_info;
char *e_name;
char *e_text;


/*
 * The monster race arrays
 */
monster_race *r_info;
char *r_name;
char *r_text;


/*
 * The dungeon arrays
 */
dungeon_info_type *d_info;
char *d_name;
char *d_text;


/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Hack -- The special Angband "Keyboard Suffix"
 * This variable is used to choose an appropriate macro-trigger definition
 */
#ifdef JP
cptr ANGBAND_KEYBOARD = "JAPAN";
#else
cptr ANGBAND_KEYBOARD = "0";
#endif

/*
 * Hack -- The special Angband "Graphics Suffix"
 * This variable is used to choose an appropriate "graf-xxx" file
 */
cptr ANGBAND_GRAF = "ascii";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;

/*
 * High score files (binary)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_APEX;

/*
 * Bone files for player ghosts (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_BONE;

/*
 * Binary image files for the "*_info" arrays (binary)
 * These files are not portable between platforms
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_EDIT;

/*
 * Script files
 * These files are portable between platforms.
 */
cptr ANGBAND_DIR_SCRIPT;

/*
 * Various extra files (ascii)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_FILE;

/*
 * Help files (normal) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_HELP;

/*
 * Help files (spoilers) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_INFO;

/*
 * Default user "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_PREF;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Various extra files (binary)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_XTRA;


/*
 * Total Hack -- allow all items to be listed (even empty ones)
 * This is only used by "do_cmd_inven_e()" and is cleared there.
 */
/*:::装備品の一覧を表示するとき未装備のスロットも表示するフラグ*/
bool item_tester_full;

/*:::get_item()で使われるフラグの一つ　両手持ちに関する何からしいが詳細不明*/
//これをTRUEにしないと装備品選択時に両手持ち部分が表示される？
bool item_tester_no_ryoute = FALSE;

/*
 * Here is a "pseudo-hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
byte item_tester_tval;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
bool (*item_tester_hook)(object_type*);



/*
 * Current "comp" function for ang_sort()
 */
bool (*ang_sort_comp)(vptr u, vptr v, int a, int b);


/*
 * Current "swap" function for ang_sort()
 */
void (*ang_sort_swap)(vptr u, vptr v, int a, int b);



/*
 * Hack -- function hooks to restrict "get_mon_num_prep()" function
 */
monster_hook_type get_mon_num_hook;
monster_hook_type get_mon_num2_hook;


/*
 * Hack -- function hook to restrict "get_obj_num_prep()" function
 */
/*:::Q004 この構文よく知らない。*/
/*::: get_obj_num_hookを「intを引数としboolを返す関数」へのポインタとして定義しさらにk_idxをグローバル変数にした・・？*/
bool (*get_obj_num_hook)(int k_idx);


/* Hack, monk armour */
bool monk_armour_aux;
bool monk_notify_aux = FALSE ;

#ifdef ALLOW_EASY_OPEN /* TNB */
bool easy_open;
#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM /* TNB */
bool easy_disarm;
#endif /* ALLOW_EASY_DISARM -- TNB */

#ifdef ALLOW_EASY_FLOOR /* TNB */
bool easy_floor;
#endif /* ALLOW_EASY_FLOOR -- TNB */

bool use_command;
bool center_player;
bool center_running;

/* Auto-destruction options */
bool destroy_items;
bool destroy_feeling;
bool destroy_identify;
bool leave_worth;
bool leave_equip;
bool leave_wanted;
bool leave_corpse;
bool leave_junk;
bool leave_chest;
bool leave_special;

/* Nikki */
bool record_fix_art;
bool record_rand_art;
bool record_destroy_uniq;
bool record_fix_quest;
bool record_rand_quest;
bool record_maxdepth;
bool record_stair;
bool record_buy;
bool record_sell;
bool record_danger;
bool record_arena;
bool record_ident;
bool record_named_pet;
char record_o_name[MAX_NLEN];
s32b record_turn;

/*
 * Wilderness
 */
wilderness_type **wilderness;


/*
 * Buildings
 */
building_type building[MAX_BLDG];


/*
 * Maximum number of quests
 */
u16b max_quests;

/*
 * Maximum number of monsters in r_info.txt
 */
u16b max_r_idx;

/*
 * Maximum number of items in k_info.txt
 */
u16b max_k_idx;

/*
 * Maximum number of vaults in v_info.txt
 */
u16b max_v_idx;

/*
 * Maximum number of terrain features in f_info.txt
 */
u16b max_f_idx;

/*
 * Maximum number of artifacts in a_info.txt
 */
u16b max_a_idx;

/*
 * Maximum number of ego-items in e_info.txt
 */
u16b max_e_idx;

/*
 * Maximum number of dungeon in e_info.txt
 */
u16b max_d_idx;

/*
 * Maximum number of objects in the level
 */
u16b max_o_idx;

/*
 * Maximum number of monsters in the level
 */
u16b max_m_idx;

/*
 * Maximum size of the wilderness
 */
s32b max_wild_x;
s32b max_wild_y;

/*
 * Quest info
 */
quest_type *quest;

/*
 * Quest text
 */
/*:::クエスト受領、完了、失敗のときの文が入る*/
char quest_text[10][128];

/*
 * Current line of the quest text
 */
int quest_text_line;

/*
 * Default spell color table (quark index)
 */
s16b gf_color[MAX_GF];

/*
 * Flags for initialization
 */
/*:::荒野や町など初期化がどこまで完了したかを示している？*/
int init_flags;


/*
 * The "highscore" file descriptor, if available.
 */
/*:::ハイスコアファイルのポインタ*/
int highscore_fd = -1;

/*:::突然変異による回復速度減少(% 無変異で100 最低10まで下がる)*/
int mutant_regenerate_mod = 100;

bool can_save = FALSE;        /* Game can be saved */

/*:::時間停止をするモンスターに関するフラグらしい*/
s16b world_monster;

bool world_player;

/*:::以下5つは捕獲モンスターのパラメータ格納用。モンスターボール発動時にしか使われていない*/
int cap_mon;
int cap_mspeed;
int cap_hp;
int cap_maxhp;
u16b cap_nickname;

///mod150614 闘技場変更に伴い、古い変数を全て無効化した
/*
s16b battle_mon[4];
int sel_monster;
int battle_odds;
int kakekin;
u32b mon_odds[4];
*/

/*:::ペットのターゲットモンスターのidx*/
int pet_t_m_idx;
int riding_t_m_idx;

/*:::賞金首のidxが入る。既に換金済みの場合idxに+10000される*/
s16b kubi_r_idx[MAX_KUBI];
/*:::本日の賞金首ID*/
s16b today_mon;

bool write_level;

u32b playtime;
u32b start_time;

int tsuri_dir;

//bool sukekaku;
bool new_mane;

/*:::感知範囲外でモンスター同士が戦ったターンはtrue process_world()で「何かが聞こえた」と表示*/
bool mon_fight;

//輝夜(永琳プレイ時)に輝夜が交戦したとき、残りHPを%で記録
byte kaguya_fight = 100;

/*:::ワールドマップで襲撃を受けたときTRUEになる*/
bool ambush_flag;

/*:::ワールドマップで襲撃されたときTRUE。敵が近くに生成されて起きている？*/
bool generate_encounter;

cptr screen_dump = NULL;

/*** Terrain feature variables ***/

/* Nothing */
s16b feat_none;

/* Floor */
s16b feat_floor;

/* Objects */
s16b feat_glyph;
s16b feat_explosive_rune;
s16b feat_mirror;
s16b feat_elder_sign;

/* Doors */
door_type feat_door[MAX_DOOR_TYPES];

/* Stairs */
s16b feat_up_stair;
s16b feat_down_stair;
s16b feat_entrance;

/* Special traps */
s16b feat_trap_open;
s16b feat_trap_armageddon;
s16b feat_trap_piranha;

/* Rubble */
s16b feat_rubble;

/* Seams */
s16b feat_magma_vein;
s16b feat_quartz_vein;

/* Walls */
s16b feat_granite;
s16b feat_permanent;

/* Glass floor */
s16b feat_glass_floor;

/* Glass walls */
s16b feat_glass_wall;
s16b feat_permanent_glass_wall;

/* Pattern */
s16b feat_pattern_start;
s16b feat_pattern_1;
s16b feat_pattern_2;
s16b feat_pattern_3;
s16b feat_pattern_4;
s16b feat_pattern_end;
s16b feat_pattern_old;
s16b feat_pattern_exit;
s16b feat_pattern_corrupted;

/* Various */
s16b feat_black_market;
s16b feat_town;

/* Terrains */
s16b feat_deep_water;
s16b feat_shallow_water;
s16b feat_deep_lava;
s16b feat_shallow_lava;
s16b feat_dirt;
s16b feat_grass;
s16b feat_flower;
s16b feat_brake;
s16b feat_tree;
s16b feat_mountain;
s16b feat_swamp;

//v1.1.64 ダンジョン「地獄」用
s16b feat_needle_mat;
//v1.1.68 新地形「氷塊」
s16b feat_ice_wall;
//v1.1.85 新地形追加
s16b feat_heavy_cold_zone;
s16b feat_cold_zone;
s16b feat_heavy_electrical_zone;
s16b feat_electrical_zone;
s16b feat_deep_acid_puddle;
s16b feat_acid_puddle;
s16b feat_deep_poisonous_puddle;
s16b feat_poisonous_puddle;
s16b feat_deep_miasma;
s16b feat_thin_miasma;
//v1.1.91
s16b feat_oil_field;



/* Unknown grid (not detected) */
s16b feat_undetected;

///mod140209 暗い穴追加
s16b feat_dark_pit;

/*
 * Which dungeon ?
 */
/*:::今いるダンジョン　地上なら0*/
byte dungeon_type;
s16b *max_dlv;

//v1.1.25 ダンジョン制覇フラグ
byte *flag_dungeon_complete;

s16b feat_wall_outer;
s16b feat_wall_inner;
s16b feat_wall_solid;
s16b floor_type[100], fill_type[100];

/*:::画面送りの判定に使うらしい*/
bool now_damaged;
s16b now_message;
bool use_menu;

#ifdef CHUUKEI
bool chuukei_server;
bool chuukei_client;
char *server_name;
int server_port;
#endif

/* for movie */
bool browsing_movie;

#ifdef TRAVEL
/* for travel */
travel_type travel;
#endif

/* for snipers */
/*:::どの射撃をしたかのフラグ*/
int snipe_type = SP_NONE;
bool reset_concent = FALSE;   /* Concentration reset flag */
/*:::スナイパーが特殊射撃をしたかどうかのフラグ*/
bool is_fired = FALSE;

///mod140373 難易度
byte difficulty;
//mod140407 引退時にのみTRUE
bool finish_the_game = FALSE;

//mod140420 ＠の召喚時、TRUEにしておくと＠の所属を無視する
bool ignore_summon_align = FALSE;

///mod140705 騎乗時の落馬確率通知メッセージ用
byte rakuba_type=0;

///mod140719 ランダムユニーク
char random_unique_name[3][80];
s16b random_unique_akills = 0;

///mod140814 高速詠唱
bool quick_cast = FALSE;

///mod140820 エンジニア
bool scouter_use = FALSE;

/*:::new_class_powerからの特技使用時に次回行動までの必要エネルギーを再設定する変数。これが0なら通常通り100が、0でなければその数値がenergy_useに格納される*/
int new_class_power_change_energy_need=0;

///mod140820 さとりが読心するときのフラグ
bool satori_reading = FALSE;


///mod151016 村紗がmap_area()で水地形のみを感知する専用フラグ
bool murasa_detect_water = FALSE;

///mod150129 霊夢
u32b osaisen = 0;
//int oonusa_dam = 0; v1.1.44 削除

///mod150219 魔理沙用
u32b marisa_magic_power[8] = {0,0,0,0,0,0,0,0};

///mod150331 正邪用
bool flag_seija_quest_comp = FALSE;

bool flag_sumireko_wakeup = FALSE;

///mod150614 闘技場変更
s16b battle_mon_team_type[4];//闘技場a-dの選択肢のチーム編成 0なら単体、正ならランダムチーム、負なら固定チーム
int	 battle_mon_wager; //闘技場で賭けた金額
int  battle_mon_odds; //闘技場で賭けた倍率*100
u32b battle_mon_team_bet; //闘技場で賭けたチーム MFLAG_BATTLE_MON_TEAM_A～Dが入る
int  battle_mon_base_level; //闘技場でのモンスター生成基本レベル　レイマリ配置時のパラメータ算出に使うためグローバル変数に保持する必要がある
battle_mon_type battle_mon_list[BATTLE_MON_LIST_MAX]; //闘技場モンスターのr_idxと所属チームが入る

bool flag_mokou_resurrection = FALSE;
bool flag_mokou_possess = FALSE; //妹紅がパゼストバイフェニックスを使って敵にダメージを与えるとき専用フラグ　血の呪いの対象にならない

///mod150719 布都
s16b feat_plate; //皿

///mod150801 スターサファイア
s16b feat_bomb;

///mod150919 ヘカーティア特殊処理用
//特別行動のとき、残った体のうちでHPの高いほうを選定し、自分を消滅させて別のを呼ぶ
//呼ぶとき自分のHPをhecate_hpに格納し、呼んだ体のHPに該当エントリの数値を設定する
//[1][2]が倒されたとき[0]の体を呼び出す。
//階移動のとき、[0][1][2]に5600(最大HP)を入れる
u32b hecatia_hp[3] = {0,0,0};

///mod150919 特殊モンスターの名前用変数
char extra_mon_name[80];

//輝夜クエストの進行度　ゲーム終了後も次の＠に引き継がれる
int kaguya_quest_level = 0;

//魔法書を消費してどんな魔法も一度だけ使う特殊フラグ
bool flag_spell_consume_book = FALSE;
//v1.1.67 習得済みの魔法をひとつ忘れる特殊フラグ
bool flag_spell_forget = FALSE;

//EXダンジョン用建物idxが格納される。f_infoのBUILDING_EX1-4に対応し、この値を参照してdungeon()直後にbuilding[0-3]が書き直される
byte building_ex_idx[BLDG_EX_NUM] = {0,0,0,0};
//Exダンジョンに出現した建物の履歴　最近出た建物を出にくくするため記録
byte	ex_buildings_history[EX_BUILDINGS_HISTORY_MAX] = {0,0,0,0,0,0};
//建物用パラメータ　とりあえず0-3はf_infoのBUILDING_EX1-4に対応。255のときもう建物を利用できない。
byte	ex_buildings_param[EX_BUILDINGS_PARAM_MAX] = {0,0,0,0,0,0};


//特殊な自宅アクセスーモードのフラグ

//魔理沙の着せかえ魔法やEXTRA建物など　家から出る時に確認
bool hack_flag_access_home = FALSE;
//魔法の矢筒発動　矢のみ扱える
bool hack_flag_access_home_only_arrow = FALSE;
//お燐にアイテムを持ってきてもらう　一種類の取り出しだけ
bool hack_flag_access_home_orin = FALSE;



/*:::Exモードのダンジョン内建物でモンスターが出てきて強制的に建物から出されるための特殊変数*/
/*:::idxが設定されるとsummon_named_creature,typeが指定されるとsummon_specificが呼ばれる*/
int hack_ex_bldg_summon_idx = 0;
int hack_ex_bldg_summon_type = 0;
int hack_ex_bldg_summon_num = 0;
u32b hack_ex_bldg_summon_mode = 0L;

///mod160305 ヤマビコ可能な魔法を受けた時idxを記録 アイテムカードで使うために独立変数にした
s16b monspell_yamabiko = 0;
///mod160319 パルスィの積怨返し用ダメージカウント　アイテムカード用に別変数にした
u32b parsee_damage_count = 0L;

///mod160326 UNIQUE2フラグ持ちを倒した時スコアに加算されるようにする
u32b score_bonus_kill_unique2 = 0L;

//total_points_new()でエントランスのアイテムを計算し直すかどうかのフラグ　ゲーム開始時、エントランスに物をおいた時、エントランスの物を処分した時にTRUE
bool hack_flag_calc_museum_score = TRUE;

//銃による射撃に特殊な効果を付与する。現状清蘭特技専用
int	special_shooting_mode = 0;

//モンスター「八尺さま」のターゲットm_idx
s16b hassyakusama_target_idx = 0;
//ドッペルゲンガー生成フラグ
bool flag_generate_doppelganger = FALSE;

//v1.1.86 リリーホワイト(アビリティカードで出てくる版)生成フラグ
//呼び出されるレベルも兼ねるため整数型にする
int flag_generate_lilywhite = 0;

//袿姫特技　鬼形造形術で特殊ランダムユニークを生成するための生成フラグ兼生成タイプ保持
int special_idol_generate_type = 0;

bool break_eibon_flag = FALSE;

bool flag_kinkakuzi = FALSE;

byte dimension_door_distance = 0;

//v1.1.21 銃のクリティカル計算フラグ
u32b hack_gun_fire_critical_flag = 0L;
//銃の命中修正値　クリティカル処理に使う
int	 hack_gun_fire_critical_hit_bonus = 0;

//銃を撃つときTRUEになり、project_mで経験値処理。
//銃には広範囲に攻撃するものがあり、複数のモンスターに攻撃したときその一体一体で熟練度が入ったら変なので最初に当たった一体だけ熟練度処理する
bool hack_flag_gain_gun_skill_exp = FALSE;

int hack_der_freischutz_basenum = 0;

//v1.1.24 レーザートラップ用　project()を＠やモンスターの場所以外から撃ちたいときに
int	hack_project_start_x = 0;
int	hack_project_start_y = 0;
//v1.1.44 project()の着弾点も記録する
int	hack_project_end_x = 0;
int	hack_project_end_y = 0;

//v1.1.24 急流下りクエスト用
u32b	qrkdr_rec_turn = 0;


u32b testcount = 0;
u32b testcount2 = 0;

//v1.1.25 色々カウント追加
s32b gambling_gain = 0;
u32b score_count_pinch = 0;
u32b score_count_pinch2 = 0;

//v1.1.25 スコアサーバ名
char score_server_name[SCORE_SERVER_STR_LEN] = "";
char score_server_key[SCORE_SERVER_STR_LEN] = "";

//v1.1.27 結界ガード
int count_damage_guard_break;

//v1.1.30 友好的なモンスターに許可なく隣接攻撃するフラグ
bool flag_friendly_attack = FALSE;

//v1.1.32 ロープウェイに乗っているフラグ p_ptr->teleport_townと一緒に使う
//v1.1.91 魔法の森から旧地獄市街に行くときにも使うことにしたので、boolのフラグでなくintのモード値にする
int teleport_town_mode = 0;

//v1.1.35 get_itemでリピートのとき別のアイテムが選ばれることを許容しないフラグ
bool item_tester_not_allow_different_repeat = FALSE;

//v1.1.39 変異強力発動フラグ
bool hack_flag_powerup_mutation = FALSE;

//v1.1.43 サニーの特技「ルチルフレクション」専用フラグ
bool flag_sunny_refraction = FALSE;

//v1.1.44 布都の特技で皿が割れた時のフラグ
bool flag_futo_broken_plate = FALSE;



//v1.1.51	「夢の世界に行く(仮)」コマンド関連
s16b	nightmare_diary_count = 0; 	//夢の世界に行った回数
s16b	nightmare_diary_win = 0;	//夢の世界で勝った回数
u32b	nightmare_total_point = 0;	//夢の世界で得た合計ポイント

//夢の世界に出るモンスターIDX マップ生成時と商品アイテム生成時に使用　勝負中の中断に備えてセーブデータに記録する
s16b	nightmare_mon_r_idx[NIGHTMARE_DIARY_MONSTER_MAX];

//今いる夢の世界で勝ったとき得られるポイント
u32b	tmp_nightmare_point;
s16b	nightmare_mon_base_level;	//夢の世界の可変パラメータモンスターのベースレベル　

//v1.1.55 アリーナに入ったときのHPを記録する
s16b	nightmare_record_hp = 0;

//v1.1.58 BGMを鳴らすモンスターのr_idx
int music_r_idx = 0;

//v1.1.58
//＠の行動開始時にselect_floor_music()を呼んで音楽を更新するかどうかのフラグ
//最初PU_MUSICを設定してp_ptr->updateで処理しようとしたが要らん更新が起こりすぎるので独立フラグにした
bool	flag_update_floor_music = FALSE;
//music.cfgから設定するオプション
bool	flag_unique_class_use_monster_bgm = FALSE;
bool	flag_display_bgm_title = FALSE;

//今流れている音楽のタイトルを保存する場所へのポインタ
cptr bgm_title_text_ptr = NULL;

//敵対召喚されたモンスターを買収するフラグ
bool	flag_bribe_summon_monsters = FALSE;

//月虹市場でトレードした回数 毎日6:00にリセット　player_wipe()でリセット
s16b	ability_card_trade_count = 0;

//10連ボックスを購入した回数　player_wipe()でリセット
s16b	buy_gacha_box_count = 0;

// カードショップに支払った累計金額　新キャラ作り直してもリセットされない
u32b	total_pay_cardshop = 0;

//店の在庫を'x'で調べるときにTRUEになる一時フラグ
bool hack_flag_store_examine = FALSE;

//テスト用
int	gv_test1=0;
int	gv_test2=0;
int	gv_test3=0;
int	gv_test4=0;

//v2.0.1 「生命爆発の薬」の効果による爆発を発生させるフラグ
bool	flag_life_explode = FALSE;

//v2.0.5 ぬえプレイ中にこのフラグが立っているとき行動時に正体不明状態に変身可能かどうかチェックする
bool	flag_nue_check_undefined = TRUE;

//v2.0.11 慧ノ子(モンスター)のトラップ生成が全てトラバサミになる特殊フラグ
bool	hack_flag_enoko_make_beartrap = FALSE;

//v2.1.0 怨霊に応援されているときnew_class_power_aux経由の攻撃の属性が変更される特殊フラグ
bool	hack_flag_darkness_power = FALSE;

