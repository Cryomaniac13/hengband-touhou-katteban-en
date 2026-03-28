/*
 * File: rooms.c
 * Purpose: make rooms. Used by generate.c when creating dungeons.
 */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "generate.h"
#include "grid.h"
#include "rooms.h"


/*
 * [from SAngband (originally from OAngband)]
 *
 * Table of values that control how many times each type of room will
 * appear.  Each type of room has its own row, and each column
 * corresponds to dungeon levels 0, 10, 20, and so on.  The final
 * value is the minimum depth the room can appear at.  -LM-
 *
 * Level 101 and below use the values for level 100.
 *
 * Rooms with lots of monsters or loot may not be generated if the
 * object or monster lists are already nearly full.  Rooms will not
 * appear above their minimum depth.  Tiny levels will not have space
 * for all the rooms you ask for.
 */
static room_info_type room_info_normal[ROOM_T_MAX] =
{
	/* Depth */
	/*  0  10  20  30  40  50  60  70  80  90 100  min limit */

	{{999,900,800,700,600,500,400,300,200,100,  0},  0}, /*NORMAL   */
	{{  1, 10, 20, 30, 40, 50, 60, 70, 80, 90,100},  1}, /*OVERLAP  */
	{{  1, 10, 20, 30, 40, 50, 60, 70, 80, 90,100},  3}, /*CROSS    */
	{{  1, 10, 20, 30, 40, 50, 60, 70, 80, 90,100},  3}, /*INNER_F  */
	{{  0,  1,  1,  1,  2,  3,  5,  6,  8, 10, 13}, 10}, /*NEST     */
	{{  0,  1,  1,  2,  3,  4,  6,  8, 10, 13, 16}, 10}, /*PIT      */
	{{  0,  1,  1,  1,  2,  2,  3,  5,  6,  8, 10}, 10}, /*LESSER_V */
	{{  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  4}, 20}, /*GREATER_V*/
	{{  0,100,200,300,400,500,600,700,800,900,999}, 10}, /*FRACAVE  */
	{{  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2}, 10}, /*RANDOM_V */
	{{  0,  4,  8, 12, 16, 20, 24, 28, 32, 36, 40},  3}, /*OVAL     */
	{{  1,  6, 12, 18, 24, 30, 36, 42, 48, 54, 60}, 10}, /*CRYPT    */
	{{  0,  0,  1,  1,  1,  2,  3,  4,  5,  6,  8}, 20}, /*TRAP_PIT */
	{{  0,  0,  1,  1,  1,  2,  3,  4,  5,  6,  8}, 20}, /*TRAP     */
	{{  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  2}, 40}, /*GLASS    */
	{{  1,  1,  1,  1,  1,  1,  1,  2,  2,  3,  3},  1}, /*ARCADE   */
};


/* Build rooms in descending order of difficulty. */
static byte room_build_order[ROOM_T_MAX] = {
	ROOM_T_GREATER_VAULT,
	ROOM_T_ARCADE,
	ROOM_T_RANDOM_VAULT,
	ROOM_T_LESSER_VAULT,
	ROOM_T_TRAP_PIT,
	ROOM_T_PIT,
	ROOM_T_NEST,
	ROOM_T_TRAP,
	ROOM_T_GLASS,
	ROOM_T_INNER_FEAT,
	ROOM_T_OVAL,
	ROOM_T_CRYPT,
	ROOM_T_OVERLAP,
	ROOM_T_CROSS,
	ROOM_T_FRACAVE,
	ROOM_T_NORMAL,
};


static void place_locked_door(int y, int x)
{
	if (d_info[dungeon_type].flags1 & DF1_NO_DOORS)
	{
		place_floor_bold(y, x);
	}
	else
	{
		set_cave_feat(y, x, feat_locked_door_random((d_info[dungeon_type].flags1 & DF1_GLASS_DOOR) ? DOOR_GLASS_DOOR : DOOR_DOOR));
		cave[y][x].info &= ~(CAVE_FLOOR);
		delete_monster(y, x);
	}
}

static void place_secret_door(int y, int x, int type)
{
	if (d_info[dungeon_type].flags1 & DF1_NO_DOORS)
	{
		place_floor_bold(y, x);
	}
	else
	{
		cave_type *c_ptr = &cave[y][x];

		if (type == DOOR_DEFAULT)
		{
			type = ((d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
			        one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 16 : 256)) ? DOOR_CURTAIN :
			        ((d_info[dungeon_type].flags1 & DF1_GLASS_DOOR) ? DOOR_GLASS_DOOR : DOOR_DOOR);
		}

		/* Create secret door */
		place_closed_door(y, x, type);

		if (type != DOOR_CURTAIN)
		{
			/* Hide by inner wall because this is used in rooms only */
			c_ptr->mimic = feat_wall_inner;

			/* Floor type terrain cannot hide a door */
			if (feat_supports_los(c_ptr->mimic) && !feat_supports_los(c_ptr->feat))
			{
				if (have_flag(f_info[c_ptr->mimic].flags, FF_MOVE) || have_flag(f_info[c_ptr->mimic].flags, FF_CAN_FLY))
				{
					c_ptr->feat = one_in_(2) ? c_ptr->mimic : floor_type[randint0(100)];
				}
				c_ptr->mimic = 0;
			}
		}

		c_ptr->info &= ~(CAVE_FLOOR);
		delete_monster(y, x);
	}
}

/*
 * This funtion makes a very small room centred at (x0, y0)
 * This is used in crypts, and random elemental vaults.
 *
 * Note - this should be used only on allocated regions
 * within another room.
 */
static void build_small_room(int x0, int y0)
{
	int x, y;

	for (y = y0 - 1; y <= y0 + 1; y++)
	{
		place_inner_bold(y, x0 - 1);
		place_inner_bold(y, x0 + 1);
	}

	for (x = x0 - 1; x <= x0 + 1; x++)
	{
		place_inner_bold(y0 - 1, x);
		place_inner_bold(y0 + 1, x);
	}

	/* Place a secret door on one side */
	switch (randint0(4))
	{
		case 0: place_secret_door(y0, x0 - 1, DOOR_DEFAULT); break;
		case 1: place_secret_door(y0, x0 + 1, DOOR_DEFAULT); break;
		case 2: place_secret_door(y0 - 1, x0, DOOR_DEFAULT); break;
		case 3: place_secret_door(y0 + 1, x0, DOOR_DEFAULT); break;
	}

	/* Clear mimic type */
	cave[y0][x0].mimic = 0;

	/* Add inner open space */
	place_floor_bold(y0, x0);
}


/*
 * This function tunnels around a room if
 * it will cut off part of a cave system.
 */
static void check_room_boundary(int x1, int y1, int x2, int y2)
{
	int count, x, y;
	bool old_is_floor, new_is_floor;


	/* Initialize */
	count = 0;

	old_is_floor = get_is_floor(x1 - 1, y1);

	/*
	 * Count the number of floor-wall boundaries around the room
	 * Note: diagonal squares are ignored since the player can move diagonally
	 * to bypass these if needed.
	 */

	/* Above the top boundary */
	for (x = x1; x <= x2; x++)
	{
		new_is_floor = get_is_floor(x, y1 - 1);

		/* Increment counter if they are different */
		if (new_is_floor != old_is_floor) count++;

		old_is_floor = new_is_floor;
	}

	/* Right boundary */
	for (y = y1; y <= y2; y++)
	{
		new_is_floor = get_is_floor(x2 + 1, y);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor) count++;

		old_is_floor = new_is_floor;
	}

	/* Bottom boundary */
	for (x = x2; x >= x1; x--)
	{
		new_is_floor = get_is_floor(x, y2 + 1);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor) count++;

		old_is_floor = new_is_floor;
	}

	/* Left boundary */
	for (y = y2; y >= y1; y--)
	{
		new_is_floor = get_is_floor(x1 - 1, y);

		/* increment counter if they are different */
		if (new_is_floor != old_is_floor) count++;

		old_is_floor = new_is_floor;
	}

	/* If all the same, or only one connection exit. */
	if (count <= 2) return;


	/* Tunnel around the room so to prevent problems with caves */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			set_floor(x, y);
		}
	}
}


/*
 *  Helper function for find_space().
 *
 *  Is this a good location?
 */
static bool find_space_aux(int blocks_high, int blocks_wide, int block_y, int block_x)
{
	int by1, bx1, by2, bx2, by, bx;

	/* Itty-bitty rooms must shift about within their rectangle */
	if (blocks_wide < 3)
	{
		if ((blocks_wide == 2) && (block_x % 3) == 2)
			return FALSE;
	}

	/* Rooms with width divisible by 3 must be fitted to a rectangle. */
	else if ((blocks_wide % 3) == 0)
	{
		/* Must be aligned to the left edge of a 11x33 rectangle. */
		if ((block_x % 3) != 0)
			return FALSE;
	}

	/*
	 * Big rooms that do not have a width divisible by 3 must be
	 * aligned towards the edge of the dungeon closest to them.
	 */
	else
	{
		/* Shift towards left edge of dungeon. */
		if (block_x + (blocks_wide / 2) <= dun->col_rooms / 2)
		{
			if (((block_x % 3) == 2) && ((blocks_wide % 3) == 2))
				return FALSE;
			if ((block_x % 3) == 1)
				return FALSE;
		}

		/* Shift toward right edge of dungeon. */
		else
		{
			if (((block_x % 3) == 2) && ((blocks_wide % 3) == 2))
				return FALSE;
			if ((block_x % 3) == 1)
				return FALSE;
		}
	}

	/* Extract blocks */
	by1 = block_y + 0;
	bx1 = block_x + 0;
	by2 = block_y + blocks_high;
	bx2 = block_x + blocks_wide;

	/* Never run off the screen */
	if ((by1 < 0) || (by2 > dun->row_rooms)) return FALSE;
	if ((bx1 < 0) || (bx2 > dun->col_rooms)) return FALSE;

	/* Verify available space */
	for (by = by1; by < by2; by++)
	{
		for (bx = bx1; bx < bx2; bx++)
		{
			if (dun->room_map[by][bx])
			{
				return FALSE;
			}
		}
	}

	/* This location is okay */
	return TRUE;
}


/*
 * Find a good spot for the next room.  -LM-
 *
 * Find and allocate a free space in the dungeon large enough to hold
 * the room calling this function.
 *
 * We allocate space in 11x11 blocks, but want to make sure that rooms
 * align neatly on the standard screen.  Therefore, we make them use
 * blocks in few 11x33 rectangles as possible.
 *
 * Be careful to include the edges of the room in height and width!
 *
 * Return TRUE and values for the center of the room if all went well.
 * Otherwise, return FALSE.
 */
static bool find_space(int *y, int *x, int height, int width)
{
	int candidates, pick;
	int by, bx, by1, bx1, by2, bx2;
	int block_y = 0, block_x = 0;


	/* Find out how many blocks we need. */
	int blocks_high = 1 + ((height - 1) / BLOCK_HGT);
	int blocks_wide = 1 + ((width - 1) / BLOCK_WID);

	/* There are no way to allocate such huge space */
	if (dun->row_rooms < blocks_high) return FALSE;
	if (dun->col_rooms < blocks_wide) return FALSE;

	/* Initiallize */
	candidates = 0;

	/* Count the number of valid places */
	for (block_y = dun->row_rooms - blocks_high; block_y >= 0; block_y--)
	{
		for (block_x = dun->col_rooms - blocks_wide; block_x >= 0; block_x--)
		{
			if (find_space_aux(blocks_high, blocks_wide, block_y, block_x))
			{
				/* Find a valid place */
				candidates++;
			}
		}
	}

	/* No place! */
	if (!candidates)
	{
		return FALSE;
	}

	/* Normal dungeon */
	if (!(d_info[dungeon_type].flags1 & DF1_NO_CAVE))
	{
		/* Choose a random one */
		pick = randint1(candidates);
	}

	/* NO_CAVE dungeon (Castle) */
	else
	{
		/* Always choose the center one */
		pick = candidates/2 + 1;
	}

	/* Pick up the choosen location */
	for (block_y = dun->row_rooms - blocks_high; block_y >= 0; block_y--)
	{
		for (block_x = dun->col_rooms - blocks_wide; block_x >= 0; block_x--)
		{
			if (find_space_aux(blocks_high, blocks_wide, block_y, block_x))
			{
				pick--;

				/* This one is picked? */
				if (!pick) break;
			}
		}

		if (!pick) break;
	}

	/* Extract blocks */
	by1 = block_y + 0;
	bx1 = block_x + 0;
	by2 = block_y + blocks_high;
	bx2 = block_x + blocks_wide;

	/*
	 * It is *extremely* important that the following calculation
	 * be *exactly* correct to prevent memory errors
	 */

	/* Acquire the location of the room */
	(*y) = ((by1 + by2) * BLOCK_HGT) / 2;
	(*x) = ((bx1 + bx2) * BLOCK_WID) / 2;

	/* Save the room location */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = *y;
		dun->cent[dun->cent_n].x = *x;
		dun->cent_n++;
	}

	/* Reserve some blocks. */
	for (by = by1; by < by2; by++)
	{
		for (bx = bx1; bx < bx2; bx++)
		{
			//v2.1.5 よく分からんがコンパイラが警告を出しているので例外処理を入れておく
			if (by < 0 || bx < 0) return FALSE;

			dun->room_map[by][bx] = TRUE;
		}
	}


	/*
	 * Hack- See if room will cut off a cavern.
	 *
	 * If so, fix by tunneling outside the room in such a
	 * way as to connect the caves.
	 */
	check_room_boundary(*x - width / 2 - 1, *y - height / 2 - 1, *x + (width - 1) / 2 + 1, *y + (height - 1) / 2 + 1);


	/* Success. */
	return TRUE;
}



/*
 * Room building routines.
 *
 * Room types:
 *   1 -- normal
 *   2 -- overlapping
 *   3 -- cross shaped
 *   4 -- large room with features
 *   5 -- monster nests
 *   6 -- monster pits
 *   7 -- simple vaults
 *   8 -- greater vaults
 *   9 -- fractal caves
 *  10 -- random vaults
 *  11 -- circular rooms
 *  12 -- crypts
 *  13 -- trapped monster pits
 *  14 -- trapped room
 *  15 -- glass room
 *  16 -- underground arcade
 */


/*
 * Type 1 -- normal rectangular rooms
 */
static bool build_type1(void)
{
	int y, x, y2, x2, yval, xval;
	int y1, x1, xsize, ysize;

	bool light;

	cave_type *c_ptr;

	bool curtain = (d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
		one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 48 : 512);

	/* Pick a room size */
	y1 = randint1(4);
	x1 = randint1(11);
	y2 = randint1(3);
	x2 = randint1(11);

	xsize = x1 + x2 + 1;
	ysize = y1 + y2 + 1;

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, ysize + 2, xsize + 2))
	{
		/* Limit to the minimum room size, and retry */
		y1 = 1;
		x1 = 1;
		y2 = 1;
		x2 = 1;

		xsize = x1 + x2 + 1;
		ysize = y1 + y2 + 1;

		/* Find and reserve some space in the dungeon.  Get center of room. */
		if (!find_space(&yval, &xval, ysize + 2, xsize + 2)) return FALSE;
	}

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));


	/* Get corner values */
	y1 = yval - ysize / 2;
	x1 = xval - xsize / 2;
	y2 = yval + (ysize - 1) / 2;
	x2 = xval + (xsize - 1) / 2;


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}


	/* Hack -- Occasional curtained room */
	if (curtain && (y2 - y1 > 2) && (x2 - x1 > 2))
	{
		for (y = y1; y <= y2; y++)
		{
			c_ptr = &cave[y][x1];
			c_ptr->feat = feat_door[DOOR_CURTAIN].closed;
			c_ptr->info &= ~(CAVE_MASK);
			c_ptr = &cave[y][x2];
			c_ptr->feat = feat_door[DOOR_CURTAIN].closed;
			c_ptr->info &= ~(CAVE_MASK);
		}
		for (x = x1; x <= x2; x++)
		{
			c_ptr = &cave[y1][x];
			c_ptr->feat = feat_door[DOOR_CURTAIN].closed;
			c_ptr->info &= ~(CAVE_MASK);
			c_ptr = &cave[y2][x];
			c_ptr->feat = feat_door[DOOR_CURTAIN].closed;
			c_ptr->info &= ~(CAVE_MASK);
		}
	}


	/* Hack -- Occasional pillar room */
	if (one_in_(20))
	{
		for (y = y1; y <= y2; y += 2)
		{
			for (x = x1; x <= x2; x += 2)
			{
				c_ptr = &cave[y][x];
				place_inner_grid(c_ptr);
			}
		}
	}

	/* Hack -- Occasional room with four pillars */
	else if (one_in_(20))
	{
		if ((y1 + 4 < y2) && (x1 + 4 < x2))
		{
			c_ptr = &cave[y1 + 1][x1 + 1];
			place_inner_grid(c_ptr);

			c_ptr = &cave[y1 + 1][x2 - 1];
			place_inner_grid(c_ptr);

			c_ptr = &cave[y2 - 1][x1 + 1];
			place_inner_grid(c_ptr);

			c_ptr = &cave[y2 - 1][x2 - 1];
			place_inner_grid(c_ptr);
		}
	}

	/* Hack -- Occasional ragged-edge room */
	else if (one_in_(50))
	{
		for (y = y1 + 2; y <= y2 - 2; y += 2)
		{
			c_ptr = &cave[y][x1];
			place_inner_grid(c_ptr);
			c_ptr = &cave[y][x2];
			place_inner_grid(c_ptr);
		}
		for (x = x1 + 2; x <= x2 - 2; x += 2)
		{
			c_ptr = &cave[y1][x];
			place_inner_grid(c_ptr);
			c_ptr = &cave[y2][x];
			place_inner_grid(c_ptr);
		}
	}
	/* Hack -- Occasional divided room */
	else if (one_in_(50))
	{
		bool curtain2 = (d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
			one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 2 : 128);

		if (randint1(100) < 50)
		{
			/* Horizontal wall */
			for (x = x1; x <= x2; x++)
			{
				place_inner_bold(yval, x);
				if (curtain2) cave[yval][x].feat = feat_door[DOOR_CURTAIN].closed;
			}

			/* Prevent edge of wall from being tunneled */
			place_solid_bold(yval, x1 - 1);
			place_solid_bold(yval, x2 + 1);
		}
		else
		{
			/* Vertical wall */
			for (y = y1; y <= y2; y++)
			{
				place_inner_bold(y, xval);
				if (curtain2) cave[y][xval].feat = feat_door[DOOR_CURTAIN].closed;
			}

			/* Prevent edge of wall from being tunneled */
			place_solid_bold(y1 - 1, xval);
			place_solid_bold(y2 + 1, xval);
		}

		place_random_door(yval, xval, TRUE);
		if (curtain2) cave[yval][xval].feat = feat_door[DOOR_CURTAIN].closed;
	}

	return TRUE;
}


/*
 * Type 2 -- Overlapping rectangular rooms
 */
static bool build_type2(void)
{
	int			y, x, xval, yval;
	int			y1a, x1a, y2a, x2a;
	int			y1b, x1b, y2b, x2b;
	bool		light;
	cave_type   *c_ptr;

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 11, 25)) return FALSE;

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));

	/* Determine extents of the first room */
	y1a = yval - randint1(4);
	y2a = yval + randint1(3);
	x1a = xval - randint1(11);
	x2a = xval + randint1(10);

	/* Determine extents of the second room */
	y1b = yval - randint1(3);
	y2b = yval + randint1(4);
	x1b = xval - randint1(10);
	x2b = xval + randint1(11);


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2a + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2a + 1][x];
		place_outer_grid(c_ptr);
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2b + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2b + 1][x];
		place_outer_grid(c_ptr);
	}



	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
		}
	}

	return TRUE;
}



/*
 * Type 3 -- Cross shaped rooms
 *
 * Builds a room at a row, column coordinate
 *
 * Room "a" runs north/south, and Room "b" runs east/east
 * So the "central pillar" runs from x1a, y1b to x2a, y2b.
 *
 * Note that currently, the "center" is always 3x3, but I think that
 * the code below will work (with "bounds checking") for 5x5, or even
 * for unsymetric values like 4x3 or 5x3 or 3x4 or 3x5, or even larger.
 */
static bool build_type3(void)
{
	int			y, x, dy, dx, wy, wx;
	int			y1a, x1a, y2a, x2a;
	int			y1b, x1b, y2b, x2b;
	int			yval, xval;
	bool		light;
	cave_type   *c_ptr;


	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 11, 25)) return FALSE;


	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));

	/* For now, always 3x3 */
	wx = wy = 1;

	/* Pick max vertical size (at most 4) */
	dy = rand_range(3, 4);

	/* Pick max horizontal size (at most 15) */
	dx = rand_range(3, 11);


	/* Determine extents of the north/south room */
	y1a = yval - dy;
	y2a = yval + dy;
	x1a = xval - wx;
	x2a = xval + wx;

	/* Determine extents of the east/west room */
	y1b = yval - wy;
	y2b = yval + wy;
	x1b = xval - dx;
	x2b = xval + dx;


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		c_ptr = &cave[y][x1a - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2a + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		c_ptr = &cave[y1a - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2a + 1][x];
		place_outer_grid(c_ptr);
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		c_ptr = &cave[y][x1b - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2b + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		c_ptr = &cave[y1b - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2b + 1][x];
		place_outer_grid(c_ptr);
	}


	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
		}
	}



	/* Special features (3/4) */
	switch (randint0(4))
	{
		/* Large solid middle pillar */
		case 1:
		{
			for (y = y1b; y <= y2b; y++)
			{
				for (x = x1a; x <= x2a; x++)
				{
					c_ptr = &cave[y][x];
					place_inner_grid(c_ptr);
				}
			}
			break;
		}

		/* Inner treasure vault */
		case 2:
		{
			/* Build the vault */
			for (y = y1b; y <= y2b; y++)
			{
				c_ptr = &cave[y][x1a];
				place_inner_grid(c_ptr);
				c_ptr = &cave[y][x2a];
				place_inner_grid(c_ptr);
			}
			for (x = x1a; x <= x2a; x++)
			{
				c_ptr = &cave[y1b][x];
				place_inner_grid(c_ptr);
				c_ptr = &cave[y2b][x];
				place_inner_grid(c_ptr);
			}

			/* Place a secret door on the inner room */
			switch (randint0(4))
			{
				case 0: place_secret_door(y1b, xval, DOOR_DEFAULT); break;
				case 1: place_secret_door(y2b, xval, DOOR_DEFAULT); break;
				case 2: place_secret_door(yval, x1a, DOOR_DEFAULT); break;
				case 3: place_secret_door(yval, x2a, DOOR_DEFAULT); break;
			}

			/* Place a treasure in the vault */
			place_object(yval, xval, 0L);

			/* Let's guard the treasure well */
			vault_monsters(yval, xval, randint0(2) + 3);

			/* Traps naturally */
			vault_traps(yval, xval, 4, 4, randint0(3) + 2);

			break;
		}

		/* Something else */
		case 3:
		{
			/* Occasionally pinch the center shut */
			if (one_in_(3))
			{
				/* Pinch the east/west sides */
				for (y = y1b; y <= y2b; y++)
				{
					if (y == yval) continue;
					c_ptr = &cave[y][x1a - 1];
					place_inner_grid(c_ptr);
					c_ptr = &cave[y][x2a + 1];
					place_inner_grid(c_ptr);
				}

				/* Pinch the north/south sides */
				for (x = x1a; x <= x2a; x++)
				{
					if (x == xval) continue;
					c_ptr = &cave[y1b - 1][x];
					place_inner_grid(c_ptr);
					c_ptr = &cave[y2b + 1][x];
					place_inner_grid(c_ptr);
				}

				/* Sometimes shut using secret doors */
				if (one_in_(3))
				{
					int door_type = ((d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
						one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 16 : 256)) ? DOOR_CURTAIN :
						((d_info[dungeon_type].flags1 & DF1_GLASS_DOOR) ? DOOR_GLASS_DOOR : DOOR_DOOR);

					place_secret_door(yval, x1a - 1, door_type);
					place_secret_door(yval, x2a + 1, door_type);
					place_secret_door(y1b - 1, xval, door_type);
					place_secret_door(y2b + 1, xval, door_type);
				}
			}

			/* Occasionally put a "plus" in the center */
			else if (one_in_(3))
			{
				c_ptr = &cave[yval][xval];
				place_inner_grid(c_ptr);
				c_ptr = &cave[y1b][xval];
				place_inner_grid(c_ptr);
				c_ptr = &cave[y2b][xval];
				place_inner_grid(c_ptr);
				c_ptr = &cave[yval][x1a];
				place_inner_grid(c_ptr);
				c_ptr = &cave[yval][x2a];
				place_inner_grid(c_ptr);
			}

			/* Occasionally put a pillar in the center */
			else if (one_in_(3))
			{
				c_ptr = &cave[yval][xval];
				place_inner_grid(c_ptr);
			}

			break;
		}
	}

	return TRUE;
}


/*
 * Type 4 -- Large room with inner features
 *
 * Possible sub-types:
 *	1 - Just an inner room with one door
 *	2 - An inner room within an inner room
 *	3 - An inner room with pillar(s)
 *	4 - Inner room has a maze
 *	5 - A set of four inner rooms
 */
static bool build_type4(void)
{
	int         y, x, y1, x1;
	int         y2, x2, tmp, yval, xval;
	bool        light;
	cave_type   *c_ptr;


	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 11, 25)) return FALSE;

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Outer Walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}


	/* The inner room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_inner_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_inner_grid(c_ptr);
	}


	/* Inner room variations */
	switch (randint1(5))
	{
		/* Just an inner room with a monster */
		case 1:
		{
			/* Place a secret door */
			switch (randint1(4))
			{
				case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
				case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
				case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
				case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
			}

			/* Place a monster in the room */
			vault_monsters(yval, xval, 1);

			break;
		}

		/* Treasure Vault (with a door) */
		case 2:
		{
			/* Place a secret door */
			switch (randint1(4))
			{
				case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
				case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
				case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
				case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
			}

			/* Place another inner room */
			for (y = yval - 1; y <= yval + 1; y++)
			{
				for (x = xval -  1; x <= xval + 1; x++)
				{
					if ((x == xval) && (y == yval)) continue;
					c_ptr = &cave[y][x];
					place_inner_grid(c_ptr);
				}
			}

			/* Place a locked door on the inner room */
			switch (randint1(4))
			{
				case 1: place_locked_door(yval - 1, xval); break;
				case 2: place_locked_door(yval + 1, xval); break;
				case 3: place_locked_door(yval, xval - 1); break;
				case 4: place_locked_door(yval, xval + 1); break;
			}

			/* Monsters to guard the "treasure" */
			vault_monsters(yval, xval, randint1(3) + 2);

			/* Object (80%) */
			if (randint0(100) < 80)
			{
				place_object(yval, xval, 0L);
			}

			/* Stairs (20%) */
			else
			{
				place_random_stairs(yval, xval);
			}

			/* Traps to protect the treasure */
			vault_traps(yval, xval, 4, 10, 2 + randint1(3));

			break;
		}

		/* Inner pillar(s). */
		case 3:
		{
			/* Place a secret door */
			switch (randint1(4))
			{
				case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
				case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
				case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
				case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
			}

			/* Large Inner Pillar */
			for (y = yval - 1; y <= yval + 1; y++)
			{
				for (x = xval - 1; x <= xval + 1; x++)
				{
				c_ptr = &cave[y][x];
				place_inner_grid(c_ptr);
				}
			}

			/* Occasionally, two more Large Inner Pillars */
			if (one_in_(2))
			{
				tmp = randint1(2);
				for (y = yval - 1; y <= yval + 1; y++)
				{
					for (x = xval - 5 - tmp; x <= xval - 3 - tmp; x++)
					{
					c_ptr = &cave[y][x];
					place_inner_grid(c_ptr);
					}
					for (x = xval + 3 + tmp; x <= xval + 5 + tmp; x++)
					{
					c_ptr = &cave[y][x];
					place_inner_grid(c_ptr);
					}
				}
			}

			/* Occasionally, some Inner rooms */
			if (one_in_(3))
			{
				int door_type = ((d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
					one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 16 : 256)) ? DOOR_CURTAIN :
					((d_info[dungeon_type].flags1 & DF1_GLASS_DOOR) ? DOOR_GLASS_DOOR : DOOR_DOOR);

				/* Long horizontal walls */
				for (x = xval - 5; x <= xval + 5; x++)
				{
				c_ptr = &cave[yval - 1][x];
				place_inner_grid(c_ptr);
				c_ptr = &cave[yval + 1][x];
				place_inner_grid(c_ptr);
				}

				/* Close off the left/right edges */
				c_ptr = &cave[yval][xval - 5];
				place_inner_grid(c_ptr);
				c_ptr = &cave[yval][xval + 5];
				place_inner_grid(c_ptr);

				/* Secret doors (random top/bottom) */
				place_secret_door(yval - 3 + (randint1(2) * 2), xval - 3, door_type);
				place_secret_door(yval - 3 + (randint1(2) * 2), xval + 3, door_type);

				/* Monsters */
				vault_monsters(yval, xval - 2, randint1(2));
				vault_monsters(yval, xval + 2, randint1(2));

				/* Objects */
				if (one_in_(3)) place_object(yval, xval - 2, 0L);
				if (one_in_(3)) place_object(yval, xval + 2, 0L);
			}

			break;
		}

		/* Maze inside. */
		case 4:
		{
			/* Place a secret door */
			switch (randint1(4))
			{
				case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
				case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
				case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
				case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
			}

			/* Maze (really a checkerboard) */
			for (y = y1; y <= y2; y++)
			{
				for (x = x1; x <= x2; x++)
				{
					if (0x1 & (x + y))
					{
						c_ptr = &cave[y][x];
						place_inner_grid(c_ptr);
					}
				}
			}

			/* Monsters just love mazes. */
			vault_monsters(yval, xval - 5, randint1(3));
			vault_monsters(yval, xval + 5, randint1(3));

			/* Traps make them entertaining. */
			vault_traps(yval, xval - 3, 2, 8, randint1(3));
			vault_traps(yval, xval + 3, 2, 8, randint1(3));

			/* Mazes should have some treasure too. */
			vault_objects(yval, xval, 3);

			break;
		}

		/* Four small rooms. */
		case 5:
		{
			int door_type = ((d_info[dungeon_type].flags1 & DF1_CURTAIN) &&
				one_in_((d_info[dungeon_type].flags1 & DF1_NO_CAVE) ? 16 : 256)) ? DOOR_CURTAIN :
				((d_info[dungeon_type].flags1 & DF1_GLASS_DOOR) ? DOOR_GLASS_DOOR : DOOR_DOOR);

			/* Inner "cross" */
			for (y = y1; y <= y2; y++)
			{
				c_ptr = &cave[y][xval];
				place_inner_grid(c_ptr);
			}
			for (x = x1; x <= x2; x++)
			{
				c_ptr = &cave[yval][x];
				place_inner_grid(c_ptr);
			}

			/* Doors into the rooms */
			if (randint0(100) < 50)
			{
				int i = randint1(10);
				place_secret_door(y1 - 1, xval - i, door_type);
				place_secret_door(y1 - 1, xval + i, door_type);
				place_secret_door(y2 + 1, xval - i, door_type);
				place_secret_door(y2 + 1, xval + i, door_type);
			}
			else
			{
				int i = randint1(3);
				place_secret_door(yval + i, x1 - 1, door_type);
				place_secret_door(yval - i, x1 - 1, door_type);
				place_secret_door(yval + i, x2 + 1, door_type);
				place_secret_door(yval - i, x2 + 1, door_type);
			}

			/* Treasure, centered at the center of the cross */
			vault_objects(yval, xval, 2 + randint1(2));

			/* Gotta have some monsters. */
			vault_monsters(yval + 1, xval - 4, randint1(4));
			vault_monsters(yval + 1, xval + 4, randint1(4));
			vault_monsters(yval - 1, xval - 4, randint1(4));
			vault_monsters(yval - 1, xval + 4, randint1(4));

			break;
		}
	}

	return TRUE;
}


/*
 * The following functions are used to determine if the given monster
 * is appropriate for inclusion in a monster nest or monster pit or
 * the given type.
 *
 * None of the pits/nests are allowed to include "unique" monsters.
 */


/*
 * Monster validation macro
 *
 * Line 1 -- forbid town monsters
 * Line 2 -- forbid uniques
 * Line 3 -- forbid aquatic monsters
 */
///mod140929 視界外隣接テレポを使う敵がPITやNESTに出ないようにした
///mod141026 どうもここがpitにユニークが出るバグの原因としか思えないので普通の関数にしてしまう・・が、改善せず。
/*
#define vault_monster_okay(I) \
	(mon_hook_dungeon(I) && \
	 !(r_info[I].flags1 & RF1_UNIQUE) && \
	 !(r_info[I].flags7 & RF7_UNIQUE2) && \
	 !(r_info[I].flagsr & RFR_RES_ALL) && \
	 !(r_info[I].flags9 & RF9_TELE_HI_APPROACH) && \
	 !(r_info[I].flags7 & RF7_AQUATIC))
*/

bool vault_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if(!mon_hook_dungeon(r_idx)) return FALSE;
	if(r_ptr->flags1 & RF1_UNIQUE) return FALSE;
	if(r_ptr->flags7 & RF7_UNIQUE2) return FALSE;
	if(r_ptr->flagsr & RFR_RES_ALL) return FALSE;
	if(r_ptr->flags9 & RF9_TELE_HI_APPROACH) return FALSE;
	if(r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	return TRUE;
}


/* Race index for "monster pit (clone)" */
static int vault_aux_race;

/* Race index for "monster pit (symbol clone)" */
static char vault_aux_char;

/* Breath mask for "monster pit (dragon)" */
static u32b vault_aux_dragon_mask4;


/*
 * Helper monster selection function
 */
static bool vault_aux_simple(int r_idx)
{
	/* Okay */
	return (vault_monster_okay(r_idx));
}


/*
 * Helper function for "monster nest (jelly)"
 */
static bool vault_aux_jelly(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if ((r_ptr->flags2 & RF2_KILL_BODY) && !(r_ptr->flags1 & RF1_NEVER_BLOW)) return (FALSE);

	/* Also decline evil jellies (like death molds and shoggoths) */
	if (r_ptr->flags3 & (RF3_ANG_CHAOS)) return (FALSE);

	/* Require icky thing, jelly, mold, or mushroom */
	if (!my_strchr("ijm,", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (animal)"
 */
static bool vault_aux_animal(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require "animal" flag */
	if (!(r_ptr->flags3 & (RF3_ANIMAL))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (undead)"
 */
static bool vault_aux_undead(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require Undead */
	if (!(r_ptr->flags3 & (RF3_UNDEAD))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (chapel)"
 */
static bool vault_aux_chapel_g(int r_idx)
{
	static int chapel_list[] = {
		MON_NOV_PRIEST, MON_NOV_PALADIN, MON_NOV_PRIEST_G, MON_NOV_PALADIN_G,
		MON_PRIEST, MON_JADE_MONK, MON_IVORY_MONK, MON_ULTRA_PALADIN,
		MON_EBONY_MONK, MON_W_KNIGHT, MON_KNI_TEMPLAR, MON_PALADIN,
		MON_TOPAZ_MONK, 0};

	int i;

	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if (r_ptr->flags3 & (RF3_ANG_CHAOS)) return (FALSE);
	if ((r_idx == MON_A_GOLD) || (r_idx == MON_A_SILVER)) return (FALSE);

	/* Require "priest" or Angel */

	if (r_ptr->d_char == 'A') return TRUE;

	for (i = 0; chapel_list[i]; i++)
		if (r_idx == chapel_list[i]) return TRUE;

	return FALSE;
}


/*
 * Helper function for "monster nest (kennel)"
 */
static bool vault_aux_kennel(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require a Zephyr Hound or a dog */
	if (!my_strchr("CZ", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (mimic)"
 */
static bool vault_aux_mimic(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require mimic */
	if (!my_strchr("!$&(/=?[\\|", r_ptr->d_char)) return (FALSE);

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (clone)"
 */
static bool vault_aux_clone(int r_idx)
{
	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	return (r_idx == vault_aux_race);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol_e(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if ((r_ptr->flags2 & RF2_KILL_BODY) && !(r_ptr->flags1 & RF1_NEVER_BLOW)) return (FALSE);

	if (r_ptr->flags3 & (RF3_ANG_COSMOS)) return (FALSE);

	/* Decline incorrect symbol */
	if (r_ptr->d_char != vault_aux_char) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol_g(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if ((r_ptr->flags2 & RF2_KILL_BODY) && !(r_ptr->flags1 & RF1_NEVER_BLOW)) return (FALSE);

	if (r_ptr->flags3 & (RF3_ANG_CHAOS)) return (FALSE);

	/* Decline incorrect symbol */
	if (r_ptr->d_char != vault_aux_char) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (orc)"
 */
/*:::オークPITを作るための判定関数らしい。*/
///mod131231 オークPIT補助関数　とりあえず妖怪に置き換えてみる
static bool vault_aux_orc(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require orc */
	if (!(r_ptr->flags3 & RF3_KWAI)) return (FALSE);

	/* Decline undead */
	//if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (troll)"
 */
/*:::トロルピット生成用判定関数　*/
//mod131231 トロルピット用関数　とりあえずそのまま全て妖精に置き換えた
static bool vault_aux_troll(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require troll */
	if (!(r_ptr->flags3 & RF3_FAIRY)) return (FALSE);

	/* Decline undead */
	//if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (giant)"
 */
///mod131231 巨人PIT生成用　混沌勢力のみにしておいた
static bool vault_aux_giant(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require giant */
	if (!(r_ptr->flags3 & RF3_DEMIHUMAN)) return (FALSE);

	if (!(r_ptr->flags3 & RF3_ANG_CHAOS)) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (dragon)"
 */
static bool vault_aux_dragon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* Require dragon */
	if (!(r_ptr->flags3 & RF3_DRAGON)) return (FALSE);

	/* Hack -- Require correct "breath attack" */
	if (r_ptr->flags4 != vault_aux_dragon_mask4) return (FALSE);

	/* Decline undead */
	if (r_ptr->flags3 & RF3_UNDEAD) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (demon)"
 */
static bool vault_aux_demon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if ((r_ptr->flags2 & RF2_KILL_BODY) && !(r_ptr->flags1 & RF1_NEVER_BLOW)) return (FALSE);

	/* Require demon */
	if (!(r_ptr->flags3 & RF3_DEMON)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (lovecraftian)"
 */
static bool vault_aux_cthulhu(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	if ((r_ptr->flags2 & RF2_KILL_BODY) && !(r_ptr->flags1 & RF1_NEVER_BLOW)) return (FALSE);

	/* Require eldritch horror */
	if (!(r_ptr->flags2 & (RF2_ELDRITCH_HORROR))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (clone)"
 */
static void vault_prep_clone(void)
{
	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple, NULL);

	/* Pick a race to clone */
	vault_aux_race = get_mon_num(dun_level + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL, NULL);
}


/*
 * Helper function for "monster pit (symbol clone)"
 */
static void vault_prep_symbol(void)
{
	int r_idx;

	/* Apply the monster restriction */
	get_mon_num_prep(vault_aux_simple, NULL);

	/* Pick a race to clone */
	r_idx = get_mon_num(dun_level + 10);

	/* Remove the monster restriction */
	get_mon_num_prep(NULL, NULL);

	/* Extract the symbol */
	vault_aux_char = r_info[r_idx].d_char;
}


/*
 * Helper function for "monster pit (dragon)"
 */
static void vault_prep_dragon(void)
{
	/* Pick dragon type */
	switch (randint0(6))
	{
		/* Black */
		case 0:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ACID;

			/* Done */
			break;
		}

		/* Blue */
		case 1:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_ELEC;

			/* Done */
			break;
		}

		/* Red */
		case 2:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_FIRE;

			/* Done */
			break;
		}

		/* White */
		case 3:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_COLD;

			/* Done */
			break;
		}

		/* Green */
		case 4:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = RF4_BR_POIS;

			/* Done */
			break;
		}

		/* Multi-hued */
		default:
		{
			/* Restrict dragon breath type */
			vault_aux_dragon_mask4 = (RF4_BR_ACID | RF4_BR_ELEC |
											  RF4_BR_FIRE | RF4_BR_COLD |
											  RF4_BR_POIS);

			/* Done */
			break;
		}
	}
}


/*
 * Helper function for "monster pit (dark elf)"
 */
static bool vault_aux_dark_elf(int r_idx)
{
	int i;
	static int dark_elf_list[] =
	{
		MON_D_ELF, MON_D_ELF_MAGE, MON_D_ELF_WARRIOR, MON_D_ELF_PRIEST,
		MON_D_ELF_LORD, MON_D_ELF_WARLOCK, MON_D_ELF_DRUID, MON_NIGHTBLADE,
		MON_D_ELF_SORC, MON_D_ELF_SHADE, 0,
	};

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return FALSE;

	/* Require dark elves */
	for (i = 0; dark_elf_list[i]; i++)
		if (r_idx == dark_elf_list[i]) return TRUE;

	/* Assume not */
	return FALSE;
}


typedef struct vault_aux_type vault_aux_type;


struct vault_aux_type
{
	cptr name;
	bool (*hook_func)(int r_idx);
	void (*prep_func)(void);
	int level;
	int chance;
};


static int pick_vault_type(vault_aux_type *l_ptr, s16b allow_flag_mask)
{
	int tmp, total, count;

	vault_aux_type *n_ptr;

	/* Calculate the total possibilities */
	for (n_ptr = l_ptr, total = 0, count = 0; TRUE; n_ptr++, count++)
	{
		/* Note end */
		if (!n_ptr->name) break;

		/* Ignore excessive depth */
		if (n_ptr->level > dun_level) continue;

		/* Not matched with pit/nest flag */
		if (!(allow_flag_mask & (1L << count))) continue;

		/* Count this possibility */
		total += n_ptr->chance * MAX_DEPTH / (MIN(dun_level, MAX_DEPTH - 1) - n_ptr->level + 5);
	}

	/* Pick a random type */
	tmp = randint0(total);

	/* Find this type */
	for (n_ptr = l_ptr, total = 0, count = 0; TRUE; n_ptr++, count++)
	{
		/* Note end */
		if (!n_ptr->name) break;

		/* Ignore excessive depth */
		if (n_ptr->level > dun_level) continue;

		/* Not matched with pit/nest flag */
		if (!(allow_flag_mask & (1L << count))) continue;

		/* Count this possibility */
		total += n_ptr->chance * MAX_DEPTH / (MIN(dun_level, MAX_DEPTH - 1) - n_ptr->level + 5);

		/* Found the type */
		if (tmp < total) break;
	}

	return n_ptr->name ? count : -1;
}

static vault_aux_type nest_types[] =
{
#ifdef JP
	{"クローン",     vault_aux_clone,    vault_prep_clone,   5, 3},
	{"ゼリー",       vault_aux_jelly,    NULL,               5, 6},
	{"シンボル(善)", vault_aux_symbol_g, vault_prep_symbol, 25, 2},
	{"シンボル(悪)", vault_aux_symbol_e, vault_prep_symbol, 25, 2},
	{"ミミック",     vault_aux_mimic,    NULL,              30, 4},
	{"狂気",         vault_aux_cthulhu,  NULL,              70, 2},
	{"犬小屋",       vault_aux_kennel,   NULL,              45, 4},
	{"動物園",       vault_aux_animal,   NULL,              35, 5},
	{"教会",         vault_aux_chapel_g, NULL,              75, 4},
	{"アンデッド",   vault_aux_undead,   NULL,              75, 5},
	{NULL,           NULL,               NULL,               0, 0},
#else
	{"clone",        vault_aux_clone,    vault_prep_clone,   5, 3},
	{"jelly",        vault_aux_jelly,    NULL,               5, 6},
	{"symbol good",  vault_aux_symbol_g, vault_prep_symbol, 25, 2},
	{"symbol evil",  vault_aux_symbol_e, vault_prep_symbol, 25, 2},
	{"mimic",        vault_aux_mimic,    NULL,              30, 4},
	{"lovecraftian", vault_aux_cthulhu,  NULL,              70, 2},
	{"kennel",       vault_aux_kennel,   NULL,              45, 4},
	{"animal",       vault_aux_animal,   NULL,              35, 5},
	{"chapel",       vault_aux_chapel_g, NULL,              75, 4},
	{"undead",       vault_aux_undead,   NULL,              75, 5},
	{NULL,           NULL,               NULL,               0, 0},
#endif
};

static vault_aux_type pit_types[] =
{
#ifdef JP
	{"妖怪",       vault_aux_orc,      NULL,               5, 6},
	{"妖精",       vault_aux_troll,    NULL,              20, 6},
	{"人型生物", vault_aux_giant,    NULL,              50, 6},
	{"狂気",         vault_aux_cthulhu,  NULL,              80, 2},
	{"シンボル(善)", vault_aux_symbol_g, vault_prep_symbol, 70, 1},
	{"シンボル(悪)", vault_aux_symbol_e, vault_prep_symbol, 70, 1},
	{"教会",         vault_aux_chapel_g, NULL,              65, 2},
	{"ドラゴン",     vault_aux_dragon,   vault_prep_dragon, 70, 6},
	{"デーモン",     vault_aux_demon,    NULL,              80, 6},
	{"ダークエルフ", vault_aux_dark_elf, NULL,              45, 4},
	{NULL,           NULL,               NULL,               0, 0},
#else
	{"youkai",       vault_aux_orc,      NULL,               5, 6},
	{"fairy",        vault_aux_troll,    NULL,              20, 6},
	{"humanoid",     vault_aux_giant,    NULL,              50, 6},
	{"lovecraftian", vault_aux_cthulhu,  NULL,              80, 2},
	{"symbol good",  vault_aux_symbol_g, vault_prep_symbol, 70, 1},
	{"symbol evil",  vault_aux_symbol_e, vault_prep_symbol, 70, 1},
	{"chapel",       vault_aux_chapel_g, NULL,              65, 2},
	{"dragon",       vault_aux_dragon,   vault_prep_dragon, 70, 6},
	{"demon",        vault_aux_demon,    NULL,              80, 6},
	{"dark elf",     vault_aux_dark_elf, NULL,              45, 4},
	{NULL,           NULL,               NULL,               0, 0},
#endif
};


/* Nest types code */
#define NEST_TYPE_CLONE        0
#define NEST_TYPE_JELLY        1
#define NEST_TYPE_SYMBOL_GOOD  2
#define NEST_TYPE_SYMBOL_EVIL  3
#define NEST_TYPE_MIMIC        4
#define NEST_TYPE_LOVECRAFTIAN 5
#define NEST_TYPE_KENNEL       6
#define NEST_TYPE_ANIMAL       7
#define NEST_TYPE_CHAPEL       8
#define NEST_TYPE_UNDEAD       9

/* Pit types code */
#define PIT_TYPE_ORC           0
#define PIT_TYPE_TROLL         1
#define PIT_TYPE_GIANT         2
#define PIT_TYPE_LOVECRAFTIAN  3
#define PIT_TYPE_SYMBOL_GOOD   4
#define PIT_TYPE_SYMBOL_EVIL   5
#define PIT_TYPE_CHAPEL        6
#define PIT_TYPE_DRAGON        7
#define PIT_TYPE_DEMON         8
#define PIT_TYPE_DARK_ELF      9


/*
 * Hack -- Get the string describing subtype of pit/nest
 * Determined in prepare function (some pit/nest only)
 */
static cptr pit_subtype_string(int type, bool nest)
{
	static char inner_buf[256] = "";

	inner_buf[0] = '\0'; /* Init string */

	if (nest) /* Nests */
	{
		switch (type)
		{
		case NEST_TYPE_CLONE:
			sprintf(inner_buf, "(%s)", r_name + r_info[vault_aux_race].name);
			break;
		case NEST_TYPE_SYMBOL_GOOD:
		case NEST_TYPE_SYMBOL_EVIL:
			sprintf(inner_buf, "(%c)", vault_aux_char);
			break;
		}
	}
	else /* Pits */
	{
		switch (type)
		{
		case PIT_TYPE_SYMBOL_GOOD:
		case PIT_TYPE_SYMBOL_EVIL:
			sprintf(inner_buf, "(%c)", vault_aux_char);
			break;
		case PIT_TYPE_DRAGON:
			switch (vault_aux_dragon_mask4)
			{
#ifdef JP
			case RF4_BR_ACID: strcpy(inner_buf, "(酸)");   break;
			case RF4_BR_ELEC: strcpy(inner_buf, "(稲妻)"); break;
			case RF4_BR_FIRE: strcpy(inner_buf, "(火炎)"); break;
			case RF4_BR_COLD: strcpy(inner_buf, "(冷気)"); break;
			case RF4_BR_POIS: strcpy(inner_buf, "(毒)");   break;
			case (RF4_BR_ACID | RF4_BR_ELEC | RF4_BR_FIRE | RF4_BR_COLD | RF4_BR_POIS):
				strcpy(inner_buf, "(万色)"); break;
			default: strcpy(inner_buf, "(未定義)"); break;
#else
			case RF4_BR_ACID: strcpy(inner_buf, "(acid)");      break;
			case RF4_BR_ELEC: strcpy(inner_buf, "(lightning)"); break;
			case RF4_BR_FIRE: strcpy(inner_buf, "(fire)");      break;
			case RF4_BR_COLD: strcpy(inner_buf, "(frost)");     break;
			case RF4_BR_POIS: strcpy(inner_buf, "(poison)");    break;
			case (RF4_BR_ACID | RF4_BR_ELEC | RF4_BR_FIRE | RF4_BR_COLD | RF4_BR_POIS):
				strcpy(inner_buf, "(multi-hued)"); break;
			default: strcpy(inner_buf, "(undefined)"); break;
#endif
			}
			break;
		}
	}

	return inner_buf;
}


/* A struct for nest monster information with cheat_hear */
typedef struct
{
	s16b r_idx;
	bool used;
}
nest_mon_info_type;


/*
 * Comp function for sorting nest monster information
 */
static bool ang_sort_comp_nest_mon_info(vptr u, vptr v, int a, int b)
{
	nest_mon_info_type *nest_mon_info = (nest_mon_info_type *)u;
	int w1 = nest_mon_info[a].r_idx;
	int w2 = nest_mon_info[b].r_idx;
	monster_race *r1_ptr = &r_info[w1];
	monster_race *r2_ptr = &r_info[w2];
	int z1, z2;

	/* Unused */
	(void)v;

	/* Extract used info */
	z1 = nest_mon_info[a].used;
	z2 = nest_mon_info[b].used;

	/* Compare used status */
	if (z1 < z2) return FALSE;
	if (z1 > z2) return TRUE;

	/* Compare levels */
	if (r1_ptr->level < r2_ptr->level) return TRUE;
	if (r1_ptr->level > r2_ptr->level) return FALSE;

	/* Compare experience */
	if (r1_ptr->mexp < r2_ptr->mexp) return TRUE;
	if (r1_ptr->mexp > r2_ptr->mexp) return FALSE;

	/* Compare indexes */
	return w1 <= w2;
}


/*
 * Swap function for sorting nest monster information
 */
static void ang_sort_swap_nest_mon_info(vptr u, vptr v, int a, int b)
{
	nest_mon_info_type *nest_mon_info = (nest_mon_info_type *)u;
	nest_mon_info_type holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = nest_mon_info[a];
	nest_mon_info[a] = nest_mon_info[b];
	nest_mon_info[b] = holder;
}


#define NUM_NEST_MON_TYPE 64

/*
 * Type 5 -- Monster nests
 *
 * A monster nest is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type strewn about the room.
 *
 * The monsters are chosen from a set of 64 randomly selected monster
 * races, to allow the nest creation to fail instead of having "holes".
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the nest.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which
 * case the nest will be empty.
 *
 * Note that "monster nests" will never contain "unique" monsters.
 */
static bool build_type5(void)
{
	int y, x, y1, x1, y2, x2, xval, yval;
	int i;
	nest_mon_info_type nest_mon_info[NUM_NEST_MON_TYPE];

	monster_type align;

	cave_type *c_ptr;

	int cur_nest_type = pick_vault_type(nest_types, d_info[dungeon_type].nest);
	vault_aux_type *n_ptr;

	/* No type available */
	if (cur_nest_type < 0) return FALSE;

	n_ptr = &nest_types[cur_nest_type];

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func) (*(n_ptr->prep_func))();

	/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func, NULL);

	align.sub_align = SUB_ALIGN_NEUTRAL;

	/* Pick some monster types */
	for (i = 0; i < NUM_NEST_MON_TYPE; i++)
	{
		int r_idx = 0, attempts = 100;
		monster_race *r_ptr = NULL;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(dun_level + 11);
			r_ptr = &r_info[r_idx];

			/* Decline incorrect alignment */
			if (monster_has_hostile_align(&align, 0, 0, r_ptr)) continue;

			/* Accept this monster */
			break;
		}

		/* Notice failure */
		if (!r_idx || !attempts) return FALSE;

		/* Note the alignment */
		if (r_ptr->flags3 & RF3_ANG_CHAOS) align.sub_align |= SUB_ALIGN_EVIL;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) align.sub_align |= SUB_ALIGN_GOOD;

		nest_mon_info[i].r_idx = r_idx;
		nest_mon_info[i].used = FALSE;
	}

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 11, 25)) return FALSE;

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}


	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_inner_grid(c_ptr);
	}

	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_inner_grid(c_ptr);
	}
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			add_cave_info(y, x, CAVE_ICKY);
		}
	}

	/* Place a secret door */
	switch (randint1(4))
	{
		case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
		case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
		case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
		case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
	}

	/* Describe */
	if (cheat_room)
	{
		/* Room type */
#ifdef JP
		msg_format("モンスター部屋(nest)(%s%s)", n_ptr->name, pit_subtype_string(cur_nest_type, TRUE));
#else
		msg_format("Monster nest (%s%s)", n_ptr->name, pit_subtype_string(cur_nest_type, TRUE));
#endif
	}

	/* Place some monsters */
	for (y = yval - 2; y <= yval + 2; y++)
	{
		for (x = xval - 9; x <= xval + 9; x++)
		{
			int r_idx;

			i = randint0(NUM_NEST_MON_TYPE);
			r_idx = nest_mon_info[i].r_idx;

			/* Place that "random" monster (no groups) */
			(void)place_monster_aux(0, y, x, r_idx, 0L);

			nest_mon_info[i].used = TRUE;
		}
	}

	if (cheat_room && cheat_hear)
	{
		ang_sort_comp = ang_sort_comp_nest_mon_info;
		ang_sort_swap = ang_sort_swap_nest_mon_info;
		ang_sort(nest_mon_info, NULL, NUM_NEST_MON_TYPE);

		/* Dump the entries (prevent multi-printing) */
		for (i = 0; i < NUM_NEST_MON_TYPE; i++)
		{
			if (!nest_mon_info[i].used) break;
			for (; i < NUM_NEST_MON_TYPE - 1; i++)
			{
				if (nest_mon_info[i].r_idx != nest_mon_info[i + 1].r_idx) break;
				if (!nest_mon_info[i + 1].used) break;
			}
			msg_print(r_name + r_info[nest_mon_info[i].r_idx].name);
		}
	}

	return TRUE;
}


/*
 * Type 6 -- Monster pits
 *
 * A monster pit is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type organized in the room.
 *
 * The inside room in a monster pit appears as shown below, where the
 * actual monsters in each location depend on the type of the pit
 *
 *   #####################
 *   #0000000000000000000#
 *   #0112233455543322110#
 *   #0112233467643322110#
 *   #0112233455543322110#
 *   #0000000000000000000#
 *   #####################
 *
 * Note that the monsters in the pit are now chosen by using "get_mon_num()"
 * to request 16 "appropriate" monsters, sorting them by level, and using
 * the "even" entries in this sorted list for the contents of the pit.
 *
 * Hack -- all of the "dragons" in a "dragon" pit must be the same "color",
 * which is handled by requiring a specific "breath" attack for all of the
 * dragons.  This may include "multi-hued" breath.  Note that "wyrms" may
 * be present in many of the dragon pits, if they have the proper breath.
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the pit.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which case
 * the pit will be empty.
 *
 * Note that "monster pits" will never contain "unique" monsters.
 */
static bool build_type6(void)
{
	int y, x, y1, x1, y2, x2, xval, yval;
	int i, j;

	int what[16];

	monster_type align;

	cave_type *c_ptr;

	int cur_pit_type = pick_vault_type(pit_types, d_info[dungeon_type].pit);
	vault_aux_type *n_ptr;

	/* No type available */
	if (cur_pit_type < 0) return FALSE;

	n_ptr = &pit_types[cur_pit_type];

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func) (*(n_ptr->prep_func))();

	/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func, NULL);

	align.sub_align = SUB_ALIGN_NEUTRAL;

	/* Pick some monster types */
	for (i = 0; i < 16; i++)
	{
		int r_idx = 0, attempts = 100;
		monster_race *r_ptr = NULL;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(dun_level + 11);
			r_ptr = &r_info[r_idx];

			/* Decline incorrect alignment */
			if (monster_has_hostile_align(&align, 0, 0, r_ptr)) continue;

			/* Accept this monster */
			break;
		}

		/* Notice failure */
		if (!r_idx || !attempts) return FALSE;

		/* Note the alignment */
		if (r_ptr->flags3 & RF3_ANG_CHAOS) align.sub_align |= SUB_ALIGN_EVIL;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) align.sub_align |= SUB_ALIGN_GOOD;

		what[i] = r_idx;
	}

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 11, 25)) return FALSE;

	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}

	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_inner_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_inner_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_inner_grid(c_ptr);
	}
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			add_cave_info(y, x, CAVE_ICKY);
		}
	}

	/* Place a secret door */
	switch (randint1(4))
	{
		case 1: place_secret_door(y1 - 1, xval, DOOR_DEFAULT); break;
		case 2: place_secret_door(y2 + 1, xval, DOOR_DEFAULT); break;
		case 3: place_secret_door(yval, x1 - 1, DOOR_DEFAULT); break;
		case 4: place_secret_door(yval, x2 + 1, DOOR_DEFAULT); break;
	}

	/* Sort the entries */
	for (i = 0; i < 16 - 1; i++)
	{
		/* Sort the entries */
		for (j = 0; j < 16 - 1; j++)
		{
			int i1 = j;
			int i2 = j + 1;

			int p1 = r_info[what[i1]].level;
			int p2 = r_info[what[i2]].level;

			/* Bubble */
			if (p1 > p2)
			{
				int tmp = what[i1];
				what[i1] = what[i2];
				what[i2] = tmp;
			}
		}
	}

	/* Message */
	if (cheat_room)
	{
		/* Room type */
#ifdef JP
		msg_format("モンスター部屋(pit)(%s%s)", n_ptr->name, pit_subtype_string(cur_pit_type, FALSE));
#else
		msg_format("Monster pit (%s%s)", n_ptr->name, pit_subtype_string(cur_pit_type, FALSE));
#endif
	}

	/* Select the entries */
	for (i = 0; i < 8; i++)
	{
		/* Every other entry */
		what[i] = what[i * 2];

		if (cheat_hear)
		{
			/* Message */
			msg_print(r_name + r_info[what[i]].name);
		}
	}

	/* Top and bottom rows */
	for (x = xval - 9; x <= xval + 9; x++)
	{
		place_monster_aux(0, yval - 2, x, what[0], PM_NO_KAGE);
		place_monster_aux(0, yval + 2, x, what[0], PM_NO_KAGE);
	}

	/* Middle columns */
	for (y = yval - 1; y <= yval + 1; y++)
	{
		place_monster_aux(0, y, xval - 9, what[0], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 9, what[0], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 8, what[1], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 8, what[1], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 7, what[1], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 7, what[1], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 6, what[2], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 6, what[2], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 5, what[2], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 5, what[2], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 4, what[3], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 4, what[3], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 3, what[3], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 3, what[3], PM_NO_KAGE);

		place_monster_aux(0, y, xval - 2, what[4], PM_NO_KAGE);
		place_monster_aux(0, y, xval + 2, what[4], PM_NO_KAGE);
	}

	/* Above/Below the center monster */
	for (x = xval - 1; x <= xval + 1; x++)
	{
		place_monster_aux(0, yval + 1, x, what[5], PM_NO_KAGE);
		place_monster_aux(0, yval - 1, x, what[5], PM_NO_KAGE);
	}

	/* Next to the center monster */
	place_monster_aux(0, yval, xval + 1, what[6], PM_NO_KAGE);
	place_monster_aux(0, yval, xval - 1, what[6], PM_NO_KAGE);

	/* Center monster */
	place_monster_aux(0, yval, xval, what[7], PM_NO_KAGE);

	return TRUE;
}


/* coordinate translation code */
static void coord_trans(int *x, int *y, int xoffset, int yoffset, int transno)
{
	int i;
	int temp;

	/*
	 * transno specifies what transformation is required. (0-7)
	 * The lower two bits indicate by how much the vault is rotated,
	 * and the upper bit indicates a reflection.
	 * This is done by using rotation matrices... however since
	 * these are mostly zeros for rotations by 90 degrees this can
	 * be expressed simply in terms of swapping and inverting the
	 * x and y coordinates.
	 */
	for (i = 0; i < transno % 4; i++)
	{
		/* rotate by 90 degrees */
		temp = *x;
		*x = -(*y);
		*y = temp;
	}

	if (transno / 4)
	{
		/* Reflect depending on status of 3rd bit. */
		*x = -(*x);
	}

	/* Add offsets so vault stays in the first quadrant */
	*x += xoffset;
	*y += yoffset;
}


/*
 * Hack -- fill in "vault" rooms
 */
/*:::v_infoの内容を解釈してcave[][]に展開する*/
/*:::data:v_textにあるvault記述部の、該当vaultについての記述が始まってる部分のアドレス*/
static void build_vault(int yval, int xval, int ymax, int xmax, cptr data,
		int xoffset, int yoffset, int transno)
{
	int dx, dy, x, y, i, j;

	cptr t;

	cave_type *c_ptr;


	/* Place dungeon features and objects */
	/*:::vault地形データの改行判定はxmaxデータのみ。v_info.txtをいじるときはくれぐれも気をつけないといけない。*/
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* prevent loop counter from being overwritten */
			i = dx;
			j = dy;

			/* Flip / rotate */
			coord_trans(&i, &j, xoffset, yoffset, transno);

			/* Extract the location */
			if (transno % 2 == 0)
			{
				/* no swap of x/y */
				x = xval - (xmax / 2) + i;
				y = yval - (ymax / 2) + j;
			}
			else
			{
				/* swap of x/y */
				x = xval - (ymax / 2) + i;
				y = yval - (xmax / 2) + j;
			}

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Lay down a floor */
			place_floor_grid(c_ptr);

			/* Remove any mimic */
			c_ptr->mimic = 0;

			/* Part of a vault */
			c_ptr->info |= (CAVE_ROOM | CAVE_ICKY);

			/* Analyze the grid */
			switch (*t)
			{
				/* Granite wall (outer) */
			case '%':
				place_outer_noperm_grid(c_ptr);
				break;

				/* Granite wall (inner) */
			case '#':
				place_inner_grid(c_ptr);
				break;

				/* Glass wall (inner) */
			case '$':
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
				break;

				/* Permanent wall (inner) */
			case 'X':
				place_inner_perm_grid(c_ptr);
				break;

				/* Permanent glass wall (inner) */
			case 'Y':
				place_inner_perm_grid(c_ptr);
				c_ptr->feat = feat_permanent_glass_wall;
				break;

				/* Treasure/trap */
			case '*':
				if (randint0(100) < 75)
				{
					place_object(y, x, 0L);
				}
				else
				{
					place_trap(y, x);
				}
				break;

				/* Secret doors */
			case '+':
				place_secret_door(y, x, DOOR_DEFAULT);
				break;

				/* Secret glass doors */
			case '-':
				place_secret_door(y, x, DOOR_GLASS_DOOR);
				if (is_closed_door(c_ptr->feat)) c_ptr->mimic = feat_glass_wall;
				break;

				/* Curtains */
			case '\'':
				place_secret_door(y, x, DOOR_CURTAIN);
				break;

				/* Trap */
			case '^':
				place_trap(y, x);
				break;

				/* Black market in a dungeon */
			case 'S':
				set_cave_feat(y, x, feat_black_market);
				//store_init(NO_TOWN, STORE_BLACK);
				store_init(TOWN_UG, STORE_BLACK);
				break;

				/* The Pattern */
			case 'p':
				set_cave_feat(y, x, feat_pattern_start);
				break;

			case 'a':
				set_cave_feat(y, x, feat_pattern_1);
				break;

			case 'b':
				set_cave_feat(y, x, feat_pattern_2);
				break;

			case 'c':
				set_cave_feat(y, x, feat_pattern_3);
				break;

			case 'd':
				set_cave_feat(y, x, feat_pattern_4);
				break;

			case 'P':
				set_cave_feat(y, x, feat_pattern_end);
				break;

			case 'B':
				set_cave_feat(y, x, feat_pattern_exit);
				break;

			case 'A':
				/* Reward for Pattern walk */
				object_level = base_level + 12;
				place_object(y, x, AM_GOOD | AM_GREAT);
				object_level = base_level;
				break;
			}
		}
	}


	/* Place dungeon monsters and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* prevent loop counter from being overwritten */
			i = dx;
			j = dy;

			/* Flip / rotate */
			coord_trans(&i, &j, xoffset, yoffset, transno);

			/* Extract the location */
			if (transno % 2 == 0)
			{
				/* no swap of x/y */
				x = xval - (xmax / 2) + i;
				y = yval - (ymax / 2) + j;
			}
			else
			{
				/* swap of x/y */
				x = xval - (ymax / 2) + i;
				y = yval - (xmax / 2) + j;
			}

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Analyze the symbol */
			switch (*t)
			{
				/* Monster */
				case '&':
				{
					monster_level = base_level + 5;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
					break;
				}

				/* Meaner monster */
				case '@':
				{
					monster_level = base_level + 11;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
					break;
				}

				/* Meaner monster, plus treasure */
				case '9':
				{
					monster_level = base_level + 9;
					place_monster(y, x, PM_ALLOW_SLEEP);
					monster_level = base_level;
					object_level = base_level + 7;
					place_object(y, x, AM_GOOD);
					object_level = base_level;
					break;
				}

				/* Nasty monster and treasure */
				case '8':
				{
					monster_level = base_level + 40;
					place_monster(y, x, PM_ALLOW_SLEEP);
					monster_level = base_level;
					object_level = base_level + 20;
					place_object(y, x, AM_GOOD | AM_GREAT);
					object_level = base_level;
					break;
				}

				/* Monster and/or object */
				case ',':
				{
					if (randint0(100) < 50)
					{
						monster_level = base_level + 3;
						place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
						monster_level = base_level;
					}
					if (randint0(100) < 50)
					{
						object_level = base_level + 7;
						place_object(y, x, 0L);
						object_level = base_level;
					}
					break;
				}

			}
		}
	}
}


/*
 * Type 7 -- simple vaults (see "v_info.txt")
 */
/*:::vault生成。v_info[]からtyp==7のデータを選定しbuild_vault()へ渡す*/
static bool build_type7(void)
{
	vault_type *v_ptr;
	int dummy;
	int x, y;
	int xval, yval;
	int xoffset, yoffset;
	int transno;

	/* Pick a lesser vault */
	for (dummy = 0; dummy < SAFE_MAX_ATTEMPTS; dummy++)
	{
		/* Access a random vault record */
		v_ptr = &v_info[randint0(max_v_idx)];

		/* Accept the first lesser vault */
		if (v_ptr->typ == 7) break;
	}

	/* No lesser vault found */
	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
#ifdef JP
			msg_print("警告！小さな地下室を配置できません！");
#else
			msg_print("Warning! Could not place lesser vault!");
#endif
		}
		return FALSE;
	}

	/* pick type of transformation (0-7) */
	transno = randint0(8);

	/* calculate offsets */
	x = v_ptr->wid;
	y = v_ptr->hgt;

	/* Some huge vault cannot be ratated to fit in the dungeon */
	if (x+2 > cur_hgt-2)
	{
		/* Forbid 90 or 270 degree ratation */
		transno &= ~1;
	}

	coord_trans(&x, &y, 0, 0, transno);

	if (x < 0)
	{
		xoffset = -x - 1;
	}
	else
	{
		xoffset = 0;
	}

	if (y < 0)
	{
		yoffset = -y - 1;
	}
	else
	{
		yoffset = 0;
	}

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, abs(y), abs(x))) return FALSE;

#ifdef FORCE_V_IDX
	v_ptr = &v_info[2];
#endif

	/* Message */
#ifdef JP
	if (cheat_room) msg_format("小さな地下室(%s)", v_name + v_ptr->name);
#else
	if (cheat_room) msg_format("Lesser vault (%s)", v_name + v_ptr->name);
#endif

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid,
		    v_text + v_ptr->text, xoffset, yoffset, transno);

	return TRUE;
}


/*
 * Type 8 -- greater vaults (see "v_info.txt")
 */
static bool build_type8(void)
{
	vault_type *v_ptr;
	int dummy;
	int xval, yval;
	int x, y;
	int transno;
	int xoffset, yoffset;

	/* Pick a greater vault */
	for (dummy = 0; dummy < SAFE_MAX_ATTEMPTS; dummy++)
	{
		/* Access a random vault record */
		v_ptr = &v_info[randint0(max_v_idx)];
		//test v_ptr = &v_info[110];

		/* Accept the first greater vault */
		if (v_ptr->typ == 8) break;
	}

	/* No greater vault found */
	if (dummy >= SAFE_MAX_ATTEMPTS)
	{
		if (cheat_room)
		{
#ifdef JP
			msg_print("警告！巨大な地下室を配置できません！");
#else
			msg_print("Warning! Could not place greater vault!");
#endif
		}
		return FALSE;
	}

	/* pick type of transformation (0-7) */
	transno = randint0(8);

	/* calculate offsets */
	x = v_ptr->wid;
	y = v_ptr->hgt;

	/* Some huge vault cannot be ratated to fit in the dungeon */
	if (x+2 > cur_hgt-2)
	{
		/* Forbid 90 or 270 degree ratation */
		transno &= ~1;
	}

	coord_trans(&x, &y, 0, 0, transno);

	if (x < 0)
	{
		xoffset = - x - 1;
	}
	else
	{
		xoffset = 0;
	}

	if (y < 0)
	{
		yoffset = - y - 1;
	}
	else
	{
		yoffset = 0;
	}

	/*
	 * Try to allocate space for room.  If fails, exit
	 *
	 * Hack -- Prepare a bit larger space (+2, +2) to
	 * prevent generation of vaults with no-entrance.
	 */
	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, abs(y) + 2, abs(x) + 2)) return FALSE;

#ifdef FORCE_V_IDX
	v_ptr = &v_info[76 + randint1(3)];
#endif

	/* Message */
#ifdef JP
	if (cheat_room) msg_format("巨大な地下室(%s)", v_name + v_ptr->name);
#else
	if (cheat_room) msg_format("Greater vault (%s)", v_name + v_ptr->name);
#endif

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr->hgt, v_ptr->wid,
		    v_text + v_ptr->text, xoffset, yoffset, transno);

	return TRUE;
}

/*
 * Structure to hold all "fill" data
 */

typedef struct fill_data_type fill_data_type;

struct fill_data_type
{
	/* area size */
	int xmin;
	int ymin;
	int xmax;
	int ymax;

	/* cutoffs */
	int c1;
	int c2;
	int c3;

	/* features to fill with */
	int feat1;
	int feat2;
	int feat3;

	int info1;
	int info2;
	int info3;

	/* number of filled squares */
	int amount;
};

static fill_data_type fill_data;


/* Store routine for the fractal cave generator */
/* this routine probably should be an inline function or a macro. */
/*:::generate_hmap()から使われるアルゴリズムの一部　詳細不明*/
static void store_height(int x, int y, int val)
{
	/* if on boundary set val > cutoff so walls are not as square */
	if (((x == fill_data.xmin) || (y == fill_data.ymin) ||
	     (x == fill_data.xmax) || (y == fill_data.ymax)) &&
	    (val <= fill_data.c1)) val = fill_data.c1 + 1;

	/* store the value in height-map format */
	cave[y][x].feat = val;

	return;
}


/*
* Explanation of the plasma fractal algorithm:
*
* A grid of points is created with the properties of a 'height-map'
* This is done by making the corners of the grid have a random value.
* The grid is then subdivided into one with twice the resolution.
* The new points midway between two 'known' points can be calculated
* by taking the average value of the 'known' ones and randomly adding
* or subtracting an amount proportional to the distance between those
* points.  The final 'middle' points of the grid are then calculated
* by averaging all four of the originally 'known' corner points.  An
* random amount is added or subtracted from this to get a value of the
* height at that point.  The scaling factor here is adjusted to the
* slightly larger distance diagonally as compared to orthogonally.
*
* This is then repeated recursively to fill an entire 'height-map'
* A rectangular map is done the same way, except there are different
* scaling factors along the x and y directions.
*:::よく解らんが、指定範囲の地形を与えられたシードに従いランダムに近く生成するのだと思う
*
* A hack to change the amount of correlation between points is done using
* the grd variable.  If the current step size is greater than grd then
* the point will be random, otherwise it will be calculated by the
* above algorithm.  This makes a maximum distance at which two points on
* the height map can affect each other.
*
* How fractal caves are made:
*
* When the map is complete, a cut-off value is used to create a cave.
* Heights below this value are "floor", and heights above are "wall".
* This also can be used to create lakes, by adding more height levels
* representing shallow and deep water/ lava etc.
*:::ランダムに近い生成が終わった後で、閾値に従い壁や床に再構成する？
*
* The grd variable affects the width of passages.
* The roug variable affects the roughness of those passages
*
* The tricky part is making sure the created cave is connected.  This
* is done by 'filling' from the inside and only keeping the 'filled'
* floor.  Walls bounding the 'filled' floor are also kept.  Everything
* else is converted to the normal _extra_.
 */


/*
 *  Note that this uses the cave.feat array in a very hackish way
 *  the values are first set to zero, and then each array location
 *  is used as a "heightmap"
 *  The heightmap then needs to be converted back into the "feat" format.
 *
 *  grd=level at which fractal turns on.  smaller gives more mazelike caves
 *  roug=roughness level.  16=normal.  higher values make things more convoluted
 *    small values are good for smooth walls.
 *  size=length of the side of the square cave system.
 */
/*:::x0,y0:詳細不明　生成対象エリアサイズ？
 *:::xsiz:フロアサイズX ysiz:フロアサイズY
 *:::grd:詳細不明 生成アルゴリズムの選択に関係？
 *:::roug:詳細不明のランダム値　高いと壁や通路がでこぼこになる？
 *:::cutoff:詳細不明 壁や床(水や溶岩も)の生成判定基準に使われるらしい　*/
/*:::cave[][].featが一時的情報置き場として使われている*/
/*:::Q012この関数が終わった時点でcave[][]はランダムなのか？部屋と通路ができているのか？ざっと見たができてるとは思えないが*/
static void generate_hmap(int y0, int x0, int xsiz, int ysiz, int grd, int roug, int cutoff)
{
	int xhsize, yhsize, xsize, ysize, maxsize;

	/*
	 * fixed point variables- these are stored as 256 x normal value
	 * this gives 8 binary places of fractional part + 8 places of normal part
	 */

	u16b xstep, xhstep, ystep, yhstep;
	u16b xstep2, xhstep2, ystep2, yhstep2;
	u16b i, j, ii, jj, diagsize, xxsize, yysize;

	/* Cache for speed */
	u16b xm, xp, ym, yp;

	/* redefine size so can change the value if out of range */
	xsize = xsiz;
	ysize = ysiz;

	/* Paranoia about size of the system of caves */
	if (xsize > 254) xsize = 254;
	if (xsize < 4) xsize = 4;
	if (ysize > 254) ysize = 254;
	if (ysize < 4) ysize = 4;

	/* get offsets to middle of array */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	/* fix rounding problem */
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	/* get limits of region */
	fill_data.xmin = x0 - xhsize;
	fill_data.ymin = y0 - yhsize;
	fill_data.xmax = x0 + xhsize;
	fill_data.ymax = y0 + yhsize;

	/* Store cutoff in global for quick access */
	fill_data.c1 = cutoff;

	/*
	* Scale factor for middle points:
	* About sqrt(2) * 256 - correct for a square lattice
	* approximately correct for everything else.
	 */
	diagsize = 362;

	/* maximum of xsize and ysize */
	maxsize = (xsize > ysize) ? xsize : ysize;

	/* Clear the section */
	for (i = 0; i <= xsize; i++)
	{
		for (j = 0; j <= ysize; j++)
		{
			/* -1 is a flag for "not done yet" */
			cave[(int)(fill_data.ymin + j)][(int)(fill_data.xmin + i)].feat = -1;
			/* Clear icky flag because may be redoing the cave */
			cave[(int)(fill_data.ymin + j)][(int)(fill_data.xmin + i)].info &= ~(CAVE_ICKY);
		}
	}

	/* Boundaries are walls */
	cave[fill_data.ymin][fill_data.xmin].feat = maxsize;
	cave[fill_data.ymax][fill_data.xmin].feat = maxsize;
	cave[fill_data.ymin][fill_data.xmax].feat = maxsize;
	cave[fill_data.ymax][fill_data.xmax].feat = maxsize;

	/* Set the middle square to be an open area. */
	cave[y0][x0].feat = 0;

	/* Initialize the step sizes */
	xstep = xhstep = xsize * 256;
	ystep = yhstep = ysize * 256;
	xxsize = xsize * 256;
	yysize = ysize * 256;

	/*
	 * Fill in the rectangle with fractal height data -
	 * like the 'plasma fractal' in fractint.
	 */
	while ((xhstep > 256) || (yhstep > 256))
	{
		/* Halve the step sizes */
		xstep = xhstep;
		xhstep /= 2;
		ystep = yhstep;
		yhstep /= 2;

		/* cache well used values */
		xstep2 = xstep / 256;
		ystep2 = ystep / 256;

		xhstep2 = xhstep / 256;
		yhstep2 = yhstep / 256;

		/* middle top to bottom. */
		for (i = xhstep; i <= xxsize - xhstep; i += xstep)
		{
			for (j = 0; j <= yysize; j += ystep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == -1)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint1(maxsize));
					}
					else
					{
						/* Average of left and right points +random bit */
						store_height(ii, jj,
							(cave[jj][fill_data.xmin + (i - xhstep) / 256].feat
							 + cave[jj][fill_data.xmin + (i + xhstep) / 256].feat) / 2
							 + (randint1(xstep2) - xhstep2) * roug / 16);
					}
				}
			}
		}


		/* middle left to right. */
		for (j = yhstep; j <= yysize - yhstep; j += ystep)
		{
			for (i = 0; i <= xxsize; i += xstep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == -1)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint1(maxsize));
					}
					else
					{
						/* Average of up and down points +random bit */
						store_height(ii, jj,
							(cave[fill_data.ymin + (j - yhstep) / 256][ii].feat
							+ cave[fill_data.ymin + (j + yhstep) / 256][ii].feat) / 2
							+ (randint1(ystep2) - yhstep2) * roug / 16);
					}
				}
			}
		}

		/* center. */
		for (i = xhstep; i <= xxsize - xhstep; i += xstep)
		{
			for (j = yhstep; j <= yysize - yhstep; j += ystep)
			{
				/* cache often used values */
				ii = i / 256 + fill_data.xmin;
				jj = j / 256 + fill_data.ymin;

				/* Test square */
				if (cave[jj][ii].feat == -1)
				{
					if (xhstep2 > grd)
					{
						/* If greater than 'grid' level then is random */
						store_height(ii, jj, randint1(maxsize));
					}
					else
					{
						/* Cache reused values. */
						xm = fill_data.xmin + (i - xhstep) / 256;
						xp = fill_data.xmin + (i + xhstep) / 256;
						ym = fill_data.ymin + (j - yhstep) / 256;
						yp = fill_data.ymin + (j + yhstep) / 256;

						/*
						 * Average over all four corners + scale by diagsize to
						 * reduce the effect of the square grid on the shape of the fractal
						 */
						store_height(ii, jj,
							(cave[ym][xm].feat + cave[yp][xm].feat
							+ cave[ym][xp].feat + cave[yp][xp].feat) / 4
							+ (randint1(xstep2) - xhstep2) * (diagsize / 16) / 256 * roug);
					}
				}
			}
		}
	}
}


static bool hack_isnt_wall(int y, int x, int c1, int c2, int c3, int feat1, int feat2, int feat3, int info1, int info2, int info3)
{
	/*
	 * function used to convert from height-map back to the
	 *  normal angband cave format
	 */
	if (cave[y][x].info & CAVE_ICKY)
	{
		/* already done */
		return FALSE;
	}
	else
	{
		/* Show that have looked at this square */
		cave[y][x].info|= (CAVE_ICKY);

		/* Use cutoffs c1-c3 to allocate regions of floor /water/ lava etc. */
		if (cave[y][x].feat <= c1)
		{
			/* 25% of the time use the other tile : it looks better this way */
			if (randint1(100) < 75)
			{
				cave[y][x].feat = feat1;
				cave[y][x].info &= ~(CAVE_MASK);
				cave[y][x].info |= info1;
				return TRUE;
			}
			else
			{
				cave[y][x].feat = feat2;
				cave[y][x].info &= ~(CAVE_MASK);
				cave[y][x].info |= info2;
				return TRUE;
			}
		}
		else if (cave[y][x].feat <= c2)
		{
			/* 25% of the time use the other tile : it looks better this way */
			if (randint1(100) < 75)
			{
				cave[y][x].feat = feat2;
				cave[y][x].info &= ~(CAVE_MASK);
				cave[y][x].info |= info2;
				return TRUE;
			}
			else
			{
				cave[y][x].feat = feat1;
				cave[y][x].info &= ~(CAVE_MASK);
				cave[y][x].info |= info1;
				return TRUE;
			}
		}
		else if (cave[y][x].feat <= c3)
		{
			cave[y][x].feat = feat3;
			cave[y][x].info &= ~(CAVE_MASK);
			cave[y][x].info |= info3;
			return TRUE;
		}
		/* if greater than cutoff then is a wall */
		else
		{
			place_outer_bold(y, x);
			return FALSE;
		}
	}
}




/*
 * Quick and nasty fill routine used to find the connected region
 * of floor in the middle of the cave
 */
static void cave_fill(byte y, byte x)
{
	int i, j, d;
	int ty, tx;

	int flow_tail = 1;
	int flow_head = 0;


	/*** Start Grid ***/

	/* Enqueue that entry */
	temp_y[0] = y;
	temp_x[0] = x;


	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		ty = temp_y[flow_head];
		tx = temp_x[flow_head];

		/* Forget that entry */
		if (++flow_head == TEMP_MAX) flow_head = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			int old_head = flow_tail;

			/* Child location */
			j = ty + ddy_ddd[d];
			i = tx + ddx_ddd[d];

			/* Paranoia Don't leave the cave */
			if (!in_bounds(j, i))
			{
				/* affect boundary */
				cave[j][i].info |= CAVE_ICKY;
/*				return; */
			}

			/* If within bounds */
			else if ((i > fill_data.xmin) && (i < fill_data.xmax)
				&& (j > fill_data.ymin) && (j < fill_data.ymax))
			{
				/* If not a wall or floor done before */
				if (hack_isnt_wall(j, i,
					fill_data.c1, fill_data.c2, fill_data.c3,
					fill_data.feat1, fill_data.feat2, fill_data.feat3,
					fill_data.info1, fill_data.info2, fill_data.info3))
				{
					/* Enqueue that entry */
					temp_y[flow_tail] = j;
					temp_x[flow_tail] = i;

					/* Advance the queue */
					if (++flow_tail == TEMP_MAX) flow_tail = 0;

					/* Hack -- Overflow by forgetting new entry */
					if (flow_tail == flow_head)
					{
						flow_tail = old_head;
					}
					else
					{
						/* keep tally of size of cave system */
						(fill_data.amount)++;
					}
				}
			}
			else
			{
				/* affect boundary */
				cave[j][i].info |= CAVE_ICKY;
			}
		}
	}
}


static bool generate_fracave(int y0, int x0, int xsize, int ysize, int cutoff, bool light, bool room)
{
	int x, y, i, xhsize, yhsize;

	/* offsets to middle from corner */
	xhsize = xsize / 2;
	yhsize = ysize / 2;


	/*
	 * select region connected to center of cave system
	 * this gets rid of alot of isolated one-sqaures that
	 * can make teleport traps instadeaths...
	 */

	/* cutoffs */
	fill_data.c1 = cutoff;
	fill_data.c2 = 0;
	fill_data.c3 = 0;

	/* features to fill with */
	fill_data.feat1 = floor_type[randint0(100)];
	fill_data.feat2 = floor_type[randint0(100)];
	fill_data.feat3 = floor_type[randint0(100)];

	fill_data.info1 = CAVE_FLOOR;
	fill_data.info2 = CAVE_FLOOR;
	fill_data.info3 = CAVE_FLOOR;

	/* number of filled squares */
	fill_data.amount = 0;

	cave_fill((byte)y0, (byte)x0);

	/* if tally too small, try again */
	if (fill_data.amount < 10)
	{
		/* too small - clear area and try again later */
		for (x = 0; x <= xsize; ++x)
		{
			for (y = 0; y <= ysize; ++y)
			{
				place_extra_bold(y0 + y - yhsize, x0 + x - xhsize);
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ICKY | CAVE_ROOM);
			}
		}
		return FALSE;
	}

	/*
	 * Do boundarys-check to see if they are next to a filled region
	 * If not then they are set to normal granite
	 * If so then they are marked as room walls.
	 */
	for (i = 0; i <= xsize; ++i)
	{
		/* top boundary */
		if ((cave[0 + y0 - yhsize][i + x0 - xhsize].info & CAVE_ICKY) && (room))
		{
			/* Next to a 'filled' region? - set to be room walls */
			place_outer_bold(y0 + 0 - yhsize, x0 + i - xhsize);
			if (light) cave[y0 + 0 - yhsize][x0 + i - xhsize].info |= (CAVE_GLOW);
			cave[y0 + 0 - yhsize][x0 + i - xhsize].info |= (CAVE_ROOM);
			place_outer_bold(y0 + 0 - yhsize, x0 + i - xhsize);
		}
		else
		{
			/* set to be normal granite */
			place_extra_bold(y0 + 0 - yhsize, x0 + i - xhsize);
		}

		/* bottom boundary */
		if ((cave[ysize + y0 - yhsize][i + x0 - xhsize].info & CAVE_ICKY) && (room))
		{
			/* Next to a 'filled' region? - set to be room walls */
			place_outer_bold(y0 + ysize - yhsize, x0 + i - xhsize);
			if (light) cave[y0 + ysize - yhsize][x0 + i - xhsize].info|=(CAVE_GLOW);
			cave[y0 + ysize - yhsize][x0 + i - xhsize].info|=(CAVE_ROOM);
			place_outer_bold(y0 + ysize - yhsize, x0 + i - xhsize);
		}
		else
		{
			/* set to be normal granite */
			place_extra_bold(y0 + ysize - yhsize, x0 + i - xhsize);
		}

		/* clear the icky flag-don't need it any more */
		cave[y0 + 0 - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + ysize - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
	}

	/* Do the left and right boundaries minus the corners (done above) */
	for (i = 1; i < ysize; ++i)
	{
		/* left boundary */
		if ((cave[i + y0 - yhsize][0 + x0 - xhsize].info & CAVE_ICKY) && room)
		{
			/* room boundary */
			place_outer_bold(y0 + i - yhsize, x0 + 0 - xhsize);
			if (light) cave[y0 + i - yhsize][x0 + 0 - xhsize].info |= (CAVE_GLOW);
			cave[y0 + i - yhsize][x0 + 0 - xhsize].info |= (CAVE_ROOM);
			place_outer_bold(y0 + i - yhsize, x0 + 0 - xhsize);
		}
		else
		{
			/* outside room */
			place_extra_bold(y0 + i - yhsize, x0 + 0 - xhsize);
		}
		/* right boundary */
		if ((cave[i + y0 - yhsize][xsize + x0 - xhsize].info & CAVE_ICKY) && room)
		{
			/* room boundary */
			place_outer_bold(y0 + i - yhsize, x0 + xsize - xhsize);
			if (light) cave[y0 + i - yhsize][x0 + xsize - xhsize].info |= (CAVE_GLOW);
			cave[y0 + i - yhsize][x0 + xsize - xhsize].info |= (CAVE_ROOM);
			place_outer_bold(y0 + i - yhsize, x0 + xsize - xhsize);
		}
		else
		{
			/* outside room */
			place_extra_bold(y0 + i - yhsize, x0 + xsize - xhsize);
		}

		/* clear icky flag -done with it */
		cave[y0 + i - yhsize][x0 + 0 - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + i - yhsize][x0 + xsize - xhsize].info &= ~(CAVE_ICKY);
	}


	/* Do the rest: convert back to the normal format */
	for (x = 1; x < xsize; ++x)
	{
		for (y = 1; y < ysize; ++y)
		{
			if (is_floor_bold(y0 + y - yhsize, x0 + x - xhsize) &&
			    (cave[y0 + y - yhsize][x0 + x - xhsize].info & CAVE_ICKY))
			{
				/* Clear the icky flag in the filled region */
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~CAVE_ICKY;

				/* Set appropriate flags */
				if (light) cave[y0 + y - yhsize][x0 + x - xhsize].info |= (CAVE_GLOW);
				if (room) cave[y0 + y - yhsize][x0 + x - xhsize].info |= (CAVE_ROOM);
			}
			else if (is_outer_bold(y0 + y - yhsize, x0 + x - xhsize) &&
				 (cave[y0 + y - yhsize][x0 + x - xhsize].info & CAVE_ICKY))
			{
				/* Walls */
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ICKY);
				if (light) cave[y0 + y - yhsize][x0 + x - xhsize].info |= (CAVE_GLOW);
				if (room)
				{
					cave[y0 + y - yhsize][x0 + x - xhsize].info |= (CAVE_ROOM);
				}
				else
				{

					place_extra_bold(y0 + y - yhsize, x0 + x - xhsize);
					cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ROOM);
				}
			}
			else
			{
				/* Clear the unconnected regions */
				place_extra_bold(y0 + y - yhsize, x0 + x - xhsize);
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ICKY | CAVE_ROOM);
			}
		}
	}

	/*
	 * XXX XXX XXX There is a slight problem when tunnels pierce the caves:
	 * Extra doors appear inside the system.  (Its not very noticeable though.)
	 * This can be removed by "filling" from the outside in.  This allows a separation
	 * from _outer_ with _inner_.  (Internal walls are  _outer_ instead.)
	 * The extra effort for what seems to be only a minor thing (even non-existant if you
	 * think of the caves not as normal rooms, but as holes in the dungeon), doesn't seem
	 * worth it.
	 */

	return TRUE;
}


/*
 * Driver routine to create fractal cave system
 */
static bool build_type9(void)
{
	int grd, roug, cutoff, xsize, ysize, y0, x0;

	bool done, light, room;

	/* get size: note 'Evenness'*/
	xsize = randint1(22) * 2 + 6;
	ysize = randint1(15) * 2 + 6;

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&y0, &x0, ysize + 1, xsize + 1))
	{
		/* Limit to the minimum room size, and retry */
		xsize = 8;
		ysize = 8;

		/* Find and reserve some space in the dungeon.  Get center of room. */
		if (!find_space(&y0, &x0, ysize + 1, xsize + 1))
		{
			/*
			 * Still no space?!
			 * Try normal room
			 */
			return build_type1();
		}
	}

	light = done = FALSE;
	room = TRUE;

	if ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS)) light = TRUE;

	while (!done)
	{
		/* Note: size must be even or there are rounding problems
		* This causes the tunnels not to connect properly to the room */

		/* testing values for these parameters feel free to adjust */
		grd = 1 << (randint0(4));

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* about size/2 */
		cutoff = randint1(xsize / 4) + randint1(ysize / 4) +
			 randint1(xsize / 4) + randint1(ysize / 4);

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format + clean up */
		done = generate_fracave(y0, x0, xsize, ysize, cutoff, light, room);
	}

	return TRUE;
}

#ifdef ALLOW_CAVERNS_AND_LAKES
/*
 * Builds a cave system in the center of the dungeon.
 */
/*:::cavernとは、*破壊*された後みたいな地形のことだと思われる*/
void build_cavern(void)
{
	int grd, roug, cutoff, xsize, ysize, x0, y0;
	bool done, light;

	light = done = FALSE;
	if ((dun_level <= randint1(50)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS)) light = TRUE;

	/* Make a cave the size of the dungeon */
	xsize = cur_wid - 1;
	ysize = cur_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = randint1(4) + 4;

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* about size/2 */
		cutoff = xsize / 2;

		 /* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format+ clean up */
		done = generate_fracave(y0 + 1, x0 + 1, xsize, ysize, cutoff, light, FALSE);
	}
}

/*:::湖・溶岩地形などの生成*/
/*:::x0,y0:詳細不明　生成対象エリアサイズ？
 *:::xsize:フロアサイズX ysize:フロアサイズY
 *:::c1,c2,c3:地形生成に関わるらしいランダム値 xsizeを超えない
 *:::type:LAKE_T_LAVA～LAKE_T_FIRE_VAULTの1-7の定数 rooms.h*/

static bool generate_lake(int y0, int x0, int xsize, int ysize, int c1, int c2, int c3, int type)
{
	int x, y, i, xhsize, yhsize;
	int feat1, feat2, feat3;

	/* offsets to middle from corner */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	/* Get features based on type */
	switch (type)
	{
	case LAKE_T_LAVA: /* Lava */
		feat1 = feat_deep_lava;
		feat2 = feat_shallow_lava;
		feat3 = floor_type[randint0(100)];
		break;
	case LAKE_T_WATER: /* Water */
		feat1 = feat_deep_water;
		feat2 = feat_shallow_water;
		feat3 = floor_type[randint0(100)];
		break;
	case LAKE_T_CAVE: /* Collapsed cave */
		feat1 = floor_type[randint0(100)];
		feat2 = floor_type[randint0(100)];
		feat3 = feat_rubble;
		break;
	case LAKE_T_EARTH_VAULT: /* Earth vault */
		feat1 = feat_rubble;
		feat2 = floor_type[randint0(100)];
		feat3 = feat_rubble;
		break;
	case LAKE_T_AIR_VAULT: /* Air vault */
		feat1 = feat_grass;
		feat2 = feat_tree;
		feat3 = feat_grass;
		break;
	case LAKE_T_WATER_VAULT: /* Water vault */
		feat1 = feat_shallow_water;
		feat2 = feat_deep_water;
		feat3 = feat_shallow_water;
		break;
	case LAKE_T_FIRE_VAULT: /* Fire Vault */
		feat1 = feat_shallow_lava;
		feat2 = feat_deep_lava;
		feat3 = feat_shallow_lava;
		break;
	case LAKE_T_ACID:
		feat1 = feat_deep_acid_puddle;
		feat2 = feat_acid_puddle;
		feat3 = floor_type[randint0(100)];
		break;

	case LAKE_T_ACID_VAULT:
		feat1 = feat_acid_puddle;
		feat2 = feat_deep_acid_puddle;
		feat3 = feat_acid_puddle;
		break;
	case LAKE_T_POIS:
		feat1 = feat_deep_poisonous_puddle;
		feat2 = feat_poisonous_puddle;
		feat3 = floor_type[randint0(100)];
		break;
	case LAKE_T_POIS_VAULT:
		feat1 = feat_poisonous_puddle;
		feat2 = feat_deep_poisonous_puddle;
		feat3 = feat_poisonous_puddle;
		break;
	case LAKE_T_COLD:
		feat1 = feat_heavy_cold_zone;
		feat2 = feat_cold_zone;
		feat3 = floor_type[randint0(100)];
		break;
	case LAKE_T_COLD_VAULT:
		feat1 = feat_cold_zone;
		feat2 = feat_heavy_cold_zone;
		feat3 = feat_cold_zone;
		break;
	case LAKE_T_ELEC:
		feat1 = feat_heavy_electrical_zone;
		feat2 = feat_electrical_zone;
		feat3 = floor_type[randint0(100)];
		break;
	case LAKE_T_ELEC_VAULT:
		feat1 = feat_electrical_zone;
		feat2 = feat_heavy_electrical_zone;
		feat3 = feat_electrical_zone;
		break;


	/* Paranoia */
	default: return FALSE;
	}

	/*
	 * select region connected to center of cave system
	 * this gets rid of alot of isolated one-sqaures that
	 * can make teleport traps instadeaths...
	 */

	/* cutoffs */
	fill_data.c1 = c1;
	fill_data.c2 = c2;
	fill_data.c3 = c3;

	/* features to fill with */
	fill_data.feat1 = feat1;
	fill_data.feat2 = feat2;
	fill_data.feat3 = feat3;

	fill_data.info1 = 0;
	fill_data.info2 = 0;
	fill_data.info3 = 0;

	/* number of filled squares */
	fill_data.amount = 0;

	/* select region connected to center of cave system
	* this gets rid of alot of isolated one-sqaures that
	* can make teleport traps instadeaths... */
	cave_fill((byte)y0, (byte)x0);

	/* if tally too small, try again */
	if (fill_data.amount < 10)
	{
		/* too small -clear area and try again later */
		for (x = 0; x <= xsize; ++x)
		{
			for (y = 0; y <= ysize; ++y)
			{
				place_floor_bold(y0 + y - yhsize, x0 + x - xhsize);
				cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ICKY);
			}
		}
		return FALSE;
	}

	/* Do boundarys- set to normal granite */
	for (i = 0; i <= xsize; ++i)
	{
		place_extra_bold(y0 + 0 - yhsize, x0 + i - xhsize);
		place_extra_bold(y0 + ysize - yhsize, x0 + i - xhsize);

		/* clear the icky flag-don't need it any more */
		cave[y0 + 0 - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + ysize - yhsize][x0 + i - xhsize].info &= ~(CAVE_ICKY);
	}

	/* Do the left and right boundaries minus the corners (done above) */

	for (i = 1; i < ysize; ++i)
	{
		place_extra_bold(y0 + i - yhsize, x0 + 0 - xhsize);
		place_extra_bold(y0 + i - yhsize, x0 + xsize - xhsize);

		/* clear icky flag -done with it */
		cave[y0 + i - yhsize][x0 + 0 - xhsize].info &= ~(CAVE_ICKY);
		cave[y0 + i - yhsize][x0 + xsize - xhsize].info &= ~(CAVE_ICKY);
	}


	/* Do the rest: convert back to the normal format */
	for (x = 1; x < xsize; ++x)
	{
		for (y = 1; y < ysize; ++y)
		{
			/* Fill unconnected regions with granite */
			if ((!(cave[y0 + y - yhsize][x0 + x - xhsize].info & CAVE_ICKY)) ||
				is_outer_bold(y0 + y - yhsize, x0 + x - xhsize))
				place_extra_bold(y0 + y - yhsize, x0 + x - xhsize);

			/* turn off icky flag (no longer needed.) */
			cave[y0 + y - yhsize][x0 + x - xhsize].info &= ~(CAVE_ICKY | CAVE_ROOM);

			/* Light lava */
			if (cave_have_flag_bold(y0 + y - yhsize, x0 + x - xhsize, FF_LAVA))
			{
				if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS)) cave[y0 + y - yhsize][x0 + x - xhsize].info |= CAVE_GLOW;
			}
		}
	}

	return TRUE;
}


/*
 * makes a lake/collapsed cave system in the center of the dungeon
 */

/*:::type:LAKE_T_LAVA～LAKE_T_FIRE_VAULTの1-7の定数 rooms.h*/
void build_lake(int type)
{
	int grd, roug, xsize, ysize, x0, y0;
	bool done = FALSE;
	int c1, c2, c3;

	/* paranoia - exit if lake type out of range. */
	//v1.1.85 最大値がFIRE_VAULT決め打ちだったので書き換え
	if (type <= 0 || type > LAKE_T_MAX)
	{
		msg_format("Invalid lake type (%d)", type);
		return;
	}

	/* Make the size of the dungeon */
	xsize = cur_wid - 1;
	ysize = cur_hgt - 1;
	x0 = xsize / 2;
	y0 = ysize / 2;

	/* Paranoia: make size even */
	xsize = x0 * 2;
	ysize = y0 * 2;

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = randint1(3) + 4;

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* Make up size of various componants */
		/* Floor */
		c3 = 3 * xsize / 4;

		/* Deep water/lava */
		c1 = randint0(c3 / 2) + randint0(c3 / 2) - 5;

		/* Shallow boundary */
		c2 = (c1 + c3) / 2;

		/* make it */
		generate_hmap(y0 + 1, x0 + 1, xsize, ysize, grd, roug, c3);

		/* Convert to normal format+ clean up */
		done = generate_lake(y0 + 1, x0 + 1, xsize, ysize, c1, c2, c3, type);
	}
}
#endif /* ALLOW_CAVERNS_AND_LAKES */


/*
 * Routine used by the random vault creators to add a door to a location
 * Note that range checking has to be done in the calling routine.
 *
 * The doors must be INSIDE the allocated region.
 */
static void add_door(int x, int y)
{
	/* Need to have a wall in the center square */
	if (!is_outer_bold(y, x)) return;

	/* look at:
	 *  x#x
	 *  .#.
	 *  x#x
	 *
	 *  where x=don't care
	 *  .=floor, #=wall
	 */

	if (is_floor_bold(y-1,x) && is_floor_bold(y+1,x) &&
	    (is_outer_bold(y, x - 1) && is_outer_bold(y, x + 1)))
	{
		/* secret door */
		place_secret_door(y, x, DOOR_DEFAULT);

		/* set boundarys so don't get wide doors */
		place_solid_bold(y, x - 1);
		place_solid_bold(y, x + 1);
	}


	/* look at:
	 *  x#x
	 *  .#.
	 *  x#x
	 *
	 *  where x = don't care
	 *  .=floor, #=wall
	 */
	if (is_outer_bold(y - 1, x) && is_outer_bold(y + 1, x) &&
	    is_floor_bold(y,x-1) && is_floor_bold(y,x+1))
	{
		/* secret door */
		place_secret_door(y, x, DOOR_DEFAULT);

		/* set boundarys so don't get wide doors */
		place_solid_bold(y - 1, x);
		place_solid_bold(y + 1, x);
	}
}


/*
 * Routine that fills the empty areas of a room with treasure and monsters.
 */
/*:::指定範囲をモンスターやアイテムやトラップで埋める*/
static void fill_treasure(int x1, int x2, int y1, int y2, int difficulty_rate)
{
	int x, y, cx, cy, size;
	s32b value;

	/* center of room:*/
	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;

	/* Rough measure of size of vault= sum of lengths of sides */
	size = abs(x2 - x1) + abs(y2 - y1);

	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			/* Thing added based on distance to center of vault
			 * Difficulty is 1-easy to 10-hard */
			value = ((((s32b)(distance(cx, cy, x, y))) * 100) / size) + randint1(10) - difficulty_rate;

			/* hack- empty square part of the time */
			if ((randint1(100) - difficulty_rate * 3) > 50) value = 20;

			 /* if floor, shallow water and lava */
			if (is_floor_bold(y, x) ||
			    (cave_have_flag_bold(y, x, FF_PLACE) && cave_have_flag_bold(y, x, FF_DROP)))
			{
				/* The smaller 'value' is, the better the stuff */
				if (value < 0)
				{
					/* Meanest monster + treasure */
					monster_level = base_level + 40;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
					object_level = base_level + 20;
					place_object(y, x, AM_GOOD);
					object_level = base_level;
				}
				else if (value < 5)
				{
					/* Mean monster +treasure */
					monster_level = base_level + 20;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
					object_level = base_level + 10;
					place_object(y, x, AM_GOOD);
					object_level = base_level;
				}
				else if (value < 10)
				{
					/* Monster */
					monster_level = base_level + 9;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
				}
				else if (value < 17)
				{
					/* Intentional Blank space */

					/*
					 * (Want some of the vault to be empty
					 * so have room for group monsters.
					 * This is used in the hack above to lower
					 * the density of stuff in the vault.)
					 */
				}
				else if (value < 23)
				{
					/* Object or trap */
					if (randint0(100) < 25)
					{
						place_object(y, x, 0L);
					}
					else
					{
						place_trap(y, x);
					}
				}
				else if (value < 30)
				{
					/* Monster and trap */
					monster_level = base_level + 5;
					place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					monster_level = base_level;
					place_trap(y, x);
				}
				else if (value < 40)
				{
					/* Monster or object */
					if (randint0(100) < 50)
					{
						monster_level = base_level + 3;
						place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
						monster_level = base_level;
					}
					if (randint0(100) < 50)
					{
						object_level = base_level + 7;
						place_object(y, x, 0L);
						object_level = base_level;
					}
				}
				else if (value < 50)
				{
					/* Trap */
					place_trap(y, x);
				}
				else
				{
					/* Various Stuff */

					/* 20% monster, 40% trap, 20% object, 20% blank space */
					if (randint0(100) < 20)
					{
						place_monster(y, x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
					}
					else if (randint0(100) < 50)
					{
						place_trap(y, x);
					}
					else if (randint0(100) < 50)
					{
						place_object(y, x, 0L);
					}
				}

			}
		}
	}
}


/*
 * This function creates a random vault that looks like a collection of bubbles.
 * It works by getting a set of coordinates that represent the center of each
 * bubble.  The entire room is made by seeing which bubble center is closest. If
 * two centers are equidistant then the square is a wall, otherwise it is a floor.
 * The only exception is for squares really near a center, these are always floor.
 * (It looks better than without this check.)
 *
 * Note: If two centers are on the same point then this algorithm will create a
 *       blank bubble filled with walls. - This is prevented from happening.
 */
static void build_bubble_vault(int x0, int y0, int xsize, int ysize)
{
	#define BUBBLENUM 10		/* number of bubbles */

	/* array of center points of bubbles */
	coord center[BUBBLENUM];

	int i, j, x, y;
	u16b min1, min2, temp;
	bool done;

	/* Offset from center to top left hand corner */
	int xhsize = xsize / 2;
	int yhsize = ysize / 2;


	if (cheat_room) msg_print("Bubble Vault");

	/* Allocate center of bubbles */
	center[0].x = (byte)randint1(xsize - 3) + 1;
	center[0].y = (byte)randint1(ysize - 3) + 1;

	for (i = 1; i < BUBBLENUM; i++)
	{
		done = FALSE;

		/* get center and check to see if it is unique */
		while (!done)
		{
			done = TRUE;

			x = randint1(xsize - 3) + 1;
			y = randint1(ysize - 3) + 1;

			for (j = 0; j < i; j++)
			{
				/* rough test to see if there is an overlap */
				if ((x == center[j].x) && (y == center[j].y)) done = FALSE;
			}
		}

		center[i].x = x;
		center[i].y = y;
	}


	/* Top and bottom boundaries */
	for (i = 0; i < xsize; i++)
	{
		x = x0 - xhsize + i;

		place_outer_noperm_bold(y0 - yhsize + 0, x);
		cave[y0 - yhsize + 0][x].info |= (CAVE_ROOM | CAVE_ICKY);
		place_outer_noperm_bold(y0 - yhsize + ysize - 1, x);
		cave[y0 - yhsize + ysize - 1][x].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Left and right boundaries */
	for (i = 1; i < ysize - 1; i++)
	{
		y = y0 - yhsize + i;

		place_outer_noperm_bold(y, x0 - xhsize + 0);
		cave[y][x0 - xhsize + 0].info |= (CAVE_ROOM | CAVE_ICKY);
		place_outer_noperm_bold(y, x0 - xhsize + xsize - 1);
		cave[y][x0 - xhsize + xsize - 1].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Fill in middle with bubbles */
	for (x = 1; x < xsize - 1; x++)
	{
		for (y = 1; y < ysize - 1; y++)
		{
			/* Get distances to two closest centers */

			/* initialize */
			min1 = distance(x, y, center[0].x, center[0].y);
			min2 = distance(x, y, center[1].x, center[1].y);

			if (min1 > min2)
			{
				/* swap if in wrong order */
				temp = min1;
				min1 = min2;
				min2 = temp;
			}

			/* Scan the rest */
			for (i = 2; i < BUBBLENUM; i++)
			{
				temp = distance(x, y, center[i].x, center[i].y);

				if (temp < min1)
				{
					/* smallest */
					min2 = min1;
					min1 = temp;
				}
				else if (temp < min2)
				{
					/* second smallest */
					min2 = temp;
				}
			}
			if (((min2 - min1) <= 2) && (!(min1 < 3)))
			{
				/* Boundary at midpoint+ not at inner region of bubble */
				place_outer_noperm_bold(y0 - yhsize + y, x0 - xhsize + x);
			}
			else
			{
				/* middle of a bubble */
				place_floor_bold(y0 - yhsize + y, x0 - xhsize + x);
			}

			/* clean up rest of flags */
			cave[y0 - yhsize + y][x0 - xhsize + x].info |= (CAVE_ROOM | CAVE_ICKY);
		}
	}

	/* Try to add some random doors */
	for (i = 0; i < 500; i++)
	{
		x = randint1(xsize - 3) - xhsize + x0 + 1;
		y = randint1(ysize - 3) - yhsize + y0 + 1;
		add_door(x, y);
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 2, y0 - yhsize + 1, y0 - yhsize + ysize - 2, randint1(5));
}


/*
 * Overlay a rectangular room given its bounds
 * This routine is used by build_room_vault
 * The area inside the walls is not touched:
 * only granite is removed- normal walls stay
 */
static void build_room(int x1, int x2, int y1, int y2)
{
	int x, y, i, xsize, ysize, temp;

	/* Check if rectangle has no width */
	if ((x1 == x2) || (y1 == y2)) return;

	/* initialize */
	if (x1 > x2)
	{
		/* Swap boundaries if in wrong order */
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	if (y1 > y2)
	{
		/* Swap boundaries if in wrong order */
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	/* get total widths */
	xsize = x2 - x1;
	ysize = y2 - y1;


	/* Top and bottom boundaries */
	for (i = 0; i <= xsize; i++)
	{
		place_outer_noperm_bold(y1, x1 + i);
		cave[y1][x1 + i].info |= (CAVE_ROOM | CAVE_ICKY);
		place_outer_noperm_bold(y2, x1 + i);
		cave[y2][x1 + i].info |= (CAVE_ROOM | CAVE_ICKY);
	}

	/* Left and right boundaries */
	for (i = 1; i < ysize; i++)
	{
		place_outer_noperm_bold(y1 + i, x1);
		cave[y1 + i][x1].info|=(CAVE_ROOM | CAVE_ICKY);
		place_outer_noperm_bold(y1 + i, x2);
		cave[y1 + i][x2].info|=(CAVE_ROOM | CAVE_ICKY);
	}

	/* Middle */
	for (x = 1; x < xsize; x++)
	{
		for (y = 1; y < ysize; y++)
		{
			if (is_extra_bold(y1+y, x1+x))
			{
				/* clear the untouched region */
				place_floor_bold(y1 + y, x1 + x);
				cave[y1 + y][x1 + x].info |= (CAVE_ROOM | CAVE_ICKY);
			}
			else
			{
				/* make it a room- but don't touch */
				cave[y1 + y][x1 + x].info |= (CAVE_ROOM | CAVE_ICKY);
			}
		}
	}
}


/* Create a random vault that looks like a collection of overlapping rooms */

static void build_room_vault(int x0, int y0, int xsize, int ysize)
{
	int i, x1, x2, y1, y2, xhsize, yhsize;

	/* get offset from center */
	xhsize = xsize / 2;
	yhsize = ysize / 2;

	if (cheat_room) msg_print("Room Vault");

	/* fill area so don't get problems with arena levels */
	for (x1 = 0; x1 < xsize; x1++)
	{
		int x = x0 - xhsize + x1;

		for (y1 = 0; y1 < ysize; y1++)
		{
			int y = y0 - yhsize + y1;

			place_extra_bold(y, x);
			cave[y][x].info &= (~CAVE_ICKY);
		}
	}

	/* add ten random rooms */
	for (i = 0; i < 10; i++)
	{
		x1 = randint1(xhsize) * 2 + x0 - xhsize;
		x2 = randint1(xhsize) * 2 + x0 - xhsize;
		y1 = randint1(yhsize) * 2 + y0 - yhsize;
		y2 = randint1(yhsize) * 2 + y0 - yhsize;
		build_room(x1, x2, y1, y2);
	}

	/* Add some random doors */
	for (i = 0; i < 500; i++)
	{
		x1 = randint1(xsize - 3) - xhsize + x0 + 1;
		y1 = randint1(ysize - 3) - yhsize + y0 + 1;
		add_door(x1, y1);
	}

	/* Fill with monsters and treasure, high difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 2, y0 - yhsize + 1, y0 - yhsize + ysize - 2, randint1(5) + 5);
}


/* Create a random vault out of a fractal cave */
static void build_cave_vault(int x0, int y0, int xsiz, int ysiz)
{
	int grd, roug, cutoff, xhsize, yhsize, xsize, ysize, x, y;
	bool done, light, room;

	/* round to make sizes even */
	xhsize = xsiz / 2;
	yhsize = ysiz / 2;
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	if (cheat_room) msg_print("Cave Vault");

	light = done = FALSE;
	room = TRUE;

	while (!done)
	{
		/* testing values for these parameters feel free to adjust */
		grd = 1 << randint0(4);

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* about size/2 */
		cutoff = randint1(xsize / 4) + randint1(ysize / 4) +
			 randint1(xsize / 4) + randint1(ysize / 4);

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, cutoff);

		/* Convert to normal format+ clean up */
		done = generate_fracave(y0, x0, xsize, ysize, cutoff, light, room);
	}

	/* Set icky flag because is a vault */
	for (x = 0; x <= xsize; x++)
	{
		for (y = 0; y <= ysize; y++)
		{
			cave[y0 - yhsize + y][x0 - xhsize + x].info |= CAVE_ICKY;
		}
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 1, y0 - yhsize + 1, y0 - yhsize + ysize - 1, randint1(5));
}

/*
 * maze vault -- rectangular labyrinthine rooms
 *
 * maze vault uses two routines:
 *    r_visit - a recursive routine that builds the labyrinth
 *    build_maze_vault - a driver routine that calls r_visit and adds
 *                   monsters, traps and treasure
 *
 * The labyrinth is built by creating a spanning tree of a graph.
 * The graph vertices are at
 *    (x, y) = (2j + x1, 2k + y1)   j = 0,...,m-1    k = 0,...,n-1
 * and the edges are the vertical and horizontal nearest neighbors.
 *
 * The spanning tree is created by performing a suitably randomized
 * depth-first traversal of the graph. The only adjustable parameter
 * is the randint0(3) below; it governs the relative density of
 * twists and turns in the labyrinth: smaller number, more twists.
 */
/*:::迷路地形作成のためのルーチン*/
static void r_visit(int y1, int x1, int y2, int x2,
		    int node, int dir, int *visited)
{
	int i, j, m, n, temp, x, y, adj[4];

	/* dimensions of vertex array */
	m = (x2 - x1) / 2 + 1;
	n = (y2 - y1) / 2 + 1;

	/* mark node visited and set it to a floor */
	visited[node] = 1;
	x = 2 * (node % m) + x1;
	y = 2 * (node / m) + y1;
	place_floor_bold(y, x);

	/* setup order of adjacent node visits */
	if (one_in_(3))
	{
		/* pick a random ordering */
		for (i = 0; i < 4; i++)
			adj[i] = i;
		for (i = 0; i < 4; i++)
		{
			j = randint0(4);
			temp = adj[i];
			adj[i] = adj[j];
			adj[j] = temp;
		}
		dir = adj[0];
	}
	else
	{
		/* pick a random ordering with dir first */
		adj[0] = dir;
		for (i = 1; i < 4; i++)
			adj[i] = i;
		for (i = 1; i < 4; i++)
		{
			j = 1 + randint0(3);
			temp = adj[i];
			adj[i] = adj[j];
			adj[j] = temp;
		}
	}

	for (i = 0; i < 4; i++)
	{
		switch (adj[i])
		{
			case 0:
				/* (0,+) - check for bottom boundary */
				if ((node / m < n - 1) && (visited[node + m] == 0))
				{
					place_floor_bold(y + 1, x);
					r_visit(y1, x1, y2, x2, node + m, dir, visited);
				}
				break;
			case 1:
				/* (0,-) - check for top boundary */
				if ((node / m > 0) && (visited[node - m] == 0))
				{
					place_floor_bold(y - 1, x);
					r_visit(y1, x1, y2, x2, node - m, dir, visited);
				}
				break;
			case 2:
				/* (+,0) - check for right boundary */
				if ((node % m < m - 1) && (visited[node + 1] == 0))
				{
					place_floor_bold(y, x + 1);
					r_visit(y1, x1, y2, x2, node + 1, dir, visited);
				}
				break;
			case 3:
				/* (-,0) - check for left boundary */
				if ((node % m > 0) && (visited[node - 1] == 0))
				{
					place_floor_bold(y, x - 1);
					r_visit(y1, x1, y2, x2, node - 1, dir, visited);
				}
		} /* end switch */
	}
}

/*:::ミノタウロスの迷宮の処理*/
/*:::迷路型vaultの生成にも使われている*/
void build_maze_vault(int x0, int y0, int xsize, int ysize, bool is_vault)
{
	int y, x, dy, dx;
	int y1, x1, y2, x2;
	int m, n, num_vertices, *visited;
	bool light;
	cave_type *c_ptr;


	if (cheat_room && is_vault) msg_print("Maze Vault");

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && is_vault && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));

	/* Pick a random room size - randomized by calling routine */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;

	/* generate the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			c_ptr->info |= CAVE_ROOM;
			if (is_vault) c_ptr->info |= CAVE_ICKY;
			if ((x == x1 - 1) || (x == x2 + 1) || (y == y1 - 1) || (y == y2 + 1))
			{
				place_outer_grid(c_ptr);
			}
			else if (!is_vault)
			{
				place_extra_grid(c_ptr);
			}
			else
			{
				place_inner_grid(c_ptr);
			}
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* dimensions of vertex array */
	m = dx + 1;
	n = dy + 1;
	num_vertices = m * n;

	/* initialize array of visited vertices */
	C_MAKE(visited, num_vertices, int);

	/* traverse the graph to create a spaning tree, pick a random root */
	r_visit(y1, x1, y2, x2, randint0(num_vertices), 0, visited);

	/* Fill with monsters and treasure, low difficulty */
	if (is_vault) fill_treasure(x1, x2, y1, y2, randint1(5));

	C_KILL(visited, num_vertices, int);
}


/* Build a "mini" checkerboard vault
 *
 * This is done by making a permanent wall maze and setting
 * the diagonal sqaures of the checker board to be granite.
 * The vault has two entrances on opposite sides to guarantee
 * a way to get in even if the vault abuts a side of the dungeon.
 */
static void build_mini_c_vault(int x0, int y0, int xsize, int ysize)
{
	int dy, dx;
	int y1, x1, y2, x2, y, x, total;
	int m, n, num_vertices;
	int *visited;

	if (cheat_room) msg_print("Mini Checker Board Vault");

	/* Pick a random room size */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;


	/* generate the room */
	for (x = x1 - 2; x <= x2 + 2; x++)
	{
		if (!in_bounds(y1-2,x)) break;

		cave[y1-2][x].info |= (CAVE_ROOM | CAVE_ICKY);

		place_outer_noperm_bold(y1-2, x);
	}

	for (x = x1 - 2; x <= x2 + 2; x++)
	{
		if (!in_bounds(y2+2,x)) break;

		cave[y2+2][x].info |= (CAVE_ROOM | CAVE_ICKY);

		place_outer_noperm_bold(y2+2, x);
	}

	for (y = y1 - 2; y <= y2 + 2; y++)
	{
		if (!in_bounds(y,x1-2)) break;

		cave[y][x1-2].info |= (CAVE_ROOM | CAVE_ICKY);

		place_outer_noperm_bold(y, x1-2);
	}

	for (y = y1 - 2; y <= y2 + 2; y++)
	{
		if (!in_bounds(y,x2+2)) break;

		cave[y][x2+2].info |= (CAVE_ROOM | CAVE_ICKY);

		place_outer_noperm_bold(y, x2+2);
	}

	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			c_ptr->info |= (CAVE_ROOM | CAVE_ICKY);

			/* Permanent walls */
			place_inner_perm_grid(c_ptr);
		}
	}


	/* dimensions of vertex array */
	m = dx + 1;
	n = dy + 1;
	num_vertices = m * n;

	/* initialize array of visited vertices */
	C_MAKE(visited, num_vertices, int);

	/* traverse the graph to create a spannng tree, pick a random root */
	r_visit(y1, x1, y2, x2, randint0(num_vertices), 0, visited);

	/* Make it look like a checker board vault */
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			total = x - x1 + y - y1;
			/* If total is odd- and is a floor then make a wall */
			if ((total % 2 == 1) && is_floor_bold(y, x))
			{
				place_inner_bold(y, x);
			}
		}
	}

	/* Make a couple of entrances */
	if (one_in_(2))
	{
		/* left and right */
		y = randint1(dy) + dy / 2;
		place_inner_bold(y1 + y, x1 - 1);
		place_inner_bold(y1 + y, x2 + 1);
	}
	else
	{
		/* top and bottom */
		x = randint1(dx) + dx / 2;
		place_inner_bold(y1 - 1, x1 + x);
		place_inner_bold(y2 + 1, x1 + x);
	}

	/* Fill with monsters and treasure, highest difficulty */
	fill_treasure(x1, x2, y1, y2, 10);

	C_KILL(visited, num_vertices, int);
}


/* Build a town/ castle by using a recursive algorithm.
 * Basically divide each region in a probalistic way to create
 * smaller regions.  When the regions get too small stop.
 *
 * The power variable is a measure of how well defended a region is.
 * This alters the possible choices.
 */
static void build_recursive_room(int x1, int y1, int x2, int y2, int power)
{
	int xsize, ysize;
	int x, y;
	int choice;

	/* Temp variables */
	int t1, t2, t3, t4;

	xsize = x2 - x1;
	ysize = y2 - y1;

	if ((power < 3) && (xsize > 12) && (ysize > 12))
	{
		/* Need outside wall +keep */
		choice = 1;
	}
	else
	{
		if (power < 10)
		{
			/* Make rooms + subdivide */
			if ((randint1(10) > 2) && (xsize < 8) && (ysize < 8))
			{
				choice = 4;
			}
			else
			{
				choice = randint1(2) + 1;
			}
		}
		else
		{
			/* Mostly subdivide */
			choice = randint1(3) + 1;
		}
	}

	/* Based on the choice made above, do something */

	switch (choice)
	{
		case 1:
		{
			/* Outer walls */

			/* top and bottom */
			for (x = x1; x <= x2; x++)
			{
				place_outer_bold(y1, x);
				place_outer_bold(y2, x);
			}

			/* left and right */
			for (y = y1 + 1; y < y2; y++)
			{
				place_outer_bold(y, x1);
				place_outer_bold(y, x2);
			}

			/* Make a couple of entrances */
			if (one_in_(2))
			{
				/* left and right */
				y = randint1(ysize) + y1;
				place_floor_bold(y, x1);
				place_floor_bold(y, x2);
			}
			else
			{
				/* top and bottom */
				x = randint1(xsize) + x1;
				place_floor_bold(y1, x);
				place_floor_bold(y2, x);
			}

			/* Select size of keep */
			t1 = randint1(ysize / 3) + y1;
			t2 = y2 - randint1(ysize / 3);
			t3 = randint1(xsize / 3) + x1;
			t4 = x2 - randint1(xsize / 3);

			/* Do outside areas */

			/* Above and below keep */
			build_recursive_room(x1 + 1, y1 + 1, x2 - 1, t1, power + 1);
			build_recursive_room(x1 + 1, t2, x2 - 1, y2, power + 1);

			/* Left and right of keep */
			build_recursive_room(x1 + 1, t1 + 1, t3, t2 - 1, power + 3);
			build_recursive_room(t4, t1 + 1, x2 - 1, t2 - 1, power + 3);

			/* Make the keep itself: */
			x1 = t3;
			x2 = t4;
			y1 = t1;
			y2 = t2;
			xsize = x2 - x1;
			ysize = y2 - y1;
			power += 2;

			/* Fall through */
		}
		case 4:
		{
			/* Try to build a room */
			if ((xsize < 3) || (ysize < 3))
			{
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						place_inner_bold(y, x);
					}
				}

				/* Too small */
				return;
			}

			/* Make outside walls */
			/* top and bottom */
			for (x = x1 + 1; x <= x2 - 1; x++)
			{
				place_inner_bold(y1 + 1, x);
				place_inner_bold(y2 - 1, x);
			}

			/* left and right */
			for (y = y1 + 1; y <= y2 - 1; y++)
			{
				place_inner_bold(y, x1 + 1);
				place_inner_bold(y, x2 - 1);
			}

			/* Make a door */
			y = randint1(ysize - 3) + y1 + 1;

			if (one_in_(2))
			{
				/* left */
				place_floor_bold(y, x1 + 1);
			}
			else
			{
				/* right */
				place_floor_bold(y, x2 - 1);
			}

			/* Build the room */
			build_recursive_room(x1 + 2, y1 + 2, x2 - 2, y2 - 2, power + 3);
			break;
		}
		case 2:
		{
			/* Try and divide vertically */
			if (xsize < 3)
			{
				/* Too small */
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						place_inner_bold(y, x);
					}
				}
				return;
			}

			t1 = randint1(xsize - 2) + x1 + 1;
			build_recursive_room(x1, y1, t1, y2, power - 2);
			build_recursive_room(t1 + 1, y1, x2, y2, power - 2);
			break;
		}
		case 3:
		{
			/* Try and divide horizontally */
			if (ysize < 3)
			{
				/* Too small */
				for (y = y1; y < y2; y++)
				{
					for (x = x1; x < x2; x++)
					{
						place_inner_bold(y, x);
					}
				}
				return;
			}

			t1 = randint1(ysize - 2) + y1 + 1;
			build_recursive_room(x1, y1, x2, t1, power - 2);
			build_recursive_room(x1, t1 + 1, x2, y2, power - 2);
			break;
		}
	}
}


/* Build a castle */

/* Driver routine: clear the region and call the recursive
* room routine.
*
*This makes a vault that looks like a castle/ city in the dungeon.
*/
static void build_castle_vault(int x0, int y0, int xsize, int ysize)
{
	int dy, dx;
	int y1, x1, y2, x2;
	int y, x;

	/* Pick a random room size */
	dy = ysize / 2 - 1;
	dx = xsize / 2 - 1;

	y1 = y0 - dy;
	x1 = x0 - dx;
	y2 = y0 + dy;
	x2 = x0 + dx;

	if (cheat_room) msg_print("Castle Vault");

	/* generate the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave[y][x].info |= (CAVE_ROOM | CAVE_ICKY);
			/* Make everything a floor */
			place_floor_bold(y, x);
		}
	}

	/* Make the castle */
	build_recursive_room(x1, y1, x2, y2, randint1(5));

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x1, x2, y1, y2, randint1(3));
}


/*
 * Add outer wall to a floored region
 * Note: no range checking is done so must be inside dungeon
 * This routine also stomps on doors
 */
static void add_outer_wall(int x, int y, int light, int x1, int y1, int x2, int y2)
{
	cave_type *c_ptr;
	feature_type *f_ptr;
	int i, j;

	if (!in_bounds(y, x)) return;

	c_ptr = &cave[y][x];

	/* hack- check to see if square has been visited before
	* if so, then exit (use room flag to do this) */
	if (c_ptr->info & CAVE_ROOM) return;

	/* set room flag */
	c_ptr->info |= CAVE_ROOM;

	f_ptr = &f_info[c_ptr->feat];

	if (is_floor_bold(y, x))
	{
		for (i = -1; i <= 1; i++)
		{
			for (j = -1; j <= 1; j++)
			{
				if ((x + i >= x1) && (x + i <= x2) &&
					 (y + j >= y1) && (y + j <= y2))
				{
					add_outer_wall(x + i, y + j, light, x1, y1, x2, y2);
					if (light) c_ptr->info |= CAVE_GLOW;
				}
			}
		}
	}
	else if (is_extra_bold(y, x))
	{
		/* Set bounding walls */
		place_outer_bold(y, x);
		if (light) c_ptr->info |= CAVE_GLOW;
	}
	else if (permanent_wall(f_ptr))
	{
		/* Set bounding walls */
		if (light) c_ptr->info |= CAVE_GLOW;
	}
}


/*
 * Hacked distance formula - gives the 'wrong' answer.
 * Used to build crypts
 */
static int dist2(int x1, int y1, int x2, int y2,
		 int h1, int h2, int h3, int h4)
{
	int dx, dy;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	/* Basically this works by taking the normal pythagorean formula
	 * and using an expansion to express this in a way without the
	 * square root.  This approximate formula is then perturbed to give
	 * the distorted results.  (I found this by making a mistake when I was
	 * trying to fix the circular rooms.)
	 */

	/* h1-h4 are constants that describe the metric */
	if (dx >= 2 * dy) return (dx + (dy * h1) / h2);
	if (dy >= 2 * dx) return (dy + (dx * h1) / h2);
	return (((dx + dy) * 128) / 181 +
		(dx * dx / (dy * h3) + dy * dy / (dx * h3)) * h4);
	/* 128/181 is approx. 1/sqrt(2) */
}


/*
 * Build target vault.
 * This is made by two concentric "crypts" with perpendicular
 * walls creating the cross-hairs.
 */
static void build_target_vault(int x0, int y0, int xsize, int ysize)
{
	int rad, x, y;

	/* Make a random metric */
	int h1, h2, h3, h4;
	h1 = randint1(32) - 16;
	h2 = randint1(16);
	h3 = randint1(32);
	h4 = randint1(32) - 16;

	if (cheat_room) msg_print("Target Vault");

	/* work out outer radius */
	if (xsize > ysize)
	{
		rad = ysize / 2;
	}
	else
	{
		rad = xsize / 2;
	}

	/* Make floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			/* clear room flag */
			cave[y][x].info &= ~(CAVE_ROOM);

			/* Vault - so is "icky" */
			cave[y][x].info |= CAVE_ICKY;

			if (dist2(y0, x0, y, x, h1, h2, h3, h4) <= rad - 1)
			{
				/* inside- so is floor */
				place_floor_bold(y, x);
			}
			else
			{
				/* make granite outside so arena works */
				place_extra_bold(y, x);
			}

			/* proper boundary for arena */
			if (((y + rad) == y0) || ((y - rad) == y0) ||
			    ((x + rad) == x0) || ((x - rad) == x0))
			{
				place_extra_bold(y, x);
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, FALSE, x0 - rad - 1, y0 - rad - 1,
		       x0 + rad + 1, y0 + rad + 1);

	/* Add inner wall */
	for (x = x0 - rad / 2; x <= x0 + rad / 2; x++)
	{
		for (y = y0 - rad / 2; y <= y0 + rad / 2; y++)
		{
			if (dist2(y0, x0, y, x, h1, h2, h3, h4) == rad / 2)
			{
				/* Make an internal wall */
				place_inner_bold(y, x);
			}
		}
	}

	/* Add perpendicular walls */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		place_inner_bold(y0, x);
	}

	for (y = y0 - rad; y <= y0 + rad; y++)
	{
		place_inner_bold(y, x0);
	}

	/* Make inner vault */
	for (y = y0 - 1; y <= y0 + 1; y++)
	{
		place_inner_bold(y, x0 - 1);
		place_inner_bold(y, x0 + 1);
	}
	for (x = x0 - 1; x <= x0 + 1; x++)
	{
		place_inner_bold(y0 - 1, x);
		place_inner_bold(y0 + 1, x);
	}

	place_floor_bold(y0, x0);


	/* Add doors to vault */
	/* get two distances so can place doors relative to centre */
	x = (rad - 2) / 4 + 1;
	y = rad / 2 + x;

	add_door(x0 + x, y0);
	add_door(x0 + y, y0);
	add_door(x0 - x, y0);
	add_door(x0 - y, y0);
	add_door(x0, y0 + x);
	add_door(x0, y0 + y);
	add_door(x0, y0 - x);
	add_door(x0, y0 - y);

	/* Fill with stuff - medium difficulty */
	fill_treasure(x0 - rad, x0 + rad, y0 - rad, y0 + rad, randint1(3) + 3);
}


#ifdef ALLOW_CAVERNS_AND_LAKES
/*
 * This routine uses a modified version of the lake code to make a
 * distribution of some terrain type over the vault.  This type
 * depends on the dungeon depth.
 *
 * Miniture rooms are then scattered across the vault.
 */
static void build_elemental_vault(int x0, int y0, int xsiz, int ysiz)
{
	int grd, roug;
	int c1, c2, c3;
	bool done = FALSE;
	int xsize, ysize, xhsize, yhsize, x, y, i;
	int type=0;


	if (cheat_room) msg_print("Elemental Vault");

	/* round to make sizes even */
	xhsize = xsiz / 2;
	yhsize = ysiz / 2;
	xsize = xhsize * 2;
	ysize = yhsize * 2;

	if (dun_level < 25)
	{
		/* Earth vault  (Rubble) */
		type = LAKE_T_EARTH_VAULT;
	}
	else if (dun_level < 50)
	{
		/* Air vault (Trees) */
		type = LAKE_T_AIR_VAULT;
	}
	else if (dun_level < 75)
	{
		//v1.1.85
		type = apply_new_lake_type(TRUE);
		/* Water vault (shallow water) */
		if(!type) type = LAKE_T_WATER_VAULT;
	}
	else
	{
		//v1.1.85
		type = apply_new_lake_type(TRUE);
		/* Fire vault (shallow lava) */
		if (!type) type = LAKE_T_FIRE_VAULT;
	}

	while (!done)
	{
		/* testing values for these parameters: feel free to adjust */
		grd = 1 << (randint0(3));

		/* want average of about 16 */
		roug = randint1(8) * randint1(4);

		/* Make up size of various componants */
		/* Floor */
		c3 = 2 * xsize / 3;

		/* Deep water/lava */
		c1 = randint0(c3 / 2) + randint0(c3 / 2) - 5;

		/* Shallow boundary */
		c2 = (c1 + c3) / 2;

		/* make it */
		generate_hmap(y0, x0, xsize, ysize, grd, roug, c3);

		/* Convert to normal format+ clean up */
		done = generate_lake(y0, x0, xsize, ysize, c1, c2, c3, type);
	}

	/* Set icky flag because is a vault */
	for (x = 0; x <= xsize; x++)
	{
		for (y = 0; y <= ysize; y++)
		{
			cave[y0 - yhsize + y][x0 - xhsize + x].info |= CAVE_ICKY;
		}
	}

	/* make a few rooms in the vault */
	for (i = 1; i <= (xsize * ysize) / 50; i++)
	{
		build_small_room(x0 + randint0(xsize - 4) - xsize / 2 + 2,
				 y0 + randint0(ysize - 4) - ysize / 2 + 2);
	}

	/* Fill with monsters and treasure, low difficulty */
	fill_treasure(x0 - xhsize + 1, x0 - xhsize + xsize - 1,
		      y0 - yhsize + 1, y0 - yhsize + ysize - 1, randint1(5));
}
#endif /* ALLOW_CAVERNS_AND_LAKES */


/*
 * Random vaults
 */
static bool build_type10(void)
{
	int y0, x0, xsize, ysize, vtype;

	/* Get size */
	/* big enough to look good, small enough to be fairly common. */
	xsize = randint1(22) + 22;
	ysize = randint1(11) + 11;

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&y0, &x0, ysize + 1, xsize + 1)) return FALSE;

	/* Select type of vault */
#ifdef ALLOW_CAVERNS_AND_LAKES
	do
	{
		vtype = randint1(15);
	}
	while ((d_info[dungeon_type].flags1 & DF1_NO_CAVE) &&
		((vtype == 1) || (vtype == 3) || (vtype == 8) || (vtype == 9) || (vtype == 11)));
#else /* ALLOW_CAVERNS_AND_LAKES */
	do
	{
		vtype = randint1(7);
	}
	while ((d_info[dungeon_type].flags1 & DF1_NO_CAVE) &&
		((vtype == 1) || (vtype == 3)));
#endif /* ALLOW_CAVERNS_AND_LAKES */


	switch (vtype)
	{
		/* Build an appropriate room */
		case 1: case  9: build_bubble_vault(x0, y0, xsize, ysize); break;
		case 2: case 10: build_room_vault(x0, y0, xsize, ysize); break;
		case 3: case 11: build_cave_vault(x0, y0, xsize, ysize); break;
		case 4: case 12: build_maze_vault(x0, y0, xsize, ysize, TRUE); break;
		case 5: case 13: build_mini_c_vault(x0, y0, xsize, ysize); break;
		case 6: case 14: build_castle_vault(x0, y0, xsize, ysize); break;
		case 7: case 15: build_target_vault(x0, y0, xsize, ysize); break;
#ifdef ALLOW_CAVERNS_AND_LAKES
		case 8: build_elemental_vault(x0, y0, xsize, ysize); break;
#endif /* ALLOW_CAVERNS_AND_LAKES */
		/* I know how to add a few more... give me some time. */

		/* Paranoia */
		default: return FALSE;
	}

	return TRUE;
}


/*
 * Build an vertical oval room.
 * For every grid in the possible square, check the distance.
 * If it's less than the radius, make it a room square.
 *
 * When done fill from the inside to find the walls,
 */
static bool build_type11(void)
{
	int rad, x, y, x0, y0;
	int light = FALSE;

	/* Occasional light */
	if ((randint1(dun_level) <= 15) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS)) light = TRUE;

	rad = randint0(9);

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&y0, &x0, rad * 2 + 1, rad * 2 + 1)) return FALSE;

	/* Make circular floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			if (distance(y0, x0, y, x) <= rad - 1)
			{
				/* inside- so is floor */
				place_floor_bold(y, x);
			}
			else if (distance(y0, x0, y, x) <= rad + 1)
			{
				/* make granite outside so arena works */
				place_extra_bold(y, x);
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, light, x0 - rad, y0 - rad, x0 + rad, y0 + rad);

	return TRUE;
}


/*
 * Build crypt room.
 * For every grid in the possible square, check the (fake) distance.
 * If it's less than the radius, make it a room square.
 *
 * When done fill from the inside to find the walls,
 */
static bool build_type12(void)
{
	int rad, x, y, x0, y0;
	int light = FALSE;
	bool emptyflag = TRUE;

	/* Make a random metric */
	int h1, h2, h3, h4;
	h1 = randint1(32) - 16;
	h2 = randint1(16);
	h3 = randint1(32);
	h4 = randint1(32) - 16;

	/* Occasional light */
	if ((randint1(dun_level) <= 5) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS)) light = TRUE;

	rad = randint1(9);

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&y0, &x0, rad * 2 + 3, rad * 2 + 3)) return FALSE;

	/* Make floor */
	for (x = x0 - rad; x <= x0 + rad; x++)
	{
		for (y = y0 - rad; y <= y0 + rad; y++)
		{
			/* clear room flag */
			cave[y][x].info &= ~(CAVE_ROOM);

			if (dist2(y0, x0, y, x, h1, h2, h3, h4) <= rad - 1)
			{
				/* inside - so is floor */
				place_floor_bold(y, x);
			}
			else if (distance(y0, x0, y, x) < 3)
			{
				place_floor_bold(y, x);
			}
			else
			{
				/* make granite outside so arena works */
				place_extra_bold(y, x);
			}

			/* proper boundary for arena */
			if (((y + rad) == y0) || ((y - rad) == y0) ||
			    ((x + rad) == x0) || ((x - rad) == x0))
			{
				place_extra_bold(y, x);
			}
		}
	}

	/* Find visible outer walls and set to be FEAT_OUTER */
	add_outer_wall(x0, y0, light, x0 - rad - 1, y0 - rad - 1,
		       x0 + rad + 1, y0 + rad + 1);

	/* Check to see if there is room for an inner vault */
	for (x = x0 - 2; x <= x0 + 2; x++)
	{
		for (y = y0 - 2; y <= y0 + 2; y++)
		{
			if (!is_floor_bold(y, x))
			{
				/* Wall in the way */
				emptyflag = FALSE;
			}
		}
	}

	if (emptyflag && one_in_(2))
	{
		/* Build the vault */
		build_small_room(x0, y0);

		/* Place a treasure in the vault */
		place_object(y0, x0, 0L);

		/* Let's guard the treasure well */
		vault_monsters(y0, x0, randint0(2) + 3);

		/* Traps naturally */
		vault_traps(y0, x0, 4, 4, randint0(3) + 2);
	}

	return TRUE;
}


/*
 * Helper function for "trapped monster pit"
 */
static bool vault_aux_trapped_pit(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return (FALSE);

	/* No wall passing monster */
	if (r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Type 13 -- Trapped monster pits
 *
 * A trapped monster pit is a "big" room with a straight corridor in
 * which wall opening traps are placed, and with two "inner" rooms
 * containing a "collection" of monsters of a given type organized in
 * the room.
 *
 * The trapped monster pit appears as shown below, where the actual
 * monsters in each location depend on the type of the pit
 *
 *  #########################
 *  #                       #
 *  ####################### #
 *  #####001123454321100### #
 *  ###0012234567654322100# #
 *  ####################### #
 *  #           ^           #
 *  # #######################
 *  # #0012234567654322100###
 *  # ###001123454321100#####
 *  # #######################
 *  #                       #
 *  #########################
 *
 * Note that the monsters in the pit are now chosen by using "get_mon_num()"
 * to request 16 "appropriate" monsters, sorting them by level, and using
 * the "even" entries in this sorted list for the contents of the pit.
 *
 * Hack -- all of the "dragons" in a "dragon" pit must be the same "color",
 * which is handled by requiring a specific "breath" attack for all of the
 * dragons.  This may include "multi-hued" breath.  Note that "wyrms" may
 * be present in many of the dragon pits, if they have the proper breath.
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the pit.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which case
 * the pit will be empty.
 *
 * Note that "monster pits" will never contain "unique" monsters.
 */
static bool build_type13(void)
{
	static int placing[][3] = {
		{-2, -9, 0}, {-2, -8, 0}, {-3, -7, 0}, {-3, -6, 0},
		{+2, -9, 0}, {+2, -8, 0}, {+3, -7, 0}, {+3, -6, 0},
		{-2, +9, 0}, {-2, +8, 0}, {-3, +7, 0}, {-3, +6, 0},
		{+2, +9, 0}, {+2, +8, 0}, {+3, +7, 0}, {+3, +6, 0},
		{-2, -7, 1}, {-3, -5, 1}, {-3, -4, 1},
		{+2, -7, 1}, {+3, -5, 1}, {+3, -4, 1},
		{-2, +7, 1}, {-3, +5, 1}, {-3, +4, 1},
		{+2, +7, 1}, {+3, +5, 1}, {+3, +4, 1},
		{-2, -6, 2}, {-2, -5, 2}, {-3, -3, 2},
		{+2, -6, 2}, {+2, -5, 2}, {+3, -3, 2},
		{-2, +6, 2}, {-2, +5, 2}, {-3, +3, 2},
		{+2, +6, 2}, {+2, +5, 2}, {+3, +3, 2},
		{-2, -4, 3}, {-3, -2, 3},
		{+2, -4, 3}, {+3, -2, 3},
		{-2, +4, 3}, {-3, +2, 3},
		{+2, +4, 3}, {+3, +2, 3},
		{-2, -3, 4}, {-3, -1, 4},
		{+2, -3, 4}, {+3, -1, 4},
		{-2, +3, 4}, {-3, +1, 4},
		{+2, +3, 4}, {+3, +1, 4},
		{-2, -2, 5}, {-3, 0, 5}, {-2, +2, 5},
		{+2, -2, 5}, {+3, 0, 5}, {+2, +2, 5},
		{-2, -1, 6}, {-2, +1, 6},
		{+2, -1, 6}, {+2, +1, 6},
		{-2, 0, 7}, {+2, 0, 7},
		{0, 0, -1}
	};

	int y, x, y1, x1, y2, x2, xval, yval;
	int i, j;

	int what[16];

	monster_type align;

	cave_type *c_ptr;

	int cur_pit_type = pick_vault_type(pit_types, d_info[dungeon_type].pit);
	vault_aux_type *n_ptr;

	/* Only in Angband */
	if (dungeon_type != DUNGEON_ANGBAND) return FALSE;

	/* No type available */
	if (cur_pit_type < 0) return FALSE;

	n_ptr = &pit_types[cur_pit_type];

	/* Process a preparation function if necessary */
	if (n_ptr->prep_func) (*(n_ptr->prep_func))();

	/* Prepare allocation table */
	get_mon_num_prep(n_ptr->hook_func, vault_aux_trapped_pit);

	align.sub_align = SUB_ALIGN_NEUTRAL;

	/* Pick some monster types */
	for (i = 0; i < 16; i++)
	{
		int r_idx = 0, attempts = 100;
		monster_race *r_ptr = NULL;

		while (attempts--)
		{
			/* Get a (hard) monster type */
			r_idx = get_mon_num(dun_level + 0);
			r_ptr = &r_info[r_idx];

			/* Decline incorrect alignment */
			if (monster_has_hostile_align(&align, 0, 0, r_ptr)) continue;

			/* Accept this monster */
			break;
		}

		/* Notice failure */
		if (!r_idx || !attempts) return FALSE;

		/* Note the alignment */
		if (r_ptr->flags3 & RF3_ANG_CHAOS) align.sub_align |= SUB_ALIGN_EVIL;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) align.sub_align |= SUB_ALIGN_GOOD;

		what[i] = r_idx;
	}

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, 13, 25)) return FALSE;

	/* Large room */
	y1 = yval - 5;
	y2 = yval + 5;
	x1 = xval - 11;
	x2 = xval + 11;

	/* Fill with inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_inner_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
		}
	}

	/* Place the floor area 1 */
	for (x = x1 + 3; x <= x2 - 3; x++)
	{
		c_ptr = &cave[yval-2][x];
		place_floor_grid(c_ptr);
		add_cave_info(yval-2, x, CAVE_ICKY);

		c_ptr = &cave[yval+2][x];
		place_floor_grid(c_ptr);
		add_cave_info(yval+2, x, CAVE_ICKY);
	}

	/* Place the floor area 2 */
	for (x = x1 + 5; x <= x2 - 5; x++)
	{
		c_ptr = &cave[yval-3][x];
		place_floor_grid(c_ptr);
		add_cave_info(yval-3, x, CAVE_ICKY);

		c_ptr = &cave[yval+3][x];
		place_floor_grid(c_ptr);
		add_cave_info(yval+3, x, CAVE_ICKY);
	}

	/* Corridor */
	for (x = x1; x <= x2; x++)
	{
		c_ptr = &cave[yval][x];
		place_floor_grid(c_ptr);
		c_ptr = &cave[y1][x];
		place_floor_grid(c_ptr);
		c_ptr = &cave[y2][x];
		place_floor_grid(c_ptr);
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}

	/* Random corridor */
	if (one_in_(2))
	{
		for (y = y1; y <= yval; y++)
		{
			place_floor_bold(y, x2);
			place_solid_bold(y, x1-1);
		}
		for (y = yval; y <= y2 + 1; y++)
		{
			place_floor_bold(y, x1);
			place_solid_bold(y, x2+1);
		}
	}
	else
	{
		for (y = yval; y <= y2 + 1; y++)
		{
			place_floor_bold(y, x1);
			place_solid_bold(y, x2+1);
		}
		for (y = y1; y <= yval; y++)
		{
			place_floor_bold(y, x2);
			place_solid_bold(y, x1-1);
		}
	}

	/* Place the wall open trap */
	cave[yval][xval].mimic = cave[yval][xval].feat;
	cave[yval][xval].feat = feat_trap_open;

	/* Sort the entries */
	for (i = 0; i < 16 - 1; i++)
	{
		/* Sort the entries */
		for (j = 0; j < 16 - 1; j++)
		{
			int i1 = j;
			int i2 = j + 1;

			int p1 = r_info[what[i1]].level;
			int p2 = r_info[what[i2]].level;

			/* Bubble */
			if (p1 > p2)
			{
				int tmp = what[i1];
				what[i1] = what[i2];
				what[i2] = tmp;
			}
		}
	}

	/* Message */
	if (cheat_room)
	{
		/* Room type */
#ifdef JP
		msg_format("%s%sの罠ピット", n_ptr->name, pit_subtype_string(cur_pit_type, FALSE));
#else
		msg_format("Trapped monster pit (%s%s)", n_ptr->name, pit_subtype_string(cur_pit_type, FALSE));
#endif
	}

	/* Select the entries */
	for (i = 0; i < 8; i++)
	{
		/* Every other entry */
		what[i] = what[i * 2];

		if (cheat_hear)
		{
			/* Message */
			msg_print(r_name + r_info[what[i]].name);
		}
	}

	for (i = 0; placing[i][2] >= 0; i++)
	{
		y = yval + placing[i][0];
		x = xval + placing[i][1];
		place_monster_aux(0, y, x, what[placing[i][2]], PM_NO_KAGE);
	}

	return TRUE;
}


/*
 * Type 14 -- trapped rooms
 *
 * A special trap is placed at center of the room
 */
static bool build_type14(void)
{
	int y, x, y2, x2, yval, xval;
	int y1, x1, xsize, ysize;

	bool light;

	cave_type *c_ptr;
	s16b trap;

	/* Pick a room size */
	y1 = randint1(4);
	x1 = randint1(11);
	y2 = randint1(3);
	x2 = randint1(11);

	xsize = x1 + x2 + 1;
	ysize = y1 + y2 + 1;

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, ysize + 2, xsize + 2)) return FALSE;

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));


	/* Get corner values */
	y1 = yval - ysize / 2;
	x1 = xval - xsize / 2;
	y2 = yval + (ysize - 1) / 2;
	x2 = xval + (xsize - 1) / 2;


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}

	if (dun_level < 30 + randint1(30))
		trap = feat_trap_piranha;
	else
		trap = feat_trap_armageddon;

	/* Place a special trap */
	c_ptr = &cave[rand_spread(yval, ysize/4)][rand_spread(xval, xsize/4)];
	c_ptr->mimic = c_ptr->feat;
	c_ptr->feat = trap;

	/* Message */
	if (cheat_room)
	{
#ifdef JP
		msg_format("%sの部屋", f_name + f_info[trap].name);
#else
		msg_format("Room of %s", f_name + f_info[trap].name);
#endif
	}

	return TRUE;
}


/*
 * Helper function for "glass room"
 */
static bool vault_aux_lite(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return FALSE;

	/* Require lite attack */
	if (!(r_ptr->flags4 & RF4_BR_LITE) && !(r_ptr->flags5 & RF5_BA_LITE)) return FALSE;

	/* No wall passing monsters */
	if (r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)) return FALSE;

	/* No disintegrating monsters */
	if (r_ptr->flags4 & RF4_BR_DISI) return FALSE;

	return TRUE;
}

/*
 * Helper function for "glass room"
 */
static bool vault_aux_shards(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Validate the monster */
	if (!vault_monster_okay(r_idx)) return FALSE;

	/* Require shards breath attack */
	if (!(r_ptr->flags4 & RF4_BR_SHAR)) return FALSE;

	return TRUE;
}

/*
 * Hack -- determine if a template is potion
 */
static bool kind_is_potion(int k_idx)
{
	return k_info[k_idx].tval == TV_POTION;
}

/*
 * Type 15 -- glass rooms
 */
static bool build_type15(void)
{
	int y, x, y2, x2, yval, xval;
	int y1, x1, xsize, ysize;
	bool light;

	cave_type *c_ptr;

	/* Pick a room size */
	xsize = rand_range(9, 13);
	ysize = rand_range(9, 13);

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, ysize + 2, xsize + 2)) return FALSE;

	/* Choose lite or dark */
	light = ((dun_level <= randint1(25)) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS));

	/* Get corner values */
	y1 = yval - ysize / 2;
	x1 = xval - xsize / 2;
	y2 = yval + (ysize - 1) / 2;
	x2 = xval + (xsize - 1) / 2;

	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->feat = feat_glass_floor;
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr->feat = feat_glass_wall;
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
		c_ptr->feat = feat_glass_wall;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr->feat = feat_glass_wall;
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
		c_ptr->feat = feat_glass_wall;
	}

	switch (randint1(3))
	{
	case 1: /* 4 lite breathers + potion */
		{
			int dir1, dir2;

			/* Prepare allocation table */
			get_mon_num_prep(vault_aux_lite, NULL);

			/* Place fixed lite berathers */
			for (dir1 = 4; dir1 < 8; dir1++)
			{
				int r_idx = get_mon_num(dun_level);

				y = yval + 2 * ddy_ddd[dir1];
				x = xval + 2 * ddx_ddd[dir1];
				if (r_idx) place_monster_aux(0, y, x, r_idx, PM_ALLOW_SLEEP);

				/* Walls around the breather */
				for (dir2 = 0; dir2 < 8; dir2++)
				{
					c_ptr = &cave[y + ddy_ddd[dir2]][x + ddx_ddd[dir2]];
					place_inner_grid(c_ptr);
					c_ptr->feat = feat_glass_wall;
				}
			}

			/* Walls around the potion */
			for (dir1 = 0; dir1 < 4; dir1++)
			{
				y = yval + 2 * ddy_ddd[dir1];
				x = xval + 2 * ddx_ddd[dir1];
				c_ptr = &cave[y][x];
				place_inner_perm_grid(c_ptr);
				c_ptr->feat = feat_permanent_glass_wall;
				cave[yval + ddy_ddd[dir1]][xval + ddx_ddd[dir1]].info |= (CAVE_ICKY);
			}

			/* Glass door */
			dir1 = randint0(4);
			y = yval + 2 * ddy_ddd[dir1];
			x = xval + 2 * ddx_ddd[dir1];
			place_secret_door(y, x, DOOR_GLASS_DOOR);
			c_ptr = &cave[y][x];
			if (is_closed_door(c_ptr->feat)) c_ptr->mimic = feat_glass_wall;

			/* Place a potion */
			get_obj_num_hook = kind_is_potion;
			place_object(yval, xval, AM_NO_FIXED_ART);
			cave[yval][xval].info |= (CAVE_ICKY);
		}
		break;

	case 2: /* 1 lite breather + random object */
		{
			int r_idx, dir1;

			/* Pillars */
			c_ptr = &cave[y1 + 1][x1 + 1];
			place_inner_grid(c_ptr);
			c_ptr->feat = feat_glass_wall;

			c_ptr = &cave[y1 + 1][x2 - 1];
			place_inner_grid(c_ptr);
			c_ptr->feat = feat_glass_wall;

			c_ptr = &cave[y2 - 1][x1 + 1];
			place_inner_grid(c_ptr);
			c_ptr->feat = feat_glass_wall;

			c_ptr = &cave[y2 - 1][x2 - 1];
			place_inner_grid(c_ptr);
			c_ptr->feat = feat_glass_wall;

			/* Prepare allocation table */
			get_mon_num_prep(vault_aux_lite, NULL);

			r_idx = get_mon_num(dun_level);
			if (r_idx) place_monster_aux(0, yval, xval, r_idx, 0L);

			/* Walls around the breather */
			for (dir1 = 0; dir1 < 8; dir1++)
			{
				c_ptr = &cave[yval + ddy_ddd[dir1]][xval + ddx_ddd[dir1]];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
			}

			/* Curtains around the breather */
			for (y = yval - 1; y <= yval + 1; y++)
			{
				place_closed_door(y, xval - 2, DOOR_CURTAIN);
				place_closed_door(y, xval + 2, DOOR_CURTAIN);
			}
			for (x = xval - 1; x <= xval + 1; x++)
			{
				place_closed_door(yval - 2, x, DOOR_CURTAIN);
				place_closed_door(yval + 2, x, DOOR_CURTAIN);
			}

			/* Place an object */
			place_object(yval, xval, AM_NO_FIXED_ART);
			cave[yval][xval].info |= (CAVE_ICKY);
		}
		break;

	case 3: /* 4 shards breathers + 2 potions */
		{
			int dir1;

			/* Walls around the potion */
			for (y = yval - 2; y <= yval + 2; y++)
			{
				c_ptr = &cave[y][xval - 3];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
				c_ptr = &cave[y][xval + 3];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
			}
			for (x = xval - 2; x <= xval + 2; x++)
			{
				c_ptr = &cave[yval - 3][x];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
				c_ptr = &cave[yval + 3][x];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
			}
			for (dir1 = 4; dir1 < 8; dir1++)
			{
				c_ptr = &cave[yval + 2 * ddy_ddd[dir1]][xval + 2 * ddx_ddd[dir1]];
				place_inner_grid(c_ptr);
				c_ptr->feat = feat_glass_wall;
			}

			/* Prepare allocation table */
			get_mon_num_prep(vault_aux_shards, NULL);

			/* Place shard berathers */
			for (dir1 = 4; dir1 < 8; dir1++)
			{
				int r_idx = get_mon_num(dun_level);

				y = yval + ddy_ddd[dir1];
				x = xval + ddx_ddd[dir1];
				if (r_idx) place_monster_aux(0, y, x, r_idx, 0L);
			}

			/* Place two potions */
			if (one_in_(2))
			{
				get_obj_num_hook = kind_is_potion;
				place_object(yval, xval - 1, AM_NO_FIXED_ART);
				get_obj_num_hook = kind_is_potion;
				place_object(yval, xval + 1, AM_NO_FIXED_ART);
			}
			else
			{
				get_obj_num_hook = kind_is_potion;
				place_object(yval - 1, xval, AM_NO_FIXED_ART);
				get_obj_num_hook = kind_is_potion;
				place_object(yval + 1, xval, AM_NO_FIXED_ART);
			}

			for (y = yval - 2; y <= yval + 2; y++)
				for (x = xval - 2; x <= xval + 2; x++)
					cave[y][x].info |= (CAVE_ICKY);

		}
		break;
	}

	/* Message */
	if (cheat_room)
	{
#ifdef JP
		msg_print("ガラスの部屋");
#else
		msg_print("Glass room");
#endif
	}

	return TRUE;
}


/* Create a new floor room with optional light */
void generate_room_floor(int y1, int x1, int y2, int x2, int light)
{
	int y, x;

	cave_type *c_ptr;

	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Point to grid */
			c_ptr = &cave[y][x];
			place_floor_grid(c_ptr);
			c_ptr->info |= (CAVE_ROOM);
			if (light) c_ptr->info |= (CAVE_GLOW);
		}
	}
}

void generate_fill_perm_bold(int y1, int x1, int y2, int x2)
{
	int y, x;

	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Point to grid */
			place_extra_perm_bold(y, x);
		}
	}
}

/* Minimum & maximum town size */
#define MIN_TOWN_WID ((MAX_WID / 3) / 2)
#define MIN_TOWN_HGT ((MAX_HGT / 3) / 2)
#define MAX_TOWN_WID ((MAX_WID / 3) * 2 / 3)
#define MAX_TOWN_HGT ((MAX_HGT / 3) * 2 / 3)

/* Struct for build underground buildings */
typedef struct
{
	int y0, x0; /* North-west corner (relative) */
	int y1, x1; /* South-east corner (relative) */
}
ugbldg_type;

ugbldg_type *ugbldg;

/*
 * Precalculate buildings' location of underground arcade
 */
static bool precalc_ugarcade(int town_hgt, int town_wid, int n)
{
	int i, y, x, center_y, center_x, tmp, attempt = 10000;
	int max_bldg_hgt = 3 * town_hgt / MAX_TOWN_HGT;
	int max_bldg_wid = 5 * town_wid / MAX_TOWN_WID;
	ugbldg_type *cur_ugbldg;
	bool **ugarcade_used, abort;

	/* Allocate "ugarcade_used" array (2-dimension) */
	C_MAKE(ugarcade_used, town_hgt, bool *);
	C_MAKE(*ugarcade_used, town_hgt * town_wid, bool);
	for (y = 1; y < town_hgt; y++) ugarcade_used[y] = *ugarcade_used + y * town_wid;

	/* Calculate building locations */
	for (i = 0; i < n; i++)
	{
		cur_ugbldg = &ugbldg[i];
		(void)WIPE(cur_ugbldg, ugbldg_type);

		do
		{
			/* Find the "center" of the store */
			center_y = rand_range(2, town_hgt - 3);
			center_x = rand_range(2, town_wid - 3);

			/* Determine the store boundaries */
			tmp = center_y - randint1(max_bldg_hgt);
			cur_ugbldg->y0 = MAX(tmp, 1);
			tmp = center_x - randint1(max_bldg_wid);
			cur_ugbldg->x0 = MAX(tmp, 1);
			tmp = center_y + randint1(max_bldg_hgt);
			cur_ugbldg->y1 = MIN(tmp, town_hgt - 2);
			tmp = center_x + randint1(max_bldg_wid);
			cur_ugbldg->x1 = MIN(tmp, town_wid - 2);

			/* Scan this building's area */
			for (abort = FALSE, y = cur_ugbldg->y0; (y <= cur_ugbldg->y1) && !abort; y++)
			{
				for (x = cur_ugbldg->x0; x <= cur_ugbldg->x1; x++)
				{
					if (ugarcade_used[y][x])
					{
						abort = TRUE;
						break;
					}
				}
			}

			attempt--;
		}
		while (abort && attempt); /* Accept this building if no overlapping */

		/* Failed to generate underground arcade */
		if (!attempt) break;

		/*
		 * Mark to ugarcade_used[][] as "used"
		 * Note: Building-adjacent grids are included for preventing
		 * connected bulidings.
		 */
		for (y = cur_ugbldg->y0 - 1; y <= cur_ugbldg->y1 + 1; y++)
		{
			for (x = cur_ugbldg->x0 - 1; x <= cur_ugbldg->x1 + 1; x++)
			{
				ugarcade_used[y][x] = TRUE;
			}
		}
	}

	/* Free "ugarcade_used" array (2-dimension) */
	C_KILL(*ugarcade_used, town_hgt * town_wid, bool);
	C_KILL(ugarcade_used, town_hgt, bool *);

	/* If i < n, generation is not allowed */
	return i == n;
}

/*
 * Actually create buildings
 * Note: ltcy and ltcx indicate "left top corner".
 */
static void build_stores(int ltcy, int ltcx, int stores[], int n)
{
	int i, j, y, x;
	ugbldg_type *cur_ugbldg;

	for (i = 0; i < n; i++)
	{
		cur_ugbldg = &ugbldg[i];

		/* Generate new room */
		generate_room_floor(
			ltcy + cur_ugbldg->y0 - 2, ltcx + cur_ugbldg->x0 - 2,
			ltcy + cur_ugbldg->y1 + 2, ltcx + cur_ugbldg->x1 + 2,
			FALSE);
	}

	for (i = 0; i < n; i++)
	{
		cur_ugbldg = &ugbldg[i];

		/* Build an invulnerable rectangular building */
		generate_fill_perm_bold(
			ltcy + cur_ugbldg->y0, ltcx + cur_ugbldg->x0,
			ltcy + cur_ugbldg->y1, ltcx + cur_ugbldg->x1);

		/* Pick a door direction (S,N,E,W) */
		switch (randint0(4))
		{
		/* Bottom side */
		case 0:
			y = cur_ugbldg->y1;
			x = rand_range(cur_ugbldg->x0, cur_ugbldg->x1);
			break;

		/* Top side */
		case 1:
			y = cur_ugbldg->y0;
			x = rand_range(cur_ugbldg->x0, cur_ugbldg->x1);
			break;

		/* Right side */
		case 2:
			y = rand_range(cur_ugbldg->y0, cur_ugbldg->y1);
			x = cur_ugbldg->x1;
			break;

		/* Left side */
		default:
			y = rand_range(cur_ugbldg->y0, cur_ugbldg->y1);
			x = cur_ugbldg->x0;
			break;
		}

		for (j = 0; j < max_f_idx; j++)
		{
			if (have_flag(f_info[j].flags, FF_STORE))
			{
				if (f_info[j].subtype == stores[i]) break;
			}
		}

		/* Clear previous contents, add a store door */
		if (j < max_f_idx)
		{
			cave_set_feat(ltcy + y, ltcx + x, j);

			/* Init store */
		//	store_init(NO_TOWN, stores[i]);
			store_init(TOWN_UG, stores[i]);
		}
	}
}


/*
 * Type 16 -- Underground Arcade
 *
 * Town logic flow for generation of new town
 * Originally from Vanilla 3.0.3
 *
 * We start with a fully wiped cave of normal floors.
 *
 * Note that town_gen_hack() plays games with the R.N.G.
 *
 * This function does NOT do anything about the owners of the stores,
 * nor the contents thereof.  It only handles the physical layout.
 */
///bldg 地下街
static bool build_type16(void)
{
	int stores[] =
	{
		STORE_GENERAL, STORE_ARMOURY, STORE_WEAPON, STORE_TEMPLE,
		STORE_ALCHEMIST, STORE_MAGIC, STORE_BLACK, STORE_BOOK,
	};
	int n = sizeof stores / sizeof (int);
	int i, y, x, y1, x1, yval, xval;
	int town_hgt = rand_range(MIN_TOWN_HGT, MAX_TOWN_HGT);
	int town_wid = rand_range(MIN_TOWN_WID, MAX_TOWN_WID);
	bool prevent_bm = FALSE;

	/* Hack -- If already exist black market, prevent building */
	for (y = 0; (y < cur_hgt) && !prevent_bm; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			if (cave[y][x].feat == FF_STORE)
			{
				prevent_bm = (f_info[cave[y][x].feat].subtype == STORE_BLACK);
				break;
			}
		}
	}
	for (i = 0; i < n; i++)
	{
		if ((stores[i] == STORE_BLACK) && prevent_bm) stores[i] = stores[--n];
	}
	if (!n) return FALSE;

	/* Allocate buildings array */
	C_MAKE(ugbldg, n, ugbldg_type);

	/* If cannot build stores, abort */
	if (!precalc_ugarcade(town_hgt, town_wid, n))
	{
		/* Free buildings array */
		C_KILL(ugbldg, n, ugbldg_type);
		return FALSE;
	}

	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, town_hgt + 4, town_wid + 4))
	{
		/* Free buildings array */
		C_KILL(ugbldg, n, ugbldg_type);
		return FALSE;
	}

	/* Get top left corner */
	y1 = yval - (town_hgt / 2);
	x1 = xval - (town_wid / 2);

	/* Generate new room */
	generate_room_floor(
		y1 + town_hgt / 3, x1 + town_wid / 3,
		y1 + town_hgt * 2 / 3, x1 + town_wid * 2 / 3, FALSE);

	/* Build stores */
	build_stores(y1, x1, stores, n);

#ifdef JP
	if (cheat_room) msg_print("地下街");
#else
	if (cheat_room) msg_print("Underground Arcade");
#endif

	/* Free buildings array */
	C_KILL(ugbldg, n, ugbldg_type);

	return TRUE;
}



//このフロアにおけるEXダンジョン建物の出やすさ(大きいほど出やすい)を計算して返す
//idxごとに二度呼ばれて同じ値が返る必要があるのでランダム値を使わないこと
byte calc_ex_dun_bldg_prob(int ex_bldg_idx)
{
	int lev = dun_level;

	switch(ex_bldg_idx)
	{
	case BLDG_EX_STORE_GENERAL:
	case BLDG_EX_STORE_ARMOURY:
	case BLDG_EX_STORE_WEAPON:
	case BLDG_EX_STORE_TEMPLE:
	case BLDG_EX_STORE_ALCHEMIST:
	case BLDG_EX_STORE_MAGIC:
	case BLDG_EX_STORE_BOOK:
		return 15;
	case BLDG_EX_STORE_BLACK:
		return 30;

	case BLDG_EX_REIMU:
		if(p_ptr->pclass == CLASS_REIMU) return 0;
		else return 25;
	case BLDG_EX_KOGASA:
		if(p_ptr->pclass == CLASS_KOGASA) return 0;
		else return (20 - lev / 10);
	case BLDG_EX_UG_GUILDS: //ギルドが必要な職はギルドに辿り着きやすくしておく
		if(p_ptr->pclass == CLASS_ENGINEER || p_ptr->pclass == CLASS_NITORI || p_ptr->pclass == CLASS_ARCHER
		|| p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_JEWELER || p_ptr->pclass == CLASS_CHEMIST || p_ptr->pclass == CLASS_MISUMARU)
			return 100;
		else return 10;

	case BLDG_EX_PATCHOULI:
		if(p_ptr->pclass == CLASS_PATCHOULI || p_ptr->pclass == CLASS_REMY || p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_SAKUYA || p_ptr->pclass == CLASS_MEIRIN) return 60;
		else return 30;
	case BLDG_EX_SMITH:
		return 15;
	case BLDG_EX_MYSTIA:
		if(p_ptr->pclass == CLASS_MYSTIA) return 0;
		else if(lev < 30) return 15;
		else if(lev < 60) return 10;
		else return 5;
	case BLDG_EX_AKISISTERS:
		if(p_ptr->pclass == CLASS_SHIZUHA || p_ptr->pclass == CLASS_MINORIKO) return 0;
		else if(lev < 25) return 30;
		else return 10;

	case BLDG_EX_EIRIN:
		return 25;

	case BLDG_EX_FLAN:
		if(p_ptr->pclass == CLASS_FLAN) return 0;
		else if(lev < 20) return 0;
		else return 10;
	case BLDG_EX_UDONGE:
		if(p_ptr->pclass == CLASS_UDONGE) return 0;
		if (p_ptr->pclass == CLASS_KAGUYA) return 0;
		if (p_ptr->pclass == CLASS_EIRIN) return 0;
		return 15;
	case BLDG_EX_CHAOS_BLADE:
		if(lev > 70) return 0;
		else if(lev > 30) return 10;
		else return 5;
	case BLDG_EX_IKUCHI:
		if(lev < 30) return 20;
		else return 7;
	case BLDG_EX_PUFFBALL:
		if (lev < 30) return 10;
		else return 3;

	case BLDG_EX_ABANDONED:
		if(p_ptr->pclass == CLASS_MARISA && !is_special_seikaku(SEIKAKU_SPECIAL_MARISA)) return 100;
		else return 10;
	case BLDG_EX_SUKIMA:
		return 30;
	case BLDG_EX_HINA:
		if(p_ptr->pclass == CLASS_HINA) return 50;
		else return 20;
	case BLDG_EX_KOURIN:
		return 30;
	case BLDG_EX_SHINREIBYOU:
		if(p_ptr->pclass == CLASS_MIKO || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_FUTO) return 60;
		else return 30;
	case BLDG_EX_PRINT_TENGU:
		if(dun_level < 30) return 0;
		else if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU) return 20;
		else return 10;
	case BLDG_EX_ORIN:
		if(p_ptr->pclass == CLASS_ORIN) return 0;
		else return 25;

	case BLDG_EX_GRASSROOTS:
		if(p_ptr->grassroots) return 50;
		else return 25;

	case BLDG_EX_SUWAKO:
		if(p_ptr->pclass == CLASS_SUWAKO) return 0;
		else return 10;
	case BLDG_EX_ORC:
		if(lev > 30) return 0;
		else return 15;
	case BLDG_EX_MARISA:
		if(p_ptr->pclass == CLASS_MARISA) return 0;
		else return 20;

	case BLDG_EX_NAZRIN:
		if(p_ptr->pclass == CLASS_NAZRIN) return 0;
		else if(lev > 50) return 5;
		else if(lev > 25) return 20;
		else return 30;

	//v1.1.32
	case BLDG_EX_OKINA:
		if (p_ptr->pclass == CLASS_YUKARI) return 0;
		if (p_ptr->pclass == CLASS_OKINA) return 0;
		else if (lev > 40) return 15;
		else if (lev > 25) return 5;
		else return 0;
	case BLDG_EX_MORIYA:
		if (lev > 99)
			return 5;
		if (p_ptr->pclass == CLASS_KANAKO || p_ptr->pclass == CLASS_SUWAKO || p_ptr->pclass == CLASS_SANAE)
			return 30;
		else
			return 15;

	case BLDG_EX_LARVA:
		if (lev > 10) return 0;
		if (p_ptr->pclass == CLASS_LARVA) return 50;
		else return 10;

	case BLDG_EX_KEIKI:
		if (lev < 40) return 0;
		else return 10;

		//v1.1.87 月虹市場　カード売人系以外の職では出ない
	case BLDG_EX_CHIMATA:
		if (CHECK_ABLCARD_DEALER_CLASS) return 100;
		else				return 0;

		//v1.1.92 尤魔
	case BLDG_EX_YUMA:
		if (lev < 30) return 5;
		else if (lev < 60) return 10;
		else return 15;

	case BLDG_EX_ZASHIKI:
		if (p_ptr->prace == RACE_ZASHIKIWARASHI) return 30;
		else return 10;

	case BLDG_EX_HATATE:
		//すでにターゲットがいるときはそのフロアを通過するまでこの建物は出ない
		if (p_ptr->hatate_mon_search_ridx) return 0;

		if (p_ptr->pclass == CLASS_HATATE) return 50;
		else return 10;

	case BLDG_EX_GHOSTS:
		if (p_ptr->muta4 & MUT4_GHOST_CHEERS) return 0;
		else if (lev < 20) return 0;
		else if (lev < 75) return 10;
		else return 20;

	case BLDG_EX_KEIKI2:
		if (p_ptr->realm1 == TV_STONE_INCIDENT || p_ptr->realm2 == TV_STONE_INCIDENT)
		{
			return 30;
		}
		else
		{
			return 5;
		}

	case BLDG_EX_HOME:
		//渡里ニナの特技専用 EXTRA特殊建物としては出現しない
		return 0;

		//v2.1.6 チョコレートの川
	case BLDG_EX_CHOCOLATE:
		if (p_ptr->pclass == CLASS_NINA) return 30;
		else return 5;


	default:
		msg_format(_("WARNING:建物idx(%d)の出現確率が設定されていない",
                    "WARNING: Undefined rarity for building idx %d"),ex_bldg_idx);
		return 0;



	}
	return 0;
}



/*:::Extraモードのダンジョン内建物idxを決定する。*/
byte	get_extra_dungeon_building_idx(void)
{

	byte idx = 0;
	byte i,j;
	int attempt;

	for(attempt=1000;attempt>0;attempt--)
	{
		bool flag_ok = TRUE;
		int tester=0;

		//出現率計算してidxを決定
		for(j=1;j<BLDG_EX_MAX;j++)
		{
			tester += calc_ex_dun_bldg_prob(j);
		}
		if (cheat_xtra) msg_format("room_kind_prob:%d", tester);
		tester = randint1(tester);
		for(j=1;j<BLDG_EX_MAX;j++)
		{
			tester -= calc_ex_dun_bldg_prob(j);
			if(tester <= 0) break;
		}
		idx = j;
		if(j == BLDG_EX_MAX)//calc_ex_dun_bldg_prob()が二度目に同じ値を返さなかった時ここに来るかも
		{
			msg_print(_("ERROR:calc_ex_dun_bldg_prob()の計算がおかしい",
                        "ERROR: Weird math in calc_ex_dun_bldg_prob()"));
			idx = BLDG_EX_REIMU;
		}

		//過去6フロアで出た建物が続けて出づらいようにする
		for(i=0;i<EX_BUILDINGS_HISTORY_MAX;i++)
		{
//msg_format("history[%d]:%d",i,ex_buildings_history[i]);
			if(idx == ex_buildings_history[i] && !one_in_(i*2+2)) flag_ok = FALSE;
		}

		if(!flag_ok) continue;

		break;
	}
	if(!attempt)
	{
		msg_print(_("WARNING:get_extra_dungeon_building_idx()を1000回試行してidxが決まらなかった",
                    "WARNING: get_extra_dungeon_building_idx() did not pick idx after 1000 attempts"));
		return 0;
	}

	//履歴リストを更新　添字が後ろほど新しい
	for(i=0;i<EX_BUILDINGS_HISTORY_MAX-1;i++) ex_buildings_history[i] = ex_buildings_history[i+1];
	ex_buildings_history[EX_BUILDINGS_HISTORY_MAX-1] = idx;


	return idx;
}


//店一つづつにした方がいいかも
/*:::Extraモード用ダンジョン内建物の初期化 dungeon()の基本ループ前に呼ばれる*/
//フロアでセーブ＆ロードするたびに呼ばれるのでランダム値やカウント値などは使えない
void	init_extra_dungeon_buildings(void)
{
	int i,j;

	if(!dun_level)
	{
		msg_print(_("ERROR:地上でinit_extra_dungeon_buildings()が呼ばれた",
                    "ERROR: init_extra_dungeon_buildings() called on surface"));
		return;
	}
	//まず全部の店の情報を初期化
	init_buildings();

	for(i=0;i<BLDG_EX_NUM;i++)
	{switch(building_ex_idx[i])
	{
		//移動賽銭箱？
		case BLDG_EX_REIMU:
		{
			sprintf(building[i].name,_("出張賽銭箱", "Donation box"));
			sprintf(building[i].owner_name,_("博麗　霊夢", "Reimu Hakurei"));

			//てゐは20階以降でないと賽銭箱を使えない。一応スカム防止に
			if(p_ptr->pclass != CLASS_TEWI || dun_level > 20)
			{
				sprintf(building[i].act_names[0],_("お賽銭を入れる", "Donate money"));
				building[i].letters[0] = 'a';
				building[i].actions[0] = BACT_THROW_OSAISEN;
				sprintf(building[i].act_names[4],_("アイテムカードをトレードする", "Trade item cards"));
				building[i].letters[4] = 't';
				building[i].actions[4] = BACT_ITEMCARD_TRADING;
			}

			sprintf(building[i].act_names[1],_("解呪", "Remove curses"));
			building[i].letters[1] = 'b';
			building[i].actions[1] = BACT_REMOVE_CURSE;
			building[i].member_costs[1] = 1000;

			sprintf(building[i].act_names[2],_("能力回復", "Restore stats"));
			building[i].letters[2] = 'c';
			building[i].actions[2] = BACT_RESTORE;
			building[i].member_costs[2] = 1000;

			sprintf(building[i].act_names[3],_("治療", "Healing"));
			building[i].letters[3] = 'd';
			building[i].actions[3] = BACT_HEALING;
			building[i].member_costs[3] = dun_level * 100;


			break;
		}
		case BLDG_EX_MORIYA:
		{
			sprintf(building[i].name, _("守矢神社分社", "Moriya Branch Shrine"));
			sprintf(building[i].owner_name, _("(無人)", "(empty)"));

			sprintf(building[i].act_names[0], _("解呪", "Remove curses"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_REMOVE_CURSE;
			building[i].member_costs[0] = 500;

			sprintf(building[i].act_names[1], _("能力回復", "Restore stats"));
			building[i].letters[1] = 'b';
			building[i].actions[1] = BACT_RESTORE;
			building[i].member_costs[1] = 600;

			sprintf(building[i].act_names[2], _("神様職業案内", "Deity job advisor"));
			building[i].letters[2] = 'c';
			building[i].actions[2] = BACT_HELLO_WORK;
			building[i].member_costs[2] = 5000;


			break;
		}
		case BLDG_EX_MARISA:
		{
			sprintf(building[i].name,_("移動霧雨魔法店", "Portable Kirisame Magic Shop"));
			sprintf(building[i].owner_name,_("霧雨　魔理沙", "Marisa Kirisame"));

			sprintf(building[i].act_names[0],_("アイテムカードをトレードする", "Trade item cards"));
			building[i].letters[0] = 't';
			building[i].actions[0] = BACT_ITEMCARD_TRADING;

			sprintf(building[i].act_names[2],_("アイテムを全て鑑定する", "Identify inventory"));
			building[i].letters[2] = 'i';
			building[i].actions[2] = BACT_IDENTS;
			building[i].member_costs[2] = 500;

			sprintf(building[i].act_names[4], _("食事する", "Buy food and drink"));
			building[i].letters[4] = 'f';
			building[i].actions[4] = BACT_FOOD;
			building[i].member_costs[4] = 100;


			break;
		}
		case BLDG_EX_KOGASA:
		{
			sprintf(building[i].name,_("小傘の仕事場", "Kogasa's Workshop"));
			sprintf(building[i].owner_name,_("多々良　小傘", "Kogasa Tatara"));
			sprintf(building[i].act_names[0],_("鍛冶を頼む", "Request blacksmith services"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_KOGASA_SMITH;
			break;
		}

		case BLDG_EX_UG_GUILDS:
		{
			sprintf(building[i].name,_("地下ギルド", "Underground Guild"));
			sprintf(building[i].owner_name,_("赤帽子の河童", "Red-hatted Kappa"));

			sprintf(building[i].act_names[0],_("矢を強化する", "Enchant arrows"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_ARCHER_GUILD_ARROW_ENC;

			sprintf(building[i].act_names[1],_("矢に属性付与する", "Brand arrows"));
			building[i].letters[1] = 'b';
			building[i].actions[1] = BACT_ARCHER_GUILD_ARROW_EGO;

			sprintf(building[i].act_names[2],_("機械を制作する", "Create machines"));
			building[i].letters[2] = 'c';
			building[i].actions[2] =  BACT_MAKE_MACHINE;

			sprintf(building[i].act_names[3],_("消耗品を調達する", "Request supplies"));
			building[i].letters[3] = 'd';
			building[i].actions[3] = BACT_BUY_MACHINE_SUPPLY;

			sprintf(building[i].act_names[4],_("鍛冶宝飾工房を使う", "Blacksmith/Jewelery workshop"));
			building[i].letters[4] = 'e';
			building[i].actions[4] = BACT_SMITH_GUILD;

			sprintf(building[i].act_names[5],_("アイテムを鑑定する", "Identify item"));
			building[i].letters[5] = 'f';
			building[i].actions[5] = BACT_IDENT_ONE;
			building[i].member_costs[5] = 50;

			//v1.1.62 ひとつ下と逆になってたので修正
			sprintf(building[i].act_names[6],_("調剤所を確認する", "Check chemistry recipes"));
			building[i].letters[6] = 'g';
			building[i].actions[6] = BACT_CHECK_DRUG_RECIPE;

			sprintf(building[i].act_names[7],_("調剤室を借りる", "Rent a pharmacy room"));
			building[i].letters[7] = 'h';
			building[i].actions[7] = BACT_COMPOUND_DRUG;

			break;
		}
		case BLDG_EX_PATCHOULI:
		{
			sprintf(building[i].name,_("パチュリーの隠し部屋", "Patchouli's Hidden Room"));
			sprintf(building[i].owner_name,_("小悪魔", "Koakuma"));

			sprintf(building[i].act_names[0],_("アイテム調査", "Research item"));
			building[i].letters[0] = 'i';
			building[i].actions[0] = BACT_RESEARCH_ITEM;
			//パチュリーは*鑑定*無料
			if(p_ptr->pclass != CLASS_PATCHOULI) building[i].member_costs[0] = 500;
			sprintf(building[i].act_names[2],_("魔道具を充填する", "Recharge magic device"));
			building[i].letters[2] = 'r';
			building[i].actions[2] = BACT_RECHARGE;
			sprintf(building[i].act_names[3],_("魔道具を全て充填する", "Recharge all magic devices"));
			building[i].letters[3] = 'a';
			building[i].actions[3] = BACT_RECHARGE_ALL;

			break;
		}
		case BLDG_EX_MYSTIA:
		{
			sprintf(building[i].name,_("屋台", "Stall"));
			sprintf(building[i].owner_name,_("ミスティア・ローレライ", "Mystia Lorelei"));
			sprintf(building[i].act_names[0],_("食事をする", "Buy food"));
			building[i].letters[0] = 'f';
			building[i].actions[0] = BACT_FOOD;
			building[i].member_costs[0] = 50;
		}
		break;
		case BLDG_EX_AKISISTERS:
		{
			sprintf(building[i].name,_("屋台", "Stall"));
			sprintf(building[i].owner_name,_("秋静葉・秋穣子", "Shizuha & Minoriko Aki"));
			sprintf(building[i].act_names[0],_("焼き芋を買う", "Buy roast sweet potatoes"));
			building[i].letters[0] = 'p';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_HINA:
		{
			sprintf(building[i].name,_("屋台", "Stall"));
			sprintf(building[i].owner_name,_("(無人)", "(empty)"));

			sprintf(building[i].act_names[0],_("人形を買う", "Buy a doll"));
			building[i].letters[0] = 'p';
			building[i].actions[0] = BACT_BUY_HINA_NINGYOU;
			sprintf(building[i].act_names[1],_("売上を回収する", "Collect sales"));
			building[i].letters[1] = 'g';
			building[i].actions[1] = BACT_HINA_MAKE_MONEY;
		}
		break;

		case BLDG_EX_EIRIN:
		{
			sprintf(building[i].name,_("永琳の出張診療所", "Eirin's Clinic"));
			sprintf(building[i].owner_name,_("八意　永琳", "Eirin Yagokoro"));

			sprintf(building[i].act_names[0],_("治療を受ける", "Receive healing"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_HEALING_PLUS;
			building[i].member_costs[0] = dun_level * 200;

			sprintf(building[i].act_names[1],_("能力低下治療", "Recover lowered stats"));
			building[i].letters[1] = 'b';
			building[i].actions[1] = BACT_RESTORE;
			building[i].member_costs[1] = 500;

			sprintf(building[i].act_names[2],_("突然変異治療", "Cure mutations"));
			building[i].letters[2] = 'c';
			building[i].actions[2] = BACT_LOSE_MUTATION;
			building[i].member_costs[2] = 5000;

			sprintf(building[i].act_names[4], _("食事する", "Buy food and drink"));
			building[i].letters[4] = 'f';
			building[i].actions[4] = BACT_FOOD;
			building[i].member_costs[4] = 10;


		}
		break;

		case BLDG_EX_SMITH:
		{
			sprintf(building[i].name,_("鬼の地下工房", "Oni Underground Workshop"));
			sprintf(building[i].owner_name,_("流れの鍛冶職人", "Blacksmith"));

			sprintf(building[i].act_names[0],_("武器を強化する", "Enchant weapon"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_ENCHANT_WEAPON;
			building[i].member_costs[0] = 300;

			sprintf(building[i].act_names[1],_("防具を強化する", "Enchant armor"));
			building[i].letters[1] = 'b';
			building[i].actions[1] = BACT_ENCHANT_ARMOR;
			building[i].member_costs[1] = 200;

			sprintf(building[i].act_names[2],_("弓を強化する", "Enchant bow"));
			building[i].letters[2] = 'c';
			building[i].actions[2] = BACT_ENCHANT_BOW;
			building[i].member_costs[2] = 400;

			sprintf(building[i].act_names[3],_("矢を強化する", "Enchant arrows"));
			building[i].letters[3] = 'd';
			building[i].actions[3] = BACT_ENCHANT_ARROWS;
			building[i].member_costs[3] = 10;

			sprintf(building[i].act_names[4],_("折れた武器を修復する", "Repair broken weapon"));
			building[i].letters[4] = 'e';
			building[i].actions[4] = BACT_BROKEN_WEAPON;
			building[i].member_costs[4] = 1000;

		}
		break;
		case BLDG_EX_FLAN:
		case BLDG_EX_CHAOS_BLADE:
		case BLDG_EX_SUKIMA:
		{
			sprintf(building[i].name,_("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name,_("(無人)", "(empty)"));

			sprintf(building[i].act_names[0],_("辺りを調べる", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_SUWAKO:
		{
			sprintf(building[i].name,_("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name,_("洩矢　諏訪子", "Suwako Moriya"));

			sprintf(building[i].act_names[0],_("様子を見る", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_UDONGE:
		{
			sprintf(building[i].name,_("開けた場所", "Open area"));
			sprintf(building[i].owner_name,_("鈴仙・優曇華院・イナバ", "Reisen Udongein Inaba"));

			sprintf(building[i].act_names[0],_("辺りを見る", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_KOURIN:
		{
			sprintf(building[i].name,_("開けた場所", "Open area"));
			sprintf(building[i].owner_name,_("森近　霖之助", "Rinnosuke Morichika"));

			sprintf(building[i].act_names[0],_("アイテムを調査してもらう", "Research item"));
			building[i].letters[0] = 'i';
			building[i].actions[0] = BACT_RESEARCH_ITEM;
			building[i].member_costs[0] = 300;
			sprintf(building[i].act_names[2],_("珍品を売りつける", "Sell curios"));
			building[i].letters[2] = 's';
			building[i].actions[2] = BACT_SELL_CURIOSITY;

		}
		break;
		case BLDG_EX_IKUCHI:
		case BLDG_EX_PUFFBALL:
		{
			sprintf(building[i].name,_("開けた場所", "Open area"));
			sprintf(building[i].owner_name,_("無人", "Empty"));

			sprintf(building[i].act_names[0],_("辺りを見る", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_GRASSROOTS:
		{
			sprintf(building[i].name,_("開けた場所", "Open area"));
			sprintf(building[i].owner_name,_("草の根妖怪のキャンプ", "Grassroots Youkai Camp"));

			if(p_ptr->pclass != CLASS_KAGEROU)
			{
				sprintf(building[i].act_names[0],_("影狼と話す", "Talk to Kagerou"));
				building[i].letters[0] = 'k';
				building[i].actions[0] = BACT_TALK_KAGEROU;
			}
			sprintf(building[i].act_names[2],_("物々交換を申し出る", "Exchange items"));
			building[i].letters[2] = 'b';
			building[i].actions[2] = BACT_BARTER;
			sprintf(building[i].act_names[4],_("アイテムカードをトレードする", "Trade item cards"));
			building[i].letters[4] = 't';
			building[i].actions[4] = BACT_ITEMCARD_TRADING;
			//v1.1.18 EXTRA用座談会追加
			if(p_ptr->grassroots)
			{
				//talk_kagerou()のネットワーク加入後にもこれと同じ処理をしている
				sprintf(building[i].act_names[6],_("雑談をする", "Chat"));
				building[i].letters[6] = 'r';
				building[i].actions[6] = BACT_EX_RUMOR_NEW;
			}
		}
		break;


		case BLDG_EX_PRINT_TENGU:
		{
			sprintf(building[i].name,_("天狗の印刷所", "Tengu Printing Shop"));
			sprintf(building[i].owner_name,_("山伏天狗の技師", "Yamabushi Tengu"));

			sprintf(building[i].act_names[0],_("話しかける", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_ABANDONED:
		case BLDG_EX_ORC:
		{
			sprintf(building[i].name,_("廃屋", "Abandoned house"));
			sprintf(building[i].owner_name,_("(無人)", "(empty)"));

			sprintf(building[i].act_names[0],_("屋内を探索する", "Search inside"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;

			if(building_ex_idx[i] == BLDG_EX_ABANDONED && p_ptr->pclass == CLASS_MARISA && !is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
			{
				sprintf(building[i].act_names[1],_("魔法を実験する", "Experiment with spells"));
				building[i].letters[1] = 'b';
				building[i].actions[1] = BACT_MARISA_MAKE_MAGIC_RECIPE;

				sprintf(building[i].act_names[2],_("魔法の効果を確認する", "Check spell effects"));
				building[i].letters[2] = 'c';
				building[i].actions[2] = BACT_MARISA_CHECK_MAGIC;

				sprintf(building[i].act_names[3],_("魔力を抽出する", "Extract magical power"));
				building[i].letters[3] = 'd';
				building[i].actions[3] = BACT_MARISA_EXTRACT_MATERIAL;

				sprintf(building[i].act_names[4],_("魔力の備蓄を確認する", "Check stored magical power"));
				building[i].letters[4] = 'e';
				building[i].actions[4] = BACT_MARISA_CHECK_ESSENCE;

				sprintf(building[i].act_names[5],_("メモを読む", "Read memo"));
				building[i].letters[5] = 'f';
				building[i].actions[5] = BACT_MARISA_READ_MEMO;

				sprintf(building[i].act_names[6],_("魔法を作る", "Create spells"));
				building[i].letters[6] = 'g';
				building[i].actions[6] = BACT_MARISA_MAKE_MAGIC;

				sprintf(building[i].act_names[7],_("持っていく魔法を選ぶ", "Take spells"));
				building[i].letters[7] = 'h';
				building[i].actions[7] = BACT_MARISA_CARRY_MAGIC;
			}
		}
		break;
		case BLDG_EX_SHINREIBYOU:
		{
			sprintf(building[i].name,_("神霊廟出張所", "Divine Spirit Mausoleum Branch Office"));
			sprintf(building[i].owner_name,_("豊聡耳　神子", "Toyosatomimi no Miko"));
			sprintf(building[i].act_names[0],_("アイテムを拠点に送る", "Send item to exploration base"));
			building[i].letters[0] = 'i';
			building[i].actions[0] = BACT_ITEM_TRANSFER;
			if(p_ptr->pclass == CLASS_MIKO) building[i].member_costs[0] = 0;
			else if(p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_FUTO) building[i].member_costs[0] = 100;
			else building[i].member_costs[0] = 500;

			sprintf(building[i].act_names[2],_("魔道具を充填する", "Recharge magic device"));
			building[i].letters[2] = 'r';
			building[i].actions[2] = BACT_RECHARGE;
			sprintf(building[i].act_names[3],_("魔道具を全て充填する", "Recharge all magic devices"));
			building[i].letters[3] = 'a';
			building[i].actions[3] = BACT_RECHARGE_ALL;

			break;
		}

		case BLDG_EX_ORIN:
		{
			sprintf(building[i].name,_("開けた場所", "Open area"));
			sprintf(building[i].owner_name,_("火焔猫　燐", "Rin Kaenbyou"));
			sprintf(building[i].act_names[0],_("アイテムを拠点に送る", "Send item to base"));
			building[i].letters[0] = 'i';
			building[i].actions[0] = BACT_ITEM_TRANSFER;
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI || p_ptr->pclass == CLASS_UTSUHO) building[i].member_costs[0] = 0;
			else building[i].member_costs[0] = 500;

			sprintf(building[i].act_names[2],_("アイテムを拠点から運んでもらう", "Receive item from base"));
			building[i].letters[2] = 't';
			building[i].actions[2] = BACT_ORIN_DELIVER_ITEM;
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI || p_ptr->pclass == CLASS_UTSUHO) building[i].member_costs[0] = 0;
			else building[i].member_costs[2] = 1000;

			break;
		}

		case BLDG_EX_NAZRIN:
		{
			sprintf(building[i].name,_("廃屋", "Abandoned house"));
			sprintf(building[i].owner_name,_("ナズーリン", "Nazrin"));

			sprintf(building[i].act_names[0],_("話しかける", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;

			sprintf(building[i].act_names[2],_("アイテムを鑑定する", "Identify item"));
			building[i].letters[2] = 'f';
			building[i].actions[2] = BACT_IDENT_ONE;
			if(p_ptr->pclass == CLASS_SHOU || p_ptr->pclass == CLASS_BYAKUREN) building[i].member_costs[2] = 10;
			else building[i].member_costs[2] = 50;

		}
		break;
		case BLDG_EX_OKINA:
		{
			sprintf(building[i].name,_("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name,_("摩多羅 隠岐奈", "Okina Matara"));

			sprintf(building[i].act_names[0],_("話しかける", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_LARVA:
		{
			if (p_ptr->pclass == CLASS_LARVA)
			{
				sprintf(building[i].name, _("あなたの別荘", "Your vacation home"));
				sprintf(building[i].owner_name, _("(無人)", "(empty)"));
				sprintf(building[i].act_names[0], _("屋内を探索する", "Search inside"));
				building[i].letters[0] = 'a';
				building[i].actions[0] = BACT_EX_SEARCH_AROUND;

				sprintf(building[i].act_names[1], _("休憩していく", "Rest"));
				building[i].letters[1] = 'r';
				building[i].actions[1] = BACT_REST;
				building[i].member_costs[1] = 0;
				sprintf(building[i].act_names[2], _("食事する", "Make food"));
				building[i].letters[2] = 'f';
				building[i].actions[2] = BACT_FOOD;
				building[i].member_costs[2] = 0;
			}
			else
			{
				sprintf(building[i].name, _("奇妙な場所", "Strange place"));
				sprintf(building[i].owner_name, _("(無人)", "(empty)"));
				sprintf(building[i].act_names[0], _("屋内を探索する", "Search inside"));
				building[i].letters[0] = 'a';
				building[i].actions[0] = BACT_EX_SEARCH_AROUND;

			}
		}
		break;
		case BLDG_EX_KEIKI:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("埴安神 袿姫", "Keiki Haniyasushin"));

			sprintf(building[i].act_names[0], _("話しかける", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;
		case BLDG_EX_KEIKI2:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("埴安神 袿姫", "Keiki Haniyasushin"));

			sprintf(building[i].act_names[0], _("話しかける", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;

		case BLDG_EX_CHIMATA:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("千亦＆龍＆典＆百々世", "Chimata&Megumu&Tsukasa&Momoyo"));

			sprintf(building[i].act_names[0], _("アビリティカード交換会", "Trade ability cards"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_TRADING_ABILITY_CARD;

			sprintf(building[i].act_names[1], _("アビリティカードを売却する", "Sell ability cards"));
			building[i].letters[1] = 's';
			building[i].actions[1] = BACT_SELL_ABILITY_CARD;

			sprintf(building[i].act_names[2], _("アビリティカードを買う", "Buy ability cards"));
			building[i].letters[2] = 'p';
			building[i].actions[2] = BACT_BUY_ABILITY_CARD;

			sprintf(building[i].act_names[3], _("アビリティカードを調べる", "Check ability cards"));
			building[i].letters[3] = 'x';
			building[i].actions[3] = BACT_CHECK_ABILITY_CARD;

		}
		break;


		case BLDG_EX_YUMA:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("饕餮　尤魔", "Yuuma Toutetsu"));

			sprintf(building[i].act_names[0], _("話を聞く", "Talk"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_RUMOR_NEW;


			sprintf(building[i].act_names[2], _("食料とアイテムカードを交換する", "Exchange food for item cards"));
			building[i].letters[2] = 't';
			building[i].actions[2] = BACT_ITEMCARD_TRADING;

			sprintf(building[i].act_names[4], _("呪われた装備品を食べてもらう", "Feed cursed equipment"));
			building[i].letters[4] = 'e';
			building[i].actions[4] = BACT_DESTROY_ITEM;

		}
		break;

		case BLDG_EX_ZASHIKI:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("テレワーク中の座敷わらし達", "Zashiki-Warashi"));

			sprintf(building[i].act_names[0], _("解呪", "Remove curse"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_REMOVE_CURSE;
			building[i].member_costs[0] = 500;

			sprintf(building[i].act_names[1], _("鑑定", "Identify"));
			building[i].letters[1] = 'i';
			building[i].actions[1] = BACT_IDENT_ONE;
			building[i].member_costs[1] = 50;

			sprintf(building[i].act_names[2], _("探索拠点を使う(一度のみ)", "Exploration base (one time)"));
			building[i].letters[2] = 'h';
			building[i].actions[2] = BACT_EX_SEARCH_AROUND;

			sprintf(building[i].act_names[4], _("食事する", "Buy food and drink"));
			building[i].letters[4] = 'f';
			building[i].actions[4] = BACT_FOOD;
			building[i].member_costs[4] = 10;

		}
		break;

		case BLDG_EX_HATATE:
		{
			if (p_ptr->pclass == CLASS_HATATE)
			{

				sprintf(building[i].name, _("我がアジト", "Your hideout"));
				sprintf(building[i].owner_name, _("姫海棠　はたて", "Hatate Himekaidou"));

				sprintf(building[i].act_names[0], _("人探しをする", "Search for someone"));
				building[i].letters[0] = 's';
				building[i].actions[0] = BACT_HATATE_SEARCH_MON;
				building[i].member_costs[0] = 0;

				sprintf(building[i].act_names[2], _("鑑定", "Identify"));
				building[i].letters[2] = 'i';
				building[i].actions[2] = BACT_IDENT_ONE;
				building[i].member_costs[2] = 0;

				sprintf(building[i].act_names[4], _("休憩していく", "Rest"));
				building[i].letters[4] = 'r';
				building[i].actions[4] = BACT_REST;
				building[i].member_costs[4] = 0;
				sprintf(building[i].act_names[5], _("食事する", "Get food and drink"));
				building[i].letters[5] = 'f';
				building[i].actions[5] = BACT_FOOD;
				building[i].member_costs[5] = 0;

			}
			else
			{
				sprintf(building[i].name, _("殺風景な部屋", "Dull boring room"));
				sprintf(building[i].owner_name, _("姫海棠　はたて", "Hatate Himekaidou"));

				sprintf(building[i].act_names[0], _("人探しを頼む", "Search for someone"));
				building[i].letters[0] = 's';
				building[i].actions[0] = BACT_HATATE_SEARCH_MON;
				building[i].member_costs[0] = dun_level * 100;

				sprintf(building[i].act_names[2], _("鑑定", "Identify"));
				building[i].letters[2] = 'i';
				building[i].actions[2] = BACT_IDENT_ONE;
				building[i].member_costs[2] = 50;

			}

		}
		break;

		case BLDG_EX_GHOSTS:
		{
			sprintf(building[i].name, _("開けた場所", "Open area"));
			sprintf(building[i].owner_name, _("怨霊の群れ", "Swarm of vengeful spirits"));

			sprintf(building[i].act_names[0], _("辺りを見る", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;

		case BLDG_EX_CHOCOLATE:
		{
			sprintf(building[i].name, _("奇妙な場所", "Strange place"));
			sprintf(building[i].owner_name, _("無人", "empty"));

			sprintf(building[i].act_names[0], _("辺りを見る", "Look around"));
			building[i].letters[0] = 'a';
			building[i].actions[0] = BACT_EX_SEARCH_AROUND;
		}
		break;


	}
	}
}



//難易度EXTRAのダンジョン内特殊建物の生成 建てた建物のIDXを返す。建たなかったら0
//bldg_array_idx:building_ex_idx[4]の添字
//0:難易度EXTRA
//1-2:一応将来的にEXTRA建物を増やすなどの用途に使えるように半端にコードを組んでいるが完全なものではないので追加時はちゃんと見直す
//3:ニナの特技で生成する建物に使うのでここでは扱わない
static byte build_type_ex(int bldg_array_idx)
{
	int i, y, x, y1, x1, y2, x2, yval, xval,xsize,ysize;
	cave_type *c_ptr;

	switch (bldg_array_idx)
	{
	case BLDG_EX_MODE_EXTRA:
		//BUILDING_EX1(SUBTYPE=0)を使用する。建物情報をbuilding_ex_idx[0]に格納
		//1-8のときBUILDING_EXでなく店が作られる
		building_ex_idx[bldg_array_idx] = get_extra_dungeon_building_idx();

		//テスト用
		if (p_ptr->wizard) building_ex_idx[bldg_array_idx] = BLDG_EX_CHOCOLATE;
		break;

	case BLDG_EX_MODE_NINA:
		msg_format(_("build_type_ex()にニナ特技用の記憶域が指定された", "Memory area for Nina's special ability was specified in build_type_ex()"));
		return 0;

	default:
		msg_format(_("build_type_ex()に未実装の値(%d)が渡された", "Unimplemented value (%d) passed to build_type_ex()"), bldg_array_idx);
		return 0;

	}


	/* Find and reserve some space in the dungeon.  Get center of room. */
	if (!find_space(&yval, &xval, ysize, xsize)) return 0;

	/* Get corner values */
	y1 = yval - ysize / 2;
	x1 = xval - xsize / 2;
	y2 = yval + (ysize - 1) / 2;
	x2 = xval + (xsize - 1) / 2;

	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			c_ptr = &cave[y][x];
			if(y == yval && x == xval)
			{
				switch(building_ex_idx[bldg_array_idx])
				{
				case BLDG_EX_STORE_GENERAL:
					cave_set_feat(y, x, f_tag_to_index_in_init("GENERAL_STORE"));
					store_init(TOWN_UG, STORE_GENERAL);
					break;

				case BLDG_EX_STORE_ARMOURY:
					cave_set_feat(y, x, f_tag_to_index_in_init("ARMOURY"));
					store_init(TOWN_UG, STORE_ARMOURY);
					break;

				case BLDG_EX_STORE_WEAPON:
					cave_set_feat(y, x, f_tag_to_index_in_init("WEAPON_SMITHS"));
					store_init(TOWN_UG, STORE_WEAPON);
					break;

				case BLDG_EX_STORE_TEMPLE:
					cave_set_feat(y, x, f_tag_to_index_in_init("TEMPLE"));
					store_init(TOWN_UG, STORE_TEMPLE);
					break;
				case BLDG_EX_STORE_ALCHEMIST:
					cave_set_feat(y, x, f_tag_to_index_in_init("ALCHEMY_SHOP"));
					store_init(TOWN_UG, STORE_ALCHEMIST);
					break;
				case BLDG_EX_STORE_MAGIC:
					cave_set_feat(y, x, f_tag_to_index_in_init("MAGIC_SHOP"));
					store_init(TOWN_UG, STORE_MAGIC);
					break;
				case BLDG_EX_STORE_BLACK:
					cave_set_feat(y, x, f_tag_to_index_in_init("BLACK_MARKET"));
					store_init(TOWN_UG, STORE_BLACK);
					break;
				case BLDG_EX_STORE_BOOK:
					cave_set_feat(y, x, f_tag_to_index_in_init("BOOKSTORE"));
					store_init(TOWN_UG, STORE_BOOK);
					break;


				default:
					cave_set_feat(y, x, f_tag_to_index_in_init(format("BUILDING_EX%d", bldg_array_idx+1)));
					break;
				}
			}
			else if((y == yval) || (x == xval) || (y > y1 && y < y2 && x > x1 && x < x2))
			{
				place_floor_grid(c_ptr);
			}
			else
			{
				place_inner_perm_grid(c_ptr);
			}

			//v1.1.61 フラグ修正　CAVE_INNERは「部屋の中の壁」を指すらしくこのフラグのせいでstreamerの処理が変になってた
			//..のだが、CAVE_INNERを消したら時々店入り口が消える。tunnel生成関数が悪さをしているらしい。
			//よく分からんのでbuild_streamer()内でCAVE_VALUTのグリッドにはstreamerを生成しないことにした
			c_ptr->info |= (CAVE_INNER | CAVE_ROOM | CAVE_ICKY | CAVE_VAULT);
		}
	}

	/* Walls around the room */
	/*
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		c_ptr = &cave[y][x1 - 1];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y][x2 + 1];
		place_outer_grid(c_ptr);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		c_ptr = &cave[y1 - 1][x];
		place_outer_grid(c_ptr);
		c_ptr = &cave[y2 + 1][x];
		place_outer_grid(c_ptr);
	}

	place_floor_bold(y1 - 1, xval);
	place_floor_bold(y2 + 1, xval);
	place_floor_bold(yval, x1 - 1);
	place_floor_bold(yval, x2 + 1);
	*/

	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		x = x1 - 1;
		if (!in_bounds(y, x)) continue;
		place_floor_bold(y, x);
	}

	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		x = x2 + 1;
		if (!in_bounds(y, x)) continue;
		place_floor_bold(y, x);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		y = y1 - 1;
		if (!in_bounds(y, x)) continue;
		place_floor_bold(y, x);
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		y = y2 + 1;
		if (!in_bounds(y, x)) continue;
		place_floor_bold(y, x);
	}



	if (cheat_room) msg_format(_("EXTRA用特殊建築物:idx%d", "Special extra building: ID is %d"), building_ex_idx[bldg_array_idx]);

	return building_ex_idx[bldg_array_idx];
}


/*
 * Attempt to build a room of the given type at the given block
 *
 * Note that we restrict the number of "crowded" rooms to reduce
 * the chance of overflowing the monster list during level creation.
 */
static bool room_build(int typ)
{
	/* Build a room */
	switch (typ)
	{
	/* Build an appropriate room */
	case ROOM_T_NORMAL:        return build_type1();
	case ROOM_T_OVERLAP:       return build_type2();
	case ROOM_T_CROSS:         return build_type3();
	case ROOM_T_INNER_FEAT:    return build_type4();
	case ROOM_T_NEST:          return build_type5();
	case ROOM_T_PIT:           return build_type6();
	case ROOM_T_LESSER_VAULT:  return build_type7();
	case ROOM_T_GREATER_VAULT: return build_type8();
	case ROOM_T_FRACAVE:       return build_type9();
	case ROOM_T_RANDOM_VAULT:  return build_type10();
	case ROOM_T_OVAL:          return build_type11();
	case ROOM_T_CRYPT:         return build_type12();
	case ROOM_T_TRAP_PIT:      return build_type13();
	case ROOM_T_TRAP:          return build_type14();
	case ROOM_T_GLASS:         return build_type15();
	case ROOM_T_ARCADE:        return build_type16();
	}

	/* Paranoia */
	return FALSE;
}


#define MOVE_PLIST(dst, src) (prob_list[dst] += prob_list[src], prob_list[src] = 0)

/*
 * [from SAngband (originally from OAngband)]
 *
 * Generate rooms in dungeon.  Build bigger rooms at first.
 */
/*:::ランダムダンジョン内に部屋を配置する。各部屋の出やすさを考慮してランダムに決定しroom_build()へ渡す*/
///sys 特殊vaultとか増やしたくなったらここを編集すべきだが正直触りたくない
bool generate_rooms(void)
{
	int i;
	bool remain;
	int crowded = 0;
	int total_prob;
	int prob_list[ROOM_T_MAX];
	int rooms_built = 0;
	int area_size = 100 * (cur_hgt*cur_wid) / (MAX_HGT*MAX_WID);

	int level_index = MIN(10, div_round(dun_level, 10));

	/* Number of each type of room on this level */
	s16b room_num[ROOM_T_MAX];

	/* Limit number of rooms */
	int dun_rooms_lim = DUN_ROOMS_MAX * area_size / 100;

	/* Assume normal cave */

	room_info_type *room_info_ptr = room_info_normal;

	/*
	 * Initialize probability list.
	 */
	/*:::room_info_ptrにはmin(その部屋の出現開始階層)とprob(10分割された階ごとの部屋のでやすさ)が格納されている。*/
	/*:::従って、ここでprob_list[0-15]にそれぞれの部屋の出やすさ(0なら出ない)が格納される*/
	for (i = 0; i < ROOM_T_MAX; i++)
	{
		/* No rooms allowed above their minimum depth. */
		if (dun_level < room_info_ptr[i].min_level)
		{
			prob_list[i] = 0;
		}
		else
		{
			int jyoon_card_num = count_ability_card(ABL_CARD_JYOON);

			///mod160221 ExtraモードではPITやNESTがあまり出ない 地下街出やすい
			if(EXTRA_MODE)
			{
				if(i == ROOM_T_NEST || i == ROOM_T_PIT || i == ROOM_T_TRAP_PIT)
					prob_list[i] = room_info_ptr[i].prob[level_index] / 4;
				else if(i  == ROOM_T_ARCADE)
					prob_list[i] = room_info_ptr[i].prob[level_index] += 2 + dun_level / 25; //v1.1.62 ちょっと調整
				else
					prob_list[i] = room_info_ptr[i].prob[level_index];
			}
			else
			{
				prob_list[i] = room_info_ptr[i].prob[level_index];
			}

			//v2.0.1 アビリティカード「抗いがたきジュリ扇」によるvault,pit出現率アップ
			if (jyoon_card_num)
			{
				int mult = calc_ability_card_mod_param(ABL_CARD_JYOON, jyoon_card_num);

				if (i == ROOM_T_NEST || i == ROOM_T_PIT || i == ROOM_T_LESSER_VAULT || i == ROOM_T_GREATER_VAULT || i == ROOM_T_RANDOM_VAULT
					|| i == ROOM_T_TRAP_PIT || i == ROOM_T_TRAP || i == ROOM_T_GLASS || i == ROOM_T_ARCADE )
					prob_list[i] = prob_list[i] * mult / 100;
			}

		}


		//if (cheat_room) msg_format("type%d prob:%d", i, prob_list[i]);
	}

	/*
	 * XXX -- Various dungeon types and options.
	 */
	/*:::オプションの内容やダンジョンの種類により出なくなる部屋のprob_list[]を0にしている*/
	/* Ironman sees only Greater Vaults */
	if (ironman_rooms && !((d_info[dungeon_type].flags1 & (DF1_BEGINNER | DF1_CHAMELEON | DF1_SMALLEST))))
	{
		for (i = 0; i < ROOM_T_MAX; i++)
		{
			if (i == ROOM_T_GREATER_VAULT) prob_list[i] = 1;
			else prob_list[i] = 0;
		}
	}
	///mod150907 夢の世界はvaultが出やすい
	else if (d_info[dungeon_type].flags1 & DF1_DREAM_WORLD)
	{
		prob_list[ROOM_T_LESSER_VAULT] += MAX(0,(dun_level - 30)) / 2;
		prob_list[ROOM_T_GREATER_VAULT] += MAX(0,(dun_level - 30)) / 4;
		prob_list[ROOM_T_RANDOM_VAULT] += MAX(0,(dun_level - 30)) / 6;
	}

	/* Forbidden vaults */
	else if (d_info[dungeon_type].flags1 & DF1_NO_VAULT)
	{
		prob_list[ROOM_T_LESSER_VAULT] = 0;
		prob_list[ROOM_T_GREATER_VAULT] = 0;
		prob_list[ROOM_T_RANDOM_VAULT] = 0;
	}


	/* NO_CAVE dungeon (Castle)*/
	if (d_info[dungeon_type].flags1 & DF1_NO_CAVE)
	{
		MOVE_PLIST(ROOM_T_NORMAL, ROOM_T_FRACAVE);
		MOVE_PLIST(ROOM_T_INNER_FEAT, ROOM_T_CRYPT);
		MOVE_PLIST(ROOM_T_INNER_FEAT, ROOM_T_OVAL);
	}

	/* CAVE dungeon (Orc cave etc.) */
	else if (d_info[dungeon_type].flags1 & DF1_CAVE)
	{
		MOVE_PLIST(ROOM_T_FRACAVE, ROOM_T_NORMAL);
	}

	/* No caves when a (random) cavern exists: they look bad */
	else if (dun->cavern || dun->empty_level)
	{
		prob_list[ROOM_T_FRACAVE] = 0;
	}

	/* Forbidden glass rooms */
	if (!(d_info[dungeon_type].flags1 & DF1_GLASS_ROOM))
	{
		prob_list[ROOM_T_GLASS] = 0;
	}

	if (!(d_info[dungeon_type].flags1 & DF1_ARCADE))
	{
		prob_list[ROOM_T_ARCADE] = 0;
	}

	/*
	 * Initialize number of rooms,
	 * And calcurate total probability.
	 */
	for (total_prob = 0, i = 0; i < ROOM_T_MAX; i++)
	{
		room_num[i] = 0;
		total_prob += prob_list[i];
	}

	/*
	 * Prepare the number of rooms, of all types, we should build
	 * on this level.
	 */
	/*:::先ほどまでの部屋の出やすさを考慮しそのフロアに置くそれぞれの部屋の数を決めてroom_num[]に格納*/
	for (i = dun_rooms_lim; i > 0; i--)
	{
		int room_type;
		int rand = randint0(total_prob);

		/* Get room_type randomly */

		for (room_type = 0; room_type < ROOM_T_MAX; room_type++)
		{
			if (rand < prob_list[room_type]) break;
			else rand -= prob_list[room_type];
		}

		/* Paranoia */
		if (room_type >= ROOM_T_MAX) room_type = ROOM_T_NORMAL;

		/* Increase the number of rooms of that type we should build. */
		room_num[room_type]++;

		switch (room_type)
		{
		case ROOM_T_NEST:
		case ROOM_T_PIT:
		case ROOM_T_LESSER_VAULT:
		case ROOM_T_TRAP_PIT:
		case ROOM_T_GLASS:
		case ROOM_T_ARCADE:

			/* Large room */
			i -= 2;
			break;

		case ROOM_T_GREATER_VAULT:
		case ROOM_T_RANDOM_VAULT:

			/* Largest room */
			i -= 3;
			break;
		}
	}

	//Extraモードでは専用建物が出る。ボスフロアでは出ない。
	if(EXTRA_MODE && (dun_level != 100 && dun_level != 110 && dun_level != 120 && dun_level != 127))
	{
		int ex_built_idx;
		ex_built_idx = build_type_ex(BLDG_EX_MODE_EXTRA);
		if(ex_built_idx)
		{
			rooms_built++;
			//店が出たとき地下街は生成されない
			if(ex_built_idx <= BLDG_EX_STORE_BOOK) room_num[ROOM_T_ARCADE] = 0;
		}
	}

	//v2.1.6 ニナの特技(もしくはそれを再現するアイテムカード)による地下街強制出現
	//EXTRAで特殊建物として店が出たときには同じ在庫と店主の店が離れたところに重複して出ることになるがまあ仕方ない
	if ((p_ptr->special_flags & SPF_DUNGEON_ARCADE) && (d_info[dungeon_type].flags1 & DF1_ARCADE))
	{
		if (cheat_room) msg_print(_("*地下街確定生成*", "*Dungeon town generation confirmed*"));
		room_num[ROOM_T_ARCADE] = 1;
		p_ptr->special_flags &= ~(SPF_DUNGEON_ARCADE);
	}

	/*
	 * Build each type of room one by one until we cannot build any more.
	 * [from SAngband (originally from OAngband)]
	 */
	while (TRUE)
	{
		/* Assume no remaining rooms */
		remain = FALSE;

		for (i = 0; i < ROOM_T_MAX; i++)
		{
			/* What type of room are we building now? */
			/*:::room_build_order[]は、rooms.hのROOM_T_LESSER_VAULTなどに対応する形で部屋番号が入っている*/
			int room_type = room_build_order[i];

			/* Go next if none available */
			if (!room_num[room_type]) continue;

			/* Use up one unit */
			room_num[room_type]--;

			/* Build the room. */
			/*:::部屋を配置する。NESTとPITと開門PITはフロアに合計2つ?まで。*/
			if (room_build(room_type))
			{
				/* Increase the room built count. */
				rooms_built++;

				/* Mark as there was some remaining rooms */
				remain = TRUE;

				switch (room_type)
				{
				case ROOM_T_PIT:
				case ROOM_T_NEST:
				case ROOM_T_TRAP_PIT:

					/* Avoid too many monsters */
					if (++crowded >= 2)
					{
						room_num[ROOM_T_PIT] = 0;
						room_num[ROOM_T_NEST] = 0;
						room_num[ROOM_T_TRAP_PIT] = 0;
					}
					break;

				case ROOM_T_ARCADE:

					/* Avoid double-town */
					room_num[ROOM_T_ARCADE] = 0;
					break;
				}
			}
		}

		/* End loop if no room remain */
		if (!remain) break;
	}

	if (rooms_built < 2) return FALSE;

	if (cheat_room)
	{
#ifdef JP
		msg_format("部屋数: %d", rooms_built);
#else
		msg_format("Number of Rooms: %d", rooms_built);
#endif
	}

	return TRUE;
}

//v2.1.6 渡里ニナがダンジョン内に特殊建物を生成する特技
//紫の自宅生成とは違い未クリアクエストダンジョンやボスフロアでも使用可能だが周囲に広いスペースが必要
//現時点ではニナ専用だがEXTRAアイテムカードなどで他の職業が使っても問題は起こらないはず
//特殊生成建物としてf_infoのN:233:BUILDING_EX4を使用する
//その建物の建物idxを記録するためにbuilding_ex_idx[3]を使用する
//行動順消費するときTRUEを返す
bool nina_make_special_building()
{
	int x = px, y = py;
	int i;
	int building_idx;
	int list_len,menu_line,choose_num;
	bool flag_ok;

	if (building_ex_idx[BLDG_EX_MODE_NINA])
	{
		msg_print(_("すでにこのフロアで蜃気楼の建物を作り出している。", "There's already a mirage building on this level."));
		return FALSE;
	}

	if (!dun_level || p_ptr->inside_arena)
	{
		msg_print(_("このフロアでは使えない。", "You can't use it on this level."));
		return FALSE;

	}

	//周辺地形チェック 7x7の正方形エリア
	flag_ok = TRUE;
	for (y = py-3; y <= py+3; y++)
	{
		for (x = px-3; x <= px+3; x++)
		{
			if (!in_bounds(y, x)) continue;

			//モンスターがいると不可
			if (cave[y][x].m_idx)
			{
				msg_print(_("近くにモンスターがいる！", "There are monsters nearby!"));
				flag_ok = FALSE;
			}

			//壁があると不可
			if (cave_have_flag_bold(y, x, FF_WALL))
			{
				msg_print(_("もっと開けた場所が必要だ。", "You need more open space."));
				flag_ok = FALSE;
			}

			//＠がいる場所はアイテムやトラップやルーンなども不可
			if (player_bold(y,x) && !cave_clean_bold(y, x))
			{
				msg_print(_("床上の障害物が邪魔で建物を作れない。", "Something on the floor is in the way of construction."));
				flag_ok = FALSE;
			}

			if (!flag_ok)break;
		}
		if (!flag_ok)break;
	}
	if (!flag_ok) return FALSE;


	//作れる建物をカウント
	list_len = 0;
	for (i = 0; nina_build_table[i].lev; i++)
	{
		if (p_ptr->lev >= nina_build_table[i].lev) list_len++;
		else break;
	}
	if (!list_len)
	{
		msg_print(_("まだ作れる建物がない。", "You can't make buildings yet."));//ここには来ないはず
		return FALSE;
	}

	//作れる建物をリスト表示
	screen_save();
	menu_line = 1;
	Term_erase(17, menu_line++, 255);
	Term_erase(17, menu_line, 255);
	prt(_("どんな建物の蜃気楼を作りますか？(ESC:キャンセル)", "Create a mirage of which building? (ESC: Cancel)"), menu_line++, 20);
	for (i = 0; i < list_len; i++)
	{
		Term_erase(17, menu_line, 255);
		prt(format("%c) %s", ('a' + i), nina_build_table[i].bldg_name), menu_line++, 25);
	}
	Term_erase(17, menu_line, 255);

	//入力を受け付け
	while (1)
	{
		char c = inkey();
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		choose_num = c - 'a';

		if (choose_num >= 0 && choose_num < list_len) break;

	}
	screen_load();

	building_idx = nina_build_table[choose_num].bldg_idx;

	//building_ex_idx[]への格納はinit_extra_dungeon_buildings()の前に行う必要がある
	building_ex_idx[BLDG_EX_MODE_NINA] = building_idx;

	//建物の生成
	switch (building_idx)
	{
		//普通の店の場合

	case BLDG_EX_STORE_GENERAL:
		cave_set_feat(py, px, f_tag_to_index_in_init("GENERAL_STORE"));
		store_init(TOWN_UG, STORE_GENERAL);
		break;

	case BLDG_EX_STORE_ARMOURY:
		cave_set_feat(py, px, f_tag_to_index_in_init("ARMOURY"));
		store_init(TOWN_UG, STORE_ARMOURY);
		break;

	case BLDG_EX_STORE_WEAPON:
		cave_set_feat(py, px, f_tag_to_index_in_init("WEAPON_SMITHS"));
		store_init(TOWN_UG, STORE_WEAPON);
		break;

	case BLDG_EX_STORE_TEMPLE:
		cave_set_feat(py, px, f_tag_to_index_in_init("TEMPLE"));
		store_init(TOWN_UG, STORE_TEMPLE);
		break;
	case BLDG_EX_STORE_ALCHEMIST:
		cave_set_feat(py, px, f_tag_to_index_in_init("ALCHEMY_SHOP"));
		store_init(TOWN_UG, STORE_ALCHEMIST);
		break;
	case BLDG_EX_STORE_MAGIC:
		cave_set_feat(py, px, f_tag_to_index_in_init("MAGIC_SHOP"));
		store_init(TOWN_UG, STORE_MAGIC);
		break;
	case BLDG_EX_STORE_BLACK:
		cave_set_feat(py, px, f_tag_to_index_in_init("BLACK_MARKET"));
		store_init(TOWN_UG, STORE_BLACK);
		break;
	case BLDG_EX_STORE_BOOK:
		cave_set_feat(py, px, f_tag_to_index_in_init("BOOKSTORE"));
		store_init(TOWN_UG, STORE_BOOK);
		break;
	case BLDG_EX_HOME:
		cave_set_feat(py, px, f_tag_to_index_in_init("HOME"));
		break;

	default:
		//特殊建物の場合
		cave_set_feat(py, px, f_tag_to_index_in_init("BUILDING_EX4"));
		//特殊建物にbldg[].lettersを再設定する
		init_extra_dungeon_buildings();

	}
	//フロアから出るときに消去するための専用フラグを設定
	cave[py][px].cave_xtra_flag |= CAVE_XTRA_FLAG_NINA;

	msg_print(_("周囲の景色がゆらめき、あなたの前に建物が現れた。", "The scenery around you starts to shimmer, and a building appears before you."));

	return TRUE;

}

//v2.1.5 ニナが生成した建物を消去する
//フロア移動時に呼ばれる
void nina_erase_special_building()
{

	int y, x;
	bool flag_find = FALSE;

	//特殊建物種別記録兼特殊建物生成済みフラグをクリア
	building_ex_idx[BLDG_EX_MODE_NINA] = 0;

	//EXTRAの場合このフロアにはもう来ないからフラグのクリアだけでいい
	if (EXTRA_MODE) return;

	//それ以外なら建物自体を消す必要があるので消すべき建物を探す
	for (y = 1; y < cur_hgt -1 ; y++)
	{
		for (x = 1; x < cur_wid -1 ; x++)
		{
			//ニナの特技で生成した建物につく専用フラグ
			if (cave[y][x].cave_xtra_flag & CAVE_XTRA_FLAG_NINA)
			{
					flag_find = TRUE;
			}
			if (flag_find)break;
		}
		if (flag_find)break;
	}

	if (!flag_find)
	{
		msg_print(_("ERROR:nina_erase_special_building()で建物の検出に失敗", "ERROR: Failed to find a building in nina_erase_special_building()"));
		return;
	}

	//建物を床に変える
	cave_set_feat(y, x, feat_floor);
	//グリッドの特殊フラグをクリア
	cave[y][x].cave_xtra_flag &= ~(CAVE_XTRA_FLAG_NINA);

	msg_print(_("蜃気楼の建物がフロアから消えたようだ。", "The mirage building vanishes from the level."));

}




