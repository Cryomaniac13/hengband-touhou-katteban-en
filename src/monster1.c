/* File: monster1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: describe monsters (using monster memory) */

#include "angband.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[4] =
#ifdef JP
{ "それ", "彼", "彼女","彼女たち" };
#else
{ "it", "he", "she" };
#endif

static cptr wd_his[4] =
#ifdef JP
{ "それの", "彼の", "彼女の", "彼女たちの" };
#else
{ "its", "his", "her" };
#endif



/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
    (((c) == 1) ? (s) : (p))






/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b kills = r_ptr->r_tkills;

	if (cheat_know) return (TRUE);

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int r_idx, int i)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b a = r_ptr->r_blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	if (d >= ((4+level)*MAX_UCHAR)/80) d = ((4+level)*MAX_UCHAR-1)/80;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}

static cptr get_blow_method_desc(int method)
{
    cptr p;

    p = NULL;

    switch (method)
    {
        case RBM_HIT:		p = "hit"; break;
        case RBM_TOUCH:		p = "touch"; break;
        case RBM_PUNCH:		p = "punch"; break;
        case RBM_KICK:		p = "kick"; break;
        case RBM_CLAW:		p = "claw"; break;
        case RBM_BITE:		p = "bite"; break;
        case RBM_STING:		p = "stab"; break;
        case RBM_SLASH:		p = "slash"; break;
        case RBM_BUTT:		p = "butt"; break;
        case RBM_CRUSH:		p = "crush"; break;
        case RBM_ENGULF:	p = "engulf"; break;
        case RBM_CHARGE: 	p = "give an invoice";   break;
        case RBM_CRAWL:		p = "crawl on you"; break;
        case RBM_DROOL:		p = "drool on you"; break;
        case RBM_SPIT:		p = "spit"; break;
        case RBM_EXPLODE:	p = "explode"; break;
        case RBM_GAZE:		p = "gaze"; break;
        case RBM_WAIL:		p = "wail"; break;
        case RBM_SPORE:		p = "release spores"; break;
        case RBM_DANCE:		p = _("踊る", "dance"); break;
        case RBM_BEG:		p = "beg"; break;
        case RBM_INSULT:	p = "insult"; break;
        case RBM_MOAN:		p = "moan"; break;
        case RBM_SHOW:  	p = "sing"; break;
        case RBM_SPEAR:  	p = "thrust"; break;
        case RBM_BOOK:  	p = "bludgeon with a book"; break;
        case RBM_MOFUMOFU: 	p = "envelop with fluffy tail"; break;
        case RBM_SQUEEZE:  	p = "squeeze"; break;
        case RBM_DRAG:  	p = "drag"; break;
        case RBM_INJECT:  	p = "inject"; break;
        case RBM_SMILE:  	p = "smile"; break;
        case RBM_HEADBUTT: 	p = "headbutt"; break;
        case RBM_FLASH:  	p = "flash"; break;
        case RBM_HOWL:  	p = "howl"; break;
        case RBM_WHISPER:  	p = "whisper"; break;
        case RBM_PRESS:  	p = "press"; break;
        case RBM_POINT:  	p = "point"; break;
        case RBM_STRIKE:  	p = "strike"; break;
        case RBM_PLAY:  	p = "play"; break;
		case RBM_SMOKE:  	p = "blow smoke"; break;
    }

    return p;
}

static cptr get_blow_effect_desc(int effect)
{
    cptr p;

    p = NULL;

    switch (effect)
    {
        case RBE_SUPERHURT: p = "powerfully attack"; break;
        case RBE_HURT:     p = "attack"; break;
        case RBE_POISON:   p = "poison"; break;
        case RBE_UN_BONUS: p = "disenchant"; break;
        case RBE_UN_POWER: p = "drain charges"; break;
        case RBE_EAT_GOLD: p = "steal gold"; break;
        case RBE_EAT_ITEM: p = "steal items"; break;
        case RBE_EAT_FOOD: p = "eat your food"; break;
        case RBE_EAT_LITE: p = "absorb light"; break;
        case RBE_ACID:     p = "shoot acid"; break;
        case RBE_ELEC:     p = "electrocute"; break;
        case RBE_FIRE:     p = "burn"; break;
        case RBE_COLD:     p = "freeze"; break;
        case RBE_BLIND:    p = "blind"; break;
        case RBE_CONFUSE:  p = "confuse"; break;
        case RBE_TERRIFY:  p = "terrify"; break;
        case RBE_PARALYZE: p = "paralyze"; break;
        case RBE_LOSE_STR: p = "reduce strength"; break;
        case RBE_LOSE_INT: p = "reduce intelligence"; break;
        case RBE_LOSE_WIS: p = "reduce wisdom"; break;
        case RBE_LOSE_DEX: p = "reduce dexterity"; break;
        case RBE_LOSE_CON: p = "reduce constitution"; break;
        case RBE_LOSE_CHR: p = "reduce charisma"; break;
        case RBE_LOSE_ALL: p = "reduce all stats"; break;
        case RBE_SHATTER:  p = "shatter"; break;
        case RBE_EXP_10:   p = "lower experience (by 10d6+)"; break;
        case RBE_EXP_20:   p = "lower experience (by 20d6+)"; break;
        case RBE_EXP_40:   p = "lower experience (by 40d6+)"; break;
        case RBE_EXP_80:   p = "lower experience (by 80d6+)"; break;
        case RBE_DISEASE:  p = "disease"; break;
        case RBE_TIME:     p = "time"; break;
        case RBE_EXP_VAMP: p = "drain life force"; break;
        case RBE_DR_MANA:  p = "drain mana force"; break;
        case RBE_INERTIA:  p = "slow"; break;
        case RBE_STUN:     p = "stun"; break;
        case RBE_CHAOS:    p = "attack with chaos"; break;
        case RBE_ELEMENT:  p = "attack with elements"; break;
        case RBE_MUTATE:   p = "cause mutations"; break;
        case RBE_SMITE:    p = "purify"; break;
        case RBE_DROWN:    p = "drown"; break;
        case RBE_KILL:     p = "invite to death"; break;
        case RBE_CURSE:    p = "curse"; break;
        case RBE_PHOTO:    p = "take a photo"; break;
        case RBE_MELT:     p = "melt"; break;
        case RBE_BLEED:    p = "wound"; break;
        case RBE_INSANITY: p = "cause insanity"; break;
        case RBE_HUNGER:   p = "cause hunger"; break;
    }

    return p;
}

/*
 * Prepare hook for c_roff(). It will be changed for spoiler generation in wizard1.c.
 */
void (*hook_c_roff)(byte attr, cptr str) = c_roff;

static void hooked_roff(cptr str)
{
	/* Spawn */
	hook_c_roff(TERM_WHITE, str);
}

static void roff_new_section()
{
    hooked_roff("\n");
}

/*
 * Hack -- display monster information using "hooked_roff()"
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
/*:::モンスターの思い出を表示する*/
/*:::mode:0x03のときウィザードコマンドからのスポイラー生成　全てのフラグを表示するが＠が倒した数や倒された数などは書かず出力先はファイル*/
///mon 思い出表示　フラグ処理など

static void roff_aux(int r_idx, int mode)
{
	monster_race    *r_ptr = &r_info[r_idx];
	bool            old = FALSE;
	int             m, n, r;
	cptr            p, q;

#ifdef JP
	char            jverb_buf[64];
#else
	bool            sin = FALSE;
#endif
	int             msex = 0;

	bool nightmare = ironman_nightmare && !(mode & 0x02);
	int speed = nightmare ? r_ptr->speed + 5 : r_ptr->speed;

	bool            breath = FALSE;
	bool            magic = FALSE;

	u32b		flags1;
	u32b		flags2;
	u32b		flags3;
	u32b		flags4;
	u32b		flags5;
	u32b		flags6;
	u32b		flags7;
	u32b		flags9;

	u32b		flagsr;

	byte drop_gold, drop_item;

	int		vn = 0;
	//byte		color[96];
	//cptr		vp[96];

	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加 魔法数 96→128
	byte		color[128];
	cptr		vp[128];

	bool know_everything = FALSE;

	/* Obtain a copy of the "known" number of drops */
	drop_gold = r_ptr->r_drop_gold;
	drop_item = r_ptr->r_drop_item;

	/* Obtain a copy of the "known" flags */
	/*:::＠の「知っている」モンスターフラグを得る*/
	flags1 = (r_ptr->flags1 & r_ptr->r_flags1);
	flags2 = (r_ptr->flags2 & r_ptr->r_flags2);
	flags3 = (r_ptr->flags3 & r_ptr->r_flags3);
	flags4 = (r_ptr->flags4 & r_ptr->r_flags4);
	flags5 = (r_ptr->flags5 & r_ptr->r_flags5);
	flags6 = (r_ptr->flags6 & r_ptr->r_flags6);
	/*:::flags7は空を飛んでいるなど「見ればわかる」フラグ*/
	flags7 = (r_ptr->flags7 & r_ptr->flags7);

	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加
	flags9 = (r_ptr->flags9 & r_ptr->r_flags9);
	flagsr = (r_ptr->flagsr & r_ptr->r_flagsr);

	/* cheat_know or research_mon() */
	if (cheat_know || (mode & 0x01))
		know_everything = TRUE;

	/* Cheat -- Know everything */
	if (know_everything)
	{
		/* Hack -- maximal drops */
		drop_gold = drop_item =
		(((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

		/* Hack -- but only "valid" drops */
		if (r_ptr->flags1 & RF1_ONLY_GOLD) drop_item = 0;
		if (r_ptr->flags1 & RF1_ONLY_ITEM) drop_gold = 0;

		/* Hack -- know all the flags */
		flags1 = r_ptr->flags1;
		flags2 = r_ptr->flags2;
		flags3 = r_ptr->flags3;
		flags4 = r_ptr->flags4;
		flags5 = r_ptr->flags5;
		flags6 = r_ptr->flags6;
	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加
		flags9 = r_ptr->flags9;

		flagsr = r_ptr->flagsr;
	}


	///mod150731 彼、彼女などの文言　三妖精のみ「彼女たち」の3にする
	/* Extract a gender (if applicable) */
	if(r_idx == MON_3FAIRIES) msex = 3;
	else if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	/* Assume some "obvious" flags */
	if (r_ptr->flags1 & RF1_UNIQUE)  flags1 |= (RF1_UNIQUE);
	if (r_ptr->flags1 & RF1_QUESTOR) flags1 |= (RF1_QUESTOR);
	if (r_ptr->flags1 & RF1_MALE)    flags1 |= (RF1_MALE);
	if (r_ptr->flags1 & RF1_FEMALE)  flags1 |= (RF1_FEMALE);

	/* Assume some "creation" flags */
	if (r_ptr->flags1 & RF1_FRIENDS) flags1 |= (RF1_FRIENDS);
	if (r_ptr->flags1 & RF1_ESCORT)  flags1 |= (RF1_ESCORT);
	if (r_ptr->flags1 & RF1_ESCORTS) flags1 |= (RF1_ESCORTS);

	/* Killing a monster reveals some properties */
	if (r_ptr->r_tkills || know_everything)
	{
		/* Know "race" flags */
		///mon フラグ
		///mod131231 モンスターフラグ変更
		if (r_ptr->flags3 & RF3_KWAI)      flags3 |= (RF3_KWAI);
		if (r_ptr->flags3 & RF3_DEITY)    flags3 |= (RF3_DEITY);
		if (r_ptr->flags3 & RF3_DEMIHUMAN)    flags3 |= (RF3_DEMIHUMAN);
		if (r_ptr->flags3 & RF3_DRAGON)   flags3 |= (RF3_DRAGON);
		if (r_ptr->flags3 & RF3_DEMON)    flags3 |= (RF3_DEMON);
		if (r_ptr->flags3 & RF3_UNDEAD)   flags3 |= (RF3_UNDEAD);
		if (r_ptr->flags3 & RF3_ANG_CHAOS)     flags3 |= (RF3_ANG_CHAOS);
		if (r_ptr->flags3 & RF3_ANG_COSMOS)     flags3 |= (RF3_ANG_COSMOS);
		if (r_ptr->flags3 & RF3_ANIMAL)   flags3 |= (RF3_ANIMAL);
		if (r_ptr->flags3 & RF3_ANG_AMBER) flags3 |= (RF3_ANG_AMBER);
//		if (r_ptr->flags2 & RF2_HUMAN)    flags2 |= (RF2_HUMAN);
		if (r_ptr->flags3 & RF3_HUMAN)    flags3 |= (RF3_HUMAN);

		/* Know 'quantum' flag */
		if (r_ptr->flags2 & RF2_QUANTUM)  flags2 |= (RF2_QUANTUM);

		/* Know "forced" flags */
		if (r_ptr->flags1 & RF1_FORCE_DEPTH) flags1 |= (RF1_FORCE_DEPTH);
		if (r_ptr->flags1 & RF1_FORCE_MAXHP) flags1 |= (RF1_FORCE_MAXHP);
	}

	/* For output_monster_spoiler() */
	if (mode & 0x02)
	{
		/* Nothing to do */
	}
	else if(r_idx == MON_EXTRA_FIELD)
	{
		hooked_roff(_("あなたはいまこの姿になっている。", "You have taken on this form."));
		hooked_roff("\n");
	}

	/* Treat uniques differently */
	else if (flags1 & RF1_UNIQUE)
	{
		/* Hack -- Determine if the unique is "dead" */
		bool dead = (r_ptr->max_num == 0) ? TRUE : FALSE;

		///msg140101
		/* We've been killed... */
		if (r_ptr->r_deaths)
		{
			/* Killed ancestors */
#ifdef JP
			hooked_roff(format("%^sはあなたを過去に %d 回倒している",
					   wd_he[msex], r_ptr->r_deaths));
#else
			hooked_roff(format("%^s has defeated you %d time(s) in the past",
					   wd_he[msex], r_ptr->r_deaths));
#endif


			/* But we've also killed it */
			if (dead)
			{
#ifdef JP
				hooked_roff(format("が、すでに仕返しは済ませている。"));
#else
				hooked_roff(format(", but you had your revenge.  "));
#endif

			}

			/* Unavenged (ever) */
			else
			{
#ifdef JP
				hooked_roff(format("。仕返しはまだだ。"));
#else
				hooked_roff(format(". You haven't had your revenge yet.  "));
#endif

			}

			/* Start a new line */
			hooked_roff("\n");
		}

		/* Dead unique who never hurt us */
		else if (dead)
		{
#ifdef JP
			hooked_roff("あなたはこの仇敵をすでに倒している。");
#else
			hooked_roff("You have defeated this foe.  ");
#endif

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	/* Not unique, but killed us */
	else if (r_ptr->r_deaths)
	{
		/* Dead ancestors */
#ifdef JP
		hooked_roff(format("このモンスターはあなたを過去に %d 回倒している",
			    r_ptr->r_deaths ));
#else
		hooked_roff(format("This creature has defeated you %d time(s) in the past", r_ptr->r_deaths));
#endif


		/* Some kills this life */
		if (r_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("が、あなたはこのモンスターを少なくとも %d 体は倒している。", r_ptr->r_pkills));
#else
			hooked_roff(format(", and you have defeated at least %d of these creatures.  ", r_ptr->r_pkills));
#endif

		}

		/* Some kills past lives */
		else if (r_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("が、あなたは過去にこのモンスターを少なくとも %d 回は倒している。", r_ptr->r_tkills));
#else
			hooked_roff(format(", and you have defeated at least %d of the creatures in the past.  ", r_ptr->r_tkills));
#endif

		}

		/* No kills */
		else
		{
#ifdef JP
			hooked_roff(format("が、まだ%sを倒したことはない。", wd_he[msex]));
#else
			hooked_roff(format(", and you haven't defeated %s yet.  ", wd_he[msex]));
#endif

		}

		/* Start a new line */
		hooked_roff("\n");
	}

	/* Normal monsters */
	else
	{
		/* Killed some this life */
		if (r_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("あなたはこのモンスターを少なくとも %d 体は倒している。", r_ptr->r_pkills));
#else
			hooked_roff(format("You have defeated at least %d of these creatures.  ", r_ptr->r_pkills));
#endif

		}

		/* Killed some last life */
		else if (r_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("あなたは過去にこのモンスターを少なくとも %d 体は倒している。", r_ptr->r_tkills));
#else
			hooked_roff(format("You have defeated at least %d of these creatures in the past.  ", r_ptr->r_tkills));
#endif

		}

		/* Killed none */
		else
		{
#ifdef JP
			hooked_roff("このモンスターを倒したことはない。");
#else
			hooked_roff("You don't recall defeating this creature.  ");
#endif
		}

		/* Start a new line */
		hooked_roff("\n");
	}
	///tmp
	//return;
	/* Descriptions */
	{
		/*:::モンスターの解説文を表示*/
		cptr tmp = r_text + r_ptr->text;

		if (tmp[0])
		{
			/* Dump it */
			hooked_roff(tmp);

			/* Start a new line */
            hooked_roff("\n");
            hooked_roff("\n");
		}
	}

	if (r_idx == MON_KAGE)
	{
		/* All done */
		hooked_roff("\n");

		return;
	}

	/* Nothing yet */
	old = FALSE;

	/* Describe location */
	if (r_ptr->level == 0)
	{
#ifdef JP
		hooked_roff(format("%^sは地上に住み", wd_he[msex]));
#else
		hooked_roff(format("%^s lives on the surface", wd_he[msex]));
#endif

		old = TRUE;
	}
	else if (r_ptr->r_tkills || know_everything)
	{
		if (depth_in_feet)
		{
#ifdef JP
			hooked_roff(format("%^sは通常地下 %d フィートで出現し", wd_he[msex], r_ptr->level * 50));
#else
			hooked_roff(format("%^s is normally found at depths of %d feet", wd_he[msex], r_ptr->level * 50));
#endif
		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは通常地下 %d 階で出現し", wd_he[msex], r_ptr->level));
#else
			hooked_roff(format("%^s is normally found on dungeon level %d", wd_he[msex], r_ptr->level));
#endif
		}
		old = TRUE;
	}


	/* Describe movement */
	///mon カメレオンのモンスター思い出表示
	if (r_idx == MON_CHAMELEON)
	{
#ifdef JP
		hooked_roff("、他のモンスターに化ける。");
#else
		hooked_roff("and can take the shape of other monster.");
#endif
		return;
	}
	else
	{
		/* Introduction */
		if (old)
		{
#ifdef JP
			hooked_roff("、");
#else
			hooked_roff(", and ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}
#ifndef JP
		hooked_roff("moves");
#endif

		/* Random-ness */
		if ((flags1 & RF1_RAND_50) || (flags1 & RF1_RAND_25))
		{
			/* Adverb */
			if ((flags1 & RF1_RAND_50) && (flags1 & RF1_RAND_25))
			{
#ifdef JP
				hooked_roff("かなり");
#else
				hooked_roff(" extremely");
#endif

			}
			else if (flags1 & RF1_RAND_50)
			{
#ifdef JP
				hooked_roff("幾分");
#else
				hooked_roff(" somewhat");
#endif

			}
			else if (flags1 & RF1_RAND_25)
			{
#ifdef JP
				hooked_roff("少々");
#else
				hooked_roff(" a bit");
#endif

			}

			/* Adjective */
#ifdef JP
			hooked_roff("不規則に");
#else
			hooked_roff(" erratically");
#endif


			/* Hack -- Occasional conjunction */
#ifdef JP
			if (speed != 110) hooked_roff("、かつ");
#else
			if (speed != 110) hooked_roff(", and");
#endif

		}

		/* Speed */
		if (speed > 110)
		{
#ifdef JP
			if (speed > 139) hook_c_roff(TERM_RED, "信じ難いほど");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, "猛烈に");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, "非常に");
			else if (speed > 124) hook_c_roff(TERM_UMBER, "かなり");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, "やや");
			hook_c_roff(TERM_L_RED, "素早く");
#else
			if (speed > 139) hook_c_roff(TERM_RED, " incredibly");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, " extremely");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, " very");
			else if (speed > 124) hook_c_roff(TERM_UMBER, " fairly");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, " somewhat");
			hook_c_roff(TERM_L_RED, " quickly");
#endif

		}
		else if (speed < 110)
		{
#ifdef JP
			if (speed < 90) hook_c_roff(TERM_L_GREEN, "信じ難いほど");
			else if (speed < 95) hook_c_roff(TERM_BLUE, "非常に");
			else if (speed < 100) hook_c_roff(TERM_BLUE, "かなり");
			else if (speed > 104) hook_c_roff(TERM_GREEN, "やや");
			hook_c_roff(TERM_L_BLUE, "ゆっくりと");
#else
			if (speed < 90) hook_c_roff(TERM_L_GREEN, " incredibly");
			else if (speed < 95) hook_c_roff(TERM_BLUE, " very");
			else if (speed < 100) hook_c_roff(TERM_BLUE, " fairly");
			else if (speed > 104) hook_c_roff(TERM_GREEN, " somewhat");
			hook_c_roff(TERM_L_BLUE, " slowly");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff("普通の速さで");
#else
			hooked_roff(" at normal speed");
#endif

		}
#ifdef JP
		hooked_roff("動いている");
#endif
	}

	/* The code above includes "attack speed" */
	if (flags1 & RF1_NEVER_MOVE)
	{
		/* Introduce */
		if (old)
		{
#ifdef JP
			hooked_roff("、しかし");
#else
			hooked_roff(", but ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}

		/* Describe */
#ifdef JP
		hooked_roff("侵入者を追跡しない");
#else
		hooked_roff("does not deign to chase intruders");
#endif

	}

	/* End this sentence */
	if (old)
	{
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".");

		roff_new_section();
#endif

		old = FALSE;
	}


	/* Describe experience if known */
	///mod140719 種族などの情報は倒さなくても表示するようにした
	//if (r_ptr->r_tkills || know_everything)
	{
		/* Introduction */
#ifdef JP
		hooked_roff("この");
#else
		hooked_roff("This");
#endif


		/* Describe the "quality" */
#ifdef JP
if (flags3 & RF3_ANG_CHAOS)            hook_c_roff(TERM_L_DARK, "混沌の勢力に属する");
#else
		if (flags3 & RF3_ANG_CHAOS)            hook_c_roff(TERM_L_DARK, " chaotically aligned");
#endif

#ifdef JP
if (flags3 & RF3_ANG_COSMOS)            hook_c_roff(TERM_YELLOW, "秩序の勢力に属する");
#else
		if (flags3 & RF3_ANG_COSMOS)            hook_c_roff(TERM_YELLOW, " lawfully aligned");
#endif

/*:::「狂気を誘う」などの記述は倒さなくても表示するように下に移した*/


/*:::すっきりしないので下に移動した
#ifdef JP
if (flags3 & RF3_ANIMAL)          hook_c_roff(TERM_L_GREEN, "自然界の");
#else
		if (flags3 & RF3_ANIMAL)          hook_c_roff(TERM_L_GREEN, " natural");
#endif
#ifdef JP
if (flags3 & RF3_UNDEAD)          hook_c_roff(TERM_VIOLET, "アンデッドの");
#else
		if (flags3 & RF3_UNDEAD)          hook_c_roff(TERM_VIOLET, " undead");
#endif
*/
#ifdef JP
if (flags3 & RF3_ANG_AMBER)        hook_c_roff(TERM_VIOLET, "アンバーの王族の");
#else
		if (flags3 & RF3_ANG_AMBER)        hook_c_roff(TERM_VIOLET, " Amberite");
#endif



	///mod131231 モンスターフラグ変更
	if ((flags3 & (RF3_ANIMAL | RF3_DRAGON | RF3_DEMON | RF3_FAIRY | RF3_DEMIHUMAN | RF3_DEITY | RF3_KWAI | RF3_HUMAN | RF3_UNDEAD | RF3_HANIWA)) || (flags2 & (RF2_QUANTUM)))
	//if ((flags3 & (RF3_DRAGON | RF3_DEMON | RF3_GIANT | RF3_TROLL | RF3_ORC)) || (flags2 & (RF2_QUANTUM | RF2_HUMAN)))
	{
		bool flag_halfblood = FALSE;
	/* Describe the "race" */

#ifdef JP
if (flags3 & RF3_DRAGON){
 hook_c_roff(TERM_ORANGE, "ドラゴン");
flag_halfblood = TRUE;
}
#else
    if (flags3 & RF3_DRAGON)    hook_c_roff(TERM_ORANGE, " dragon");
#endif

#ifdef JP
if (flags3 & RF3_UNDEAD)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "アンデッド");
}
#else
    if (flags3 & RF3_UNDEAD)    hook_c_roff(TERM_VIOLET, " undead");
#endif

#ifdef JP
if (flags3 & RF3_ANIMAL)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_GREEN, "獣");
}
#else
    if (flags3 & RF3_ANIMAL)    hook_c_roff(TERM_L_GREEN, " beast");
#endif

#ifdef JP
if (flags3 & RF3_DEMON)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "悪魔");
}
#else
    if (flags3 & RF3_DEMON)    hook_c_roff(TERM_VIOLET, " demon");
#endif

#ifdef JP
if (flags3 & RF3_DEMIHUMAN)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_UMBER, "人間型生物");
}
#else
		if (flags3 & RF3_DEMIHUMAN)    hook_c_roff(TERM_L_UMBER, " demihuman");
#endif

#ifdef JP
if (flags3 & RF3_DEITY)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_BLUE, "神格");
}
#else
		if (flags3 & RF3_DEITY)    hook_c_roff(TERM_L_BLUE, " deity");
#endif

#ifdef JP
if (flags3 & RF3_KWAI){
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_UMBER, "妖怪");
}

#else
		if (flags3 & RF3_KWAI)      hook_c_roff(TERM_UMBER, " youkai");
#endif

#ifdef JP
if (flags3 & RF3_HUMAN)
{
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_WHITE, "人間");
}

	//if (flags2 & RF2_HUMAN) hook_c_roff(TERM_L_WHITE, "人間");
#else
		if (flags3 & RF3_HUMAN) hook_c_roff(TERM_L_WHITE, " human");
#endif

#ifdef JP
if (flags2 & RF2_QUANTUM){
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "量子生物");
}
#else
		if (flags2 & RF2_QUANTUM)  hook_c_roff(TERM_VIOLET, " quantum creature");
#endif


#ifdef JP
if (flags3 & RF3_FAIRY){
	if(flag_halfblood == TRUE) 	hooked_roff("にして");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_YELLOW, "妖精");
}
#else
		if (flags3 & RF3_FAIRY)    hook_c_roff(TERM_VIOLET, " fairy");
#endif

#ifdef JP
	if (flags3 & RF3_HANIWA) {
		if (flag_halfblood == TRUE) 	hooked_roff("にして");
		else 	flag_halfblood = TRUE;
		hook_c_roff(TERM_ORANGE, "埴輪");
	}
#else
    if (flags3 & RF3_HANIWA)    hook_c_roff(TERM_ORANGE, " haniwa");
#endif
	}

#ifdef JP
else                            hooked_roff("存在");
#else
		else                            hooked_roff(" creature");
#endif


#ifdef JP
		hooked_roff("を倒すことは");
#endif
		/* Group some variables */
		{
			long i, j;

#ifdef JP
			i = p_ptr->lev;
			hooked_roff(format(" %lu レベルのキャラクタにとって", (long)i));

			i = (long)r_ptr->mexp * r_ptr->level / (p_ptr->max_plv+2);
			j = ((((long)r_ptr->mexp * r_ptr->level % (p_ptr->max_plv+2)) *
			       (long)1000 / (p_ptr->max_plv+2) + 5) / 10);

			hooked_roff(format(" 約%ld.%02ld ポイントの経験となる。",
				(long)i, (long)j ));
#else
			/* calculate the integer exp part */
			i = (long)r_ptr->mexp * r_ptr->level / (p_ptr->max_plv+2);

			/* calculate the fractional exp part scaled by 100, */
			/* must use long arithmetic to avoid overflow  */
			j = ((((long)r_ptr->mexp * r_ptr->level % (p_ptr->max_plv+2)) *
			       (long)1000 / (p_ptr->max_plv+2) + 5) / 10);

			/* Mention the experience */
			hooked_roff(format(" is worth about %ld.%02ld point%s",
				    (long)i, (long)j,
				    (((i == 1) && (j == 0)) ? "" : "s")));

			/* Take account of annoying English */
			p = "th";
			i = p_ptr->lev % 10;
			if ((p_ptr->lev / 10) == 1) /* nothing */;
			else if (i == 1) p = "st";
			else if (i == 2) p = "nd";
			else if (i == 3) p = "rd";

			/* Take account of "leading vowels" in numbers */
			q = "";
			i = p_ptr->lev;
			if ((i == 8) || (i == 11) || (i == 18)) q = "n";

			/* Mention the dependance on the player's level */
			hooked_roff(format(" for a%s %lu%s level character.  ",
				    q, (long)i, p));
            roff_new_section();
#endif

		}
	}
	///mod140608 見てわかる特徴は倒さなくても表示するようにした
	if (flags7 & RF7_CAN_FLY || (r_ptr->flags2 & RF2_GIGANTIC) || (flags2 & RF2_POWERFUL) || (flags2 & RF2_ELDRITCH_HORROR))
	{
		/*:::最初、bool eld=(flags2 & RF2_ELDRITCH_HORROR)みたいに初期化しててfly以外正常に動作しなかった。*/
		/*:::どうもbool型は内部的にはbyte型で、このような初期化をすると末尾の8バイト分しか使われないらしい。*/
		/*:::これまでに作ったコードが心配だ。-140608-*/
		bool fly=FALSE,giga=FALSE,power=FALSE,eld=FALSE;
		if(flags7 & RF7_CAN_FLY) fly = TRUE;
		if(r_ptr->flags2 & RF2_GIGANTIC) giga = TRUE;
		if(flags2 & RF2_POWERFUL) power = TRUE;
		if(flags2 & RF2_ELDRITCH_HORROR) eld = TRUE;

		hook_c_roff(TERM_WHITE, format(_("%^sは", "%^s is a "), wd_he[msex]));
#ifdef JP
		if (giga)
		{
			if(!(fly || eld || power))hook_c_roff(TERM_ORANGE, "巨大だ。");
			else hook_c_roff(TERM_ORANGE, "巨大で");

		}
		if (power)
		{
			if(!( fly || eld))hook_c_roff(TERM_RED, "力強い。");
			else hook_c_roff(TERM_RED, "力強く");
		}
		if (fly)
		{
			if(!eld)	hook_c_roff(TERM_L_BLUE,"空を飛んでいる。");
			else	hook_c_roff(TERM_L_BLUE,"空を飛んでいて");
		}
		if (eld)
		{
			hook_c_roff(TERM_VIOLET, "狂気を誘う悍ましい姿だ。");
		}
    #else
        if (giga)
		{
            if(!(fly || eld || power))hook_c_roff(TERM_ORANGE, "gigantic ");
			else hook_c_roff(TERM_ORANGE, "gigantic, ");
		}
		if (power)
		{
		    if(!( fly || eld))hook_c_roff(TERM_RED, "powerful ");
			else hook_c_roff(TERM_RED, "powerful, ");
		}
		if (fly)
		{
		    if(!eld)	hook_c_roff(TERM_L_BLUE,"flying ");
			else	hook_c_roff(TERM_L_BLUE,"flying, ");
		}
		if (eld)
		{
		    hook_c_roff(TERM_VIOLET, "madness inducing ");
		}

		hook_c_roff(TERM_WHITE, "creature. ");
		roff_new_section();
    #endif

	}



///mod131231 モンスターフラグ変更
	if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC) && (flags2 & RF2_AURA_COLD))
	//if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, format("%^sは炎と氷とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_VIOLET, format("%^s is surrounded by flames, ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, format("%^sは炎とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_RED, format("%^s is surrounded by flames and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_COLD))
	//else if ((flags2 & RF2_AURA_FIRE) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^sは炎と氷に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by flames and ice.  ", wd_he[msex]));
#endif
	}
	//else if ((flags3 & RF3_AURA_COLD) && (flags2 & RF2_AURA_ELEC))
	else if ((flags2 & RF2_AURA_COLD) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_GREEN, format("%^sは氷とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_GREEN, format("%^s is surrounded by ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_FIRE)
	{
#ifdef JP
		hook_c_roff(TERM_RED, format("%^sは炎に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_RED, format("%^s is surrounded by flames.  ", wd_he[msex]));
#endif
	}
	//else if (flags3 & RF3_AURA_COLD)
	else if (flags2 & RF2_AURA_COLD)
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^sは氷に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by ice.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_ELEC)
	{
#ifdef JP
		hook_c_roff(TERM_L_BLUE, format("%^sはスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_BLUE, format("%^s is surrounded by electricity.  ", wd_he[msex]));
#endif
	}

	if (flags2 & RF2_REFLECTING)
	{
#ifdef JP
		hooked_roff(format("%^sは矢の呪文を跳ね返す。", wd_he[msex]));
#else
		hooked_roff(format("%^s reflects bolt spells.  ", wd_he[msex]));
#endif

	}

#ifndef JP
    if ((flags2 & RF2_AURA_FIRE) || (flags2 & RF2_AURA_ELEC) || (flags2 & RF2_AURA_COLD) || (flags2 & RF2_REFLECTING))
    {
        roff_new_section();
    }
#endif // JP

	/* Describe escorts */
	if ((flags1 & RF1_ESCORT) || (flags1 & RF1_ESCORTS))
	{
#ifdef JP
		hooked_roff(format("%^sは通常護衛を伴って現れる。", wd_he[msex]));
#else
		hooked_roff(format("%^s usually appears with escorts.  ", wd_he[msex]));
#endif
	}

	/* Describe friends */
	else if (flags1 & RF1_FRIENDS)
	{
#ifdef JP
		hooked_roff(format("%^sは通常集団で現れる。", wd_he[msex]));
#else
		hooked_roff(format("%^s usually appears in groups.  ", wd_he[msex]));
#endif
	}

	/* Collect inate attacks */
	vn = 0;
#ifdef JP
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "悲鳴で助けを求める";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "shriek for help";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & RF4_ROCKET)  {vp[vn] = "ロケットを発射する";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_ROCKET)  {vp[vn] = "shoot a rocket";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_WAVEMOTION)  {vp[vn] = "波動砲を発射する";color[vn++] = TERM_WHITE;}
#else
	if (flags4 & RF4_WAVEMOTION)  {vp[vn] = "fire a wave-motion cannon";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_BA_FORCE) {vp[vn] = "気を放って攻撃する";color[vn++] = TERM_YELLOW;}
#else
	if (flags4 & RF4_BA_FORCE) {vp[vn] = "attack by releasing Ki";color[vn++] = TERM_UMBER;}
#endif
#ifdef JP
	if (flags4 & RF4_DANMAKU) {vp[vn] = "弾幕を放って攻撃する";color[vn++] = TERM_RED;}
#else
	if (flags4 & RF4_DANMAKU) {vp[vn] = "fire danmaku";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_SHOOT) {vp[vn] = "遠隔攻撃をする";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_SHOOT) {vp[vn] = "attack from distance";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags6 & (RF6_SPECIAL) || flags4 & (RF4_SPECIAL2)) {vp[vn] = "特別な行動をする";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_SPECIAL) || flags4 & (RF4_SPECIAL2)) {vp[vn] = "do something";color[vn++] = TERM_VIOLET;}
#endif

	/* Describe inate attacks */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_OR);
				hook_c_roff(color[n], jverb_buf);
				hook_c_roff(color[n], "り");
				hooked_roff("、");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			/* Intro */
			if (n == 0) hooked_roff(" may ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("ことがある。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Collect breaths */
	vn = 0;
#ifdef JP
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "酸";color[vn++] = TERM_GREEN;}
#else
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "稲妻";color[vn++] = TERM_BLUE;}
#else
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "火炎";color[vn++] = TERM_RED;}
#else
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "frost";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "毒";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "地獄";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "閃光";color[vn++] = TERM_YELLOW;}
#else
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "暗黒";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "darkness";color[vn++] = TERM_L_DARK;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_HOLY))		{vp[vn] = "破邪";color[vn++] = TERM_WHITE;}
#else
	if (flags4 & (RF4_BR_HOLY))		{vp[vn] = "holy energy";color[vn++] = TERM_WHITE;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_HELL))		{vp[vn] = "地獄の業火";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_HELL))		{vp[vn] = "hellfire";color[vn++] = TERM_L_DARK;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "衝撃波";color[vn++] = TERM_ORANGE;}
#else
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "shockwaves";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_AQUA))		{vp[vn] = "水";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_AQUA))		{vp[vn] = "water";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "カオス";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "劣化";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "因果混乱";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "nexus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "時間逆転";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "遅鈍";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "重力";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "破片";color[vn++] = TERM_L_UMBER;}
#else
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "プラズマ";color[vn++] = TERM_L_RED;}
#else
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif

	///del131231 フォースブレス廃止
/*
#ifdef JP
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "フォース";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif
	*/
#ifdef JP
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "魔力";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "mana";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "核熱";color[vn++] = TERM_L_RED;}
#else
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "nuclear energy";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "分解";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "disintegration";color[vn++] = TERM_SLATE;}
#endif


	/* Describe breaths */
	if (vn)
	{
		/* Note breath */
		breath = TRUE;

		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("や");
#else
			if (n == 0) hooked_roff(" may breathe ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("のブレスを吐くことがある");
#endif
	}


	/* Collect spells */
	vn = 0;
#ifdef JP
if (flags5 & (RF5_BA_ACID))         {vp[vn] = "アシッド・ボール";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BA_ACID))         {vp[vn] = "produce acid balls";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "サンダー・ボール";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "produce lightning balls";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "ファイア・ボール";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "produce fire balls";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_COLD))         {vp[vn] = "アイス・ボール";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BA_COLD))         {vp[vn] = "produce frost balls";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_POIS))         {vp[vn] = "悪臭雲";color[vn++] = TERM_L_GREEN;}
#else
	if (flags5 & (RF5_BA_POIS))         {vp[vn] = "produce poison balls";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_NETH))         {vp[vn] = "地獄球";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_NETH))         {vp[vn] = "produce nether balls";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_WATE))         {vp[vn] = "ウォーター・ボール";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_WATE))         {vp[vn] = "produce water balls";color[vn++] = TERM_BLUE;}
#endif

///del 放射能球廃止
/*
#ifdef JP
if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "放射能球";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "produce balls of radiation";color[vn++] = TERM_L_GREEN;}
#endif
*/
#ifdef JP
if (flags5 & (RF5_BA_MANA))         {vp[vn] = "魔力の嵐";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BA_MANA))         {vp[vn] = "invoke mana storms";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_DARK))         {vp[vn] = "暗黒の嵐";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_DARK))         {vp[vn] = "invoke darkness storms";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_LITE))         {vp[vn] = "閃光の嵐";color[vn++] = TERM_YELLOW;}
#else
	if (flags5 & (RF5_BA_LITE))         {vp[vn] = "invoke light storms";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "純ログルス";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "invoke raw Logrus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "破滅の手";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "invoke the Hand of Doom";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "光の剣";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "psycho-spear";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "魔力吸収";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "drain mana";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "精神攻撃";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "cause mind blasting";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "脳攻撃";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "cause brain smashing";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "軽傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "cause light wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "重傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "cause serious wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "致命傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "cause critical wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "死の言霊";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "cause mortal wounds";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ACID))         {vp[vn] = "アシッド・ボルト";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BO_ACID))         {vp[vn] = "produce acid bolts";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "サンダー・ボルト";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "produce lightning bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "ファイア・ボルト";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "produce fire bolts";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_COLD))         {vp[vn] = "アイス・ボルト";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BO_COLD))         {vp[vn] = "produce frost bolts";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_NETH))         {vp[vn] = "地獄の矢";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BO_NETH))         {vp[vn] = "produce nether bolts";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_WATE))         {vp[vn] = "ウォーター・ボルト";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_WATE))         {vp[vn] = "produce water bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_MANA))         {vp[vn] = "魔力の矢";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BO_MANA))         {vp[vn] = "produce mana bolts";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "プラズマ・ボルト";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "produce plasma bolts";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "極寒の矢";color[vn++] = TERM_WHITE;}
#else
	if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "produce ice bolts";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_MISSILE))         {vp[vn] = "マジックミサイル";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_MISSILE))         {vp[vn] = "produce magic missiles";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_SCARE))           {vp[vn] = "恐怖";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_SCARE))           {vp[vn] = "terrify";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_BLIND))           {vp[vn] = "目くらまし";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BLIND))           {vp[vn] = "blind";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_CONF))            {vp[vn] = "混乱";color[vn++] = TERM_L_UMBER;}
#else
	if (flags5 & (RF5_CONF))            {vp[vn] = "confuse";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_SLOW))            {vp[vn] = "減速";color[vn++] = TERM_UMBER;}
#else
	if (flags5 & (RF5_SLOW))            {vp[vn] = "slow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_HOLD))            {vp[vn] = "麻痺";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_HOLD))            {vp[vn] = "paralyze";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_HASTE))           {vp[vn] = "加速";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_HASTE))           {vp[vn] = "haste-self";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_HEAL))            {vp[vn] = "治癒";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_HEAL))            {vp[vn] = "heal-self";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "無敵化";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "make invulnerable";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags4 & RF4_DISPEL)    {vp[vn] = "魔力消去";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_DISPEL)    {vp[vn] = "dispel-magic";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_BLINK))           {vp[vn] = "ショートテレポート";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_BLINK))           {vp[vn] = "blink-self";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TPORT))           {vp[vn] = "テレポート";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TPORT))           {vp[vn] = "teleport-self";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_WORLD))            {vp[vn] = "時を止める";color[vn++] = TERM_L_BLUE;}
#else
	if (flags6 & (RF6_WORLD))            {vp[vn] = "stop the time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_TO))         {vp[vn] = "テレポートバック";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_TELE_TO))         {vp[vn] = "teleport to";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "テレポートアウェイ";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "teleport away";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "テレポート・レベル";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "teleport level";color[vn++] = TERM_ORANGE;}
#endif

	if (flags6 & (RF6_DARKNESS))
	{
		if ((p_ptr->pclass != CLASS_NINJA) || (r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags7 & RF7_DARK_MASK) || (r_ptr->flagsr & RFR_HURT_LITE))
		{
#ifdef JP
			vp[vn] =  "暗闇"; color[vn++] = TERM_L_DARK;
#else
			vp[vn] = "create darkness"; color[vn++] = TERM_L_DARK;
#endif
		}
		else
		{
#ifdef JP
			vp[vn] = "閃光"; color[vn++] = TERM_YELLOW;
#else
			vp[vn] = "create light"; color[vn++] = TERM_YELLOW;
#endif
		}
	}

#ifdef JP
if (flags6 & (RF6_TRAPS))           {vp[vn] = "トラップ";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_TRAPS))           {vp[vn] = "create traps";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_FORGET))          {vp[vn] = "記憶消去";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_FORGET))          {vp[vn] = "cause amnesia";color[vn++] = TERM_BLUE;}
#endif
///mod 死者復活→コズミック・ホラー

#ifdef JP
if (flags6 & (RF6_COSMIC_HORROR))      {vp[vn] = "コズミック・ホラー";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_COSMIC_HORROR))      {vp[vn] = "invoke cosmic horror";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "モンスター一体召喚";color[vn++] = TERM_SLATE;}
#else
	if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "summon a monster";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "モンスター複数召喚";color[vn++] = TERM_L_WHITE;}
#else
	if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "summon monsters";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_KIN))           {vp[vn] = "救援召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_KIN))           {vp[vn] = "summon aid";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANT))           {vp[vn] = "アリ召喚";color[vn++] = TERM_RED;}
#else
	if (flags6 & (RF6_S_ANT))           {vp[vn] = "summon ants";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "虫召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "summon spiders";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HOUND))         {vp[vn] = "ハウンド召喚";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_S_HOUND))         {vp[vn] = "summon hounds";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "ヒドラ召喚";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "summon hydras";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "天使一体召喚";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "summon an angel";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DEMON))         {vp[vn] = "悪魔召喚";color[vn++] = TERM_L_RED;}
#else
	if (flags6 & (RF6_S_DEMON))         {vp[vn] = "summon a demon";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "アンデッド一体召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "summon an undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "ドラゴン一体召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "summon a dragon";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "強力なアンデッド召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "summon Greater Undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "古代ドラゴン召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "summon Ancient Dragons";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_CYBER))         {vp[vn] = "サイバーデーモン召喚";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_S_CYBER))         {vp[vn] = "summon Cyberdemons";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "アンバーの王族召喚";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "summon Lords of Amber";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "ユニーク・モンスター召喚";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "summon Unique Monsters";color[vn++] = TERM_VIOLET;}
#endif


#ifdef JP
if (flags9 & (RF9_ACID_STORM))        {vp[vn] = "酸の嵐";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_THUNDER_STORM))        {vp[vn] = "雷の嵐";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_FIRE_STORM))        {vp[vn] = "炎の嵐";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_ICE_STORM))        {vp[vn] = "氷の嵐";color[vn++] = TERM_L_WHITE;}
if (flags9 & (RF9_TOXIC_STORM))        {vp[vn] = "毒素の嵐";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_BA_POLLUTE))        {vp[vn] = "汚染の球";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_BA_DISI))        {vp[vn] = "原子分解";color[vn++] = TERM_SLATE;}
if (flags9 & (RF9_BA_HOLY))        {vp[vn] = "破邪の光球";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BA_METEOR))        {vp[vn] = "メテオストライク";color[vn++] = TERM_L_DARK;}
if (flags9 & (RF9_BA_DISTORTION))        {vp[vn] = "空間歪曲の球";color[vn++] = TERM_L_BLUE;}
if (flags9 & (RF9_PUNISH_1))        {vp[vn] = "退魔1";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_2))        {vp[vn] = "退魔2";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_3))        {vp[vn] = "退魔3";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_4))        {vp[vn] = "退魔4";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BO_HOLY))        {vp[vn] = "聖なる矢";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_GIGANTIC_LASER))        {vp[vn] = "巨大レーザー";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_BO_SOUND))        {vp[vn] = "♪のボルト";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_S_ANIMAL))        {vp[vn] = "動物召喚";color[vn++] = TERM_L_UMBER;}
if (flags9 & (RF9_BEAM_LITE))        {vp[vn] = "レーザー";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_SONICWAVE))        {vp[vn] = "超音波";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_HELLFIRE))        {vp[vn] = "ヘルファイア";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_HOLYFIRE))        {vp[vn] = "ホーリーファイア";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_FINALSPARK))        {vp[vn] = "ファイナルスパーク";color[vn++] = TERM_WHITE;}
if (flags9 & (RF9_TORNADO))        {vp[vn] = "竜巻";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_DESTRUCTION))        {vp[vn] = "*破壊*";color[vn++] = TERM_RED;}
if (flags9 & (RF9_S_DEITY))        {vp[vn] = "神格召喚";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_S_HI_DEMON))        {vp[vn] = "上位悪魔召喚";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_S_KWAI))        {vp[vn] = "妖怪呼集";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_TELE_APPROACH))        {vp[vn] = "隣接テレポート";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_TELE_HI_APPROACH))        {vp[vn] = "視界外隣接テレポート";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_MAELSTROM))        {vp[vn] = "メイルシュトロム";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_ALARM))        {vp[vn] = "警報";color[vn++] = TERM_L_RED;}
#else
if (flags9 & (RF9_ACID_STORM))        {vp[vn] = "produce acid storms";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_THUNDER_STORM))        {vp[vn] = "produce lightning storms";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_FIRE_STORM))        {vp[vn] = "produce fire storms";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_ICE_STORM))        {vp[vn] = "produce ice storms";color[vn++] = TERM_L_WHITE;}
if (flags9 & (RF9_TOXIC_STORM))        {vp[vn] = "produce toxic storms";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_BA_POLLUTE))        {vp[vn] = "produce pollution balls";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_BA_DISI))        {vp[vn] = "cause disintegration";color[vn++] = TERM_SLATE;}
if (flags9 & (RF9_BA_HOLY))        {vp[vn] = "produce holy orbs";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BA_METEOR))        {vp[vn] = "invoke meteor strikes";color[vn++] = TERM_L_DARK;}
if (flags9 & (RF9_BA_DISTORTION))        {vp[vn] = "cause space distortions";color[vn++] = TERM_L_BLUE;}
if (flags9 & (RF9_PUNISH_1))        {vp[vn] = "perform exorcism (1)";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_2))        {vp[vn] = "perform exorcism (2)";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_3))        {vp[vn] = "perform exorcism (3)";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_4))        {vp[vn] = "perform exorcism (4)";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BO_HOLY))        {vp[vn] = "produce holy bolts";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_GIGANTIC_LASER))        {vp[vn] = "fire giant lasers";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_BO_SOUND))        {vp[vn] = "shoot musical notes";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_S_ANIMAL))        {vp[vn] = "summon animals";color[vn++] = TERM_L_UMBER;}
if (flags9 & (RF9_BEAM_LITE))        {vp[vn] = "fire lasers";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_SONICWAVE))        {vp[vn] = "fire supersonic waves";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_HELLFIRE))        {vp[vn] = "invoke hellfire";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_HOLYFIRE))        {vp[vn] = "invoke holy fire";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_FINALSPARK))        {vp[vn] = "fire Final Spark";color[vn++] = TERM_WHITE;}
if (flags9 & (RF9_TORNADO))        {vp[vn] = "invoke tornadoes";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_DESTRUCTION))        {vp[vn] = "cause *Destruction*";color[vn++] = TERM_RED;}
if (flags9 & (RF9_S_DEITY))        {vp[vn] = "summon deities";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_S_HI_DEMON))        {vp[vn] = "summon Greater Demons";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_S_KWAI))        {vp[vn] = "summon youkai";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_TELE_APPROACH))        {vp[vn] = "teleport towards target";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_TELE_HI_APPROACH))        {vp[vn] = "teleport towards target (while out of sight)";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_MAELSTROM))        {vp[vn] = "produce maelstroms";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_ALARM))        {vp[vn] = "sound an alarm and haste allies";color[vn++] = TERM_L_RED;}
#endif


	/* Describe spells */
	if (vn)
	{
		/* Note magic */
		magic = TRUE;

		/* Intro */
		if (breath)
		{
#ifdef JP
			hooked_roff("、なおかつ");
#else
			hooked_roff(", and is also");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s is", wd_he[msex]));
#endif

		}

#ifdef JP
		/* Adverb */
		if (flags2 & (RF2_SMART_EX)) hook_c_roff(TERM_ORANGE, "極めて的確に");
		else if (flags2 & (RF2_SMART)) hook_c_roff(TERM_YELLOW, "的確に");

		/* Verb Phrase */
		hooked_roff("魔法を使うことができ、");
#else
		/* Verb Phrase */
		hooked_roff(" magical, casting spells");

		/* Adverb */
		if (flags2 & (RF2_SMART_EX)) hook_c_roff(TERM_ORANGE, " extremely intelligently");
		else if (flags2 & RF2_SMART) hook_c_roff(TERM_YELLOW, " intelligently");
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("、");
#else
			if (n == 0) hooked_roff(" which ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("の呪文を唱えることがある");
#endif
	}


	/* End the sentence about inate/other spells */
	if (breath || magic)
	{
		/* Total casting */
		m = r_ptr->r_cast_spell;

		/* Average frequency */
		n = r_ptr->freq_spell;

		/* Describe the spell frequency */
		if (m > 100 || know_everything)
		{
#ifdef JP
			hooked_roff(format("(確率:1/%d)", 100 / n));
#else
			hooked_roff(format("; 1 time in %d", 100 / n));
#endif

		}

		/* Guess at the frequency */
		else if (m)
		{
			n = ((n + 9) / 10) * 10;
#ifdef JP
			hooked_roff(format("(確率:約1/%d)", 100 / n));
#else
			hooked_roff(format("; about 1 time in %d", 100 / n));
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif

	}

	/* Describe monster "toughness" */
	if (know_armour(r_idx))
	{
		/* Armor */
#ifdef JP
		hooked_roff(format("%^sは AC%d の防御力と", wd_he[msex], r_ptr->ac));
#else
		hooked_roff(format("%^s has an armor rating of %d", wd_he[msex], r_ptr->ac));
#endif

		/* Maximized hitpoints */
		if ((flags1 & RF1_FORCE_MAXHP) || (r_ptr->hside == 1))
		{
			u32b hp = r_ptr->hdice * (nightmare ? 2 : 1) * r_ptr->hside;

			if(difficulty == DIFFICULTY_EASY) hp = MAX(1,hp / 2);
			else if(difficulty == DIFFICULTY_NORMAL) hp = MAX(1,hp * 3 / 4);


#ifdef JP
			hooked_roff(format(" %d の体力がある。", (s16b)MIN(30000, hp)));
#else
			hooked_roff(format(" and a life rating of %d.  ", (s16b)MIN(30000, hp)));
#endif
		}

		/* Variable hitpoints */
		else
		{
			int hd = r_ptr->hdice * (nightmare ? 2 : 1);
			if(difficulty == DIFFICULTY_EASY) hd = MAX(1,hd / 2);
			else if(difficulty == DIFFICULTY_NORMAL) hd = MAX(1,hd * 3 / 4);
#ifdef JP
			hooked_roff(format(" %dd%d の体力がある。", hd , r_ptr->hside));
#else
			hooked_roff(format(" and a life rating of %dd%d.  ", hd , r_ptr->hside));
#endif
		}

		roff_new_section();
	}



	/* Collect special abilities. */
	vn = 0;
#ifdef JP
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "ダンジョンを照らす";     color[vn++] = TERM_WHITE; }
#else
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "illuminate the dungeon"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "ダンジョンを暗くする";   color[vn++] = TERM_L_DARK; }
#else
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "darken the dungeon";     color[vn++] = TERM_L_DARK; }
#endif

#ifdef JP
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "ドアを開ける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "open doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "ドアを打ち破る"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "bash down doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "壁をすり抜ける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "pass through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "壁を掘り進む"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "bore through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "弱いモンスターを押しのける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "push past weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "弱いモンスターを倒す"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "destroy weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "アイテムを拾う"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "pick up objects"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "アイテムを壊す"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "destroy objects"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_TELE_AFTER) { vp[vn] = "テレポートを追尾する"; color[vn++] = TERM_ORANGE; }
#else
	if (flags2 & RF2_TELE_AFTER) { vp[vn] = "follow teleporting enemies"; color[vn++] = TERM_WHITE; }
#endif





	/* Describe special abilities. */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_AND);
				hook_c_roff(color[n], jverb_buf);
				hooked_roff("、");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			if (n == 0) hooked_roff(" can ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" and ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("ことができる。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif
	}


	/* Describe special abilities. */
	if (flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2))
	{
#ifdef JP
		hooked_roff(format("%^sは光っている。", wd_he[msex]));
#else
		hooked_roff(format("%^s is shining.  ", wd_he[msex]));
#endif

	}
	if (flags7 & (RF7_SELF_DARK_1 | RF7_SELF_DARK_2))
	{
#ifdef JP
		hook_c_roff(TERM_L_DARK, format("%^sは暗黒に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_DARK, format("%^s is surrounded by darkness.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_INVISIBLE)
	{
#ifdef JP
		hooked_roff(format("%^sは透明で目に見えない。", wd_he[msex]));
#else
		hooked_roff(format("%^s is invisible.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_COLD_BLOOD)
	{
#ifdef JP
		hooked_roff(format("%^sは赤外線では感知できない。", wd_he[msex]));
#else
		hooked_roff(format("%^s is cold blooded.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_EMPTY_MIND)
	{
#ifdef JP
		hooked_roff(format("%^sはテレパシーでは感知できない。", wd_he[msex]));
#else
		hooked_roff(format("%^s is not detected by telepathy.  ", wd_he[msex]));
#endif

	}
	else if (flags2 & RF2_WEIRD_MIND)
	{
#ifdef JP
		hooked_roff(format("%^sはまれにテレパシーで感知できる。", wd_he[msex]));
#else
		hooked_roff(format("%^s is rarely detected by telepathy.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_MULTIPLY)
	{
#ifdef JP
		hook_c_roff(TERM_L_UMBER, format("%^sは爆発的に増殖する。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_UMBER, format("%^s breeds explosively.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_REGENERATE)
	{
#ifdef JP
		hook_c_roff(TERM_L_WHITE, format("%^sは素早く体力を回復する。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_WHITE, format("%^s regenerates quickly.  ", wd_he[msex]));
#endif

	}
	if (flags7 & RF7_RIDING)
	{
#ifdef JP
		hook_c_roff(TERM_SLATE, format("%^sに乗ることができる。", wd_he[msex]));
#else
		hook_c_roff(TERM_SLATE, format("%^s is suitable for riding.  ", wd_he[msex]));
#endif

	}

#ifndef JP
    if (flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2 | RF7_SELF_DARK_1 | RF7_SELF_DARK_2 | RF7_RIDING) ||
        flags2 & (RF2_INVISIBLE | RF2_COLD_BLOOD | RF2_EMPTY_MIND | RF2_WEIRD_MIND | RF2_MULTIPLY | RF2_REGENERATE))
    {
        roff_new_section();
    }
#endif // JP


	/* Collect susceptibilities */
	vn = 0;
#ifdef JP
///mod131231 モンスターフラグ変更 岩石妖怪弱点RF3からRFRへ
	if (flagsr & RFR_HURT_ROCK) {vp[vn] = "岩を除去するもの";color[vn++] = TERM_UMBER;}
#else
	if (flagsr & RFR_HURT_ROCK) {vp[vn] = "rock remover";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
///mod131231 モンスターフラグ変更
	if (flagsr & RFR_HURT_LITE) {vp[vn] = "明るい光";color[vn++] = TERM_YELLOW;}
	//if (flags3 & RF3_HURT_LITE) {vp[vn] = "明るい光";color[vn++] = TERM_YELLOW;}
#else
	if (flagsr & RFR_HURT_LITE) {vp[vn] = "bright light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
///mod131231 モンスターフラグ変更 火炎弱点RF3からRFRへ
	if (flagsr & RFR_HURT_FIRE) {vp[vn] = "炎";color[vn++] = TERM_RED;}
#else
	if (flagsr & RFR_HURT_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

///mod131231 モンスターフラグ変更 冷気弱点RF3からRFRへ
#ifdef JP
	if (flagsr & RFR_HURT_COLD) {vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flagsr & RFR_HURT_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif
#ifdef JP
	if (flagsr & RFR_HURT_ELEC) {vp[vn] = "電撃";color[vn++] = TERM_BLUE;}
	if (flagsr & RFR_HURT_WATER) {vp[vn] = "水";color[vn++] = TERM_L_BLUE;}
	///pend 地獄の業火弱点は隠す？
	//if (flagsr & RFR_HURT_HELL) {vp[vn] = "地獄の業火";color[vn++] = TERM_L_DARK;}
	if (flagsr & RFR_HURT_HOLY) {vp[vn] = "破邪";color[vn++] = TERM_YELLOW;}

	if (flags3 & RF3_HANIWA) { vp[vn] = "衝撃"; color[vn++] = TERM_WHITE; }

#else
	if (flagsr & RFR_HURT_ELEC) {vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
	if (flagsr & RFR_HURT_WATER) {vp[vn] = "water";color[vn++] = TERM_L_BLUE;}
	///pend 地獄の業火弱点は隠す？
	//if (flagsr & RFR_HURT_HELL) {vp[vn] = "地獄の業火";color[vn++] = TERM_L_DARK;}
	if (flagsr & RFR_HURT_HOLY) {vp[vn] = "holiness";color[vn++] = TERM_YELLOW;}

	if (flags3 & RF3_HANIWA) { vp[vn] = "impact"; color[vn++] = TERM_WHITE; }
#endif

	/* Describe susceptibilities */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sには", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("や");
#else
			if (n == 0) hooked_roff(" is hurt by ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("でダメージを与えられる。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif

	}


	/* Collect immunities */
	vn = 0;
#ifdef JP
	if (flagsr & RFR_IM_ACID) {vp[vn] = "酸";color[vn++] = TERM_GREEN;}
#else
	if (flagsr & RFR_IM_ACID) {vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_ELEC) {vp[vn] = "稲妻";color[vn++] = TERM_BLUE;}
#else
	if (flagsr & RFR_IM_ELEC) {vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_FIRE) {vp[vn] = "炎";color[vn++] = TERM_RED;}
#else
	if (flagsr & RFR_IM_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_COLD) {vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flagsr & RFR_IM_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_POIS) {vp[vn] = "毒";color[vn++] = TERM_L_GREEN;}
#else
	if (flagsr & RFR_IM_POIS) {vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif


	/* Collect resistances */
#ifdef JP
	if (flagsr & RFR_RES_LITE) {vp[vn] = "閃光";color[vn++] = TERM_YELLOW;}
#else
	if (flagsr & RFR_RES_LITE) {vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_DARK) {vp[vn] = "暗黒";color[vn++] = TERM_L_DARK;}
#else
	if (flagsr & RFR_RES_DARK) {vp[vn] = "dark";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_NETH) {vp[vn] = "地獄";color[vn++] = TERM_L_DARK;}
#else
	if (flagsr & RFR_RES_NETH) {vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_WATE) {vp[vn] = "水";color[vn++] = TERM_BLUE;}
#else
	if (flagsr & RFR_RES_WATE) {vp[vn] = "water";color[vn++] = TERM_BLUE;}
#endif

///mod131231 プラズマ耐性変更
/*
#ifdef JP
	if (flagsr & RFR_RES_PLAS) {vp[vn] = "プラズマ";color[vn++] = TERM_L_RED;}
#else
	if (flagsr & RFR_RES_PLAS) {vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif
*/

#ifdef JP
	if (flagsr & RFR_RES_SHAR) {vp[vn] = "破片";color[vn++] = TERM_L_UMBER;}
#else
	if (flagsr & RFR_RES_SHAR) {vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_SOUN) {vp[vn] = "轟音";color[vn++] = TERM_ORANGE;}
#else
	if (flagsr & RFR_RES_SOUN) {vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_CHAO) {vp[vn] = "カオス";color[vn++] = TERM_VIOLET;}
#else
	if (flagsr & RFR_RES_CHAO) {vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif


#ifdef JP
	if (flagsr & RFR_RES_HOLY) {vp[vn] = "破邪";color[vn++] = TERM_L_WHITE;}
#else
	if (flagsr & RFR_RES_HOLY) {vp[vn] = "holy energy";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_DISE) {vp[vn] = "劣化";color[vn++] = TERM_VIOLET;}
#else
	if (flagsr & RFR_RES_DISE) {vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

///フォース耐性削除
/*
#ifdef JP
	if (flagsr & RFR_RES_WALL) {vp[vn] = "フォース";color[vn++] = TERM_UMBER;}
#else
	if (flagsr & RFR_RES_WALL) {vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif
*/
#ifdef JP
	if (flagsr & RFR_RES_INER) {vp[vn] = "遅鈍";color[vn++] = TERM_SLATE;}
#else
	if (flagsr & RFR_RES_INER) {vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_TIME) {vp[vn] = "時空攻撃";color[vn++] = TERM_L_BLUE;}
#else
	if (flagsr & RFR_RES_TIME) {vp[vn] = "space-time attacks";color[vn++] = TERM_L_BLUE;}
#endif
///mod131231 重力耐性削除
/*
#ifdef JP
	if (flagsr & RFR_RES_GRAV) {vp[vn] = "重力";color[vn++] = TERM_SLATE;}
#else
	if (flagsr & RFR_RES_GRAV) {vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif
*/
#ifdef JP
	if (flagsr & RFR_RES_ALL) {vp[vn] = "あらゆる攻撃";color[vn++] = TERM_YELLOW;}
#else
	if (flagsr & RFR_RES_ALL) {vp[vn] = "any sort of attacks";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if ((flagsr & RFR_RES_TELE) && !(r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "テレポート";color[vn++] = TERM_ORANGE;}
#else
	if ((flagsr & RFR_RES_TELE) && !(r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "teleportation";color[vn++] = TERM_ORANGE;}
#endif


	/* Describe immunities and resistances */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("と");
#else
			if (n == 0) hooked_roff(" resists ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("の耐性を持っている。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif

	}


	if ((r_ptr->r_xtra1 & MR1_SINKA) || know_everything)
	{
		if (r_ptr->next_r_idx)
		{
#ifdef JP
			hooked_roff(format("%^sは経験を積むと、", wd_he[msex]));
#else
			hooked_roff(format("%^s will evolve into ", wd_he[msex]));
#endif
			hook_c_roff(TERM_YELLOW, format("%s", r_name+r_info[r_ptr->next_r_idx].name));
#ifdef JP
			hooked_roff(format("に進化する。"));
#else
			hooked_roff(format(" when %s gets enough experience.  ", wd_he[msex]));
			roff_new_section();
#endif
		}
		else if (!(r_ptr->flags1 & RF1_UNIQUE))
		{
#ifdef JP
			hooked_roff(format("%sは進化しない。", wd_he[msex]));
#else
			hooked_roff(format("%^s won't evolve.  ", wd_he[msex]));
			roff_new_section();
#endif
		}

	}

	/* Collect non-effects */
	vn = 0;
#ifdef JP
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "朦朧としない";color[vn++] = TERM_ORANGE;}
#else
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "stunned";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "恐怖を感じない";color[vn++] = TERM_SLATE;}
#else
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "frightened";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "混乱しない";color[vn++] = TERM_L_UMBER;}
#else
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "confused";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "眠らされない";color[vn++] = TERM_BLUE;}
#else
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "slept";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if ((flagsr & RFR_RES_TELE) && (r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "テレポートされない";color[vn++] = TERM_ORANGE;}
#else
	if ((flagsr & RFR_RES_TELE) && (r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "teleported";color[vn++] = TERM_ORANGE;}
#endif

	/* Describe non-effects */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("し、");
#else
			if (n == 0) hooked_roff(" cannot be ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif

	}


	/* Do we know how aware it is? */
	if ((((int)r_ptr->r_wake * (int)r_ptr->r_wake) > r_ptr->sleep) ||
		  (r_ptr->r_ignore == MAX_UCHAR) ||
	    (r_ptr->sleep == 0 && r_ptr->r_tkills >= 10) || know_everything)
	{
		cptr act;

		if (r_ptr->sleep > 200)
		{
#ifdef JP
			act = "を無視しがちであるが";
#else
			act = "prefers to ignore";
#endif

		}
		else if (r_ptr->sleep > 95)
		{
#ifdef JP
			act = "に対してほとんど注意を払わないが";
#else
			act = "pays very little attention to";
#endif

		}
		else if (r_ptr->sleep > 75)
		{
#ifdef JP
			act = "に対してあまり注意を払わないが";
#else
			act = "pays little attention to";
#endif

		}
		else if (r_ptr->sleep > 45)
		{
#ifdef JP
			act = "を見過ごしがちであるが";
#else
			act = "tends to overlook";
#endif

		}
		else if (r_ptr->sleep > 25)
		{
#ifdef JP
			act = "をほんの少しは見ており";
#else
			act = "takes quite a while to see";
#endif

		}
		else if (r_ptr->sleep > 10)
		{
#ifdef JP
			act = "をしばらくは見ており";
#else
			act = "takes a while to see";
#endif

		}
		else if (r_ptr->sleep > 5)
		{
#ifdef JP
			act = "を幾分注意深く見ており";
#else
			act = "is fairly observant of";
#endif

		}
		else if (r_ptr->sleep > 3)
		{
#ifdef JP
			act = "を注意深く見ており";
#else
			act = "is observant of";
#endif

		}
		else if (r_ptr->sleep > 1)
		{
#ifdef JP
			act = "をかなり注意深く見ており";
#else
			act = "is very observant of";
#endif

		}
		else if (r_ptr->sleep > 0)
		{
#ifdef JP
			act = "を警戒しており";
#else
			act = "is vigilant for";
#endif

		}
		else
		{
#ifdef JP
			act = "をかなり警戒しており";
#else
			act = "is ever vigilant for";
#endif

		}

#ifdef JP
		hooked_roff(format("%^sは侵入者%s、 %d フィート先から侵入者に気付くことがある。",
		     wd_he[msex], act, 10 * r_ptr->aaf));
#else
		hooked_roff(format("%^s %s intruders, which %s may notice from %d feet.  ",
			    wd_he[msex], act, wd_he[msex], 10 * r_ptr->aaf));
        roff_new_section();
#endif

	}


	/* Drops gold and/or items */
	if (drop_gold || drop_item)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s may carry", wd_he[msex]));

		/* No "n" needed */
		sin = FALSE;
#endif


		/* Count maximum drop */
		n = MAX(drop_gold, drop_item);

		/* One drop (may need an "n") */
		if (n == 1)
		{
#ifdef JP
			hooked_roff("一つの");
#else
			hooked_roff(" a");
			sin = TRUE;
#endif
		}

		/* Two drops */
		else if (n == 2)
		{
#ifdef JP
			hooked_roff("一つか二つの");
#else
			hooked_roff(" one or two");
#endif

		}

		/* Many drops */
		else
		{
#ifdef JP
			hooked_roff(format(" %d 個までの", n));
#else
			hooked_roff(format(" up to %d", n));
#endif

		}


		/* Great */
		if (flags1 & RF1_DROP_GREAT)
		{
#ifdef JP
			p = "特別な";
#else
			p = " exceptional";
#endif

		}

		/* Good (no "n" needed) */
		else if (flags1 & RF1_DROP_GOOD)
		{
#ifdef JP
			p = "上質な";
#else
			p = " good";
			sin = FALSE;
#endif
		}

		/* Okay */
		else
		{
			p = NULL;
		}


		/* Objects */
		if (drop_item)
		{
			/* Handle singular "an" */
#ifndef JP
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "object(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("アイテム");
#else
			hooked_roff(" object");
			if (n != 1) hooked_roff("s");
#endif


			/* Conjunction replaces variety, if needed for "gold" below */
#ifdef JP
			p = "や";
#else
			p = " or";
#endif

		}

		/* Treasures */
		if (drop_gold)
		{
#ifndef JP
			/* Cancel prefix */
			if (!p) sin = FALSE;

			/* Handle singular "an" */
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "treasure(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("財宝");
#else
			hooked_roff(" treasure");
			if (n != 1) hooked_roff("s");
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("を持っていることがある。");
#else
		hooked_roff(".  ");
		roff_new_section();
#endif

	}


	/* Count the number of "known" attacks */
	for (n = 0, m = 0; m < 4; m++)
	{
		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;
		if (r_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Count known attacks */
		if (r_ptr->r_blows[m] || know_everything) n++;
	}

	/* Examine (and count) the actual attacks */
	for (r = 0, m = 0; m < 4; m++)
	{
		int method, effect, d1, d2;

		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;
		if (r_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Skip unknown attacks */
		if (!r_ptr->r_blows[m] && !know_everything) continue;

		/* Extract the attack info */
		method = r_ptr->blow[m].method;
		effect = r_ptr->blow[m].effect;
		d1 = r_ptr->blow[m].d_dice;
		d2 = r_ptr->blow[m].d_side;

		/* No method yet */
		p = NULL;

		/* Acquire the method */
		switch (method)
		{
#ifdef JP
case RBM_HIT:		p = "攻撃する"; break;
#else
			case RBM_HIT:		p = "hit"; break;
#endif

#ifdef JP
case RBM_TOUCH:		p = "触る"; break;
#else
			case RBM_TOUCH:		p = "touch"; break;
#endif

#ifdef JP
case RBM_PUNCH:		p = "殴る"; break;
#else
			case RBM_PUNCH:		p = "punch"; break;
#endif

#ifdef JP
case RBM_KICK:		p = "蹴る"; break;
#else
			case RBM_KICK:		p = "kick"; break;
#endif

#ifdef JP
case RBM_CLAW:		p = "ひっかく"; break;
#else
			case RBM_CLAW:		p = "claw"; break;
#endif

#ifdef JP
case RBM_BITE:		p = "噛む"; break;
#else
			case RBM_BITE:		p = "bite"; break;
#endif

#ifdef JP
case RBM_STING:		p = "刺す"; break;
#else
			case RBM_STING:		p = "stab"; break;
#endif

#ifdef JP
case RBM_SLASH:		p = "斬る"; break;
#else
			case RBM_SLASH:		p = "slash"; break;
#endif

#ifdef JP
case RBM_BUTT:		p = "角で突く"; break;
#else
			case RBM_BUTT:		p = "butt"; break;
#endif

#ifdef JP
case RBM_CRUSH:		p = "体当たりする"; break;
#else
			case RBM_CRUSH:		p = "crush"; break;
#endif

#ifdef JP
case RBM_ENGULF:	p = "飲み込む"; break;
#else
			case RBM_ENGULF:	p = "engulf"; break;
#endif

#ifdef JP
case RBM_CHARGE: 	p = "請求書をよこす"; break;
#else
			case RBM_CHARGE: 	p = "give an invoice";   break;
#endif

#ifdef JP
case RBM_CRAWL:		p = "体の上を這い回る"; break;
#else
			case RBM_CRAWL:		p = "crawl on you"; break;
#endif

#ifdef JP
case RBM_DROOL:		p = "よだれをたらす"; break;
#else
			case RBM_DROOL:		p = "drool on you"; break;
#endif

#ifdef JP
case RBM_SPIT:		p = "つばを吐く"; break;
#else
			case RBM_SPIT:		p = "spit"; break;
#endif

#ifdef JP
case RBM_EXPLODE:	p = "爆発する"; break;
#else
			case RBM_EXPLODE:	p = "explode"; break;
#endif

#ifdef JP
case RBM_GAZE:		p = "にらむ"; break;
#else
			case RBM_GAZE:		p = "gaze"; break;
#endif

#ifdef JP
case RBM_WAIL:		p = "泣き叫ぶ"; break;
#else
			case RBM_WAIL:		p = "wail"; break;
#endif

#ifdef JP
case RBM_SPORE:		p = "胞子を飛ばす"; break;
#else
			case RBM_SPORE:		p = "release spores"; break;
#endif

			case RBM_DANCE:		p = _("踊る", "dance"); break;
#ifdef JP
case RBM_BEG:		p = "金をせがむ"; break;
#else
			case RBM_BEG:		p = "beg"; break;
#endif

#ifdef JP
case RBM_INSULT:	p = "侮辱する"; break;
#else
			case RBM_INSULT:	p = "insult"; break;
#endif

#ifdef JP
case RBM_MOAN:		p = "うめく"; break;
#else
			case RBM_MOAN:		p = "moan"; break;
#endif

#ifdef JP
case RBM_SHOW:  	p = "歌う"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

#ifdef JP
case RBM_SPEAR:  	p = "突く"; break;
#else
			case RBM_SPEAR:  	p = "thrust"; break;
#endif
#ifdef JP
case RBM_BOOK:  	p = "本で殴る"; break;
#else
			case RBM_BOOK:  	p = "bludgeon with a book"; break;
#endif
#ifdef JP
case RBM_MOFUMOFU:  	p = "尻尾でモフモフする"; break;
#else
			case RBM_MOFUMOFU:  	p = "envelop with fluffy tail"; break;
#endif
#ifdef JP
case RBM_SQUEEZE:  	p = "締め付ける"; break;
#else
			case RBM_SQUEEZE:  	p = "squeeze"; break;
#endif
#ifdef JP
case RBM_DRAG:  	p = "引きずり込む"; break;
#else
			case RBM_DRAG:  	p = "drag"; break;
#endif

#ifdef JP
case RBM_INJECT:  	p = "注射する"; break;
#else
			case RBM_INJECT:  	p = "inject"; break;
#endif

#ifdef JP
case RBM_SMILE:  	p = "微笑む"; break;
#else
			case RBM_SMILE:  	p = "smile"; break;
#endif
#ifdef JP
case RBM_HEADBUTT:  	p = "頭突きする"; break;
#else
			case RBM_HEADBUTT:  	p = "headbutt"; break;
#endif
#ifdef JP
case RBM_FLASH:  	p = "光る"; break;
#else
			case RBM_FLASH:  	p = "flash"; break;
#endif
#ifdef JP
case RBM_HOWL:  	p = "吠える"; break;
#else
			case RBM_HOWL:  	p = "howl"; break;
#endif
#ifdef JP
case RBM_WHISPER:  	p = "ささやく"; break;
#else
			case RBM_WHISPER:  	p = "whisper"; break;
#endif
#ifdef JP
case RBM_PRESS:  	p = "押し潰す"; break;
#else
			case RBM_PRESS:  	p = "press"; break;
#endif
#ifdef JP
case RBM_POINT:  	p = "指差す"; break;
#else
			case RBM_POINT:  	p = "point"; break;
#endif
#ifdef JP
case RBM_STRIKE:  	p = "打ち据える"; break;
#else
			case RBM_STRIKE:  	p = "strike"; break;
#endif
#ifdef JP
case RBM_PLAY:  	p = "演奏する"; break;
#else
			case RBM_PLAY:  	p = "play"; break;
#endif

#ifdef JP
			case RBM_SMOKE:  	p = "煙を吹きかける"; break;
#else
			case RBM_SMOKE:  	p = "blow smoke"; break;
#endif
		}


		/* Default effect */
		q = NULL;

		/* Acquire the effect */
		switch (effect)
		{
#ifdef JP
///msg131230 SUPERHURTの記述変更
case RBE_SUPERHURT:	q = "強烈な攻撃を加える"; break;

case RBE_HURT:    	q = "攻撃する"; break;
#else
			case RBE_SUPERHURT: q = "powerfully attack"; break;
			case RBE_HURT:    	q = "attack"; break;
#endif

#ifdef JP
case RBE_POISON:  	q = "毒をくらわす"; break;
#else
			case RBE_POISON:  	q = "poison"; break;
#endif

#ifdef JP
case RBE_UN_BONUS:	q = "劣化させる"; break;
#else
			case RBE_UN_BONUS:	q = "disenchant"; break;
#endif

#ifdef JP
case RBE_UN_POWER:	q = "充填魔力を吸収する"; break;
#else
			case RBE_UN_POWER:	q = "drain charges"; break;
#endif

#ifdef JP
case RBE_EAT_GOLD:	q = "金を盗む"; break;
#else
			case RBE_EAT_GOLD:	q = "steal gold"; break;
#endif

#ifdef JP
case RBE_EAT_ITEM:	q = "アイテムを盗む"; break;
#else
			case RBE_EAT_ITEM:	q = "steal items"; break;
#endif

#ifdef JP
case RBE_EAT_FOOD:	q = "あなたの食料を食べる"; break;
#else
			case RBE_EAT_FOOD:	q = "eat your food"; break;
#endif

#ifdef JP
case RBE_EAT_LITE:	q = "明かりを吸収する"; break;
#else
			case RBE_EAT_LITE:	q = "absorb light"; break;
#endif

#ifdef JP
case RBE_ACID:    	q = "酸を飛ばす"; break;
#else
			case RBE_ACID:    	q = "shoot acid"; break;
#endif

#ifdef JP
case RBE_ELEC:    	q = "感電させる"; break;
#else
			case RBE_ELEC:    	q = "electrocute"; break;
#endif

#ifdef JP
case RBE_FIRE:    	q = "燃やす"; break;
#else
			case RBE_FIRE:    	q = "burn"; break;
#endif

#ifdef JP
case RBE_COLD:    	q = "凍らせる"; break;
#else
			case RBE_COLD:    	q = "freeze"; break;
#endif

#ifdef JP
case RBE_BLIND:   	q = "盲目にする"; break;
#else
			case RBE_BLIND:   	q = "blind"; break;
#endif

#ifdef JP
case RBE_CONFUSE: 	q = "混乱させる"; break;
#else
			case RBE_CONFUSE: 	q = "confuse"; break;
#endif

#ifdef JP
case RBE_TERRIFY: 	q = "恐怖させる"; break;
#else
			case RBE_TERRIFY: 	q = "terrify"; break;
#endif

#ifdef JP
case RBE_PARALYZE:	q = "麻痺させる"; break;
#else
			case RBE_PARALYZE:	q = "paralyze"; break;
#endif

#ifdef JP
case RBE_LOSE_STR:	q = "腕力を減少させる"; break;
#else
			case RBE_LOSE_STR:	q = "reduce strength"; break;
#endif

#ifdef JP
case RBE_LOSE_INT:	q = "知能を減少させる"; break;
#else
			case RBE_LOSE_INT:	q = "reduce intelligence"; break;
#endif

#ifdef JP
case RBE_LOSE_WIS:	q = "賢さを減少させる"; break;
#else
			case RBE_LOSE_WIS:	q = "reduce wisdom"; break;
#endif

#ifdef JP
case RBE_LOSE_DEX:	q = "器用さを減少させる"; break;
#else
			case RBE_LOSE_DEX:	q = "reduce dexterity"; break;
#endif

#ifdef JP
case RBE_LOSE_CON:	q = "耐久力を減少させる"; break;
#else
			case RBE_LOSE_CON:	q = "reduce constitution"; break;
#endif

#ifdef JP
case RBE_LOSE_CHR:	q = "魅力を減少させる"; break;
#else
			case RBE_LOSE_CHR:	q = "reduce charisma"; break;
#endif

#ifdef JP
case RBE_LOSE_ALL:	q = "全ステータスを減少させる"; break;
#else
			case RBE_LOSE_ALL:	q = "reduce all stats"; break;
#endif

#ifdef JP
case RBE_SHATTER:	q = "粉砕する"; break;
#else
			case RBE_SHATTER:	q = "shatter"; break;
#endif

#ifdef JP
case RBE_EXP_10:	q = "経験値を減少(10d6+)させる"; break;
#else
			case RBE_EXP_10:	q = "lower experience (by 10d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_20:	q = "経験値を減少(20d6+)させる"; break;
#else
			case RBE_EXP_20:	q = "lower experience (by 20d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_40:	q = "経験値を減少(40d6+)させる"; break;
#else
			case RBE_EXP_40:	q = "lower experience (by 40d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_80:	q = "経験値を減少(80d6+)させる"; break;
#else
			case RBE_EXP_80:	q = "lower experience (by 80d6+)"; break;
#endif

#ifdef JP
case RBE_DISEASE:	q = "病気にする"; break;
#else
			case RBE_DISEASE:	q = "disease"; break;
#endif

#ifdef JP
case RBE_TIME:      q = "時間を逆戻りさせる"; break;
#else
			case RBE_TIME:      q = "time"; break;
#endif

#ifdef JP
case RBE_EXP_VAMP:  q = "生命力を吸収する"; break;
#else
			case RBE_EXP_VAMP:  q = "drain life force"; break;
#endif

#ifdef JP
case RBE_DR_MANA:  q = "魔力を奪う"; break;
#else
			case RBE_DR_MANA:  q = "drain mana force"; break;
#endif

#ifdef JP
			case RBE_INERTIA:  q = "減速させる"; break;
			case RBE_STUN:     q = "朦朧とさせる"; break;
#else
			case RBE_INERTIA:  q = "slow"; break;
			case RBE_STUN:     q = "stun"; break;
#endif

///mod131230 モンスター攻撃エフェクト追加
#ifdef JP
			case RBE_CHAOS:  q = "カオスで攻撃する"; break;
			case RBE_ELEMENT:     q = "エレメントで攻撃する"; break;
			case RBE_MUTATE:     q = "突然変異を誘発する"; break;
			case RBE_SMITE:     q = "浄化する"; break;
			case RBE_DROWN:     q = "溺れさせる"; break;
			case RBE_KILL:     q = "死に誘う"; break;
			case RBE_CURSE:     q = "呪う"; break;
			case RBE_PHOTO:     q = "写真撮影する"; break;
			case RBE_MELT:     q = "消化する"; break;
			case RBE_BLEED:     q = "怪我を負わせる"; break;
			case RBE_INSANITY:     q = "狂気をもたらす"; break;
			case RBE_HUNGER:     q = "飢餓をもたらす"; break;
#else
            case RBE_CHAOS:  q = "attack with chaos"; break;
			case RBE_ELEMENT:     q = "attack with elements"; break;
			case RBE_MUTATE:     q = "cause mutations"; break;
			case RBE_SMITE:     q = "purify"; break;
			case RBE_DROWN:     q = "drown"; break;
			case RBE_KILL:     q = "invite to death"; break;
			case RBE_CURSE:     q = "curse"; break;
			case RBE_PHOTO:     q = "take a photo"; break;
			case RBE_MELT:     q = "melt"; break;
			case RBE_BLEED:     q = "wound"; break;
			case RBE_INSANITY:     q = "cause insanity"; break;
			case RBE_HUNGER:     q = "cause hunger"; break;
			default: q = "OTHER"; break;
#endif
		}


#ifdef JP
		if ( r == 0 ) hooked_roff( format("%^sは", wd_he[msex]) );

		/***若干表現を変更 ita ***/

			/* Describe damage (if known) */
		if (d1 && d2 && (know_everything || know_damage(r_idx, m)))
		  {

		    /* Display the damage */
		    hooked_roff(format(" %dd%d ", d1, d2));
		    hooked_roff("のダメージで");
		  }
		/* Hack -- force a method */
		if (!p) p = "何か奇妙なことをする";

		/* Describe the method */
		/* XXしてYYし/XXしてYYする/XXし/XXする */

		///mod131231 モンスター攻撃の思い出から「殴って」を削除してみる
		if(method != RBM_HIT)
		{

			if(q) jverb( p ,jverb_buf, JVERB_TO);
			else if(r!=n-1) jverb( p ,jverb_buf, JVERB_AND);
			else strcpy(jverb_buf, p);

			hooked_roff(jverb_buf);
		}
		/* Describe the effect (if any) */
		if (q)
		{
		  if(r!=n-1) jverb( q,jverb_buf, JVERB_AND);
		  else strcpy(jverb_buf,q);
		  hooked_roff(jverb_buf);
		}
		if(r!=n-1) hooked_roff("、");
#else
		/* Introduce the attack description */
#if 0
		if (!r)
		{
			hooked_roff(format("%^s can ", wd_he[msex]));
		}
		else if (r < n-1)
		{
			hooked_roff(", ");
		}
		else
		{
			hooked_roff(", and ");
		}


		/* Hack -- force a method */
		if (!p) p = "do something weird";

		/* Describe the method */
		hooked_roff(p);


		/* Describe the effect (if any) */
		if (q)
		{
			/* Describe the attack type */
			hooked_roff(" to ");
			hooked_roff(q);

			/* Describe damage (if known) */
			if (d1 && d2 && (know_everything || know_damage(r_idx, m)))
			{
				/* Display the damage */
				hooked_roff(" with damage");
				hooked_roff(format(" %dd%d", d1, d2));
			}
		}
#endif
        {
            cptr blow_desc;

            p = get_blow_method_desc(method);
            q = get_blow_effect_desc(effect);
            blow_desc = "";

            if (!r)
            {
                hooked_roff(format("%^s has the following melee attacks: ", wd_he[msex]));
                hooked_roff("\n");
            }
            else
            {
                hooked_roff("\n");
            }

            if (q)
            {
                blow_desc = format("  %^s to %s", p, q);
            }
            else
            {
                blow_desc = format("  %^s", p);
            }

            if (d1 && d2 && (know_everything || know_damage(r_idx, m)))
            {
                hooked_roff(format("%-50s(%dd%d damage)", blow_desc, d1, d2));
            }
            else if (d1 && d2)
            {
                hooked_roff(format("%-50s(damage unknown)", blow_desc));
            } else
            {
                hooked_roff(format("%-50s", blow_desc));
            }
        }
#endif



		/* Count the attacks as printed */
		r++;
	}

	/* Finish sentence above */
	if (r)
	{
#ifdef JP
		hooked_roff("。");
#else
		roff_new_section();
#endif

#ifndef JP
        /* Display average damage per round if all attacks are known */
        {
            bool all_attacks_known;
            int avg_damage;

            all_attacks_known = TRUE;
            avg_damage = 0;

            for (r = 0, m = 0; m < 4; m++)
            {
                int effect;
                int d1, d2;
                int avg_dam;

                /* Skip non-attacks */
                if (!r_ptr->blow[m].method) continue;
                if (r_ptr->blow[m].method == RBM_SHOOT) continue;

                if (!r_ptr->r_blows[m] && !know_everything) all_attacks_known = FALSE;
                if (!know_damage(r_idx, m)) all_attacks_known = FALSE;

                /* Mana draining melee attacks don't damage HP, ignore them */
                effect = r_ptr->blow[m].effect;
                if (effect == RBE_DR_MANA) continue;

                /* Extract the attack info */
                d1 = r_ptr->blow[m].d_dice;
                d2 = r_ptr->blow[m].d_side;

                /* Calculate average damage for the blow */
                avg_dam = (d1 + (d1 * d2)) / 2;
                avg_damage += avg_dam;
            }

            if (all_attacks_known || know_everything)
            {
                hooked_roff(format("%^s deals %d damage per round in melee on average (assuming no elemental resistances or vulnerabilities).  ", wd_he[msex], avg_damage));
                roff_new_section();
            }
        }
#endif // JP
	}

	/* Notice lack of attacks */
	else if (flags1 & RF1_NEVER_BLOW)
	{
#ifdef JP
		hooked_roff(format("%^sは物理的な攻撃方法を持たない。", wd_he[msex]));
#else
		hooked_roff(format("%^s has no physical attacks.  ", wd_he[msex]));
		roff_new_section();
#endif

	}

	/* Or describe the lack of knowledge */
	else
	{
#ifdef JP
		hooked_roff(format("%s攻撃については何も知らない。", wd_his[msex]));
#else
		hooked_roff(format("Nothing is known about %s attack.  ", wd_his[msex]));
		roff_new_section();
#endif

	}


	/*
	 * Notice "Quest" monsters, but only if you
	 * already encountered the monster.
	 */
	if ((flags1 & RF1_QUESTOR) && ((r_ptr->r_sights) && (r_ptr->max_num) && ((r_idx == MON_TAISAI) || (r_idx == MON_SERPENT) ||(r_idx == MON_YUKARI) )))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, "このモンスターこそが倒すべきボスであるらしい・・");
#else
		hook_c_roff(TERM_VIOLET, "Looks like this monster is a boss that you should defeat...");
#endif
	}

	else if (flags7 & RF7_GUARDIAN)
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, "このモンスターはダンジョンの主である。");
#else
		hook_c_roff(TERM_L_RED, "This monster is the master of a dungeon.");
#endif
	}

	/* All done */
	hooked_roff("\n");
}

/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
/*:::モンスターの思い出の一行目を記述する*/
void roff_top(int r_idx)
{
	monster_race	*r_ptr = &r_info[r_idx];

	byte		a1, a2;
	char		c1, c2;
	char        buf2[255];

	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

#ifndef JP
	/* A title (use "The" for non-uniques) */
	if (!((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}
#endif

	///mod140712 付喪神の思い出表記調整
	/*:::付喪神の思い出表示　r_idxしか情報がないのでm_listを再走査して名前を得ている。
	/*:::'/'コマンドやモンスター仙人からは付喪神を参照できないようにしてあるのでここに来るときは必ずフロアに該当付喪神がいて元アイテムを持っているはず*/
	/*:::..と思ったらサブウィンドウに思い出表示にしておいて付喪神にターゲットしたままフロア移動したらそうならない。やむなく適当なメッセージを代わりに入れる*/
	if(r_idx >= MON_TSUKUMO_WEAPON1 && r_idx <= MON_TSUKUMO_WEAPON5 )
	{
		int i;
		monster_type *m_ptr;
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if (m_ptr->r_idx == r_idx ) break;
		}
		if (i == m_max)
		{
			msg_print("ERROR:roff_top()");
			return;
		}
		if(i == m_max || !m_ptr->hold_o_idx)
		{
			if(p_ptr->pclass == CLASS_TSUKUMO_MASTER) sprintf(buf2,_("(以前の付喪神)", "(former tsukumogami)"));
			else sprintf(buf2,_("(以前魔力付与した武器)", "(former enchanted weapon)"));
		}
		else
			object_desc(buf2,&o_list[m_ptr->hold_o_idx],OD_NAME_ONLY);
		Term_addstr(-1, TERM_WHITE, buf2);
	}
	///mod140719 ランダムユニーク
	else if(IS_RANDOM_UNIQUE_IDX(r_idx))
	{
		sprintf(buf2,"%s",random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
		Term_addstr(-1, TERM_WHITE, buf2);
	}
	///mod150928 変身中モンスター
	else if(r_idx == MON_EXTRA_FIELD)
	{
		sprintf(buf2,"%s",extra_mon_name);
		Term_addstr(-1, TERM_WHITE, buf2);
	}

	/* Dump the name */
	else
		Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_add_bigch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_add_bigch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");

	/* Wizards get extra info */
	/*:::ウィザードモード中はモンスターインデックスが表示される*/
	if (p_ptr->wizard)
	{
		char buf[6];

		sprintf(buf, "%d", r_idx);

		Term_addstr(-1, TERM_WHITE, " (");
		Term_addstr(-1, TERM_L_BLUE, buf);
		Term_addch(TERM_WHITE, ')');
	}
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
/*:::メインウィンドウ上部にモンスターの思い出を表示する*/
void screen_roff(int r_idx, int mode)
{
	/* Flush messages */
	msg_print(NULL);

	/* Begin recall */
	Term_erase(0, 1, 255);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(r_idx, mode);

	/* Describe monster */
	roff_top(r_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
/*:::モンスターの思い出を表示する*/
void display_roff(int r_idx)
{
	int y;

	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* Begin recall */
	Term_gotoxy(0, 1);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(r_idx, 0);

	/* Describe monster */
	roff_top(r_idx);
}



/*
 * Hack -- output description of the given monster race
 */
/*:::モンスターの完全な思い出をスポイラーとして出力する*/
void output_monster_spoiler(int r_idx, void (*roff_func)(byte attr, cptr str))
{
	hook_c_roff = roff_func;

	/* Recall monster */
	roff_aux(r_idx, 0x03);
}

/*:::ダンジョン用モンスターhook WILD_ONLYは出ない*/
bool mon_hook_dungeon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!(r_ptr->flags8 & RF8_WILD_ONLY))
		return TRUE;
	else
	{
		dungeon_info_type *d_ptr = &d_info[dungeon_type];
		if ((d_ptr->mflags8 & RF8_WILD_MOUNTAIN) &&
		    (r_ptr->flags8 & RF8_WILD_MOUNTAIN)) return TRUE;
		return FALSE;
	}
}

/*:::荒野（海）用モンスターhook*/
static bool mon_hook_ocean(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_OCEAN)
		return TRUE;
	else
		return FALSE;
}

/*:::荒野（浅い水）用モンスターhook*/

static bool mon_hook_shore(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_SHORE)
		return TRUE;
	else
		return FALSE;
}

/*:::荒野（荒地）用モンスターhook*/
static bool mon_hook_waste(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_WASTE | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::町用モンスターhook*/
static bool mon_hook_town(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//if (r_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL))
	///mod141230 人里がちょっとらしくないので街住みのモンスターのみ出現
	if (r_ptr->flags8 & (RF8_WILD_TOWN))
		return TRUE;
	else
		return FALSE;
}

//v1.1.32　特殊街用hook
//神社や永遠亭など特定のモンスターしか出したくないか香霖堂など一切のモンスターを出したくないとき使う。
//ここでTRUEを返したモンスターしか出現しない。
//ただし、wilderness_gen()内でモンスターを生成したあとはdungeon()に入るときmonster_levelが0にリセットされるので、
//街生成後の時間経過によるモンスター生成ではレベル制限に引っかかってどのみち出れない。
#define LEV_CHK_OK (r_ptr->level < MAX(10,max_dlv[DUNGEON_ANGBAND]))
static bool mon_hook_special_town(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if(r_ptr->flags1 & RF1_QUESTOR)
		return FALSE;

	switch(p_ptr->town_num)
	{
	case TOWN_EIENTEI:
		if(r_idx == MON_YOUKAI_RABBIT || r_idx == MON_YOUKAI_RABBIT2) return TRUE;
		if(r_idx == MON_BAMBOO_FAIRY) return TRUE;
		if((r_idx == MON_TEWI || r_idx == MON_UDONGE) && LEV_CHK_OK) return TRUE;
		break;
	case TOWN_KOUMA:
		if(r_idx == MON_FAIRY || r_idx == MON_FLOWER_FAIRY || r_idx == MON_D_FAIRY || r_idx == MON_CIRNO || r_idx == MON_FAIRY_MAID) return TRUE;
		if(r_idx == MON_HOHGOBLIN) return TRUE;
		if(r_idx == MON_WAKASAGI) return TRUE;
		if(r_idx == MON_MEIRIN && LEV_CHK_OK) return TRUE;
		if(r_idx == MON_KOAKUMA && LEV_CHK_OK) return TRUE;
		break;
	case TOWN_HAKUREI:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		//あうん、三月精、お燐は出やすい
		if((r_idx == MON_ORIN ||  r_idx == MON_SHINMYOUMARU ||r_idx == MON_3FAIRIES || r_idx == MON_AUNN) && LEV_CHK_OK)
			return TRUE;

		//v1.1.43 クラウンピースも出るようにした
		if (r_idx == MON_CLOWNPIECE && LEV_CHK_OK)	return TRUE;

		//幻想ユニーク全部出るようにしようかと思ったがやめた
		//if(is_gen_unique(r_idx) && one_in_(10) && LEV_CHK_OK)
		//	return TRUE;
		break;
	case TOWN_MYOURENJI:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		if(r_idx >= MON_NAZRIN && r_idx <= MON_KYOUKO && LEV_CHK_OK)
			return TRUE;
		if(r_idx == MON_AUNN && LEV_CHK_OK)
			return TRUE;
		if(r_idx == MON_YOUKAI_MOUSE)
			return TRUE;
		break;
	case TOWN_MORIYA:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		if(r_idx == MON_KAPPA || r_idx == MON_AUNN)
			return TRUE;
		if(r_idx >= MON_SHIZUHA && r_idx <= MON_SUWAKO && LEV_CHK_OK)
			return TRUE;
		break;

	case TOWN_HIGAN:
		if (r_idx == MON_EIKI || r_idx == MON_KUTAKA)
			return TRUE;
		break;

	case TOWN_KOURIN:
		if (r_idx == MON_LAIKA) return TRUE;
		break;

	default:
		break;

		//注:人里はmon_hook_townで処理され、WILD_TOWNモンスター全て出る

	}

	return FALSE;
}
#undef LEV_CHK_OK


/*:::荒野（森）用モンスターhook*/
static bool mon_hook_wood(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_WOOD | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::荒野（溶岩）用モンスターhook*/
static bool mon_hook_volcano(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_VOLCANO)
		return TRUE;
	else
		return FALSE;
}

/*:::荒野（山）用モンスターhook*/
static bool mon_hook_mountain(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_MOUNTAIN)
		return TRUE;
	else
		return FALSE;
}

/*:::荒野（草地）用モンスターhook*/

static bool mon_hook_grass(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_GRASS | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::水棲モンスター用hook*/
static bool mon_hook_deep_water(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//v1.1.64 地上の深い水地形にWILD_OCEANかつWILD_ONLYのモンスターが出ないので条件式いじる
	//if (!mon_hook_dungeon(r_idx)) return FALSE;
	if (dun_level && !mon_hook_dungeon(r_idx)) return FALSE;

	if (r_ptr->flags7 & RF7_AQUATIC)
		return TRUE;
	else
		return FALSE;
}

/*:::浅い水用モンスター用hook　荒野専用と火炎オーラ持ち以外は通る*/

static bool mon_hook_shallow_water(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if (r_ptr->flags2 & RF2_AURA_FIRE)
		return FALSE;
	else
		return TRUE;
}

///mod131231 モンスターフラグ変更
static bool mon_hook_lava(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if (((r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK) ||
	     (r_ptr->flags7 & RF7_CAN_FLY)) &&
	    !(r_ptr->flags2 & RF2_AURA_COLD))
	    //!(r_ptr->flags3 & RF3_AURA_COLD))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_acid(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ACID_MASK) || (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}

static bool mon_hook_pois(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ACID_MASK) || (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_cold(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_COLD_MASK))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_elec(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK))
		return TRUE;
	else
		return FALSE;
}





static bool mon_hook_floor(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!(r_ptr->flags7 & RF7_AQUATIC) ||
	    (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}

/*:::get_mon_num_prep()用のhookを設定するための関数*/
/*:::wilderness[][].terrainに応じ地形ごとに出現可能モンスター判定関数のアドレスを返す*/
///system 荒野用のモンスター種類決定ルーチンの基幹部分　フレーバーのために変更するかも
monster_hook_type get_monster_hook(void)
{

	if (!dun_level && !p_ptr->inside_quest)
	{
		switch (wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain)
		{
		case TERRAIN_TOWN:
			return (monster_hook_type)mon_hook_town;
		/*:::w_info.txtの街でterrain値が2の場合ここに来る。実質的に敵が出ない・・？*/
		//v1.1.32　↑のやり方では万歳楽やボーダイルなど変なのが出るので新しい特殊地形を作った
		case TERRAIN_DEEP_WATER:
			return (monster_hook_type)mon_hook_ocean;
		case TERRAIN_SHALLOW_WATER:
		case TERRAIN_SWAMP:
			return (monster_hook_type)mon_hook_shore;
		case TERRAIN_DIRT:
		case TERRAIN_DESERT:
			return (monster_hook_type)mon_hook_waste;
		case TERRAIN_GRASS:
			return (monster_hook_type)mon_hook_grass;
		case TERRAIN_TREES:
			return (monster_hook_type)mon_hook_wood;
		case TERRAIN_SHALLOW_LAVA:
		case TERRAIN_DEEP_LAVA:
			return (monster_hook_type)mon_hook_volcano;
		case TERRAIN_MOUNTAIN:
			return (monster_hook_type)mon_hook_mountain;
		case TERRAIN_SPECIAL_TOWN:
			return (monster_hook_type)mon_hook_special_town;

		default:
			return (monster_hook_type)mon_hook_dungeon;
		}
	}
	else
	{
		return (monster_hook_type)mon_hook_dungeon;
	}
}

/*:::get_mon_num_prep()用のhook関数のアドレスを地形に応じて返す　ダンジョン内専用？*/
///system ダンジョン用hook　水、溶岩の出現可能判定　ひょっとしたら変更することがあるかも
monster_hook_type get_monster_hook2(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];

	/* Set the monster list */

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		/* Deep water */
		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			return (monster_hook_type)mon_hook_deep_water;
		}

		/* Shallow water */
		else
		{
			return (monster_hook_type)mon_hook_shallow_water;
		}
	}

	/* Lava */
	else if (have_flag(f_ptr->flags, FF_LAVA))
	{
		return (monster_hook_type)mon_hook_lava;
	}
	//v1.1.85
	else if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		return (monster_hook_type)mon_hook_pois;
	}
	else if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		return (monster_hook_type)mon_hook_acid;
	}
	else if (have_flag(f_ptr->flags, FF_COLD_PUDDLE))
	{
		return (monster_hook_type)mon_hook_cold;
	}
	else if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE))
	{
		return (monster_hook_type)mon_hook_elec;
	}






	else return (monster_hook_type)mon_hook_floor;
}


void set_friendly(monster_type *m_ptr)
{
	m_ptr->smart |= SM_FRIENDLY;
}

void set_pet(monster_type *m_ptr)
{
	if (!is_pet(m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	/* Check for quest completion */
	check_quest_completion(m_ptr);

	m_ptr->smart |= SM_PET;
	if (!(r_info[m_ptr->r_idx].flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)))
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
}

/*
 * Makes the monster hostile towards the player
 */
/*:::モンスターを敵対的にする*/
void set_hostile(monster_type *m_ptr)
{
	int i;
	if (p_ptr->inside_battle) return;

	//輝夜(主)は敵対しない
	if(m_ptr->r_idx == MON_MASTER_KAGUYA) return;

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);
	///mod141129 友好的な敵が敵対する時、＠を見ている他の友好的な敵も敵対する
	if(is_friendly(m_ptr) )
	{
		int add = 0;
		m_ptr->smart &= ~SM_FRIENDLY;
		for (i = 1; i < m_max; i++)
		{
			monster_type *m2_ptr = &m_list[i];
			monster_race *r2_ptr;
			if(!m2_ptr->r_idx) continue;
			if(m2_ptr->r_idx == MON_MASTER_KAGUYA) continue;
			r2_ptr = &r_info[m2_ptr->r_idx];

			///mod150830
			if(r2_ptr->flags2 & RF2_EMPTY_MIND) continue;

			if(is_friendly(m2_ptr) && (m2_ptr->cdis <= MAX_SIGHT) && !MON_CONFUSED(m2_ptr) && !MON_CSLEEP(m2_ptr))
			{
				m2_ptr->smart &= ~SM_FRIENDLY;
				add++;
			}
			//サトリコンクリフトと狐狸戦争では友好的な敵に攻撃すると全ての友好敵が敵対する
			//v1.1.91 ヤクザ戦争も
			else if(is_friendly(m2_ptr) && (p_ptr->inside_quest == QUEST_SATORI || p_ptr->inside_quest == QUEST_KORI || p_ptr->inside_quest == QUEST_YAKUZA_1))
			{

				m2_ptr->smart &= ~SM_FRIENDLY;
				add++;
			}
		}
#ifdef JP
		if(add) msg_format("周囲の味方%sも敵に回った気がする・・",(add > 1)?"達":"");
#else
        if(add) msg_format("You feel nearby friend%s turn hostile...",(add > 1)?"s":"");
#endif
	}

	m_ptr->smart &= ~SM_PET;
	m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the monster
 */
 /*:::友好的な敵を怒らせる。ダメージを与えたときここに来る*/
//寝てても起こさないらしい
void anger_monster(monster_type *m_ptr)
{
	if (p_ptr->inside_battle) return;

	if(m_ptr->r_idx == MON_MASTER_KAGUYA) return;

	if(is_pet(m_ptr))
	{
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_REPUTATION, -1);
	}

	else if (is_friendly(m_ptr))
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%^sは怒った！", m_name);
#else
		msg_format("%^s gets angry!", m_name);
#endif
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		set_deity_bias(DBIAS_REPUTATION, -1);

		set_hostile(m_ptr);

		///del131214 virtue
		//chg_virtue(V_INDIVIDUALISM, 1);
		//chg_virtue(V_HONOUR, -1);
		//chg_virtue(V_JUSTICE, -1);
		//chg_virtue(V_COMPASSION, -1);
	}
}


/*
 * Check if monster can cross terrain
 */
/*:::モンスターが渡れる地形かどうか判別*/
bool monster_can_cross_terrain(s16b feat, monster_race *r_ptr, u16b mode)
{
	feature_type *f_ptr = &f_info[feat];

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_RIDING))
		{
			if (!(r_ptr->flags7 & RF7_CAN_FLY)) return FALSE;
		}
		else
		{
			if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && (r_ptr->flags7 & RF7_CAN_FLY)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && (r_ptr->flags7 & RF7_CAN_SWIM)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if ((r_ptr->flags2 & RF2_PASS_WALL) && (!(mode & CEM_RIDING) || p_ptr->pass_wall)) return TRUE;
	}

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	/* Some monsters can walk on mountains */
	if (have_flag(f_ptr->flags, FF_MOUNTAIN) && (r_ptr->flags8 & RF8_WILD_MOUNTAIN)) return TRUE;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (!(r_ptr->flags7 & RF7_AQUATIC))
		{
			/* Deep water */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Shallow water */
			else if (r_ptr->flags2 & RF2_AURA_FIRE) return FALSE;
		}
	}

	/* Aquatic monster into non-water? */
	else if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)) return FALSE;
	}
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)) return FALSE;
	}
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)) return FALSE;
	}




	return TRUE;
}


/*
 * Strictly check if monster can enter the grid
 */
bool monster_can_enter(int y, int x, monster_race *r_ptr, u16b mode)
{
	cave_type *c_ptr = &cave[y][x];

	/* Player or other monster */
	if (player_bold(y, x)) return FALSE;
	if (c_ptr->m_idx) return FALSE;

	return monster_can_cross_terrain(c_ptr->feat, r_ptr, mode);
}


/*
 * Check if this monster has "hostile" alignment (aux)
 */
/*:::所属勢力チェック　邪悪と善良の喧嘩*/

static bool check_hostile_align(byte sub_align1, byte sub_align2)
{

	//v1.1.91 クエストダンジョンで複数勢力のモンスターが殴り合う用
	if ((sub_align1 & SUB_ALIGN_QUEST_MASK) && (sub_align1 & SUB_ALIGN_QUEST_MASK))
	{
		if (sub_align1 != sub_align2) return TRUE;
		else return FALSE;
	}

	if (sub_align1 != sub_align2)
	{
		if (((sub_align1 & SUB_ALIGN_EVIL) && (sub_align2 & SUB_ALIGN_GOOD)) ||
			((sub_align1 & SUB_ALIGN_GOOD) && (sub_align2 & SUB_ALIGN_EVIL)))
			return TRUE;
	}

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Check if two monsters are enemies
 */
/*:::モンスター二体が敵対状態かどうか判定*/
/*:::詳細未読*/
///system アンバーを大ゲンカさせたり勢力闘争させるならここを操作すべきか
bool are_enemies(monster_type *m_ptr, monster_type *n_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *s_ptr = &r_info[n_ptr->r_idx];

	///mod150616 闘技場変更
	if (p_ptr->inside_battle)
	{
		if (is_pet(m_ptr) || is_pet(n_ptr)) return FALSE;
		//同チームのフラグが立ってなければ敵対
		return (!(m_ptr->mflag & n_ptr->mflag & MFLAG_BATTLE_MON_MASK));
	}

	//パルスィに嫉妬心を煽られたモンスターの処理
	if (p_ptr->pclass == CLASS_PARSEE)
	{
		if (p_ptr->tim_general[0])
		{

			if (m_ptr->cdis <= MAX_SIGHT && !(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) return TRUE;
			if (n_ptr->cdis <= MAX_SIGHT && !(s_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) return TRUE;
		}
		if ((m_ptr->mflag & MFLAG_SPECIAL) && (r_ptr->level < s_ptr->level)) return TRUE;
		if ((n_ptr->mflag & MFLAG_SPECIAL) && (r_ptr->level > s_ptr->level)) return TRUE;

	}

	/*:::街に出るモンスターは敵対しない*/
	if ((r_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL))
	    && (s_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL)))
	{
		if (!is_pet(m_ptr) && !is_pet(n_ptr)) return FALSE;
	}

	//v1.1.15 魔法の得意な職の場合、配下同士が喧嘩しないようにした
	if(cp_ptr->magicmaster && is_pet(m_ptr) && is_pet(n_ptr))
		return FALSE;


	//ぬえの正体不明の種
	if((m_ptr->mflag & MFLAG_NUE_UNKNOWN || n_ptr->mflag & MFLAG_NUE_UNKNOWN)) return TRUE;

	//v1.1.43 クラウンピースの狂気の松明
	if ((m_ptr->mflag & MFLAG_LUNATIC_TORCH || n_ptr->mflag & MFLAG_LUNATIC_TORCH)) return TRUE;

	//v1.1.73 孤立状態
	if (n_ptr->mflag & MFLAG_ISOLATION) return TRUE;


	//菫子は幻想ユニークから追っかけ回され中
	if((m_ptr->r_idx == MON_SUMIREKO) && is_gen_unique(n_ptr->r_idx)
		||(n_ptr->r_idx == MON_SUMIREKO) && is_gen_unique(m_ptr->r_idx))
		return TRUE;

	/*:::正邪は無条件にあらゆる存在に敵対する*/
	if((m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) && (n_ptr->r_idx != MON_SEIJA && n_ptr->r_idx != MON_SEIJA_D))
		return TRUE;
	if((n_ptr->r_idx == MON_SEIJA || n_ptr->r_idx == MON_SEIJA_D) && (m_ptr->r_idx != MON_SEIJA && m_ptr->r_idx != MON_SEIJA_D))
		return TRUE;
	/*:::赤蛮奇の頭の特殊処理　＠に敵対するなら同じように敵対  ...普通の配下処理で問題ない気もする*/
	if(m_ptr->r_idx == MON_BANKI_HEAD_1 || m_ptr->r_idx == MON_BANKI_HEAD_2 || n_ptr->r_idx == MON_BANKI_HEAD_1 || n_ptr->r_idx == MON_BANKI_HEAD_2)
	{
		if(is_hostile(m_ptr) || is_hostile(n_ptr)) return TRUE;
		else return FALSE;
	}
	/*:::特殊処理　クエスト中の藍は妖怪狐に敵対しない*/
	if(p_ptr->inside_quest == QUEST_KORI)
	{
		if(m_ptr->r_idx == MON_RAN && n_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;
		if(n_ptr->r_idx == MON_RAN && m_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;

	}

	/*:::所属勢力チェック*/
	/* Friendly vs. opposite aligned normal or pet */
	if (check_hostile_align(m_ptr->sub_align, n_ptr->sub_align))
	{
		if (!(m_ptr->mflag2 & MFLAG2_CHAMELEON) || !(n_ptr->mflag2 & MFLAG2_CHAMELEON)) return TRUE;
	}

	//友好的でないレイマリはあらゆる存在に敵対する
	//..ようにしていたが、召喚魔理沙が自分の召喚したモンスターに攻撃されてしまうので魔理沙のみ通常処理に戻す
	if(m_ptr->r_idx == MON_REIMU && is_hostile(m_ptr) || n_ptr->r_idx == MON_REIMU && is_hostile(n_ptr))
		return TRUE;
/*
	if(m_ptr->r_idx == MON_MARISA && is_hostile(m_ptr) || n_ptr->r_idx == MON_MARISA && is_hostile(n_ptr))
		return TRUE;
*/




	/* Hostile vs. non-hostile */
	/*:::＠の敵対者と友好な者は敵対関係*/
	if (is_hostile(m_ptr) != is_hostile(n_ptr))
	{
		return TRUE;
	}

	/* Default */
	return FALSE;
}





///mod140420 @の勢力判定　今のところ＠の召喚時にCHAOSとCOSMOSどちら側が出るかの判定にしか使ってない
bool you_are_human_align(void)
{

	//v1.1.47 華扇は破邪弱点だが召喚で秩序側が出るようにしとく
	if (p_ptr->pclass == CLASS_KASEN && !is_special_seikaku(SEIKAKU_SPECIAL_KASEN)) return TRUE;

	//暫定　とりあえず破邪に弱い＠は妖怪側
	if(is_hurt_holy() > 0) return FALSE;
	else return TRUE;

}

/*
 * Check if this monster race has "hostile" alignment
 * If user is player, m_ptr == NULL.
 */

/*:::指定したモンスターが＠もしくは別の指定モンスターに敵対かどうか判定する*/
///sys モンスター敵対条件設定部
bool monster_has_hostile_align(monster_type *m_ptr, int pa_good, int pa_evil, monster_race *r_ptr)
{
	byte sub_align1 = SUB_ALIGN_NEUTRAL;
	byte sub_align2 = SUB_ALIGN_NEUTRAL;

	if (m_ptr) /* For a monster */
	{
		sub_align1 = m_ptr->sub_align;
	}
	else /* For player */
	{
		/*
		if (p_ptr->align >= pa_good) sub_align1 |= SUB_ALIGN_GOOD;
		if (p_ptr->align <= pa_evil) sub_align1 |= SUB_ALIGN_EVIL;
		*/
		///mod140420 ＠の召喚時などの暫定勢力判定
		if(you_are_human_align()) sub_align1 |= SUB_ALIGN_GOOD;
		else sub_align1 |= SUB_ALIGN_EVIL;
	}

	/* Racial alignment flags */
	if (r_ptr->flags3 & RF3_ANG_CHAOS) sub_align2 |= SUB_ALIGN_EVIL;
	if (r_ptr->flags3 & RF3_ANG_COSMOS) sub_align2 |= SUB_ALIGN_GOOD;

	if (check_hostile_align(sub_align1, sub_align2)) return TRUE;

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Is the monster "alive"?
 *
 * Used to determine the message to print for a killed monster.
 * ("dies", "destroyed")
 */
///mon flag 無生物フラグ処理
///mod140222 人間、動物などのフラグを持たない神格は無生物扱いにするようにした
bool monster_living(monster_race *r_ptr)
{
	/* Non-living, undead, or demon */
	if (r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING))
		return FALSE;
	else if(r_ptr->flags3 & (RF3_KWAI | RF3_DEMIHUMAN | RF3_DRAGON | RF3_ANIMAL | RF3_HUMAN))
		return TRUE;
	else if (r_ptr->flags3 & RF3_DEITY )
		return FALSE;
	else
		return TRUE;
}


/*
 * Is this monster declined to be questor or bounty?
 */
/*:::ランクエにも賞金首にもならない奴を判定　今のところバーノール＝ルパートのみ*/
bool no_questor_or_bounty_uniques(int r_idx)
{
	switch (r_idx)
	{
	/*
	 * Decline them to be questor or bounty because they use
	 * special motion "split and combine"
	 */
	case MON_BANORLUPART:
	case MON_BANOR:
	case MON_LUPART:
		return TRUE;
	default:
		return FALSE;
	}
}
