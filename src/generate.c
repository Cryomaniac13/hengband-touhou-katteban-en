/* File: generate.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Dungeon generation */

/*
 * Note that Level generation is *not* an important bottleneck,
 * though it can be annoyingly slow on older machines...  Thus
 * we emphasize "simplicity" and "correctness" over "speed".
 *
 * This entire file is only needed for generating levels.
 * This may allow smart compilers to only load it when needed.
 *
 * Consider the "v_info.txt" file for vault generation.
 *
 * In this file, we use the "special" granite and perma-wall sub-types,
 * where "basic" is normal, "inner" is inside a room, "outer" is the
 * outer wall of a room, and "solid" is the outer wall of the dungeon
 * or any walls that may not be pierced by corridors.  Thus the only
 * wall type that may be pierced by a corridor is the "outer granite"
 * type.  The "basic granite" type yields the "actual" corridors.
 *
 * Note that we use the special "solid" granite wall type to prevent
 * multiple corridors from piercing a wall in two adjacent locations,
 * which would be messy, and we use the special "outer" granite wall
 * to indicate which walls "surround" rooms, and may thus be "pierced"
 * by corridors entering or leaving the room.
 *
 * Note that a tunnel which attempts to leave a room near the "edge"
 * of the dungeon in a direction toward that edge will cause "silly"
 * wall piercings, but will have no permanently incorrect effects,
 * as long as the tunnel can *eventually* exit from another side.
 * And note that the wall may not come back into the room by the
 * hole it left through, so it must bend to the left or right and
 * then optionally re-enter the room (at least 2 grids away).  This
 * is not a problem since every room that is large enough to block
 * the passage of tunnels is also large enough to allow the tunnel
 * to pierce the room itself several times.
 *
 * Note that no two corridors may enter a room through adjacent grids,
 * they must either share an entryway or else use entryways at least
 * two grids apart.  This prevents "large" (or "silly") doorways.
 *
 * To create rooms in the dungeon, we first divide the dungeon up
 * into "blocks" of 11x11 grids each, and require that all rooms
 * occupy a rectangular group of blocks.  As long as each room type
 * reserves a sufficient number of blocks, the room building routines
 * will not need to check bounds.  Note that most of the normal rooms
 * actually only use 23x11 grids, and so reserve 33x11 grids.
 *
 * Note that the use of 11x11 blocks (instead of the old 33x11 blocks)
 * allows more variability in the horizontal placement of rooms, and
 * at the same time has the disadvantage that some rooms (two thirds
 * of the normal rooms) may be "split" by panel boundaries.  This can
 * induce a situation where a player is in a room and part of the room
 * is off the screen.  It may be annoying enough to go back to 33x11
 * blocks to prevent this visual situation.
 *
 * Note that the dungeon generation routines are much different (2.7.5)
 * and perhaps "DUN_ROOMS" should be less than 50.
 *
 * XXX XXX XXX Note that it is possible to create a room which is only
 * connected to itself, because the "tunnel generation" code allows a
 * tunnel to leave a room, wander around, and then re-enter the room.
 *
 * XXX XXX XXX Note that it is possible to create a set of rooms which
 * are only connected to other rooms in that set, since there is nothing
 * explicit in the code to prevent this from happening.  But this is less
 * likely than the "isolated room" problem, because each room attempts to
 * connect to another room, in a giant cycle, thus requiring at least two
 * bizarre occurances to create an isolated section of the dungeon.
 *
 * Note that (2.7.9) monster pits have been split into monster "nests"
 * and monster "pits".  The "nests" have a collection of monsters of a
 * given type strewn randomly around the room (jelly, animal, or undead),
 * while the "pits" have a collection of monsters of a given type placed
 * around the room in an organized manner (orc, troll, giant, dragon, or
 * demon).  Note that both "nests" and "pits" are now "level dependant",
 * and both make 16 "expensive" calls to the "get_mon_num()" function.
 *
 * Note that the cave grid flags changed in a rather drastic manner
 * for Angband 2.8.0 (and 2.7.9+), in particular, dungeon terrain
 * features, such as doors and stairs and traps and rubble and walls,
 * are all handled as a set of 64 possible "terrain features", and
 * not as "fake" objects (440-479) as in pre-2.8.0 versions.
 *
 * The 64 new "dungeon features" will also be used for "visual display"
 * but we must be careful not to allow, for example, the user to display
 * hidden traps in a different way from floors, or secret doors in a way
 * different from granite walls, or even permanent granite in a different
 * way from granite.  XXX XXX XXX
 */

#include "angband.h"
#include "generate.h"
#include "grid.h"
#include "rooms.h"
#include "streams.h"

int dun_tun_rnd;
int dun_tun_chg;
int dun_tun_con;
int dun_tun_pen;
int dun_tun_jct;


/*
 * Dungeon generation data -- see "cave_gen()"
 */
dun_data *dun;


/*
 * Count the number of walls adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds(y, x)"
 *
 * We count only granite walls and permanent walls.
 */
static int next_to_walls(int y, int x)
{
	int k = 0;

	if (in_bounds(y + 1, x) && is_extra_bold(y + 1, x)) k++;
	if (in_bounds(y - 1, x) && is_extra_bold(y - 1, x)) k++;
	if (in_bounds(y, x + 1) && is_extra_bold(y, x + 1)) k++;
	if (in_bounds(y, x - 1) && is_extra_bold(y, x - 1)) k++;

	return (k);
}


/*
 *  Helper function for alloc_stairs().
 *
 *  Is this a good location for stairs?
 */
static bool alloc_stairs_aux(int y, int x, int walls)
{
	/* Access the grid */
	cave_type *c_ptr = &cave[y][x];

	/* Require "naked" floor grid */
	if (!is_floor_grid(c_ptr)) return FALSE;
	if (pattern_tile(y, x)) return FALSE;
	if (c_ptr->o_idx || c_ptr->m_idx) return FALSE;

	/* Require a certain number of adjacent walls */
	if (next_to_walls(y, x) < walls) return FALSE;

	return TRUE;
}


/*
 * Places some staircases near walls
 */
static bool alloc_stairs(int feat, int num, int walls)
{
	int i;
	int shaft_num = 0;

	feature_type *f_ptr = &f_info[feat];

	if (have_flag(f_ptr->flags, FF_LESS))
	{
		/* No up stairs in town or in ironman mode */
		if (ironman_downward || !dun_level) return TRUE;

		if (dun_level > d_info[dungeon_type].mindepth)
			shaft_num = (randint1(num+1))/2;
	}
	else if (have_flag(f_ptr->flags, FF_MORE))
	{
		int q_idx = quest_number(dun_level);

		/* No downstairs on quest levels */
		if (dun_level > 1 && q_idx)
		{
			monster_race *r_ptr = &r_info[quest[q_idx].r_idx];

			/* The quest monster(s) is still alive? */
			if (!(r_ptr->flags1 & RF1_UNIQUE) || 0 < r_ptr->max_num)
				return TRUE;
		}

		/* No downstairs at the bottom */
		if (dun_level >= d_info[dungeon_type].maxdepth) return TRUE;

		/*:::Easyでは50階以降に行けない*/
		if(difficulty == DIFFICULTY_EASY && dungeon_type == DUNGEON_ANGBAND &&  dun_level == 50) return TRUE;


		if ((dun_level < d_info[dungeon_type].maxdepth-1) && !quest_number(dun_level+1))
			shaft_num = (randint1(num)+1)/2;

		/*:::Easyでは50階以降に行けない*/
		if(difficulty == DIFFICULTY_EASY && dungeon_type == DUNGEON_ANGBAND &&  dun_level > 48) shaft_num = 0;
	}

	/* Paranoia */
	else return FALSE;

	if(EXTRA_MODE) shaft_num = 0;

	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		while (TRUE)
		{
			int y = 0, x = 0;
			cave_type *c_ptr;

			int candidates = 0;
			int pick;

			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if (alloc_stairs_aux(y, x, walls))
					{
						/* A valid space found */
						candidates++;
					}
				}
			}

			/* No valid place! */
			if (!candidates)
			{
				/* There are exactly no place! */
				if (walls <= 0) return FALSE;

				/* Decrease walls limit, and try again */
				walls--;
				continue;
			}

			/* Choose a random one */
			pick = randint1(candidates);

			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if (alloc_stairs_aux(y, x, walls))
					{
						pick--;

						/* Is this a picked one? */
						if (!pick) break;
					}
				}

				if (!pick) break;
			}

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Clear possible garbage of hidden trap */
			c_ptr->mimic = 0;

			/* Clear previous contents, add stairs */
			c_ptr->feat = (i < shaft_num) ? feat_state(feat, FF_SHAFT) : feat;

			/* No longer "FLOOR" */
			c_ptr->info &= ~(CAVE_FLOOR);

			/* Success */
			break;
		}
	}
	return TRUE;
}

/*:::特定のダンジョンに特定のアイテムを群生して配置する。配置される確率は探索技能に依存する。*/
static void alloc_souvenir(void)
{
	int i,j,x,y;
	object_type forge;
	object_type *o_ptr = &forge;

	if(!dun_level || !dungeon_type) return;
	if( p_ptr->skill_srh < 1) return;

	/*:::特産品テーブルを最後までループ*/
	for(i=0;souvenir_table[i].dun_num;i++)
	{
		int chance, mult;
		if(dungeon_type != souvenir_table[i].dun_num || dun_level <  souvenir_table[i].min_lev || dun_level >  souvenir_table[i].max_lev) continue;
		if(souvenir_table[i].tval == TV_MATERIAL && souvenir_table[i].sval == SV_MATERIAL_HIHIIROKANE && !weird_luck()) continue; //緋緋色金は特別出にくい

		if(!(EXTRA_MODE) && souvenir_table[i].tval == TV_ITEMCARD) continue; //アイテムカードはEXTRAモードでしか出ない

		/*:::Memo:skill_srhは開始時20～60、最終50～150程度。よほど職や装備を選ばないと300は超えない*/
		mult = p_ptr->skill_srh * p_ptr->skill_srh / 300;
		if(mult > 500) mult = 500;
		if(mult < 1) mult = 1;
		chance = souvenir_table[i].chance * mult;

		//v1.1.75 わかさぎ姫は石(TV_MATERIAL分類)を見つける確率が高いことにする
		if (p_ptr->pclass == CLASS_WAKASAGI && (souvenir_table[i].tval == TV_MATERIAL)) chance *= 2;

		if(chance > 5000) chance = 5000;


		/*:::狭いダンジョンでは発見確率を減らす*/
		//v1.1.61 EXTRAモードでは減らさない
		if(!EXTRA_MODE)
			chance = chance * cur_hgt / MAX_HGT * cur_wid / MAX_WID;

		/*:::アイテムを「発見」したら配置処理に移る。*/
		if(randint0(10000) < chance)
		{
			cave_type *c_ptr;
			int num = randint1(souvenir_table[i].max_num);
			int dummy = 0;

			if (cheat_peek) msg_format(_("ダンジョン特産品生成 TV: %d SV: %d Num: %d ",
                                        "Dungeon specialty item generated TV: %d SV: %d Num: %d "), souvenir_table[i].tval,souvenir_table[i].sval,num);
			/* まずはダンジョン全域からアイテムを置ける場所を一か所探す*/
			while (dummy < SAFE_MAX_ATTEMPTS)
			{
				dummy++;
				y = randint0(cur_hgt);
				x = randint0(cur_wid);
				c_ptr = &cave[y][x];
				if (!cave_drop_bold(y, x) || c_ptr->o_idx || c_ptr->m_idx ||!in_bounds(y, x)) continue;
				if (!cave_empty_bold(y, x)) continue;
				if (player_bold(y, x)) continue;
				break;
			}

			if (dummy >= SAFE_MAX_ATTEMPTS)
			{
				if (cheat_peek) msg_print(_("WARNING:ダンジョン特産品アイテム配置不可",
                                            "WARNING: Failed to place dungeon specialty item"));
				return;
			}
			/*:::見つけた場所周辺で配置にふさわしい場所を数か所探し、アイテムを配置する*/
			for(j = 0;j < num;j++){
				int attempts = 500;
				int place_x,place_y;
				while (attempts--)
				{
					scatter(&place_y, &place_x, y, x, 3, 0);
					c_ptr = &cave[place_y][place_x];
					if (!cave_drop_bold(place_y, place_x) || c_ptr->o_idx || c_ptr->m_idx ||!in_bounds(place_y, place_x)) continue;
					if (!cave_empty_bold(place_y, place_x)) continue;
					if (player_bold(place_y, place_x)) continue;
					break;
				}
				if(attempts == 0) break;
				object_prep(o_ptr, lookup_kind(souvenir_table[i].tval, souvenir_table[i].sval));
				apply_magic(o_ptr, base_level, AM_NO_FIXED_ART);
				drop_near(o_ptr,-1,place_y,place_x);
	//			drop_here(o_ptr,place_y,place_x);
			}

		}

	}


}


/*:::特定のダンジョンに特定のアイテムを配置する。現在は鈴瑚の支援物資投下のみ*/
static void alloc_special_item(void)
{
	int i,j,x,y;
	object_type forge;
	object_type *o_ptr = &forge;

	if(!dun_level || !dungeon_type) return;

	//鈴瑚の支援物資投下カウンタが1000を超えたとき
	if(p_ptr->pclass == CLASS_RINGO && p_ptr->magic_num1[4] > 1000)
	{

		cave_type *c_ptr;
		int dummy = 0;
		int dummy_x, dummy_y;
		/* ダンジョン全域からアイテムを置ける場所を一か所探す*/
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			dummy++;
			y = randint0(cur_hgt);
			x = randint0(cur_wid);
			c_ptr = &cave[y][x];
			if (!in_bounds(y, x) || !cave_drop_bold(y, x) || c_ptr->o_idx || c_ptr->m_idx ) continue;
			if (!cave_empty_bold(y, x)) continue;
			if (player_bold(y, x)) continue;
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_peek) msg_print(_("WARNING:鈴瑚支援物資アイテム配置不可",
                                        "WARNING: Failed to place supplies for Ringo"));
			return;
		}
		object_prep(o_ptr, lookup_kind(TV_CHEST, SV_CHEST_SUPPLY));
		apply_magic(o_ptr, base_level, AM_NO_FIXED_ART);
		drop_near(o_ptr,-1,y,x);

		p_ptr->magic_num1[4] = 0;
		dummy_x = (x-6+randint1(11));
		dummy_y = (y-6+randint1(11));
		if(dummy_x < 1) dummy_x = 1;
		if(dummy_y < 1) dummy_y = 1;
		msg_format(_("《こちら清蘭、ポイント%d,%d付近に支援物資を投下した。》",
                    "< This is Seiran; Relief supplies delivered around point %d,%d. >"),dummy_x,dummy_y);

	}

}


/*:::霊夢の占いで宝物を予言された時特定ダンジョンの特定階に高価な物を生成*/
static void alloc_reimu_treasure(void)
{
	int x,y;
	object_type forge;
	object_type *o_ptr = &forge;
	int power;
	int dummy = 0;
	cave_type *c_ptr;

	//草の根ネットワーク未加入で占い結果がお宝で未生成のときのみ先に進む
	if(p_ptr->grassroots || p_ptr->today_mon != FORTUNETELLER_TREASURE || !p_ptr->barter_value) return;
	if(!dun_level || !dungeon_type) return;
	//特定ダンジョンの特定階層のときのみ先に進む
	if( dun_level != p_ptr->barter_value % 1000 || dungeon_type != p_ptr->barter_value / 1000) return;

	power = dun_level / 2;

	while(!one_in_(3)) power += 10;
	while(power < 1000 && ( one_in_(7) || weird_luck() || weird_luck())) power *= 2;
	power += randint1(10);
	if(power > 1000) power = 1000;

	if(power < 10) //三冊目
	{
		object_prep(o_ptr, lookup_kind(randint1(13),2));
	}
	else if(power < 20) //金5000～10000
	{
		object_prep(o_ptr, OBJ_GOLD_LIST + MAX_GOLD - 1);
		o_ptr->pval = 5000 + randint0(5000);
	}
	else if(power < 60) //高級生成の良ベース武器・リボン
	{
		if(one_in_(11))		object_prep(o_ptr, lookup_kind(TV_RIBBON,SV_BIG_RIBBON));
		else if(one_in_(10))object_prep(o_ptr, lookup_kind(TV_KNIFE,SV_WEAPON_HAYABUSA));
		else if(one_in_(9)) object_prep(o_ptr, lookup_kind(TV_SWORD,SV_WEAPON_BLADE_OF_CHAOS));
		else if(one_in_(8))	object_prep(o_ptr, lookup_kind(TV_KATANA,SV_WEAPON_ANCIENT_KATANA));
		else if(one_in_(7))	object_prep(o_ptr, lookup_kind(TV_HAMMER,SV_WEAPON_KANASAIBOU));
		else if(one_in_(6))	object_prep(o_ptr, lookup_kind(TV_STICK,SV_WEAPON_IRONSTICK));
		else if(one_in_(5))	object_prep(o_ptr, lookup_kind(TV_AXE,SV_WEAPON_MASAKARI));
		else if(one_in_(4))	object_prep(o_ptr, lookup_kind(TV_SPEAR,SV_WEAPON_HEAVY_LANCE));
		else if(one_in_(3))	object_prep(o_ptr, lookup_kind(TV_POLEARM,SV_WEAPON_SCYTHE_OF_SLICING));
		else if(one_in_(2))	object_prep(o_ptr, lookup_kind(TV_BOW,SV_FIRE_LONG_BOW));
		else				object_prep(o_ptr, lookup_kind(TV_CROSSBOW,SV_FIRE_HEAVY_CROSSBOW));
		apply_magic(o_ptr, (dun_level + randint1(20)), AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
	}
	else if(power < 90) //高級素材・酒
	{
		if(one_in_(10) && weird_luck())
			object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_HIHIIROKANE));
		else if(one_in_(11))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT));
		else if(one_in_(10))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_METEORICIRON));
		else if(one_in_(9)) object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_DRAGONNAIL));
		else if(one_in_(8))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_DIAMOND));
		else if(one_in_(7))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_EMERALD));
		else if(one_in_(6))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_RUBY));
		else if(one_in_(5))	object_prep(o_ptr, lookup_kind(TV_MATERIAL,SV_MATERIAL_SAPPHIRE));
		else if(one_in_(3))	object_prep(o_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_GOLDEN_MEAD));
		else if(one_in_(2))	object_prep(o_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_NECTAR));
		else				object_prep(o_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_TRUE_ONIKOROSHI));
	}

	else if(power < 100) //四冊目
	{
		object_prep(o_ptr, lookup_kind(randint1(13),3));
	}
	else //ランダムアーティファクト
	{
		do
		{
			do
			{
				object_prep(o_ptr, get_obj_num(dun_level));
			}while(o_ptr->tval != TV_RIBBON	&& o_ptr->tval != TV_RING && o_ptr->tval != TV_AMULET && !object_is_melee_weapon(o_ptr) && !object_is_armour(o_ptr));
			create_artifact(o_ptr, FALSE);
		}while(object_is_cursed(o_ptr));
	}

	while (dummy < SAFE_MAX_ATTEMPTS)
	{
		dummy++;
		y = randint0(cur_hgt);
		x = randint0(cur_wid);
		c_ptr = &cave[y][x];
		if (!cave_drop_bold(y, x) || c_ptr->o_idx || c_ptr->m_idx ||!in_bounds(y, x)) continue;
		if (!cave_empty_bold(y, x)) continue;
		if (player_bold(y, x)) continue;
		break;
	}

	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_peek) msg_print(_("WARNING:霊夢の占いのお宝配置不可",
                                    "WARNING: Failed to place treasure for Reimu's divination"));
		return;
	}
	drop_near(o_ptr,-1,y,x);
	if (cheat_peek) msg_format(_("お宝生成:power(%d)",
                                "Treasure generated: power(%d)"),power);

	//生成後、変数クリア(today_monは日付をまたぐまでクリアしない)
	p_ptr->barter_value = 0;

}


/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(int set, int typ, int num)
{
	int y = 0, x = 0, k;
	int dummy = 0;
	cave_type *c_ptr;

	/* A small level has few objects. */
	num = num * cur_hgt * cur_wid / (MAX_HGT*MAX_WID) + 1;

	//v1.1.61 EXTRAモードでは狭い部屋でもアイテムが減りにくい
	//(v1.1.61以降フロアサイズが常に2*2以下になっているのでそのままでは4/9以下になるのでこの補正
	if (EXTRA_MODE) num *= 2;

	/* Place some objects */
	for (k = 0; k < num; k++)
	{
		/* Pick a "legal" spot */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			bool room;

			dummy++;

			/* Location */
			y = randint0(cur_hgt);
			x = randint0(cur_wid);

			c_ptr = &cave[y][x];

			/* Require "naked" floor grid */
			if (!is_floor_grid(c_ptr) || c_ptr->o_idx || c_ptr->m_idx) continue;

			/* Avoid player location */
			if (player_bold(y, x)) continue;

			/* Check for "room" */
			room = (cave[y][x].info & CAVE_ROOM) ? TRUE : FALSE;

			/* Require corridor? */
			if ((set == ALLOC_SET_CORR) && room) continue;

			/* Require room? */
			if ((set == ALLOC_SET_ROOM) && !room) continue;

			/* Accept it */
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
#ifdef JP
msg_print("警告！アイテムを配置できません！");
#else
				msg_print("Warning! Could not place object!");
#endif

			}
			return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
			{
				place_rubble(y, x);
				cave[y][x].info &= ~(CAVE_FLOOR);
				break;
			}

			case ALLOC_TYP_TRAP:
			{
				place_trap(y, x);
				cave[y][x].info &= ~(CAVE_FLOOR);
				break;
			}

			case ALLOC_TYP_GOLD:
			{
				place_gold(y, x);
				break;
			}

			case ALLOC_TYP_OBJECT:
			{
				place_object(y, x, 0L);
				break;
			}
		}
	}
}


/*
 * Count the number of "corridor" grids adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds(y1, x1)"
 *
 * XXX XXX This routine currently only counts actual "empty floor"
 * grids which are not in rooms.  We might want to also count stairs,
 * open doors, closed doors, etc.
 */
static int next_to_corr(int y1, int x1)
{
	int i, y, x, k = 0;

	cave_type *c_ptr;

	/* Scan adjacent grids */
	for (i = 0; i < 4; i++)
	{
		/* Extract the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Skip non floors */
		if (cave_have_flag_grid(c_ptr, FF_WALL)) continue;

		/* Skip non "empty floor" grids */
		if (!is_floor_grid(c_ptr))
			continue;

		/* Skip grids inside rooms */
		if (c_ptr->info & (CAVE_ROOM)) continue;

		/* Count these grids */
		k++;
	}

	/* Return the number of corridors */
	return (k);
}


/*
 * Determine if the given location is "between" two walls,
 * and "next to" two corridor spaces.  XXX XXX XXX
 *
 * Assumes "in_bounds(y, x)"
 */
static bool possible_doorway(int y, int x)
{
	/* Count the adjacent corridors */
	if (next_to_corr(y, x) >= 2)
	{
		/* Check Vertical */
		if (cave_have_flag_bold(y - 1, x, FF_WALL) &&
		    cave_have_flag_bold(y + 1, x, FF_WALL))
		{
			return (TRUE);
		}

		/* Check Horizontal */
		if (cave_have_flag_bold(y, x - 1, FF_WALL) &&
		    cave_have_flag_bold(y, x + 1, FF_WALL))
		{
			return (TRUE);
		}
	}

	/* No doorway */
	return (FALSE);
}


/*
 * Places door at y, x position if at least 2 walls found
 */
static void try_door(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Ignore walls */
	if (cave_have_flag_bold(y, x, FF_WALL)) return;

	/* Ignore room grids */
	if (cave[y][x].info & (CAVE_ROOM)) return;

	/* Occasional door (if allowed) */
	if ((randint0(100) < dun_tun_jct) && possible_doorway(y, x) && !(d_info[dungeon_type].flags1 & DF1_NO_DOORS))
	{
		/* Place a door */
		place_random_door(y, x, FALSE);
	}
}


/* Place quest monsters */
/*:::ランダムダンジョンのクエスト用モンスター配置*/
///quest
bool place_quest_monsters(void)
{
	int i;

	/* Handle the quest monster placements */
	for (i = 0; i < max_quests; i++)
	{
		monster_race *r_ptr;
		u32b mode;
		int j;

		if (quest[i].status != QUEST_STATUS_TAKEN ||
		    (quest[i].type != QUEST_TYPE_KILL_LEVEL &&
		     quest[i].type != QUEST_TYPE_RANDOM) ||
		    quest[i].level != dun_level ||
		    dungeon_type != quest[i].dungeon ||
		    (quest[i].flags & QUEST_FLAG_PRESET))
		{
			/* Ignore it */
			continue;
		}

		r_ptr = &r_info[quest[i].r_idx];

		/* Hack -- "unique" monsters must be "unique" */
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (r_ptr->cur_num >= r_ptr->max_num)) continue;

		mode = (PM_NO_KAGE | PM_NO_PET);

		if (!(r_ptr->flags1 & RF1_FRIENDS))
			mode |= PM_ALLOW_GROUP;

		for (j = 0; j < (quest[i].max_num - quest[i].cur_num); j++)
		{
			int k;

			for (k = 0; k < SAFE_MAX_ATTEMPTS; k++)
			{
				int x, y;
				int l;

				/* Find an empty grid */
				for (l = SAFE_MAX_ATTEMPTS; l > 0; l--)
				{
					cave_type    *c_ptr;
					feature_type *f_ptr;

					y = randint0(cur_hgt);
					x = randint0(cur_wid);

					c_ptr = &cave[y][x];
					f_ptr = &f_info[c_ptr->feat];

					if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) continue;
					if (!monster_can_enter(y, x, r_ptr, 0)) continue;
					if (distance(y, x, py, px) < 10) continue;
					if (c_ptr->info & CAVE_ICKY) continue;
					else break;
				}

				/* Failed to place */
				if (!l) return FALSE;

				/* Try to place the monster */
				if (place_monster_aux(0, y, x, quest[i].r_idx, mode))
				{
					/* Success */
					break;
				}
				else
				{
					/* Failure - Try again */
					continue;
				}
			}

			/* Failed to place */
			if (k == SAFE_MAX_ATTEMPTS) return FALSE;
		}
	}

	return TRUE;
}


/*
 * Set boundary mimic and add "solid" perma-wall
 */
static void set_bound_perm_wall(cave_type *c_ptr)
{
	if (bound_walls_perm)
	{
		/* Clear boundary mimic */
		c_ptr->mimic = 0;
	}
	else
	{
		feature_type *f_ptr = &f_info[c_ptr->feat];

		/* Hack -- Decline boundary walls with known treasure  */
		if ((have_flag(f_ptr->flags, FF_HAS_GOLD) || have_flag(f_ptr->flags, FF_HAS_ITEM)) &&
		    !have_flag(f_ptr->flags, FF_SECRET))
			c_ptr->feat = feat_state(c_ptr->feat, FF_ENSECRET);

		/* Set boundary mimic */
		c_ptr->mimic = c_ptr->feat;
	}

	/* Add "solid" perma-wall */
	place_solid_perm_grid(c_ptr);
}


//LAKEタイプ(LAKE_T_XXXXXXXX)の新地形を強引に決める関数
//flag_vault:elemental_vault()から呼ばれるときにTRUE LAKE_T_*_VAULTを返す
//この関数のあとで数値が決まってなかったら既存のタイプ決定ルーチンに行くので、ここで全く0を返さないようにすると新地形しか出なくなる
int apply_new_lake_type(bool flag_vault)
{
	int lake_type = 0;

	int chance = randint0(120);

	switch (dungeon_type)
	{

	case DUNGEON_FOREST:
		//魔法の森 奥に行くと魔法吹雪や毒沼がよく出る
		if (dun_level > 25)
		{
			if (chance < 60) lake_type = LAKE_T_COLD;
			else if (chance < 80) lake_type = LAKE_T_POIS;
		}
		break;

	case DUNGEON_ZIGOKUDANI:
	case DUNGEON_HELL:
		//地獄谷と地獄 酸と毒
		if (chance < 40) lake_type = LAKE_T_ACID;
		else if (chance < 80) lake_type = LAKE_T_POIS;
		break;


	default:
		//鉄獄と龍窟と混沌の領域、もしくは上で指定してないダンジョンのelemental_vault:
		//2/3の確率で既存、残りを新地形4つで分け合う
		if (dun_level > 50)
		{
			if (chance < 10) lake_type = LAKE_T_ACID;
			else if (chance < 20) lake_type = LAKE_T_POIS;
			else if (chance < 30) lake_type = LAKE_T_COLD;
			else if (chance < 40) lake_type = LAKE_T_ELEC;
		}
		break;

	}


	if (flag_vault)
	{
		if (lake_type == LAKE_T_ACID) lake_type = LAKE_T_ACID_VAULT;
		if (lake_type == LAKE_T_POIS) lake_type = LAKE_T_POIS_VAULT;
		if (lake_type == LAKE_T_COLD) lake_type = LAKE_T_COLD_VAULT;
		if (lake_type == LAKE_T_ELEC) lake_type = LAKE_T_ELEC_VAULT;

	}


	if (cheat_room) msg_format("new_lake_type:%d",lake_type);

	return lake_type;
}

/*
 * Generate various caverns and lakes
 *
 * There were moved from cave_gen().
 */
/*:::ランダムダンジョン内の特殊地形　大きな洞窟や湖*/
static void gen_caverns_and_lakes(void)
{
#ifdef ALLOW_CAVERNS_AND_LAKES
	/* Possible "destroyed" level */
	if ((dun_level > 30) && one_in_(DUN_DEST*2) && (small_levels) && (d_info[dungeon_type].flags1 & DF1_DESTROY))
	{
		dun->destroyed = TRUE;

		/* extra rubble around the place looks cool */
		build_lake(one_in_(2) ? LAKE_T_CAVE : LAKE_T_EARTH_VAULT);
	}

	/* Make a lake some of the time */
	if (one_in_(LAKE_LEVEL) && !dun->empty_level && !dun->destroyed &&
	    (d_info[dungeon_type].flags1 & DF1_LAKE_MASK))
	{
		int count = 0;
		//すべての種類のLAKEを均等の確率で配置するための乱数制御用カウントと思われる。
		//ここで足されるcountと下の判定部で引かれるcountの合計が地形ごとに一緒になること。
		if (d_info[dungeon_type].flags1 & DF1_LAKE_WATER) count += 3;
		if (d_info[dungeon_type].flags1 & DF1_LAKE_LAVA) count += 3;
		if (d_info[dungeon_type].flags1 & DF1_LAKE_RUBBLE) count += 3;
		if (d_info[dungeon_type].flags1 & DF1_LAKE_TREE) count += 3;

		//v1.1.85 HACK - 新地形のLAKE追加　ここでやってるランダム選定ルーチンの上に強引にlaketype選定を差し込む
		if (d_info[dungeon_type].flags1 & DF1_NEW_LAKE) dun->laketype = apply_new_lake_type(FALSE);



		if (d_info[dungeon_type].flags1 & DF1_LAKE_LAVA && !dun->laketype)
		{
			/* Lake of Lava */
			if ((dun_level > 80) && (randint0(count) < 2)) dun->laketype = LAKE_T_LAVA;
			count -= 2;

			/* Lake of Lava2 */
			if (!dun->laketype && (dun_level > 80) && one_in_(count)) dun->laketype = LAKE_T_FIRE_VAULT;
			count--;
		}

		if ((d_info[dungeon_type].flags1 & DF1_LAKE_WATER) && !dun->laketype)
		{
			/* Lake of Water */
			if ((dun_level > 50) && randint0(count) < 2) dun->laketype = LAKE_T_WATER;
			count -= 2;

			/* Lake of Water2 */
			if (!dun->laketype && (dun_level > 50) && one_in_(count)) dun->laketype = LAKE_T_WATER_VAULT;
			count--;
		}

		if ((d_info[dungeon_type].flags1 & DF1_LAKE_RUBBLE) && !dun->laketype)
		{
			/* Lake of rubble */
			if ((dun_level > 35) && (randint0(count) < 2)) dun->laketype = LAKE_T_CAVE;
			count -= 2;

			/* Lake of rubble2 */
			if (!dun->laketype && (dun_level > 35) && one_in_(count)) dun->laketype = LAKE_T_EARTH_VAULT;
			count--;
		}

		/* Lake of tree */
		if ((dun_level > 5) && (d_info[dungeon_type].flags1 & DF1_LAKE_TREE) && !dun->laketype) dun->laketype = LAKE_T_AIR_VAULT;

		if (dun->laketype)
		{
			if (cheat_room)
#ifdef JP
				msg_print("湖を生成。");
#else
				msg_print("Lake on the level.");
#endif

			build_lake(dun->laketype);
		}
	}

	if ((dun_level > DUN_CAVERN) && !dun->empty_level &&
	    (d_info[dungeon_type].flags1 & DF1_CAVERN) &&
	    !dun->laketype && !dun->destroyed && (randint1(1000) < dun_level))
	{
		dun->cavern = TRUE;

		/* make a large fractal cave in the middle of the dungeon */

		if (cheat_room)
#ifdef JP
			msg_print("洞窟を生成。");
#else
			msg_print("Cavern on level.");
#endif

		build_cavern();
	}
#endif /* ALLOW_CAVERNS_AND_LAKES */

	/* Hack -- No destroyed "quest" levels */
	/*:::なんでこんな後ろのほうにこの処理がある？すでに処理が済んでしまってると思うが*/
	if (quest_number(dun_level)) dun->destroyed = FALSE;
}



/*
 * Generate a new dungeon level
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
/*:::ランダムダンジョン生成の実体部分*/
static bool cave_gen(void)
{
	int i, k, y, x;

	dun_data dun_body;

	/* Global data */
	dun = &dun_body;

	dun->destroyed = FALSE;
	dun->empty_level = FALSE;
	dun->cavern = FALSE;
	dun->laketype = 0;

	/* Fill the arrays of floors and walls in the good proportions */
	/*:::floor_type[]とfill_type[]を設定*/
	set_floor_and_wall(dungeon_type);

	/* Prepare allocation table */
	/*:::出現モンスターを設定*/
	get_mon_num_prep(get_monster_hook(), NULL);

	/* Randomize the dungeon creation values */
	dun_tun_rnd = rand_range(DUN_TUN_RND_MIN, DUN_TUN_RND_MAX);
	dun_tun_chg = rand_range(DUN_TUN_CHG_MIN, DUN_TUN_CHG_MAX);
	dun_tun_con = rand_range(DUN_TUN_CON_MIN, DUN_TUN_CON_MAX);
	dun_tun_pen = rand_range(DUN_TUN_PEN_MIN, DUN_TUN_PEN_MAX);
	dun_tun_jct = rand_range(DUN_TUN_JCT_MIN, DUN_TUN_JCT_MAX);

	/* Actual maximum number of rooms on this level */
	/*:::ダンジョンのサイズに合わせて部屋最大数を決定*/
	dun->row_rooms = cur_hgt / BLOCK_HGT;
	dun->col_rooms = cur_wid / BLOCK_WID;

	/* Initialize the room table */
	for (y = 0; y < dun->row_rooms; y++)
	{
		for (x = 0; x < dun->col_rooms; x++)
		{
			dun->room_map[y][x] = FALSE;
		}
	}

	/* No rooms yet */
	dun->cent_n = 0;

	/* Empty arena levels */
	if (ironman_empty_levels || ((d_info[dungeon_type].flags1 & DF1_ARENA) && (empty_levels && one_in_(EMPTY_LEVEL))))
	{
		dun->empty_level = TRUE;

		if (cheat_room)
#ifdef JP
			msg_print("アリーナレベル");
#else
			msg_print("Arena level.");
#endif
	}
	/*:::最初に全てのマスを壁（アリーナフロアは床）で埋める*/
	if (dun->empty_level)
	{
		/* Start with floors */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/*:::指定位置に床を配置する*/
				place_floor_bold(y, x);
			}
		}

		/* Special boundary walls -- Top and bottom */
		for (x = 0; x < cur_wid; x++)
		{
			/*:::指定位置に壁を配置する*/
			place_extra_bold(0, x);
			place_extra_bold(cur_hgt - 1, x);
		}

		/* Special boundary walls -- Left and right */
		for (y = 1; y < (cur_hgt - 1); y++)
		{
			place_extra_bold(y, 0);
			place_extra_bold(y, cur_wid - 1);
		}
	}
	///mod150906 夢の世界
	else if(d_info[dungeon_type].flags1 & DF1_DREAM_WORLD)
	{
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				if(!(x % 10) && (y % 10) || (x % 10) && !(y % 10))
				{
					place_extra_bold(y, x);
				}
				else
				{
					place_floor_bold(y, x);
				}
			}
		}
		/* Special boundary walls -- Top and bottom */
		for (x = 0; x < cur_wid; x++)
		{
			/*:::指定位置に壁を配置する*/
			place_extra_bold(0, x);
			place_extra_bold(cur_hgt - 1, x);
		}

		/* Special boundary walls -- Left and right */
		for (y = 1; y < (cur_hgt - 1); y++)
		{
			place_extra_bold(y, 0);
			place_extra_bold(y, cur_wid - 1);
		}
	}
	else
	{
		/* Start with walls */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				place_extra_bold(y, x);
			}
		}
	}


	/* Generate various caverns and lakes */
	/*:::確率で湖や破壊後地形？などを生成*/
	gen_caverns_and_lakes();


	/* Build maze */
	/*:::ダンジョン「迷宮」用処理*/
	if (d_info[dungeon_type].flags1 & DF1_MAZE)
	{
		build_maze_vault(cur_wid/2-1, cur_hgt/2-1, cur_wid-4, cur_hgt-4, FALSE);

		/* Place 3 or 4 down stairs near some walls */
		if (!alloc_stairs(feat_down_stair, rand_range(2, 3), 3)) return FALSE;

		/* Place 1 or 2 up stairs near some walls */
		if (!alloc_stairs(feat_up_stair, 1, 3)) return FALSE;
	}

	/* Build some rooms */
	else
	{
		int tunnel_fail_count = 0;

		/*
		 * Build each type of room in turn until we cannot build any more.
		 */
		if (!generate_rooms()) return FALSE;


		/* Make a hole in the dungeon roof sometimes at level 1 */
		/*:::地下一階では登り階段の近くに木ができやすい処理らしい*/
		if (dun_level == 1)
		{
			while (one_in_(DUN_MOS_DEN))
			{
				place_trees(randint1(cur_wid - 2), randint1(cur_hgt - 2));
			}
		}

		/* Destroy the level if necessary */
		/*:::地形を破壊する cavernとは別物らしい*/
		if (dun->destroyed) destroy_level();

		/* Hack -- Add some rivers */
		/*:::川か溶岩流追加処理*/
		if (one_in_(3) && (randint1(dun_level) > 5))
		{
			int feat1 = 0, feat2 = 0;

			//v1.1.85 新地形の川決定部
			if (d_info[dungeon_type].flags1 & DF1_NEW_RIVER)
			{
				//LAKE作成時と同じ判定を使うことにした
				int river_type = apply_new_lake_type(FALSE);

				switch (river_type)
				{
				case 0:
					break;
				case LAKE_T_ACID:
					feat1 = feat_deep_acid_puddle;
					feat2 = feat_acid_puddle;
					break;
				case LAKE_T_POIS:
					feat1 = feat_deep_poisonous_puddle;
					feat2 = feat_poisonous_puddle;
					break;
				case LAKE_T_COLD:
					feat1 = feat_heavy_cold_zone;
					feat2 = feat_cold_zone;
					break;
				case LAKE_T_ELEC:
					feat1 = feat_heavy_electrical_zone;
					feat2 = feat_electrical_zone;
					break;
				default:
					msg_format(_("ERROR:cave_gen()の川地形判定処理で未定義の値が使われた(%d)",
                                "ERROR: Undefined value (%d) used in streamer logic in cave_gen()"),river_type);

				}
				if (feat1 && cheat_room) msg_print(_("(新地形の川)", "(new terrain streamer)"));
			}

			//既存の川地形決定部
			if (!feat1)
			{
				/* Choose water or lava */
				if ((randint1(MAX_DEPTH * 2) - 1 > dun_level) && (d_info[dungeon_type].flags1 & DF1_WATER_RIVER))
				{
					feat1 = feat_deep_water;
					feat2 = feat_shallow_water;
				}
				else if (d_info[dungeon_type].flags1 & DF1_LAVA_RIVER)
				{
					feat1 = feat_deep_lava;
					feat2 = feat_shallow_lava;
				}
				else feat1 = 0;
			}


			if (feat1)
			{
				feature_type *f_ptr = &f_info[feat1];

				/* Only add river if matches lake type or if have no lake at all */
				if (((dun->laketype == LAKE_T_LAVA) && have_flag(f_ptr->flags, FF_LAVA)) ||
				    ((dun->laketype == LAKE_T_WATER) && have_flag(f_ptr->flags, FF_WATER)) ||
				     !dun->laketype)
				{
					add_river(feat1, feat2);
				}
			}
		}


		/* Hack -- Scramble the room order */
		/*:::部屋をシャッフル・・？*/
		for (i = 0; i < dun->cent_n; i++)
		{
			int ty, tx;
			int pick = rand_range(0, i);

			ty = dun->cent[i].y;
			tx = dun->cent[i].x;
			dun->cent[i].y = dun->cent[pick].y;
			dun->cent[i].x = dun->cent[pick].x;
			dun->cent[pick].y = ty;
			dun->cent[pick].x = tx;
		}


		/* Start with no tunnel doors */
		dun->door_n = 0;

		/*:::*/
		/* Hack -- connect the first room to the last room */
		y = dun->cent[dun->cent_n-1].y;
		x = dun->cent[dun->cent_n-1].x;

		/* Connect all the rooms together */
		/*:::部屋同士を繋いでいるらしい*/
		for (i = 0; i < dun->cent_n; i++)
		{
			int j;

			/* Reset the arrays */
			dun->tunn_n = 0;
			dun->wall_n = 0;

			/* Connect the room to the previous room */
			if (randint1(dun_level) > d_info[dungeon_type].tunnel_percent)
			{
				/* make cave-like tunnel */
				(void)build_tunnel2(dun->cent[i].x, dun->cent[i].y, x, y, 2, 2);
			}
			else
			{
				/* make normal tunnel */
				if (!build_tunnel(dun->cent[i].y, dun->cent[i].x, y, x)) tunnel_fail_count++;
			}

			if (tunnel_fail_count >= 2) return FALSE;

			/* Turn the tunnel into corridor */
			for (j = 0; j < dun->tunn_n; j++)
			{
				cave_type *c_ptr;
				feature_type *f_ptr;

				/* Access the grid */
				y = dun->tunn[j].y;
				x = dun->tunn[j].x;

				/* Access the grid */
				c_ptr = &cave[y][x];
				f_ptr = &f_info[c_ptr->feat];

				/* Clear previous contents (if not a lake), add a floor */
				///mod160318 EXモードでは湖などに優先して通路ができるようにしてみる
				if (!have_flag(f_ptr->flags, FF_MOVE) || EXTRA_MODE || (!have_flag(f_ptr->flags, FF_WATER) && !have_flag(f_ptr->flags, FF_LAVA)))
				{
					/* Clear mimic type */
					c_ptr->mimic = 0;

					place_floor_grid(c_ptr);
				}
			}

			/* Apply the piercings that we found */
			for (j = 0; j < dun->wall_n; j++)
			{
				cave_type *c_ptr;

				/* Access the grid */
				y = dun->wall[j].y;
				x = dun->wall[j].x;

				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Clear mimic type */
				c_ptr->mimic = 0;

				/* Clear previous contents, add up floor */
				place_floor_grid(c_ptr);

				/* Occasional doorway */
				if ((randint0(100) < dun_tun_pen) && !(d_info[dungeon_type].flags1 & DF1_NO_DOORS))
				{
					/* Place a random door */
					place_random_door(y, x, TRUE);
				}
			}

			/* Remember the "previous" room */
			y = dun->cent[i].y;
			x = dun->cent[i].x;
		}

		/* Place intersection doors */
		/*:::交差点にドア設置？*/
		for (i = 0; i < dun->door_n; i++)
		{
			/* Extract junction location */
			y = dun->door[i].y;
			x = dun->door[i].x;

			/* Try placing doors */
			try_door(y, x - 1);
			try_door(y, x + 1);
			try_door(y - 1, x);
			try_door(y + 1, x);
		}

		/*:::階段設置*/
		/* Place 3 or 4 down stairs near some walls */
		if (!alloc_stairs(feat_down_stair, rand_range(3, 4), 3)) return FALSE;

		/* Place 1 or 2 up stairs near some walls */
		if (!alloc_stairs(feat_up_stair, rand_range(1, 2), 3)) return FALSE;
	}



	//川の生成は上のほうのadd_riverで行う。ここは鉱脈とかを生成する部分らしい
	if (!dun->laketype)
	{
		if (d_info[dungeon_type].stream2)
		{
			/* Hack -- Add some quartz streamers */
			for (i = 0; i < DUN_STR_QUA; i++)
			{
				build_streamer(d_info[dungeon_type].stream2, DUN_STR_QC);
			}
		}

		if (d_info[dungeon_type].stream1)
		{
			/* Hack -- Add some magma streamers */
			for (i = 0; i < DUN_STR_MAG; i++)
			{
				build_streamer(d_info[dungeon_type].stream1, DUN_STR_MC);
			}
		}
	}

	/*:::外周への永久壁配置*/
	/* Special boundary walls -- Top and bottom */
	for (x = 0; x < cur_wid; x++)
	{
		set_bound_perm_wall(&cave[0][x]);
		set_bound_perm_wall(&cave[cur_hgt - 1][x]);
	}

	/* Special boundary walls -- Left and right */
	for (y = 1; y < (cur_hgt - 1); y++)
	{
		set_bound_perm_wall(&cave[y][0]);
		set_bound_perm_wall(&cave[y][cur_wid - 1]);
	}


	/* Determine the character location */
	/*:::＠配置*/
	if (!new_player_spot()) return FALSE;

	/*:::ワーグクエ・ランクエ・オベロン配置？*/
	if (!place_quest_monsters()) return FALSE;

	/* Basic "amount" */
	/*:::配置モンスター数計算*/
	k = (dun_level / 3);
	if (k > 10) k = 10;
	if (k < 2) k = 2;

	/* Pick a base number of monsters */
	i = d_info[dungeon_type].min_m_alloc_level;

	/* To make small levels a bit more playable */
	if (cur_hgt < MAX_HGT || cur_wid < MAX_WID)
	{
		int small_tester = i;

		i = (i * cur_hgt) / MAX_HGT;
		i = (i * cur_wid) / MAX_WID;
		i += 1;

		if (i > small_tester) i = small_tester;
		else if (cheat_hear)
		{
#ifdef JP
msg_format("モンスター数基本値を %d から %d に減らします", small_tester, i);
#else
			msg_format("Reduced monsters base from %d to %d", small_tester, i);
#endif

		}
	}

	i += randint1(8);

	/* Put some monsters in the dungeon */
/*:::モンスター配置*/
	for (i = i + k; i > 0; i--)
	{
		(void)alloc_monster(0, PM_ALLOW_SLEEP);
	}

	/* Place some traps in the dungeon */
/*:::トラップ配置*/
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint1(k));

	/* Put some rubble in corridors (except NO_CAVE dungeon (Castle)) */
/*:::岩配置*/
	if (!(d_info[dungeon_type].flags1 & DF1_NO_CAVE)) alloc_object(ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint1(k));

/*:::アイテム配置(vault除く)*/
	/* Mega Hack -- No object at first level of deeper dungeon */
	if (p_ptr->enter_dungeon && dun_level > 1)
	{
		/* No stair scum! */
		object_level = 1;
	}

	/* Put some objects in rooms */
	alloc_object(ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ROOM, 3));

	/* Put some objects/gold in the dungeon */
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ITEM, 3));
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_GOLD, randnor(DUN_AMT_GOLD, 3));


	///mod140105 群生した特産品を配置
	alloc_souvenir();
	alloc_special_item();
	///mod141231 霊夢の占いで宝が出ると言われた時の宝生成
	alloc_reimu_treasure();

	/* Set back to default */
	object_level = base_level;

	/* Put the Guardian */
	/*:::ダンジョンの主配置*/
	if (!alloc_guardian(TRUE)) return FALSE;

	/*:::確率で全てのマスを明るくする(GLOW)*/
	if (dun->empty_level && (!one_in_(DARK_EMPTY) || (randint1(100) > dun_level)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS))
	{
		/* Lite the cave */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave[y][x].info |= (CAVE_GLOW);
			}
		}
	}

	return TRUE;
}


/*
 * Builds the arena after it is entered -KMW-
 */
/*:::アリーナダンジョン生成の続き　基本地形を置いた後で呼ばれる　ゲートや＠の配置など？*/
static void build_arena(void)
{
	int yval, y_height, y_depth, xval, x_left, x_right;
	register int i, j;

	yval = SCREEN_HGT / 2;
	xval = SCREEN_WID / 2;
	y_height = yval - 10;
	y_depth = yval + 10;
	x_left = xval - 32;
	x_right = xval + 32;

	for (i = y_height; i <= y_height + 5; i++)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (i = y_depth; i >= y_depth - 5; i--)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_left; j <= x_left + 17; j++)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_right; j >= x_right - 17; j--)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}

	place_extra_perm_bold(y_height+6, x_left+18);
	cave[y_height+6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_depth-6, x_left+18);
	cave[y_depth-6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_height+6, x_right-18);
	cave[y_height+6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_depth-6, x_right-18);
	cave[y_depth-6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);

	i = y_height + 5;
	j = xval;
	cave[i][j].feat = f_tag_to_index("ARENA_GATE");
	cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
	player_place(i, j);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
/*:::アリーナダンジョン生成*/
//v1.1.56 無効化
#if 0
static void arena_gen(void)
{
	int y, x;
	int qy = 0;
	int qx = 0;

	/* Smallest area */
	cur_hgt = SCREEN_HGT;
	cur_wid = SCREEN_WID;

	/* Start with solid walls */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create "solid" perma-wall */
			place_solid_perm_bold(y, x);

			/* Illuminate and memorize the walls */
			cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Then place some floors */
	for (y = qy + 1; y < qy + SCREEN_HGT - 1; y++)
	{
		for (x = qx + 1; x < qx + SCREEN_WID - 1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = feat_floor;
		}
	}

	build_arena();

	if(!place_monster_aux(0, py + 5, px, arena_info[p_ptr->arena_number].r_idx, (PM_NO_KAGE | PM_NO_PET)))
	{
		p_ptr->exit_bldg = TRUE;
		p_ptr->arena_number++;
#ifdef JP
		msg_print("相手は欠場した。あなたの不戦勝だ。");
#else
		msg_print("The enemy is unable appear. You won by default.");
#endif
	}

}
#endif


//v1.1.51 新アリーナ	nightmare_gen()から呼ばれる
//適当な街ファイルを開き、指定座標から一定範囲を読み取って新アリーナマップの地形にする
//失敗したらFALSEを返す
bool	process_dun_file_to_nightmare(void)
{
	FILE	*fp;
	char buf[1024];
	bool flag_ok = TRUE;

	int lines = 0;

	//街ファイルと流用開始位置を予め決めた組み合わせからランダムに選ぶ
	int map_idx = randint0(NIGHTMARE_GEN_MAP_TABLE_SIZE);

	path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, nightmare_gen_map_table[map_idx].townfilename);

	fp = my_fopen(buf, "r");
	if (!fp)
	{
		msg_format(_("ファイル %s のオープンに失敗した",
                    "Failed to open file %s."), buf);
		return FALSE;
	}

	if (cheat_room) msg_format("map:%s(X%d,Y%d)", nightmare_gen_map_table[map_idx].townfilename, nightmare_gen_map_table[map_idx].x, nightmare_gen_map_table[map_idx].y);

	//t00000xx.txtを一行ずつ読む
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		int i;
		char *s;


		//空行と#コメントをパス
		if (!buf[0]) continue;
		if (buf[0] == '#') continue;

		//D:で始まる行以外は全て無視する。
		//もし将来D:行に対して?:行による条件分岐を作った場合、ここで対処しないと新アリーナのマップが歪む。
		//まあ放っておいてもフリーズなど深刻なバグには繋がらないだろうけど
		if (buf[0] != 'D' || buf[1] != ':') continue;

		lines++;

		//指定された行に来るまでパス
		if (lines < nightmare_gen_map_table[map_idx].y) continue;
		//決まった行数処理し終えたら終了
		if ((lines - nightmare_gen_map_table[map_idx].y) >= (SCREEN_HGT-2)) break;


		s = buf + 2; //アドレスをD:の次の文字に合わせる
		if (strlen(s) <= (unsigned int)nightmare_gen_map_table[map_idx].x + SCREEN_WID-2)
		{
			msg_format(_("ERROR:map_idx(%d)の指定x座標値が大きすぎる",
                        "ERROR: Specified x coordinate in map_idx (%d) is too large"), map_idx);
			flag_ok = FALSE;
			break;
		}

		//ここからD:行を一文字ずつ処理
		for (i = 0; i < SCREEN_WID-2; i++)
		{
			char c;
			int x, y;

			c = buf[3 + nightmare_gen_map_table[map_idx].x + i];
			x = 1 + i;
			y = 1 + lines - nightmare_gen_map_table[map_idx].y;

		//	if (lines == nightmare_gen_map_table[map_idx].y+ 5) msg_format("%d,%d %c",y,x, c);

			//地形配置。基本的地形以外の文字は全て床とする
			switch (c)
			{
			case '^':
				cave_set_feat(y, x, feat_mountain);
				break;
			case 'T':
				cave_set_feat(y, x, feat_tree);
				break;
			case 'W':
				cave_set_feat(y, x, feat_deep_water);
				break;
			case 'V':
				cave_set_feat(y, x, feat_shallow_water);
				break;
			case 'L':
				cave_set_feat(y, x, feat_deep_lava);
				break;
			case 'K':
				cave_set_feat(y, x, feat_shallow_lava);
				break;
			case 'C':
				cave_set_feat(y, x, feat_dark_pit);
				break;
			case ',':
				cave_set_feat(y, x, feat_dirt);
				break;
			case ';':
				place_rubble(y, x);
				break;
			case '-':
				cave_set_feat(y, x, feat_grass);
				break;
			case '=':
				cave_set_feat(y, x, feat_swamp);
				break;
			case '#':
				place_extra_perm_bold(y, x);
				break;
			default:
				cave[y][x].feat = feat_floor;
				break;
			}
		}
	}

	{
		int try_count, i, j;
		for (try_count = 500; try_count; try_count--)
		{
			i = randint1(SCREEN_HGT - 2) + 1;
			j = randint1(SCREEN_WID - 2) + 1;
			if (!in_bounds(i, j)) continue;
			if (!cave_empty_bold(i, j)) continue;
			if (!cave_have_flag_bold(i, j, FF_MOVE)) continue;

			//適当な位置を選定して＠と階段を置く
			cave[i][j].feat = f_tag_to_index("ARENA_GATE");
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
			player_place(i, j);

			break;
		}
	}


	my_fclose(fp);
	return flag_ok;

}


/*:::v1.1.51 新アリーナダンジョン生成*/
static void nightmare_gen(void)
{
	int y, x;
	int qy = 0;
	int qx = 0;
	int i;

	/* Smallest area */
	cur_hgt = SCREEN_HGT;
	cur_wid = SCREEN_WID;

	/* Start with solid walls */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create "solid" perma-wall */
			place_solid_perm_bold(y, x);

			/* Illuminate and memorize the walls */
			cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Then place some floors */
	for (y = qy + 1; y < qy + SCREEN_HGT - 1; y++)
	{
		for (x = qx + 1; x < qx + SCREEN_WID - 1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = feat_floor;
		}
	}

	if (!process_dun_file_to_nightmare())
	{
		build_arena();
	}

	//モンスターをランダム配置
	for (i = 0; i < NIGHTMARE_DIARY_MONSTER_MAX; i++)
	{
		int tmp_x, tmp_y, try_count;

		if (!nightmare_mon_r_idx[i]) break;

		for (try_count = 300; try_count; try_count--)
		{
			tmp_y = randint1(SCREEN_HGT - 2)+1;
			tmp_x = randint1(SCREEN_WID - 2)+1;
			if (!in_bounds(tmp_y, tmp_x)) continue;
			if (!cave_empty_bold(tmp_y, tmp_x)) continue;

			break;
		}
		if (!try_count)
		{
			msg_print(_("ERROR:nightmare_gen()にてモンスター配置位置決定に失敗した",
                        "ERROR: Failed to place monsters in nightmare_gen()"));
			return;
		}

		if (r_info[nightmare_mon_r_idx[i]].flags7 & RF7_AQUATIC)
		{
			cave_set_feat(tmp_y, tmp_x, feat_deep_water);

		}

		if (!place_monster_aux(0, tmp_y, tmp_x, nightmare_mon_r_idx[i], (PM_NO_KAGE | PM_NO_PET | PM_IGNORE_TERRAIN)))
		{
			msg_format(_("ERROR:nightmare_gen()にてモンスター配置(IDX:%d)に失敗した",
                        "ERROR: Failed to place monster (IDX: %d) in nightmare_gen()"), nightmare_mon_r_idx[i]);
		}

	}

}



/*
 * Builds the arena after it is entered -KMW-
 */
static void build_battle(void)
{
	int yval, y_height, y_depth, xval, x_left, x_right;
	register int i, j;

	yval = SCREEN_HGT / 2;
	xval = SCREEN_WID / 2;
	y_height = yval - 10;
	y_depth = yval + 10;
	x_left = xval - 32;
	x_right = xval + 32;

	for (i = y_height; i <= y_height + 5; i++)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (i = y_depth; i >= y_depth - 3; i--)
		for (j = x_left; j <= x_right; j++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_left; j <= x_left + 17; j++)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}
	for (j = x_right; j >= x_right - 17; j--)
		for (i = y_height; i <= y_depth; i++)
		{
			place_extra_perm_bold(i, j);
			cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
		}

	place_extra_perm_bold(y_height+6, x_left+18);
	cave[y_height+6][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_depth-4, x_left+18);
	cave[y_depth-4][x_left+18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_height+6, x_right-18);
	cave[y_height+6][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);
	place_extra_perm_bold(y_depth-4, x_right-18);
	cave[y_depth-4][x_right-18].info |= (CAVE_GLOW | CAVE_MARK);

	for (i = y_height + 1; i <= y_height + 5; i++)
		for (j = x_left + 20 + 2 * (y_height + 5 - i); j <= x_right - 20 - 2 * (y_height + 5 - i); j++)
		{
			cave[i][j].feat = feat_permanent_glass_wall;
		}

	i = y_height + 1;
	j = xval;
	cave[i][j].feat = f_tag_to_index("BUILDING_3");
	cave[i][j].info |= (CAVE_GLOW | CAVE_MARK);
	player_place(i, j);
}


/*
 * Town logic flow for generation of arena -KMW-
 */
static void battle_gen(void)
{
	int y, x, i;
	int qy = 0;
	int qx = 0;

	/* Start with solid walls */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			/* Create "solid" perma-wall */
			place_solid_perm_bold(y, x);

			/* Illuminate and memorize the walls */
			cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Then place some floors */
	for (y = qy + 1; y < qy + SCREEN_HGT - 1; y++)
	{
		for (x = qx + 1; x < qx + SCREEN_WID - 1; x++)
		{
			/* Create empty floor */
			cave[y][x].feat = feat_floor;
		}
	}

	build_battle();

	///mod150610 闘技場変更
	for(i=0;i<BATTLE_MON_LIST_MAX;i++)
	{
		int cnt = 0;
		if(!battle_mon_list[i].r_idx) break;
		while(1)
		{

			if(!battle_mon_list[i].r_idx) break;
			x = rand_range((SCREEN_WID / 2 - 30),SCREEN_WID / 2 + 30);
			y = rand_range((SCREEN_HGT / 2 - 8),SCREEN_WID / 2);

			if(place_monster_aux(0, y, x, battle_mon_list[i].r_idx,(PM_NO_KAGE | PM_NO_PET)))
			{
				set_friendly(&m_list[cave[y][x].m_idx]);
				m_list[cave[y][x].m_idx].mflag |= battle_mon_list[i].team;
				break;
			}

			if(cnt++ > 10000)
			{
				msg_format(_("ERROR:battle_gen()にてr_idx:%dが配置試行回数を超えた",
                            "ERROR: Maximum amount of attempts exceeded in battle_gen for r_idx: %d"),battle_mon_list[i].r_idx);
				break;
			}
		}

		/*
		if(place_monster_aux(0, py + 8 + (i/2)*4, px - 2 + (i%2)*4, battle_mon_list[i].r_idx,(PM_NO_KAGE | PM_NO_PET | PM_FORCE_FRIENDLY)))
		{

		}
		else
		{
			msg_format("ERROR:新闘技場にてr_idx(%d)を配置失敗した",battle_mon_list[i].r_idx);
		}
		//set_friendly(&m_list[cave[py+8+(i/2)*4][px-2+(i%2)*4].m_idx]);
		*/
	}
	for(i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;

		/* Hack -- Detect monster */
		m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

		/* Update the monster */
		update_mon(i, FALSE);
	}
}


/*
 * Generate a quest level
 */
static void quest_gen(void)
{
	int x, y;


	/* Start with perm walls */
	/*:::全マスを永久壁で塗る*/
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			place_solid_perm_bold(y, x);
		}
	}

	/* Set the quest level */
	base_level = quest[p_ptr->inside_quest].level;
	dun_level = base_level;
	object_level = base_level;
	monster_level = base_level;

	if (record_stair) do_cmd_write_nikki(NIKKI_TO_QUEST, p_ptr->inside_quest, NULL);

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	init_flags = INIT_CREATE_DUNGEON;

	process_dungeon_file("q_info.txt", 0, 0, MAX_HGT, MAX_WID);

	//v1.1.78 クエスト「肉体付き酔夢」のときダンジョン生成直後に泥酔する
	if (p_ptr->inside_quest == QUEST_MIYOI)
	{
		set_alcohol(DRANK_2 + randint1(5000));

	}
}


/*::: -Mega Hack- EXTRAモード用クエストフロア生成　階ごとに特定のクエスト定義ファイルを呼んでフロア生成し、最後に登り階段を下り階段にする*/
static void extra_quest_gen(void)
{
	int x, y;

	if(!EXTRA_QUEST_FLOOR)
	{
		msg_print(_("ERROR:extra_quest_gen()に入る判定がおかしい",
                    "ERROR: Weird entrance to extra_quest_gen()"));
		return;
	}

	///mod160331 この初期化がなかったのでたまにクエストフロアの生成でアイテムやモンスターが配置されなかった。
	//quest_gen()では町のマップ生成で初期化されるので初期化不要だった
	cur_hgt = MAX_HGT;
	cur_wid = MAX_WID;

	/*:::全マスを永久壁で塗る*/
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			place_solid_perm_bold(y, x);
		}
	}
	base_level = dun_level;
	object_level = base_level;
	monster_level = base_level;

	if (record_stair) do_cmd_write_nikki(NIKKI_TO_QUEST, p_ptr->inside_quest, NULL);

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), NULL);

	init_flags = INIT_CREATE_DUNGEON;
	process_dungeon_file("q_pref.txt", 0, 0, MAX_HGT, MAX_WID);
	switch(dun_level)
	{
	case 10:
		process_dungeon_file("quest02_t01_orc.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 20:
		process_dungeon_file("quest06_t08_mimic.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 30:
		process_dungeon_file("quest05_t10_vapor.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 40:
		process_dungeon_file("quest52_t11_vault.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 50:
		process_dungeon_file("quest11_t08_clone.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 60:
		process_dungeon_file("quest19_t02_oldcastle.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 70:
		process_dungeon_file("quest21_t04_flandre.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 80:
		process_dungeon_file("quest16_t05_sennin.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	case 90:
		process_dungeon_file("quest48_ex_eric.txt", 0, 0, MAX_HGT, MAX_WID);
		break;
	default:
		msg_format(_("ERROR:extra_quest_gen()が呼ばれた時のdun_level(%d)が想定外",
                    "ERROR: Unexpected dun_level (%d) upon calling extra_quest_gen()"),dun_level);
		process_dungeon_file("quest02_t01_orc.txt", 0, 0, MAX_HGT, MAX_WID);
		break;


	}


	//登り階段を下り階段にする
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(have_flag(f_info[cave[y][x].feat].flags, FF_STAIRS) && have_flag(f_info[cave[y][x].feat].flags, FF_LESS))
			{
				cave_set_feat(y, x, feat_down_stair);
			}
		}
	}

}



/* Make a real level */
static bool level_gen(cptr *why)
{
	int level_height, level_width;

	//v1.1.61 EXTRAモードでは常に中くらいのサイズの部屋にする
	if ((always_small_levels || ironman_small_levels || EXTRA_MODE ||
	    (one_in_(SMALL_LEVEL) && small_levels) ||
	     (d_info[dungeon_type].flags1 & DF1_BEGINNER) ||
		(d_info[dungeon_type].flags1 & DF1_SMALL) ||
		(d_info[dungeon_type].flags1 & DF1_SMALLEST)) &&
	    !(d_info[dungeon_type].flags1 & DF1_BIG))
	{
		if (cheat_room)
#ifdef JP
			msg_print("小さなフロア");
#else
			msg_print("A 'small' dungeon level.");
#endif

		if (d_info[dungeon_type].flags1 & DF1_SMALLEST)
		{
			level_height = 1;
			level_width = 1;
		}
		//v1.1.61 EXTRAモードでは常に中くらいのサイズの部屋にする
		else if (d_info[dungeon_type].flags1 & (DF1_BEGINNER | DF1_SMALL) || EXTRA_MODE)
		{
			if (always_small_levels || ironman_small_levels || d_info[dungeon_type].flags1 & (DF1_SMALL))
			{
				if (one_in_(2))
				{
					level_height = 1;
					level_width = 2;

				}
				else
				{
					level_height = 2;
					level_width = 1;

				}
			}
			else
			{
				level_height = 2;
				level_width = 2;
			}
		}
		else
		{
			/*:::level_heightとwidthは3/3以外のどれかになる*/
			do
			{
				level_height = randint1(MAX_HGT/SCREEN_HGT);
				level_width = randint1(MAX_WID/SCREEN_WID);
			} while ((level_height == MAX_HGT / SCREEN_HGT) &&
				(level_width == MAX_WID / SCREEN_WID));
//				   || EXTRA_MODE && level_height==1 && level_width==1);
		}

		cur_hgt = level_height * SCREEN_HGT;
		cur_wid = level_width * SCREEN_WID;

		/* Assume illegal panel */
		panel_row_min = cur_hgt;
		panel_col_min = cur_wid;

		if (cheat_room)
		  msg_format("X:%d, Y:%d.", cur_wid, cur_hgt);
	}
	/*:::普通のダンジョン。level_heightとwidthは小さいダンジョンの幅算出用なのでここでは不要*/
	else
	{
		/* Big dungeon */
		cur_hgt = MAX_HGT;
		cur_wid = MAX_WID;

		/* Assume illegal panel */
		panel_row_min = cur_hgt;
		panel_col_min = cur_wid;
	}

	/* Make a dungeon */
	if (!cave_gen())
	{
#ifdef JP
*why = "ダンジョン生成に失敗";
#else
		*why = "could not place player";
#endif

		return FALSE;
	}
	else return TRUE;
}


/*
 * Wipe all unnecessary flags after cave generation
 */
void wipe_generate_cave_flags(void)
{
	int x, y;

	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Wipe unused flags */
			cave[y][x].info &= ~(CAVE_MASK);
		}
	}

	if (dun_level)
	{
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				/* There might be trap */
				cave[y][x].info |= CAVE_UNSAFE;
			}
		}
	}
}


/*
 *  Clear and empty the cave
 */
void clear_cave(void)
{
	int x, y, i;

	/* Very simplified version of wipe_o_list() */
	(void)C_WIPE(o_list, o_max, object_type);
	o_max = 1;
	o_cnt = 0;

	/* Very simplified version of wipe_m_list() */
	for (i = 1; i < max_r_idx; i++)
		r_info[i].cur_num = 0;
	(void)C_WIPE(m_list, m_max, monster_type);
	m_max = 1;
	m_cnt = 0;
	for (i = 0; i < MAX_MTIMED; i++) mproc_max[i] = 0;

	/* Pre-calc cur_num of pets in party_mon[] */
	precalc_cur_num_of_pet();


	/* Start with a blank cave */
	for (y = 0; y < MAX_HGT; y++)
	{
		for (x = 0; x < MAX_WID; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			/* No flags */
			c_ptr->info = 0;

			/* No features */
			c_ptr->feat = 0;

			/* No objects */
			c_ptr->o_idx = 0;

			/* No monsters */
			c_ptr->m_idx = 0;

			/* No special */
			c_ptr->special = 0;

			/* No mimic */
			c_ptr->mimic = 0;

			/* No flow */
			c_ptr->cost = 0;
			c_ptr->dist = 0;
			c_ptr->when = 0;

			//v1.1.35追加 v1.1.33でダンジョングリッド用変数追加したときここの初期化が必要なことに気づかなかったが多分問題ないだろう..
			c_ptr->cave_xtra_flag = 0L;
			c_ptr->cave_xtra1 = 0;
			c_ptr->cave_xtra2 = 0;

		}
	}

	/* Mega-Hack -- no player yet */
	px = py = 0;

	/* Set the base level */
	base_level = dun_level;

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;
}


/*
 * Generates a random dungeon level			-RAK-
 *
 * Hack -- regenerate any "overflow" levels
 */
///ダンジョン生成
void generate_cave(void)
{
	int num;

	/* Fill the arrays of floors and walls in the good proportions */
	/*:::グローバル変数floor_typeとfill_typeにdungeon_typeによる地形出現率に応じて地形データを100回格納している。*/
	set_floor_and_wall(dungeon_type);

	/* Generate */
	for (num = 0; TRUE; num++)
	{
		bool okay = TRUE;

		cptr why = NULL;

		/* Clear and empty the cave */
		/*:::現在の階に関する様々なグローバル変数の初期化*/
		clear_cave();

		/* Build the arena -KMW- */
		if (p_ptr->inside_arena)
		{
			/* Small arena */
			//v1.1.51 新アリーナ実装
			nightmare_gen();
//			arena_gen();
		}

		/* Build the battle -KMW- */
		else if (p_ptr->inside_battle)
		{
			/* Small arena */
			///sysdel tougi
			battle_gen();
		}
		///mod160225 Extraモード用特殊クエストフロア生成
		else if(EXTRA_QUEST_FLOOR)
		{
			extra_quest_gen();
		}
		else if (p_ptr->inside_quest)
		{
			quest_gen();
		}
		/* Build the town */
		/*:::地上に居る（来た）とき*/
		else if (!dun_level)
		{
			/* Make the wilderness */
			if (p_ptr->wild_mode) wilderness_gen_small();/*:::ワールドマップ*/
			else wilderness_gen();
		}

		/* Build a real level */
		else
		{
			/*:::ダンジョン内のフロアを生成する。地形・モンスター・vaultなど全て*/
			okay = level_gen(&why);
		}


		/* Prevent object over-flow */
		if (o_max >= max_o_idx)
		{
			/* Message */
#ifdef JP
why = "アイテムが多すぎる";
#else
			why = "too many objects";
#endif


			/* Message */
			okay = FALSE;
		}
		/* Prevent monster over-flow */
		else if (m_max >= max_m_idx)
		{
			/* Message */
#ifdef JP
why = "モンスターが多すぎる";
#else
			why = "too many monsters";
#endif


			/* Message */
			okay = FALSE;
		}

		/* Accept */
		if (okay) break;

		/* Message */
#ifdef JP
if (why) msg_format("生成やり直し(%s)", why);
#else
		if (why) msg_format("Generation restarted (%s)", why);
#endif


		/* Wipe the objects */
		wipe_o_list();

		/* Wipe the monsters */
		wipe_m_list();
	}

	/* Glow deep lava and building entrances */
	/*:::溶岩と建物入り口を光らせる*/
	glow_deep_lava_and_bldg();

	/* Reset flag */
	p_ptr->enter_dungeon = FALSE;

	wipe_generate_cave_flags();
}
