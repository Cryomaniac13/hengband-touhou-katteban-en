
/* File: misc.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: misc code */

#include "angband.h"




/*
 * Converts stat num into a six-char (right justified) string
 */
/*:::intのステータス値を表示用に計算し直し文字列として返す。168→18/150など*/


///mod140104 パラメータ上限と表記を変更
void cnv_stat(int val, char *out_val)
{

	if(val > STAT_ADD_MAX)
			sprintf(out_val, "   !%2d", STAT_ADD_MAX);
	else
			sprintf(out_val, "    %2d", val);

#if 0
	/* Above 18 */
	if (val > 18)
	{
		int bonus = (val - 18);

		if (bonus >= 220)
		{
			sprintf(out_val, "18/%3s", "***");
		}
		else if (bonus >= 100)
		{
			sprintf(out_val, "18/%03d", bonus);
		}
		else
		{
			sprintf(out_val, " 18/%02d", bonus);
		}
	}

	/* From 3 to 18 */
	else
	{
		sprintf(out_val, "    %2d", val);
	}
#endif
}



/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
/*:::ステータスの増減を計算する。valueが元値でamountが増減値*/
/*:::内部的には18/10は18+10,18/108は18+108で保持されているらしい*/

///mod140104 パラメータ上限と表記を変更
s16b modify_stat_value(int value, int amount)
{
	int    i;

	value += amount;
	if(value < 3) value = 3;
	return (value);
#if 0
	/* Reward */
	if (amount > 0)
	{
		/* Apply each point */
		for (i = 0; i < amount; i++)
		{
			/* One point at a time */
			if (value < 18) value++;

			/* Ten "points" at a time */
			else value += 10;
		}
	}

	/* Penalty */
	else if (amount < 0)
	{
		/* Apply each point */
		for (i = 0; i < (0 - amount); i++)
		{
			/* Ten points at a time */
			if (value >= 18+10) value -= 10;

			/* Hack -- prevent weirdness */
			else if (value > 18) value = 18;

			/* One point at a time */
			else if (value > 3) value--;
		}
	}

	/* Return new value */
	return (value);
#endif
}



/*
 * Print character info at given row, column in a 13 char field
 */
static void prt_field(cptr info, int row, int col)
{
	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", row, col);

	/* Dump the info itself */
	c_put_str(TERM_L_BLUE, info, row, col);
}


/*
 *  Whether daytime or not
 */
bool is_daytime(void)
{
	s32b len = TURNS_PER_TICK * TOWN_DAWN;
	if ((turn % len) < (len / 2))
		return TRUE;
	else
		return FALSE;
}

//v1.1.85 鯢呑亭の関係で夜を0時前と0時過ぎに分ける必要が出たので追加
//0時から5:59までTRUE
bool is_midnight(void)
{
	int day, hour, min;

	extract_day_hour_min(&day, &hour, &min);

	if (hour >= 0 && hour <= 5) return TRUE;
	else return FALSE;

}





/*
 * Extract day, hour, min
 */
/*:::日時分を計算*/
void extract_day_hour_min(int *day, int *hour, int *min)
{
	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/
	s32b turn_in_today = (turn + A_DAY / 4) % A_DAY;/*:::午前零時からのターン数か*/

	/*:::turnは1日目朝6時から開始で24時から2日目　アンデッドは24時からターン18時間経過で開始し次の24時から2日目*/
/*
	switch (p_ptr->start_race)
	{
	case RACE_VAMPIRE:
	case RACE_SKELETON:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		*day = (turn - A_DAY * 3 / 4) / A_DAY + 1;
		break;
	default:
		*day = (turn + A_DAY / 4) / A_DAY + 1;
		break;
	}
*/
	*day = (turn + A_DAY / 4) / A_DAY + 1;

	/*:::時間と分を計算。1日を100000ターンとしてるので1時間は4167、1分は69ターンだがその辺はやや適当に計算してるらしい*/
	*hour = (24 * turn_in_today / A_DAY) % 24;
	*min = (1440 * turn_in_today / A_DAY) % 60;
}

/*
 * Print time
 */
///mod140329 バイド細胞に侵食されたらカウントダウンされるようにした
void prt_time(void)
{
	int day, hour, min;
	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/

	if(p_ptr->muta3 & MUT3_BYDO)
	{
		/*:::バイド細胞カウント　もっとマシな計算方法があるはずだが思いつかなかった*/
		day = ((A_DAY * 3) - p_ptr->bydo) / A_DAY;
		hour = ((A_DAY * 3) - p_ptr->bydo ) % A_DAY / (A_DAY / 24);
		min =  ((A_DAY * 3) - p_ptr->bydo ) % A_DAY % (A_DAY / 24) / (A_DAY / 24 / 60);
		if(hour==24) hour=23;
		if(min==60) min=59;
		if((s32b)p_ptr->bydo > A_DAY*3)
			c_put_str(TERM_RED, format("******************"), ROW_DAY, COL_DAY);
		else
			c_put_str(TERM_RED, format("** %d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);

		return;
	}
	else if(p_ptr->pclass == CLASS_LILY_WHITE)
	{
		/*:::リリーホワイト用カウント　もっとマシな計算方法があるはずだが略*/
		day = ((A_DAY * LILY_LIMIT ) - turn) / A_DAY;
		hour = ((A_DAY * LILY_LIMIT ) - turn ) % A_DAY / (A_DAY / 24);
		min =  ((A_DAY * LILY_LIMIT ) - turn ) % A_DAY % (A_DAY / 24) / (A_DAY / 24 / 60);
		if(hour==24) hour=23;
		if(min==60) min=59;
		if(turn > A_DAY*LILY_LIMIT)	c_put_str(TERM_RED, format("******************"), ROW_DAY, COL_DAY);
		else if(turn < A_DAY * 10) c_put_str(TERM_WHITE, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);
		else if(turn < A_DAY * 20) c_put_str(TERM_YELLOW, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);
		else if(turn < A_DAY * (LILY_LIMIT - 1)) c_put_str(TERM_ORANGE, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);
		else  c_put_str(TERM_RED, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);

		return;
	}
	else if(p_ptr->pclass == CLASS_RAN && !p_ptr->total_winner)
	{
		/*:::藍用カウント　もっとマシな計算方法があるはずだが略*/
		day = ((A_DAY * RAN_LIMIT ) - turn) / A_DAY;
		hour = ((A_DAY * RAN_LIMIT ) - turn ) % A_DAY / (A_DAY / 24);
		min =  ((A_DAY * RAN_LIMIT ) - turn ) % A_DAY % (A_DAY / 24) / (A_DAY / 24 / 60);
		if(hour==24) hour=23;
		if(min==60) min=59;
		if(turn > A_DAY*RAN_LIMIT)	c_put_str(TERM_RED, format("******************"), ROW_DAY, COL_DAY);
		else if(turn < A_DAY * (RAN_LIMIT - 1)) c_put_str(TERM_ORANGE, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);
		else  c_put_str(TERM_RED, format("** %2d - %2d:%2d **", day,hour,min), ROW_DAY, COL_DAY);

		return;
	}

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DAY, COL_DAY);

	extract_day_hour_min(&day, &hour, &min);

	/* Dump the info itself */
#ifdef JP
	if (day < 1000) c_put_str(TERM_WHITE, format("%2d日目", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "***日目", ROW_DAY, COL_DAY);
#else
	if (day < 1000) c_put_str(TERM_WHITE, format("Day%3d", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "Day***", ROW_DAY, COL_DAY);
#endif

	c_put_str(TERM_WHITE, format("%2d:%02d    ", hour, min), ROW_DAY, COL_DAY+7);
}

///sys 今いる場所を文字列にして返す
cptr map_name(void)
{
	if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)
	    && (quest[p_ptr->inside_quest].flags & QUEST_FLAG_PRESET))
#ifdef JP
		return "クエスト";
#else
		return "Quest";
#endif
	else if (EXTRA_QUEST_FLOOR)
		return _("クエスト", "Quest");

	else if (p_ptr->wild_mode)
#ifdef JP
		return "地上";
#else
		return "Surface";
#endif
	else if (p_ptr->inside_arena)
#ifdef JP
	//	return "アリーナ";
		return "悪夢";
#else
		return "Nightmare";
#endif
	else if (p_ptr->inside_battle)
#ifdef JP
		return "(観戦)";
#else
		return "(Spectating)";
#endif
	else if (!dun_level && p_ptr->town_num)
		return town[p_ptr->town_num].name;
	else
		return d_name+d_info[dungeon_type].name;
}

/*
 * Print dungeon
 */
static void prt_dungeon(void)
{
	cptr dungeon_name;
	int col;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DUNGEON, COL_DUNGEON);

	dungeon_name = map_name();

	col = COL_DUNGEON + 6 - strlen(dungeon_name)/2;
	if (col < 0) col = 0;

	/* Dump the info itself */
	c_put_str(TERM_L_UMBER, format("%s",dungeon_name),
		  ROW_DUNGEON, col);
}



/*
 * Print character stat in given row, column
 */
/*:::ステータス現在値を表示する。一つずつ指定する*/
///sys STR-CHAパラメータの画面表示
///mod140325 ステータス上限超過処理適用
static void prt_stat(int stat)
{
	char tmp[32];

	/* Display "injured" stat */
	if (p_ptr->stat_cur[stat] < p_ptr->stat_max[stat])
	{
		put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
		//cnv_stat(p_ptr->stat_use[stat], tmp);
		if(p_ptr->stat_ind[stat]+3 == STAT_HYPERMAX)
		{
			sprintf(tmp,"   *%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_BLUE, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else if(p_ptr->stat_ind[stat]+3 > STAT_ADD_MAX)
		{
			sprintf(tmp,"   !%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_BLUE, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else if(p_ptr->stat_ind[stat]+3 == STAT_ADD_MAX)
		{
			sprintf(tmp,"   !%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else
		{
			sprintf(tmp,"    %2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
	}

	/* Display "healthy" stat */
	else
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		//cnv_stat(p_ptr->stat_use[stat], tmp);
		if(p_ptr->stat_ind[stat]+3 == STAT_HYPERMAX)
		{
			sprintf(tmp,"   *%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_BLUE, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else if(p_ptr->stat_ind[stat]+3 > STAT_ADD_MAX)
		{
			sprintf(tmp,"   !%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_BLUE, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else if(p_ptr->stat_ind[stat]+3 == STAT_ADD_MAX)
		{
			sprintf(tmp,"   !%2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		else
		{
			sprintf(tmp,"    %2d",p_ptr->stat_ind[stat]+3);
			c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
		}
		//c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Indicate natural maximum */
	if (p_ptr->stat_max[stat] == p_ptr->stat_max_max[stat])
	{
#ifdef JP
		/* 日本語にかぶらないように表示位置を変更 */
		//put_str("!", ROW_STAT + stat, 5);
#else
		//put_str("!", ROW_STAT + stat, 3);
#endif

	}
}


/*
 *  Data structure for status bar
 */
#define BAR_TSUYOSHI 0
#define BAR_HALLUCINATION 1
#define BAR_BLINDNESS 2
#define BAR_PARALYZE 3
#define BAR_CONFUSE 4
#define BAR_POISONED 5
#define BAR_AFRAID 6
#define BAR_LEVITATE 7
#define BAR_REFLECTION 8
#define BAR_PASSWALL 9
#define BAR_WRAITH 10
#define BAR_PROTEVIL 11
#define BAR_KAWARIMI 12
#define BAR_MAGICDEFENSE 13
#define BAR_EXPAND 14
#define BAR_STONESKIN 15
#define BAR_MULTISHADOW 16
#define BAR_REGMAGIC 17
#define BAR_ULTIMATE 18
#define BAR_INVULN 19
#define BAR_IMMACID 20
#define BAR_RESACID 21
#define BAR_IMMELEC 22
#define BAR_RESELEC 23
#define BAR_IMMFIRE 24
#define BAR_RESFIRE 25
#define BAR_IMMCOLD 26
#define BAR_RESCOLD 27
#define BAR_RESPOIS 28
#define BAR_RESNETH 29
#define BAR_RESTIME 30
#define BAR_DUSTROBE 31
#define BAR_SHFIRE 32
//#define BAR_TOUKI 33 //練気術師の纏闘気　今使ってないからそのうち何かに使おう→v1.1.44 うどんげ「イビルアンジュレーション」に転用
#define BAR_EVIL_UNDULATION 33
#define BAR_SHHOLY 34
#define BAR_EYEEYE 35
#define BAR_BLESSED 36
#define BAR_HEROISM 37
#define BAR_BERSERK 38
#define BAR_ATTKFIRE 39
#define BAR_ATTKCOLD 40
#define BAR_ATTKELEC 41
#define BAR_ATTKACID 42
#define BAR_ATTKPOIS 43
#define BAR_ATTKCONF 44
#define BAR_SENSEUNSEEN 45
#define BAR_TELEPATHY 46
#define BAR_REGENERATION 47
#define BAR_INFRAVISION 48
#define BAR_STEALTH 49
#define BAR_SUPERSTEALTH 50
#define BAR_RECALL 51
#define BAR_ALTER 52
//呪術関係を再利用
//#define BAR_SHCOLD 53
#define BAR_HIBERNATION 53
//#define BAR_SHELEC 54
#define BAR_SILENT 54
//#define BAR_SHSHADOW 55
#define BAR_SILENT2 55
#define BAR_MAG_ABSORB 56
//#define BAR_BUILD 57
#define BAR_SAKUYA	57
//#define BAR_ANTIMULTI 58
#define BAR_HINA 58
//#define BAR_ANTITELE 59
#define BAR_YUKARI 59
//#define BAR_ANTIMAGIC 60
#define BAR_OPTICAL 60
//#define BAR_PATIENCE 61
#define BAR_DANGO_IMM 61
//#define BAR_REVENGE 62
#define BAR_EIBON 62
//#define BAR_RUNESWORD 63
#define BAR_SAGUME 63
//#define BAR_VAMPILIC 64
#define BAR_ORBITAL_TRAP 64
//#define BAR_CURE 65
#define BAR_HECATIA_MIRROR	65
//#define BAR_ESP_EVIL 66
#define BAR_JUNKO_TIME_SKILL 66

#define BAR_RESWATER 67
#define BAR_DARKSIDEOFTHEMOON 68
#define BAR_RESCHAO 69
#define BAR_NENNBAKU 70
#define BAR_EXSTAT 71
#define BAR_REACTIVE_HEAL 72
#define BAR_RADAR 73
#define BAR_EVILWEAPON 74
#define BAR_RESDARK 75
#define BAR_UNITE_DARK 76
#define BAR_LUCKY 77
#define BAR_FORESIGHT 78
#define BAR_DEPORTATION 79
#define BAR_CLAWEXTEND 80
#define BAR_SPEEDSTER 81
#define BAR_FORCEWEAPON 82
#define BAR_RES_INSANITY 83
#define BAR_KAMIOROSHI 84
#define BAR_ATTACK_FORE 85
#define BAR_ATTACK_UNSUMMON 86
#define BAR_ATTACK_INC_DICES 87
#define BAR_ATTACK_CHAOS 88
#define BAR_KAIOUKEN	89
#define BAR_HEAD_SHOT	90
#define BAR_MANA_SHIELD 91
#define BAR_SYURA 92
#define BAR_STATUE_FORM	93
#define BAR_HELLFIRE_BARRIER	94
#define BAR_ATTACK_VORPAL		95
#define BAR_WASTE_MONEY			96
#define BAR_PLUCK_PIGEON		97
#define BAR_BACKDOOR_POWER		98
#define BAR_SPELLCARD			99
#define BAR_MUEN				100
#define BAR_HEAVY				101
#define BAR_YACHIE_BRIBE		102
#define BAR_YUYUKO_YUUGA		103
#define BAR_SUIKEN				104
#define BAR_LETTY_POWERUP		105 //v1.1.85 レティ耐性貫通
#define BAR_SUPERSTEALTH_FOREST	106 //v1.1.88 木隠れの技術
#define BAR_MOMOYO_EAT			107 //v1.1.89 百々世の何とかイーター

//ここの値は127が現在の限界である。(v1.1.46で95から拡張)


/*:::一時ステータス表示用　多すぎるときは一文字の短縮版が使われる*/
static struct {
	byte attr;
	cptr sstr;
	cptr lstr;
} bar[]
#ifdef JP
= {
	{TERM_YELLOW, "つ", "つよし"},
	{TERM_VIOLET, "幻", "幻覚"},
	{TERM_L_DARK, "盲", "盲目"},
	{TERM_RED, "痺", "麻痺"},
	{TERM_VIOLET, "乱", "混乱"},
	{TERM_GREEN, "毒", "毒"},
	{TERM_BLUE, "恐", "恐怖"},
	{TERM_L_BLUE, "浮", "浮遊"},
	{TERM_SLATE, "反", "反射"},
	{TERM_SLATE, "壁", "壁抜け"},
	{TERM_L_DARK, "幽", "幽体"},
	{TERM_SLATE, "邪", "防邪"},
	{TERM_VIOLET, "変", "変わり身"},
	{TERM_YELLOW, "魔", "魔法鎧"},
	{TERM_L_DARK, "死", "死霊オーラ"},
	{TERM_WHITE, "壁", "物理防御"},
	{TERM_L_BLUE, "分", "分身"},
	{TERM_SLATE, "防", "魔法防御"},
	{TERM_YELLOW, "究", "究極"},
	{TERM_YELLOW, "無", "無敵"},
	{TERM_L_GREEN, "酸", "酸免疫"},
	{TERM_GREEN, "酸", "耐酸"},
	{TERM_L_BLUE, "電", "電免疫"},
	{TERM_BLUE, "電", "耐電"},
	{TERM_L_RED, "火", "火免疫"},
	{TERM_RED, "火", "耐火"},
	{TERM_WHITE, "冷", "冷免疫"},
	{TERM_SLATE, "冷", "耐冷"},
	{TERM_GREEN, "毒", "耐毒"},
	{TERM_L_DARK, "獄", "耐地獄"},
	{TERM_L_BLUE, "時", "耐時間"},
	{TERM_L_DARK, "片", "破片オーラ"},
	{TERM_L_RED, "オ", "火オーラ"},
//	{TERM_WHITE, "闘", "闘気"},
	{TERM_VIOLET, "壁", "障壁波動" },
	{TERM_WHITE, "聖", "聖オーラ"},
	{TERM_VIOLET, "目", "復讐"},
	{TERM_WHITE, "祝", "祝福"},
	{TERM_WHITE, "勇", "勇"},
	{TERM_RED, "狂", "狂乱"},
	{TERM_L_RED, "火", "魔剣火"},
	{TERM_WHITE, "冷", "魔剣冷"},
	{TERM_L_BLUE, "電", "魔剣電"},
	{TERM_SLATE, "酸", "魔剣酸"},
	{TERM_L_GREEN, "毒", "魔剣毒"},
	{TERM_RED, "乱", "混乱打撃"},
	{TERM_L_BLUE, "視", "透明視"},
	{TERM_ORANGE, "テ", "テレパシ"},
	{TERM_L_BLUE, "回", "回復"},
	{TERM_L_RED, "赤", "赤外"},
	{TERM_UMBER, "隠", "隠密"},
	{TERM_YELLOW, "隠", "超隠密"},
	{TERM_WHITE, "帰", "帰還"},
	{TERM_WHITE, "現", "現実変容"},
	/* Hex */
	{TERM_WHITE, "眠", "冬眠"},//冷オーラから変更
	{TERM_BLUE, "静", "静寂"},
	{TERM_L_BLUE, "静", "静寂"},
	{TERM_YELLOW, "吸", "魔力吸収"},//マジックアブソーバー
	{TERM_WHITE, "時", "咲夜の世界"},
	{TERM_L_DARK, "雛", "呪いの雛人形"},
	{TERM_VIOLET, "境", "境界操作"},
	{TERM_L_BLUE, "光", "光学迷彩"},
	{TERM_SLATE, "免", "特殊免疫"},
	{TERM_BLUE, "車", "霧の車輪"},
	{TERM_YELLOW, "弾", "神々の弾冠"},
	{TERM_RED, "月", "公転周期の罠"},
	{TERM_YELLOW, "反", "反射鏡"},
	{TERM_WHITE, "純", "名もなき技"},
	{TERM_L_BLUE, "水", "耐水"},
	{TERM_L_DARK, "月", "闇符"},
	{TERM_VIOLET, "沌", "耐混沌"},
	{TERM_UMBER, "縛", "テレポ阻害"},
	{TERM_L_BLUE, "強", "強化"},
	{TERM_L_BLUE, "回", "反応回復"},
	{TERM_RED, "感", "レーダー"},
	{TERM_VIOLET, "魔", "魔剣"},
	{TERM_L_DARK, "暗", "耐暗黒"},
	{TERM_L_DARK, "闇", "闇融和"},
	{TERM_WHITE, "運", "幸運"},
	{TERM_WHITE, "予", "予知"},
	{TERM_UMBER, "送", "送還術"},
	{TERM_WHITE, "爪", "爪強化"},
	{TERM_L_BLUE, "駆", "高速移動"},
	{TERM_YELLOW, "理", "理力"},
	{TERM_VIOLET, "狂", "耐狂気"},
	{TERM_RED, "神", "神降ろし"},
	{TERM_L_BLUE, "予", "予見魔剣"},
	{TERM_VIOLET, "歪", "歪曲魔剣"},
	{TERM_YELLOW, "武", "強化魔剣"},
	{TERM_VIOLET, "沌", "混沌魔剣"},
	{TERM_RED, "界", "界王拳"},
	{TERM_RED, "狙", "照準"},
	{TERM_L_BLUE, "防", "結界ガード"},
	{TERM_RED, "血", "修羅の血"},
	{TERM_L_DARK, "像", "石像化"},

	{TERM_L_DARK, "救", "救済"},
	{ TERM_RED, "切", "切れ味" },
	{ TERM_YELLOW, "富", "散財" },
	{ TERM_RED, "禍", "財禍" },
	{ TERM_YELLOW, "扉", "後戸の力" },
	{ TERM_YELLOW, "符", "スペルカード" },

	{ TERM_YELLOW, "芸", "無縁の芸能者" },
	{ TERM_L_DARK, "重", "加重" },
	{ TERM_YELLOW, "金", "買収" },

	{ TERM_L_BLUE, "誘", "死出の誘蛾灯" },
	{ TERM_YELLOW, "酔", "酔拳" },
	{ TERM_WHITE, "貫", "冷気耐性貫通" },

	{ TERM_GREEN, "森", "超隠密(森)" },

	{ TERM_RED, "喰", "暴食" },

	{0, NULL, NULL}
};
#else
= {
	{TERM_YELLOW, "Ts", "Tsuyoshi"},
	{TERM_VIOLET, "Ha", "Halluc"},
	{TERM_L_DARK, "Bl", "Blind"},
	{TERM_RED, "Pa", "Paralyzed"},
	{TERM_VIOLET, "Cf", "Confused"},
	{TERM_GREEN, "Po", "Poisoned"},
	{TERM_BLUE, "Af", "Afraid"},
	{TERM_L_BLUE, "Lv", "Levit"},
	{TERM_SLATE, "Rf", "Reflect"},
	{TERM_SLATE, "Pw", "PassWall"},
	{TERM_L_DARK, "Wr", "Wraith"},
	{TERM_SLATE, "Ev", "PrtEvl"},
	{TERM_VIOLET, "Kw", "Kawarimi"},
	{TERM_YELLOW, "Md", "MgcArm"},
	{TERM_L_DARK, "Gh", "GhstAura"},
	{TERM_WHITE, "Df", "PhysDef"},
	{TERM_L_BLUE, "Ms", "MltShdw"},
	{TERM_SLATE, "Rm", "ResMag"},
	{TERM_YELLOW, "Ul", "Ultima"},
	{TERM_YELLOW, "Iv", "Invuln"},
	{TERM_L_GREEN, "IAc", "ImmAcid"},
	{TERM_GREEN, "Ac", "Acid"},
	{TERM_L_BLUE, "IEl", "ImmElec"},
	{TERM_BLUE, "El", "Elec"},
	{TERM_L_RED, "IFi", "ImmFire"},
	{TERM_RED, "Fi", "Fire"},
	{TERM_WHITE, "ICo", "ImmCold"},
	{TERM_SLATE, "Co", "Cold"},
	{TERM_GREEN, "Po", "Pois"},
	{TERM_L_DARK, "Nt", "Nthr"},
	{TERM_L_BLUE, "Ti", "Time"},
	{TERM_L_DARK, "Mr", "Mirr"},
	{TERM_L_RED, "SFi", "SFire"},
//	{TERM_WHITE, "Fo", "Force"},
    {TERM_VIOLET, "Br", "BarrWave"},
	{TERM_WHITE, "Ho", "Holy"},
	{TERM_VIOLET, "Ee", "Revenge"},
	{TERM_WHITE, "Bs", "Bless"},
	{TERM_WHITE, "He", "Hero"},
	{TERM_RED, "Br", "Berserk"},
	{TERM_L_RED, "BFi", "BFire"},
	{TERM_WHITE, "BCo", "BCold"},
	{TERM_L_BLUE, "BEl", "BElec"},
	{TERM_SLATE, "BAc", "BAcid"},
	{TERM_L_GREEN, "BPo", "BPois"},
	{TERM_RED, "TCf", "TchCnf"},
	{TERM_L_BLUE, "Se", "SInv"},
	{TERM_ORANGE, "Te", "Telepa"},
	{TERM_L_BLUE, "Rg", "Regen"},
	{TERM_L_RED, "If", "Infr"},
	{TERM_UMBER, "Sl", "Stealth"},
	{TERM_YELLOW, "SSl", "SprStlth"},
	{TERM_WHITE, "Rc", "Recall"},
	{TERM_WHITE, "Al", "Alter"},
	/* Hex */
	{TERM_WHITE, "Hb", "Hibern"},//冷オーラから変更
	{TERM_BLUE, "Sln", "Silence"},
	{TERM_L_BLUE, "Sln", "Silence"},
	{TERM_YELLOW, "Abs", "MgcAbsrb"},//マジックアブソーバー
	{TERM_WHITE, "Tim", "SakuyaWld"},
	{TERM_L_DARK, "Hn", "CrsHina"},
	{TERM_VIOLET, "Br", "BarrManip"},
	{TERM_L_BLUE, "Op", "OptCmflg"},
	{TERM_SLATE, "Imm", "SpecImm"},
	{TERM_BLUE, "Frr", "FrrFog"},
	{TERM_YELLOW, "Crn", "Crntion"},
	{TERM_RED, "Mn", "OrbtlTrp"},
	{TERM_YELLOW, "Rf", "RfMirr"},
	{TERM_WHITE, "Pr", "NmlssPw"},
	{TERM_L_BLUE, "Wat", "RWater"},
	{TERM_L_DARK, "Mn", "DarkSign"},
	{TERM_VIOLET, "Ch", "RChaos"},
	{TERM_UMBER, "Bnd", "StopTele"},
	{TERM_L_BLUE, "Pw", "Empower"},
	{TERM_L_BLUE, "Hl", "RctvHeal"},
	{TERM_RED, "Rdr", "Radar"},
	{TERM_VIOLET, "Dm", "Dmnblade"},
	{TERM_L_DARK, "Dk", "RDark"},
	{TERM_L_DARK, "DK", "DarkUnt"},
	{TERM_WHITE, "Lck", "GoodLuck"},
	{TERM_WHITE, "Pcg", "Precog"},
	{TERM_UMBER, "Uns", "Unsummon"},
	{TERM_WHITE, "Clw", "Claws"},
	{TERM_L_BLUE, "Sw", "SwftMove"},
	{TERM_YELLOW, "Fc", "ForceWpn"},
	{TERM_VIOLET, "Ins", "RInsan"},
	{TERM_RED, "Dv", "DvnPoss"},
	{TERM_L_BLUE, "Fs", "BFsight"},
	{TERM_VIOLET, "Dst", "BDistrt"},
	{TERM_YELLOW, "Pow", "BPower"},
	{TERM_VIOLET, "Ch", "BChaos"},
	{TERM_RED, "Kai", "Kaioken"},
	{TERM_RED, "Aim", "Aim"},
	{TERM_L_BLUE, "Def", "BarrGrd"},
	{TERM_RED, "Bld", "AsuraBld"},
	{TERM_L_DARK, "St", "Statue"},

	{TERM_L_DARK, "Rlf", "Relief"},
	{ TERM_RED, "Shp", "Sharp" },
	{ TERM_YELLOW, "Wlt", "Spend" },
	{ TERM_RED, "Ftn", "Fortune" },
	{ TERM_YELLOW, "Dr", "Backdoor" },
	{ TERM_YELLOW, "Sc", "Spllcard" },

	{ TERM_YELLOW, "Pf", "UnatPrfm" },
	{ TERM_L_DARK, "Wgt", "Weight" },
	{ TERM_YELLOW, "Mny", "Recruit" },

	{ TERM_L_BLUE, "Lur", "LghtTrap" },
	{ TERM_YELLOW, "Drk", "DrnkFist" },
	{ TERM_WHITE, "Pnt", "PntRCold" },

	{ TERM_GREEN, "For", "FrstSlth" },

	{ TERM_RED, "Eat", "Glutton" },

	{0, NULL, NULL}
};
#endif

#define ADD_FLG(FLG) (bar_flags[FLG / 32] |= (1L << (FLG % 32)))
#define IS_FLG(FLG) (bar_flags[FLG / 32] & (1L << (FLG % 32)))


/*
 *  Show status bar
 */
/*:::耐火や混乱など一時的状態を画面下に表示　ステータスが多すぎる場合短縮版がある*/
///sys　耐火など一時的ステータスを画面下に表示
//v1.1.46 bar_flags[]のサイズを3から4に拡張
static void prt_status(void)
{
	u32b bar_flags[4];
	int wid, hgt, row_statbar, max_col_statbar;
	int i, col = 0, num = 0;
	int space = 2;

	Term_get_size(&wid, &hgt);
	row_statbar = hgt + ROW_STATBAR;
	max_col_statbar = wid + MAX_COL_STATBAR;

	Term_erase(0, row_statbar, max_col_statbar);

	bar_flags[0] = bar_flags[1] = bar_flags[2] = bar_flags[3] = 0L;

	/* Tsuyoshi  */
	if (p_ptr->tsuyoshi) ADD_FLG(BAR_TSUYOSHI);

	/* Hallucinating */
	if (p_ptr->image) ADD_FLG(BAR_HALLUCINATION);

	/* Blindness */
	if (p_ptr->blind) ADD_FLG(BAR_BLINDNESS);

	/* Paralysis */
	if (p_ptr->paralyzed) ADD_FLG(BAR_PARALYZE);

	/* Confusion */
	if (p_ptr->confused) ADD_FLG(BAR_CONFUSE);

	/* Posioned */
	if (p_ptr->poisoned) ADD_FLG(BAR_POISONED);

	/* Times see-invisible */
	if (p_ptr->tim_invis) ADD_FLG(BAR_SENSEUNSEEN);

	/* Timed esp */
	if (IS_TIM_ESP()) ADD_FLG(BAR_TELEPATHY);

	/* Timed regenerate */
	if (p_ptr->tim_regen) ADD_FLG(BAR_REGENERATION);

	/* Timed infra-vision */
	if (p_ptr->tim_infra) ADD_FLG(BAR_INFRAVISION);

	/* Protection from evil */
	if (p_ptr->protevil) ADD_FLG(BAR_PROTEVIL);

	/* Invulnerability */
	if (IS_INVULN()) ADD_FLG(BAR_INVULN);

	//v1.1.78
	if ((p_ptr->special_attack & ATTACK_SUIKEN)) ADD_FLG(BAR_SUIKEN);

	//v1.1.85
	if (WINTER_LETTY2) ADD_FLG(BAR_LETTY_POWERUP);

	/* Wraith form */
	if (p_ptr->wraith_form && p_ptr->pclass == CLASS_RUMIA) ADD_FLG(BAR_DARKSIDEOFTHEMOON);
	else if (p_ptr->wraith_form) ADD_FLG(BAR_WRAITH);

	/* Kabenuke */
	if (p_ptr->kabenuke) ADD_FLG(BAR_PASSWALL);

	if (p_ptr->tim_reflect) ADD_FLG(BAR_REFLECTION);
	else if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO2))
		ADD_FLG(BAR_REFLECTION);

	/* Heroism */
	if (IS_HERO()) ADD_FLG(BAR_HEROISM);

	/* Super Heroism / berserk */
	if (p_ptr->shero) ADD_FLG(BAR_BERSERK);

	/* Blessed */
	if (IS_BLESSED()) ADD_FLG(BAR_BLESSED);

	/* Shield */
	if (p_ptr->magicdef) ADD_FLG(BAR_MAGICDEFENSE);

	if (p_ptr->tim_sh_death) ADD_FLG(BAR_EXPAND);

	if (p_ptr->shield) ADD_FLG(BAR_STONESKIN);

	if (p_ptr->special_defense & NINJA_KAWARIMI) ADD_FLG(BAR_KAWARIMI);

	/* Oppose Acid */
	if (p_ptr->special_defense & DEFENSE_ACID) ADD_FLG(BAR_IMMACID);
	if (IS_OPPOSE_ACID()) ADD_FLG(BAR_RESACID);

	/* Oppose Lightning */
	if (p_ptr->special_defense & DEFENSE_ELEC) ADD_FLG(BAR_IMMELEC);
	if (IS_OPPOSE_ELEC()) ADD_FLG(BAR_RESELEC);

	/* Oppose Fire */
	if (p_ptr->special_defense & DEFENSE_FIRE) ADD_FLG(BAR_IMMFIRE);
	if (IS_OPPOSE_FIRE()) ADD_FLG(BAR_RESFIRE);

	/* Oppose Cold */
	if (p_ptr->special_defense & DEFENSE_COLD) ADD_FLG(BAR_IMMCOLD);
	if (IS_OPPOSE_COLD()) ADD_FLG(BAR_RESCOLD);

	/* Oppose Poison */
	if (IS_OPPOSE_POIS()) ADD_FLG(BAR_RESPOIS);

	/* Word of Recall */
	if (p_ptr->word_recall) ADD_FLG(BAR_RECALL);

	/* Alter realiry */
	if (p_ptr->alter_reality) ADD_FLG(BAR_ALTER);

	/* Afraid */
	if (p_ptr->afraid) ADD_FLG(BAR_AFRAID);

	/* Resist time */
	if (p_ptr->tim_res_time) ADD_FLG(BAR_RESTIME);

	if (p_ptr->multishadow) ADD_FLG(BAR_MULTISHADOW);

	/* Confusing Hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE) ADD_FLG(BAR_ATTKCONF);

	if (p_ptr->resist_magic) ADD_FLG(BAR_REGMAGIC);

	/* Ultimate-resistance */
	if (p_ptr->ult_res) ADD_FLG(BAR_ULTIMATE);

	/* tim levitation */
	if (p_ptr->tim_levitation) ADD_FLG(BAR_LEVITATE);

	if (p_ptr->tim_res_nether) ADD_FLG(BAR_RESNETH);

	if (p_ptr->dustrobe) ADD_FLG(BAR_DUSTROBE);

	if (p_ptr->tim_res_dark) ADD_FLG(BAR_RESDARK);

	///mod140209
	if (p_ptr->tim_res_water) ADD_FLG(BAR_RESWATER);
	if (p_ptr->tim_res_chaos) ADD_FLG(BAR_RESCHAO);
	if (p_ptr->tim_unite_darkness) ADD_FLG(BAR_UNITE_DARK);

	if (p_ptr->tim_superstealth)
	{
		if(p_ptr->superstealth_type == SUPERSTEALTH_TYPE_OPTICAL)
			ADD_FLG(BAR_OPTICAL);
		else if (p_ptr->superstealth_type == SUPERSTEALTH_TYPE_FOREST)
			ADD_FLG(BAR_SUPERSTEALTH_FOREST);
		else
			ADD_FLG(BAR_SUPERSTEALTH);
	}

	//v1.1.44
	if(p_ptr->special_defense & CHECK_EVIL_UNDULATION_MASK)
		ADD_FLG(BAR_EVIL_UNDULATION);

	///mod140502
	if (p_ptr->lucky) ADD_FLG(BAR_LUCKY);
	if (p_ptr->foresight) ADD_FLG(BAR_FORESIGHT);
	if (p_ptr->deportation) ADD_FLG(BAR_DEPORTATION);
	//v1.1.54
	if(CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO))ADD_FLG(BAR_DEPORTATION);

	///mod140510
	if (p_ptr->tim_speedster) ADD_FLG(BAR_SPEEDSTER);
	if (p_ptr->tim_res_insanity) ADD_FLG(BAR_RES_INSANITY);
	if(p_ptr->kamioroshi_count) ADD_FLG(BAR_KAMIOROSHI);

	///mod140325
	if (p_ptr->clawextend) ADD_FLG(BAR_CLAWEXTEND);

	///mod140325
	if (p_ptr->reactive_heal || (p_ptr->pclass == CLASS_MEDICINE && cave_have_flag_bold(py, px, FF_POISON_PUDDLE) && cave_have_flag_bold(py, px, FF_DEEP))) ADD_FLG(BAR_REACTIVE_HEAL);

	///mod140329
	if (p_ptr->radar_sense) ADD_FLG(BAR_RADAR);

	///mod140321
	if (p_ptr->nennbaku) ADD_FLG(BAR_NENNBAKU);

	///mod150420
	if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0])  ADD_FLG(BAR_KAIOUKEN);

	//v1.1.57
	if (p_ptr->pclass == CLASS_OKINA && p_ptr->tim_general[0])  ADD_FLG(BAR_MUEN);

	//v1.1.69
	if (p_ptr->pclass == CLASS_URUMI && p_ptr->tim_general[0])  ADD_FLG(BAR_HEAVY);

	///v1.1.76
	if (p_ptr->pclass == CLASS_YUYUKO && p_ptr->tim_general[0])  ADD_FLG(BAR_YUYUKO_YUUGA);

	//v1.1.89 百々世
	if (p_ptr->pclass == CLASS_MOMOYO)
	{
		if (p_ptr->tim_general[1] || p_ptr->tim_general[2] || p_ptr->tim_general[3]) ADD_FLG(BAR_MOMOYO_EAT);
	}

	for(i=0;i<6;i++) if(p_ptr->tim_addstat_count[i])  ADD_FLG(BAR_EXSTAT);

	if(p_ptr->pclass == CLASS_RINGO && p_ptr->tim_general[0]) ADD_FLG(BAR_EXSTAT);

	if(USE_NAMELESS_ARTS)
	{
		for (i = 0; i < TIM_GENERAL_MAX; i++)
		{
			if (p_ptr->tim_general[i])
			{
				if(p_ptr->pclass == CLASS_JUNKO)
					ADD_FLG(BAR_JUNKO_TIME_SKILL);
				else
					ADD_FLG(BAR_BACKDOOR_POWER);
			}
		}
	}
	if(p_ptr->tim_spellcard_count)
		ADD_FLG(BAR_SPELLCARD);

	/* Mahouken */
	if (p_ptr->special_attack & ATTACK_FIRE) ADD_FLG(BAR_ATTKFIRE);
	if (p_ptr->special_attack & ATTACK_COLD) ADD_FLG(BAR_ATTKCOLD);
	if (p_ptr->special_attack & ATTACK_ELEC) ADD_FLG(BAR_ATTKELEC);
	if (p_ptr->special_attack & ATTACK_ACID) ADD_FLG(BAR_ATTKACID);
	if (p_ptr->special_attack & ATTACK_POIS) ADD_FLG(BAR_ATTKPOIS);
	if (p_ptr->special_defense & NINJA_S_STEALTH) ADD_FLG(BAR_SUPERSTEALTH);

	if (p_ptr->special_attack & ATTACK_DARK) ADD_FLG(BAR_EVILWEAPON);
	if (p_ptr->special_attack & ATTACK_FORCE) ADD_FLG(BAR_FORCEWEAPON);
	if (p_ptr->special_attack & ATTACK_FORESIGHT) ADD_FLG(BAR_ATTACK_FORE);
	if (p_ptr->special_attack & ATTACK_UNSUMMON) ADD_FLG(BAR_ATTACK_UNSUMMON);
	if (p_ptr->special_attack & ATTACK_INC_DICES) ADD_FLG(BAR_ATTACK_INC_DICES);
	if (p_ptr->special_attack & ATTACK_CHAOS) ADD_FLG(BAR_ATTACK_CHAOS);

	if (p_ptr->special_attack & ATTACK_VORPAL) ADD_FLG(BAR_ATTACK_VORPAL);

	if (p_ptr->tim_sh_fire) ADD_FLG(BAR_SHFIRE);

	/* tim stealth */
	if (IS_TIM_STEALTH()) ADD_FLG(BAR_STEALTH);

	//if (p_ptr->tim_sh_touki) ADD_FLG(BAR_TOUKI);

	/* Holy aura */
	if (p_ptr->tim_sh_holy) ADD_FLG(BAR_SHHOLY);

	/* An Eye for an Eye */
	if (p_ptr->tim_eyeeye) ADD_FLG(BAR_EYEEYE);

	//諏訪子冬眠
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0]) ADD_FLG(BAR_HIBERNATION);
	//ルナチャ静寂
	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0]))
		 ADD_FLG(BAR_SILENT);
	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[1]))
		 ADD_FLG(BAR_SILENT2);
	if(p_ptr->pclass == CLASS_MARISA && (p_ptr->tim_general[0]))
		 ADD_FLG(BAR_MAG_ABSORB);

	if(p_ptr->pclass == CLASS_RINGO && p_ptr->tim_general[1])
		 ADD_FLG(BAR_DANGO_IMM);

	if(SAKUYA_WORLD)
		 ADD_FLG(BAR_SAKUYA);

	if(p_ptr->special_defense & SD_HINA_NINGYOU)
		 ADD_FLG(BAR_HINA);


	if(p_ptr->special_defense & SD_STATUE_FORM)
		 ADD_FLG(BAR_STATUE_FORM);
	else if(p_ptr->special_defense & SD_EIBON_WHEEL)
		 ADD_FLG(BAR_EIBON);

	if(p_ptr->pclass == CLASS_YUKARI && (p_ptr->tim_general[0] || p_ptr->tim_general[1] || p_ptr->tim_general[2] || p_ptr->tim_general[3]))
		 ADD_FLG(BAR_YUKARI);

	if(p_ptr->pclass == CLASS_SAGUME && (p_ptr->tim_general[0]))
		 ADD_FLG(BAR_SAGUME);

	if(p_ptr->special_flags & SPF_ORBITAL_TRAP)
		 ADD_FLG(BAR_ORBITAL_TRAP);

	if(p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
		 ADD_FLG(BAR_HECATIA_MIRROR);

	if(p_ptr->pclass == CLASS_YOUMU && p_ptr->tim_general[0])
		 ADD_FLG(BAR_SYURA);

	if(p_ptr->special_attack & ATTACK_HEAD_SHOT)
		 ADD_FLG(BAR_HEAD_SHOT);

	if (p_ptr->special_defense & SD_MANA_SHIELD)
		 ADD_FLG(BAR_MANA_SHIELD);


	if(p_ptr->special_defense & SD_HELLFIRE_BARRIER)
		 ADD_FLG(BAR_HELLFIRE_BARRIER);

	if (p_ptr->special_attack & ATTACK_WASTE_MONEY)
		ADD_FLG(BAR_WASTE_MONEY);
	if (p_ptr->pclass == CLASS_JYOON && p_ptr->tim_general[0])
		ADD_FLG(BAR_PLUCK_PIGEON);
	//v1.1.73
	if (p_ptr->pclass == CLASS_YACHIE && p_ptr->magic_num1[0])
		ADD_FLG(BAR_YACHIE_BRIBE);



	/* Hex spells */
/*
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling(HEX_BLESS)) ADD_FLG(BAR_BLESSED);
		if (hex_spelling(HEX_DEMON_AURA)) { ADD_FLG(BAR_SHFIRE); ADD_FLG(BAR_REGENERATION); }
		if (hex_spelling(HEX_XTRA_MIGHT)) ADD_FLG(BAR_MIGHT);
		if (hex_spelling(HEX_DETECT_EVIL)) ADD_FLG(BAR_ESP_EVIL);
		if (hex_spelling(HEX_ICE_ARMOR)) ADD_FLG(BAR_SHCOLD);
		if (hex_spelling(HEX_RUNESWORD)) ADD_FLG(BAR_RUNESWORD);
		if (hex_spelling(HEX_BUILDING)) ADD_FLG(BAR_BUILD);
		if (hex_spelling(HEX_ANTI_TELE)) ADD_FLG(BAR_ANTITELE);
		if (hex_spelling(HEX_SHOCK_CLOAK)) ADD_FLG(BAR_SHELEC);
		if (hex_spelling(HEX_SHADOW_CLOAK)) ADD_FLG(BAR_SHSHADOW);
		if (hex_spelling(HEX_CONFUSION)) ADD_FLG(BAR_ATTKCONF);
		if (hex_spelling(HEX_EYE_FOR_EYE)) ADD_FLG(BAR_EYEEYE);
		if (hex_spelling(HEX_ANTI_MULTI)) ADD_FLG(BAR_ANTIMULTI);
		if (hex_spelling(HEX_VAMP_BLADE)) ADD_FLG(BAR_VAMPILIC);
		if (hex_spelling(HEX_ANTI_MAGIC)) ADD_FLG(BAR_ANTIMAGIC);
		if (hex_spelling(HEX_CURE_LIGHT) ||
			hex_spelling(HEX_CURE_SERIOUS) ||
			hex_spelling(HEX_CURE_CRITICAL)) ADD_FLG(BAR_CURE);

		if (p_ptr->magic_num2[2])
		{
			if (p_ptr->magic_num2[1] == 1) ADD_FLG(BAR_PATIENCE);
			if (p_ptr->magic_num2[1] == 2) ADD_FLG(BAR_REVENGE);
		}
	}
*/
	/* Calcurate length */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			col += strlen(bar[i].lstr) + 1;
			num++;
		}
	}

	/* If there are not excess spaces for long strings, use short one */
	if (col - 1 > max_col_statbar)
	{
		space = 0;
		col = 0;

		for (i = 0; bar[i].sstr; i++)
		{
			if (IS_FLG(i))
			{
				col += strlen(bar[i].sstr);
			}
		}

		/* If there are excess spaces for short string, use more */
		if (col - 1 <= max_col_statbar - (num-1))
		{
			space = 1;
			col += num - 1;
		}
	}


	//スペースに余裕があれば現在のBGMを描画する
	if (flag_display_bgm_title && bgm_title_text_ptr)
	{
		int bgm_title_len = strlen(bgm_title_text_ptr);

		if (bgm_title_len  < (max_col_statbar - col - 2))
		{

			col = (max_col_statbar - col - bgm_title_len - 8) / 2;

			//曲名を右詰めで描画
			c_put_str(TERM_WHITE, bgm_title_text_ptr, row_statbar, (max_col_statbar - strlen(bgm_title_text_ptr) - 2));
		}
		else
			col = (max_col_statbar - col) / 2;

	}
	else
	{
		/* Centering display column */
		col = (max_col_statbar - col) / 2;

	}



	/* Display status bar */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			cptr str;
			if (space == 2) str = bar[i].lstr;
			else str = bar[i].sstr;

			c_put_str(bar[i].attr, str, row_statbar, col);
			col += strlen(str);
			if (space > 0) col++;
			if (col > max_col_statbar) break;
		}
	}
}



/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
/*:::職業ごとのランク、勝利者などの文字の表示*/
///sys 勝利者、職業ランクなどの表示
static void prt_title(void)
{
	cptr p = "";
	//char str[14];
	char str[18];

	/* Wizard */
	if (p_ptr->wizard)
	{
#ifdef JP
		/* 英日切り替え機能 称号 */
		p = "[ウィザード]";
#else
		p = "[=-WIZARD-=]";
#endif

	}

	/* Winner */
	else if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
	{
//		if (p_ptr->arena_number > MAX_ARENA_MONS + 2)
		if (PLAYER_IS_TRUE_WINNER)
		{
#ifdef JP
			if(IS_SHION_TRUE_LOSER)
				p = "*真・敗北者*";
			else
				/* 英日切り替え機能 称号 */
				p = "*真・勝利者*";
#else
			if(IS_SHION_TRUE_LOSER)
				p = "*TRUELOSER*";
			else
				p = "*TRUEWINNER*";
#endif
		}
		else
		{
#ifdef JP
			/* 英日切り替え機能 称号 */
			p = "***勝利者***";
#else
			p = "***WINNER***";
#endif
		}
	}

	/* Normal */
	else
	{
		///mod140126　画面左側に出すのを職業称号でなく職業名にした。さらに画面左側を二文字拡張して職業名八文字まで入るようにした
		//my_strcpy(str, player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5], sizeof(str));
		my_strcpy(str, player_class_name, sizeof(str));
		p = str;
	}

	prt_field(p, ROW_TITLE, COL_TITLE);
}


/*
 * Prints level
 */
static void prt_level(void)
{
	char tmp[32];

#ifdef JP
	sprintf(tmp, "%5d", p_ptr->lev);
#else
	sprintf(tmp, "%6d", p_ptr->lev);
#endif


	if (p_ptr->lev >= p_ptr->max_plv)
	{
#ifdef JP
		put_str("レベル ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("LEVEL ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
	else
	{
#ifdef JP
		put_str("xレベル", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("Level ", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
}


/*
 * Display the experience
 */
static void prt_exp(void)
{
	char out_val[32];

	if ((!exp_need)||(p_ptr->prace == RACE_ANDROID))
	{
#ifdef JP
	(void)sprintf(out_val, "%7ld", (long)p_ptr->exp);
#else
	(void)sprintf(out_val, "%8ld", (long)p_ptr->exp);
#endif
	}
	else
	{
		if (p_ptr->lev >= PY_MAX_LEVEL)
		{
			(void)sprintf(out_val, "********");
		}
		else
		{
#ifdef JP
			(void)sprintf(out_val, "%7ld", (long)(player_exp [p_ptr->lev - 1] * p_ptr->expfact / 100L) - p_ptr->exp);
#else
			(void)sprintf(out_val, "%8ld", (long)(player_exp [p_ptr->lev - 1] * p_ptr->expfact / 100L) - p_ptr->exp);
#endif
		}
	}

	if (p_ptr->exp >= p_ptr->max_exp)
	{
#ifdef JP
		if (p_ptr->prace == RACE_ANDROID) put_str("強化 ", ROW_EXP, 0);
		else put_str("経験 ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 5);
#else
		if (p_ptr->prace == RACE_ANDROID) put_str("Cst ", ROW_EXP, 0);
		else put_str("EXP ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
	else
	{
#ifdef JP
		put_str("x経験", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 5);
#else
		put_str("Exp ", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
}

/*
 * Prints current gold
 */
static void prt_gold(void)
{
	char tmp[32];

#ifdef JP
	put_str("＄ ", ROW_GOLD, COL_GOLD);
#else
	put_str("AU ", ROW_GOLD, COL_GOLD);
#endif

	sprintf(tmp, "%9ld", (long)p_ptr->au);
	c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}



/*
 * Prints current AC
 */
static void prt_ac(void)
{
	char tmp[32];

#ifdef JP
/* AC の表示方式を変更している */
	put_str(" ＡＣ(     )", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 6);
#else
	put_str("Cur AC ", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 7);
#endif

}


/*
 * Prints Cur/Max hit points
 */
static void prt_hp(void)
{
/* ヒットポイントの表示方法を変更 */
	char tmp[32];

	byte color;

	/* タイトル */
/*	put_str(" ＨＰ・ＭＰ", ROW_HPMP, COL_HPMP); */

	put_str("HP", ROW_CURHP, COL_CURHP);

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		sprintf(tmp, "%5ld", (long int)p_ptr->mhp);
		if(p_ptr->magic_num1[0] < 10000)
			color = TERM_L_GREEN;
		else if(p_ptr->magic_num1[0] < 20000)
			color = TERM_YELLOW;
		else
			color = TERM_RED;

		c_put_str(color, tmp, ROW_CURHP, COL_CURHP+7);
		return;
	}

	/* 現在のヒットポイント */
	sprintf(tmp, "%4ld", (long int)p_ptr->chp);

	if (p_ptr->chp >= p_ptr->mhp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP+3);


	/* 区切り */
	put_str( "/", ROW_CURHP, COL_CURHP + 7 );

	/* 最大ヒットポイント */
	sprintf(tmp, "%4ld", (long int)p_ptr->mhp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 8 );
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp(void)
{
/* マジックポイントの表示方法を変更している */
	char tmp[32];
	byte color;

	/* Do not show mana unless it matters */
	//if (!mp_ptr->spell_book) return;

	/* タイトル */
/*	put_str(" ＭＰ / 最大", ROW_MAXSP, COL_MAXSP); */

#ifdef JP
	put_str("MP", ROW_CURSP, COL_CURSP);
#else
	put_str("MP", ROW_CURSP, COL_CURSP);
#endif

	/* 現在のマジックポイント */
	sprintf(tmp, "%4ld", (long int)p_ptr->csp);

	if (p_ptr->csp >= p_ptr->msp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->csp > (p_ptr->msp * mana_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP+3);

	/* 区切り */
	put_str( "/", ROW_CURSP, COL_CURSP + 7 );

	/* 最大マジックポイント */
	sprintf(tmp, "%4ld", (long int)p_ptr->msp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 8);
}


/*
 * Prints depth in stat area
 */
static void prt_depth(void)
{
	char depths[32];
	int wid, hgt, row_depth, col_depth;
	byte attr = TERM_WHITE;

	Term_get_size(&wid, &hgt);
	col_depth = wid + COL_DEPTH;
	row_depth = hgt + ROW_DEPTH;

	if (!dun_level)
	{
#ifdef JP
		strcpy(depths, "地上");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else if (p_ptr->inside_quest && !dungeon_type)
	{
#ifdef JP
		strcpy(depths, "地上");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else
	{
#ifdef JP
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "%d 階", dun_level);
#else
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "Lev %d", dun_level);
#endif


		/* Get color of level based on feeling  -JSV- */
		switch (p_ptr->feeling)
		{
		case  0: attr = TERM_SLATE;   break; /* Unknown */
		case  1: attr = TERM_L_BLUE;  break; /* Special */
		case  2: attr = TERM_VIOLET;  break; /* Horrible visions */
		case  3: attr = TERM_RED;     break; /* Very dangerous */
		case  4: attr = TERM_L_RED;   break; /* Very bad feeling */
		case  5: attr = TERM_ORANGE;  break; /* Bad feeling */
		case  6: attr = TERM_YELLOW;  break; /* Nervous */
		case  7: attr = TERM_L_UMBER; break; /* Luck is turning */
		case  8: attr = TERM_L_WHITE; break; /* Don't like */
		case  9: attr = TERM_WHITE;   break; /* Reasonably safe */
		case 10: attr = TERM_WHITE;   break; /* Boring place */
		}
	}

	/* Right-Adjust the "depth", and clear old values */
	c_prt(attr, format("%7s", depths), row_depth, col_depth);
}


/*
 * Prints status of hunger
 */
static void prt_hunger(void)
{
	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
#ifdef JP
		c_put_str(TERM_RED, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "空腹  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Normal */
	else if (p_ptr->food < PY_FOOD_FULL)
	{
		c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "満腹  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_L_GREEN, "Full  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Gorged */
	else
	{
#ifdef JP
		//c_put_str(TERM_GREEN, "食過ぎ", ROW_HUNGRY, COL_HUNGRY);
		c_put_str(TERM_GREEN, "過食", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_GREEN, "Gorged", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
///mod150114 移動状態表示を追加してみた
static void prt_state(void)
{
	byte attr = TERM_WHITE;
	char text[7];
	cave_type *c_ptr = &cave[py][px];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	bool fish_tail = FALSE;

	if((prace_is_(RACE_NINGYO) || p_ptr->muta3 & MUT3_FISH_TAIL) && !(p_ptr->riding && !CLASS_RIDING_BACKDANCE)) fish_tail = TRUE;

	/* Repeating */
	if (command_rep)
	{
		if (command_rep > 999)
		{
#ifdef JP
sprintf(text, "%2d00", command_rep / 100);
#else
			(void)sprintf(text, "%2d00", command_rep / 100);
#endif

		}
		else
		{
#ifdef JP
sprintf(text, "  %2d", command_rep);
#else
			(void)sprintf(text, "  %2d", command_rep);
#endif

		}
	}
	/* Action */
	else
	{
		switch(p_ptr->action)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				strcpy(text, "探索");
#else
				strcpy(text, "Sear");
#endif
				break;
			}
			case ACTION_REST:
			{
				int i;

				/* Start with "Rest" */
#ifdef JP
				strcpy(text, "    ");
#else
				strcpy(text, "    ");
#endif


				/* Extensive (timed) rest */
				if (resting >= 1000)
				{
					i = resting / 100;
					text[3] = '0';
					text[2] = '0';
					text[1] = '0' + (i % 10);
					text[0] = '0' + (i / 10);
				}

				/* Long (timed) rest */
				else if (resting >= 100)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					i = i / 10;
					text[2] = '0' + (i % 10);
					text[1] = '0' + (i / 10);
				}

				/* Medium (timed) rest */
				else if (resting >= 10)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					text[2] = '0' + (i / 10);
				}

				/* Short (timed) rest */
				else if (resting > 0)
				{
					i = resting;
					text[3] = '0' + (i);
				}

				/* Rest until healed */
				else if (resting == -1)
				{
					text[0] = text[1] = text[2] = text[3] = '*';
				}

				/* Rest until done */
				else if (resting == -2)
				{
					text[0] = text[1] = text[2] = text[3] = '&';
				}
				break;
			}
			///class 青魔
			case ACTION_LEARN:
			{
#ifdef JP
				strcpy(text, "学習");
#else
				strcpy(text, "lear");
#endif
				if (new_mane) attr = TERM_L_RED;
				break;
			}
			case ACTION_FISH:
			{
#ifdef JP
				strcpy(text, "釣り");
#else
				strcpy(text, "fish");
#endif
				break;
			}
			///class 修行僧
			case ACTION_KAMAE:
			{
				int i;
				for (i = 0; i < MAX_KAMAE; i++)
					if (p_ptr->special_defense & (KAMAE_GENBU << i)) break;
				switch (i)
				{
					case 0: attr = TERM_GREEN;break;
					case 1: attr = TERM_WHITE;break;
					case 2: attr = TERM_L_BLUE;break;
					case 3: attr = TERM_L_RED;break;
				}
				strcpy(text, kamae_shurui[i].desc);
				break;
			}
			///class 剣術
			case ACTION_KATA:
			{
				int i;
				if (p_ptr->pclass == CLASS_YUYUKO) //v1.1.76
				{
					strcpy(text, _("涅槃", "Nirv"));
				}
				else
				{
					for (i = 0; i < MAX_KATA; i++)
						if (p_ptr->special_defense & (KATA_IAI << i)) break;
					strcpy(text, kata_shurui[i].desc);
				}

				break;
			}
			///class 吟遊
			case ACTION_SING:
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_SANNYO)
					strcpy(text, "煙草");
				else
					strcpy(text, "歌  ");


#else
				if(p_ptr->pclass == CLASS_SANNYO)
					strcpy(text, "Smok");
				else
                    strcpy(text, "Sing");
#endif
				break;
			}
			///class 忍者
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				strcpy(text, "速駆");
#else
				strcpy(text, "Fast");
#endif
				break;
			}
			///class 呪術
			case ACTION_SPELL:
			{
#ifdef JP
				strcpy(text, "詠唱");
#else
				strcpy(text, "Spel");
#endif
				break;
			}

			default:
			{
				strcpy(text, "    ");
				break;
			}
		}
	}

	/* Display the info (or blanks) */
	c_put_str(attr, format("%5.5s",text), ROW_STATE, COL_STATE);


	///mod150114 表示位置変更
	if(p_ptr->drowning)
	{
		if(prace_is_(RACE_VAMPIRE)) attr = TERM_RED;
		else  attr = TERM_ORANGE;
		strcpy(text, _("水没", "Drwn"));
	}
	else if(CHECK_CONCEALMENT)
	{
		attr = TERM_L_DARK;
		strcpy(text, _("潜伏", "Hide"));
	}
	else if((p_ptr->pclass == CLASS_MURASA || p_ptr->prace == RACE_KAPPA) && have_flag(f_ptr->flags, FF_WATER))
	{
		attr = TERM_L_BLUE;
		strcpy(text, _("水中", "Watr"));
	}
	//人魚が変身したときは、水地形パワーアップはそのままで鈍足ペナルティは消えるはず
	else if(p_ptr->prace == RACE_NINGYO && have_flag(f_ptr->flags, FF_WATER))
	{
		attr = TERM_L_BLUE;
		strcpy(text, _("速泳", "SSwm"));
	}
	else if(CHECK_FAIRY_TYPE == 32 && have_flag(f_ptr->flags, FF_WATER))
	{
		attr = TERM_L_BLUE;
		strcpy(text, _("速泳", "SSwm"));
	}
	else if(p_ptr->levitation)
	{
		if(p_ptr->speedster)
		{
			attr = TERM_L_GREEN;
			strcpy(text, _("飛翔", "Fly"));
		}
		else
		{
			attr = TERM_L_DARK;
			strcpy(text, _("浮遊", "Levi"));
		}
	}
	else if(have_flag(f_ptr->flags, FF_WATER))
	{
		if(p_ptr->speedster)
		{
			attr = TERM_L_BLUE;
			strcpy(text, _("速泳", "SSwm"));
		}
		else
		{
			attr = TERM_WHITE;
			strcpy(text, _("水泳", "Swim"));
		}
	}
	else if(have_flag(f_ptr->flags, FF_TREE) && !p_ptr->kill_wall && (p_ptr->pclass != CLASS_RANGER && !prace_is_(RACE_YAMAWARO)))
	{
		if(p_ptr->speedster)
		{
			attr = TERM_WHITE;
			strcpy(text, _("徒歩", "Walk"));
		}
		else
		{
			if(fish_tail || (p_ptr->mimic_form == MIMIC_SLIME)) attr = TERM_RED;
			else attr = TERM_L_UMBER;
			strcpy(text, _("鈍足", "Slow"));
		}
	}

	else
	{
		if(p_ptr->speedster)
		{
			attr = TERM_YELLOW;
			strcpy(text, _("俊足", "Swft"));
		}
		else if(fish_tail || (p_ptr->mimic_form == MIMIC_SLIME))
		{
			attr = TERM_L_UMBER;
			strcpy(text, _("鈍足", "Slow"));
		}
		else
		{
			attr = TERM_WHITE;
			strcpy(text, _("徒歩", "Walk"));
		}
	}
	c_put_str(attr, format("%5.5s",text), ROW_MOVESTATE, COL_MOVESTATE);

}


/*
 * Prints the speed of a character.			-CJS-
 */
/*:::加速値を画面に表示する*/
static void prt_speed(void)
{
	int i = p_ptr->pspeed;
	bool is_fast = IS_FAST();

	byte attr = TERM_WHITE;
	char buf[32] = "";
	int wid, hgt, row_speed, col_speed;

	Term_get_size(&wid, &hgt);
	col_speed = wid + COL_SPEED;
	row_speed = hgt + ROW_SPEED;

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	/*:::探索中は減速するらしい*/
	if (p_ptr->action == ACTION_SEARCH && !p_ptr->lightspeed) i += 10;


	/*:::加速値を画面に表示する処理*/
	//v1.1.41 舞と里乃の特殊騎乗はモンスターの速度に影響されない
	if (CLASS_RIDING_BACKDANCE && p_ptr->riding)
	{
		if (i > 110)
		{
			if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
			else if ((is_fast && !p_ptr->slow) || p_ptr->lightspeed) attr = TERM_YELLOW;
			else attr = TERM_L_GREEN;
			sprintf(buf, _("ダンス(+%d)", "Dance(+%d)"), (i - 110));
		}
		else if (i < 110)
		{
			if (is_fast && !p_ptr->slow) attr = TERM_YELLOW;
			else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
			else attr = TERM_L_UMBER;
			sprintf(buf, _("ダンス(-%d)", "Dance(-%d)"), (110 - i));
		}
		else
			strcpy(buf, _("ダンス", "Dance"));

	}
	/* Fast */
	else if (i > 110)
	{
		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			if (MON_FAST(m_ptr) && !MON_SLOW(m_ptr)) attr = TERM_L_BLUE;
			else if (MON_SLOW(m_ptr) && !MON_FAST(m_ptr)) attr = TERM_VIOLET;
			else attr = TERM_GREEN;
		}
		else if ((is_fast && !p_ptr->slow) || p_ptr->lightspeed || p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0]) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_GREEN;
#ifdef JP
		sprintf(buf, "%s(+%d)", ((p_ptr->riding) ? "乗馬" : "加速"), (i - 110));
#else
		sprintf(buf, "%s(+%d)", ((p_ptr->riding) ? "Ride" : "Fast"), (i - 110));
#endif

	}

	/* Slow */
	else if (i < 110)
	{
		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			if (MON_FAST(m_ptr) && !MON_SLOW(m_ptr)) attr = TERM_L_BLUE;
			else if (MON_SLOW(m_ptr) && !MON_FAST(m_ptr)) attr = TERM_VIOLET;
			else attr = TERM_RED;
		}
		else if (is_fast && !p_ptr->slow) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_UMBER;
#ifdef JP
		sprintf(buf, "%s(-%d)", ((p_ptr->riding) ? "乗馬" : "減速"), (110 - i));
#else
		sprintf(buf, "%s(-%d)", ((p_ptr->riding) ? "Ride" : "Slow"), (110 - i));
#endif
	}
	else if (p_ptr->riding)
	{
		attr = TERM_GREEN;
#ifdef JP
		strcpy(buf, "乗馬中");
#else
		strcpy(buf, "Riding");
#endif
	}

	/* Display the speed */
	//v1.1.41 -9s→-12s
	c_put_str(attr, format("%-12s", buf), row_speed, col_speed);
}


static void prt_study(void)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	///mod150822 メルランとリリカは学習表示しない
	if (p_ptr->new_spells && p_ptr->pclass != CLASS_MERLIN && p_ptr->pclass != CLASS_LYRICA)
	{
#ifdef JP
		put_str("学習", row_study, col_study);
#else
		put_str("Stud", row_study, col_study);
#endif

	}
	else
	{
		put_str("    ", row_study, col_study);
	}
}

///class ものまね
static void prt_imitation(void)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (p_ptr->mane_num)
		{
			byte attr;
			if (new_mane) attr = TERM_L_RED;
			else attr = TERM_WHITE;
#ifdef JP
			c_put_str(attr, "まね", row_study, col_study);
#else
			c_put_str(attr, "Imit", row_study, col_study);
#endif
		}
		else
		{
			put_str("    ", row_study, col_study);
		}
	}
}


static void prt_cut(void)
{
	int c = p_ptr->cut;

	if (c > 1000)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "深刻な大怪我", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_L_RED, "Critical wound", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 200)
	{
#ifdef JP
		c_put_str(TERM_RED, "ひどい深手  ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "大怪我      ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "大変な傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 25)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどい傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 10)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "軽傷        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "かすり傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Graze       ", ROW_CUT, COL_CUT);
#endif

	}
	else
	{
		put_str("            ", ROW_CUT, COL_CUT);
	}
}


///mod140330 泥酔度が限界を超えたときも意識不明瞭扱いにすることにした
static void prt_stun(void)
{
	int s = p_ptr->stun;

	if(p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
	{
#ifdef JP
		c_put_str(TERM_RED, "意識不明瞭  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif
	}
	else if (s > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "意識不明瞭  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどく朦朧  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "朦朧        ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, COL_STUN);
#endif

	}
	else
	{
		put_str("            ", ROW_STUN, COL_STUN);
	}


	//v1.1.66 ステータス異常「破損」の描画。泥酔で倒れている場合はそちらを優先
	if (REF_BROKEN_VAL && !(p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL)))
	{
		s = REF_BROKEN_VAL;

		if(s < BROKEN_1) c_put_str(TERM_YELLOW, _("傷　        ", "Chipped     "), ROW_STUN, COL_STUN);
		else if (s < BROKEN_2) c_put_str(TERM_YELLOW, _("亀裂　        ", "Cracked     "), ROW_STUN, COL_STUN);
		else if (s < BROKEN_3) c_put_str(TERM_ORANGE, _("破損　        ", "Broken      "), ROW_STUN, COL_STUN);
		else  c_put_str(TERM_RED, _("大破　        ", "Wrecked     "), ROW_STUN, COL_STUN);

	}


}



/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
static void health_redraw(bool riding)
{
	s16b health_who;
	int row, col;
	monster_type *m_ptr;

	if (riding)
	{
		health_who = p_ptr->riding;
		row = ROW_RIDING_INFO;
		col = COL_RIDING_INFO;
	}
	else
	{
		health_who = p_ptr->health_who;
		row = ROW_INFO;
		col = COL_INFO;
	}

	m_ptr = &m_list[health_who];

	/* Not tracking */
	if (!health_who)
	{
		/* Erase the health bar */
		Term_erase(col, row, 12);
	}

	/* Tracking an unseen monster */
	else if (!m_ptr->ml)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a hallucinatory monster */
	else if (p_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a dead monster (???) */
	else if (m_ptr->hp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a visible monster */
	else
	{
		/* Extract the "percent" of health */
		int pct = 100L * m_ptr->hp / m_ptr->maxhp;
		int pct2 = 100L * m_ptr->hp / m_ptr->max_maxhp;

		/* Convert percent into "health" */
		int len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		/* Default to almost dead */
		byte attr = TERM_RED;

		/* Invulnerable */
		if (MON_INVULNER(m_ptr)) attr = TERM_WHITE;

		/* Asleep */
		else if (MON_CSLEEP(m_ptr)) attr = TERM_BLUE;

		/* Afraid */
		else if (MON_MONFEAR(m_ptr)) attr = TERM_VIOLET;

		/* Healthy */
		else if (pct >= 100) attr = TERM_L_GREEN;

		/* Somewhat Wounded */
		else if (pct >= 60) attr = TERM_YELLOW;

		/* Wounded */
		else if (pct >= 25) attr = TERM_ORANGE;

		/* Badly wounded */
		else if (pct >= 10) attr = TERM_L_RED;

		/* Default to "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");

		/* Dump the current "health" (use '*' symbols) */
		Term_putstr(col + 1, row, len, attr, "**********");
	}
}



void prt_score(void)
{
	put_str(format("Score:%lu",total_points_new(FALSE)),ROW_SCORE,COL_SCORE);

}


/*
 * Display basic info (mostly left of map)
 */
///sys メイン画面左側の情報部分の再描画
static void prt_frame_basic(void)
{
	int i;


	if(difficulty == DIFFICULTY_EASY)	c_put_str(TERM_L_GREEN, _("EASY", "Easy"), ROW_DIFFICULTY, COL_DIFFICULTY+6);
	if(difficulty == DIFFICULTY_NORMAL)	c_put_str(TERM_WHITE, _("NORMAL", "Normal"), ROW_DIFFICULTY, COL_DIFFICULTY+4);
	if(difficulty == DIFFICULTY_HARD)	c_put_str(TERM_L_BLUE, _("HARD", "Hard"), ROW_DIFFICULTY, COL_DIFFICULTY+6);
	if(difficulty == DIFFICULTY_LUNATIC)	c_put_str(TERM_VIOLET, _("LUNATIC", "Lunatic"), ROW_DIFFICULTY, COL_DIFFICULTY+3);
	if(difficulty == DIFFICULTY_EXTRA)	c_put_str(TERM_RED, _("EXTRA", "Extra"), ROW_DIFFICULTY, COL_DIFFICULTY+5);

	put_str(format("Score:%lu",total_points_new(FALSE)),ROW_SCORE,COL_SCORE);

	/* Race and Class */
	if (p_ptr->mimic_form)
		prt_field(mimic_info[p_ptr->mimic_form].title, ROW_RACE, COL_RACE);
	else
	{
		//char str[14];
		///mod150103 野良神様の種族名表記
		char str[18];
		if(p_ptr->prace == RACE_STRAYGOD)
			my_strcpy(str, deity_table[p_ptr->race_multipur_val[3]].deity_name, sizeof(str));
		else
			my_strcpy(str, rp_ptr->title, sizeof(str));
		prt_field(str, ROW_RACE, COL_RACE);
	}
/*	prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS); */
/*	prt_field(ap_ptr->title, ROW_SEIKAKU, COL_SEIKAKU); */


	/* Title */
	prt_title();

	/* Level/Experience */
	prt_level();
	prt_exp();

	/* All Stats */
	for (i = 0; i < 6; i++) prt_stat(i);

	/* Armor */
	prt_ac();

	/* Hitpoints */
	prt_hp();

	/* Spellpoints */
	prt_sp();

	/* Gold */
	prt_gold();

	/* Current depth */
	prt_depth();

	/* Special */
	health_redraw(FALSE);
	health_redraw(TRUE);
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(void)
{
	/* Cut/Stun */
	prt_cut();
	prt_stun();

	/* Food */
	prt_hunger();

	/* State */
	prt_state();

	/* Speed */
	prt_speed();

	/* Study spells */
	prt_study();

	prt_imitation();

	prt_status();
}


/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_INVEN))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display inventory */
		display_inven();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}

/*:::足元のアイテムリストをサブウィンドウに表示*/
void print_floor_item_list(int x, int y, int max_lines){
	int i,line;
	object_type *o_ptr;
	cave_type *c_ptr = &cave[y][x];
	char buf[256];
	s16b this_o_idx, next_o_idx = 0;
	char o_name[MAX_NLEN];
	byte attr = TERM_WHITE;

	if(max_lines < 3) return; //Paranoia

	Term_gotoxy(0, 0);
	if(player_bold(y,x))
		sprintf(buf, _("あなたの足元のアイテム一覧",
                        "List of items at your position"));
	else
		sprintf(buf, _("(X:%03d Y:%03d)の発見済みアイテム一覧",
                        "List of known items at (X:%03d Y:%03d)"),x,y);
	Term_addstr(-1, TERM_WHITE, buf);

	line=1;
	/*:::指定グリッドのアイテムを全てスキャン*/
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		o_ptr = &o_list[this_o_idx];
		next_o_idx = o_ptr->next_o_idx;

		/*:::未発見アイテム、金はパス*/
		if (!(o_ptr->marked & OM_FOUND)) continue;
		if (o_ptr->tval == TV_GOLD) continue;

		Term_gotoxy(0, line);

		if(line >= max_lines)
		{
			Term_addstr(-1, TERM_WHITE, "-- and more --");
			break;
		}
		else
		{
			if (p_ptr->image)
			{
				attr = TERM_WHITE;
				strcpy(buf, _("何か奇妙な物", "something strange"));
			}
			else
			{
				object_desc(o_name, o_ptr, 0);
				sprintf(buf, "%s", o_name);
				attr = tval_to_attr[o_ptr->tval % 128];
			}
			Term_addstr(-1, attr, buf);
			line++;
		}
	}
}


///mod140428
/*:::サブウィンドウに足元かターゲットグリッドのアイテムリストを表示する*/
void fix_floor_item_list(int y,int x)
{
	int j;
	int w, h;
	static bool floor_item_zero = FALSE;

	/*:::@の足元にアイテムがない状態が続いたら足元アイテムリストを更新しない*/
	if(player_bold(y,x))
	{
		if(!cave[y][x].o_idx && floor_item_zero) return;
	    else if(!cave[y][x].o_idx) floor_item_zero=TRUE;
		else floor_item_zero=FALSE;
	}
	else floor_item_zero=FALSE;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		if (!angband_term[j]) continue;
		if (!(window_flag[j] & (PW_FLOOR_ITEM_LIST))) continue;
		Term_activate(angband_term[j]);
		Term_get_size(&w, &h);
		Term_clear();
		print_floor_item_list(x, y, h);
		Term_fresh();
		Term_activate(old);
	}
}




/*
 * Print monster info in line
 * nnn X LV name
 *  nnn : number or unique(U) or wanted unique(W)
 *  X   : symbol of monster
 *  LV  : monster lv if known
 *  name: name of monster
 */
static void print_monster_line(int x, int y, monster_type* m_ptr, int n_same){
	char buf[256];
	int i;
	int r_idx = m_ptr->ap_r_idx;
	monster_race* r_ptr = &r_info[r_idx];

	Term_gotoxy(x, y);
	if(!r_ptr)return;
	//Number of 'U'nique
	if(r_ptr->flags1&RF1_UNIQUE)
	{//unique

		///del131221 賞金首処理関連
#if 0
		bool is_kubi = FALSE;
		for(i=0;i<MAX_KUBI;i++){
			if(kubi_r_idx[i] == r_idx){
				is_kubi = TRUE;
				break;
			}
		}
		Term_addstr(-1, TERM_WHITE, is_kubi?"  W":"  U");
#endif
		bool is_kubi = FALSE;
		for(i=0;i<MAX_KUBI;i++){
			if(kubi_r_idx[i] == r_idx){
				is_kubi = TRUE;
				break;
			}
		}
		Term_addstr(-1, TERM_WHITE, is_kubi? "  W" : "  U");

	}
	else if(r_ptr->flags7 & RF7_VARIABLE && r_ptr->flags7 & RF7_UNIQUE2)
	{
		Term_addstr(-1, TERM_WHITE, "  V");
	}
	else
	{
		sprintf(buf, "%3d", n_same);
		Term_addstr(-1, TERM_WHITE, buf);
	}
	//symbol
	Term_addstr(-1, TERM_WHITE, " ");
	//Term_add_bigch(r_ptr->d_attr, r_ptr->d_char);
	//Term_addstr(-1, TERM_WHITE, "/");
	Term_add_bigch(r_ptr->x_attr, r_ptr->x_char);
	//LV
	if (r_ptr->r_tkills && !(m_ptr->mflag2 & MFLAG2_KAGE)){
		sprintf(buf, " %2d", r_ptr->level);
	}else{
		strcpy(buf, " ??");
	}
	Term_addstr(-1, TERM_WHITE, buf);
	//name

	if(IS_RANDOM_UNIQUE_IDX(m_ptr->r_idx))
		sprintf(buf, " %s ", random_unique_name[m_ptr->r_idx - MON_RANDOM_UNIQUE_1]);
	else
		sprintf(buf, " %s ", r_name+r_ptr->name);

	Term_addstr(-1, TERM_WHITE, buf);

	//Term_addstr(-1, TERM_WHITE, look_mon_desc(m_ptr, 0));
}

 /*
	max_lines : 最大何行描画するか．
*/
/*:::モンスターのリストをサブウィンドウに表示*/
/*:::モンスターリストはtarget_set_prepare()によってtemp_x[],temp_y[],temp_nに格納されていること*/
void print_monster_list(int x, int y, int max_lines){
	int line = y;
	monster_type* last_mons = NULL;
	monster_type* m_ptr = NULL;
	int n_same = 0;
	int i;

	for(i=0;i<temp_n;i++){
		cave_type* c_ptr = &cave[temp_y[i]][temp_x[i]];
		if(!c_ptr->m_idx || !m_list[c_ptr->m_idx].ml)continue;//no mons or cannot look
		m_ptr = &m_list[c_ptr->m_idx];
		if(is_pet(m_ptr))continue;//pet
		if(!m_ptr->r_idx)continue;//dead?
		{
			/*
			int r_idx = m_ptr->ap_r_idx;
			monster_race* r_ptr = &r_info[r_idx];
			cptr name = (r_name + r_ptr->name);
			cptr ename = (r_name + r_ptr->name);
			//ミミック類や「それ」等は、一覧に出てはいけない
			if(r_ptr->flags1&RF1_CHAR_CLEAR)continue;
			if((r_ptr->flags1&RF1_NEVER_MOVE)&&(r_ptr->flags2&RF2_CHAR_MULTI))continue;
			//『ヌル』は、一覧に出てはいけない
			if((strcmp(name, "生ける虚無『ヌル』")==0)||
			   (strcmp(ename, "Null the Living Void")==0))continue;
			//"金無垢の指輪"は、一覧に出てはいけない
			if((strcmp(name, "金無垢の指輪")==0)||
				(strcmp(ename, "Plain Gold Ring")==0))continue;
			*/
		}

		//ソート済みなので同じモンスターは連続する．これを利用して同じモンスターをカウント，まとめて表示する．
		if(!last_mons){//先頭モンスター
			last_mons = m_ptr;
			n_same = 1;
			continue;
		}
		//same race?
		if(last_mons->ap_r_idx == m_ptr->ap_r_idx){
			n_same++;
			continue;//表示処理を次に回す
		}
		//print last mons info
		print_monster_line(x, line++, last_mons, n_same);
		n_same = 1;
		last_mons = m_ptr;
		if(line-y-1==max_lines){//残り1行
			break;
		}
	}
	if(line-y-1==max_lines && i!=temp_n){
		Term_gotoxy(x, line);
		Term_addstr(-1, TERM_WHITE, "-- and more --");
	}else{
		if(last_mons)print_monster_line(x, line++, last_mons, n_same);
	}


	//v1.1.66 特定条件のグリッドをカウントするtemp_n,temp_x[],temp_y[]をリセットしておく
	temp_n = 0;



}
/*
 * Hack -- display monster list in sub-windows
 */
/*:::サブウィンドウにモンスターリストを表示する*/
static void fix_monster_list(void)
{
	int j;
	int w, h;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MONSTER_LIST))) continue;

		/* Activate */
		Term_activate(angband_term[j]);
		Term_get_size(&w, &h);

		Term_clear();

		//モンスター一覧を生成，ソート
		if(full_mon_list)
			target_set_prepare_full();
		else
			target_set_prepare_look();

		print_monster_list(0, 0, h);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}



}




/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_EQUIP))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display equipment */
		display_equip();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display equipment in sub-windows
 */
static void fix_spell(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_SPELL))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display spell list */
		display_spell_list();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display character in sub-windows
 */
/*:::サブウィンドウにステータス画面（Cコマンドの一番最初の画面）を表示する*/
static void fix_player(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_PLAYER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		update_playtime();

		/* Display player */
		display_player(0);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display recent messages in sub-windows
 *
 * XXX XXX XXX Adjust for width and split messages
 */
/*:::サブウィンドウにメッセージを表示する*/
static void fix_message(void)
{
	int j, i;
	int w, h;
	int x, y;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MESSAGE))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Get size */
		Term_get_size(&w, &h);

		/* Dump messages */
		for (i = 0; i < h; i++)
		{
			/* Dump the message on the appropriate line */
			Term_putstr(0, (h - 1) - i, -1, (byte)((i < now_message) ? TERM_WHITE : TERM_SLATE), message_str((s16b)i));

			/* Cursor */
			Term_locate(&x, &y);

			/* Clear to end of line */
			Term_erase(x, y, 255);
		}

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
/*:::サブウィンドウにダンジョン全体図を表示する*/
static void fix_overhead(void)
{
	int j;

	int cy, cx;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;
		int wid, hgt;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OVERHEAD))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Full map in too small window is useless  */
		Term_get_size(&wid, &hgt);
		if (wid > COL_MAP + 2 && hgt > ROW_MAP + 2)
		{
			/* Redraw map */
			/*:::サブウィンドウにミニマップを表示する。cx,cyはミニマップ内での＠の位置を返すがここでは使わないダミー*/
			display_map(&cy, &cx);

			/* Fresh */
			Term_fresh();
		}

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display dungeon view in sub-windows
 */
/*:::サブウィンドウに周囲の地形を表示*/
static void fix_dungeon(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_DUNGEON))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw dungeon view */
		display_dungeon();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display monster recall in sub-windows
 */
/*:::サブウィンドウにモンスターの思い出を表示する*/
static void fix_monster(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MONSTER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->monster_race_idx) display_roff(p_ptr->monster_race_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display object recall in sub-windows
 */
/*:::サブウィンドウにアイテムの詳細を表示する。ただし現在は魔法書限定らしい*/
static void fix_object(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OBJECT))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->object_kind_idx) display_koff(p_ptr->object_kind_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
/*:::覚えている魔法を計算し、忘れたり思い出したりしたとき記述する*/
///realm class
static void calc_spells(void)
{
	int			i, j, k, levels;
	int			num_allowed;
	int         num_boukyaku = 0;

	magic_type	*s_ptr;
	magic_type forge;
	int which;
	int bonus = 0;
	cptr p;

	s_ptr = &forge;

	/* Hack -- must be literate */
	//if (!mp_ptr->spell_book) return;
	if (cp_ptr->realm_aptitude[0] == 0 && p_ptr->pclass != CLASS_SAMURAI && p_ptr->pclass != CLASS_YOUMU) return;

	//リリカとメルラン特殊処理
	if (p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA) return;

	/* Hack -- wait for creation */
	if (!character_generated) return;

	/* Hack -- handle "xtra" mode */
	/*:::初期化中フラグ？*/
	if (character_xtra) return;
	///class 赤魔とスペマスは魔法習得に関係ない
	//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
	if (REALM_SPELLMASTER)
	{
		p_ptr->new_spells = 0;
		return;
	}
	//v1.1.60 赤蛮奇は専用性格「スマホ首」以外魔法習得できない
	else if (p_ptr->pclass == CLASS_BANKI && !is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
	{
		p_ptr->new_spells = 0;
		return;

	}



	//p = spell_category_name(mp_ptr->spell_book);
	p = spell_category_name(p_ptr->realm1);

	/* Determine the number of spells allowed */
	//levels = p_ptr->lev - mp_ptr->spell_first + 1;
	/*:::魔法習得開始レベルを厳密に計算するのは難しいので、ここでは「純魔法職以外はLv3以降」にしておく。*/
	levels = p_ptr->lev;
	if(!cp_ptr->magicmaster ) levels -= 2;
	if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU ) levels = 1;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[p_ptr->stat_ind[cp_ptr->spell_stat]] * levels / 2);

	//if ((p_ptr->pclass != CLASS_SAMURAI) && (mp_ptr->spell_book != TV_LIFE_BOOK))
	if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_MARISA)
	{
		bonus = 10;
	}
	else if (p_ptr->pclass != CLASS_SAMURAI && p_ptr->pclass != CLASS_YOUMU)
	{
		bonus = 4;
	}

	if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
	{
		num_allowed = 32;
	}
	else if (p_ptr->pclass == CLASS_MARISA)
	{
		if (num_allowed>(150+bonus)) num_allowed = 150+bonus;
	}
	else if (p_ptr->realm2 == REALM_NONE)
	{
		num_allowed = (num_allowed+1)/2;
		if (num_allowed>(32+bonus)) num_allowed = 32+bonus;
	}
	else if ((p_ptr->pclass == CLASS_MAGE) || (p_ptr->pclass == CLASS_PRIEST))
	{
		if (num_allowed>(96+bonus)) num_allowed = 96+bonus;
	}
	else
	{
		if (num_allowed>(80+bonus)) num_allowed = 80+bonus;
	}

	/* Count the number of spells we know */
	for (j = 0; j < 64; j++)
	{
		/* Count known spells */
		if ((j < 32) ?
		    (p_ptr->spell_forgotten1 & (1L << j)) :
		    (p_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			num_boukyaku++;
		}
	}

	/* See how many spells we must forget or may learn */
	p_ptr->new_spells = num_allowed + p_ptr->add_spells + num_boukyaku - p_ptr->learned_spells;

	/* Forget spells which are too hard */
	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;

		/* Access the spell */
		j = p_ptr->spell_order[i];

		/* Skip non-spells */
		if (j >= 99) continue;


		/* Get the spell */
		/*
		if (!is_magic((j < 32) ? p_ptr->realm1 : p_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[p_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		*/
		if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) s_ptr = &hissatsu_info[j];
		else if (j < 32)
	//		s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];
			calc_spell_info(s_ptr,p_ptr->realm1,j);
		else
			//s_ptr = &mp_ptr->info[p_ptr->realm2-1][j%32];
			calc_spell_info(s_ptr,p_ptr->realm2,j%32);

		/* Skip spells we are allowed to know */
		if (s_ptr->slevel <= p_ptr->lev) continue;

		/* Is it known? */
		if ((j < 32) ?
		    (p_ptr->spell_learned1 & (1L << j)) :
		    (p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%sの%sを忘れてしまった。",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have forgotten the %s of %s.", p,
			do_spell(which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if (p_ptr->new_spells >= 0) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) continue;

		/* Forget it (if learned) */
		if ((j < 32) ?
		    (p_ptr->spell_learned1 & (1L << j)) :
		    (p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%sの%sを忘れてしまった。",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have forgotten the %s of %s.", p,
				   do_spell(which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Check for spells to remember */
	for (i = 0; i < 64; i++)
	{
		/* None left to remember */
		if (p_ptr->new_spells <= 0) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_forgotten1 && !p_ptr->spell_forgotten2) break;

		/* Get the next spell we learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) break;

		/* Access the spell */
		/*
		if (!is_magic((j < 32) ? p_ptr->realm1 : p_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[p_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		*/
		if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) s_ptr = &hissatsu_info[j];
		else if (j<32)
			//s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];
			calc_spell_info(s_ptr,p_ptr->realm1,j);
		else
			//s_ptr = &mp_ptr->info[p_ptr->realm2-1][j%32];
			calc_spell_info(s_ptr,p_ptr->realm2,j%32);

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > p_ptr->lev) continue;

		/* First set of spells */
		if ((j < 32) ?
		    (p_ptr->spell_forgotten1 & (1L << j)) :
		    (p_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Known once more */
			if (j < 32)
			{
				p_ptr->spell_learned1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%sの%sを思い出した。",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have remembered the %s of %s.",
				   p, do_spell(which, j%32, SPELL_NAME));
#endif


			/* One less can be learned */
			p_ptr->new_spells--;
		}
	}

	k = 0;

	if (p_ptr->realm2 == REALM_NONE)
	{
		/* Count spells that can be learned */
		for (j = 0; j < 32; j++)
		{
			//if (!is_magic(p_ptr->realm1)) s_ptr = &technic_info[p_ptr->realm1-MIN_TECHNIC][j];
			//else s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];
			if(p_ptr->realm1 == TV_BOOK_HISSATSU) s_ptr = &hissatsu_info[j];
			else	calc_spell_info(s_ptr,p_ptr->realm1,j);

			/* Skip spells we cannot remember */
			if (s_ptr->slevel > p_ptr->lev) continue;

			/* Skip spells we already know */
			if (p_ptr->spell_learned1 & (1L << j))
			{
				continue;
			}

			/* Count it */
			k++;
		}
		if (k > 32) k = 32;
	//	if ((p_ptr->new_spells > k) && ((mp_ptr->spell_book == TV_LIFE_BOOK) || (mp_ptr->spell_book == TV_HISSATSU_BOOK))) p_ptr->new_spells = k;
		if ((p_ptr->new_spells > k) && (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)) p_ptr->new_spells = k;
	}

	if (p_ptr->new_spells < 0) p_ptr->new_spells = 0;

	/* Spell count changed */
	if (p_ptr->old_spells != p_ptr->new_spells)
	{
		/* Message if needed */
		if (p_ptr->new_spells)
		{
			/* Message */
#ifdef JP
			if( p_ptr->new_spells < 10 ){
				msg_format("あと %d つの%sを学べる。", p_ptr->new_spells, p);
			}else{
				msg_format("あと %d 個の%sを学べる。", p_ptr->new_spells, p);
			}
#else
			msg_format("You can learn %d more %s%s.",
				   p_ptr->new_spells, p,
				   (p_ptr->new_spells != 1) ? "s" : "");
#endif

		}

		/* Save the new_spells value */
		p_ptr->old_spells = p_ptr->new_spells;

		/* Redraw Study Status */
		p_ptr->redraw |= (PR_STUDY);

		/* Redraw object recall */
		p_ptr->window |= (PW_OBJECT);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
/*:::最大MPを計算する。不適切な装備のMP減少もここ*/
	///class seikaku MP計算
///mod140208 どの職でもMPを持つようにしてみる。魔法習得開始Lvに合わせるのは諦めた。
///mod160503 cumber_gloveの判定ルーチンをcalc_bonuses()に移動した

static void calc_mana(void)
{
	int		msp, levels, cur_wgt, max_wgt;
	int plev = p_ptr->lev;
	int mana_rate = cp_ptr->mana_rate;

	//object_type	*o_ptr;
	int lev_dec = 0;


	if (is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI) )mana_rate++;

	switch (mana_rate)
	{
	case 1:
		levels = p_ptr->lev - 4;
		break;
	case 2:
		levels = p_ptr->lev - 4;
		break;
	case 3:
		levels = p_ptr->lev - 2;
		break;
	default:
		levels = p_ptr->lev;
		break;
	}

	if(p_ptr->pclass == CLASS_SUWAKO && levels < 1) levels = 1;

	if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_MARTIAL_ARTIST) levels = 1;

	//v1.1.86 MP適性D以上の＠はレベル1でもMPがあるようにした
	if(levels < 1 && mana_rate <= 1)
	{
		/* Save new mana */
		p_ptr->msp = 0;
			/* Display mana later */
		p_ptr->redraw |= (PR_MANA);
		return;
	}

	/* Extract "effective" player level */
	//levels = (p_ptr->lev - mp_ptr->spell_first) + 1;

	/*:::MP補正は常に賢さを使い、種族だけでなくクラスの値との平均で計算するようにした*/
	if(mana_rate == 1)
	{
		msp = 10 + (adj_mag_mana[p_ptr->stat_ind[A_WIS]] ) * 2;
		if (msp) msp += (msp * (rp_ptr->r_adj[A_WIS] + cp_ptr->c_adj[A_WIS]) / 40);
	}
	else
	{
		/* Extract total mana */
		msp = 15 + MAX((adj_mag_mana[p_ptr->stat_ind[A_WIS]] * (levels+3) / 4),0);
		/* Hack -- usually add one mana */
		if (msp) msp++;
		if (msp) msp += (msp * (rp_ptr->r_adj[A_WIS] + cp_ptr->c_adj[A_WIS]) / 40);

		//v1.1.86 MP適性の低い職の低レベル時MPに調整
		if (levels < 1) msp = msp * 3 / (4-levels);

		if(mana_rate == 2) msp -= msp / 5;
		if(mana_rate == 3) msp -= msp / 10;
		if(mana_rate == 5) msp += msp / 4;


		//if (msp && (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)) msp += msp/2;
		/* Hack: High mages have a 25% mana bonus */
		//if (msp && (p_ptr->pclass == CLASS_HIGH_MAGE)) msp += msp / 4;
		//if (msp && (p_ptr->pclass == CLASS_SORCERER)) msp += msp*(25+p_ptr->lev)/100;
	}

	///mod160503 cumber_gloveの判定ルーチンをcalc_bonuses()に移動した

	///mod150122 慧音特殊処理
	if(p_ptr->pclass == CLASS_KEINE && p_ptr->magic_num1[0]) msp += p_ptr->lev * 4;
	//霊夢特殊処理
	if(p_ptr->pclass == CLASS_REIMU ) msp += osaisen_rank() * osaisen_rank() * p_ptr->lev / 20;

	//成美特殊処理
	if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
		msp += msp / 4;


	//v1.1.32 パチュリー特殊性格
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
	{
		msp += msp*(25 + p_ptr->lev/2) / 100;
	}


	//正邪特殊処理 不思議道具の使用回数をここで決定する
	if(p_ptr->pclass == CLASS_SEIJA)
	{
		p_ptr->magic_num2[SEIJA_ITEM_HIRARI] = (plev > 4) ? (plev / 2): 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_HIRARI] > p_ptr->magic_num2[SEIJA_ITEM_HIRARI]) p_ptr->magic_num1[SEIJA_ITEM_HIRARI] = p_ptr->magic_num2[SEIJA_ITEM_HIRARI];

		p_ptr->magic_num2[SEIJA_ITEM_SUKIMA] = (plev > 8) ? (2 + (plev-7) / 4) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_SUKIMA] > p_ptr->magic_num2[SEIJA_ITEM_SUKIMA]) p_ptr->magic_num1[SEIJA_ITEM_SUKIMA] = p_ptr->magic_num2[SEIJA_ITEM_SUKIMA];

		p_ptr->magic_num2[SEIJA_ITEM_DECOY] = (plev > 12) ? (1 + (plev-10) / 5) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_DECOY] > p_ptr->magic_num2[SEIJA_ITEM_DECOY]) p_ptr->magic_num1[SEIJA_ITEM_DECOY] = p_ptr->magic_num2[SEIJA_ITEM_DECOY];

		p_ptr->magic_num2[SEIJA_ITEM_BOMB] = (plev > 15) ? (1 + (plev-12) / 7) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_BOMB] > p_ptr->magic_num2[SEIJA_ITEM_BOMB]) p_ptr->magic_num1[SEIJA_ITEM_BOMB] = p_ptr->magic_num2[SEIJA_ITEM_BOMB];

		p_ptr->magic_num2[SEIJA_ITEM_CAMERA] = (plev > 23) ? (7 + (plev-24) / 3) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_CAMERA] > p_ptr->magic_num2[SEIJA_ITEM_CAMERA]) p_ptr->magic_num1[SEIJA_ITEM_CAMERA] = p_ptr->magic_num2[SEIJA_ITEM_CAMERA];

		p_ptr->magic_num2[SEIJA_ITEM_ONMYOU] = (plev > 27) ? (3 + (plev-25) / 6) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_ONMYOU] > p_ptr->magic_num2[SEIJA_ITEM_ONMYOU]) p_ptr->magic_num1[SEIJA_ITEM_ONMYOU] = p_ptr->magic_num2[SEIJA_ITEM_ONMYOU];

		p_ptr->magic_num2[SEIJA_ITEM_KODUCHI] = (plev > 31) ? (3 + (plev-30) / 5) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_KODUCHI] > p_ptr->magic_num2[SEIJA_ITEM_KODUCHI]) p_ptr->magic_num1[SEIJA_ITEM_KODUCHI] = p_ptr->magic_num2[SEIJA_ITEM_KODUCHI];

		p_ptr->magic_num2[SEIJA_ITEM_JIZO] = (plev > 35) ? (1 + (plev-35) / 6) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_JIZO] > p_ptr->magic_num2[SEIJA_ITEM_JIZO]) p_ptr->magic_num1[SEIJA_ITEM_JIZO] = p_ptr->magic_num2[SEIJA_ITEM_JIZO];

		p_ptr->magic_num2[SEIJA_ITEM_CHOCHIN] = (plev > 39) ? (1 + (plev-40) / 5) : 0;
		if(p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN] > p_ptr->magic_num2[SEIJA_ITEM_CHOCHIN]) p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN] = p_ptr->magic_num2[SEIJA_ITEM_CHOCHIN];


	}

	if(p_ptr->pclass == CLASS_SUWAKO)
	{
		if(msp > 20) msp = 20 + (msp-20) / 3;
		msp += p_ptr->magic_num1[0] / 100;
	}
	//v1.1.21 兵士
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_MANA_PLUS))
	{
		msp += msp / 2;
	}



	/* Assume player not encumbered by armor */
	//p_ptr->cumber_armor = FALSE;
	p_ptr->heavy_cast = 0;
	///item MP減少処理 TVAL
	/* Weigh the armor */
	cur_wgt = 0;
	///mod140315 装備重量のMP減少処理をやめ、コストが最大倍まで上昇するようにした。
	cur_wgt += inventory[INVEN_RARM].weight / 2;
	cur_wgt += inventory[INVEN_LARM].weight / 2;
	cur_wgt += inventory[INVEN_RIBBON].weight;
	cur_wgt += inventory[INVEN_BODY].weight;
	cur_wgt += inventory[INVEN_HEAD].weight;
	cur_wgt += inventory[INVEN_OUTER].weight;
	cur_wgt += inventory[INVEN_HANDS].weight;
	cur_wgt += inventory[INVEN_FEET].weight;

	if(cp_ptr->heavy_cast_mult!=0)
	{
		max_wgt = 100 + p_ptr->lev * 2 + ( adj_str_wgt[p_ptr->stat_ind[A_STR]] * 2 ) * cp_ptr->heavy_cast_mult;
		///mod140813 パチュリーは特別重い装備が苦手
		if(p_ptr->pclass == CLASS_PATCHOULI) max_wgt = max_wgt * 2 / 3;
		///mod151205 三月精は装備限界重量が大きい
		if (p_ptr->pclass == CLASS_3_FAIRIES)
		{
			if(is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
				max_wgt *= 6;
			else
				max_wgt *= 3;
		}

		if(cur_wgt > max_wgt) p_ptr->heavy_cast = (cur_wgt - max_wgt) * 100 / max_wgt;
		if( p_ptr->heavy_cast > 100) p_ptr->heavy_cast = 100;
	}

	//v1.1.80 座敷わらしが「古のものの水晶」に住んでいるとMP+100
	if (p_ptr->prace == RACE_ZASHIKIWARASHI)
	{
		object_type *o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_ELDER_THINGS_CRYSTAL)
		{
			msp += 100;
		}
	}

	//v1.1.86 アビリティカードによるMP増加 一部のMP低い戦士とかには無効
	if(mana_rate >= 2)
	{
		int card_num;

		card_num = count_ability_card(ABL_CARD_SPELL);
		if (card_num)
		{
			msp += calc_ability_card_add_hp_sp(ABL_CARD_SPELL, card_num);
		}


	}

#if 0
	/* Subtract a percentage of maximum mana. */
	///class 装備重量によるMP減少
	switch (p_ptr->pclass)
	{
		/* For these classes, mana is halved if armour
		 * is 30 pounds over their weight limit. */
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_BLUE_MAGE:
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_SORCERER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight;
			break;
		}

		/* Mana halved if armour is 40 pounds over weight limit. */
		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_TOURIST:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight*2/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight*2/3;
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_BEASTMASTER:
		case CLASS_MIRROR_MASTER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/2;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/2;
			break;
		}

		/* Mana halved if armour is 50 pounds over weight limit. */
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_RED_MAGE:
		case CLASS_WARRIOR_MAGE:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/3;
			break;
		}

		/* Mana halved if armour is 60 pounds over weight limit. */
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/5;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/5;
			break;
		}

		/* For new classes created, but not yet added to this formula. */
		default:
		{
			break;
		}
	}
	/* Determine the weight allowance */
	max_wgt = mp_ptr->spell_weight;

	/* Heavy armor penalizes mana by a percentage.  -LM- */
	if ((cur_wgt - max_wgt) > 0)
	{
		/* Encumbered */
		p_ptr->cumber_armor = TRUE;



		/* Subtract a percentage of maximum mana. */
		switch (p_ptr->pclass)
		{
			/* For these classes, mana is halved if armour
			 * is 30 pounds over their weight limit. */
			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_BLUE_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 600;
				break;
			}

			/* Mana halved if armour is 40 pounds over weight limit. */
			case CLASS_PRIEST:
			case CLASS_MINDCRAFTER:
			case CLASS_BEASTMASTER:
			case CLASS_BARD:
			case CLASS_FORCETRAINER:
			case CLASS_TOURIST:
			case CLASS_MIRROR_MASTER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}

			case CLASS_SORCERER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 900;
				break;
			}

			/* Mana halved if armour is 50 pounds over weight limit. */
			case CLASS_ROGUE:
			case CLASS_RANGER:
			case CLASS_MONK:
			case CLASS_RED_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1000;
				break;
			}

			/* Mana halved if armour is 60 pounds over weight limit. */
			case CLASS_PALADIN:
			case CLASS_CHAOS_WARRIOR:
			case CLASS_WARRIOR_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1200;
				break;
			}

			case CLASS_SAMURAI:
			{
				p_ptr->cumber_armor = FALSE;
				break;
			}

			/* For new classes created, but not yet added to this formula. */
			default:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}

		}
	}
#endif
	/* Mana can never be negative */
	if (msp < 0) msp = 0;
	if(msp > 999) msp = 999;


	/* Maximum mana has changed */
	if (p_ptr->msp != msp)
	{
		/* Enforce maximum */
		if ((p_ptr->csp >= msp) && (p_ptr->pclass != CLASS_SAMURAI))
		{
			p_ptr->csp = msp;
			p_ptr->csp_frac = 0;
		}

#ifdef JP
		/* レベルアップの時は上昇量を表示する */
		if ((level_up == 1) && (msp > p_ptr->msp))
		{
			msg_format("最大マジック・ポイントが %d 増加した！",
				   (msp - p_ptr->msp));
		}
#else
		if ((level_up == 1) && (msp > p_ptr->msp))
		{
			msg_format("Maximum MP increased by %d!", (msp - p_ptr->msp));
		}
#endif
		/* Save new mana */
		p_ptr->msp = msp;

		/* Display mana later */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}


	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;


	/* Take note when "armor state" changes */
	if (!p_ptr->old_cumber_armor && p_ptr->heavy_cast || p_ptr->old_cumber_armor && !p_ptr->heavy_cast)
	{
		/* Message */
		if (p_ptr->heavy_cast)
		{
#ifdef JP
			msg_print("装備の重さで呪文が唱えにくい感じがする。");
#else
			msg_print("The weight of your equipment makes it difficult to cast spells.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("ぐっと楽に体を動かせるようになった。");
#else
			msg_print("You feel able to move more freely.");
#endif

		}

		/* Save it */
		if(p_ptr->heavy_cast) p_ptr->old_cumber_armor = TRUE;
		else p_ptr->old_cumber_armor = FALSE;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
/*:::最大HPを計算する　スペマスと狂戦士の補正などもここで行う*/
static void calc_hitpoints(void)
{
	int bonus, mhp;
	byte tmp_hitdie;

	/* Un-inflate "half-hitpoint bonus per level" value */
	bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_CON]]) - 128) * p_ptr->lev / 4;

	/* Calculate hitpoints */
	mhp = p_ptr->player_hp[p_ptr->lev - 1];


	///class スペマスと変身時HP計算
	//v1.1.32 パチュリー特殊性格追加
	if (p_ptr->mimic_form)
	{
		if (p_ptr->pclass == CLASS_SORCERER
			 || is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
			tmp_hitdie = mimic_info[p_ptr->mimic_form].r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
		else
			tmp_hitdie = mimic_info[p_ptr->mimic_form].r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;
		mhp = mhp * tmp_hitdie / p_ptr->hitdie;
	}

	//v1.1.86 アビリティカードによるHP増加
	{
		int add_hp1 = 0, add_hp2 = 0, card_num;

		card_num = count_ability_card(ABL_CARD_LIFE);
		if (card_num)
		{
			add_hp1 = calc_ability_card_add_hp_sp(ABL_CARD_LIFE, card_num);
		}
		card_num = count_ability_card(ABL_CARD_PHOENIX);
		if (card_num)
		{
			add_hp2 = calc_ability_card_add_hp_sp(ABL_CARD_PHOENIX, card_num);
		}

		mhp += MAX(add_hp1, add_hp2);
	}




	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
	{
		mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
		mhp += 10;
	}

	else if (p_ptr->pclass == CLASS_SORCERER)
	{
		if (p_ptr->lev < 30)
			mhp = (mhp * (45+p_ptr->lev) / 100);
		else
			mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
	}
	if(prace_is_(RACE_STRAYGOD))
	{
		if(p_ptr->race_multipur_val[3] == 42) bonus += 50;
		if(p_ptr->race_multipur_val[3] == 43) bonus += 100;
		if(p_ptr->race_multipur_val[3] == 47) bonus += 50;
		if(p_ptr->race_multipur_val[3] == 48) bonus += 150;
		if(p_ptr->race_multipur_val[3] == 51) bonus += 100;
		if(p_ptr->race_multipur_val[3] == 63) bonus += 100;
	}

	if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->tim_general[0]) bonus += 50 + p_ptr->lev;

	if(p_ptr->pclass == CLASS_MERLIN) bonus += (p_ptr->lev + 5) * 2; //メルランにHD+4相当の補正

	mhp += bonus;

	//ネムノは縄張りにいるときパワーアップ HD+4相当
	if(IS_NEMUNO_IN_SANCTUARY)
	{
		mhp += (p_ptr->lev + 5) * 2;
	}

	///class 狂戦士HP計算
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		mhp = mhp*(110+(((p_ptr->lev + 40) * (p_ptr->lev + 40) - 1550) / 110))/100;
	}
	else if(p_ptr->pclass == CLASS_SEIJA)
	{
		mhp = mhp * 6 / 7;
	}

	/* Always have at least one hitpoint per level */
	if (mhp < p_ptr->lev + 1) mhp = p_ptr->lev + 1;

	/* Factor in the hero / superhero settings */
	if (IS_HERO()) mhp += 10;
	if (p_ptr->shero && ( p_ptr->pseikaku != SEIKAKU_BERSERK)) mhp += 30;
	if (p_ptr->tsuyoshi) mhp += 50;

	///class 呪術
	/* Factor in the hex spell settings */
	if (hex_spelling(HEX_XTRA_MIGHT)) mhp += 15;
	if (hex_spelling(HEX_BUILDING)) mhp += 60;
	//ドラゴン変身時に強力なボーナス
	if(p_ptr->mimic_form == MIMIC_DRAGON)
	{
		int rank = p_ptr->mimic_dragon_rank;
		int tmp_bonus;
		if(rank > 159) tmp_bonus = rank * 50;
		if(rank > 144) tmp_bonus = rank * 40;//ここelseつけるべきだった気がするがまあいいか
		else if(rank > 130) tmp_bonus = rank * 30;
		else tmp_bonus = rank *20;

		if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
			tmp_bonus /= 2;

		mhp += tmp_bonus;


	}
	//v1.1.21 兵士白兵最終段階でHP+10%
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_LETHAL_WEAPON))
		mhp += mhp / 10;



	if(p_ptr->pclass == CLASS_REIMU && !p_ptr->mimic_form) mhp += osaisen_rank() * osaisen_rank() * p_ptr->lev / 10;
	if(p_ptr->pclass == CLASS_KANAKO && !p_ptr->mimic_form) mhp += kanako_rank() * p_ptr->lev;

	//ドレミーなどのモンスター変身
	if(IS_METAMORPHOSIS)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		if(r_ptr->flags1 & RF1_FORCE_MAXHP)
			mhp = r_ptr->hdice * r_ptr->hside;
		else
			mhp = r_ptr->hdice * (r_ptr->hside+1) / 2;

	}

	if (SUPER_SHION)
	{
		mhp += p_ptr->lev * 5;
	}




	//純狐の純化判定..やめた
	/*
	if(p_ptr->pclass == CLASS_JUNKO)
	{
		int i;
		bool flag_pure = TRUE;
		for(i=INVEN_RARM;i<=INVEN_FEET;i++)
		{
			if(inventory[i].k_idx && inventory[i].xtra3 != SPECIAL_ESSENCE_OTHER)
			{
				flag_pure = FALSE;
				break;
			}
		}
		if(flag_pure) mhp *= 2;
	}
	*/

	if(mhp > 9999) mhp = 9999;

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		mhp = CLOWNPIECE_MAX_HP - p_ptr->magic_num1[0];
		if(mhp < 0) mhp=0;
	}

	/* New maximum hitpoints */
	if (p_ptr->mhp != mhp)
	{
		/* Enforce maximum */
		if (p_ptr->chp >= mhp)
		{
			p_ptr->chp = mhp;
			p_ptr->chp_frac = 0;
		}

#ifdef JP
		/* レベルアップの時は上昇量を表示する */
		if ((level_up == 1) && (mhp > p_ptr->mhp))
		{
			msg_format("最大ヒット・ポイントが %d 増加した！",
				   (mhp - p_ptr->mhp) );
		}
#endif
		/* Save the new max-hitpoints */
		p_ptr->mhp = mhp;

		/* Display hitpoints (later) */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 *
 * SWD: Experimental modification: multiple light sources have additive effect.
 *
 */
/*:::光源範囲を計算*/
///item 光源範囲計算
///mod131223 TVAL,ego
static void calc_torch(void)
{
	int i;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Assume no light */
	p_ptr->cur_lite = 0;

	///mod140420
	if(p_ptr->tim_unite_darkness) return;

	//v1.1.59 サニーは溜め込んだ日光により自前光源
	if (p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_3_FAIRIES)
	{
		if (p_ptr->magic_num1[0]) p_ptr->cur_lite += 1 + p_ptr->magic_num1[0] / 1000;
	}

	///mod141208 リグルとお空に自前光源
	if(p_ptr->pclass == CLASS_WRIGGLE || p_ptr->pclass == CLASS_UTSUHO) p_ptr->cur_lite += 1 + p_ptr->lev / 12;

	if(p_ptr->pclass == CLASS_KAGUYA) p_ptr->cur_lite += p_ptr->lev / 10;

	//依神女苑の特殊インベントリに入っている指輪の処理
	if (p_ptr->pclass == CLASS_JYOON)
	{
		if( p_ptr->lev > 14)
			p_ptr->cur_lite += p_ptr->lev / 15;

		for (i = 0; i < INVEN_ADD_MAX; i++)
		{
			o_ptr = &inven_add[i];

			if (check_invalidate_inventory(INVEN_RIGHT))continue;
			if (!o_ptr->k_idx) continue;
			object_flags(o_ptr, flgs);

			if (have_flag(flgs, TR_LITE))	p_ptr->cur_lite++;

		}

	}

	if( check_activated_nameless_arts(JKF1_SELF_LIGHT)) p_ptr->cur_lite += 5; //純狐発光

	/* Loop through all wielded items */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		if(check_invalidate_inventory(i))continue;

		/* Examine actual lites */
		if ((i == INVEN_LITE) && (o_ptr->k_idx) && (o_ptr->tval == TV_LITE))
		{
			if (o_ptr->name2 == EGO_LITE_DARKNESS)
			{
				if (o_ptr->sval == SV_LITE_TORCH)
				{
					p_ptr->cur_lite -= 1;
				}

				/* Lanterns (with fuel) provide more lite */
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
					p_ptr->cur_lite -= 2;
				}

				else if (o_ptr->sval == SV_LITE_FEANOR)
				{
					p_ptr->cur_lite -= 3;
				}
			}
			/* Torches (with fuel) provide some lite */
			else if ((o_ptr->sval == SV_LITE_TORCH) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 1;
			}
			else if(o_ptr->name1 == ART_TRAPEZOHEDRON)
			{
				p_ptr->cur_lite -= 4;
			}

			/* Artifact Lites provide permanent, bright, lite */
			//v.1.15 fixedからアーティファクト全てに変更
			else if (object_is_artifact(o_ptr))
			{
				//v1.1.53 クラウンピースの松明が充填中の間は明かりが消える
				if (o_ptr->name1 == ART_CLOWNPIECE && o_ptr->timeout);
				else p_ptr->cur_lite += 3;
			}

			/* Lanterns (with fuel) provide more lite */
			else if ((o_ptr->sval == SV_LITE_LANTERN) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 2;
			}

			else if (o_ptr->sval == SV_LITE_FEANOR)
			{
				p_ptr->cur_lite += 2;
			}


			if (o_ptr->name2 == EGO_LITE_BRIGHT) p_ptr->cur_lite++;
		}
		else
		{
			/* Skip empty slots */
			if (!o_ptr->k_idx) continue;

			/* Extract the flags */
			object_flags(o_ptr, flgs);

			/* does this item glow? */
			if (have_flag(flgs, TR_LITE))
			{
				if ((o_ptr->name1 == ART_NIGHT)) p_ptr->cur_lite--;
				else p_ptr->cur_lite++;
			}
		}

	}

	/* max radius is 14 (was 5) without rewriting other code -- */
	/* see cave.c:update_lite() and defines.h:LITE_MAX */
	if (d_info[dungeon_type].flags1 & DF1_DARKNESS && p_ptr->cur_lite > 1)
		p_ptr->cur_lite = 1;

	/*
	 * check if the player doesn't have light radius,
	 * but does weakly glow as an intrinsic.
	 */
	if (p_ptr->cur_lite <= 0 && p_ptr->lite) p_ptr->cur_lite++;

	if (p_ptr->cur_lite > 14) p_ptr->cur_lite = 14;
	if (p_ptr->cur_lite < 0) p_ptr->cur_lite = 0;

	/* end experimental mods */

	/* Notice changes in the "lite radius" */
	if (p_ptr->old_lite != p_ptr->cur_lite)
	{
		/* Update stuff */
		/* Hack -- PU_MON_LITE for monsters' darkness */
		p_ptr->update |= (PU_LITE | PU_MON_LITE | PU_MONSTERS);

		/* Remember the old lite */
		p_ptr->old_lite = p_ptr->cur_lite;

		if ((p_ptr->cur_lite > 0) && (p_ptr->special_defense & NINJA_S_STEALTH))
			set_superstealth(FALSE);
	}
}



/*
 * Computes current weight limit.
 */
/*:::重量制限を計算*/
///class 狂戦士特殊処理
u32b weight_limit(void)
{
	u32b i;

	/* Weight limit based only on strength */
	i = (u32b)adj_str_wgt[p_ptr->stat_ind[A_STR]] * 50; /* Constant was 100 */


	//v1.1.38 ちょっと整頓　小人の変身で重量制限が変わるものと変わらないものを分けた
	if (p_ptr->prace == RACE_KOBITO)
	{
		switch(p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		case MIMIC_GIGANTIC:
		case MIMIC_BEAST:
		case MIMIC_MARISA:
		case MIMIC_DRAGON:
		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			break;
		default:
			i = i * 2 / 3;
			break;
		}
	}

	if (p_ptr->pclass == CLASS_YUGI) i = i * 3 / 2;
	else if (p_ptr->pclass == CLASS_URUMI) i = i * 3 / 2;//v1.1.69 潤美は重量過多になりにくい
	else if (p_ptr->pclass == CLASS_SAKI) i *= 2;//v1.1.71
	else if (p_ptr->pclass == CLASS_SH_DEALER) i = i * 5 / 4;
	else if (p_ptr->pclass == CLASS_3_FAIRIES)
	{
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			i *= 4;
		else
			i *= 2;
	}
	else if (p_ptr->pclass == CLASS_PATCHOULI) i = i * 2 / 3;
	else if (p_ptr->pclass == CLASS_NINJA) i = i * 2 / 3;
	else if (p_ptr->pclass == CLASS_MARTIAL_ARTIST) i = i * 2 / 3;

	if(p_ptr->pseikaku == SEIKAKU_ELEGANT) i = i * 4 / 5;

	/* Return the result */
	return i;
}

/*:::装備品が無効化されるかどうかのチェック。TRUEが返るときその装備品は無効化される。特殊な変身処理用*/
/*:::この関数はザックを含むインベントリ全てに対して呼ばれるので注意*/
bool check_invalidate_inventory(int slot)
{

	if(!p_ptr->mimic_form && !p_ptr->clawextend) return FALSE;

	if(p_ptr->clawextend)
	{
		if(slot == INVEN_RARM || slot == INVEN_LARM || slot == INVEN_HANDS ) return TRUE;
		else return FALSE;
	}

	switch(p_ptr->mimic_form)
	{
	//獣変身時 リボン・指輪・アミュレット・クローク以外が無効化
	case	MIMIC_BEAST:
		if(slot == INVEN_RARM || slot == INVEN_LARM || slot == INVEN_LITE || slot == INVEN_BODY || slot == INVEN_HEAD || slot == INVEN_HANDS || slot == INVEN_FEET) return TRUE;
		else return FALSE;
		break;//Paranoia
	//スライム変身時　装備全てが無効化
	case MIMIC_SLIME:
		if(slot > INVEN_PACK && slot < INVEN_TOTAL)return TRUE;
		else return FALSE;
	//竜変化　リボン以外無効化
	case	MIMIC_DRAGON:
		if(slot == INVEN_RARM || slot == INVEN_LARM || slot == INVEN_LITE || slot == INVEN_BODY || slot == INVEN_LEFT || slot == INVEN_RIGHT || slot == INVEN_OUTER || slot == INVEN_NECK || slot == INVEN_HEAD || slot == INVEN_HANDS || slot == INVEN_FEET) return TRUE;
		else return FALSE;
		break;//Paranoia
	//猫変身時 リボン・指輪・アミュレット・クローク以外が無効化
	case	MIMIC_CAT:
		if(slot == INVEN_RARM || slot == INVEN_LARM || slot == INVEN_LITE || slot == INVEN_BODY || slot == INVEN_HEAD || slot == INVEN_HANDS || slot == INVEN_FEET) return TRUE;
		else return FALSE;
		break;//Paranoia
	//ドレミー変身など　リボンとアミュ以外無効化
		//v1.1.47 紫苑の強制完全憑依中は全てのスロット無効化
	case MIMIC_METAMORPHOSE_NORMAL:
	case MIMIC_METAMORPHOSE_MONST:
	case MIMIC_METAMORPHOSE_GIGANTIC:

		if (SHION_POSSESSING)
		{
			if (slot > INVEN_PACK && slot < INVEN_TOTAL)return TRUE;
			else return FALSE;
		}
		else
		{

			if (slot == INVEN_RARM || slot == INVEN_LARM || slot == INVEN_LITE || slot == INVEN_BODY || slot == INVEN_LEFT || slot == INVEN_RIGHT || slot == INVEN_OUTER || slot == INVEN_HEAD || slot == INVEN_HANDS || slot == INVEN_FEET) return TRUE;
			else return FALSE;
		}

	default:
		return FALSE;
	}

	return FALSE;//Paranoia

}

/*:::指定した装備スロットに白兵用武器が装備されているかどうか判定*/
bool buki_motteruka(int i)
{
	return ((inventory[i].k_idx && object_is_melee_weapon(&inventory[i]) && !check_invalidate_inventory(i)) ? TRUE : FALSE);
}





/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
/*:::＠の現在のパラメータ、耐性、能力を計算する。種族・職業・性格・装備・一時効果・所持品重量など全て*/
/*:::かなり長いが慎重に変えないといけない*/
///race class item 最重要 パラメータ計算
///mod131224 TVAL関係全面見直し
///mod140216 格闘攻撃回数見直し
void calc_bonuses(void)
{
	int             i, j, hold, neutral[2];
	int             new_speed;
	int             default_hand = 0;
	int             empty_hands_status = empty_hands(TRUE);
	int             extra_blows[2];
	int             extra_shots;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	bool            omoi = FALSE;
//	bool            yoiyami = FALSE;
	bool            down_saving = FALSE;
#if 0
	bool            have_dd_s = FALSE, have_dd_t = FALSE;
#endif
	bool            have_sw = FALSE, have_kabe = FALSE;
	bool            easy_2weapon = FALSE;
	bool            riding_levitation = FALSE;
	//bool			martialarts = FALSE;
	s16b this_o_idx, next_o_idx = 0;
	const player_race *tmp_rp_ptr;

	/* Save the old vision stuff */
	bool old_telepathy = p_ptr->telepathy;
	bool old_esp_animal = p_ptr->esp_animal;
	bool old_esp_undead = p_ptr->esp_undead;
	bool old_esp_demon = p_ptr->esp_demon;
	bool old_esp_kwai = p_ptr->esp_kwai;
	bool old_esp_deity = p_ptr->esp_deity;
	bool old_esp_dragon = p_ptr->esp_dragon;
	bool old_esp_human = p_ptr->esp_human;
	bool old_esp_evil = p_ptr->esp_evil;
	bool old_esp_good = p_ptr->esp_good;
	bool old_esp_nonliving = p_ptr->esp_nonliving;
	bool old_esp_unique = p_ptr->esp_unique;
	bool old_see_inv = p_ptr->see_inv;
	bool old_mighty_throw = p_ptr->mighty_throw;

	/* Save the old armor class */
	/*:::ACは回避と軽減で分けられているのか？*/
	bool old_dis_ac = p_ptr->dis_ac;
	bool old_dis_to_a = p_ptr->dis_to_a;

	int plev = p_ptr->lev;
	bool alice_doll_attack = FALSE;

	bool flag_gun_kata = FALSE; //銃を使った格闘攻撃をするときTRUE


	/* Clear extra blows/shots */
	extra_blows[0] = extra_blows[1] = extra_shots = 0;

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) p_ptr->stat_add[i] = 0;


	/* Clear the Displayed/Real armor class */
	p_ptr->dis_ac = p_ptr->ac = 0;

	/* Clear the Displayed/Real Bonuses */
	p_ptr->dis_to_h[0] = p_ptr->to_h[0] = 0;
	p_ptr->dis_to_h[1] = p_ptr->to_h[1] = 0;
	p_ptr->dis_to_d[0] = p_ptr->to_d[0] = 0;
	p_ptr->dis_to_d[1] = p_ptr->to_d[1] = 0;
	p_ptr->dis_to_h_b = p_ptr->to_h_b = 0;
	p_ptr->dis_to_a = p_ptr->to_a = 0;
	p_ptr->to_h_m = 0;
	p_ptr->to_d_m = 0;

	p_ptr->to_m_chance = 0;

	/* Clear the Extra Dice Bonuses */
	///item class 巫女のときの大幣ボーナスを追加しよう 両手持ちのときの処理に注意
	p_ptr->to_dd[0] = p_ptr->to_ds[0] = 0;
	p_ptr->to_dd[1] = p_ptr->to_ds[1] = 0;

	/* Start with "normal" speed */
	new_speed = 110;

	/* Start with a single blow per turn */
	p_ptr->num_blow[0] = 1;
	p_ptr->num_blow[1] = 1;

	/* Start with a single shot per turn */
	p_ptr->num_fire = 100;

	/* Reset the "xtra" tval */
	p_ptr->tval_xtra = 0;

	/* Reset the "ammo" tval */
	p_ptr->tval_ammo = 0;

	/* Clear all the flags */
	///sys res 耐性や能力を実装したらここに追加する必要がある
	p_ptr->cursed = 0L;
	p_ptr->bless_blade = FALSE;
	p_ptr->xtra_might = FALSE;
	p_ptr->impact[0] = FALSE;
	p_ptr->impact[1] = FALSE;
	p_ptr->pass_wall = FALSE;
	p_ptr->kill_wall = FALSE;
	p_ptr->dec_mana = FALSE;
	p_ptr->easy_spell = FALSE;
	p_ptr->heavy_spell = FALSE;
	p_ptr->see_inv = FALSE;
	p_ptr->free_act = FALSE;
	p_ptr->slow_digest = FALSE;
	p_ptr->regenerate = FALSE;
	p_ptr->can_swim = FALSE;
	p_ptr->levitation = FALSE;
	//p_ptr->hold_life = FALSE;
	p_ptr->telepathy = FALSE;
	p_ptr->esp_animal = FALSE;
	p_ptr->esp_undead = FALSE;
	p_ptr->esp_demon = FALSE;
	p_ptr->esp_kwai = FALSE;
	p_ptr->esp_deity = FALSE;
	p_ptr->esp_dragon = FALSE;
	p_ptr->esp_human = FALSE;
	p_ptr->esp_evil = FALSE;
	p_ptr->esp_good = FALSE;
	p_ptr->esp_nonliving = FALSE;
	p_ptr->esp_unique = FALSE;
	p_ptr->lite = FALSE;
	p_ptr->sustain_str = FALSE;
	p_ptr->sustain_int = FALSE;
	p_ptr->sustain_wis = FALSE;
	p_ptr->sustain_con = FALSE;
	p_ptr->sustain_dex = FALSE;
	p_ptr->sustain_chr = FALSE;
	p_ptr->resist_acid = FALSE;
	p_ptr->resist_elec = FALSE;
	p_ptr->resist_fire = FALSE;
	p_ptr->resist_cold = FALSE;
	p_ptr->resist_pois = FALSE;
	p_ptr->resist_conf = FALSE;
	p_ptr->resist_sound = FALSE;
	p_ptr->resist_lite = FALSE;
	p_ptr->resist_dark = FALSE;
	p_ptr->resist_chaos = FALSE;
	p_ptr->resist_disen = FALSE;
	p_ptr->resist_shard = FALSE;

	p_ptr->resist_water = FALSE;
	p_ptr->resist_holy = FALSE;
	p_ptr->resist_insanity = FALSE;
	p_ptr->speedster = FALSE;

	p_ptr->resist_blind = FALSE;
	p_ptr->resist_neth = FALSE;
	p_ptr->resist_time = FALSE;
	p_ptr->resist_fear = FALSE;
	p_ptr->reflect = FALSE;
	p_ptr->sh_fire = FALSE;
	p_ptr->sh_elec = FALSE;
	p_ptr->sh_cold = FALSE;
	p_ptr->anti_magic = FALSE;
	p_ptr->anti_tele = FALSE;
	p_ptr->warning = FALSE;
	p_ptr->mighty_throw = FALSE;
	p_ptr->see_nocto = FALSE;

	p_ptr->immune_acid = FALSE;
	p_ptr->immune_elec = FALSE;
	p_ptr->immune_fire = FALSE;
	p_ptr->immune_cold = FALSE;

	p_ptr->ryoute = FALSE;
	p_ptr->migite = FALSE;
	p_ptr->hidarite = FALSE;
	p_ptr->no_flowed = FALSE;

	p_ptr->align = friend_align;

	p_ptr->drowning = FALSE;
	p_ptr->do_martialarts = FALSE;


	/*:::一時的種族変異*/
	if (p_ptr->mimic_form) tmp_rp_ptr = &mimic_info[p_ptr->mimic_form];
	else tmp_rp_ptr = &race_info[p_ptr->prace];

/*:::技能値の種族基本値をベースにする*/
	///sys グレイズ技能初期化予定地
	/* Base infravision (purely racial) */
	p_ptr->see_infra = tmp_rp_ptr->infra;

	/* Base skill -- disarming */
	p_ptr->skill_dis = tmp_rp_ptr->r_dis + cp_ptr->c_dis + ap_ptr->a_dis;

	/* Base skill -- magic devices */
	p_ptr->skill_dev = tmp_rp_ptr->r_dev + cp_ptr->c_dev + ap_ptr->a_dev;

	/* Base skill -- saving throw */
	p_ptr->skill_sav = tmp_rp_ptr->r_sav + cp_ptr->c_sav + ap_ptr->a_sav;

	/* Base skill -- stealth */
	p_ptr->skill_stl = tmp_rp_ptr->r_stl + cp_ptr->c_stl + ap_ptr->a_stl;

	/* Base skill -- searching ability */
	p_ptr->skill_srh = tmp_rp_ptr->r_srh + cp_ptr->c_srh + ap_ptr->a_srh;

	/* Base skill -- searching frequency */
	///mod140105 種族変更（準備）playe_race.r_fosを削除
	//p_ptr->skill_fos = tmp_rp_ptr->r_fos + cp_ptr->c_fos + ap_ptr->a_fos;
	p_ptr->skill_fos = tmp_rp_ptr->r_srh + cp_ptr->c_srh + ap_ptr->a_srh;

	/* Base skill -- combat (normal) */
	p_ptr->skill_thn = tmp_rp_ptr->r_thn + cp_ptr->c_thn + ap_ptr->a_thn;

	/* Base skill -- combat (shooting) */
	p_ptr->skill_thb = tmp_rp_ptr->r_thb + cp_ptr->c_thb + ap_ptr->a_thb;

	///sys いずれ投擲と射撃はちゃんと分けたい
	/* Base skill -- combat (throwing) */
	p_ptr->skill_tht = tmp_rp_ptr->r_thb + cp_ptr->c_thb + ap_ptr->a_thb;

	/* Base skill -- digging */
	p_ptr->skill_dig = 0;

	///mod150103 野良神様のパラメータ補正
	if(prace_is_(RACE_STRAYGOD))
	{
		p_ptr->skill_dis += deity_table[p_ptr->race_multipur_val[3]].r_dis;
		p_ptr->skill_dev += deity_table[p_ptr->race_multipur_val[3]].r_dev;
		p_ptr->skill_sav += deity_table[p_ptr->race_multipur_val[3]].r_sav;
		p_ptr->skill_stl += deity_table[p_ptr->race_multipur_val[3]].r_stl;
		p_ptr->skill_srh += deity_table[p_ptr->race_multipur_val[3]].r_srh;
		p_ptr->skill_fos += deity_table[p_ptr->race_multipur_val[3]].r_graze;
		p_ptr->skill_thn += deity_table[p_ptr->race_multipur_val[3]].r_thn;
		p_ptr->skill_thb += deity_table[p_ptr->race_multipur_val[3]].r_thb;
		p_ptr->skill_tht += deity_table[p_ptr->race_multipur_val[3]].r_thb;

	}

	//美鈴専用攻撃速度　デフォ100
	if(p_ptr->pclass == CLASS_MEIRIN) p_ptr->magic_num1[0] = 100;

	/*:::両手持ちしたときや左手のみで近接武器を装備しようとしたとき、装備箇所が強制的に右手に変更される。*/
	/*:::従って、このルーチンが呼ばれたとき右手が空なら左手も必ず空である*/

	/*:::武器装備箇所の判定　両手持ちの可否は重量100以上かどうかだけで決まるようだが武器分類で決めることにした*/
	if (buki_motteruka(INVEN_RARM)) p_ptr->migite = TRUE;
	if (buki_motteruka(INVEN_LARM))
	{
		p_ptr->hidarite = TRUE;
		/*:::ここはどういうときに来るのか？「左利き」設定時？*/
		//↑二刀流で左手の武器が呪われて右手武器を外したとき
		if (!p_ptr->migite) default_hand = 1;
	}
	/*:::手綱を持ってないとき,両手持ち判定処理*/
	if (CAN_TWO_HANDS_WIELDING())
	{
		//三月精は両手持ちにならない
		if (p_ptr->migite && (empty_hands(FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(&inventory[INVEN_RARM]))
		{
			p_ptr->ryoute = TRUE;
		}
		else if (p_ptr->hidarite && (empty_hands(FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(&inventory[INVEN_LARM]))
		{
			p_ptr->ryoute = TRUE;
		}
		/*:::武器を持っていない、もしくは両手持ち可能な武器を持っていないとき*/
		else
		{
			///class 格闘可能判定
			/*:::修行僧、練気術士、狂戦士が両手が空のとき格闘判定（migite/ryouteがTRUEに)*/

			//switch (p_ptr->pclass)
			//{
			//case CLASS_MONK:
			//case CLASS_FORCETRAINER:
			//case CLASS_BERSERKER:
				/*:::両手に武器も盾も手綱もないとき、格闘フラグとryoteがTRUE ryoteはあとでFALSEに変更される*/
				if (empty_hands(FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					//martialarts追加・・したがp_ptrに統合した
					//martialarts = TRUE;
					p_ptr->do_martialarts = TRUE;
					p_ptr->migite = TRUE;
					p_ptr->ryoute = TRUE;
				}
				//break;
			//}
		}
	}

	/*:::手に武器を持っていなかったとき　格闘職以外の素手攻撃判定用？*/
	if (!p_ptr->migite && !p_ptr->hidarite)
	{
		//martialarts = TRUE;
		p_ptr->do_martialarts = TRUE;
		/*:::右手が空ならmigiteをTRUEに*/
		if (empty_hands_status & EMPTY_HAND_RARM) p_ptr->migite = TRUE;
		/*:::右手に武器以外のものを持っていて左手が空のときhidariteをTRUEに*/
		else if (empty_hands_status == EMPTY_HAND_LARM)
		{
			p_ptr->hidarite = TRUE;
			default_hand = 1;
		}
		///mod141116 特殊処理　キスメは両手がふさがっていても格闘ができる
		else if(p_ptr->pclass == CLASS_KISUME)
		{
			p_ptr->migite = TRUE;
			p_ptr->ryoute = TRUE;
		}


	}


	/*:::-Hack-両手インベントリが無効化される変異時は強制的に格闘とする*/
	if (check_invalidate_inventory(INVEN_RARM) && check_invalidate_inventory(INVEN_LARM))
	{
		p_ptr->do_martialarts = TRUE;
		p_ptr->migite = TRUE;
		p_ptr->ryoute = TRUE;
		//v1.1.69 default_handを1にしたせいで変身時の指輪とかの殺戮修正が効いてない。遅まきながら修正
		//default_hand = 1;
	}
	///mod160513 近接攻撃武器を持たず銃を持っているとき、銃による特殊格闘フラグを立てる
	//v1.1.70  && !p_ptr->mimic_formの記述を削除し変身中でも銃格闘可能に。
	else if(!p_ptr->migite && !p_ptr->hidarite && p_ptr->pclass != CLASS_KISUME
		&& p_ptr->pclass != CLASS_ALICE && !p_ptr->clawextend)
	{
		if(inventory[INVEN_RARM].tval == TV_GUN)
		{
			p_ptr->do_martialarts = TRUE;
			p_ptr->migite = TRUE;
			flag_gun_kata = TRUE;
		}
		else if(inventory[INVEN_LARM].tval == TV_GUN)
		{
			p_ptr->do_martialarts = TRUE;
			p_ptr->hidarite = TRUE;
			flag_gun_kata = TRUE;

		}
	}

	/*
	if (cheat_xtra)
	{
		if (p_ptr->migite) msg_print("chk:migite");
		if (p_ptr->hidarite) msg_print("chk_hidarite");
		if (flag_gun_kata) msg_print("chk:gun_kata");
	}
	*/

	/*:::修行僧が構えを取っているときに手に何か持ったら構えが解ける*/
	///class 修行僧特殊
	if (p_ptr->special_defense & KAMAE_MASK)
	{
		if (!(empty_hands_status & EMPTY_HAND_RARM))
		{
			set_action(ACTION_NONE);
		}
	}

	//アリス特殊処理
	if(p_ptr->pclass == CLASS_ALICE)
	{

		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;
			if(object_is_melee_weapon(&inven_add[i]))
			{
				alice_doll_attack = TRUE;
				break;
			}
		}
		if(alice_doll_attack)
		{
			p_ptr->do_martialarts = FALSE;
			p_ptr->ryoute = FALSE;
			p_ptr->migite = TRUE;
		}
		else
		{
			p_ptr->do_martialarts = TRUE;
			p_ptr->ryoute = TRUE;
			p_ptr->migite = TRUE;

		}
	}

	//v1.1.51 夢の世界と新アリーナでは強制的に浮遊を得ることにした。浮遊なしでは新アリーナのマップによっては詰むことがあるため。
	if (IN_DREAM_WORLD) p_ptr->levitation = TRUE;



	/*:::職業とレベル毎の耐性を計算*/
	///class res 職業による耐性付加など
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
			if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 44) p_ptr->regenerate = TRUE;
			break;

		case CLASS_ARCHER:
			if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_blind = TRUE;
			break;

		case CLASS_PRIEST:
			if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 44) p_ptr->resist_insanity = TRUE;
			if (p_ptr->lev > 39)
			{
				if(is_good_realm(p_ptr->realm1)) p_ptr->resist_holy = TRUE;
				else  p_ptr->resist_neth = TRUE;
			}
			break;
		case CLASS_HIGH_MAGE:
			if(p_ptr->lev > 19 && p_ptr->realm1 == TV_BOOK_FORESEE) p_ptr->warning = TRUE;
			if(p_ptr->lev > 19 && (p_ptr->realm1 == TV_BOOK_TRANSFORM || p_ptr->realm1 == TV_BOOK_LIFE)) p_ptr->regenerate = TRUE;
			if(p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_OCCULT)  p_ptr->resist_insanity = TRUE;
			break;



		case CLASS_PALADIN:
			if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_holy = TRUE;
			break;

		case CLASS_MAID:
			if (p_ptr->lev > 24) p_ptr->sustain_dex = TRUE;
			if (p_ptr->lev > 24) p_ptr->sustain_con = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
			break;
		case CLASS_RANGER:
			if (p_ptr->lev > 19) p_ptr->free_act = TRUE;
			if (p_ptr->lev > 29) p_ptr->esp_animal = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_pois = TRUE;
			break;

		case CLASS_ROGUE:
			if (p_ptr->lev > 39) p_ptr->mighty_throw = TRUE;
			if (p_ptr->lev > 29) p_ptr->see_inv = TRUE;
			break;

	//	case CLASS_CHAOS_WARRIOR:
	//		if (p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
	//		if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
	//		break;
		case CLASS_MINDCRAFTER:
			if (p_ptr->lev >  9) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 19) p_ptr->sustain_wis = TRUE;
			if (p_ptr->lev > 24) p_ptr->levitation = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 39) p_ptr->telepathy = TRUE;
			break;
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
			/* Unencumbered Monks become faster every 10 levels */
			if (!(heavy_armor()))
			{
				///race seikaku 妖精とクラッコンの特殊処理
				//if (!(prace_is_(RACE_KLACKON) ||
				//     prace_is_(RACE_SPRITE) ||
				//      (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)))
				//	new_speed += (p_ptr->lev) / 10;

				/* Free action if unencumbered at level 25 */
				if  (p_ptr->lev > 24)
					p_ptr->free_act = TRUE;
			}
			break;
		case CLASS_SORCERER:
			p_ptr->to_a -= 50;
			p_ptr->dis_to_a -= 50;
			break;
		case CLASS_BARD:
			p_ptr->resist_sound = TRUE;
			break;
		case CLASS_SAMURAI:
			if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
			break;
/*
		case CLASS_BERSERKER:
			p_ptr->shero = 1;
			p_ptr->sustain_str = TRUE;
			p_ptr->sustain_dex = TRUE;
			p_ptr->sustain_con = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->free_act = TRUE;
			new_speed += 2;
			if (p_ptr->lev > 29) new_speed++;
			if (p_ptr->lev > 39) new_speed++;
			if (p_ptr->lev > 44) new_speed++;
			if (p_ptr->lev > 49) new_speed++;
			p_ptr->to_a += 10+p_ptr->lev/2;
			p_ptr->dis_to_a += 10+p_ptr->lev/2;
			p_ptr->skill_dig += (100+p_ptr->lev*8);
			if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
			p_ptr->redraw |= PR_STATUS;
			break;
*/
		case CLASS_MIRROR_MASTER:
			if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
			break;
		case CLASS_NINJA:
			new_speed += (p_ptr->lev + 15) / 20;
			p_ptr->to_a += p_ptr->lev/2+5;
			p_ptr->dis_to_a += p_ptr->lev/2+5;

			p_ptr->see_nocto = TRUE;
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 14) p_ptr->free_act = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
			if (p_ptr->lev > 19) p_ptr->speedster = TRUE;
			if (p_ptr->lev > 24) p_ptr->sustain_dex = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 34) p_ptr->mighty_throw = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_dark = TRUE;
			if (p_ptr->lev > 44)
			{
				p_ptr->oppose_pois = 1;
				p_ptr->redraw |= PR_STATUS;
			}
			break;
		case CLASS_TSUKUMO_MASTER:
			if (p_ptr->lev >  9) p_ptr->see_inv = TRUE;
			break;

		case CLASS_SYUGEN:
			if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 44 && is_good_realm(p_ptr->realm1)) p_ptr->resist_holy = TRUE;
			break;
		case CLASS_MAGIC_KNIGHT:
			if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->regenerate = TRUE;
			break;

		case CLASS_RESEARCHER:

			if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_blind = TRUE;

			break;

		case CLASS_ENGINEER:
		case CLASS_NITORI:
			p_ptr->levitation = TRUE;
			if( p_ptr->lev > 24) p_ptr->resist_blind = TRUE;
			if( p_ptr->lev > 39) p_ptr->resist_shard = TRUE;
			break;
		case CLASS_JEWELER:
			if( p_ptr->lev > 19) p_ptr->resist_blind = TRUE;
			break;


		case CLASS_RUMIA:
			p_ptr->resist_dark = TRUE;
			break;
		case CLASS_YOUMU:
			if (p_ptr->lev > 24) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 29)  p_ptr->speedster = TRUE;
			break;

	case CLASS_TEWI:
		if (p_ptr->lev >  9) p_ptr->free_act = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_holy = TRUE;
		break;
	case CLASS_UDONGE:
		p_ptr->see_inv = TRUE;
		if (p_ptr->lev >  9) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_sound = TRUE;
		if (p_ptr->lev > 24) p_ptr->resist_insanity = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_lite = TRUE;
		if (p_ptr->lev > 39) p_ptr->telepathy = TRUE;
		break;
	case CLASS_IKU:
		p_ptr->resist_elec = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->oppose_elec = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		break;
	case CLASS_KOMACHI:
		if (p_ptr->lev > 19) 	p_ptr->resist_water = TRUE;
		if (p_ptr->lev > 29) 	p_ptr->speedster = TRUE;
		if (p_ptr->lev > 39) 	p_ptr->resist_time = TRUE;
		break;
	case CLASS_KOGASA:
		p_ptr->resist_water = TRUE;
		break;

	case CLASS_KASEN:
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			p_ptr->see_inv = TRUE;
			p_ptr->resist_neth = TRUE;
		}
		p_ptr->resist_fear = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_dark = TRUE;
		if (p_ptr->lev > 19) p_ptr->sustain_str = TRUE;
		if (p_ptr->lev > 19) p_ptr->sustain_con = TRUE;
		if (p_ptr->lev > 34) p_ptr->resist_insanity = TRUE;
		break;
	case CLASS_SUIKA:
		p_ptr->resist_fear = TRUE;
		p_ptr->regenerate = TRUE;
		if (p_ptr->lev > 19) 	p_ptr->resist_sound = TRUE;
		if (p_ptr->lev > 39) 	p_ptr->resist_shard = TRUE;
		break;
	case CLASS_KOISHI:
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->free_act = TRUE;
		if(p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
		break;
	case CLASS_MOMIZI:
		p_ptr->see_nocto = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_blind = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
		break;
	case CLASS_SEIGA:
		if (p_ptr->lev > 14) p_ptr->resist_dark = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_neth = TRUE;

		if (p_ptr->lev > 39) p_ptr->pass_wall = TRUE;
		else if (p_ptr->lev > 24) p_ptr->kill_wall = TRUE;
		break;
	case CLASS_CIRNO:
		p_ptr->resist_cold = TRUE;
		if (p_ptr->lev > 14) p_ptr->sh_cold = TRUE;
		if (p_ptr->lev > 24) p_ptr->immune_cold = TRUE;
		if (p_ptr->lev > 34) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 44) p_ptr->resist_insanity = TRUE;

		//v1.1.32
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
		{
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_fear = TRUE;
		}

		break;
	case CLASS_ORIN:
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_neth = TRUE;

		if (p_ptr->lev > 9) p_ptr->esp_undead = TRUE;
		if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 34)
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}

		if (p_ptr->lev > 44) p_ptr->resist_insanity = TRUE;
		break;
	case CLASS_SHINMYOUMARU:
		p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 29) p_ptr->speedster = TRUE;
		break;
	case CLASS_NAZRIN:
		if (p_ptr->lev > 29) p_ptr->resist_holy = TRUE;
		break;
	case CLASS_LETTY:
		p_ptr->resist_cold = TRUE;
		if (p_ptr->lev > 19)
		{
			p_ptr->oppose_cold = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		if (p_ptr->lev > 39) p_ptr->immune_cold = TRUE;
		break;

	case CLASS_PATCHOULI:
		p_ptr->levitation = TRUE;
		if (p_ptr->lev > 4) p_ptr->resist_fire = TRUE;
		if (p_ptr->lev > 9) p_ptr->resist_cold = TRUE;
		if (p_ptr->lev > 14) p_ptr->resist_elec = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_acid = TRUE;

		//v1.1.32 パチュリー特殊性格
		if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
		{
			p_ptr->ac -= 50;
			p_ptr->dis_to_a -= 50;
		}

		break;

	case CLASS_AYA:
		//v1.1.80 専用性格のとき加速-2
		new_speed += p_ptr->lev / 5 ;
		if (!is_special_seikaku(SEIKAKU_SPECIAL_AYA)) new_speed += 2;

		if (p_ptr->lev > 9) p_ptr->speedster = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_sound = TRUE;

		break;
	case CLASS_BANKI:
		p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
		break;

	case CLASS_MYSTIA:
		new_speed += p_ptr->lev / 15 ;
		p_ptr->see_infra += 5;
		p_ptr->levitation = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_sound = TRUE;

		//v1.1.75
		if (p_ptr->lev > 39) p_ptr->see_nocto = TRUE;


		break;
	case CLASS_FLAN:
		p_ptr->levitation = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		if (p_ptr->lev > 34) p_ptr->kill_wall = TRUE;
		if (p_ptr->lev > 34)
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}

		break;

	case CLASS_SHOU:
		p_ptr->see_infra += 5;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_holy = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 34) p_ptr->resist_neth = TRUE;
		break;

	case CLASS_YUYUKO:
		p_ptr->esp_undead = TRUE;
		break;

	case CLASS_SATORI:
		if (p_ptr->lev > 9)	p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 24) p_ptr->telepathy = TRUE;
		if (p_ptr->lev > 39) p_ptr->resist_insanity = TRUE;
		break;

	case CLASS_KYOUKO:
		p_ptr->resist_sound = TRUE;
		if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
		break;

	case CLASS_TOZIKO:
		p_ptr->resist_elec = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->oppose_elec = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		break;

	case CLASS_LILY_WHITE:
		p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_insanity = TRUE;
		break;

	case CLASS_KISUME:
		p_ptr->resist_fire = TRUE;
		p_ptr->levitation = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_water = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_neth = TRUE;
		break;
	case CLASS_HATATE:
		p_ptr->see_inv = TRUE;
		break;
	case CLASS_MIKO:
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->free_act = TRUE;
		if (p_ptr->lev > 4) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 9) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 9) p_ptr->esp_human = TRUE;
		if (p_ptr->lev > 14) p_ptr->levitation = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_lite = TRUE;
		if (p_ptr->lev > 24) p_ptr->warning = TRUE;
		if (p_ptr->lev > 29) p_ptr->telepathy = TRUE;
		if (p_ptr->lev > 39) p_ptr->resist_time = TRUE;
		break;
	case CLASS_KOKORO:
		if (p_ptr->lev > 19) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 39) p_ptr->resist_insanity = TRUE;
		break;
	case CLASS_WRIGGLE:
		if (p_ptr->lev > 24) p_ptr->warning = TRUE;
		if (p_ptr->lev > 39) p_ptr->resist_lite = TRUE;
		break;
	case CLASS_YUUKA:
		p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_lite = TRUE;
		break;
	case CLASS_CHEN:
		p_ptr->see_infra+=3;
		if(!p_ptr->magic_num1[0])
		{
			if (p_ptr->lev > 24) p_ptr->speedster = TRUE;
			new_speed += 1 + (p_ptr->lev - 8) / 16;
		}
		break;
	case CLASS_MURASA:
		p_ptr->resist_water = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		if(p_ptr->lev > 19) p_ptr->free_act = TRUE;
		if(p_ptr->lev > 29) p_ptr->resist_pois = TRUE;
		if(p_ptr->lev > 39) p_ptr->resist_insanity = TRUE;
		break;
	case CLASS_KEINE:
		if(p_ptr->lev > 9) p_ptr->see_inv = TRUE;
		if(p_ptr->lev > 19) p_ptr->resist_conf = TRUE;
		if(p_ptr->lev > 34) p_ptr->sustain_int = TRUE;
		if(p_ptr->lev > 34) p_ptr->sustain_wis = TRUE;
		//白沢化時
		if(p_ptr->magic_num1[0] && !p_ptr->mimic_form)
		{
			p_ptr->resist_holy = TRUE;
			p_ptr->resist_dark = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_insanity = TRUE;
			p_ptr->easy_spell = TRUE;
			p_ptr->warning = TRUE;
			p_ptr->esp_kwai = TRUE;
			p_ptr->esp_demon = TRUE;
			p_ptr->esp_undead = TRUE;
			p_ptr->esp_animal = TRUE;
			p_ptr->esp_deity = TRUE;
			p_ptr->esp_dragon = TRUE;
			p_ptr->esp_unique = TRUE;
		}
		break;
	case CLASS_YUGI:
		if(p_ptr->lev > 9) p_ptr->resist_fire = TRUE;
		if(p_ptr->lev > 14) p_ptr->resist_cold = TRUE;
		if(p_ptr->lev > 19) p_ptr->sustain_str = TRUE;
		if(p_ptr->lev > 29) p_ptr->resist_lite = TRUE;

		break;
	case CLASS_REIMU:
		{
			int rank = osaisen_rank();
			p_ptr->levitation = TRUE;
			p_ptr->resist_holy = TRUE;
			if(rank > 2) p_ptr->see_inv = TRUE;
			if(rank > 7) p_ptr->dec_mana = TRUE;
			if(rank > 8)
			{
				p_ptr->resist_acid = TRUE;
				p_ptr->resist_fire = TRUE;
				p_ptr->resist_cold = TRUE;
				p_ptr->resist_elec = TRUE;
				p_ptr->resist_pois = TRUE;
			}
			if(plev > 4 && rank > 3) p_ptr->slow_digest = TRUE;
			if(plev > 4 && rank > 6) p_ptr->regenerate = TRUE;
			if(plev > 9 && rank > 4) p_ptr->free_act = TRUE;
			if(plev > 14 && rank > 3) p_ptr->resist_fear = TRUE;
			if(plev > 19) p_ptr->esp_kwai = TRUE;
			if(plev > 19 && rank > 4) p_ptr->resist_conf = TRUE;
			if(plev > 24 && rank > 1) p_ptr->esp_undead = TRUE;
			if(plev > 24 && rank > 5) p_ptr->resist_neth = TRUE;
			if(plev > 29 && rank > 2) p_ptr->esp_demon = TRUE;
			//lev30rank5 破邪免疫
			if(plev > 34 && rank > 5) p_ptr->esp_evil = TRUE;
			if(plev > 34 && rank > 4) p_ptr->resist_lite = TRUE;
			if(plev > 39 && rank > 8) p_ptr->resist_time = TRUE;
			if(plev > 39 && rank > 6) p_ptr->resist_chaos = TRUE;
			if(plev > 44 && rank > 7) p_ptr->resist_insanity = TRUE;
			//lev45rank9 完全ESP
		}
		break;

	case CLASS_KAGEROU:
		if(p_ptr->lev > 19) p_ptr->see_inv = TRUE;
		if(p_ptr->lev > 39) p_ptr->speedster = TRUE;
		break;

	case CLASS_SANAE:
		if(p_ptr->lev > 9) p_ptr->esp_deity = TRUE;
		if(p_ptr->lev > 19) p_ptr->levitation = TRUE;
		if(p_ptr->lev > 39) p_ptr->resist_holy = TRUE;
		break;
	case CLASS_REMY:
		p_ptr->levitation = TRUE;
		if(p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
		if(plev > 14) new_speed ++;
		if(plev > 24) new_speed ++;
		if(plev > 34) new_speed ++;
		break;
	case CLASS_BYAKUREN:
		p_ptr->resist_holy = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
		if(p_ptr->tim_general[0]) //白蓮強化時
		{
			new_speed += 2 + p_ptr->lev / 8;
			p_ptr->regenerate = TRUE;
			p_ptr->lite = TRUE;
			if(p_ptr->lev > 4) p_ptr->resist_fire = TRUE;
			if(p_ptr->lev > 4) p_ptr->resist_cold = TRUE;
			if(p_ptr->lev > 9) p_ptr->resist_elec = TRUE;
			if(p_ptr->lev > 9) p_ptr->resist_acid = TRUE;
			if(p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
			if(p_ptr->lev > 24) p_ptr->sustain_str = FALSE;
			if(p_ptr->lev > 24) p_ptr->sustain_con = FALSE;
			if(p_ptr->lev > 24) p_ptr->sustain_dex = FALSE;
			if(p_ptr->lev > 29) p_ptr->resist_lite = TRUE;
			if(p_ptr->lev > 34) p_ptr->mighty_throw = TRUE;
			if(p_ptr->lev > 34) p_ptr->speedster = TRUE;
			if(p_ptr->lev > 39) p_ptr->resist_chaos = TRUE;
			if(p_ptr->lev > 39) p_ptr->resist_disen = TRUE;
			if(p_ptr->lev > 44) p_ptr->immune_fire = TRUE;

			p_ptr->skill_thn += 20+plev*8/5;
			p_ptr->skill_thb += 10+plev;
			p_ptr->skill_tht += 10+plev;

			p_ptr->to_a += 20 + plev;
			p_ptr->dis_to_a += 20 + plev;
		}

		break;
	case CLASS_NUE:
		p_ptr->see_nocto = TRUE;
		break;
	case CLASS_SEIJA:
		if(plev > 39) p_ptr->resist_time = TRUE;
		break;
	case CLASS_UTSUHO:
		p_ptr->resist_fire = TRUE;
		p_ptr->levitation = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_lite = TRUE;
		if(plev > 19) p_ptr->sh_fire = TRUE;
		if(plev > 24) p_ptr->resist_blind = TRUE;
		if(plev > 29) p_ptr->resist_dark = TRUE;
		if(plev > 34) p_ptr->immune_fire = TRUE;
		break;

	case CLASS_YAMAME:
		p_ptr->resist_pois = TRUE;
		if(plev > 9) p_ptr->free_act = TRUE;
		if(plev > 24) p_ptr->see_inv = TRUE;
		if(plev > 39) p_ptr->resist_dark = TRUE;
		//v1.1.47
		p_ptr->skill_dig += p_ptr->lev * 5;
		break;

	case CLASS_MARTIAL_ARTIST:
		if(plev > 19) p_ptr->resist_fear = TRUE;
		if(plev > 29) p_ptr->sustain_str = TRUE;
		if(plev > 39) p_ptr->levitation = TRUE;
		break;

	case CLASS_SUWAKO:
		p_ptr->resist_water = TRUE;
		p_ptr->see_inv = TRUE;
		if(plev > 9) p_ptr->resist_neth = TRUE;
		if(plev > 19) p_ptr->resist_fear = TRUE;
		if(plev > 29) p_ptr->resist_dark = TRUE;
		if(plev > 39) p_ptr->resist_chaos = TRUE;
		break;
	case CLASS_RAN:
		new_speed += (p_ptr->lev + 5) / 10 ;
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_pois = TRUE;
		break;
	case CLASS_EIKI:
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->esp_unique = TRUE;
		if(plev > 29) p_ptr->resist_chaos = TRUE;
		if(plev > 39) p_ptr->telepathy = TRUE;

		break;
	case CLASS_MEIRIN:
		p_ptr->regenerate = TRUE;
		if(plev > 4)p_ptr->resist_fire = TRUE;
		if(plev > 14)p_ptr->see_inv = TRUE;
		if(plev > 24)p_ptr->free_act = TRUE;
		if(plev > 34)p_ptr->resist_lite = TRUE;
		break;
	case CLASS_PARSEE:
		p_ptr->resist_water = TRUE;
		p_ptr->resist_acid = TRUE;
		if(plev > 19)p_ptr->resist_fear = TRUE;
		if(plev > 34)p_ptr->resist_neth = TRUE;
		break;

	case CLASS_SHINMYOU_2:
		p_ptr->resist_fear = TRUE;
		p_ptr->levitation = TRUE;
		if(prace_is_(RACE_KOBITO)) p_ptr->speedster = TRUE;
		p_ptr->resist_water = TRUE;
		new_speed += (p_ptr->lev ) / 10 ;
		if(prace_is_(RACE_KOBITO)) p_ptr->skill_stl -= 10;
		break;
	case CLASS_SUMIREKO:
			p_ptr->levitation = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_time = TRUE;

			for(i=0;i<7;i++)
			{
				int ac_bonus;
				if(!inven_add[i].k_idx) continue;
				//v1.1.20 テレキネシス中のアイテムのAC修正減少
				ac_bonus = inven_add[i].weight / 20;
				ac_bonus += inven_add[i].ac / 10;
				ac_bonus += inven_add[i].to_a / 10;
				p_ptr->to_a += ac_bonus;
				p_ptr->dis_to_a += ac_bonus;
			}
			//v1.1.52 新性格用
			if (check_activated_nameless_arts(JKF1_KILL_WALL)) p_ptr->kill_wall = TRUE;


			break;
	case CLASS_ICHIRIN:
			p_ptr->warning = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_lite = TRUE;
		break;
	case CLASS_MOKOU:
		p_ptr->resist_fire = TRUE;
		if (p_ptr->lev > 9) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_lite = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		break;
	case CLASS_KANAKO:
		{
			int rank = kanako_rank();
			p_ptr->levitation = TRUE;
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 14) p_ptr->free_act = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
			if (rank > 2) p_ptr->resist_fear = TRUE;
			if (rank > 4) p_ptr->regenerate = TRUE;
			if (rank > 4) p_ptr->slow_digest = TRUE;
			if (rank > 6) p_ptr->immune_elec = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_HIZIRIN) p_ptr->resist_holy = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MIMIMI) p_ptr->resist_lite = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_REIMU) p_ptr->resist_time = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_CTHULHU) p_ptr->resist_water = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_OBERON) p_ptr->resist_disen = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_AZAT) p_ptr->resist_insanity = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MORGOTH) p_ptr->resist_dark = TRUE;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_J) p_ptr->resist_chaos = TRUE;


		}
		break;

	case CLASS_FUTO:
		if (p_ptr->lev > 4) p_ptr->resist_fire = TRUE;
		if (p_ptr->lev > 14) p_ptr->resist_water = TRUE;
		if (p_ptr->lev > 24) p_ptr->levitation = TRUE;
		if (p_ptr->lev > 34) p_ptr->warning = TRUE;

		//v1.1.42追加　これ忘れてたのでレベル40になっても二重耐性付与されず耐火薬などで二重耐性付与するとそれが戻らないという形になっていた
		if (p_ptr->lev > 39)
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}

		break;

	case CLASS_SUNNY:
		if (p_ptr->lev > 19) p_ptr->resist_lite = TRUE;
		if (p_ptr->magic_num1[0] >= SUNNY_CHARGE_SUNLIGHT_3) p_ptr->sh_fire = TRUE;
		break;
	case CLASS_LUNAR:
		if (p_ptr->lev > 19) p_ptr->resist_dark = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_sound = TRUE;
		break;
	case CLASS_STAR:
		if(plev > 19) p_ptr->resist_conf = TRUE;
		break;

	case CLASS_3_FAIRIES:
		if (p_ptr->lev > 19) p_ptr->resist_lite = TRUE;
		if (p_ptr->lev > 34) p_ptr->resist_sound = TRUE;

		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->free_act = TRUE;
		}

		break;

	case CLASS_MARISA:
		p_ptr->levitation = TRUE;
		p_ptr->resist_pois = TRUE;
		if(plev > 9) p_ptr->resist_fire = TRUE;
		if(plev > 19) p_ptr->free_act = TRUE;
		if(plev > 24) p_ptr->speedster = TRUE;
		if(plev > 29) p_ptr->resist_lite = TRUE;
		new_speed += (p_ptr->lev ) / 7;
		break;

	case CLASS_LUNASA:
		p_ptr->resist_sound = TRUE;
		if(plev > 19) p_ptr->resist_conf = TRUE;
		if(plev > 34) p_ptr->warning = TRUE;
		break;

	case CLASS_MERLIN:
		p_ptr->resist_sound = TRUE;
		if(plev > 19) p_ptr->resist_fear = TRUE;
		if(plev > 34) p_ptr->resist_insanity = TRUE;

		break;

	case CLASS_LYRICA:
		p_ptr->resist_sound = TRUE;
		if(plev > 19) p_ptr->resist_blind = TRUE;

		if(music_singing(MUSIC_NEW_LYRICA_SOLO))
		{
			p_ptr->skill_sav += 30;
		}
		break;
	case CLASS_CLOWNPIECE:
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_neth = TRUE;
		if(plev > 19) p_ptr->resist_conf = TRUE;
		new_speed += (p_ptr->lev ) / 9;
		break;
	case CLASS_DOREMY:
		p_ptr->free_act = TRUE;
		if(plev > 14) p_ptr->see_inv = TRUE;
		if(plev > 24) p_ptr->resist_chaos = TRUE;
		if(plev > 39) p_ptr->resist_insanity = TRUE;
		if(IN_DREAM_WORLD) p_ptr->telepathy = TRUE;
		break;

	case CLASS_YATSUHASHI:
	case CLASS_BENBEN:
		if(plev > 19) p_ptr->resist_sound = TRUE;

		break;
	case CLASS_HINA:
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->levitation = TRUE;

		if(plev > 24) p_ptr->resist_pois = TRUE;
		if(p_ptr->magic_num1[0] > 14999) p_ptr->resist_insanity = TRUE;
		if(p_ptr->magic_num1[0] > 19999) p_ptr->reflect = TRUE;
		break;
	case CLASS_SAKUYA:
		p_ptr->free_act = TRUE;
		if(plev > 9) p_ptr->slow_digest = TRUE;
		if(plev > 14) p_ptr->resist_time = TRUE;
		if(plev > 24) p_ptr->resist_fear = TRUE;
		if(plev > 39) p_ptr->speedster = TRUE;
		new_speed += (p_ptr->lev ) / 8;
		break;

	case CLASS_RAIKO:
		p_ptr->resist_elec = TRUE;
		if (p_ptr->lev > 9) p_ptr->resist_sound = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->sh_elec = TRUE;
			p_ptr->oppose_elec = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		if (p_ptr->lev > 34)p_ptr->levitation = TRUE;

		break;
	case CLASS_CHEMIST:
		if(plev > 14) p_ptr->free_act = TRUE;
		if(plev > 29) p_ptr->resist_pois = TRUE;
		break;

	case CLASS_MAMIZOU:
		p_ptr->see_inv = TRUE;
		if(plev > 14) p_ptr->free_act = TRUE;
		if(plev > 24) p_ptr->resist_conf = TRUE;
		if(plev > 34) p_ptr->resist_chaos = TRUE;
		if(plev > 39) p_ptr->telepathy = TRUE;

		break;
	case CLASS_YUKARI:
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 14) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_chaos = TRUE;
		if (p_ptr->lev > 29) p_ptr->reflect = TRUE;
		if (p_ptr->lev > 39) p_ptr->pass_wall = TRUE;
		break;
	case CLASS_RINGO:
		if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 19) p_ptr->warning = TRUE;
		if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 39) p_ptr->sustain_int = TRUE;
		if (p_ptr->lev > 39) p_ptr->sustain_wis = TRUE;
		break;

	case CLASS_SEIRAN:
		if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 44) p_ptr->resist_time = TRUE;
		break;

	case CLASS_EIRIN:
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 34) p_ptr->resist_insanity = TRUE;
		break;

	case CLASS_KAGUYA:
		p_ptr->sustain_chr = TRUE;
		if(plev > 19) p_ptr->resist_disen = TRUE;
		if(plev > 29) p_ptr->resist_time = TRUE;
		if(plev > 39) p_ptr->resist_insanity = TRUE;
		break;
	case CLASS_TOYOHIME:
		p_ptr->resist_water = TRUE;
		if(plev > 19) p_ptr->resist_time = TRUE;
		if(plev > 34) p_ptr->esp_evil = TRUE;

	case CLASS_YORIHIME:
		p_ptr->resist_water = TRUE;
		if(plev > 9) p_ptr->resist_lite = TRUE;
		if(plev > 19) p_ptr->resist_dark = TRUE;
		if(plev > 29) p_ptr->resist_shard = TRUE;
		break;

	case CLASS_HECATIA:
		{
			p_ptr->lite = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_fire = TRUE;
			if(plev > 19) p_ptr->resist_conf = TRUE;
			if(plev > 29) p_ptr->resist_insanity = TRUE;
			if(plev > 39) p_ptr->easy_spell = TRUE;

			if(hecatia_body_is_(HECATE_BODY_OTHER))
			{
				if(plev > 24) p_ptr->resist_chaos = TRUE;
				if(plev > 44) p_ptr->resist_time = TRUE;
			}
			if(hecatia_body_is_(HECATE_BODY_MOON))
			{
				if(plev > 24) p_ptr->resist_dark = TRUE;
				if(plev > 34) p_ptr->resist_lite = TRUE;
			}
			if(hecatia_body_is_(HECATE_BODY_EARTH))
			{
				if(plev > 24) p_ptr->resist_acid = TRUE;
				if(plev > 24) p_ptr->resist_cold = TRUE;
				if(plev > 24) p_ptr->resist_elec = TRUE;
				if(plev > 34) p_ptr->resist_pois = TRUE;
			}
		}
		break;
	case CLASS_JUNKO:
		if(check_activated_nameless_arts(JKF1_KILL_WALL)) p_ptr->kill_wall = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->see_inv = TRUE;
		break;

	case CLASS_SOLDIER:

		if(plev > 19) p_ptr->resist_fire = TRUE;
		if(plev > 29) p_ptr->resist_fear = TRUE;
		if(plev > 39) p_ptr->warning = TRUE;

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_SRH_DIS_PLUS))
		{
			p_ptr->skill_dig += 20 + plev * 2;
			p_ptr->skill_dis += 10 + plev ;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETECTING))
		{
			p_ptr->skill_srh += 5 + plev / 2;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_SNEAKING))
		{
			p_ptr->skill_stl += 3;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_LETHAL_WEAPON))
		{
			p_ptr->to_a += 25;
			p_ptr->dis_to_a += 25;
			p_ptr->to_h[0] += plev / 5;
			p_ptr->to_h[1] += plev / 5;
			p_ptr->to_h_m  += plev / 5;
			p_ptr->to_d[0] += plev / 10;
			p_ptr->to_d[1] += plev / 10;
			p_ptr->to_d_m  += plev / 10;
			p_ptr->dis_to_h[0] += plev / 5;
			p_ptr->dis_to_h[1] += plev / 5;
			p_ptr->dis_to_d[0] += plev / 10;
			p_ptr->dis_to_d[1] += plev / 10;
			extra_blows[0] += 1;
			extra_blows[1] += 1;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		{
			p_ptr->resist_shard = TRUE;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_HAWK_EYE))
		{
			p_ptr->see_inv = TRUE;
			p_ptr->skill_thb += 25 + plev / 2;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_WEAPON_MASTERY))
		{
			p_ptr->skill_thb += 25 + plev / 2;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_NOCTO_VISION))
		{
			p_ptr->see_nocto = TRUE;
		}
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_DER_FREISCHUTZ))
		{
			p_ptr->skill_thb += 25 + plev / 2;
		}
		break;

	case CLASS_NEMUNO:
		//ネムノは縄張りにいるときパワーアップ
		if(IS_NEMUNO_IN_SANCTUARY)
		{
			int add_stat = 2;
			if (plev > 29) add_stat++;
			p_ptr->resist_fear = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->to_a += 25 + plev / 2;
			p_ptr->dis_to_a += 25 + plev / 2;
			p_ptr->to_h[0] += 4 + plev / 3;
			p_ptr->to_h[1] += 4 + plev / 3;
			p_ptr->to_h_m  += 4 + plev / 3;
			p_ptr->to_d[0] += 4 + plev / 4;
			p_ptr->to_d[1] += 4 + plev / 4;
			p_ptr->to_d_m  += 4 + plev / 4;
			p_ptr->dis_to_h[0] += 4 + plev / 3;
			p_ptr->dis_to_h[1] += 4 + plev / 3;
			p_ptr->dis_to_d[0] += 4 + plev / 4;
			p_ptr->dis_to_d[1] += 4 + plev / 4;
			extra_blows[0] += plev / 20;
			extra_blows[1] += plev / 20;

			for(i=0;i<6;i++) p_ptr->stat_add[i] += add_stat;
			new_speed += 2 + plev / 15;
		}

		if(plev > 24) p_ptr->resist_fear = TRUE;
		if(plev > 34) p_ptr->resist_water = TRUE;
		break;
	case CLASS_AUNN:
		p_ptr->resist_cold = TRUE;
		p_ptr->esp_deity = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->resist_holy = TRUE;
		if(plev > 14) p_ptr->esp_kwai = TRUE;
		if(plev > 14) p_ptr->esp_demon = TRUE;
		if(plev > 19) p_ptr->see_inv = TRUE;
		if(plev > 24) p_ptr->resist_pois = TRUE;
		if(plev > 29) p_ptr->esp_evil = TRUE;
		if(plev > 34) p_ptr->resist_fear = TRUE;
		p_ptr->to_a += 5 + plev / 2;
		p_ptr->dis_to_a += 5 + plev / 2;
		break;

	case CLASS_NARUMI:
		p_ptr->resist_holy = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->oppose_cold = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			p_ptr->regenerate = TRUE;
		break;

	case CLASS_KOSUZU:
		//何もない
		break;
	case CLASS_LARVA:
		for(i=0;i<6;i++)
			p_ptr->stat_add[i]+= plev / 15;

		if (plev > 29) p_ptr->sustain_con = TRUE;
		if (plev > 39) p_ptr->resist_pois = TRUE;

		break;

	case CLASS_MAI:
	case CLASS_SATONO:
		if (plev > 9)	p_ptr->sustain_chr = TRUE;
		if (plev > 19)	p_ptr->resist_conf = TRUE;
		if (plev > 29)	p_ptr->resist_chaos = TRUE;
		if (plev > 39)	p_ptr->resist_insanity = TRUE;

		break;
	case CLASS_VFS_CLOWNPIECE:
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_neth = TRUE;
		if (plev > 19) p_ptr->resist_insanity = TRUE;
		break;

	case CLASS_JYOON:
		if (plev > 19) p_ptr->free_act = TRUE;
		if (plev > 34) p_ptr->resist_lite = TRUE;
		break;

	case CLASS_SHION:
		p_ptr->free_act = TRUE;
		p_ptr->levitation = TRUE;
		if (plev > 9) p_ptr->see_inv = TRUE;
		if (plev > 19) p_ptr->resist_conf = TRUE;
		if (plev > 29) p_ptr->resist_chaos = TRUE;

		if (SUPER_SHION)
		{
			p_ptr->cursed |= (TRC_AGGRAVATE);
			new_speed += plev * 2 / 5;

			p_ptr->to_a += plev * 4;
			p_ptr->dis_to_a += plev * 4;
			p_ptr->to_h[0] += plev ;
			p_ptr->to_h[1] += plev ;
			p_ptr->to_h_m += plev ;
			p_ptr->to_d[0] += plev / 2;
			p_ptr->to_d[1] += plev / 2;
			p_ptr->to_d_m += plev / 2;
			p_ptr->dis_to_h[0] += plev ;
			p_ptr->dis_to_h[1] += plev ;
			p_ptr->dis_to_d[0] += plev / 2 ;
			p_ptr->dis_to_d[1] += plev / 2 ;
		}
		break;

	case CLASS_OKINA:
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->see_nocto = TRUE;
		p_ptr->levitation = TRUE;

		if (plev > 9) p_ptr->resist_conf = TRUE;
		if (plev > 19) p_ptr->resist_dark = TRUE;
		if (plev > 24) p_ptr->resist_acid = TRUE;
		if (plev > 24) p_ptr->resist_elec = TRUE;
		if (plev > 24) p_ptr->resist_fire = TRUE;
		if (plev > 24) p_ptr->resist_cold = TRUE;
		if (plev > 29) p_ptr->resist_chaos = TRUE;
		if (plev > 39) p_ptr->resist_time = TRUE;
		break;

	case CLASS_MAYUMI:
		p_ptr->sustain_con = TRUE;
		p_ptr->resist_fear = TRUE;
		if(plev > 29) p_ptr->resist_cold = TRUE;
		if (plev > 19) p_ptr->resist_conf = TRUE;
		if (p_ptr->lev > 29)//Lv30以降火炎二重耐性
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}

		break;

	case CLASS_KUTAKA:

		p_ptr->warning = TRUE;
		p_ptr->levitation = TRUE;
		p_ptr->resist_pois = TRUE;
		if(plev > 19)	p_ptr->resist_water = TRUE;
		break;

	case CLASS_URUMI:

		p_ptr->resist_water = TRUE;
		if (plev > 19)	p_ptr->resist_fear = TRUE;

		break;

	case CLASS_SAKI:
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->free_act = TRUE;
		if(plev > 9) p_ptr->resist_elec = TRUE;

		if(plev>19) p_ptr->esp_undead = TRUE;
		if (plev > 19) p_ptr->sustain_str = TRUE;

		new_speed += 3 + plev / 12;

		break;

	case CLASS_YACHIE:


		p_ptr->resist_water = TRUE;
		if (plev > 14) p_ptr->resist_fire = TRUE;
		if (plev > 19) p_ptr->resist_elec = TRUE;
		if (plev > 24) p_ptr->resist_acid = TRUE;

		if (plev>19) p_ptr->esp_undead = TRUE;
		if (plev>29) p_ptr->resist_conf = TRUE;

		p_ptr->ac += MAX(10,plev);
		p_ptr->dis_ac += MAX(10, plev);

		break;

	case CLASS_KEIKI:

		p_ptr->resist_fire = TRUE;
		p_ptr->resist_water = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->sustain_dex = TRUE;

		if (plev>24) p_ptr->esp_nonliving = TRUE;
		if (p_ptr->lev > 29)
		{
			p_ptr->oppose_fire = 1;
			p_ptr->redraw |= PR_STATUS;
		}


		break;

	case CLASS_TAKANE:

		if (plev > 24) p_ptr->resist_conf = TRUE;
		if (plev > 44) p_ptr->esp_unique = TRUE;

		if (p_ptr->tim_general[0])//木隠れの技術
		{
			if (cave_have_flag_bold((py), (px), FF_TREE))
				p_ptr->skill_stl += 8;
			else
				p_ptr->skill_stl += 2;
		}

		break;

	case CLASS_SANNYO:

		if (plev > 19) p_ptr->resist_fear = TRUE;
		if (plev > 29) p_ptr->resist_conf = TRUE;


		break;

	case CLASS_MOMOYO:

		p_ptr->resist_pois = TRUE;
		p_ptr->resist_fear = TRUE;

		if (p_ptr->lev > 19)
		{
			p_ptr->oppose_pois = 1;
			p_ptr->redraw |= PR_STATUS;
		}

		if (p_ptr->lev > 29)p_ptr->resist_neth = TRUE;

		if (p_ptr->lev > 34)p_ptr->sustain_str = TRUE;
		if (p_ptr->lev > 34)p_ptr->sustain_dex = TRUE;
		if (p_ptr->lev > 34)p_ptr->sustain_con = TRUE;

		p_ptr->skill_dig += p_ptr->lev * 10;

		if (p_ptr->tim_general[0]) p_ptr->kill_wall = TRUE;


		break;


	default:
		break;


	}

	/***** Races ****/
	/*:::種族関係処理(変身時)*/
	///magic res
	if (p_ptr->mimic_form && p_ptr->mimic_form != MIMIC_GIGANTIC)
	{
		switch (p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
			//p_ptr->hold_life = TRUE;
			p_ptr->resist_chaos = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->oppose_fire = 1;
			p_ptr->see_inv=TRUE;
			new_speed += 3;
			p_ptr->redraw |= PR_STATUS;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
			p_ptr->align -= 200;
			break;
		case MIMIC_DEMON_LORD:
		//	p_ptr->hold_life = TRUE;
			p_ptr->resist_chaos = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->immune_fire = TRUE;
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->resist_disen = TRUE;
			///del131228 因果混乱耐性
			//p_ptr->resist_nexus = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->sh_fire = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->telepathy = TRUE;
			p_ptr->levitation = TRUE;
			p_ptr->kill_wall = TRUE;
			new_speed += 5;
			p_ptr->to_a += 20;
			p_ptr->dis_to_a += 20;
			p_ptr->align -= 200;
			break;
		case MIMIC_VAMPIRE:
			p_ptr->resist_dark = TRUE;
			p_ptr->levitation = TRUE;
		//	p_ptr->hold_life = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->see_inv = TRUE;
			new_speed += 3;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
			if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
			break;
		//獣変身時 r酸電火冷毒乱麻恐 急警　俊足 加速lev/2 ノクト
		case MIMIC_BEAST:
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->speedster = TRUE;
			p_ptr->see_inv = TRUE;
			new_speed += p_ptr->lev / 2;
			p_ptr->warning = TRUE;
			p_ptr->see_nocto = TRUE;
			p_ptr->to_a += plev * 2;
			p_ptr->dis_to_a += plev * 2;
			break;
		case MIMIC_SLIME:
			p_ptr->resist_acid = TRUE;
			p_ptr->immune_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->resist_blind = TRUE;
			p_ptr->resist_insanity = TRUE;
			p_ptr->resist_shard = TRUE;
			p_ptr->resist_sound = TRUE;
			p_ptr->resist_water = TRUE;

			p_ptr->regenerate = TRUE;
			p_ptr->see_inv = TRUE;
			new_speed += p_ptr->lev / 5;
			p_ptr->see_nocto = TRUE;
			p_ptr->to_a += plev * 3;
			p_ptr->dis_to_a += plev * 3;

			break;
		case MIMIC_MARISA:
		{
			p_ptr->levitation = TRUE;
			p_ptr->speedster = TRUE;
			new_speed += p_ptr->lev / 10;
			break;
		}
		case MIMIC_MIST:
		{
			p_ptr->levitation = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_blind = TRUE;
			p_ptr->resist_shard = TRUE;
			p_ptr->resist_sound = TRUE;
			break;
		}

		//竜変身時 r酸電火冷毒麻　それ以外は詠唱時のレベルと能力に依存
		case MIMIC_DRAGON:
		{
			int rank = p_ptr->mimic_dragon_rank;

			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_nocto = TRUE;
			new_speed += rank / 5;
			p_ptr->to_a += rank * 3 / 2;
			p_ptr->dis_to_a += rank * 3 / 2;

			if(rank > 99) p_ptr->resist_pois = TRUE;
			if(rank > 99) p_ptr->levitation = TRUE;
			if(rank > 99) p_ptr->see_inv = TRUE;

			if(rank > 102) p_ptr->resist_fear = TRUE;
			if(rank > 104) p_ptr->regenerate = TRUE;
			if(rank > 106) p_ptr->resist_conf = TRUE;

			if(rank > 110) p_ptr->resist_chaos = TRUE;
			if(rank > 112) p_ptr->resist_water = TRUE;
			if(rank > 114) p_ptr->resist_blind = TRUE;
			if(rank > 115) p_ptr->sh_fire = TRUE;

			if(rank > 116) p_ptr->sustain_str = TRUE;
			if(rank > 117) p_ptr->sustain_con = TRUE;
			if(rank > 118) p_ptr->sustain_dex = TRUE;
			if(rank > 119) p_ptr->resist_lite = TRUE;
			if(rank > 120) p_ptr->resist_dark = TRUE;
			if(rank > 122) p_ptr->sh_elec = TRUE;

			if(rank > 124) p_ptr->telepathy = TRUE;
			if(rank > 126) p_ptr->resist_shard = TRUE;

			if(rank > 127) p_ptr->resist_sound = TRUE;
			if(rank > 128) p_ptr->resist_neth = TRUE;
			if(rank > 129) p_ptr->immune_fire = TRUE;

			/*:::一時強化抜きではrankの限界は130まで(ハイメイジ除く)*/
			if(rank > 134) p_ptr->resist_disen = TRUE;
			if(rank > 134) p_ptr->sh_cold = TRUE;
			if(rank > 134) p_ptr->sustain_int = TRUE;
			if(rank > 134) p_ptr->sustain_wis = TRUE;
			if(rank > 134) p_ptr->sustain_chr = TRUE;

			if(rank >139) new_speed += 10;
			if(rank >139) p_ptr->resist_time = TRUE;

			if(rank >144) p_ptr->resist_holy = TRUE;
			if(rank >144) p_ptr->speedster = TRUE;
			if(rank >144) p_ptr->immune_elec = TRUE;

			if(rank > 149) p_ptr->resist_insanity = TRUE;
			if(rank > 149) p_ptr->pass_wall = TRUE;
			else if(rank > 129) p_ptr->kill_wall = TRUE;

			//ハイメイジが黄金の蜂蜜酒とか飲んだとき
			if(rank > 159) p_ptr->immune_acid = TRUE;
			if(rank > 159) p_ptr->immune_cold = TRUE;
		}
		break;

		case MIMIC_CAT:
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->speedster = TRUE;
			p_ptr->see_inv = TRUE;
			new_speed += p_ptr->lev / 2;
			p_ptr->warning = TRUE;
			p_ptr->see_nocto = TRUE;
			p_ptr->to_a += plev * 3 / 2;
			p_ptr->dis_to_a += plev * 3 / 2;
			break;
		case MIMIC_NUE:
			p_ptr->levitation = TRUE;
			p_ptr->resist_dark = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			if (p_ptr->lev > 4) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev > 9) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_acid = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_chaos = TRUE;
			if (p_ptr->lev > 24) p_ptr->resist_neth = TRUE;
			if (p_ptr->lev > 29) p_ptr->speedster = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 39) p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 39)p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 39)p_ptr->resist_insanity = TRUE;
			break;

		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			{
				monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

				p_ptr->see_nocto = TRUE;
				p_ptr->to_a += r_ptr->ac;
				p_ptr->dis_to_a += r_ptr->ac;

				if(r_ptr->flags2 & RF2_REFLECTING) p_ptr->reflect = TRUE;
				if(r_ptr->flags2 & RF2_INVISIBLE) p_ptr->skill_stl += 4;
				if(r_ptr->flags2 & RF2_COLD_BLOOD) p_ptr->skill_stl += 1;
				if(r_ptr->flags2 & RF2_REGENERATE) p_ptr->regenerate = TRUE;
				if(r_ptr->flags2 & RF2_ELDRITCH_HORROR || r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) p_ptr->resist_insanity = TRUE;
				if(r_ptr->flags2 & RF2_AURA_FIRE) p_ptr->sh_fire = TRUE;
				if(r_ptr->flags2 & RF2_AURA_ELEC) p_ptr->sh_elec = TRUE;
				if(r_ptr->flags2 & RF2_AURA_COLD) p_ptr->sh_cold = TRUE;
				if(r_ptr->flags2 & RF2_PASS_WALL) p_ptr->pass_wall = TRUE;
				else if(r_ptr->flags2 & RF2_KILL_WALL) p_ptr->kill_wall = TRUE;
				if(r_ptr->flags2 & RF2_GIGANTIC) p_ptr->skill_stl -= 4;

				if(r_ptr->flags3 & RF3_NO_FEAR) p_ptr->resist_fear = TRUE;
				if(r_ptr->flags3 & RF3_NO_CONF) p_ptr->resist_conf = TRUE;

				if(r_ptr->flags7 & RF7_CAN_FLY) p_ptr->levitation = TRUE;

				if(r_ptr->flagsr & RFR_IM_ACID) p_ptr->resist_acid = TRUE;
				if(r_ptr->flagsr & RFR_IM_FIRE) p_ptr->resist_fire = TRUE;
				if(r_ptr->flagsr & RFR_IM_COLD) p_ptr->resist_cold = TRUE;
				if(r_ptr->flagsr & RFR_IM_ELEC) p_ptr->resist_elec = TRUE;
				if(r_ptr->flagsr & RFR_IM_POIS) p_ptr->resist_pois = TRUE;

				if(r_ptr->flagsr & RFR_RES_LITE) p_ptr->resist_lite = TRUE;
				if(r_ptr->flagsr & RFR_RES_DARK) p_ptr->resist_dark = TRUE;
				if(r_ptr->flagsr & RFR_RES_NETH) p_ptr->resist_neth = TRUE;
				if(r_ptr->flagsr & RFR_RES_WATE) p_ptr->resist_water = TRUE;
				if(r_ptr->flagsr & RFR_RES_SHAR) p_ptr->resist_shard = TRUE;
				if(r_ptr->flagsr & RFR_RES_SOUN) p_ptr->resist_sound = TRUE;
				if(r_ptr->flagsr & RFR_RES_CHAO) p_ptr->resist_chaos = TRUE;
				if(r_ptr->flagsr & RFR_RES_HOLY) p_ptr->resist_holy = TRUE;
				if(r_ptr->flagsr & RFR_RES_DISE) p_ptr->resist_disen = TRUE;
				if(r_ptr->flagsr & RFR_RES_TIME) p_ptr->resist_time = TRUE;

				new_speed += (r_ptr->speed - 110);

			}
			break;

		case MIMIC_KOSUZU_GHOST:
			{
				p_ptr->see_inv = TRUE;
				p_ptr->resist_cold = TRUE;
				p_ptr->resist_neth = TRUE;
				p_ptr->levitation = TRUE;
				p_ptr->resist_fear = TRUE;
				p_ptr->esp_human = TRUE;
			}
			break;
		case MIMIC_KOSUZU_HYAKKI:
			{
				int bonus = plev / 3;
				if(p_ptr->do_martialarts) bonus *= 2;

				p_ptr->see_inv = TRUE;
				p_ptr->resist_shard = TRUE;
				p_ptr->resist_dark = TRUE;
				p_ptr->resist_neth = TRUE;
				p_ptr->resist_fear = TRUE;
				p_ptr->resist_conf = TRUE;
				p_ptr->resist_insanity = TRUE;
				p_ptr->resist_blind = TRUE;
				p_ptr->free_act = TRUE;

				new_speed += 1 + plev / 12;
				p_ptr->to_a += plev/2+5;
				p_ptr->dis_to_a += plev/2+5;
				extra_blows[0] += 1;
				extra_blows[1] += 1;

				p_ptr->to_h[0] += bonus;
				p_ptr->to_h[1] += bonus;
				p_ptr->to_h_m  += bonus;
				p_ptr->to_d[0] += bonus / 2;
				p_ptr->to_d[1] += bonus / 2;
				p_ptr->to_d_m  += bonus / 2;
				p_ptr->dis_to_h[0] += bonus;
				p_ptr->dis_to_h[1] += bonus;
				p_ptr->dis_to_d[0] += bonus / 2;
				p_ptr->dis_to_d[1] += bonus / 2;
			}
			break;

		case MIMIC_GOD_OF_NEW_WORLD:
			p_ptr->regenerate = TRUE;
			p_ptr->levitation = TRUE;
			p_ptr->slow_digest = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_holy = TRUE;
			p_ptr->resist_lite = TRUE;
			p_ptr->resist_dark = TRUE;
			extra_blows[0] += 1;
			extra_blows[1] += 1;
			new_speed += 5;

			break;

			//巨大化の処理は通常の種族に準じることにした
			/*
		case MIMIC_GIGANTIC:
			p_ptr->free_act = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->regenerate = TRUE;
			if (p_ptr->pclass == CLASS_SUIKA) p_ptr->resist_dark = TRUE;
			p_ptr->sustain_str = TRUE;
			p_ptr->sustain_con = TRUE;
			p_ptr->resist_insanity = TRUE;
			*/

			break;

		}
	}
	/*:::種族関係処理(通常時)*/
	///res race 種族耐性
	else
	{
		switch (p_ptr->prace)
		{
		case RACE_HALF_YOUKAI:
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
			break;

		case RACE_SENNIN:
			if(p_ptr->pclass != CLASS_KASEN) p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 19 && p_ptr->pclass != CLASS_KASEN)
			{
				if( p_ptr->realm1>TV_BOOK_CHAOS || p_ptr->realm1 < TV_BOOK_TRANSFORM ) p_ptr->resist_holy = TRUE;
				else if ( p_ptr->realm1==TV_BOOK_CHAOS)p_ptr->resist_chaos = TRUE;
				else if ( p_ptr->realm1==TV_BOOK_TRANSFORM)p_ptr->resist_pois = TRUE;
				else if ( p_ptr->realm1==TV_BOOK_NECROMANCY)p_ptr->resist_neth = TRUE;
				else if ( p_ptr->realm1==TV_BOOK_DARKNESS)p_ptr->resist_dark = TRUE;
			}

			if (p_ptr->lev > 24) p_ptr->sustain_wis = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 39) p_ptr->levitation = TRUE;
			break;
		case RACE_TENNIN:
			p_ptr->resist_fear = TRUE;
			p_ptr->resist_holy = TRUE;
			p_ptr->levitation = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->esp_undead = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_lite = TRUE;
			if (p_ptr->lev > 24) p_ptr->sustain_wis = TRUE;
			break;

		case RACE_FAIRY:
			p_ptr->regenerate = TRUE;
			p_ptr->levitation = TRUE;
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_holy = TRUE;
			break;
		case RACE_KAPPA:
			p_ptr->resist_water = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_acid = TRUE;
			break;
		case RACE_MAGICIAN:
			if (p_ptr->lev > 9) p_ptr->levitation = TRUE;
			if (p_ptr->lev > 15) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 24) p_ptr->sustain_int = TRUE;
			break;
		case RACE_KARASU_TENGU:
			p_ptr->levitation = TRUE;
			if (p_ptr->lev > 24) p_ptr->speedster = TRUE;
			if(p_ptr->pclass != CLASS_AYA) new_speed += (p_ptr->lev) / 7;
			break;
		case RACE_HAKUROU_TENGU:
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->warning = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_pois = TRUE;
			break;
		case RACE_ONI:
			p_ptr->resist_fear = TRUE;
			p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 9) p_ptr->resist_dark = TRUE;
			if (p_ptr->lev > 19) p_ptr->sustain_str = TRUE;
			if (p_ptr->lev > 19) p_ptr->sustain_con = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_insanity = TRUE;
			break;
		case RACE_DEATH:
			p_ptr->resist_neth = TRUE;
			p_ptr->free_act = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 24) p_ptr->esp_undead = TRUE;
			if (p_ptr->lev > 24) p_ptr->esp_human = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_dark = TRUE;
			if (p_ptr->lev > 44) p_ptr->resist_insanity = TRUE;

			break;
		case RACE_WARBEAST:
			if (p_ptr->lev > 29) p_ptr->warning = TRUE;
			if(p_ptr->pclass != CLASS_AUNN)
				new_speed += (p_ptr->lev) / 15;
			break;
		case RACE_TSUKUMO:
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_blind = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			break;
		case RACE_ANDROID:
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->free_act = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 39) p_ptr->resist_insanity = TRUE;
			break;

		case RACE_GYOKUTO:
			if(p_ptr->pclass != CLASS_UDONGE) p_ptr->resist_holy = TRUE;
			break;
		case RACE_YOUKO:
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 9) p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 19) p_ptr->warning = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
			if (p_ptr->lev > 39) p_ptr->telepathy = TRUE;
			if((p_ptr->realm1 < TV_BOOK_TRANSFORM  || p_ptr->realm1 > TV_BOOK_CHAOS) && p_ptr->lev > 44) p_ptr->resist_holy = TRUE;
			break;
		case RACE_YAMAWARO:
			p_ptr->resist_water = TRUE;
			break;
		case RACE_BAKEDANUKI:
			if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
			break;
		case RACE_NINGYO:
			p_ptr->resist_water = TRUE;
			break;

		case RACE_HOFGOBLIN:
			p_ptr->slow_digest = TRUE;
			p_ptr->free_act = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_pois = TRUE;
			break;
		case RACE_NYUDOU:
			if (p_ptr->lev > 9) p_ptr->free_act = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_shard = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_sound = TRUE;
			break;
		case RACE_IMP:
			p_ptr->resist_dark = TRUE;
			p_ptr->resist_insanity = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_neth = TRUE;
			break;

		case RACE_GOLEM:
			new_speed += -3 + p_ptr->lev / 15;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->resist_blind = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 19) p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 19) p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_insanity = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_neth = TRUE;

		break;

		case RACE_ZOMBIE:
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 14) p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 14) p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
			p_ptr->resist_insanity = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_dark = TRUE;
			if (p_ptr->lev > 29) p_ptr->levitation = TRUE;

			break;

		case RACE_VAMPIRE:
			p_ptr->resist_dark = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_insanity = TRUE;
			if (p_ptr->lev > 34) p_ptr->levitation = TRUE;
			if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
			if (p_ptr->lev > 39) p_ptr->speedster = TRUE;
			new_speed += (p_ptr->lev) / 10;

			break;
		case RACE_SPECTRE:
			p_ptr->levitation = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_shard = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->pass_wall = TRUE;
			if (p_ptr->lev > 34) p_ptr->telepathy = TRUE;
			break;

		case RACE_DAIYOUKAI:
			p_ptr->levitation = TRUE;
			p_ptr->resist_dark = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 4) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev > 9) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_acid = TRUE;
			if (p_ptr->lev > 24) p_ptr->resist_pois = TRUE;
			if (p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_fear = TRUE;
			if (p_ptr->lev > 39) p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 39)p_ptr->slow_digest = TRUE;
			break;
		case RACE_DEMIGOD:
			p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 24) p_ptr->regenerate = TRUE;
			if (p_ptr->lev > 24) p_ptr->slow_digest = TRUE;
			break;

		case RACE_NINJA:
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->resist_blind = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->slow_digest = TRUE;
			p_ptr->speedster = TRUE;
			p_ptr->mighty_throw = TRUE;
			new_speed += (p_ptr->lev) / 7;

		break;

		case RACE_ULTIMATE:
			new_speed += (p_ptr->lev) / 7;
			p_ptr->kill_wall = TRUE;
			break;

		case RACE_LICH:
			p_ptr->free_act = TRUE;
			p_ptr->resist_blind = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->resist_fear = TRUE;
			p_ptr->resist_insanity = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_dark = TRUE;
			p_ptr->resist_chaos = TRUE;
			p_ptr->resist_time = TRUE;
			p_ptr->telepathy = TRUE;
			break;


		case RACE_STRAYGOD:
			{
				int j;
				for(j=A_STR;j<=A_CHR;j++) p_ptr->stat_add[j] += deity_table[p_ptr->race_multipur_val[3]].stat_adj[j];
				apply_deity_ability(p_ptr->race_multipur_val[3],&new_speed);
				apply_deity_ability(p_ptr->race_multipur_val[4] / 128,&new_speed);
				apply_deity_ability(p_ptr->race_multipur_val[4] % 128,&new_speed);

			}
			break;
		case RACE_HOURAI:
			p_ptr->resist_neth = TRUE;
			p_ptr->slow_digest = TRUE;
			break;

		case RACE_LUNARIAN:
			p_ptr->see_inv = TRUE;
			p_ptr->sustain_int = TRUE;
			if (p_ptr->lev >  9)	p_ptr->resist_conf = TRUE;
			if (p_ptr->lev > 19)	p_ptr->warning = TRUE;
			if (p_ptr->lev > 29)	p_ptr->easy_spell = TRUE;
			if (p_ptr->lev > 39)	p_ptr->resist_time = TRUE;

			break;

		case RACE_MAGIC_JIZO:
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_holy = TRUE;
			if(plev > 9) p_ptr->resist_fire = TRUE;
			if(plev > 19) p_ptr->sustain_wis = TRUE;
			if(plev > 29) p_ptr->resist_conf = TRUE;
			if(plev > 39) p_ptr->resist_neth = TRUE;

		break;

		case RACE_HALF_GHOST:
			p_ptr->resist_neth = TRUE;
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
			break;

		case RACE_HANIWA:
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->regenerate = TRUE;
			p_ptr->resist_insanity = TRUE;
			if(p_ptr->lev > 19) p_ptr->see_inv = TRUE;


			break;
		case RACE_ANIMAL_GHOST:
			p_ptr->regenerate = TRUE;
			p_ptr->levitation = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 29)p_ptr->warning = TRUE;
			new_speed += (p_ptr->lev) / 15;

			break;

		case RACE_ZASHIKIWARASHI:

			p_ptr->warning = TRUE;
			if(plev > 29)p_ptr->esp_human = TRUE;

			break;



		//case RACE_ELF:
			//p_ptr->resist_lite = TRUE;
			//break;
		//case RACE_HOBBIT:
			//p_ptr->hold_life = TRUE;
			//break;
		//case RACE_GNOME:
			//p_ptr->free_act = TRUE;
			//break;
		//case RACE_DWARF:
		//	p_ptr->resist_blind = TRUE;
		//	break;
		//case RACE_HALF_ORC:
			//p_ptr->resist_dark = TRUE;
			//break;

		//case RACE_HALF_TROLL:
			//p_ptr->sustain_str = TRUE;

		//	if (p_ptr->lev > 14)
		//	{
		//		/* High level trolls heal fast... */
		//		p_ptr->regenerate = TRUE;

			//	if (p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_BERSERKER)
				//{
				//	p_ptr->slow_digest = TRUE;
					/* Let's not make Regeneration
					 * a disadvantage for the poor warriors who can
					 * never learn a spell that satisfies hunger (actually
					 * neither can rogues, but half-trolls are not
					 * supposed to play rogues) */
			//	}
			//}
			//break;
		//case RACE_AMBERITE:
		//	p_ptr->sustain_con = TRUE;
		//	p_ptr->regenerate = TRUE;  /* Amberites heal fast... */
		//	break;
		//case RACE_HIGH_ELF:
		//	p_ptr->resist_lite = TRUE;
		//	p_ptr->see_inv = TRUE;
		//	break;
		//case RACE_BARBARIAN:
		//	p_ptr->resist_fear = TRUE;
		//	break;
		//case RACE_HALF_OGRE:
			//p_ptr->resist_dark = TRUE;
		//	p_ptr->sustain_str = TRUE;
		//	break;
		//case RACE_HALF_GIANT:
		//	p_ptr->sustain_str = TRUE;
		//	p_ptr->resist_shard = TRUE;
		//	break;
		//case RACE_HALF_TITAN:
		//	p_ptr->resist_chaos = TRUE;
		//	break;
		//case RACE_CYCLOPS:
		//	p_ptr->resist_sound = TRUE;
		//	break;
		//case RACE_YEEK:
		//	p_ptr->resist_acid = TRUE;
		//	if (p_ptr->lev > 19) p_ptr->immune_acid = TRUE;
		//	break;
		//case RACE_KLACKON:
		//	p_ptr->resist_conf = TRUE;
		//	p_ptr->resist_acid = TRUE;

			/* Klackons become faster */
		//	new_speed += (p_ptr->lev) / 10;
		//	break;
		//case RACE_KOBOLD:
		//	p_ptr->resist_pois = TRUE;
		//	break;
		//case RACE_NIBELUNG:
		//	p_ptr->resist_disen = TRUE;
		//	p_ptr->resist_dark = TRUE;
		//	break;
		//case RACE_DARK_ELF:
		//	p_ptr->resist_dark = TRUE;
		//	if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
		//	break;
/*
		case RACE_DRACONIAN:
			p_ptr->levitation = TRUE;
			if (p_ptr->lev >  4) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_acid = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_pois = TRUE;
			break;
		case RACE_MIND_FLAYER:
			p_ptr->sustain_int = TRUE;
			p_ptr->sustain_wis = TRUE;
			if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->telepathy = TRUE;
			break;
		case RACE_IMP:
			p_ptr->resist_fire = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			break;
		case RACE_GOLEM:
			p_ptr->slow_digest = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			//if (p_ptr->lev > 34) p_ptr->hold_life = TRUE;
			break;
			*/
		//case RACE_SKELETON:
		//	p_ptr->resist_shard = TRUE;
		//	p_ptr->hold_life = TRUE;
		//	p_ptr->see_inv = TRUE;
		//	p_ptr->resist_pois = TRUE;
		//	if (p_ptr->lev > 9) p_ptr->resist_cold = TRUE;
		//	break;
		//case RACE_ZOMBIE:
		//	p_ptr->resist_neth = TRUE;
		//	p_ptr->hold_life = TRUE;
		//	p_ptr->see_inv = TRUE;
		//	p_ptr->resist_pois = TRUE;
		//	p_ptr->slow_digest = TRUE;
		//	if (p_ptr->lev > 4) p_ptr->resist_cold = TRUE;
		//	break;
		//case RACE_VAMPIRE:
			/*:::暗黒免疫処理はダメージを受けたとき個別で行っている*/
		//	p_ptr->resist_dark = TRUE;
		//	p_ptr->hold_life = TRUE;
		//	p_ptr->resist_neth = TRUE;
		//	p_ptr->resist_cold = TRUE;
		//	p_ptr->resist_pois = TRUE;
		//	if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
		//	break;
		//case RACE_SPECTRE:
		//	p_ptr->levitation = TRUE;
		//	p_ptr->free_act = TRUE;
			/*:::地獄吸収処理はダメージを受けたとき個別で行っている*/
		//	p_ptr->resist_neth = TRUE;
		//	p_ptr->hold_life = TRUE;
		//	p_ptr->see_inv = TRUE;
		//	p_ptr->resist_pois = TRUE;
		//	p_ptr->slow_digest = TRUE;
		//	p_ptr->resist_cold = TRUE;
		//	p_ptr->pass_wall = TRUE;
		//	if (p_ptr->lev > 34) p_ptr->telepathy = TRUE;
		//	break;
		//case RACE_SPRITE:
		//	p_ptr->levitation = TRUE;
		//	p_ptr->resist_lite = TRUE;

			/* Sprites become faster */
		//	new_speed += (p_ptr->lev) / 10;
		//	break;
		//case RACE_BEASTMAN:
		//	p_ptr->resist_conf  = TRUE;
		//	p_ptr->resist_sound = TRUE;
		//	break;
		case RACE_ENT:
			/* Ents dig like maniacs, but only with their hands. */
			if (!inventory[INVEN_RARM].k_idx)
				p_ptr->skill_dig += p_ptr->lev * 10;
			/* Ents get tougher and stronger as they age, but lose dexterity. */
			if (p_ptr->lev > 25) p_ptr->stat_add[A_STR]++;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_STR]++;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_STR]++;

			if (p_ptr->lev > 25) p_ptr->stat_add[A_DEX]--;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_DEX]--;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_DEX]--;

			if (p_ptr->lev > 25) p_ptr->stat_add[A_CON]++;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_CON]++;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_CON]++;
			break;
		case RACE_ANGEL:
			p_ptr->levitation = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->align += 200;
			break;
			/*
		case RACE_DEMON:
			p_ptr->resist_fire  = TRUE;
			p_ptr->resist_neth  = TRUE;
		//	p_ptr->hold_life = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 44)
			{
				p_ptr->oppose_fire = 1;
				p_ptr->redraw |= PR_STATUS;
			}
			p_ptr->align -= 200;
			break;
			*/
			/*
		case RACE_DUNADAN:
			p_ptr->sustain_con = TRUE;
			break;
		case RACE_S_FAIRY:
			p_ptr->levitation = TRUE;
			break;
		case RACE_KUTAR:
			p_ptr->resist_conf = TRUE;
			break;
			*/
	//	case RACE_ANDROID:
	//		p_ptr->slow_digest = TRUE;
	//		p_ptr->free_act = TRUE;
	//		p_ptr->resist_pois = TRUE;
	//		p_ptr->hold_life = TRUE;
	//		break;
		default:
			/* Do nothing */
			;
		}
	}
/*:::種族処理終了*/

	switch(CHECK_FAIRY_TYPE)
	{
	case 11:
		if(p_ptr->chp < p_ptr->mhp/2) p_ptr->speedster = TRUE;
		break;
	case 16:
		p_ptr->skill_sav += plev / 5 + 10;
		break;
	case 17:
		p_ptr->skill_stl += 1 + plev / 15;
		break;
	case 18:
		p_ptr->skill_srh += 5 + plev / 10;
		break;
	case 19:
		p_ptr->skill_thb += 15 + plev / 5;
		p_ptr->skill_tht += 15 + plev / 5;
		break;
	case 20:
		p_ptr->skill_dis += 15 + plev / 5;
		break;

	default:
		break;

	}


	/*:::究極の耐性・夢想の型処理*/
	///res
	if (p_ptr->ult_res || (p_ptr->special_defense & KATA_MUSOU) || p_ptr->prace == RACE_ULTIMATE || SUPER_SHION)
	{
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->see_inv = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->slow_digest = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->levitation = TRUE;
		//p_ptr->hold_life = TRUE;
		p_ptr->telepathy = TRUE;
		p_ptr->lite = TRUE;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_chr = TRUE;
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_elec = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_sound = TRUE;
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_disen = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->sh_fire = TRUE;
		p_ptr->sh_elec = TRUE;
		p_ptr->sh_cold = TRUE;
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_water = TRUE;
		p_ptr->resist_insanity = TRUE;

	}

	else if(p_ptr->foresight || p_ptr->mimic_form == MIMIC_MIST)
	{
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;

	}
	/* Temporary shield */
	/*:::横に伸びる、石肌、魔法の鎧*/
	else if (p_ptr->tim_sh_death || p_ptr->shield || p_ptr->magicdef || (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME))
	{
		p_ptr->to_a += 50;
		p_ptr->dis_to_a += 50;
	}

	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0])
	{
		p_ptr->to_a += 30;
		p_ptr->dis_to_a += 30;
		p_ptr->skill_sav += 20;
		p_ptr->resist_lite = TRUE;
	}

	if(p_ptr->clawextend)
	{
		p_ptr->to_a += 30;
		p_ptr->dis_to_a += 30;
		p_ptr->to_d[0] += 4 + plev / 3;
		p_ptr->to_d[1] += 4 + plev / 3;
		p_ptr->to_d_m  += 4 + plev / 3;
		p_ptr->dis_to_d[0] += 4 + plev / 3;
		p_ptr->dis_to_d[1] += 4 + plev / 3;
	}
	/*:::高速移動（一時）*/
	if (p_ptr->tim_speedster)
	{
		p_ptr->speedster = TRUE;
	}
	///mod150419 格闘家が素手時に気を溜めるとパワーアップ
	if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->do_martialarts && (p_ptr->csp > p_ptr->msp || p_ptr->tim_general[0]))
	{
		int power = MAX(0,(p_ptr->csp - p_ptr->msp));
		if(p_ptr->tim_general[0]) power += 100;
		p_ptr->to_a += power / 7;
		p_ptr->dis_to_a += power / 7;
		p_ptr->to_d[0] += power / 10;
		p_ptr->to_d[1] += power / 10;
		p_ptr->to_d_m  += power / 10;
		p_ptr->dis_to_d[0] += power / 10;
		p_ptr->dis_to_d[1] += power / 10;

	}


	/*:::対地獄（一時）*/
	if (p_ptr->tim_res_nether || p_ptr->tim_sh_death)
	{
		p_ptr->resist_neth = TRUE;
	}
	/*:::火のオーラ(一時)*/
	if (p_ptr->tim_sh_fire)
	{
		p_ptr->sh_fire = TRUE;
	}
	/*:::時間逆転耐性(一時)*/
	if (p_ptr->tim_res_time)
	{
		p_ptr->resist_time = TRUE;
	}
	if (p_ptr->tim_res_chaos)
	{
		p_ptr->resist_chaos = TRUE;
	}
	if (p_ptr->tim_res_holy)
	{
		p_ptr->resist_holy = TRUE;
	}
	if (p_ptr->tim_res_water)
	{
		p_ptr->resist_water = TRUE;
	}
	if (p_ptr->tim_res_insanity)
	{
		p_ptr->resist_insanity = TRUE;
	}
	if (p_ptr->tim_res_dark)
	{
		p_ptr->resist_dark = TRUE;
	}



	/* Sexy Gal */
	if (p_ptr->pseikaku == SEIKAKU_SEXY) p_ptr->cursed |= (TRC_AGGRAVATE);
//	if (p_ptr->pseikaku == SEIKAKU_NAMAKE) p_ptr->to_m_chance += 10;
	if (p_ptr->pseikaku == SEIKAKU_KIREMONO) p_ptr->to_m_chance -= 3;
	if (p_ptr->pseikaku == SEIKAKU_TRIGGER) p_ptr->to_m_chance -= 5;
	if ((p_ptr->pseikaku == SEIKAKU_GAMAN) || (p_ptr->pseikaku == SEIKAKU_CHIKARA)) p_ptr->to_m_chance++;

	/* Lucky man */
	if (p_ptr->pseikaku == SEIKAKU_LUCKY) p_ptr->muta3 |= MUT3_GOOD_LUCK;

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf  = TRUE;
	//	p_ptr->hold_life = TRUE;
		if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;

		//if ((p_ptr->prace != RACE_KLACKON) && (p_ptr->prace != RACE_SPRITE))
			/* Munchkin become faster */
			new_speed += (p_ptr->lev) / 10 + 5;
	}
	else if( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		p_ptr->shero = 1;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->free_act = TRUE;
		if (p_ptr->lev > 39) p_ptr->speedster = TRUE;
		new_speed += 2;
		if (p_ptr->lev > 29) new_speed++;
		if (p_ptr->lev > 39) new_speed++;
		if (p_ptr->lev > 44) new_speed++;
		if (p_ptr->lev > 49) new_speed++;
		p_ptr->to_a += 10+p_ptr->lev/2;
		p_ptr->dis_to_a += 10+p_ptr->lev/2;
		p_ptr->skill_dig += (100+p_ptr->lev*8);
		p_ptr->redraw |= PR_STATUS;

		p_ptr->to_d[0] += 3 + plev / 4;
		p_ptr->to_d[1] += 3 + plev / 4;
		p_ptr->to_d_m  += 3 + plev / 4;
		p_ptr->dis_to_d[0] += 3 + plev / 4;
		p_ptr->dis_to_d[1] += 3 + plev / 4;

	}

/*
	if (music_singing(MUSIC_WALL))
	{
		p_ptr->kill_wall = TRUE;
	}
*/
	/* Hack -- apply racial/class stat maxes */
	/* Apply the racial modifiers */
	for (i = 0; i < 6; i++)
	{
		/* Modify the stats for "race" */
		/*:::種族･職業・性格によるSTR-CHAパラメータ補正値を計算*/
		p_ptr->stat_add[i] += (tmp_rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i]);
	}

	///mod140121 石頭変異のペナルティなど
	if (p_ptr->muta2 & MUT2_HARDHEAD)
	{
		p_ptr->stat_add[A_INT] -= 1;
		p_ptr->stat_add[A_WIS] += 1;
	}
	if (p_ptr->muta2 & MUT2_TAIL)
	{
		p_ptr->stat_add[A_DEX] += 4;
	}
	//神秘ハイのボーナス
	if(p_ptr->pclass == CLASS_HIGH_MAGE && (p_ptr->realm1 == TV_BOOK_MYSTIC))
	{
			extra_blows[0] += (p_ptr->lev/40);
			extra_blows[1] += (p_ptr->lev/40);
	}
	//パルスィはHPが減るとパワーアップ キャラメイク時の画面に出ないようフラグ処理
	else if(p_ptr->pclass == CLASS_PARSEE && character_dungeon)
	{
		int to_d = (p_ptr->mhp - p_ptr->chp) / 20;
		int to_h = (p_ptr->mhp - p_ptr->chp) / 20;
		int to_b = (p_ptr->mhp - p_ptr->chp) * 5 / p_ptr->mhp;

		extra_blows[0] += to_b;
		extra_blows[1] += to_b;

		p_ptr->to_h[0] += to_h;
		p_ptr->to_h[1] += to_h;
		p_ptr->to_h_m  += to_h;
		p_ptr->to_d[0] += to_d;
		p_ptr->to_d[1] += to_d;
		p_ptr->to_d_m  += to_d;
		p_ptr->dis_to_h[0] += to_h;
		p_ptr->dis_to_h[1] += to_h;
		p_ptr->dis_to_d[0] += to_d;
		p_ptr->dis_to_d[1] += to_d;

		new_speed += (p_ptr->mhp - p_ptr->chp) / (15+plev/2);

	}


	///mod140316 地形によるパラメータ変化
	//ここを変更したらfiles.cのdisplay_player_stat_info()とplayer_flags()も同様に変更すること
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		//種族
		//河童は水地形でパワーアップ
		if(p_ptr->prace == RACE_KAPPA && have_flag(f_ptr->flags, FF_WATER))
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_CON] += 2;
			p_ptr->regenerate = TRUE;
		}
		//v1.1.86 山童は水でなく森でパワーアップ
		else if (p_ptr->prace == RACE_YAMAWARO && have_flag(f_ptr->flags, FF_TREE))
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_CON] += 2;
			p_ptr->regenerate = TRUE;
		}
		else if(p_ptr->prace == RACE_NINGYO && have_flag(f_ptr->flags, FF_WATER))
		{
			p_ptr->speedster = TRUE;
			new_speed += 3 + p_ptr->lev / 15;
		}
		else if(CHECK_FAIRY_TYPE == 32 && have_flag(f_ptr->flags, FF_WATER))
		{
			p_ptr->speedster = TRUE;
		}
		else if(CHECK_FAIRY_TYPE == 39 && have_flag(f_ptr->flags, FF_WATER))
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_CON] += 2;
		}
		else if(CHECK_FAIRY_TYPE == 43 && have_flag(f_ptr->flags, FF_TREE))
		{
			p_ptr->stat_add[A_STR] += 1;
			p_ptr->stat_add[A_WIS] += 1;
			p_ptr->stat_add[A_CON] += 1;
		}
		else if(CHECK_FAIRY_TYPE == 5 && have_flag(f_ptr->flags, FF_LAVA))
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_CON] += 2;
			p_ptr->to_h[0] += plev / 6;
			p_ptr->to_h[1] += plev / 6;
			p_ptr->to_h_m  += plev / 6;
			p_ptr->to_d[0] += plev / 6;
			p_ptr->to_d[1] += plev / 6;
			p_ptr->to_d_m  += plev / 6;
			p_ptr->dis_to_h[0] += plev / 6;
			p_ptr->dis_to_h[1] += plev / 6;
			p_ptr->dis_to_d[0] += plev / 6;
			p_ptr->dis_to_d[1] += plev / 6;
		}
		else if(CHECK_FAIRY_TYPE == 27 && have_flag(f_ptr->flags, FF_PLANT))
		{
			p_ptr->stat_add[A_STR] += 1;
			p_ptr->stat_add[A_WIS] += 1;
			p_ptr->stat_add[A_DEX] += 1;
			p_ptr->stat_add[A_CON] += 1;
		}
		else if(CHECK_FAIRY_TYPE == 30 && !dun_level && turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
		{
			for(i=0;i<6;i++) p_ptr->stat_add[i] += 1;
		}
		else if(CHECK_FAIRY_TYPE == 46 && p_ptr->image)
		{
			for(i=0;i<6;i++) p_ptr->stat_add[i] += 3;
			new_speed += 3;
		}
		else if (p_ptr->pclass == CLASS_YAMAME && have_flag(f_ptr->flags, FF_SPIDER_NEST_1))
		{
			p_ptr->to_a += p_ptr->lev / 2;
			p_ptr->dis_to_a += p_ptr->lev / 2;
			if (plev > 29) p_ptr->speedster = TRUE;
		}


		//職業
		//わかさぎ姫も水地形でパワーアップ
		if(p_ptr->pclass == CLASS_WAKASAGI && have_flag(f_ptr->flags, FF_WATER))
		{

			p_ptr->regenerate = TRUE;
			p_ptr->free_act = TRUE;
			for(i=0;i<6;i++) p_ptr->stat_add[i] += 1 + p_ptr->lev / 11;
			p_ptr->to_a += 25 + p_ptr->lev / 2;
			p_ptr->dis_to_a += 25 + p_ptr->lev / 2;
			p_ptr->to_h[0] += plev / 3;
			p_ptr->to_h[1] += plev / 3;
			p_ptr->to_h_m  += plev / 3;
			p_ptr->to_d[0] += plev / 5;
			p_ptr->to_d[1] += plev / 5;
			p_ptr->to_d_m  += plev / 5;
			p_ptr->dis_to_h[0] += plev / 3;
			p_ptr->dis_to_h[1] += plev / 3;
			p_ptr->dis_to_d[0] += plev / 5;
			p_ptr->dis_to_d[1] += plev / 5;
			extra_blows[0] += (p_ptr->lev/24);
			extra_blows[1] += (p_ptr->lev/24);

		}
		//村紗も水地形でパワーアップ
		else if(p_ptr->pclass == CLASS_MURASA && have_flag(f_ptr->flags, FF_WATER))
		{
			p_ptr->regenerate = TRUE;
			for(i=0;i<6;i++) p_ptr->stat_add[i] += 2;
			p_ptr->to_a += 25;
			p_ptr->dis_to_a += 25;
			p_ptr->to_h[0] += 10;
			p_ptr->to_h[1] += 10;
			p_ptr->to_h_m  += 10;
			p_ptr->to_d[0] += 5;
			p_ptr->to_d[1] += 5;
			p_ptr->to_d_m  += 5;
			p_ptr->dis_to_h[0] += 10;
			p_ptr->dis_to_h[1] += 10;
			p_ptr->dis_to_d[0] += 5;
			p_ptr->dis_to_d[1] += 5;
			new_speed += 2;

		}
		//1.1.22 兵士潜伏技能
		else if(CHECK_CONCEALMENT)
		{
			if(have_flag(f_ptr->flags,FF_WATER) && !prace_is_(RACE_VAMPIRE) || have_flag(f_ptr->flags,FF_TREE))
			{
				p_ptr->skill_stl += 2 + plev / 10;
			}
		}




	}


	/* I'm adding the mutations here for the lack of a better place... */
	/*:::MUTA1や2にここで処理される変異を入れても問題ないと思うが処理が少し冗長になる*/
	///sys 突然変異によるパラメータ計算

	/*:::Hack- 小傘の本体が呪われたらパラメータダウン*/
	if(p_ptr->pclass == CLASS_KOGASA)
	{
		int j,dummy=0;
		if(inventory[INVEN_RARM].curse_flags & TRC_HEAVY_CURSE) dummy = -3;
		else if( inventory[INVEN_RARM].curse_flags & TRC_CURSED) dummy = -1;
		for(j=0;j<6;j++)p_ptr->stat_add[j] += dummy;
	}

	/*:::アルコール中毒の＠がシラフになると全能力-3*/
	if((p_ptr->muta2 & MUT2_ALCOHOL) && p_ptr->alcohol == DRANK_0)
	{
		for(j=0;j<6;j++)p_ptr->stat_add[j] -= 3;
		new_speed -= 3;

	}
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_FORESEE) p_ptr->skill_srh += p_ptr->lev * 2;


	///mod150110 橙が濡れている時全能力-3
	if(p_ptr->pclass == CLASS_CHEN && p_ptr->magic_num1[0])
	{
		for(j=0;j<6;j++)p_ptr->stat_add[j] -= 3;
	}


	///mod140813 喘息
	if(p_ptr->asthma >= ASTHMA_4) p_ptr->stat_add[A_CON] -= 4;
	if(p_ptr->asthma >= ASTHMA_2) p_ptr->skill_stl -= 3;




	if (p_ptr->muta3)
	{
		/* Hyper Strength */
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
			p_ptr->stat_add[A_STR] += 4;
		}

		/* Puny */
		if (p_ptr->muta3 & MUT3_PUNY)
		{
			p_ptr->stat_add[A_STR] -= 4;
		}

		/* Living computer */
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
			p_ptr->stat_add[A_INT] += 4;
			p_ptr->stat_add[A_WIS] += 4;
		}

		if (p_ptr->muta3 & MUT3_WARNING)
		{
			p_ptr->warning = TRUE;
		}

		if (p_ptr->muta3 & MUT3_MORONIC)
		{
			p_ptr->stat_add[A_INT] -= 4;
			p_ptr->stat_add[A_WIS] -= 4;
		}

		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
			p_ptr->stat_add[A_CON] += 4;
		}

		if (p_ptr->muta3 & MUT3_ALBINO)
		{
			p_ptr->stat_add[A_CON] -= 4;
		}

		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
			p_ptr->speedster = TRUE;
		}
		if (p_ptr->muta3 & MUT3_RES_WATER)
		{
			p_ptr->resist_water = TRUE;
		}



		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
			p_ptr->stat_add[A_CHR] -= 3;
		}

		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
			p_ptr->see_inv = TRUE;

			//p_ptr->skill_fos += 15;
			p_ptr->skill_srh += 15;

		}

		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
			p_ptr->skill_sav += (15 + (p_ptr->lev / 5));
		}

		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
			p_ptr->skill_stl -= 3;
		}

		if (p_ptr->muta3 & MUT3_BYDO)
		{
			p_ptr->stat_add[A_STR] += 5;
			p_ptr->stat_add[A_INT] += 5;
			p_ptr->stat_add[A_WIS] += 5;
			p_ptr->stat_add[A_CON] += 5;
			p_ptr->stat_add[A_DEX] += 5;
			p_ptr->stat_add[A_CHR] -= 50;
			new_speed += 3;
			p_ptr->cursed |= TRC_AGGRAVATE;
		}

		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
			p_ptr->resist_water = TRUE;
		}

		if (p_ptr->muta3 & MUT3_ACID_SCALES)
		{
			p_ptr->resist_acid = TRUE;
			p_ptr->stat_add[A_CHR] -= 2;
		}
		if (p_ptr->muta3 & MUT3_ELEC_SCALES)
		{
			p_ptr->resist_elec = TRUE;
			p_ptr->stat_add[A_CHR] -= 2;
		}
		if (p_ptr->muta3 & MUT3_FIRE_SCALES)
		{
			p_ptr->resist_fire = TRUE;
			p_ptr->stat_add[A_CHR] -= 2;
		}
		if (p_ptr->muta3 & MUT3_COLD_SCALES)
		{
			p_ptr->resist_cold = TRUE;
			p_ptr->stat_add[A_CHR] -= 2;
		}
		if (p_ptr->muta3 & MUT3_POIS_SCALES)
		{
			p_ptr->resist_pois = TRUE;
			p_ptr->stat_add[A_CHR] -= 2;
		}


		if (p_ptr->muta3 & MUT3_WINGS)
		{
			p_ptr->levitation = TRUE;
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			p_ptr->resist_fear = TRUE;
		}

		if (p_ptr->muta3 & MUT3_FEVER)
		{
			p_ptr->stat_add[A_INT] -= 2;
			p_ptr->stat_add[A_DEX] -= 2;
		}

		if (p_ptr->muta3 & MUT3_ESP)
		{
			p_ptr->telepathy = TRUE;
		}

		if (p_ptr->muta3 & MUT3_LIMBER)
		{
			p_ptr->stat_add[A_DEX] += 3;
		}

		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
			p_ptr->stat_add[A_DEX] -= 3;
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			p_ptr->free_act = TRUE;
			p_ptr->skill_stl += 1;
		}


	}
	//v1.1.64
	if (p_ptr->muta4)
	{
		if (p_ptr->muta4 & MUT4_GHOST_WOLF)
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_INT] -= 2;
			p_ptr->stat_add[A_WIS] -= 2;
			p_ptr->stat_add[A_CON] += 2;
			p_ptr->stat_add[A_DEX] += 2;
		}
		if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
		{
			p_ptr->stat_add[A_STR] -= 2;
			p_ptr->stat_add[A_INT] += 2;
			p_ptr->stat_add[A_WIS] += 2;
			p_ptr->stat_add[A_CON] -= 2;
			p_ptr->skill_srh += 10;

		}
		if (p_ptr->muta4 & MUT4_GHOST_OTTER)
		{
			for (j = 0; j < 6; j++)p_ptr->stat_add[j] -= 1;

			p_ptr->skill_dev += 8;
			p_ptr->skill_sav += 10;
			p_ptr->skill_stl += 1;
			p_ptr->skill_srh += 5;
			p_ptr->skill_fos += 5;
			p_ptr->skill_dig += 20;

		}
		if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH)
		{
			p_ptr->stat_add[A_STR] -= 4;
			p_ptr->resist_pois = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_water = TRUE;

		}
		if (p_ptr->muta4 & MUT4_GHOST_COW)
		{
			p_ptr->stat_add[A_STR] += 4;
			p_ptr->stat_add[A_CON] += 4;
			p_ptr->skill_stl -= 4;
		}
		if (p_ptr->muta4 & MUT4_GHOST_CHICKEN)
		{
			p_ptr->stat_add[A_CHR] += 4;
			p_ptr->ac -= 20;
			p_ptr->dis_ac -= 20;
			p_ptr->to_a -= 20;
		}
		if (p_ptr->muta4 & MUT4_GHOST_TORTOISE)
		{
			p_ptr->ac += 50;
			p_ptr->dis_ac += 50;
			p_ptr->to_a += 50;
			new_speed -= 3;
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
		{
			p_ptr->skill_sav += 40;
		}
		if (p_ptr->muta4  & MUT4_GHOST_HANGOKUOH)
		{
			p_ptr->resist_neth = TRUE;
		}


	}


	/*:::つよしスペシャル（一時）*/
	if (p_ptr->tsuyoshi)
	{
		p_ptr->stat_add[A_STR] += 4;
		p_ptr->stat_add[A_CON] += 4;
	}

	if(p_ptr->nennbaku) p_ptr->anti_tele = TRUE;



	//アリスの採掘、盾耐性計算 オルレアンに盾のみ装備させて格闘することも考えられるのでalice_doll_attackは使わない
	if(p_ptr->pclass == CLASS_ALICE)
	{
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;

			o_ptr = &inven_add[i];
			object_flags(o_ptr, flgs);

			if(object_is_melee_weapon(o_ptr)) //武器の採掘
			{
				p_ptr->skill_dig += (o_ptr->weight / 10);
				if (have_flag(flgs, TR_TUNNEL)) p_ptr->skill_dig += (o_ptr->pval * 20);
			}
			else //盾のACと耐性
			{
				p_ptr->ac += o_ptr->ac;
				p_ptr->dis_ac += o_ptr->ac;
				p_ptr->to_a += o_ptr->to_a;
				if (object_is_known(o_ptr)) p_ptr->dis_to_a += o_ptr->to_a;

				if (have_flag(flgs, TR_IM_FIRE)) p_ptr->immune_fire = TRUE;
				if (have_flag(flgs, TR_IM_ACID)) p_ptr->immune_acid = TRUE;
				if (have_flag(flgs, TR_IM_COLD)) p_ptr->immune_cold = TRUE;
				if (have_flag(flgs, TR_IM_ELEC)) p_ptr->immune_elec = TRUE;

				if (have_flag(flgs, TR_RES_ACID))   p_ptr->resist_acid = TRUE;
				if (have_flag(flgs, TR_RES_ELEC))   p_ptr->resist_elec = TRUE;
				if (have_flag(flgs, TR_RES_FIRE))   p_ptr->resist_fire = TRUE;
				if (have_flag(flgs, TR_RES_COLD))   p_ptr->resist_cold = TRUE;
				if (have_flag(flgs, TR_RES_POIS))   p_ptr->resist_pois = TRUE;
				if (have_flag(flgs, TR_RES_LITE))   p_ptr->resist_lite = TRUE;
				if (have_flag(flgs, TR_RES_DARK))   p_ptr->resist_dark = TRUE;
				if (have_flag(flgs, TR_RES_SHARDS)) p_ptr->resist_shard = TRUE;
				if (have_flag(flgs, TR_RES_WATER))  p_ptr->resist_water = TRUE;
				if (have_flag(flgs, TR_RES_HOLY))  p_ptr->resist_holy = TRUE;

				if (have_flag(flgs, TR_RES_SOUND))  p_ptr->resist_sound = TRUE;
				if (have_flag(flgs, TR_RES_NETHER)) p_ptr->resist_neth = TRUE;
				if (have_flag(flgs, TR_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
				if (have_flag(flgs, TR_RES_DISEN))  p_ptr->resist_disen = TRUE;
				if (have_flag(flgs, TR_RES_TIME))  p_ptr->resist_time = TRUE;

				if (have_flag(flgs, TR_FREE_ACT))    p_ptr->free_act = TRUE;
				if (have_flag(flgs, TR_RES_BLIND))  p_ptr->resist_blind = TRUE;
				if (have_flag(flgs, TR_RES_FEAR))   p_ptr->resist_fear = TRUE;
				if (have_flag(flgs, TR_RES_CONF))   p_ptr->resist_conf = TRUE;
				if (have_flag(flgs, TR_RES_INSANITY))  p_ptr->resist_insanity = TRUE;

				if (have_flag(flgs, TR_REFLECT))  p_ptr->reflect = TRUE;

			}
		}
	}


	//v1.1.79 座敷わらしの移動屋敷パラメータ処理

	if (p_ptr->prace == RACE_ZASHIKIWARASHI && !p_ptr->mimic_form)
	{

		o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		//★☆のときはその品のパラメータをそのまま獲得することにする。
		//BMで売られる骨董品のツボも含む。
		//なんか似た処理ばっかあちこちでやってるので整理したいがどうしたものか
		if (object_is_artifact(o_ptr))
		{
			int tmp_addstat;

			object_flags(o_ptr, flgs);
			tmp_addstat = o_ptr->pval;

			p_ptr->ac += o_ptr->ac;
			p_ptr->dis_ac += o_ptr->ac;
			p_ptr->to_a += o_ptr->to_a;
			p_ptr->dis_to_a += o_ptr->to_a;

			if (have_flag(flgs, TR_STR)) p_ptr->stat_add[A_STR] += tmp_addstat;
			if (have_flag(flgs, TR_INT)) p_ptr->stat_add[A_INT] += tmp_addstat;
			if (have_flag(flgs, TR_WIS)) p_ptr->stat_add[A_WIS] += tmp_addstat;
			if (have_flag(flgs, TR_DEX)) p_ptr->stat_add[A_DEX] += tmp_addstat;
			if (have_flag(flgs, TR_CON)) p_ptr->stat_add[A_CON] += tmp_addstat;
			if (have_flag(flgs, TR_CHR)) p_ptr->stat_add[A_CHR] += tmp_addstat;

			if (have_flag(flgs, TR_MAGIC_MASTERY))    p_ptr->skill_dev += 8 * tmp_addstat;
			if (have_flag(flgs, TR_STEALTH)) p_ptr->skill_stl += tmp_addstat;
			if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_srh += tmp_addstat * 5;
			if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_fos += tmp_addstat * 5;
			if (have_flag(flgs, TR_INFRA)) p_ptr->see_infra += tmp_addstat;
			if (have_flag(flgs, TR_TUNNEL)) p_ptr->skill_dig += (tmp_addstat * 10);
			if (have_flag(flgs, TR_SPEED)) new_speed += tmp_addstat;

			if (have_flag(flgs, TR_ESP_EVIL)) p_ptr->esp_evil = TRUE;
			if (have_flag(flgs, TR_ESP_GOOD)) p_ptr->esp_good = TRUE;
			if (have_flag(flgs, TR_ESP_ANIMAL)) p_ptr->esp_animal = TRUE;
			if (have_flag(flgs, TR_ESP_HUMAN)) p_ptr->esp_human = TRUE;
			if (have_flag(flgs, TR_ESP_UNDEAD)) p_ptr->esp_undead = TRUE;
			if (have_flag(flgs, TR_ESP_DRAGON)) p_ptr->esp_dragon = TRUE;
			if (have_flag(flgs, TR_ESP_DEITY)) p_ptr->esp_deity = TRUE;
			if (have_flag(flgs, TR_ESP_DEMON)) p_ptr->esp_demon = TRUE;
			if (have_flag(flgs, TR_ESP_KWAI)) p_ptr->esp_kwai = TRUE;
			if (have_flag(flgs, TR_ESP_NONLIVING)) p_ptr->esp_nonliving = TRUE;
			if (have_flag(flgs, TR_ESP_UNIQUE)) p_ptr->esp_unique = TRUE;

			if (have_flag(flgs, TR_SUST_STR)) p_ptr->sustain_str = TRUE;
			if (have_flag(flgs, TR_SUST_INT)) p_ptr->sustain_int = TRUE;
			if (have_flag(flgs, TR_SUST_WIS)) p_ptr->sustain_wis = TRUE;
			if (have_flag(flgs, TR_SUST_DEX)) p_ptr->sustain_dex = TRUE;
			if (have_flag(flgs, TR_SUST_CON)) p_ptr->sustain_con = TRUE;
			if (have_flag(flgs, TR_SUST_CHR)) p_ptr->sustain_chr = TRUE;

			if (have_flag(flgs, TR_EASY_SPELL))  p_ptr->easy_spell = TRUE;
			if (have_flag(flgs, TR_DEC_MANA))  p_ptr->dec_mana = TRUE;
			if (have_flag(flgs, TR_REFLECT))  p_ptr->reflect = TRUE;

			if (have_flag(flgs, TR_IM_FIRE)) p_ptr->immune_fire = TRUE;
			if (have_flag(flgs, TR_IM_ACID)) p_ptr->immune_acid = TRUE;
			if (have_flag(flgs, TR_IM_COLD)) p_ptr->immune_cold = TRUE;
			if (have_flag(flgs, TR_IM_ELEC)) p_ptr->immune_elec = TRUE;

			if (have_flag(flgs, TR_RES_ACID))   p_ptr->resist_acid = TRUE;
			if (have_flag(flgs, TR_RES_ELEC))   p_ptr->resist_elec = TRUE;
			if (have_flag(flgs, TR_RES_FIRE))   p_ptr->resist_fire = TRUE;
			if (have_flag(flgs, TR_RES_COLD))   p_ptr->resist_cold = TRUE;
			if (have_flag(flgs, TR_RES_POIS))   p_ptr->resist_pois = TRUE;

			if (have_flag(flgs, TR_RES_LITE))   p_ptr->resist_lite = TRUE;
			if (have_flag(flgs, TR_RES_DARK))   p_ptr->resist_dark = TRUE;
			if (have_flag(flgs, TR_RES_SHARDS)) p_ptr->resist_shard = TRUE;
			if (have_flag(flgs, TR_RES_SOUND))  p_ptr->resist_sound = TRUE;
			if (have_flag(flgs, TR_RES_NETHER)) p_ptr->resist_neth = TRUE;
			if (have_flag(flgs, TR_RES_WATER))  p_ptr->resist_water = TRUE;
			if (have_flag(flgs, TR_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
			if (have_flag(flgs, TR_RES_DISEN))  p_ptr->resist_disen = TRUE;
			if (have_flag(flgs, TR_RES_HOLY))  p_ptr->resist_holy = TRUE;
			if (have_flag(flgs, TR_RES_TIME))  p_ptr->resist_time = TRUE;

			if (have_flag(flgs, TR_FREE_ACT))    p_ptr->free_act = TRUE;
			if (have_flag(flgs, TR_RES_BLIND))  p_ptr->resist_blind = TRUE;
			if (have_flag(flgs, TR_RES_FEAR))   p_ptr->resist_fear = TRUE;
			if (have_flag(flgs, TR_RES_CONF))   p_ptr->resist_conf = TRUE;
			if (have_flag(flgs, TR_RES_INSANITY))  p_ptr->resist_insanity = TRUE;

			if (have_flag(flgs, TR_LEVITATION))   p_ptr->levitation = TRUE;
			if (have_flag(flgs, TR_SEE_INVIS))   p_ptr->see_inv = TRUE;
			if (have_flag(flgs, TR_SLOW_DIGEST))   p_ptr->slow_digest = TRUE;
			if (have_flag(flgs, TR_REGEN))   p_ptr->regenerate = TRUE;
			if (have_flag(flgs, TR_LITE))   p_ptr->lite = TRUE;
			if (have_flag(flgs, TR_SPEEDSTER))   p_ptr->speedster = TRUE;
			if (have_flag(flgs, TR_TELEPATHY))   p_ptr->telepathy = TRUE;


			if (have_flag(flgs, TR_SH_FIRE))   p_ptr->sh_fire = TRUE;
			if (have_flag(flgs, TR_SH_ELEC))   p_ptr->sh_elec = TRUE;
			if (have_flag(flgs, TR_SH_COLD))   p_ptr->sh_cold = TRUE;

		}
		else if(o_ptr->tval == TV_SOUVENIR)
		{
			switch (o_ptr->sval)
			{
				//道寿の壺:火炎ブレス
				//茂林寺の茶釜:蒸気のブレス

			case SV_SOUVENIR_SHINMYOU_HOUSE: //針妙丸の家　元素耐性
				p_ptr->resist_acid = TRUE;
				p_ptr->resist_elec = TRUE;
				p_ptr->resist_fire = TRUE;
				p_ptr->resist_cold = TRUE;
				break;

			case SV_SOUVENIR_VAMPIRE_COFFIN: //吸血鬼の棺桶　暗黒地獄耐性
				p_ptr->resist_dark = TRUE;
				p_ptr->resist_neth = TRUE;
				break;

				//マイクロンフト・ボブ　モンスター複数召喚

				//ラフノールの鏡　反射、AC+100
			case SV_SOUVENIR_MIRROR_OF_RUFFNOR:
				p_ptr->reflect = TRUE;
				p_ptr->to_a += 100;
				p_ptr->dis_to_a += 100;
				break;

				//鳥の巣風エクステ 毒耐性
			case SV_SOUVENIR_KUTAKA:
				p_ptr->resist_pois = TRUE;
				break;
				//運命の切札　次元の扉

				//スキーズブラズニル　肉体能力上昇・水破邪耐性
			case SV_SOUVENIR_SKIDBLADNIR:
				p_ptr->stat_add[A_STR] += 4;
				p_ptr->stat_add[A_WIS] += 4;
				p_ptr->stat_add[A_CON] += 4;
				p_ptr->resist_holy = TRUE;
				p_ptr->resist_water = TRUE;
				break;

				//古書型コンピュータ　知的能力上昇
			case SV_SOUVENIR_MOON_COMPUTER:
				p_ptr->stat_add[A_INT] += 4;
				p_ptr->stat_add[A_WIS] += 4;
				p_ptr->sustain_int = TRUE;
				p_ptr->sustain_wis = TRUE;
				p_ptr->resist_conf = TRUE;
				p_ptr->easy_spell = TRUE;

				break;

				//貝殻の戦車　加速、壁抜け
			case SV_SOUVENIR_NODENS_CHARIOT:
				new_speed += 5;
				p_ptr->pass_wall = TRUE;
				break;

				//古のものの水晶　冷気オーラ,MP+100
			case SV_SOUVENIR_ELDER_THINGS_CRYSTAL:
				p_ptr->sh_cold = TRUE;
				break;

			default:
				break;





			}







		}
	}



	//v1.1.46
	//追加インベントリinven_add[]に格納された装備品のパラメータと耐性を計算する。今の所女苑の指輪専用
	//パラメータ類は基本的に半減
	//他にもステータス画面など色々な箇所で修正が必要になり実に面倒だが正直今更リファクタリングする気にならない
	if (p_ptr->pclass == CLASS_JYOON)
	{
		int extra_shots_sum = 0;

		for (i = 0; i<INVEN_ADD_MAX; i++)
		{
			int bonus_to_h, bonus_to_d, bonus_to_blows;
			int pv;

			//特殊な変身時など指輪スロットが無効化されているときは効果なし
			if (check_invalidate_inventory(INVEN_RIGHT) || check_invalidate_inventory(INVEN_LEFT)) break;

			o_ptr = &inven_add[i];
			if (!o_ptr->k_idx) continue;
			if (o_ptr->tval != TV_RING) continue; //paranoia
			object_flags(o_ptr, flgs);
			pv = o_ptr->pval;

			//AC処理
			p_ptr->ac += o_ptr->ac / 2;
			p_ptr->dis_ac += o_ptr->ac / 2;
			p_ptr->to_a += o_ptr->to_a / 2;
			if (object_is_known(o_ptr)) p_ptr->dis_to_a += o_ptr->to_a / 2;

			//耐性や呪いなどフラグ処理
			if (have_flag(flgs, TR_AGGRAVATE))   p_ptr->cursed |= TRC_AGGRAVATE;
			//他の呪いは無視する。呪い発動処理とか実装困難なため

			if (have_flag(flgs, TR_DEC_MANA))    p_ptr->dec_mana = TRUE;
			if (have_flag(flgs, TR_SLOW_DIGEST)) p_ptr->slow_digest = TRUE;
			if (have_flag(flgs, TR_REGEN))       p_ptr->regenerate = TRUE;
			if (have_flag(flgs, TR_TELEPATHY))   p_ptr->telepathy = TRUE;
			if (have_flag(flgs, TR_ESP_ANIMAL))  p_ptr->esp_animal = TRUE;
			if (have_flag(flgs, TR_ESP_UNDEAD))  p_ptr->esp_undead = TRUE;
			if (have_flag(flgs, TR_ESP_DEMON))   p_ptr->esp_demon = TRUE;
			if (have_flag(flgs, TR_ESP_KWAI))     p_ptr->esp_kwai = TRUE;
			if (have_flag(flgs, TR_ESP_DEITY))   p_ptr->esp_deity = TRUE;
			if (have_flag(flgs, TR_ESP_DRAGON))  p_ptr->esp_dragon = TRUE;
			if (have_flag(flgs, TR_ESP_HUMAN))   p_ptr->esp_human = TRUE;
			if (have_flag(flgs, TR_ESP_EVIL))    p_ptr->esp_evil = TRUE;
			if (have_flag(flgs, TR_ESP_GOOD))    p_ptr->esp_good = TRUE;
			if (have_flag(flgs, TR_ESP_NONLIVING)) p_ptr->esp_nonliving = TRUE;
			if (have_flag(flgs, TR_ESP_UNIQUE))  p_ptr->esp_unique = TRUE;

			if (have_flag(flgs, TR_SEE_INVIS))   p_ptr->see_inv = TRUE;
			if (have_flag(flgs, TR_LEVITATION))     p_ptr->levitation = TRUE;
			if (have_flag(flgs, TR_FREE_ACT))    p_ptr->free_act = TRUE;
			/*:::警告付きのアイテムに$と銘を彫ると警告しなくなる*/
			if (have_flag(flgs, TR_WARNING))
			{
				if (!o_ptr->inscription || !(my_strchr(quark_str(o_ptr->inscription), '$')))	  p_ptr->warning = TRUE;
			}
			if (have_flag(flgs, TR_IM_FIRE)) p_ptr->immune_fire = TRUE;
			if (have_flag(flgs, TR_IM_ACID)) p_ptr->immune_acid = TRUE;
			if (have_flag(flgs, TR_IM_COLD)) p_ptr->immune_cold = TRUE;
			if (have_flag(flgs, TR_IM_ELEC)) p_ptr->immune_elec = TRUE;

			if (have_flag(flgs, TR_RES_ACID))   p_ptr->resist_acid = TRUE;
			if (have_flag(flgs, TR_RES_ELEC))   p_ptr->resist_elec = TRUE;
			if (have_flag(flgs, TR_RES_FIRE))   p_ptr->resist_fire = TRUE;
			if (have_flag(flgs, TR_RES_COLD))   p_ptr->resist_cold = TRUE;
			if (have_flag(flgs, TR_RES_POIS))   p_ptr->resist_pois = TRUE;
			if (have_flag(flgs, TR_RES_FEAR))   p_ptr->resist_fear = TRUE;
			if (have_flag(flgs, TR_RES_CONF))   p_ptr->resist_conf = TRUE;
			if (have_flag(flgs, TR_RES_SOUND))  p_ptr->resist_sound = TRUE;
			if (have_flag(flgs, TR_RES_LITE))   p_ptr->resist_lite = TRUE;
			if (have_flag(flgs, TR_RES_DARK))   p_ptr->resist_dark = TRUE;
			if (have_flag(flgs, TR_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
			if (have_flag(flgs, TR_RES_DISEN))  p_ptr->resist_disen = TRUE;
			if (have_flag(flgs, TR_RES_SHARDS)) p_ptr->resist_shard = TRUE;

			if (have_flag(flgs, TR_RES_BLIND))  p_ptr->resist_blind = TRUE;
			if (have_flag(flgs, TR_RES_NETHER)) p_ptr->resist_neth = TRUE;

			if (have_flag(flgs, TR_REFLECT))  p_ptr->reflect = TRUE;
			if (have_flag(flgs, TR_SH_FIRE))  p_ptr->sh_fire = TRUE;
			if (have_flag(flgs, TR_SH_ELEC))  p_ptr->sh_elec = TRUE;
			if (have_flag(flgs, TR_SH_COLD))  p_ptr->sh_cold = TRUE;
			if (have_flag(flgs, TR_NO_MAGIC)) p_ptr->anti_magic = TRUE;
			if (have_flag(flgs, TR_NO_TELE))  p_ptr->anti_tele = TRUE;

			if (have_flag(flgs, TR_SUST_STR)) p_ptr->sustain_str = TRUE;
			if (have_flag(flgs, TR_SUST_INT)) p_ptr->sustain_int = TRUE;
			if (have_flag(flgs, TR_SUST_WIS)) p_ptr->sustain_wis = TRUE;
			if (have_flag(flgs, TR_SUST_DEX)) p_ptr->sustain_dex = TRUE;
			if (have_flag(flgs, TR_SUST_CON)) p_ptr->sustain_con = TRUE;
			if (have_flag(flgs, TR_SUST_CHR)) p_ptr->sustain_chr = TRUE;

			if (have_flag(flgs, TR_SPEEDSTER)) p_ptr->speedster = TRUE;
			if (have_flag(flgs, TR_RES_WATER))  p_ptr->resist_water = TRUE;
			if (have_flag(flgs, TR_RES_HOLY))  p_ptr->resist_holy = TRUE;
			if (have_flag(flgs, TR_RES_TIME))  p_ptr->resist_time = TRUE;
			if (have_flag(flgs, TR_RES_INSANITY))  p_ptr->resist_insanity = TRUE;
			if (have_flag(flgs, TR_EASY_SPELL)) p_ptr->easy_spell = TRUE;


			//殺戮修正と追加攻撃はスロットの場所と武器の装備状況によりどこにパラメータを割り振るか分岐
			//ここもp_ptr->ryouteだのdo_martialartsだのdefault_handだのややこしいので作り直したかったが正直今更である
			bonus_to_h = o_ptr->to_h / 2;
			bonus_to_d = o_ptr->to_d / 2;
			if (have_flag(flgs, TR_BLOWS)) bonus_to_blows = pv / 2;
			else bonus_to_blows = 0;
			if (p_ptr->ryoute || i < 4)
			{
				extra_blows[0] += bonus_to_blows;
				p_ptr->to_h[0] += bonus_to_h;
				p_ptr->to_d[0] += bonus_to_d;

				if (object_is_known(o_ptr))
				{
					p_ptr->dis_to_h[0] += bonus_to_h;
					p_ptr->dis_to_d[0] += bonus_to_d;
				}
			}
			if (p_ptr->ryoute || i >= 4)
			{
				extra_blows[1] += bonus_to_blows;
				p_ptr->to_h[1] += bonus_to_h;
				p_ptr->to_d[1] += bonus_to_d;

				if (object_is_known(o_ptr))
				{
					p_ptr->dis_to_h[1] += bonus_to_h;
					p_ptr->dis_to_d[1] += bonus_to_d;
				}
			}

			//追加射撃は個数を数えておきあとで合計数から計算する
			if (have_flag(flgs, TR_XTRA_SHOTS)) extra_shots_sum++;
			//強力投擲は面倒なので一個でも効果発揮することにする。
			if (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_MIGHTY_THROW) p_ptr->mighty_throw = TRUE;

			//pval処理 効果半減(端数切捨て)
			if (have_flag(flgs, TR_MAGIC_MASTERY))    p_ptr->skill_dev += 4 * pv;
			if (have_flag(flgs, TR_STR)) p_ptr->stat_add[A_STR] += pv / 2;
			if (have_flag(flgs, TR_INT)) p_ptr->stat_add[A_INT] += pv / 2;
			if (have_flag(flgs, TR_WIS)) p_ptr->stat_add[A_WIS] += pv / 2;
			if (have_flag(flgs, TR_DEX)) p_ptr->stat_add[A_DEX] += pv / 2;
			if (have_flag(flgs, TR_CON)) p_ptr->stat_add[A_CON] += pv / 2;
			if (have_flag(flgs, TR_CHR)) p_ptr->stat_add[A_CHR] += pv / 2;
			if (have_flag(flgs, TR_STEALTH)) p_ptr->skill_stl += pv / 2;
			if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_srh += (pv * 5 / 2);

			if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_fos += (pv * 5 / 2);
			if (have_flag(flgs, TR_INFRA)) p_ptr->see_infra += pv / 2;
			if (have_flag(flgs, TR_TUNNEL)) p_ptr->skill_dig += (pv * 10);
			if (have_flag(flgs, TR_SPEED)) new_speed += pv / 2;
		}

		//追加射撃の個数2つごとに本来の指輪一つとして扱う
		extra_shots += extra_shots_sum / 2;


	}


	/*:::装備品による耐性や能力付加　*/
	/* Scan the usable inventory */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		int bonus_to_h, bonus_to_d,pv;
		o_ptr = &inventory[i];
		pv = o_ptr->pval;

		///mod150411 おくう特殊処理
		if(p_ptr->pclass == CLASS_UTSUHO && (i == INVEN_HANDS || i == INVEN_FEET)) pv = (pv + 1) / 2;

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/*:::変身で無効化されている装備部位*/
		if(check_invalidate_inventory(i)) continue;

		/* Extract the item flags */
		object_flags(o_ptr, flgs);

		/*:::装備品の呪いフラグを＠のステータスにコピー　下位4bitは「呪い・強力な呪い・永遠の呪い」のフラグなのでコピーしない*/
		p_ptr->cursed |= (o_ptr->curse_flags & (0xFFFFFFF0L));
		///item131221 チェンソーが勝手に喋る用フラグをストブリに差し替えてみた
		if (o_ptr->name1 == ART_STORMBRINGER) p_ptr->cursed |= TRC_STORMBRINGER;

		/* Affect stats */
		if (have_flag(flgs, TR_STR)) p_ptr->stat_add[A_STR] += pv;
		if (have_flag(flgs, TR_INT)) p_ptr->stat_add[A_INT] += pv;
		if (have_flag(flgs, TR_WIS)) p_ptr->stat_add[A_WIS] += pv;
		if (have_flag(flgs, TR_DEX)) p_ptr->stat_add[A_DEX] += pv;
		if (have_flag(flgs, TR_CON)) p_ptr->stat_add[A_CON] += pv;
		if (have_flag(flgs, TR_CHR)) p_ptr->stat_add[A_CHR] += pv;

		//特殊処理　ベルシエル冠の知能賢さ探索上昇
		if(o_ptr->name1 == ART_BERUTHIEL)
		{
			p_ptr->stat_add[A_INT] +=10;
			p_ptr->stat_add[A_WIS] +=10;
			p_ptr->skill_srh +=10;
		}


///pend グレイズ実装予定地
		if (have_flag(flgs, TR_MAGIC_MASTERY))    p_ptr->skill_dev += 8 * pv;

		/* Affect stealth */
		if (have_flag(flgs, TR_STEALTH)) p_ptr->skill_stl += pv;

		/* Affect searching ability (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_srh += (pv * 5);

		//v1.1.32 作業服
		if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_WORKER)
		{
			 p_ptr->skill_dis += MAX(pv,1) * 10;
		}

		/* Affect searching frequency (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) p_ptr->skill_fos += (pv * 5);

		/* Affect infravision */
		if (have_flag(flgs, TR_INFRA)) p_ptr->see_infra += pv;

		/* Affect digging (factor of 20) */
		if (have_flag(flgs, TR_TUNNEL)) p_ptr->skill_dig += (pv * 20);

		/* Affect speed */
		if (have_flag(flgs, TR_SPEED)) new_speed += pv;

		/* Affect blows */
		/*:::追加攻撃　アリスなど多腕系に一工夫要*/
		if (have_flag(flgs, TR_BLOWS))
		{
			///mod151205 三月精特殊処理　武器を片方でも持ってれば常に両手スロットに割り振られる
			if(p_ptr->pclass == CLASS_3_FAIRIES && !p_ptr->do_martialarts)
			{
				if(i == INVEN_RARM) extra_blows[0] += pv;
				else if(i == INVEN_LARM) extra_blows[1] += pv;
				else
				{
					extra_blows[0] += (pv > 0)?(pv+1)/2:(pv-1)/2;
					extra_blows[1] += (pv)/2;
				}
			}
			else
			{
				/*:::右手武器か右指輪の処理中で両手持ちでないならblows[0]に加算*/
				if((i == INVEN_RARM || i == INVEN_RIGHT) && !p_ptr->ryoute) extra_blows[0] += pv;
				/*:::左手武器か左指輪の処理中で両手持ちでないならblows[1]に加算*/
				else if((i == INVEN_LARM || i == INVEN_LEFT) && !p_ptr->ryoute) extra_blows[1] += pv;
				/*:::それ以外ならblow[0]と[1]両方に加算（両手持ちかシヴァジャケのときここに来る)*/
				else {extra_blows[0] += pv; extra_blows[1] += pv;}
			}
		}

		/* Hack -- cause earthquakes */
		if (have_flag(flgs, TR_IMPACT)) p_ptr->impact[(i == INVEN_RARM) ? 0 : 1] = TRUE;

		/* Boost shots */
		if (have_flag(flgs, TR_XTRA_SHOTS)) extra_shots++;

		///item res アイテムフラグの＠パラメータ反映
		/* Various flags */
		if (have_flag(flgs, TR_AGGRAVATE))   p_ptr->cursed |= TRC_AGGRAVATE;
		if (have_flag(flgs, TR_DRAIN_EXP))   p_ptr->cursed |= TRC_DRAIN_EXP;
		if (have_flag(flgs, TR_TY_CURSE))    p_ptr->cursed |= TRC_TY_CURSE;
		if (have_flag(flgs, TR_ADD_L_CURSE)) p_ptr->cursed |= TRC_ADD_L_CURSE;
		if (have_flag(flgs, TR_ADD_H_CURSE)) p_ptr->cursed |= TRC_ADD_H_CURSE;
		if (have_flag(flgs, TR_DEC_MANA))    p_ptr->dec_mana = TRUE;
		if (have_flag(flgs, TR_BLESSED))     p_ptr->bless_blade = TRUE;
		if (have_flag(flgs, TR_XTRA_MIGHT))  p_ptr->xtra_might = TRUE;/*:::強力射*/
		if (have_flag(flgs, TR_SLOW_DIGEST)) p_ptr->slow_digest = TRUE;/*:::遅消化*/
		if (have_flag(flgs, TR_REGEN))       p_ptr->regenerate = TRUE;///sys ここで累算するようにすれば急回復重複とか実装可能か
		if (have_flag(flgs, TR_TELEPATHY))   p_ptr->telepathy = TRUE;
		if (have_flag(flgs, TR_ESP_ANIMAL))  p_ptr->esp_animal = TRUE;
		if (have_flag(flgs, TR_ESP_UNDEAD))  p_ptr->esp_undead = TRUE;
		if (have_flag(flgs, TR_ESP_DEMON))   p_ptr->esp_demon = TRUE;
		if (have_flag(flgs, TR_ESP_KWAI))     p_ptr->esp_kwai = TRUE;
		if (have_flag(flgs, TR_ESP_DEITY))   p_ptr->esp_deity = TRUE;
		if (have_flag(flgs, TR_ESP_DRAGON))  p_ptr->esp_dragon = TRUE;
		if (have_flag(flgs, TR_ESP_HUMAN))   p_ptr->esp_human = TRUE;
		if (have_flag(flgs, TR_ESP_EVIL))    p_ptr->esp_evil = TRUE;
		if (have_flag(flgs, TR_ESP_GOOD))    p_ptr->esp_good = TRUE;
		if (have_flag(flgs, TR_ESP_NONLIVING)) p_ptr->esp_nonliving = TRUE;
		if (have_flag(flgs, TR_ESP_UNIQUE))  p_ptr->esp_unique = TRUE;

		if (have_flag(flgs, TR_SEE_INVIS))   p_ptr->see_inv = TRUE;
		if (have_flag(flgs, TR_LEVITATION))     p_ptr->levitation = TRUE;
		if (have_flag(flgs, TR_FREE_ACT))    p_ptr->free_act = TRUE;
	//	if (have_flag(flgs, TR_HOLD_LIFE))   p_ptr->hold_life = TRUE;
		/*:::警告付きのアイテムに$と銘を彫ると警告しなくなる*/
		if (have_flag(flgs, TR_WARNING)){
			if (!o_ptr->inscription || !(my_strchr(quark_str(o_ptr->inscription),'$')))
			  p_ptr->warning = TRUE;
		}

		/*:::ランダムテレポート　該当装備品が呪われていると強制発動ランダムになり、銘に.を入れておくと何も起こらない*/
		if (have_flag(flgs, TR_TELEPORT))
		{
			if (object_is_cursed(o_ptr)) p_ptr->cursed |= TRC_TELEPORT;
			else
			{
				cptr insc = quark_str(o_ptr->inscription);

				if (o_ptr->inscription && my_strchr(insc, '.'))
				{
					/*
					 * {.} will stop random teleportation.
					 */
				}
				else
				{
					/* Controlled random teleportation */
					p_ptr->cursed |= TRC_TELEPORT_SELF;
				}
			}
		}

		/* Immunity flags */
		if (have_flag(flgs, TR_IM_FIRE)) p_ptr->immune_fire = TRUE;
		if (have_flag(flgs, TR_IM_ACID)) p_ptr->immune_acid = TRUE;
		if (have_flag(flgs, TR_IM_COLD)) p_ptr->immune_cold = TRUE;
		if (have_flag(flgs, TR_IM_ELEC)) p_ptr->immune_elec = TRUE;

		/* Resistance flags */
		if (have_flag(flgs, TR_RES_ACID))   p_ptr->resist_acid = TRUE;
		if (have_flag(flgs, TR_RES_ELEC))   p_ptr->resist_elec = TRUE;
		if (have_flag(flgs, TR_RES_FIRE))   p_ptr->resist_fire = TRUE;
		if (have_flag(flgs, TR_RES_COLD))   p_ptr->resist_cold = TRUE;
		if (have_flag(flgs, TR_RES_POIS))   p_ptr->resist_pois = TRUE;
		if (have_flag(flgs, TR_RES_FEAR))   p_ptr->resist_fear = TRUE;
		if (have_flag(flgs, TR_RES_CONF))   p_ptr->resist_conf = TRUE;
		if (have_flag(flgs, TR_RES_SOUND))  p_ptr->resist_sound = TRUE;
		if (have_flag(flgs, TR_RES_LITE))   p_ptr->resist_lite = TRUE;
		if (have_flag(flgs, TR_RES_DARK))   p_ptr->resist_dark = TRUE;
		if (have_flag(flgs, TR_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
		if (have_flag(flgs, TR_RES_DISEN))  p_ptr->resist_disen = TRUE;
		if (have_flag(flgs, TR_RES_SHARDS)) p_ptr->resist_shard = TRUE;

		//if (have_flag(flgs, TR_RES_NEXUS))  p_ptr->resist_nexus = TRUE;
		if (have_flag(flgs, TR_RES_BLIND))  p_ptr->resist_blind = TRUE;
		if (have_flag(flgs, TR_RES_NETHER)) p_ptr->resist_neth = TRUE;

		if (have_flag(flgs, TR_REFLECT))  p_ptr->reflect = TRUE;
		if (have_flag(flgs, TR_SH_FIRE))  p_ptr->sh_fire = TRUE;
		if (have_flag(flgs, TR_SH_ELEC))  p_ptr->sh_elec = TRUE;
		if (have_flag(flgs, TR_SH_COLD))  p_ptr->sh_cold = TRUE;
		if (have_flag(flgs, TR_NO_MAGIC)) p_ptr->anti_magic = TRUE;
		if (have_flag(flgs, TR_NO_TELE))  p_ptr->anti_tele = TRUE;

		/* Sustain flags */
		if (have_flag(flgs, TR_SUST_STR)) p_ptr->sustain_str = TRUE;
		if (have_flag(flgs, TR_SUST_INT)) p_ptr->sustain_int = TRUE;
		if (have_flag(flgs, TR_SUST_WIS)) p_ptr->sustain_wis = TRUE;
		if (have_flag(flgs, TR_SUST_DEX)) p_ptr->sustain_dex = TRUE;
		if (have_flag(flgs, TR_SUST_CON)) p_ptr->sustain_con = TRUE;
		if (have_flag(flgs, TR_SUST_CHR)) p_ptr->sustain_chr = TRUE;

		/*:::新アイテムフラグ*/
		if (have_flag(flgs, TR_SPEEDSTER)) p_ptr->speedster = TRUE;
		if (have_flag(flgs, TR_RES_WATER))  p_ptr->resist_water = TRUE;
		if (have_flag(flgs, TR_RES_HOLY))  p_ptr->resist_holy = TRUE;
		if (have_flag(flgs, TR_RES_TIME))  p_ptr->resist_time = TRUE;
		if (have_flag(flgs, TR_RES_INSANITY))  p_ptr->resist_insanity = TRUE;

		///mod131228 源氏はフラグ化
		if (have_flag(flgs, TR_GENZI))	easy_2weapon = TRUE;

		if( check_activated_nameless_arts(JKF1_MASTER_MELEE)) easy_2weapon = TRUE;


///mod131224 ego
		///ego 宵闇、源氏、耐時間逆転、強力投擲、魔導士エゴなど
		//if (o_ptr->name2 == EGO_YOIYAMI) yoiyami = TRUE;
		//if (o_ptr->name2 == EGO_HANDS_MUSOU) easy_2weapon = TRUE;
		//if (o_ptr->name2 == EGO_RING_RES_TIME) p_ptr->resist_time = TRUE;
		if (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_MIGHTY_THROW) p_ptr->mighty_throw = TRUE;
		if (have_flag(flgs, TR_EASY_SPELL)) p_ptr->easy_spell = TRUE;
		//if (o_ptr->name2 == EGO_AMU_FOOL) p_ptr->heavy_spell = TRUE;
		//if (o_ptr->name2 == EGO_AMU_NAIVETY) down_saving = TRUE;

		if (o_ptr->curse_flags & TRC_LOW_MAGIC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				p_ptr->to_m_chance += 10;
			}
			else
			{
				p_ptr->to_m_chance += 3;
			}
		}

		/*:::モンスターボールの場合処理はここまで　というかもっと早く終わってても良さそうなもんだが*/
		if (o_ptr->tval == TV_CAPTURE) continue;

/*:::AC処理*/
		///mod150411 おくう特殊処理
		if(p_ptr->pclass == CLASS_UTSUHO && (i == INVEN_HANDS || i == INVEN_FEET))
		{
			p_ptr->ac += (o_ptr->ac + 1) / 2;
			p_ptr->dis_ac += (o_ptr->ac + 1) / 2;
			p_ptr->to_a += (o_ptr->to_a + 1) / 2;
			if (object_is_known(o_ptr)) p_ptr->dis_to_a += (o_ptr->to_a + 1) / 2;
		}
		else
		{
			/* Modify the base armor class */
			p_ptr->ac += o_ptr->ac;
			/* The base armor class is always known */
			p_ptr->dis_ac += o_ptr->ac;
			/* Apply the bonuses to armor class */
			p_ptr->to_a += o_ptr->to_a;
			/* Apply the mental bonuses to armor class, if known */
			if (object_is_known(o_ptr)) p_ptr->dis_to_a += o_ptr->to_a;

		}


/*:::攻撃が外れやすい呪われた武器の命中率処理*/
		if (o_ptr->curse_flags & TRC_LOW_MELEE)
		{
			int slot = i - INVEN_RARM;
			if (slot < 2)
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					p_ptr->to_h[slot] -= 15;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h[slot] -= 15;
				}
				else
				{
					p_ptr->to_h[slot] -= 5;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h[slot] -= 5;
				}
			}
			else
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					p_ptr->to_h_b -= 15;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h_b -= 15;
				}
				else
				{
					p_ptr->to_h_b -= 5;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h_b -= 5;
				}
			}
		}
/*:::攻撃を受けやすくする呪いのAC処理*/
		if (o_ptr->curse_flags & TRC_LOW_AC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				p_ptr->to_a -= 30;
				if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_a -= 30;
			}
			else
			{
				p_ptr->to_a -= 10;
				if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_a -= 10;
			}
		}

		/* Hack -- do not apply "weapon" bonuses */
		/*:::近接武器の場合ここで終了*/
		if (i == INVEN_RARM && buki_motteruka(i)) continue;
		if (i == INVEN_LARM && buki_motteruka(i)) continue;

		/* Hack -- do not apply "bow" bonuses */
		/*:::弓の修正値はここでは扱わない　（弓の修正値が格闘の殺戮修正に加算されないようにする）*/
		//if (i == INVEN_BOW) continue;
		if(object_is_shooting_weapon(o_ptr)) continue;

		bonus_to_h = o_ptr->to_h;
		bonus_to_d = o_ptr->to_d;

		/*:::忍者の殺戮修正は半分*/
		///mod150411 おくう特殊処理
		if (p_ptr->pclass == CLASS_NINJA || (p_ptr->pclass == CLASS_UTSUHO && (i == INVEN_HANDS || i == INVEN_FEET)))
		{
			if (o_ptr->to_h > 0) bonus_to_h = (o_ptr->to_h+1)/2;
			if (o_ptr->to_d > 0) bonus_to_d = (o_ptr->to_d+1)/2;
		}
		///mod150808 アリス人形使用時の殺戮修正は1/5
		else if(alice_doll_attack)
		{
			bonus_to_h = (o_ptr->to_h+4)/5;
			bonus_to_d = (o_ptr->to_d+4)/5;
		}

		/* To Bow and Natural attack */

		/* Apply the bonuses to hit/damage */
		p_ptr->to_h_b += bonus_to_h;
		p_ptr->to_h_m += bonus_to_h;
		p_ptr->to_d_m += bonus_to_d;

		/* Apply the mental bonuses tp hit/damage, if known */
		if (object_is_known(o_ptr)) p_ptr->dis_to_h_b += bonus_to_h;

		/* To Melee */
		/*:::片手武器か二刀流時の指輪の殺戮修正*/
		if ((i == INVEN_LEFT || i == INVEN_RIGHT) && !p_ptr->ryoute && !alice_doll_attack && p_ptr->pclass != CLASS_3_FAIRIES)
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->to_h[i-INVEN_RIGHT] += bonus_to_h;
			p_ptr->to_d[i-INVEN_RIGHT] += bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->dis_to_h[i-INVEN_RIGHT] += bonus_to_h;
				p_ptr->dis_to_d[i-INVEN_RIGHT] += bonus_to_d;
			}
		}
		/*:::二刀流時の指輪以外の装備品の殺戮修正*/
		///mod151205 三月精武器所持時は常にここで処理する
		else if (p_ptr->pclass == CLASS_3_FAIRIES && !p_ptr->do_martialarts || p_ptr->migite && p_ptr->hidarite)
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
			p_ptr->to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
			p_ptr->to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
			p_ptr->to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->dis_to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
				p_ptr->dis_to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
				p_ptr->dis_to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
				p_ptr->dis_to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;
			}
		}
		/*:::それ以外の殺戮修正　二刀流でないときの指輪以外、両手持ちや格闘のときの指輪*/
		else
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->to_h[default_hand] += bonus_to_h;
			p_ptr->to_d[default_hand] += bonus_to_d;

			/* Apply the mental bonuses to hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->dis_to_h[default_hand] += bonus_to_h;
				p_ptr->dis_to_d[default_hand] += bonus_to_d;
			}
		}

		//if (p_ptr->wizard) msg_format("def:%d i:%d d:%d to_d:%d",default_hand, i, bonus_to_d, p_ptr->dis_to_d[default_hand]);

	}/*:::装備インベントリループ終了*/

	if (old_mighty_throw != p_ptr->mighty_throw)
	{
		/* Redraw average damege display of Shuriken */
		p_ptr->window |= PW_INVEN;
	}

	if (p_ptr->cursed & TRC_TELEPORT) p_ptr->cursed &= ~(TRC_TELEPORT_SELF);



	///mod140412 吸血鬼月齢パラメータ処理
	if((prace_is_(RACE_VAMPIRE) || p_ptr->mimic_form == MIMIC_VAMPIRE)	)
	{
		int dummy;
		if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
		{
			if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30) dummy = 3;
			else dummy = 0;
		}
		else
		{
			if(p_ptr->resist_lite) dummy = -1;
			else dummy = -2;
		}
		for (i = 0; i < 6; i++) p_ptr->stat_add[i] += dummy;
		new_speed += dummy;
	}
	///mod140402 獣人満月パラメータ処理
	///mod150121 慧音は除く
	//v1.1.33 あうんも除く
	if(prace_is_(RACE_WARBEAST)	&& p_ptr->pclass != CLASS_KEINE	&& p_ptr->pclass != CLASS_AUNN)
	{
		if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
		{
			int dummy = 2;
			if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30)
			{
				for (i = 0; i < 6; i++) p_ptr->stat_add[i] += dummy;
				new_speed += dummy;
			}
		}
	}
	//慧音白沢化中
	else if(p_ptr->pclass == CLASS_KEINE && !p_ptr->mimic_form && p_ptr->magic_num1[0])
	{
		p_ptr->stat_add[A_STR] += 3;
		p_ptr->stat_add[A_INT] += 5;
		p_ptr->stat_add[A_WIS] += 4;
		p_ptr->stat_add[A_DEX] += 3;
		p_ptr->stat_add[A_CON] += 3;
		p_ptr->stat_add[A_CHR] += 1;
		p_ptr->to_h[0] += 30;
		p_ptr->to_h[1] += 30;
		p_ptr->to_h_m  += 30;
		p_ptr->dis_to_h[0] += 30;
		p_ptr->dis_to_h[1] += 30;
		new_speed += 2;
	}
	//サニー
	else if(p_ptr->pclass == CLASS_SUNNY)
	{
		//v1.1.59 日光ポイントが「元気一杯」以上だと全パラメータ+1
		if (p_ptr->magic_num1[0] >= SUNNY_CHARGE_SUNLIGHT_3)
		{
			for (i = 0; i < 6; i++) p_ptr->stat_add[i] += 1;
			new_speed += 1;
		}
		//日中は全パラメータ+1
		if(is_daytime())
			for (i = 0; i < 6; i++) p_ptr->stat_add[i] += 1;
		//夜間は全パラメータ-1 ただし日光ポイントがそこそこあればペナルティ無効
		else if(p_ptr->magic_num1[0] < SUNNY_CHARGE_SUNLIGHT_2)
			for (i = 0; i < 6; i++) p_ptr->stat_add[i] -= 1;
	}
	//ルナ
	else if(p_ptr->pclass == CLASS_LUNAR)
	{
		if(is_daytime())
			for (i = 0; i < 6; i++) p_ptr->stat_add[i] -= 1;
		else
			for (i = 0; i < 6; i++) p_ptr->stat_add[i] += 1;
	}
	//霊夢(パラメータ)
	else if(p_ptr->pclass == CLASS_REIMU)
	{
		int rank = osaisen_rank();
		for (i = 0; i < 6; i++) p_ptr->stat_add[i] += rank;
		if(rank == 9)
		{
			extra_blows[0] += 2;
			extra_blows[1] += 2;
			new_speed += plev / 5;
		}
		else if(rank > 5)
		{
			extra_blows[0] += 1;
			extra_blows[1] += 1;
			new_speed += plev / 5;
		}
	}
	else if(p_ptr->pclass == CLASS_KANAKO)
	{
		int rank = kanako_rank();
		for (i = 0; i < 6; i++) p_ptr->stat_add[i] += rank;
		if(rank > 5)
		{
			extra_blows[0] += 1;
			extra_blows[1] += 1;
		}
	}
	//ドレミーパラメータアップ
	else if(p_ptr->pclass == CLASS_DOREMY && IN_DREAM_WORLD)
	{
		for (i = 0; i < 6; i++) p_ptr->stat_add[i] += 2;

	}
	//v1.1.17 純狐白兵能力アップ
	else if( check_activated_nameless_arts(JKF1_MASTER_MELEE))
	{
		p_ptr->to_a += 30 + (p_ptr->lev * 2 / 5);
		p_ptr->dis_to_a += 30 + (p_ptr->lev * 2 / 5);
		p_ptr->to_h[0] += plev / 2;
		p_ptr->to_h[1] += plev / 2;
		p_ptr->to_h_m  += plev / 2;
		p_ptr->to_d[0] += plev / 3;
		p_ptr->to_d[1] += plev / 3;
		p_ptr->to_d_m  += plev / 3;
		p_ptr->dis_to_h[0] += plev / 2;
		p_ptr->dis_to_h[1] += plev / 2;
		p_ptr->dis_to_d[0] += plev / 3;
		p_ptr->dis_to_d[1] += plev / 3;
	}

	//v1.1.21 兵士パラメータアップ　獣人満月と累積するようにelseはつけない
	if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_MUSCLE))
	{
		p_ptr->stat_add[A_STR] += 2;
		p_ptr->stat_add[A_DEX] += 2;
		p_ptr->stat_add[A_CON] += 2;
	}
	if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_MAGITEM))
	{
		p_ptr->stat_add[A_INT] += 2;
		p_ptr->stat_add[A_WIS] += 2;
		p_ptr->skill_dev += 20;

	}


	//v1.1.86
	if (TRUE)
	{
		int card_num;
		// 「弱肉強食の理」「暴食のムカデ」カードによる隣接攻撃ボーナス
		card_num = count_ability_card(ABL_CARD_SAKI);
		if (card_num)
		{
			int bonus = 10;
			if (card_num > 1) bonus += (card_num - 1) * 2;
			if (card_num >= 9) bonus = 32;

			p_ptr->to_h[0] += bonus;
			p_ptr->dis_to_h[0] += bonus;
			p_ptr->to_h[1] += bonus;
			p_ptr->dis_to_h[1] += bonus;
			p_ptr->to_d[0] += bonus;
			p_ptr->dis_to_d[0] += bonus;
			p_ptr->to_d[1] += bonus;
			p_ptr->dis_to_d[1] += bonus;
		}
		card_num = count_ability_card(ABL_CARD_MUKADE);
		if (card_num)
		{
			int border_lis[10] = { 100,95,92,89,86,86,83,80,77,75 };
			int add_lis[10] = { 1,1,1,1,1,2,2,2,2,3 };

            if (card_num > 9) card_num = 9;//paranoia

			//HPがMAXHPのborder%以上のとき攻撃回数がadd増加
			if (p_ptr->mhp * border_lis[card_num] / 100 <= p_ptr->chp)
			{
				extra_blows[0] += add_lis[card_num];
				extra_blows[1] += add_lis[card_num];

			}


		}

		//v1.1.86 埴輪カードによる魔法防御上昇
		card_num = count_ability_card(ABL_CARD_HANIWA);
		if (card_num)
		{

			int sav_bonus = calc_ability_card_prob(ABL_CARD_HANIWA, card_num);

			p_ptr->skill_sav += sav_bonus;
		}
		//v1.1.86 勾玉カードによるAC上昇
		card_num = count_ability_card(ABL_CARD_MAGATAMA);
		if (card_num)
		{
			int ac_bonus = calc_ability_card_prob(ABL_CARD_MAGATAMA, card_num);

			p_ptr->ac += ac_bonus;
			p_ptr->dis_to_a += ac_bonus;

		}


	}

	/* Monks get extra ac for armour _not worn_ */
	/*:::修行僧と錬気術師のAC追加処理や構え処理*/
	///class 修行僧、練気
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER)) && !heavy_armor())
	{
		if (!(inventory[INVEN_BODY].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev * 3) / 2;
			p_ptr->dis_to_a += (p_ptr->lev * 3) / 2;
		}
		if (!(inventory[INVEN_OUTER].k_idx) && (p_ptr->lev > 15))
		{
			p_ptr->to_a += ((p_ptr->lev - 13) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 13) / 3);
		}
		if (!(inventory[INVEN_LARM].k_idx) && (p_ptr->lev > 10))
		{
			p_ptr->to_a += ((p_ptr->lev - 8) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 8) / 3);
		}
		if (!(inventory[INVEN_HEAD].k_idx) && (p_ptr->lev > 4))
		{
			p_ptr->to_a += (p_ptr->lev - 2) / 3;
			p_ptr->dis_to_a += (p_ptr->lev -2) / 3;
		}
		if (!(inventory[INVEN_HANDS].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 2);
			p_ptr->dis_to_a += (p_ptr->lev / 2);
		}
		if (!(inventory[INVEN_FEET].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 3);
			p_ptr->dis_to_a += (p_ptr->lev / 3);
		}
		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 3;
		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->stat_add[A_INT] -= 1;
			p_ptr->stat_add[A_WIS] -= 1;
			p_ptr->stat_add[A_DEX] -= 2;
			p_ptr->stat_add[A_CON] += 3;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->stat_add[A_STR] -= 2;
			p_ptr->stat_add[A_INT] += 1;
			p_ptr->stat_add[A_WIS] += 1;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 2;
		}
	}

	if (p_ptr->special_defense & KATA_KOUKIJIN)
	{
		for (i = 0; i < 6; i++)
			p_ptr->stat_add[i] += 5;
		p_ptr->to_a -= 50;
		p_ptr->dis_to_a -= 50;
	}

	/* Hack -- aura of fire also provides light */
	if (p_ptr->sh_fire) p_ptr->lite = TRUE;

	///race ゴーレムなどのACボーナス
	/* Golems also get an intrinsic AC bonus */
	if ( prace_is_(RACE_TENNIN) || prace_is_(RACE_NINJA)|| prace_is_(RACE_NYUDOU))
	{
		p_ptr->to_a += 10 + (p_ptr->lev * 2 / 5);
		p_ptr->dis_to_a += 10 + (p_ptr->lev * 2 / 5);
	}
	else if (prace_is_(RACE_SENNIN))
	{
		p_ptr->to_a += 5 + p_ptr->lev / 5;
		p_ptr->dis_to_a += 5 + p_ptr->lev / 5;
	}
	else if (prace_is_(RACE_KOBITO))
	{
		p_ptr->to_a += 30 ;
		p_ptr->dis_to_a += 30;
	}
	else if (prace_is_(RACE_GOLEM))
	{
		p_ptr->to_a += 30 + (p_ptr->lev * 2 / 5);
		p_ptr->dis_to_a += 30 + (p_ptr->lev * 2 / 5);
	}
	else if (prace_is_(RACE_MAGIC_JIZO))
	{
		p_ptr->to_a += 15 + (p_ptr->lev / 5);
		p_ptr->dis_to_a += 15 + (p_ptr->lev / 5);
	}
	else if (p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_YUKARI)
	{
		p_ptr->to_a += 30 + (p_ptr->lev * 2 / 5);
		p_ptr->dis_to_a += 30 + (p_ptr->lev * 2 / 5);
	}



	///mod140121 変異のACボーナス
	if (p_ptr->muta2 & MUT2_HARDHEAD)
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
	}
	if (p_ptr->muta3 & MUT3_PEGASUS)
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
	}
	if (p_ptr->muta3 & (MUT3_ACID_SCALES | MUT3_ELEC_SCALES | MUT3_FIRE_SCALES | MUT3_COLD_SCALES | MUT3_POIS_SCALES))
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
	}



	/* Hex bonuses */
	/*:::呪術師処理*/
	///realm 呪術特殊処理
	/*
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling_any()) p_ptr->skill_stl -= (1 + p_ptr->magic_num2[0]);
		if (hex_spelling(HEX_DETECT_EVIL)) p_ptr->esp_evil = TRUE;
		if (hex_spelling(HEX_XTRA_MIGHT)) p_ptr->stat_add[A_STR] += 4;
		if (hex_spelling(HEX_BUILDING))
		{
			p_ptr->stat_add[A_STR] += 4;
			p_ptr->stat_add[A_DEX] += 4;
			p_ptr->stat_add[A_CON] += 4;
		}
		if (hex_spelling(HEX_DEMON_AURA))
		{
			p_ptr->sh_fire = TRUE;
			p_ptr->regenerate = TRUE;
		}
		if (hex_spelling(HEX_ICE_ARMOR))
		{
			p_ptr->sh_cold = TRUE;
			p_ptr->to_a += 30;
			p_ptr->dis_to_a += 30;
		}
		if (hex_spelling(HEX_SHOCK_CLOAK))
		{
			p_ptr->sh_elec = TRUE;
			new_speed += 3;
		}
		for (i = INVEN_RARM; i <= INVEN_FEET; i++)
		{
			int ac = 0;
			o_ptr = &inventory[i];
			if (!o_ptr->k_idx) continue;
			if (!object_is_armour(o_ptr)) continue;
			if (!object_is_cursed(o_ptr)) continue;
			ac += 5;
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE) ac += 7;
			if (o_ptr->curse_flags & TRC_PERMA_CURSE) ac += 13;
			p_ptr->to_a += ac;
			p_ptr->dis_to_a += ac;
		}
	}
	*/


	if(p_ptr->tim_unite_darkness)
	{
		p_ptr->lite = FALSE;
		p_ptr->see_nocto = TRUE;
	}




	//v1.1.66 ステータス異常「破損」の処理
	if (REF_BROKEN_VAL)
	{
		if (REF_BROKEN_VAL < BROKEN_1)//傷
		{
			p_ptr->stat_add[A_CHR] -= 2;


		}
		else if (REF_BROKEN_VAL < BROKEN_2)//亀裂
		{
			p_ptr->stat_add[A_STR] -= 3;
			p_ptr->stat_add[A_DEX] -= 3;
			p_ptr->stat_add[A_CON] -= 3;
			p_ptr->stat_add[A_CHR] -= 3;
			p_ptr->to_a -= 20;
			p_ptr->dis_to_a -= 20;


		}
		else if (REF_BROKEN_VAL < BROKEN_3) //破損
		{
			for (i = 0; i<6; i++) p_ptr->stat_add[i] -= 4;
			new_speed -= 4;
			p_ptr->to_a -= 30;
			p_ptr->dis_to_a -= 30;
			p_ptr->skill_dev -= 12;
			p_ptr->skill_stl -= 3;
			p_ptr->skill_srh -= 8;
			p_ptr->skill_fos -= 8;
			p_ptr->skill_dig -= 40;
			p_ptr->to_h[0] -= 20;
			p_ptr->to_h[1] -= 20;
			p_ptr->to_h_b -= 20;
			p_ptr->to_h_m -= 20;
			p_ptr->dis_to_h[0] -= 20;
			p_ptr->dis_to_h[1] -= 20;
			p_ptr->dis_to_h_b -= 20;
			p_ptr->to_d[0] -= 10;
			p_ptr->to_d[1] -= 10;
			p_ptr->to_d_m -= 10;
			p_ptr->dis_to_d[0] -= 10;
			p_ptr->dis_to_d[1] -= 10;

		}
		else //大破
		{
			for(i=0;i<6;i++) p_ptr->stat_add[i] -= 7;
			new_speed -= 7;

			p_ptr->to_a -= 50;
			p_ptr->dis_to_a -= 50;
			p_ptr->skill_dev -= 20;
			p_ptr->skill_stl -= 4;
			p_ptr->skill_srh -= 16;
			p_ptr->skill_fos -= 16;
			p_ptr->skill_dig -= 80;
			p_ptr->to_h[0] -= 40;
			p_ptr->to_h[1] -= 40;
			p_ptr->to_h_b -= 40;
			p_ptr->to_h_m -= 40;
			p_ptr->dis_to_h[0] -= 40;
			p_ptr->dis_to_h[1] -= 40;
			p_ptr->dis_to_h_b -= 40;
			p_ptr->to_d[0] -= 20;
			p_ptr->to_d[1] -= 20;
			p_ptr->to_d_m -= 20;
			p_ptr->dis_to_d[0] -= 20;
			p_ptr->dis_to_d[1] -= 20;

		}
	}




	/*↓基本パラメータ確定処理**********************************************************************/
	/* Calculate stats */
	for (i = 0; i < 6; i++)
	{
		int top, use, ind;

		///mod140325 一時能力増加
		bool flag_limitbreak = FALSE;
		int timstatplus = 0;

		/*:::一時的能力増加の適用と後の限界突破処理要フラグ処理*/

		/*::: -Hack- 巨大化は最優先で処理 種族修正値でパラメータ加算したのでここでstat_addには加算しない*/
		if((p_ptr->mimic_form == MIMIC_GIGANTIC || p_ptr->mimic_form == MIMIC_DRAGON)&& (i==A_STR || i==A_CON) )
		{
			timstatplus = 20;
			flag_limitbreak = TRUE;
		}
		//スーパー貧乏神の紫苑
		else if (SUPER_SHION)
		{
			timstatplus = plev * 2 / 5;
			flag_limitbreak = TRUE;
		}
		//アメノウズメ
		else if((p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME) && (i==A_CHR) )
		{
			timstatplus = 20;
			flag_limitbreak = TRUE;
		}
		//界王拳
		else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0] && (i == A_STR || i == A_DEX || i == A_CON))
		{
			timstatplus = 20;
			flag_limitbreak = TRUE;
		}
		//山如「殺戮の山の女王」使用中
		else if (music_singing(MUSIC_NEW_SANNYO_BERSERK) && (i == A_STR || i == A_DEX || i == A_CON))
		{
			timstatplus = 10;
			flag_limitbreak = TRUE;

		}
		//狂戦士化 腕力+5
		else if(i==A_STR && p_ptr->shero && p_ptr->tim_addstat_num[i] < 105)
		{
			/*::: hack 狂戦士化したときに斬魔刀を両手で持っていると腕力上昇が10になる */
			//v1.1.88 別に両手持ちでなくていいか
			if(check_equip_specific_fixed_art(ART_ZANMATOU,TRUE))timstatplus = 10;
			//if(!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].name1 == ART_ZANMATOU ) timstatplus = 10;
			else timstatplus = 5;
			flag_limitbreak = TRUE;
		}
		else if(p_ptr->tim_addstat_count[i])
		{
			timstatplus = p_ptr->tim_addstat_num[i];
			if(timstatplus >= 100)
			{
				timstatplus -= 100;
				flag_limitbreak = TRUE;
			}
		}
		//鈴瑚強化
		if(p_ptr->pclass == CLASS_RINGO && (i == A_STR || i == A_DEX || i == A_CON))
		{
			if(p_ptr->magic_num1[1] && (p_ptr->magic_num1[1] > timstatplus || !flag_limitbreak))
			{
				flag_limitbreak = TRUE;
				timstatplus = p_ptr->magic_num1[1];
			}
		}

		p_ptr->stat_add[i] += timstatplus;


		/* Extract the new "stat_use" value for the stat */
		/*:::stat_max[]に先ほどまで計算した増減値を足す。18/10などの表記に対応。*/
		/*:::Q017このルーチンが始まってからstat_max[]が初期化されてないように見えるんだがどこで整合を取っているのだろう。*/
		/*:::↑stat_max[]はキャラメイク時+レベルアップ+薬の基礎値でここでは参照されてるだけのようだ*/
		top = modify_stat_value(p_ptr->stat_max[i], p_ptr->stat_add[i]);

		/* Notice changes */
		if (p_ptr->stat_top[i] != top)
		{
			/* Save the new value */
			p_ptr->stat_top[i] = top;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(p_ptr->stat_cur[i], p_ptr->stat_add[i]);
		/*:::幻影に覆われている突然変異の処理*/
#if 0
		///mutation 幻影変異と魅力
		if ((i == A_CHR) && (p_ptr->muta3 & MUT3_ILL_NORM))
		{
			/* 10 to 18/90 charisma, guaranteed, based on level */
			if (use < 8 + 2 * p_ptr->lev)
			{
				use = 8 + 2 * p_ptr->lev;
			}
		}
#endif
		/* Notice changes */
		if (p_ptr->stat_use[i] != use)
		{
			/* Save the new value */
			p_ptr->stat_use[i] = use;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}

		///sys この辺18/***表記関連
		/* Values: 3, 4, ..., 17 */

		///mod140104 パラメータ上限と表記を変更	ここ3行変更
		ind = use - 3;
		if(ind < 0) ind = 0;

		//勇儀の腕力はデフォで限界突破
		if(p_ptr->pclass == CLASS_YUGI && i == A_STR)
		{
			if(ind > STAT_HYPERMAX-3) ind = STAT_HYPERMAX-3;
		}
		//性格狂気の腕力体力も限界無視
		else if( p_ptr->pseikaku == SEIKAKU_BERSERK && (i == A_STR || i == A_CON))
		{
			if(ind > STAT_HYPERMAX-3) ind = STAT_HYPERMAX-3;
		}
		//輝夜は魅力限界突破してみる
		else if( p_ptr->pclass == CLASS_KAGUYA && i == A_CHR)
		{
			if(ind > STAT_HYPERMAX-3) ind = STAT_HYPERMAX-3;
		}
		else
		{
			if(ind > STAT_ADD_MAX-3) ind = STAT_ADD_MAX-3;
		}
		///mod140325
		/*:::-Mega Hack- 特定の一時能力増加によるパラメータ限界突破処理 p_ptr->tim_addstat_num[]に100以上の値がセットされたとき値-100が能力に加算されその値の範囲で能力限界が40から50に変わる*/
		if(flag_limitbreak && use > STAT_ADD_MAX)
		{
			//超過値を計算しindに加算する。
			int excess = timstatplus;
			if( (use - STAT_ADD_MAX) < excess) excess = use - STAT_ADD_MAX;
			if((STAT_HYPERMAX - STAT_ADD_MAX) < excess) excess = (STAT_HYPERMAX - STAT_ADD_MAX); //最大10

			ind += excess;
			///mod150131 勇儀腕力超過対応
			if(ind > STAT_HYPERMAX - 3) ind = STAT_HYPERMAX - 3;
		}
#if 0
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18+219) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else ind = (37);
#endif
		/* Notice changes */
		if (p_ptr->stat_ind[i] != ind)
		{
			/* Save the new index */
			p_ptr->stat_ind[i] = ind;

			/* Change in CON affects Hitpoints */
			if (i == A_CON)
			{
				p_ptr->update |= (PU_HP);
			}

			if(i == cp_ptr->spell_stat) p_ptr->update |= (PU_SPELLS);
			if (i == A_WIS)	p_ptr->update |= (PU_MANA);

#if 0
			/* Change in INT may affect Mana/Spells */
			else if (i == A_INT)
			{
				if (cp_ptr->spell_stat == A_INT)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_WIS)
			{
				if (cp_ptr->spell_stat == A_WIS)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_CHR)
			{
				if (cp_ptr->spell_stat == A_CHR)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}
#endif
			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}
	}

	//v1.1.66 磨弓「忠誠心が力になる」能力　腕力耐久が賢さより低い場合賢さの値まで上がる
	if (p_ptr->pclass == CLASS_MAYUMI)
	{
		if (p_ptr->stat_top[A_STR] < p_ptr->stat_top[A_WIS]) p_ptr->stat_top[A_STR] = p_ptr->stat_top[A_WIS];
		if (p_ptr->stat_use[A_STR] < p_ptr->stat_use[A_WIS]) p_ptr->stat_use[A_STR] = p_ptr->stat_use[A_WIS];
		if (p_ptr->stat_ind[A_STR] < p_ptr->stat_ind[A_WIS]) p_ptr->stat_ind[A_STR] = p_ptr->stat_ind[A_WIS];

		if (p_ptr->stat_top[A_CON] < p_ptr->stat_top[A_WIS]) p_ptr->stat_top[A_CON] = p_ptr->stat_top[A_WIS];
		if (p_ptr->stat_use[A_CON] < p_ptr->stat_use[A_WIS]) p_ptr->stat_use[A_CON] = p_ptr->stat_use[A_WIS];
		if (p_ptr->stat_ind[A_CON] < p_ptr->stat_ind[A_WIS]) p_ptr->stat_ind[A_CON] = p_ptr->stat_ind[A_WIS];

	}





	//格闘家の重量が軽い時のボーナス　パラメータ計算後にしないと装備変更のとき判定がおかしくなる
	if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && !heavy_armor())
	{
		p_ptr->to_a += 5 + plev / 2;
		p_ptr->dis_to_a += 5 + plev / 2;
		new_speed += (plev+5) / 15;
	}


	/*:::キスメ特殊処理*/
	if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent)
	{
		p_ptr->to_h[0] += p_ptr->concent * 10;
		p_ptr->dis_to_h[0] += p_ptr->concent * 10;
		p_ptr->to_h[1] += p_ptr->concent * 10;
		p_ptr->dis_to_h[1] += p_ptr->concent * 10;
		p_ptr->to_d[0] += p_ptr->concent * 10;
		p_ptr->dis_to_d[0] += p_ptr->concent * 10;
		p_ptr->to_d[1] += p_ptr->concent * 10;
		p_ptr->dis_to_d[1] += p_ptr->concent * 10;
	}


	/* Apply temporary "stun" */
	/*:::ひどく朦朧*/
	///mod140402 溺れている時もひどい朦朧と同様のペナルティ ちょっと適当
	if (p_ptr->stun > 50 || p_ptr->drowning)
	{
		p_ptr->to_h[0] -= 20;
		p_ptr->to_h[1] -= 20;
		p_ptr->to_h_b  -= 20;
		p_ptr->to_h_m  -= 20;
		p_ptr->dis_to_h[0] -= 20;
		p_ptr->dis_to_h[1] -= 20;
		p_ptr->dis_to_h_b  -= 20;
		p_ptr->to_d[0] -= 20;
		p_ptr->to_d[1] -= 20;
		p_ptr->to_d_m -= 20;
		p_ptr->dis_to_d[0] -= 20;
		p_ptr->dis_to_d[1] -= 20;
	}
	/*:::朦朧*/
	else if (p_ptr->stun)
	{
		p_ptr->to_h[0] -= 5;
		p_ptr->to_h[1] -= 5;
		p_ptr->to_h_b -= 5;
		p_ptr->to_h_m -= 5;
		p_ptr->dis_to_h[0] -= 5;
		p_ptr->dis_to_h[1] -= 5;
		p_ptr->dis_to_h_b -= 5;
		p_ptr->to_d[0] -= 5;
		p_ptr->to_d[1] -= 5;
		p_ptr->to_d_m -= 5;
		p_ptr->dis_to_d[0] -= 5;
		p_ptr->dis_to_d[1] -= 5;
	}

	if (p_ptr->afraid)
	{
		p_ptr->to_h[0] -= 5;
		p_ptr->to_h[1] -= 5;
		p_ptr->to_h_b -= 5;
		p_ptr->to_h_m -= 5;
		p_ptr->dis_to_h[0] -= 5;
		p_ptr->dis_to_h[1] -= 5;
		p_ptr->dis_to_h_b -= 5;
		p_ptr->to_d[0] -= 5;
		p_ptr->to_d[1] -= 5;
		p_ptr->to_d_m -= 5;
		p_ptr->dis_to_d[0] -= 5;
		p_ptr->dis_to_d[1] -= 5;
	}

///sys 一時効果でパラメータ限界突破するならこの辺に表記
	/*:::一時効果など*/
	/* Wraith form */
	///mod140213 ルーミアのとき幽体化（ダークサイドオブザムーン）で壁抜けしないようにした
	if (p_ptr->wraith_form)
	{
		p_ptr->reflect = TRUE;
		if(p_ptr->pclass != CLASS_RUMIA) p_ptr->pass_wall = TRUE;
	}

	if (p_ptr->kabenuke)
	{
		p_ptr->pass_wall = TRUE;
	}
	if(p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->pass_wall = TRUE;
	}

	/* Temporary blessing */
	if (IS_BLESSED())
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
		p_ptr->to_h[0] += 10;
		p_ptr->to_h[1] += 10;
		p_ptr->to_h_b  += 10;
		p_ptr->to_h_m  += 10;
		p_ptr->dis_to_h[0] += 10;
		p_ptr->dis_to_h[1] += 10;
		p_ptr->dis_to_h_b += 10;
		p_ptr->skill_sav += 10;
	}
/*
	if (p_ptr->magicdef)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->levitation = TRUE;
	}
*/

	///mod140725 予見の魔法剣の命中率修正
	if(p_ptr->special_attack & ATTACK_FORESIGHT)
	{
			p_ptr->to_h[0] += (p_ptr->lev);
			p_ptr->to_h[1] += (p_ptr->lev);
			p_ptr->dis_to_h[0] += (p_ptr->lev);
			p_ptr->dis_to_h[1] += (p_ptr->lev);
	}

	/* Temporary "Hero" */
	if (IS_HERO())
	{
		p_ptr->to_h[0] += 12;
		p_ptr->to_h[1] += 12;
		p_ptr->to_h_b  += 12;
		p_ptr->to_h_m  += 12;
		p_ptr->dis_to_h[0] += 12;
		p_ptr->dis_to_h[1] += 12;
		p_ptr->dis_to_h_b  += 12;
	}


	/* Temporary "Beserk" */
	if (p_ptr->shero)
	{
		///mod140325 狂戦士化で腕力を上げるので殺戮修正は削除
		//p_ptr->to_h[0] += 12;
		//p_ptr->to_h[1] += 12;
		p_ptr->to_h_b  -= 12;
		//p_ptr->to_h_m  += 12;
		///mod150808 アリスのみ威力上昇する
		if(alice_doll_attack)
		{
			p_ptr->to_d[0] += 2+(p_ptr->lev/10);
			p_ptr->to_d[1] += 2+(p_ptr->lev/10);
			p_ptr->to_d_m  += 2+(p_ptr->lev/10);
			p_ptr->dis_to_d[0] += 2+(p_ptr->lev/10);
			p_ptr->dis_to_d[1] += 2+(p_ptr->lev/10);
		}
		//p_ptr->to_d[0] += 3+(p_ptr->lev/5);
		//p_ptr->to_d[1] += 3+(p_ptr->lev/5);
		//p_ptr->to_d_m  += 3+(p_ptr->lev/5);
		//p_ptr->dis_to_h[0] += 12;
		//p_ptr->dis_to_h[1] += 12;
		p_ptr->dis_to_h_b  -= 12;
		p_ptr->to_a -= 10;
		p_ptr->dis_to_a -= 10;
		p_ptr->skill_stl -= 7;
		p_ptr->skill_dev -= 20;
		p_ptr->skill_sav -= 30;
		p_ptr->skill_srh -= 15;
		p_ptr->skill_fos -= 15;
		p_ptr->skill_tht -= 20;
		p_ptr->skill_dig += 30;
	}

	/* Temporary "fast" */
	if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0] && !p_ptr->riding)
	{
		new_speed += 20;
	}
	else if (IS_FAST())
	{
		new_speed += 10;
	}

	/* Temporary "slow" */
	if (p_ptr->slow)
	{
		new_speed -= 10;
	}

	/* Temporary "telepathy" */
	if (IS_TIM_ESP())
	{
		p_ptr->telepathy = TRUE;
	}

	/*:::魔法による元素免疫　全免疫のためには少し書き換える必要ある*/
	///mod140211 元素全免疫の魔法のために少し変更
	if (p_ptr->ele_immune)
	{
		if (p_ptr->special_defense & DEFENSE_ACID)
			p_ptr->immune_acid = TRUE;
		if (p_ptr->special_defense & DEFENSE_ELEC)
			p_ptr->immune_elec = TRUE;
		if (p_ptr->special_defense & DEFENSE_FIRE)
			p_ptr->immune_fire = TRUE;
		if (p_ptr->special_defense & DEFENSE_COLD)
			p_ptr->immune_cold = TRUE;
	}

	/* Temporary see invisible */
	if (p_ptr->tim_invis)
	{
		p_ptr->see_inv = TRUE;
	}

	/* Temporary infravision boost */
	if (p_ptr->tim_infra)
	{
		p_ptr->see_infra+=3;
	}

	/* Temporary regeneration boost */
	if (p_ptr->tim_regen)
	{
		p_ptr->regenerate = TRUE;
	}

	/* Temporary levitation */
	if (p_ptr->tim_levitation)
	{
		p_ptr->levitation = TRUE;
	}

	/* Temporary reflection */
	if (p_ptr->tim_reflect || (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO2)))
	{
		p_ptr->reflect = TRUE;
	}


	if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
	{
		p_ptr->reflect = TRUE;
		p_ptr->resist_lite = TRUE;
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_SUMIYOSHI)
	{
		p_ptr->speedster = TRUE;
		p_ptr->resist_water = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->warning = TRUE;

	}
	if (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
	{
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->skill_sav += (20 + (p_ptr->lev / 5));

	}
	if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
	{
		p_ptr->resist_fire = TRUE;
		p_ptr->immune_fire = TRUE;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if (IS_HERO() || p_ptr->shero)
	{
		p_ptr->resist_fear = TRUE;
	}
	/* 未来予知 */
	if (p_ptr->foresight)
	{
		//v1.1.80 命中上昇追加
		p_ptr->to_h[0] += 24;
		p_ptr->to_h[1] += 24;
		p_ptr->to_h_b += 24;
		p_ptr->to_h_m += 24;
		p_ptr->dis_to_h[0] += 24;
		p_ptr->dis_to_h[1] += 24;
		p_ptr->dis_to_h_b += 24;

		p_ptr->warning = TRUE;
	}


	/* Hack -- Telepathy Change */
	///item ESP
	if (p_ptr->telepathy != old_telepathy)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	if ((p_ptr->esp_animal != old_esp_animal) ||
	    (p_ptr->esp_undead != old_esp_undead) ||
	    (p_ptr->esp_demon != old_esp_demon) ||
	    (p_ptr->esp_kwai != old_esp_kwai) ||
	    (p_ptr->esp_deity != old_esp_deity) ||
	    (p_ptr->esp_dragon != old_esp_dragon) ||
	    (p_ptr->esp_human != old_esp_human) ||
	    (p_ptr->esp_evil != old_esp_evil) ||
	    (p_ptr->esp_good != old_esp_good) ||
	    (p_ptr->esp_nonliving != old_esp_nonliving) ||
	    (p_ptr->esp_unique != old_esp_unique))
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Hack -- See Invis Change */
	if (p_ptr->see_inv != old_see_inv)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Bloating slows the player down (a little) */
	if (p_ptr->food >= PY_FOOD_MAX) new_speed -= 10;

	if (p_ptr->special_defense & KAMAE_SUZAKU) new_speed += 10;

	/*:::格闘技能による命中率補正*/
	//if ((p_ptr->migite && (empty_hands_status & EMPTY_HAND_RARM)) ||
	//    (p_ptr->hidarite && (empty_hands_status & EMPTY_HAND_LARM)))
	if(p_ptr->do_martialarts)
	{
		///mod131226 skill 技能と武器技能の統合
		///pend 格闘技能による格闘命中補正　いずれ調整入れる予定

		if(p_ptr->mimic_form == MIMIC_BEAST ||  p_ptr->mimic_form == MIMIC_DRAGON || p_ptr->mimic_form == MIMIC_SLIME )
		{
			p_ptr->to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 300 + 30;
			p_ptr->dis_to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 300 + 30;
		}
		else if(p_ptr->clawextend || p_ptr->mimic_form == MIMIC_CAT || p_ptr->mimic_form == MIMIC_DEMON_LORD)
		{
			p_ptr->to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 400 + 20;
			p_ptr->dis_to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 400 + 20;
		}
		else
		{
			p_ptr->to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 200;
			p_ptr->dis_to_h[default_hand] += (ref_skill_exp(SKILL_MARTIALARTS) - WEAPON_EXP_BEGINNER) / 200;
		}
		//弁々ダブルスコアにボーナス
		if(p_ptr->pclass == CLASS_BENBEN && music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE) && !p_ptr->mimic_form)
		{
			p_ptr->to_h[default_hand] += 30;
			p_ptr->dis_to_h[default_hand] += 30;
			p_ptr->to_d[default_hand] += 10;
			p_ptr->dis_to_d[default_hand] += 10;

		}


		//p_ptr->to_h[default_hand] += (p_ptr->skill_exp[GINOU_SUDE] - WEAPON_EXP_BEGINNER) / 200;
		//p_ptr->dis_to_h[default_hand] += (p_ptr->skill_exp[GINOU_SUDE] - WEAPON_EXP_BEGINNER) / 200;
	}

	//雛には厄の量に応じてボーナス
	if(p_ptr->pclass == CLASS_HINA)
	{
		int yaku = p_ptr->magic_num1[0];

		if(p_ptr->do_martialarts)
		{
			p_ptr->to_dd[0] += yaku / 999;
			p_ptr->to_h[0] += yaku / 444;
			p_ptr->dis_to_h[0] += yaku / 444;
			p_ptr->to_d[0] += yaku / 666;
			p_ptr->dis_to_d[0] += yaku / 666;
		}
		else
		{
			p_ptr->to_dd[0] += yaku / 4800;
			p_ptr->to_h[0] += yaku / 1200;
			p_ptr->dis_to_h[0] += yaku / 1200;
			p_ptr->to_d[0] += yaku / 1800;
			p_ptr->dis_to_d[0] += yaku / 1800;

			p_ptr->to_dd[1] += yaku / 4800;
			p_ptr->to_h[1] += yaku / 1200;
			p_ptr->dis_to_h[1] += yaku / 1200;
			p_ptr->to_d[1] += yaku / 1800;
			p_ptr->dis_to_d[1] += yaku / 1800;
		}
		p_ptr->to_a += yaku / 200;
		p_ptr->dis_to_a += yaku / 200;

		if(yaku > HINA_YAKU_LIMIT2) p_ptr->cursed |= (TRC_AGGRAVATE);
		if(yaku > HINA_YAKU_LIMIT1) p_ptr->skill_stl -= yaku / 1500;

	}


	/*:::二刀流命中率計算　特殊武器によるボーナス有り*/
	///mod151205 三月精はペナルティなし
	if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM) && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		int penalty1, penalty2;
		///mod131226 skill 技能と武器技能の統合
		//penalty1 = ((100 - p_ptr->skill_exp[GINOU_NITOURYU] / 160) - (130 - inventory[INVEN_RARM].weight) / 8);
		//penalty2 = ((100 - p_ptr->skill_exp[GINOU_NITOURYU] / 160) - (130 - inventory[INVEN_LARM].weight) / 8);
		penalty1 = ((100 - ref_skill_exp(SKILL_2WEAPONS) / 160) - (130 - inventory[INVEN_RARM].weight) / 8);
		penalty2 = ((100 - ref_skill_exp(SKILL_2WEAPONS) / 160) - (130 - inventory[INVEN_LARM].weight) / 8);

		//v1.1.62 二刀流計算順番変更
		//源氏エゴなど
		if (easy_2weapon)
		{
			if (penalty1 > 0) penalty1 /= 2;
			if (penalty2 > 0) penalty2 /= 2;
		}

		//刺セットボーナス
		if ((inventory[INVEN_RARM].name1 == ART_QUICKTHORN) && (inventory[INVEN_LARM].name1 == ART_TINYTHORN))
		{
			penalty1 = penalty1 / 2 - 5;
			penalty2 = penalty2 / 2 - 5;
			new_speed += 7;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}
		//v1.1.65 鬼切丸(髭切)と薄縁(膝丸)のセットボーナス
		else if ((inventory[INVEN_RARM].name1 == ART_HIZAKIRIMARU) && (inventory[INVEN_LARM].name1 == ART_ONIKIRIMARU)
			|| (inventory[INVEN_RARM].name1 == ART_ONIKIRIMARU) && (inventory[INVEN_LARM].name1 == ART_HIZAKIRIMARU))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;

		}
		//宮本武蔵セットボーナス
		else if ((inventory[INVEN_RARM].name1 == ART_MUSASI_KATANA) && (inventory[INVEN_LARM].name1 == ART_MUSASI_WAKIZASI))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}

		else
		{
			//右手に武蔵の刀を持っているとペナルティ軽減
			if (inventory[INVEN_RARM].name1 == ART_MUSASI_KATANA && penalty1 > 0)
			{
				penalty1 /= 2;
			}
			//左手に武蔵の脇差を持っているとペナルティ軽減
			if (inventory[INVEN_LARM].name1 == ART_MUSASI_WAKIZASI && penalty2 > 0)
			{
				penalty1 = MAX(0, penalty1 - 10);
				penalty2 /= 2;
			}
			//左手にマンゴーシュか脇差を持っているとペナルティ軽減
			else if ((inventory[INVEN_LARM].tval == TV_KNIFE) && (inventory[INVEN_LARM].sval == SV_WEAPON_MAIN_GAUCHE)
				|| (inventory[INVEN_LARM].tval == TV_KATANA) && (inventory[INVEN_LARM].sval == SV_WEAPON_SHORT_KATANA))
			{
				penalty1 = MAX(0, penalty1 - 10);
				penalty2 = MAX(0, penalty2 - 10);
			}

		}

		if (inventory[INVEN_RARM].tval == TV_POLEARM) penalty1 += 10;
		if (inventory[INVEN_LARM].tval == TV_POLEARM) penalty2 += 10;
		p_ptr->to_h[0] -= penalty1;
		p_ptr->to_h[1] -= penalty2;
		p_ptr->dis_to_h[0] -= penalty1;
		p_ptr->dis_to_h[1] -= penalty2;
	}

	/* Extract the current weight (in tenth pounds) */
	j = p_ptr->total_weight;

	if (!p_ptr->riding)
	{
		/* Extract the "weight limit" (in tenth pounds) */
		/*:::重量制限を計算*/
		i = (int)weight_limit();
	}
	//v1.1.41 舞と里乃の騎乗は重量制限や加速修正の対象にならない。
	else if (CLASS_RIDING_BACKDANCE)
	{
		monster_type *riding_m_ptr = &m_list[p_ptr->riding];
		monster_race *riding_r_ptr = &r_info[riding_m_ptr->r_idx];

		riding_levitation = (riding_r_ptr->flags7 & RF7_CAN_FLY) ? TRUE : FALSE;
		if (riding_r_ptr->flags7 & (RF7_CAN_SWIM | RF7_AQUATIC)) p_ptr->can_swim = TRUE;
		if (!(riding_r_ptr->flags2 & RF2_PASS_WALL)) p_ptr->pass_wall = FALSE;
		if (riding_r_ptr->flags2 & RF2_KILL_WALL) p_ptr->kill_wall = TRUE;
		i = (int)weight_limit();

	}
	/*:::騎乗時の重量制限と加速修正など*/
	else
	{
		monster_type *riding_m_ptr = &m_list[p_ptr->riding];
		monster_race *riding_r_ptr = &r_info[riding_m_ptr->r_idx];
		int speed = riding_m_ptr->mspeed;

		if (riding_m_ptr->mspeed > 110)
		{
			///mod131226 skill 技能と武器技能の統合
			//new_speed = 110 + (s16b)((speed - 110) * (p_ptr->skill_exp[GINOU_RIDING] * 3 + p_ptr->lev * 160L - 10000L) / (22000L));
			new_speed = 110 + (s16b)((speed - 110) * (ref_skill_exp(SKILL_RIDING) * 3 + p_ptr->lev * 160L - 10000L) / (22000L));
			if (new_speed < 110) new_speed = 110;
		}
		else
		{
			new_speed = speed;
		}
		new_speed += (p_ptr->skill_exp[GINOU_RIDING] + p_ptr->lev *160L)/3200;

		//v1.1.24 乗り物は低熟練度でも減速しない
		if(riding_r_ptr->flags7 & RF7_ONLY_RIDING && new_speed < riding_m_ptr->mspeed) new_speed = riding_m_ptr->mspeed;

		/*:::騎乗加速処理*/
		if (MON_FAST(riding_m_ptr)) new_speed += 10;
		if (MON_SLOW(riding_m_ptr)) new_speed -= 10;
		riding_levitation = (riding_r_ptr->flags7 & RF7_CAN_FLY) ? TRUE : FALSE;
		/*:::騎乗水中進入処理*/
		if (riding_r_ptr->flags7 & (RF7_CAN_SWIM | RF7_AQUATIC)) p_ptr->can_swim = TRUE;

		/*:::騎乗壁抜け・壁堀り処理*/
		if (!(riding_r_ptr->flags2 & RF2_PASS_WALL)) p_ptr->pass_wall = FALSE;
		if (riding_r_ptr->flags2 & RF2_KILL_WALL) p_ptr->kill_wall = TRUE;
		///mod131226 skill 技能と武器技能の統合
		//if (p_ptr->skill_exp[GINOU_RIDING] < RIDING_EXP_SKILLED) j += (p_ptr->wt * 3 * (RIDING_EXP_SKILLED - p_ptr->skill_exp[GINOU_RIDING])) / RIDING_EXP_SKILLED;
		if (ref_skill_exp(SKILL_RIDING) < RIDING_EXP_SKILLED) j += (p_ptr->wt * 3 * (RIDING_EXP_SKILLED - ref_skill_exp(SKILL_RIDING))) / RIDING_EXP_SKILLED;

		/* Extract the "weight limit" */
		i = 1500 + riding_r_ptr->level * 25;
	}

	/* XXX XXX XXX Apply "encumbrance" from weight */
	/*:::encumbrance:(重荷/負担)　20%重量超過ごとに1減速*/
	if (j > i) new_speed -= ((j - i) / (i / 5));

	/* Searching slows the player down */
	if (p_ptr->action == ACTION_SEARCH) new_speed -= 10;

	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	/*:::パラメータによる各能力へのボーナス適用　配列はtables.cで定義されてるがなぜ配列で128足してここで128引くのか？なんか符号関係の不具合防止？*/
	p_ptr->to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);

	//一輪格闘時特殊処理
	if(p_ptr->pclass == CLASS_ICHIRIN && p_ptr->do_martialarts)
	{
		p_ptr->to_d[0] += 5 + plev / 5;
		p_ptr->to_d[1] += 5 + plev / 5;
		p_ptr->to_d_m  += 5 + plev / 5;
		p_ptr->dis_to_d[0] += 5 + plev / 5;
		p_ptr->dis_to_d[1] += 5 + plev / 5;
	}
	//それ以外の腕力修正(アリスの人形攻撃に腕力は関わらない)
	else if(!alice_doll_attack)
	{
		p_ptr->to_d[0] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->to_d[1] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->to_d_m  += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->dis_to_d[0] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->dis_to_d[1] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	}

	p_ptr->to_h[0] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h[1] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_m  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);

	if(!alice_doll_attack)
	{
		p_ptr->to_h[0] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->to_h[1] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
		p_ptr->to_h_m  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	}
	/* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->dis_to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h[0] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h[1] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h[0] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h[1] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);


	/* Obtain the "hold" value */
	/*:::STRから武器重量限界を計算*/
	///system 武器重量限界計算　ここ作り直す予定
	hold = adj_str_hold[p_ptr->stat_ind[A_STR]];


	/* Examine the "current bow" */
	///item 弓枠処理
///mod140308 弓枠廃止処理関連
	//o_ptr = &inventory[INVEN_BOW];
	{
		int dummy;
		o_ptr = &inventory[INVEN_PACK + shootable(&dummy)];
		/* Assume not heavy */
		p_ptr->shoot_penalty = dummy;
	}
	/* It is hard to carholdry a heavy bow */
	/*:::carholdry（しっかり保持する）？　重い弓は命中率修正が下がる処理*/
	/*:::弓を装備していなかった場合はここの処理どうなる？o_ptr->weightが0？*/
#if 0
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy bow */
		p_ptr->to_h_b  += 2 * (hold - o_ptr->weight / 10);
		p_ptr->dis_to_h_b  += 2 * (hold - o_ptr->weight / 10);

		/* Heavy Bow */
		p_ptr->heavy_shoot = TRUE;
	}
#endif

	/* Compute "extra shots" if needed */
	/*:::装備している弓の種類に合わせて適切な矢玉の種類を判定*/
	///item 矢玉選定
	if (o_ptr->k_idx)
	{
		if(o_ptr->tval == TV_BOW)p_ptr->tval_ammo = TV_ARROW;
		else if(o_ptr->tval == TV_CROSSBOW)p_ptr->tval_ammo = TV_BOLT;
		else if(o_ptr->tval == TV_GUN)p_ptr->tval_ammo = TV_BULLET;

		/* Apply special flags */
	//	if (o_ptr->k_idx && !p_ptr->heavy_shoot)

		{


			/* Extra shots */
			/*:::装備、職業による追加射撃処理*/
			///class 射撃回数
			p_ptr->num_fire += (extra_shots * 100);

			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) p_ptr->num_fire += (p_ptr->lev * 2);

			/* Hack -- Rangers love Bows */
			if ((p_ptr->pclass == CLASS_RANGER || p_ptr->pclass == CLASS_EIRIN) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 4);
			}

			if ((p_ptr->pclass == CLASS_CAVALRY || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_FUTO || p_ptr->pclass == CLASS_MAYUMI) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 3);
			}

			if (p_ptr->pclass == CLASS_ARCHER)
			{
				if (p_ptr->tval_ammo == TV_ARROW)
					p_ptr->num_fire += ((p_ptr->lev * 5)+50);
				else if ((p_ptr->tval_ammo == TV_BOLT))
					p_ptr->num_fire += (p_ptr->lev * 4);
			}
			if (p_ptr->pclass == CLASS_HATATE || p_ptr->pclass == CLASS_LYRICA)
			{
				if (p_ptr->tval_ammo == TV_ARROW)
					p_ptr->num_fire += p_ptr->lev * 2;
				else if ((p_ptr->tval_ammo == TV_BOLT))
					p_ptr->num_fire += (p_ptr->lev * 3);
			}



			/*
			 * Addendum -- also "Reward" high level warriors,
			 * with _any_ missile weapon -- TY
			 */
			if (p_ptr->pclass == CLASS_WARRIOR &&
			   (p_ptr->tval_ammo == TV_ARROW || p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->num_fire += (p_ptr->lev * 2);
			}
			//古道具屋はクロスボウの射撃が地味に上手い
			if (p_ptr->pclass == CLASS_SH_DEALER && ( p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->num_fire += (p_ptr->lev);
			}
			///mod140415 探検家のクロスボウ射撃ボーナス
			if ((p_ptr->pclass == CLASS_SEIRAN || p_ptr->pclass == CLASS_ROGUE || p_ptr->pclass == CLASS_RANGER|| p_ptr->pclass == CLASS_SOLDIER) &&
			    (p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->num_fire += (p_ptr->lev * 2);
			}
			///mod140819 エンジニアのクロスボウ射撃ボーナス
			if ((p_ptr->pclass == CLASS_NITORI || p_ptr->pclass == CLASS_ENGINEER) &&  (p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->num_fire += (p_ptr->lev);
			}

			/* Snipers love Cross bows */
			if ((p_ptr->pclass == CLASS_SNIPER) &&
				(p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->to_h_b += (10 + (p_ptr->lev / 5));
				p_ptr->dis_to_h_b += (10 + (p_ptr->lev / 5));
			}

		}
		///mod140315 弓ペナルティ

		if(p_ptr->shoot_penalty == SHOOT_HIGH_PENALTY)
		{
			p_ptr->num_fire = p_ptr->num_fire * 80 / 100;
			p_ptr->to_h_b = p_ptr->to_h_b / 2;
			p_ptr->dis_to_h_b = p_ptr->dis_to_h_b / 2;
		}
		else if(p_ptr->shoot_penalty == SHOOT_LOW_PENALTY)
		{
			p_ptr->to_h_b = p_ptr->to_h_b * 2 / 3;
			p_ptr->dis_to_h_b = p_ptr->dis_to_h_b * 2 / 3;
		}



	}


	if (p_ptr->ryoute) hold *= 2;
	else if(p_ptr->pclass == CLASS_3_FAIRIES) hold *= 2;
	if (p_ptr->pclass == CLASS_YUGI) hold *= 2; //勇儀は重量過多になりにくい

	/*:::両手に持った武器の処理*/
	for(i = 0 ; i < 2 ; i++)
	{
		/* Examine the "main weapon" */
		o_ptr = &inventory[INVEN_RARM+i];

		object_flags(o_ptr, flgs);

		/* Assume not heavy */
		p_ptr->heavy_wield[i] = FALSE;
		p_ptr->icky_wield[i] = FALSE;
		p_ptr->riding_wield[i] = FALSE;

		/*:::攻撃回数。武器を持ってないほうは(一時的に)攻撃回数が1になる*/
		if (!buki_motteruka(INVEN_RARM+i))
		{
			p_ptr->num_blow[i]=1;
			continue;
		}
		/* It is hard to hold a heavy weapon */
		if (hold < o_ptr->weight / 10)
		{
			/* Hard to wield a heavy weapon */
			p_ptr->to_h[i] += 2 * (hold - o_ptr->weight / 10);
			p_ptr->dis_to_h[i] += 2 * (hold - o_ptr->weight / 10);

			/* Heavy weapon */
			p_ptr->heavy_wield[i] = TRUE;
		}
		/*:::heavy_wieldとomoiはどう違うのか?*/
		/*:::武器があまりに重いとomoiがTRUEになり、いくつかのボーナスが適用されなくなるらしい*/
		else if (p_ptr->ryoute && (hold < o_ptr->weight/5)) omoi = TRUE;

		/*:::マンゴーシュか脇差を左に持っていると命中率が少し高い*/
		///mod151205 三月精除く
		if ((i == 1) && (p_ptr->pclass != CLASS_3_FAIRIES) && ((o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_MAIN_GAUCHE)
			|| (o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_SHORT_KATANA)))
		{
			p_ptr->to_a += 5;
			p_ptr->dis_to_a += 5;
		}

		/* Normal weapons */
		/*:::攻撃回数計算（ハードコーディング)*/
		///mod 攻撃回数関連のnum,mul,div値をこのルーチン内のハードコーディングでなくclass_info[]内に記述することにした
		if (o_ptr->k_idx && !p_ptr->heavy_wield[i])
		{
			int str_index, dex_index;

			int num = 0, wgt = 0, mul = 0, div = 0;

			num = cp_ptr->weapon_num;
			mul = cp_ptr->weapon_mul;
			wgt = cp_ptr->weapon_div;

			/*:::騎兵が騎乗に適した装備をしている時の特殊処理*/
			if(p_ptr->pclass == CLASS_CAVALRY && (p_ptr->riding) && (have_flag(flgs, TR_RIDING)))
			{
				num = 5; wgt = 70; mul = 4;
			}

			///mod140315 武器を両手持ちしている時攻撃回数上限が1増えるようにしてみる
			if(p_ptr->ryoute) num++;

			//v1.1.60 赤蛮奇専用職業は攻撃回数5→3
			if (is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
				num -= 2;

///del140202 ここに職業ごとmul値とかがハードコーディングされていたがplayer_classの値を参照することにして消した

			/* Hex - extra mights gives +1 bonus to max blows */
		//	if (hex_spelling(HEX_XTRA_MIGHT) || hex_spelling(HEX_BUILDING)) { num++; wgt /= 2; mul += 2; }

			/* Enforce a minimum "weight" (tenth pounds) */
			/*:::wgtか武器重量の大きいほうが計算に使われる　（wgtが小さいほうが軽い武器を装備したとき攻撃回数が増えやすい）*/
			div = ((o_ptr->weight < wgt) ? wgt : o_ptr->weight);

			/* Access the strength vs weight */
			/*:::mulは大きいほうが攻撃回数が増えやすい*/
			str_index = (adj_str_blow[p_ptr->stat_ind[A_STR]] * mul / div);

			//if (p_ptr->ryoute && !omoi) str_index++;
			if (p_ptr->ryoute && !omoi) str_index+=2;
			if (p_ptr->pclass == CLASS_NINJA) str_index = MAX(0, str_index-1);

			/* Maximal value */
			if (str_index > 11) str_index = 11;

			/* Index by dexterity */
			dex_index = (adj_dex_blow[p_ptr->stat_ind[A_DEX]]);

			/* Maximal value */
			if (dex_index > 11) dex_index = 11;

			//			msg_format("chk1:str:%d dex:%d", str_index,dex_index);
			//			msg_format("chk1:base_blow:%d", p_ptr->num_blow[i]);

			/* Use the blows table */
			p_ptr->num_blow[i] = blows_table[str_index][dex_index];
			/* Maximal value */
			if (p_ptr->num_blow[i] > num) p_ptr->num_blow[i] = num;

			/* Add in the "bonus blows" */
			p_ptr->num_blow[i] += extra_blows[i];

			///mod140814 短剣装備で熟練度高いと攻撃回数増加
			if(o_ptr->tval == TV_KNIFE)
			{
				if(p_ptr->migite && p_ptr->hidarite && (p_ptr->pclass != CLASS_3_FAIRIES))
				{
					if(ref_skill_exp(TV_KNIFE) >= 5400) p_ptr->num_blow[i]++;
				}
				else
				{
					if(ref_skill_exp(TV_KNIFE) >= 3600) p_ptr->num_blow[i]++;
					if(ref_skill_exp(TV_KNIFE) >= 7200) p_ptr->num_blow[i]++;
				}
			}
			///class 職業ごと攻撃回数特殊処理
			if (p_ptr->pclass == CLASS_WARRIOR) p_ptr->num_blow[i] += (p_ptr->lev / 40);
			else if (p_ptr->pclass == CLASS_MOMOYO) p_ptr->num_blow[i] += (p_ptr->lev / 24);
			//else if ((p_ptr->pclass == CLASS_ROGUE) && (o_ptr->weight < 50) && (p_ptr->stat_ind[A_DEX] >= 30)) p_ptr->num_blow[i] ++;
			else if ((p_ptr->pclass == CLASS_ROGUE) && (o_ptr->tval == TV_KNIFE)) p_ptr->num_blow[i] ++;

			if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
				 p_ptr->num_blow[i] += (p_ptr->lev / 30);

			if( p_ptr->pseikaku == SEIKAKU_BERSERK)
				p_ptr->num_blow[i] += (p_ptr->lev / 23);

			///mod140813 光速移動中攻撃回数増加してみる
			if(p_ptr->lightspeed) p_ptr->num_blow[i] += 2;
			//v1.1.17
			if( check_activated_nameless_arts(JKF1_MASTER_MELEE)) p_ptr->num_blow[i] += 1 + plev / 24;

			if (p_ptr->special_defense & KATA_FUUJIN) p_ptr->num_blow[i] -= 1;

			if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) p_ptr->num_blow[i] = 1;

			//キスメの飛び上がり中は攻撃回数強制1
			if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent) p_ptr->num_blow[i] = 1;

			/* Require at least one blow */
			if (p_ptr->num_blow[i] < 1) p_ptr->num_blow[i] = 1;

			/* Boost digging skill by weapon weight */
			p_ptr->skill_dig += (o_ptr->weight / 10);
		}

		/* Assume okay */
		/* Priest weapon penalty for non-blessed edged weapons */
		/*:::仙術領域プリは祝福か鈍器、妖術領域プリは祝福されていない武器でないとペナルティということにする*/
		if ((p_ptr->pclass == CLASS_PRIEST)
			&& ((is_good_realm(p_ptr->realm1) && !(have_flag(flgs, TR_BLESSED)) && (o_ptr->tval != TV_HAMMER) && (o_ptr->tval != TV_STICK))
			|| (!is_good_realm(p_ptr->realm1) && have_flag(flgs, TR_BLESSED)) ))
		{
			/* Reduce the real bonuses */
			p_ptr->to_h[i] -= 2;
			p_ptr->to_d[i] -= 2;

			/* Reduce the mental bonuses */
			p_ptr->dis_to_h[i] -= 2;
			p_ptr->dis_to_d[i] -= 2;

			/* Icky weapon */
			p_ptr->icky_wield[i] = TRUE;
		}
		//守護者は祝福された武器にボーナス、呪われた武器にペナルティ
		else if (p_ptr->pclass == CLASS_PALADIN)
		{
			if(have_flag(flgs, TR_BLESSED)) p_ptr->to_dd[i] += 2;
			else if(object_is_cursed(o_ptr))
			{
				p_ptr->to_h[i] -= 10;
				p_ptr->to_d[i] -= 10;
				p_ptr->dis_to_h[i] -= 10;
				p_ptr->dis_to_d[i] -= 10;
			}

		}
		//スペマスのペナルティ
		else if (p_ptr->pclass == CLASS_SORCERER)
		{
			if (!((o_ptr->tval == TV_STICK) && ((o_ptr->sval == SV_WEAPON_WIZSTAFF) )))
			{
				/* Reduce the real bonuses */
				p_ptr->to_h[i] -= 200;
				p_ptr->to_d[i] -= 200;

				/* Reduce the mental bonuses */
				p_ptr->dis_to_h[i] -= 200;
				p_ptr->dis_to_d[i] -= 200;

				/* Icky weapon */
				p_ptr->icky_wield[i] = TRUE;
			}
			else
			{
				/* Reduce the real bonuses */
				p_ptr->to_h[i] -= 30;
				p_ptr->to_d[i] -= 10;

				/* Reduce the mental bonuses */
				p_ptr->dis_to_h[i] -= 30;
				p_ptr->dis_to_d[i] -= 10;
			}
		}
		//v1.1.21 兵士短剣技能
		else if(p_ptr->pclass == CLASS_SOLDIER)
		{
			if ((o_ptr->tval == TV_KNIFE) && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_KNIFE_MASTERY))
			{
				p_ptr->to_dd[i] += 2;

			}

		}
		//v1.1.60 針妙丸は針を装備するとボーナスを得ることにする
		else if (p_ptr->pclass == CLASS_SHINMYOUMARU || p_ptr->pclass == CLASS_SHINMYOU_2)
		{
			if (o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE)
			{
				p_ptr->num_blow[i]++;
				p_ptr->to_dd[i] += 2;
				p_ptr->to_h[i] += plev;
				p_ptr->dis_to_h[i] += plev;

			}

		}
		//v1.1.90 山如は扇を装備するとボーナス
		else if (p_ptr->pclass == CLASS_SANNYO)
		{
			if (o_ptr->tval == TV_MAGICWEAPON && o_ptr->sval == SV_MAGICWEAPON_FAN)
			{
				p_ptr->to_h[i] += plev;
				p_ptr->dis_to_h[i] += plev;

			}

		}


		/* Hex bonuses */
		/*
		if (p_ptr->realm1 == REALM_HEX)
		{
			if (object_is_cursed(o_ptr))
			{
				if (o_ptr->curse_flags & (TRC_CURSED)) { p_ptr->to_h[i] += 5; p_ptr->dis_to_h[i] += 5; }
				if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { p_ptr->to_h[i] += 7; p_ptr->dis_to_h[i] += 7; }
				if (o_ptr->curse_flags & (TRC_PERMA_CURSE)) { p_ptr->to_h[i] += 13; p_ptr->dis_to_h[i] += 13; }
				if (o_ptr->curse_flags & (TRC_TY_CURSE)) { p_ptr->to_h[i] += 5; p_ptr->dis_to_h[i] += 5; }
				if (hex_spelling(HEX_RUNESWORD))
				{
					if (o_ptr->curse_flags & (TRC_CURSED)) { p_ptr->to_d[i] += 5; p_ptr->dis_to_d[i] += 5; }
					if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { p_ptr->to_d[i] += 7; p_ptr->dis_to_d[i] += 7; }
					if (o_ptr->curse_flags & (TRC_PERMA_CURSE)) { p_ptr->to_d[i] += 13; p_ptr->dis_to_d[i] += 13; }
				}
			}
		}
		*/



		if ( p_ptr->pseikaku == SEIKAKU_BERSERK)
		{
			p_ptr->to_h[i] += p_ptr->lev/5;
			p_ptr->to_d[i] += p_ptr->lev/6;
			p_ptr->dis_to_h[i] += p_ptr->lev/5;
			p_ptr->dis_to_d[i] += p_ptr->lev/6;
			if (((i == 0) && !p_ptr->hidarite) || p_ptr->ryoute)
			{
				p_ptr->to_h[i] += p_ptr->lev/5;
				p_ptr->to_d[i] += p_ptr->lev/6;
				p_ptr->dis_to_h[i] += p_ptr->lev/5;
				p_ptr->dis_to_d[i] += p_ptr->lev/6;
			}
		}


		///item 乗馬武器
		if (p_ptr->riding)
		{
			//v1.1.41舞と里乃の乗馬中攻撃(モンスターの後ろで踊りながら攻撃する感じ)はペナルティ激重
			if (CLASS_RIDING_BACKDANCE)
			{
				int penalty = 120 - plev;

				if (have_flag(flgs, TR_RIDING))
					penalty /= 2;
				else
					p_ptr->riding_wield[i] = TRUE;

				p_ptr->to_h[i] -= penalty;
				p_ptr->dis_to_h[i] -= penalty;
			}
			else if ((o_ptr->tval == TV_SPEAR) && ((o_ptr->sval == SV_WEAPON_LANCE) || (o_ptr->sval == SV_WEAPON_HEAVY_LANCE)))
			{
				p_ptr->to_h[i] +=15;
				p_ptr->dis_to_h[i] +=15;
				p_ptr->to_dd[i] += 2;
			}
			/*:::乗馬中だが乗馬に適さない武器を持っている場合*/
			else if (!(have_flag(flgs, TR_RIDING)))
			{
				///class 乗馬不適武器ペナルティ
				int penalty;
				int penalty_min;
				///mod140928 騎乗ペナルティ計算少し変更　乗馬適正を考慮
				penalty = r_info[m_list[p_ptr->riding].r_idx].level - ref_skill_exp(SKILL_RIDING) / 80;
				penalty_min = 60 - cp_ptr->skill_aptitude[SKILL_RIDING] * 10;


				if(penalty < penalty_min) penalty = penalty_min;

				if(p_ptr->pclass == CLASS_CAVALRY || p_ptr->pclass == CLASS_MAYUMI) penalty = MAX(0,20 - p_ptr->lev / 2);


/*
				if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
				{
					penalty = 5;
				}
				else
				{
					///mod131226 skill 技能と武器技能の統合
					//penalty = r_info[m_list[p_ptr->riding].r_idx].level - p_ptr->skill_exp[GINOU_RIDING] / 80;
					penalty = r_info[m_list[p_ptr->riding].r_idx].level - ref_skill_exp(SKILL_RIDING) / 80;
					penalty += 30;
					if (penalty < 30) penalty = 30;
				}
*/
				p_ptr->to_h[i] -= penalty;
				p_ptr->dis_to_h[i] -= penalty;

				/* Riding weapon */
				p_ptr->riding_wield[i] = TRUE;
			}
		}

		if(p_ptr->special_attack & ATTACK_INC_DICES)
		{
				p_ptr->to_ds[i] += 1 + p_ptr->lev / 24;
		}
		//巫女と神官は大幣にダイスボーナス
		if((p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_SANAE) && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_OONUSA)
		{
				p_ptr->to_dd[i] += 3;
				p_ptr->to_ds[i] += 3;

				//v1.1.65 霊夢は光源のないときには大幣に火をつけて光源にできることにした。ひょっとしたらアレは大幣ではないのかもしれんが
				if (p_ptr->pclass == CLASS_REIMU)
					p_ptr->lite = TRUE;

		}
		//剣術家は刀にダイスボーナス
		if(p_ptr->pclass == CLASS_SAMURAI && o_ptr->tval == TV_KATANA)
		{
			if(p_ptr->lev > 24) p_ptr->to_dd[i]++;
			if(p_ptr->lev > 44) p_ptr->to_dd[i]++;
		}

		//v1.1.89 百々世はつるはしにダイスボーナス
		if (p_ptr->pclass == CLASS_MOMOYO && o_ptr->tval == TV_AXE && o_ptr->sval == SV_WEAPON_PICK)
		{
			p_ptr->to_dd[i] += 1 + plev / 24;
		}


		//v1.1.34 空海の錫杖と笠を装備しているとパワーアップ
		if(o_ptr->name1 == ART_KUKAI && check_equip_specific_fixed_art(ART_KUKAI_KASA,TRUE))
		{
				p_ptr->to_dd[i] += 3;
				p_ptr->to_ds[i] += 3;
				p_ptr->easy_spell = TRUE;
		}
		//一部の妖精は花にダイスボーナス
		if(CHECK_FAIRY_TYPE == 8 && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_FLOWER)
		{
				p_ptr->to_ds[i] += 3;
		}
		//死神は鎌にダイスボーナス
		if(p_ptr->prace == RACE_DEATH && o_ptr->tval == TV_POLEARM && (o_ptr->sval == SV_WEAPON_SCYTHE_OF_SLICING || o_ptr->sval == SV_WEAPON_WAR_SCYTHE))
		{
				p_ptr->to_ds[i] += 2;
		}
		//穣子も鎌にダイスボーナス
		if(p_ptr->pclass == CLASS_MINORIKO && o_ptr->tval == TV_POLEARM && (o_ptr->sval == SV_WEAPON_SCYTHE_OF_SLICING || o_ptr->sval == SV_WEAPON_WAR_SCYTHE))
		{
				p_ptr->num_blow[i] += 1;
				p_ptr->to_h[i] +=10;
				p_ptr->dis_to_h[i] +=10;
				p_ptr->to_ds[i] += 1;

		}
		/*:::チキンナイフ特殊処理 クエスト放棄数ダイス加算*/
		if(o_ptr->name1 == ART_CHICKEN_KNIFE)
		{
			int cnt_tmp;
			int add_count = 0;
			for (cnt_tmp = 0; cnt_tmp < max_quests; cnt_tmp++)
			{
				//v1.1.80 受領不可クエスト(レベル0失敗扱い)はカウントしないことにする
				if (!quest[cnt_tmp].complev) continue;

				if (quest[cnt_tmp].status == QUEST_STATUS_FAILED_DONE || quest[cnt_tmp].status == QUEST_STATUS_FAILED)	add_count++;

				//v1.1.83 ダイス増加上限を+49(50d3)に制限
				if (add_count >= 49) break;

			}
			p_ptr->to_dd[i] += add_count;

		}
		//巨大化中は武器も大きくなってダイス増加
		if(p_ptr->mimic_form == MIMIC_GIGANTIC)
		{
				p_ptr->to_dd[i] += 2;
		}

		/*:::キスメ特殊処理*/
		if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent)
		{
			p_ptr->to_dd[i] += (p_ptr->lev / 20 + o_ptr->dd/2 + p_ptr->concent) * (p_ptr->concent);
		}

	}/*:::右手/左手装備ループ完了*/



	if (p_ptr->riding)
	{
		int penalty = 0;
		int penalty_min;

		p_ptr->riding_ryoute = FALSE;


		if (p_ptr->ryoute || (empty_hands(FALSE) == EMPTY_HAND_NONE)) p_ptr->riding_ryoute = TRUE;
		/*:::修行僧、錬気術師、狂戦士の乗馬両手処理*/
		///class
		else if (p_ptr->pet_extra_flags & PF_RYOUTE)
		{
			if ((empty_hands(FALSE) != EMPTY_HAND_NONE) && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			p_ptr->riding_ryoute = TRUE;
			/*
			switch (p_ptr->pclass)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if ((empty_hands(FALSE) != EMPTY_HAND_NONE) && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
					p_ptr->riding_ryoute = TRUE;
				break;
			}
			*/
		}
		/*:::Mega Hack - 乗馬技能が7200あると手綱を離しても馬を操れるようにする*/
		if(ref_skill_exp(SKILL_RIDING) >= RIDING_PERFECT) p_ptr->riding_ryoute = FALSE;

		///mod140928 騎乗ペナルティ計算少し変更　乗馬適正を考慮 クロスボウは少し撃ちにくい
		penalty = r_info[m_list[p_ptr->riding].r_idx].level - ref_skill_exp(SKILL_RIDING) / 80;
		penalty_min = 60 - cp_ptr->skill_aptitude[SKILL_RIDING] * 10;
		if(penalty < penalty_min) penalty = penalty_min;
		if (p_ptr->tval_ammo == TV_BOLT) penalty += 10;
		if(p_ptr->pclass == CLASS_CAVALRY || p_ptr->pclass == CLASS_MAYUMI) penalty = MAX(0,20 - p_ptr->lev / 2);

#if 0
		/*:::魔獣使いと騎兵が弓を装備したとき以外は乗馬時飛び道具のペナルティが大きい*/
		if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
		{
			if (p_ptr->tval_ammo != TV_ARROW) penalty = 5;
		}
		else
		{
			penalty = r_info[m_list[p_ptr->riding].r_idx].level - ref_skill_exp(SKILL_RIDING) / 80;
			///mod131226 skill 技能と武器技能の統合
			//penalty = r_info[m_list[p_ptr->riding].r_idx].level - p_ptr->skill_exp[GINOU_RIDING] / 80;
			penalty += 30;
			if (penalty < 30) penalty = 30;
		}
		if (p_ptr->tval_ammo == TV_BOLT) penalty *= 2;
#endif
		p_ptr->to_h_b -= penalty;
		p_ptr->dis_to_h_b -= penalty;
	}


	///mod140216 職業にかかわらず素手で複数攻撃するようにした
	/*:::格闘攻撃回数など処理*/
	if (p_ptr->do_martialarts)
	{
		int tmp_blows;
		int tmp_bonus;
		int blow_num_base_max;

		/*:::基礎格闘攻撃回数*/
		if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			tmp_blows = (ref_skill_exp(SKILL_MARTIALARTS)+1000) / 2000 + p_ptr->lev / 15 + (p_ptr->stat_ind[A_DEX]+3) / 13 ;
		else
			tmp_blows = (ref_skill_exp(SKILL_MARTIALARTS)+1000) / 2000 + p_ptr->lev / 20 + (p_ptr->stat_ind[A_DEX]+3) / 16 ;

		if(p_ptr->prace == RACE_NINJA ) tmp_blows += 2;
		if(p_ptr->pclass == CLASS_NINJA ) tmp_blows -= 1;

		if( p_ptr->pseikaku == SEIKAKU_BERSERK)
				tmp_blows += (p_ptr->lev / 16);

		//武道の神様特殊処理
		if(p_ptr->prace == RACE_STRAYGOD &&
			p_ptr->race_multipur_val[3] == 24 ||
			p_ptr->race_multipur_val[4] / 128 == 24 ||
			p_ptr->race_multipur_val[4] % 128 == 24	)
			tmp_blows += 2;

		/*:::何か(手綱含む　爪強化除く)を手に持っていると攻撃回数マイナス*/
		if(!(empty_hands_status & EMPTY_HAND_LARM) && !p_ptr->clawextend)
		{
			if(flag_gun_kata) tmp_blows -= 1;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST || p_ptr->pclass == CLASS_ICHIRIN) 	tmp_blows -= 3;
			else 	tmp_blows -= 2;
		}
		if(!(empty_hands_status & EMPTY_HAND_RARM) && !p_ptr->clawextend)
		{
			if(flag_gun_kata) tmp_blows -= 1;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST || p_ptr->pclass == CLASS_ICHIRIN) 	tmp_blows -= 3;
			else 	tmp_blows -= 2;
		}

		/*:::重い鎧は攻撃回数半減*/
		if(heavy_armor()) tmp_blows /= 2;
		//弁々ダブルスコア例外処理
		if(p_ptr->pclass == CLASS_BENBEN && music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE) && !p_ptr->mimic_form && tmp_blows < 4)
			tmp_blows = 4;

		//v1.1.37 小鈴変身中(手に持ってる巻物で攻撃回数減少するのも考慮)
		if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI)
			tmp_blows += 4;

		if(p_ptr->lightspeed) tmp_blows += 2;
		else if(p_ptr->clawextend) tmp_blows += 1;
		else if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->tim_general[0]) tmp_blows += 1;

		if( check_activated_nameless_arts(JKF1_MASTER_MELEE)) tmp_blows += 1 + plev / 24;

		if (SUPER_SHION) tmp_blows += 1 + plev / 16;
		//v1.1.58 三妖精専用性格
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))tmp_blows += plev / 20;

		/*:::最低1回は攻撃*/
		if(tmp_blows < 1) tmp_blows = 1;

		/*:::変身時は攻撃回数が特殊*/
		if(p_ptr->mimic_form == MIMIC_BEAST || p_ptr->mimic_form == MIMIC_CAT  || p_ptr->mimic_form == MIMIC_DRAGON)tmp_blows = 2 + ref_skill_exp(SKILL_MARTIALARTS) / 2000 + p_ptr->lev / 20 + (p_ptr->stat_ind[A_DEX]+3) / 16;
		if(p_ptr->mimic_form == MIMIC_SLIME)tmp_blows = 1 + plev / 15 + (p_ptr->stat_ind[A_DEX]+3) / 20;

		//二丁拳銃のとき銃威力の一部を格闘ダメージに加える
		if(flag_gun_kata && select_gun_melee_mode() == MELEE_MODE_GUN_TWOHAND)
		{
			tmp_bonus = 0;
			tmp_bonus += inventory[INVEN_RARM].to_d + inventory[INVEN_RARM].dd * (inventory[INVEN_RARM].ds+1) / 2;
			tmp_bonus += inventory[INVEN_LARM].to_d + inventory[INVEN_LARM].dd * (inventory[INVEN_LARM].ds+1) / 2;
			tmp_bonus /= 4;

			if(tmp_bonus > 100) tmp_bonus = 100;
			if(tmp_bonus < 0) tmp_bonus = 0;

			p_ptr->to_d[0] += tmp_bonus;
			p_ptr->to_d[1] += tmp_bonus;
			p_ptr->dis_to_d[0] += tmp_bonus;
			p_ptr->dis_to_d[1] += tmp_bonus;

			tmp_blows += 2;
		}

		/*:::最高攻撃回数は職業による格闘適性に依存 3～7*/
		/*:::ただし変身時は特殊*/
		blow_num_base_max = (cp_ptr->skill_aptitude[SKILL_MARTIALARTS] + 2);

		if(p_ptr->lightspeed) blow_num_base_max = 8;
		else if( check_activated_nameless_arts(JKF1_MASTER_MELEE))  blow_num_base_max = 7;
		else if(p_ptr->mimic_form == MIMIC_BEAST) blow_num_base_max = 7;
		else if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI) blow_num_base_max = 5;
		else if(p_ptr->mimic_form == MIMIC_CAT) blow_num_base_max = 5;
		else if(p_ptr->mimic_form == MIMIC_DRAGON) blow_num_base_max = 6;
		else if(p_ptr->mimic_form == MIMIC_SLIME) blow_num_base_max = 4;
		else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
		{
			blow_num_base_max += 1;
			if(p_ptr->tim_general[0]) blow_num_base_max += 1;
		}

		if (p_ptr->prace == RACE_NINJA) blow_num_base_max += 1;

		if( p_ptr->pseikaku == SEIKAKU_BERSERK)
		{
			blow_num_base_max += 2;
			if(blow_num_base_max > 9) blow_num_base_max = 9;
		}

		if(tmp_blows > blow_num_base_max)	tmp_blows = blow_num_base_max;



		///mod150110 橙　濡れていない時ボーナス
		if(p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0]) tmp_blows += 1;
		//v1.1.89
		if (p_ptr->pclass == CLASS_MOMOYO) tmp_blows += plev / 24;


		/*:::修正値ボーナス　レベルと熟練度に依存*/
		tmp_bonus  = p_ptr->lev / 2 * ref_skill_exp(SKILL_MARTIALARTS) / 8000;
		if(p_ptr->mimic_form == MIMIC_BEAST) tmp_bonus  += 5 + p_ptr->lev / 2;
		if(p_ptr->mimic_form == MIMIC_DRAGON) tmp_bonus  += p_ptr->mimic_dragon_rank / 3;
		else if( p_ptr->clawextend || p_ptr->mimic_form == MIMIC_SLIME) tmp_bonus  +=  p_ptr->lev / 3;
		if(heavy_armor()) tmp_bonus /= 2;


		//v1.1.41舞と里乃の乗馬中攻撃(モンスターの後ろで踊りながら攻撃する感じ)はペナルティ激重
		if (CLASS_RIDING_BACKDANCE && p_ptr->riding)
		{
			int penalty = 120 - plev;
			p_ptr->riding_wield[0] = TRUE;
			p_ptr->riding_ryoute = TRUE;

			tmp_bonus -= penalty;
		}


		if(p_ptr->migite)
		{
			p_ptr->num_blow[0] = extra_blows[0] + tmp_blows;
			if(p_ptr->num_blow[0] < 1) p_ptr->num_blow[0] = 1;
			p_ptr->to_h[0] += tmp_bonus;
			p_ptr->dis_to_h[0] += tmp_bonus;
			p_ptr->to_d[0] += tmp_bonus / 2;
			p_ptr->dis_to_d[0] += tmp_bonus / 2;
		}
		//今のところ、クロスボウを持っている時しかここには来ないはず
		else if(p_ptr->hidarite)
		{
			p_ptr->num_blow[1] = extra_blows[1] + tmp_blows;
			if(p_ptr->num_blow[1] < 1) p_ptr->num_blow[1] = 1;
			p_ptr->to_h[1] += tmp_bonus;
			p_ptr->dis_to_h[1] += tmp_bonus;
			p_ptr->to_d[1] += tmp_bonus / 2;
			p_ptr->dis_to_d[1] += tmp_bonus / 2;
		}
		//キスメは常に1
		if(p_ptr->pclass == CLASS_KISUME)
		{
			if(p_ptr->migite) p_ptr->num_blow[0] = 1;
			else if(p_ptr->hidarite) p_ptr->num_blow[1] = 1;
		}
		//美鈴攻撃回数特殊処理
		if(p_ptr->pclass == CLASS_MEIRIN && p_ptr->num_blow[0] > 1)
		{
			int old_blow = p_ptr->num_blow[0];
			int new_blow = old_blow - old_blow / 2;

			p_ptr->num_blow[0] = new_blow;
			p_ptr->magic_num1[0] = 133 * new_blow / old_blow;
			if(p_ptr->concent)
			{
				int i;
				p_ptr->magic_num1[0] -= p_ptr->concent * 3;
				if(p_ptr->magic_num1[0] < 25) p_ptr->magic_num1[0] = 25;
			}

		}

	}

#if 0
	/* Different calculation for monks with empty hands */
	/*:::修行僧・錬気術師・狂戦士の素手攻撃回数計算*/
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) &&
		(empty_hands_status & EMPTY_HAND_RARM) && !p_ptr->hidarite)
	{
		int blow_base = p_ptr->lev + adj_dex_blow[p_ptr->stat_ind[A_DEX]];
		p_ptr->num_blow[0] = 0;

		if (p_ptr->pclass == CLASS_FORCETRAINER)
		{
			if (blow_base > 18) p_ptr->num_blow[0]++;
			if (blow_base > 31) p_ptr->num_blow[0]++;
			if (blow_base > 44) p_ptr->num_blow[0]++;
			if (blow_base > 58) p_ptr->num_blow[0]++;
			if (p_ptr->magic_num1[0])
			{
				p_ptr->to_d[0] += (p_ptr->magic_num1[0]/5);
				p_ptr->dis_to_d[0] += (p_ptr->magic_num1[0]/5);
			}
		}
		else
		{
			if (blow_base > 12) p_ptr->num_blow[0]++;
			if (blow_base > 22) p_ptr->num_blow[0]++;
			if (blow_base > 31) p_ptr->num_blow[0]++;
			if (blow_base > 39) p_ptr->num_blow[0]++;
			if (blow_base > 46) p_ptr->num_blow[0]++;
			if (blow_base > 53) p_ptr->num_blow[0]++;
			if (blow_base > 59) p_ptr->num_blow[0]++;
		}

		if (heavy_armor() && (p_ptr->pclass != CLASS_BERSERKER))
			p_ptr->num_blow[0] /= 2;
		else
		{
			p_ptr->to_h[0] += (p_ptr->lev / 3);
			p_ptr->dis_to_h[0] += (p_ptr->lev / 3);

			p_ptr->to_d[0] += (p_ptr->lev / 6);
			p_ptr->dis_to_d[0] += (p_ptr->lev / 6);
		}

		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->to_a -= 40;
			p_ptr->dis_to_a -= 40;

		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
			p_ptr->to_a -= 50;
			p_ptr->dis_to_a -= 50;
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->sh_fire = TRUE;
			p_ptr->sh_elec = TRUE;
			p_ptr->sh_cold = TRUE;
			p_ptr->levitation = TRUE;
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->dis_to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->reflect = TRUE;
			p_ptr->num_blow[0] -= 2;
			if ((p_ptr->pclass == CLASS_MONK) && (p_ptr->lev > 42)) p_ptr->num_blow[0]--;
			if (p_ptr->num_blow[0] < 0) p_ptr->num_blow[0] = 0;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->to_h[0] -= (p_ptr->lev / 3);
			p_ptr->to_d[0] -= (p_ptr->lev / 6);

			p_ptr->dis_to_h[0] -= (p_ptr->lev / 3);
			p_ptr->dis_to_d[0] -= (p_ptr->lev / 6);
			p_ptr->num_blow[0] /= 2;
			p_ptr->levitation = TRUE;
		}

		/*:::ここで基本攻撃回数1が加算されている*/
		p_ptr->num_blow[0] += 1+extra_blows[0];
	}
#endif

	//v1.1.41 舞と里乃のバックダンスは＠とモンスター両方が浮遊していないと浮遊状態にならない
	if (CLASS_RIDING_BACKDANCE)
	{
		if (p_ptr->riding) p_ptr->levitation = p_ptr->levitation & riding_levitation;
	}
	else
	{
		if (p_ptr->riding) p_ptr->levitation = riding_levitation;
	}

	/*
	monk_armour_aux = FALSE;

	if (martialarts && heavy_armor())
	{
		monk_armour_aux = TRUE;
	}
	*/

	/*:::武器熟練度による命中率補正*/
	///item 武器熟練度
	for (i = 0; i < 2; i++)
	{
		if (buki_motteruka(INVEN_RARM+i))
		{
			int tval = inventory[INVEN_RARM+i].tval - TV_WEAPON_BEGIN;
			int sval = inventory[INVEN_RARM+i].sval;
			///mod131227 skill 武器熟練度
			//p_ptr->to_h[i] += (p_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;
			//p_ptr->dis_to_h[i] += (p_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;
			p_ptr->to_h[i] += (ref_skill_exp(inventory[INVEN_RARM+i].tval) - WEAPON_EXP_BEGINNER) / 200;
			p_ptr->dis_to_h[i] += (ref_skill_exp(inventory[INVEN_RARM+i].tval) - WEAPON_EXP_BEGINNER) / 200;
			///sys item class 修行僧や忍者の「ふさわしくない装備」判定
			/*　一時無効化
			if ((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER))
			{
				if (!s_info[p_ptr->pclass].w_max[tval][sval])
				{
					p_ptr->to_h[i] -= 40;
					p_ptr->dis_to_h[i] -= 40;
					p_ptr->icky_wield[i] = TRUE;
				}
			}
			else if (p_ptr->pclass == CLASS_NINJA)
			{
				if ((s_info[CLASS_NINJA].w_max[tval][sval] <= WEAPON_EXP_BEGINNER) || (inventory[INVEN_LARM-i].tval == TV_SHIELD))
				{
					p_ptr->to_h[i] -= 40;
					p_ptr->dis_to_h[i] -= 40;
					p_ptr->icky_wield[i] = TRUE;
					p_ptr->num_blow[i] /= 2;
					if (p_ptr->num_blow[i] < 1) p_ptr->num_blow[i] = 1;
				}
			}
			*/
			///del item 神罰の鉄球を装備していると悪になるらしい とりあえず削除
			//if (inventory[INVEN_RARM + i].name1 == ART_IRON_BALL) p_ptr->align -= 1000;
		}
	}

	/* Maximum speed is (+99). (internally it's 110 + 99) */
	/* Temporary lightspeed forces to be maximum speed */
	if ((p_ptr->lightspeed && !p_ptr->riding && !CLASS_RIDING_BACKDANCE) || (new_speed > 209))
	{
		new_speed = 209;
	}

	/* Minimum speed is (-99). (internally it's 110 - 99) */
	if (new_speed < 11) new_speed = 11;

	/* Display the speed (if needed) */
	if (p_ptr->pspeed != (byte)new_speed)
	{
		p_ptr->pspeed = (byte)new_speed;
		p_ptr->redraw |= (PR_SPEED);
	}
/*
	if (yoiyami)
	{
		if (p_ptr->to_a > (0 - p_ptr->ac))
			p_ptr->to_a = 0 - p_ptr->ac;
		if (p_ptr->dis_to_a > (0 - p_ptr->dis_ac))
			p_ptr->dis_to_a = 0 - p_ptr->dis_ac;
	}
*/
	/* Redraw armor (if needed) */
	if ((p_ptr->dis_ac != old_dis_ac) || (p_ptr->dis_to_a != old_dis_to_a))
	{
		/* Redraw */
		p_ptr->redraw |= (PR_ARMOR);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}

	/*:::両手装備時の命中、ダメージボーナス　ただしあまりに武器が重い時は適用されない*/
	if (p_ptr->ryoute && !omoi)
	{
		int bonus_to_h=0, bonus_to_d=0;
		bonus_to_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128)/2;
		bonus_to_h = ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128) + ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);

		p_ptr->to_h[default_hand] += MAX(bonus_to_h,1);
		p_ptr->dis_to_h[default_hand] += MAX(bonus_to_h,1);
		p_ptr->to_d[default_hand] += MAX(bonus_to_d,1);
		p_ptr->dis_to_d[default_hand] += MAX(bonus_to_d,1);
	}


	/*:::これまで攻撃関係処理に使っていたryoteをFAUSEに戻しているらしい*/
	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))) p_ptr->ryoute = FALSE;
	if (p_ptr->do_martialarts) p_ptr->ryoute = FALSE;

	/* Affect Skill -- stealth (bonus one) */
	/*:::スキル計算*/
	p_ptr->skill_stl += 1;
	/*:::一時的隠密増加　隠遁の歌のみ？*/
	if (IS_TIM_STEALTH()) p_ptr->skill_stl += 99;
	if(p_ptr->tim_unite_darkness) p_ptr->skill_stl += 10;
	/* Affect Skill -- disarming (DEX and INT) */
	p_ptr->skill_dis += adj_dex_dis[p_ptr->stat_ind[A_DEX]];
	p_ptr->skill_dis += adj_int_dis[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- magic devices (INT) */
	p_ptr->skill_dev += adj_int_dev[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- saving throw (WIS) */
	p_ptr->skill_sav += adj_wis_sav[p_ptr->stat_ind[A_WIS]];

	/* Affect Skill -- digging (STR) */
	p_ptr->skill_dig += adj_str_dig[p_ptr->stat_ind[A_STR]];

	/* Affect Skill -- disarming (Level, by Class) */
	p_ptr->skill_dis += ((cp_ptr->x_dis * p_ptr->lev / 10) + (ap_ptr->a_dis * p_ptr->lev / 50));

	/* Affect Skill -- magic devices (Level, by Class) */
	p_ptr->skill_dev += ((cp_ptr->x_dev * p_ptr->lev / 10) + (ap_ptr->a_dev * p_ptr->lev / 50));

	/* Affect Skill -- saving throw (Level, by Class) */
	p_ptr->skill_sav += ((cp_ptr->x_sav * p_ptr->lev / 10) + (ap_ptr->a_sav * p_ptr->lev / 50));

	/* Affect Skill -- stealth (Level, by Class) */
	p_ptr->skill_stl += (cp_ptr->x_stl * p_ptr->lev / 10);

	/* Affect Skill -- search ability (Level, by Class) */
	p_ptr->skill_srh += (cp_ptr->x_srh * p_ptr->lev / 10);
	///mod140421
	p_ptr->skill_srh += adj_general[p_ptr->stat_ind[A_DEX]] + adj_general[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- search frequency (Level, by Class) */
	p_ptr->skill_fos += (cp_ptr->x_fos * p_ptr->lev / 10);

	/* Affect Skill -- combat (normal) (Level, by Class) */
	p_ptr->skill_thn += ((cp_ptr->x_thn * p_ptr->lev / 10) + (ap_ptr->a_thn * p_ptr->lev / 50));

	/*:::獣変身時に打撃能力にボーナス*/
	if(p_ptr->mimic_form == MIMIC_BEAST || p_ptr->mimic_form == MIMIC_DRAGON) p_ptr->skill_thn +=p_ptr->lev * 2;

	/* Affect Skill -- combat (shooting) (Level, by Class) */
	p_ptr->skill_thb += ((cp_ptr->x_thb * p_ptr->lev / 10) + (ap_ptr->a_thb * p_ptr->lev / 50));

	/* Affect Skill -- combat (throwing) (Level, by Class) */
	p_ptr->skill_tht += ((cp_ptr->x_thb * p_ptr->lev / 10) + (ap_ptr->a_thb * p_ptr->lev / 50));

	//霊夢特殊処理(技能)
	if(p_ptr->pclass == CLASS_REIMU)
	{
		int rank = osaisen_rank();
		int lv = p_ptr->lev;

		p_ptr->skill_dis += rank * 3 + rank * lv / 10;
		p_ptr->skill_dev += rank * 3 + rank * lv / 10;
		p_ptr->skill_sav += rank * 3 + rank * lv / 10;
		p_ptr->skill_srh += rank * 2 + rank * lv / 20;
		p_ptr->skill_fos += rank * 2 + rank * lv / 20;
		p_ptr->skill_stl += rank / 2;
		p_ptr->skill_thn += rank * 5 + rank * lv / 5;
		p_ptr->skill_thb += rank * 5 + rank * lv / 5;
		p_ptr->skill_tht += rank * 5 + rank * lv / 5;

	}
	else if(p_ptr->pclass == CLASS_KANAKO)
	{
		int rank = kanako_rank();
		int lv = p_ptr->lev;

		p_ptr->skill_dis += rank * 2 + rank * lv / 10;
		p_ptr->skill_dev += rank * 2 + rank * lv / 10;
		p_ptr->skill_sav += rank * 2 + rank * lv / 10;
		p_ptr->skill_srh += rank * 2 + rank * lv / 25;
		p_ptr->skill_fos += rank * 2 + rank * lv / 25;
		p_ptr->skill_stl += rank / 3;
		p_ptr->skill_thn += rank * 5 + rank * lv / 5;
		p_ptr->skill_thb += rank * 5 + rank * lv / 5;
		p_ptr->skill_tht += rank * 5 + rank * lv / 5;

	}

	///mod150110 橙が濡れたらパラメータ弱体化
	else if(p_ptr->pclass == CLASS_CHEN && p_ptr->magic_num1[0])
	{
		p_ptr->skill_dis -= cp_ptr->c_dis / 3 + (cp_ptr->x_dis * p_ptr->lev / 30);
		p_ptr->skill_dev -= cp_ptr->c_dev / 3 + (cp_ptr->x_dev * p_ptr->lev / 30);
		p_ptr->skill_sav -= cp_ptr->c_sav / 3 + (cp_ptr->x_sav * p_ptr->lev / 30);
		p_ptr->skill_stl -= cp_ptr->c_stl / 3 + (cp_ptr->x_stl * p_ptr->lev / 30);
		p_ptr->skill_srh -= cp_ptr->c_srh / 3 + (cp_ptr->x_srh * p_ptr->lev / 30);
		p_ptr->skill_fos -= cp_ptr->c_fos / 3 + (cp_ptr->x_fos * p_ptr->lev / 30);
		p_ptr->skill_thn -= cp_ptr->c_thn / 3 + (cp_ptr->x_thn * p_ptr->lev / 30);
		p_ptr->skill_thb -= cp_ptr->c_thb / 3 + (cp_ptr->x_thb * p_ptr->lev / 30);
		p_ptr->skill_tht -= cp_ptr->c_thb / 3 + (cp_ptr->x_thb * p_ptr->lev / 30);

	}



	/*:::反感装備無効処理　影フェアリーから付喪神へ*/
	if ((prace_is_(RACE_TSUKUMO)) && (p_ptr->pseikaku != SEIKAKU_SEXY) && !(p_ptr->muta3 & MUT3_BYDO)
		&& (p_ptr->cursed & TRC_AGGRAVATE))
	{
		p_ptr->cursed &= ~(TRC_AGGRAVATE);
		p_ptr->skill_stl = MIN(p_ptr->skill_stl - 3, (p_ptr->skill_stl + 2) / 2);
	}

	/* Limit Skill -- stealth from 0 to 30 */
	/*:::隠密は0～30*/
	if (p_ptr->skill_stl > 30) p_ptr->skill_stl = 30;
	if (p_ptr->skill_stl < 0) p_ptr->skill_stl = 0;

	/* Limit Skill -- digging from 1 up */
	if (p_ptr->skill_dig < 1) p_ptr->skill_dig = 1;


	if (p_ptr->anti_magic && (p_ptr->skill_sav < (90 + p_ptr->lev))) p_ptr->skill_sav = 90 + p_ptr->lev;

	//if (p_ptr->tsubureru) p_ptr->skill_sav = 10;

	if ((p_ptr->ult_res || p_ptr->resist_magic || p_ptr->magicdef) && (p_ptr->skill_sav < (95 + p_ptr->lev))) p_ptr->skill_sav = 95 + p_ptr->lev;

	//メルラン特殊処理
	if (p_ptr->pclass == CLASS_MERLIN && p_ptr->shield) p_ptr->skill_sav /= 3;

	else if (down_saving) p_ptr->skill_sav /= 2;

	//if(p_ptr->pclass == CLASS_HINA) p_ptr->skill_sav = 0;


	/* Hack -- Each elemental immunity includes resistance */
	if (p_ptr->immune_acid) p_ptr->resist_acid = TRUE;
	if (p_ptr->immune_elec) p_ptr->resist_elec = TRUE;
	if (p_ptr->immune_fire) p_ptr->resist_fire = TRUE;
	if (p_ptr->immune_cold) p_ptr->resist_cold = TRUE;


	//v1.1.24 急流下りクエスト中は浮遊禁止、反テレポ
	if(QRKDR)
	{
		p_ptr->levitation = FALSE;
		p_ptr->anti_tele = TRUE;
	}

	//v1.1.27 半魔法状態だと結界ガード解除
	if(p_ptr->anti_magic)
	{
		set_mana_shield(FALSE,FALSE);
	}

	//v1.1.24 溺れフラグ計算の場所変更
	{
		cave_type *c_ptr = &cave[py][px];
		feature_type *f_ptr = &f_info[c_ptr->feat];
		if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) )
		{
			if((prace_is_(RACE_VAMPIRE) ||  !p_ptr->resist_water && p_ptr->total_weight > weight_limit()) && !p_ptr->levitation && !p_ptr->tim_levitation && !p_ptr->can_swim)
			{
				p_ptr->drowning = TRUE;
				if(prace_is_(RACE_VAMPIRE))
				{
					p_ptr->to_a -= 50;
					p_ptr->dis_to_a -= 50;
				}
				else
				{
					p_ptr->to_a -= 20;
					p_ptr->dis_to_a -= 20;
				}

			}
		}
	}




	/*:::一部の徳を計算*/
	/* Determine player alignment */
	///del131224 virtue
#if 0
	for (i = 0, j = 0; i < 8; i++)
	{
		switch (p_ptr->vir_types[i])
		{
		case V_JUSTICE:
			p_ptr->align += p_ptr->virtues[i] * 2;
			break;
		case V_CHANCE:
			/* Do nothing */
			break;
		case V_NATURE:
		case V_HARMONY:
			neutral[j++] = i;
			break;
		case V_UNLIFE:
			p_ptr->align -= p_ptr->virtues[i];
			break;
		default:
			p_ptr->align += p_ptr->virtues[i];
			break;
		}
	}
	for (i = 0; i < j; i++)
	{
		if (p_ptr->align > 0)
		{
			p_ptr->align -= p_ptr->virtues[neutral[i]] / 2;
			if (p_ptr->align < 0) p_ptr->align = 0;
		}
		else if (p_ptr->align < 0)
		{
			p_ptr->align += p_ptr->virtues[neutral[i]] / 2;
			if (p_ptr->align > 0) p_ptr->align = 0;
		}
	}
#endif

	/* Hack -- handle "xtra" mode */
	///キャラメイクや武器比較などメッセージが不要なときここで終了
	if (character_xtra) return;

	///mod140315 グローブが合わないとき、MPが減少するのでなく失敗率が上昇するように変更
	///mod151205 三月精は不向きなグローブを3双装備しないとグローブによる魔法制限がかからない
	///mod160503 このルーチンをcalc_mana()からここに持ってきた。銃の失敗率上昇メッセージに使うため。
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		u32b tmp_flgs[TR_FLAG_SIZE];
		int cnt = 0;

		p_ptr->cumber_glove = FALSE;
		for(i=INVEN_RIGHT;i<=INVEN_FEET;i++)
		{
			o_ptr = &inventory[i];
			if(o_ptr->tval != TV_GLOVES) continue;
			object_flags(o_ptr, tmp_flgs);
			if (o_ptr->k_idx && !(have_flag(tmp_flgs, TR_FREE_ACT)) &&
				!(have_flag(tmp_flgs, TR_DEC_MANA)) && !(have_flag(tmp_flgs, TR_EASY_SPELL)) &&
				!((have_flag(tmp_flgs, TR_MAGIC_MASTERY)) && (o_ptr->pval > 0)) &&
			    !((have_flag(tmp_flgs, TR_DEX)) && (o_ptr->pval > 0)))
			{
				cnt++;
			}
		}
		if(cnt>2) p_ptr->cumber_glove = TRUE;

		//v1.1.68 専用性格のときペナルティなし
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			p_ptr->cumber_glove = FALSE;
	}
	else
	{
		u32b tmp_flgs[TR_FLAG_SIZE];

		p_ptr->cumber_glove = FALSE;
		o_ptr = &inventory[INVEN_HANDS];
		object_flags(o_ptr, tmp_flgs);

		/* Normal gloves hurt mage-type spells */
		if (o_ptr->k_idx &&
		    !(have_flag(tmp_flgs, TR_FREE_ACT)) &&
			!(have_flag(tmp_flgs, TR_DEC_MANA)) &&
			!(have_flag(tmp_flgs, TR_EASY_SPELL)) &&
			!((have_flag(tmp_flgs, TR_MAGIC_MASTERY)) && (o_ptr->pval > 0)) &&
		    !((have_flag(tmp_flgs, TR_DEX)) && (o_ptr->pval > 0)))
		{
			p_ptr->cumber_glove = TRUE;
		}
	}

	if (p_ptr->old_cumber_glove != p_ptr->cumber_glove)
	{
		if(inventory[INVEN_RARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_GUN)
		{
			if (p_ptr->cumber_glove)
				msg_print(_("手が覆われて銃を扱いにくい。", "You have trouble handling your gun with your covered hands."));
			else
				msg_print(_("手が自由になり銃を扱いやすくなった。", "You have easier time handling your gun now that your hands can move freely."));
		}
		if ( cp_ptr->realm_aptitude[0])
		{
			if (p_ptr->cumber_glove)
				msg_print(_("手が覆われて魔法を使いにくい。", "You have trouble casting spells with your covered hands."));
			else
				msg_print(_("手が自由になり魔法を使いやすくなった。", "You have easier time casting spells now that your hands can move freely."));
		}
		p_ptr->old_cumber_glove = p_ptr->cumber_glove;
	}

	///mod140705 騎乗時に落馬確率を表示するメッセージ rakuba()を変更するとここも変える必要がある
	if(p_ptr->riding && !CLASS_RIDING_BACKDANCE)
	{
		byte new_rakuba_type;

		int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
		int ridingskill = ref_skill_exp(SKILL_RIDING);

		if(p_ptr->riding_ryoute) ridinglevel += 20;

		///mod140705 乗馬スキルが極めて高いと落馬回避判定にボーナス　騎兵とかでレベルの上がったパワーDに乗るための修正
		//if(ridingskill > 6400) ridinglevel = MAX(ridinglevel-20,0);

		if( (100 + ridinglevel * 2) < (ridingskill / 25 + 10))
		{
			if(r_info[m_list[p_ptr->riding].r_idx].flags1 & RF1_UNIQUE) new_rakuba_type = 1;
			else new_rakuba_type = 2;
		}
		else if(((100 + ridinglevel * 2) * 4 / 5) < (ridingskill / 30 + 10)) new_rakuba_type = 3;
		else new_rakuba_type = 4;

		if(rakuba_type != new_rakuba_type)
		{
			rakuba_type = new_rakuba_type;
#ifdef JP
			if(rakuba_type == 1) msg_print("このモンスターを完全に乗りこなすのは無理かもしれない・・");
			else if(rakuba_type == 2) msg_print("このモンスターなら完璧に乗りこなせる。");
			else if(rakuba_type == 3) msg_print("このモンスターを少し持て余し気味だ。");
			else if(rakuba_type == 4) msg_print("このモンスターからはいつ振り落とされるかわからない！");
#else
            if(rakuba_type == 1) msg_print("You might be able to perfectly control this monster...");
			else if(rakuba_type == 2) msg_print("You are perfectly controlling this monster.");
			else if(rakuba_type == 3) msg_print("This monster is somewhat unruly.");
			else if(rakuba_type == 4) msg_print("You don't know when this monster is going to throw you off!");
#endif
		}
	}
	else
		rakuba_type = 0;

	/*:::武器重量、ふさわしくない装備などのメッセージ*/

	/* Take note when "heavy bow" changes */
	//if (p_ptr->old_heavy_shoot != p_ptr->heavy_shoot)
	if (p_ptr->old_shoot_penalty != p_ptr->shoot_penalty)
	{
#ifdef JP
		if (p_ptr->shoot_penalty == SHOOT_HIGH_PENALTY)
			msg_print("手に持った武器が邪魔で射撃をしにくい。");
		else if (p_ptr->shoot_penalty == SHOOT_LOW_PENALTY)
			msg_print("手に持った物が邪魔で少し射撃をしにくい。");
		else if (p_ptr->shoot_penalty == SHOOT_HEAVY_WEAPON)
			msg_print("射撃武器が重すぎて構えられない。");
		else if (p_ptr->shoot_penalty == SHOOT_GUN_ONLY)
			msg_print("この装備では銃しか使えない。");
		else if (p_ptr->shoot_penalty == SHOOT_UNSUITABLE)
		{
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
				msg_print("この装備ではうまく射撃ができない。");
		}
		else if (p_ptr->shoot_penalty == SHOOT_NO_PENALTY && p_ptr->old_shoot_penalty != SHOOT_NO_WEAPON)
			msg_print("邪魔なものを手放したので射撃をしやすくなった。");
#else
        if (p_ptr->shoot_penalty == SHOOT_HIGH_PENALTY)
			msg_print("It's hard to shoot while wielding a weapon as well.");
		else if (p_ptr->shoot_penalty == SHOOT_LOW_PENALTY)
			msg_print("It's somewhat hard to shoot while holding something else as well.");
		else if (p_ptr->shoot_penalty == SHOOT_HEAVY_WEAPON)
			msg_print("You have trouble wielding such a heavy bow.");
		else if (p_ptr->shoot_penalty == SHOOT_GUN_ONLY)
			msg_print("You can only shoot guns with your current equipment.");
		else if (p_ptr->shoot_penalty == SHOOT_UNSUITABLE)
		{
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
				msg_print("You can't shoot very well with your current equipment.");
		}
		else if (p_ptr->shoot_penalty == SHOOT_NO_PENALTY && p_ptr->old_shoot_penalty != SHOOT_NO_WEAPON)
			msg_print("You have easier time shooting now that your hands are free.");
#endif
#if 0
		/* Message */
		if (p_ptr->heavy_shoot)
		{
#ifdef JP
			msg_print("射撃武器が重くてうまく構えられない。");
#else
			msg_print("You have trouble wielding such a heavy bow.");
#endif

		}
	//	else if (inventory[INVEN_BOW].k_idx)
		else if (p_ptr->tval_ammo)
		{
#ifdef JP
			msg_print("この弓なら装備していても辛くない。");
#else
			msg_print("You have no trouble wielding your bow.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("重い弓を装備からはずして体が楽になった。");
#else
			msg_print("You feel relieved to put down your heavy bow.");
#endif

		}
#endif
		/* Save it */
		p_ptr->old_shoot_penalty = p_ptr->shoot_penalty;
	}

	for (i = 0 ; i < 2 ; i++)
	{
		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_heavy_wield[i] != p_ptr->heavy_wield[i])
		{
			/* Message */
			if (p_ptr->heavy_wield[i])
			{
#ifdef JP
				msg_print("こんな重い武器を装備しているのは大変だ。");
#else
				msg_print("You have trouble wielding such a heavy weapon.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("これなら装備していても辛くない。");
#else
				msg_print("You have no trouble wielding your weapon.");
#endif

			}
			else if (p_ptr->heavy_wield[1-i])
			{
#ifdef JP
				msg_print("まだ武器が重い。");
#else
				msg_print("You have still trouble wielding a heavy weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("重い武器を装備からはずして体が楽になった。");
#else
				msg_print("You feel relieved to put down your heavy weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_heavy_wield[i] = p_ptr->heavy_wield[i];
		}

		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_riding_wield[i] != p_ptr->riding_wield[i] )
		{
			/* Message */
			if (p_ptr->riding_wield[i])
			{
#ifdef JP
				if (CLASS_RIDING_BACKDANCE)
				{
					if(p_ptr->do_martialarts)
						msg_print("背後で踊っているので格闘攻撃が届きにくい。");
					else
						msg_print("この武器はダンス中に使うにはむかないようだ。");
				}
				else
					msg_print("この武器は乗馬中に使うにはむかないようだ。");

#else
				if (CLASS_RIDING_BACKDANCE)
				{
					if(p_ptr->do_martialarts)
						msg_print("It's difficulty to use martial arts while backdancing.");
					else
						msg_print("This weapon is not suitable for use while dancing.");
				}
				else
					msg_print("This weapon is not suitable for use while riding.");
#endif

			}
			else if (!p_ptr->riding)
			{
#ifdef JP
				if (CLASS_RIDING_BACKDANCE)
					msg_print("武器を振り回しやすくなった。");
				else
					msg_print("この武器は徒歩で使いやすい。");
#else
				if (CLASS_RIDING_BACKDANCE)
					msg_print("You have easier time wielding your weapon.");
				else
					msg_print("This weapon was not suitable for use while riding.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				if (CLASS_RIDING_BACKDANCE)
					msg_print("この武器はダンス中にも少しは使いやすい。");
				else
					msg_print("これなら乗馬中にぴったりだ。");
#else
				if (CLASS_RIDING_BACKDANCE)
					msg_print("This weapon is somewhat easy to use while dancing as well.");
				else
					msg_print("This weapon is suitable for use while riding.");
#endif

			}
			/* Save it */
			p_ptr->old_riding_wield[i] = p_ptr->riding_wield[i];
		}

		/* Take note when "illegal weapon" changes */
		if (p_ptr->old_icky_wield[i] != p_ptr->icky_wield[i])
		{
			/* Message */
			if (p_ptr->icky_wield[i])
			{
#ifdef JP
				msg_print("今の装備はどうも自分にふさわしくない気がする。");
#else
				msg_print("You do not feel comfortable with your weapon.");
#endif
				if (hack_mind)
				{
					chg_virtue(V_FAITH, -1);
				}
			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("今の装備は自分にふさわしい気がする。");
#else
				msg_print("You feel comfortable with your weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("装備をはずしたら随分と気が楽になった。");
#else
				msg_print("You feel more comfortable after removing your weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_icky_wield[i] = p_ptr->icky_wield[i];
		}
	}

	if (p_ptr->riding && (p_ptr->old_riding_ryoute != p_ptr->riding_ryoute) && !CLASS_RIDING_BACKDANCE)
	{
		/* Message */
		if (p_ptr->riding_ryoute)
		{
#ifdef JP
			msg_format("%s馬を操れない。", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "両手がふさがっていて" : "");
#else
            if (empty_hands(FALSE) == EMPTY_HAND_NONE) {
                msg_print("You are using both hand for fighting, and you can't control your mount.");
            } else {
                msg_print("You can't control your mount.");
            }
#endif
		}
		else
		{
#ifdef JP
			msg_format("%s馬を操れるようになった。", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "手が空いて" : "");
#else
			if (empty_hands(FALSE) == EMPTY_HAND_NONE) {
                msg_print("You began to control your mount with one hand.");
            } else {
                msg_print("You began to control your mount.");
            }
#endif
		}

		p_ptr->old_riding_ryoute = p_ptr->riding_ryoute;
	}

	/*:::heavy_armorは魔法や一部特技にも影響するようにしたので常にメッセージ*/
	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && (monk_armour_aux != monk_notify_aux))
	//if (p_ptr->do_martialarts && heavy_armor() && (monk_notify_aux == FALSE))
	if ( heavy_armor() && (monk_notify_aux == FALSE))
	{
		monk_notify_aux = TRUE;
#ifdef JP
		msg_print("装備が重くて俊敏に動くのが難しい。");
#else
			msg_print("The weight of your equipment encumbers your movement.");
#endif
	}
	//else if( p_ptr->do_martialarts && !heavy_armor() && (monk_notify_aux == TRUE))
	else if(!heavy_armor() && (monk_notify_aux == TRUE))
	{
		monk_notify_aux = FALSE;
#ifdef JP
		msg_print("この装備なら支障なく動けそうだ。");
#else
			msg_print("You feel able to move unencumbered with your current equipment.");
#endif

	}
	//else monk_notify_aux = FALSE;

	/*:::足元とアイテム欄を調べ、「石の壁」か「壁抜け」を使える状態だったら「モンスターによって匂いで追跡されない」フラグを立てる・・？*/
	/*:::よく解らんが消してしまって大丈夫だろう*/
	///del140208 壁抜けなどによる匂い追跡解除処理らしいのを消した
#if 0
	for (i = 0; i < INVEN_PACK; i++)
	{
#if 0
		if ((inventory[i].tval == TV_SORCERY_BOOK) && (inventory[i].sval == 2)) have_dd_s = TRUE;
		if ((inventory[i].tval == TV_TRUMP_BOOK) && (inventory[i].sval == 1)) have_dd_t = TRUE;
#endif
		if ((inventory[i].tval == TV_NATURE_BOOK) && (inventory[i].sval == 2)) have_sw = TRUE;
		if ((inventory[i].tval == TV_CRAFT_BOOK) && (inventory[i].sval == 2)) have_kabe = TRUE;
	}
	for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

#if 0
		if ((o_ptr->tval == TV_SORCERY_BOOK) && (o_ptr->sval == 3)) have_dd_s = TRUE;
		if ((o_ptr->tval == TV_TRUMP_BOOK) && (o_ptr->sval == 1)) have_dd_t = TRUE;
#endif
		if ((o_ptr->tval == TV_NATURE_BOOK) && (o_ptr->sval == 2)) have_sw = TRUE;
		if ((o_ptr->tval == TV_CRAFT_BOOK) && (o_ptr->sval == 2)) have_kabe = TRUE;
	}

	if (p_ptr->pass_wall && !p_ptr->kill_wall) p_ptr->no_flowed = TRUE;
#if 0
	if (have_dd_s && ((p_ptr->realm1 == REALM_SORCERY) || (p_ptr->realm2 == REALM_SORCERY) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		const magic_type *s_ptr = &mp_ptr->info[REALM_SORCERY-1][SPELL_DD_S];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_dd_t && ((p_ptr->realm1 == REALM_TRUMP) || (p_ptr->realm2 == REALM_TRUMP) || (p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)))
	{
		const magic_type *s_ptr = &mp_ptr->info[REALM_TRUMP-1][SPELL_DD_T];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}
#endif
	if (have_sw && ((p_ptr->realm1 == REALM_NATURE) || (p_ptr->realm2 == REALM_NATURE) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		const magic_type *s_ptr = &mp_ptr->info[REALM_NATURE-1][SPELL_SW];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_kabe && ((p_ptr->realm1 == REALM_CRAFT) || (p_ptr->realm2 == REALM_CRAFT) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		const magic_type *s_ptr = &mp_ptr->info[REALM_CRAFT-1][SPELL_KABE];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}
#endif


}



/*
 * Handle "p_ptr->notice"
 */
/*:::アイテム自動拾い、ザック整理*/
void notice_stuff(void)
{
	/* Notice stuff */
	if (!p_ptr->notice) return;


	/* Actually do auto-destroy */
	if (p_ptr->notice & (PN_AUTODESTROY))
	{
		p_ptr->notice &= ~(PN_AUTODESTROY);
		autopick_delayed_alter();
	}

	/* Combine the pack */
	if (p_ptr->notice & (PN_COMBINE))
	{
		p_ptr->notice &= ~(PN_COMBINE);
		combine_pack();
	}

	/* Reorder the pack */
	if (p_ptr->notice & (PN_REORDER))
	{
		p_ptr->notice &= ~(PN_REORDER);
		reorder_pack();
	}
}


/*
 * Handle "p_ptr->update"
 */
void update_stuff(void)
{
	/* Update stuff */
	if (!p_ptr->update) return;


	if (p_ptr->update & (PU_BONUS))
	{
		p_ptr->update &= ~(PU_BONUS);
		calc_bonuses();

		//v1.1.78 装備変更による酔拳使用可否チェック　ここに書いていいものか
		check_suiken();
	}
	if (p_ptr->update & (PU_TORCH))
	{
		p_ptr->update &= ~(PU_TORCH);
		/*:::光源範囲を計算*/
		calc_torch();
	}

	if (p_ptr->update & (PU_HP))
	{
		p_ptr->update &= ~(PU_HP);
		/*:::最大HPを計算する*/
		calc_hitpoints();
	}

	if (p_ptr->update & (PU_MANA))
	{
		p_ptr->update &= ~(PU_MANA);
		/*:::最大MPを計算する*/
		calc_mana();
	}

	if (p_ptr->update & (PU_SPELLS))
	{
		p_ptr->update &= ~(PU_SPELLS);
		/*:::魔法を忘れたり思い出したりする可能性のあるとき計算する*/
		calc_spells();
	}

	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;

	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;

	if (p_ptr->update & (PU_UN_LITE))
	{
		p_ptr->update &= ~(PU_UN_LITE);
		forget_lite();
	}
	if (p_ptr->update & (PU_UN_VIEW))
	{
		p_ptr->update &= ~(PU_UN_VIEW);
		forget_view();
	}

	if (p_ptr->update & (PU_VIEW))
	{
		p_ptr->update &= ~(PU_VIEW);
		update_view();
	}
	if (p_ptr->update & (PU_LITE))
	{
		p_ptr->update &= ~(PU_LITE);
		update_lite();
	}

	if (p_ptr->update & (PU_FLOW))
	{
		p_ptr->update &= ~(PU_FLOW);
		update_flow();
	}

	if (p_ptr->update & (PU_DISTANCE))
	{
		p_ptr->update &= ~(PU_DISTANCE);

		/* Still need to call update_monsters(FALSE) after update_mon_lite() */
		/* p_ptr->update &= ~(PU_MONSTERS); */

		/*:::フロアのモンスターが＠に認識されているかどうかを再計算、再描画する*/
		update_monsters(TRUE);
	}

	if (p_ptr->update & (PU_MON_LITE))
	{
		p_ptr->update &= ~(PU_MON_LITE);
		update_mon_lite();
	}

	/*
	 * Mega-Hack -- Delayed visual update
	 * Only used if update_view(), update_lite() or update_mon_lite() was called
	 */
	/*:::モンスターの光源移動に伴いもう一度アップデート？*/
	if (p_ptr->update & (PU_DELAY_VIS))
	{
		p_ptr->update &= ~(PU_DELAY_VIS);
		delayed_visual_update();
	}

	if (p_ptr->update & (PU_MONSTERS))
	{
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
	}


}


/*
 * Handle "p_ptr->redraw"
 */
void redraw_stuff(void)
{
	/* Redraw stuff */
	if (!p_ptr->redraw) return;


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;



	/* Hack -- clear the screen */
	if (p_ptr->redraw & (PR_WIPE))
	{
		p_ptr->redraw &= ~(PR_WIPE);
		msg_print(NULL);
		Term_clear();
	}


	if (p_ptr->redraw & (PR_MAP))
	{
		p_ptr->redraw &= ~(PR_MAP);
		prt_map();
	}


	if (p_ptr->redraw & (PR_BASIC))
	{
		p_ptr->redraw &= ~(PR_BASIC);
		p_ptr->redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		p_ptr->redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		p_ptr->redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		p_ptr->redraw &= ~(PR_DEPTH | PR_HEALTH | PR_UHEALTH);
		prt_frame_basic();
		prt_time();
		prt_dungeon();
	}

	if (p_ptr->redraw & (PR_EQUIPPY))
	{
		p_ptr->redraw &= ~(PR_EQUIPPY);
		print_equippy(); /* To draw / delete equippy chars */
	}

	if (p_ptr->redraw & (PR_MISC))
	{
		p_ptr->redraw &= ~(PR_MISC);
		///mod150103 野良神様の種族名表記
		if(p_ptr->prace == RACE_STRAYGOD)
			prt_field(deity_table[p_ptr->race_multipur_val[3]].deity_name, ROW_RACE, COL_RACE);
		else
			prt_field(rp_ptr->title, ROW_RACE, COL_RACE);
/*		prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS); */

	}

	if (p_ptr->redraw & (PR_TITLE))
	{
		p_ptr->redraw &= ~(PR_TITLE);
		prt_title();
	}

	if (p_ptr->redraw & (PR_LEV))
	{
		p_ptr->redraw &= ~(PR_LEV);
		prt_level();
	}

	if (p_ptr->redraw & (PR_EXP))
	{
		p_ptr->redraw &= ~(PR_EXP);
		prt_exp();
	}

	if (p_ptr->redraw & (PR_STATS))
	{
		p_ptr->redraw &= ~(PR_STATS);
		prt_stat(A_STR);
		prt_stat(A_INT);
		prt_stat(A_WIS);
		prt_stat(A_DEX);
		prt_stat(A_CON);
		prt_stat(A_CHR);
	}

	if (p_ptr->redraw & (PR_STATUS))
	{
		p_ptr->redraw &= ~(PR_STATUS);
		prt_status();
	}

	if (p_ptr->redraw & (PR_ARMOR))
	{
		p_ptr->redraw &= ~(PR_ARMOR);
		prt_ac();
	}

	if (p_ptr->redraw & (PR_HP))
	{
		p_ptr->redraw &= ~(PR_HP);
		prt_hp();
	}

	if (p_ptr->redraw & (PR_MANA))
	{
		p_ptr->redraw &= ~(PR_MANA);
		prt_sp();
	}

	if (p_ptr->redraw & (PR_GOLD))
	{
		p_ptr->redraw &= ~(PR_GOLD);
		prt_gold();
	}

	if (p_ptr->redraw & (PR_DEPTH))
	{
		p_ptr->redraw &= ~(PR_DEPTH);
		prt_depth();
	}

	if (p_ptr->redraw & (PR_HEALTH))
	{
		p_ptr->redraw &= ~(PR_HEALTH);
		health_redraw(FALSE);
	}

	if (p_ptr->redraw & (PR_UHEALTH))
	{
		p_ptr->redraw &= ~(PR_UHEALTH);
		health_redraw(TRUE);
	}


	if (p_ptr->redraw & (PR_EXTRA))
	{
		p_ptr->redraw &= ~(PR_EXTRA);
		p_ptr->redraw &= ~(PR_CUT | PR_STUN);
		p_ptr->redraw &= ~(PR_HUNGER);
		p_ptr->redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY | PR_IMITATION | PR_STATUS);
		prt_frame_extra();
	}

	if (p_ptr->redraw & (PR_CUT))
	{
		p_ptr->redraw &= ~(PR_CUT);
		prt_cut();
	}

	if (p_ptr->redraw & (PR_STUN))
	{
		p_ptr->redraw &= ~(PR_STUN);
		prt_stun();
	}

	if (p_ptr->redraw & (PR_HUNGER))
	{
		p_ptr->redraw &= ~(PR_HUNGER);
		prt_hunger();
	}

	if (p_ptr->redraw & (PR_STATE))
	{
		p_ptr->redraw &= ~(PR_STATE);
		prt_state();
	}

	if (p_ptr->redraw & (PR_SPEED))
	{
		p_ptr->redraw &= ~(PR_SPEED);
		prt_speed();
	}

	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (p_ptr->redraw & (PR_IMITATION))
		{
			p_ptr->redraw &= ~(PR_IMITATION);
			prt_imitation();
		}
	}
	else if (p_ptr->redraw & (PR_STUDY))
	{
		p_ptr->redraw &= ~(PR_STUDY);
		prt_study();
	}
}


/*
 * Handle "p_ptr->window"
 */
/*:::サブウィンドウ処理*/
void window_stuff(void)
{
	int j;

	u32b mask = 0L;


	/* Nothing to do */
	if (!p_ptr->window) return;

	/* Scan windows */
	/*:::サブウィンドウは7つだがどう対応しているのか？0はメイン？*/
	for (j = 0; j < 8; j++)
	{
		/* Save usable flags */
		if (angband_term[j]) mask |= window_flag[j];
	}

	/* Apply usable flags */
	p_ptr->window &= mask;

	/* Nothing to do */
	if (!p_ptr->window) return;

	if (p_ptr->window & (PW_FLOOR_ITEM_LIST))
	{
		p_ptr->window &= ~(PW_FLOOR_ITEM_LIST);
		fix_floor_item_list(py,px);
	}

	/* Display inventory */
	if (p_ptr->window & (PW_INVEN))
	{
		p_ptr->window &= ~(PW_INVEN);
		fix_inven();
	}

	/* Display equipment */
	if (p_ptr->window & (PW_EQUIP))
	{
		p_ptr->window &= ~(PW_EQUIP);
		fix_equip();
	}

	/* Display spell list */
	if (p_ptr->window & (PW_SPELL))
	{
		p_ptr->window &= ~(PW_SPELL);
		fix_spell();
	}

	/* Display player */
	if (p_ptr->window & (PW_PLAYER))
	{
		p_ptr->window &= ~(PW_PLAYER);
		fix_player();
	}

	/* Display monster list */
	if (p_ptr->window & (PW_MONSTER_LIST))
	{
		p_ptr->window &= ~(PW_MONSTER_LIST);
		fix_monster_list();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_MESSAGE))
	{
		p_ptr->window &= ~(PW_MESSAGE);
		fix_message();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_OVERHEAD))
	{
		p_ptr->window &= ~(PW_OVERHEAD);
		fix_overhead();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_DUNGEON))
	{
		p_ptr->window &= ~(PW_DUNGEON);
		fix_dungeon();
	}

	/* Display monster recall */
	if (p_ptr->window & (PW_MONSTER))
	{
		p_ptr->window &= ~(PW_MONSTER);
		fix_monster();
	}

	/* Display object recall */
	if (p_ptr->window & (PW_OBJECT))
	{
		p_ptr->window &= ~(PW_OBJECT);
		fix_object();
	}
}


/*
 * Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window"
 */
/*:::ステータス・パラメータ・マップの再計算・再描画処理*/
void handle_stuff(void)
{
	/* Update stuff */
	if (p_ptr->update) update_stuff();
	/* Redraw stuff */
	if (p_ptr->redraw) redraw_stuff();
	/* Window stuff */
	if (p_ptr->window) window_stuff();

}

/*:::手が空かどうか判定する。乗馬中で手綱を持っているかどうかを考慮している。*/
/*:::riding_control:乗馬中かどうか？*/
s16b empty_hands(bool riding_control)
{
	s16b status = EMPTY_HAND_NONE;

	if (!inventory[INVEN_RARM].k_idx && !check_invalidate_inventory(INVEN_RARM)) status |= EMPTY_HAND_RARM;
	if (!inventory[INVEN_LARM].k_idx && !check_invalidate_inventory(INVEN_LARM)) status |= EMPTY_HAND_LARM;

	if (riding_control && (status != EMPTY_HAND_NONE) && p_ptr->riding && !(p_ptr->pet_extra_flags & PF_RYOUTE) && !CLASS_RIDING_BACKDANCE)
	{
		if (status & EMPTY_HAND_LARM) status &= ~(EMPTY_HAND_LARM);
		else if (status & EMPTY_HAND_RARM) status &= ~(EMPTY_HAND_RARM);
	}

	return status;
}

/*:::修行僧、錬気術師、忍者の重量制限を計算*/
///item 修行僧などの重量制限計算　TVAL順番依存処理あり
///mod140315 どの職でも参照されるようにした
///mod140621 全ての装備がそのまま適用される代わりに、腕力により20Kgまで持てるようにした
bool heavy_armor(void)
{
	u16b monk_arm_wgt = 0;
	u16b can_wear = (adj_general[p_ptr->stat_ind[A_STR]] * 5 + (p_ptr->lev * 3) + 100);
	int i;

	//if ((p_ptr->pclass != CLASS_MONK) && (p_ptr->pclass != CLASS_FORCETRAINER) && (p_ptr->pclass != CLASS_NINJA)) return FALSE;

	/* Weight the armor */
	//if(inventory[INVEN_RARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_RARM].weight;
	//if(inventory[INVEN_LARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_LARM].weight;
/*
	if(inventory[INVEN_RARM].tval > TV_KNIFE) monk_arm_wgt += inventory[INVEN_RARM].weight;
	if(inventory[INVEN_LARM].tval > TV_KNIFE) monk_arm_wgt += inventory[INVEN_LARM].weight;
	monk_arm_wgt += inventory[INVEN_RIBBON].weight;
	monk_arm_wgt += inventory[INVEN_BODY].weight;
	monk_arm_wgt += inventory[INVEN_HEAD].weight;
	monk_arm_wgt += inventory[INVEN_OUTER].weight;
	monk_arm_wgt += inventory[INVEN_HANDS].weight;
	monk_arm_wgt += inventory[INVEN_FEET].weight;
*/
	for(i=INVEN_RARM;i<INVEN_TOTAL;i++)
		if(!check_invalidate_inventory(i))
			monk_arm_wgt += inventory[i].weight;

	if(p_ptr->pclass == CLASS_YUGI) monk_arm_wgt /= 2; //勇儀は重量過多になりにくい
	if (p_ptr->pclass == CLASS_URUMI) monk_arm_wgt /= 2; //v1.1.69 潤美も重量過多になりにくい
	if (p_ptr->pclass == CLASS_SAKI) monk_arm_wgt /= 2; //v1.1.71 早鬼も重量過多になりにくい

	if(p_ptr->pclass == CLASS_MARTIAL_ARTIST) can_wear = can_wear * 3 / 4; //格闘家は重量過多になりやすい
   //三月精は重量限界も三人分
	if (p_ptr->pclass == CLASS_3_FAIRIES)
	{
		//v1.1.68 専用性格のときさらに倍
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			can_wear *= 6;
		else
			can_wear *= 3;
	}

	return (monk_arm_wgt > can_wear);
//	return (monk_arm_wgt > (100 + (p_ptr->lev * 4)));
}



