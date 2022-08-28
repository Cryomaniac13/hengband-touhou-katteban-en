
#include "angband.h"

/*:::純狐関係のコード　膨れそうなので別ファイルにした*/

//p_ptr->magic_num1[0-98]:作った「名もなき技」のパラメータ格納 9*11
//p_ptr->magic_num1[99-107]:予備
//p_ptr->magic_num2[0]:空き
//p_ptr->magic_num2[1-107]:技生成重み付けのためのフラグカウント領域


//純狐が月面勢力フラグの敵を倒したとき口走るセリフ
void junko_msg(void)
{

	if (p_ptr->pclass != CLASS_JUNKO) return;

	switch (randint1(7))
	{
	case 1:
	case 2:
	case 3:
	case 4:
		msg_print(_("「不倶戴天の敵、嫦娥よ。見てるか！？」",
                    "'Chang'e, my eternal enemy. Are you watching!?'"));
		break;
	case 5:
		msg_print(_("「私の力の前では無力！絶対的無力！」",
                    "'You're powerless before my power! Absolutely powerless!'"));
		break;
	case 6:
		msg_print(_("「大胆不敵な策だったけど、詰めが甘かった様ね。」",
                    "'That was a bold measure, but looks like you got outsmarted.'"));
		break;
	case 7:
		msg_print(_("「お前が出てくるまで、こいつをいたぶり続けよう！」",
                    "'I'll keep playing with them until you come out!'"));
		break;

	}


}



/*****************ここから純化関連*********************************************/

///純狐が「純化」可能なアイテム　
static bool item_tester_hook_purify_able(object_type *o_ptr)
{
	if(o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER) return FALSE;

	if(object_is_melee_weapon(o_ptr)) return TRUE;
	if(object_is_armour(o_ptr)) return TRUE;

	if(o_ptr->tval == TV_RIBBON) return TRUE;
	if(o_ptr->tval == TV_RING) return TRUE;
	if(o_ptr->tval == TV_AMULET) return TRUE;
	if(o_ptr->tval == TV_LITE)
	{
		if(o_ptr->sval == SV_LITE_TORCH) return FALSE;
		else if(o_ptr->sval == SV_LITE_LANTERN) return FALSE;
		else return TRUE;
	}

	return FALSE;

}

//純狐特技「純化Ⅱ」による装備品純化
//装備品を凡庸化し、削除されたフラグをACか攻撃修正かパラメータ上昇値に変換する
bool purify_equipment(void)
{

	int item;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	int bonus = 0;
	int old_pv,old_to_h,old_to_d,old_to_a;

	item_tester_hook = item_tester_hook_purify_able;

	q = _("どのアイテムを純化しますか？", "Purify which item?");
	s = _("純化できるアイテムがありません。", "You don't have items to purify.");

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	if (!get_check_strict(format(_("純化しますか？ [%s]", "Purify? [%s]"), o_name),CHECK_OKAY_CANCEL)) return FALSE;

	//装備品の追加フラグをボーナス値として得る。
	//また追加フラグを特技フラグとして得ている。
	bonus = flag_cost(o_ptr,o_ptr->pval,TRUE);
	if(bonus < 0) bonus = 0;

	//古いボーナス値を保持(最低0)
	old_pv = MAX(0,o_ptr->pval);
	old_to_h = MAX(0,o_ptr->to_h);
	old_to_d = MAX(0,o_ptr->to_d);
	old_to_a = MAX(0,o_ptr->to_a);


	//mundane_spell()からコピー
	{
		byte iy = o_ptr->iy;                 /* Y-position on map, or zero */
		byte ix = o_ptr->ix;                 /* X-position on map, or zero */
		s16b next_o_idx = o_ptr->next_o_idx; /* Next object in stack (if any) */
		byte marked = o_ptr->marked;         /* Object is marked */
		s16b weight = o_ptr->number * o_ptr->weight;
		u16b inscription = o_ptr->inscription;

		/* Wipe it clean */
		object_prep(o_ptr, o_ptr->k_idx);

		o_ptr->iy = iy;
		o_ptr->ix = ix;
		o_ptr->next_o_idx = next_o_idx;
		o_ptr->marked = marked;
		o_ptr->inscription = inscription;
		if (item >= 0) p_ptr->total_weight += (o_ptr->weight - weight);
	}
	o_ptr->pval = old_pv;
	//o_ptr->to_h = old_to_h;
	//o_ptr->to_d = old_to_d;
	//o_ptr->to_a = old_to_a;
	bonus += old_to_h * 1000;
	bonus += old_to_d * 1000;
	bonus += old_to_a * 1000;


	if(object_is_melee_weapon(o_ptr)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_ACCURACY)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_DAMAGE)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SLAYING))
	{
		if(bonus > 200000) bonus = 200000;
		o_ptr->to_d += bonus / (1500+randint1(500));
		o_ptr->to_h += bonus / (1500+randint1(500));
		if(o_ptr->to_d > 99) o_ptr->to_d = 99;
		if(o_ptr->to_h > 99) o_ptr->to_h = 99;

		if(!k_info[o_ptr->k_idx].pval) o_ptr->pval = 0;

	}
	else if(object_is_armour(o_ptr) || o_ptr->tval == TV_RIBBON)
	{
		if(bonus > 200000) bonus = 200000;
		o_ptr->to_a += bonus / (1000+randint1(500));
		if(o_ptr->to_a > 99) o_ptr->to_a = 99;

		if(!k_info[o_ptr->k_idx].pval) o_ptr->pval = 0;
	}
	else //指輪とアミュと光源がここに来るはず
	{
		object_type forge;
		object_type *tmp_o_ptr = &forge;

		//元々pvalのある品かどうか判定するため、同じk_idxの品をノーマル生成する。k_info[].pvalは指輪とアミュの場合全て0であり判定に使えない。
		object_prep(tmp_o_ptr, o_ptr->k_idx);
		apply_magic(tmp_o_ptr,1,AM_FORCE_NORMAL);

		if(tmp_o_ptr->pval)
		{
			if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SPEED)
			{
				o_ptr->pval += bonus / (5000);
				if(o_ptr->pval > 99) o_ptr->pval = 99;
			}
			else if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_MUSCLE)
			{
				o_ptr->pval += bonus / (8000);
				if(o_ptr->pval > 9) o_ptr->pval = 9;
			}
			else if(o_ptr->tval == TV_AMULET && o_ptr->sval == SV_AMULET_BRILLIANCE)
			{
				o_ptr->pval += bonus / (8000);
				if(o_ptr->pval > 9) o_ptr->pval = 9;
			}
			else if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_ATTACKS)
			{
				o_ptr->pval += bonus / (10000 + randint1(10000));
				if(o_ptr->pval > 5) o_ptr->pval = 5;
				if(o_ptr->pval == 5 && !weird_luck()) o_ptr->pval = 4;
				if(o_ptr->pval  > 3 && !weird_luck()) o_ptr->pval = 3;
			}
			else
			{
				o_ptr->pval += bonus / (3000);
				if(o_ptr->pval > 99) o_ptr->pval = 99;
			}

		}
		else
		{
			o_ptr->pval = 0;
			if(bonus > 200000) bonus = 200000;
			o_ptr->to_a += bonus / (1000);
			if(o_ptr->to_a > 99) o_ptr->to_a = 99;
		}

	}


	/*:::元素から保護*/
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

	//鑑定済み
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	//エゴアイテムにする
	o_ptr->name2 = EGO_JUNKO_PURIFIED;

	//純化済フラグを立てる
	o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	return TRUE;

}


/*:::純狐用コマンド「純化Ⅰ」　アイテムを価値/100のMPに変換する*/
bool convert_item_to_mana(void)
{
	int item, amt = 1;
	int old_number;
	long price;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	char out_val[MAX_NLEN + 32];

	cptr q, s;

	q = _("どのアイテムを純化しますか？", "Purify which item?");
	s = _("MPに変えられる物がありません。", "You don't have anything to convert to MP.");

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);
	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return FALSE;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	o_ptr->number = old_number;

	sprintf(out_val, _("%sは消滅します。よろしいですか？", "%s will disappear.  Is that OK? "), o_name);
	if (!get_check_strict(out_val, CHECK_OKAY_CANCEL)) return FALSE;

	price = object_value_real(o_ptr);


	//純狐の場合純化するアイテムによってフラグ値を得る
	if (p_ptr->pclass == CLASS_JUNKO)
	{
		int flag_val = 10;
		int i;
		u32b flgs[TR_FLAG_SIZE];
		int tv = o_ptr->tval;
		int sv = o_ptr->sval;
		int pv = o_ptr->pval;

		object_flags(o_ptr, flgs);


		//魔法書など
		if (tv <= TV_BOOK_END)
		{
			if (sv == 0) flag_val = 10 * amt;
			if (sv == 1) flag_val = 30 * amt;
			if (sv == 2) flag_val = 80 * amt;
			if (sv == 3) flag_val = 200 * amt;

			switch (tv)
			{
			case TV_BOOK_ELEMENT:

				if (sv == 0) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv == 1) add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv == 2) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RES_ELEM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
				if (sv == 3)  add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				break;
			case TV_BOOK_FORESEE:
				add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_TRAPS, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_OBJ, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ENLIGHT, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ALTER_REALITY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_RESTORE_NEXUS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_LUCKY, flag_val);
				add_junko_flag_val(JKF2_TIME, flag_val);
				break;
			case TV_BOOK_ENCHANT:
				add_junko_flag_val(JKF1_FOOD, flag_val);
				if (sv == 0) add_junko_flag_val(JKF1_SELF_LIGHT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MAGICDEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MAKE_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MANA, flag_val);
				break;
			case TV_BOOK_SUMMONS:
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_D_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_DISTURB_SUMMON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_CALL_METEOR, flag_val);
				add_junko_flag_val(JKF2_DISTORTION, flag_val);
				add_junko_flag_val(JKF2_CHAOS, flag_val);
				break;

			case TV_BOOK_MYSTIC:
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ENLIGHT, flag_val);
				if (sv <= 1) add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_D_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_INVULNER, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_STASIS, flag_val);
				break;

			case TV_BOOK_LIFE:
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RESTORE, flag_val);
				add_junko_flag_val(JKF1_CURE_ALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ULTI_RES, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_HOLY, flag_val);
				break;
			case TV_BOOK_PUNISH:
				if (sv == 0) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv == 1) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv == 2) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 1 || sv == 2) add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_CURE_ALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_BREATH_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_LITE, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_HOLY, flag_val);

				break;
			case TV_BOOK_NATURE:
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 2)add_junko_flag_val(JKF1_HEAL2, flag_val);
				add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RES_ELEM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MOSES, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_WATER, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_WIND, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_STEAM, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MAKE_TREE, flag_val);
				if (sv >= 2)  add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				break;
			case TV_BOOK_TRANSFORM:
				if (sv <= 1) add_junko_flag_val(JKF1_SELF_LIGHT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_BREATH_DEF, flag_val);
				if (sv == 1 || sv == 2) add_junko_flag_val(JKF1_KILL_WALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SHIELD, flag_val);
				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_SHARDS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				break;
			case TV_BOOK_DARKNESS:
				if (sv <= 1) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DUSTROBE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_STASIS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_SILENT_FLOOR, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_DARK, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_HELL, flag_val);
				break;
			case TV_BOOK_NECROMANCY:
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DEATH_RAY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_NETHER, flag_val);
				break;
			case TV_BOOK_CHAOS:
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_FLOOR, flag_val / 4);
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DESTROY, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_CALL_CHAOS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_CALL_METEOR, flag_val);

				add_junko_flag_val(JKF2_CHAOS, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_MANA, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISTORTION, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
				break;
			case TV_BOOK_HISSATSU:
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASTER_MELEE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MULTI_HIT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_INVULNER, flag_val);
				break;
			case TV_BOOK_MEDICINE:
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RESTORE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_RESTORE_NEXUS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ULTI_RES, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);

				break;
			case TV_BOOK_OCCULT:
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DUSTROBE, flag_val);
				add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_DESTRUCT_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_WATER, flag_val);
				if (sv == 3) add_junko_flag_val(JKF2_MAKE_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISTORTION, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

				break;
			}
		}

		//装備品(笛除く)
		else if (tv >= TV_EQUIP_BEGIN && tv <= TV_WEARABLE_END)
		{
			flag_val = price * amt / 500;
			if (flag_val < amt) flag_val = amt * 5;

			if (tv == TV_ARROW || tv == TV_BOLT)
				add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
			else if (tv == TV_KNIFE || tv == TV_SPEAR)
				add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
			else if (tv == TV_SWORD || tv == TV_KATANA)
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
			else if (tv == TV_STICK || tv == TV_HAMMER || tv == TV_AXE)
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
			else if (tv == TV_MAGICITEM || tv == TV_MAGICWEAPON)
				add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
			else if (tv == TV_POLEARM || tv == TV_OTHERWEAPON)
				add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
			else if (tv == TV_BOW || tv == TV_CROSSBOW)
				add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
			else if (tv == TV_GUN)
				add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
			else if (tv == TV_DRAG_ARMOR)
				add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);

			junko_gain_equipment_flags(o_ptr, amt, flgs);

		}
		else if (tv == TV_WHISTLE) //笛
		{
			flag_val = 100 * amt;
			add_junko_flag_val(JKF2_SOUND, flag_val);
		}
		else if (tv == TV_STAFF) //杖
		{
			switch (sv)
			{
			case SV_STAFF_TELEPORTATION:
				add_junko_flag_val(JKF1_TELEPO, 100 * amt);
				break;
			case SV_STAFF_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 50 * amt);
				break;
			case SV_STAFF_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 50 * amt);
				break;
			case SV_STAFF_STARLITE:
				add_junko_flag_val(JKF1_ATTACK_BEAM, 30 * amt);
				add_junko_flag_val(JKF2_LITE, 30);
				break;
			case SV_STAFF_LITE:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				add_junko_flag_val(JKF2_LITE, 40 * amt);
				break;
			case SV_STAFF_MAPPING:
				add_junko_flag_val(JKF1_ENLIGHT, 10 * amt);
				break;
			case SV_STAFF_DETECT_GOLD:
			case SV_STAFF_DETECT_ITEM:
				add_junko_flag_val(JKF1_DETECT_OBJ, 100 * amt);
				break;
			case SV_STAFF_DETECT_TRAP:
			case SV_STAFF_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_TRAPS, 100 * amt);
				break;
			case SV_STAFF_DETECT_INVIS:
			case SV_STAFF_DETECT_EVIL:
				add_junko_flag_val(JKF1_DETECT_MON, 70 * amt);
				break;
			case SV_STAFF_CURE_LIGHT:
				add_junko_flag_val(JKF1_HEAL1, 30 * amt);
				break;
			case SV_STAFF_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 80 * amt);
				break;
			case SV_STAFF_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 200 * amt);
				break;
			case SV_STAFF_THE_MAGI:
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);
				add_junko_flag_val(JKF2_MANA, 1000 * amt);
				break;
			case SV_STAFF_SPEED:
				add_junko_flag_val(JKF1_SPEED, 100 * amt);
				break;
			case SV_STAFF_PROBING:
				add_junko_flag_val(JKF1_IDENTIFY, 100 * amt);
				break;
			case SV_STAFF_DISPEL_EVIL:
				add_junko_flag_val(JKF2_HOLY, 40 * amt);
				break;
			case SV_STAFF_HOLINESS:
				add_junko_flag_val(JKF2_HOLY, 100 * amt);
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 100 * amt);
				break;
			case SV_STAFF_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 100 * amt);
				break;
			case SV_STAFF_EARTHQUAKES:
				add_junko_flag_val(JKF1_EARTHQUAKES, 100 * amt);
				break;
			case SV_STAFF_DESTRUCTION:
				add_junko_flag_val(JKF1_DESTROY, 100 * amt);
				break;
			case SV_STAFF_DARKNESS:
				add_junko_flag_val(JKF2_DARK, 50 * amt);
				break;
			case SV_STAFF_MSTORM:
				add_junko_flag_val(JKF2_MANA, 300 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 400 * amt);
				break;
			}

		}
		else if (tv == TV_FIGURINE || tv == TV_CAPTURE || tv == TV_BUNBUN || tv == TV_KAKASHI) //人形と妖魔本と新聞 耐性とレベルに応じて属性フラグ値を得る
		{
			monster_race *r_ptr = &r_info[pv];
			flag_val = r_ptr->level * r_ptr->level / (tv == TV_FIGURINE ? 20 : 100);
			if (flag_val < 10) flag_val = 10;
			flag_val *= amt;

			if (r_ptr->flagsr & RFR_IM_ACID) add_junko_flag_val(JKF2_ACID, flag_val);
			if (r_ptr->flagsr & RFR_IM_FIRE) add_junko_flag_val(JKF2_FIRE, flag_val);
			if (r_ptr->flagsr & RFR_IM_ELEC) add_junko_flag_val(JKF2_ELEC, flag_val);
			if (r_ptr->flagsr & RFR_IM_COLD) add_junko_flag_val(JKF2_COLD, flag_val);
			if (r_ptr->flagsr & RFR_IM_POIS) add_junko_flag_val(JKF2_POIS, flag_val);
			if (r_ptr->flagsr & RFR_RES_LITE) add_junko_flag_val(JKF2_LITE, flag_val);
			if (r_ptr->flagsr & RFR_RES_DARK) add_junko_flag_val(JKF2_DARK, flag_val);
			if (r_ptr->flagsr & RFR_RES_NETH) add_junko_flag_val(JKF2_NETHER, flag_val);
			if (r_ptr->flagsr & RFR_RES_WATE) add_junko_flag_val(JKF2_WATER, flag_val);
			if (r_ptr->flagsr & RFR_RES_SHAR) add_junko_flag_val(JKF2_SHARDS, flag_val);
			if (r_ptr->flagsr & RFR_RES_SOUN) add_junko_flag_val(JKF2_SOUND, flag_val);
			if (r_ptr->flagsr & RFR_RES_CHAO) add_junko_flag_val(JKF2_CHAOS, flag_val);
			if (r_ptr->flagsr & RFR_RES_HOLY) add_junko_flag_val(JKF2_HOLY, flag_val);
			if (r_ptr->flagsr & RFR_RES_DISE) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
			if (r_ptr->flagsr & RFR_RES_TIME) add_junko_flag_val(JKF2_TIME, flag_val);
		}
		else if (tv == TV_WAND)//魔法棒
		{
			switch (sv)
			{
			case SV_WAND_TELEPORT_AWAY:
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 30 * amt);
				break;
			case SV_WAND_DISARMING:
			case SV_WAND_TRAP_DOOR_DEST:
				add_junko_flag_val(JKF1_DISARM, 50 * amt);
				break;
			case SV_WAND_STONE_TO_MUD:
				add_junko_flag_val(JKF2_DISINTEGRATE, 30 * amt);
				break;
			case SV_WAND_LITE:
				add_junko_flag_val(JKF2_LITE, 30 * amt);
				break;
			case SV_WAND_SLEEP_MONSTER:
			case SV_WAND_SLOW_MONSTER:
			case SV_WAND_CONFUSE_MONSTER:
			case SV_WAND_FEAR_MONSTER:
			case SV_WAND_CHARM_MONSTER:
				add_junko_flag_val(JKF1_STASIS, 20 * amt);
				break;
			case SV_WAND_DRAIN_LIFE:
				add_junko_flag_val(JKF2_NETHER, 50 * amt);
				break;
			case SV_WAND_POLYMORPH:
				add_junko_flag_val(JKF2_CHAOS, 25 * amt);
				break;
			case SV_WAND_STINKING_CLOUD:
				add_junko_flag_val(JKF2_POIS, 20 * amt);
				break;
			case SV_WAND_MAGIC_MISSILE:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				break;
			case SV_WAND_ACID_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ACID, 30 * amt);
				break;
			case SV_WAND_FIRE_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_FIRE, 30 * amt);
				break;
			case SV_WAND_COLD_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_COLD, 30 * amt);
				break;
			case SV_WAND_ACID_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_ACID, 50 * amt);
				break;
			case SV_WAND_ELEC_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_ELEC, 50 * amt);
				break;
			case SV_WAND_FIRE_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_WAND_COLD_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_COLD, 50 * amt);
				break;
			case SV_WAND_DISINTEGRATE:
				add_junko_flag_val(JKF1_ATTACK_BALL, 100 * amt);
				add_junko_flag_val(JKF2_DISINTEGRATE, 100 * amt);
				break;
			case SV_WAND_DRAGON_FIRE:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 100 * amt);
				add_junko_flag_val(JKF2_FIRE, 200 * amt);
				break;
			case SV_WAND_DRAGON_COLD:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 100 * amt);
				add_junko_flag_val(JKF2_COLD, 200 * amt);
				break;
			case SV_WAND_DRAGON_BREATH:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 200 * amt);
				break;
			case SV_WAND_ROCKETS:
				add_junko_flag_val(JKF1_ATTACK_ROCKET, 500 * amt);
				add_junko_flag_val(JKF2_SHARDS, 500 * amt);
				break;
			case SV_WAND_STRIKING:
				add_junko_flag_val(JKF2_METEOR, 500 * amt);
				break;
			case SV_WAND_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 100 * amt);
				break;
			}
		}



		else if (tv == TV_ROD) //ロッド
		{
			switch (sv)
			{
			case SV_ROD_DETECT_TRAP:
			case SV_ROD_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_TRAPS, 50 * amt);
				break;
			case SV_ROD_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 300 * amt);
				break;
			case SV_ROD_RECALL:
				add_junko_flag_val(JKF1_TELEPO, 100 * amt);
				break;
			case SV_ROD_ILLUMINATION:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				break;
			case SV_ROD_MAPPING:
				add_junko_flag_val(JKF1_ENLIGHT, 50 * amt);
				break;
			case SV_ROD_DETECTION:
				add_junko_flag_val(JKF1_DETECT_TRAPS, 200 * amt);
				add_junko_flag_val(JKF1_DETECT_OBJ, 200 * amt);
				add_junko_flag_val(JKF1_DETECT_MON, 200 * amt);
				break;
			case SV_ROD_PROBING:
				add_junko_flag_val(JKF1_DETECT_MON, 200 * amt);
				break;
			case SV_ROD_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 200 * amt);
				break;
			case SV_ROD_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 500 * amt);
				break;
			case SV_ROD_RESTORATION:
				add_junko_flag_val(JKF1_RESTORE, 200 * amt);
				break;
			case SV_ROD_SPEED:
				add_junko_flag_val(JKF1_SPEED, 300 * amt);
				break;
			case SV_ROD_PESTICIDE:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 20 * amt);
				break;
			case SV_ROD_TELEPORT_AWAY:
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 50 * amt);
				break;
			case SV_ROD_DISARMING:
				add_junko_flag_val(JKF1_DISARM, 100 * amt);
				break;
			case SV_ROD_LITE:
				add_junko_flag_val(JKF2_LITE, 30 * amt);
				break;
			case SV_ROD_SLEEP_MONSTER:
			case SV_ROD_SLOW_MONSTER:
				add_junko_flag_val(JKF1_STASIS, 30 * amt);
				break;
			case SV_ROD_DRAIN_LIFE:
				add_junko_flag_val(JKF2_NETHER, 80 * amt);
				break;
			case SV_ROD_POLYMORPH:
				add_junko_flag_val(JKF2_CHAOS, 20 * amt);
				break;
			case SV_ROD_ACID_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ACID, 50 * amt);
				break;
			case SV_ROD_ELEC_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ELEC, 50 * amt);
				break;
			case SV_ROD_FIRE_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_ROD_COLD_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_COLD, 50 * amt);
				break;
			case SV_ROD_ACID_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_ACID, 100 * amt);
				break;
			case SV_ROD_ELEC_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_ELEC, 100 * amt);
				break;
			case SV_ROD_FIRE_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_FIRE, 100 * amt);
				break;
			case SV_ROD_COLD_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_COLD, 100 * amt);
				break;
			case SV_ROD_HAVOC:
				add_junko_flag_val(JKF1_CALL_CHAOS, 500 * amt);
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 500 * amt);
				break;
			case SV_ROD_STONE_TO_MUD:
				add_junko_flag_val(JKF1_KILL_WALL, 50 * amt);
				add_junko_flag_val(JKF2_DISINTEGRATE, 50 * amt);
				break;

			}
		}
		else if (tv == TV_SCROLL)//巻物
		{

			switch (sv)
			{
			case SV_SCROLL_DARKNESS:
				add_junko_flag_val(JKF2_DARK, 5 * amt);
				break;
			case SV_SCROLL_CURSE_ARMOR:
			case SV_SCROLL_CURSE_WEAPON:
				add_junko_flag_val(JKF2_DISENCHANT, 50 * amt);
				break;
			case SV_SCROLL_TRAP_CREATION:
				add_junko_flag_val(JKF2_MAKE_DOOR, 20 * amt);
				break;
			case SV_SCROLL_PHASE_DOOR:
				add_junko_flag_val(JKF1_TELEPO, 2 * amt);
				break;
			case SV_SCROLL_TELEPORT:
				add_junko_flag_val(JKF1_TELEPO, 5 * amt);
				break;
			case SV_SCROLL_TELEPORT_LEVEL:
			case SV_SCROLL_WORD_OF_RECALL:
				add_junko_flag_val(JKF1_TELEPO, 10 * amt);
				break;
			case SV_SCROLL_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 3 * amt);
				break;
			case SV_SCROLL_STAR_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 50 * amt);
				break;
			case SV_SCROLL_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 5 * amt);
				break;
			case SV_SCROLL_STAR_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 50 * amt);
				break;
			case SV_SCROLL_ENCHANT_ARMOR:
				add_junko_flag_val(JKF1_ADD_ESSENCE2, 10 * amt);
				break;
			case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
			case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
				add_junko_flag_val(JKF1_ADD_ESSENCE1, 10 * amt);
				break;
			case SV_SCROLL_STAR_ENCHANT_ARMOR:
				add_junko_flag_val(JKF1_ADD_ESSENCE2, 50 * amt);
				break;
			case SV_SCROLL_STAR_ENCHANT_WEAPON:
				add_junko_flag_val(JKF1_ADD_ESSENCE1, 50 * amt);
				break;
			case SV_SCROLL_RECHARGING:
				add_junko_flag_val(JKF1_MASTER_MAGITEM, 5 * amt);
				break;
			case SV_SCROLL_MUNDANITY:
				add_junko_flag_val(JKF1_SILENT_FLOOR, 20 * amt);
				break;
			case SV_SCROLL_LIGHT:
				add_junko_flag_val(JKF1_SELF_LIGHT, 10 * amt);
				break;
			case SV_SCROLL_MAPPING:
				add_junko_flag_val(JKF1_ENLIGHT, 2 * amt);
				break;
			case SV_SCROLL_DETECT_GOLD:
			case SV_SCROLL_DETECT_ITEM:
				add_junko_flag_val(JKF1_DETECT_OBJ, 5 * amt);
				break;
			case SV_SCROLL_DETECT_TRAP:
			case SV_SCROLL_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_TRAPS, 5 * amt);
				break;
			case SV_SCROLL_DETECT_INVIS:
				add_junko_flag_val(JKF1_DETECT_MON, 5 * amt);
				break;
			case SV_SCROLL_HOLY_CHANT:
				add_junko_flag_val(JKF1_RES_MAGIC, 3 * amt);
				break;
			case SV_SCROLL_PROTECTION_FROM_EVIL:
				add_junko_flag_val(JKF1_SHIELD, 20 * amt);
				break;
			case SV_SCROLL_RUNE_OF_PROTECTION:
				add_junko_flag_val(JKF1_DISTURB_SUMMON, 20 * amt);
				break;
			case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
				add_junko_flag_val(JKF1_DISARM, 5 * amt);
				break;
			case SV_SCROLL_STAR_DESTRUCTION:
				add_junko_flag_val(JKF1_DESTROY, 20 * amt);
				break;
			case SV_SCROLL_DISPEL_UNDEAD:
				add_junko_flag_val(JKF2_HOLY, 20 * amt);
				break;
			case SV_SCROLL_SPELL:
				add_junko_flag_val(JKF1_ABSORB_MAGIC, 50 * amt);
				break;
			case SV_SCROLL_GENOCIDE:
			case SV_SCROLL_MASS_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 30 * amt);
				break;
			case SV_SCROLL_ACQUIREMENT:
				add_junko_flag_val(JKF1_LUCKY, 100 * amt);
				break;
			case SV_SCROLL_STAR_ACQUIREMENT:
				add_junko_flag_val(JKF1_LUCKY, 1000 * amt);
				break;
			case SV_SCROLL_FIRE:
				add_junko_flag_val(JKF2_FIRE, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_ICE:
				add_junko_flag_val(JKF2_COLD, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_CHAOS:
				add_junko_flag_val(JKF2_CHAOS, 500 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_ARTIFACT:
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);

				break;
			}
		}
		else if (tv == TV_SOUVENIR) //珍品
		{

			switch (sv)
			{
			case SV_SOUVENIR_GAMEBOY:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 1000 * amt);
				break;
			case SV_SOUVENIR_ILMENITE:
				add_junko_flag_val(JKF1_MAGICDEF, 300 * amt);
				break;
			case SV_SOUVENIR_PHOENIX_FEATHER:
				add_junko_flag_val(JKF2_NUKE, 1000 * amt);
				add_junko_flag_val(JKF1_HEAL2, 1000 * amt);
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 1000 * amt);
				add_junko_flag_val(JKF1_BREATH_DEF, 1000 * amt);
				break;
			case SV_SOUVENIR_SMARTPHONE:
				add_junko_flag_val(JKF1_D_DOOR, 300 * amt);
				add_junko_flag_val(JKF1_ALTER_REALITY, 300 * amt);
				break;
			case SV_SOUVENIR_KAPPA_5:
				add_junko_flag_val(JKF1_RES_ELEM, 1000 * amt);
				add_junko_flag_val(JKF1_LUCKY, 1000 * amt);
				break;
			case SV_SOUVENIR_PRISM:
				add_junko_flag_val(JKF2_LITE, 300 * amt);
				add_junko_flag_val(JKF2_FIRE, 300 * amt);
				add_junko_flag_val(JKF2_ELEC, 300 * amt);
				add_junko_flag_val(JKF2_COLD, 300 * amt);
				break;
			case SV_SOUVENIR_KAPPA_ARM:
				add_junko_flag_val(JKF1_MULTI_HIT, 300 * amt);
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				break;
			case SV_SOUVENIR_BIG_EGG:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 1000 * amt);
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 1000 * amt);
				break;
			case SV_SOUVENIR_ASIA:
				add_junko_flag_val(JKF2_LITE, 1000 * amt);
				add_junko_flag_val(JKF2_HOLY, 1000 * amt);
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				break;
			case SV_SOUVENIR_ULTRASONIC:
				add_junko_flag_val(JKF1_MASS_GENO, 300 * amt);
				break;
			case SV_SOUVENIR_STARS_AND_STRIPES:
				add_junko_flag_val(JKF1_MUSCLE_UP, 1000 * amt);
				add_junko_flag_val(JKF1_MENTAL_UP, 1000 * amt);
				add_junko_flag_val(JKF1_CALL_CHAOS, 1000 * amt);
				break;
			case SV_SOUVENIR_OUIJA_BOARD:
				add_junko_flag_val(JKF1_DETECT_MON, 500 * amt);
				add_junko_flag_val(JKF1_DETECT_OBJ, 500 * amt);
				add_junko_flag_val(JKF1_DETECT_TRAPS, 500 * amt);
				break;
			case SV_SOUVENIR_KINKAKUJI:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 1000 * amt);
				add_junko_flag_val(JKF1_CALL_METEOR, 1000 * amt);
				break;
			case SV_SOUVENIR_KESERAN_PASARAN:
				add_junko_flag_val(JKF1_VANISH_BEAM, 1000 * amt);
				add_junko_flag_val(JKF1_LUCKY, 200 * amt);
				break;
			case SV_SOUVENIR_MARISA_TUBO:
				add_junko_flag_val(JKF1_SILENT_FLOOR, 300 * amt);
				break;
			case SV_SOUVENIR_DOUJU:
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				add_junko_flag_val(JKF2_FIRE, 500 * amt);
				add_junko_flag_val(JKF2_WATER, 500 * amt);
				break;

			case SV_SOUVENIR_KODOKUZARA:
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);
				add_junko_flag_val(JKF2_POIS, 1000 * amt);
				add_junko_flag_val(JKF2_HELL, 1000 * amt);
				break;

			}
		}
		else if (tv == TV_MATERIAL) //素材
		{
			switch (sv)
			{
			case SV_MATERIAL_HIKARIGOKE:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				break;
			case SV_MATERIAL_HEMATITE:
			case SV_MATERIAL_MAGNETITE:
				add_junko_flag_val(JKF2_MANA, 10 * amt);
				break;
			case SV_MATERIAL_ARSENIC:
			case SV_MATERIAL_MERCURY:
				add_junko_flag_val(JKF2_POIS, 50 * amt);
				break;
			case SV_MATERIAL_MITHRIL:
				add_junko_flag_val(JKF2_HOLY, 50 * amt);
				break;
			case SV_MATERIAL_ADAMANTITE:
				add_junko_flag_val(JKF1_SHIELD, 50 * amt);
				break;
			case SV_MATERIAL_DRAGONNAIL:
				add_junko_flag_val(JKF1_HEAL2, 200 * amt);
				add_junko_flag_val(JKF1_WHIRL_WIND, 200 * amt);
				add_junko_flag_val(JKF1_MULTI_HIT, 100 * amt);

				break;
			case SV_MATERIAL_DRAGONSCALE:
				add_junko_flag_val(JKF2_FIRE, 200 * amt);
				add_junko_flag_val(JKF2_ELEC, 200 * amt);
				add_junko_flag_val(JKF2_ACID, 200 * amt);
				add_junko_flag_val(JKF2_COLD, 200 * amt);
				add_junko_flag_val(JKF1_RES_MAGIC, 100 * amt);
				break;
			case SV_MATERIAL_GELSEMIUM:
				add_junko_flag_val(JKF2_POIS, 500 * amt);
				break;
			case SV_MATERIAL_DIAMOND:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 500 * amt);
				break;
			case SV_MATERIAL_EMERALD:
				add_junko_flag_val(JKF1_BREATH_DEF, 500 * amt);
				break;
			case SV_MATERIAL_RUBY:
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 500 * amt);
				break;
			case SV_MATERIAL_SAPPHIRE:
				add_junko_flag_val(JKF1_VANISH_BEAM, 500 * amt);
				break;
			case SV_MATERIAL_METEORICIRON:
				add_junko_flag_val(JKF1_CALL_METEOR, 1000 * amt);
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				break;
			case SV_MATERIAL_HIHIIROKANE:
				for (i = 1; i <= JKF1_MAX; i++) p_ptr->magic_num2[i] = 255;//全フラグ値MAX
				break;
			case SV_MATERIAL_ICESCALE:
				add_junko_flag_val(JKF2_COLD, 300 * amt);
				add_junko_flag_val(JKF2_SHARDS, 100 * amt);
				break;
			case SV_MATERIAL_NIGHTMARE_FRAGMENT:
				add_junko_flag_val(JKF1_STASIS, 300 * amt);
				add_junko_flag_val(JKF1_DEATH_RAY, 300 * amt);
				break;
			case SV_MATERIAL_HOPE_FRAGMENT:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 300 * amt);
				add_junko_flag_val(JKF1_MOSES, 300 * amt);
				break;
			case SV_MATERIAL_TAKAKUSAGORI:
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				add_junko_flag_val(JKF1_CURE_ALL, 300 * amt);
				add_junko_flag_val(JKF1_RESTORE, 300 * amt);
				break;
			case SV_MATERIAL_IZANAGIOBJECT:
				add_junko_flag_val(JKF1_ALTER_REALITY, 500 * amt);
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 300 * amt);
				add_junko_flag_val(JKF2_TIME, 300 * amt);
				break;

			case SV_MATERIAL_RYUUZYU:
				add_junko_flag_val(JKF1_BREATH_DEF, 300 * amt);
				add_junko_flag_val(JKF2_MANA, 100 * amt);

				break;


			case SV_MATERIAL_ISHIZAKURA:
			case SV_MATERIAL_SKULL:
				add_junko_flag_val(JKF2_NETHER, 100 * amt);
				break;
			}
		}

		else if (tv == TV_MUSHROOM)
		{

			switch (sv)
			{
			case SV_MUSHROOM_RESTORING:
				add_junko_flag_val(JKF1_RESTORE, 50 * amt);
				break;
			case SV_MUSHROOM_MON_SUPER:
				add_junko_flag_val(JKF1_MUSCLE_UP, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_SPARK, 1000 * amt);
				break;
			}
		}
		else if (tv == TV_POTION) //薬
		{

			switch (sv)
			{
			case SV_POTION_POISON:
				add_junko_flag_val(JKF2_POIS, 10 * amt);
				break;
			case SV_POTION_SLOWNESS:
			case SV_POTION_BLINDNESS:
			case SV_POTION_SLEEP:
				add_junko_flag_val(JKF1_STASIS, 10 * amt);
				break;
			case SV_POTION_RUINATION:
				add_junko_flag_val(JKF1_CALL_CHAOS, 50 * amt);
				break;
			case SV_POTION_DETONATIONS:
				add_junko_flag_val(JKF1_ATTACK_BALL, 300 * amt);
				add_junko_flag_val(JKF2_SHARDS, 300 * amt);
				break;
			case SV_POTION_DEATH:
				add_junko_flag_val(JKF1_DEATH_RAY, 300 * amt);
				break;
			case SV_POTION_SLOW_POISON:
			case SV_POTION_CURE_POISON:
			case SV_POTION_BOLDNESS:
				add_junko_flag_val(JKF1_CURE_ALL, 10 * amt);
				break;
			case SV_POTION_SPEED:
				add_junko_flag_val(JKF1_SPEED, 40 * amt);
				break;
			case SV_POTION_RESIST_HEAT:
			case SV_POTION_RESIST_COLD:
				add_junko_flag_val(JKF1_RES_ELEM, 10 * amt);
				break;
			case SV_POTION_CURE_CRITICAL:
				add_junko_flag_val(JKF1_HEAL1, 10 * amt);
				break;
			case SV_POTION_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 20 * amt);
				break;
			case SV_POTION_STAR_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 50 * amt);
				break;
			case SV_POTION_LIFE:
				add_junko_flag_val(JKF1_HEAL2, 100 * amt);
				add_junko_flag_val(JKF1_CURE_ALL, 100 * amt);
				add_junko_flag_val(JKF1_RESTORE, 100 * amt);
				break;
			case SV_POTION_RESTORE_MANA:
				add_junko_flag_val(JKF1_ABSORB_MAGIC, 50 * amt);
				break;
			case SV_POTION_RESTORE_EXP:
			case SV_POTION_RES_STR:
			case SV_POTION_RES_INT:
			case SV_POTION_RES_WIS:
			case SV_POTION_RES_CON:
			case SV_POTION_RES_DEX:
			case SV_POTION_RES_CHR:
				add_junko_flag_val(JKF1_RESTORE, 10 * amt);
				break;
			case SV_POTION_INC_STR:
			case SV_POTION_INC_CON:
			case SV_POTION_INC_DEX:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				break;
			case SV_POTION_INC_INT:
			case SV_POTION_INC_WIS:
			case SV_POTION_INC_CHR:
				add_junko_flag_val(JKF1_MENTAL_UP, 100 * amt);
				break;
			case SV_POTION_AUGMENTATION:
				add_junko_flag_val(JKF1_MUSCLE_UP, 500 * amt);
				add_junko_flag_val(JKF1_MENTAL_UP, 500 * amt);
				break;
			case SV_POTION_ENLIGHTENMENT:
				add_junko_flag_val(JKF1_ENLIGHT, 50 * amt);
				break;
			case SV_POTION_STAR_ENLIGHTENMENT:
				add_junko_flag_val(JKF1_ENLIGHT, 500 * amt);
				break;
			case SV_POTION_SELF_KNOWLEDGE:
				add_junko_flag_val(JKF1_MASTER_MELEE, 50 * amt);
				break;
			case SV_POTION_EXPERIENCE:
				add_junko_flag_val(JKF1_MASTER_MELEE, 200 * amt);
				break;
			case SV_POTION_RESISTANCE:
				add_junko_flag_val(JKF1_RES_ELEM, 50 * amt);
				break;
			case SV_POTION_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 50 * amt);
				add_junko_flag_val(JKF1_HEAL1, 30 * amt);
				break;
			case SV_POTION_INVULNERABILITY:
				add_junko_flag_val(JKF1_INVULNER, 50 * amt);
				break;
			case SV_POTION_NEW_LIFE:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 100 * amt);
				break;
			case SV_POTION_POLYMORPH:
				add_junko_flag_val(JKF2_CHAOS, 100 * amt);
				break;
			case SV_POTION_NANIKASUZUSHIKUNARU:
				add_junko_flag_val(JKF2_COLD, 1000 * amt);
				break;
			case SV_POTION_POISON2:
				add_junko_flag_val(JKF2_POIS, 100 * amt);
				break;
			case SV_POTION_LOVE:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 100 * amt);
				break;

			}
		}

		else if (tv == TV_ALCOHOL)
		{

			switch (sv)
			{
			case SV_ALCOHOL_GOLDEN_MEAD:
				add_junko_flag_val(JKF1_MENTAL_UP, 200 * amt);
				add_junko_flag_val(JKF1_RES_ELEM, 200 * amt);
				break;
			case SV_ALCOHOL_MOON:
				add_junko_flag_val(JKF1_ALTER_REALITY, 100 * amt);
				break;
			case SV_ALCOHOL_NECTAR:
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 100 * amt);
				add_junko_flag_val(JKF1_BREATH_DEF, 100 * amt);
				break;
			case SV_ALCOHOL_SYUTYUU:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				break;
			case SV_ALCOHOL_TRUE_ONIKOROSHI:
				add_junko_flag_val(JKF1_DESTROY, 300 * amt);
				break;
			case SV_ALCOHOL_KUSHINADA:
				add_junko_flag_val(JKF1_STASIS, 300 * amt);
				break;
			}
		}
		else if (tv == TV_COMPOUND)//合成薬　EXTRAでうどんげから手に入る分だけ
		{

			switch (sv)
			{
			case SV_COMPOUND_MUSCLE_DRUG:
				add_junko_flag_val(JKF1_MUSCLE_UP, 50 * amt);
				break;
			case SV_COMPOUND_HAPPY:
				add_junko_flag_val(JKF1_LUCKY, 10 * amt);
				break;
			case SV_COMPOUND_BREATH_OF_FIRE:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 50 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_COMPOUND_JEKYLL:
				add_junko_flag_val(JKF1_MASTER_MELEE, 50 * amt);
				break;
			case SV_COMPOUND_METAMOR_BEAST:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				add_junko_flag_val(JKF1_RUSH_ATTACK, 100 * amt);
				add_junko_flag_val(JKF2_CHAOS, 50 * amt);
				add_junko_flag_val(JKF1_MASTER_MELEE, 100 * amt);
				break;
			case SV_COMPOUND_STAR_DETORNATION:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 200 * amt);
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 50 * amt);
				add_junko_flag_val(JKF2_NUKE, 500 * amt);
				break;
			}
		}

	}

	if (price < 100)
	{
		msg_format(_("%sは消滅したが、霊力を得られなかった。",
                    "%s disappears, but you haven't gained any spiritual power."), o_name);

	}
	else
	{
		price /= 100;

		if (amt > 1) price *= amt;
		if (price > 999) price = 999;
		msg_format(_("%sは%dのMPとなって消滅した。",
                    "%s disappears, turning into %d MP."), o_name, price);
		player_gain_mana(price);
	}

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

	return TRUE;
}

//純狐が装備品フラグによって特技フラグ値を得る。
//二つの純化コマンドから使うので分離..したが結局一箇所からしか使ってない
void junko_gain_equipment_flags(object_type *o_ptr, int amt, u32b flgs[TR_FLAG_SIZE])
{
	int i;
	int flag_val;
	int price;
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	int pv = o_ptr->pval;

	if (p_ptr->pclass != CLASS_JUNKO) { msg_print(_("ERROR:純狐以外でjunko_gain_equipment_flags()が呼ばれた",
                                                    "ERROR: junko_gain_equipment_flags() called for non-Junko")); return; }

	price = object_value_real(o_ptr);

	flag_val = price * amt / 500;
	if (flag_val < amt) flag_val = amt * 5;

	if (object_is_artifact(o_ptr))
	{
		if (object_is_shooting_weapon(o_ptr)) add_junko_flag_val(JKF1_ATTACK_FLOOR, flag_val);
		else if (object_is_melee_weapon(o_ptr))
		{
			if (tv == TV_SPEAR || tv == TV_ARROW || tv == TV_BOLT)
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
			else if (tv == TV_KNIFE)
				add_junko_flag_val(JKF1_MULTI_HIT, flag_val);
			else
				add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);

			add_junko_flag_val(JKF1_MASTER_MELEE, flag_val);

		}
		else if (tv == TV_CLOTHES || tv == TV_ARMOR || tv == TV_DRAG_ARMOR)
		{
			if (price > 80000) add_junko_flag_val(JKF1_MAGICDEF, flag_val);
			else if (price > 30000) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
			else if (price > 10000) add_junko_flag_val(JKF1_SHIELD, flag_val);

		}
		else if (tv == TV_RIBBON)
		{
			add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
		}
		else add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);

		if (have_flag(flgs, TR_REGEN)) add_junko_flag_val(JKF1_HEAL2, flag_val);
		if (have_flag(flgs, TR_TELEPORT)) add_junko_flag_val(JKF1_D_DOOR, flag_val);
		if (have_flag(flgs, TR_NO_MAGIC)) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
		if (have_flag(flgs, TR_NO_TELE)) add_junko_flag_val(JKF1_DESTRUCT_DEF, flag_val);

		if (have_flag(flgs, TR_ACTIVATE) && price > 30000) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val);


		if (object_is_weapon_ammo(o_ptr))
			add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
		else
			add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);

	}
	else
	{
		if (have_flag(flgs, TR_REGEN)) add_junko_flag_val(JKF1_HEAL1, flag_val);
		if (have_flag(flgs, TR_TELEPORT)) add_junko_flag_val(JKF1_TELEPO, flag_val);
		if (have_flag(flgs, TR_NO_MAGIC)) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);

	}

	if (have_flag(flgs, TR_SLOW_DIGEST)) add_junko_flag_val(JKF1_FOOD, flag_val);
	for (i = TR_ESP_EVIL; i <= TR_ESP_UNIQUE; i++) if (have_flag(flgs, i)) add_junko_flag_val(JKF1_DETECT_MON, flag_val / 3);
	if (have_flag(flgs, TR_WARNING)) add_junko_flag_val(JKF1_DETECT_TRAPS, flag_val);
	for (i = TR_SUST_STR; i <= TR_SUST_CHR; i++) if (have_flag(flgs, i)) add_junko_flag_val(JKF1_RESTORE, flag_val / 2);

	if (have_flag(flgs, TR_FREE_ACT) || have_flag(flgs, TR_RES_FEAR) || have_flag(flgs, TR_RES_BLIND) || have_flag(flgs, TR_RES_CONF))
		add_junko_flag_val(JKF1_CURE_ALL, flag_val);

	if (have_flag(flgs, TR_BLESSED)) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
	if (have_flag(flgs, TR_REFLECT)) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
	if (have_flag(flgs, TR_IMPACT)) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

	if (have_flag(flgs, TR_MAGIC_MASTERY)) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val * pv / 4);
	if (have_flag(flgs, TR_STR)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_CON)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_DEX)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_INT)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_WIS)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_CHR)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_SPEED)) add_junko_flag_val(JKF1_SPEED, flag_val * pv / 2);
	if (have_flag(flgs, TR_TUNNEL)) add_junko_flag_val(JKF1_KILL_WALL, flag_val * pv / 2);
	if (have_flag(flgs, TR_STEALTH)) add_junko_flag_val(JKF1_S_STEALTH, flag_val * pv / 5);
	if (have_flag(flgs, TR_SEARCH)) add_junko_flag_val(JKF1_DETECT_OBJ, flag_val * pv / 2);
	if (have_flag(flgs, TR_BLOWS)) add_junko_flag_val(JKF1_MULTI_HIT, flag_val * pv / 2);

	if (have_flag(flgs, TR_DISARM)) add_junko_flag_val(JKF1_DISARM, flag_val * pv / 2);
	if (have_flag(flgs, TR_SAVING)) add_junko_flag_val(JKF1_RES_MAGIC, flag_val * pv / 2);

	if (have_flag(flgs, TR_RES_ACID) && have_flag(flgs, TR_RES_ELEC) && have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_RES_COLD))
		add_junko_flag_val(JKF1_RES_ELEM, flag_val);

	if (have_flag(flgs, TR_IM_ACID)) add_junko_flag_val(JKF2_ACID, 5000);
	if (have_flag(flgs, TR_IM_FIRE)) add_junko_flag_val(JKF2_FIRE, 5000);
	if (have_flag(flgs, TR_IM_ELEC)) add_junko_flag_val(JKF2_ELEC, 5000);
	if (have_flag(flgs, TR_IM_COLD)) add_junko_flag_val(JKF2_COLD, 5000);

	if (have_flag(flgs, TR_RES_ACID)) add_junko_flag_val(JKF2_ACID, flag_val);
	if (have_flag(flgs, TR_BRAND_ACID)) add_junko_flag_val(JKF2_ACID, flag_val);
	if (have_flag(flgs, TR_RES_ELEC)) add_junko_flag_val(JKF2_ELEC, flag_val);
	if (have_flag(flgs, TR_BRAND_ELEC)) add_junko_flag_val(JKF2_ELEC, flag_val);
	if (have_flag(flgs, TR_RES_FIRE)) add_junko_flag_val(JKF2_FIRE, flag_val);
	if (have_flag(flgs, TR_BRAND_FIRE)) add_junko_flag_val(JKF2_FIRE, flag_val);
	if (have_flag(flgs, TR_RES_COLD)) add_junko_flag_val(JKF2_COLD, flag_val);
	if (have_flag(flgs, TR_BRAND_COLD)) add_junko_flag_val(JKF2_COLD, flag_val);
	if (have_flag(flgs, TR_RES_POIS)) add_junko_flag_val(JKF2_POIS, flag_val);
	if (have_flag(flgs, TR_BRAND_POIS)) add_junko_flag_val(JKF2_POIS, flag_val);

	if (have_flag(flgs, TR_RES_LITE)) add_junko_flag_val(JKF2_LITE, flag_val);
	if (have_flag(flgs, TR_RES_DARK)) add_junko_flag_val(JKF2_DARK, flag_val);
	if (have_flag(flgs, TR_RES_SHARDS)) add_junko_flag_val(JKF2_SHARDS, flag_val);
	if (have_flag(flgs, TR_RES_WATER)) add_junko_flag_val(JKF2_WATER, flag_val);
	if (have_flag(flgs, TR_RES_SOUND)) add_junko_flag_val(JKF2_SOUND, flag_val);
	if (have_flag(flgs, TR_XTRA_MIGHT)) add_junko_flag_val(JKF2_NUKE, flag_val);
	if (have_flag(flgs, TR_XTRA_SHOTS)) add_junko_flag_val(JKF2_NUKE, flag_val);

	if (have_flag(flgs, TR_FORCE_WEAPON)) add_junko_flag_val(JKF2_MANA, flag_val);

	if (have_flag(flgs, TR_IMPACT)) add_junko_flag_val(JKF2_METEOR, flag_val);

	if (have_flag(flgs, TR_RES_DISEN)) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
	if (have_flag(flgs, TR_RES_TIME)) add_junko_flag_val(JKF2_TIME, flag_val);
	if (have_flag(flgs, TR_SPEEDSTER)) add_junko_flag_val(JKF2_DISTORTION, flag_val);

	if (have_flag(flgs, TR_RES_HOLY)) add_junko_flag_val(JKF2_HOLY, flag_val);
	if (have_flag(flgs, TR_SLAY_EVIL)) add_junko_flag_val(JKF2_HOLY, flag_val);
	if (have_flag(flgs, TR_KILL_EVIL)) add_junko_flag_val(JKF2_HOLY, flag_val);

	if (have_flag(flgs, TR_SLAY_GOOD)) add_junko_flag_val(JKF2_HELL, flag_val);
	if (have_flag(flgs, TR_KILL_GOOD)) add_junko_flag_val(JKF2_HELL, flag_val);

	if (have_flag(flgs, TR_VORPAL)) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
	if (have_flag(flgs, TR_EX_VORPAL)) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
	if (have_flag(flgs, TR_LEVITATION)) add_junko_flag_val(JKF2_WIND, flag_val);

	//v1.1.36追加
	if (have_flag(flgs, TR_RES_CHAOS)) add_junko_flag_val(JKF2_CHAOS, flag_val);
	if (have_flag(flgs, TR_RES_NETHER)) add_junko_flag_val(JKF2_NETHER, flag_val);

}


/************この下「名もなき技」関連*********************************/

//純狐の技生成名候補二次元リストの要素数
#define JUNKO_NAME_LIST_NUM1	76
#define JUNKO_NAME_LIST_NUM_SUB	10

//純狐技生成名リスト1 ここの添え字が技基本情報テーブルから参照される
cptr	junko_arts_name_words_1[JUNKO_NAME_LIST_NUM1][JUNKO_NAME_LIST_NUM_SUB] =
{
#ifdef JP
	{"嫦娥","嫦娥","嫦娥","嫦娥","嫦娥","嫦娥","嫦娥","嫦娥","嫦娥","嫦娥",},//ダミー兼嫦娥
//1
	{"敵","敵","人間","人間","標的","標的","獲物","獲物","相手","相手"},//攻撃の標的など1
	{"小人","愚者","痩せ馬","鳥無き里の蝙蝠","尾を振る犬","似非者","鸚鵡","中原の鹿","蛇に睨まれた蛙","蓼食う虫"},//攻撃の標的など2
	{"虎","啄木鳥","喪家の狗","百舌鳥","鷹の前の雀","生簀の鯉","石亀","海狸","捕らぬ狸","人鳥"},//攻撃の標的など3
	{"袋の鼠","狡兎","駑馬","籠鳥","心の猿","張子の虎","行路の人","屠所の羊","小水の魚","俎上の鯉"},//攻撃の標的など4
	{"夏の虫","烏合の衆","面々の蜂","雨後の筍","蜘蛛の子","有象無象","燕雀","群羊","兎穴","鶏群"},//攻撃の標的など5 周囲や視界攻撃用
//6
	{"全て","全て","全て","世界","世界","三千世界の鴉","大地","天地","天上天下","六道衆生"},//攻撃の標的など　フロア攻撃
	{"","","ための","ための","ための","だけの","だけの","だけの","ためだけの","ためだけの"},//「ための」など
	{"","純粋な","純粋な","単純な","単純な","平易な","至純なる","無名の","無名の","飾りのない"},//「純粋な」など
	{"を和らげる","を和らげる","を防ぐ","に対抗する","へ対抗する","を緩和する","を緩和する","に抵抗する","に抗う","に抗う"},//防御系(弱)
	{"を弾く","を弾く","を消し去る","を消し去る","を無力化する","を無力化する","が効かなくなる","を打ち消す","を打ち消す","が効かなくなる"},//防御系(強)
//11
	{"弾","弾","弾","弾","矢","矢","矢","光弾","光弾","光弾"},//攻撃：ボルト
	{"弾幕","弾幕","弾幕","弾幕","光球","光球","光球","珠","玉","球"},//攻撃：ボール、ロケット
	{"光","光","光","閃き","閃き","閃き","煌めき","煌めき","光線","光線"},//攻撃：ビーム
	{"嵐","嵐","渦","渦","波動","波動","光","光","弾幕","弾幕"},//攻撃：周囲、視界内
	{"言葉","言葉","叫び","嘆き","ささやき","ささやき","嘆息","嘆息","溜息","溜息"},//ブレス
//16
	{"力","力","力","権能","光","光","意志","意志","願い","願い"},//攻撃：フロア
	{"我が身","我が身","自ら","自ら","この身","この身","この躰","この躰","自身","自身"},//自分用特技
	{"に活力をもたらす","に活力をもたらす","に活力をもたらす","の疲れを取り去る","の疲れを取り去る","の疲れを拭い去る","の疲れを拭い去る","に力を取り込む","を再び動かす","を再び動かす"},//空腹充足
	{"力","力","遣方","作用","策","手立て","方法","手段","方途","所作"},//補助特技など
	{"の傷を癒す","の傷を癒す","の傷を癒す","の傷を塞ぐ","の傷を塞ぐ","の血を洗う","の血を洗う","を治す","を治す","を治療する"},//回復
//21
	{"を見つける","を見つける","を見つけ出す","を見つけ出す","を見出す","をあぶり出す","を照らし出す","を感じ取る","をいぶし出す","をいぶし出す"},//感知
	{"罠","罠","罠","仕掛け","仕掛け","敵の策","敵の策","愚策","愚策","愚策"},//トラップ
	{"物体","物体","物品","物品","もの","もの","玩具","玩具","佩物","得物"},//アイテム
	{"世界","世界","天下","森羅万象","天地","千里の野","山海","世","無辺","三界"},//フロア対象(攻撃以外)
	{"に蒙を啓く","に蒙を啓く","に蒙を啓く","に蒙を啓く","に光をもたらす","に光をもたらす","に光をもたらす","を見通す","を見通す","を見透かす"},//啓蒙
//26
	{"の穢れを拭い去る","の穢れを拭い去る","の穢れを拭い去る","に力を取り戻す","に力を取り戻す","に力を取り戻す","の枷を消し去る","の枷を消し去る","を常態に復する","の機能を修復する"},//能力、経験値復活
	{"彼方へと跳躍する","彼方へと跳躍する","瞬きと共に移動する","瞬きと共に移動する","空間を歪める","空間を歪める","ここから消える","ここから消える","この場を離脱する","この場を離脱する"},//テレポ
	{"を瞬く間に飛ぶ","を瞬く間に飛ぶ","を一足に飛ぶ","を一足に飛ぶ","を一息に駆ける","を一息に駆ける","にあまねく去来する","へと扉を開く","への門を開け放つ","へ道を繋ぐ"},//次元の扉
	{"を斬り払う","を追い散らす","を蹴散らす","を蹴散らす","を撫で斬りにする","を撫で斬りにする","を踏みしだく","を踏みにじる","を切り捌く","をちぎっては投げる"},//周囲攻撃
	{"へ斬りかかる","に斬りかかる","へ飛び掛かる","に跳び掛かる","を追いかける","を追いかける","へ追いすがる","との間合いを詰める","との距離を消す","へ不意打ちを見舞う"},//入身
//31
	{"をなますにする","を叩きのめす","を滅多打ちにする","を滅多打ちにする","を可愛がる","を可愛がる","をいたぶり続ける","をいたぶり続ける","をいたぶり続ける","をいたぶり続ける"},//連続攻撃
	{"を癒す","を癒す","を癒す","の病魔を払う","の病魔を払う","の異常を治す","の異常を治す","の熱を冷ます","の熱を冷ます","に纏わりつく幻覚を消す"},//状態異常治癒
	{"を純化する","を純化する","の因果を修復する","の因果を修復する","を復旧する","を復旧する","を再生する","を再生する","を再構成する","を再構成する"},//状態異常治癒＋回復＋変異治癒
	{"力","力","力","光","光","光","思い","思い","因子","因子"},//名詞ほか
	{"光線を逸らす","光線を逸らす","光線を逸らす","敵弾を弾く","敵弾を弾く","敵弾を弾く","弾幕を避ける","弾幕を避ける","レーザーを消し去る","ビームを無力化する"},//ビーム・ボルト無効
//36
	{"方策","干渉","防御","防御","対策","反計","措置","介入","覆い","保護"},//名詞（防御用）
	{"飛来物","飛来物","爆発","爆発","爆風","爆風","暴風","嵐","ボム","ボム"},//ボール、ロケット
	{"放射攻撃","放射攻撃","息吹","息吹","ブレス","ブレス","ブレス","ブレス","噴射攻撃","噴射攻撃"},//ブレス
	{"破壊の力","破壊の力","破壊の光","破壊の光","滅びの光","滅びの光","破壊の言葉","破壊の言葉","*破壊*","*破壊*"},//*破壊*阻止
	{"魔力","魔力","魔力","魔法","魔法","霊力","仙力","術法","呪文","術"},//魔法など
//41
	{"を吸収する","を吸収する","を吸収する","を食う","を食う","を我が力とする","を変換する","を吸い取る","を転化する","を転用する"},//吸収
	{"岩を穿つ","岩を穿つ","道なき道を通る","道なき道を通る","穴を掘る","穴を掘る","壁を壊す","壁を壊す","岩を砕く","岩を砕く"},//壁掘り移動
	{"玩具","玩具","手妻","手妻","道具","道具","道具","杖","杖","杖"},//魔道具
	{"の秘めたる力を開放する","の秘めたる力を開放する","の秘めたる力を開放する","の力を強化する","の力を強化する","を上手に使う","を上手に使う","を上手に使う","の効果を増す","の効果を増す"},//魔道具強力使用
	{"を強靭にする","を強靭にする","に剛力をもたらす","に剛力をもたらす","を強壮たらしめる","を強壮たらしめる","を健康にする","を健康にする","に武勇の誉れをもたらす","に武勇の誉れをもたらす"},//肉体強化
//46
	{"精神","精神","精神","心","心","心","知性","知性","気力","気力"},//精神
	{"を研ぎ澄ませる","を研ぎ澄ませる","を研ぎ澄ませる","を高める","を高める","を高める","を強く持つ","を強く持つ","の働きを活発にする","の働きを活発にする"},//精神強化
	{"全て","全て","全て","全て","あらゆる攻撃","あらゆる攻撃","害意","害意","悪意","悪意"},//無敵化
	{"の螺子を巻く","の螺子を巻く","の螺子を巻く","を加速する","を加速する","を加速する","の動きを速める","の動きを速める","の動きを速める","の動きを速める"},//加速
	{"外力","外力","外力","腕力","腕力","狼藉","暴力","暴力","武力","武力"},//物理防御上昇
//51
	{"から身を隠す","から身を隠す","から身を隠す","の影に隠れる","の影に隠れる","の影に隠れる","の意識から消える","の意識から消える","の死角に入る","の死角に入る"},//超隠密
	{"壁","壁","壁","岩","岩","大地","大地","岩塊","岩壁","土壁"},//壁抜け1
	{"を抜ける","を抜ける","をすり抜ける","をすり抜ける","を通り抜ける","に姿を隠す","と同化する","と同化する","に身を隠す","に身を隠す"},//壁抜け2
	{"を寄らば斬る","を寄らば斬る","を寄らば斬る","を寄らば斬る","を篩いにかける","を剪定する","を剪定する","を桂剥きする","を間引く","を笹掻きにする"},//破片オーラ
	{"元素","元素","元素","元素の力","元素の力","精霊力","精霊力","精霊力","地水火風","五行"},//エレメント耐性
//56
	{"に力を付与する","に力を付与する","に力を付与する","に力を付与する","に力を付与する","を加工する","を加工する","を加工する","に一味加える","に一味加える"},//スレイ、耐性付与
	{"の本質を露わにする","をよく見る","を良く見る","を見定める","を見定める","を見定める","を品定めする","を品定めする","を目利きする","を目利きする"},//鑑定
	{"を沈黙に閉ざす","を沈黙に閉ざす","から声を奪う","から声を奪う","に静寂をもたらす","に静寂をもたらす","を黙らせる","を黙らせる","を静かにする","を静かにする"},//静寂
	{"を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる","を壊滅させる"},//混沌招来
	{"を追い払う","を追い払う","を追い払う","を遠ざける","を遠ざける","を遠ざける","に消えてもらう","に消えてもらう","を視界から消す","を視界から消す"},//視界内アウェイ、周辺抹殺
//61
	{"を睨み殺す","を睨み殺す","を睨め殺す","を睨め殺す","の肝をつぶす","の肝をつぶす","を射抜く","を射抜く","を刺す","を刺す"},//死の光線
	{"目","目","目","視線","視線","視線","一睨","一睨","睥睨","睥睨"},//死の光線2
	{"川","川","海","海","湖","湖","大河","大河","滝","滝"},//海が割れる日1
	{"を涸らす","を涸らす","を割る","を割る","を割る","を干上がらせる","を干上がらせる","を干上がらせる","の水を巻き上げる","の水を巻き上げる"},//海が割れる日2
	{"を射竦める","を射竦める","を射竦める","を射竦める","を竦ませる","を竦ませる","を竦ませる","を呼び止める","を呼び止める","を呼び止める"},//金縛り
//66
	{"招かれざる客","招かれざる客","邪魔者","邪魔者","新たな敵","新たな敵","敵影","敵影","邪魔立てする者","邪魔立てする者"},//召喚阻害
	{"ここではない何処かへ行く","永い夢から覚める","世界を塗り替える","別の世界を夢見る","覚めない夢を見る","ここではない何処かへ行く","永い夢から覚める","世界を塗り替える","別の世界を夢見る","覚めない夢を見る"},//現実変容
	{"星を落とす","星を落とす","星を落とす","星を落とす","星を落とす","星を落とす","星を落とす","星を落とす","星を落とす","星を落とす"},//流星群
	{"曇りなき","直截な","無垢な","原初の","本質的な","本質的な","原始の","最初の","名もなき","名も無き"},//純粋2
	{"方策","やり方","手並み","手の内","手","効験","気質","効力","手管","仕掛け"},//補助特技など2
//71
	{"が光を放つ","が光を放つ","が光を放つ","が後光を纏う","が後光を纏う","が後光を纏う","が暗闇を照らす","が暗闇を照らす","が暗闇を照らす","が暗闇を照らす"},
	{"を万夫不当の猛者とする","が武芸百般に通じる","が武芸百般に通じる","が達人の手練を得る","に達人の手練をもたらす","を無双の境地に至らしめる","を万夫不当の猛者にする","が武芸百般に通じる","に達人の手練をもたらす","を無双の境地に至らしめる"},
	{"大地を","大地を","大地を","大地を","地を","地を","地を","地盤を","地盤を","地盤を"},
	{"揺らす","揺らす","揺らす","揺るがす","揺るがす","揺るがす","揺さぶる","揺さぶる","崩す","崩す"},
	{"幸運を引き寄せる","幸運を引き寄せる","幸運を掴み取る","運命を捻じ曲げる","運命を捻じ曲げる","豪運をもたらす","運気を高める","運気を高める","賽の目を操る","賽の目を操る"},
#else
	{"Chang'e","Chang'e","Chang'e","Chang'e","Chang'e","Chang'e","Chang'e","Chang'e","Chang'e","Chang'e",},//ダミー兼嫦娥
//1
	{"Enemy","Enemy","Human","Human","Target","Target","Prey","Prey","Opponent","Opponent"},//攻撃の標的など1
	{"Inchling","Fool","Horse","Bat","Dog","Hypocrite","Parrot","Deer","Frog","Insects"},//攻撃の標的など2
	{"Tiger","Woodpecker","Hound","Shriek","Sparrow","Carp","Turtle","Beaver","Tanuki","Birdman"},//攻撃の標的など3
	{"Cornered Rat","Wily Rabbit","Kazuma","Caged Bird","Monkey","Paper Tiger","Travller","Sheep","Fish","Carp"},//攻撃の標的など4
	{"Summer Insects","Disorderly Flock","Hundred Bees","Bamboo Shoots","Spiders","Elephant","Swallow","Flock","Rabbit Hole","Rabbit Swarm"},//攻撃の標的など5 周囲や視界攻撃用
//6
	{"All","All","All","World","World","World","Earth","Heaven and Earth","All Over Heaven","Six Paths"},//攻撃の標的など　フロア攻撃
	{"","","For","For","For","Only","Only","Only","Only For","Only For"},//「ための」など
	{"","Pristine","Pristine","Simple","Simple","Plain","Pure","Nameless","Nameless","Undecorated"},//「純粋な」など
	{"Relieving","Relieving","Protecting","Counteracting","Resisting","Supporting","Supporting","Resisting","Enduring","Enduring"},//防御系(弱)
	{"Striking","Striking","Erasing","Erasing","Depowering","Depowering","Unaffecting","Countering","Countering","Withstanding"},//防御系(強)
//11
	{"Shot","Shot","Shot","Shot","Bolt","Bolt","Bolt","Light Shot","Light Shot","Light Shot"},//攻撃：ボルト
	{"Danmaku","Danmaku","Danmaku","Danmaku","Light Orb","Light Orb","Light Orb","Pearl","Orb","Sphere"},//攻撃：ボール、ロケット
	{"Light","Light","Light","Flashing","Flashing","Flashing","Sparkling","Sparkling","Light Ray","Light Ray"},//攻撃：ビーム
	{"Storm","Storm","Whirlpool","Whirlpool","Wave-Motion","Wave-Motion","Light","Light","Danmaku","Danmaku"},//攻撃：周囲、視界内
	{"Word","Word","Shout","Grief","Whisper","Whisper","Sigh","Sigh","Breath","Breath"},//ブレス
//16
	{"Power","Power","Power","Authority","Light","Light","Will","Will","Wish","Wish"},//攻撃：フロア
	{"My Body","My Body","Myself","Myself","This Body","This Body","This Form","This Form","Myself","Myself"},//自分用特技
	{"Vitalizing","Vitalizing","Vitalizing","Invigorating","Invigorating","Invigorating","Invigorating","Empowering","Reactivating","Reactivating"},//空腹充足
	{"Power","Power","Using","Acting","Measure","Means","Law","Means","Way","Behaviour"},//補助特技など
	{"Healing","Healing","Curing","Recovering","Recovering","Bloodwashing","Bloodwashing","Healing","Healing","Healing"},//回復
//21
	{"Discovering","Discovering","Discovering","Discovering","Discovering","Detecting","Illuminating","Sensing","Tracking","Tracking"},//感知
	{"Trap","Trap","Trap","Mechanism","Mechanism","Enemy Plan","Enemy Plan","Foolish Plan","Foolish Plan","Foolish Plan"},//トラップ
	{"Items","Items","Items","Items","Items","Items","Tools","Tools","Things","Treasures"},//アイテム
	{"World","World","Under Heaven","All Creation","Heaven and Earth","Thousand Ri","Mountains an Sea","World","Unlimited","Three Worlds"},//フロア対象(攻撃以外)
	{"Enlightening","Enlightening","Enlightening","Enlightening","Illuminating","Illuminating","Illuminating","Seeing Through","Seeing Through","Seeing Through"},//啓蒙
//26
	{"Wiping Impurity","Wiping Impurity","Wiping Impurity","Restoring Power","Restoring Power","Restoring Power","Removing Restraints","Removing Restraints","Returning to Normal","Recovering Function"},//能力、経験値復活
	{"Jumping Away","Jumping Away","Instantly Moving","Instantly Moving","Distorting Space","Distorting Space","Vanishing Away","Vanishing Away","Leaving this Place","Leaving this Place"},//テレポ
	{"Instantly Traveling","Instantly Traveling","Instantly Traveling","Instantly Traveling","Instantly Traveling","Instantly Traveling","Coming and Going","Opening Doors","Opening Gates","Opening Routes"},//次元の扉
	{"Cleaving","Banishing","Dispersing","Dispersing","Striking Down","Striking Down","Trampling","Trampling","Reaping","Tearing Away"},//周囲攻撃
	{"Slashing","Slashing","Charging","Leaping Into","Pursuing","Pursuing","Pursuing","Cornering","Closing Distance","Surprise Attacking"},//入身
//31
	{"Beating Down","Beating Down","Cutting Down","Cutting Down","Cutting Down","Cutting Down","Continuous Striking","Continuous Striking","Continuous Striking","Continuous Striking"},//連続攻撃
	{"Curing","Curing","Curing","Healing Sickness","Healing Sickness","Healing Abnormalities","Healing Abnormalities","Cooling Fever","Cooling Fever","Cleaning Hallucinations"},//状態異常治癒
	{"Purifying","Purifying","Restoring Karma","Restoring Karma","Restoring","Restoring","Rebirth","Rebirth","Recreating","Recreating"},//状態異常治癒＋回復＋変異治癒
	{"Power","Power","Power","Light","Light","Light","Thought","Though","Factor","Factor"},//名詞ほか
	{"Diverting Rays","Diverting Rays","Diverting Rays","Dodging Bullets","Dodging Bullets","Dodging Bullets","Dodging Danmaku","Dodging Danmaku","Erasing Lasers","Negating Beams"},//ビーム・ボルト無効
//36
	{"Measure","Interference","Defense","Denfese","Countermeasure","Countermeasure","Means","Intervention","Cover","Protection"},//名詞（防御用）
	{"Flying","Flying","Explosion","Explosion","Explosion","Explosion","Storm","Storm","Bomb","Bomb"},//ボール、ロケット
	{"Blast Attack","Blast Attack","Breath","Breath","Breath","Breath","Breath","Breath","Breath Attack","Breath Attack"},//ブレス
	{"Destructive Power","Destructive Power","Destructive Light","Destructive Light","Light of Ruin","Light of Ruin","Word of Destruction","Word of Destruction","*Destruction*","*Destruction*"},//*破壊*阻止
	{"Mana","Mana","Mana","Magic","Magic","Spirit","Hermit Power","Technique","Spell","Arts"},//魔法など
//41
	{"Draining","Draining","Draining","Consuming","Consuming","Turning to Power","Converting","Absorbing","Converting","Diverting"},//吸収
	{"Stone Striking","Stone Striking","Opening Ways","Opening Ways","Tunnel Digging","Tunnel Digging","Wall Breaking","Wall Breaking","Rock Crushing","Rock Crushing"},//壁掘り移動
	{"Tool","Tool","Means","Means","Device","Device","Device","Staff","Staff","Staff"},//魔道具
	{"Unleashing Power","Unleashing Power","Unleashing Power","Raising Power","Raising Power","Properly Using","Properly Using","Properly Using","Effect Empowering","Effect Empowering"},//魔道具強力使用
	{"Raising Strength","Raising Strength","Granting Strength","Granting Strength","Granting Power","Granting Power","Invigorating","Invigorating","Granting Heroism","Granting Heroism"},//肉体強化
//46
	{"Mind","Mind","Mind","Spirit","Spirit","Spirit","Knowledge","Knowledge","Spirit Power","Spirit Power"},//精神
	{"Sharpening","Sharpening","Sharpening","Raising","Raising","Raising","Empowering","Empowering","Activating","Activating"},//精神強化
	{"All","All","All","All","Any Attacks","Any Attacks","Harm","Harm","Malice","Malice"},//無敵化
	{"Winding","Winding","Winding","Hastening","Hastening","Hastening","Raising Speed","Raising Speed","Raising Speed","Raising Speed"},//加速
	{"External Power","External Power","External Power","Strength","Strength","Wolf","Explosive Power","Explosive Power","Warrior Power","Warrior Power"},//物理防御上昇
//51
	{"Concealing","Concealing","Concealing","Hiding in Shadows","Hiding in Shadows","Hiding in Shadows","Vanishing from Mind","Vanishing from Mind","Entering Blind Spot","Entering Blind Spot"},//超隠密
	{"Wall","Wall","Wall","Rock","Rock","Earth","Earth","Stone","Stone Wall","Earth Wall"},//壁抜け1
	{"Passing","Passing","Going Through","Going Through","Passing Through","Hiding Inside","Becoming One With","Becoming One With","Hiding Your Form","Hiding Your Form"},//壁抜け2
	{"Slashing at Approach","Slashing at Approach","Slashing at Approach","Slashing at Approach","Decorated with Thorns","Pruning","Pruning","Cleaving Away","Thinning Out","Scraping Away"},//破片オーラ
	{"Elements","Elements","Elements","Element Power","Element Power","Spirit Power","Spirit Power","Spirit Power","Earth, Water, Fire, Wind","Five Elements"},//エレメント耐性
//56
	{"Imbuing with Power","Imbuing with Power","Imbuing with Power","Imbuing with Power","Imbuing with Power","Enhancing","Enhancing","Enhancing","Tweaking","Tweaking"},//スレイ、耐性付与
	{"Revealing true Form","Appraising","Appraising","Appraising","Appraising","Appraising","Identifying","Identifying","Appraising","Appraising"},//鑑定
	{"Silencing","Silencing","Voice Stealing","Voice Stealing","Bestowing Silence","Bestowing Silence","Shutting Up","Shutting Up","Making Silent","Making Silent"},//静寂
	{"Devastating","Devastating","Devastating","Devastating","Devastating","Devastating","Devastating","Devastating","Devastating","Devastating"},//混沌招来
	{"Banishing","Banishing","Banishing","Throwing Away","Throwing Away","Throwing Away","Making Disappear","Making Disappear","Removing from Sight","Removing from Sight"},//視界内アウェイ、周辺抹殺
//61
	{"Killing with Gaze","Killing with Gaze","Killing with Gaze","Killing with Gaze","Liver Crushing","Liver Crushing","Shooting Through","Shooting Through","Skewering","Skewering"},//死の光線
	{"Eye","Eye","Eye","Beam","Beam","Beam","Glaer","Glare","Stare","Staer"},//死の光線2
	{"River","River","Sea","Sea","Lake","Lake","Great River","Great River","Waterfall","Waterfall"},//海が割れる日1
	{"Dripping","Dripping","Splititng","Splititng","Splititng","Drying","Drying","Drying","Water Withdrawing","Water Withdrawing"},//海が割れる日2
	{"Binding","Binding","Binding","Binding","Stopping","Stopping","Stopping","Calling to Stop","Calling to Stop","Calling to Stop"},//金縛り
//66
	{"Uninvited Guests","Uninvited Guests","Annoyances","Annoyances","New Enemies","New Enemies","Enemy Shadows","Enemy Shadows","Those in My Way","Those in My Way"},//召喚阻害
	{"Go Away Somewher Else","Awaken from Eternal Dream","Go to Another World","See Dreams of Another World","See an Unfamiliar World","Go Away Somewher Else","Awaken from Eternal Sleep","Go to Another World","See Dreams of Another World","See an Unfamiliar World"},//現実変容
	{"Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars","Dropping Stars"},//流星群
	{"Cloudless","Straightforward","Innocent","Original","Essential","Essential","Prime","Original","Nameless","Nameless"},//純粋2
	{"Means","Ways of Doing","Hands-On","In my Hand","Hand","Efficacy","Temperament","Effect Power","Means","Mechanism"},//補助特技など2
//71
	{"Light Releasing","Light Releasing","Light Releasing","Afterglowing","Afterglowing","Afterglowing","Illuminating Darkness","Illuminating Darkness","Illuminating Darkness","Illuminating Darkness"},
	{"Bringing Unjustified Ferocity","Familiarizing with Combat Arts","Familiarizing with Combat Arts","Obtaining Mastery Training","Obtaining Mastery Training","Becoming Unrivaled","Bringing Unjustified Ferocity","Familiarizing with Combat Arts","Obtaining Mastery Training","Becoming Unrivaled"},
	{"Earth","Earth","Earth","Earth","Ground","Ground","Ground","Surface","Surface","Surface"},
	{"Shaking","Shaking","Shaking","Trembling","Trembling","Trembling","Trembling","Trembling","Breaking","Breaking"},
	{"Drawing on Good Fortune","Drawing on Good Fortune","Invoking Good Fortune","Warping Fortune","Warping Fortune","Bringing Good Luck","Raising Luck","Raising Luck","Manipulating Fortune","Manipulating Fortune"},
#endif


//	{"","","","","","","","","",""},//

};

//純狐技生成名リスト2　属性(JKF2_*の数)に対応　攻撃用
cptr	junko_arts_name_words_2_atk[JKF2_END - JKF2_START + 1][JUNKO_NAME_LIST_NUM_SUB] =
{
#ifdef JP
	{"を溶かす","を溶かす","を溶かす","を溶かす","を爛れさせる","を爛れさせる","を浸蝕する","を浸蝕する","を消化する","を消化する"},//酸
	{"を痺れさせる","を痺れさせる","を痺れさせる","を痺れさせる","を痺れさせる","を感電させる","を感電させる","を感電させる","を感電させる","を感電させる",},//電撃
	{"を燃やす","を燃やす","を燃やす","を焼く","を焼く","を焼く","を焦がす","を焦がす","を焦がす","を焼却する",},//火炎、溶岩生成
	{"を冷やす","を冷やす","を冷やす","を凍らせる","を凍らせる","を凍らせる","を凍えさせる","を凍えさせる","を冷凍する","を冷凍する"},//冷気
	{"を苦しめる","を苦しめる","を苦しめる","を病に冒す","を病に冒す","に悪い風を入れる","に悪い風を入れる","をいたぶる","をいたぶる","をいたぶる"},//毒
	{"の目を潰す","の目を潰す","の目を潰す","を光輝に拉ぐ","を光輝に拉ぐ","を光に包む","を光に包む","を光に包む","を焼く","を加熱する",},//閃光
	{"を闇に包む","を闇に包む","を闇に包む","の目を覆う","の目を覆う","の目を覆う","を塗り潰す","を塗り潰す","を暗闇で覆う","を暗闇で覆う"},//暗黒
	{"を切り裂く","を切り裂く","を切り裂く","を切り裂く","を傷つける","を傷つける","を穴だらけにする","を襤褸布にする","を襤褸布にする","を襤褸布にする"},//破片
	{"を溺れさせる","を溺れさせる","を水責めにする","を水責めにする","を水責めにする","を沈める","を沈める","を沈める","を魚の餌にする","を魚の餌にする",},//水、水地形生成
	{"へ歌いかける","へ歌いかける","の耳を潰す","の耳を潰す","を打ちのめす","を打ちのめす","に呼びかける","に呼びかける","に呼びかける","に呼ばわる",},//轟音
	{"を焼き払う","を焼き払う","を焼き払う","を核の炎で包む","を核の炎で包む","を核の炎で包む","を消し炭にする","を消し炭にする","を焼き尽くす","を焼き尽くす"},//核熱
	{"を引き裂く","を引き裂く","を引き裂く","を滅ぼす","を滅ぼす","を滅ぼす","を破壊する","を破壊する","を破壊する","を消し去る"},//魔力
	{"を押し潰す","を押し潰す","を押し潰す","を押し潰す","を打ち壊す","を打ち壊す","を打ち壊す","を打ち壊す","を打ち壊す","を打ち壊す"},//隕石
	{"を引っ掻き回す","を引っ掻き回す","に混沌をもたらす","に混沌をもたらす","に混沌をもたらす","を変容させる","を変容させる","を変容させる","を変異させる","を変異させる"},//カオス
	{"を殺す","を殺す","を殺す","を殺す","を殺す","を殺す","に穢れをもたらす","に穢れをもたらす","に穢れをもたらす","に穢れをもたらす",},//地獄
	{"の力を削ぐ","の力を削ぐ","の力を削ぐ","の力を削ぐ","を冴えなくする","を冴えなくする","から力を奪う","から力を奪う","から力を奪う","から力を奪う",},//劣化
	{"を若返らせる","を若返らせる","の因果を絶つ","の因果を絶つ","の因果を絶つ","の因果を絶つ","をあの頃に戻す","をあの頃に戻す","をあの頃に戻す","をあの頃に戻す"},//時間逆転
	{"を歪める","を歪める","を歪める","を歪める","を歪める","を捻じ曲げる","を捻じ曲げる","を捻じ曲げる","を捻じ曲げる","を捻じ曲げる"},//空間歪曲
	{"を浄化する","を浄化する","を浄化する","を浄化する","を浄化する","を浄化する","を浄化する","を浄化する","を浄化する","を浄化する",},//破邪
	{"を地獄に落とす","を地獄に落とす","を地獄に落とす","を地獄に落とす","を地獄の釜に放り込む","に地獄の扉を開く","に地獄の扉を開く","を地獄の炉にくべる","を地獄の炉にくべる","を地獄の炉にくべる",},//地獄の劫火
	{"を消す","を消す","を消す","を消す","を消し去る","を消し去る","を消し去る","を消滅させる","を消滅させる","を消滅させる",},//分解
	{"を吹き飛ばす","を吹き飛ばす","を吹き飛ばす","を吹き飛ばす","を吹き散らす","を吹き散らす","を吹き散らす","を宙に舞わせる","を宙に舞わせる","を宙に舞わせる",},//竜巻
	{"を蒸す","を蒸す","を蒸す","を蒸す","を蒸す","を蒸し焼きにする","を蒸し焼きにする","を蒸し焼きにする","を蒸し焼きにする","を蒸し焼きにする",},//蒸気
	{"を埋める","を埋める","を埋める","を閉じ込める","を閉じ込める","を捕まえる","を捕まえる","を生き埋めにする","を生き埋めにする","を生き埋めにする",},//岩石生成
	{"をドアで挟む","をドアで挟む","をドアで挟む","をドアで挟む","をドアで挟む","を追い詰める","を追い詰める","を追い詰める","を閉じ込める","を閉じ込める",},//ドア生成
	{"を緑化する","を緑化する","を緑化する","を茨で囲む","を茨で囲む","を縛り付ける","を縛り付ける","に花を咲かせる","に花を咲かせる","に花を咲かせる",},//森生成
#else
    {"Melting","Melting","Melting","Melting","Melting","Melting","Corroding","Corroding","Dissolving","Dissolving"},//酸
	{"Zapping","Zapping","Zapping","Zapping","Zapping","Electrifying","Electrifying","Electrifying","Electrifying","Electrifying",},//電撃
	{"Burning","Burning","Burning","Scorching","Scorching","Scorching","Blazing","Blazing","Blazing","Incinerating",},//火炎、溶岩生成
	{"Cooling","Cooling","Cooling","Freezing","Freezing","Freezing","Freezing","Freezing","Refrigerating","Refrigerating"},//冷気
	{"Harming","Harming","Harming","Sickness Inducing","Sickness Inducing","Sickness Inducing","Sickness Inducing","Plaguing","Plaguing","Plaguing"},//毒
	{"Blinding","Blinding","Blinding","Shining","Shining","Bedazzling","Bedazzling","Bedazzling","Scorching","Scorching",},//閃光
	{"Darkening","Darkening","Darkening","Eye Clouding","Eye Clouding","Eye Clouding","Obscuring","Obscuring","Night Bringing","Night Bringing"},//暗黒
	{"Cleaving","Cleaving","Cleaving","Cleaving","Wounding","Wounding","Hole Punching","Shredding","Shredding","Shredding"},//破片
	{"Drowning","Drowning","Submerging","Submerging","Submerging","Sinking","Sinking","Sinking","Feeding to Fishes","Feeding to Fishes",},//水、水地形生成
	{"Singing","Singing","Ear Destroying","Ear Destroying","Resounding","Resounding","Calling","Calling","Calling","Calling",},//轟音
	{"Roasting","Roasting","Roasting","Nuclear Heating","Nuclear Heating","Nuclear Heating","Turning to Ashes","Turning to Ashes","Utterly Scorching","Utterly Scorching"},//核熱
	{"Tearing","Tearing","Tearing","Destroying","Destroying","Destroying","Devastating","Devastating","Devastating","Erasing"},//魔力
	{"Striking","Striking","Striking","Striking","Crushing","Crushing","Crushing","Crushing","Crushing","Crushing"},//隕石
	{"Twisting","Twisting","Flooding with Chaos","Flooding with Chaos","Flooding with Chaos","Warping","Warping","Warping","Transforming","Transforming"},//カオス
	{"Killing","Killing","Killing","Killing","Killing","Killing","Impurifying","Impurifying","Impurifying","Impurifying",},//地獄
	{"Power Draining","Power Draining","Power Draining","Power Draining","Dulling","Dulling","Power Stealing","Power Stealing","Power Stealing","Power Stealing",},//劣化
	{"Deaging","Deaging","Cutting Causality","Cutting Causality","Cutting Causality","Cutting Causality","Returning to Past","Returning to Past","Returning to Past","Returning to Past"},//時間逆転
	{"Distorting","Distorting","Distorting","Distorting","Distorting","Warping","Warping","Warping","Warping","Warping"},//空間歪曲
	{"Purifying","Purifying","Purifying","Purifying","Purifying","Purifying","Purifying","Purifying","Purifying","Purifying",},//破邪
	{"Dropping to Hell","Dropping to Hell","Dropping to Hell","Dropping to Hell","Dropping to Hell Cauldron","Opening Gates of Hell","Opening Gates of Hell","Throwing into Hell","Throwing into Hell","Throwing into Hell",},//地獄の劫火
	{"Erasing","Erasing","Erasing","Erasing","Banishing","Banishing","Banishing","Wiping Away","Wiping Away","Wiping Away",},//分解
	{"Blowing Away","Blowing Away","Blowing Away","Blowing Away","Scattering","Scattering","Scattering","Throwing in Air","Throwing in Air","Throwing in Air",},//竜巻
	{"Steaming","Steaming","Steaming","Steaming","Steaming","Steam Scalding","Steam Scalding","Steam Scalding","Steam Scalding","Steam Scalding",},//蒸気
	{"Burying","Burying","Burying","Sealing In","Sealing In","Capturing","Capturing","Burying Alive","Burying Alive","Burying Alive",},//岩石生成
	{"Capturing in Doors","Capturing in Doors","Capturing in Doors","Capturing in Doors","Capturing in Doors","Cornering","Cornering","Cornering","Sealing In","Sealing In",},//ドア生成
	{"Viridifying","Viridifying","Viridifying","Surrounding with Thorns","Surrounding with Thorns","Restraining","Restraining","Blooming Flowers","Blooming Flowers","Blooming Flowers",},//森生成
#endif
};
/*
//純狐技生成名リスト2　属性(JKF2_*の数)に対応　防御用
cptr	junko_arts_name_words_2_def[JKF2_END - JKF2_START + 1][JUNKO_NAME_LIST_NUM_SUB] =
{
	{"酸","酸","酸","酸","酸","酸","腐蝕","腐蝕","腐蝕性物質","腐蝕性物質"},//酸
	{"電撃","電撃","電撃","雷","雷","稲妻","稲妻","落雷","落雷","雷"},//電撃
	{"炎","炎","火","火","火炎","火炎","熱","熱","炎","炎",},//火炎
	{"冷気","冷気","冷気","冷気","冷気","氷","氷","氷","氷","氷"},//冷気
	{"毒","毒","毒","毒物","毒物","瘴気","瘴気","悪い風","病魔","病魔",},//毒
	{"光","光","光","光","熱線","熱線","閃光","閃光","閃光","閃光"},//閃光
	{"闇","闇","闇","闇","暗闇","暗闇","暗黒","暗黒","暗黒","暗黒",},//暗黒
	{"破片","破片","破片","破片","破片","飛礫","飛礫","飛礫","飛礫","飛礫"},//破片
	{"水","水","水","水","水流","水流","水流","水流","流水","流水"},//水
	{"轟音","轟音","轟音","轟音","音","音","音","衝撃","衝撃","衝撃",},//轟音
	{"核の炎","核の炎","核の炎","核の炎","核の炎","核熱","核熱","核熱","核熱","核熱"},//核熱
	{"魔力","魔力","魔力","魔力","魔力","純魔力","純魔力","純魔力","純魔力","純魔力",},//魔力
	{"星","星","星","流星","流星","流星","隕石","隕石","星辰","星辰",},//隕石
	{"混沌","混沌","混沌","混沌","原初の力","原初の力","原初の力","反秩序","反秩序","反秩序",},//カオス
	{"地獄の力","地獄の力","地獄の力","地獄の力","死穢","死穢","死穢","死穢","死穢","死穢",},//地獄
	{"劣化","劣化","劣化","劣化","劣化","反魔力","反魔力","反魔力","反魔力","反魔力",},//劣化
	{"時空の力","時空の力","時空の力","因果","因果","因果","空間の歪み","空間の歪み","空間の歪み","空間の歪み"},//時間逆転・空間歪曲共用
	{"","","","","","","","","",""},//空間歪曲　上と耐性が被るので不要
	{"破邪の力","破邪の力","破邪の力","破邪の力","聖なる力","聖なる力","聖なる力","聖なる力","聖なる力","聖なる力"},//破邪
	{"地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火","地獄の劫火"},//地獄の劫火
	{"分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃","分解攻撃"},//分解
	{"風","風","風","風","竜巻","竜巻","竜巻","竜巻","野分","野分",},//竜巻
	{"","","","","","","","","","",},//蒸気：敵が使わないので不要
	{"","","","","","","","","","",},//岩生成：敵が使わないので不要
	{"","","","","","","","","","",},//ドア生成：敵が使わないので不要
	{"","","","","","","","","","",},//森生成：敵が使わないので不要
};
*/

/*:::純狐技作成のためのフラグ加算処理*/
void	add_junko_flag_val(int flag_num, int add_val)
{
	int i;

	int new_val;

	if(p_ptr->pclass != CLASS_JUNKO) return;

	if(flag_num < 0 || flag_num > 107)
	{
		msg_format(_("ERROR:add_junko_flag_val()に不正なnum値(%d)が入力された",
                    "ERROR: Incorrect num value (%d) in add_junko_flag_val()"));
		return;
	}

	new_val = p_ptr->magic_num2[flag_num];

	//値が増えるほど増加量を減らしておく。もっと効率良いプログラムがありそうなものだが。
	//780くらいでフラグ値が255になる。
	for(i=new_val/50;i<6;i++)
	{
		int add = MIN(50,( add_val / (i+1)));

		new_val += add;
		add_val -= add * (i+1);
		if(add_val < i+2) break;
	}

	if(new_val > 255) new_val = 255;

	p_ptr->magic_num2[flag_num] = (byte)new_val;

}


/*:::名も無き技内部パラメータ用構造体*/
//nameless_arts_generate_typeは特技を生成するときに参照されるデータの型、
//こちらは特技を発動するときに使うパラメータを一時管理するための型である
typedef struct nameless_arts_activate_param_type nameless_arts_activate_param_type;
struct nameless_arts_activate_param_type
{

	s32b	art_idx;	//nameless_arts_generate_tableのインデックス値
	s32b	gf_type;	//GF_***に対応する属性値　ところでGFとは何の略なのだろう？未だに知らない。
	s32b	cost;		//消費MP
	s32b	rad;		//半径
	s32b	base;		//基本効果値
	s32b	dice;
	s32b	sides;
	s32b	counter_idx;	//tim_general[]のどのカウンタを使うかのインデックス値 0-4
	s32b	quark_idx;	//技の名前を保存するためのquark_str()インデックス
	s32b	xtra1;		//その他パラメータ
	s32b	xtra2;

};

/*:::純狐用技生成規則リスト*/
//添字がJKF1_***に対応する
typedef struct nameless_arts_generate_type nameless_arts_generate_type;
struct nameless_arts_generate_type
{
	u16b	rarity;		//高いほど選定されにくい 1-100
	byte	min_lev; 	//使用可能レベル(weird_luck()を通ると10レベル上まで出る)
	s16b	cost;		//基礎コスト
	u16b	base; 		//基礎ベース値(レベル1を想定)
	u16b	dice;		//基礎ダイス数
	u16b	sides; 		//基礎ダイス面
	u16b	lev_coef;	//50レベルにしたときの効果上昇係数(%) 0で変化なし

	s16b	name_idx[5];//名前インデックス junko_arts_name_words_1[][]の1つめの添字に相当

	bool	need_flag2;	//JKF2_**を参考にして属性値を設定する必要があるかどうか
	bool	attack;		//攻撃用の技のときTRUE
	bool	use_counter;//tim_general[]カウンタを使うかどうか

};

/*:::「名もなき技」生成規則リスト*/
//添字がJKF1_***に対応する
const nameless_arts_generate_type nameless_arts_generate_table[JKF1_MAX+1] =
{
	{255,0,0,0,0,0,0,{0,0,0,0,0},FALSE,FALSE,FALSE},//始端ダミー
//1
	{10, 1, 3,  8, 2,  6,200,{1,-1,7,8,11},TRUE,TRUE,FALSE},//属性攻撃1 ボルト 20-60dam
	{10, 7,12, 25, 3,  9,400,{1,-1,7,8,12},TRUE,TRUE,FALSE},//属性攻撃2 ボール 50-250dam
	{15,15,10, 30, 6,  6,300,{1,-1,7,8,13},TRUE,TRUE,FALSE},//属性攻撃3 ビーム 50-200dam
	{25,30,18, 60, 8,  8,300,{1,-1,7,8,12},TRUE,TRUE,FALSE},//属性攻撃4 ロケット 100-400dam
	{40,25,30, 10, 5,  5,150,{1,-1,7,8,15},TRUE,TRUE,FALSE},//属性攻撃5 ブレス 現在HPの数分の一
	{60,40,20, 50, 1, 30,400,{1,-1,7,8,13},TRUE,TRUE,FALSE},//属性攻撃6 大型レーザー 80-400dam
	{15,10,10, 50, 3, 12,500,{1,-1,7,8,14},TRUE,TRUE,FALSE},//属性攻撃7 自分中心ボール 最大80-500dam
	{45,40,25, 30, 4,  8,400,{1,-1,7,8,14},TRUE,TRUE,FALSE},//属性攻撃8 視界内攻撃 70-300dam
	{80,57,60, 50, 1, 30,300,{6,-1,7,8,16},TRUE,TRUE,FALSE},//属性攻撃9 フロア攻撃 weird_luck要　300damくらい
	{25, 1,10,  0, 0,  0,  0,{17,18,7,19,0},FALSE,FALSE,FALSE},//空腹充足
//11
	{25, 5, 5, 20, 2,  6,300,{17,20,7,19, 0},FALSE,FALSE,FALSE},//回復(弱)
	{45,35,10,100, 0,  0,300,{17,20,7,19, 0},FALSE,FALSE,FALSE},//回復(強)
	{20, 1, 5, 20, 0,  0,200,{ 1,21,7,19, 0},FALSE,FALSE,FALSE},//モンスター感知
	{25, 1, 5, 20, 0,  0,200,{22,21,7,19, 0},FALSE,FALSE,FALSE},//罠感知
	{30, 1, 5, 20, 0,  0,200,{23,21,7,19, 0},FALSE,FALSE,FALSE},//アイテム感知
	{60,35,60,  0, 0,  0,  0,{24,25,7, 8,19},FALSE,FALSE,FALSE},//啓蒙
	{35,12,30,  0, 0,  0,  0,{17,26,7, 8,19},FALSE,FALSE,FALSE},//能力、経験値復活
	{15, 1, 7, 50, 0,  0,200,{27, 7,19, 0, 0},FALSE,FALSE,FALSE},//テレポ
	{80,35,15, 20, 0,  0,300,{24,28, 7, 8,19},FALSE,FALSE,FALSE},//次元の扉
	{40,30,30,  0, 0,  0,  0,{ 5,29, 7,19, 0},FALSE,TRUE,FALSE},//全周囲攻撃
//21
	{35,15, 6,  3, 0,  0,300,{ 1,30, 7,19, 0},FALSE,TRUE,FALSE},//入身
	{65,40,72,  0, 0,  0,  0,{ 1,31, 7, 8,19},FALSE,TRUE,FALSE},//連続攻撃 xtra1の攻撃回数分攻撃する
	{30, 5,10,  0, 0,  0,  0,{17,32, 7,19, 0},FALSE,FALSE,FALSE},//癒し
	{80,40,99,  0, 0,  0,  0,{17,33, 7, 8,34},FALSE,FALSE,FALSE},//因果修復
	{40,10,30,  0, 0,  0,  0,{23,26, 7,19, 0},FALSE,FALSE,FALSE},//解呪
//26
	{80,20,25, 10, 1, 10,100,{35,7, 8,36, 0},FALSE,FALSE,TRUE},//ビーム・ボルト無効化
	{80,30,25,  8, 1,  8,100,{37,9, 7, 8,36},FALSE,FALSE,TRUE},//爆風保護
	{80,40,25,  5, 1,  5,100,{38,9, 7, 8,36},FALSE,FALSE,TRUE},//ブレス保護
	{80,48,60,  4, 1,  4,  0,{39,9, 7, 8,36},FALSE,FALSE,TRUE},//*破壊*防御
	{60,20,20, 15, 1, 15,100,{40,41,7, 8,36},FALSE,FALSE,TRUE},//魔法吸収
//31
	{45,20,30, 20, 1, 20,  0,{42,7,19, 0, 0},FALSE,FALSE,TRUE},//壁掘り
	{55,30,50, 30, 0,  0,  0,{43,44,7,19, 0},FALSE,FALSE,TRUE},//魔道具強力使用

	{40,10,12,  7, 1,  7,200,{17,45, 7, 8,19},FALSE,FALSE,FALSE},//肉体強化 強化量をxtra1に記録
	{40,10,12,  7, 1,  7,200,{46,47, 7, 8,19},FALSE,FALSE,FALSE},//精神強化 強化量をxtra1に記録
	{90,47,80,  4, 1,  4,  0,{48,10, 7, 8,36},FALSE,FALSE,FALSE},//無敵化
//36
	{30,20,15, 10, 1, 10,100,{17,49, 7, 8,19},FALSE,FALSE,FALSE},//加速
	{40,30,20, 10, 1, 10,  0,{40, 9, 7, 8,36},FALSE,FALSE,FALSE},//魔法防御
	{30,25,25, 20, 1, 20,  0,{50, 9, 7, 8,36},FALSE,FALSE,FALSE},//AC+50
	{70,40,60, 10, 1, 10,  0,{48, 9, 7, 8,36},FALSE,FALSE,FALSE},//魔法の鎧
	{80,45,70, 25, 1, 25,  0,{ 1,51, 7, 8,19},FALSE,FALSE,FALSE},//超隠密
//41
	{60,35,50, 20, 1, 20,  0,{52,53, 7, 8,19},FALSE,FALSE,FALSE},//壁抜け
	{40,20,30, 20, 1, 20,  0,{ 5,54, 7, 8,19},FALSE,FALSE,FALSE},//破片オーラ
	{35,15,20, 15, 1, 15,100,{55, 9, 7, 8,36},FALSE,FALSE,FALSE},//エレメント耐性
	{80,45,75, 15, 1, 15,  0,{48, 9, 7, 8,19},FALSE,FALSE,FALSE},//究極の耐性
	{60,15,80,  0, 0,  0,  0,{23,56, 7, 8,19}, TRUE,FALSE,FALSE},//武器への能力付与
//46
	{60,15,80,  0, 0,  0,  0,{23,56, 7, 8,19}, TRUE,FALSE,FALSE},//装備品への耐性付与
	{35,10,15,  0, 0,  0,100,{23,57, 0, 0, 0},FALSE,FALSE,FALSE},//鑑定
	{45,30,25,  5, 1,  4,100,{ 8,39, 0, 0, 0},FALSE, TRUE,FALSE},//*破壊*
	{80,45,180, 0, 0,  0,  0,{24,58, 7, 8,16},FALSE,FALSE,FALSE},//静寂
	{70,40,40,100, 0,  0,100,{ 5,60, 7, 8,16},FALSE, TRUE,FALSE},//周辺抹殺
//51
	{70,30,60,  0, 0,  0,  0,{ 5,59, 7, 8,19},FALSE, TRUE,FALSE},//混沌招来
	{40,20,20, 50, 1, 50,200,{ 5,60, 7, 8,19},FALSE,FALSE,FALSE},//視界内アウェイ
	{75,25,35,  0, 0,  0,  0,{ 5,61,62, 0, 0},FALSE, TRUE,FALSE},//死の光線
	{80,30,40, 50, 1, 50,200,{63,64, 7, 8,14},FALSE, TRUE,FALSE},//海が割れる日
	{30,20,10, 50, 1, 50,200,{ 5,65, 7, 8,19},FALSE,FALSE,FALSE},//金縛り
//56
	{70,15,20, 10, 1, 10,100,{66,60, 7, 8,19},FALSE,FALSE,FALSE},//召喚阻害
	{55,35,50,  0, 0,  0,  0,{67, 7, 8,19, 0},FALSE,FALSE,FALSE},//現実変容
	{80,45,60, 50, 1, 50,100,{68, 7, 8,16, 0},FALSE, TRUE,FALSE},//流星群
	{30, 1, 5, 50, 1, 50,200,{17,71, 7, 8,19},FALSE,FALSE, TRUE},//発光
	{80,20,25, 20, 0,  0,200,{17,72, 7, 8,19},FALSE,FALSE, TRUE},//白兵能力上昇
//61
	{40,15,15,  3, 1,  3,100,{73,74, 7, 8,34},FALSE, TRUE,FALSE},//地震
	{30, 1,10,  5, 0,  0,100,{22,10, 7, 8,19},FALSE,FALSE,FALSE},//トラップ解除
	{60,30,50, 30, 1, 30,  0,{75, 7, 8,19, 0},FALSE,FALSE,FALSE},//幸運

//	{ 0, 0, 0,  0, 0,  0,  0,{ 0, 0,0, 0, 0},FALSE,FALSE,FALSE},//


};



//「名もなき技」の効果を画面に表示する。元画面の保存や再描画処理は行わない。
void	display_nameless_art_effect(nameless_arts_activate_param_type *naap_ptr, int mode)
{
	int i;

	for (i = 15; i<23; i++) Term_erase(17, i, 255);

	prt(format(_("特技タイプ:%s",
                "Technique type: %s"), nameless_skill_type_desc[naap_ptr->art_idx]), 16, 30);
	if (naap_ptr->gf_type) prt(format(_("属性:%s", "Element: %s"), gf_desc_name[naap_ptr->gf_type]), 17, 30);
	prt(format(_("消費MP:%d", "MP cost: %d"), naap_ptr->cost), 18, 30);
	if (naap_ptr->rad>0) prt(format(_("半径:%d", "rad: %d"), ABS(naap_ptr->rad)), 19, 30);

	if (naap_ptr->dice && naap_ptr->sides && naap_ptr->base)
		prt(format(_("威力/効果:%dd%d+%d", "dam/pow: %dd%d+%d"), naap_ptr->dice, naap_ptr->sides, naap_ptr->base), 20, 30);
	else if (naap_ptr->dice && naap_ptr->sides)
		prt(format(_("威力/効果:%dd%d", "dam/pow: %dd%d"), naap_ptr->dice, naap_ptr->sides), 20, 30);
	else if (naap_ptr->base)
		prt(format(_("威力/効果:%d", "dam/pow: %d"), naap_ptr->base), 20, 30);


}


//「名もなき技」のパラメータをp_ptr->magic1[]から取得する
bool	read_nameless_arts_param_from_player(int idx, nameless_arts_activate_param_type *naap_ptr)
{

	//p_ptr->magic_num1[]を名もなき技に使うクラスのみが使う
	if (!USE_NAMELESS_ARTS)
	{
		msg_print(_("ERROR:read_nameless_arts_param_from_player()が未登録のキャラクターで呼ばれた",
                    "ERROR: read_nameless_arts_param_from_player() called for wrong character"));
		return FALSE;
	}
	if (idx < 0 || idx >= JUNKO_ARTS_COUNT_MAX)
	{
		msg_format(_("ERROR:read_nameless_arts_param_from_player()に渡されたidxがおかしい",
                    "ERROR: weird idx passed to read_nameless_arts_param_from_player()"),idx);
		return FALSE;
	}

	naap_ptr->art_idx = JUNKO_ARTS_PARAM(idx, JKAP_JKF1);
	naap_ptr->gf_type = JUNKO_ARTS_PARAM(idx, JKAP_GF);
	naap_ptr->cost = JUNKO_ARTS_PARAM(idx, JKAP_COST);
	naap_ptr->rad = JUNKO_ARTS_PARAM(idx, JKAP_RAD);
	naap_ptr->base = JUNKO_ARTS_PARAM(idx, JKAP_BASE);
	naap_ptr->dice = JUNKO_ARTS_PARAM(idx, JKAP_DICE);
	naap_ptr->sides = JUNKO_ARTS_PARAM(idx, JKAP_SIDES);
	naap_ptr->counter_idx = JUNKO_ARTS_PARAM(idx, JKAP_COUNT_NUM);
	naap_ptr->quark_idx = JUNKO_ARTS_PARAM(idx, JKAP_QUARK_IDX);
	naap_ptr->xtra1 = JUNKO_ARTS_PARAM(idx, JKAP_XTRA_VAL1);
	naap_ptr->xtra2 = JUNKO_ARTS_PARAM(idx, JKAP_XTRA_VAL2);

	return TRUE;

}




//純狐がスレイなどをエッセンス付与できる武器を判定するhook
bool object_allow_add_essence_junko1(object_type *o_ptr)
{
	if (!object_is_melee_weapon(o_ptr)) return FALSE;
	if (object_refuse_enchant_weapon(o_ptr)) return FALSE;
	if (object_is_artifact(o_ptr)) return FALSE;
	if (object_is_smith(o_ptr)) return FALSE;

	return TRUE;
}

//純狐が耐性エッセンス付与できる武器防具を判定するhook
bool object_allow_add_essence_junko2(object_type *o_ptr)
{
	if (object_is_artifact(o_ptr)) return FALSE;
	if (object_is_smith(o_ptr)) return FALSE;
	if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH) return FALSE;
	if (item_tester_hook_jeweler(o_ptr) || object_is_armour(o_ptr)) return TRUE;

	return FALSE;

}







/*:::
*「名も無き技」のパラメータを渡し、実際に効果発動する部分。
*元々activate_nameless_art()に含まれていたが、移植性確保のために別関数にした。最初からやっとけという話である。
*コストや失敗率(あれば)の処理は外で行う。
*ターゲット選択でキャンセルするなど行動順消費しないときFALSEを返す。
*/
bool	activate_nameless_art_aux(nameless_arts_activate_param_type *naap_ptr, int mode)
{
	int dir, i;
	int art_idx, typ, rad, base, dice, sides, counter_idx, quark_idx, xtra1, xtra2;

	int dice_roll_result; //ダイスを振ってベースを足した値。ダメージや効果時間など。効果によっては別の関数にbaseやdiceやsidesを渡すのでこの値を使わない

	art_idx = naap_ptr->art_idx;
	typ = naap_ptr->gf_type;
	rad = naap_ptr->rad;
	base = naap_ptr->base;
	dice = naap_ptr->dice;
	sides = naap_ptr->sides;
	counter_idx = naap_ptr->counter_idx;
	xtra1 = naap_ptr->xtra1;
	xtra2 = naap_ptr->xtra2;

	dice_roll_result = ((dice>0 && sides>0) ? damroll(dice, sides) : 0) + base;

	switch (art_idx)
	{
		//ボルト
	case JKF1_ATTACK_BOLT:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_bolt(typ, dir, dice_roll_result);
		break;
	}
	//ボール
	case JKF1_ATTACK_BALL:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball_hide(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball_hide(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//ビーム
	case JKF1_ATTACK_BEAM:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_beam(typ, dir, dice_roll_result);
		break;
	}
	//ロケット
	case JKF1_ATTACK_ROCKET:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//ブレス
	case JKF1_ATTACK_BREATH:
	{
		int dam;
		if (rad >= 0 || rad < -2)
		{
			msg_format(_("ERROR:純狐ブレスのrad値がおかしい(%d)",
                        "ERROR: Weird rad value for Junko breath (%d)"), rad);
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;
		dam = p_ptr->chp * xtra1 / 100; //ブレスの威力はxtra1値で変化
		if (dam < 1) dam = 1;
		fire_ball(typ, dir, dam, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball(GF_FIRE, dir, dam, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball(GF_WATER, dir, dam, rad);
		break;
	}
	//大型レーザー
	case JKF1_ATTACK_SPARK:
	{
		if (rad>2 || rad < 1)
		{
			msg_format(_("ERROR:純狐スパークのrad値がおかしい(%d)",
                        "ERROR: Weird rad value for Junko spark (%d)"), rad);
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;
		fire_spark(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_spark(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_spark(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//周囲ボール
	case JKF1_ATTACK_AROUND:
	{
		project(0, rad, py, px, dice_roll_result, typ, (PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		if (typ == GF_LAVA_FLOW)
			project(0, rad, py, px, dice_roll_result, GF_FIRE, (PROJECT_HIDE | PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		if (typ == GF_WATER_FLOW)
			project(0, rad, py, px, dice_roll_result, GF_WATER, (PROJECT_HIDE | PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		break;
	}
	//視界内攻撃
	case JKF1_ATTACK_SIGHT:
	{
		project_hack2(typ, dice, sides, base);
		if (typ == GF_LAVA_FLOW)
			project_hack2(GF_FIRE, dice, sides, base);
		if (typ == GF_WATER_FLOW)
			project_hack2(GF_WATER, dice, sides, base);
		break;
	}
	//フロア攻撃
	case JKF1_ATTACK_FLOOR:
	{
		floor_attack(typ, dice, sides, base, 0);

		break;
	}
	//空腹充足
	case JKF1_FOOD:
	{
		set_food(PY_FOOD_MAX - 1);
		break;
	}
	//回復
	case JKF1_HEAL1:
	case JKF1_HEAL2:
	{
		hp_player(dice_roll_result);
		set_cut(0);
		set_poisoned(0);

		break;
	}
	//感知など
	case JKF1_DETECT_MON:
	{
		detect_monsters_invis(base);
		detect_monsters_normal(base);
		break;
	}
	case JKF1_DETECT_TRAPS:
	{
		detect_traps(base, TRUE);
		break;
	}
	case JKF1_DETECT_OBJ:
	{
		detect_objects_normal(base);
		break;
	}
	case JKF1_ENLIGHT:
	{
		wiz_lite(FALSE);
		break;
	}
	//経験値、パラメータ復活
	case JKF1_RESTORE:
	{
		bool flag_notice = FALSE;
		if (restore_level()) flag_notice = TRUE;
		for (i = 0; i<6; i++) if (do_res_stat(i)) flag_notice = TRUE;

		if (!flag_notice) msg_print(_("あなたの能力や生命力は損なわれてはいない。",
                                    "Your stats and life force aren't drained."));

		break;
	}
	//テレポ
	case JKF1_TELEPO:
	{
		teleport_player(dice_roll_result, 0L);
		break;
	}
	//次元の扉
	case JKF1_D_DOOR:
	{
		dimension_door_distance = MAX(20, dice_roll_result);
		if (!dimension_door(D_DOOR_CHOSEN_DIST)) return FALSE;
		break;
	}
	//周囲攻撃 ACT_WHIRLWINDからコピー
	//v1.1.33 別関数に独立
	case JKF1_WHIRL_WIND:
	{
		whirlwind_attack();
		break;
	}
	//入身
	case JKF1_RUSH_ATTACK:
	{
		int len = MAX(3, dice_roll_result);
		if (!rush_attack(NULL, len, 0)) return FALSE;
		break;
	}
	//連続攻撃
	case JKF1_MULTI_HIT:
	{
		int mult = MAX(2, xtra1);
		monster_type    *m_ptr;
		int x, y;

		if (!get_rep_dir(&dir, FALSE)) return FALSE;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (cave[y][x].m_idx)
		{
			m_ptr = &m_list[cave[y][x].m_idx];
			for (i = 0; i<mult; i++)
			{
				py_attack(y, x, 0L);
				if (!m_ptr->r_idx) break;
				if (p_ptr->is_dead) break;

			}
		}
		else
		{
			msg_print(_("そちらには何もいない。", "There's nobody here."));
			return FALSE;
		}


		break;
	}
	//状態異常治癒
	case JKF1_CURE_ALL:
	{
		if (!p_ptr->stun && !p_ptr->cut && !p_ptr->poisoned && !p_ptr->image && !p_ptr->alcohol && !p_ptr->asthma && !p_ptr->afraid)
		{
			msg_print(_("特に体調は悪くない。",
                        "You're already feeling well."));
			break;
		}
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);
		set_alcohol(0);
		set_asthma(0);
		set_afraid(0);

		break;
	}
	//因果修復
	case JKF1_RESTORE_NEXUS:
	{
		msg_print(_("あなたは自らを修正し始めた..",
                    "You start restoring yourself..."));
		hp_player(5000);
		set_poisoned(0);
		set_stun(0);
		set_cut(0);
		set_image(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_alcohol(0);
		restore_level();
		dispel_player();
		set_asthma(0);

		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
		{
			if (muta_erasable_count()) msg_print(_("全ての突然変異が治った。",
                                                    "All of your mutations have been cured."));
			p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
			p_ptr->muta1 = p_ptr->muta1_perma;
			p_ptr->muta2 = p_ptr->muta2_perma;
			p_ptr->muta3 = p_ptr->muta3_perma;
			p_ptr->muta4 = p_ptr->muta4_perma;
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			handle_stuff();
			mutant_regenerate_mod = calc_mutant_regenerate_mod();
		}
		break;
	}
	//解呪
	case JKF1_REMOVE_CURSE:
	{
		if (remove_all_curse())
			msg_print(_("装備品の呪縛が消えた。",
                        "The curses on your equipment have been removed."));
		else
			msg_print(_("身につけたものは呪われていない。",
                        "You're not wearing or wielding anything cursed."));

		break;
	}
	//特殊一時効果全般
	case JKF1_VANISH_BEAM:
	case JKF1_EXPLOSION_DEF:
	case JKF1_BREATH_DEF:
	case JKF1_DESTRUCT_DEF:
	case JKF1_ABSORB_MAGIC:
	case JKF1_KILL_WALL:
	case JKF1_MASTER_MAGITEM:
	case JKF1_SELF_LIGHT:
	case JKF1_MASTER_MELEE:
	{

		if (mode == NAMELESS_ART_MODE_SPELLCARD)
			set_tim_spellcard_effect(dice_roll_result, FALSE, art_idx);
		else
			set_tim_general(dice_roll_result, FALSE, counter_idx, art_idx);

		break;
	}

	case JKF1_MUSCLE_UP:
	{

		msg_print(_("身体能力を強化した。", "You enhance your physical stats."));
		set_tim_addstat(A_STR, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_DEX, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_CON, xtra1, dice_roll_result, FALSE);

		break;
	}
	case JKF1_MENTAL_UP:
	{

		msg_print(_("精神を強化した。", "You enhance your mind."));
		set_tim_addstat(A_INT, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_WIS, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_CHR, xtra1, dice_roll_result, FALSE);

		break;
	}
	case JKF1_INVULNER:
	{
		set_invuln(dice_roll_result, FALSE);

		break;
	}
	case JKF1_SPEED:
	{
		set_fast(dice_roll_result, FALSE);

		break;
	}
	case JKF1_RES_MAGIC:
	{
		set_resist_magic(dice_roll_result, FALSE);

		break;
	}
	case JKF1_SHIELD:
	{
		set_shield(dice_roll_result, FALSE);

		break;
	}
	case JKF1_MAGICDEF:
	{
		set_magicdef(dice_roll_result, FALSE);

		break;
	}
	case JKF1_S_STEALTH:
	{
		set_tim_superstealth(dice_roll_result, FALSE, SUPERSTEALTH_TYPE_NORMAL);

		break;
	}
	case JKF1_PASS_WALL:
	{
		set_kabenuke(dice_roll_result, FALSE);

		break;
	}
	case JKF1_DUSTROBE:
	{

		msg_print(_("あなたは攻撃的なオーラをまとった！",
                    "You are shrouded in an offensive aura!"));
		set_dustrobe(dice_roll_result, FALSE);

		break;
	}
	case JKF1_RES_ELEM: //付与される一時耐性はビットフラグで決定済み
	{
		if ((xtra1) % 2) set_oppose_acid(dice_roll_result, FALSE);
		if ((xtra1 >> 1) % 2) set_oppose_elec(dice_roll_result, FALSE);
		if ((xtra1 >> 2) % 2) set_oppose_fire(dice_roll_result, FALSE);
		if ((xtra1 >> 3) % 2) set_oppose_cold(dice_roll_result, FALSE);
		if ((xtra1 >> 4) % 2) set_oppose_pois(dice_roll_result, FALSE);
		break;
	}
	case JKF1_ULTI_RES:
	{
		set_oppose_acid(dice_roll_result, FALSE);
		set_oppose_elec(dice_roll_result, FALSE);
		set_oppose_fire(dice_roll_result, FALSE);
		set_oppose_cold(dice_roll_result, FALSE);
		set_oppose_pois(dice_roll_result, FALSE);
		set_ultimate_res(dice_roll_result, FALSE);

		break;
	}

	//エッセンス付与 xtra1に入れたTR_*値を+1してxtra1に入れる
	case JKF1_ADD_ESSENCE1:
	case JKF1_ADD_ESSENCE2:
	{
		cptr q, s;
		int item;
		object_type *o_ptr;
		char o_name[MAX_NLEN];
		cptr essence_name = "";

		if (art_idx == JKF1_ADD_ESSENCE1)
			item_tester_hook = object_allow_add_essence_junko1;
		else
			item_tester_hook = object_allow_add_essence_junko2;

		item_tester_no_ryoute = TRUE;

		q = _("どのアイテムに力を付与しますか？", "Empower which item?");
		s = _("改良できるアイテムがありません。", "You don't have items to enhance.");

		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return FALSE;
		if (item >= 0) 	o_ptr = &inventory[item];
		else 		o_ptr = &o_list[0 - item];

		object_desc(o_name, o_ptr, OD_NAME_ONLY);

		//能力名を鍛冶師用エッセンス配列から探す
		for (i = 0; essence_info[i].add >= 0; i++)
		{
			if (essence_info[i].add != xtra1) continue;
			essence_name = essence_info[i].add_name;
			break;
		}

		if (!get_check(format(_("%sに「%s」の能力を付与します。よろしいですか？",
                                "You will empower %s with '%s' ability. Are you sure?"), o_name, essence_name))) return FALSE;

		o_ptr->xtra3 = xtra1 + 1; //エッセンス付与 TRフラグ+1の値

		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);
		p_ptr->update |= (PU_BONUS);

		break;
	}
	case JKF1_IDENTIFY:
	{
		if (xtra1)
		{
			if (!identify_fully(FALSE)) return FALSE;
		}
		else
		{
			if (!ident_spell(FALSE)) return FALSE;
		}

		break;
	}
	case JKF1_DESTROY:
	{
		destroy_area(py, px, dice_roll_result, FALSE, FALSE, FALSE);

		break;
	}
	case JKF1_SILENT_FLOOR:
	{
		msg_print(_("周囲が唐突に静まり返った..", "Everything around you suddenly becomes silent..."));
		p_ptr->silent_floor = 1;

		break;
	}
	case JKF1_MASS_GENO:
	{
		mass_genocide(dice_roll_result, TRUE);
		break;
	}
	case JKF1_CALL_CHAOS:
	{
		call_chaos();
		break;
	}
	case JKF1_TELE_AWAY_ALL:
	{
		banish_monsters(dice_roll_result);
		break;
	}
	case JKF1_DEATH_RAY:
	{
		deathray_monsters();
		break;
	}
	case JKF1_MOSES:
	{
		project_hack2(GF_MOSES, dice, sides, base);
		break;
	}
	case JKF1_STASIS:
	{
		stasis_monsters(dice_roll_result);
		break;
	}
	case JKF1_DISTURB_SUMMON:
	{
		set_deportation(dice_roll_result, FALSE);
		break;
	}
	case JKF1_ALTER_REALITY:
	{
		alter_reality();
		break;
	}
	case JKF1_CALL_METEOR:
	{

		cast_meteor(dice_roll_result, rad, GF_METEOR);
		break;
	}
	case JKF1_EARTHQUAKES:
	{
		earthquake(py, px, dice_roll_result);
	}
	break;
	case JKF1_DISARM:
	{
		project(0, dice_roll_result, py, px, 0, GF_KILL_DOOR, PROJECT_HIDE | PROJECT_GRID, -1);
	}
	break;
	case JKF1_LUCKY:
	{
		set_tim_lucky(dice_roll_result, FALSE);
		break;
	}

	default:
		msg_format(_("ERROR:activate_nameless_art_aux()へ不正なフラグ値が渡された(%d)",
                    "ERROR: Incorrect flag value (%d) passed to activate_nameless_art_aux()"), art_idx);
		return FALSE;
	}

	return TRUE;
}


//「名もなき技」使用ルーチン
//magic_num1[]に技パラメータが保存されている職がこのルーチンを使う。今の所純狐と夢の世界の菫子が使用する。
//行動順消費するときTRUEを返す
//v1.1.56 実行部を分離し技のパラメータを構造体で管理するよう変更
bool	activate_nameless_art(bool only_info)
{

	int num, i;
	int list_num = JUNKO_ARTS_COUNT;//最大9
	int dir;
	bool flag_choose = FALSE;
	nameless_arts_activate_param_type naap;

	int mode;



	if (!USE_NAMELESS_ARTS)
	{
		msg_print(_("ERROR:activate_nameless_art()が未登録のキャラクターで呼ばれた",
                    "ERROR: activate_nameless_art() called for wrong character"));
		return FALSE;
	}

	if (p_ptr->pclass == CLASS_JUNKO)
		mode = NAMELESS_ART_MODE_JUNKO;
	else
		mode = NAMELESS_ART_MODE_SUMIREKO;

	if (!JUNKO_ARTS_PARAM(0, JKAP_JKF1))
	{
		msg_print(_("まだ技がない。", "You don't have any techniques yet."));
		return FALSE;
	}


	//ALLOW_REPEATリピート処理
	if (repeat_pull(&num)) flag_choose = TRUE;

	//技選択ループ
	screen_save();
	while (!flag_choose)
	{
		char c;
		num = -1;

		//画面クリア
		for (i = 1; i<16; i++) Term_erase(17, i, 255);

		if (only_info)
			c_prt(TERM_WHITE, _("どの情報を確認しますか？", "Browse which technique?"), 3, 20);
		else
			c_prt(TERM_WHITE, _("どれを使いますか？", "Use which technique?"), 3, 20);

		c_prt(TERM_WHITE, _("                                            使用MP 効果",
                            "                                            MP cost  Effect"), 4, 20);
		//リスト表示
		for (i = 0; i<list_num; i++)
		{
			byte color;
			char tmp_desc[64];
			char tmp_desc2[64];

			read_nameless_arts_param_from_player(i, &naap);


			if (!naap.art_idx) break;//ここの技が作られていない

			if (p_ptr->csp < naap.cost) color = TERM_L_DARK;//MP不足
			else  color = TERM_WHITE;

			if (naap.gf_type && naap.art_idx != JKF1_ADD_ESSENCE1 && naap.art_idx != JKF1_ADD_ESSENCE2)
				sprintf(tmp_desc2, "[%s]", gf_desc_name[naap.gf_type]);
			else
				sprintf(tmp_desc2, "");

			if (naap.gf_type == GF_MAKE_TREE || naap.gf_type == GF_STONE_WALL || naap.gf_type == GF_MAKE_DOOR)
				sprintf(tmp_desc, "");
			else if (naap.art_idx == JKF1_ATTACK_BREATH) //ブレスはxtra1の値で威力が変わる
				sprintf(tmp_desc, "(%d)", p_ptr->chp * naap.xtra1 / 100);
			else if (naap.base < 1 && (naap.dice < 1 || naap.sides < 1))
				sprintf(tmp_desc, "");
			else if (naap.base < 1)
				sprintf(tmp_desc, "(%dd%d)", naap.dice, naap.sides);
			else if (naap.dice < 1 || naap.sides < 1)
				sprintf(tmp_desc, "(%d)", naap.base);
			else
				sprintf(tmp_desc, "(%d+%dd%d)", naap.base, naap.dice, naap.sides);

			c_prt(color, format(_("%c) %-45s %-4d %s%s%s",
                                  "%c) %-45s %-4d   %s%s%s"), ('a' + i), quark_str(naap.quark_idx),
				naap.cost, tmp_desc2, tmp_desc, ((naap.art_idx == JKF1_ATTACK_AROUND) ? "/2" : "")), 5 + i, 17);

		}

		c = inkey();

		if (c == ESCAPE)
		{
			break;
		}

		if (c >= 'a' && c <= ('a' + list_num - 1))
		{
			//技がないときを除き選択肢反映してループから出る
			num = c - 'a';
			if (!JUNKO_ARTS_PARAM(num, JKAP_JKF1)) continue;
			if (!only_info) flag_choose = TRUE;
			repeat_push(num);
		}

		//only_infoのとき画面に情報だけ表示してループ続行 ESCで終了
		if (only_info && num >= 0)
		{
			read_nameless_arts_param_from_player(num, &naap);
			display_nameless_art_effect(&naap, mode);

			continue;
		}

	}
	screen_load();

	if (num < 0) return FALSE; //ESCは終了

	//選択した技のパラメータをp_ptr->magic_num1[]から取得
	read_nameless_arts_param_from_player(num, &naap);


	if (cheat_xtra)
	{
		msg_format("name:%s", quark_str(naap.quark_idx));
		msg_format("idx:%d GF:%d", naap.art_idx, naap.gf_type);
		msg_format("base:%d,dice:%d,sides:%d", naap.base, naap.dice, naap.sides);
		msg_format("counter_idx:%d", naap.counter_idx);
		msg_format("xtra:%d/%d", naap.xtra1, naap.xtra2);
	}


	if (p_ptr->csp < naap.cost)
	{
		msg_print(_("MPが足りない。", "You don't have enough MP."));
		return FALSE;
	}

	//特技実行
	if (!activate_nameless_art_aux(&naap, mode)) return FALSE;

	//MP消費
	p_ptr->csp -= naap.cost;

	return TRUE;

}


//特定のJKF1_*フラグの一時効果がtim_gen[]のどれかで有効になっているかどうかをチェック
bool check_activated_nameless_arts(int art_idx)
{
	int i;

	//スペカの判定
	if (p_ptr->tim_spellcard_count && art_idx == p_ptr->spellcard_effect_idx)
		return TRUE;

	if(!USE_NAMELESS_ARTS) return FALSE;

	for(i=0;i<JUNKO_ARTS_COUNT;i++)
	{
		int tmp_idx = JUNKO_ARTS_PARAM(i,JKAP_JKF1);
		int count_num = JUNKO_ARTS_PARAM(i,JKAP_COUNT_NUM);
		if(count_num < 0) continue;//カウントを使わない特技
		if(count_num >= TIM_GENERAL_MAX){msg_format(_("nameless arts count_num設定エラー(%d)",
                                                        "nameless arts count_num parameter error (%d)"),count_num); return FALSE;}//paranoia
		if( tmp_idx == art_idx && p_ptr->tim_general[count_num]) return TRUE;

	}

	return FALSE;
}


//「名も無き技」の時間効果が発動/終了されたときのメッセージを返す。
//特技art_idxを指定する。不明な場合はtim_general[]のインデックスから探す。
cptr msg_tim_nameless_arts(int art_idx, int counter_idx, bool activate)
{
	int i;

	//tim_general[]のタイムカウンタ終了時はart_idx値が保持されていないので、現在使える特技から該当するものを探す

	if ((USE_NAMELESS_ARTS) && !art_idx)
	{
		for (i = 0; i<JUNKO_ARTS_COUNT; i++)
		{
			if (!nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) continue;
			if (JUNKO_ARTS_PARAM(i, JKAP_COUNT_NUM) == counter_idx)
			{
				art_idx = JUNKO_ARTS_PARAM(i, JKAP_JKF1);
				if (cheat_xtra) msg_format("art_idx found:%d", art_idx);
				break;
			}
		}
	}

	switch (art_idx)
	{
	case JKF1_VANISH_BEAM:
		if (activate)
			return _("あなたはレーザーやボルトに対して身構えた。",
                    "You prepare to deflect lasers or bolts.");
		else
			return _("レーザーやボルトへの防御が解けた。",
                    "You are no longer protected from lasers and bolts.");

	case JKF1_EXPLOSION_DEF:
		if (activate)
			return _("あなたは爆風に対して備えた。",
                    "You ward yourself against explosions.");
		else
			return _("爆風への防御が解けた。",
                    "You are no longer protected from explosions.");

	case JKF1_BREATH_DEF:
		if (activate)
			return _("あなたはブレス攻撃に対して備えた。",
                    "You ward yourself against breath attacks.");
		else
			return _("ブレス攻撃への防御が解けた。",
                    "You are no longer protected from breath attacks.");

	case JKF1_DESTRUCT_DEF:
		if (activate)
			return _("あなたは*破壊*に備えた。",
                    "You ward yourself against *Destruction*.");
		else
			return _("*破壊*の力への防御が解けた。",
                    "You are no longer protected from *Destruction*.");


	case JKF1_ABSORB_MAGIC:
		if (activate)
			return _("あなたは敵が魔法を使うのを待ち構えている...",
                    "You start waiting for your enemies to cast spells...");
		else
			return _("魔法を吸収できなくなった。",
                    "You are no longer able to absorb magic.");

	case JKF1_KILL_WALL:
		if (activate)
			return _("あなたの歩みは力強さを増した。",
                    "You start walking with tremendous power.");
		else
			return _("触るだけで壁を破壊できなくなった。",
                    "You no longer can destroy walls just by touching them.");
	case JKF1_MASTER_MAGITEM:
		if (activate)
			return _("今なら魔道具をもっと上手く扱える。",
                    "You feel more proficient at using magic devices.");
		else
			return _("魔道具を上手く扱えなくなった。",
                    "You are no longer proficient at using magic devices.");

	case JKF1_SELF_LIGHT:
		if (activate)
			return _("あなたは光り始めた。",
                    "You start emitting light.");
		else
			return _("あなたは光らなくなった。",
                    "You no longer emit light.");
	case JKF1_MASTER_MELEE:
		if (activate)
			return _("あなたは武術の達人になった！",
                    "You become a master of combat!");
		else
			return _("動きの切れが悪くなった。",
                    "You feel your movements getting duller.");
	default:
		return _("(ERROR:この一時効果のメッセージが登録されていない)",
                "(ERROR: No message found for this temporary effect)");


	}

}


//v1.1.56 真勝利後限定フレーバーアイテム「スペルカード」関連
//霧雨魔法店に蓄積された魔力値からカード作成のときのボーナス値を計算する
int calc_make_spellcard_bonus(void)
{
	int mag_type;
	int marisa_total_mag_power = 0;
	int flag_bonus;

	//スペル作成のためのフラグボーナス値を魔法店に備蓄された魔力値から適当に重みづけして計算する。普通に箱全部取るだけなら1万くらい貯まってるだろうか。
	for (mag_type = 0; mag_type<8; mag_type++)
	{
		int power_mult;
		switch (mag_type)
		{
		case MARISA_POWER_SUPER:
			power_mult = 7; break;
		case MARISA_POWER_ILLU:
		case MARISA_POWER_STAR:
			power_mult = 5; break;
		case MARISA_POWER_LITE:
		case MARISA_POWER_DARK:
			power_mult = 3; break;
		default:
			power_mult = 1; break;

		}
		marisa_total_mag_power += marisa_magic_power[mag_type] * power_mult;
	}

	if (marisa_total_mag_power < 1000)
		flag_bonus = 10;
	else if (marisa_total_mag_power < 25000)
		flag_bonus = marisa_total_mag_power / 100;
	else
		flag_bonus = 250 + (marisa_total_mag_power - 25000) / 200;
	if (flag_bonus > 500) flag_bonus = 500;

	if (p_ptr->wizard) flag_bonus = 50 + randint1(450);

	return flag_bonus;

}


//名もなき技生成サブ関数1/3
//特技art_idx(JKF1_***に対応する数でjunko_arts_table[]のインデックス)を決定する
//成功したらart_idxを、失敗したら0を返す
int	make_nameless_arts_sub1_choose_idx(int mode)
{
	int art_idx;
	int loop_max = 100;
	int plev = p_ptr->lev;

	//100回選定を試す
	while (loop_max--)
	{
		int randomizer = 0;
		int i;
		bool flag_ok = TRUE;

		//まずレアリティに準じた確率でart_idxを選定する
		if (mode == NAMELESS_ART_MODE_JUNKO)
		{	//純狐はmagic_num2[]に格納されたフラグ値により選定確率が変化する(正直あまり機能していない気もする)

			//あまり生成しすぎてフラグ値がなくなる事態を防ぐため、レベルが足りてるフラグ値を少し増やす。レアリティの高いフラグは増えづらい
			for (i = 1; i <= JKF1_MAX; i++)
			{
				if (p_ptr->magic_num2[i] > 100) continue;
				if (nameless_arts_generate_table[i].min_lev > plev) continue;
				if (nameless_arts_generate_table[i].rarity > 30 && !one_in_(nameless_arts_generate_table[i].rarity/3)) continue;
				p_ptr->magic_num2[i]++;
			}

			//フラグ値とレアリティに準じた確率で特技JKF値を選定する

			for (i = 1; i <= JKF1_MAX; i++)
			{
				randomizer += p_ptr->magic_num2[i] * 100 / (nameless_arts_generate_table[i].rarity);
			}
			randomizer = randint1(randomizer);
			for (i = 1; i <= JKF1_MAX; i++)
			{
				randomizer -= p_ptr->magic_num2[i] * 100 / (nameless_arts_generate_table[i].rarity);
				if (randomizer <= 0) break;
			}
			art_idx = i;

		}
		else
		{	//純狐以外はレアリティのみで決める


			for (i = 1; i <= JKF1_MAX; i++)
			{
				randomizer += 1000 / nameless_arts_generate_table[i].rarity;
			}
			randomizer = randint1(randomizer);
			for (i = 1; i <= JKF1_MAX; i++)
			{
				randomizer -= 1000 / nameless_arts_generate_table[i].rarity;
				if (randomizer <= 0) break;
			}
			art_idx = i;

		}

		if (art_idx <= 0 || art_idx > JKF1_MAX)
		{
			msg_format(_("ERROR:make_nameless_arts_sub1()のidxがおかしい(%d)",
                        "ERROR: Weird idx (%d) for make_nameless_arts_sub1()"), art_idx);
			return 0;
		}

		//生成modeに合わせて特定の技の選定を抑止する
		switch (mode)
		{
			//純狐
		case NAMELESS_ART_MODE_JUNKO:
			//同じ技が被ったらやり直し(属性攻撃技除く)
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == art_idx && art_idx >JKF1_ATTACK_FLOOR) { flag_ok = FALSE; break; }
			//レベル制限。でもたまに15レベル上まで出る
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }

			//tim_genカウンタを使う技がすでに5つある場合選び直し
			if (nameless_arts_generate_table[art_idx].use_counter)
			{
				int	tim_counter_num = 0;
				for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
				if (tim_counter_num >= 5) { flag_ok = FALSE; break; }
			}

			break;
			//菫子専用性格時
		case NAMELESS_ART_MODE_SUMIREKO:

			//超能力にある技は出ないことにする
			if (art_idx == JKF1_DETECT_MON || art_idx == JKF1_DETECT_TRAPS || art_idx == JKF1_DETECT_OBJ || art_idx == JKF1_ENLIGHT || art_idx == JKF1_TELEPO
				|| art_idx == JKF1_SPEED || art_idx == JKF1_SHIELD || art_idx == JKF1_RES_ELEM || art_idx == JKF1_IDENTIFY)
			{
				flag_ok = FALSE; break;
			}

			//エッセンス付与もやめとく
			if (art_idx == JKF1_ADD_ESSENCE1 || art_idx == JKF1_ADD_ESSENCE2) { flag_ok = FALSE; break; }

			//tim_genカウンタを使う技がすでに5つある場合選び直し
			if (nameless_arts_generate_table[art_idx].use_counter)
			{
				int	tim_counter_num = 0;
				for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
				if (tim_counter_num >= 5) { flag_ok = FALSE; break; }
			}

			//レベル制限。でもたまに15レベル上まで出る
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }

			//使用可能レベルが＠のレベルより20以上低い技は出ないことにする
			if (nameless_arts_generate_table[art_idx].min_lev < (plev - 20)) { flag_ok = FALSE; break; }

			//同じ技が被ったらやり直し(属性攻撃技除く)
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == art_idx && art_idx >JKF1_ATTACK_FLOOR) { flag_ok = FALSE; break; }

			break;

			//真・勝利後限定　霧雨魔法店でスペルカード制作
		case NAMELESS_ART_MODE_SPELLCARD:


			//真・勝利後なので弱い技や店売りアイテムで代替できる技は出さない
			if (art_idx == JKF1_ATTACK_BOLT || art_idx == JKF1_FOOD || art_idx == JKF1_HEAL1 || art_idx == JKF1_DETECT_TRAPS || art_idx == JKF1_DETECT_OBJ) { flag_ok = FALSE; break; }
			if (art_idx == JKF1_REMOVE_CURSE || art_idx == JKF1_IDENTIFY || art_idx == JKF1_SELF_LIGHT) { flag_ok = FALSE; break; }
			//*破壊*防御はもう用がないはずなので出さない
			if (art_idx == JKF1_DESTRUCT_DEF) { flag_ok = FALSE; break; }
			//エッセンス付与はフラグ管理が面倒だし今更要らんと思うのでやめとく
			if (art_idx == JKF1_ADD_ESSENCE1 || art_idx == JKF1_ADD_ESSENCE2) { flag_ok = FALSE; break; }
			//レベル制限。でもたまに15レベル上まで出る
			//もうレベルMAXだと思うのでフロア攻撃(レベル57)が出づらい処理としてのみ機能するはず
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }
			break;


		default:
			msg_format(_("ERROR:make_nameless_arts_sub1()に正しいmode値(%d)が設定されていない",
                        "ERROR: Incorrect mode value (%d) in make_nameless_arts_sub1()"), mode);
			return 0;

		}

		if (!flag_ok) continue;

		break;

	}

	if (loop_max < 1)
	{
		msg_format(_("ERROR:make_nameless_arts_sub1()で100回繰り返しても技が決まらなかった",
                    "ERROR: make_nameless_arts_sub1() did not pick a technique after 100 attempts"));
		return 0;
	}

	//純狐は技の生成に使ったフラグ値を50減らす
	if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		p_ptr->magic_num2[art_idx] -= MIN(50, p_ptr->magic_num2[art_idx]);
	}

	return art_idx;

}


//名もなき技生成サブ関数2/3
//特技art_idxを渡し、攻撃技の属性値を決める。
//決めた属性値はnaap_ptr内に格納する
//成功したらelem_idx(JKF2_***に対応する値)を、失敗するか不要な場合0を返す
int	make_nameless_arts_sub2_choose_gf(int mode, nameless_arts_activate_param_type *naap_ptr)
{
	int gf_idx, elem_idx,art_idx;
	byte tmp_flags[MAGIC_NUM_SIZE];
	int loop_max = 100;
	int plev = p_ptr->lev;
	int i;

	art_idx = naap_ptr->art_idx;
	//最初に0を入れておく
	naap_ptr->gf_type = 0;

	//属性値の不要な特技は何もしない
	if (!nameless_arts_generate_table[art_idx].need_flag2) return 0;

	//tmp_flags[]に、属性値の付与されやすさの初期設定を行う。
	//菫子専用性格の場合、属性値のレアリティ代わりにランダムな値を加算する。レベルが高いほど上位属性が当たりやすいことにする。
	//スペルカード制作も同じでいいだろう
	if (mode == NAMELESS_ART_MODE_SUMIREKO || mode == NAMELESS_ART_MODE_SPELLCARD)
	{
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			if (i <= JKF2_POIS) tmp_flags[i] = 200;
			else tmp_flags[i] = 50 + p_ptr->lev * 5;
		}
	}
	//純狐はmagic_num2[]に記録されたフラグ値により属性の付与されやすさが変化する
	else if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		//フラグ値にランダムな値を加算
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			tmp_flags[i] = p_ptr->magic_num2[i];

			if (i <= JKF2_POIS && tmp_flags[i] < 100)  tmp_flags[i] += randint1(10);
			else if (i <= JKF2_STEAM && tmp_flags[i] < 50)  tmp_flags[i] += randint1(5);
			else if (i <= JKF2_END && tmp_flags[i] < 30)  tmp_flags[i] += randint1(3);

		}

	}
	else
	{
		msg_format(_("ERROR:make_nameless_arts_sub2_choose_gf()に適切な動作モードが設定されていない",
                    "ERROR: Improper mode value for make_nameless_arts_sub2_choose_gf()"));
		return 0;
	}


	while (loop_max--)
	{
		int randomizer = 0;
		bool flag_ok = TRUE;

		//tmp_flags[]に設定した値をもとにして属性値をランダムに決める
		//(注意:この時点ではあくまで属性フラグ列のidx(JKF2_***)でありGF_XXXの値ではない)
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			randomizer += tmp_flags[i];
		}
		randomizer = randint1(randomizer);
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			randomizer -= tmp_flags[i];
			if (randomizer <= 0) break;
		}
		elem_idx = i;

		if (elem_idx < JKF2_START || elem_idx > JKF2_END)
		{
			msg_format(_("ERROR:make_nameless_arts_sub2_choose_gf()で決定された属性値がおかしい(%d)",
                        "ERROR: Picked weird element value (%d) for make_nameless_arts_sub2_choose_gf()"), elem_idx);
			return 0;
		}

		//ドア生成など地形変化形属性のボルトやフロア攻撃はちょっと変なので除外
		if ((art_idx == JKF1_ATTACK_BOLT || art_idx == JKF1_ATTACK_FLOOR) && (elem_idx == JKF2_MAKE_WALL || elem_idx == JKF2_MAKE_DOOR || elem_idx == JKF2_MAKE_TREE))
			flag_ok = FALSE;


		//属性フラグ列idxをGF_XXXの値に変換する。ときどき火炎がプラズマになったり変化をつける

		switch (elem_idx)
		{
		case JKF2_ACID:
			gf_idx = GF_ACID;
			break;
		case JKF2_ELEC:
			if (one_in_(6)) gf_idx = GF_PLASMA;
			else gf_idx = GF_ELEC;
			break;
		case JKF2_FIRE:
			if (one_in_(9) && art_idx != JKF1_ATTACK_FLOOR && art_idx != JKF1_ATTACK_BOLT) gf_idx = GF_LAVA_FLOW;
			else if (one_in_(6)) gf_idx = GF_PLASMA;
			else gf_idx = GF_FIRE;
			break;
		case JKF2_COLD:
			if (one_in_(9)) gf_idx = GF_ICE;
			else gf_idx = GF_COLD;
			break;
		case JKF2_POIS:
			if (one_in_(7)) gf_idx = GF_POLLUTE;
			else gf_idx = GF_POIS;
			break;
		case JKF2_LITE:
			gf_idx = GF_LITE;
			break;
		case JKF2_DARK:
			gf_idx = GF_DARK;
			break;
		case JKF2_SHARDS:
			if (one_in_(6)) gf_idx = GF_WINDCUTTER;
			gf_idx = GF_SHARDS;
			break;
		case JKF2_WATER:
			if (one_in_(6) && art_idx != JKF1_ATTACK_FLOOR && art_idx != JKF1_ATTACK_BOLT) gf_idx = GF_WATER_FLOW;
			else gf_idx = GF_WATER;
			break;
		case JKF2_SOUND:
			gf_idx = GF_SOUND;
			break;
		case JKF2_NUKE:
			gf_idx = GF_NUKE;
			break;
		case JKF2_MANA:
			gf_idx = GF_MANA;
			break;
		case JKF2_METEOR:
			gf_idx = GF_METEOR;
			break;
		case JKF2_CHAOS:
			gf_idx = GF_CHAOS;
			break;
		case JKF2_NETHER:
			gf_idx = GF_NETHER;
			break;
		case JKF2_DISENCHANT:
			gf_idx = GF_DISENCHANT;
			break;
		case JKF2_TIME:
			gf_idx = GF_TIME;
			break;
		case JKF2_DISTORTION:
			if (one_in_(3)) gf_idx = GF_NEXUS;
			else gf_idx = GF_DISTORTION;
			break;
		case JKF2_HOLY:
			gf_idx = GF_HOLY_FIRE;
			break;
		case JKF2_HELL:
			gf_idx = GF_HELL_FIRE;
			break;
		case JKF2_DISINTEGRATE:
			gf_idx = GF_DISINTEGRATE;
			break;
		case JKF2_WIND:
			gf_idx = GF_TORNADO;
			break;
		case JKF2_STEAM:
			gf_idx = GF_STEAM;
			break;
		case JKF2_MAKE_WALL:
			gf_idx = GF_STONE_WALL;
			break;
		case JKF2_MAKE_DOOR:
			gf_idx = GF_MAKE_DOOR;
			break;
		case JKF2_MAKE_TREE:
			gf_idx = GF_MAKE_TREE;
			break;
		default:
			msg_format(_("ERROR:make_nameless_arts_sub2_choose_gf()でフラグ値%dに対応するGF値が決まっていない",
                        "ERROR: No corresponding GF for flag value %d in make_nameless_arts_sub2_choose_gf()"), elem_idx);
			return 0;
		}

		//純狐と菫子は、すでに持っている技と同じ種類かつ同じ属性の技が出たら選び直し
		if (mode == NAMELESS_ART_MODE_JUNKO || mode == NAMELESS_ART_MODE_SUMIREKO)
		{
			for (i = 0; i < JUNKO_ARTS_COUNT_MAX; i++)
			{
				if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == naap_ptr->art_idx && JUNKO_ARTS_PARAM(i, JKAP_GF) == gf_idx)
					flag_ok = FALSE;
			}
		}

		if (!flag_ok) continue;

		break;

	}

	if (!loop_max)
	{
		msg_format(_("ERROR:make_nameless_arts_sub2()で100回繰り返しても属性値が決まらなかった",
                    "ERROR: make_nameless_arts_sub2() failed to pick an element after 100 attempts"));
		return 0;
	}

	//純狐は技の生成に使ったフラグ値を50減らす
	if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		p_ptr->magic_num2[elem_idx] -= MIN(50, p_ptr->magic_num2[elem_idx]);
	}


	naap_ptr->gf_type = gf_idx;

	return elem_idx;

}


//名もなき技生成サブ関数3/3
//特技のコストやダイスなどまだ決めてないパラメータを決定する
void	make_nameless_arts_sub3_apply_other_params(nameless_arts_activate_param_type *naap_ptr, int mode)
{

	int art_idx, gf_idx, cost, rad, base, dice, sides, counter_idx, xtra1, xtra2;
	int plev = p_ptr->lev;
	int flag_bonus;
	int mult;	//威力に対するランダム補正

	mult = randnor(100, 20);
	if (mult < 75) mult = 75;

	//ダイス値などに対するボーナスを決定
	switch (mode)
	{
	case NAMELESS_ART_MODE_JUNKO:
		//純狐は純化によりエッセンスが貯まっている場合技がパワーアップする
		flag_bonus = 100 + p_ptr->magic_num2[naap_ptr->art_idx];
		if (p_ptr->shero) flag_bonus += 100;
		break;
	case NAMELESS_ART_MODE_SPELLCARD:
		//スペカ製作時は霧雨魔法店に貯まった魔力を消費する
		flag_bonus = calc_make_spellcard_bonus();
		break;
	default:
		flag_bonus = 100 + plev * 2;
		break;

	}

	art_idx = naap_ptr->art_idx;
	gf_idx = naap_ptr->gf_type;
	cost = 0;
	rad = 0;
	base = 0;
	dice = 0;
	sides = 0;
	counter_idx = 0;
	xtra1 = 0;
	xtra2 = 0;


	//コスト決める
	cost = nameless_arts_generate_table[art_idx].cost;
	cost += cost * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
	cost = cost * mult / 100;
	if (cost < 1) cost = 1;
	if (cost > 400) cost = 400;

	//ダイス決める
	base = nameless_arts_generate_table[art_idx].base;
	dice = nameless_arts_generate_table[art_idx].dice;
	sides = nameless_arts_generate_table[art_idx].sides;
	if (base || dice || sides)
	{
		if (base)
		{
			base += base * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
			base = base * (mult + (flag_bonus - 100) / 2) / 100;
			if (base < 1) base = 1;
		}
		if (dice && sides && base)
		{

			//元素系はダメージ大きめにする
			if (gf_idx >= GF_ELEC && gf_idx <= JKF2_FIRE)
			{
				base += randint1(base);
				dice += dice / 2 + randint1((dice + 1) / 2);
			}

			if (one_in_(2))
				dice += dice * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
			else
				sides += sides * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;


			sides = sides * (mult + (flag_bonus - 100) / 2) / 100;
			//攻撃のみダイスボーナス判定
			while (nameless_arts_generate_table[art_idx].attack && randint1(100) < plev) dice++;


			if (dice <= 0 || sides <= 0)
			{
				dice = 0;
				sides = 0;
			}
			else if (sides == 1)
			{
				base += sides;
				dice = 0;
				sides = 0;
			}

			if (one_in_(32) && dice > 1 && sides > 1 && nameless_arts_generate_table[art_idx].attack)//低確率でダイス超不安定化　ダイスボーナス
			{
				sides = dice * (sides + 1) * (150 + randint1(plev * 3)) / 100 + base * (300 + randint1(plev * 4)) / 100;
				dice = 1;
				base = 0;
			}
			else if (one_in_(8) && dice > 1 && sides > 1)//低確率でダイス不安定化　ダイスボーナス
			{
				sides = dice * (sides + 1) * (100 + randint1(plev * 2)) / 100;
				dice = 1;
			}
			else if (one_in_(4)) //中確率で全部baseに
			{
				base += dice * (sides + 1) / 2;
				dice = 0;
				sides = 0;
			}
		}

	}


	//特殊な一次効果をもたらす特技に対し、tim_general[]のどれを使うかインデックス0-4を設定する
	//純狐と菫子は一時カウンターを5つまで使用する
	if (mode == NAMELESS_ART_MODE_SUMIREKO || mode == NAMELESS_ART_MODE_JUNKO)
	{
		//カウンタを使うときtim_general[]のどれを使うか登録
		if (nameless_arts_generate_table[art_idx].use_counter)
		{
			int i;
			int	tim_counter_num = 0;
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
			counter_idx = tim_counter_num;
		}
		else
			counter_idx = 0;

	}
	//スペルカード制作の場合、tim_generalではなく専用のカウンターを使うことにしたのでidx設定不要
	else if (mode == NAMELESS_ART_MODE_SPELLCARD)
	{
		counter_idx = 0;

	}

	//その他必要なパラメータ設定
	switch (art_idx)
	{
	case JKF1_ATTACK_BALL:
	case JKF1_ATTACK_ROCKET:
		rad = 1 + plev / 24 + (flag_bonus - 100) / 60;
		break;
	case JKF1_ATTACK_AROUND:
		rad = 1 + plev / 15 + (flag_bonus - 100) / 20;
		if (rad > 8) rad = 8;
		break;
	case JKF1_ATTACK_SPARK:
		rad = 1;
		if (plev * 2 + flag_bonus > 250) rad++;
		break;

	case  JKF1_ATTACK_BREATH: //ブレスは「現在HPの何%の威力のブレスダメージになるか」の値をxtra1に保存
	{
		int per;
		if (randint1(100) < (plev + flag_bonus - 100)) rad = -2;
		else rad = -1;

		xtra1 = (flag_bonus - 100 + plev) / 2;
		if (xtra1 > 100) xtra1 = 100;
		if (xtra1 < 30) xtra1 = 30;
	}
	break;
	case JKF1_RUSH_ATTACK:
		if (base < 2) base = 2;
		if (base > MAX_SIGHT) base = MAX_SIGHT;
		break;
	case JKF1_MULTI_HIT:
		xtra1 = 2 + (flag_bonus - 100 + plev) / 100;
		break;
	case JKF1_MUSCLE_UP:
	case JKF1_MENTAL_UP:
	{
		xtra1 = 1 + randint1(3) + plev / 8;

		if (flag_bonus > 100)
		{
			xtra1 += 100;
		}
	}
	break;
	case JKF1_RES_ELEM://エレメント耐性
	{
		int chance = plev + flag_bonus / 4;
		int bitflag = 0;

		if (flag_bonus > 120) bitflag = 31;//フラグ120以上で全耐性

										   //どのエレメント耐性を得るかランダムに決めビットフラグでxtra1に記録
		while (!bitflag)
		{
			if (randint0(100) < chance) bitflag += 1;
			if (randint0(100) < chance) bitflag += 2;
			if (randint0(100) < chance) bitflag += 4;
			if (randint0(100) < chance) bitflag += 8;
			if (randint0(100) < chance) bitflag += 16;
		}
		xtra1 = bitflag;
	}
	break;

	//武器エッセンス付与
	case JKF1_ADD_ESSENCE1:
		switch (naap_ptr->gf_type)
		{
		case GF_ACID: case GF_POLLUTE: xtra1 = TR_BRAND_ACID; break;
		case GF_ELEC: case GF_PLASMA: xtra1 = TR_BRAND_ELEC; break;
		case GF_FIRE: case GF_LAVA_FLOW: xtra1 = TR_BRAND_FIRE; break;
		case GF_COLD: case GF_ICE: xtra1 = TR_BRAND_COLD; break;
		case GF_POIS: xtra1 = TR_BRAND_POIS; break;
		case GF_LITE: xtra1 = TR_SLAY_EVIL; break;
		case GF_DARK: xtra1 = TR_SLAY_GOOD; break;
		case GF_SHARDS: case GF_WINDCUTTER: xtra1 = TR_VORPAL; break;
		case GF_WATER: case GF_WATER_FLOW: xtra1 = TR_SLAY_UNDEAD; break;
		case GF_SOUND:xtra1 = TR_SLAY_ANIMAL; break;
		case GF_NUKE:xtra1 = TR_SLAY_HUMAN; break;
		case GF_MANA:xtra1 = TR_FORCE_WEAPON; break;
		case GF_METEOR:xtra1 = TR_CHAOTIC; break;
		case GF_CHAOS: xtra1 = TR_CHAOTIC; break;
		case GF_NETHER: xtra1 = TR_VAMPIRIC; break;
		case GF_DISENCHANT:xtra1 = TR_VAMPIRIC; break;
		case GF_TIME:case GF_NEXUS: xtra1 = TR_SLAY_DEITY; break;
		case GF_DISTORTION:xtra1 = TR_SLAY_DEMON; break;
		case GF_HOLY_FIRE:xtra1 = TR_SLAY_EVIL; break;
		case GF_HELL_FIRE:xtra1 = TR_SLAY_GOOD; break;
		case GF_DISINTEGRATE:xtra1 = TR_VORPAL; break;
		case GF_TORNADO:xtra1 = TR_IMPACT; break;
		case GF_STEAM:xtra1 = TR_SLAY_KWAI; break;
		case GF_STONE_WALL:xtra1 = TR_SLAY_DRAGON; break;
		case GF_MAKE_DOOR:xtra1 = TR_FORCE_WEAPON; break;
		case GF_MAKE_TREE:xtra1 = TR_FORCE_WEAPON; break;

		default:xtra1 = TR_FORCE_WEAPON; break;

		}
		break;

	case JKF1_ADD_ESSENCE2:
		switch (naap_ptr->gf_type)
		{
		case GF_ACID:xtra1 = TR_RES_ACID; break;
		case GF_ELEC: case GF_PLASMA: xtra1 = TR_RES_ELEC; break;
		case GF_FIRE: case GF_LAVA_FLOW: xtra1 = TR_RES_FIRE; break;
		case GF_COLD: case GF_ICE: xtra1 = TR_RES_COLD; break;
		case GF_POIS: case GF_POLLUTE: xtra1 = TR_RES_POIS; break;
		case GF_LITE:xtra1 = TR_RES_LITE; break;
		case GF_DARK:xtra1 = TR_RES_DARK; break;
		case GF_SHARDS:case GF_WINDCUTTER: xtra1 = TR_RES_SHARDS; break;
		case GF_WATER:case GF_WATER_FLOW: xtra1 = TR_RES_WATER; break;
		case GF_SOUND:xtra1 = TR_RES_SOUND; break;
		case GF_NUKE:xtra1 = TR_RES_BLIND; break;
		case GF_MANA:xtra1 = TR_DEC_MANA; break;
		case GF_METEOR:xtra1 = TR_SPEEDSTER; break;
		case GF_CHAOS:xtra1 = TR_RES_CHAOS; break;
		case GF_NETHER:xtra1 = TR_RES_NETHER; break;
		case GF_DISENCHANT:xtra1 = TR_RES_DISEN; break;
		case GF_TIME: case GF_NEXUS: xtra1 = TR_RES_TIME; break;
		case GF_DISTORTION:xtra1 = TR_RES_TIME; break;
		case GF_HOLY_FIRE:xtra1 = TR_RES_HOLY; break;
		case GF_HELL_FIRE:xtra1 = TR_RES_INSANITY; break;
		case GF_DISINTEGRATE:xtra1 = TR_REFLECT; break;
		case GF_TORNADO:xtra1 = TR_REFLECT; break;
		case GF_STEAM:xtra1 = TR_RES_WATER; break;
		case GF_STONE_WALL:xtra1 = TR_TELEPORT; break;
		case GF_MAKE_DOOR:xtra1 = TR_NO_MAGIC; break;
		case GF_MAKE_TREE:xtra1 = TR_REGEN; break;

		default:xtra1 = TR_REGEN; break;
		}
		break;


	//鑑定　*鑑定*のときxtra1=1
	case JKF1_IDENTIFY:
	{
		if (plev + p_ptr->stat_ind[A_INT] + flag_bonus > 200) xtra1 = 1;
	}
	break;

	case JKF1_CALL_METEOR:
	{
		rad = 1 + (plev + flag_bonus) / 80;
	}
	break;


	default:
		break;
	}


	naap_ptr->cost = cost;
	naap_ptr->rad = rad;
	naap_ptr->base = base;
	naap_ptr->dice = dice;
	naap_ptr->sides = sides;
	naap_ptr->counter_idx = counter_idx;
	naap_ptr->xtra1 = xtra1;
	naap_ptr->xtra2 = xtra2;

}



/*:::純狐が名もなき技を作り出す*/
/*:::創り出した技のパラメータなどを格納するためにp_ptr->magic_num1[0-98]を使用する*/
//現在のレベルで習得できる全ての特技を一度にリセットして作り直す
//magic_num2[]に蓄積したフラグ値によって選定されやすさに影響が出る
//命名は技の種類と属性によって自動的に行われる
bool	junko_make_nameless_arts(void)
{
	int num = JUNKO_ARTS_COUNT;//最大9
	int i;
	int plev = p_ptr->lev;
	int elem_flag_idx;
	int flag_total_val = 0;

	if (p_ptr->wild_mode || p_ptr->inside_battle) return FALSE;

	//今有効なタイムカウント能力を解除
	for (i = 0; i<TIM_GENERAL_MAX; i++)set_tim_general(0, TRUE, i, 0);

	//すべての技をリセット
	for (i = 0; i<(11 * JUNKO_ARTS_COUNT); i++) p_ptr->magic_num1[i] = 0;


	for (num -= 1; num >= 0; num--)//最大8→0
	{
		nameless_arts_activate_param_type naap;
		int flag_bonus;
		int name_num = 500;
		char art_desc[255];
		cptr name_str[5];
		cptr name_sep[5];

		//発動IDX決定
		naap.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_JUNKO);
		if (!naap.art_idx) return FALSE;

		//属性決定
		elem_flag_idx = make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_JUNKO, &naap);

		//ダイス値などその他決定
		make_nameless_arts_sub3_apply_other_params(&naap, NAMELESS_ART_MODE_JUNKO);


		//命名
		while (name_num--)
		{
			for (i = 0; i<5; i++)
			{
				int n_idx = nameless_arts_generate_table[naap.art_idx].name_idx[i];
				//空
#ifdef JP
				if (!n_idx) name_str[i] = "";
#else
                if (!n_idx) {
                    name_str[i] = "";
                    name_sep[i] = "";
                }
#endif
				//属性リスト2(攻撃用)を参照
				//ここでJKF2値が要る
				else if (n_idx == -1)
				{
					name_str[i] = junko_arts_name_words_2_atk[elem_flag_idx - JKF2_START][randint0(JUNKO_NAME_LIST_NUM_SUB)];
#ifndef JP
                    if (strlen(name_str[i]) > 0) {
                        name_sep[i] = " ";
                    } else {
                        name_sep[i] = "";
                    }
#endif
				}
				else if (n_idx > 0 && n_idx < JUNKO_NAME_LIST_NUM1)
				{
					//「敵」「相手」などの名詞を「袋の鼠」など別の名詞群に変える
					if (n_idx == 1)
					{
						if (p_ptr->shero) n_idx = 0; //嫦娥
						else if (one_in_(9))     n_idx += 3;
						else if (one_in_(8)) n_idx += 2;
						else if (one_in_(7)) n_idx++;
					}
					//よく使う文字列もう少し追加
					if (n_idx == 8 && one_in_(3)) n_idx = 69;
					if (n_idx == 19 && one_in_(3)) n_idx = 70;

					if (p_ptr->shero && n_idx == 5) n_idx = 0; //嫦娥

					name_str[i] = junko_arts_name_words_1[n_idx][randint0(JUNKO_NAME_LIST_NUM_SUB)];

#ifndef JP
                    if (strlen(name_str[i]) > 0) {
                        name_sep[i] = " ";
                    } else {
                        name_sep[i] = "";
                    }
#endif
				}
				else
				{
					msg_format(_("ERROR:nameless_arts_generate_table[]のn_idx値がおかしい(%d)",
                                "ERROR: Weird n_idx value (%d) in nameless_arts_generate_table[]"), n_idx);
					return FALSE;
				}
			}
#ifdef JP
			sprintf(art_desc, "%s%s%s%s%s", name_str[0], name_str[1], name_str[2], name_str[3], name_str[4]);
#else
            /*sprintf(art_desc, "%s%s%s%s%s%s%s%s%s",
                    name_str[0], name_sep[0],
                    name_str[1], name_sep[1],
                    name_str[2], name_sep[2],
                    name_str[3], name_sep[3],
                    name_str[4]);*/
            sprintf(art_desc, "%s%s%s%s%s%s%s%s%s",
                    name_str[4], name_sep[4],
                    name_str[3], name_sep[3],
                    name_str[2], name_sep[2],
                    name_str[1], name_sep[1],
                    name_str[0]);
#endif

			//名前が長すぎると付け直し
			//v1.1.52 50→45
			if (strlen(art_desc) > 45) continue;
			break;

		}
		if (name_num<1)
		{
			msg_format(_("ERROR:技の名前が決まらない idx:%d gf_type:%d",
                        "ERROR: Failed to pick technique name idx: %d gf_type: %d"), naap.art_idx, naap.gf_type);
			return FALSE;

		}

		naap.quark_idx = quark_add(art_desc);

		JUNKO_ARTS_PARAM(num, JKAP_JKF1) = naap.art_idx;
		JUNKO_ARTS_PARAM(num, JKAP_GF) = naap.gf_type;
		JUNKO_ARTS_PARAM(num, JKAP_COST) = naap.cost;
		JUNKO_ARTS_PARAM(num, JKAP_RAD) = naap.rad;
		JUNKO_ARTS_PARAM(num, JKAP_BASE) = naap.base;
		JUNKO_ARTS_PARAM(num, JKAP_DICE) = naap.dice;
		JUNKO_ARTS_PARAM(num, JKAP_SIDES) = naap.sides;
		JUNKO_ARTS_PARAM(num, JKAP_COUNT_NUM) = naap.counter_idx;
		JUNKO_ARTS_PARAM(num, JKAP_QUARK_IDX) = naap.quark_idx;
		JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL1) = naap.xtra1;
		JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL2) = naap.xtra2;



	}

	//msg_format("flag_total_val:%d",flag_total_val);

	return TRUE;
}


/*隠岐奈が新たな技を作り出す。今の所夢の世界の菫子専用*/
//習得した技は純狐同様にmagic_num1[]にパラメータを保存する
//num(いくつめの技か)の入力を受ける。0-8
//純狐と違い、一つだけ技を生成し、num番目の技領域に格納し、技の効果を表示して名前の入力を受け付ける。一度習得した技は変化しない。
//v1.1.56 関数とかいろいろ整頓
bool	okina_make_nameless_arts(int num)
{
	int i;
	int plev = p_ptr->lev;
	int flag_bonus;
	char art_desc[255] = "";
	nameless_arts_activate_param_type naap;

	//magic_num1[]のサイズの都合で技数は9まで
	if (num < 0 || num >= JUNKO_ARTS_COUNT_MAX) { msg_format(_("ERROR:okina_make_nameless_arts()に不正なnum値(%d)が渡された",
                                                                "ERROR: Incorrect num value (%d) passed to okina_make_nameless_arts()"), num); return FALSE; }


	//発動IDX決定
	naap.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_SUMIREKO);
	if (!naap.art_idx) return FALSE;

	//属性決定
	(void)make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_SUMIREKO, &naap);

	//ダイス値などその他決定
	make_nameless_arts_sub3_apply_other_params(&naap, NAMELESS_ART_MODE_SUMIREKO);

	screen_save();

	while (1)
	{

		for (i = 7; i<22; i++) Term_erase(17, i, 255);

		prt(_("隠岐奈「お前に新たな力を貸してやろう！」",
            "Okina - 'I'll lend you a new power!'"), 12, 28);

		display_nameless_art_effect(&naap, NAMELESS_ART_MODE_SUMIREKO);

		if (!get_string(_("この技を何と名づけますか？:",
                        "Name this technique: "), art_desc, 45)) continue;

		prt(format(_("名称:%s", "Name: %s"), art_desc), 21, 30);

		if (!get_check_strict(_("よろしいですか？：", "Are you sure? "), CHECK_OKAY_CANCEL))continue;

		break;
	}


	screen_load();
	naap.quark_idx = quark_add(art_desc);


	//技パラメータをmagic_num1に格納

	//p_ptr->magic_num1[0-98]の添字%11が、
	//0:JKF1値
	//1:属性(GF)
	//2:消費MP
	//3:rad
	//4:base
	//5:dice
	//6:sides
	//7:tim_genカウンタの何番を使うか
	//8:名前用quarkインデックス
	//9:flg、もしくはほか何か設定値
	//10:予備
	//の値を格納する

	JUNKO_ARTS_PARAM(num, JKAP_JKF1) = naap.art_idx;
	JUNKO_ARTS_PARAM(num, JKAP_GF) = naap.gf_type;
	JUNKO_ARTS_PARAM(num, JKAP_COST) = naap.cost;
	JUNKO_ARTS_PARAM(num, JKAP_RAD) = naap.rad;
	JUNKO_ARTS_PARAM(num, JKAP_BASE) = naap.base;
	JUNKO_ARTS_PARAM(num, JKAP_DICE) = naap.dice;
	JUNKO_ARTS_PARAM(num, JKAP_SIDES) = naap.sides;
	JUNKO_ARTS_PARAM(num, JKAP_COUNT_NUM) = naap.counter_idx;
	JUNKO_ARTS_PARAM(num, JKAP_QUARK_IDX) = naap.quark_idx;
	JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL1) = naap.xtra1;
	JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL2) = naap.xtra2;

	return TRUE;
}





//v1.1.56 真勝利後限定フレーバーアイテム「スペルカード」関連
//カードのパラメータをアイテムに記録する
bool	record_spellcard_param_onto_card(object_type *o_ptr, nameless_arts_activate_param_type *naap_ptr)
{
	if (o_ptr->tval != TV_SPELLCARD)
	{
		msg_format(_("ERROR:record_spellcard_param_onto_card()にTV_SPELLCARD以外が渡された(%d)",
                    "ERROR: record_spellcard_param_onto_card() called for non-TV_SPELLCARD (%d)"), o_ptr->tval); return FALSE;
	}

	if (naap_ptr->art_idx <= 0 || naap_ptr->art_idx > JKF1_MAX)
	{
		msg_format(_("ERROR:record_spellcard_param_onto_card()にてnaap_ptr->art_idxがおかしい(%d)",
                    "ERROR: Weird naap_ptr->art_idx (%d) for record_spellcard_param_onto_card()"), naap_ptr->art_idx); return FALSE;
	}

	o_ptr->pval = (s16b)(naap_ptr->art_idx);
	o_ptr->xtra1 = (byte)(naap_ptr->gf_type);
	o_ptr->xtra2 = (byte)(naap_ptr->rad);
	o_ptr->xtra3 = (byte)(naap_ptr->counter_idx);//現状常に0だが将来何かするかもしれんので一応記録処理入れとく

	o_ptr->ac = (s16b)(naap_ptr->base);//投げる人がいるかもしれないのでto_dとdsには記録しないでおく。菫子特技で浮かせるとかした場合は...もう勝手にして
	o_ptr->dd = (s16b)(naap_ptr->dice);
	o_ptr->to_a = (s16b)(naap_ptr->sides);
	o_ptr->to_h = (s16b)(naap_ptr->cost);

	o_ptr->xtra4 = (s16b)(naap_ptr->xtra1);
	o_ptr->xtra5 = (s16b)(naap_ptr->xtra2);

	o_ptr->art_name = (s16b)(naap_ptr->quark_idx);//命名したスペカ名を記録する。これによりスペルカードがランダムアーティファクトになる。

	return TRUE;
}

//v1.1.56 真勝利後限定フレーバーアイテム「スペルカード」関連
//カードのパラメータをアイテムから読み出す
bool	read_spellcard_param_from_card(object_type *o_ptr, nameless_arts_activate_param_type *naap_ptr)
{
	int tim_rad;
	if (o_ptr->tval != TV_SPELLCARD)
	{
		msg_format(_("ERROR:read_spellcard_param_from_card()にTV_SPELLCARD以外が渡された(%d)",
                    "ERROR: read_spellcard_param_from_card() called for non-TV_SPELLCARD (%d)"), o_ptr->tval); return FALSE;
	}

	if (o_ptr->pval <= 0 || o_ptr->pval > JKF1_MAX)
	{
		msg_format(_("ERROR:read_spellcard_param_from_card()にてart_idxがおかしい(%d)",
                    "ERROR: Weird art_idx (%d) for read_spellcard_param_from_card()"), o_ptr->pval); return FALSE;
	}

	naap_ptr->art_idx = o_ptr->pval;
	naap_ptr->gf_type = o_ptr->xtra1;

	//ブレスのときradがマイナスになるのを忘れて符号なし8bitに保存したので読み出すときに変換処理する
	if (o_ptr->xtra2 > 127)
		naap_ptr->rad = o_ptr->xtra2 - 256;
	else
		naap_ptr->rad = o_ptr->xtra2;

	naap_ptr->counter_idx = o_ptr->xtra3;

	naap_ptr->base = o_ptr->ac;
	naap_ptr->dice = o_ptr->dd;
	naap_ptr->sides = o_ptr->to_a;
	naap_ptr->cost = o_ptr->to_h;

	naap_ptr->xtra1 = o_ptr->xtra4;
	naap_ptr->xtra2 = o_ptr->xtra5;

	naap_ptr->quark_idx = o_ptr->art_name;

	return TRUE;

}


/*:::真・勝利後限定で無限スペカを開発する建物用コマンド。
 *まあ真・勝利後だしゲームバランスとか気にする必要はないだろう。
 *スペルカードはuコマンドから使用でき、発動部は純狐系の「名も無き技」を使用する。
 *カードのpvalやxtraなどobject_type部に発動に必要なパラメータを記憶しておく。
 */
void	bact_marisa_make_spellcard(void)
{

	int marisa_total_mag_power = 0;
	int mag_type;
	int flag_bonus;
	object_type	forge;
	object_type 	*o_ptr = &forge;
	int price = 10000000;//一枚$1000万
	int k_idx;
	nameless_arts_activate_param_type card_param;

	if (!PLAYER_IS_TRUE_WINNER && !p_ptr->wizard)
	{
		msg_print(_("ERROR:*真・勝利*していないのにmarisa_make_spellcard()が呼ばれた",
                    "ERROR: marisa_make_spellcard() called for non-true winner"));
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print(_("荷物が一杯だ。", "Your inventory is full."));
		clear_bldg(4, 22);
		return;
	}

	screen_save();

	clear_bldg(4, 22);

	flag_bonus = calc_make_spellcard_bonus();

	//画面に現在のパワーを大まかに表示し続行確認
	if (p_ptr->pclass == CLASS_MARISA)
	{
		prt(_("今なら最強のスペルカードを作れる気がする。",
            "You feel like making the strongest spellcard right now."), 10, 15);
		if (flag_bonus > 400)
			prt(_("魔力の備蓄も唸るほどある！",
                "Your mana stockpile is enormous!"), 11, 15);
		else if (flag_bonus > 250)
			prt(_("魔力の備蓄も十分だろう。",
                "You have plenty of mana stockpiled."), 11, 15);
		else if (flag_bonus > 100)
			prt(_("魔力の備蓄も幾らかはある。",
                "You have some mana stockpiled."), 11, 15);
		else
			prt(_("魔力の備蓄が少し心許ないが……",
                "You feel like you don't have enough mana in stock..."), 11, 15);
	}
	else
	{
		prt(_("魔理沙「ほう、私に新しいスペルカードを作ってほしいのか。",
            "Marisa - 'So, you want me to make a new spellcard?"), 10, 15);
		prt(_("　面白そうだ。引き受けてやるぜ。",
            "  Very interesting. Very well, I'll give it a try!"), 11, 15);
		if (flag_bonus > 400)
			prt(_("　魔力の備蓄も唸るほどあるぜ！」",
                "  My mana stockpile is enormous!'"), 12, 15);
		else if (flag_bonus > 250)
			prt(_("　魔力の備蓄も十分だろう。」",
                "  I have plenty of mana stockpiled.'"), 12, 15);
		else if (flag_bonus > 100)
			prt(_("　魔力の備蓄も幾らかはあるしな。」",
                "  I have some mana stockpiled.'"), 12, 15);
		else
			prt(_("　でも魔力の備蓄が少し心許ないな。」",
                "  You know, I don't think I have much mana in stock.'"), 12, 15);

	}

	if (!get_check_strict(_("一枚$1000万かかる。作りますか？",
                            "It'll cost $10,000,000. Are you sure?"), CHECK_DEFAULT_Y))
	{
		screen_load();
		return;
	}

	if (p_ptr->au < price)
	{
		msg_print(_("お金が足りない。", "You don't have enough money."));
		screen_load();
		return;
	}

	k_idx = lookup_kind(TV_SPELLCARD, 0);
	if (!k_idx)
	{
		screen_load();
		return;
	}
	object_prep(o_ptr, k_idx);

	//発動IDX決定
	card_param.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_SPELLCARD);
	//属性決定
	make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_SPELLCARD, &card_param);
	//ダイス値などその他決定
	make_nameless_arts_sub3_apply_other_params(&card_param, NAMELESS_ART_MODE_SPELLCARD);

	//性能を表示し命名　性能表示部をカードを読むルーチンと共通化する
	while (TRUE)
	{
		char art_desc[255] = "";

		display_nameless_art_effect(&card_param, NAMELESS_ART_MODE_SPELLCARD);

		if (!get_string(_("このスペルを何と名づけますか？:",
                        "Name this spell: "), art_desc, 45)) continue;
		prt(format(_("名称:%s", "Name: %s"), art_desc), 21, 30);
		if (!get_check_strict(_("よろしいですか？：", "Are you sure? "), CHECK_OKAY_CANCEL))continue;
		card_param.quark_idx = quark_add(art_desc);
		break;
	}


	//カードのobject_type各要素に発動パラメータを記録し☆化
	record_spellcard_param_onto_card(o_ptr, &card_param);
	o_ptr->ident |= (IDENT_MENTAL);

	//金消費とカード引き渡し
	p_ptr->au -= price;
	building_prt_gold();
	inven_carry(o_ptr);

	//魔法店に備蓄された魔力値を一律20%消費するものとする
	for (mag_type = 0; mag_type<8; mag_type++)
		marisa_magic_power[mag_type] = marisa_magic_power[mag_type] * 4 / 5;

	clear_bldg(4, 22);
	prt(format(_("【%s】が完成した！",
                "[%s] is completed!"), quark_str(card_param.quark_idx)), 18, 30);

	inkey();
	screen_load();

}

//スペルカードをIコマンドやrコマンドで読んだときに効果を表示
void look_spellcard(object_type *o_ptr)
{
	nameless_arts_activate_param_type card_param;
	int k, wid, hgt;

	if (o_ptr->tval != TV_SPELLCARD) { msg_print(_("ERROR:look_spellcard()にスペカ以外が渡された",
                                                    "ERROR: look_spellcard() called for non-spellcard")); return; }

	if (!read_spellcard_param_from_card(o_ptr, &card_param)) return;

	screen_save();
	Term_get_size(&wid, &hgt);
	for (k = 1; k < hgt; k++) prt("", k, 13);

	display_nameless_art_effect(&card_param, NAMELESS_ART_MODE_SPELLCARD);
	inkey();
	screen_load();

}

//スペルカードを使う
void use_spellcard(object_type *o_ptr)
{
	nameless_arts_activate_param_type card_param;
	int k, wid, hgt;

	int dec_mp = 0;
	int dec_hp = 0;

	if (o_ptr->tval != TV_SPELLCARD) { msg_print(_("ERROR:use_spellcard()にスペカ以外が渡された",
                                                    "ERROR: use_spellcard() called for non-spellcard")); return; }

	if(!read_spellcard_param_from_card(o_ptr, &card_param)) return;

	//処理の都合上キャンセルしても行動順消費してしまうのでなんか行動してるっぽいメッセージ入れとく。コストも必ず消費するようにする。
	msg_format(_("あなたは【%s】のカードを掲げた！",
                "You raise the card [%s]!"), quark_str(card_param.quark_idx));


	//コスト処理
	dec_mp = card_param.cost;
	if (dec_mp > p_ptr->csp)
	{
		dec_hp = dec_mp - p_ptr->csp;
		dec_mp = p_ptr->csp;
	}

	if (dec_hp > p_ptr->chp)
	{
		msg_print(_("しかしカードを使うだけの力がない。",
                    "You don't have enough power to use this card."));
		return;
	}
	p_ptr->csp -= dec_mp;
	p_ptr->chp -= dec_hp;
	p_ptr->redraw |= (PR_HP | PR_MANA);

	//カード実行処理
	activate_nameless_art_aux(&card_param, NAMELESS_ART_MODE_SPELLCARD);


}

