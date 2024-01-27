/* File: files.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: code dealing with files (and death) */

#include "angband.h"

#include "player/player-log-save.h"


/*
 * You may or may not want to use the following "#undef".
 */
/* #undef _POSIX_SAVED_IDS */


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(getuid()) != 0)
	{
#ifdef JP
quit("setuid(): 正しく許可が取れません！");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(getgid()) != 0)
	{
#ifdef JP
quit("setgid(): 正しく許可が取れません！");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): 正しく許可が取れません！");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): 正しく許可が取れません！");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif

# endif

#endif

}


/*
 * Hack -- grab permissions
 */
/*:::たぶんUnixやLinux用の権限確認　*/
void safe_setuid_grab(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(player_euid) != 0)
	{
#ifdef JP
quit("setuid(): 正しく許可が取れません！");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(player_egid) != 0)
	{
#ifdef JP
quit("setgid(): 正しく許可が取れません！");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): 正しく許可が取れません！");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): 正しく許可が取れません！");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif /* SAFE_SETUID_POSIX */

# endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 *
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 *
 * Hack -- We will always extract at least one token
 */
/*::: ":"か"/"で区切られた文字列を分割して**tokenに格納し、その数を返す num以上の部分は無視される　*/
s16b tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;

	char *s = buf;


	/* Process */
	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if ((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			/*:::TOKENIZE_CHECKQUOTE=0x01*/
			/*:::modeにこの数値が入っているときは'や\に何らかの処理をするらしい？どんなふうに使うのかどうも分からん*/
			if ((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				/* Advance */
				t++;

				/* Handle backslash */
				if (*t == '\\') t++;

				/* Require a character */
				if (!*t) break;

				/* Advance */
				t++;

				/* Hack -- Require a close quote */
				if (*t != '\'') *t = '\'';
			}

			/* Handle back-slash */
			if (*t == '\\') t++;
		}

		/* Nothing left */
		if (!*t) break;

		/* Nuke and advance */
		/*:::ここでsに\0が入ってtokens一つ分が一つの文字列として独立した？*/
		*t++ = '\0';

		/* Save the token */
		tokens[i++] = s;

		/* Advance */
		s = t;
	}

	/* Save the token */
	tokens[i++] = s;

	/* Number found */
	return (i);
}


/* A number with a name */
typedef struct named_num named_num;

struct named_num
{
	cptr name;		/* The name of this thing */
	int num;			/* A number associated with it */
};

///mod131229 GF_xxxの追加
//spell.xx.prfの記述と内部的なGF_XXXの値を対応付けるための表
//GF_XXXの順番どおりでなくてもいいがなるべく揃えときたい
/* Index of spell type names */
static named_num gf_desc[] =
{
	{"GF_ELEC", 				GF_ELEC				},
	{"GF_POIS", 				GF_POIS				},
	{"GF_ACID", 				GF_ACID				},
	{"GF_COLD", 				GF_COLD				},
	{"GF_FIRE",		 			GF_FIRE				},
	{"GF_PSY_SPEAR",			GF_PSY_SPEAR			},
	{"GF_MISSILE",				GF_MISSILE			},
	{"GF_ARROW",				GF_ARROW				},
	{"GF_PLASMA",				GF_PLASMA			},
	{"GF_WATER",				GF_WATER				},
	{"GF_LITE",					GF_LITE				},
	{"GF_DARK",					GF_DARK				},
	{"GF_LITE_WEAK",			GF_LITE_WEAK		},
	{"GF_DARK_WEAK",			GF_DARK_WEAK		},
	{"GF_SHARDS",				GF_SHARDS			},
	{"GF_SOUND",				GF_SOUND				},
	{"GF_CONFUSION",			GF_CONFUSION		},
	{"GF_FORCE",				GF_FORCE				},
	{"GF_INACT",				GF_INACT			},
	{"GF_MANA",					GF_MANA				},
	{"GF_METEOR",				GF_METEOR			},
	{"GF_ICE",					GF_ICE				},
	{"GF_CHAOS",				GF_CHAOS				},
	//30
	{"GF_NETHER",				GF_NETHER			},
	{"GF_DISENCHANT",			GF_DISENCHANT		},
	{"GF_NEXUS",				GF_NEXUS				},
	{"GF_TIME",					GF_TIME				},
	{"GF_GRAVITY",				GF_GRAVITY			},
	{"GF_DEC_ATK",				GF_DEC_ATK },
	{"GF_DEC_DEF",				GF_DEC_DEF },
	{"GF_DEC_MAG",				GF_DEC_MAG },
	{"GF_DEC_ALL",				GF_DEC_ALL },
	{"GF_KILL_WALL",			GF_KILL_WALL		},
	{"GF_KILL_DOOR",			GF_KILL_DOOR		},
	{"GF_KILL_TRAP",			GF_KILL_TRAP		},
	{ "GF_NO_MOVE",				GF_NO_MOVE },
	{ "GF_BERSERK",				GF_BERSERK },
	{ "GF_SUPER_EGO",			GF_SUPER_EGO },

	{"GF_MAKE_DOOR",			GF_MAKE_DOOR		},
	{"GF_MAKE_TRAP",			GF_MAKE_TRAP		},
	{"GF_MAKE_TREE",			GF_MAKE_TREE		},
	{ "GF_DIG_OIL",			GF_DIG_OIL },
	{ "GF_ALCOHOL",			GF_ALCOHOL },

	{"GF_OLD_CLONE",			GF_OLD_CLONE		},
	{"GF_OLD_POLY",			GF_OLD_POLY			},
	{"GF_OLD_HEAL",			GF_OLD_HEAL			},
	{"GF_OLD_SPEED",			GF_OLD_SPEED		},
	{"GF_OLD_SLOW",			GF_OLD_SLOW			},
	{"GF_OLD_CONF",			GF_OLD_CONF			},
	{"GF_OLD_SLEEP",			GF_OLD_SLEEP		},
	{"GF_OLD_DRAIN",			GF_OLD_DRAIN		},
	{ "GF_PIT_FALL",			GF_PIT_FALL },
	{ "GF_ACTIV_TRAP",			GF_ACTIV_TRAP },
	{"GF_AWAY_UNDEAD",		GF_AWAY_UNDEAD		},
	{"GF_AWAY_EVIL",			GF_AWAY_EVIL		},
	{"GF_AWAY_ALL",			GF_AWAY_ALL			},
	{"GF_TURN_UNDEAD",		GF_TURN_UNDEAD		},
	{"GF_TURN_EVIL",			GF_TURN_EVIL		},
	{"GF_TURN_ALL",			GF_TURN_ALL			},
	{"GF_DISP_UNDEAD",		GF_DISP_UNDEAD		},
	{"GF_DISP_EVIL",			GF_DISP_EVIL		},
	{"GF_DISP_ALL",			GF_DISP_ALL			},
	{"GF_DISP_DEMON",			GF_DISP_DEMON		},
	{"GF_DISP_LIVING",		GF_DISP_LIVING		},
	{"GF_ROCKET",				GF_ROCKET			},
	{"GF_NUKE",					GF_NUKE				},
	{"GF_MAKE_GLYPH",			GF_MAKE_GLYPH		},
	{"GF_STASIS",				GF_STASIS			},
	{"GF_STONE_WALL",			GF_STONE_WALL		},
	{"GF_DEATH_RAY",			GF_DEATH_RAY		},
	{"GF_STUN",					GF_STUN				},
	{"GF_HOLY_FIRE",			GF_HOLY_FIRE		},
	{"GF_HELL_FIRE",			GF_HELL_FIRE		},
	{"GF_DISINTEGRATE",		GF_DISINTEGRATE	},
	{"GF_CHARM",				GF_CHARM				},
	{"GF_CONTROL_UNDEAD",	GF_CONTROL_UNDEAD	},
	{"GF_CONTROL_ANIMAL",	GF_CONTROL_ANIMAL	},
	{"GF_PSI",					GF_PSI				},
	{"GF_PSI_DRAIN",			GF_PSI_DRAIN		},
	{"GF_TELEKINESIS",		GF_TELEKINESIS		},
	{"GF_JAM_DOOR",			GF_JAM_DOOR			},
	{"GF_DOMINATION",			GF_DOMINATION		},
	{"GF_DISP_GOOD",			GF_DISP_GOOD		},
	{"GF_DRAIN_MANA",			GF_DRAIN_MANA		},
	{"GF_MIND_BLAST",			GF_MIND_BLAST		},
	{"GF_BRAIN_SMASH",			GF_BRAIN_SMASH		},
	{"GF_CAUSE_1",			GF_CAUSE_1		},
	{"GF_CAUSE_2",			GF_CAUSE_2		},
	{"GF_CAUSE_3",			GF_CAUSE_3		},
	{"GF_CAUSE_4",			GF_CAUSE_4		},
	{"GF_HAND_DOOM",			GF_HAND_DOOM		},
	{"GF_CAPTURE",			GF_CAPTURE		},
	{"GF_ANIM_DEAD",			GF_ANIM_DEAD		},
	{"GF_CONTROL_LIVING",		GF_CONTROL_LIVING	},
	{"GF_IDENTIFY",			GF_IDENTIFY	},
	{"GF_ATTACK",			GF_ATTACK	},
	{"GF_ENGETSU",			GF_ENGETSU	},
	{"GF_GENOCIDE",			GF_GENOCIDE	},
	{"GF_PHOTO",			GF_PHOTO	},
	{"GF_CONTROL_DEMON",	GF_CONTROL_DEMON	},
	{"GF_LAVA_FLOW",	GF_LAVA_FLOW	},
	{"GF_BLOOD_CURSE",	GF_BLOOD_CURSE	},
	{"GF_SEEKER",			GF_SEEKER			},
	{"GF_SUPER_RAY",		GF_SUPER_RAY			},
	{"GF_STAR_HEAL",		GF_STAR_HEAL			},
	{"GF_WATER_FLOW",		GF_WATER_FLOW			},
	{"GF_CRUSADE",		GF_CRUSADE			},
	{"GF_STASIS_EVIL",			GF_STASIS_EVIL		},
	{"GF_WOUNDS",			GF_WOUNDS		},
	{"GF_DISTORTION",		GF_DISTORTION	},
	{"GF_COSMIC_HORROR",			GF_COSMIC_HORROR		},
	{"GF_PUNISH_1",			GF_PUNISH_1		},
	{"GF_PUNISH_2",			GF_PUNISH_2		},
	{"GF_PUNISH_3",			GF_PUNISH_3		},
	{"GF_PUNISH_4",			GF_PUNISH_4		},
	{"GF_POLLUTE",			GF_POLLUTE		},
	{"GF_TORNADO",			GF_TORNADO		},
	{"GF_KYUTTOSHITEDOKA_N",GF_KYUTTOSHITEDOKA_N},
	{"GF_WINDCUTTER",			GF_WINDCUTTER		},
	{"GF_REDEYE",			GF_REDEYE		},
	{"GF_RYUURI",			GF_RYUURI		},
	{"GF_POSSESSION",			GF_POSSESSION		},
	{"GF_SOULSTEAL",			GF_SOULSTEAL		},
	{"GF_NIGHTMARE",			GF_NIGHTMARE		},
	{"GF_TRAIN",			GF_TRAIN		},
	{"GF_DISP_KWAI",			GF_DISP_KWAI		},
	{"GF_NORTHERN",			GF_NORTHERN		},
	{"GF_BANKI_BEAM",			GF_BANKI_BEAM		},
	{"GF_N_E_P",			GF_N_E_P		},
	{"GF_HOUTOU",			GF_HOUTOU		},
	{"GF_BANKI_BEAM2",			GF_BANKI_BEAM2		},
	{"GF_CHARM_PALADIN",				GF_CHARM_PALADIN	},
	{"GF_SATORI",				GF_SATORI	},
	{"GF_MAKE_FLOWER",				GF_MAKE_FLOWER	},
	{"GF_KOKORO",				GF_KOKORO	},
	{"GF_MOSES",				GF_MOSES	},
	{"GF_REDMAGIC",				GF_REDMAGIC	},
	{"GF_GUNGNIR",				GF_GUNGNIR	},
	{"GF_WALL_TO_DOOR",				GF_WALL_TO_DOOR	},
	{"GF_STASIS_DRAGON",			GF_STASIS_DRAGON			},
	{"GF_CHARM_C",			GF_CHARM_C			},
	{"GF_CHARM_J",			GF_CHARM_J			},
	{"GF_CHARM_B",			GF_CHARM_B			},
	{"GF_MAKE_SPIDER_NEST",	GF_MAKE_SPIDER_NEST},
	{"GF_THROW_PLATE",	GF_THROW_PLATE},
	{"GF_BLAZINGSTAR",	GF_BLAZINGSTAR},
	{"GF_SOULSCULPTURE",	GF_SOULSCULPTURE},
	{"GF_YAKU",	GF_YAKU},
	{"GF_STEAM",	GF_STEAM},
	{"GF_SEIRAN_BEAM",	GF_SEIRAN_BEAM},
	{"GF_PURIFY",	GF_PURIFY},
	{"GF_SPECIAL_SHOT",	GF_SPECIAL_SHOT},
	{"GF_YOUMU",	GF_YOUMU},
	{"GF_STASIS_LIVING",			GF_STASIS_LIVING },
	{ "GF_LUNATIC_TORCH",			GF_LUNATIC_TORCH },
	{ "GF_BRAIN_FINGER_PRINT",			GF_BRAIN_FINGER_PRINT },
	{ "GF_RAINBOW",			GF_RAINBOW },
	{ "GF_CONTROL_FISH",			GF_CONTROL_FISH },
	{ "GF_MAKE_BLIZZARD",			GF_MAKE_BLIZZARD },
	{ "GF_MAKE_POISON_PUDDLE",			GF_MAKE_POISON_PUDDLE },
	{ "GF_MAKE_ACID_PUDDLE",			GF_MAKE_ACID_PUDDLE },
	{ "GF_MAKE_STORM",			GF_MAKE_STORM },
	{ "GF_KANAMEISHI",			GF_KANAMEISHI },
	{ "GF_HOLY_WATER",			GF_HOLY_WATER },
	{ "GF_TIMED_SHARD",			GF_TIMED_SHARD },


	{NULL, 						0						}
};


/*
 * Parse a sub-file of the "extra info" (format shown below)
 *
 * Each "action" line has an "action symbol" in the first column,
 * followed by a colon, followed by some command specific info,
 * usually in the form of "tokens" separated by colons or slashes.
 *
 * Blank lines, lines starting with white space, and lines starting
 * with pound signs ("#") are ignored (as comments).
 *
 * Note the use of "tokenize()" to allow the use of both colons and
 * slashes as delimeters, while still allowing final tokens which
 * may contain any characters including "delimiters".
 *
 * Note the use of "strtol()" to allow all "integers" to be encoded
 * in decimal, hexidecimal, or octal form.
 *
 * Note that "monster zero" is used for the "player" attr/char, "object
 * zero" will be used for the "stack" attr/char, and "feature zero" is
 * used for the "nothing" attr/char.
 *
 * Parse another file recursively, see below for details
 *   %:<filename>
 *
 * Specify the attr/char values for "monsters" by race index
 *   R:<num>:<a>:<c>
 *
 * Specify the attr/char values for "objects" by kind index
 *   K:<num>:<a>:<c>
 *
 * Specify the attr/char values for "features" by feature index
 *   F:<num>:<a>:<c>
 *
 * Specify the attr/char values for unaware "objects" by kind tval
 *   U:<tv>:<a>:<c>
 *
 * Specify the attr/char values for inventory "objects" by kind tval
 *   E:<tv>:<a>:<c>
 *
 * Define a macro action, given an encoded macro action
 *   A:<str>
 *
 * Create a normal macro, given an encoded macro trigger
 *   P:<str>
 *
 * Create a command macro, given an encoded macro trigger
 *   C:<str>
 *
 * Create a keyset mapping
 *   S:<key>:<key>:<dir>
 *
 * Turn an option off, given its name
 *   X:<str>
 *
 * Turn an option on, given its name
 *   Y:<str>
 *
 * Specify visual information, given an index, and some data
 *   V:<num>:<kv>:<rv>:<gv>:<bv>
 *
 * Specify the set of colors to use when drawing a zapped spell
 *   Z:<type>:<str>
 *
 * Specify a macro trigger template and macro trigger names.
 *   T:<template>:<modifier chr>:<modifier name1>:<modifier name2>:...
 *   T:<trigger>:<keycode>:<shift-keycode>
 *
 */

 /*:::設定変更コマンドの解釈部*/
errr process_pref_file_command(char *buf)
{
	int i, j, n1, n2;

	char *zz[16];


	/* Require "?:*" format */
	if (buf[1] != ':') return 1;


	switch (buf[0])
	{
	/* Mega-Hack -- read external player's history file */
	/* Process "H:<history>" */
	case 'H':
		add_history_from_pref_line(buf + 2);
		return 0;

	/* Process "R:<num>:<a>/<c>" -- attr/char for monster races */
	case 'R':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			monster_race *r_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_r_idx) return 1;
			r_ptr = &r_info[i];
			if (n1 || (!(n2 & 0x80) && n2)) r_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if (n2) r_ptr->x_char = n2;
			return 0;
		}
		break;

	/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	case 'K':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *k_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_k_idx) return 1;
			k_ptr = &k_info[i];
			if (n1 || (!(n2 & 0x80) && n2)) k_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if (n2) k_ptr->x_char = n2;
			return 0;
		}
		break;

	/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
	/* "F:<num>:<a>/<c>" */
	/* "F:<num>:<a>/<c>:LIT" */
	/* "F:<num>:<a>/<c>:<la>/<lc>:<da>/<dc>" */
	case 'F':
		{
			feature_type *f_ptr;
			int num = tokenize(buf + 2, F_LIT_MAX * 2 + 1, zz, TOKENIZE_CHECKQUOTE);

			if ((num != 3) && (num != 4) && (num != F_LIT_MAX * 2 + 1)) return 1;
			else if ((num == 4) && !streq(zz[3], "LIT")) return 1;

			i = (huge)strtol(zz[0], NULL, 0);
			if (i >= max_f_idx) return 1;
			f_ptr = &f_info[i];

			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[F_LIT_STANDARD] = n1; /* Allow TERM_DARK text */
			if (n2) f_ptr->x_char[F_LIT_STANDARD] = n2;

			/* Mega-hack -- feat supports lighting */
			switch (num)
			{
			/* No lighting support */
			case 3:
				n1 = f_ptr->x_attr[F_LIT_STANDARD];
				n2 = f_ptr->x_char[F_LIT_STANDARD];
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					f_ptr->x_attr[j] = n1;
					f_ptr->x_char[j] = n2;
				}
				break;

			/* Use default lighting */
			case 4:
				apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);
				break;

			/* Use desired lighting */
			case F_LIT_MAX * 2 + 1:
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					n1 = strtol(zz[j * 2 + 1], NULL, 0);
					n2 = strtol(zz[j * 2 + 2], NULL, 0);
					if (n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[j] = n1; /* Allow TERM_DARK text */
					if (n2) f_ptr->x_char[j] = n2;
				}
				break;
			}
		}
		return 0;

	/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	case 'S':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			misc_to_attr[j] = n1;
			misc_to_char[j] = n2;
			return 0;
		}
		break;

	/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	case 'U':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			for (i = 1; i < max_k_idx; i++)
			{
				object_kind *k_ptr = &k_info[i];
				if (k_ptr->tval == j)
				{
					if (n1) k_ptr->d_attr = n1;
					if (n2) k_ptr->d_char = n2;
				}
			}
			return 0;
		}
		break;

	/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	case 'E':
		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = strtol(zz[1], NULL, 0);
			if (n1) tval_to_attr[j] = n1;
			return 0;
		}
		break;

	/* Process "A:<str>" -- save an "action" for later */
	case 'A':
		text_to_ascii(macro__buf, buf+2);
		return 0;

	/* Process "P:<str>" -- normal macro */
	case 'P':
	{
		char tmp[1024];

		text_to_ascii(tmp, buf+2);
		macro_add(tmp, macro__buf);
		return 0;
	}

	/* Process "C:<str>" -- create keymap */
	case 'C':
	{
		int mode;
		char tmp[1024];

		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return 1;

		mode = strtol(zz[0], NULL, 0);
		if ((mode < 0) || (mode >= KEYMAP_MODES)) return 1;

		text_to_ascii(tmp, zz[1]);
		if (!tmp[0] || tmp[1]) return 1;
		i = (byte)(tmp[0]);

		string_free(keymap_act[mode][i]);

		keymap_act[mode][i] = string_make(macro__buf);

		return 0;
	}

	/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	case 'V':
		if (tokenize(buf+2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return 0;
		}
		break;

	/* Process "X:<str>" -- turn option off */
	/* Process "Y:<str>" -- turn option on */
	case 'X':
	case 'Y':
		for (i = 0; option_info[i].o_desc; i++)
		{
			if (option_info[i].o_var &&
			    option_info[i].o_text &&
			    streq(option_info[i].o_text, buf + 2))
			{
				int os = option_info[i].o_set;
				int ob = option_info[i].o_bit;

				if ((p_ptr->playing || character_xtra) &&
					(OPT_PAGE_BIRTH == option_info[i].o_page) && !p_ptr->wizard)
				{
#ifdef JP
					msg_format("初期オプションは変更できません! '%s'", buf);
#else
					msg_format("Birth options can not changed! '%s'", buf);
#endif
					msg_print(NULL);
					return 0;
				}

				if (buf[0] == 'X')
				{
					/* Clear */
					option_flag[os] &= ~(1L << ob);
					(*option_info[i].o_var) = FALSE;
				}
				else
				{
					/* Set */
					option_flag[os] |= (1L << ob);
					(*option_info[i].o_var) = TRUE;
				}
				return 0;
			}
		}

		/* don't know that option. ignore it.*/
#ifdef JP
		msg_format("オプションの名前が正しくありません： %s", buf);
#else
		msg_format("Ignored invalid option: %s", buf);
#endif
		msg_print(NULL);
		return 0;

	/* Process "Z:<type>:<str>" -- set spell color */
		/*:::魔法などの属性ごとの色設定部分 spell-xx.prf*/
	case 'Z':
	{
		/* Find the colon */
		char *t = my_strchr(buf + 2, ':');

		/* Oops */
		if (!t) return 1;

		/* Nuke the colon */
		*(t++) = '\0';

		for (i = 0; gf_desc[i].name; i++)
		{
			/* Match this type */
			if (streq(gf_desc[i].name, buf + 2))
			{
				/* Remember this color set */
				gf_color[gf_desc[i].num] = quark_add(t);

				/* Success */
				return 0;
			}
		}

		break;
	}

	/* Initialize macro trigger names and a template */
	/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
	/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
	case 'T':
	{
		int tok = tokenize(buf+2, 2+MAX_MACRO_MOD, zz, 0);

		/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
		if (tok >= 4)
		{
			int num;

			if (macro_template != NULL)
			{
				num = strlen(macro_modifier_chr);

				/* Kill the template string */
				string_free(macro_template);
				macro_template = NULL;

				/* Kill flag characters of modifier keys */
				string_free(macro_modifier_chr);

				/* Kill corresponding modifier names */
				for (i = 0; i < num; i++)
				{
					string_free(macro_modifier_name[i]);
				}

				/* Kill trigger name strings */
				for (i = 0; i < max_macrotrigger; i++)
				{
					string_free(macro_trigger_name[i]);
					string_free(macro_trigger_keycode[0][i]);
					string_free(macro_trigger_keycode[1][i]);
				}

				max_macrotrigger = 0;
			}

			if (*zz[0] == '\0') return 0; /* clear template */

			/* Number of modifier flags */
			num = strlen(zz[1]);

			/* Limit the number */
			num = MIN(MAX_MACRO_MOD, num);

			/* Stop if number of modifier is not correct */
			if (2 + num != tok) return 1;

			/* Get a template string */
			macro_template = string_make(zz[0]);

			/* Get flag characters of modifier keys */
			macro_modifier_chr = string_make(zz[1]);

			/* Get corresponding modifier names */
			for (i = 0; i < num; i++)
			{
				macro_modifier_name[i] = string_make(zz[2+i]);
			}
		}

		/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
		else if (tok >= 2)
		{
			char buf2[1024];
			int m;
			char *t, *s;
			if (max_macrotrigger >= MAX_MACRO_TRIG)
			{
#ifdef JP
				msg_print("マクロトリガーの設定が多すぎます!");
#else
				msg_print("Too many macro triggers!");
#endif
				return 1;
			}
			m = max_macrotrigger;
			max_macrotrigger++;

			/* Take into account the escape character  */
			t = buf2;
			s = zz[0];
			while (*s)
			{
				if ('\\' == *s) s++;
				*t++ = *s++;
			}
			*t = '\0';

			/* Get a trigger name */
			macro_trigger_name[m] = string_make(buf2);

			/* Get the corresponding key code */
			macro_trigger_keycode[0][m] = string_make(zz[1]);

			if (tok == 3)
			{
				/* Key code of a combination of it with the shift key */
				macro_trigger_keycode[1][m] = string_make(zz[2]);
			}
			else
			{
				macro_trigger_keycode[1][m] = string_make(zz[1]);
			}
		}

		/* No error */
		return 0;
	}
	}

	/* Failure */
	return 1;
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
/*:::設定ファイルの行の?:[条件式]の解釈をする　条件が一致したら'0'以外が返るはず*/
cptr process_pref_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';
	static char tmp[10];

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	/*:::///patch131222本家rev3510の修正を適用？*/
#ifdef PATCH_ORIGIN
	while (iswspace(*s)) s++;
#else
	while (isspace(*s)) s++;
#endif
	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_pref_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "0";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = process_pref_file_expr(&s, &f);
				if (streq(t, p)) v = "1";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && atoi(p) > atoi(t)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);

				/* Compare two numbers instead of string */
				if (*t && atoi(p) < atoi(t)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		if (f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
#ifdef JP
		while (iskanji(*s) || (isprint(*s) && !my_strchr(" []", *s)))
		{
			if (iskanji(*s)) s++;
			s++;
		}
#else
		while (isprint(*s) && !my_strchr(" []", *s)) ++s;
#endif

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if (*b == '$')
		{
			/* System */
			if (streq(b+1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			else if (streq(b+1, "KEYBOARD"))
			{
				v = ANGBAND_KEYBOARD;
			}

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			/* Monochrome mode */
			else if (streq(b+1, "MONOCHROME"))
			{
				if (arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			/* Race */
			else if (streq(b+1, "RACE"))
			{
#ifdef JP
				v = rp_ptr->E_title;
#else
				v = rp_ptr->title;
#endif
			}

			/* Class */
			else if (streq(b+1, "CLASS"))
			{
#ifdef JP
				v = cp_ptr->E_title;
#else
				v = cp_ptr->title;
#endif
			}

			//v1.1.70 性格追加
			else if (streq(b + 1, "PERSONALITY"))
			{
#ifdef JP
				v = ap_ptr->E_title;
#else
                v = ap_ptr->title;
#endif
			}

			/* Player */
			else if (streq(b+1, "PLAYER"))
			{
				static char tmp_player_name[32];
				char *pn, *tpn;
				for (pn = player_name, tpn = tmp_player_name; *pn; pn++, tpn++)
				{
#ifdef JP
					if (iskanji(*pn))
					{
						*(tpn++) = *(pn++);
						*tpn = *pn;
						continue;
					}
#endif
					*tpn = my_strchr(" []", *pn) ? '_' : *pn;
				}
				*tpn = '\0';
				v = tmp_player_name;
			}

			/* First realm */
			else if (streq(b+1, "REALM1"))
			{
#ifdef JP
				v = E_realm_names[p_ptr->realm1];
#else
				v = realm_names[p_ptr->realm1];
#endif
			}

			/* Second realm */
			else if (streq(b+1, "REALM2"))
			{
#ifdef JP
				v = E_realm_names[p_ptr->realm2];
#else
				v = realm_names[p_ptr->realm2];
#endif
			}

			/* Level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%02d", p_ptr->lev);
				v = tmp;
			}

			/* Autopick auto-register is in-use or not? */
			else if (streq(b+1, "AUTOREGISTER"))
			{
				if (p_ptr->autopick_autoregister)
					v = "1";
				else
					v = "0";
			}
			else if (streq(b+1, "EXTRA_MODE"))
			{
				if (EXTRA_MODE)
					v = "ON";
				else
					v = "OFF";
			}
			else if (streq(b+1, "BERSERKER"))
			{
				if (p_ptr->pseikaku == SEIKAKU_BERSERK)
					v = "ON";
				else
					v = "OFF";
			}

			/* Money */
			else if (streq(b+1, "MONEY"))
			{
				sprintf(tmp, "%09ld", (long int)p_ptr->au);
				v = tmp;
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}


#define PREF_TYPE_NORMAL   0
#define PREF_TYPE_AUTOPICK 1
#define PREF_TYPE_HISTPREF 2

/*
 * Open the "user pref file" and parse it.
 */
/*:::設定ファイルの読み込みと解釈*/
static errr process_pref_file_aux(cptr name, int preftype)
{
	FILE *fp;

	char buf[1024];

	char old[1024];

	int line = -1;

	errr err = 0;

	bool bypass = FALSE;


	/* Open the file */
	fp = my_fopen(name, "r");

	/* No such file */
	if (!fp) return (-1);

	/* Process the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Count lines */
		line++;

		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
#ifdef JP
		if (!iskanji(buf[0]))
#endif
	/*:::///patch131222本家rev3510の修正を適用？*/
#ifdef PATCH_ORIGIN
		if (iswspace(buf[0])) continue;
#else
		if (isspace(buf[0])) continue;
#endif
		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if (bypass) continue;


		/* Process "%:<file>" */
		if (buf[0] == '%')
		{
			static int depth_count = 0;

			/* Ignore if deeper than 20 level */
			if (depth_count > 20) continue;

			/* Count depth level */
			depth_count++;

  			/* Process that file if allowed */
			switch (preftype)
			{
			case PREF_TYPE_AUTOPICK:
				(void)process_autopick_file(buf + 2);
				break;
			case PREF_TYPE_HISTPREF:
				(void)process_histpref_file(buf + 2);
				break;
			default:
				(void)process_pref_file(buf + 2);
				break;
			}

			/* Set back depth level */
			depth_count--;

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* This is not original pref line... */
		if (err)
		{
			if (preftype != PREF_TYPE_AUTOPICK)
  				break;
			err = process_autopick_file_command(buf);
		}
	}


	/* Error */
	if (err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
#ifdef JP
	      msg_format("ファイル'%s'の%d行でエラー番号%dのエラー。", name, line, err);
	      msg_format("('%s'を解析中)", old);
#else
		msg_format("Error %d in line %d of file '%s'.", err, line, name);
		msg_format("Parsing '%s'", old);
#endif
		msg_print(NULL);
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
/*:::*.prfの設定ファイルを読み込む*/
errr process_pref_file(cptr name)
{
	char buf[1024];

	errr err1, err2;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_PREF, name);

	/* Process the system pref file */
	err1 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);

	/* Stop at parser errors, but not at non-existing file */
	if (err1 > 0) return err1;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* Process the user pref file */
	err2 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);


	/* User file does not exist, but read system pref file */
	if (err2 < 0 && !err1)
		return -2;

	/* Result of user file processing */
	return err2;
}



#ifdef CHECK_TIME

/*
 * Operating hours for ANGBAND (defaults to non-work hours)
 */
static char days[7][29] =
{
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

/*
 * Restict usage (defaults to no restrictions)
 */
static bool check_time_flag = FALSE;

#endif


/*
 * Handle CHECK_TIME
 */
///sysdel
/*:::アングバンドの門処理で使われているらしい　　と思いきやmain.cからも呼ばれてる？どちらにしても使われていないか*/
errr check_time(void)
{

#ifdef CHECK_TIME

	time_t      c;
	struct tm   *tp;

	/* No restrictions */
	if (!check_time_flag) return (0);

	/* Check for time violation */
	c = time((time_t *)0);
	tp = localtime(&c);

	/* Violation */
	if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') return (1);

#endif

	/* Success */
	return (0);
}



/*
 * Initialize CHECK_TIME
 */
errr check_time_init(void)
{

#ifdef CHECK_TIME

	FILE        *fp;

	char	buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "time.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Assume restrictions */
	check_time_flag = TRUE;

	/* Parse the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Chop the buffer */
		buf[29] = '\0';

		/* Extract the info */
		if (prefix(buf, "SUN:")) strcpy(days[0], buf);
		if (prefix(buf, "MON:")) strcpy(days[1], buf);
		if (prefix(buf, "TUE:")) strcpy(days[2], buf);
		if (prefix(buf, "WED:")) strcpy(days[3], buf);
		if (prefix(buf, "THU:")) strcpy(days[4], buf);
		if (prefix(buf, "FRI:")) strcpy(days[5], buf);
		if (prefix(buf, "SAT:")) strcpy(days[6], buf);
	}

	/* Close it */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}



#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int                 cp_time[4];
	int                 dk_xfer[4];
	unsigned int        v_pgpgin;
	unsigned int        v_pgpgout;
	unsigned int        v_pswpin;
	unsigned int        v_pswpout;
	unsigned int        v_intr;
	int                 if_ipackets;
	int                 if_ierrors;
	int                 if_opackets;
	int                 if_oerrors;
	int                 if_collisions;
	unsigned int        v_swtch;
	long                avenrun[3];
	struct timeval      boottime;
	struct timeval      curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
/*:::アングバンドへの門処理でのみ使われている　消してしまおう・・と思いきやmain.c()からも使われてる？どちらにしても使われていないか*/
///sysdel
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime    st;

	/* Success if not checking */
	if (!check_load_value) return (0);

	/* Check the load */
	if (0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if (val1 >= val2) return (1);
	}

#endif

	/* Success */
	return (0);
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE        *fp;

	char	buf[1024];

	char	temphost[MAXHOSTNAMELEN+1];
	char	thishost[MAXHOSTNAMELEN+1];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		int value;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if (sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if (!streq(temphost, thishost) &&
		    !streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		/* Done */
		break;
	}

	/* Close the file */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}


#define ENTRY_BARE_HAND 0
#define ENTRY_TWO_HANDS 1
#define ENTRY_RIGHT_HAND1 2
#define ENTRY_LEFT_HAND1 3
#define ENTRY_LEFT_HAND2 4
#define ENTRY_RIGHT_HAND2 5
#define ENTRY_POSTURE 6
#define ENTRY_SHOOT_HIT_DAM 7
#define ENTRY_SHOOT_POWER 8
#define ENTRY_SPEED 9
#define ENTRY_BASE_AC 10
#define ENTRY_LEVEL 11
#define ENTRY_CUR_EXP 12
#define ENTRY_MAX_EXP 13
#define ENTRY_EXP_TO_ADV 14
#define ENTRY_GOLD 15
#define ENTRY_DAY 16
#define ENTRY_HP 17
#define ENTRY_SP 18
#define ENTRY_PLAY_TIME 19
#define ENTRY_SKILL_FIGHT 20
#define ENTRY_SKILL_SHOOT 21
#define ENTRY_SKILL_SAVING 22
#define ENTRY_SKILL_STEALTH 23
#define ENTRY_SKILL_PERCEP 24
#define ENTRY_SKILL_SEARCH 25
#define ENTRY_SKILL_DISARM 26
#define ENTRY_SKILL_DEVICE 27
#define ENTRY_BLOWS 28
#define ENTRY_SHOTS 29
#define ENTRY_AVG_DMG 30
#define ENTRY_INFRA 31

#define ENTRY_NAME 32
#define ENTRY_SEX 33
#define ENTRY_RACE 34
#define ENTRY_CLASS 35
#define ENTRY_REALM 36
#define ENTRY_PATRON 37
#define ENTRY_AGE 38
#define ENTRY_HEIGHT 39
#define ENTRY_WEIGHT 40
#define ENTRY_SOCIAL 41
#define ENTRY_ALIGN 42

#define ENTRY_EXP_ANDR 43
#define ENTRY_EXP_TO_ADV_ANDR 44
#define ENTRY_ALICE_DOLL_ATTACK 45
#define ENTRY_METAMORPHOSIS		46
#define ENTRY_MONSTER_ATTACK	47
#define ENTRY_GUN_CHANCE_RIGHT	48
#define ENTRY_GUN_CHANCE_BOTH	49
#define ENTRY_GUN_CHARGE_RIGHT	50
#define ENTRY_GUN_CHANCE_LEFT	51
#define ENTRY_GUN_CHARGE_LEFT	52
#define ENTRY_HECATIA_BODY		53
#define ENTRY_ANIMAL_GHOST_STRIFE	54
#define ENTRY_SPECIAL_ATTACK	55


///sys ステータス画面の項目とそれぞれの表示位置
static struct
{
	int col;
	int row;
	int len;
	///mod150208 何か警告出てるので20→24に変えた
	char header[24];
} disp_player_line[]
#ifdef JP
= {
	{ 1, 10, 25, "打撃修正(格闘)"},
	{ 1, 10, 25, "打撃修正(両手)"},
	{ 1, 10, 25, "打撃修正(右手)"},
	{ 1, 10, 25, "打撃修正(左手)"},
	{ 1, 11, 25, "打撃修正(左手)"},
	{ 1, 11, 25, "打撃修正(右手)"},
	{ 1, 11, 25, ""},
	{ 1, 15, 25, "射撃攻撃修正"},
	{ 1, 16, 25, "射撃武器倍率"},
	{ 1, 20, 25, "加速"},
	{ 1, 19, 25, "ＡＣ"},
	{29, 13, 21, "レベル"},
	{29, 14, 21, "経験値"},
	{29, 15, 21, "最大経験"},
	{29, 16, 21, "次レベル"},
	{29, 17, 21, "所持金"},
	{29, 19, 21, "日付"},
	{29, 10, 21, "ＨＰ"},
	{29, 11, 21, "ＭＰ"},
	{29, 20, 21, "プレイ時間"},
	{53, 10, -1, "打撃攻撃  :"},
	{53, 11, -1, "射撃攻撃  :"},
	{53, 12, -1, "魔法防御  :"},
	{53, 13, -1, "隠密行動  :"},
	{53, 15, -1, "知覚      :"},
	{53, 16, -1, "探索      :"},
	{53, 17, -1, "解除      :"},
	{53, 18, -1, "魔法道具  :"},
	{ 1, 12, 25, "打撃回数"},
	{ 1, 17, 25, "射撃回数"},
	{ 1, 13, 25, "平均ダメージ"},
	{53, 20, -1, "赤外線視力:"},
//	{26,  1, -1, "名前  : "},
	{16,  1, -1, "名前  : "},
	{ 1,  3, -1, "性別     : "},
	{ 1,  4, -1, "種族     : "},
	{ 1,  5, -1, "職業     : "},
	{ 1,  6, -1, "魔法     : "},
	{ 1,  7, -1, "守護魔神 : "},
	{29,  3, 21, "年齢"},
	{29,  4, 21, "身長"},
	{29,  5, 21, "体重"},
	//{29,  6, 21, "社会的地位"},
	//{29,  7, 21, "属性"},
	{29,  6, 21, "難易度"},
	{29,  7, 21, "スコア"},
	{29, 14, 21, "強化度"},
	{29, 16, 21, "次レベル"},
	{ 1, 10, 25, "打撃修正(人形)"},
	{ 1,  8, -1, "変身　　 : "},
	{ 1, 10, 25, "打撃修正(特殊)"},
	{ 1, 15, 25, "銃(右手)成功率"},
	{ 1, 15, 25, "銃(両手)成功率"},
	{ 1, 16, 25, "装填時間"},
	{ 1, 17, 25, "銃(左手)成功率"},
	{ 1, 18, 25, "装填時間"},
	{ 1,  7, -1, "体　　　 : "},
	{ 1,  7, -1, "所属勢力 : " },
	{ 1, 10, 25, "(特殊)" },

};
#else
= {
	{ 1, 10, 25, "Bare hand"},
	{ 1, 10, 25, "Two hands"},
	{ 1, 10, 25, "Right hand"},
	{ 1, 10, 25, "Left hand"},
	{ 1, 11, 25, "Left hand"},
	{ 1, 11, 25, "Right hand"},
	{ 1, 11, 25, ""},
	{ 1, 15, 25, "Shooting"},
	{ 1, 16, 25, "Multiplier"},
	{ 1, 20, 25, "Speed"},
	{ 1, 19, 25, "AC"},
	{29, 13, 21, "Level"},
	{29, 14, 21, "Experience"},
	{29, 15, 21, "Max Exp"},
	{29, 16, 21, "Exp to Adv"},
	{29, 17, 21, "Gold"},
	{29, 19, 21, "Time"},
	{29, 10, 21, "HP"},
	{29, 11, 21, "MP"},
	{29, 20, 21, "Play time"},
	{53, 10, -1, "Fighting   : "},
	{53, 11, -1, "Shooting   : "},
	{53, 12, -1, "ProtFrMagic: "},
	{53, 13, -1, "Stealth    : "},
	{53, 15, -1, "Perception : "},
	{53, 16, -1, "Searching  : "},
	{53, 17, -1, "Disarming  : "},
	{53, 18, -1, "MagicDevice: "},
	{ 1, 12, 25, "Blows/Round"},
	{ 1, 17, 25, "Shots/Round"},
	{ 1, 13, 25, "AverageDmg/Rnd"},
	{53, 20, -1, "Infra-Vision: "},
//	{26,  1, -1, "Name  : "},
	{16,  1, -1, "Name  : "},
	{ 1,  3, -1, "Sex      : "},
	{ 1,  4, -1, "Race     : "},
	{ 1,  5, -1, "Class    : "},
	{ 1,  6, -1, "Magic    : "},
	{ 1,  7, -1, "Patron   : "},
	{29,  3, 21, "Age"},
	{29,  4, 21, "Height"},
	{29,  5, 21, "Weight"},
	//{29,  6, 21, "社会的地位"},
	//{29,  7, 21, "属性"},
	{29,  3, 21, "Difficulty"},
	{29,  4, 21, "Score"},
	{29, 14, 21, "Construction"},
	{29, 16, 21, "Const to Adv"},
	{ 1, 10, 25, "Combat(Doll)"},
	{ 1,  8, -1, "Transform: "},
	{ 1, 10, 25, "Combat(Spc)"},
	{ 1, 15, 25, "Gun% (Right)"},
	{ 1, 15, 25, "Gun% (Both)"},
	{ 1, 16, 25, "EquipTime"},
	{ 1, 17, 25, "Gun% (Left)"},
	{ 1, 18, 25, "EquipTime"},
	{ 1,  7, -1, "Body     : "},
	{ 1,  7, -1, "Alignment: " },
	{ 1, 10, 25, "(Special)" },

};
#endif

static void display_player_one_line(int entry, cptr val, byte attr)
{
	char buf[40];

	int row = disp_player_line[entry].row;
	int col = disp_player_line[entry].col;
	int len = disp_player_line[entry].len;
	cptr head = disp_player_line[entry].header;

	int head_len = strlen(head);

	Term_putstr(col, row, -1, TERM_WHITE, head);

	if (!val)
		return;

	if (len > 0)
	{
		int val_len = len - head_len;
		sprintf(buf, "%*.*s", val_len, val_len, val);
		Term_putstr(col + head_len, row, -1, attr, buf);
	}
	else
	{
		Term_putstr(col + head_len, row, -1, attr, val);
	}

	return;
}

/*:::攻撃力表示*/
static void display_player_melee_bonus(int hand, int hand_entry)
{
	char buf[160];
	//int show_tohit = p_ptr->dis_to_h[hand];
	//v1.1.71 攻撃命中率表示に近接技能の値を反映
	int show_tohit = p_ptr->dis_to_h[hand] + p_ptr->skill_thn / BTH_PLUS_ADJ;
	int show_todam = p_ptr->dis_to_d[hand];
	object_type *o_ptr = &inventory[INVEN_RARM + hand];


	//アリスの武器熟練度を適当に計算して加算
	if (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
	{
		int i,attack_doll_num = 0, skill_sum=0,to_h_sum=0,to_d_sum=0;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			int blow_num_temp;
			o_ptr = &inven_add[i];
			if(!o_ptr->k_idx) continue;
			if(!object_is_melee_weapon(o_ptr)) continue;
			blow_num_temp = calc_doll_blow_num(i);
			attack_doll_num+= blow_num_temp;

			skill_sum += ref_skill_exp(o_ptr->tval) * blow_num_temp;

			if (object_is_known(o_ptr))
			{
				to_h_sum += o_ptr->to_h * blow_num_temp;
				to_d_sum += o_ptr->to_d * blow_num_temp;
			}

		}
		if(!attack_doll_num){msg_print(_("ERROR:アリス攻撃回数計算がおかしい", "ERROR: zero result for Alice's attack count")); return;}
		show_tohit += (skill_sum/attack_doll_num - WEAPON_EXP_BEGINNER)/200 + to_h_sum / attack_doll_num;
		show_todam += to_d_sum / attack_doll_num;
	}
	else if(!p_ptr->do_martialarts)
	{
		/* Hack -- add in weapon info if known */
		if (object_is_known(o_ptr) && !check_invalidate_inventory(INVEN_RARM + hand)) show_tohit += o_ptr->to_h;
		if (object_is_known(o_ptr) && !check_invalidate_inventory(INVEN_RARM + hand)) show_todam += o_ptr->to_d;
	}

	/* Melee attacks */
	sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

	/* Dump the bonuses to hit/dam */
	if(IS_METAMORPHOSIS)
		display_player_one_line(ENTRY_MONSTER_ATTACK, "(---,---)", TERM_L_BLUE);
	//v2.0.9 美宵特殊
	else if (p_ptr->pclass == CLASS_MIYOI && p_ptr->do_martialarts && !p_ptr->mimic_form)
	{
		display_player_one_line(ENTRY_SPECIAL_ATTACK, "(---,---)", TERM_L_BLUE);
	}
	else if (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
		display_player_one_line(ENTRY_ALICE_DOLL_ATTACK, buf, TERM_L_BLUE);
	else if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		display_player_one_line(ENTRY_BARE_HAND, buf, TERM_L_BLUE);
	else if (p_ptr->ryoute)
		display_player_one_line(ENTRY_TWO_HANDS, buf, TERM_L_BLUE);
	else
		display_player_one_line(hand_entry, buf, TERM_L_BLUE);
}


/*
 * Prints the following information on the screen.
 */
///sys Cコマンドによる＠の情報　1ページ目中　スキルや攻撃力など
///mod140308 弓枠廃止処理関連
///mod160508 銃仕様変更に伴い射撃関係の表記変更
static void display_player_middle(void)
{
	char buf[160];

	/* Base skill */
	int show_tohit = p_ptr->dis_to_h_b;
	int show_todam = 0;
	bool bow1=FALSE,bow2=FALSE;
	/* Range weapon */
	//object_type *o_ptr = &inventory[INVEN_BOW];
	object_type *o_ptr;

	int tmul = 0;
	int e;
	int tester;
	bool flag_use_gun = FALSE;

	//v1.1.71 射撃技能を反映...しようとしたらもう書いてた。
	/*
	if (object_is_shooting_weapon(&inventory[INVEN_RARM]))
	{
		bow1 = TRUE;
		o_ptr = &inventory[INVEN_RARM];
	}
	if (object_is_shooting_weapon(&inventory[INVEN_LARM]))
	{
		bow2 = TRUE;
		o_ptr = &inventory[INVEN_LARM];
	}

	show_tohit += p_ptr->skill_thb / BTH_PLUS_ADJ;
	//射撃武器熟練度の値も反映。どちらかの手に射撃武器を持っているときのみ。
	//do_cmd_fire_aux()と同じ計算式を記述。射撃技能値は投擲技能値と共同なので技能値にまとめることはできない。
	if (bow1 && !bow2 || !bow1 && bow2)
	{
		if (o_ptr->tval == TV_CROSSBOW)
			show_tohit += ref_skill_exp(TV_CROSSBOW) / 400;
		else if (o_ptr->tval == TV_BOW)
			show_tohit += (ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200;

	}
	*/

	if(CHECK_USE_GUN) flag_use_gun = TRUE;

	o_ptr =  &inventory[INVEN_PACK + shootable(&tester)];

	if (p_ptr->migite)
	{
		display_player_melee_bonus(0, left_hander ? ENTRY_LEFT_HAND1 : ENTRY_RIGHT_HAND1);
	}

	if (p_ptr->hidarite)
	{
		display_player_melee_bonus(1, left_hander ? ENTRY_RIGHT_HAND2: ENTRY_LEFT_HAND2);
	}
	///sysdel  修行僧の構え表示
	else if ((p_ptr->pclass == CLASS_MONK) && (empty_hands(TRUE) & EMPTY_HAND_RARM))
	{
		int i;
		if (p_ptr->special_defense & KAMAE_MASK)
		{
			for (i = 0; i < MAX_KAMAE; i++)
			{
				if ((p_ptr->special_defense >> i) & KAMAE_GENBU) break;
			}
			if (i < MAX_KAMAE)
#ifdef JP
				display_player_one_line(ENTRY_POSTURE, format("%sの構え", kamae_shurui[i].desc), TERM_YELLOW);
#else
				display_player_one_line(ENTRY_POSTURE, format("%s form", kamae_shurui[i].desc), TERM_YELLOW);
#endif
		}
		else
#ifdef JP
				display_player_one_line(ENTRY_POSTURE, "構えなし", TERM_YELLOW);
#else
				display_player_one_line(ENTRY_POSTURE, "none", TERM_YELLOW);
#endif
	}

	/* Apply weapon bonuses */
	if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

	if(flag_use_gun)
	{
		int chance;
		int timeout;

		if(inventory[INVEN_RARM].tval == TV_GUN)
		{
			int kyouten_card_num = 0;
			chance = calc_gun_fire_chance(INVEN_RARM);
			timeout = calc_gun_timeout(&inventory[INVEN_RARM]) * calc_gun_charge_mult(INVEN_RARM);

			//v1.1.87d 法力経典による充填ブースト反映
			kyouten_card_num = count_ability_card(ABL_CARD_KYOUTEN);
			if (kyouten_card_num)
			{
				timeout = timeout * 100 / (100+CALC_ABL_KYOUTEN_RECHARGE_BONUS(kyouten_card_num));
			}

			//銃両手持ち
			if(!inventory[INVEN_LARM].k_idx)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_BOTH,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else if(left_hander)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_LEFT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else
			{
				display_player_one_line(ENTRY_GUN_CHANCE_RIGHT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}

		}
		if(inventory[INVEN_LARM].tval == TV_GUN)
		{
			chance = calc_gun_fire_chance(INVEN_LARM);
			timeout = calc_gun_timeout(&inventory[INVEN_LARM]) * calc_gun_charge_mult(INVEN_LARM);
			//銃両手持ち(右手に武器左手に銃→銃呪われる→武器外す、で左手両手持ちにもなりうるはず？)
			if(!inventory[INVEN_RARM].k_idx)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_BOTH,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else if(left_hander)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_RIGHT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else
			{
				display_player_one_line(ENTRY_GUN_CHANCE_LEFT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
		}
	}
	else
	{
		///item 弓の命中率とクロスボウの命中ボーナス　ステータス画面用
		///mod131227 skill tval 射撃命中率
		///item もし銃を実装したらここで命中率計算要
		if (o_ptr->tval == TV_CROSSBOW)
			show_tohit += ref_skill_exp(TV_CROSSBOW) / 400;
		else
			show_tohit += (ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200;
		/* Range attacks */
		display_player_one_line(ENTRY_SHOOT_HIT_DAM, format("(%+d,%+d)", show_tohit, show_todam), TERM_L_BLUE);

		if(o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
		{
			tmul = bow_tmul(o_ptr->tval, o_ptr->sval);
			if (p_ptr->xtra_might) tmul++;
			tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		}

		/* shoot power */
		display_player_one_line(ENTRY_SHOOT_POWER, format("x%d.%02d", tmul/100, tmul%100), TERM_L_BLUE);

	}


	/* Dump the armor class */
	display_player_one_line(ENTRY_BASE_AC, format("[%d,%+d]", p_ptr->dis_ac, p_ptr->dis_to_a), TERM_L_BLUE);

	/* Dump speed */
	{
		int tmp_speed = 0;
		byte attr;
		int i;

		i = p_ptr->pspeed-110;

		/* Hack -- Visually "undo" the Search Mode Slowdown */
		if (p_ptr->action == ACTION_SEARCH) i += 10;

		if (i > 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_GREEN;
			else
				attr = TERM_GREEN;
		}
		else if (i == 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_BLUE;
			else
				attr = TERM_GREEN;
		}
		else
		{
			if (!p_ptr->riding)
				attr = TERM_L_UMBER;
			else
				attr = TERM_RED;
		}

		if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
		{
			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0]) tmp_speed += 20;
			else if (IS_FAST()) tmp_speed += 10;
			if (p_ptr->slow) tmp_speed -= 10;
			if (p_ptr->lightspeed) tmp_speed = 99;
		}
		else
		{
			if (MON_FAST(&m_list[p_ptr->riding])) tmp_speed += 10;
			if (MON_SLOW(&m_list[p_ptr->riding])) tmp_speed -= 10;
		}

		if (tmp_speed)
		{
			if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
				sprintf(buf, "(%+d%+d)", i-tmp_speed, tmp_speed);
			else
#ifdef JP
				sprintf(buf, "乗馬中 (%+d%+d)", i-tmp_speed, tmp_speed);
#else
				sprintf(buf, "Riding (%+d%+d)", i-tmp_speed, tmp_speed);
#endif

			if (tmp_speed > 0)
				attr = TERM_YELLOW;
			else
				attr = TERM_VIOLET;
		}
		else
		{
			if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
				sprintf(buf, "(%+d)", i);
			else
#ifdef JP
				sprintf(buf, "乗馬中 (%+d)", i);
#else
				sprintf(buf, "Riding (%+d)", i);
#endif
		}

		display_player_one_line(ENTRY_SPEED, buf, attr);
	}

	/* Dump character level */
	display_player_one_line(ENTRY_LEVEL, format("%d", p_ptr->lev), TERM_L_GREEN);

	/* Dump experience */
	if (p_ptr->prace == RACE_ANDROID) e = ENTRY_EXP_ANDR;
	else e = ENTRY_CUR_EXP;

	if (p_ptr->exp >= p_ptr->max_exp)
		display_player_one_line(e, format("%ld", (long) p_ptr->exp), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", (long) p_ptr->exp), TERM_YELLOW);

	/* Dump max experience */
	if (p_ptr->prace == RACE_ANDROID)
		/* Nothing */;
	else
		display_player_one_line(ENTRY_MAX_EXP, format("%ld", (long) p_ptr->max_exp), TERM_L_GREEN);

	/* Dump exp to advance */
	if (p_ptr->prace == RACE_ANDROID) e = ENTRY_EXP_TO_ADV_ANDR;
	else e = ENTRY_EXP_TO_ADV;

	if (p_ptr->lev >= PY_MAX_LEVEL)
		display_player_one_line(e, "*****", TERM_L_GREEN);
	else if (p_ptr->prace == RACE_ANDROID)
		display_player_one_line(e, format("%ld", (long)(player_exp_a[p_ptr->lev - 1] * p_ptr->expfact / 100L)), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", (long)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L)), TERM_L_GREEN);

	/* Dump gold */
	display_player_one_line(ENTRY_GOLD, format("%ld", (long)p_ptr->au), TERM_L_GREEN);

	/* Dump Day */
	{
		int day, hour, min;
		extract_day_hour_min(&day, &hour, &min);

#ifdef JP
		if (day < MAX_DAYS) sprintf(buf, "%d日目 %2d:%02d", day, hour, min);
		else sprintf(buf, "*****日目 %2d:%02d", hour, min);
#else
		if (day < MAX_DAYS) sprintf(buf, "Day %d %2d:%02d", day, hour, min);
		else sprintf(buf, "Day ***** %2d:%02d", hour, min);
#endif
	}
	display_player_one_line(ENTRY_DAY, buf, TERM_L_GREEN);

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		if (p_ptr->magic_num1[0] < 10000)
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_L_GREEN);
		else if (p_ptr->magic_num1[0] < 20000)
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_RED);
	}
	else
	{
		/* Dump hit point */
		if (p_ptr->chp >= p_ptr->mhp)
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_L_GREEN);
		else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_RED);
	}

	/* Dump mana power */
	if (p_ptr->csp >= p_ptr->msp)
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_L_GREEN);
	else if (p_ptr->csp > (p_ptr->msp * mana_warn) / 10)
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_RED);

	/* Dump play time */
	display_player_one_line(ENTRY_PLAY_TIME, format("%.2lu:%.2lu:%.2lu", (unsigned long)playtime/(60*60), (unsigned long)(playtime/60)%60, (unsigned long)playtime%60), TERM_L_GREEN);
}


/*
 * Hack -- pass color info around this file
 */
static byte likert_color = TERM_WHITE;


/*
 * Returns a "rating" of x depending on y
 */
//v1.1.81 本家の「技能値に実値を表示する」オプションを追加
static cptr likert(int x, int y)
{
	static char dummy[40] = "";

	/* Paranoia */
	if (y <= 0) y = 1;

	/* Negative value */
	if (x < 0)
	{
		likert_color = TERM_L_DARK;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-最低", x);
		else
			sprintf(dummy, "最低");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Very Bad", x);
		else
			sprintf(dummy, "Very Bad");
		return dummy;
#endif
	}

	/* Analyze the value */
	switch ((x / y))
	{
	case 0:
	case 1:
		likert_color = TERM_RED;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-悪い", x);
		else
			sprintf(dummy, "悪い");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Bad", x);
		else
			sprintf(dummy, "Bad");
		return dummy;
#endif

	case 2:
		likert_color = TERM_L_RED;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-劣る", x);
		else
			sprintf(dummy, "劣る");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Poor", x);
		else
			sprintf(dummy, "Poor");
		return dummy;
#endif

	case 3:
	case 4:
		likert_color = TERM_ORANGE;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-普通", x);
		else
			sprintf(dummy, "普通");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Fair", x);
		else
			sprintf(dummy, "Fair");
		return dummy;
#endif

	case 5:
		likert_color = TERM_YELLOW;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-良い", x);
		else
			sprintf(dummy, "良い");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Good", x);
		else
			sprintf(dummy, "Good");
		return dummy;
#endif

	case 6:
		likert_color = TERM_YELLOW;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-大変良い", x);
		else
			sprintf(dummy, "大変良い");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Very Good", x);
		else
			sprintf(dummy, "Very Good");
		return dummy;
#endif

	case 7:
	case 8:
		likert_color = TERM_L_GREEN;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-卓越", x);
		else
			sprintf(dummy, "卓越");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Excellent", x);
		else
			sprintf(dummy, "Excellent");
		return dummy;
#endif

	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		likert_color = TERM_GREEN;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-超越", x);
		else
			sprintf(dummy, "超越");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Superb", x);
		else
			sprintf(dummy, "Superb");
		return dummy;
#endif

	case 14:
	case 15:
	case 16:
	case 17:
		likert_color = TERM_BLUE;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-英雄的", x);
		else
			sprintf(dummy, "英雄的");
		return dummy;
#else
		if (show_actual_value)
			sprintf(dummy, "%3d-Heroic", x);
		else
			sprintf(dummy, "Heroic");
		return dummy;
#endif

	default:
		likert_color = TERM_VIOLET;
#ifdef JP
		if(show_actual_value)
			sprintf(dummy, "%3d-伝説的[%d]",x, (int)((((x / y) - 17) * 5) / 2));
		else
			sprintf(dummy, "伝説的[%d]", (int)((((x / y) - 17) * 5) / 2));
#else
		if(show_actual_value)
			sprintf(dummy, "%3d-Legendary[%d]",x, (int)((((x / y) - 17) * 5) / 2));
		else
			sprintf(dummy, "Legendary[%d]", (int)((((x / y) - 17) * 5) / 2));
#endif
		return dummy;
	}
}

///mod140223 格闘攻撃の威力(100発分合計)を概算する。一行動ごとの攻撃回数は掛けられていない。
//本家では職や構えごとの専用テーブルを使ってたが職と攻撃手段の増加で不可能になったためpy_attack_aux()の計算処理を100回試行して合計することにした。
//Cコマンドを実行するたびに値がずれるが仕方ない。
//v1.1.65 display_player_various()内にあったが別関数に分離
int calc_martialarts_dam_x100(hand_idx)
{
	int sum=0;
	int num = 100;
	int count;

	if (!p_ptr->do_martialarts)
	{
		msg_print(_("ERROR:格闘でないときにcalc_martialarts_dam()が呼ばれた",
                    "ERROR: calc_martialarts_dam() called when not using martial arts"));
		return 0;
	}

	if (hand_idx == 0 && !p_ptr->migite || hand_idx == 1 && !p_ptr->hidarite)
		return 0;

	for (count = 0; count<num; count++)
	{
		const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
		int times;
		int martial_arts_method = find_martial_arts_method(0);//ここではmodeは常に0
		int max_times = ref_skill_exp(SKILL_MARTIALARTS) / 2000 + 1; //格闘熟練度による、攻撃種類選定試行数。攻撃回数ではない
		for (times = 0; times < max_times; times++)
		{
			int min_level = 1;
			do
			{
				int mode_pick[50];
				int mode_cnt = 0;
				int j;
				for (j = 0; (ma_blows_new[j].method != 0 && mode_cnt < 50); j++)
					if (ma_blows_new[j].method == martial_arts_method)mode_pick[mode_cnt++] = j;

				if (!mode_cnt) return 0;//v1.1.92 専用格闘攻撃を設定し忘れてキャラメイク画面でクラッシュしたので追加

				ma_ptr = &ma_blows_new[mode_pick[randint0(mode_cnt)]];
				min_level = ma_ptr->min_level;
			} while ((min_level > p_ptr->lev) || (randint1(p_ptr->lev) < ma_ptr->chance));

			if ((ma_ptr->min_level >= old_ptr->min_level))	old_ptr = ma_ptr;
			else ma_ptr = old_ptr;
		}
		if (p_ptr->pclass == CLASS_KISUME) sum += calc_kisume_bonus();
		sum += ((ma_ptr->dd + p_ptr->to_dd[hand_idx]) * (ma_ptr->ds + p_ptr->to_ds[hand_idx] + 1)) / 2;
	}

	sum += p_ptr->dis_to_d[hand_idx] * num;

	return sum;

}


/*
 * Prints ratings on certain abilities
 *
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
/*:::ステータス画面　＠のスキルや変異攻撃など*/
///sys ＠のスキルや変異攻撃など表示
///mod140308 弓枠廃止処理関連
///mod140323 変異格闘など記述変更
static void display_player_various(void)
{
	int         tmp, damage[2], to_h[2], blows1, blows2, i, basedam;
	int			xthn, xthb, xfos, xsrh;
	int			xdis, xdev, xsav, xstl;
	cptr		desc;
	///mod140323 変異格闘の攻撃回数だったが追加格闘全般の回数として使うことにした
	int         muta_att = 0;
	u32b flgs[TR_FLAG_SIZE];
	int		shots, shot_frac,tester;
	bool dokubari;
	bool alice = (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts);
	bool flag_use_gun = FALSE;

	object_type		*o_ptr;

	if(CHECK_USE_GUN) flag_use_gun = TRUE;

	//突然変異などで追加格闘が出る可能性があるときはここでmuta_attを加算してステータス画面の攻撃回数のところに「+α」を表示する
	if (p_ptr->pclass == CLASS_AUNN && p_ptr->multishadow) muta_att++;
	if((!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_SHIELD || !check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_SHIELD) && ref_skill_exp(SKILL_SHIELD) > 3200) muta_att++;
	if (p_ptr->muta2 & MUT2_HARDHEAD || (buki_motteruka(INVEN_RARM) && ref_skill_exp(SKILL_MARTIALARTS) > 3200)) muta_att++;
	if (p_ptr->muta2 & MUT2_HORNS)     muta_att++;
	if (p_ptr->muta2 & MUT2_BIGHORN)      muta_att++;
	if (p_ptr->muta2 & MUT2_BIGTAIL) muta_att++;
	if (p_ptr->muta3 & MUT3_FISH_TAIL) muta_att++;
	if(inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].tval == TV_STICK && p_ptr->ryoute)
	{
		int num_xtra = 0;
		if(p_ptr->pclass == CLASS_MARTIAL_ARTIST) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 3000) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 4500) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 6000) num_xtra++;
		if(heavy_armor()) num_xtra /= 2;
		if(num_xtra) muta_att++;
	}
	xthn = p_ptr->skill_thn + (p_ptr->to_h_m * BTH_PLUS_ADJ);
	xthb = p_ptr->skill_thb + (p_ptr->to_h_b * BTH_PLUS_ADJ);

	///mod160508 銃使用時には射撃速度パラメータを表示しない。display_player_middle()で発動成功率や充填速度などを表示する
	if(!flag_use_gun)
	{
		/* Shooting Skill (with current bow and normal missile) */
		//o_ptr = &inventory[INVEN_BOW];
		o_ptr = &inventory[INVEN_PACK + shootable(&tester)];
		///160508 今の今まで知らなかったんだが射撃の技能値表示には武器の命中率も含まれるらしい。銃の表記と整合性を取りにくいが適当に計算しよう
		xthb +=  (o_ptr->to_h * BTH_PLUS_ADJ);

		/* If the player is wielding one? */
		if (tester > SHOOT_UNSUITABLE && o_ptr->k_idx)
		{
			s16b energy_fire = bow_energy(o_ptr->tval, o_ptr->sval);

			/* Calculate shots per round */
			shots = p_ptr->num_fire * 100;
			shot_frac = (shots * 100 / energy_fire) % 100;
			shots = shots / energy_fire;
#if 0
			//アーチャーのクリムゾン連射処理を廃止した
			if (o_ptr->name1 == ART_CRIMSON)
			{
				shots = 1;
				shot_frac = 0;
				if (p_ptr->pclass == CLASS_ARCHER)
				{
					/* Extra shot at level 10 */
					if (p_ptr->lev >= 10) shots++;

					/* Extra shot at level 30 */
					if (p_ptr->lev >= 30) shots++;

					/* Extra shot at level 45 */
					if (p_ptr->lev >= 45) shots++;
				}
			}
#endif
		}
		else
		{
			shots = 0;
			shot_frac = 0;
		}
	}

	if(alice)//アリス隣接攻撃の特殊処理
	{
		blows1 = 0;
		blows2 = 0;
		damage[0] = 0;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			int tmp_blows = calc_doll_blow_num(i);
			if(!tmp_blows) continue;
			blows1 += tmp_blows;
			dokubari = FALSE;

			o_ptr = &inven_add[i];
			if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;

			if(dokubari)
				damage[0] += 100; //あとで100で割る
			else
			{
				basedam = ((int)(o_ptr->dd) * (int)(o_ptr->ds + 1)) * 50;
				object_flags_known(o_ptr, flgs);

				if (have_flag(flgs, TR_EX_VORPAL))
					basedam = basedam * 5 / 3;
				else if (have_flag(flgs, TR_VORPAL))
					basedam = basedam * 11 / 9;
				basedam = calc_expect_crit(o_ptr->weight, o_ptr->to_h, basedam, p_ptr->dis_to_h[0], dokubari);

				damage[0] += basedam  * tmp_blows;
				damage[0] += p_ptr->dis_to_d[0] * tmp_blows * 100;

				if (object_is_known(o_ptr))
					damage[0] += o_ptr->to_d * tmp_blows * 100;
			}

		}


	}
	else if(IS_METAMORPHOSIS) //モンスター変身時
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		damage[0] = 0;
		blows1 = 0;
		blows2 = 0;
		for(i=0;i<4;i++)
		{
			if(!r_ptr->blow[i].method) continue;
			if(r_ptr->blow[i].method == RBM_SHOOT) continue;
			if(r_ptr->blow[i].effect == RBE_DR_MANA) continue;
			damage[0] += r_ptr->blow[i].d_dice * (r_ptr->blow[i].d_side+1) / 2;
			blows1++;
		}
	}
	else if (p_ptr->do_martialarts)//普通の格闘 v1.1.65で別関数に分離した
	{
		for (i = 0; i < 2; i++)
		{
			damage[i] = calc_martialarts_dam_x100(i);
		}
		blows1 = p_ptr->migite ? p_ptr->num_blow[0] : 0;
		blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;

	}
	else//武器攻撃
	{
		for(i = 0; i < 2; i++)
		{
			damage[i] = p_ptr->dis_to_d[i] * 100;
			o_ptr = &inventory[INVEN_RARM + i];

			/* Average damage per round */

			if (o_ptr->k_idx)
			{
				int vorpal = 0; //v1.1.45 ネムノの包丁研ぎで一時切れ味付加したときステータス画面に反映
				to_h[i] = 0;
				dokubari = FALSE;
				///item 毒針
				///mod131223
				if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;
				if (object_is_known(o_ptr))
				{
					damage[i] += o_ptr->to_d * 100;
					to_h[i] += o_ptr->to_h;
				}
				//basedam = ((o_ptr->dd + p_ptr->to_dd[i]) * (o_ptr->ds + p_ptr->to_ds[i] + 1)) * 50;
				basedam = ((int)(o_ptr->dd + p_ptr->to_dd[i]) * (int)(o_ptr->ds + p_ptr->to_ds[i] + 1)) * 50;
				object_flags_known(o_ptr, flgs);

				basedam = calc_expect_crit(o_ptr->weight, to_h[i], basedam, p_ptr->dis_to_h[i], dokubari);
				//if ((o_ptr->ident & IDENT_MENTAL) && ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)))
				if (have_flag(flgs, TR_EX_VORPAL)) vorpal = 2;
				else if (have_flag(flgs, TR_VORPAL)) vorpal = 1;

				if (object_has_a_blade(o_ptr) && p_ptr->special_attack & ATTACK_VORPAL) vorpal++;

				if(vorpal >= 2)
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if(vorpal == 1)
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}



				if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON) || p_ptr->special_attack & ATTACK_FORCE )&& (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
					basedam = basedam * 7 / 2;
			}
			else basedam = 0;

			damage[i] += basedam;

			if (!p_ptr->do_martialarts && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) damage[i] = 1;
			if (damage[i] < 0) damage[i] = 0;
		}
		blows1 = p_ptr->migite ? p_ptr->num_blow[0]: 0;
		blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;
	}
	/* Basic abilities */

	xdis = p_ptr->skill_dis;
	xdev = p_ptr->skill_dev;
	xsav = p_ptr->skill_sav;
	xstl = p_ptr->skill_stl;
	xsrh = p_ptr->skill_srh;
	xfos = p_ptr->skill_fos;

	desc = likert(xthn, 12);
	display_player_one_line(ENTRY_SKILL_FIGHT, desc, likert_color);

	desc = likert(xthb, 12);
	display_player_one_line(ENTRY_SKILL_SHOOT, desc, likert_color);

	desc = likert(xsav, 7);
	display_player_one_line(ENTRY_SKILL_SAVING, desc, likert_color);

	/* Hack -- 0 is "minimum stealth value", so print "Very Bad" */
	desc = likert((xstl > 0) ? xstl : -1, 1);
	display_player_one_line(ENTRY_SKILL_STEALTH, desc, likert_color);

	/*:::*/
	desc = likert(xfos, 6);
	//display_player_one_line(ENTRY_SKILL_PERCEP, desc, likert_color);

	desc = likert(xsrh, 6);
	display_player_one_line(ENTRY_SKILL_SEARCH, desc, likert_color);

	desc = likert(xdis, 8);
	display_player_one_line(ENTRY_SKILL_DISARM, desc, likert_color);

	desc = likert(xdev, 6);
	display_player_one_line(ENTRY_SKILL_DEVICE, desc, likert_color);

	if(!flag_use_gun)
		display_player_one_line(ENTRY_SHOTS, format("%d.%02d", shots, shot_frac), TERM_L_BLUE);

	if(alice)
	{
		if (!muta_att)
			display_player_one_line(ENTRY_BLOWS, format("%d", blows1), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_BLOWS, format(_("%d+α", "%d+@"), blows1), TERM_L_BLUE);

		if(!muta_att)
			desc = format("%d", damage[0] / 100);//アリスのdamage[0]は攻撃回数分合計済み
		else
			desc = format(_("%d+α", "%d+@"), damage[0] / 100);

	}
	else if (p_ptr->pclass == CLASS_MIYOI && p_ptr->do_martialarts && !p_ptr->mimic_form)
	{
		;//美宵特集格闘はダメージを表示しない
	}
	else if(IS_METAMORPHOSIS)
	{
		display_player_one_line(ENTRY_BLOWS, format("%d", blows1), TERM_L_BLUE);
		desc = format("%d", damage[0]);

	}
	else
	{
		//py_attack()の追加攻撃不可条件と一致すること
		if((mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_EXATACK)
		|| p_ptr->pclass == CLASS_3_FAIRIES ) muta_att = 0;

		if (!muta_att)
			display_player_one_line(ENTRY_BLOWS, format("%d+%d", blows1, blows2), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_BLOWS, format(_("%d+%d+α","%d+%d+@"), blows1, blows2), TERM_L_BLUE);

		if ((damage[0]+damage[1]) == 0)
			desc = _("(攻撃不可)", "(can't attack)");
		else if(!muta_att)
			desc = format("%d+%d", blows1 * damage[0] / 100, blows2 * damage[1] / 100);
		///mod140323 変異部位や追加格闘のダメージは近似値計算するのも面倒なので+αでごまかすことにした
		else
			desc = format(_("%d+%d+α","%d+%d+@"), blows1 * damage[0] / 100, blows2 * damage[1] / 100);
	}

	if (p_ptr->pclass == CLASS_MIYOI && p_ptr->do_martialarts && !p_ptr->mimic_form)
	{
		;//美宵特集格闘はダメージを表示しない
	}
	else
	{
		display_player_one_line(ENTRY_AVG_DMG, desc, TERM_L_BLUE);
	}

	if(p_ptr->do_martialarts && p_ptr->pclass == CLASS_MEIRIN)
	{
		int num = 100 / p_ptr->magic_num1[0];
		int num_flac = 10000 / p_ptr->magic_num1[0] % 100;
		Term_putstr(1, 11, -1, TERM_WHITE, _("攻撃速度", "Attack speed"));
		Term_putstr(21, 11, -1, TERM_L_BLUE, format("x%d.%02d",num,num_flac));
	}
	else if(p_ptr->pclass == CLASS_RAIKO && music_singing(MUSIC_NEW_RAIKO_PRISTINE))
	{
		int need = MAX((75 - p_ptr->concent * 5),25);
		int num = 100 / need;
		int num_flac = 10000 / need % 100;
		Term_putstr(1, 14, -1, TERM_WHITE, _("攻撃速度", "Attack speed"));
		Term_putstr(21, 14, -1, TERM_L_BLUE, format("x%d.%02d",num,num_flac));
	}

	display_player_one_line(ENTRY_INFRA, format("%d feet", p_ptr->see_infra * 10), TERM_WHITE);
}





#define PLAYER_ADD_OBJ_FLG_MODE_RESIST	0x00000001L //耐性
#define PLAYER_ADD_OBJ_FLG_MODE_IMMUNE	0x00000002L //免疫
#define PLAYER_ADD_OBJ_FLG_MODE_SPEED	0x00000004L //加速(他のpvalと表示される場所が違うので)
#define PLAYER_ADD_OBJ_FLG_MODE_PVAL	0x00000008L //加速以外のpval(未使用)
#define PLAYER_ADD_OBJ_FLG_MODE_SLAYS	0x00000010L //武器のスレイや属性(未使用)

#define PLAYER_ADD_OBJ_FLG_MODE_WARNING	0x00000040L //警告(銘処理が必要かもしれないので分ける)
#define PLAYER_ADD_OBJ_FLG_MODE_OTHERS	0x00000080L //ESPや能力維持や透明視認などほかの能力(投擲や乗馬など対象外のフラグもある)




//player_flags()とplayer_immunity()内でのみ使う
//特別なインベントリにアイテムを装備している＠がそのアイテムの耐性フラグなどを＠のフラグとして表示するための関数
//obj_flgsからflgsへ必要なフラグをコピー
//今のところ饕餮専用　いずれ女苑とかアリスも統合したい
static void player_add_object_flag(u32b flgs[TR_FLAG_SIZE], u32b obj_flgs[TR_FLAG_SIZE], u32b mode)
{

	//必要なフラグをコピー(同じflgsなのでそのままコピーしようかと思ったがpvalやスレイなど関係ないフラグも多いので一つ一つコピーする

	//耐性
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_RESIST)
	{

		if (have_flag(obj_flgs, TR_RES_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(obj_flgs, TR_RES_ELEC)) add_flag(flgs, TR_RES_ELEC);
		if (have_flag(obj_flgs, TR_RES_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(obj_flgs, TR_RES_COLD)) add_flag(flgs, TR_RES_COLD);
		if (have_flag(obj_flgs, TR_RES_POIS)) add_flag(flgs, TR_RES_POIS);
		if (have_flag(obj_flgs, TR_RES_LITE)) add_flag(flgs, TR_RES_LITE);
		if (have_flag(obj_flgs, TR_RES_DARK)) add_flag(flgs, TR_RES_DARK);
		if (have_flag(obj_flgs, TR_RES_SHARDS)) add_flag(flgs, TR_RES_SHARDS);
		if (have_flag(obj_flgs, TR_RES_SOUND)) add_flag(flgs, TR_RES_SOUND);
		if (have_flag(obj_flgs, TR_RES_NETHER)) add_flag(flgs, TR_RES_NETHER);
		if (have_flag(obj_flgs, TR_RES_WATER)) add_flag(flgs, TR_RES_WATER);
		if (have_flag(obj_flgs, TR_RES_CHAOS)) add_flag(flgs, TR_RES_CHAOS);
		if (have_flag(obj_flgs, TR_RES_DISEN)) add_flag(flgs, TR_RES_DISEN);
		if (have_flag(obj_flgs, TR_RES_HOLY)) add_flag(flgs, TR_RES_HOLY);
		if (have_flag(obj_flgs, TR_RES_TIME)) add_flag(flgs, TR_RES_TIME);

		if (have_flag(obj_flgs, TR_FREE_ACT)) add_flag(flgs, TR_FREE_ACT);
		if (have_flag(obj_flgs, TR_RES_BLIND)) add_flag(flgs, TR_RES_BLIND);
		if (have_flag(obj_flgs, TR_RES_FEAR)) add_flag(flgs, TR_RES_FEAR);
		if (have_flag(obj_flgs, TR_RES_CONF)) add_flag(flgs, TR_RES_CONF);
		if (have_flag(obj_flgs, TR_RES_INSANITY)) add_flag(flgs, TR_RES_INSANITY);

	}

	//免疫
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_IMMUNE)
	{

		if (have_flag(obj_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(obj_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		if (have_flag(obj_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(obj_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);

	}

	//加速
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_SPEED)
	{
		if (have_flag(obj_flgs, TR_SPEED)) add_flag(flgs, TR_SPEED);
	}

	//加速以外のpvalは未使用
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_PVAL)
	{
		msg_print(_("未実装", "Unimplemented"));
	}
	//武器のスレイや属性(未実装)
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_SLAYS)
	{
		msg_print(_("未実装", "Unimplemented"));
	}
	//警告
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_WARNING)
	{
		if (have_flag(obj_flgs, TR_WARNING)) add_flag(flgs, TR_WARNING);
	}

	//その他の能力
	if (mode & PLAYER_ADD_OBJ_FLG_MODE_OTHERS)
	{

		if (have_flag(obj_flgs, TR_ESP_EVIL)) add_flag(flgs, TR_ESP_EVIL);
		if (have_flag(obj_flgs, TR_ESP_GOOD))  add_flag(flgs, TR_ESP_GOOD);
		if (have_flag(obj_flgs, TR_ESP_ANIMAL))  add_flag(flgs, TR_ESP_ANIMAL);
		if (have_flag(obj_flgs, TR_ESP_HUMAN))  add_flag(flgs, TR_ESP_HUMAN);
		if (have_flag(obj_flgs, TR_ESP_UNDEAD))  add_flag(flgs, TR_ESP_UNDEAD);
		if (have_flag(obj_flgs, TR_ESP_DRAGON))  add_flag(flgs, TR_ESP_DRAGON);
		if (have_flag(obj_flgs, TR_ESP_DEITY))  add_flag(flgs, TR_ESP_DEITY);
		if (have_flag(obj_flgs, TR_ESP_DEMON))  add_flag(flgs, TR_ESP_DEMON);
		if (have_flag(obj_flgs, TR_ESP_KWAI))  add_flag(flgs, TR_ESP_KWAI);
		if (have_flag(obj_flgs, TR_ESP_NONLIVING))  add_flag(flgs, TR_ESP_NONLIVING);
		if (have_flag(obj_flgs, TR_ESP_UNIQUE))  add_flag(flgs, TR_ESP_UNIQUE);
		if (have_flag(obj_flgs, TR_TELEPATHY))  add_flag(flgs, TR_TELEPATHY);

		if (have_flag(obj_flgs, TR_SUST_STR)) add_flag(flgs, TR_SUST_STR);
		if (have_flag(obj_flgs, TR_SUST_INT)) add_flag(flgs, TR_SUST_INT);
		if (have_flag(obj_flgs, TR_SUST_WIS)) add_flag(flgs, TR_SUST_WIS);
		if (have_flag(obj_flgs, TR_SUST_DEX)) add_flag(flgs, TR_SUST_DEX);
		if (have_flag(obj_flgs, TR_SUST_CON)) add_flag(flgs, TR_SUST_CON);
		if (have_flag(obj_flgs, TR_SUST_CHR)) add_flag(flgs, TR_SUST_CHR);

		if (have_flag(obj_flgs, TR_EASY_SPELL)) add_flag(flgs, TR_EASY_SPELL);
		if (have_flag(obj_flgs, TR_DEC_MANA)) add_flag(flgs, TR_DEC_MANA);
		if (have_flag(obj_flgs, TR_REFLECT)) add_flag(flgs, TR_REFLECT);

		if (have_flag(obj_flgs, TR_LEVITATION)) add_flag(flgs, TR_LEVITATION);
		if (have_flag(obj_flgs, TR_SEE_INVIS)) add_flag(flgs, TR_SEE_INVIS);
		if (have_flag(obj_flgs, TR_SLOW_DIGEST)) add_flag(flgs, TR_SLOW_DIGEST);
		if (have_flag(obj_flgs, TR_REGEN)) add_flag(flgs, TR_REGEN);
		if (have_flag(obj_flgs, TR_LITE)) add_flag(flgs, TR_LITE);
		if (have_flag(obj_flgs, TR_SPEEDSTER)) add_flag(flgs, TR_SPEEDSTER);

		if (have_flag(obj_flgs, TR_SH_FIRE)) add_flag(flgs, TR_SH_FIRE);
		if (have_flag(obj_flgs, TR_SH_ELEC)) add_flag(flgs, TR_SH_ELEC);
		if (have_flag(obj_flgs, TR_SH_COLD)) add_flag(flgs, TR_SH_COLD);

		//強力射撃、追加射撃、投擲、乗馬、祝福、テレポート、二刀流、反魔法、反テレポ、ブーメラン、呪い型フラグは非対象

	}


}






/*
 * Obtain the "flags" for the player as if he was an item
 */
/*:::＠の能力をフラグとしてまとめる　ステータス画面での一覧表示用*/
static void player_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;
	int plev = p_ptr->lev;

	//アリスと座敷わらしが使う
	u32b doll_flgs[TR_FLAG_MAX];
	object_type *o_ptr;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	/* Classes */
	switch (p_ptr->pclass)
	{
	case CLASS_WARRIOR:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_REGEN);
		break;

	case CLASS_PRIEST:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 39)
		{
			if(is_good_realm(p_ptr->realm1)) add_flag(flgs, TR_RES_HOLY);
			else  add_flag(flgs, TR_RES_NETHER);
		}
		break;

	case CLASS_HIGH_MAGE:
		if(p_ptr->lev > 19 && p_ptr->realm1 == TV_BOOK_FORESEE) add_flag(flgs, TR_WARNING);
		if(p_ptr->lev > 19 && (p_ptr->realm1 == TV_BOOK_TRANSFORM || p_ptr->realm1 == TV_BOOK_LIFE)) add_flag(flgs, TR_REGEN);

		if(p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_OCCULT) add_flag(flgs, TR_RES_INSANITY);
		break;



	case CLASS_ARCHER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_BLIND);
		break;

	case CLASS_SAMURAI:
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_PALADIN:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_MAID:
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 39)add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_ROGUE:
		if (p_ptr->lev > 29) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_THROW);
		break;
	case CLASS_RANGER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 29) add_flag(flgs, TR_ESP_ANIMAL);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_POIS);
		break;
		/*
	case CLASS_CHAOS_WARRIOR:
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 39)
			add_flag(flgs, TR_RES_FEAR);
		break;
		*/
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		if ((p_ptr->lev > 9) && !heavy_armor())
			add_flag(flgs, TR_SPEED);
		if ((p_ptr->lev>24) && !heavy_armor())
			add_flag(flgs, TR_FREE_ACT);
		break;
	case CLASS_NINJA:
		if (p_ptr->lev > 4) add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 14) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 34) add_flag(flgs, TR_THROW);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_DARK);
		break;
	case CLASS_MINDCRAFTER:
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_WIS);
		if (p_ptr->lev > 24) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		break;
	case CLASS_BARD:
		add_flag(flgs, TR_RES_SOUND);
		break;
		/*
	case CLASS_BERSERKER:
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_REFLECT);
		break;
		*/
	case CLASS_MIRROR_MASTER:
		if(p_ptr->lev > 39)add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_TSUKUMO_MASTER:
		if(p_ptr->lev > 9)add_flag(flgs, TR_SEE_INVIS);
		break;

	case CLASS_SYUGEN:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 44 && is_good_realm(p_ptr->realm1)) add_flag(flgs, TR_RES_HOLY);
		break;

	case CLASS_MAGIC_KNIGHT:
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_REGEN);
		break;

	case CLASS_RESEARCHER:
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_BLIND);

		break;

	case CLASS_ENGINEER:
	case CLASS_NITORI:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_JEWELER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_BLIND);
		break;


	case CLASS_RUMIA:
		add_flag(flgs, TR_RES_DARK);
		break;
	case CLASS_YOUMU:
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_TEWI:
		if (p_ptr->lev >  9) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_UDONGE:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev >  9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		break;
	case CLASS_IKU:
		add_flag(flgs, TR_RES_ELEC);
		break;

	case CLASS_KOMACHI:
		if (p_ptr->lev > 19) 	add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 29) 	add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 39) 	add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_KOGASA:
		add_flag(flgs, TR_RES_WATER);
		break;


	case CLASS_KASEN:
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_SEE_INVIS);
		}
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 19)add_flag(flgs, TR_SUST_STR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SUIKA:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 19) 	add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 39) 	add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_KOISHI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_FREE_ACT);
		if(p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		break;
	case CLASS_MOMIZI:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_SEIGA:
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_CIRNO:
		add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		//v1.1.32
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_FEAR);
		}

		break;
	case CLASS_ORIN:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev >  9) add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SHINMYOUMARU:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_NAZRIN:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_LETTY:
		add_flag(flgs, TR_RES_COLD);
		break;
	case CLASS_PATCHOULI:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
		break;
	case CLASS_AYA:
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 9)  add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_BANKI:
		 add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_MYSTIA:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_FLAN:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SHOU:
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_HOLY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_NETHER);
		break;

	case CLASS_YUYUKO:
		add_flag(flgs, TR_ESP_UNDEAD);
		break;
	case CLASS_SATORI:
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 24) add_flag(flgs, TR_TELEPATHY);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KYOUKO:
		add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 39) add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_TOZIKO:
		add_flag(flgs, TR_RES_ELEC);
		break;
	case CLASS_LILY_WHITE:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KISUME:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_HATATE:
		 add_flag(flgs, TR_SEE_INVIS);
		break;
	case CLASS_MIKO:
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 4) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 9) add_flag(flgs, TR_ESP_HUMAN);
		if (p_ptr->lev > 14) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_KOKORO:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_WRIGGLE:
		add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_YUUKA:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_CHEN:
		if(!p_ptr->magic_num1[0])
		{
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
		}
		break;
	case CLASS_MURASA:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 19) add_flag(flgs, TR_FREE_ACT);
		if(p_ptr->lev > 29) add_flag(flgs, TR_RES_POIS);
		if(p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KEINE:
		if(p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if(p_ptr->lev > 34) add_flag(flgs, TR_SUST_INT);
		if(p_ptr->lev > 34) add_flag(flgs, TR_SUST_WIS);
		break;
	case CLASS_YUGI:
		if(p_ptr->lev > 9)   add_flag(flgs, TR_RES_FIRE);
		if(p_ptr->lev > 14)  add_flag(flgs, TR_RES_COLD);
		if(p_ptr->lev > 19)  add_flag(flgs, TR_SUST_STR);
		if(p_ptr->lev > 29)  add_flag(flgs, TR_RES_LITE);

		break;
	case CLASS_REIMU:
		{
			int rank = osaisen_rank();
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_HOLY);
			if(rank > 2) add_flag(flgs, TR_SEE_INVIS);
			if(rank > 7) add_flag(flgs, TR_DEC_MANA);
			if(rank > 8)
			{
				add_flag(flgs, TR_RES_ACID);
				add_flag(flgs, TR_RES_FIRE);
				add_flag(flgs, TR_RES_COLD);
				add_flag(flgs, TR_RES_ELEC);
				add_flag(flgs, TR_RES_POIS);
 			}
			if(plev > 4 && rank > 3) add_flag(flgs, TR_SLOW_DIGEST);
			if(plev > 4 && rank > 6) add_flag(flgs, TR_REGEN);
			if(plev > 9 && rank > 4) add_flag(flgs, TR_FREE_ACT);
			if(plev > 14 && rank > 3) add_flag(flgs, TR_RES_FEAR);
			if(plev > 19) add_flag(flgs, TR_ESP_KWAI);
			if(plev > 19 && rank > 4) add_flag(flgs, TR_RES_CONF);
			if(plev > 24 && rank > 1) add_flag(flgs, TR_ESP_UNDEAD);
			if(plev > 24 && rank > 5) add_flag(flgs, TR_RES_NETHER);
			if(plev > 29 && rank > 2) add_flag(flgs, TR_ESP_DEMON);
			if(plev > 34 && rank > 5) add_flag(flgs, TR_ESP_EVIL);
			if(plev > 34 && rank > 4) add_flag(flgs, TR_RES_LITE);
			if(plev > 39 && rank > 8) add_flag(flgs, TR_RES_TIME);
			if(plev > 39 && rank > 6) add_flag(flgs, TR_RES_CHAOS);
			if(plev > 44 && rank > 7) add_flag(flgs, TR_RES_INSANITY);
		}
		break;
	case CLASS_KAGEROU:
		if(p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_SANAE:
		if(p_ptr->lev > 9) add_flag(flgs, TR_ESP_DEITY);
		if(p_ptr->lev > 19) add_flag(flgs, TR_LEVITATION);
		if(p_ptr->lev > 39) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_REMY:
		add_flag(flgs, TR_LEVITATION);
		if(p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_BYAKUREN:
		add_flag(flgs, TR_RES_HOLY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if(p_ptr->tim_general[0]) //白蓮強化時
		{
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_LITE);
			if(p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
			if(p_ptr->lev > 4) add_flag(flgs, TR_RES_COLD);
			if(p_ptr->lev > 9) add_flag(flgs, TR_RES_ELEC);
			if(p_ptr->lev > 9) add_flag(flgs, TR_RES_ACID);
			if(p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_STR);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_CON);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
			if(p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
			if(p_ptr->lev > 34) add_flag(flgs, TR_THROW);
			if(p_ptr->lev > 34) add_flag(flgs, TR_SPEEDSTER);
			if(p_ptr->lev > 39) add_flag(flgs, TR_RES_CHAOS);
			if(p_ptr->lev > 39) add_flag(flgs, TR_RES_DISEN);
			if(p_ptr->lev > 44) add_flag(flgs, TR_IM_FIRE);
		}
		break;
	case CLASS_SEIJA:
		if(plev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_UTSUHO:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_LITE);
			if(plev > 19) add_flag(flgs, TR_SH_FIRE);
			if(plev > 24) add_flag(flgs, TR_RES_BLIND);
			if(plev > 29) add_flag(flgs, TR_RES_DARK);
			if(plev > 34) add_flag(flgs, TR_IM_FIRE);
		break;

	case CLASS_YAMAME:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_TUNNEL);

		if(plev > 9) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 39) add_flag(flgs, TR_RES_DARK);
		break;

	case CLASS_MARTIAL_ARTIST:
		if(!heavy_armor() && plev > 9)
			add_flag(flgs, TR_SPEED);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 29) add_flag(flgs, TR_SUST_STR);
		if(plev > 39) add_flag(flgs, TR_LEVITATION);
		break;

	case CLASS_SUWAKO:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_SEE_INVIS);
		if(plev > 9) add_flag(flgs, TR_RES_NETHER);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 29) add_flag(flgs, TR_RES_DARK);
		if(plev > 39) add_flag(flgs, TR_RES_CHAOS);
		break;
	case CLASS_RAN:
		if(plev > 4) add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		break;
	case CLASS_EIKI:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_ESP_UNIQUE);
		if(plev > 29) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_MEIRIN:
		add_flag(flgs, TR_REGEN);
		if(plev > 4)add_flag(flgs, TR_RES_FIRE);
		if(plev > 14)add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24)add_flag(flgs, TR_FREE_ACT);
		if(plev > 34)add_flag(flgs, TR_RES_LITE);

		break;
	case CLASS_PARSEE:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_ACID);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_SHINMYOU_2:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_LEVITATION);
		if(prace_is_(RACE_KOBITO)) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 9) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_SUMIREKO:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_ICHIRIN:
		add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_MOKOU:
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev >  9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_KANAKO:
		{
			int rank = kanako_rank();
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SEE_INVIS);
			if (p_ptr->lev >  9) add_flag(flgs, TR_RES_ELEC);
			if (p_ptr->lev > 14) add_flag(flgs, TR_FREE_ACT);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
			if (rank > 2) add_flag(flgs, TR_RES_FEAR);
			if (rank > 4) add_flag(flgs, TR_REGEN);
			if (rank > 4) add_flag(flgs, TR_SLOW_DIGEST);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_HIZIRIN) add_flag(flgs, TR_RES_HOLY);;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MIMIMI) add_flag(flgs, TR_RES_LITE);;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_REIMU) add_flag(flgs, TR_RES_TIME);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_CTHULHU) add_flag(flgs, TR_RES_WATER);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_OBERON) add_flag(flgs, TR_RES_DISEN);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_AZAT) add_flag(flgs, TR_RES_INSANITY);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MORGOTH) add_flag(flgs, TR_RES_DARK);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_J) add_flag(flgs, TR_RES_CHAOS);

			break;
		}
	case CLASS_FUTO:
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 24) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 34) add_flag(flgs, TR_WARNING);
		break;
	case CLASS_SUNNY:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->magic_num1[0])add_flag(flgs, TR_LITE);//v1.1.59
		if (p_ptr->magic_num1[0] >= SUNNY_CHARGE_SUNLIGHT_3) add_flag(flgs, TR_SH_FIRE);
		break;
	case CLASS_LUNAR:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_STAR:
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		break;

	case CLASS_3_FAIRIES:
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_SOUND);
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
		}
		if (p_ptr->magic_num1[0])add_flag(flgs, TR_LITE);//v1.1.59

		break;

	case CLASS_ALICE:
		//アリスの人形が装備している盾
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;
			o_ptr = &inven_add[i];
			if(object_is_melee_weapon(o_ptr)) continue;
			object_flags(o_ptr, doll_flgs);

			if (have_flag(doll_flgs, TR_RES_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(doll_flgs, TR_RES_ELEC)) add_flag(flgs, TR_RES_ELEC);
			if (have_flag(doll_flgs, TR_RES_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(doll_flgs, TR_RES_COLD)) add_flag(flgs, TR_RES_COLD);
			if (have_flag(doll_flgs, TR_RES_POIS)) add_flag(flgs, TR_RES_POIS);
			if (have_flag(doll_flgs, TR_RES_LITE)) add_flag(flgs, TR_RES_LITE);
			if (have_flag(doll_flgs, TR_RES_DARK)) add_flag(flgs, TR_RES_DARK);
			if (have_flag(doll_flgs, TR_RES_SHARDS)) add_flag(flgs, TR_RES_SHARDS);
			if (have_flag(doll_flgs, TR_RES_WATER)) add_flag(flgs, TR_RES_WATER);
			if (have_flag(doll_flgs, TR_RES_HOLY)) add_flag(flgs, TR_RES_HOLY);
			if (have_flag(doll_flgs, TR_RES_SOUND)) add_flag(flgs, TR_RES_SOUND);
			if (have_flag(doll_flgs, TR_RES_NETHER)) add_flag(flgs, TR_RES_NETHER);
			if (have_flag(doll_flgs, TR_RES_CHAOS)) add_flag(flgs, TR_RES_CHAOS);
			if (have_flag(doll_flgs, TR_RES_DISEN)) add_flag(flgs, TR_RES_DISEN);
			if (have_flag(doll_flgs, TR_RES_TIME)) add_flag(flgs, TR_RES_TIME);

			if (have_flag(doll_flgs, TR_FREE_ACT)) add_flag(flgs, TR_FREE_ACT);
			if (have_flag(doll_flgs, TR_RES_BLIND)) add_flag(flgs, TR_RES_BLIND);
			if (have_flag(doll_flgs, TR_RES_FEAR)) add_flag(flgs, TR_RES_FEAR);
			if (have_flag(doll_flgs, TR_RES_CONF)) add_flag(flgs, TR_RES_CONF);
			if (have_flag(doll_flgs, TR_RES_INSANITY)) add_flag(flgs, TR_RES_INSANITY);

			if (have_flag(doll_flgs, TR_REFLECT)) add_flag(flgs, TR_REFLECT);
		}
		break;
	case CLASS_MARISA:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_POIS);
		if(plev > 6) add_flag(flgs, TR_SPEED);
		if(plev > 9) add_flag(flgs, TR_RES_FIRE);
		if(plev > 19) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 29) add_flag(flgs, TR_RES_LITE);
		break;

	case CLASS_LUNASA:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_WARNING);

		break;

	case CLASS_MERLIN:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_LYRICA:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_BLIND);
		break;

	case CLASS_CLOWNPIECE:
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_NETHER);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 8) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_DOREMY:
		add_flag(flgs, TR_FREE_ACT);
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_RES_INSANITY);
		if(IN_DREAM_WORLD) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_YATSUHASHI:
	case CLASS_BENBEN:
		if(plev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_HINA:
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		if(plev > 24) add_flag(flgs, TR_RES_POIS);
		if(p_ptr->magic_num1[0] > 14999) add_flag(flgs, TR_RES_INSANITY);
		if(p_ptr->magic_num1[0] > 19999) add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_SAKUYA:
		add_flag(flgs, TR_FREE_ACT);
		if(plev > 9) add_flag(flgs, TR_SLOW_DIGEST);
		if(plev > 14) add_flag(flgs, TR_RES_TIME);
		if(plev > 24) add_flag(flgs, TR_RES_FEAR);
		if(plev > 39) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 7) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_RAIKO:
		add_flag(flgs, TR_RES_ELEC);
		if(plev > 9) add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_LEVITATION);

		break;
	case CLASS_CHEMIST:
		if(plev > 14) add_flag(flgs, TR_FREE_ACT);
		if(plev > 29) add_flag(flgs, TR_RES_POIS);
		break;

	case CLASS_MAMIZOU:
		add_flag(flgs, TR_SEE_INVIS);
		if(plev > 14) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_YUKARI:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_REFLECT);
		break;
	case CLASS_RINGO:
		if(plev > 9) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 19) add_flag(flgs, TR_WARNING);
		if(plev > 29) add_flag(flgs, TR_RES_CONF);
		if(plev > 39) add_flag(flgs, TR_SUST_WIS);
		if(plev > 39) add_flag(flgs, TR_SUST_INT);
		break;

	case CLASS_SEIRAN:
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 44) add_flag(flgs, TR_RES_TIME);
		break;

	case CLASS_EIRIN:
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KAGUYA:
		add_flag(flgs, TR_SUST_CHR);
		if(plev >  9) add_flag(flgs, TR_LITE);
		if(plev > 19) add_flag(flgs, TR_RES_DISEN);
		if(plev > 29) add_flag(flgs, TR_RES_TIME);
		if(plev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_TOYOHIME:
		add_flag(flgs, TR_RES_WATER);
		if(plev > 19) add_flag(flgs, TR_RES_TIME);
		if(plev > 34) add_flag(flgs, TR_ESP_EVIL);
		break;

	case CLASS_YORIHIME:
		add_flag(flgs, TR_RES_WATER);
		if(plev > 9) add_flag(flgs, TR_RES_LITE);
		if(plev > 19) add_flag(flgs, TR_RES_DARK);
		if(plev > 29) add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_HECATIA:
		{
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_LITE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			if(plev > 19) add_flag(flgs, TR_RES_CONF);
			if(plev > 29) add_flag(flgs, TR_RES_INSANITY);
			if(plev > 39) add_flag(flgs, TR_EASY_SPELL);

			if(hecatia_body_is_(HECATE_BODY_OTHER))
			{
				if(plev > 24) add_flag(flgs, TR_RES_CHAOS);
				if(plev > 44) add_flag(flgs, TR_RES_TIME);
			}
			if(hecatia_body_is_(HECATE_BODY_MOON))
			{
				if(plev > 24) add_flag(flgs, TR_RES_DARK);
				if(plev > 34) add_flag(flgs, TR_RES_LITE);
			}
			if(hecatia_body_is_(HECATE_BODY_EARTH))
			{
				if(plev > 24) add_flag(flgs, TR_RES_ACID);
				if(plev > 24) add_flag(flgs, TR_RES_COLD);
				if(plev > 24) add_flag(flgs, TR_RES_ELEC);
				if(plev > 34) add_flag(flgs, TR_RES_POIS);
			}
		}
		break;
	case CLASS_JUNKO:
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_FREE_ACT);
		break;

	case CLASS_SOLDIER:
		if(plev > 19) add_flag(flgs, TR_RES_FIRE);
		if(plev > 29) add_flag(flgs, TR_RES_FEAR);
		if(plev > 39) add_flag(flgs, TR_WARNING);

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_HAWK_EYE))
			add_flag(flgs, TR_SEE_INVIS);
		break;

	case CLASS_NEMUNO:
		if(IS_NEMUNO_IN_SANCTUARY)
		{
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SPEED);

		}

		if(plev > 24) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_WATER);
		break;
	case CLASS_AUNN:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_ESP_DEITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_HOLY);
		if(plev > 14) add_flag(flgs, TR_ESP_KWAI);
		if(plev > 14) add_flag(flgs, TR_ESP_DEMON);
		if(plev > 19) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24) add_flag(flgs, TR_RES_POIS);
		if(plev > 29) add_flag(flgs, TR_ESP_EVIL);
		if(plev > 34) add_flag(flgs, TR_RES_FEAR);

		break;
	case CLASS_NARUMI:
		add_flag(flgs, TR_RES_HOLY);
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))	add_flag(flgs, TR_REGEN);
		break;

	case CLASS_LARVA:
		if (plev > 29) add_flag(flgs, TR_SUST_CON);
		if (plev > 39) add_flag(flgs, TR_RES_POIS);

		break;

	case CLASS_MAI:
	case CLASS_SATONO:
		if (plev > 9)	add_flag(flgs, TR_SUST_CHR);
		if (plev > 19)	add_flag(flgs, TR_RES_CONF);
		if (plev > 29)	add_flag(flgs, TR_RES_CHAOS);
		if (plev > 39)	add_flag(flgs, TR_RES_INSANITY);

		break;

	case CLASS_VFS_CLOWNPIECE:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_NETHER);
		if (plev > 19)	add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_JYOON:
		if (plev >  14) add_flag(flgs, TR_LITE);
		if (plev >  19) add_flag(flgs, TR_FREE_ACT);
		if (plev >  34) add_flag(flgs, TR_RES_LITE);
		break;

	case CLASS_SHION:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_LEVITATION);
		if (plev >  9) add_flag(flgs, TR_SEE_INVIS);
		if (plev >  19) add_flag(flgs, TR_RES_CONF);
		if (plev >  29) add_flag(flgs, TR_RES_CHAOS);
		break;

	case CLASS_OKINA:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		if (plev >  9) add_flag(flgs, TR_RES_CONF);
		if (plev >  19) add_flag(flgs, TR_RES_DARK);
		if (plev >  24) add_flag(flgs, TR_RES_ACID);
		if (plev >  24) add_flag(flgs, TR_RES_ELEC);
		if (plev >  24) add_flag(flgs, TR_RES_FIRE);
		if (plev >  24) add_flag(flgs, TR_RES_COLD);
		if (plev >  29) add_flag(flgs, TR_RES_CHAOS);
		if (plev >  39) add_flag(flgs, TR_RES_TIME);
		break;

	case CLASS_MAYUMI:
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_RES_FEAR);
		if (plev > 19) add_flag(flgs, TR_RES_CONF);
		if (plev > 29) add_flag(flgs, TR_RES_COLD);

		break;

	case CLASS_KUTAKA:
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_POIS);
		if (plev > 19)	add_flag(flgs, TR_RES_WATER);
		break;

	case CLASS_URUMI:
		add_flag(flgs, TR_RES_WATER);
		if (plev > 19)	add_flag(flgs, TR_RES_FEAR);
		break;

	case CLASS_SAKI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (plev>9) add_flag(flgs, TR_RES_ELEC);
		if (plev>19) add_flag(flgs, TR_ESP_UNDEAD);
		if (plev>19) add_flag(flgs, TR_SUST_STR);
		break;

	case CLASS_YACHIE:
		add_flag(flgs, TR_RES_WATER);
		if (plev  >14)add_flag(flgs, TR_RES_FIRE);
		if (plev  >19)add_flag(flgs, TR_RES_ELEC);
		if (plev  >24)add_flag(flgs, TR_RES_ACID);

		if (plev  >19)add_flag(flgs, TR_ESP_UNDEAD);
		if (plev  >29)add_flag(flgs, TR_RES_CONF);
		break;

	case CLASS_KEIKI:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_SUST_DEX);
		if (plev > 24)		add_flag(flgs, TR_ESP_NONLIVING);
		break;

	case CLASS_MIKE:
		if(plev > 29) add_flag(flgs, TR_RES_HOLY);

		break;
	case CLASS_TAKANE:

		if (plev > 24) add_flag(flgs, TR_RES_CONF);
		if (plev > 44) add_flag(flgs, TR_ESP_UNIQUE);
		break;

	case CLASS_SANNYO:

		if (plev > 19) add_flag(flgs, TR_RES_FEAR);
		if (plev > 29) add_flag(flgs, TR_RES_CONF);

		break;

	case CLASS_MOMOYO:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_TUNNEL);

		if(plev > 29)add_flag(flgs, TR_RES_NETHER);
		if (plev > 34)add_flag(flgs, TR_SUST_STR);
		if (plev > 34)add_flag(flgs, TR_SUST_DEX);
		if (plev > 34)add_flag(flgs, TR_SUST_CON);

		if (plev > 34)add_flag(flgs, TR_BRAND_POIS);

		if (plev > 23) add_flag(flgs, TR_BLOWS);

		break;
	case CLASS_TSUKASA:

		if (plev > 19) add_flag(flgs, TR_FREE_ACT);
		if (plev > 39) add_flag(flgs, TR_RES_FEAR);

		break;

	case CLASS_MEGUMU:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_FIRE);
		if (plev >  4) add_flag(flgs, TR_SPEED);
		if (plev >  9) add_flag(flgs, TR_SEE_INVIS);
		if (plev > 19) add_flag(flgs, TR_RES_CONF);
		if (plev > 29) add_flag(flgs, TR_RES_LITE);

		break;

	case CLASS_MISUMARU:
		if (plev > 9) add_flag(flgs, TR_ESP_HUMAN);
		if (plev >19) add_flag(flgs, TR_ESP_ANIMAL);
		if (plev >24) add_flag(flgs, TR_RES_HOLY);
		if (plev >29) add_flag(flgs, TR_ESP_KWAI);
		if (plev >39) add_flag(flgs, TR_ESP_EVIL);
		break;


	case CLASS_YUMA:
		{
			u32b obj_flgs[TR_FLAG_SIZE];
			u32b mode = (PLAYER_ADD_OBJ_FLG_MODE_RESIST| PLAYER_ADD_OBJ_FLG_MODE_SPEED| PLAYER_ADD_OBJ_FLG_MODE_WARNING| PLAYER_ADD_OBJ_FLG_MODE_OTHERS);

			for (i = 0; i < TR_FLAG_SIZE; i++)
				obj_flgs[i] = 0;

			//magic_num2[]に記録している装備品フラグ処理
			for (i = 0; i<TR_FLAG_MAX; i++)
			{
				if (p_ptr->magic_num2[i]) add_flag(obj_flgs, i);
			}
			player_add_object_flag(flgs, obj_flgs, mode);

			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_POIS);
		}
		break;

	case CLASS_CHIMATA:
		{
			int card_rank = (CHIMATA_CARD_RANK);

			add_flag(flgs, TR_SEE_INVIS);
			if (plev > 39) 	add_flag(flgs, TR_RES_CHAOS);

			if (card_rank >= 2)
			{
				add_flag(flgs, TR_RES_CONF);
			}
			if (card_rank >= 3)
			{
				add_flag(flgs, TR_FREE_ACT);
			}
			if (card_rank >= 4)
			{
				add_flag(flgs, TR_RES_BLIND);

			}
			if (card_rank >= 5)
			{
				add_flag(flgs, TR_RES_FEAR);

			}
			if (card_rank >= 6)
			{
				add_flag(flgs, TR_RES_INSANITY);

			}
			if (card_rank >= 7)
			{
				add_flag(flgs, TR_LITE);
				add_flag(flgs, TR_RES_LITE);
				add_flag(flgs, TR_RES_DARK);

			}
			if (card_rank >= 8)
			{
				add_flag(flgs, TR_TELEPATHY);
			}
			if (card_rank >= 9)
			{
				add_flag(flgs, TR_RES_TIME);
				add_flag(flgs, TR_SPEED);
			}

		}
		break;

	case CLASS_MIYOI:
		if (plev > 9) add_flag(flgs, TR_SLOW_DIGEST);
		if (plev > 29) add_flag(flgs, TR_RES_WATER);
		break;

	case CLASS_BITEN:
		if (plev > 24) add_flag(flgs, TR_FREE_ACT);
		if (plev > 39) add_flag(flgs, TR_RES_FEAR);
		break;

	case CLASS_ENOKO:

		add_flag(flgs, TR_TUNNEL);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_NETHER);
		break;

	case CLASS_CHIYARI:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_NETHER);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
		if (p_ptr->lev > 34) add_flag(flgs, TR_SH_FIRE);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_DISEN);
		break;

	case CLASS_HISAMI:
		add_flag(flgs, TR_SUST_CHR);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_TIME);
		break;

	default:
		break; /* Do nothing */
	}





	/* Races */
	if (p_ptr->mimic_form && p_ptr->mimic_form != MIMIC_GIGANTIC)
	{
		switch(p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_CHAOS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_DEMON_LORD:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_CHAOS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_DISEN);
			//add_flag(flgs, TR_RES_NEXUS);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_IM_FIRE);
			add_flag(flgs, TR_SH_FIRE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_TELEPATHY);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_VAMPIRE:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_NETHER);
			if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_BEAST:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			add_flag(flgs, TR_WARNING);
			break;
		case MIMIC_SLIME:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_INSANITY);
			add_flag(flgs, TR_RES_BLIND);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_RES_SHARDS);
			add_flag(flgs, TR_RES_SOUND);
			add_flag(flgs, TR_RES_WATER);
			break;
		case MIMIC_MARISA:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			break;
		case MIMIC_MIST:
		{
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_BLIND);
			add_flag(flgs, TR_RES_SHARDS);
			add_flag(flgs, TR_RES_SOUND);
			break;
		}
		case MIMIC_DRAGON:
		{
			int rank =p_ptr->mimic_dragon_rank;
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_SPEED);

			if(rank > 99) add_flag(flgs, TR_RES_POIS);
			if(rank > 99) add_flag(flgs, TR_LEVITATION);
			if(rank > 99) add_flag(flgs, TR_SEE_INVIS);

			if(rank > 102) add_flag(flgs, TR_RES_FEAR);
			if(rank > 104) add_flag(flgs, TR_REGEN);
			if(rank > 106) add_flag(flgs, TR_RES_CONF);

			if(rank > 110) add_flag(flgs, TR_RES_CHAOS);
			if(rank > 112) add_flag(flgs, TR_RES_WATER);
			if(rank > 114) add_flag(flgs, TR_RES_BLIND);

			if(rank > 116) add_flag(flgs, TR_SUST_STR);
			if(rank > 117) add_flag(flgs, TR_SUST_CON);
			if(rank > 118) add_flag(flgs, TR_SUST_DEX);
			if(rank > 119) add_flag(flgs, TR_RES_LITE);
			if(rank > 120) add_flag(flgs, TR_RES_DARK);

			if(rank > 124) add_flag(flgs, TR_TELEPATHY);
			if(rank > 126) add_flag(flgs, TR_RES_SHARDS);

			if(rank > 127) add_flag(flgs, TR_RES_SOUND);
			if(rank > 128) add_flag(flgs, TR_RES_NETHER);

			/*:::一時強化抜きではrankの限界は130まで*/
			if(rank > 134) add_flag(flgs, TR_RES_DISEN);
			if(rank > 134) add_flag(flgs, TR_SUST_INT);
			if(rank > 134) add_flag(flgs, TR_SUST_WIS);
			if(rank > 134) add_flag(flgs, TR_SUST_CHR);

			if(rank >139) add_flag(flgs, TR_RES_TIME);
			if(rank >144) add_flag(flgs, TR_RES_HOLY);
			if(rank >144) add_flag(flgs, TR_SPEEDSTER);

			if(rank > 149) add_flag(flgs, TR_RES_INSANITY);
			break;
		}
		case MIMIC_CAT:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			add_flag(flgs, TR_WARNING);
			break;
		case MIMIC_NUE:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_POIS);
			if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
			if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
			if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CHAOS);
			if (p_ptr->lev > 24) add_flag(flgs, TR_RES_NETHER);
			if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
			if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);
			if (p_ptr->lev > 39) add_flag(flgs, TR_REGEN);
			if (p_ptr->lev > 39) add_flag(flgs, TR_SLOW_DIGEST);
			if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
			break;


		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			{
				monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

				if(r_ptr->flags2 & RF2_REFLECTING) add_flag(flgs, TR_REFLECT);
				if(r_ptr->flags2 & RF2_REGENERATE) add_flag(flgs, TR_REGEN);
				if(r_ptr->flags2 & RF2_ELDRITCH_HORROR || r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) add_flag(flgs, TR_RES_INSANITY);
				if(r_ptr->flags2 & RF2_AURA_FIRE) add_flag(flgs, TR_LITE);

				if(r_ptr->flags3 & RF3_NO_FEAR) add_flag(flgs, TR_RES_FEAR);
				if(r_ptr->flags3 & RF3_NO_CONF) add_flag(flgs, TR_RES_CONF);

				if(r_ptr->flags7 & RF7_CAN_FLY) add_flag(flgs, TR_LEVITATION);

				if(r_ptr->flagsr & RFR_IM_ACID) add_flag(flgs, TR_RES_ACID);
				if(r_ptr->flagsr & RFR_IM_FIRE) add_flag(flgs, TR_RES_FIRE);
				if(r_ptr->flagsr & RFR_IM_COLD) add_flag(flgs, TR_RES_COLD);
				if(r_ptr->flagsr & RFR_IM_ELEC) add_flag(flgs, TR_RES_ELEC);
				if(r_ptr->flagsr & RFR_IM_POIS) add_flag(flgs, TR_RES_POIS);

				if(r_ptr->flagsr & RFR_RES_LITE) add_flag(flgs, TR_RES_LITE);
				if(r_ptr->flagsr & RFR_RES_DARK) add_flag(flgs, TR_RES_DARK);
				if(r_ptr->flagsr & RFR_RES_NETH) add_flag(flgs, TR_RES_NETHER);
				if(r_ptr->flagsr & RFR_RES_WATE) add_flag(flgs, TR_RES_WATER);
				if(r_ptr->flagsr & RFR_RES_SHAR) add_flag(flgs, TR_RES_SHARDS);
				if(r_ptr->flagsr & RFR_RES_SOUN) add_flag(flgs, TR_RES_SOUND);
				if(r_ptr->flagsr & RFR_RES_CHAO) add_flag(flgs, TR_RES_CHAOS);
				if(r_ptr->flagsr & RFR_RES_HOLY) add_flag(flgs, TR_RES_HOLY);
				if(r_ptr->flagsr & RFR_RES_DISE) add_flag(flgs, TR_RES_DISEN);
				if(r_ptr->flagsr & RFR_RES_TIME) add_flag(flgs, TR_RES_TIME);

				if(r_ptr->speed > 110) add_flag(flgs,TR_SPEED);
			}
			break;

		case MIMIC_KOSUZU_GHOST:
			{
				add_flag(flgs, TR_SEE_INVIS);
				add_flag(flgs, TR_RES_COLD);
				add_flag(flgs, TR_RES_NETHER);
				add_flag(flgs, TR_LEVITATION);
				add_flag(flgs, TR_RES_FEAR);
				add_flag(flgs, TR_ESP_HUMAN);
			}
			break;
		case MIMIC_KOSUZU_HYAKKI:
			{
				add_flag(flgs, TR_SEE_INVIS);
				add_flag(flgs, TR_RES_SHARDS);
				add_flag(flgs, TR_RES_DARK);
				add_flag(flgs, TR_RES_NETHER);
				add_flag(flgs, TR_RES_FEAR);
				add_flag(flgs, TR_RES_CONF);
				add_flag(flgs, TR_RES_BLIND);
				add_flag(flgs, TR_FREE_ACT);
				add_flag(flgs, TR_SPEED);
				if(plev > 20)
					add_flag(flgs, TR_BLOWS);
			}
			break;

		case MIMIC_GOD_OF_NEW_WORLD:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SLOW_DIGEST);
			add_flag(flgs, TR_RES_HOLY);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_LITE);
			add_flag(flgs, TR_RES_DARK);
			break;

			break;


/* GIGANTICは通常の種族に準じることにした
		case MIMIC_GIGANTIC:
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_REGEN);
		//if (p_ptr->lev > 9) add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_SUST_STR);
			add_flag(flgs, TR_SUST_CON);
			add_flag(flgs, TR_RES_INSANITY);
			break;
*/

		}
	}
	else
	{
	switch (p_ptr->prace)
	{
	case RACE_HALF_YOUKAI:
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
		break;

	case RACE_SENNIN:
		if(p_ptr->pclass != CLASS_KASEN) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19 && p_ptr->pclass != CLASS_KASEN)
		{
			if( p_ptr->realm1>TV_BOOK_CHAOS || p_ptr->realm1 < TV_BOOK_TRANSFORM )  add_flag(flgs, TR_RES_HOLY);
			else if ( p_ptr->realm1==TV_BOOK_CHAOS) add_flag(flgs, TR_RES_CHAOS);
			else if ( p_ptr->realm1==TV_BOOK_TRANSFORM) add_flag(flgs, TR_RES_POIS);
			else if ( p_ptr->realm1==TV_BOOK_NECROMANCY) add_flag(flgs, TR_RES_NETHER);
			else if ( p_ptr->realm1==TV_BOOK_DARKNESS) add_flag(flgs, TR_RES_DARK);
		}

		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_WIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_LEVITATION);
		break;
	case RACE_TENNIN:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 14)  add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19)  add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_WIS);
		break;

	case RACE_FAIRY:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_KAPPA:
		add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ACID);
		break;
	case RACE_MAGICIAN:
		if (p_ptr->lev > 9) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 15) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_INT);
		break;

	case RACE_KARASU_TENGU:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SPEEDSTER);
		break;
	case RACE_HAKUROU_TENGU:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_POIS);
		break;


	case RACE_ONI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 19)add_flag(flgs, TR_SUST_STR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case RACE_DEATH:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_NETHER);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 24) add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 24) add_flag(flgs, TR_ESP_HUMAN);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);

		break;
	case RACE_WARBEAST:
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 29) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 14 && p_ptr->pclass != CLASS_AUNN)
			add_flag(flgs, TR_SPEED);
		break;

	case RACE_TSUKUMO:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		break;
	case RACE_ANDROID:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39)  add_flag(flgs, TR_RES_INSANITY);
		break;

	case RACE_GYOKUTO:
		if(p_ptr->pclass != CLASS_UDONGE) add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_YOUKO:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		if((p_ptr->realm1 < TV_BOOK_TRANSFORM  || p_ptr->realm1 > TV_BOOK_CHAOS) && p_ptr->lev > 44)add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_YAMAWARO:
		add_flag(flgs, TR_RES_WATER);
		break;

	case RACE_BAKEDANUKI:
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		break;
	case RACE_NINGYO:
		add_flag(flgs, TR_RES_WATER);
		break;

	case RACE_HOFGOBLIN:
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_POIS);
		break;
	case RACE_NYUDOU:
		if (p_ptr->lev > 9) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SHARDS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);
		break;
	case RACE_IMP:
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_NETHER);
		break;

	case RACE_GOLEM:
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_RES_NETHER);
	break;




	case RACE_ZOMBIE:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 14) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_LEVITATION);
		break;

	case RACE_VAMPIRE:
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 34) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 10) add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);

		break;
	case RACE_SPECTRE:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 34)  add_flag(flgs, TR_TELEPATHY);
		break;
	case RACE_DAIYOUKAI:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_POIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);;
		if (p_ptr->lev > 39) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SLOW_DIGEST);
		break;
	case RACE_STRAYGOD:
		{
			apply_deity_ability_flags(p_ptr->race_multipur_val[3],flgs);
			apply_deity_ability_flags(p_ptr->race_multipur_val[4]/128,flgs);
			apply_deity_ability_flags(p_ptr->race_multipur_val[4]%128,flgs);
		}
		break;
	case RACE_DEMIGOD:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 24) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SLOW_DIGEST);
		break;

	case RACE_NINJA:
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		break;

	case RACE_ULTIMATE:
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		break;
	case RACE_LICH:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_TELEPATHY);
		break;

	case RACE_HOURAI:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SLOW_DIGEST);

		break;
	case RACE_LUNARIAN:
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_SUST_INT);
		if (p_ptr->lev > 9)	add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19)	add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29)	add_flag(flgs, TR_EASY_SPELL);
		if (p_ptr->lev > 39)	add_flag(flgs, TR_RES_TIME);
		break;
	case RACE_MAGIC_JIZO:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_HOLY);
		if(plev > 9) add_flag(flgs, TR_RES_FIRE);
		if(plev > 19) add_flag(flgs, TR_SUST_WIS);
		if(plev > 29) add_flag(flgs, TR_RES_CONF);
		if(plev > 39) add_flag(flgs, TR_RES_NETHER);
		break;

	case RACE_HALF_GHOST:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev >  9)add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		break;

	case RACE_HANIWA:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);

		break;
	case RACE_ANIMAL_GHOST:
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SPEED);
		break;

	case RACE_ZASHIKIWARASHI:
		{
			add_flag(flgs, TR_WARNING);
			if (p_ptr->lev > 29) add_flag(flgs, TR_ESP_HUMAN);

			//移動屋敷として装備している品のパラメータを＠の耐性能力として反映する
			//同じflgsなのでそのままコピーすればと思ったが一部使われないパラメータがあるのでやめとく
			//flgsはすでに使われてるのでアリスのときに使ったdoll_flgsを流用

			o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

			if (object_is_artifact(o_ptr))
			{
				object_flags(o_ptr, doll_flgs);

				if (have_flag(doll_flgs, TR_SPEED)) add_flag(flgs, TR_SPEED);

				if (have_flag(doll_flgs, TR_ESP_EVIL)) add_flag(flgs, TR_ESP_EVIL);
				if (have_flag(doll_flgs, TR_ESP_GOOD))  add_flag(flgs, TR_ESP_GOOD);
				if (have_flag(doll_flgs, TR_ESP_ANIMAL))  add_flag(flgs, TR_ESP_ANIMAL);
				if (have_flag(doll_flgs, TR_ESP_HUMAN))  add_flag(flgs, TR_ESP_HUMAN);
				if (have_flag(doll_flgs, TR_ESP_UNDEAD))  add_flag(flgs, TR_ESP_UNDEAD);
				if (have_flag(doll_flgs, TR_ESP_DRAGON))  add_flag(flgs, TR_ESP_DRAGON);
				if (have_flag(doll_flgs, TR_ESP_DEITY))  add_flag(flgs, TR_ESP_DEITY);
				if (have_flag(doll_flgs, TR_ESP_DEMON))  add_flag(flgs, TR_ESP_DEMON);
				if (have_flag(doll_flgs, TR_ESP_KWAI))  add_flag(flgs, TR_ESP_KWAI);
				if (have_flag(doll_flgs, TR_ESP_NONLIVING))  add_flag(flgs, TR_ESP_NONLIVING);
				if (have_flag(doll_flgs, TR_ESP_UNIQUE))  add_flag(flgs, TR_ESP_UNIQUE);

				if (have_flag(doll_flgs, TR_SUST_STR)) add_flag(flgs, TR_SUST_STR);
				if (have_flag(doll_flgs, TR_SUST_INT)) add_flag(flgs, TR_SUST_INT);
				if (have_flag(doll_flgs, TR_SUST_WIS)) add_flag(flgs, TR_SUST_WIS);
				if (have_flag(doll_flgs, TR_SUST_DEX)) add_flag(flgs, TR_SUST_DEX);
				if (have_flag(doll_flgs, TR_SUST_CON)) add_flag(flgs, TR_SUST_CON);
				if (have_flag(doll_flgs, TR_SUST_CHR)) add_flag(flgs, TR_SUST_CHR);

				if (have_flag(doll_flgs, TR_EASY_SPELL)) add_flag(flgs, TR_EASY_SPELL);
				if (have_flag(doll_flgs, TR_DEC_MANA)) add_flag(flgs, TR_DEC_MANA);
				if (have_flag(doll_flgs, TR_REFLECT)) add_flag(flgs, TR_REFLECT);

				if (have_flag(doll_flgs, TR_RES_ACID)) add_flag(flgs, TR_RES_ACID);
				if (have_flag(doll_flgs, TR_RES_ELEC)) add_flag(flgs, TR_RES_ELEC);
				if (have_flag(doll_flgs, TR_RES_FIRE)) add_flag(flgs, TR_RES_FIRE);
				if (have_flag(doll_flgs, TR_RES_COLD)) add_flag(flgs, TR_RES_COLD);
				if (have_flag(doll_flgs, TR_RES_POIS)) add_flag(flgs, TR_RES_POIS);

				if (have_flag(doll_flgs, TR_RES_LITE)) add_flag(flgs, TR_RES_LITE);
				if (have_flag(doll_flgs, TR_RES_DARK)) add_flag(flgs, TR_RES_DARK);
				if (have_flag(doll_flgs, TR_RES_SHARDS)) add_flag(flgs, TR_RES_SHARDS);
				if (have_flag(doll_flgs, TR_RES_WATER)) add_flag(flgs, TR_RES_WATER);
				if (have_flag(doll_flgs, TR_RES_HOLY)) add_flag(flgs, TR_RES_HOLY);
				if (have_flag(doll_flgs, TR_RES_SOUND)) add_flag(flgs, TR_RES_SOUND);
				if (have_flag(doll_flgs, TR_RES_NETHER)) add_flag(flgs, TR_RES_NETHER);
				if (have_flag(doll_flgs, TR_RES_CHAOS)) add_flag(flgs, TR_RES_CHAOS);
				if (have_flag(doll_flgs, TR_RES_DISEN)) add_flag(flgs, TR_RES_DISEN);
				if (have_flag(doll_flgs, TR_RES_TIME)) add_flag(flgs, TR_RES_TIME);

				if (have_flag(doll_flgs, TR_FREE_ACT)) add_flag(flgs, TR_FREE_ACT);
				if (have_flag(doll_flgs, TR_RES_BLIND)) add_flag(flgs, TR_RES_BLIND);
				if (have_flag(doll_flgs, TR_RES_FEAR)) add_flag(flgs, TR_RES_FEAR);
				if (have_flag(doll_flgs, TR_RES_CONF)) add_flag(flgs, TR_RES_CONF);
				if (have_flag(doll_flgs, TR_RES_INSANITY)) add_flag(flgs, TR_RES_INSANITY);

				if (have_flag(doll_flgs, TR_LEVITATION)) add_flag(flgs, TR_LEVITATION);
				if (have_flag(doll_flgs, TR_SEE_INVIS)) add_flag(flgs, TR_SEE_INVIS);
				if (have_flag(doll_flgs, TR_SLOW_DIGEST)) add_flag(flgs, TR_SLOW_DIGEST);
				if (have_flag(doll_flgs, TR_REGEN)) add_flag(flgs, TR_REGEN);
				if (have_flag(doll_flgs, TR_LITE)) add_flag(flgs, TR_LITE);
				if (have_flag(doll_flgs, TR_SPEEDSTER)) add_flag(flgs, TR_SPEEDSTER);
				if (have_flag(doll_flgs, TR_TELEPATHY)) add_flag(flgs, TR_TELEPATHY);

				if (have_flag(doll_flgs, TR_SH_FIRE)) add_flag(flgs, TR_SH_FIRE);
				if (have_flag(doll_flgs, TR_SH_ELEC)) add_flag(flgs, TR_SH_ELEC);
				if (have_flag(doll_flgs, TR_SH_COLD)) add_flag(flgs, TR_SH_COLD);

			}

			else if (o_ptr->tval == TV_SOUVENIR)
			{
				switch (o_ptr->sval)
				{
					//道寿の壺:火炎ブレス
					//茂林寺の茶釜:蒸気のブレス

				case SV_SOUVENIR_SHINMYOU_HOUSE: //針妙丸の家　元素耐性
					add_flag(flgs, TR_RES_ACID);
					add_flag(flgs, TR_RES_ELEC);
					add_flag(flgs, TR_RES_FIRE);
					add_flag(flgs, TR_RES_COLD);

					break;
				case SV_SOUVENIR_VAMPIRE_COFFIN: //吸血鬼の棺桶　暗黒地獄耐性
					add_flag(flgs, TR_RES_DARK);
					add_flag(flgs, TR_RES_NETHER);
					break;

					//ラフノールの鏡　反射、AC100
				case SV_SOUVENIR_MIRROR_OF_RUFFNOR:
					add_flag(flgs, TR_REFLECT);
					break;

					//鳥の巣風エクステ 毒耐性
				case SV_SOUVENIR_KUTAKA:
					add_flag(flgs, TR_RES_POIS);
					break;

					//スキーズブラズニル　肉体能力上昇・破邪耐性
				case SV_SOUVENIR_SKIDBLADNIR:
					add_flag(flgs, TR_RES_HOLY);
					add_flag(flgs, TR_RES_WATER);
					break;

					//古書型コンピュータ　知的能力上昇
				case SV_SOUVENIR_MOON_COMPUTER:
					add_flag(flgs, TR_SUST_INT);
					add_flag(flgs, TR_SUST_WIS);
					add_flag(flgs, TR_RES_CONF);
					add_flag(flgs, TR_EASY_SPELL);
					break;

					//貝殻の戦車　加速、壁抜け
				case SV_SOUVENIR_NODENS_CHARIOT:
					add_flag(flgs, TR_SPEED);
					break;

					//古のものの水晶　冷気オーラ,MP+100
				case SV_SOUVENIR_ELDER_THINGS_CRYSTAL:
					add_flag(flgs, TR_SH_COLD);
					break;

					//酔魔の酒の空き瓶　能力なし　レイシャルオーク召喚



				default:
					break;
				}
			}
		}

		break;

	//case RACE_ELF:
	//	add_flag(flgs, TR_RES_LITE);
	//	break;
	//case RACE_HOBBIT:
		//add_flag(flgs, TR_HOLD_LIFE);
		//break;
	//case RACE_GNOME:
		//add_flag(flgs, TR_FREE_ACT);
		//break;

	//case RACE_DWARF:
		//add_flag(flgs, TR_RES_BLIND);
		//break;
	//case RACE_HALF_ORC:
		//add_flag(flgs, TR_RES_DARK);
		//break;
	//case RACE_HALF_TROLL:
		//add_flag(flgs, TR_SUST_STR);
		//if (p_ptr->lev > 14)
		//{
		//	add_flag(flgs, TR_REGEN);
			//if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_BERSERKER))
			//{
				//add_flag(flgs, TR_SLOW_DIGEST);
				/*
				 * Let's not make Regeneration a disadvantage
				 * for the poor warriors who can never learn
				 * a spell that satisfies hunger (actually
				 * neither can rogues, but half-trolls are not
				 * supposed to play rogues)
				 */
		//	}
		//}
		//break;
	//case RACE_AMBERITE:
	//	add_flag(flgs, TR_SUST_CON);
	//	add_flag(flgs, TR_REGEN); /* Amberites heal fast */
	//	break;
	//case RACE_HIGH_ELF:
	//	add_flag(flgs, TR_RES_LITE);
	//	add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_BARBARIAN:
	//	add_flag(flgs, TR_RES_FEAR);
	//	break;
	//case RACE_HALF_OGRE:
		//add_flag(flgs, TR_SUST_STR);
	//	add_flag(flgs, TR_RES_DARK);
	//	break;
	//case RACE_HALF_GIANT:
	//	add_flag(flgs, TR_RES_SHARDS);
	//	add_flag(flgs, TR_SUST_STR);
	//	break;
	//case RACE_HALF_TITAN:
		//add_flag(flgs, TR_RES_CHAOS);
	//	break;
	//case RACE_CYCLOPS:
	//	add_flag(flgs, TR_RES_SOUND);
	//	break;
	//case RACE_YEEK:
	//	add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_IM_ACID);
	//	break;
	//case RACE_KLACKON:
	//	add_flag(flgs, TR_RES_CONF);
	//	add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SPEED);
	//	break;
	//case RACE_KOBOLD:
	//	add_flag(flgs, TR_RES_POIS);
	//	break;
	//case RACE_NIBELUNG:
	//	add_flag(flgs, TR_RES_DISEN);
	//	add_flag(flgs, TR_RES_DARK);
	//	break;
	//case RACE_DARK_ELF:
	//	add_flag(flgs, TR_RES_DARK);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_DRACONIAN:
	//	add_flag(flgs, TR_LEVITATION);
	//	if (p_ptr->lev > 4)
	//		add_flag(flgs, TR_RES_FIRE);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_RES_COLD);
	//	if (p_ptr->lev > 14)
	//		add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_RES_ELEC);
	//	if (p_ptr->lev > 34)
	//		add_flag(flgs, TR_RES_POIS);
	//	break;
	//case RACE_MIND_FLAYER:
	//	add_flag(flgs, TR_SUST_INT);
	//	add_flag(flgs, TR_SUST_WIS);
	//	if (p_ptr->lev > 14)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	if (p_ptr->lev > 29)
	//		add_flag(flgs, TR_TELEPATHY);
	//	break;
	//case RACE_IMP:
	//	add_flag(flgs, TR_RES_FIRE);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_GOLEM:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
		//if (p_ptr->lev > 34)
			//add_flag(flgs, TR_HOLD_LIFE);
	//	break;
	//case RACE_SKELETON:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_RES_SHARDS);
		//add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_POIS);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_ZOMBIE:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
	//	if (p_ptr->lev > 4)
	//		add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_VAMPIRE:
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_DARK);
	//	add_flag(flgs, TR_RES_NETHER);
	//	if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_SPECTRE:
	//	add_flag(flgs, TR_LEVITATION);
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_COLD);
	//	add_flag(flgs, TR_SEE_INVIS);
		//add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
		/* XXX pass_wall */
	//	if (p_ptr->lev > 34)
	//		add_flag(flgs, TR_TELEPATHY);
	//	break;
	//case RACE_SPRITE:
	//	add_flag(flgs, TR_RES_LITE);
	//	add_flag(flgs, TR_LEVITATION);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SPEED);
	//	break;
//	case RACE_BEASTMAN:
//		add_flag(flgs, TR_RES_SOUND);
//		add_flag(flgs, TR_RES_CONF);
//		break;
	case RACE_ANGEL:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SEE_INVIS);
		break;
//	case RACE_DEMON:
	//	add_flag(flgs, TR_RES_FIRE);
		//add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	if (p_ptr->lev > 9)
		//	add_flag(flgs, TR_SEE_INVIS);
		//break;
		/*
	case RACE_DUNADAN:
		add_flag(flgs, TR_SUST_CON);
		break;
	case RACE_S_FAIRY:
		add_flag(flgs, TR_LEVITATION);
		break;
	case RACE_KUTAR:
		add_flag(flgs, TR_RES_CONF);
		break;
		*/
	//case RACE_ANDROID:
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	break;
	default:
		; /* Do nothing */
	}
	}


	if( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);
	}

	/* Mutations */
	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
			add_flag(flgs, TR_SEE_INVIS);
		}
		///mod141218　追加
		if (p_ptr->muta3 & MUT3_BYDO)
		{
			add_flag(flgs, TR_SPEED);
		}

		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
			add_flag(flgs, TR_SPEEDSTER);
		}
		if (p_ptr->muta3 & MUT3_WARNING)
		{
			add_flag(flgs, TR_WARNING);
		}
		if (p_ptr->muta3 & MUT3_RES_WATER)
		{
			add_flag(flgs, TR_RES_WATER);
		}
		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
			add_flag(flgs, TR_RES_WATER);
		}

		if (p_ptr->muta3  & MUT3_ACID_SCALES)	add_flag(flgs, TR_RES_ACID);
		if (p_ptr->muta3  & MUT3_ELEC_SCALES)	add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->muta3  & MUT3_FIRE_SCALES)	add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->muta3  & MUT3_COLD_SCALES)	add_flag(flgs, TR_RES_COLD);
		if (p_ptr->muta3  & MUT3_POIS_SCALES)	add_flag(flgs, TR_RES_POIS);


		if (p_ptr->muta3 & MUT3_WINGS)
		{
			add_flag(flgs, TR_LEVITATION);
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			add_flag(flgs, TR_RES_FEAR);
		}


		if (p_ptr->muta3 & MUT3_ESP)
		{
			add_flag(flgs, TR_TELEPATHY);
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			add_flag(flgs, TR_FREE_ACT);
		}
	}

	if (p_ptr->muta4)
	{
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_RES_WATER);
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_RES_POIS);
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_FREE_ACT);

		if (p_ptr->muta4  & MUT4_GHOST_HANGOKUOH)	add_flag(flgs, TR_RES_NETHER);

	}

	if((prace_is_(RACE_VAMPIRE) || prace_is_(RACE_WARBEAST)	|| p_ptr->mimic_form == MIMIC_VAMPIRE)	)
	{
		if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000 && (turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30))
		{
			add_flag(flgs, TR_SPEED);
		}

	}

	if (p_ptr->muta3 & MUT3_BYDO) add_flag(flgs, TR_AGGRAVATE);

	if(CHECK_FAIRY_TYPE == 46 && p_ptr->image) add_flag(flgs, TR_SPEED);


	switch(CHECK_FAIRY_TYPE)
	{
	case 11:
		if(p_ptr->chp < p_ptr->mhp/2) add_flag(flgs, TR_SPEEDSTER);
		break;
	default:
		break;
	}

	if (p_ptr->pseikaku == SEIKAKU_SEXY) add_flag(flgs, TR_AGGRAVATE);

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
	//	add_flag(flgs, TR_HOLD_LIFE);
		if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 9)
			add_flag(flgs, TR_SPEED);
	}
	if (p_ptr->special_defense & KATA_FUUJIN)
		add_flag(flgs, TR_REFLECT);
	if (p_ptr->special_defense & KAMAE_GENBU)
		add_flag(flgs, TR_REFLECT);
	if (p_ptr->special_defense & KAMAE_SUZAKU)
		add_flag(flgs, TR_LEVITATION);
	if (p_ptr->special_defense & KAMAE_SEIRYU)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
	}
	if (p_ptr->special_defense & KATA_MUSOU || p_ptr->prace == RACE_ULTIMATE)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_LITE);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_SUST_CHR);
	}

	//v1.1.24 急流下りクエスト中は浮遊禁止
	if(QRKDR)
	{
		remove_flag(flgs, TR_LEVITATION);
	}

}

/*:::＠の一時ステータスによる能力、フラグなどをまとめる*/
///sys magic race 一時ステータスフラグをまとめる
static void tim_player_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	//v1.1.51 夢の世界と新アリーナでは強制的に浮遊を得ることにした。浮遊なしでは新アリーナのマップによっては詰むことがあるため。
	if (IN_DREAM_WORLD) add_flag(flgs, TR_LEVITATION);

	if (p_ptr->foresight)
		add_flag(flgs, TR_WARNING);

	if (IS_HERO() || p_ptr->shero)
		add_flag(flgs, TR_RES_FEAR);
	if (p_ptr->tim_invis)
		add_flag(flgs, TR_SEE_INVIS);
	if (p_ptr->tim_regen)
		add_flag(flgs, TR_REGEN);
	if (IS_TIM_ESP())
		add_flag(flgs, TR_TELEPATHY);
	if (IS_FAST() || p_ptr->slow)
		add_flag(flgs, TR_SPEED);
	/*:::イークの酸免疫があるときは酸の二重耐性を表記しない*/
	//if (IS_OPPOSE_ACID() && !(p_ptr->special_defense & DEFENSE_ACID) && !(prace_is_(RACE_YEEK) && (p_ptr->lev > 19)))
	if (IS_OPPOSE_ACID() && !(p_ptr->special_defense & DEFENSE_ACID) )
		add_flag(flgs, TR_RES_ACID);
	if (IS_OPPOSE_ELEC() && !(p_ptr->special_defense & DEFENSE_ELEC))
		add_flag(flgs, TR_RES_ELEC);
	if (IS_OPPOSE_FIRE() && !(p_ptr->special_defense & DEFENSE_FIRE))
		add_flag(flgs, TR_RES_FIRE);
	if (IS_OPPOSE_COLD() && !(p_ptr->special_defense & DEFENSE_COLD))
		add_flag(flgs, TR_RES_COLD);
	if (IS_OPPOSE_POIS())
		add_flag(flgs, TR_RES_POIS);

	if (p_ptr->special_attack & ATTACK_DARK)
		add_flag(flgs, TR_SLAY_GOOD);
	if (p_ptr->special_attack & ATTACK_DARK && p_ptr->lev > 44)
		add_flag(flgs, TR_VAMPIRIC);
	if (p_ptr->special_attack & ATTACK_FORCE)
		add_flag(flgs, TR_FORCE_WEAPON);

	if (p_ptr->special_attack & ATTACK_ACID)
		add_flag(flgs, TR_BRAND_ACID);
	if (p_ptr->special_attack & ATTACK_ELEC)
		add_flag(flgs, TR_BRAND_ELEC);
	if (p_ptr->special_attack & ATTACK_FIRE)
		add_flag(flgs, TR_BRAND_FIRE);
	if (p_ptr->special_attack & ATTACK_COLD)
		add_flag(flgs, TR_BRAND_COLD);
	if (p_ptr->special_attack & ATTACK_POIS)
		add_flag(flgs, TR_BRAND_POIS);
	if (p_ptr->special_defense & DEFENSE_ACID)
		add_flag(flgs, TR_IM_ACID);
	if (p_ptr->special_defense & DEFENSE_ELEC)
		add_flag(flgs, TR_IM_ELEC);
	if (p_ptr->special_defense & DEFENSE_FIRE)
		add_flag(flgs, TR_IM_FIRE);
	if (p_ptr->special_defense & DEFENSE_COLD)
		add_flag(flgs, TR_IM_COLD);
	if (p_ptr->wraith_form)
		add_flag(flgs, TR_REFLECT);
	/* by henkma */
	if (p_ptr->tim_reflect || (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO2)))
		add_flag(flgs, TR_REFLECT);

	if (check_activated_nameless_arts(JKF1_SELF_LIGHT))
		add_flag(flgs, TR_LITE);

	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0])
		add_flag(flgs, TR_RES_LITE);

	if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
		add_flag(flgs, TR_REFLECT);

	if (p_ptr->kamioroshi & KAMIOROSHI_SUMIYOSHI)
	{
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SPEEDSTER);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_WARNING);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
	{
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_DARK);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_BRAND_FIRE);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_IZUNA)
		add_flag(flgs, TR_SLAY_EVIL);

	if(p_ptr->nennbaku)
		add_flag(flgs, TR_NO_TELE);

	//v1.1.89 百々世ドラゴンイーター
	if(p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		add_flag(flgs, TR_SLAY_DRAGON);


	//慧音白沢化
	if(p_ptr->pclass == CLASS_KEINE && p_ptr->magic_num1[0] && !p_ptr->mimic_form)
	{
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_EASY_SPELL);
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_ESP_KWAI);
		add_flag(flgs, TR_ESP_DEMON);
		add_flag(flgs, TR_ESP_UNDEAD);
		add_flag(flgs, TR_ESP_ANIMAL);
		add_flag(flgs, TR_ESP_DEITY);
		add_flag(flgs, TR_ESP_DRAGON);
		add_flag(flgs, TR_ESP_UNIQUE);
		add_flag(flgs, TR_SPEED);
	}

	/*:::地形によるフラグ処理*/
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		if(p_ptr->prace == RACE_KAPPA && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs, TR_REGEN);
		}
		else if (p_ptr->prace == RACE_YAMAWARO && have_flag(f_ptr->flags, FF_TREE))
		{
			add_flag(flgs, TR_REGEN);
		}
		else if((p_ptr->prace == RACE_NINGYO) && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
			add_flag(flgs, TR_SPEED);
		}

		else if(p_ptr->pclass == CLASS_WAKASAGI && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_FREE_ACT);
		}
		else if(p_ptr->pclass == CLASS_MURASA && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_REGEN);
		}
		else if(CHECK_FAIRY_TYPE == 32 && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
		}
		else if (p_ptr->pclass == CLASS_YAMAME && have_flag(f_ptr->flags, FF_SPIDER_NEST_1))
		{
			if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		}


	}


/*
	if (p_ptr->magicdef)
	{
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_LEVITATION);
	}
*/
	if (p_ptr->tim_res_nether || p_ptr->tim_sh_death)
	{
		add_flag(flgs, TR_RES_NETHER);
	}
	if (p_ptr->tim_res_insanity)
	{
		add_flag(flgs, TR_RES_INSANITY);
	}
	if (p_ptr->tim_res_water)
	{
		add_flag(flgs, TR_RES_WATER);
	}
	if (p_ptr->tim_res_chaos)
	{
		add_flag(flgs, TR_RES_CHAOS);
	}
	if (p_ptr->tim_res_dark)
	{
		add_flag(flgs, TR_RES_DARK);
	}
	if (p_ptr->tim_speedster)
	{
		add_flag(flgs, TR_SPEEDSTER);
	}

	///mod141218 追加
	if (p_ptr->lightspeed)
	{
		add_flag(flgs, TR_SPEED);
	}
	///mod150712
	if (p_ptr->tim_levitation)
	{
		add_flag(flgs, TR_LEVITATION);
	}

	if (p_ptr->tim_sh_fire)
	{
		add_flag(flgs, TR_SH_FIRE);
	}
	if (p_ptr->ult_res || SUPER_SHION)
	{

		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);

		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_LITE);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_SUST_CHR);
	}

	/* Hex bonuses */
	/*
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling(HEX_DEMON_AURA))
		{
			add_flag(flgs, TR_SH_FIRE);
			add_flag(flgs, TR_REGEN);
		}
		if (hex_spelling(HEX_ICE_ARMOR)) add_flag(flgs, TR_SH_COLD);
		if (hex_spelling(HEX_SHOCK_CLOAK)) add_flag(flgs, TR_SH_ELEC);
	}
	*/

	//v1.1.24 急流下りクエスト中は浮遊とテレポ禁止
	if(QRKDR)
	{
		add_flag(flgs, TR_NO_TELE);
		remove_flag(flgs, TR_LEVITATION);
	}



}


/* Mode flags for displaying player flags */
#define DP_CURSE   0x01
#define DP_IMM     0x02
#define DP_WP      0x08


/*
 * Equippy chars
 */
/*:::所持金とパラメータの間の装備品シンボル一覧を表示　Cコマンドのステータス画面でも使われる*/
static void display_player_equippy(int y, int x, u16b mode)
{
	int i, max_i;

	byte a;
	char c;

	object_type *o_ptr;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else max_i = INVEN_TOTAL;

	/* Dump equippy chars */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		/* Object */
		o_ptr = &inventory[i];

		a = object_attr(o_ptr);
		c = object_char(o_ptr);

		/* Clear the part of the screen */
		if (!equippy_chars || !o_ptr->k_idx || check_invalidate_inventory(i))
		{
			c = ' ';
			a = TERM_DARK;
		}

		/* Dump */
		Term_putch(x + i - INVEN_RARM, y, a, c);
	}
}


void print_equippy(void)
{
	display_player_equippy(ROW_EQUIPPY, COL_EQUIPPY, 0);
}

/*
 *
 */
/*:::装備品による元素免疫？*/
static void known_obj_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	/* Check equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b o_flgs[TR_FLAG_SIZE];

		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;
		if(check_invalidate_inventory(i)) continue;

		/* Known flags */
		object_flags_known(o_ptr, o_flgs);

		if (have_flag(o_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(o_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		if (have_flag(o_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(o_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
	}

	//v1.1.46 女苑の指輪の処理
	if (p_ptr->pclass == CLASS_JYOON)
	{
		for (i = 0; i < INVEN_ADD_MAX; i++)
		{
			u32b o_flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			o_ptr = &inven_add[i];

			if (!o_ptr->k_idx) continue;
			if (check_invalidate_inventory(INVEN_RIGHT) || check_invalidate_inventory(INVEN_LEFT)) continue;

			object_flags_known(o_ptr, o_flgs);

			if (have_flag(o_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(o_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
			if (have_flag(o_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(o_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);

		}

	}


}

/*:::＠の免疫を表示　ステータス画面用*/
///race realm class 免疫を表示
static void player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (is_hurt_holy() == -100)	add_flag(flgs, TR_RES_HOLY);
	if(immune_insanity()) add_flag(flgs, TR_RES_INSANITY);
	//死者の神,海神,太陽神,雷神
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 61) add_flag(flgs, TR_RES_NETHER);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 44) add_flag(flgs, TR_RES_LITE);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 47) add_flag(flgs, TR_RES_WATER);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 55) add_flag(flgs, TR_RES_ELEC);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 74) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_WAKASAGI && p_ptr->lev > 19) add_flag(flgs, TR_RES_WATER);
	if(p_ptr->pclass == CLASS_MURASA) add_flag(flgs, TR_RES_WATER);

	if (prace_is_(RACE_SPECTRE))
		add_flag(flgs, TR_RES_NETHER);
	if (prace_is_(RACE_HANIWA))
		add_flag(flgs, TR_RES_NETHER);
	if (prace_is_(RACE_ANIMAL_GHOST))
		add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->pclass == CLASS_YOSHIKA)
		add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->mimic_form == MIMIC_VAMPIRE || prace_is_(RACE_VAMPIRE) || (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19) || p_ptr->tim_unite_darkness)
		add_flag(flgs, TR_RES_DARK);
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD)
		add_flag(flgs, TR_RES_FIRE);
	if (p_ptr->mimic_form == MIMIC_SLIME)
		add_flag(flgs, TR_RES_ACID);
	if (p_ptr->mimic_form == MIMIC_NUE)
		add_flag(flgs, TR_RES_DARK);

	if (p_ptr->pclass == CLASS_HECATIA)
		add_flag(flgs, TR_RES_NETHER);

	//v1.1.21 兵士
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		add_flag(flgs, TR_RES_SHARDS);



	if(p_ptr->prace == RACE_MAGICIAN) add_flag(flgs, TR_SLOW_DIGEST);

	//アリスの人形が装備している盾
	if(p_ptr->pclass == CLASS_ALICE)
	{
		u32b doll_flgs[TR_FLAG_MAX];
		object_type *o_ptr;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;
			o_ptr = &inven_add[i];
			if(object_is_melee_weapon(o_ptr)) continue;

			object_flags(o_ptr, doll_flgs);

			if (have_flag(doll_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(doll_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(doll_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
			if (have_flag(doll_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		}
	}

	//v1.1.79 種族座敷わらしが移動屋敷として装備しているアイテム
	if (prace_is_(RACE_ZASHIKIWARASHI))
	{
		u32b doll_flgs[TR_FLAG_MAX];
		object_type *o_ptr;
		o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		object_flags(o_ptr, doll_flgs);

		if (have_flag(doll_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(doll_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(doll_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
		if (have_flag(doll_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);



	}

	//尤魔が食べた免疫装備
	if (p_ptr->pclass == CLASS_YUMA)
	{
		u32b obj_flgs[TR_FLAG_SIZE];
		u32b mode = (PLAYER_ADD_OBJ_FLG_MODE_IMMUNE);
		//magic_num2[]に記録している装備品フラグ処理
		for (i = 0; i<TR_FLAG_MAX; i++)
		{
			if (p_ptr->magic_num2[i]) add_flag(obj_flgs, i);
		}
		player_add_object_flag(flgs, obj_flgs, mode);

	}

	if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->lev > 44 && p_ptr->tim_general[0])
	{
		add_flag(flgs, TR_RES_FIRE);
	}
	if (p_ptr->mimic_form == MIMIC_DRAGON)
	{
			int rank =p_ptr->mimic_dragon_rank;

			if(rank > 129) add_flag(flgs, TR_RES_FIRE);
			if(rank > 144) add_flag(flgs, TR_RES_ELEC);
			if(rank > 159) add_flag(flgs, TR_RES_ACID);
			if(rank > 159) add_flag(flgs, TR_RES_COLD);
			if(rank > 169) add_flag(flgs, TR_TELEPATHY);
	}
	if(p_ptr->pclass == CLASS_MOMIZI && p_ptr->lev > 34) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_STAR && p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
	if(p_ptr->pclass == CLASS_3_FAIRIES && p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
	if(p_ptr->pclass == CLASS_REIMU && p_ptr->lev > 44 && osaisen_rank() > 8) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_KANAKO && kanako_rank() > 6) add_flag(flgs, TR_RES_ELEC);

	if(p_ptr->pclass == CLASS_CIRNO && p_ptr->lev > 24) add_flag(flgs, TR_RES_COLD);
	if(p_ptr->pclass == CLASS_LETTY && p_ptr->lev > 39) add_flag(flgs, TR_RES_COLD);
	if(p_ptr->pclass == CLASS_MEDICINE) add_flag(flgs, TR_RES_POIS);
	if(p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);

	if(p_ptr->pclass == CLASS_UTSUHO)
	{
		if(p_ptr->lev > 34)  add_flag(flgs, TR_RES_FIRE);
		if(p_ptr->lev > 44)  add_flag(flgs, TR_RES_LITE);

	}
	if(p_ptr->pclass == CLASS_HINA)
	{
		if(p_ptr->magic_num1[0] > 9999) add_flag(flgs, TR_RES_DARK);
		if(p_ptr->magic_num1[0] > 9999) add_flag(flgs, TR_RES_NETHER);
	}
	if(p_ptr->pclass == CLASS_EIRIN) add_flag(flgs, TR_RES_POIS);

	//v1.1.75 暗視能力を持つとき光源に*が表示されることにした

	if (p_ptr->pclass == CLASS_MYSTIA && p_ptr->lev > 39)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_NINJA)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_MOMIZI)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_NUE)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_OKINA)add_flag(flgs, TR_LITE);
	if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET, SS_M_NOCTO_VISION))add_flag(flgs, TR_LITE);


	//else if (prace_is_(RACE_YEEK) && p_ptr->lev > 19)
	//	add_flag(flgs, TR_RES_ACID);
}

/*:::＠の一時免疫*/
static void tim_player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (p_ptr->special_defense & DEFENSE_ACID)
		add_flag(flgs, TR_RES_ACID);
	if (p_ptr->special_defense & DEFENSE_ELEC)
		add_flag(flgs, TR_RES_ELEC);
	if (p_ptr->special_defense & DEFENSE_FIRE)
		add_flag(flgs, TR_RES_FIRE);
	if (p_ptr->special_defense & DEFENSE_COLD)
		add_flag(flgs, TR_RES_COLD);
	if (p_ptr->wraith_form)
		add_flag(flgs, TR_RES_DARK);
	if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
		add_flag(flgs, TR_RES_LITE);
	if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
		add_flag(flgs, TR_RES_FIRE);



	//v1.1.75 暗視能力を持つとき光源に*が表示されることにした

	if (p_ptr->tim_unite_darkness)add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_BEAST) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_SLIME) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_DRAGON) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_CAT) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_MONST) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_GIGANTIC) add_flag(flgs, TR_LITE);


}

/*:::＠の弱点属性*/
///racel class 弱点属性フラグの計算
static void player_vuln_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	///mod140105 弱点表記をこれで全てまとめられるはず
	if(is_hurt_holy() > 0) add_flag(flgs,TR_RES_HOLY);
	if(is_hurt_disen() > 0) add_flag(flgs,TR_RES_DISEN);
	if(is_hurt_lite() > 0) add_flag(flgs,TR_RES_LITE);
	if(is_hurt_water() > 0) add_flag(flgs,TR_RES_WATER);

	if(is_hurt_fire() > 0) add_flag(flgs,TR_RES_FIRE);
	if(is_hurt_elec() > 0) add_flag(flgs,TR_RES_ELEC);
	if(is_hurt_cold() > 0) add_flag(flgs,TR_RES_COLD);
	if(is_hurt_acid() > 0) add_flag(flgs,TR_RES_ACID);

	if(prace_is_(RACE_LUNARIAN))
	{
		add_flag(flgs,TR_RES_NETHER);
		add_flag(flgs,TR_RES_DARK);
	}

	if (prace_is_(RACE_HANIWA))
	{
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_SOUND);
	}

	if ((p_ptr->muta3 & MUT3_VULN_ELEM) || (p_ptr->special_defense & KATA_KOUKIJIN))
	{
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
	}
	//v1.1.24 急流下りクエスト中は浮遊禁止
	if(QRKDR)
	{
		add_flag(flgs, TR_LEVITATION);
	}

	if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
	{
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
	}

	if (p_ptr->pclass == CLASS_YUMA)
	{
		add_flag(flgs, TR_SLOW_DIGEST);
	}

	if(IS_VULN_FEAR)
		add_flag(flgs, TR_RES_FEAR);


	//if (prace_is_(RACE_ANDROID))
		//add_flag(flgs, TR_RES_ELEC);
	//if (prace_is_(RACE_ENT))
	//	add_flag(flgs, TR_RES_FIRE);
	//if (prace_is_(RACE_VAMPIRE) || prace_is_(RACE_S_FAIRY) ||
	//    (p_ptr->mimic_form == MIMIC_VAMPIRE))
	//	add_flag(flgs, TR_RES_LITE);
}


/*
 * A struct for storing misc. flags
 */
typedef struct {
	u32b player_flags[TR_FLAG_SIZE];
	u32b tim_player_flags[TR_FLAG_SIZE];
	u32b player_imm[TR_FLAG_SIZE];
	u32b tim_player_imm[TR_FLAG_SIZE];
	u32b player_vuln[TR_FLAG_SIZE];
	u32b known_obj_imm[TR_FLAG_SIZE];
} all_player_flags;





//拡張インベントリスロットの[0-3]を右手指輪、[4-7]を左手指輪としてフラグを一つにまとめる。
//女苑がdisplay_flag_aux()で指輪の耐性表示をするのに使う。
void	obj_flags_known_jyoon_ring(int slot, u32b flgs[TR_FLAG_SIZE])
{
	int i;

	if (p_ptr->pclass != CLASS_JYOON) { msg_print(_("ERROR:女苑以外でobj_flags_known_jyoon_ring()が呼ばれた",
                                                    "ERROR: obj_flags_known_jyoon_ring() called for non-Jo'on")); return; }
	if (slot != INVEN_RIGHT && slot != INVEN_LEFT) { msg_print(_("ERROR:obj_flags_known_jyoon_ring()が不正なスロットで呼ばれた",
                                                                "ERROR: obj_flags_known_jyoon_ring() called for incorrect slot")); return; }

	for (i = 0; i < TR_FLAG_SIZE; i++) flgs[i] = 0L;

	for (i = 0; i<4; i++)
	{
		int j;
		u32b tmp_flgs[TR_FLAG_SIZE];
		object_type *o_ptr = &inven_add[(slot - INVEN_RIGHT) * 4 + i];
		if (!o_ptr->k_idx)continue;

		object_flags_known(o_ptr, tmp_flgs);

		for (j = 0; j < TR_FLAG_SIZE; j++)	flgs[j] |= tmp_flgs[j];
	}

	//拡張インベントリに入れた指輪の呪いは発動しないのでフラグ削除(反感除く)する
	remove_flag(flgs, TR_TY_CURSE);
	remove_flag(flgs, TR_DRAIN_EXP);
	remove_flag(flgs, TR_ADD_L_CURSE);
	remove_flag(flgs, TR_ADD_H_CURSE);

}




/*
 * Helper function, see below
 */
static void display_flag_aux(int row, int col, cptr header,
				    int flag1, all_player_flags *f, u16b mode)
{
	int     i;
	bool    vuln = FALSE;
	int max_i;

	bool	empty = FALSE;
	bool	imm		= FALSE;

	if (have_flag(f->player_vuln, flag1) &&
	    !(have_flag(f->known_obj_imm, flag1) ||
	      have_flag(f->player_imm, flag1) ||
	      have_flag(f->tim_player_imm, flag1)))
		vuln = TRUE;

	if (!have_flag(f->player_flags, flag1) && !have_flag(f->tim_player_flags, flag1)
		&& !have_flag(f->player_imm, flag1) && !have_flag(f->tim_player_imm, flag1)
		&& !have_flag(f->known_obj_imm, flag1))
		empty = TRUE;

	if (have_flag(f->player_imm, flag1) || have_flag(f->tim_player_imm, flag1)
		|| have_flag(f->known_obj_imm, flag1))
		imm = TRUE;

	if (mode & DP_CURSE) empty = TRUE;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else max_i = INVEN_TOTAL;
	///mod141122 耐性が埋まってないと暗く表示するためのフラグ整理
	for (i = INVEN_RARM; i < max_i; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr;
		if(check_invalidate_inventory(i))continue;

		o_ptr = &inventory[i];
		//v1.1.46 女苑の指輪は特殊フラグ処理
		if (p_ptr->pclass == CLASS_JYOON && (i == INVEN_RIGHT || i == INVEN_LEFT))
		{
			obj_flags_known_jyoon_ring(i, flgs);
		}
		else
		{
			if (!o_ptr->k_idx) continue;
			object_flags_known(o_ptr, flgs);
		}

		if(flag1 == TR_LITE)
		{
			if(have_flag(flgs, TR_LITE)) empty = FALSE;
			else if(have_flag(flgs, TR_SH_FIRE)) empty = FALSE;
			else if(i == INVEN_LITE)
			{
				if(o_ptr->name2 == EGO_LITE_DARKNESS) continue;
				else if((o_ptr->sval == SV_LITE_TORCH || o_ptr->sval == SV_LITE_LANTERN) && !o_ptr->xtra4) continue;
				else empty = FALSE;
			}
		}

		if(flag1 == TR_SLAY_EVIL && have_flag(flgs,TR_KILL_EVIL)) empty = FALSE;
		else if(flag1 == TR_SLAY_GOOD && have_flag(flgs,TR_KILL_GOOD)) empty = FALSE;
		else if(flag1 == TR_SLAY_ANIMAL && have_flag(flgs,TR_KILL_ANIMAL)) empty = FALSE;
		else if(flag1 == TR_SLAY_HUMAN && have_flag(flgs,TR_KILL_HUMAN)) empty = FALSE;
		else if(flag1 == TR_SLAY_UNDEAD && have_flag(flgs,TR_KILL_UNDEAD)) empty = FALSE;
		else if(flag1 == TR_SLAY_DRAGON && have_flag(flgs,TR_KILL_DRAGON)) empty = FALSE;
		else if(flag1 == TR_SLAY_DEITY && have_flag(flgs,TR_KILL_DEITY)) empty = FALSE;
		else if(flag1 == TR_SLAY_DEMON && have_flag(flgs,TR_KILL_DEMON)) empty = FALSE;
		else if(flag1 == TR_SLAY_KWAI && have_flag(flgs,TR_KILL_KWAI)) empty = FALSE;
		else if(flag1 == TR_VORPAL && have_flag(flgs,TR_EX_VORPAL)) empty = FALSE;

		if (mode & DP_CURSE)
		{
			if(object_is_cursed(o_ptr))empty = FALSE;
		}
		else
		{
			if (have_flag(flgs, flag1)) empty = FALSE;
		}
	}

	/* Header */
	if (!(mode & DP_IMM))
	{
		if(imm) c_put_str(TERM_L_BLUE, header, row, col);
		else if(vuln && !empty) c_put_str(TERM_ORANGE, header, row, col);
		else if(vuln) c_put_str(TERM_RED, header, row, col);
		else if(empty) c_put_str(TERM_L_DARK, header, row, col);
		else c_put_str(TERM_WHITE, header, row, col);
	}
	/* Advance */
	col += strlen(header) + 1;



	/* Check equipment */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];
		//v1.1.46 女苑の指輪は特殊フラグ処理
		if (p_ptr->pclass == CLASS_JYOON && (i == INVEN_RIGHT || i == INVEN_LEFT))
		{
			obj_flags_known_jyoon_ring(i, flgs);
		}
		else
		{
			object_flags_known(o_ptr, flgs);
		}

		/*:::無効化されたスロット*/
		if(check_invalidate_inventory(i))
			c_put_str(TERM_DARK, ".", row, col);

		else if(flag1 == TR_LITE)
		{
			bool lite;
			if(!o_ptr->k_idx) lite = FALSE;
			else if(have_flag(flgs, TR_LITE)) lite = TRUE;
			else if(have_flag(flgs, TR_SH_FIRE)) lite = TRUE;
			else if(i == INVEN_LITE)
			{
				if(o_ptr->name2 == EGO_LITE_DARKNESS)lite = FALSE;
				else if((o_ptr->sval == SV_LITE_TORCH || o_ptr->sval == SV_LITE_LANTERN) && !o_ptr->xtra4)lite = FALSE;
				else lite = TRUE;
			}
			else lite = FALSE;

			if(lite) c_put_str((byte)TERM_WHITE,"+", row, col);
			else if(empty) c_put_str((byte)TERM_L_DARK, ".", row, col);
			else c_put_str((byte)TERM_SLATE, ".", row, col);
		}
		/* Default */
		else if (!(mode & DP_IMM))
		{
			if(imm) c_put_str(TERM_L_BLUE, ".", row, col);
			else if(empty) c_put_str((byte)(vuln ? TERM_RED : TERM_L_DARK), ".", row, col);
			else c_put_str((byte)(vuln ? TERM_ORANGE : TERM_SLATE), ".", row, col);
		}

		/* Check flags */
		if(check_invalidate_inventory(i)) /*何もしない*/;
		else if (mode & DP_CURSE)
		{
			if ((mode & DP_CURSE) && (have_flag(flgs, TR_ADD_L_CURSE) || have_flag(flgs, TR_ADD_H_CURSE)))
				c_put_str(TERM_L_DARK, "+", row, col);
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE)))
				c_put_str(TERM_WHITE, "+", row, col);
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & TRC_PERMA_CURSE))
				c_put_str(TERM_RED, "*", row, col);
		}
		else
		{

			if (have_flag(flgs, flag1))
			{
				if(imm || (mode & DP_IMM)) c_put_str(TERM_L_BLUE,(mode & DP_IMM) ? "*" : "+", row, col);
				else if(!empty && vuln) c_put_str(TERM_ORANGE,(mode & DP_IMM) ? "*" : "+", row, col);
				else if(empty) c_put_str((byte)(vuln ? TERM_L_RED : TERM_L_DARK),(mode & DP_IMM) ? "*" : "+", row, col);
				else c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE),(mode & DP_IMM) ? "*" : "+", row, col);
			}
		}

		/* Advance */
		col++;
	}

	/* Assume that player flag is already written */
	if (mode & DP_IMM) return;

	/* Default */
	if(imm) c_put_str(TERM_L_BLUE, ".", row, col);
	else if(!empty && vuln) c_put_str(TERM_ORANGE, ".", row, col);
	else if(empty) c_put_str((byte)(vuln ? TERM_RED : TERM_L_DARK), ".", row, col);
	else c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

	/* Player flags */
	if (have_flag(f->player_flags, flag1))
		c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE), "+", row, col);

	/* Timed player flags */
	if (have_flag(f->tim_player_flags, flag1)) c_put_str((byte)(vuln ? TERM_ORANGE : TERM_YELLOW), "#", row, col);

	/* Immunity */
	if (have_flag(f->tim_player_imm, flag1)) c_put_str(TERM_YELLOW, "*", row, col);
	if (have_flag(f->player_imm, flag1)) c_put_str(TERM_L_BLUE, "*", row, col);

	/* Vulnerability */
	if (vuln)
	{
		if(empty) c_put_str(TERM_RED, "v", row, col + 1);
		else c_put_str(TERM_ORANGE, "v", row, col + 1);
	}
}



/*
 * Special display, part 1
 */
/*:::現在の耐性を表示する*/
///sys race class magic 重要　ステータス画面に耐性を表示
///mod131228 TRフラグ入れ替えにより属性変更
static void display_player_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(f.player_flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 12;
	col = 1;

	display_player_equippy(row-2, col+8, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_flag_aux(row+0, col, "耐酸  :", TR_RES_ACID, &f, 0);
display_flag_aux(row+0, col, "耐酸  :", TR_IM_ACID, &f, DP_IMM);
display_flag_aux(row+1, col, "耐電撃:", TR_RES_ELEC, &f, 0);
display_flag_aux(row+1, col, "耐電撃:", TR_IM_ELEC, &f, DP_IMM);
display_flag_aux(row+2, col, "耐火炎:", TR_RES_FIRE, &f, 0);
display_flag_aux(row+2, col, "耐火炎:", TR_IM_FIRE, &f, DP_IMM);
display_flag_aux(row+3, col, "耐冷気:", TR_RES_COLD, &f, 0);
display_flag_aux(row+3, col, "耐冷気:", TR_IM_COLD, &f, DP_IMM);
display_flag_aux(row+4, col, "耐毒  :", TR_RES_POIS, &f, 0);
display_flag_aux(row+5, col, "耐閃光:", TR_RES_LITE, &f, 0);
display_flag_aux(row+6, col, "耐暗黒:", TR_RES_DARK, &f, 0);
display_flag_aux(row+7, col, "耐破片:", TR_RES_SHARDS, &f, 0);
display_flag_aux(row+8, col, "耐水  :", TR_RES_WATER, &f, 0);
display_flag_aux(row+9, col, "耐破邪:", TR_RES_HOLY, &f, 0);
#else
	display_flag_aux(row+0, col, "Acid  :", TR_RES_ACID, &f, 0);
	display_flag_aux(row+0, col, "Acid  :", TR_IM_ACID, &f, DP_IMM);
	display_flag_aux(row+1, col, "Elec  :", TR_RES_ELEC, &f, 0);
	display_flag_aux(row+1, col, "Elec  :", TR_IM_ELEC, &f, DP_IMM);
	display_flag_aux(row+2, col, "Fire  :", TR_RES_FIRE, &f, 0);
	display_flag_aux(row+2, col, "Fire  :", TR_IM_FIRE, &f, DP_IMM);
	display_flag_aux(row+3, col, "Cold  :", TR_RES_COLD, &f, 0);
	display_flag_aux(row+3, col, "Cold  :", TR_IM_COLD, &f, DP_IMM);
	display_flag_aux(row+4, col, "Poison:", TR_RES_POIS, &f, 0);
	display_flag_aux(row+5, col, "Light :", TR_RES_LITE, &f, 0);
	display_flag_aux(row+6, col, "Dark  :", TR_RES_DARK, &f, 0);
	display_flag_aux(row+7, col, "Shard :", TR_RES_SHARDS, &f, 0);
	display_flag_aux(row+8, col, "Water :", TR_RES_WATER, &f, 0);
	display_flag_aux(row+9, col, "Holy  :", TR_RES_HOLY, &f, 0);
#endif


	/*** Set 2 ***/

	row = 12;
	col = 26;

	display_player_equippy(row-2, col+8, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_flag_aux(row+0, col, "耐轟音:", TR_RES_SOUND, &f, 0);
display_flag_aux(row+1, col, "耐地獄:", TR_RES_NETHER, &f, 0);
display_flag_aux(row+2, col, "耐時空:", TR_RES_TIME, &f, 0);
display_flag_aux(row+3, col, "耐混沌:", TR_RES_CHAOS, &f, 0);
display_flag_aux(row+4, col, "耐劣化:", TR_RES_DISEN, &f, 0);
display_flag_aux(row+5, col, "耐恐怖:", TR_RES_FEAR, &f, 0);
display_flag_aux(row+6, col, "耐麻痺:", TR_FREE_ACT, &f, 0);
display_flag_aux(row+7, col, "耐混乱:", TR_RES_CONF, &f, 0);
display_flag_aux(row+8, col, "耐盲目:", TR_RES_BLIND, &f, 0);
display_flag_aux(row+9, col, "耐狂気:", TR_RES_INSANITY, &f, 0);
#else
	display_flag_aux(row+0, col, "Sound :", TR_RES_SOUND, &f, 0);
	display_flag_aux(row+1, col, "Nether:", TR_RES_NETHER, &f, 0);
	display_flag_aux(row+2, col, "SpTime:", TR_RES_TIME, &f, 0);
	display_flag_aux(row+3, col, "Chaos :", TR_RES_CHAOS, &f, 0);
	display_flag_aux(row+4, col, "Disnch:", TR_RES_DISEN, &f, 0);
	display_flag_aux(row+5, col, "Fear  :", TR_RES_FEAR, &f, 0);
	display_flag_aux(row+6, col, "FrAct :", TR_FREE_ACT, &f, 0);
	display_flag_aux(row+7, col, "Conf  :", TR_RES_CONF, &f, 0);
	display_flag_aux(row+8, col, "Blind :", TR_RES_BLIND, &f, 0);
	display_flag_aux(row+9, col, "Insane:", TR_RES_INSANITY, &f, 0);
#endif


	/*** Set 3 ***/

	row = 12;
	col = 51;

#ifdef JP
	display_player_equippy(row-2, col+10, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+10);
#else
    display_player_equippy(row-2, col+12, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+12);
#endif

#ifdef JP
display_flag_aux(row+0, col, "透明視認:", TR_SEE_INVIS, &f, 0);
display_flag_aux(row+1, col, "警告    :", TR_WARNING, &f, 0);
display_flag_aux(row+2, col, "栄養持続:", TR_SLOW_DIGEST, &f, 0);
display_flag_aux(row+3, col, "急速回復:", TR_REGEN, &f, 0);
display_flag_aux(row+4, col, "浮遊    :", TR_LEVITATION, &f, 0);
display_flag_aux(row+5, col, "高速移動:", TR_SPEEDSTER, &f, 0);
display_flag_aux(row+6, col, "反テレポ:", TR_NO_TELE, &f, 0);
display_flag_aux(row+7, col, "反魔法  :", TR_NO_MAGIC, &f, 0);
display_flag_aux(row+8, col, "反射    :", TR_REFLECT, &f, 0);
display_flag_aux(row+9, col, "呪い    :", 0, &f, DP_CURSE);
#else
	display_flag_aux(row+0, col, "SeeInvis  :", TR_SEE_INVIS, &f, 0);
	display_flag_aux(row+1, col, "Warning   :", TR_WARNING, &f, 0);
	display_flag_aux(row+2, col, "SlowDigest:", TR_SLOW_DIGEST, &f, 0);
	display_flag_aux(row+3, col, "Regen     :", TR_REGEN, &f, 0);
	display_flag_aux(row+4, col, "Levitation:", TR_LEVITATION, &f, 0);
	display_flag_aux(row+5, col, "Swiftness :", TR_SPEEDSTER, &f, 0);
	display_flag_aux(row+6, col, "NoTele    :", TR_NO_TELE, &f, 0);
	display_flag_aux(row+7, col, "NoMagic   :", TR_NO_MAGIC, &f, 0);
	display_flag_aux(row+8, col, "Reflection:", TR_REFLECT, &f, 0);
	display_flag_aux(row+9, col, "Cursed    :", 0, &f, DP_CURSE);
#endif

}


/*
 * Special display, part 2
 */
/*:::スレイやテレパスなどの状態を表示する*/
///item res sys スレイ・テレパスなどのパラメータ画面表示
///mod131228 TRフラグ入れ替えなど
static void display_player_other_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(f.player_flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 3;
	col = 1;

#ifdef JP
	display_player_equippy(row-2, col+12, DP_WP);

	c_put_str(TERM_WHITE, "ab@", row-1, col+12);
#else
    display_player_equippy(row-2, col+13, DP_WP);

	c_put_str(TERM_WHITE, "ab@", row-1, col+13);
#endif

#ifdef JP
	display_flag_aux(row+ 0, col, "混沌 倍打 :", TR_SLAY_EVIL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "混沌 倍打 :", TR_KILL_EVIL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "秩序 倍打 :", TR_SLAY_GOOD, &f, DP_WP);
	display_flag_aux(row+ 1, col, "秩序 倍打 :", TR_KILL_GOOD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 2, col, "人間 倍打 :", TR_SLAY_HUMAN, &f, DP_WP);
	display_flag_aux(row+ 2, col, "人間 倍打 :", TR_KILL_HUMAN, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 3, col, "動物 倍打 :", TR_SLAY_ANIMAL, &f, DP_WP);
	display_flag_aux(row+ 3, col, "動物 倍打 :", TR_KILL_ANIMAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 4, col, "不死 倍打 :", TR_SLAY_UNDEAD, &f, DP_WP);
	display_flag_aux(row+ 4, col, "不死 倍打 :", TR_KILL_UNDEAD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 5, col, "竜   倍打 :", TR_SLAY_DRAGON, &f, DP_WP);
	display_flag_aux(row+ 5, col, "竜   倍打 :", TR_KILL_DRAGON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 6, col, "悪魔 倍打 :", TR_SLAY_DEMON, &f, DP_WP);
	display_flag_aux(row+ 6, col, "悪魔 倍打 :", TR_KILL_DEMON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 7, col, "神格 倍打 :", TR_SLAY_DEITY, &f, DP_WP);
	display_flag_aux(row+ 7, col, "神格 倍打 :", TR_KILL_DEITY, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 8, col, "妖怪 倍打 :", TR_SLAY_KWAI, &f, DP_WP);
	display_flag_aux(row+ 8, col, "妖怪 倍打 :", TR_KILL_KWAI, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 9, col, "溶解      :", TR_BRAND_ACID, &f, DP_WP);
	display_flag_aux(row+10, col, "電撃      :", TR_BRAND_ELEC, &f, DP_WP);
	display_flag_aux(row+11, col, "焼棄      :", TR_BRAND_FIRE, &f, DP_WP);
	display_flag_aux(row+12, col, "凍結      :", TR_BRAND_COLD, &f, DP_WP);
	display_flag_aux(row+13, col, "毒殺      :", TR_BRAND_POIS, &f, DP_WP);
	display_flag_aux(row+14, col, "切れ味    :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+14, col, "切れ味    :", TR_EX_VORPAL, &f, DP_WP|DP_IMM);
	display_flag_aux(row+15, col, "地震      :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+16, col, "吸血      :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+17, col, "カオス効果:", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+18, col, "理力      :", TR_FORCE_WEAPON, &f, DP_WP);
#else
	display_flag_aux(row+ 0, col, "Slay Chaos :", TR_SLAY_EVIL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "Slay Chaos :", TR_KILL_EVIL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "Slay Lawful:", TR_SLAY_GOOD, &f, DP_WP);
	display_flag_aux(row+ 1, col, "Slay Lawful:", TR_KILL_GOOD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 2, col, "Slay Human :", TR_SLAY_HUMAN, &f, DP_WP);
	display_flag_aux(row+ 2, col, "Slay Human :", TR_KILL_HUMAN, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 3, col, "Slay Animal:", TR_SLAY_ANIMAL, &f, DP_WP);
	display_flag_aux(row+ 3, col, "Slay Animal:", TR_KILL_ANIMAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 4, col, "Slay Undead:", TR_SLAY_UNDEAD, &f, DP_WP);
	display_flag_aux(row+ 4, col, "Slay Undead:", TR_KILL_UNDEAD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 5, col, "Slay Dragon:", TR_SLAY_DRAGON, &f, DP_WP);
	display_flag_aux(row+ 5, col, "Slay Dragon:", TR_KILL_DRAGON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 6, col, "Slay Demon :", TR_SLAY_DEMON, &f, DP_WP);
	display_flag_aux(row+ 6, col, "Slay Demon :", TR_KILL_DEMON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 7, col, "Slay Deity :", TR_SLAY_DEITY, &f, DP_WP);
	display_flag_aux(row+ 7, col, "Slay Deity :", TR_KILL_DEITY, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 8, col, "Slay Youkai:", TR_SLAY_KWAI, &f, DP_WP);
	display_flag_aux(row+ 8, col, "Slay Youkai:", TR_KILL_KWAI, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 9, col, "Melting    :", TR_BRAND_ACID, &f, DP_WP);
	display_flag_aux(row+10, col, "Shocking   :", TR_BRAND_ELEC, &f, DP_WP);
	display_flag_aux(row+11, col, "Burning    :", TR_BRAND_FIRE, &f, DP_WP);
	display_flag_aux(row+12, col, "Freezing   :", TR_BRAND_COLD, &f, DP_WP);
	display_flag_aux(row+13, col, "Venomous   :", TR_BRAND_POIS, &f, DP_WP);
	display_flag_aux(row+14, col, "Sharpness  :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+14, col, "Sharpness  :", TR_EX_VORPAL, &f, DP_WP|DP_IMM);
	display_flag_aux(row+15, col, "Quake      :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+16, col, "Vampiric   :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+17, col, "Chaotic    :", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+18, col, "Force Wpn  :", TR_FORCE_WEAPON, &f, DP_WP);
#endif


	/*** Set 2 ***/

	row = 3;
	col = col + 12 + 7;

	display_player_equippy(row-2, col+13, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+13);

#ifdef JP
	display_flag_aux(row+ 0, col, "テレパシー :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 1, col, "ユニークESP:", TR_ESP_UNIQUE, &f, 0);
	display_flag_aux(row+ 2, col, "無生物ESP  :", TR_ESP_NONLIVING, &f, 0);
	display_flag_aux(row+ 3, col, "混沌ESP    :", TR_ESP_EVIL, &f, 0);
	display_flag_aux(row+ 4, col, "秩序ESP    :", TR_ESP_GOOD, &f, 0);
	display_flag_aux(row+ 5, col, "人間ESP    :", TR_ESP_HUMAN, &f, 0);
	display_flag_aux(row+ 6, col, "動物ESP    :", TR_ESP_ANIMAL, &f, 0);
	display_flag_aux(row+ 7, col, "不死ESP    :", TR_ESP_UNDEAD, &f, 0);
	display_flag_aux(row+ 8, col, "竜ESP      :", TR_ESP_DRAGON, &f, 0);
	display_flag_aux(row+ 9, col, "悪魔ESP    :", TR_ESP_DEMON, &f, 0);
	display_flag_aux(row+10, col, "神格ESP    :", TR_ESP_DEITY, &f, 0);
	display_flag_aux(row+11, col, "妖怪ESP    :", TR_ESP_KWAI, &f, 0);
	//display_flag_aux(row+12, col, "           :",  , &f, 0);
	display_flag_aux(row+13, col, "腕力維持   :", TR_SUST_STR, &f, 0);
	display_flag_aux(row+14, col, "知力維持   :", TR_SUST_INT, &f, 0);
	display_flag_aux(row+15, col, "賢さ維持   :", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+16, col, "器用維持   :", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+17, col, "耐久維持   :", TR_SUST_CON, &f, 0);
	display_flag_aux(row+18, col, "魅力維持   :", TR_SUST_CHR, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Telepathy  :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 1, col, "ESP Uniques:", TR_ESP_UNIQUE, &f, 0);
	display_flag_aux(row+ 2, col, "ESP Nonlvng:", TR_ESP_NONLIVING, &f, 0);
	display_flag_aux(row+ 3, col, "ESP Chaos  :", TR_ESP_EVIL, &f, 0);
	display_flag_aux(row+ 4, col, "ESP Lawful :", TR_ESP_GOOD, &f, 0);
	display_flag_aux(row+ 5, col, "ESP Human  :", TR_ESP_HUMAN, &f, 0);
	display_flag_aux(row+ 6, col, "ESP Animal :", TR_ESP_ANIMAL, &f, 0);
	display_flag_aux(row+ 7, col, "ESP Undead :", TR_ESP_UNDEAD, &f, 0);
	display_flag_aux(row+ 8, col, "ESP Dragon :", TR_ESP_DRAGON, &f, 0);
	display_flag_aux(row+ 9, col, "ESP Demon  :", TR_ESP_DEMON, &f, 0);
	display_flag_aux(row+10, col, "ESP Deity  :", TR_ESP_DEITY, &f, 0);
	display_flag_aux(row+11, col, "ESP Youkai :", TR_ESP_KWAI, &f, 0);
	//display_flag_aux(row+12, col, "           :",  , &f, 0);
	display_flag_aux(row+13, col, "Sustain STR:", TR_SUST_STR, &f, 0);
	display_flag_aux(row+14, col, "Sustain INT:", TR_SUST_INT, &f, 0);
	display_flag_aux(row+15, col, "Sustain WIS:", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+16, col, "Sustain DEX:", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+17, col, "Sustain CON:", TR_SUST_CON, &f, 0);
	display_flag_aux(row+18, col, "Sustain CHR:", TR_SUST_CHR, &f, 0);
#endif


	/*** Set 3 ***/

	row = 3;
	col = col + 12 + 17;

	display_player_equippy(row-2, col+14, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+14);

#ifdef JP
	display_flag_aux(row+ 0, col, "加速        :", TR_SPEED, &f, 0);
	display_flag_aux(row+ 1, col, "追加攻撃    :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 2, col, "赤外線視力  :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "魔法道具支配:", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "隠密        :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "探索        :", TR_SEARCH, &f, 0);
	display_flag_aux(row+ 6, col, "採掘        :", TR_TUNNEL, &f, 0);

	//v1.1.99 追加
	display_flag_aux(row + 7, col, "罠解除　　　:", TR_DISARM, &f, 0);
	display_flag_aux(row + 8, col, "魔法防御上昇:", TR_SAVING, &f, 0);

	display_flag_aux(row+ 9, col, "魔法難度減少:", TR_EASY_SPELL, &f, 0);
	display_flag_aux(row+10, col, "消費魔力減少:", TR_DEC_MANA, &f, 0);
	display_flag_aux(row+11, col, "祝福        :", TR_BLESSED, &f, 0);
	display_flag_aux(row+12, col, "光源        :", TR_LITE, &f, 0);
	///mod141219 光源は中で例外処理することにした
	//display_flag_aux(row+11, col, "光源        :", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+13, col, "乗馬        :", TR_RIDING, &f, 0);
	display_flag_aux(row+14, col, "投擲        :", TR_THROW, &f, 0);//要らんかも

	display_flag_aux(row+15, col, "経験値吸収  :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+16, col, "乱テレポート:", TR_TELEPORT, &f, 0);
	display_flag_aux(row+17, col, "反感        :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+18, col, "太古の怨念  :", TR_TY_CURSE, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Speed       :", TR_SPEED, &f, 0);
	display_flag_aux(row+ 1, col, "Blows       :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 2, col, "Infravision :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "Device Mstry:", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "Stealth     :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "Searching   :", TR_SEARCH, &f, 0);
	display_flag_aux(row+ 6, col, "Digging     :", TR_TUNNEL, &f, 0);

	display_flag_aux(row+ 7, col, "Disarming   :", TR_DISARM, &f, 0);
	display_flag_aux(row+ 8, col, "Magic Prot. :", TR_SAVING, &f, 0);

	display_flag_aux(row+ 9, col, "Easy Spells :", TR_EASY_SPELL, &f, 0);
	display_flag_aux(row+10, col, "Cheap Spells:", TR_DEC_MANA, &f, 0);
	display_flag_aux(row+11, col, "Blessed     :", TR_BLESSED, &f, 0);
	display_flag_aux(row+12, col, "Light       :", TR_LITE, &f, 0);
	///mod141219 光源は中で例外処理することにした
	//display_flag_aux(row+11, col, "光源        :", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+13, col, "Riding      :", TR_RIDING, &f, 0);
	display_flag_aux(row+14, col, "Throwing    :", TR_THROW, &f, 0);//要らんかも

	display_flag_aux(row+15, col, "Drain EXP   :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+16, col, "Teleport    :", TR_TELEPORT, &f, 0);
	display_flag_aux(row+17, col, "Aggravate   :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+18, col, "Foul Curse  :", TR_TY_CURSE, &f, 0);
#endif

}


/*
 * Special display, part 2a
 */
/*:::＠の情報表示画面3ページ目の一部*/
static void display_player_misc_info(void)
{
	char	buf[80];
	char	tmp[80];

	/* Display basics */
#ifdef JP
//put_str("名前  :", 1, 26);
put_str("名前:", 1, 16);
put_str("性別:", 3, 1);
put_str("種族:", 4, 1);
put_str("職業:", 5, 1);
#else
	put_str("Name  :", 1, 16);
	put_str("Sex   :", 3, 1);
	put_str("Race  :", 4, 1);
	put_str("Class :", 5, 1);
#endif
	if(p_ptr->psex == SEX_MALE)
		strcpy(tmp,ap_ptr->title);
	else
		strcpy(tmp,ap_ptr->f_title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"の");
#else
		strcat(tmp," ");
#endif
	strcat(tmp,player_name);

	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 7);
	///mod150103 種族名記述　野良神様専用処理追加
	if(prace_is_(RACE_STRAYGOD))
		c_put_str(TERM_L_BLUE, deity_table[p_ptr->race_multipur_val[3]].deity_name, 4, 7);
	else
		c_put_str(TERM_L_BLUE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), 4, 7);
	//c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 9);
	///mod140206 ＠職業名変更対応
	c_put_str(TERM_L_BLUE, player_class_name, 5, 7);

	/* Display extras */
#ifdef JP
put_str("ＬＶ:", 6, 1);
put_str("ＨＰ:", 7, 1);
put_str("ＭＰ:", 8, 1);
#else
	put_str("Level :", 6, 1);
	put_str("HP    :", 7, 1);
	put_str("MP    :", 8, 1);
#endif


	(void)sprintf(buf, "%d", (int)p_ptr->lev);
	c_put_str(TERM_L_BLUE, buf, 6, 7);

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
		(void)sprintf(buf, "%d", (int)p_ptr->mhp);
	else
		(void)sprintf(buf, "%d/%d", (int)p_ptr->chp, (int)p_ptr->mhp);

	c_put_str(TERM_L_BLUE, buf, 7, 7);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->csp, (int)p_ptr->msp);
	c_put_str(TERM_L_BLUE, buf, 8, 7);
}


/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
/*:::ステータス画面の＠のパラメータ*/
///sys ＠のパラメータ計算（ステータス画面用）

///mod140104 パラメータ上限と表記を変更
static void display_player_stat_info(void)
{
	int i, e_adj;
	int stat_col, stat;
	int row, col;

	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	byte a;
	char c;

	char buf[80];


	/* Column */
	stat_col = 22;

	/* Row */
	row = 3;

	/* Print out the labels for the columns */
#ifdef JP
c_put_str(TERM_WHITE, "能力", row, stat_col+1);
c_put_str(TERM_BLUE, "  基本", row, stat_col+7);
c_put_str(TERM_L_BLUE, " 種 職 性 装 ", row, stat_col+13);
c_put_str(TERM_L_GREEN, "合計", row, stat_col+28);
c_put_str(TERM_YELLOW, "現在", row, stat_col+35);
#else
	c_put_str(TERM_WHITE, "Stat", row, stat_col+1);
	c_put_str(TERM_BLUE, "  Base", row, stat_col+7);
	c_put_str(TERM_L_BLUE, "RacClaPerMod", row, stat_col+13);
	c_put_str(TERM_L_GREEN, "Actual", row, stat_col+26);
	c_put_str(TERM_YELLOW, "Current", row, stat_col+32);
#endif


	/* Display the stats */
	for (i = 0; i < 6; i++)
	{
		int r_adj,c_adj;
		int tmp;

		if (p_ptr->mimic_form) r_adj = mimic_info[p_ptr->mimic_form].r_adj[i];
		else r_adj = rp_ptr->r_adj[i];

		//職業補正値　霊夢に例外処理
		c_adj = (int)cp_ptr->c_adj[i];
		if(p_ptr->pclass == CLASS_REIMU) c_adj += osaisen_rank();
		else if(p_ptr->pclass == CLASS_KANAKO) c_adj += kanako_rank();
		else if(p_ptr->pclass == CLASS_DOREMY && IN_DREAM_WORLD) c_adj += 2;
		else if(p_ptr->pclass == CLASS_SUNNY)
		{
			if(is_daytime()) c_adj += 1;
			else c_adj -= 1;
		}
		else if(p_ptr->pclass == CLASS_LUNAR)
		{
			if(is_daytime()) c_adj -= 1;
			else c_adj += 1;
		}
		else if (p_ptr->pclass == CLASS_LARVA)
		{
			c_adj += p_ptr->lev / 15;
		}

		//v1.1.21 兵士パラメータアップ　獣人満月と累積するようにelseはつけない
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_MUSCLE))
		{
			if(i == A_STR) c_adj += 2;
			if(i == A_DEX) c_adj += 2;
			if(i == A_CON) c_adj += 2;
		}
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_MAGITEM))
		{
			if(i == A_INT) c_adj += 2;
			if(i == A_WIS) c_adj += 2;
		}

		/* Calculate equipment adjustment */
		e_adj = 0;
		///sys 18/***などの表記に関する部分
		/* Icky formula to deal with the 18 barrier */

		///mod140104 パラメータ上限と表記を変更
		e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];

		/*
		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - p_ptr->stat_max[i]) / 10;
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - 18) / 10 - p_ptr->stat_max[i] + 18;

		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - (p_ptr->stat_max[i] - 19) / 10 - 19;
		*/
		///race エントのパラメータ特殊処理
		if (prace_is_(RACE_ENT))
		{
			switch (i)
			{
				case A_STR:
				case A_CON:
					if (p_ptr->lev > 25) r_adj++;
					if (p_ptr->lev > 40) r_adj++;
					if (p_ptr->lev > 45) r_adj++;
					break;
				case A_DEX:
					if (p_ptr->lev > 25) r_adj--;
					if (p_ptr->lev > 40) r_adj--;
					if (p_ptr->lev > 45) r_adj--;
					break;
			}
		}
		if (prace_is_(RACE_STRAYGOD))
		{
			r_adj += deity_table[p_ptr->race_multipur_val[3]].stat_adj[i];
		}
		///mod140412 吸血鬼パラメータ処理
		if((prace_is_(RACE_VAMPIRE) || p_ptr->mimic_form == MIMIC_VAMPIRE)	)
		{
			if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30) r_adj += 3;
			}
			else
			{
				if(!p_ptr->resist_lite) r_adj -= 2;
				else r_adj -= 1;
			}
		}
		///mod140412 獣人パラメータ処理
		///mod150121 慧音は除く
		if(prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE)
		{
			if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30) r_adj += 2;
			}
		}
		//慧音白沢化中
		else if(p_ptr->pclass == CLASS_KEINE && !p_ptr->mimic_form && p_ptr->magic_num1[0])
		{
			if(i == A_STR) r_adj += 3;
			if(i == A_INT) r_adj += 5;
			if(i == A_WIS) r_adj += 4;
			if(i == A_DEX) r_adj += 3;
			if(i == A_CON) r_adj += 3;
			if(i == A_CHR) r_adj += 1;
		}


		e_adj -= r_adj;
		e_adj -= c_adj;
		e_adj -= ap_ptr->a_adj[i];

		if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			/* Reduced name of stat */
			c_put_str(TERM_WHITE, stat_names_reduced[i], row + i+1, stat_col+1);
		else
			c_put_str(TERM_WHITE, stat_names[i], row + i+1, stat_col+1);


		/* Internal "natural" max value.  Maxes at 18/100 */
		/* This is useful to see if you are maxed out */
		cnv_stat(p_ptr->stat_max[i], buf);

		if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
		{
#ifdef JP
			c_put_str(TERM_BLUE, "!", row + i+1, stat_col + 6);
		//	c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 6);
#else
			c_put_str(TERM_BLUE, "!", row + i+1, stat_col + 4);
#endif
		}

		c_put_str(TERM_BLUE, buf, row + i+1, stat_col + 13 - strlen(buf));

		/* Race, class, and equipment modifiers */
		(void)sprintf(buf, "%3d", r_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 13);
		//職業補正値
		(void)sprintf(buf, "%3d", c_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 16);
		//性格補正値
		(void)sprintf(buf, "%3d", (int)ap_ptr->a_adj[i]);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 19);

		(void)sprintf(buf, "%3d", (int)e_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 22);

		///mod140326 パラメータ限界超え表記処理のため変更
		/* Actual maximal modified value */
		//cnv_stat(p_ptr->stat_top[i], buf);
		tmp = p_ptr->stat_ind[i]+3 + (p_ptr->stat_top[i] - p_ptr->stat_use[i]);
		if(tmp == STAT_HYPERMAX)
		{
			(void)sprintf(buf, "   *%2d", tmp);
			c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 26);
		}
		else if(tmp > STAT_ADD_MAX)
		{
			(void)sprintf(buf, "   !%2d", tmp);
			c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 26);
		}
		else if(tmp == STAT_ADD_MAX)
		{
			(void)sprintf(buf, "   !%2d", tmp);
			c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);
		}
		else
		{
			(void)sprintf(buf, "    %2d", tmp);
			c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);
		}

		/* Only display stat_use if not maximal */
		if (p_ptr->stat_use[i] < p_ptr->stat_top[i])
		{
			(void)sprintf(buf, "    %2d", p_ptr->stat_ind[i]+3);
			c_put_str(TERM_YELLOW, buf, row + i+1, stat_col + 33);
		}
	}

	/* Column */
	col = stat_col + 41;

	/* Header and Footer */
	c_put_str(TERM_WHITE, "abcdefghijkl@", row, col);
#ifdef JP
c_put_str(TERM_L_GREEN, "能力修正", row - 1, col);
#else
	c_put_str(TERM_L_GREEN, "Modification", row - 1, col);
#endif


	/* Process equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Access object */
		o_ptr = &inventory[i];

		/* Acquire "known" flags */
		object_flags_known(o_ptr, flgs);

		/* Initialize color based of sign of pval. */
		///sys パラメータが10以上上がるような装備は色を変えて一の桁だけ出すようにしてみる？
		for (stat = 0; stat < 6; stat++)
		{
			/* Default */
			a = TERM_SLATE;
			c = '.';

			if(check_invalidate_inventory(i))
			{
				//無効化された装備品には何もしない
			}

			/* Boost */
			else if (have_flag(flgs, stat))
			{
				/* Default */
				c = '*';

				/* Good */
				if (o_ptr->pval > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				if (have_flag(flgs, stat + TR_SUST_STR))
				{
					/* Dark green for sustained stats */
					a = TERM_GREEN;
				}

				/* Bad */
				if (o_ptr->pval < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (o_ptr->pval > -10) c = '0' - o_ptr->pval;
				}
			}

			/* Sustain */
			else if (have_flag(flgs, stat + TR_SUST_STR))
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Dump proper character */
			Term_putch(col, row + stat+1, a, c);
		}

		/* Advance */
		col++;
	}

	//v1.1.46 女苑が拡張スロットに装備した指輪のパラメータ表示
	if (p_ptr->pclass == CLASS_JYOON)
	{
		int tmp_col = stat_col + 44;//dスロットから開始
		int j;

		//右手指、左手指ループ
		for (i = INVEN_RIGHT; i <= INVEN_LEFT; i++)
		{
			int stat_sum[6] = { 0,0,0,0,0,0 };
			bool sust_stat[6] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };
			if (check_invalidate_inventory(i)) continue;

			//右手指、左手指それぞれに対応する拡張インベントリのスロットスープ 0-3が右手で4-7が左手
			for (j = 0; j<4; j++)
			{
				int slot = (i - INVEN_RIGHT) * 4 + j;
				int pv;
				o_ptr = &inven_add[slot];
				if (!o_ptr->k_idx) continue;

				pv = o_ptr->pval / 2; //指輪はpval半減とする
				object_flags_known(o_ptr, flgs);
				//パラメータループ　A_STR～A_CHRが0～6であることが前提になっている
				//対応パラメータのpvalあるいは維持フラグを集計する
				for (stat = 0; stat < 6; stat++)
				{
					if (have_flag(flgs, stat))
						stat_sum[stat] += pv;

					if (have_flag(flgs, stat + TR_SUST_STR))
						sust_stat[stat] = TRUE;

				}
			}

			//集計が済んだパラメータを表示する。パラメータの正負と維持能力の有無で色や記号を変える。
			for (stat = 0; stat < 6; stat++)
			{
				a = TERM_SLATE;
				c = '.';
				if (stat_sum[stat] > 0)
				{
					c = '*';
					a = TERM_L_GREEN;

					if (stat_sum[stat] > 0 && stat_sum[stat] < 10)
					c = '0' + stat_sum[stat];
					if (sust_stat[stat])
						a = TERM_GREEN;
				}

				else if (stat_sum[stat] < 0)
				{
					a = TERM_RED;
					if (stat_sum[stat] > -10)
						c = '0' - stat_sum[stat];
				}
				else if (sust_stat[stat])
				{
					a = TERM_GREEN;
					c = 's';
				}
				Term_putch(tmp_col, row + stat + 1, a, c);
			}
			tmp_col++;

		}
	}




	/* Player flags */
	player_flags(flgs);

	/* Check stats */
	for (stat = 0; stat < 6; stat++)
	{
		/* Default */
		a = TERM_SLATE;
		c = '.';



		/* Mutations ... */
		///mod140316 もう無条件でここの中に入ることにする。大した処理増にもならんだろう。
	//	if (p_ptr->muta2 || p_ptr->muta3 || p_ptr->tsuyoshi || p_ptr->pclass == CLASS_KOGASA)
		{
			int dummy = 0;
			feature_type *f_ptr = &f_info[cave[py][px].feat];





			///mod140316 地形によるパラメータ変化
			//種族
			if((p_ptr->prace == RACE_KAPPA) && have_flag(f_ptr->flags, FF_WATER))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if ((p_ptr->prace == RACE_YAMAWARO) && have_flag(f_ptr->flags, FF_TREE))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 5 && have_flag(f_ptr->flags, FF_LAVA))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 27 && have_flag(f_ptr->flags, FF_PLANT))
			{
				if (stat == A_STR || stat == A_WIS || stat == A_CON || stat == A_DEX) dummy += 1;
			}
			else if(CHECK_FAIRY_TYPE == 30 && !dun_level && turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				dummy += 1;
			}
			else if(CHECK_FAIRY_TYPE == 39 && have_flag(f_ptr->flags, FF_WATER))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 43 && have_flag(f_ptr->flags, FF_TREE))
			{
				if (stat == A_STR || stat == A_WIS || stat == A_CON) dummy += 1;
			}
			//職業
			if(p_ptr->pclass == CLASS_WAKASAGI && have_flag(f_ptr->flags, FF_WATER)) dummy += 1 + p_ptr->lev / 11;
			else if(p_ptr->pclass == CLASS_MURASA && have_flag(f_ptr->flags, FF_WATER)) dummy += 2;

			if(CHECK_FAIRY_TYPE == 46 && p_ptr->image)
			{
				dummy += 3;
			}

			//バイド細胞に侵食されると全能力+5
			if(p_ptr->muta3 & MUT3_BYDO && stat != A_CHR) dummy += 5;
			if(p_ptr->muta3 & MUT3_BYDO && stat == A_CHR) dummy -= 50;

			if (p_ptr->muta4 & MUT4_GHOST_OTTER) dummy -= 1;

			//アル中がシラフになると能力ダウン
			if(p_ptr->muta2 & MUT2_ALCOHOL && p_ptr->alcohol == DRANK_0) dummy -= 3;

			///mod150110 橙が濡れたらパラメータダウン
			if(p_ptr->pclass == CLASS_CHEN && p_ptr->magic_num1[0]) dummy -= 3;

			/*:::Hack- 小傘の本体が呪われたらパラメータダウン*/
			if(p_ptr->pclass == CLASS_KOGASA)
			{
				if( inventory[INVEN_RARM].curse_flags & TRC_HEAVY_CURSE) dummy -= 3;
				else if( inventory[INVEN_RARM].curse_flags & TRC_CURSED) dummy -= 1;
			}

			if (stat == A_STR)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (p_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (p_ptr->tsuyoshi) dummy += 4;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH) dummy -= 4;
				if (p_ptr->muta4 & MUT4_GHOST_COW) dummy += 4;

			}
			else if (stat == A_INT)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;


				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_FEVER) dummy -= 2;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta2 & MUT2_HARDHEAD) dummy -= 1;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy += 2;

			}
			else if (stat == A_WIS)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;

				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta2 & MUT2_HARDHEAD) dummy += 1;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy += 2;
			}


			else if (stat == A_DEX)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_FEVER) dummy -= 2;
				//if (p_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (p_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
				if (p_ptr->muta2 & MUT2_TAIL) dummy += 4;
				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;

			}
			else if (stat == A_CON)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (p_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (p_ptr->tsuyoshi) dummy += 4;
				if(p_ptr->asthma >= ASTHMA_4) dummy -= 4;
				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_COW) dummy += 4;

			}
			else if (stat == A_CHR)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;
				else if (REF_BROKEN_VAL) dummy -= 2;

				if (p_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 3;
			//	if (p_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				//if (p_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				//if (p_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
				if (p_ptr->muta3 & (MUT3_ACID_SCALES | MUT3_ELEC_SCALES | MUT3_FIRE_SCALES | MUT3_COLD_SCALES | MUT3_POIS_SCALES)) dummy -= 2;

				if (p_ptr->muta4 & MUT4_GHOST_CHICKEN) dummy += 4;
			}




			/* Boost */
			if (dummy)
			{
				/* Default */
				c = '*';

				/* Good */
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				/* Bad */
				if (dummy < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (dummy > -10) c = '0' - dummy;
				}
			}
		}


		/* Sustain */
		if (have_flag(flgs, stat + TR_SUST_STR))
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}




		/* Dump */
		Term_putch(col, row + stat+1, a, c);
	}
}


/*
 * Display the character on the screen (various modes)
 *
 * The top one and bottom two lines are left blank.
 *
 * Mode 0 = standard display with skills
 * Mode 1 = standard display with history
 * Mode 2 = summary of various things
 * Mode 3 = summary of various things (part 2)
 * Mode 4 = mutations
 */
/*:::＠の情報を表示する。Cコマンド・ウィンドウオプション・ゲーム完了時ダンプ　詳細未読*/
///sys ＠の情報表示　重要
void display_player(int mode)
{
	int i;

	char	buf[80];
	char	tmp[64];


	/* XXX XXX XXX */
	/*:::Cコマンドで突然変異を表示するオプションがONのとき?　Cコマンドではhを押すたびmodeが加算されるためこうやって余りを計算するらしい*/
	if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4) && display_mutations)
		mode = (mode % 5);
	else
		mode = (mode % 4);

	/* Erase screen */
	clear_from(0);

	/* Standard */
	if ((mode == 0) || (mode == 1))
	{
		/* Name, Sex, Race, Class */
#ifdef JP
	if(p_ptr->psex == SEX_MALE)
		sprintf(tmp, "%s%s%s", ap_ptr->title, ap_ptr->no == 1 ? "の":"", player_name);
	else
		sprintf(tmp, "%s%s%s", ap_ptr->f_title, ap_ptr->no == 1 ? "の":"", player_name);
#else
    if(p_ptr->psex == SEX_MALE)
		sprintf(tmp, "%s %s", ap_ptr->title, player_name);
    else
        sprintf(tmp, "%s %s", ap_ptr->f_title, player_name);
#endif

		display_player_one_line(ENTRY_NAME, tmp, TERM_L_BLUE);
		display_player_one_line(ENTRY_SEX, sp_ptr->title, TERM_L_BLUE);
		///mod150103 種族名記述　野良神様専用処理追加
		if(prace_is_(RACE_STRAYGOD))
			display_player_one_line(ENTRY_RACE, deity_table[p_ptr->race_multipur_val[3]].deity_name, TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_RACE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), TERM_L_BLUE);
	//	display_player_one_line(ENTRY_CLASS, cp_ptr->title, TERM_L_BLUE);
		///mod140206 職業名変更対応
		display_player_one_line(ENTRY_CLASS, player_class_name, TERM_L_BLUE);

		if (p_ptr->realm1)
		{
			if (p_ptr->realm2)
				sprintf(tmp, "%s, %s", realm_names[p_ptr->realm1], realm_names[p_ptr->realm2]);
			else
				strcpy(tmp, realm_names[p_ptr->realm1]);

			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				display_player_one_line(ENTRY_REALM, tmp, TERM_L_DARK);
			else
				display_player_one_line(ENTRY_REALM, tmp, TERM_L_BLUE);
		}
		///sys muta class カオスパトロンの記述
		//if ((p_ptr->pclass == CLASS_CHAOS_WARRIOR) || (p_ptr->muta2 & MUT2_CHAOS_GIFT))
		if(IS_METAMORPHOSIS)
			display_player_one_line(ENTRY_METAMORPHOSIS, extra_mon_name, TERM_L_BLUE);

		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
			display_player_one_line(ENTRY_PATRON, chaos_patrons[p_ptr->chaos_patron], TERM_L_BLUE);
		//v1.1.70 動物霊の所属勢力
		else if (p_ptr->prace == RACE_ANIMAL_GHOST)
		{
			display_player_one_line(ENTRY_ANIMAL_GHOST_STRIFE, animal_ghost_strife_names[p_ptr->race_multipur_val[0]], TERM_L_BLUE);
		}

		//v1.1.15 ヘカーティアの今の体がどれか表示
		if(p_ptr->pclass == CLASS_HECATIA)
		{
			cptr hecatia_body_desc[] = {_("異界", "Otherworld"),_("地球","Earth"),_("月", "Moon")};
			display_player_one_line(ENTRY_HECATIA_BODY, hecatia_body_desc[p_ptr->magic_num2[0]], TERM_L_BLUE);

		}

		/* Age, Height, Weight, Social */
		/* 身長はセンチメートルに、体重はキログラムに変更してあります */
		///sysdel 年齢身長体重
#ifdef JP
		///del131221 年齢体格社会的地位を表示しない
		//display_player_one_line(ENTRY_AGE, format("%d才" ,(int)p_ptr->age), TERM_L_BLUE);
		//display_player_one_line(ENTRY_HEIGHT, format("%dcm" ,(int)((p_ptr->ht*254)/100)), TERM_L_BLUE);
		//display_player_one_line(ENTRY_WEIGHT, format("%dkg" ,(int)((p_ptr->wt*4536)/10000)), TERM_L_BLUE);
		//display_player_one_line(ENTRY_SOCIAL, format("%d  " ,(int)p_ptr->sc), TERM_L_BLUE);
#else
		//display_player_one_line(ENTRY_AGE, format("%d" ,(int)p_ptr->age), TERM_L_BLUE);
		//display_player_one_line(ENTRY_HEIGHT, format("%d" ,(int)p_ptr->ht), TERM_L_BLUE);
		//display_player_one_line(ENTRY_WEIGHT, format("%d" ,(int)p_ptr->wt), TERM_L_BLUE);
		//display_player_one_line(ENTRY_SOCIAL, format("%d" ,(int)p_ptr->sc), TERM_L_BLUE);
#endif
		///del131229 属性を表示しない
		//display_player_one_line(ENTRY_ALIGN, format("%s" ,your_alignment()), TERM_L_BLUE);


	if(difficulty == DIFFICULTY_EASY)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,_("EASY", "Easy")), TERM_L_GREEN);
	if(difficulty == DIFFICULTY_NORMAL) display_player_one_line(ENTRY_SOCIAL, format("%s" ,_("NORMAL", "Normal")), TERM_L_WHITE);
	if(difficulty == DIFFICULTY_HARD)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,_("HARD", "Hard")), TERM_L_BLUE);
	if(difficulty == DIFFICULTY_LUNATIC)display_player_one_line(ENTRY_SOCIAL, format("%s" ,_("LUNATIC", "Lunatic")), TERM_VIOLET);
	if(difficulty == DIFFICULTY_EXTRA)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,_("EXTRA", "Extra")), TERM_L_RED);
	//スコア表示　キャラクターメイク時には表示しない
	//v1.1.53b スコア上限40億に拡張 luはunsigned long
	if(character_generated) display_player_one_line(ENTRY_ALIGN, format("%lu" ,(unsigned long)total_points_new(FALSE)), TERM_L_WHITE);


		///mod140104 パラメータ上限と表記を変更
		///mod140326 パラメータ上限突破処理に対応
		/* Display the stats */
		for (i = 0; i < 6; i++)
		{
			/* Special treatment of "injured" stats */
			if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			{
				int value;

				/* Use lowercase stat name */
				put_str(stat_names_reduced[i], 3 + i, 53);

				/* Get the current stat */
				//value = p_ptr->stat_use[i];
				value = p_ptr->stat_ind[i]+3;

				/* Obtain the current stat (modified) */
				//cnv_stat(value, buf);
				//能力低下したがまだ限界突破で50のとき
				if(value == STAT_HYPERMAX)
				{
					sprintf(buf,"   *%2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
				}
				//能力低下したがまだ限界突破のとき
				else if(value > STAT_ADD_MAX)
				{
					sprintf(buf,"   !%2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
					value = p_ptr->stat_ind[i]+3 + (p_ptr->stat_max[i] - p_ptr->stat_cur[i]);
					if(value == STAT_HYPERMAX) sprintf(buf,"   *%2d",value);
					else sprintf(buf,"   !%2d",value);
					c_put_str(TERM_L_BLUE, buf, 3 + i, 67);

				}
				//通常の能力低下時
				else
				{
					if(value == STAT_ADD_MAX) sprintf(buf,"   !%2d",value);
					else sprintf(buf,"    %2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_YELLOW, buf, 3 + i, 60);
					value = p_ptr->stat_ind[i]+3 + (p_ptr->stat_max[i] - p_ptr->stat_cur[i]);
					if(value > STAT_ADD_MAX)
					{
						value = STAT_ADD_MAX; //pend 限界突破中で現在値40未満になった時の表示がおかしいかもしれないが放置
						sprintf(buf,"   !%2d",value);
						c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
					}
					else
					{
						sprintf(buf,"    %2d",value);
						c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
					}
				}
				/* Display the current stat (modified) */
				//c_put_str(TERM_YELLOW, buf, 3 + i, 60);

				/* Acquire the max stat */
				//value = p_ptr->stat_top[i];

				/* Obtain the maximum stat (modified) */
				//cnv_stat(value, buf);

				/* Display the maximum stat (modified) */
				//c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
			}

			/* Normal treatment of "normal" stats */
			else
			{
				int value;
				value = p_ptr->stat_ind[i]+3;
				/* Assume uppercase stat name */
				put_str(stat_names[i], 3 + i, 53);
				/* Obtain the current stat (modified) */
				//cnv_stat(p_ptr->stat_use[i], buf);
				if(value == STAT_HYPERMAX) sprintf(buf,"   *%2d",value);
				else if(value >= STAT_ADD_MAX) sprintf(buf,"   !%2d",value);
				else sprintf(buf,"    %2d",value);

				/* Display the current stat (modified) */
				if(value > STAT_ADD_MAX) c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
				else c_put_str(TERM_L_GREEN, buf, 3 + i, 60);
			}

			if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
			{
#ifdef JP
				//c_put_str(TERM_WHITE, "!", 3 + i, 58);
#else
				c_put_str(TERM_WHITE, "!", 3 + i, 58-2);
#endif
			}
		}
		/* Display "history" info */
		if (mode == 1)
		{
			char statmsg[1000];

#ifdef JP
			put_str("(キャラクターの生い立ち)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				put_str(p_ptr->history[i], i + 12, 10);
			}

			*statmsg = '\0';

			if (p_ptr->is_dead)
			{
				if (p_ptr->total_winner)
				{
#ifdef JP
					if(streq(p_ptr->died_from, "hourai"))
					{
						sprintf(statmsg, "…あなたは永遠を生きる蓬莱人となった。");
					}
					else if (streq(p_ptr->died_from, "true_loser"))
					{
						sprintf(statmsg, "…あなたは誰一人勝者のいない世界をもたらした。");
					}
					else if(p_ptr->pclass == CLASS_OUTSIDER)
					{
						if (streq(p_ptr->died_from, "triumph"))
							strcpy(statmsg, "…あなたは幻想郷の英雄として外界へ帰還した。");
						else
							sprintf(statmsg, "…あなたは幻想郷に永住した。");
					}
					else
						sprintf(statmsg, "…あなたは勝利の後引退した。");

//					sprintf(statmsg, "…あなたは勝利の後%sした。", streq(p_ptr->died_from, "Seppuku") ? "切腹" : "引退");
#else
                    if(streq(p_ptr->died_from, "hourai"))
					{
						sprintf(statmsg, "...You became an eternally living person of Hourai.");
					}
					else if (streq(p_ptr->died_from, "true_loser"))
					{
						sprintf(statmsg, "...You have brought about a world where no one has won. ");
					}
					else if(p_ptr->pclass == CLASS_OUTSIDER)
					{
						if (streq(p_ptr->died_from, "triumph"))
							strcpy(statmsg, "...You returned to the outside world as a hero of Gensoukyou.");
						else
							sprintf(statmsg, "...You took permanent residence in Gensoukyou.");
					}
					else
						sprintf(statmsg, "...You retired from the adventure after winning.");
#endif
				}
				else if (streq(p_ptr->died_from, "escape"))
				{
#ifdef JP
					strcpy(statmsg, "…あなたは幻想郷から脱出した。");
#else
                    strcpy(statmsg, "...You escaped from Gensoukyou.");
#endif
				}
				else if (streq(p_ptr->died_from, "lost"))
				{
#ifdef JP
					strcpy(statmsg, "…あなたは最初から存在しなかった。");
#else
                    strcpy(statmsg, "...You have never existed in the first place.");
#endif
				}
				else if (streq(p_ptr->died_from, "mission_failed"))//永琳で輝夜が倒された時は原因をダンプ出力
				{
#ifdef JP
					char m_name[80];
					int r_idx = p_ptr->magic_num1[0];
					if(!r_idx) sprintf(m_name,"あなたに倒された。");
					else if(r_idx < 0) sprintf(m_name,"不慮の事故により倒れた。");
					else if(IS_RANDOM_UNIQUE_IDX(r_idx))
						sprintf(m_name,"%sに倒された。", random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
					else sprintf(m_name,"%sに倒された。",r_name + r_info[r_idx].name);

					sprintf(statmsg, "…輝夜は%sで%s",map_name(),m_name);
#else
                    char m_name[80];
					int r_idx = p_ptr->magic_num1[0];
					if(!r_idx) sprintf(m_name,"you");
					else if(r_idx < 0) sprintf(m_name,"an unfortunate accident");
					else if(IS_RANDOM_UNIQUE_IDX(r_idx))
						sprintf(m_name,"defeated by %s", random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
					else sprintf(m_name,"defeated by %s",r_name + r_info[r_idx].name);

					sprintf(statmsg, "...Kaguya was %s in %s.",m_name,map_name());
#endif
				}
				else if (!dun_level)
				{
#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "…あなたは%sで%sに殺された。", map_name(), p_ptr->died_from);
					sprintf(statmsg, "…あなたは%sで%sに倒された。", map_name(), p_ptr->died_from);

#else
					sprintf(statmsg, "...You were defeated by %s in %s.", p_ptr->died_from, map_name());
#endif
				}
				else if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest))
				{
					/* Get the quest text */
					/* Bewere that INIT_ASSIGN resets the cur_num. */
					init_flags = INIT_NAME_ONLY;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "…あなたは、クエスト「%s」で%sに殺された。", quest[p_ptr->inside_quest].name, p_ptr->died_from);
					sprintf(statmsg, "…あなたは、クエスト「%s」で%sに倒された。", quest[p_ptr->inside_quest].name, p_ptr->died_from);

#else
					sprintf(statmsg, "...You were defeated by %s in the quest '%s'.", p_ptr->died_from, quest[p_ptr->inside_quest].name);
#endif
				}
				else if(INSIDE_EXTRA_QUEST)
				{
#ifdef JP
					sprintf(statmsg, "…あなたは、%d階のエクストラクエスト中に%sに倒された。",dun_level, p_ptr->died_from);
#else
                    sprintf(statmsg, "...You were defeated by %s on level %d in an extra quest.",p_ptr->died_from, dun_level);
#endif
				}
				else
				{
#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "…あなたは、%sの%d階で%sに殺された。", map_name(), dun_level, p_ptr->died_from);
					sprintf(statmsg, "…あなたは、%sの%d階で%sに倒された。", map_name(), dun_level, p_ptr->died_from);
#else
					sprintf(statmsg, "...You were defeated by %s on level %d of %s.", p_ptr->died_from, dun_level, map_name());
#endif
				}
			}
			else if (character_dungeon)
			{
				if (!dun_level)
				{
#ifdef JP
					sprintf(statmsg, "…あなたは現在、 %s にいる。", map_name());
#else
					sprintf(statmsg, "...Now, you are in %s.", map_name());
#endif
				}
				else if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest))
				{
					/* Clear the text */
					/* Must be done before doing INIT_SHOW_TEXT */
					for (i = 0; i < 10; i++)
					{
						quest_text[i][0] = '\0';
					}
					quest_text_line = 0;

					/* Get the quest text */
					init_flags = INIT_NAME_ONLY;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					sprintf(statmsg, "…あなたは現在、 クエスト「%s」を遂行中だ。", quest[p_ptr->inside_quest].name);
#else
					sprintf(statmsg, "...Now, you are in the quest '%s'.", quest[p_ptr->inside_quest].name);
#endif
				}
				else if(INSIDE_EXTRA_QUEST)
				{
#ifdef JP
					sprintf(statmsg, "…あなたは、%d階のエクストラクエストを遂行中だ。",dun_level);
#else
                    sprintf(statmsg, "...Now, you are in an extra quest on dungeon level %d.",dun_level);
#endif
				}
				else
				{
#ifdef JP
					sprintf(statmsg, "…あなたは現在、 %s の %d 階で探索している。", map_name(), dun_level);
#else
					sprintf(statmsg, "...Now, you are exploring level %d of %s.", dun_level, map_name());
#endif
				}
			}

			if (*statmsg)
			{
				char temp[64*2], *t;
				roff_to_buf(statmsg, 60, temp, sizeof(temp));
				t = temp;
				for(i=0 ; i<2 ; i++)
				{
					if(t[0]==0)
						break;
					else
					{
						put_str(t, i + 5 + 12, 10);
						t += strlen(t)+1;
					}
				}
			}

		}

		/* Display "various" info */
		else
		{
			display_player_middle();
			display_player_various();
		}
	}

	/* Special */
	else if (mode == 2)
	{
		/* See "http://www.cs.berkeley.edu/~davidb/angband.html" */

		/* Dump the info */
		display_player_misc_info();
		display_player_stat_info();
		display_player_flag_info();
	}

	/* Special */
	else if (mode == 3)
	{
		display_player_other_flag_info();
	}

	else if (mode == 4)
	{
		do_cmd_knowledge_mutations();
	}
}


/*
 *
 */
static void dump_aux_display_player(FILE *fff)
{
	int x, y;
	byte a;
	char c;
	char		buf[1024];

	/* Display player */
	display_player(0);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
#ifdef JP
		fprintf(fff, "%s\n", buf);
#else
		fprintf(fff, "%s\n", buf);
#endif

	}

	/* Display history */
	display_player(1);

	/* Dump part of the screen */
	for (y = 10; y < 19; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 1) */
	display_player(2);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 2) */
	display_player(3);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	//v2.0.13 体力ランクを表示する
	//ゲーム終了時と自己分析後のみ
	if (p_ptr->is_dead || (p_ptr->knowledge & KNOW_HPRATE))
	{
		int percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
			(2 * p_ptr->hitdie + ((PY_MAX_LEVEL - 1 + 3) * (p_ptr->hitdie + 1))));

		fprintf(fff, _("\n 現在の体力ランク : %d/100\n", "Current life rating: %d/100"), percent);
	}

	fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_pet(FILE *fff)
{
	int i;
	bool pet = FALSE;
	bool pet_settings = FALSE;
	char pet_name[80];

	for (i = m_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (!is_pet(m_ptr)) continue;
		pet_settings = TRUE;
		if (!m_ptr->nickname && (p_ptr->riding != i)) continue;
		if (!pet)
		{
#ifdef JP
			fprintf(fff, "\n\n  [主な配下]\n\n");
#else
			fprintf(fff, "\n\n  [Leading Pets]\n\n");
#endif
			pet = TRUE;
		}
		monster_desc(pet_name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
		fprintf(fff, "%s\n", pet_name);
	}

	if (pet_settings)
	{
#ifdef JP
		fprintf(fff, "\n\n  [配下への命令]\n");
#else
		fprintf(fff, "\n\n  [Command for Followers]\n");
#endif

#ifdef JP
		fprintf(fff, "\n ドアを開ける:                       %s", (p_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Followers open doors:               %s", (p_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n アイテムを拾う:                     %s", (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Followers pick up items:            %s", (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n テレポート系魔法を使う:             %s", (p_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow teleport:                     %s", (p_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n 攻撃魔法を使う:                     %s", (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow cast attack spell:            %s", (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n 召喚魔法を使う:                     %s", (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow cast summon spell:            %s", (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n プレイヤーを巻き込む範囲魔法を使う: %s", (p_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow involve player in area spell: %s", (p_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#endif

		fputc('\n', fff);
	}
}


/*
 *
 */
static void dump_aux_class_special(FILE *fff)
{
	if (p_ptr->pclass == CLASS_BLUE_MAGE)
	{
		int i = 0;
		int j = 0;
		int l1 = 0;
		int l2 = 0;
		int num = 0;
		int spellnum[MAX_MONSPELLS];
		s32b f4 = 0, f5 = 0, f6 = 0;
		char p[60][80];
		int col = 0;
		bool pcol = FALSE;

		for (i=0;i<60;i++) { p[i][0] = '\0'; }

#ifdef JP
		strcat(p[col], "\n\n  [学習済みの青魔法]\n");
#else
		strcat(p[col], "\n\n  [Learned Blue Magic]\n");
#endif


		for (j=1;j<6;j++)
		{
			col++;
			set_rf_masks(&f4, &f5, &f6, j);
			switch(j)
			{
				case MONSPELL_TYPE_BOLT:
#ifdef JP
					strcat(p[col], "\n     [ボルト型]\n");
#else
					strcat(p[col], "\n     [Bolt  Type]\n");
#endif
					break;

				case MONSPELL_TYPE_BALL:
#ifdef JP
					strcat(p[col], "\n     [ボール型]\n");
#else
					strcat(p[col], "\n     [Ball  Type]\n");
#endif
					break;

				case MONSPELL_TYPE_BREATH:
#ifdef JP
					strcat(p[col], "\n     [ブレス型]\n");
#else
					strcat(p[col], "\n     [  Breath  ]\n");
#endif
					break;

				case MONSPELL_TYPE_SUMMON:
#ifdef JP
					strcat(p[col], "\n     [召喚魔法]\n");
#else
					strcat(p[col], "\n     [Summonning]\n");
#endif
					break;

				case MONSPELL_TYPE_OTHER:
#ifdef JP
					strcat(p[col], "\n     [ その他 ]\n");
#else
					strcat(p[col], "\n     [Other Type]\n");
#endif
					break;
			}

			for (i = 0, num = 0; i < 32; i++)
			{
				if ((0x00000001 << i) & f4) spellnum[num++] = i;
			}
			for (; i < 64; i++)
			{
				if ((0x00000001 << (i - 32)) & f5) spellnum[num++] = i;
			}
			for (; i < 96; i++)
			{
				if ((0x00000001 << (i - 64)) & f6) spellnum[num++] = i;
			}

			col++;
			pcol = FALSE;
			strcat(p[col], "       ");

			for (i = 0; i < num; i++)
			{
				if (p_ptr->magic_num2[spellnum[i]])
				{
					pcol = TRUE;
					/* Dump blue magic */
					l1 = strlen(p[col]);
					l2 = strlen(monster_powers_short[spellnum[i]]);
					if ((l1 + l2) >= 75)
					{
						strcat(p[col], "\n");
						col++;
						strcat(p[col], "       ");
					}
					strcat(p[col], monster_powers_short[spellnum[i]]);
					strcat(p[col], ", ");
				}
			}

			if (!pcol)
			{
#ifdef JP
				strcat(p[col], "なし");
#else
				strcat(p[col], "None");
#endif
			}
			else
			{
				if (p[col][strlen(p[col])-2] == ',')
				{
					p[col][strlen(p[col])-2] = '\0';
				}
				else
				{
					p[col][strlen(p[col])-10] = '\0';
				}
			}

			strcat(p[col], "\n");
		}

		for (i=0;i<=col;i++)
		{
			fputs(p[i], fff);
		}
	}
	else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
	{
		char s[EATER_EXT][MAX_NLEN];
		int tval, ext, k_idx;
		int i, magic_num;

		if (p_ptr->pclass == CLASS_SEIJA)
			fprintf(fff, _("\n\n  [隠し持った魔法道具]\n",
                            "\n\n [  Hidden Devices  ]"));
		else
			fprintf(fff, _("\n\n  [取り込んだ魔法道具]\n",
                            "\n\n [ Absorbed Devices ]"));

		for (ext = 0; ext < 3; ext++)
		{
			int eat_num = 0;

			/* Dump an extent name */
			switch (ext)
			{
			case 0:
				tval = TV_STAFF;
#ifdef JP
				fprintf(fff, "\n[杖]\n");
#else
				fprintf(fff, "\n[Staffs]\n");
#endif
				break;
			case 1:
				tval = TV_WAND;
#ifdef JP
				fprintf(fff, "\n[魔法棒]\n");
#else
				fprintf(fff, "\n[Wands]\n");
#endif
				break;
			case 2:
				tval = TV_ROD;
#ifdef JP
				fprintf(fff, "\n[ロッド]\n");
#else
				fprintf(fff, "\n[Rods]\n");
#endif
				break;
			}

			/* Get magic device names that were eaten */
			for (i = 0; i < EATER_EXT; i++)
			{
				int idx = EATER_EXT * ext + i;

				magic_num = p_ptr->magic_num2[idx];
				if (!magic_num) continue;

				k_idx = lookup_kind(tval, i);
				if (!k_idx) continue;
				sprintf(s[eat_num], "%23s (%2d)", (k_name + k_info[k_idx].name), magic_num);
				eat_num++;
			}

			/* Dump magic devices in this extent */
			if (eat_num > 0)
			{
				for (i = 0; i < eat_num; i++)
				{
					fputs(s[i], fff);
					if (i % 3 < 2) fputs("    ", fff);
					else fputs("\n", fff);
				}

				if (i % 3 > 0) fputs("\n", fff);
			}
			else /* Not found */
			{
#ifdef JP
				fputs("  (なし)\n", fff);
#else
				fputs("  (none)\n", fff);
#endif
			}
		}
	}
	//v1.1.52 菫子専用性格の特技をリストアップ
	//v2.0.4 魅須丸の勾玉も追加
	else if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) || p_ptr->pclass == CLASS_MISUMARU)
	{
		int i,imax;


		if (p_ptr->pclass == CLASS_MISUMARU)
		{
			fprintf(fff, _("\n\n  [勾玉の力]\n",
                        "\n\n [Magatama Powers]\n"));
			imax = INVEN2_MAGATAMA_NUM_MAX;
		}
		else
		{
			fprintf(fff, _("\n\n  [後戸の力]\n",
                        "\n\n [Power of Backdoor]\n"));
			imax = 8;
		}

		//特技最大数ループ
		for (i = 0; i < imax; i++)
		{
			char buf[256] = "";
			int quark_idx=0;
			int art_idx,typ,rad,base,dice,sides,cost,xtra1;

			//魅須丸は装備している勾玉から技のパラメータを取得
			if (p_ptr->pclass == CLASS_MISUMARU)
			{
				char o_name[128];
				object_type *o_ptr = &inven_add[i];

				if (!o_ptr->k_idx || o_ptr->tval != TV_SPELLCARD) continue;

				art_idx = o_ptr->pval;
				typ = o_ptr->xtra1;
				rad = o_ptr->xtra2;
				base = o_ptr->ac;
				dice = o_ptr->dd;
				sides = o_ptr->to_a;
				cost = o_ptr->to_h;
				xtra1 = o_ptr->xtra4;
				object_desc(o_name, o_ptr, OD_NAME_ONLY);
				fprintf(fff, " %s(%s)\n", o_name, nameless_skill_type_desc[art_idx]); //名称と特技種別

			}
			//ほか(悪夢菫子)はp_ptr->magic_num1[]から技のパラメータを取得
			else
			{
				quark_idx = JUNKO_ARTS_PARAM(i, JKAP_QUARK_IDX);
				art_idx = JUNKO_ARTS_PARAM(i, JKAP_JKF1);
				typ = JUNKO_ARTS_PARAM(i, JKAP_GF);
				rad = JUNKO_ARTS_PARAM(i, JKAP_RAD);
				base = JUNKO_ARTS_PARAM(i, JKAP_BASE);
				dice = JUNKO_ARTS_PARAM(i, JKAP_DICE);
				sides = JUNKO_ARTS_PARAM(i, JKAP_SIDES);
				cost = JUNKO_ARTS_PARAM(i, JKAP_COST);
				xtra1 = JUNKO_ARTS_PARAM(i, JKAP_XTRA_VAL1);
				if (!art_idx) break;
				fprintf(fff, "    %s(%s)\n", quark_str(quark_idx), nameless_skill_type_desc[art_idx]); //名称と特技種別
			}

			fprintf(fff, _("      消費MP:%-3d",
                            "      MP Cost:%-3d"), cost);
			if (typ && art_idx != JKF1_ADD_ESSENCE1 && art_idx != JKF1_ADD_ESSENCE2)
				fprintf(fff, _("  属性:%-10s",
                                "Elemnt:%-10s"), gf_desc_name[typ]);
			if (rad>0)
				fprintf(fff,_("  半径:%-2d",
                                "Radius:%-2d"), ABS(rad));

			if (typ == GF_STONE_WALL || typ == GF_MAKE_DOOR || typ == GF_MAKE_TREE)
			{	} //壁生成などは内部的にダメージ値が記録されているが使わないので表示しない
			else if (art_idx == JKF1_ATTACK_BREATH)
				fprintf(fff, _("  威力:現在HPの%d%%",
                                " Power:%d%% of cur HP"), xtra1);
			else if (dice && sides && base)
				fprintf(fff, _("  威力/効果:%dd%d+%d",
                                " Power/Eff:%dd%d+%d"), dice, sides, base);
			else if (dice && sides)
				fprintf(fff, _("  威力/効果:%dd%d",
                                " Power/Eff:%dd%d"), dice, sides);
			else if (base)
				fprintf(fff, _("  威力/効果:%d",
                                " Power/Eff:%d"),base);

			fprintf(fff, "\n");

		}

	}
	//v1.1.81 弾幕研究家
	else if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		int i,j;
		int monspell_kind_type[6] = { NEW_MSPELL_TYPE_BOLT_BEAM_ROCKET ,NEW_MSPELL_TYPE_BALL ,NEW_MSPELL_TYPE_BREATH ,NEW_MSPELL_TYPE_OTHER_ATTACK ,NEW_MSPELL_TYPE_SUMMMON ,NEW_MSPELL_TYPE_OTHER };
		cptr monspell_kind_desc[6] = { _("(ボルト/ビーム/ロケット)", "(Bolts/Beams/Rocket)"),
                                   _("(ボール)", "(Balls)"),
                                   _("(ブレス)", "(Breaths)"),
                                   _("(その他の攻撃)", "(Other Attacks)"),
                                   _("(召喚)", "(Summoning)"),
                                   _("(その他)", "(Other)") };
		bool flag_exist;
		fprintf(fff, _("\n\n  [習得済みの特技]\n",
                        "\n\n  [Known special abilities]\n"));

		for (i = 0; i < 6; i++)
		{
			flag_exist = FALSE;
			fprintf(fff, "    %s\n",monspell_kind_desc[i]);

			for (j = 1; j <= MAX_MONSPELLS2; j++)
			{
				int spell_exp;

				//ボルト,ボール,ブレスなどの分類が合わなければパス
				if (monspell_list2[j].monspell_type != monspell_kind_type[i]) continue;
				//未習得ならパス
				if (!check_monspell_learned(j)) continue;

				flag_exist = TRUE;
				fprintf(fff, "      %s\n", monspell_list2[j].name);

			}
			if(!flag_exist)
				fprintf(fff, _("      (なし)\n",
                                "      (none)\n"));

		}

	}
}


/*
 *
 */
static void dump_aux_quest(FILE *fff)
{
	int i;
	int *quest_num;
	int dummy;


#ifdef JP
	fprintf(fff, "\n\n  [クエスト情報]\n");
#else
	fprintf(fff, "\n\n  [Quest Information]\n");
#endif

	/* Allocate Memory */
	C_MAKE(quest_num, max_quests, int);

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++) quest_num[i] = i;
	ang_sort_comp = ang_sort_comp_quest_num;
	ang_sort_swap = ang_sort_swap_quest_num;
	ang_sort(quest_num, &dummy, max_quests);

	/* Dump Quest Information */
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);
	fputc('\n', fff);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);
}


/*
 *
 */
static void dump_aux_last_message(FILE *fff)
{
	if (p_ptr->is_dead)
	{
		if (!p_ptr->total_winner)
		{
			int i;

#ifdef JP
			fprintf(fff, "\n  [倒れる直前に起こったこと]\n\n");
#else
			fprintf(fff, "\n  [What happened before defeat]\n\n");
#endif
			for (i = MIN(message_num(), 30); i >= 0; i--)
			{
				fprintf(fff,"> %s\n",message_str((s16b)i));
			}
			fputc('\n', fff);
		}

		/* Hack -- *Winning* message */
		else if (p_ptr->last_message)
		{
#ifdef JP
			fprintf(fff, "\n  [*勝利*メッセージ]\n\n");
#else
			fprintf(fff, "\n  [*Winning* Message]\n\n");
#endif
			fprintf(fff,"  %s\n", p_ptr->last_message);
			fputc('\n', fff);
		}
	}
}


/*
 *
 */
static void dump_aux_recall(FILE *fff)
{
	int y;

#ifdef JP
	fprintf(fff, "\n  [帰還場所]\n\n");
#else
	fprintf(fff, "\n  [Recall Depth]\n\n");
#endif

	for (y = 1; y < max_d_idx; y++)
	{
		bool seiha = FALSE;

		if(flag_dungeon_complete[y]) seiha = TRUE;

		if (!d_info[y].maxdepth) continue;
		if (!max_dlv[y]) continue;
#if 0
		if (d_info[y].final_guardian)
		{
			if (!r_info[d_info[y].final_guardian].max_num) seiha = TRUE;
		}
		/*:::オベロンとサーペント倒しても帰還が最深でないと制覇マークがつかなかったので修正*/
		else if(y == DUNGEON_ANGBAND && quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED) seiha = TRUE;
		else if(y == DUNGEON_CHAOS && quest[QUEST_SERPENT].status == QUEST_STATUS_FINISHED) seiha = TRUE;
		else if (max_dlv[y] == d_info[y].maxdepth) seiha = TRUE;
#endif

#ifdef JP
		fprintf(fff, "   %c%-20s: %3d 階\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#else
		fprintf(fff, "   %c%-16s: level %3d\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#endif
	}
}


/*
 *
 */
static void dump_aux_options(FILE *fff)
{
#ifdef JP
	fprintf(fff, "\n  [オプション設定]\n");
#else
	fprintf(fff, "\n  [Option Settings]\n");
#endif


	if (preserve_mode)
#ifdef JP
		fprintf(fff, "\n 保存モード:         ON");
#else
		fprintf(fff, "\n Preserve Mode:      ON");
#endif

	else
#ifdef JP
		fprintf(fff, "\n 保存モード:         OFF");
#else
		fprintf(fff, "\n Preserve Mode:      OFF");
#endif


	if (ironman_small_levels)
#ifdef JP
		fprintf(fff, "\n 小さいダンジョン:   ALWAYS");
#else
		fprintf(fff, "\n Small Levels:       ALWAYS");
#endif

	else if (always_small_levels)
#ifdef JP
		fprintf(fff, "\n 小さいダンジョン:   ON");
#else
		fprintf(fff, "\n Small Levels:       ON");
#endif

	else if (small_levels)
#ifdef JP
		fprintf(fff, "\n 小さいダンジョン:   ENABLED");
#else
		fprintf(fff, "\n Small Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n 小さいダンジョン:   OFF");
#else
		fprintf(fff, "\n Small Levels:       OFF");
#endif


	if (vanilla_town)
#ifdef JP
		fprintf(fff, "\n 元祖の町のみ:       ON");
#else
		fprintf(fff, "\n Vanilla Town:       ON");
#endif

	else if (lite_town)
#ifdef JP
		fprintf(fff, "\n 小規模な町:         ON");
#else
		fprintf(fff, "\n Lite Town:          ON");
#endif


	if (ironman_shops)
#ifdef JP
		fprintf(fff, "\n 店なし:             ON");
#else
		fprintf(fff, "\n No Shops:           ON");
#endif

	if (ironman_no_fixed_art)
		fprintf(fff, _("\n ★が出現しない:     ON",
                     "\n No fixed artifacts: ON"));

	if (no_capture_book)
		fprintf(fff, _("\n 妖魔本が出現しない: ON",
                     "\n No demon books:     ON"));


	if (ironman_downward)
#ifdef JP
		fprintf(fff, "\n 階段を上がれない:   ON");
#else
		fprintf(fff, "\n Diving Only:        ON");
#endif


	if (ironman_rooms)
#ifdef JP
		fprintf(fff, "\n 普通でない部屋:     ON");
#else
		fprintf(fff, "\n Unusual Rooms:      ON");
#endif


	if (ironman_nightmare)
#ifdef JP
		fprintf(fff, "\n 悪夢モード:         ON");
#else
		fprintf(fff, "\n Nightmare Mode:     ON");
#endif

	if (ironman_no_bonus)
		fprintf(fff, _("\n ボーナスポイント:   0",
                        "\n Bonus points:        0"));
	else if (birth_bonus_10)
		fprintf(fff, _("\n ボーナスポイント:   10",
                        "\n Bonus points:       10"));
	else if (birth_bonus_20)
		fprintf(fff, _("\n ボーナスポイント:   20",
                        "\n Bonus points:       20"));


	if (ironman_empty_levels)
#ifdef JP
		fprintf(fff, "\n アリーナ:           ALWAYS");
#else
		fprintf(fff, "\n Arena Levels:       ALWAYS");
#endif

	else if (empty_levels)
#ifdef JP
		fprintf(fff, "\n アリーナ:           ENABLED");
#else
		fprintf(fff, "\n Arena Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n アリーナ:           OFF");
#else
		fprintf(fff, "\n Arena Levels:       OFF");
#endif


	fputc('\n', fff);

	if (p_ptr->noscore)
#ifdef JP
		fprintf(fff, "\n 何か不正なことをしてしまっています。\n");
#else
		fprintf(fff, "\n You have done something illegal.\n");
#endif

	fputc('\n', fff);
}


//v1.1.56 アリーナ戦歴を無効化する。　p_ptr->arena_numberをkourindou_numberに変えるのに邪魔なため
#if 0
static void dump_aux_arena(FILE *fff)
{
	if (lite_town || vanilla_town) return;

	if (p_ptr->arena_number < 0)
	{
		if (p_ptr->arena_number <= ARENA_DEFEATED_OLD_VER)
		{
#ifdef JP
			fprintf(fff, "\n 闘技場: 敗北\n");
#else
			fprintf(fff, "\n Arena: Defeated\n");
#endif
		}
		else
		{
#ifdef JP
			fprintf(fff, "\n 闘技場: %d回戦で%sの前に敗北\n", -p_ptr->arena_number,
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name);
#else
			fprintf(fff, "\n Arena: Defeated by %s in the %d%s fight\n",
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name,
				-p_ptr->arena_number, get_ordinal_number_suffix(-p_ptr->arena_number));
#endif
		}
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS + 2)
	{
#ifdef JP
		fprintf(fff, "\n 闘技場: 真のチャンピオン\n");
#else
		fprintf(fff, "\n Arena: True Champion\n");
#endif
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS - 1)
	{
#ifdef JP
		fprintf(fff, "\n 闘技場: チャンピオン\n");
#else
		fprintf(fff, "\n Arena: Champion\n");
#endif
	}
	else
	{
#ifdef JP
		fprintf(fff, "\n 闘技場: %2d勝\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number));
#else
		fprintf(fff, "\n Arena: %2d Victor%s\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number), (p_ptr->arena_number > 1) ? "ies" : "y");
#endif
	}

	fprintf(fff, "\n");
}
#endif

/*
 *
 */
static void dump_aux_monsters(FILE *fff)
{
	/* Monsters slain */

	int k;
	long uniq_total = 0;
	long norm_total = 0;
	s16b *who;

	/* Sort by monster level */
	u16b why = 2;

#ifdef JP
	fprintf(fff, "\n  [倒したモンスター]\n\n");
#else
	fprintf(fff, "\n  [Defeated Monsters]\n\n");
#endif

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Count monster kills */
	for (k = 1; k < max_r_idx; k++)
	{
		monster_race *r_ptr = &r_info[k];

		/* Ignore unused index */
 		if (!r_ptr->name) continue;

		/* Unique monsters */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			bool dead = (r_ptr->max_num == 0);
			if (dead)
			{
				norm_total++;

				/* Add a unique monster to the list */
				who[uniq_total++] = k;
			}
		}

		/* Normal monsters */
		else
		{
			if (r_ptr->r_pkills > 0)
			{
				norm_total += r_ptr->r_pkills;
			}
		}
	}


	/* No monsters is defeated */
	if (norm_total < 1)
	{
#ifdef JP
		fprintf(fff,"まだ敵を倒していません。\n");
#else
		fprintf(fff,"You have defeated no enemies yet.\n");
#endif
	}

	/* Defeated more than one normal monsters */
	else if (uniq_total == 0)
	{
#ifdef JP
		fprintf(fff,"%ld体の敵を倒しています。\n", norm_total);
#else
		fprintf(fff,"You have defeated %ld %s.\n", norm_total, norm_total == 1 ? "enemy" : "enemies");
#endif
	}

	/* Defeated more than one unique monsters */
	else /* if (uniq_total > 0) */
	{
#ifdef JP
		fprintf(fff, "%ld体のユニーク・モンスターを含む、合計%ld体の敵を倒しています。\n", uniq_total, norm_total);
#else
		fprintf(fff, "You have defeated %ld %s including %ld unique monster%s in total.\n", norm_total, norm_total == 1 ? "enemy" : "enemies", uniq_total, (uniq_total == 1 ? "" : "s"));
#endif


		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array by dungeon depth of monsters */
		ang_sort(who, &why, uniq_total);

#ifdef JP
		fprintf(fff, "\n《上位%ld体のユニーク・モンスター》\n", MIN(uniq_total, 10));
#else
		fprintf(fff, "\n< Unique monsters top %ld >\n", MIN(uniq_total, 10));
#endif

		/* Print top 10 */
		for (k = uniq_total - 1; k >= 0 && k >= uniq_total - 10; k--)
		{
			monster_race *r_ptr = &r_info[who[k]];

#ifdef JP
			fprintf(fff, "  %-50s (レベル%3d)\n", (r_name + r_ptr->name), r_ptr->level);
#else
			fprintf(fff, "  %-40s (level %3d)\n", (r_name + r_ptr->name), r_ptr->level);
#endif
		}

	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);
}


/*
 *
 */
static void dump_aux_race_history(FILE *fff)
{
	///mod150104 野良神様の履歴
	if(p_ptr->prace == RACE_STRAYGOD)
	{
		int rank1=0,rank2=0,rank3=0;
		rank2 = p_ptr->race_multipur_val[4] / 128;

		fprintf(fff, _("\n  [神性履歴]\n",
                        "\n  [Divinity history]"));
		if(rank2)
		{
			rank3 = p_ptr->race_multipur_val[3];
			rank1 = p_ptr->race_multipur_val[4] % 128;
			fprintf(fff, _("\n あなたはかつて%sだった。",
                            "\n You once were %s."), deity_table[rank1].deity_name);
			fprintf(fff, _("\n やがて%sとなり、",
                            "\n Eventually you became %s,"), deity_table[rank2].deity_name);
			fprintf(fff, _("\n そして%sとなった。\n",
                            "\n and then you became %s.\n"), deity_table[rank3].deity_name);
		}
		else if(p_ptr->race_multipur_val[4])
		{
			rank1 = p_ptr->race_multipur_val[4];
			rank2 = p_ptr->race_multipur_val[3];
			fprintf(fff, _("\n あなたはかつて%sだった。",
                            "\n You once were %s."), deity_table[rank1].deity_name);
			fprintf(fff, _("\n その後%sとなった。\n",
                            "\n Then you became %s.\n"), deity_table[rank2].deity_name);
		}
		else
		{
			rank1 = p_ptr->race_multipur_val[3];
			fprintf(fff, _("\n あなたは%sだ。\n", "You are %s."), deity_table[rank1].deity_name);
		}

	}

	else if (p_ptr->old_race1 || p_ptr->old_race2)
	{
		int i;

#ifdef JP
		fprintf(fff, "\n\n あなたは%sとして生まれた。", race_info[p_ptr->start_race].title);
#else
		fprintf(fff, "\n\n You were born as %s.", race_info[p_ptr->start_race].title);
#endif
		for (i = 0; i < MAX_RACES; i++)
		{
			if (p_ptr->start_race == i) continue;
			if (i < 32)
			{
				if (!(p_ptr->old_race1 & 1L << i)) continue;
			}
			else
			{
				if (!(p_ptr->old_race2 & 1L << (i-32))) continue;
			}
#ifdef JP
			fprintf(fff, "\n あなたはかつて%sだった。", race_info[i].title);
#else
			fprintf(fff, "\n You were a %s before.", race_info[i].title);
#endif
		}

		fputc('\n', fff);
	}
	//v1.1.51
	if (nightmare_diary_count && nightmare_total_point)
	{
		int iissune;
		int mult;

		mult = calc_player_score_mult() ;
		//TODO:このスコア倍率*難易度補正の計算もラッパー？とかいうのを作って一箇所にまとめるべきかも
		if (difficulty == DIFFICULTY_EASY) mult /= 10;
		else if (difficulty == DIFFICULTY_HARD) mult = mult * 2;
		else if (difficulty == DIFFICULTY_LUNATIC) mult = mult * 8; //v1.1.53b 7→8
		else if (difficulty == DIFFICULTY_EXTRA) mult = mult * 4;

		if (mult < 1) mult = 1;

		//イイっすねポイント:平均獲得スコア*スコア倍率
		iissune = nightmare_total_point / nightmare_diary_count * mult / 100;
		if (iissune < nightmare_diary_count) iissune = nightmare_diary_count;

		fprintf(fff, _("\n  [悪夢]\n",
                        "\n  [Nightmare]\n"));
		fprintf(fff, _("  合計撃破回数  %d\n",
                        " Total wins     %d\n"), nightmare_diary_win);
		fprintf(fff, _("  合計挑戦回数  %d\n",
                        " Total attempts %d\n"), nightmare_diary_count);
		fprintf(fff, _("  合計評価点    %d\n",
                        " Total points   %d\n"), nightmare_total_point);
		fprintf(fff, _("  イイっすね！  %d\n",
                        " Likes          %d\n"), iissune);

	}

	if(CHECK_FAIRY_TYPE)
	{
		fprintf(fff, _("\n  [能力]\n\n",
                        "\n  [Ability]\n\n"));
		fprintf(fff, "%s\n\n", fairy_table[CHECK_FAIRY_TYPE].fairy_desc);
	}
	if(p_ptr->pclass == CLASS_MOKOU)
	{
		fprintf(fff, _("\n[リザレクション回数] %d回\n",
                        "\n[Times ressurected] %d time(s)\n"), p_ptr->magic_num1[0]);
	}
	if (p_ptr->pclass == CLASS_SHION)
	{
		if(finish_the_game && !p_ptr->magic_num1[2])
			fprintf(fff, _("\n[憑依回数]\n　あなたは一度も憑依を使わずにゲームクリアした！(スコア3倍) \n",
                            "\n[Times used possession]\n  You cleared the game without using possession even once! (Score is tripled) \n"));
		else
			fprintf(fff, _("\n[憑依回数] %d回\n",
                            "\n[Times used possession] %d time(s)\n"), p_ptr->magic_num1[2]);
	}

	if(p_ptr->pclass == CLASS_SOLDIER)
	{
		int i;
		fprintf(fff, _("\n[習得技能] \n", "\n[Acquired Skill] \n"));
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			fprintf(fff, "  %s: %d \n",soldier_skill_table[i][0].name, p_ptr->magic_num2[i]);

		}
	}

	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		int card_num = chimata_calc_card_rank();
		int percent = card_num * 100 / ABILITY_CARD_LIST_LEN;
		fprintf(fff, _("\n  [市場]", "\n  [Marketplace]"));
		fprintf(fff, _("\nあなたは%d種類のアビリティカードを作って流通させた。(達成度%d%%) \n",
                    "You have created %d kinds of ability cards currently in circulation. (completion rate %d%%) \n"), card_num,percent);


	}

	if(p_ptr->pclass == CLASS_KAGUYA)
	{
		fprintf(fff, _("\n  [神宝]",
                        "\n  [Treasures]"));
		if(!kaguya_quest_level || ironman_no_fixed_art)
			fprintf(fff, _("\nあなたは神宝を全く持たずに冒険を開始した。\n",
                            "\nYou started your quest without any of the treasures.\n"));
		else if(kaguya_quest_level >= 5)
			fprintf(fff, _("\nあなたは全ての神宝を持って冒険を開始した。\n",
                            "\nYou started your quest with all of the treasures.\n"));
		else
			fprintf(fff, _("\nあなたは%d個の神宝を持って冒険を開始した。\n",
                            "\nYou started your quest with %d of the treasures.\n"), kaguya_quest_level);
	}
	if(CLASS_IS_PRISM_SISTERS && !p_ptr->magic_num2[75])
	{
		fprintf(fff, _("\nあなたは一度も姉妹と冒険を交代していない。 \n",
                        "\nYou didn't switch with your other sisters during your quest. \n"));
	}

	if(p_ptr->pclass == CLASS_RINGO)
	{
		fprintf(fff, _("\n  [団子消費数]",
                        "\n  [Dango consumed]"));
		if(!p_ptr->magic_num1[0])
			fprintf(fff, _("\nあなたはこの冒険で全く団子を口にしなかった。 \n",
                            "\nYou didn't eat dango during your quest. \n"));
		else if(p_ptr->magic_num1[0] < 10000)
			fprintf(fff, _("\nあなたはこの冒険で%d個の団子を食べた。 \n",
                            "\nYou ate %d dango during your quest. \n"),p_ptr->magic_num1[0]);
		else
			fprintf(fff, _("\nあなたはこの冒険で一万個以上の団子を食べた。 \n",
                            "\nYou ate over ten thousand dango during your quest. \n"));
	}

	if (p_ptr->pclass == CLASS_JYOON)
	{
		//専用性格のときには油の量(石油地形に変えたグリッド数)を表示する
		//さらにツケを残したまま勝利引退した場合専用メッセージ入れよう
		if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
		{
			fprintf(fff, _("\n  [油の量]", "\n  [Oil amount]"));
			if(p_ptr->magic_num1[1])
				fprintf(fff, _("\nあなたはこの冒険で%d%06d リットルもの油を撒き散らした。 \n",
                            "\nYou've excavated %d%06d liters of oil during your adventure. \n"), p_ptr->magic_num1[1], p_ptr->magic_num1[0]);
			else
				fprintf(fff, _("\nあなたはこの冒険で%d リットルの油を撒き散らした。 \n",
                            "\nYou've excavated %d liters of oil during your adventure. \n"), p_ptr->magic_num1[0]);

			if (p_ptr->magic_num1[4]>0 && finish_the_game)
			{
				fprintf(fff, _("\n  [ツケ]", "\n  [Bill]"));
				fprintf(fff, _("\nあなたは$%dのツケの支払いから逃げ切った！\n",
                            "\nYou got away from paying your bill of $%d!\n"), p_ptr->magic_num1[4]);

			}
		}
	    else
        {
            fprintf(fff, _("\n  [散財]", "\n  [Money wasting]"));
            if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1])
            {
                fprintf(fff, _("\nあなたはこの冒険で$1たりとも無駄にしなかった。 \n",
                                "\nYou didn't waste even $1 during your quest. \n"));
            }
            else if (p_ptr->magic_num1[1] > 1000)
            {
                fprintf(fff, _("\nあなたはこの冒険で$%d,%03d,%03d,%03dを散財した。 \n",
                                "\nYou wasted $%d,%03d,%03d,%03d during your quest. \n"), p_ptr->magic_num1[1] / 1000, p_ptr->magic_num1[1] % 1000, p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

            }
            else if (p_ptr->magic_num1[1])
            {
                fprintf(fff, _("\nあなたはこの冒険で$%d,%03d,%03dを散財した。 \n",
                                "\nYou wasted $%d,%03d,%03d during your quest. \n"), p_ptr->magic_num1[1] % 1000, p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

            }
            else if (p_ptr->magic_num1[0] > 1000)
            {
                fprintf(fff, _("\nあなたはこの冒険で$%d,%03dを散財した。 \n",
                                "\nYou wasted $%d,%03d during your quest. \n"), p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

            }
            else
            {
                fprintf(fff, _("\nあなたはこの冒険で$%dを散財した。 \n",
                                "\nYou wasted $%d during your quest. \n"), p_ptr->magic_num1[0] % 1000);

            }
        }



	}

}


static void dump_aux_skills(FILE *fff)
{
	int i,skill_exp;
#ifdef JP
	char skill_name[SKILL_EXP_MAX][12] ={"格闘","盾","乗馬","二刀流","投擲","","","","","","短剣","長剣","刀","鈍器","棒","斧","槍","長柄武器","弓","クロスボウ","銃"};
#else
    char skill_name[SKILL_EXP_MAX][14] ={
	    "Martial Arts",
	    "Shields",
	    "Riding",
	    "Dual Wielding",
	    "Throwing",
	    "","","","","",
	    "Short Blades",
	    "Long Blades",
	    "Katanas",
	    "Blunt Weapons",
	    "Staves",
	    "Axes",
	    "Spears",
	    "Polearms",
	    "Bows",
	    "Crossbows",
	    "Guns"};
#endif

	fprintf(fff, _("\n  [技能一覧]\n",
                   "\n  [Skills List]\n"));
#ifdef JP
	fprintf(fff, "技能の種類    適性    技能レベル\n");
#else
    fprintf(fff, "Skill          Aptitude  Skill Level\n");
#endif

	for (i = 0; i < SKILL_EXP_MAX; i++)
	{
		if(i==5)
		{
			i=9;
			fprintf(fff, "\n");
			continue;
		}
		skill_exp = ref_skill_exp(i);
		fprintf(fff, "%-14s ", skill_name[i]);

		if(cp_ptr->skill_aptitude[i] == 0) fprintf(fff, _("(皆無)", "(n/a)"));
		if(cp_ptr->skill_aptitude[i] == 1) fprintf(fff, "( E )");
		if(cp_ptr->skill_aptitude[i] == 2) fprintf(fff, "( D )");
		if(cp_ptr->skill_aptitude[i] == 3) fprintf(fff, "( C )");
		if(cp_ptr->skill_aptitude[i] == 4) fprintf(fff, "( B )");
		if(cp_ptr->skill_aptitude[i] == 5) fprintf(fff, "( A )");


		fprintf(fff, "     %d",skill_exp / SKILL_LEV_TICK);
		if (skill_exp > cp_ptr->skill_aptitude[i] * SKILL_LEV_TICK * 10) fprintf(fff, _("(成長上限突破)", " (beyond limit)"));
		else if (skill_exp == cp_ptr->skill_aptitude[i] * SKILL_LEV_TICK * 10) fprintf(fff, _("(成長上限)", " (at limit)"));
		else fprintf(fff, " ");

		fprintf(fff, "\n");
	}

}


/*
 *
 */
static void dump_aux_realm_history(FILE *fff)
{
	if (p_ptr->old_realm)
	{
		int i;

		fputc('\n', fff);
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (!(p_ptr->old_realm & 1L << i)) continue;
#ifdef JP
			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				fprintf(fff, "\n ルナサはかつて%s魔法を使えた。", realm_names[i+1]);
			else
				fprintf(fff, "\n あなたはかつて%s魔法を使えた。", realm_names[i+1]);
#else
			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				fprintf(fff, "\n Lunasa was able to use %s magic before.", realm_names[i+1]);
			else
				fprintf(fff, "\n You were able to use %s magic before.", realm_names[i+1]);
#endif
		}

		if (p_ptr->old_realm & 1L << (TV_BOOK_OCCULT-1))
				fprintf(fff, _("\n あなたはかつて秘術を使えた。",
                                "\n You were able to use occult before."));

		fputc('\n', fff);
	}
}


/*
 *
 */
#if 0
static void dump_aux_virtues(FILE *fff)
{
#ifdef JP
	fprintf(fff, "\n\n  [プレイヤーの徳]\n\n");
#else
	fprintf(fff, "\n\n  [Virtues]\n\n");
#endif

#ifdef JP
	fprintf(fff, "属性 : %s\n", your_alignment());
#else
	fprintf(fff, "Your alighnment : %s\n", your_alignment());
#endif

	fprintf(fff, "\n");
	dump_virtues(fff);
}
#endif

/*
 *
 */
static void dump_aux_mutations(FILE *fff)
{
	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
	{
#ifdef JP
		fprintf(fff, "\n\n  [突然変異]\n\n");
#else
		fprintf(fff, "\n\n  [Mutations]\n\n");
#endif

		dump_mutations(fff);
	}
}


/*
 *
 */
static void dump_aux_equipment_inventory(FILE *fff)
{
	int i;
	char o_name[MAX_NLEN];

	/* Dump the equipment */
	if (equip_cnt)
	{
#ifdef JP
		fprintf(fff, "  [キャラクタの装備]\n\n");
#else
		fprintf(fff, "  [Character Equipment]\n\n");
#endif

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{

			///mod140308 弓枠廃止処理関連
			/*:::Mega Hack 弓を両手で持っている時、右手側に強引に（武器を両手持ち）と表示する*/
	//		if ((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute)
			if (((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute
				|| (i == INVEN_RARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_RARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_LARM])
				|| (i == INVEN_LARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_LARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_RARM]))
				&& p_ptr->pclass != CLASS_3_FAIRIES)
				strcpy(o_name, _("(武器を両手持ち)", "(weapon held with both arms)"));
			else if(i == INVEN_RARM)
				object_desc(o_name, &inventory[i], OD_WEAPON_RARM);
			else if(i == INVEN_LARM)
				object_desc(o_name, &inventory[i], OD_WEAPON_LARM);
			else
				object_desc(o_name, &inventory[i], 0);

			fprintf(fff, "%c) %s\n",
				index_to_label(i), o_name);
		}
		fprintf(fff, "\n\n");
	}

	/* Dump the inventory */
#ifdef JP
	fprintf(fff, "  [キャラクタの持ち物]\n\n");
#else
	fprintf(fff, "  [Character Inventory]\n\n");
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Don't dump the empty slots */
		if (!inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(o_name, &inventory[i], 0);
		fprintf(fff, "%c) %s\n", index_to_label(i), o_name);
	}

	/* Add an empty line */
	fprintf(fff, "\n\n");
}

///mod141010
/*:::追加インベントリの中身をダンプに出力する*/
static void dump_aux_inventory2(FILE *fff)
{
	int i;
	char o_name[MAX_NLEN];
	char out_desc[24 + MAX_NLEN];
	int inven2_num = calc_inven2_num();
	object_type *o_ptr;
	int pc = p_ptr->pclass;

	///mod160311 Extraモードでアイテムカードスロットを表示
	if(EXTRA_MODE)
	{
		int slot_max = calc_itemcard_slot_size();
		fprintf(fff, _("  [アイテムカードスロット]\n\n",
                        "  [Item card slots]\n\n"));
		for (i = INVEN_SPECIAL_ITEM_SLOT_START; i <= slot_max; i++)
		{
			o_ptr = &inven_special[i];
			if(!o_ptr->k_idx) strcpy(out_desc, _("(空き)", "(empty)"));
			else sprintf(out_desc,_("『%s』(%d)", "'%s' (%d)"),support_item_list[o_ptr->pval].name,o_ptr->number);
			fprintf(fff, "%c) %s\n",index_to_label(i-1), out_desc);
		}
		fprintf(fff, "\n\n");
	}

	//v1.1.79 新種族「座敷わらし」の移動屋敷
	if (p_ptr->prace == RACE_ZASHIKIWARASHI)
	{
		o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		fprintf(fff, _("  [移動式屋敷]\n\n", "  [Mobile household]\n\n"));
		object_desc(o_name, o_ptr, OD_NAME_AND_ENCHANT);
		fprintf(fff, "  %s\n\n\n", o_name);
	}

	//追加インベントリのない職は終了
	if(!inven2_num) return;

	//v2.0.4 魅須丸は勾玉特技リストのときに表示するのでここでは不要
	if (p_ptr->pclass == CLASS_MISUMARU)
		return;

	switch(pc)
	{
	case CLASS_ORIN:
		fprintf(fff, _("  [猫車の中身]\n\n",
                        "  [In cart]\n\n"));break;
	case CLASS_ENGINEER:
		fprintf(fff, _("  [バックパックの武装]\n\n",
                        "  [Backpack equipment]\n\n"));break;
	case CLASS_NITORI:
		fprintf(fff, _("  [リュックの中]\n\n",
                        "  [In rucksack]\n\n"));break;
	case CLASS_SH_DEALER:
		fprintf(fff, _("  [道具箱の中身]\n\n",
                        "  [In toolbox]\n\n"));break;
	case CLASS_SUMIREKO:
		fprintf(fff, _("  [周囲に浮いているもの]\n\n",
                        "  [Surrounding flying items]\n\n"));break;
	case CLASS_ALICE:
		fprintf(fff, _("  [人形の装備品]\n\n",
                        "  [Doll equipment]\n\n"));break;
	case CLASS_JYOON:
		fprintf(fff, _("  [指輪]\n\n",
                        "  [Rings]\n\n")); break;
	case CLASS_SAKUYA:
		fprintf(fff, _("  [ナイフホルダー]\n\n",
                        "  [Knife holder]\n\n"));break;
	case CLASS_CHEMIST:
		fprintf(fff, _("  [薬箱の中]\n\n",
                        "  [In medicine box]\n\n"));break;
	case CLASS_TAKANE:
	case CLASS_CARD_DEALER:
	case CLASS_SANNYO:
	case CLASS_MIKE:
		fprintf(fff, _("  [カードケースの中]\n\n",
                        "  [In card case]\n\n")); break;
	default:
		fprintf(fff, _("  [追加インベントリ]\n\n",
                        "  [Extra inventory]\n\n"));break;
	}

	/*:::追加インベントリ表示　常にすべての欄*/
	for (i = 0; i < inven2_num; i++)
	{
		o_ptr = &inven_add[i];

		if(!o_ptr->k_idx) strcpy(out_desc, _("(空き)", "(empty)"));
		else if(pc == CLASS_ENGINEER || pc == CLASS_NITORI)
		{
			object_desc(o_name, o_ptr, 0);
			if(o_ptr->xtra4) sprintf(out_desc,_("%s(故障中)", "%s (out of order)"),o_name);
			else	sprintf(out_desc,"%s(%d/%d)",o_name,o_ptr->pval,machine_table[o_ptr->sval].charge);
		}
		else if(pc == CLASS_ALICE)
		{
			object_desc(o_name, o_ptr, 0);
			sprintf(out_desc,"%15s:%s",alice_doll_name[i],o_name);
		}
		else if (pc == CLASS_JYOON)
		{
			object_desc(o_name, o_ptr, 0);
			sprintf(out_desc, "%15s:%s", jyoon_inven2_finger_name[i], o_name);
		}
		else object_desc(out_desc, o_ptr, 0);

		fprintf(fff, "%c) %s\n",index_to_label(i), out_desc);

	}

	/* Add an empty line */
	fprintf(fff, "\n\n");


}


/*
 *
 */
static void dump_aux_home_museum(FILE *fff)
{
	char o_name[MAX_NLEN];
	store_type  *st_ptr;

	/* Do we need it?? */
	/* process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x); */

	/* Print the home */
	st_ptr = &town[1].store[STORE_HOME];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [我が家のアイテム]\n");
#else
		fprintf(fff, "  [Home Inventory]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if ((i % 12) == 0)
#ifdef JP
				fprintf(fff, "\n ( %d ページ )\n", x++);
#else
				fprintf(fff, "\n ( page %d )\n", x++);
#endif
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}


	/* Print the home */
	st_ptr = &town[1].store[STORE_MUSEUM];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [エントランスに飾ったアイテム]\n");
		//fprintf(fff, "  [博物館のアイテム]\n");
#else
		fprintf(fff, "  [Entrance Decorations]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
		if ((i % 12) == 0) fprintf(fff, "\n ( %d ページ )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
#else
		if ((i % 12) == 0) fprintf(fff, "\n ( page %d )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
#endif

		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}
}


/*
 * Output the character dump to a file
 */
/*:::ダンプファイル出力*/
///system
errr make_character_dump(FILE *fff)
{
    int test = 0; //テストするとき1にする

#ifdef JP
	fprintf(fff, "  [幻想蛮怒 %d.%d.%dT キャラクタ情報]\n\n",
		H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);
#else
	fprintf(fff, "  [Gensouband %d.%d.%dT character dump]\n\n",
		H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);
#endif

	update_playtime();

	dump_aux_display_player(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("display_player..");
	dump_aux_last_message(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("last_message..");
	dump_aux_options(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("options..");
	dump_aux_recall(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("recall..");
	dump_aux_quest(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("quest..");
	//dump_aux_arena(fff);
	dump_aux_monsters(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("monsters..");
	//dump_aux_virtues(fff);
	dump_aux_race_history(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("race his..");
	dump_aux_realm_history(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("realm his..");
	dump_aux_class_special(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("class_special..");
	dump_aux_mutations(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	dump_aux_skills(fff);
	if (test) { msg_format("%d", test++);	inkey(); }

	//msg_print("muta..");
	dump_aux_pet(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("pet..");
	fputs("\n\n", fff);
	dump_aux_equipment_inventory(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	dump_aux_inventory2(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("Inven..");
	dump_aux_home_museum(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("home..");

#ifdef JP
	fprintf(fff, "  [チェックサム: \"%s\"]\n\n", get_check_sum());
#else
	fprintf(fff, "  [Check Sum: \"%s\"]\n\n", get_check_sum());
#endif
	//msg_print("chesksum..");

	return 0;
}

/*
 * Hack -- Dump a character description file
 *
 * XXX XXX XXX Allow the "full" flag to dump additional info,
 * and trigger its usage from various places in the code.
 */
/*:::指定したファイル名でダンプを出力する*/
errr file_character(cptr name)
{
	int		fd = -1;
	FILE		*fff = NULL;
	char		buf[1024];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		//v1.1.56 念の為160→1280
		char out_val[1280];

		/* Close the file */
		(void)fd_close(fd);

		/* Build query */
#ifdef JP
		(void)sprintf(out_val, "現存するファイル %s に上書きしますか? ", buf);
#else
		(void)sprintf(out_val, "Replace existing file %s? ", buf);
#endif


		/* Ask */
		if (get_check_strict(out_val, CHECK_NO_HISTORY)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* Invalid file */
	if (!fff)
	{
		/* Message */
#ifdef JP
		prt("キャラクタ情報のファイルへの書き出しに失敗しました！", 0, 0);
#else
		prt("Character dump failed!", 0, 0);
#endif

		(void)inkey();

		/* Error */
		return (-1);
	}

	(void)make_character_dump(fff);

	/* Close it */
	my_fclose(fff);


	/* Message */
#ifdef JP
	//v1.1.28 死亡済みならメッセージ出さない(スコアサーバのログに出ないように)
	if(!p_ptr->is_dead)
		msg_print("キャラクタ情報のファイルへの書き出しに成功しました。");
#else
	if(!p_ptr->is_dead)
		msg_print("Character dump successful.");
#endif

	msg_print(NULL);

	/* Success */
	return (0);
}


/*
 * Display single line of on-line help file
 *
 * You can insert some special color tag to change text color.
 * Such as...
 * WHITETEXT [[[[y|SOME TEXT WHICH IS DISPLAYED IN YELLOW| WHITETEXT
 *
 * A colored segment is between "[[[[y|" and the last "|".
 * You can use any single character in place of the "|".
 */
static void show_file_aux_line(cptr str, int cy, cptr shower)
{
	static const char tag_str[] = "[[[[";
	byte color = TERM_WHITE;
	char in_tag = '\0';
	int cx = 0;
	int i;
	char lcstr[1024];

	if (shower)
	{
		/* Make a lower case version of str for searching */
		strcpy(lcstr, str);
		str_tolower(lcstr);
	}

	/* Initial cursor position */
	Term_gotoxy(cx, cy);

	for (i = 0; str[i];)
	{
		int len = strlen(&str[i]);
		int showercol = len + 1;
		int bracketcol = len + 1;
		int endcol = len;
		cptr ptr;

		/* Search for a shower string in the line */
		if (shower)
		{
			ptr = my_strstr(&lcstr[i], shower);
			if (ptr) showercol = ptr - &lcstr[i];
		}

		/* Search for a color segment tag */
		ptr = in_tag ? my_strchr(&str[i], in_tag) : my_strstr(&str[i], tag_str);
		if (ptr) bracketcol = ptr - &str[i];

		/* A color tag is found */
		if (bracketcol < endcol) endcol = bracketcol;

		/* The shower string is found before the color tag */
		if (showercol < endcol) endcol = showercol;

		/* Print a segment of the line */
		Term_addstr(endcol, color, &str[i]);
		cx += endcol;
		i += endcol;

		/* Shower string? */
		if (endcol == showercol)
		{
			int showerlen = strlen(shower);

			/* Print the shower string in yellow */
			Term_addstr(showerlen, TERM_YELLOW, &str[i]);
			cx += showerlen;
			i += showerlen;
		}

		/* Colored segment? */
		else if (endcol == bracketcol)
		{
			if (in_tag)
			{
				/* Found the end of colored segment */
				i++;

				/* Now looking for an another tag_str */
				in_tag = '\0';

				/* Set back to the default color */
				color = TERM_WHITE;
			}
			else
			{
				/* Found a tag_str, and get a tag color */
				i += sizeof(tag_str)-1;

				/* Get tag color */
				color = color_char_to_attr(str[i]);

				/* Illegal color tag */
				if (color == 255 || str[i+1] == '\0')
				{
					/* Illegal color tag */
					color = TERM_WHITE;

					/* Print the broken tag as a string */
					Term_addstr(-1, TERM_WHITE, tag_str);
					cx += sizeof(tag_str)-1;
				}
				else
				{
					/* Skip the color tag */
					i++;

					/* Now looking for a close tag */
					in_tag = str[i];

					/* Skip the close-tag-indicator */
					i++;
				}
			}
		}

	} /* for (i = 0; str[i];) */

	/* Clear rest of line */
	Term_erase(cx, cy, 255);
}


/*
 * Recursive file perusal.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * Return FALSE on 'q' to exit from a deep, otherwise TRUE.
 */
/*:::指定したファイルを開き表示する。詳細未読*/
bool show_file(bool show_version, cptr name, cptr what, int line, int mode)
{
	int i, n, skey;

	/* Number of "real" lines passed by */
	int next = 0;

	/* Number of "real" lines in the file */
	int size = 0;

	/* Backup value for "line" */
	int back = 0;

	/* This screen has sub-screens */
	bool menu = FALSE;

	/* Current help file */
	FILE *fff = NULL;

	/* Find this string (if any) */
	cptr find = NULL;

	/* Jump to this tag */
	cptr tag = NULL;

	/* Hold strings to find/show */
	char finder_str[81];
	char shower_str[81];
	char back_str[81];

	/* String to show */
	cptr shower = NULL;

	/* Filename */
	char filename[1024];

	/* Describe this thing */
	char caption[128];

	/* Path buffer */
	char path[1024];

	/* General buffer */
	char buf[1024];

	/* Sub-menu information */
	char hook[68][32];

	bool reverse = (line < 0);

	int wid, hgt, rows;

	Term_get_size(&wid, &hgt);
	rows = hgt - 4;

	/* Wipe finder */
	strcpy(finder_str, "");

	/* Wipe shower */
	strcpy(shower_str, "");

	/* Wipe caption */
	strcpy(caption, "");

	/* Wipe the hooks */
	for (i = 0; i < 68; i++)
	{
		hook[i][0] = '\0';
	}

	/* Copy the filename */
	strcpy(filename, name);

	n = strlen(filename);

	/* Extract the tag from the filename */
	for (i = 0; i < n; i++)
	{
		if (filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack XXX XXX XXX */
	if (what)
	{
		/* Caption */
		strcpy(caption, what);

		/* Access the "file" */
		strcpy(path, name);

		/* Open */
		fff = my_fopen(path, "r");
	}

	/* Look in "help" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "ヘルプ・ファイル'%s'", name);
#else
		sprintf(caption, "Help file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_HELP, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "スポイラー・ファイル'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_INFO, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR, name);

		for (i = 0; path[i]; i++)
			if ('\\' == path[i])
				path[i] = PATH_SEP[0];

		/* Caption */
#ifdef JP
sprintf(caption, "スポイラー・ファイル'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Oops */
	if (!fff)
	{
		/* Message */
#ifdef JP
msg_format("'%s'をオープンできません。", name);
#else
		msg_format("Cannot open '%s'.", name);
#endif

		msg_print(NULL);

		/* Oops */
		return (TRUE);
	}


	/* Pre-Parse the file */
	while (TRUE)
	{
		char *str = buf;

		/* Read a line or stop */
		if (my_fgets(fff, buf, sizeof(buf))) break;

		/* XXX Parse "menu" items */
		if (prefix(str, "***** "))
		{
			/* Notice "menu" requests */
			if ((str[6] == '[') && isalpha(str[7]))
			{
				/* Extract the menu item */
				int k = str[7] - 'A';

				/* This is a menu file */
				menu = TRUE;

				if ((str[8] == ']') && (str[9] == ' '))
				{
					/* Extract the menu item */
					strncpy(hook[k], str + 10, 31);

					/* Make sure it's null-terminated */
					hook[k][31] = '\0';
				}
			}
			/* Notice "tag" requests */
			else if (str[6] == '<')
			{
				size_t len = strlen(str);

				if (str[len - 1] == '>')
				{
					str[len - 1] = '\0';
					if (tag && streq(str + 7, tag)) line = next;
				}
			}

			/* Skip this */
			continue;
		}

		/* Count the "real" lines */
		next++;
	}

	/* Save the number of "real" lines */
	size = next;

	/* start from bottom when reverse mode */
	if (line == -1) line = ((size-1)/rows)*rows;

	/* Clear screen */
	Term_clear();

	/* Display the file */
	while (TRUE)
	{
		/* Restart when necessary */
		if (line >= size - rows) line = size - rows;
		if (line < 0) line = 0;

		/* Re-open the file if needed */
		if (next > line)
		{
			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			/* Oops */
			if (!fff) return (FALSE);

			/* File has been restarted */
			next = 0;
		}

		/* Goto the selected line */
		while (next < line)
		{
			/* Get a line */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Skip tags/links */
			if (prefix(buf, "***** ")) continue;

			/* Count the lines */
			next++;
		}

		/* Dump the next 20, or rows, lines of the file */
		for (i = 0; i < rows; )
		{
			cptr str = buf;

			/* Hack -- track the "first" line */
			if (!i) line = next;

			/* Get a line of the file or stop */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Hack -- skip "special" lines */
			if (prefix(buf, "***** ")) continue;

			/* Count the "real" lines */
			next++;

			/* Hack -- keep searching */
			if (find && !i)
			{
				char lc_buf[1024];

				/* Make a lower case version of str for searching */
				strcpy(lc_buf, str);
				str_tolower(lc_buf);

				if (!my_strstr(lc_buf, find)) continue;
			}

			/* Hack -- stop searching */
			find = NULL;

			/* Dump the line */
			show_file_aux_line(str, i + 2, shower);

			/* Count the printed lines */
			i++;
		}

		while (i < rows)
		{
			/* Clear rest of line */
			Term_erase(0, i + 2, 255);

			i++;
		}

		/* Hack -- failed search */
		if (find)
		{
			bell();
			line = back;
			find = NULL;
			continue;
		}


		/* Show a general "title" */
		if (show_version)
		{
			prt(format(
#ifdef JP
				"[幻想蛮怒 %d.%d.%dT, %s, %d/%d]",
#else
				"[Gensouband %d.%d.%d, %s, Line %d/%d]",
#endif

			   H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH,
			   caption, line, size), 0, 0);
		}
		else
		{
			prt(format(
#ifdef JP
				"[%s, %d/%d]",
#else
				"[%s, Line %d/%d]",
#endif
				caption, line, size), 0, 0);
		}

		/* Prompt -- small files */
		if (size <= rows)
		{
			/* Wait for it */
#ifdef JP
prt("[キー:(?)ヘルプ (ESC)終了]", hgt - 1, 0);
#else
			prt("[Press ESC to exit.]", hgt - 1, 0);
#endif

		}

		/* Prompt -- large files */
		else
		{
#ifdef JP
			if(reverse)
				prt("[キー:(RET/スペース)↑ (-)↓ (?)ヘルプ (ESC)終了]", hgt - 1, 0);
			else
				prt("[キー:(RET/スペース)↓ (-)↑ (?)ヘルプ (ESC)終了]", hgt - 1, 0);
#else
			prt("[Press Return, Space, -, =, /, |, or ESC to exit.]", hgt - 1, 0);
#endif
		}

		/* Get a special key code */
		skey = inkey_special(TRUE);

		switch (skey)
		{
		/* Show the help for the help */
		case '?':
			/* Hack - prevent silly recursion */
#ifdef JP
			if (strcmp(name, "jhelpinfo.txt") != 0)
				show_file(TRUE, "jhelpinfo.txt", NULL, 0, mode);
#else
			if (strcmp(name, "helpinfo.txt") != 0)
				show_file(TRUE, "helpinfo.txt", NULL, 0, mode);
#endif
			break;

		/* Hack -- try showing */
		case '=':
			/* Get "shower" */
#ifdef JP
			prt("強調: ", hgt - 1, 0);
#else
			prt("Show: ", hgt - 1, 0);
#endif

			strcpy(back_str, shower_str);
			if (askfor(shower_str, 80))
			{
				if (shower_str[0])
				{
					/* Make it lowercase */
					str_tolower(shower_str);

					/* Show it */
					shower = shower_str;
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(shower_str, back_str);
			break;

		/* Hack -- try finding */
		case '/':
		case KTRL('s'):
			/* Get "finder" */
#ifdef JP
			prt("検索: ", hgt - 1, 0);
#else
			prt("Find: ", hgt - 1, 0);
#endif

			strcpy(back_str, finder_str);
			if (askfor(finder_str, 80))
			{
				if (finder_str[0])
				{
					/* Find it */
					find = finder_str;
					back = line;
					line = line + 1;

					/* Make finder lowercase */
					str_tolower(finder_str);

					/* Show it */
					shower = finder_str;
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(finder_str, back_str);
			break;

		/* Hack -- go to a specific line */
		case '#':
			{
				char tmp[81];
#ifdef JP
				prt("行: ", hgt - 1, 0);
#else
				prt("Goto Line: ", hgt - 1, 0);
#endif

				strcpy(tmp, "0");

				if (askfor(tmp, 80)) line = atoi(tmp);
			}
			break;

		/* Hack -- go to the top line */
		case SKEY_TOP:
			line = 0;
			break;

		/* Hack -- go to the bottom line */
		case SKEY_BOTTOM:
			line = ((size - 1) / rows) * rows;
			break;

		/* Hack -- go to a specific file */
		case '%':
			{
				char tmp[81];
#ifdef JP
				prt("ファイル・ネーム: ", hgt - 1, 0);
				strcpy(tmp, "jhelp.hlp");
#else
				prt("Goto File: ", hgt - 1, 0);
				strcpy(tmp, "help.hlp");
#endif

				if (askfor(tmp, 80))
				{
					if (!show_file(TRUE, tmp, NULL, 0, mode)) skey = 'q';
				}
			}
			break;

		/* Allow backing up */
		case '-':
			line = line + (reverse ? rows : -rows);
			if (line < 0) line = 0;
			break;

		/* One page up */
		case SKEY_PGUP:
			line = line - rows;
			if (line < 0) line = 0;
			break;

		/* Advance a single line */
		case '\n':
		case '\r':
			line = line + (reverse ? -1 : 1);
			if (line < 0) line = 0;
			break;

		/* Move up / down */
		case '8':
		case SKEY_UP:
			line--;
			if (line < 0) line = 0;
			break;

		case '2':
		case SKEY_DOWN:
			line++;
			break;

		/* Advance one page */
		case ' ':
			line = line + (reverse ? -rows : rows);
			if (line < 0) line = 0;
			break;

		/* One page down */
		case SKEY_PGDOWN:
			line = line + rows;
			break;
		}

		/* Recurse on numbers */
		if (menu)
		{
			int key = -1;

			if (!(skey & SKEY_MASK) && isalpha(skey))
				key = skey - 'A';

			if ((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if (!show_file(TRUE, hook[key], NULL, 0, mode))
					skey = 'q';
			}
		}

		/* Hack, dump to file */
		if (skey == '|')
		{
			FILE *ffp;
			char buff[1024];
			char xtmp[162];

			strcpy (xtmp, "");

#ifdef JP
			if (!get_string("ファイル名: ", xtmp, 80)) continue;
#else
			if (!get_string("File name: ", xtmp, 80)) continue;
#endif

			/* Close it */
			my_fclose(fff);

			/* Build the filename */
			path_build(buff, sizeof(buff), ANGBAND_DIR_USER, xtmp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			ffp = my_fopen(buff, "w");

			/* Oops */
			if (!(fff && ffp))
			{
#ifdef JP
				msg_print("ファイルを開けません。");
#else
				msg_print("Failed to open file.");
#endif
				skey = ESCAPE;
				break;
			}

			sprintf(xtmp, "%s: %s", player_name, what ? what : caption);
			my_fputs(ffp, xtmp, 80);
			my_fputs(ffp, "\n", 80);

			while (!my_fgets(fff, buff, sizeof(buff)))
				my_fputs(ffp, buff, 80);

			/* Close it */
			my_fclose(fff);
			my_fclose(ffp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");
		}

		/* Return to last screen */
		if ((skey == ESCAPE) || (skey == '<')) break;

		/* Exit on the ^q */
		if (skey == KTRL('q')) skey = 'q';

		/* Exit on the q key */
		if (skey == 'q') break;
	}

	/* Close the file */
	my_fclose(fff);

	/* Escape */
	if (skey == 'q') return (FALSE);

	/* Normal return */
	return (TRUE);
}


/*
 * Peruse the On-Line-Help
 */
/*:::ヘルプを読む*/
void do_cmd_help(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the main help file */
#ifdef JP
	///mod140927 ヘルプファイル変更
	//(void)show_file(TRUE, "jhelp.hlp", NULL, 0, 0);
	(void)show_file(TRUE, "thelp.hlp", NULL, 0, 0);
#else
	(void)show_file(TRUE, "thelp_en.hlp", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
/*:::＠の名前に関する機種ごとの可否判定処理、セーブファイルベース名など決定*/
void process_player_name(bool sf)
{
	int i, k = 0;
	char old_player_base[32] = "";

	if (character_generated) strcpy(old_player_base, player_base);

	/* Cannot be too long */
#if defined(MACINTOSH) || defined(MSDOS) || defined(USE_EMX) || defined(AMIGA) || defined(ACORN) || defined(VM)
#ifdef MSDOS
	if (strlen(player_name) > 8)
#else
	if (strlen(player_name) > 15)
#endif
	{
		/* Name too long */
#ifdef JP
quit_fmt("'%s'という名前は長すぎます！", player_name);
#else
		quit_fmt("The name '%s' is too long!", player_name);
#endif

	}
#endif

	/* Cannot contain "icky" characters */
	for (i = 0; player_name[i]; i++)
	{
		/* No control characters */
#ifdef JP
		if (iskanji(player_name[i])){i++;continue;}
		if (iscntrl( (unsigned char)player_name[i]))
#else
		if (iscntrl(player_name[i]))
#endif

		{
			/* Illegal characters */
#ifdef JP
quit_fmt("'%s' という名前は不正なコントロールコードを含んでいます。", player_name);
#else
			quit_fmt("The name '%s' contains control chars!", player_name);
#endif

		}
	}


#ifdef MACINTOSH

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif


		/* Convert "dot" to "underscore" */
		if (c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif

		/* Accept some letters */
#ifdef JP
		if(iskanji(c)){
		  if(k + 2 >= sizeof(player_base) || !player_name[i+1]) break;
		  player_base[k++] = c;
		  i++;
		  player_base[k++] = player_name[i];
		}
#ifdef SJIS
		else if (iskana(c)) player_base[k++] = c;
#endif
		else
#endif
		/* Convert path separator to underscore */
		if (!strncmp(PATH_SEP, player_name+i, strlen(PATH_SEP))){
			player_base[k++] = '_';
			i += strlen(PATH_SEP);
		}
		/* Convert some characters to underscore */
#ifdef MSDOS
		else if (my_strchr(" \"*+,./:;<=>?[\\]|", c)) player_base[k++] = '_';
#elif defined(WINDOWS)
		else if (my_strchr("\"*,/:;<>?\\|", c)) player_base[k++] = '_';
#endif
		else if (isprint(c)) player_base[k++] = c;
	}

#endif


#if defined(MSDOS)

	/* Hack -- max length */
	if (k > 8) k = 8;

#endif

	/* Terminate */
	player_base[k] = '\0';

	/* Require a "base" name */
	if (!player_base[0]) strcpy(player_base, "PLAYER");


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif
	if (!savefile_base[0] && savefile[0])
	{
		cptr s;
		s = savefile;
		while (1)
		{
			cptr t;
			t = my_strstr(s, PATH_SEP);
			if (!t)
				break;
			s = t+1;
		}
		strcpy(savefile_base, s);
	}

	if (!savefile_base[0] || !savefile[0])
		sf = TRUE;

	///mod140316 セーブファイル名を＠名からの自動設定でなく自分で指定するようにした。ユニークプレイヤーの実装で同名セーブファイル上書きが起こりやすくなったため。
	/*:::セーブファイル名を指定する。今の環境ではここにはキャラメイク時にしか入らないはず。*/
	///
	/// Added a precompiler option for to bring back original behaviour; apparently this function can get
	/// called upon program launch before character creation on non-Windows platforms (including .live server)
#ifndef DONT_ASK_FOR_SAVEFILE_NAME
	if (sf)
	{
		bool flag_err;
		while(1)
		{
			FILE *fp;
			char temp[128];

			flag_err = FALSE;
			strcpy(temp, player_base);
#ifdef JP
			get_string("セーブファイル名を入力してください: ", temp, 30);
#else
            get_string("Enter savefile name: ", temp, 30);
#endif
			if(!temp[0]) strcpy(temp, player_base);
			/*:::入力したセーブファイルにwindowsで使えない文字があったら_に変換する。正直ここ自信ない。*/
			for (i = 0; temp[i]; i++)
			{
				unsigned char c = temp[i];
#ifdef JP
				if(iskanji(c))
				{
					i++;
					continue;
				}
				else if(iskana(c)) continue;
				else if (my_strchr("\"*,/:;<>?\\|", c))
				{
					flag_err = TRUE;
					temp[i] = '_';
				}
#else
                if (my_strchr("\"*,/:;<>?\\|", c))
				{
					flag_err = TRUE;
					temp[i] = '_';
				}
#endif
			}
			strcpy(savefile_base, temp);
			path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
			/*:::セーブファイルを開いてみて開けたら同じファイルが存在*/
			fp = my_fopen(savefile, "r");
			if(fp)
			{
				my_fclose(fp);
#ifdef JP
				msg_format("セーブファイルがすでに存在します。");
				if (get_check_strict("上書きしますか？：", CHECK_OKAY_CANCEL))break;
				else continue;
#else
                msg_format("Savefile already exists.");
				if (get_check_strict("Overwrite?:", CHECK_OKAY_CANCEL))break;
				else continue;
#endif
			}
			break;
		}
		if(flag_err)
		{
#ifdef JP
			msg_format("Windowsのファイル名に使えない文字が'_'に変換されました。");
#else
            msg_format("Characters that can't be used in Windows filenames were changed to '_'.");
#endif
			inkey();
		}
	}


//#if 0
#else
	/*:::上の変更がミスだったときのためのバックアップ:::*/
	/* Change the savefile name */
	if (sf)
	{
		char temp[128];

		strcpy(savefile_base, player_base);

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		(void)sprintf(temp, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		(void)sprintf(temp, "%s", player_base);
#endif

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		(void)sprintf(temp, "%s.sv", player_base);
#endif /* VM */

		/* Build the filename */
		path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
	}
	/*:::バックアップここまで:::*/
#endif

	/* Load an autopick preference file */
	if (character_generated)
	{
		if (!streq(old_player_base, player_base)) autopick_load_pref(FALSE);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_player(0)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?  XXX XXX
 *
 * What a horrible name for a global function.  XXX XXX XXX
 */
/*:::＠の名前を変更する　Cコマンドかキャラクターメイキング時に呼ばれる*/
void get_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("キャラクターの名前を入力して下さい: ", tmp, 30))
//	if (get_string("キャラクターの名前を入力して下さい: ", tmp, 15))
#else
	if (get_string("Enter a name for your character: ", tmp, 30))
#endif
	{
		/* Use the name */
		strcpy(player_name, tmp);
	}

	if (0 == strlen(player_name))
	{
		/* Use default name */
		strcpy(player_name, "PLAYER");
	}

	/*:::画面上部の「性格+名前」の部分を書き換える*/
	if(p_ptr->psex == SEX_MALE)
		strcpy(tmp,ap_ptr->title);
	else
		strcpy(tmp,ap_ptr->f_title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"の");
#else
	strcat(tmp, " ");
#endif
	strcat(tmp,player_name);

	/* Re-Draw the name (in light blue) */
	//Term_erase(34, 1, 255);
	Term_erase(14, 1, 255);
	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);

	/* Erase the prompt, etc */
	clear_from(22);
}

/*:::＠の職業名を変更する　ユニークプレイヤーのときCコマンドから使える。
 *:::あるいは職業普通人の時キャラメイクで使える。*/
void get_class_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_class_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("キャラクターの職業名を再設定できます: ", tmp, 17))
//	if (get_string("キャラクターの名前を入力して下さい: ", tmp, 15))
#else
    if (get_string("Change the name of your character class: ", tmp, 17))
#endif
	{
		/* Use the name */
		strcpy(player_class_name, tmp);
	}

	if (0 == strlen(player_class_name))
	{
		/* Use default name */
		if(p_ptr->psex == SEX_MALE ) strcpy(player_class_name, cp_ptr->title);
		else  strcpy(player_class_name, cp_ptr->f_title);
	}

}

/*
 * Hack -- commit suicide
 */
/*:::引退・自殺*/
///system
void do_cmd_suicide(void)
{
	int i;

	/* Flush input */
	flush();

	//v1.1.51
	if (p_ptr->inside_arena)
	{
#ifdef JP
		msg_print("夢の中でそのような決断を下すべきではない。");
#else
        msg_print("You shouldn't make such decision in your dreams.");
#endif
		return;
	}

	/* Verify Retirement */
	if (p_ptr->total_winner)
	{
		/* Verify */
#ifdef JP
	if (p_ptr->pclass == CLASS_OUTSIDER && !get_check_strict("幻想郷に永住しますか? ", CHECK_NO_HISTORY)) return;
	if (p_ptr->pclass != CLASS_OUTSIDER && !get_check_strict("この冒険を終えますか? ", CHECK_NO_HISTORY)) return;
#else
    if (p_ptr->pclass == CLASS_OUTSIDER && !get_check_strict("Do you want to take residence in Gensoukyou? ", CHECK_NO_HISTORY)) return;
	if (p_ptr->pclass != CLASS_OUTSIDER && !get_check_strict("Do you want to retire? ", CHECK_NO_HISTORY)) return;
#endif

	}

	/* Verify Suicide */
	else
	{
		/* Verify */
#ifdef JP
		if (!get_check("この冒険を放棄しますか？")) return;
		///msg131221 dead
		//if (!get_check("本当に自殺しますか？")) return;
#else
		if (!get_check("Do you want to abandon your quest? ")) return;
#endif
	}


	if (!p_ptr->noscore)
	{
		/* Special Verification for suicide */
#ifdef JP
prt("確認のため '@' を押して下さい。", 0, 0);
#else
		prt("Please verify by typing the '@' sign: ", 0, 0);
#endif

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;

		if(p_ptr->total_winner) finish_the_game = TRUE;

		else 		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_GAMEOVER);//v1.1.58

	}

	/* Initialize "last message" buffer */
	/*:::断末魔や勝利メッセージが空でなければ初期化・・？*/
	if (p_ptr->last_message) string_free(p_ptr->last_message);
	p_ptr->last_message = NULL;

	/* Hack -- Note *winning* message */
	/*:::last_words:遺言オプション*/
	if (p_ptr->total_winner && last_words)
	{
		char buf[1024] = "";

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_WINNER); //v1.1.58

		do
		{
#ifdef JP
			while (!get_string("*勝利*メッセージ: ", buf, sizeof buf)) ;
#else
			while (!get_string("*Winning* message: ", buf, sizeof buf)) ;
#endif
		}
#ifdef JP
		while (!get_check_strict("よろしいですか？", CHECK_NO_HISTORY));
#else
		while (!get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
#endif

		if (buf[0])
		{
			p_ptr->last_message = string_make(buf);
			msg_print(p_ptr->last_message);
		}
	}

	/* Stop playing */
	p_ptr->playing = FALSE;

	/* Kill the player */
	p_ptr->is_dead = TRUE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	if (!p_ptr->total_winner)
	{
#ifdef JP
		///msg131221 dead
		//do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "ダンジョンの探索に絶望して自殺した。");
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "今回のゲームを放棄した。");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ゲームオーバー --------");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "abandoned their adventure");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	}

	/* Cause of death */
#ifdef JP
(void)strcpy(p_ptr->died_from, "途中終了");
#else
	(void)strcpy(p_ptr->died_from, "Quitting");
#endif

}



/*
 * Save the game
 */
/*:::セーブする*/
void do_cmd_save_game(int is_autosave)
{
	/* Autosaves do not disturb */
	if (is_autosave)
	{
#ifdef JP
msg_print("自動セーブ中");
#else
		msg_print("Autosaving the game...");
#endif

	}
	else
	{
		/* Disturb the player */
		disturb(1, 1);
	}

	/* Clear messages */
	msg_print(NULL);

	/* Handle stuff */
	handle_stuff();

	/* Message */
#ifdef JP
prt("ゲームをセーブしています...", 0, 0);
#else
	prt("Saving game...", 0, 0);
#endif


	/* Refresh */
	Term_fresh();

	/* The player is not dead */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(セーブ)");
#else
	(void)strcpy(p_ptr->died_from, "(saved)");
#endif


	/* Forbid suspend */
	/*:::何もしてない？*/
	signals_ignore_tstp();

	/* Save the player */
	if (save_player())
	{
#ifdef JP
prt("ゲームをセーブしています... 終了", 0, 0);
#else
		prt("Saving game... done.", 0, 0);
#endif

	}

	/* Save failed (oops) */
	else
	{
#ifdef JP
prt("ゲームをセーブしています... 失敗！", 0, 0);
#else
		prt("Saving game... failed!", 0, 0);
#endif

		if (!p_ptr->playing && !p_ptr->is_dead)
		{
			Term_clear();
#ifdef JP
			prt("大変申し訳ありません。セーブに失敗しました。", 5, 5);
			prt("強制的に人里に戻して再セーブを試みます。", 6, 5);
			prt("念のため続行前にsaveフォルダのアクセス権などを見直して下さい。", 7, 5);
			prt("userフォルダに緊急ダンプが出力されます。", 8, 5);
			prt("バグ発見のために作者に送っていただけると幸いです。", 9, 5);
			prt("何かキーを押すと続行します。", 12, 5);
#else
			prt("Sorry, failed to save.", 5, 5);
			prt("Forcibly returning to the remote area and trying to save again.", 6, 5);
			prt("Just in case, please review the permissions on the save folder before ", 7, 5);
			prt("continuing.", 8, 5);
			prt("An emergency dump will be written to the user folder.  I would", 8, 5);
			prt("appriate it if you could send it to the author.", 9, 5);
			prt("Press any key to continue.", 12, 5);
#endif
			(void)inkey();
			(void)file_character("heng_katte_panic_dump.txt");
			p_ptr->word_recall = 0;
			p_ptr->alter_reality = 0;
			dun_level = 0;
			p_ptr->inside_quest = 0;
			dungeon_type = 0;
			p_ptr->floor_id = 0;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->riding = 0;
			wipe_m_list();
			change_floor();
			if(save_player()) prt(_("再セーブに成功。", "Successful resave."), 14, 5);
			else prt(_("再セーブ失敗..", "Resave failed."), 14, 5);
			(void)inkey();
		}
	}

#ifdef NEW_PLAYER_LOG
	if (save_player_log(savefile, p_log_ptr))
	{
		prt("Saving player log... done.", 0, 0);
	}
	else
	{
		prt("Saving player log... failed!", 0, 0);
	}
#endif

	/* Allow suspend again */
	signals_handle_tstp();

	/* Refresh */
	Term_fresh();

	/* Note that the player is not dead */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(元気に生きている)");
#else
	(void)strcpy(p_ptr->died_from, "(alive and well)");
#endif

	/* HACK -- don't get sanity blast on updating view */
	hack_mind = FALSE;

	/* Update stuff */
	update_stuff();

	/* Initialize monster process */
	mproc_init();

	/* HACK -- reset the hackish flag */
	hack_mind = TRUE;
}


/*
 * Save the game and exit
 */
/*:::セーブして終了　ここではフラグを入れるだけ*/
void do_cmd_save_and_exit(void)
{
	p_ptr->playing = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----ゲーム中断----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Save and Exit Game ----");
#endif
}


/*:::スコア計算　新*/
u32b total_points_new(bool test)
{
	u32b point, point_h=0, point_l=0;
	s16b max_dl = 0;
	int i,j,mult;
	int mult_race, mult_days;
	int value_tmp;
	int value_items = 0, value_items_max = 0;
	int questpoint=0,monpoint=0,turnbonus=0,floorpoint=0;
	object_type		*o_ptr;
	store_type *st_ptr_tmp;

	//v1.1.53b スコア計算に使うターン数を「ゲームターン」でなく「ダンジョンターン」にした。
	//この値は広域マップで移動したときの一歩を1ターン相当としてカウントするもの。
	//Extraが広域マップに行く必要がなく日数ボーナスを稼ぎやすいので公平性を期すため変更する。
	int score_turn = dungeon_turn;

	//この値はプログラム立ち上げ中ずっと保持する。セーブはしない。
	static int value_items_museum=0, value_items_max_museum=0;


	//最新到達階層によりボーナス
	for (i = 0; i < max_d_idx; i++)	if(max_dlv[i] > max_dl)	max_dl = max_dlv[i];

	if(difficulty == DIFFICULTY_EASY) floorpoint = max_dl * max_dl * 3;
	else if(difficulty == DIFFICULTY_NORMAL) floorpoint = max_dl * max_dl * 5;
	else if(difficulty == DIFFICULTY_HARD) floorpoint = max_dl * max_dl * 10;
	else if(difficulty == DIFFICULTY_EXTRA) floorpoint = max_dl * max_dl * 30;
	else if(difficulty == DIFFICULTY_LUNATIC) floorpoint = max_dl * max_dl * 50;

	point_l += floorpoint;
if(test) msg_format("FLOOR_BONUS:%d",floorpoint);


	//経験値/10(上限1000000)
	point_l += MIN(1000000,p_ptr->max_exp / 10);
if(test) msg_format("EXP(MAX1000k):%d",p_ptr->max_exp / 10);

	st_ptr_tmp = &town[1].store[STORE_HOME];
	for (j = 0; j < st_ptr_tmp->stock_num; j++)
	{
		o_ptr = &st_ptr_tmp->stock[j];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);

	}

	//性格「蒐集家」は博物館のアイテムをスコアに計算しない(スコアサーバ実装でボツになったが)
	if(p_ptr->pseikaku == SEIKAKU_COLLECTOR)
	{
		value_items_museum=0;
		value_items_max_museum=0;
		hack_flag_calc_museum_score = FALSE;
	}
	///mod160326 博物館にたくさんアイテムがあると計算が重そうなので特定のフラグが立っている時しか計算しないことにした
	//is_deadの判定はなくてもいいはずだが将来変なバグ起こして最終ダンプがおかしくならないように一応つけとく
	else if(p_ptr->is_dead || hack_flag_calc_museum_score)
	{
		value_items_museum=0;
		value_items_max_museum=0;
		st_ptr_tmp = &town[1].store[STORE_MUSEUM];
		for (j = 0; j < st_ptr_tmp->stock_num; j++)
		{
			o_ptr = &st_ptr_tmp->stock[j];
			if (!o_ptr->k_idx) continue;
			if(finish_the_game) value_tmp = object_value_real(o_ptr);
			else value_tmp = object_value(o_ptr);
			if((value_items_museum + value_items)<10000000) value_items_museum += value_tmp * o_ptr->number;
			value_items_max_museum = MAX(value_items_max_museum, value_tmp);
		}
		hack_flag_calc_museum_score = FALSE;
	}
	value_items += value_items_museum;
	value_items_max = MAX(value_items_max, value_items_max_museum);

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);
	}
	//追加インベントリの中身のスコア計算
	for (i = 0; i < INVEN_ADD_MAX; i++)
	{
		o_ptr = &inven_add[i];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);
	}

	if(value_items<10000000) value_items += p_ptr->au;

	//v1.1.92 女苑(専用性格)がツケを未精算の場合計算から差っ引く
	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && p_ptr->magic_num1[4])
	{
		value_items -= p_ptr->magic_num1[4];
	}

	//v1.1.47 大借金抱えてプレイ開始する＠のために下限追加
	if (value_items < 0) value_items = 0;
	//所持金+所持品の価値/10(上限1000000)
	point_l += MIN(1000000,value_items / 10);
	//最も高価な所持品の価値(上限1000000)
	point_l += MIN(1000000,value_items_max);
if(test) msg_format("ITEMS(MAX1000k):%d",value_items / 10);
if(test) msg_format("SOUVENIR(MAX1000k):%d",value_items_max);

	//完了したクエスト Lev^2*10(基本)
	for (i = 1; i < max_quests; i++)
	{
		///mod150504 Hack - 藍は紫クエストを達成済みだがスコアなどには参入しない
		if((p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI) && i == QUEST_YUKARI) continue;

		if(quest[i].status == QUEST_STATUS_COMPLETED || quest[i].status == QUEST_STATUS_FINISHED)
		{
			//Extraモードではクエストが少ないのでボーナス二倍
			questpoint += quest[i].level * quest[i].level * (60 - quest[i].complev) * (EXTRA_MODE?2:1);
			//特定クエストにボーナス
			if(i == QUEST_YUKARI)	questpoint += 100000;
			if(i == QUEST_TAISAI)	questpoint += 500000;
			if(i == QUEST_SERPENT)	questpoint += 1000000;

		}
	}

	point_l += questpoint;
if(test) msg_format("QUEST:%d",questpoint);

	//打倒ユニークモンスター Lev^2 アンバーの王族に+10000
	for(i=1;i<max_r_idx;i++)
	{
		monster_race *r_ptr = &r_info[i];

		if(!(r_ptr->flags1 & RF1_UNIQUE) || !r_ptr->r_akills) continue;

		monpoint += r_ptr->level * r_ptr->level;

		//旧ラスボスのオベロンを倒すとスコアが大幅に上がる。EXTRAでは困難なのでさらにボーナス
		if (i == MON_OBERON)
		{
			if(EXTRA_MODE) monpoint += 600000;
			else monpoint += 300000;
		}
		else if ((r_ptr->flags3 & RF3_ANG_AMBER) && r_ptr->r_akills) monpoint += 10000;
	}

	///mod160326 UNIQUE2打倒時もユニークと同様にスコア算出
	monpoint += score_bonus_kill_unique2;

if(test) msg_format("UNIQUE:%d",monpoint);
	point_l += monpoint;

	//オベロン倒して引退するときターンに応じてボーナス(キャノンボウラーはボーナス多い)
	if(finish_the_game && (quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED))
	{
		if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 30) turnbonus += (TURNS_PER_TICK * TOWN_DAWN * 30 - score_turn);
		//v1.1.53b 難易度LUNATICのときはどうせ反感同然なので何の性格でも同様のボーナスを得られることにした
		if(p_ptr->pseikaku == SEIKAKU_SEXY || (difficulty == DIFFICULTY_LUNATIC))
		{
			if(EXTRA_MODE) turnbonus *= 4;
			else turnbonus *= 2;
		}

		if(p_ptr->pclass == CLASS_SAKUYA) turnbonus /= 2;

if(test) msg_format("TURNBONUS:%d",turnbonus);
		point_l += turnbonus;
	}



	//種族職業倍率適用
	//v1.1.42 一部を別関数に分離
	mult = calc_player_score_mult() * 100;

	//v1.1.48 紫苑が一度も憑依を使わずゲームクリアしたらスコア3倍にしてしまう
	if (p_ptr->pclass == CLASS_SHION)
	{
		if (finish_the_game && !p_ptr->magic_num1[2]) mult *= 3;
	}


/*
	mult_race = race_info[p_ptr->prace].score_mult;
if(test) msg_format("mult_race_start:%d",mult_race);
	for (i = 0; i < MAX_RACES; i++)
	{
		if (i < 32)
			{
				if (!(p_ptr->old_race1 & 1L << i) || mult_race < race_info[i].score_mult) continue;
			}
			else
			{
				if (!(p_ptr->old_race2 & 1L << (i-32)) || mult_race < race_info[i].score_mult) continue;
			}
		mult_race = race_info[i].score_mult;
	}
if(test) msg_format("mult_race_total:%d",mult_race);

	mult = mult * mult_race / 100;
	mult = mult * class_info[p_ptr->pclass].score_mult / 100;
*/

	//難易度、オプションなど
	if(difficulty == DIFFICULTY_EASY) mult /= 10;
	else if(difficulty == DIFFICULTY_HARD) mult = mult * 2;
	else if(difficulty == DIFFICULTY_LUNATIC) mult = mult * 8; //v1.1.53b 7→8
	else if(difficulty == DIFFICULTY_EXTRA) mult = mult * 4;

	if(EXTRA_MODE)
	{
		if(ironman_empty_levels) mult = mult * 6 / 5;
	 	if(ironman_small_levels) mult = mult * 4 / 3;
		if(ironman_rooms) mult = mult * 4 / 3;
		if(ironman_shops) mult = mult * 4 / 3;
		else if(!powerup_home) mult = mult * 12 / 10;

		extra_mode_score_mult(&mult,p_ptr->pclass);//EXTRA専用のクラス別補正
	}
	else
	{
		if(ironman_empty_levels) mult = mult * 11 / 10;
		if(ironman_small_levels) mult = mult * 11 / 10;
		if(ironman_rooms) mult = mult * 4 / 5;
		if(ironman_shops) mult = mult * 3 / 2;
		else if(!powerup_home) mult = mult * 12 / 10;
	}

	if (ironman_no_bonus)  mult = mult * 3 / 2;
	else if (birth_bonus_10)  mult = mult * 6 / 5;
	else if (birth_bonus_20) mult = mult * 21 / 20;


	if(ironman_no_fixed_art)
	{
		if(p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_JEWELER) mult = mult * 11 / 10;
		else 	mult = mult * 13 / 10;
	}
	else if(!preserve_mode)
	{
		if(EXTRA_MODE)
			mult = mult * 11 / 10;
		else
			mult = mult * 12 / 10;
	}

	if(!no_capture_book) mult = mult * 21 / 20;


	//日数経過でボーナスかペナルティ
	//30日以内なら一日1%ずつボーナス
	if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 30)
	{
		mult_days = (TURNS_PER_TICK * TOWN_DAWN * 30 - score_turn) / (TURNS_PER_TICK * TOWN_DAWN) + 1;
		if(p_ptr->pclass == CLASS_SAKUYA) mult_days /= 2;
	}
	//60日以内ならペナルティなし
	else if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 60)
	{
		mult_days = 0;
	}
	//それ以降は6日経過毎に1%ペナルティ(360日の-50%で下げ止まる)
	else
	{
		mult_days = -1 - (score_turn - TURNS_PER_TICK * TOWN_DAWN * 60) / (TURNS_PER_TICK * TOWN_DAWN) / 4;
		if(mult_days < -50) mult_days = -50;
	}
	mult = mult * (100 + mult_days) / 100;

	if(p_ptr->pseikaku == SEIKAKU_MUNCHKIN) mult /= 100;
	//else if(p_ptr->pseikaku == SEIKAKU_BERSERK) mult /= 2; v1.1.42別関数に分離

	if(finish_the_game) mult *= 2;

	//v1.1.32 クラス専用処理 ちょっと記法変更
	//v1.1.42 別関数に分離
	/*
	switch(p_ptr->pclass)
	{
	case CLASS_MOKOU:
		//妹紅はリザレクション回数でスコア減少
		if( p_ptr->magic_num1[0] ) mult /= (1 + MIN(99,p_ptr->magic_num1[0]));
		break;
	case CLASS_KAGUYA:
		if(kaguya_quest_level && !ironman_no_fixed_art)
		{
			//輝夜は初期所持神宝の数でスコア減少
			for(i=0;i<kaguya_quest_level;i++) mult /= 2;
		}
		break;
	case CLASS_CIRNO:
		//v1.1.32 チルノの性格「日焼けした」はスコア半分
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mult /= 2;
		break;
	case CLASS_LUNASA:
	case CLASS_MERLIN:
	case CLASS_LYRICA:
		//プリズムリバー姉妹は一度も交代しないとスコア30%アップ
		if(!p_ptr->magic_num2[75]) mult = mult * 13 / 10;
		break;
	case CLASS_NARUMI:
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			mult = mult * 3 / 4;
		break;
	}
	*/

	if(finish_the_game && streq(p_ptr->died_from, "hourai"))
	{
		mult = mult * 4 / 3;
	}

	mult /= 100;

	if(mult < 1) mult = 1;
if(test) msg_format("mult:%d",mult);

	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;
if(test) msg_format("point_h:%d",point_h);

	//v1.1.53c スコア上限を32bit上限値に拡張
	//if(point_h > 15257) point = 999999999;//paranoir
	if(point_h > 65535) point = 0xFFFFFFFFL;
	else point = (point_h << 16) + (point_l);

if(test) msg_format("TOTAL_SCORE:%lu",point);

	return point;

}


/*
 * Hack -- Calculates the total number of points earned		-JWT-
 */
///sys スコアを計算する　重要
//v1.1.56 無効化
#if 0
long total_points(void)
{
	int i, mult = 100;
	s16b max_dl = 0;
	u32b point, point_h, point_l;
	int arena_win = MIN(p_ptr->arena_number, MAX_ARENA_MONS);

	if (!preserve_mode) mult += 10;
	if (!autoroller) mult += 10;
	if (!smart_learn) mult -= 20;
	if (smart_cheat) mult += 30;
	if (ironman_shops) mult += 50;
	if (ironman_small_levels) mult += 10;
	if (ironman_empty_levels) mult += 20;
	if (!powerup_home) mult += 50;
	if (ironman_rooms) mult += 100;
	if (ironman_nightmare) mult += 100;

	if (mult < 5) mult = 5;

	for (i = 0; i < max_d_idx; i++)
		if(max_dlv[i] > max_dl)
			max_dl = max_dlv[i];

	point_l = (p_ptr->max_max_exp + (100 * max_dl));
	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;

	point = (point_h << 16) + (point_l);
	if (p_ptr->arena_number >= 0)
		point += (arena_win * arena_win * (arena_win > 29 ? 1000 : 100));

	if (ironman_downward) point *= 2;
	/*
	if (p_ptr->pclass == CLASS_BERSERKER)
	{
		if ( p_ptr->prace == RACE_SPECTRE )
			point = point / 5;
	}
	*/

	if ((p_ptr->pseikaku == SEIKAKU_MUNCHKIN) && point)
	{
		point = 1;
		if (p_ptr->total_winner) point = 2;
	}
	if (easy_band) point = (0 - point);

	return point;
}
#endif

#ifdef JP
#define GRAVE_LINE_WIDTH 31
#else
#define GRAVE_LINE_WIDTH 64
#endif

/*
 * Centers a string within a GRAVE_LINE_WIDTH character string		-JWT-
 */
static void center_string(char *buf, cptr str)
{
	int i, j;

	/* Total length */
	i = strlen(str);

	/* Necessary border */
	j = GRAVE_LINE_WIDTH / 2 - i / 2;

	/* Mega-Hack */
	(void)sprintf(buf, "%*s%s%*s", j, "", str, GRAVE_LINE_WIDTH - i - j, "");
}


#if 0
/*
 * Save a "bones" file for a dead character
 *
 * Note that we will not use these files until Angband 2.8.0, and
 * then we will only use the name and level on which death occured.
 *
 * Should probably attempt some form of locking...
 */
static void make_bones(void)
{
	FILE                *fp;

	char                str[1024];


	/* Ignore wizards and borgs */
	if (!(p_ptr->noscore & 0x00FF))
	{
		/* Ignore people who die in town */
		if (dun_level)
		{
			char tmp[128];

			/* XXX XXX XXX "Bones" name */
			sprintf(tmp, "bone.%03d", dun_level);

			/* Build the filename */
			path_build(str, sizeof(str), ANGBAND_DIR_BONE, tmp);

			/* Attempt to open the bones file */
			fp = my_fopen(str, "r");

			/* Close it right away */
			if (fp) my_fclose(fp);

			/* Do not over-write a previous ghost */
			if (fp) return;

			/* File type is "TEXT" */
			FILE_TYPE(FILE_TYPE_TEXT);

			/* Grab permissions */
			safe_setuid_grab();

			/* Try to write a new "Bones File" */
			fp = my_fopen(str, "w");

			/* Drop permissions */
			safe_setuid_drop();

			/* Not allowed to write it?  Weird. */
			if (!fp) return;

			/* Save the info */
			fprintf(fp, "%s\n", player_name);
			fprintf(fp, "%d\n", p_ptr->mhp);
			fprintf(fp, "%d\n", p_ptr->prace);
			fprintf(fp, "%d\n", p_ptr->pclass);

			/* Close and save the Bones file */
			my_fclose(fp);
		}
	}
}
#endif


/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux)(void) = NULL;


/*
 * Display a "tomb-stone"
 */
/*:::墓石、死因などを表示する部分*/
///msg131221 死にかかわるメッセージを書き換えた
static void print_tomb(void)
{
	bool done = FALSE;

#if 0
	/* Do we use a special tombstone ? */
	if (tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux)();
	}
#endif
	/* Print the text-tombstone */
	if (!done)
	{
		cptr   p;
		char   tmp[160];
		char   buf[1024];
		char   dummy[80];
		char   *t;
	//	FILE   *fp;
		time_t ct = time((time_t)0);
		int    extra_line = 0;

		/* Clear screen */
		Term_clear();
///mod140118 墓石削除
#if 0
		/* Build the filename */
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead_j.txt");
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead.txt");
#endif

		/* Open the News file */
		fp = my_fopen(buf, "r");

		/* Dump */
		if (fp)
		{
			int i = 0;

			/* Dump the file to the screen */
			while (0 == my_fgets(fp, buf, sizeof(buf)))
			{
				/* Display and advance */
				put_str(buf, i++, 0);
			}

			/* Close */
			my_fclose(fp);
		}
#endif

		/* King or Queen */
		///sys 勝利メッセージ　難易度ごとに「ハードローグライカー」とか入れようか
		if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
		{
#ifdef JP
			/* 英日切り替え */
			//p= "偉大なる者";
			if(difficulty == DIFFICULTY_EASY) p= "イージーローグライカー";
			else if(difficulty == DIFFICULTY_NORMAL) p= "ノーマルローグライカー";
			else if(difficulty == DIFFICULTY_HARD) p= "ハードローグライカー";
			else if(difficulty == DIFFICULTY_LUNATIC) p= "ルナティックローグライカー";
			else  p= "エクストラローグライカー";
		center_string(buf, p);
		put_str(buf, 8, 11);

#else
			if(difficulty == DIFFICULTY_EASY) p= "Easy Rogueliker";
			else if(difficulty == DIFFICULTY_NORMAL) p= "Normal Rogueliker";
			else if(difficulty == DIFFICULTY_HARD) p= "Hard Rogueliker";
			else if(difficulty == DIFFICULTY_LUNATIC) p= "Lunatic Rogueliker";
			else  p= "Extra Rogueliker";
            center_string(buf, p);
            put_str(buf, 8, 11);
#endif
		}

		/* Normal */
		/*
		else
		{
			p =  player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5];
		}
		*/


		center_string(buf, format("SCORE:%lu",(unsigned long)total_points_new(FALSE)));
		put_str(buf, 4, 11);

		center_string(buf, player_name);
		put_str(buf, 6, 11);


#ifndef JP
		center_string(buf, "the");
		put_str(buf, 7, 11);
#endif

		//center_string(buf, p);
		//put_str(buf, 8, 11);

		//center_string(buf, cp_ptr->title);
		///mod140206 職業名変更対応
		center_string(buf, player_class_name);
		put_str(buf, 10, 11);

#ifdef JP
		(void)sprintf(tmp, "レベル: %d", (int)p_ptr->lev);
#else
		(void)sprintf(tmp, "Level: %d", (int)p_ptr->lev);
#endif
		center_string(buf, tmp);
		put_str(buf, 11, 11);

#ifdef JP
		(void)sprintf(tmp, "経験値: %ld", (long)p_ptr->exp);
#else
		(void)sprintf(tmp, "Exp: %ld", (long)p_ptr->exp);
#endif
		center_string(buf, tmp);
		put_str(buf, 12, 11);

#ifdef JP
		(void)sprintf(tmp, "所持金: %ld", (long)p_ptr->au);
#else
		(void)sprintf(tmp, "AU: %ld", (long)p_ptr->au);
#endif
		center_string(buf, tmp);
		put_str(buf, 13, 11);

		/* 墓に刻む言葉をオリジナルより細かく表示 */
		if (streq(p_ptr->died_from, _("途中終了", "Quitting")))
		{
			///msg131221
			strcpy(tmp, _("<放棄>", "<Abandoned>"));
		}
		else if (streq(p_ptr->died_from, "ripe") && p_ptr->pclass == CLASS_OUTSIDER)
		{
			strcpy(tmp, _("幻想郷に永住した。", "Took residence in Gensoukyou."));
		}
		else if (streq(p_ptr->died_from, "triumph"))
		{
			strcpy(tmp, _("幻想郷の英雄となったのち、外界へと帰還した。",
                            "Became a hero of Gensoukyou and returned to the Outside world."));
		}
		else if(streq(p_ptr->died_from, "hourai"))
		{
			sprintf(tmp, _("永遠の命を手にした。",
                            "Obtained eternal life."));
		}
		else if (streq(p_ptr->died_from, "true_loser"))
		{
			sprintf(tmp, _("自分を含めて全てに敗北をもたらした。",
                            "Brought defeat upon everyone, herself included."));
		}
		else if(streq(p_ptr->died_from, "mission_failed"))
		{
			sprintf(tmp, _("姫を守りきれなかった。",
                            "Failed to protect the princess."));
		}
		else if (streq(p_ptr->died_from, "escape"))
		{
			strcpy(tmp, _("幻想郷から脱出した。",
                            "Escaped from Gensoukyou."));
		}
		else if (streq(p_ptr->died_from, "lost"))
		{
			strcpy(tmp, _("<消滅>", "<Disappeared>"));
		}
		else if (streq(p_ptr->died_from, "ripe"))
		{
			strcpy(tmp, _("＊＊＊ゲームクリア＊＊＊",
                            "*******GAME CLEAR*******"));
		}
		else if (streq(p_ptr->died_from, "Seppuku"))
		{
			strcpy(tmp, _("勝利の後、切腹", "Committed seppuku after victory"));
		}
		else
		{
			roff_to_buf(p_ptr->died_from, GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
#ifdef JP
			t = tmp + strlen(tmp) + 1;
			if (*t)
			{
				strcpy(dummy, t); /* 2nd line */
				if (*(t + strlen(t) + 1)) /* Does 3rd line exist? */
				{
					for (t = dummy + strlen(dummy) - 2; iskanji(*(t - 1)); t--) /* Loop */;
					strcpy(t, _("…", "..."));
				}
				else if (my_strstr(tmp, "『") && suffix(dummy, "』"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "『");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				else if (my_strstr(tmp, "「") && suffix(dummy, "」"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "「");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				center_string(buf, dummy);
				put_str(buf, 15, 11);
				extra_line = 1;
			}
#endif
		}
		center_string(buf, tmp);
#ifdef JP
		put_str(buf, 14, 11);
#else
        put_str(buf, 16, 11);
#endif

		if (!streq(p_ptr->died_from, "ripe") && !streq(p_ptr->died_from, "true_loser") && !streq(p_ptr->died_from, "Seppuku") && !streq(p_ptr->died_from, "escape") && !streq(p_ptr->died_from, "triumph") && !streq(p_ptr->died_from, "lost") && !streq(p_ptr->died_from, "hourai")&& !streq(p_ptr->died_from, "mission_failed") )
		{
			if (dun_level == 0)
			{
				cptr locate_desc = p_ptr->town_num ? _("街", "town") : _("荒野", "wilderness");
				if (streq(p_ptr->died_from, _("途中終了", "Quitting")))
				{
					sprintf(tmp, _("%sで倒れた", "Collapsed in %s"), locate_desc);
				}
				else
				{
					sprintf(tmp, _("に%sで倒された", "Defeated in %s by "), locate_desc);
				}
			}
			else
			{
				if (streq(p_ptr->died_from, _("途中終了", "Quitting")))
				{
					sprintf(tmp, _("地下 %d 階で倒れた", "Collapsed on dungeon level %d"), dun_level);
				}
				else
				{
					sprintf(tmp, _("に地下 %d 階で倒された", "Defeated on dungeon level %d by "), dun_level);
				}
			}
			center_string(buf, tmp);
#ifdef JP
			put_str(buf, 15 + extra_line, 11);
#else
            put_str(buf, 15, 11);
#endif
		}

		(void)sprintf(tmp, "%-.24s", ctime(&ct));
		center_string(buf, tmp);
		put_str(buf, 17, 11);

#ifdef JP
		msg_format("さようなら、%s!", player_name);
#else
		msg_format("Goodbye, %s!", player_name);
#endif
	}
}


/*
 * Display some character info
 */
/*:::ダンプを保存、または画面に表示する*/
static void show_info(void)
{
	int             i, j, k, l;
	object_type		*o_ptr;
	store_type		*st_ptr;

	int save_check_num = 100;

	/* Hack -- Know everything in the inven/equip */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
	}

	for (i = 1; i < max_towns; i++)
	{
		st_ptr = &town[i].store[STORE_HOME];

		/* Hack -- Know everything in the home */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			o_ptr = &st_ptr->stock[j];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Aware and Known */
			object_aware(o_ptr);
			object_known(o_ptr);
		}
	}

	/* Hack -- Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Flush all input keys */
	flush();

	/* Flush messages */
	msg_print(NULL);


	/* Describe options */
///mod150403 ファイルダンプは自動的に行われることにした。
	prt(_("キャラクターの記録がファイルに書き出されます。",
            "Character dump will be created."), 20, 0);
//	prt("（注：勝手版なのでスコア送信は行われません。）", 22, 0);

#if 0
	prt("キャラクターの記録をファイルに書き出すことができます。", 20, 0);
	prt("リターンキーでキャラクターを見ます。ESCで中断します。", 21, 0);
	/* Dump character records as requested */
	while (TRUE)
	{
		char out_val[160];

		/* Prompt */
#ifdef JP
put_str("ファイルネーム: ", 23, 0);
#else
		put_str("Filename: ", 23, 0);
#endif


		/* Default */
		strcpy(out_val, "");

		/* Ask for filename (or abort) */
		if (!askfor(out_val, 60)) return;

		/* Return means "show on screen" */
		if (!out_val[0]) break;

		/* Save screen */
		screen_save();

		/* Dump a character file */
		(void)file_character(out_val);

		/* Load screen */
		screen_load();
	}
#endif

	//v1.1.56 テストプレイしていたときになぜか異常終了したので念の為ループに回数制限
	while (save_check_num--)
	{
		char out_val[160];

		strcpy(out_val, "");
		put_str(_("ファイル名: ", "Filename: "), 23, 0);
		askfor(out_val, 80);

		if(!strlen(out_val))
		{
			if(auto_dump) strcpy(out_val, format("autodump_%s.txt",player_name));
			else break;
		}

		screen_save();
		//file_characterは0が返った時成功
		if(!file_character(out_val)) break;
		screen_load();



	}
	screen_load();

	if (!save_check_num)
	{
		prt(_("ERROR:キャラクターダンプに100回失敗しました",
                "ERROR: Character dump failed 100 times"), 20, 20);
	}

	update_playtime();

	/* Display player */
	display_player(0);

	/* Prompt for inventory */
#ifdef JP
prt("何かキーを押すとさらに情報が続きます (ESCで中断): ", 23, 0);
#else
	prt("Hit any key to see more information (ESC to abort): ", 23, 0);
#endif


	/* Allow abort at this point */
	if (inkey() == ESCAPE) return;


	/* Show equipment and inventory */

	/* Equipment -- if any */
	if (equip_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_equip(0);
#ifdef JP
prt("装備していたアイテム: -続く-", 0, 0);
#else
		prt("You are using: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Inventory -- if any */
	if (inven_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_inven(0);
#ifdef JP
prt("持っていたアイテム: -続く-", 0, 0);
#else
		prt("You are carrying: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Homes in the different towns */
	for (l = 1; l < max_towns; l++)
	{
		st_ptr = &town[l].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
			/* Display contents of the home */
			for (k = 0, i = 0; i < st_ptr->stock_num; k++)
			{
				/* Clear screen */
				Term_clear();

				/* Show 12 items */
				for (j = 0; (j < 12) && (i < st_ptr->stock_num); j++, i++)
				{
					char o_name[MAX_NLEN];
					char tmp_val[80];

					/* Acquire item */
					o_ptr = &st_ptr->stock[i];

					/* Print header, clear line */
					sprintf(tmp_val, "%c) ", I2A(j));
					prt(tmp_val, j+2, 4);

					/* Display object description */
					object_desc(o_name, o_ptr, 0);
					c_put_str(tval_to_attr[o_ptr->tval], o_name, j+2, 7);
				}

				/* Caption */
#ifdef JP
prt(format("拠点に置いてあったアイテム ( %d ページ): -続く-", k+1), 0, 0);
#else
				prt(format("Your home contains (page %d): -more-", k+1), 0, 0);
#endif


				/* Wait for it */
				if (inkey() == ESCAPE) return;
			}
		}
	}
}

/*:::スコアの登録をできるかチェック*/
static bool check_score(void)
{
	/* Clear screen */
	Term_clear();


#ifdef SCORE_SERVER_TEST
	msg_print("テストサーバにスコアを送ります。");
	return TRUE;
#endif

	/* No score file */
	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("スコア・ファイルが使用できません。");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return FALSE;
	}

#ifndef SCORE_WIZARDS
	/* Wizard-mode pre-empts scoring */
	if (p_ptr->noscore & 0x000F)
	{
#ifdef JP
msg_print("ウィザード・モードではスコアが記録されません。");
#else
		msg_print("Score not registered for wizards.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_BORGS
	/* Borg-mode pre-empts scoring */
	if (p_ptr->noscore & 0x00F0)
	{
#ifdef JP
msg_print("ボーグ・モードではスコアが記録されません。");
#else
		msg_print("Score not registered for borgs.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_CHEATERS
	/* Cheaters are not scored */
	if (p_ptr->noscore & 0xFF00)
	{
#ifdef JP
msg_print("チートフラグが立っているためスコアが記録されません。");
//msg_print("詐欺をやった人はスコアが記録されません。");
#else
		msg_print("Score not registered for cheaters.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

	/* Interupted */
#ifdef JP
if (!p_ptr->total_winner && streq(p_ptr->died_from, "強制終了"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Interrupting"))
#endif

	{
#ifdef JP
msg_print("強制終了のためスコアが記録されません。");
#else
		msg_print("Score not registered due to interruption.");
#endif

		msg_print(NULL);
		return FALSE;
	}

	/* Quitter */
#ifdef JP
if (!p_ptr->total_winner && streq(p_ptr->died_from, "途中終了"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Quitting"))
#endif

	{
#ifdef JP
msg_print("途中終了のためスコアが記録されません。");
#else
		msg_print("Score not registered due to quitting.");
#endif

		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 */
/*:::ゲームの終了処理*/
///\131117 スコア送信削除
void close_game(void)
{
	char buf[1024];
	bool do_send = TRUE;

/*	cptr p = "[i:キャラクタの情報, f:ファイル書き出し, t:スコア, x:*鑑定*, ESC:ゲーム終了]"; */

	/* Handle stuff */
	handle_stuff();

	/* Flush the messages */
	msg_print(NULL);

	/* Flush the input */
	flush();


	/* No suspending now */
	signals_ignore_tstp();


	/* Hack -- Character is now "icky" */
	character_icky = TRUE;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Grab permissions */
	safe_setuid_grab();

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	safe_setuid_drop();

	/* Handle death */
	if (p_ptr->is_dead)
	{
		/* Handle retirement */
		/*:::王冠表示*/
		if (p_ptr->total_winner) kingly();

		/* Save memories */
#ifdef JP
		if (!cheat_save || get_check("ゲームオーバーデータをセーブしますか？ "))
#else
		if (!cheat_save || get_check("Save death? "))
#endif
		{

#ifdef JP
if (!save_player()) msg_print("セーブ失敗！");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		else do_send = FALSE;

		/* You are dead */
		/*:::墓石表示*/
		print_tomb();

		flush();

		/* Show more info */
		/*:::ダンプの保存か表示*/
		show_info();

		/* Clear screen */
		Term_clear();

		/*:::スコア送信の資格があるかチェック*/
		if (check_score())
		{
			/*:::スコア送信を試みる*/
			if ((!send_world_score(do_send)))
			{
#ifdef JP
				if (get_check_strict("後でスコアを登録するために待機しますか？", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#else
				if (get_check_strict("Stand by for later score registration? ", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#endif
				{
					p_ptr->wait_report_score = TRUE;
					p_ptr->is_dead = FALSE;
#ifdef JP
					if (!save_player()) msg_print("セーブ失敗！");
#else
					if (!save_player()) msg_print("death save failed!");
#endif
				}
			}
			if (!p_ptr->wait_report_score)
				(void)top_twenty();
		}
		else if (highscore_fd >= 0)
		{
			display_scores_aux(0, 10, -1, NULL);
		}
#if 0
		/* Dump bones file */
		make_bones();
#endif
	}

	/* Still alive */
	else
	{
		/* Save the game */
		do_cmd_save_game(FALSE);

		/* Prompt for scores XXX XXX XXX */
#ifdef JP
prt("リターンキーか ESC キーを押して下さい。", 0, 40);
#else
		prt("Press Return (or Escape).", 0, 40);
#endif

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_EXIT);//v1.1.58


		/* Predict score (or ESCAPE) */
		if (inkey() != ESCAPE) predict_score();
	}


	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Kill all temporal files */
	clear_saved_floor_files();

	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
	/* If nothing important has happened, just quit */
#ifdef JP
	if (!character_generated || character_saved) quit("緊急事態");
#else
	if (!character_generated || character_saved) quit("panic");
#endif

	/* Mega-Hack -- see "msg_print()" */
	msg_flag = FALSE;

	/* Clear the top line */
	prt("", 0, 0);

	/* Hack -- turn off some things */
	disturb(1, 1);

	/* Mega-Hack -- Delay death */
	if (p_ptr->chp < 0) p_ptr->is_dead = FALSE;

	/* Hardcode panic save */
	p_ptr->panic_save = 1;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(緊急セーブ)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Panic save, or get worried */
#ifdef JP
	if (!save_player()) quit("緊急セーブ失敗！");
#else
	if (!save_player()) quit("panic save failed!");
#endif


	/* Successful panic save */
#ifdef JP
	quit("緊急セーブ成功！");
#else
	quit("panic save succeeded!");
#endif
}


/*
 * Get a random line from a file
 * Based on the monster speech patch by Matt Graham,
 */
/*:::☆生成から来たとき：file_name: a_high_j.txtなど　entry:artifact_bias値 *output:生成後ファイル名 */
/*:::file_name:読まれるファイル名
 *:::entry:関連するインデックス名
 *:::読み込んだファイルに「N:数字」の行があり、entryとその数字が一致すればその行から次の空白行までにある行の内容からランダムに一行が選ばれるらしい*/
/*:::"N:*"の行以降はデフォルト名*/
/*:::成功時、戻り値は0になることに注意*/
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE    *fp;
	char    buf[1024];
	int     counter, test;
	int     line_num = 0;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, file_name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Failed */
	if (!fp) return -1;

	/* Find the entry of the monster */
	while (TRUE)
	{
		/* Get a line from the file */
		if (my_fgets(fp, buf, sizeof(buf)) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for lines starting with 'N:' */
			if ((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				/*:::N:*はデフォルト候補*/
				if (buf[2] == '*')
				{
					/* Default lines */
					break;
				}
				/*:::モンスター関連　entryのモンスターインデックスの敵がmaleのとき*/
				else if (buf[2] == 'M')
				{
					if (r_info[entry].flags1 & RF1_MALE) break;
				}
				/*:::モンスター関連　entryのモンスターインデックスの敵がfemaleのとき*/
				else if (buf[2] == 'F')
				{
					if (r_info[entry].flags1 & RF1_FEMALE) break;
				}
				/* Get the monster number */
				/*:::entry値とN:??の数字が一致したとき*/
				else if (sscanf(&(buf[2]), "%d", &test) != EOF)
				{
					/* Is it the right number? */
					if (test == entry) break;
				}
				else
				{
					/* Error while converting the number */
					msg_format("Error in line %d of %s!", line_num, file_name);
					my_fclose(fp);
					return -1;
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return -1;
		}
	}
	/*:::この時点で、entry値で指定された候補の行が記された部分の先頭にfpが来ているはず*/
	/*:::一行ずつ読み込み、N:と#で始まる行は無視し、空白行かファイル終端までに得られた行のどれかをoutputへ格納*/
	/* Get the random line */
	for (counter = 0; ; counter++)
	{
		while (TRUE)
		{
			test = my_fgets(fp, buf, sizeof(buf));

			/* Count the lines */
			/* line_num++; No more needed */

			//読み込み成功(my_fgetsは成功時0が返る)
			if (!test)
			{
				/* Ignore lines starting with 'N:' */
				if ((buf[0] == 'N') && (buf[1] == ':')) continue;
				/*:::#で始まる行はbreakせず、再びループでmy_fgets()へ*/
				if (buf[0] != '#') break;
			}
			else break;
		}

		/* Abort */
		/*:::空白行かファイル終端で終了？*/
		if (!buf[0]) break;

		/* Copy the line */
		if (one_in_(counter + 1)) strcpy(output, buf);
	}

	/* Close the file */
	my_fclose(fp);

	/* Success */
	return counter ? 0 : -1;
}


#ifdef JP
errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count)
{
	int  i, j, kanji;
	errr result = 1;

	for (i = 0; i < count; i++)
	{
		result = get_rnd_line(file_name, entry, output);
		if (result) break;
		kanji = 0;
		for (j = 0; output[j]; j++) kanji |= iskanji(output[j]);
		if (kanji) break;
	}
	return result;
}
#endif

/*
 * Process file for auto picker/destroyer.
 */
/*:::自動拾いファイルの読み込みをする*/
errr process_autopick_file(cptr name)
{
	char buf[1024];

	errr err = 0;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	err = process_pref_file_aux(buf, PREF_TYPE_AUTOPICK);

	/* Result */
	return (err);
}


/*
 * Process file for player's history editor.
 */
errr process_histpref_file(cptr name)
{
	char buf[1024];
	errr err = 0;
	bool old_character_xtra = character_xtra;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* Hack -- prevent modification birth options in this file */
	character_xtra = TRUE;

	err = process_pref_file_aux(buf, PREF_TYPE_HISTPREF);

	character_xtra = old_character_xtra;

	/* Result */
	return (err);
}

/*:::オープン済みのバイナリファイルのポインタを指定の番地まで進める？*/
///system　＠の年齢設定が使われている。うっかり削除しないように注意。
static errr counts_seek(int fd, u32b where, bool flag)
{
	huge seekpoint;
	char temp1[128], temp2[128];
	u32b zero_header[3] = {0L, 0L, 0L};
	int i;

#ifdef SAVEFILE_USE_UID
	(void)sprintf(temp1, "%d.%s.%d%d%d", player_uid, savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#else
	(void)sprintf(temp1, "%s.%d%d%d", savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#endif
	/*:::charの配列の中身は0で初期化されるらしい。*/
	/*:::temp1[i]のchar値のビット列を適当に反転したりしなかったりしてる？簡易暗号化か？*/
	for (i = 0; temp1[i]; i++)
		temp1[i] ^= (i+1) * 63;
	/*:::面倒になったのでここで読むのやめる。p_ptrなどのビット数を増やした場合あふれないかチェックしないといけないかも*/
	seekpoint = 0;
	while (1)
	{
		if (fd_seek(fd, seekpoint + 3 * sizeof(u32b)))
			return 1;
		if (fd_read(fd, (char*)(temp2), sizeof(temp2)))
		{
			if (!flag)
				return 1;
			/* add new name */
			fd_seek(fd, seekpoint);
			fd_write(fd, (char*)zero_header, 3*sizeof(u32b));
			fd_write(fd, (char*)(temp1), sizeof(temp1));
			break;
		}

		if (strcmp(temp1, temp2) == 0)
			break;

		seekpoint += 128 + 3 * sizeof(u32b);
	}

	return fd_seek(fd, seekpoint + where * sizeof(u32b));
}

/*:::簡易暗号化済みのz_info_j.rawから指定した番地の数値(32bit)を読み込んでいるらしい*/
u32b counts_read(int where)
{
	int fd;
	u32b count = 0;
	char buf[1024];

#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info.raw");
#endif
	fd = fd_open(buf, O_RDONLY);

	if (counts_seek(fd, where, FALSE) ||
	    fd_read(fd, (char*)(&count), sizeof(u32b)))
		count = 0;

	(void)fd_close(fd);

	return count;
}


/*:::z_info_j.rawを開き、暗号化済みの指定番地に32bitの数値を書き込んでいるらしい　たぶん不正監視用*/
errr counts_write(int where, u32b count)
{
	int fd;
	char buf[1024];
	errr err;

#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info.raw");
#endif

	/* Grab permissions */
	/*:::たぶんUnixやLinux用の権限確認　中では何もされていない*/
	safe_setuid_grab();

	/*:::0_RDWRはReadAndWriteのフラグ fdにはファイルデスクリプタ?が格納される*/
	fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	/*:::これもたぶんUnixやLinux用の権限関係　中では何もされていない*/
	safe_setuid_drop();

	/*:::z_info_j.rawが存在しないとき*/
	if (fd < 0)
	{
		/* File type is "DATA" */
		/*:::winでは関係ない？*/
		FILE_TYPE(FILE_TYPE_DATA);

		/* Grab permissions */
		safe_setuid_grab();

		/* Create a new high score file */
		/*:::バイナリファイルz_info_j.rawを作成する。0644は作成者読み書きそれ以外読み込みのみ可のパーミッション*/
		fd = fd_make(buf, 0644);

		/* Drop permissions */
		safe_setuid_drop();
	}

	/* Grab permissions */
	safe_setuid_grab();

	/*:::これもUNIXやLinux専用と思われる*/
	err = fd_lock(fd, F_WRLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return 1;

	/*:::ここ途中までしか読んでない 結局whereとは何なのか　番地みたいなものか*/
	counts_seek(fd, where, TRUE);
	fd_write(fd, (char*)(&count), sizeof(u32b));

	/* Grab permissions */
	safe_setuid_grab();

	err = fd_lock(fd, F_UNLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return 1;

	(void)fd_close(fd);

	return 0;
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	/* Redraw the term */
	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if (p_ptr->is_dead)
	{
		/* Mark the savefile */
#ifdef JP
(void)strcpy(p_ptr->died_from, "強制終了");
#else
		(void)strcpy(p_ptr->died_from, "Abortion");
#endif

		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("強制終了");
#else
		quit("interrupt");
#endif

	}

	/* Allow suicide (after 5) */
	else if (signal_count >= 5)
	{
		/* Cause of "death" */
#ifdef JP
(void)strcpy(p_ptr->died_from, "強制終了中");
#else
		(void)strcpy(p_ptr->died_from, "Interrupting");
#endif


		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Stop playing */
		p_ptr->playing = FALSE;

		/* Suicide */
		p_ptr->is_dead = TRUE;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("強制終了");
#else
		quit("interrupt");
#endif

	}

	/* Give warning (after 4) */
	else if (signal_count >= 4)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);

		/* Clear the top line */
		Term_erase(0, 0, 255);

		/* Display the cause */
#ifdef JP
Term_putstr(0, 0, -1, TERM_WHITE, "熟慮の上の自殺！");
#else
		Term_putstr(0, 0, -1, TERM_WHITE, "Contemplating suicide!");
#endif


		/* Flush */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if (signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	forget_lite();
	forget_view();
	clear_mon_lite();

	/* Clear the bottom line */
	Term_erase(0, hgt - 1, 255);

	/* Give a warning */
	Term_putstr(0, hgt - 1, -1, TERM_RED,
#ifdef JP
"恐ろしいソフトのバグが飛びかかってきた！");
#else
		    "A gruesome software bug LEAPS out at you!");
#endif


	/* Message */
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "緊急セーブ...");
#else
	Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save...");
#endif


#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----ゲーム異常終了----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Panic Save and Abort Game ----");
#endif

	/* Flush output */
	Term_fresh();

	/* Panic Save */
	p_ptr->panic_save = 1;

	/* Panic save */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(緊急セーブ)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Attempt to save */
	if (save_player())
	{
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "緊急セーブ成功！");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save succeeded!");
#endif

	}

	/* Save failed */
	else
	{
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "緊急セーブ失敗！");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save failed!");
#endif

	}

	/* Flush output */
	Term_fresh();

	/* Quit */
#ifdef JP
quit("ソフトのバグ");
#else
	quit("software bug");
#endif

}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else	/* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}
#endif	/* HANDLE_SIGNALS */

//スコアサーバに送るプレイヤー名を登録する
void get_score_server_name(void)
{

	bool need_set = FALSE;
	char tmp_name[SCORE_SERVER_STR_LEN] = "";
	char tmp_pass[SCORE_SERVER_STR_LEN] = "";
	int i;

	if(!strlen(score_server_name))
		need_set = TRUE;
	else if(!get_check_strict(format(_("プレイヤー「%s」としてスコアを送ります。", "Send the score as player \"%s\"? "), score_server_name), CHECK_DEFAULT_Y | CHECK_NO_HISTORY))
		need_set = TRUE;

	if(!need_set) return;

	screen_save();
	Term_clear();

	while(1)
	{
#ifdef JP
		prt("「プレイヤー名」を設定してください。", 5,25);
		prt("「プレイヤー名」は、スコアサーバに表示されます。", 6,25);
		prt("設定せずにスコアを送ることもできます。", 7,25);
		prt("一部の記号は使用できません。", 8,25);
#else
		prt("Please set the \"player name\".", 5, 25);
		prt("The \"player name\" will be displayed on the score server.", 6, 25);
		prt("You can also send the score without setting it.", 7, 25);
		prt("Some symbols can not be used in the name.", 8, 25);
#endif

		if (get_string(_("プレイヤー名: ", "Player name: "), tmp_name, SCORE_SERVER_STR_LEN-4))
		{

			for (i = 0; tmp_name[i]; i++)
			{
				unsigned char c = tmp_name[i];

#ifdef JP
				if(iskanji(c) || iskana(c))
				{
					i++;
					continue;
				}
				else if (my_strchr("\"*,/:;<>?\\| ", c)) tmp_name[i] = '_';
#else
				if (my_strchr("\"*,/:;<>?\\| ", c)) tmp_name[i] = '_';
#endif
			}
		}
		else
			strcpy(tmp_name,"");

		Term_clear();

		if(!strlen(tmp_name))
			prt(_("プレイヤー名は設定されません。", "The player name is not set."), 5,25);
		else
			prt(format(_("プレイヤー名:%s", "Player name: %s"), tmp_name), 5,25);

		if (get_check_strict(_("よろしいですか？", "Is that name okay? "), CHECK_DEFAULT_Y | CHECK_NO_HISTORY)) break;

	}
	//いま記録されてるのと一致しない(strcmpが!=0)とき保存
	if(strcmp(score_server_name,tmp_name))
	{
		strcpy(score_server_name,tmp_name);
		//スコア送信直前なのでセーブが必要
		if (!save_player()) msg_print(_("セーブ失敗！", "Save failed!"));
	}

	screen_load();

}


//v1.1.42 プレイヤースコア倍率(%単位)を計算する。
//種族・職業・性格に関する倍率を計算するが、難易度・初期オプション・日数経過ペナルティなどの倍率は計算しない。
//スコア計算、自分に関する情報の「あなたのスコア倍率」値、クエストの特殊ランダムユニークモンスターのパワー決定に使う。
int	calc_player_score_mult(void)
{
	int i;
	int mult;

	//まず種族によるスコアを計算する。今の種族の倍率を入れ、これまでの種族変容履歴にもっとスコアの低いのがあれば上書きする。
	mult = race_info[p_ptr->prace].score_mult;

	//v1.1.53b 妖精薬師が蓬莱の薬を飲んだとき種族蓬莱人の種族倍率が適用されてしまっていたので薬師出身の蓬莱人のスコア倍率を高めとく
	if (p_ptr->prace == RACE_HOURAI && p_ptr->pclass == CLASS_CHEMIST) mult = 200;

	for (i = 0; i < MAX_RACES; i++)
	{
		if (i < 32)
		{
			if (!(p_ptr->old_race1 & 1L << i) || mult < race_info[i].score_mult) continue;
		}
		else
		{
			if (!(p_ptr->old_race2 & 1L << (i - 32)) || mult < race_info[i].score_mult) continue;
		}
		mult = race_info[i].score_mult;
	}

	//職業によるスコア倍率を乗ずる。
	mult = mult * class_info[p_ptr->pclass].score_mult / 100;

	//一部の職業や性格による特殊処理を行う。

	switch (p_ptr->pclass)
	{
	case CLASS_MOKOU:
		//妹紅はリザレクション回数でスコア減少
		if (p_ptr->magic_num1[0]) mult /= (1 + MIN(99, p_ptr->magic_num1[0]));
		break;
	case CLASS_KAGUYA:
		//輝夜は初期所持神宝の数でスコア減少
		if (kaguya_quest_level && !ironman_no_fixed_art)
		{
			for (i = 0; i<kaguya_quest_level; i++) mult /= 2;
		}
		break;
	case CLASS_CIRNO:
		//v1.1.32 チルノの性格「日焼けした」はスコア半分
		if (is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mult /= 2;
		break;
	case CLASS_LUNASA:
	case CLASS_MERLIN:
	case CLASS_LYRICA:
		//プリズムリバー姉妹は一度も交代しなければスコア30%アップ
		if (!p_ptr->magic_num2[75]) mult = mult * 13 / 10;
		break;
	case CLASS_NARUMI:
		//好戦的な成美はスコア-25%
		if (is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			mult = mult * 3 / 4;
		break;
	case CLASS_3_FAIRIES:
		//v1.1.68 妖精大集合はスコア1/3
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			mult /= 3;
		break;

	case CLASS_MEGUMU:
		//v2.0.3 専用性格はスコア1/4
		if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU))
			mult /= 4;
		break;


	}

	//性格「狂気の」はスコア半分
	if (p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		mult /= 2;

	}


	if (mult<1) mult = 1;
	return	mult;
}
