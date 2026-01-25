/* Purpose: Object flavor code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
/*:::魔法書や油壺など、最初から鑑定済みのアイテムの判定*/
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
	{
		/* Spellbooks */
		/*
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		*/
		case TV_BOOK_ELEMENT:
		case TV_BOOK_FORESEE:
		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:
		case TV_BOOK_MYSTIC:
		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:
		case TV_BOOK_NATURE:
		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:
		case TV_BOOK_NECROMANCY:
		case TV_BOOK_CHAOS:
  		case TV_BOOK_HISSATSU:
  		case TV_BOOK_MEDICINE:
		case TV_BOOK_OCCULT:
		case TV_STONE_INCIDENT:
		{
			return (TRUE);
		}

		/* Simple items */
		case TV_FLASK:
		case TV_WHISTLE:
		case TV_MATERIAL:
		case TV_SOUVENIR:
		case TV_COMPOUND:
		case TV_ALCOHOL:
		case TV_SWEETS:
		case TV_SOFTDRINK:
		case TV_MACHINE:
		case TV_MUSHROOM:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		case TV_FOOD:
		case TV_BUNBUN:
		case TV_KAKASHI:
		case TV_ITEMCARD:
		case TV_STRANGE_OBJ:
		case TV_SPELLCARD:
		case TV_ABILITY_CARD:
		{
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Create a name from random parts.
 */
void get_table_name_aux(char *out_string)
{
#ifdef JP
	char Syllable[80];
	get_rnd_line("aname_j.txt", 1, Syllable);
	strcpy(out_string, Syllable);
	get_rnd_line("aname_j.txt", 2, Syllable);
	strcat(out_string, Syllable);
#else
#define MAX_SYLLABLES 164       /* Used with scrolls (see below) */
    char NamePart[80];
	get_rnd_line("aname_e.txt", 1, NamePart);
	strcpy(out_string, NamePart);
	strcat(out_string, " ");
	get_rnd_line("aname_e.txt", 2, NamePart);
	strcat(out_string, NamePart);

	/*static cptr syllables[MAX_SYLLABLES] = {
		"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
		"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
		"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
		"byt", "comp", "con", "cos", "cre", "dalf", "dan",
		"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
		"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
		"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
		"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
		"kho", "kli", "klis", "la", "lech", "man", "mar",
		"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
		"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
		"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
		"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
		"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
		"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
		"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
		"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
		"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
		"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
		"yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
		"foo", "ju", "xuxu"
	};

	int testcounter = randint1(3) + 1;

	strcpy(out_string, "");

	if (randint1(3) == 2)
	{
		while (testcounter--)
			strcat(out_string, syllables[randint0(MAX_SYLLABLES)]);
	}
	else
	{
		char Syllable[80];
		testcounter = randint1(2) + 1;
		while (testcounter--)
		{
			(void)get_rnd_line("elvish.txt", 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[0] = toupper(out_string[0]);

	out_string[16] = '\0';*/
#endif
}


/*
 * Create a name from random parts with quotes.
 */
void get_table_name(char *out_string)
{
	char buff[80];
	get_table_name_aux(buff);

#ifdef JP
	sprintf(out_string, "『%s』", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Make random Sindarin name
 */
/*:::アーティファクト用にシンダリン語の名前を決める*/
void get_table_sindarin_aux(char *out_string)
{
	char Syllable[80];
#ifdef JP
	char tmp[80];
#endif

	get_rnd_line("sname.txt", 1, Syllable);
#ifdef JP
	strcpy(tmp, Syllable);
#else
	strcpy(out_string, Syllable);
#endif

	get_rnd_line("sname.txt", 2, Syllable);
#ifdef JP
	strcat(tmp, Syllable);
	sindarin_to_kana(out_string, tmp);
#else
    out_string[0] = toupper(out_string[0]);
	strcat(out_string, Syllable);
#endif
}


/*
 * Make random Sindarin name with quotes
 */
///mod140719 ランダムユニーク命名用に、『』をつけずにできるようにした
void get_table_sindarin(char *out_string ,bool use_brackets)
{
	char buff[80];
	get_table_sindarin_aux(buff);

#ifdef JP
	if(use_brackets) sprintf(out_string, "『%s』", buff);
	else sprintf(out_string, "%s", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Shuffle flavor indices of a group of objects with given tval
 */
static void shuffle_flavors(byte tval)
{
	s16b *k_idx_list;
	int k_idx_list_num = 0;
	int i;

	/* Allocate an array for a list of k_idx */
	C_MAKE(k_idx_list, max_k_idx, s16b);

	/* Search objects with given tval for shuffle */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip non-Rings */
		if (k_ptr->tval != tval) continue;

		/* Paranoia -- Skip objects without flavor */
		if (!k_ptr->flavor) continue;

		/* Skip objects with a fixed flavor name */
		if (have_flag(k_ptr->flags, TR_FIXED_FLAVOR)) continue;

		/* Remember k_idx */
		k_idx_list[k_idx_list_num] = i;

		/* Increase number of remembered indices */
		k_idx_list_num++;
	}

	/* Shuffle flavors */
	for (i = 0; i < k_idx_list_num; i++)
	{
		object_kind *k1_ptr = &k_info[k_idx_list[i]];
		object_kind *k2_ptr = &k_info[k_idx_list[randint0(k_idx_list_num)]];

		/* Swap flavors of this pair */
		s16b tmp = k1_ptr->flavor;
		k1_ptr->flavor = k2_ptr->flavor;
		k2_ptr->flavor = tmp;
	}

	/* Free an array for a list of k_idx */
	C_KILL(k_idx_list, max_k_idx, s16b);
}

/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
/*:::アイテムの不確定名をシャッフルしている*/
///item 不確定名のある巻物や薬などを追加したとき
void flavor_init(void)
{
	int i;
	u32b state_backup[4];

	/* Hack -- Backup the RNG state */
	Rand_state_backup(state_backup);

	/* Hack -- Induce consistant flavors */
	Rand_state_init(seed_flavor);


	/* Initialize flavor index of each object by itself */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip objects without flavor name */
		if (!k_ptr->flavor_name) continue;

		/*
		 * Initialize flavor index to itself
		 *  -> Shuffle it later
		 */
		k_ptr->flavor = i;
	}

	/* Shuffle Rings */
	shuffle_flavors(TV_RING);

	/* Shuffle Amulets */
	shuffle_flavors(TV_AMULET);

	/* Shuffle Staves */
	shuffle_flavors(TV_STAFF);

	/* Shuffle Wands */
	shuffle_flavors(TV_WAND);

	/* Shuffle Rods */
	shuffle_flavors(TV_ROD);

	/* Shuffle Mushrooms */
	///mod140622 キノコのフレーバー置き換え
	shuffle_flavors(TV_MUSHROOM);

	/* Shuffle Potions */
	shuffle_flavors(TV_POTION);

	/* Shuffle Scrolls */
	shuffle_flavors(TV_SCROLL);


	/* Hack -- Restore the RNG state */
	Rand_state_restore(state_backup);

	/* Analyze every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name) continue;

		/* No flavor yields aware */
		if (!k_ptr->flavor) k_ptr->aware = TRUE;

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
	}
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s) *t++ = *s++;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




#ifdef JP
/*
 * 日本語の個数表示ルーチン
 *（cmd1.c で流用するために object_desc_japanese から移動した。）
 */
///item アイテムの個数を数えるときの単位
///mod131223 アイテムの個数
char *object_desc_kosuu(char *t, object_type *o_ptr)
{
    t = object_desc_num(t, o_ptr->number);

    switch (o_ptr->tval)
    {
      case TV_BOLT:
      case TV_ARROW:
      case TV_POLEARM:
      case TV_HAMMER:
      case TV_STICK:
      case TV_SPEAR:
  	  case TV_OTHERWEAPON:
      case TV_STAFF:
      case TV_WAND:
      case TV_ROD:
      case TV_RIBBON:
      case TV_SOFTDRINK:
      case TV_ALCOHOL:

	  {
	  t = object_desc_str(t, "本");
	  break;
      }
	  case TV_PHOTO:
	  case TV_ITEMCARD:
	  case TV_ABILITY_CARD:
      {
	  t = object_desc_str(t, "枚");
	  break;
      }

	  case TV_SCROLL:
      {
	  t = object_desc_str(t, "巻");
	  break;
      }
      case TV_POTION:
      {
	  t = object_desc_str(t, "服");
	  break;
      }
	  case TV_BOOK_ELEMENT:
	  case TV_BOOK_FORESEE:
	  case TV_BOOK_ENCHANT:
	  case TV_BOOK_SUMMONS:
	  case TV_BOOK_MYSTIC:
	  case TV_BOOK_LIFE:
	  case TV_BOOK_PUNISH:
	  case TV_BOOK_NATURE:
	  case TV_BOOK_TRANSFORM:
	  case TV_BOOK_DARKNESS:
	  case TV_BOOK_NECROMANCY:
	  case TV_BOOK_CHAOS:
  	  case TV_BOOK_HISSATSU:
  	  case TV_BOOK_MEDICINE:
  	  case TV_MAGICITEM:
  	  case TV_CAPTURE:
	  case TV_BOOK_OCCULT:
/*
	  case  TV_LIFE_BOOK:
      case  TV_SORCERY_BOOK:
      case  TV_NATURE_BOOK:
      case  TV_CHAOS_BOOK:
      case  TV_DEATH_BOOK:
      case  TV_TRUMP_BOOK:
      case  TV_ARCANE_BOOK:
      case  TV_CRAFT_BOOK:
      case  TV_DAEMON_BOOK:
      case  TV_CRUSADE_BOOK:
      case  TV_MUSIC_BOOK:
      case  TV_HISSATSU_BOOK:
	  case TV_HEX_BOOK:
*/
	  {
	  t = object_desc_str(t, "冊");
	  break;
      }
      case TV_CLOTHES:
      case TV_ARMOR:
      case TV_DRAG_ARMOR:
      case TV_CLOAK:
      {
	  t = object_desc_str(t, "着");
	  break;
      }
      case TV_KNIFE:
	  case TV_SWORD:
      case TV_KATANA:
      case TV_BOW:
      case TV_AXE:
	  {
	  t = object_desc_str(t, "振");
	  break;
      }
      case TV_BOOTS:
      {
	  t = object_desc_str(t, "足");
	  break;
      }
      case TV_GUN:
      {
		  t = object_desc_str(t, "挺");
		  break;
      }
      case TV_BULLET:
      {
	  t = object_desc_str(t, "発");
	  break;
      }
	    /* 食べもの by ita */
      case TV_FOOD:
      case TV_SWEETS:
	  {
	      t = object_desc_str(t, "食");
	      break;
      }
      default:
      {
	  if (o_ptr->number < 10)
	  {
	      t = object_desc_str(t, "つ");
	  }
	  else
	  {
	      t = object_desc_str(t, "個");
	  }
	  break;
      }
  }
  return (t);
}
#endif

/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if (v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Structs and tables for Auto Inscription for flags
 */

typedef struct flag_insc_table
{
#ifdef JP
	cptr japanese;
#endif
	cptr english;
	int flag;
	int except_flag;
} flag_insc_table;

#ifdef JP
static flag_insc_table flag_insc_plus[] =
{
	{ "攻", "At", TR_BLOWS, -1 },
	{ "速", "Sp", TR_SPEED, -1 },
	{ "腕", "St", TR_STR, -1 },
	{ "知", "In", TR_INT, -1 },
	{ "賢", "Wi", TR_WIS, -1 },
	{ "器", "Dx", TR_DEX, -1 },
	{ "耐", "Cn", TR_CON, -1 },
	{ "魅", "Ch", TR_CHR, -1 },
	{ "道", "Md", TR_MAGIC_MASTERY, -1 },
	{ "隠", "Sl", TR_STEALTH, -1 },
	{ "探", "Sr", TR_SEARCH, -1 },
	{ "赤", "If", TR_INFRA, -1 },
	{ "掘", "Dg", TR_TUNNEL, -1 },
	//v1.1.99 追加
	{ "解", "Da", TR_DISARM, -1 },
	{ "抗", "Sv", TR_SAVING, -1 },

	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "酸", "Ac", TR_IM_ACID, -1 },
	{ "電", "El", TR_IM_ELEC, -1 },
	{ "火", "Fi", TR_IM_FIRE, -1 },
	{ "冷", "Co", TR_IM_COLD, -1 },
	{ NULL, NULL, 0, -1 }
};

///mod131228 TRフラグ変更
static flag_insc_table flag_insc_resistance[] =
{
	{ "酸", "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "電", "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "火", "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "冷", "Co", TR_RES_COLD, TR_IM_COLD },
	{ "毒", "Po", TR_RES_POIS, -1 },
	{ "閃", "Li", TR_RES_LITE, -1 },
	{ "暗", "Dk", TR_RES_DARK, -1 },
	{ "破", "Sh", TR_RES_SHARDS, -1 },
	{ "水", "Wa", TR_RES_WATER, -1 },
	{ "聖", "Ho", TR_RES_HOLY, -1 },
	{ "時", "Ti", TR_RES_TIME, -1 },
	{ "轟", "So", TR_RES_SOUND, -1 },
	{ "獄", "Nt", TR_RES_NETHER, -1 },
	{ "沌", "Ca", TR_RES_CHAOS, -1 },
	{ "劣", "Di", TR_RES_DISEN, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "易", "Es", TR_EASY_SPELL, -1 },
	{ "減", "Dm", TR_DEC_MANA, -1 },
	{ "投", "Th", TR_THROW, -1 },
	{ "双", "To", TR_GENZI, -1 },
	{ "反", "Rf", TR_REFLECT, -1 },
	{ "視", "Si", TR_SEE_INVIS, -1 },
	{ "麻", "Fa", TR_FREE_ACT, -1 },
	{ "恐", "Fe", TR_RES_FEAR, -1 },
	{ "乱", "Co", TR_RES_CONF, -1 },
	{ "盲", "Bl", TR_RES_BLIND, -1 },
	{ "狂", "In", TR_RES_INSANITY, -1 },
	{ "食", "Sd", TR_SLOW_DIGEST, -1 },
	{ "活", "Rg", TR_REGEN, -1 },
	{ "浮", "Lv", TR_LEVITATION, -1 },
	{ "明", "Lu", TR_LITE, -1 },
	{ "警", "Wr", TR_WARNING, -1 },
	{ "俊", "Sp", TR_SPEEDSTER, -1 },
	{ "倍", "Xm", TR_XTRA_MIGHT, -1 },
	{ "射", "Xs", TR_XTRA_SHOTS, -1 },
	{ "瞬", "Te", TR_TELEPORT, -1 },
	{ "怒", "Ag", TR_AGGRAVATE, -1 },
	{ "祝", "Bs", TR_BLESSED, -1 },
	{ "忌", "Ty", TR_TY_CURSE, -1 },
	{ "呪", "C-", TR_ADD_L_CURSE, -1 },
	{ "詛", "C+", TR_ADD_H_CURSE, -1 },
	{ "戻", "C+", TR_BOOMERANG, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "炎", "F", TR_SH_FIRE, -1 },
	{ "電", "E", TR_SH_ELEC, -1 },
	{ "冷", "C", TR_SH_COLD, -1 },
	{ "魔", "M", TR_NO_MAGIC, -1 },
	{ "瞬", "T", TR_NO_TELE, -1 },
	{ NULL, NULL, 0, -1 }
};



static flag_insc_table flag_insc_brand[] =
{
	{ "酸", "A", TR_BRAND_ACID, -1 },
	{ "電", "E", TR_BRAND_ELEC, -1 },
	{ "焼", "F", TR_BRAND_FIRE, -1 },
	{ "凍", "Co", TR_BRAND_COLD, -1 },
	{ "毒", "P", TR_BRAND_POIS, -1 },
	{ "沌", "Ca", TR_CHAOTIC, -1 },
	{ "吸", "V", TR_VAMPIRIC, -1 },
	{ "震", "Q", TR_IMPACT, -1 },
//	{ "切", "S", TR_VORPAL, -1 },
	{ "切", "S", TR_VORPAL, TR_EX_VORPAL },
	{ "理", "M", TR_FORCE_WEAPON, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "沌", "*", TR_KILL_EVIL, -1 },
	{ "人", "p", TR_KILL_HUMAN, -1 },
	{ "龍", "D", TR_KILL_DRAGON, -1 },
	{ "神", "S", TR_KILL_DEITY, -1 },
	{ "妖", "K", TR_KILL_KWAI, -1 },
	{ "秩", "C", TR_KILL_GOOD, -1 },
	{ "悪", "U", TR_KILL_DEMON, -1 },
	{ "死", "L", TR_KILL_UNDEAD, -1 },
	{ "動", "Z", TR_KILL_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "沌", "*", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "人", "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "竜", "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "神", "S", TR_SLAY_DEITY, TR_KILL_DEITY },
	{ "妖", "K", TR_SLAY_KWAI, TR_KILL_KWAI },
	{ "秩", "C", TR_SLAY_GOOD, TR_KILL_GOOD },
	{ "悪", "U", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "死", "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "動", "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "感", "Tele", TR_TELEPATHY, -1 },
	{ "沌", "Evil", TR_ESP_EVIL, -1 },
	{ "秩", "Good", TR_ESP_GOOD, -1 },
	{ "無", "Nolv", TR_ESP_NONLIVING, -1 },
	{ "個", "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "人", "p", TR_ESP_HUMAN, -1 },
	{ "竜", "D", TR_ESP_DRAGON, -1 },
	{ "神", "o", TR_ESP_DEITY, -1 },
	{ "妖", "T", TR_ESP_KWAI, -1 },
	{ "悪", "U", TR_ESP_DEMON, -1 },
	{ "死", "L", TR_ESP_UNDEAD, -1 },
	{ "動", "Z", TR_ESP_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "腕", "St", TR_SUST_STR, -1 },
	{ "知", "In", TR_SUST_INT, -1 },
	{ "賢", "Wi", TR_SUST_WIS, -1 },
	{ "器", "Dx", TR_SUST_DEX, -1 },
	{ "耐", "Cn", TR_SUST_CON, -1 },
	{ "魅", "Ch", TR_SUST_CHR, -1 },
	{ NULL, NULL, 0, -1 }
};

#else
static flag_insc_table flag_insc_plus[] =
{
	{ "At", TR_BLOWS, -1 },
	{ "Sp", TR_SPEED, -1 },
	{ "St", TR_STR, -1 },
	{ "In", TR_INT, -1 },
	{ "Wi", TR_WIS, -1 },
	{ "Dx", TR_DEX, -1 },
	{ "Cn", TR_CON, -1 },
	{ "Ch", TR_CHR, -1 },
	{ "Md", TR_MAGIC_MASTERY, -1 },
	{ "Sl", TR_STEALTH, -1 },
	{ "Sr", TR_SEARCH, -1 },
	{ "If", TR_INFRA, -1 },
	{ "Dg", TR_TUNNEL, -1 },
	//v1.1.99 追加
	{ "Dsrm", TR_DISARM, -1 },
	{ "PrMg", TR_SAVING, -1 },

	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "Ac", TR_IM_ACID, -1 },
	{ "El", TR_IM_ELEC, -1 },
	{ "Fi", TR_IM_FIRE, -1 },
	{ "Co", TR_IM_COLD, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "Co", TR_RES_COLD, TR_IM_COLD },
	{ "Po", TR_RES_POIS, -1 },
	{ "Li", TR_RES_LITE, -1 },
	{ "Dk", TR_RES_DARK, -1 },
	{ "Sh", TR_RES_SHARDS, -1 },
	{ "Wa", TR_RES_WATER, -1 },
	{ "Ho", TR_RES_HOLY, -1 },
	{ "Ti", TR_RES_TIME, -1 },
	{ "So", TR_RES_SOUND, -1 },
	{ "Nt", TR_RES_NETHER, -1 },
	{ "Ca", TR_RES_CHAOS, -1 },
	{ "Di", TR_RES_DISEN, -1 },
	{ NULL, 0, -1 }
};


static flag_insc_table flag_insc_misc[] =
{
	{ "Es", TR_EASY_SPELL, -1 },
	{ "Dm", TR_DEC_MANA, -1 },
	{ "Th", TR_THROW, -1 },
	{ "To", TR_GENZI, -1 },
	{ "Rf", TR_REFLECT, -1 },
	{ "Fa", TR_FREE_ACT, -1 },
	{ "Si", TR_SEE_INVIS, -1 },
	{ "Fe", TR_RES_FEAR, -1 },
	{ "Co", TR_RES_CONF, -1 },
	{ "Bl", TR_RES_BLIND, -1 },
	{ "In", TR_RES_INSANITY, -1 },
	{ "Sd", TR_SLOW_DIGEST, -1 },
	{ "Rg", TR_REGEN, -1 },
	{ "Lv", TR_LEVITATION, -1 },
	{ "Lu", TR_LITE, -1 },
	{ "Wr", TR_WARNING, -1 },
	{ "Sw", TR_SPEEDSTER, -1 },
	{ "Xm", TR_XTRA_MIGHT, -1 },
	{ "Xs", TR_XTRA_SHOTS, -1 },
	{ "Te", TR_TELEPORT, -1 },
	{ "Ag", TR_AGGRAVATE, -1 },
	{ "Bs", TR_BLESSED, -1 },
	{ "Ty", TR_TY_CURSE, -1 },
	{ "C-", TR_ADD_L_CURSE, -1 },
	{ "C+", TR_ADD_H_CURSE, -1 },
	{ "Bm", TR_BOOMERANG, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "F", TR_SH_FIRE, -1 },
	{ "E", TR_SH_ELEC, -1 },
	{ "C", TR_SH_COLD, -1 },
	{ "M", TR_NO_MAGIC, -1 },
	{ "T", TR_NO_TELE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "Ac", TR_BRAND_ACID, -1 },
	{ "El", TR_BRAND_ELEC, -1 },
	{ "Fi", TR_BRAND_FIRE, -1 },
	{ "Co", TR_BRAND_COLD, -1 },
	{ "Po", TR_BRAND_POIS, -1 },
	{ "Ch", TR_CHAOTIC, -1 },
	{ "Vmp", TR_VAMPIRIC, -1 },
	{ "Qk", TR_IMPACT, -1 },
//	{ "S", TR_VORPAL, -1 },
    { "Shp", TR_VORPAL, TR_EX_VORPAL },
	{ "Mn", TR_FORCE_WEAPON, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "Ch", TR_KILL_EVIL, -1 },
	{ "Hm", TR_KILL_HUMAN, -1 },
	{ "Dr", TR_KILL_DRAGON, -1 },
	{ "Dt", TR_KILL_DEITY, -1 },
	{ "Yk", TR_KILL_KWAI, -1 },
	{ "Lw", TR_KILL_GOOD, -1 },
	{ "Dm", TR_KILL_DEMON, -1 },
	{ "Un", TR_KILL_UNDEAD, -1 },
	{ "An", TR_KILL_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "Ch", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "Hm", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "Dr", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "Dt", TR_SLAY_DEITY, TR_KILL_DEITY },
	{ "Yk", TR_SLAY_KWAI, TR_KILL_KWAI },
	{ "Lw", TR_SLAY_GOOD, TR_KILL_GOOD },
	{ "Dm", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "Un", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "An", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "Tele", TR_TELEPATHY, -1 },
	{ "Chs", TR_ESP_EVIL, -1 },
	{ "Law", TR_ESP_GOOD, -1 },
	{ "Nolv", TR_ESP_NONLIVING, -1 },
	{ "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "Hm", TR_ESP_HUMAN, -1 },
	{ "Dr", TR_ESP_DRAGON, -1 },
	{ "Dt", TR_ESP_DEITY, -1 },
	{ "Yk", TR_ESP_KWAI, -1 },
	{ "Dm", TR_ESP_DEMON, -1 },
	{ "Un", TR_ESP_UNDEAD, -1 },
	{ "An", TR_ESP_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "St", TR_SUST_STR, -1 },
	{ "In", TR_SUST_INT, -1 },
	{ "Wi", TR_SUST_WIS, -1 },
	{ "Dx", TR_SUST_DEX, -1 },
	{ "Cn", TR_SUST_CON, -1 },
	{ "Ch", TR_SUST_CHR, -1 },
	{ NULL, 0, -1 }
};
#endif

/* Simple macro for get_inscription() */
#define ADD_INSC(STR) (void)(ptr = object_desc_str(ptr, (STR)))

/*
 *  Helper function for get_inscription()
 */
static char *inscribe_flags_aux(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE], bool kanji, char *ptr)
{
#ifndef JP
	(void)kanji;
#endif

	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
		    (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
#ifdef JP
			ADD_INSC(kanji ? fi_ptr->japanese : fi_ptr->english);
#else
			ADD_INSC(fi_ptr->english);
#endif
		fi_ptr++;
	}

	return ptr;
}


/*
 *  Special variation of have_flag for auto-inscription
 */
static bool have_flag_of(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE])
{
	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
		   (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
			return (TRUE);
		fi_ptr++;
	}

	return (FALSE);
}

///item sys flag アイテムの能力をr火みたいに表記する部分らしい
static char *get_ability_abbreviation(char *ptr, object_type *o_ptr, bool kanji, bool all)
{
	char *prev_ptr = ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);


	/* Remove obvious flags */
	if (!all)
	{
		object_kind *k_ptr = &k_info[o_ptr->k_idx];
		int j;

		/* Base object */
		for (j = 0; j < TR_FLAG_SIZE; j++)
			flgs[j] &= ~k_ptr->flags[j];

		if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~a_ptr->flags[j];
		}

		if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~e_ptr->flags[j];
		}
	}


	/* Plusses */
	if (have_flag_of(flag_insc_plus, flgs))
	{
		if (kanji)
			ADD_INSC("+");
#ifndef JP
        ADD_INSC("+");
#endif
	}
	ptr = inscribe_flags_aux(flag_insc_plus, flgs, kanji, ptr);


	/* Immunity */
	if (have_flag_of(flag_insc_immune, flgs))
	{
		if (!kanji && ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
		ADD_INSC("*");
	}
	ptr = inscribe_flags_aux(flag_insc_immune, flgs, kanji, ptr);

	//Hack - グリマリには+3符と記述
	if(o_ptr->name1 == ART_GRIMOIRE_OF_MARISA) ADD_INSC(_("符","Cd"));

	//v1.1.32 作業服に解と記述 廃止
	//if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_WORKER) ADD_INSC("解");

	/* Resistance */
	if (have_flag_of(flag_insc_resistance, flgs))
	{
		if (kanji)
			ADD_INSC("r");
		else if (ptr != prev_ptr)
		{
			ADD_INSC(";r");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_resistance, flgs, kanji, ptr);

	/* Misc Ability */
	if (have_flag_of(flag_insc_misc, flgs))
	{
		if (ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_misc, flgs, kanji, ptr);

	if (o_ptr->name2 == EGO_BODY_POVERTY) ADD_INSC(_(";貧", ";Pv"));

	/* Aura */
	if (have_flag_of(flag_insc_aura, flgs))
	{
		ADD_INSC("[");
	}
	ptr = inscribe_flags_aux(flag_insc_aura, flgs, kanji, ptr);

	/*:::Hack *切れ味*のときに|切と表示できるようにしてみた*/
	if(have_flag(flgs, TR_EX_VORPAL)) ADD_INSC(_("|X切", "|XSh"));

	/* Brand Weapon */
	if (have_flag_of(flag_insc_brand, flgs))
		ADD_INSC("|");
	ptr = inscribe_flags_aux(flag_insc_brand, flgs, kanji, ptr);

	/* Kill Weapon */
	if (have_flag_of(flag_insc_kill, flgs))
		ADD_INSC("/X");
	ptr = inscribe_flags_aux(flag_insc_kill, flgs, kanji, ptr);

	/* Slay Weapon */
	if (have_flag_of(flag_insc_slay, flgs))
		ADD_INSC("/");
	ptr = inscribe_flags_aux(flag_insc_slay, flgs, kanji, ptr);

	/* Esp */
	if (kanji)
	{
		if (have_flag_of(flag_insc_esp1, flgs) ||
		    have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}
	else
	{
		if (have_flag_of(flag_insc_esp1, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		if (have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}

	/* sustain */
	if (have_flag_of(flag_insc_sust, flgs))
	{
		ADD_INSC("(");
	}
	ptr = inscribe_flags_aux(flag_insc_sust, flgs, kanji, ptr);

	*ptr = '\0';

	return ptr;
}


/*
 *  Get object inscription with auto inscription of object flags.
 */
static void get_inscription(char *buff, object_type *o_ptr)
{
	cptr insc = quark_str(o_ptr->inscription);
	char *ptr = buff;

	/* Not fully identified */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		/* Copy until end of line or '#' */
		while (*insc)
		{
			if (*insc == '#') break;
#ifdef JP
			if (iskanji(*insc)) *buff++ = *insc++;
#endif
			*buff++ = *insc++;
		}

		*buff = '\0';
		return;
	}

	*buff = '\0';
	for (; *insc; insc++)
	{
		/* Ignore fake artifact inscription */
		if (*insc == '#') break;

		/* {%} will be automatically converted */
		else if ('%' == *insc)
		{
			bool kanji = FALSE;
			bool all;
			cptr start = ptr;

			/* check for too long inscription */
			if (ptr >= buff + MAX_NLEN) continue;

#ifdef JP
			if ('%' == insc[1])
			{
				insc++;
				kanji = FALSE;
			}
			else
			{
				kanji = TRUE;
			}
#endif
			if ('a' == insc[1] && 'l' == insc[2] && 'l' == insc[3])
			{
				all = TRUE;
				insc += 3;
			}
			else
			{
				all = FALSE;
			}

			ptr = get_ability_abbreviation(ptr, o_ptr, kanji, all);

			if (ptr == start)
				ADD_INSC(" ");
		}
		else
		{
			*ptr++ = *insc;
		}
	}
	*ptr = '\0';
}

///mod151031 宝物庫用に武器のダイスを非表示にするルーチンだったが、
//宝物庫再実装のため書き換えた
bool object_is_quest_target(object_type *o_ptr)
{

	if(quest[p_ptr->inside_quest].type != QUEST_TYPE_FIND_ARTIFACT
		&& quest[p_ptr->inside_quest].type != QUEST_TYPE_FIND_RANDOM_ARTIFACT)
	return FALSE;
	//★がターゲットのとき★と同じ種類の品がTRUE
	if(quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_ARTIFACT)
	{
		int a_idx = quest[p_ptr->inside_quest].k_idx;
		if (a_idx)
		{
			artifact_type *a_ptr = &a_info[a_idx];
			if (!(a_ptr->gen_flags & TRG_INSTA_ART))
			{
				if((o_ptr->tval == a_ptr->tval) && (o_ptr->sval == a_ptr->sval))
				{
					return TRUE;
				}
			}
		}
	}
	//☆がターゲットのとき
	else
	{
		int k_idx = quest[p_ptr->inside_quest].k_idx;
		if (k_idx)
		{
			if(k_info[k_idx].tval == o_ptr->tval && k_info[k_idx].sval == o_ptr->sval)
				return TRUE;
		}
	}
/*
	if (p_ptr->inside_quest)
	{
		int a_idx = quest[p_ptr->inside_quest].k_idx;
		if (a_idx)
		{
			artifact_type *a_ptr = &a_info[a_idx];
			if (!(a_ptr->gen_flags & TRG_INSTA_ART))
			{
				if((o_ptr->tval == a_ptr->tval) && (o_ptr->sval == a_ptr->sval))
				{
					return TRUE;
				}
			}
		}
	}
*/
	return FALSE;
}

/*:::武器の投擲ダメージを文字列にして返す*/
///mod140930 武器投擲ダメージ
cptr print_throw_dam(object_type *o_ptr)
{
	int mult = 1; //通常投擲を想定
	int use = 100; //energy_use
	bool suitable_item = FALSE;
	u32b flgs[TR_FLAG_SIZE];
	int dam;
	int dd,ds;
	int turn_dam;

	//オプションOFFのとき
	if(!show_throwing_dam) return "";

	//v2.1.4 ユイマンの弓非装備時には矢に対しても投擲ダメージを表示
	if (p_ptr->pclass == CLASS_YUIMAN && !object_is_shooting_weapon(&inventory[INVEN_RARM]) && !object_is_shooting_weapon(&inventory[INVEN_LARM]))
	{
		if (!object_is_melee_weapon(o_ptr) && !object_is_ammo(o_ptr)) return "";
	}
	//それ以外は武器のみが対象
	else
	{
		if (!object_is_melee_weapon(o_ptr)) return "";
	}
	//特殊変身中は投擲不可
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE) return " (0/0)";
	//未鑑定だと威力不明 (ただし鑑定済みでないとこのルーチンが呼ばれない)
	if(!object_is_known(o_ptr)) return " (?/?)";

	object_flags(o_ptr, flgs);
	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;

	//v2.1.4 ユイマンが矢を投擲するときボーナス
	if (p_ptr->pclass == CLASS_YUIMAN && object_is_ammo(o_ptr)) suitable_item = TRUE;

	if (p_ptr->mighty_throw) mult += 2;

	if(ref_skill_exp(SKILL_THROWING) > 1600)
	{
		if(suitable_item)	use -= (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
		else use -= (ref_skill_exp(SKILL_THROWING) -1600) / 128;
		if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && p_ptr->pclass == CLASS_REIMU)
			use -= osaisen_rank() * 2;
	}
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) use -= use / 5;

	if(use < 20) use = 20;

	dd = o_ptr->dd;
	ds = o_ptr->ds;

	if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE
		&&(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_SANAE) )
	{
		dd += 3;
	}
	//v1.1.21 兵士短剣技能
	else if ((o_ptr->tval == TV_KNIFE) && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_KNIFE_MASTERY))
	{
		dd += 2;
	}


	if(suitable_item && object_is_artifact(o_ptr)) dd *= 2;
	dam = dd * (ds + 1) / 2; //ダメージダイス期待値
	if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
		dam = dam * 7 / 2; //理力

	if (o_ptr->to_d > 0) dam += o_ptr->to_d;
	else	dam += -o_ptr->to_d;

	if (suitable_item)
	{
		dam *= (2+mult);
		dam += p_ptr->to_d_m;
	}
	else
	{
		dam *= mult;
	}

	if(dam<0) dam = 0; //Paranoia

	turn_dam = dam * 100 / use;

	return format(" (%d/%d)",dam,turn_dam);

}


/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be MAX_NLEN chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under MAX_NLEN-1 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * Mode:
 *   OD_NAME_ONLY        : The Cloak of Death
 *   OD_NAME_AND_ENCHANT : The Cloak of Death [1,+3]
 *   OD_OMIT_INSCRIPTION : The Cloak of Death [1,+3] (+2 to Stealth)
 *   0                   : The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 *
 *   OD_OMIT_PREFIX      : Forbidden numeric prefix
 *   OD_NO_PLURAL        : Forbidden use of plural
 *   OD_STORE            : Assume to be aware and known
 *   OD_NO_FLAVOR        : Allow to hidden flavor
 *   OD_FORCE_FLAVOR     : Get un-shuffled flavor name
 */
/*:::アイテムについて記述する パラメータ、銘、未鑑定名など関連*/
///item ego アイテムについての記述
///mod131223 TVAL/SVAL
void object_desc(char *buf, object_type *o_ptr, u32b mode)
{
	/* Extract object kind name */
	cptr            kindname = k_name + k_info[o_ptr->k_idx].name;

	/* Extract default "base" string */
	cptr            basenm = kindname;

	/* Assume no "modifier" string */
	cptr            modstr = "";

	int             power;

	bool            aware = FALSE;
	bool            known = FALSE;
	bool            flavor = TRUE;

	bool            show_weapon = FALSE;
	bool            show_armour = FALSE;

	//装備品ではないがACやto_hなどにパラメータが設定されている特殊なアイテム
	bool			flag_not_equipment = FALSE;

	bool            ego_prefix = FALSE;

	cptr            s, s0;
	char            *t;

	char            p1 = '(', p2 = ')';
	char            b1 = '[', b2 = ']';
	char            c1 = '{', c2 = '}';

	char            tmp_val[MAX_NLEN+160];
	char            tmp_val2[MAX_NLEN+10];
	char            fake_insc_buf[30];

	u32b flgs[TR_FLAG_SIZE];

	object_type *bow_ptr;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	object_kind *flavor_k_ptr = &k_info[k_ptr->flavor];

	int dummy;

	/* Extract some flags */
	object_flags(o_ptr, flgs);

	/* See if the object is "aware" */
	if (object_is_aware(o_ptr)) aware = TRUE;

	/* See if the object is "known" */
	if (object_is_known(o_ptr)) known = TRUE;

	/* Allow flavors to be hidden when aware */
	if (aware && ((mode & OD_NO_FLAVOR) || plain_descriptions)) flavor = FALSE;

	/* Object is in the inventory of a store or spoiler */
	if ((mode & OD_STORE) || (o_ptr->ident & IDENT_STORE))
	{
		/* Don't show flavors */
		flavor = FALSE;

		/* Pretend known and aware */
		aware = TRUE;
		known = TRUE;
	}

	/* Force to be flavor name only */
	if (mode & OD_FORCE_FLAVOR)
	{
		aware = FALSE;
		flavor = TRUE;
		known = FALSE;

		/* Cancel shuffling */
		flavor_k_ptr = k_ptr;
	}

	/* Analyze the object */
	///item res TVAL
	switch (o_ptr->tval)
	{
		/* Some objects are easy to describe */

		case TV_FLASK:
		case TV_CHEST:
		case TV_WHISTLE:
		case TV_BUNBUN:
		case TV_KAKASHI:
		case TV_SOUVENIR:
		case TV_COMPOUND:
		case TV_MATERIAL:
		case TV_SOFTDRINK:
		case TV_ALCOHOL:
		case TV_SWEETS:
		case TV_FOOD:
		case TV_BOOK_ELEMENT:
		case TV_BOOK_FORESEE:
		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:
		case TV_BOOK_MYSTIC:
		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:
		case TV_BOOK_NATURE:
		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:
		case TV_BOOK_NECROMANCY:
		case TV_BOOK_CHAOS:
  		case TV_BOOK_HISSATSU:
		case TV_BOOK_OCCULT:
	  	case TV_BOOK_MEDICINE:
			///item いずれ適切な場所に動かす予定
		case TV_MAGICITEM:
		case TV_MAGICWEAPON:
		case TV_MACHINE:
		case TV_RIBBON:
		case TV_ITEMCARD:
		case TV_STRANGE_OBJ:
		case TV_SPELLCARD:
		case TV_ABILITY_CARD:
		case TV_STONE_INCIDENT:

		{
			break;
		}
		case TV_CAPTURE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

			if (known)
			{
				if (!o_ptr->pval)
				{
#ifdef JP
					modstr = " (空)";
#else
					modstr = " (empty)";
#endif
				}
				//v1.1.16 妖魔本バグによるセーブファイル読み込み不能に対応
				else if(o_ptr->pval < 0 || o_ptr->pval >= max_r_idx)
				{
					sprintf(tmp_val2, _(" (ERROR:妖魔本のr_idx値が不正(%d))",
                                        " (ERROR: incorrect r_idx value (%d) for demon book"),o_ptr->pval);
					modstr = tmp_val2;

				}
				else
				{
#ifdef JP
					sprintf(tmp_val2, " (%s)",r_name + r_ptr->name);
					modstr = tmp_val2;
#else
					cptr t = r_name + r_ptr->name;

					if (!(r_ptr->flags1 & RF1_UNIQUE))
					{
						sprintf(tmp_val2, " (%s%s)", (is_a_vowel(*t) ? "an " : "a "), t);

						modstr = tmp_val2;
					}
					else
					{
						sprintf(tmp_val2, "(%s)", t);

						modstr = t;
					}
#endif
				}
			}
			break;
		}
		//v1.1.60
		case TV_ANTIQUE:

			break;

		/* Figurines/Statues */
		case TV_FIGURINE:
		case TV_PHOTO:

			///del131221 像
		//case TV_STATUE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

#ifdef JP
			modstr = r_name + r_ptr->name;
#else
			cptr t = r_name + r_ptr->name;

			if (!(r_ptr->flags1 & RF1_UNIQUE))
			{
				sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "), t);

				modstr = tmp_val2;
			}
			else
			{
				modstr = t;
			}
#endif


			break;
		}


///del131221 死体

#if 0
		/* Corpses */
		case TV_CORPSE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

			modstr = r_name + r_ptr->name;

#ifdef JP
			basenm = "#%";
#else
			if (r_ptr->flags1 & RF1_UNIQUE)
				basenm = "& % of #";
			else
				basenm = "& # %";
#endif

			break;
		}
#endif

		/* Missiles/ Bows/ Weapons */
		case TV_BULLET:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_CROSSBOW:
		case TV_GUN:
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:
		case TV_OTHERWEAPON:
			{
			show_weapon = TRUE;
			break;
		}

		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_HEAD:
		case TV_SHIELD:
		case TV_CLOTHES:
		case TV_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_MASK:
		{
			show_armour = TRUE;
			break;
		}

		/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

		/* Amulets (including a few "Specials") */
		///item 未鑑定時と鑑定後で表記の全く違うアイテムはこんな感じ
		case TV_AMULET:
		{
			/* Known artifacts */
			if (aware)
			{
				if (object_is_fixed_artifact(o_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;


			//v1.1.12 博麗アミュレット追加。ベース名に「の」が付かない
#ifdef JP
			if(o_ptr->sval == SV_AMULET_HAKUREI)
			{
				if (!flavor)    basenm = "%アミュレット";
				else if (aware) basenm = "%#アミュレット";
				else	basenm = "#アミュレット";
			}
			else
			{
				if (!flavor)    basenm = "%のアミュレット";
				else if (aware) basenm = "%の#アミュレット";
				else            basenm = "#アミュレット";
			}
#else
            if(o_ptr->sval == SV_AMULET_HAKUREI)
			{
				if (!flavor)    basenm = "% Amulet~";
				else if (aware) basenm = "% # Amulet~";
				else	basenm = "# Amulet~";
			}
			else
			{
				if (!flavor)    basenm = "Amulet~ of %";
				else if (aware) basenm = "# Amulet~ of %";
				else            basenm = "# Amulet~";
			}
#endif


			break;
		}

		/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if (aware)
			{
				if (object_is_fixed_artifact(o_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%の指輪";
			else if (aware) basenm = "%の#指輪";
			else            basenm = "#指輪";
#else
			if (!flavor)    basenm = "& Ring~ of %";
			else if (aware) basenm = "& # Ring~ of %";
			else            basenm = "& # Ring~";
#endif


			if (!k_ptr->to_h && !k_ptr->to_d && (o_ptr->to_h || o_ptr->to_d)) show_weapon = TRUE;



			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%の杖";
			else if (aware) basenm = "%の#杖";
			else            basenm = "#杖";
#else
			if (!flavor)    basenm = "& Staff~ of %";
			else if (aware) basenm = "& # Staff~ of %";
			else            basenm = "& # Staff~";
#endif

			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%の魔法棒";
			else if (aware) basenm = "%の#魔法棒";
			else            basenm = "#魔法棒";
#else
			if (!flavor)    basenm = "& Wand~ of %";
			else if (aware) basenm = "& # Wand~ of %";
			else            basenm = "& # Wand~";
#endif

			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%のロッド";
			else if (aware) basenm = "%の#ロッド";
			else            basenm = "#ロッド";
#else
			if (!flavor)    basenm = "& Rod~ of %";
			else if (aware) basenm = "& # Rod~ of %";
			else            basenm = "& # Rod~";
#endif

			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%の巻物";
			else if (aware) basenm = "「#」と書かれた%の巻物";
			else            basenm = "「#」と書かれた巻物";
#else
			if (!flavor)    basenm = "& Scroll~ of %";
			else if (aware) basenm = "& Scroll~ titled \"#\" of %";
			else            basenm = "& Scroll~ titled \"#\"";
#endif

			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%の薬";
			else if (aware) basenm = "%の#薬";
			else            basenm = "#薬";

			//なにか涼しくなる薬は「の」を除く
			//惚れ薬も
			//ヒーローの薬→抗鬱薬
			if(o_ptr->sval == SV_POTION_NANIKASUZUSHIKUNARU || o_ptr->sval == SV_POTION_LOVE || o_ptr->sval == SV_POTION_HEROISM)
			{
				if (!flavor)    basenm = "%薬";
				else if (aware) basenm = "%#薬";
				else	basenm = "#薬";
			}
#else
			if (!flavor)    basenm = "& Potion~ of %";
			else if (aware) basenm = "& # Potion~ of %";
			else            basenm = "& # Potion~";

			// Special cases for some potions
			if (!flavor || aware) {
                if (o_ptr->sval == SV_POTION_INVULNERABILITY)
                {
                    basenm = "& Ultramarine Orb Elixir~";
                } else if (o_ptr->sval == SV_POTION_NANIKASUZUSHIKUNARU) {
                    basenm = "& Bottle~ of Cooling Medicine";
                }
			}
#endif

			break;
		}

		case TV_MUSHROOM:
		{
			/* Ordinary food is "boring" */
			if (!k_ptr->flavor_name) break;

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%のキノコ";
			else if (aware) basenm = "%の#キノコ";
			else            basenm = "#キノコ";
#else
			if (!flavor)    basenm = "& Mushroom~ of %";
			else if (aware) basenm = "& # Mushroom~ of %";
			else            basenm = "& # Mushroom~";
#endif

			break;
		}
#if 0
		/* Magic Books */
		case TV_LIFE_BOOK:
		{
#ifdef JP
			basenm = "生命の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Life Magic %";
			else
				basenm = "& Life Spellbook~ %";
#endif

			break;
		}

		case TV_SORCERY_BOOK:
		{
#ifdef JP
			basenm = "仙術の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Sorcery %";
			else
				basenm = "& Sorcery Spellbook~ %";
#endif

			break;
		}

		case TV_NATURE_BOOK:
		{
#ifdef JP
			basenm = "自然の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Nature Magic %";
			else
				basenm = "& Nature Spellbook~ %";
#endif

			break;
		}

		case TV_CHAOS_BOOK:
		{
#ifdef JP
			basenm = "カオスの魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Chaos Magic %";
			else
				basenm = "& Chaos Spellbook~ %";
#endif

			break;
		}

		case TV_DEATH_BOOK:
		{
#ifdef JP
			basenm = "暗黒の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Death Magic %";
			else
				basenm = "& Death Spellbook~ %";
#endif

			break;
		}

		case TV_TRUMP_BOOK:
		{
#ifdef JP
			basenm = "トランプの魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Trump Magic %";
			else
				basenm = "& Trump Spellbook~ %";
#endif

			break;
		}

		case TV_ARCANE_BOOK:
		{
#ifdef JP
			basenm = "秘術の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Arcane Magic %";
			else
				basenm = "& Arcane Spellbook~ %";
#endif

			break;
		}

		case TV_CRAFT_BOOK:
		{
#ifdef JP
			basenm = "匠の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Craft Magic %";
			else
				basenm = "& Craft Spellbook~ %";
#endif

			break;
		}

		case TV_DAEMON_BOOK:
		{
#ifdef JP
			basenm = "悪魔の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Daemon Magic %";
			else
				basenm = "& Daemon Spellbook~ %";
#endif

			break;
		}

		case TV_CRUSADE_BOOK:
		{
#ifdef JP
			basenm = "破邪の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Crusade Magic %";
			else
				basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}

		case TV_MUSIC_BOOK:
		{
#ifdef JP
			basenm = "歌集%";
#else
			basenm = "& Song Book~ %";
#endif

			break;
		}

		case TV_HISSATSU_BOOK:
		{
#ifdef JP
			basenm = "& 武芸の書%";
#else
			basenm = "Book~ of Kendo %";
#endif

			break;
		}

		case TV_HEX_BOOK:
		{
#ifdef JP
			basenm = "呪術の魔法書%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Crusade Magic %";
			else
				basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}
#endif
		/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

		/* Used in the "inventory" routine */
		default:
		{
#ifdef JP
			strcpy(buf, "(なし)");
#else
			strcpy(buf, "(nothing)");
#endif

			return;
		}
	}

    // Confirm if the ego-item name is a prefix
	if (object_is_ego(o_ptr)) {
        ego_item_type *e_ptr = &e_info[o_ptr->name2];
        if (my_strchr(e_name + e_ptr->name, '@')) {
            ego_prefix = TRUE;
        }
	}

	/* Use full name from k_info or a_info */
	if (aware && have_flag(flgs, TR_FULL_NAME))
	{
		if (known && o_ptr->name1) basenm = a_name + a_info[o_ptr->name1].name;
		else basenm = kindname;
	}

	/* Start dumping the result */
	t = tmp_val;

#ifdef JP
	if (basenm[0] == '&')
		s = basenm + 2;
	else
		s = basenm;

	/* No prefix */
	if (mode & OD_OMIT_PREFIX)
	{
		/* Nothing */
	}
	else if (o_ptr->number > 1)
	{
		t = object_desc_kosuu(t, o_ptr);
		t = object_desc_str(t, "の ");
	}

	/* 英語の場合アーティファクトは The が付くので分かるが
	 * 日本語では分からないのでマークをつける
	 */
	 //v1.1.48 紫苑の差し押さえられたアイテム
	if (p_ptr->pclass == CLASS_SHION && o_ptr->tval >= TV_EQUIP_BEGIN && o_ptr->tval <= TV_EQUIP_END && o_ptr->xtra5)
	{
		t = object_desc_str(t, "差押えられた");

	}
	/*:::Hack-小傘特殊表記*/
	if (known)
	{
		if (object_is_fixed_artifact(o_ptr)) t = object_desc_str(t, "★");
		//八雲紫の日傘とベースアイテムを共用したら不具合出たので重量で区別
		else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30) t = object_desc_str(t, "◆");
		else if (o_ptr->art_name)
		{
			if(o_ptr->tval == TV_MASK)
				t = object_desc_str(t, "○");
			else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE); //弾幕夢の写真には☆をつけない
			else if (o_ptr->tval == TV_ABILITY_CARD); //v1.1.86 アビリティカードにも☆をつけない
			else
				t = object_desc_str(t, "☆");
		}
	}

#else

#ifndef JP
    if (object_is_smith(o_ptr))
    {
        if(o_ptr->xtra3 == SPECIAL_ESSENCE_ONI)//武器修復品
            t = object_desc_str(t, _("◇", "<> "));
        else //鍛冶品
            //	t = object_desc_str(t, format("鍛冶師%sの", player_name));
            t = object_desc_str(t, format(_("◇", "<> ")));
    }
#endif


	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		if (p_ptr->pclass == CLASS_SHION && o_ptr->tval >= TV_EQUIP_BEGIN && o_ptr->tval <= TV_EQUIP_END && o_ptr->xtra5)
        {
            t = object_desc_str(t, "(Seized) ");
        }

		/* No prefix */
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (object_is_artifact(o_ptr) || o_ptr->art_name))

		//else if ((known && object_is_artifact(o_ptr)) ||
		        // ((o_ptr->tval == TV_CORPSE) &&
		        //  (r_info[o_ptr->pval].flags1 & RF1_UNIQUE)))

		{
			//t = object_desc_str(t, "The ");
			if (object_is_fixed_artifact(o_ptr)) t = object_desc_str(t, "* ");
            //八雲紫の日傘とベースアイテムを共用したら不具合出たので重量で区別
            else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30) t = object_desc_str(t, "<> ");
            else if (o_ptr->art_name)
            {
                if(o_ptr->tval == TV_MASK)
                    t = object_desc_str(t, "() ");
                else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE); //弾幕夢の写真には☆をつけない
                else if (o_ptr->tval == TV_ABILITY_CARD); //v1.1.86 アビリティカードにも☆をつけない
                else
                    t = object_desc_str(t, "X ");
            }
        }

		/* A single one */
		else
		{
			bool vowel;

			switch (*s)
			{
			case '#': vowel = is_a_vowel(modstr[0]); break;
			case '%': vowel = is_a_vowel(*kindname); break;
			default:  vowel = is_a_vowel(*s); break;
			}

			if (ego_prefix == TRUE) {
                ego_item_type *e_ptr = &e_info[o_ptr->name2];
                vowel = is_a_vowel(*((e_name + e_ptr->name) + 1));
			}

			if (vowel)
			{
				/* A single one, with a vowel */
				t = object_desc_str(t, "an ");
			}
			else
			{
				/* A single one, without a vowel */
				t = object_desc_str(t, "a ");
			}
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (object_is_artifact(o_ptr) || o_ptr->art_name))

		//else if ((known && object_is_artifact(o_ptr)) ||
		        // ((o_ptr->tval == TV_CORPSE) &&
		        //  (r_info[o_ptr->pval].flags1 & RF1_UNIQUE)))

		{
			//t = object_desc_str(t, "The ");
			if (object_is_fixed_artifact(o_ptr)) t = object_desc_str(t, "* ");
            //八雲紫の日傘とベースアイテムを共用したら不具合出たので重量で区別
            else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30) t = object_desc_str(t, "<> ");
            else if (o_ptr->art_name)
            {
                if(o_ptr->tval == TV_MASK)
                    t = object_desc_str(t, "() ");
                else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE); //弾幕夢の写真には☆をつけない
                else if (o_ptr->tval == TV_ABILITY_CARD); //v1.1.86 アビリティカードにも☆をつけない
                else
                    t = object_desc_str(t, "X ");
            }
        }

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}
#endif

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

	if (object_is_smith(o_ptr))
	{
		if(o_ptr->xtra3 == SPECIAL_ESSENCE_ONI)//武器修復品
			t = object_desc_str(t, _("◇", ""));
		else if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)//付喪神使いの妖器
		{
			if(o_ptr->xtra1 >= 100)	t = object_desc_str(t, _("(*妖器*)", "(*Youkai*)"));
			else	t = object_desc_str(t, _("(妖器)","(Youkai)"));
		}
		else //鍛冶品
		//	t = object_desc_str(t, format("鍛冶師%sの", player_name));
			t = object_desc_str(t, format(_("◇", "")));

	}

	//勾玉には素材名を付加
	//o_ptr->xtra6に素材k_idxが記録されている
	if (o_ptr->tval == TV_SPELLCARD && o_ptr->sval == SV_SPELLCARD_MAGATAMA)
	{
		t = object_desc_str(t, format(_("%sの", "%s "),k_name+k_info[o_ptr->xtra6].name));

	}

	/* 伝説のアイテム、名のあるアイテムの名前を付加する */
#ifdef JP
	if (known)
	{
		/* ランダム・アーティファクト */
		if (o_ptr->art_name)
		{
			cptr temp = quark_str(o_ptr->art_name);

			/* '『' から始まらない伝説のアイテムの名前は最初に付加する */
			/* 英語版のセーブファイルから来た 'of XXX' は,「XXXの」と表示する */
			///mod141206 こころの面は特殊
			if ( o_ptr->tval == TV_MASK)
			{
				t = object_desc_str(t, temp);
			}
			else if (o_ptr->tval == TV_SPELLCARD)
			{
				;
			}
			else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 新アリーナ写真
			{
				t = object_desc_str(t, "弾幕夢");
				t = object_desc_str(t, temp);
				t = object_desc_str(t, "の");
			}
			else if (strncmp(temp, "of ", 3) == 0)
			{
				t = object_desc_str(t, &temp[3]);
				t = object_desc_str(t, "の");
			}
			else if ((strncmp(temp, "『", 2) != 0) &&
				 (strncmp(temp, "《", 2) != 0) &&
				(temp[0] != '\''))
				t = object_desc_str(t, temp);
		}
		/* 伝説のアイテム */
		else if (o_ptr->name1 && !have_flag(flgs, TR_FULL_NAME))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];
			/* '『' から始まらない伝説のアイテムの名前は最初に付加する */
			if (strncmp(a_name + a_ptr->name, "『", 2) != 0)
			{
				t = object_desc_str(t, a_name + a_ptr->name);
			}
		}
		/* 名のあるアイテム */
		else if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];
			t = object_desc_str(t, e_name + e_ptr->name);
		}
	}
#else
    if (known) {
        if (o_ptr->art_name) {
            cptr temp = quark_str(o_ptr->art_name);
            if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 新アリーナ写真
            {
                t = object_desc_str(t, "Danmaku Dream ");
                t = object_desc_str(t, temp);
                t = object_desc_str(t, " ");
            }
        }
        // If an ego-item name starts with '@', it's a prefix
        // Put it before item name
        else if (object_is_ego(o_ptr) && ego_prefix == TRUE) {
            ego_item_type *e_ptr = &e_info[o_ptr->name2];
			t = object_desc_str(t, (e_name + e_ptr->name) + 1);
			t = object_desc_str(t, " ");
        }
    }
#endif

	/* Copy the string */
	for (s0 = NULL; *s || s0; )
	{
		/* The end of the flavour/kind string. */
		if (!*s)
		{
			s = s0 + 1;
			s0 = NULL;
		}

		/* Begin to append the modifier (flavor) */
		else if ((*s == '#') && !s0)
		{
			s0 = s;
			s = modstr;

			/* Paranoia -- Never append multiple modstrs */
			modstr = "";
		}

		/* Begin to append the kind name */
		else if ((*s == '%') && !s0)
		{
			s0 = s;
			s = kindname;

			/* Paranoia -- Never append multiple kindnames */
			kindname = "";
		}

#ifndef JP
		/* Pluralizer */
		else if (*s == '~')
		{
			/* Add a plural if needed */
			if (!(mode & OD_NO_PLURAL) && (o_ptr->number != 1))
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h')) *t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
			s++;
		}
#endif

		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s++;
		}
	}

	/* Terminate */
	*t = '\0';


#ifdef JP
	/* '『'から始まる伝説のアイテムの名前は最後に付加する */
	if (known)
	{
		/* ランダムアーティファクトの名前はセーブファイルに記録
		   されるので、英語版の名前もそれらしく変換する */
		if (o_ptr->art_name)
		{
			char temp[256];
			int itemp;
			strcpy(temp, quark_str(o_ptr->art_name));


			if ( o_ptr->tval == TV_MASK)
			{
				//t = object_desc_str(t, temp);
			}
			else if (o_ptr->tval == TV_SPELLCARD && o_ptr->sval == SV_SPELLCARD_CARD)
			{
				t = object_desc_str(t, "【");
				t = object_desc_str(t, temp);
				t = object_desc_str(t, "】");

			}
			/* MEGA HACK by ita */
			else if (strncmp(temp, "『", 2) == 0 ||
			    strncmp(temp, "《", 2) == 0)
				t = object_desc_str(t, temp);
			else if (temp[0] == '\'')
			{
				itemp = strlen(temp);
				temp[itemp - 1] = 0;
				t = object_desc_str(t, "『");
				t = object_desc_str(t, &temp[1]);
				t = object_desc_str(t, "』");
			}
		}
		else if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];
			if (strncmp(a_name + a_ptr->name, "『", 2) == 0)
			{
				t = object_desc_str(t, a_name + a_ptr->name);
			}
		}
		else if (o_ptr->inscription)
		{
			cptr str = quark_str(o_ptr->inscription);

			while(*str)
			{
				if (iskanji(*str))
				{
					str += 2;
					continue;
				}
				if (*str == '#') break;
				str++;
			}
			if (*str)
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(o_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_str(t,"『");
				t = object_desc_str(t, &str[1]);
				t = object_desc_str(t,"』");
			}
		}
	}




	//v1.1.85 妖魔本以外のモンスター捕獲アイテム
	if (activation_index(o_ptr) == ACT_ART_CAPTURE)
	{
		monster_race *r_ptr = &r_info[o_ptr->xtra6];
		if (known)
		{
			if (!o_ptr->xtra6)
			{
				t = object_desc_str(t, "(空)");
			}
			//v1.1.16 妖魔本バグによるセーブファイル読み込み不能に対応
			else if (o_ptr->xtra6 < 0 || o_ptr->xtra6 >= max_r_idx)
			{
				sprintf(tmp_val2, " (ERROR:アイテム内のr_idx値が不正(%d))", o_ptr->xtra6);
				t = object_desc_str(t, tmp_val2);

			}
			else
			{
				sprintf(tmp_val2, " (%s)", r_name + r_ptr->name);
				t = object_desc_str(t, tmp_val2);

			}
		}
	}


#else
	/* Hack -- Append "Artifact" or "Special" names */
	if (known && !have_flag(flgs, TR_FULL_NAME))
	{
		/* Is it a new random artifact ? */
		if (o_ptr->art_name)
		{
		    char temp[256];
			int itemp;
			strcpy(temp, quark_str(o_ptr->art_name));

		    if (o_ptr->tval == TV_MASK) {
                t = object_desc_str(t, " of ");
                t = object_desc_str(t, temp);
            } else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE) ;
		    else {
                t = object_desc_chr(t, ' ');
                t = object_desc_str(t, quark_str(o_ptr->art_name));
            }
		}

		/* Grab any artifact name */
		else if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, a_name + a_ptr->name);
		}

		/* Grab any ego-item name */
		else
		{
		    // Do not add ego-item name after the main object name if it was a prefix
			if (object_is_ego(o_ptr) && ego_prefix != TRUE)
			{
				ego_item_type *e_ptr = &e_info[o_ptr->name2];

				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, e_name + e_ptr->name);
			}

			if (o_ptr->inscription && my_strchr(quark_str(o_ptr->inscription), '#'))
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(o_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, &str[1]);
			}
		}
	}
#endif


	/* No more details wanted */
	if (mode & OD_NAME_ONLY) goto object_desc_done;

	//スペカのpvalとか殺戮修正は表示しない
	if(o_ptr->tval == TV_SPELLCARD && o_ptr->sval == SV_SPELLCARD_CARD)
		goto object_desc_done;

	//特殊収集アイテムのpvalとか殺戮修正は表示しない.最後に価格だけ表示する
	if (o_ptr->tval == TV_ANTIQUE)
	{

		sprintf(tmp_val2, " ($%d)", o_ptr->score_value);

		t = object_desc_str(t, tmp_val2);
		goto object_desc_done;
	}

	///mod151031 宝物庫のターゲットアイテムは名前のみ表示にした
	if(object_is_quest_target(o_ptr) && !known)
	{
		goto object_desc_done;
	}


	/* Hack -- Chests must be described in detail */
	if (o_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!o_ptr->pval)
		{
#ifdef JP
			t = object_desc_str(t, "(空)");
#else
			t = object_desc_str(t, " (empty)");
#endif
		}

		/* May be "disarmed" */
		else if (o_ptr->pval < 0)
		{
			if (chest_traps[0 - o_ptr->pval])
			{
#ifdef JP
				t = object_desc_str(t, "(解除済)");
#else
				t = object_desc_str(t, " (disarmed)");
#endif
			}
			else
			{
#ifdef JP
				t = object_desc_str(t, "(非施錠)");
#else
				t = object_desc_str(t, " (unlocked)");
#endif
			}
		}
		else if (o_ptr->pval >= CHEST_TRAP_LIST_LENGTH)
		{
			t = object_desc_str(t, "(ERROR)");
		}
		/* Describe the traps, if any */
		else
		{
			//v1.1.97 箱トラップ内容を一新

//			switch (chest_traps[o_ptr->pval])
			switch (chest_new_traps[o_ptr->pval])
			{


				case CHEST_TRAP_NOTHING:
				{
#ifdef JP
					t = object_desc_str(t, "(施錠)");
#else
					t = object_desc_str(t, " (Locked)");
#endif
					break;
				}
				case CHEST_TRAP_LOSE_STR:
					t = object_desc_str(t, _("(腕力低下ダーツ)", "(STR lowering dart)"));
					break;
				case CHEST_TRAP_LOSE_CON:
					t = object_desc_str(t, _("(耐久低下ダーツ)", "(CON lowering dart)"));
					break;
				case CHEST_TRAP_LOSE_MAG:
					t = object_desc_str(t, _("(知能低下ダーツ)", "(INT lowering dart)"));
					break;


				case CHEST_TRAP_BA_POIS:
					t = object_desc_str(t, _("(毒ガス)", "(Poison gas)"));
					break;
				case CHEST_TRAP_BA_SLEEP:
					t = object_desc_str(t, _("(睡眠ガス)", "(Sleeping gas)"));
					break;
				case CHEST_TRAP_BA_CONF:
					t = object_desc_str(t, _("(混乱ガス)", "(Confusion gas)"));
					break;
				case CHEST_TRAP_EXPLODE:
					t = object_desc_str(t, _("(爆弾)", "(Bomb)"));
					break;
				case CHEST_TRAP_ALARM:
					t = object_desc_str(t, _("(アラーム)", "(Alarm)"));
					break;
				case CHEST_TRAP_SUMMON:
					t = object_desc_str(t, _("(召喚のルーン)", "(Summoning runes)"));
					break;
				case CHEST_TRAP_S_BIRD:
					t = object_desc_str(t, _("(鳥召喚のルーン)", "(Bird summoning runes)"));
					break;

				case CHEST_TRAP_S_ELEMENTAL:
					t = object_desc_str(t, _("(精霊召喚のルーン)", "(Elemental summoning runes)"));
					break;
				case CHEST_TRAP_S_DEMON:
					t = object_desc_str(t, _("(悪魔召喚のルーン)", "(Demon summoning runes)"));
					break;

				case CHEST_TRAP_S_DRAGON:
					t = object_desc_str(t, _("(竜召喚のルーン)", "(Dragon summoning runes)"));
					break;
				case CHEST_TRAP_S_CHIMERA:
					t = object_desc_str(t, _("(巨獣召喚のルーン)", "(Beast summoning runes)"));
					break;
				case CHEST_TRAP_S_VORTEX:
					t = object_desc_str(t, _("(魔渦召喚のルーン)", "(Vortex summoning runes)"));
					break;
				case CHEST_TRAP_S_KWAI:
					t = object_desc_str(t, _("(妖怪召喚のルーン)", "(Youkai summoning runes)"));
					break;

				case CHEST_TRAP_SUIKI:
					t = object_desc_str(t, _("(水鬼鬼神長の罠)", "(Sui-Ki Kishin chief trap)"));
					break;

				case CHEST_TRAP_RUIN:
					t = object_desc_str(t, _("(破滅のトラップ)", "(Ruination trap)"));
					break;
				case CHEST_TRAP_SLINGSHOT:
					t = object_desc_str(t, _("(石つぶて)", "(Slingshot)"));
					break;
				case CHEST_TRAP_ARROW:
					t = object_desc_str(t, _("(矢)", "(Arrow)"));
					break;
				case CHEST_TRAP_STEEL_ARROW:
					t = object_desc_str(t, _("(鋼鉄の矢)", "(Steel arrow)"));
					break;
				case CHEST_TRAP_TELEPORTER:
					t = object_desc_str(t, _("(テレポーター)", "(Teleporter)"));
					break;
				case CHEST_TRAP_PUNCH:
					t = object_desc_str(t, _("(パンチ)", "(Punch)"));
					break;
				case CHEST_TRAP_BR_FIRE:
					t = object_desc_str(t, _("(火炎放射)", "(Flamethrower)"));
					break;
				case CHEST_TRAP_BR_ACID:
					t = object_desc_str(t, _("(酸の噴射)", "(Acid spray)"));
					break;
				case CHEST_TRAP_BA_TIME:
					t = object_desc_str(t, _("(逆玉手箱)", "(Time reversal)"));
					break;
				case CHEST_TRAP_MIMIC:
					t = object_desc_str(t, _("(ミミック)", "(Mimic)"));
					break;
				case CHEST_TRAP_MAGIC_DRAIN:
					t = object_desc_str(t, _("(魔力吸収)", "(Magic drain)"));
					break;
				case CHEST_TRAP_BA_BERSERK:
					t = object_desc_str(t, _("(狂化ガス)", "(Maddening gas)"));
					break;
				case CHEST_TRAP_FUSION:
					t = object_desc_str(t, _("(フュージョン)", "(Fusion)"));
					break;

				default:
				{
					t = object_desc_str(t, "(ERROR)");
					break;
				}

#if 0
				case CHEST_LOSE_STR:
				{
#ifdef JP
					t = object_desc_str(t, "(毒針)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_LOSE_CON:
				{
#ifdef JP
					t = object_desc_str(t, "(毒針)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_POISON:
				{
#ifdef JP
					t = object_desc_str(t, "(ガス・トラップ)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_PARALYZE:
				{
#ifdef JP
					t = object_desc_str(t, "(ガス・トラップ)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_EXPLODE:
				{
#ifdef JP
					t = object_desc_str(t, "(爆発装置)");
#else
					t = object_desc_str(t, " (Explosion Device)");
#endif
					break;
				}
				case CHEST_SUMMON:
				case CHEST_BIRD_STORM:
				case CHEST_E_SUMMON:
				case CHEST_H_SUMMON:
				{
#ifdef JP
					t = object_desc_str(t, "(召喚のルーン)");
#else
					t = object_desc_str(t, " (Summoning Runes)");
#endif
					break;
				}
				case CHEST_RUNES_OF_EVIL:
				{
#ifdef JP
					t = object_desc_str(t, "(邪悪なルーン)");
#else
					t = object_desc_str(t, " (Gleaming Black Runes)");
#endif
					break;
				}
				case CHEST_ALARM:
				{
#ifdef JP
					t = object_desc_str(t, "(警報装置)");
#else
					t = object_desc_str(t, " (Alarm)");
#endif
					break;
				}
				default:
				{
#ifdef JP
					t = object_desc_str(t, "(マルチ・トラップ)");
#else
					t = object_desc_str(t, " (Multiple Traps)");
#endif
					break;
				}
#endif

			}
		}
	}
	else if(o_ptr->tval == TV_BUNBUN)
	{
		if(show_special_info && (o_ptr->ident & IDENT_MENTAL))
			t = object_desc_str(t, format(_("・号外《其の%04d・%s》",
                                            " - Extra <That %04d - %s>"),o_ptr->xtra5,r_name+r_info[o_ptr->pval].name));
		else
			t = object_desc_str(t, format(_("・号外《其の%04d》",
                                            " - Extra <That %04d>"),o_ptr->xtra5));
	}
	else if(o_ptr->tval == TV_KAKASHI)
	{
		if(show_special_info && (o_ptr->ident & IDENT_MENTAL))
			t = object_desc_str(t, format(_("・特別特集＃%04d・%s",
                                            " - Special Feature #%04d - %s"),o_ptr->xtra5,r_name+r_info[o_ptr->pval].name));
		else
			t = object_desc_str(t, format(_("・特別特集＃%04d",
                                            " - Special Feature #%04d"),o_ptr->xtra5));
	}
	else if(o_ptr->tval == TV_ITEMCARD)
	{
		t = object_desc_str(t, format(_("『%s』", " '%s'"),support_item_list[o_ptr->pval].name));
	}
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
		{
			t = object_desc_str(t, format("(ERROR:idx%d)", o_ptr->pval));
		}
		else
		{
			t = object_desc_str(t, format(_("『%s』", "'%s'"), ability_card_list[o_ptr->pval].card_name));
		}
	}

	else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 新アリーナ写真 スコア入力
	{
		if (known)
		{
			char tmp_buf[32];

			int points = o_ptr->xtra4 * 10000 + o_ptr->xtra5;
			sprintf(tmp_buf, _("(%d points)", " (%d points)"), points);
			t = object_desc_str(t, tmp_buf);
		}
	}
	//勾玉に「その他の特殊な情報を表示する」オプションを付けているとき
	else if (o_ptr->tval == TV_SPELLCARD && o_ptr->sval == SV_SPELLCARD_MAGATAMA && show_special_info)
	{
		char tmp_buf[128];

		magatama_effect_desc(tmp_buf, o_ptr);

		t = object_desc_str(t, tmp_buf);

	}


	/* Display the item like a weapon */
	if (have_flag(flgs, TR_SHOW_MODS)) show_weapon = TRUE;

	/* Display the item like a weapon */
	if (object_is_smith(o_ptr) && (o_ptr->xtra3 == 1 + ESSENCE_SLAY_GLOVE))
		show_weapon = TRUE;

	/* Display the item like a weapon */
	if (o_ptr->to_h && o_ptr->to_d) show_weapon = TRUE;

	/* Display the item like armour */
	if (o_ptr->ac) show_armour = TRUE;

	//勾玉に殺戮修正やACを表示させない
	if (o_ptr->tval == TV_SPELLCARD)
	{
		flag_not_equipment = TRUE;
	}


	/* Dump base weapon info */
	switch (o_ptr->tval)
	{
		/* Missiles and Weapons */
		case TV_BULLET:
		case TV_BOLT:
		case TV_ARROW:
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:
		case TV_OTHERWEAPON:
		case TV_MAGICWEAPON:
		case TV_GUN:

		/* In Vault Quest, hide the dice of target weapon. */
			/*:::宝物庫ではダイスを表示しない*/
		///mod151031 宝物庫再実装　名前しか表示しないようにするため別の場所へ
		/*
		if(object_is_quest_target(o_ptr) && !known)
		{
			break;
		}
		*/
		///mod160505 未鑑定の銃はダイスが表示されない
		//if(o_ptr->tval == TV_GUN && !known) break;

		/* Append a "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		if(mode & OD_WEAPON_RARM) t = object_desc_num(t, o_ptr->dd + p_ptr->to_dd[0]);
		else if(mode & OD_WEAPON_LARM) t = object_desc_num(t, o_ptr->dd + p_ptr->to_dd[1]);
		else
			t = object_desc_num(t, o_ptr->dd);
		t = object_desc_chr(t, 'd');
		if(mode & OD_WEAPON_RARM) t = object_desc_num(t, o_ptr->ds + p_ptr->to_ds[0]);
		else if(mode & OD_WEAPON_LARM) t = object_desc_num(t, o_ptr->ds + p_ptr->to_ds[1]);
		else
			t = object_desc_num(t, o_ptr->ds);
		t = object_desc_chr(t, p2);

		/* All done */
		break;


		/* Bows get a special "damage string" */
		case TV_BOW:
		case TV_CROSSBOW:
		//case TV_GUN:

		/* Mega-Hack -- Extract the "base power" */
		power = bow_tmul(o_ptr->tval, o_ptr->sval);

		/* Apply the "Extra Might" flag */
		if (have_flag(flgs, TR_XTRA_MIGHT)) power++;

		/* Append a special "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_chr(t, 'x');
		t = object_desc_num(t, power);
		t = object_desc_chr(t, p2);

		/* All done */
		break;
	}


	/* Add the weapon bonuses */
	if (known && !flag_not_equipment)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (o_ptr->to_h)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (o_ptr->to_d)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}
	}

	//bow_ptr = &inventory[INVEN_BOW];
	bow_ptr = &inventory[INVEN_PACK + shootable(&dummy)];

	/* If have a firing weapon + ammo matches bow */
	/*:::矢玉の期待ダメージ*/
	//if (bow_ptr->k_idx && (o_ptr->tval == p_ptr->tval_ammo))
	if (dummy > SHOOT_UNSUITABLE && bow_ptr->k_idx && (o_ptr->tval == p_ptr->tval_ammo))
	{
		int avgdam = o_ptr->dd * (o_ptr->ds + 1) * 10 / 2;
		int tmul = bow_tmul(bow_ptr->tval, bow_ptr->sval);
		s16b energy_fire = bow_energy(bow_ptr->tval, bow_ptr->sval);

		/* See if the bow is "known" - then set damage bonus */
		if (object_is_known(bow_ptr)) avgdam += (bow_ptr->to_d * 10);

		/* Effect of ammo */
		if (known) avgdam += (o_ptr->to_d * 10);

		///mod141018 矢に理力が付いているときのダメージ増加処理
		if (have_flag(flgs, TR_FORCE_WEAPON) && (p_ptr->csp >=10))
			avgdam = avgdam * 5 / 2;

		/* Get extra "power" from "extra might" */
		if (p_ptr->xtra_might) tmul++;


		tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

		/* Launcher multiplier */
		avgdam *= tmul;
		avgdam /= (100 * 10);

		/* Get extra damage from concentration */
		if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent) avgdam = boost_concentration_damage(avgdam);

		if (avgdam < 0) avgdam = 0;

		/* Display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		if (p_ptr->num_fire == 0)
		{
			t = object_desc_chr(t, '0');
		}
		else
		{
			/* Calc effects of energy */
			avgdam *= (p_ptr->num_fire * 100);
			avgdam /= energy_fire;
			t = object_desc_num(t, avgdam);
		}

		t = object_desc_chr(t, p2);
	}

#if 0
	/*:::ニンジャのくさび投げダメージ*/
	///sysdel item
	else if ((p_ptr->pclass == CLASS_NINJA) && (o_ptr->tval == TV_SPIKE))
	{
		int avgdam = p_ptr->mighty_throw ? (1 + 3) : 1;
		s16b energy_fire = 100 - p_ptr->lev;

		avgdam += ((p_ptr->lev + 30) * (p_ptr->lev + 30) - 900) / 55;

		/* Display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		/* Calc effects of energy */
		avgdam = 100 * avgdam / energy_fire;

		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, p2);
	}
#endif
	/* Add the armor bonuses */
	if (known && !flag_not_equipment)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, o_ptr->ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (o_ptr->to_a)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour && !flag_not_equipment)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, o_ptr->ac);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode & OD_NAME_AND_ENCHANT) goto object_desc_done;


	if (known) /* Known item only */
	{
		/*
		 * Hack -- Wands and Staffs have charges.  Make certain how many charges
		 * a stack of staffs really has is clear. -LM-
		 */
		/*:::杖と魔法棒の回数*/
		if (((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_WAND)))
		{
			/* Dump " (N charges)" */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Clear explaination for staffs. */
			if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
			{
				t = object_desc_num(t, o_ptr->number);
				t = object_desc_str(t, "x ");
			}
			t = object_desc_num(t, o_ptr->pval);
#ifdef JP
			t = object_desc_str(t, "回分");
#else
			t = object_desc_str(t, " charge");
			if (o_ptr->pval != 1) t = object_desc_chr(t, 's');
#endif

			t = object_desc_chr(t, p2);
		}
		/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
		 * be in any state of charge or discharge, this now includes a number. -LM-
		 */
		/*:::ロッドの充填*/
		else if (o_ptr->tval == TV_ROD)
		{
			/* Hack -- Dump " (# charging)" if relevant */
			if (o_ptr->timeout)
			{
				/* Stacks of rods display an exact count of charging rods. */
				if (o_ptr->number > 1)
				{
					/* Paranoia. */
					if (k_ptr->pval == 0) k_ptr->pval = 1;

					/* Find out how many rods are charging, by dividing
					 * current timeout by each rod's maximum timeout.
					 * Ensure that any remainder is rounded up.  Display
					 * very discharged stacks as merely fully discharged.
					 */
					power = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
					if (power > o_ptr->number) power = o_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
#ifdef JP
					t = object_desc_str(t, "本 充填中)");
#else
					t = object_desc_str(t, " charging)");
#endif
				}

				/* "one Rod of Perception (1 charging)" would look tacky. */
				else
				{
#ifdef JP
					t = object_desc_str(t, "(充填中)");
#else
					t = object_desc_str(t, " (charging)");
#endif
				}
			}
		}
		//v1.1.86
		else if (o_ptr->tval == TV_ABILITY_CARD)
		{
			if (o_ptr->timeout)
			{
				if (o_ptr->number > 1)
				{
					int base_charge_turn = ability_card_list[o_ptr->pval].charge_turn;

					power = (o_ptr->timeout + (base_charge_turn - 1)) / base_charge_turn;
					if (power > o_ptr->number) power = o_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
					t = object_desc_str(t, _("枚 充填中)", " charging)"));
				}

				else
				{
					t = object_desc_str(t, _("(充填中)", "(charging)"));
				}
			}
		}

		/*:::pval表示　加速なども*/
		/* Dump "pval" flags for wearable items */
		if (have_pval_flags(flgs))
		{
			/* Start the display */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Dump the "pval" itself */
			t = object_desc_int(t, o_ptr->pval);

			/* Do not display the "pval" flags */
			if (have_flag(flgs, TR_HIDE_TYPE))
			{
				/* Nothing */
			}

			/* Speed */
			else if (have_flag(flgs, TR_SPEED))
			{
				/* Dump " to speed" */
#ifdef JP
				t = object_desc_str(t, "加速");
#else
				t = object_desc_str(t, " to speed");
#endif
			}

			/* Attack speed */
			else if (have_flag(flgs, TR_BLOWS))
			{
				/* Add " attack" */
#ifdef JP
				t = object_desc_str(t, "攻撃");
#else
				t = object_desc_str(t, " attack");

				/* Add "attacks" */
				if (ABS(o_ptr->pval) != 1) t = object_desc_chr(t, 's');
#endif
			}

			/* Stealth */
			else if (have_flag(flgs, TR_STEALTH))
			{
				/* Dump " to stealth" */
#ifdef JP
				t = object_desc_str(t, "隠密");
#else
				t = object_desc_str(t, " to stealth");
#endif
			}

			/* Search */
			else if (have_flag(flgs, TR_SEARCH))
			{
				/* Dump " to searching" */
#ifdef JP
				t = object_desc_str(t, "探索");
#else
				t = object_desc_str(t, " to searching");
#endif
			}

			/* Infravision */
			///item 赤外線視力廃止するなら
			else if (have_flag(flgs, TR_INFRA))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "赤外線視力");
#else
				t = object_desc_str(t, " to infravision");
#endif
			}
			//v1.1.99 魔法防御
			else if (have_flag(flgs, TR_SAVING))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "魔法防御");
#else
				t = object_desc_str(t, " to protection from magic");
#endif
			}
			//v1.1.99 解除
			else if (have_flag(flgs, TR_DISARM))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "解除");
#else
				t = object_desc_str(t, " to disarming");
#endif
			}

			/* Finish the display */
			t = object_desc_chr(t, p2);
		}

		///mod140930 投擲時ダメージ表記追加
		t = object_desc_str(t, print_throw_dam(o_ptr));

		/* Hack -- Process Lanterns/Torches */
		//if ((o_ptr->tval == TV_LITE) && (!(object_is_fixed_artifact(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))))
		//v1.1.15　☆も外す
		if ((o_ptr->tval == TV_LITE) && (!(object_is_artifact(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))))
		{
			/* Hack -- Turns of light for normal lites */
#ifdef JP
			t = object_desc_chr(t, '(');
#else
			t = object_desc_str(t, " (with ");
#endif

			if (o_ptr->name2 == EGO_LITE_LONG) t = object_desc_num(t, o_ptr->xtra4 * 2);
			else t = object_desc_num(t, o_ptr->xtra4);
#ifdef JP
			t = object_desc_str(t, "ターンの寿命)");
#else
			t = object_desc_str(t, " turns of light)");
#endif
		}

		if(o_ptr->tval == TV_GUN)
		{
			int timeout_base = calc_gun_timeout(o_ptr) * 1000;
			int bullets = calc_gun_load_num(o_ptr);

			t = object_desc_str(t, format("(%d/%d)",(timeout_base * bullets - o_ptr->timeout) / timeout_base,bullets));
		}
		/* Indicate charging objects, but not rods. */
		else if (o_ptr->timeout && (o_ptr->tval != TV_ROD && o_ptr->tval != TV_ABILITY_CARD))
		{
			t = object_desc_str(t, _("(充填中)", "(charging)"));
		}
	}


	/* No more details wanted */
	/*:::この関数の最後から二行目まで行っている*/
	if (mode & OD_OMIT_INSCRIPTION) goto object_desc_done;


	/* Prepare real inscriptions in a buffer */
	tmp_val2[0] = '\0';

	/* Auto abbreviation inscribe */
	if ((abbrev_extra || abbrev_all) && (o_ptr->ident & IDENT_MENTAL))
	{
		if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '%'))
		{
			bool kanji, all;

#ifdef JP
			kanji = TRUE;
#else
			kanji = FALSE;
#endif
			all = abbrev_all;

			get_ability_abbreviation(tmp_val2, o_ptr, kanji, all);
		}
	}

	/* Use the standard inscription if available */
	if (o_ptr->inscription)
	{
		char buff[1024];

		if (tmp_val2[0]) strcat(tmp_val2, ", ");

		/* Get inscription and convert {%} */
		get_inscription(buff, o_ptr);

		/* strcat with correct treating of kanji */
		my_strcat(tmp_val2, buff, sizeof(tmp_val2));
	}


	/* No fake inscription yet */
	fake_insc_buf[0] = '\0';

	/* Use the game-generated "feeling" otherwise, if available */
	if (o_ptr->feeling)
	{
		strcpy(fake_insc_buf, game_inscriptions[o_ptr->feeling]);
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (object_is_cursed(o_ptr) && (known || (o_ptr->ident & IDENT_SENSE)))
	{
#ifdef JP
		strcpy(fake_insc_buf, "呪われている");
#else
		strcpy(fake_insc_buf, "cursed");
#endif
	}

	/* Note "unidentified" if the item is unidentified */
	else if (((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)
		   || (o_ptr->tval == TV_LITE) || (o_ptr->tval == TV_FIGURINE))
		 && aware && !known
		 && !(o_ptr->ident & IDENT_SENSE))
	{
#ifdef JP
		strcpy(fake_insc_buf, "未鑑定");
#else
		strcpy(fake_insc_buf, "unidentified");
#endif
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & IDENT_EMPTY))
	{
#ifdef JP
		strcpy(fake_insc_buf, "空");
#else
		strcpy(fake_insc_buf, "empty");
#endif
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_is_tried(o_ptr))
	{
#ifdef JP
		strcpy(fake_insc_buf, "未判明");
#else
		strcpy(fake_insc_buf, "tried");
#endif
	}

	/* Note the discount, if any */
	//こころの面は敵レベルをdiscountに格納しているので割り引き表示をしない
	if (o_ptr->discount && o_ptr->tval != TV_MASK)
	{
		/* Hidden by real inscription unless in a store */
		if (!tmp_val2[0] || (o_ptr->ident & IDENT_STORE))
		{
			char discount_num_buf[4];

			/* Append to other fake inscriptions if any */
			if (fake_insc_buf[0]) strcat(fake_insc_buf, ", ");

			(void)object_desc_num(discount_num_buf, o_ptr->discount);
			strcat(fake_insc_buf, discount_num_buf);
#ifdef JP
			strcat(fake_insc_buf, "%引き");
#else
			strcat(fake_insc_buf, "% off");
#endif
		}
	}


	/* Append the inscription, if any */
	if (fake_insc_buf[0] || tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);

		/* Append fake inscriptions */
		if (fake_insc_buf[0])
		{
			t = object_desc_str(t, fake_insc_buf);
		}

		/* Append a separater */
		if (fake_insc_buf[0] && tmp_val2[0])
		{
			t = object_desc_chr(t, ',');
			t = object_desc_chr(t, ' ');
		}

		/* Append real inscriptions */
		if (tmp_val2[0])
		{
			t = object_desc_str(t, tmp_val2);
		}

		t = object_desc_chr(t, c2);
	}

object_desc_done:
	my_strcpy(buf, tmp_val, MAX_NLEN);
}



// v1.1.51 新アリーナ
//達成後、倒したモンスターたちのスペカ名を繋げたフレーバーアイテム「弾幕夢【○○】の写真」を生成する
void	make_nightmare_diary_photo(byte x, byte y)
{
	object_type forge;
	object_type *q_ptr = &forge;
	char name_buf[255];
	char *t = name_buf;

	int insc_idx_table[NIGHTMARE_DIARY_MONSTER_MAX];//nightmare_spellname_table[]の添字を5つまで選ぶ
	int insc_idx_num;//実際に↑に記録された数

	int i,j;
	int try_count;

	//適切な名前が決まるまで最大100回の試行
	for (try_count = 100; try_count; try_count--)
	{
		int cur_length = 0;
		bool flag_last_word = FALSE; //スペカ名の末尾に使える文字列が既に選ばれたフラグ
		bool flag_pos0_word = FALSE; //スペカ名の末尾に使わなければいけない文字列が既に選ばれたフラグ

		//前のループの値を消すための初期化
		insc_idx_num = 0;
		for (i = 0; i < NIGHTMARE_DIARY_MONSTER_MAX; i++) insc_idx_table[i] = 0;

		//戦ったモンスター数ループ
		for (i = 0; i < NIGHTMARE_DIARY_MONSTER_MAX; i++)
		{
			int tmp_insc_idx = 0;
			int r_idx = nightmare_mon_r_idx[i];//新アリーナ生成時に記録した、相手モンスターのr_idx
			int candidate_count = 0;

			if (!r_idx) break;

			//文字列候補テーブルを終端ダミーまでループ
			for (j = 0; nightmare_spellname_table[j].r_idx; j++)
			{
				bool flag_ok = FALSE;

				//モンスターがランダムユニーク1(ここでは夢の世界の＠)の場合、文字列候補のclass_idxが＠の職業と一致していれば一致とみなす
				//＠がユニーククラスでないときは文字列選定の対象としない
				if (r_idx == MON_RANDOM_UNIQUE_1)
				{
					if (cp_ptr->flag_only_unique)
					{
						if (p_ptr->pclass == nightmare_spellname_table[j].class_idx) flag_ok = TRUE;
					}
					else break;
				}
				//それ以外のときはr_idx同士の一致で判定
				else
				{
					if (r_idx == nightmare_spellname_table[j].r_idx) flag_ok = TRUE;
				}

				//すでに「末尾に使わなければいけない文字」が選ばれている場合同様の文字を選ぶことはできない
				if (flag_pos0_word && !nightmare_spellname_table[j].position) flag_ok = FALSE;

				//条件に一致した文字列のidxを一時的に記憶
				if (flag_ok)
				{
					candidate_count++;

					if (one_in_(candidate_count)) tmp_insc_idx = j;

				}
			}
			if (!candidate_count)
			{
				if (r_idx != MON_RANDOM_UNIQUE_1 || cp_ptr->flag_only_unique)
				{
					msg_format(_("WARNING:idx(%d)の弾幕写真名が設定されていない",
                                "WARNING: Danmaku photo name not set for idx (%d)"), r_idx);
				}

				continue;
			}
			//このモンスターに対応する文字列が決まったので一時的領域から生成用配列に移す
			insc_idx_table[insc_idx_num] = tmp_insc_idx;
			insc_idx_num++;

			cur_length += strlen(nightmare_spellname_table[tmp_insc_idx].phrase);
#ifndef JP
            cur_length += 1;
#endif // JP

			if (nightmare_spellname_table[tmp_insc_idx].position < 10) flag_last_word = TRUE;
			if (nightmare_spellname_table[tmp_insc_idx].position == 0) flag_pos0_word = TRUE;
		}
		if (!i)
		{
			msg_print(_("ERROR:nightmare_mon_r_idx[]が空である",
                        "ERROR:nightmare_mon_r_idx[] is empty"));
			return;
		}
		if (insc_idx_num < 2)
		{
			msg_format(_("ERROR:nightmare_mon_r_idx[%d,%d,%d,%d,%d]に対応する文字列データが不足している",
                        "ERROR: Not enough character data for nightmare_mon_r_idx[%d,%d,%d,%d,%d]"), nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
			return;
		}

		//合成スペカ名が接続詞抜きで全角25文字超えたらやり直し
		if (cur_length > 50) continue;

		//合成スペカ名の末尾に使える文字列がひとつも選定されなかったらやり直し
		if (!flag_last_word) continue;


		break;
	}

	if (!try_count)
	{
		msg_format(_("ERROR:idx[%d,%d,%d,%d,%d]で100回繰り返してもスペカ名が決まらなかった",
                    "ERROR: Spellcard name not chosen after 100 tries for idx[%d,%d,%d,%d,%d]"),
                    nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
		return;

	}

	if (cheat_xtra)
	{
		msg_format("r_idx[%d,%d,%d,%d,%d]", nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
		msg_format("insc_idx_num:%d", insc_idx_num);
		msg_format("insc_idx[%d,%d,%d,%d,%d]", insc_idx_table[0], insc_idx_table[1], insc_idx_table[2], insc_idx_table[3], insc_idx_table[4]);
	}

	//選ばれたスペカ名部品idxを優先度に従い並べ替え。適当にやったら効率悪いセレクションソートになったがどうせ要素数たった5なので問題なし
	for (i = 0; i < insc_idx_num-1; i++)
	{
		for (j = i+1; j < insc_idx_num; j++)
		{
			if (nightmare_spellname_table[insc_idx_table[i]].position < nightmare_spellname_table[insc_idx_table[j]].position)
			{
				int swap;
				swap = insc_idx_table[i];
				insc_idx_table[i] = insc_idx_table[j];
				insc_idx_table[j] = swap;
			}
		}
	}

	//並べ替えた文字列idxに従い、文字列を接続詞と一緒に連結。連結のためにobject_desc_str()を使用した
	t = object_desc_str(t, _("【", "["));
	for (i = 0; i < insc_idx_num; i++)
	{
		//本体前に接続詞があるとき、文字列全体の最初でなければ置く
		if(i != 0 && nightmare_spellname_table[insc_idx_table[i]].conj1) {
            t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].conj1);
#ifndef JP
            t = object_desc_str(t, " ");
#endif
		}

		//本体を置く
		t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].phrase);

		//本体後ろに接続詞があるとき、文字列全体の最後でなく、ひとつ後ろの語に本体前接続詞がなければ置く
		//最初にi<insc_idx_num-1を条件にしてあるのでi+1が配列の外にはみ出すことはないはず
		if (i < insc_idx_num-1 && nightmare_spellname_table[insc_idx_table[i]].conj2 && !nightmare_spellname_table[insc_idx_table[i+1]].conj1) {
#ifndef JP
            t = object_desc_str(t, " ");
#endif
            t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].conj2);
		}

#ifndef JP
        if (i < (insc_idx_num - 1)) {
            t = object_desc_str(t, " ");
        }
#endif
	}
	t = object_desc_str(t, _("】", "]"));

	//アイテム生成開始
	object_prep(q_ptr, lookup_kind(TV_PHOTO, SV_PHOTO_NIGHTMARE));

	//内部的にランダムアーティファクト扱いにする
	q_ptr->art_name = quark_add(name_buf);
	//*鑑定*済み扱いにする
	object_aware(q_ptr);
	object_known(q_ptr);
	q_ptr->ident |= (IDENT_MENTAL);
	//今回のバトルでの獲得ポイントをxtra4と5に分けて記録する
	q_ptr->xtra4 = (s16b)(tmp_nightmare_point / 10000);
	q_ptr->xtra5 = (s16b)(tmp_nightmare_point % 10000);

	drop_near(q_ptr, 0,y, x);


}



