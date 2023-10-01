/* File: types.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: global type declarations */


/*
 * This file should ONLY be included by "angband.h"
 */

/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Certain data is saved in multiple places for efficient access, currently,
 * this includes the tval/sval/weight fields in "object_type", various fields
 * in "header_type", and the "m_idx" and "o_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */





/*
 * Feature state structure
 *
 * - Action (FF_*)
 * - Result (f_info ID)
 */
typedef struct feature_state feature_state;

struct feature_state
{
	byte action;
	s16b result;
};


/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	u32b name;                /* Name (offset) */
	u32b text;                /* Text (offset) */
	s16b tag;                 /* Tag (offset) */

	/*:::*/
	s16b mimic;               /* Feature to mimic */

	/*:::defines.hにFF_????として定義されているフラグが入ってるらしい。*/
	u32b flags[FF_FLAG_SIZE]; /* Flags */

	u16b priority;            /* Map priority */
	s16b destroyed;           /* Default destroyed state */

	feature_state state[MAX_FEAT_STATES];


	byte subtype;
	/*:::ドアに鍵が掛かっている場合もpowerに数値が入っている*/
	byte power;

	byte d_attr[F_LIT_MAX];   /* Default feature attribute */
	byte d_char[F_LIT_MAX];   /* Default feature character */

	byte x_attr[F_LIT_MAX];   /* Desired feature attribute */
	byte x_char[F_LIT_MAX];   /* Desired feature character */
};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */
	/*:::不確定名あれば　ゲーム開始時にシャッフルされる*/
	u32b flavor_name;		/* Flavor name (offset) */

	/*:::tval:アイテムの分類番号 例えば重鎧なら37 defines.hに定義されk_info.txtに記録されている*/
	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s16b pval;			/* Object extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage dice/sides */

	s16b weight;		/* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags[TR_FLAG_SIZE];	/* Flags */

	u32b gen_flags;		/* flags for generate */


	/*:::基本生成レベルとレアリティ以外にレベルとレアリティの設定をしたいとき4つまで入力・・らしいが、使われ方がよくわからない。*/
	/*:::init_object_allocでだけ使われている。*/
	byte locale[4];		/* Allocation level(s) */
	byte chance[4];		/* Allocation chance(s) */

	byte level;			/* Level */
	/*:::k_info.txtのレアリティ値が入っているが、アイテム生成時にはレアリティとして使われていないようだ*/
	byte extra;			/* Something */


	byte d_attr;		/* Default object attribute */
	byte d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	byte x_char;		/* Desired object character */


	s16b flavor;		/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */

	/*:::鑑定済み*/
	bool aware;			/* The player is "aware" of the item's effects */

	/*:::使ったことがある*/
	bool tried;			/* The player has "tried" one of the items */

	byte act_idx;		/* Activative ability index */
};



/*
 * Information about "artifacts".
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */

typedef struct artifact_type artifact_type;

struct artifact_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */ /*:::よく分からんが価格計算用？0なら無価値扱い？*/

	u32b flags[TR_FLAG_SIZE];       /* Artifact Flags */

	u32b gen_flags;		/* flags for generate */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */

	s16b floor_id;          /* Leaved on this location last time */

	byte act_idx;		/* Activative ability index */
};


/*
 * Information about "ego-items".
 */

typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte slot;			/* Standard slot value */
	byte rating;		/* Rating boost */ /*:::*/

	byte level;			/* Minimum level */
	byte rarity;		/* Object rarity */

	byte max_to_h;		/* Maximum to-hit bonus */
	byte max_to_d;		/* Maximum to-dam bonus */
	byte max_to_a;		/* Maximum to-ac bonus */

	byte max_pval;		/* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	u32b flags[TR_FLAG_SIZE];	/* Ego-Item Flags */

	u32b gen_flags;		/* flags for generate */

	byte act_idx;		/* Activative ability index */
};




/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */

typedef struct monster_blow monster_blow;

struct monster_blow
{
	/*:::u16bに変更する？*/
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};


typedef struct mbe_info_type mbe_info_type;

struct mbe_info_type
{
	int power;        /* The attack "power" */
	int explode_type; /* Explosion effect */
};


/*
 * Monster "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Note that several of these fields, related to "recall", can be
 * scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc).  All of the "recall"
 * fields have a special prefix to aid in searching for them.
 */


typedef struct monster_race monster_race;
/*:::モンスター情報　種族ごと:::*/
struct monster_race
{
	u32b name;				/* Name (offset) */
#ifdef JP
	u32b E_name;                    /* 英語名 (offset) *//*:::スポイラーファイル出力には使われているようだ*/
#endif
	u32b text;				/* Text (offset) */

	byte hdice;				/* Creatures hit dice count */
	byte hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	/*:::使われていない。r_info.txtでは常に0が入っている*/
	/*:::ここの数値を「出現ダンジョン」として使うことにした。RF8_STAY_AT_HOMEと合わせて使う。*/
	s16b extra;				/* Unused (for now) */

	/*:::呪文使用頻度　%で格納される。1/2なら50*/
	byte freq_spell;		/* Spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */
	u32b flags7;			/* Flags 7 (movement related abilities) */
	u32b flags8;			/* Flags 8 (wilderness info) */
	/*:::flags9には新たなモンスター魔法を入れた*/
	u32b flags9;			/* Flags 9 (drops info) */
	u32b flagsr;			/* Flags R (resistances info) */

	monster_blow blow[4];	/* Up to four blows per round */

	s16b next_r_idx;
	u32b next_exp;

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */


	byte d_attr;			/* Default monster attribute */
	byte d_char;			/* Default monster character */


	byte x_attr;			/* Desired monster attribute */
	byte x_char;			/* Desired monster character */


	byte max_num;			/* Maximum population allowed per level */


	byte cur_num;			/* Monster population on current level */

	/*:::ユニークの現在地。新しく生成されたらその階のIDに書き換わり、古いマップからは消える*/
	s16b floor_id;                  /* Location of unique monster */

	/*:::見るたびにカウントされている。実際には「一度でも見たことがあれば～」的な条件文にのみ使われているようだ*/
	s16b r_sights;			/* Count sightings of this monster */
	s16b r_deaths;			/* Count deaths from this monster */

	s16b r_pkills;			/* Count visible monsters killed in this life */
	s16b r_akills;			/* Count all monsters killed in this life */
	s16b r_tkills;			/* Count monsters killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */

	byte r_xtra1;			/* Something (unused) */
	byte r_xtra2;			/* Something (unused) */

	byte r_drop_gold;		/* Max number of gold dropped at once */
	byte r_drop_item;		/* Max number of item dropped at once */

	/*:::モンスターが呪文を唱えるところを見た回数？　100を越えたら呪文使用確率がわかる？*/
	byte r_cast_spell;		/* Max number of other spells seen */

	byte r_blows[4];		/* Number of times each blow type was seen */

	/*:::＠が知っているモンスターの特徴　7は今のところ使われてない(見ればわかる、もしくは知りようのない特徴)*/
	u32b r_flags1;			/* Observed racial flags */
	u32b r_flags2;			/* Observed racial flags */
	u32b r_flags3;			/* Observed racial flags */
	u32b r_flags4;			/* Observed racial flags */
	u32b r_flags5;			/* Observed racial flags */
	u32b r_flags6;			/* Observed racial flags */
	/* u32b r_flags7; */			/* Observed racial flags */

	///mod140102 flags9追加
	u32b r_flags9;			/*:::新魔法用に新たに作った*/
	u32b r_flagsr;			/* Observed racial resistance flags */
};



/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */
};


/*
 * Information about "skill"
 */

typedef struct skill_table skill_table;

/*:::武器熟練度、騎乗、マーシャルアーツ、二刀流のスキルの初期値、最大値*/
struct skill_table
{
	s16b w_start[5][64];	  /* start weapon exp */
	s16b w_max[5][64];        /* max weapon exp */
	s16b s_start[10];	  /* start skill */
	s16b s_max[10];           /* max skill */
};


/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "o_idx" and "m_idx" fields are very interesting.  There are
 * many places in the code where we need quick access to the actual
 * monster or object(s) in a given cave grid.  The easiest way to
 * do this is to simply keep the index of the monster and object
 * (if any) with the grid, but this takes 198*66*4 bytes of memory.
 * Several other methods come to mind, which require only half this
 * amound of memory, but they all seem rather complicated, and would
 * probably add enough code that the savings would be lost.  So for
 * these reasons, we simply store an index into the "o_list" and
 * "m_list" arrays, using "zero" when no monster/object is present.
 *
 * Note that "o_idx" is the index of the top object in a stack of
 * objects, using the "next_o_idx" field of objects (see below) to
 * create the singly linked list of objects.  If "o_idx" is zero
 * then there are no objects in the grid.
 *
 * Note the special fields for the "MONSTER_FLOW" code.
 */

typedef struct cave_type cave_type;

struct cave_type
{
	u16b info;		/* Hack -- cave flags */

	/*:::地形情報*/
	s16b feat;		/* Hack -- feature type */

	s16b o_idx;		/* Object in this grid */

	s16b m_idx;		/* Monster in this grid */

	/*:::ワールドマップの街番号、ダンジョンへの階段のダンジョンIDなど色々入る*/
	s16b special;	/* Special cave info */

	/*:::隠しドアなど*/
	s16b mimic;		/* Feature to mimic */

	byte cost;		/* Hack -- cost of flowing */
	byte dist;		/* Hack -- distance from player */
	/*:::そのグリッドのコストが計算されたターン？*/
	byte when;		/* Hack -- when cost was computed */

	//v1.1.24 飛行時のdist値 詳細はupdate_flying_dist()参照
	byte flying_dist;

	//v1.1.33 ダンジョングリッド用変数追加 セーブのためにcave_template_type関連にも色々追記
	u32b cave_xtra_flag;
	s16b cave_xtra1;
	u16b cave_xtra2;

};



/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	byte y;
	byte x;
};



/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;
/**::::アイテム情報:::*/
struct object_type
{
	/*:::k_info.txtの番号?*/
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	 /*:::パラメータ上昇値だけでなく箱の生成階、魔道具の使用回数、像や人形、死体のモンスターインデックスも入っている*/
	s16b pval;			/* Item extra-parameter */

	/*:::割引率 0-99*/
	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	/*:::★のアーティファクトIDが入る*/
	//mod140506 ★の数が255を超えたのでname1をu16bにした
	u16b name1;			/* Artifact type, if any */
	byte name2;			/* Ego-Item type, if any */

	/*:::使われていないらしいがコードが残っている。"powerful item"らしい。*/
	/*:::付喪神使いの妖器化カウントに使うことにした。100,200になったとき能力が付与され200でカウントがストップする。*/
	//鍛冶師と宝飾師のとき、鬼の鍛冶屋による武器修復が行われたフラグとしても使うことにした。
	//武器「銃」にも使う
	byte xtra1;			/* Extra info type (now unused) */
	//☆の発動インデックスなど？
	byte xtra2;			/* Extra info activation index */
	/*:::鍛冶師用フラグだが、おもちゃのカンヅメの生成階とかモンスターボールのパラメータなど別のことにも使われている*/
	/*:::付喪神使いの武器の妖器化設定にも使うことにした。妖器化済のときxtra3に255が入る。*/
	//雛の厄落としにも使う
	//v1.1.15 純化された武具にも使う
	byte xtra3;			/* Extra info for weaponsmith */
	s16b xtra4;			/* Extra info fuel or captured monster's current HP *//*:::油壷とランタンの燃料にも使われている*/

	//狐狸戦争の報酬つづら、こころ生成面でも使う
	//雛の呪い解除経験値(1/1000)カウントとしても使うことにした
	//新聞の号数にも使う
	//紫苑の差し押さえカウントにも使う
	s16b xtra5;			/* Extra info captured monster's max HP */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked *//*:::見つかった、触ったなどのフラグ*/

	u16b inscription;	/* Inscription index *//*:::銘のインデックスをここに入れるらしい。実体の文字列はquark__str[]内にある？*/
	u16b art_name;      /* Artifact name (random artifacts) */

	/*:::簡易鑑定したときその種類IDが入る*/
	byte feeling;          /* Game generated inscription number (eg, pseudo-id) */

	/*:::pval対象値、スレイ、耐性、その他能力など片端からここに入ってビットが満杯近い*/
	u32b art_flags[TR_FLAG_SIZE];        /* Extra Flags for ego and artifacts */


	u32b curse_flags;        /* Flags for curse */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */

	//v1.1.25 価格(スコアサーバ用　
	//旧バージョンからのデータロード時に0を入れ、
	//object_value_realで0以外になったとき更新する
	s32b score_value;
	//生成されたターンと階層
	u32b create_turn;
	byte create_lev;

	//v1.1.81 拡張用変数追加
	s16b xtra6;
	s16b xtra7;
	s16b xtra8;
	s16b xtra9;
	u32b new_obj_flags;
	s16b new_obj_param16[8];
	u32b new_obj_param32[4];


};



/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */

typedef struct monster_type monster_type;
/*:::モンスター情報（個体ごと）位置、HP、加速など  :::*/
struct monster_type
{
	s16b r_idx;		/* Monster race index */
	s16b ap_r_idx;		/* Monster race appearance index *//*:::たぬき以外はr_idxと一致？*/
	byte sub_align;		/* Sub-alignment for a neutral monster */

	byte fy;		/* Y location on map */
	byte fx;		/* X location on map */

	s16b hp;		/* Current Hit points */
	s16b maxhp;		/* Max Hit points */
	s16b max_maxhp;		/* Max Max Hit points */

	s16b mtimed[MAX_MTIMED];	/* Timed status counter */

	byte mspeed;	        /* Monster "speed" */
	s16b energy_need;	/* Monster "energy" */

	byte cdis;		/* Current dis from player */

	//byte mflag;		/* Extra monster flags */
	u32b mflag;		/* Extra monster flags */
	byte mflag2;		/* Extra monster flags */

	bool ml;		/* Monster is "visible" */

	s16b hold_o_idx;	/* Object being held (if any) */

	s16b target_y;		/* Can attack !los player */
	s16b target_x;		/* Can attack !los player */

	/*:::ペットの名前　ここに記録するのはquark_str()用のインデックス*/
	u16b nickname;		/* Monster's Nickname */

	u32b exp;

	/*:::モンスターが記憶している＠の耐性情報が入るらしい smart_cheatのときは使われない*/
	u32b smart;			/* Field for "smart_learn" */

	/*:::このモンスターを召喚したモンスターのidx*/
	s16b parent_m_idx;

	//v1.1.81 拡張用変数追加
	s16b future_use[7];

	u32b mflag3;
	u32b mflag4;
	s16b xtra_dir_param;
	s16b timed_shard_count;//遅効性の管狐弾

	s16b new_monster_param3;
	s16b new_monster_param4;
	s16b new_monster_param5;
	s16b new_monster_param6;
	s16b new_monster_param7;
	s16b new_monster_param8;

	u16b mon_random_number; //生成時に0-65535のランダムな数値を入れる。WEIRD_MIND感知判定などランダムだが個体依存なことに使う。


};




/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */


/*:::get_obj_num_prepから使われてるが何の意味があるのかよく分からない*/
/*:::アイテムやモンスターの基本階層を全種類分最初に保持し、その後アイテムやモンスター生成時にどれが選ばれるか決めるときに使う？*/
/*:::prob1-3にどんな数値が入ってどう使われるのかはまだはっきりしない　100辺りを上限にするレアリティ値？*/
/*:::get_mon_num_prepのalloc_race_tableもこれを使っている*/

typedef struct alloc_entry alloc_entry;

struct alloc_entry
{
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	byte prob1;		/* Probability, pass 1 */
	byte prob2;		/* Probability, pass 2 */
	byte prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *
 *	- Normal Value (TRUE or FALSE)
 *
 *	- Option Page Number (or zero)
 *
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *
 *	- Textual name (or NULL)
 *	- Textual description
 */

typedef struct option_type option_type;

struct option_type
{
	bool	*o_var;

	byte	o_norm;

	byte	o_page;

	byte	o_set;
	byte	o_bit;

	cptr	o_text;
	cptr	o_desc;
};


/*
 * Structure for the "quests"
 */
typedef struct quest_type quest_type;

struct quest_type
{
	s16b status;            /* Is the quest taken, completed, finished? */

	s16b type;              /* The quest type */

	char name[60];          /* Quest name */
	s16b level;             /* Dungeon level */

	/*:::ワーグクエなど指定数のモンスターを倒すクエスト用*/
	s16b r_idx;             /* Monster race */
	s16b cur_num;           /* Number killed */
	s16b max_num;           /* Number required */

	/*:::宝物庫用？アーティファクトインデックスが入るらしい*/
	s16b k_idx;             /* object index */

	s16b num_mon;           /* number of monsters on level */

	byte flags;             /* quest flags */

							/*:::どこのダンジョンで行われるか　鉄獄は1 イークは2　オリジナルマップは0*/
	byte dungeon;           /* quest dungeon */

	byte complev;           /* player level (complete) */

	//v1.1.25 達成時プレイ時間を記録する
	u32b comptime;


};

/*:::今は実質使われてないはず*/
/*
 * A store owner
 */
typedef struct owner_type owner_type;

struct owner_type
{
	cptr owner_name;	/* Name */

	s16b max_cost;		/* Purse limit */

	byte max_inflate;	/* Inflation (max) */
	byte min_inflate;	/* Inflation (min) */

	byte haggle_per;	/* Haggle unit */

	byte insult_max;	/* Insult limit */

	byte owner_race;	/* Owner race */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	byte type;				/* Store type */

	byte owner;				/* Owner index */
	byte extra;				/* Unused for now */

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */

	/*:::最後に訪れたときのゲームターン(s32b turn)*/
	s32b last_visit;		/* Last visited on this turn */

	/*:::BM以外の店では予め準備されたデータから商品のベースアイテムを決めているらしい*/
	s16b table_num;			/* Table -- Number of entries */
	s16b table_size;		/* Table -- Total Size of Array */
	s16b *table;			/* Table -- Legal item kinds */

	s16b stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */
};


/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
typedef struct magic_type magic_type;

///mod140315 byteからintにした
struct magic_type
{
	int slevel;		/* Required level (to learn) */ /*:::その職で使えない魔法は100以上が入るらしい*/
	int smana;			/* Required mana (to cast) */
	int sfail;			/* Minimum chance of failure */
	int sexp;			/* Encoded experience bonus */
};

///mod140207 魔法変更
#if 0
/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

typedef struct player_magic player_magic;

struct player_magic
{
	/*:::ここがTV_LIFE_BOOKだと「祈り」で魔法を使う職と判定されるらしい*/
	/*:::Q021 spell_bookの役割がよく解らない　第一領域がそのまま入ってるとも思えないが*/
	/*:::SORCERY,LIFE,MUSIC,HISSATSU,NONEのいずれかが入る。魔法習得方法などに影響する*/
	int spell_book;		/* Tval of spell books (if any) */
	int spell_xtra;		/* Something for later */

	int spell_stat;		/* Stat for spells (if any)  *//*:::影響パラメータ*/
	int spell_type;		/* Spell type (mage/priest) *//*:::使われていないらしい*/

	int spell_first;		/* Level of first spell *//*:::魔法習得個数やMP最大値計算に使う*/
	int spell_weight;		/* Weight that hurts spells */

	/*:::魔法10領域分の基本情報　添え字はREALM_???から1引いた値*/
	magic_type info[MAX_MAGIC][32];    /* The available spells */
};
#endif


/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;			/* Type of sex */
	cptr winner;		/* Name of winner */
#ifdef JP
	cptr E_title;		/* 英語性別 */
	cptr E_winner;		/* 英語性別 */
#endif
};


/*
 * Player racial info
 */

typedef struct player_race player_race;

struct player_race
{
	cptr title;			/* Type of race */

#ifdef JP
	cptr E_title;		/* 英語種族 */
#endif
	s16b r_adj[6];		/* Racial stat bonuses */

	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_sav;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */

	///mod140105 種族変更（準備）
	s16b r_graze;		/*:::感知技能を探索と統合しグレイズ技能と入れ替える*/
//	s16b r_fos;			/* search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */

	byte r_mhp;			/* Race hit-dice modifier */
	byte r_exp;			/* Race experience factor */
	///sysdel 不要なので別の値に使おう
	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females)	  */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	byte infra;			/* Infra-vision	range */

	/*:::向いてるクラス？削除しとこうか*/
	u32b choice;        /* Legal class choices */
/*    byte choice_xtra;   */

	byte realm_aptitude[16];/*:::各魔法領域に対する適性　添字はTV_BOOK_???に対応 [0]はダミー　0は習得不可、5は普通、9は大得意*/
	byte score_mult;		/*:::スコア倍率　基本100*/
	bool flag_nofixed;		/*:::まだ実装されてない　キャラ選択時灰色　自己変容時不可*/
	bool flag_nobirth;		/*:::初期選択不可　キャラ選択時表示されない 自己変容時可*/
	bool flag_special;		/*:::リッチなど　キャラ選択時表示されない 自己変容時不可　一部特殊転生時に可*/
	bool flag_only_unique;  /*:::ユニーククラス専用種族のときTRUE　キャラ選択時表示されず自己変容などでもこの種族にならない*/

	//v1.1.42 ＠に似せたランダムユニークを作るための種族情報
	char random_unique_symbol;
	u32b random_unique_flags3;




};


/*
 * Player class info
 */

typedef struct player_class player_class;

struct player_class
{
	cptr title;			/* Type of class */
	cptr f_title; ///mod140129 女性用職業名追加

#ifdef JP
	/*:::自動拾いやプロセスファイル解釈などの部分で条件判定に使ってるらしい*/
	cptr E_title;		/* 英語職業 */
#endif
	s16b c_adj[6];		/* Class stat modifier */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	//ここグレイズ値に変更予定
	s16b c_fos;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	//ここグレイズ値に変更予定
	s16b x_fos;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */ //大きいほどペット維持コストが高くなる

	///mod140128 追加項目

	/*:::各魔法領域への適性　習得レベルや難度に影響
	 *:::[0]は魔法習得フラグで0だと非魔法職、1だと一領域使用可、2だと赤魔やスペマスなど特殊習得 3だと二領域習得可
	 *:::[1～15]は魔法書のTVALに対応し、0:使用不可 1:司書など(二冊目まで　遅い) 2:一般人など（一部の魔法のみ　習得かなり遅い） 3:探検家など（一部使えない　習得遅い） 4:メイドなど（習得やや遅い） 5:メイジなど 6:ハイメイジなど（高適性)*/
	/*:::  元素、予見、付与、召喚、神秘、生命、破邪、自然、変容、暗黒、死霊、混沌、必殺剣、予備領域1、予備領域2の順*/
	byte realm_aptitude[16];

	/*:::攻撃回数計算値　calc_bonuses()のハードコーディングから移す*/
	byte spell_stat; //魔法をどの能力値で使うか　基本はA_INT
	byte weapon_num;
	byte weapon_mul;
	int weapon_div;
	/*:::スキル初期値と成長率と限界値に影響 1～5*/
	/*:::0:格闘 1:盾 2:乗馬 3:二刀 4:投擲 5-9:ダミー
	 *:::10:短剣～20:銃*/
	byte skill_aptitude[SKILL_EXP_MAX];
	/*:::スコア倍率　基本100*/
	int score_mult;
	/*:::装備重量による魔法への悪影響の起きにくさ 0(影響なしor魔法不使用) 1(ペナルティ重)-5(ペナルティ軽)*/
	byte heavy_cast_mult;
	/*:::武器、魔法系の簡易鑑定精度 1(低い)～5(高い)*/
	byte sense_arms;
	byte sense_mags;

	/*:::純魔法職はTRUE 魔力充填や炎の巻物などの処理に関係 デュアル5%判定もここから*/
	bool magicmaster;
	/*:::まだ実装作業が済んでないことを示すフラグ　キャラ選択時灰色になって選べない*/
	bool flag_nofixed;
	/*:::ユニーククラスフラグ　一般選択から選べない*/
	bool flag_only_unique;

	//草の根ネットワークに加入できる職はTRUE 主にユニーククラスの加入制限用
	bool flag_join_grassroots;

	/*:::MPの増えやすさ 1～5*/
	byte mana_rate;

	//v1.1.58 ユニーククラスに対応するr_idx
	u16b unique_r_idx;

};


typedef struct player_seikaku player_seikaku;
struct player_seikaku
{
	/*:::*/
	cptr title;			/* Type of seikaku */
	cptr f_title;			/* Type of seikaku */ //女性用

#ifdef JP
	cptr E_title;		/* 英語性格 */
#endif

	s16b a_adj[6];		/* seikaku stat bonuses */

	s16b a_dis;			/* seikaku disarming */
	s16b a_dev;			/* seikaku magic devices */
	s16b a_sav;			/* seikaku saving throw */
	s16b a_stl;			/* seikaku stealth */
	s16b a_srh;			/* seikaku search ability */
	s16b a_fos;			/* seikaku search frequency */
	s16b a_thn;			/* seikaku combat (normal) */
	s16b a_thb;			/* seikaku combat (shooting) */

	s16b a_mhp;			/* Race hit-dice modifier */

	byte no;			/* の */
	//v1.1.32 性別による性格制限を消した
	//byte sex;			/* seibetu seigen */

	bool flag_nofixed;

	//v2.0.6 あまり普通でない性格　尤魔がランダムに性格が変わるときの除外判定に使う
	bool flag_strange_select;

};


/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This structure contains several "blocks" of information.
 *   (1) the "permanent" info
 *   (2) the "variable" info
 *   (3) the "transient" info
 *
 * All of the "permanent" info, and most of the "variable" info,
 * is saved in the savefile.  The "transient" info is recomputed
 * whenever anything important changes.
 */

typedef struct player_type player_type;

struct player_type
{
	s16b oldpy;		/* Previous player location -KMW- */
	s16b oldpx;		/* Previous player location -KMW- */

	byte psex;			/* Sex index */
	byte prace;			/* Race index */
	byte pclass;		/* Class index */
	byte pseikaku;		/* Seikaku index */
	byte realm1;        /* First magic realm */
	byte realm2;        /* Second magic realm */

	//v1.1.13
	//oopsという未使用変数だったが、一部特技でONになる特殊なフラグを格納することにした。
	//reset_tim_flags()以外では自動解除されない
	//アイテムカードから使用するなど特定のクラスに限らない特技を実装したいときに
	byte special_flags;

	byte hitdie;		/* Hit dice (sides) */
	/*:::EXPペナルティ*/
	u16b expfact;       /* Experience factor
			     * Note: was byte, causing overflow for Amberite
			     * characters (such as Amberite Paladins)
			     */

	s16b age;			/* Characters age */ //←種族「動物霊」の所属勢力決定用乱数として使っている
	s16b ht;			/* Height */
	s16b wt;			/* Weight */
	s16b sc;			/* Social Class */ //←妖精用の特技保存領域にした。他種族使用不可



	s32b au;			/* Current Gold *//*:::所持金*/

	s32b max_max_exp;	/* Max max experience (only to calculate score) */
	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u32b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b lev;			/* Level */

	s16b town_num;			/* Current town number */
	/*:::アリーナのどこまで勝ち上がったか　負けたら負の値が入る*/
	//s16b arena_number;		/* monster number in arena -KMW- */
	s16b kourindou_number;	//v1.1.56 香霖堂での珍品蒐集のためarena_numberをこれに変えた
	bool inside_arena;		/* Is character inside arena? */
	s16b inside_quest;		/* Inside quest level */
	bool inside_battle;		/* Is character inside tougijou? */

	/*:::ワールドマップでの位置*/
	s32b wilderness_x;	/* Coordinates in the wilderness */
	s32b wilderness_y;
	bool wild_mode;		/*:::ワールドマップ時にTRUE*/

	s32b mhp;			/* Max hit pts */
	s32b chp;			/* Cur hit pts */

	u32b chp_frac;		/* Cur hit frac (times 2^16) */	//HPの端数。自然回復量の計算に使う

	s32b msp;			/* Max mana pts */
	s32b csp;			/* Cur mana pts */
	u32b csp_frac;		/* Cur mana frac (times 2^16) */

	s16b max_plv;		/* Max Player Level *//*:::現在レベルダウンしてても考慮しない最高地点レベル*/

	s16b stat_max[6];	/* Current "maximal" stat values */
		/*:::基本パラメータ最大値 18/70～18/130*/
	s16b stat_max_max[6];	/* Maximal "maximal" stat values */
	s16b stat_cur[6];	/* Current "natural" stat values */

	/*:::習得済み魔法の数　領域変更で減らない*/
	s16b learned_spells;
	/*:::追加魔法習得枠　呪文の巻物で増える*/
	s16b add_spells;

	u32b count;

	s16b fast;		/* Timed -- Fast */
	s16b slow;		/* Timed -- Slow */
	s16b blind;		/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;		/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;		/* Timed -- Cut *//*:::切り傷度*/
	s16b stun;		/* Timed -- Stun *//*:::朦朧度 100で意識不明瞭、50でひどく朦朧*/

	s16b protevil;		/* Timed -- Protection */
	s16b invuln;		/* Timed -- Invulnerable */
	s16b ult_res;		/* Timed -- Ultimate Resistance */
	s16b hero;		/* Timed -- Heroism */
	s16b shero;		/* Timed -- Super Heroism */
	s16b shield;		/* Timed -- Shield Spell */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- See Invisible */
	s16b tim_infra;		/* Timed -- Infra Vision */
	s16b tsuyoshi;		/* Timed -- Tsuyoshi Special */
	s16b ele_attack;	/* Timed -- Elemental Attack */
	s16b ele_immune;	/* Timed -- Elemental Immune */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */


	s16b tim_esp;       /* Timed ESP *//*:::一時テレパス*/
	s16b wraith_form;   /* Timed wraithform */

	s16b resist_magic;  /* Timed Resist Magic (later) */
	s16b tim_regen;
	s16b kabenuke;
	s16b tim_stealth;
	s16b tim_levitation;
	s16b tim_sh_touki;
	s16b lightspeed;
	s16b tim_sh_death;//死霊のオーラ
	/*:::magicdefとresist_magicの違いは？*/
	/*:::magicdef:魔法の鎧　魔法防御上昇+AC50+様々な耐性　魔力消去の対象*/
	/*:::resist_magic:練気術の魔法防御上昇 魔法防御上昇だけだが魔力消去の対象にはならない*/
	s16b magicdef;
	s16b tim_res_nether;	/* Timed -- Nether resistance */
	s16b tim_res_time;	/* Timed -- Time resistance */
	byte mimic_form;
	s16b tim_mimic;
	s16b tim_sh_fire;
	s16b tim_sh_holy;
	s16b tim_eyeeye;

	/* for mirror master */
	s16b tim_reflect;       /* Timed -- Reflect */
	s16b multishadow;       /* Timed -- Multi-shadow */
	s16b dustrobe;          /* Timed -- Robe of dust */

	//カオスパトロン　職業にかかわらずゲーム開始時にランダムに決まり、カオスパトロン変異を得ると表示される。
	///mod150103 種族野良神様のとき、クイックスタートのために最初の神格タイプをここに格納することにした。野良神様のときはカオスパトロン変異を得られない。
	s16b chaos_patron;
	u32b muta1;
	u32b muta2;
	u32b muta3;

	///mod140316 鬼の角や人魚の尻尾など消えない突然変異をここに記録するようにした
	u32b muta1_perma;
	u32b muta2_perma;
	u32b muta3_perma;

	//v1.1.64 追加
	u32b muta4;
	u32b muta4_perma;


	//この変数もう使わんから予備領域にしてしまいたいけどどこで何に使われてるか分からんから使いづらい
	s16b virtues[8];
	s16b vir_types[8];

	/*:::帰還の発動ターン数*/
	s16b word_recall;	  /* Word of recall counter */
	s16b alter_reality;	  /* Alter reality counter */
	/*:::地上から帰還するときの帰還先*/
	byte recall_dungeon;      /* Dungeon set to be recalled */

	/*:::＠の行動までに必要なエネルギー。時間停止中は-1000以下になる*/
	s16b energy_need;	  /* Energy needed for next move */
	/*:::process_upkeep_with_speed()で使われるエネルギー*/
	s16b enchant_energy_need;	  /* Energy needed for next upkeep effect	 */

	s16b food;		  /* Current nutrition */

	///mod140326 泥酔度
	s16b alcohol;

	u32b special_attack;	  /* Special attack capacity -LM- */
	/*:::剣術の型など*/
	u32b special_defense;	  /* Special block capacity -LM- */
	byte action;		  /* Currently action */

	/*:::習得した魔法のフラグ*/
	u32b spell_learned1;	  /* bit mask of spells learned */
	u32b spell_learned2;	  /* bit mask of spells learned */
	/*:::一度使用した魔法のフラグ　初回使用時の経験値獲得のためと思われる*/
	u32b spell_worked1;	  /* bit mask of spells tried and worked */
	u32b spell_worked2;	  /* bit mask of spells tried and worked */
	u32b spell_forgotten1;	  /* bit mask of spells learned but forgotten */
	u32b spell_forgotten2;	  /* bit mask of spells learned but forgotten */
	byte spell_order[64];	  /* order spells learned/remembered/forgotten */

	s16b spell_exp[64];       /* Proficiency of spells */
	///del131227　武器経験値はskill_expと統合したため削除
	//s16b weapon_exp[5][64];   /* Proficiency of weapons */
	///mod131226 skill skill_exp[]を武器技能値と統合するため10→21まで拡張
	//v1.1.32 基本的にgain_skill_exp()でしか変化しないがオキナパワーで限界を超えるルートもある
	s16b skill_exp[SKILL_EXP_MAX];       /* Proficiency of misc. skill */
	//s16b skill_exp[10];       /* Proficiency of misc. skill */

	/*:::職業ごと多目的変数として使われている*/
	//v1.1.94 magic_numのサイズを108→256に変更
	s32b magic_num1[MAGIC_NUM_SIZE];     /* Array for non-spellbook type magic */
	byte magic_num2[MAGIC_NUM_SIZE];     /* Flags for non-spellbook type magics */

	s16b mane_spell[MAX_MANE];
	s16b mane_dam[MAX_MANE];
	s16b mane_num;

	s16b concent;      /* Sniper's concentration level */

	s16b player_hp[PY_MAX_LEVEL];
	//v1.1.33b 80→256
	char died_from[256];   	  /* What killed the player */
	cptr last_message;        /* Last message on death or retirement */
	char history[4][60];  	  /* Textual "history" for the Player */

	u16b total_winner;	  /* Total winner */
	u16b panic_save;	  /* Panic save */

	/*:::チートフラグ？*/
	u16b noscore;		  /* Cheating flags */

	bool wait_report_score;   /* Waiting to report score */
	bool is_dead;		  /* Player is dead */

	bool wizard;		  /* Player is in wizard mode */

	s16b riding;              /* Riding on a monster of this index */
	byte knowledge;           /* Knowledge about yourself */
	s32b visit;               /* Visited towns */

	byte start_race;          /* Race at birth */
	s32b old_race1;           /* Record of race changes */
	s32b old_race2;           /* Record of race changes */
	/*:::過去使えた領域のビット毎フラグ　「あなたはかつて～領域を使えた」のダンプのためか*/
	s16b old_realm;           /* Record of realm changes */

	/*:::ペットへの命令による＠からの設定距離*/
	s16b pet_follow_distance; /* Length of the imaginary "leash" for pets */
	/*:::ペットへの命令 ビットごとのフラグで処理*/
	s16b pet_extra_flags;     /* Various flags for controling pets */

	///mod141231 日替わり賞金首だったが、霊夢の占い効果として再利用することにした
	s16b today_mon;           /* Wanted monster */

								/*:::トラップ感知済みエリアにいるかどうか*/
	bool dtrap;               /* Whether you are on trap-safe grids */
	s16b floor_id;            /* Current floor location */

	bool autopick_autoregister; /* auto register is in-use or not */

	byte feeling;		/* Most recent dungeon feeling *//*:::階の雰囲気*/
	s32b feeling_turn;	/* The turn of the last dungeon feeling *//*:::雰囲気感知用変数　この階に降りてきたturn数、前回雰囲気を感知したturn数が入っているはず*/


	/*** Temporary fields ***/

	bool playing;			/* True if player is playing */
	/*:::マップを出るとき？*/
	bool leaving;			/* True if player is leaving */

	//アリーナから外に出ていいフラグ　基本FALSEだがアリーナで勝負がついたらTRUEになる
	byte exit_bldg;			/* Goal obtained in arena? -KMW- */

	bool leaving_dungeon;	/* True if player is leaving the dungeon */
	/*:::街移動のとき*/
	bool teleport_town;
	bool enter_dungeon;     /* Just enter the dungeon */

	/*:::＠がターゲットにしててHPバーが出てるmonのID?*/
	s16b health_who;	/* Health bar trackee */

	/*:::＠がターゲットにしてるmonのID health_whoより長く保持されるらしい*/
	s16b monster_race_idx;	/* Monster race trackee */

	s16b object_kind_idx;	/* Object kind trackee */

	/*:::習得可能魔法数*/
	s16b new_spells;	/* Number of spells available */
	s16b old_spells;

	s16b old_food_aux;	/* Old value of food */

	/*:::鎧、グローブのMP減少がどう変化したか判定するための一時値*/
	bool old_cumber_armor;
	bool old_cumber_glove;

	bool old_heavy_wield[2];
	//bool old_heavy_shoot;
	bool old_icky_wield[2];
	bool old_riding_wield[2];
	bool old_riding_ryoute;
	bool old_monlite;


	s16b old_lite;		/* Old radius of lite (if any) */

	int heavy_cast; //cumber_armorから変更　コストが上昇するようにする
	//bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield[2];	/* Heavy weapon */
	//bool heavy_shoot;	/* Heavy shooter */
	byte shoot_penalty; //射撃ペナルティ
	byte old_shoot_penalty; //射撃ペナルティ
	bool icky_wield[2];	/* Icky weapon */
	bool riding_wield[2];	/* Riding weapon */
	bool riding_ryoute;	/* Riding weapon */
	/*:::モンスターによって照らされている状態　忍者の超隠密に関係*/
	bool monlite;

	s16b cur_lite;		/* Radius of lite (if any) */


	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	/*:::再描画の対象にするもの　ターゲットmonのHPバーなど様々なフラグが入る*/
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	s16b stat_use[6];	/* Current modified stats */
	s16b stat_top[6];	/* Maximal modified stats */

	///class 剣術家用変数
	/*:::剣術家　process_player()で＠の番が回ってきたらリセットされる*/
	bool sutemi;
	bool counter;

	///sysdel 善良度　削除予定
	s32b align;				/* Good/evil/neutral */
	s16b run_py;
	s16b run_px;


	/*** Extracted fields ***/

	u32b total_weight;	/* Total weight being carried */

	/*:::種族職業性格装備などによる加減算値*/
	s16b stat_add[6];	/* Modifiers to stat values */
	s16b stat_ind[6];	/* Indexes into stat tables */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */
	bool resist_pois;	/* Resist poison */

	bool resist_conf;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	bool resist_shard;	/* Resist shards */
	///del131228 因果混乱耐性削除
	//bool resist_nexus;	/* Resist nexus */
	bool resist_blind;	/* Resist blindness */
	bool resist_neth;	/* Resist nether */
	bool resist_fear;	/* Resist fear */
	bool resist_time;	/* Resist time */

	bool reflect;       /* Reflect 'bolt' attacks */
	bool sh_fire;       /* Fiery 'immolation' effect */
	bool sh_elec;       /* Electric 'immolation' effect */
	bool sh_cold;       /* Cold 'immolation' effect */

	bool anti_magic;    /* Anti-magic */
	bool anti_tele;     /* Prevent teleportation */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	u32b cursed;            /* Player is cursed */

	bool can_swim;		/* No damage falling */
	bool levitation;		/* No damage falling */
	bool lite;		/* Permanent light */
	bool free_act;		/* Never paralyzed */
	bool see_inv;		/* Can see invisible */
	bool regenerate;	/* Regenerate hit pts */
	//bool hold_life;		/* Resist life draining */

	bool telepathy;		/* Telepathy */
	bool esp_animal;
	bool esp_undead;
	bool esp_demon;
	//bool esp_orc;
	//bool esp_troll;
	//bool esp_giant;
	bool esp_dragon;
	bool esp_human;
	bool esp_evil;
	bool esp_good;
	bool esp_nonliving;
	bool esp_unique;

	bool slow_digest;	/* Slower digestion */
	bool bless_blade;	/* Blessed blade */
	bool xtra_might;	/* Extra might bow *//*:::強力射*/
	bool impact[2];		/* Earthquake blows */
	bool pass_wall;     /* Permanent wraithform */
	bool kill_wall;
	bool dec_mana;
	bool easy_spell;
	bool heavy_spell;
	bool warning;
	bool mighty_throw;
	bool see_nocto;		/* Noctovision *//*:::光源なしでダンジョンを見通す*/

	s16b to_dd[2]; /* Extra dice/sides */ /*:::今のところ騎兵のランス系のみ？*/
	s16b to_ds[2];

	s16b dis_to_h[2];	/* Known bonus to hit (wield) */
	s16b dis_to_h_b;	/* Known bonus to hit (bow) */
	s16b dis_to_d[2];	/* Known bonus to dam (wield) */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_h[2];			/* Bonus to hit (wield) */
	s16b to_h_b;			/* Bonus to hit (bow) */
	s16b to_h_m;			/* Bonus to hit (misc) */
	s16b to_d[2];			/* Bonus to dam (wield) */
	s16b to_d_m;			/* Bonus to dam (misc) */
	s16b to_a;			/* Bonus to ac */

	/*:::魔法難易度*/
	s16b to_m_chance;		/* Minusses to cast chance */

	/*:::一本の武器を両手持ちしているときTRUE　ただし両手が空で格闘するときcalc_bonuses()内で一時的にTRUEになる*/
	bool ryoute;
	/*:::右手で攻撃するときTRUE　武器、素手含む*/
	bool migite;
	/*:::左手で攻撃するときTRUE　武器、素手含む*/
	bool hidarite;
	bool no_flowed;/*:::モンスターから匂いによって追跡されないフラグ？*/

	s16b ac;			/* Base ac */

	s16b see_infra;		/* Infravision range */

	s16b skill_dis;		/* Skill: Disarming */
	s16b skill_dev;		/* Skill: Magic Devices *//*:::魔導具使用*/
	s16b skill_sav;		/* Skill: Saving throw */ /*:::魔法防御 普通は100行かない程度*/
	s16b skill_stl;		/* Skill: Stealth factor */
	s16b skill_srh;		/* Skill: Searching ability */
	/*:::知覚能力　高いと移動時常にserch()処理をする？*/
	s16b skill_fos;		/* Skill: Searching frequency */ //いずれここをグレイズ技能に変更する予定
	s16b skill_thn;		/* Skill: To hit (normal) */
	s16b skill_thb;		/* Skill: To hit (shooting) */
	s16b skill_tht;		/* Skill: To hit (throwing) */ /*:::投擲技能　ほぼ射撃と同じ値*/
	s16b skill_dig;		/* Skill: Digging */

	s16b num_blow[2];	/* Number of blows */
	/*:::射撃回数*100*/
	s16b num_fire;		/* Number of shots */

	byte tval_xtra;		/* Correct xtra tval */

	/*:::現在装備している弓枠に適合する矢玉の種類*/
	byte tval_ammo;		/* Correct ammo tval */


	///mod131228 新しい耐性や一時効果を追加　セーブ/ロードはwr/rd_extraの"Future use"の近くに
	/*:::一時効果以外はセーブ・ロードする必要がない。ロード時のcalc_bonuses()で種族職業装備から計算されるため。*/
	bool resist_holy;
	bool resist_water;
	s16b tim_res_water;
	bool resist_insanity;
	bool speedster;
	bool esp_deity;
	bool esp_kwai;

	s16b nennbaku;//死霊領域の念縛

	s16b tim_res_chaos;
	s16b tim_superstealth;
	s16b tim_res_dark;
	s16b tim_unite_darkness;

	///mod140222 何か一時効果を入れたいとき用に追加
	/*:::職業ごと汎用カウント reset_tim_flags()でリセット　基本的に魔力消去対象にならない*/
	s16b tim_general[TIM_GENERAL_MAX];

	///mod140324 一時能力増加用カウント、上昇値 (上昇値が100を超えると(上昇値-100)になるが40の限界を超えるようにする)
	s16b tim_addstat_count[6];
	s16b tim_addstat_num[6];
	//生命領域の反応回復
	s16b reactive_heal;

	u32b bydo;//バイド細胞侵食 MUT3_BYDOがあるとカウントが増え、３日後ゲームオーバー
	s16b radar_sense;//変容領域　レーダーセンス

	u32b grassroots; //草の根ネットワーク加入判定 兼 アイテム交換を開始したturn

	//草の根ネットワークで交換中のアイテムの価格
	///mod141231 霊夢の占いの特殊変数としても使うことにした
	s32b barter_value;

	bool drowning; //溺れている状態

	///mod140406 弓の両手装備とか全職格闘導入とかで格闘判定がややこしくなったのでフラグ化した。
	bool do_martialarts;

	s16b silent_floor;//オープンパンドラの「敵を黙らせる」効果
	s32b maripo;
	u32b maripo_presented_flag;

	///mod140502
	s16b lucky;//予見領域「幸運のメカニズム」
	s16b foresight;//予見領域「未来予知」
	s16b deportation;//召喚領域「送還術」
	s16b tim_res_insanity;

	s16b tim_speedster;
	s16b clawextend;//変容領域　爪強化

	/*:::破邪領域の神降ろし　フラグとカウント*/
	u32b kamioroshi;
	s16b kamioroshi_count;


	byte pspeed;		/* Current speed */

	//↓追加予定の特殊効果の変数 saveとloadには反映済み

	s16b tim_res_holy;
	/*:::変容領域のドラゴン変化詠唱時のレベル+知能+賢さ　最大150 変容ハイは+20 ドラゴンのパラメータに影響 */
	s16b mimic_dragon_rank;

	/*:::喘息度*/
	s16b asthma;


	/*:::種族関係の多目的変数 この値はクイックスタートに保存されないので保存したければ他に記録する必要がある*/
	//野良神様の属性値、妖精の特技、ゴーレム/妖怪人形/埴輪の破損タイムカウント
	//動物霊の所属勢力
	s16b race_multipur_val[5];

	s16b monkey_count ;//秘術魔法「猿の手」使用回数カウント

	//v1.1.56 アイテム「スペルカード」による一時効果をカウントする領域
	s16b tim_spellcard_count;	//v1.1.56 スペルカードのタイムカウンタ
	s16b spellcard_effect_idx;	//v1.1.56 スペルカードの発動IDX

	//モンスター変身中のr_idx
	s16b metamor_r_idx;

	//アビリティカード価格の高騰の度合い 初期10,最終的に300～1000くらいで乱高下？
	s16b abilitycard_price_rate;

	//v1.1.88 発動中の一時超隠密のタイプ(普通、光学迷彩、森隠れなど)を記録する
	s16b superstealth_type;

	//v1.1.93 一時反感
	s16b tim_aggravation;

	//v2.0.1 赤蛮奇のアイテム探索特技(アビリティカードでも使う)
	s16b tim_rokuro_head_search;
	//v2.0.1 アビリティカードのひらり布の有効時間(正邪の特技のひらり布ではない)
	s16b tim_hirari_nuno;

	//v2.0.11 移動禁止のバッドステータス
	s16b tim_no_move;
	//v2.0.11 移送の罠
	s16b transportation_trap;

	u32b animal_ghost_align_flag;	//v1.1.91
	u32b quest_special_flag;		//v1.1.98 一部クエストをどんな状態で進行したか記録する
	u32b cooking_material_flag;		//v2.0.8 鯢呑亭に持ち込める料理素材モンスターの素材所持フラグ 各ビットがmonster_food_list[]のインデックス
	u32b ptype_new_flags4;

	//v2.0.5 はたて建物効果 future_use_s16bから2つ使う
	s16b hatate_mon_search_ridx;
	s16b hatate_mon_search_dungeon;


	s16b future_use_s16b[14];
	s32b future_use_s32b[8];


};


/*
 * A structure to hold "rolled" information
 */
typedef struct birther birther;

/*:::クイックスタート用に保持されるキャラクターメイキングデータ*/
struct birther
{
	byte psex;         /* Sex index */
	byte prace;        /* Race index */
	byte pclass;       /* Class index */
	byte pseikaku;     /* Seikaku index */
	byte realm1;       /* First magic realm */
	byte realm2;       /* Second magic realm */

	s16b age;
	s16b ht;
	s16b wt;

	/*:::社会的地位　チートオプションやEASYでコンティニューしたとき0になる*/
	///mod150315 ここの値を妖精の特殊能力用に保持することにした
	s16b sc;

	s32b au;

	s16b stat_max[6];	/* Current "maximal" stat values */
	/*:::基本パラメータ最大値 18/70～18/130*/
	s16b stat_max_max[6];	/* Maximal "maximal" stat values */
	s16b player_hp[PY_MAX_LEVEL];

	s16b chaos_patron;

	s16b vir_types[8];

	char history[4][60];

	///mod140307 難易度変更
	byte difficulty;

	bool quick_ok;
};


/* For Monk martial arts */
#if 0
typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr    desc;       /* A verbose attack description */
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;         /* Damage dice */
	int     ds;         /* Damage sides */
	int     effect;     /* Special effects */
};
#endif

typedef struct martial_arts_new martial_arts_new;

struct martial_arts_new
{
	cptr    desc;       /* A verbose attack description */
	int		method;		/*:::攻撃の分類　事前に決められた分類の技の中からしか選ばれない*/
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;         /* Damage dice */
	int     ds;         /* Damage sides */
	int     effect;     /* Special effects */
	int		weight;     /*:::重量 ここの値に@のレベルを掛けた値がクリティカル判定に使われる　2～20程度*/
};




typedef struct kamae kamae;

struct kamae
{
	cptr    desc;       /* A verbose kamae description */
	int     min_level;  /* Minimum level to use */
	cptr    info;
};

/* Mindcrafters */
typedef struct mind_type mind_type;
struct mind_type
{
	int     min_lev;
	int     mana_cost;
	int     fail;
	cptr    name;
};

typedef struct mind_power mind_power;
struct mind_power
{
	mind_type info[MAX_MIND_POWERS];
};

///mod140202
/*:::今後新しく追加する職業ごとの技はレイシャル含めて全てこれを使う予定。*/
/*:::「技」は英語でpowerなのかskillなのかtechniqueなのかartsなのかよく分からない。*/
typedef struct class_power_type class_power_type;
struct class_power_type
{
	int min_lev; //使用可能レベル　配列の最後にはここを99にした空エントリを入れる
	int cost;
	int fail;
	bool use_hp; //HPを消費するときTRUE
	bool is_magic; //魔法の時TRUE 魔法難度減少などの効果を受けるが反魔法のとき使えなくなる
	byte stat; //失敗率の参照になるパラメータ A_STR～A_CHR
	byte heavy_armor_fail; //装備が重い時の失敗率加算
	byte lev_inc_cost; //レベル上昇ごとに増えるコスト*10　レベル上昇で威力が跳ね上がる特技などに
	cptr name;
	cptr tips; //bコマンドで読める説明文
};




/* Imitator */

typedef struct monster_power monster_power;
struct monster_power
{
	int     level;
	int     smana;
	int     fail;
	int     manedam;
	int     manefail;
	int     use_stat;
	cptr    name;
};


/*
 * A structure to describe a building.
 * From Kamband
 */
 /*:::大改造予定*/
typedef struct building_type building_type;

struct building_type
{
	///mod140821 文字数14までいけるようにした
	char name[45];                  /* proprietor name */
	char owner_name[30];            /* proprietor name */
	char owner_race[30];            /* proprietor race */

	char act_names[8][30];          /* action names */
	s32b member_costs[8];           /* Costs for class members of building */
	s32b other_costs[8];		    /* Costs for nonguild members ←現在全てのプレイヤーがis_owner()を通るためここの価格は使われないはず*/
	char letters[8];                /* action letters */
	s16b actions[8];                /* action codes */
	s16b action_restr[8];           /* action restrictions */

	s16b member_class[MAX_CLASS];   /* which classes are part of guild */
	s16b member_race[MAX_RACES];    /* which classes are part of guild */
	s16b member_realm[MAX_MAGIC+1]; /* which realms are part of guild */
};


/* Border */
typedef struct border_type border_type;
struct border_type
{
	s16b north[MAX_WID];
	s16b south[MAX_WID];
	s16b east[MAX_HGT];
	s16b west[MAX_HGT];
	s16b north_west;
	s16b north_east;
	s16b south_west;
	s16b south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain or a town
 */
typedef struct wilderness_type wilderness_type;
struct wilderness_type
{
	int         terrain;
	int         town;	/*:::街番号*/
	int         road;
	u32b        seed;	/*:::荒野の地形をランダム生成するときのための乱数の種？*/
	s16b        level;
	byte        entrance; /*:::ダンジョンへの階段があるかどうか。あるならそのダンジョンのインデックスを格納*/
};


/*
 * A structure describing a town with
 * stores and buildings
 */
typedef struct town_type town_type;
struct town_type
{
	char        name[32];
	u32b        seed;      /* Seed for RNG */
	store_type	*store;    /* The stores [MAX_STORES] */
	byte        numstores;
};

/* Dungeons */
typedef struct dun_type dun_type;
struct dun_type
{
	byte min_level; /* Minimum level in the dungeon */
	byte max_level; /* Maximum dungeon level allowed */

	cptr name;      /* The name of the dungeon */
};

/*
 * Sort-array element
 */
typedef struct tag_type tag_type;

struct tag_type
{
	int     tag;
	int     index;
};

typedef bool (*monster_hook_type)(int r_idx);


/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func)(object_type *);


/*
 * Semi-Portable High Score List Entry (128 bytes) -- BEN
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */

typedef struct high_score high_score;

struct high_score
{
	char what[8];		/* Version info (string) */

	char pts[10];		/* Total Score (number) */

	char gold[10];		/* Total Gold (number) */

	char turns[10];		/* Turns Taken (number) */

	char day[10];		/* Time stamp (string) */

	char who[16];		/* Player Name (string) */

	char uid[8];		/* Player UID (number) */

	char sex[2];		/* Player Sex (string) */
	char p_r[3];		/* Player Race (number) */
	char p_c[3];		/* Player Class (number) */
	char p_a[3];		/* Player Seikaku (number) */

	char cur_lev[4];		/* Current Player Level (number) */
	char cur_dun[4];		/* Current Dungeon Level (number) */
	char max_lev[4];		/* Max Player Level (number) */
	char max_dun[4];		/* Max Dungeon Level (number) */

	char how[40];		/* Method of death (string) */
};


typedef struct
{
	s16b feat;    /* Feature tile */
	byte percent; /* Chance of type */
}
feat_prob;


/* A structure for the != dungeon types */
typedef struct dungeon_info_type dungeon_info_type;
struct dungeon_info_type {
	u32b name;		/* Name */
	u32b text;		/* Description */

	byte dy;
	byte dx;

	/*:::出やすい床と壁を3つまで出現率と一緒に登録しておけるらしい*/
	feat_prob floor[DUNGEON_FEAT_PROB_NUM]; /* Floor probability */
	feat_prob fill[DUNGEON_FEAT_PROB_NUM];  /* Cave wall probability */

	s16b outer_wall;                        /* Outer wall tile */
	s16b inner_wall;                        /* Inner wall tile */
	s16b stream1;                           /* stream tile */
	s16b stream2;                           /* stream tile */

	s16b mindepth;         /* Minimal depth */
	s16b maxdepth;         /* Maximal depth */
	byte min_plev;         /* Minimal plev needed to enter -- it's an anti-cheating mesure */
	s16b pit;
	s16b nest;
	byte mode;		/* Mode of combinaison of the monster flags */

	int min_m_alloc_level;	/* Minimal number of monsters per level */
	/*:::ダンジョンごとのモンスターpop率*/
	int max_m_alloc_chance;	/* There is a 1/max_m_alloc_chance chance per round of creating a new monster */

	/*:::ダンジョンフラグ*/
	u32b flags1;		/* Flags 1 */

	/*:::ダンジョンで出現するモンスターのフラグ これをどのように適用するかはmodeで決定　これが適用されるかどうかはMONSTER_DIV_?による*/
	u32b mflags1;		/* The monster flags that are allowed */
	u32b mflags2;
	u32b mflags3;
	u32b mflags4;
	u32b mflags5;
	u32b mflags6;
	u32b mflags7;
	u32b mflags8;
	u32b mflags9;
	u32b mflagsr;

	//v1.1.86 配列サイズを5→50へ
	char r_char[DUNGEON_R_CHAR_MAX];		/* Monster race allowed */
	int final_object;	/* The object you'll find at the bottom */
	int final_artifact;	/* The artifact you'll find at the bottom */
	/*:::ダンジョンの主のモンスターIDが入っているらしい*/
	int final_guardian;	/* The artifact's guardian. If an artifact is specified, then it's NEEDED */

	/*:::数値が高いほど出てくる敵にばらつきがない？get_mon_num_prep()参照 64以上だとモンスターフラグによる制限が働かないらしい*/
	byte special_div;	/* % of monsters affected by the flags/races allowed, to add some variety */
	/*:::フロア生成時に部屋をつなぐ関数がbuild_tunnel()でなくbuild_tunnel2()になる確率　詳細不明*/
	int tunnel_percent;
	/*:::apply_magic()で使われる上質、高級品判定値の上限*/
	int obj_great;
	int obj_good;
};


/*
 *  A structure type for entry of auto-picker/destroyer
 */
typedef struct {
	/*:::アイテム名 部分一致も可*/
	cptr name;          /* Items which have 'name' as part of its name match */
	/*:::処理したアイテムにどんな銘を刻むか*/
	cptr insc;          /* Items will be auto-inscribed as 'insc' */
	/*:::未判明、レア、クローク、一冊目など条件判定にかかわるフラグ全て*/
	u32b flag[2];       /* Misc. keyword to be matched */
	/*:::拾うか壊すか放置するか、全体マップに表示するかなどのフラグ*/
	byte action;        /* Auto-pickup or Destroy or Leave items */
	/*:::武器のダイス目指定　デフォは0*/
	byte dice;          /* Weapons which have more than 'dice' dice match */
	/*:::武器防具のボーナス指定?*/
	byte bonus;         /* Items which have more than 'bonus' magical bonus match */
} autopick_type;


/*
 *  A structure type for the saved floor
 */
typedef struct
{
	/*:::p_ptr->floor_idにはここの値が入る*/
	s16b floor_id;        /* No recycle until 65536 IDs are all used */

	byte savefile_id;     /* ID for savefile (from 0 to MAX_SAVED_FLOOR) */
	s16b dun_level;
	s32b last_visit;      /* Time count of last visit. 0 for new floor. */
	u32b visit_mark;      /* Older has always smaller mark. */
	s16b upper_floor_id;  /* a floor connected with level teleportation */
	s16b lower_floor_id;  /* a floor connected with level tel. and trap door */
} saved_floor_type;


/*
 *  A structure type for terrain template of saving dungeon floor
 */
typedef struct
{
	u16b info;
	s16b feat;
	s16b mimic;
	s16b special;
	u16b occurrence;
	//v1.1.33 洞窟用フラグ追加
	u32b cave_xtra_flag;
	s16b cave_xtra1;
	u16b cave_xtra2;

} cave_template_type;


/*
 * A structure type for arena entry
 */
typedef struct
{
	s16b r_idx; /* Monster (0 means victory prizing) */
	byte tval;  /* tval of prize (0 means no prize) */
	byte sval;  /* sval of prize */
} arena_type;


/*
 * A structure type for doors
 */
typedef struct
{
	s16b open;
	s16b broken;
	s16b closed;
	s16b locked[MAX_LJ_DOORS];
	s16b num_locked;
	s16b jammed[MAX_LJ_DOORS];
	s16b num_jammed;
} door_type;


#ifdef TRAVEL
/*
 *  A structure type for travel command
 */
typedef struct {
	int run; /* Remaining grid number */
	int cost[MAX_HGT][MAX_WID];
	int x; /* Target X */
	int y; /* Target Y */
	int dir; /* Running direction */
} travel_type;
#endif

typedef struct {
	cptr flag;
	byte index;
	byte level;
	s32b value;
	struct {
		int constant;
		int dice;
	} timeout;
	cptr desc;
} activation_type;

typedef struct {
	int flag;
	int type;
	cptr name;
} dragonbreath_type;


///mod140306 種族、職業、性別ごとのゲーム開始時所持アイテム
typedef struct outfit_type outfit_type;

struct outfit_type
{
	int type; //種族、職業ID
	int psex; //性別 0:女性限定 1:男性限定 それ以外：男女共通
	int artifact_idx; //固定アーティファクトを持って開始する場合a_infoのidx 通常0 IDXが入る場合tval,sval,numは無視される
	int tval; //アイテムのtval
	int sval; //アイテムのsval
	int num;  //アイテムの個数
};


/*:::ダンジョン特産品テーブル*/
typedef struct souvenir_type souvenir_type;
struct souvenir_type
{
	int dun_num;//発見ダンジョン
	int tval;
	int sval;
	int min_lev;//発見最低階層
	int max_lev;//発見最大階層
	int chance;//基本発見率 0.01%単位
	int max_num;//発見されたとき一か所で見つかる最大数
};


typedef struct marisa_store_type marisa_store_type;
struct marisa_store_type
{
	int tval;
	int sval;
	int maripo;
};

///mod140429 アーチャーギルド用
typedef struct guild_arts_type guild_arts_type;
struct guild_arts_type
{
	int tval;
	int sval;
	int type;
	int num;
	cptr name;
};

///mod140819 エンジニア関係
/*:::機械仕様テーブル　添え字がSVALに一致する*/
typedef struct machine_type machine_type;
struct machine_type
{
	int difficulty;//作成難度（レベル+知能+器用)
	int tval; //燃料tval(0は使い捨て)
	int sval; //燃料sval
	int charge;//基本使用回数　初期pvalに等しく使用するたびに1減る　燃料補給するとこの値に戻る
	int hate_water;//水攻撃を受けたとき使用不能になる確率 0～100 ダメージで確率増減
	int hate_elec;//電撃攻撃を受けたとき使用不能になる確率
};


/*:::青魔、ものまね系用モンスター魔法新リストのための型*/
/*:::これらのレベルや難易度などの各項目は職によっては無視される*/
typedef struct monspells2 monspells2;
struct monspells2
{
	int     level; //基本使用可能レベル
	int     smana; //基本消費MP
	int     fail; //基本難易度
	int     use_stat; //難易度に関係するステータス
	bool	use_words; //響子のオウム返しが可能かどうか　呪文のほか叫びや轟音ブレスも含まれる
	bool	attack_spell; //攻撃魔法　紫専用フラグ
	int		priority; //モンスター変身のとき技の多いモンスターは全部リストに載せられないので要らなそうなのを削るための優先順位　少ないほど消されにくいが0は載らない
	cptr    name; //呪文名
	//v1.1.57追加
	byte monspell_type; //魔法をボルト・ボール・ブレスなどの分類ごとに選択するための分類値 NEW_MSPELL_TYPE_***に対応 未分類は0

};

/*:::野良神様用*/
typedef struct deity_type deity_type;
struct deity_type
{
	s16b	rank;	//0:終端用ダミー 1:初期 2:Lev30 3:Lev45
	cptr	deity_name;	//神様の種類名
	s16b	dbias_cosmos; //秩序度　0-255
	s16b	dbias_warlike; //好戦度
	s16b	dbias_reputation; //名声値　最初はみな低い
	s16b	stat_adj[6];	//能力修正値
	s16b	r_dis;
	s16b	r_dev;
	s16b	r_sav;
	s16b	r_stl;
	s16b	r_srh;
	s16b	r_graze;
	s16b	r_thn;
	s16b	r_thb;

};

///mod140225 説明文のcptr infoを増やした
///mod150208 racial.cからここに持ってきた
typedef struct power_desc_type power_desc_type;

struct power_desc_type
{
	char name[40];
	int  level;
	int  cost;
	int  stat;
	int  fail;
	int  number;
	cptr info;
};


/*:::妖精用*/
typedef struct fairy_type fairy_type;
struct fairy_type
{
	byte	rarity;	//0-99 妖精タイプ選定時、randint0(100)がrarityを下回ったら再抽選される。だからこの数値がそのまま確率になるわけではない。
	cptr	fairy_desc;//○○する程度の能力
};

/*:::新闘技場専用特殊チームリスト*/
typedef struct battle_mon_special_team_type battle_mon_special_team_type;
struct battle_mon_special_team_type
{
	int level;	 //最低生成レベル
	cptr 	name; //チーム名
	int  r_idx_list[8];
};

/*:::新闘技場専用モンスターリスト*/
typedef struct battle_mon_type battle_mon_type;
struct battle_mon_type
{
	int     r_idx; //モンスター名 0はダミー
	u32b     team; //チーム MFLAG_BATTLE_MON_TEAM_A～D
};


///mod150811 魔理沙
struct marisa_magic_type
{
	cptr	name; //名前
	cptr	info; //説明文
	s16b	use_magic_power[8]; //調合に必要な魔力の種類と量
	bool	is_drug; //薬調合のときTRUE メッセージに影響

};


/*:::調剤素材テーブル*/
typedef struct drug_material_type drug_material_type;
struct drug_material_type
{
	byte	tval; //素材のtvalとsval
	byte	sval;

	int	cost; //調剤室を借りるのに必要な金額

	//素材の持つ属性値
	int	elem[8];
};


/*:::調剤テーブル*/
typedef struct drug_compound_type drug_compound_type;
struct drug_compound_type
{
	byte	tval; //生成される品のtvalとsval
	byte	sval;

	//特定のSVALの本を所有していると難易度減少する？
	byte	difficulty;

	//この薬が生成されるための素材合計必要属性値　
	//(負のときは数値以下の値になる必要がある。属性値を満たす者が複数ある場合は特定必要素材の設定されているもの、表の下にある物が優先される)
	int	elem[8];

	//　材料コストがこのコストを大きく超えると複数生成されることがある。
	int		base_cost;
	//上の材料コストを超過した時の最大生成数
	int		max_num;

	//特定アイテムはこの素材が含まれないといけない?
	byte	special_material_tval;
	byte	special_material_sval;
};

/*:::秘伝書テーブル*/
typedef struct drug_recipe_type drug_recipe_type;
struct drug_recipe_type
{
	byte	tval; //完成品のtvalとsval
	byte	sval;

	//素材1～3として表示されるtv,sv
	byte	mat1_tv;
	byte	mat1_sv;
	byte	mat2_tv;
	byte	mat2_sv;
	byte	mat3_tv;
	byte	mat3_sv;

};



/*:::EXTRAモード用サポートアイテムリスト*/
typedef struct support_item_type support_item_type;
struct support_item_type
{
	byte	rarity;		//少ないほど出やすい 0のとき終端ダミー
	byte	min_lev; 	//基本生成開始階　ここより前だと生成率が落ちる
	byte	max_lev; 	//基本生成終了階　ここより後だと生成率が落ちる
	byte	prod_num;//基礎生成枚数　1のものは常に1
	byte	cost; //アイテム交換のときの価値

	s16b	r_idx;	 	//このidxの敵を倒したとき入手しやすい 0のとき無視
	class_power_type *func_table; //このアイテムが参照するユニーククラス特技テーブルのアドレス　不要かもしれんけど一応入れとく
	cptr	(*func_aux)(int,bool); //このアイテムで使用するユニーククラス特技実行関数へのポインタ　文法これで合ってるのか自信ない
	byte	func_num;	//func_tableの要素番号　ユニーククラスの特技変更時に順番が替わったらここの番号を変え忘れないよう注意しないと最悪バグる。
	cptr	name; 		//アイテム名　object_descから参照する。
	cptr	infotxt;//説明文
};

/*:::エクストラクエスト報酬テーブル*/
typedef struct ex_quest_rew_type ex_quest_rew_type;
struct ex_quest_rew_type
{
	byte	level; //クエストフロア階 0は終端ダミー
	byte	a_idx; //★のときのインデックス
	byte	tval; //報酬のtv,sv
	byte	sval;
	u32b	am_mode; //apply_magic()をするときのモード
	byte 	num;	//個数


};

/*:::銃基礎パラメータテーブル*/
//配列添字が銃器svalに対応
//ACT_GUN_VARIABLEによる射撃にのみ使う。
typedef struct gun_base_param_type gun_base_param_type;
struct gun_base_param_type
{

	byte	gf_type;//射撃したときの基本属性 object_prep()を通った時xtra1に格納される

	//射撃したときの攻撃分類 GUN_FIRE_MODE_*に対応 xtra4に格納
	byte	project_type;

	//基礎充填ターン数
	s16b	charge_turn;

	//装弾数
	byte bullets;

};


/*:::秘術「異界との接触」のモンスターテーブル*/
typedef struct occult_contact_type occult_contact_type;
struct occult_contact_type
{
	byte	featflag1; //地形FF_*に一致
	byte	featflag2; //地形FF_*に一致
	byte	featflag3; //地形FF_*に一致しない
	s16b	r_idx;	//召喚される敵
	cptr	feat_desc; //地形表記

};

//v1.1.17 純狐でessence_infoを使うためにobject2.cから移動
/*
 *  A structure for smithing
 */
typedef struct essence_type essence_type;
struct essence_type
{
	int add;       /* TR flag number or special essence id */
	cptr add_name; /* Name of this ability */
	int type;      /* Menu number */
	int essence;   /* Index for carrying essences */
	int value;     /* Needed value to add this ability */
};

/*:::兵士用特技リスト*/
typedef struct soldier_skill_type soldier_skill_type;
struct soldier_skill_type
{
	byte	lev;	//習得可能レベル？不要かも
	cptr	name;	//特技名
	cptr	info;	//説明文　\で改行

};


//v1.1.22
//兵士の弾丸換装テーブル用
//配列添字はp_ptr->magic_num1[]の添字とSSM_***に一致し、残弾数の管理に使われる。
typedef struct soldier_bullet_type soldier_bullet_type;
struct soldier_bullet_type
{
	int 	cost;		//この弾を射撃したときの消費MP
	cptr	name;		//銃弾名
	cptr	info; 		//説明文 \\で改行
	byte	magic_bullet_lev;	//「銃弾変更」で使うとき、どの技能レベルから使えるか

};

//v1.1.22
//兵士の弾丸加工用
typedef struct soldier_bullet_making_type soldier_bullet_making_type;
struct soldier_bullet_making_type
{
	byte	shooting_mode;	//SSM_***に一致 0は終端ダミー
	byte	mat_tval; 	//この弾を作る材料のtv,sv
	byte	mat_sval;
	byte	prod_num; 	//素材一つ当たりの基本生産数

};


//v1.1.24
//急流下りクエスト順位決定用のスコアリスト
typedef struct torrent_quest_score_type torrent_quest_score_type;
struct torrent_quest_score_type
{
	int		turn;
	s16b	class_id;
	s16b	class_id2;
	cptr	name;

};


//v1.1.51	「夢の世界に行く(仮)」コマンドで見る夢の選択肢
typedef struct nightmare_stage_type nightmare_stage_type;
struct nightmare_stage_type
{
	cptr desc;			//ステージ名
	u32b need_points;	//このステージが選択肢に表示されるために必要な合計ポイント数
	byte level;			//出現モンスターの基本レベル
	u32b min_points;	//このステージで得られるポイントがmin/maxから外れたらモンスター選定やり直し
	u32b max_points;

};

//v1.1.51	「夢の世界に行く(仮)」コマンドで合成スペカ名に使われる文字列
typedef struct nightmare_spellname_type nightmare_spellname_type;
struct nightmare_spellname_type
{
	int r_idx;		//登録されたr_idxがこれと一致する文字列が選ばれる。0が入った終端ダミーが必要
	int class_idx;	//＠がユニーククラスで夢の世界の＠(ランダムユニーク)を倒したときこのidxに合うものが選ばれる
	cptr conj1;	//前につく接続詞 空ならNULL
	cptr phrase;	//文字列本体
	cptr conj2;	//後につく接続詞　空ならNULL
	byte position;	//この数字が大きいほど前に来る。完成したスペカ名はこの係数が10未満の文字列を少なくとも一つ含まないといけない

};



//v1.1.51	新アリーナの地形生成に使う街ファイル名と流用開始位置xy座標
typedef struct nightmare_gen_map_type nightmare_gen_map_type;
struct nightmare_gen_map_type
{
	cptr	townfilename; //街ファイル名 t0000001.txt　など
	byte	x;	//流用開始するポイント(左上)
	byte	y;

};

//v1.1.51 新アリーナ用賞品テーブル
typedef struct
{
	byte nightmare_level; //0～7
	byte rarity;//この値で出現率を比例配分する。大きいほど出やすい。配列の最後にrarity=0の終端ダミーを入れる
	byte tval;
	byte sval;
	byte num; //基本生成個数
} nightmare_treasure_type;


//v1.1.57	隠岐奈「秘神の暗躍弾幕」の攻撃タイプ
typedef struct
{
	byte lev;	//選択可能レベル　この構造体はレベル順に並んでいること
	cptr desc;	//攻撃対象分類名
	u32b rf3_flag;  //攻撃対象条件(r_ptr->flags3と照会する)
}pattern_attack_type;


//v1.1.86 アビリティカード情報テーブル
typedef struct
{
	cptr	card_name; 	//カードの名前
	u16b	rarity_rank;	//出やすさ0～4　0は通常出現しない。ランダム生成時には(1:80% 2:16% 3: 3.2% 4:0.8% weird_luck()でランクアップ)で選定される
	u16b	trade_value;	//交換価値　トレードのときには大体このカードよりちょっと低いものが出る。EXTRAではアイテムカードとのトレードにも使う?
	bool	activate; 	//発動可能なカードのときTRUE
	byte	difficulty;	//魔道具としての発動難易度
	u16b	charge_turn;	//チャージ時間 最大300まで(99枚スタックしたときにobject_type.timeout値が32767以内でないとオーバーフローする)
	u16b	r_idx;		//そのカードを持っているモンスター

} ability_card_type;

//v2.0.8 鯢呑亭に持ち込める食物モンスターリスト
typedef struct
{
	int r_idx;  	//モンスターidx
	int food_amount;//満腹度上昇
	cptr desc;	//料理名称

}monster_food_type;





