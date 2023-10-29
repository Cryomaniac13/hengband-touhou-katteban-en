/* File: cmd2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 2) */

#include "angband.h"


/*:::クエストダンジョンからクエスト未完了で出る場合に確認する処理*/
bool confirm_leave_level(bool down_stair)
{
	quest_type *q_ptr = &quest[p_ptr->inside_quest];

	//v1.1.24 出口到達クエスト用の棄権処理追加
	/* Confirm leaving from once only quest */
	if (confirm_quest && p_ptr->inside_quest &&(
		  q_ptr->type == QUEST_TYPE_RANDOM ||
	     (q_ptr->flags & QUEST_FLAG_ONCE &&	(q_ptr->status != QUEST_STATUS_COMPLETED)) ||
		 (q_ptr->flags & QUEST_FLAG_TOWER && ((q_ptr->status != QUEST_STATUS_STAGE_COMPLETED) ||
			 (down_stair && (quest[QUEST_TOWER1].status != QUEST_STATUS_COMPLETED)))) ||
		  q_ptr->type == QUEST_TYPE_FIND_EXIT

						 ))
	{
#ifdef JP
		msg_print("この階を一度去ると二度と戻って来られません。");
		if (get_check("本当にこの階を去りますか？")) return TRUE;
#else
		msg_print("You can't come back here once you leave this floor.");
		if (get_check("Really leave this floor? ")) return TRUE;
#endif
	}
	else
	{
		///mod140113 クエストフラグにQUEST_RETRYを付加する。
		/*:::これにより再挑戦可能なクエストに二度目以降に入ったときアイテムが落ちていないようにする*/
		if( p_ptr->inside_quest ) q_ptr->flags |= QUEST_FLAG_RETRY;
		if((q_ptr->status != QUEST_STATUS_STAGE_COMPLETED) && p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -1);
		return TRUE;
	}
	return FALSE;
}


/*
 * Go up one level
 */
/*:::ダンジョン内で階段を上がる　荒野は別箇所で処理されている*/
void do_cmd_go_up(void)
{
	bool go_up = FALSE;

	/* Player grid */
	cave_type *c_ptr = &cave[py][px];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	int up_num = 0;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Verify stairs */
	if (!have_flag(f_ptr->flags, FF_LESS))
	{
#ifdef JP
		msg_print("ここには上り階段が見当たらない。");
#else
		msg_print("I see no up staircase here.");
#endif

		return;
	}

	/* Quest up stairs */
	/*:::クエストフラグをもつ階段の場合?*/
	if (have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Cancel the command */
		if (!confirm_leave_level(FALSE)) return;


		/* Success */
#ifdef JP
		///del131213 msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("なんだこの階段は！");
		//else
			msg_print("上の階に登った。");
#else
		msg_print("You enter the up staircase.");
#endif

		leave_quest_check();
		/*:::塔のための処理？*/
		p_ptr->inside_quest = c_ptr->special;

		/* Activate the quest */
		/*:::特定クエストは新たな階を生成？塔専用か？*/
		if (!quest[p_ptr->inside_quest].status)
		{
			if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
			{
				init_flags = INIT_ASSIGN;
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			}
			quest[p_ptr->inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!p_ptr->inside_quest)
		{
			dun_level = 0;
		}

		/* Leaving */
		p_ptr->leaving = TRUE;

		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;

		/* Hack -- take a turn */
		energy_use = 100;

		/* End the command */
		return;
	}

	if (!dun_level)
	{
		go_up = TRUE;
	}
	else
	{
		go_up = confirm_leave_level(FALSE);
	}

	/* Cancel the command */
	if (!go_up) return;

	/* Hack -- take a turn */
	energy_use = 100;
	/*:::オプションによりオートセーブ*/
	if (autosave_l) do_cmd_save_game(TRUE);

	/* For a random quest */
	if (p_ptr->inside_quest &&
	    quest[p_ptr->inside_quest].type == QUEST_TYPE_RANDOM)
	{
		leave_quest_check();

		p_ptr->inside_quest = 0;
	}

	/* For a fixed quest */
	/*:::ランクエ以外*/
	if (p_ptr->inside_quest &&
	    quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
	{

		leave_quest_check();

		p_ptr->inside_quest = c_ptr->special;
		dun_level = 0;
		up_num = 0;
	}

	/* For normal dungeon and random quest */
	else
	{
		/* New depth */
		if (have_flag(f_ptr->flags, FF_SHAFT))
		{
			/* Create a way back */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_SHAFT);

			up_num = 2;
		}
		else
		{
			/* Create a way back */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP);

			up_num = 1;
		}

		/* Get out from current dungeon */
		if (dun_level - up_num < d_info[dungeon_type].mindepth)
			up_num = dun_level;
	}

#ifdef JP
	if (record_stair) do_cmd_write_nikki(NIKKI_STAIR, 0-up_num, "階段を上った");
#else
	if (record_stair) do_cmd_write_nikki(NIKKI_STAIR, 0-up_num, "climbed up the stairs to");
#endif

	/* Success */
#ifdef JP
	///del131213 msg
	//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
	//	msg_print("なんだこの階段は！");
	//else if (up_num == dun_level)
	if (up_num == dun_level)
		msg_print("地上に戻った。");
	else
		msg_print("階段を上って新たなる迷宮へと足を踏み入れた。");
#else
	if (up_num == dun_level)
		msg_print("You go back to the surface.");
	else
		msg_print("You enter a maze of up staircases.");
#endif

	/* Leaving */
	p_ptr->leaving = TRUE;
}



/*
* Return the number of features around (or under) the character.
* Usually look for doors and floor traps.
*/
/*:::＠の周辺の特定の物の数を数える。数えるものは関数ポインタで処理を渡す*/
static int count_dt(int *y, int *x, bool(*test)(int feat), bool under)
{
	int d, count, xx, yy;

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < 9; d++)
	{
		cave_type *c_ptr;
		s16b feat;

		/* if not searching under player continue */
		if ((d == 8) && !under) continue;

		/* Extract adjacent (legal) location */
		yy = py + ddy_ddd[d];
		xx = px + ddx_ddd[d];

		/* Get the cave */
		c_ptr = &cave[yy][xx];

		/* Must have knowledge */
		if (!(c_ptr->info & (CAVE_MARK))) continue;

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Not looking for this feature */
		if (!((*test)(feat))) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


/*
* Determine if a grid contains a chest
*/
static s16b chest_check(int y, int x, bool trapped)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;


	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Skip unknown chests XXX XXX */
		/* if (!(o_ptr->marked & OM_FOUND)) continue; */

		/* Check for non empty chest */
		if ((o_ptr->tval == TV_CHEST) &&
			(((!trapped) && (o_ptr->pval)) || /* non empty */
			((trapped) && (o_ptr->pval > 0)))) /* trapped only */
		{
			return (this_o_idx);
		}
	}

	/* No chest */
	return (0);
}

/*
* Return the number of chests around (or under) the character.
* If requested, count only trapped chests.
*/
/*:::＠周辺の箱のあるグリッド数を数える*/
/*:::trapped:トラップのある箱のみを数える*/
static int count_chests(int *y, int *x, bool trapped)
{
	int d, count, o_idx;

	object_type *o_ptr;

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < 9; d++)
	{
		/* Extract adjacent (legal) location */
		int yy = py + ddy_ddd[d];
		int xx = px + ddx_ddd[d];

		/* No (visible) chest is there */
		if ((o_idx = chest_check(yy, xx, FALSE)) == 0) continue;

		/* Grab the object */
		o_ptr = &o_list[o_idx];

		/* Already open */
		if (o_ptr->pval == 0) continue;

		/* No (known) traps here */
		if (trapped && (!object_is_known(o_ptr) ||
			!chest_traps[o_ptr->pval])) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


//v1.1.97 わざと罠を踏む cmd_go_down()からのみ使用
static void step_on_trap(void)
{
	int dir,x,y;
	s16b feat;
	cave_type *c_ptr;

	if (!get_check_strict(_("近くの罠をわざと踏みますか？", "Are you sure you want to deliberately step on a nearby trap?"), CHECK_DEFAULT_Y)) return;
	if (!get_rep_dir(&dir, TRUE)) return;

	/* Get requested location */
	y = py + ddy[dir];
	x = px + ddx[dir];

	/* Get requested grid */
	c_ptr = &cave[y][x];

	if (!is_known_trap(c_ptr))
	{
		msg_print(_("そこには罠が見当たらない。", "You don't see any traps there."));
		return;
	}

	//move_player()に新しいフラグを実装しわざとトラップ発動させる
	move_player(dir, FALSE, FALSE,TRUE);

	//直接実行する場合＠はトラップの場所へ動かないので落とし戸とかに落ちられない。イメージ的にも移動すべきか。
	//activate_floor_trap(y, x, FALSE);

}

/*
 * Go down one level
 */
/*:::階段を下りる*/
//v1.1.97 罠をわざと踏む機能を追加
void do_cmd_go_down(void)
{
	/* Player grid */
	cave_type *c_ptr = &cave[py][px];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	int trap_count = 0;

	bool fall_trap = FALSE;
	int down_num = 0;

	int ty, tx;

	//足元もしくは隣接した感知済みの罠を数える
	trap_count = count_dt(&ty, &tx, is_trap, TRUE);


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Verify stairs */
	//v1.1.97 落とし戸はstep_on_trap()で処理するのでここで階段でなくトラップと見なすようにする
	if (!(have_flag(f_ptr->flags, FF_MORE) && !have_flag(f_ptr->flags, FF_TRAP)))
	{
#ifdef JP
		msg_print("ここには下り階段が見当たらない。");
#else
		msg_print("I see no down staircase here.");
#endif
		//v1.1.97 わざと罠を踏む
		if (trap_count) step_on_trap();

		return;

	}

	//v1.1.97 もうこのフラグは立たないはずだが消すのも面倒なのでそのまま
	if (have_flag(f_ptr->flags, FF_TRAP)) fall_trap = TRUE;

	/* Quest entrance */
	if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		do_cmd_quest();
	}
	/*::: Mega Hack - Extraモードでオベロン打倒後に階段を降りるとサーペントクエストを受領し混沌の領域101階へ*/
	else if(EXTRA_MODE && dun_level == 100 && quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED)
	{
		int tmp = p_ptr->inside_quest;

		p_ptr->inside_quest = QUEST_SERPENT;
		init_flags |= INIT_ASSIGN;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);
		quest[QUEST_SERPENT].status = QUEST_STATUS_TAKEN;
		p_ptr->inside_quest = tmp;

		p_ptr->oldpx = px;
		p_ptr->oldpy = py;
		dungeon_type = DUNGEON_CHAOS;

		p_ptr->leaving = TRUE;
		prepare_change_floor_mode(CFM_FIRST_FLOOR | CFM_DOWN);
		if (autosave_l) do_cmd_save_game(TRUE);
		energy_use = 100;

		msg_print(_("あなたはダンジョンの更なる深層へと降りていった・・",
                    "You descend even further into the dungeon..."));
	}

	/* Quest down stairs */
	else if (have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Confirm Leaving */
		if(!confirm_leave_level(TRUE)) return;

#ifdef JP
		///del131213 msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("なんだこの階段は！");
		//else
			msg_print("下の階に降りた。");
#else
			msg_print("You enter the down staircase.");
#endif

		leave_quest_check();
		leave_tower_check();

		p_ptr->inside_quest = c_ptr->special;

		/* Activate the quest */
		/*:::塔専用処理？*/
		if (!quest[p_ptr->inside_quest].status)
		{
			if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
			{
				init_flags = INIT_ASSIGN;
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			}
			quest[p_ptr->inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!p_ptr->inside_quest)
		{
			dun_level = 0;
		}

		/* Leaving */
		p_ptr->leaving = TRUE;

		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;


        /* Hack -- take a turn */
        energy_use = 100;
	}

	else
	{
		int target_dungeon = 0;
		if (!dun_level)
		{
			target_dungeon = have_flag(f_ptr->flags, FF_ENTRANCE) ? c_ptr->special : DUNGEON_ANGBAND;

			/*:::帰還なしオプションでは鉄獄以外のダンジョンの階段へ入れない*/
			if (ironman_downward && (target_dungeon != DUNGEON_ANGBAND))
			{
#ifdef JP
				msg_print("ダンジョンの入口は塞がれている！");
#else
				msg_print("The entrance of this dungeon is closed!");
#endif
				return;
			}
			/*:::初めて入るときはmax_dlv[]が0らしい。確認をする*/
			if (!max_dlv[target_dungeon])
			{
#ifdef JP
				msg_format("ここには%sの入り口(%d階相当)があります", d_name+d_info[target_dungeon].name, d_info[target_dungeon].mindepth);
				if (!get_check("本当にこのダンジョンに入りますか？")) return;
#else
				msg_format("There is the entrance of %s (Danger level: %d)", d_name+d_info[target_dungeon].name, d_info[target_dungeon].mindepth);
				if (!get_check("Do you really get in this dungeon? ")) return;
#endif
			}

			/* Save old player position */
			p_ptr->oldpx = px;
			p_ptr->oldpy = py;
			dungeon_type = (byte)target_dungeon;

			/*
			 * Clear all saved floors
			 * and create a first saved floor
			 */
			prepare_change_floor_mode(CFM_FIRST_FLOOR);
		}
		//エクストラクエスト未完了
		if(INSIDE_EXTRA_QUEST)
		{
			if(!confirm_leave_level(TRUE)) return;
			leave_quest_check();
			p_ptr->inside_quest = 0;
		}

		/* Hack -- take a turn */
		energy_use = 100;

		if (autosave_l) do_cmd_save_game(TRUE);

		/* Go down */
		if (have_flag(f_ptr->flags, FF_SHAFT) && !EXTRA_MODE) down_num += 2;
		else down_num += 1;

		if (!dun_level)
		{
			/* Enter the dungeon just now */
			p_ptr->enter_dungeon = TRUE;
			down_num = d_info[dungeon_type].mindepth;
		}

		if (record_stair)
		{
#ifdef JP
			if (fall_trap) do_cmd_write_nikki(NIKKI_STAIR, down_num, "落とし戸に落ちた");
			else do_cmd_write_nikki(NIKKI_STAIR, down_num, "階段を下りた");
#else
			if (fall_trap) do_cmd_write_nikki(NIKKI_STAIR, down_num, "fell through a trap door");
			else do_cmd_write_nikki(NIKKI_STAIR, down_num, "climbed down the stairs to");
#endif
		}

		if (fall_trap)
		{
#ifdef JP
			msg_print("わざと落とし戸に落ちた。");
#else
			msg_print("You deliberately jump through the trap door.");
#endif
		}
		else
		{
			/* Success */
			if (target_dungeon)
			{
#ifdef JP
				msg_format("%sへ入った。", d_text + d_info[dungeon_type].text);
#else
				msg_format("You entered %s.", d_text + d_info[dungeon_type].text);
#endif
			}
			else
			{
#ifdef JP
				///del131213 msg
				//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				//	msg_print("なんだこの階段は！");
				//else
				msg_print("階段を下りて新たなる迷宮へと足を踏み入れた。");
#else
				msg_print("You enter a maze of down staircases.");
#endif
			}
		}


		/* Leaving */
		p_ptr->leaving = TRUE;

		if (fall_trap)
			/*:::落とし戸に落ちたときはランダムな場所に＠が生成されるオプション*/
		{
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
		}
		else if(EXTRA_MODE)
		{
			prepare_change_floor_mode(CFM_DOWN | CFM_NO_RETURN);
		}
		else
		{
			if (have_flag(f_ptr->flags, FF_SHAFT))
			{
				/* Create a way back */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_SHAFT);
			}
			else
			{
				/* Create a way back */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN);
			}
		}
	}
}



/*
 * Simple command to "search" for one turn
 */
/*:::探索*/
void do_cmd_search(void)
{
	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	/* Search */
	search();
}



/*
 * Allocates objects upon opening a chest    -BEN-
 *
 * Disperse treasures from the given chest, centered at (x,y).
 *
 * Small chests often contain "gold", while Large chests always contain
 * items.  Wooden chests contain 2 items, Iron chests contain 4 items,
 * and Steel chests contain 6 items.  The "value" of the items in a
 * chest is based on the "power" of the chest, which is in turn based
 * on the level on which the chest is generated.
 */
/*:::箱が開きアイテムが散らばる。scatterがTRUEだとダンジョン中に散らばる*/
/*:::開けると特定アイテムが出たり特殊なことが起こる箱を追加した*/
static void chest_death(bool scatter, int y, int x, s16b o_idx)
{
	int number;

	int k_quantity = 0; //k_idxを指定する箱で個数を指定するとき

	bool small;
	u32b mode = AM_GOOD;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr = &o_list[o_idx];

	if (o_ptr->tval != TV_CHEST)
	{
		msg_format(_("ERROR:tvalがTV_CHEST以外(%d)のときchest_death()が呼ばれた",
                    "ERROR: chest_death() called with tval other than TV_CHEST (%d)"),o_ptr->tval);
		return;
	}

	/*:::出るアイテムの質と個数を決定*/
	/* Small chests often hold "gold" */
	small = (o_ptr->sval < SV_CHEST_MIN_LARGE);

	/* Determine how much to drop (see above) */
	number = (o_ptr->sval % SV_CHEST_MIN_LARGE) * 2;
	/*:::カンヅメの生成レベルは特殊*/
	if (o_ptr->sval == SV_CHEST_KANDUME)
	{
		number = 5;
		small = FALSE;
		mode |= AM_GREAT;
		object_level = o_ptr->xtra3;
	}
	else
	{
		/* Determine the "value" of the items */
		//object_level = ABS(o_ptr->pval) + 10;
		//v1.1.97 箱の中身のレベルをモンスターと同じく生成フロア+5にした
		object_level = MIN(127,o_ptr->xtra3);
	}

	if (cheat_peek) msg_format("object_level:%d",object_level);

	/* Zero pval means empty chest */
	/*:::空いた箱か爆発した箱*/
	if (!o_ptr->pval) number = 0;

	/* Opening a chest */
	/*:::make_object()のときに箱が生成されないためのグローバルフラグ*/
	opening_chest = TRUE;

	/*:::特定の設備やクエストで手に入る箱は中身が固定されている*/
	///mod140117 中身指定箱ルーチンを追加
	if(o_ptr->sval >= SV_CHEST_MARIPO_01 && o_ptr->sval != SV_CHEST_KANDUME)
	{
		int a_idx = 0;
		int k_idx = 0;

		///mod140323 クローン地獄報酬　靴を履けない＠にはシヴァジャケ
		///mod141118 キスメ特殊報酬　シヴァ桶
		//人魚メイドにはシヴァ手袋
		if(o_ptr->sval == SV_CHEST_CLONE)
		{
			if (p_ptr->pclass == CLASS_KISUME)
			{
				object_level = 60;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_ARMOR, SV_ARMOR_TUB));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				q_ptr->pval = 4;
				add_flag(q_ptr->art_flags, TR_SPEED);
				add_flag(q_ptr->art_flags, TR_FREE_ACT);
				if(q_ptr->to_a < 16) q_ptr->to_a = 16;
				q_ptr->art_name = quark_add(_("シヴァの化身の", "of Shiva's Avatar"));
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if (p_ptr->prace == RACE_NINGYO && p_ptr->pclass == CLASS_MAID)
			{
				object_level = 60;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_GLOVES, SV_HAND_DRESSGLOVES));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				q_ptr->pval = 4;
				add_flag(q_ptr->art_flags, TR_SPEED);
				add_flag(q_ptr->art_flags, TR_FREE_ACT);
				q_ptr->art_name = quark_add(_("シヴァの化身の", "of Shiva's Avatar"));
				(void)drop_near(q_ptr, -1, y, x);


			}
			else if (p_ptr->prace == RACE_NINGYO || p_ptr->pclass == CLASS_TOZIKO)
			{
				a_idx = ART_SHIVA_JACKET;
			}
			else
			{
				a_idx = ART_SHIVA_SHOES;
			}
		}


		/*:::古い城報酬　職業ごとに指定する*/
		else if(o_ptr->sval == SV_CHEST_OLDCASTLE)
		{
			if(p_ptr->pclass == CLASS_WARRIOR)
			{
				//小人戦士は特殊処理で小さき棘とすばやき棘がセット
				if(p_ptr->prace == RACE_KOBITO)
				{
					bool ok = FALSE;
					artifact_type *a_ptr;
					a_ptr = &a_info[ART_TINYTHORN];
					if (!a_ptr->cur_num)
					{
						ok = TRUE;
						if (create_named_art(ART_TINYTHORN, y, x))
						{
							a_ptr->cur_num = 1;

							if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
						}
						else if (!preserve_mode) a_ptr->cur_num = 1;
					}
					a_ptr = &a_info[ART_QUICKTHORN];
					if (!a_ptr->cur_num)
					{
						ok = TRUE;
						if (create_named_art(ART_QUICKTHORN, y, x))
						{
							a_ptr->cur_num = 1;

							if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
						}
						else if (!preserve_mode) a_ptr->cur_num = 1;
					}
					if(!ok) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//☆生成
				}
				else
					a_idx = ART_GLAIVE_PAIN;
			}
			else if(p_ptr->pclass == CLASS_CAVALRY)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_GUNGNIR;
				else
					a_idx = ART_GLAIVE_PAIN;
			}

			else if(p_ptr->pclass == CLASS_ROGUE) a_idx = ART_ALHAZRED;
			else if(p_ptr->pclass == CLASS_MAGE || p_ptr->pclass == CLASS_HIGH_MAGE)
			{
				if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU)
					a_idx = ART_ODUNU;
				else
					a_idx = ART_INDRA;
			}
			else if(p_ptr->pclass == CLASS_RANGER) a_idx = ART_BARD_BOW;
			else if(p_ptr->pclass == CLASS_MAID && p_ptr->psex == SEX_MALE) a_idx = ART_WALTER;
			else if(p_ptr->pclass == CLASS_MAID && p_ptr->psex == SEX_FEMALE) a_idx = ART_ROBERTA;
			else if(p_ptr->pclass == CLASS_ARCHER) a_idx = ART_YORIMASA;
			else if(p_ptr->pclass == CLASS_CIVILIAN)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_POWER;
				else
					a_idx = ART_CHAINSWORD;
			}

			else if(p_ptr->pclass == CLASS_TEACHER) a_idx = ART_JACK;
			else if(p_ptr->pclass == CLASS_PRIEST) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_OUTSIDER) a_idx = ART_AMENONUBOKO;
			//雷神の鉞の高級生成
			else if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)
			{
				q_ptr = &forge;
				if(p_ptr->prace == RACE_KOBITO) object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_HAYABUSA));
				else object_prep(q_ptr, lookup_kind(TV_AXE, SV_WEAPON_MASAKARI));
				apply_magic(q_ptr, 60, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
				if(p_ptr->prace == RACE_KOBITO) q_ptr->pval = 3;
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if(p_ptr->pclass == CLASS_SYUGEN) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_MAGIC_KNIGHT) a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_LIBRARIAN) a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_ENGINEER)
			{
				if(p_ptr->prace == RACE_KOBITO || gun_free_world)
					a_idx = ART_MARIO;
				else
					 a_idx = ART_CRIMSON;
			}
			else if(p_ptr->pclass == CLASS_SAMURAI) a_idx = ART_AGLARANG;
			else if(p_ptr->pclass == CLASS_MINDCRAFTER) a_idx = ART_ELMI;
			else if(p_ptr->pclass == CLASS_PALADIN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_SH_DEALER)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else if(p_ptr->pclass == CLASS_JEWELER) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_NINJA) a_idx = ART_NIGHT;

			else if (p_ptr->pclass == CLASS_KASEN)
			{
				if(is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
					a_idx = ART_ECSEDI;
				else
					a_idx = ART_METALHAND;
			}
			else if(p_ptr->pclass == CLASS_SUIKA) a_idx = ART_SHUTEN_DOJI;
			else if(p_ptr->pclass == CLASS_TEWI) a_idx = ART_DAIKOKU;
			else if(p_ptr->pclass == CLASS_KOGASA) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_IKU) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_KOMACHI) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_KOISHI) a_idx = ART_CHAINSWORD;
			else if (p_ptr->pclass == CLASS_UDONGE)
			{
				if(is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
					a_idx = ART_CRIMSON;
				else
					a_idx = ART_BLOODYMOON;
			}
			else if(p_ptr->pclass == CLASS_MOMIZI) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_YOUMU) a_idx = ART_ZANTETSU;
			else if(p_ptr->pclass == CLASS_SEIGA) a_idx = ART_HEAVENLY_MAIDEN;
			else if(p_ptr->pclass == CLASS_CIRNO) a_idx = ART_EXCALIBUR;
			else if(p_ptr->pclass == CLASS_ORIN) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU) a_idx = ART_AMENOKAGAMI;
			else if(p_ptr->pclass == CLASS_NAZRIN) a_idx = ART_KUKAI;
			else if(p_ptr->pclass == CLASS_LETTY) a_idx = ART_GRAYSWANDIR;
			else if(p_ptr->pclass == CLASS_RUMIA) a_idx = ART_GRAYSWANDIR;

			else if(p_ptr->pclass == CLASS_YOSHIKA) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_PATCHOULI) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_AYA) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_BANKI) a_idx = ART_DEATH_CSYTHE;
			else if(p_ptr->pclass == CLASS_MYSTIA) a_idx = ART_CHICKEN_KNIFE;
			else if(p_ptr->pclass == CLASS_FLAN) a_idx = ART_TWILIGHT;
			else if(p_ptr->pclass == CLASS_SHOU) a_idx = ART_BISYAMON;
			else if(p_ptr->pclass == CLASS_MEDICINE)  a_idx = ART_METALHAND;
			else if(p_ptr->pclass == CLASS_YUYUKO)  a_idx = ART_SAIGYOU;
			else if(p_ptr->pclass == CLASS_SATORI)  a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_KYOUKO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_TOZIKO) a_idx = ART_YORIMASA;
			else if(p_ptr->pclass == CLASS_LILY_WHITE) a_idx = ART_HURIN;
			else if(p_ptr->pclass == CLASS_KISUME) a_idx = ART_MJOLLNIR;
			else if(p_ptr->pclass == CLASS_HATATE) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_WAKASAGI) a_idx = ART_TUOR;
			else if(p_ptr->pclass == CLASS_MIKO) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_KOKORO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_WRIGGLE) a_idx = ART_NAUSICAA;
			else if(p_ptr->pclass == CLASS_YUUKA)  a_idx = ART_NARYA;
			else if(p_ptr->pclass == CLASS_CHEN) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_MURASA) a_idx = ART_SARASVATI;
			else if(p_ptr->pclass == CLASS_KEINE) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_YUGI) a_idx = ART_CAMBELEG;
			else if(p_ptr->pclass == CLASS_REIMU) a_idx = ART_IDUNOME;
			else if(p_ptr->pclass == CLASS_KAGEROU) a_idx = ART_LUTHIEN;
			else if(p_ptr->pclass == CLASS_SHIZUHA) a_idx = ART_COLLUIN;
			else if(p_ptr->pclass == CLASS_SANAE) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_MINORIKO) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_REMY) a_idx = ART_GUNGNIR;
			else if(p_ptr->pclass == CLASS_NITORI) a_idx = ART_HIRAISHI;
			else if(p_ptr->pclass == CLASS_BYAKUREN) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_NUE) a_idx = ART_TRAPEZOHEDRON;
			else if(p_ptr->pclass == CLASS_SEIJA) a_idx = ART_TUOR;
			else if(p_ptr->pclass == CLASS_UTSUHO) a_idx = ART_GIL_GALAD;
			else if(p_ptr->pclass == CLASS_YAMAME) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			{
				if(p_ptr->prace == RACE_NINGYO) a_idx = ART_NENYA;
				else	a_idx = ART_SHIVA_JACKET;
			}
			else if(p_ptr->pclass == CLASS_MAGIC_EATER) a_idx = ART_LAPUTA;
			else if(p_ptr->pclass == CLASS_SUWAKO) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_RAN) a_idx = ART_GOKASHITIKIN;
			else if(p_ptr->pclass == CLASS_EIKI) a_idx = ART_JUDGE;
			else if(p_ptr->pclass == CLASS_TENSHI) a_idx = ART_THORIN;
			else if(p_ptr->pclass == CLASS_MEIRIN) a_idx = ART_CHINGRONG;
			else if(p_ptr->pclass == CLASS_PARSEE) a_idx = ART_TERROR;
			else if(p_ptr->pclass == CLASS_SHINMYOU_2) a_idx = ART_CHARIOT;
			else if(p_ptr->pclass == CLASS_SUMIREKO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_ICHIRIN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_MOKOU) a_idx = ART_GURENKI;
			else if(p_ptr->pclass == CLASS_KANAKO) a_idx = ART_MAGATAMA;
			else if(p_ptr->pclass == CLASS_FUTO) a_idx = ART_SEIMEI;
			else if(p_ptr->pclass == CLASS_SUNNY) a_idx = ART_WEREWINDLE;
			else if(p_ptr->pclass == CLASS_LUNAR) a_idx = ART_ZYOGA;
			else if(p_ptr->pclass == CLASS_STAR) a_idx = ART_TRAPEZOHEDRON;
			else if(p_ptr->pclass == CLASS_ALICE) a_idx = ART_QUEEN_OF_RED;
			else if(p_ptr->pclass == CLASS_MARISA) a_idx = ART_CADUCEUS;
			else if(p_ptr->pclass == CLASS_LUNASA || p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				a_idx = ART_HARMONIA;
			else if(p_ptr->pclass == CLASS_CLOWNPIECE || p_ptr->pclass == CLASS_VFS_CLOWNPIECE) a_idx = ART_BLOODYMOON;
			else if(p_ptr->pclass == CLASS_DOREMY) a_idx = ART_METALHAND;
			else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI) a_idx = ART_SARASVATI;
			else if (p_ptr->pclass == CLASS_SAKUYA) //咲夜　ランダムアーティファクトメイド服　元素耐性と[魔瞬削除保証
			{
				object_level = 75;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_MAID));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				add_flag(q_ptr->art_flags, TR_RES_ACID);
				add_flag(q_ptr->art_flags, TR_RES_ELEC);
				add_flag(q_ptr->art_flags, TR_RES_FIRE);
				add_flag(q_ptr->art_flags, TR_RES_COLD);
				remove_flag(q_ptr->art_flags, TR_NO_TELE);
				remove_flag(q_ptr->art_flags, TR_NO_MAGIC);
				q_ptr->to_a = 18 + randint0(6);

				(void)drop_near(q_ptr, -1, y, x);
			}
			else if(p_ptr->pclass == CLASS_HINA)
				a_idx = ART_HARMONIA;
			else if(p_ptr->pclass == CLASS_3_FAIRIES) a_idx = ART_THE_DAWN;
			else if(p_ptr->pclass == CLASS_RAIKO) a_idx = ART_INDRA;
			else if(p_ptr->pclass == CLASS_CHEMIST) a_idx = ART_AESCULAPIUS;
			else if(p_ptr->pclass == CLASS_MAMIZOU) a_idx = ART_GOGO;
			else if(p_ptr->pclass == CLASS_YUKARI) a_idx = ART_ZYOGA;
			else if(p_ptr->pclass == CLASS_RINGO) a_idx = ART_CRIMSON;
			else if(p_ptr->pclass == CLASS_SEIRAN) a_idx = ART_MASKET;
			else if(p_ptr->pclass == CLASS_EIRIN) a_idx = ART_AMANOMAKAKO;
			else if(p_ptr->pclass == CLASS_KAGUYA) k_idx = lookup_kind(TV_MATERIAL,SV_MATERIAL_MYSTERIUM);

			else if(p_ptr->pclass == CLASS_SAGUME) a_idx = ART_QUEEN_OF_RED;
			else if(p_ptr->pclass == CLASS_REISEN2) a_idx = ART_MASKET;
			else if(p_ptr->pclass == CLASS_TOYOHIME) a_idx = ART_SHIOMITSUTAMA;
			else if(p_ptr->pclass == CLASS_YORIHIME) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_HECATIA) a_idx = ART_CADUCEUS;
			else if(p_ptr->pclass == CLASS_JUNKO) a_idx = ART_KRONOS;
			else if (p_ptr->pclass == CLASS_SOLDIER)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_BILLY_THE_KID;
				else
					a_idx = ART_MASKET;
			}
			else if(p_ptr->pclass == CLASS_NEMUNO) a_idx = ART_KINTAROU;
			else if(p_ptr->pclass == CLASS_AUNN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_NARUMI) a_idx = ART_KUKAI_KASA;
			else if(p_ptr->pclass == CLASS_KOSUZU) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_LARVA) a_idx = ART_AMBER;
			else if(p_ptr->pclass == CLASS_MAI) a_idx = ART_RED_SHOES;
			else if (p_ptr->pclass == CLASS_SATONO) a_idx = ART_ASTARTE;
			else if(p_ptr->pclass == CLASS_JYOON) a_idx = ART_ECSEDI;
			else if (p_ptr->pclass == CLASS_SHION) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_OKINA) a_idx = ART_TRAPEZOHEDRON;
			else if (p_ptr->pclass == CLASS_EIKA) a_idx = ART_SARASVATI;
			else if (p_ptr->pclass == CLASS_MAYUMI) a_idx = ART_AGLARANG;
			else if (p_ptr->pclass == CLASS_KUTAKA) a_idx = ART_CHICKEN_KNIFE;
			else if (p_ptr->pclass == CLASS_URUMI) a_idx = ART_TUOR;
			else if (p_ptr->pclass == CLASS_SAKI) a_idx = ART_FEANOR_BOOTS;
			else if (p_ptr->pclass == CLASS_YACHIE) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_KEIKI)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else if (p_ptr->pclass == CLASS_RESEARCHER) a_idx = ART_PALANTIR;
			else if (p_ptr->pclass == CLASS_CARD_DEALER) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_TAKANE) a_idx = ART_THRANDUIL;
			else if (p_ptr->pclass == CLASS_MOMOYO) a_idx = ART_NAIN_PICK;
			else if (p_ptr->pclass == CLASS_SANNYO) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_MIKE) a_idx = ART_DAIKOKU;
			else if (p_ptr->pclass == CLASS_TSUKASA) a_idx = ART_EIBON;
			else if (p_ptr->pclass == CLASS_MEGUMU) a_idx = ART_ODUNU;
			else if (p_ptr->pclass == CLASS_MISUMARU) a_idx = ART_PALANTIR;
			else if (p_ptr->pclass == CLASS_YUMA) a_idx = ART_CHIYOU;
			else if (p_ptr->pclass == CLASS_CHIMATA) a_idx = ART_CADUCEUS;
			else if (p_ptr->pclass == CLASS_MIYOI) a_idx = ART_TUOR;
			else if (p_ptr->pclass == CLASS_BITEN) a_idx = ART_NYOIBOU;
			else if (p_ptr->pclass == CLASS_ENOKO) a_idx = ART_PALANTIR;


			else k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//☆生成
		}
		else if(o_ptr->sval == SV_CHEST_MARIPO_01) k_idx = 549;
		else if(o_ptr->sval == SV_CHEST_MARIPO_02) k_idx = 550;
		else if(o_ptr->sval == SV_CHEST_MARIPO_03) k_idx = 548;
		else if(o_ptr->sval == SV_CHEST_MARIPO_04) k_idx = 400;
		else if(o_ptr->sval == SV_CHEST_MARIPO_05) k_idx = 397;
		else if(o_ptr->sval == SV_CHEST_MARIPO_06) k_idx = 578;
		else if(o_ptr->sval == SV_CHEST_MARIPO_07) k_idx = 556;
		else if(o_ptr->sval == SV_CHEST_MARIPO_08) k_idx = 574;

		else if(o_ptr->sval == SV_CHEST_MARIPO_09) k_idx = 347;
		else if(o_ptr->sval == SV_CHEST_MARIPO_10) k_idx = 265;
		else if(o_ptr->sval == SV_CHEST_MARIPO_11) k_idx = 422;
		else if(o_ptr->sval == SV_CHEST_MARIPO_12) k_idx = 266;
		else if(o_ptr->sval == SV_CHEST_MARIPO_13) k_idx = 428;
		else if(o_ptr->sval == SV_CHEST_MARIPO_14) k_idx = 354;
		else if(o_ptr->sval == SV_CHEST_MARIPO_15) k_idx = 601;
		else if(o_ptr->sval == SV_CHEST_MARIPO_16) k_idx = 580;

		else if(o_ptr->sval == SV_CHEST_MARIPO_17) k_idx = 356;
		//サイバーデーモンの人形
		else if(o_ptr->sval == SV_CHEST_MARIPO_18)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 391);
			q_ptr->pval = MON_CYBER;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//妖刀(75階相当高級生成)
		else if(o_ptr->sval == SV_CHEST_MARIPO_19)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 105);//妖刀
			apply_magic(q_ptr, 75, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
			(void)drop_near(q_ptr, -1, y, x);
		}
		else if(o_ptr->sval == SV_CHEST_MARIPO_20) a_idx = 129;
		else if(o_ptr->sval == SV_CHEST_MARIPO_21) a_idx = 86;
		else if(o_ptr->sval == SV_CHEST_MARIPO_22) a_idx = 22;
		else if(o_ptr->sval == SV_CHEST_MARIPO_23) a_idx = 234;
		//白いオーラ永久
		else if(o_ptr->sval == SV_CHEST_MARIPO_24)
		{
			msg_print(_("箱を開けると、真っ白な煙が噴き出してあなたを包んだ…",
                        "As you open the box, pure white smoke pours forth and envelops you..."));
			msg_print(NULL);
			(void)gain_random_mutation(193);//白いオーラ
			p_ptr->muta3_perma |= MUT3_GOOD_LUCK;

		}
		/*:::レミリアからの書簡*/
		else if (o_ptr->sval == SV_CHEST_REMY)
		{
			msg_print(_("英文の礼状だ。高級そうなリボンが一緒に入っている。",
                        "It contains a thank-you note in English, with a high class ribbon attached to it."));
			q_ptr = &forge;
			object_prep(q_ptr, 160);//リボン
			apply_magic(q_ptr, 20, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
			(void)drop_near(q_ptr, -1, y, x);
		}
		/*:::ドラゴン退治報酬　スピードの指輪+5～9*/
		else if (o_ptr->sval == SV_CHEST_FAIRY)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 259);
			q_ptr->pval = 4 + randint1(3);
			if(EXTRA_MODE) q_ptr->pval += 3;
			if(weird_luck()) q_ptr->pval += 2;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//妹紅刺客クエ報酬　新生、突然変異、爆発、何か涼しくなる薬
		//mod151227 惚れ薬追加
		///mod160618 永琳のときは不死鳥の羽に
		else if(o_ptr->sval == SV_CHEST_EIRIN)
		{
			if(p_ptr->pclass == CLASS_EIRIN || p_ptr->pclass == CLASS_KAGUYA)
			{
				k_idx = lookup_kind(TV_SOUVENIR, SV_SOUVENIR_PHOENIX_FEATHER);
			}
			else
			{
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_NEW_LIFE));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POLYMORPH));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_DETONATIONS));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_NANIKASUZUSHIKUNARU));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_LOVE));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
			}
		}
		/*:::墓地警備員急募報酬　肉体強化の指輪+3～5*/
		else if (o_ptr->sval == SV_CHEST_BYAKUREN)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_MUSCLE));
			q_ptr->pval = 3 + randint1(15)/7;
			(void)drop_near(q_ptr, -1, y, x);
		}
		else if(o_ptr->sval == SV_CHEST_SEIJA)
		{
			msg_print(_("中には金銀財宝と巻物が入っていた。",
                        "It contains gold and silver along with a scroll."));
			p_ptr->au += 1000000L;
			p_ptr->redraw |= (PR_GOLD);
			k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//☆生成
		}
		///mod140629 狐狸戦争
		else if (o_ptr->sval == SV_CHEST_KORI)
		{
			//藍チーム
			if(o_ptr->xtra5 == 1 && p_ptr->pclass != CLASS_RAN) a_idx = ART_GOKASHITIKIN;
			//狸チーム
			else if(o_ptr->xtra5 == 2 && p_ptr->pclass != CLASS_MAMIZOU) a_idx = ART_TONBO;
			//にとりの全滅はアミュでなく大金
			else if(p_ptr->pclass == CLASS_NITORI)
			{
				msg_print(_("皆の掛け金が入っている。あなたの総取りだ。",
                            "You collect the bets everyone made."));
				p_ptr->au += 3000000L;
				p_ptr->redraw |= (PR_GOLD);
			}
			//全滅 浮遊が必ず付く名前固定の反射アミュ☆
			else
			{
				object_level = 50;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_REFLECTION));
				create_artifact(q_ptr, FALSE);
				add_flag(q_ptr->art_flags, TR_LEVITATION);
				q_ptr->art_name = quark_add(_("エンジニア河城にとりの", "of Nitori Kawashiro"));
				(void)drop_near(q_ptr, -1, y, x);
			}

		}


		//v1.1.91 ヤクザ抗争1
		else if (o_ptr->sval == SV_CHEST_YAKUZA1)
		{
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_ZOKU;
				else
					a_idx = ART_TETSUGETA;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_TARNKAPPE;
				else
					a_idx = ART_SPECTRAL_DRAGON;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_TRAPEZOHEDRON;
				else
					a_idx = ART_MAGMA;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_NOMINOSUKUNE;
				else
					a_idx = ART_OOONOTE;
			}
			//全滅 呪われていないランダムな指輪のランダムアーティファクト
			else
			{
				int tmp_sv;

				while (1)
				{
					object_kind *k_ptr;
					int k_idx;
					//ランダムに指輪ベースアイテムを選定
					tmp_sv = randint1(SV_RING_MAX);
					k_idx = lookup_kind(TV_RING, tmp_sv);
					if (!k_idx) continue;
					k_ptr = &k_info[k_idx];
					//呪い確定や★確定のベースアイテムは除外
					if (k_ptr->gen_flags & (TRG_CURSED)) continue;
					if (k_ptr->gen_flags & (TRG_INSTA_ART)) continue;
					break;
				}

				object_level = 60;
				while (1)
				{
					q_ptr = &forge;
					object_prep(q_ptr, lookup_kind(TV_RING, tmp_sv));
					apply_magic(q_ptr, object_level, AM_FORCE_NORMAL);
					create_artifact(q_ptr, FALSE);
					if (object_is_cursed(q_ptr)) continue;
					break;
				}
				object_level = base_level;

				(void)drop_near(q_ptr, -1, y, x);
			}

		}
		//v1.1.98 連続昏睡事件Ⅱ
		else if (o_ptr->sval == SV_CHEST_HANGOKU2)
		{
			int i = 5+randint1(15);
			q_ptr = &forge;
			for (; i > 0; i--)
			{
				int roll = randint1(100);

				if(roll == 100)
					object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_METEORICIRON));
				else if(roll > 90)
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_MAGIC_WATERMELON));
				else if(roll > 75)
					object_prep(q_ptr, lookup_kind(TV_MUSHROOM, SV_MUSHROOM_PUFFBALL));
				else if (roll > 55)
					object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_FLOWER));
				else
					object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_SKULL));

				apply_magic(q_ptr, object_level, 0L);
				(void)drop_near(q_ptr, -1, y, x);
			}

		}
		/*:::サトリ・コンフリクト報酬　魔法職なら第一領域、スペマス系なら使用可能な領域からランダム、どちらでもないなら全くランダムな四冊目魔法書*/
		else if(o_ptr->sval == SV_CHEST_SATORI)
		{
			int book_tv;
			if(cp_ptr->realm_aptitude[0] == 1 || cp_ptr->realm_aptitude[0] == 3) book_tv = p_ptr->realm1;
			else if(p_ptr->pclass == CLASS_CHEMIST) book_tv = TV_BOOK_MEDICINE;
			else if(cp_ptr->realm_aptitude[0] == 2)
			{
				int j,cnt=0;
				for(j=1;j<=MAX_MAGIC;j++)
				{
					///mod スペマス系職の一部で報酬が生成されなかったので修正
					if(cp_ptr->realm_aptitude[j] >1 && rp_ptr->realm_aptitude[j])
					{
						cnt++;
						if(one_in_(cnt)) book_tv = j;
					}
				}

				if(!cnt) book_tv = randint1(TV_BOOK_END-1); //ランダムな本の最終巻　TV=15は未実装の予備領域のため-1しとく
			}
			else book_tv = randint1(TV_BOOK_END-1);

			k_idx = lookup_kind(book_tv,3);

			/*::: Hack - マリサコンクリフトは箱を開ければ達成にする*/
			if(p_ptr->inside_quest == QUEST_MARICON) check_find_art_quest_completion(o_ptr);

		}
		/*:::仙人なんて怖くない　赤蛮奇はシルバーDSM 妖精は加護の指輪 ほかは耐破邪アミュ*/
		else if(o_ptr->sval == SV_CHEST_GRASSROOT)
		{
			if(p_ptr->pclass == CLASS_BANKI) k_idx = lookup_kind(TV_DRAG_ARMOR,SV_DRAGARMOR_SILVER);
			else if(p_ptr->prace == RACE_FAIRY) k_idx = lookup_kind(TV_RING,SV_RING_LORDLY);
			else k_idx =  lookup_kind(TV_AMULET,SV_AMULET_RES_HOLY);

		}
		/*:::ヘカーティア討伐　剣竜の鎧 魔理沙は緋緋色金*/
		else if(o_ptr->sval == SV_CHEST_HECATIA)
		{

			if(p_ptr->pclass == CLASS_MARISA)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else
				a_idx = ART_RAZORBACK;
		}
		//ヤマメ　毒針　鬼の場合酒
		else if(o_ptr->sval == SV_CHEST_YAMAME)
		{

			if(p_ptr->prace == RACE_ONI)
				k_idx = lookup_kind(TV_ALCOHOL, SV_ALCOHOL_SYUTYUU);
			else
				k_idx = lookup_kind(TV_KNIFE, SV_WEAPON_DOKUBARI);
		}
		//「支援物資」と書かれた箱
		else if(o_ptr->sval == SV_CHEST_SUPPLY)
		{
			k_quantity = 10 + randint1(10);
			if(weird_luck()) k_quantity *= 2;

			k_idx = lookup_kind(TV_SWEETS, SV_SWEETS_DANGO);
		}
		///v1.1.18 種族/クラス限定クエスト
		else if (o_ptr->sval == SV_CHEST_SPECIAL_QUEST)
		{

			switch(o_ptr->xtra5)
			{
				case QUEST_TYPHOON:
				{
					q_ptr = &forge;
					object_prep(q_ptr, lookup_kind(TV_HEAD, SV_HEAD_TOKIN));//頭襟
					apply_magic(q_ptr, 40, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
					(void)drop_near(q_ptr, -1, y, x);
				}
				break;
				default:
				{
					msg_format(_("ERROR:この箱のxtra5値がおかしい(%d)",
                                "ERROR: Weird xtra5 (%d) for this chest"),o_ptr->xtra5);
					k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//詫び☆

				}
			}
		}
		///v1.1.24 急流下りクエスト
		else if (o_ptr->sval == SV_CHEST_TORRENT)
		{

			switch(qrkdr_rec_turn)
			{
				case 1:
					msg_print(_("箱には優勝賞品が入っていた。", "It contains the first place prize."));
					k_idx =  lookup_kind(TV_MATERIAL, SV_MATERIAL_IZANAGIOBJECT);
					break;
				case 2:
					msg_print(_("箱には準優勝賞品が入っていた。", "It contains the second place prize."));
					k_idx =  lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONNAIL);
					break;
				case 3:
					msg_print(_("箱には三位入賞賞品が入っていた。", "It contains the third place prize."));
					k_idx =  lookup_kind(TV_POTION, SV_POTION_INC_CON);
					break;

				default:
					msg_print(_("箱には参加賞が入っていた…", "It contains the participation trophy..."));
					k_idx =  lookup_kind(TV_ALCOHOL, SV_ALCOHOL_KAPPA);
					break;
			}
		}
		//v1.1.36 小鈴失踪クエスト
		else if(o_ptr->sval == SV_CHEST_KOSUZU)
		{
			msg_print(_("中には数巻の巻物が入っていた。", "It contains several scrolls."));

			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_FIRE));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_ICE));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_CHAOS));
			q_ptr->number = randint1(3);;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//v1.1.42 夢の＠打倒クエスト
		else if (o_ptr->sval == SV_CHEST_DREAM)
		{
			msg_print(_("中には古びた鍵が入っていた。", "It contains an old key."));
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SHILVER_KEY));
			(void)drop_near(q_ptr, -1, y, x);
		}
		//v1.1.87 10連ガチャ
		else if (o_ptr->sval == SV_CHEST_10_GACHA)
		{
			int i;
			q_ptr = &forge;

			if (p_ptr->pclass == CLASS_CHIMATA)
			{
				msg_print(_("ERROR:千亦でアビリティカードの箱が開けられた",
                            "ERROR: Ability card chest opened with Chimata"));
				return;
			}

			for (i = 0; i < 10; i++)
			{
				object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
				apply_magic(q_ptr, 1, 0);
				if (ability_card_list[q_ptr->pval].rarity_rank >= 4)
				{
					project_hack2(GF_RAINBOW, 0, 0, 777);
				}
				else if (ability_card_list[q_ptr->pval].rarity_rank >= 3)
				{
					project(0,5,py,px,77,GF_LITE_WEAK,(PROJECT_KILL|PROJECT_JUMP),-1);
				}

				(void)drop_near(q_ptr, -1, y, x);

			}

		}
		else if(o_ptr->sval == SV_CHEST_DEBUG)
		{
			int j;
			int rank[ABILITY_CARD_LIST_LEN];
			msg_print(_("テスト用アビリティカード生成", "Generating ability cards for testing"));


			for (j = 0; j < ABILITY_CARD_LIST_LEN; j++)
			{
				rank[j] = 0;
			}
			for (j = 0; j < 1000000; j++)
			{
				int tmp;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
				tmp = apply_magic_abilitycard(q_ptr, -1, 0, 0);
				rank[q_ptr->pval]++;
			}

			for (j = 0; j < ABILITY_CARD_LIST_LEN; j++)
			{
				msg_format("idx%d:%d", j,rank[j]);

			}



			(void)drop_near(q_ptr, -1, y, x);


		}
		else 	msg_print(_("ERROR:chest_death未定義", "ERROR: undefined chest_death"));



		if(a_idx > 0)
		{
			artifact_type *a_ptr = &a_info[a_idx];

			//v1.1.18 銃が出ないオプションだと★銃が出る時☆生成にする
			if(a_ptr->tval == TV_GUN && gun_free_world)
				k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
			else if (!a_ptr->cur_num)
			{
				/* Create the artifact */
				if (create_named_art(a_idx, y, x))
				{
					a_ptr->cur_num = 1;

					/* Hack -- Memorize location of artifact in saved floors */
					if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
				}
				else if (!preserve_mode) a_ptr->cur_num = 1;
			}
			else k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//すでに出現済みの場合☆生成
		}


		if (k_idx)
		{
			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			///mod150103 アイテム生成レベルをobj_levelからplev*2に変更
			apply_magic(q_ptr, (p_ptr->lev * 2), AM_NO_FIXED_ART | AM_GOOD);
			if(k_quantity) q_ptr->number = k_quantity;
			(void)drop_near(q_ptr, -1, y, x);
		}
	}//中身確定箱処理終了



	/* Drop some objects (non-chests) */
	else
	for (; number > 0; --number)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Small chests often drop gold */
		if (small && (randint0(100) < 25))
		{
			/* Make some gold */
			if (!make_gold(q_ptr)) continue;
		}

		/* Otherwise drop an item */
		else
		{
			/* Make a good object */
			/*:::アイテム生成*/
			if (!make_object(q_ptr, mode)) continue;
		}

		/* If chest scatters its contents, pick any floor square. */
		/*:::箱の中身がダンジョン中に散らばる罠にかかったとき*/
		if (scatter)
		{
			int i;
			for (i = 0; i < 200; i++)
			{
				/* Pick a totally random spot. */
				y = randint0(MAX_HGT);
				x = randint0(MAX_WID);

				/* Must be an empty floor. */
				if (!cave_empty_bold(y, x)) continue;

				/* Place the object there. */
				drop_near(q_ptr, -1, y, x);

				/* Done. */
				break;
			}
		}
		/* Normally, drop object near the chest. */
		else drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* No longer opening a chest */
	opening_chest = FALSE;

	/* Empty */
	o_ptr->pval = 0;

	/* Known */
	object_known(o_ptr);
}



//v1.1.97 箱のトラップが発動し罠が消える。chest_trap()をこれに置き換える。
//遠隔解除によって＠以外のモンスターなどを罠にかけることができるようにする。
//このトラップの発動によって箱自体が破壊されることもある。
//trap_player:＠が罠付き箱を開けるか解除失敗して罠にかかるときTRUE。
//FALSEのとき罠によっては周囲の適当なプレイヤーやモンスターをランダムにターゲットしてボルトなどを発射する
void activate_chest_trap(int y, int x, s16b o_idx, bool trap_player)
{

	bool trap_monster = FALSE;
	int  i, trap_type;
	int dam,typ;
	int trap_range = MAX_RANGE;

	u32b summon_flag = (PM_ALLOW_GROUP | PM_NO_PET);

	//ボルトなどが箱から撃ち出されるときのターゲットグリッド
	int ty = 0, tx = 0;

	//箱からターゲットまでの距離
	int target_dist = 0;

	object_type *o_ptr = &o_list[o_idx];//箱なので常に床上

	int chest_level = o_ptr->xtra3;//箱のレベル(生成レベル+5)

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	//もしかすると他のトラップの発動などによりこの箱が破壊された状態で呼ばれるかもしれないので念のため
	if (!o_ptr->k_idx) return;
	if (o_ptr->pval >= CHEST_TRAP_LIST_LENGTH)
	{
		msg_format(_("ERROR:この箱のpvalがトラップインデックスの範囲外(%d)",
                    "ERROR: pval for this chest is out of bounds for trap index (%d)"),o_ptr->pval);
		o_ptr->pval = 0 - o_ptr->pval;//GF_ACTIV_TRAP経由でここに来た時箱が解錠されないとループになると思うので一応
		return;
	}

	//罠の種類を取得
	trap_type = chest_new_traps[o_ptr->pval];

	//フランが鍵開けに失敗するとよく爆発する
	if (p_ptr->pclass == CLASS_FLAN && one_in_(2)) trap_type = (CHEST_TRAP_EXPLODE);

	//箱を解除済みにする
	o_ptr->pval = (0 - o_ptr->pval);
	object_known(o_ptr);

	//v1.1.98 トラップなし(施錠)のとき終了
	if (trap_type == CHEST_TRAP_NOTHING) return;

	//GF_ACTIV_TRAP経由でここに来た時箱が解錠されないとループになると思うので時間停止中でも解除しておく
	if (SAKUYA_WORLD || world_player)
	{
		msg_print(_("箱の罠は作動しなかった。", "The trap on the chest didn't go off."));
		return;
	}


	//ターゲットグリッドと箱からターゲットまでの距離を求める。トラップの種類によってはこの処理が無駄になるが大した問題はないだろう
	//＠が普通に箱の罠にかかったとき
	if (trap_player)
	{
		tx = px;
		ty = py;
		if (!player_bold(y, x)) target_dist = 1;
	}
	//箱の上に誰かがいる状態で箱のトラップが起動されたとき
	else if (player_bold(y, x) || cave[y][x].m_idx)
	{
		tx = x;
		ty = y;
	}
	//それ以外のとき、箱の近くのプレイヤーやモンスターをランダムに選定する
	else
	{
		int tmp_dist = 255;
		//まず＠を初期値にする
		if (projectable(py, px, y, x))
		{
			tx = px;
			ty = py;
			tmp_dist = distance(py, px, y, x);
		}
		//それ以下の距離にモンスターがいればそちらが選ばれる(処理の簡略化のためm_idxが高いモンスター優先)
		for (i = 0; i < m_max; i++)
		{
			int tmp_dist2;

			monster_type *m_ptr_tmp = &m_list[i];
			if (!m_ptr_tmp->r_idx) continue;

			tmp_dist2 = distance(y, x, m_ptr_tmp->fy, m_ptr_tmp->fx);
			if (tmp_dist2 > tmp_dist) continue; //等距離なら＠よりモンスター優先で当たることになる
			if (!projectable(y, x, m_ptr_tmp->fy, m_ptr_tmp->fx)) continue;

			tmp_dist = tmp_dist2;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		target_dist = tmp_dist;


	}

	//＠勢力以外のモンスターがターゲットになっているとき召喚トラップの召喚フラグを変更
	if (!trap_player && cave[ty][tx].m_idx && is_hostile(&m_list[cave[ty][tx].m_idx]))
	{
		summon_flag = (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY);
	}

	//if (p_ptr->wizard) trap_type = CHEST_TRAP_FUSION; //テスト用

	switch (trap_type)
	{

	case CHEST_TRAP_LOSE_STR:
	case CHEST_TRAP_LOSE_CON:
	case CHEST_TRAP_LOSE_MAG:

		if (trap_type == CHEST_TRAP_LOSE_STR)
			typ = GF_DEC_ATK;
		else if (trap_type == CHEST_TRAP_LOSE_CON)
			typ = GF_DEC_DEF;
		else
			typ = GF_DEC_MAG;

		//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
		trap_range = 6;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 100 + randint1(chest_level);
		msg_print(_("箱から小さなダーツが撃ち出された。", "A small dart shoots out from the chest."));
		//project()の発射位置を特殊変数で設定して射出処理
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, typ, (PROJECT_REFLECTABLE | PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_TELEPORTER:
	{
		trap_range = 5;
		msg_print(_("おおっと！テレポーター！", "Oops! A teleporter!"));
		//まず＠に影響を及ぼさない周辺モンスターテレポート
		project(0, trap_range, y, x, 100, GF_AWAY_ALL, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);
		//範囲内の＠を強制テレポート。壁の中に出ることもある。
		if (distance(py, px, y, x) < trap_range) teleporter_trap();
		break;
	}

	case CHEST_TRAP_BA_POIS:
		dam = chest_level * 3;
		msg_print(_("箱から緑色のガスが噴き出した！", "Green gas pours out from the chest!"));
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_POIS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_BA_SLEEP:
		dam = 150 + chest_level * 3;
		msg_print(_("箱から白いガスが噴き出した！", "White gas pours out from the chest!"));
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_OLD_SLEEP, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_BA_CONF:
		dam = 150 + chest_level * 3;
		msg_print(_("箱からきらめくガスが噴き出した！", "Gas of scintillating colors pours out from the chest!"));
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_EXPLODE:
		dam = 150 + chest_level * 2;
		msg_print(_("おおっと！爆弾！", "Oops! A bomb!"));
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_SHARDS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		//この箱アイテム消滅
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);

		break;

	case CHEST_TRAP_ALARM:
		dam = 150 + chest_level * 3;

#ifdef JP
		msg_print("けたたましい音が鳴り響いた！");
#else
		msg_print("There is a loud, shrill noise!");
#endif
		//スピードモンスターをなくす
		aggravate_monsters(0, FALSE);
		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_STUN, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

		break;

	case CHEST_TRAP_SUMMON:
	case CHEST_TRAP_S_BIRD:
	case CHEST_TRAP_S_ELEMENTAL:
	case CHEST_TRAP_S_DEMON:
	case CHEST_TRAP_S_DRAGON:
	case CHEST_TRAP_S_CHIMERA:
	case CHEST_TRAP_S_VORTEX:
	case CHEST_TRAP_S_KWAI:
	{
		int summon_num;
		bool summon_done = FALSE;
		cptr summon_msg;
		int summon_lev;
		int summon_mode;

		switch (trap_type)
		{
		case CHEST_TRAP_S_KWAI:
			summon_num = 4 + randint1(4);
			summon_msg = _("つづらの中から魑魅魍魎がぞろぞろ湧き出てきた！",
                         "Wicked spirits come out of the basket!");
			summon_lev = chest_level;
			summon_mode = SUMMON_KWAI;
			break;

		case CHEST_TRAP_S_VORTEX:
			summon_num = 5 + randint1(5);
			summon_msg = _("渦巻が合体し、破裂した！", "Vortices coalesce and burst forth!");
			summon_lev = chest_level / 2;
			summon_mode = SUMMON_VORTEX;
			break;



		case CHEST_TRAP_S_CHIMERA:
			summon_num = 3 + randint1(3);
			summon_msg = _("奇妙な姿の怪物が襲って来た！", "Strange beast come forth!");
			summon_lev = chest_level;
			summon_mode = SUMMON_HYBRID;
			break;

		case CHEST_TRAP_S_DRAGON:
			summon_num = 2 + randint1(2);
			summon_msg = _("暗闇にドラゴンの影がぼんやりと現れた！", "Dragonic forms appear in the darkness!");
			summon_lev = chest_level;
			summon_mode = SUMMON_DRAGON;
			break;

		case CHEST_TRAP_S_DEMON:
			summon_num = 2 + randint1(2);
			summon_msg = _("炎と硫黄の雲の中に悪魔が姿を現した！", "Demons appear in a cloud of fire and sulfur!");
			summon_lev = chest_level;
			summon_mode = SUMMON_DEMON;
			break;

		case CHEST_TRAP_S_ELEMENTAL:
			summon_num = 3 + randint1(3);
			summon_msg = _("宝を守るためにエレメンタルが現れた！", "Elementals guarding the treasure appear!");
			summon_lev = chest_level;
			summon_mode = SUMMON_ELEMENTAL;
			break;

		case CHEST_TRAP_S_BIRD:
			summon_num = 5 + randint1(5);
			summon_msg = _("箱から鳥の群れが飛び出した！", "A flock of birds flies out from the chest!");
			summon_lev = chest_level / 2 + 1;
			summon_mode = SUMMON_BIRD;
			break;


		default:
			summon_num = 4 + randint1(4);
			summon_msg = _("箱からモンスターが現れた！", "Monsters appear from the chest!");
			summon_lev = chest_level;
			summon_mode = 0;
			break;



		}

		for (i = 0; i < summon_num; i++)
		{
			if(summon_specific(0, y, x, summon_lev, summon_mode, summon_flag)) summon_done = TRUE;
		}
		if (summon_done)
		{
			msg_format("%s", summon_msg);

			if (summon_flag & PM_FORCE_FRIENDLY)
				msg_print(_("召喚のルーンはターゲットを誤認したようだ！", "Looks like the summoning runes misidentified their target!"));
		}
		else
		{
			msg_print(_("召喚のルーンは不発だった！", "The summoning runes failed to activate!"));
		}

		break;
	}

	case CHEST_TRAP_SUIKI:
	{
		msg_print(_("地面から超高圧の水流が噴き出した！", "A jet of water bursts out from the ground at extremely high pressure!"));
		dam = chest_level * 4;

		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_WATER_FLOW, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_WATER, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);


		if (trap_player)
		{
			msg_print(_("あなたは一瞬動きを封じられた！", "You are momentarily unable to move!"));
			p_ptr->energy_need += ENERGY_NEED();
		}
		else if (cave[y][x].m_idx)
		{
			char            m_name[80];
			monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
			msg_format(_("%sは怯んでいる！", "%^s is shaken!"),m_name);
			monster_delay(cave[y][x].m_idx, 100 + randint1(100));
			project(PROJECT_WHO_TRAP, 0, y, x, 25+chest_level/4, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);


		}

		break;
	}


	case CHEST_TRAP_RUIN:

		if (trap_player)
		{
			/* Determine how many nasty tricks can be played. */
			int nasty_tricks_count = 4 + randint0(3);

			/* Message. */
#ifdef JP
			msg_print("恐ろしい声が響いた:  「暗闇が汝をつつまん！」");
#else
			msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

			/* This is gonna hurt... */
			for (; nasty_tricks_count > 0; nasty_tricks_count--)
			{
				/* ...but a high saving throw does help a little. */
				if (randint1(100 + chest_level) > p_ptr->skill_sav)
				{
#ifdef JP
					if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "破滅のトラップの宝箱", -1);
#else
					if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "ruination chest trap", -1);
#endif
					else if (one_in_(5)) (void)set_cut(p_ptr->cut + 200);
					else if (one_in_(4))
					{
						if (!p_ptr->free_act)
							(void)set_paralyzed(p_ptr->paralyzed + 2 +
								randint0(6));
						else
							(void)set_stun(p_ptr->stun + 10 +
								randint0(100));
					}
					else if (one_in_(3)) apply_disenchant(0, 0);
					else if (one_in_(2))
					{
						(void)do_dec_stat(A_STR);
						(void)do_dec_stat(A_DEX);
						(void)do_dec_stat(A_CON);
						(void)do_dec_stat(A_INT);
						(void)do_dec_stat(A_WIS);
						(void)do_dec_stat(A_CHR);
					}
					else (void)fire_meteor(-1, GF_NETHER, y, x, 150, 1);
				}
			}
		}
		else if (target_dist <= 1)
		{
			msg_print(_("邪悪なルーンが発動した！", "Evil runes get activated!"));
			project(0, 0, ty, tx, 100 + chest_level * 3, GF_DEC_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(0, 0, ty, tx, 10 + chest_level / 2, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(PROJECT_WHO_TRAP, 0, y, x, 100+chest_level*2, GF_HAND_DOOM, (PROJECT_KILL | PROJECT_HIDE | PROJECT_JUMP), -1);

		}
		else
		{
			msg_print(_("邪悪なルーンが発動したが不発に終わったようだ。", "Evil runes were activated, but failed to go off."));

		}
		break;

	case CHEST_TRAP_SLINGSHOT:

		//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
		trap_range = 6;
		if(target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 10 + randint1(chest_level/2);
		msg_print(_("箱から石礫が撃ち出された！", "A stone flies out from the chest!"));
		//project()の発射位置を特殊変数で設定して射出処理
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP|PROJECT_KILL|PROJECT_GRID), -1);
		break;


	case CHEST_TRAP_ARROW:

		//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
		trap_range = 12;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 25 + randint1(chest_level);
		msg_print(_("箱から矢が撃ち出された！", "An arrow shoots out from the chest!"));
		//project()の発射位置を特殊変数で設定して射出処理
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_STEEL_ARROW:

		//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
		trap_range = 15;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 50 + damroll(2,chest_level);
		msg_print(_("箱から鋼鉄の矢が撃ち出された！", "A steel arrow shoots out from the chest!"));
		//project()の発射位置を特殊変数で設定して射出処理
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_PUNCH:
	{
		int	dice = chest_level / 10 + 4;
		int	sides = dice;
		dam = damroll(dice, sides);
		if (trap_player)
		{
			msg_print(_("バネ仕掛けのパンチンググローブがあなたの顔面にめり込んだ！",
                    "A spring-loaded boxing glove hits you in the face!"));
			take_hit(DAMAGE_NOESCAPE, dam, _("罠", "trap"), -1);
			(void)set_stun(p_ptr->stun + 50 + randint0(50));
		}
		else
		{
			//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
			trap_range = 2;
			if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
			msg_print(_("箱からパンチンググローブが撃ち出された！", "A boxing glove shoots out from the chest!"));
			//project()の発射位置を特殊変数で設定して射出処理
			hack_project_start_x = x;
			hack_project_start_y = y;
			(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_FORCE, (PROJECT_KILL), -1);
		}
		break;
	}

	case CHEST_TRAP_BR_FIRE:
	case CHEST_TRAP_BR_ACID:
		//トラップの射程を設定し、その範囲内にターゲットがいなければ適当に決める
		trap_range = 8;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = chest_level * 6;
		if (dam < 100) dam = 100;
		if (trap_type == CHEST_TRAP_BR_FIRE)
		{
			typ = GF_FIRE;
			msg_print(_("火炎放射だ！", "Fire shoots out!"));
		}
		else
		{
			typ = GF_ACID;
			msg_print(_("箱から強酸が噴き出した！", "Acid sprays out from the chest!"));
		}

		//project()の発射位置を特殊変数で設定して射出処理
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, -2, ty, tx, dam, typ, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER), -1);
		break;

	case CHEST_TRAP_BA_TIME:
		dam = chest_level * 2 + randint1(chest_level);
		msg_print(_("逆玉手箱だ！", "It's a time reversal trap!"));
		project(PROJECT_WHO_TRAP, 5, y, x, dam, GF_TIME, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

		break;


	case CHEST_TRAP_MIMIC:
		msg_print(_("なんと箱はミミックだった！", "What? The chest was a mimic!"));
		//この箱アイテム消滅
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);
		//ミミック召喚
		if (!summon_named_creature(0, y, x, MON_MIMIC_CHEST, 0L))
			msg_print(_("しかしミミックはどこかへ消えた。", "The mimic disappears into thin air."));

		break;

	case CHEST_TRAP_MAGIC_DRAIN:
		trap_range = 5;
		dam = chest_level * 4;
		msg_print(_("周囲の魔力が箱に吸い込まれた！", "The chest sucks in nearby mana!"));

		//＠に影響を及ぼさない魔法力低下のボール
		project(0, trap_range, y, x, dam, GF_DEC_MAG, (PROJECT_JUMP | PROJECT_KILL), -1);
		//範囲内モンスターに魔力消去
		for (i = 1; i<m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if (!m_ptr->r_idx) continue;
			if (distance(m_ptr->fy, m_ptr->fx, y, x) > trap_range) continue;
			if (!projectable(m_ptr->fy, m_ptr->fx, y, x)) continue;

			dispel_monster_status(i);
		}

		//＠が範囲内なら魔力消去とMPダメージ
		if (distance(py, px, y, x) < trap_range)
		{
			dispel_player();
			p_ptr->csp -= dam;
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
			}

		}

		break;

	case CHEST_TRAP_BA_BERSERK:
		dam = 150 + randint1(chest_level * 3);
		msg_print(_("箱から真っ赤なガスが噴き出した！", "Bright red gas pours out from the chest!"));
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_BERSERK, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_FUSION:
		msg_print(_("おおっと！ニュークリアフュージョン！", "Oops! Nuclear fusion!"));
		dam = chest_level * 8;
		if (dam < 300) dam = 300;

		//この箱アイテム消滅
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);
		//視界内核熱攻撃
		project_hack3(PROJECT_WHO_TRAP, y, x, GF_NUKE, 0, 0, dam);
		break;


	default:
		msg_format(_("ERROR:trap_type(%d)の処理が定義されていない",
                    "ERROR: Undefined logic for trap_type %d"),trap_type);
		break;
	}

	hack_project_start_x = 0;
	hack_project_start_y = 0;

}

/*
 * Chests have traps too.
 *
 * Exploding chest destroys contents (and traps).
 * Note that the chest itself is never destroyed.
 */
/*:::箱のトラップが発動する*/
//v1.1.97 この関数をchest_trap()に置き換えた
#if 0
static void chest_trap(int y, int x, s16b o_idx)
{
	int  i, trap;

	object_type *o_ptr = &o_list[o_idx];

	int mon_level = o_ptr->xtra3;

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	/* Obtain the traps */
	/*:::tables.cのchest_traps[]にpvalごとの罠の種類が記述されている。intだがビットフラグなので罠が複数あることもある*/
	trap = chest_traps[o_ptr->pval];

	//フランが鍵開けに失敗するとよく爆発する
	if(p_ptr->pclass == CLASS_FLAN && one_in_(2)) trap = (CHEST_EXPLODE);

	if(SAKUYA_WORLD || world_player)
	{
		msg_print(_("箱の罠は作動しなかった。", "The chest trap didn't go off."));
		return;
	}

	/* Lose strength */
	if (trap & (CHEST_LOSE_STR))
	{
#ifdef JP
		msg_print("仕掛けられていた小さな針に刺されてしまった！");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "毒針", -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "a poison needle", -1);
#endif

		(void)do_dec_stat(A_STR);
	}

	/* Lose constitution */
	if (trap & (CHEST_LOSE_CON))
	{
#ifdef JP
		msg_print("仕掛けられていた小さな針に刺されてしまった！");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "毒針", -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "a poison needle", -1);
#endif

		(void)do_dec_stat(A_CON);
	}

	/* Poison */
	if (trap & (CHEST_POISON))
	{
#ifdef JP
		msg_print("突如吹き出した緑色のガスに包み込まれた！");
#else
		msg_print("A puff of green gas surrounds you!");
#endif

		if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
		{
			(void)set_poisoned(p_ptr->poisoned + 10 + randint1(20));
		}
		if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 3000);
	}

	/* Paralyze */
	if (trap & (CHEST_PARALYZE))
	{
#ifdef JP
		msg_print("突如吹き出した黄色いガスに包み込まれた！");
#else
		msg_print("A puff of yellow gas surrounds you!");
#endif


		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + 10 + randint1(20));
		}
	}

	/* Summon monsters */
	if (trap & (CHEST_SUMMON))
	{
		int num = 2 + randint1(3);
#ifdef JP
		msg_print("突如吹き出した煙に包み込まれた！");
#else
		msg_print("You are enveloped in a cloud of smoke!");
#endif


		for (i = 0; i < num; i++)
		{
			if (randint1(100)<dun_level)
				activate_hi_summon(py, px, FALSE);
			else
				(void)summon_specific(0, y, x, mon_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Elemental summon. */
	if (trap & (CHEST_E_SUMMON))
	{
#ifdef JP
		msg_print("宝を守るためにエレメンタルが現れた！");
#else
		msg_print("Elemental beings appear to protect their treasures!");
#endif
		for (i = 0; i < randint1(3) + 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Force clouds, then summon birds. */
	if (trap & (CHEST_BIRD_STORM))
	{
#ifdef JP
		msg_print("鳥の群れがあなたを取り巻いた！");
#else
		msg_print("A storm of birds swirls around you!");
#endif

		for (i = 0; i < randint1(3) + 3; i++)
			(void)fire_meteor(-1, GF_FORCE, y, x, o_ptr->pval / 5, 7);

		for (i = 0; i < randint1(5) + o_ptr->pval / 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_BIRD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Various colorful summonings. */
	if (trap & (CHEST_H_SUMMON))
	{
		/* Summon demons. */
		if (one_in_(4))
		{
#ifdef JP
			msg_print("炎と硫黄の雲の中に悪魔が姿を現した！");
#else
			msg_print("Demons materialize in clouds of fire and brimstone!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)fire_meteor(-1, GF_FIRE, y, x, 10, 5);
				(void)summon_specific(0, y, x, mon_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon dragons. */
		else if (one_in_(3))
		{
#ifdef JP
			msg_print("暗闇にドラゴンの影がぼんやりと現れた！");
#else
			msg_print("Draconic forms loom out of the darkness!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon hybrids. */
		else if (one_in_(2))
		{
#ifdef JP
			msg_print("奇妙な姿の怪物が襲って来た！");
#else
			msg_print("Creatures strange and twisted assault you!");
#endif

			for (i = 0; i < randint1(5) + 3; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_HYBRID, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon vortices (scattered) */
		else
		{
#ifdef JP
			msg_print("渦巻が合体し、破裂した！");
#else
			msg_print("Vortices coalesce and wreak destruction!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_VORTEX, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}
	}

	/* Dispel player. */
	if ((trap & (CHEST_RUNES_OF_EVIL)) && o_ptr->k_idx)
	{
		/* Determine how many nasty tricks can be played. */
		int nasty_tricks_count = 4 + randint0(3);

		/* Message. */
#ifdef JP
		msg_print("恐ろしい声が響いた:  「暗闇が汝をつつまん！」");
#else
		msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

		/* This is gonna hurt... */
		for (; nasty_tricks_count > 0; nasty_tricks_count--)
		{
			/* ...but a high saving throw does help a little. */
			if (randint1(100+o_ptr->pval*2) > p_ptr->skill_sav)
			{
#ifdef JP
				if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "破滅のトラップの宝箱", -1);
#else
				if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "a chest dispel-player trap", -1);
#endif
				else if (one_in_(5)) (void)set_cut(p_ptr->cut + 200);
				else if (one_in_(4))
				{
					if (!p_ptr->free_act)
						(void)set_paralyzed(p_ptr->paralyzed + 2 +
						randint0(6));
					else
						(void)set_stun(p_ptr->stun + 10 +
						randint0(100));
				}
				else if (one_in_(3)) apply_disenchant(0,0);
				else if (one_in_(2))
				{
					(void)do_dec_stat(A_STR);
					(void)do_dec_stat(A_DEX);
					(void)do_dec_stat(A_CON);
					(void)do_dec_stat(A_INT);
					(void)do_dec_stat(A_WIS);
					(void)do_dec_stat(A_CHR);
				}
				else (void)fire_meteor(-1, GF_NETHER, y, x, 150, 1);
			}
		}
	}

	/* Aggravate monsters. */
	if (trap & (CHEST_ALARM))
	{
#ifdef JP
		msg_print("けたたましい音が鳴り響いた！");
#else
		msg_print("An alarm sounds!");
#endif
		aggravate_monsters(0,TRUE);
	}

	/* Explode */
	/*:::爆発箱。箱のpvalが0になることでこのあとのchest_death()でアイテムが出ない*/
	if ((trap & (CHEST_EXPLODE)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("突然、箱が爆発した！");
		msg_print("箱の中の物はすべて粉々に砕け散った！");
#else
		msg_print("There is a sudden explosion!");
		msg_print("Everything inside the chest is destroyed!");
#endif

		o_ptr->pval = 0;
		sound(SOUND_EXPLODE);
#ifdef JP
		take_hit(DAMAGE_ATTACK, damroll(5, 8), "爆発する箱", -1);
#else
		take_hit(DAMAGE_ATTACK, damroll(5, 8), "an exploding chest", -1);
#endif

	}
	/* Scatter contents. */
	if ((trap & (CHEST_SCATTER)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("宝箱の中身はダンジョンじゅうに散乱した！");
#else
		msg_print("The contents of the chest scatter all over the dungeon!");
#endif
		chest_death(TRUE, y, x, o_idx);
		o_ptr->pval = 0;
	}
}
#endif


/*
 * Attempt to open the given chest at the given location
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::箱を開ける。罠があったら発動する。*/
static bool do_cmd_open_chest(int y, int x, s16b o_idx)
{
	int i, j;

	bool flag = TRUE;

	bool more = FALSE;

	object_type *o_ptr = &o_list[o_idx];


	/* Take a turn */
	energy_use = 100;

	/* Attempt to unlock it */
	/*:::鍵が掛かっている（か罠がある）場合箱のpvalがプラス*/
	/*:::解除スキルとpvalで開錠成功率を計算　ただし成功しても罠は解除されない*/
	if (o_ptr->pval > 0)
	{
		/* Assume locked, and thus not open */
		flag = FALSE;

		/* Get the "disarm" factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the difficulty */
		j = i - o_ptr->pval;

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Success -- May still have traps */
		if (randint0(100) < j)
		{
#ifdef JP
			msg_print("鍵をはずした。");
#else
			msg_print("You have picked the lock.");
#endif

			gain_exp(1);
			flag = TRUE;
		}

		/* Failure -- Keep trying */
		else
		{
			/* We may continue repeating */
			more = TRUE;
			if (flush_failure) flush();
#ifdef JP
			msg_print("鍵をはずせなかった。");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Allowed to open */
	if (flag)
	{
		/* Apply chest traps, if any */
		/*:::箱の罠があれば発動する*/
		//v1.1.97 箱トラップ仕様変更
		activate_chest_trap(y, x, o_idx, TRUE);
		//chest_trap(y, x, o_idx);

		/* Let the Chest drop items */
		/*:::箱が開き、アイテムが出る*/
		//v1.1.97 箱の罠によってこの箱が破壊される可能性があるので条件式追加
		if (!o_list[o_idx].k_idx)
		{
			if (cheat_peek) msg_print(_("(箱が破壊されている)", "(the chest was destroyed)"));
		}
		else
		{
			chest_death(FALSE, y, x, o_idx);
		}
	}

	/* Result */
	return (more);
}


#if defined(ALLOW_EASY_OPEN) || defined(ALLOW_EASY_DISARM) /* TNB */

/*
 * Return TRUE if the given feature is an open door
 */
static bool is_open(int feat)
{
	return have_flag(f_info[feat].flags, FF_CLOSE) && (feat != feat_state(feat, FF_CLOSE));
}



/*
 * Convert an adjacent location to a direction.
 */
static int coords_to_dir(int y, int x)
{
	int d[3][3] = { {7, 4, 1}, {8, 5, 2}, {9, 6, 3} };
	int dy, dx;

	dy = y - py;
	dx = x - px;

	/* Paranoia */
	if (ABS(dx) > 1 || ABS(dy) > 1) return (0);

	return d[dx + 1][dy + 1];
}

#endif /* defined(ALLOW_EASY_OPEN) || defined(ALLOW_EASY_DISARM) -- TNB */


/*
 * Perform the basic "open" command on doors
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::ドアを開けるor開錠する*/
static bool do_cmd_open_aux(int y, int x)
{
	int i, j;

	/* Get requested grid */
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	bool more = FALSE;


	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		///mod140829 フランはドアを問答無用で壊す
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("%sはがっちりと閉じられていたようだ。", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else msg_format("%sはがっちりと閉じられているようだ。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("The %s used to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else msg_format("The %s appears to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif


	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Flan */
		if(p_ptr->pclass == CLASS_FLAN)
		{
			msg_format(_("%sに鍵がかかっていたようだ。",
                        "The %s used to be locked."), f_name + f_info[get_feat_mimic(c_ptr)].name);
			cave_alter_feat(y, x, FF_BASH);
		}
		/* Success */
		else if (randint0(100) < j)
		{
			/* Message */
#ifdef JP
			msg_print("鍵をはずした。");
#else
			msg_print("You have picked the lock.");
#endif

			/* Open the door */
			cave_alter_feat(y, x, FF_OPEN);

			/* Sound */
			sound(SOUND_OPENDOOR);

			/* Experience */
			gain_exp(1);
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("鍵をはずせなかった。");
#else
			msg_print("You failed to pick the lock.");
#endif


			/* We may keep trying */
			more = TRUE;
		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (more);
}



/*
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked door/chest is worth one experience point.
 */
/*:::ドアか箱を開ける*/
void do_cmd_open(void)
{
	int y, x, dir;

	s16b o_idx;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_OPEN /* TNB */

	/* Option: Pick a direction */
	/*:::easy_openのとき適当な一つがあれば自動的にターゲットとして選択する*/
	if (easy_open)
	{
		int num_doors, num_chests;

		/* Count closed doors (locked or jammed) */
		/*:::＠の周辺の閉じたドアの数を数える*/
		num_doors = count_dt(&y, &x, is_closed_door, FALSE);

		/* Count chests (locked) */
		/*:::＠の周辺の箱のあるグリッドの数を数える*/
		num_chests = count_chests(&y, &x, FALSE);

		/* See if only one target */
		if (num_doors || num_chests)
		{
			bool too_many = (num_doors && num_chests) || (num_doors > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_OPEN -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir, TRUE))
	{
		s16b feat;
		cave_type *c_ptr;

		/* Get requested location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get requested grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Check for chest */
		o_idx = chest_check(y, x, FALSE);

		/* Nothing useful */
		if (!have_flag(f_info[feat].flags, FF_OPEN) && !o_idx)
		{
			/* Message */
#ifdef JP
		msg_print("そこには開けるものが見当たらない。");
#else
			msg_print("You see nothing there to open.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx && p_ptr->riding != c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
		msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Handle chests */
		else if (o_idx)
		{
			/* Open the chest */
			more = do_cmd_open_chest(y, x, o_idx);
		}

		/* Handle doors */
		else
		{
			/* Open the door */
			more = do_cmd_open_aux(y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}



/*
 * Perform the basic "close" command
 *
 * Assume destination is an open/broken door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::ドアを閉じる*/
static bool do_cmd_close_aux(int y, int x)
{
	/* Get grid and contents */
	cave_type *c_ptr = &cave[y][x];
	s16b      old_feat = c_ptr->feat;
	bool      more = FALSE;

	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Open door */
	if (have_flag(f_info[old_feat].flags, FF_CLOSE))
	{
		s16b closed_feat = feat_state(old_feat, FF_CLOSE);

		/* Hack -- object in the way */
		/*:::ここ良く解らん。開いたドアの場所にアイテム落としたりルーン敷いたりできないはずだが*/
		if ((c_ptr->o_idx || (c_ptr->info & CAVE_OBJECT)) &&
		    (closed_feat != old_feat) && !have_flag(f_info[closed_feat].flags, FF_DROP))
		{
			/* Message */
#ifdef JP
			msg_print("何かがつっかえて閉まらない。");
#else
			msg_print("There seems stuck.");
#endif
		}
		else
		{
			/* Close the door */
			cave_alter_feat(y, x, FF_CLOSE);

			/* Broken door */
			if (old_feat == c_ptr->feat)
			{
				/* Message */
#ifdef JP
				msg_print("ドアは壊れてしまっている。");
#else
				msg_print("The door appears to be broken.");
#endif
			}
			else
			{
				/* Sound */
				sound(SOUND_SHUTDOOR);
			}
		}
	}

	/* Result */
	return (more);
}


/*
 * Close an open door.
 */
/*:::ドアを閉める*/
void do_cmd_close(void)
{
	int y, x, dir;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_OPEN /* TNB */

	/* Option: Pick a direction */
	if (easy_open)
	{
		/* Count open doors */
		if (count_dt(&y, &x, is_open, FALSE) == 1)
		{
			command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_OPEN -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get requested location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require open/broken door */
		if (!have_flag(f_info[feat].flags, FF_CLOSE))
		{
			/* Message */
#ifdef JP
			msg_print("そこには閉じるものが見当たらない。");
#else
			msg_print("You see nothing there to close.");
#endif
		}

		/* Monster in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Close the door */
		else
		{
			/* Close the door */
			more = do_cmd_close_aux(y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}


/*
 * Determine if a given grid may be "tunneled"
 */
/*:::見えない地形と掘れない地形はFALSEを返し、掘れる地形はTRUEを返す*/
static bool do_cmd_tunnel_test(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Must have knowledge */
	if (!(c_ptr->info & CAVE_MARK))
	{
		/* Message */
#ifdef JP
		msg_print("そこには何も見当たらない。");
#else
		msg_print("You see nothing there.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Must be a wall/door/etc */
	if (!cave_have_flag_grid(c_ptr, FF_TUNNEL))
	{
		/* Message */
#ifdef JP
		msg_print("そこには掘るものが見当たらない。");
#else
		msg_print("You see nothing there to tunnel.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}


/*
 * Perform the basic "tunnel" command
 *
 * Assumes that no monster is blocking the destination
 *
 * Do not use twall anymore
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::トンネルを掘る作業　リピート中のときTRUE,掘り終わる/掘れないときFALSEを返す*/
static bool do_cmd_tunnel_aux(int y, int x)
{
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;
	int power;
	cptr name;
	bool more = FALSE;

	/* Verify legality */
	/*:::掘れる地形かどうか判別*/
	if (!do_cmd_tunnel_test(y, x)) return (FALSE);

	/* Take a turn */
	energy_use = 100;

	/* Get grid */
	/*:::f_info[]からpowerを得る　mimicでなく実体*/
	c_ptr = &cave[y][x];
	f_ptr = &f_info[c_ptr->feat];
	power = f_ptr->power;

	//フランはすぐに壁を掘れる
	//勇儀と天子も
	//袿姫も
	if(p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI) power = 0;

	/* Feature code (applying "mimic" field) */
	mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];
	/*:::f_info[get_feat_mimic(c_ptr)]から地形名を得る*/
	name = f_name + mimic_f_ptr->name;

	/* Sound */
	sound(SOUND_DIG);
	/*:::永久壁は掘れない*/
	if (have_flag(f_ptr->flags, FF_PERMANENT))
	{
		/* Titanium */
		if (have_flag(mimic_f_ptr->flags, FF_PERMANENT))
		{
#ifdef JP
			msg_print("この岩は硬すぎて掘れないようだ。");
#else
			msg_print("This seems to be permanent rock.");
#endif
		}

		/* Map border (mimiccing Permanent wall) */
		else
		{
#ifdef JP
			msg_print("そこは掘れない!");
#else
			msg_print("You can't tunnel through that!");
#endif
		}
	}

	/* Dig or tunnel */
	/*:::地形フラグとpowerとp_ptr->skill_digで穴掘り判定。*/
	/*:::木や壁の場合power+randint(40*power)が掘削技能値を下回れば穴掘り成功*/
	/*:::穴掘りに成功したら地形を地面にし、FALSEを返す*/
	/*:::掘れなかったらTRUEを返す*/
	/*:::隠しドアを掘っているとき1/4で地形感知判定*/
	///sys skill 掘削技能関連
	else if (have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		/* Dig */
		if (p_ptr->skill_dig > randint0(20 * power))
		{
			/* Message */
#ifdef JP
			msg_format("%sをくずした。", name);
#else
			msg_format("You have removed the %s.", name);
#endif

			/* Remove the feature */
			cave_alter_feat(y, x, FF_TUNNEL);

			/* Update some things */
			p_ptr->update |= (PU_FLOW);
		}
		else
		{
			/* Message, keep digging */
#ifdef JP
			msg_format("%sをくずしている。", name);
#else
			msg_format("You dig into the %s.", name);
#endif

			more = TRUE;
		}
	}

	else
	{
		bool tree = have_flag(mimic_f_ptr->flags, FF_TREE);

		/* Tunnel */
		if (p_ptr->skill_dig > power + randint0(40 * power))
		{
#ifdef JP
			if (tree) msg_format("%sを切り払った。", name);
			else
			{

				if(difficulty == DIFFICULTY_LUNATIC && one_in_(100))
				{
					msg_print("落盤だ！");
					earthquake(y, x, 3 + randint1(3));

				}
				else
				{
					if(p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI)
						msg_print("一瞬で壁に大穴が開いた！");
					else if(p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI)
						msg_print("壁が引っ込んだ。");
					else
						msg_print("穴を掘り終えた。");
				}
				p_ptr->update |= (PU_FLOW);
			}
#else
			if (tree) msg_format("You have cleared away the %s.", name);
			else
			{
				if(difficulty == DIFFICULTY_LUNATIC && one_in_(100))
				{
					msg_print("Cave-in!");
					earthquake(y, x, 3 + randint1(3));

				}
				else
				{
					if(p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI)
						msg_print("In an instant, you open up a large hole in the wall!");
					else if(p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI)
						msg_print("You chisel down the wall.");
					else
						msg_print("You have finished the tunnel.");
				}
				p_ptr->update |= (PU_FLOW);
			}
#endif

			/* Sound */
			if (have_flag(f_ptr->flags, FF_GLASS)) sound(SOUND_GLASS);

			/* Remove the feature */
			cave_alter_feat(y, x, FF_TUNNEL);

			chg_virtue(V_DILIGENCE, 1);
			chg_virtue(V_NATURE, -1);
		}

		/* Keep trying */
		else
		{
			if (tree)
			{
				/* We may continue chopping */
#ifdef JP
				msg_format("%sを切っている。", name);
#else
				msg_format("You chop away at the %s.", name);
#endif
				/* Occasional Search XXX XXX */
				if (randint0(100) < 25) search();
			}
			else
			{
				/* We may continue tunelling */
#ifdef JP
				msg_format("%sに穴を掘っている。", name);
#else
				msg_format("You tunnel into the %s.", name);
#endif
			}

			more = TRUE;
		}
	}

	if (is_hidden_door(c_ptr))
	{
		/* Occasional Search XXX XXX */
		if (randint0(100) < 25) search();
	}

	/* Result */
	return more;
}


/*
 * Tunnels through "walls" (including rubble and closed doors)
 *
 * Note that you must tunnel in order to hit invisible monsters
 * in walls, though moving into walls still takes a turn anyway.
 *
 * Digging is very difficult without a "digger" weapon, but can be
 * accomplished by strong players using heavy weapons.
 */
/*:::トンネルを掘る。岩と閉じたドアも含まれる*/
/*:::壁の中にいる見えないモンスターにも自動的に攻撃する*/
void do_cmd_tunnel(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;
	s16b feat;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	/*:::リピート中処理*/
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}
	//九十九姉妹演奏中断
	stop_tsukumo_music();

	/* Get a direction to tunnel, or Abort */
	/*:::掘る方向を入力させる*/
	if (get_rep_dir(&dir,FALSE))
	{
		/* Get location */
		/*:::＠のいる場所からddx[],ddy[]を参照し掘る場所のマスを計算*/
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */

		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		/*:::対象マスのcave[][]から地形データを得る。*/
		/*:::(c_ptr->mimicに数値があるならその数値、なければc_ptr->featの数値のf_info[].mimicを得ている)*/
		feat = get_feat_mimic(c_ptr);

		/* No tunnelling through doors */
/*Q019 （c_ptrからmimic処理をして得られた値からさらにf_info[].mimicを得てその数値のf_info[]を参照している。なんでこんな回りくどい処理を？）*/
		if (have_flag(f_info[feat].flags, FF_DOOR))
		{
			/* Message */
#ifdef JP
			msg_print("ドアは掘れない。");
#else
			msg_print("You cannot tunnel through doors.");
#endif
		}

		/* No tunnelling through most features */
		else if (!have_flag(f_info[feat].flags, FF_TUNNEL))
		{
#ifdef JP
			msg_print("そこは掘れない。");
#else
			msg_print("You can't tunnel through that.");
#endif
		}

		/* A monster is in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Try digging */
		else
		{
			/* Tunnel through walls */
			/*:::実際にトンネルを掘る作業*/
			more = do_cmd_tunnel_aux(y, x);

			//尤魔は追加で満腹度を消費する
			if (p_ptr->pclass == CLASS_YUMA)
			{
				set_food(p_ptr->food - (50 + p_ptr->lev * 2));
			}
		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(0, 0);
}


#ifdef ALLOW_EASY_OPEN /* TNB */

/*
 * easy_open_door --
 *
 *	If there is a jammed/closed/locked door at the given location,
 *	then attempt to unlock/open it. Return TRUE if an attempt was
 *	made (successful or not), otherwise return FALSE.
 *
 *	The code here should be nearly identical to that in
 *	do_cmd_open_test() and do_cmd_open_aux().
 */
bool easy_open_door(int y, int x)
{
	int i, j;

	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Must be a closed door */
	if (!is_closed_door(c_ptr->feat))
	{
		/* Nope */
		return (FALSE);
	}

	//v1.1.68 新地形「氷塊」
	if (have_flag(f_ptr->flags, FF_ICE_WALL))
		return (FALSE);


	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("%sはがっちりと閉じられていたようだ。", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else	msg_format("%sはがっちりと閉じられているようだ。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("The %s used to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else	msg_format("The %s appears to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		if(p_ptr->pclass == CLASS_FLAN)
		{
			msg_format(_("%sに鍵がかかっていたようだ。", "The %s used to be locked."), f_name + f_info[get_feat_mimic(c_ptr)].name);
			cave_alter_feat(y, x, FF_BASH);
		}
		/* Success */
		else if (randint0(100) < j)
		{
			/* Message */
#ifdef JP
			msg_print("鍵をはずした。");
#else
			msg_print("You have picked the lock.");
#endif

			/* Open the door */
			cave_alter_feat(y, x, FF_OPEN);

			/* Sound */
			sound(SOUND_OPENDOOR);

			/* Experience */
			gain_exp(1);
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("鍵をはずせなかった。");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (TRUE);
}

#endif /* ALLOW_EASY_OPEN -- TNB */


/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::箱のトラップを解除する*/
static bool do_cmd_disarm_chest(int y, int x, s16b o_idx)
{
	int i, j;

	bool more = FALSE;

	object_type *o_ptr = &o_list[o_idx];


	/* Take a turn */
	energy_use = 100;

	/* Get the "disarm" factor */
	/*:::解除技能と箱のpvalで成功率を算出*/
	i = p_ptr->skill_dis;

	/* Penalize some conditions */
	if (p_ptr->blind || no_lite()) i = i / 10;
	if (p_ptr->confused || p_ptr->image) i = i / 10;

	/* Extract the difficulty */
	j = i - o_ptr->pval;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	/* Must find the trap first. */
	if (!object_is_known(o_ptr))
	{
#ifdef JP
		msg_print("トラップが見あたらない。");
#else
		msg_print("I don't see any traps.");
#endif

	}

	/* Already disarmed/unlocked */
	else if (o_ptr->pval <= 0)
	{
#ifdef JP
		msg_print("箱にはトラップが仕掛けられていない。");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* No traps to find. */
	else if (!chest_traps[o_ptr->pval])
	{
#ifdef JP
		msg_print("箱にはトラップが仕掛けられていない。");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* Success (get a lot of experience) */
	/*:::通ったらo_ptr->pvalを負にする(罠解除済み)*/
	else if (randint0(100) < j)
	{
#ifdef JP
		msg_print("箱に仕掛けられていたトラップを解除した。");
#else
		msg_print("You have disarmed the chest.");
#endif

		gain_exp(o_ptr->pval);
		o_ptr->pval = (0 - o_ptr->pval);
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* We may keep trying */
		more = TRUE;
		if (flush_failure) flush();
#ifdef JP
		msg_print("箱のトラップ解除に失敗した。");
#else
		msg_print("You failed to disarm the chest.");
#endif

	}

	/* Failure -- Set off the trap */
	/*:::失敗したら、成功率とランダム値で罠発動判定しトラップ作動*/
	else
	{
#ifdef JP
		msg_print("トラップを作動させてしまった！");
#else
		msg_print("You set off a trap!");
#endif

		sound(SOUND_FAIL);
		//v1.1.97 箱トラップ仕様変更
		activate_chest_trap(y, x, o_idx, TRUE);
		//chest_trap(y, x, o_idx);
	}

	/* Result */
	return (more);
}


/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::床上のトラップを解除する。箱のトラップとほぼ同じ。罠の発動は解除されてない状態で移動することで行われる*/
#ifdef ALLOW_EASY_DISARM /* TNB */

bool do_cmd_disarm_aux(int y, int x, int dir)

#else /* ALLOW_EASY_DISARM -- TNB */

static bool do_cmd_disarm_aux(int y, int x, int dir)

#endif /* ALLOW_EASY_DISARM -- TNB */
{
	/* Get grid and contents */
	cave_type *c_ptr = &cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Access trap name */
	cptr name = (f_name + f_ptr->name);

	/* Extract trap "power" */
	int power = f_ptr->power;

	bool more = FALSE;

	/* Get the "disarm" factor */
	int i = p_ptr->skill_dis;

	int j;

	/* Take a turn */
	energy_use = 100;

	/* Penalize some conditions */
	if (p_ptr->blind || no_lite()) i = i / 10;
	if (p_ptr->confused || p_ptr->image) i = i / 10;

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_FLAN && p_ptr->lev > 29)  j=100; //常に成功
	else if(p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		if(randint1(p_ptr->lev) > 10) j = 100;
		else i=0;
	}

	/* Success */
	if (randint0(100) < j)
	{
		/* Message */
#ifdef JP
		if(p_ptr->pclass == CLASS_FLAN || p_ptr->pseikaku == SEIKAKU_BERSERK)
			msg_format("%sを破壊した。", name);
		else if(p_ptr->pclass == CLASS_YUGI)
			msg_format("%sを踏み潰した。", name);
		else
			msg_format("%sを解除した。", name);
#else
		if(p_ptr->pclass == CLASS_FLAN || p_ptr->pseikaku == SEIKAKU_BERSERK)
			msg_format("You have destroyed the %s.", name);
		else if(p_ptr->pclass == CLASS_YUGI)
			msg_format("You have trampled the %s.", name);
		else
			msg_format("You have disarmed the %s.", name);
#endif

		/* Reward */
		gain_exp(power);

		/* Remove the trap */
		cave_alter_feat(y, x, FF_DISARM);

#ifdef ALLOW_EASY_DISARM /* TNB */

		/* Move the player onto the trap */
		move_player(dir, easy_disarm, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

		/* move the player onto the trap grid */
		move_player(dir, FALSE, FALSE, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* Failure */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_format("%sの解除に失敗した。", name);
#else
		msg_format("You failed to disarm the %s.", name);
#endif

		/* We may keep trying */
		more = TRUE;
	}

	/* Failure -- Set off the trap */
	else
	{
		/* Message */
#ifdef JP
		msg_format("%sを作動させてしまった！", name);
#else
		msg_format("You set off the %s!", name);
#endif

#ifdef ALLOW_EASY_DISARM /* TNB */

		/* Move the player onto the trap */
		move_player(dir, easy_disarm, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

		/* Move the player onto the trap */
		move_player(dir, FALSE, FALSE, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */
	}

	/* Result */
	return (more);
}


/*
 * Disarms a trap, or chest
 */
/*:::罠を解除する*/
void do_cmd_disarm(void)
{
	int y, x, dir;

	s16b o_idx;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Option: Pick a direction */
	if (easy_disarm)
	{
		int num_traps, num_chests;

		/* Count visible traps */
		num_traps = count_dt(&y, &x, is_trap, TRUE);

		/* Count chests (trapped) */
		num_chests = count_chests(&y, &x, TRUE);

		/* See if only one target */
		if (num_traps || num_chests)
		{
			bool too_many = (num_traps && num_chests) || (num_traps > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if (get_rep_dir(&dir,TRUE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Check for chests */
		o_idx = chest_check(y, x, TRUE);

		/* Disarm a trap */
		if (!is_trap(feat) && !o_idx)
		{
			/* Message */
#ifdef JP
			msg_print("そこには解除するものが見当たらない。");
#else
			msg_print("You see nothing there to disarm.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx && p_ptr->riding != c_ptr->m_idx)
		{
			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Disarm chest */
		else if (o_idx)
		{
			/* Disarm the chest */
			more = do_cmd_disarm_chest(y, x, o_idx);
		}

		/* Disarm trap */
		else
		{
			/* Disarm the trap */
			more = do_cmd_disarm_aux(y, x, dir);
		}
	}

	/* Cancel repeat unless told not to */
	if (!more) disturb(0, 0);
}


/*
 * Perform the basic "bash" command
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::体当たり ドアの中に敵がいないこと*/
static bool do_cmd_bash_aux(int y, int x, int dir)
{
	/* Get grid */
	cave_type	*c_ptr = &cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Hack -- Bash power based on strength */
	/* (Ranges from 3 to 20 to 100 to 200) */
	int bash = adj_str_blow[p_ptr->stat_ind[A_STR]];

	/* Extract door power */
	int temp = f_ptr->power;

	bool		more = FALSE;

	cptr name = f_name + f_info[get_feat_mimic(c_ptr)].name;

	/* Take a turn */
	energy_use = 100;

	/* Message */
#ifdef JP
	if(p_ptr->pclass != CLASS_YUGI)
		msg_format("%sに体当たりをした！", name);

#else
	if(p_ptr->pclass != CLASS_YUGI)
		msg_format("You smash into the %s!", name);
#endif

	/* Compare bash power to door power XXX XXX XXX */
	temp = (bash - (temp * 10));
	///class 狂戦士はドアを叩き壊しやすい
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) temp *= 2;

	/* Hack -- always have a chance */
	if (temp < 1) temp = 1;

	if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_SAKI) temp = 100; //必ず成功する。

	/* Hack -- attempt to bash down the door */
	if (randint0(100) < temp)
	{

		/* Sound */
		sound(have_flag(f_ptr->flags, FF_GLASS) ? SOUND_GLASS : SOUND_OPENDOOR);

		if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_SAKI)
		{
			msg_format(_("%sを蹴り壊した。", "You kick the %s down."), name);
			cave_alter_feat(y, x, FF_BASH);
		}

		/* Break down the door */
		else if ((randint0(100) < 50) || (feat_state(c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS))
		{
			msg_format(_("%sが壊れた！", "The %s breaks!"), name);
			cave_alter_feat(y, x, FF_BASH);
		}

		/* Open the door */
		else
		{
			msg_format(_("%sが開いた！", "The %s opens!"), name);
			cave_alter_feat(y, x, FF_OPEN);
		}

		/* Hack -- Fall through the door */
		move_player(dir, FALSE, FALSE,FALSE);
	}

	/* Saving throw against stun */
	else if (randint0(100) < adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
		 p_ptr->lev)
	{
		/* Message */
#ifdef JP
		msg_format("この%sは頑丈だ。", name);
#else
		msg_format("The %s holds firm.", name);
#endif


		/* Allow repeated bashing */
		more = TRUE;
	}

	/* High dexterity yields coolness */
	else
	{
		/* Message */
#ifdef JP
		///msg131213
		//msg_print("体のバランスをくずしてしまった。");
		msg_print("ドアノブが脇腹に突き刺さった！");
#else
		msg_print("You get stabbed by the doorknob!");
#endif


		/* Hack -- Lose balance ala paralysis */
		(void)set_paralyzed(p_ptr->paralyzed + 2 + randint0(2));
	}

	/* Result */
	return (more);
}


/*
 * Bash open a door, success based on character strength
 *
 * For a closed door, pval is positive if locked; negative if stuck.
 *
 * For an open door, pval is positive for a broken door.
 *
 * A closed door can be opened - harder if locked. Any door might be
 * bashed open (and thereby broken). Bashing a door is (potentially)
 * faster! You move into the door way. To open a stuck door, it must
 * be bashed. A closed door can be jammed (see do_cmd_spike()).
 *
 * Creatures can also open or bash doors, see elsewhere.
 */
/*:::閉じたドアを破壊する*/
void do_cmd_bash(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		s16b feat;

		/* Bash location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Nothing useful */
		if (!have_flag(f_info[feat].flags, FF_BASH))
		{
			/* Message */
#ifdef JP
			msg_print("そこには体当たりするものが見当たらない。");
#else
			msg_print("You see nothing there to bash.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Bash a closed door */
		else
		{
			/* Bash the door */
			more = do_cmd_bash_aux(y, x, dir);
		}
	}

	/* Unless valid action taken, cancel bash */
	if (!more) disturb(0, 0);
}


/*
 * Manipulate an adjacent grid in some way
 *
 * Attack monsters, tunnel through walls, disarm traps, open doors.
 *
 * Consider confusion XXX XXX XXX
 *
 * This command must always take a turn, to prevent free detection
 * of invisible monsters.
 */
/*:::指定方向に何かする汎用コマンド*/
void do_cmd_alter(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction */
	if (get_rep_dir(&dir,TRUE))
	{
		s16b feat;
		feature_type *f_ptr;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Take a turn */
		energy_use = 100;

		/* Attack monsters */
		if (c_ptr->m_idx)
		{
			/* Attack */
			py_attack(y, x, 0);
		}

		/* Locked doors */
		else if (have_flag(f_ptr->flags, FF_OPEN))
		{
			more = do_cmd_open_aux(y, x);
		}

		/* Bash jammed doors */
		else if (have_flag(f_ptr->flags, FF_BASH))
		{
			more = do_cmd_bash_aux(y, x, dir);
		}

		/* Tunnel through walls */
		else if (have_flag(f_ptr->flags, FF_TUNNEL))
		{
			more = do_cmd_tunnel_aux(y, x);
		}

		/* Close open doors */
		else if (have_flag(f_ptr->flags, FF_CLOSE))
		{
			more = do_cmd_close_aux(y, x);
		}

		/* Disarm traps */
		else if (have_flag(f_ptr->flags, FF_DISARM))
		{
			more = do_cmd_disarm_aux(y, x, dir);
		}

		/* Oops */
		else
		{
			/* Oops */
#ifdef JP
			msg_print("何もない空中を攻撃した。");
#else
			msg_print("You attack the empty air.");
#endif

		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(0, 0);
}


/*
 * Find the index of some "spikes", if possible.
 *
 * XXX XXX XXX Let user choose a pile of spikes, perhaps?
 */
/*:::くさびを探す。jコマンドからのみ呼ばれる*/
///del131223 くさび
///sysdel item くさびを探す
#if 0
static bool get_spike(int *ip)
{
	int i;

	/* Check every item in the pack */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Check the "tval" code */
		if (o_ptr->tval == TV_SPIKE)
		{
			/* Save the spike index */
			(*ip) = i;

			/* Success */
			return (TRUE);
		}
	}

	/* Oops */
	return (FALSE);
}
#endif

/*
 * Jam a closed door with a spike
 *
 * This command may NOT be repeated
 */
/*:::がっちり閉じたドアはpvalが負らしい*/
/*:::ドアにくさびを打つ*/
///sys item ドアに楔を打つ処理　もう削除してしまうことにした
#if 0
void do_cmd_spike(void)
{
	int dir;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		int y, x, item;
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require closed door */
		if (!have_flag(f_info[feat].flags, FF_SPIKE))
		{
			/* Message */
#ifdef JP
			msg_print("そこにはくさびを打てるものが見当たらない。");
#else
			msg_print("You see nothing there to spike.");
#endif

		}

		/* Get a spike */
		else if (!get_spike(&item))
		{
			/* Message */
#ifdef JP
			msg_print("くさびを持っていない！");
#else
			msg_print("You have no spikes!");
#endif
		}

		/* Is a monster in the way? */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Go for it */
		else
		{
			/* Take a turn */
			energy_use = 100;

			/* Successful jamming */
#ifdef JP
			msg_format("%sにくさびを打ち込んだ。", f_name + f_info[feat].name);
#else
			msg_format("You jam the %s with a spike.", f_name + f_info[feat].name);
#endif

			cave_alter_feat(y, x, FF_SPIKE);

			/* Use up, and describe, a single spike, from the bottom */
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
	}
}
#endif


/*
 * Support code for the "Walk" and "Jump" commands
 */
/*:::＠が歩く pickup:物を拾う*/
void do_cmd_walk(bool pickup)
{
	int dir;

	bool more = FALSE;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		/* Take a turn */
		energy_use = 100;

		if ((dir != 5) && (p_ptr->special_defense & KATA_MUSOU))
		{
			set_action(ACTION_NONE);
		}

		//依姫がその場に待機しているとき祇園様のリセットフラグが消える
		if(p_ptr->pclass == CLASS_YORIHIME && dir == 5 && (p_ptr->kamioroshi & KAMIOROSHI_GION))
		{
			reset_concent = FALSE;
		}

		///sys 全体マップで移動したときのターン消費
		///mod140814 文は全体マップ移動が速い
		/* Hack -- In small scale wilderness it takes MUCH more time to move */
		if (p_ptr->wild_mode)
		{
			energy_use *= ((MAX_HGT + MAX_WID) / 2);
			if(p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_MARISA || p_ptr->pclass == CLASS_SAKI) energy_use /= 2;
			if(CHECK_FAIRY_TYPE == 22) energy_use /= 2;
		}

		//v2.0.3 高速移動などのenergy_use値調整を別関数に分離
		/*
		///sys 早駆けのターン消費
		if (p_ptr->action == ACTION_HAYAGAKE) energy_use = energy_use * (45-(p_ptr->lev/2)) / 100;
		///mod150105 村紗水中高速移動
		else if(p_ptr->pclass == CLASS_MURASA)
		{
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if(have_flag(f_ptr->flags, FF_WATER)) energy_use /= 3;
		}
		///mod131228 高速移動のターン消費
		else if(p_ptr->speedster) energy_use = energy_use * (75-p_ptr->lev/2) / 100;
		///mod131228 スライム変化時のターン消費
		else if(p_ptr->mimic_form == MIMIC_SLIME && !p_ptr->levitation) energy_use = energy_use * 3 / 2;
		///mod140907 人魚のターン消費
		///mod150321 乗馬時は無関係
		else if((prace_is_(RACE_NINGYO) || p_ptr->muta3 & MUT3_FISH_TAIL) && !p_ptr->levitation && !p_ptr->riding)
		{
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if(!have_flag(f_ptr->flags, FF_WATER)) energy_use = energy_use * 4 / 3;
		}
		*/
		walk_energy_modify();



		/* Actually move the character */
		move_player(dir, pickup, FALSE,FALSE);

		/* Allow more walking */
		more = TRUE;
	}

	/* Hack again -- Is there a special encounter ??? */
	/*:::荒野移動時の襲撃判定*/
	///sys 荒野襲撃判定
	///mod140814 文は襲撃を受けない
	//魔理沙も追加
	//時間停止中咲夜も追加
	if (p_ptr->wild_mode && !cave_have_flag_bold(py, px, FF_TOWN) && (p_ptr->pclass != CLASS_AYA) && (p_ptr->pclass != CLASS_MARISA) && (p_ptr->pclass != CLASS_SAKI) && !SAKUYA_WORLD)
	{
		bool flag_encounter = FALSE;

		int tmp = 120 + p_ptr->lev*10 - wilderness[py][px].level + 5;
		if (tmp < 1) tmp = 1;

		//迷いの竹林を徒歩で移動していると襲撃を受けやすい
		if(!p_ptr->levitation && distance(py,px,55,46) < 6)
		{
			tmp = randint1(tmp);
			//msg_print("chk:徒歩襲撃率上昇");
		}

		if (((wilderness[py][px].level + 5) > (p_ptr->lev / 2)) && randint0(tmp) < (21 - p_ptr->skill_stl))
			flag_encounter = TRUE;

		//三途の河
		if (wilderness[py][px].level >= 40 && wilderness[py][px].terrain == TERRAIN_DEEP_WATER)
		{
			//msg_print("sans");
			//種族死神とか神様は襲われないがそれ以外は超高確率で襲われる
			if (p_ptr->prace == RACE_DEATH
				|| p_ptr->prace == RACE_DEITY
				|| p_ptr->prace == RACE_STRAYGOD)
				flag_encounter = FALSE;
			else if(randint0(10) < (26 - p_ptr->skill_stl))
				flag_encounter = TRUE;

		}



		if(flag_encounter)
		{
			/* Inform the player of his horrible fate :=) */
#ifdef JP
			msg_print("襲撃だ！");
#else
			msg_print("You are ambushed !");
#endif

			/* Go into large wilderness view */
			p_ptr->oldpy = randint1(MAX_HGT-2);
			p_ptr->oldpx = randint1(MAX_WID-2);
			change_wild_mode();

			/* Give first move to monsters */
			energy_use = 100;

			/* HACk -- set the encouter flag for the wilderness generation */
			generate_encounter = TRUE;
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}



/*
 * Start running.
 */
void do_cmd_run(void)
{
	int dir;

	/* Hack -- no running when confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて走れない！");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		/* Hack -- Set the run counter */
		running = (command_arg ? command_arg : 1000);

		/* First step */
		run_step(dir);
	}
}



/*
 * Stay still.  Search.  Enter stores.
 * Pick up treasure if "pickup" is true.
 */
void do_cmd_stay(bool pickup)
{
	u32b mpe_mode = MPE_STAYING | MPE_ENERGY_USE;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	break_eibon_flag = FALSE;

	if (pickup) mpe_mode |= MPE_DO_PICKUP;
	(void)move_player_effect(py, px, mpe_mode);
}



/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
void do_cmd_rest(void)
{

	set_action(ACTION_NONE);
	///class 吟遊詩人と呪術ハイは休憩中止める
	//if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] || p_ptr->magic_num1[1]))
///mod140817 歌関係に吟遊詩人以外の職も便乗
	if ((CHECK_MUSIC_CLASS) && (p_ptr->magic_num1[0] || p_ptr->magic_num1[1]))
	{

		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	/* Hex */
	if (hex_spelling_any()) stop_hex_spell_all();

	/* Prompt for time if needed */
	if (command_arg <= 0)
	{
#ifdef JP
		cptr p = "休憩 (0-9999, '*' で HP/MP全快, '&' で必要なだけ): ";
#else
		cptr p = "Rest (0-9999, '*' for HP/SP, '&' as needed): ";
#endif


		char out_val[80];

		/* Default */
		strcpy(out_val, "&");

		/* Ask for duration */
		if (!get_string(p, out_val, 4)) return;

		/* Rest until done */
		if (out_val[0] == '&')
		{
			command_arg = (-2);
		}

		/* Rest a lot */
		else if (out_val[0] == '*')
		{
			command_arg = (-1);
		}

		/* Rest some */
		else
		{
			command_arg = atoi(out_val);
			if (command_arg <= 0) return;
		}
	}


	/* Paranoia */
	if (command_arg > 9999) command_arg = 9999;

	///class 忍者は休憩中超隠密を解除
	if (p_ptr->special_defense & NINJA_S_STEALTH) set_superstealth(FALSE);

	/* Take a turn XXX XXX XXX (?) */
	energy_use = 100;


	/* The sin of sloth */
	///del131209 virtue
	/*
	if (command_arg > 100)
		chg_virtue(V_DILIGENCE, -1);
	*/

	/* Why are you sleeping when there's no need?  WAKE UP!*/
/*
	if ((p_ptr->chp == p_ptr->mhp) &&
	    (p_ptr->csp == p_ptr->msp) &&
	    !p_ptr->blind && !p_ptr->confused &&
	    !p_ptr->poisoned && !p_ptr->afraid &&
	    !p_ptr->stun && !p_ptr->cut &&
	    !p_ptr->slow && !p_ptr->paralyzed &&
	    !p_ptr->image && !p_ptr->word_recall &&
	    !p_ptr->alter_reality)
			chg_virtue(V_DILIGENCE, -1);
*/

	if(p_ptr->pclass == CLASS_MEIRIN)
	{
		if ((p_ptr->chp == p_ptr->mhp) &&
	    (p_ptr->csp == p_ptr->msp) &&
	    !p_ptr->blind && !p_ptr->confused &&
	    !p_ptr->poisoned && !p_ptr->afraid &&
	    !p_ptr->stun && !p_ptr->cut &&
	    !p_ptr->slow && !p_ptr->paralyzed &&
	    !p_ptr->image )
		 msg_print(_("あなたは居眠りを始めた・・", "You start to fall asleep..."));
		else msg_print(_("あなたは回復に集中した・・", "You start recovering..."));
	}

	break_eibon_flag = FALSE;

	/* Save the rest code */
	resting = command_arg;
	p_ptr->action = ACTION_REST;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Refresh */
	Term_fresh();

}


/*
 * Determines the odds of an object breaking when thrown at a monster
 *
 * Note that artifacts never break, see the "drop_near()" function.
 */
/*:::矢と投擲物の破壊判定　アーチャーは壊れにくい*/
///class item TVAL 矢と投擲物の破壊判定
int breakage_chance(object_type *o_ptr)
{
	int archer_bonus = (p_ptr->pclass == CLASS_ARCHER ? (p_ptr->lev-1)/7 + 4: 0);

	/* Examine the snipe type */
	if (snipe_type)
	{
		if (snipe_type == SP_KILL_WALL) return (100);
		if (snipe_type == SP_EXPLODE) return (100);
		if (snipe_type == SP_PIERCE) return (100);
		if (snipe_type == SP_FINAL) return (100);
		if (snipe_type == SP_NEEDLE) return (100);
		if (snipe_type == SP_EVILNESS) return (40);
		if (snipe_type == SP_HOLYNESS) return (40);
	}

	/* Examine the item type */

	switch (o_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
			if(o_ptr->sval == SV_FLASK_OIL) return (100);
			else return (10);
		case TV_POTION:
		//case TV_BOTTLE:
		case TV_FOOD:
		case TV_MUSHROOM:
		case TV_BULLET:
		//case TV_JUNK:
			return (100);

		/* Often break */
		case TV_LITE:
		case TV_SCROLL:
		//case TV_SKELETON:
			return (50);

		/* Sometimes break */
		case TV_WAND:
		//case TV_SPIKE:
			return (25);
		case TV_ARROW:
			return (20 - archer_bonus * 2);

			//合成薬　蓬莱の薬は割れない
		case TV_COMPOUND:
			if(o_ptr->sval == SV_COMPOUND_HOURAI) return 0;
			else return 100;

		/* Rarely break */
		//case TV_SHOT:
		case TV_BOLT:
			return (10 - archer_bonus);

		case TV_OTHERWEAPON:
			return 15;

		default:
			if(p_ptr->pclass == CLASS_NINJA && object_is_melee_weapon(o_ptr)) return (8 - p_ptr->lev / 7);
			else if(p_ptr->pclass == CLASS_SAKUYA && object_is_melee_weapon(o_ptr)) return (8 - p_ptr->lev / 9);
			///mod150918　悪夢の欠片はかならず壊れる
			else if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_NIGHTMARE_FRAGMENT) return (100);
			else return (10);
	}
}

/*:::射撃のダメージ計算　ベースダメージにスレイ、属性、理力を計算する　というか理力があるらしい*/
/*:::近接武器とは違ってダイスだけでなくダメージ全てに倍率がかかるらしい。*/
///sys item ego mon flg 射撃命中時の矢のスレイ計算など
static s16b tot_dam_aux_shot(object_type *o_ptr, int tdam, monster_type *m_ptr)
{
	int mult = 10;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		///mod131223
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Slay Animal */
			if ((have_flag(flgs, TR_SLAY_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 17) mult = 17;
			}

			/* Kill Animal */
			if ((have_flag(flgs, TR_KILL_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 27) mult = 27;
			}

			/* Slay Evil */
			if ((have_flag(flgs, TR_SLAY_EVIL)) &&
			    (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_CHAOS;
				}

				if (mult < 15) mult = 15;
			}

			/* Kill Evil */
			if ((have_flag(flgs, TR_KILL_EVIL)) &&
			    (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_CHAOS;
				}

				if (mult < 25) mult = 25;
			}
			//mod160630 秩序スレイ追加
			if ((have_flag(flgs, TR_SLAY_GOOD)) &&
			    (r_ptr->flags3 & RF3_ANG_COSMOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_COSMOS;
				}

				if (mult < 15) mult = 15;
			}

			if ((have_flag(flgs, TR_KILL_GOOD)) &&
			    (r_ptr->flags3 & RF3_ANG_COSMOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_COSMOS;
				}

				if (mult < 25) mult = 25;
			}

			/* Slay Human */


///mod131231 モンスターフラグ変更
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (r_ptr->flags3 & RF3_HUMAN))
//			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_HUMAN;
//					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 17) mult = 17;
			}

			/* Kill Human */
			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (r_ptr->flags2 & RF3_HUMAN))
//			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_HUMAN;
//					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 27) mult = 27;
			}

			/* Slay Undead */
			if ((have_flag(flgs, TR_SLAY_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 20) mult = 20;
			}

			/* Kill Undead */
			if ((have_flag(flgs, TR_KILL_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 30) mult = 30;
			}

			/* Slay Demon */
			if ((have_flag(flgs, TR_SLAY_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 20) mult = 20;
			}

			/* Kill Demon */
			if ((have_flag(flgs, TR_KILL_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 30) mult = 30;
			}

///mod131231 モンスターフラグ変更
			if ((have_flag(flgs, TR_SLAY_KWAI)) &&
			    (r_ptr->flags3 & RF3_KWAI))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_KWAI;
				}

				if (mult < 20) mult = 20;
			}
			if ((have_flag(flgs, TR_KILL_KWAI)) &&
			    (r_ptr->flags3 & RF3_KWAI))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_KWAI;
				}

				if (mult < 30) mult = 30;
			}


			if ((have_flag(flgs, TR_SLAY_DEITY)) &&
			    (r_ptr->flags3 & RF3_DEITY))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEITY;
				}

				if (mult < 20) mult = 20;
			}

			if ((have_flag(flgs, TR_KILL_DEITY)) &&
			    (r_ptr->flags3 & RF3_DEITY))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEITY;
				}

				if (mult < 30) mult = 30;
			}

			//DEMIHUMANには人スレイが少し効くようにした
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (r_ptr->flags3 & RF3_DEMIHUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMIHUMAN;
				}

				if (mult < 15) mult = 15;
			}

			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (r_ptr->flags3 & RF3_DEMIHUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMIHUMAN;
				}

				if (mult < 25) mult = 25;
			}

			/* Slay Dragon  */
			if ((have_flag(flgs, TR_SLAY_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 20) mult = 20;
			}

			/* Execute Dragon */
			if ((have_flag(flgs, TR_KILL_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 30) mult = 30;

				//スマウグにバルド矢を当てると大ダメージになる特殊処理
				if ((o_ptr->name1 == ART_BARD_ARROW) &&
				    (m_ptr->r_idx == MON_SMAUG) &&
				//    (inventory[INVEN_BOW].name1 == ART_BARD))
				    (inventory[INVEN_RARM].name1 == ART_BARD || inventory[INVEN_LARM].name1 == ART_BARD))
					mult *= 5;

			}
			/*:::源頼政の弓で射撃するとぬえに大ダメージ*/
			if ( (m_ptr->r_idx == MON_NUE) && (inventory[INVEN_RARM].name1 == ART_YORIMASA || inventory[INVEN_LARM].name1 == ART_YORIMASA))
			{
				char mname[256];
				monster_desc(mname, m_ptr, 0);

				//v1.1.83 正体不明解除するようにしてみる
				mult *= 3;
				if (m_ptr->mflag2 & MFLAG2_KAGE)
				{
					m_ptr->mflag2 &= ~(MFLAG2_KAGE);
					m_ptr->ap_r_idx = m_ptr->r_idx;
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
				msg_format(_("%sは弱点を突かれて悲鳴を上げた！",
                            "%s lets out a scream!"), mname);

			}

			/* Brand (Acid) */
			if (have_flag(flgs, TR_BRAND_ACID))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Elec) */
			if (have_flag(flgs, TR_BRAND_ELEC))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Fire) */
			if (have_flag(flgs, TR_BRAND_FIRE))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
///mod131231 モンスターフラグ変更 火炎弱点RF3からRFRへ
					if (r_ptr->flagsr & RFR_HURT_FIRE)
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(m_ptr))
						{
							r_ptr->r_flagsr |= RFR_HURT_FIRE;
						}
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Cold) */
			if (have_flag(flgs, TR_BRAND_COLD))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
					}
				}
				/* Otherwise, take the damage */
				else
				{
///mod131231 モンスターフラグ変更 冷気弱点RF3からRFRへ

					if (r_ptr->flagsr & RFR_HURT_COLD)
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(m_ptr))
						{
							r_ptr->r_flagsr |= RFR_HURT_COLD;
						}
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Poison) */
			if (have_flag(flgs, TR_BRAND_POIS))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}
			///mod141018 矢玉の理力処理　ダメージ全てを2.5倍
			//消費MPをMAXの1/30から10で固定に修正
			//if ((have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (p_ptr->msp / 30)))
			if ((have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp >= 10))
			{
				//p_ptr->csp -= (1+(p_ptr->msp / 30));
				p_ptr->csp -= 10;
				p_ptr->redraw |= (PR_MANA);
				mult = mult * 5 / 2;
			}
			break;
		}
	}

	/* Sniper */
	///mod141116 条件式追加
	//v1.1.74 スナイパー条件削除
	//if (p_ptr->pclass == CLASS_SNIPER && snipe_type) mult = tot_dam_aux_snipe(mult, m_ptr);
	if (snipe_type) mult = tot_dam_aux_snipe(mult, m_ptr);

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 *
 * See "calc_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a monster.
 *:::射撃した矢玉は壊れやすい
 *
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 *
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *:::追加射撃の効果は使用行動ターンの減少で実現
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *:::弓の倍率は全てのダメージを倍加させる？
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 *:::強力射は射程も増えるらしい
 *
 * Note that Bows of "Extra Shots" give an extra shot.
 */
/*:::射撃処理　弓と矢は選択済み
 *:::item:選択した矢玉のインベントリ/床上インデックス
 *:::j_ptr:装備中の弓のobject_type*/
void do_cmd_fire_aux(int item, object_type *j_ptr)
{
	int dir;
	int i, j, y, x, ny, nx, ty, tx, prev_y, prev_x;
	int tdam_base, tdis, thits, tmul;
	int bonus, chance;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	bool hit_body = FALSE;

	char o_name[MAX_NLEN];

	u16b path_g[512];	/* For calcuration of path length */

	int msec = delay_factor * delay_factor * delay_factor;

	/* STICK TO */
	bool stick_to = FALSE;

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Sniper - Cannot shot a single arrow twice */
	/*:::ダブルショットで矢が一本しかなかったら通常射撃になる*/
	if ((snipe_type == SP_DOUBLE) && (o_ptr->number < 2)) snipe_type = SP_NONE;

	/* Describe the object */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Use the proper number of shots */
	/*:::現在の射撃回数(*100)*/
	thits = p_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam_base = damroll(o_ptr->dd, o_ptr->ds) + o_ptr->to_d + j_ptr->to_d;

	/* Actually "fire" the object */
	/*:::命中率計算　クロスボウは当たりやすい*/
	bonus = (p_ptr->to_h_b + o_ptr->to_h + j_ptr->to_h);


	///mod131227 skill 弓の命中率計算 新武器技能値適用
	if (j_ptr->tval == TV_CROSSBOW)
		chance = (p_ptr->skill_thb + (ref_skill_exp(TV_CROSSBOW) / 400 + bonus) * BTH_PLUS_ADJ);
	else
		chance = (p_ptr->skill_thb + ((ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200 + bonus) * BTH_PLUS_ADJ);
	/* v1.1.71 弓やクロスボウの武器熟練度はここで計算しているが近接武器の熟練度はcalc_bonuses()で技能値p_ptr->to_h[]に算入している。
	　 スッキリしないしC画面に武器技能を反映する邪魔になるのでこの計算もcalc_bonuses()で技能値p_ptr->t_h_bにまとめようと思ったが、
	  調べてみると投擲の命中率にもp_ptr->t_h_bが使われていたので断念。そういえば数年前にも気になりつつ放置してたんだった。
	*/


	///mod140928 恐怖状態だと命中率減少
	if(p_ptr->afraid) chance = chance * 2 / 3;

	/*:::弓の種類による基本射撃速度　まだ最終値ではない*/
	energy_use = bow_energy(j_ptr->tval, j_ptr->sval);
	/*:::弓の種類による基本倍率　これもまだ最終値ではない*/
	tmul = bow_tmul(j_ptr->tval, j_ptr->sval);

	/* Get extra "power" from "extra might" */
	if (p_ptr->xtra_might) tmul++;

	/*:::adj_str_tdは126-148*/
	tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

	/* Boost the damage */
	tdam_base *= tmul;
	tdam_base /= 100;

	/* Base range */

	tdis = 13 + tmul/80;
	///item sys 弓TVAL変更関連
	///mod131223 tval
	//if ((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
	if (j_ptr->tval == TV_CROSSBOW)
	{
		if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent)
			tdis -= (5 - (p_ptr->concent + 1) / 2);
		else
			tdis -= 5;
	}
	/*:::弓の射程は最高20マス　クロスボウは射程が短いがx5弓でスナイパーが集中すれば20マスになる*/
	project_length = tdis + 1;

	/* Get a direction (or cancel) */
	/*:::ターゲットを選択する。キャンセルしたら行動消費とスナイパーの特殊射撃をリセット*/
	if (!get_aim_dir(&dir))
	{
		energy_use = 0;

		if (snipe_type == SP_AWAY) snipe_type = SP_NONE;

		/* need not to reset project_length (already did)*/

		return;
	}

	/* Predict the "target" location */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	//「咲夜の世界」
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Get projection path length */
	tdis = project_path(path_g, project_length, py, px, ty, tx, PROJECT_PATH|PROJECT_THRU) - 1;

	project_length = 0; /* reset to default */

	/* Don't shoot at my feet */
	/*:::スナイパーのシュート＆アウェイが発動するかと思ったがそうでもなかった。is_firedがONにならないためか*/
	if (tx == px && ty == py)
	{
		energy_use = 0;

		/* project_length is already reset to 0 */

		return;
	}


	/* Take a (partial) turn */
	/*:::消費エネルギー最終版*/
	energy_use = (energy_use / thits);
	is_fired = TRUE;

	/* Sniper - Difficult to shot twice at 1 turn */
	if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_DOUBLE)  p_ptr->concent = (p_ptr->concent + 1) / 2;

	/*:::矢が飛ぶ処理*/
	/* Sniper - Repeat shooting when double shots */
	for (i = 0; i < ((snipe_type == SP_DOUBLE) ? 2 : 1); i++)
	{

	/* Start at the player */
	y = py;
	x = px;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Single object */
	q_ptr->number = 1;

	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Sound */
	sound(SOUND_SHOOT);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* The shot does not hit yet */
	hit_body = FALSE;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{


		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, py, px, ty, tx);

		/* Shatter Arrow */
		if (snipe_type == SP_KILL_WALL)
		{
			cave_type *c_ptr = &cave[ny][nx];

			if (cave_have_flag_grid(c_ptr, FF_HURT_ROCK) && !c_ptr->m_idx)
			{
#ifdef JP
				if (c_ptr->info & (CAVE_MARK)) msg_print("岩が砕け散った。");
#else
				if (c_ptr->info & (CAVE_MARK)) msg_print("Wall rocks were shattered.");
#endif
				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

				/* Destroy the wall */
				cave_alter_feat(ny, nx, FF_HURT_ROCK);

				hit_body = TRUE;
				break;
			}
		}

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && !cave[ny][nx].m_idx) break;

		/* Advance the distance */
		cur_dis++;

		/* Sniper */
		if (snipe_type == SP_LITE)
		{
			cave[ny][nx].info |= (CAVE_GLOW);

			/* Notice */
			note_spot(ny, nx);

			/* Redraw */
			lite_spot(ny, nx);
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny, nx) && player_can_see_bold(ny, nx))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny, nx);
			move_cursor_relative(ny, nx);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(ny, nx);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Sniper */
		if (snipe_type == SP_KILL_TRAP)
		{
			project(0, 0, ny, nx, 0, GF_KILL_TRAP,
				(PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		/* Sniper */
		if (snipe_type == SP_EVILNESS)
		{
			cave[ny][nx].info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Notice */
			note_spot(ny, nx);

			/* Redraw */
			lite_spot(ny, nx);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx;
		y = ny;


		/* Monster here, Try to hit it */
		/*:::モンスターが居たら当たる処理、徳変化、武器熟練度、乗馬熟練度処理など*/
		if (cave[y][x].m_idx)
		{
			int armour;
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;
///del131214 virtue
/*
			if (MON_CSLEEP(m_ptr))
			{
				if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
				if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
			}
*/
			///sys 射撃時の武器経験値処理
			///mod131227 skill tval弓の経験値獲得 新武器技能値適用
			gain_skill_exp(j_ptr->tval,m_ptr);
			/*
			if ((r_ptr->level + 10) > p_ptr->lev)
			{

				int now_exp = p_ptr->weapon_exp[0][j_ptr->sval];
				if (now_exp < s_info[p_ptr->pclass].w_max[0][j_ptr->sval])
				{
					int amount = 0;
					if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
					else if (now_exp < WEAPON_EXP_SKILLED) amount = 25;
					else if ((now_exp < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19)) amount = 10;
					else if (p_ptr->lev > 34) amount = 2;
					p_ptr->weapon_exp[0][j_ptr->sval] += amount;
					p_ptr->update |= (PU_BONUS);
				}
			}
			*/

			///sys 射撃時の乗馬経験値処理
			if (p_ptr->riding)
			{
				///mod131226 skill 技能と武器技能の統合
				gain_skill_exp(SKILL_RIDING,m_ptr);
#if 0
				if ((p_ptr->skill_exp[GINOU_RIDING] < s_info[p_ptr->pclass].s_max[GINOU_RIDING])
					&& ((p_ptr->skill_exp[GINOU_RIDING] - (RIDING_EXP_BEGINNER * 2)) / 200 < r_info[m_list[p_ptr->riding].r_idx].level)
					&& one_in_(2))
				{
					p_ptr->skill_exp[GINOU_RIDING] += 1;
					p_ptr->update |= (PU_BONUS);
				}
#endif

			}
			/* Some shots have hit bonus */
			armour = r_ptr->ac;

			//v1.1.94 モンスター防御力低下中はAC25%カット
			if (MON_DEC_DEF(m_ptr))
			{
				armour = MONSTER_DECREASED_AC(armour);

			}

			/*
			if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent)
			{
				armour *= (10 - p_ptr->concent);
				armour /= 10;
			}
			*/

			//v1.1.74 セイントスターアローを発動効果として復活したがconcentがないので命中率を少し補正
			if (snipe_type == SP_FINAL)
			{
				armour /= 2;
			}

			/* Did we hit it (penalize range) */
			/*:::命中判定*/
			if (test_hit_fire(chance - cur_dis, armour, m_ptr->ml))
			{
				bool fear = FALSE;
				int tdam = tdam_base;

				/* Get extra damage from concentration */
				if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent) tdam = boost_concentration_damage(tdam);

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
#ifdef JP
					msg_format("%sが敵を捕捉した。", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%sが%sに命中した。", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this monster race */
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

						/* Hack -- Track this monster */
						health_track(c_ptr->m_idx);
					}
				}

				if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_NEEDLE)
				{
					if ((randint1(randint1(r_ptr->level / (3 + p_ptr->concent)) + (8 - p_ptr->concent)) == 1)
						&& !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
					{
						char m_name[80];

						/* Get "the monster" or "it" */
						monster_desc(m_name, m_ptr, 0);

						tdam = m_ptr->hp + 1;
#ifdef JP
						msg_format("%sの急所に突き刺さった！", m_name);
#else
						msg_format("Your shot sticked on a critical spot of %s!", m_name);
#endif
					}
					else tdam = 1;
				}
				else
				{
					/*:::通常の命中ダメージ処理*/
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux_shot(q_ptr, tdam, m_ptr);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					/* Modify the damage */
					tdam = mon_damage_mod(m_ptr, tdam, FALSE);
				}

				/* Complex message */
				///sys 射撃のダメージ表示
				if (p_ptr->wizard || cheat_xtra)
				{
#ifdef JP
					msg_format("%d/%d のダメージを与えた。",
						   tdam, m_ptr->hp);
#else
					msg_format("You do %d (out of %d) damage.",
						   tdam, m_ptr->hp);
#endif

				}

				/* Sniper */
				if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_EXPLODE)
				{
					u16b flg = (PROJECT_STOP | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID);

					sound(SOUND_EXPLODE); /* No explode sound - use breath fire instead */
					project(0, ((p_ptr->concent + 1) / 2 + 1), ny, nx, tdam, GF_MISSILE, flg, -1);
					break;
				}

				/* Sniper */
				if (snipe_type == SP_HOLYNESS)
				{
					cave[ny][nx].info |= (CAVE_GLOW);

					/* Notice */
					note_spot(ny, nx);

					/* Redraw */
					lite_spot(ny, nx);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					/* STICK TO */
					/*:::アーティファクトの矢は刺さる*/
					if (object_is_fixed_artifact(q_ptr))
					{

						stick_to = TRUE;
#ifdef JP
						msg_format("%sは%sに突き刺さった！",o_name, m_name);
#else
						msg_format("%^s have stuck into %s!",o_name, m_name);
#endif
					}

					/* Message */
					message_pain(c_ptr->m_idx, tdam);

					/* Anger the monster */
					if (tdam > 0) anger_monster(m_ptr);

					///mod141018　特殊処理　矢に鍛冶師技能で|沌が付いていたら混乱の手と同じ効果
					{
						u32b flgs[TR_FLAG_SIZE];
						object_flags(q_ptr, flgs);
						if(have_flag(flgs,TR_CHAOTIC) && !(r_ptr->flags3 & RF3_NO_CONF) && !(r_ptr->flagsr & RFR_RES_CHAO) && (randint0(100) > r_ptr->level))
						{
							msg_format(_("%^sは混乱したようだ。", "%^s is confused."), m_name);
							(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
						}
					}

					/* Take note */
					if (fear && m_ptr->ml)
					{

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the monster name (or "it") */
						monster_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^sは恐怖して逃げ出した！", m_name);
#else
						msg_format("%^s flees in terror!", m_name);
#endif

					}

					set_target(m_ptr, py, px);

					/* Sniper */
					if (snipe_type == SP_RUSH)
					{
						int n = randint1(5) + 3;
						int m_idx = c_ptr->m_idx;

						for ( ; cur_dis <= tdis; )
						{
							int ox = nx;
							int oy = ny;

							if (!n) break;

							/* Calculate the new location (see "project()") */
							mmove2(&ny, &nx, py, px, ty, tx);

							/* Stopped by wilderness boundary */
							if (!in_bounds2(ny, nx)) break;

							/* Stopped by walls/doors */
							if (!player_can_enter(cave[ny][nx].feat, 0)) break;

							/* Stopped by monsters */
							if (!cave_empty_bold(ny, nx)) break;

							cave[ny][nx].m_idx = m_idx;
							cave[oy][ox].m_idx = 0;

							m_ptr->fx = nx;
							m_ptr->fy = ny;

							/* Update the monster (new location) */
							update_mon(c_ptr->m_idx, TRUE);

							lite_spot(ny, nx);
							lite_spot(oy, ox);

							Term_fresh();
							Term_xtra(TERM_XTRA_DELAY, msec);

							x = nx;
							y = ny;
							cur_dis++;
							n--;
						}
					}
				}
			}

			/* Sniper */
			/*:::貫通処理*/
			if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_PIERCE)
			{
				if(p_ptr->concent < 1) break;
				p_ptr->concent--;
				continue;
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	/*:::矢玉の破壊率計算*/
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	if (stick_to)
	{
		int m_idx = cave[y][x].m_idx;
		monster_type *m_ptr = &m_list[m_idx];
		int o_idx = o_pop();

		/*:::矢が刺さったはずがすでにフロアのアイテムが一杯の時？*/
		if (!o_idx)
		{
#ifdef JP
			msg_format("%sはどこかへ行った。", o_name);
#else
			msg_format("The %s have gone to somewhere.", o_name);
#endif
			if (object_is_fixed_artifact(q_ptr))
			{
				a_info[j_ptr->name1].cur_num = 0;
			}
			return;
		}

		o_ptr = &o_list[o_idx];
		object_copy(o_ptr, q_ptr);

		/* Forget mark */
		o_ptr->marked &= OM_TOUCHED;

		/* Forget location */
		o_ptr->iy = o_ptr->ix = 0;

		/* Memorize monster */
		o_ptr->held_m_idx = m_idx;

		/* Build a stack */
		o_ptr->next_o_idx = m_ptr->hold_o_idx;

		/* Carry object */
		m_ptr->hold_o_idx = o_idx;
	}
	else if (cave_have_flag_bold(y, x, FF_PROJECT))
	{
		/* Drop (or break) near that location */
		(void)drop_near(q_ptr, j, y, x);
	}
	else
	{
		/* Drop (or break) near that location */
		(void)drop_near(q_ptr, j, prev_y, prev_x);
	}

	/* Sniper - Repeat shooting when double shots */
	}

	/* Sniper - Loose his/her concentration after any shot */
	//if (p_ptr->concent) reset_concentration(FALSE);
	///mod141116 判定式変更
	if(CLASS_USE_CONCENT)
	{
		reset_concentration(FALSE);

	}

}

///mod160508
//銃の必要充填時間基本値を計算する。熟練度などに関係ないアイテム自体の値で発動インデックスにより変化する
int calc_gun_timeout(object_type *o_ptr)
{
	const activation_type* const act_ptr = find_activation_info(o_ptr);
	int timeout;
	if(o_ptr->tval != TV_GUN){msg_print(_("ERROR:銃以外でcalc_gun_timeout()が呼ばれた",
                                        "ERROR: calc_gun_timeout() called for non-gun"));return 0;}
	if(!act_ptr){msg_print(_("ERROR:calc_gun_timeout()で扱われた発動インデックスがおかしい",
                            "ERROR: Weird activation index in calc_gun_timeout()"));return 0;}

	//GUN_VARIABLE(銃の可変特殊発動ルーチン)の場合sval値に応じた値を得る
	if(act_ptr->index == ACT_GUN_VARIABLE)
	{
		timeout = gun_base_param_table[o_ptr->sval].charge_turn;
	}
	//それ以外の発動の場合、発動テーブルからタイムアウト値(ベース部分のみ)を得る
	else
	{
		timeout = act_ptr->timeout.constant;
	}

	return timeout;

}

//銃による射撃の実行処理　基本的にAコマンドによる発動と同じだが高級品、特別製、清蘭の発動内容変更に対応するため基本パラメータを最初に一括処理する
void do_cmd_fire_gun_aux(int slot, int dir)
{
	int plev = p_ptr->lev;

	int dice=0,sides=0,base=0;
	int typ = GF_MISSILE;
	int mode = GUN_FIRE_MODE_DEFAULT;
	int rad=0;
	int dam;
	int timeout_base, timeout_max;
	char o_name[160];
	object_type *o_ptr = &inventory[slot];
	int chance;

	cptr shot_msg = NULL; //射撃時メッセージ

	const activation_type* const act_ptr = find_activation_info(o_ptr);

	if(o_ptr->tval != TV_GUN){msg_print(_("ERROR:銃以外でactivate_gun()が呼ばれた",
                                        "ERROR: activate_gun() called for a non-gun"));return;}

	if(!act_ptr->index)
	{
		msg_print(_("この銃は撃てないようだ。", "Doesn't look like this gun can shoot."));
		return;
	}





	/*::: -Mega Hack- 銃のタイムアウト値は1000倍にし、十倍の頻度で充填ルーチンを呼び、そのたびに100減らす。
	 *::: さらに減らす数値を高速射撃や銃熟練度に応じて増加させる。*/

	//射撃一発ごとのタイムアウト増加値
	timeout_base = calc_gun_timeout(o_ptr) * 1000;
	//射撃可能なタイムアウト上限値　これを越えると弾切れ
	timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
	if(o_ptr->timeout > timeout_max)
	{
		msg_print(_("装弾が済んでいない。",
                    "It's still reloading.")); //ここの上の関数で判定は済ませておいたが、ほかからここが直接呼ばれるかもしれないのでここでもチェックしとく
		return;
	}

	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
		energy_use = 75;
	else
		energy_use = 100;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print(_("銃が作動しない。", "The gun doesn't work."));
		sound(SOUND_FAIL);
		return;
	}



	chance = calc_gun_fire_chance(slot);


	//射撃の基礎パラメータとメッセージを設定する ★関連はハードコーディングしてしまう
	switch(act_ptr->index)
	{
	case ACT_CRIMSON:
		rad = 2;
		base = p_ptr->lev * p_ptr->lev * 6 / 50;
		typ = GF_ROCKET;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = _("せっかくだから『クリムゾン』をぶっぱなした！",
                    "It's about time; you blast 'Crimson'!");
		break;

	case ACT_BFG9000:
		rad = 5;
		base = 800;
		dice = 8;
		sides = 100;
		typ = GF_DISP_ALL;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = _("銃身から巨大なエネルギー弾が放たれた！",
                    "The gun fires a giant energy bullet!");
		break;
	case ACT_BAROQUE:
		rad=1;
		base = 500;
		typ = GF_GENOCIDE;
		mode = GUN_FIRE_MODE_ROCKET;
		if(one_in_(6)) shot_msg = _("羽のようなものが飛び散るのが見えた気がした...",
                                    "You thought you saw feather flying around for a moment...");
		else shot_msg = _("天使銃を発射した！", "You fire the angelic gun!");
		break;
	case ACT_DER_FREISCHUTZ:
		rad = 1;
		base = plev*2;
		if(special_shooting_mode == SSM_SEIRAN5) base *= 5;
		typ = GF_SHARDS;
		mode = GUN_FIRE_MODE_MADAN;
		shot_msg = _("魔弾を放った！", "You fire a magic bullet!");


		break;

	case ACT_YAKUZA_GUN:
		dice = 1;
		sides = 20;
		if(special_shooting_mode == SSM_SEIRAN5 || special_shooting_mode == SSM_ELEC_BEAM) dice = 20;
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_BLAST;
#ifdef JP
		if(one_in_(10)) shot_msg = "「ザッケンナコラーッ！」";
		else shot_msg = "BLAMBLAMBLAMBLAMBLAM!";
#else
        shot_msg = "BLAMBLAMBLAMBLAMBLAM!";
#endif
		break;

	case ACT_HARKONNEN2:
		base = 1;
		mode = GUN_FIRE_MODE_VLADIMIR;
		shot_msg = _("広域立体制圧用爆裂焼夷榴弾を発射した！",
                    "You fire a wide-area suppression explosive incendiary bomb!");
		break;


	//基本的な銃
	case ACT_GUN_VARIABLE:
		dice = o_ptr->dd;
		sides = o_ptr->ds;
		base = o_ptr->to_d;
		typ = o_ptr->xtra1;
		mode = o_ptr->xtra4;
		//何かmsg入れる？
		break;

	default:
		msg_print(_("ERROR:この銃の射撃基礎パラメータ処理がされていない",
                    "ERROR: Shooting parameter logic not defined for this gun."));
		return;
	}


	//ダメージ計算
	dam = base;
	if(dice && sides) dam += damroll(dice,sides);
	if(dam < 1)
	{
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format(_("%sは不発だ！", "Your %s misfires!"),o_name);
		return;
	}

	//兵士「銃弾変更」のとき
	if(special_shooting_mode >= SSM_CHANGE_AMMO_START && special_shooting_mode <= SSM_CHANGE_AMMO_END)
	{
		int bullet_idx = special_shooting_mode;
		//ハルコンネン２など特殊な武器は非対象。そのまま普通に撃ってしまうと残弾などを消費してしまうのでここで終了
		//v1.1.22c ロケランも除外。☆で一撃3000くらいのが出てしまって簡単にカンストする。
 		if(o_ptr->sval == SV_FIRE_GUN_ROCKET ||
			mode == GUN_FIRE_MODE_MADAN || mode == GUN_FIRE_MODE_VLADIMIR || mode == GUN_FIRE_MODE_BLAST
			|| act_ptr->index == ACT_BFG9000 || act_ptr->index == ACT_BAROQUE)
		{
			if(p_ptr->pclass == CLASS_SEIRAN)
				msg_print(_("この銃でその特技を使うことはできない。",
                            "You cannot use your ability with this gun."));
			else
				msg_print(_("この銃は銃弾を変更できない。",
                            "You cannot change ammo for this gun."));
			return;
		}


		//職業兵士の「銃弾変更」に関する残弾やMPの処理
		if (p_ptr->pclass == CLASS_SOLDIER)
		{

			if (soldier_bullet_table[bullet_idx].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[bullet_idx])
			{
				msg_print(_("弾切れだ！", "Out of ammo!"));
				return;
			}
			else if (soldier_bullet_table[bullet_idx].cost > p_ptr->csp)
			{
				msg_print(_("MPが足りない。", "Not enough MP."));
				return;
			}
			p_ptr->csp -= soldier_bullet_table[bullet_idx].cost;
			if (soldier_bullet_table[bullet_idx].magic_bullet_lev == SS_M_MATERIAL_BULLET)
				p_ptr->magic_num1[bullet_idx] -= 1;

		}


		//「射撃」以外の武器を銃弾変更すると威力減少
		switch(typ)
		{
		case GF_ARROW:
			break;
		case GF_FIRE:
		case GF_COLD:
		case GF_ACID:
		case GF_ELEC:
		case GF_POIS:
		case GF_PLASMA:
		case GF_NUKE:
		case GF_POLLUTE:
		case GF_ICE:
		case GF_STEAM:
			dam -= dam / 2;
			break;
		default:
			dam -= dam / 3;
			break;
		}
	}

	//v1.1.21 兵士実装に伴い属性変更を整頓
	//special_shooting_modeによる属性変更など
	switch(special_shooting_mode)
	{
	case 0:	//特殊射撃なし
		break;

	case SSM_SEIRAN1: //清蘭1 弾道消去
		if(mode == GUN_FIRE_MODE_BLAST) break;
 		if(mode == GUN_FIRE_MODE_MADAN) break;
		if(mode == GUN_FIRE_MODE_VLADIMIR) break;
		chance = 100; //外れない
		if(!rad && (mode == GUN_FIRE_MODE_BALL || mode == GUN_FIRE_MODE_ROCKET || mode == GUN_FIRE_MODE_BREATH || mode == GUN_FIRE_MODE_SPARK ))
			rad = 1 + p_ptr->lev / 20;
		mode = GUN_FIRE_MODE_JUMP;
		break;

	case SSM_SEIRAN2: //清蘭2 アウェイ追加
		break;
	case SSM_SEIRAN3: //清蘭3 跳弾
		break;
	case SSM_SEIRAN4: //清蘭4 全弾射撃
		break;

	case SSM_SEIRAN5: //清蘭5 属性変更
		if(mode == GUN_FIRE_MODE_VLADIMIR) break;
 		if(mode == GUN_FIRE_MODE_ROCKET) break;
		mode = GUN_FIRE_MODE_BEAM;
		typ = GF_SEIRAN_BEAM;
		dam = dam * 3 / 2;
		shot_msg = _("必殺の一撃が空間を歪めた！", "A critical attack creates a space distortion!");
		break;

	case SSM_BLACK_PEGASUS:
		if (mode == GUN_FIRE_MODE_VLADIMIR) break;
		if (mode == GUN_FIRE_MODE_MADAN) break;
		mode = GUN_FIRE_MODE_ROCKET;
		typ = GF_METEOR;
		dam *= 2 ;
		rad = 5;
		shot_msg = _("天をも震わせる一撃を放った！", "You fire a shot that shakes the heavens!");
		break;


	case SSM_CONFUSE: //牽制射撃 ダメージ0 混乱付与　範囲攻撃系の銃には無効
 		if(mode == GUN_FIRE_MODE_MADAN
		||mode == GUN_FIRE_MODE_VLADIMIR
 		||mode == GUN_FIRE_MODE_BREATH
 		||mode == GUN_FIRE_MODE_SPARK
 		||mode == GUN_FIRE_MODE_BALL
 		||mode == GUN_FIRE_MODE_ROCKET)
		{
			msg_print(_("この銃では牽制にならない。", "You can't do it with this gun."));
			return;
		}

		if(one_in_(3))		shot_msg = _("敵の足元を狙って撃った。", "You shoot at enemy's feet.");
		else if(one_in_(2))	shot_msg = _("敵の鼻先を掠めて撃った。", "You shoot at the tip of enemy's nose.");
		else				shot_msg = _("敵の機先を制して撃った。", "You shoot just above the enemy's head.");
		typ = GF_SPECIAL_SHOT;

	//case SSM_HEAD_SHOT: //ヘッドショット クリティカル判定発生..だったが別処理にした
	//	break;
	case SSM_RAPID_FIRE: //ラピッドファイア 乱射
		if(is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
			shot_msg = _("弾丸が空中で乱反射し標的へ向かった！",
                        "Your bullets reflect in midair and aim at the target!");
		break;

	case SSM_FIRE:
		typ = GF_FIRE;
		shot_msg = _("赤熱する弾丸を放った！", "You shoot a red-hot bullet!");
		dam *= 2;
		break;
	case SSM_COLD:
		typ = GF_COLD;
		shot_msg = _("冷気をまとう弾丸を放った！", "You shoot a bullet covered in frost!");
		dam *= 2;
		break;
	case SSM_ELEC:
		typ = GF_ELEC;
		shot_msg = _("火花を散らす弾丸を放った！", "You shoot a sparking bullet!");
		dam *= 2;
		break;
	case SSM_ACID:
		typ = GF_ACID;
		shot_msg = _("酸の沸き立つ弾丸を放った！", "You shoot a bullet dripping acid!");
		dam *= 2;
		break;
	case SSM_LIGHT: //閃光弾
		typ = GF_SPECIAL_SHOT;
		rad = 2;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = _("閃光弾を放った！", "You shoot a bullet of light!");
		break;
	case SSM_VAMPIRIC:
		typ = GF_SPECIAL_SHOT;
		mode = GUN_FIRE_MODE_BOLT;
		shot_msg = _("血に飢えた弾を放った！", "You shoot a bloodthirsty bullet!");
		break;
	case SSM_SONICWAVE:
		typ = GF_SOUND;
		mode = GUN_FIRE_MODE_BEAM;
		shot_msg = _("衝撃波を放った！", "You shoot a shockwave!");
		break;
	case SSM_GRAV:
		typ = GF_GRAVITY;
		rad = 2;
		mode = GUN_FIRE_MODE_BALL;
		shot_msg = _("力場の弾丸を放った！", "You shoot a gravitational bullet!");
		break;

	case SSM_POIS:
		typ = GF_POIS;
		dam *= 3;
		shot_msg = _("毒ガス弾を放った！", "You shoot a poison gas bullet!");
		break;
	case SSM_HEAVY:
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_DEFAULT;
		dam *= 2;
		shot_msg = _("高密度の弾丸を放った！", "You fire a high density bullet!");
		break;
	case SSM_BREATH:
		mode = GUN_FIRE_MODE_BREATH;
		dam *= 3;
		shot_msg = _("銃口から激しいブレスが吹き出された！", "Violent breath shoots out of your gun!");
		break;
	case SSM_ICE:
		typ = GF_ICE;
		mode = GUN_FIRE_MODE_ROCKET;
		rad = 1;
		dam *= 3;
		shot_msg = _("凍てつく弾丸を放った！", "You fire a freezing bullet!");
		break;

	case SSM_HOLY:
		typ = GF_HOLY_FIRE;
		mode = GUN_FIRE_MODE_ROCKET;
		rad = 2;
		dam *= 3;
		shot_msg = _("神々しく輝く弾丸を放った！", "You fire a bullet radiating divine light!");
		break;

	case SSM_RED:
		typ = GF_FIRE;
		mode = GUN_FIRE_MODE_BEAM;
		dam *= 5;
		shot_msg = _("真紅の光線を放った！", "You fire a deep-red beam!");
		break;
	case SSM_DIAMOND:
		typ = GF_PSY_SPEAR;
		mode = GUN_FIRE_MODE_BEAM;
		dam *= 4;
		shot_msg = _("銃口から眩い輝きが溢れた！", "Dazzling glow bursts forth out of your gun!");
		break;
	case SSM_METEOR:
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_DEFAULT;
		dam *= 4;
		shot_msg = _("銃口から流星が放たれた！", "You fire a meteor!");
		break;
	case SSM_HIHIIROKANE:
		typ = GF_DISINTEGRATE;
		mode = GUN_FIRE_MODE_SPARK;
		dam *= 10;
		shot_msg = _("巨大なエネルギーが放たれた！", "You fire a massive energy blast!");
		break;


	case SSM_SLAY_ANIMAL:
	case SSM_SLAY_NONLIV:
	case SSM_SLAY_EVIL:
	case SSM_SLAY_HUMAN:
	case SSM_SLAY_DRAGON:
	case SSM_SLAY_DEITY:
	case SSM_SILVER:
	case SSM_NIGHTMARE:
	case SSM_SLAY_FLYER:
		typ = GF_SPECIAL_SHOT;
		break;

	case SSM_ELEC_BEAM: //電撃ビーム
		if (mode == GUN_FIRE_MODE_MADAN	|| mode == GUN_FIRE_MODE_VLADIMIR)
		{
			msg_print(_("この銃では技が使えない。", "You can't use your ability with this gun."));
			return;
		}
		typ = GF_ELEC;
		mode = GUN_FIRE_MODE_BEAM;
		break;

	default:
		break;

	}

	//充填処理(発動に失敗しても充填待ちになる)
	if(o_ptr->timeout + timeout_base > 32000L)
	{
		msg_print(_("ERROR:この銃のタイムアウト値が32000を超えた",
                    "ERROR: Gun timeout value over 32000"));
		return;
	}
	o_ptr->timeout += timeout_base;
	p_ptr->window |= PW_EQUIP;
	//ガンカタ残弾判定のため弾切れチェック
	if(o_ptr->timeout > timeout_max)
		p_ptr->update |= PU_BONUS;

	if (cheat_xtra) msg_format("gun_chance:%d%%", chance);
	//発動失敗判定
	if(randint1(100) > chance)
	{
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format(_("%sの射撃に失敗した！弾はあらぬ方向へ飛んでいった..",
                    "Your %s misfires! The bullet gets launched in wrong direction..."),o_name);
		return;
	}


	//v1.1.21 一部特殊射撃のときのクリティカルボーナスを計算するかどうかのフラグを立てる。
	//散弾銃で範囲の敵全部にクリティカルとか出たらなんか変なので、
	//最初にproject_m()が呼ばれたときにだけダメージ増加計算してフラグをクリアする。
	//ここで直接倍率を計算しないのは、「近接射撃」の隣接判定やヘッドショットの対生物判定などがここでは難しいため
	if(mode != GUN_FIRE_MODE_BLAST && mode != GUN_FIRE_MODE_MADAN && mode != GUN_FIRE_MODE_VLADIMIR)
	{
		//前回の行動後にリセットされてるはずだが一応
		hack_gun_fire_critical_flag = 0L;
		//クリティカル判定に使う銃の命中修正を保持
		hack_gun_fire_critical_hit_bonus = o_ptr->to_h;

		//if(special_shooting_mode == SSM_HEAD_SHOT)//ヘッドショット
		if(p_ptr->special_attack & ATTACK_HEAD_SHOT)
			hack_gun_fire_critical_flag |= GUN_CRIT_HEAD;

		if(p_ptr->concent)//精神集中
			hack_gun_fire_critical_flag |= GUN_CRIT_CONCENT;

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_CLOSE_RANGE_SHOT))//近接射撃
			hack_gun_fire_critical_flag |= GUN_CRIT_CLOSE;

		if(p_ptr->pseikaku == SEIKAKU_TRIGGER)//トリガーハッピー
			hack_gun_fire_critical_flag |= GUN_CRIT_TRIGGERHAPPY;

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_NOCTO_VISION) && !(cave[py][px].info & CAVE_GLOW) && p_ptr->cur_lite <= 0)
			hack_gun_fire_critical_flag |= GUN_CRIT_NIGHT;

		if(CHECK_CONCEALMENT)
			hack_gun_fire_critical_flag |= GUN_CRIT_CONCEAL;

		//兵士「銃弾変更」のとき
		if(special_shooting_mode >= SSM_CHANGE_AMMO_START && special_shooting_mode <= SSM_CHANGE_AMMO_END)
			hack_gun_fire_critical_flag |= GUN_CRIT_SPECIALBULLET;

		//魔弾の射手
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_DER_FREISCHUTZ))
			hack_gun_fire_critical_flag |= GUN_CRIT_MAGICBULLET;

		//射撃やボルト以外はクリティカルしにくい
		if(mode == GUN_FIRE_MODE_SPARK || mode == GUN_FIRE_MODE_BREATH)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_5;
		else if(mode == GUN_FIRE_MODE_BALL || mode == GUN_FIRE_MODE_ROCKET)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_3;
		else if(mode == GUN_FIRE_MODE_BEAM)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_2;
		//連射系技能のときにはクリティカルしにくい
		else if(special_shooting_mode == SSM_RAPID_FIRE || special_shooting_mode == SSM_SEIRAN4)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_2;

	}

	//msg
	if(shot_msg) msg_format("%s",shot_msg);

	//経験値を得るためのフラグ
	hack_flag_gain_gun_skill_exp = TRUE;

	switch(mode)
	{
	case GUN_FIRE_MODE_DEFAULT: //射撃
	case GUN_FIRE_MODE_BOLT://ボルト
		fire_bolt(typ,dir,dam);
		break;
	case GUN_FIRE_MODE_BALL:
		rad = 1 + p_ptr->lev / 20;
		fire_ball(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_BEAM:
		fire_beam(typ,dir,dam);
		break;
	case GUN_FIRE_MODE_ROCKET:
		if(!rad) rad = 1 + p_ptr->lev / 20;
		fire_rocket(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_BREATH:
		rad = -2 - p_ptr->lev / 40;
		fire_ball(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_SPARK:
		rad = 1;
		fire_spark(typ,dir,dam,rad);
		break;
	case GUN_FIRE_MODE_JUMP:
		fire_ball_jump(typ,dir,dam,rad,_("銃弾は空間を跳躍した！", "The shot jumps through space!"));
		break;
	case GUN_FIRE_MODE_BLAST:
		fire_blast(typ,dir,dice,sides,20,3,0L);
		break;
	case GUN_FIRE_MODE_MADAN:
		{ //v1.1.44 ルーチン見直し

			if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET, SS_M_DER_FREISCHUTZ))
				hack_der_freischutz_basenum = 20 + p_ptr->concent;
			else
				hack_der_freischutz_basenum = 0;

			if (cheat_xtra) msg_format("basenum_init:%d", hack_der_freischutz_basenum);
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_SHARDS, rad);
			hack_der_freischutz_basenum = 0;

		}

		/*
		{
			int ty,tx;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_DER_FREISCHUTZ))
				hack_der_freischutz_basenum = 20 + p_ptr->concent;
			else
				hack_der_freischutz_basenum = 0;

			if(cheat_xtra) msg_format("basenum_init:%d",hack_der_freischutz_basenum);
			project(0, rad, ty, tx, dam, GF_SHARDS, flg, -1);
		}
		*/
		break;
	//ハルコンネン2 ダミーアイテム投擲として特殊処理する
	case GUN_FIRE_MODE_VLADIMIR:
		{
			object_type forge;
			object_type *tmp_o_ptr = &forge;
			int ty,tx;

			if(special_shooting_mode == SSM_SEIRAN2) special_shooting_mode = 0L;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}

			object_prep(tmp_o_ptr,lookup_kind(TV_COMPOUND,SV_COMPOUND_VLADIMIR));

			do_cmd_throw_aux2(py,px,ty,tx,2,tmp_o_ptr,5);

		}
		break;
	default:
		msg_print(_("ERROR:do_cmd_fire_gun_aux()にこの武器の設定がされていない",
                    "ERROR: Undefined weapon in do_cmd_fire_gun_aux()"));
		return;
	}
	hack_flag_gain_gun_skill_exp = FALSE;


	return;

}


//兵士特技「ラピッドファイア」うどんげ専用性格特技「ルナティックダブル」に使う。うどんげ新性格の特技に流用するために別関数にした。
//行動順消費するときTRUEを返す
//shoot_num:最大連射数
//mode
//0:通常
//1:ライトニングネイ　電撃属性ビームになる
bool	rapid_fire(int shoot_num, int mode)
{
	int dir = 5;
	int timeout_base, timeout_max;
	int count;
	object_type *o_ptr;
	bool flag_ammo = FALSE;
	bool flag_mon = FALSE;

	if (!CHECK_USE_GUN)
	{
		msg_print(_("銃を撃てない。", "You're not holding a gun."));
		return FALSE;
	}

	switch (mode)
	{
	case 1:
		special_shooting_mode = SSM_ELEC_BEAM;
		msg_print(_("あなたの銃は電光のように閃いた！", "Your gun flashes like lightning!"));
		break;

	default:
		special_shooting_mode = SSM_RAPID_FIRE;
		break;

	}


	for (count = 0; count < shoot_num ; count++)
	{
		bool righthand = FALSE;
		bool lefthand = FALSE;

		//残弾チェック
		if (inventory[INVEN_RARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_RARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if (o_ptr->timeout <= timeout_max) righthand = TRUE;
		}
		if (!get_random_target(0, 0)) break;
		flag_mon = TRUE;
		if (righthand)do_cmd_fire_gun_aux(INVEN_RARM, dir);

		if (inventory[INVEN_LARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_LARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if (o_ptr->timeout <= timeout_max) lefthand = TRUE;
		}
		if (!righthand && !lefthand) break;
		else flag_ammo = TRUE;

		if (!get_random_target(0, 0)) break;
		if (lefthand)do_cmd_fire_gun_aux(INVEN_LARM, dir);
	}
	special_shooting_mode = 0L;

	if (!flag_mon)
	{
		msg_print(_("周囲に標的がいない。", "There are no targets around."));
		return FALSE;
	}
	else if (!flag_ammo)
	{
		msg_print(_("しかし弾がなかった。", "You don't have ammo."));
		return FALSE;
	}

	return TRUE;

}


/*:::射撃をする*/
///sys item 射撃ルーチン
///mod160603 清蘭特殊射撃のためにboolを返すことにした
bool do_cmd_fire(void)
{
	int item;
	object_type *j_ptr;
	cptr q, s;
	int tester;

	is_fired = FALSE;	/* not fired yet */

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE || p_ptr->clawextend)
	{
		msg_print(_("この姿では射撃ができない。", "You can't shoot while in this form."));
		return FALSE;
	}

	///item sys 弓のobject_typeを得る部分
	/* Get the "bow" (if any) */
	//j_ptr = &inventory[INVEN_BOW];
	j_ptr = &inventory[INVEN_PACK + shootable(&tester)];


	if (tester == SHOOT_HEAVY_WEAPON)
	{
#ifdef JP
			msg_print("射撃武器が重すぎて構えられない。");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}

	if (tester == SHOOT_UNSUITABLE)
	{
#ifdef JP
			msg_print("この装備ではうまく射撃ができない。");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}


	/* Require a launcher */
	if (!j_ptr->tval)
	{
#ifdef JP
		msg_print("射撃用の武器を持っていない。");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}

/*
	///mod131223
	if (j_ptr->tval ==TV_GUN && j_ptr->sval == SV_FIRE_GUN_CRIMSON
		|| j_ptr->tval ==TV_GUN && j_ptr->sval == SV_FIRE_GUN_MUSKET)
	//if (j_ptr->sval == SV_CRIMSON)
	{
#ifdef JP
		msg_print("この武器は発動して使うもののようだ。");
#else
		msg_print("Do activate.");
#endif
		flush();
		return;
	}
*/

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	//九十九姉妹演奏中断
	stop_tsukumo_music();

	///mod160503 銃装備時、銃発動専用ルーチンへ飛ぶ
	if(CHECK_USE_GUN)
	{
		bool righthand = FALSE;
		bool lefthand = FALSE;
		int dir;
		int timeout_base, timeout_max;
		object_type *o_ptr;

		if(inventory[INVEN_RARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_RARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if(o_ptr->timeout <= timeout_max) righthand = TRUE;
		}
		if(inventory[INVEN_LARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_LARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if(o_ptr->timeout <= timeout_max) lefthand = TRUE;
		}

		if(!righthand && !lefthand)
		{
			msg_print(_("まだ銃弾の装填が済んでいない。", "You haven't loaded the ammo yet."));
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;

		if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);
		if(dir == 5 && !target_okay()) lefthand = FALSE;
		if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);

		//撃ったかどうか判定するためにenergy_useを使う
		if(energy_use) return TRUE;
		else return FALSE;
	}

	/* Require proper missile */
	///item 装備している弓に適合した矢玉
	item_tester_tval = p_ptr->tval_ammo;

	/* Get an item */
#ifdef JP
	q = "どれを撃ちますか? ";
	s = "発射されるアイテムがありません。";
#else
	q = "Fire which item? ";
	s = "You have nothing to fire.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		flush();
		return FALSE;
	}

	/* Fire the item */
	do_cmd_fire_aux(item, j_ptr);



	//v1.1.74 弓の発動効果でセイントスターアローを追加したので復活
	if (snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("射撃の反動が体を襲った。");
#else
		msg_print("You are hit by recoil.");
#endif
		(void)set_slow(p_ptr->slow + randint0(7) + 7, FALSE);
		(void)set_stun(p_ptr->stun + randint1(25));
	}



	return TRUE;

#if 0
	///class スナイパーの射撃後特殊処理
	if (!is_fired || p_ptr->pclass != CLASS_SNIPER) return;

	/*:::スナイパーの特殊射撃処理*/
	/* Sniper actions after some shootings */
	if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_AWAY)
	{
		teleport_player(10 + (p_ptr->concent * 2), 0L);
	}
	if (snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("射撃の反動が体を襲った。");
#else
		msg_print("A reactionary of shooting attacked you. ");
#endif
		(void)set_slow(p_ptr->slow + randint0(7) + 7, FALSE);
		(void)set_stun(p_ptr->stun + randint1(25));
	}

#endif

}

///mod131224
static bool item_tester_hook_boomerang(object_type *o_ptr)
{
	//if ((o_ptr->tval==TV_DIGGING) || (o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM) || (o_ptr->tval == TV_HAFTED)) return (TRUE);
	if(object_is_melee_weapon(o_ptr)) return (TRUE);
	/* Assume not */
	return (FALSE);
}


/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
/*:::投擲*/
/*:::mult:投擲パワー　強力投擲変異以外は1*/
/*:::boomerang:剣術家の「ブーメラン」のときTRUE mult値が跳ね上がるのでほかの特技でTRUEにするとき注意*/
/*shriken:
 *忍者の八方手裏剣のときインベントリ番号が入っていた(現在使われていない)
 *それ以外のとき-1が入ってる。宇佐見菫子特技による特殊投擲のとき-2、美天の棒投擲のとき-3
 TODO:
 boomerangとshurikenのフラグの扱いと内部の処理が混乱しているのでそのうち整理したい
 */

bool do_cmd_throw_aux(int mult, bool boomerang, int shuriken)
{
	int dir, item;
	int i, j, y, x, ty, tx, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div, dd, ds;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;
	bool suitable_item = FALSE;

	bool sumireko_throwing = FALSE;//菫子特殊投擲　必ず戻ってくる

	char o_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TR_FLAG_SIZE];
	cptr q, s;
	bool success_catch = FALSE; //「必ず投げたものが帰ってきてキャッチできる」フラグと「投げたものをキャッチできた」フラグを兼用している

	bool do_drop = TRUE;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print(_("この姿では投擲ができない。", "Your current form is unsuitable for throwing."));
		return FALSE;
	}

	if(p_ptr->pclass == CLASS_SUMIREKO && shuriken == -2 && p_ptr->lev > 34) sumireko_throwing = TRUE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/*:::忍者の八方手裏剣のときは投げるアイテムが決まっている*/
	if (shuriken >= 0)
	{
		item = shuriken;
	}
	/*:::剣術家がブーメランの技を使うときは手に持ってる武器を投げる　二刀流の時は選ぶ*/
	else if (boomerang)
	{
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			item_tester_hook = item_tester_hook_boomerang;
#ifdef JP
			q = "どの武器を投げますか? ";
			s = "投げる武器がない。";
#else
			q = "Throw which item? ";
			s = "You have nothing to throw.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP)))
			{
				flush();
				return FALSE;
			}
		}
		else if (buki_motteruka(INVEN_LARM)) item = INVEN_LARM;
		else item = INVEN_RARM;
	}
	//v2.0.11 美天の棒投げ　棒しか投げられない
	else if (shuriken == -3)
	{
		/* Get an item */
#ifdef JP
		q = "どの棒を投げますか? ";
		s = "投げるアイテムがない。";
#else
		q = "Throw which staff? ";
		s = "You have nothing to throw.";
#endif
		item_tester_tval = TV_STICK;
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP)))
		{
			flush();
			return FALSE;
		}

	}
	/*:::それ以外の時は投げるものを選ぶ*/
	else
	{
		/* Get an item */
#ifdef JP
		q = "どのアイテムを投げますか? ";
		s = "投げるアイテムがない。";
#else
		q = "Throw which item? ";
		s = "You have nothing to throw.";
#endif

		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP)))
		{
			flush();
			return FALSE;
		}
	}

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	///mod140415 外せない装備を投げないようにする
	if(item >= INVEN_RARM && !wield_check(item,INVEN_PACK)) return FALSE;

	/* Item is cursed */
	/*:::装備中のものを投げようとして呪われている場合*/
	if (object_is_cursed(o_ptr) && (item >= INVEN_RARM))
	{
		/* Oops */
#ifdef JP
		msg_print("ふーむ、どうやら呪われているようだ。");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif

		/* Nope */
		return FALSE;
	}
	/*:::アリーナではブーメラン以外使えない*/
	//v1.1.51 解禁
#if 0
	if (p_ptr->inside_arena && !boomerang)
	{
		//if (o_ptr->tval != TV_SPIKE)
		{
#ifdef JP
			msg_print("アリーナではアイテムを使えない！");
#else
			msg_print("You're in the arena now. This is hand-to-hand!");
#endif
			msg_print(NULL);

			/* Nope */
			return FALSE;
		}
	}
#endif

	if(sumireko_throwing && object_is_weapon(o_ptr)) boomerang = TRUE;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Extract the thrown object's flags. */
	object_flags(q_ptr, flgs);
	/*:::たいまつ用フラグ処理*/
	torch_flags(q_ptr, flgs);

	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;

	/* Distribute the charges of rods/wands between the stacks */
	/*:::魔法棒やロッドを投げるときに残った分の充填数や時間を減らす処理*/
	distribute_charges(o_ptr, q_ptr, 1);

	/* Single object */
	/*:::投げるのは常に一つ*/
	q_ptr->number = 1;

	/* Description */
	object_desc(o_name, q_ptr, OD_OMIT_PREFIX);

	/*:::強力投擲の指輪の効果*/
	if (p_ptr->mighty_throw) mult += 2;


	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	/*:::最低重量10に固定　重量10が1ポンド(0.5kg)扱い*/
	///mod140901 射程短くした。武器重量全体的に軽くした分を補正
	//div = ((q_ptr->weight > 10) ? q_ptr->weight : 10);
	div = (((q_ptr->weight * 2) > 10) ? (q_ptr->weight * 2) : 10);
	if ((have_flag(flgs, TR_THROW)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->stat_ind[A_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if (tdis > mul) tdis = mul;

	//美天の棒投げ　少し射程が長い
	if (shuriken == -3) tdis += 3;

	if(tdis > 18) tdis = 18;
	if(tdis < 1) tdis = 1;

	if(sumireko_throwing) tdis = 18;

	/*:::八方手裏剣の場合＠の位置から±50のxyがターゲットになる*/
	if (shuriken >= 0)
	{
		ty = randint0(101)-50+py;
		tx = randint0(101)-50+px;
	}
	/*:::それ以外はターゲットを選ぶ*/
	else
	{
		project_length = tdis + 1;

		/* Get a direction (or cancel) */
		if (!get_aim_dir(&dir)) return FALSE;

		/* Predict the "target" location */
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		/* Check for "target request" */
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}

		project_length = 0;  /* reset to default */
	}
	/*:::トールハンマー、イージスファング、ブーメランのフラグ*/
	///mod140830 投げても返ってくるアイテムをフラグに変更
	//if ((q_ptr->name1 == ART_MJOLLNIR) ||
	//    (q_ptr->name1 == ART_AEGISFANG) || boomerang)
	if ( have_flag(flgs, TR_BOOMERANG) || boomerang)
		return_when_thrown = TRUE;

	if (shuriken == -3) return_when_thrown = TRUE;

	/*:::アイテム減少、「まだ○個の△を持っている」などの記述*/
	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		if (!return_when_thrown)
			inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}
	/*:::装備品を投げたとき*/
	if (item >= INVEN_RARM)
	{
		equiped_item = TRUE;
		p_ptr->redraw |= (PR_EQUIPPY);
	}

	/* Take a turn */
	///mod140830 投擲技能が高いと速度が上がることにする 最大三倍程度
	energy_use = 100;
	if(ref_skill_exp(SKILL_THROWING) > 1600)
	{
		int dec;

		if(suitable_item)	dec = (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
		else dec = (ref_skill_exp(SKILL_THROWING) -1600) / 128;

		if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && p_ptr->pclass == CLASS_REIMU)
			dec += osaisen_rank() * 2;

		energy_use -= dec;
	}
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) energy_use -= energy_use / 5;

	if(energy_use < 20) energy_use = 20;//paranoia

	if (p_ptr->wizard) msg_format("throw_aux() use_energy:%d", energy_use);

	///class　投擲速度ボーナス 上の速度ボーナスを入れたので削除した
	/*:::盗賊と忍者は速度にボーナス*/
	/* Rogue and Ninja gets bonus */
	//if ((p_ptr->pclass == CLASS_ROGUE) || (p_ptr->pclass == CLASS_NINJA))
	//	energy_use -= p_ptr->lev;

	/* Start at the player */
	y = py;
	x = px;


	/* Hack -- Handle stuff */
	handle_stuff();
	/*:::投擲アイテムの決定が済んだのでshrikenを別のフラグとして再利用しているらしい*/
	/*:::投げて大きなダメージの武器は忍者が投げるとさらに高ボーナス*/
	///class item 投擲向きの武器と忍者
	///mod131223 tval
	//if ((p_ptr->pclass == CLASS_NINJA) && ((q_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TR_THROW)) && (q_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	///mod140830 suitable_itemとしてフラグを分け、職にかかわらず投擲向き武器の処理は行われるようにした
	//if ((p_ptr->pclass == CLASS_NINJA) && (((have_flag(flgs, TR_THROW)) && (q_ptr->tval == TV_KNIFE)))) shuriken = TRUE;
	//else shuriken = FALSE;

	/* Chance of hitting */
	/*:::投擲向きの武器は命中判定に武器自身の命中率が加わる*/
	if (suitable_item) chance = ((p_ptr->skill_tht) +
		((p_ptr->to_h_b + q_ptr->to_h) * BTH_PLUS_ADJ));
	else chance = (p_ptr->skill_tht + (p_ptr->to_h_b * BTH_PLUS_ADJ));

	/*:::忍者が投擲向きのものを投げた場合命中率増加*/
	//if (shuriken) chance *= 2;
	///職でなく投擲スキルで代用することにした 最大二倍
	if(suitable_item) chance += chance * ref_skill_exp(SKILL_THROWING) / 8000;
	else chance += chance * ref_skill_exp(SKILL_THROWING) / 16000;

	///mod140928 恐怖状態だと命中率減少
	if(p_ptr->afraid) chance = chance * 2 / 3;

	//「咲夜の世界」
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	/*:::投げたものが飛ぶ処理*/
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], py, px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			/*:::壁に当たって、投げたのが薬、人形、あるいはそれ以外を投げて壁の中にモンスターがいなかった場合ループを抜ける*/
			if ((q_ptr->tval == TV_FIGURINE) || object_is_potion(q_ptr) || !cave[ny[cur_dis]][nx[cur_dis]].m_idx) break;
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny[cur_dis], nx[cur_dis]) && player_can_see_bold(ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Monster here, Try to hit it */
		if (cave[y][x].m_idx)
		{
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int mon_ac = r_ptr->ac;

			//v1.1.94 モンスター防御力低下中はAC25%カット
			if (MON_DEC_DEF(m_ptr))
			{
				mon_ac = MONSTER_DECREASED_AC(mon_ac);
			}


			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit_fire(chance - cur_dis, mon_ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
#ifdef JP
					msg_format("%sが敵を捕捉した。", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%sが%sに命中した。", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this monster race */
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

						/* Hack -- Track this monster */
						health_track(c_ptr->m_idx);
					}
				}
				//v1.1.27 銃をぶん投げたときは別処理にする
				if(q_ptr->tval == TV_GUN)
				{
					gun_throw_effect(y,x,q_ptr);
					//投擲と銃の両方の経験値を得ることにしておく
					gain_skill_exp(SKILL_THROWING, m_ptr);
					gain_skill_exp(TV_GUN, m_ptr);
				}
				else
				{

					/* Hack -- Base damage from thrown object */
					dd = q_ptr->dd;
					ds = q_ptr->ds;
					torch_dice(q_ptr, &dd, &ds); /* throwing a torch */

					//巫女系が針を投げるとダイスボーナス
					if(q_ptr->tval == TV_OTHERWEAPON && q_ptr->sval == SV_OTHERWEAPON_NEEDLE
						&&(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_SANAE) )
					{
						dd += 3;
					}
					//v1.1.21 兵士短剣技能
					else if ((q_ptr->tval == TV_KNIFE) && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_KNIFE_MASTERY))
					{
						dd += 2;

					}

					if(suitable_item && object_is_artifact(q_ptr)) dd *= 2; //投擲向きのアーティファクトを投げた時のボーナス追加
					tdam = damroll(dd, ds);
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux(q_ptr, tdam, m_ptr, 0, TRUE);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);
					if (q_ptr->to_d > 0)
						tdam += q_ptr->to_d;
					else
						tdam += -q_ptr->to_d;

					if (!sumireko_throwing && boomerang)
					{
						tdam *= (mult+p_ptr->num_blow[item - INVEN_RARM]);
						tdam += p_ptr->to_d_m;
					}
					else if (suitable_item)
					{
						tdam *= (2+mult);
						tdam += p_ptr->to_d_m;
					}
					else
					{
						tdam *= mult;
					}

					//if (shuriken)
					//{
					//	tdam += ((p_ptr->lev+30)*(p_ptr->lev+30)-900)/55;
					//}

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					///mod150620 ダメージに重量ボーナスがつくようにしてみた
					tdam += damroll(MAX(1,(tdis / 3)), MAX(q_ptr->weight/10,1));

					/* Modify the damage */
					tdam = mon_damage_mod(m_ptr, tdam, FALSE);

					/* Complex message */
					if (p_ptr->wizard)
					{
	#ifdef JP
						msg_format("%d/%dのダメージを与えた。",
							   tdam, m_ptr->hp);
	#else
						msg_format("You do %d (out of %d) damage.",
							   tdam, m_ptr->hp);
	#endif

					}
					//投擲スキルを得る
					gain_skill_exp(SKILL_THROWING, m_ptr);

					/* Hit the monster, check for death */
					if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
					{
						/* Dead monster */
					}

					/* No death */
					else
					{
						/* Message */
						message_pain(c_ptr->m_idx, tdam);

						/* Anger the monster */
						if ((tdam > 0) && !object_is_potion(q_ptr))
							anger_monster(m_ptr);

						//v2.0.5 投擲にターゲット変更(反撃召喚など用)が設定されていなかった
						set_target(m_ptr, py, px);

						/* Take note */
						if (fear && m_ptr->ml)
						{
							char m_name[80];

							/* Sound */
							sound(SOUND_FLEE);

							/* Get the monster name (or "it") */
							monster_desc(m_name, m_ptr, 0);

							/* Message */
	#ifdef JP
							msg_format("%^sは恐怖して逃げ出した！", m_name);
	#else
							msg_format("%^s flees in terror!", m_name);
	#endif

						}
					}

				}
			}

			/* Stop looking */
			break;
		}
	}

	/* decrease toach's fuel */
	if (hit_body) torch_lost_fuel(q_ptr);

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	/* Figurines transform */
	if ((q_ptr->tval == TV_FIGURINE) && !(p_ptr->inside_arena))
	{
		j = 100;

		if (!(summon_named_creature(0, y, x, q_ptr->pval,
					    !(object_is_cursed(q_ptr)) ? PM_FORCE_PET : 0L)))
#ifdef JP
msg_print("人形は捻じ曲がり砕け散ってしまった！");
#else
			msg_print("The figurine writhes and then shatters.");
#endif

		else if (object_is_cursed(q_ptr))
#ifdef JP
msg_print("これはあまり良くない気がする。");
#else
			msg_print("You have a bad feeling about this.");
#endif

	}

	//v1.1.43 オニフスベは破裂して混乱ボール発生
	if (q_ptr->tval == TV_MUSHROOM && q_ptr->sval == SV_MUSHROOM_PUFFBALL)
	{
		if (hit_body || hit_wall)
		{
			msg_format(_("%sは破裂した！", "%s bursts!"), o_name);
			project(0, 3, y, x, damroll(8, 12), GF_CONFUSION, (PROJECT_JUMP | PROJECT_KILL), -1);
			do_drop = FALSE;
		}
		else j = 0;
	}


	/* Potions smash open */
	if (object_is_potion(q_ptr))
	{
		if (hit_body || hit_wall || (randint1(100) < j))
		{
			/* Message */
#ifdef JP
			msg_format("%sは砕け散った！", o_name);
#else
			msg_format("The %s shatters!", o_name);
#endif


			if (potion_smash_effect(0, y, x, q_ptr->k_idx))
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				/* ToDo (Robert): fix the invulnerability */
				if (cave[y][x].m_idx &&
				    is_friendly(&m_list[cave[y][x].m_idx]) &&
				    !MON_INVULNER(m_ptr) &&
					m_ptr->r_idx != MON_MASTER_KAGUYA
					)
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%sは怒った！", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif

					set_hostile(&m_list[cave[y][x].m_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}

	if (return_when_thrown)
	{
		int back_chance = randint1(30)+20+((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
		char o2_name[MAX_NLEN];

		//ブーメランフラグ付きの★☆を剣術家特技「ブーメラン」で投げたら必ず回収できる
		if (have_flag(flgs, TR_BOOMERANG) && object_is_artifact(q_ptr) && boomerang) success_catch = TRUE;

		//菫子特殊投擲は必ず回収できる
		if(sumireko_throwing) success_catch = TRUE;

		//美天が初期所持の★を棒投げした場合必ず返ってくる
		if (shuriken == -3 && q_ptr->name1 == ART_BITEN) success_catch = TRUE;

		//投擲熟練度が高い場合必ず回収できる
		if (ref_skill_exp(SKILL_THROWING) >= 5600) success_catch = TRUE;

		if (cheat_xtra && success_catch) msg_print("(come back)");

		j = -1;
		if (boomerang) back_chance += 4+randint1(5);
		object_desc(o2_name, q_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && !one_in_(100) || success_catch)
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if (panel_contains(ny[i], nx[i]) && player_can_see_bold(ny[i], nx[i]))
				{
					char c = object_char(q_ptr);
					byte a = object_attr(q_ptr);

					/* Draw, Hilite, Fresh, Pause, Erase */
					print_rel(c, a, ny[i], nx[i]);
					move_cursor_relative(ny[i], nx[i]);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(ny[i], nx[i]);
					Term_fresh();
				}
				else
				{
					/* Pause anyway, for consistancy */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			//必ず戻ってくる場合メッセージを変える
			if (success_catch)
			{
				if (item >= 0)
				{
					msg_format(_("%sは吸い込まれるようにあなたの手の中に納まった。",
                                "%s returns to your hand as if it was sucked in."), o2_name);
				}
				else
				{
					msg_format(_("%sはあなたの足元へ静かに滑り込んだ。", "%s silently slides beneath your feet."), o2_name);
					y = py;
					x = px;
				}

			}

			else if((back_chance > 37) && !p_ptr->blind && (item >= 0))
			{
#ifdef JP
				msg_format("%sが手元に返ってきた。", o2_name);
#else
				msg_format("%s comes back to you.", o2_name);
#endif
				success_catch = TRUE;
			}
			else
			{
				if (item >= 0)
				{
#ifdef JP
					msg_format("%sを受け損ねた！", o2_name);
#else
					msg_format("%s backs, but you can't catch!", o2_name);
#endif
				}
				else
				{
#ifdef JP
					msg_format("%sが返ってきた。", o2_name);
#else
					msg_format("%s comes back.", o2_name);
#endif
				}
				y = py;
				x = px;
			}
		}
		else
		{
#ifdef JP
			msg_format("%sが返ってこなかった！", o2_name);
#else
			msg_format("%s doesn't back!", o2_name);
#endif
		}
	}

	if (success_catch)
	{
		if (item == INVEN_RARM || item == INVEN_LARM)
		{
			/* Access the wield slot */
			o_ptr = &inventory[item];

			/* Wear the new stuff */
			object_copy(o_ptr, q_ptr);

			/* Increase the weight */
			p_ptr->total_weight += q_ptr->weight;

			/* Increment the equip counter by hand */
			equip_cnt++;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Recalculate torch */
			p_ptr->update |= (PU_TORCH);

			/* Recalculate mana XXX */
			p_ptr->update |= (PU_MANA);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);
		}
		else
		{
			inven_carry(q_ptr);
		}
		do_drop = FALSE;
	}
	else if (equiped_item)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	/* Drop (or break) near that location */
	if (do_drop)
	{
		if (cave_have_flag_bold(y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, prev_y, prev_x);
		}
	}

	//v1.1.30 キスメ投擲強化
	if (p_ptr->pclass == CLASS_KISUME) reset_concent = FALSE;

	return TRUE;
}


/*
 * Throw an object from the pack or floor.
 */
void do_cmd_throw(void)
{
	do_cmd_throw_aux(1, FALSE, -1);
}


#ifdef TRAVEL
/*
 * Hack: travel command
 */
#define TRAVEL_UNABLE 9999

static int flow_head = 0;
static int flow_tail = 0;
static s16b temp2_x[MAX_SHORT];
static s16b temp2_y[MAX_SHORT];

/* Hack: forget the "flow" information */
//全グリッドに設定されたトラベルコマンド用移動コスト値をリセットする
void forget_travel_flow(void)
{
	int x, y;

	/* Check the entire dungeon */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Forget the old data */
			travel.cost[y][x] = MAX_SHORT;
		}
	}
}

//地形に応じた移動コスト値を計算する
static int travel_flow_cost(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];
	int cost = 1;

	/* Avoid obstacles (ex. trees) */
	if (have_flag(f_ptr->flags, FF_AVOID_RUN)) cost += 1;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (have_flag(f_ptr->flags, FF_DEEP) && !p_ptr->levitation) cost += 5;
	}

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		int lava = 2;
		if (!p_ptr->resist_fire) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}

	//v1.1.85 地形追加
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_acid) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_pois) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_COLD_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_cold) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_elec) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}






	/* Detected traps and doors */
	if (cave[y][x].info & (CAVE_MARK))
	{
		if (have_flag(f_ptr->flags, FF_DOOR)) cost += 1;
		if (have_flag(f_ptr->flags, FF_TRAP)) cost += 10;
	}

	return (cost);
}

//与えられたコスト値に地形に応じたコスト増加値を加算してtravel.cost[][]に格納する
static void travel_flow_aux(int y, int x, int n, bool wall)
{
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int old_head = flow_head;
	int add_cost = 1;
	int base_cost = (n % TRAVEL_UNABLE);
	int from_wall = (n / TRAVEL_UNABLE);
	int cost;

	/* Ignore out of bounds */
	if (!in_bounds(y, x)) return;

	/* Ignore unknown grid except in wilderness */
	if (dun_level > 0 && !(c_ptr->info & CAVE_KNOWN)) return;

	//2.0.3 氷塊地形は壁と同様に計算する
	/* Ignore "walls" and "rubble" (include "secret doors") */
	if (have_flag(f_ptr->flags, FF_WALL) || have_flag(f_ptr->flags, FF_ICE_WALL) ||
		have_flag(f_ptr->flags, FF_CAN_DIG) ||
		(have_flag(f_ptr->flags, FF_DOOR) && cave[y][x].mimic) ||
		(!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !p_ptr->levitation))
	{
		if (!wall || !from_wall) return;
		add_cost += TRAVEL_UNABLE;
	}
	else
	{
		add_cost = travel_flow_cost(y, x);
	}

	cost = base_cost + add_cost;

	/* Ignore lower cost entries */
	if (travel.cost[y][x] <= cost) return;

	/* Save the flow cost */
	travel.cost[y][x] = cost;

	/* Enqueue that entry */
	temp2_y[flow_head] = y;
	temp2_x[flow_head] = x;

	/* Advance the queue */
	if (++flow_head == MAX_SHORT) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if (flow_head == flow_tail) flow_head = old_head;

	return;
}

/*:::座標y,xへの到達コストを全グリッドに対して計算する*/
///mod160722 輝夜特技で使うためextern
void travel_flow(int ty, int tx)
{
	int x, y, d;
	bool wall = FALSE;
	feature_type *f_ptr = &f_info[cave[py][px].feat];

	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	/* is player in the wall? */
	if (!have_flag(f_ptr->flags, FF_MOVE)) wall = TRUE;

	/* Start at the target grid */
	travel_flow_aux(ty, tx, 0, wall);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp2_y[flow_tail];
		x = temp2_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == MAX_SHORT) flow_tail = 0;

		/* Ignore too far entries */
		//if (distance(ty, tx, y, x) > 100) continue;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			/* Add that child if "legal" */
			travel_flow_aux(y + ddy_ddd[d], x + ddx_ddd[d], travel.cost[y][x], wall);
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;
}

/*:::トラベルコマンド*/
//'`'コマンド。目的地を選択し、travel.cost[][]を再計算し、travel.runに255を格納する
//実際の移動はtravel_step()で行う
void do_cmd_travel(void)
{
	int x, y, i;
	int dx, dy, sx, sy;
	feature_type *f_ptr;

	if (!tgt_pt(&x, &y)) return;

	if ((x == px) && (y == py))
	{
#ifdef JP
		msg_print("すでにそこにいます！");
#else
		msg_print("You are already there!!");
#endif
		return;
	}

	f_ptr = &f_info[cave[y][x].feat];

	if ((cave[y][x].info & CAVE_MARK) &&
		(have_flag(f_ptr->flags, FF_WALL) ||
			have_flag(f_ptr->flags, FF_CAN_DIG) ||
			(have_flag(f_ptr->flags, FF_DOOR) && cave[y][x].mimic)))
	{
#ifdef JP
		msg_print("そこには行くことができません！");
#else
		msg_print("You cannot travel there!");
#endif
		return;
	}

	travel.x = x;
	travel.y = y;

	forget_travel_flow();
	travel_flow(y, x);

	/* Travel till 255 steps */
	travel.run = 255;

	/* Paranoia */
	travel.dir = 0;

	/* Decides first direction */
	dx = abs(px - x);
	dy = abs(py - y);
	sx = ((x == px) || (dx < dy)) ? 0 : ((x > px) ? 1 : -1);
	sy = ((y == py) || (dy < dx)) ? 0 : ((y > py) ? 1 : -1);

	for (i = 1; i <= 9; i++)
	{
		if ((sx == ddx[i]) && (sy == ddy[i])) travel.dir = i;
	}
}
#endif


//v2.0.3 歩くときの高速移動や人魚の尾などによるenergy_use値変更をdo_cmd_walk()から分離した
//run_step()やtravel_step()からも使うため
void walk_energy_modify(void)
{

	///sys 早駆けのターン消費
	if (p_ptr->action == ACTION_HAYAGAKE) energy_use = energy_use * (45 - (p_ptr->lev / 2)) / 100;
	///mod150105 村紗水中高速移動
	else if (p_ptr->pclass == CLASS_MURASA)
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		if (have_flag(f_ptr->flags, FF_WATER)) energy_use /= 3;
	}
	///mod131228 高速移動のターン消費
	else if (p_ptr->speedster) energy_use = energy_use * (75 - p_ptr->lev / 2) / 100;
	///mod131228 スライム変化時のターン消費
	else if (p_ptr->mimic_form == MIMIC_SLIME && !p_ptr->levitation) energy_use = energy_use * 3 / 2;
	///mod140907 人魚のターン消費
	///mod150321 乗馬時は無関係
	else if ((prace_is_(RACE_NINGYO) || p_ptr->muta3 & MUT3_FISH_TAIL) && !p_ptr->levitation && !p_ptr->riding)
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		if (!have_flag(f_ptr->flags, FF_WATER)) energy_use = energy_use * 4 / 3;
	}



}
