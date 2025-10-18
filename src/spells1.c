/* File: spells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell projection */

#include "angband.h"

/* ToDo: Make this global */
/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16


static int rakubadam_m;
static int rakubadam_p;

/*:::project()の射程距離をデフォルト値以外にしたい場合は呼ぶ前にここに値を入れておく*/
/*:::-1にしたら射程制限がなくなるらしい*/
int project_length = 0;

/*
 * Get another mirror. for SEEKER
 */
static void next_mirror( int* next_y , int* next_x , int cury, int curx)
{
	int mirror_x[10],mirror_y[10]; /* 鏡はもっと少ない */
	int mirror_num=0;              /* 鏡の数 */
	int x,y;
	int num;

	for( x=0 ; x < cur_wid ; x++ )
	{
		for( y=0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x])){
				mirror_y[mirror_num]=y;
				mirror_x[mirror_num]=x;
				mirror_num++;
			}
		}
	}
	if( mirror_num )
	{
		num=randint0(mirror_num);
		*next_y=mirror_y[num];
		*next_x=mirror_x[num];
		return;
	}
	*next_y=cury+randint0(5)-2;
	*next_x=curx+randint0(5)-2;
	return;
}

/*
 * Get a legal "multi-hued" color for drawing "spells"
 */
static byte mh_attr(int max)
{
	switch (randint1(max))
	{
		case  1: return (TERM_RED);
		case  2: return (TERM_GREEN);
		case  3: return (TERM_BLUE);
		case  4: return (TERM_YELLOW);
		case  5: return (TERM_ORANGE);
		case  6: return (TERM_VIOLET);
		case  7: return (TERM_L_RED);
		case  8: return (TERM_L_GREEN);
		case  9: return (TERM_L_BLUE);
		case 10: return (TERM_UMBER);
		case 11: return (TERM_L_UMBER);
		case 12: return (TERM_SLATE);
		case 13: return (TERM_WHITE);
		case 14: return (TERM_L_WHITE);
		case 15: return (TERM_L_DARK);
	}

	return (TERM_WHITE);
}


/*
 * Return a color to use for the bolt/ball spells
 */
/*:::属性により色を決める　戻り値はmisc_to_attr[]とmisc_to_char[]へのシフト量*/
byte spell_color(int type)
{
	/* Check if A.B.'s new graphics should be used (rr9) */
	if (streq(ANGBAND_GRAF, "new"))
	{
		/* Analyze */
		switch (type)
		{
			case GF_PSY_SPEAR:      return (0x06);
			case GF_MISSILE:        return (0x0F);
			case GF_ACID:           return (0x04);
			case GF_ELEC:           return (0x02);
			case GF_FIRE:           return (0x00);
			case GF_COLD:           return (0x01);
			case GF_DISP_LIVING:
			case GF_POIS:           return (0x03);
			case GF_HOLY_FIRE:      return (0x02);
			case GF_HELL_FIRE:      return (0x00);
			case GF_MANA:           return (0x0E);
			  /* by henkma */
			case GF_SEEKER:         return (0x0E);
			case GF_SUPER_RAY:      return (0x0E);

			case GF_ARROW:          return (0x0F);
			case GF_WATER:
			case GF_HOLY_WATER:
			case GF_MOSES:
			case GF_SPECIAL_SHOT:
				return (0x04);
			case GF_NETHER:         return (0x07);
			case GF_CHAOS:          return (mh_attr(15));
			case GF_DISENCHANT:     return (0x0F);
			case GF_NEXUS:          return (0x0C);
			case GF_CONFUSION:      return (mh_attr(4));
			case GF_SOUND:          return (0x09);
			case GF_SHARDS:         return (0x08);
			case GF_TIMED_SHARD:         return (0x08);

			case GF_FORCE:          return (0x09);
			case GF_INACT:        return (0x09);
			case GF_GRAVITY:        return (0x09);
			case GF_TIME:           return (0x09);
			case GF_LITE_WEAK:      return (0x06);
			case GF_LITE:           return (0x06);
			case GF_DARK_WEAK:      return (0x07);
			case GF_DARK:           return (0x07);
			case GF_PLASMA:         return (0x0B);
			case GF_METEOR:         return (0x00);
			case GF_KANAMEISHI:         return (0x00);
			case GF_ICE:            return (0x01);
			case GF_ROCKET:         return (0x0F);
			case GF_DEATH_RAY:      return (0x07);
			case GF_NUKE:           return (mh_attr(2));
			case GF_DISINTEGRATE:   return (0x05);
			case GF_PSI:
			case GF_PSI_DRAIN:
			case GF_TELEKINESIS:
			case GF_DOMINATION:
			case GF_DRAIN_MANA:
			case GF_MIND_BLAST:
			case GF_BRAIN_SMASH:
						return (0x09);
			case GF_CAUSE_1:
			case GF_CAUSE_2:
			case GF_CAUSE_3:
			case GF_CAUSE_4:        return (0x0E);
			case GF_HAND_DOOM:
			case GF_KYUTTOSHITEDOKA_N:
				return (0x07);
			case GF_CAPTURE  :      return (0x0E);
			case GF_IDENTIFY:       return (0x01);
			case GF_ATTACK:        return (0x0F);
			case GF_BANKI_BEAM:
			case GF_BANKI_BEAM2:
			case GF_PHOTO   :      return (0x06);
				///mod131229 ボルトの軌跡の色　とりあえず適当に
			case GF_WINDCUTTER:
			case GF_TRAIN:
				return (0x0D);
			case GF_REDEYE:
			case GF_SOULSCULPTURE:
			case GF_YOUMU:
			case GF_SATORI:
			case GF_POSSESSION:
			case GF_REDMAGIC:
			case GF_GUNGNIR:
			case GF_BLAZINGSTAR:
			case GF_PURIFY:
				return (0x00);
			case GF_RYUURI:
			case GF_SEIRAN_BEAM:
				return (0x02);
			case GF_OLD_SLEEP:
			case GF_NORTHERN:
				return (0x02);
			case GF_HOUTOU:
				return (0x09);
			case GF_YAKU:
				return (0x04);
			case GF_STEAM:
				return (0x01);
			default:return (0x01);

		}
	}
	/* Normal tiles or ASCII */
	else
	{
		byte a;
		char c;

		/* Lookup the default colors for this type */
		cptr s = quark_str(gf_color[type]);

		/* Oops */
		if (!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = my_strchr(color_char, c) - color_char;

		/* Invalid color (note check for < 0 removed, gave a silly
		 * warning because bytes are always >= 0 -- RG) */
		if (a > 15) return (TERM_WHITE);

		/* Use this color */
		return (a);
	}

	/* Standard "color" */
	return (TERM_WHITE);
}


/*
 * Find the attr/char pair to use for a spell effect
 *
 * It is moving (or has moved) from (x,y) to (nx,ny).
 *
 * If the distance is not "one", we (may) return "*".
 */
/*:::ボルトが飛んでいくときのエフェクトに使う文字と色を算出
 *:::返り値は16bitで、上位8bitが色、下位8bitが文字
 */
u16b bolt_pict(int y, int x, int ny, int nx, int typ)
{
	int base;

	byte k;

	byte a;
	char c;

	/* No motion (*) */
	if ((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if (nx == x) base = 0x40;

	/* Horizontal (-) */
	else if (ny == y) base = 0x50;

	/* Diagonal (/) */
	else if ((ny - y) == (x - nx)) base = 0x60;

	/* Diagonal (\) */
	else if ((ny - y) == (nx - x)) base = 0x70;

	/* Weird (*) */
	/*:::ビームならここ*/
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	a = misc_to_attr[base + k];
	c = misc_to_char[base + k];

	/* Create pict */
	return (PICT(a, c));
}


/*
 * Determine the path taken by a projection.
 *:::ボールやビームなどの経路を特定する。
 *
 * The projection will always start from the grid (y1,x1), and will travel
 * towards the grid (y2,x2), touching one grid per unit of distance along
 * the major axis, and stopping when it enters the destination grid or a
 * wall grid, or has travelled the maximum legal distance of "range".
 *:::(y1,x1)から(y2,x2)までの直線を決め、それに沿ってグリッドを選出する。壁か射程範囲で止まる。
 *
 * Note that "distance" in this function (as in the "update_view()" code)
 * is defined as "MAX(dy,dx) + MIN(dy,dx)/2", which means that the player
 * actually has an "octagon of projection" not a "circle of projection".
 *:::射程距離は上記。結果、射程範囲は使用者から八角形になる。
 *
 * The path grids are saved into the grid array pointed to by "gp", and
 * there should be room for at least "range" grids in "gp".  Note that
 * due to the way in which distance is calculated, this function normally
 * uses fewer than "range" grids for the projection path, so the result
 * of this function should never be compared directly to "range".  Note
 * that the initial grid (y1,x1) is never saved into the grid array, not
 * even if the initial grid is also the final grid.  XXX XXX XXX
 *:::グリッドはgp[]に保存される。gp[]は射程範囲に十分な長さがあること。
 *:::経路はしばしば途中で止まるのでgp[]はrangeまで長くならない事に注意（？）。
 *:::gp[]に開始グリッド(x1,y1)は入らない。
 *
 * The "flg" flags can be used to modify the behavior of this function.
 *
 * In particular, the "PROJECT_STOP" and "PROJECT_THRU" flags have the same
 * semantics as they do for the "project" function, namely, that the path
 * will stop as soon as it hits a monster, or that the path will continue
 * through the destination grid, respectively.
 *:::STOPはモンスターに当たると止まり、THRUはモンスターに当たっても続く
 *
 * The "PROJECT_JUMP" flag, which for the "project()" function means to
 * start at a special grid (which makes no sense in this function), means
 * that the path should be "angled" slightly if needed to avoid any wall
 * grids, allowing the player to "target" any grid which is in "view".
 * This flag is non-trivial and has not yet been implemented, but could
 * perhaps make use of the "vinfo" array (above).  XXX XXX XXX
 *:::何言ってるか分からん。
 *
 * This function returns the number of grids (if any) in the path.  This
 * function will return zero if and only if (y1,x1) and (y2,x2) are equal.
 *:::ターゲットまでのグリッド数を返す。始点と終点が同じとき0を返す。
 *
 * This algorithm is similar to, but slightly different from, the one used
 * by "update_view_los()", and very different from the one used by "los()".

 *:::gp[]には、x,y両方のグリッド座標が入るらしい。上位8bitがy、下位8bitがx
 *:::詳しい計算は未読
 */
sint project_path(u16b *gp, int range, int y1, int x1, int y2, int x2, int flg)
{
	int y, x;

	int n = 0;
	int k = 0;

	/* Absolute */
	int ay, ax;

	/* Offsets */
	int sy, sx;

	/* Fractions */
	int frac;

	/* Scale factors */
	int full, half;

	/* Slope */
	int m;

	/* No path necessary (or allowed) */
	if ((x1 == x2) && (y1 == y2)) return (0);


	/* Analyze "dy" */
	if (y2 < y1)
	{
		ay = (y1 - y2);
		sy = -1;
	}
	else
	{
		ay = (y2 - y1);
		sy = 1;
	}

	/* Analyze "dx" */
	if (x2 < x1)
	{
		ax = (x1 - x2);
		sx = -1;
	}
	else
	{
		ax = (x2 - x1);
		sx = 1;
	}


	/* Number of "units" in one "half" grid */
	half = (ay * ax);

	/* Number of "units" in one "full" grid */
	full = half << 1;

	/* Vertical */
	if (ay > ax)
	{
		/* Let m = ((dx/dy) * full) = (dx * dx * 2) */
		m = ax * ax * 2;

		/* Start */
		y = y1 + sy;
		x = x1;

		frac = m;

		if (frac > half)
		{
			/* Advance (X) part 2 */
			x += sx;

			/* Advance (X) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if (frac > half)
				{
					/* Advance (X) part 2 */
					x += sx;

					/* Advance (X) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (Y) */
			y += sy;
		}
	}

	/* Horizontal */
	else if (ax > ay)
	{
		/* Let m = ((dy/dx) * full) = (dy * dy * 2) */
		m = ay * ay * 2;

		/* Start */
		y = y1;
		x = x1 + sx;

		frac = m;

		/* Vertical change */
		if (frac > half)
		{
			/* Advance (Y) part 2 */
			y += sy;

			/* Advance (Y) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if (frac > half)
				{
					/* Advance (Y) part 2 */
					y += sy;

					/* Advance (Y) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (X) */
			x += sx;
		}
	}

	/* Diagonal */
	else
	{
		/* Start */
		y = y1 + sy;
		x = x1 + sx;

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (n >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;
		}
	}

	/* Length */
	return (n);
}



/*
 * Mega-Hack -- track "affected" monsters (see "project()" comments)
 */
static int project_m_n;
static int project_m_x;
static int project_m_y;
/* Mega-Hack -- monsters target */
static s16b monster_target_x;
static s16b monster_target_y;


/*
 * We are called from "project()" to "damage" terrain features
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * XXX XXX XXX Perhaps we should affect doors?
 */
/*:::魔法などの地形に対する影響*/
/*:::＠に見えるところで何か起こったらTRUEを返す*/
/*:::who:使用者 r:ターゲット座標の爆心地からの距離 y,x ターゲット座標 dam:ダメージ typ:属性*/
///sys magic 属性　攻撃魔法などによる地形に対する影響　木の破壊や溶岩変化など
bool project_f(int who, int r, int y, int x, int dam, int typ)
{
	cave_type       *c_ptr = &cave[y][x];
	feature_type    *f_ptr = &f_info[c_ptr->feat];

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);


	/* XXX XXX XXX */
	/*:::負の値がFALSEとみなされる場合のための処理・・・・？*/
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	//閃光弾の爆心地からライトエリア
	if(typ == GF_SPECIAL_SHOT && special_shooting_mode == SSM_LIGHT && !r)
		lite_room(y,x);

	///mod150721
	/*:::皿の破壊処理*/
	if (is_plate_grid(c_ptr))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
		case GF_DISINTEGRATE:
			message = _("消えた", "disappears");break;
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_ROCKET:
		case GF_SHARDS:
		case GF_SOUND:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_TRAIN:
		case GF_DISENCHANT:

			message = _("割れた", "breaks");break;
		default:
			message = NULL;break;
		}
		if (message && known)
		{
			msg_format(_("皿は%s。", "The plate %s."), message);
			futo_break_plate(y,x);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	//爆弾の誘爆処理
	else if(((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_BOMB)))
	{
		int tmp_dam = 0;
		switch (typ)
		{
		case GF_TIME:
		case GF_DISINTEGRATE:
		case GF_PIT_FALL:
			//爆弾消滅処理
			msg_format(_("爆弾は消えた。", "The bomb disappears."));
			c_ptr->info &= ~(CAVE_MARK);
			c_ptr->info &= ~(CAVE_OBJECT);
			c_ptr->mimic = 0;
			c_ptr->special = 0;
			note_spot(y, x);
			lite_spot(y, x);
			break;
		case GF_FIRE:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_SHARDS:
			tmp_dam = dam / 10 + randint0(3);
			bomb_count(y,x,-tmp_dam);
			break;
		case GF_NUKE:
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_ROCKET:
			tmp_dam = randint1(dam/3);
			bomb_count(y,x,-tmp_dam);
			break;

		case GF_WATER:
		case GF_INACT:
			tmp_dam = 0-(dam/5);
			bomb_count(y,x,-tmp_dam);
			break;
		case GF_CHAOS:
			tmp_dam = 30 - randint1(60);
			bomb_count(y,x,-tmp_dam);
			break;

		default:
			break;

		}
	}
	//v1.1.68 新地形「氷塊」
	else if (have_flag(f_ptr->flags, FF_ICE_WALL))
	{

		if (typ == GF_FIRE || typ == GF_NUKE || typ == GF_STEAM)
		{
			msg_format(_("氷が溶けた。", "The ice melts."));
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
			cave_set_feat(y, x, feat_floor);

		}
		else if (typ == GF_LITE)
		{
			msg_format(_("氷の中を光が乱反射している...",
                            "The light chaotically refracts in the ice..."));

			if (randint1(100) < dam)
			{
				if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
				cave_set_feat(y, x, feat_floor);
				msg_format(_("氷が大爆発した!", "The ice explodes!"));
				project(PROJECT_WHO_EXPLODE_ICE, 5, y, x, dam * 4, GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
			}

		}

	}
	///mod150402
	/*:::蜘蛛の巣の破壊処理*/
	else if (have_flag(f_ptr->flags, FF_SPIDER_NEST_1))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
		case GF_WATER:
		case GF_HOLY_WATER:
			message = _("消えた", "disappears");break;
		case GF_COLD:
		case GF_NORTHERN:
		case GF_ICE:
			message = _("砕けた", "shatters");break;
		case GF_FIRE:
		case GF_ACID:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_NUKE:
		case GF_STEAM:
			message = _("溶けた", "melts");break;
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_ROCKET:
		case GF_SOUND:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_WINDCUTTER:
		case GF_TRAIN:
		case GF_MOSES:
		case GF_REDMAGIC:
		case GF_GUNGNIR:

			message = _("吹き飛んだ", "gets blown away");break;
		default:
			message = NULL;break;
		}
		if (message && known)
		{
			msg_format(_("巣は%s。", "The web %s."), message);
			cave_set_feat(y, x, feat_floor);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	//v1.1.21 ポータルの破壊処理　(壊しとかないと爆弾とポータルとバリケードでハメ殺しできそうなので)
	else if (have_flag(f_ptr->flags, FF_PORTAL))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
			message = _("消えた", "disappears");break;
		case GF_NUKE:
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_ROCKET:
		case GF_SHARDS:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TRAIN:
			message = _("破壊された", "is destroyed");break;
		default:
			message = NULL;break;
		}
		if (message)
		{
			msg_format(_("ポータルが%s！", "The portal %s!"), message);
			cave_set_feat(y, x, feat_floor);
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	/*:::木の破壊処理*/
	else if (have_flag(f_ptr->flags, FF_TREE))
	{
		cptr message;
		switch (typ)
		{
		case GF_POIS:
		case GF_POLLUTE:
		case GF_DEATH_RAY:
		case GF_YAKU:
#ifdef JP
			message = "枯れた";break;
#else
			message = "was blasted.";break;
#endif
		case GF_TIME:
#ifdef JP
			message = "縮んだ";break;
#else
			message = "shrank.";break;
#endif
		case GF_ACID:
#ifdef JP
			message = "溶けた";break;
#else
			message = "melted.";break;
#endif
		case GF_COLD:
		case GF_NORTHERN:
		case GF_ICE:
#ifdef JP
			message = "凍り、砕け散った";break;
#else
			message = "was frozen and smashed.";break;
#endif
		case GF_FIRE:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_NUKE:
#ifdef JP
			message = "燃えた";break;
#else
			message = "burns up!";break;
#endif
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_SHARDS:
		case GF_ROCKET:
		case GF_SOUND:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_GRAVITY:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_WINDCUTTER:
		case GF_TRAIN:
		case GF_MOSES:
		case GF_REDMAGIC:
		case GF_GUNGNIR:
		case GF_BLAZINGSTAR:

#ifdef JP
			message = "粉砕された";break;
#else
			message = "was crushed.";break;
#endif
		case GF_SOULSCULPTURE:
		    message = _("斬り倒された", "was cut down.");break;

		case GF_YOUMU:
			if (p_ptr->pclass == CLASS_BEEKEEPER)
			{
				message = _("粉砕された", "was crushed.");
				break;
			}
			else
			{
				message = _("斬り倒された", "was cut down.");
				break;
			}

		default:
			message = NULL;break;
		}

		if(message) cave_set_feat(y, x, one_in_(3) ? feat_brake : feat_grass);

		if (message && known)
		{
#ifdef JP
			msg_format("木は%s。", message);
#else
			msg_format("A tree %s", message);
#endif

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}

	}



	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
		//ここにきちんと書かなくても何もされないが
		case GF_CAPTURE:
		case GF_REDMAGIC:
		case GF_GUNGNIR:
		case GF_HAND_DOOM:
		case GF_CAUSE_1:
		case GF_CAUSE_2:
		case GF_CAUSE_3:
		case GF_CAUSE_4:
		case GF_MIND_BLAST:
		case GF_BRAIN_SMASH:
		case GF_DRAIN_MANA:
		case GF_PSY_SPEAR:
		case GF_FORCE:
		case GF_HOLY_FIRE:
		case GF_HELL_FIRE:
		case GF_PSI:
		case GF_PSI_DRAIN:
		case GF_TELEKINESIS:
		case GF_DOMINATION:
		case GF_IDENTIFY:
		case GF_ATTACK:
		case GF_ACID:
		case GF_ELEC:
		case GF_COLD:
		case GF_ICE:
		case GF_FIRE:
		case GF_PLASMA:
		case GF_METEOR:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_COSMIC_HORROR:
		case GF_DISTORTION:
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		case GF_KYUTTOSHITEDOKA_N:
		case GF_WINDCUTTER:
		case GF_REDEYE:
		case GF_RYUURI:
		case GF_DISP_LIVING:
		case GF_POSSESSION:
		case GF_SOULSTEAL:
		case GF_NIGHTMARE:
		case GF_N_E_P:
		case GF_SATORI:
		case GF_KOKORO:
		case GF_SOULSCULPTURE:
		case GF_STEAM:
		case GF_BRAIN_FINGER_PRINT:
		case GF_RAINBOW:
		{
			break;
		}

		/* Destroy Traps (and Locks) */
		/*:::トラップ解除*/
		//v1.1.17 純化もトラップ解除するようにしてみる
		case GF_KILL_TRAP:
		case GF_PURIFY:
		{
			/* Reveal secret doors */
			if (is_hidden_door(c_ptr))
			{
				/* Pick a door */
				disclose_grid(y, x);

				/* Check line of sight */
				if (known)
				{
					obvious = TRUE;
				}
			}

			/* Destroy traps */
			if (is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if (known && typ == GF_KILL_TRAP)
				{
#ifdef JP
					msg_print("まばゆい閃光が走った！");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the trap */
				cave_alter_feat(y, x, FF_DISARM);
			}

			/* Locked doors are unlocked */
			if (is_closed_door(c_ptr->feat) && f_ptr->power && have_flag(f_ptr->flags, FF_OPEN))
			{
				s16b old_feat = c_ptr->feat;

				/* Unlock the door */
				cave_alter_feat(y, x, FF_DISARM);

				/* Check line of sound */
				if (known && (old_feat != c_ptr->feat))
				{
#ifdef JP
					msg_print("カチッと音がした！");
#else
					msg_print("Click!");
#endif

					obvious = TRUE;
				}
			}

			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		/* Destroy Doors (and traps) */
		/*:::トラップ・ドア破壊*/
		case GF_KILL_DOOR:
		{
			/* Destroy all doors and traps */
			if (is_trap(c_ptr->feat) || have_flag(f_ptr->flags, FF_DOOR))
			{
				/* Check line of sight */
				if (known)
				{
					/* Message */
#ifdef JP
					msg_print("まばゆい閃光が走った！");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the feature */
				cave_alter_feat(y, x, FF_TUNNEL);
			}

			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		//v1.1.96 トラップ発動
		case GF_ACTIV_TRAP:
		{
			/* Destroy traps */
			if (is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if (known && typ == GF_KILL_TRAP)
				{
#ifdef JP
					msg_print("トラップが発動した！");
#else
					msg_print("The trap activates!");
#endif
					obvious = TRUE;
				}

				activate_floor_trap(y, x, 0L);

			}


		}
		break;


		/*:::ドア固定（魔法の施錠？）*/
		/*:::固定可能な閉じたドアに有効　開いたドアには矢が当たらない*/
		case GF_JAM_DOOR: /* Jams a door (as if with a spike) */
		{
			if (have_flag(f_ptr->flags, FF_SPIKE))
			{
				s16b old_mimic = c_ptr->mimic;
				feature_type *mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

				cave_alter_feat(y, x, FF_SPIKE);

				c_ptr->mimic = old_mimic;

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				/* Check line of sight */
				if (known && have_flag(mimic_f_ptr->flags, FF_OPEN))
				{
					/* Message */
#ifdef JP
					msg_format("%sに何かがつっかえて開かなくなった。", f_name + mimic_f_ptr->name);
#else
					msg_format("The %s seems stuck.", f_name + mimic_f_ptr->name);
#endif

					obvious = TRUE;
				}
			}
			break;
		}

		/* Destroy walls (and doors) */
		/*:::岩石溶解*/
		case GF_KILL_WALL:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
#ifdef JP

					if (have_flag(f_ptr->flags, FF_ICE_WALL))
						msg_format("%sが溶けた！", f_name + f_info[get_feat_mimic(c_ptr)].name);
					else
						msg_format("%sが溶けて泥になった！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					if (have_flag(f_ptr->flags, FF_ICE_WALL))
						msg_format("The %s melts!", f_name + f_info[get_feat_mimic(c_ptr)].name);
					else
						msg_format("The %s turns into mud!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}
		case GF_WALL_TO_DOOR:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_format(_("%sがドアになった！", "%s turns into a door!"), f_name + f_info[get_feat_mimic(c_ptr)].name);
					obvious = TRUE;
				}

				cave_set_feat(y, x, feat_door[DOOR_DOOR].closed);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}

		/*:::宝塔レーザー　床に財宝を落とす */
		case GF_HOUTOU:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				object_type forge;
				object_type *q_ptr = &forge;
				object_wipe(q_ptr);
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					if (p_ptr->inside_arena)
						msg_format(_("%sが宝石に変わった...がすぐ消えてしまった。", "%s turns into treasure... but then quickly fades away."), f_name + f_info[get_feat_mimic(c_ptr)].name);
					else
						msg_format(_("%sが宝石に変わった！", "%s turns into treasure!"), f_name + f_info[get_feat_mimic(c_ptr)].name);

					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				if (!cave_naked_bold(y, x)) break;
				if (player_bold(y, x)) break;

				//v2.1.0 夢日記では宝塔レーザーの宝石変化を無効化する
				if (p_ptr->inside_arena) break;

				if (!make_gold(q_ptr)) break;

				(void)drop_near(q_ptr, -1, y, x);
				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}

		/* Make doors */
		case GF_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_door[DOOR_DOOR].closed);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		//v2.0.1 要石属性　爆心地を岩地形に変える
		case GF_KANAMEISHI:
		{
			//爆心地のみ対象
			if (r) break;

			//永久地形には無効
			if (cave_have_flag_bold(y, x, FF_PERMANENT)) break;

			//モンスターやアイテムのあるところには無効
			if ((cave[y][x].info & CAVE_OBJECT) || (cave[y][x].o_idx) || (cave[y][x].m_idx)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_rubble);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}

		/* Make doors */
		case GF_MAKE_FLOWER:
		{
			/* Require a "naked" floor grid */
			if (!cave_clean_bold(y, x) || cave[y][x].m_idx) break;

			/* Not on the player */
			//if (player_bold(y, x)) break;

			if(one_in_(4)) cave_set_feat(y, x, feat_tree);
			else cave_set_feat(y, x, feat_flower);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		/* Make traps */
		/*:::トラップ生成*/
		case GF_MAKE_TRAP:
		{
			/* Place a trap */
			place_trap(y, x);

			break;
		}

		/* Make doors */
		/*:::森林生成*/
		case GF_MAKE_TREE:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_tree);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;


			break;
		}


		case GF_MAKE_SPIDER_NEST:
		{
			if (!cave_have_flag_bold(y, x, FF_FLOOR)) break;
			if ((cave[y][x].info & CAVE_OBJECT)) break;
			cave_set_feat(y, x, f_tag_to_index_in_init("SPIDER_NEST"));
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		case GF_THROW_PLATE:
		{
			if(!futo_can_place_plate(y,x)) break;

			cave[y][x].info |= CAVE_OBJECT;
			cave[y][x].mimic = feat_plate;

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}


		case GF_MAKE_GLYPH:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Create a glyph */
			c_ptr->info |= CAVE_OBJECT;
			c_ptr->mimic = feat_glyph;

			/* Notice */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);

			break;
		}

		case GF_STONE_WALL:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			//v1.1.89 百々世特技用に壁から岩にする。普通に石壁生成したときも岩になってしまうがまあ実用上の問題はないだろう
			if(p_ptr->pclass == CLASS_MOMOYO)
				cave_set_feat(y, x, feat_rubble);
			else
				cave_set_feat(y, x, feat_granite);

			break;
		}


		case GF_LAVA_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Lava */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow lava */
				cave_set_feat(y, x, feat_shallow_lava);
			}
			/* Deep Lava */
			else if (dam)
			{
				/* Place a deep lava */
				cave_set_feat(y, x, feat_deep_lava);
			}

			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}

		case GF_WATER_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(y, x, feat_shallow_water);
			}
			/* Deep Water */
			else if (dam)
			{
				/* Place a deep water */
				cave_set_feat(y, x, feat_deep_water);
			}
			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		//v1.1.91
		case GF_DIG_OIL:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Ignore grid without enough space */
			//トラップ創造で罠地形を敷き詰めたら石油地形変化を防げたりする
			if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

			//v1.1.92 女苑専用性格では油地形に変化させたグリッド数を記録することにしてみる
			if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON ) && !who)
				jyoon_record_money_waste(1);

			cave_set_feat(y, x, feat_oil_field);

			//＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}


		case GF_MAKE_BLIZZARD: //v1.1.85 魔法吹雪
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_cold_zone);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//魔法吹雪生成は溶岩や水と違い壁を削らない
				cave_set_feat(y, x, feat_heavy_cold_zone);
			}
			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_STORM: //v1.1.85 魔法の雷雲
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_electrical_zone);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//雷雲生成は溶岩や水と違い壁を削らない
				cave_set_feat(y, x, feat_heavy_electrical_zone);
			}
			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_POISON_PUDDLE: //v1.1.85 毒の沼
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_poisonous_puddle);
			}
			else if (dam)
			{
				cave_set_feat(y, x, feat_deep_poisonous_puddle);
			}
			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_ACID_PUDDLE: //v1.1.85 酸の沼
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_acid_puddle);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//魔法吹雪生成は溶岩や水と違い壁を削らない
				cave_set_feat(y, x, feat_deep_acid_puddle);
			}
			//v1.1.85 ＠のいる位置が地形変化して何らかのパラメータが変わるかもしれんので追加
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;
			break;
		}


		case GF_MOSES:
		{
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;
			//v1.1.85 酸の沼と毒の沼も追加
			if (!have_flag(f_ptr->flags, FF_WATER) && !have_flag(f_ptr->flags, FF_LAVA) && !have_flag(f_ptr->flags, FF_ACID_PUDDLE && !have_flag(f_ptr->flags, FF_POISON_PUDDLE))) break;
			cave_set_feat(y, x,  feat_floor);
			break;
		}
		/* Lite up the grid */
		case GF_LITE_WEAK:
		case GF_LITE:
		{
			/* Turn on the light */
			if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				c_ptr->info |= (CAVE_GLOW);

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Observe */
				if (player_can_see_bold(y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

				if (p_ptr->special_defense & NINJA_S_STEALTH)
				{
					if (player_bold(y, x)) set_superstealth(FALSE);
				}
			}

			break;
		}

		/* Darken the grid */
		case GF_DARK_WEAK:
		case GF_DARK:
		{
			bool do_dark = !p_ptr->inside_battle && !is_mirror_grid(c_ptr);
			int j;

			/* Turn off the light. */
			if (do_dark)
			{
				if (dun_level || !is_daytime())
				{
					for (j = 0; j < 9; j++)
					{
						int by = y + ddy_ddd[j];
						int bx = x + ddx_ddd[j];

						if (in_bounds2(by, bx))
						{
							cave_type *cc_ptr = &cave[by][bx];

							if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
							{
								do_dark = FALSE;
								break;
							}
						}
					}

					if (!do_dark) break;
				}

				c_ptr->info &= ~(CAVE_GLOW);

				/* Hack -- Forget "boring" grids */
				if (!have_flag(f_ptr->flags, FF_REMEMBER))
				{
					/* Forget */
					c_ptr->info &= ~(CAVE_MARK);

					/* Notice */
					note_spot(y, x);
				}

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Notice */
				if (player_can_see_bold(y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);
			}

			/* All done */
			break;
		}

		case GF_SHARDS:
		case GF_ROCKET:
		{
			if (is_mirror_grid(c_ptr))
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was broken!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(y, x);
				project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 50))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was broken!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}

		case GF_SOUND:
		{
			if (is_mirror_grid(c_ptr) && p_ptr->lev < 40)
			{
#ifdef JP
				msg_print("鏡が割れた！");
#else
				msg_print("The mirror was broken!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(y, x);
				project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 200))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%sが割れた！", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was broken!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}

		case GF_DISINTEGRATE:
		case GF_BLAZINGSTAR:
		{
			/* Destroy mirror/glyph */
			if (is_mirror_grid(c_ptr) || is_glyph_grid(c_ptr) || is_explosive_rune_grid(c_ptr))
				remove_mirror(y, x);

			/* Permanent features don't get effect */
			/* But not protect monsters and other objects */
			if (have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT))
			{
				cave_alter_feat(y, x, FF_HURT_DISI);

				/* Update some things -- similar to GF_KILL_WALL */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}
	}

	lite_spot(y, x);
	/* Return "Anything seen?" */
	return (obvious);
}



/*
 * We are called from "project()" to "damage" objects
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * Perhaps we should only SOMETIMES damage things on the ground.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 */
/*:::指定グリッド、指定属性による床上アイテムに対する破壊処理　*/
///item spell 属性によるアイテム破壊処理
bool project_o(int who, int r, int y, int x, int dam, int typ)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	u32b flgs[TR_FLAG_SIZE];

	char o_name[MAX_NLEN];

	int k_idx = 0;
	bool is_potion = FALSE;

	bool flag_need_restart_loop;


	/* XXX XXX XXX */
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);

	do
	{
		flag_need_restart_loop = FALSE;

        /* Scan all objects in the grid */
        /*:::床上アイテムループ*/
        for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
        {
            /* Acquire object */
            object_type *o_ptr = &o_list[this_o_idx];

            bool is_art = FALSE;
            bool ignore = FALSE;
            bool do_kill = FALSE;

            cptr note_kill = NULL;

#ifndef JP
            /* Get the "plural"-ness */
            bool plural = (o_ptr->number > 1);
#endif

            /* Acquire next object */
            next_o_idx = o_ptr->next_o_idx;

            /* Extract the flags */
            object_flags(o_ptr, flgs);

            /* Check for artifact */
            if (object_is_artifact(o_ptr)) is_art = TRUE;

            ///mod151227 蓬莱の薬は破壊されない
            if(o_ptr->tval == TV_COMPOUND && o_ptr->sval == SV_COMPOUND_HOURAI) ignore = TRUE;

            //v1.1.79 ラフノールの鏡も破壊されない
            if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MIRROR_OF_RUFFNOR) ignore = TRUE;

            /* Analyze the type */
            /*:::ベースアイテムが指定属性に弱い場合破壊フラグをON*/
            /*:::個別に属性保護されたアイテムは無視フラグをON*/
            switch (typ)
            {
                /* Acid -- Lots of things */
			case GF_ACID:
			{
				if (hates_acid(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "融けてしまった！";
#else
					note_kill = (plural ? " melt!" : " melts!");
#endif

					if (have_flag(flgs, TR_IGNORE_ACID)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
			case GF_ELEC:
			{
				if (hates_elec(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
			case GF_FIRE:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
			case GF_COLD:
			{
				if (hates_cold(o_ptr))
				{
#ifdef JP
                    note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
			case GF_PLASMA:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_elec(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
                    note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Cold */
			case GF_METEOR:
			case GF_KANAMEISHI:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "燃えてしまった！";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_cold(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
                    note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
			case GF_ICE:
			case GF_NORTHERN:
			case GF_SHARDS:
			case GF_FORCE:
			case GF_SOUND:
			case GF_TRAIN:
			case GF_TORNADO:
			{
				if (hates_cold(o_ptr))
				{
#ifdef JP
                    note_kill = "砕け散ってしまった！";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
				}
				break;
			}

			/* Mana and Chaos -- destroy everything */
			case GF_MANA:
			case GF_SEEKER:
			case GF_SUPER_RAY:
			{
				do_kill = TRUE;
#ifdef JP
                note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				break;
			}

			case GF_DISINTEGRATE:
			case GF_BLAZINGSTAR:
			{
				do_kill = TRUE;
#ifdef JP
                note_kill = "蒸発してしまった！";
#else
				note_kill = (plural ? " evaporate!" : " evaporates!");
#endif

				break;
			}

			case GF_CHAOS:
			{
				do_kill = TRUE;
#ifdef JP
                note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_CHAOS)) ignore = TRUE;
				else if ((o_ptr->tval == TV_SCROLL) && (o_ptr->sval == SV_SCROLL_CHAOS)) ignore = TRUE;
				break;
			}
			///mod140211劣化属性もアイテムすべて壊すようにした
			case GF_DISENCHANT:
			{
				do_kill = TRUE;
#ifdef JP
                note_kill = "崩れてしまった！";
#else
				note_kill = (plural ? " break down!" : " breaks down!");
#endif

				if (have_flag(flgs, TR_RES_DISEN)) ignore = TRUE;
				break;
			}
			///空間歪曲属性と汚染属性 1/3でアイテムを破壊する
			case GF_DISTORTION:
			case GF_POLLUTE:
			{
				if (!one_in_(3)) break;

				do_kill = TRUE;
#ifdef JP
                note_kill = "壊れてしまった！";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_TIME)) ignore = TRUE;
				break;
			}


			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
			case GF_HOLY_FIRE:
			case GF_HELL_FIRE:
			{
				if (object_is_cursed(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
                    note_kill = "壊れてしまった！";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				}
				break;
			}

			case GF_IDENTIFY:
			{
				identify_item(o_ptr);

				/* Auto-inscription */
				autopick_alter_item((-this_o_idx), FALSE);
				break;
			}

			//v1.1.97 トラップ発動
			case GF_ACTIV_TRAP:
			{
				if (o_ptr->tval == TV_CHEST)
				{
					if (o_ptr->pval > 0)
					{
						activate_chest_trap(y, x, this_o_idx, FALSE);
						obvious = TRUE;

						// -Hack- ＠以外へのトラップ効果の実装により、
						//箱が爆発したり箱のトラップで混沌('E')を倒したりするなどしてこのグリッドの他のアイテムがループ中に破壊されることがある。
						//そのままループを続けるとnext_o_idxがずれて処理がおかしくなるのでこのグリッドに対するアイテム処理ループをやり直すことにした。
						flag_need_restart_loop = TRUE;
					}
				}
			}
			break;

			/* Unlock chests */
			case GF_KILL_TRAP:
			case GF_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if (o_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if (o_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						o_ptr->pval = (0 - o_ptr->pval);

						/* Identify */
						object_known(o_ptr);

						/* Notice */
						if (known && (o_ptr->marked & OM_FOUND))
						{
#ifdef JP
                            msg_print("カチッと音がした！");
#else
							msg_print("Click!");
#endif

							obvious = TRUE;
						}
					}
				}

				break;
			}
			///sysdel 死者復活
			//case GF_ANIM_DEAD:
			{
				///del131214  死者復活
#if 0
				if (o_ptr->tval == TV_CORPSE)
				{
					int i;
					u32b mode = 0L;

					if (!who || is_pet(&m_list[who]))
						mode |= PM_FORCE_PET;

					for (i = 0; i < o_ptr->number; i++)
					{
						if (((o_ptr->sval == SV_CORPSE) && (randint1(100) > 80)) ||
						    ((o_ptr->sval == SV_SKELETON) && (randint1(100) > 60)))
						{
							if (!note_kill)
							{
#ifdef JP
                                note_kill = "灰になった。";
#else
                                note_kill = (plural ? " become dust." : " becomes dust.");
#endif
							}
							continue;
						}
						else if (summon_named_creature(who, y, x, o_ptr->pval, mode))
						{
#ifdef JP
                            note_kill = "生き返った。";
#else
                            note_kill = " revived.";
#endif
						}
						else if (!note_kill)
						{
#ifdef JP
                            note_kill = "灰になった。";
#else
							note_kill = (plural ? " become dust." : " becomes dust.");
#endif
						}
					}
					do_kill = TRUE;
					obvious = TRUE;
				}
#endif
				break;
			}
            }


            /* Attempt to destroy the object */
            if (do_kill)
            {
                /* Effect "observed" */
                if (known && (o_ptr->marked & OM_FOUND))
                {
                    obvious = TRUE;
                    object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
                }

                /* Artifacts, and other objects, get to resist */
                /*:::アーティファクトと元素保護アイテムは影響を受けない*/
                if (is_art || ignore)
                {
                    /* Observe the resist */
                    if (known && (o_ptr->marked & OM_FOUND))
                    {
#ifdef JP
                        msg_format("%sは影響を受けない！",
                            o_name);
#else
                        msg_format("The %s %s unaffected!",
                            o_name, (plural ? "are" : "is"));
#endif

                    }
                }

                /* Kill it */
                /*:::アイテム破壊処理　*/
                else
                {
                    /* Describe if needed */
                    if (known && (o_ptr->marked & OM_FOUND) && note_kill)
                    {
#ifdef JP
                        msg_format("%sは%s", o_name, note_kill);
#else
                        msg_format("The %s%s", o_name, note_kill);
#endif

                    }

                    k_idx = o_ptr->k_idx;
                    is_potion = object_is_potion(o_ptr);


                    /* Delete the object */
                    delete_object_idx(this_o_idx);

                    /* Potions produce effects when 'shattered' */
                    if (is_potion)
                    {
                        (void)potion_smash_effect(who, y, x, k_idx);
                    }

                    /* Redraw */
                    lite_spot(y, x);
                }
            }


			if (flag_need_restart_loop)
			{
				if (cheat_xtra) msg_print("RESTART LOOP");
				break;
			}

		} //next_o_idxループ

	} while (flag_need_restart_loop);

	/* Return "Anything seen?" */
	return (obvious);
}





//v1.1.90
//モンスターに対するセービングスロー処理
//モンスターが抵抗に成功したらTRUE
//if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
//↑のような処理と入れ替える
bool	mon_saving_throw(monster_type *m_ptr, int power)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int chance = power;
	int rlev = r_ptr->level;

	//v1.1.95 防御低下状態なら抵抗力が下がることにした
	if (MON_DEC_DEF(m_ptr))
	{
		rlev -= MAX(25, rlev / 4);
		if (rlev < 0) rlev = 0;
	}

	//ユニークには抵抗されやすい
	if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 2 / 3;

	//神格には抵抗されやすい
	if (r_ptr->flags3 & RF3_DEITY) chance = chance * 3 / 4;

	//力強いモンスターには抵抗されやすい
	if (r_ptr->flags2 & RF2_POWERFUL) chance = chance * 3 / 4;

	//元の判定式の10というマジックナンバーがなんか気に入らんから＠のレベルにした。まあ実質何の違いもないだろう。
	if (chance < p_ptr->lev) chance = p_ptr->lev;

	if (cheat_xtra) msg_format("saving throw: power%d-lev%d",chance,r_ptr->level);

	if (r_ptr->level >= randint1(chance)) return TRUE;
	else return FALSE;

}



/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * This routine takes a "source monster" (by index) which is mostly used to
 * determine if the player is causing the damage, and a "radius" (see below),
 * which is used to decrease the power of explosions with distance, and a
 * location, via integers which are modified by certain types of attacks
 * (polymorph and teleport being the obvious ones), a default damage, which
 * is modified as needed based on various properties, and finally a "damage
 * type" (see below).
 *:::主に＠によるモンスターへのダメージに使う。radius（距離）で減衰。
 *:::radやdamはダメージを与えない一部の攻撃のパラメータとして使われることもある。
 *
 * Note that this routine can handle "no damage" attacks (like teleport) by
 * taking a "zero" damage, and can even take "parameters" to attacks (like
 * confuse) by accepting a "damage", using it to calculate the effect, and
 * then setting the damage to zero.  Note that the "damage" parameter is
 * divided by the radius, so monsters not at the "epicenter" will not take
 * as much damage (or whatever)...
 *
 * Note that "polymorph" is dangerous, since a failure in "place_monster()"'
 * may result in a dereference of an invalid pointer.  XXX XXX XXX
 *:::チェンジモンスターは危険。place_monster()の失敗はポインタエラーの原因になる？
 *
 * Various messages are produced, and damage is applied.
 *
 * Just "casting" a substance (i.e. plasma) does not make you immune, you must
 * actually be "made" of that substance, or "breathe" big balls of it.
 *
 * We assume that "Plasma" monsters, and "Plasma" breathers, are immune
 * to plasma.
 *
 * We assume "Nether" is an evil, necromantic force, so it doesn't hurt undead,
 * and hurts evil less.  If can breath nether, then it resists it as well.
 *:::死のパワーはアンデッドには無効、邪悪な敵に耐性。
 *
 * Damage reductions use the following formulas:
 *   Note that "dam = dam * 6 / (randint1(6) + 6);"
 *     gives avg damage of .655, ranging from .858 to .500
 *   Note that "dam = dam * 5 / (randint1(6) + 6);"
 *     gives avg damage of .544, ranging from .714 to .417
 *   Note that "dam = dam * 4 / (randint1(6) + 6);"
 *     gives avg damage of .444, ranging from .556 to .333
 *   Note that "dam = dam * 3 / (randint1(6) + 6);"
 *     gives avg damage of .327, ranging from .427 to .250
 *   Note that "dam = dam * 2 / (randint1(6) + 6);"
 *     gives something simple.
 *
 * In this function, "result" messages are postponed until the end, where
 * the "note" string is appended to the monster name, if not NULL.  So,
 * to make a spell have "no effect" just set "note" to NULL.  You should
 * also set "notice" to FALSE, or the player will learn what the spell does.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
/* "flg" was added. */
/*:::モンスターへのダメージ*/
///mon res magic モンスターの属性耐性、弱点など
///sys ダメージを表記とかするとき変更要
bool project_m(int who, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg)
{
	int tmp;

	cave_type *c_ptr = &cave[y][x];

	monster_type *m_ptr = &m_list[c_ptr->m_idx];
	monster_type *caster_ptr = (who > 0) ? &m_list[who] : NULL;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char killer[80];

	/* Is the monster "seen"? */
	bool seen = m_ptr->ml;
	bool seen_msg = is_seen(m_ptr);

	bool slept = (bool)MON_CSLEEP(m_ptr);

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Can the player know about this effect? */
	bool known = ((m_ptr->cdis <= MAX_SIGHT) || p_ptr->inside_battle);

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Gets the monster angry at the source of the effect? */
	bool get_angry = FALSE;

	/* Polymorph setting (true or false) */
	bool do_poly = FALSE;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Time amount (amount to time) */
	int do_time = 0;

	//v1.1.94 ステータス異常追加
	int do_dec_atk = 0;
	int do_dec_def = 0;
	int do_dec_mag = 0;

	//v1.1.95 追加
	int do_drunk = 0;//←実処理未実装
	int do_berserk = 0;
	int do_not_move = 0;



	bool heal_leper = FALSE;

	/* Hold the monster name */
	char m_name[80];

#ifndef JP
	char m_poss[10];
#endif

	int photo = 0;

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = extract_note_dies(real_r_ptr(m_ptr));

	int ty = m_ptr->fy;
	int tx = m_ptr->fx;

	/*:::この攻撃の使用者のレベル　＠なら二倍になるらしい*/
	int caster_lev = (who > 0) ? r_info[caster_ptr->r_idx].level : (p_ptr->lev * 2);

	//v1.1.41 モンスターがモンスターを倒したときプレイヤーとモンスターの両方が経験値を得られるフラグ。舞と里乃専用
	bool flag_monster_and_player_gain_exp = FALSE;


	/* Nobody here */
	/*:::このグリッドに何も居なければ終了*/
	if (!c_ptr->m_idx) return (FALSE);

	/* Never affect projector */
	/*:::呪文の使用者には効果を及ぼさない。乗馬中のときにも回復や加速を除いて効果を及ぼさない。*/
	///sys magic もし乗馬モンスターや使用者に効果を及ぼすボールとか作るならここに記述する
	if (who && (c_ptr->m_idx == who)) return (FALSE);
	if ((c_ptr->m_idx == p_ptr->riding) && !who && !(typ == GF_OLD_HEAL) && !(typ == GF_OLD_SPEED) && !(typ == GF_STAR_HEAL)) return (FALSE);
	///sysdel mon 助さん格さん　削除予定
	//if (sukekaku && ((m_ptr->r_idx == MON_SUKE) || (m_ptr->r_idx == MON_KAKU))) return FALSE;

	/* Don't affect already death monsters */
	/* Prevents problems with chain reactions of exploding monsters */
	/*:::死んだモンスターへは効果を及ぼさない*/
	if (m_ptr->hp < 0) return (FALSE);


	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Get the monster name (BEFORE polymorphing) */
	monster_desc(m_name, m_ptr, 0);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

	if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) disturb(1, 1);

	//v1.1.21 兵士が特定の射撃を行ったとき「クリティカル」として威力が強化される判定
	//射撃を行ってproject_mが最初に呼ばれたモンスターにのみ適用される。
	if(hack_gun_fire_critical_flag)
	{
		int chance;
		int tmp2=0;
		tmp = 0;

		if(cheat_xtra) msg_format("flag:%x",hack_gun_fire_critical_flag);
		//近接射撃
		if(hack_gun_fire_critical_flag & GUN_CRIT_CLOSE)
		{
			if( m_ptr->cdis == 1)
				tmp += 100;
			else if( m_ptr->cdis == 2)
				tmp += 50;
		}
		//ヘッドショット
		if((hack_gun_fire_critical_flag & GUN_CRIT_HEAD))
		{
			if(monster_living(r_ptr)) tmp += 200;
			else tmp += 50;
		}
		//トリガーハッピー
		if((hack_gun_fire_critical_flag & GUN_CRIT_TRIGGERHAPPY))
		{
			tmp += 100;
		}
		//精神集中
		if((hack_gun_fire_critical_flag & GUN_CRIT_CONCENT))
		{
				tmp += p_ptr->concent * 10;
		}
		//ノクト+暗闇射撃
		if((hack_gun_fire_critical_flag & GUN_CRIT_NIGHT))
		{
			if(check_mon_blind(c_ptr->m_idx)) tmp += 100;
		}

		//潜伏技能
		if((hack_gun_fire_critical_flag & GUN_CRIT_CONCEAL))
		{
			tmp += 100;
		}
		//魔弾の射手
		if((hack_gun_fire_critical_flag & GUN_CRIT_MAGICBULLET))
		{
			tmp += 200;
		}
		//銃弾変更
		if((hack_gun_fire_critical_flag & GUN_CRIT_SPECIALBULLET))
		{
			switch(special_shooting_mode)
			{

			case SSM_SLAY_ANIMAL:
				if(r_ptr->flags3 & RF3_ANIMAL) tmp += 200; break;
			case SSM_SLAY_NONLIV:
				if(r_ptr->flags3 & RF3_NONLIVING) tmp += 200; break;
			case SSM_SLAY_EVIL:
				if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) tmp += 200; break;
			case SSM_SLAY_HUMAN:
				if(r_ptr->flags3 & RF3_HUMAN) tmp += 400; break;
			case SSM_SLAY_FLYER:
				if(r_ptr->flags7 & RF7_CAN_FLY) tmp += 200; break;
			case SSM_SILVER:
				if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) tmp += 400;
				else if(r_ptr->flags3 & RF3_ANG_CHAOS) tmp += 200;
				break;
			case SSM_HEAVY:
				tmp += 200; break;
			case SSM_SLAY_DRAGON:
				if(r_ptr->flags3 & RF3_DRAGON) tmp += 400; break;
			case SSM_SLAY_DEITY:
				if(r_ptr->flags3 & RF3_DEITY) tmp += 400; break;
			case SSM_NIGHTMARE:
				if(r_ptr->flagsr & RFR_HURT_HELL) tmp += 200; break;
			case SSM_METEOR:
				tmp += 600; break;

			default:
				break;
			}
		}

		if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_5)
			tmp /= 5;
		else if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_3)
			tmp /= 3;
		else if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_2)
			tmp /= 2;

		chance = (tmp + p_ptr->lev * 2 + (p_ptr->to_h_b + hack_gun_fire_critical_hit_bonus) * BTH_PLUS_ADJ);

		if (tmp > 0 && randint1(1000) <= chance)
		{
			tmp2 = randint1(tmp / 3);

			if(tmp2 < tmp / 8) tmp2 = tmp / 8;
			if(tmp2 < 50) tmp2 = 50;

			if(tmp2 < 100)
				msg_print(_("手ごたえがあった！", "It was a good shot!"));
			else if(tmp2 < 200)
				msg_print(_("かなりの手ごたえがあった！", "It was a great shot!"));
			else if(tmp2 < 300)
				msg_print(_("会心の一撃だ！", "It was a superb shot!"));
			else if(tmp2 < 400)
				msg_print(_("最高の会心の一撃だ！", "It was a *GREAT* shot!"));
			else
				msg_print(_("比類なき最高の会心の一撃だ！", "It was a *SUPERB* shot!"));

			dam += dam * tmp2 / 100;

		}
		if(tmp && cheat_xtra) msg_format("tmp:%d chance:%d tmp2:%d dam:%d",tmp,chance,tmp2,dam);

		hack_gun_fire_critical_flag = 0L;
	}

	//v1.1.21
	//銃の技能経験値を得る。
	if(	hack_flag_gain_gun_skill_exp)
	{
		gain_skill_exp(TV_GUN,m_ptr);
		hack_flag_gain_gun_skill_exp = FALSE;
	}


	/* Analyze the damage type */
	switch (typ)
	{
		/* Magic Missile -- pure damage */
		case GF_MISSILE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Acid */
		case GF_ACID:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_ACID)
			{
#ifdef JP
note = "にはかなり耐性がある！";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ACID);
			}
			break;
		}

		/* Electricity */
		case GF_ELEC:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_IM_ELEC)
			{
#ifdef JP
note = "にはかなり耐性がある！";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ELEC);
			}
			else if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
#ifdef JP
note = "はひどい痛手をうけた。";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			break;
		}

		/* Fire damage */
		case GF_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);

			if (r_ptr->flagsr & RFR_IM_FIRE)
			{
#ifdef JP
note = "にはかなり耐性がある！";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
			}
///mod131231 モンスターフラグ変更 火炎弱点RF3からRFRへ
			else if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
#ifdef JP
note = "はひどい痛手をうけた。";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			break;
		}

		/* Cold */
		case GF_COLD:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_COLD && !(WINTER_LETTY2)) //v1.1.85 レティの耐性貫通処理
			{


				note = _("にはかなり耐性がある！", " resists a lot.");
				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_COLD);
			}
///mod131231 モンスターフラグ変更 冷気弱点RF3からRFRへ

			else if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
#ifdef JP
note = "はひどい痛手をうけた。";
#else
				note = " is hit hard.";
#endif
				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			break;
		}

		/* Poison */
		case GF_POIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_POIS)
			{
#ifdef JP
note = "にはかなり耐性がある！";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_POIS);
			}
			break;
		}
		/*:::汚染属性　毒+劣化 それぞれで1/3にしておく*/
		case GF_POLLUTE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_POIS || r_ptr->flagsr & RFR_RES_DISE )
			{
#ifdef JP
note = "にはかなり耐性がある！";
#endif
				if(r_ptr->flagsr & RFR_IM_POIS)
				{
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_POIS);
				}
				if(r_ptr->flagsr & RFR_RES_DISE)
				{
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DISE);
				}
			}
			else
			{
				//v1.1.94 どちらの耐性もなければ防御力低下判定
				if (!mon_saving_throw(m_ptr, dam))
				{
					do_dec_def = 8 + randint1(8);

				}
			}
			break;
		}
		/* Nuclear waste */
		case GF_NUKE:
		{
			int dam_lite = dam / 2 + 1;
			int dam_fire = dam / 2 + 1;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam_fire = oil_field_damage_mod(dam_fire, y, x);

			if (r_ptr->flagsr & RFR_IM_FIRE || r_ptr->flagsr & RFR_RES_LITE)
			{
				note = _("には耐性がある。", " resists.");

				if(r_ptr->flagsr & RFR_IM_FIRE)
				{
					dam_fire /= 9;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
				}
				if(r_ptr->flagsr & RFR_RES_LITE)
				{
					dam_lite = dam_lite * 2 / (randint1(6)+6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");
				dam_lite *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");
				dam_fire *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}

			dam = dam_lite + dam_fire;

			break;
		}
		///mod160110 蒸気属性
		case GF_STEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_FIRE || r_ptr->flagsr & RFR_RES_WATE)
			{
				if (r_ptr->flagsr & RFR_IM_FIRE && r_ptr->flagsr & RFR_RES_WATE)
					note = _("にはかなり耐性がある！", " resists a lot.");
				else
					note = _("には耐性がある。", " resists.");

				if(r_ptr->flagsr & RFR_IM_FIRE){
					dam -= dam/3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
				}
				if(r_ptr->flagsr & RFR_RES_WATE){
					dam -= dam/3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");

				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			if (r_ptr->flagsr & (RFR_HURT_WATER))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");

				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}

			break;
		}
		/* Hellfire -- hurts Evil */
		case GF_REDMAGIC:
		case GF_HELL_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
///mod131231 モンスターフラグ変更 ヘルファイアはフラグによる弱点処理へ
			if (r_ptr->flagsr & RFR_HURT_HELL)
			{
				dam *= 2;
				if(typ == GF_REDMAGIC) note = _("の体は紅い魔力に侵食された！", " is eroded by the red magic!");
				else note = _("の体は地獄の業火によりひどい痛手を受けた。", " is hurt by the hellfire.");

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HELL);
			}
			break;
		}

///mod131231 モンスターフラグ変更 破邪攻撃はフラグによる耐性・弱点処理へ
		/* Holy Fire -- hurts Evil, Good are immune, others _resist_ */
		case GF_HOLY_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_HOLY && r_ptr->flagsr & RFR_HURT_HELL)
			{
				dam = 0;
#ifdef JP
note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_HOLY;
			}
			else if (r_ptr->flagsr & RFR_RES_HOLY)
			{
				dam /= 4;
#ifdef JP
note = "には耐性がある。";
#else
				note = " resists.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_HOLY;
			}
			else if (r_ptr->flagsr & RFR_HURT_HOLY)
			{
				dam *= 2;
#ifdef JP
note = "の体は清浄な力によりひどい痛手を受けた。";
#else
				note = " is hit hard by the purifying energy.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_HURT_HOLY;
			}

			else if(r_ptr->flags3 & RF3_ANG_CHAOS)
			{
#ifdef JP
note = "には少し効果が高いようだ。";
#else
				note = " is hit a bit hard.";
#endif

				dam *= 4; dam /= 3;
			}
			break;
		}

		/* Arrow -- XXX no defense */
		/*:::射撃？*/
		case GF_ARROW:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Plasma -- XXX perhaps check ELEC or FIRE */
		case GF_PLASMA:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);



			if ((r_ptr->flagsr & RFR_IM_FIRE) || (r_ptr->flagsr & RFR_IM_ELEC))
			{
#ifdef JP
note = "には耐性がある。";
#else
				note = " resists.";
#endif
				if(r_ptr->flagsr & RFR_IM_FIRE){
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);

				}
				if(r_ptr->flagsr & RFR_IM_ELEC){
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ELEC);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");
				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = _("はひどい痛手をうけた。", " is hit hard.");
				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}


			break;
		}

		/* Nether -- see above */
		case GF_NETHER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140222 混沌勢力はデフォでは地獄耐性を持たず、代わりに悪魔が持つことにした
			if (r_ptr->flagsr & RFR_RES_NETH)
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
#ifdef JP
					note = "には完全な耐性がある。";
#else
					note = " is immune.";
#endif

					dam = 0;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
				}
				else
				{
#ifdef JP
					note = "には耐性がある。";
#else
					note = " resists.";
#endif

					dam *= 3; dam /= randint1(3) + 6;
				}
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_NETH);
			}
			else if (r_ptr->flags3 & RF3_UNDEAD)
			{
				dam /= 4;
#ifdef JP
				note = "にはかなり耐性がある。";
#else
				note = " resists a lot.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
			}
			else if (r_ptr->flags3 & RF3_DEMON)
			{
				dam /= 2;
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists somewhat.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}
			break;
		}

		//v2.0.2 聖水属性　水と破邪の複合属性
		//今のところ久侘歌の「命の分水嶺」専用
		case GF_HOLY_WATER:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flagsr & RFR_HURT_HOLY && r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = _("はひどい痛手を受けた！", " is severely wounded!");
				dam *= 3;

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);

			}
			else if (r_ptr->flagsr & RFR_HURT_HOLY)
			{
				note = _("は聖なる水を浴びて身をすくめた。", " shudders after being doused in holy water.");
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
				dam *= 2;
			}
			else if (r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = _("は水が苦手なようだ。", " doesn't like water.");
				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}

			else if (r_ptr->flagsr & (RFR_RES_WATE | RFR_RES_HOLY))
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (r_ptr->flagsr & (RFR_RES_WATE) && is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
				if (r_ptr->flagsr & (RFR_RES_HOLY) && is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_HOLY);

			}

			if (dam > randint1(r_ptr->level * 3) && !(r_ptr->flagsr & (RFR_RES_WATE|RFR_RES_HOLY)) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

		}
		break;

		/* Water (acid) damage -- Water spirits/elementals are immune */
		case GF_WATER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mon 水属性への一部モンスター特殊処理
			if (r_ptr->flagsr & RFR_RES_WATE)
			{
				if ((m_ptr->r_idx == MON_WATER_ELEM) || (m_ptr->r_idx == MON_UNMAKER) || (m_ptr->r_idx == MON_MURASA))
				{
#ifdef JP
					note = "には完全な耐性がある。";
#else
					note = " is immune.";
#endif

					dam = 0;
				}
				else
				{
#ifdef JP
					note = "には耐性がある。";
#else
					note = " resists.";
#endif

					dam *= 3; dam /= randint1(6) + 6;
				}
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
			}
			else if(r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = _("は水が苦手なようだ！", " does not like water!");
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
				dam *= 2;
			}
			if(dam > randint1(r_ptr->level*3) && !(r_ptr->flagsr & RFR_RES_WATE) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

			break;
		}
		//海が割れる日専用　基本水だが水棲系に大ダメージ
		case GF_MOSES:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mon 水属性への一部モンスター特殊処理
			else if (r_ptr->flags7 & RF7_AQUATIC)
			{
					note = _("は地面の上でもがいている！", " struggles on the ground!");
					dam *= 2;
			}
			else if (r_ptr->flagsr & RFR_RES_WATE)
			{
				note = _("には耐性がある。", " resists.");
				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
			}
			else if(r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = _("は水が苦手なようだ！", " does not like water!");
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
				dam *= 2;
			}

			if(dam > randint1(r_ptr->level*3) && !(r_ptr->flagsr & RFR_RES_WATE) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

			break;
		}


		/* Chaos -- Chaos breathers resist */
		case GF_CHAOS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_CHAO)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_CHAO);
			}
			else if ((r_ptr->flags3 & RF3_DEMON) && one_in_(3))
			{
#ifdef JP
				note = "はいくらか耐性を示した。";
#else
				note = " resists somewhat.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}
			else
			{
				do_poly = TRUE;
				do_conf = (5 + randint1(11) + r) / (r + 1);
			}
			break;
		}


		case GF_TIMED_SHARD:

		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;

			}
			else
			{
#ifdef JP
				note = "に管狐弾を食らわせた。";
#else
				note = " is hit by the kuda-gitsune bullets.";
#endif
			}
			//10行動の間ダメージを与える
			m_ptr->timed_shard_count = 10;
			get_angry = TRUE;

			dam = 0;

			break;
		}


		/* Shards -- Shard breathers resist */
		case GF_SHARDS:
		case GF_THROW_PLATE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;

			}
			//v1.1.64 ハニワは轟音破片弱点にしてみる
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = _("は身震いした。", " shudders.");
				dam = dam * 2;

			}

			//v1.1.94 破片耐性がないとき追加効果で攻撃力低下(ダメージ値を判定パワーとしてセービングスロー)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam/10);
			}

			break;
		}

		/* Rocket: Shard resistance helps */
		case GF_ROCKET:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "はいくらか耐性を示した。";
#else
				note = " resists somewhat.";
#endif

				dam /= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;
			}
			//v1.1.64 ハニワは轟音破片弱点にしてみる
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = _("は身震いした。", " shudders.");
				dam = dam * 2;

			}

			//v1.1.94 破片耐性がないとき追加効果で攻撃力低下(ダメージ値を判定パワーとしてセービングスロー)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam / 10);
			}

			break;
		}


		/* Sound -- Sound breathers resist */
		case GF_SOUND:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SOUN)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SOUN);
			}
			//v1.1.64 ハニワは轟音弱点にしてみる
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = _("は身震いした。", " shudders.");
				dam *= 2;
				do_stun = (20 + randint1(30) + r) / (r + 1);
			}
			else
				do_stun = (10 + randint1(15) + r) / (r + 1);

			break;
		}

		/* Confusion */
		case GF_CONFUSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags3 & RF3_NO_CONF)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
			}
			else
			{
				do_conf = (10 + randint1(15) + r) / (r + 1);
				do_dec_mag = 8 + randint1(dam/10);
			}
			break;
		}

		/* Disenchantment -- Breathers and Disenchanters resist */
		case GF_DISENCHANT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_DISE)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DISE);
				break;
			}

			//v1.1.94 劣化耐性がないとき追加効果で防御力低下(ダメージ値を判定パワーとしてセービングスロー)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_def = 8 + randint1(dam/10);
			}



			break;
		}

		/* Nexus -- Breathers and Existers resist */
		case GF_NEXUS:
		case GF_SEIRAN_BEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140914 モンスターの因果混乱耐性　少し変更
			//耐性がない時HP最大値が減ったり消滅したりテレポするよう変更
			if (r_ptr->flagsr & RFR_RES_TIME)
			{
				note = _("はいくらか耐性を示した。", " resists somewhat.");

				dam *= 4; dam /= randint1(3) + 5;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
				break;
			}
			else if(typ == GF_NEXUS)
			{
				do_time = (dam + 1) / 2;
				//抹殺処理　ダメージ回避やメッセージ回避のためplayer_castをFALSEにして呼ぶ
				if ((dam <= m_ptr->hp) && (r_ptr->level < randint1(200)) && genocide_aux(c_ptr->m_idx, 200, FALSE, (r_ptr->level + 1) / 2,
                                                                             _("モンスター消滅", "monster disappearance")))
				{
					if (seen_msg) msg_format(_("%sはこのフロアにいなかったことになった。",
                                                "%s never was on this level."), m_name);
					return TRUE;
				}
				else if((dam <= m_ptr->hp) && !(r_ptr->flagsr & RFR_RES_TELE))
				{
					do_dist = randint1(200);
					if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
				}

				//v1.1.94 因果混乱の追加効果で判定なしの全能力低下
				do_dec_atk = 16 + randint1(dam / 10);
				do_dec_def = 16 + randint1(dam / 10);
				do_dec_mag = 16 + randint1(dam / 10);

			}
			break;

		}

		/* Force */
		case GF_FORCE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod131231 フォース耐性削除　NO_STUNがあれば朦朧としないことにした
/*
			if (r_ptr->flagsr & RFR_RES_WALL)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WALL);
			}
*/
			if (!(r_ptr->flags3 & RF3_NO_STUN)) do_stun = (randint1(15) + r) / (r + 1);
			break;
		}

		/* Inertia -- breathers resist */
		/*:::遅鈍属性*/
		case GF_INACT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_INER)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_INER);
			}
			else
			{
				/* Powerful monsters can resist */
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				//v1.1.90 判定変更
				if (mon_saving_throw(m_ptr, dam))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}
			break;
		}

		/* Time -- breathers resist */
		case GF_TIME:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_TIME)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
			}
			else
			{
				do_time = (dam + 1) / 2;

				//v1.1.94 時間逆転の追加効果で全能力低下(要セービングスロー)
				if (!mon_saving_throw(m_ptr, dam))
				{
					do_dec_atk = 8 + randint1(dam / 10);
					do_dec_def = 8 + randint1(dam / 10);
					do_dec_mag = 8 + randint1(dam / 10);
				}

			}
			break;
		}
		///mod 空間歪曲属性 耐性ないとテレポ
		case GF_DISTORTION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_TIME)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
			}
			else if(!(r_ptr->flagsr & RFR_RES_TELE))
			{
				//v1.1.55 闘技場で空間歪曲やったときモンスターが消える？のか知らんが念の為値小さくしとく
				if (p_ptr->inside_battle)
					do_dist = 10;
				else
					do_dist = randint1(100);

				if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
			}
			break;
		}

		/* Gravity -- breathers resist */
		//モンスターへの重力属性
		case GF_GRAVITY:
		{
			bool resist_tele = FALSE;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_TELE)
			{
				if (r_ptr->flags1 & (RF1_UNIQUE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "は動かされなかった。";
#else
					note = " is unaffected!";
#endif

					resist_tele = TRUE;
				}
				else if (r_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "には耐性がある！";
#else
					note = " resists!";
#endif

					resist_tele = TRUE;
				}
			}

			if (!resist_tele) do_dist = 10;
			else do_dist = 0;
			if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;

			if (r_ptr->flagsr & RFR_RES_TIME || r_ptr->flags7 & RF7_CAN_FLY)
			{
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				do_dist = 0;
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_GRAV);
			}
			else
			{
				/*:::時空耐性も飛行もない巨大なモンスターには重力がよく効くことにしてみる*/
				if(r_ptr->flags2 & (RF2_GIGANTIC)){
					msg_format(_("%^sの巨大な体が激しく地面に叩きつけられた！",
                                "The giant body of %s gets violently smashed into the ground!"), m_name);
					dam *= 2;
				}

				/* 1. slowness */
				/* Powerful monsters can resist */
				//v1.1.90 判定変更
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))

				if (mon_saving_throw(m_ptr, dam))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}

				/* 2. stun */
				do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				if (mon_saving_throw(m_ptr, dam))
				{
					/* Resist */
					do_stun = 0;
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった。";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}



			}
			break;
		}

		///mod140103 竜巻属性　巨大なモンスターには効かない　浮遊でダメージ軽減
		//テレポ耐性は関係なしにする。裂風剣でも無視してることだし。
		case GF_TORNADO:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
#ifdef JP
				note = "の巨大な体には効果が薄いようだ。";
#else
                note = " is affected very little.";
#endif
				dam /= 8;
				break;
			}

			do_dist = 10;
			if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;

			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				note = _("は吹き飛ばされたが空中でバランスを取った。",
                        " gets blown away, but regain balance in the air.");
				dam  /= (randint1(3) + 1);
			}
			else note = _("は吹き飛ばされた！", " gets blown away!");

			break;
		}

		///mod14502 電車属性　敵をテレポ耐性無視して吹き飛ばすが巨大な敵は飛ばない
		case GF_TRAIN:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_SHINMYOU_2)
					note = "は重くて釣り上げられなかった。";
				else
					note = "の巨大な体は吹き飛ばなかった。";
#else
                if(p_ptr->pclass == CLASS_SHINMYOU_2)
					note = " is too heavy for you.";
				else
					note = " did not get thrown away.";
#endif
				break;
			}
			else if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
			else
			{
				if(p_ptr->pclass == CLASS_SHINMYOU_2)
					note = _("を釣り上げた！", " gets snared!");
				else
					note = _("はどこかへ跳ね飛ばされた！", " gets thrown aside to somewhere!");

				do_dist = 10 + randint1(20);
			}
			break;
		}

		/* Pure damage */
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/*:::レティのノーザンウィナー*/
		case GF_NORTHERN:
		{
			int msleep = MON_CSLEEP(m_ptr);
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			else if(r_ptr->flagsr & RFR_IM_COLD && !WINTER_LETTY2)//v1.1.85 レティ耐性貫通処理
			{
				note = _("はいくらか耐性を示した。", " resists somewhat.");
				dam = dam * 3 / (5 + randint1(4));
			}

			if (msleep)
			{
				dam *= 2;
			}


			if(msleep && m_ptr->hp >= dam &&
				( ((r_ptr->flagsr & RFR_IM_COLD) || !monster_living(r_ptr) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ) &&  (r_ptr->level > randint1(msleep / 3 + p_ptr->lev * 2))  || (r_ptr->level > randint1(msleep + p_ptr->lev * 5))  ) )
			{
				note = _("を起こしてしまった！", " wakes up!");
			}

			else if(msleep)
			{
				if(msleep < 2) msleep = 2;
				note = _("はまだ眠っている・・", " is still sleeping...");
				do_sleep = msleep / 2 + randint1(msleep / 2);
			}
			note_dies = _("は凍りつき、動かなくなった。", " is frozen still and can't move.");

			break;
		}


		/* Pure damage */
		case GF_DISINTEGRATE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod131231 モンスターフラグ変更 岩石妖怪弱点RF3からRFRへ
			if (r_ptr->flagsr & RFR_HURT_ROCK)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);
#ifdef JP
note = "の皮膚がただれた！";
note_dies = "は蒸発した！";

			if(is_gen_unique(m_ptr->r_idx)) note_dies = "は目を回して倒れた。";

#else
				note = " loses some skin!";
				note_dies = " evaporates!";

			if(is_gen_unique(m_ptr->r_idx)) note_dies = " spins around and collapses.";

#endif

				dam *= 2;
			}
			break;
		}
		/*:::精神攻撃*/
		case GF_PSI:
		{
			if (seen) obvious = TRUE;

			/* PSI only works if the monster can see you! -- RG */
			if (!(los(m_ptr->fy, m_ptr->fx, py, px)))
			{
#ifdef JP
				if (seen_msg) msg_format("%sはあなたが見えないので影響されない！", m_name);
#else
				if (seen_msg) msg_format("%^s can't see you, and isn't affected!", m_name);
#endif
				skipped = TRUE;
				break;
			}

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;

			}
			else if ((r_ptr->flags2 & (RF2_STUPID | RF2_WEIRD_MIND)) ||
			         (r_ptr->flags3 & RF3_ANIMAL) ||
			         (r_ptr->level > randint1(3 * dam)))
			{
				dam /= 3;
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif


				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				    (r_ptr->level > p_ptr->lev / 2) &&
				    one_in_(2))
				{
					note = NULL;
#ifdef JP
					msg_format("%^sの堕落した精神は攻撃を跳ね返した！", m_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					if ((randint0(100 + r_ptr->level / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
					{
#ifdef JP
						msg_print("しかし効力を跳ね返した！");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Injure +/- confusion */
						monster_desc(killer, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
						take_hit(DAMAGE_ATTACK, dam, killer, -1);  /* has already been /3 */
						if (one_in_(4) && !CHECK_MULTISHADOW())
						{
							switch (randint1(4))
							{
								case 1:
									set_confused(p_ptr->confused + 3 + randint1(dam));
									break;
								case 2:
									set_stun(p_ptr->stun + randint1(dam));
									break;
								case 3:
								{
									if (r_ptr->flags3 & RF3_NO_FEAR)
#ifdef JP
										note = "には効果がなかった。";
#else
										note = " is unaffected!";
#endif

									else
										set_afraid(p_ptr->afraid + 3 + randint1(dam));
									break;
								}
								default:
									if (!p_ptr->free_act)
										(void)set_paralyzed(p_ptr->paralyzed + randint1(dam));
									break;
							}
						}
					}
					dam = 0;
				}
				break;
			}

			//v1.1.94 精神攻撃系の追加効果で魔法低下
			do_dec_mag = 8 + randint1(dam / 10);

			if ((dam > 0) && one_in_(4))
			{
				switch (randint1(4))
				{
					case 1:
						do_conf = 3 + randint1(dam);
						break;
					case 2:
						do_stun = 3 + randint1(dam);
						break;
					case 3:
						do_fear = 3 + randint1(dam);
						break;
					default:
#ifdef JP
						note = "は眠り込んでしまった！";
#else
						note = " falls asleep!";
#endif

						do_sleep = 3 + randint1(dam);
						break;
				}
			}

#ifdef JP
			///msg dead
			//note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
			note_dies = "は倒れた。";
#else
			note_dies = " is defeated.";
#endif

			break;
		}

		case GF_PSI_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				break;
			}
			/*
			* Powerful demons & undead can turn a mindcrafter's
			* attacks back on them
			*/
			else if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				(r_ptr->level > p_ptr->lev / 2) && (one_in_(2)))
			{
				note = NULL;
				msg_format(_("%^sの堕落した精神は攻撃を跳ね返した！",
                            "%^s delivers a mental backlash!"), m_name);

				/* Saving throw */
				if ((randint0(100 + r_ptr->level / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
				{
					msg_print(_("あなたは効力を跳ね返した！", "You resist the effects!"));
				}
				else
				{
					/* Injure + mana drain */
					monster_desc(killer, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
					if (!CHECK_MULTISHADOW())
					{
						msg_print(_("逆に魔力を吸いとられてしまった！", "Your mana was drained instead!"));

						p_ptr->csp -= dam;
						if (p_ptr->csp < 0) p_ptr->csp = 0;
						p_ptr->redraw |= PR_MANA;
						p_ptr->window |= (PW_SPELL);
					}
					take_hit(DAMAGE_ATTACK, dam / 3, killer, -1);
				}
				dam = 0;
				break;
			}
			else if (r_ptr->flags2 & (RF2_STUPID | RF2_WEIRD_MIND))
			{
				dam /= 3;
				note = _("には耐性がある。", " resists.");
				break;
			}
			//魔法を全く持たないモンスターはMPを持たないと解釈し、MPを吸収できない。
			else if (!HAS_ANY_MAGIC(r_ptr))
			{
				note = _("は魔力を持たないようだ。", " doesn't have mana.");
			}
			else if(r_ptr->level > randint1(2 * dam))
			{
				dam /= 2;
				note = _("は攻撃に抵抗した！", " resists the attack!");
				break;
			}
			else if (dam > 0)
			{
				int b = (dam + 1) / 2 + randint1(dam + 1) / 2;
				msg_format(_("%sから魔力を吸い取った！", "You drain mana from %s!"), m_name);

				player_gain_mana(b);

			}
			note_dies = _("は倒れた。", " is defeated.");

			//v1.1.94 精神攻撃系の追加効果で魔法低下
			do_dec_mag = 8 + randint1(dam / 10);


		}
		break;
		case GF_TELEKINESIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (one_in_(4))
			{
				if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
				else do_dist = 7;
			}

			/* 1. stun */
			do_stun = damroll((caster_lev / 20) + 3 , dam) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->level > 5 + randint1(dam)))
			{
				/* Resist */
				do_stun = 0;
				/* No obvious effect */
				obvious = FALSE;
			}
			break;
		}

		case GF_BLAZINGSTAR:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_dist = 100;
			break;
		}

		/* Psycho-spear -- powerful magic missile */
		///mod150305 グングニル追加
		case GF_GUNGNIR:
		case GF_PSY_SPEAR:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Meteor -- powerful magic missile */
		case GF_METEOR:
		case GF_KANAMEISHI:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		case GF_DOMINATION:
		{
			if (!is_hostile(m_ptr)) break;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) ||
			    (r_ptr->flags3 & RF3_NO_CONF) ||
			    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				    (r_ptr->level > p_ptr->lev / 2) &&
				    (one_in_(2)))
				{
					note = NULL;
#ifdef JP
					msg_format("%^sの堕落した精神は攻撃を跳ね返した！", m_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					if (randint0(100 + r_ptr->level/2) < p_ptr->skill_sav)
					{
#ifdef JP
						msg_print("しかし効力を跳ね返した！");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Confuse, stun, terrify */
						switch (randint1(4))
						{
							case 1:
								set_stun(p_ptr->stun + dam / 2);
								break;
							case 2:
								set_confused(p_ptr->confused + dam / 2);
								break;
							default:
							{
								if (r_ptr->flags3 & RF3_NO_FEAR)
#ifdef JP
									note = "には効果がなかった。";
#else
									note = " is unaffected!";
#endif

								else
									set_afraid(p_ptr->afraid + dam);
							}
						}
					}
				}
				else
				{
					/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}
			else
			{
				if ((dam > 29) && (randint1(100) < dam))
				{
#ifdef JP
					///sys　隷属→従った
note = "はあなたに従った。";
#else
					note = " is now your follower.";
#endif
					if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -2);
					set_pet(m_ptr);
				}
				else
				{
					switch (randint1(4))
					{
						case 1:
							do_stun = dam / 2;
							break;
						case 2:
							do_conf = dam / 2;
							break;
						default:
							do_fear = dam;
					}
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}



		/* Ice -- Cold + Cuts + Stun */
		case GF_ICE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_stun = (randint1(15) + 1) / (r + 1);
			if (r_ptr->flagsr & RFR_IM_COLD && !(WINTER_LETTY2))//v1.1.85 レティ耐性貫通処理
			{
#ifdef JP
				note = "にはかなり耐性がある。";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_COLD);
			}
///mod131231 モンスターフラグ変更 冷気弱点RF3からRFRへ

			else if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
#ifdef JP
				note = "はひどい痛手をうけた。";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			break;
		}


		/* Drain Life */
		case GF_OLD_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (!monster_living(r_ptr))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}

#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else do_time = (dam+7)/8;

			break;
		}

		/* Death Ray */
		case GF_DEATH_RAY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (!monster_living(r_ptr))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}

#ifdef JP
				note = "には完全な耐性がある。";
#else
				note = " is immune.";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
			}
			//v1.1.60赤蛮奇の頭に死の光線耐性付与
			else if (m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2))
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
			}

/*
			else if (((r_ptr->flags1 & RF1_UNIQUE) &&
				 (randint1(888) != 666)) ||
				 (((r_ptr->level + randint1(20)) > randint1((caster_lev / 2) + randint1(10))) &&
				 randint1(100) != 66))
 */
			else if (randint0(randint1(r_ptr->level/7)+5))
			{
				note = _("は死の光線に抵抗した！", " resists the death ray!");

				obvious = FALSE;
				dam = 0;
			}

			break;
		}

		/* Polymorph monster (Use "dam" as "power") */
		///mod140211 チェンジモンスターはカオス耐性持ちには効かなくした
		case GF_OLD_POLY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful monsters can resist */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flagsr & RFR_RES_CHAO) ||
			    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
note = "には効果がなかった。";
#else
				note = " is unaffected!";
#endif

				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Clone monsters (Ignore "dam") */
		case GF_OLD_CLONE:
		{
			if (seen) obvious = TRUE;

			if ((p_ptr->inside_arena) || is_pet(m_ptr) || (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) || (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2)))
			{
#ifdef JP
note = "には効果がなかった。";
#else
 note = " is unaffected!";
#endif
			}
			else
			{
				/* Heal fully */
				m_ptr->hp = m_ptr->maxhp;

				/* Attempt to clone. */
				if (multiply_monster(c_ptr->m_idx, TRUE, 0L))
				{
#ifdef JP
					///sys クローンモンスター　メッセージ変える？
note = "が分裂した！";
#else
					note = " spawns!";
#endif

				}
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Heal Monster (use "dam" as amount of healing) */
		case GF_STAR_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			(void)set_monster_csleep(c_ptr->m_idx, 0);

			if (m_ptr->maxhp < m_ptr->max_maxhp)
			{
#ifdef JP
				if (seen_msg) msg_format("%^sの強さが戻った。", m_name);
#else
				if (seen_msg) msg_format("%^s recovers %s vitality.", m_name, m_poss);
#endif
				m_ptr->maxhp = m_ptr->max_maxhp;
			}

			if (!dam)
			{
				/* Redraw (later) if needed */
				if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
				if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);
				break;
			}

			/* Fall through */
		}
		case GF_OLD_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			(void)set_monster_csleep(c_ptr->m_idx, 0);
			if (MON_STUNNED(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは朦朧状態から立ち直った。", m_name);
#else
				if (seen_msg) msg_format("%^s is no longer stunned.", m_name);
#endif
				(void)set_monster_stunned(c_ptr->m_idx, 0);
			}
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは混乱から立ち直った。", m_name);
#else
				if (seen_msg) msg_format("%^s is no longer confused.", m_name);
#endif
				(void)set_monster_confused(c_ptr->m_idx, 0);
			}
			if (MON_MONFEAR(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^sは勇気を取り戻した。", m_name);
#else
				if (seen_msg) msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
				(void)set_monster_monfear(c_ptr->m_idx, 0);
			}

			/* Heal */
			///sys monHP30000制限？
			if (m_ptr->hp < 30000) m_ptr->hp += dam;

			/* No overflow */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
///del131231 virtue
#if 0
			///sysdel mon virtue
			if (!who)
			{
				chg_virtue(V_VITALITY, 1);

				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_INDIVIDUALISM, 1);

				if (is_friendly(m_ptr))
					chg_virtue(V_HONOUR, 1);
				else if (!(r_ptr->flags3 & RF3_EVIL))
				{
					if (r_ptr->flags3 & RF3_GOOD)
						chg_virtue(V_COMPASSION, 2);
					else
						chg_virtue(V_COMPASSION, 1);
				}

				if (r_ptr->flags3 & RF3_ANIMAL)
					chg_virtue(V_NATURE, 1);
			}

			if (m_ptr->r_idx == MON_LEPER)
			{
				heal_leper = TRUE;
				if (!who) chg_virtue(V_COMPASSION, 5);
			}
#endif
			/* Redraw (later) if needed */
			if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Message */
#ifdef JP
			note = "は体力を回復したようだ。";
#else
			note = " looks healthier.";
#endif

			//野良神様　配下や友好モンスターをHP半分以下から回復させると名声などを得る
			if(p_ptr->prace == RACE_STRAYGOD && !(m_ptr->mflag & MFLAG_SPECIAL2) && (m_ptr->hp < m_ptr->max_maxhp / 2))
			{
				m_ptr->mflag |= MFLAG_SPECIAL2;
				if(is_pet(m_ptr))
				{
					set_deity_bias(DBIAS_WARLIKE, -1);
				}
				else if(is_friendly(m_ptr))
				{
					set_deity_bias(DBIAS_REPUTATION, 2);
				}
			}


			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Speed Monster (Ignore "dam") */
		case GF_OLD_SPEED:
		{
			if (seen) obvious = TRUE;

			/* Speed up */
			if (set_monster_fast(c_ptr->m_idx, MON_FAST(m_ptr) + 100))
			{
#ifdef JP
				note = "の動きが速くなった。";
#else
				note = " starts moving faster.";
#endif
			}

			if (!who)
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_INDIVIDUALISM, 1);
				if (is_friendly(m_ptr))
					chg_virtue(V_HONOUR, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Slow Monster (Use "dam" as "power") */
		case GF_OLD_SLOW:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//v1.1.90 判定変更
			/* Powerful monsters can resist */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))

			if (r_ptr->flagsr & (RFR_RES_INER) || mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* Normal monsters slow down */
			else
			{
				if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
				{
#ifdef JP
					note = "の動きが遅くなった。";
#else
					note = " starts moving slower.";
#endif
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/*:::足絡めなどのスロウモンスター　地形やモンスターの種類により無効処理*/
		case GF_SLOW_TWINE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
                note = " is unaffected!";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if ((r_ptr->flags2 & RF2_PASS_WALL) || (r_ptr->flags7 & RF7_CAN_FLY) )
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}

			if(r_ptr->flags1 & RF1_UNIQUE) dam /= 2;
			if(r_ptr->flags2 & RF2_KILL_WALL) dam /= 2;
			if(r_ptr->flags8 & RF8_WILD_WOOD) dam/= 2;
			if(r_ptr->flags2 & RF2_POWERFUL) dam/= 2;
			if(r_ptr->flags2 & RF2_GIGANTIC)dam/=2;

			if(r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
			{
#ifdef JP
				note = "は枷を振り解いた！";
#else
                note = " brushes off the entangling!";
#endif
				obvious = FALSE;
			}
			else
			{
				if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
				{
#ifdef JP
					note = "の動きが遅くなった。";
#else
                    note = " starts moving slower.";
#endif
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power") */
		case GF_OLD_SLEEP:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->flags3 & RF3_NO_SLEEP) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 判定変更
			if (r_ptr->flags3 & (RF3_NO_SLEEP) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_SLEEP)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
				}

				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
note = "は眠り込んでしまった！";
#else
				note = " falls asleep!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Sleep (Use "dam" as "power") */
		/*:::封魔（破邪領域）*/
		case GF_STASIS_EVIL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    !(r_ptr->flags3 & RF3_ANG_CHAOS) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 判定変更
			if (!(r_ptr->flags3 & RF3_ANG_CHAOS) || mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
note = "は動けなくなった！";
#else
				note = " is suspended!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power") */
		/*:::神威、幻惑、印籠などのモンスター停止処理*/
		case GF_STASIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 判定変更
			if (mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */

				note = _("は動けなくなった！", " is suspended!");

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/*:::ドラゴン子守唄*/
		case GF_STASIS_DRAGON:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (!(r_ptr->flags3 & RF3_DRAGON))
			{
				obvious = FALSE;
			}
			else if (r_ptr->flags1 & RF1_UNIQUE)
			{
				note = _("は眠らなかった！", " does not fall asleep!");
				obvious = FALSE;
			}
			else
			{
				note = _("はその場にうずくまり、眠り始めた。", " curls up on the spot and sleeps.");
				do_sleep = 100;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.34 成美生命操作　妖精に無条件で効く
		case GF_STASIS_LIVING:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (!monster_living(r_ptr))
			{
				note = _("には効果がなかった！", " is unaffected!");
			}
			else if (r_ptr->flags3 & RF3_FAIRY)
			{
				note = _("の動きが止まった。", " stops moving.");
				do_sleep = 500;
				(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8));
			}
			else
			{
				//v1.1.90 判定変更
				if (mon_saving_throw(m_ptr, dam))
				{
					note = _("には効果がなかった！", " is unaffected!");
				}
				else
				{
					if(!(r_ptr->flagsr & RFR_RES_INER))
					{
						set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8));
					}
					do_sleep = 100+randint1(dam);
					note = _("は動けなくなった！", " has stopped moving!");

				}

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Charm monster */
		case GF_CHARM:
		case GF_CHARM_PALADIN:
		{
			//int vir;
			dam += (adj_chr_chm[p_ptr->stat_ind[A_CHR]] - 1);
			//vir = virtue_number(V_HARMONY);
			//if (vir)
			//{
			//	dam += p_ptr->virtues[vir-1]/10;
			//}

			//vir = virtue_number(V_INDIVIDUALISM);
			//if (vir)
			//{
			//	dam -= p_ptr->virtues[vir-1]/20;
			//}

			///mod140817 配下にはチャームが効かないことにする
			if(is_pet(m_ptr)) return (FALSE);

			if (seen) obvious = TRUE;

			if(typ == GF_CHARM_PALADIN && (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				note = _("は説得に耳を貸さない。", " does not listen to the lecture.");
				dam = 0;
				break;
			}
			if(typ == GF_CHARM_PALADIN && (m_ptr->mflag2 & MFLAG2_NOPET))
			{
				note = _("は説得に応じない。", " does not respond to the lecture.");
				dam = 0;
				break;
			}
			else if(typ == GF_CHARM_PALADIN && (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)))
			{
				note = _("は無反応だ。", " does not react.");
				dam = 0;
				break;
			}

			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			//v2.0.2 条件式を他のバステ系に揃えた。GF_CHARMが混乱耐性のある敵に効かないのは従来同様
			//if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
			//	dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flags3 & RF3_NO_CONF) && typ == GF_CHARM ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				mon_saving_throw(m_ptr, dam)   )
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
				if(typ == GF_CHARM_PALADIN)
				note = "の説得に失敗した。";
				else note = "には効果がなかった！";
#else
				if(typ == GF_CHARM_PALADIN) note = " disregards the lecture.";
				else note = " is unaffected!";
#endif

				obvious = FALSE;

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
				if(typ == GF_CHARM_PALADIN)
				note = "の説得に成功した！";

				else note = "は突然友好的になったようだ！";
#else
				if(typ == GF_CHARM_PALADIN) note = " is touched by the lecture!";
				else note = " suddenly seems friendly!";
#endif

				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);

				//chg_virtue(V_INDIVIDUALISM, -1);
				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Control undead */
		case GF_CONTROL_UNDEAD:
		{
			int vir;
			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			  (!(r_ptr->flags3 & RF3_UNDEAD)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
				///sys 隷属メッセージ変更予定
note = "はあなたに服従した。";
#else
				note = " now obeys you.";
#endif

				if(r_ptr->level+10 > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Control demon */
		case GF_CONTROL_DEMON:
		{
			int vir;
			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			  (!(r_ptr->flags3 & RF3_DEMON)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
			///sys 隷属メッセージ変更予定
note = "はあなたに服従した。";
#else
				note = " now obeys you.";
#endif

				if(r_ptr->level+10 > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_CHARM_C:
		case GF_CHARM_J:
		case GF_CHARM_B:
		case GF_CONTROL_FISH:
		{

			if (seen) obvious = TRUE;
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR))
				|| (m_ptr->mflag2 & MFLAG2_NOPET)

				|| typ == GF_CONTROL_FISH && !(r_ptr->flags7 & RF7_AQUATIC)

				|| typ == GF_CHARM_C && r_ptr->d_char != 'C'
				|| typ == GF_CHARM_B && r_ptr->d_char != 'B'
				|| typ == GF_CHARM_J && r_ptr->d_char != 'J')
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (r_ptr->level > randint1(dam))
			{
				note = _("には効果がなかった！", " is unaffected!");

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
				note = _("はあなたに敵意を抱いている！", " hates you too much!");

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
				note = _("はなついた。", " is tamed.");
				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}
		case GF_CONTROL_ANIMAL:
		{
			int vir;

			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_NATURE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) ||
			  (!(r_ptr->flags3 & (RF3_ANIMAL))) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->flags3 & (RF3_NO_CONF)) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
note = "はなついた。";
#else
				note = " is tamed!";
#endif

				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);

				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Tame animal */
		case GF_CONTROL_LIVING:
		{
			int vir;

			//vir = virtue_number(V_UNLIFE);
			if (seen) obvious = TRUE;

			dam += (adj_chr_chm[p_ptr->stat_ind[A_CHR]]);
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if (r_ptr->flags3 & (RF3_NO_CONF)) dam -= 30;
			if (dam < 1) dam = 1;
#ifdef JP
msg_format("%sを見つめた。",m_name);
#else
			msg_format("You stare into %s.", m_name);
#endif
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 !monster_living(r_ptr) ||
				 ((r_ptr->level+10) > randint1(dam)))
			{
				/* Resist */
				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "はあなたに敵意を抱いている！";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
		///sys 隷属メッセージ変更予定
note = "はあなたに服従した。";
#else
				note = " now obeys you.";
#endif

				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);

				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Confusion (Use "dam" as "power") */
		case GF_OLD_CONF:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Get confused later */
			do_conf = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			//    (r_ptr->flags3 & (RF3_NO_CONF)) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 判定変更
			if (r_ptr->flags3 & (RF3_NO_CONF) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				//v1.1.94 精神攻撃系の追加効果で魔法低下
				do_dec_mag = 8 + randint1(8);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_STUN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

			/* Attempt a saving throw */
			//v1.1.90 判定変更
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			if(r_ptr->flags3 & (RF3_NO_STUN) || mon_saving_throw(m_ptr,dam))
			{
				/* Resist */
				do_stun = 0;

				/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}




		/* Lite, but only hurts susceptible creatures */
		case GF_LITE_WEAK:
		{
			if (!dam)
			{
				skipped = TRUE;
				break;
			}
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				dam = 0;
				break;
			}
			/* Hurt by light */
///mod131231 モンスターフラグ変更 閃光弱点RF3からRFRへ
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);

				/* Special effect */
#ifdef JP
		///sysdel dead
note = "は光に身をすくめた！";
note_dies = "は光を受けて倒れた！";
#else
				note = " cringes from the light!";
				note_dies = " collapses from the light!";
#endif

			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}



		/* Lite -- opposite of Dark */
		case GF_LITE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_LITE)
			{
#ifdef JP
				note = "には耐性がある！";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
			}
			///mod131231 モンスターフラグ変更 閃光弱点RF3からRFRへ
			else if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
#ifdef JP
						///sysdel dead
				note = "は光に身をすくめた！";
				note_dies = "は光を受けて倒れた！";
#else
				note = " cringes from the light!";
				note_dies = " collapses from the light!";
#endif

				dam *= 2;
			}
			break;
		}


		/* Dark -- opposite of Lite */
		/*:::デーモンは暗黒攻撃に耐性を持つようにしておく*/
		case GF_DARK:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_DARK)
			{
#ifdef JP
				note = "には耐性がある！";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DARK);
			}
			if (r_ptr->flags3 & RF3_DEMON)
			{
#ifdef JP
				note = "はいくらか耐性を示した。";
#else
				note = " resists.";
#endif

				dam /= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}


			break;
		}


		/* Stone to Mud */
		case GF_KILL_WALL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				dam = 0;
				break;
			}
			/* Hurt by rock remover */
			///mod131231 モンスターフラグ変更 岩石妖怪弱点RF3からRFRへ
			if (r_ptr->flagsr & (RFR_HURT_ROCK))
			{
				/* Notice effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);

				/* Cute little message */
#ifdef JP
			///sysdel dead
note = "の皮膚がただれた！";
note_dies = "は溶けた！";
if (is_gen_unique(m_ptr->r_idx)) note_dies = "は目を回して倒れた。";

#else
				note = " loses some skin!";
				note_dies = " dissolves!";
if (is_gen_unique(m_ptr->r_idx)) note_dies = " spins around and collapses.";

#endif

			}

			/* Usually, ignore the effects */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Teleport undead (Use "dam" as "power") */
		case GF_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "には効果がなかった！";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "には耐性がある！";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport evil (Use "dam" as "power") */
		case GF_AWAY_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "には効果がなかった！";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "には耐性がある！";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport monster (Use "dam" as "power") */
		case GF_AWAY_ALL:
		{
			bool resists_tele = FALSE;
			if (r_ptr->flagsr & RFR_RES_TELE)
			{
				if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					resists_tele = TRUE;
				}
				else if (r_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
					note = "には耐性がある！";
#else
					note = " resists!";
#endif

					resists_tele = TRUE;
				}
			}

			if (!resists_tele)
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Prepare to teleport */
				do_dist = dam;

				if(!who && r_ptr->level > randint1(500)) set_deity_bias(DBIAS_WARLIKE, -1);

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn undead (Use "dam" as "power") */
		///magic アンデッド退散　ここを即死効果に変更しようか？
		case GF_TURN_UNDEAD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				//if (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				//v1.1.90 判定変更
				if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
				{
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
					do_fear = 0;
				}
				else
				{
					//v1.1.94 精神攻撃系の追加効果で魔法低下
					do_dec_mag = 8 + randint1(8);

				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn evil (Use "dam" as "power") */
		case GF_TURN_EVIL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				//if (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				//v1.1.90 判定変更
				if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
				{
					/* No obvious effect */
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
					do_fear = 0;
				}
				else
				{
					//v1.1.94 精神攻撃系の追加効果で魔法低下
					do_dec_mag = 8 + randint1(8);

				}

			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn monster (Use "dam" as "power") */
		case GF_TURN_ALL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

			/*:::Hack 小町の威圧処理　恐慌モンスターの魔法棒など全部強力になってしまうがまあいいか*/
			if(p_ptr->pclass == CLASS_KOMACHI && (r_ptr->flags3 & RF3_UNDEAD || monster_living(r_ptr))) dam *= 2;

			/* Apply some fear */
			do_fear = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			//    (r_ptr->flags3 & (RF3_NO_FEAR)) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 判定変更
			if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
			{
				/* No obvious effect */
#ifdef JP
note = "は恐怖しなかった。";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				do_fear = 0;
			}
			else
			{
				//v1.1.94 精神攻撃系の追加効果で魔法低下
				do_dec_mag = 8 + randint1(8);

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Dispel undead */
		case GF_DISP_UNDEAD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;

				/* Message */
#ifdef JP
			///sysdel dead
				note = "は身震いした。";
				note_dies = "は倒れた。";
#else
				note = " shudders.";
				note_dies = " is defeated.";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Dispel evil */
		///mod131231 混沌勢力を対象にしておく
		case GF_DISP_EVIL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);

				/* Message */
#ifdef JP
			///sysdel dead

				note = "は身震いした。";
				note_dies = "は倒れた。";
#else
				note = " shudders.";
				note_dies = " is defeated.";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel good */
		///mod131231 秩序勢力を対象にしておく
		case GF_DISP_GOOD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect good */
			if (r_ptr->flags3 & (RF3_ANG_COSMOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_COSMOS);

				/* Message */
#ifdef JP
						///sysdel dead
				note = "は身震いした。";
				note_dies = "は倒れた。";
#else
				note = " shudders.";
				note_dies = " is defeated.";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel living */
		case GF_DISP_LIVING:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect non-undead */
			if (monster_living(r_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
#ifdef JP
				///sysdel dead
note = "は身震いした。";
note_dies = "は倒れた。";
#else
				note = " shudders.";
				note_dies = " is defeated.";
#endif

			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel demons */
		case GF_DISP_DEMON:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if (r_ptr->flags3 & (RF3_DEMON))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;


				/* Message */
#ifdef JP
					///sysdel dead
				note = "は身震いした。";
				note_dies = "は倒れた。";
#else
				note = " shudders.";
				note_dies = " is defeated.";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}
		/* Dispel demons */
		case GF_DISP_KWAI:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if (r_ptr->flags3 & (RF3_KWAI))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_KWAI);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;
				note = _("は身震いした。", " shudders.");
				note_dies = _("は倒れた。", " is defeated.");
			}

			else
			{
				skipped = TRUE;
				dam = 0;
			}

			break;
		}

		/* Dispel monster */
		/*:::力の杖など*/
		case GF_DISP_ALL:
		case GF_HOUTOU:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
#ifdef JP
			///sysdel dead
note = "は身震いした。";
note_dies = "は倒れた。";
#else
			note = " shudders.";
			note_dies = " is defeated.";
#endif


			break;
		}

		/* Drain mana */
		case GF_DRAIN_MANA:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///sys monspell RF4-6flag 魔法を使うモンスターが魔力吸収を使うときの判定？
			if (HAS_ANY_MAGIC(r_ptr))
			{
				if (who > 0)
				{
					/* Heal the monster */
					if (caster_ptr->hp < caster_ptr->maxhp)
					{
						/* Heal */
						caster_ptr->hp += 6 * dam;
						if (caster_ptr->hp > caster_ptr->maxhp) caster_ptr->hp = caster_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == who) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == who) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (see_s_msg)
						{
							/* Get the monster name */
							monster_desc(killer, caster_ptr, 0);
#ifdef JP
							msg_format("%^sは気分が良さそうだ。", killer);
#else
							msg_format("%^s appears healthier.", killer);
#endif
						}
					}
				}
				else
				{
					/* Message */
#ifdef JP
					msg_format("%sから魔力を吸いとった。", m_name);
					//msg_format("%sから精神エネルギーを吸いとった。", m_name);
#else
					msg_format("You draw mana from %s.", m_name);
#endif
					if(p_ptr->csp < p_ptr->msp) msg_print(_("少し頭がハッキリした。", "You feel your head clear a little."));
					p_ptr->csp += dam;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
			}
			else
			{
#ifdef JP
			//	if (see_s_msg) msg_format("%sには効果がなかった。", m_name);
				if (see_s_msg) msg_format("%sは魔力を持たないようだ。", m_name);
#else
				if (see_s_msg) msg_format("%s is unaffected!", m_name);
#endif
			}
			dam = 0;
			break;
		}

		/* Mind blast */
		//v1.1.90 BRAIN_SMASHと統合した
		case GF_MIND_BLAST:
		case GF_BRAIN_SMASH:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP

			//if (!who) msg_format("%sをじっと睨んだ。", m_name);
#else
			//if (!who) msg_format("You gaze intently at %s.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//	 (r_ptr->flags3 & RF3_NO_CONF) ||
			//	 (r_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 10))
			//v1.1.90 判定変更
			else if (r_ptr->flags3 & (RF3_NO_CONF) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
#ifdef JP
				note = "に抵抗された！";
#else
				note = " is unaffected!";
#endif
				dam = 0;
				break;
		}
			else if (r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_WEIRD_MIND);
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
				dam /= 3;
				break;
			}
			else
			{
#ifdef JP
			///sysdel dead
				note = "は精神攻撃を食らった。";
				note_dies = "は倒れた。";
#else
				note = " is blasted by psionic energy.";
				note_dies = " is defeated.";
#endif

				if (typ == GF_BRAIN_SMASH)
				{
					do_conf = randint0(8) + 8;
					do_stun = randint0(8) + 8;
					(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4));
				}
				else
				{
					do_conf = randint0(4) + 4;

				}

				//v1.1.94 精神攻撃系の追加効果で魔法低下
				do_dec_mag = 8 + randint1(dam / 10);

			}
			break;
		}

		/* CAUSE_1 */
		case GF_CAUSE_1:
		{
			if (seen) obvious = TRUE;

			if(!who && p_ptr->pclass == CLASS_BENBEN)
				msg_format(_("%sに怨霊が襲いかかった！", "%s is attacked by vengeful spirits!"), m_name);

			/* Message */
#ifdef JP
			//if (!who) msg_format("%sを指差して呪いをかけた。", m_name);
#else
			//if (!who) msg_format("You point at %s and curse.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 呪い系は無生物には効かなくした
			else if(!monster_living(r_ptr))
			{
				note = _("には効果がなかった。", " is unaffected!");
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "は呪いに抵抗した。";
#else
				note = "resists the curse.";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_2 */
		case GF_CAUSE_2:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%sを指差して恐ろしげに呪いをかけた。", m_name);
#else
			//if (!who) msg_format("You point at %s and curse horribly.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 呪い系は無生物には効かなくした
			else if(!monster_living(r_ptr))
			{
				note = _("には効果がなかった。", " is unaffected!");
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "は呪いに抵抗した。";
#else
				note = " resists the curse.";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_3 */
		case GF_CAUSE_3:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%sを指差し、恐ろしげに呪文を唱えた！", m_name);
#else
			//if (!who) msg_format("You point at %s, incanting terribly!", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 呪い系は無生物には効かなくした
			else if(!monster_living(r_ptr))
			{
				note = _("には効果がなかった。", " is unaffected!");
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "は呪いに抵抗した。";
#else
				note = " resists the curse.";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_4 */
		case GF_CAUSE_4:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			///sys magic 秘孔メッセージ変更
			//if (!who) msg_format("%sに向けて死の言霊を放った。", m_name);
#else
			//if (!who) msg_format("You point at %s, screaming the word, 'DIE!'.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 呪い系は無生物には効かなくした
			else if(!monster_living(r_ptr))
			{
				note = _("には効果がなかった。", " is unaffected!");
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}
			/* Attempt a saving throw */
			///mon ケンシロウ特殊処理
			if ((randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35)) && ((who <= 0) || (caster_ptr->r_idx != MON_KENSHIROU)))
			{
#ifdef JP
				note = "は呪いに抵抗した。";
#else
				note = " resists the curse.";
#endif
				dam = 0;
			}
			break;
		}

		///mod140107 フラン特殊攻撃の敵への威力　一応破滅の手と同じにしておく
		///mod150609 ユニークだろうと必中にしてみた
		case GF_KYUTTOSHITEDOKA_N:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else
			{
				dam = ((40 + randint1(20)) * m_ptr->hp) / 100;

				if (m_ptr->hp < dam) dam = m_ptr->hp - 1;
			}
			break;
		}
		/* HAND_DOOM */
		case GF_HAND_DOOM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags3 & RF3_HANIWA)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif
				dam = 0;
			}
			else
			{
				if ((who > 0) ? ((caster_lev + randint1(dam)) > (r_ptr->level + 10 + randint1(20))) :
	//			   (((caster_lev / 2) + randint1(dam)) > (r_ptr->level + randint1(200))))
				   (((caster_lev ) + randint1(dam)) > (r_ptr->level  + randint1(120))))
				{
					dam = ((40 + randint1(20)) * m_ptr->hp) / 100;

					if (m_ptr->hp < dam) dam = m_ptr->hp - 1;
				}
				else
				{
#ifdef JP
					note = "は破滅に対し抵抗した！";
#else
					note = " resists the destruction!";
#endif
					dam = 0;
				}
			}
			break;
		}

		case GF_COSMIC_HORROR:
		{
			int chance;
			if (seen) obvious = TRUE;
			chance = randint1(100 + (caster_lev * 2));

			if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) chance /= 2;
			if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ) chance = chance * 2 / 3;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			else if (r_ptr->flags2 & (RF2_ELDRITCH_HORROR | RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = _("には効果がなかった。", " is unaffected!");
				dam = 0;
			}
			else if (chance < r_ptr->level)
			{
				note = _("は攻撃に抵抗した。", " resists the attack.");
				dam = 0;
			}
			else
			{
				do_stun = dam / 8 + randint1(dam / 4);
				do_conf = dam / 8 + randint1(dam / 4);
				do_fear = dam / 8 + randint1(dam / 4);

				//v1.1.94 精神攻撃系の追加効果で魔法低下
				do_dec_mag = 16 + randint1(dam / 8);
			}

			break;
		}



		/* Capture monster */
		case GF_CAPTURE:
		{
			int nokori_hp;
			if ((p_ptr->inside_quest && (quest[p_ptr->inside_quest].type == QUEST_TYPE_KILL_ALL) && !is_pet(m_ptr)) ||
			    (r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flags7 & (RF7_NAZGUL)) || (r_ptr->flags7 & (RF7_VARIABLE))
				|| (r_ptr->flags7 & (RF7_UNIQUE2)) || (r_ptr->flags1 & RF1_QUESTOR) || (m_ptr->mflag & MFLAG_EPHEMERA)
				|| m_ptr->parent_m_idx || (r_ptr->flags7 & (RF7_FORCE_GO_STRAIGHT)) || (r_ptr->flags7 & (RF7_ONLY_RIDING)))
			{
#ifdef JP
				msg_format("%sには効果がなかった。",m_name);
#else
				msg_format("%^s is unaffected!", m_name);
#endif
				skipped = TRUE;
				break;
			}

			if (is_pet(m_ptr)) nokori_hp = m_ptr->maxhp * 4L;
			///class 魔獣使いはモンスターボールが得意
			else if ((p_ptr->pclass == CLASS_BEASTMASTER) && monster_living(r_ptr))
				nokori_hp = m_ptr->maxhp * 3 / 10;
			else
				nokori_hp = m_ptr->maxhp * 3 / 20;

			if (m_ptr->hp >= nokori_hp)
			{
#ifdef JP
				msg_format("もっと弱らせないと。");
#else
				msg_format("You need to weaken %s more.", m_name);
#endif
				skipped = TRUE;
			}
			else if (m_ptr->hp < randint0(nokori_hp))
			{
				if (m_ptr->mflag2 & MFLAG2_CHAMELEON) choose_new_monster(c_ptr->m_idx, FALSE, MON_CHAMELEON);
#ifdef JP
				msg_format("%sを捕えた！",m_name);
#else
				msg_format("You capture %^s!", m_name);
#endif
				cap_mon = m_ptr->r_idx;
				cap_mspeed = m_ptr->mspeed;
				cap_hp = m_ptr->hp;
				cap_maxhp = m_ptr->max_maxhp;
				cap_nickname = m_ptr->nickname; /* Quark transfer */
				/*:::乗馬中にモンスターボールを反射されたら落馬するらしい*/
				if (c_ptr->m_idx == p_ptr->riding)
				{
					if (rakuba(-1, FALSE))
					{
#ifdef JP
						msg_print("地面に落とされた。");
#else
						msg_format("You have fallen from %s.", m_name);
#endif
					}
				}

				delete_monster_idx(c_ptr->m_idx);

				return (TRUE);
			}
			else
			{
#ifdef JP
msg_format("うまく捕まえられなかった。");
#else
				msg_format("You failed to capture %s.", m_name);
#endif
				skipped = TRUE;
			}
			break;
		}

		/* Attack (Use "dam" as attack type) */
		/*:::通常攻撃属性 必殺剣など　damに攻撃の種類が入る*/
		case GF_ATTACK:
		{
			/* Return this monster's death */
			return py_attack(y, x, dam);
		}
		case GF_SOULSCULPTURE:
		{
			/* Return this monster's death */
			return py_attack(y, x, HISSATSU_SOULSCULPTURE);
		}
		case GF_YOUMU:
		{
			//v2.0.19 養蜂家の視界内攻撃に流用
			if (p_ptr->pclass == CLASS_BEEKEEPER)
			{
				return py_attack(y, x, HISSATSU_ATTACK_BEE);
			}
			else
			{
				return py_attack(y, x, HISSATSU_NO_AURA);
			}
		}


		/* Sleep (Use "dam" as "power") */
		case GF_ENGETSU:
		{
			int effect = 0;
			bool done = TRUE;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;
			}
			if (MON_CSLEEP(m_ptr))
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				dam = 0;
				skipped = TRUE;
				break;
			}

			if (one_in_(5)) effect = 1;
			else if (one_in_(4)) effect = 2;
			else if (one_in_(3)) effect = 3;
			else done = FALSE;

			if (effect == 1)
			{
				/* Powerful monsters can resist */
				if ((r_ptr->flags1 & RF1_UNIQUE) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
#ifdef JP
					note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}

				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
#ifdef JP
						note = "の動きが遅くなった。";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}

			else if (effect == 2)
			{
				do_stun = damroll((p_ptr->lev / 10) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Resist */
					do_stun = 0;

					/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}

			else if (effect == 3)
			{
				/* Attempt a saving throw */
				if ((r_ptr->flags1 & RF1_UNIQUE) ||
				    (r_ptr->flags3 & RF3_NO_SLEEP) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Memorize a flag */
					if (r_ptr->flags3 & RF3_NO_SLEEP)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
					}

					/* No obvious effect */
#ifdef JP
note = "には効果がなかった！";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
				else
				{
					/* Go to sleep (much) later */
#ifdef JP
note = "は眠り込んでしまった！";
#else
					note = " falls asleep!";
#endif

					do_sleep = 500;
				}
			}

			if (!done)
			{
#ifdef JP
note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* GENOCIDE */
		/*:::モンスター消滅*/
		case GF_GENOCIDE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

#ifdef JP
			if (genocide_aux(c_ptr->m_idx, dam, !who, (r_ptr->level + 1) / 2, "モンスター消滅"))
#else
			if (genocide_aux(c_ptr->m_idx, dam, !who, (r_ptr->level + 1) / 2, "monster vanishing"))
#endif
			{
#ifdef JP
				///sysdel dead
				if (seen_msg) msg_format("%sはどこかへ消えた。", m_name);
#else
				if (seen_msg) msg_format("%^s disappeared!", m_name);
#endif
				//chg_virtue(V_VITALITY, -1);
				return TRUE;
			}

			skipped = TRUE;
			break;
		}
		/*:::写真撮影*/
		case GF_PHOTO:
		{
#ifdef JP
			if (!who) msg_format("%sを写真に撮った。", m_name);
#else
			if (!who) msg_format("You take a photograph of %s.", m_name);
#endif
			/* Hurt by light */
			///mod131231 モンスターフラグ変更 閃光弱点RF3からRFRへ
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);

				/* Special effect */
#ifdef JP
				///sysdel dead
				note = "は光に身をすくめた！";
				note_dies = "は光を受けて倒れた！";
#else
				note = " cringes from the light!";
				note_dies = " collapses from the light!";
#endif
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			photo = m_ptr->r_idx;

			break;
		}


		/* blood curse */
		case GF_BLOOD_CURSE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.94 追加効果で能力低下(ダメージ値を判定パワーとしてセービングスロー)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
			}

			break;
		}

		//v1.1.66 磨弓「不敗の無尽兵団」用に書き換え
		case GF_CRUSADE:
		{
			if (seen) obvious = TRUE;

			if ((r_ptr->flags3 & (RF3_HANIWA)) && is_pet(m_ptr))
			{
				note = _("の動きが速くなった。", " starts moving faster.");

				(void)set_monster_fast(c_ptr->m_idx, MON_FAST(m_ptr) + 100);

			}

			else
			{
				if (!(r_ptr->flags3 & RF3_NO_FEAR))
				{
					do_fear = randint1(90)+10;
				}
				else if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_FEAR);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/*:::＠による軽傷の呪いなど*/
		case GF_WOUNDS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + dam) < (r_ptr->level + 50))
			{

#ifdef JP
note = "には効果がなかった。";
#else
				note = " is unaffected!";
#endif
				dam = 0;
			}
			break;
		}


		/* PUNISH_1/2/3/4 破邪弱点の敵にしか効かない。効いたら敵に朦朧、恐怖付与を試みる。*/
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		{
			int power;
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%sを指差して魔除けをかけた。", m_name);
#endif
			if(typ == GF_PUNISH_1) power = 100;
			else if(typ == GF_PUNISH_2) power = 120;
			else if(typ == GF_PUNISH_3) power = 140;
			else power = 160;


			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/*:::破邪弱点の敵にしか効かない*/
			else if (!(r_ptr->flagsr & RFR_HURT_HOLY))
			{
#ifdef JP
				note = "には効果がなかった。";
#else
                note = " is unaffected!";
#endif
				dam = 0;
			}

			/* Attempt a saving throw */
			else if (randint0(power + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "は呪文に抵抗した。";
#else
				note = " resists the spell.";
#endif
				dam = 0;
			}

			else
			{
				dam *= 2;

				if (!(r_ptr->flags3 & RF3_NO_FEAR)) do_fear = damroll(3, (dam / 2)) + 1;
				if (!(r_ptr->flags3 & RF3_NO_STUN) && (typ == GF_PUNISH_3 || typ == GF_PUNISH_4) ) do_stun = damroll(3, (dam / 2)) + 1;

				//v1.1.94 追加効果で能力低下
				if (one_in_(3))
					do_dec_atk = 8 + randint1(8);
				else if (one_in_(2))
					do_dec_def = 8 + randint1(8);
				else
					do_dec_mag = 8 + randint1(8);
			}

			break;
		}

		/* 死霊領域の怨霊憑依など */
		//v1.1.95 狂戦士化効果にした
		/*
		case GF_POSSESSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags3 & RF3_HANIWA)//v1.1.64
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}


			if(r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
			if(r_ptr->flags1 & RF1_UNIQUE) dam /= 2;
			if(r_ptr->flags3 & RF3_UNDEAD) dam /= 3;
			if(r_ptr->flags3 & RF3_DEMON) dam /= 3;
			if(r_ptr->flags3 & RF3_NONLIVING) dam /= 2;
			if(r_ptr->flags2 & RF2_POWERFUL) dam /= 2;
			if(r_ptr->flags3 & RF3_KWAI) dam *= 2;

			if(randint1(dam/2) + dam/2 < r_ptr->level)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
			}
			else
			{
				msg_format(_("%sに怨霊が取り憑いた！",
                            "%s is possessed by a vengeful spirit!"), m_name);
				do_stun = dam / 8 + randint1(dam / 4);
				do_conf = dam / 8 + randint1(dam / 4);
				do_fear = dam / 8 + randint1(dam / 4);

				(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 16 + randint1(16));

				dam = 0;
			}


			break;
		}
		*/

		/* 死霊領域のソウルスティール */
		case GF_SOULSTEAL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(!monster_living(r_ptr))
			{
				note = _("には効果がなかった。", " is unaffected!");
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			else if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ||(r_ptr->flags1 & RF1_QUESTOR))
			{
				note = _("には効いていないようだ。", " doesn't seem like a valid target.");
				dam = 0;
				break;
			}
			//v1.1.65　耐性は重複しないようにした
			if (r_ptr->flagsr & RFR_RES_NETH) dam /= 5;
			else if(r_ptr->flags2 & RF2_POWERFUL) dam /= 2;
			else if(r_ptr->flags2 & RF2_SMART) dam /= 2;

			if (randint0(dam) < (r_ptr->level))
			{
#ifdef JP
				note = "は呪文に抵抗した！";
#else
				note = " is unaffected!";
#endif
				dam = 0;
			}
			else
			{
				note_dies = _("の生命力を奪い取った！", " is drained of life force!");
				dam = m_ptr->hp + 1;
				hp_player(dam);
			}





			break;
		}



		/*:::ウィンドカッター専用　敵ACに準じた確率で回避される*/
		case GF_WINDCUTTER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(randint1(r_ptr->ac ) > (p_ptr->lev + 25)) //v1.1.96
			{
				note = _("には当たらなかった。", " did not get hit.");
				dam = 0;
			}

			break;
		}


		/*::: 狂気属性　ウドンゲなど　ユニークに効き反射されないが特定の敵に効果が薄い半物理の精神攻撃 */
		case GF_REDEYE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_ELDRITCH_HORROR)
			{
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
#ifdef JP
				note = "にはかなり耐性がある！";
#else
				note = " resists a lot!";
#endif
				dam /= 9;
				break;
			}
			else if (r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_WEIRD_MIND);
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
				dam /= 3;
				break;
			}
			//else if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//	 (r_ptr->flags3 & RF3_NO_CONF) ||
			//	 (r_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 80))
			//v1.1.90 判定変更
			else if (r_ptr->flags3 & (RF3_NO_CONF) || r_ptr->flags3 & (RF3_NONLIVING) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
				note = _("は攻撃に抵抗した。", " resists the attack.");
				dam /= 2;
				break;
			}
			else
			{
#ifdef JP
				//note = "は精神攻撃を食らった。";
				note_dies = "は倒れた。";
#else
				//note = " is blasted by psionic energy.";
				note_dies = " is defeated.";
#endif

				do_conf = randint0(8) + 8;
				if (caster_lev > 29)
				{
					do_stun = randint0(8) + 8;
					(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4));
				}

				//v1.1.94 精神攻撃系の追加効果で魔法低下
				do_dec_mag = 8 + randint1(dam / 10);
			}
			break;
		}


		/*::: さとり専用　アンデッドに効きやすい精神攻撃 */
		case GF_SATORI:
		{
			bool ok = TRUE;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				note = _("は無反応だ。", " does not react.");
				dam = 0;
				ok = FALSE;
				break;
			}
			else if (r_ptr->flags2 & RF2_ELDRITCH_HORROR)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_ELDRITCH_HORROR);
				note = _("には耐性がある。", " resists.");
				dam /= 9;
				ok = FALSE;
			}
			else if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags2 & RF2_POWERFUL)
			{
				note = _("は精神攻撃にある程度抵抗した。", " resists the mental attack somewhat.");
				dam /= 2;
				ok = FALSE;
			}

			if(r_ptr->flags3 & RF3_UNDEAD)
			{
				note = _("は激しく苦しみだした！", " is suffering!");
				dam *= 2;
			}

			note_dies = _("は倒れた。", " is defeated.");

			if (ok && !(r_ptr->flags3 & RF3_NO_FEAR) && ((r_ptr->flags3 & RF3_UNDEAD) || randint1(r_ptr->level) < caster_lev))
				do_fear = damroll(3, (dam / 2)) + 1;

			if (ok && !(r_ptr->flags3 & RF3_NO_CONF) && ((r_ptr->flags3 & RF3_UNDEAD) || randint1(r_ptr->level) < caster_lev) )
				do_conf = randint0(8) + 8;

			if (ok && !mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam / 8);
			}

			break;
		}
		//v1.1.43 クラウンピース「狂気の松明」効果
		case GF_LUNATIC_TORCH:
		{
			//特殊フラグ付与判定と処理
			lunatic_torch(c_ptr->m_idx, dam);

			dam = 0;
			break;
		}
		//雛の厄攻撃　抵抗されなかったとき厄消費
		case GF_YAKU:
		{
			int sav;

			if(p_ptr->pclass != CLASS_HINA)
			{
				msg_print(_("ERROR:雛以外でGF_YAKU属性攻撃が使われた",
                            "ERROR: GF_YAKU element attack used for non-Hina"));
				dam=0;
				break;
			}

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			sav = r_ptr->level + randint1(30);

			if (r_ptr->flagsr & RFR_RES_NETH)
			{
				sav *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_NETH);
			}
			if (r_ptr->flagsr & RFR_RES_DARK)
			{
				sav *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DARK);
			}
			if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
				sav *= 2;
			if(r_ptr->flags2 & RF2_POWERFUL)
				sav *= 2;

			if(randint1(sav) > p_ptr->lev)
			{
				note = _("は厄に抵抗した！", " resists the misfortune!");
				dam=0;
				break;
			}

			note = _("は厄を受けた！", " receives misfortune!");
			hina_gain_yaku(-dam/10);

			//v1.1.94 精神攻撃系の追加効果で魔法低下
			do_dec_mag = 8 + randint1(8);

			break;
		}
		/*:::小町の生魂流離の鎌専用*/
		case GF_RYUURI:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//v1.1.47 無生物にも効くようにした。即死効果やHP吸収効果はない。
			/*
			if(!monster_living(r_ptr))
			{
				note = "には効果がなかった。";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}
			*/
			if (monster_living(r_ptr))
			{
				if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)
					|| (randint1(r_ptr->level * 4) > p_ptr->lev))
				{
					do_time = (dam + 7) / 8;
					hp_player(MIN(dam / 3, m_ptr->hp));
				}
				else
				{
					note_dies = _("の急所に深々と刃が突き刺さった！",
                                    " is deeply wounded in a critical spot!");
					if (dam < m_ptr->hp) dam = m_ptr->hp + 1;
					hp_player(dam / 3);
				}
			}

			break;
		}
		case GF_KOKORO:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = _("は感情を持たないようだ。", " does not have any emotions.");
				dam = 0;
				break;
			}

			break;
		}

		//v1.1.42 アンデッド、デーモン、SMARTの敵にも効くようにする代わりに少し起きやすくした
		case GF_NIGHTMARE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//else if (r_ptr->flags2 & (RF2_EMPTY_MIND) || (r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_NONLIVING)))
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND))
			{
				dam = 0;
#ifdef JP
				note = "には完全な耐性がある！";
#else
				note = " is immune.";
#endif
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;

			}
			else if ((r_ptr->flags2 & (RF2_WEIRD_MIND)) ||
					(r_ptr->flags3 & (RF3_NONLIVING)) ||
			         (r_ptr->flags1 & RF1_UNIQUE) )
			{
				dam /= 2;
#ifdef JP
				note = "には耐性がある。";
#else
				note = " resists.";
#endif
				break;
			}


			else if (!MON_CSLEEP(m_ptr))
			{
				note = _("は精神攻撃を食らった!", " is hit by the mental attack!");
			}
//			else if (randint1(r_ptr->level) > (p_ptr->lev * 3 / 2) || one_in_(10))
			else if (randint1(r_ptr->level) > (p_ptr->lev) || one_in_(10))
			{
				dam *= 2;
				note = _("は悪夢を見て飛び起きた!", " jumps, having seen a nightmare!");
			}
			else
			{
				dam *= 2;
				note = _("は悪夢にうなされている・・", " is having nightmares...");
				do_sleep = randint1(p_ptr->lev);
			}

			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_mag = 8 + randint1(dam / 10);
			}

#ifdef JP
			///msg dead
			//note_dies = "の精神は崩壊し、肉体は抜け殻となった。";
			note_dies = "は倒れた。";
#else
			note_dies = " is defeated.";
#endif

			break;
		}

		case GF_BANKI_BEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
                note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}
		case GF_BANKI_BEAM2:
		{
			//ヘルズレイは頭の分身に無効
			if(m_ptr->r_idx == MON_BANKI_HEAD_1 ||m_ptr->r_idx == MON_BANKI_HEAD_2) return (TRUE);
			if(is_pet(m_ptr)) return(TRUE);

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "には完全な耐性がある！";
#else
                note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}
		case GF_N_E_P:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(r_ptr->flags3 & (RF3_GEN_MASK))
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}
			else if(r_ptr->flagsr & RFR_RES_TIME)
			{
				note = _("は因果律の修正に抵抗した！", " resists the causality correction!");
				dam = dam * 6 / (12 + randint0(7));
				break;
			}
			note_dies = _("は消滅した。", " vanishes.");

			break;
		}

		//v1.1.61 ブレインフィンガープリント
		case GF_BRAIN_FINGER_PRINT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}
			else
			{
				//現在までに受けたダメージ量でダメージが増える。ボス格を後半瞬殺したりできないよう多少補正する。
				int mon_damaged = (m_ptr->maxhp - m_ptr->hp);

				if (mon_damaged < 3000) dam = mon_damaged / 2;
				else if (mon_damaged < 7000) dam = 1500 + (mon_damaged - 3000) / 4;
				else dam = 2500 + (mon_damaged - 7000) / 6;

				dam += adj_general[p_ptr->stat_ind[A_CHR]] * 8;

				if (r_ptr->flags3 & RF3_UNDEAD) dam *= 2;

				if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) dam /= 2;
				if (r_ptr->flags2 & RF2_POWERFUL) dam = dam * 2 / 3;

				if (dam < 1) dam = 1;

				note = _("の苦痛の記憶がさらなる苦痛を与えた！",
                        " is hurt even more by memories of earlier trauma!");
				break;
			}

			//v1.1.94 精神系攻撃で魔法能力低下
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_mag = 8 + randint1(dam / 10);
			}

			note_dies = _("は倒れた。", " is defeated.");

			break;
		}

		//v1.1.63 虹属性　閃光判定で閃光・混乱耐性のないモンスターを確実に混乱させる
		//v1.1.86 ダメージを閃光と水の半々で処理する。閃光と水の両方に弱いモンスターにはダメージ3倍。
		case GF_RAINBOW:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flagsr & RFR_HURT_LITE) && (r_ptr->flagsr & RFR_HURT_WATER))
			{
				note = _("は太陽と雨の力を受けて悲鳴を上げた！",
                        " screams after getting hit by the sun and water!");
				dam *= 3;
			}
			else
			{
				int lite_dam, water_dam;
				lite_dam = (dam + 1) / 2;
				water_dam = (dam + 1) / 2;

				if (r_ptr->flagsr & RFR_RES_LITE)
				{
					note = _("はいくらかの耐性を示した。", " resists somewhat.");

					lite_dam = lite_dam * 2 / (randint1(6) + 6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
				else if (r_ptr->flagsr & (RFR_HURT_LITE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
					///sysdel dead
					note = _("は光に身をすくめた！", " cringes from the light!");
					note_dies = _("は光を受けて倒れた！", " collapses from the light!");
					lite_dam *= 2;
				}

				if (r_ptr->flagsr & RFR_RES_WATE || r_ptr->flags7 & RF7_AQUATIC)
				{
					note = _("はいくらかの耐性を示した。", " resists somewhat.");

					water_dam = water_dam * 3 / (randint1(6) + 6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
				else if (r_ptr->flagsr & (RFR_HURT_WATER))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
					///sysdel dead
					note = _("は水が苦手なようだ！", " does not like water!");
					note_dies = _("は水を受けて倒れた！", " collapses from the water attack!");
					water_dam *= 2;
				}

				dam = lite_dam + water_dam;

			}

			if (r_ptr->flagsr & RFR_RES_LITE)
			{
				note = _("には耐性がある！", " resists.");

				dam *= 2; dam /= (randint1(6) + 6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
			}
			///mod131231 モンスターフラグ変更 閃光弱点RF3からRFRへ

			if (!(r_ptr->flagsr & RFR_RES_LITE) && !(r_ptr->flags3 & RF3_NO_CONF))
			{
				do_conf = 7 + randint1(7);

			}
			break;
		}

		//v1.1.17 純狐の「純化」属性　全弱点に刺さる
		case GF_PURIFY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flagsr & (RFR_HURT_HOLY))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
			}
			if (r_ptr->flagsr & (RFR_HURT_HELL))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HELL);
			}
			if (r_ptr->flagsr & (RFR_HURT_WATER))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}
			if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			if (r_ptr->flagsr & (RFR_HURT_ROCK))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);
			}
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				note = _("は純光を受けて身をすくめた。", " cringes from the purifying light.");
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
			}

			break;
		}



		/*:::銃による特殊射撃　special_shooting_modeで分岐*/
		case GF_SPECIAL_SHOT:
		{
			int power;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には完全な耐性がある！", " is immune.");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			switch(special_shooting_mode)
			{

			case SSM_CONFUSE:	//牽制射撃
				power = dam;
				dam = 0;
				if (r_ptr->flags3 & (RF3_NO_FEAR) )
				{
					note = _("には効果がなかった！", " is unaffected!");
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_FEAR);
					break;
				}
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 2;
				if(randint1(r_ptr->level) < randint1(power+caster_lev))
				{
					if (!(r_ptr->flagsr & RFR_RES_INER) && set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
						note = _("の動きが遅くなった。", " starts moving slower.");
					}

					if(p_ptr->lev > 19 && !(r_ptr->flags3 & (RF3_NO_CONF))) do_conf = randint1(4) + randint0(power/10);
				}
				else
				{
					note = _("は惑わされなかった！", " wasn't fooled!");

				}
				break;

			case SSM_LIGHT:	//閃光弾

				if (r_ptr->flagsr & (RFR_HURT_LITE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
					note = _("に閃光弾が直撃した！", " is hurt by the light shot!");
					dam *= 3;
				}
				break;
			case SSM_VAMPIRIC: //吸血弾
				if (!monster_living(r_ptr))
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
						if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
						if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					}
					note = _("には効果がなかった！", " is unaffected!");
					dam = 0;
				}
				else
				{
					note = _("から生命力を吸収した！", " is drained of life force!");
					do_time = (dam+7)/8;
					hp_player(dam);
				}
				break;

			case SSM_SLAY_ANIMAL:
				if (r_ptr->flags3 & (RF3_ANIMAL))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANIMAL);
					note = _("に獣殺しの弾丸が食い込んだ！", " is hurt by the beast-slaying shot!");
					dam *= 3;
				}
				break;

			case SSM_SLAY_NONLIV:
				if (r_ptr->flags3 & (RF3_NONLIVING))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = _("は弾丸を受けて動きが乱れている。", " is hurt by the shot!");
					dam *= 3;
				}
				break;
			case SSM_SLAY_EVIL:
				if (r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = _("の体を弾丸が焦がした！", " is scorched by the shot!");
					dam *= 2;
				}
				break;
			case SSM_SLAY_HUMAN:
				if (r_ptr->flags3 & (RF3_HUMAN))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_HUMAN);
					if (p_ptr->pclass == CLASS_SEIRAN)
					{
						note = _("の精神に打撃を与えた！", " is mentally hurt by the attack!");
						dam *= 2;
					}
					else
					{
						note = _("は怨霊に取り巻かれた！", " is surrounded by vengeful spirits!");
						dam *= 3;
					}

				}
				else if (r_ptr->flags3 & (RF3_DEMIHUMAN))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMIHUMAN);
					if (p_ptr->pclass == CLASS_SEIRAN)
						note = _("の精神に打撃を与えた！", " is mentally hurt by the attack!");
					else
						note = _("は怨霊に取り巻かれた！", " is surrounded by vengeful spirits!");

					dam *= 2;
				}
				break;

			case SSM_SLAY_DRAGON:
				if (r_ptr->flags3 & (RF3_DRAGON))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DRAGON);
					note = _("の硬い鱗を竜殺しの弾丸が撃ち抜いた！", " is pierced by the dragon-slaying shot!");
					dam *= 3;
				}
				break;
			case SSM_SLAY_DEITY:
				if (r_ptr->flags3 & (RF3_DEITY))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEITY);
					note = _("の体を神殺しの弾丸が撃ち抜いた！", " is pierced by the deity-slaying shot!");
					dam *= 3;
				}
				break;
			case SSM_SILVER:
				if (r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = _("の体を銀の弾丸が貫いた！", " is pierced by the silver bullet!");
					dam *= 3;
				}
				else if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					r_ptr->r_flags3 |= (RF3_ANG_CHAOS);
					note = _("の体を銀の弾丸が焦がした！", " is scorched by the silver bullet!");
					dam = dam * 3 / 2;

				}
				break;
			case SSM_NIGHTMARE:
				if(r_ptr->flagsr & RFR_HURT_HELL)
				{
					note = _("の清浄な体を悪夢の弾丸が撃ち抜いた！", " is pierced by the nightmare shot!");
					dam *= 3;
				}
				break;

			case SSM_SLAY_FLYER:
				if (r_ptr->flags7 & (RF7_CAN_FLY))
				{
					note = _("を撃ち落とした！", " is hit hard!");
					dam *= 2;
				}
				break;


			default:
				msg_format(_("ERROR:special_shooting_mode(%d)でのGF_SPECIAL_SHOT処理が記述されていない",
                            "ERROR: GF_SPECIAL_SHOT logic not defined for special_shooting_mode (%d)"),special_shooting_mode);
				break;

			}


			break;
		}


		//v1.1.94
		case GF_DEC_ATK:
		case GF_DEC_DEF:
		case GF_DEC_MAG:
		case GF_DEC_ALL:
		case GF_DIG_OIL:
		case GF_SUPER_EGO:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//スーパーエゴ　無精神には無効
			if(typ == GF_SUPER_EGO && (r_ptr->flags2 & (RF2_WEIRD_MIND | RF2_EMPTY_MIND)))
			{
				note = _("には効果がなかった！", " is unaffected!");
				obvious = FALSE;
			}

			if (mon_saving_throw(m_ptr, dam))
			{
				note = _("には効果がなかった！", " is unaffected!");
				obvious = FALSE;
			}

			switch (typ)
			{
			case GF_DEC_ATK:
			case GF_DIG_OIL:
				do_dec_atk = 8 + randint1(8);
				break;
			case GF_DEC_DEF:
				do_dec_def = 8 + randint1(8);
				break;
			case GF_DEC_MAG:
				do_dec_mag = 8 + randint1(8);
				break;
			case GF_DEC_ALL:
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
				break;

			case GF_SUPER_EGO:
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
				do_not_move = 16 + randint1(16);
				break;
			}

			//破滅の薬のdamを500程度にしておけばJにも1/2くらいで効く。25d25でも効かなくはない。
			//他の薬は100くらいでいいだろう

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.95 狂戦士化
		case GF_BERSERK:
		case GF_POSSESSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//通常の精神を持たない敵、クエスト打倒対象には効かない
			if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || r_ptr->flags1 & RF1_QUESTOR)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				break;
			}

			//怨霊憑依は妖怪に効きやすい
			if (typ == GF_POSSESSION && r_ptr->flags3 & RF3_KWAI) dam = dam * 3 / 2;

			//配下は抵抗せずに狂戦士化する
			if (is_pet(m_ptr))
			{
				do_berserk = 16 + randint1(dam / 4);
			}
			else
			{
				//混乱しない敵には効きづらい
				if (r_ptr->flags3 & RF3_NO_CONF )
				{
					dam = dam * 2 / 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
				//抵抗判定に失敗したら狂戦士化
				if (mon_saving_throw(m_ptr, dam))
				{
					note = _("は効果に抵抗した！", " resists the effect!");
					dam = 0;
					break;
				}

				do_berserk = 8 + randint1(dam / 8);

			}



			/* No "real" damage */
			dam = 0;
			break;
		}


		case GF_NO_MOVE: //移動禁止属性
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			//はぐれメタル以外確定で成功する。ただし脱出されやすい。
			//process_monsters_mtimed_aux(),process_monster()参照
			do_not_move = dam;

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.97 落とし穴属性 床トラップの発動と「深い穴生成」の魔法
		//TODO:てゐの落とし穴も統合したかったが色々挙動が違うので後回し
		case GF_PIT_FALL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				note = _("は悠々と穴の上を飛んでいる...", " leisurely floats above the chasm...");
				dam = 0;
				break;
			}

			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
				if (seen_msg) msg_format(_("%sの巨体が地響きを立てて穴に落ちた！", "%^s crashes down into the chasm with a deafening noise!"),m_name);
				dam *= 2;
			}
			else
			{
				if (seen_msg) msg_format(_("%sが穴に落ちた！", "%^s falls into the chasm!"), m_name);
			}

			do_stun = 8 + randint1(dam/10);

		}
		break;

		//v2.0.9 酒を呑ませる属性
		case GF_ALCOHOL:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = _("には効果がなかった！", " is unaffected!");
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//アルコール耐性
			//無生物は酔わない
			if (r_ptr->flags3 & RF3_NONLIVING)
			{
				dam = 0;
			}
			else if (dam < 0)//一撃で倒す特殊処理
			{
				dam = 30000;

			}
			//鬼やウワバミは非常に酒に強い
			else if (m_ptr->r_idx == MON_SUIKA || m_ptr->r_idx == MON_YUGI || m_ptr->r_idx == MON_KASEN || m_ptr->r_idx == MON_YUMA || m_ptr->r_idx == MON_UWABAMI)
			{
				dam /= 16;
				note = _("にはかなり耐性がある！", " resists a lot!");
			}

			//天狗は酒に強い
			else if (r_ptr->d_char == 't')
			{
				dam /= 4;
				note = _("には耐性がある。", " resists.");
			}
			/*
			//毒耐性で抵抗させようかと思ったがやめた
			else if (r_ptr->flagsr & RFR_IM_POIS)
			{
				dam /= 3;
				note = "には耐性がある。";

			}
			*/
			//巨大な敵と力強い敵は酔いにくい
			else if (r_ptr->flags2 & (RF2_GIGANTIC | RF2_POWERFUL))
			{
				dam /= 2;
				note = _("はいくらか耐性を示した。", " resists somewhat.");
			}

			//人間には効果3倍、人間型生物には効果2倍
			if (r_ptr->flags3 & RF3_HUMAN)
			{
				dam *= 3;

			}
			else if (r_ptr->flags3 & RF3_DEMIHUMAN)
			{
				dam *= 2;
			}

			if (!dam)
			{
				note = _("は全く酔う様子がない！", " isn't getting drunk at all!");
				break;
			}


			//泥酔度上昇
			set_monster_timed_status_add(MTIMED2_DRUNK, c_ptr->m_idx, MON_DRUNK(m_ptr) + dam);

			//泥酔度がモンスターの最大HPを超えたら即死ダメージ
			if (MON_DRUNK(m_ptr) > m_ptr->maxhp)
			{
				dam = m_ptr->maxhp + 1;
			}
			else
			{
				dam = 0;
			}
			break;
		}

		/* Default */
		default:
		{
			/* Irrelevant */
			skipped = TRUE;

			/* No damage */
			dam = 0;

			break;
		}



	}//switch(typ)終了

	/* Absolutely no effect */
	if (skipped) return (FALSE);

	/*:::Mega Hack - 付喪神の攻撃による経験値とアイテムは＠の攻撃扱いにする*/
	///mod140709 付喪神使い
	if(p_ptr->pclass == CLASS_TSUKUMO_MASTER && who > 0 && (caster_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && caster_ptr->r_idx <= MON_TSUKUMO_WEAPON5))
		who = 0;
	//mod140716 妖夢も同じにする
	if(p_ptr->pclass == CLASS_YOUMU && who > 0 && caster_ptr->r_idx == MON_YOUMU && (caster_ptr->mflag & MFLAG_EPHEMERA))
		who = 0;
	//mod140816 赤蛮奇の頭（物理攻撃・レーザー共に）も同じにする
	if(p_ptr->pclass == CLASS_BANKI && who > 0 &&
		(caster_ptr->r_idx == MON_BANKI_HEAD_1 || caster_ptr->r_idx == MON_BANKI_HEAD_2)
		&& (caster_ptr->mflag & MFLAG_EPHEMERA))
		who = 0;
	//赤蛮奇ビームをアリス用に再利用
	if(typ == GF_BANKI_BEAM2) who = 0;
	//v1.1.41 舞と里乃が踊って強化しているモンスターも同じにする。このときプレイヤーと配下の両方が経験値を得られるようフラグを立てる
	if (CLASS_RIDING_BACKDANCE && p_ptr->riding == who)
	{
		who = 0;

		if(p_ptr->pclass != CLASS_TSUKASA) flag_monster_and_player_gain_exp = TRUE;
	}

	//早苗のおみくじは自分もモンスターもダメージを受けるようにここでwhoを0にする
	//妹紅の自爆、サニーの爆弾も
	if(who == PROJECT_WHO_OMIKUJI_HELL || who == PROJECT_WHO_SUICIDAL_FIRE || who == PROJECT_WHO_EXPLODE_BOMB || who == PROJECT_WHO_DEEPECO_BOMB)
		who = 0;

	/* "Unique" monsters cannot be polymorphed */
	if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_VARIABLE)) do_poly = FALSE;

	/* Quest monsters cannot be polymorphed */
	if (r_ptr->flags1 & RF1_QUESTOR) do_poly = FALSE;

	if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_poly = FALSE;

	/* "Unique" and "quest" monsters can only be "killed" by the player. */
	///sys ユニークが＠の攻撃以外で一定以下にHP減らなくなるような処理はここに書く
	if (((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) || ((r_ptr->flags7 & RF7_VARIABLE ) && (r_ptr->flags7 & RF7_UNIQUE2)) || (r_ptr->flags7 & RF7_NAZGUL)) && (m_ptr->r_idx != MON_MASTER_KAGUYA) &&!p_ptr->inside_battle && !(m_ptr->mflag & MFLAG_EPHEMERA))
	{
		if (who && (dam > m_ptr->hp)) dam = m_ptr->hp;
	}

	//v1.1.22 最大ダメージを9999にする
	//v2.0.9 アルコールのときだけはこの制限を取り払う
	if(typ != GF_ALCOHOL && dam > 9999) dam = 9999;

	///sysdel virtue
	/*
	if (!who && slept)
	{
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}
	*/

	/* Modify the damage */
	tmp = dam;
	dam = mon_damage_mod(m_ptr, dam, (bool)(typ == GF_BLAZINGSTAR || typ == GF_PSY_SPEAR || typ == GF_GUNGNIR));
#ifdef JP
	if ((tmp > 0) && (dam == 0) && typ != GF_PHOTO) note = "はダメージを受けていない。";
#else
	if ((tmp > 0) && (dam == 0) && typ != GF_PHOTO) note = " is unharmed.";
#endif

	//清蘭特殊射撃
	if(special_shooting_mode == SSM_SEIRAN2 && !(r_ptr->flagsr & (RFR_RES_TELE | RFR_RES_TIME)) && dam)
	{
		do_dist = 10 + randint1(15) + randint1(p_ptr->lev);
	}

	/* Check for death */
	if (dam > m_ptr->hp)
	{
		/* Extract method of death */
		note = note_dies;
		//こころ面作成
		if(typ == GF_KOKORO) make_mask(m_ptr);

		//v1.1.49 鰻を焼くと食料になるようにしてみる
		//ミスティアはmonster_death()内のlシンボル打倒処理のところで処理するのでここでは除外
		if (m_ptr->r_idx == MON_EEL && p_ptr->pclass != CLASS_MYSTIA)
		{
			switch (typ)
			{
			case GF_FIRE:
			case GF_ELEC:
			case GF_PLASMA:
			case GF_NUKE:
			case GF_STEAM:
				{
					object_type forge;
					object_type *tmp_o_ptr = &forge;

					object_prep(tmp_o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_EEL));
					(void)drop_near(tmp_o_ptr, -1, m_ptr->fy, m_ptr->fx);


				}


			default:
				break;
			}
		}
	}
	else //モンスターがこの攻撃で死なないとき、ステータス異常付与判定など
	{
		/* Sound and Impact resisters never stun */
		//v1.1.90 轟音耐性で朦朧しなくなる仕様をなくした
		if (do_stun && !(r_ptr->flags3 & RF3_NO_STUN))
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Get stunned */
			if (MON_STUNNED(m_ptr))
			{
#ifdef JP
				note = "はひどくもうろうとした。";
#else
				note = " is more dazed.";
#endif

				tmp = MON_STUNNED(m_ptr) + (do_stun / 2);
			}
			else
			{
#ifdef JP
				note = "はもうろうとした。";
#else
				note = " is dazed.";
#endif

				tmp = do_stun;
			}

			/* Apply stun */
			(void)set_monster_stunned(c_ptr->m_idx, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		/* Confusion and Chaos resisters (and sleepers) never confuse */
		//v1.1.90 カオス耐性で混乱しなくなる仕様をなくした
		if (do_conf && !(r_ptr->flags3 & RF3_NO_CONF) )
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Already partially confused */
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
				note = "はさらに混乱したようだ。";
#else
				note = " looks more confused.";
#endif

				tmp = MON_CONFUSED(m_ptr) + (do_conf / 2);
			}

			/* Was not confused */
			else
			{
#ifdef JP
				note = "は混乱したようだ。";
#else
				note = " looks confused.";
#endif

				tmp = do_conf;
			}

			/* Apply confusion */
			(void)set_monster_confused(c_ptr->m_idx, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		if (do_dec_atk)
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_ATK(m_ptr))
			{
				tmp = MON_DEC_ATK(m_ptr) + do_dec_atk / 2;
			}
			else
			{
				note = _("は攻撃力が下がったようだ。", " has lost some attack power.");
				tmp = do_dec_atk;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_ATK, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		if (do_dec_def)
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_DEF(m_ptr))
			{
				tmp = MON_DEC_DEF(m_ptr) + do_dec_def / 2;
			}
			else
			{
				note = _("は防御力が下がったようだ。", " has lost some defense power.");
				tmp = do_dec_def;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_DEF, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		//魔法低下　魔法を持たないモンスターには効かない
		if (do_dec_mag && HAS_ANY_MAGIC(r_ptr))
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_MAG(m_ptr))
			{
				tmp = MON_DEC_MAG(m_ptr) + do_dec_mag / 2;
			}
			else
			{
				note = _("は魔法力が下がったようだ。", " has lost some magic power.");
				tmp = do_dec_mag;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_MAG, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		//破滅の薬投擲などで複数の能力が下がったときメッセージを変えておく
		if (do_dec_atk && do_dec_def || do_dec_atk && do_dec_mag || do_dec_def && do_dec_mag)
		{
			note = _("は複数の能力が下がったようだ。", " has all stats reduced.");
		}



		if (do_berserk)
		{
			if (seen) obvious = TRUE;

			if (MON_BERSERK(m_ptr))
			{
				tmp = MON_BERSERK(m_ptr) + do_berserk / 2;
			}
			else
			{
				note = _("は激しく暴れだした！", " goes into a violent rage!");
				tmp = do_berserk;
			}
			(void)set_monster_timed_status_add(MTIMED2_BERSERK, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		if (do_not_move)
		{
			if (seen) obvious = TRUE;

			if (MON_NO_MOVE(m_ptr))
			{
				tmp = MON_NO_MOVE(m_ptr) + do_not_move / 2;
			}
			else
			{
				note = _("は移動できなくなった！", " becomes unable to move!");
				tmp = do_not_move;
			}
			(void)set_monster_timed_status_add(MTIMED2_NO_MOVE, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}



		if (do_time)
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			if (do_time >= m_ptr->maxhp) do_time = m_ptr->maxhp - 1;

			if (do_time)
			{
#ifdef JP
				note = "は弱くなったようだ。";
#else
				note = " seems weakened.";
#endif
				m_ptr->maxhp -= do_time;
				if ((m_ptr->hp - dam) > m_ptr->maxhp) dam = m_ptr->hp - m_ptr->maxhp;
			}
			get_angry = TRUE;
		}

		/* Mega-Hack -- Handle "polymorph" -- monsters get a saving throw */
		if (do_poly && (randint1(90) > r_ptr->level))
		{
			if (polymorph_monster(y, x))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Monster polymorphs */
#ifdef JP
				note = "が変身した！";
#else
				note = " changes!";
#endif

				/* Turn off the damage */
				dam = 0;
			}
			else
			{
				/* No polymorph */
#ifdef JP
				note = "には効果がなかった！";
#else
				note = " is unaffected!";
#endif
			}

			/* Hack -- Get new monster */
			m_ptr = &m_list[c_ptr->m_idx];

			/* Hack -- Get new race */
			r_ptr = &r_info[m_ptr->r_idx];
		}

		/* Handle "teleport" */
		//電車と竜巻は別メッセージ
		if (do_dist && !(p_ptr->riding && c_ptr->m_idx == p_ptr->riding) )
		{

			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
#ifdef JP
			if(typ == GF_BLAZINGSTAR) note = "は吹っ飛ばされた！";
			else if(typ != GF_TRAIN && typ != GF_TORNADO) note = "が消え去った！";
#else
			if(typ == GF_BLAZINGSTAR) note = " was blown away!";
			else if(typ != GF_TRAIN && typ != GF_TORNADO) note = " disappears!";
#endif

		//	if (!who) chg_virtue(V_VALOUR, -1);

			/* Teleport */
			teleport_away(c_ptr->m_idx, do_dist,
						(!who ? TELEPORT_DEC_VALOUR : 0L) | TELEPORT_PASSIVE);

			/* Hack -- get new location */
			y = m_ptr->fy;
			x = m_ptr->fx;

			/* Hack -- get new grid */
			c_ptr = &cave[y][x];
		}

		/* Fear */
		//v1.1.90 恐怖しないモンスターの条件追加　
		if (do_fear && !(r_ptr->flags3 & RF3_NO_FEAR))
		{
			/* Set fear */
			(void)set_monster_monfear(c_ptr->m_idx, MON_MONFEAR(m_ptr) + do_fear);

			/* Get angry */
			get_angry = TRUE;
		}
	}

	if (typ == GF_DRAIN_MANA)
	{
		/* Drain mana does nothing */
	}

	/* If another monster did the damage, hurt the monster by hand */
	else if (who)
	{
		/* Redraw (later) if needed */
		if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
		if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);

		/* Wake the monster up */
		(void)set_monster_csleep(c_ptr->m_idx, 0);



		/* Hurt the monster */
		m_ptr->hp -= dam;

		//輝夜交戦中メッセージ ないと思うがゼロ除算避けの条件式つけとく
		if(m_ptr->r_idx == MON_MASTER_KAGUYA && !m_ptr->ml && m_ptr->max_maxhp)
		{
			kaguya_fight = (byte) (m_ptr->hp * 100 / m_ptr->max_maxhp);
		}

		/* Dead monster */
		if (m_ptr->hp < 0)
		{
			bool sad = FALSE;
			bool parsee = FALSE;
			bool mine = FALSE;

			if (is_pet(m_ptr) && !(m_ptr->ml))
				sad = TRUE;

			//パルスィの幻影はモンスターに倒されると爆発する
			if(m_ptr->r_idx == MON_PARSEE && (m_ptr->mflag & MFLAG_EPHEMERA))
				parsee = TRUE;

			if(m_ptr->r_idx == MON_SAGUME_MINE) mine = TRUE;
			//v1.1.24
			else if(m_ptr->r_idx == MON_YOUR_BOAT || m_ptr->r_idx == MON_ANCIENT_SHIP)
				note = _("は沈んだ。", " sunk.");

			/* Give detailed messages if destroyed */
			if (known && note)
			{
				monster_desc(m_name, m_ptr, MD_TRUE_NAME);
				if (see_s_msg)
				{
					msg_format("%^s%s", m_name, note);
				}
				else
				{
					mon_fight = TRUE;
				}
			}

			if (who > 0) monster_gain_exp(who, m_ptr->r_idx);

			//v1.1.48 紫苑はフロアでモンスターが倒れたときその経験値相当の値を「不運パワー」として蓄積する
			if (p_ptr->pclass == CLASS_SHION)
			{
				p_ptr->magic_num1[1] += r_info[m_ptr->r_idx].mexp;

			}

			//輝夜が倒されたときの敵を記録
			if(p_ptr->pclass == CLASS_EIRIN && m_ptr->r_idx == MON_MASTER_KAGUYA)
			{
				if(who > 0) p_ptr->magic_num1[0] = m_list[who].r_idx;
				else p_ptr->magic_num1[0] = who;
			}

			/* Generate treasure, etc */
			monster_death(c_ptr->m_idx, FALSE);

			/* Delete the monster */
			delete_monster_idx(c_ptr->m_idx);

			if(parsee)
			{
				msg_print(_("あなたの幻影は爆発した！", "Your phantom explodes!"));
				project(0, 5, y,x, 100 + p_ptr->lev * 10, GF_DISP_ALL, PROJECT_KILL | PROJECT_GRID, -1);


			}
			else if (sad)
			{
				if(mine)
					msg_print(_("機雷が何かを補足したようだ。", "Someone has triggered the landmine."));
				else
					msg_print(_("少し悲しい気分がした。", "You feel a bit sad."));
			}
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen_msg) msg_format("%^s%s", m_name, note);

			/* Hack -- Pain message */
			/*:::ダメージに対するモンスターのリアクションを記述*/
			else if (see_s_msg)
			{
				message_pain(c_ptr->m_idx, dam);
			}
			else
			{
				mon_fight = TRUE;
			}

			/* Hack -- handle sleep */
			//ほかのステータス異常と違ってRF3_NO_SLEEPの判定をしていないのはGF_STASIS系の処理と共用するためか
			if (do_sleep) (void)set_monster_csleep(c_ptr->m_idx, do_sleep);
		}
	}

	///sysdel 病人を治す
	else if (heal_leper)
	{
#ifdef JP
		if (seen_msg) msg_print("不潔な病人は病気が治った！");
#else
		if (seen_msg) msg_print("The Mangy looking leper is healed!");
#endif

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_HEAL_LEPER, m2_name);
		}

		delete_monster_idx(c_ptr->m_idx);
	}

	/* If the player did it, give him experience, check fear */
	/*:::＠による攻撃*/
	else
	{
		bool fear = FALSE;

		int tmp_r_idx = m_ptr->r_idx;

		/* Hurt the monster, check for fear and death */
		if (mon_take_hit(c_ptr->m_idx, dam, &fear, note_dies))
		{
			/* Dead monster */

			if (flag_monster_and_player_gain_exp) monster_gain_exp(p_ptr->riding, tmp_r_idx);
		}

		/* Damaged monster */
		else
		{
			/* HACK - anger the monster before showing the sleep message */
			if (do_sleep) anger_monster(m_ptr);

			if((m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_MOKOU) && m_ptr->hp >= 0)
			{
				//何もしない　正邪や妹紅が復活したとき「倒した」とメッセージが出るのを抑止する
			}
			/* Give detailed messages if visible or destroyed */
			else if (note && seen_msg)
#ifdef JP
				msg_format("%s%s", m_name, note);
#else
				msg_format("%^s%s", m_name, note);
#endif


			/* Hack -- Pain message */
			//v1.1.43 do_fearの条件式を消した
			//else if (known && (dam || !do_fear))
			else if (known && dam)
			{
				message_pain(c_ptr->m_idx, dam);
			}

			/* Anger monsters */
			if (((dam > 0) || get_angry) && !do_sleep)
				anger_monster(m_ptr);


			/* Hack -- handle sleep */
			//ほかのステータス異常と違ってRF3_NO_SLEEPの判定をしていないのはGF_STASIS系の処理と共用するためか
			if (do_sleep) (void)set_monster_csleep(c_ptr->m_idx, do_sleep);
			/* Take note */
			else if ((fear || do_fear) && seen)
			{
				sound(SOUND_FLEE);
				msg_format(_("%^sは恐怖して逃げ出した！", "%^s flees in terror!"), m_name);
			}
		}
	}

	if ((typ == GF_BLOOD_CURSE) && one_in_(4))
	{
		int curse_flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);
		int count = 0;
		do
		{
			switch (randint1(28))
			{
			case 1: case 2:
				if (!count)
				{
#ifdef JP
msg_print("地面が揺れた...");
#else
					msg_print("The ground trembles...");
#endif

					earthquake(ty, tx, 4 + randint0(4));
					if (!one_in_(6)) break;
				}
			case 3: case 4: case 5: case 6:
				if (!count)
				{
					int dam = damroll(10, 10);
#ifdef JP
msg_print("純粋な魔力の次元への扉が開いた！");
#else
					msg_print("A portal opens to a plane of raw mana!");
#endif

					project(0, 8, ty,tx, dam, GF_MANA, curse_flg, -1);
					if (!one_in_(6)) break;
				}
			case 7: case 8:
				if (!count)
				{
#ifdef JP
msg_print("空間が歪んだ！");
#else
					msg_print("Space warps about you!");
#endif

					if (m_ptr->r_idx) teleport_away(c_ptr->m_idx, damroll(10, 10), TELEPORT_PASSIVE);
					if (one_in_(13)) count += activate_hi_summon(ty, tx, TRUE);
					if (!one_in_(6)) break;
				}
			case 9: case 10: case 11:
#ifdef JP
msg_print("エネルギーのうねりを感じた！");
#else
				msg_print("You feel a surge of energy!");
#endif

				project(0, 7, ty, tx, 50, GF_DISINTEGRATE, curse_flg, -1);
				if (!one_in_(6)) break;
			case 12: case 13: case 14: case 15: case 16:
				aggravate_monsters(0,TRUE);
				if (!one_in_(6)) break;
			case 17: case 18:
				count += activate_hi_summon(ty, tx, TRUE);
				if (!one_in_(6)) break;
			case 19: case 20: case 21: case 22:
			{
				bool pet = !one_in_(3);
				u32b mode = PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_NO_PET | PM_FORCE_FRIENDLY);

				count += summon_specific((pet ? -1 : 0), py, px, (pet ? p_ptr->lev*2/3+randint1(p_ptr->lev/2) : dun_level), 0, mode);
				if (!one_in_(6)) break;
			}
			case 23: case 24: case 25:
				///mod131228 生命力維持削除
				//if (p_ptr->hold_life && (randint0(100) < 75)) break;
				if (p_ptr->resist_neth && (randint0(100) < 75)) break;

#ifdef JP
msg_print("生命力が体から吸い取られた気がする！");
#else
				msg_print("You feel your life draining away...");
#endif

				//if (p_ptr->hold_life) lose_exp(p_ptr->exp / 160);
				if (p_ptr->resist_neth) lose_exp(p_ptr->exp / 160);

				else lose_exp(p_ptr->exp / 16);
				if (!one_in_(6)) break;
			case 26: case 27: case 28:
			{
				int i = 0;
				if (one_in_(13))
				{
					while (i < 6)
					{
						do
						{
							(void)do_dec_stat(i);
						}
						while (one_in_(2));

						i++;
					}
				}
				else
				{
					(void)do_dec_stat(randint0(6));
				}
				break;
			}
			}
		}
		while (one_in_(5));
	}

	if (p_ptr->inside_battle)
	{
		p_ptr->health_who = c_ptr->m_idx;
		p_ptr->redraw |= (PR_HEALTH);
		redraw_stuff();
	}

	/* XXX XXX XXX Verify this code */

	/* Update the monster */
	if (m_ptr->r_idx) update_mon(c_ptr->m_idx, FALSE);

	/* Redraw the monster grid */
	lite_spot(y, x);


	/* Update monster recall window */
	if ((p_ptr->monster_race_idx == m_ptr->r_idx) && (seen || !m_ptr->r_idx))
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}

	if ((dam > 0) && !is_pet(m_ptr) && !is_friendly(m_ptr))
	{
		if (!who)
		{
			if (!(flg & PROJECT_NO_HANGEKI))
			{
				set_target(m_ptr, monster_target_y, monster_target_x);
			}
		}
		else if ((who > 0) && is_pet(caster_ptr) && !player_bold(m_ptr->target_y, m_ptr->target_x))
		{
			set_target(m_ptr, caster_ptr->fy, caster_ptr->fx);
		}
	}

	if (p_ptr->riding && (p_ptr->riding == c_ptr->m_idx) && (dam > 0))
	{
		if (m_ptr->hp > m_ptr->maxhp/3) dam = (dam + 1) / 2;
		rakubadam_m = (dam > 200) ? 200 : dam;
	}

	if (photo)
	{

		if(IS_RANDOM_UNIQUE_IDX(photo))
		{
			if(!who) msg_print(_("なぜかうまく写らなかった。", "For some reason, the photo didn't come out well."));
		}
		else
		{

			object_type *q_ptr;
			object_type forge;

			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_PHOTO, 0));
			q_ptr->pval = photo;
			q_ptr->ident |= (IDENT_MENTAL);
			(void)drop_near(q_ptr, -1, py, px);
		}
	}
	/* Track it */
	project_m_n++;
	project_m_x = x;
	project_m_y = y;

	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_m()" above.
 *
 * If "rad" is non-zero, then the blast was centered elsewhere, and the damage
 * is reduced (see "project_m()" above).  This can happen if a monster breathes
 * at the player and hits a wall instead.
 *
 * NOTE (Zangband): 'Bolt' attacks can be reflected back, so we need
 * to know if this is actually a ball or a bolt spell
 *
 *
 * We return "TRUE" if any "obvious" effects were observed.  XXX XXX Actually,
 * we just assume that the effects were obvious, for historical reasons.
 */
/*:::＠に対するボルト・ボール・ビーム・ブレスのダメージ処理*/
/*:::monspell:モンスターの攻撃魔法ID？青魔法ラーニング用？*/
///sys class res race ＠に対するダメージ処理　重要
///mod131229 水、破邪、コズミックホラーなど暫定的に追加　耐性適用を変更
bool project_p(int who, cptr who_name, int r, int y, int x, int dam, int typ, int flg, int monspell)
{
	int k = 0;
	int rlev = 0;

	/* Hack -- assume obvious */
	bool obvious = TRUE;

	/* Player blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Player needs a "description" (he is blind) */
	bool fuzzy = FALSE;

	/* Source monster */
	monster_type *m_ptr = NULL;

	/* Monster name (for attacks) */
	char m_name[80];

	/* Monster name (for damage) */
	char killer[80];

	/* Hack -- messages */
	cptr act = NULL;

	int get_damage = 0;


	/* Player is not here */
	/*:::変わり身中なら確率で回避し処理終了*/
	if (!player_bold(y, x)) return (FALSE);

	if (p_ptr->pclass == CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (p_ptr->lev+20))&& who && (who != p_ptr->riding))
	{
		if (kawarimi(TRUE)) return TRUE;
	}
	else if (p_ptr->pclass != CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && dam && (randint0(55) < (p_ptr->lev*3/5+20)) && who && (who != p_ptr->riding))
	{
		if (kawarimi(TRUE)) return FALSE;
	}

	//v1.1.77 お燐(専用性格)の追跡　
	if (orin_escape(who)) return TRUE;


	//諏訪子冬眠中無敵
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		if( typ != GF_PSY_SPEAR && typ != GF_DISINTEGRATE)
		{
			msg_print(_("しかしあなたには届かなかった。", "But it didn't reach you."));
			return FALSE;
		}
		else
		{
			msg_print(_("攻撃は地中にまで届いた！", "The attack reached below the ground!"));
			set_tim_general(0,TRUE,0,1);
		}
	}

	/* Player cannot hurt himself */
	if (!who) return (FALSE);
	if (who == p_ptr->riding) return (FALSE);

	//v1.1.17 純狐一時スキル
	//v1.1.52 菫子新性格追加
	//v2.0.4bスペカや勾玉が無効だったので修正
	if(check_activated_nameless_arts(JKF1_VANISH_BEAM) && typ != GF_PSY_SPEAR &&
		(flg & (PROJECT_BEAM | PROJECT_MASTERSPARK | PROJECT_REFLECTABLE)))
		{
			msg_print(_("あなたは攻撃を無効化した！", "You have negated the attack!"));
			return FALSE;
		}


	/*:::反射処理*/
	///＠の反射処理　鴉天狗のボルト無効化もここに記述か
	if ((p_ptr->reflect || ((p_ptr->special_defense & KATA_FUUJIN) && !p_ptr->blind)) && (flg & PROJECT_REFLECTABLE) && !one_in_(10))
	{
		byte t_y, t_x;
		int max_attempts = 10;

#ifdef JP
		if (blind) msg_print("何かが跳ね返った！");
		else if (p_ptr->special_defense & KATA_FUUJIN) msg_print("風の如く武器を振るって弾き返した！");
		else msg_print("攻撃が跳ね返った！");
#else
		if (blind) msg_print("Something bounces!");
		else if (p_ptr->special_defense & KATA_FUUJIN) msg_print("You swing your weapon like the wind, deflecting the attack!");
		else msg_print("The attack bounces!");
#endif


		/* Choose 'new' target */
		if (who > 0)
		{
			do
			{
				t_y = m_list[who].fy - 1 + randint1(3);
				t_x = m_list[who].fx - 1 + randint1(3);
				max_attempts--;
			}
			while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(py, px, t_y, t_x));

			if (max_attempts < 1)
			{
				t_y = m_list[who].fy;
				t_x = m_list[who].fx;
			}
		}
		else
		{
			t_y = py - 1 + randint1(3);
			t_x = px - 1 + randint1(3);
		}

		project(0, 0, t_y, t_x, dam, typ, (PROJECT_STOP|PROJECT_KILL|PROJECT_REFLECTABLE), monspell);

		disturb(1, 1);
		return TRUE;
	}
	///mod140316 鴉天狗ボルト消去
	///mod140608 高レベルのチルノはボルトを冷凍
	//v1.1.52 菫子特殊性格もスマホを持っているときに限りボルト消去
	/*:::鴉天狗はボルトを消去することがある ニンジャはボルトを回避する*/
	if (((p_ptr->pclass == CLASS_CIRNO && p_ptr->lev > 39)
		|| p_ptr->prace == RACE_KARASU_TENGU
		|| p_ptr->prace == RACE_NINJA
		|| is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) && check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE) >= 0
		|| p_ptr->mimic_form == MIMIC_MIST)
		&& !p_ptr->blind && (flg & PROJECT_REFLECTABLE) )
	{
		m_ptr = &m_list[who];
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);
		if(!one_in_(10) && randint1(rlev*3/2) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_DEX]]*2)
		{
			if(p_ptr->prace == RACE_NINJA)
				msg_print(_("「イヤーッ！」あなたはブリッジでこれを回避！", "You evade the enemy attack!"));
			else if(p_ptr->mimic_form == MIMIC_MIST)
				msg_print(_("敵弾はあなたの体をすり抜けた！", "The enemy bullets go through your body!"));
			else if(p_ptr->pclass == CLASS_CIRNO)
				msg_print(_("あなたは敵弾を凍らせた！", "You freeze the enemy bullets!"));
			else
				msg_print(_("あなたは敵の弾を撮影して消した！", "You take a photo of the enemy bullets, clearing them!"));

			return (FALSE);
		}
	}
	///mod141116 針妙丸新機能　ボルトを小判に変換
	else if ((p_ptr->pclass == CLASS_SHINMYOUMARU) && !p_ptr->blind && (flg & PROJECT_REFLECTABLE) && !p_ptr->tim_general[0] && !p_ptr->magic_num1[0])
	{
		m_ptr = &m_list[who];
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);
		if(!one_in_(10) && randint1(rlev*3/2) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_DEX]]*2)
		{
			msg_print(_("小槌を振ると敵弾は小判へと変化した！", "You swing your mallet, turning enemy bullets into money!"));
			p_ptr->au += dam;
			p_ptr->redraw |= (PR_GOLD);
			return (FALSE);
		}
	}
	//v1.1.30
	else if ((p_ptr->pclass == CLASS_YOUMU) && !p_ptr->blind && (flg & PROJECT_REFLECTABLE))
	{
		int chance = 0;
		if(inventory[INVEN_RARM].tval == TV_KATANA) chance += 20 + p_ptr->lev;
		if(inventory[INVEN_LARM].tval == TV_KATANA) chance += 20 + p_ptr->lev;
		if(randint0(100) < chance)
		{
			msg_print(_("あなたは敵弾を切り払った！", "You cut through enemy bullets!"));
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_CAMERA] &&  !p_ptr->blind && !p_ptr->confused && !p_ptr->paralyzed && (flg & PROJECT_REFLECTABLE) && !one_in_(10) )
	{
		msg_print(_("あなたは敵の弾を撮影して消した！", "You take a photo of the enemy bullets, clearing them!"));
		p_ptr->magic_num1[SEIJA_ITEM_CAMERA] -= 1;
		return (FALSE);

	}
	///mod150315 妖精特技
	else if(CHECK_FAIRY_TYPE == 2 && (flg & PROJECT_REFLECTABLE) && one_in_(3))
	{
		msg_print(_("あなたは敵弾をグレイズした！", "You graze the enemy bullets!"));
		return (FALSE);
	}




	if(CHECK_RINGO_IMMUNE(typ))
	{
		msg_print(_("あなたは完全な免疫を示した！", "You are fully immune!"));
		return (FALSE);
	}

	/* XXX XXX XXX */
	/* Limit maximum damage */
	if (dam > 1600) dam = 1600;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* If the player is blind, be more descriptive */
	if (blind) fuzzy = TRUE;


	if (who > 0)
	{
		/* Get the source monster */
		m_ptr = &m_list[who];
		/* Extract the monster level */
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);

		/* Get the monster name */
		monster_desc(m_name, m_ptr, 0);

		/* Get the monster's real name (gotten before polymorph!) */
		strcpy(killer, who_name);
	}
	else
	{
		/*:::一部の死因用に負の値で定義されている*/
		switch (who)
		{
		case PROJECT_WHO_UNCTRL_POWER:
#ifdef JP
			strcpy(killer, "制御できない力の氾流");
#else
			strcpy(killer, "uncontrollable power storm");
#endif
			break;

		case PROJECT_WHO_GLASS_SHARDS:
#ifdef JP
			strcpy(killer, "ガラスの破片");
#else
			strcpy(killer, "shards of glass");
#endif
			break;
		case PROJECT_WHO_OMIKUJI_HELL:
			strcpy(killer, _("大凶のおみくじ", "omikuji of disaster"));
			break;
		case PROJECT_WHO_SUICIDAL_FIRE:
			strcpy(killer, _("自分の起こした炎", "flame caused by yourself"));
			break;
		case PROJECT_WHO_EXPLODE_BOMB:
			strcpy(killer, _("自分の仕掛けた爆弾", "your own bomb"));
			break;
		case PROJECT_WHO_DEEPECO_BOMB:
			strcpy(killer, _("自分の投げた爆弾", "your own thrown bomb"));
			break;
		case PROJECT_WHO_CONTACT_YOG:
			strcpy(killer, _("『ヨグ＝ソトート』との接触", "contact with Yog-Sothoth"));
			rlev = 90;
			break;
		case PROJECT_WHO_TRAP:
			strcpy(killer, _("罠", "trap"));
			break;
		case PROJECT_WHO_EXPLODE_ICE:
			strcpy(killer, _("氷の破片", "shards of ice"));
			break;


		default:
#ifdef JP
			strcpy(killer, "(その他)");
#else
			strcpy(killer, "(other)");
#endif
			break;
		}

		/* Paranoia */
		strcpy(m_name, killer);
	}

	/* Analyze the damage */
	///sys res 属性とアイテム破壊　亜空間球とか死に誘うとか追加要
	switch (typ)
	{
		/* Standard damage -- hurts inventory too */
		case GF_ACID:
		{
#ifdef JP
			if (fuzzy) msg_print("酸で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by acid!");
#endif

			get_damage = acid_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("火炎で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by fire!");
#endif

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);


			get_damage = fire_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_COLD:
		{
#ifdef JP
			if (fuzzy) msg_print("冷気で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by cold!");
#endif

			get_damage = cold_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_ELEC:
		{
#ifdef JP
			if (fuzzy) msg_print("電撃で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by lightning!");
#endif

			get_damage = elec_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- also poisons player */
		case GF_POIS:
		{
			bool double_resist = IS_OPPOSE_POIS();
#ifdef JP
			if (fuzzy) msg_print("毒で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by poison!");
#endif

			if(p_ptr->pclass == CLASS_MEDICINE)
			{
				msg_print(_("体が毒で満たされる。", "You body is filled with poison."));
				hp_player(dam / 3);
				set_food(p_ptr->food + dam*10);
				break;
			}
			if(p_ptr->pclass == CLASS_EIRIN)
			{
				msg_print(_("あなたには完全な免疫がある！", "You are fully immune!"));
				dam=0;
				break;
			}

			if (p_ptr->resist_pois) dam = (dam + 2) / 3;
			if (double_resist) dam = (dam + 2) / 3;

			if ((!(double_resist || p_ptr->resist_pois)) &&
			     one_in_(HURT_CHANCE) && !CHECK_MULTISHADOW())
			{
				do_dec_stat(A_CON);
			}
			///mod140813 喘息悪化
			if ((p_ptr->muta2 & MUT2_ASTHMA) && !CHECK_MULTISHADOW())
			{
				set_asthma(p_ptr->asthma + dam * 20);
				if (!(double_resist || p_ptr->resist_pois)) set_asthma(p_ptr->asthma + 500 + randint0(1000) );
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (!(double_resist || p_ptr->resist_pois) && !CHECK_MULTISHADOW())
			{
				set_poisoned(p_ptr->poisoned + randint0(dam) + 10);
			}
			break;
		}

		/* 汚染ダメージ　毒+劣化　どちらの耐性もないと全能力減少 */
		case GF_POLLUTE:
		{
			int dam_pois, dam_disen;
			bool double_resist = IS_OPPOSE_POIS();
#ifdef JP
			if (fuzzy) msg_print("何か極めて有害な物質で攻撃された！");
#else
            if (fuzzy) msg_print("You are hit by something extremely harmful!");
#endif
			dam_pois = dam / 2 + 1;
			dam_disen = dam / 2 + 1;

			if (p_ptr->resist_pois) dam_pois = (dam_pois + 2) / 3;
			if (double_resist) dam_pois = (dam_pois + 2) / 3;

			if(p_ptr->pclass == CLASS_EIRIN)
				dam_pois = 0;
			else
				get_damage = take_hit(DAMAGE_ATTACK, dam_pois, killer, monspell);

			if (!(double_resist || p_ptr->resist_pois) && !CHECK_MULTISHADOW())
				set_poisoned(p_ptr->poisoned + randint0(dam) + 10);

			if(p_ptr->is_dead) break;

			dam_disen = mod_disen_dam(dam_disen);

			if ( !p_ptr->resist_disen && !CHECK_MULTISHADOW())
			{
				(void)apply_disenchant(0,0);
			}
			get_damage += take_hit(DAMAGE_ATTACK, dam_disen, killer, monspell);

			if ((!(double_resist || p_ptr->resist_pois || p_ptr->resist_disen)) && !CHECK_MULTISHADOW())
			{
				do_dec_stat(A_STR);
				do_dec_stat(A_INT);
				do_dec_stat(A_WIS);
				do_dec_stat(A_DEX);
				do_dec_stat(A_CON);
				do_dec_stat(A_CHR);
			}

			break;
		}

		/* Standard damage -- also poisons / mutates player */
		/*:::放射能：耐性がなければ16%で自己変容、4%で突然変異、さらに17%で装備侵食*/
		///sys 放射能属性は閃光・火炎の複属性に変更
		case GF_NUKE:
		{
			int dam_fire, dam_lite;
			bool double_resist = IS_OPPOSE_FIRE();
#ifdef JP
			if (fuzzy) msg_print("強力な熱線で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by intense heat!");
#endif

			dam_fire = dam / 2;
			dam_lite = dam / 2;

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam_fire = oil_field_damage_mod(dam_fire, y, x);


			//v1.1.59サニーミルクが光を取り込む
			sunny_charge_sunlight(dam_lite);


			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
				dam_lite = 0;
			else if(p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
				dam_lite = 0;
			else dam_lite = mod_lite_dam(dam_lite);

			if (!blind && !p_ptr->resist_blind && !p_ptr->resist_lite && !CHECK_MULTISHADOW() && dam_lite)
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam_lite, killer, monspell);

			if (dam_lite && p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print(_("閃光のため非物質的な影の存在でいられなくなった。",
                            "The light forces you out of your incorporeal shadow form."));

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}


			if(!p_ptr->is_dead)get_damage += fire_dam(dam_fire, killer, 0);

			break;
		}

		/* Standard damage */
		case GF_MISSILE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			//v2.0.17 残無虚無操作による無属性耐性
			if (p_ptr->pclass == CLASS_ZANMU && (p_ptr->tim_general[0]))
			{
				dam = (dam + 1) / 2;
				msg_print(_("あなたには耐性がある！", "You resist!"));
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Holy Orb -- Player only takes partial damage */
		///sys res 神聖攻撃
		case GF_HOLY_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("何か清らかなもので攻撃された！");
			//if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something purifying!");
#endif
			///mod140103 破邪耐性・弱点考慮用関数作った
			dam = mod_holy_dam(dam);

			if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
			{
				hina_gain_yaku(-(dam * 4));
				if(!p_ptr->magic_num1[0])
					msg_print(_("破邪攻撃で厄が全て消えてしまった！", "All of your misfortune vanished from the holy energy!"));
				else
					msg_print(_("破邪攻撃で厄が少し消えてしまった！", "Some of your misfortune vanishes from the holy energy!"));
			}

			//v2.1.0 怨霊の応援は破邪系攻撃を受けると消える
			if (!CHECK_MULTISHADOW() && (p_ptr->muta4 & MUT4_GHOST_CHEERS))
			{
				lose_mutation(218);
			}

			/*
			if (p_ptr->align > 10)
				dam /= 2;
			else if (p_ptr->align < -10)
				dam *= 2;
			*/
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
#ifdef JP
				msg_print("破邪の光を受けて非物質的な影の存在でいられなくなった。");
#else
				msg_print("The holy light forces you out of your incorporeal shadow form.");
#endif

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}


			break;
		}

		case GF_HELL_FIRE:
		///sys res ヘルファイア攻撃
		{
#ifdef JP
			if (fuzzy) msg_print("何か汚らわしいもので攻撃された！");
			//if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something filthy!");
#endif


			if(p_ptr->special_defense & SD_HELLFIRE_BARRIER)
			{
				msg_print(_("大いなる慈悲があなたを地獄の業火から守った！",
                            "You are protected from the fires of hell!"));
				dam = 0;
				p_ptr->special_defense &= ~(SD_HELLFIRE_BARRIER);
				p_ptr->redraw |= (PR_STATUS);
				break;
			}

			if (prace_is_(RACE_TENNIN) || prace_is_(RACE_LUNARIAN))
			{
				msg_print(_("地獄の業火があなたの清浄な肉体を深く傷つけた！",
                            "The hellfire deeply wounds your pure body!"));
				dam = dam * 4 / 3;
			}
			else if ((prace_is_(RACE_GYOKUTO)&&p_ptr->pclass != CLASS_UDONGE))
			{
				msg_print(_("地獄の業火があなたの清浄な肉体を傷つけた！",
                            "The hellfire wounds your pure body!"));
				dam = dam * 6 / 5;
			}
			else if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_HELL)
			{
				msg_print(_("地獄の業火があなたの清浄な肉体を深く傷つけた！",
                            "The hellfire deeply wounds your pure body!"));
				dam = dam * 3 / 2;
			}

			if(p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
			{
				dam = dam * 6 / (randint1(5) + 7);
			}

			if (p_ptr->pclass == CLASS_HECATIA || p_ptr->pclass == CLASS_ZANMU && p_ptr->lev > 39)
			{
				msg_print(_("あなたには耐性がある。", "You resist."));
				dam /= 2;
			}


			/*
			if (p_ptr->align > 10)
				dam *= 2;
			*/
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Arrow -- XXX no dodging */
		case GF_ARROW:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif

			else if ((!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_LARM].name1 == ART_ZANTETSU))
			{
#ifdef JP
				msg_print("矢を斬り捨てた！");
#else
				msg_print("You cut down the arrow!");
#endif
				break;
			}

			if(p_ptr->kamioroshi & KAMIOROSHI_KANAYAMAHIKO)
			{
				msg_print(_("しかし金山彦命がそれを分解した！",
                            "However, Kanayamahiko-no-Mikoto breaks it down!"));
				break;
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Plasma -- XXX No resist */
	///sys res プラズマ
		///mod140103 プラズマ耐性　火電で半分ずつ処理　轟音朦朧は少し軽減
		case GF_PLASMA:
		{

#ifdef JP
			if (fuzzy) msg_print("何かとても熱いもので攻撃された！");
#else
            if (fuzzy) msg_print("You are hit by something very hot!");
#endif

			//v1.1.91 石油地形火炎ダメージ
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);


			get_damage = fire_dam(dam/2, killer, monspell);
			if(!p_ptr->is_dead)get_damage +=  elec_dam(dam/2, killer, monspell);

			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				int k = (randint1((dam > 50) ? 25 : (dam / 2 + 1)));
				(void)set_stun(p_ptr->stun + k);
			}
			break;
		}

		/* Nether -- drain experience */
		/*:::地獄属性　幽霊の吸収処理も*/
		case GF_NETHER:
		{
#ifdef JP
			if (fuzzy) msg_print("地獄の力で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by nether forces!");
#endif
			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				}
				else
				{
					yaku = randint1(dam);
					msg_print(_("あなたは攻撃の一部を厄として吸収した。", "You partially absorb the attack as misfortune."));
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}
			if (prace_is_(RACE_LUNARIAN))
			{
				msg_print(_("あなたの肉体は穢れた力に傷つけられた！",
                            "Your body is wounded by the impure forces!"));
				dam = dam * 4 / 3;
			}

			//死者の神の地獄免疫
			if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 61)
			{
				dam = 0;
				msg_print(_("あなたには完全な耐性がある！", "You are fully immune!"));
				learn_spell(monspell);
				break;
			}
			//v1.1.15 ヘカ地獄免疫
			//v2.0.17 残無も追加
			else if(p_ptr->pclass == CLASS_HECATIA || (p_ptr->pclass == CLASS_ZANMU && p_ptr->lev > 29))
			{
				dam = 0;
				msg_print(_("あなたには完全な耐性がある！", "You are fully immune!"));
				learn_spell(monspell);
				break;
			}

			else if (prace_is_(RACE_HANIWA))
			{
				dam = 0;
				msg_print(_("あなたには完全な耐性がある！", "You are fully immune!"));
				learn_spell(monspell);
				break;
			}
			else if (p_ptr->resist_neth)
			{
				if (!(prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_YUMA))
					dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW()) drain_exp(200 + (p_ptr->exp / 100), 200 + (p_ptr->exp / 1000), 75);

			if ((p_ptr->pclass == CLASS_YOSHIKA || prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_YUMA) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("地獄の力を吸収した！");
#else
				msg_print("You feel invigorated!");
#endif

				hp_player(dam / 4);
				learn_spell(monspell);
			}
			else
			{
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}

			break;
		}

		/* Water -- stun/confuse */
		///mod131229 sys res 水攻撃
		case GF_WATER:
		case GF_HOLY_WATER: //一応追加しておくが＠が受ける予定はない。この属性の攻撃を敵がするなら破邪弱点処理を追加すること
		{
#ifdef JP
			if (fuzzy) msg_print("激流で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something wet!");
#endif
			dam = mod_water_dam(dam);

			if( dam == 0 ) break;

			//if (p_ptr->resist_water)
			//{
			//	dam /= 2;
			//}
			//if (!CHECK_MULTISHADOW())

			if (p_ptr->chp >= dam && p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0] && !CHECK_MULTISHADOW())
			{
				msg_print(_("水を被って式が剥がれてしまった！", "You lose your shikigami power after getting wet!"));
				p_ptr->magic_num1[0] = 1;
				p_ptr->update |= (PU_BONUS);
			}
			//v2.0.20 藍も水を受けたら少し頭が悪くなる
			else if (p_ptr->chp >= dam && p_ptr->pclass == CLASS_RAN && !CHECK_MULTISHADOW() && (!p_ptr->resist_water || one_in_(3)) )
			{
				msg_print(_("水を被って式の調子が悪くなった...", "Getting wet makes your shikigami powers weaker..."));
				do_dec_stat(A_INT);
			}


			else if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
			{
				hina_gain_yaku(-(dam * 2));
				if(!p_ptr->magic_num1[0])
					msg_print(_("水で厄が全て流れてしまった！", "The water washes away all of your misfortune!"));
				else
					msg_print(_("水で厄が少し流れ出てしまった！", "The water washes away some of your misfortune!"));
			}

			if (!CHECK_MULTISHADOW() && !p_ptr->resist_water)
			{
				///mod131229 水攻撃の朦朧とアイテム破壊は水耐性で判定することにした
				//if (!p_ptr->resist_sound)
				{
					set_stun(p_ptr->stun + randint1(30));
				}
				if (!p_ptr->resist_conf)
				{
					set_confused(p_ptr->confused + randint1(5) + 5);
				}

				if (one_in_(5))
				{
					inven_damage(set_cold_destroy, 3);
				}
			}
			///mod140821 エンジニアの機械が故障する処理
			engineer_malfunction(GF_WATER, dam);

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}


		//v1.1.86 敵も使ってくることになったので追加
		case GF_RAINBOW:
		{
			int water_dam, lite_dam;
			water_dam = (dam + 1) / 2;
			lite_dam = (dam + 1) / 2;

			if (fuzzy) msg_print(_("何かで攻撃された！", "You are hit by something!"));

			//v1.1.59サニーミルクが光を取り込む
			sunny_charge_sunlight(lite_dam);

			if (CHECK_FAIRY_TYPE == 37)
			{
				msg_print(_("あなたは光の一部を栄養にした！", "The light nourishes you!"));
				set_food(MIN(p_ptr->food + lite_dam, PY_FOOD_MAX - 1));
				lite_dam = lite_dam * 4 / 5;
			}

			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
			{
				lite_dam = 0;
			}
			else if (p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
			{

				lite_dam = 0;
			}
			else
			{
				lite_dam = mod_lite_dam(lite_dam);

				if (!CHECK_MULTISHADOW() && !p_ptr->resist_conf && p_ptr->resist_lite)
				{
					set_confused(p_ptr->confused + randint1(5) + 5);
				}
			}

			//水ダメージも受けるが、アイテム破壊、エンジニアの機械不調、橙の式剥がれは起こらないことにする
			water_dam = mod_water_dam(water_dam);

			dam = lite_dam + water_dam;
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);


			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print(_("閃光のため非物質的な影の存在でいられなくなった。",
                            "The light forces you out of your incorporeal shadow form."));

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}

			break;
		}


		/* Chaos -- many effects */
		case GF_CHAOS:
		{
#ifdef JP
			if (fuzzy) msg_print("無秩序の波動で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by a wave of anarchy!");
#endif

			if (p_ptr->resist_chaos)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}

			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_conf)
				{
					(void)set_confused(p_ptr->confused + randint0(20) + 10);
				}
				if (!p_ptr->resist_chaos)
				{
					(void)set_image(p_ptr->image + randint1(10));
					if (one_in_(3))
					{
#ifdef JP
						msg_print("あなたの身体はカオスの力で捻じ曲げられた！");
#else
						msg_print("Your body is twisted by chaos!");
#endif

						(void)gain_random_mutation(0);
					}
				}
				if (!p_ptr->resist_neth && !p_ptr->resist_chaos)
				{
					drain_exp(5000 + (p_ptr->exp / 100), 500 + (p_ptr->exp / 1000), 75);
				}

				if (!p_ptr->resist_chaos || one_in_(9))
				{
					inven_damage(set_elec_destroy, 2);
					inven_damage(set_fire_destroy, 2);
				}
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Shards -- mostly cutting */
		case GF_SHARDS:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
			{
				msg_print(_("あなたは全くの無傷だ。", "You are unharmed."));
				dam = 0;
				break;
			}

			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print(_("無数の破片があなたを傷つけた！", "Countless shards wound you!"));
				dam += dam / 4;
			}


			if (p_ptr->resist_shard)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_cut(p_ptr->cut + dam);
			}

			if (!p_ptr->resist_shard || one_in_(13))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Sound -- mostly stunning */
		case GF_SOUND:
		{
#ifdef JP
			if (fuzzy) msg_print("轟音で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something loud!");
#endif
			///mod141106 響子轟音免疫
			if(p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29) break;


			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print(_("衝撃があなたを傷つけた！", "The shockwave wounds you!"));
				dam += dam / 4;
			}


			if (p_ptr->resist_sound)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_stun(p_ptr->stun + k);
			}

			if (!p_ptr->resist_sound || one_in_(13))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Pure confusion */
		case GF_CONFUSION:
		{
#ifdef JP
			if (fuzzy) msg_print("何か混乱するもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something puzzling!");
#endif

			if (p_ptr->resist_conf)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_confused(p_ptr->confused + randint1(20) + 10);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		//v2.0.11 移動禁止属性をプレイヤーにも適用
		case GF_NO_MOVE:
		{

			if (!CHECK_MULTISHADOW())
			{
				set_no_move(dam);
			}

			dam = 0;
			break;
		}




		/* Disenchantment -- see above */
		case GF_DISENCHANT:
		{
#ifdef JP
			if (fuzzy) msg_print("何かさえないもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something static!");
#endif

			dam = mod_disen_dam(dam);



			if (!p_ptr->resist_disen && !CHECK_MULTISHADOW())
			{
				(void)apply_disenchant(0,0);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Nexus -- see above */
		case GF_NEXUS:
		{
#ifdef JP
			if (fuzzy) msg_print("何か奇妙かつ強力な攻撃を受けた！");
			//if (fuzzy) msg_print("何か奇妙なもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something strange and powerful!");
#endif
			if (p_ptr->resist_time)
			//if (p_ptr->resist_nexus)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				apply_nexus(m_ptr);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Force -- mostly stun */
		case GF_FORCE:
		{
#ifdef JP
			if (fuzzy) msg_print("気のパワーで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by the power of Ki!");
#endif

			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			{
				dam -= p_ptr->lev * 3;
				if(dam < 1)
				{
					msg_print(_("あなたは放たれた気を弾いた！", "You repel the released Ki!"));
					dam = 0;
					break;
				}

			}
			else if (prace_is_(RACE_LUNARIAN))
			{
				msg_print(_("あなたの肉体は穢れた力に傷つけられた！",
                            "Your body is wounded by the impure forces!"));
				dam = dam * 4 / 3;
			}


			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}


		/* Rocket -- stun, cut */
		case GF_ROCKET:
		{
#ifdef JP
			if (fuzzy) msg_print("爆発があった！");
#else
			if (fuzzy) msg_print("There is an explosion!");
#endif
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
			{
				msg_print(_("あなたは爆炎の中から無傷で現れた。", "You walk unharmed out of the explosion."));
				dam = 0;
				break;
			}

			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print(_("強烈な爆風があなたを傷つけた！", "The powerful explosion wounds you!"));
				dam += dam / 4;
			}

			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}

			if (p_ptr->resist_shard)
			{
				dam /= 2;
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_cut(p_ptr->cut + (dam / 2));
			}

			if (!p_ptr->resist_shard || one_in_(12))
			{
				inven_damage(set_cold_destroy, 3);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Inertia -- slowness */
		case GF_INACT:
		{
#ifdef JP
			if (fuzzy) msg_print("何か遅いもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif
			if((IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_INER) || p_ptr->pclass == CLASS_SAKI)
			{
				msg_print(_("あなたには耐性がある。", "You resist."));
				dam /= 2;
			}

			else if (!CHECK_MULTISHADOW()) (void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Lite -- blinding */
		case GF_LITE:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif
			///mod140122 閃光ダメージ計算方法変更
			/*

			if (p_ptr->resist_lite)
			{
				dam *= 4; dam /= (randint1(4) + 7);
			}
			else if (!blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}

			if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
			{
#ifdef JP
				if (!CHECK_MULTISHADOW()) msg_print("光で肉体が焦がされた！");
#else
				if (!CHECK_MULTISHADOW()) msg_print("The light scorches your flesh!");
#endif

				dam *= 2;
			}

			else if (prace_is_(RACE_S_FAIRY))
			{
				dam = dam * 4 / 3;
			}
			if (p_ptr->wraith_form) dam *= 2;
			*/

			//v1.1.59サニーミルクが光を取り込む
			sunny_charge_sunlight(dam);

			if(CHECK_FAIRY_TYPE == 37)
			{
				msg_print(_("あなたは光の一部を栄養にした！", "The light nourishes you!"));
				set_food(MIN(p_ptr->food + dam,PY_FOOD_MAX-1));
				dam = dam * 4 / 5;
				if(!dam) break;
			}

			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
			{
				dam = 0;
				break;
			}
			else if(p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
			{
				msg_print(_("反射鏡が光線を弾いた！", "The reflector deflects the ray!"));

				dam = 0;
				break;
			}
			else dam = mod_lite_dam(dam);

			if(!dam) break;

			if (!p_ptr->resist_lite && !blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print(_("閃光のため非物質的な影の存在でいられなくなった。",
                            "The light forces you out of your incorporeal shadow form."));

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}

			break;
		}

		/* Dark -- blinding */
		case GF_DARK:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				}
				else
				{
					yaku = randint1(dam);
					msg_print(_("あなたは攻撃の一部を厄として吸収した。", "You partially absorb the attack as misfortune."));
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}

			if (prace_is_(RACE_LUNARIAN))
			{
				msg_print(_("あなたの肉体は穢れた力に傷つけられた！", "Your body is wounded by the impure forces!"));
				dam = dam * 4 / 3;
			}

			if (p_ptr->resist_dark)
			{
				if (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 39)
				{
#ifdef JP
					msg_print("あなたは闇の力を吸収した。");
#else
                    msg_print("You absorb the power of darkness.");
#endif
					hp_player(dam / 4);
				}

				dam *= 4; dam /= (randint1(4) + 7);
				///sys res 吸血鬼暗黒無効化処理　残す？消す？
				if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE) || (p_ptr->mimic_form == MIMIC_NUE) || p_ptr->wraith_form || (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19) || p_ptr->tim_unite_darkness) dam = 0;
			}
			else if (!blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}
		case GF_DISTORTION:
		{
#ifdef JP
			if (fuzzy) msg_print("何か奇妙なもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something weird!");
#endif

			if (p_ptr->resist_time)
			{
				dam *= 4; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				int tmp;
				msg_print(_("あなたは亜空間に呑み込まれた！", "You get swallowed by a subspace!"));
				teleport_player(50+randint1(50), TELEPORT_PASSIVE);
				for(tmp=0;tmp<6;tmp++)if(one_in_(2)) do_dec_stat(tmp);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Time -- bolt fewer effects XXX */
		case GF_TIME:
		{
#ifdef JP
			if (fuzzy) msg_print("過去からの衝撃に攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by a blast from the past!");
#endif

			if (p_ptr->resist_time)
			{
				dam *= 4;
				dam /= (randint1(4) + 7);
#ifdef JP
				msg_print("時間が通り過ぎていく気がする。");
#else
				msg_print("You feel as if time is passing you by.");
#endif
			}
			else if (!CHECK_MULTISHADOW())
			{
				switch (randint1(10))
				{
					case 1: case 2: case 3: case 4: case 5:
					{
						if (p_ptr->prace == RACE_ANDROID) break;
#ifdef JP
						msg_print("人生が逆戻りした気がする。");
#else
						msg_print("You feel life has clocked back.");
#endif

						lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
						break;
					}

					case 6: case 7: case 8: case 9:
					{
						switch (randint1(6))
						{
#ifdef JP
							case 1: k = A_STR; act = "強く"; break;
							case 2: k = A_INT; act = "聡明で"; break;
							case 3: k = A_WIS; act = "賢明で"; break;
							case 4: k = A_DEX; act = "器用で"; break;
							case 5: k = A_CON; act = "健康で"; break;
							case 6: k = A_CHR; act = "美しく"; break;
#else
							case 1: k = A_STR; act = "strong"; break;
							case 2: k = A_INT; act = "bright"; break;
							case 3: k = A_WIS; act = "wise"; break;
							case 4: k = A_DEX; act = "agile"; break;
							case 5: k = A_CON; act = "hale"; break;
							case 6: k = A_CHR; act = "beautiful"; break;
#endif
						}

#ifdef JP
						msg_format("あなたは以前ほど%sなくなってしまった...。", act);
#else
						msg_format("You're not as %s as you used to be...", act);
#endif

						p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
						if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
						p_ptr->update |= (PU_BONUS);
						break;
					}

					case 10:
					{
#ifdef JP
						msg_print("あなたは以前ほど力強くなくなってしまった...。");
#else
						msg_print("You're not as powerful as you used to be...");
#endif

						for (k = 0; k < 6; k++)
						{
							p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 7) / 8;
							if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
						}
						p_ptr->update |= (PU_BONUS);
						break;
					}
				}
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Gravity -- stun plus slowness plus teleport */
		case GF_GRAVITY:
		{

			if(CHECK_FAIRY_TYPE == 40)
			{
				msg_print(_("あなたには完全な耐性がある！", "You are fully immune!"));
				dam = 0;
				break;
			}

			if (fuzzy) msg_print(_("何か重いもので攻撃された！", "You are hit by something heavy!"));
			msg_print(_("周辺の重力がゆがんだ。", "Gravity warps around you."));


			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_time) teleport_player(5, TELEPORT_PASSIVE);
				if (!p_ptr->levitation && !p_ptr->resist_time)
					(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
				if (!(p_ptr->resist_sound || p_ptr->levitation || p_ptr->resist_time))
				{
					int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
					(void)set_stun(p_ptr->stun + k);
				}
			}
			if (p_ptr->levitation)
			{
				dam = (dam * 2) / 3;
			}
			if (p_ptr->resist_time)
			{
				dam = (dam * 2) / 3;
			}
			if (!p_ptr->resist_time &&  (!p_ptr->levitation || one_in_(13)))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}





		/*::: 竜巻属性 */
		case GF_TORNADO:
		{
			int wgt = (p_ptr->total_weight / 20);
#ifdef JP
			if (fuzzy) msg_print("暴風で攻撃された！");
#else
            if (fuzzy) msg_print("You are hit by strong winds!");
#endif

			if( (p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_SANAE || p_ptr->pclass == CLASS_FUTO) && p_ptr->lev > 29)
			{
				msg_print(_("あなたは竜巻を打ち消した！", "You dispel the tornado!"));
				dam =0;
				break;
			}
			if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
			{
				msg_print(_("あなたの巨大な体は飛ばされなかった。", "Your gigantic body wasn't blown away."));
				dam /= 4;
			}
			/*:::重量が重いと抵抗　100KgでLev100の敵に耐えられる*/
			else if( wgt > rlev)
			{
				msg_print(_("荷物が重くて飛ばされなかった。", "You weren't blown you because of the weight of items you're carrying."));
				dam /= 3;
			}
			else if (p_ptr->levitation)
			{
				msg_print(_("あなたは吹き飛ばされたが空中で体勢を立て直した。", "You get blown away, but you regain balance in midair."));
				dam /= 2;
				teleport_player(5+randint1(5), TELEPORT_PASSIVE);
			}

			else if (!CHECK_MULTISHADOW())
			{
				msg_print(_("あなたは吹き飛ばされた！", "You get blown away!"));
				teleport_player(10+randint0(20), TELEPORT_PASSIVE);
				if(one_in_(3))inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/*::: 電車属性 */
		case GF_TRAIN:
		{
			if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
			{
				msg_print(_("あなたの巨大な体は跳ね飛ばされなかった。", "Your gigantic body wasn't thrown aside."));
			}
			else if (!CHECK_MULTISHADOW())
			{
				if (fuzzy) msg_print(_("なにか巨大なものに跳ね飛ばされた！", "You get thrown aside by something huge!"));
				else msg_print(_("あなたは跳ね飛ばされた！", "You get thrown aside!"));
				teleport_player(10+randint0(20), TELEPORT_PASSIVE);
				if(one_in_(3))inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Standard damage */
		case GF_DISINTEGRATE:
		{
#ifdef JP
			if (fuzzy) msg_print("純粋なエネルギーで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by pure energy!");
#endif
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_ROCK)
			{
				msg_print(_("あなたの硬質な身体が分解していく！",
                            "Your hard body is getting broken down!"));
				dam = dam * 3 / 2;
			}

			//v2.0.17 残無虚無操作による無属性耐性
			if (p_ptr->pclass == CLASS_ZANMU && (p_ptr->tim_general[0]))
			{
				dam = (dam + 1) / 2;
				msg_print(_("あなたには耐性がある！", "You resist!"));
			}


			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		case GF_OLD_HEAL:
		{
#ifdef JP
			if (fuzzy) msg_print("何らかの攻撃によって気分がよくなった。");
#else
			if (fuzzy) msg_print("You are hit by something invigorating!");
#endif

			(void)hp_player(dam);
			dam = 0;
			break;
		}

		case GF_OLD_SPEED:
		{
#ifdef JP
			if (fuzzy) msg_print("何かで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			(void)set_fast(p_ptr->fast + randint1(5), FALSE);
			dam = 0;
			break;
		}

		//v1.1.97 なぜか無かったので追加
		case GF_OLD_CONF:
		{
			if (fuzzy) msg_print(_("何かで攻撃された！", "You are hit by something!"));

			if (!p_ptr->resist_conf && !CHECK_MULTISHADOW())
			{
				(void)set_confused(p_ptr->confused + randint1(10) + 10);
			}
			dam = 0;
			break;
		}

		case GF_OLD_SLOW:
		{
#ifdef JP
			if (fuzzy) msg_print("何かが体に絡みついた気がする！");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif

			(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
			break;
		}

		case GF_OLD_SLEEP:
		{
			if (p_ptr->free_act)  break;
#ifdef JP
			/*if (fuzzy) */ if(!p_ptr->paralyzed) msg_print("あなたは眠ってしまった！");
#else
			if(!p_ptr->paralyzed) msg_print("You fall asleep!");
#endif

			if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print(_("頭がスッキリした！でも何か忘れているような。",
                                                                    "Your head feels clear! But you feel you've forgotten something."));

			if (ironman_nightmare)
			{
#ifdef JP
				msg_print("恐ろしい光景が頭に浮かんできた。");
#else
				msg_print("A horrible vision enters your mind.");
#endif

				/* Pick a nightmare */
				get_mon_num_prep(get_nightmare, NULL);

				/* Have some nightmares */
				have_nightmare(get_mon_num(MAX_DEPTH));

				/* Remove the monster restriction */
				get_mon_num_prep(NULL, NULL);
			}

			set_paralyzed(p_ptr->paralyzed + dam);
			dam = 0;
			break;
		}

		/* Pure damage */
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("魔法のオーラで攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by an aura of magic!");
#endif

			//v2.0.17 残無虚無操作による無属性耐性
			if (p_ptr->pclass == CLASS_ZANMU && (p_ptr->tim_general[0]))
			{
				dam = (dam + 1) / 2;
				msg_print(_("あなたには耐性がある！", "You resist!"));
			}


			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Pure damage */
		case GF_PSY_SPEAR:
		case GF_GUNGNIR:
		{
#ifdef JP
			if (fuzzy) msg_print("強力なエネルギーの塊で攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by powerful energy!");
#endif

			get_damage = take_hit(DAMAGE_FORCE, dam, killer, monspell);
			break;
		}

		/* Pure damage */
		case GF_METEOR:
		{
#ifdef JP
			if (fuzzy) msg_print("何かが空からあなたの頭上に落ちてきた！");
#else
			if (fuzzy) msg_print("Something falls from the sky onto your head!");
#endif

			if(CHECK_FAIRY_TYPE == 54)
			{
				msg_print(_("しかし隕石の破片はあなたの頭の上をかすめていった！",
                            "The meteor shards grazed the top of your head!"));
				dam = 0;
				break;
			}
			if(p_ptr->pclass == CLASS_STAR)
			{
				msg_print(_("あなたは幾らかの耐性を示した。", "You resist somewhat."));
				dam = dam * 2 / 3;
			}

			//v2.0.17 残無虚無操作による無属性耐性
			if (p_ptr->pclass == CLASS_ZANMU && (p_ptr->tim_general[0]))
			{
				dam = (dam + 1) / 2;
				msg_print(_("あなたには耐性がある！", "You resist!"));
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			if (!p_ptr->resist_shard || one_in_(13))
			{
				if (!p_ptr->immune_fire) inven_damage(set_fire_destroy, 2);
				inven_damage(set_cold_destroy, 2);
			}

			break;
		}

		/* Ice -- cold plus stun plus cuts */
		case GF_ICE:
		{
#ifdef JP
			if (fuzzy) msg_print("何か鋭く冷たいもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something sharp and cold!");
#endif

			get_damage = cold_dam(dam, killer, monspell);
			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_shard)
				{
					(void)set_cut(p_ptr->cut + damroll(5, 8));
				}
				if (!p_ptr->resist_sound)
				{
					(void)set_stun(p_ptr->stun + randint1(15));
				}

				if ((!(p_ptr->resist_cold || IS_OPPOSE_COLD())) || one_in_(12))
				{
					if (!p_ptr->immune_cold) inven_damage(set_cold_destroy, 3);
				}
			}

			break;
		}

		/* Death Ray */
		case GF_DEATH_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("何か非常に冷たいもので攻撃された！");
#else
			if (fuzzy) msg_print("You are hit by something extremely cold!");
#endif

			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				}
				else
				{
					yaku = randint1(dam);
					msg_print(_("あなたは攻撃の一部を厄として吸収した。", "You partially absorb the attack as misfortune."));
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}
			else if (p_ptr->mimic_form)
			{
				if (!(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING))
					get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			else
			{

				if (RACE_RESIST_DEATH_RAY)
				{
					dam = 0;
					break;
				}
			}

			break;
		}

		/* Drain mana */
		case GF_DRAIN_MANA:
		{
			if(diehardmind())
			{
				msg_print(_("あなたは精神攻撃に無反応だ。", "You don't react to the mental attack."));
				break;
			}
			else 			if (CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
			}
			else if (p_ptr->csp)
			{
				/* Basic message */
#ifdef JP
				if (who > 0) msg_format("%^sに精神エネルギーを吸い取られてしまった！", m_name);
				else msg_print("精神エネルギーを吸い取られてしまった！");
#else
				if (who > 0) msg_format("%^s draws psychic energy from you!", m_name);
				else msg_print("Your psychic energy is drawn!");
#endif

				/* Full drain */
				if (dam >= p_ptr->csp)
				{
					dam = p_ptr->csp;
					p_ptr->csp = 0;
					p_ptr->csp_frac = 0;
				}

				/* Partial drain */
				else
				{
					p_ptr->csp -= dam;
				}

				learn_spell(monspell);

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);

				if (who > 0)
				{
					/* Heal the monster */
					if (m_ptr->hp < m_ptr->maxhp)
					{
						/* Heal */
						m_ptr->hp += (6 * dam);
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == who) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == who) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (m_ptr->ml)
						{
#ifdef JP
							msg_format("%^sは気分が良さそうだ。", m_name);
#else
							msg_format("%^s appears healthier.", m_name);
#endif
						}
					}
				}
			}

			dam = 0;
			break;
		}

		/* Mind blast */
		case GF_MIND_BLAST:
		case GF_REDEYE:
		{
			if(diehardmind())
			{
				msg_print(_("あなたは無反応だ。", "You don't react."));
				break;

			}
			else if ((randint0(100 + rlev / 2) < MAX(5, p_ptr->skill_sav)) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psionic energy.");
#endif

					if (!p_ptr->resist_conf)
					{
						(void)set_confused(p_ptr->confused + randint0(4) + 4);
					}

					if (!p_ptr->resist_chaos && one_in_(3))
					{
						(void)set_image(p_ptr->image + randint0(250) + 150);
					}

					p_ptr->csp -= 50;
					if (p_ptr->csp < 0)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= PR_MANA;
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* Brain smash */
		case GF_BRAIN_SMASH:
		{
			if(diehardmind())
			{
				msg_print(_("あなたは無反応だ。", "You don't react."));
				break;

			}
			else 			if ((randint0(100 + rlev / 2) < MAX(5, p_ptr->skill_sav)) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("霊的エネルギーで精神が攻撃された。");
#else
					msg_print("Your mind is blasted by psionic energy.");
#endif

					p_ptr->csp -= 100;
					if (p_ptr->csp < 0)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= PR_MANA;
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW())
				{
					if (!p_ptr->resist_blind)
					{
						(void)set_blind(p_ptr->blind + 8 + randint0(8));
					}
					if (!p_ptr->resist_conf)
					{
						(void)set_confused(p_ptr->confused + randint0(4) + 4);
					}
					if (!p_ptr->free_act)
					{
						(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
					}
					(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);

					while (randint0(100 + rlev / 2) > (MAX(5, p_ptr->skill_sav)))
						(void)do_dec_stat(A_INT);
					while (randint0(100 + rlev / 2) > (MAX(5, p_ptr->skill_sav)))
						(void)do_dec_stat(A_WIS);

					if (!p_ptr->resist_chaos)
					{
						(void)set_image(p_ptr->image + randint0(250) + 150);
					}
				}
			}
			break;
		}

		/* コズミックホラー 分身無効にしておくか*/
		case GF_COSMIC_HORROR:
		{
			if(diehardmind())
			{
				msg_print(_("あなたは無反応だ。", "You don't react."));
				break;
			}
			//skill_savはLev50で精々100程度　反魔法アミュで130くらい
			else if (immune_insanity() || p_ptr->resist_insanity && p_ptr->skill_sav > (30+randint1(rlev)))
			{
#ifdef JP
				msg_print("しかしあなたの精神は影響を受けなかった！");
#else
				msg_print("You resist the effects!");
#endif
			}
			else
			{
				int blast;
#ifdef JP
				msg_print("あなたの精神は宇宙の深淵へと呑み込まれた。");
#else
				msg_print("Your mind is blasted by psionic energy.");
#endif
				if (p_ptr->resist_insanity) dam/=2;
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

				if(!p_ptr->resist_insanity)
				{
					int div;

					blast = randint0(rlev / (p_ptr->resist_fear?2:1));

					if(IS_VULN_FEAR) blast += 12 + randint1(12);

					if(blast < 10)
					{
						if(!p_ptr->resist_conf) set_confused(100+randint0(100));
					}
					else if(blast < 20)
					{
						if(!p_ptr->resist_chaos) set_image(100+randint0(100));
					}
					else if(blast < 30)
					{
						set_stun(p_ptr->stun + 20 + randint0(31));
					}
					else if(blast < 40)
					{
						int cnt;
						for(cnt=0;cnt<6;cnt++) do_dec_stat(cnt);
					}
					else if(blast < 60)
					{
						msg_print(_("何かが目の前に佇んでいる。", "Something stands before you."));
						summon_specific(0, py, px, dun_level, SUMMON_INSANITY, (PM_NO_PET));
					}
					else if(blast < 70)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
						p_ptr->redraw = PR_MANA;
						redraw_stuff();
						msg_print(_("自分を取り戻すのに全ての精神力を使い果たした・・",
                                    "You've exhausted all of your mental energy to regain your composure..."));
					}
					else if(blast < 80)
					{
						int muta[5] = {75,76,77,79,130};
						msg_print(_("禁断の知識に脳が耐え切れない！",
                                    "Your brain can't withstand the forbidden knowledge!"));
						gain_random_mutation(muta[randint0(5)]);
					}
					else if(blast < 90)
					{
						int cnt;
						msg_print(_("宇宙の深淵から悍ましい存在があなたを追跡してきた！",
                                    "You are pursued by terrifying beings from the depths of the cosmos!"));
						for(cnt = randint1(3);cnt>0;cnt--)summon_specific(0, py, px, dun_level, SUMMON_INSANITY2, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
					}
					else
					{
						msg_print(_("あなたの肉体は活動を停止した。",
                                    "Your body stops functioning."));
						if (p_ptr->free_act)
							set_paralyzed(p_ptr->paralyzed + randint1(3));
						else
							set_paralyzed(p_ptr->paralyzed + randint1(13));

					}
				}

			}
			break;
		}

		/* cause 1 */
		case GF_CAUSE_1:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは攻撃を不運パワーとして吸収した。", "You absorb the attack as misfortune power."));
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 呪いは暗黒耐性か地獄耐性でダメージ半減することにした
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(15, 0);
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 2 */
		case GF_CAUSE_2:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは攻撃を不運パワーとして吸収した。", "You absorb the attack as misfortune power."));
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 呪いは暗黒耐性か地獄耐性でダメージ半減することにした
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(25, MIN(rlev / 2 - 15, 5));
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 3 */
		case GF_CAUSE_3:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは攻撃を不運パワーとして吸収した。", "You absorb the attack as misfortune power."));
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 呪いは暗黒耐性か地獄耐性でダメージ半減することにした
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(33, MIN(rlev / 2 - 15, 15));
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 4 */
		case GF_CAUSE_4:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは攻撃を不運パワーとして吸収した。", "You absorb the attack as misfortune power."));
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !(m_ptr->r_idx == MON_KENSHIROU) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 呪いは暗黒耐性か地獄耐性でダメージ半減することにした
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW()) (void)set_cut(p_ptr->cut + damroll(10, 10));
			}
			break;
		}

		/* Hand of Doom */
		case GF_HAND_DOOM:
		{
			if(p_ptr->pclass == CLASS_CLOWNPIECE)
			{
				msg_format(_("しかし効力を跳ね返した！", "You resist the effects!"));
				break;
			}
			else if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print(_("あなたは攻撃を厄として吸収した。", "You absorb the attack as misfortune."));
				hina_gain_yaku(500);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは攻撃を不運パワーとして吸収した。", "You absorb the attack as misfortune power."));
				p_ptr->magic_num1[1] += 5000;
				dam = 0;
			}
			else if ((randint0(100 + rlev/2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_format("しかし効力を跳ね返した！");
#else
				msg_format("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("あなたは命が薄まっていくように感じた！");
#else
					msg_print("You feel your life fading away!");
#endif
					curse_equipment(40, 20);
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, m_name, monspell);

				if (p_ptr->chp < 1) p_ptr->chp = 1; /* Paranoia */
			}
			break;
		}
		/* フラン特殊攻撃　必中破滅の手+切り傷+経験値減少+全能力低下 */
		case GF_KYUTTOSHITEDOKA_N:
		{
			if(p_ptr->pclass == CLASS_CLOWNPIECE) dam /=10;

			msg_print(_("体から生命力が弾け飛んだ気がする！",
                        "You feel life force being torn away from your body!"));
			get_damage = take_hit(DAMAGE_ATTACK, dam, m_name, monspell);
			(void)set_cut(p_ptr->cut + damroll(1, 256));
			if (p_ptr->chp < 1) p_ptr->chp = 1; /* Paranoia */
			lose_exp(p_ptr->exp / 10);
			do_dec_stat(A_STR);
			do_dec_stat(A_INT);
			do_dec_stat(A_WIS);
			do_dec_stat(A_DEX);
			do_dec_stat(A_CON);
			do_dec_stat(A_CHR);

			break;
		}
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		{

			//v2.1.0 怨霊の応援は破邪系攻撃を受けると消える
			if (!CHECK_MULTISHADOW() && (p_ptr->muta4 & MUT4_GHOST_CHEERS) && (randint0(100 + rlev / 2) >= p_ptr->skill_sav))
			{
				lose_mutation(218);
			}

			if(  is_hurt_holy() <= 0 )
			{
#ifdef JP
				msg_print("しかしあなたには効果がなかった。");
#else
				msg_print("You are unaffected.");
#endif
			}

			else if ( CHECK_MULTISHADOW() || (randint0(100 + rlev / 2) < p_ptr->skill_sav) )
			{
#ifdef JP
				msg_print("しかし効力を跳ね返した！");
#else
                msg_print("You resist the effects!");
#endif
				//learn_spell(monspell);
			}
			else
			{

				dam = mod_holy_dam(dam);

				if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
				{
					hina_gain_yaku(-(dam * 3));
					if(!p_ptr->magic_num1[0])
						msg_print(_("魔除けで厄が落ちてしまった！", "The charm dispelled your misfortune!"));
					else
						msg_print(_("魔除けで厄が少し落ちてしまった！", "The charm dispelled a bit of your misfortune!"));
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW()) (void)set_stun(p_ptr->stun + dam/5);
			}
			break;
		}

		case GF_WINDCUTTER:
		{

			if(randint1(p_ptr->ac + p_ptr->to_a) > 50)
			{
				msg_print(_("風の刃はあなたに当たらなかった。", "The blade of wind didn't hit you."));
			}
			else get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			break;
		}
		case GF_BANKI_BEAM:
		{
#ifdef JP
			if (fuzzy) msg_print("分身の放ったビームが自分に当たったようだ！");
#else
            if (fuzzy) msg_print("Looks like you got hit by a beam shot by your own clone!");
#endif
			get_damage = take_hit(DAMAGE_ATTACK, dam, _("自分のビーム", "your own beam"), monspell);
			break;
		}

		//v1.1.97
		case GF_DEC_ATK:
		{
			if(!CHECK_MULTISHADOW())
				do_dec_stat(A_STR);
			dam = 0;
			break;
		}

		case GF_DEC_DEF:
		{
			if (!CHECK_MULTISHADOW())
				do_dec_stat(A_CON);
			dam = 0;
			break;
		}

		case GF_DEC_MAG:
		{
			if (!CHECK_MULTISHADOW())
				do_dec_stat(A_INT);
			dam = 0;
			break;
		}
		case GF_DEC_ALL:
		{

			if (!CHECK_MULTISHADOW())
			{
				do_dec_stat(A_STR);
				do_dec_stat(A_INT);
				do_dec_stat(A_WIS);
				do_dec_stat(A_DEX);
				do_dec_stat(A_CON);
				do_dec_stat(A_CHR);

			}
			dam = 0;
		}
		break;
		case GF_BERSERK:
		{
			if (!CHECK_MULTISHADOW()) set_shero(p_ptr->shero + 25 + randint1(25), FALSE);


			dam = 0;
		}
		break;

		case GF_PIT_FALL:
		{

			if (p_ptr->levitation)
			{
				msg_print(_("あなたは飛んでダメージを回避した。", "You avoid harm as you are floating."));
				break;
			}
			else
			{
				msg_print(_("穴に落ちた！", "You fall into a pit!"));
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				set_stun(p_ptr->stun + 8 + randint1(8));

			}

		}

		break;



		/* Default */
		default:
		{
			/* No damage */
			dam = 0;

			break;
		}
	}
	//鈴瑚のダンゴインフリューエンス
	if(p_ptr->pclass == CLASS_RINGO && get_damage)
	{
		p_ptr->magic_num1[2] = typ;
	}

	/* Hex - revenge damage stored */
	///race 呪術ハイ
	revenge_store(get_damage);

	if ((p_ptr->tim_eyeeye || hex_spelling(HEX_EYE_FOR_EYE))
		&& (get_damage > 0) && !p_ptr->is_dead && (who > 0))
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
		else if (p_ptr->tim_eyeeye) set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
	}

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_JINKIN)
		&& (who > 0) && m_ptr->r_idx && get_damage > 0 && !p_ptr->is_dead)
	{
		msg_format(_("%sは琴の音に包まれた・・", "%^s is enveloped in the sound of the koto..."), m_name);
		project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
	}


	if (p_ptr->riding && dam > 0)
	{
		rakubadam_p = (dam > 200) ? 200 : dam;
	}


	/* Disturb */
	disturb(1, 1);


	//なぜこんなところに変わり身ルーチンがあるのだろう？
	if ((p_ptr->special_defense & NINJA_KAWARIMI) && dam && who && (who != p_ptr->riding))
	{
		(void)kawarimi(FALSE);
	}


	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Find the distance from (x, y) to a line.
 */
/*:::軸からの距離？ブレスの範囲関係で使うらしい*/
int dist_to_line(int y, int x, int y1, int x1, int y2, int x2)
{
	/* Vector from (x, y) to (x1, y1) */
	int py = y1 - y;
	int px = x1 - x;

	/* Normal vector */
	int ny = x2 - x1;
	int nx = y1 - y2;

   /* Length of N */
	int pd = distance(y1, x1, y, x);
	int nd = distance(y1, x1, y2, x2);

	if (pd > nd) return distance(y, x, y2, x2);

	/* Component of P on N */
	nd = ((nd) ? ((py * ny + px * nx) / nd) : 0);

   /* Absolute value */
   return((nd >= 0) ? nd : 0 - nd);
}



/*
 * XXX XXX XXX
 * Modified version of los() for calculation of disintegration balls.
 * Disintegration effects are stopped by permanent walls.
 */
bool in_disintegration_range(int y1, int x1, int y2, int x2)
{
	/* Delta */
	int dx, dy;

	/* Absolute */
	int ax, ay;

	/* Signs */
	int sx, sy;

	/* Fractions */
	int qx, qy;

	/* Scanners */
	int tx, ty;

	/* Scale factors */
	int f1, f2;

	/* Slope, or 1/Slope, of LOS */
	int m;


	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);


	/* Handle adjacent (or identical) grids */
	if ((ax < 2) && (ay < 2)) return (TRUE);


	/* Paranoia -- require "safe" origin */
	/* if (!in_bounds(y1, x1)) return (FALSE); */


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			if (!cave_stop_disintegration(y1 + sy, x1)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			if (!cave_stop_disintegration(y1, x1 + sx)) return (TRUE);
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if (ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		qy = ay * ay;
		m = qy << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if (qy == f2)
		{
			ty = y1 + sy;
			qy -= f1;
		}
		else
		{
			ty = y1;
		}

		/* Note (below) the case (qy == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
				qy -= f1;
				tx += sx;
			}
			else
			{
				ty += sy;
				qy -= f1;
				tx += sx;
			}
		}
	}

	/* Travel vertically */
	else
	{
		/* Let m = dx / dy * 2 * (dx * dy) = 2 * dx * dx */
		qx = ax * ax;
		m = qx << 1;

		ty = y1 + sy;

		if (qx == f2)
		{
			tx = x1 + sx;
			qx -= f1;
		}
		else
		{
			tx = x1;
		}

		/* Note (below) the case (qx == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
				qx -= f1;
				ty += sy;
			}
			else
			{
				tx += sx;
				qx -= f1;
				ty += sy;
			}
		}
	}

	/* Assume los */
	return (TRUE);
}


/*
 * breath shape
 */
/*:::ブレスの効果が及ぶ全グリッドを計算 詳細未読*/
///mod140328 大型レーザー用フラグ追加
void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ, bool masterspark)
{
	int by = y1;
	int bx = x1;
	int brad = 0;
	int brev = rad * rad / dist;
	int bdis = 0;
	int cdis;
	int path_n = 0;
	int mdis = distance(y1, x1, y2, x2) + rad;

	while (bdis <= mdis)
	{
		int x, y;

		if ((0 < dist) && (path_n < dist))
		{
			int ny = GRID_Y(path_g[path_n]);
			int nx = GRID_X(path_g[path_n]);
			int nd = distance(ny, nx, y1, x1);

			/* Get next base point */
			if (bdis >= nd)
			{
				by = ny;
				bx = nx;
				path_n++;
			}
		}

		/* Travel from center outward */
		for (cdis = 0; cdis <= brad; cdis++)
		{
			/* Scan the maximal blast area of radius "cdis" */
			for (y = by - cdis; y <= by + cdis; y++)
			{
				for (x = bx - cdis; x <= bx + cdis; x++)
				{
					/* Ignore "illegal" locations */
					if (!in_bounds(y, x)) continue;

					/* Enforce a circular "ripple" */
					if (distance(y1, x1, y, x) != bdis) continue;

					/* Enforce an arc */
					if (distance(by, bx, y, x) != cdis) continue;

					switch (typ)
					{
					case GF_LITE:
					case GF_LITE_WEAK:
						/* Lights are stopped by opaque terrains */
						if (!los(by, bx, y, x)) continue;
						break;
					case GF_DISINTEGRATE:
					case GF_BLAZINGSTAR:
						/* Disintegration are stopped only by perma-walls */
						if (!in_disintegration_range(by, bx, y, x)) continue;
						break;
					default:
						/* Ball explosions are stopped by walls */
						if (!projectable(by, bx, y, x)) continue;
						break;
					}

					/* Save this grid */
					gy[*pgrids] = y;
					gx[*pgrids] = x;
					(*pgrids)++;
				}
			}
		}

		/* Encode some more "radius" info */
		gm[bdis + 1] = *pgrids;

		/* Increase the size */
		if(masterspark)
		{
			brad = (path_n+1) / 2 ;
			if(brad < 0 ) brad = 0;
			if(brad > rad) brad = rad;
		}
		else
			brad = rad * (path_n + brev) / (dist + brev);

		/* Find the next ripple */
		bdis++;
	}

	/* Store the effect size */
	*pgm_rad = bdis;
}


/*
 * Generic "beam"/"bolt"/"ball" projection routine.
 *:::投射攻撃用基本ルーチン
 *
 * Input:
 *   who: Index of "source" monster (zero for "player")
 *:::who:呪文などの使用者 ＠なら0 モンスターならm_list[]のインデックス
 *:::    負のときは
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *:::rad: ボールの半径　ブレスのとき負で呼ばれるらしい
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *:::dam:ベースダメージ
 *   typ: Type of damage to apply to monsters (and objects)
 *:::typ:属性
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *:::flg:攻撃の種類、影響を与える範囲などのフラグ
 *:::monspell:？
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *:::ビーム・ボルト・ボールを指定した場所に向けて指定した属性・種類で放ち、ダメージを与える
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *:::ボルトは地面のアイテムに影響を与えずターゲットの間にモンスターがいたらそれに当たる。
 *:::ボールはターゲットの間にモンスターがいても通過しターゲットで爆発、使用者以外のキャラと地面のアイテムに影響
 *:::ビームは使用者以外のターゲットまでのものを全て貫通するが地面のものには（滅多に）影響を与えない。
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *:::PROJECT_HIDEが有効になっているときエフェクトが表示されない？
 *:::PROJECT_JUMPが有効になっているとき効果がターゲットに突然発生？(使用者に関する処理をしない？）
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
 *:::＠は直接倒したモンスターの経験値しか得られない。ユニークは＠の攻撃でないと死なない
 *
 * Only 256 grids can be affected per projection, limiting the effective
 * "radius" of standard ball attacks to nine units (diameter nineteen).
 *:::この関数による効果の最大グリッド数は256グリッド
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets to the initial location (see "line_spell()"), or by calculating
 * "virtual targets" far away from the player.
 *:::LOSトリック？
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits somethings (useful for "stone to mud").
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *:::ボルトとビームは壁に当たると壁の中で爆発する処理をしている？
 *
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.  Some bug reports indicate that this is still
 * happening in 2.7.8 for Windows, though it appears to be impossible.
 *:::ボールは壁の手前で爆発する。でないと壁の向こうに爆発が届く。（古いバージョンのウィンドウズではまだ起こる？）
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 *:::攻撃が届くエリアは事前に計算される。爆発は内から外の順に処理される。そうすると見栄えがいい。
 *:::ボール攻撃でモンスターが死んでアイテムを落とし、外側に落ちたアイテムが同じボールで破壊されることがないように修正した？
 *
 * Walls and doors are included in the blast area, so that they can be
 * "burned" or "melted" in later versions.
 *:::壁やドアは影響範囲に含まれる。
 *
 * This algorithm is intended to maximize simplicity, not necessarily
 * efficiency, since this function is not a bottleneck in the code.
 *:::この関数はシンプルさ優先であまり効率的ではないが、大したボトルネックにはならない。
 *
 * We apply the blast effect from ground zero outwards, in several passes,
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *:::爆発の影響は、内から外へ、さらに地形・床上アイテム・モンスター・＠の順に処理する。
 *:::壁の溶解はその壁の中に何か居てもその前に行われる。
 *:::モンスターが死んで落としたアイテムはその攻撃から保護される。
 *:::＠が死ぬかテレポアウェイされる前に＠は全ての効果を見られる。（別に普通だと思うが英語読み間違えてる？）
 *:::これらの処理をどう解釈するかはさておき動作には問題ない（？）
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
 * pleasing visual effect at low cost.
 *:::爆心地から外へ処理を記述するほうが記述やビジュアル面で見栄えがいい・・？
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *:::爆発のダメージは距離で急速に減衰する。1/距離になる。
 *
 * Notice the "napalm" effect of "beam" weapons.  First they "project" to
 * the target, and then the damage "flows" along this beam of destruction.
 * The damage at every grid is the same as at the "center" of a "ball"
 * explosion, since the "beam" grids are treated as if they ARE at the
 * center of a "ball" explosion.
 *:::ビームはまずターゲットが処理され、その後ターゲットまでの各グリッドが処理される・・？
 *:::ビームの届く全ての範囲はボールの中心と同じく減衰しないダメージが適用される
 *
 * Currently, specifying "beam" plus "ball" means that locations which are
 * covered by the initial "beam", and also covered by the final "ball", except
 * for the final grid (the epicenter of the ball), will be "hit twice", once
 * by the initial beam, and once by the exploding ball.  For the grid right
 * next to the epicenter, this results in 150% damage being done.  The center
 * does not have this problem, for the same reason the final grid in a "beam"
 * plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
 * grids which are covered by the "ball" will NOT work, as then they will
 * receive LESS damage than they should.  Do not combine "beam" with "ball".
 *:::よく分からん。ビーム・ボール・ボルトのフラグを複数指定すると何度もダメージを与えることがあるのでやめろってことか？
 *
 * The array "gy[],gx[]" with current size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *:::gx[]gy[]は影響を受けるグリッドが格納される
 *
 * Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
 * zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
 * first blast grid (see above) with radius "N" from the blast center.  Note
 * that only the first gm[1] grids in the blast area thus take full damage.
 * Also, note that gm[rad+1] is always equal to "grids", which is the total
 * number of blast grids.
 *:::gm[]は威力ごとのグリッドを示す
 *:::gm[0]は常に0。
 *:::gm[1]は爆発中心もしくはビームで全ダメージが入るグリッドの数？
 *:::gm[N(2以上)]以降は、gy[]gx[]に爆発中心からの距離順にグリッド列が入ってる中の距離Nの最初のインデックスが入っている？

 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *:::処理終了時、x2,y2にはボールの中心とビーム・ボルトの（最後の）ターゲットが入っている？
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *:::半径1の球の効果範囲が5グリッドなのか9グリッドなのかは距離算出ルーチンによる
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *:::何にも当たらず射程範囲を超えたら爆発しない
 *
 * Note that for consistency, we "pretend" that the bolt actually takes "time"
 * to move from point A to point B, even if the player cannot see part of the
 * projection path.  Note that in general, the player will *always* see part
 * of the path, since it either starts at the player or ends on the player.
 *:::ボルトは＠に見えてなくても到達まで時間が掛かるように見える処理にした・・？
 *
 * Hack -- we assume that every "projection" is "self-illuminating".
 *:::全ての効果は暗くても見える
 *
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *:::モンスター一体だけに当たった場合、PROJCET_JUMPが有効でない限りそのモンスターの情報を表示する
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *:::全てのボール・ボルト・ビームは到達地点で爆発する・・？射程範囲との兼ね合いは？
 *
 * Note that we must call "handle_stuff()" after affecting terrain features
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_monsters()" need to be called.
 *:::処理終了時にはhandle_stuff()を呼ぶこと

 *:::射程距離をMAX以外に指定したい場合はこの関数を呼ぶ前にグローバル変数のproject_lengthに入れておくらしい

 */

bool project(int who, int rad, int y, int x, int dam, int typ, int flg, int monspell)
{
	int i, t, dist;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	int msec = delay_factor * delay_factor * delay_factor;

	/* Assume the player sees nothing */
	bool notice = FALSE;

	/* Assume the player has seen nothing */
	bool visual = FALSE;

	/* Assume the player has seen no blast grids */
	bool drawn = FALSE;

	/* Assume to be a normal ball spell */
	/*:::ブレス攻撃(rad<0)のときTRUE*/
	bool breath = FALSE;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	bool old_hide = FALSE;

	//v1.1.44 削除
	//布都の六壬神火と貴竜の矢が皿に当たったら再発動するための特殊フラグ　
	//bool futo_broken_plate = FALSE;

	/* Number of grids in the "path" */
	int path_n = 0;

	/* Actual grids in the "path" */
	u16b path_g[1024];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[2048], gy[2048];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	/* Attacker's name (prepared before polymorph)*/
	char who_name[80];

	/* Can the player see the source of this effect? */
	bool see_s_msg = TRUE;

	int range;

	// -Hack- 魔弾系専用変数
	//＠の攻撃(who=0)は発射点が＠の位置になるよう設定される。
	//しかし魔弾系の技の再発動処理は前回の着弾点から発射されないといけない。
	//なので魔弾系の技を再発動するときはこの変数に発射点を格納してからwho=-1でproject()を呼び、
	//project()再帰の中で改めて発射点にこの座標を格納してwhoを0に戻す。
	//↑v1.1.44ではPROJECT_OPTでしか使われてないはず。いずれ消したい
	static int mdn_tmp_x, mdn_tmp_y;

	/* Initialize by null string */
	who_name[0] = '\0';

	rakubadam_p = 0;
	rakubadam_m = 0;

	/* Default target of monsterspell is player */
	monster_target_y=py;
	monster_target_x=px;

	//v1.1.30 バグ拾い出しのための警告メッセージ
	//v1.1.31 無害な薬の投擲などでtyp=0で呼ばれることもあるようなので通常表示されないようにした
	if(cheat_xtra && !typ)
	{
		msg_print(_("WARNING:project()にtyp値が設定されていない", "WARNING:typ value is zero in project()"));
	}

	///mod141214
	/*:::射撃連打系の魔法や技で、敵の爆発や血の呪いで＠が倒れた時などに不具合になるかもしれないので中断処理入れとく*/
	if(who < 1 && (p_ptr->is_dead || p_ptr->paralyzed || p_ptr->chp < 0)) return FALSE;

	///清蘭特殊射撃 乱射系、ボール、大型レーザー、魔弾の射手など特殊銃でないとき一度だけ再発動するフラグを付与する
	if(special_shooting_mode == SSM_SEIRAN3 && !rad && !(flg & (PROJECT_FAST | PROJECT_DISI | PROJECT_MASTERSPARK)))
	{
		flg |= PROJECT_OPT;

	}


	//紫の境界操作 GF値が変わる　空欄や特殊属性があるので個別処理
	if(p_ptr->pclass == CLASS_YUKARI && !who && p_ptr->tim_general[1])
	{
		msg_print(_("攻撃の属性が変質した！", "The attack shifts to a different element!"));

		if(typ == GF_FIRE)		typ = GF_PSY_SPEAR;
		//else if(typ == GF_PLASMA) typ = GF_WATER; v1.1.85 typ:13を吹雪生成で埋めた
		//else if(typ == GF_DARK_WEAK) typ = GF_SHARDS; v1.1.85 typ:19を雷雲生成で埋めた
		//else if(typ == GF_INACT) typ = GF_MANA; v1.1.85 typ:25を酸の沼生成で埋めた
		//else if(typ == GF_ICE) typ = GF_CHAOS; v1.1.85 typ:29を毒の沼生成で埋めた
		//else if(typ == GF_GRAVITY) typ = GF_KILL_WALL; typ:36-39を埋めた
		//else if(typ == GF_BERSERK) typ = GF_MAKE_DOOR; 45まで埋まった
		//else if(typ == GF_DIG_OIL) typ = GF_OLD_CLONE; //v1.1.92 MAKE_TREEとOLD_CLONEの間にDIG_OILを追加した v1.1.94 もうひとつ埋めた
		//else if(typ == GF_OLD_DRAIN) typ = GF_AWAY_UNDEAD; //v1.1.96 落とし穴属性とトラップ発動属性で埋めた
		else if(typ == GF_HAND_DOOM) typ = GF_CONTROL_LIVING;//モンスターボールと死者復活は飛ばす
		else if(typ == GF_MAKE_FLOWER) typ = GF_MOSES;//GF_KOKOROはこころ専用の特殊属性なので飛ばす
		else if(typ == GF_STEAM) typ = GF_PURIFY;//GF_SEIRAN_BEAMを飛ばす
		else if (typ == GF_PURIFY) typ = GF_LUNATIC_TORCH;//v1.1.63 特殊射撃用属性を飛ばす
		else if (typ == GF_HOLY_WATER) typ = GF_ELEC;//v1.1.63　末尾は最初に戻る　定期的に修正しよう (MAX_GFを使って自動的に設定すると特殊属性を追加したときにうっかり飛ばし忘れるかもしれんので手動で編集する)
		else typ++;

		if (typ >= MAX_GF) typ = GF_ELEC;//念のため
	}
	//v2.0.1 変異「怨霊の応援」があるとき特技の属性が変質するフラグ
	else if (hack_flag_darkness_power)
	{
		msg_print(_("攻撃の属性が変質した！", "The attack shifts to a different element!"));
		typ = GF_DARK;
		dam = dam * 6 / 5;
		if (dam < 5) dam = 5;
		if (dam > 1600) dam = 1600;
	}

	//v1.1.86 アビリティカード「はじける赤蛙」効果
	//＠の撃ったボルトがロケットになり威力変化
	if (!rad && !who && dam && (flg & PROJECT_STOP))
	{
		int card_num = count_ability_card(ABL_CARD_AKAGAERU);

		if (card_num)
		{
			int mult = 75;
			if (card_num > 1) mult += (card_num - 1) * 7;
			if (card_num >= 9) mult = 150;

			rad = 1;
			if (card_num >= 9) rad++;

			gm_rad = rad;

			dam = dam * mult / 100;
			if (!dam) dam = 1;

		}

	}


	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		///mod151229 この一行を追加しておかないとPROJECT_JUMPで倒された時ダンプに出ないらしい
		if(who>0) monster_desc(who_name, &m_list[who], MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);

		jump = TRUE;
	}

	else if (flg & (/*PROJECT_MADAN | */ PROJECT_OPT))
	{
		if(!who) //最初に撃ったとき
		{
			x1 = px;
			y1 = py;
		}
		else //反射後 who=-1でproject()が呼ばれる
		{
			special_shooting_mode = 0L;//清蘭特殊射撃フラグoff
			who=0;
			flg &= ~(PROJECT_OPT);
			x1 = mdn_tmp_x;
			y1 = mdn_tmp_y;
		}
	}
	/* v1.1.44 ルーチン見直しにつき削除
	else if (flg & (PROJECT_FUTO) )//布都特殊処理
	{
		if(!who) //最初に撃ったとき
		{
			x1 = px;
			y1 = py;
		}
		else //project()内から再帰処理するときはwho=-1でproject()を呼ぶ
		{
			who=0;
			x1 = mdn_tmp_x;
			y1 = mdn_tmp_y;
		}
	}
	*/
	//v1.1.24 スタート位置を個別設定する
	else if(hack_project_start_x && hack_project_start_y)
	{
		x1 = hack_project_start_x;
		y1 = hack_project_start_y;
		hack_project_start_x = 0;
		hack_project_start_y = 0;

	}

	/* Start at player */
	else if (who <= 0)
	{
		x1 = px;
		y1 = py;
	}

	/* Start at monster */
	else if (who > 0)
	{
		x1 = m_list[who].fx;
		y1 = m_list[who].fy;
		monster_desc(who_name, &m_list[who], MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
	}

	/* Oops */
	else
	{
		x1 = x;
		y1 = y;
	}
	/*:::反射用？*/
	y_saver = y1;
	x_saver = x1;


	/* Default "destination" */
	y2 = y;
	x2 = x;

	///mod140907 大型レーザー系を使用者と同じ場所に撃つとフリーズするので回避　ただのボールになるはず
	if((flg & PROJECT_MASTERSPARK) && x1 == x2 && y1 == y2) flg &= ~(PROJECT_MASTERSPARK);

	/* Hack -- verify stuff */
	if (flg & (PROJECT_THRU))
	{
		if ((x1 == x2) && (y1 == y2))
		{
			flg &= ~(PROJECT_THRU);
		}
	}

	/* Handle a breath attack */
	if (rad < 0)
	{
		rad = 0 - rad;
		if(rad > 3) rad = 3;

		breath = TRUE;
		if (flg & PROJECT_HIDE) old_hide = TRUE;
		flg |= PROJECT_HIDE;
	}


	/* Hack -- Assume there will be no blast (max radius 32) */
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;


	/* Initial grid */
	y = y1;
	x = x1;
	dist = 0;

	/* Collect beam grids */
	if (flg & (PROJECT_BEAM))
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}
	/*:::閃光・分解の処理*/
	switch (typ)
	{
	case GF_LITE:
	case GF_LITE_WEAK:
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_LOS);
		break;
	case GF_DISINTEGRATE:
	case GF_BLAZINGSTAR:
		flg |= (PROJECT_GRID);
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;
	case GF_SEIRAN_BEAM:
		if ((flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;


	}

	range = project_length ? project_length : MAX_RANGE;

	if(flg & PROJECT_LONG_RANGE) range = 255;



	/* Calculate the projection path */
	/*:::始点からターゲット（もしくは障害物）までの経路を算出。*/
	/*:::path_g[512]の各要素にx,y両方の座標が入る。上位8bitがy、下位8bitがx*/
	/*:::projcet_lengthはグローバルでこの関数開始から使われてない。射程をMAXより縮めたいとき事前に指定しておくらしい。*/
	path_n = project_path(path_g, (range), y1, x1, y2, x2, flg);
	/*:::壁に隣接して壁に向けて大型レーザー撃つとフリーズするので対策*/
	if((flg & PROJECT_MASTERSPARK) && path_n < 2) flg &= ~(PROJECT_MASTERSPARK);

	/* Hack -- Handle stuff */
	handle_stuff();

	///sysdel class 鏡使いのシーカーレイとスーパーレイ処理
	/* Giga-Hack SEEKER & SUPER_RAY */
	/*:::シーカーレイとスーパーレイの反射に関する処理らしい。よく分からんので飛ばす*/
	if( typ == GF_SEEKER )
	{
		int j;
		int last_i=0;

		/* Mega-Hack */
		/*:::staticのグローバル変数*/
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;


			/* Only do visuals if requested */
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(0,0,y,x,dam,GF_SEEKER))notice=TRUE;
			if( is_mirror_grid(&cave[y][x]))
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				monster_target_y=(s16b)y;
				monster_target_x=(s16b)x;

				remove_mirror(y,x);
				next_mirror( &oy,&ox,y,x );

				path_n = i+project_path(&(path_g[i+1]), (project_length ? project_length : MAX_RANGE), y, x, oy, ox, flg);
				for( j = last_i; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					if(project_m(0,0,y,x,dam,GF_SEEKER,flg,TRUE))notice=TRUE;
					if(!who && (project_m_n==1) && !jump ){
					  if(cave[project_m_y][project_m_x].m_idx >0 ){
					    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

					    if (m_ptr->ml)
					    {
					      /* Hack -- auto-recall */
					      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					      /* Hack - auto-track */
					      health_track(cave[project_m_y][project_m_x].m_idx);
					    }
					  }
					}
					(void)project_f(0,0,y,x,dam,GF_SEEKER);
				}
				last_i = i;
			}
		}
		for( i = last_i ; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			if(project_m(0,0,y,x,dam,GF_SEEKER,flg,TRUE))
			  notice=TRUE;
			if(!who && (project_m_n==1) && !jump ){
			  if(cave[project_m_y][project_m_x].m_idx >0 ){
			    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

			      /* Hack - auto-track */
			      health_track(cave[project_m_y][project_m_x].m_idx);
			    }
			  }
			}
			(void)project_f(0,0,y,x,dam,GF_SEEKER);
		}
		return notice;
	}
	else if(typ == GF_SUPER_RAY){
		int j;
		int second_step = 0;

		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;


			/* Only do visuals if requested */
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(0,0,y,x,dam,GF_SUPER_RAY) )notice=TRUE;
			if (!cave_have_flag_bold(y, x, FF_PROJECT))
			{
				if( second_step )continue;
				break;
			}
			if( is_mirror_grid(&cave[y][x]) && !second_step )
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				monster_target_y=(s16b)y;
				monster_target_x=(s16b)x;

				remove_mirror(y,x);
				for( j = 0; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					(void)project_f(0,0,y,x,dam,GF_SUPER_RAY);
				}
				path_n = i;
				second_step =i+1;
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x+1, flg);
			}
		}
		for( i = 0; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			(void)project_m(0,0,y,x,dam,GF_SUPER_RAY,flg,TRUE);
			if(!who && (project_m_n==1) && !jump ){
			  if(cave[project_m_y][project_m_x].m_idx >0 ){
			    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

			      /* Hack - auto-track */
			      health_track(cave[project_m_y][project_m_x].m_idx);
			    }
			  }
			}
			(void)project_f(0,0,y,x,dam,GF_SUPER_RAY);
		}
		return notice;
	}
	/*:::スーパーレイとシーカーレイの処理終了*/

	//布都が皿の上で六壬神火とか使ったときのための例外処理
	if(flg & (PROJECT_FUTO) && !path_n && is_plate_grid(&cave[y1][x1]))
	{
		//futo_break_plate(y1,x1);
		flag_futo_broken_plate = TRUE;
	}

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int oy = y;
		int ox = x;

		/*:::path_g[]から座標を取り出す*/
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if (flg & PROJECT_DISI)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx) && (rad > 0)) break;
		}
		//壁から跳ね返る射撃が壁を通り抜けないようにする
		else if(flg & PROJECT_OPT)
		{
			if (!cave_los_bold(ny, nx) ) break;
		}

		else if (flg & PROJECT_LOS)
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_los_bold(ny, nx) && (rad > 0)) break;
		}
		else
		{
			//v1.1.21 「インターセプター」技能持ちの兵士が敵の攻撃を撃ち落とす判定
			if(who > 0 && rad > 0 && !breath && !(flg & (PROJECT_JUMP | PROJECT_DISI | PROJECT_MASTERSPARK))
				&& HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_INTERCEPTOR))
			{
				if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_RARM].sval != SV_FIRE_GUN_ROCKET
					|| inventory[INVEN_LARM].tval == TV_GUN && inventory[INVEN_LARM].sval != SV_FIRE_GUN_ROCKET)
				{
					if(projectable(ny,nx,py,px) && is_hostile(&m_list[who]) && weird_luck())
					{
						msg_print(_("あなたは攻撃を撃ち落とした！", "You shoot down the attack!"));
						break;
					}
				}
			}


			/* Hack -- Balls explode before reaching walls */
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && (rad > 0)) break;
			//皿投げは半径0だが壁の前で爆発
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && typ == GF_THROW_PLATE) break;
		}

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		/*:::ボルトの軌跡描画処理　ビームでもここに入るがあとから書き換えてる*/
		if (!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				/*:::ボルトを表示するときの文字と色を決める*/
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/*:::Hack 怨霊憑依の軌跡は'G'で表現*/
				if(typ == GF_POSSESSION) c = 'G';

				/* Visual effects */
				/*:::指定位置に一文字描画*/
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);
				/*if (fresh_before)*/ Term_fresh();
				/*:::基本ウェイト量分のディレイ処理と思われる*/
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				/*if (fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				/*:::ビームならここに入り、描画を'*'に書き換えている*/
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		//布都特技　皿の場所あるいは所定のターゲットで止まるビーム
		if(flg & (PROJECT_FUTO))
		{
			if(is_plate_grid(&cave[ny][nx]))
			{
				//futo_break_plate(ny,nx);
				flag_futo_broken_plate = TRUE;
				break;
			}

			if(ny == y2 && nx == x2) break;
		}

	}
	/*:::ここで壁に当たった分グリッド数を減らしている*/
	path_n = i;
	/* Save the "blast epicenter" */
	by = y;
	bx = x;

	//v1.1.44 着弾点をグローバル変数に記録する
	hack_project_end_y = y;
	hack_project_end_x = x;

	if (breath && !path_n)
	{
		breath = FALSE;
		gm_rad = rad;
		if (!old_hide)
		{
			flg &= ~(PROJECT_HIDE);
		}
	}


	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = path_n;
	dist_hack = dist;

	project_length = 0;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE || flg & PROJECT_LONG_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		//if ((flg & (PROJECT_BEAM)) && !(flg & (PROJECT_FUTO)) && (grids > 0)) grids--;
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*:::
		 *
		 * Create a columnar master spark
		 *
		 *    **************
		 *   ***************
		 * p********@*******
		 *   ***************
		 *    **************
		 */
		if(flg & (PROJECT_MASTERSPARK)) breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,TRUE);
		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		else if (breath)
		{
			flg &= ~(PROJECT_HIDE);
			/*:::ブレスの効果が及ぶ全グリッドを計算する*/
			breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,FALSE);
		}
		/*:::ボールの爆発の影響が及ぶ全グリッドを計算する*/
		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				/*:::正方形にスキャンしdistから外れたらcontinue*/
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!in_bounds2(y, x)) continue;

						///mod150308 菊一文字コンプレッサー専用　上下にしか爆発しない
						if(flg & (PROJECT_COMPRESS) && x !=bx) continue;

						///mod150720 布都の貴竜の矢の特殊処理。皿に当たった時しか爆発せず、爆風は斜め四方向にのみ飛ぶ。
						//六壬神火はradが0なのでここに来ない。
						if(flg & (PROJECT_FUTO))
						{
							if(flag_futo_broken_plate)
							{
								if(((x-bx != y-by) && (x-bx != by-y) )) continue;
							}
							else
							{
								if(x != bx || y != by) continue;
							}
						}

						/* Enforce a "circular" explosion */
						if (distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case GF_LITE:
						case GF_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if (!los(by, bx, y, x)) continue;
							break;
						case GF_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if (!in_disintegration_range(by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if (!projectable(by, bx, y, x)) continue;
							break;
						}

						/* Save this grid */
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}
	/*:::影響範囲の算出終了*/

	/* Speed -- ignore "non-explosions" */
	/*:::ボールやボルトが射程範囲外まで飛んだとき？*/
	if (!grids) return (FALSE);

	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					/*:::ここも描画文字は*になる*/
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					//v1.1.63 虹属性の色をy座標によって変えてみる
					if (typ == GF_RAINBOW)
					{
						switch (y % 7)
						{
						case 0:a = TERM_VIOLET; break;
						case 1:a = TERM_BLUE; break;
						case 2:a = TERM_L_BLUE; break;
						case 3:a = TERM_L_GREEN; break;
						case 4:a = TERM_YELLOW; break;
						case 5:a = TERM_ORANGE; break;
						case 6:a = TERM_RED; break;

						}

					}

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush each "radius" seperately */
			/*if (fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/*:::描画計算した中から＠に見えてる部分だけ描画している？*/
		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if (fresh_before)*/ Term_fresh();
		}
	}


	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/*:::モンスターにダメージを与える効果の場合、＠から見えるならメッセージを出す？*/
	/*:::どうもwhoが負で呼ばれることもあるらしい。どんな時だ？←ガラスが割れた時など*/
	if (flg & PROJECT_KILL)
	{
		see_s_msg = (who > 0) ? is_seen(&m_list[who]) :
			(!who ? TRUE : (player_can_see_bold(y1, x1) && projectable(py, px, y1, x1)));
	}


	/* Check features */
	/*:::床上に効果を及ぼす場合*/
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			/*:::gm[]に距離ごとのgx,gy[]開始位置が入ってることを利用*/
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the grid */
				if (project_f(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/* Check objects */
	/*:::床上のアイテムに効果を及ぼす場合*/
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if (project_o(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}

	}


	/* Check monsters */
	/*:::モンスターに効果を及ぼす場合*/
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;
		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			//清蘭特殊射撃用フラグ処理　ボルトが敵に当たると反射フラグが消える
			if(cave[y][x].m_idx && !(flg & PROJECT_BEAM) && (flg & PROJECT_OPT))
				flg &= ~(PROJECT_OPT);


			/* A single bolt may be reflected */
			///sys 反射処理　ボルト無効や弾幕回避もここで処理しよう
			if (grids <= 1)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				monster_race *ref_ptr = &r_info[m_ptr->r_idx];
				/*:::反射可能でx,yにモンスターがいてそのモンスターが反射持ちで＠の騎乗モンスターでなく＠への攻撃でもないとき9/10で反射*/
				if ((flg & PROJECT_REFLECTABLE) && cave[y][x].m_idx && (ref_ptr->flags2 & RF2_REFLECTING) &&
				    ((cave[y][x].m_idx != p_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
				    (!who || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					/*:::反射先を決定*/
					do
					{
						t_y = y_saver - 1 + randint1(3);
						t_x = x_saver - 1 + randint1(3);
						max_attempts--;
					}
					while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(y, x, t_y, t_x));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (is_seen(m_ptr))
					{
#ifdef JP
						if ((m_ptr->r_idx == MON_KENSHIROU) || (m_ptr->r_idx == MON_RAOU))
							msg_print("「北斗神拳奥義・二指真空把！」");
						else if (m_ptr->r_idx == MON_DIO) msg_print("ディオ・ブランドーは指一本で攻撃を弾き返した！");
						else msg_print("攻撃は跳ね返った！");
#else
						if ((m_ptr->r_idx == MON_KENSHIROU) || (m_ptr->r_idx == MON_RAOU))
							msg_print("'Hokuto Shinken-ougi Nishishinkuha!'");
						else if (m_ptr->r_idx == MON_DIO) msg_print("Dio Brando deflects the attack with one finger！");
						else msg_print("The attack bounces!");
#endif
					}
					if (is_original_ap_and_seen(m_ptr)) ref_ptr->r_flags2 |= RF2_REFLECTING;

					/* Reflected bolts randomly target either one */
					if (player_bold(y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					/*:::再帰処理で反射*/
					project(cave[y][x].m_idx, 0, t_y, t_x, dam, typ, flg, monspell);

					/* Don't affect the monster any longer */
					continue;
				}

				else if ((flg & PROJECT_REFLECTABLE) && !(MON_CSLEEP(m_ptr)) &&
					((m_ptr->r_idx == MON_NINJA_SLAYER) || (m_ptr->r_idx == MON_AYA) || (m_ptr->r_idx == MON_HATATE) || (m_ptr->r_idx == MON_KARASU_TENGU) || (m_ptr->r_idx == MON_SEIJA)|| (m_ptr->r_idx == MON_SEIJA_D)  ))
				{
					if((m_ptr->r_idx == MON_AYA) && !one_in_(10)
					|| (m_ptr->r_idx == MON_HATATE) && !one_in_(10)
					|| (m_ptr->r_idx == MON_SEIJA) && !one_in_(7)
					|| (m_ptr->r_idx == MON_SEIJA_D) && !one_in_(7)
					|| (m_ptr->r_idx == MON_KARASU_TENGU) && !one_in_(5))
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format(_("%^sは弾を撮影して消した！", "%^s takes a photo of the bullets, clearing them!"), m_name);
						return (notice);
					}
					else if (m_ptr->r_idx == MON_NINJA_SLAYER)
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format(_("「イヤーッ！」%^sはブリッジでこれを回避！", "%^s evades it with a bridge!"), m_name);
						return (notice);

					}


				}


			}


			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this monster */
			if (p_ptr->riding && player_bold(y, x))
			{
				//v1.1.84 舞や里乃は通常の騎乗でなく後ろで踊っているのでボルトが当たりにくい
				bool flag_shield = (who && CLASS_RIDING_BACKDANCE && !one_in_(4));

				if (flag_shield && (flg & PROJECT_REFLECTABLE)) flg &= ~(PROJECT_PLAYER);

				/* Aimed on the player */
				/*:::＠を狙った攻撃の場合*/
				if (flg & PROJECT_PLAYER)
				{


					/*:::＠だけが対象のとき騎乗中のモンスターに影響はない（＠への処理はあとで）*/
					//
					if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
					{
						/*
						 * A beam or bolt is well aimed
						 * at the PLAYER!
						 * So don't affects the mount.
						 */
						continue;
					}
					/*:::ボールやブレスは騎乗モンスターにも影響を与えるが威力は減衰する？*/
					else
					{
						/*
						 * The spell is not well aimed,
						 * So partly affect the mount too.
						 */
						effective_dist++;
					}
				}

				/*
				 * This grid is the original target.
				 * Or aimed on your horse.
				 */
				/*:::PROJCET_PLAYERでない場合モンスターが狙われた攻撃である　あとは薬の破壊とか？*/
				else if (((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				 * Otherwise this grid is not the
				 * original target, it means that line
				 * of fire is obstructed by this
				 * monster.
				 */
				/*
				 * A beam or bolt will hit either
				 * player or mount.  Choose randomly.
				 */
				/*:::ビームは＠とモンスターのどちらかにしか当たらない*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if (one_in_(2) || flag_shield)
					{
						/* Hit the mount with full damage */
					}
					else
					{
						/* Hit the player later */
						flg |= PROJECT_PLAYER;

						/* Don't affect the mount */
						continue;
					}
				}

				/*
				 * The spell is not well aimed, so
				 * partly affect both player and
				 * mount.
				 */
				/*:::ボールやブレスが離れたところで爆発した場合？威力が減衰しすぎる気がするが*/
				else
				{
					effective_dist++;
				}
			}

			/* Affect the monster in the grid */
			if (project_m(who, effective_dist, y, x, dam, typ, flg, see_s_msg)) notice = TRUE;
		}


		/* Player affected one monster (without "jumping") */
		/*:::＠の攻撃でモンスター一体だけがダメージを受けたとき思い出などを更新*/
		/*:::project_mの中でモンスターがダメージを受けたときprojcet_m_nが++される*/
		if (!who && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (cave[y][x].m_idx > 0)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				if (m_ptr->ml)
				{
					/* Hack -- auto-recall */
					if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					/* Hack - auto-track */
					if (m_ptr->ml) health_track(cave[y][x].m_idx);
				}
			}
		}

	}


	/* Check player */
	/*:::＠への処理*/
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if (!player_bold(y, x)) continue;

			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if (p_ptr->riding)
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
				{
					/* Hit the player with full damage */
				}

				/*
				 * Hack -- When this grid was not the
				 * original target, a beam or bolt
				 * would hit either player or mount,
				 * and should be choosen randomly.
				 *
				 * But already choosen to hit the
				 * mount at this point.
				 *
				 * Or aimed on your horse.
				 */
				/*:::ビームかボルトはモンスターから外れたときしかPROJECT_PLAYERがつかないので、PROJECT_PLAYERがないならすでに騎乗モンスターに当たっている*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
				{
					/*
					 * A beam or bolt is well aimed
					 * at the mount!
					 * So don't affects the player.
					 */
					continue;
				}
				else
				{
					/*
					 * The spell is not well aimed,
					 * So partly affect the player too.
					 */
					effective_dist++;
				}
			}
			/*:::敵の攻撃を逸らす特殊処理*/
			if(who > 0 && (PLAYER_CAN_USE_ARTS))
			{
				//光線を逸らす妖精
				if(CHECK_FAIRY_TYPE == 53 && flg & PROJECT_BEAM && typ == GF_LITE )
				{
					msg_print(_("あなたは光線を逸らした！", "You deflect the beam!"));
					break;
				}
				//格闘家の廻し受け
				if((p_ptr->pclass == CLASS_MARTIAL_ARTIST) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)) )
				{
					int caster_lev = r_info[m_list[who].r_idx].level;
					int power = p_ptr->lev;
					if( r_info[m_list[who].r_idx].flags2 & RF2_POWERFUL) caster_lev = caster_lev * 3 / 2;

					if(p_ptr->csp > 100) power = power * p_ptr->csp / 100;
					if(p_ptr->tim_general[0]) power *= 2;
					if(heavy_armor()) power /= 5;

					if(randint1(caster_lev) < randint1(power))
					{
						int check = damroll(3,p_ptr->lev);
						if(check < 30)
							msg_print(_("あなたは咄嗟に飛び退いて攻撃を回避した！",
                                        "You jump away and dodge the attack!"));
						else if(check < 80)
							msg_print(_("あなたは廻し受けで攻撃を回避した！",
                                        "You spin and dodge the attack!"));
						else if(check < 120)
							msg_print(_("あなたは硬気功で攻撃を回避した！",
                                        "You use a Ki technique to dodge the attack!"));
						else
							msg_print(_("あなたはフェニックスウィングで攻撃を回避した！",
                                        "You dodge the attack with a Phoenix Wing!"));

						break;
					}

				}
				//針妙丸小型化状態
				if(p_ptr->mimic_form == MIMIC_SHINMYOU && one_in_(2) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)))
				{
					msg_print(_("あなたは岩陰に隠れて攻撃をやり過ごした！",
                                "You hid between rocks and avoided the attack!"));
					break;
				}

				//v2.0.1 アビリティカード「気ままな無脊椎動物」ボルト確率無効
				if (flg & (PROJECT_STOP) && !rad)
				{
					int card_num = count_ability_card(ABL_CARD_JELLYFISH);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_JELLYFISH, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("クラゲが攻撃を食べた！", "Your jellyfish intercepts the attack!"));
							break;
						}
					}
				}


				//v1.1.86 アビリティカード「背中の扉」ボルトとビーム確率無効(光の剣除く)
				if (typ != GF_PSY_SPEAR && (flg & (PROJECT_BEAM | PROJECT_STOP)) && !rad)
				{
					int card_num = count_ability_card(ABL_CARD_BACKDOOR);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_BACKDOOR, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("攻撃は背中の扉に吸い込まれた！", "The attack gets absorbed into the door on your back!"));
							break;
						}
					}
				}
				//v1.1.86 アビリティカード「弾幕の亡霊」ボール確率無効
				if (!(flg & (PROJECT_THRU | PROJECT_STOP | PROJECT_MASTERSPARK)) && rad>0 && !breath)
				{
					int card_num = count_ability_card(ABL_CARD_DANMAKUGHOST);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_DANMAKUGHOST, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("弾幕の亡霊が魔法を消し去った！", "The danmaku ghost cancels the spell!"));
							break;
						}
					}
				}
				//v1.1.86 アビリティカード「重低音バスドラム」ロケット確率無効
				//v1.1.88 気弾処理が抜けてたので追加
				if (((flg & PROJECT_STOP) && rad && !breath))
				{
					int card_num = count_ability_card(ABL_CARD_BASS_DRUM);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_BASS_DRUM, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("重低音バスドラムが爆風と破片を相殺した！",
                                        "The heavy bass drum cancelled the explosion and shards!"));
							break;
						}
					}
				}
				//v1.1.86 アビリティカード「サイコキネシス」ブレス確率無効
				if (breath)
				{
					int card_num = count_ability_card(ABL_CARD_PSYCHOKINESIS);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_PSYCHOKINESIS, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("念動力の壁がブレスを防いだ！", "You block the breath with a psychokinetic barrier!"));
							break;
						}
					}
				}
				//v1.1.86 アビリティカード「鬱陶しいUFO」　光の剣と呪い系攻撃以外全てを確率でカット
				if (typ != GF_PSY_SPEAR && !(flg & PROJECT_AIMED))
				{
					int card_num = count_ability_card(ABL_CARD_UFO);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_UFO, card_num);
						if (randint1(100) <= prob)
						{
							msg_print(_("攻撃が鬱陶しいUFOに遮られた！", "The attack got blocked by the annoying UFO!"));
							break;
						}
					}



				}



			}

			if(who > 0 && p_ptr->pclass == CLASS_ICHIRIN &&  unzan_guard())
			{
				break;
			}
			//爆風防御　ブレスは防げないが自分が壁か森にいて敵が隣接している状態でブレスを吐かれた場合上の方でbreathフラグがFALSEになるので半減の対象になる。まあいいか
			else if((check_activated_nameless_arts(JKF1_EXPLOSION_DEF) || p_ptr->tim_res_blast)&& rad > 0 && !breath)
			{
				msg_print(_("あなたは攻撃に対しいくらかの耐性を示した。", "You somewhat resist the attack."));
				effective_dist++;
			}
			else if(check_activated_nameless_arts(JKF1_BREATH_DEF) && breath)
			{
				msg_print(_("あなたは攻撃に対しいくらかの耐性を示した。", "You somewhat resist the attack."));
				effective_dist++;
			}


			/* Affect the player */
			if (project_p(who, who_name, effective_dist, y, x, dam, typ, flg, monspell)) notice = TRUE;
		}
	}

	//v1.1.44 着弾地点からランダムな敵に向けて一度だけ再発動する特技の処理
	if (flg & (PROJECT_OPT))
	{
		monster_type *m_ptr_tmp;
		int tx, ty, j;
		int cnt = 0;

		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
			if (!projectable(by, bx, m_ptr_tmp->fy, m_ptr_tmp->fx)) continue;
			cnt++;
			if (!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}

		if (cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			project(-1, 0, ty, tx, dam, typ, flg, -1);
			//whoを-1にして再帰呼び出しする。発射地点としてmdn_tmp_x/yを記録する。
			//hack_project_start_x/yと機能が被るのでいずれ整頓したい
		}
	}
	//魔弾の射手用特殊処理 v1.1.44で削除
	/*
	if((flg & (PROJECT_OPT)) || hack_der_freischutz_basenum > 0 ||
		(flg & (PROJECT_MADAN)) && !one_in_((p_ptr->pclass == CLASS_REIMU) ? (osaisen_rank()) : 8))
	{
		monster_type *m_ptr_tmp;
		int tx,ty,j;
		int cnt=0;


		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
		//	if(!projectable(y,x, m_ptr_tmp->fy,m_ptr_tmp->fx) && (y != m_ptr_tmp->fy || x != m_ptr_tmp->fx)) continue;
			if(!projectable(by,bx, m_ptr_tmp->fy,m_ptr_tmp->fx)) continue;
			cnt++;
			if(!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		if(cheat_xtra) msg_format("魔弾Cnt:%d",cnt);

		if(hack_der_freischutz_basenum > 0 && cnt)
		{
			if(cheat_xtra) msg_format("basenum:%d",hack_der_freischutz_basenum);
			hack_der_freischutz_basenum -= MAX(1,(7 / cnt)); //敵が少ないとき数が減りやすいようにしとく。射程外から撃てるのであまり一体相手に連続で入るようにすると強すぎる。
			if(hack_der_freischutz_basenum<0) hack_der_freischutz_basenum=0;
		}

		if(cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			if(oonusa_dam)
			{
				if(p_ptr->pclass == CLASS_UTSUHO) project(-1,3,ty,tx,oonusa_dam,GF_LITE,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_REIMU) project(-1,(osaisen_rank() / 4),ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_SUWAKO) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_YUKARI) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else msg_print("ERROR:oonusa_dam関連の設定が不足");
			}
			else if(flg & (PROJECT_MADAN)) project(-1,1,ty,tx,p_ptr->lev * 2,GF_SHARDS,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
			else if(flg & (PROJECT_OPT)) project(-1,0,ty,tx,dam,typ,flg,-1);
		}
	}
	*/
	//六壬神火と貴竜の矢の再発動処理
	//v1.1.44 削除
	/*
	else if((flg & PROJECT_FUTO) && futo_broken_plate)
	{
		int next_x, next_y;
		mdn_tmp_x = bx;
		mdn_tmp_y = by;
		if(futo_determine_target(by,bx,&next_y,&next_x))
		{
			if(typ == GF_FIRE)
				return project(-1,0,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			else
				return project(-1,5,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
		}
	}
	*/

	if(flg & PROJECT_MOVE) //ブレイジングスター
	{
		teleport_player_to(by,bx,TELEPORT_NONMAGICAL);
	}

	//v1.1.53 クラウンピース(VFS)は火を見ると楽しくなる
	//(プレイヤーに見える位置で火炎、核熱のボールあるいはブレスが炸裂すると短時間ヒーロー化する)
	if (typ == GF_FIRE || typ == GF_NUKE )
	{
		if (rad && player_can_see_bold(hack_project_end_y, hack_project_end_x) )
		{
			clownpiece_likes_fire(typ);
		}

	}

	if (p_ptr->riding)
	{
		char m_name[80];

		monster_desc(m_name, &m_list[p_ptr->riding], 0);

		if (rakubadam_m > 0)
		{
			if (rakuba(rakubadam_m, FALSE))
			{
#ifdef JP
msg_format("%^sに振り落とされた！", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if (p_ptr->riding && rakubadam_p > 0)
		{
			if(rakuba(rakubadam_p, FALSE))
			{
#ifdef JP
msg_format("%^sから落ちてしまった！", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}
	}



	/* Return "something was noticed" */
	return (notice);
}

bool binding_field( int dam )
{
	int mirror_x[10],mirror_y[10]; /* 鏡はもっと少ない */
	int mirror_num=0;              /* 鏡の数 */
	int x,y;
	int centersign;
	int x1,x2,y1,y2;
	u16b p;
	int msec= delay_factor*delay_factor*delay_factor;

	/* 三角形の頂点 */
	int point_x[3];
	int point_y[3];

	/* Default target of monsterspell is player */
	monster_target_y=py;
	monster_target_x=px;

	for( x=0 ; x < cur_wid ; x++ )
	{
		for( y=0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x]) &&
			    distance(py,px,y,x) <= MAX_RANGE &&
			    distance(py,px,y,x) != 0 &&
			    player_has_los_bold(y,x) &&
			    projectable(py, px, y, x)
			    ){
				mirror_y[mirror_num]=y;
				mirror_x[mirror_num]=x;
				mirror_num++;
			}
		}
	}

	if( mirror_num < 2 )return FALSE;

	point_x[0] = randint0( mirror_num );
	do {
	  point_x[1] = randint0( mirror_num );
	}
	while( point_x[0] == point_x[1] );

	point_y[0]=mirror_y[point_x[0]];
	point_x[0]=mirror_x[point_x[0]];
	point_y[1]=mirror_y[point_x[1]];
	point_x[1]=mirror_x[point_x[1]];
	point_y[2]=py;
	point_x[2]=px;

	x=point_x[0]+point_x[1]+point_x[2];
	y=point_y[0]+point_y[1]+point_y[2];

	centersign = (point_x[0]*3-x)*(point_y[1]*3-y)
		- (point_y[0]*3-y)*(point_x[1]*3-x);
	if( centersign == 0 )return FALSE;

	x1 = point_x[0] < point_x[1] ? point_x[0] : point_x[1];
	x1 = x1 < point_x[2] ? x1 : point_x[2];
	y1 = point_y[0] < point_y[1] ? point_y[0] : point_y[1];
	y1 = y1 < point_y[2] ? y1 : point_y[2];

	x2 = point_x[0] > point_x[1] ? point_x[0] : point_x[1];
	x2 = x2 > point_x[2] ? x2 : point_x[2];
	y2 = point_y[0] > point_y[1] ? point_y[0] : point_y[1];
	y2 = y2 > point_y[2] ? y2 : point_y[2];

	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					/* Visual effects */
					if(!(p_ptr->blind)
					   && panel_contains(y,x)){
					  p = bolt_pict(y,x,y,x, GF_MANA );
					  print_rel(PICT_C(p), PICT_A(p),y,x);
					  move_cursor_relative(y, x);
					  /*if (fresh_before)*/ Term_fresh();
					  Term_xtra(TERM_XTRA_DELAY, msec);
					}
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_f(0,0,y,x,dam,GF_MANA);
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_o(0,0,y,x,dam,GF_MANA);
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_m(0,0,y,x,dam,GF_MANA,
					  (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP),TRUE);
				}
			}
		}
	}
	if( one_in_(7) ){
#ifdef JP
		msg_print("鏡が結界に耐えきれず、壊れてしまった。");
#else
		msg_print("The field broke a mirror");
#endif
		remove_mirror(point_y[0],point_x[0]);
	}

	return TRUE;
}

void seal_of_mirror( int dam )
{
	int x,y;

	for( x = 0 ; x < cur_wid ; x++ )
	{
		for( y = 0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x]))
			{
				if(project_m(0,0,y,x,dam,GF_GENOCIDE,
							 (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP),TRUE))
				{
					if( !cave[y][x].m_idx )
					{
						remove_mirror(y,x);
					}
				}
			}
		}
	}
	return;
}


#if 0
/*:::魔理沙ブレイジングスター専用ルーチン　project()を改変*/
bool blazing_star(int who, int rad, int dir, int *y_end, int x_end, int dam)
{
	int i, t;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int msec = delay_factor * delay_factor * delay_factor;
	int path_n = 0;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Actual grids in the "path" */
	u16b path_g[512];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[1024], gy[1024];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	if(dir == 5)
	{
		msg_print("ERROR:blazing_star()に渡したdirが5");
	}

	x1 = px;
	y1 = py;

	/* Default "destination" */
	y2 = py + ddy[dir] * 256;
	x2 = px + ddx[dir] * 256;

	i=0;

	//	path_n = project_path(path_g, (range), y1, x1, y2, x2, flg);
	while(1)
	{
		int tx,ty;
		tx = px + ddx[dir] * i;
		ty = py + ddy[dir] * i;
		if(!in_bounds(ty,tx)) break;
		if(cave_have_flag_bold(ty,tx,FF_PERMANENT)) break;
		path_g[path_n] = GRID(ty, tx);
		path_n++;

	}
	if(!path_n)
	{
		msg_print("そちらには撃てない。");
		return FALSE;
	}

	/* Hack -- Handle stuff */
	handle_stuff();

#if 0
	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{

		int oy = py;
		int ox = px;

		/*:::path_g[]から座標を取り出す*/
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		/*:::ボルトの軌跡描画処理　ビームでもここに入るがあとから書き換えてる*/
		if (!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				/*:::ボルトを表示するときの文字と色を決める*/
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/*:::Hack 怨霊憑依の軌跡は'G'で表現*/
				if(typ == GF_POSSESSION) c = 'G';

				/* Visual effects */
				/*:::指定位置に一文字描画*/
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);
				/*if (fresh_before)*/ Term_fresh();
				/*:::基本ウェイト量分のディレイ処理と思われる*/
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				/*if (fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				/*:::ビームならここに入り、描画を'*'に書き換えている*/
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

	}
#endif

	/*:::ここで壁に当たった分グリッド数を減らしている*/
	//path_n = i;

	/* Save the "blast epicenter" */
	by = y;
	bx = x;


	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = path_n;
	dist_hack = dist;

	project_length = 0;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		//if ((flg & (PROJECT_BEAM)) && !(flg & (PROJECT_FUTO)) && (grids > 0)) grids--;
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*:::
		 *
		 * Create a columnar master spark
		 *
		 *    **************
		 *   ***************
		 * p********@*******
		 *   ***************
		 *    **************
		 */
		if(flg & (PROJECT_MASTERSPARK)) breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,TRUE);
		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		else if (breath)
		{
			flg &= ~(PROJECT_HIDE);
			/*:::ブレスの効果が及ぶ全グリッドを計算する*/
			breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,FALSE);
		}
		/*:::ボールの爆発の影響が及ぶ全グリッドを計算する*/
		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				/*:::正方形にスキャンしdistから外れたらcontinue*/
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!in_bounds2(y, x)) continue;

						///mod150308 菊一文字コンプレッサー専用　上下にしか爆発しない
						if(flg & (PROJECT_COMPRESS) && x !=bx) continue;
						///mod150720 貴竜の矢は敵に当たったときは爆発せず、皿に当たった時も斜め方向にしか爆発しない
						if(flg & (PROJECT_FUTO))
						{
							if(futo_broken_plate)
							{
								if(((x-bx != y-by) && (x-bx != by-y) )) continue;
							}
							else
							{
								if(x != bx || y != by) continue;
							}
						}

						/* Enforce a "circular" explosion */
						if (distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case GF_LITE:
						case GF_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if (!los(by, bx, y, x)) continue;
							break;
						case GF_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if (!in_disintegration_range(by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if (!projectable(by, bx, y, x)) continue;
							break;
						}

						/* Save this grid */
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}
	/*:::影響範囲の算出終了*/

	/* Speed -- ignore "non-explosions" */
	/*:::ボールやボルトが射程範囲外まで飛んだとき？*/
	if (!grids) return (FALSE);


	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					/*:::ここも描画文字は*になる*/
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush each "radius" seperately */
			/*if (fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}
		/*:::描画計算した中から＠に見えてる部分だけ描画している？*/
		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if (fresh_before)*/ Term_fresh();
		}
	}


	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/*:::モンスターにダメージを与える効果の場合、＠から見えるならメッセージを出す？*/
	/*:::どうもwhoが負で呼ばれることもあるらしい。どんな時だ？←ガラスが割れた時など*/
	if (flg & PROJECT_KILL)
	{
		see_s_msg = (who > 0) ? is_seen(&m_list[who]) :
			(!who ? TRUE : (player_can_see_bold(y1, x1) && projectable(py, px, y1, x1)));
	}


	/* Check features */
	/*:::床上に効果を及ぼす場合*/
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			/*:::gm[]に距離ごとのgx,gy[]開始位置が入ってることを利用*/
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the grid */
				if (project_f(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/* Check objects */
	/*:::床上のアイテムに効果を及ぼす場合*/
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if (project_o(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}

	}


	/* Check monsters */
	/*:::モンスターに効果を及ぼす場合*/
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* A single bolt may be reflected */
			///sys 反射処理　ボルト無効や弾幕回避もここで処理しよう
			if (grids <= 1)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				monster_race *ref_ptr = &r_info[m_ptr->r_idx];
				/*:::反射可能でx,yにモンスターがいてそのモンスターが反射持ちで＠の騎乗モンスターでなく＠への攻撃でもないとき9/10で反射*/
				if ((flg & PROJECT_REFLECTABLE) && cave[y][x].m_idx && (ref_ptr->flags2 & RF2_REFLECTING) &&
				    ((cave[y][x].m_idx != p_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
				    (!who || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					/*:::反射先を決定*/
					do
					{
						t_y = y_saver - 1 + randint1(3);
						t_x = x_saver - 1 + randint1(3);
						max_attempts--;
					}
					while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(y, x, t_y, t_x));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (is_seen(m_ptr))
					{
#ifdef JP
						if ((m_ptr->r_idx == MON_KENSHIROU) || (m_ptr->r_idx == MON_RAOU))
							msg_print("「北斗神拳奥義・二指真空把！」");
						else if (m_ptr->r_idx == MON_DIO) msg_print("ディオ・ブランドーは指一本で攻撃を弾き返した！");
						else msg_print("攻撃は跳ね返った！");
#else
						msg_print("The attack bounces!");
#endif
					}
					if (is_original_ap_and_seen(m_ptr)) ref_ptr->r_flags2 |= RF2_REFLECTING;

					/* Reflected bolts randomly target either one */
					if (player_bold(y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					/*:::再帰処理で反射*/
					project(cave[y][x].m_idx, 0, t_y, t_x, dam, typ, flg, monspell);

					/* Don't affect the monster any longer */
					continue;
				}

				else if ((flg & PROJECT_REFLECTABLE) && !(MON_CSLEEP(m_ptr)) &&
					((m_ptr->r_idx == MON_NINJA_SLAYER) || (m_ptr->r_idx == MON_AYA) || (m_ptr->r_idx == MON_HATATE) || (m_ptr->r_idx == MON_KARASU_TENGU) || (m_ptr->r_idx == MON_SEIJA)|| (m_ptr->r_idx == MON_SEIJA_D)  ))
				{
					if((m_ptr->r_idx == MON_AYA) && !one_in_(10)
					|| (m_ptr->r_idx == MON_HATATE) && !one_in_(10)
					|| (m_ptr->r_idx == MON_SEIJA) && !one_in_(7)
					|| (m_ptr->r_idx == MON_SEIJA_D) && !one_in_(7)
					|| (m_ptr->r_idx == MON_KARASU_TENGU) && !one_in_(5))
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("%^sは弾を撮影して消した！", m_name);
						return (notice);
					}
					else if (m_ptr->r_idx == MON_NINJA_SLAYER)
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("「イヤーッ！」%^sはブリッジでこれを回避！", m_name);
						return (notice);

					}


				}


			}


			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this monster */
			if (p_ptr->riding && player_bold(y, x))
			{
				/* Aimed on the player */
				/*:::＠を狙った攻撃の場合*/
				if (flg & PROJECT_PLAYER)
				{
					/*:::＠だけが対象のとき騎乗中のモンスターに影響はない（＠への処理はあとで）*/
					if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
					{
						/*
						 * A beam or bolt is well aimed
						 * at the PLAYER!
						 * So don't affects the mount.
						 */
						continue;
					}
					/*:::ボールやブレスは騎乗モンスターにも影響を与えるが威力は減衰する？*/
					else
					{
						/*
						 * The spell is not well aimed,
						 * So partly affect the mount too.
						 */
						effective_dist++;
					}
				}

				/*
				 * This grid is the original target.
				 * Or aimed on your horse.
				 */
				/*:::PROJCET_PLAYERでない場合モンスターが狙われた攻撃である　あとは薬の破壊とか？*/
				else if (((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				 * Otherwise this grid is not the
				 * original target, it means that line
				 * of fire is obstructed by this
				 * monster.
				 */
				/*
				 * A beam or bolt will hit either
				 * player or mount.  Choose randomly.
				 */
				/*:::ビームは＠とモンスターのどちらかにしか当たらない*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if (one_in_(2))
					{
						/* Hit the mount with full damage */
					}
					else
					{
						/* Hit the player later */
						flg |= PROJECT_PLAYER;

						/* Don't affect the mount */
						continue;
					}
				}

				/*
				 * The spell is not well aimed, so
				 * partly affect both player and
				 * mount.
				 */
				/*:::ボールやブレスが離れたところで爆発した場合？威力が減衰しすぎる気がするが*/
				else
				{
					effective_dist++;
				}
			}

			/* Affect the monster in the grid */
			if (project_m(who, effective_dist, y, x, dam, typ, flg, see_s_msg)) notice = TRUE;
		}


		/* Player affected one monster (without "jumping") */
		/*:::＠の攻撃でモンスター一体だけがダメージを受けたとき思い出などを更新*/
		/*:::project_mの中でモンスターがダメージを受けたときprojcet_m_nが++される*/
		if (!who && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (cave[y][x].m_idx > 0)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				if (m_ptr->ml)
				{
					/* Hack -- auto-recall */
					if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					/* Hack - auto-track */
					if (m_ptr->ml) health_track(cave[y][x].m_idx);
				}
			}
		}

	}


	/* Check player */
	/*:::＠への処理*/
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if (!player_bold(y, x)) continue;

			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if (p_ptr->riding)
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
				{
					/* Hit the player with full damage */
				}

				/*
				 * Hack -- When this grid was not the
				 * original target, a beam or bolt
				 * would hit either player or mount,
				 * and should be choosen randomly.
				 *
				 * But already choosen to hit the
				 * mount at this point.
				 *
				 * Or aimed on your horse.
				 */
				/*:::ビームかボルトはモンスターから外れたときしかPROJECT_PLAYERがつかないので、PROJECT_PLAYERがないならすでに騎乗モンスターに当たっている*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
				{
					/*
					 * A beam or bolt is well aimed
					 * at the mount!
					 * So don't affects the player.
					 */
					continue;
				}
				else
				{
					/*
					 * The spell is not well aimed,
					 * So partly affect the player too.
					 */
					effective_dist++;
				}
			}
			/*:::敵の攻撃を逸らす特殊処理*/
			if(who > 0 && !p_ptr->blind && !p_ptr->paralyzed && !p_ptr->confused)
			{
				if(CHECK_FAIRY_TYPE == 53 && flg & PROJECT_BEAM && typ == GF_LITE )
				{
					msg_print("あなたは光線を逸らした！");
					break;
				}
				//格闘家の廻し受け
				if((p_ptr->pclass == CLASS_MARTIAL_ARTIST) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)) )
				{
					int caster_lev = r_info[m_list[who].r_idx].level;
					int power = p_ptr->lev;
					if( r_info[m_list[who].r_idx].flags2 & RF2_POWERFUL) caster_lev = caster_lev * 3 / 2;

					if(p_ptr->csp > 100) power = power * p_ptr->csp / 100;
					if(p_ptr->tim_general[0]) power *= 2;
					if(heavy_armor()) power /= 5;

					if(randint1(caster_lev) < randint1(power))
					{
						int check = damroll(3,p_ptr->lev);
						if(check < 30)
							msg_print("あなたは咄嗟に飛び退いて攻撃を回避した！");
						else if(check < 80)
							msg_print("あなたは廻し受けで攻撃を回避した！");
						else if(check < 120)
							msg_print("あなたは硬気功で攻撃を回避した！");
						else
							msg_print("あなたはフェニックスウィングで攻撃を回避した！");

						break;
					}

				}
				if(p_ptr->mimic_form == MIMIC_SHINMYOU && one_in_(2) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)))
				{
					msg_print("あなたは岩陰に隠れて攻撃をやり過ごした！");
					break;
				}
			}
			else if(who > 0 && p_ptr->pclass == CLASS_ICHIRIN &&  unzan_guard())
			{
				break;
			}


			/* Affect the player */
			if (project_p(who, who_name, effective_dist, y, x, dam, typ, flg, monspell)) notice = TRUE;
		}
	}
	//魔弾の射手用特殊処理
	if((flg & (PROJECT_OPT)) || (flg & (PROJECT_MADAN)) && !one_in_((p_ptr->pclass == CLASS_REIMU) ? (osaisen_rank()) : 8))
	{
		monster_type *m_ptr_tmp;
		int tx,ty,j;
		int cnt=0;
		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
		//	if(!projectable(y,x, m_ptr_tmp->fy,m_ptr_tmp->fx) && (y != m_ptr_tmp->fy || x != m_ptr_tmp->fx)) continue;
			if(!projectable(by,bx, m_ptr_tmp->fy,m_ptr_tmp->fx)) continue;
			cnt++;
			if(!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		if(cheat_xtra) msg_format("魔弾Cnt:%d",cnt);
		if(cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			if(oonusa_dam)
			{
				if(p_ptr->pclass == CLASS_UTSUHO) project(-1,3,ty,tx,oonusa_dam,GF_LITE,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_REIMU) project(-1,(osaisen_rank() / 4),ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_SUWAKO) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else msg_print("ERROR:oonusa_dam関連の設定が不足");
			}
			else if(flg & (PROJECT_MADAN)) project(-1,1,ty,tx,p_ptr->lev * 2,GF_SHARDS,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
			else if(flg & (PROJECT_OPT)) project(-1,0,ty,tx,dam,typ,flg,-1);
		}
	}
	//六壬神火と貴竜の矢の再発動処理
	else if((flg & PROJECT_FUTO) && futo_broken_plate)
	{
		int next_x, next_y;
		mdn_tmp_x = bx;
		mdn_tmp_y = by;
		if(futo_determine_target(by,bx,&next_y,&next_x))
		{
			if(typ == GF_FIRE)
				return project(-1,0,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			else
				return project(-1,5,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
		}
	}



	if (p_ptr->riding)
	{
		char m_name[80];

		monster_desc(m_name, &m_list[p_ptr->riding], 0);

		if (rakubadam_m > 0)
		{
			if (rakuba(rakubadam_m, FALSE))
			{
#ifdef JP
msg_format("%^sに振り落とされた！", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if (p_ptr->riding && rakubadam_p > 0)
		{
			if(rakuba(rakubadam_p, FALSE))
			{
#ifdef JP
msg_format("%^sから落ちてしまった！", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}
	}

	/* Return "something was noticed" */
	return (notice);
}


#endif


