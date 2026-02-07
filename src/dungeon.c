/* File: dungeonc */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband game engine */

#include "angband.h"
#include "player/player-log-load.h"

#define TY_CURSE_CHANCE 200
#define CHAINSWORD_NOISE 100

static bool load = TRUE;

//「広域マップではdungeon_turn値が増えにくい処理」をHPMP自動回復処理が行われている間は適用しないようにするための判定値と思われる
static int wild_regen = 20;


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
/*:::簡易鑑定　強い簡易鑑定*/
///mod151121 雛で使うのでexternした

byte value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (object_is_artifact(o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if (object_is_ego(o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	///mod151121追加..しようと思ったがやめた。IDENT_BROKEN扱いされると悪影響が出るかもしれない
	//if(o_ptr->pval < 0 || o_ptr->to_a < 0 || o_ptr->to_d < 0 || o_ptr->to_h < 0)
	//	 return FEEL_BROKEN;

	if ((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
/*:::弱い簡易鑑定*/
static byte value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (object_is_artifact(o_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if (object_is_ego(o_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(int slot, bool heavy)
{
	byte        feel;
	object_type *o_ptr = &inventory[slot];
	char        o_name[MAX_NLEN];

	/* We know about it already, do not tell us again */
	if (o_ptr->ident & (IDENT_SENSE))return;

	if(check_invalidate_inventory(slot)) return;

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

	/* Skip non-feelings */
	if (!feel) return;

	/* Bad luck */
	if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !randint0(13))
	{
		switch (feel)
		{
			case FEEL_TERRIBLE:
			{
				feel = FEEL_SPECIAL;
				break;
			}
			case FEEL_WORTHLESS:
			{
				feel = FEEL_EXCELLENT;
				break;
			}
			case FEEL_CURSED:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_GOOD : FEEL_AVERAGE;
				else
					feel = FEEL_UNCURSED;
				break;
			}
			case FEEL_AVERAGE:
			{
				feel = randint0(2) ? FEEL_CURSED : FEEL_GOOD;
				break;
			}
			case FEEL_GOOD:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_CURSED : FEEL_AVERAGE;
				else
					feel = FEEL_CURSED;
				break;
			}
			case FEEL_EXCELLENT:
			{
				feel = FEEL_WORTHLESS;
				break;
			}
			case FEEL_SPECIAL:
			{
				feel = FEEL_TERRIBLE;
				break;
			}
		}
	}

	/* Stop everything */
	if (disturb_minor) disturb(0, 0);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	//映姫特殊処理
	if(p_ptr->pclass == CLASS_EIKI)
	{
		identify_item(o_ptr);
		object_desc(o_name, o_ptr, 0L);
		msg_format(_("これは%sに違いない。", "You are certain this is %s."),o_name);
	}
	else
	{
		/* Message (equipment) */
		if (slot >= INVEN_RARM)
		{
#ifdef JP
			msg_format("%s%s(%c)は%sという感じがする...",
				describe_use(slot),o_name, index_to_label(slot),game_inscriptions[feel]);
#else
            msg_format("You feel the %s you are %s (%c) is %s...",
				o_name, describe_use(slot), index_to_label(slot),game_inscriptions[feel]);
#endif
		}
		/* Message (inventory) */
		else
		{
			msg_format(_("ザックの中の%s(%c)は%sという感じがする...",
                        "You feel the %s in your inventory (%c) is %s..."),
				o_name, index_to_label(slot),game_inscriptions[feel]);

		}
		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);
		/* Set the "inscription" */
		o_ptr->feeling = feel;
	}


	/* Auto-inscription/destroy */
	autopick_alter_item(slot, destroy_feeling);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}



/*
 * Sense the inventory
 *
 *   Class 0 = Warrior --> fast and heavy
 *   Class 1 = Mage    --> slow and light
 *   Class 2 = Priest  --> fast but light
 *   Class 3 = Rogue   --> okay and heavy
 *   Class 4 = Ranger  --> slow but heavy  (changed!)
 *   Class 5 = Paladin --> slow but heavy
 */
///class item TVAL 所持アイテムの簡易鑑定
static void sense_inventory1(void)
{
	int         i;
	int         plev = p_ptr->lev;
	bool        heavy = FALSE;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	///mod140202 簡易鑑定能力をクラス固定の変数参照にした
	switch (cp_ptr->sense_arms)
	{
		case 1:
			if (0 != randint0(240000L / (plev + 5))) return;
			break;
		case 2:
			if (0 != randint0(80000L / (plev * plev + 40))) return;
			break;
		case 3:
			if (0 != randint0(70000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		case 4:
			if (0 != randint0(20000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		case 5:
			if (0 != randint0(8000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		default:
			msg_print(_("ERROR:player_class.sense_armsの設定値がおかしい",
                        "ERROR: Weird setting in player_class.sense_arms"));
			return;

	}

#if 0
	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_SMITH:
		{
			/* Good sensing */
			if (0 != randint0(6000L / (plev * plev + 50))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		{
			/* Good (light) sensing */
			if (0 != randint0(10000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_ROGUE:
		case CLASS_NINJA:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_PALADIN:
		case CLASS_SNIPER:
		{
			/* Bad sensing */
			if (0 != randint0(77777L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		{
			/* Bad sensing */
			if (0 != randint0(75000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_IMITATOR:
		case CLASS_BLUE_MAGE:
		case CLASS_MIRROR_MASTER:
		{
			/* Bad sensing */
			if (0 != randint0(55000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
			/* Bad sensing */
			if (0 != randint0(80000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_BEASTMASTER:
		{
			/* Bad sensing */
			if (0 != randint0(65000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}
		case CLASS_BERSERKER:
		{
			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}
	}
#endif
	///del131221  virtue
	//if (compare_virtue(V_KNOWLEDGE, 100, VIRTUE_LARGE)) heavy = TRUE;

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			///mod131223 簡易鑑定可能な武器防具
			case TV_BULLET:
			case TV_ARROW:
			case TV_BOLT:
			case TV_BOW:
			case TV_CROSSBOW:
			case TV_GUN:
			case TV_KNIFE:
			case TV_SWORD:
			case TV_KATANA:
			case TV_HAMMER:
			case TV_STICK:
			case TV_AXE:
			case TV_SPEAR:
			case TV_POLEARM:
			case TV_OTHERWEAPON:
			case TV_BOOTS:
			case TV_GLOVES:
			case TV_HEAD:
			case TV_SHIELD:
			case TV_CLOAK:
			case TV_CLOTHES:
			case TV_ARMOR:
			case TV_DRAG_ARMOR:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		/* Good luck */
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(i, heavy);
	}
}

/*:::簡易鑑定処理*/
///class item TVAL 簡易鑑定
static void sense_inventory2(void)
{
	int         i;
	int         plev = p_ptr->lev;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;



	///mod140202 簡易鑑定能力をクラス固定の変数参照にした
	switch (cp_ptr->sense_mags)
	{
		case 1:
			return;
		case 2:
			if (0 != randint0(120000L / (plev * plev + 20))) return;
			break;
		case 3:
			if (0 != randint0(80000L / (plev * plev + 40))) return;
			break;
		case 4:
			if (0 != randint0(20000L / (plev * plev + 40))) return;
			break;
		case 5:
			if (0 != randint0(8000L / (plev * plev + 40))) return;
			break;
		default:
			msg_print(_("ERROR:player_class.sense_magsの設定値がおかしい",
                        "ERROR: Weird setting in player_class.sense_mags"));
			return;
	}
#if 0

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		case CLASS_BERSERKER:
		case CLASS_SNIPER:
		{
			return;
		}

		case CLASS_SMITH:
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_IMITATOR:
		case CLASS_BEASTMASTER:
		case CLASS_NINJA:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		case CLASS_MONK:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_ROGUE:
		case CLASS_FORCETRAINER:
		case CLASS_MINDCRAFTER:
		{
			/* Good sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		case CLASS_MIRROR_MASTER:
		case CLASS_BLUE_MAGE:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Done */
			break;
		}
	}
#endif
	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		///mod131223　簡易鑑定可能な魔法のアイテムと装備
		switch (o_ptr->tval)
		{

			case TV_MAGICITEM:
			case TV_MAGICWEAPON:
			case TV_RIBBON:
			case TV_RING:
			case TV_AMULET:
			case TV_LITE:
			case TV_WHISTLE:
			case TV_FIGURINE:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		sense_inventory_aux(i, TRUE);
	}
}



/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
#ifdef JP
	if (!EXTRA_MODE && get_check("他の階にテレポートしますか？"))
#else
	if (!EXTRA_MODE && get_check("Teleport level? "))
#endif

	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if (ironman_downward)
			min_level = dun_level;

		/* Maximum level */
		if (dungeon_type == DUNGEON_ANGBAND)
		{
			if (dun_level > 100)
				max_level = MAX_DEPTH - 1;
			else if (dun_level == 100)
				max_level = 100;
		}
		else
		{
			max_level = d_info[dungeon_type].maxdepth;
			min_level = d_info[dungeon_type].mindepth;
		}

		/* Prompt */
#ifdef JP
		sprintf(ppp, "テレポート先:(%d-%d)", min_level, max_level);
#else
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);
#endif


		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}
#ifdef JP
	//else if (get_check("通常テレポート？"))
	else if (get_check("このフロアの望む場所に転移しますか？"))
#else
	else if (get_check("Teleport to desired location on this floor? "))
#endif
	{
		dimension_door(D_DOOR_PATTERN);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (command_arg < min_level) command_arg = min_level;

	/* Paranoia */
	if (command_arg > max_level) command_arg = max_level;

	/* Accept request */
#ifdef JP
	msg_format("%d 階にテレポートしました。", command_arg);
#else
	msg_format("You teleport to dungeon level %d.", command_arg);
#endif


	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_PAT_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	energy_use = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	p_ptr->leaving = TRUE;
}


static void wreck_the_pattern(void)
{
	int to_ruin = 0, r_y, r_x;
	int pattern_type = f_info[cave[py][px].feat].subtype;

	if (pattern_type == PATTERN_TILE_WRECKED)
	{
		/* Ruined already */
		return;
	}

#ifdef JP
	msg_print("パターンを血で汚してしまった！");
	msg_print("何か恐ろしい事が起こった！");
#else
	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");
#endif

	if (!IS_INVULN())
#ifdef JP
		take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "パターン損壊", -1);
#else
		take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "corrupting the Pattern", -1);
#endif

	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, py, px, 4, 0);

		if (pattern_tile(r_y, r_x) &&
		    (f_info[cave[r_y][r_x].feat].subtype != PATTERN_TILE_WRECKED))
		{
			cave_set_feat(r_y, r_x, feat_pattern_corrupted);
		}
	}

	cave_set_feat(py, px, feat_pattern_corrupted);
}


/* Returns TRUE if we are on the Pattern... */
static bool pattern_effect(void)
{
	int pattern_type;

	if (!pattern_tile(py, px)) return FALSE;
/*
	if ((prace_is_(RACE_AMBERITE)) &&
	    (p_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern();
	}
*/
	pattern_type = f_info[cave[py][px].feat].subtype;

	switch (pattern_type)
	{
	case PATTERN_TILE_END:
		(void)set_poisoned(0);
		(void)set_image(0);
		(void)set_stun(0);
		(void)set_cut(0);
		(void)set_blind(0);
		(void)set_afraid(0);
		(void)do_res_stat(A_STR);
		(void)do_res_stat(A_INT);
		(void)do_res_stat(A_WIS);
		(void)do_res_stat(A_DEX);
		(void)do_res_stat(A_CON);
		(void)do_res_stat(A_CHR);
		(void)restore_level();
		(void)hp_player(1000);

		cave_set_feat(py, px, feat_pattern_old);

#ifdef JP
		msg_print("「パターン」のこの部分は他の部分より強力でないようだ。");
#else
		msg_print("This section of the Pattern looks less powerful.");
#endif

		/*
		 * We could make the healing effect of the
		 * Pattern center one-time only to avoid various kinds
		 * of abuse, like luring the win monster into fighting you
		 * in the middle of the pattern...
		 */
		break;

	case PATTERN_TILE_OLD:
		/* No effect */
		break;

	case PATTERN_TILE_TELEPORT:
		pattern_teleport();
		break;

	case PATTERN_TILE_WRECKED:
		if (!IS_INVULN())
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, 200, "壊れた「パターン」を歩いたダメージ", -1);
#else
			take_hit(DAMAGE_NOESCAPE, 200, "walking the corrupted Pattern", -1);
#endif
		break;

	default:
		//if (prace_is_(RACE_AMBERITE) && !one_in_(2))
		//	return TRUE;
		//else
			if (!IS_INVULN())
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, damroll(1, 3), "「パターン」を歩いたダメージ", -1);
#else
			take_hit(DAMAGE_NOESCAPE, damroll(1, 3), "walking the Pattern", -1);
#endif
		break;
	}

	return TRUE;
}





/*
 * Regenerate hit points				-RAK-
 */
/*:::＠のHP自動回復　詳細未読*/
static void regenhp(int percent)
{
	s32b new_chp;
	u32b new_chp_frac;
	s32b old_chp;

	if (p_ptr->special_defense & KATA_KOUKIJIN) return;
	if (p_ptr->action == ACTION_HAYAGAKE) return;

	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	///mod140720 芳香と生命ハイは*急回復*する
	if(p_ptr->pclass == CLASS_YOSHIKA ||  (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39))
	{
		percent = percent * p_ptr->lev / 10;
	}
	//尤魔もレベルによって*急回復*
	else if (p_ptr->pclass == CLASS_YUMA)
	{
		percent = percent * (10 + p_ptr->lev) / 6;
	}
	//阿梨夜も早い
	else if (p_ptr->pclass == CLASS_ARIYA)
	{
		percent = percent * 3;
	}

	else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])//諏訪子冬眠
	{
		percent *= 10;
	}
	else if(p_ptr->pclass == CLASS_SUNNY && is_daytime()) percent *= 2;
	else if(p_ptr->pclass == CLASS_LUNAR && !is_daytime()) percent *= 2;
	else if(p_ptr->prace == RACE_HOURAI) percent = percent * 5 / 2;
	else //肉体強化地蔵所有時 +50%～+150%
	{
		int jizo_card_num = count_ability_card(ABL_CARD_JIZO);

		if (jizo_card_num)
		{
			int mult = calc_ability_card_prob(ABL_CARD_JIZO, jizo_card_num);

			percent += percent * mult / 100;

		}
	}


	/*
	 * Extract the new hitpoints
	 *
	 * 'percent' is the Regen factor in unit (1/2^16)
	 */
	new_chp = 0;
	new_chp_frac = (p_ptr->mhp * percent + PY_REGEN_HPBASE);



	/* Convert the unit (1/2^16) to (1/2^32) */
	s64b_LSHIFT(new_chp, new_chp_frac, 16);

	/* Regenerating */
	s64b_add(&(p_ptr->chp), &(p_ptr->chp_frac), new_chp, new_chp_frac);


	/* Fully healed */
	if (0 < s64b_cmp(p_ptr->chp, p_ptr->chp_frac, p_ptr->mhp, 0))
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		wild_regen = 20;
	}
}


/*
 * Regenerate mana points
 */
/*:::MP回復、もしくは超過コストによる減少 upkeep_factor=ペット維持などコスト regen_amount=回復割合基本値　詳細未読*/
static void regenmana(int upkeep_factor, int regen_amount)
{
	s32b old_csp = p_ptr->csp;
	s32b regen_rate = regen_amount * 100 - upkeep_factor * PY_REGEN_NORMAL;

	//v1.1.84 麻痺しているときは回復しない
	if (p_ptr->paralyzed) return;

	/*
	 * Excess mana will decay 32 times faster than normal
	 * regeneration rate.
	 */
	if (p_ptr->csp > p_ptr->msp)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b decay = 0;
		u32b decay_frac = (p_ptr->msp * 32 * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		///mod150419 格闘家の超過MP減少量はもっと早くする
		if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
		{
			decay_frac *= 3;
			p_ptr->update |= (PU_BONUS);

		}
		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(decay, decay_frac, 16);

		/* Decay */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), decay, decay_frac);

		/* Stop decaying */
		if (p_ptr->csp < p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}

	/* Regenerating mana (unless the player has excess mana) */
	else if (regen_rate > 0)
	{
		/* (percent/100) is the Regen factor in unit (1/2^16) */
		s32b new_mana = 0;
		u32b new_mana_frac = (p_ptr->msp * regen_rate / 100 + PY_REGEN_MNBASE);

		//パチュリーはMP回復速度が早い 最大2.5倍
		if(p_ptr->pclass == CLASS_PATCHOULI)
		{
			new_mana_frac += new_mana_frac * p_ptr->lev / 20;
		}
		else if (p_ptr->pclass == CLASS_ARIYA)//阿梨夜はもっと早い 3倍
		{
			new_mana_frac *= 3;
		}
		else if(p_ptr->prace == RACE_LUNARIAN)//月の民もMP回復速度が早い 2倍
		{
			new_mana_frac *= 2;
		}
		else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0]) //諏訪子冬眠中は10倍
		{
			new_mana_frac *= 10;
		}
		else//v1.1.68 アビリティカード「転ばぬ先のスペル」所有時 1.25～2倍
		{
			int p_card_num = count_ability_card(ABL_CARD_PATHE_SPELL);
			if (p_card_num)
			{
				int percen = calc_ability_card_prob(ABL_CARD_PATHE_SPELL, p_card_num);
				new_mana_frac += new_mana_frac * percen / 100;

			}
		}

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(new_mana, new_mana_frac, 16);

		/* Regenerate */
		s64b_add(&(p_ptr->csp), &(p_ptr->csp_frac), new_mana, new_mana_frac);

		/* Must set frac to zero even if equal */
		if (p_ptr->csp >= p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}


	/* Reduce mana (even when the player has excess mana) */
	if (regen_rate < 0)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		//s32b reduce_mana = 0;
		//u32b reduce_mana_frac = (p_ptr->msp * (-1) * regen_rate / 100 + PY_REGEN_MNBASE);
		/* Convert the unit (1/2^16) to (1/2^32) */
		//s64b_LSHIFT(reduce_mana, reduce_mana_frac, 16);

		//ペット超過コストを乱暴に変更
		///mod141101 超過コスト緩和
		//s32b reduce_mana = upkeep_factor / 10;
		s32b reduce_mana = 0 - regen_rate / PY_REGEN_NORMAL / 3;
		u32b reduce_mana_frac = 0;
		if(reduce_mana < 0) reduce_mana = 0;



		/* Reduce mana */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), reduce_mana, reduce_mana_frac);

		/* Check overflow */
		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}


	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);

		wild_regen = 20;
	}
}



/*
 * Regenerate magic
 * regen_amount: PY_REGEN_NORMAL * 2 (if resting) * 2 (if having regenarate)
 */
static void regenmagic(int regen_amount)
{
	s32b new_mana;
	int i;
	int dev = 30;
	int mult = (dev + adj_mag_mana[p_ptr->stat_ind[A_INT]]); /* x1 to x2 speed bonus for recharging */

	if(p_ptr->pclass != CLASS_SEIJA) //正邪のときは杖と魔法棒が回復しない
	{
		for (i = 0; i < EATER_EXT*2; i++)
		{
			if (!p_ptr->magic_num2[i]) continue;
			if (p_ptr->magic_num1[i] == ((long)p_ptr->magic_num2[i] << 16)) continue;

			/* Increase remaining charge number like float value */
			new_mana = (regen_amount * mult * ((long)p_ptr->magic_num2[i] + 13)) / (dev * 8);
			p_ptr->magic_num1[i] += new_mana;

			/* Check maximum charge */
			if (p_ptr->magic_num1[i] > (p_ptr->magic_num2[i] << 16))
			{
				p_ptr->magic_num1[i] = ((long)p_ptr->magic_num2[i] << 16);
			}
			wild_regen = 20;
		}
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if (!p_ptr->magic_num1[i]) continue;
		if (!p_ptr->magic_num2[i]) continue;

		/* Decrease remaining period for charging */
		new_mana = (regen_amount * mult * ((long)p_ptr->magic_num2[i] + 10) * EATER_ROD_CHARGE)
					/ (dev * 16 * PY_REGEN_NORMAL);
		p_ptr->magic_num1[i] -= new_mana;

		/* Check minimum remaining period for charging */
		if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
		wild_regen = 20;
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
/*:::モンスター全ての自然回復 process_world()から呼ばれる*/
///system モンスター自然回復　悪夢モードはもっと回復速度を早くしてもいいかも
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];


		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			//v1.1.41 舞特殊騎乗　回復速度増加
			if (p_ptr->pclass == CLASS_MAI && i == p_ptr->riding)
			{
				int chr = p_ptr->stat_ind[A_CHR] + 3;
				frac += MAX(0, frac * (chr - 5) / 15);
			}

			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
		}
	}
}


/*
 * Regenerate the captured monsters (once per 30 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
/*:::「所有している」モンスターボールの中のモンスターのHPを回復する。家や床のボールの中のモンスターは回復しない。*/
static void regen_captured_monsters(void)
{
	int i, frac;
	bool heal = FALSE;

	/* Regenerate everyone */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		monster_race *r_ptr;
		object_type *o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;
		//v1.1.85 妖魔本以外にも捕獲できる装備を追加。そちらはxtra6にモンスターr_idxを記録する.xtra4と5は同じ
		if (o_ptr->tval == TV_CAPTURE)
		{
			if (!o_ptr->pval) continue;

			r_ptr = &r_info[o_ptr->pval];
		}
		else
		{
			if (activation_index(o_ptr) != ACT_ART_CAPTURE) continue;
			if (!o_ptr->xtra6) continue;
			r_ptr = &r_info[o_ptr->xtra6];
		}


		heal = TRUE;


		/* Allow regeneration (if needed) */
		if (o_ptr->xtra4 < o_ptr->xtra5)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->xtra5 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			o_ptr->xtra4 += frac;

			/* Do not over-regenerate */
			if (o_ptr->xtra4 > o_ptr->xtra5) o_ptr->xtra4 = o_ptr->xtra5;
		}
	}

	if (heal)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}
}

/*:::明かりが消えたり消えそうな時のメッセージ*/
static void notice_lite_change(object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->xtra4 < 100) || (!(o_ptr->xtra4 % 100)))
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (p_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->xtra4 == 0) o_ptr->xtra4++;
	}

	/* The light is now out */
	else if (o_ptr->xtra4 == 0)
	{
		disturb(0, 1);
#ifdef JP
msg_print("明かりが消えてしまった！");
#else
		msg_print("Your light has gone out!");
#endif

		/* Recalculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Some ego light lose its effects without fuel */
		p_ptr->update |= (PU_BONUS);
	}

	/* The light is getting dim */
	else if (o_ptr->name2 == EGO_LITE_LONG)
	{
		if ((o_ptr->xtra4 < 50) && (!(o_ptr->xtra4 % 5))
		    && (turn % (TURNS_PER_TICK*2)))
		{
			if (disturb_minor) disturb(0, 1);
#ifdef JP
msg_print("明かりが微かになってきている。");
#else
			msg_print("Your light is growing faint.");
#endif

		}
	}

	/* The light is getting dim */
	else if ((o_ptr->xtra4 < 100) && (!(o_ptr->xtra4 % 10)))
	{
		if (disturb_minor) disturb(0, 1);
#ifdef JP
msg_print("明かりが微かになってきている。");
#else
		msg_print("Your light is growing faint.");
#endif

	}
}

/*:::クエストの終了状態をチェック？詳細未読*/
///quest
void leave_quest_check(void)
{
	/* Save quest number for dungeon pref file ($LEAVING_QUEST) */
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
	    ((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
	    (quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)p_ptr->lev;

		/* Additional settings */
		switch (quest[leaving_quest].type)
		{
		case QUEST_TYPE_TOWER:
			quest[QUEST_TOWER1].status = QUEST_STATUS_FAILED;
			quest[QUEST_TOWER1].complev = (byte)p_ptr->lev;
			break;
		case QUEST_TYPE_FIND_ARTIFACT:
			a_info[quest[leaving_quest].k_idx].gen_flags &= ~(TRG_QUESTITEM);
			break;
		case QUEST_TYPE_RANDOM:
			r_info[quest[leaving_quest].r_idx].flags1 &= ~(RF1_QUESTOR);

			/* Floor of random quest will be blocked */
			prepare_change_floor_mode(CFM_NO_RETURN);
			break;
		}

		/* Record finishing a quest */
		if (quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			if (record_rand_quest) do_cmd_write_nikki(NIKKI_RAND_QUEST_F, leaving_quest, NULL);
		}
		else
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_F, leaving_quest, NULL);
		}
	}
	//特殊処理　魔理沙の月都万象展とマリコンクエは階段を登ると達成→終了へ
	else if(leaving_quest == QUEST_MOON_VAULT && quest[leaving_quest].status == QUEST_STATUS_COMPLETED)
	{
		quest[QUEST_MOON_VAULT].status = QUEST_STATUS_FINISHED;
	}
	else if(leaving_quest == QUEST_MARICON && quest[leaving_quest].status == QUEST_STATUS_COMPLETED)
	{
		quest[QUEST_MARICON].status = QUEST_STATUS_FINISHED;
	}
	else if(leaving_quest)
	{
		quest[leaving_quest].flags |= QUEST_FLAG_RETRY;
	}


}

/*:::塔から出るときの処理*/
void leave_tower_check(void)
{
	leaving_quest = p_ptr->inside_quest;
	/* Check for Tower Quest */
	if (leaving_quest &&
		(quest[leaving_quest].type == QUEST_TYPE_TOWER) &&
		(quest[QUEST_TOWER1].status != QUEST_STATUS_COMPLETED))
	{
		if(quest[leaving_quest].type == QUEST_TYPE_TOWER)
		{
			quest[QUEST_TOWER1].status = QUEST_STATUS_FAILED;
			quest[QUEST_TOWER1].complev = (byte)p_ptr->lev;
		}
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 *
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
/*:::サイコメトリー*/
bool psychometry(void)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
q = "どのアイテムを調べますか？";
s = "調べるアイテムがありません。";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr))
	{
#ifdef JP
msg_print("何も新しいことは判らなかった。");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if (!feel)
	{
#ifdef JP
msg_format("%sからは特に変わった事は感じとれなかった。", o_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", o_name);
#endif

		return TRUE;
	}

#ifdef JP
msg_format("%sは%sという感じがする...",
    o_name,  game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...",
			   o_name, ((o_ptr->number == 1) ? "is" : "are"),
			   game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	o_ptr->feeling = feel;

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	///item TVAL サイコメトリーのときの自動破壊可能フラグ？
	///mod131223 tval
	switch (o_ptr->tval)
	{
	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_CROSSBOW:
	case TV_GUN:
	case TV_MAGICITEM:
	case TV_MAGICWEAPON:
	case TV_OTHERWEAPON:
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_RING:
	case TV_RIBBON:
	case TV_AMULET:
	case TV_LITE:
	case TV_FIGURINE:
	case TV_WHISTLE:
		okay = TRUE;
		break;
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(item, (bool)(okay && destroy_feeling));

	/* Something happened */
	return (TRUE);
}


/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
/*:::銘!!による再充填通知*/
static void recharged_notice(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	cptr s;

	if (o_ptr->name1 == ART_CLOWNPIECE)
	{
		msg_print(_("松明に再び火が灯った。", "The torch is lit again."));
		p_ptr->update |= PU_TORCH;
		return;
	}

	/* No inscription */
	if (!o_ptr->inscription) return;

	/* Find a '!' */
	s = my_strchr(quark_str(o_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			if(o_ptr->tval == TV_ABILITY_CARD)
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX));
			else
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/* Notify the player */
			if(o_ptr->name1 == ART_YAKUZAGUN_1 || o_ptr->name1 == ART_YAKUZAGUN_2)
				msg_format(_("%s「装填完了ドスエ」", "%s is reloaded."), o_name);
			else if(o_ptr->tval == TV_GUN && o_ptr->sval != SV_FIRE_GUN_LASER && o_ptr->sval != SV_FIRE_GUN_LUNATIC)
				msg_format(_("%sは再装填された。", "%s is reloaded."), o_name);
			else
				msg_format(_("%sは再充填された。", "%s has finished charging."), o_name);

			disturb(0, 0);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = my_strchr(s + 1, '!');
	}
}

/*:::process_upkeep_with_speed()から呼ばれる。歌の効果発生*/
///mod140817 歌関係に吟遊詩人以外の職も便乗
static void check_music(void)
{
	const magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	//if (p_ptr->pclass != CLASS_BARD) return;
	if (!(CHECK_MUSIC_CLASS)) return;

	if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1]) return;
	if (p_ptr->anti_magic)
	{
		stop_singing();
		stop_tsukumo_music();
		return;
	}

	spell = p_ptr->magic_num2[0];
	if(p_ptr->pclass == CLASS_BARD)
	{
		msg_print(_("ERROR:吟遊詩人の歌は無効化中",
                    "ERROR: Bard song is negated"));
		return;
		//s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];
		//need_mana = mod_need_mana(s_ptr->smana, spell, REALM_MUSIC);
	}
	else if(p_ptr->pclass == CLASS_MYSTIA)
	{
		switch(spell)
		{
		case MUSIC_NEW_MYSTIA_YAZYAKU: //ミスティア　夜雀の歌
			need_mana = 4; break;
		case MUSIC_NEW_MYSTIA_MYSTERIOUS: //ミスティア　ミステリアスソング
			need_mana = 24; break;
		case MUSIC_NEW_MYSTIA_CHORUSMASTER: //ミスティア　真夜中のコーラスマスター
			need_mana = 36; break;
		case MUSIC_NEW_MYSTIA_HUMAN_CAGE: //v1.1.47 ミスティア　ヒューマンケージ
			need_mana = 30; break;

		default:
			msg_format(_("ERROR:この職業の音楽idx(%d)の継続コストがcheck_music()に登録されていない",
                        "ERROR: Maintenance cost for music idx (%d) of this class not set in check_music()"),spell);
			return;

		}
	}
	else if(p_ptr->pclass == CLASS_KYOUKO)
	{
		switch(spell)
		{
		case MUSIC_NEW_KYOUKO_NENBUTSU: //響子　無限念仏
			need_mana = 24; break;
		case MUSIC_NEW_KYOUKO_SCREAM: //響子　プライマルスクリーム
			need_mana = 36; break;

		default:
			msg_format(_("ERROR:この職業の音楽idx(%d)の継続コストがcheck_music()に登録されていない",
                        "ERROR: Maintenance cost for music idx (%d) of this class not set in check_music()"),spell);
			return;

		}
	}
	//三姉妹は1つづつなので横着
	else if(p_ptr->pclass == CLASS_LUNASA) need_mana = 24;
	else if(p_ptr->pclass == CLASS_MERLIN) need_mana = 36;
	else if(p_ptr->pclass == CLASS_LYRICA) need_mana = 8;
	else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI)
	{
		switch(spell)
		{
		case MUSIC_NEW_TSUKUMO_HEIKE: //怨霊「平家の大怨霊」
			need_mana = 9; break;
		case MUSIC_NEW_TSUKUMO_STORM: //嵐のアンサンブル
			need_mana = 24; break;
		case MUSIC_NEW_TSUKUMO_DOUBLESCORE: //ダブルスコア
			need_mana = 48; break;
		case MUSIC_NEW_TSUKUMO_SCOREWEB: //スコアウェブ
			need_mana = 36; break;
		case MUSIC_NEW_TSUKUMO_NORIGOTO: //天の詔琴
			need_mana = 12; break;
		case MUSIC_NEW_TSUKUMO_JINKIN: //人琴ともに亡ぶ
			need_mana = 24; break;
		case MUSIC_NEW_TSUKUMO_ECHO: //エコーチェンバー
			need_mana = 32; break;
		case MUSIC_NEW_TSUKUMO_ECHO2: //平安の残響
			need_mana = 8; break;

		default:
			msg_format(_("ERROR:この職業の音楽idx(%d)の継続コストがcheck_music()に登録されていない",
                        "ERROR: Maintenance cost for music idx (%d) of this class not set in check_music()"),spell);
			return;
		}
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		switch(spell)
		{
		case MUSIC_NEW_RAIKO_AYANOTUDUMI: //アヤノツヅミ
			need_mana = 6; break;
		case MUSIC_NEW_RAIKO_DENDEN: //怒りのデンデン太鼓
			need_mana = 32; break;
		case MUSIC_NEW_RAIKO_PRISTINE: //プリスティンビート
			need_mana = 48; break;

		default:
			msg_format(_("ERROR:この職業の音楽idx(%d)の継続コストがcheck_music()に登録されていない",
                        "ERROR: Maintenance cost for music idx (%d) of this class not set in check_music()"),spell);
			return;

		}
	}
	else if (p_ptr->pclass == CLASS_WAKASAGI)
	{
		need_mana = 10;
	}
	else if (p_ptr->pclass == CLASS_SANNYO)
	{
		switch (spell)
		{
		case MUSIC_NEW_SANNYO_SMOKE:
			need_mana = 0; break;
		case MUSIC_NEW_SANNYO_FEAR:
			need_mana = 6; break;
		case MUSIC_NEW_SANNYO_CONF:
			need_mana = 12; break;
		case MUSIC_NEW_SANNYO_STUN:
			need_mana = 20; break;
		case MUSIC_NEW_SANNYO_SLOW:
			need_mana = 30; break;
		case MUSIC_NEW_SANNYO_MINDBLAST:
			need_mana = 48; break;
		case MUSIC_NEW_SANNYO_BERSERK:
			need_mana = 80; break;

		default:
			msg_format(_("ERROR:この職業の音楽idx(%d)の継続コストがcheck_music()に登録されていない",
                        "ERROR: Maintenance cost for music idx (%d) of this class not set in check_music()"), spell);
			return;

		}


	}
	else
	{
		msg_print(_("ERROR:この職業の歌継続処理がcheck_music()に登録されていない",
                    "ERROR: No logic for music maintenace for this class in check_music()"));
		return;
	}

	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();

		return;
	}
	else
	{
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), need_mana, need_mana_frac);

		p_ptr->redraw |= PR_MANA;
		if (p_ptr->magic_num1[1])
		{
			p_ptr->magic_num1[0] = p_ptr->magic_num1[1];
			p_ptr->magic_num1[1] = 0;

			if(p_ptr->pclass == CLASS_SANNYO)
				msg_print(_("葉を詰め直した。", "You repack your leaves."));
			else
				msg_print(_("歌を再開した。", "You resume singing."));

			p_ptr->action = ACTION_SING;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Redraw map and status bar */
			p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}
	if(p_ptr->pclass == CLASS_BARD)
	{
		/*
		if (p_ptr->spell_exp[spell] < SPELL_EXP_BEGINNER) p_ptr->spell_exp[spell] += 5;
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_SKILLED)	{ if (one_in_(2) && (dun_level > 4) && ((dun_level + 10) > p_ptr->lev)) p_ptr->spell_exp[spell] += 1; }
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_EXPERT)	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_MASTER)	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }
		*/
	}
	/* Do any effects of continual song */
	//do_spell(REALM_MUSIC, spell, SPELL_CONT);
	do_spell(TV_BOOK_MUSIC, spell, SPELL_CONT);

}


/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(u32b flag)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!(p_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		if(check_invalidate_inventory(i)) continue;

		if (o_ptr->curse_flags & flag)
		{
			choices[number] = i;
			number++;
		}
		else if ((flag == TRC_ADD_L_CURSE) || (flag == TRC_ADD_H_CURSE))
		{
			u32b cf = (flag == TRC_ADD_L_CURSE) ? TR_ADD_L_CURSE : TR_ADD_H_CURSE;
			u32b flgs[TR_FLAG_SIZE];
			object_flags(o_ptr, flgs);
			if (have_flag(flgs, cf))
			{
				choices[number] = i;
				number++;
			}
		}
	}

	/* Choice one of them */
	return (&inventory[choices[randint0(number)]]);
}


//v1.1.86
//新ダンジョン「虹龍洞」で酸素が必要なプレイヤーの判定
static bool check_player_need_oxygen()
{
	bool need_oxygen;

	//水耐性があれば呼吸できることにする
	if (p_ptr->resist_water) return FALSE;

	//種族で基本判定
	need_oxygen = (RACE_NEED_OXYGEN);

	//クラスで例外判定
	switch (p_ptr->pclass)
	{
		//尸解仙だから呼吸してなさそう
	case CLASS_FUTO:
	case CLASS_MIKO:
		//なぜか酸素いらなそう
	case CLASS_KASEN:
		need_oxygen = FALSE;
		break;

	}

	//一時変身によって変化。呼吸しない側の種族が呼吸が必要になったりはしない
	switch (p_ptr->mimic_form)
	{
		//デーモンやアンデッドや無生物などに変身したら呼吸不要
	case MIMIC_DEMON:
	case MIMIC_DEMON_LORD:
	case MIMIC_VAMPIRE:
	case MIMIC_SLIME:
	case MIMIC_MIST:
		need_oxygen = FALSE;
		break;
	case MIMIC_METAMORPHOSE_MONST:
	case MIMIC_METAMORPHOSE_GIGANTIC:
	case MIMIC_METAMORPHOSE_NORMAL:
		if (r_info[MON_EXTRA_FIELD].flags3 & (RF3_KWAI | RF3_DEITY | RF3_DEMON | RF3_NONLIVING | RF3_UNDEAD | RF3_HANIWA | RF3_FAIRY)) need_oxygen = FALSE;
		if (r_info[MON_EXTRA_FIELD].flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) need_oxygen = TRUE;
		//プレイヤーが持ってる水耐性と違ってモンスターが持ってる水耐性って無酸素エリアで呼吸できる感じがしないがまあ仕様を揃えておく
		if (r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_WATE) need_oxygen = FALSE;
		break;

	}

	return need_oxygen;

}

//v1.1.85
//溶岩などの地形ダメージを適用する。　毒沼とか色々追加したので別関数に分ける。とりあえず酸電火冷毒のみ移した。
//feat_flag:FF_LAVAなど属性を示す地形フラグ
//ダメージなしのときFALSEを返す
static bool terrain_dam(int feat_flag)
{
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	int dam=0;
	int dam_x100=0;
	cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
	cptr name_add = "";

	if (p_ptr->is_dead) return FALSE;

	//地形ダメージ一切無効の面々
	if (IS_INVULN()) return FALSE;
	if (p_ptr->pclass == CLASS_SUWAKO) return FALSE;
	if (p_ptr->pclass == CLASS_YUMA) return FALSE;
	if (p_ptr->pclass == CLASS_CHIMI) return FALSE; //v2.1.2
	if (p_ptr->pclass == CLASS_FUTO && p_ptr->lev > 29) return FALSE;
	if (SAKUYA_WORLD) return FALSE;
	if (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_NATURE) return FALSE;


 	switch (feat_flag)
	{
	case FF_LAVA:
		if (p_ptr->immune_fire) return FALSE;

		if (CHECK_FAIRY_TYPE == 5) return FALSE;//溶岩で元気になる妖精

		if (have_flag(f_ptr->flags, FF_DEEP))//深い溶岩　浮遊は1/5ダメ
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = _("の熱", "heat of ");
				dam_x100 /= 5;
			}
		}
		else  //浅い溶岩　浮遊はダメージなし
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		dam_x100 = mod_fire_dam(dam_x100);

		break;
	case FF_ACID_PUDDLE:
		if (p_ptr->immune_acid) return FALSE;

		if (have_flag(f_ptr->flags, FF_DEEP))//深い酸　浮遊は1/5ダメ
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = _("の飛沫", "splashes of ");
				dam_x100 /= 5;
			}
		}
		else  //浅い酸　浮遊はダメージなし
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		dam_x100 = mod_acid_dam(dam_x100);

		break;

	case FF_COLD_PUDDLE:
		if (p_ptr->immune_cold) return FALSE;

		if (p_ptr->pclass == CLASS_LETTY) return FALSE;
		if (p_ptr->pclass == CLASS_CIRNO) return FALSE;

		//ダメージは溶岩や酸の半分だが浮遊で軽減されない
		if (have_flag(f_ptr->flags, FF_DEEP))
			dam_x100 = 3000 + randint0(2000);
		else
			dam_x100 = 1500 + randint0(1000);

		dam_x100 = mod_cold_dam(dam_x100);

		break;
	case FF_ELEC_PUDDLE:
		if (p_ptr->immune_elec) return FALSE;
		if (p_ptr->pclass == CLASS_IKU) return FALSE;
		if (p_ptr->pclass == CLASS_RAIKO) return FALSE;

		//ダメージは溶岩や酸の半分だが浮遊で軽減されない
		if (have_flag(f_ptr->flags, FF_DEEP))
			dam_x100 = 3000 + randint0(2000);
		else
			dam_x100 = 1500 + randint0(1000);

		dam_x100 = mod_elec_dam(dam_x100);

		break;

	case FF_POISON_PUDDLE:
		if (p_ptr->pclass == CLASS_EIRIN) return FALSE;

		if (p_ptr->pclass == CLASS_MOMOYO) return FALSE;

		if (p_ptr->pclass == CLASS_MEDICINE)
		{
			set_food(MIN(PY_FOOD_MAX - 1, p_ptr->food + 100));
			return FALSE;
		}

		if (have_flag(f_ptr->flags, FF_DEEP))//深い毒沼　浮遊は1/5ダメ
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = _("の煙", "smoke from ");
				dam_x100 /= 5;
			}
		}
		else  //浅い毒　浮遊はダメージなし
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		if (p_ptr->resist_pois) dam_x100 = (dam_x100 + 2) / 3;
		if(IS_OPPOSE_POIS()) dam_x100 = (dam_x100 + 2) / 3;

		//喘息悪化,毒
		if ((p_ptr->muta2 & MUT2_ASTHMA))set_asthma(p_ptr->asthma + dam_x100 / 100);
		if(!p_ptr->resist_pois && !IS_OPPOSE_POIS()) set_poisoned(p_ptr->poisoned + randint1(dam_x100/100));

		break;

	default:
		msg_format(_("ERROR:定義されていない地形(%d)でterrain_dam()が呼ばれた",
                    "ERROR: terrain_dam() called for undefined terrain (%d)"),feat_flag);
		return FALSE;
	}

	//100倍ダメージを通常に再計算　100倍にしてあったのは少数計算のためらしいが、比較演算子の戻り値を数値として扱うとかなんか怖いので書き直す
	//dam = dam_x100 / 100 + (randint0(100) < (dam_x100 % 100));
	dam = dam_x100 / 100;
	if (randint0(100) < (dam_x100 % 100)) dam++;

	if (!dam) return FALSE;

	//ダメージ処理とメッセージ　
#ifdef JP
	msg_format("%s%sでダメージを受けた！", name,name_add);
#else
    msg_format("You are damaged by %s%s!", name_add, name);
#endif
	take_hit(DAMAGE_NOESCAPE, dam, name, -1);


	if (p_ptr->wild_mode)
	{
		/*:::-Hack- 全体マップ移動中はenergy_useが数倍になってて溶岩地形に入った瞬間何度も食らって死にかねず不自然なので直しておく*/
		energy_use = MIN(energy_use, 100);
		change_wild_mode();
	}

	return TRUE;

}

/*
 * Handle timed damage and regeneration every 10 game turns
 */
/*:::＠へのターンダメージと再生*/
static void process_world_aux_hp_and_sp(void)
{
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;

	/* Default regeneration */
	int regen_amount = PY_REGEN_NORMAL;

	//v1.1.59 サニーミルクが日光浴をして日光を体に蓄える
	if (!p_ptr->inside_arena && !p_ptr->inside_battle && (p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_3_FAIRIES))
	{
		if (is_daytime() && !dun_level)
			sunny_charge_sunlight(1);
		else
			sunny_charge_sunlight(-1);

	}

	/*** Damage over Time ***/


	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > HINA_YAKU_LIMIT3 && one_in_(6) && !IS_INVULN())
	{
		take_hit(DAMAGE_NOESCAPE, 1, _("厄の負担", "burden of misfortune"), -1);
	}


	/* Take damage from poison */
	if (p_ptr->poisoned && !IS_INVULN())
	{
		/* Take damage */
#ifdef JP
		take_hit(DAMAGE_NOESCAPE, 1, "毒", -1);
#else
		take_hit(DAMAGE_NOESCAPE, 1, "poison", -1);
#endif

	}
	///sys 傷のダメージでメッセージ出すにはここ編集
	/* Take damage from cuts */
	if (p_ptr->cut && !IS_INVULN())
	{
		int dam;

		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > CUT_7)
		{
			dam = 200;
		}

		else if (p_ptr->cut > CUT_6)
		{
			dam = 80;
		}

		/* Severe cut */
		else if (p_ptr->cut > CUT_5)
		{
			dam = 32;
		}

		else if (p_ptr->cut > CUT_4)
		{
			dam = 16;
		}

		else if (p_ptr->cut > CUT_3)
		{
			dam = 7;
		}

		else if (p_ptr->cut > CUT_2)
		{
			dam = 3;
		}

		/* Other cuts */
		else
		{
			dam = 1;
		}

		if(p_ptr->pclass == CLASS_HINA)
		{
			hina_gain_yaku(-dam);
		}
		else
		{
			/* Take damage */
			///msg131221
			take_hit(DAMAGE_NOESCAPE, dam, _("怪我の痛み", "pain from wounds"), -1);
		}


	}


	/* (Vampires) Take damage from sunlight */
	///sysdel131221 吸血鬼の陽光ダメージを無しにした
#if 0
	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		if (!dun_level && !p_ptr->resist_lite && !IS_INVULN() && is_daytime())
		{
			if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				/* Take damage */
#ifdef JP
msg_print("日光があなたのアンデッドの肉体を焼き焦がした！");
take_hit(DAMAGE_NOESCAPE, 1, "日光", -1);
#else
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(DAMAGE_NOESCAPE, 1, "sunlight", -1);
#endif

				cave_no_regen = TRUE;
			}
		}

		if (inventory[INVEN_LITE].tval && (inventory[INVEN_LITE].name2 != EGO_LITE_DARKNESS) &&
		    !p_ptr->resist_lite)
		{
			object_type * o_ptr = &inventory[INVEN_LITE];
			char o_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			/* Get an object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
msg_format("%sがあなたのアンデッドの肉体を焼き焦がした！", o_name);
#else
			msg_format("The %s scorches your undead flesh!", o_name);
#endif


			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

#ifdef JP
sprintf(ouch, "%sを装備したダメージ", o_name);
#else
			sprintf(ouch, "wielding %s", o_name);
#endif

			if (!IS_INVULN()) take_hit(DAMAGE_NOESCAPE, 1, ouch, -1);
		}
	}
#endif

	//v1.1.64 新地形「針のむしろ」　浮遊せずに入るとダメージと負傷
	if (have_flag(f_ptr->flags, FF_NEEDLE) && !IS_INVULN() && !p_ptr->levitation)
	{
		int damage = damroll(4, 10);
		msg_print(_("針が刺さった！", "You get stabbed by the needles!"));
		take_hit(DAMAGE_NOESCAPE, damage, format(_("%sの上に乗ったダメージ",
                                                "damage from climbing %s"), f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
		set_cut(damage / 2);

	}


	//v1.1.85 地形追加。ダメージ処理を別関数に移動した
	if (have_flag(f_ptr->flags, FF_LAVA) && terrain_dam(FF_LAVA)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_COLD_PUDDLE) && terrain_dam(FF_COLD_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE) && terrain_dam(FF_ELEC_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE) && terrain_dam(FF_ACID_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE) && terrain_dam(FF_POISON_PUDDLE)) cave_no_regen = TRUE;
#if 0
	///sys 溶岩ダメージ　メッセージ入れようか？
	if (have_flag(f_ptr->flags, FF_LAVA) && !IS_INVULN() && !p_ptr->immune_fire)
	{
		int damage = 0;

		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if (!p_ptr->levitation)
		{
			damage = 3000 + randint0(2000);
		}

		if(CHECK_FAIRY_TYPE == 5) damage = 0;
		else if(p_ptr->pclass == CLASS_SUWAKO) damage = 0;
		else if(p_ptr->pclass == CLASS_FUTO && p_ptr->lev > 29) damage = 0;
		else if(SAKUYA_WORLD) damage = 0;


		if (damage)
		{
			///race　溶岩ダメージ処理
			/*
			if (prace_is_(RACE_ENT)) damage += damage / 3;
			if (p_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE()) damage = damage / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			damage = mod_fire_dam(damage);

			if (p_ptr->levitation) damage = damage / 5;

			damage = damage / 100 + (randint0(100) < (damage % 100));

			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("熱で火傷した！");
				take_hit(DAMAGE_NOESCAPE, damage, format("%sの上に浮遊したダメージ", f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
#else
				msg_print("The heat burns you!");
				take_hit(DAMAGE_NOESCAPE, damage, format("flying over %s", f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
#endif
			}
			else
			{
				cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
#ifdef JP
				msg_format("%sで火傷した！", name);
#else
				msg_format("The %s burns you!", name);
#endif
				take_hit(DAMAGE_NOESCAPE, damage, name, -1);
			}
			///mod140719
			if (p_ptr->wild_mode)
			{
				/*:::-Hack- 全体マップ移動中はenergy_useが数倍になってて溶岩地形に入った瞬間何度も食らって死にかねず不自然なので直しておく*/
				energy_use = MIN(energy_use, 100);
				change_wild_mode();
			}
			cave_no_regen = TRUE;
		}
	}
#endif

	if (p_ptr->drowning)
	{
		int dam;
		if (prace_is_(RACE_VAMPIRE)) dam = 30 + randint1(20);
		else dam = randint1(20);

		take_hit(DAMAGE_NOESCAPE, dam, _("溺れ", "drowning"), -1);
		cave_no_regen = TRUE;
	}
	//v1.1.86 虹龍洞深部では酸素の必要なプレーヤーはダメージ
	else if (dungeon_type == DUNGEON_KOURYUU && check_player_need_oxygen())
	{
		if (dun_level <= 35)
		{
			if (one_in_(100)) msg_print(_("体調がすぐれない。空気が悪いのだろうか。",
                                            "You're not feeling well. Is there something wrong with the air?"));
			cave_no_regen = TRUE;

		}
		else if (dun_level > 35 && dun_level <= 45)
		{
			if (one_in_(20)) msg_print(_("何だか息が苦しい...", "You have hard time breathing..."));
			cave_no_regen = TRUE;
			take_hit(DAMAGE_NOESCAPE, 1, _("酸欠", "lack of oxygen"), -1);

		}
		else if (dun_level > 45 && dun_level <= 55)
		{
			if(one_in_(5) && PLAYER_CAN_COMMAND)msg_print(_("まともに呼吸ができない。何も考えられなくなってきた。",
                                                            "You can't breathe properly, and you have hard time thinking straight."));

			cave_no_regen = TRUE;
			if(one_in_(4))set_stun(p_ptr->stun + 10 + randint1(10));
			if (one_in_(3)) do_dec_stat(A_INT);
			if (!p_ptr->resist_conf)set_confused(p_ptr->confused + randint1(5));

			take_hit(DAMAGE_NOESCAPE, 5 + randint1(5), _("酸欠", "lack of oxygen"), -1);

		}
		else
		{
			if (PLAYER_CAN_COMMAND)msg_print(_("意識が途切れそうだ！ここは人が立ち入るべき場所ではない！",
                                                "You are about to faint! This isn't a place for humans!"));
			cave_no_regen = TRUE;
			set_stun(p_ptr->stun + 20 + randint1(20));
			do_dec_stat(A_INT);
			if (!p_ptr->resist_conf)set_confused(p_ptr->confused + 10 + randint1(10));
			if (!p_ptr->resist_chaos)set_image(p_ptr->image + 10 + randint1(10));
			take_hit(DAMAGE_NOESCAPE, 10 + randint1(10), _("酸欠", "lack of oxygen"), -1);

		}

	}


#if 0
	if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
	    !p_ptr->levitation && !p_ptr->can_swim)
	{
		if (p_ptr->total_weight > weight_limit())
		{
			/* Take damage */
#ifdef JP

			//msg_print("溺れている！");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "溺れ", -1);
#else
			msg_print("You are drowning!");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "drowning", -1);
#endif

			cave_no_regen = TRUE;
		}
	}
#endif

	if (p_ptr->riding)
	{
		int damage;
		//v1.1.78 二重耐性のときオーラダメージを受けないよう変更
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_FIRE) && !p_ptr->immune_fire && !(IS_OPPOSE_FIRE()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (prace_is_(RACE_ENT)) damage += damage / 3;
			if (p_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE()) damage = damage / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			damage = mod_fire_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

	#ifdef JP
msg_print("熱い！");
take_hit(DAMAGE_NOESCAPE, damage, "炎のオーラ", -1);
#else
			msg_print("It's hot!");
			take_hit(DAMAGE_NOESCAPE, damage, "Fire aura", -1);
#endif
		}
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_ELEC) && !p_ptr->immune_elec && !(IS_OPPOSE_ELEC()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (prace_is_(RACE_KAPPA)) damage += damage / 3;
			if (p_ptr->resist_elec) damage = damage / 3;
			if (IS_OPPOSE_ELEC()) damage = damage / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			damage = mod_elec_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

#ifdef JP
msg_print("痛い！");
take_hit(DAMAGE_NOESCAPE, damage, "電気のオーラ", -1);
#else
			msg_print("It hurts!");
			take_hit(DAMAGE_NOESCAPE, damage, "Elec aura", -1);
#endif
		}

///mod131231 モンスターフラグ変更
//		if ((r_info[m_list[p_ptr->riding].r_idx].flags3 & RF3_AURA_COLD) && !p_ptr->immune_cold)
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_COLD) && !p_ptr->immune_cold && !(IS_OPPOSE_COLD()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (p_ptr->resist_cold) damage = damage / 3;
			if (IS_OPPOSE_COLD()) damage = damage / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			damage = mod_cold_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

#ifdef JP
msg_print("冷たい！");
take_hit(DAMAGE_NOESCAPE, damage, "冷気のオーラ", -1);
#else
			msg_print("It's cold!");
			take_hit(DAMAGE_NOESCAPE, damage, "Cold aura", -1);
#endif
		}
	}

	/* Spectres -- take damage when moving through walls */
	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
	{
		if (!IS_INVULN() && !p_ptr->wraith_form && !p_ptr->kabenuke
			&& CHECK_FAIRY_TYPE != 41
			&& !(IS_METAMORPHOSIS && (r_info[MON_EXTRA_FIELD].flags2 & RF2_PASS_WALL))
			&& !((p_ptr->pclass == CLASS_SEIGA) && (p_ptr->lev > 39))
			&& !((p_ptr->pclass == CLASS_OKINA) && (have_flag(f_ptr->flags, FF_DOOR)))
			&& !((p_ptr->pclass == CLASS_YUKARI) && (p_ptr->lev > 39))
		    && ((p_ptr->chp > (p_ptr->lev / 5)) || !p_ptr->pass_wall))
		{
			cptr dam_desc;

			cave_no_regen = TRUE;


			if (p_ptr->pass_wall)
			{
#ifdef JP
				msg_print("体の分子が分解した気がする！");
				dam_desc = "密度";
#else
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
#endif
			}
			else
			{
#ifdef JP
				msg_print("崩れた岩に押し潰された！");
				dam_desc = "硬い岩";
#else
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
#endif
			}

			take_hit(DAMAGE_NOESCAPE, 1 + (p_ptr->lev / 5), dam_desc, -1);
		}
	}


	/*** handle regeneration ***/

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}
	}

	//v1.1.41 舞と里乃が「装備が重くて俊敏に動くのが難しい」ときに踊っていると自然回復速度大幅低下
	if (CLASS_RIDING_BACKDANCE && (p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO)
		&& heavy_armor() && p_ptr->riding)
	{
		regen_amount /= 3;
	}

	/* Are we walking the pattern? */
	if (pattern_effect())
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
		if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
		{
			regen_amount /= 2;
		}
		if (p_ptr->cursed & TRC_SLOW_REGEN)
		{
			regen_amount /= 5;
		}
	}


	/* Searching or Resting */
	if ((p_ptr->action == ACTION_SEARCH) || (p_ptr->action == ACTION_REST))
	{
		regen_amount = regen_amount * 2;
		//美鈴は休憩時の回復が早い
		if(p_ptr->pclass == CLASS_MEIRIN && (p_ptr->action == ACTION_REST)) regen_amount = regen_amount * 2;
	}

	upkeep_factor = calculate_upkeep();

	/* No regeneration while special action */
	if ((p_ptr->action == ACTION_LEARN) ||
	    (p_ptr->action == ACTION_HAYAGAKE) ||
	    (p_ptr->special_defense & KATA_KOUKIJIN))
	{
		upkeep_factor += 100;
	}

	/* Regenerate the mana */
	regenmana(upkeep_factor, regen_amount);

	//v1.1.15 ヘカーティアの体のHP回復
	if(p_ptr->pclass == CLASS_HECATIA)
	{
		int i;
		for(i=0;i<6;i++)
		{
			p_ptr->magic_num1[i] += p_ptr->lev * 4;
			if(p_ptr->magic_num1[i] > 10000) p_ptr->magic_num1[i] = 10000;
		}
	}


	/* Recharge magic eater's power */
	///class　魔道具術士の魔法自然回復
	if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
	{
		regenmagic(regen_amount);
	}

	if ((p_ptr->csp == 0) && (p_ptr->csp_frac == 0))
	{
		/*:::100を超えるときは常にペットが居る*/
		while (upkeep_factor > 100)
		{
#ifdef JP
			///msg131221
			msg_print("こんなに多くの配下を制御できない！");
#else
			msg_print("Too many followers to control at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss();

			upkeep_factor = calculate_upkeep();

#ifdef JP
			msg_format("維持ＭＰは %d%%", upkeep_factor);
#else
			msg_format("Upkeep: %d%% MP.", upkeep_factor);
#endif
			msg_print(NULL);
		}
	}

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > HINA_YAKU_LIMIT3) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * mutant_regenerate_mod) / 100;

	/* Regenerate Hit Points if needed */
	if ((p_ptr->chp < p_ptr->mhp) && !cave_no_regen)
	{
		regenhp(regen_amount);
	}
}


/*
 * Handle timeout every 10 game turns
 */
/*:::ステータス異常や一時効果などの有効期間カウント*/
static void process_world_aux_timeout(void)
{
	const int dec_count = (easy_band ? 2 : 1);
	int i;

	/*** Timeout Various Things ***/
	//スターサファイアの爆弾起爆 アイテムカードを使うためEXモードでも対象に
	//v1.1.14 サグメも追加
	//v1.1.21 もう面倒だから全員カウントしてしまう。走るときにちょっと重いかもしれんが
	//if(EXTRA_MODE || p_ptr->pclass == CLASS_SAGUME || p_ptr->pclass == CLASS_STAR || p_ptr->pclass == CLASS_3_FAIRIES)
	{
		int x,y;
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				if(have_flag(f_info[cave[y][x].mimic].flags, FF_BOMB)) bomb_count(y,x,-1);

			}
		}
	}

	//v1.1.57 特殊武器「七星の剣」のタイムカウントと消滅処理
	//カウントの記録にxtra4を使うことにした。
	//v2.0.18 フランの「魔法の剣」も同じ処理で消滅
	for (i = INVEN_RARM; i <= INVEN_LARM; i++)
	{
		object_type *o_ptr = &inventory[i];

		if (o_ptr->tval != TV_SWORD || (o_ptr->sval != SV_WEAPON_SEVEN_STAR && o_ptr->sval != SV_WEAPON_MAGIC_SWORD)) continue;

		o_ptr->xtra4++;

		if (o_ptr->xtra4 > 25)
		{
			msg_print(_("手の中の剣が消えた。", "The sword in your hand vanishes."));
			inven_item_increase(i, -1);
			inven_item_optimize(i);
			kamaenaoshi(i);
			p_ptr->update |= (PU_BONUS | PU_TORCH);
			p_ptr->redraw |= (PR_EQUIPPY | PR_HP | PR_MANA);
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			update_stuff();
			disturb(1, 1);
		}

	}

	/* Mimic */
	if (p_ptr->tim_mimic)
	{
		(void)set_mimic(p_ptr->tim_mimic - 1, p_ptr->mimic_form, TRUE);
	}

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - dec_count);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - dec_count);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1, TRUE);
	}

	/*:::カオスパトロンからすでに報酬をもらったカウントを元に戻す*/
	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1, TRUE);
	}

	/* Timed temporary elemental brands. -LM- */
	if (p_ptr->ele_attack)
	{
		p_ptr->ele_attack--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_attack) set_ele_attack(0, 0);
	}

	/* Timed temporary elemental immune. -LM- */
	if (p_ptr->ele_immune)
	{
		p_ptr->ele_immune--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_immune) set_ele_immune(0, 0);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1, TRUE);
	}

	/* Timed stealth */
	if (p_ptr->tim_stealth)
	{
		(void)set_tim_stealth(p_ptr->tim_stealth - 1, TRUE);
	}

	/* Timed levitation */
	if (p_ptr->tim_levitation)
	{
		(void)set_tim_levitation(p_ptr->tim_levitation - 1, TRUE);
	}

	///mod140813 光速移動　行動回数カウントだったのを通常のタイムアップ処理に変更した
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr->lightspeed - 1, TRUE);
	}

	/* Timed sh_touki */
	if (p_ptr->tim_sh_touki)
	{
		(void)set_tim_sh_touki(p_ptr->tim_sh_touki - 1, TRUE);
	}

	/* Timed sh_fire */
	if (p_ptr->tim_sh_fire)
	{
		(void)set_tim_sh_fire(p_ptr->tim_sh_fire - 1, TRUE);
	}

	/* Timed sh_holy */
	if (p_ptr->tim_sh_holy)
	{
		(void)set_tim_sh_holy(p_ptr->tim_sh_holy - 1, TRUE);
	}

	/* Timed eyeeye */
	if (p_ptr->tim_eyeeye)
	{
		(void)set_tim_eyeeye(p_ptr->tim_eyeeye - 1, TRUE);
	}

	if (p_ptr->tim_res_insanity)
	{
		(void)set_tim_res_insanity(p_ptr->tim_res_insanity - 1, TRUE);
	}

	/* Timed resist-magic */
	if (p_ptr->resist_magic)
	{
		(void)set_resist_magic(p_ptr->resist_magic - 1, TRUE);
	}

	/* Timed regeneration */
	if (p_ptr->tim_regen)
	{
		(void)set_tim_regen(p_ptr->tim_regen - 1, TRUE);
	}
	/*:::神降ろし用カウントダウン*/
	if (p_ptr->kamioroshi_count)
	{
		p_ptr->kamioroshi_count--;
		if (!p_ptr->kamioroshi_count) set_kamioroshi(0, 0);
	}
	/* Timed resist nether */
	if (p_ptr->tim_res_nether)
	{
		(void)set_tim_res_nether(p_ptr->tim_res_nether - 1, TRUE);
	}

	if (p_ptr->tim_res_dark)
	{
		(void)set_tim_res_dark(p_ptr->tim_res_dark - 1, TRUE);
	}
	for(i=0;i<TIM_GENERAL_MAX;i++)
	{
		if (p_ptr->tim_general[i]) (void)set_tim_general(p_ptr->tim_general[i] - 1, TRUE, i, 0);
	}

	///mod140325 一時的能力上昇
	for(i=0;i<6;i++)
	{
		if (p_ptr->tim_addstat_count[i]) (void)set_tim_addstat(i,0,p_ptr->tim_addstat_count[i] - 1, TRUE);
	}

	if (p_ptr->tim_rokuro_head_search)
	{
		rokuro_head_search_item(p_ptr->tim_rokuro_head_search - 1, FALSE);
	}
	if (p_ptr->tim_hirari_nuno)
	{
		set_hirarinuno_card(p_ptr->tim_hirari_nuno - 1, TRUE);

	}


	/*::: 幸運のメカニズム */
	if (p_ptr->lucky)
	{
		(void)set_tim_lucky(p_ptr->lucky - 1, TRUE);
	}
	/*::: 未来予知 */
	if (p_ptr->foresight)
	{
		(void)set_foresight(p_ptr->foresight - 1, TRUE);
	}
	/*::: 送還術 */
	if (p_ptr->deportation)
	{
		(void)set_deportation(p_ptr->deportation - 1, TRUE);
	}

	/*::: 爪強化 */
	if (p_ptr->clawextend)
	{
		(void)set_clawextend(p_ptr->clawextend - 1, TRUE);
	}

	/*::: Timed speedster*/
	if (p_ptr->tim_speedster)
	{
		(void)set_tim_speedster(p_ptr->tim_speedster - 1, TRUE);
	}
	/*::: Timed resist water */
	if (p_ptr->tim_res_water)
	{
		(void)set_tim_res_water(p_ptr->tim_res_water - 1, TRUE);
	}
	/*::: Timed resist chaos */
	if (p_ptr->tim_res_chaos)
	{
		(void)set_tim_res_chaos(p_ptr->tim_res_chaos - 1, TRUE);
	}
	/*::: レーダーセンス */
	if (p_ptr->radar_sense)
	{
		(void)set_radar_sense(p_ptr->radar_sense - 1, TRUE);
	}

	if (p_ptr->tim_unite_darkness)
	{
		(void)set_tim_unite_darkness(p_ptr->tim_unite_darkness - 1, TRUE);
	}

	/*::: 一時超隠密 */
	if (p_ptr->tim_superstealth)
	{
		(void)set_tim_superstealth(p_ptr->tim_superstealth - 1, TRUE, 0);
	}

	//v1.1.56
	if (p_ptr->tim_spellcard_count)
	{
		set_tim_spellcard_effect(p_ptr->tim_spellcard_count - 1, TRUE, 0);
	}

	/* Timed resist time */
	if (p_ptr->tim_res_time)
	{
		(void)set_tim_res_time(p_ptr->tim_res_time - 1, TRUE);
	}
	///mod140326
	/* Reactive Heal */
	if (p_ptr->reactive_heal)
	{
		(void)set_reactive_heal(p_ptr->reactive_heal - 1, TRUE);
	}
	/* Timed reflect */
	if (p_ptr->tim_reflect)
	{
		(void)set_tim_reflect(p_ptr->tim_reflect - 1, TRUE);
	}
	/* 念縛 */
	if (p_ptr->nennbaku)
	{
		(void)set_nennbaku(p_ptr->nennbaku - 1, TRUE);
	}
	//v1.1.93 一時反感
	if (p_ptr->tim_aggravation)
	{
		(void)set_tim_aggravation(p_ptr->tim_aggravation - 1, TRUE);
	}

	//v2.1.1 新魔法用追加
	if (p_ptr->tim_res_blast)
	{
		(void)set_res_blast(p_ptr->tim_res_blast - 1);
	}
	if (p_ptr->tim_rob_mana)
	{
		(void)set_rob_mana(p_ptr->tim_rob_mana - 1);
	}


	if (p_ptr->transportation_trap)
	{
		set_tim_transportation_trap(p_ptr->transportation_trap - 1, TRUE);

	}


	/* Multi-shadow */
	if (p_ptr->multishadow)
	{
		(void)set_multishadow(p_ptr->multishadow - 1, TRUE);
	}

	/* Timed Robe of dust */
	if (p_ptr->dustrobe)
	{
		(void)set_dustrobe(p_ptr->dustrobe - 1, TRUE);
	}

	/* Timed infra-vision */
	if (p_ptr->kabenuke)
	{
		(void)set_kabenuke(p_ptr->kabenuke - 1, TRUE);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - dec_count);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - dec_count);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - dec_count);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1, TRUE);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - dec_count, TRUE);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1, TRUE);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1, TRUE);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
		(void)set_wraith_form(p_ptr->wraith_form - 1, TRUE);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1, TRUE);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1, TRUE);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1, TRUE);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1, TRUE);
	}

	/* Tsubureru */
	if (p_ptr->tim_sh_death)
	{
		(void)set_tim_sh_death(p_ptr->tim_sh_death - 1, TRUE);
	}

	/* Magicdef */
	if (p_ptr->magicdef)
	{
		(void)set_magicdef(p_ptr->magicdef - 1, TRUE);
	}

	/* Tsuyoshi */
	if (p_ptr->tsuyoshi)
	{
		(void)set_tsuyoshi(p_ptr->tsuyoshi - 1, TRUE);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1, TRUE);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1, TRUE);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1, TRUE);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1, TRUE);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1, TRUE);
	}

	if (p_ptr->ult_res)
	{
		(void)set_ultimate_res(p_ptr->ult_res - 1, TRUE);
	}

	/*** Poison and Stun and Cut ***/


	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	//v1.1.66 ステータス異常「破損」
	if (REF_BROKEN_VAL)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		set_broken(-(adjust)); //set_stun()などと違い増減値を直接渡す

	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}

	//v2.0.11 移動禁止 回復には耐久でなく腕力を使う
	if (p_ptr->tim_no_move)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_STR]] + 1;

		/* Apply some healing */
		(void)set_no_move(p_ptr->tim_no_move - adjust);
	}

	///mod140326 アルコールも同様に治癒するようにする
	if (p_ptr->alcohol)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		///mod150131 意識不明瞭時、ちょっと回復早く
		//v1.1.12 もっと早く
		//v1.1.78 さらに5倍 *10→*50
		if(p_ptr->alcohol >= DRANK_4)
		{
			adjust += 5;
			adjust *= 50;
		}
		(void)set_alcohol(p_ptr->alcohol - adjust);
	}
	///mod140813 喘息も同様に治癒するようにする
	if (p_ptr->asthma)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		(void)set_asthma(p_ptr->asthma - adjust);
	}


}


/*
 * Handle burning fuel every 10 game turns
 */
/*:::光源の燃料減少処理*/
static void process_world_aux_light(void)
{
	/* Check for light being wielded */
	object_type *o_ptr = &inventory[INVEN_LITE];

	if(check_invalidate_inventory(INVEN_LITE)) return;

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if (!(object_is_fixed_artifact(o_ptr) || o_ptr->sval == SV_LITE_FEANOR) && (o_ptr->xtra4 > 0))
		{
			/* Decrease life-span */
			if (o_ptr->name2 == EGO_LITE_LONG)
			{
				if (turn % (TURNS_PER_TICK*2)) o_ptr->xtra4--;
			}
			else o_ptr->xtra4--;

			/* Notice interesting fuel steps */
			notice_lite_change(o_ptr);
		}
	}
}


/*
 * Handle mutation effects once every 10 game turns
 */
/*:::突然変異のランダム発動 muta2のみ*/
static void process_world_aux_mutation(void)
{
	/* No mutation with effects */
	if (!p_ptr->muta2) return;

	/* No effect on monster arena */
	if (p_ptr->inside_battle) return;

	/* No effect on the global map */
	if (p_ptr->wild_mode) return;


	if ((p_ptr->muta2 & MUT2_BERS_RAGE) && one_in_(3000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("突然あなたの激情が爆発した！");
#else
		msg_print("You feel a fit of rage coming over you!");
#endif

		(void)set_shero(10 + randint1(p_ptr->lev), FALSE);
		(void)set_afraid(0);
	}

	if ((p_ptr->muta2 & MUT2_COWARDICE) && (randint1(3000) == 13))
	{
		if (!p_ptr->resist_fear || IS_VULN_FEAR)
		{
			disturb(0, 1);
#ifdef JP
			msg_print("とても暗い... とても恐い！");
#else
			msg_print("It's so dark... so scary!");
#endif

			set_afraid(p_ptr->afraid + 13 + randint1(26));
		}
	}

	if ((p_ptr->muta2 & MUT2_RTELEPORT) && (randint1(5000) == 88)  || p_ptr->pclass == CLASS_KOISHI && one_in_(2000))
	{
		///mod140121 ランダムテレポ変異　耐性防御不可 低確率で何か拾っている

			disturb(1, 1);

			teleport_player(30 + randint0(170), TELEPORT_NONMAGICAL);
			msg_print(_("…いつの間にか知らない場所にいた。", "...You are suddenly standing in an unfamiliar place."));
			msg_print(NULL);
			msg_print(NULL);

			if( (p_ptr->lev + 40 < dun_level) && weird_luck())
			{
				msg_print(_("何かを手に持っていた。", "You're holding something in your hand."));
				acquirement(py, px, 1, TRUE, FALSE);
			}

	}

	if ((p_ptr->muta2 & MUT2_HALLU) && (randint1(6400) == 42))
	{
		if (!p_ptr->resist_chaos)
		{
			disturb(0, 1);
			p_ptr->redraw |= PR_EXTRA;
			(void)set_image(p_ptr->image + randint0(50) + 20);
		}
	}

	if ((p_ptr->muta2 & MUT2_YAKU) && (randint1(3000) == 13))
	{
		disturb(0, 1);
		curse_equipment(100,(dun_level / 3 + 1));

	}

	if ((p_ptr->muta2 & MUT2_PROD_MANA) &&
	    !p_ptr->anti_magic && one_in_(9000))
	{
		int dire = 0;
		disturb(0, 1);
#ifdef JP
		msg_print("魔法のエネルギーが突然あなたの中に流れ込んできた！エネルギーを解放しなければならない！");
#else
		msg_print("Magical energy flows through you! You must release it!");
#endif

		flush();
		msg_print(NULL);
		(void)get_hack_dir(&dire);
		fire_ball(GF_MANA, dire, p_ptr->lev * 2, 3);
	}

	if ((p_ptr->muta2 & MUT2_ATT_DEMON) &&
	    !p_ptr->anti_magic && (randint1(6666) == 666))
	{
		bool pet = one_in_(6);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px,
				    dun_level, SUMMON_DEMON, mode))
		{
#ifdef JP
			msg_print("あなたはデーモンを引き寄せた！");
#else
			msg_print("You have attracted a demon!");
#endif

			disturb(0, 1);
		}
	}

	if ((p_ptr->muta2 & MUT2_SPEED_FLUX) && one_in_(6000))
	{
		disturb(0, 1);
		if (one_in_(2))
		{
#ifdef JP
			msg_print("精力的でなくなった気がする。");
#else
			msg_print("You feel less energetic.");
#endif

			if (p_ptr->fast > 0)
			{
				set_fast(0, TRUE);
			}
			else
			{
				set_slow(randint1(30) + 10, FALSE);
			}
		}
		else
		{
#ifdef JP
			msg_print("精力的になった気がする。");
#else
			msg_print("You feel more energetic.");
#endif

			if (p_ptr->slow > 0)
			{
				set_slow(0, TRUE);
			}
			else
			{
				set_fast(randint1(30) + 10, FALSE);
			}
		}
		msg_print(NULL);
	}
	if ((p_ptr->muta2 & MUT2_BANISH_ALL) && one_in_(9000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("突然ほとんど孤独になった気がする。");
#else
		msg_print("You suddenly feel almost lonely.");
#endif

		banish_monsters(100);
		///del131221 店主は交代しない
#if 0
		if (!dun_level && p_ptr->town_num)
		{
			int n;

			/* Pick a random shop (except home) */
			do
			{
				n = randint0(MAX_STORES);
			}
			while ((n == STORE_HOME) || (n == STORE_MUSEUM));

#ifdef JP
			msg_print("店の主人が丘に向かって走っている！");
#else
			msg_print("You see one of the shopkeepers running for the hills!");
#endif

			store_shuffle(n);
		}
#endif
		msg_print(NULL);
	}

	if ((p_ptr->muta2 & MUT2_EAT_LIGHT) && one_in_(3000))
	{
		object_type *o_ptr;

#ifdef JP
		msg_print("影につつまれた。");
#else
		msg_print("A shadow passes over you.");
#endif

		msg_print(NULL);

		/* Absorb light from the current possition */
		if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
		{
			hp_player(10);
		}

		o_ptr = &inventory[INVEN_LITE];

		/* Absorb some fuel in the current lite */
		if (o_ptr->tval == TV_LITE)
		{
			/* Use some fuel (except on artifacts) */
			if (!object_is_fixed_artifact(o_ptr) && (o_ptr->xtra4 > 0))
			{
				/* Heal the player a bit */
				hp_player(o_ptr->xtra4 / 20);

				/* Decrease life-span of lite */
				o_ptr->xtra4 /= 2;

#ifdef JP
				msg_print("光源からエネルギーを吸収した！");
#else
				msg_print("You absorb energy from your light!");
#endif


				/* Notice interesting fuel steps */
				notice_lite_change(o_ptr);
			}
		}

		/*
		 * Unlite the area (radius 10) around player and
		 * do 50 points damage to every affected monster
		 */
		unlite_area(50, 10);
	}

	if ((p_ptr->muta2 & MUT2_ATT_ANIMAL) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_ANIMAL, mode))
		{
#ifdef JP
			msg_print("動物を引き寄せた！");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 1);
		}
	}

	if ((p_ptr->muta2 & MUT2_RAW_CHAOS) &&
	    !p_ptr->anti_magic && one_in_(8000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("周りの空間が歪んでいる気がする！");
#else
		msg_print("You feel the world warping around you!");
#endif

		msg_print(NULL);
		fire_ball(GF_CHAOS, 0, p_ptr->lev, 8);
	}
	if ((p_ptr->muta2 & MUT2_NORMALITY) && one_in_(5000))
	{
		if (!lose_mutation(0))
#ifdef JP
			msg_print("通常に近づいていく気がする。");
#else
		msg_print("You feel oddly normal.");
#endif

	}
	if ((p_ptr->muta2 & MUT2_WRAITH) && !p_ptr->anti_magic && one_in_(3000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("非物質化した！");
#else
		msg_print("You feel insubstantial!");
#endif

		msg_print(NULL);
		set_wraith_form(randint1(p_ptr->lev / 2) + (p_ptr->lev / 2), FALSE);
	}
	if ((p_ptr->muta2 & MUT2_ATT_GHOST) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_GHOST, mode))
		{
#ifdef JP
			msg_print("幽霊が寄ってきた！");
#else
			msg_print("You have attracted a ghost!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_ATT_KWAI) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_KWAI, mode))
		{
#ifdef JP
			msg_print("妖怪が寄ってきた！");
#else
			msg_print("You have attracted a youkai!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_ATT_DRAGON) &&
	    !p_ptr->anti_magic && one_in_(3000))
	{
		bool pet = one_in_(5);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_DRAGON, mode))
		{
#ifdef JP
			msg_print("ドラゴンを引き寄せた！");
#else
			msg_print("You have attracted a dragon!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_XOM) && one_in_(3000))
	{
#ifdef JP
			msg_print("ゾム「残念ながらまだこれしか持ってない」");
#else
			msg_print("Xom 'Sorry, but I haven't got anything else for now'");
#endif
		(void)set_shero(10 + randint1(p_ptr->lev), FALSE);
		(void)set_afraid(0);

	}
	if ((p_ptr->muta2 & MUT2_BIRDBRAIN) && one_in_(3000) || p_ptr->pclass == CLASS_KOISHI && one_in_(1000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("…？　何かを忘れている気がする。");
#else
		msg_print("...? You feel you've forgotten something.");
#endif

		lose_all_info();
		if (music_singing_any()) stop_singing();
		if (hex_spelling_any()) stop_hex_spell_all();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	if ((p_ptr->muta2 & MUT2_WALK_SHAD) &&
	    !p_ptr->anti_magic && one_in_(12000) && !p_ptr->inside_arena)
	{
		alter_reality();
	}

	if ((p_ptr->muta2 & MUT2_WARNING) && one_in_(1000))
	{
		int danger_amount = 0;
		int monster;

		for (monster = 0; monster < m_max; monster++)
		{
			monster_type    *m_ptr = &m_list[monster];
			monster_race    *r_ptr = &r_info[m_ptr->r_idx];

			/* Paranoia -- Skip dead monsters */
			if (!m_ptr->r_idx) continue;

			if (r_ptr->level >= p_ptr->lev)
			{
				danger_amount += r_ptr->level - p_ptr->lev + 1;
			}
		}

		if (danger_amount > 100)
#ifdef JP
			msg_print("非常に恐ろしい気がする！");
#else
		msg_print("You feel utterly terrified!");
#endif

		else if (danger_amount > 50)
#ifdef JP
			msg_print("恐ろしい気がする！");
#else
		msg_print("You feel terrified!");
#endif

		else if (danger_amount > 20)
#ifdef JP
			msg_print("非常に心配な気がする！");
#else
		msg_print("You feel very worried!");
#endif

		else if (danger_amount > 10)
#ifdef JP
			msg_print("心配な気がする！");
#else
		msg_print("You feel paranoid!");
#endif

		else if (danger_amount > 5)
#ifdef JP
			msg_print("ほとんど安全な気がする。");
#else
		msg_print("You feel almost safe.");
#endif

		else
#ifdef JP
			msg_print("寂しい気がする。");
#else
		msg_print("You feel lonely.");
#endif

	}
	if ((p_ptr->muta2 & MUT2_INVULN) && !p_ptr->anti_magic &&
	    one_in_(5000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("無敵な気がする！");
#else
		msg_print("You feel invincible!");
#endif

		msg_print(NULL);
		(void)set_invuln(randint1(8) + 8, FALSE);
	}

	///mod140813 喘息
	if ((p_ptr->muta2 & MUT2_ASTHMA) && one_in_(1000))
	{
		msg_print(_("喘息の発作だ！", "You have an asthma attack!"));
		(void)set_asthma(p_ptr->asthma + 1000 + randint0(2000));
	}

	if ((p_ptr->muta2 & MUT2_THE_WORLD) && !p_ptr->anti_magic &&
	    one_in_(4000))
	{
		disturb(0, 1);
		if (world_player)
		{
#ifdef JP
			msg_print("既に時は止まっている。");
#else
            msg_print("Time is already stopped.");
#endif
		}
		else
		{
			world_player = TRUE;
			if(p_ptr->pclass == CLASS_SAKUYA) sakuya_time_stop(FALSE);

#ifdef JP
			msg_print("突如、辺りの全てが静止した。");
#else
            msg_print("Suddenly everything around you stands still.");
#endif
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			/* Hack */
			p_ptr->energy_need -= 1000 + (400)*TURNS_PER_TICK/10;
			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);
			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();
		}
	}
	if ((p_ptr->muta2 & MUT2_DISARM) && one_in_(10000))
	{
		int slot = 0;
		object_type *o_ptr = NULL;

		disturb(0, 1);
#ifdef JP
		msg_print("足がもつれて転んだ！");
		take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "転倒", -1);
#else
		msg_print("You trip over your own feet!");
		take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "tripping", -1);
#endif

		msg_print(NULL);
		if (buki_motteruka(INVEN_RARM))
		{
			slot = INVEN_RARM;
			o_ptr = &inventory[INVEN_RARM];

			if (buki_motteruka(INVEN_LARM) && one_in_(2))
			{
				o_ptr = &inventory[INVEN_LARM];
				slot = INVEN_LARM;
			}
		}
		else if (buki_motteruka(INVEN_LARM))
		{
			o_ptr = &inventory[INVEN_LARM];
			slot = INVEN_LARM;
		}

		///pend 武器をうっかり落とす部分。一部キャラクターは除外処理要
		if (slot && !object_is_cursed(o_ptr) && p_ptr->pclass != CLASS_KOGASA)
		{
#ifdef JP
			msg_print("武器を落としてしまった！");
#else
			msg_print("You drop your weapon!");
#endif
			inven_drop(slot, 1);
		}
	}
}

/*
 * Handle curse effects once every 10 game turns
 */
/*:::呪い発生処理　トランプ武器のランダムテレポや審判の宝石のHP吸収も*/
static void process_world_aux_curse(void)
{

	if(p_ptr->inside_battle) return;

	//v1.1.48 今更だが広域マップでは発動しないことにした
	if (p_ptr->wild_mode) return;

	//v2.1.0 瑞霊は装備品の呪い発生を防ぐ
	if (p_ptr->pclass == CLASS_MIZUCHI) return;

	//v1.1.48 紫苑の不運パワーが多すぎるとき確率で勝手にスーパー貧乏神に変身する
	if (p_ptr->pclass == CLASS_SHION && one_in_(99) && !p_ptr->tim_mimic)
	{
		int power = CALC_SUPER_SHION_TURNS;

		if (randint1(power) > 13)
		{

			msg_print(_("溜め込まれた不運パワーが暴走した！",
                        "The power of misfortune you've accumulated runs wild!"));
			disturb(1, 1);
			set_tim_general(power, FALSE, 0, 0);

			//変身時にmagic_num1[0]を現在ターン数にし[1]をリセット
			p_ptr->magic_num1[0] = turn;
			p_ptr->magic_num1[1] = 0;
			//HP全回復
			hp_player(9999);


		}
		else if (power >= 13)
		{
			msg_print(_("溜め込まれた不運パワーが暴走しそうな気がする...",
                        "You feel the power of misfortune you've accumulated is about to run wild..."));

		}
	}

	//v1.1.48 紫苑がスーパー貧乏神のときには色々起こる
	if (SUPER_SHION && one_in_(66) && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		int mode = randint1(6);
		switch (mode)
		{

		case 1:
		case 2:
		{
			int y, x, feat;
			feature_type *f_ptr;

			if (mode == 1)msg_print(_("あなたは鉄砲水に襲われた！", "You cause a flash flood!"));
			if (mode == 2)msg_print(_("あなたは溶岩流に襲われた！", "You cause a lava flood!"));

			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					cave_type *c_ptr = &cave[y][x];
					feat = get_feat_mimic(c_ptr);
					f_ptr = &f_info[feat];

					if (mode == 1 && have_flag(f_ptr->flags, FF_FLOOR))
						cave_set_feat(y, x, feat_deep_water);
					if (mode == 2 && have_flag(f_ptr->flags, FF_FLOOR))
						cave_set_feat(y, x, feat_deep_lava);
				}
			}

			p_ptr->redraw |= (PR_MAP);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();
			break;
		}

		case 3:
			if (dun_level)
			{
				msg_print(_("あなたは落盤に襲われた！", "You cause a cave-in!"));
				earthquake(py, px, 12);

			}
			else
			{
				msg_print(_("あなたは竜巻に襲われた！", "You cause a tornado!"));
				project(0,9,py,px,666,GF_TORNADO,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
			}
			break;
		case 4:
			msg_print(_("あなたは流星雨に襲われた！", "You cause a meteor rain!"));
			cast_meteor(300, 4, GF_METEOR);
			break;
		case 5:
			msg_print(_("あなたは謎の時空の歪みに襲われた！", "You cause a mysterious space-time distortion!"));
			project_hack2(GF_DISTORTION, 1, 1000, 0);
			break;
		case 6:
			msg_print(_("無尽蔵の負のオーラが放たれた！", "You release an inexhaustible negative aura!"));
			floor_attack(GF_DISENCHANT, 0, 0, p_ptr->lev * 8, 0);
			break;
		}


	}

	//雛の厄が多い時のペナルティ
	if(p_ptr->pclass == CLASS_HINA  && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		int yaku = p_ptr->magic_num1[0];

		if(yaku > HINA_YAKU_LIMIT3)
		{
			if(one_in_(50))
			{
				int power = yaku / 50;
				switch(randint1(8))
				{
				case 1:
					msg_print(_("突如、巨大な竜巻が発生した！", "Suddenly, a massive tornado forms!"));
					project(0,8,py,px,power+randint1(power),GF_TORNADO,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 2:
					if(dun_level && !p_ptr->inside_quest)
					{
						msg_print(_("突如、地震が発生した！", "Suddenly, an earthquake happens!"));
						earthquake_aux(py,px,10,0,0);
					}
					else
						msg_print(_("地響きが聞こえた・・", "You hear the ground rumbling..."));
					break;
				case 3:
					msg_print(_("突如、流星雨が降り注いだ！", "Suddenly, a meteor rain occurs!"));
					cast_meteor(power/2, 3,GF_METEOR);
					break;
				case 4:
					msg_print(_("突如、重力場異常が発生した！", "Suddenly, a gravitational anomaly forms!"));
					project(0,8,py,px,power+randint1(power),GF_GRAVITY,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 5:
					msg_print(_("突如、カオスの渦が発生した！", "Suddenly, a whirlpool of chaos forms!"));
					project(0,8,py,px,power+randint1(power),GF_CHAOS,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 6:
					msg_print(_("突如、大嵐が発生した！", "Suddenly, there is a massive storm!"));
					project_hack2(GF_TSUKUMO_STORM,1,power/2,power/2);
					break;
				case 7:
					msg_print(_("突如、プラズマ球が発生した！", "Suddenly, a plasma sphere forms!"));
					project(0,8,py,px,power+randint1(power),GF_PLASMA,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 8:
					mass_genocide(power,FALSE);
					msg_print(_("辺りから他者の気配が消えた気がする。", "You feel the presence of other nearby beings vanish."));
					break;

				}

			}
		}
		if(yaku > (HINA_YAKU_LIMIT3) && dun_level > 10 && !p_ptr->inside_quest && one_in_(200))
		{
			bool entry = FALSE;
			if(one_in_(2))
			{
				if(summon_named_creature(0,py,px,MON_REIMU,PM_NO_PET)) entry = TRUE;
			}
			else
			{
				if(summon_named_creature(0,py,px,MON_MARISA,PM_NO_PET)) entry = TRUE;
			}

			if(entry)
			{
				msg_print(_("異変解決の専門家があなたを退治しにやってきた！",
                            "The incident resolution specialists have come to exterminate you!"));
				disturb(1, 1);
			}

		}
	}

	if ((p_ptr->cursed & TRC_P_FLAG_MASK) && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 * can actually be useful!
		 */
		if ((p_ptr->cursed & TRC_TELEPORT_SELF) && one_in_(200))
		{
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			int i, i_keep = 0, count = 0;

			/* Scan the equipment with random teleport ability */
			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				u32b flgs[TR_FLAG_SIZE];
				o_ptr = &inventory[i];

				/* Skip non-objects */
				if (!o_ptr->k_idx) continue;

				if(check_invalidate_inventory(i)) continue;

				/* Extract the item flags */
				object_flags(o_ptr, flgs);

				if (have_flag(flgs, TR_TELEPORT))
				{
					/* {.} will stop random teleportation. */
					if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '.'))
					{
						count++;
						if (one_in_(count)) i_keep = i;
					}
				}
			}

			o_ptr = &inventory[i_keep];
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
			msg_format("%sがテレポートの能力を発動させようとしている。", o_name);
#else
			msg_format("Your %s is activating teleportation.", o_name);
#endif

#ifdef JP
			if (get_check_strict("テレポートしますか？", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
			{
				disturb(0, 1);
				teleport_player(50, 0L);
			}
			else
			{
#ifdef JP
				msg_format("%sに{.}(ピリオド)と銘を刻むと発動を抑制できます。", o_name);
#else
				msg_format("You can inscribe {.} on your %s to disable random teleportation. ", o_name);
#endif
				disturb(1, 1);
			}
		}
		/* Make a chainsword noise */
		///item131221 チェーンソーのメッセージをストームブリンガーに差し替えてみた
		if ((p_ptr->cursed & TRC_STORMBRINGER) && one_in_(CHAINSWORD_NOISE))
		{
			char noise[1024];
#ifdef JP
			if (!get_rnd_line("storm_j.txt", 0, noise))
			//if (!get_rnd_line("chainswd_j.txt", 0, noise))

#else
			if (!get_rnd_line("storm.txt", 0, noise))
			//if (!get_rnd_line("storm.txt", 0, noise)))
#endif
				msg_print(noise);
			disturb(FALSE, FALSE);
		}
		/* TY Curse */
		if ((p_ptr->cursed & TRC_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
		{
			int count = 0;
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 20)
			{
				msg_format(_("太古の呪いが発動しかけたがどうにか宥めた。",
                            "Ancient foul curse was about to activate, but you calm the item down."));
				return;
			}
			(void)activate_ty_curse(FALSE, &count);
		}
		/* Handle experience draining */
		if (p_ptr->prace != RACE_ANDROID &&
			((p_ptr->cursed & TRC_DRAIN_EXP) && one_in_(4)))
		{
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 3) return;

			if(p_ptr->pclass == CLASS_HINA)
			{
				hina_gain_yaku(randint1(3));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				p_ptr->magic_num1[1] += 100 + randint1(100);
				return;
			}

			//v2.1.5 阿梨夜経験値吸収無効化
			if (p_ptr->pclass == CLASS_ARIYA) return;


			p_ptr->exp -= (p_ptr->lev+1)/2;
			if (p_ptr->exp < 0) p_ptr->exp = 0;
			p_ptr->max_exp -= (p_ptr->lev+1)/2;
			if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;
			check_experience();
		}
		/* Add light curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_L_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(50 + randint1(50));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format(_("呪いが発動しかけたがどうにか宥めた。",
                            "A curse was about to activate, but you calm the item down."));
				return;
			}
			o_ptr = choose_cursed_obj_name(TRC_ADD_L_CURSE);

			new_curse = get_curse(0, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Add heavy curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_H_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(300 + randint1(300));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 5000 + randint1(5000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format(_("強力な呪いが発動しかけたがどうにか宥めた。",
                            "A powerful curse was about to activate, but you calm the item down."));
				return;
			}
			o_ptr = choose_cursed_obj_name(TRC_ADD_H_CURSE);

			new_curse = get_curse(1, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("悪意に満ちた黒いオーラが%sをとりまいた...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Call animal */
		if ((p_ptr->cursed & TRC_CALL_ANIMAL) && one_in_(2500))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(50 + randint1(50));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format(_("呪いが発動しかけたがどうにか宥めた。",
                            "A curse was about to activate, but you calm the item down."));
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sが動物を引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 1);
			}
		}
		/* Call demon */
		if ((p_ptr->cursed & TRC_CALL_DEMON) && one_in_(1111))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(200 + randint1(200));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 3000 + randint1(3000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format(_("強力な呪いが発動しかけたがどうにか宥めた。",
                            "A powerful curse was about to activate, but you calm the item down."));
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sが悪魔を引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted a demon!", o_name);
#endif

				disturb(0, 1);
			}
		}
		/* Call dragon */
		if ((p_ptr->cursed & TRC_CALL_DRAGON) && one_in_(800))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(300 + randint1(300));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 5000 + randint1(5000);
				return;
			}
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format(_("強力な呪いが発動しかけたがどうにか宥めた。",
                            "A powerful curse was about to activate, but you calm the item down."));
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_DRAGON,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sがドラゴンを引き寄せた！", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 1);
			}
		}
		if ((p_ptr->cursed & TRC_COWARDICE) && one_in_(1500))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(30 + randint1(10));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format(_("呪いが発動しかけたがどうにか宥めた。",
                            "A curse was about to activate, but you calm the item down."));
				return;
			}
			if (!p_ptr->resist_fear || IS_VULN_FEAR)
			{
				disturb(0, 1);
#ifdef JP
				msg_print("とても暗い... とても恐い！");
#else
				msg_print("It's so dark... so scary!");
#endif

				set_afraid(p_ptr->afraid + 13 + randint1(26));
			}
		}
		/* Teleport player */
		if ((p_ptr->cursed & TRC_TELEPORT) && one_in_(200) && !p_ptr->anti_tele)
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(100 + randint1(200));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の強力な呪いが発動したが、あなたはそれを吸い取った。",
                            "A powerful curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 2000 + randint1(2000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format(_("強力な呪いが発動しかけたがどうにか宥めた。",
                            "A powerful curse was about to activate, but you calm the item down."));
				return;
			}
			disturb(0, 1);

			/* Teleport player */
			teleport_player(40, TELEPORT_PASSIVE);
		}
		/* Handle HP draining */
		if ((p_ptr->cursed & TRC_DRAIN_HP) && one_in_(666))
		{
			char o_name[MAX_NLEN];
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(20 + randint1(20));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 500 + randint1(500);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format(_("呪いが発動しかけたがどうにか宥めた。",
                            "A curse was about to activate, but you calm the item down."));
				return;
			}

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sはあなたの体力を吸収した！", o_name);
#else
			msg_format("Your %s drains HP from you!", o_name);
#endif
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev*2, 100), o_name, -1);
		}
		/* Handle mana draining */
		if ((p_ptr->cursed & TRC_DRAIN_MANA) && p_ptr->csp && one_in_(666))
		{
			char o_name[MAX_NLEN];
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				hina_gain_yaku(25 + randint1(25));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("装備品の呪いが発動したが、あなたはそれを吸い取った。",
                            "A curse on your equipment activates, but you absorb it."));
				p_ptr->magic_num1[1] += 500 + randint1(500);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format(_("呪いが発動しかけたがどうにか宥めた。",
                            "A curse was about to activate, but you calm the item down."));
				return;
			}

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%sはあなたの魔力を吸収した！", o_name);
#else
			msg_format("Your %s drains mana from you!", o_name);
#endif
			p_ptr->csp -= MIN(p_ptr->lev, 50);
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= PR_MANA;
		}
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if (one_in_(999) && !p_ptr->anti_magic)
	{
		object_type *o_ptr;

		o_ptr = &inventory[INVEN_LITE];
		if (o_ptr->name1 == ART_JUDGE)
		{

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("『審判の宝石』があなたの体力を吸収しようとしたが、逆に厄を吸い取った。",
                            "The Jewel of Judgment tries to drain your life force, but you absorb the curse in return."));
				hina_gain_yaku(200 + randint1(200));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format(_("『審判の宝石』があなたの体力を吸収しようとしたが、逆に不運パワーを吸い取った。",
                            "The Jewel of Judgment tries to drain your life force, but you absorb the misfortune power in return."));
				p_ptr->magic_num1[1] += 2000 + randint1(2000);
				return;
			}

			if (object_is_known(o_ptr))
				msg_print(_("『審判の宝石』はあなたの体力を吸収した！",
                            "The Jewel of Judgment drains HP from you!"));
			else
				msg_print(_("なにかがあなたの体力を吸収した！",
                            "Something drains HP from you!"));
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev, 50), _("審判の宝石", "the Jewel of Judgment"), -1);

		}

	}

	//v1.1.48 貧乏神エゴ 三妖精は違うスロットに装備できるのでペナルティなしだがまあ問題なかろう
	if (inventory[INVEN_BODY].name2 == EGO_BODY_POVERTY && one_in_(100))
	{
		int lose_money = p_ptr->au / 20 + randint0(p_ptr->au / 20) + 1;

		//v1.1.48 紫苑は呪いを不運パワーとして吸収する
		if (p_ptr->pclass == CLASS_SHION)
		{
			p_ptr->magic_num1[1] += 1000 + randint1(1000);
			return;
		}

		if (p_ptr->au < 1) return;

		//財布をすられない能力持ちの妖精は防御する
		if (CHECK_FAIRY_TYPE != 9)
		{
			p_ptr->au -= lose_money;
			p_ptr->redraw |= PR_GOLD;
			msg_print(_("財布が軽くなった気がする。", "You wallet feels lighter."));
			disturb(0, 1);
		}
	}

	/*:::月の羽衣に記憶喪失効果*/
	if (one_in_(999) && !p_ptr->anti_magic)
	{
		if ( check_equip_specific_fixed_art(ART_HAGOROMO_MOON,TRUE) && !p_ptr->resist_insanity)
		{
			msg_print(_("月の羽衣がかすかに光った気がする・・", "The Hagoromo of the Moon glows faintly..."));
			if (lose_all_info()) msg_print(_("急に記憶が曖昧になった。", "Your memories get vague and unclear all of the sudden."));
		}
	}
	//永遠亭の近くでは道に迷う
	if(!dun_level && !p_ptr->wild_mode && distance(p_ptr->wilderness_y,p_ptr->wilderness_x,55,46) < 6 && !p_ptr->town_num && one_in_(10) && !p_ptr->levitation && randint0(p_ptr->skill_srh) < 10)
	{
		teleport_player(100, TELEPORT_NONMAGICAL);
		msg_print(_("..道に迷った気がする。", "...You feel you've strayed from the path."));

	}
	//雛ゲームオーバー
	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] >= HINA_YAKU_MAX)
	{
		msg_format(_("溜まりに溜まった厄が大爆発を起こした！",
                    "All the misfortune you've accumulated causes a massive explosion!"));
		project_hack2(GF_DISINTEGRATE,0,0,9999);
		take_hit(DAMAGE_NOESCAPE,9999,_("厄の大爆発", "massive misfortune explosion"),-1);
	}

}

///mod140330 泥酔度による処理
//v1.1.19 泥酔度マジックナンバー排除
void process_world_aux_alcohol(void)
{
	int freq;
	int eccentricity;

	/*:::アル中でないとき泥酔度最大段階なら何もしない（意識不明瞭で倒れているため）*/
	if(!(p_ptr->muta2 & MUT2_ALCOHOL) && p_ptr->alcohol >= DRANK_4)return;
	//ワールドマップでは何も起こらない
	if(p_ptr->wild_mode) return;
	if (p_ptr->inside_battle) return;

	if(p_ptr->alcohol < DRANK_2) return;
	else if(p_ptr->alcohol < DRANK_3) freq = 100;
	else if(p_ptr->alcohol < DRANK_4) freq = 20;
	else freq = 10;

	if(!one_in_(freq)) return;
	/*:::これ以降、泥酔による何らかの効果発生*/

	if(p_ptr->alcohol < DRANK_3) eccentricity = randint0(50);
	else eccentricity = randint0(90);

	if(eccentricity < 10)//祝福
	{
		msg_format(_("何だかとても幸せな気分だ！", "You feel very happy!"));
		set_blessed(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 15)//減速
	{
		msg_format(_("何だか体が重い・・", "Your body feels heavy..."));
		set_slow(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 20)//混乱
	{
		msg_format(_("何だか・・何だか・・何だっけ？", "Wha... huh... what?"));
		if(!p_ptr->resist_conf) set_confused(randint1(25) + 25);
	}
	else if(eccentricity < 25)//加速
	{
		msg_format(_("何だか体が軽い！", "Your body feels light!"));
		set_fast(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 30)//騒音
	{
		msg_format(_("何だか大声で歌い出したくなった！", "You sing out loud!"));
		aggravate_monsters(0,FALSE);
	}
	else if(eccentricity < 35)//朦朧
	{
		msg_format(_("何だか頭がぼんやりする・・", "Your head is hazy..."));
		set_stun(p_ptr->stun + 10 + randint0(21));
	}
	else if(eccentricity < 40)//狂戦士化
	{
		msg_format(_("何だか無性に人を斬りたくなった・・", "You have an urge to cut someone..."));
			set_shero(randint1(25) + 25, FALSE);
			hp_player(30);
			set_afraid(0);
	}
	else if(eccentricity < 45)//切り傷
	{
		msg_format(_("何だか知らないうちに怪我をしている・・", "You get injured for some unkown reason..."));
		take_hit(DAMAGE_NOESCAPE,randint1(10),_("酔っ払って負った怪我", "injury received while drunk"),-1);
		set_cut(p_ptr->cut + randint1(10));
	}
	else if(eccentricity < 50)//記憶喪失＆テレポート
	{
		if(lose_all_info())
		{
			msg_format(_("何だか記憶が曖昧だ。", "Your memories get hazy."));

			if(one_in_(3))
			{
				msg_print(_("・・というかここはどこだ？", "...Where am I?"));
				teleport_player(10 + randint1(200), TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
			}
		}
	}
	/*:::以下、DRANK_3以降のみ*/
	else if(eccentricity < 55)//胃の掃除
	{
		msg_format(_("何だか・・", "Uhh..."));
		if (!throw_up())
		{
			msg_format(_("吐きそうになったが抑えた。", "You had an urge to vomit, but you held back."));

		}
	}
	else if(eccentricity < 60)//回復
	{
		if(p_ptr->chp < p_ptr->mhp)
		{
			msg_format(_("何だか怪我の痛みを感じない気がする。", "You don't feel pain from your injuries."));
			hp_player(p_ptr->lev * 4);
		}
	}

	else if(eccentricity < 65)//幻覚
	{
		if(!p_ptr->resist_pois && !p_ptr->resist_chaos)
		{
			msg_format(_("何だか周りの景色が踊って見える・・", "Everything around you seemingly starts to dance..."));
			if(!p_ptr->resist_chaos) set_image(20 + randint0(81));
		}
	}

	else if(eccentricity < 70)//装備解除
	{
		int slot = INVEN_RIBBON + randint0(10);

		if(inventory[slot].k_idx )
		{
			if(!wield_check(slot,0,0)) msg_format(_("何だか脱ぎたくなったが自重した。", "You get an urge to disrobe, but you hold back."));
			else if( object_is_cursed(&inventory[slot]) || check_invalidate_inventory(slot) ) msg_format(_("何だか脱ぎたくなったが脱げなかった。",
                                                                                                            "You get an urge to disrobe, but you fail to do so."));
			else
			{
				msg_format(_("何だか脱ぎたくなった。", "You get an urge to disrobe."));
				inven_takeoff(slot,1);
			}
		}
	}
	else if(eccentricity < 75)//腕耐限界突破
	{
		int time = 25 + randint0(25);
		msg_format(_("何だか肉体の限界をどこかに置き忘れた気がする！",
                    "You feel like you've forgotten the limits of your body!"));
		set_tim_addstat(A_STR,100 + 10,time,FALSE);
		set_tim_addstat(A_CON,100 + 10,time,FALSE);
	}
	else if(eccentricity < 80)//加速+飛翔+器用限界突破
	{
		int time = 25 + randint0(25);
		msg_format(_("何だかものすごく体が軽い！", "Your body feels extremely light!"));
		set_fast(time,FALSE);
		set_tim_levitation(time,FALSE);
		set_tim_addstat(A_DEX,100 + 10,time,FALSE);
		//set_tim_speedstar(time,FALSE);

	}
	else if(eccentricity < 85)//現実変容（即時）
	{
		msg_format(_("何だか長い夢を見ていた気がする。", "You feel like you've been seeing a long dream."));
		if (p_ptr->inside_arena || ironman_downward)
			msg_print(_("いや、気のせいか？", "No, that was just your imagination?"));
		else
		{
			/*:::0ではまずいはず*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
	}
	else //無敵化
	{
		msg_format(_("何だか今ならなんでもできる気がする！", "You feel like you can do anything right now!"));
		(void)set_invuln(randint1(8) + 8, FALSE);
	}

}


/*
 * Handle recharging objects once every 10 game turns
 */
/*:::ロッドなどの自動充填*/
static void process_world_aux_recharge(void)
{
	int i;
	bool changed;

	int hiziri_card_num;
	hiziri_card_num = count_ability_card(ABL_CARD_KYOUTEN);

	/* Process equipment */
	for (changed = FALSE, i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		///mod160508 銃の充填はここでは行わない
		if(o_ptr->tval == TV_GUN) continue;
		if(check_invalidate_inventory(i)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			//v1.1.86 「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				if (randint1(100) <= CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num)) o_ptr->timeout--;
			}
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}

	/*
	 * Recharge rods.  Rods now use timeout to control charging status,
	 * and each charging rod in a stack decreases the stack's timeout by
	 * one per turn. -LM-
	 */
	for (changed = FALSE, i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Determine how many rods are charging. */
			int temp = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
			if (temp > o_ptr->number) temp = o_ptr->number;


			//v1.1.86 「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//増加率

				//基本充填量*増加率の整数部分はそのまま加算
				int bonus = temp * prob / 100;
				//基本充填量*増加率の余り部分はそれを%値として判定通れば加算
				if (randint1(100) <= (temp*prob) % 100) bonus++;

				temp += bonus;
			}

			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if (o_ptr->timeout % k_ptr->pval)
			{
				changed = TRUE;
			}
		}

		//v1.1.86 アビリティカードの充填
		if ((o_ptr->tval == TV_ABILITY_CARD) && (o_ptr->timeout))
		{
			int base_charge_time,temp;

			if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
			{
				msg_format(_("ERROR:不正なカードidx(%d)を持ったアビリティカードが充填処理された",
                            "ERROR: Recharging logic called for an ability card with an incorrect card idx (%d)"), o_ptr->pval);
				continue;
			}
			base_charge_time = ability_card_list[o_ptr->pval].charge_turn;//一枚あたりの基本チャージ時間

			if (!base_charge_time) { msg_format(" ERROR:card_charge(%d)", o_ptr->pval); continue; }

			//基本チャージ時間が0だったら即刻チャージを終了させて次へ。
			//普通こんな事は起こらないが、もし発動型カードを所有型カードに仕様変更したときチャージ中のカードを持ってたらここに来る
			if (!base_charge_time)
			{
				p_ptr->window |= (PW_INVEN);
				o_ptr->timeout = 0;
				continue;
			}

			//例えば3本中1本だけが充填中のとき、3本とも充填中のときに比べてタイムアウト値の減り方が1/3になる処理か。
			temp = (o_ptr->timeout + (base_charge_time - 1)) / base_charge_time;
			if (temp > o_ptr->number) temp = o_ptr->number;

			//「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//増加率
				//基本充填量*増加率の整数部分はそのまま加算
				int bonus = temp * prob / 100;
				//基本充填量*増加率の余り部分はそれを%値として判定通れば加算
				if (randint1(100) <= (temp*prob) % 100) bonus++;

				temp += bonus;
			}


			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if (o_ptr->timeout % base_charge_time)
			{
				changed = TRUE;
			}
		}


	}


	//v2.0.7 千亦がmagic_num[]に記録した保有カードの充填
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		for (i = 0; i < ABILITY_CARD_LIST_LEN; i++)
		{
			int base_charge_time, temp;

			//千亦はチャージ値をmagic_num1[card_idx]に記録している
			if (!p_ptr->magic_num1[i]) continue;

			base_charge_time = ability_card_list[i].charge_turn;//一枚あたりの基本チャージ時間

			if (!base_charge_time) { msg_format(" ERROR:card_charge(%d)", i); continue; }

			//例えば3本中1本だけが充填中のとき、3本とも充填中のときに比べてタイムアウト値の減り方が1/3になる処理
			//カードの枚数は流通ランクに等しい
			temp = (p_ptr->magic_num1[i] + (base_charge_time - 1)) / base_charge_time;
			if (temp > CHIMATA_CARD_RANK) temp = CHIMATA_CARD_RANK;

			//「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//増加率
																			//基本充填量*増加率の整数部分はそのまま加算
				int bonus = temp * prob / 100;
				//基本充填量*増加率の余り部分はそれを%値として判定通れば加算
				if (randint1(100) <= (temp*prob) % 100) bonus++;

				temp += bonus;
			}


			/* Decrease timeout by that number. */
			p_ptr->magic_num1[i] -= temp;

			/* Boundary control. */
			if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;

			//アイテムではないので通知処理は行わない

		}

	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		wild_regen = 20;
	}


	/* Process objects on floor */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge rods on the ground.  No messages. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Charge it */
			o_ptr->timeout -= o_ptr->number;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;
		}
	}

	//v1.1.86 床上のアビリティカードは充填されない


	//v1.1.46 女苑が専用スロットに装備している指輪の充填処理を行う
	if (p_ptr->pclass == CLASS_JYOON)	for (changed = FALSE, i = 0; i < INVEN_ADD_MAX; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inven_add[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval == TV_GUN) continue;

		//指輪スロットが無効化されているとき充填しない。EXTRAモードの狸の葉っぱで変身してるときだけ関係するはず。
		//本来右手指と左手指で分けるべきなのだろうがそんな変身ないしもう一緒くたにしておく。
		if (check_invalidate_inventory(INVEN_RIGHT)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			//v1.1.86 「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				if (randint1(100) <= CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num)) o_ptr->timeout--;
			}
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;


			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
		}
	}
}

///mod160509
/*:::銃の装填 recharge()の十倍の頻度で呼び出される。
 *timeout値には通常の千倍の値が格納されており、基本100減少させる。
 *銃熟練度などに応じて減少値が増加する。*/
static void process_world_aux_recharge_gun(void)
{
	int i;
	bool changed = FALSE;

	int hiziri_card_num;
	hiziri_card_num = count_ability_card(ABL_CARD_KYOUTEN);


	//行動不能時にはチャージされない
	if(p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100)
		|| p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL)) return;

	if (p_ptr->special_defense & KATA_MASK) return;

	for (i = 0; i <= INVEN_LARM; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inventory[i];

		//技能「メンテナンス」がない限り装備中の銃にしかチャージされない
		if(!HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_MAINTENANCE) && i < INVEN_RARM) continue;


		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval != TV_GUN) continue;
		if(check_invalidate_inventory(i)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			int ammo_old,ammo_new;
			int charge = 100;
			int timeout_base = calc_gun_timeout(o_ptr) * 1000;

			ammo_old = o_ptr->timeout / timeout_base;

			charge = charge * 100 / calc_gun_charge_mult(i);

			//メンテナンスがあっても装備中でない銃のチャージ速度は半分
			if(i < INVEN_RARM) charge /= 2;

			//v1.1.86「法力経典」カードによる充填ブースト
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//増加率
				//基本充填量*増加率の整数部分はそのまま加算
				int bonus = charge * prob / 100;
				//基本充填量*増加率の余り部分はそれを%値として判定通れば加算
				if (randint1(100) <= (charge*prob) % 100) bonus++;

				charge += bonus;
			}


			o_ptr->timeout -= charge;

			if(o_ptr->timeout < 0) o_ptr->timeout = 0;

			ammo_new = o_ptr->timeout / timeout_base;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
			else if(ammo_old != ammo_new)
			{
				changed = TRUE;
				//v1.1.21 ガンカタは残弾ありのときしかやらなくしたのでここでチェック
				if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN)
					p_ptr->update |= PU_BONUS;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP | PW_INVEN);
		wild_regen = 20;
	}

}

/*
 * Handle involuntary movement once every 10 game turns
 */
/*:::その他時間効果*/
static void process_world_aux_movement(void)
{
	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{

		//v1.1.68 すでにp_ptr->leavingがTRUEのとき、同じタイミングで帰還が発動しないようにカウントをひとつ遅らせる。
		//ここにはturn値が10の倍数のときにしか来ないので、turnが10の倍数の時に＠の行動順が来て階移動をしてそれが帰還の発動と重なったときにのみ顕在化する不具合だったらしい。
		if (p_ptr->leaving && p_ptr->word_recall == 1)
		{
			if (cheat_xtra) msg_format("recall:%d(+1)",p_ptr->word_recall);
			p_ptr->word_recall++;
		}

		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (p_ptr->word_recall == 1) && !p_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		p_ptr->word_recall--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!p_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 1);


			//月都万象展のトラップ発動
			if(p_ptr->inside_quest == QUEST_MOON_VAULT && quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
			{
				msg_print(_("地上の密室の術が帰還の術を打ち消した！",
                            "Your recall is countered by a stronger spell!"));
			}

			/* Determine the level */
			else if (dun_level || p_ptr->inside_quest || p_ptr->enter_dungeon)
			{
				msg_print(_("上に引っ張りあげられる感じがする！",
							"You feel yourself yanked upwards!"));

				if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				dun_level = 0;
				dungeon_type = 0;

				leave_quest_check();
				leave_tower_check();

				p_ptr->inside_quest = 0;

				p_ptr->leaving = TRUE;
			}
			else
			{
				msg_print(_("下に引きずり降ろされる感じがする！",
							"You feel yourself yanked downwards!"));

				dungeon_type = p_ptr->recall_dungeon;

				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);
				///system ここ触れば好きな階層に帰還できそう
				/* New depth */
				dun_level = max_dlv[dungeon_type];
				if (dun_level < 1) dun_level = 1;

				/* Nightmare mode makes recall more dangerous */
				if (ironman_nightmare && !randint0(666) && (dungeon_type == DUNGEON_ANGBAND))
				{
					if (dun_level < 50)
					{
						dun_level *= 2;
					}
					else if (dun_level < 99)
					{
						dun_level = (dun_level + 99) / 2;
					}
					else if (dun_level > 100)
					{
						dun_level = d_info[dungeon_type].maxdepth - 1;
					}
				}

				if (p_ptr->wild_mode)
				{
					p_ptr->wilderness_y = py;
					p_ptr->wilderness_x = px;
				}
				else
				{
					/* Save player position */
					p_ptr->oldpx = px;
					p_ptr->oldpy = py;
				}
				p_ptr->wild_mode = FALSE;

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				p_ptr->leaving = TRUE;

				if (dungeon_type == DUNGEON_ANGBAND)
				{
					int i;

					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if ((quest[i].type == QUEST_TYPE_RANDOM) &&
						    (quest[i].status == QUEST_STATUS_TAKEN) &&
						    (quest[i].level < dun_level))
						{
							quest[i].status = QUEST_STATUS_FAILED;
							quest[i].complev = (byte)p_ptr->lev;
							r_info[quest[i].r_idx].flags1 &= ~(RF1_QUESTOR);
						}
					}
				}
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}


	/* Delayed Alter reality */
	if (p_ptr->alter_reality)
	{

		//v1.1.68 すでにp_ptr->leavingがTRUEのとき、同じタイミングで現実変容が発動しないようにカウントをひとつ遅らせる。少し上の帰還の処理と同様
		if (p_ptr->leaving && p_ptr->alter_reality == 1)
		{
			if (cheat_xtra) msg_format("alter_reality:%d(+1)", p_ptr->alter_reality);
			p_ptr->alter_reality++;
		}

		if (autosave_l && (p_ptr->alter_reality == 1) && !p_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards alter */
		p_ptr->alter_reality--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the alter reality */
		if (!p_ptr->alter_reality)
		{
			/* Disturbing! */
			disturb(0, 1);

			/* Determine the level */
			if (!quest_number(dun_level) && dun_level)
			{
#ifdef JP
				msg_print("世界が変わった！");
#else
				msg_print("The world changes!");
#endif

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("世界が少しの間変化したようだ。");
#else
				msg_print("The world seems to change for a moment!");
#endif
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}


/*
 * Count number of adjacent monsters
 */
static int get_monster_crowd_number(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	int my = m_ptr->fy;
	int mx = m_ptr->fx;
	int i;
	int count = 0;

	for (i = 0; i < 7; i++)
	{
		int ay = my + ddy_ddd[i];
		int ax = mx + ddx_ddd[i];

		if (!in_bounds(ay, ax)) continue;

		/* Count number of monsters */
		if (cave[ay][ax].m_idx > 0) count++;
 	}

	return count;
}



/*
 * Dungeon rating is no longer linear
 */
#define RATING_BOOST(delta) (delta * delta + 50 * delta)

/*
 * Examine all monsters and unidentified objects,
 * and get the feeling of current dungeon floor
 */
/*:::雰囲気を計算する。このフロアのモンスター数と触ってないアイテムを階レベルと比較*/
static byte get_dungeon_feeling(void)
{
	const int base = 10;
	int rating = 0;
	int i;

	/* Hack -- no feeling in the town */
	if (!dun_level) return 0;

	/* Examine each monster */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;
		int delta = 0;

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Ignore pet */
		if (is_pet(m_ptr)) continue;

		if(m_ptr->r_idx == MON_MASTER_KAGUYA) continue;

		r_ptr = &r_info[m_ptr->r_idx];

		/* Unique monsters */
		///mod150224 UNIQUE2も含むことにした
		if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_UNIQUE2))
		{
			/* Nearly out-of-depth unique monsters */
			if (r_ptr->level + 10 > dun_level)
			{
				/* Boost rating by twice delta-depth */
				delta += (r_ptr->level + 10 - dun_level) * 2 * base;
			}
		}
		else
		{
			/* Out-of-depth monsters */
			if (r_ptr->level > dun_level)
			{
				/* Boost rating by delta-depth */
				delta += (r_ptr->level - dun_level) * base;
			}
		}

		/* Unusually crowded monsters get a little bit of rating boost */
		if (r_ptr->flags1 & RF1_FRIENDS)
		{
			if (5 <= get_monster_crowd_number(i)) delta += 1;
		}
		else
		{
			if (2 <= get_monster_crowd_number(i)) delta += 1;
		}


		rating += RATING_BOOST(delta);
	}

	/* Examine each unidentified object */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];
		int delta = 0;

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip known objects */
		if (object_is_known(o_ptr))
		{
			/* Touched? */
			if (o_ptr->marked & OM_TOUCHED) continue;
		}

		/* Skip pseudo-known objects */
		if (o_ptr->ident & IDENT_SENSE) continue;

		///mod160314 アイテムカード　最低生成階とコストで適当に計算
		if(o_ptr->tval == TV_ITEMCARD)
		{
			int fake_lev = (support_item_list[o_ptr->pval].min_lev + support_item_list[o_ptr->pval].cost * 4) / 2;

			if(fake_lev > dun_level) delta += (fake_lev - dun_level) * base;

		}

		/* Ego objects */
		if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			delta += e_ptr->rating * base;
		}

		/* Artifacts */
		if (object_is_artifact(o_ptr))
		{
			s32b cost = object_value_real(o_ptr);

			delta += 10 * base;
			if (cost > 10000L) delta += 10 * base;
			if (cost > 50000L) delta += 10 * base;
			if (cost > 100000L) delta += 10 * base;

			/* Special feeling */
			if (!preserve_mode) return 1;
		}
		///item 雰囲気計算
		///mod131223 雰囲気計算 tval
		if (o_ptr->tval == TV_DRAG_ARMOR) delta += 30 * base;
		if (o_ptr->tval == TV_DRAG_ARMOR && o_ptr->sval >=  SV_DRAGARMOR_MULTI) delta += 30 * base;

		if (o_ptr->tval == TV_SHIELD &&
		    o_ptr->sval == SV_DRAGON_SHIELD) delta += 5 * base;
		if (o_ptr->tval == TV_GLOVES &&
		    o_ptr->sval == SV_HAND_DRAGONGLOVES) delta += 5 * base;
		if (o_ptr->tval == TV_BOOTS &&
		    o_ptr->sval == SV_LEG_DRAGONBOOTS) delta += 5 * base;
		if (o_ptr->tval == TV_HEAD &&
		    o_ptr->sval == SV_HEAD_DRAGONHELM) delta += 5 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_SPEED &&
		    !object_is_cursed(o_ptr)) delta += 25 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_LORDLY &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;
		if (o_ptr->tval == TV_AMULET &&
		    o_ptr->sval == SV_AMULET_THE_MAGI &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;

		/* Out-of-depth objects */
		if (!object_is_cursed(o_ptr) && !object_is_broken(o_ptr) &&
		    k_ptr->level > dun_level)
		{
			/* Rating increase */
			delta += (k_ptr->level - dun_level) * base;
		}

		rating += RATING_BOOST(delta);
	}


	if (rating > RATING_BOOST(1000)) return 2;
	if (rating > RATING_BOOST(800)) return 3;
	if (rating > RATING_BOOST(600)) return 4;
	if (rating > RATING_BOOST(400)) return 5;
	if (rating > RATING_BOOST(300)) return 6;
	if (rating > RATING_BOOST(200)) return 7;
	if (rating > RATING_BOOST(100)) return 8;
	if (rating > RATING_BOOST(0)) return 9;

	return 10;
}


/*
 * Update dungeon feeling, and announce it if changed
 */
/*:::雰囲気感知判定と処理 1turnごとに呼ばれる*/
//v1.1.59 staticを外し、強制感知フラグ追加
void update_dungeon_feeling(bool flag_feel_now)
{
	byte new_feeling;
	int quest_num;
	int delay;

	/* No feeling on the surface */
	if (!dun_level) return;

	/* No feeling in the arena */
	if (p_ptr->inside_battle) return;

	/* Extract delay time */
	//v2.1.2 参照するパラメータを「知覚(fos)」から「探索(srh)」に変更
	if (p_ptr->pclass == CLASS_IKU) //衣玖は最速で感知
		delay = 10 * 30 * TURNS_PER_TICK / 100;
	else if (p_ptr->pclass == CLASS_CHIMI) //チミも感知が早め
		delay = MAX(10, 120 - p_ptr->skill_srh) * (150 - dun_level) * TURNS_PER_TICK / 100;
	else
		delay = MAX(10, 150 - p_ptr->skill_srh) * (150 - dun_level) * TURNS_PER_TICK / 100;

 	/* Not yet felt anything */
	//if (turn < p_ptr->feeling_turn + delay && !cheat_xtra) return;
	///mod140216 衣玖は一瞬で雰囲気を感知する
	///mod140907 ・・予定だったが、どうも挙動がおかしくなるので一瞬でなく最速にした
	//if (turn < (p_ptr->feeling_turn + delay) && !cheat_xtra && p_ptr->pclass != CLASS_IKU) return;
	if (!flag_feel_now && turn < (p_ptr->feeling_turn + delay) && !cheat_xtra) return;

	/* Extract quest number (if any) */
	quest_num = quest_number(dun_level);
	///system クエスト関連
	/* No feeling in a quest */
	if (quest_num &&
	    (is_fixed_quest_idx(quest_num) &&
	  //   !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_SERPENT) ||
	     !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_YUKARI) ||
	       !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) return;

	if(EXTRA_QUEST_FLOOR) return;

	/* Get new dungeon feeling */
	new_feeling = get_dungeon_feeling();

	/* Remember last time updated */
	p_ptr->feeling_turn = turn;

	/* No change */
	if (p_ptr->feeling == new_feeling) return;

	/* Dungeon feeling is changed */
	p_ptr->feeling = new_feeling;

	/* Announce feeling */
	do_cmd_feeling();

	select_floor_music(); //v1.1.58

	/* Update the level indicator */
	p_ptr->redraw |= (PR_DEPTH);

	/* Disturb */
	if (disturb_minor) disturb(0, 0);
}


/*
 * Handle certain things once every 10 game turns
 */
/*:::1ゲームターンごと、10ゲームターンごとに行われる処理*/
static void process_world(void)
{
	int day, hour, min;

	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;
	s32b prev_turn_in_today = ((turn - TURNS_PER_TICK) % A_DAY + A_DAY / 4) % A_DAY;
	int prev_min = (1440 * prev_turn_in_today / A_DAY) % 60;

	extract_day_hour_min(&day, &hour, &min);

	/* Update dungeon feeling, and announce it if changed */
	update_dungeon_feeling(FALSE);

	///mod160508 銃の充填処理
	process_world_aux_recharge_gun();

	///mod160814 サグメ「神々の弾冠」消滅判定
	if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0] && p_ptr->csp < 1)
	{
		set_tim_general(0,TRUE,0,0);
	}

	///mod150616 闘技場変更
	if (p_ptr->inside_battle && !p_ptr->leaving)
	{
		int i;
		int win_m_idx = 0;
		int number_mon[4] = {0,0,0,0};
		int number_team = 0;
		u32b bitflag_survive = 0L;

		/* Examine each monster */
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if(!m_ptr->r_idx) continue;
			if(i == p_ptr->riding) continue;

			if(!(m_ptr->mflag & MFLAG_BATTLE_MON_MASK))
				msg_print(_("WARNING:闘技場にどのチームにも属さず乗馬中でもないモンスターがいる",
                            "WARNING: There are monsters in the arena that don't belong to any team and aren't mounted"));
			else
				bitflag_survive |= (MFLAG_BATTLE_MON_MASK & m_ptr->mflag);

			win_m_idx = i;
		}


		if (!bitflag_survive)
		{
			p_ptr->au -= battle_mon_wager;
			gambling_gain -= battle_mon_wager;
			msg_print(_("「あー、こりゃ相討ちだね。悪いけど胴元の総取りさ。」",
                        "'Ah, they took each other out. I'm sorry, but you've lost your wager.'"));
			msg_print(NULL);
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else if (count_bits(bitflag_survive) == 1)
		{

			char m_name[80];
			monster_type *wm_ptr;

			p_ptr->au -= battle_mon_wager;
			gambling_gain -= battle_mon_wager;

			if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_A) i = 0;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_B) i = 1;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_C) i = 2;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_D) i = 3;
			if(battle_mon_team_type[i] == 0)
			{
				wm_ptr = &m_list[win_m_idx];
				monster_desc(m_name, wm_ptr, 0);
				msg_format(_("%sが勝利した！", "%^s wins!"), m_name);
			}
			else if(battle_mon_team_type[i] < 0)
			{
				msg_format(_("%sが勝利した！", "%s wins!"), battle_mon_special_team_list[0-battle_mon_team_type[i]].name);
			}
			else
			{
				msg_format(_("%sが勝利した！", "%s wins!"), battle_mon_random_team_name_list[battle_mon_team_type[i]]);
			}
			msg_print(NULL);


			if(battle_mon_team_bet & bitflag_survive )
			{
				int reward;

				reward = battle_mon_wager / 100 * battle_mon_odds;
				if(reward > 9999999) reward = 9999999;

				if(battle_mon_odds < 1000 && p_ptr->psex == SEX_MALE) msg_print(_("「おめでとう！兄さんの勝ちだ！」", "'Congratulations, you win!'"));
				else if(battle_mon_odds < 1000 ) msg_print(_("「おめでとう！姐さんの勝ちだ！」", "'Congratulations, you win!'"));
				else if(reward < 1000000 )
				{
					//大穴を当てると秩序度大幅マイナス、名声大幅プラス
					msg_print(_("「すごいじゃないか、大穴だよ！」", "'Amazing, that's quite a sum!'"));
					set_deity_bias(DBIAS_REPUTATION, 3);
					set_deity_bias(DBIAS_COSMOS, -5);
				}
				else if(reward < 9999999)
				{
					msg_print(_("「どうしよう、山の神様から預かったダム工事費が・・」",
                                "'What should I do, building that dam for the god from the mountain will cost...'"));
					set_deity_bias(DBIAS_REPUTATION, 10);
					set_deity_bias(DBIAS_COSMOS, -30);
				}
				else
				{
					msg_print(_("雀斑の河童は泡を吹いて倒れ、ほかの河童が慌てて介抱を始めた。",
                                "The freckled kappa girl faints and collapses on the floor, and another kappa rushes to her side."));
					set_deity_bias(DBIAS_REPUTATION, 100);
					set_deity_bias(DBIAS_COSMOS, -200);
				}
				msg_print(NULL);
				msg_format(_("%d＄を受け取った。", "You receive $%d."), reward);
				p_ptr->au += reward;

				//v1.1.25 ギャンブル収支をカウントすることにした
				gambling_gain += reward;
				if(gambling_gain > 99999999) gambling_gain = 99999999;

			}
			else
			{
				msg_print(_("「残念だったね。まあ懲りずにまた来てよ。」",
                            "'That was unfortunate. Don't hesitate to try again.'"));
			}
			msg_print(NULL);
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else if (turn - old_turn == 300 * TURNS_PER_TICK)
		{
			msg_print(_("「タイムアップだ。残念だけど返金するよ。」",
                        "'Time's up! Sorry, but here's your money back.'"));
			msg_print(NULL);
			p_ptr->energy_need = 0;
		}
		//それ以外のとき、まだ勝負がついてない

		//v1.1.96 闘技場で「応答なし」にならないように追加　たぶんこの関数でいいはず
		if (turn % 10 == 0) Term_flush();

	}

	/*** Check monster arena ***/

//闘技場の元のルーチン
#if 0
	if (p_ptr->inside_battle && !p_ptr->leaving)
	{
		int i2, j2;
		int win_m_idx = 0;
		int number_mon = 0;

		/* Count all hostile monsters */
		for (i2 = 0; i2 < cur_wid; ++i2)
			for (j2 = 0; j2 < cur_hgt; j2++)
			{
				cave_type *c_ptr = &cave[j2][i2];

				if ((c_ptr->m_idx > 0) && (c_ptr->m_idx != p_ptr->riding))
				{
					number_mon++;
					win_m_idx = c_ptr->m_idx;
				}
			}

		if (number_mon == 0)
		{
#ifdef JP
			msg_print("相打ちに終わりました。");
#else
			msg_print("They have kill each other at the same time.");
#endif
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if ((number_mon-1) == 0)
		{
			char m_name[80];
			monster_type *wm_ptr;

			wm_ptr = &m_list[win_m_idx];

			monster_desc(m_name, wm_ptr, 0);
#ifdef JP
			msg_format("%sが勝利した！", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if (win_m_idx == (sel_monster+1))
			{
#ifdef JP
				msg_print("おめでとうございます。");
#else
				msg_print("Congratulations.");
#endif
#ifdef JP
				msg_format("%d＄を受け取った。", battle_odds);
#else
				msg_format("You received %d gold.", battle_odds);
#endif
				p_ptr->au += battle_odds;
			}
			else
			{
#ifdef JP
				msg_print("残念でした。");
#else
				msg_print("You lost gold.");
#endif
			}
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if (turn - old_turn == 150*TURNS_PER_TICK)
		{
#ifdef JP
			msg_print("申し分けありませんが、この勝負は引き分けとさせていただきます。");
#else
			msg_format("This battle have ended in a draw.");
#endif
			p_ptr->au += kakekin;
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
	}
#endif



	//v2.0.1 「生命爆発の薬」のカードで爆発が起こるとき
	if (flag_life_explode && !p_ptr->is_dead)
	{
		flag_life_explode = FALSE;
		project(0, 5, py, px, p_ptr->csp, GF_FORCE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
	}




	/* Every 10 game turns */
	if (turn % TURNS_PER_TICK) return;
	/*:::ここから先、内部的10turn(ゲーム上で1ターン)ごとの処理*/

	//v2.0.6 尤魔が食事で得た特技や耐性が消えるカウントダウン
	if (p_ptr->pclass == CLASS_YUMA && turn % YUMA_FLAG_DELETE_TICK == 0) yuma_lose_extra_power(1);

	/*** Check the Time and Load ***/
	///system アングバンドへの門・・・？
	//ずっと昔に大学のPCで遊んでいる人を締め出すために実装された機能らしい
#if 0
	if (!(turn % (50*TURNS_PER_TICK)))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 1);

				/* Count warnings */
				closing_flag++;

				/* Message */
#ifdef JP
msg_print("アングバンドへの門が閉じかかっています...");
msg_print("ゲームを終了するかセーブするかして下さい。");
#else
				msg_print("The gates to ANGBAND are closing...");
				msg_print("Please finish up and/or save your game.");
#endif

			}

			/* Slam the gate */
			else
			{
				/* Message */
#ifdef JP
msg_print("今、アングバンドへの門が閉ざされました。");
#else
				msg_print("The gates to ANGBAND are now closed.");
#endif


				/* Stop playing */
				p_ptr->playing = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
	}
#endif

	/*** Attempt timed autosave ***/
	/*:::オートセーブ*/
	if (autosave_t && autosave_freq && !p_ptr->inside_battle)
	{
		if (!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	if (mon_fight && !ignore_unview)
	{
#ifdef JP
		msg_print("何かが聞こえた。");
#else
		msg_print("You hear noise.");
#endif
	}
	//永琳プレイ中に輝夜が視界外でダメージ受けたときkaguya_fightに残りHP割合が記録される
	if(kaguya_fight && !ignore_unview && kaguya_fight < 100)
	{

		if(kaguya_fight > 80)
			msg_print(_("輝夜が交戦中のようだ。", "Looks like Kaguya is fighting."));
		else if(kaguya_fight > 50)
			msg_print(_("輝夜は苦戦しているようだ。", "Looks like Kaguya is struggling."));
		else
			msg_print(_("輝夜が危険だ！", "Kaguya is in danger!"));

		if(kaguya_fight <= 80)disturb(0,1);


		kaguya_fight = 100;

	}

	/*** Handle the wilderness/town (sunshine) ***/

	///mod140821 エンジニアギルドの消耗品の値段が変わる
	if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2))) engineer_guild_price_change();

	///mod160522 鈴瑚の支援物資投下カウンタ　およそ15分ごとに
	if(p_ptr->pclass == CLASS_RINGO && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 1000)))
	{
		p_ptr->magic_num1[4] += dun_level / 10 + 1;
	}

	/* While in town/wilderness */
	///system　満月フラグとか実装するならここか
	if (!dun_level && !p_ptr->inside_quest && !p_ptr->inside_battle && !p_ptr->inside_arena)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			//夜の屋外でパワーアップする妖精
			if(CHECK_FAIRY_TYPE == 30)
			{
				p_ptr->redraw |= (PR_STATE);
				p_ptr->update |= (PU_BONUS);
				handle_stuff();
			}

			/* Check for dawn */
			dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			/*:::夜が明けたらマップを明るくする*/
			if (dawn)
			{
				int y, x;

				msg_print(_("夜が明けた。", "The day breaks."));

				if (!p_ptr->wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Assume lit */
							c_ptr->info |= (CAVE_GLOW);

							/* Hack -- Memorize lit grids if allowed */
							if (view_perma_grids) c_ptr->info |= (CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(y, x);
						}
					}
				}
			}

			/*:::夜になったらマップを暗くする*/
			/* Night falls */
			else
			{
				int y, x;

				/* Message */
#ifdef JP
				msg_print("日が沈んだ。");
#else
				msg_print("The sun has fallen.");
#endif

				if (!p_ptr->wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Feature code (applying "mimic" field) */
							feature_type *f_ptr = &f_info[get_feat_mimic(c_ptr)];

							if (!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
							    !have_flag(f_ptr->flags, FF_ENTRANCE))
							{
								/* Assume dark */
								c_ptr->info &= ~(CAVE_GLOW);

								if (!have_flag(f_ptr->flags, FF_REMEMBER))
								{
									/* Forget the normal floor grid */
									c_ptr->info &= ~(CAVE_MARK);

									/* Hack -- Notice spot */
									note_spot(y, x);
								}
							}
						}

						/* Glow deep lava and building entrances */
						glow_deep_lava_and_bldg();
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS | PU_MON_LITE);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			if (p_ptr->special_defense & NINJA_S_STEALTH)
			{
				if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
			}
		}
	}

	//6:00と18:00に行われる処理
	//EXTRAモード以外
	if (!(turn % (TURNS_PER_TICK * TOWN_DAWN / 2)) && !EXTRA_MODE)
	{
		//月虹市場のトレードカウントをリセット
		//アビリティカードの高騰度を設定してカードショップの在庫を更新
		ability_card_trade_count = 0;
		set_abilitycard_price_rate();
		make_ability_card_store_list();
		break_market();

		//v2.0.19 養蜂家の蜂蜜増加もここに追加する
		if (p_ptr->pclass == CLASS_BEEKEEPER) add_honey();
	}

	//野良神様は日数経過で好戦度と名声変化
	if(p_ptr->prace == RACE_STRAYGOD && !(turn % (TURNS_PER_TICK * TOWN_DAWN)))
	{
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_WARLIKE] > 60) set_deity_bias(DBIAS_WARLIKE, -3);
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 80) set_deity_bias(DBIAS_REPUTATION, -1);
	}

	///mod151114 雛の人形販売
	if(p_ptr->pclass == CLASS_HINA && !(turn % (TURNS_PER_TICK * TOWN_DAWN / 24)))
	{
		p_ptr->magic_num1[1] += randint1((p_ptr->lev + p_ptr->stat_ind[A_CHR] + 3 + 20) * 5);
		if(p_ptr->magic_num1[1] > 100000) p_ptr->magic_num1[1] = 100000;
	}
	//雛「呪いの雛人形」の厄消費
	if (p_ptr->pclass == CLASS_HINA && (p_ptr->special_defense & SD_HINA_NINGYOU))
	{
		hina_gain_yaku(-(50+randint1(100)));
		if(!p_ptr->magic_num1[0])
		{
			msg_print(_("もう厄が残っていない。", "You don't have misfortune left."));
			p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
			p_ptr->redraw |= (PR_STATUS);
		}
	}

	//紫苑の装備品差し押さえ処理 半日で100カウントして没収
	if (p_ptr->pclass == CLASS_SHION && !(turn % (TURNS_PER_TICK * TOWN_DAWN / 200)))
	{
		int i;
		for (i = INVEN_RARM; i <= INVEN_FEET; i++)
		{
			shion_seizure(i);

		}

	}


	///mod140412 月齢処理
	///mod150121 慧音除く
	//v2.0.19 あうんも除く
	if((prace_is_(RACE_VAMPIRE)
		|| (prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE && p_ptr->pclass != CLASS_AUNN)
		||p_ptr->mimic_form == MIMIC_VAMPIRE)
		&& !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
	{
		bool dawn;
		bool beast = prace_is_(RACE_WARBEAST);


		dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));
		if(dawn)
		{
			if(beast)
			{
				if(turn / A_DAY % 30 + 15 == 30 && (dun_level))
				{
					msg_print(_("夜が明けたらしい。", "The dawn has come."));
					disturb(0,1);
				}
			}
			else
			{
				if(dun_level) msg_print(_("夜が明けたらしい。", "The dawn has come."));
				msg_print(_("体がだるい・・", "Your body feels heavy..."));
				disturb(0,1);
			}
		}
		else
		{
			if(beast)
			{
				if(turn / A_DAY % 30 + 16 == 30)
				{
					if(dun_level) msg_print(_("日が沈んだらしい。", "The sun has set."));
					msg_print(_("満月の晩だ！力がみなぎる！", "It's full moon! You feel full of power!"));
					disturb(0,1);
				}
			}
			else
			{
				if(dun_level) msg_print(_("日が沈んだらしい。", "The sun has set."));
				if(turn / A_DAY % 30 + 16 == 30)
					msg_print(_("満月の晩だ！力がみなぎる！", "It's full moon! You feel full of power!"));
				else
					msg_print(_("力が湧いてきた。", "You feel your power growing."));
				disturb(0,1);
			}
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
	}
	else if(p_ptr->pclass == CLASS_SUNNY && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)) )
	{
		if(!(turn % (TURNS_PER_TICK * TOWN_DAWN)))
		{
			if(dun_level) msg_format(_("夜が明けたらしい。", "The dawn has come."));
			msg_format(_("元気が出てきた！", "You feel energetic!"));
		}
		else
		{
			if(dun_level) msg_format(_("日が沈んだようだ。", "The sun has set."));
			if(dun_level) msg_format(_("元気が出ない・・", "You don't feel energetic..."));
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

	}
	else if(p_ptr->pclass == CLASS_LUNAR && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)) )
	{
		if(!(turn % (TURNS_PER_TICK * TOWN_DAWN)))
		{
			if(dun_level) msg_format(_("夜が明けたらしい。", "The dawn has come."));
			msg_format(_("元気が出ない・・", "You don't feel energetic..."));
		}
		else
		{
			if(dun_level) msg_format(_("日が沈んだようだ。", "The sun has set."));
			msg_format(_("元気が出てきた。", "You feel energetic."));
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

	}


	///mod150121 慧音特殊処理
	else if(p_ptr->pclass == CLASS_KEINE && !(p_ptr->inside_battle))
	{
		//満月時に地上にいると白沢化
		if(FULL_MOON && !dun_level && !p_ptr->magic_num1[0])
		{
			//ワールドマップ解除
			if(p_ptr->wild_mode)
			{
				energy_use = MIN(energy_use,100);
				change_wild_mode();
			}
			//変身解除
			if(p_ptr->mimic_form) set_mimic(0,0,TRUE);
			p_ptr->muta2_perma &= ~(MUT2_HARDHEAD);
			p_ptr->muta2 &= ~(MUT2_HARDHEAD);
			p_ptr->muta2 |= (MUT2_BIGHORN);//角変異
			p_ptr->muta2_perma |= (MUT2_BIGHORN);
			msg_print(_("満月だ。髪の色が変わり、白沢の角が現れた・・",
                        "It's full moon. Your hair color changes, and hakutaku horns appear..."));
			//場合によっては装備解除
			if(inventory[INVEN_HEAD].k_idx &&
			(	inventory[INVEN_HEAD].sval == SV_HEAD_METALCAP
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STARHELM
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STEELHELM
			||  inventory[INVEN_HEAD].sval == SV_HEAD_DRAGONHELM  ))
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format(_("%sが脱げてしまった！", "Your %s comes off!"),obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}
			p_ptr->update |= (PU_BONUS);
			p_ptr->magic_num1[0] = 1;//フラグ
			handle_stuff();

		}
		//白沢化解除
		else if(!(FULL_MOON) && p_ptr->magic_num1[0])
		{
			if(p_ptr->wild_mode)
			{
				energy_use = MIN(energy_use,100);
				change_wild_mode();
			}
			//変身解除
			if(p_ptr->mimic_form) set_mimic(0,0,TRUE);
			msg_print(_("夜明けだ。髪の色が戻り、白沢の角が消えた。",
                        "It's dawn. Your hair color returns to normal, and your hakutaku horns disappear."));
			p_ptr->muta2_perma &= ~(MUT2_BIGHORN);
			p_ptr->muta2 &= ~(MUT2_BIGHORN);
			p_ptr->muta2 |= (MUT2_HARDHEAD);//石頭
			p_ptr->muta2_perma |= (MUT2_HARDHEAD);
			p_ptr->update |= (PU_BONUS);
			p_ptr->magic_num1[0] = 0;//フラグ
			handle_stuff();

		}
	}


#if 0
	/* While in the dungeon (vanilla_town or lite_town mode only) */
	/*:::小規模な街、元祖の町のダンジョン内でたまに店主を交替*/
	if ((vanilla_town || (lite_town && !p_ptr->inside_quest && !p_ptr->inside_battle && !p_ptr->inside_arena)) && dun_level)
	{
		/*** Shuffle the Storekeepers ***/

		/* Chance is only once a day (while in dungeon) */
		if (!(turn % (TURNS_PER_TICK * STORE_TICKS)))
		{
			/* Sometimes, shuffle the shop-keepers */
			if (one_in_(STORE_SHUFFLE))
			{
				int n, i;

				/* Pick a random shop (except home and museum) */
				do
				{
					n = randint0(MAX_STORES);
				}
				while ((n == STORE_HOME) || (n == STORE_MUSEUM));

				/* Check every feature */
				for (i = 1; i < max_f_idx; i++)
				{
					/* Access the index */
					feature_type *f_ptr = &f_info[i];

					/* Skip empty index */
					if (!f_ptr->name) continue;

					/* Skip non-store features */
					if (!have_flag(f_ptr->flags, FF_STORE)) continue;

					/* Verify store type */
					if (f_ptr->subtype == n)
					{
						/* Message */
#ifdef JP
						if (cheat_xtra) msg_format("%sの店主をシャッフルします。", f_name + f_ptr->name);
#else
						if (cheat_xtra) msg_format("Shuffle a Shopkeeper of %s.", f_name + f_ptr->name);
#endif

						/* Shuffle it */
						//store_shuffle(n);

						break;
					}
				}
			}
		}
	}
#endif

	/*** Process the monsters ***/
	/*:::ダンジョンに新たなモンスターが湧く処理*/
	/* Check for creature generation. */
	if (one_in_(d_info[dungeon_type].max_m_alloc_chance) &&
	    !p_ptr->inside_arena && !p_ptr->inside_quest && !p_ptr->inside_battle && !(EXTRA_QUEST_FLOOR))
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, 0);
	}

	/* Hack -- Check for creature regeneration */
	/*:::モンスター自動回復*/
	if (!(turn % (TURNS_PER_TICK*10)) && !p_ptr->inside_battle) regen_monsters();
	if (!(turn % (TURNS_PER_TICK*3))) regen_captured_monsters();

	/*:::付喪神使いの帰還命令処理*/
	if (!(turn % (TURNS_PER_TICK*10)) && p_ptr->pclass == CLASS_TSUKUMO_MASTER && !(p_ptr->inside_battle)) tsukumo_recall();


	if (!p_ptr->leaving)
	{
		int i;

		/* Hack -- Process the counters of monsters if needed */
		/*:::MAX_MTIMED=16*/
		for (i = 0; i < MAX_MTIMED; i++)
		{
			if (mproc_max[i] > 0) process_monsters_mtimed(i);
		}
	}

	/*:::バイド細胞侵食変異を受けているとカウント開始　3日でゲームオーバー　治療してもカウントはリセットされない*/
	if(p_ptr->muta3 & MUT3_BYDO)
	{
		p_ptr->bydo += TURNS_PER_TICK;
		if((s32b)p_ptr->bydo > A_DAY * 3) p_ptr->bydo = A_DAY * 3+1;
		if((s32b)p_ptr->bydo > A_DAY * 3 && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,
                                                                        _("夏の夕暮れ　やさしく迎えてくれるのは　海鳥達だけなのか？",
                                                                            "Bydo cell infection"),0);
	}

	/*:::リリーホワイトのカウントダウン*/
	if(p_ptr->pclass == CLASS_LILY_WHITE)
	{
		if(turn > A_DAY * LILY_LIMIT && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,_("春の終わり", "end of spring"),0);
	}
	/*:::藍のカウントダウン*/
	if(p_ptr->pclass == CLASS_RAN && !p_ptr->total_winner)
	{
		if(turn > A_DAY * RAN_LIMIT && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,_("主からの再教育", "re-education from your master"),0);
	}
	//v1.1.92 女苑(専用性格)のツケの取り立て
	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && p_ptr->magic_num1[4] && !CHECK_JYOON_BUY_ON_CREDIT)
	{
		msg_print(_("ついに店主たちが痺れを切らした！", "The shopkeepers have ran out of patience!"));
		msg_print(NULL);
		p_ptr->au -= p_ptr->magic_num1[4];
		p_ptr->magic_num1[4] = 0;
		p_ptr->redraw |= PR_GOLD;
		redraw_stuff();
		msg_print(_("ツケを取り立てられてしまった...", "Your bill was collected..."));

	}

	/* Date changes */
	/*日付が変わったときの処理　日記記入と本日の賞金首変更*/
	///system
	if (!hour && !min)
	{
		if (min != prev_min)
		{
			p_ptr->today_mon = 0; //霊夢占いリセット


			do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

			//v2.05 はたての人探しリセット EXTRAでは日数経過でなく対象フロア通過でリセットするのでここでは何もしない
			if (!EXTRA_MODE)
			{
				p_ptr->hatate_mon_search_ridx = 0;
				p_ptr->hatate_mon_search_dungeon = 0;
			}


		//	determine_today_mon(FALSE);
		}
	}
	/*:::悪夢モードの太古の怨念処理*/
	/*
	 * Nightmare mode activates the TY_CURSE at midnight
	 *
	 * Require exact minute -- Don't activate multiple times in a minute
	 */
	if (ironman_nightmare && (min != prev_min))
	{
		/* Every 15 minutes after 11:00 pm */
		if ((hour == 23) && !(min % 15))
		{
			/* Disturbing */
			disturb(0, 1);

			switch (min / 15)
			{
			case 0:
#ifdef JP
				msg_print("遠くで不気味な鐘の音が鳴った。");
#else
				msg_print("You hear a distant bell toll ominously.");
#endif
				break;

			case 1:
#ifdef JP
				msg_print("遠くで鐘が二回鳴った。");
#else
				msg_print("A distant bell sounds twice.");
#endif
				break;

			case 2:
#ifdef JP
				msg_print("遠くで鐘が三回鳴った。");
#else
				msg_print("A distant bell sounds three times.");
#endif
				break;

			case 3:
#ifdef JP
				msg_print("遠くで鐘が四回鳴った。");
#else
				msg_print("A distant bell tolls four times.");
#endif
				break;
			}
		}

		/* TY_CURSE activates at midnight! */
		if (!hour && !min)
		{
			int count = 0;

			disturb(1, 1);
#ifdef JP
			msg_print("遠くで鐘が何回も鳴り、死んだような静けさの中へ消えていった。");
#else
			msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif

			activate_ty_curse(FALSE, &count);
		}
	}


	/*** Check the Food, and Regenerate ***/
	/*:::＠の満腹度処理*/
	///system 魔法使いや妖怪人形はここで空腹にならないようにすればいい？set_food内でやったほうがいいか
	if (!p_ptr->inside_battle)
	{
		/* Digest quickly when gorged */
		if (p_ptr->food >= PY_FOOD_MAX)
		{
			/* Digest a lot of food */
			(void)set_food(p_ptr->food - 100);
		}

		/* Digest normally -- Every 50 game turns */
		///sys 急回復を累積させるにはこの辺いじる必要あり
		else if (!(turn % (TURNS_PER_TICK*5)))
		{
			/* Basic digestion rate based on speed */
			//無加速10,加速+10で20
			int digestion = SPEED_TO_ENERGY(p_ptr->pspeed);

			/* Regeneration takes more food */
			if (p_ptr->regenerate)
				digestion += 20;
			if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
				digestion += 20;
			if (p_ptr->cursed & TRC_FAST_DIGEST)
				digestion += 30;

			if(p_ptr->pclass == CLASS_YOSHIKA)
				digestion += 20;


			/* Slow digestion takes less food */
			if (p_ptr->slow_digest)
				digestion -= 5;

			///mod140608 種族ごとに満腹減少をちょっと補正
			if(p_ptr->prace == RACE_SENNIN) digestion /= 3;
			if(p_ptr->prace == RACE_FAIRY) digestion /= 3;
			if(p_ptr->pclass == CLASS_HINA)	digestion /= 3;
			if(p_ptr->prace == RACE_ONI) digestion += 10;

			//v2.0.6 尤魔満腹度消費増加　レベルにより最大10倍
			if (p_ptr->pclass == CLASS_YUMA) digestion = digestion * (10 + p_ptr->lev) / 6;

			/* Minimal digestion */
			if (digestion < 1) digestion = 1;
			/* Maximal digestion */
			if (digestion > 1000) digestion = 1000;

			//妖精(光合成)
			if(CHECK_FAIRY_TYPE ==37 && !dun_level && turn % (TURNS_PER_TICK * TOWN_DAWN) < 50000)
			{
				set_food(MIN(p_ptr->food + digestion,PY_FOOD_MAX-1));
			}
			/* Digest some food */
			else
				(void)set_food(p_ptr->food - digestion);
		}


		/* Getting Faint */
		if (p_ptr->food < PY_FOOD_FAINT)
		{
			/* Faint occasionally */
			//v2.0.6 尤魔は空腹で麻痺しない(process_player()で暴走状態になる)
			if (!p_ptr->paralyzed && (randint0(100) < 10) && p_ptr->pclass != CLASS_YUMA)
			{
				/* Message */
#ifdef JP
				msg_print("あまりにも空腹で気絶してしまった。");
#else
				msg_print("You faint from the lack of food.");
#endif

				disturb(1, 1);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + 1 + randint0(5));
				if(one_in_(2)) set_deity_bias(DBIAS_REPUTATION, -1);
			}

			/* Starve to death (slowly) */
			if (p_ptr->food < PY_FOOD_STARVE)
			{
				/* Calculate damage */
				int dam = (PY_FOOD_STARVE - p_ptr->food) / 10;

				/* Take damage */
				//v2.0.6 尤魔究極モードの無敵化でも空腹ダメージを受けるようにここの無敵化処理を削除する
				//まあ尤魔以外で無敵化時の空腹ダメージを気にする状況もそうないだろう
#ifdef JP
				//if (!IS_INVULN())
					take_hit(DAMAGE_LOSELIFE, dam, "空腹", -1);
#else
				//if (!IS_INVULN())
                    take_hit(DAMAGE_LOSELIFE, dam, "starvation", -1);
#endif
			}
		}
	}


	//v1.1.86 クエスト「守矢の策動Ⅱ」のダンジョン内で丸太が降ってくる処理
	if (dun_level &&  p_ptr->inside_quest == QUEST_MORIYA_2)
	{
		int i;
		for (i = 0; i < 50; i++)
		{
			int x, y;
			x = randint1(220);
			y = 1;

			if (x > 190) x -= 30; //ラストスパート(X=160~190)は丸太の密度二倍

			//v2.0 マップの最上部を駆け抜けたら丸太に当たらないのはクエストの趣旨に反するし少々不自然なのでダメージを受けることにした
			if (player_bold(y, x))
			{
				msg_print(_("落ちてきた丸太が脳天に直撃した！", "A falling log lands on top of your head!"));
				disturb(1, 1);
				if (!IS_INVULN()) take_hit(DAMAGE_NOESCAPE, 50 + randint1(25), _("丸太", "falling log"), -1);
				set_stun(p_ptr->stun + randint1(50));
			}
			else if (in_bounds(y, x) && cave_empty_bold2(y, x))
			{
				summon_named_creature(0, y, x, MON_MARUTA, PM_NO_PET);
			}
		}
	}


	///mod140302 仙人(華扇意外)と天人が死神に襲われる処理
	//v1.1.42 EXTRAクエストフロアでは出ないようにした
	if ((p_ptr->prace == RACE_SENNIN || p_ptr->prace == RACE_TENNIN)
		&& (p_ptr->pclass != CLASS_KASEN)
		&& p_ptr->lev > 19
		&& !p_ptr->inside_arena
		&& !(EXTRA_QUEST_FLOOR)
		&& !p_ptr->inside_battle
		&& (!p_ptr->inside_quest || !is_fixed_quest_idx(p_ptr->inside_quest)))
	{
		int chance = 3000 - dun_level * 16;
		if( one_in_(chance))
		{
			//v1.2.00 レベルをdun_levelから(plev+dun_level)/2にした。ヘルファイアDが最終盤まで出ないように
			if (summon_specific(0, py, px, (dun_level+p_ptr->lev)/2, SUMMON_DEATH,(PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				msg_format(_("地獄からの刺客が現れた！", "An assassin from hell appears!"));
		}
	}


	if(p_ptr->special_defense & MUSOU_TENSEI)
	{
		if(p_ptr->csp < 50)
		{
			msg_print(_("再び様々なものにとらわれるようになった。",
                        "You regain earthly obsessions."));
			p_ptr->special_defense &= ~(MUSOU_TENSEI);
			p_ptr->redraw |= (PR_MAP | PR_STATUS);
			p_ptr->update |= (PU_MONSTERS | PU_BONUS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
		else
		{
			p_ptr->csp -= 50;
			p_ptr->redraw |= PR_MANA;
		}
	}



	///mod140821 エンジニアの故障中の機械が修復される処理
	engineer_malfunction(0, 1);

	/*:::泥酔度による処理*/
	process_world_aux_alcohol();

	/* Process timed damage and regeneration */
	process_world_aux_hp_and_sp();

	/* Process timeout */
	process_world_aux_timeout();

	/* Process light */
	process_world_aux_light();

	/* Process mutation effects */
	process_world_aux_mutation();

	/* Process curse effects */
	process_world_aux_curse();

	/* Process recharging */
	process_world_aux_recharge();

	/* Feel the inventory */
	sense_inventory1();
	sense_inventory2();

	/* Involuntary Movement */
	process_world_aux_movement();
}

///mod140225
/*:::bコマンドで表示する説明を選ぶ*/
int choose_browse(void)
{
	int j, line;
	char temp[62*5];
	char com;

	screen_save();
	while(1)
	{
		Term_erase(12, 5, 255);
		Term_erase(12, 4, 255);
		Term_erase(12, 3, 255);
		Term_erase(12, 2, 255);
		Term_erase(12, 1, 255);

		put_str(_("a/u/U) 身体特性・突然変異の発動", "a/u/U) Activated body abilities/mutations"), 2, 16);
		put_str(_("b/m/M) 習得済みの魔法", "b/m/M) Known spells"), 3, 16);
		put_str(_("c/j/J) 習得済みの職業特技", "c/j/J) Known class abilities"), 4, 16);

		prt(_("何についての説明を読みますか？", "Browse what?"),0,0);
		com = inkey();
		if (com == ESCAPE){ screen_load();return 0;}
		if (com=='a'||com=='u'||com=='U') {screen_load();return 1;}
		if (com=='b'||com=='m'||com=='M') {screen_load();return 2;}
		if (com=='c'||com=='j'||com=='J') {screen_load();return 3;}
	}

}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Wizard mode is not permitted */
		if (!allow_debug_opts || arg_wizard)
		{
#ifdef JP
			msg_print("ウィザードモードは許可されていません。 ");
#else
			msg_print("Wizard mode is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("ウィザードモードはデバッグと実験のためのモードです。 ");
		msg_print("一度ウィザードモードに入るとスコアは記録されません。");
#else
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にウィザードモードに入りたいのですか? "))
#else
		if (!get_check("Are you sure you want to enter wizard mode? "))
#endif
		{
			return (FALSE);
		}

		if (p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			msg_print(_("ウィザードモードに入ると次の蒐集家にエントランスを引き継げなくなります。",
                        "If you enter wizard mode, you'll be unable to carry over items at your entrance to next Collector."));
			if(!get_check(_("***本当に***よろしいですか? ",
                            "Are you ***REALLY*** sure?")))	return FALSE;
		}


#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "ウィザードモードに突入してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to enter wizard mode.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Debug mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("デバッグコマンドは許可されていません。 ");
#else
			msg_print("Use of debug command is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("デバッグ・コマンドはデバッグと実験のためのコマンドです。 ");
		msg_print("デバッグ・コマンドを使うとスコアは記録されません。");
#else
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にデバッグ・コマンドを使いますか? "))
#else
		if (!get_check("Are you sure you want to use debug commands? "))
#endif
		{
			return (FALSE);
		}

		if (p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			msg_print(_("デバッグコマンドを使うと次の蒐集家にエントランスを引き継げなくなります。",
                        "If you enter wizard mode, you'll be unable to carry over items at your entrance to next Collector."));
			if(!get_check(_("***本当に***よろしいですか? ",
                            "Are you ***REALLY*** sure?")))	return FALSE;
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "デバッグモードに突入してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use debug commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & 0x0010))
	{
		/* Mention effects */
#ifdef JP
		msg_print("ボーグ・コマンドはデバッグと実験のためのコマンドです。 ");
		msg_print("ボーグ・コマンドを使うとスコアは記録されません。");
#else
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("本当にボーグ・コマンドを使いますか? "))
#else
		if (!get_check("Are you sure you want to use borg commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "ボーグ・コマンドを使用してスコアを残せなくなった。");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to use borg commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	int old_now_message = now_message;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	/*:::コマンドが'n'のとき、前回のコマンドを持ってきてcommand_cmdに入れる。そうでないときコマンドを次のリピート用に登録する。*/
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	now_message = 0;

	/* Sniper */
	///class スナイパー集中処理
	//if ((p_ptr->pclass == CLASS_SNIPER) && (p_ptr->concent))
	///mod141116 スナイパー判定を変更
	if ((CLASS_USE_CONCENT) && (p_ptr->concent))
		reset_concent = TRUE;

	break_eibon_flag = TRUE;

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		case '\n':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (p_ptr->wizard)
			{
				p_ptr->wizard = FALSE;
#ifdef JP
msg_print("ウィザードモード解除。");
#else
				msg_print("Wizard mode off.");
#endif

			}
			else if (enter_wizard_mode())
			{
				p_ptr->wizard = TRUE;
#ifdef JP
msg_print("ウィザードモード突入。");
#else
				msg_print("Wizard mode on.");
#endif

			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				if (!p_ptr->wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if (!p_ptr->wild_mode)
			{
				if(p_ptr->pclass == CLASS_3_FAIRIES)
					do_cmd_wield_3_fairies();
				else
					do_cmd_wield(FALSE);
			}
			break_eibon_flag = FALSE;
			break;
		}

		/* Take off equipment */
		case 't':
		{
			if (!p_ptr->wild_mode) do_cmd_takeoff();
			break_eibon_flag = FALSE;
			break;
		}

		/* Drop an item */
		case 'd':
		{
			if (!p_ptr->wild_mode) do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break_eibon_flag = FALSE;
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */

		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		/*:::サブウィンドウの装備一覧とザック一覧を切り替える*/
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		/*:::指定方向に何かする汎用コマンド*/
		case '+':
		{
			if (!p_ptr->wild_mode) do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			if (!p_ptr->wild_mode) do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(TRUE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(!always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		//v1.1.27 結界ガード
		case KTRL('g'):
		{
			if(set_mana_shield(TRUE,FALSE))
				energy_use = 100;
			break;
		}

		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			if (!p_ptr->wild_mode) do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			if (p_ptr->action == ACTION_SEARCH) set_action(ACTION_NONE);
			else set_action(ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case SPECIAL_KEY_STORE:
		{
			if (!p_ptr->wild_mode) do_cmd_store();
			break;
		}

		/* Enter building -KMW- */
		case SPECIAL_KEY_BUILDING:
		{
			if (!p_ptr->wild_mode) do_cmd_bldg();
			break;
		}

		/* Enter quest level -KMW- */
		case SPECIAL_KEY_QUEST:
		{
			if (!p_ptr->wild_mode) do_cmd_quest();
			break;
		}

		/* Go up staircase */
		case '<':
		{


			if (!p_ptr->wild_mode && !dun_level && !p_ptr->inside_arena && !p_ptr->inside_quest)
			{
				if (vanilla_town) break;

				if (ambush_flag && !SAKUYA_WORLD)
				{
#ifdef JP
					msg_print("襲撃から逃げるにはマップの端まで移動しなければならない。");
#else
					msg_print("To flee the ambush you have to reach the edge of the map.");
#endif
					break;
				}

				if (p_ptr->food < PY_FOOD_WEAK)
				{
#ifdef JP
					msg_print("その前に食事をとらないと。");
#else
					msg_print("You must eat something here.");
#endif
					break;
				}

				change_wild_mode();
			}
			else
				do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			if(cave_have_flag_bold(py,px,FF_PORTAL))
				pass_through_portal();
			else if (p_ptr->wild_mode)
				change_wild_mode();
			else
				do_cmd_go_down();

			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			if (!p_ptr->wild_mode) do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			if (!p_ptr->wild_mode) do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			//楔を打つコマンド削除
			//if (!p_ptr->wild_mode) do_cmd_spike();
			//職業能力説明を入れておく。いずれbコマンドに統合予定。
			///mod140906 ローグライクコマンドのとき技実行にするように変更
			if(rogue_like_commands)
			{
				 if(!p_ptr->wild_mode || p_ptr->pclass == CLASS_YUKARI) do_cmd_new_class_power(FALSE);
			}
			else do_cmd_new_class_power(TRUE);

			break;
		}
		case 'J':
		{
			///mod140202 職業ごと特殊能力
			if (!(p_ptr->wild_mode && p_ptr->pclass != CLASS_YUKARI)) do_cmd_new_class_power(FALSE);
			break;
		}

		/* Bash a door */
		case 'B':
		{
			if (!p_ptr->wild_mode) do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			if (!p_ptr->wild_mode) do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			///class realm
			///sys 呪文学習　自宅限定かつ日数ペナルティにするか？
			//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
			if (REALM_SPELLMASTER)
				msg_print(_("あなたは呪文を学習する必要はない。",
                            "You don't have to learn spells."));
			else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
				do_cmd_gain_hissatsu();
			else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
				gain_magic();
			else
				do_cmd_study();

			break_eibon_flag = FALSE;

			break;
		}

		/* Browse a book */
		case 'b':
		{
			int choose;
			if (!repeat_pull(&choose))
			{
				choose = choose_browse();
				if(choose) repeat_push(choose);
			}
			switch(choose)
			{
			case 0:
				break;
			case 1:
				do_cmd_racial_power(TRUE);
				break;
			case 2:
				if(p_ptr->pclass == CLASS_MINDCRAFTER || p_ptr->pclass == CLASS_SUMIREKO) do_cmd_mind_browse();
				else do_cmd_browse();
				break;
			case 3:
				do_cmd_new_class_power(TRUE);
				break;
			}
			break;

/*
			if ( (p_ptr->pclass == CLASS_MINDCRAFTER) ||
			     (p_ptr->pclass == CLASS_BERSERKER) ||
			     (p_ptr->pclass == CLASS_NINJA) ||
			     (p_ptr->pclass == CLASS_MIRROR_MASTER)
			     ) do_cmd_mind_browse();
			else if (p_ptr->pclass == CLASS_SMITH)
				msg_print("鍛冶一時無効化中");
				//do_cmd_kaji(TRUE);
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE, FALSE);
			else if (p_ptr->pclass == CLASS_SNIPER)
				do_cmd_snipe_browse();
			else do_cmd_browse();
			break;
*/
		}

		/* Cast a spell */
		///class mコマンド
		case 'm':
		{
			/* -KMW- */
			if (!p_ptr->wild_mode)
			{
				//if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_ARCHER) || (p_ptr->pclass == CLASS_CAVALRY))
				if(cp_ptr->realm_aptitude[0] == 0 && p_ptr->pclass != CLASS_SAMURAI
					&& p_ptr->pclass != CLASS_YOUMU && p_ptr->pclass != CLASS_MINDCRAFTER && p_ptr->pclass != CLASS_SUMIREKO && p_ptr->pclass != CLASS_SEIJA
					&& p_ptr->pclass != CLASS_MAGIC_EATER )
				{
#ifdef JP
					msg_print("あなたは呪文を使えない。");
#else
					msg_print("You cannot cast spells!");
#endif
				}
				else if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC)/* && (p_ptr->pclass != CLASS_SMITH)*/)
				{
#ifdef JP
					msg_print("ダンジョンが魔法を吸収した！");
#else
					msg_print("The dungeon absorbs all attempted magic!");
#endif
					msg_print(NULL);
				}
				else if (p_ptr->anti_magic && (p_ptr->pclass != CLASS_BERSERKER)/* && (p_ptr->pclass != CLASS_SMITH)*/)
				{
#ifdef JP

					cptr which_power = "魔法";
#else
					cptr which_power = "magic";
#endif
					if (p_ptr->pclass == CLASS_MINDCRAFTER || p_ptr->pclass == CLASS_SUMIREKO)
#ifdef JP
						which_power = "超能力";
#else
						which_power = "psionic powers";
#endif
					else if (p_ptr->pclass == CLASS_IMITATOR)
#ifdef JP
						which_power = "ものまね";
#else
						which_power = "imitation";
#endif
					else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
#ifdef JP
						which_power = "必殺剣";
#else
						which_power = "hissatsu";
#endif
					else if (p_ptr->pclass == CLASS_MIRROR_MASTER)
#ifdef JP
						which_power = "鏡魔法";
#else
						which_power = "mirror magic";
#endif
/*
					else if (p_ptr->pclass == CLASS_NINJA)
#ifdef JP
						which_power = "忍術";
#else
						which_power = "ninjutsu";
#endif
*/
/*
					else if (mp_ptr->spell_book == TV_LIFE_BOOK)
#ifdef JP
						which_power = "祈り";
#else
						which_power = "prayer";
#endif
*/

#ifdef JP
					msg_format("反魔法バリアが%sを邪魔した！", which_power);
#else
					msg_format("An anti-magic shell disrupts your %s!", which_power);
#endif
					energy_use = 0;
				}

				else if (p_ptr->shero && p_ptr->pseikaku != SEIKAKU_BERSERK )
				{
#ifdef JP
					msg_format("狂戦士化していて頭が回らない！");
#else
					msg_format("You cannot think directly!");
#endif
					energy_use = 0;
				}
				else
				{
					if ((p_ptr->pclass == CLASS_MINDCRAFTER) ||
					    (p_ptr->pclass == CLASS_SUMIREKO) ||
					   // (p_ptr->pclass == CLASS_BERSERKER) ||
					   // (p_ptr->pclass == CLASS_NINJA) ||
					    (p_ptr->pclass == CLASS_MIRROR_MASTER)
					    )
						do_cmd_mind();
					///sysdel ものまね、青魔、鍛冶、スナイパーはしばらく無効化しておこう
					else if (p_ptr->pclass == CLASS_IMITATOR)
						do_cmd_mane(FALSE);
					else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
						do_cmd_magic_eater(FALSE, FALSE);
					else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
						do_cmd_hissatsu();
					else if (p_ptr->pclass == CLASS_BLUE_MAGE)
						do_cmd_cast_learned();
					//else if (p_ptr->pclass == CLASS_SMITH)
					//do_cmd_kaji(FALSE);
					else if (p_ptr->pclass == CLASS_SNIPER)
						do_cmd_snipe();
					else
						do_cmd_cast();
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			if (!p_ptr->wild_mode) do_cmd_pet();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		/*:::アイテムに銘を付ける*/
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		/*:::アイテムの銘を消す*/
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
		case 'A':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_activate();
				//v1.1.51 アリーナ内でも道具類を使えることにする
#if 0
			if (!p_ptr->inside_arena)
				do_cmd_activate();
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break_eibon_flag = FALSE;
			break;
		}

		/* Fire an item */
		case 'f':
		{
			if (!p_ptr->wild_mode) (void)do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_throw();
			}
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 アリーナ内でも道具類を使えることにする
				do_cmd_aim_wand();
#if 0
				if (!p_ptr->inside_arena)
				do_cmd_aim_wand();
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 アリーナ内でも道具類を使えることにする
				if (use_command && rogue_like_commands)
				{
					do_cmd_use();
				}
				else
				{
					do_cmd_zap_rod();
				}


#if 0
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_zap_rod();
			}
#endif
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 アリーナ内でも道具類を使えることにする
				do_cmd_quaff_potion();
#if 0
			if (!p_ptr->inside_arena)
				do_cmd_quaff_potion();
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 アリーナ内でも道具類を使えることにする
				do_cmd_read_scroll();
#if 0
				if (!p_ptr->inside_arena)
				do_cmd_read_scroll();
			else
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Use a staff */
		case 'u':
		{
			if (!p_ptr->wild_mode)
			{
			//v1.1.51 アリーナ内でも道具類を使えることにする
			//if (p_ptr->inside_arena)
			if(FALSE)
			{
#ifdef JP
msg_print("アリーナが魔法を吸収した！");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && !rogue_like_commands)
			{
				do_cmd_use();
			}
			else
				do_cmd_use_staff();
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			if (!p_ptr->wild_mode) do_cmd_racial_power(FALSE);
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			if (!p_ptr->wild_mode) do_cmd_target();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		case '$':
		{
			do_cmd_reload_autopick();
			break;
		}
		case '_':
		{
			do_cmd_edit_autopick();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			do_cmd_redraw();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			do_cmd_redraw();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			(void)combine_and_reorder_home(STORE_HOME);
			do_cmd_redraw();
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			if (!p_ptr->wild_mode) do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(old_now_message);
			break;
		}

		/* Show quest status -KMW- */
		case KTRL('Q'):
		{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			now_message = old_now_message;
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			do_cmd_save_game(FALSE);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			do_cmd_time();
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		case SPECIAL_KEY_QUIT:
		{
			do_cmd_save_and_exit();
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Record/stop "Movie" */
		///sysdel131221 ムービー無効
		///mod150224 ムービー復活？
		case ']':
		{
			prepare_movie_hooks();
			break;
		}

		case '[':
        {
            verify_panel_force_recenter();
            break;
        }

		/* Make random artifact list */
		case KTRL('V'):
		{
			spoil_random_artifact("randifact.txt");
			break;
		}

#ifdef TRAVEL
		case '`':
		{
			if (!p_ptr->wild_mode) do_cmd_travel();
			if (p_ptr->special_defense & KATA_MUSOU)
			{
				set_action(ACTION_NONE);
			}
			break;
		}
#endif

		/* Hack -- Unknown command */
		default:
		{
			if (flush_failure) flush();
			if (one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
#ifdef JP
				if (!get_rnd_line("error_j.txt", 0, error_m))
#else
				if (!get_rnd_line("error.txt", 0, error_m))
#endif

					msg_print(error_m);
			}
			else
#ifdef JP
prt(" '?' でヘルプが表示されます。", 0, 0);
#else
				prt("Type '?' for help.", 0, 0);
#endif

			break;
		}
	}
	if (!energy_use && !now_message)
		now_message = old_now_message;
}



/*:::釣りコマンドで釣れるモンスター判定　水棲でJjlwの非ユニークであること*/
///sys 釣り　ユニークが釣れるようにするにはここ設定要　ここだけでは足りないが
static bool monster_tsuri(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if ((r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) && my_strchr("Jjlw", r_ptr->d_char))
		return TRUE;
	else
		return FALSE;
}


/* Hack -- Pack Overflow */
/*:::アイテムあふれ処理。inventory[23]を使う*/
static void pack_overflow(void)
{
	if (inventory[INVEN_PACK].k_idx)
	{
		char o_name[MAX_NLEN];
		object_type *o_ptr;

		/* Is auto-destroy done? */
		notice_stuff();
		if (!inventory[INVEN_PACK].k_idx) return;

		/* Access the slot to be dropped */
		o_ptr = &inventory[INVEN_PACK];

		/* Disturbing */
		disturb(0, 1);

		/* Warning */
#ifdef JP
		msg_print("ザックからアイテムがあふれた！");
#else
		msg_print("Your pack overflows!");
#endif

		/* Describe */
		object_desc(o_name, o_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%s(%c)を落とした。", o_name, index_to_label(INVEN_PACK));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(INVEN_PACK));
#endif

		/* Drop it (carefully) near the player */
		/*:::アイテムを落とす。消滅はしない。*/
		(void)drop_near(o_ptr, 0, py, px);

		/* Modify, Describe, Optimize */
		inven_item_increase(INVEN_PACK, -255);
		inven_item_describe(INVEN_PACK);
		inven_item_optimize(INVEN_PACK);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}
}

/*:::＠の速度でエネルギー100毎に発生する処理　今のところ歌と呪術のみ*/
/*:::＠の行動に合わせると歌いながらアイテム落としなどでターンを刻んで何度も効果発生できるから処理を分けているのだと思われる*/
/*
 * process the effects per 100 energy at player speed.
 */
static void process_upkeep_with_speed(void)
{

	int seiran_card_num = count_ability_card(ABL_CARD_SEIRAN);

	/* Give the player some energy */
	if (!load && p_ptr->enchant_energy_need > 0 && !p_ptr->leaving)
	{
		p_ptr->enchant_energy_need -= SPEED_TO_ENERGY(p_ptr->pspeed);
	}

	///mod151219 麻痺、意識不明、混乱状態になると歌などを解除するようにしてみる
	///v1.1.12 休憩も解除されたので休憩だけは除外
	if(p_ptr->action && p_ptr->action != ACTION_REST && (p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL)))
	{
		set_action(ACTION_NONE);
	}

	/* No turn yet */
	if (p_ptr->enchant_energy_need > 0) return;

	//v1.1.48 追加
	if (p_ptr->wild_mode) return;

	//v1.1.92 追加
	if (p_ptr->inside_battle) return;

	while (p_ptr->enchant_energy_need <= 0)
	{

		//v2.0.1 アビリティカード「舞い降りた兎」の効果
		//target_whoのモンスターが視界内にいるとロケットによる追加攻撃
		if (seiran_card_num)
		{
			int prob = calc_ability_card_prob(ABL_CARD_SEIRAN, seiran_card_num);

			if (randint0(100) < prob && target_who > 0 && target_okay())
			{
				monster_type *m_ptr = &m_list[target_who];

				if (is_hostile(m_ptr))
				{
					fire_rocket(GF_ROCKET, 5, p_ptr->lev * 2, 1);
				}


			}
		}

		///mod140329 レーダーセンスの処理をここに入れてみる
		if(!load && p_ptr->radar_sense)
		{
			int rad = p_ptr->lev / 2 + 5;

			map_area(rad);
		//	detect_all(rad);
			detect_traps(rad,TRUE);
			detect_objects_normal(rad);
			detect_objects_gold(rad);

		}
		///mod150116 村紗は水地形を感知する
		if(p_ptr->pclass == CLASS_MURASA && p_ptr->lev > 19
			|| CHECK_FAIRY_TYPE == 38)
		{
			murasa_detect_water = TRUE;
			map_area(15 + p_ptr->lev / 2);
			murasa_detect_water = FALSE;
		}

		//ご先祖様が見ているぞ
		if(p_ptr->pclass == CLASS_KOISHI && !load && p_ptr->magic_num1[0] && one_in_(4))
		{
			int dam = p_ptr->lev + 30 + adj_general[p_ptr->stat_ind[A_CHR]] * 2;
			bool flag = FALSE;
			p_ptr->magic_num1[0] -= 1;
			msg_print(_("周囲の人影が動いた！", "One of the figures surrounding you moves!"));
			if(fire_random_target(GF_REDEYE, dam, 4, 0,0))flag = TRUE;
			if(fire_random_target(GF_REDEYE, dam, 4, 0,0))flag = TRUE;

			if(!flag) msg_print(_("・・しかし敵がいなかった。", "...However, there were no nearby enemies."));

		}
		//インペリシャブルシューティング
		if(p_ptr->pclass == CLASS_MOKOU && !load && p_ptr->magic_num2[0])
		{
			int dam = p_ptr->lev * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			dam += randint1(dam);
			msg_print(_("不尽の弾幕が放たれた！", "You fire imperishable danmaku!"));
			project(0,5,py,px,dam,GF_MISSILE,PROJECT_KILL,-1);

			p_ptr->csp -= 30;
			if(p_ptr->csp < 1)
			{
				msg_print(_("気力が尽きた。", "You are exhausted."));
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->magic_num2[0] = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();

		}
		//v1.1.48 スーパー貧乏神状態の紫苑　視界内劣化攻撃
		if (SUPER_SHION && one_in_(6))
		{

			msg_print(_("負のオーラが周囲を呑み込んだ！", "Negative aura spreads around you!"));
			project_hack2(GF_DISENCHANT,0,0, p_ptr->lev * 8);

		}

		//v1.1.76 幽々子特技「西行寺無余涅槃」
		if (p_ptr->pclass == CLASS_YUYUKO && p_ptr->special_defense & (KATA_MUSOU))
		{
			yuyuko_nehan();

		}


		//v1.1.92 女苑専用性格　ダメージを食らった紫苑が油を撒く
		//食らったダメージはmagic_num1[6]に記録されている
		if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && !p_ptr->inside_battle)
		{
			int plev = p_ptr->lev;
			//姉がキレたあと疲労しているとき
			if (p_ptr->magic_num1[6] < 0)
			{
				p_ptr->magic_num1[6] += 1;
			}
			//姉がキレたら油を大量にばらまいた後疲労する
			else if (p_ptr->magic_num1[6] >= 30000)
			{
				int i;
				int dam = plev * 4;
				int rad = plev / 12 + 1;
				msg_print(_("姉がキレた！", "Your sister loses her temper!"));
				disturb(0, 0);

				for (i = 1; i<10; i++)
				{
					int x, y;
					scatter(&y, &x, py, px, 4, 0);

					//石油地形生成と水攻撃(見えない)のランダム発動
					project(0, rad, y, x, dam, GF_DIG_OIL, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);
					project(0, rad, y, x, dam, GF_WATER, PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE, -1);
				}
				//しばらく値をマイナスにする。これが0以上になるまで攻撃を肩代わりしてくれない(take_hit()参照)
				p_ptr->magic_num1[6] = -100 - randint1(100);

			}
			//それ以外のとき、確率で石油を撒いて少しカウントを減らす
			else if (p_ptr->magic_num1[6]>0)
			{
				int x, y;
				int rad = plev / 20;

				//ここもう少し判定調整？ダメージが貯まっていればいるほど発動しやすく、あまり毎ターン発動しないように
				if (randint1(SHION_OIL_BOMB_LIMIT) < p_ptr->magic_num1[6] * 5)
				{
					scatter(&y, &x, py, px, 1, 0);
					msg_print(_("姉の体から石油が滴り落ちた。", "Oil pours out of your sister's body."));
					project(0, rad, y, x, p_ptr->lev * 2, GF_DIG_OIL, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);

					p_ptr->magic_num1[6] -= 50;
					if (p_ptr->magic_num1[6] < 0) p_ptr->magic_num1[6] = 0;

				}

			}

		}

		/* Handle the player song */
		/*:::継続する歌のMP計算や効果発生など*/
		if (!load) check_music();

		/* Hex - Handle the hex spells */
		if (!load) check_hex();
		if (!load) revenge_spell();

		/* There is some randomness of needed energy */
		p_ptr->enchant_energy_need += ENERGY_NEED();
	}
}


/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	//v1.1.86 前回行動時からHPが変化したかどうか確認するための変数　アビリティカード「暴食のムカデ」に関係
	static int prev_turn_hp = 0;

	//ゲーム開始時に一度だけ行われる処理がある場合ここに記述。
	//この処理の結果はクイックスタートには保存されない。
	if (hack_startup)
	{
		//エンジニアギルド
		engineer_guild_price_change();

		//v1.1.21 兵士が最初のスキルポイントを得る
		if(p_ptr->pclass == CLASS_SOLDIER)
			get_soldier_skillpoint();

		//v2.0.14 ちやりの血液の初期所持
		if (p_ptr->pclass == CLASS_CHIYARI) p_ptr->magic_num1[0] = 100;

		//v2.1.2 チミの弱体メッセージはゲーム開始時には出ないようなので説明がてら出しておく
		if (p_ptr->pclass == CLASS_CHIMI)
		{
			msg_print(_("ここは人里だ。自然が切り開かれた場所では思うように力を発揮できない！",
					"This is Human Village. You're unable to access your power in a place with natural terrain cleared away like this!"));
		}

		//v1.1.52 菫子新性格
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			okina_make_nameless_arts(0);
		}

		//v1.1.87 アビリティカード高騰度とカードショップ在庫の設定
		set_abilitycard_price_rate();
		make_ability_card_store_list();

		//種族野良神様 初期神性セット
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			p_ptr->race_multipur_val[3] = p_ptr->chaos_patron;
			p_ptr->race_multipur_val[DBIAS_COSMOS] = deity_table[p_ptr->race_multipur_val[3]].dbias_cosmos;
			p_ptr->race_multipur_val[DBIAS_WARLIKE] = deity_table[p_ptr->race_multipur_val[3]].dbias_warlike;
			p_ptr->race_multipur_val[DBIAS_REPUTATION] = deity_table[p_ptr->race_multipur_val[3]].dbias_reputation;
			p_ptr->redraw |= PR_MISC;
			redraw_stuff();
		}
		///mod160721 輝夜開始時にどの強敵がどの神宝を持っているか決める
		if(p_ptr->pclass == CLASS_KAGUYA)
		{
			int tmp_list[KAGUYA_QUEST_MAX] = {MON_MORGOTH,MON_AZATHOTH,MON_ONE_RING,MON_UNICORN_ORD,MON_OMEGA};

			//モンスターリストをシャッフルする。もっと効率よいアルゴリズムがいくらでもありそうな気がするが。
			for(i=0;i<100;i++)
			{
				int n,m,tmp;

				n = randint0(KAGUYA_QUEST_MAX);
				m = randint0(KAGUYA_QUEST_MAX);

				if(n == m) continue;

				tmp = tmp_list[n];
				tmp_list[n] = tmp_list[m];
				tmp_list[m] = tmp;
			}
			//リストを保存する。kaguya_quest_art_list[]の添え字と一致
			for(i=0;i<KAGUYA_QUEST_MAX;i++)
			{
				p_ptr->magic_num1[i] = tmp_list[i];
			}
		}
		//v1.1.15
		//ヘカーティア開始時
		if(p_ptr->pclass == CLASS_HECATIA)
		{
			//3つの体のHP/MPの割合を示す変数をMAXにする(実質デバッグモードでないと関係ないが)
			for(i=0;i<6;i++) p_ptr->magic_num1[i] = 10000;
			//地球と月の体の性格を設定する
			get_hecatia_seikaku();
			//追加インベントリの前半に入っているのはどの体の装備品かフラグ　とりあえず地球
			p_ptr->magic_num2[1] = HECATE_BODY_EARTH;

		}

		//v2.0.7 千亦の初期所持カード
		if (p_ptr->pclass == CLASS_CHIMATA)
		{
			p_ptr->magic_num2[ABL_CARD_BLANK] = 1;
			p_ptr->magic_num2[ABL_CARD_LIFE] = 1;
			p_ptr->magic_num2[ABL_CARD_SPELL] = 1;
			p_ptr->magic_num2[ABL_CARD_MUGIMESHI] = 1;
			p_ptr->magic_num2[ABL_CARD_TSUKASA] = 1;
			p_ptr->magic_num2[ABL_CARD_MUKADE] = 1;

			chimata_calc_card_rank();

		}

		/* 鬼はゲーム開始時から酔っている*/
		///mod150205↑萃香だけにした
		if (p_ptr->pclass == CLASS_SUIKA) p_ptr->alcohol = 2000;

		/*::: わかさぎ姫と影狼は最初から草の根妖怪ネットワークに加入している*/
		if (p_ptr->pclass == CLASS_WAKASAGI) p_ptr->grassroots = 1;
		if (p_ptr->pclass == CLASS_KAGEROU) p_ptr->grassroots = 1;

		// 藍は最初から鉄獄の場所を知っている
		if (p_ptr->pclass == CLASS_RAN) max_dlv[DUNGEON_ANGBAND] = 1;

		// 雛は厄が500溜まっている
		if (p_ptr->pclass == CLASS_HINA) p_ptr->magic_num1[0] = 500;



		hack_startup = FALSE;
	}


	if(p_ptr->pclass == CLASS_MOKOU && flag_mokou_resurrection)
	{
		mokou_resurrection();
	}
	//v2.1.0 憑依中の瑞霊がやられたとき強制変身解除しHP半分残して復活
	else if (p_ptr->pclass == CLASS_MIZUCHI && flag_mokou_resurrection)
	{
		flag_mokou_resurrection = FALSE;

		msg_print(_("緊急脱出！", "Emergency escape!"));
		p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
		p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);
		set_mimic(0, 0, TRUE);

		p_ptr->chp = p_ptr->mhp / 2;

		p_ptr->redraw |= (PR_MANA | PR_HP);
		handle_stuff();


	}

	/*:::闘技場なら全モンスターが常に把握されている状態にする。そのあと＠は行動をせずそのまま終了*/
	if (p_ptr->inside_battle)
	{
		for(i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;

			/* Hack -- Detect monster */

			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);
		}
		prt_time();
	}
	else if (!(load && p_ptr->energy_need <= 0))
	{
		/* Give the player some energy */
		/*:::loadがFalseのとき、もしくはenergy_needが1以上のときENERGYを得る。loadは何だろう。ゲームロード直後のみtrue？*/
		p_ptr->energy_need -= SPEED_TO_ENERGY(p_ptr->pspeed);
	}

	/* No turn yet */
	if (p_ptr->energy_need > 0) return;

	/*:::コマンドがリピート中のときは時間を更新しない*/
	if (!command_rep)
	{
			prt_time();
			///mod140322 行動開始時にスコア表示を更新する
			prt_score();

	}
	/*** Check for interupts ***/

	//v2.0.6 尤魔の飢餓モード
	if (p_ptr->pclass == CLASS_YUMA)
	{
		if (check_yuma_ultimate_mode()) return;
	}


	/*:::完全休憩処理　-1や-2だと完全回復まで続けるらしい*/
	///sys もし将来「最大値を超えてHPを回復する」何かを実装するとき、ここ気を付けないとループする
	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			/*:::MP部分だけ>=なのは剣術家のための記述と思われる*/

			if ((p_ptr->chp == p_ptr->mhp) && (p_ptr->csp >= p_ptr->msp))
			{
				set_action(ACTION_NONE);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
				!p_ptr->tim_no_move && //v2.0.11 移動禁止
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall &&
				!REF_BROKEN_VAL && //v1.1.66
			    !p_ptr->alter_reality)
			{
				set_action(ACTION_NONE);
			}
		}
	}
	/*:::釣り*/
	if (p_ptr->action == ACTION_FISH)
	{
		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 10);
		if (one_in_(1000))
		{
			int r_idx;
			bool success = FALSE;
			get_mon_num_prep(monster_tsuri,NULL);
			r_idx = get_mon_num(dun_level ? dun_level : wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level);
			msg_print(NULL);
			if (r_idx && one_in_(2))
			{
				int y, x;
				y = py+ddy[tsuri_dir];
				x = px+ddx[tsuri_dir];
				if (place_monster_aux(0, y, x, r_idx, PM_NO_KAGE))
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%sが釣れた！", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if (!success)
			{
#ifdef JP
				msg_print("餌だけ食われてしまった！くっそ～！");
#else
				msg_print("Damn!  The fish stole your bait!");
#endif
			}
			disturb(0, 1);
		}
	}

	/* Handle "abort" */
	/*:::「連続コマンドはキー入力で中断」オプションがONのとき*/
	if (check_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || travel.run || command_rep || (p_ptr->action == ACTION_REST) || (p_ptr->action == ACTION_FISH))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			/*:::何かキー入力があれば中断する*/
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 1);

				/* Hack -- Show a Message */
#ifdef JP
msg_print("中断しました。");
#else
				msg_print("Canceled.");
#endif

			}
		}
	}

	/*:::Mega Hack ＠が赤蛮奇や妖夢のとき、行動開始時に分身が見ている地形とアイテムを記憶する*/
	update_minion_sight();

	/*:::ペット状態異常回復　朦朧・混乱･恐怖は技能が関わる*/
	if (p_ptr->riding && !p_ptr->confused && !p_ptr->blind)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (MON_CSLEEP(m_ptr))
		{
			char m_name[80];

			/* Recover fully */
			(void)set_monster_csleep(p_ptr->riding, 0);

			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%^sを起こした。", m_name);
#else
			msg_format("You have woken %s up.", m_name);
#endif
		}
		///sys 乗馬モンスター状態回復　騎乗技能判定

		if (MON_STUNNED(m_ptr))
		{
			/* Hack -- Recover from stun */
			///mod131226 skill 技能と武器技能の統合
			//if (set_monster_stunned(p_ptr->riding,(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (MON_STUNNED(m_ptr) - 1)))
			if (set_monster_stunned(p_ptr->riding,(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING) || p_ptr->pclass == CLASS_MAI) ? 0 : (MON_STUNNED(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを朦朧状態から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer stunned.", m_name);
#endif
			}
		}

		if (MON_CONFUSED(m_ptr))
		{
			/* Hack -- Recover from confusion */
			if (set_monster_confused(p_ptr->riding,
				(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING)) ? 0 : (MON_CONFUSED(m_ptr) - 1)))
///mod131226 skill 技能と武器技能の統合
//				(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (MON_CONFUSED(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを混乱状態から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer confused.", m_name);
#endif
			}
		}

		if (MON_MONFEAR(m_ptr))
		{
			/* Hack -- Recover from fear */
			///mod131226 skill 技能と武器技能の統合
			if (set_monster_monfear(p_ptr->riding,
				(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING)) ? 0 : (MON_MONFEAR(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^sを恐怖から立ち直らせた。", m_name);
#else
				msg_format("%^s is no longer afraid.", m_name);
#endif
			}
		}

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}

	load = FALSE;

	/* Fast */
	/*:::錬気術師のフラグ処理*/
	///class 練気術士の光速移動　有効時間は行動回数でカウント
	///mod140813 通常の一時処理と同じ場所に移動
/*
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr->lightspeed - 1, TRUE);
	}
*/
	/*:::錬気術師のmagic_num1[0]は溜まった気の量らしい*/
	///class 練気術　気のターン毎減少40
	if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
	{
		if (p_ptr->magic_num1[0] < 40)
		{
			p_ptr->magic_num1[0] = 0;
		}
		else p_ptr->magic_num1[0] -= 40;
		p_ptr->update |= (PU_BONUS);
	}
	/*:::青魔の学習のMP減少処理*/
	///class 青魔
	if (p_ptr->action == ACTION_LEARN)
	{
		s32b cost = 0L;
		u32b cost_frac = (p_ptr->msp + 30L) * 256L;

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(cost, cost_frac, 16);


		if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, cost, cost_frac) < 0)
		{
			/* Mana run out */
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
			set_action(ACTION_NONE);
		}
		else
		{
			/* Reduce mana */
			s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), cost, cost_frac);
		}
		p_ptr->redraw |= PR_MANA;
	}

	/*:::夢想の型のMP減少*/
	///class 剣術
	if (p_ptr->special_defense & KATA_MASK)
	{
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			//v1.1.76 幽々子最終奥義処理変更
			if (p_ptr->pclass == CLASS_YUYUKO)
			{
				if (p_ptr->csp < 50)
				{
					set_action(ACTION_NONE);
				}
				else
				{
					p_ptr->csp -= 50;
					p_ptr->redraw |= (PR_MANA);
				}


			}
			else //通常の夢想の型
			{
				if (p_ptr->csp < 3)
				{
					set_action(ACTION_NONE);
				}
				else
				{
					p_ptr->csp -= 2;
					p_ptr->redraw |= (PR_MANA);
				}
			}



		}
	}



	//v1.1.27 結界ガードシステム
	count_damage_guard_break = 0;


	//v2.0.5 ぬえが変身可能かどうかチェックし、可能なら変身する
	//変身可能チェックはここ限定にする。恐らくモンスターが消滅したときの変身可能チェックが想定しないときに呼ばれているのがクラッシュバグの原因
	if (p_ptr->pclass == CLASS_NUE && flag_nue_check_undefined)
	{

		check_nue_undefined();

		flag_nue_check_undefined = FALSE;

	}
	//v2.1.2 チミ　フロアの自然地形をカウント
	else if (p_ptr->pclass == CLASS_CHIMI && flag_chimi_need_count_feat)
	{
		chimi_count_feat(TRUE);
	}



	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	/*:::コマンド受付・実行ループ　何かエネルギーを使用する行動をするまで繰り返す*/
	while (p_ptr->energy_need <= 0)
	{
		p_ptr->window |= PW_PLAYER;
		p_ptr->sutemi = FALSE;
		now_damaged = FALSE;

		//v1.1.95 カウンター状態の解除　衣玖「羽衣は時の如く」使用中は解除されない
		//p_ptr->counterはセーブ/ロードされないのでここで弥縫的にTRUEにし直す
		if (p_ptr->pclass == CLASS_IKU && p_ptr->tim_general[0] && PLAYER_CAN_USE_ARTS)
		{
			p_ptr->counter = TRUE;
			p_ptr->redraw |= PR_STATUS;
		}
		else
		{
			p_ptr->counter = FALSE;
			p_ptr->redraw |= PR_STATUS;
		}

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Place the cursor on the player */
		move_cursor_relative(py, px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		/*:::アイテムあふれ処理*/
		pack_overflow();


		/* Hack -- cancel "lurking browse mode" */
		/*:::get_item()をbrowseモードで動かすための変数らしい　詳細不明*/
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;

		///mod150822 プリズムリバー三姉妹は意識不明のときたまに助けが来る
		if(CLASS_IS_PRISM_SISTERS
			&& (p_ptr->paralyzed || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
			&& one_in_(7))
		{
			prism_change_class(TRUE);
			p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);
			redraw_stuff();

		}

		if (!PLAYER_CAN_COMMAND && p_ptr->au > 1000)
		{
			int enma_card_num = count_ability_card(ABL_CARD_EIKI);

			if (randint1(3) <= enma_card_num)
			{
				int percen = calc_ability_card_prob(ABL_CARD_EIKI,enma_card_num);
				int payment = p_ptr->au * percen / 100;

				screen_save();
				for (i = 9; i<16; i++) Term_erase(17, i, 255);

				put_str(_("遠くから声が聞こえる...", "You hear a distant voice..."), 10, 30);
				if(percen >= 100)
					put_str(_("「有り金全てです。」", "'All of your money.'"), 12, 35);
				else
					put_str(format(_("「＄%dです。」", "'$%d.'"),payment), 12, 35);

				if (get_check_strict(_("支払いますか？", "Pay?"), CHECK_OKAY_CANCEL))
				{
					p_ptr->au -= payment;
					set_paralyzed(0);
					set_stun(0);
					set_alcohol(0);
					if (p_ptr->food < PY_FOOD_WEAK) set_food(PY_FOOD_ALERT);
					set_confused(0);
					set_blind(0);
					set_image(0);
					if (p_ptr->chp < p_ptr->mhp / 2) p_ptr->chp = p_ptr->mhp / 2;

					msg_print(_("あなたは不意に目覚めた。妙に財布が軽い...",
                                "You are suddenly awake. Oddly, your wallet feels lighter..."));
				}
				screen_load();
				do_cmd_redraw();

			}
		}


		///sysdel tougi
		if (p_ptr->inside_battle)
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_command();
		}

		/* Paralyzed or Knocked Out */
		else if (p_ptr->paralyzed || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
			if(p_ptr->pclass == CLASS_SUMIREKO && !EXTRA_MODE && !p_ptr->inside_arena)
			{
				flag_sumireko_wakeup = TRUE;
				p_ptr->leaving = TRUE;
			}
		}
		///mod140330 アル中でない者が泥酔度15000超えたら気絶
		//v1.1.19 15000
		else if (p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
		{
			/* Take a turn */
			energy_use = 100;
			if(p_ptr->pclass == CLASS_SUMIREKO && !EXTRA_MODE && !p_ptr->inside_arena)
			{
				flag_sumireko_wakeup = TRUE;
				p_ptr->leaving = TRUE;
			}
		}
		/* Resting */
		else if (p_ptr->action == ACTION_REST)
		{
			/* Timed rest */
			/*:::全回復でなくターン指定休憩のとき*/
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				if (!resting) set_action(ACTION_NONE);

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}
		//諏訪子冬眠中
		else if(p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
		{
			energy_use = 100;
		}

		/* Fishing */
		else if (p_ptr->action == ACTION_FISH)
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);
		}

#ifdef TRAVEL
		/* Traveling */
		else if (travel.run)
		{
			/* Take a step */
			travel_step();
		}
#endif

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{

			//v1.1.86 前回行動時からHPが変わってたらcauc_bonuses()を再実行する。
			//アビリティカード「暴食のムカデ」に関係
			//v2.0.1 追加カード「不屈の脳筋」も
			if (prev_turn_hp != p_ptr->chp)
			{
				prev_turn_hp = p_ptr->chp;
				if (count_ability_card(ABL_CARD_MUKADE) || count_ability_card(ABL_CARD_SAKI_2))
				{
					p_ptr->update |= PU_BONUS;
					update_stuff();

				}
			}

			/* Place the cursor on the player */
			move_cursor_relative(py, px);
			///mod140917 コマンド待受時にモンスターリスト更新
			//v1.1.66 常時ONにする
			//if(full_mon_list)
			{
				p_ptr->window |= PW_MONSTER_LIST;
				window_stuff();
			}

			//v1.1.58 フロアのBGM更新チェック
			if (flag_update_floor_music)
			{
				select_floor_music();
				flag_update_floor_music = FALSE;
			}

			can_save = TRUE;
			/* Get a command (normal) */
			request_command(FALSE);
			can_save = FALSE;

			/* Process the command */
			process_command();
		}
		/*:::コマンド選択、実行ループ終了*/

		/* Hack -- Pack Overflow */
		pack_overflow();

		/*** Clean up ***/

		/* Significant */
		/*:::何かエネルギーを消費する行動をした場合*/
		if (energy_use)
		{
			///mod150114 移動状態を画面左に表示することにしたので、何か行動するたびに表示を更新することにした
			p_ptr->redraw |= (PR_STATE);

			/* Use some energy */
			if (world_player || energy_use > 400)
			{
				/* The Randomness is irrelevant */
				p_ptr->energy_need += energy_use * TURNS_PER_TICK / 10;
			}
			else
			{
				/* There is some randomness of needed energy */
				p_ptr->energy_need += (s16b)((s32b)energy_use * ENERGY_NEED() / 100L);
			}

			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			/*:::多色モンスターの再描画処理？*/
			if (shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Skip unseen monsters */
					if (!m_ptr->ml) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->ap_r_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_SHAPECHANGER)))
						continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			/*:::詳細不明。「あとでモンスター情報を更新する」フラグ？*/
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					//召喚などで出てきたモンスターが魔法を使うようになる。
					//(トラップなど＠の行動で召喚されたモンスターはその行動終了時点でここに入るので実質魔法無制限)
					/* Nice monsters get mean */
					if (m_ptr->mflag & MFLAG_NICE)
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag2 & MFLAG2_MARK)
					{
						/* Maintain detection */
						if (m_ptr->mflag2 & MFLAG2_SHOW)
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(i, FALSE);

							if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
			/*:::ものまね師のものまね情報更新*/
			///class ものまね特殊処理
			if (p_ptr->pclass == CLASS_IMITATOR)
			{
				if (p_ptr->mane_num > (p_ptr->lev > 44 ? 3 : p_ptr->lev > 29 ? 2 : 1))
				{
					p_ptr->mane_num--;
					for (i = 0; i < p_ptr->mane_num; i++)
					{
						p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
						p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
					}
				}
				new_mane = FALSE;
				p_ptr->redraw |= (PR_IMITATION);
			}
			if (p_ptr->action == ACTION_LEARN)
			{
				new_mane = FALSE;
				p_ptr->redraw |= (PR_STATE);
			}

			/*:::時間停止中はenergy_needが-1000より小さくなっている*/
			if (world_player && (p_ptr->energy_need > - 1000))
			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

#ifdef JP
				msg_print("「時は動きだす…」");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				world_player = FALSE;
				p_ptr->energy_need = ENERGY_NEED();
				//v1.1.58
				flag_update_floor_music = TRUE;
				/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
				handle_stuff();
			}

			//v1.1.21 銃関係の一時値をリセット
			hack_gun_fire_critical_flag = 0L;
			special_shooting_mode = 0;

		}
		/*:::何かエネルギーを消費する行動をした場合の処理完了*/

		///mod160303 EXモードの建物内で特定の行動をとった時これらの変数が設定され敵召喚
		if(hack_ex_bldg_summon_idx || hack_ex_bldg_summon_type )
		{

			if (hack_ex_bldg_summon_type)
			{
				summon_specific(0, py, px, dun_level, hack_ex_bldg_summon_type, hack_ex_bldg_summon_mode);
			}
			else if (hack_ex_bldg_summon_idx)
			{
				int tmp_cnt;
				if (hack_ex_bldg_summon_num < 1) hack_ex_bldg_summon_num = 1;
				for(tmp_cnt =0; tmp_cnt <hack_ex_bldg_summon_num; tmp_cnt++)
					summon_named_creature(0, py, px, hack_ex_bldg_summon_idx, hack_ex_bldg_summon_mode);
			}
			hack_ex_bldg_summon_num = 0;
			hack_ex_bldg_summon_idx = 0;
			hack_ex_bldg_summon_type = 0;
			hack_ex_bldg_summon_mode = 0L;
		}

		/* Hack -- notice death */
		if (!p_ptr->playing || p_ptr->is_dead)
		{
			world_player = FALSE;
			break;
		}

		//エイボンの霧の車輪無効化処理
		if(energy_use && break_eibon_flag) break_eibon_wheel();

		/* Sniper */
		if (energy_use && reset_concent) reset_concentration(TRUE);

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/// -Mega Hack- 咲夜の世界特殊処理
		//energy_needを常に0に書き換え、ターン消費無しで行動し続ける。ループ注意
		if (SAKUYA_WORLD)
		{

			set_paralyzed(0);

			if(p_ptr->action == ACTION_REST && p_ptr->food < PY_FOOD_WEAK)
			{
				msg_print(_("お腹が空いて倒れそうだ。休憩どころではない。",
                            "You are about to collapse from hunger. You can't rest."));
				p_ptr->action = ACTION_NONE;
				p_ptr->energy_need = 0;
			}

			if(p_ptr->energy_need > 0)//空腹度処理 だいたい通常行動と同じような処理にする
			{
				int digestion = 10;
				//v1.1.19 泥酔度マジックナンバー排除
				if(p_ptr->alcohol >= DRANK_4) set_alcohol(DRANK_4-1);
				if(p_ptr->confused && one_in_(7)) set_confused(0);
				if(p_ptr->shero && one_in_(10)) set_shero(0,TRUE);

				if (p_ptr->regenerate) digestion += 20;
				if (p_ptr->cursed & TRC_FAST_DIGEST) digestion += 30;
				if (p_ptr->slow_digest)	digestion -= 5;


				digestion = digestion * p_ptr->energy_need / 500;

				if(p_ptr->action == ACTION_REST)
					digestion = digestion * (1 + 50*(p_ptr->mhp - p_ptr->chp)/p_ptr->mhp);

				(void)set_food(p_ptr->food - digestion);
				if (p_ptr->wild_mode && p_ptr->food < PY_FOOD_WEAK)
				{
					change_wild_mode();
				}
				if(p_ptr->food < PY_FOOD_STARVE) take_hit(DAMAGE_LOSELIFE, 1, _("空腹", "hunger"), -1);
			}

			if(p_ptr->action == ACTION_REST)
			{
				msg_print(_("あなたは休憩した。", "You take a break."));
				p_ptr->chp = p_ptr->mhp;
				p_ptr->chp_frac = 0L;
				set_stun(0);
				set_cut(0);
				set_image(0);
				set_blind(0);
				set_confused(0);
				set_shero(0,TRUE);
			    if (p_ptr->food >= PY_FOOD_MAX) p_ptr->food = PY_FOOD_MAX -1;
				p_ptr->redraw |= PR_HP | PR_EXTRA | PR_STATE | PR_STATUS;
				p_ptr->action = ACTION_NONE;
			}
			else if(p_ptr->action == ACTION_FISH)//paranoia
			{
				msg_print(_("今釣りなどしても意味がない。", "No point in fishing right now."));
				p_ptr->action = ACTION_NONE;
			}


			resting = 0;
			p_ptr->energy_need = 0;
			handle_stuff();

		}

	}
	/*:::コマンド受付実行ループ完了*/

	//v1.1.84 さとりのトラップ自動感知に使う
	p_ptr->update |= (PU_VIEW);

	//v1.1.86　アビリティカードでHP/MPが変動することになったので行動済ませるたびに計算する
	p_ptr->update |= (PU_HP | PU_MANA);


	/* Update scent trail */
	update_smell();

}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
/*:::現在階での処理を行い、階移動・ゲーム中断・ゲームオーバー以外はここのループを繰り返す*/
static void dungeon(bool load_game)
{
	int quest_num = 0;
	int i;

	/* Set the base level */
	base_level = dun_level;

	/* Reset various flags */
	hack_mind = FALSE;

	/* Not leaving */
	p_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;

#if 0 /* Don't reset here --- It's used for Arena */
	command_new = 0;
#endif

	command_rep = 0;
	command_arg = 0;
	command_dir = 0;


	/* Cancel the target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;
	ambush_flag = FALSE;

	/* Cancel the health bar */
	/*:::ターゲットの体力ゲージを消す？*/
	health_track(0);

	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;


	/* Disturb */
	disturb(1, 1);

	/* Get index of current quest (if any) */
	/*:::クエスト中ならクエストIDを、そうでないなら0を返す*/
	quest_num = quest_number(dun_level);

	/* Inside a quest? */
	if (quest_num)
	{
		/* Mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}

	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
		if (record_maxdepth) do_cmd_write_nikki(NIKKI_MAXDEAPTH, dun_level, NULL);

		///mod160522 鈴瑚の支援物資投下カウンタ フロアリセットでスカムできないようmagic_num2に独自で最深到達階数を管理している
		if(p_ptr->pclass == CLASS_RINGO && max_dlv[dungeon_type] > p_ptr->magic_num2[dungeon_type])
		{
			int tmp = (max_dlv[dungeon_type] - p_ptr->magic_num2[dungeon_type]) * (dun_level+100);
			if(tmp > 1000) tmp = 1000;
			p_ptr->magic_num1[4] += tmp;
			p_ptr->magic_num2[dungeon_type] = max_dlv[dungeon_type];
		}

	}
	//v1.1.25 ダンジョン制覇フラグ
	if(!p_ptr->inside_quest && dun_level && dun_level == d_info[dungeon_type].maxdepth && !flag_dungeon_complete[dungeon_type])
	{
		if(dungeon_type != DUNGEON_CHAOS &&
			(!d_info[dungeon_type].final_guardian || !r_info[d_info[dungeon_type].final_guardian].max_num))
		{
			msg_format(_("あなたは%sを制覇した！", "You have conquered %s!"),d_name+d_info[dungeon_type].name);
			flag_dungeon_complete[dungeon_type] = 1;
		}

	}

	/*:::ペットのコスト計算らしいが戻り値を受け取っていない。中の"p_ptr->update |= (PU_BONUS)"の処理のために実行しているのか？*/
	(void)calculate_upkeep();

	/* Validate the panel */
	/*:::現在画面の升目数を得る？*/
	panel_bounds_center();

	/* Verify the panel */
	/*:::描画関係のチェックや初期化？*/
	verify_panel();

	/* Flush messages */
	msg_print(NULL);

	/* Enter "xtra" mode */
	/*:::初期化中状態のフラグを立てる？*/
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_TORCH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_DISTANCE | PU_FLOW);

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	/*:::ステータス・パラメータ・マップの再計算・再描画処理*/
	handle_stuff();
	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Handle "p_ptr->notice" */
	notice_stuff();

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	handle_stuff();

	/*::: -Giga Hack- 霊夢・魔理沙など可変パラメータモンスターがいるときパラメータを再計算する*/
	/*:::ゲーム開始直後はr_infoの初期値なので直接上書きする*/
	for(i=1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		if(!(r_info[m_ptr->r_idx].flags7 & RF7_VARIABLE)) continue;

		if (m_ptr->r_idx == MON_REIMU ) apply_mon_race_reimu();
		else if (m_ptr->r_idx == MON_MARISA ) apply_mon_race_marisa();
		else if (m_ptr->r_idx == MON_MASTER_KAGUYA ) apply_mon_race_master_kaguya();
		else if(IS_RANDOM_UNIQUE_IDX(m_ptr->r_idx)); //ランダムユニークはロード時にパラメータを読み込むのでここでは何もしない
		else if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
		{
			apply_mon_race_tsukumoweapon(&o_list[m_ptr->hold_o_idx],m_ptr->r_idx);
			lore_do_probe(m_ptr->r_idx);

		}
		else if(m_ptr->r_idx == MON_BANKI_HEAD_1 || m_ptr->r_idx == MON_BANKI_HEAD_2)
			(void) apply_mon_race_banki_head(m_ptr->r_idx);
		else if(m_ptr->r_idx == MON_GREEN_EYED)
			(void) apply_mon_race_green_eyed();

		else msg_format(_("ERROR:可変パラメータモンスター(r_idx:%d)のロード時パラメータ再設定がされていない",
                            "ERROR: Variable parameter monster (r_idx: %d) was not reset upon loading"),m_ptr->r_idx);
	}


	//ぬえ専用処理　フロアに入った直後の変身判定
	//v2.0.5 この判定をprocess_player()に移す。ここではフラグだけ立てる
	//if(p_ptr->pclass == CLASS_NUE)
	//{
	//	check_nue_undefined();
	//}
	flag_nue_check_undefined = TRUE;

	/*:::Mega Hack - Extraモードダンジョン内建物の名前やできることを設定　セーブ＆ロードに対応するためフロアに降り立った直後に呼ぶ*/
	if(EXTRA_MODE && dun_level)
	{
		init_extra_dungeon_buildings();
	}

	/* Refresh */
	Term_fresh();

	/*:::クエスト中でランクエでもオベロンでもJでもなくメインダンジョンクエストでもない場合、雰囲気感知処理*/
	/*:::クエストダンジョンに入ったとき「典型的なクエストのダンジョンのようだ」と表示させるための処理らしいが、*/
	/*:::それにしては条件式がややこしすぎる気がする。何か見落としているのか。*/
	if (quest_num && (is_fixed_quest_idx(quest_num) &&
	  //  !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_SERPENT) ||
	    !((quest_num == QUEST_TAISAI) ||(quest_num == QUEST_YUKARI) ||
	    !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling();

	if (p_ptr->inside_battle)
	{
		if (load_game)
		{
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else
		{
			msg_print(_("試合開始！", "Fight!"));
			msg_print(NULL);
		}
	}

	if(	flag_sumireko_wakeup)
	{
		flag_sumireko_wakeup = FALSE;
		msg_print(_("あなたは外界で目覚め、しばしの現実生活のあと再び眠り、幻想郷へ降り立った。",
                    "You wake up in the Outside world, go back to sleep after a day of your real life and return to Gensoukyou."));

	}

	//v1.1.43 経験値テーブルを変えたのでダンジョンのメインループ前に経験値チェックを入れておく
	check_experience();

	/*::: 吟遊詩人の霊的知覚に関する処理らしい*/
	//霊的知覚を歌い始めてからのターン数をmagic_num1[0]を加算することで管理しようとしてやっぱりやめた？？？
	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT))
		p_ptr->magic_num1[0] = MUSIC_DETECT;

	/* Hack -- notice death or departure */
	/*:::死亡かゲーム終了でここを出る*/
	if (!p_ptr->playing || p_ptr->is_dead) return;

	/* Print quest message if appropriate */
	///sys ランクエとダンジョンの主の処理
	if (!p_ptr->inside_quest && (dungeon_type == DUNGEON_ANGBAND))
	{
		quest_discovery(random_quest_number(dun_level));
		p_ptr->inside_quest = random_quest_number(dun_level);
	}
	if ((dun_level == d_info[dungeon_type].maxdepth) && d_info[dungeon_type].final_guardian && (CHECK_GUARDIAN_CAN_POP))
	{
		if (r_info[d_info[dungeon_type].final_guardian].max_num)
#ifdef JP
			msg_format("この階には%sの主である%sが棲んでいる。",
				   d_name+d_info[dungeon_type].name,
				   r_name+r_info[d_info[dungeon_type].final_guardian].name);
#else
			msg_format("%^s lives in this level as the keeper of %s.",
					   r_name+r_info[d_info[dungeon_type].final_guardian].name,
					   d_name+d_info[dungeon_type].name);
#endif
	}

	/*:::Hack - EXダンジョンの中ボス*/
	if (dungeon_type == DUNGEON_CHAOS)
	{
		monster_type *m_ptr_tmp;
		char temp_name[80];
		if(dun_level == 110 && r_info[MON_MORGOTH].max_num)
		{
			msg_format(_("この階には%sが棲んでいる。",
                        "%s lives on this level."),
				   r_name+r_info[MON_MORGOTH].name);
		}
		if(dun_level == 120 && r_info[MON_AZATHOTH].max_num)
		{
			msg_format(_("この階には%sが棲んでいる。",
                        "%s lives on this level."),
				   r_name+r_info[MON_AZATHOTH].name);
		}

	}
	//v1.1.27 結界ガード
	if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		set_mana_shield(FALSE,FALSE);
	}

	/*:::階に降りてきた直後は超隠密ではない？*/
	///class 忍者特殊処理
	if (!load_game && (p_ptr->special_defense & NINJA_S_STEALTH)) set_superstealth(FALSE);

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	hack_mind = TRUE;

	/*:::フロア到着直後、地上への帰還直後、アリーナ入場時に＠が最初に行動できるようにするための処理*/
	if (p_ptr->energy_need > 0 && !p_ptr->inside_battle &&
	    (dun_level || p_ptr->leaving_dungeon || p_ptr->inside_arena))
		p_ptr->energy_need = 0;

	///mod151104 「咲夜の世界」中特殊処理
	if (SAKUYA_WORLD)
	{
		p_ptr->energy_need = 0;
	}

	/*
	//バグでクエスト失敗したある人の救済処理
	if (difficulty == DIFFICULTY_NORMAL && p_ptr->prace == RACE_STRAYGOD && p_ptr->pclass == CLASS_MAID
		&& (quest[QUEST_OOGAME].status == QUEST_STATUS_FAILED || quest[QUEST_OOGAME].status == QUEST_STATUS_FAILED_DONE))
	{
		complete_quest(QUEST_OOGAME);

	}
	*/



	/* Not leaving dungeon */
	p_ptr->leaving_dungeon = FALSE;

	/* Initialize monster process */
	/*mproc_list[][]/mproc_max[]にフロアのモンスターの一時ステータス情報を格納*/
	mproc_init();

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		/*:::フロアのモンスターが最大に近くなったときモンスターを強制圧縮*/
		if ((m_cnt + 32 > max_m_idx) && !p_ptr->inside_battle) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		/*:::死亡済みモンスターが増えたときリストを整理*/
		if ((m_cnt + 32 < m_max) && !p_ptr->inside_battle) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		/*:::フロアのアイテム数が限界近くになったとき強制圧縮*/
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		/*:::フロアの無効なアイテムが増えたらリストを整理*/
		if (o_cnt + 32 < o_max) compact_objects(0);


		/* Process the player */
		process_player();

		process_upkeep_with_speed();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;


		/* Process the world */
		process_world();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Count game turns */
		turn++;

		/*:::wild_regenは初期値20 dungeon_turn_limitはbirth時に決められる。*/
		if (dungeon_turn < dungeon_turn_limit)
		{
			if (!p_ptr->wild_mode || wild_regen) dungeon_turn++;
			/*:::全体マップではdungeon_turnは増えにくいらしい。*/
			else if (p_ptr->wild_mode)
			{
				//v1.1.59 サニーに日光蓄積の特技をつけたので日光蓄積時にdungeon_turnを消費するようにする
				if (p_ptr->pclass == CLASS_SUNNY && p_ptr->magic_num1[0] < SUNNY_CHARGE_SUNLIGHT_MAX && is_daytime())
					dungeon_turn++;
				if(!(turn % ((MAX_HGT + MAX_WID) / 2))) dungeon_turn++;
			}
		}

		prevent_turn_overflow();

		if (wild_regen) wild_regen--;
	}/*:::メインループ終了　＠は死ぬか階移動するかゲーム終了した*/

	/* Inside a quest and non-unique questor? */
	/*:::特定非ユニーク殲滅クエストだった場合モンスターからフラグを外す*/
	if (quest_num && !(r_info[quest[quest_num].r_idx].flags1 & RF1_UNIQUE))
	{
		/* Un-mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 &= ~RF1_QUESTOR;
	}

	//セーブ終了でも死亡でもないとき。通常の階移動をする前の処理
	/* Not save-and-quit and not dead? */
	if (p_ptr->playing && !p_ptr->is_dead)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;

		//ヘカーティア特殊処理
		for(i=0;i<3;i++) hecatia_hp[i] = 5600;

		//Exモード用建物内カウントをリセット
		for(i=0;i<EX_BUILDINGS_PARAM_MAX;i++) ex_buildings_param[i] = 0;

		//赤蛮奇特技のアイテム探索を中断
		if (p_ptr->tim_rokuro_head_search)
		{
			rokuro_head_search_item(0, TRUE);
		}

		//響子の記憶している魔法をクリア。アイテムカードから使えるよう独立変数にした
		kyouko_echo(TRUE,0);
		//パルスィのダメージカウントをクリア
		parsee_damage_count = 0L;
		//モンスター「八尺さま」のターゲット
		hassyakusama_target_idx = 0;

		//v1.1.58
		flag_update_floor_music = TRUE;

		//v1.1.87 EXTRAモードのとき、フロアを移動するたびにアビリティカードの販売リスト、交換回数、高騰度を更新
		if (EXTRA_MODE)
		{
			ability_card_trade_count = 0;
			set_abilitycard_price_rate();
			make_ability_card_store_list();
		}

		//v2.0.5 EXTRAではたてのモンスター捜索をしている場合そのフロアを通過したらリセット
		if (EXTRA_MODE && dun_level >= p_ptr->hatate_mon_search_dungeon % 1000)
		{
			if (p_ptr->wizard) msg_print("hatate reset");
			p_ptr->hatate_mon_search_dungeon = 0;
			p_ptr->hatate_mon_search_ridx = 0;
		}

		//v2.0.19 養蜂家は難易度EXTRAのときフロア移動に伴い蜂蜜を得る
		if (EXTRA_MODE && p_ptr->pclass == CLASS_BEEKEEPER)
		{
			add_honey();
		}

		//v1.1.13 公転周期の罠の解除
		if(p_ptr->special_flags & SPF_ORBITAL_TRAP)
		{
			msg_print(_("公転周期の罠が効力を失った。",
                        "The orbital trap is no longer effective."));
			p_ptr->special_flags &= ~(SPF_ORBITAL_TRAP);
		}
		//v1.1.35 ネムノ縄張りクリア
		if(p_ptr->pclass == CLASS_NEMUNO)
		{
			erase_nemuno_sanctuary(FALSE,FALSE);
		}

		//v1.1.93 ミケの「このフロアですでに客を呼んだ」フラグをクリア
		if (p_ptr->pclass == CLASS_MIKE) p_ptr->magic_num2[100] = 0;

		///mod140423
		if(p_ptr->silent_floor)
		{
			p_ptr->silent_floor=0;
			msg_print(_("ダンジョンから異質な魔力が消え去った。",
                        "Extraordinary magical power vanishes from the dungeon."));
		}
		///mod140502
		if(p_ptr->lucky)
		{
				set_tim_lucky(0,TRUE);
		}
		///mod150304 束縛系特技をクリア
		//v1.1.95 移動禁止系特技をMTIMED2_NO_MOVEに統合したので処理削除
//		if(p_ptr->pclass == CLASS_KOMACHI || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_REMY || p_ptr->pclass == CLASS_MAMIZOU)

		//マミゾウが変身させているモンスターの解除
		if(p_ptr->pclass == CLASS_MAMIZOU)
		{
			if(p_ptr->magic_num1[0]) set_tim_general(0,TRUE,0,0);
		}
		//v1.1.17 純狐技リセットと一時能力解除
		if(p_ptr->pclass == CLASS_JUNKO)
		{
			junko_make_nameless_arts();
		}
		///mod141101 さとりの魔法リストをクリア
		if(p_ptr->pclass == CLASS_SATORI)
		{
			make_magic_list_satori(TRUE);
		}
		//v1.1.95 パルスィの藁人形カウントをクリア
		if (p_ptr->pclass == CLASS_PARSEE)
		{
			p_ptr->magic_num1[4] = 0;
			p_ptr->magic_num2[4] = 0;
		}

		//v2.1.2 チミ　フロアの自然地形をカウントするフラグをON
		if (p_ptr->pclass == CLASS_CHIMI)
		{
			flag_chimi_need_count_feat = TRUE;
		}

		//v2.1.5 阿梨夜　恒久の冬が発動していたら解除
		if (ARIYA_STOP)
		{
			p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
		}



		//v1.1.77 お燐(専用性格)の追跡対象モンスターをクリア
		/* v2.0.15 追跡対象をMFLAG_SPECIALで管理することにしたのでクリアする必要がなくなった
		if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN) && p_ptr->magic_num1[0])
		{
			msg_print(_("追跡を中断した。", "Your tracking is interrupted."));
			p_ptr->magic_num1[0] = 0;
		}
		*/

		//妹紅インペリシャブルシューティングを解除
		if(p_ptr->pclass == CLASS_MOKOU && p_ptr->magic_num2[0])
		{
			p_ptr->magic_num2[0] = 0;
		}
		//ルナチャ能力解除
		if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] || p_ptr->tim_general[1]))
		{
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);
		}
		if(p_ptr->pclass == CLASS_SANAE && p_ptr->magic_num1[1])
		{
			msg_print(_("フロアから神の気配が消えた。", "Divine presence vanishes from this floor."));
			p_ptr->magic_num1[1] = 0;
		}
		if(p_ptr->pclass == CLASS_KOISHI && p_ptr->magic_num1[0])
		{
			msg_print(_("周囲の人影が消えた。", "The figures surrounding you disappear."));
			p_ptr->magic_num1[0] = 0;
		}
		///mod150516 天子の要石をクリア
		if(p_ptr->pclass == CLASS_TENSHI)
		{
			p_ptr->magic_num1[0] = 0;
			p_ptr->magic_num1[1] = 0;
			p_ptr->magic_num1[2] = 0;

		}
		//ドレミー変身解除
		if(p_ptr->pclass == CLASS_DOREMY && IS_METAMORPHOSIS)
		{
			p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
			p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);
			set_mimic(0,0,TRUE);
		}
		//v1.1.48 紫苑憑依解除..はフロアを出てもされないことにした。そうでもしないと弱すぎる。
		/*
		if (SHION_POSSESSING)
		{
			p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
			p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);
			set_mimic(0, 0, TRUE);
		}
		*/
		//雛の呪い中断
		if (p_ptr->pclass == CLASS_HINA)
		{
			if(p_ptr->magic_num1[0]) set_tim_general(0,TRUE,0,0);
			if(p_ptr->special_defense & SD_HINA_NINGYOU)
			{
				msg_print(_("厄を撒き散らすのを止めた。", "You stop spreading misfortune."));
				p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		if (p_ptr->pclass == CLASS_YACHIE)
		{
			if (p_ptr->magic_num1[0])
			{
				msg_print(_("買収工作を止めた。", "You stop acquisition."));
				p_ptr->magic_num1[0] = 0;
				p_ptr->redraw |= PR_STATUS;
			}
		}


		///mod140517 階移動したとき、EPHEMERAフラグを持つ配下の消滅処理 所持しているアイテムは落とす
		//ドッペルゲンガーも消す
		//v1.1.78 RF2_PHANTOM持ちのモンスターも消す
		for (i = 1; i < m_max; i++)
		{
			char m_name[80];
			int dist_tmp;
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];

			if(!(m_ptr->mflag & MFLAG_EPHEMERA) && !(r_ptr->flags2 & RF2_PHANTOM)) continue;

			monster_desc(m_name, m_ptr, 0);
			if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
				msg_format(_("%sは元の武器に戻った。", "%s returns to its original weapon form."), m_name);
			else if(m_ptr->r_idx == MON_YOUMU )
				msg_format(_("あなたの半霊は元の霊体に戻った。", "Your ghost half returns to its original form."));
			else
				msg_format(_("%sは消えた。", "%s disappears."), m_name);
			monster_drop_carried_objects(m_ptr);
			delete_monster_idx(i);
		}
		//輝夜(主)も消える。
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if(m_ptr->r_idx != MON_MASTER_KAGUYA) continue;
			delete_monster_idx(i);
		}
		//ドッペルゲンガーも消す
		for (i = 1; i < m_max; i++)
		{
			monster_race *r_ptr;
			char m_name[80];
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];
			if(!(r_ptr->flags7 & RF7_DOPPELGANGER)) continue;

			monster_desc(m_name, m_ptr, 0);
			msg_format(_("あなたのドッペルゲンガーは消えた。", "Your doppelganger disappears."), m_name);

			monster_drop_carried_objects(m_ptr);
			delete_monster_idx(i);
		}

		/*
		 * Maintain Unique monsters and artifact, save current
		 * floor, then prepare next floor
		 */
		leave_floor();

		/* Forget the flag */
		reinit_wilderness = FALSE;
	}

	/* Write about current level on the play record once per level */
	write_level = TRUE;
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
/*:::user-win.prf,マクロ設定ファイル、自動拾いファイル読み込み*/
static void load_all_pref_files(void)
{
	char buf[1024];


	/* Access the "user" pref file */
	sprintf(buf, "user.prf");

	/* Process that file */
	process_pref_file(buf);

	/*:::user-win.prfを開いている。色設定関連らしい*/
	/* Access the "user" system pref file */
	sprintf(buf, "user-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/*:::種族ごとのprfファイル？それらしいファイルはない*/
	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/*:::クラスごとのprfファイル？それらしいファイルはない*/
	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", cp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/*:::@名.prf マクロ設定ファイル読み込みか*/
	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Access the savefile pref file */
	sprintf(buf, "%s.prf", savefile_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

		/* Process that file */
		process_pref_file(buf);
	}


	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
 * Extract option variables from bit sets
 */
/*:::ロードしたoption_flag[]をoption_info[]を参照し各オプション変数に展開している*/
///mod140406 初期設定から外したオプションをここでdifficulty参照して適用し直すことにした。
void extract_option_vars(void)
{
	int i;

	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}

			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}


	///mod140307 難易度による自動オプション選択
	///mod140406 ロード時反映されていなかったのでplayer_birth()から移動してきた
	switch(difficulty)
	{
	case DIFFICULTY_EASY:
		easy_band = TRUE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_NORMAL:
		easy_band = FALSE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_HARD:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_LUNATIC:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = TRUE;
		ironman_nightmare = TRUE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_EXTRA:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = TRUE;
		break;
	}




}


/*
 * Determine bounty uniques
 */
///del131221
#if 0
void determine_bounty_uniques(void)
{
	int          i, j, tmp;
	monster_race *r_ptr;

	get_mon_num_prep(NULL, NULL);
	for (i = 0; i < MAX_KUBI; i++)
	{
		while (1)
		{
			kubi_r_idx[i] = get_mon_num(MAX_DEPTH - 1);
			r_ptr = &r_info[kubi_r_idx[i]];

			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			if (!(r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON))) continue;

			if (r_ptr->rarity > 100) continue;

			if (no_questor_or_bounty_uniques(kubi_r_idx[i])) continue;

			for (j = 0; j < i; j++)
				if (kubi_r_idx[i] == kubi_r_idx[j]) break;

			if (j == i) break;
		}
	}

	/* Sort them */
	for (i = 0; i < MAX_KUBI - 1; i++)
	{
		for (j = i; j < MAX_KUBI; j++)
		{
			if (r_info[kubi_r_idx[i]].level > r_info[kubi_r_idx[j]].level)
			{
				tmp = kubi_r_idx[i];
				kubi_r_idx[i] = kubi_r_idx[j];
				kubi_r_idx[j] = tmp;
			}
		}
	}
}
#endif

/*
 * Determine today's bounty monster
 * Note: conv_old is used if loaded 0.0.3 or older save file
 */
/*:::本日の賞金首決定　削除予定*/
///sysdel
#if 0
void determine_today_mon(bool conv_old)
{
	int max_dl = 3, i;
	bool old_inside_battle = p_ptr->inside_battle;
	monster_race *r_ptr;

	if (!conv_old)
	{
		for (i = 0; i < max_d_idx; i++)
		{
			if (max_dlv[i] < d_info[i].mindepth) continue;
			if (max_dl < max_dlv[i]) max_dl = max_dlv[i];
		}
	}
	else max_dl = MAX(max_dlv[DUNGEON_ANGBAND], 3);

	p_ptr->inside_battle = TRUE;
	get_mon_num_prep(NULL, NULL);

	while (1)
	{
		today_mon = get_mon_num(max_dl);
		r_ptr = &r_info[today_mon];

		if (r_ptr->flags1 & RF1_UNIQUE) continue;
		if (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2)) continue;
		if (r_ptr->flags2 & RF2_MULTIPLY) continue;
		if ((r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) != (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) continue;
		if (r_ptr->level < MIN(max_dl / 2, 40)) continue;
		if (r_ptr->rarity > 10) continue;
		break;
	}

	p_ptr->today_mon = 0;
	p_ptr->inside_battle = old_inside_battle;
}
#endif

/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
/*:::ゲームルーチンの一番上　新規時TRUE,ファイル開いたときFALSE*/
void play_game(bool new_game)
{
	int i;
	bool load_game = TRUE;
	bool init_random_seed = FALSE;
	int tmp_data;

	//v1.1.25 プレイ時間がバグる報告あり。
	//原因不明だがstart_timeが未設定のときにupdate_playtime()が行われているらしい。
	//とりあえず最初の最初にstart_timeを設定するようにしておく
	start_time = time(NULL) - 1;

/*:::データ中継サーバ？どのみち今は無効らしい*/
#ifdef CHUUKEI
	if (chuukei_client)
	{
		reset_visuals();
		browse_chuukei();
		return;
	}

	else if (chuukei_server)
	{
		prepare_chuukei_hooks();
	}
#endif

	///\131117 sysdel
	///mod150224 ムービー復活？
	if (browsing_movie)
	{
		reset_visuals();
		browse_movie();
		return;
	}
	///race 獣人開始時変異用
	hack_startup = FALSE;

	/* Hack -- Character is "icky" */
	/*:::キャラクターが初期化中でゲーム進行がまだできないときTRUE?*/
	character_icky = TRUE;

	/* Make sure main term is active */
	/*:::重要ルーチンらしい。そのまま使う。*/
	Term_activate(angband_term[0]);

	/* Initialise the resize hooks */
	/*:::これもよく分からんがそのまま使う。*/
	angband_term[0]->resize_hook = resize_map;

	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if (angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);


	/* Attempt to load */
	/*:::セーブファイルを読みに行ってる。新規時も一度入る？*/
	/*:::データがロードされたらcharacter_loaded=TRUE*/
	if (!load_player())
	{
		/* Oops */
#ifdef JP
quit("セーブファイルが壊れています");
#else
		quit("broken savefile");
#endif

	}

#ifdef NEW_PLAYER_LOG
	load_player_log(savefile, p_log_ptr);
#endif

	/* Extract the options */
	/*:::オプションの設定情報をフラグ用変数に入れなおしているらしい*/
	extract_option_vars();

	/* Report waited score */
	/*:::スコア送信を待機していた場合ここへ*/
	if ( p_ptr->wait_report_score)
	{
		char buf[1024];
		bool success;

#ifdef JP
		if (!get_check_strict("待機していたスコア登録を今行ないますか？", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Do you register score now? ", CHECK_NO_HISTORY))
#endif
			quit(0);

		//start_timeをリセット
		start_time = time(NULL);

		//v1.1.25 -hack- スコア計算フラグを再設定
		if(p_ptr->chp >= 0 && p_ptr->total_winner)
			finish_the_game = TRUE;

		/* Update stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Update stuff */
		update_stuff();

		p_ptr->is_dead = TRUE;

		//v1.1.25 スコアサーバに送るデータをis_dead状態にするために追加
		if (!save_player()) msg_print(_("セーブ失敗！", "Saving failed!"));


		/* No suspending now */
		signals_ignore_tstp();

		/* Hack -- Character is now "icky" */
		character_icky = TRUE;

		/* Build the filename */
		//path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");
		path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, SCORES_RAW_FILE);

		/* Open the high score file, for reading/writing */
		highscore_fd = fd_open(buf, O_RDWR);
		/* Handle score, show Top scores */
		success = send_world_score(TRUE);


#ifdef JP
		if (!success && !get_check_strict("スコア登録を諦めますか？", CHECK_NO_HISTORY))
#else
		if (!success && !get_check_strict("Do you give up score registration? ", CHECK_NO_HISTORY))
#endif
		{
#ifdef JP
			//v1.1.25 スコアサーバに送るデータをis_dead状態にしてたのをまた戻す
			p_ptr->is_dead = FALSE;
			if (!save_player()) msg_print("セーブ失敗！");
			prt("引き続き待機します。", 0, 0);
#else
			prt("standing by for future registration...", 0, 0);
#endif

			(void)inkey();
		}
		else
		{
			p_ptr->wait_report_score = FALSE;
			top_twenty();
#ifdef JP
			if (!save_player()) msg_print("セーブ失敗！");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		/* Shut the high score file */
		(void)fd_close(highscore_fd);

		/* Forget the high score fd */
		highscore_fd = -1;

		/* Allow suspending now */
		signals_handle_tstp();

		quit(0);
	}
	/*:::新規時TRUE*/
	creating_savefile = new_game;

	/* Nothing loaded */
	/*:::新規時、またはロード失敗時にここに来る*/
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Prepare to init the RNG */
		init_random_seed = TRUE;

		/* Initialize the saved floors data */
		init_saved_floors(FALSE);
	}

	/* Old game is loaded.  But new game is requested. */
	/*:::ここに来るのはどういう条件だろう？*/
	else if (new_game)
	{
		/* Initialize the saved floors data */
		init_saved_floors(TRUE);
	}

	/* Process old character */
	/*:::ロードした＠名が現在の機種に適合しているかチェックしてるらしい*/
	if (!new_game)
	{
		/* Process the player name */
		process_player_name(FALSE);
//testmsg(format("name:%s",player_name));
	}

	/* Init the RNG */
	/*:::新規時、ランダム値の種をセット*/
	if (init_random_seed)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Seed the RNG */
		Rand_state_init(seed);
	}

	/* Roll new character */
	if (new_game)
	{

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Start in town */
		dun_level = 0;
		p_ptr->inside_quest = 0;
		p_ptr->inside_arena = FALSE;
		p_ptr->inside_battle = FALSE;

		/*:::よくわからんが日記記述用？*/
		write_level = TRUE;

		/* Hack -- seed for flavors */
		/*:::何がしかのランダム要素に使うらしい？*/
		seed_flavor = randint0(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = randint0(0x10000000);

		/* Roll up a new character */
		/*:::キャラクターメイク　最重要*/
		///class race seikaku system
		player_birth();

		/*:::プレイ時間とセーブ回数カウントのリセット*/
		counts_write(2,0);
		p_ptr->count = 0;

		load = FALSE;
		/*:::賞金首リストの生成　削除予定*/
		///del131221　賞金首リスト
		//determine_bounty_uniques();
		/*:::本日の賞金首の設定　削除予定*/
		///sysdel 賞金首
		//determine_today_mon(FALSE);

		/* Initialize object array */
		/*:::落ちてるアイテムを全て削除*/
		wipe_o_list();


	}
	else
	{
		write_level = FALSE;

#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ----ゲーム再開----");
#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ---- Restart Game ----");
#endif
#if 0
/*
 * 1.0.9 以前はセーブ前に p_ptr->riding = -1 としていたので、再設定が必要だった。
 * もう不要だが、以前のセーブファイルとの互換のために残しておく。
 */
		///sysdel
		if (p_ptr->riding == -1)
		{
			p_ptr->riding = 0;
			for (i = m_max; i > 0; i--)
			{
				if (player_bold(m_list[i].fy, m_list[i].fx))
				{
					p_ptr->riding = i;
					break;
				}
			}
		}
#endif
		///mod150315 妖精特技
		//v1.0.50より前で開始して妖精でプレイ中にバージョンアップした場合ここに来るはず
		//妖精特技を取得し、クイックスタート用にprevious_char.scに保存する
		if(p_ptr->prace == RACE_FAIRY && !CHECK_FAIRY_TYPE) get_fairy_type(TRUE);



		//v1.1.31c 妖精特技不具合対応
		//birth()で特技設定したときのミスで特技idxをクイックスタート用に保存したp_ptr->scとprev-scが初期値の50のままになっているので設定し直し
		if(p_ptr->prace == RACE_FAIRY && previous_char.prace == RACE_FAIRY &&
			CHECK_FAIRY_TYPE && p_ptr->sc == 50)
		{

			p_ptr->sc = p_ptr->race_multipur_val[0];
			previous_char.sc = p_ptr->sc;
		}


	}
	/*:::ここから先は新規時、ロード時とも共通*/

	creating_savefile = FALSE;
	/*:::様々なフラグのリセット*/
	p_ptr->teleport_town = FALSE;
	p_ptr->sutemi = FALSE;
	world_monster = FALSE;
	now_damaged = FALSE;
	now_message = 0;
	start_time = time(NULL) - 1;
	record_o_name[0] = '\0';

	/* Reset map panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Sexy gal gets bonus to maximum weapon skill of whip */
	/*:::なぜキャラメイク時に書かないのか解らない。どちらにしても削除予定*/
	///del131223 seikaku
	//if (p_ptr->pseikaku == SEIKAKU_SEXY)
	//	s_info[p_ptr->pclass].w_max[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

	///mod141005 ユニーククラスの2Pキャラの色を変えてみる
	for (i = 1; i < max_r_idx; i++) if(monster_is_you(i)) r_info[i].d_attr = (r_info[i].d_attr)%15+1;

	/* Fill the arrays of floors and walls in the good proportions */
	/*:::グローバル変数floor_typeとfill_typeにダンジョンの種類による地形出現率に応じて地形データを100回格納している。*/
	/*:::新規の時は「荒野」として実行される？　どうもここでこの関数が呼ばれる理由が分からない。ロード時にはどうなるのか？*/
	set_floor_and_wall(dungeon_type);

	/* Flavor the objects */
	/*:::薬などの未鑑定時名をランダムにつけるのだと思う*/
	/*:::ロード時もランダムのシードが保持されているので同じ並びになる*/
	flavor_init();

	//v1.1.85 ★が出ないオプション処理　鉄獄1階で★が床落ちしてたそう。
	//これまでchange_floor()の後にこの処理を実行してたが、EXTRAだと地下一階生成のほうが早い。盲点だった。
	//コードあまり読み直さず雑にコピー。
	//TODO:元々あった下の方の同じ処理が不要かどうか調べて消しとく
	if (ironman_no_fixed_art)
	{
		int a_idx;
		for (a_idx = 1; a_idx < max_a_idx; a_idx++)	a_info[a_idx].cur_num = 1;
	}


	/* Flash a message */
#ifdef JP
prt("お待ち下さい...", 0, 0);
#else
	prt("Please wait...", 0, 0);
#endif


	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	/*:::これはウィザードモード突入のときtrueになるのか？検索してもTrueになる処理が見当たらないが*/
	/*:::どうもここはコマンドラインからデバッグオプション付きでhengを立ち上げたとき来るらしい？*/
	if (arg_wizard)
	{
		if (enter_wizard_mode())
		{
			p_ptr->wizard = TRUE;

			if (p_ptr->is_dead || !py || !px)
			{
				/* Initialize the saved floors data */
				init_saved_floors(TRUE);

				/* Avoid crash */
				p_ptr->inside_quest = 0;

				/* Avoid crash in update_view() */
				py = px = 10;
			}
		}
		else if (p_ptr->is_dead)
		{
			quit("Already dead.");
		}
	}

	/* Initialize the town-buildings if necessary */
	/*:::荒野か町に居てクエスト中でない場合、*/
	if (!dun_level && !p_ptr->inside_quest)
	{
		/* Init the wilderness */
		/*:::ワールドマップの読み込み*/
		process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);
		/* Init the town */
		init_flags = INIT_ONLY_BUILDINGS;
		/*:::町の読み込み t_pref.txtを読んだ後はp_ptr->town_numを参照しそれぞれのファイルを読む*/
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);

		select_floor_music(); //v1.1.58

	}
//testmsg(format("character_dungeon:%x",character_dungeon));
	/*:::エクストラモードは鉄獄1階から開始*/
	if(EXTRA_MODE && new_game)
	{
		dungeon_type = DUNGEON_ANGBAND;
		p_ptr->enter_dungeon = TRUE;
		dun_level = 1;
		prepare_change_floor_mode(CFM_RAND_PLACE | CFM_FIRST_FLOOR);
		change_floor();

	}

	/* Generate a dungeon level if needed */
	/*:::フロアを生成。新規時にはここに来て街が生成されるはずだが、＠の初期位置はどう決まるんだろう*/
	else if (!character_dungeon)
	{
		change_floor();
	}

	else
	{
		/* HACK -- Restore from panic-save */
		if (p_ptr->panic_save)
		{
//testmsg(format("py:%d px:%d cur_wid:%d cur_hgt:%d",py,px,cur_wid,cur_hgt));
			/* No player?  -- Try to regenerate floor */
			if (!py || !px)
			{
#ifdef JP
				msg_print("プレイヤーの位置がおかしい。フロアを再生成します。");
#else
				msg_print("What a strange player location.  Regenerate the dungeon floor.");
#endif
				change_floor();
			}

			/* Still no player?  -- Try to locate random place */
			if (!py || !px) py = px = 10;

			/* No longer in panic */
			p_ptr->panic_save = 0;
		}
	}

	/* Character is now "complete" */
	character_generated = TRUE;


	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	if (new_game)
	{
		char buf[80];

#ifdef JP
		sprintf(buf, "%sに降り立った。", map_name());
#else
		sprintf(buf, "You are standing in the %s.", map_name());
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	}


	/* Start game */
	p_ptr->playing = TRUE;

	/* Reset the visual mappings */
	/*:::画面の表示設定を初期値に戻す？*/
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files();

	/*::: -Hack- ★が出ないオプション処理　全ての★を出現済みにする ゲーム開始後に★が追加された場合に備えロード時にもこの処理をする。*/
	if(ironman_no_fixed_art)
	{
		int a_idx;
		for (a_idx = 1; a_idx < max_a_idx; a_idx++)	a_info[a_idx].cur_num = 1;
	}


	/* Give startup outfit (after loading pref files) */
	/*:::ゲーム開始時、持っているアイテムを決める処理など*/
	if (new_game)
	{
		//キャラメイク時の初期所持アイテムを得る
		player_outfit();

		///mod150315 妖精特技
		/*::: -Hack- 妖精クイックスタート */
		//開始時に技を持っていない場合、クイックスタートなのでscに保存されているはずの特技値をコピー
		//scが50の場合、旧バージョンからのクイックスタートでまだ特技付与処理が行われていないので再付与処理
		if(p_ptr->prace == RACE_FAIRY && !CHECK_FAIRY_TYPE && !(cp_ptr->flag_only_unique))
		{
			//if(p_ptr->sc == 50) v1.1.31c レアリティ100のダミーが選択される不具合があったので条件式変更
			if(p_ptr->sc <= FAIRY_TYPE_MAX && fairy_table[p_ptr->sc].rarity >= 100 )
			{
				get_fairy_type(TRUE);
			}
			else
			{
				if(p_ptr->sc > FAIRY_TYPE_MAX || p_ptr->sc < 1 )
				{
					msg_format(_("ERROR:p_ptr->scに保存されているはずの妖精特技設定値がおかしい(%d)",
                                "ERROR: Weird fairy special ability value (%d) that had to be saved in p_ptr->sc"),p_ptr->sc);
					get_fairy_type(TRUE);
				}
				else
				{
					p_ptr->race_multipur_val[0] = p_ptr->sc;
				}
			}
		}
		//v1.1.70 動物霊クイックスタートの所属勢力決定
		if (p_ptr->prace == RACE_ANIMAL_GHOST && !CHECK_ANIMAL_GHOST_STRIFE)
		{
			get_animal_ghost_family(FALSE);
		}

	}

	/* React to changes */
	/*:::詳細不明。ウィンドウ関連？*/
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Window stuff */
	/*:::サブウィンドウに関する設定？*/
	window_stuff();


	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	/*:::何らかの理由でセーブ時に死んだときの処理？死んだときに変愚蛮怒を強制終了したらたぶんここに来る？*/
	if (p_ptr->chp < 0) p_ptr->is_dead = TRUE;

	if (p_ptr->prace == RACE_ANDROID) calc_android_exp();

	/*:::騎兵・魔獣使いニューゲーム時の馬生成処理*/
	///class mon
	if (new_game && !EXTRA_MODE && ((p_ptr->pclass == CLASS_CAVALRY) || (p_ptr->pclass == CLASS_BEASTMASTER)))
	{
		monster_type *m_ptr;
		int pet_r_idx = ((p_ptr->pclass == CLASS_CAVALRY) ? MON_HORSE : MON_YASE_HORSE);
		monster_race *r_ptr = &r_info[pet_r_idx];
		place_monster_aux(0, py, px - 1, pet_r_idx,
				  (PM_FORCE_PET | PM_NO_KAGE));
		m_ptr = &m_list[hack_m_idx_ii];
		m_ptr->mspeed = r_ptr->speed;
		m_ptr->maxhp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->max_maxhp = m_ptr->maxhp;
		m_ptr->hp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->energy_need = ENERGY_NEED() + ENERGY_NEED();
	}

	/*:::Mega Hack - 輝夜クエスト進行度に合わせ、該当アイテムのQUESTITEMフラグを外して通常生成されるようにする*/
	///mod160721 輝夜プレイ時は全てQUESTITEMのまま
	if(kaguya_quest_level < KAGUYA_QUEST_MAX && quest[QUEST_KAGUYA].status < QUEST_STATUS_COMPLETED && p_ptr->pclass != CLASS_KAGUYA)
	{
		a_info[kaguya_quest_art_list[kaguya_quest_level]].gen_flags &= ~(TRG_QUESTITEM);
 	}
	//v1.1.26 輝夜クエスト完了後は適当にどれか一つフラグ外す。起動のたびに変わらないようにする必要があるのでseed_flavorを使う。
	//これだと輝夜クエの最後のアイテムの納品を済ませたキャラの次以降の起動時にもう一つ神宝が生成されてしまう可能性があるが、まあ大した問題にはならないか。
	else if(kaguya_quest_level == KAGUYA_QUEST_MAX && p_ptr->pclass != CLASS_KAGUYA)
	{
		a_info[kaguya_quest_art_list[(seed_flavor % KAGUYA_QUEST_MAX)]].gen_flags &= ~(TRG_QUESTITEM);
 	}

	///mod160310 Extraモードでは一部を除きモンスターのQUESTORフラグを外す
	if(EXTRA_MODE)
	{
		for (i = 1; i < max_r_idx; i++)
		{
			//v1.1.92 菫子(闘技場専用)も出さないよう追加

			if(i == MON_TAISAI || i == MON_OBERON || i== MON_SERPENT || i == MON_MORGOTH || i == MON_AZATHOTH || i == MON_FLAN || i == MON_SUMIREKO_2
			|| i == MON_GOLFIMBUL || i == MON_HECATIA1 || i == MON_HECATIA2 || i == MON_HECATIA3 || i == MON_MIKO || i == MON_ERIC)
			{
				r_info[i].flags1 |= (RF1_QUESTOR);
			}
			else
			{
				r_info[i].flags1 &= ~(RF1_QUESTOR);
			}
		}
	}

	//デバッグ、データ救出用

	if(0)
	{
			Term_clear();
			put_str(_("***強制人里帰還***", "*** forced return to the village ***"), 17, 40);
			(void)inkey();
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
			(void)set_food(PY_FOOD_MAX - 1);
			reset_tim_flags();
			dun_level = 0;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			leave_quest_check();
			leaving_quest = 0;
			p_ptr->inside_quest = 0;
			if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
			dungeon_type = 0;
			py = 33;
			px = 180;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->wild_mode = FALSE;
			wipe_m_list();
			leave_floor();
			change_floor();

	}

	/*:::店や家のアイテムをまとめ、価格と種類順に整列する*/
	(void)combine_and_reorder_home(STORE_HOME);
	(void)combine_and_reorder_home(STORE_MUSEUM);

	select_floor_music(); //v1.1.58

	/* Process */
	/*:::メインルーチン*/
	while (TRUE)
	{

		/* Process the level */
		/*:::今の階での処理　階移動、死亡、ゲーム終了したら出る*/
		dungeon(load_game);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		character_xtra = FALSE;

		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);


		/* Forget the lite */
		forget_lite();

		/* Forget the view */
		forget_view();

		/* Forget the view */
		clear_mon_lite();

		/* Handle "quit and save" */
		if (!p_ptr->playing && !p_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		if (!p_ptr->is_dead) wipe_m_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		load_game = FALSE;

		/* Accidental Death */
		/*:::＠が引退や切腹でなく死んだとき　ここではアリーナとウィザードモードの復活だけ処理*/

		if (p_ptr->playing && p_ptr->is_dead)
		{
			//v1.1.51 新アリーナ実装に伴い書き換え
#if 0
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;
				if (p_ptr->arena_number > MAX_ARENA_MONS)
					p_ptr->arena_number++;
				else
					p_ptr->arena_number = -1 - p_ptr->arena_number;
				p_ptr->is_dead = FALSE;
				p_ptr->chp = 0;
				p_ptr->chp_frac = 0;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags();

				/* Leave through the exit */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_RAND_CONNECT);

				/* prepare next floor */
				leave_floor();
			}
#endif
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;

				p_ptr->is_dead = FALSE;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags();
				//v1.1.55 敗北時のHPを全快でなくアリーナに入ったときに戻す
				p_ptr->chp = MIN(nightmare_record_hp,p_ptr->mhp);
				p_ptr->chp_frac = 0;

				//v1.1.54
				if (p_ptr->food < PY_FOOD_ALERT) p_ptr->food = PY_FOOD_ALERT - 1;

				/* Leave through the exit */

				//敗北後に再挑戦したらモンスターの配置が同じになってるのでCFM_NO_RETURNに変えた
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

				/* prepare next floor */
				leave_floor();
			}
			else if (p_ptr->inside_quest == QUEST_MIYOI)
			{
				p_ptr->is_dead = FALSE;
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_poisoned(0);
				(void)set_afraid(0);
				(void)set_paralyzed(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);
				set_alcohol(0);
				p_ptr->chp = p_ptr->mhp;
				p_ptr->word_recall = 0;
				p_ptr->redraw |= (PR_STATUS);
				leave_quest_check();
				p_ptr->inside_quest = 0;
				dun_level = 0;
				p_ptr->leaving = TRUE;
				if(p_ptr->pclass == CLASS_DOREMY)
					msg_print(_("あなたは悪夢から離脱した。", "You escape from the nightmare."));
				else
					msg_print(_("はっ！夢だったか。", "Ha! Was that just a dream?"));
			}
			else
			{
				/* Mega-Hack -- Allow player to cheat death */
#ifdef JP
				if ((p_ptr->wizard || cheat_live || difficulty == DIFFICULTY_EASY) && get_check("コンティニューしますか? "))
#else
				if ((p_ptr->wizard || cheat_live) && !get_check("Continue? "))
#endif
				{
					//v1.1.70 コンティニューしたときにSCと年齢をリセットする処理を削除
					/* Mark social class, reset age, if needed */
					//if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

					/* Increase age */
					p_ptr->age++;

					/* Mark savefile */
					if(difficulty != DIFFICULTY_EASY) p_ptr->noscore |= 0x0001;

					msg_print(NULL);

					/* Restore hit points */
					p_ptr->chp = p_ptr->mhp;
					p_ptr->chp_frac = 0;

					if (p_ptr->pclass == CLASS_MAGIC_EATER)
					{
						for (i = 0; i < EATER_EXT*2; i++)
						{
							p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < EATER_EXT*3; i++)
						{
							p_ptr->magic_num1[i] = 0;
						}
					}
					/* Restore spell points */
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;

					/* Hack -- cancel recall */
					if (p_ptr->word_recall)
					{
						/* Message */
#ifdef JP
						msg_print("張りつめた大気が流れ去った...");
#else
						msg_print("A tension leaves the air around you...");
#endif

						msg_print(NULL);

						/* Hack -- Prevent recall */
						p_ptr->word_recall = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Hack -- cancel alter */
					if (p_ptr->alter_reality)
					{
						/* Hack -- Prevent alter */
						p_ptr->alter_reality = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Note cause of death XXX XXX XXX */
#ifdef JP
					(void)strcpy(p_ptr->died_from, "コンティニュー");
#else
					(void)strcpy(p_ptr->died_from, "continuing");
#endif

					/* Do not die */
					p_ptr->is_dead = FALSE;

					/* Hack -- Healing */
					(void)set_blind(0);
					(void)set_confused(0);
					(void)set_poisoned(0);
					(void)set_afraid(0);
					(void)set_paralyzed(0);
					(void)set_image(0);
					(void)set_stun(0);
					(void)set_cut(0);
					set_alcohol(0);

					if(p_ptr->pclass == CLASS_HINA) hina_gain_yaku(-99999);

					/* Hack -- Prevent starvation */
					(void)set_food(PY_FOOD_MAX - 1);

					dun_level = 0;
					p_ptr->inside_arena = FALSE;
					p_ptr->inside_battle = FALSE;
					leaving_quest = 0;
					p_ptr->inside_quest = 0;
					if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
					dungeon_type = 0;
					if (lite_town || vanilla_town)
					{
						p_ptr->wilderness_y = 1;
						p_ptr->wilderness_x = 1;
						if (vanilla_town)
						{
							p_ptr->oldpy = 10;
							p_ptr->oldpx = 34;
						}
						else
						{
							p_ptr->oldpy = 33;
							p_ptr->oldpx = 180;
						}
					}
					else
					{
						p_ptr->wilderness_y = 34;
						p_ptr->wilderness_x = 39;
						p_ptr->oldpy = 33;
						p_ptr->oldpx = 180;
					}

					/* Leaving */
					p_ptr->wild_mode = FALSE;
					p_ptr->leaving = TRUE;

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            しかし、コンティニューした。");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            but continued.");
#endif

					/* Prepare next floor */
					leave_floor();
					wipe_m_list();
				}
			}
		}
		///mod150620 菫子特殊処理　麻痺、意識不明瞭、急性アル中で強制的に人里入口へ戻る
		else if(flag_sumireko_wakeup)
		{

			Term_clear();
			put_str(_("あなたは意識を失った・・", "You lose consciousness..."), 17 , 40);
			(void)inkey();
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
			(void)set_food(PY_FOOD_MAX - 1);
			reset_tim_flags();
			dun_level = 0;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			leave_quest_check();
			leaving_quest = 0;
			p_ptr->inside_quest = 0;
			if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
			dungeon_type = 0;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->wild_mode = FALSE;
			leave_floor();
			wipe_m_list();
		}

		/* Handle "death" */
		if (p_ptr->is_dead) break;

		/* Make a new level */
		change_floor();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}

///mod140126 アンデッドが夜からゲーム開始するのをやめた
s32b turn_real(s32b hoge)
{
#if 0
	switch (p_ptr->start_race)
	{
	case RACE_VAMPIRE:
	case RACE_SKELETON:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		return hoge - (TURNS_PER_TICK * TOWN_DAWN * 3 / 4);
	default:
		return hoge;
	}
#endif
		return hoge;

}

/*
 * ターンのオーバーフローに対する対処
 * ターン及びターンを記録する変数をターンの限界の1日前まで巻き戻す.
 */
void prevent_turn_overflow(void)
{
	int rollback_days, i, j;
	s32b rollback_turns;

	if (turn < turn_limit) return;

	rollback_days = 1 + (turn - turn_limit) / (TURNS_PER_TICK * TOWN_DAWN);
	rollback_turns = TURNS_PER_TICK * TOWN_DAWN * rollback_days;

	if (turn > rollback_turns) turn -= rollback_turns;
	else turn = 1; /* Paranoia */
	if (old_turn > rollback_turns) old_turn -= rollback_turns;
	else old_turn = 1;
	if (old_battle > rollback_turns) old_battle -= rollback_turns;
	else old_battle = 1;
	if (p_ptr->feeling_turn > rollback_turns) p_ptr->feeling_turn -= rollback_turns;
	else p_ptr->feeling_turn = 1;

	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			store_type *st_ptr = &town[i].store[j];

			if (st_ptr->last_visit > -10L * TURNS_PER_TICK * STORE_TICKS)
			{
				st_ptr->last_visit -= rollback_turns;
				if (st_ptr->last_visit < -10L * TURNS_PER_TICK * STORE_TICKS) st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
			}

			if (st_ptr->store_open)
			{
				st_ptr->store_open -= rollback_turns;
				if (st_ptr->store_open < 1) st_ptr->store_open = 1;
			}
		}
	}
}
