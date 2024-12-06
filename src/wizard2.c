/* File: wizard2.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Wizard commands */

#include "angband.h"

static bool kind_is_sword(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval == TV_SWORD) && (k_ptr->sval > SV_WEAPON_BROKEN_SWORD))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


//アイテムの全てのパラメータをテキストとして書き出すテストコマンド
//v1.1.16 idx値を指定するようにしてexternした
void print_all_item_param(int idx)
{
	int item;
	cptr            q, s;
	byte iy, ix, marked, number;
	s16b next_o_idx, weight;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	if(idx < 0 || idx > INVEN_FEET)
	{

		q = _("どのアイテムを確認しますか？", "Which item do you want to check? ");
		s = _("確認できるアイテムがありません。", "There are no items to check.");

		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return;
		/* Get the item (in the pack) */
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			o_ptr = &o_list[0 - item];
		}
	}
	else o_ptr = &inventory[idx];

	//object_desc(o_name, o_ptr, 0);

	//msg_format("name:%s",o_name);
	msg_format("k_idx:%d",o_ptr->k_idx);
	msg_format("iy,ix:(%d,%d)",o_ptr->iy,o_ptr->ix);

	msg_format("tv,sv:(%d,%d)",o_ptr->tval,o_ptr->sval);
	msg_format("pval:(%d)",o_ptr->pval);
	msg_format("discount:(%d)",o_ptr->discount);
	msg_format("number:(%d)",o_ptr->number);
	msg_format("weight:(%d)",o_ptr->weight);
	msg_format("name1:(%d)",o_ptr->name1);
	msg_format("name2:(%d)",o_ptr->name2);
	msg_format("xtra1:(%d)",o_ptr->xtra1);
	msg_format("xtra2:(%d)",o_ptr->xtra2);
	msg_format("xtra3:(%d)",o_ptr->xtra3);
	msg_format("xtra4:(%d)",o_ptr->xtra4);
	msg_format("xtra5:(%d)",o_ptr->xtra5);

	msg_format("to_h:(%d)",o_ptr->to_h);
	msg_format("to_d:(%d)",o_ptr->to_d);
	msg_format("to_a:(%d)",o_ptr->to_a);
	msg_format("ac:(%d)",o_ptr->ac);

	msg_format("dd,ds:(%d,%d)",o_ptr->dd,o_ptr->ds);
	msg_format("timeout:(%d)",o_ptr->timeout);

	msg_format("ident:(%x)",o_ptr->ident);
	msg_format("marked:(%x)",o_ptr->ident);
	if(o_ptr->inscription) msg_format("insc:%s",quark_str(o_ptr->inscription));
	if(o_ptr->art_name) msg_format("art_name:%s",quark_str(o_ptr->art_name));
	msg_format("feel:(%x)",o_ptr->feeling);
	msg_format("flag1:(%x)",o_ptr->art_flags[0]);
	msg_format("flag2:(%x)",o_ptr->art_flags[1]);
	msg_format("flag3:(%x)",o_ptr->art_flags[2]);
	msg_format("flag4:(%x)",o_ptr->art_flags[3]);

	msg_format("curse_flag:(%x)",o_ptr->curse_flags);

	msg_format("next_idx:(%d)",o_ptr->next_o_idx);
	msg_format("held_idx:(%d)",o_ptr->held_m_idx);

	if(p_ptr->pclass == CLASS_RAIKO && !object_is_fixed_artifact(o_ptr) && object_is_melee_weapon(o_ptr))
	{
		int power = k_info[o_ptr->k_idx].level;
		msg_format("k_info Lev:(%d)",power);
		if(object_is_artifact(o_ptr)) power += 50;
		else if(object_is_ego(o_ptr)) power += e_info[o_ptr->name2].rarity;
		power += flag_cost(o_ptr,o_ptr->pval,FALSE) / 1000;
		msg_format("Raiko_power:(%d)",power);

	}

	msg_format("flag_cost:(%d)",flag_cost(o_ptr,o_ptr->pval,FALSE));
	if(p_ptr->pclass == CLASS_JUNKO)
		msg_format("flag_cost(all):(%d)",flag_cost(o_ptr,o_ptr->pval,TRUE));
	msg_format("score_value:(%d)",o_ptr->score_value);
	msg_format("create_turn:(%d)",o_ptr->create_turn);
	msg_format("create_lev:(%d)",o_ptr->create_lev);


}





/*:::デバッグ用処理領域の追加*/
static void do_cmd_wiz_hack_ben_2(void)
{
	int i,j;
	monster_type *m_ptr;
	monster_race *r_ptr;
	int tot=0;
	object_type *q_ptr;
	object_type *o_ptr;
	object_type forge;
	char desc[160];

	/*
	//移動禁止テスト
	if (!p_ptr->tim_no_move)
	{
		set_no_move(100);
	}
	else
	{
		set_no_move(0);
	}
	*/

	//千亦　カードランクMAX
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		for (i = 0; i < ABILITY_CARD_LIST_LEN; i++) p_ptr->magic_num2[i] = 1;
		return;
	}

	//尤魔　空腹で倒れる直前にする
	if (p_ptr->pclass == CLASS_YUMA)
	{
		p_ptr->magic_num2[200] = 0;
		set_food(100);
		return;
	}

	if (gv_test1 > 0)
	{
		msg_format("try:%d success:%d avg_chance = %d.%d", gv_test1, gv_test2, gv_test3 / gv_test1 / 10, (gv_test3 / gv_test1) % 10);
		gv_test1 = 0;
		gv_test2 = 0;
		gv_test3 = 0;

	}

//	get_score_server_name();
//	send_world_score(TRUE);


//	select_floor_music();
//	return;

	if (p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_3_FAIRIES)
	{
		msg_format(_("日光ポイント:%d", "Nikko points: %d"), p_ptr->magic_num1[0]);
		return;
	}


	if (p_ptr->pclass == CLASS_SUWAKO)
	{
		p_ptr->magic_num1[0] += 10000;
		msg_print(_("畏れポイント+10000", "+10000 fear points"));
	}

	//カード売人のアビリティカード高騰度変化テスト
	if (p_ptr->pclass == CLASS_CARD_DEALER)
	{
		int miss_num = 0;
		int min = 10000;
		int	max = 0;
		int avg = 0;
		int tot = 0;
		int hdays = 56;
		int count = 10000;
		for (i = 0; i < count; i++)
		{
			int tmp = 10;
			int j;

			for (j = 1; j < hdays; j++)
			{
				if (j < 6) tmp += 1;
				else if (j < 14) tmp += 1+randint1(2);
				else if(j<28)tmp += tmp * 14 / 100 + randint1(tmp * 6 / 100);
				else
				{
					int mod = damroll(3, 100) - 150;

					if ((tmp + mod) < 300)
					{
						if (!one_in_(3)) mod *= -1; //あまり下には張り付きにくい
						else mod = tmp - 300; //派手に計算間違ってた
						//else mod = 300 - tmp;
					}
					else if ((tmp + mod) > 600)
					{
						mod = 600 - tmp;
					}

					tmp += mod;

					if (tmp < 0) break;

				}

			}
			tot += tmp;
			if (max < tmp) max = tmp;
			if (min > tmp) min = tmp;

			if (tmp < 300) miss_num++;

		}
		avg = tot / count;

		msg_format("hdays:%d min:%d max:%d avg:%d ",hdays,min,max,avg);
		msg_format("miss:%d ", miss_num);

	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
	{
		for(i=0;i<=p_ptr->lev / 7;i++)
			okina_make_nameless_arts(i);
		return;
	}

	if(p_ptr->pclass == CLASS_NEMUNO)
	{
		if(cave[py][px].info & CAVE_ROOM)
			msg_print("room:");

		msg_format("flag:%x x1:%d x2:%d",cave[py][px].cave_xtra_flag,cave[py][px].cave_xtra1,cave[py][px].cave_xtra2);

	}

	if(p_ptr->pclass == CLASS_JUNKO)
	{
		for(j=0;j<11;j++)
		{
			for(i=0;i<10;i++)
			{
				int num = j*10+i;
				if(num > 107) return;
				msg_format("[%d:%d]",num,p_ptr->magic_num2[num]);

			}
			inkey();
		}

	}

	if(p_ptr->pclass == CLASS_SOLDIER)
	{
		gain_soldier_skill();
	}
	//return;
/*
	for(j=0;j<o_max;j++)
	{
		o_ptr = &o_list[j];
		if (!o_ptr->k_idx) continue;
		object_desc(desc, o_ptr, 0);
		msg_format("(%d,%d):%s",o_ptr->iy,o_ptr->ix,desc);
	}
*/
/*
	for(i=1;i<max_a_idx;i++)
	{
		if(a_info[i].gen_flags & TRG_QUESTITEM) msg_format("QUESTITEM:%d",i);
	}
	return;
*/
/*
	if(p_ptr->pclass == CLASS_KAGUYA)
	{
		char tmp_val[160];
		int tmp;

		sprintf(tmp_val, "0");
		if (!get_string(">>", tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > 5) tmp=5;
		kaguya_quest_level = tmp;

		return;
	}
*/

	if(p_ptr->prace == RACE_STRAYGOD)
	{
		char tmp_val[160];
		int tmp;

		msg_print(_("野良神様神性値強制設定", "Override stray god divinity values"));
		sprintf(tmp_val, "0");
		if (!get_string(_("秩序値: ", "Order value: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > 255) tmp=255;
		p_ptr->race_multipur_val[0] = tmp;

		sprintf(tmp_val, "0");
		if (!get_string(_("好戦値: ", "Good fight value: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > 255) tmp=255;
		p_ptr->race_multipur_val[1] = tmp;

		sprintf(tmp_val, "0");
		if (!get_string(_("名声値: ", "Fame: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > 255) tmp=255;
		p_ptr->race_multipur_val[2] = tmp;

	}
	else if(p_ptr->prace == RACE_FAIRY)
	{
		char tmp_val[160];
		int tmp;

		msg_format("%d:%s",CHECK_FAIRY_TYPE,fairy_table[CHECK_FAIRY_TYPE].fairy_desc);
		sprintf(tmp_val, "%d",CHECK_FAIRY_TYPE);
		if (!get_string(_("新特性: ", "New features: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<1) tmp=1;
		if(tmp > FAIRY_TYPE_MAX) tmp=FAIRY_TYPE_MAX;
		p_ptr->race_multipur_val[0] = tmp;

	}

	else if(p_ptr->pclass == CLASS_SEIJA)
	{
		int i;

		for(i=0;i<MAGIC_NUM_SIZE_OLD;i++) msg_format("magic_num%d:%d/%d",i,p_ptr->magic_num1[i],p_ptr->magic_num2[i]);


	}
	else if(p_ptr->pclass == CLASS_KANAKO)
	{
		char tmp_val[160] = "";
		int tmp;

		if (!get_string(_("信仰値: ", "Faith value: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > 20000) tmp=20000;
		p_ptr->magic_num1[0] = tmp;
		p_ptr->update |= PU_BONUS;
		p_ptr->redraw |= PR_STATUS;
		handle_stuff();
	}
	else if(p_ptr->pclass == CLASS_HINA)
	{
		char tmp_val[160] = "";
		int tmp;

		if (!get_string(_("厄値: ", "Bad value: "), tmp_val, 9)) return;
		tmp = atol(tmp_val);
		if(tmp<0) tmp=0;
		if(tmp > HINA_YAKU_MAX) tmp = HINA_YAKU_MAX;
		p_ptr->magic_num1[0] = tmp;
		p_ptr->update |= PU_BONUS;
		p_ptr->redraw |= PR_STATUS;
		handle_stuff();
	}
	else if (p_ptr->pclass == CLASS_SHION)
	{
		//スーパー貧乏神に変身
		set_tim_general(9, FALSE, 0, 0);


	}
	//p_ptr->magic_num1[]の中身表示
	else if (p_ptr->pclass == CLASS_KEIKI)
	{

		for (i = 0; i < 22; i++)
		{
			int tmp = i * 5;
			if(i == 21)
				msg_format("%03d) %5d %5d %5d",tmp, p_ptr->magic_num1[tmp], p_ptr->magic_num1[tmp +1], p_ptr->magic_num1[tmp +2]);
			else
				msg_format("%03d) %5d %5d %5d %5d %5d", tmp, p_ptr->magic_num1[tmp], p_ptr->magic_num1[tmp + 1], p_ptr->magic_num1[tmp + 2], p_ptr->magic_num1[tmp + 3], p_ptr->magic_num1[tmp + 4]);


		}


	}

	//v1.1.82 弾幕研究家
	//全特技習得済みと全特技未習得を切り替える。関係ないmagic_num値も雑に変えている。
	if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		static int tester=0;

		tester++;
		for (i = 1; i < 100; i++)
		{
			if (tester%2)
			{
				p_ptr->magic_num1[i] = 100;
				p_ptr->magic_num2[i] = 100;
			}
			else
			{
				p_ptr->magic_num1[i] = 0;
				p_ptr->magic_num2[i] = 0;
			}
		}
		msg_print("magic_exp toggle");


	}


	//msg_format("py:%d px:%d",py,px);
	/*
	if(inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].xtra1 < 100)
	{
		inventory[INVEN_RARM].xtra1 += 49;
		msg_print("tweak xtra1");
	}
	*/
	/*
	for(i=1;i<max_r_idx;i++)
	{
		r_ptr = &r_info[i];
		if(!(r_ptr->flags1 & RF1_UNIQUE)) continue;
		tot += r_ptr->level;
	}
	msg_format("total enemy lev:%d",tot);
	*/

}


/*
 * Roll the hitdie -- aux of do_cmd_rerate()
 */
/*:::p_ptr->hitdieを参照し、p_ptr->player_hp[0～50]を決定する*/
void do_cmd_rerate_aux(void)
{
	/*:::Lv50時のHPは平均値±25%辺りに制限されるらしい HD10で70程度*/
	///system フレーバーのためにこの制限を取っ払っても面白いかもしれない。25%から外れることはほとんどないだろうが。
	/* Minimum hitpoints at highest level */
	int min_value = p_ptr->hitdie + ((PY_MAX_LEVEL + 2) * (p_ptr->hitdie + 1)) * 3 / 8;

	/* Maximum hitpoints at highest level */
	int max_value = p_ptr->hitdie + ((PY_MAX_LEVEL + 2) * (p_ptr->hitdie + 1)) * 5 / 8;

	int i;

	/* Rerate */
	while (1)
	{
		/* Pre-calculate level 1 hitdice */
		/*:::レベル1のHP:HD+3d(HD)*/
		p_ptr->player_hp[0] = p_ptr->hitdie;

		for (i = 1; i < 4; i++)
		{
			p_ptr->player_hp[0] += randint1(p_ptr->hitdie);
		}

		/* Roll the hitpoint values */
		/*:::さらにLVごとに1d(HD)加算*/
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			p_ptr->player_hp[i] = p_ptr->player_hp[i - 1] + randint1(p_ptr->hitdie);
		}

		/* Require "valid" hitpoints at highest level */
		if ((p_ptr->player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (p_ptr->player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}
}


/*
 * Hack -- Rerate Hitpoints
 */
/*:::体力ランクを再計算する*/

void do_cmd_rerate(bool display)
{
	int percent;

	/* Rerate */
	do_cmd_rerate_aux();

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * p_ptr->hitdie +
		((PY_MAX_LEVEL - 1+3) * (p_ptr->hitdie + 1))));


	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	if (display)
	{
#ifdef JP
		msg_format("現在の体力ランクは %d/100 です。", percent);
#else
		msg_format("Your life rate is %d/100 now.", percent);
#endif
		p_ptr->knowledge |= KNOW_HPRATE;
	}
	else
	{
#ifdef JP
		msg_print("体力ランクが変わった。");
#else
		msg_print("Life rate is changed.");
#endif
		p_ptr->knowledge &= ~(KNOW_HPRATE);
	}
}


#ifdef ALLOW_WIZARD

/*
 * Dimension Door
 */
static bool wiz_dimension_door(void)
{
	int	x = 0, y = 0;

	if (!tgt_pt(&x, &y)) return FALSE;

	teleport_player_to(y, x, TELEPORT_ANYPLACE);//v2.0.11 NONMAGICALからANYPLACEにした

	return (TRUE);
}


/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_create_named_art(void)
{
	char tmp_val[10] = "";
	int a_idx;

	/* Query */
	if (!get_string("Artifact ID:", tmp_val, 3)) return;

	/* Extract */
	a_idx = atoi(tmp_val);
	if(a_idx < 0) a_idx = 0;
	if(a_idx >= max_a_idx) a_idx = 0;

	/* Create the artifact */
	(void)create_named_art(a_idx, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
/*:::テスト用に様々なコマンドを入れる場所*/
static void do_cmd_wiz_hack_ben(void)
{
	int powers;
	int i,j;
	int pmax,pavg,count0,count10,count20,count30,count_ex;
	int count25=0;
	monster_type *m_ptr;
	monster_race *r_ptr;
	char m_name[80];
	int tot;
	int power;
	int dir;
	long test;

    (void)probing();

	msg_format("yakuzaII_target_r_idx:%d", quest[QUEST_YAKUZA_2].r_idx);

	if (r_info[MON_BITEN].flags1 & RF1_QUESTOR) msg_print("biten_questor");
	if (r_info[MON_BITEN].flags3 & RF3_WANTED) msg_print("biten_wanted");

	msg_format("yakuzaII_stat:%d", quest[QUEST_YAKUZA_2].status);

	/*
	msg_format("today_mon:%d", p_ptr->today_mon);
	msg_format("hatate_mon:%d", p_ptr->hatate_mon_search_ridx);
	msg_format("hatate_dun:%d", p_ptr->hatate_mon_search_dungeon);
	msg_format("food:%d", p_ptr->food);
	if(p_ptr->pclass == CLASS_YUMA) msg_format("foodstock:%d", p_ptr->magic_num2[200]);
	*/

	return;

	msg_format("testmsg:o_cnt:%d", o_cnt);

	for (i = 0; i < MAGIC_NUM_SIZE; i++)
	{
		msg_format("magic_num%d:%d/%d", i, p_ptr->magic_num1[i], p_ptr->magic_num2[i]);
		if (i % 10 == 0) msg_print("     ");
	}



	msg_format("turn:%d", turn);
	msg_format("taisai_comptime:%d", quest[QUEST_TAISAI].comptime);



	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
	{
		msg_format("magic_num1[6]:%d", p_ptr->magic_num1[6]);
	}

	if (p_ptr->pclass == CLASS_KANAKO)
	{
		msg_format("magic_num1[0]:%d", p_ptr->magic_num1[0]);
	}

	msg_format("dun_level:%d", dun_level);
//	msg_format("turn: %d  dun_turn:%d", turn, dungeon_turn);


	//for(i=0;i<10;i++)
	//msg_format("magic_num1[%d]：%d",i,p_ptr->magic_num1[i]);

	msg_format("total_pay: %d", total_pay_cardshop);
	msg_format("box_count: %d", buy_gacha_box_count);


	msg_format("show1: (%d,%d)", p_ptr->dis_to_h[0], p_ptr->dis_to_d[0]);
	msg_format("real1: (%d,%d)", p_ptr->to_h[0], p_ptr->to_d[0]);
	msg_format("show2: (%d,%d)", p_ptr->dis_to_h[1], p_ptr->dis_to_d[1]);
	msg_format("real2: (%d,%d)", p_ptr->to_h[1], p_ptr->to_d[1]);
	/*
	*/

	//msg_format("turn: %d  recall:%d", turn, p_ptr->word_recall);
	msg_format("alcohol: %d",p_ptr->alcohol);
	//msg_format("cut:%d stun:%d broken:%d", p_ptr->cut, p_ptr->stun, p_ptr->race_multipur_val[0]);



	return;


	msg_format("max_towns:%d", max_towns);

	msg_format("wild_x:%d", p_ptr->wilderness_x);
	msg_format("wild_y:%d", p_ptr->wilderness_y);

	msg_format("ver:%d.%d.%d.%d", start_ver[0], start_ver[1], start_ver[2], start_ver[3]);

	msg_format("town_num:%d", p_ptr->town_num);
	msg_format("music_r_idx:%d", music_r_idx);


	if(p_ptr->prace == RACE_FAIRY)
		msg_format("idx:%d sc:%d prev_sc:%d",p_ptr->race_multipur_val[0],p_ptr->sc,previous_char.sc);


	msg_format("attr:%x char:%x", r_info[0].x_attr, r_info[0].x_char);

	msg_format("old_hp:%d", nightmare_record_hp);


	msg_format("points:%lu",(unsigned long)total_points_new(TRUE));

	//msg_format("gain:%d",gambling_gain);
	//msg_format("pinch:%d",score_count_pinch);
	msg_format("icky:%d",character_icky);
	msg_format("max_d_idx:%d", max_d_idx);



	//msg_format(" Score-name:%s(len:%d) ",score_server_name,strlen(score_server_name));

//	item_recall(0);

	//msg_format("quark__num:%d",quark__num);

	//msg_format("UNIQUE2_SCORE:%d",score_bonus_kill_unique2);
	//if(p_ptr->pclass == CLASS_RINGO) msg_format("支援物資投下カウンタ：%d",p_ptr->magic_num1[4]);

	//set_shield(20000,FALSE);

//	msg_format("max_m_idx:%d m_max:%d m_cnt:%d",max_m_idx,m_max,m_cnt);

	//msg_format("cave_info:%x",cave[py][px].info);
	//if(cave[py][px].info & CAVE_GLOW) msg_print("grow");
	/*
	msg_format("ru_1:%s",random_unique_name[0]);
	msg_format("ru_2:%s",random_unique_name[1]);
	msg_format("ru_3:%s",random_unique_name[2]);
	*/

	//msg_format("check_v:%d",testcount2);
//	msg_format("check_x:%d",testcount);
//	msg_format("max_quests:%d",max_quests);
//	msg_format("message_num:%d",message_num());
//	msg_format("p_ptr->to_h_b:%d",p_ptr->to_h_b);
//	msg_format("p_ptr->skill_thb:%d",p_ptr->skill_thb);
	msg_format("turn:%d",turn);
	msg_format("dungeon_turn:%d", dungeon_turn);
	msg_format("inside_quest:%d",p_ptr->inside_quest);
	msg_format("inside_arena:%d", p_ptr->inside_arena);

//	msg_format("floor_id:%d",p_ptr->floor_id );
	msg_format("skill_dev:%d",p_ptr->skill_dev);
	msg_format("skill_dis:%d", p_ptr->skill_dis);
	msg_format("skill_thb:%d",p_ptr->skill_thb);
	//msg_format("to_h_b*3:%d",p_ptr->to_h_b * 3);

	//msg_format("realm:%d/%d",p_ptr->realm1,p_ptr->realm2);
	//msg_format("monkey:%d",p_ptr->monkey_count);

	//msg_format("hassyaku_target:%d",hassyakusama_target_idx);

	//if(p_ptr->see_inv) msg_print("SEE_INV:TRUE");

	//msg_format("kaguya_quest:%d",kaguya_quest_level);
	//msg_format("grassroots:%d  value:%d",p_ptr->grassroots,p_ptr->barter_value);

	//msg_format("Hetate_HP:%d/%d/%d",hecatia_hp[0],hecatia_hp[1],hecatia_hp[2]);

	if(p_ptr->pclass == CLASS_SUWAKO) msg_format(_("畏れポイント：%d", "Fear point: %d"), p_ptr->magic_num1[0]);
	if(p_ptr->pclass == CLASS_KANAKO) msg_format(_("信仰ポイント：%d", "Faith point: %d"), p_ptr->magic_num1[0]);

	//msg_format("k_idx:%d",inventory[INVEN_RARM].k_idx);
	//msg_format("flavor:%d",k_info[inventory[INVEN_RARM].k_idx].flavor);
	//msg_format("x_attr:%d",k_info[inventory[INVEN_RARM].k_idx].x_attr);

	//msg_format("Osisen:%d",osaisen);
	/*
	tot = 0;
	for(i=0;i<1000;i++)
	{
		while(randint0(10))   tot++;
	}
	msg_format("Test:%d/1000",tot);
	*/
/*
	pmax = 0;
	pavg = 0;
	if(p_ptr->prace == RACE_STRAYGOD)
	{
		msg_format("秩序：%d",p_ptr->race_multipur_val[DBIAS_COSMOS]);
		msg_format("好戦：%d",p_ptr->race_multipur_val[DBIAS_WARLIKE]);
		msg_format("名声：%d",p_ptr->race_multipur_val[DBIAS_REPUTATION]);
		msg_format("現タイプ：%d",p_ptr->race_multipur_val[3]);
	}
*/

	//for(i=0;i<8;i++) msg_format("Magic%d:%d",i,marisa_magic_power[i]);


	/*
	for(j=1;j<11;j++)
	{
	count0=0;
	count10=0;
	count20=0;
	count30=0;
	count_ex=0;
	for(i=0;i<10000;i++)
	{
	power = j*10 / 2;
	while(!one_in_(3)) power += 10;
	while(power < 1000 && (weird_luck() || weird_luck() || weird_luck())) power *= 2;
	//if(weird_luck()) power *= 5;
	power += randint1(10);
	if(power > 1000) power = 1000;

	if(power<20) count0++;
	else if(power<60) count10++;
	else if(power<90) count20++;
	else if(power<100) count30++;
	else  count_ex++;
	}

	msg_format("LEV:%d 30:%d 50:%d 80:%d 100:%d EX:%d",j*10,count0,count10,count20,count30,count_ex);
	}
	*/

	//msg_format("kamioroshi:%x",p_ptr->kamioroshi);

	//msg_print("test:mut");
	//gain_random_mutation(0);

	//msg_format("wild_level:%d",wilderness[py][px].level );

	//msg_format("asthma:%d",p_ptr->asthma );
	msg_format("dungeon_type:%d",dungeon_type );
	msg_format("base_level:%d",base_level );
	msg_format("monster_level:%d",monster_level );

	//for(i=0;i<10000;i++) if(weird_luck()) count0++;

	//msg_format("WEIRD_LUCK:%d",count0);
	//msg_format("OBJlev:%d",object_level);

	//msg_format("special:%d",cave[py][px].special);

	//msg_format("Search:%d",p_ptr->skill_srh);
	//msg_format("Stealth:%d", p_ptr->skill_stl);
	//msg_format("Silent:%d",p_ptr->silent_floor);
	//msg_format("Maripo:%d",p_ptr->maripo);
	//tele_town(TRUE);
	//msg_format("NO★？:%s",ironman_no_fixed_art?"TRUE":"FALSE");
	//msg_format("NO capture？:%s",no_capture_book?"TRUE":"FALSE");

	//msg_format("Serpen?:%d",quest[QUEST_SERPENT].status);

	//msg_format("grassroots:%d  value:%d",p_ptr->grassroots,p_ptr->barter_value);
	//process_world_aux_alcohol();

	//msg_format("turn:%d bydo:%d",turn,p_ptr->bydo);

	//change_race(randint0(MAX_RACES),"");
/*
	msg_format("究極の生命体%sの誕生だッーっ！」",player_name);
		if (p_ptr->prace < 32)
	{
		p_ptr->old_race1 |= 1L << p_ptr->prace;
	}
	else
	{
		p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
	}
	p_ptr->prace = RACE_ULTIMATE;
	rp_ptr = &race_info[p_ptr->prace];
	p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;
	if (p_ptr->pclass == CLASS_SORCERER)
		p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
	else
		p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

	do_cmd_rerate(FALSE);
	check_experience();
	p_ptr->redraw |= (PR_BASIC);
	p_ptr->update |= (PU_BONUS);
	handle_stuff();
	lite_spot(py, px);
*/
	//set_mimic(100, MIMIC_GIGANTIC, FALSE);

	//msg_format("泥酔度:%d",p_ptr->alcohol);

//	msg_format("Difficulty:%d",difficulty);

	//for(i=0;i<max_r_idx;i++) 	msg_format("ID:%d Ex:%d",i,r_info[i].extra);


	/*
	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		monster_desc(m_name, m_ptr, 0);
		msg_format("%d(%d,%d):%s",i,m_ptr->fx,m_ptr->fy,m_name);

	}
		msg_format("m_max:%d",m_max);
	*/


	//msg_format("magicmaster:%d",cp_ptr->magicmaster);

	//msg_format("Mrate:%d",cp_ptr->mana_rate);
	//msg_format("adj-mana:%d",adj_mag_mana[p_ptr->stat_ind[A_WIS]]);

	//display_rumor_new(0);
	//msg_format("Food:%d",p_ptr->food);
	//msg_format("STAT_CUR_CON:%d",p_ptr->stat_cur[A_CON]);

	//msg_format("WEIRD:%d",weird_luck());
	//msg_format("ObjLev:%d",object_level);
	//msg_format("Sav:%d",p_ptr->skill_sav);
	//msg_format("Wgt:%d",p_ptr->total_weight);
	//msg_format("Stun:%d",p_ptr->stun);
	/*
	msg_format("StatMaxMax:%d",p_ptr->stat_max_max[A_STR]);
	msg_print(NULL);
	msg_format("StatAdd:%d",p_ptr->stat_add[A_STR]);
	msg_print(NULL);
	msg_format("StatMax:%d",p_ptr->stat_max[A_STR]);
	msg_print(NULL);
	msg_format("StatCur:%d",p_ptr->stat_cur[A_STR]);
	msg_print(NULL);
	msg_format("StatInd:%d",p_ptr->stat_ind[A_STR]);
	msg_print(NULL);
	msg_format("StatTop:%d",p_ptr->stat_top[A_STR]);
	msg_print(NULL);
	msg_format("StatUse:%d",p_ptr->stat_use[A_STR]);
	msg_print(NULL);
	*/
#if 0
	for(i=0;i<100000;i++){

		// ☆生成powersテスト
		powers = randint1(5) + 1;
		while (one_in_(powers) || one_in_(7) || one_in_(10) || weird_luck())
			powers++;

		powers += m_bonus(5,object_level);

		if (weird_luck())
			powers *= 2;
		/*
		//ボーナスロールテスト
		powers = damroll(3,6);
		while(1){
			if(powers == 99) break;
			if(one_in_(3)) break;
			powers++;
		}
		*/

		if(powers<10) count0++;
		else if(powers<20) count10++;
		else if(powers<30) count20++;
		else if(powers<40) count30++;
		else count_ex++;

		if(powers>24) count25++;
		if(pmax<powers) pmax = powers;
		pavg += powers;
	}
	pavg /= 100000;

	msg_format("0-10:%d", count0);
	msg_format("10-20:%d", count10);
	msg_format("25:%d", count25);
	msg_format("20-30:%d", count20);
	msg_format("30-40:%d", count30);
	msg_format("40-:%d", count_ex);
	msg_format("MAX:%d", pmax);
	msg_format("AVG:%d", pavg);
#endif
	//msg_format("upkeepfactor:%d", calculate_upkeep());

	/* Oops */
	//msg_print("Oops.");
	(void)probing();


	nightmare_total_point += 100000;
	msg_format("nightmare_total_point:%d", nightmare_total_point);
	msg_format("tmp_nightmare_point:%d", tmp_nightmare_point);
	msg_format("nightmare_diary_count:%d", nightmare_diary_count);
	msg_format("nightmare_diary_win:%d", nightmare_diary_win);
	msg_format("nightmare_mon_base_level:%d", nightmare_mon_base_level);

}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
/*:::モンスターの起きている集団を生成する*/
static void do_cmd_summon_horde(void)
{
	int wy = py, wx = px;
	int attempts = 1000;

	while (--attempts)
	{
		scatter(&wy, &wx, py, px, 3, 0);
		if (cave_empty_bold(wy, wx)) break;
	}

	(void)alloc_horde(wy, wx);
}

#endif /* MONSTER_HORDES */


/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_RED, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_L_DARK, '-');
		}
	}
}


#define K_MAX_DEPTH 110

/*
 * Output a rarity graph for a type of object.
 */
/*:::詳細不明　ウィザードコマンドのアイテム操作のとき左側にベースアイテムのレア度を表示？*/
///mod131229 ウィザードコマンド関係　色とか少し変更
static void prt_alloc(byte tval, byte sval, int row, int col)
{
	int i, j;
	int home = 0;
	u32b maxr = 1, maxt = 1, ratio;
	u32b rarity[K_MAX_DEPTH];
	u32b total[K_MAX_DEPTH];
	s32b maxd = 1, display[22];
	byte c = TERM_WHITE;
	cptr r = "+--common--+";
	object_kind *k_ptr;


	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Wipe the tables */
	(void)C_WIPE(rarity, K_MAX_DEPTH, u32b);
	(void)C_WIPE(total, K_MAX_DEPTH, u32b);
	(void)C_WIPE(display, 22, s32b);

	/* Scan all entries */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		int total_frac = 0;
		for (j = 0; j < alloc_kind_size; j++)
		{
			int prob = 0;

			if (table[j].level <= i)
			{
				prob = table[j].prob1 * GREAT_OBJ * K_MAX_DEPTH;
			}
			else if (table[j].level - 1 > 0)
			{
				prob = table[j].prob1 * i * K_MAX_DEPTH / (table[j].level - 1);
			}

			/* Acquire this kind */
			k_ptr = &k_info[table[j].index];

			/* Accumulate probabilities */
			total[i] += prob / (GREAT_OBJ * K_MAX_DEPTH);
			total_frac += prob % (GREAT_OBJ * K_MAX_DEPTH);

			/* Accumulate probabilities */
			if ((k_ptr->tval == tval) && (k_ptr->sval == sval))
			{
				home = k_ptr->level;
				rarity[i] += prob;
			}
		}
		total[i] += total_frac / (GREAT_OBJ * K_MAX_DEPTH);
	}

	/* Find maxima */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		if (rarity[i] > maxr) maxr = rarity[i];
		if (total[i] > maxt) maxt = total[i];
	}

	if (maxr / (GREAT_OBJ * K_MAX_DEPTH) != 0)
		ratio = maxt / (maxr / (GREAT_OBJ * K_MAX_DEPTH));
	else
		ratio = 99999L;

	/* Simulate a log graph */
	if (ratio > 1000)
	{
		c = TERM_L_WHITE;
		r = "+-uncommon-+";
	}
	if (ratio > 3000)
	{
		c = TERM_YELLOW;
		r = "+---rare---+";
	}
	if (ratio > 32768L)
	{
		c = TERM_RED;
		r = "+-VeryRare-+";
	}

	/* Calculate probabilities for each range */
	for (i = 0; i < 22; i++)
	{
		/* Shift the values into view */

		int possibility = 0;
		for (j = i * K_MAX_DEPTH / 22; j < (i + 1) * K_MAX_DEPTH / 22; j++)
			possibility += rarity[j] * (100 * maxt / total[j]);

		possibility = possibility / maxr;

		/* display[i] = log_{sqrt(2)}(possibility) */
		display[i] = 0;
		while (possibility)
		{
			display[i]++;
			possibility = possibility * 1000 / 1414;
		}

		/* Track maximum */
		if (display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	if (maxd > 10) for (i = 0; i < 22; i++)
	{
		display[i] = display[i] - maxd + 10;
	}

	/* Graph the rarities */
	for (i = 0; i < 22; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE,  '|');

		prt(format("%d", (i * K_MAX_DEPTH / 220) % 10), row + i + 1, col);

		if (display[i] <= 0)
			continue;

		/* Note the level */
		if ((i * K_MAX_DEPTH / 22 <= home) && (home < (i + 1) * K_MAX_DEPTH / 22))
		{
			c_prt(TERM_RED, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
		else
		{
			c_prt(c, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
	}

	/* Make it look nice */
	prt(r, row, col);
}

static void do_cmd_wiz_reset_class(void)
{
	int tmp_int;
	char tmp_val[160];
	char ppp[80];

	/* Prompt */
	sprintf(ppp, "Class (0-%d): ", MAX_CLASS - 1);

	/* Default */
	sprintf(tmp_val, "%d", p_ptr->pclass);

	/* Query */
	if (!get_string(ppp, tmp_val, 2)) return;

	/* Extract */
	tmp_int = atoi(tmp_val);

	/* Verify */
	if (tmp_int < 0 || tmp_int >= MAX_CLASS) return;

	/* Save it */
	p_ptr->pclass = tmp_int;

	/* Redraw inscription */
	p_ptr->window |= (PW_PLAYER);

	/* {.} and {$} effect p_ptr->warning and TRC_TELEPORT_SELF */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	update_stuff();
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!target_who) return;

	/* Teleport to the target */
	teleport_player_to(target_row, target_col, TELEPORT_NONMAGICAL);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
/*:::＠のパラメータ操作*/
///sys パラメータ操作、熟練度関係、
static void do_cmd_wiz_change_aux(void)
{
	int i, j;
	int tmp_int;
	long tmp_long;
	s16b tmp_s16b;
	char tmp_val[160];
	char ppp[80];


	/* Query the stats */
	for (i = 0; i < 6; i++)
	{
		/* Prompt */
		sprintf(ppp, "%s (3-%d): ", stat_names[i], p_ptr->stat_max_max[i]);

		/* Default */
		sprintf(tmp_val, "%d", p_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 3)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > p_ptr->stat_max_max[i]) tmp_int = p_ptr->stat_max_max[i];
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	sprintf(tmp_val, "%d", WEAPON_EXP_MASTER);

	/* Query */
#ifdef JP
	if (!get_string("熟練度: ", tmp_val, 9)) return;
#else
	if (!get_string("Proficiency: ", tmp_val, 9)) return;
#endif

	/* Extract */
	tmp_s16b = atoi(tmp_val);

	/* Verify */
	if (tmp_s16b < WEAPON_EXP_UNSKILLED) tmp_s16b = WEAPON_EXP_UNSKILLED;
	if (tmp_s16b > WEAPON_EXP_MASTER) tmp_s16b = WEAPON_EXP_MASTER;
///dell131227 武器技能weapon_exp[][]は廃止
/*
	for (j = 0; j <= TV_WEAPON_END - TV_WEAPON_BEGIN; j++)
	{

		for (i = 0;i < 64;i++)
		{
			p_ptr->weapon_exp[j][i] = tmp_s16b;
			if (p_ptr->weapon_exp[j][i] > s_info[p_ptr->pclass].w_max[j][i]) p_ptr->weapon_exp[j][i] = s_info[p_ptr->pclass].w_max[j][i];
		}
	}
*/

	///mod140130 ウィザードモードの技能値指定　新しい技能値上限に対応
	for (j = 0; j < SKILL_EXP_MAX; j++)
	{
		int skill_max = cp_ptr->skill_aptitude[j] * SKILL_LEV_TICK * 10;
		p_ptr->skill_exp[j] = tmp_s16b;
		//if (p_ptr->skill_exp[j] > s_info[p_ptr->pclass].s_max[j]) p_ptr->skill_exp[j] = s_info[p_ptr->pclass].s_max[j];
		if(p_ptr->skill_exp[j] > skill_max) p_ptr->skill_exp[j] = skill_max;
	}


	for (j = 0; j < 32; j++)
		p_ptr->spell_exp[j] = (tmp_s16b > SPELL_EXP_MASTER ? SPELL_EXP_MASTER : tmp_s16b);
	for (; j < 64; j++)
		p_ptr->spell_exp[j] = (tmp_s16b > SPELL_EXP_EXPERT ? SPELL_EXP_EXPERT : tmp_s16b);

	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	//v1.1.48 マイナスの金額も設定できるようにした
	//if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	if (p_ptr->prace != RACE_ANDROID)
	{
		/* Save */
		p_ptr->max_exp = tmp_long;
		p_ptr->exp = tmp_long;

		/* Update */
		check_experience();
	}
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(void)
{
	/* Interact */
	do_cmd_wiz_change_aux();

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
/*:::ウィザードコマンドアイテム操作用のアイテム表示欄*/
///item res debug ウィザードモードのoコマンド
static void wiz_display_item(object_type *o_ptr)
{
	int i, j = 13;
	u32b flgs[TR_FLAG_SIZE];
	char buf[256];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	prt_alloc(o_ptr->tval, o_ptr->sval, 1, 0);

	/* Describe fully */
	object_desc(buf, o_ptr, OD_STORE);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
		   o_ptr->k_idx, k_info[o_ptr->k_idx].level,
		   o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
		   o_ptr->number, o_ptr->weight,
		   o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
		   o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
		   o_ptr->name1, o_ptr->name2, (long)object_value_real(o_ptr)), 7, j);

	prt(format("ident = %04x  xtra1 = %-4d  xtra2 = %-4d  timeout = %-d",
		   o_ptr->ident, o_ptr->xtra1, o_ptr->xtra2, o_ptr->timeout), 8, j);

	prt(format("xtra3 = %-4d  xtra4 = %-4d  xtra5 = %-4d  cursed  = %-d",
		   o_ptr->xtra3, o_ptr->xtra4, o_ptr->xtra5, o_ptr->curse_flags), 9, j);

	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT.......BRAND......SLAY....", 11, j);
	prt("      m s    cvfvXqpaefcegahdd d", 12, j);
	prt("siwdccaseidsahaoovaocliovonuerge", 13, j);
	prt("tnieohgtrniptamcrokiierlioimaaom", 14, j);
	prt("rtsxnaDlcfgdkopepresdcedldmadgdo", 15, j);
	prt_binary(flgs[0], 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt(".KILL.....ESP........SUST..  m t", 18, j);
	prt("kegahdd dkegahdd dknu      emish", 19, j);
	prt("wvonuergewvonuergewonsiwdccaaghr", 20, j);
	prt("aioimaaomaioimaaomalitnieohsnhoo", 21, j);
	prt("ildmadgdoildmadgdoiiqrtsxnayattw", 22, j);
	prt_binary(flgs[1], 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt(" r****RESIST.........POWER......", 11, j+32);
	prt("reaefcaefcpldssnwcdhtffbcilvdrlw", 12, j+32);
	prt("ifcliocliooiahoeahioireloneiieia", 13, j+32);
	prt("dlierlierlitrruttaslmeannsvsggtr", 14, j+32);
	prt("eedceddcedsekddhroeyeerdfaiieeen", 15, j+32);
	prt_binary(flgs[2], 16, j+32);

	prt("+------------FLAGS4------------+", 17, j+32);
	prt(".....[[[nnCURSE....A            ", 18, j+32);
	prt("bt2  fecto a aaiiiiCM BR   hnF  ", 19, j+32);
	prt("lewreilolm geddggggTAUOE  miaf D", 20, j+32);
	prt("eleusrelpaTgxdd....ITSOA  odmi i", 21, j+32);
	prt("sepnpecdogYrpLHaefcVEEMD..deex s", 22, j+32);
	prt_binary(flgs[3], 23, j+32);

	prt("+------------FLAGS5------------+", 10, j + 64);
	prt("                                ", 11, j + 64);
	prt("                                ", 12, j + 64);
	prt("S                               ", 13, j + 64);
	prt("a                               ", 14, j + 64);
	prt("v                               ", 15, j + 64);
	prt_binary(flgs[4], 16, j + 64);

}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
/*:::ウィザードコマンドのアイテム生成から使うTVAL一覧　最後は0にしておくこと*/
///item debug TVAL
///mod131223 TVAL総とっかえ
static tval_desc tvals[] =
{

	{ TV_BOOK_ELEMENT	,	"Elem_book"	},
	{ TV_BOOK_FORESEE	,	"Fore_book"	},
	{ TV_BOOK_ENCHANT	,	"Encha_book"	},
	{ TV_BOOK_SUMMONS,		"Summo_book"	},
	{ TV_BOOK_MYSTIC,		"Myst_book"	},
	{ TV_BOOK_LIFE	,	"Life_book"	},
	{ TV_BOOK_PUNISH	,	"Punish_book"	},
	{ TV_BOOK_NATURE	,	"Nature_book"	},
	{ TV_BOOK_TRANSFORM	,	"Trans_book"	},
	{ TV_BOOK_DARKNESS	,	"Dark_book"	},
	{ TV_BOOK_NECROMANCY,		"Necro_book"	},
	{ TV_BOOK_CHAOS	,	"Chaos_book"	},
	{ TV_BOOK_HISSATSU	,	"Hissatsu_book"	},
	{ TV_KNIFE,             "Knife"                },
	{ TV_SWORD,             "Sword"                },
	{ TV_KATANA,             "Katana"                },
	{ TV_HAMMER,             "Hammer"                },
	{ TV_STICK,             "Stick"                },
	{ TV_AXE,            "Axe"        },
	{ TV_SPEAR,           "Spear"              },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_BOW,               "Bow"                  },
	{ TV_CROSSBOW,               "XBow"                  },
	{ TV_GUN,               "Gun"                  },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_BULLET,            "Bullets"                },

	{ TV_MAGICITEM,            "MagicItem"                },
	{ TV_MAGICWEAPON,            "MagicWeapon"                },
	{ TV_OTHERWEAPON,            "OtherWeapon"                },
	{ TV_RIBBON,            "RIBBON"               },

	{ TV_SHIELD,            "Shield"               },
	{ TV_HEAD,              "Helm_etc"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_ARMOR,        "Hard Armor"           },
	{ TV_CLOTHES,        "Clothes"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
	{ TV_WHISTLE,           "Whistle"	},
	{ TV_CHEST,             "Chest"                },
	{ TV_CAPTURE,           "Capture Ball"         },
	{ TV_FIGURINE,          "Magical Figurine"     },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask"                },
	{ TV_PHOTO,             "Photo"                },
	{ TV_MACHINE,             "MACHINE"                },
	{ TV_SOUVENIR,             "Souvenir"                },
	{ TV_MATERIAL,             "Material"                },
	{ TV_MUSHROOM,             "Mushroom"                },
	{ TV_SOFTDRINK,             "Softdrink"                },
	{ TV_ALCOHOL,             "Alcohol"                },
	{ TV_SWEETS,             "Sweets"                },
	{ TV_BUNBUN,             "News_aya"                },
	{ TV_KAKASHI,             "News_hatate"                },
	{ TV_BOOK_MEDICINE	,	"Drug_resipe"	},
	{ TV_COMPOUND	,	"Compound"	},
	{ TV_ITEMCARD	,	"EX_Itemcard"	},
	{ TV_BOOK_OCCULT	,	"Occult_books"	},
	{ TV_ABILITY_CARD	,	"Ability_card" },


	{ 0,                    NULL                   }
};


/*
 * Strip an "object name" into a buffer
 */
void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
#ifdef JP
		if (iskanji(*str)) {*t++ = *str++; *t++ = *str; continue;}
#endif
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


/*
 * Specify tval and sval (type and subtype of object) originally
 * by RAK, heavily modified by -Bernd-
 *
 * This function returns the k_idx of an object type, or zero if failed
 *
 * List up to 50 choices in three columns
 */
/*:::ウィザードアイテム生成用　tvalとsvalを一覧から選択しk_idxにして返す*/
///item debug TVAL SVAL
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	int tval;

	cptr tval_desc;
	char ch;

	int choice[80];

	char buf[160];


	/* Clear screen */
	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 80) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 20 * (num / 20);
		ch = listsym[num];
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* Me need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if (!get_com("Get what type of object? ", &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is illegal */
	if ((num < 0) || (num >= max_num)) return (0);

	/* Base object type chosen, fill in tval */
	/*:::選択結果をTVAL値に変換*/
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	/*:::ここでTVALに応じたアイテム一覧を表示し選択を受けてSVALを表示しているらしい*/
	for (num = 0, i = 1; (num < 80) && (i < max_k_idx); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
		{
			/* Prepare it */
			row = 2 + (num % 20);
			col = 20 * (num / 20);
			ch = listsym[num];
			strcpy(buf,"                    ");

			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			/*:::戻り値用のk_idx候補*/
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is "illegal" */
	if ((num < 0) || (num >= max_num)) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
/*:::ウィザードコマンド専用　アイテムのpval,AC,DAM,HITを変更*/
static void wiz_tweak_item(object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];
	int old_pval;

	/* Hack -- leave artifacts alone */
	if (object_is_artifact(o_ptr)) return;

	old_pval = o_ptr->pval;
	p = "Enter new 'pval' setting: ";
	sprintf(tmp_val, "%d", o_ptr->pval);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->pval = atoi(tmp_val);
	///mod151214 新聞テスト用コマンド追加
	if((o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI) && old_pval != o_ptr->pval)
	{
		if(o_ptr->pval >= max_r_idx) o_ptr->pval = max_r_idx-1;

		o_ptr->xtra5 = numbering_newspaper(o_ptr->pval);
		msg_print(_("(新聞のxtra5が再設定された)", "(newspaper xtra5 has been reset)"));
	}

	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
/*:::ウィザードコマンド専用　指定アイテムをベースに再生成する*/
static void wiz_reroll_item(object_type *o_ptr)
{
	object_type forge;
	object_type *q_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if (object_is_artifact(o_ptr)) return;


	/* Get local object */
	q_ptr = &forge;

	/* Copy the object */
	object_copy(q_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(q_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [w]orthless, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch, FALSE))
		{
			/* Preserve wizard-generated artifacts */
			if (object_is_fixed_artifact(q_ptr))
			{
				a_info[q_ptr->name1].cur_num = 0;
				q_ptr->name1 = 0;
			}

			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Preserve wizard-generated artifacts */
		if (object_is_fixed_artifact(q_ptr))
		{
			a_info[q_ptr->name1].cur_num = 0;
			q_ptr->name1 = 0;
		}

		switch(ch)
		{
			/* Apply bad magic, but first clear object */
			case 'w': case 'W':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT | AM_CURSED);
				break;
			}
			/* Apply bad magic, but first clear object */
			case 'c': case 'C':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_CURSED);
				break;
			}
			/* Apply normal magic, but first clear object */
			case 'n': case 'N':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART);
				break;
			}
			/* Apply good magic, but first clear object */
			case 'g': case 'G':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD);
				break;
			}
			/* Apply great magic, but first clear object */
			case 'e': case 'E':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
				break;
			}
			/* Apply special magic, but first clear object */
			case 's': case 'S':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, AM_GOOD | AM_GREAT | AM_SPECIAL);

				/* Failed to create artifact; make a random one */
				if (!object_is_artifact(q_ptr)) create_artifact(q_ptr, FALSE);
				break;
			}
		}
		q_ptr->iy = o_ptr->iy;
		q_ptr->ix = o_ptr->ix;
		q_ptr->next_o_idx = o_ptr->next_o_idx;
		q_ptr->marked = o_ptr->marked;
	}


	/* Notice change */
	if (changed)
	{
		/* Apply changes */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}
}



/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
/*:::ウィザードコマンド用　指定ベースで何万回と再生成を行い今のアイテムと比較？*/
static void wiz_statistics(object_type *o_ptr)
{
	u32b i, matches, better, worse, other, correct;

	u32b test_roll = 1000000;

	char ch;
	cptr quality;

	u32b mode;

	object_type forge;
	object_type	*q_ptr;

	cptr q = "Rolls: %ld  Correct: %ld  Matches: %ld  Better: %ld  Worse: %ld  Other: %ld";

	cptr p = "Enter number of items to roll: ";
	char tmp_val[80];


	/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
	if (object_is_fixed_artifact(o_ptr)) a_info[o_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch, FALSE)) break;

		if (ch == 'n' || ch == 'N')
		{
			mode = 0L;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			mode = AM_GOOD;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			mode = AM_GOOD | AM_GREAT;
			quality = "excellent";
		}
		else
		{
			break;
		}

		sprintf(tmp_val, "%ld", (long int)test_roll);
		if (get_string(p, tmp_val, 10)) test_roll = atol(tmp_val);
		test_roll = MAX(1, test_roll);

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
					  quality, dun_level);
		msg_print(NULL);

		/* Set counters to zero */
		correct = matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= test_roll; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, correct, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Create an object */
			make_object(q_ptr, mode);


			/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
			if (object_is_fixed_artifact(q_ptr)) a_info[q_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (q_ptr->tval)) continue;
			if ((o_ptr->sval) != (q_ptr->sval)) continue;

			/* One more correct item */
			correct++;

			/* Check for match */
			if ((q_ptr->pval == o_ptr->pval) &&
				 (q_ptr->to_a == o_ptr->to_a) &&
				 (q_ptr->to_h == o_ptr->to_h) &&
				 (q_ptr->to_d == o_ptr->to_d) &&
				 (q_ptr->name1 == o_ptr->name1))
			{
				matches++;
			}

			/* Check for better */
			else if ((q_ptr->pval >= o_ptr->pval) &&
						(q_ptr->to_a >= o_ptr->to_a) &&
						(q_ptr->to_h >= o_ptr->to_h) &&
						(q_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((q_ptr->pval <= o_ptr->pval) &&
						(q_ptr->to_a <= o_ptr->to_a) &&
						(q_ptr->to_h <= o_ptr->to_h) &&
						(q_ptr->to_d <= o_ptr->to_d))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, correct, matches, better, worse, other);
		msg_print(NULL);
	}


	/* Hack -- Normally only make a single artifact */
	if (object_is_fixed_artifact(o_ptr)) a_info[o_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
/*:::アイテムの数を変更する*/
static void wiz_quantity_item(object_type *o_ptr)
{
	int         tmp_int, tmp_qnt;

	char        tmp_val[100];


	//v2.0.16 矢、ボルト、針のランダムアーティファクトは個数を変えられるようにする
	if (object_is_fixed_artifact(o_ptr)) return;
	if (object_is_artifact(o_ptr) && !object_is_needle_arrow_bolt(o_ptr)) return;


	/* Store old quantity. -LM- */
	tmp_qnt = o_ptr->number;

	/* Default */
	sprintf(tmp_val, "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 2))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		o_ptr->number = tmp_int;
	}

	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval = o_ptr->pval * o_ptr->number / tmp_qnt;
	}
}

/* debug command for blue mage */
static void do_cmd_wiz_blue_mage(void)
{

	int				i = 0;
	int				j = 0;
	s32b            f4 = 0, f5 = 0, f6 = 0;

	for (j=1; j<6; j++)
	{

		set_rf_masks(&f4, &f5, &f6, j);

		for (i = 0; i < 32; i++)
		{
			if ((0x00000001 << i) & f4) p_ptr->magic_num2[i] = 1;
		}
		for (; i < 64; i++)
		{
			if ((0x00000001 << (i - 32)) & f5) p_ptr->magic_num2[i] = 1;
		}
		for (; i < 96; i++)
		{
			if ((0x00000001 << (i - 64)) & f6) p_ptr->magic_num2[i] = 1;
		}
	}
}


/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
/*:::アイテム操作*/
static void do_cmd_wiz_play(void)
{
	int item;

	object_type	forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	bool changed;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* The item was not changed */
	changed = FALSE;


	/* Save the screen */
	screen_save();


	/* Get local object */
	q_ptr = &forge;

	/* Copy object */
	object_copy(q_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(q_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch, FALSE))
		{
			changed = FALSE;
			break;
		}

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(q_ptr);
		}

		if (ch == 'r' || ch == 'R')
		{
			wiz_reroll_item(q_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(q_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(q_ptr);
		}
	}


	/* Restore the screen */
	screen_load();


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Recalcurate object's weight */
		if (item >= 0)
		{
			p_ptr->total_weight += (q_ptr->weight * q_ptr->number)
				- (o_ptr->weight * o_ptr->number);
		}

		/* Change */
		object_copy(o_ptr, q_ptr);


		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
/*:::アイテムを作成する　エゴなど品質はここでは指定できない*/
///sys item debug ウィザードコマンドアイテム作成
static void wiz_create_item(void)
{
	object_type	forge;
	object_type *q_ptr;

	int k_idx;


	/* Save the screen */
	screen_save();

	/* Get object base type */
	/*:::TVAL,SVALの一覧を出し選択を受けてk_idxを得る*/
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	screen_load();


	/* Return if failed */
	if (!k_idx) return;

	if (k_info[k_idx].gen_flags & TRG_INSTA_ART)
	{
		int i;

		/* Artifactify */
		for (i = 1; i < max_a_idx; i++)
		{
			/* Ignore incorrect tval */
			if (a_info[i].tval != k_info[k_idx].tval) continue;

			/* Ignore incorrect sval */
			if (a_info[i].sval != k_info[k_idx].sval) continue;

			/* Create this artifact */
			(void)create_named_art(i, py, px);

			/* All done */
			msg_print("Allocated(INSTA_ART).");

			return;
		}
	}

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
	object_prep(q_ptr, k_idx);

	/* Apply magic */
	apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART);

	/* Drop the object from heaven */
	(void)drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
	/* Restore stats */
	(void)res_stat(A_STR);
	(void)res_stat(A_INT);
	(void)res_stat(A_WIS);
	(void)res_stat(A_CON);
	(void)res_stat(A_DEX);
	(void)res_stat(A_CHR);

	/* Restore the level */
	(void)restore_level();

	/* Heal the player */
	if (p_ptr->chp < p_ptr->mhp)
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}

	/* Restore mana */
	if (p_ptr->csp < p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;

		p_ptr->redraw |= (PR_MANA);
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}

	/* Cure stuff */
	(void)set_blind(0);
	(void)set_confused(0);
	(void)set_poisoned(0);
	(void)set_afraid(0);
	(void)set_paralyzed(0);
	(void)set_image(0);
	(void)set_stun(0);
	(void)set_cut(0);
	(void)set_slow(0, TRUE);

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(void)
{
	/* Ask for level */
	if (command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];
		int		tmp_dungeon_type;

		/* Prompt */
		sprintf(ppp, "Jump which dungeon : ");

		/* Default */
		sprintf(tmp_val, "%d", dungeon_type);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 2)) return;

		tmp_dungeon_type = atoi(tmp_val);
		if (!d_info[tmp_dungeon_type].maxdepth || (tmp_dungeon_type > max_d_idx)) tmp_dungeon_type = DUNGEON_ANGBAND;

		/* Prompt */
		sprintf(ppp, "Jump to level (0, %d-%d): ", d_info[tmp_dungeon_type].mindepth, d_info[tmp_dungeon_type].maxdepth);

		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);

		dungeon_type = tmp_dungeon_type;
	}

	/* Paranoia */
	if (command_arg < d_info[dungeon_type].mindepth) command_arg = 0;

	/* Paranoia */
	if (command_arg > d_info[dungeon_type].maxdepth) command_arg = d_info[dungeon_type].maxdepth;

	/* Accept request */
	msg_format("You jump to dungeon level %d.", command_arg);

	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	prepare_change_floor_mode(CFM_RAND_PLACE);

	if (!dun_level) dungeon_type = 0;
	p_ptr->inside_arena = FALSE;
	p_ptr->wild_mode = FALSE;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_WIZ_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	energy_use = 0;

	/* Prevent energy_need from being too lower than 0 */
	p_ptr->energy_need = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
/*:::全ての未判明アイテムを判明させる*/
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= command_arg)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare object */
			object_prep(q_ptr, i);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
/*:::指定数のモンスター（敵）を召喚*/
static void do_cmd_wiz_summon(int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		(void)summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(int r_idx)
{
	(void)summon_named_creature(0, py, px, r_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(int r_idx)
{
	(void)summon_named_creature(0, py, px, r_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP | PM_FORCE_PET));
}



/*
 * Hack -- Delete all nearby monsters
 */
/*:::近くのモンスターを消滅させる*/
static void do_cmd_wiz_zap(void)
{
	int i;


	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip the mount */
		if (i == p_ptr->riding) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT)
		{
			if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
			}

			delete_monster_idx(i);
		}
	}
}


/*
 * Hack -- Delete all monsters
 */
/*:::フロアのモンスターを全て消滅させる*/
static void do_cmd_wiz_zap_all(void)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip the mount */
		if (i == p_ptr->riding) continue;

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_WIZ_ZAP, m_name);
		}

		/* Delete this monster */
		delete_monster_idx(i);
	}
}


/*
 * Create desired feature
 */
/*:::指定位置に指定した地形を作る*/
static void do_cmd_wiz_create_feature(void)
{
	static int   prev_feat = 0;
	static int   prev_mimic = 0;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	char         tmp_val[160];
	int          tmp_feat, tmp_mimic;
	int          y, x;

	if (!tgt_pt(&x, &y)) return;

	c_ptr = &cave[y][x];

	/* Default */
	sprintf(tmp_val, "%d", prev_feat);

	/* Query */
#ifdef JP
	if (!get_string("地形: ", tmp_val, 3)) return;
#else
	if (!get_string("Feature: ", tmp_val, 3)) return;
#endif

	/* Extract */
	tmp_feat = atoi(tmp_val);
	if (tmp_feat < 0) tmp_feat = 0;
	else if (tmp_feat >= max_f_idx) tmp_feat = max_f_idx - 1;

	/* Default */
	sprintf(tmp_val, "%d", prev_mimic);

	/* Query */
#ifdef JP
	if (!get_string("地形 (mimic): ", tmp_val, 3)) return;
#else
	if (!get_string("Feature (mimic): ", tmp_val, 3)) return;
#endif

	/* Extract */
	tmp_mimic = atoi(tmp_val);
	if (tmp_mimic < 0) tmp_mimic = 0;
	else if (tmp_mimic >= max_f_idx) tmp_mimic = max_f_idx - 1;

	cave_set_feat(y, x, tmp_feat);
	c_ptr->mimic = tmp_mimic;

	f_ptr = &f_info[get_feat_mimic(c_ptr)];

	if (have_flag(f_ptr->flags, FF_GLYPH) ||
	    have_flag(f_ptr->flags, FF_MINOR_GLYPH))
		c_ptr->info |= (CAVE_OBJECT);
	else if (have_flag(f_ptr->flags, FF_MIRROR))
		c_ptr->info |= (CAVE_GLOW | CAVE_OBJECT);

	/* Notice */
	note_spot(y, x);

	/* Redraw */
	lite_spot(y, x);

	/* Update some things */
	p_ptr->update |= (PU_FLOW);

	prev_feat = tmp_feat;
	prev_mimic = tmp_mimic;
}


#define NUM_O_SET 8
#define NUM_O_BIT 32

/*
 * Hack -- Dump option bits usage
 */
///sys オプション設定状況のダンプ
static void do_cmd_dump_options(void)
{
	int  i, j;
	FILE *fff;
	char buf[1024];
	int  **exist;

	/* Build the filename */
	path_build(buf, sizeof buf, ANGBAND_DIR_USER, "opt_info.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "a");

	/* Oops */
	if (!fff)
	{
#ifdef JP
		msg_format("ファイル %s を開けませんでした。", buf);
#else
		msg_format("Failed to open file %s.", buf);
#endif
		msg_print(NULL);
		return;
	}

	/* Allocate the "exist" array (2-dimension) */
	C_MAKE(exist, NUM_O_SET, int *);
	C_MAKE(*exist, NUM_O_BIT * NUM_O_SET, int);
	for (i = 1; i < NUM_O_SET; i++) exist[i] = *exist + i * NUM_O_BIT;

	/* Check for exist option bits */
	for (i = 0; option_info[i].o_desc; i++)
	{
		const option_type *ot_ptr = &option_info[i];
		if (ot_ptr->o_var) exist[ot_ptr->o_set][ot_ptr->o_bit] = i + 1;
	}
		///\sys131117 FAKE_VERからH_VERへ
	fprintf(fff, "[Option bits usage on Hengband(TH) %d.%d.%dT]\n\n",
	        H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);

	fputs("Set - Bit (Page) Option Name\n", fff);
	fputs("------------------------------------------------\n", fff);
	/* Dump option bits usage */
	for (i = 0; i < NUM_O_SET; i++)
	{
		for (j = 0; j < NUM_O_BIT; j++)
		{
			if (exist[i][j])
			{
				const option_type *ot_ptr = &option_info[exist[i][j] - 1];
				fprintf(fff, "  %d -  %02d (%4d) %s\n",
				        i, j, ot_ptr->o_page, ot_ptr->o_text);
			}
			else
			{
				fprintf(fff, "  %d -  %02d\n", i, j);
			}
		}
		fputc('\n', fff);
	}

	/* Free the "exist" array (2-dimension) */
	C_KILL(*exist, NUM_O_BIT * NUM_O_SET, int);
	C_KILL(exist, NUM_O_SET, int *);

	/* Close it */
	my_fclose(fff);

#ifdef JP
	msg_format("オプションbit使用状況をファイル %s に書き出しました。", buf);
#else
	msg_format("Option bits usage dump saved to file %s.", buf);
#endif
}


#ifdef ALLOW_SPOILERS

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(void);



/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(void)
{
	int     x, y;
	char    cmd;


	/* Get a "debug command" */
	get_com("Debug Command: ", &cmd, FALSE);

	/* Analyze the command */
	switch (cmd)
	{
	/* Nothing */
	case ESCAPE:
	case ' ':
	case '\n':
	case '\r':
		break;

#ifdef ALLOW_SPOILERS

	/* Hack -- Generate Spoilers */
	case '"':
		do_cmd_spoilers();
		break;

#endif /* ALLOW_SPOILERS */

	/* Hack -- Help */
	case '?':
		do_cmd_help();
		break;

	/* Cure all maladies */
	case 'a':
		do_cmd_wiz_cure_all();
		break;

	/* Know alignment */
	case 'A':
		///sysdel アライメント
		msg_format("Your alignment is %d.", p_ptr->align);
		break;

	/* Teleport to target */
	case 'b':
		do_cmd_wiz_bamf();
		break;

	case 'B':
		///sysdel 闘技場？
		//battle_monsters();
		break;

	/* Create any object */
	case 'c':
		wiz_create_item();
		break;

	/* Create a named artifact */
	case 'C':
		wiz_create_named_art();
		break;

	/* Detect everything */
	case 'd':
		detect_all(DETECT_RAD_ALL * 3);
		break;

	/* Dimension_door */
	case 'D':
		wiz_dimension_door();
		break;

	/* Edit character */
	case 'e':
		do_cmd_wiz_change();
		break;

	/* Blue Mage Only */
	case 'E':
		if (p_ptr->pclass == CLASS_BLUE_MAGE)
		{
			do_cmd_wiz_blue_mage();
		}
		break;

	/* View item info */
	case 'f':
		identify_fully(FALSE);
		break;

	/* Create desired feature */
	case 'F':
		do_cmd_wiz_create_feature();
		break;

	/* Good Objects */
	case 'g':
		if (command_arg <= 0) command_arg = 1;
		acquirement(py, px, command_arg, FALSE, TRUE);
		break;

	/* Hitpoint rerating */
		/*:::体力ランクを再計算する*/
	case 'h':
		do_cmd_rerate(TRUE);
		break;

#ifdef MONSTER_HORDES
	case 'H':
		do_cmd_summon_horde();
		break;
#endif /* MONSTER_HORDES */

	/* Identify */
	case 'i':
		(void)ident_spell(FALSE);
		break;

	/* Go up or down in the dungeon */
	case 'j':
		do_cmd_wiz_jump();
		break;

	/* Self-Knowledge */
	case 'k':
		self_knowledge();
		break;

	/* Learn about objects */
	case 'l':
		do_cmd_wiz_learn();
		break;

	/* Magic Mapping */
	case 'm':
		map_area(DETECT_RAD_ALL * 3);
		break;

	/* Mutation */
	case 'M':
		(void)gain_random_mutation(command_arg);
		break;

	/* Reset Class */
	///sysdel クラス変更らしいが機能していないようだ
	case 'R':
		(void)do_cmd_wiz_reset_class();
		break;

	/* Specific reward */
	/*:::カオスパトロンからの報酬*/
	case 'r':
		(void)gain_level_reward(command_arg);
		break;

	/* Summon _friendly_ named monster */
	/*:::指定IDの友好的なモンスターを召喚*/
	case 'N':
		do_cmd_wiz_named_friendly(command_arg);
		break;

	/* Summon Named Monster */
	/*:::指定IDのモンスターを召喚*/
	case 'n':
		do_cmd_wiz_named(command_arg);
		break;

	/* Dump option bits usage */
	case 'O':
		do_cmd_dump_options();
		break;

	/* Object playing routines */
	case 'o':
		do_cmd_wiz_play();
		break;

	/* Phase Door */
	case 'p':
		teleport_player(10, 0L);
		break;

#if 0
	/* Complete a Quest -KMW- */
	case 'q':
		for (i = 0; i < max_quests; i++)
		{
			if (p_ptr->quest[i].status == QUEST_STATUS_TAKEN)
			{
				p_ptr->quest[i].status++;
				msg_print("Completed Quest");
				msg_print(NULL);
				break;
			}
		}
		if (i == max_quests)
		{
			msg_print("No current quest");
			msg_print(NULL);
		}
		break;
#endif

	/* Make every dungeon square "known" to test streamers -KMW- */
	/*:::啓蒙　壁の中まで含む全てのグリッドを既知にする*/
	case 'u':
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
			}
		}
		wiz_lite(FALSE);
		break;

	/* Summon Random Monster(s) */
	/*:::指定数の敵を召喚*/
	case 's':
		if (command_arg <= 0) command_arg = 1;
		do_cmd_wiz_summon(command_arg);
		break;

	/* Teleport */
	case 't':
		teleport_player(100, 0L);
		break;

	/* Very Good Objects */
	case 'v':
		if (command_arg <= 0) command_arg = 1;
		acquirement(py, px, command_arg, TRUE, TRUE);
		break;

	/* Wizard Light the Level */
	case 'w':
		wiz_lite((bool)(p_ptr->pclass == CLASS_NINJA));
		break;

	/* Increase Experience */
	/*:::回数：で指定した量の経験値を得る*/
	case 'x':
		gain_exp(command_arg ? command_arg : (p_ptr->exp + 1));
		break;

	/* Zap Monsters (Genocide) */
	/*:::近くのモンスターを消滅させる*/
	case 'z':
		do_cmd_wiz_zap();
		break;

	/* Zap Monsters (Omnicide) */
	/*:::フロアのモンスターを全て消滅させる*/
	case 'Z':
		do_cmd_wiz_zap_all();
		break;

	/* Hack -- whatever I desire */
	case '_':
		do_cmd_wiz_hack_ben();
		break;

	case 'I':
		do_cmd_wiz_hack_ben_2();
		break;
	//アイテムのパラメータをテキストとして書き出す
	case 'P':
		print_all_item_param(-1);
		break;

		///mod160218 アイテムカードのテストコマンド
	case 'S':
		support_item_test();
		break;


	/* Not a Wizard Command */
	default:
		msg_print("That is not a valid debug command.");
		break;
	}
}






#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif

