/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



/*
 * Display inventory
 */
/*:::ザックの中を表示*/
void do_cmd_inven(void)
{
	char out_val[160];


	/* Note that we are in "inventory" mode */
	command_wrk = FALSE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "inventory" mode */
	if (easy_floor) command_wrk = (USE_INVEN);

#endif /* ALLOW_EASY_FLOOR */

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	(void)show_inven(0);

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

#ifdef JP
	sprintf(out_val, "持ち物： 合計 %3d.%1d kg (限界の%lu%%) コマンド: ",
	    (int)lbtokg1(p_ptr->total_weight) , (int)lbtokg2(p_ptr->total_weight) ,
	    (unsigned long)((p_ptr->total_weight * 100) / weight_limit()));
#else
	sprintf(out_val, "Inventory: carrying %3d.%1d kg (%ld%% of capacity). Command: ",
	    (int)lbtokg1(p_ptr->total_weight), (int)lbtokg2(p_ptr->total_weight) ,
	    (unsigned long)(p_ptr->total_weight * 100) / weight_limit());
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Load screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}


/*
 * Display equipment
 */
/*:::装備品一覧を表示しコマンドを待つ*/
void do_cmd_equip(void)
{
	char out_val[160];


	/* Note that we are in "equipment" mode */
	command_wrk = TRUE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "equipment" mode */
	if (easy_floor) command_wrk = (USE_EQUIP);

#endif /* ALLOW_EASY_FLOOR  */

	/* Save the screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	(void)show_equip(0);

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Build a prompt */
#ifdef JP
	sprintf(out_val, "装備： 合計 %3d.%1d kg (限界の%ld%%) コマンド: ",
	    (int)lbtokg1(p_ptr->total_weight) , (int)lbtokg2(p_ptr->total_weight) ,
	    (long int)((p_ptr->total_weight * 100) / weight_limit()));
#else
	sprintf(out_val, "Equipment: carrying %3d.%1d kg (%ld%% of capacity). Command: ",
	    (int)lbtokg1(p_ptr->total_weight), (int)lbtokg2(p_ptr->total_weight),
	    (long int)((p_ptr->total_weight * 100) / weight_limit()));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
/*:::do_cmd_wield()から使われる。装備可能なアイテムを選別する。*/
///item 水着特殊処理とTVAL
///sys 装備制限職や種族はここに処理入れようか？
///mod150403 staticでなくした
bool item_tester_hook_wear(object_type *o_ptr)
{
	///del131223 水着削除
	//if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI))
	//	if (p_ptr->psex == SEX_MALE) return FALSE;

	/* Check for a usable slot */
	/*:::装備品でないものは-1を返す関数*/
	if (wield_slot(o_ptr) >= INVEN_RARM) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}

///item TVAL 装備変更　手に持つ可能性があるものをすべて指定
///mod140309 射撃武器も対象にした
///mod151214 バグ修正　MAGICWEAPON追加
static bool item_tester_hook_mochikae(object_type *o_ptr)
{
	///mod151202 三妖精用特殊処理 盾が入らない
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		if (((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)) ||
		    (o_ptr->tval == TV_CAPTURE) ||
		    (o_ptr->tval == TV_MAGICWEAPON) ||
		    (o_ptr->tval == TV_MAGICITEM) ||
		    (o_ptr->tval == TV_OTHERWEAPON)) return (TRUE);
		if(object_is_shooting_weapon(o_ptr)) return TRUE;
		return (FALSE);
	}

	/* Check for a usable slot */
	if (((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)) ||
	    (o_ptr->tval == TV_SHIELD) || (o_ptr->tval == TV_CAPTURE) ||
	    (o_ptr->tval == TV_MAGICITEM) ||
	    (o_ptr->tval == TV_MAGICWEAPON) ||
	    (o_ptr->tval == TV_OTHERWEAPON)) return (TRUE);
	if(object_is_shooting_weapon(o_ptr)) return TRUE;
	/* Assume not wearable */
	return (FALSE);
}


#if 0
v1.1.93 紛らわしいし似た役割の関数がすでにあるので削除
static bool item_tester_hook_melee_weapon(object_type *o_ptr)
{
	/* Check for a usable slot */
	if ((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)
		|| (o_ptr->tval == TV_OTHERWEAPON))return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}
#endif


//数字が少ないほど優先してINVEN_RARMに入る。クロスボウと銃を「通常右手に持つが近接武器と一緒に持ったら左手になる」なんて変な仕様にしたせいで判定が面倒になって今になって作った
int check_priority_inven_rarm(object_type *o_ptr)
{
	switch(o_ptr->tval)
	{
	case TV_BOW:
		return 4;
	case TV_CAPTURE:
	case TV_MAGICITEM:
	case TV_SHIELD:
		return 3;
	case TV_CROSSBOW:
	case TV_GUN:
		return 2;
	default:
		return 1;
	}

}


bool select_ring_slot = FALSE;

/*
 * Wield or wear a single item from the pack or floor
 */
/*:::装備変更*/
///item TVAL 装備変更
///mod131223 tval
//mod160515 mochikaeオプション追加　両手装備品のみ
void do_cmd_wield(bool mochikae)
{
	int item, slot;
	bool asia = FALSE;

	object_type forge,forge2;
	object_type *q_ptr;

	object_type *o_ptr;
	bool moon_vault_trap = FALSE;

	bool flag_quickdraw = FALSE;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	/*:::右手が盾で左手が武器など、持ち替えをするときのフラグ*/
	int need_switch_wielding = 0;

	/*:::右手にクロスボウを持ち左手が弓で両方は持たない時など、特定スロットの装備を外すときのフラグを追加*/
	int need_takeoff = 0;
	object_type *takeoff_o_ptr;
	takeoff_o_ptr = &forge2;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print(_("この姿では装備を変更できない。", "You cannot change equipment in your current form."));
		return ;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Restrict the choices */
	if(mochikae)
		item_tester_hook = item_tester_hook_mochikae;
	else
		item_tester_hook = item_tester_hook_wear;

	/* Get an item */
#ifdef JP
	q = "どれを装備しますか? ";
	s = "装備可能なアイテムがない。";
#else
	q = "Wear/Wield which item? ";
	s = "You have nothing you can wear or wield.";
#endif
	/*:::装備するアイテムを選ぶ。一覧への表示はitem_tester_hookでフィルタされる*/
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Check the slot */
	slot = wield_slot(o_ptr);

	if(o_ptr->tval == TV_GUN && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
		flag_quickdraw = TRUE;

	///mod160515 「右手と左手に別々の物を持つとき常に確認する」オプションを追加
	if((always_check_arms) && (slot == INVEN_RARM || slot == INVEN_LARM))
	{
		int old_slot = slot;
		if(!inventory[INVEN_RARM].k_idx && !inventory[INVEN_LARM].k_idx)
		{	//両手が空の時何もせずそのまま装備
			;
		}
		else if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx)
		{	//すでに両手に物を持っているとき、常に入れ替え確認
			item_tester_hook = item_tester_hook_mochikae;
			q = _("どちらの装備品と取り替えますか?", "Replace which piece of equipment?");
			s = _("ERROR:do_cmd_wield()の処理がおかしい(1)", "ERROR: Weird logic in do_cmd_wield(1)");
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;

			//取り替えた結果、普通左手に持つ物を右手に持ってて右手に持つものを左手に持つ状態になったら左右の装備を入れ替える
			if(slot == INVEN_RARM && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
				need_switch_wielding = INVEN_LARM;
			else if(slot == INVEN_LARM && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
				need_switch_wielding = INVEN_RARM;
		}
		else
		{	//左右どちらかのスロットがふさがっているとき、それを外すかどうか確認する
			if (get_check(_("今手に持っている物と一緒に装備しますか？",
                            "Do you want to put it in your free hand?")))
			{
//					if(inventory[INVEN_LARM].k_idx && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
//						need_switch_wielding = INVEN_LARM;
//					else if(inventory[INVEN_RARM].k_idx && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
//						need_switch_wielding = INVEN_RARM;

				//v1.1.15b 盾装備時に弓を持とうとした時盾を外してしまうので修正
				if(inventory[slot].k_idx)
				{
					if(slot == INVEN_RARM && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
					{
						slot = INVEN_LARM;
						need_switch_wielding = INVEN_RARM;
					}
					if(slot == INVEN_LARM && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
					{
						slot = INVEN_RARM;
						need_switch_wielding = INVEN_LARM;
					}

				}
			}
			else
			{	//外す処理
				if(inventory[INVEN_RARM].k_idx)
				{
					need_takeoff = INVEN_RARM;
					if(check_priority_inven_rarm(o_ptr) < 3) slot = INVEN_RARM;
				}
				else if(inventory[INVEN_LARM].k_idx)
				{
					need_takeoff = INVEN_LARM;
					if(check_priority_inven_rarm(o_ptr) > 2) slot = INVEN_LARM;
				}
			}
		}
	}
	else
	/*:::武器・盾・リングはどこに装備するか決まらないので選択したアイテムを何処に装備するか決めてslotに格納*/
	switch (o_ptr->tval)
	{
	/* Shields and some misc. items */
	/*:::盾、モンスターボール*/
		///item TVAL
	case TV_CAPTURE:
	case TV_MAGICITEM:
	case TV_SHIELD:
	//case TV_CARD:
		/* Dual wielding */
		/*:::二刀流の場合どちらの武器と取り替えるか選ぶ*/
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			/* Restrict the choices */
			/*:::選択一覧に(毒針以外の)武器しか出さないためのhook*/
			/*:::・・なんだが、毒針二刀流にしててもちゃんと両方候補に出る。どういうことか。*/
			//↑cmd3.cとbldg.cに同名の関数があり、ここのは毒針も選択対象

			//v1.1.93 ここ独自のmelee_weapon判定関数を削除して普通のに変えた。TV_MAGICWEAPONが新たに対象になる
			//item_tester_hook = item_tester_hook_melee_weapon;
			item_tester_hook = object_is_melee_weapon;

			item_tester_no_ryoute = TRUE;

			/* Choose a weapon from the equipment only */
#ifdef JP
			q = "どちらの武器と取り替えますか?";
			s = "おっと。";
#else
			q = "Replace which weapon? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM) need_switch_wielding = INVEN_LARM;
		}
		/*:::左手に武器を持っている場合右手に盾とか装備？実際にはしてないが。ここには来るのか？*/
		else if (buki_motteruka(INVEN_LARM)) slot = INVEN_RARM;

		///mod140309 弓枠廃止関連
		/*:::左右どちらかの手に射撃武器を持っていて新たに盾などを装備する場合の処理を追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::右手に射撃武器を持ち、左手に何も持ってないとき（クロスボウか銃を両手持ち、もしくはクロスボウか銃と手綱）は、そのまま盾などを左手に装備*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				;
			}
			/*:::左手に射撃武器を持ち、右手に何も持ってないとき（弓を両手持ち、もしくは弓と手綱）は、盾などを右手に装備*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				slot = INVEN_RARM;
			}
			/*:::両手に射撃武器を含む何かを持っているとき　まずはどちらを外すか選ぶ*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "どちらの装備品と取り替えますか?";
				s = "おっと。";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::右手の装備と取り替え、左手に残ったのが近接武器・銃・クロスボウなら持ち替える*/
					if(object_is_melee_weapon(&inventory[INVEN_LARM]) || inventory[INVEN_LARM].tval == TV_CROSSBOW || inventory[INVEN_LARM].tval == TV_GUN)
					{
						need_switch_wielding = INVEN_LARM;
					}
					/*:::左手にあるのが弓、盾などならそのまま装備する*/
				}
				else
				{
					/*:::左手の装備と取り替え、右手に残ったのが弓なら持ち替える*/
					if(inventory[INVEN_RARM].tval == TV_BOW)
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Both arms are already used by non-weapon */
		/*:::盾などを両手に持っている場合どちらかと入れ替える*/
		else if (inventory[INVEN_RARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_RARM]) &&
		         inventory[INVEN_LARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_LARM]))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "どちらの手に装備しますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
		}
		break;

	/* Melee weapons */
	///item TVAL
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
	case TV_OTHERWEAPON:
	case TV_MAGICWEAPON:


		///mod140309 弓枠廃止関連
		/*:::左右どちらかの手に射撃武器を持っていて新たに近接武器を装備する場合の処理を追加*/
		if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::右手に射撃武器を持ち、左手に何も持ってないとき（クロスボウか銃を両手持ち、もしくはクロスボウか銃と手綱）は、射撃武器を左手に持ち替え*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				slot = INVEN_LARM;
				need_switch_wielding = INVEN_RARM;
			}
			/*:::左手に射撃武器を持ち、右手に何も持ってないとき（弓を両手持ち、もしくは弓と手綱）は、新たな近接武器をそのまま右手に装備*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				;
			}
			/*:::両手に射撃武器を含む何かを持っているとき　まずはどちらを外すか選ぶ*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "どちらの装備品と取り替えますか?";
				s = "おっと。";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::右手の装備と取り替えた場合、左手に持っているのが何であろうとそのまま*/
					;
				}
				else
				{
					/*:::左手の装備と取り替えた場合、右手に持っているのが近接武器以外なら持ち替える*/
					if(!object_is_melee_weapon(&inventory[INVEN_RARM]))
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Asking for dual wielding */
		/*:::近接武器を装備しようとしてslotがLARMになるのは右手にのみ何かを装備しているとき　*/
		/*:::盾やキャプチャーだけ装備しているときは左手に持っているので右手にのみ何か持ってるならそれは常に近接武器*/
		/*:::↑弓枠廃止でそうとも限らなくなったがその処理は上で済ませた*/
		else if (slot == INVEN_LARM)
		{
#ifdef JP
			if (!get_check("二刀流で戦いますか？")) slot = INVEN_RARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_RARM;
#endif
		}

		else if (!inventory[INVEN_RARM].k_idx && buki_motteruka(INVEN_LARM))
		{
#ifdef JP
			if (!get_check("二刀流で戦いますか？")) slot = INVEN_LARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_LARM;
#endif
		}

		/* Both arms are already used */
		else if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "どちらの手に装備しますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if ((slot == INVEN_LARM) && !buki_motteruka(INVEN_RARM))
				need_switch_wielding = INVEN_RARM;
		}
		break;


///mod140309 弓枠廃止処理関連
	/*:::弓を新たに装備　slotは常にINVEN_LARM*/
	case TV_BOW:
		/*:::両手にそれぞれ何かを持ってる場合、どちらを外すか選んで残った方は常に右手で持つ*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "どちらの装備品と持ち替えますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM)	need_switch_wielding = INVEN_LARM;
		}
		/*:::射撃武器のみ持っているとき、両手に持つか確認しNOなら新しい方だけ持つ　フラグ追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (!get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) need_takeoff = INVEN_RARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) slot = INVEN_RARM;
		}
		/*:::近接武器のみ持っているとき（必ず右手のはず）、追加処理なし　弓は左手で持つ*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			;
		}
		/*:::盾や妖魔本のみ持っているとき（必ず左手のはず）、弓を左手に持ってそれらは右手へ*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			slot = INVEN_RARM;
			need_switch_wielding = INVEN_LARM;
		}
		/*:::何も装備していないとき、弓は左手へ*/
		else
		{
			;
		}
		break;

	/*:::銃やクロスボウを新たに装備　slotは右手に何か持ってたらINVEN_LARM*/
	case TV_CROSSBOW:
	case TV_GUN:
		/*:::両手にそれぞれ何かを持ってる場合、まずどちらを外すか選ぶ*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "どちらの装備品と持ち替えますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			/*:::残ったのが近接武器ならそちらを右手に持ち銃かクロスボウは左手へ*/
			if (slot == INVEN_RARM && object_is_melee_weapon(&inventory[INVEN_LARM]))
			{
				need_switch_wielding = INVEN_LARM;
			}
			//残ったのが銃の場合持ち替えない
			else if(slot == INVEN_LARM && inventory[INVEN_RARM].tval == TV_GUN ||
				slot == INVEN_RARM && inventory[INVEN_LARM].tval == TV_GUN )
			{
				; //何もしない
			}
			/*:::残ったのが盾や妖魔本ならそれを左手に持つ*/
			else if (slot == INVEN_LARM && !object_is_melee_weapon(&inventory[INVEN_RARM]))
			{
				need_switch_wielding = INVEN_RARM;
			}
		}
		/*:::射撃武器のみ持っているとき、両手に持つか確認しNOなら新しい方だけ持つ　フラグ追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (!get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) need_takeoff = INVEN_LARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) slot = INVEN_LARM;
			else slot = INVEN_RARM;
		}
		/*:::近接武器のみ持っているとき（必ず右手のはず）、銃とクロスボウは左手で持つ*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			slot = INVEN_LARM;
		}
		/*:::盾や妖魔本のみ持っているとき（必ず左手のはず）、追加処理なし　銃とクロスボウは右手へ*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			;
		}
		/*:::何も装備していないとき、銃とクロスボウは右手へ*/
		else
		{
			;
		}
		break;

	/* Rings */
	case TV_RING:
		/* Choose a ring slot */
		if (inventory[INVEN_LEFT].k_idx && inventory[INVEN_RIGHT].k_idx)
		{
#ifdef JP
			q = "どちらの指輪と取り替えますか?";
#else
			q = "Replace which ring? ";
#endif
		}
		else
		{
#ifdef JP
			q = "どちらの手に装備しますか?";
#else
			q = "Equip which hand? ";
#endif
		}

#ifdef JP
		s = "おっと。";
#else
		s = "Oops.";
#endif

		/* Restrict the choices */
		select_ring_slot = TRUE;
		item_tester_no_ryoute = TRUE;

		if (!get_item(&slot, q, s, (USE_EQUIP)))
		{
			select_ring_slot = FALSE;
			return;
		}
		select_ring_slot = FALSE;
		break;
	}
	/*:::武器・盾・リングのスロット決定処理終了*/

	///mod140122 装備制限処理追加
	if(!(wield_check(slot,item,0))) return;

	//v1.1.43 need_takeoffの装備外し制限処理が抜けていたので追加
	if (need_takeoff && !(wield_check(need_takeoff, INVEN_PACK,0))) return;

	/* Prevent wielding into a cursed slot */
	if (object_is_cursed(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], (OD_OMIT_PREFIX | OD_NAME_ONLY));

		/* Message */
#ifdef JP
		msg_format("%s%sは呪われているようだ。",
			   describe_use(slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(slot));
#endif

		/* Cancel the command */
		return;
	}

	///mod160807 need_takeoffを使うときの呪い処理が足りなかったので追加
	if (need_takeoff && object_is_cursed(&inventory[need_takeoff]))
	{
		object_desc(o_name, &inventory[need_takeoff], (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
		msg_format("%s%sは呪われているようだ。",  describe_use(need_takeoff) , o_name );
#else
        msg_format("The %s you are %s appears to be cursed.", o_name, describe_use(need_takeoff));
#endif
		return;
	}



	if (confirm_wear &&
		((object_is_cursed(o_ptr) && object_is_known(o_ptr)) ||
		((o_ptr->ident & IDENT_SENSE) &&
			(FEEL_BROKEN <= o_ptr->feeling) && (o_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "本当に%s{呪われている}を使いますか？", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif

		if (!get_check(dummy)) return;
	}
	///item system 石仮面の種族変容 アンドロイド（妖怪人形）も変異するようにしようと思ったがよく考えたら経験値計算が大変なことになる
	if ((o_ptr->name1 == ART_STONEMASK) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		char dummy[MAX_NLEN+80];

		int j;

		for (j = 0; j < INVEN_PACK; j++) if(inventory[j].tval == TV_SOUVENIR && inventory[j].sval == SV_SOUVENIR_ASIA) asia = TRUE;

		if(asia)
		{
			msg_print(_("仮面の額に「エイジャの赤石」がぴったり収まっている。",
                        "'Red Stone of Aja' would perfectly fit in the forehead of your mask."));
			msg_print(NULL);
			sprintf(dummy, _("このまま装備してみますか？", "Wear it like that?"));
			if (!get_check(dummy)) return;
		}
		else if(p_ptr->prace != RACE_VAMPIRE)
		{
			/* Describe it */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			sprintf(dummy, _("%sを装備すると吸血鬼になります。よろしいですか？",
                            "Wearing %s will turn you into a vampire. Are you sure?"), o_name);
			if (!get_check(dummy)) return;
		}
	}
	///mod140115 メンポの種族変容 妖怪人形は変身しない
	if ((o_ptr->name1 == ART_MENPO) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "%sを装備するとニンジャになります。よろしいですか？", o_name);
#else
        sprintf(dummy, "Wearing %s will turn you into a ninja. Are you sure?", o_name);
#endif

		if (!get_check(dummy)) return;
	}

	//九十九姉妹演奏中断
	stop_tsukumo_music();

	//v1.1.95 衣玖特技中断
	if (p_ptr->pclass == CLASS_IKU && p_ptr->tim_general[0]) set_tim_general(0, TRUE, 0, 0);

	if (need_switch_wielding && !object_is_cursed(&inventory[need_switch_wielding]))
	{
		object_type *slot_o_ptr = &inventory[slot];
		object_type *switch_o_ptr = &inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmp_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		object_copy(otmp_ptr, switch_o_ptr);
		object_copy(switch_o_ptr, slot_o_ptr);
		object_copy(slot_o_ptr, otmp_ptr);
#ifdef JP
		msg_format("%sを%sに構えなおした。", switch_name, (slot == INVEN_RARM) ? (left_hander ? "左手" : "右手") : (left_hander ? "右手" : "左手"));
#else
		msg_format("You wield %s at %s hand.", switch_name, (slot == INVEN_RARM) ? (left_hander ? "left" : "right") : (left_hander ? "right" : "left"));
#endif

		slot = need_switch_wielding;
	}



	/*:::★入手型クエストで該当アイテムを拾ったかチェック*/
	/*:::宝物庫のターゲットを床から直接装備したときのための処理と思われる*/
	check_find_art_quest_completion(o_ptr);


	//月都万象展用特殊アイテム
	if(	p_ptr->inside_quest == QUEST_MOON_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
	{
		activate_moon_vault_trap();
	}



	///seikaku
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		identify_item(o_ptr);

		/* Auto-inscription */
		autopick_alter_item(item, FALSE);
	}

	/* Take a turn */
	if(flag_quickdraw)
	{
		msg_print(_("あなたは素早く銃を構えた。", "You quickly draw your gun."));
		energy_use = 50;
	}
	else
		energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}
	else if(need_takeoff)
	{
		(void)inven_takeoff(need_takeoff, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Increase the weight */
	p_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;

#ifdef JP
#define STR_WIELD_RARM "%s(%c)を右手に装備した。"
#define STR_WIELD_LARM "%s(%c)を左手に装備した。"
#define STR_WIELD_ARMS "%s(%c)を両手で構えた。"
#else
#define STR_WIELD_RARM "You are wielding %s (%c) in your right hand."
#define STR_WIELD_LARM "You are wielding %s (%c) in your left hand."
#define STR_WIELD_ARMS "You are wielding %s (%c) with both hands."
#endif

	/* Where is the item now */
	switch (slot)
	{
	case INVEN_RARM:
		if (object_allow_two_hands_wielding(o_ptr) && (empty_hands(FALSE) == EMPTY_HAND_LARM) && CAN_TWO_HANDS_WIELDING())
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_LARM : STR_WIELD_RARM);
		break;

	case INVEN_LARM:
		if (object_allow_two_hands_wielding(o_ptr) && (empty_hands(FALSE) == EMPTY_HAND_RARM) && CAN_TWO_HANDS_WIELDING())
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_RARM : STR_WIELD_LARM);
		break;

	case INVEN_RIBBON:
#ifdef JP
		act = "%s(%c)を結んだ。";
#else
		act = "You have tied %s (%c).";
#endif
		break;

	case INVEN_LITE:
#ifdef JP
		act = "%s(%c)を光源にした。";
#else
		act = "Your light source is %s (%c).";
#endif
		break;

	default:
#ifdef JP
		act = "%s(%c)を装備した。";
#else
		act = "You are wearing %s (%c).";
#endif
		break;
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, 0);

	/* Message */
	msg_format(act, o_name, index_to_label(slot));


	/* Cursed! */
	if (object_is_cursed(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("うわ！ すさまじく冷たい！");
#else
		msg_print("Oops! It feels deathly cold!");
#endif


		//chg_virtue(V_HARMONY, -1);

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	///item race 石仮面の種族変容
	/* The Stone Mask make the player turn into a vampire! */
	if ((o_ptr->name1 == ART_STONEMASK) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		/* Turn into a vampire */
		if(asia) change_race(RACE_ULTIMATE, "");
		else if(p_ptr->prace != RACE_VAMPIRE) change_race(RACE_VAMPIRE, "");
	}
	///mod140115 メンポの種族変容　妖怪人形は変身しない
	if (o_ptr->name1 == ART_MENPO && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		/*:::生い立ち欄が書き換わる*/
		sprintf(p_ptr->history[0], _("あなたは%sとして平穏に生きていた。", "You led a peaceful life as a %s."),rp_ptr->title);
		sprintf(p_ptr->history[1], _("しかしある日あなたに平安時代のニンジャ・ソウルが憑依した！",
                                    "But one day you got possessed by Heian era Ninja Soul!"));
		if(one_in_(6))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、太歳星君への復讐だ！",
                                        "Your goal: Take revenge on Taisui Xingjun!"));
		else if(one_in_(5))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、月の女神嫦娥への復讐だ！",
                                        "Your goal: Take revenge on the moon goddess Chang'e!"));
		else if(one_in_(4))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、『混沌のサーペント』への復讐だ！",
                                        "Your goal: Take revenge on the Serpent of Chaos!"));
		else if(one_in_(3))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、全てのニンジャの抹殺だ！",
                                        "Your goal: Exterminate all ninjas!"));
		else if(one_in_(2))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、自らを虐げた者達への復讐だ！",
                                        "Your goal: Take revenge on those who have oppressed you!"));
		else
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、全アンバーの王族への復讐だ！",
                                        "Your goal: Take revenge on all of the Lords of Amber!"));
		sprintf(p_ptr->history[3], _("走れ、%s、走れ！", "Run, %s, run!"), player_name);
		/* Turn into a NINJA */
		change_race(RACE_NINJA, "");
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA | PU_HP);

	p_ptr->redraw |= (PR_EQUIPPY | PR_HP | PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();
}


/*:::光の三妖精専用の装備変更*/
void do_cmd_wield_3_fairies(void)
{
	int item, slot;
	bool asia = FALSE;

	object_type forge,forge2;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	/*:::右手が盾で左手が武器など、持ち替えをするときのフラグ*/
	int need_switch_wielding = 0;

	/*:::右手にクロスボウを持ち左手が弓で両方は持たない時など、特定スロットの装備を外すときのフラグを追加*/
	int need_takeoff = 0;
	object_type *takeoff_o_ptr;
	takeoff_o_ptr = &forge2;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print(_("この姿では装備を変更できない。", "You cannot change equipment in your current form."));
		return ;
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item */
	q = _("どれを装備しますか? ", "Wear/Wield which item? ");
	s = _("装備可能なアイテムがない。", "You have nothing you can wear or wield.");
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;
	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	/* Check the slot */
	slot = wield_slot_3_fairies(o_ptr);

	//どのスロットに装備するか選ぶ必要があるとき
	switch (o_ptr->tval)
	{
	//妖魔本(盾は別部位)
	case TV_CAPTURE:
	case TV_MAGICITEM:
		/*:::二刀流の場合どちらの武器と取り替えるか選ぶ*/
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			/* Restrict the choices */

			//v1.1.93 ここ独自のmelee_weapon判定関数を削除して普通のに変えた。TV_MAGICWEAPONが新たに対象になる
			//item_tester_hook = item_tester_hook_melee_weapon;
			item_tester_hook = object_is_melee_weapon;

			item_tester_no_ryoute = TRUE;
			q = _("どちらの武器と取り替えますか?", "Replace which weapon? ");
			s = _("おっと。", "Oops.");

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM) need_switch_wielding = INVEN_LARM;
		}
		else if (buki_motteruka(INVEN_LARM)) slot = INVEN_RARM;

		/*:::左右どちらかの手に射撃武器を持っていて新たに盾などを装備する場合の処理を追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::右手に射撃武器を持ち、左手に何も持ってないとき（クロスボウか銃を両手持ち、もしくはクロスボウか銃と手綱）は、そのまま盾などを左手に装備*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				;
			}
			/*:::左手に射撃武器を持ち、右手に何も持ってないとき（弓を両手持ち、もしくは弓と手綱）は、盾などを右手に装備*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				slot = INVEN_RARM;
			}
			/*:::両手に射撃武器を含む何かを持っているとき　まずはどちらを外すか選ぶ*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
				q = _("どちらの武器と取り替えますか?", "Replace which weapon? ");
				s = _("おっと。", "Oops.");
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::右手の装備と取り替え、左手に残ったのが近接武器・銃・クロスボウなら持ち替える*/
					if(object_is_melee_weapon(&inventory[INVEN_LARM]) || inventory[INVEN_LARM].tval == TV_CROSSBOW || inventory[INVEN_LARM].tval == TV_GUN)
					{
						need_switch_wielding = INVEN_LARM;
					}
					/*:::左手にあるのが弓、盾などならそのまま装備する*/
				}
				else
				{
					/*:::左手の装備と取り替え、右手に残ったのが弓なら持ち替える*/
					if(inventory[INVEN_RARM].tval == TV_BOW)
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Both arms are already used by non-weapon */
		/*:::盾などを両手に持っている場合どちらかと入れ替える*/
		else if (inventory[INVEN_RARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_RARM]) &&
		         inventory[INVEN_LARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_LARM]))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "どちらの手に装備しますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
		}
		break;

	/* Melee weapons */
	///item TVAL
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
	case TV_OTHERWEAPON:
	case TV_MAGICWEAPON:


		///mod140309 弓枠廃止関連
		/*:::左右どちらかの手に射撃武器を持っていて新たに近接武器を装備する場合の処理を追加*/
		if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::右手に射撃武器を持ち、左手に何も持ってないとき（クロスボウか銃を両手持ち、もしくはクロスボウか銃と手綱）は、射撃武器を左手に持ち替え*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				slot = INVEN_LARM;
				need_switch_wielding = INVEN_RARM;
			}
			/*:::左手に射撃武器を持ち、右手に何も持ってないとき（弓を両手持ち、もしくは弓と手綱）は、新たな近接武器をそのまま右手に装備*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				;
			}
			/*:::両手に射撃武器を含む何かを持っているとき　まずはどちらを外すか選ぶ*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "どちらの装備品と取り替えますか?";
				s = "おっと。";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::右手の装備と取り替えた場合、左手に持っているのが何であろうとそのまま*/
					;
				}
				else
				{
					/*:::左手の装備と取り替えた場合、右手に持っているのが近接武器以外なら持ち替える*/
					if(!object_is_melee_weapon(&inventory[INVEN_RARM]))
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		//二刀流確認処理を削除

		/* Both arms are already used */
		else if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			q = _("どちらの手に装備しますか?", "Equip which hand? ");
			s = _("おっと。", "Oops.");

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if ((slot == INVEN_LARM) && !buki_motteruka(INVEN_RARM))
				need_switch_wielding = INVEN_RARM;
		}
		break;


///mod140309 弓枠廃止処理関連
	/*:::弓を新たに装備　slotは常にINVEN_LARM*/
	case TV_BOW:
		/*:::両手にそれぞれ何かを持ってる場合、どちらを外すか選んで残った方は常に右手で持つ*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "どちらの装備品と持ち替えますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM)	need_switch_wielding = INVEN_LARM;
		}
		/*:::射撃武器のみ持っているとき、両手に持つか確認しNOなら新しい方だけ持つ　フラグ追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (!get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) need_takeoff = INVEN_RARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) slot = INVEN_RARM;
		}
		/*:::近接武器のみ持っているとき（必ず右手のはず）、追加処理なし　弓は左手で持つ*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			;
		}
		/*:::盾や妖魔本のみ持っているとき（必ず左手のはず）、弓を左手に持ってそれらは右手へ*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			slot = INVEN_RARM;
			need_switch_wielding = INVEN_LARM;
		}
		/*:::何も装備していないとき、弓は左手へ*/
		else
		{
			;
		}
		break;

	/*:::銃やクロスボウを新たに装備　slotは右手に何か持ってたらINVEN_LARM*/
	case TV_CROSSBOW:
	case TV_GUN:
		/*:::両手にそれぞれ何かを持ってる場合、まずどちらを外すか選ぶ*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "どちらの装備品と持ち替えますか?";
			s = "おっと。";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			/*:::残ったのが近接武器ならそちらを右手に持ち銃かクロスボウは左手へ*/
			if (slot == INVEN_RARM && object_is_melee_weapon(&inventory[INVEN_LARM]))
			{
				need_switch_wielding = INVEN_LARM;
			}
			/*:::残ったのが盾や妖魔本ならそれを左手に持つ*/
			if (slot == INVEN_LARM && !object_is_melee_weapon(&inventory[INVEN_RARM]))
			{
				need_switch_wielding = INVEN_RARM;
			}
		}
		/*:::射撃武器のみ持っているとき、両手に持つか確認しNOなら新しい方だけ持つ　フラグ追加*/
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (!get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) need_takeoff = INVEN_LARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (get_check(_("両手にそれぞれ射撃武器を持ちますか？",
                            "Do you want to dual wield ranged weapons?"))) slot = INVEN_LARM;
			else slot = INVEN_RARM;
		}
		/*:::近接武器のみ持っているとき（必ず右手のはず）、銃とクロスボウは左手で持つ*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			slot = INVEN_LARM;
		}
		/*:::盾や妖魔本のみ持っているとき（必ず左手のはず）、追加処理なし　銃とクロスボウは右手へ*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			;
		}
		/*:::何も装備していないとき、銃とクロスボウは右手へ*/
		else
		{
			;
		}
		break;

		case TV_RIBBON:
		case TV_RING:
		case TV_AMULET:
		case TV_WHISTLE:
		case TV_DRAG_ARMOR:
		case TV_ARMOR:
		case TV_CLOTHES:
		case TV_CLOAK:
		case TV_HEAD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_SHIELD:

		//v1.1.68 三月精の通常性格では盾はここでの処理不要
		if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) && o_ptr->tval == TV_SHIELD)
			break;

		//既に全スロット埋まっているときここに入るはず
		if (slot == INVEN_FEET && inventory[INVEN_FEET].k_idx)
		{
			q = _("どの装備品と取り替えますか?", "Replace which equipment?");
			s = _("おっと。", "Oops.");
		}

		/*::: hack - select_ring_slotの役割を上書きして武器、盾、光源以外の全スロットを対象にしてしまう*/
		select_ring_slot = TRUE;
		item_tester_no_ryoute = TRUE;

		if (!get_item(&slot, q, s, (USE_EQUIP)))
		{
			select_ring_slot = FALSE;
			return;
		}
		select_ring_slot = FALSE;
		break;
	}
	/*:::武器・盾・リングのスロット決定処理終了*/

	if(!(wield_check(slot,item,0))) return;


	/* Prevent wielding into a cursed slot */
	if (object_is_cursed(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], (OD_OMIT_PREFIX | OD_NAME_ONLY));

		/* Message */
#ifdef JP
		msg_format("%s%sは呪われているようだ。",
			   describe_use(slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(slot));
#endif

		/* Cancel the command */
		return;
	}

	if (confirm_wear &&
		((object_is_cursed(o_ptr) && object_is_known(o_ptr)) ||
		((o_ptr->ident & IDENT_SENSE) &&
			(FEEL_BROKEN <= o_ptr->feeling) && (o_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "本当に%s{呪われている}を使いますか？", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif

		if (!get_check(dummy)) return;
	}
	///item system 石仮面の種族変容 アンドロイド（妖怪人形）も変異するようにしようと思ったがよく考えたら経験値計算が大変なことになる
	if ((o_ptr->name1 == ART_STONEMASK) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		char dummy[MAX_NLEN+80];

		int j;

		for (j = 0; j < INVEN_PACK; j++) if(inventory[j].tval == TV_SOUVENIR && inventory[j].sval == SV_SOUVENIR_ASIA) asia = TRUE;

		if(asia)
		{
			msg_print(_("仮面の額に「エイジャの赤石」がぴったり収まっている。",
                        "'Red Stone of Aja' would perfectly fit in the forehead of your mask."));
			msg_print(NULL);
			sprintf(dummy, _("このまま装備してみますか？", "Wear it as it is?"));
			if (!get_check(dummy)) return;
		}
		else if(p_ptr->prace != RACE_VAMPIRE)
		{
			/* Describe it */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			sprintf(dummy, _("%sを装備すると吸血鬼になります。よろしいですか？",
                            "Wearing %s will turn you into a vampire. Are you sure?"), o_name);
			if (!get_check(dummy)) return;
		}
	}
	///mod140115 メンポの種族変容 妖怪人形は変身しない
	if ((o_ptr->name1 == ART_MENPO) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "%sを装備するとニンジャになります。よろしいですか？", o_name);
#else
        sprintf(dummy, "Wearing %s will turn you into a ninja. Are you sure?", o_name);
#endif

		if (!get_check(dummy)) return;
	}


	if (need_switch_wielding && !object_is_cursed(&inventory[need_switch_wielding]))
	{
		object_type *slot_o_ptr = &inventory[slot];
		object_type *switch_o_ptr = &inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmp_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		object_copy(otmp_ptr, switch_o_ptr);
		object_copy(switch_o_ptr, slot_o_ptr);
		object_copy(slot_o_ptr, otmp_ptr);

#ifdef JP
		msg_format("%sを持ち替えた。", switch_name);
#else
		/*
		 * Skipping this English message because of the more detailed
		 * message below (which is displayed in the English version but
		 * not the Japanese version).
		 */
#if 0
		msg_format("You switch %s to your other hand.", switch_name);
#endif
#endif

#ifdef JP
	//	msg_format("%sを%sに構えなおした。", switch_name, (slot == INVEN_RARM) ? (left_hander ? "左手" : "右手") : (left_hander ? "右手" : "左手"));
#else
		msg_format("You wield %s at %s hand.", switch_name, (slot == INVEN_RARM) ? (left_hander ? "left" : "right") : (left_hander ? "right" : "left"));
#endif

		slot = need_switch_wielding;
	}

	/*:::★入手型クエストで該当アイテムを拾ったかチェック*/
	/*:::宝物庫のターゲットを床から直接装備したときのための処理と思われる*/
	check_find_art_quest_completion(o_ptr);


	///seikaku
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		identify_item(o_ptr);

		/* Auto-inscription */
		autopick_alter_item(item, FALSE);
	}

	/* Take a turn */
	energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}
	else if(need_takeoff)
	{
		(void)inven_takeoff(need_takeoff, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Increase the weight */
	p_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;
/*
#ifdef JP
#define STR_WIELD_RARM "%s(%c)を右手に装備した。"
#define STR_WIELD_LARM "%s(%c)を左手に装備した。"
#define STR_WIELD_ARMS "%s(%c)を両手で構えた。"
#else
#define STR_WIELD_RARM "You are wielding %s (%c) in your right hand."
#define STR_WIELD_LARM "You are wielding %s (%c) in your left hand."
#define STR_WIELD_ARMS "You are wielding %s (%c) with both hands."
#endif
*/
	/* Where is the item now */
	switch (slot)
	{
	case INVEN_RARM:
	case INVEN_LARM:
#ifdef JP
		act = "%s(%c)を構えた。";
#else
		act = "You are shooting with %s (%c).";
#endif
		break;

	case INVEN_LITE:
#ifdef JP
		act = "%s(%c)を光源にした。";
#else
		act = "Your light source is %s (%c).";
#endif
		break;

	default:
#ifdef JP
		act = "%s(%c)を装備した。";
#else
		act = "You are wearing %s (%c).";
#endif
		break;
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, 0);

	/* Message */
	msg_format(act, o_name, index_to_label(slot));


	/* Cursed! */
	if (object_is_cursed(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("うわ！ すさまじく冷たい！");
#else
		msg_print("Oops! It feels deathly cold!");
#endif


		//chg_virtue(V_HARMONY, -1);

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	///item race 石仮面の種族変容
	/* The Stone Mask make the player turn into a vampire! */
	if ((o_ptr->name1 == ART_STONEMASK) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		/* Turn into a vampire */
		if(asia) change_race(RACE_ULTIMATE, "");
		else if(p_ptr->prace != RACE_VAMPIRE) change_race(RACE_VAMPIRE, "");
	}
	///mod140115 メンポの種族変容　妖怪人形は変身しない
	if (o_ptr->name1 == ART_MENPO && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		/*:::生い立ち欄が書き換わる*/
		sprintf(p_ptr->history[0], _("あなたは%sとして平穏に生きていた。", "You led a peaceful life as a %s."),rp_ptr->title);
		sprintf(p_ptr->history[1], _("しかしある日あなたに平安時代のニンジャ・ソウルが憑依した！",
                                    "But one day you got possessed by Heian era Ninja Soul!"));
		if(one_in_(6))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、太歳星君への復讐だ！",
                                        "Your goal: Take revenge on Taisui Xingjun!"));
		else if(one_in_(5))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、月の女神嫦娥への復讐だ！",
                                        "Your goal: Take revenge on the moon goddess Chang'e!"));
		else if(one_in_(4))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、『混沌のサーペント』への復讐だ！",
                                        "Your goal: Take revenge on the Serpent of Chaos!"));
		else if(one_in_(3))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、全てのニンジャの抹殺だ！",
                                        "Your goal: Exterminate all ninjas!"));
		else if(one_in_(2))
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、自らを虐げた者達への復讐だ！",
                                        "Your goal: Take revenge on those who have oppressed you!"));
		else
			sprintf(p_ptr->history[2], _("あなたの目的は唯一つ、全アンバーの王族への復讐だ！",
                                        "Your goal: Take revenge on all of the Lords of Amber!"));
		sprintf(p_ptr->history[3], _("走れ、%s、走れ！", "Run, %s, run!"), player_name);
		/* Turn into a NINJA */
		change_race(RACE_NINJA, "");
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	p_ptr->redraw |= (PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();
}


/*:::装備品を投げたり落としたり外したり売ったりしたとき、残った装備品を適切に装備し直す*/
/*:::item:外した装備品のスロット*/
///mod140309 弓枠廃止関連処理追加
void kamaenaoshi(int item)
{
	object_type *o_ptr, *new_o_ptr;
	char o_name[MAX_NLEN];

	if (item == INVEN_RARM)
	{
		/*:::右手装備を外して左手に銃やクロスボウを持っていた時、右手に持ち替える*/
		if( inventory[INVEN_LARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_CROSSBOW)
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);
			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_RARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_LARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_LARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif

				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format(_("%sを構え直した。", "You are wielding %s."), o_name);
				else
#ifdef JP
					msg_format("%sを%sで構えた。", o_name, (left_hander ? "左手" : "右手"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}
		else if (buki_motteruka(INVEN_LARM))
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);

			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_RARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_LARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_LARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format(_("%sを構え直した。", "You are wielding %s."), o_name);
				else
#ifdef JP
					msg_format("%sを%sで構えた。", o_name, (left_hander ? "左手" : "右手"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}
		else if(inventory[INVEN_LARM].tval == TV_BOW)
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			if(p_ptr->pclass != CLASS_3_FAIRIES)
				msg_format("%sを両手で構えた。", o_name);
#else
			msg_format("You are wielding %s with both hands.", o_name);
#endif

		}

	}
	else if (item == INVEN_LARM)
	{
		o_ptr = &inventory[INVEN_RARM];
		if (o_ptr->k_idx) object_desc(o_name, o_ptr, 0);

		/*:::左手装備を外して右手に弓を持っていた時、左手に持ち替える*/
		if( inventory[INVEN_RARM].tval == TV_BOW)
		{
			o_ptr = &inventory[INVEN_RARM];
			object_desc(o_name, o_ptr, 0);
			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_LARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_RARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_RARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format(_("%sを構え直した。", "You are wielding %s."), o_name);
				else
#ifdef JP
					msg_format("%sを%sで構えた。", o_name, (left_hander ? "左手" : "右手"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%sを両手で構えた。", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}

		else if (buki_motteruka(INVEN_RARM) || o_ptr->tval == TV_CROSSBOW || o_ptr->tval == TV_GUN)
		{
			if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
				msg_format("%sを両手で構えた。", o_name);
#else
				msg_format("You are wielding %s with both hands.", o_name);
#endif
		}
		else if (!(empty_hands(FALSE) & EMPTY_HAND_RARM) && !object_is_cursed(o_ptr))
		{
			new_o_ptr = &inventory[INVEN_LARM];
			object_copy(new_o_ptr, o_ptr);
			p_ptr->total_weight += o_ptr->weight;
			inven_item_increase(INVEN_RARM, -((int)o_ptr->number));
			inven_item_optimize(INVEN_RARM);
#ifdef JP
			msg_format("%sを持ち替えた。", o_name);
#else
			msg_format("You switched hand of %s.", o_name);
#endif
		}
	}
}


/*
 * Take off an item
 */
/*:::装備を外す*/
///item TVAL
void do_cmd_takeoff(void)
{
	int item;
	bool flag_quickdraw = FALSE;

	object_type *o_ptr;

	cptr q, s;
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print(_("この姿では装備を変更できない。", "You cannot change equipment in your current form."));
		return ;
	}
	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "どれを装備からはずしますか? ";
	s = "はずせる装備がない。";
#else
	q = "Take off which item? ";
	s = "You are not wearing anything to take off.";
#endif
	/*:::外す装備を選択。装備欄一覧で十分なのでhookの指定は不要*/
	if (!get_item(&item, q, s, (USE_EQUIP))) return;

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

	if(o_ptr->tval == TV_GUN && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
		flag_quickdraw = TRUE;

	///mod140122 装備制限
	if(!(wield_check(item,INVEN_PACK,0))) return;

	///sys 装備限定職、種族処理記述要 （装備解除）
	//v1.1.28 ちょっと記述と仕様変更
	/* Item is cursed */
	if (object_is_cursed(o_ptr) )
	{
		bool force_remove1 = FALSE; //無理やり外すフラグ
		bool force_remove2 = FALSE; //宥めて外すフラグ

		if(o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
			msg_print(_("装備を外せない。この装備品の呪いはあまりにも強い！",
                        "You can't remove it. The curse is too powerful!"));
			return;
		}

		if(p_ptr->pclass == CLASS_FLAN) force_remove1 = TRUE;
		if(p_ptr->pclass == CLASS_YUGI) force_remove1 = TRUE;
		if(p_ptr->pseikaku == SEIKAKU_BERSERK) force_remove1 = TRUE;

		if(p_ptr->pclass == CLASS_TSUKUMO_MASTER) force_remove2 = TRUE;
		if(p_ptr->prace == RACE_TSUKUMO) force_remove2 = TRUE;
		if (p_ptr->pclass == CLASS_MIZUCHI) force_remove2 = TRUE;

		if(force_remove1 || force_remove2)
		{
			bool success = FALSE;

			if(o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				if(one_in_(7))
				{
					if(force_remove2)
						msg_print(_("強力に呪われた品をなだめて何とか外れてもらった。",
                                    "You persuade the strongly cursed item to let go."));
					else
						msg_print(_("強力な呪いを力づくで剥がした！",
                                    "You tear off the strong curse by force!"));
					success = TRUE;
				}
				else
				{
					if(force_remove2)
					{
						msg_print(_("強力に呪われた品をなだめて外そうとしたが失敗した。",
                                    "You failed to persuade the strongly cursed item to let go."));
					}
					else
					{
						msg_print(_("強力な呪いを力づくで剥がそうとしたが抵抗された！",
                                    "You try to tear off the strong curse by force, but it resists!"));
						if(one_in_(2))
						{
							msg_print(_("ダメージを受けた！", "You take damage!"));
							take_hit(DAMAGE_NOESCAPE,randint1(50),_("強力な呪いを力ずくで剥がそうとした痛み",
                                                                    "pain from trying to forcefully remove a strong curse"),-1);
						}
					}
				}
			}
			else
			{
				if(one_in_(4))
				{
					if(force_remove2)
						msg_print(_("呪われた品をなだめて何とか外れてもらった。",
                                    "You persuade the cursed item to let go."));
					else
						msg_print(_("呪われた装備を力づくで剥がした！",
                                    "You tear off the cursed equipment by force!"));
					success = TRUE;
				}
				else
				{
					if(force_remove2)
						msg_print(_("呪われた品をなだめようとしたが失敗した。",
                                    "You failed to persuade the cursed item to let go."));
					else
					{
						msg_print(_("呪われた装備を力づくで剥がそうとしたが抵抗された！",
                                    "You try to tear off the cursed equipment by force, but it resists!"));
						if(one_in_(4))
						{
							msg_print(_("ダメージを受けた！", "You take damage!"));
							take_hit(DAMAGE_NOESCAPE,randint1(20),_("呪いを力ずくで剥がそうとした痛み",
                                                                    "pain from trying to forcefully remove a curse"),-1);
						}
					}
				}
			}

			if(success)
			{
				o_ptr->ident |= (IDENT_SENSE);

				if(force_remove1)
				{
					o_ptr->curse_flags = 0L;
					/* Take note */
					o_ptr->feeling = FEEL_NONE;
					msg_print(_("呪いを打ち破った。", "The curse is broken."));
				}
				/* Recalculate the bonuses */
				p_ptr->update |= (PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}
			else
			{
				return;
			}
		}
		else
		{
			msg_print(_("装備を外せない。どうやら呪われているようだ。",
                        "You can't remove the equipment. Looks like it's cursed."));
			return;
		}

#if 0
		///class 狂戦士が呪われた装備を剥がす特殊処理
		///mod140119 付喪神も追加した　呪い解除はされない
		if ((o_ptr->curse_flags & TRC_PERMA_CURSE) || (p_ptr->pclass != CLASS_TSUKUMO_MASTER && p_ptr->prace != RACE_TSUKUMO && p_ptr->pclass != CLASS_FLAN))
		{
			/* Oops */
#ifdef JP
			msg_print("ふーむ、どうやら呪われているようだ。");
#else
			msg_print("Hmmm, it seems to be cursed.");
#endif

			/* Nope */
			return;
		}
		/*:::狂戦士は力づくで装備をはがす　狂戦士(と付喪神)以外はすでにreturnしている*/
		if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && one_in_(7)) || one_in_(4))
		{
#ifdef JP

			if(p_ptr->pclass == CLASS_FLAN ) msg_print("呪われた装備を力づくで剥がした！");
			else msg_print("呪われた品をなだめて何とか外れてもらった。");
#else
			msg_print("You teared a cursed equipment off by sheer strength!");
#endif

			/* Hack -- Assume felt */
			o_ptr->ident |= (IDENT_SENSE);

			if(p_ptr->pclass == CLASS_FLAN )
			{
				o_ptr->curse_flags = 0L;
				/* Take note */
				o_ptr->feeling = FEEL_NONE;
#ifdef JP
				msg_print("呪いを打ち破った。");
#else
				msg_print("You break the curse.");
#endif
			}
			/* Recalculate the bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);


		}
		else
		{
#ifdef JP
			msg_print("装備を外せなかった。");
#else
			msg_print("You couldn't remove the equipment.");
#endif
			energy_use = 50;
			return;
		}
#endif

	}


	//九十九姉妹演奏中断
	stop_tsukumo_music();

	/* Take a partial turn */
	if(flag_quickdraw)
	{
		msg_print(_("あなたは素早く銃を納めた。", "You quickly holster your gun."));
		energy_use = 25;
	}
	else
		energy_use = 50;

	/* Take off the item */
	(void)inven_takeoff(item, 255);

	/*:::両手持ちなどの再判定？*/
	kamaenaoshi(item);

	calc_android_exp();

	p_ptr->redraw |= (PR_EQUIPPY);
	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

}


/*
 * Drop an item
 */
/*:::アイテムを落とす*/
void do_cmd_drop(void)
{
	int item, amt = 1;

	object_type *o_ptr;

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "どのアイテムを落としますか? ";
	s = "落とせるアイテムを持っていない。";
#else
	q = "Drop which item? ";
	s = "You have nothing to drop.";
#endif

	///mod160224 紫で金をアイテム欄に拾えるバグへの対応のため全アイテム表示ON
	//だったが、dコマンドリピートで空アイテムが落ちるなどバグ頻出するのでもう戻す
	//item_tester_full = TRUE;
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		//item_tester_full = FALSE;
		return;
	}
	//item_tester_full = FALSE;

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

	///mod140122 装備制限
	if(!(wield_check(item,INVEN_PACK,0))) return;


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_RARM) && object_is_cursed(o_ptr))
	{
		///sys 装備限定職、種族処理記述要3
		/* Oops */
#ifdef JP
		msg_print("ふーむ、どうやら呪われているようだ。");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(item, amt);

	if (item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	p_ptr->redraw |= (PR_EQUIPPY);
}

///sysdel 魔法書破壊で経験値を得るための判定
static bool high_level_book(object_type *o_ptr)
{
	/*
	if ((o_ptr->tval == TV_LIFE_BOOK) ||
	    (o_ptr->tval == TV_SORCERY_BOOK) ||
	    (o_ptr->tval == TV_NATURE_BOOK) ||
	    (o_ptr->tval == TV_CHAOS_BOOK) ||
	    (o_ptr->tval == TV_DEATH_BOOK) ||
	    (o_ptr->tval == TV_TRUMP_BOOK) ||
	    (o_ptr->tval == TV_CRAFT_BOOK) ||
	    (o_ptr->tval == TV_DAEMON_BOOK) ||
	    (o_ptr->tval == TV_CRUSADE_BOOK) ||
	    (o_ptr->tval == TV_MUSIC_BOOK) ||
		(o_ptr->tval == TV_HEX_BOOK))
	{
		if (o_ptr->sval > 1)
			return TRUE;
		else
			return FALSE;
	}
	*/
	return FALSE;
}


/*
 * Destroy an item
 */
/*:::アイテムを壊す*/
/*:::command_argが設定されている場合確認しない？*/
void do_cmd_destroy(void)
{
	int			item, amt = 1;
	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;
	object_type             forge;
	object_type             *q_ptr = &forge;

	char		o_name[MAX_NLEN];

	char		out_val[MAX_NLEN+40];

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;


	/* Get an item */
#ifdef JP
	q = "どのアイテムを壊しますか? ";
	s = "壊せるアイテムを持っていない。";
#else
	q = "Destroy which item? ";
	s = "You have nothing to destroy.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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

	/* Verify unless quantity given beforehand */
	/*:::command_argが1以上でアイテムが無価値でないか確認オプションがONの場合確認する*/
	if (!force && (confirm_destroy || (object_value(o_ptr) > 0)))
	{
		object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

		/* Make a verification */
		sprintf(out_val,
#ifdef JP
			"本当に%sを壊しますか? [y/n/Auto]",
#else
			"Really destroy %s? [y/n/Auto]",
#endif
			o_name);

		msg_print(NULL);

		/* HACK : Add the line to message buffer */
		message_add(out_val);
		p_ptr->window |= (PW_MESSAGE);
		window_stuff();

		/* Get an acceptable answer */
		/*:::アイテムを壊すかどうか確認し、選択によっては自動破壊の対象に追加*/
		while (TRUE)
		{
			char i;

			/* Prompt */
			prt(out_val, 0, 0);

			i = inkey();

			/* Erase the prompt */
			prt("", 0, 0);


			if (i == 'y' || i == 'Y')
			{
				break;
			}
			if (i == ESCAPE || i == 'n' || i == 'N')
			{
				/* Cancel */
				return;
			}
			if (i == 'A')
			{
				/* Add an auto-destroy preference line */
				if (autopick_autoregister(o_ptr))
				{
					/* Auto-destroy it */
					autopick_alter_item(item, TRUE);
				}

				/* The object is already destroyed. */
				return;
			}
		} /* while (TRUE) */
	}

	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, 0);
	o_ptr->number = old_number;

	/* Take a turn */
	energy_use = 100;

	/* Artifacts cannot be destroyed */
	/*:::特別製は壊せない。未鑑定なら簡易鑑定する*/
	if (!can_player_destroy_object(o_ptr))
	{
		energy_use = 0;

		/* Message */
#ifdef JP
		msg_format("%sは破壊不可能だ。", o_name);
#else
		msg_format("You cannot destroy %s.", o_name);
#endif

		/* Done */
		return;
	}

	object_copy(q_ptr, o_ptr);

	/* Message */
#ifdef JP
	msg_format("%sを壊した。", o_name);
#else
	msg_format("You destroy %s.", o_name);
#endif

	sound(SOUND_DESTITEM);

	/* Reduce the charges of rods/wands */
	/*:::ロッドと魔法棒の充填数処理*/
	reduce_charges(o_ptr, amt);

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/*:::一部職が上位魔法書を壊すと経験値を得る処理*/
	///sysdel class 戦士やパラディンの魔法書破壊経験値獲得処理
#if 0
	if (high_level_book(q_ptr))
	{
		bool gain_expr = FALSE;

		if (p_ptr->prace == RACE_ANDROID)
		{
		}
		else if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_BERSERKER))
		{
			gain_expr = TRUE;
		}
		else if (p_ptr->pclass == CLASS_PALADIN)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
			else
			{
				if (is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
		}

		if (gain_expr && (p_ptr->exp < PY_MAX_EXP))
		{
			s32b tester_exp = p_ptr->max_exp / 20;
			if (tester_exp > 10000) tester_exp = 10000;
			if (q_ptr->sval < 3) tester_exp /= 4;
			if (tester_exp<1) tester_exp = 1;

#ifdef JP
msg_print("更に経験を積んだような気がする。");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(tester_exp * amt);
		}
		if (high_level_book(q_ptr) && q_ptr->tval == TV_LIFE_BOOK)
		{
			chg_virtue(V_UNLIFE, 1);
			chg_virtue(V_VITALITY, -1);
		}
		else if (high_level_book(q_ptr) && q_ptr->tval == TV_DEATH_BOOK)
		{
			chg_virtue(V_UNLIFE, -1);
			chg_virtue(V_VITALITY, 1);
		}

		if (q_ptr->to_a || q_ptr->to_h || q_ptr->to_d)
			chg_virtue(V_ENCHANT, -1);

		if (object_value_real(q_ptr) > 30000)
			chg_virtue(V_SACRIFICE, 2);

		else if (object_value_real(q_ptr) > 10000)
			chg_virtue(V_SACRIFICE, 1);
	}
	///sysdel virtue
	if (q_ptr->to_a != 0 || q_ptr->to_d != 0 || q_ptr->to_h != 0)
		chg_virtue(V_HARMONY, 1);
#endif
	if (item >= INVEN_RARM) calc_android_exp();
}


/*
 * Observe an item which has been *identify*-ed
 */
/*:::アイテムの詳細情報を調べる*/
void do_cmd_observe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "どのアイテムを調べますか? ";
	s = "調べられるアイテムがない。";
#else
	q = "Examine which item? ";
	s = "You have nothing to examine.";
#endif

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


	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		///mod150121 慧音白沢時特殊処理
		if(p_ptr->pclass == CLASS_KEINE && p_ptr->magic_num1[0] )
		{
			msg_print(_("このアイテムについての知識が頭に流れ込んでくる！",
                        "Knowledge about this item flows through your mind!"));
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->ident |= (IDENT_MENTAL);
			p_ptr->update |= (PU_BONUS);
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
		}
		else
		{
			msg_print(_("このアイテムについて特に知っていることはない。",
                        "You don't know anything special about this item."));
			return;
		}
	}


	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
#ifdef JP
	msg_format("%sを調べている...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif

	/* Describe it fully */
#ifdef JP
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("特に変わったところはないようだ。");
#else
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("You see nothing special.");
#endif

}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
/*:::アイテムの銘を消す*/
void do_cmd_uninscribe(void)
{
	int   item;

	object_type *o_ptr;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "どのアイテムの銘を消しますか? ";
	s = "銘を消せるアイテムがない。";
#else
	q = "Un-inscribe which item? ";
	s = "You have nothing to un-inscribe.";
#endif

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

	/* Nothing to remove */
	if (!o_ptr->inscription)
	{
#ifdef JP
		msg_print("このアイテムには消すべき銘がない。");
#else
		msg_print("That item had no inscription to remove.");
#endif

		return;
	}

	/* Message */
#ifdef JP
	msg_print("銘を消した。");
#else
	msg_print("Inscription removed.");
#endif


	/* Remove the incription */
	/*:::銘へのインデックスを削除　quarkに格納された銘本体には何もしないらしい*/
	o_ptr->inscription = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* .や$の関係で, 再計算が必要なはず -- henkma */
	p_ptr->update |= (PU_BONUS);

}


/*
 * Inscribe an object with a comment
 */
/*:::アイテムに銘を刻む*/
void do_cmd_inscribe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	char		out_val[80];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "どのアイテムに銘を刻みますか? ";
	s = "銘を刻めるアイテムがない。";
#else
	q = "Inscribe which item? ";
	s = "You have nothing to inscribe.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_format(_("いまそのアイテムには触れない。", "You can't touch that item right now."));
			return;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe the activity */
	object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

	/* Message */
#ifdef JP
	msg_format("%sに銘を刻む。", o_name);
#else
	msg_format("Inscribing %s.", o_name);
#endif

	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription */
	if (o_ptr->inscription)
	{
		/* Start with the old inscription */
		strcpy(out_val, quark_str(o_ptr->inscription));
	}

	/* Get a new inscription (possibly empty) */
#ifdef JP
	if (get_string("銘: ", out_val, 80))
#else
	if (get_string("Inscription: ", out_val, 80))
#endif
	{
		/* Save the inscription */
		o_ptr->inscription = quark_add(out_val);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* .や$の関係で, 再計算が必要なはず -- henkma */
		p_ptr->update |= (PU_BONUS);
	}
}



/*
 * An "item_tester_hook" for refilling lanterns
 */
/*:::ランタンに補給　油壷か他のランタン*/
///mod140819 TV_FLASKに他のアイテム増やしたので油壺だけ対象にするよう修正
static bool item_tester_refill_lantern(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL) return (TRUE);

	/* Laterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
/*:::ランタンへの燃料補給*/
///item ego ランタンへの燃料補給
static void do_cmd_refill_lamp(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;

	if(check_invalidate_inventory(INVEN_LITE))
	{
		msg_print(_("いまは光源を扱えない。", "You can't use a light source right now."));
		return;
	}

	if (world_player || SAKUYA_WORLD)
	{
		msg_print(_("ランタンの灯は凍ったようにゆらめきを止めている・・",
                    "The lantern's light is frozen mid flicker..."));
		return;
	}


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;


	/* Get an item */
#ifdef JP
	q = "どの油つぼから注ぎますか? ";
	s = "油つぼがない。";
#else
	q = "Refill with which flask? ";
	s = "You have no flasks of oil.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the lantern */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4;

	/* Message */
#ifdef JP
	msg_print("ランプに油を注いだ。");
#else
	msg_print("You fuel your lamp.");
#endif

	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("ランプが消えてしまった！");
#else
		msg_print("Your lamp has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("しかしランプは全く光らない。");
#else
		msg_print("Curiously, your lamp doesn't light.");
#endif
	}
	else if (j_ptr->xtra4 >= FUEL_LAMP)
	{
		j_ptr->xtra4 = FUEL_LAMP;
#ifdef JP
		msg_print("ランプの油は一杯だ。");
#else
		msg_print("Your lamp is full.");
#endif

	}

	if(object_is_artifact(o_ptr))
	{
		o_ptr->xtra4 = 0;
		p_ptr->update |= (PU_TORCH | PU_BONUS);
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		return;
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH) && !object_is_artifact(o_ptr)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
///item たいまつの寿命を延ばす
static void do_cmd_refill_torch(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;

	if(check_invalidate_inventory(INVEN_LITE))
	{
		msg_print(_("いまは松明に触れない。", "You can't use torches right now."));
		return;
	}
	if (world_player || SAKUYA_WORLD)
	{
		msg_print(_("松明の灯は凍ったようにゆらめきを止めている・・",
                    "The torch's flame is frozen mid flicker..."));
		return;
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get an item */
#ifdef JP
	q = "どの松明で明かりを強めますか? ";
	s = "他に松明がない。";
#else
	q = "Refuel with which torch? ";
	s = "You have no extra torches.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the primary torch */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4 + 5;

	/* Message */
#ifdef JP
	msg_print("松明を結合した。");
#else
	msg_print("You combine the torches.");
#endif


	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("松明が消えてしまった！");
#else
		msg_print("Your torch has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("しかし松明は全く光らない。");
#else
		msg_print("Curiously, your torch don't light.");
#endif
	}
	/* Over-fuel message */
	else if (j_ptr->xtra4 >= FUEL_TORCH)
	{
		j_ptr->xtra4 = FUEL_TORCH;
#ifdef JP
		msg_print("松明の寿命は十分だ。");
#else
		msg_print("Your torch is fully fueled.");
#endif

	}

	/* Refuel message */
	else
	{
#ifdef JP
		msg_print("松明はいっそう明るく輝いた。");
#else
		msg_print("Your torch glows more brightly.");
#endif

	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * Refill the players lamp, or restock his torches
 */
/*:::燃料補給*/
///item TVAL たいまつかランタンへの燃料補給
void do_cmd_refill(void)
{
	object_type *o_ptr;

	/* Get the light */
	o_ptr = &inventory[INVEN_LITE];

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
#ifdef JP
		msg_print("光源を装備していない。");
#else
		msg_print("You are not wielding a light.");
#endif

	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp();
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH && !object_is_artifact(o_ptr))
	{
		do_cmd_refill_torch();
	}

	/* No torch to refill */
	else
	{
#ifdef JP
		msg_print("この光源は寿命を延ばせない。");
#else
		msg_print("Your light cannot be refilled.");
#endif

	}
}


/*
 * Target command
 */
/*:::ターゲット設定　lコマンドとどう違うのかよく解らん*/
void do_cmd_target(void)
{
	/* Target set */
	if (target_set(TARGET_KILL))
	{
#ifdef JP
		msg_print("ターゲット決定。");
#else
		msg_print("Target Selected.");
#endif

	}

	/* Target aborted */
	else
	{
#ifdef JP
		msg_print("ターゲット解除。");
#else
		msg_print("Target Aborted.");
#endif

	}
}



/*
 * Look command
 */
void do_cmd_look(void)
{
	/*TEST*/
	p_ptr->window |= PW_MONSTER_LIST;
	p_ptr->window |= PW_FLOOR_ITEM_LIST;
	window_stuff();
	/*TEST*/

	/* Look around */
	if (target_set(TARGET_LOOK))
	{
#ifdef JP
		msg_print("ターゲット決定。");
#else
		msg_print("Target Selected.");
#endif

	}
}



/*
 * Allow the player to examine other sectors on the map
 */
/*:::マップを見渡す　詳細未読*/
void do_cmd_locate(void)
{
	int		dir, y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);


	/* Start at current panel */
	y2 = y1 = panel_row_min;
	x2 = x1 = panel_col_min;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
#ifdef JP
			strcpy(tmp_val, "真上");
#else
			tmp_val[0] = '\0';
#endif

		}
		else
		{
#ifdef JP
			sprintf(tmp_val, "%s%s",
				((y2 < y1) ? "北" : (y2 > y1) ? "南" : ""),
				((x2 < x1) ? "西" : (x2 > x1) ? "東" : ""));
#else
			sprintf(tmp_val, "%s%s of",
				((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
				((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
#endif

		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
#ifdef JP
			"マップ位置 [%d(%02d),%d(%02d)] (プレイヤーの%s)  方向?",
#else
			"Map sector [%d(%02d),%d(%02d)], which is%s your sector.  Direction?",
#endif

			y2 / (hgt / 2), y2 % (hgt / 2),
			x2 / (wid / 2), x2 % (wid / 2), tmp_val);

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;

			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command, TRUE)) break;

			/* Extract the action (if any) */
			dir = get_keymap_dir(command);

			/* Error */
			if (!dir) bell();
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		if (change_panel(ddy[dir], ddx[dir]))
		{
			y2 = panel_row_min;
			x2 = panel_col_min;
		}
	}


	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();
}



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
/*:::ang_sort()に使うための整列関数のひとつ？*/
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
/*:::ang_sort()のクイックソート用の入れ替えルーチン*/
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "multiple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
/*:::モンスターの思い出を見る　詳細未読*/
void do_cmd_query_symbol(void)
{
	int		i, n, r_idx;
	char	sym, query;
	char	buf[128];

	bool	all = FALSE;
	bool	uniq = FALSE;
	bool	norm = FALSE;
	bool	ride = FALSE;
	char    temp[80] = "";

	bool	recall = FALSE;

	u16b	why = 0;
	u16b	*who;

	/* Get a character, or abort */
#ifdef JP
	if (!get_com("知りたい文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^R乗馬,^M名前): ", &sym, FALSE)) return;
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE)) return;
#endif

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "全モンスターのリスト");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク・モンスターのリスト");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク外モンスターのリスト");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	else if (sym == KTRL('R'))
	{
		all = ride = TRUE;
#ifdef JP
		strcpy(buf, "乗馬可能モンスターのリスト");
#else
		strcpy(buf, "Ridable monster list.");
#endif
	}
	/* XTRA HACK WHATSEARCH */
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("名前(英語の場合小文字で可)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;
			return;
		}
#ifdef JP
		sprintf(buf, "名前:%sにマッチ",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
		sprintf(buf, "%c - %s", sym, "無効な文字");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif
	}

	/* Display the result */
	prt(buf, 0, 0);

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		///mod140712 可変パラメータモンスターは/コマンドから参照できないようにする
		if(r_ptr->flags7 & RF7_VARIABLE) continue;

		/* Nothing to recall */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require ridable monsters if needed */
		if (ride && !(r_ptr->flags7 & (RF7_RIDING))) continue;

		/* XTRA HACK WHATSEARCH */
		if (temp[0])
		{
		  int xx;
		  char temp2[80];

		  for (xx=0; temp[xx] && xx<80; xx++)
		  {
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }

#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);

#ifdef JP
		  if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp) )
#else
		  if (my_strstr(temp2, temp))
#endif
			  who[n++]=i;
		}

		/* Collect "appropriate" monsters */
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}


	/* Prompt XXX XXX XXX */
#ifdef JP
	put_str("思い出を見ますか? (k:打倒数順/y/n): ", 0, 36);
#else
	put_str("Recall details? (k/y/n): ", 0, 40);
#endif


	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);

	why = 2;

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array */
	ang_sort(who, &why, n);

	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Catch "escape" */
	if (query != 'y')
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}

	/* Sort if needed */
	if (why == 4)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Auto-recall */
		monster_race_track(r_idx);

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save the screen */
				screen_save();

				/* Recall on screen */
				screen_roff(who[i], 0);
			}

			/* Hack -- Begin the prompt */
			roff_top(r_idx);

			/* Hack -- Complete the prompt */
#ifdef JP
			Term_addstr(-1, TERM_WHITE, " ['r'思い出, ESC]");
#else
			Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
#endif

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Restore */
				screen_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Re-display the identity */
	prt(buf, 0, 0);
}


/*:::現在の種族や職業などで選択したアイテムを装備（あるいは装備外し）可能かチェックする。*/
/*:::slotはアイテムの装備箇所のINVEN_???番号 何か装備されている場合それが外されることになる。*/
/*:::item_newは新しく装備するアイテムのINVEN_???番号（ザック）あるいは負の場合床上のo_list添え字*/
/*:::装備外し、落とし、売却で新たに装備するものがない時item_newにはあふれ領域のINVEN_PACKを入れてこの関数を呼ぶことにした。*/
/*:::これで問題ないとは思うが我ながらかなりひどいやり方だ。ポインタをいまだによく分かっていない。*/
///mod140914 ユニーククラスが石仮面などで種族変容した時外してはいけないものも外れてしまうので条件式を少し変更
//slot==item_newのときはOKにしておく
//v2.0.18 パラメータmodeを追加
//mode0:普通 1:投擲時
bool wield_check( int slot, int item_new, int mode)
{
	object_type *o_ptr_old = &inventory[slot];
	object_type *o_ptr_new;

	if(slot < INVEN_RARM) return TRUE;

	if(item_new >= 0) o_ptr_new = &inventory[item_new];
	else			  o_ptr_new = &o_list[0 - item_new];


	//msg_format("slot:%d oldTV:%d newTV:%d",slot, o_ptr_old->tval, o_ptr_new->tval);

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD)
	{
		msg_print(_("今は装備を変更できない。", "You cannot change equipment in your current form."));
		return (FALSE);
	}

	if(check_invalidate_inventory(slot))
	{
		msg_print(_("今その装備を変更することはできない。", "You cannot swap item in this slot right now."));
		return (FALSE);
	}

	//v2.0.18 一時アイテム「魔法の剣」は投擲時を除き外せない
	if (o_ptr_old->tval == TV_SWORD && o_ptr_old->sval == SV_WEAPON_MAGIC_SWORD && mode != 1)
	{
		msg_print(_("その装備を外すことはできない。", "You cannot remove this equipment."));
		return (FALSE);
	}


	//v1.1.57 一時アイテム「七星剣」は外せない
	if (o_ptr_old->tval == TV_SWORD && o_ptr_old->sval == SV_WEAPON_SEVEN_STAR)
	{
		msg_print(_("その装備を外すことはできない。", "You cannot remove this equipment."));
		return (FALSE);

	}

	//種族処理
	/*:::天狗は常に頭襟を装備*/
	//v1.1.80 専用性格の文は別
	if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU){
		if(slot == INVEN_HEAD && o_ptr_new->sval != SV_HEAD_TOKIN && !is_special_seikaku(SEIKAKU_SPECIAL_AYA))
		{
			msg_print(_("あなたは頭に頭襟を装備しないといけない。",
                        "You have to be wearing a tokin on your head."));
			return (FALSE);
		}
	}

	//職業個別設定

	if(p_ptr->pclass == CLASS_KOKORO)
	{
		if(slot == INVEN_HEAD && o_ptr_new->tval == TV_HEAD)
		{
			msg_print(_("あなたは感情の面以外を被りたくはない。",
                        "You don't want to put anything on your head aside from masks of emotion."));
			return (FALSE);
		}
	}
	//v2.0.12慧ノ子は武器や盾を持てない グリマリなど武器扱いでない特殊アイテムは持てる
	else if (p_ptr->pclass == CLASS_ENOKO)
	{
		if ((slot == INVEN_RARM || slot == INVEN_LARM) && (object_is_weapon(o_ptr_new) || object_is_shooting_weapon(o_ptr_new)|| o_ptr_new->tval == TV_SHIELD))
		{
			msg_print(_("トラバサミに引っかかって持てない。", "You can't hold that - it'll get caught in your beartrap."));
			return (FALSE);
		}
	}

	else if(p_ptr->pclass == CLASS_ALICE)
	{
		if(slot == INVEN_HANDS)
		{
			msg_print(_("それをつけると人形を操れない。", "You won't be able to manipulate dolls while wearing this."));
			return (FALSE);
		}
		if(object_is_weapon(o_ptr_new))
		{
			msg_print(_("あなたは自ら武器を持つつもりはない。", "You have no intention of wielding a weapon yourself."));
			return (FALSE);
		}
		else if(slot == INVEN_RARM)
		{
			msg_print(_("それを持つと人形を操れない。", "You won't be able to manipulate dolls."));
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI)
	{
		if(slot == INVEN_HANDS)
		{
			msg_print(_("小手や手袋は演奏の邪魔だ。", "Gloves would get in the way of your performance."));
			return (FALSE);
		}
	}

	//三月精
	//v1.1.68 専用性格「妖精大集合」では制限なし
	else if(p_ptr->pclass == CLASS_3_FAIRIES && !is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) &&
		slot >= INVEN_RIGHT && slot <= INVEN_FEET && slot != INVEN_LITE)
	{
		int i;
		int cnt = 0;
		for( i= INVEN_RIGHT;i<=INVEN_FEET;i++)
		{
			if(i == INVEN_LITE) continue;
			if(!inventory[i].k_idx) continue;
			if(i == slot) continue;

			if(inventory[i].tval == o_ptr_new->tval) cnt++;
			else if(wield_slot(&inventory[i]) == wield_slot(o_ptr_new)) cnt++;

		}
		if(cnt > 2)
		{
			msg_print(_("すでに三人ともその部位の装備をしている。",
                        "All three of you already have this slot covered."));
			return (FALSE);
		}
	}

	else if(p_ptr->pclass == CLASS_MAID || p_ptr->pclass == CLASS_SAKUYA)
	{
		if(slot == INVEN_BODY && p_ptr->psex == SEX_MALE && (o_ptr_new->tval != TV_CLOTHES || o_ptr_new->sval != SV_CLOTH_SUIT))
		{
			msg_print(_("あなたは常にスーツを着用しないといけない。",
                        "You have to be wearing a suit."));
			return (FALSE);
		}
		if(slot == INVEN_BODY && p_ptr->psex == SEX_FEMALE &&  (o_ptr_new->tval != TV_CLOTHES || o_ptr_new->sval != SV_CLOTH_MAID))
		{
			msg_print(_("あなたは常にメイド服を着用しないといけない。",
                        "You have to be wearing a maid uniform."));
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_KISUME)
	{
		if(slot == INVEN_BODY && (o_ptr_new->tval != TV_ARMOR || o_ptr_new->sval != SV_ARMOR_TUB))
		{
			msg_print(_("あなたは桶から出られない。",
                        "You cannot leave your bucket."));
			return (FALSE);
		}
	}

	//v1.1.37 小鈴一時変身
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI && o_ptr_old->name1 == ART_HYAKKI && slot != item_new)
	{
		msg_print(_("今は絵巻物を手放せない。", "You can't let go of the picture scroll right now."));
		return (FALSE);
	}

	/*:::小傘特殊処理　武器を外せない*/
	///mod151101 八雲紫の日傘とベースアイテムを共用したら色々不具合でたので継ぎ接ぎ。今後は横着せずちゃんと分けよう。
	if(p_ptr->pclass == CLASS_KOGASA
		&& o_ptr_old->tval == TV_STICK && o_ptr_old->sval == SV_WEAPON_KOGASA && o_ptr_old->weight == 30 && slot != item_new)
	{
		msg_print(_("それを手放すことはできない。というかそれはあなたの本体だ。",
                    "You cannot let go of this, that's your true body."));
		return (FALSE);
	}

	if(p_ptr->pclass == CLASS_IKU && slot == INVEN_OUTER && o_ptr_old->name1 == ART_HEAVENLY_MAIDEN && slot != item_new)
	{
		msg_print(_("大事な羽衣だ。手放さないようにしよう。",
                    "That's a very important hagoromo. Better not remove it."));
		return (FALSE);
	}
	if(p_ptr->pclass == CLASS_YUGI  && o_ptr_old->tval == TV_SHIELD && o_ptr_old->sval == SV_SAKAZUKI && slot != item_new)
	{
		msg_print(_("それを手放すつもりはない。", "You have no intention of putting it away."));
		return (FALSE);
	}
	if(p_ptr->pclass == CLASS_EIKI &&
		!ironman_no_fixed_art &&
		(object_is_melee_weapon(o_ptr_new) && o_ptr_new->name1 != ART_EIKI
		|| o_ptr_old->name1 == ART_EIKI && slot != item_new))
	{
		msg_print(_("あなたは悔悟の棒以外のものを振るうつもりはない。",
                    "You have no intention of wielding anything aside from Rod of Remorse."));
		return (FALSE);
	}

	/*:::人魚、屠自古は靴を履けない　外すことはできるがここに来ずに自動的に外れる予定*/
	if(p_ptr->muta3 & MUT3_FISH_TAIL || p_ptr->prace == RACE_NINGYO  || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_KISUME || (CHECK_MIZUCHI_GHOST)){
		if(slot == INVEN_FEET && o_ptr_new->tval != 0)
		{
			msg_print(_("あなたは靴を履くことができない。", "You cannot wear boots."));
			return (FALSE);
		}
	}
	/*:::赤蛮奇は首飾りをつけられない*/
	if(p_ptr->pclass == CLASS_BANKI ){
		if(slot == INVEN_NECK && o_ptr_new->tval != 0)
		{
			msg_print(_("あなたは首飾りをつけられない。", "You cannot wear amulets."));
			return (FALSE);
		}
	}
	//女苑は指輪を専用コマンドから装備する。
	//また一定価格以下のものを装備できない。←ボツ
	if (p_ptr->pclass == CLASS_JYOON)
	{
		if (slot >= INVEN_RIGHT && slot <= INVEN_LEFT)
		{
			msg_print(_("指輪の装備は職業特技コマンドから行う。",
                        "You have to use your class ability to put on rings."));
			return (FALSE);
		}
		/*
		if (o_ptr_new->k_idx && object_is_cheap_to_jyoon(o_ptr_new))
		{
			msg_print("あなたはこんな安物を装備したくない。");
			return (FALSE);
		}

		if (o_ptr_old->k_idx && object_is_cheap_to_jyoon(o_ptr_old) && slot != item_new)
		{
			msg_print("このような安物は二度と身につけることはないだろう。");
			if (!get_check_strict("本当に今の装備を外しますか？", CHECK_OKAY_CANCEL))
			{
				return FALSE;
			}
		}
		*/

	}

	//v1.1.73
	if (p_ptr->pclass == CLASS_YACHIE)
	{
		if (o_ptr_new->tval == TV_ARMOR || o_ptr_new->tval == TV_DRAG_ARMOR)
		{
			msg_print(_("背中の甲羅が邪魔で鎧を装着できない。",
                        "The shell on your back prevents you from wearing armor."));
			return (FALSE);

		}
	}



	/*:::角が生えてると兜をかぶれない*/
	if(p_ptr->muta2 & (MUT2_BIGHORN) || p_ptr->pclass == CLASS_KASEN){
		if(slot == INVEN_HEAD && (
			o_ptr_new->sval == SV_HEAD_METALCAP
		||  o_ptr_new->sval == SV_HEAD_STARHELM
		||  o_ptr_new->sval == SV_HEAD_STEELHELM
		||  o_ptr_new->sval == SV_HEAD_DRAGONHELM
			))
		{
			if(p_ptr->pclass == CLASS_KASEN)
				msg_print(_("あなたはそれを装備できない。", "You cannot wear that."));
			else
				msg_print(_("角が邪魔で兜をかぶれない。", "You horns prevent you from wearing a helmet."));
			return (FALSE);
		}
	}

	if(p_ptr->pclass == CLASS_HECATIA)
	{
		if(slot == INVEN_HEAD && (
			o_ptr_new->sval == SV_HEAD_METALCAP
		||  o_ptr_new->sval == SV_HEAD_STARHELM
		||  o_ptr_new->sval == SV_HEAD_STEELHELM
		||  o_ptr_new->sval == SV_HEAD_DRAGONHELM ))
		{
			msg_print(_("あなたは兜を装備できない。", "You cannot wear helmets."));
			return (FALSE);
		}
	}


	if(p_ptr->muta3 & MUT3_PEGASUS){
		if(slot == INVEN_HEAD && o_ptr_new->tval != 0)
		{
			msg_print(_("あなたは今頭に何も装備できない。", "You can't wear anything on your head right now."));
			return (FALSE);
		}
	}


	if(p_ptr->prace == RACE_KOBITO || p_ptr->pclass == CLASS_NINJA)
	{
		if(((slot == INVEN_RARM || slot == INVEN_LARM) && o_ptr_new->weight > 60)
			|| (slot == INVEN_BODY && o_ptr_new->weight > 100)
			|| (slot == INVEN_HEAD && o_ptr_new->weight > 50)
			|| (slot == INVEN_HANDS && o_ptr_new->weight > 30)
			|| (slot == INVEN_FEET && o_ptr_new->weight > 30))
		{
			msg_print(_("それを装備すると重くてうまく動けない。",
                        "You won't be able to move with such heavy equipment."));
			return (FALSE);

		}
	}

	if(p_ptr->pclass == CLASS_NINJA)
	{
		if(o_ptr_new->tval == TV_SHIELD)
		{
			msg_print(_("盾は動きの邪魔だ。", "A shield would obstruct your movement."));
			return (FALSE);

		}
	}

	return (TRUE);
}
