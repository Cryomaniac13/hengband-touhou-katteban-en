/* File: mspells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells (attack player) */

#include "angband.h"


/*
 * And now for Intelligent monster attacks (including spells).
 *
 * Original idea and code by "DRS" (David Reeves Sward).
 * Major modifications by "BEN" (Ben Harrison).
 *
 * Give monsters more intelligent attack/spell selection based on
 * observations of previous attacks on the player, and/or by allowing
 * the monster to "cheat" and know the player status.
 *
 * Maintain an idea of the player status, and use that information
 * to occasionally eliminate "ineffective" spell attacks.  We could
 * also eliminate ineffective normal attacks, but there is no reason
 * for the monster to do this, since he gains no benefit.
 * Note that MINDLESS monsters are not allowed to use this code.
 * And non-INTELLIGENT monsters only use it partially effectively.
 *
 * Actually learn what the player resists, and use that information
 * to remove attacks or spells before using them.  This will require
 * much less space, if I am not mistaken.  Thus, each monster gets a
 * set of 32 bit flags, "smart", build from the various "SM_*" flags.
 *
 * This has the added advantage that attacks and spells are related.
 * The "smart_learn" option means that the monster "learns" the flags
 * that should be set, and "smart_cheat" means that he "knows" them.
 * So "smart_cheat" means that the "smart" field is always up to date,
 * while "smart_learn" means that the "smart" field is slowly learned.
 * Both of them have the same effect on the "choose spell" routine.
 */



/*
 * Internal probability routine
 */
static bool int_outof(monster_race *r_ptr, int prob)
{
	/* Non-Smart monsters are half as "smart" */
	if (!(r_ptr->flags2 & RF2_SMART)) prob = prob / 2;

	/* Roll the dice */
	return (randint0(100) < prob);
}


//v1.1.32 指定モンスターの隣に「ランクアップ可能なモンスター」がいればそのm_idxを返す。
//舞と里乃の特別行動用
static int can_rankup_monster(monster_type *m_ptr)
{
	int i,x,y;
	int m_idx = 0;

	for(i=0;i<10;i++)
	{
		monster_type *tm_ptr;
		monster_race *tr_ptr;

		y = m_ptr->fy+ddy[i];
		x = m_ptr->fx+ddx[i];

		if(!in_bounds(y,x)) continue;
		if(!cave[y][x].m_idx) continue;
		tm_ptr = &m_list[cave[y][x].m_idx];
		tr_ptr = &r_info[tm_ptr->r_idx];
		if (tm_ptr->mflag2 & MFLAG2_CHAMELEON)continue;
		if (tr_ptr->flags7 & RF7_TANUKI) continue;
		if(!tr_ptr->next_exp) continue;
		if(are_enemies(m_ptr,tm_ptr)) continue;

		m_idx = cave[y][x].m_idx;
		break;
	}
	return m_idx;

}

/*
 * Remove the "bad" spells from a spell list
 */
/*:::候補スペルリストの中から効果のないものを除外する　STUPIDやsmartオプションOFFのときは何もしない*/
///res system　mon f9flag 学習により敵が効果のない魔法を使わなくなる
///mod140406 呪文学習処理を新フラグ・スペルに合わせ変更
static void remove_bad_spells(int m_idx, u32b *f4p, u32b *f5p, u32b *f6p, u32b *f9p)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b f4 = (*f4p);
	u32b f5 = (*f5p);
	u32b f6 = (*f6p);
	u32b f9 = (*f9p);

	u32b smart = 0L;

	/* Too stupid to know anything */
	if (r_ptr->flags2 & RF2_STUPID) return;

	/* Must be cheating or learning */
	if (!smart_cheat && !smart_learn) return;

	/* Update acquired knowledge */
	if (smart_learn)
	{
		/* Hack -- Occasionally forget player status */
		/* Only save SM_FRIENDLY, SM_PET or SM_CLONED */
		if (m_ptr->smart && (randint0(100) < 1)) m_ptr->smart &= (SM_FRIENDLY | SM_PET | SM_CLONED);

		/* Use the memorized flags */
		smart = m_ptr->smart;
	}


	/* Cheat if requested */
	//if (smart_cheat)
	///mod140406 SMART_EXフラグの敵は最初から全て知っている設定にする
	if (smart_cheat || r_ptr->flags2 & RF2_SMART_EX)
	{
		/* Know basic info */
		if (p_ptr->resist_acid) smart |= (SM_RES_ACID);
		if (IS_OPPOSE_ACID()) smart |= (SM_OPP_ACID);
		if (p_ptr->immune_acid) smart |= (SM_IMM_ACID);
		if (p_ptr->resist_elec) smart |= (SM_RES_ELEC);
		if (IS_OPPOSE_ELEC()) smart |= (SM_OPP_ELEC);
		if (p_ptr->immune_elec) smart |= (SM_IMM_ELEC);
		if (p_ptr->resist_fire) smart |= (SM_RES_FIRE);
		if (IS_OPPOSE_FIRE()) smart |= (SM_OPP_FIRE);
		if (p_ptr->immune_fire) smart |= (SM_IMM_FIRE);
		if (p_ptr->resist_cold) smart |= (SM_RES_COLD);
		if (IS_OPPOSE_COLD()) smart |= (SM_OPP_COLD);
		if (p_ptr->immune_cold) smart |= (SM_IMM_COLD);

		/* Know poison info */
		if (p_ptr->resist_pois) smart |= (SM_RES_POIS);
		if (IS_OPPOSE_POIS()) smart |= (SM_OPP_POIS);

		/* Know special resistances */
		if (p_ptr->resist_neth) smart |= (SM_RES_NETH);
		if (p_ptr->resist_lite) smart |= (SM_RES_LITE);
		if (p_ptr->resist_dark) smart |= (SM_RES_DARK);
		if (p_ptr->resist_fear && !(IS_VULN_FEAR)) smart |= (SM_RES_FEAR);
		if (p_ptr->resist_conf) smart |= (SM_RES_CONF);
		if (p_ptr->resist_chaos) smart |= (SM_RES_CHAOS);
		if (p_ptr->resist_disen) smart |= (SM_RES_DISEN);
		if (p_ptr->resist_blind) smart |= (SM_RES_BLIND);
		///del131228 res_nexus
		//if (p_ptr->resist_nexus) smart |= (SM_RES_NEXUS);
		if (p_ptr->resist_sound) smart |= (SM_RES_SOUND);
		if (p_ptr->resist_shard) smart |= (SM_RES_SHARD);
		if (p_ptr->reflect) smart |= (SM_IMM_REFLECT);

		/* Know bizarre "resistances" */
		if (p_ptr->free_act) smart |= (SM_IMM_FREE);
		//if (!p_ptr->msp) smart |= (SM_IMM_MANA);
		/*:::追加フラグ*/
		if(p_ptr->resist_time)  smart |= (SM_RES_TIME);
		if(p_ptr->resist_holy)  smart |= (SM_RES_HOLY);
		if(p_ptr->resist_water)  smart |= (SM_RES_WATER);

	}


	/* Nothing known */
	if (!smart) return;


	if (smart & SM_IMM_ACID)
	{
		f4 &= ~(RF4_BR_ACID);
		f5 &= ~(RF5_BA_ACID);
		f5 &= ~(RF5_BO_ACID);
		f9 &= ~(RF9_ACID_STORM);
	}
	else if ((smart & (SM_OPP_ACID)) && (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_ACID_STORM);
	}
	else if ((smart & (SM_OPP_ACID)) || (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_ACID_STORM);
	}


	if (smart & (SM_IMM_ELEC))
	{
		f4 &= ~(RF4_BR_ELEC);
		f5 &= ~(RF5_BA_ELEC);
		f5 &= ~(RF5_BO_ELEC);
		f9 &= ~(RF9_THUNDER_STORM);
	}
	else if ((smart & (SM_OPP_ELEC)) && (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_THUNDER_STORM);
	}
	else if ((smart & (SM_OPP_ELEC)) || (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_THUNDER_STORM);
	}


	if (smart & (SM_IMM_FIRE))
	{
		f4 &= ~(RF4_BR_FIRE);
		f5 &= ~(RF5_BA_FIRE);
		f5 &= ~(RF5_BO_FIRE);
		f9 &= ~(RF9_FIRE_STORM);
	}
	else if ((smart & (SM_OPP_FIRE)) && (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_FIRE_STORM);
	}
	else if ((smart & (SM_OPP_FIRE)) || (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_FIRE_STORM);
	}


	if (smart & (SM_IMM_COLD))
	{
		f4 &= ~(RF4_BR_COLD);
		f5 &= ~(RF5_BA_COLD);
		f5 &= ~(RF5_BO_COLD);
		f5 &= ~(RF5_BO_ICEE);
		f9 &= ~(RF9_ICE_STORM);
	}
	else if ((smart & (SM_OPP_COLD)) && (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_ICE_STORM);
	}
	else if ((smart & (SM_OPP_COLD)) || (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 20)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_ICE_STORM);
	}

	//核熱属性使用中止判定
	if (smart & (SM_IMM_FIRE | SM_OPP_FIRE) && smart & (SM_RES_LITE) )
	{
		if (int_outof(r_ptr, 70)) f9 &= ~(RF9_GIGANTIC_LASER);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NUKE);
	}
	//プラズマ属性
	if (smart & (SM_IMM_FIRE | SM_OPP_FIRE) &&  smart & (SM_IMM_ELEC | SM_OPP_ELEC) )
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_PLAS);
		if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_PLAS);
	}

	if ((smart & (SM_OPP_POIS)) && (smart & (SM_RES_POIS)))
	{
		if(p_ptr->pclass == CLASS_MEDICINE || p_ptr->pclass == CLASS_EIRIN)
		{
			f4 &= ~(RF4_BR_POIS);
			f5 &= ~(RF5_BA_POIS);
			f9 &= ~(RF9_TOXIC_STORM);
			if (int_outof(r_ptr, 60)) f9 &= ~(RF9_BA_POLLUTE);
		}
		else
		{
			if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_POIS);
			if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_POIS);
			if (int_outof(r_ptr, 80)) f9 &= ~(RF9_TOXIC_STORM);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BA_POLLUTE);
		}
	}
	else if ((smart & (SM_OPP_POIS)) || (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_POIS);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_TOXIC_STORM);
	}

	if (smart & (SM_RES_NETH))
	{
		if (prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_HECATIA)
		{
			f4 &= ~(RF4_BR_NETH);
			f5 &= ~(RF5_BA_NETH);
			f5 &= ~(RF5_BO_NETH);
		}
		else
		{
			if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_NETH);
		}
	}

	if (smart & (SM_RES_LITE))
	{

		if(is_hurt_lite() < 0)
		{
			f4 &= ~(RF4_BR_LITE);
			f5 &= ~(RF5_BA_LITE);
			f9 &= ~(RF9_BEAM_LITE);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_LITE);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_LITE);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BEAM_LITE);
		}

	}

	//呪い攻撃　暗黒、地獄耐性で判定追加
	if (smart & (SM_RES_DARK) || smart & (SM_RES_NETH) )
	{
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_1);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_2);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_3);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_4);
	}
	if (smart & (SM_RES_DARK))
	{
		if (prace_is_(RACE_VAMPIRE) || p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19)
		{
			f4 &= ~(RF4_BR_DARK);
			f5 &= ~(RF5_BA_DARK);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_DARK);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_DARK);
		}
	}

	if (smart & (SM_RES_FEAR))
	{
		f5 &= ~(RF5_SCARE);
	}

	if (smart & (SM_RES_CONF))
	{
		f5 &= ~(RF5_CONF);
//		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_CONF);
	}

	if (smart & (SM_RES_CHAOS))
	{
		if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_CHAO);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BA_CHAO);
	}

	if (smart & (SM_RES_DISEN))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_DISE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_BA_POLLUTE);
	}

	if (smart & (SM_RES_BLIND))
	{
		f5 &= ~(RF5_BLIND);
	}

	//if (smart & (SM_RES_NEXUS))
	//{
	//	if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NEXU);
	//	f6 &= ~(RF6_TELE_LEVEL);
	//}

	if (smart & (SM_RES_SOUND))
	{

		if (p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29)
		{
			f4 &= ~(RF4_BR_SOUN);
			f9 &= ~(RF9_BO_SOUND);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_SOUN);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BO_SOUND);
		}
	}

	if (smart & (SM_RES_SHARD))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_SHAR);

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		{
			f4 &= ~(RF4_BR_SHAR | RF4_ROCKET);
		}

	}

	if (smart & (SM_IMM_REFLECT))
	{
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_NETH);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_WATE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_MANA);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_PLAS);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_MISSILE);
		if (int_outof(r_ptr, 150)) f4 &= ~(RF4_SHOOT);
		f4 &= ~(RF4_DANMAKU);
		if (int_outof(r_ptr, 150)) f9 &= ~(RF9_BO_HOLY);
	}

	if (smart & (SM_IMM_FREE))
	{
		f5 &= ~(RF5_HOLD);
		f5 &= ~(RF5_SLOW);
	}
/*
	if (smart & (SM_IMM_MANA))
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}
*/
	/*:::追加属性*/
	if (smart & (SM_RES_HOLY))
	{
		f9 &= ~(RF9_PUNISH_1);
		f9 &= ~(RF9_PUNISH_2);
		f9 &= ~(RF9_PUNISH_3);
		f9 &= ~(RF9_PUNISH_4);
		if(is_hurt_holy() < 0)
		{
			f4 &= ~(RF4_BR_HOLY);
			f9 &= ~(RF9_BA_HOLY);
			f9 &= ~(RF9_BO_HOLY);
			f9 &= ~(RF9_HOLYFIRE);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_HOLY);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BA_HOLY);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_HOLYFIRE);
			if (int_outof(r_ptr, 70)) f9 &= ~(RF9_BO_HOLY);
		}

	}
	if (smart & (SM_RES_TIME))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_NEXU);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_TIME);
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_GRAV);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_BA_DISTORTION);
		if (int_outof(r_ptr, 70)) f6 &= ~(RF6_TELE_AWAY);
		f6 &= ~(RF6_TELE_LEVEL);

	}
	if (smart & (SM_RES_WATER))
	{

		if (m_ptr->r_idx == MON_YUMA)
		{
			; //尤魔のメイルシュトロムは回復行動も兼ねているので＠に水耐性があっても止めない
		}
		else if(is_hurt_water() < 0)
		{
 			f4 &= ~(RF4_BR_AQUA);
			f5 &= ~(RF5_BO_WATE);
			f5 &= ~(RF5_BA_WATE);
			f9 &= ~(RF9_MAELSTROM);

		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_AQUA);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_WATE);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_WATE);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_MAELSTROM);
		}

	}


	/* XXX XXX XXX No spells left? */
	/* if (!f4 && !f5 && !f6) ... */

	(*f4p) = f4;
	(*f5p) = f5;
	(*f6p) = f6;
	(*f9p) = f9;
}


/*
 * Determine if there is a space near the player in which
 * a summoned creature can appear
 */
/*:::＠の近くにモンスターを召喚するスペースがあるかどうか判定*/
bool summon_possible(int y1, int x1)
{
	int y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if (!in_bounds(y, x)) continue;

			/* Only check a circular area */
			if (distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if (pattern_tile(y, x)) continue;

			/* Require empty floor grid in line of projection */
			if (cave_empty_bold(y, x) && projectable(y1, x1, y, x) && projectable(y, x, y1, x1)) return (TRUE);
		}
	}

	return FALSE;
}

/*:::敵が死人返しの魔法を使えるか判定*/
///del131221 死体関連削除
#if 0
bool raise_possible(monster_type *m_ptr)
{
	int xx, yy;
	int y = m_ptr->fy;
	int x = m_ptr->fx;
	s16b this_o_idx, next_o_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if (distance(y, x, yy, xx) > 5) continue;
			if (!los(y, x, yy, xx)) continue;
			if (!projectable(y, x, yy, xx)) continue;

			c_ptr = &cave[yy][xx];
			/* Scan the pile of objects */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Known to be worthless? */
				if (o_ptr->tval == TV_CORPSE)
				{
					if (!monster_has_hostile_align(m_ptr, 0, 0, &r_info[o_ptr->pval])) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
#endif

/*
 * Originally, it was possible for a friendly to shoot another friendly.
 * Change it so a "clean shot" means no equally friendly monster is
 * between the attacker and target.
 */
/*
 * Determine if a bolt spell will hit the player.
 *
 * This is exactly like "projectable", but it will
 * return FALSE if a monster is in the way.
 * no equally friendly monster is
 * between the attacker and target.
 */
/*:::同勢力のモンスターにボルトが当たるかどうか判定*/
bool clean_shot(int y1, int x1, int y2, int x2, bool _friend)
{
	/* Must be the same as projectable() */

	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if ((cave[y][x].m_idx > 0) && !((y == y2) && (x == x2)))
		{
			monster_type *m_ptr = &m_list[cave[y][x].m_idx];
			if (_friend == is_pet(m_ptr))
			{
				return (FALSE);
			}
		}
		/* Pets may not shoot through the character - TNB */
		if (player_bold(y, x))
		{
			if (_friend) return (FALSE);
		}
	}

	return (TRUE);
}

/*
 * Cast a bolt at the player
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void bolt(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_PLAYER;
	if (typ != GF_ARROW) flg  |= PROJECT_REFLECTABLE;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void beam(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Cast a breath (or ball) attack at the player
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
///sys mon 破邪系の呪いなどモンスターの投射系スキル実装時変更要？
static void breath(int y, int x, int m_idx, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if ((rad < 1) && breath) rad = (r_ptr->flags2 & (RF2_POWERFUL)) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	switch (typ)
	{
	///spell 軌跡のない魔法はここで定義する必要がある
	case GF_ROCKET:
		flg |= PROJECT_STOP;
		break;
	case GF_DRAIN_MANA:
	case GF_MIND_BLAST:
	case GF_BRAIN_SMASH:
	case GF_CAUSE_1:
	case GF_CAUSE_2:
	case GF_CAUSE_3:
	case GF_CAUSE_4:
	case GF_PUNISH_1:
	case GF_PUNISH_2:
	case GF_PUNISH_3:
	case GF_PUNISH_4:
	case GF_COSMIC_HORROR:
	case GF_HAND_DOOM:
	case GF_KYUTTOSHITEDOKA_N:
		flg |= (PROJECT_HIDE | PROJECT_AIMED);
		break;
	case GF_METEOR:
	case GF_TORNADO:
		flg |= PROJECT_JUMP;
		break;

	}


	/* Target the player with a ball attack */
	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

///mod140328 大型レーザー
void masterspark(int y, int x, int m_idx, int typ, int dam_hp, int monspell, bool learnable, int rad)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_THRU | PROJECT_FAST | PROJECT_MASTERSPARK;
	if(typ == GF_DISINTEGRATE) flg |= PROJECT_DISI;

	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*:::装備の呪いを決める*/
u32b get_curse(int power, object_type *o_ptr)
{
	u32b new_curse;

	while(1)
	{
		new_curse = (1 << (randint0(MAX_CURSE)+4));
		if (power == 2)
		{
			if (!(new_curse & TRC_HEAVY_MASK)) continue;
		}
		else if (power == 1)
		{
			if (new_curse & TRC_SPECIAL_MASK) continue;
		}
		else if (power == 0)
		{
			if (new_curse & TRC_HEAVY_MASK) continue;
		}
		if (new_curse == TRC_LOW_MELEE && !object_is_weapon(o_ptr)) continue;
		if (new_curse == TRC_LOW_AC && !object_is_armour(o_ptr)) continue;
		break;
	}
	return new_curse;
}

/*:::アイテムを呪う。chance:呪われる確率 heavy_chance:呪われた場合それが強い呪いになる確率*/
void curse_equipment(int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	int slot;
	u32b        new_curse;
	u32b oflgs[TR_FLAG_SIZE];
	///item アイテム呪い　装備欄特殊職のとき注意
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_format(_("装備品が呪われかけたが、あなたが呪いを吸い取った。",
                    "Your equipment was about to get cursed, but you absorbed it."));
		hina_gain_yaku(chance * 2 + heavy_chance * 5 +  randint1(30));
		return;
	}
	if (p_ptr->pclass == CLASS_SHION)
	{
		msg_format(_("装備品が呪われかけたが、あなたが代わりに不幸を背負った。",
                    "Your equipment was about to get cursed, but you took it in as your misfortune instead."));
		p_ptr->magic_num1[1] += (chance * 10 + heavy_chance * 50 + randint1(1000));
		return;
	}


	slot = INVEN_RARM + randint0(12);

	if(check_invalidate_inventory(slot)) return;
	if (randint1(100) > chance) return;
	o_ptr = &inventory[slot];

	if (!o_ptr->k_idx) return;

	object_flags(o_ptr, oflgs);

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	//v1.1.15 純狐の純化済み装備品は呪われない
	if(p_ptr->pclass == CLASS_JUNKO && o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER)
	{
		msg_format(_("%sは呪いを跳ね返した！", "%s resists being cursed!"), o_name);
		return;
	}

	/* Extra, biased saving throw for blessed items */
	if (have_flag(oflgs, TR_BLESSED) && (randint1(888) > chance))
	{
#ifdef JP
msg_format("%sは呪いを跳ね返した！", o_name,
#else
		msg_format("Your %s resist%s cursing!", o_name,
#endif

			((o_ptr->number > 1) ? "" : "s"));
		/* Hmmm -- can we wear multiple items? If not, this is unnecessary */
		return;
	}

	if ((randint1(100) <= heavy_chance) &&
	    (object_is_artifact(o_ptr) || object_is_ego(o_ptr)))
	{
		if (!(o_ptr->curse_flags & TRC_HEAVY_CURSE))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		o_ptr->curse_flags |= TRC_CURSED;
		curse_power++;
	}
	else
	{
		if (!object_is_cursed(o_ptr))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_CURSED;
	}
	if (heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, o_ptr);
	if (!(o_ptr->curse_flags & new_curse))
	{
		changed = TRUE;
		o_ptr->curse_flags |= new_curse;
	}

	if (changed)
	{
#ifdef JP
msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", o_name);
#else
		msg_format("There is a malignant black aura surrounding %s...", o_name);
#endif

		if(p_ptr->pclass == CLASS_KOGASA && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30)
			msg_format(_("何だか体調が悪い気がする...", "You don't feel so good..."));

		o_ptr->feeling = FEEL_NONE;
	}
	p_ptr->update |= (PU_BONUS);
}

///sys mon ここから下、しばらくモンスタースペルの分類用関数　全てスペル番号のハードコーディング
/*
 * Return TRUE if a spell is good for hurting the player (directly).
 */
static bool spell_attack(byte spell)
{
	/* All RF4 spells hurt (except for shriek and dispel) */
	if (spell < 128 && spell > 96 && spell != 98) return (TRUE);

	/* Various "ball" spells */
	if (spell >= 128 && spell <= 128 + 8) return (TRUE);

	/* "Cause wounds" and "bolt" spells */
	if (spell >= 128 + 12 && spell < 128 + 27) return (TRUE);

	/* Hand of Doom */
	if (spell == 160 + 1) return (TRUE);

	/* Psycho-Spear */
	if (spell == 160 + 11) return (TRUE);

	/* Cosmic Horror */
	if (spell == 160 + 15) return (TRUE);

	if(spell >= 192 && spell <= 208) return (TRUE);
	if(spell >= 210 && spell <= 215) return (TRUE);
	if(spell == 222) return (TRUE);

	if(spell == 175) return (TRUE);//コズミックホラー


	/* Doesn't hurt */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for escaping.
 */
static bool spell_escape(byte spell)
{
	/* Blink or Teleport */
	if (spell == 160 + 4 || spell == 160 + 5) return (TRUE);

	/* Teleport the player away */
	if (spell == 160 + 9 || spell == 160 + 10) return (TRUE);

	/* 破壊 */
	if (spell == 216) return (TRUE);


	/* Isn't good for escaping */
	return (FALSE);
}

/*
 * Return TRUE if a spell is good for annoying the player.
 */
static bool spell_annoy(byte spell)
{
	/* Shriek */
	if (spell == 96 + 0) return (TRUE);

	/* Brain smash, et al (added curses) */
	if (spell >= 128 + 9 && spell <= 128 + 14) return (TRUE);

	/* Scare, confuse, blind, slow, paralyze */
	if (spell >= 128 + 27 && spell <= 128 + 31) return (TRUE);

	/* Teleport to */
	if (spell == 160 + 8) return (TRUE);

	/* Teleport level */
	if (spell == 160 + 10) return (TRUE);

	/* Darkness, make traps, cause amnesia */
	if (spell >= 160 + 12 && spell <= 160 + 14) return (TRUE);
	/* 破壊 */
	if (spell == 216) return (TRUE);

	/* アラーム */
	if (spell == 223) return (TRUE);

	/* Doesn't annoy */
	return (FALSE);
}

/*
 * Return TRUE if a spell summons help.
 */
static bool spell_summon(byte spell)
{
	/* All summon spells */
	//条件式追加 192未満
	if (spell >= 160 + 16 && spell < 192) return (TRUE);

	if (spell == 209) return (TRUE);
	if (spell == 217) return (TRUE);
	if (spell == 218) return (TRUE);
	if (spell == 219) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}

#if 0
/*
 * Return TRUE if a spell raise-dead.
 */

static bool spell_raise(byte spell)
{
	/* All raise-dead spells */
	if (spell == 160 + 15) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}
#endif

/*
 * Return TRUE if a spell is good in a tactical situation.
 */
static bool spell_tactic(byte spell)
{
	/* Blink */
	if (spell == 160 + 4) return (TRUE);

	//隣接テレポ
	if (spell == 220) return (TRUE);
	if (spell == 221) return (TRUE);

	/* Not good */
	return (FALSE);
}

/*
 * Return TRUE if a spell makes invulnerable.
 */
static bool spell_invulner(byte spell)
{
	/* Invulnerability */
	if (spell == 160 + 3) return (TRUE);

	/* No invulnerability */
	return (FALSE);
}

/*
 * Return TRUE if a spell hastes.
 */
static bool spell_haste(byte spell)
{
	/* Haste self */
	if (spell == 160 + 0) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell world.
 */
static bool spell_world(byte spell)
{
	/* world */
	if (spell == 160 + 6) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell special.
 */
static bool spell_special(byte spell)
{
//	if (p_ptr->inside_battle) return FALSE;

	/* RF6_SPECIAL */
	if (spell == 160 + 7) return (TRUE);
	//RF4_SPECIAL2
	if( spell ==  96+28) return TRUE;

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell psycho-spear.
 */
static bool spell_psy_spe(byte spell)
{
	/* world */
	if (spell == 160 + 11) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for healing.
 */
static bool spell_heal(byte spell)
{
	/* Heal */
	if (spell == 160 + 2) return (TRUE);

	/* No healing */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for dispel.
 */
static bool spell_dispel(byte spell)
{
	/* Dispel */
	if (spell == 96 + 2) return (TRUE);

	/* No dispel */
	return (FALSE);
}


/*
 * Check should monster cast dispel spell.
 */
/*:::＠に魔力消去をするか判定する　ここの条件に引っかかる場合SMARTの敵は1/2で魔力消去を撃ってくる*/
///spell mon realm
bool dispel_check(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int i;

	/* Invulnabilty (including the song) */
	if (IS_INVULN() && !(p_ptr->special_defense & MUSOU_TENSEI) && !SUPER_SHION) return (TRUE);

	//v1.1.44 イビルアンジュレーション
	if (p_ptr->special_defense & EVIL_UNDULATION_MASK) return (TRUE);

	if(p_ptr->tim_superstealth) return (TRUE);
	if(p_ptr->tim_sh_death) return (TRUE);
	if(p_ptr->deportation) return (TRUE);

	for(i=0;i<6;i++) if(p_ptr->tim_addstat_num[i])
	{
		return (TRUE);
	}

	//白蓮肉体強化
	if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->tim_general[0]) return (TRUE);

	//サグメ神々の弾冠
	if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0]) return (TRUE);

	//鈴瑚肉体強化
	if(p_ptr->pclass == CLASS_RINGO && p_ptr->tim_general[0] > 2) return (TRUE);

	//v1.1.17 純狐独自一時効果　全部1/2
	if(USE_NAMELESS_ARTS)
	{
		for(i=0;i<TIM_GENERAL_MAX;i++) if(p_ptr->tim_general[0] && one_in_(2)) return (TRUE);
	}

	//v1.1.56 スペルカード　全部1/2
	if (p_ptr->tim_spellcard_count && one_in_(2)) return (TRUE);


	/* Wraith form */
	if (p_ptr->wraith_form) return (TRUE);

	/* Shield */
	if (p_ptr->shield) return (TRUE);

	/* Magic defence */
	if (p_ptr->magicdef) return (TRUE);

	/* Multi Shadow */
	if (p_ptr->multishadow) return (TRUE);

	/* Robe of dust */
	if (p_ptr->dustrobe) return (TRUE);

	/* Berserk Strength */
	if (p_ptr->shero && (p_ptr->pseikaku != SEIKAKU_BERSERK)) return (TRUE);

	/* Demon Lord */
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD) return (TRUE);
	else if(p_ptr->mimic_form == MIMIC_GIGANTIC) return (TRUE);
	else if(p_ptr->mimic_form == MIMIC_DRAGON) return (TRUE);
	else if (IS_METAMORPHOSIS)//v1.1.48 少しフラグ整理
	{
		//ドレミーと紫苑のモンスター変身は魔力消去判定にかからない
		if((p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME));

		else if (one_in_(2)) return TRUE;

	}
	else if(p_ptr->mimic_form && one_in_(2)) return (TRUE);

	if(p_ptr->kamioroshi && !one_in_(3)) return (TRUE);

	/* Elemental resistances */
	if (r_ptr->flags4 & RF4_BR_ACID || r_ptr->flags9 & RF9_ACID_STORM)
	{

		if (!p_ptr->immune_acid && (p_ptr->oppose_acid || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
		if (p_ptr->special_defense & DEFENSE_ACID) return (TRUE);
	}

	if (r_ptr->flags4 & RF4_BR_FIRE || r_ptr->flags9 & RF9_FIRE_STORM)
	{
		if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34));//非対象
		else if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29));//非対象
		else if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39));//非対象
		else if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29));//非対象
		else if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29));//非対象
		else if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29));
		else
		{
			if (!p_ptr->immune_fire && (p_ptr->oppose_fire || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_FIRE) return (TRUE);
		}
	}

	if (r_ptr->flags4 & RF4_BR_ELEC || r_ptr->flags9 & RF9_THUNDER_STORM)
	{
		if ((p_ptr->pclass == CLASS_IKU ||p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)); //非対象
		else
		{
			if (!p_ptr->immune_elec && (p_ptr->oppose_elec || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_ELEC) return (TRUE);
		}
	}

	if (r_ptr->flags4 & RF4_BR_COLD || r_ptr->flags9 & RF9_ICE_STORM)
	{
		if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) ;//非対象
		else if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) ;//非対象
		else
		{
			if (!p_ptr->immune_cold && (p_ptr->oppose_cold || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_COLD) return (TRUE);
		}
	}

	if (r_ptr->flags4 & (RF4_BR_POIS) || r_ptr->flags9 & RF9_TOXIC_STORM)
	{
		if ((p_ptr->pclass == CLASS_NINJA) && p_ptr->lev > 44);//非対象
		else if ((p_ptr->pclass == CLASS_MOMOYO) && p_ptr->lev > 19);
		else
		{
			if (p_ptr->oppose_pois || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST)) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_POIS) return (TRUE);
		}
	}

	/* Ultimate resistance */
	if (p_ptr->ult_res) return (TRUE);

	/* Potion of Neo Tsuyosi special */
	if (p_ptr->tsuyoshi) return (TRUE);

	/* Elemental Brands */
	if ((p_ptr->special_attack & ATTACK_ACID) && !(r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_FIRE) && !(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_ELEC) && !(r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_COLD) && !(r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_POIS) && !(r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)) return (TRUE);

	if (p_ptr->special_attack & ATTACK_DARK) return (TRUE);
	if (p_ptr->special_attack & ATTACK_FORCE) return (TRUE);

	/* Speed */
	if (p_ptr->pspeed < 145)
	{
		if (IS_FAST()) return (TRUE);
	}

	/* Light speed */
	if (p_ptr->lightspeed ) return (TRUE);

	if (p_ptr->riding && (m_list[p_ptr->riding].mspeed < 135))
	{
		if (MON_FAST(&m_list[p_ptr->riding])) return (TRUE);
	}

	/* No need to cast dispel spell */
	return (FALSE);
}

//v1.1.46 女苑特技「プラックピジョン」により上昇する魔法失敗率の計算
static int pluck_pigeon_magic_fail(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int tmp;

	//女苑「プラックピジョン」発動中のみ有効
	if (p_ptr->pclass != CLASS_JYOON) return 0;
	if (!p_ptr->tim_general[0]) return 0;

	//モンスターが視界内かどうかの判定はしない。
	//このルーチンはモンスターが＠に魔法を使うときの判定にしか影響しないため。

	//アイテムや財宝を一つでも持っていれば+10%、特別製ドロップはさらに+10%、さらに1個あたり失敗率2%上昇させる。
	//ユニークモンスターは効果半減。
	tmp = 0;
	if (r_ptr->flags1 & RF1_DROP_60) tmp += 120;
	if (r_ptr->flags1 & RF1_DROP_90) tmp += 180;
	if (r_ptr->flags1 & RF1_DROP_1D2) tmp += 300;
	if (r_ptr->flags1 & RF1_DROP_2D2) tmp += 600;
	if (r_ptr->flags1 & RF1_DROP_3D2) tmp += 900;
	if (r_ptr->flags1 & RF1_DROP_4D2) tmp += 1200;
	if (r_ptr->flags1 & RF1_DROP_GREAT) tmp += 1000;
	if (tmp) tmp += 1000;

	if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) tmp /= 2;


	return tmp / 100;
}

/*
 * Have a monster choose a spell from a list of "useful" spells.
 *
 * Note that this list does NOT include spells that will just hit
 * other monsters, and the list is restricted when the monster is
 * "desperate".  Should that be the job of this function instead?
 *
 * Stupid monsters will just pick a spell randomly.  Smart monsters
 * will choose more "intelligently".
 *
 * Use the helper functions above to put spells into categories.
 *
 * This function may well be an efficiency bottleneck.
 */
/*:::効果などいろいろ判定済みのspells[]からどの魔法を使うか選ぶ*/
///mon
static int choose_attack_spell(int m_idx, byte spells[], byte num)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加したのでここの配列サイズも一応96→128に拡張しておく
	byte escape[128], escape_num = 0;
	byte attack[128], attack_num = 0;
	byte summon[128], summon_num = 0;
	byte tactic[128], tactic_num = 0;
	byte annoy[128], annoy_num = 0;
	byte invul[128], invul_num = 0;
	byte haste[128], haste_num = 0;
	byte world[128], world_num = 0;
	byte special[128], special_num = 0;
	byte psy_spe[128], psy_spe_num = 0;
//	byte raise[128], raise_num = 0;
	byte heal[128], heal_num = 0;
	byte dispel[128], dispel_num = 0;

	int i;

	/* Stupid monsters choose randomly */
	if (r_ptr->flags2 & (RF2_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	///sys mon f9実装とモンスタースペル変更に伴いここの下の関数かなり変更要
	/* Categorize spells */
	for (i = 0; i < num; i++)
	{
		/* Escape spell? */
		if (spell_escape(spells[i])) escape[escape_num++] = spells[i];

		/* Attack spell? */
		if (spell_attack(spells[i])) attack[attack_num++] = spells[i];

		/* Summon spell? */
		if (spell_summon(spells[i])) summon[summon_num++] = spells[i];

		/* Tactical spell? */
		if (spell_tactic(spells[i])) tactic[tactic_num++] = spells[i];

		/* Annoyance spell? */
		if (spell_annoy(spells[i])) annoy[annoy_num++] = spells[i];

		/* Invulnerability spell? */
		if (spell_invulner(spells[i])) invul[invul_num++] = spells[i];

		/* Haste spell? */
		if (spell_haste(spells[i])) haste[haste_num++] = spells[i];

		/* World spell? */
		if (spell_world(spells[i])) world[world_num++] = spells[i];

		/* Special spell? */
		if (spell_special(spells[i])) special[special_num++] = spells[i];

		/* Psycho-spear spell? */
		if (spell_psy_spe(spells[i])) psy_spe[psy_spe_num++] = spells[i];

		/* Raise-dead spell? */
		//if (spell_raise(spells[i])) raise[raise_num++] = spells[i];

		/* Heal spell? */
		if (spell_heal(spells[i])) heal[heal_num++] = spells[i];

		/* Dispel spell? */
		if (spell_dispel(spells[i])) dispel[dispel_num++] = spells[i];
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if (world_num && (randint0(100) < 15) && !world_monster)
	{
		/* Choose haste spell */
		return (world[randint0(world_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch(m_ptr->r_idx)
		{
			case MON_BANOR:
			case MON_LUPART:
				if ((m_ptr->hp < m_ptr->maxhp / 2) && r_info[MON_BANOR].max_num && r_info[MON_LUPART].max_num) success = TRUE;
				break;

				//v1.1.36 小鈴はHPが減ったら高確率で付喪神を吸収する
			case MON_KOSUZU:
				if (randint1(m_ptr->hp) < m_ptr->maxhp / 2) success = TRUE;
				break;


			default: break;

		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Still hurt badly, couldn't flee, attempt to heal */
	if (m_ptr->hp < m_ptr->maxhp / 3 && one_in_(2))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Hurt badly or afraid, attempt to flee */
	if (((m_ptr->hp < m_ptr->maxhp / 3) || MON_MONFEAR(m_ptr)) && one_in_(2))
	{
		/* Choose escape spell if possible */
		if (escape_num) return (escape[randint0(escape_num)]);
	}

	/* special */
	///sys mon 特別な行動の成功判定
	if (special_num)
	{
		bool success = FALSE;
		switch (m_ptr->r_idx)
		{
			case MON_OHMU:
			case MON_BANOR:
			case MON_LUPART:
				break;
			case MON_MAI:
			case MON_SATONO:
			case MON_BANORLUPART:
				if (randint0(100) < 70) success = TRUE;
				break;
			case MON_ROLENTO:
			case MON_SEIJA:
			case MON_SEIJA_D:
				if (randint0(100) < 40) success = TRUE;
				break;
			default:
				///mod150216 特別行動の比率が高すぎるので50%→33%に減らす
				if (randint0(100) < 33) success = TRUE;
				break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if ((distance(py, px, m_ptr->fy, m_ptr->fx) < 4) && (attack_num || (r_ptr->flags6 & RF6_TRAPS)) && (randint0(100) < 75) && !world_monster)
	{
		/* Choose tactical spell */
		if (tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if (summon_num && (randint0(100) < 40))
	{
		/* Choose summon spell */
		return (summon[randint0(summon_num)]);
	}

	/* dispel */
	if (dispel_num && one_in_(2))
	{
		/* Choose dispel spell if possible */
		if (dispel_check(m_idx))
		{
			return (dispel[randint0(dispel_num)]);
		}
	}
	///sysdel 死人返し
	/* Raise-dead if possible (sometimes) */
#if 0
	if (raise_num && (randint0(100) < 40))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}
#endif

	/* Attack spell (most of the time) */
	if (IS_INVULN())
	{
		if (psy_spe_num && (randint0(100) < 50))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if (attack_num && (randint0(100) < 40))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if (attack_num && (randint0(100) < 85))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if (tactic_num && (randint0(100) < 50) && !world_monster)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if (invul_num && !m_ptr->mtimed[MTIMED_INVULNER] && (randint0(100) < 50))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if ((m_ptr->hp < m_ptr->maxhp * 3 / 4) && (randint0(100) < 25))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if (haste_num && (randint0(100) < 20) && !MON_FAST(m_ptr))
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if (annoy_num && (randint0(100) < 80))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return (0);
}

/*
 * スペル番号から非魔法スペルを判定する。
 */
bool spell_is_inate(u16b spell)
{
	if (spell < 32 * 4) /* Set RF4 */
	{
		if ((1L << (spell - 32 * 3)) & RF4_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 5) /* Set RF5 */
	{
		if ((1L << (spell - 32 * 4)) & RF5_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 6) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 5)) & RF6_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 7) /* Set RF9 */
	{
		if ((1L << (spell - 32 * 6)) & RF9_NOMAGIC_MASK) return TRUE;
	}

	/* This spell is not "inate" */
	return FALSE;
}
/*
 * Return TRUE if a spell is inate spell.
 */
bool spell_is_summon(u16b spell)
{
	if (spell < 32 * 4) /* Set RF4 */
	{
		if ((1L << (spell - 32 * 3)) & RF4_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 5) /* Set RF5 */
	{
		if ((1L << (spell - 32 * 4)) & RF5_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 6) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 5)) & RF6_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 7) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 6)) & RF9_SUMMON_MASK) return TRUE;
	}

	/* This spell is not "inate" */
	return FALSE;
}

//＠に直接射線が通らなくても隣のグリッドに射線が通るかどうかチェックしているらしい
static bool adjacent_grid_check(monster_type *m_ptr, int *yp, int *xp,
	int f_flag, bool (*path_check)(int, int, int, int))
{
	int i;
	int tonari;
	static int tonari_y[4][8] = {{-1, -1, -1,  0,  0,  1,  1,  1},
			                     {-1, -1, -1,  0,  0,  1,  1,  1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1}};
	static int tonari_x[4][8] = {{-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1},
			                     {-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1}};

	if (m_ptr->fy < py && m_ptr->fx < px) tonari = 0;
	else if (m_ptr->fy < py) tonari = 1;
	else if (m_ptr->fx < px) tonari = 2;
	else tonari = 3;

	for (i = 0; i < 8; i++)
	{
		int next_x = *xp + tonari_x[tonari][i];
		int next_y = *yp + tonari_y[tonari][i];
		cave_type *c_ptr;

		/* Access the next grid */
		c_ptr = &cave[next_y][next_x];

		/* Skip this feature */
		if (!cave_have_flag_grid(c_ptr, f_flag)) continue;

		if (path_check(m_ptr->fy, m_ptr->fx, next_y, next_x))
		{
			*yp = next_y;
			*xp = next_x;
			return TRUE;
		}
	}

	return FALSE;
}

#define DO_SPELL_NONE    0
#define DO_SPELL_BR_LITE 1
#define DO_SPELL_BR_DISI 2
#define DO_SPELL_BA_LITE 3
#define DO_SPELL_BA_DISI 4
#define DO_SPELL_TELE_HI_APPROACH 5
#define DO_SPELL_SPECIAL_AZATHOTH 6
#define DO_SPELL_WAVEMOTION 7
#define DO_SPELL_DESTRUCTION 8
#define DO_SPELL_FINALSPARK 9


/*
 * Creatures can cast spells, shoot missiles, and breathe.
 *
 * Returns "TRUE" if a spell (or whatever) was (successfully) cast.
 *
 * XXX XXX XXX This function could use some work, but remember to
 * keep it as optimized as possible, while retaining generic code.
 *
 * Verify the various "blind-ness" checks in the code.
 *
 * XXX XXX XXX Note that several effects should really not be "seen"
 * if the player is blind.  See also "effects.c" for other "mistakes".
 *
 * Perhaps monsters should breathe at locations *near* the player,
 * since this would allow them to inflict "partial" damage.
 *
 * Perhaps smart monsters should decline to use "bolt" spells if
 * there is a monster in the way, unless they wish to kill it.
 *
 * Note that, to allow the use of the "track_target" option at some
 * later time, certain non-optimal things are done in the code below,
 * including explicit checks against the "direct" variable, which is
 * currently always true by the time it is checked, but which should
 * really be set according to an explicit "projectable()" test, and
 * the use of generic "x,y" locations instead of the player location,
 * with those values being initialized with the player location.
 *
 * It will not be possible to "correctly" handle the case in which a
 * monster attempts to attack a location which is thought to contain
 * the player, but which in fact is nowhere near the player, since this
 * might induce all sorts of messages about the attack itself, and about
 * the effects of the attack, which the player might or might not be in
 * a position to observe.  Thus, for simplicity, it is probably best to
 * only allow "faulty" attacks by a monster if one of the important grids
 * (probably the initial or final grid) is in fact in view of the player.
 * It may be necessary to actually prevent spell attacks except when the
 * monster actually has line of sight to the player.  Note that a monster
 * could be left in a bizarre situation after the player ducked behind a
 * pillar and then teleported away, for example.
 *
 * Note that certain spell attacks do not use the "project()" function
 * but "simulate" it via the "direct" variable, which is always at least
 * as restrictive as the "project()" function.  This is necessary to
 * prevent "blindness" attacks and such from bending around walls, etc,
 * and to allow the use of the "track_target" option in the future.
 *
 * Note that this function attempts to optimize the use of spells for the
 * cases in which the monster has no spells, or has spells but cannot use
 * them, or has spells but they will have no "useful" effect.  Note that
 * this function has been an efficiency bottleneck in the past.
 *
 * Note the special "MFLAG_NICE" flag, which prevents a monster from using
 * any spell attacks until the player has had a single chance to move.
 */

/*:::モンスターが＠に対して魔法を使う処理*/
/*:::魔法、ブレス、射撃などを実行したらTRUEが返る(失敗でもTRUE　何もしなかったらFALSE) */
/*:::＠の盲目時にはメッセージが変わる*/
/*:::モンスターは＠の隣のマスに魔法を撃つことがある*/
/*:::SMARTなモンスターは味方のモンスターが間にいるとき＠にボルトを撃たない*/
/*:::詳細不明　座標チェックに関して何か言ってる*/
/*:::＠を中心でなく範囲内に捉える攻撃については完全には実装できていない*/
/*:::いくつかの攻撃処理はproject()を使わずグローバル変数に直接アクセスを行うので注意*/
/*:::モンスターが有用なスペルを持たないときの動作は最適を目指して調整してあるが昔はここがボトルネックだった？*/
///sys mon モンスター攻撃魔法フラグf9を追加する必要有り
///mod160618 引数special_flagを追加。輝夜に必ずテレポートを使わせるためのフラグ
bool make_attack_spell(int m_idx, int special_flag)
{
	int             k, thrown_spell = 0, rlev, failrate;

	///mod140102 モンスター新魔法フラグ追加に伴いflags9追加
	byte            spell[128], num = 0;
	u32b            f4, f5, f6, f9;
	//byte            spell[96], num = 0;
	//u32b            f4, f5, f6;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];
#ifndef JP
	char            m_poss[80];
#endif
	bool            no_inate = FALSE;
	bool            do_spell = DO_SPELL_NONE;
	int             dam = 0;
	u32b mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);
	int rad = 0; //For elemental spells

	int hecatia_change_idx = 0; //ヘカーティア特殊処理用
	int rankup_monster_idx = 0; //舞＆里乃特殊処理用

	/* Target location */
	int x = px;
	int y = py;

	/* Target location for lite breath */
	int x_br_lite = 0;
	int y_br_lite = 0;

	int caster_dist=m_ptr->cdis;//モンスターが魔法を使った瞬間の＠からの距離

	/* Summon count */
	int count = 0;

	/* Extract the blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && m_ptr->ml);

	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (seen && maneable && !world_monster);

	///mod151228 Hack - サイバイマン専用自爆　自爆するとこれがTRUEになり、関数終了直前にdelete_monster_idx(m_idx)する。
	bool suisidebomb = FALSE;

	/* Check "projectable" */
	bool direct;
	/*:::反魔法洞窟フラグ*/
	bool in_no_magic_dungeon = (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && dun_level
		&& (!p_ptr->inside_quest || is_fixed_quest_idx(p_ptr->inside_quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	//職業ぬえのとき敵が正体不明を見破る確率の係数(%) 負になると必ず見破られる
	int nue_check_mult = 100;

	//v1.1.95 狂戦士化状態だと魔法や特殊行動を使えない
	if (MON_BERSERK(m_ptr))
	{
		return (FALSE);
	}

	/* Cannot cast spells when confused */
	if (MON_CONFUSED(m_ptr))
	{
		reset_target(m_ptr);
		return (FALSE);
	}

	//マミゾウ二ッ岩家の裁き
	if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == m_idx)
		return (FALSE);

	/* Cannot cast spells when nice */
	if (m_ptr->mflag & MFLAG_NICE) return (FALSE);
	if (!is_hostile(m_ptr) && !special_flag) return (FALSE);

	///mod141005 幽々子の「死出の誘蛾灯」
	//v1.1.76 記述変更 視界内敵対モンスターに確率で魔法使用禁止処理
	//if (p_ptr->pclass == CLASS_YUYUKO && m_ptr->mflag & MFLAG_SPECIAL) return (FALSE);
	if (p_ptr->pclass == CLASS_YUYUKO && p_ptr->tim_general[0]	&& !is_pet(m_ptr) && los(py, px, m_ptr->fy, m_ptr->fx))
	{

		int chance = p_ptr->lev * 3 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) chance /= 2;
		if (r_ptr->level < randint1(chance))
		{
			monster_desc(m_name, m_ptr, 0);
			msg_format(_("%sは光に誘われている...", "%s is entranced by the light..."), m_name);
			return (FALSE);
		}
	}


	/*:::innate(生来の) ブレス、叫びなど肉体的な魔法の制限　魔法使用率の低いモンスターは魔法使用時にブレスなどを使わないことが多いらしいが主旨がよく解らん*/
	/* Sometimes forbid inate attacks (breaths) */
	if (randint0(100) >= (r_ptr->freq_spell * 2)) no_inate = TRUE;

	/* XXX XXX XXX Handle "track_target" option (?) */


	/* Extract the racial spell flags */
	/*:::モンスターの持っているフラグ*/
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;
	f9 = r_ptr->flags9;

	/*** require projectable player ***/

	/* Check range */
	/*:::MAX_RANGE:18 ＠が遠くにいて他にターゲットがいなければ何もしない*/

	///mod140103 視界外テレポートを持つ敵は＠が遠くてもまだ終わらないようにした。バグのもとになりそうな気もする。
	if (!(f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis > MAX_RANGE) && !m_ptr->target_y) return (FALSE);
	//if ((m_ptr->cdis > MAX_RANGE) && !m_ptr->target_y) return (FALSE);


	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	//v1.1.94 モンスター魔法力低下中はレベル判定値25%カット
	if (MON_DEC_MAG(m_ptr))
	{
		rlev = (rlev * 3 + 1) / 4;
	}

	/*:::閃光、分解ブレスの特殊優先処理*/
	///system 閃光ブレスの優先使用処理　閃光属性レーザーとか実装したら追加すべきだろう
	/* Check path for lite breath */
	if (f4 & RF4_BR_LITE)
	{
		y_br_lite = y;
		x_br_lite = x;

		if (los(m_ptr->fy, m_ptr->fx, y_br_lite, x_br_lite))
		{
			feature_type *f_ptr = &f_info[cave[y_br_lite][x_br_lite].feat];

			if (!have_flag(f_ptr->flags, FF_LOS))
			{
				if (have_flag(f_ptr->flags, FF_PROJECT) && one_in_(2)) f4 &= ~(RF4_BR_LITE);
			}
		}

		/* Check path to next grid */
		else if (!adjacent_grid_check(m_ptr, &y_br_lite, &x_br_lite, FF_LOS, los)) f4 &= ~(RF4_BR_LITE);

		/* Don't breath lite to the wall if impossible */
		if (!(f4 & RF4_BR_LITE))
		{
			y_br_lite = 0;
			x_br_lite = 0;
		}
	}

	///mod140613 恐怖している敵は＠への隣接テレポを使わない
	if (MON_MONFEAR(m_ptr))
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}
	//配下は*破壊*や時間停止を使わない
	if(is_pet(m_ptr))
	{
		f9 &= ~(RF9_DESTRUCTION);
		f6 &= ~(RF6_WORLD);
	}

	///mod140115 ＠に隣接した敵は隣接テレポを使わないようにする
	if ((f9 & (RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH)) && m_ptr->cdis < 2)
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}
	///mod140714 ＠から離れた敵はメイルシュトロムを使わないようにする v1.1.64 7→4
	if ((f9 & RF9_MAELSTROM) && m_ptr->cdis > 4 && !(r_ptr->flags2 & RF2_STUPID) )
	{
		f9 &= ~(RF9_MAELSTROM);
	}
	///＠がvault内に居るときは視界外隣接テレポで飛んでこないようにする
	if (f9 & RF9_TELE_HI_APPROACH)
	{
		cave_type    *c_ptr = &cave[y][x];
		if(c_ptr->info & CAVE_ICKY)	f9 &= ~(RF9_TELE_HI_APPROACH);
	}
	//ルナチャ静寂
	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1]))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f4 &= ~(RF4_SHRIEK | RF4_BR_SOUN);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);
	}
	//ヘカーティア特殊処理　今の体よりHPが多く倒されてないのを選定 いなければ特別行動しない
	if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
	{
		int i;
		int current_hp = m_ptr->hp;
		for(i=0;i<3;i++)
		{
			if(m_ptr->r_idx ==(MON_HECATIA1+i)) continue;
			if(r_info[MON_HECATIA1+i].r_akills) continue;
			if((int)hecatia_hp[i] < current_hp) continue;
			current_hp = hecatia_hp[i];
			hecatia_change_idx = MON_HECATIA1+i;

		}
		if(!hecatia_change_idx) f4 &= ~(RF4_SPECIAL2);
		//ピンチのとき高確率で交代する
		else if(m_ptr->hp < 2000 && one_in_(2))
		{
			f4 = RF4_SPECIAL2;
			f5 = 0L;
			f6 = 0L;
			f9 = 0L;
		}
	}
	//サイバイマンはターゲットが近くにいて体力が低くないと特別行動をしない
	if(m_ptr->r_idx == MON_SAIBAIMAN)
	{
		if(m_ptr->hp > m_ptr->maxhp / 3 || m_ptr->cdis > 1)
			f4 &= ~(RF4_SPECIAL2);
	}

	//既にランダムユニーク2がフロアにいる場合隠岐奈は特別行動2をしない
	if (m_ptr->r_idx == MON_OKINA && r_info[MON_RANDOM_UNIQUE_2].cur_num)
	{
		if (cheat_hear) msg_print(_("msg:すでにランダムユニークがフロアにいるので隠岐奈の特別行動が抑止された",
                                    "msg: Random unique monster already present on the floors; stopped Matara's special action"));
		f4 &= ~(RF4_SPECIAL2);
	}

	//v1.1.91 尤魔はHPが減ってて周囲に石油地形があるときにしか特別行動をしない。あと闘技場やアリーナでは特別行動をしない。
	if (m_ptr->r_idx == MON_YUMA)
	{
		if (p_ptr->inside_arena || p_ptr->inside_battle || (m_ptr->hp > m_ptr->maxhp * 3 / 4) || yuma_vacuum_oil(m_ptr->fy,m_ptr->fx,TRUE,0) < 1)
		{
			f4 &= ~(RF4_SPECIAL2);
		}
	}

	//v1.1.36 小鈴は付喪神がフロアにいるかダメージを受けていないと特別行動をしない
	if(m_ptr->r_idx == MON_KOSUZU)
	{
		int j;
		bool no_use = FALSE;

		if(m_ptr->hp == m_ptr->maxhp)
			no_use = TRUE;
		else
		{
			for (j = 1; j < m_max; j++)
			{
				monster_type *tmp_m_ptr = &m_list[j];
				if (tmp_m_ptr->r_idx != MON_TSUKUMO_1 && tmp_m_ptr->r_idx != MON_TSUKUMO_2 ) continue;
				break;
			}
			if(j == m_max) no_use = TRUE;
		}

		if(no_use)
			f4 &= ~(RF4_SPECIAL2);
	}


	/* Check path */

	//特殊モード　必ず隣接テレポ
	if(special_flag == 1)
	{
			do_spell = DO_SPELL_TELE_HI_APPROACH;

	}
	//＠とモンスターの間に射線が通っているとき
	else if (projectable(m_ptr->fy, m_ptr->fx, y, x))
	{
		feature_type *f_ptr = &f_info[cave[y][x].feat];

		//＠が視界内の壁やガラスの壁の中にいるとき分解など減衰しない攻撃を1/2で選択するらしい
		if (!have_flag(f_ptr->flags, FF_PROJECT))
		{
			/* Breath disintegration to the wall if possible */
			if ((f4 & RF4_BR_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BR_DISI;
			///mod140103 原子分解を壁の中の＠に向けて使いやすくした
			if ((f9 & RF9_BA_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BA_DISI;

			if ((f9 & RF9_FINALSPARK) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(3)) do_spell = DO_SPELL_FINALSPARK;

			/* Breath lite to the transparent wall if possible */
			else if ((f4 & RF4_BR_LITE) && have_flag(f_ptr->flags, FF_LOS) && one_in_(2)) do_spell = DO_SPELL_BR_LITE;
		}
	}
	//＠とモンスターの間に射線が通っていないとき
	/* Check path to next grid */
	else
	{
		bool success = FALSE;

		//視界外隣接テレポ用の定数
		int dev;
		if(difficulty == DIFFICULTY_EASY) dev = 10;
		else if(difficulty == DIFFICULTY_NORMAL) dev = 5;
		else if(difficulty == DIFFICULTY_LUNATIC) dev = 1;
		else dev = 3;


		/*:::視界外から分解ブレスとか吐くのはここらしい*/
		///system いずれ破壊とか分解ボールとか実装したらここに追加
		///視界外の20-5くらいの距離で治癒や加速や叫びを使うようにしてもいい。SMARTは30%以下で1/5、STUPID以外は15%以下で1/10の確率で行う程度がいいか
		if ((f4 & RF4_BR_DISI) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BR_DISI;
			success = TRUE;
		}

		if ((f4 & RF4_WAVEMOTION) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10)))
		{
			do_spell = DO_SPELL_WAVEMOTION;
			success = TRUE;
		}
		else if ((f9 & RF9_FINALSPARK) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_FINALSPARK;
			success = TRUE;
		}
		///mod140103 原子分解の視界外攻撃を実装した
		else if ((f9 & RF9_BA_DISI) && (m_ptr->cdis < 5) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(7) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BA_DISI;
			success = TRUE;
		}
		///視界外隣接テレポートを実装してみた　レベル*2の距離からなら飛んでくる（あまり離れた敵は行動判定がない処理もあるが）
		///mod160326 ちょっと射程距離短くした
		//else if ((f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis < rlev * 2) && one_in_(10))
		else if ((f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis < (r_ptr->aaf / dev)) && one_in_(10))
		{
			do_spell = DO_SPELL_TELE_HI_APPROACH;
			success = TRUE;
		}
		///アザトートの特殊攻撃
		else if(m_ptr->r_idx == MON_AZATHOTH && m_ptr->cdis < 10 && one_in_(6))
		{
			do_spell = DO_SPELL_SPECIAL_AZATHOTH;
			success = TRUE;
		}



		else if ((f4 & RF4_BR_LITE) && (m_ptr->cdis < MAX_RANGE/2) &&
		    los(m_ptr->fy, m_ptr->fx, y, x) && one_in_(5))
		{
			do_spell = DO_SPELL_BR_LITE;
			success = TRUE;
		}
		else if ((f5 & RF5_BA_LITE) && (m_ptr->cdis <= MAX_RANGE))
		{
			int by = y, bx = x;
			get_project_point(m_ptr->fy, m_ptr->fx, &by, &bx, 0L);
			if ((distance(by, bx, y, x) <= 3) && los(by, bx, y, x) && one_in_(5))
			{
				do_spell = DO_SPELL_BA_LITE;
				success = TRUE;
			}
		}

		//＠とモンスターとの間に視界が通っておらず、分解など他に使える特技もないとき、＠の隣のグリッドに射線が通っているかチェックし通ってればx,yを変更
		//if (!success && p_ptr->wizard) msg_print("no success");
		if (!success) success = adjacent_grid_check(m_ptr, &y, &x, FF_PROJECT, projectable);

		//それでも射線が通ってなければ召喚やテレポなど一部の特技のみ使う　いわゆる反撃召喚もここ
		if (!success)
		{

			if (m_ptr->target_y && m_ptr->target_x)
			{
				y = m_ptr->target_y;
				x = m_ptr->target_x;
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);
				success = TRUE;
			}

			if (y_br_lite && x_br_lite && (m_ptr->cdis < MAX_RANGE/2) && one_in_(5))
			{
				if (!success)
				{
					y = y_br_lite;
					x = x_br_lite;
					do_spell = DO_SPELL_BR_LITE;
					success = TRUE;
				}
				else f4 |= (RF4_BR_LITE);
			}
		}

		/* No spells */
		if (!success) return FALSE;
	}


	/*::: Hack- 破壊使用判定処理 */
	if (r_ptr->flags2 & RF2_SMART && (f9 & RF9_DESTRUCTION))
	{
		int j;
		int mcount = 0;
		for (j = 1; j < m_max; j++)
		{
			monster_race *r_ptr_tmp = &r_info[m_list[j].r_idx];
			if(distance( m_ptr->fy, m_ptr->fx,m_list[j].fy,m_list[j].fx) < 10 && r_ptr_tmp->level > r_ptr->level / 2 && are_enemies(m_ptr,&m_list[j])) mcount++;
		}
		//msg_format("CHECK:cnt=%d ",mcount);
		if(mcount > randint0(10) )
		{
			do_spell = DO_SPELL_DESTRUCTION;
		}
	}

	reset_target(m_ptr);

	/* Forbid inate attacks sometimes */
	/*:::ブレス、叫び、射撃、ロケット、特別な行動が該当する。no_inateのときこれらを使わない*/
	if (no_inate)
	{
		f4 &= ~(RF4_NOMAGIC_MASK);
		f5 &= ~(RF5_NOMAGIC_MASK);
		f6 &= ~(RF6_NOMAGIC_MASK);
		f9 &= ~(RF9_NOMAGIC_MASK);
	}
	///class 忍者特殊処理
	if (f6 & RF6_DARKNESS)
	{
		if ((p_ptr->pclass == CLASS_NINJA || p_ptr->pclass == CLASS_NUE) &&
		    !(r_ptr->flags3 & RF3_UNDEAD) &&
///mod131231 モンスターフラグ変更 閃光弱点RF3からRFRへ
			!(r_ptr->flagsr & RFR_HURT_LITE) &&
		    !(r_ptr->flags7 & RF7_DARK_MASK))
			can_use_lite_area = TRUE;

		/*:::＠が忍者のとき、暗闇の魔法を使う敵が代わりにライトエリアを使うことがあるらしい*/
		if (!(r_ptr->flags2 & RF2_STUPID))
		{
			if (d_info[dungeon_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			else if ((p_ptr->pclass == CLASS_NINJA || p_ptr->pclass == CLASS_NUE) && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}
	/*:::反魔法洞窟では魔法を使わない（STUPIDは使う）*/
	///mod140423 「敵を黙らせる」効果中も同様に処理
	///mod150604 映姫が舌を封じた敵も追加
	if ((in_no_magic_dungeon || p_ptr->silent_floor || (m_ptr->mflag & MFLAG_NO_SPELL)) && !(r_ptr->flags2 & RF2_STUPID))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);
	}

	//v1.1.36 エタニティラルバは恐怖時にしか悪臭雲を使わないようにした
	if(m_ptr->r_idx == MON_LARVA)
	{
		if (!MON_MONFEAR(m_ptr))
		{
			f5 &= ~(RF5_BA_POIS);
		}
	}

	/*:::SMARTな敵はピンチのとき攻撃魔法よりテレポートや治癒や召喚やバステを多用するようになる*/
	///sys SMARTな敵がピンチのときの行動選定　もっと細かく分けよう
	if (r_ptr->flags2 & RF2_SMART)
	{
		/* Hack -- allow "desperate" spells */
		if ((m_ptr->hp < m_ptr->maxhp / 10) &&
			(randint0(100) < 50))
		{
			/* Require intelligent spells */
			f4 &= (RF4_INT_MASK);
			f5 &= (RF5_INT_MASK);
			f6 &= (RF6_INT_MASK);
			f9 &= (RF9_INT_MASK);

			//v1.1.91 尤魔の特別行動とメイルシュトロムは回復も兼ねているので省かない
			if (m_ptr->r_idx == MON_YUMA)
			{
				f4 |= RF4_SPECIAL2;
				f9 |= RF9_MAELSTROM;
			}

		}

		/* Hack -- decline "teleport level" in some case */
		if ((f6 & RF6_TELE_LEVEL) && TELE_LEVEL_IS_INEFF(0))
		{
			f6 &= ~(RF6_TELE_LEVEL);
		}
	}





	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return (FALSE);


	/* Remove the "ineffective" spells */
	/*:::呪文学習に伴い効果の低い呪文を弾く処理*/
	remove_bad_spells(m_idx, &f4, &f5, &f6, &f9);

	//アリーナと闘技場では召喚を使わない。
	//v1.1.43 松明で狂気状態になると召喚を使わないことにしておく
	if (p_ptr->inside_arena || p_ptr->inside_battle
		|| m_ptr->mflag & MFLAG_LUNATIC_TORCH)
	{
		f4 &= ~(RF4_SUMMON_MASK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK | RF6_TELE_LEVEL);
		f9 &= ~(RF9_SUMMON_MASK);

		if (m_ptr->r_idx == MON_FLAN) f6 &= ~(RF6_SPECIAL);
		if (m_ptr->r_idx == MON_ROLENTO) f6 &= ~(RF6_SPECIAL);
		if (m_ptr->r_idx == MON_OKINA) f4 &= ~(RF4_SPECIAL2);
		if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI) f6 &= ~(RF6_SPECIAL); //v1.1.32
	}
	//諏訪子睡眠中は多くの攻撃魔法が使われない
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		f4 &= (RF4_SHRIEK | RF4_WAVEMOTION | RF4_BR_DISI);
		f5 = 0L;
		f6 &= (RF6_HASTE | RF6_HEAL | RF6_BLINK | RF6_TPORT | RF6_PSY_SPEAR | RF6_DARKNESS | RF6_TRAPS | RF6_SUMMON_MASK);
		f9 &= (RF9_DESTRUCTION | RF9_BA_DISI | RF9_SUMMON_MASK | RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH | RF9_ALARM);

	}

	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return (FALSE);

	/*:::STUPIDでないとき、＠に魔力がないとき魔力吸収を使わない、味方に当たるボルトを撃たない、無駄な召喚をしない*/
	if (!(r_ptr->flags2 & RF2_STUPID))
	{
		if (!p_ptr->csp) f5 &= ~(RF5_DRAIN_MANA);

		/* Check for a clean bolt shot */
		if (((f4 & RF4_BOLT_MASK) ||
		     (f5 & RF5_BOLT_MASK) ||
		     (f6 & RF6_BOLT_MASK) ||
		     (f9 & RF9_BOLT_MASK) ) &&
		    !clean_shot(m_ptr->fy, m_ptr->fx, py, px, FALSE))
		{
			/* Remove spells that will only hurt friends */
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
		}

		/* Check for a possible summon */
		if (((f4 & RF4_SUMMON_MASK) ||
		     (f5 & RF5_SUMMON_MASK) ||
		     (f6 & RF6_SUMMON_MASK) ||
		     (f9 & RF9_SUMMON_MASK)) &&
		    !(summon_possible(y, x)))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		//v1.1.13 モンスターがSMARTなとき「公転周期の罠」が有効だとほとんど召喚を試みない
		//v1.1.54 ネムノ特技追加
		/* Check for a possible summon */
		if ((r_ptr->flags2 & RF2_SMART)
			&&( (CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO))|| (p_ptr->special_flags & SPF_ORBITAL_TRAP))
			&&((f4 & RF4_SUMMON_MASK) ||(f5 & RF5_SUMMON_MASK) ||(f6 & RF6_SUMMON_MASK) ||(f9 & RF9_SUMMON_MASK))
			&& !one_in_(4))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		///del131221 死人返し
		/* Check for a possible raise dead */
#if 0
		if ((f6 & RF6_RAISE_DEAD) && !raise_possible(m_ptr))
		{
			/* Remove raise dead spell */
			f6 &= ~(RF6_RAISE_DEAD);
		}
#endif
//		f6 &= ~(RF6_RAISE_DEAD);
		/* Special moves restriction */
		///sys RF6の特殊行動の使用判定
		if (f6 & RF6_SPECIAL)
		{
			if ((m_ptr->r_idx == MON_ROLENTO) && !summon_possible(y, x))
			{
				f6 &= ~(RF6_SPECIAL);
			}
			/*:::フラン特殊処理　分身が既に居るときはフォーオブアカインドを使わない*/
			if(m_ptr->r_idx == MON_FLAN)
			{
				int j;
				for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_FLAN_4) f6 &= ~(RF6_SPECIAL);
			}
			///mod140626 霊夢は壁際では夢想封印を使えない
			if(m_ptr->r_idx == MON_REIMU)
			{
				if (!in_bounds2(m_ptr->fy + 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy + 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy - 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy - 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx + 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx + 1],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx - 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx - 1],FF_PROJECT))
				f6 &= ~(RF6_SPECIAL);
			}

			//v1.1.32 舞＆里乃の特別行動　自分の周囲にランクアップ可能なモンスターがいるかどうか判定し、いなければ特別行動解除
			if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI)
			{
				rankup_monster_idx = can_rankup_monster(m_ptr);

				if(!rankup_monster_idx)
					f6 &= ~(RF6_SPECIAL);
			}

			if(m_ptr->r_idx == MON_OKINA)
			{
				if(p_ptr->word_recall || p_ptr->inside_arena)
					f6 &= ~(RF6_SPECIAL);

			}

			//v1.1.78 美宵は＠に隣接していないと特別行動をしない
			if (m_ptr->r_idx == MON_MIYOI)
			{
				if (m_ptr->cdis > 1) f6 &= ~(RF6_SPECIAL);

			}

		}

		/* No spells left */
		if (!f4 && !f5 && !f6 && !f9) return (FALSE);
	}


	/*:::これまで整頓したフラグをspell[]に展開*/
	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (f4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (f5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (f6 & (1L << k)) spell[num++] = k + 32 * 5;
	}
	/*:::新しく追加したモンスタースペル */
	for (k = 0; k < 32; k++)
	{
		if (f9 & (1L << k)) spell[num++] = k + 32 * 6;
	}

	/*:::ここにf9flagを実装するには32*6でいいのか？問題ないと思うがなんで普通に0から始めないんだろう*/
	/*:::flags1を0とした値に揃えているらしい。ひょっとしてどこかで何かフラグ全体にかかわる処理に使っているのか？*/



	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!p_ptr->playing || p_ptr->is_dead) return (FALSE);

	/* Stop if player is leaving */
	if (p_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0x00);

	/* Projectable? */
	direct = player_bold(y, x);


#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif
/*
	//v1.1.85 射線が通っているが視界が遮られている(新地形「深い魔法吹雪」など)とき、SMARTでないモンスターは魔法を撃ってこないことにする？
	//隣接してると撃つ。すでに分解など撃つ魔法が決まってるときも撃つ。
	//この処理の場合、ガラスの壁と深い瘴気に挟まれているとガラスの壁の向こうから閃光ブレスを撃たれたりする。改善要。
	if (!(r_ptr->flags2 & RF2_SMART) && !do_spell )
	{
	(追記)
	距離2のとき敵が魔法を撃ったり撃たなかったりする。
	最初から射線が通ってるとここで撃たなくなるのだが、射線から外れてると「＠の隣のグリッドにターゲットをずらす」処理が先に来てターゲットとモンスターが隣接するせい。
	ターゲットとモンスターが隣接してるとLOSフラグがなくても見えている扱いになるらしい。
	どうすれば矛盾のない処理になる？隣のグリッドにターゲットをずらす処理自体に同じような処理をつけるべきか。

	別にこんな処理せず視界が遮られてても射線が通ってればバカスカ撃ってきてもいいんだが、森林生成とのイメージの整合が悩ましい。
	森林生成は視界を遮って魔法を防ぐと思われてるが、内部的には「見えないから撃たない」じゃなくて「撃っても木に当たって届かないから撃たない」と判断しているので霧の向こうから撃ってきても別に矛盾はないんだが。
	いい案が思いつかんので後回しにする。これを解決して実装したら、monst-monstの関数も同じように処理すること

	(追記2)
	実際に視界を遮る実装をしてみたが、龍窟や中層鉄獄辺りですごく事故要因になりそう。
	シンボルを~から#にしたら画面がうるさいし無理に初見殺し要素を追加するのも何なので結局視界を遮る処理自体を当面やめとくことにした。
	いずれ「霧が深くて前が見えない」みたいな場所が公式で出たら新ダンジョンとかで実装しよう。


		if (direct && !los(m_ptr->fy, m_ptr->fx, y, x) || !direct && cave_have_flag_bold(y,x,FF_LOS))
		{
			if(p_ptr->wizard)msg_format("視界が遮られているため魔法を撃たない(距離%d)",m_ptr->cdis);
			return (FALSE);
		}

	}
	*/

	/*:::最初のほうで分解ブレスや閃光ブレスを使うと決まったらそのインデックスを、そうでなければspell[]から選ぶ*/
	switch (do_spell)
	{
	case DO_SPELL_NONE:
		{
			int attempt = 10;
			while (attempt--)
			{
				thrown_spell = choose_attack_spell(m_idx, spell, num);
				if (thrown_spell) break;
			}
		}
		break;

		///sys 分解ブレスなどの特殊処理ここにも
	case DO_SPELL_BR_LITE:
		thrown_spell = 96+14; /* RF4_BR_LITE */
		break;

	case DO_SPELL_BR_DISI:
		thrown_spell = 96+31; /* RF4_BR_DISI */
		break;

	case DO_SPELL_BA_LITE:
		thrown_spell = 128+20; /* RF5_BA_LITE */
		break;

	///mod140626 ファイナルスパーク特殊処理実装
	case DO_SPELL_FINALSPARK:
		thrown_spell = 192+22; /* RF9_FINALSPARK */
		break;
	///mod140103 原子分解特殊処理実装
	case DO_SPELL_BA_DISI:
		thrown_spell = 192+6; /* RF9_BA_DISI */
		break;
	///mod140103 視界外隣接テレポート特殊処理実装
	case DO_SPELL_TELE_HI_APPROACH:
		thrown_spell = 192+29; /* RF9_BA_DISI */
		break;
	case DO_SPELL_SPECIAL_AZATHOTH:
		thrown_spell = 96+28; /* RF4_SPECIAL2 */
		break;
	case DO_SPELL_WAVEMOTION:
		thrown_spell = 96+16; /* RF4_WAVEMOTION */
		break;
	case DO_SPELL_DESTRUCTION:
		thrown_spell = 192+24; /* RF9_DESTRUCTION */
		break;


	default:
		return FALSE; /* Paranoia */
	}

	/* Abort if no spell was chosen */
	if (!thrown_spell) return (FALSE);

	/* Calculate spell failure rate */
	/*:::魔法失敗率計算　STUPIDは失敗しない*/
	///system 失敗率計算は少し変えてもいいかも
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid monsters will never fail (for jellies and such) */
	if (r_ptr->flags2 & RF2_STUPID) failrate = 0;

	//v1.1.94 魔法力低下中はさらに失敗率25%上昇
	if (MON_DEC_MAG(m_ptr)) failrate += 25;

	//v1.1.46 女苑「プラックピジョン」による魔法失敗率上昇
	failrate += pluck_pigeon_magic_fail(m_ptr);

	//v1.1.48
	if (SUPER_SHION && failrate < 50) failrate = 50;

	if (cheat_xtra) msg_format("failrate:%d%%",failrate);

	/* Check for spell failure (inate attacks never fail) */
	/*:::失敗率チェック　朦朧としてると1/2で失敗*/
	///mod150604 映姫が舌を封じた敵の処理追加
	if (!spell_is_inate(thrown_spell) && !special_flag
	    && (in_no_magic_dungeon || (MON_STUNNED(m_ptr) && one_in_(2)) || (randint0(100) < failrate) || (m_ptr->mflag & MFLAG_NO_SPELL)))
	{
		disturb(1, 1);
		/* Message */
#ifdef JP
		msg_format("%^sは呪文を唱えようとしたが失敗した。", m_name);
#else
		msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}
	//メルラン・ハッピーライブ
	if(p_ptr->pclass == CLASS_MERLIN && !spell_is_inate(thrown_spell) &&  music_singing(MUSIC_NEW_MERLIN_SOLO) && distance(py,px,m_ptr->fy,m_ptr->fx) < MAX_RANGE)
	{
		//v1.1.65 三項演算子の優先順位を間違えててtmp値が1か2になってしまっていたので修正
		int tmp = rlev * ((r_ptr->flags2 & RF2_POWERFUL)?2:1);

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)) tmp *= 2;

		if(randint1(tmp) < (p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]]))
		{
			if(one_in_(3)) msg_format(_("%^sは呪文を唱えようとしてあらぬことを口走った。",
                                        "%^s tries to cast a spell, but fails to speak correctly."), m_name);
			else if(one_in_(2))msg_format(_("%^sはシアワセだ。", "%^s looks happy."), m_name);
			else msg_format(_("%^sは少しアレな様子だ。", "%^s is acting a bit strangely."), m_name);
			return (TRUE);
		}
	}


	/* Hex: Anti Magic Barrier */
	if (!spell_is_inate(thrown_spell) && magic_barrier(m_idx))
	{
#ifdef JP
		msg_format("反魔法バリアが%^sの呪文をかき消した。", m_name);
#else
		msg_format("Anti magic barrier cancels the spell which %s casts.");
#endif
		return (TRUE);
	}
	//旧神の印による召喚妨害処理
	if(spell_is_summon(thrown_spell) && (r_ptr->flags3 & RF3_ANG_CHAOS))
	{
		int xx,yy;

		for(xx=x-10;xx<x+10;xx++) for(yy=y-10;yy<y+10;yy++)
		{
			if(!in_bounds(yy,xx)) continue;
			if(!is_elder_sign(&cave[yy][xx])) continue;
			msg_format(_("旧神の印が%^sの召喚魔法を無効化した！", "The sign of Elder Gods prevents %s from summoning!"), m_name);
			return (TRUE);
		}
	}
	if(spell_is_summon(thrown_spell) && (p_ptr->special_flags & SPF_ORBITAL_TRAP))
	{
		msg_format(_("%^sが何かを召喚しようとしたが公転周期のトラップに阻害された。",
                    "%^s tries to summon help, but the orbital trap obstructs it."), m_name);
		return (TRUE);
	}

	if (spell_is_summon(thrown_spell) && CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO) && randint1(rlev) < p_ptr->lev)
	{
		msg_format(_("%^sが何かを召喚しようとしたが、あなたは不躾な来訪者を拒絶した。",
                    "%^s tries to summon help, but you reject the uninvited guests."), m_name);
		return (TRUE);
	}

	if (spell_is_summon(thrown_spell) && SUPER_SHION && randint1(rlev) < p_ptr->lev)
	{
		msg_format(_("%^sがモンスターを召喚したが、モンスターは回れ右して帰っていった。",
                    "%^s summons a monster, but it turns around and returns."), m_name);
		return (TRUE);
	}

	//送還術による召喚妨害
	if(spell_is_summon(thrown_spell) && p_ptr->deportation )
	{
		int power = p_ptr->lev;

		if (p_ptr->pclass == CLASS_OKINA) power *= 2;

		if (randint0(rlev) < power)
		{
			msg_format(_("%^sが何かを召喚しようとしたがあなたは妨害した。",
                        "%^s tries to summon help, but you prevent it."), m_name);
			return (TRUE);

		}

	}
	if ((p_ptr->pclass == CLASS_MARISA && p_ptr->tim_general[0] || check_activated_nameless_arts(JKF1_ABSORB_MAGIC))
		&& !spell_is_inate(thrown_spell) )
	{
		int power = rlev;
		int chance = p_ptr->lev;

		if(p_ptr->pclass == CLASS_JUNKO) chance *= 2;
		if(r_ptr->flags2 & RF2_POWERFUL) power = power * 3 / 2;
		if(randint1(power) < chance)
		{
			int monspell_num = thrown_spell - 95;

			if(monspell_num < 1 || monspell_num > MAX_MONSPELLS2)
			{
				msg_format(_("ERROR:マジックアブソーバーの敵魔法処理がおかしい",
                            "ERROR: Weird logic enemy spells for magic absorber"));
			}
			else if(monspell_list2[monspell_num].level) //ダミーや特別行動は非対象
			{
				msg_format(_("あなたは%^sが唱えていた魔法を吸収した！",
                            "You absorb the spell cast by %s!"), m_name);
				p_ptr->csp += monspell_list2[monspell_num].smana;
				if(p_ptr->csp > p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();
				return (TRUE);
			}
		}
		else
		{
				msg_format(_("%^sの魔法の妨害に失敗した！",
                            "You failed to prevent %s from casting a spell!"), m_name);
		}
	}


	//v1.1.73 八千慧のモンスター買収フラグ処理
	if (spell_is_summon(thrown_spell) && p_ptr->pclass == CLASS_YACHIE && p_ptr->magic_num1[0] && randint1(rlev) < p_ptr->lev)
	{
		flag_bribe_summon_monsters = TRUE;
	}



	can_remember = is_original_ap_and_seen(m_ptr);


	/* Cast the spell. */
	/*:::ここから先、spell[]の使用番号でスイッチ　なぜかswitch文は定数宣言でなくハードコーディング*/
	///sys mon res ここから先モンスタースペルの実行部分 いくつかは耐性処理も記述
	switch (thrown_spell)
	{
		/* RF4_SHRIEK */
		case 96+0:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_KUTAKA)
			{
				msg_format(_("「コケーー！」「ピヨ！」",
                            "'Cock-a-doodle-doo!' 'Chirp!'"));
			}
			else
			{
				msg_format(_("%^sがかん高い叫び声をあげた。",
                            "%^s screams loudly."), m_name);

			}

			aggravate_monsters(m_idx,FALSE);
			nue_check_mult = 50;
			break;
		}

		/* RF4_DANMAKU */
		///弾幕攻撃　レベル分ダメージ
		case 96+1:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かを射った。", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif
			else msg_format(_("%^sが弾幕を放った。", "%^s fires danmaku."), m_name);
			dam = rlev;

			bolt(m_idx, GF_MISSILE, dam, MS_DANMAKU, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);

			break;
		}

		/* RF4_DISPEL */
		case 96+2:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
			else msg_format("%^sが魔力消去の呪文を念じた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
			else msg_format("%^s invokes a spell to dispel magic.", m_name);
#endif
			dispel_player();
			if (p_ptr->riding) dispel_monster_status(p_ptr->riding);

#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_print("やりやがったな！");
#endif
			learn_spell(MS_DISPEL);
			nue_check_mult = -1;
			break;
		}

		/* RF4_ROCKET */
		case 96+3:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを射った。", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif

#ifdef JP
else msg_format("%^sがロケットを発射した。", m_name);
#else
			else msg_format("%^s fires a rocket.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_ROCKET,
				dam, 2, FALSE, MS_ROCKET, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_SHOOT */
		case 96+4:
		{
			int r_idx = m_ptr->r_idx;
			if (!direct) return (FALSE);
			disturb(1, 1);
			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);

			if(r_idx == MON_NODEPPOU || r_idx == MON_DROLEM)
			{
				msg_format(_("%^sが何かを放った。", "%^s shoots something."), m_name);
			}
			else if(r_idx == MON_SUMIREKO || r_idx == MON_SUMIREKO_2 || r_idx == MON_CHARGE_MAN || r_idx == MON_JURAL || r_idx == MON_NITORI || r_idx == MON_GYOKUTO || r_idx == MON_SEIRAN || r_idx == MON_EAGLE_RABBIT)
			{
				msg_format(_("%^sが銃を撃った。", "%^s fires a gun."), m_name);
			}
			else if(r_idx == MON_HAMMERBROTHER)
			{
				msg_format(_("%^sがハンマーを投げた。", "%^s throws a hammer."), m_name);
			}
			else if(r_idx == MON_YAMAWARO)
			{
				msg_format(_("%^sが手裏剣を投げた。", "%^s throws a shuriken."), m_name);
			}
			else if(r_idx == MON_KISUME)
			{
				msg_format(_("%^sが骨を投げた。", "%^s throws a bone."), m_name);
			}
			else if(r_idx == MON_SAKUYA)
			{
				msg_format(_("%^sが大量のナイフを投擲した。", "%^s throws a lot of knives."), m_name);
				dam *= 2;
			}
			else if(r_idx == MON_MURASA)
			{
				msg_format(_("%^sがあなたに向かって錨を投げつけた！", "%^s throws an anchor at you!"), m_name);
				dam *= 2;
			}
			else if(r_idx == MON_HALFLING_S || r_idx == MON_CULVERIN || r_idx == MON_EIKA)
			{
				msg_format(_("%^sが石つぶてを放った。", "%^s fires a pebble."), m_name);
			}
			else if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA)
			{
				msg_format(_("%^sがあなたに向かって大岩を投げつけた！", "%^s throws a boulder at you!"), m_name);
				dam = dam * 5 / 2;
			}
			else if(r_idx == MON_NINJA_SLAYER)
			{
				msg_format(_("%^sはツヨイ・スリケンを放った！", "%^s shoots a strong shuriken!"), m_name);
				dam = dam * 2;
			}

			else msg_format(_("%^sが矢を放った。", "%^s shoots an arrow."), m_name);

			bolt(m_idx, GF_ARROW, dam, MS_SHOOT, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF4_BR_HOLY */
		case 96+5:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが聖なるブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes holy energy.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_HOLY_FIRE, dam,0, TRUE, MS_BR_HOLY, learnable);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 500;
			break;
		}

		/* RF4_BR_HELL */
		case 96+6:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが地獄の業火のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes hellfire.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_HELL_FIRE, dam,0, TRUE, MS_BR_HELL, learnable);
			//update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF4_アクアブレス */
		case 96+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sがアクアブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes water.", m_name);
#endif

			dam = ((m_ptr->hp / 8) > 350 ? 350 : (m_ptr->hp / 8));
			breath(y, x, m_idx, GF_WATER, dam,0, TRUE, MS_BR_AQUA, learnable);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF4_BR_ACID */
		case 96+8:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが酸のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes acid.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ACID, dam, 0, TRUE, MS_BR_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF4_BR_ELEC */
		case 96+9:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが稲妻のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes lightning.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ELEC, dam,0, TRUE, MS_BR_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF4_BR_FIRE */
		case 96+10:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが火炎のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes fire.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_FIRE, dam,0, TRUE, MS_BR_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF4_BR_COLD */
		case 96+11:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが冷気のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes frost.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_COLD, dam,0, TRUE, MS_BR_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF4_BR_POIS */
		case 96+12:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sがガスのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes gas.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_POIS, dam, 0, TRUE, MS_BR_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}


		/* RF4_BR_NETH */
		case 96+13:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが地獄のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_NETHER, dam,0, TRUE, MS_BR_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF4_BR_LITE */
		case 96+14:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが閃光のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes light.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y_br_lite, x_br_lite, m_idx, GF_LITE, dam,0, TRUE, MS_BR_LITE, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 400;
			break;
		}

		/* RF4_BR_DARK */
		case 96+15:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが暗黒のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes darkness.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DARK, dam,0, TRUE, MS_BR_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF4_WAVEMOTION */
		case 96+16:
		{
			disturb(1, 1);
			if (blind)
				msg_format(_("%^sが何か強力な攻撃を行った。", "%^s performs some powerful attack."), m_name);
			else if(m_ptr->r_idx == MON_TOYOHIME)
				msg_format(_("%^sは扇のようなものを取り出しあなたに向けて大きく振った！",
                            "%^s takes out a fan and sweeps it at you!"), m_name);
			else
			msg_format(_("%^sが波動砲を放った！", "%^s fires a wave-motion cannon!"), m_name);
			dam = ((m_ptr->hp / 6) > 555 ? 555 : (m_ptr->hp / 6));
			//breath(y, x, m_idx, GF_DISINTEGRATE, dam, 5, FALSE, MS_WAVEMOTION, learnable);
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, MS_WAVEMOTION, learnable,2);
			//update_smart_learn(m_idx, DRS_CONF);

			/*:::Hack - 波動砲発射後は＠が行動するまで魔法を使わない*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;

			//v1.1.58 この攻撃で初めて視界が通ったかもしれないのでBGMを再チェック
			check_music_mon_r_idx(m_idx);

			break;
		}

		/* RF4_BR_SOUN */
		case 96+17:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_JAIAN)
				msg_format(_("「ボォエ～～～～～～」", "'Booooeeeeee'"));
			else if(m_ptr->r_idx == MON_KUTAKA)
				msg_format(_("「コ　ケ　コ　ッ　コ　ー　！」", "'Cock-a-doodle-doo!'"));

#ifdef JP
else if (blind) msg_format("%^sがあなたに向けて衝撃波を放った！", m_name);
#else
			else if (blind) msg_format("%^s launches a shockwave at you!", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたに向けて衝撃波を放った！", m_name);
#else
			else msg_format("%^s launches a shockwave at you!", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SOUND, dam,0, TRUE, MS_BR_SOUND, learnable);
			update_smart_learn(m_idx, DRS_SOUND);
			break;
		}

		/* RF4_BR_CHAO */
		case 96+18:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sがカオスのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes chaos.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_CHAOS, dam,0, TRUE, MS_BR_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			nue_check_mult = 10;
			break;
		}

		/* RF4_BR_DISE */
		case 96+19:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが劣化のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes disenchantment.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISENCHANT, dam,0, TRUE, MS_BR_DISEN, learnable);
			update_smart_learn(m_idx, DRS_DISEN);
			nue_check_mult = 200;
			break;
		}

		/* RF4_BR_NEXU */
		case 96+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが因果混乱のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes nexus.", m_name);
#endif

			dam = ((m_ptr->hp / 7) > 600 ? 600 : (m_ptr->hp / 7));
			breath(y, x, m_idx, GF_NEXUS, dam,0, TRUE, MS_BR_NEXUS, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_TIME */
		case 96+21:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが時間逆転のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes time.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_TIME, dam,0, TRUE, MS_BR_TIME, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_INER */
		case 96+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが遅鈍のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes inertia.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_INACT, dam,0, TRUE, MS_BR_INERTIA, learnable);
			break;
		}

		/* RF4_BR_GRAV */
		case 96+23:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが重力のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes gravity.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_GRAVITY, dam,0, TRUE, MS_BR_GRAVITY, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_SHAR */
		case 96+24:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
				msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
else if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが破片のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes shards.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SHARDS, dam,0, TRUE, MS_BR_SHARDS, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_BR_PLAS */
		case 96+25:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sがプラズマのブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes plasma.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 1200 ? 1200 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_PLASMA, dam,0, TRUE, MS_BR_PLASMA, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_ELEC);

			break;
		}

		/* RF4_BA_FORCE */
		case 96+26:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを放った。", m_name);
#else
			if (blind) msg_format("%^s fires something.", m_name);
#endif

#ifdef JP
else msg_format("%^sが気弾を放った。", m_name);
#else
			else msg_format("%^s fires a Ki sphere.", m_name);
#endif
			if(r_ptr->flags2 & RF2_POWERFUL)
				dam = ((m_ptr->hp / 4) > 350 ? 350 : (m_ptr->hp / 4));
			else
				dam = ((m_ptr->hp / 6) > 250 ? 250 : (m_ptr->hp / 6));

			breath(y, x, m_idx, GF_FORCE, dam, 2, FALSE, MS_BA_FORCE, learnable);
			break;
		}

		/* RF4_BR_MANA */
		case 96+27:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔力のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes mana.", m_name);
#endif
			dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_MANA, dam,0, TRUE, MS_BR_MANA, learnable);
			break;
		}

		/* RF4_SPECIAL2 */
		case 96+28:
		{
			disturb(1, 1);

			//フラン特殊攻撃　必中破滅の手+ランダム切り傷+経験値減少+全能力低下　隣接してなければHP700でまあ即死はしないはず
			if(m_ptr->r_idx == MON_FLAN)
			{
				if (!direct) return (FALSE);
				if (!blind)
				{
					msg_format(_("%^sはあなたを見てニヤリと笑うと拳を握りしめた。",
                                "%^s grins at you and closes her fist."), m_name);
					msg_print(NULL);
				}
				dam = (((s32b) ((40 + randint1(20)) * (p_ptr->chp))) / 100);
				breath(y, x, m_idx, GF_KYUTTOSHITEDOKA_N, dam, 0, FALSE, -1, FALSE);


			}
			//v1.1.49 隠岐奈特殊行動　ランダムユニーク召喚
			else if (m_ptr->r_idx == MON_OKINA)
			{
				if (summon_named_creature(0, m_ptr->fy, m_ptr->fx, MON_RANDOM_UNIQUE_2, PM_ALLOW_SP_UNIQUE))
					msg_format(_("%^sは新たなモンスターを創り出した！",
                                "%^s creates a new monster!"), m_name);

			}

			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
			{
				/*:::正邪隣接時　小槌攻撃*/
				if(distance(y, x, m_ptr->fy, m_ptr->fx)<2)
				{
					if (!blind)	msg_format(_("金色の小槌でぶん殴られた！", "You are hit by a golden mallet!"));
					else msg_format(_("何かが頭上に振り下ろされた！", "Something powerfully strikes your head!"));
					dam = 100 + randint0(100);
					breath(y, x, m_idx, GF_MISSILE, dam, 0, FALSE, -1, FALSE);
					earthquake_aux(y, x, 4, m_idx);
				}
				/*:::正邪非隣接時　位置交換*/
				else
				{
					if( p_ptr->anti_tele || p_ptr->resist_time ) msg_format(_("%^sの舌打ちが聞こえた。", "%s clicks her tongue."),m_name);
					else if(cave[y][x].info & CAVE_ICKY || cave[m_ptr->fy][m_ptr->fx].info & CAVE_ICKY)  msg_format(_("%^sは何かをしようとしたが失敗したようだ。",
                                                                                                                    "%^s tries to do something, but fails."),m_name);
					else
					{
						msg_format(_("%^sはあなたと場所を交換した！", "%^s swaps places with you!"),m_name);
						(void)move_player_effect(m_ptr->fy, m_ptr->fx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
					}
				}
			}

			//アザトート特殊攻撃　近寄ると分解巨大レーザーか分解ボールを連発する
			else if(m_ptr->r_idx == MON_AZATHOTH)
			{
				int atk_num;
				if (!blind)
				{
					msg_format(_("%^sの巨大な偽足がダンジョンを縦横に貫いた！",
                                "%^s pierces the dungeon with a giant pseudopod!"), m_name);
					msg_print(NULL);
				}
				dam = 100 + randint1(100);
				for(atk_num = 6;atk_num>0;atk_num--)
				{
					int tx, ty;
					if(one_in_(6)) 	masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,(2));

					else
					{
						while(TRUE)
						{
							tx = rand_spread(x,7);
							ty = rand_spread(y,7);
							if (!in_bounds(ty, tx)) continue;
							if (cave_have_flag_grid(&cave[ty][tx], FF_PERMANENT)) continue;
							break;
						}
						(void)project(m_idx, 1 + randint1(4), ty, tx, dam, GF_DISINTEGRATE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
					}
				}

			}
			//ヘカーティア特殊行動
			else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
			{
				int dummy_x = m_ptr->fx;
				int dummy_y = m_ptr->fy;
				if(hecatia_change_idx < MON_HECATIA1 || hecatia_change_idx > MON_HECATIA3)
				{
					msg_format(_("ERROR:hecatia_change_idxがおかしい(%d)",
                                "ERROR: Weird hecatia_change_idx (%d)"),hecatia_change_idx);
					return FALSE;
				}
				if (p_ptr->inside_arena || p_ptr->inside_battle || !summon_possible(m_ptr->fy, m_ptr->fx)) return FALSE;
				hecatia_hp[m_ptr->r_idx - MON_HECATIA1] = m_ptr->hp;
				delete_monster_idx(cave[m_ptr->fy][m_ptr->fx].m_idx);

				//この中でHPを再設定する
				if(summon_named_creature(0, dummy_y, dummy_x, hecatia_change_idx, 0L))
					msg_format(_("%^sは体を交代した！", "%^s switches bodies!"), m_name);

			}
			else if(m_ptr->r_idx == MON_SAIBAIMAN)
			{
				if (!blind)
				{
					msg_format(_("%^sは自爆した！", "%^s self-detonates!"), m_name);
				}
				project(m_idx,2,m_ptr->fy,m_ptr->fx,damroll(10,80),GF_SHARDS,(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				suisidebomb = TRUE;
			}
			else if(m_ptr->r_idx == MON_KOSUZU)
			{
					msg_format(_("%^sは周囲の付喪神を吸収した！", "%^s absorbs nearby tsukumogami!"), m_name);
					absorb_tsukumo(m_idx);
			}
			else if (m_ptr->r_idx == MON_YUMA)
			{
				int oil_field_num;
				int tmp_hp;
				msg_format(_("%^sは周囲の石油を吸い込んで吸収した！",
                            "%^s sucks up and absorbs the oil around her!"), m_name);
				oil_field_num = yuma_vacuum_oil(m_ptr->fy, m_ptr->fx, FALSE, 0);

				//ないと思うがオーバーフロー防止のために一時変数を経由
				tmp_hp = m_ptr->hp + oil_field_num * 100;
				if (tmp_hp > m_ptr->maxhp) tmp_hp = m_ptr->maxhp;
				m_ptr->hp = tmp_hp;
				if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			}

			else
			{
				msg_format(_("ERROR: %^sの特別な行動2は定義されていない。",
                            "ERROR: Undefined special action 2 for %^s."), m_name);
			}

			break;
		}

		/* RF4_BR_NUKE */
		case 96+29:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが核熱のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes nuclear energy.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 1000 ? 1000 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_NUKE, dam,0, TRUE, MS_BR_NUKE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 150;
			break;
		}

		/* RF4_BA_CHAO */
		case 96+30:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが恐ろしげにつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles frighteningly.", m_name);
#endif

#ifdef JP
else msg_format("%^sが純ログルスを放った。", m_name);/*nuke me*/
#else
			else msg_format("%^s invokes a raw Logrus.", m_name);
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 3) : (rlev * 2))+ damroll(10, 10);
			breath(y, x, m_idx, GF_CHAOS, dam, 4, FALSE, MS_BALL_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			nue_check_mult = 10;
			break;
		}

		/* RF4_BR_DISI */
		case 96+31:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^sが分解のブレスを吐いた。", m_name);
#else
			else msg_format("%^s breathes disintegration.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISINTEGRATE, dam,0, TRUE, MS_BR_DISI, learnable);

			//v1.1.58 この攻撃で初めて視界が通ったかもしれないのでBGMを再チェック
			check_music_mon_r_idx(m_idx);

			break;
		}



		/* RF5_BA_ACID */
		case 128+0:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
			else msg_format("%^sがアシッド・ボールの呪文を唱えた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts an acid ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3) + 15);
			}
			breath(y, x, m_idx, GF_ACID, dam, rad, FALSE, MS_BALL_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF5_BA_ELEC */
		case 128+1:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
			else msg_format("%^sがサンダー・ボールの呪文を唱えた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts a lightning ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3 / 2) + 8);
			}
			breath(y, x, m_idx, GF_ELEC, dam, rad, FALSE, MS_BALL_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF5_BA_FIRE */
		case 128+2:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);

			if (m_ptr->r_idx == MON_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%sが何かを投げた。", m_name);
				else
					msg_format("%sは手榴弾を投げた。", m_name);
#else
				if (blind)
					msg_format("%^s throws something.", m_name);
				else
					msg_format("%^s throws a hand grenade.", m_name);
#endif
			}
			else if(m_ptr->r_idx == MON_ALICE)
			{
				if (blind)
					msg_format(_("%sが何かを投げた。", "%^s throws something."), m_name);
				else
					msg_format(_("%sは爆薬の詰まった人形を投げた。", "%^s throws a doll packed with gunpowder."), m_name);
			}

			else
			{
#ifdef JP
				if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
				else msg_format("%^sがファイア・ボールの呪文を唱えた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
				else msg_format("%^s casts a fire ball.", m_name);
#endif
			}

			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 7 / 2) + 10);
			}
			breath(y, x, m_idx, GF_FIRE, dam, rad, FALSE, MS_BALL_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF5_BA_COLD */
		case 128+3:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
			else msg_format("%^sがアイス・ボールの呪文を唱えた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts a frost ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3 / 2) + 10);
			}
			breath(y, x, m_idx, GF_COLD, dam, rad, FALSE, MS_BALL_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF5_BA_POIS */
		case 128+4:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが悪臭雲の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a stinking cloud.", m_name);
#endif

			dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF5_BA_NETH */
		case 128+5:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが地獄球の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			breath(y, x, m_idx, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF5_BA_WATE */
		case 128+6:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが流れるような身振りをした。", m_name);
#else
			else msg_format("%^s gestures fluidly.", m_name);
#endif

#ifdef JP
msg_print("あなたは渦巻きに飲み込まれた。");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
			breath(y, x, m_idx, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF5_BA_MANA */
		case 128+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔力の嵐の呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a mana storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);
			break;
		}

		/* RF5_BA_DARK */
		case 128+8:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^sが暗黒の嵐の呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a darkness storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_DRAIN_MANA */
		case 128+9:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

			dam = (randint1(rlev) / 2) + 1;
			breath(y, x, m_idx, GF_DRAIN_MANA, dam, 0, FALSE, MS_DRAIN_MANA, learnable);
			update_smart_learn(m_idx, DRS_MANA);
			break;
		}

		/* RF5_MIND_BLAST */
		case 128+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (!seen)
			{
#ifdef JP
msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^sがあなたの瞳をじっとにらんでいる。", m_name);
#else
				msg_format("%^s gazes deep into your eyes.", m_name);
#endif

			}

			dam = damroll(7, 7);
			breath(y, x, m_idx, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);
			nue_check_mult = 200;
			break;
		}

		/* RF5_BRAIN_SMASH */
		case 128+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (!seen)
			{
#ifdef JP
msg_print("何かがあなたの精神に念を放っているようだ。");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^sがあなたの瞳をじっと見ている。", m_name);
#else
				msg_format("%^s looks deep into your eyes.", m_name);
#endif

			}

			dam = damroll(12, 12);
			breath(y, x, m_idx, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);
			nue_check_mult = 200;
			break;
		}

		/* RF5_CAUSE_1 */
		case 128+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたを指さして呪った。", m_name);
#else
			else msg_format("%^s points at you and curses.", m_name);
#endif

			dam = damroll(3, 8);
			breath(y, x, m_idx, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);


			break;
		}

		/* RF5_CAUSE_2 */
		case 128+13:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたを指さして恐ろしげに呪った。", m_name);
#else
			else msg_format("%^s points at you and curses horribly.", m_name);
#endif

			dam = damroll(8, 8);
			breath(y, x, m_idx, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_CAUSE_3 */
		case 128+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを大声で叫んだ。", m_name);
#else
			if (blind) msg_format("%^s mumbles loudly.", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたを指さして恐ろしげに呪文を唱えた！", m_name);
#else
			else msg_format("%^s points at you, incanting terribly!", m_name);
#endif

			dam = damroll(10, 15);
			breath(y, x, m_idx, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_CAUSE_4 */
		case 128+15:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			///msg131221
			if (blind) msg_format("%^sが何か禍々しい言葉を叫んだ。", m_name);
#else
			if (blind) msg_format("%^s screams some terrible worlds.", m_name);
#endif

#ifdef JP
			///msg131221
			else msg_format("%^sがあなたに向けて死の言霊を放った！", m_name);
#else
			else msg_format("%^s points at you and invokes the Word of Death!", m_name);
#endif

			dam = damroll(15, 15);
			breath(y, x, m_idx, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_BO_ACID */
		case 128+16:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがアシッド・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a acid bolt.", m_name);
#endif

			dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ACID, dam, MS_BOLT_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_ELEC */
		case 128+17:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがサンダー・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a lightning bolt.", m_name);
#endif

			dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ELEC, dam, MS_BOLT_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_FIRE */
		case 128+18:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがファイア・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a fire bolt.", m_name);
#endif

			dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_FIRE, dam, MS_BOLT_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_COLD */
		case 128+19:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがアイス・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a frost bolt.", m_name);
#endif

			dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_COLD, dam, MS_BOLT_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BA_LITE */
		case 128+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
			///msg131221
else msg_format("%^sが閃光の嵐の呪文を念じた。", m_name);
#else
			else msg_format("%^s invokes a light storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 500;
			break;
		}

		/* RF5_BO_NETH */
		case 128+21:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが地獄の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a nether bolt.", m_name);
#endif

			dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
			bolt(m_idx, GF_NETHER, dam, MS_BOLT_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_WATE */
		case 128+22:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがウォーター・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a water bolt.", m_name);
#endif

			dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_WATER, dam, MS_BOLT_WATER, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF5_BO_MANA */
		case 128+23:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔力の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a mana bolt.", m_name);
#endif

			dam = randint1(rlev * 7 / 2) + 50;
			bolt(m_idx, GF_MANA, dam, MS_BOLT_MANA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_PLAS */
		case 128+24:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがプラズマ・ボルトの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a plasma bolt.", m_name);
#endif

			dam = 100 + damroll(1, 200) + (rlev * 3 * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			bolt(m_idx, GF_PLASMA, dam, MS_BOLT_PLASMA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF5_BO_ICEE */
		case 128+25:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが極寒の矢の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts an ice bolt.", m_name);
#endif

			dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_ICE, dam, MS_BOLT_ICE, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_MISSILE */
		case 128+26:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがマジック・ミサイルの呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a magic missile.", m_name);
#endif

			dam = damroll(2, 6) + (rlev / 3);
			bolt(m_idx, GF_MISSILE, dam, MS_MAGIC_MISSILE, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_SCARE */
		case 128+27:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやくと、恐ろしげな音が聞こえた。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear scary noises.", m_name);
#endif

#ifdef JP
else msg_format("%^sが恐ろしげな幻覚を作り出した。", m_name);
#else
			else msg_format("%^s casts a fearful illusion.", m_name);
#endif


			//v1.1.37 小鈴例外処理
			if(IS_VULN_FEAR)
			{
				int chance = 100 + rlev;
				if (p_ptr->resist_fear) chance /= 2;

				if(randint0(chance) > p_ptr->skill_sav)
				{
					(void)set_afraid(p_ptr->afraid + randint0(6) + 6);

					if(!p_ptr->resist_fear && randint0(rlev) > p_ptr->lev)
					{
						msg_print(_("あなたは恐怖のあまりに気絶してしまった！", "You almost faint from terror!"));
						(void)set_paralyzed(p_ptr->paralyzed + randint0(3) + 3);
					}

				}
				else
				{
					msg_print(_("しかし恐怖に侵されなかった。", "You refuse to be frightened."));
				}

			}
			else if (p_ptr->resist_fear)
			{
#ifdef JP
msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし恐怖に侵されなかった。");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_afraid(p_ptr->afraid + randint0(4) + 4);
			}
			learn_spell(MS_SCARE);
			update_smart_learn(m_idx, DRS_FEAR);
			nue_check_mult = 50;
			break;
		}

		/* RF5_BLIND */
		case 128+28:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが呪文を唱えてあなたの目をくらました！", m_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", m_name);
#endif

			if (p_ptr->resist_blind)
			{
#ifdef JP
msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_blind(12 + randint0(4));
			}
			learn_spell(MS_BLIND);
			update_smart_learn(m_idx, DRS_BLIND);
			nue_check_mult = 50;
			break;
		}

		/* RF5_CONF */
		case 128+29:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやくと、頭を悩ます音がした。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear puzzling noises.", m_name);
#endif

#ifdef JP
else msg_format("%^sが誘惑的な幻覚を作り出した。", m_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", m_name);
#endif

			if (p_ptr->resist_conf)
			{
#ifdef JP
msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし幻覚にはだまされなかった。");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_confused(p_ptr->confused + randint0(4) + 4);
			}
			learn_spell(MS_CONF);
			update_smart_learn(m_idx, DRS_CONF);
			nue_check_mult = 50;
			break;
		}

		/* RF5_SLOW */
		case 128+30:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^sがあなたの筋力を吸い取ろうとした！", m_name);
#else
			msg_format("%^s drains power from your muscles!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
			}
			learn_spell(MS_SLOW);
			update_smart_learn(m_idx, DRS_FREE);
			nue_check_mult = 50;
			break;
		}

		/* RF5_HOLD */
		case 128+31:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたの目をじっと見つめた！", m_name);
#else
			else msg_format("%^s stares deep into your eyes!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
			}
			learn_spell(MS_SLEEP);
			update_smart_learn(m_idx, DRS_FREE);
			break;
		}

		/* RF6_HASTE */
		case 160+0:
		{
			disturb(1, 1);
			if (blind)
			{
#ifdef JP
msg_format("%^sが何かをつぶやいた。", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^sが自分の体に念を送った。", m_name);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}

			/* Allow quick speed increases to base+10 */
			if (set_monster_fast(m_idx, MON_FAST(m_ptr) + 100))
			{
#ifdef JP
				msg_format("%^sの動きが速くなった。", m_name);
#else
				msg_format("%^s starts moving faster.", m_name);
#endif
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_HAND_DOOM */
		case 160+1:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^sが<破滅の手>を放った！", m_name);
#else
			msg_format("%^s invokes the Hand of Doom!", m_name);
#endif
			dam = (((s32b) ((40 + randint1(20)) * (p_ptr->chp))) / 100);
			breath(y, x, m_idx, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);
			break;
		}

		/* RF6_HEAL */
		case 160+2:
		{
			disturb(1, 1);

			/* Message */
			if (blind)
			{
#ifdef JP
msg_format("%^sが何かをつぶやいた。", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^sが自分の傷に集中した。", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}

			/* Heal some */
			m_ptr->hp += (rlev * 6);

			/* Fully healed */
			if (m_ptr->hp >= m_ptr->maxhp)
			{
				/* Fully healed */
				m_ptr->hp = m_ptr->maxhp;

				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^sは完全に治った！", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^sは完全に治ったようだ！", m_name);
#else
					msg_format("%^s sounds completely healed!", m_name);
#endif

				}
			}

			/* Partially healed */
			else
			{
				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^sは体力を回復したようだ。", m_name);
#else
					msg_format("%^s looks healthier.", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^sは体力を回復したようだ。", m_name);
#else
					msg_format("%^s sounds healthier.", m_name);
#endif

				}
			}

			/* Redraw (later) if needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);
			nue_check_mult = 10;

			/* Cancel fear */
			if (MON_MONFEAR(m_ptr))
			{
				/* Cancel fear */
				(void)set_monster_monfear(m_idx, 0);

				/* Message */
#ifdef JP
				msg_format("%^sは勇気を取り戻した。", m_name);
#else
				msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
			}
			break;
		}

		/* RF6_INVULNER */
		case 160+3:
		{
			disturb(1, 1);

			/* Message */
			if (!seen)
			{
#ifdef JP
msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
				msg_format("%^s mumbles powerfully.", m_name);
#endif

			}
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^sの体が実体を失い暗く透き通った・・", m_name);

			else
			{
#ifdef JP
msg_format("%sは無傷の球の呪文を唱えた。", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif

			}

			if (!MON_INVULNER(m_ptr)) (void)set_monster_invulner(m_idx, randint1(4) + 4, FALSE);
			nue_check_mult = 20;
			break;
		}

		/* RF6_BLINK */
		case 160+4:
		{
			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappears from the floor."), m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			disturb(1, 1);

			if (teleport_barrier(m_idx))
			{
#ifdef JP
				msg_format("%^sのテレポートは妨害された。", m_name);
			//	msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("%^s is prevented from teleporting.", m_name);
#endif
			}
			else
			{
#ifdef JP
			if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //こいしのショートテレポートはメッセージが出ない
			else 	msg_format("%^sが瞬時に消えた。", m_name);
#else
            if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //こいしのショートテレポートはメッセージが出ない
			else 	msg_format("%^s blinks away.", m_name);
#endif
				teleport_away(m_idx, 10, 0L);
				p_ptr->update |= (PU_MONSTERS);
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_TPORT */
		case 160+5:
		{

			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappears from the floor."), m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			disturb(1, 1);


			if (teleport_barrier(m_idx))
			{
#ifdef JP
				msg_format("%^sのテレポートは妨害された。", m_name);
			//	msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("%^s is prevented from teleporting.", m_name);
#endif
			}
			else
			{
#ifdef JP
			if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //こいしのテレポートはメッセージが出ない
			else	msg_format("%^sがテレポートした。", m_name);
#else
            if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //こいしのテレポートはメッセージが出ない
			else	msg_format("%^s teleports away.", m_name);
#endif
				teleport_away_followable(m_idx);
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_WORLD */
		case 160+6:
		{
			///mod140107 時間停止に咲夜を追加
			int who = 0;
			disturb(1, 1);
			if(m_ptr->r_idx == MON_DIO) who = 1;
			else if(m_ptr->r_idx == MON_SAKUYA) who = 2;
			else if(m_ptr->r_idx == MON_WONG) who = 3;
			dam = who;
			if (!process_the_world(randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

		/* RF6_SPECIAL */
		///mon 特別行動の実行処理
		case 160+7:
		{
			int k;

			disturb(1, 1);
			switch (m_ptr->r_idx)
			{
			case MON_OHMU:
				/* Moved to process_monster(), like multiplication */
				return FALSE;
			case MON_NINJA_SLAYER:
				if (blind) msg_format(_("%^sは何かを大量に放った！", "%^s fires a lot of projectiles!"), m_name);
				else msg_format(_("ナムサン！%sが放ったのは非人道兵器マキビシだ！",
                                    "%^s fires makibishi!"), m_name);

				breath(y, x, m_idx, GF_INACT, 100, 3, FALSE, 0, FALSE);
				break;

			case MON_CHIMATA:
				if (blind) msg_format(_("%^sから異様なエネルギーが放射された！",
                                        "%^s releases extraordinary energy!"), m_name);
				else msg_format(_("%^sは虹色の嵐を巻き起こした！",
                                    "%^s conjures a rainbow-colored storm!"), m_name);
				dam = (rlev * 4) + 50 + damroll(10, 10);
				breath(y, x, m_idx, GF_RAINBOW, dam, 5, FALSE, 0, FALSE);

				break;

			case MON_REIMU:
				if (blind) msg_format(_("%^sは何かを大量に放った！", "%^s fires a lot of projectiles!"), m_name);
				else msg_format(_("%^sは夢想封印を放った！", "%^s uses Fantasy Seal!"), m_name);
				cast_musou_hu_in(m_idx);
				break;
			case MON_BANORLUPART:
				{
					int dummy_hp = (m_ptr->hp + 1) / 2;
					int dummy_maxhp = m_ptr->maxhp/2;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (p_ptr->inside_arena || p_ptr->inside_battle || !summon_possible(m_ptr->fy, m_ptr->fx)) return FALSE;
					delete_monster_idx(cave[m_ptr->fy][m_ptr->fx].m_idx);
					summon_named_creature(0, dummy_y, dummy_x, MON_BANOR, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;
					summon_named_creature(0, dummy_y, dummy_x, MON_LUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("『バーノール・ルパート』が分裂した！");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}

			case MON_BANOR:
			case MON_LUPART:
				{
					int dummy_hp = 0;
					int dummy_maxhp = 0;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (!r_info[MON_BANOR].cur_num || !r_info[MON_LUPART].cur_num) return (FALSE);
					for (k = 1; k < m_max; k++)
					{
						if (m_list[k].r_idx == MON_BANOR || m_list[k].r_idx == MON_LUPART)
						{
							dummy_hp += m_list[k].hp;
							dummy_maxhp += m_list[k].maxhp;
							if (m_list[k].r_idx != m_ptr->r_idx)
							{
								dummy_y = m_list[k].fy;
								dummy_x = m_list[k].fx;
							}
							delete_monster_idx(k);
						}
					}
					summon_named_creature(0, dummy_y, dummy_x, MON_BANORLUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("『バーノール』と『ルパート』が合体した！");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			case MON_ROLENTO:
#ifdef JP
				if (blind) msg_format("%^sが何か大量に投げた。", m_name);
				else msg_format("%^sは手榴弾をばらまいた。", m_name);
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif

				{
					int num = 1 + randint1(3);

					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, mode);
					}
				}
#ifdef JP
				if (blind && count) msg_print("多くのものが間近にばらまかれる音がする。");
#else
				if (blind && count) msg_print("You hear many things are scattered nearby.");
#endif
				break;

			case MON_MICHAEL:
#ifdef JP
				msg_format("%^s「燃え尽きろ！スピキュール！」", m_name);
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1600);
#ifdef JP
				if(one_in_(3))msg_format("%^s「この俺様は強ぇだろうが！」", m_name);
				else if(one_in_(2))msg_format("%^s「うおおおおおお！熱っちいいいいいぃ！」", m_name);
				else msg_format("%^s「骨まであっためてやるよ！」", m_name);
#endif // JP

				break;
			case MON_MOKOU:
				msg_format(_("%^sはフジヤマヴォルケイノを放った！",
                            "%^s uses Fujiyama Volcano!"), m_name);
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1000);
				break;

			case MON_SHION_2:
				msg_format(_("%^sは不運と不幸をまき散らした！",
                            "%^s spreads misery and misfortune!"), m_name);
				project_hack3(m_idx, m_ptr->fy, m_ptr->fx, GF_DISENCHANT, 0, 0, 400);

				if (p_ptr->au > 0 && direct && !weird_luck() && !p_ptr->inside_arena)
				{
					p_ptr->au /= 2;
					msg_print(_("財布がすごく軽くなった気がする！",
                                "You feel your purse getting a lot lighter!"));
					p_ptr->redraw |= PR_GOLD;
				}


				break;

			case MON_FLAN:
				{
					for (k = 0; k < 3; k++)
						count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_FLAN_4, mode);
				}
				break;
				//正邪特殊行動
			case MON_SEIJA:
				{
					int decoy_m_idx = 0;
					int j;
					for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_SEIJA_D)
					{
						decoy_m_idx = j;
						break;
					}
					/*:::デコイ人形がすでにいる場合メッセージ無しでデコイとの位置交換*/
					if(decoy_m_idx)
					{
						monster_type *dm_ptr = &m_list[decoy_m_idx];
						int x1=dm_ptr->fx,y1=dm_ptr->fy  ,x2 = m_ptr->fx,y2=m_ptr->fy;

						if( cave[y1][x1].info & CAVE_ICKY || cave[y2][x2].info & CAVE_ICKY || teleport_barrier(m_idx) || teleport_barrier(decoy_m_idx))
							 msg_format(_("%^sは何かをしようとしたが失敗したようだ。",
                                        "%^s tries to do something, but fails."),m_name);
						else
						{
							cave[y2][x2].m_idx = decoy_m_idx;
							cave[y1][x1].m_idx = m_idx;
							dm_ptr->fy = y2;
							dm_ptr->fx = x2;
							m_ptr->fy = y1;
							m_ptr->fx = x1;
							update_mon(m_idx,TRUE);
							update_mon(decoy_m_idx,TRUE);
							lite_spot(y1,x1);
							lite_spot(y2,x2);
							verify_panel();
						}

					}
					/*:::デコイ人形がいない場合メッセージ無しでテレポートし、元の場所にデコイを召喚*/
					else
					{

						int old_y = m_ptr->fy;
						int old_x = m_ptr->fx;
						teleport_away(m_idx, 30, 0L);
						count += summon_named_creature(m_idx, old_y, old_x, MON_SEIJA_D, mode);
					}
				}
				break;
			case MON_YUKARI:
				{
					if (!direct) return (FALSE);
					msg_format(_("%^sが空中に穴を開けると、その中から電車が突進してきた！",
                                "%^s opens up a gap in empty space, and a train comes out of it at full speed!"), m_name);
					beam(m_idx, GF_TRAIN, 100+randint1(50), 0, FALSE);
				}
				break;
			case MON_SUMIREKO:
				{
					if (!direct) return (FALSE);
					msg_format(_("突如、どこからかあなたの方に鉄塔が倒れてきた！",
                                "Suddenly, a steel tower falls on you out of nowhere!"));
					(void)project(m_idx, 0, py, px, 100+randint1(50), GF_TRAIN, (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				}
				break;
				//v1.1.32
			case MON_SATONO:
			case MON_MAI:
				{
					char tmp_m_name[80];
					monster_desc(tmp_m_name,&m_list[rankup_monster_idx],0);
					msg_format(_("%^sは%sの背後で踊った！", "%^s dances behind the back of %s!"),m_name,tmp_m_name);
					rankup_monster(rankup_monster_idx,1);
				}
				break;
			case MON_OKINA:
				{
					msg_format(_("%^sはあなたの背後の扉を開いた！", "%^s opens a door on your back!"),m_name);
					if(p_ptr->resist_time || p_ptr->anti_tele)
					{
						msg_print(_("しかしあなたは抵抗した！", "You resist the effects!"));
					}
					else
					{
						//1のときprocess_world_aux_movement()に行けば帰還発動する
						recall_player(randint1(2));
					}
				}
				break;

				//v1.1.53　森林生成
			case MON_TSUCHINOKO:
				msg_format(_("突然蔦が生い茂って%^sを覆った。",
                            "Suddenly, vegetation completely overgrows %^s."), m_name);
				project(m_idx, 1, m_ptr->fy, m_ptr->fx, 1, GF_MAKE_TREE, (PROJECT_GRID), 0);

				break;

			case MON_MIYOI: //v1.1.78

				msg_format(_("%^sはあなたにお酒を勧めてきた。",
                            "%^s recommends sake to you."), m_name);

				if (p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
				{
					msg_print(_("しかしあなたは返事ができない。", "You are unable to respond."));

				}
				else
				{
					set_alcohol(p_ptr->alcohol + calc_alcohol_mod(2000 + randint1(2000), FALSE));
					if (p_ptr->alcohol >= DRANK_2 && !(EXTRA_MODE))
					{

						if (summon_named_creature(m_idx, py, px, MON_SYOUZYOU, mode))
							msg_print(_("何かが地響きを立ててあなたの背後に現れた！",
                                        "Something appears behind you with a loud rumble!"));
					}

				}


				break;

			default:
				if (r_ptr->d_char == 'B')
				{
					disturb(1, 1);
					if (one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^sは突然視界から消えた!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
						teleport_away(m_idx, 10, TELEPORT_NONMAGICAL);
						p_ptr->update |= (PU_MONSTERS);
					}
					else
					{
						int get_damage = 0;
						bool fear; /* dummy */

#ifdef JP
						msg_format("%^sがあなたを掴んで空中から投げ落とした。", m_name);
#else
						msg_format("%^s holds you, and drops from the sky.", m_name);
#endif
						dam = damroll(4, 8);
						teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

						sound(SOUND_FALL);

						if (p_ptr->levitation)
						{
#ifdef JP
							msg_print("あなたは静かに着地した。");
#else
							msg_print("You float gently down to the ground.");
#endif
						}
						else
						{
#ifdef JP
							msg_print("あなたは地面に叩きつけられた。");
#else
							msg_print("You crashed into the ground.");
#endif
							dam += damroll(6, 8);
						}

						/* Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						   -- henkma
						 */
						get_damage = take_hit(DAMAGE_NOESCAPE, dam, m_name, -1);
						if (p_ptr->tim_eyeeye && get_damage > 0 && !p_ptr->is_dead)
						{
#ifdef JP
							msg_format("攻撃が%s自身を傷つけた！", m_name);
#else
							char m_name_self[80];

							/* hisself */
							monster_desc(m_name_self, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

							msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
#endif
							project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);

							if(p_ptr->pclass == CLASS_HINA)
							{
								hina_gain_yaku(-get_damage/5);
								if(!p_ptr->magic_num1[0]) set_tim_eyeeye(0,TRUE);
							}

							else set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
						}

						if (p_ptr->riding) mon_take_hit_mon(p_ptr->riding, dam, &fear, extract_note_dies(real_r_ptr(&m_list[p_ptr->riding])), m_idx);
					}
					break;
				}

				/* Something is wrong */
				else return FALSE;
			}
			break;
		}

		/* RF6_TELE_TO */
		case 160+8:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

			if(p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 29)
			{
				msg_format(_("%^sがあなたを引き戻・・そうとしたがあなたは元の位置に戻った。",
                            "%^s commands you to return, but you return to your original location."), m_name);
				break;
			}
			else if(CHECK_FAIRY_TYPE == 44)
			{
				msg_format(_("%^sがあなたを引き戻・・そうとしたがあなたはすり抜けた。",
                            "%^s commands you to return, but you slip through."), m_name);
				break;
			}
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RFR_RES_TELE)
			{
				msg_format(_("%^sのテレポート・バックはあなたには効かなかった。",
                            "%^s commands you to return, but you are unaffected!"), m_name);
				break;
			}

#ifdef JP
			if(m_ptr->r_idx == MON_NINJA_SLAYER)
				msg_format("%^sがあなたを引き戻した。ドウグ社のフックロープは、マッポーの世に失われつつある職人の技を継承しつづけているのだ。", m_name);
			else
				msg_format("%^sがあなたを引き戻した。", m_name);

#else
			msg_format("%^s commands you to return.", m_name);
#endif

			teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			learn_spell(MS_TELE_TO);
			break;
		}

		/* RF6_TELE_AWAY */
		case 160+9:
		{
			if (!direct) return (FALSE);
			///mod131228 時空耐性によるテレポアウェイ抵抗を実装してみた。
			if(p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 29)
			{
				msg_format(_("%^sにテレポートさせられ・・かけたがあなたは元の位置に戻った。",
                            "%^s teleports you away, but you return to your original location."), m_name);
				break;
			}
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RFR_RES_TELE)
			{
				msg_format(_("%^sのテレポート・アウェイはあなたには効かなかった。",
                            "%^s tries to teleport you away, but you are unaffected!"), m_name);
				break;
			}

			if(p_ptr->resist_time && p_ptr->lev > randint1(rlev))
			{
				msg_format(_("%^sのテレポート・アウェイはあなたには効かなかった。",
                            "%^s tries to teleport you away, but you are unaffected!"), m_name);
				break;
			}

			disturb(1, 1);
#ifdef JP
msg_format("%^sにテレポートさせられた。", m_name);
					///msg131221
					//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					//msg_print("くっそ～");
#else
			msg_format("%^s teleports you away.", m_name);
#endif

			learn_spell(MS_TELE_AWAY);
			teleport_player_away(m_idx, 100);
			update_smart_learn(m_idx, DRS_TIME);
			nue_check_mult = 50;
			break;
		}

		/* RF6_TELE_LEVEL */
		case 160+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何か奇妙な言葉をつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles strangely.", m_name);
#endif

#ifdef JP
else msg_format("%^sがあなたの足を指さした。", m_name);
#else
			else msg_format("%^s gestures at your feet.", m_name);
#endif
///mod131228 他者テレポレベル　因果混乱耐性→時空耐性　だが削除するかも
			//if (p_ptr->resist_nexus)
			if (p_ptr->resist_time)
			{
#ifdef JP
msg_print("しかし効果がなかった！");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				teleport_level(0);
			}
			learn_spell(MS_TELE_LEVEL);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF6_PSY_SPEAR */
		case 160+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か凝縮されたエネルギーを放った。",
                                    "%^s fires some dense energy blast."), m_name);
			else if(m_ptr->r_idx == MON_REMY) msg_format(_("%^sがグングニルを投げつけた。", "%^s throws Gungnir."), m_name);
			else msg_format(_("%^sが光の剣を放った。", "%^s throws a Psycho-Spear."), m_name);

			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 150) : (randint1(rlev * 3 / 2) + 100);
			if(m_ptr->r_idx == MON_REMY) beam(m_idx, GF_GUNGNIR, dam, MS_PSY_SPEAR, learnable);
			else beam(m_idx, GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
			nue_check_mult = 300;
			break;
		}

		/* RF6_DARKNESS */
		case 160+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else if (can_use_lite_area) msg_format("%^sが辺りを明るく照らした。", m_name);
			else msg_format("%^sが暗闇の中で手を振った。", m_name);
#else
			else if (can_use_lite_area) msg_format("%^s cast a spell to light up.", m_name);
			else msg_format("%^s gestures in shadow.", m_name);
#endif

			if (can_use_lite_area)
			{
				(void)lite_area(0, 3);
				nue_check_mult = 500;
			}
			else
			{
				learn_spell(MS_DARKNESS);
				(void)unlite_area(0, 3);
			}

			break;
		}

		/* RF6_TRAPS */
		case 160+13:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。周りの地面に何かが現れた気がする。", m_name);
#else
			if (blind) msg_format("%^s mumbles, and then cackles evilly.", m_name);
#endif

#ifdef JP
else msg_format("%^sが呪文を唱えた。周りの地面に何かが現れた気がする。", m_name);
#else
			else msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif

			learn_spell(MS_MAKE_TRAP);
			nue_check_mult = 30;
			(void)trap_creation(y, x);
			break;
		}

		/* RF6_FORGET */
		case 160+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^sがあなたの記憶を消去しようとしている。", m_name);
#else
			msg_format("%^s tries to blank your mind.", m_name);
#endif

			if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if (lose_all_info())
			{
#ifdef JP
msg_print("記憶が薄れてしまった。");
#else
				msg_print("Your memories fade away.");
#endif

			}
			learn_spell(MS_FORGET);
			nue_check_mult = 50;
			break;
		}

		/* RF6_COSMIC_HORROR */
		///mod コズミック・ホラー追加
		case 160+15:
		{
			disturb(1, 1);
			{
#ifdef JP
if( m_ptr->r_idx == MON_KOISHI) msg_format("%^sがあなたの目を深く覗き込んだ…", m_name);
else	msg_format("%^sの大いなる悪意があなたの精神を絡め取った…", m_name);
#else
            if( m_ptr->r_idx == MON_KOISHI) msg_format("%^s stares deep into your eyes...", m_name);
            else	msg_format("The great will of %^s invades your mind...", m_name);
#endif

			}

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_COSMIC_HORROR, dam, 0, FALSE, MS_COSMIC_HORROR, learnable);
			//update_smart_learn(m_idx, DRS_INSANITY);
			break;
		}

		/* RF6_S_KIN */
		///sys mon 救援召喚？モンスターごとにいろいろ特殊処理
		case 160+16:
		{
			disturb(1, 1);
			//if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
			if (m_ptr->r_idx == MON_SERPENT)
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", m_name);
				else
					msg_format("%^sが混沌の強力な存在を召喚した。", m_name);
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons powerful chaotic beings.", m_name);
#endif
			}
			else if(m_ptr->r_idx == MON_ALICE)
			{
				if (blind)	msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
				else	msg_format(_("%^sが人形達を呼び出した。", "%^s calls her dolls forth."), m_name);
			}

			else if(m_ptr->r_idx == MON_YUKARI)
			{
				if (blind)	msg_format(_("%^sは式の名を呼んだ。", "%^s calls out a shikigami's name."), m_name);
				else	msg_format(_("%^sが式を呼び出した。", "%^s calls for her shikigami."), m_name);
			}
			else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
			{
				if (blind)	msg_format(_("%^sは何者かへ声をかけた。", "%^s calls for someone."), m_name);
				else	msg_format(_("%^sが手下と仲間を呼んだ！", "%^s calls subordinates and allies!"), m_name);
			}

			else if(m_ptr->r_idx == MON_SUIKA)
			{
				if (blind)	msg_format(_("急に辺りが酒臭くなった。", "There is a strong smell of sake in the air..."));
				else	msg_format(_("%^sはあなたに髪の毛を吹き付けた・・", "%^s blows some strands of hair at you..."), m_name);
			}
			else if(m_ptr->r_idx == MON_STORMBRINGER)
			{
				if (blind)	msg_format(_("%^sが何かを叫ぶと、辺りに異様な気配が満ちた。",
                                            "%^s screams something, and you sense eldritch presence."), m_name);
				else	msg_format(_("%^sは黒の剣の分身を呼び出した！", "%^s calls forth black swords!"), m_name);
			}
			else if(m_ptr->r_idx == MON_NAPPA)
			{
				if (blind)	msg_format(_("地面を掘るような音が聞こえた。", "You hear sound of someone digging."));
				else	msg_format(_("%^sは地面に何かを植えた・・", "%^s plants something in the ground..."), m_name);
			}
			else if (m_ptr->r_idx == MON_MAYUMI || m_ptr->r_idx == MON_KEIKI)
			{
				if (blind)	msg_format(_("%^sが何かを叫ぶと、辺りに異様な気配が満ちた。",
                                            "%^s screams something, and you sense something appear nearby."), m_name);
				else	msg_format(_("%^sは配下の埴輪達を呼び出した。", "%^s calls forth her haniwa followers."), m_name);
			}

			else
			{
#ifdef JP
				if (blind)
					msg_format("%^sが何かをつぶやいた。", m_name);
				else
					msg_format("%^sは魔法で%sを召喚した。",
					m_name,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"手下" : "仲間"));
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons %s %s.",
					m_name, m_poss,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"minions" : "kin"));
#endif
			}

			switch (m_ptr->r_idx)
			{
			case MON_MENELDOR:
			case MON_GWAIHIR:
			case MON_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;
			case MON_MAMIZOU:
				{
					int num = randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_TANUKI, 0);
					}
					if(!one_in_(3))	count += summon_named_creature(m_idx, y, x, MON_O_TANUKI, 0);

				}
				break;
			case MON_HECATIA1:
			case MON_HECATIA2:
			case MON_HECATIA3:
				{
					count += summon_named_creature(m_idx, y, x, MON_CLOWNPIECE, 0);
					count += summon_named_creature(m_idx, y, x, MON_JUNKO, 0);
				}
			break;

			case MON_ALICE:
				{
					int num = 1;
					if(dun_level > 39) num++;
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_DOLLS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;
			case MON_ORIN:
			{
				if(one_in_(2))
				{
					num = 5 + randint1(10);
					for (k = 0; k < num; k++)
						count += summon_named_creature(m_idx, y, x, MON_ZOMBIE_FAIRY, 0);
				}
				else
				{
					num = 5 + randint1(10);
					count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					if(one_in_(2))count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					for (k = 0; k < num; k++)
					count += summon_named_creature(m_idx, y, x, MON_ONRYOU, 0);

				}
				break;

			}
			case MON_YUKARI:
				{
					int j, idx=0;
					for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_RAN) idx = j;

					if(idx) teleport_monster_to(idx, y, x, 100, TELEPORT_PASSIVE);
					else count +=  summon_named_creature(m_idx, y, x, MON_RAN, 0);
				}
				break;
			case MON_PATCHOULI:
			case MON_MARISA:
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_LIB_GOLEM, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

				}
				break;
			case MON_SUIKA:
				{
					int k;
					int num = 5 + randint0(6);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_MINI_SUIKA, 0);
					}
				}
				break;
			case MON_EIRIN:
				{
					int k;
					int num = 5 + randint0(6);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_YOUKAI_RABBIT2, 0);
					}
				}
				break;
			case MON_TENSHI:
				{
					int k;
					int num = 3 + randint1(5);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_KANAME, 0);
					}
				}
				break;

			case MON_GREATER_DEMON:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_GREATER_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}

				}
				break;
			case MON_KOSUZU:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_TSUKUMO, 0);
					}

				}
				break;

			case MON_BULLGATES:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_IE, mode);
					}
				}
				break;

			case MON_SERPENT:
			//case MON_ZOMBI_SERPENT:
				{
	/*
					int num = 2 + randint1(3);

					if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("地面から水が吹き出した！");
#else
						msg_print("Water blew off from the ground!");
#endif
						fire_ball_hide(GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
	*/
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
				break;

			case MON_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_LOCKE_CLONE, mode);
					}
				}
				break;

			case MON_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, PM_ALLOW_GROUP);
					}
				}
				break;

			case MON_AZATHOTH:
				{
					int num = randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_INSANITY3, PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
					}
				}
				break;
			case MON_MORGOTH:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_MORGOTH_MINION, PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
					}
				}
				break;
			//v1.1.32
			case MON_NARUMI:
				{
					count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_BULLET_GOLEM, 0);
				}
				break;
			case MON_STORMBRINGER:
				{
					int num = 1 + randint1(4);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_CHAOS_BLADE, mode);
					}
				}
				break;
			case MON_NAZRIN:
			case MON_HAKUSEN:
				{
					int num = 1 + randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_RAT, PM_ALLOW_GROUP);
					}
				}
				break;
			case MON_NAPPA:
				{
					int num = randint1(6);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_SAIBAIMAN, mode);
					}
				}
				break;
			case MON_MAYUMI:
			case MON_KEIKI:
			{
				int num = 4 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HANIWA, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_FORCE_ENEMY));
				}
			}
			break;

			default:
				summon_kin_type = r_ptr->d_char; /* Big hack */

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, PM_ALLOW_GROUP);
				}
				break;
			}
#ifdef JP
			if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF6_S_CYBER */
		case 160+17:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがサイバーデーモンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

			count += summon_cyber(m_idx, y, x);

#ifdef JP
if (blind && count) msg_print("重厚な足音が近くで聞こえる。");
#else
			if (blind && count) msg_print("You hear heavy steps nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_MONSTER */
		case 160+18:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で仲間を召喚した！", m_name);
#else
			else msg_format("%^s magically summons help!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_MONSTERS */
		case 160+19:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でモンスターを召喚した！", m_name);
#else
			else msg_format("%^s magically summons monsters!", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_ANT */
		case 160+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でアリを召喚した。", m_name);
#else
			else msg_format("%^s magically summons ants.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_SPIDER */
		case 160+21:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で虫を召喚した。", m_name);
#else
			else msg_format("%^s magically summons insects.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_HOUND */
		case 160+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でハウンドを召喚した。", m_name);
#else
			else msg_format("%^s magically summons hounds.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_HYDRA */
		case 160+23:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でヒドラを召喚した。", m_name);
#else
			else msg_format("%^s magically summons hydras.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("多くのものが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_ANGEL */
		case 160+24:
		{
			int num = 1;

			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で天使を召喚した！", m_name);
#else
			else msg_format("%^s magically summons an angel!", m_name);
#endif

			if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
			{
				num += rlev/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, PM_ALLOW_GROUP);
			}

			if (count < 2)
			{
#ifdef JP
if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
				if (blind && count) msg_print("You hear something appear nearby.");
#endif
			}
			else
			{
#ifdef JP
if (blind) msg_print("多くのものが間近に現れた音がする。");
#else
				if (blind) msg_print("You hear many things appear nearby.");
#endif
			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_DEMON */
		case 160+25:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			///msg131221
			else msg_format("%^sは悪魔を召喚した！", m_name);
#else
			else msg_format("%^s magically summons a demon!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_UNDEAD */
		case 160+26:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でアンデッドの強敵を召喚した！", m_name);
#else
			else msg_format("%^s magically summons an undead adversary!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_DRAGON */
		case 160+27:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法でドラゴンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons a dragon!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("何かが間近に現れた音がする。");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));

			break;
		}

		/* RF6_S_HI_UNDEAD */
		case 160+28:
		{
			disturb(1, 1);

			///mod140726 モルゴス削除　モルゴスは救援召喚でナズグルとかバルログとか全部呼ぶ
			if (((m_ptr->r_idx == MON_SAURON) || (m_ptr->r_idx == MON_ANGMAR)) && ((r_info[MON_NAZGUL].cur_num+2) < r_info[MON_NAZGUL].max_num))
			{
				int cy = y;
				int cx = x;

#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で幽鬼戦隊を召喚した！", m_name);
#else
				else msg_format("%^s magically summons rangers of Nazgul!", m_name);
#endif
				msg_print(NULL);

				for (k = 0; k < 30; k++)
				{
					if (!summon_possible(cy, cx) || !cave_empty_bold(cy, cx))
					{
						int j;
						for (j = 100; j > 0; j--)
						{
							scatter(&cy, &cx, y, x, 2, 0);
							if (cave_empty_bold(cy, cx)) break;
						}
						if (!j) break;
					}
					if (!cave_empty_bold(cy, cx)) continue;

					if (summon_named_creature(m_idx, cy, cx, MON_NAZGUL, mode))
					{
						y = cy;
						x = cx;
						count++;
						if (count == 1)
#ifdef JP
msg_format("「幽鬼戦隊%d号、ナズグル・ブラック！」", count);
#else
							msg_format("A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'",count);
#endif
						else
#ifdef JP
msg_format("「同じく%d号、ナズグル・ブラック！」", count);
#else
							msg_format("Another one says 'Number %d, Nazgul-Black!'",count);
#endif
						msg_print(NULL);
					}
				}
#ifdef JP
msg_format("「%d人そろって、リングレンジャー！」", count);
#else
msg_format("They say 'The %d meets! We are the Ring-Ranger!'.", count);
#endif
				msg_print(NULL);
			}
			else
			{
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で強力なアンデッドを召喚した！", m_name);
#else
				else msg_format("%^s magically summons greater undead!", m_name);
#endif

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
msg_print("間近で何か多くのものが這い回る音が聞こえる。");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF6_S_HI_DRAGON */
		case 160+29:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で古代ドラゴンを召喚した！", m_name);
#else
			else msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("多くの力強いものが間近に現れた音が聞こえる。");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF6_S_AMBERITES */
		case 160+30:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sがアンバーの王族を召喚した！", m_name);
#else
			else msg_format("%^s magically summons Lords of Amber!", m_name);
#endif



			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("力強い足音が近くに現れるのが聞こえた。");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF6_S_UNIQUE */
		case 160+31:
		{
			bool uniques_are_summoned = FALSE;
			int non_unique_type = SUMMON_HI_UNDEAD;

			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^sが魔法で特別な強敵を召喚した！", m_name);
#else
			else msg_format("%^s magically summons special opponents!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (count) uniques_are_summoned = TRUE;
			///sys ユニーク召喚の勢力判定
			if ((m_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (blind && count)
			{
#ifdef JP
				msg_format("多くの%sが間近に現れた音が聞こえる。", uniques_are_summoned ? "力強いもの" : "もの");
#else
				msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}
///mod ここから先新スペル追加部
///モンスターの新スペルはラーニング不可

		/* RF9_FIRE_STORM */
		case 192+0:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが炎の嵐の呪文を唱えた。", "%^s casts a fire storm."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_FIRE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}
		/* RF9_ICE_STORM */
		case 192+1:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが氷の嵐の呪文を唱えた。", "%^s casts a frost storm."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_COLD, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}
		/* RF9_THUNDER_STORM */
		case 192+2:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが雷の嵐の呪文を唱えた。", "%^s casts a lightning storm."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_ELEC, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}
		/* RF9_ACID_STORM */
		case 192+3:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが酸の嵐の呪文を唱えた。", "%^s casts an acid storm."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_ACID, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF9_TOXIC_STORM */
		case 192+4:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが毒素の嵐の呪文を唱えた。", "%^s casts a toxic storm."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 8) + 50 + randint1(50);
			else	dam = (rlev * 5) + 25 + randint1(25);
			breath(y, x, m_idx, GF_POIS, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF9_BA_POLLUTE */
		case 192+5:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か奇怪な音を立てた。", "%^s makes strange noises."), m_name);
			else msg_format(_("%^sが汚染の球の呪文を唱えた。", "%^s casts a pollution ball."), m_name);
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 50 + damroll(10, 10);
			else	dam = (rlev * 3) + 50 + randint1(50);
			breath(y, x, m_idx, GF_POLLUTE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_POIS);
			update_smart_learn(m_idx, DRS_DISEN);
			break;
		}


		/* RF9_BA_DISI */
		case 192+6:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。突然周辺一帯の物が蒸発した気がする！",
                                    "%^s casts some complicated spell. You suddenly feel you surroundings evaporate!"), m_name);
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format(_("%^sはどこからか巨大な爆弾を取り出した！",
                                                                                            "%^s takes out a huge bomb!"), m_name);
			else msg_format(_("%^sが原子分解の呪文を唱えた。", "%^s casts a disintegration spell."), m_name);
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 2) + 50 + randint1(50);
			else	dam = rlev + 25 + randint1(25);
			breath(y, x, m_idx, GF_DISINTEGRATE, dam, rad, FALSE, 0, FALSE);
			//update_smart_learn(m_idx, DRS_POIS);

			//v1.1.58 この攻撃で初めて視界が通ったかもしれないのでBGMを再チェック
			check_music_mon_r_idx(m_idx);

			break;
		}

		/*:::RF9_BA_HOLY*/
		case 192+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを詠唱した。", m_name);
#else
			if (blind) msg_format("%^s chants something.", m_name);
#endif

#ifdef JP
else msg_format("%^sが破邪の光球の呪文を唱えた。", m_name);
#else
			else msg_format("%^s casts a ball of holy energy.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			breath(y, x, m_idx, GF_HOLY_FIRE, dam, 2, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 500;
			break;
		}

		/* RF9_METEOR */
		case 192+8:
		{
			disturb(1, 1);
			if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。何かが頭上から降ってきた！",
                                    "%^s casts some complicated spell. Something falls on your head!"), m_name);
			else msg_format(_("%^sがメテオストライクの呪文を唱えた！", "%^s invokes a meteor strike!"), m_name);
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 5;
				dam = (rlev * 4) + damroll(1,300);
			}
			else
			{
				rad = 4;
				dam = (rlev * 2) + damroll(1,150);
			}
			breath(y, x, m_idx, GF_METEOR, dam, rad, FALSE, 0, FALSE);
			break;
		}

		/*:::RF9_BA_DISTORTION*/
		case 192+9:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何か複雑な呪文を唱えた。", m_name);
#else
			if (blind) msg_format("%^s casts some complicated spell.", m_name);
#endif

#ifdef JP
else msg_format("%^sが空間歪曲の呪文を唱えた。", m_name);
#else
			else msg_format("%^s invokes a spatial distortion.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 1 : 2));
			breath(y, x, m_idx, GF_DISTORTION, dam, 4, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF9_PUNISH_1 */
		case 192+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sがあなたに魔除けのまじないを唱えた。",
                                "%^s casts a warding charm at you."), m_name);

			dam = damroll(3, 8);
			breath(y, x, m_idx, GF_PUNISH_1, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 300;
			break;
		}

		/* RF9_PUNISH_2 */
		case 192+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sがあなたに向かって聖なる言葉を唱えた。",
                                "%^s chants holy words at you."), m_name);

			dam = damroll(8, 8);
			breath(y, x, m_idx, GF_PUNISH_2, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 300;

			break;
		}

		/* RF9_PUNISH_3 */
		case 192+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かを大声で叫んだ。", "%^s screams something loudly."), m_name);
            else msg_format(_("%^sがあなたを指さして退魔の呪文を唱えた。",
                                "%^s points at you, casting a spell of exorcism."), m_name);

			dam = damroll(10, 15);
			breath(y, x, m_idx, GF_PUNISH_3, dam, 0, FALSE, 0, FALSE);
			nue_check_mult = 400;
			update_smart_learn(m_idx, DRS_HOLY);
			break;
		}

		/* RF9_CAUSE_4 */
		case 192+13:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

			///msg131221
			if (blind) msg_format(_("%^sが何か神々しい言葉を叫んだ。", "%^s screams some divine words."), m_name);
			///msg131221
			else msg_format(_("%^sがあなたに向けて破邪の印を放った！",
                                "%^s fires an exorcism sigil at you!"), m_name);

			dam = damroll(15, 15);
			breath(y, x, m_idx, GF_PUNISH_4, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 400;
			break;
		}

		/* RF9_BO_HOLY */
		case 192+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かをつぶやいた。", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif
            else msg_format(_("%^sが聖なる矢の呪文を唱えた。", "%^s casts a holy bolt."), m_name);

			dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_HOLY_FIRE, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			update_smart_learn(m_idx, DRS_REFLECT);
			nue_check_mult = 300;
			break;
		}
		/* RF9_GIGANTIC_LASER*/
		case 192+15:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが何かを放射した！", m_name);
#else
			if (blind) msg_format("%^s fires something!", m_name);
#endif
			else if(m_ptr->r_idx == MON_MARISA)msg_format(_("%^sがマスタースパークを放った！", "%^s fires a Master Spark!"), m_name);
			else msg_format(_("%^sが巨大なレーザーを放った！", "%^s fires a huge laser!"), m_name);

			dam = damroll(10, 10) +  rlev * (((r_ptr->flags2 & RF2_POWERFUL) ? 8 : 4));
			masterspark(y, x, m_idx, GF_NUKE, dam, 0, FALSE,((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			update_smart_learn(m_idx, DRS_LITE);
			update_smart_learn(m_idx, DRS_FIRE);
			nue_check_mult = 300;
			break;
		}


		/* RF9_BO_SOUND */
		case 192+16:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが大きな音を放った。", m_name);
#else
			if (blind) msg_format("%^s makes a loud noise.", m_name);
#endif

            else msg_format(_("%^sが音符の弾を放った。", "%^s fires a musical note bullet."), m_name);

			if (r_ptr->flags2 & RF2_POWERFUL)	dam = damroll(10, 10) + (rlev * 3 / 2);
			else	dam = damroll(3, 10) + rlev;

			bolt(m_idx, GF_SOUND, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_SOUND);
			break;
		}


		/* RF9_S_ANIMAL */
		///mod140419 動物召喚　
		case 192+17:
		{
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sが動物を召喚した！", "%^s magically summons animals!"), m_name);

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANIMAL, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
                msg_print(_("あなたは獰猛な唸り声に包まれた。", "You hear ferocious growls around you."));
			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}


		/* RF9_BEAM_LITE */
		case 192+18:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かを放った。", "%^s fires something."), m_name);
            else msg_format(_("%^sがレーザーを放った。", "%^s fires a laser."), m_name);

			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 4) : (rlev * 2);
			beam(m_idx, GF_LITE, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 300;
			break;
		}


		/* RF9_HELLFIRE */
		case 192+20:
		{
			disturb(1, 1);

			if (blind) msg_format(_("%^sが何か禍々しい複雑な呪文を詠唱した。",
                                    "%^s casts some ominous and complicated spell."), m_name);
			else msg_format(_("%^sがヘル・ファイアの呪文を詠唱した。", "%^s invokes hellfire."), m_name);

			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);
			breath(y, x, m_idx, GF_HELL_FIRE, dam, rad, FALSE, 0, FALSE);
			//update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF9_HOLYFIRE */
		case 192+21:
		{
			disturb(1, 1);

			if (blind) msg_format(_("%^sが何か神々しい複雑な呪文を詠唱した。",
                                    "%^s casts some complex divinely spell."), m_name);
			else msg_format(_("%^sがホーリー・ファイアの呪文を詠唱した。", "%^s invokes holy fire."), m_name);

			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);
			breath(y, x, m_idx, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 800;
			break;
		}
		/* RF9_FINALSPARK*/
		case 192+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが強烈なエネルギーを放射した！", m_name);
#else
			if (blind) msg_format("%^s releases intense energy!", m_name);
#endif
            else msg_format(_("%^sがファイナルスパークを放った！", "%^s fires a Final Spark!"), m_name);

			dam = 200 + randint1(rlev) +  rlev * 2;
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,2);
			nue_check_mult = 300;

			/*:::Hack - ファイナルスパーク発動後は＠が行動するまで魔法を使わない*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;

			//v1.1.58 この攻撃で初めて視界が通ったかもしれないのでBGMを再チェック
			check_music_mon_r_idx(m_idx);

			break;
		}


		/* RF9_TORNADO */
		case 192+23:
		{
			disturb(1, 1);

			if (blind) msg_format(_("%^sが竜巻を起こした。", "%^s creates a tornado."), m_name);
			else msg_format(_("%^sが竜巻を起こした。", "%^s creates a tornado."), m_name);

			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 3) + 50 + randint1(50);
			else	dam = (rlev * 2) + 25 + randint1(25);
			breath(y, x, m_idx, GF_TORNADO, dam, rad, FALSE, 0, FALSE);
			break;
		}


		///pend モンスターの*破壊*魔法　詳細未検証 現状＠の足元の地形が破壊されないため別処理が要るかも

		/* RF9_DESTRUCTION */
		case 192+24:
		{

			disturb(1, 1);

			if(m_ptr->cdis > MAX_SIGHT) msg_format(_("巨大な爆発音が響き、一瞬ダンジョンが揺れた。",
                                                    "A powerful explosion sounds, and the dungeon trembles momentarily."));
			else if (blind) msg_format(_("%^sが何かを唱えると地響きがした…", "%^s casts something, and there is a rumbling sound..."), m_name);
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format(_("%^sはどこからか巨大な爆弾を取り出した！",
                                                                                            "%^s takes out a huge bomb!"), m_name);
			else msg_format(_("%^sが破壊の言葉を詠唱した！", "%^s invokes the Word of Destruction!"), m_name);

			if(destroy_area(m_ptr->fy, m_ptr->fx,10,FALSE,FALSE,FALSE))
			{
				if(distance(py, px, m_ptr->fy, m_ptr->fx)<10)
				{
					if(check_activated_nameless_arts(JKF1_DESTRUCT_DEF))
					{
						msg_format(_("あなたは破壊の力を物ともしなかった！", "You are protected from destruction!"));
						break;
					}
					dam = p_ptr->chp / 2;
					if(p_ptr->pclass != CLASS_CLOWNPIECE)
					{
						//v1.1.86 NOESCAPE→ATTACKに
						take_hit(DAMAGE_ATTACK, dam, m_name, -1);
					}
					if(p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
					{
						msg_format(_("あなたは吹き飛ばされた！", "You get blown away!"));
						teleport_player_away(m_idx, 100);
					}
					nue_check_mult = 10;
				}
			}

			break;
		}
		/* RF9_S_DEITY */
		///mod140228 神格召喚　
		case 192+25:
		{
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sが神格を召喚した！", "%^s summons deities!"), m_name);

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DEITY, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
                msg_print(_("あなたの周囲は異様な空気に満たされた。", "You sense something supernatural appear nearby."));
            }
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF9_S_HI_DEMON */
		///mod140228 上位悪魔召喚　
		case 192+26:
		{
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sが上位悪魔を召喚した！", "%^s summons greater demons!"), m_name);

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
                msg_print(_("あなたの周囲は邪悪なざわめきに満たされた。", "You sense evil presence nearby."));
			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}

		/* RF9_S_KWAI */
		///mod140118 妖怪召喚　基本的にユニークは出ないことにしておく
		case 192+27:
		{
			disturb(1, 1);

            if (blind) msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
            else msg_format(_("%^sが妖怪を召喚した！", "%^s magically summons youkai!"), m_name);

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_KWAI, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
                msg_print(_("あなたの周囲は面妖なざわめきに満たされた。", "You sense mysterious presence nearby."));
			}
			if(!blind && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
			break;
		}



		/* RF9_TELE_APPROACH */
		case 192+28:
		{
			if (!direct) return (FALSE);
			if (teleport_barrier(m_idx)) break;

			disturb(1, 1);
//			teleport_monster_to(m_idx, y, x, 100, 0L);
			teleport_monster_to(m_idx, y, x, 100, TELEPORT_FORCE_NEXT);

            msg_format(_("%sが瞬時に間合いを詰めてきた。", "%s teleports closer to you."), m_name);

			break;

		}
		/* RF9_TELE_HI_APPROACH */
		case 192+29:
		{

			if ((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappear from the floor."), m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			if (teleport_barrier(m_idx)) break;

			disturb(1, 1);
			teleport_monster_to(m_idx, y, x, 100, 0L);
			///mod150926 視界外テレポからの連続行動魔法の可能性を減らした
			if (!one_in_(6) && !ironman_nightmare)
			{
				m_ptr->mflag |= (MFLAG_NICE);
				repair_monsters = TRUE;
			}

			if (m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) msg_format(_("今何か…気のせいか？",
                                                                                        "What was that... your imagination?"));
			else if (distance(y, x, m_ptr->fy, m_ptr->fx) < 3)
			{
				msg_format(_("%sが突然あなたの近くに現れた。", "%s suddenly appears close to you."), m_name);
			}
			else
			{
				msg_format(_("%sが瞬時に移動した。", "%s teleports closer to you."), m_name);
			}

			//v1.1.58 この攻撃で初めて視界が通ったかもしれないのでBGMを再チェック
			check_music_mon_r_idx(m_idx);

			break;
		}
		/* RF9_MAELSTROM*/
		case 192+30:
		{
			int rad;
	//		if (!direct) return (FALSE);
			disturb(1, 1);

			//v1.1.91 特殊処理　尤魔のメイルシュトロムは石油地形を作る

			if (blind)
				msg_format(_("突然大渦に呑み込まれた！", "You are suddenly engulfed in a maelstrom!"), m_name);
			else if (m_ptr->r_idx == MON_YUMA)
				msg_format(_("%^sが石油の大渦を巻き起こした！", "%^s conjures a huge whirlpool of oil!"),m_name);
			else
				msg_format(_("%^sがメイルシュトロムの呪文を詠唱した。", "%^s conjures a maelstrom."), m_name);

			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				dam = (rlev * 8) + randint1(rlev * 6);
				rad = 8;
			}
			else
			{
				dam = (rlev * 5) + randint1(rlev * 3);
				rad = 6;
			}
			if (m_ptr->r_idx == MON_YUMA)
			{
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP | PROJECT_HIDE), FALSE);
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_DIG_OIL, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP), FALSE);
			}
			else
			{
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), FALSE);
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, rad, GF_WATER_FLOW, (PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP | PROJECT_HIDE), FALSE);
			}
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF9_ALARM*/
		case 192+31:
		{
			disturb(1, 1);
#ifdef JP
			if(m_ptr->r_idx == MON_CLOWNPIECE)
				msg_format("%^sの持つ松明が煌々と輝いた！", m_name);
			else
				msg_format("%^sは強烈な警報音を発した！", m_name);

#else
			if(m_ptr->r_idx == MON_CLOWNPIECE)
				msg_format("The torch %^s is holding starts to glitter!", m_name);
			else
				msg_format("%^s sounds a loud alarm!", m_name);
#endif

			aggravate_monsters(m_idx,TRUE);
			break;
		}

//v1.1.58 変な括弧があったので削除　いつから間違っていた？
//		}




	}
	/*:::魔法使用終了　魔法を使用したモンスターがこの時点で死んでることもあるので注意*/

	//v1.1.73 八千慧のモンスター買収フラグ解除
	flag_bribe_summon_monsters = FALSE;

///class ものまねと青魔の特殊処理らしい
	if ((p_ptr->action == ACTION_LEARN) && thrown_spell > 175)
	{
		learn_spell(thrown_spell - 96);
	}

	//v1.1.82 新職業「弾幕研究家」の特技習得処理
	if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		//この時点で特技使用モンスターがいない場合、処理が面倒になるので習得不可
		if (m_ptr->r_idx)
		{
			learn_monspell_new(thrown_spell, m_ptr, TRUE, learnable,caster_dist);

		}

	}

	if (seen && maneable && !world_monster && (p_ptr->pclass == CLASS_IMITATOR))
	{
		if (thrown_spell != 167) /* Not RF6_SPECIAL */
		{
			if (p_ptr->mane_num == MAX_MANE)
			{
				int i;
				p_ptr->mane_num--;
				for (i = 0;i < p_ptr->mane_num;i++)
				{
					p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
					p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
				}
			}
			p_ptr->mane_spell[p_ptr->mane_num] = thrown_spell - 96;
			p_ptr->mane_dam[p_ptr->mane_num] = dam;
			p_ptr->mane_num++;
			new_mane = TRUE;

			p_ptr->redraw |= (PR_IMITATION);
		}
	}


	//ぬえ特殊処理　行動後のモンスターが正体を看破る そのモンスターがもう死んでるときは無効
	if (p_ptr->pclass == CLASS_NUE && m_ptr->r_idx)
	{
		check_nue_revealed(m_idx, nue_check_mult);

	}

	/* Remember what the monster did to us */
	/*:::＠の視界内でモンスターが使った呪文を記憶する部分*/
	if (can_remember)
	{
		/* Inate spell */
		if (thrown_spell < 32 * 4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32 * 5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32 * 6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
		else if (thrown_spell < 32 * 7)
		{
			r_ptr->r_flags9 |= (1L << (thrown_spell - 32 * 6));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/*:::「力強いモンスター」の学習　とりあえず何でも目の前で魔法使ったらわかることにしておく*/
		if(r_ptr->flags2 & RF2_POWERFUL) r_ptr->r_flags2 |= RF2_POWERFUL;


	}

	///mod140721 低難度ではモンスターは魔法を連続使用しない
	if(difficulty <= DIFFICULTY_NORMAL && m_ptr->r_idx)
	{
		m_ptr->mflag |= (MFLAG_NICE);
		repair_monsters = TRUE;
	}

	kyouko_echo(FALSE,thrown_spell);

	/* Always take note of monsters that kill you */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}
	//自爆したモンスターを消去(@が死んだらダンプに名前出すために消去回避)
	if(suisidebomb && !p_ptr->is_dead ) delete_monster_idx(m_idx);

	/* A spell was cast */
	return (TRUE);
}


