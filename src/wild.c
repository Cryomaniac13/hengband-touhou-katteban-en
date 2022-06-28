/* File: wild.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke,
 * Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Wilderness generation */

#include "angband.h"



static void set_floor_and_wall_aux(s16b feat_type[100], feat_prob prob[DUNGEON_FEAT_PROB_NUM])
{
	int lim[DUNGEON_FEAT_PROB_NUM], cur = 0, i;

	/*:::d_info.txtの記述方法を少し変えて床OR壁の地形の種類の出やすさを配列に格納*/
	lim[0] = prob[0].percent;
	for (i = 1; i < DUNGEON_FEAT_PROB_NUM; i++) lim[i] = lim[i - 1] + prob[i].percent;

	/* Paranoia */
	if (lim[DUNGEON_FEAT_PROB_NUM - 1] < 100) lim[DUNGEON_FEAT_PROB_NUM - 1] = 100;

	for (i = 0; i < 100; i++)
	{
		/*:::グローバル変数floor_typeとfill_typeにダンジョンの種類による地形出現率に応じて地形データを100回格納している。*/
		while (i == lim[cur]) cur++;
		feat_type[i] = prob[cur].feat;
	}
}

/*
 * Fill the arrays of floors and walls in the good proportions
 */
/*:::新規時は0,それ以外のときはdungeon_typeが引数のはず*/
void set_floor_and_wall(byte type)
{
	static byte cur_type = 255;/*:::この数値は関数が終了しても保持される*/
	dungeon_info_type *d_ptr;

	//EXモードのときたまにフロアの壁や床の生成規則を上書きする.クエストフロアではやらない
	//二重に呼ばれるようなので前後でずれないよう変な擬似乱数にした
	if(EXTRA_MODE && dun_level % 10 && type == DUNGEON_ANGBAND && !((dun_level + p_ptr->total_weight)%12))
	{
		int dev = 5 + dun_level / 20;
		if(dev > 9) dev = 9;
		cur_type = 255;

		if(cheat_room) msg_print(_("EXモード特殊処理：ダンジョン生成規則上書き", "EX mode special processing: overwrite dungeon generation rules"));

		switch((dun_level + p_ptr->pclass + p_ptr->prace)%dev + 1)
		{
		case 1:
		case 2:
			d_ptr = &d_info[DUNGEON_FOREST];
			break;
		case 3:
		case 4:
			d_ptr = &d_info[DUNGEON_MUEN];
			break;
		case 5:
			d_ptr = &d_info[DUNGEON_GENBU];
			break;
		case 6://壁全部森
			{
			    feat_prob tmp_fill[3] = {{0,100},{0,0},{0,0}};
			    tmp_fill[0].feat = feat_tree;

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_tree;
				feat_wall_inner = feat_tree;
				feat_wall_solid = feat_tree;
				return;
			}
		case 7://壁全部深い水
			{
				feat_prob tmp_fill[3] = {{0,80},{0,20},{0,0}};
				tmp_fill[0].feat = feat_deep_water;
				tmp_fill[1].feat = feat_shallow_water;

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_deep_water;
				feat_wall_inner = feat_deep_water;
				feat_wall_solid = feat_deep_water;
				return;
			}
		case 8://壁全部溶岩
			{
				feat_prob tmp_fill[3] = {{0,80},{0,20},{0,0}};
				tmp_fill[0].feat = feat_deep_lava;
				tmp_fill[1].feat = feat_shallow_lava;

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_deep_lava;
				feat_wall_inner = feat_deep_lava;
				feat_wall_solid = feat_deep_lava;
				return;
			}
		case 9://壁全部深い穴
			{
				feat_prob tmp_fill[3] = {{0,100},{0,0},{0,0}};
				tmp_fill[0].feat = feat_dark_pit;

				d_ptr = &d_info[type];
				set_floor_and_wall_aux(floor_type, d_ptr->floor);
				set_floor_and_wall_aux(fill_type, tmp_fill);
				feat_wall_outer = feat_dark_pit;
				feat_wall_inner = feat_dark_pit;
				feat_wall_solid = feat_dark_pit;
				return;
			}
		default:
			break;

		}
		set_floor_and_wall_aux(floor_type, d_ptr->floor);
		set_floor_and_wall_aux(fill_type, d_ptr->fill);

		feat_wall_outer = d_ptr->outer_wall;
		feat_wall_inner = d_ptr->inner_wall;
		feat_wall_solid = d_ptr->outer_wall;
		return;
	}

	/* Already filled */
	if (cur_type == type) return;

	cur_type = type;
	/*:::このd_infoはwinmainの下のinit_angbandでd_info_j.rawから読み込まれたらしい*/
	d_ptr = &d_info[type];

	/*:::グローバル変数floor_typeとfill_typeにダンジョンの種類による地形出現率に応じて地形データを100回格納している。*/
	set_floor_and_wall_aux(floor_type, d_ptr->floor);
	set_floor_and_wall_aux(fill_type, d_ptr->fill);

	feat_wall_outer = d_ptr->outer_wall;
	feat_wall_inner = d_ptr->inner_wall;
	feat_wall_solid = d_ptr->outer_wall;
}


/*
 * Helper for plasma generation.
 */
static void perturb_point_mid(int x1, int x2, int x3, int x4,
			  int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the four corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough*2 + 1;
	int tmp = randint1(tmp2) - (rough + 1);

	int avg = ((x1 + x2 + x3 + x4) / 4) + tmp;

	/* Division always rounds down, so we round up again */
	if (((x1 + x2 + x3 + x4) % 4) > 1)
		avg++;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


static void perturb_point_end(int x1, int x2, int x3,
			  int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the three corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough * 2 + 1;
	int tmp = randint0(tmp2) - rough;

	int avg = ((x1 + x2 + x3) / 3) + tmp;

	/* Division always rounds down, so we round up again */
	if ((x1 + x2 + x3) % 3) avg++;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave[ymid][xmid].feat = avg;
}


/*
 * A generic function to generate the plasma fractal.
 * Note that it uses ``cave_feat'' as temporary storage.
 * The values in ``cave_feat'' after this function
 * are NOT actual features; They are raw heights which
 * need to be converted to features.
 */
static void plasma_recursive(int x1, int y1, int x2, int y2,
			     int depth_max, int rough)
{
	/* Find middle */
	int xmid = (x2 - x1) / 2 + x1;
	int ymid = (y2 - y1) / 2 + y1;

	/* Are we done? */
	if (x1 + 1 == x2) return;

	perturb_point_mid(cave[y1][x1].feat, cave[y2][x1].feat, cave[y1][x2].feat,
		cave[y2][x2].feat, xmid, ymid, rough, depth_max);

	perturb_point_end(cave[y1][x1].feat, cave[y1][x2].feat, cave[ymid][xmid].feat,
		xmid, y1, rough, depth_max);

	perturb_point_end(cave[y1][x2].feat, cave[y2][x2].feat, cave[ymid][xmid].feat,
		x2, ymid, rough, depth_max);

	perturb_point_end(cave[y2][x2].feat, cave[y2][x1].feat, cave[ymid][xmid].feat,
		xmid, y2, rough, depth_max);

	perturb_point_end(cave[y2][x1].feat, cave[y1][x1].feat, cave[ymid][xmid].feat,
		x1, ymid, rough, depth_max);


	/* Recurse the four quadrants */
	plasma_recursive(x1, y1, xmid, ymid, depth_max, rough);
	plasma_recursive(xmid, y1, x2, ymid, depth_max, rough);
	plasma_recursive(x1, ymid, xmid, y2, depth_max, rough);
	plasma_recursive(xmid, ymid, x2, y2, depth_max, rough);
}


#define MAX_FEAT_IN_TERRAIN 18

/*
 * The default table in terrain level generation.
 */
static s16b terrain_table[MAX_WILDERNESS][MAX_FEAT_IN_TERRAIN];

/*:::荒野用ランダム地形生成ルーチン*/
static void generate_wilderness_area(int terrain, u32b seed, bool border, bool corner)
{
	int x1, y1;
	int table_size = sizeof(terrain_table[0]) / sizeof(s16b);
	int roughness = 1; /* The roughness of the level. */
	u32b state_backup[4];

	/* Unused */
	(void)border;

	/* The outer wall is easy */
	if (terrain == TERRAIN_EDGE)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = feat_permanent;
			}
		}

		/* We are done already */
		return;
	}


	/* Hack -- Backup the RNG state */
	Rand_state_backup(state_backup);

	/* Hack -- Induce consistant flavors */
	Rand_state_init(seed);

	if (!corner)
	{
		/* Create level background */
		for (y1 = 0; y1 < MAX_HGT; y1++)
		{
			for (x1 = 0; x1 < MAX_WID; x1++)
			{
				cave[y1][x1].feat = table_size / 2;
			}
		}
	}

	/*
	 * Initialize the four corners
	 * ToDo: calculate the medium height of the adjacent
	 * terrains for every corner.
	 */
	cave[1][1].feat = randint0(table_size);
	cave[MAX_HGT-2][1].feat = randint0(table_size);
	cave[1][MAX_WID-2].feat = randint0(table_size);
	cave[MAX_HGT-2][MAX_WID-2].feat = randint0(table_size);

	if (!corner)
	{
		/* Hack -- preserve four corners */
		s16b north_west = cave[1][1].feat;
		s16b south_west = cave[MAX_HGT - 2][1].feat;
		s16b north_east = cave[1][MAX_WID - 2].feat;
		s16b south_east = cave[MAX_HGT - 2][MAX_WID - 2].feat;

		/* x1, y1, x2, y2, num_depths, roughness */
		plasma_recursive(1, 1, MAX_WID-2, MAX_HGT-2, table_size-1, roughness);

		/* Hack -- copyback four corners */
		cave[1][1].feat = north_west;
		cave[MAX_HGT - 2][1].feat = south_west;
		cave[1][MAX_WID - 2].feat = north_east;
		cave[MAX_HGT - 2][MAX_WID - 2].feat = south_east;

		for (y1 = 1; y1 < MAX_HGT - 1; y1++)
		{
			for (x1 = 1; x1 < MAX_WID - 1; x1++)
			{
				cave[y1][x1].feat = terrain_table[terrain][cave[y1][x1].feat];
			}
		}
	}
	else /* Hack -- only four corners */
	{
		cave[1][1].feat = terrain_table[terrain][cave[1][1].feat];
		cave[MAX_HGT - 2][1].feat = terrain_table[terrain][cave[MAX_HGT - 2][1].feat];
		cave[1][MAX_WID - 2].feat = terrain_table[terrain][cave[1][MAX_WID - 2].feat];
		cave[MAX_HGT - 2][MAX_WID - 2].feat = terrain_table[terrain][cave[MAX_HGT - 2][MAX_WID - 2].feat];
	}

	/* Hack -- Restore the RNG state */
	Rand_state_restore(state_backup);
}



/*
 * Load a town or generate a terrain level using "plasma" fractals.
 *
 * x and y are the coordinates of the area in the wilderness.
 * Border and corner are optimization flags to speed up the
 * generation of the fractal terrain.
 * If border is set then only the border of the terrain should
 * be generated (for initializing the border structure).
 * If corner is set then only the corners of the area are needed.
 */
/*:::ワールドマップのY,Xの位置の荒野地形情報を生成する。*/
/*:::borderとcornerは端だけ生成する（隣のマップを描画するときに最外周に使う）*/
/*:::p_ptr->town_numの代入など一部変数処理もここで行われている（つまりマップ周辺を先に作らないといけない？)*/

static void generate_area(int y, int x, bool border, bool corner)
{
	int x1, y1;

	/* Number of the town (if any) */
	p_ptr->town_num = wilderness[y][x].town;

	/* Set the base level */
	base_level = wilderness[y][x].level;

	/* Set the dungeon level */
	dun_level = 0;

	/* Set the monster generation level */
	monster_level = base_level;

	/* Set the object generation level */
	object_level = base_level;


	/* Create the town */
	/*:::＠がwilderness[][].townに設定された場所にいるとき*/
	if (p_ptr->town_num)
	{
		/* Reset the buildings */
		/*:::building[]を初期化する*/
		init_buildings();

		/* Initialize the town */
		if (border | corner)
			init_flags = INIT_CREATE_DUNGEON | INIT_ONLY_FEATURES;
		else
			init_flags = INIT_CREATE_DUNGEON;
		/*:::街の地形をcave[][]に格納*/
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);
		/*:::訪れた事のある街フラグを立てる*/
		if (!corner && !border) p_ptr->visit |= (1L << (p_ptr->town_num - 1));
	}
	else
	{
		int terrain = wilderness[y][x].terrain;
		u32b seed = wilderness[y][x].seed;
		/*:::荒野地形ランダム生成*/
		generate_wilderness_area(terrain, seed, border, corner);
	}
	/*:::道の配置*/
	if (!corner && !wilderness[y][x].town)
	{
		/*
		 * Place roads in the wilderness
		 * ToDo: make the road a bit more interresting
		 */
		if (wilderness[y][x].road)
		{
			cave[MAX_HGT/2][MAX_WID/2].feat = feat_floor;

			if (wilderness[y-1][x].road)
			{
				/* North road */
				for (y1 = 1; y1 < MAX_HGT/2; y1++)
				{
					x1 = MAX_WID/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y+1][x].road)
			{
				/* North road */
				for (y1 = MAX_HGT/2; y1 < MAX_HGT - 1; y1++)
				{
					x1 = MAX_WID/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y][x+1].road)
			{
				/* East road */
				for (x1 = MAX_WID/2; x1 < MAX_WID - 1; x1++)
				{
					y1 = MAX_HGT/2;
					cave[y1][x1].feat = feat_floor;
				}
			}

			if (wilderness[y][x-1].road)
			{
				/* West road */
				for (x1 = 1; x1 < MAX_WID/2; x1++)
				{
					y1 = MAX_HGT/2;
					cave[y1][x1].feat = feat_floor;
				}
			}
		}
	}
	/*:::ダンジョンへの階段を配置*/
	if (wilderness[y][x].entrance && !wilderness[y][x].town && (p_ptr->total_winner || !(d_info[wilderness[y][x].entrance].flags1 & DF1_WINNER)))
	{
		int dy, dx;
		u32b state_backup[4];

		/* Hack -- Backup the RNG state */
		Rand_state_backup(state_backup);

		/* Hack -- Induce consistant flavors */
		Rand_state_init(wilderness[y][x].seed);

		dy = rand_range(6, cur_hgt - 6);
		dx = rand_range(6, cur_wid - 6);

		cave[dy][dx].feat = feat_entrance;
		cave[dy][dx].special = wilderness[y][x].entrance;

		/* Hack -- Restore the RNG state */
		Rand_state_restore(state_backup);
	}
}


/*
 * Border of the wilderness area
 */
static border_type border;


/*
 * Build the wilderness area outside of the town.
 */
/*:::街と地上地形を生成*/
void wilderness_gen(void)
{
	int i, y, x, lim;
	cave_type *c_ptr;
	feature_type *f_ptr;

	/* Big town */
	cur_hgt = MAX_HGT;
	cur_wid = MAX_WID;

	/* Assume illegal panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Init the wilderness */
	/*:::wilderness[][]に地形データが入る*/
	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	x = p_ptr->wilderness_x;
	y = p_ptr->wilderness_y;

	//v1.1.32 特殊街のモンスター生成判定のため、この時点でp_ptr->town_numを書き換えることにした
	p_ptr->town_num = wilderness[y][x].town;

	/* Prepare allocation table */
	/*:::荒野の地形に合わせてhookを変更しモンスター発生準備*/
	get_mon_num_prep(get_monster_hook(), NULL);

	/* North border */
	generate_area(y - 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.north[i] = cave[MAX_HGT - 2][i].feat;
	}

	/* South border */
	generate_area(y + 1, x, TRUE, FALSE);

	for (i = 1; i < MAX_WID - 1; i++)
	{
		border.south[i] = cave[1][i].feat;
	}

	/* West border */
	generate_area(y, x - 1, TRUE, FALSE);

	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.west[i] = cave[i][MAX_WID - 2].feat;
	}

	/* East border */
	generate_area(y, x + 1, TRUE, FALSE);

	for (i = 1; i < MAX_HGT - 1; i++)
	{
		border.east[i] = cave[i][1].feat;
	}

	/* North west corner */
	generate_area(y - 1, x - 1, FALSE, TRUE);
	border.north_west = cave[MAX_HGT - 2][MAX_WID - 2].feat;

	/* North east corner */
	generate_area(y - 1, x + 1, FALSE, TRUE);
	border.north_east = cave[MAX_HGT - 2][1].feat;

	/* South west corner */
	generate_area(y + 1, x - 1, FALSE, TRUE);
	border.south_west = cave[1][MAX_WID - 2].feat;

	/* South east corner */
	generate_area(y + 1, x + 1, FALSE, TRUE);
	border.south_east = cave[1][1].feat;


	/* Create terrain of the current area */
	/*:::今居るエリアの地形をcave[][]に格納。街ならt*.txt 荒野ならシードをもとに擬似乱数生成する*/
	generate_area(y, x, FALSE, FALSE);

	/*:::ワールドマップ端は永久壁にする*/
	/* Special boundary walls -- North */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[0][i].feat = feat_permanent;
		cave[0][i].mimic = border.north[i];
	}

	/* Special boundary walls -- South */
	for (i = 0; i < MAX_WID; i++)
	{
		cave[MAX_HGT - 1][i].feat = feat_permanent;
		cave[MAX_HGT - 1][i].mimic = border.south[i];
	}

	/* Special boundary walls -- West */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][0].feat = feat_permanent;
		cave[i][0].mimic = border.west[i];
	}

	/* Special boundary walls -- East */
	for (i = 0; i < MAX_HGT; i++)
	{
		cave[i][MAX_WID - 1].feat = feat_permanent;
		cave[i][MAX_WID - 1].mimic = border.east[i];
	}

	/* North west corner */
	cave[0][0].mimic = border.north_west;

	/* North east corner */
	cave[0][MAX_WID - 1].mimic = border.north_east;

	/* South west corner */
	cave[MAX_HGT - 1][0].mimic = border.south_west;

	/* South east corner */
	cave[MAX_HGT - 1][MAX_WID - 1].mimic = border.south_east;

	/* Light up or darken the area */
	/*:::昼は全エリア明るく、夜は・・偽装地形扱い？詳細不明*/
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Get the cave grid */
			c_ptr = &cave[y][x];

			if (is_daytime())
			{
				/* Assume lit */
				c_ptr->info |= (CAVE_GLOW);

				/* Hack -- Memorize lit grids if allowed */
				if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
			}
			else
			{
				/* Feature code (applying "mimic" field) */
				f_ptr = &f_info[get_feat_mimic(c_ptr)];

				/*:::鏡が置かれておらず階段でもない所は暗くする*/
				if (!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
				    !have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					/* Assume dark */
					c_ptr->info &= ~(CAVE_GLOW);

					/* Darken "boring" features */
					if (!have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Forget the grid */
						c_ptr->info &= ~(CAVE_MARK);
					}
				}
				else if (have_flag(f_ptr->flags, FF_ENTRANCE))
				{
					/* Assume lit */
					c_ptr->info |= (CAVE_GLOW);

					/* Hack -- Memorize lit grids if allowed */
					if (view_perma_grids) c_ptr->info |= (CAVE_MARK);
				}
			}
		}
	}

	//v1.1.32 索道
	//v1.1.91 地霊虹洞も
	if(teleport_town_mode)
	{
		int building_num;

		if (teleport_town_mode == TELE_TOWN_MODE_ROPEWAY)
		{
			//出てくる建物の番号 t00000xx.txt参照
			if (p_ptr->town_num == TOWN_MORIYA)
				building_num = 1;
			else
				building_num = 11;
		}
		else
		{
			if (p_ptr->town_num == TOWN_CHITEI)
				building_num = 6;
			else
				building_num = 3;

		}


		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				c_ptr = &cave[y][x];
				f_ptr = &f_info[c_ptr->feat];

				if (have_flag(f_ptr->flags, FF_BLDG))
				{
					if (f_ptr->subtype == building_num)
					{
						if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
						p_ptr->oldpy = y;
						p_ptr->oldpx = x;
					}
				}
			}
		}

		teleport_town_mode = 0;
		p_ptr->teleport_town = FALSE;
	}
	/*:::街移動で町に来た場合？*/
	else if (p_ptr->teleport_town)
	{
		/*:::Hack 神子塔のない街へ移動してきた場合のデフォルト位置を設定*/
		p_ptr->oldpy = 40;
		p_ptr->oldpx = 85;

		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/* Get the cave grid */
				c_ptr = &cave[y][x];

				/* Seeing true feature code (ignore mimic) */
				f_ptr = &f_info[c_ptr->feat];

				/*:::町ごとに建物の入り口を＠の位置とし、モンスターがいたら削除している？*/
				if (have_flag(f_ptr->flags, FF_BLDG))
				{


					if ((f_ptr->subtype == 2)
						|| ((p_ptr->town_num == TOWN_HITOZATO || p_ptr->town_num == TOWN_KOURIN) && (f_ptr->subtype == 3))
						|| (p_ptr->town_num == TOWN_KOUMA  && (f_ptr->subtype == 1)))
					{
						if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
						p_ptr->oldpy = y;
						p_ptr->oldpx = x;
					}
				}
			}
		}
		p_ptr->teleport_town = FALSE;
	}
	/*:::帰還で街や荒野に来た場合？　階段にモンスターがいたら削除している？*/
	else if (p_ptr->leaving_dungeon)
	{
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				/* Get the cave grid */
				c_ptr = &cave[y][x];

				if (cave_have_flag_grid(c_ptr, FF_ENTRANCE))
				{
					if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
					p_ptr->oldpy = y;
					p_ptr->oldpx = x;
				}
			}
		}
		/*:::leaving_dungeonでなくteleport_townなのか？*/
		p_ptr->teleport_town = FALSE;
	}
	/*:::＠を配置*/
	player_place(p_ptr->oldpy, p_ptr->oldpx);
	/* p_ptr->leaving_dungeon = FALSE;*/

	lim = (generate_encounter==TRUE)?40:MIN_M_ALLOC_TN;

	/* Make some residents */
	for (i = 0; i < lim; i++)
	{
		u32b mode = 0;

		/*:::荒野でgenerate_encounter(襲撃時?)ならモンスターが常に起きている。そうでないなら1/2で起きている。街では寝ている？*/
		if (!(generate_encounter || (one_in_(2) && (!p_ptr->town_num))))
			mode |= PM_ALLOW_SLEEP;

		/* Make a resident */
		(void)alloc_monster(generate_encounter ? 0 : 3, mode);
	}

//	msg_format("chk:monster_level:%d", monster_level);

	if(generate_encounter) ambush_flag = TRUE;
	generate_encounter = FALSE;

	/* Fill the arrays of floors and walls in the good proportions */
	set_floor_and_wall(0);

	/* Set rewarded quests to finished */
	///quest
	/*:::この描画でクエスト完了報告済みによるアイテム生成が済んだのでクエスト状態を終了にしている*/
	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status == QUEST_STATUS_REWARDED)
		{
			quest[i].status = QUEST_STATUS_FINISHED;
			if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
			set_deity_bias(DBIAS_REPUTATION, 2);
			set_deity_bias(DBIAS_COSMOS, 1);

			//正邪がクエスト達成ペナルティを受けるためのフラグ
			if(p_ptr->pclass == CLASS_SEIJA && i != QUEST_HOME1 && i != QUEST_HOME2 && i != QUEST_HOME3) flag_seija_quest_comp = TRUE;
		}
	}
}


static s16b conv_terrain2feat[MAX_WILDERNESS];

/*
 * Build the wilderness area.
 * -DG-
 */
/*:::ワールドマップを生成し、その地形情報をcave[][]に格納*/
void wilderness_gen_small()
{
	int i, j;

	/* To prevent stupid things */
	/*:::ワールドマップ生成前に念のため全て永久壁にしている？どうせならplace_solid_perm_bold()でも使えば良さそうなもんだが*/
	for (i = 0; i < MAX_WID; i++)
	for (j = 0; j < MAX_HGT; j++)
	{
		cave[j][i].feat = feat_permanent;
	}

	/* Init the wilderness */
	/*:::wilderness[][]に地形データが入る*/
	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	/* Fill the map */
	for (i = 0; i < max_wild_x; i++)
	for (j = 0; j < max_wild_y; j++)
	{

		if (wilderness[j][i].town)
		{
			// -HACK- 河童のバザーをワールドマップから見えなくする
			//v1.1.91 もうそろそろ隠し街にしなくてもいいか。あまりユーザフレンドリーと言えんし原作でもそれほど隠れてはいない
			/*
			if (wilderness[j][i].town != TOWN_KAPPA)
			{
				cave[j][i].feat = feat_town;
			}
			else
			{
				cave[j][i].feat = conv_terrain2feat[wilderness[j][i].terrain];
			}
			*/

			cave[j][i].feat = feat_town;
			cave[j][i].special = wilderness[j][i].town;
		}
		else if (wilderness[j][i].road) cave[j][i].feat = feat_floor;
		else if (wilderness[j][i].entrance && (p_ptr->total_winner || !(d_info[wilderness[j][i].entrance].flags1 & DF1_WINNER)))
		{
			cave[j][i].feat = feat_entrance;
			cave[j][i].special = (byte)wilderness[j][i].entrance;
		}
		/*:::地形情報インデックスを変換しているらしい。この配列はinit_terrain_table()で初期化されているようだ*/
		else cave[j][i].feat = conv_terrain2feat[wilderness[j][i].terrain];

		cave[j][i].info |= (CAVE_GLOW | CAVE_MARK);
	}

	cur_hgt = (s16b) max_wild_y;
	cur_wid = (s16b) max_wild_x;

	if (cur_hgt > MAX_HGT) cur_hgt = MAX_HGT;
	if (cur_wid > MAX_WID) cur_wid = MAX_WID;

	/* Assume illegal panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Place the player */
	px = p_ptr->wilderness_x;
	py = p_ptr->wilderness_y;

	p_ptr->town_num = 0;
}


typedef struct wilderness_grid wilderness_grid;

struct wilderness_grid
{
	int		terrain;    /* Terrain type */
	int		town;       /* Town number */
	s16b	level;		/* Level of the wilderness */
	byte	road;       /* Road */
	char	name[32];	/* Name of the town/wilderness */
};


static wilderness_grid w_letter[255];


/*
 * Parse a sub-file of the "extra info"
 */
/*:::w_info.txtを解釈する部分　x/y min/max : 生成されるマップの領域？ x,y: 初期ではxminとyminの値が格納されている*/
///system 河童のバザーとか紅魔館とか細工するならこの辺
errr parse_line_wilderness(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x)
{
	int i, num;
	char *zz[33];

	/* Unused */
	(void)ymin;
	(void)ymax;

	/* Paranoia */
	if (!(buf[0] == 'W')) return (PARSE_ERROR_GENERIC);

	switch (buf[2])
	{
		/* Process "W:F:<letter>:<terrain>:<town>:<road>:<name> */

		/*:::W：F(J)：<letter>：<terrain>：<level>：<town>：<road>：<name>*/
#ifdef JP
		/*:::E:英語用街*/
	case 'E':
		return 0;
	case 'F':
	case 'J':
#else
	case 'J':
		return 0;
	case 'F':
	case 'E':
#endif
	{
		if ((num = tokenize(buf+4, 6, zz, 0)) > 1)
		{
			int index = zz[0][0];

			if (num > 1)
				w_letter[index].terrain = atoi(zz[1]);
			else
				w_letter[index].terrain = 0;

			if (num > 2)
				w_letter[index].level = atoi(zz[2]);
			else
				w_letter[index].level = 0;

			if (num > 3)
				w_letter[index].town = atoi(zz[3]);
			else
				w_letter[index].town = 0;

			if (num > 4)
				w_letter[index].road = atoi(zz[4]);
			else
				w_letter[index].road = 0;

			if (num > 5)
				strcpy(w_letter[index].name, zz[5]);
			else
				w_letter[index].name[0] = 0;
		}
		else
		{
				/* Failure */
			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		break;
	}

	/* Process "W:D:<layout> */
	/* Layout of the wilderness */
	case 'D':
	{
		/* Acquire the text */
		char *s = buf+4;

		/* Length of the text */
		int len = strlen(s);

		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			/*:::W:D:のあとの文字を一つづつ辿ってwilderness[][]にw_letterの情報を写している。*/
			/*:::yは上端から始まる*/
			int idx = s[0];

			wilderness[*y][*x].terrain = w_letter[idx].terrain;

			wilderness[*y][*x].level = w_letter[idx].level;

			wilderness[*y][*x].town = w_letter[idx].town;

			wilderness[*y][*x].road = w_letter[idx].road;

			strcpy(town[w_letter[idx].town].name, w_letter[idx].name);
		}

		(*y)++;

		break;
	}

	/* Process "W:P:<x>:<y> - starting position in the wilderness */
	/*:::荒野での初期位置を決めてるらしい。辺境の町の座標だがここのデータは使われているのか？*/
	case 'P':
	{
		if ((p_ptr->wilderness_x == 0) &&
		    (p_ptr->wilderness_y == 0))
		{
			if (tokenize(buf+4, 2, zz, 0) == 2)
			{
				p_ptr->wilderness_y = atoi(zz[0]);
				p_ptr->wilderness_x = atoi(zz[1]);

				if ((p_ptr->wilderness_x < 1) ||
				    (p_ptr->wilderness_x > max_wild_x) ||
				    (p_ptr->wilderness_y < 1) ||
				    (p_ptr->wilderness_y > max_wild_y))
				{
					return (PARSE_ERROR_OUT_OF_BOUNDS);
				}
			}
			else
			{
				return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			}
		}

		break;
	}

	default:
		/* Failure */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/*:::d_infoを参照してダンジョンへの階段のある地点にダンジョンインデックスを入れている*/
	for (i = 1; i < max_d_idx; i++)
	{
		if (!d_info[i].maxdepth) continue;
		wilderness[d_info[i].dy][d_info[i].dx].entrance = i;
		/*:::ダンジョンが街に重なってる場合（イーク窟）は、そのマスの荒野レベルをダンジョンの最低レベルに設定する？　*/
		if (!wilderness[d_info[i].dy][d_info[i].dx].town)
			wilderness[d_info[i].dy][d_info[i].dx].level = d_info[i].mindepth;
	}

	/* Success */
	return (0);
}


/*
 * Generate the random seeds for the wilderness
 */
void seed_wilderness(void)
{
	int x, y;

	/* Init wilderness seeds */
	for (x = 0; x < max_wild_x; x++)
	{
		for (y = 0; y < max_wild_y; y++)
		{
			wilderness[y][x].seed = randint0(0x10000000);
			wilderness[y][x].entrance = 0;
		}
	}
}


/*
 * Pointer to wilderness_type
 */
typedef wilderness_type *wilderness_type_ptr;

/*
 * Initialize wilderness array
 */
/*:::荒野の初期化*/
errr init_wilderness(void)
{
	int i;

	/* Allocate the wilderness (two-dimension array) */
	C_MAKE(wilderness, max_wild_y, wilderness_type_ptr);
	C_MAKE(wilderness[0], max_wild_x * max_wild_y, wilderness_type);

	/* Init the other pointers */
	for (i = 1; i < max_wild_y; i++)
		wilderness[i] = wilderness[0] + i * max_wild_x;

	generate_encounter = FALSE;

	return 0;
}


static void init_terrain_table(int terrain, s16b feat_global, cptr fmt, ...)
{
	va_list vp;
	cptr    p;
	int     cur = 0;
	char    check = 'a';
	s16b    feat;
	int     num;

	/* Begin the varargs stuff */
	va_start(vp, fmt);

	/* Wilderness terrains on global map */
	conv_terrain2feat[terrain] = feat_global;

	/* Wilderness terrains on local map */
	for (p = fmt; *p; p++)
	{
		if (*p == check)
		{
			int lim;

			feat = (s16b)va_arg(vp, int);
			num = va_arg(vp, int);
			lim = cur + num;

			for (; (cur < lim) && (cur < MAX_FEAT_IN_TERRAIN); cur++)
				terrain_table[terrain][cur] = feat;
			if (cur >= MAX_FEAT_IN_TERRAIN) break;

			check++;
		}
		else /* Paranoia */
		{
			plog_fmt("Format error");
		}
	}

	/* Paranoia */
	if (cur < MAX_FEAT_IN_TERRAIN)
	{
		plog_fmt("Too few parameters");
	}

	/* End the varargs stuff */
	va_end(vp);
}


/*
 * Initialize arrays for wilderness terrains
 */
void init_wilderness_terrains(void)
{
	init_terrain_table(TERRAIN_EDGE, feat_permanent, "a",
		feat_permanent, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_TOWN, feat_town, "a",
		feat_floor, MAX_FEAT_IN_TERRAIN);

	init_terrain_table(TERRAIN_DEEP_WATER, feat_deep_water, "ab",
		feat_deep_water, 12,
		feat_shallow_water, MAX_FEAT_IN_TERRAIN - 12);

	init_terrain_table(TERRAIN_SHALLOW_WATER, feat_shallow_water, "abcde",
		feat_deep_water, 3,
		feat_shallow_water, 12,
		feat_floor, 1,
		feat_dirt, 1,
		feat_grass, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_SWAMP, feat_swamp, "abcdef",
		feat_dirt, 2,
		feat_grass, 3,
		feat_tree, 1,
		feat_brake, 1,
		feat_shallow_water, 4,
		feat_swamp, MAX_FEAT_IN_TERRAIN - 11);

	init_terrain_table(TERRAIN_DIRT, feat_dirt, "abcdef",
		feat_floor, 3,
		feat_dirt, 10,
		feat_flower, 1,
		feat_brake, 1,
		feat_grass, 1,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_GRASS, feat_grass, "abcdef",
		feat_floor, 2,
		feat_dirt, 2,
		feat_grass, 9,
		feat_flower, 1,
		feat_brake, 2,
		feat_tree, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_TREES, feat_tree, "abcde",
		feat_floor, 2,
		feat_dirt, 1,
		feat_tree, 11,
		feat_brake, 2,
		feat_grass, MAX_FEAT_IN_TERRAIN - 16);

	init_terrain_table(TERRAIN_DESERT, feat_dirt, "abc",
		feat_floor, 2,
		feat_dirt, 13,
		feat_grass, MAX_FEAT_IN_TERRAIN - 15);

	init_terrain_table(TERRAIN_SHALLOW_LAVA, feat_shallow_lava, "abc",
		feat_shallow_lava, 14,
		feat_deep_lava, 3,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 17);

	init_terrain_table(TERRAIN_DEEP_LAVA, feat_deep_lava, "abcd",
		feat_dirt, 3,
		feat_shallow_lava, 3,
		feat_deep_lava, 10,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 16);

	/*
	init_terrain_table(TERRAIN_MOUNTAIN, feat_mountain, "abcdef",
		feat_floor, 1,
		feat_brake, 1,
		feat_grass, 2,
		feat_dirt, 2,
		feat_tree, 2,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 8);
	*/
	//v1.1.78 山地形をもっと森がちにした
	init_terrain_table(TERRAIN_MOUNTAIN, feat_mountain, "abcdef",
		feat_floor, 1,
		feat_brake, 2,
		feat_grass, 2,
		feat_dirt, 5,
		feat_tree, 3,
		feat_mountain, MAX_FEAT_IN_TERRAIN - 13);

	//v1.1.32追加　特殊街　正直この関数が何やってるのか全然わからないけど街の場合terrain_tableを使わないはずだから適当でも大丈夫だと思う。たぶん。
	init_terrain_table(TERRAIN_SPECIAL_TOWN, feat_town, "a",
		feat_floor, MAX_FEAT_IN_TERRAIN);

}

/*:::ワイルドモードと荒野マップを切り替える　詳細未読*/
bool change_wild_mode(void)
{
	int i;
	bool have_pet = FALSE;

	/* It is in the middle of changing map */
	if (p_ptr->leaving) return FALSE;


	if (lite_town || vanilla_town)
	{
#ifdef JP
		msg_print("荒野なんてない。");
#else
		msg_print("No global map.");
#endif
		return FALSE;
	}

	if (p_ptr->wild_mode)
	{
		/* Save the location in the global map */
		p_ptr->wilderness_x = px;
		p_ptr->wilderness_y = py;

		/* Give first move to the player */
		p_ptr->energy_need = 0;

		/* Go back to the ordinary map */
		p_ptr->wild_mode = FALSE;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Succeed */
		return TRUE;
	}

///mod140402 ダメージを受ける地形で広域マップに入ろうとしたときキャンセル
	{
		bool flag_danger = FALSE;

		if(wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_DEEP_WATER)
		{
			if(prace_is_(RACE_VAMPIRE) && !p_ptr->levitation && !p_ptr->can_swim) flag_danger = TRUE;
			else if(!p_ptr->resist_water && !p_ptr->levitation && p_ptr->total_weight > weight_limit() && !p_ptr->can_swim) flag_danger = TRUE;
		}
		if((wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_SHALLOW_LAVA || wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain == TERRAIN_DEEP_LAVA ) && p_ptr->immune_fire) flag_danger = TRUE;
		if(flag_danger)
		{
			msg_print(_("この危険な場所からは広域マップに入れない！",
                        "You can't enter global map from such a dangerous place!"));
			return FALSE;
		}
	}


	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr) && i != p_ptr->riding) have_pet = TRUE;
		if (MON_CSLEEP(m_ptr)) continue;
		if (m_ptr->cdis > MAX_SIGHT) continue;
		if (!is_hostile(m_ptr)) continue;
#ifdef JP
		msg_print("敵がすぐ近くにいるときは広域マップに入れない！");
#else
		msg_print("You cannot enter global map, since there is some monsters nearby!");
#endif
		energy_use = 0;
		return FALSE;
	}

	if (have_pet)
	{
#ifdef JP
		cptr msg = "配下を置いて広域マップに入りますか？";
#else
		cptr msg = "Do you leave your followers behind? ";
#endif

		if (!get_check_strict(msg, CHECK_OKAY_CANCEL))
		{
			energy_use = 0;
			return FALSE;
		}
	}

	/* HACK */
	energy_use = 1000;

	if(SAKUYA_WORLD) energy_use = 0;

	/* Remember the position */
	p_ptr->oldpx = px;
	p_ptr->oldpy = py;

	/* Cancel hex spelling */
	if (hex_spelling_any()) stop_hex_spell_all();

	/* Cancel any special action */
	set_action(ACTION_NONE);

	/* Go into the global map */
	p_ptr->wild_mode = TRUE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	/* Succeed */
	return TRUE;
}
