/* File: mspells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells (attack monster) */

#include "angband.h"


/*
 * Monster casts a breath (or ball) attack at another monster.
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
///sys mon 破邪系の呪いなど投射扱いの攻撃を実装するとき変更要
static void monst_breath_monst(int m_idx, int y, int x, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if (rad < 1 && breath) rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	switch (typ)
	{
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
		flg |= (PROJECT_HIDE | PROJECT_AIMED);

	case GF_METEOR:
	case GF_TORNADO:
		flg |= PROJECT_JUMP;
		break;

		break;
	}

	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Monster casts a bolt at another monster
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void monst_bolt_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	if (typ != GF_ARROW) flg |= PROJECT_REFLECTABLE;
	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void monst_beam_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU;

	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

/*
 * Determine if a beam spell will hit the target.
 */
static bool direct_beam(int y1, int x1, int y2, int x2, monster_type *m_ptr)
{
	bool hit2 = FALSE;
	int i, y, x;

	int grid_n = 0;
	u16b grid_g[2048];

	bool friend_ = is_pet(m_ptr);

	if(m_ptr->r_idx == MON_MASTER_KAGUYA) friend_ = TRUE;

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, PROJECT_THRU);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if (y == y2 && x == x2)
			hit2 = TRUE;
		else if (friend_ && cave[y][x].m_idx > 0 &&
			 !are_enemies(m_ptr, &m_list[cave[y][x].m_idx]))
		{
			/* Friends don't shoot friends */
			return FALSE;
		}

		if (friend_ && player_bold(y, x))
			return FALSE;
	}
	if (!hit2)
		return FALSE;
	return TRUE;
}

/*:::指定グリッドがブレスの効果範囲に入るか判定するらしい*/
static bool breath_direct(int y1, int x1, int y2, int x2, int rad, int typ, bool friend_)
{
	/* Must be the same as projectable() */

	int i;

	/* Initial grid */
	int y = y1;
	int x = x1;

	int grid_n = 0;
	u16b grid_g[2048];

	int grids = 0;
	byte gx[2048], gy[2048];
	byte gm[32];
	int gm_rad = rad;

	bool hit2 = FALSE;
	bool hityou = FALSE;

	int flg;

	switch (typ)
	{
	case GF_LITE:
	case GF_LITE_WEAK:
		flg = PROJECT_LOS;
		break;
	case GF_DISINTEGRATE:
	case GF_BLAZINGSTAR:
		flg = PROJECT_DISI;
		break;
	default:
		flg = 0;
		break;
	}

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, flg);

	/* Project along the path */
	for (i = 0; i < grid_n; ++i)
	{
		int ny = GRID_Y(grid_g[i]);
		int nx = GRID_X(grid_g[i]);

		if (flg & PROJECT_DISI)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx)) break;
		}
		else if (flg & PROJECT_LOS)
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_los_bold(ny, nx)) break;
		}
		else
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT)) break;
		}

		/* Save the "blast epicenter" */
		y = ny;
		x = nx;
	}

	grid_n = i;

	if (!grid_n)
	{
		if (flg & PROJECT_DISI)
		{
			if (in_disintegration_range(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (in_disintegration_range(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
		else if (flg & PROJECT_LOS)
		{
			if (los(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (los(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
		else
		{
			if (projectable(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (projectable(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
	}
	else
	{
		breath_shape(grid_g, grid_n, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, y, x, typ, FALSE);

		for (i = 0; i < grids; i++)
		{
			/* Extract the location */
			y = gy[i];
			x = gx[i];

			if ((y == y2) && (x == x2)) hit2 = TRUE;
			if (player_bold(y, x)) hityou = TRUE;
		}
	}

	if (!hit2) return FALSE;
	if (friend_ && hityou) return FALSE;

	return TRUE;
}

/*
 * Get the actual center point of ball spells (rad > 1) (originally from TOband)
 */
void get_project_point(int sy, int sx, int *ty, int *tx, int flg)
{
	u16b path_g[128];
	int  path_n, i;

	path_n = project_path(path_g, MAX_RANGE, sy, sx, *ty, *tx, flg);

	*ty = sy;
	*tx = sx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		sy = GRID_Y(path_g[i]);
		sx = GRID_X(path_g[i]);

		/* Hack -- Balls explode before reaching walls */
		if (!cave_have_flag_bold(sy, sx, FF_PROJECT)) break;

		*ty = sy;
		*tx = sx;
	}
}

/*
 * Check should monster cast dispel spell at other monster.
 */
static bool dispel_check_monster(int m_idx, int t_idx)
{
	monster_type *t_ptr = &m_list[t_idx];

	/* Invulnabilty */
	if (MON_INVULNER(t_ptr)) return TRUE;

	/* Speed */
	if (t_ptr->mspeed < 135)
	{
		if (MON_FAST(t_ptr)) return TRUE;
	}

	/* Riding monster */
	if (t_idx == p_ptr->riding)
	{
		if (dispel_check(m_idx)) return TRUE;
	}

	/* No need to cast dispel spell */
	return FALSE;
}

/*
 * Monster tries to 'cast a spell' (or breath, etc)
 * at another monster.
 *
 * The player is only disturbed if able to be affected by the spell.
 */
/*:::モンスター→モンスターの魔法攻撃*/
///sys mon モンスター間の攻撃魔法　＠へのと同様にf9フラグ実装処理
///mod140424 新フラグ対応
bool monst_spell_monst(int m_idx)
{
	int y = 0, x = 0;
	int i, k, t_idx = 0;
	int thrown_spell=0, count = 0;
	int rlev;
	int dam = 0;
	int start;
	int plus = 1;
	u32b u_mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);
	int rad = 0; //For elemental balls

	byte spell[128], num = 0;

	char m_name[160];
	char t_name[160];

#ifndef JP
	char m_poss[160];
#endif

	monster_type *m_ptr = &m_list[m_idx];
	monster_type *t_ptr = NULL;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *tr_ptr = NULL;

	u32b f4, f5, f6, f9;

	bool wake_up = FALSE;
	bool fear = FALSE;

	bool blind = (p_ptr->blind ? TRUE : FALSE);

	bool see_m = is_seen(m_ptr);
	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (m_ptr->ml && maneable && !world_monster);

	int caster_dist = m_ptr->cdis;	//モンスターが魔法を使った瞬間の＠からの距離

	bool see_t;
	bool see_either;
	bool known;
	bool suisidebomb = FALSE; //自爆　サイバイマン用

	bool pet = is_pet(m_ptr);

	bool in_no_magic_dungeon = (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && dun_level
		&& (!p_ptr->inside_quest || is_fixed_quest_idx(p_ptr->inside_quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	bool resists_tele = FALSE;

	/* Prepare flags for summoning */
	if (!pet) u_mode |= PM_ALLOW_UNIQUE;

	/* Cannot cast spells when confused */
	if (MON_CONFUSED(m_ptr)) return (FALSE);

	//マミゾウ二ッ岩家の裁き
	if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == m_idx)
		return (FALSE);

	/* Extract the racial spell flags */
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;
	f9 = r_ptr->flags9;

	/* Target is given for pet? */
	if (pet_t_m_idx && pet)
	{
		t_idx = pet_t_m_idx;
		t_ptr = &m_list[t_idx];

		/* Cancel if not projectable (for now) */
		if ((m_idx == t_idx) || !projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
		{
			t_idx = 0;
		}
	}


	/* Is there counter attack target? */
	if (!t_idx && m_ptr->target_y)
	{
		t_idx = cave[m_ptr->target_y][m_ptr->target_x].m_idx;

		if (t_idx)
		{
			t_ptr = &m_list[t_idx];

			/* Cancel if neither enemy nor a given target */
			if ((m_idx == t_idx) ||
			    ((t_idx != pet_t_m_idx) && !are_enemies(m_ptr, t_ptr)))
			{
				t_idx = 0;
			}

			/* Allow only summoning etc.. if not projectable */
			else if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			{
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);

			}
		}
	}

	if(m_ptr->r_idx == MON_HASSYAKUSAMA && hassyakusama_target_idx)
	{
		if(m_list[hassyakusama_target_idx].r_idx)
		{
			t_idx = hassyakusama_target_idx;
			t_ptr = &m_list[t_idx];
			if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			{
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);
			}
		}
		else
			hassyakusama_target_idx = 0;
	}


	/* Look for enemies normally */
	if (!t_idx)
	{
		bool success = FALSE;

		if (p_ptr->inside_battle)
		{
			start = randint1(m_max-1) + m_max;
			if (randint0(2)) plus = -1;
		}
		else start = m_max + 1;

		/* Scan thru all monsters */
		for (i = start; ((i < start + m_max) && (i > start - m_max)); i += plus)
		{
			int dummy = (i % m_max);
			if (!dummy) continue;

			t_idx = dummy;
			t_ptr = &m_list[t_idx];

			/* Skip dead monsters */
			if (!t_ptr->r_idx) continue;

			/* Monster must be 'an enemy' */
			if ((m_idx == t_idx) || !are_enemies(m_ptr, t_ptr)) continue;

			/* Monster must be projectable */
			if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;

			/* Get it */
			success = TRUE;
			break;
		}

		/* No enemy found */
		if (!success) return FALSE;
	}


	/* OK -- we've got a target */
	y = t_ptr->fy;
	x = t_ptr->fx;
	tr_ptr = &r_info[t_ptr->r_idx];

	/* Forget old counter attack target */
	reset_target(m_ptr);

	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);


	/* Remove unimplemented spells */
	f6 &= ~(RF6_WORLD | RF6_TRAPS | RF6_FORGET);

	///mod140818 感知範囲外の敵は召喚魔法を使いにくいようにする
	if(m_ptr->cdis > r_ptr->aaf && !one_in_(13))
	{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);

	}

	if (f4 & RF4_BR_LITE)
	{
		if (!los(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			f4 &= ~(RF4_BR_LITE);
	}
	///mod141010 メイルシュトロムはターゲットとかペット巻き込みとか面倒なので敵は使わない
	//if ((f9 & RF9_MAELSTROM) && distance(y,x,m_ptr->fy,m_ptr->fx) < 8 && !(r_ptr->flags2 & RF2_STUPID))
	///mod150604 闘技場でのみ使うようにした
	if ((f9 & RF9_MAELSTROM) && (!p_ptr->inside_battle || distance(y,x,m_ptr->fy,m_ptr->fx) > 5))
	{
		f9 &= ~(RF9_MAELSTROM);
	}
	//配下は*破壊*や時間停止を使わない
	if(is_pet(m_ptr))
	{
		f9 &= ~(RF9_DESTRUCTION);
		f6 &= ~(RF6_WORLD);

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
	//ヘカーティアは＠がターゲットのときしか特別行動をしない
	//v1.1.49 摩多羅隠岐奈の特別行動2も
	if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3 || m_ptr->r_idx == MON_OKINA)
	{
			f4 &= ~(RF4_SPECIAL2);
	}
	//サイバイマンはターゲットが近くにいて体力が低くないと特別行動をしない
	if(m_ptr->r_idx == MON_SAIBAIMAN)
	{
		if(m_ptr->hp > m_ptr->maxhp / 3 || distance(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx) > 1 || pet && m_ptr->cdis < 3)
			f4 &= ~(RF4_SPECIAL2);
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


	/* Remove unimplemented special moves */
	//モンスター→モンスターの特別行動は指定したものを除き抑止
	//なお、少し下の方にもうひとつ似たような抑止ルーチンがある
	if (f6 & RF6_SPECIAL)
	{
		if ((m_ptr->r_idx != MON_ROLENTO)
			&& (m_ptr->r_idx != MON_MOKOU)
			&& (m_ptr->r_idx != MON_SHION_2)
			&& (m_ptr->r_idx != MON_MICHAEL)
			&& (m_ptr->r_idx != MON_REIMU)
			&& (m_ptr->r_idx != MON_SUMIREKO)
			&& (m_ptr->r_idx != MON_YUKARI)
			&& (m_ptr->r_idx != MON_SEIJA)
			&& (m_ptr->r_idx != MON_TSUCHINOKO)
			&& (m_ptr->r_idx != MON_CHIMATA)
			&& (r_ptr->d_char != 'B'))
			f6 &= ~(RF6_SPECIAL);

		//v1.1.32 舞と里乃の特別行動も抑止しておく。見てないところで敵がランクアップしてたらさすがに理不尽なので。
	}

	if (f6 & RF6_DARKNESS)
	{
		bool vs_ninja = (p_ptr->pclass == CLASS_NINJA) && !is_hostile(t_ptr);
///mod131231 モンスターフラグ変更　閃光弱点をRF3からRFRへ
		if (vs_ninja &&
		    !(r_ptr->flags3 & RF3_UNDEAD) &&
		    !(r_ptr->flagsr & RFR_HURT_LITE) &&
		    !(r_ptr->flags7 & RF7_DARK_MASK))
			can_use_lite_area = TRUE;

		if (!(r_ptr->flags2 & RF2_STUPID))
		{
			if (d_info[dungeon_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			else if (vs_ninja && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}

	///mod160708 ターゲットに隣接した敵は隣接テレポを使わないようにする
	if ((f9 & (RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH)) && distance(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx) < 2)
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}

	/*:::反魔法洞窟では魔法を使わない（STUPIDは使う）*/
	///mod140423 「敵を黙らせる」効果中も同様に処理
	///mod150604 映姫処理追加
	//if (in_no_magic_dungeon && !(r_ptr->flags2 & RF2_STUPID))
	if ((in_no_magic_dungeon || p_ptr->silent_floor || (m_ptr->mflag & MFLAG_NO_SPELL))&& !(r_ptr->flags2 & RF2_STUPID))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);

	}

	//アリーナと闘技場では召喚を使わない。
	//v1.1.43 松明で狂気状態になると召喚を使わないことにしておく
	if (p_ptr->inside_arena || p_ptr->inside_battle
		|| m_ptr->mflag & MFLAG_LUNATIC_TORCH)
	{
		f4 &= ~(RF4_SUMMON_MASK | RF4_SHRIEK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK | RF6_TRAPS | RF6_TELE_LEVEL | RF6_WORLD | RF6_DARKNESS);
		f9 &= ~(RF9_SUMMON_MASK | RF9_DESTRUCTION | RF9_ALARM);
		if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI) f6 &= ~(RF6_SPECIAL); //v1.1.32

		if (m_ptr->r_idx == MON_ROLENTO) f6 &= ~(RF6_SPECIAL);
	}

	// 2/3で抑止→1/2で抑止
	if (p_ptr->inside_battle && !one_in_(2))
	{
		f6 &= ~(RF6_HEAL);
	}

	//乗馬中のモンスターはテレポートや警報などを使わない
	//だが騎兵の荒馬馴らしでメネルドールに乗っていると特別行動を使ってきたという報告がある。(v1.1.61)
	if (m_idx == p_ptr->riding)
	{
		f4 &= ~(RF4_RIDING_MASK);
		f5 &= ~(RF5_RIDING_MASK);
		f6 &= ~(RF6_RIDING_MASK);
		f9 &= ~(RF9_RIDING_MASK);
	}

	//v1.1.36 エタニティラルバは恐怖時にしか悪臭雲を使わないようにした
	if(m_ptr->r_idx == MON_LARVA)
	{
		if (!MON_MONFEAR(m_ptr))
		{
			f5 &= ~(RF5_BA_POIS);
		}
	}

	if (pet)
	{
		f4 &= ~(RF4_SHRIEK);
		f6 &= ~(RF6_DARKNESS | RF6_TRAPS);
		f9 &= ~( RF9_DESTRUCTION | RF9_ALARM);

		if (!(p_ptr->pet_extra_flags & PF_TELEPORT))
		{
			f6 &= ~(RF6_BLINK | RF6_TPORT | RF6_TELE_TO | RF6_TELE_AWAY | RF6_TELE_LEVEL);
			f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
		}

		if (!(p_ptr->pet_extra_flags & PF_ATTACK_SPELL))
		{
			f4 &= ~(RF4_ATTACK_MASK);
			f5 &= ~(RF5_ATTACK_MASK);
			f6 &= ~(RF6_ATTACK_MASK);
			f9 &= ~(RF9_ATTACK_MASK);
		}

		if (!(p_ptr->pet_extra_flags & PF_SUMMON_SPELL))
		{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		/* Prevent collateral damage */
		/*:::フランの分身は攻撃にプレイヤーを巻き込む*/
		if ((m_ptr->r_idx != MON_F_SCARLET_4) && !(p_ptr->pet_extra_flags & PF_BALL_SPELL) && (m_idx != p_ptr->riding))
		{
			if ((f4 & (RF4_BALL_MASK & ~(RF4_ROCKET))) ||
			    (f5 & RF5_BALL_MASK) ||
			    (f6 & RF6_BALL_MASK) ||
				(f9 & RF9_BALL_MASK) )
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, 0L);

				if (projectable(real_y, real_x, py, px))
				{
					int dist = distance(real_y, real_x, py, px);

					if (dist <= 2)
					{
						f4 &= ~(RF4_BALL_MASK & ~(RF4_ROCKET));
						f5 &= ~(RF5_BALL_MASK);
						f6 &= ~(RF6_BALL_MASK);
						f9 &= ~(RF9_BALL_MASK);
					}
					else if (dist <= 4)
					{
						f4 &= ~(RF4_BIG_BALL_MASK);
						f5 &= ~(RF5_BIG_BALL_MASK);
						f6 &= ~(RF6_BIG_BALL_MASK);
						f9 &= ~(RF9_BIG_BALL_MASK);
					}
					else if (dist <= 5)
					{
						f9 &= ~(RF9_BA_DISI | RF9_BA_METEOR | RF9_HELLFIRE | RF9_HOLYFIRE | RF9_TORNADO);


					}
				}
				else if (f5 & RF5_BA_LITE)
				{
					if ((distance(real_y, real_x, py, px) <= 4) && los(real_y, real_x, py, px))
						f5 &= ~(RF5_BA_LITE);
				}
			}

			/*:::巨大レーザーの範囲判定はちょっと難しいので端折った*/
			f9 &= ~(RF9_GIGANTIC_LASER | RF9_FINALSPARK);
			f4 &= ~(RF4_WAVEMOTION);


			if (f4 & RF4_ROCKET)
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, PROJECT_STOP);
				if (projectable(real_y, real_x, py, px) && (distance(real_y, real_x, py, px) <= 2))
					f4 &= ~(RF4_ROCKET);
			}

			if (((f4 & RF4_BEAM_MASK) ||
			     (f5 & RF5_BEAM_MASK) ||
			     (f9 & RF9_BEAM_MASK) ||
			     (f6 & RF6_BEAM_MASK)) &&
			    !direct_beam(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, m_ptr))
			{
				f4 &= ~(RF4_BEAM_MASK);
				f5 &= ~(RF5_BEAM_MASK);
				f6 &= ~(RF6_BEAM_MASK);
				f9 &= ~(RF9_BEAM_MASK);
			}

			if ((f4 & RF4_BREATH_MASK) ||
			    (f5 & RF5_BREATH_MASK) ||
			    (f9 & RF9_BREATH_MASK) ||
			    (f6 & RF6_BREATH_MASK))
			{
				/* Expected breath radius */
				int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

				if (!breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, 0, TRUE))
				{
					f4 &= ~(RF4_BREATH_MASK);
					f5 &= ~(RF5_BREATH_MASK);
					f6 &= ~(RF6_BREATH_MASK);
					f9 &= ~(RF9_BREATH_MASK);
				}
				else if ((f4 & RF4_BR_LITE) &&
					 !breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, GF_LITE, TRUE))
				{
					f4 &= ~(RF4_BR_LITE);
				}
				else if ((f4 & RF4_BR_DISI) &&
					 !breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, GF_DISINTEGRATE, TRUE))
				{
					f4 &= ~(RF4_BR_DISI);
				}
			}
		}

		/* Special moves restriction */

		if (f6 & RF6_SPECIAL)
		{
			if (m_ptr->r_idx == MON_ROLENTO)
			{
				if ((p_ptr->pet_extra_flags & (PF_ATTACK_SPELL | PF_SUMMON_SPELL)) != (PF_ATTACK_SPELL | PF_SUMMON_SPELL))
					f6 &= ~(RF6_SPECIAL);
			}
			else if (r_ptr->d_char == 'B')
			{
				if ((p_ptr->pet_extra_flags & (PF_ATTACK_SPELL | PF_TELEPORT)) != (PF_ATTACK_SPELL | PF_TELEPORT))
					f6 &= ~(RF6_SPECIAL);
			}
			else if(m_ptr->r_idx == MON_MICHAEL);

			///mod140626 霊夢は壁際では夢想封印を使えない
			else if(m_ptr->r_idx == MON_REIMU)
			{
				if (!in_bounds2(m_ptr->fy + 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy + 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy - 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy - 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx + 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx + 1],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx - 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx - 1],FF_PROJECT))
				f6 &= ~(RF6_SPECIAL);
			}
			else if(m_ptr->r_idx == MON_TSUCHINOKO);
			else if (m_ptr->r_idx == MON_CHIMATA);

			else f6 &= ~(RF6_SPECIAL);
		}
	}

	//輝夜(主)の行動抑制処理
	if(m_ptr->r_idx == MON_MASTER_KAGUYA)
	{
		int real_y = y;
		int real_x = x;

		if(!clean_shot(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, TRUE))
		{
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
		}

		if (!direct_beam(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, m_ptr))
		{
			f4 &= ~(RF4_BEAM_MASK);
			f5 &= ~(RF5_BEAM_MASK);
			f6 &= ~(RF6_BEAM_MASK);
			f9 &= ~(RF9_BEAM_MASK);
		}

		get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, 0L);

		if (projectable(real_y, real_x, py, px))
		{
			int dist = distance(real_y, real_x, py, px);

			if (dist <= 2)
			{
				f4 &= ~(RF4_BALL_MASK & ~(RF4_ROCKET));
				f5 &= ~(RF5_BALL_MASK);
				f6 &= ~(RF6_BALL_MASK);
				f9 &= ~(RF9_BALL_MASK);
			}
			else if (dist <= 4)
			{
				f4 &= ~(RF4_BIG_BALL_MASK);
				f5 &= ~(RF5_BIG_BALL_MASK);
				f6 &= ~(RF6_BIG_BALL_MASK);
				f9 &= ~(RF9_BIG_BALL_MASK);
			}
			else if (dist <= 5)
			{
				f9 &= ~(RF9_BA_DISI | RF9_BA_METEOR | RF9_HELLFIRE | RF9_HOLYFIRE | RF9_TORNADO);


			}
		}
		else if (f5 & RF5_BA_LITE)
		{
			if ((distance(real_y, real_x, py, px) <= 4) && los(real_y, real_x, py, px))
				f5 &= ~(RF5_BA_LITE);
		}

	}

	/* Remove some spells if necessary */

	if (!(r_ptr->flags2 & RF2_STUPID))
	{
		/* Check for a clean bolt shot */
		if (((f4 & RF4_BOLT_MASK) ||
		     (f5 & RF5_BOLT_MASK) ||
		     (f9 & RF9_BOLT_MASK) ||
		     (f6 & RF6_BOLT_MASK)) &&
		    !clean_shot(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, pet))
		{
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
		}

		/* Check for a possible summon */
		if (((f4 & RF4_SUMMON_MASK) ||
		     (f5 & RF5_SUMMON_MASK) ||
		     (f9 & RF9_SUMMON_MASK) ||
		     (f6 & RF6_SUMMON_MASK)) &&
		    !(summon_possible(t_ptr->fy, t_ptr->fx)))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
		}

		//v1.1.13 モンスターがSMARTなとき「公転周期の罠」が有効だとほとんど召喚を試みない
		//v1.1.54 ネムノ特技追加
		/* Check for a possible summon */

		if ((r_ptr->flags2 & RF2_SMART)
			&& ((CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO)) || (p_ptr->special_flags & SPF_ORBITAL_TRAP))
			&& ((f4 & RF4_SUMMON_MASK) || (f5 & RF5_SUMMON_MASK) || (f6 & RF6_SUMMON_MASK) || (f9 & RF9_SUMMON_MASK))
			&& !one_in_(4))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}



		/* Dispel magic */
		if ((f4 & RF4_DISPEL) && !dispel_check_monster(m_idx, t_idx))
		{
			/* Remove dispel spell */
			f4 &= ~(RF4_DISPEL);
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
//			f6 &= ~(RF6_RAISE_DEAD);

		/* Special moves restriction */
		if (f6 & RF6_SPECIAL)
		{
			if ((m_ptr->r_idx == MON_ROLENTO) && !summon_possible(t_ptr->fy, t_ptr->fx))
			{
				f6 &= ~(RF6_SPECIAL);
			}
		}
	}

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
		}
		//RF4_DISPEL
		if(f4 & RF4_DISPEL && one_in_(2)) thrown_spell = 96 + 2;

		/* Hack -- decline "teleport level" in some case */
		if ((f6 & RF6_TELE_LEVEL) && TELE_LEVEL_IS_INEFF((t_idx == p_ptr->riding) ? 0 : t_idx))
		{
			f6 &= ~(RF6_TELE_LEVEL);
		}
		///破壊使用判定処理
		if ( !pet && (f9 & RF9_DESTRUCTION))
		{
			int j;
			int mcount = 0;
			for (j = 1; j < m_max; j++)
			{
				monster_race *r_ptr_tmp = &r_info[m_list[j].r_idx];
				if(distance( m_ptr->fy, m_ptr->fx,m_list[j].fy,m_list[j].fx) < 10 && r_ptr_tmp->level > r_ptr->level / 2 && are_enemies(m_ptr,&m_list[j])) mcount++;
			}
			//	msg_format("CHECK2:cnt=%d",mcount);
			if(mcount > randint0(10) )
			{
				thrown_spell = 192+24; /* RF9_DESTRUCTION */

			}

		}





	}

	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return FALSE;

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
	for (k = 0; k < 32; k++)
	{
		if (f9 & (1L << k)) spell[num++] = k + 32 * 6;
	}

	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!p_ptr->playing || p_ptr->is_dead) return (FALSE);

	/* Handle "leaving" */
	if (p_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0x00);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

	/* Get the target's name (or "it") */
	monster_desc(t_name, t_ptr, 0x00);

	/* Choose a spell to cast */
	/*:::まだスペルが決まっていないときランダムに決めるようにした*/
	if(!thrown_spell) thrown_spell = spell[randint0(num)];

	see_t = is_seen(t_ptr);
	see_either = (see_m || see_t);

	/* Can the player be aware of this attack? */
	known = (m_ptr->cdis <= MAX_SIGHT) || (t_ptr->cdis <= MAX_SIGHT);

	if (p_ptr->riding && (m_idx == p_ptr->riding)) disturb(1, 1);

	/* Check for spell failure (inate attacks never fail) */
	if (!spell_is_inate(thrown_spell) && (in_no_magic_dungeon || (MON_STUNNED(m_ptr) && one_in_(2)) || (m_ptr->mflag & MFLAG_NO_SPELL)))
	{
		disturb(1, 1);
		/* Message */
#ifdef JP
		if (see_m) msg_format("%^sは呪文を唱えようとしたが失敗した。", m_name);
#else
		if (see_m) msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}

	/* Hex: Anti Magic Barrier */
	if (!spell_is_inate(thrown_spell) && magic_barrier(m_idx))
	{
#ifdef JP
		if (see_m) msg_format("反魔法バリアが%^sの呪文をかき消した。", m_name);
#else
		if (see_m) msg_format("Anti magic barrier cancels the spell which %^s casts.");
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
			msg_format(_("旧神の印が%^sの召喚魔法を無効化した！", "The sign of Elder Gods prevents %^s from summoning!"), m_name);
			return (TRUE);
		}
	}
	if(spell_is_summon(thrown_spell) && (p_ptr->special_flags & SPF_ORBITAL_TRAP))
	{
		msg_format(_("%^sが何かを召喚しようとしたが公転周期のトラップに阻害された。",
                    "%^s tries to summon help, but the orbital trap obstructs it."), m_name);
		return (TRUE);
	}

	if (is_hostile(m_ptr) && spell_is_summon(thrown_spell) && CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO) && randint1(r_ptr->level) < p_ptr->lev)
	{
		msg_format(_("%^sが何かを召喚しようとしたが、あなたは不躾な来訪者を拒絶した。",
                    "%^s tries to summon help, but you reject the uninvited guests."), m_name);
		return (TRUE);
	}

	if(is_hostile(m_ptr) && spell_is_summon(thrown_spell) && p_ptr->deportation && los(y,x,py,px))
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
			else if(monspell_list2[monspell_num].level && distance(py,px, m_ptr->fy, m_ptr->fx) < MAX_RANGE) //ダミーや特別行動は非対象
			{
				msg_format(_("あなたは%^sが唱えていた魔法を吸収した！",
                            "You absorb the spell cast by %^s!"), m_name);
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
                            "You failed to prevent %^s from casting a spell!"), m_name);
		}
	}
	//メルラン・ハッピーライブ
	if(p_ptr->pclass == CLASS_MERLIN  && !spell_is_inate(thrown_spell) && music_singing(MUSIC_NEW_MERLIN_SOLO) && distance(py,px,m_ptr->fy,m_ptr->fx) < MAX_RANGE)
	{

		//v1.1.65 三項演算子の優先順位を間違えててtmp値が1か2になってしまっていたので修正
		int tmp = r_ptr->level * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)) tmp *= 2;


		if(randint1(tmp) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]])
		{
			if(one_in_(3)) msg_format(_("%^sは呪文を唱えようとしてあらぬことを口走った。",
                                        "%^s tries to cast a spell, but fails to speak correctly."), m_name);
			else if(one_in_(2))msg_format(_("%^sはシアワセだ。", "%^s looks happy."), m_name);
			else msg_format(_("%^sは少しアレな様子だ。", "%^s is acting a bit strangely."), m_name);
			return (TRUE);
		}
	}

	can_remember = is_original_ap_and_seen(m_ptr);

	switch (thrown_spell)
	{
	/* RF4_SHRIEK */
	//モンスター同士の叫びは加速させないらしい
	case 96+0:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sが%sに向かって叫んだ。", m_name, t_name);
#else
				msg_format("%^s shrieks at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		wake_up = TRUE;

		break;

		/* RF4_DANMAKU */
	case 96+1:

		if (known)
		{
			if (see_either)
			{
				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを放った。", m_name);
#else
					if (blind) msg_format("%^s fires something.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに向けて弾幕を放った。", m_name, t_name);
#else
					msg_format("%^s fires danmaku at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_SHOOT);
		}
		dam = rlev;
		monst_bolt_monst(m_idx, y, x, GF_ARROW, dam, MS_SHOOT, learnable);

		break;
	/* RF4_DISPEL */
	case 96+2:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sが%sに対して魔力消去の呪文を念じた。", m_name, t_name);
#else
				msg_format("%^s invokes a spell to dispel magic at %s.", m_name, t_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (t_idx == p_ptr->riding) dispel_player();
		dispel_monster_status(t_idx);

		break;

	/* RF4_ROCKET */
	case 96+3:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを射った。", m_name);
#else
					msg_format("%^s shoots something.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sにロケットを発射した。", m_name, t_name);
#else
					msg_format("%^s fires a rocket at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_ROCKET,
				   dam, 2, FALSE, MS_ROCKET, learnable);

		break;

	/* RF4_SHOOT */
	case 96+4:
		{
			int r_idx = m_ptr->r_idx;
			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);
			if(r_idx == MON_SAKUYA || r_idx == MON_MURASA || r_idx == MON_NINJA_SLAYER) dam *= 2;
			else  if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA) dam = dam * 5 / 2;

			if (known && see_either)
			{
				if (blind)
				{
					msg_format(_("%^sが何かをどこかへ放ったようだ。", "%^s shoots something somewhere."), m_name);
				}
				else
				{
					if(r_idx == MON_NODEPPOU || r_idx == MON_DROLEM)
						msg_format(_("%^sが%sへ何かを放った。",
                                    "%^s fires something at %^s."), m_name, t_name);
					else if(r_idx == MON_SUMIREKO || r_idx == MON_SUMIREKO_2 || r_idx == MON_CHARGE_MAN || r_idx == MON_JURAL || r_idx == MON_NITORI || r_idx == MON_GYOKUTO || r_idx == MON_SEIRAN || r_idx == MON_EAGLE_RABBIT)
						msg_format(_("%^sが%sへ銃を撃った。",
                                    "%^s fires a gun at %s."), m_name,t_name);
					else if(r_idx == MON_HAMMERBROTHER)
						msg_format(_("%^sが%sへハンマーを投げた。",
                                    "%^s throws a hammer at %s."), m_name,t_name);
					else if(r_idx == MON_YAMAWARO)
						msg_format(_("%^sが%sへ手裏剣を投げた。",
                                    "%^s throws a shuriken at %s."), m_name,t_name);
					else if(r_idx == MON_KISUME)
						msg_format(_("%^sが%sへ骨を投げた。",
                                    "%^s throws a bone at %s."), m_name,t_name);
					else if(r_idx == MON_SAKUYA)
						msg_format(_("%^sが%sへ大量のナイフを投擲した。",
                                    "%^s throws a lot of knives at %s."), m_name,t_name);
					else if(r_idx == MON_MURASA)
						msg_format(_("%^sが%sに向かって錨を投げつけた！",
                                    "%^s throws an anchor at %s!"), m_name,t_name);
					else if(r_idx == MON_HALFLING_S || r_idx == MON_CULVERIN || r_idx == MON_EIKA)
						msg_format(_("%^sが%sへ石つぶてを放った。",
                                    "%^s fires a pebble at %s."), m_name,t_name);
					else if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA)
						msg_format(_("%^sが%sのほうへ大岩を投げつけた！",
                                    "%^s throws a boulder at %s!"), m_name,t_name);
					else if(r_idx == MON_NINJA_SLAYER)
						msg_format(_("%^sは%sにツヨイ・スリケンを放った！", "%^s throws a strong shuriken at %s!"), m_name,t_name);
					else msg_format(_("%^sが%sへ矢を放った。", "%^s shoots an arrows at %s."), m_name,t_name);
				}
			}

			if (!see_either) mon_fight = TRUE;

			sound(SOUND_SHOOT);

			monst_bolt_monst(m_idx, y, x, GF_ARROW, dam, MS_SHOOT, learnable);
		}
		break;

	/* RF4_br_holy */
	case 96+5:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに聖なるブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes holy energy at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE,
				   dam,0, TRUE, MS_BR_HOLY, learnable);

		break;


	/* RF4_BRHELL */
	case 96+6:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに地獄の業火のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes hellfire at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_HELL_FIRE,
				   dam,0, TRUE, MS_BR_HELL, learnable);

		break;





		/* RF4_アクアブレス */
	case 96+7:
	if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sにアクアブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes water at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 8) > 350 ? 350 : (m_ptr->hp / 8));
		monst_breath_monst(m_idx, y, x, GF_WATER,
				   dam,0, TRUE, MS_BR_AQUA, learnable);

		break;


	/* RF4_BR_ACID */
	case 96+8:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに酸のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_ACID,
				   dam,0, TRUE, MS_BR_ACID, learnable);

		break;

	/* RF4_BR_ELEC */
	case 96+9:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに稲妻のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes lightning at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_ELEC,
				   dam,0, TRUE, MS_BR_ELEC, learnable);

		break;

	/* RF4_BR_FIRE */
	case 96+10:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに火炎のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes fire at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_FIRE,
				   dam,0, TRUE, MS_BR_FIRE, learnable);

		break;

	/* RF4_BR_COLD */
	case 96+11:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに冷気のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes frost at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_COLD,
				   dam,0, TRUE, MS_BR_COLD, learnable);
		break;

	/* RF4_BR_POIS */
	case 96+12:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sにガスのブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes gas at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_POIS,
				   dam,0, TRUE, MS_BR_POIS, learnable);

		break;

	/* RF4_BR_NETH */
	case 96+13:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに地獄のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes nether at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_NETHER,
				   dam,0, TRUE, MS_BR_NETHER, learnable);

		break;

	/* RF4_BR_LITE */
	case 96+14:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに閃光のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes light at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_LITE,
				   dam,0, TRUE, MS_BR_LITE, learnable);

		break;

	/* RF4_BR_DARK */
	case 96+15:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに暗黒のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes darkness at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DARK,
				   dam,0, TRUE, MS_BR_DARK, learnable);

		break;

	/* RF4_WAVEMOTION */
	case 96+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何か強力な攻撃をした。", m_name);
#else
					msg_format("%^s performs a powerful attack.", m_name);
#endif

				}
				else if(m_ptr->r_idx == MON_TOYOHIME)
				{
					msg_format(_("%^sは扇のようなものを取り出して%sに向けて大きく振った！",
                                "%^s takes out a fan and sweeps it at %s!"), m_name,t_name);
				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに波動砲を放った！", m_name, t_name);
#else
					msg_format("%^s fires a wave-motion cannon at %s!", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((m_ptr->hp / 6) > 555 ? 555 : (m_ptr->hp / 6));
		masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, MS_WAVEMOTION, FALSE,2);
		/*:::Hack - 波動砲発射後は＠が行動するまで魔法を使わない*/
		m_ptr->mflag |= (MFLAG_NICE);
		repair_monsters = TRUE;
		break;



	/* RF4_BR_SOUN */
	case 96+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_JAIAN)
#ifdef JP
					msg_format("「ボォエ～～～～～～」");
#else
				msg_format("'Booooeeeeee'");
#endif
				else if (blind)
				{
#ifdef JP
					msg_format("%^sが衝撃波を放った。", m_name);
#else
					msg_format("%^s launches a shockwave.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに向けて衝撃波を放った。", m_name, t_name);
#else
					msg_format("%^s launches a shockwave at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_SOUND,
				   dam,0, TRUE, MS_BR_SOUND, learnable);

		break;

	/* RF4_BR_CHAO */
	case 96+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sにカオスのブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes chaos at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_CHAOS,
				   dam,0, TRUE, MS_BR_CHAOS, learnable);

		break;

	/* RF4_BR_DISE */
	case 96+19:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに劣化のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes disenchantment at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DISENCHANT,
				   dam,0, TRUE, MS_BR_DISEN, learnable);

		break;

	/* RF4_BR_NEXU */
	case 96+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに因果混乱のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes nexus at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 7) > 600 ? 600 : (m_ptr->hp / 7));
		monst_breath_monst(m_idx, y, x, GF_NEXUS,
				   dam,0, TRUE, MS_BR_NEXUS, learnable);

		break;

	/* RF4_BR_TIME */
	case 96+21:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに時間逆転のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes time at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_TIME,
				   dam,0, TRUE, MS_BR_TIME, learnable);

		break;

	/* RF4_BR_INER */
	case 96+22:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに遅鈍のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes inertia at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_INACT,
				   dam,0, TRUE, MS_BR_INERTIA, learnable);

		break;

	/* RF4_BR_GRAV */
	case 96+23:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに重力のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes gravity at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_GRAVITY,
				   dam,0, TRUE, MS_BR_GRAVITY, learnable);

		break;

	/* RF4_BR_SHAR */
	case 96+24:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
					msg_format("「ボ帝ビルカッター！！！」");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
				else if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに破片のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes shards at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_SHARDS,
				   dam,0, TRUE, MS_BR_SHARDS, learnable);

		break;

	/* RF4_BR_PLAS */
	case 96+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sにプラズマのブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes plasma at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 4) > 1200 ? 1200 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_PLASMA,
				   dam,0, TRUE, MS_BR_PLASMA, learnable);

		break;

	/* RF4_BR_WALL →BA_FORCE*/
	case 96+26:

		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを放った。", m_name);
#else
					msg_format("%^s fires something.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに気弾を放った。", m_name, t_name);
#else
					msg_format("%^s fires a Ki sphere at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

			if(r_ptr->flags2 & RF2_POWERFUL)
				dam = ((m_ptr->hp / 4) > 350 ? 350 : (m_ptr->hp / 4));
			else
				dam = ((m_ptr->hp / 6) > 250 ? 250 : (m_ptr->hp / 6));

		monst_breath_monst(m_idx, y, x, GF_FORCE,
				   dam, 2, FALSE, MS_BA_FORCE, learnable);

		break;



	/* RF4_BR_MANA */
	case 96+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに魔力のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes mana at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_MANA,
				   dam,0, TRUE, MS_BR_MANA, learnable);

		break;

	/* RF4_SPECIAL2 */
	case 96+28:
		{
			disturb(1, 1);


			if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
			{

				/*:::正邪隣接時　小槌攻撃*/
				if(distance(y, x, m_ptr->fy, m_ptr->fx)<2)
				{
					if (see_m)	msg_format(_("%^sは%sを金色の小槌でぶん殴った！",
                                            "%^s slams %s with a golden mallet!"), m_name,t_name);
					dam = 100 + randint0(100);
					monst_breath_monst(m_idx, y, x, GF_MISSILE, dam, 0, FALSE, 0, FALSE);
					earthquake_aux(y, x, 4, m_idx);
				}
				/*:::正邪非隣接時　位置交換*/
				else
				{
					if( tr_ptr->flagsr & RFR_RES_TELE || cave[y][x].info & CAVE_ICKY || cave[m_ptr->fy][m_ptr->fx].info & CAVE_ICKY || p_ptr->riding == t_idx)
					{
						if (see_m) msg_format(_("%^sは%sに何かをしようとしたが失敗したようだ。",
                                                "%^s tries to do something to %s, but fails."),m_name,t_name);
					}
					else
					{
						int x1=x,y1=y,x2 = m_ptr->fx,y2=m_ptr->fy;
						if (see_m)msg_format(_("%^sは%sと場所を交換した。",
                                                "%^s swaps places with %s."),m_name,t_name);
						/*:::モンスター同士の位置交換処理*/
						cave[y2][x2].m_idx = t_idx;
						cave[y1][x1].m_idx = m_idx;
						t_ptr->fy = y2;
						t_ptr->fx = x2;
						m_ptr->fy = y1;
						m_ptr->fx = x1;
						update_mon(m_idx,TRUE);
						update_mon(t_idx,TRUE);
						lite_spot(y1,x1);
						lite_spot(y2,x2);
						verify_panel();
					}
				}
			}
			else if(m_ptr->r_idx == MON_F_SCARLET)
			{
				if (!blind)
				{
					msg_format(_("%^sは%sを見つめて拳を握った。", "%^s stares at %s and closes her fist."), m_name,t_name);
				}
				monst_breath_monst(m_idx, y, x, GF_KYUTTOSHITEDOKA_N, 300, 0, FALSE, 0, FALSE);


			}
			//アザトート特殊攻撃　分解巨大レーザーか分解ボールを連発する
			else if(m_ptr->r_idx == MON_AZATHOTH)
			{
				int atkcount;
				if (!blind)
				{
					msg_format(_("%^sの巨大な偽足がダンジョンを縦横に貫いた！",
                                "%^s pierces the dungeon with a giant pseudopod!"), m_name);
					msg_print(NULL);
				}
				dam = 100 + randint1(100);
				for(atkcount = 6;atkcount>0;atkcount--)
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

			else
			{
				msg_format(_("ERROR: monst_spell_monst()において%^sの特別な行動2は定義されていない。",
                            "ERROR: Undefined special action 2 for %^s in monst_spell_monst()"), m_name);
			}

			break;
		}
	/* RF4_BR_NUKE */
	case 96+29:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに核熱のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes nuclear energy at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}
		dam = ((m_ptr->hp / 4) > 1000 ? 1000 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_NUKE,
				   dam,0, TRUE, MS_BR_NUKE, learnable);
		break;

	/* RF4_BA_CHAO */
	case 96+30:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが恐ろしげにつぶやいた。", m_name);
#else
					msg_format("%^s mumbles frighteningly.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに純ログルスを放った。", m_name, t_name);
#else
					msg_format("%^s invokes raw Logrus upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 2) + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_CHAOS,
				   dam, 4, FALSE, MS_BALL_CHAOS, learnable);

		break;

	/* RF4_BR_DISI */
	case 96+31:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かのブレスを吐いた。", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに分解のブレスを吐いた。", m_name, t_name);
#else
					msg_format("%^s breathes disintegration at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DISINTEGRATE,
				   dam,0, TRUE, MS_BR_DISI, learnable);
		break;

	/* RF5_BA_ACID */
	case 128+0:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに向かってアシッド・ボールの呪文を唱えた。", m_name, t_name);
#else
					msg_format("%^s casts an acid ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_ACID, dam, rad, FALSE, MS_BALL_ACID, learnable);
		break;

	/* RF5_BA_ELEC */
	case 128+1:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに向かってサンダー・ボールの呪文を唱えた。", m_name, t_name);
#else
					msg_format("%^s casts a lightning ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_ELEC, dam, rad, FALSE, MS_BALL_ELEC, learnable);
		break;

	/* RF5_BA_FIRE */
	case 128+2:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_ROLENTO)
				{
#ifdef JP
					if (blind)
						msg_format("%^sが何かを投げた。", m_name);
					else
						msg_format("%^sが%^sに向かって手榴弾を投げた。", m_name, t_name);
#else
					if (blind)
						msg_format("%^s throws something.", m_name);
					else
						msg_format("%^s throws a hand grenade.", m_name);
#endif
				}
				else
				{
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
						msg_format("%^sが%sに向かってファイア・ボールの呪文を唱えた。", m_name, t_name);
#else
						msg_format("%^s casts a fire ball at %s.", m_name, t_name);
#endif

					}
				}
			}
			else
			{
				mon_fight = TRUE;
			}
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
		monst_breath_monst(m_idx, y, x, GF_FIRE, dam, rad, FALSE, MS_BALL_FIRE, learnable);
		break;

	/* RF5_BA_COLD */
	case 128+3:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに向かってアイス・ボールの呪文を唱えた。", m_name, t_name);
#else
					msg_format("%^s casts a frost ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_COLD, dam, rad, FALSE, MS_BALL_COLD, learnable);
		break;

	/* RF5_BA_POIS */
	case 128+4:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに向かって悪臭雲の呪文を唱えた。", m_name, t_name);
#else
					msg_format("%^s casts a stinking cloud at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_breath_monst(m_idx, y, x, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);

		break;

	/* RF5_BA_NETH */
	case 128+5:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに向かって地獄球の呪文を唱えた。", m_name, t_name);
#else
					msg_format("%^s casts a nether ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
		monst_breath_monst(m_idx, y, x, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);

		break;

	/* RF5_BA_WATE */
	case 128+6:
		if (known)
		{
			if (see_either)
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
					msg_format("%^sが%sに対して流れるような身振りをした。", m_name, t_name);
#else
					msg_format("%^s gestures fluidly at %s.", m_name, t_name);
#endif

#ifdef JP
					msg_format("%^sは渦巻に飲み込まれた。", t_name);
#else
					msg_format("%^s is engulfed in a whirlpool.", t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
		monst_breath_monst(m_idx, y, x, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);

		break;

	/* RF5_BA_MANA */
	case 128+7:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに対して魔力の嵐の呪文を念じた。", m_name, t_name);
#else
					msg_format("%^s invokes a mana storm upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);

		break;

	/* RF5_BA_DARK */
	case 128+8:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに対して暗黒の嵐の呪文を念じた。", m_name, t_name);
#else
					msg_format("%^s invokes a darkness storm upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);

		break;

	/* RF5_DRAIN_MANA */
	case 128+9:
		if (see_m)
		{
			/* Basic message */
#ifdef JP
			msg_format("%^sは精神エネルギーを%sから吸いとった。", m_name, t_name);
#else
			msg_format("%^s draws psychic energy from %s.", m_name, t_name);
#endif

		}

		dam = ((randint1(rlev) / 2) + 1);
		monst_breath_monst(m_idx, y, x, GF_DRAIN_MANA, dam, 0, FALSE, MS_DRAIN_MANA, learnable);

		break;

	/* RF5_MIND_BLAST */
	case 128+10:
		if (see_m)
		{
#ifdef JP
			msg_format("%^sは%sをじっと睨んだ。", m_name, t_name);
#else
			msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

		}

		dam = damroll(7, 7);
		monst_breath_monst(m_idx, y, x, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);

		break;

	/* RF5_BRAIN_SMASH */
	case 128+11:
		if (see_m)
		{
#ifdef JP
			msg_format("%^sは%sをじっと睨んだ。", m_name, t_name);
#else
			msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

		}

		dam = damroll(12, 12);
		monst_breath_monst(m_idx, y, x, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);

		break;

	/* RF5_CAUSE_1 */
	case 128+12:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは%sを指さして呪いをかけた。", m_name, t_name);
#else
				msg_format("%^s points at %s and curses.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(3, 8);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);

		break;

	/* RF5_CAUSE_2 */
	case 128+13:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは%sを指さして恐ろしげに呪いをかけた。", m_name, t_name);
#else
				msg_format("%^s points at %s and curses horribly.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(8, 8);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);

		break;

	/* RF5_CAUSE_3 */
	case 128+14:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは%sを指さし、恐ろしげに呪文を唱えた！", m_name, t_name);
#else
				msg_format("%^s points at %s, incanting terribly!", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(10, 15);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);

		break;

	/* RF5_CAUSE_4 */
	case 128+15:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				///msg131221
				msg_format("%^sが%sに対して死の言霊を放った。", m_name, t_name);
#else
				msg_format("%^s invokes the Word of Death against %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(15, 15);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);

		break;

	/* RF5_BO_ACID */
	case 128+16:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%sが%sに向かってアシッド・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts an acid bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_ACID,
				 dam, MS_BOLT_ACID, learnable);

		break;

	/* RF5_BO_ELEC */
	case 128+17:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってサンダー・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a lightning bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_ELEC,
				 dam, MS_BOLT_ELEC, learnable);

		break;

	/* RF5_BO_FIRE */
	case 128+18:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってファイア・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a fire bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_FIRE,
				 dam, MS_BOLT_FIRE, learnable);

		break;

	/* RF5_BO_COLD */
	case 128+19:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってアイス・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a frost bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_COLD,
				 dam, MS_BOLT_COLD, learnable);

		break;

	/* RF5_BA_LITE */
	case 128+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^sが何かを力強くつぶやいた。", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sが%sに対して閃光の嵐の呪文を念じた。", m_name, t_name);
#else
					msg_format("%^s invokes a light storm upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);

		break;

	/* RF5_BO_NETH */
	case 128+21:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かって地獄の矢の呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a nether bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
		monst_bolt_monst(m_idx, y, x, GF_NETHER,
				 dam, MS_BOLT_NETHER, learnable);

		break;

	/* RF5_BO_WATE */
	case 128+22:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってウォーター・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a water bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_WATER,
				 dam, MS_BOLT_WATER, learnable);

		break;

	/* RF5_BO_MANA */
	case 128+23:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かって魔力の矢の呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a mana bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = randint1(rlev * 7 / 2) + 50;
		monst_bolt_monst(m_idx, y, x, GF_MANA,
				 dam, MS_BOLT_MANA, learnable);

		break;

	/* RF5_BO_PLAS */
	case 128+24:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってプラズマ・ボルトの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a plasma bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 10 + damroll(8, 7) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_PLASMA,
				 dam, MS_BOLT_PLASMA, learnable);

		break;

	/* RF5_BO_ICEE */
	case 128+25:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かって極寒の矢の呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts an ice bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_ICE,
				 dam, MS_BOLT_ICE, learnable);

		break;

	/* RF5_MISSILE */
	case 128+26:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sに向かってマジック・ミサイルの呪文を唱えた。", m_name, t_name);
#else
				msg_format("%^s casts a magic missile at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(2, 6) + (rlev / 3);
		monst_bolt_monst(m_idx, y, x, GF_MISSILE,
				 dam, MS_MAGIC_MISSILE, learnable);

		break;

	/* RF5_SCARE */
	case 128+27:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが恐ろしげな幻覚を作り出した。", m_name, t_name);
#else
				msg_format("%^s casts a fearful illusion in front of %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags3 & RF3_NO_FEAR)
		{
#ifdef JP
			if (see_t) msg_format("%^sは恐怖を感じない。", t_name);
#else
			if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^sは恐怖を感じない。", t_name);
#else
			if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

		}
		else
		{
			if (set_monster_monfear(t_idx, MON_MONFEAR(t_ptr) + randint0(4) + 4)) fear = TRUE;
		}

		wake_up = TRUE;

		break;

	/* RF5_BLIND */
	case 128+28:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%sは呪文を唱えて%sの目を焼き付かせた。", m_name, t_name);
#else
				msg_format("%^s casts a spell, burning %s%s eyes.", m_name, t_name,
					   (streq(t_name, "it") ? "s" : "'s"));
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Simulate blindness with confusion */
		if (tr_ptr->flags3 & RF3_NO_CONF)
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^sは目が見えなくなった！ ", t_name);
#else
			if (see_t) msg_format("%^s is blinded!", t_name);
#endif

			(void)set_monster_confused(t_idx, MON_CONFUSED(t_ptr) + 12 + randint0(4));
		}

		wake_up = TRUE;

		break;

	/* RF5_CONF */
	case 128+29:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sの前に幻惑的な幻をつくり出した。", m_name, t_name);
#else
				msg_format("%^s casts a mesmerizing illusion in front of %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags3 & RF3_NO_CONF)
		{
#ifdef JP
			if (see_t) msg_format("%^sは惑わされなかった。", t_name);
#else
			if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^sは惑わされなかった。", t_name);
#else
			if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^sは混乱したようだ。", t_name);
#else
			if (see_t) msg_format("%^s seems confused.", t_name);
#endif

			(void)set_monster_confused(t_idx, MON_CONFUSED(t_ptr) + 12 + randint0(4));
		}

		wake_up = TRUE;

		break;

	/* RF5_SLOW */
	case 128+30:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%sが%sの筋肉から力を吸いとった。", m_name, t_name);
#else
				msg_format("%^s drains power from %s%s muscles.", m_name, t_name,
					   (streq(t_name, "it") ? "s" : "'s"));
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags1 & RF1_UNIQUE)
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{

			//			if (set_monster_slow(t_idx, MON_SLOW(t_ptr) + 50))
			//v1.1.90 付与量調整
			if (set_monster_slow(t_idx, MON_SLOW(t_ptr) + 8+randint1(8)))
			{
#ifdef JP
				if (see_t) msg_format("%sの動きが遅くなった。", t_name);
#else
				if (see_t) msg_format("%^s starts moving slower.", t_name);
#endif
			}
		}

		wake_up = TRUE;

		break;

	/* RF5_HOLD */
	case 128+31:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sは%sをじっと見つめた。", m_name, t_name);
#else
				msg_format("%^s stares intently at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if ((tr_ptr->flags1 & RF1_UNIQUE) ||
		    (tr_ptr->flags3 & RF3_NO_STUN))
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^sは麻痺した！", t_name);
#else
			if (see_t) msg_format("%^s is paralyzed!", t_name);
#endif

			(void)set_monster_stunned(t_idx, MON_STUNNED(t_ptr) + randint1(4) + 4);
		}

		wake_up = TRUE;

		break;


	/* RF6_HASTE */
	case 160+0:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sが自分の体に念を送った。", m_name);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Allow quick speed increases to base+10 */
		if (set_monster_fast(m_idx, MON_FAST(m_ptr) + 100))
		{
#ifdef JP
			if (see_m) msg_format("%^sの動きが速くなった。", m_name);
#else
			if (see_m) msg_format("%^s starts moving faster.", m_name);
#endif
		}
		break;

	/* RF6_HAND_DOOM */
	case 160+1:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sが%sに<破滅の手>を放った！", m_name, t_name);
#else
				msg_format("%^s invokes the Hand of Doom upon %s!", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 20; /* Dummy power */
		monst_breath_monst(m_idx, y, x, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);

		break;

	/* RF6_HEAL */
	case 160+2:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは自分の傷に念を集中した。", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Heal some */
		m_ptr->hp += (rlev * 6);

		/* Fully healed */
		if (m_ptr->hp >= m_ptr->maxhp)
		{
			/* Fully healed */
			m_ptr->hp = m_ptr->maxhp;

			if (known)
			{
				if (see_m)
				{
#ifdef JP
					msg_format("%^sは完全に治った！", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
					mon_fight = TRUE;
				}
			}
		}

		/* Partially healed */
		else if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは体力を回復したようだ。", m_name);
#else
				msg_format("%^s looks healthier.", m_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Redraw (later) if needed */
		if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
		if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

		/* Cancel fear */
		if (MON_MONFEAR(m_ptr))
		{
			/* Cancel fear */
			(void)set_monster_monfear(m_idx, 0);

			/* Message */
#ifdef JP
			if (see_m) msg_format("%^sは勇気を取り戻した。", m_name);
#else
			if (see_m) msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
		}

		break;

	/* RF6_INVULNER */
	case 160+3:
		if (known)
		{
			if (see_m)
			{
				disturb(1, 1);
#ifdef JP
				if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^sの体が実体を失い暗く透き通った・・", m_name);
				else msg_format("%sは無傷の球の呪文を唱えた。", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (!MON_INVULNER(m_ptr)) (void)set_monster_invulner(m_idx, randint1(4) + 4, FALSE);
		break;

	/* RF6_BLINK */
	case 160+4:

		if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
		{
			if (see_m)
				msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappears from the floor."), m_name);

			delete_monster_idx(m_idx);
			return TRUE;

		}
		else if (teleport_barrier(m_idx))
		{
			if (see_m)
			{
#ifdef JP
			msg_format("%^sのテレポートは妨害された。", m_name);
			//msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("%^s is prevented from teleporting.", m_name);
#endif
			}
		}
		else
		{
			if (see_m)
			{
#ifdef JP
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^sが瞬時に消えた。", m_name);
#else
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^s blinks away.", m_name);
#endif
			}
			teleport_away(m_idx, 10, 0L);
		}
		break;

	/* RF6_TPORT */
	case 160+5:
		if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
		{
			if (see_m)
				msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappears from the floor."), m_name);

			delete_monster_idx(m_idx);
			return TRUE;

		}
		else if (teleport_barrier(m_idx))
		{
			if (see_m)
			{
#ifdef JP

				msg_format("%^sのテレポートは妨害された。", m_name);
			//msg_format("魔法のバリアが%^sのテレポートを邪魔した。", m_name);
#else
				msg_format("%^s is prevented from teleporting.", m_name);
#endif
			}
		}
		else
		{
			if (see_m)
			{
#ifdef JP
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^sがテレポートした。", m_name);
#else
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^s teleports away.", m_name);
#endif
			}
			teleport_away_followable(m_idx);
		}
		break;

	/* RF6_WORLD */
	case 160+6:
#if 0
		int who = 0;
		if(m_ptr->r_idx = MON_DIO) who == 1;
		else if(m_ptr->r_idx = MON_WONG) who == 3;
		dam = who;
		if(!process_the_world(randint1(2)+2, who, player_has_los_bold(m_ptr->fy, m_ptr->fx))) return (FALSE);
#endif
		return FALSE;

	/* RF6_SPECIAL */
	case 160+7:
		switch (m_ptr->r_idx)
		{
		case MON_OHMU:
			/* Moved to process_monster(), like multiplication */
			return FALSE;
		case MON_REIMU:
				if (see_m)
					msg_format(_("%^sは夢想封印を放った！",
                                "%^s uses Fantasy Seal!"), m_name);
				cast_musou_hu_in(m_idx);
			break;
		case MON_ROLENTO:
			if (known)
			{
				if (see_either)
				{
					disturb(1, 1);

#ifdef JP
					msg_format("%^sは手榴弾をばらまいた。", m_name);
#else
					msg_format("%^s throws some hand grenades.", m_name);
#endif
				}
				else
				{
					mon_fight = TRUE;
				}
			}

			{
				int num = 1 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, 0);
				}
			}

			if (known && !see_t && count)
			{
				mon_fight = TRUE;
			}
			break;
		case MON_MICHAEL:
#ifdef JP
				if (see_m)
				{
					disturb(1, 1);
					msg_format("%^s「燃え尽きろ！スピキュール！」", m_name);
				}
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1600);
#ifdef JP
				if (see_m)
				{
					if(one_in_(3))msg_format("%^s「この俺様は強ぇだろうが！」", m_name);
					else if(one_in_(2))msg_format("%^s「うおおおおおお！熱っちいいいいいぃ！」", m_name);
					else msg_format("%^s「骨まであっためてやるよ！」", m_name);
				}
#endif

				break;
			case MON_MOKOU:
				if (see_m) msg_format(_("%^sはフジヤマヴォルケイノを放った！",
                                        "%^s uses Fujiyama Volcano!"), m_name);
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1000);
				break;

			case MON_SHION_2:
				msg_format(_("%^sは不運と不幸をまき散らした！",
                            "%^s spreads misery and misfortune!"), m_name);
				project_hack3(m_idx, m_ptr->fy, m_ptr->fx, GF_DISENCHANT, 0, 0, 400);

				if (p_ptr->au > 0 && projectable(py,px,m_ptr->fy,m_ptr->fx) && !weird_luck())
				{
					p_ptr->au /= 2;
					msg_print(_("財布がすごく軽くなった気がする！",
                                "You feel your purse getting a lot lighter!"));
					p_ptr->redraw |= PR_GOLD;
				}

				break;

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
						{
							if (see_m)msg_format(_("%^sは何かをしようとしたが失敗したようだ。",
                                                "%^s tries to do something, but fails."),m_name);
						}
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
						count += summon_named_creature(m_idx, old_y, old_x, MON_SEIJA_D, 0L);
					}
				}
				break;


			case MON_YUKARI:
				{
					msg_format(_("%^sが空中に穴を開けると、その中から電車が突進してきた！",
                                "%^s opens up a gap in empty space, and a train comes out of it at full speed!"), m_name);
					monst_beam_monst(m_idx, y, x, GF_TRAIN, 100+randint1(50), 0, FALSE);

				}
				break;
			case MON_SUMIREKO:
				{
					msg_format(_("%^sは鉄塔を倒した！", "%^s drops a steel tower!"), m_name);
					(void)project(m_idx, 0, y, x, GF_TRAIN, 100+randint1(50), (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				}
				break;

				//v1.1.53　森林生成
			case MON_TSUCHINOKO:
				if(see_m)	msg_format(_("突然蔦が生い茂って%^sを覆った。",
                                    "Suddenly, vegetation completely overgrows %^s."), m_name);
				project(m_idx, 1, m_ptr->fy, m_ptr->fx, 1, GF_MAKE_TREE, (PROJECT_GRID), 0);

				break;

			case MON_CHIMATA:
				if (see_m)	msg_format(_("%^sは虹色の嵐を巻き起こした！",
                                        "%^s conjures a rainbow-colored storm!"), m_name);

				dam = (rlev * 4) + 50 + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_RAINBOW, dam, 4, FALSE, 0, FALSE);

				//(void)project(m_idx, 5, y, x, GF_RAINBOW, dam, (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);

				break;


		default:
			if (r_ptr->d_char == 'B')
			{
				//v1.1.62 メネルドールに荒馬馴らしで騎乗しているときにここに来て騎乗状態が変になったという報告あり。
				//p_ptr->ridingが0のまま騎乗しているということになるが、とりあえず抑止しておく
				if (one_in_(3) && !player_bold(m_ptr->fy,m_ptr->fx))
				{
					if (see_m)
					{
#ifdef JP
						msg_format("%^sは突然急上昇して視界から消えた!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
					}
					teleport_away(m_idx, 10, TELEPORT_NONMAGICAL);
					p_ptr->update |= (PU_MONSTERS);
				}
				else
				{
					if (known)
					{
						if (see_either)
						{
#ifdef JP
							msg_format("%^sが%sを掴んで空中から投げ落とした。", m_name, t_name);
#else
							msg_format("%^s holds %s, and drops from the sky.", m_name, t_name);
#endif

						}
						else
						{
							mon_fight = TRUE;
						}
					}

					dam = damroll(4, 8);

					if (t_idx == p_ptr->riding) teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					else teleport_monster_to(t_idx, m_ptr->fy, m_ptr->fx, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

					sound(SOUND_FALL);

					if (tr_ptr->flags7 & RF7_CAN_FLY)
					{
#ifdef JP
						if (see_t) msg_format("%^sは静かに着地した。", t_name);
#else
						if (see_t) msg_format("%^s floats gently down to the ground.", t_name);
#endif
					}
					else
					{
#ifdef JP
						if (see_t) msg_format("%^sは地面に叩きつけられた。", t_name);
#else
						if (see_t) msg_format("%^s crashed into the ground.", t_name);
#endif
						dam += damroll(6, 8);
					}

					if (p_ptr->riding == t_idx)
					{
						int get_damage = 0;

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
					}

					mon_take_hit_mon(t_idx, dam, &fear, extract_note_dies(real_r_ptr(t_ptr)), m_idx);
				}
				break;
			}

			/* Something is wrong */
			else return FALSE;
		}

		/* done */
		break;

	/* RF6_TELE_TO */
	case 160+8:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sを引き戻した。", m_name, t_name);
#else
				msg_format("%^s commands %s to return.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flagsr & RFR_RES_TELE)
		{
			if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^sには効果がなかった。", t_name);
#else
					msg_format("%^s is unaffected!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
			else if (tr_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^sは耐性を持っている！", t_name);
#else
					msg_format("%^s resists!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
		}

		if (!resists_tele)
		{
			if (t_idx == p_ptr->riding) teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			else teleport_monster_to(t_idx, m_ptr->fy, m_ptr->fx, 100, TELEPORT_PASSIVE);
		}

		wake_up = TRUE;
		break;

	/* RF6_TELE_AWAY */
	case 160+9:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sは%sをテレポートさせた。", m_name, t_name);
#else
				msg_format("%^s teleports %s away.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flagsr & RFR_RES_TELE || teleport_barrier(t_idx))
		{
			if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^sには効果がなかった。", t_name);
#else
					msg_format("%^s is unaffected!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
			else if (tr_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^sは耐性を持っている！", t_name);
#else
					msg_format("%^s resists!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
		}

		if (!resists_tele)
		{
			if (t_idx == p_ptr->riding) teleport_player_away(m_idx, MAX_SIGHT * 2 + 5);

			else if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(tr_ptr->flags1 & RF1_QUESTOR))
			{
				if (see_either)
					msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappears from the floor."), t_name);

				delete_monster_idx(t_idx);
				return TRUE;

			}

			else teleport_away(t_idx, MAX_SIGHT * 2 + 5, TELEPORT_PASSIVE);
		}

		wake_up = TRUE;
		break;

	/* RF6_TELE_LEVEL */
	case 160+10:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^sが%sの足を指さした。", m_name, t_name);
#else
				msg_format("%^s gestures at %s's feet.", m_name, t_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}


		if (tr_ptr->flagsr & (RFR_EFF_RES_NEXU_MASK | RFR_RES_TELE))
		{
#ifdef JP
			if (see_t) msg_format("%^sには効果がなかった。", t_name);
#else
			if (see_t) msg_format("%^s is unaffected!", t_name);
#endif
		}


		else if ((tr_ptr->flags1 & RF1_QUESTOR) ||
			    (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10))
		{
#ifdef JP
			if (see_t) msg_format("%^sは効力を跳ね返した！", t_name);
#else
			if (see_t) msg_format("%^s resist the effects!", t_name);
#endif
		}
		else teleport_level((t_idx == p_ptr->riding) ? 0 : t_idx);

		wake_up = TRUE;
		break;

	/* RF6_PSY_SPEAR */
	case 160+11:
		if (known)
		{
			if (see_either)
			{
				if(m_ptr->r_idx == MON_REMY) msg_format(_("%^sが%sに向かってグングニルを投げつけた。",
                                                        "%^s throws Gungnir at %s."), m_name, t_name);

				else msg_format(_("%^sが%sに向かって光の剣を放った。",
                                    "%^s throws a Psycho-Spear at %s."), m_name, t_name);

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 180) : (randint1(rlev * 3 / 2) + 120);
		if(m_ptr->r_idx == MON_REMY) monst_beam_monst(m_idx, y, x, GF_GUNGNIR, dam, MS_PSY_SPEAR, learnable);
		else  monst_beam_monst(m_idx, y, x,GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
		break;

	/* RF6_DARKNESS */
	case 160+12:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				if (can_use_lite_area) msg_format("%^sが辺りを明るく照らした。", m_name);
				else msg_format("%^sが暗闇の中で手を振った。", m_name);
#else
				if (can_use_lite_area) msg_format("%^s cast a spell to light up.", m_name);
				else msg_format("%^s gestures in shadow.", m_name);
#endif

				if (see_t)
				{
#ifdef JP
					if (can_use_lite_area) msg_format("%^sは白い光に包まれた。", t_name);
					else msg_format("%^sは暗闇に包まれた。", t_name);
#else
					if (can_use_lite_area) msg_format("%^s is surrounded by a white light.", t_name);
					else msg_format("%^s is surrounded by darkness.", t_name);
#endif
				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (can_use_lite_area)
		{
			(void)project(m_idx, 3, y, x, 0, GF_LITE_WEAK, PROJECT_GRID | PROJECT_KILL, -1);
			lite_room(y, x);
		}
		else
		{
			(void)project(m_idx, 3, y, x, 0, GF_DARK_WEAK, PROJECT_GRID | PROJECT_KILL, MS_DARKNESS);
			unlite_room(y, x);
		}

		break;

	/* RF6_TRAPS */
	case 160+13:
#if 0
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sが呪文を唱えて邪悪に微笑んだ。", m_name);
#else
				msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^sが何かをつぶやいた。", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif
			}
		}

		trap_creation(y, x);

		break;
#else
		/* Not implemented */
		return FALSE;
#endif

	/* RF6_FORGET */
	case 160+14:
		/* Not implemented */
		return FALSE;

	/* RF6_RAISE_DEAD */
	case 160+15:
			if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^sは%sの精神を名状しがたき宇宙的恐怖へと陥れた。", m_name, t_name);
#else
				msg_format("%^s opens mind %s to indescribable cosmic horrors.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}
		dam = (rlev * 4) + 50 + damroll(10, 10);

		monst_breath_monst(m_idx, y, x, GF_COSMIC_HORROR, dam, 0, FALSE, MS_COSMIC_HORROR, learnable);

		break;

	/* RF6_S_KIN */
	case 160+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				//if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
				if (m_ptr->r_idx == MON_SERPENT)
				{
#ifdef JP
					msg_format("%^sが混沌の強力な存在を召喚した。", m_name);
#else
					msg_format("%^s magically summons powerful chaotic beings.", m_name);
#endif
				}
				else if(m_ptr->r_idx == MON_ALICE)
				{
					msg_format(_("%^sが人形達を呼び出した。", "%^s calls forth her dolls."), m_name);
				}
				else if(m_ptr->r_idx == MON_SUIKA)
				{
					msg_format(_("%^sが小さな分身を呼び出した。", "%^s calls forth tiny clones."), m_name);
				}
				else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
				{
					msg_format(_("%^sは手下と仲間を呼んだ！", "%^s calls for subordinates and allies!"), m_name);
				}
				else if(m_ptr->r_idx == MON_YUKARI)
				{
					msg_format(_("%^sが式を呼び出した。", "%^s calls forth her shikigami."), m_name);
				}
				else if(m_ptr->r_idx == MON_NAPPA)
				{
					msg_format(_("%^sは地面に何かを植えた・・", "%^s plants something in the ground..."), m_name);
				}
				else if (m_ptr->r_idx == MON_MAYUMI || m_ptr->r_idx == MON_KEIKI)
				{
					msg_format(_("%^sは配下の埴輪達を呼び出した。", "%^s calls forth her haniwa followers."), m_name);
				}

				else
				{
#ifdef JP
					msg_format("%sが魔法で%sを召喚した。", m_name,
						   ((r_ptr->flags1 & RF1_UNIQUE) ? "手下" : "仲間"));
#else
					msg_format("%^s magically summons %s %s.", m_name, m_poss,
						   ((r_ptr->flags1 & RF1_UNIQUE) ? "minions" : "kin"));
#endif
				}

			}
			else
			{
				mon_fight = TRUE;
			}
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

		case MON_BULLGATES:
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_IE, 0);
				}
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


		case MON_SERPENT:
	//	case MON_ZOMBI_SERPENT:
			/*
			if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6))
			{
				if (known && see_t)
				{
#ifdef JP
					msg_print("地面から水が吹き出した！");
#else
					msg_print("Water blew off from the ground!");
#endif
				}
				project(t_idx, 8, y, x, 3, GF_WATER_FLOW, PROJECT_GRID | PROJECT_HIDE, -1);
			}

			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			*/
			count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

			break;

		case MON_CALDARM:
			{
				int num = randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_LOCKE_CLONE, 0);
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

		case MON_ORIN:
			{
				if(one_in_(2))
				{
					num = 1 + randint1(2);
					for (k = 0; k < num; k++)
						count += summon_named_creature(m_idx, y, x, MON_ZOMBIE_FAIRY, 0);
				}
				else
				{
					count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					if(one_in_(2))count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					count += summon_named_creature(m_idx, y, x, MON_ONRYOU, 0);

				}

			}
			break;
		case MON_LOUSY:
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, (PM_ALLOW_GROUP));
				}
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
					count += summon_named_creature(m_idx, y, x, MON_CHAOS_BLADE, 0);
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
					count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_SAIBAIMAN, 0);
				}
			}
			break;
		case MON_MAYUMI:
		case MON_KEIKI:
		{
			int num = 4 + randint1(3);
			u32b s_mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);

			if (is_hostile(m_ptr)) s_mode |= PM_FORCE_ENEMY;

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HANIWA, s_mode);
			}
		}
		break;


		default:
			summon_kin_type = r_ptr->d_char;

			for (k = 0; k < 4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, (PM_ALLOW_GROUP));
			}
			break;
		}

		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}

		break;

	/* RF6_S_CYBER */
	case 160+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sがサイバーデーモンを召喚した！", m_name);
#else
				msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (is_friendly(m_ptr))
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_CYBER, (PM_ALLOW_GROUP));
		}
		else
		{
			count += summon_cyber(m_idx, y, x);
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_MONSTER */
	case 160+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法で仲間を召喚した！", m_name);
#else
				msg_format("%^s magically summons help!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		count += summon_specific(m_idx, y, x, rlev, 0, (u_mode));

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_MONSTERS */
	case 160+19:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法でモンスターを召喚した！", m_name);
#else
				msg_format("%^s magically summons monsters!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_ANT */
	case 160+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法でアリを召喚した。", m_name);
#else
				msg_format("%^s magically summons ants.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_SPIDER */
	case 160+21:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法で虫を召喚した。", m_name);
#else
				msg_format("%^s magically summons insects.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_HOUND */
	case 160+22:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法でハウンドを召喚した。", m_name);
#else
				msg_format("%^s magically summons hounds.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_HYDRA */
	case 160+23:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法でヒドラを召喚した。", m_name);
#else
				msg_format("%^s magically summons hydras.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_ANGEL */
	case 160+24:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法で天使を召喚した！", m_name);
#else
				msg_format("%^s magically summons an angel!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		{
			int num = 1;

			if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
			{
				num += r_ptr->level/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, (PM_ALLOW_GROUP));
			}
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_DEMON */
	case 160+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				///msg131221
				msg_format("%^sが魔法でデーモンを召喚した！", m_name);
#else
				msg_format("%^s magically summons a demon!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_UNDEAD */
	case 160+26:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%sが魔法でアンデッドを召喚した。", m_name);
#else
				msg_format("%^s magically summons undead.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_DRAGON */
	case 160+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法でドラゴンを召喚した！", m_name);
#else
				msg_format("%^s magically summons a dragon!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_HI_UNDEAD */
	case 160+28:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%sが魔法でアンデッドを召喚した。", m_name);
#else
				msg_format("%^s magically summons undead.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_HI_DRAGON */
	case 160+29:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法で古代ドラゴンを召喚した！", m_name);
#else
				msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_AMBERITES */
	case 160+30:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sがアンバーの王族を召喚した！", m_name);
#else
				msg_format("%^s magically summons Lords of Amber!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		break;

	/* RF6_S_UNIQUE */
	case 160+31:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^sが魔法で特別な強敵を召喚した！", m_name);
#else
				msg_format("%^s magically summons special opponents!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
		}

		{
			int non_unique_type = SUMMON_HI_UNDEAD;

			if ((m_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
		}
		if(see_either && !count) msg_print(_("しかし何も現れなかったようだ。", "However, nobody arrives."));
		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		break;


		//ここから新スペル


		/* RF9_FIRE_STORM */
		case 192+0:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって炎の嵐の呪文を唱えた。", "%^s casts a fire storm at %s."), m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_ICE_STORM */
		case 192+1:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって氷の嵐の呪文を唱えた。", "%^s casts a frost storm at %s."), m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_COLD, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_THUNDER_STORM */
		case 192+2:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって雷の嵐の呪文を唱えた。", "%^s casts a lightning storm at %s."), m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_ELEC, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_ACID_STORM */
		case 192+3:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって酸の嵐の呪文を唱えた。", "%^s casts an acid storm at %s."), m_name,t_name);
		}
		rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_ACID, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_TOXIC_STORM */
		case 192+4:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって毒素の嵐の呪文を唱えた。", "%^s casts a toxic storm at %s."), m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 8) + 50 + randint1(50);
			else	dam = (rlev * 5) + 25 + randint1(25);
			monst_breath_monst(m_idx, y, x, GF_POIS, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_BA_POLLUTE */
		case 192+5:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か奇怪な音を立てた。", "%^s makes strange noises."), m_name);
			else msg_format(_("%^sが%sに向かって汚染の球の呪文を唱えた。", "%^s casts a pollution ball at %s."), m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 50 + damroll(10, 10);
			else	dam = (rlev * 3) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_POLLUTE, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_BA_DISI */
		case 192+6:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format(_("%^sはどこからか巨大な爆弾を取り出した！",
                                                                                            "%^s takes out a huge bomb!"), m_name);
			else if (blind) msg_format(_("%^sが何か何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって原子分解の呪文を唱えた。", "%^s casts a disintegration spell at %s."), m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 2) + 50 + randint1(50);
			else	dam = rlev + 25 + randint1(25);
			monst_breath_monst(m_idx, y, x, GF_DISINTEGRATE, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_BA_HOLY */
		case 192+7:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何かを詠唱した。", "%^s chants something."), m_name);
			else msg_format(_("%^sが%sに向かって破邪の光球の呪文を唱えた。", "%^s casts a ball of holy energy at %s."), m_name,t_name);
		}
			rad = 2;
			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_METEOR */
		case 192+8:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かってメテオストライクの呪文を唱えた！", "%^s invokes a meteor strike against %s!"), m_name,t_name);
		}
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
			monst_breath_monst(m_idx, y, x, GF_METEOR, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_BA_DISTORTION */
		case 192+9:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か複雑な呪文を唱えた。", "%^s casts some complicated spell."), m_name);
			else msg_format(_("%^sが%sに向かって空間歪曲の呪文を唱えた！", "%^s invokes a spacial distortion against %s!"), m_name,t_name);
		}
			rad = 4;
			dam = 50 + damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 1 : 2));
			monst_breath_monst(m_idx, y, x, GF_DISTORTION, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_1 */
		case 192+10:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format(_("%^sは%sに向かって魔除けのまじないを唱えた。",
                                "%^s casts a warding charm at %s."), m_name,t_name);
			rad = 0;
		}
		dam = damroll(3, 8);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_1, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_2 */
		case 192+11:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format(_("%^sは%sに向かって聖なる言葉を唱えた。",
                                "%^s chants holy words at %s."), m_name,t_name);
		}
			rad = 0;
			dam = damroll(8, 8);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_2, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_3 */
		case 192+12:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format(_("%^sは%sに向かって退魔の呪文を唱えた。",
                                "%^s casts a spell of exorcism at %s."), m_name,t_name);
			rad = 0;
		}
			dam = damroll(10, 15);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_3, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_4 */
		case 192+13:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format(_("%^sは%sに向かって破邪の印を放った！",
                                "%^s fires an exorcism sigil at %s!"), m_name,t_name);
		}
			rad = 0;
			dam = damroll(15, 15);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_4, dam, rad, FALSE, 0, FALSE);
			break;
	/* RF9_BO_HOLY */
	case 192+14:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが%sに向かって聖なる矢の呪文を唱えた。",
                                "%^s casts a holy bolt at %s."), m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_HOLY_FIRE, dam, 0, FALSE);

		break;
	/* RF9_GIGANTIC_LASER */
	case 192+15:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				if(m_ptr->r_idx == MON_MARISA)
					msg_format(_("%^sが%sに向かってマスタースパークを放った！",
                                    "%^s fires a Master Spark at %s!"), m_name, t_name);
				else
					msg_format(_("%^sが%sに向かって巨大なレーザーを放った！",
                                    "%^s fires a huge laser at %s!"), m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		dam = damroll(10, 10) +  rlev * (((r_ptr->flags2 & RF2_POWERFUL) ? 8 : 4));
		masterspark(y, x, m_idx, GF_NUKE, dam, 0, FALSE,((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));

		break;
	/* RF9_BO_SOUND */
	case 192+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが%sに向かって音符の弾を放った。",
                                "%^s fires a musical note bullet at %s."), m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		if (r_ptr->flags2 & RF2_POWERFUL)	dam = damroll(10, 10) + (rlev * 3 / 2);
		else	dam = damroll(3, 10) + rlev;
		monst_bolt_monst(m_idx, y, x, GF_SOUND, dam, 0, FALSE);
		break;

	/* RF6_S_ANIMAL */
	case 192+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが動物を召喚した。", "%^s summons animals."), m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_ANIMAL, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
		break;
	/* RF9_BEAM_LITE */
	case 192+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが%sに向かってレーザーを放った。",
                            "%^s fires a laser at %s."), m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		if(m_ptr->r_idx == MON_BANKI_HEAD_2)
		{
			monst_beam_monst(m_idx, y, x, GF_BANKI_BEAM, rlev, 0, FALSE);
		}
		else
		{
			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 4) : (rlev * 2);
			monst_beam_monst(m_idx, y, x, GF_LITE, dam, 0, FALSE);
		}
		break;
	/* RF9_HELLFIRE */
	case 192+20:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か禍々しい呪文を唱えた。", "%^s casts some ominous spell."), m_name);
			else msg_format(_("%^sが%sに向かってヘル・ファイアの呪文を唱えた。",
                                "%^s invokes hellfire against %s."), m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);

			monst_breath_monst(m_idx, y, x, GF_HELL_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

	/* RF9_HOLYFIRE */
	case 192+21:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("%^sが何か神々しい呪文を唱えた。", "%^s casts a divinely spell."), m_name);
			else msg_format(_("%^sが%sに向かってホーリー・ファイアの呪文を唱えた。",
                                "%^s invokes holy fire against %s."), m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);

			monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_FINALSPARK*/
		case 192+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^sが強烈なエネルギーを放射した！", m_name);
#else
			if (blind) msg_format("%^s releases intense energy!", m_name);
#endif
            else msg_format(_("%^sが%sへ向けてファイナルスパークを放った！",
                                "%^s fires a Final Spark at %s!"), m_name, t_name);

			dam = 200 + randint1(rlev) +  rlev * 2;
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,2);
			/*:::Hack - ファイナルスパーク発射後は＠が行動するまで魔法を使わない*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;
			break;
		}


	/* RF9_TORNADO */
	case 192+23:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format(_("突然突風が吹き荒れた。", "Suddenly an intense storm is blowing."), m_name);
			else msg_format(_("%^sが%sのいるところに竜巻を起こした。",
                                "%^s creates a tornado on top of %s."), m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 3) + 50 + randint1(50);
			else	dam = (rlev * 2) + 25 + randint1(25);

			monst_breath_monst(m_idx, y, x, GF_TORNADO, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_DESTRUCTION */
	case 192+24:
		{

			disturb(1, 1);
			if(!known) msg_format(_("巨大な爆発音が響き、一瞬ダンジョンが揺れた。",
                                    "A powerful explosion sounds, and the dungeon trembles momentarily."));
			else if (blind) msg_format(_("%^sが何かを唱えると地響きがした…", "%^s casts something, and there is a rumbling sound..."), m_name);
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
					//v1.1.86 NOESCAPE→ATTACKに
					if(p_ptr->pclass != CLASS_CLOWNPIECE)
						take_hit(DAMAGE_ATTACK, dam, m_name, -1);
					if(p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
					{
						msg_format(_("あなたは吹き飛ばされた！", "You get blown away!"), m_name);
						teleport_player_away(m_idx, 100);
					}
				}
			}

			break;
		}

	/* RF6_S_DEITY */
	case 192+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが神格を召喚した。", "%^s summons deities."), m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DEITY, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
		break;

	/* RF6_S_HI_DEMON */
	case 192+26:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが上位悪魔を召喚した！", "%^s summons greater demons!"), m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
		break;

	/* RF6_S_KWAI */
	case 192+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format(_("%^sが妖怪を召喚した！", "%^s summons youkai!"), m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_KWAI, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print(_("しかし何も現れなかった。", "However, nobody arrives."));
		break;

		///pend とりあえず現状モンスター同士の視界外隣接テレポは視界内でしか飛ばない
		/* RF9_TELE_APPROACH */
	case 192+28:
		/* RF9_TELE_HI_APPROACH */
	case 192+29:
		{
			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (see_m)
					msg_format(_("%^sは公転周期のトラップにかかってこのフロアから消えた。",
                                "%^s triggers your orbital trap and disappear from the floor."), m_name);

				delete_monster_idx(m_idx);
				return TRUE;
			}

			if (teleport_barrier(m_idx)) break;
			disturb(1, 1);
			teleport_monster_to(m_idx, y, x, 100, 0L);
			if (see_either)
				msg_format(_("%sが瞬時に%sの近くに出現した。",
                            "%s teleports towards %s."), m_name,t_name);
			break;
		}
	//RF9_MAELSTROM 闘技場以外で使わない
	case 192+30:
		{
			int rad;
			disturb(1, 1);
			if(see_either) msg_format(_("%^sがメイルシュトロムの呪文を詠唱した。",
                                        "%^s casts a maelstrom."), m_name);

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
			(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), FALSE);
			(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, rad, GF_WATER_FLOW, (PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP | PROJECT_HIDE), FALSE);

			break;
		}


	/* RF9_ALARM */
	case 192+31:
		if (known)
		{

			if (see_m)
			{
				if(m_ptr->r_idx == MON_CLOWNPIECE)
					msg_format(_("%^sの持つ松明が煌々と輝いた！", "The torch %^s is holding starts to glitter!"), m_name);
				else
					msg_format(_("%^sは強烈な警報音を発した！", "%^s sounds a loud alarm!"), m_name);
			}
			else
			{
				msg_format(_("甲高い警報音が聞こえた！", "You hear high-pitched alarm sounds!"), m_name);
			}
		}

		wake_up = TRUE;

		break;



	default:
		msg_format(_("ERROR:monst_spell_monst()にてスペル%dの処理が未記述である",
                    "ERROR: Undefined logic for spell %d in monst_spell_monst()"),thrown_spell);


	} //モンスター魔法処理のswitch()文終了

	if (wake_up) (void)set_monster_csleep(t_idx, 0);

	if (fear && see_t)
	{
#ifdef JP
		msg_format("%^sは恐怖して逃げ出した！", t_name);
#else
		msg_format("%^s flees in terror!", t_name);
#endif
	}


	//v1.1.82 新職業「弾幕研究家」の特技習得処理
	if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		//この時点で特技使用モンスターがいない場合、処理が面倒になるので習得不可
		if (m_ptr->r_idx)
		{
			learn_monspell_new(thrown_spell, m_ptr, FALSE, learnable,caster_dist);

		}

	}
	if (m_ptr->ml && maneable && !world_monster && !p_ptr->blind && (p_ptr->pclass == CLASS_IMITATOR))
	{
		if (thrown_spell != 167) /* Not RF6_SPECIAL */
		{
			if (p_ptr->mane_num == MAX_MANE)
			{
				p_ptr->mane_num--;
				for (i = 0; i < p_ptr->mane_num - 1; i++)
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

	if(maneable) kyouko_echo(FALSE,thrown_spell);


	/* Remember what the monster did, if we saw it */
	///sys f9フラグ追加
	if (can_remember)
	{
		/* Inate spell */
		if (thrown_spell < 32*4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32*3));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32*5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32*4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32*6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32*5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
		else if (thrown_spell < 32 * 7)
		{
			r_ptr->r_flags9 |= (1L << (thrown_spell - 32 * 6));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
	}

	/* Always take note of monsters that kill you */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}

	if(suisidebomb && !p_ptr->is_dead) delete_monster_idx(m_idx);

	/* A spell was cast */
	return TRUE;
}
