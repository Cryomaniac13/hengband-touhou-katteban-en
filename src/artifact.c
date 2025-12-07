/* Purpose: Artifact code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/* Chance of using syllables to form the name instead of the "template" files */
#define SINDARIN_NAME   10
#define TABLE_NAME      20
#define A_CURSED        13
///mod131229 WEIRD_LUCKを関数に変更　ラッキーマンのとき良い値が出やすいようにする
//#define WEIRD_LUCK      12
#define BIAS_LUCK       20
#define IM_LUCK         7

/*
 * Bias luck needs to be higher than weird luck,
 * since it is usually tested several times...
 */
#define ACTIVATION_CHANCE 3


/*
 * Use for biased artifact creation
 */
static int artifact_bias;

///mod131229 白いオーラ持ちのとき様々なことが有利
///mod141231 霊夢占い追加
bool weird_luck(void)
{

	if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 8)  return (one_in_(6));
	else if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 6)  return (one_in_(7));
	else if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 4)  return (one_in_(9));

	else if(p_ptr->lucky) return (one_in_(7));
	else if(p_ptr->today_mon == FORTUNETELLER_GOODLUCK)
	{
		if(p_ptr->muta3 & MUT3_GOOD_LUCK) return (one_in_(7));
		else return (one_in_(9));
	}
	else if(p_ptr->muta3 & MUT3_GOOD_LUCK) return (one_in_(9));
	else return (one_in_(12));
}


/*
 * Choose one random sustain
 */
/*:::ランダム能力維持付加*/
void one_sustain(object_type *o_ptr)
{
	switch (randint0(6))
	{
		case 0: add_flag(o_ptr->art_flags, TR_SUST_STR); break;
		case 1: add_flag(o_ptr->art_flags, TR_SUST_INT); break;
		case 2: add_flag(o_ptr->art_flags, TR_SUST_WIS); break;
		case 3: add_flag(o_ptr->art_flags, TR_SUST_DEX); break;
		case 4: add_flag(o_ptr->art_flags, TR_SUST_CON); break;
		case 5: add_flag(o_ptr->art_flags, TR_SUST_CHR); break;
	}
}


/*
 * Choose one random high resistance
 */
/*:::ランダム上位耐性付与*/
///res ランダム上位耐性付与
///mod131228 上位耐性変更 時空耐性と神聖耐性は少し出にくいようにした
void one_high_resistance(object_type *o_ptr)
{

	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		return;
	}
	if(one_in_(16)){
		add_flag(o_ptr->art_flags, TR_RES_HOLY);
		return;
	}

	switch (randint0(9))
	{
		case  0: add_flag(o_ptr->art_flags, TR_RES_POIS);   break;
		case  1: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
		case  2: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
		case  3: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
		case  4: add_flag(o_ptr->art_flags, TR_RES_WATER);  break;
		case  5: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
		case  6: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
		case  7: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
		case  8: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
	}
}


/*
 * Choose one random high resistance ( except poison and disenchantment )
 */
 /*:::王者の加護の指輪生成専用の上位耐性付加　毒と劣化が抜けている*/
/// res　王者の加護の指輪の耐性付加
///mod131228 上位耐性変更
void one_lordly_high_resistance(object_type *o_ptr)
{
	switch (randint0(9))
	{
		case 0: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
		case 1: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
		case 2: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
		case 3: add_flag(o_ptr->art_flags, TR_RES_WATER);  break;
		case 4: add_flag(o_ptr->art_flags, TR_RES_HOLY);   break;
		case 5: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
		case 6: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
		case 7: add_flag(o_ptr->art_flags, TR_RES_TIME);   break;
		case 8: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
	}
}


/*
 * Choose one random element resistance
 */
/*:::ランダム元素耐性付与　毒除く*/
void one_ele_resistance(object_type *o_ptr)
{
	switch (randint0(4))
	{
		case  0: add_flag(o_ptr->art_flags, TR_RES_ACID); break;
		case  1: add_flag(o_ptr->art_flags, TR_RES_ELEC); break;
		case  2: add_flag(o_ptr->art_flags, TR_RES_COLD); break;
		case  3: add_flag(o_ptr->art_flags, TR_RES_FIRE); break;
	}
}


/*
 * Choose one random element or poison resistance
 */
/*:::ランダム元素耐性付与　ただし毒が付くことがある*/
void one_dragon_ele_resistance(object_type *o_ptr)
{
	if (one_in_(7))
	{
		add_flag(o_ptr->art_flags, TR_RES_POIS);
	}
	else
	{
		one_ele_resistance(o_ptr);
	}
}


/*
 * Choose one lower rank esp
 */
/*:::アイテム生成時、弱いESPを付与する*/
///item mon 弱いESP付与
///mod131229 弱いESPは動死悪竜人秩神妖とする
void one_low_esp(object_type *o_ptr)
{
	switch (randint1(8))
	{
	case 1:  add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);   break;
	case 2:  add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);   break;
	case 3:  add_flag(o_ptr->art_flags, TR_ESP_DEMON);   break;
	case 4:  add_flag(o_ptr->art_flags, TR_ESP_KWAI);   break;
	case 5:  add_flag(o_ptr->art_flags, TR_ESP_DEITY);   break;
	case 6:  add_flag(o_ptr->art_flags, TR_ESP_GOOD);   break;
	case 7:  add_flag(o_ptr->art_flags, TR_ESP_DRAGON);   break;
	case 8:  add_flag(o_ptr->art_flags, TR_ESP_HUMAN);   break;
	}
}



/*
 * Choose one random resistance
 */
/*:::ランダム耐性*/
void one_resistance(object_type *o_ptr)
{
	if (one_in_(3))
	{
		one_ele_resistance(o_ptr);
	}
	else
	{
		one_high_resistance(o_ptr);
	}
}


/*
 * Choose one random ability
 */
/*:::装備品生成時の追加能力付与*/
///item res 能力付与　混乱耐性などをこちらに移そう
void one_ability(object_type *o_ptr)
{

	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_RES_INSANITY);
		return;
	}
	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_SPEEDSTER);
		return;
	}

	switch (randint0(15))
	{
	case 0: add_flag(o_ptr->art_flags, TR_LEVITATION);     break;
	case 1: add_flag(o_ptr->art_flags, TR_LITE);        break;
	case 2: add_flag(o_ptr->art_flags, TR_SEE_INVIS);   break;
	case 3: add_flag(o_ptr->art_flags, TR_WARNING);     break;
	case 4: add_flag(o_ptr->art_flags, TR_SLOW_DIGEST); break;
	case 5: add_flag(o_ptr->art_flags, TR_REGEN);       break;
	case 6:
	case 7:
			add_flag(o_ptr->art_flags, TR_FREE_ACT);    break;
	case 8:
	case 9:
			add_flag(o_ptr->art_flags, TR_RES_CONF);    break;
	case 10:
	case 11:
			add_flag(o_ptr->art_flags, TR_RES_FEAR);    break;
	case 12:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);    break;
	case 13:
	case 14:
			one_low_esp(o_ptr);	break;
	}
}

/*
 * Choose one random activation
 */
void one_activation(object_type *o_ptr)
{
	int type = 0;
	int chance = 0;

	while (randint1(100) >= chance)
	{
		type = randint1(255);
		switch (type)
		{
			case ACT_SUNLIGHT:
			case ACT_BO_MISS_1:
			case ACT_BA_POIS_1:
			case ACT_BO_ELEC_1:
			case ACT_BO_ACID_1:
			case ACT_BO_COLD_1:
			case ACT_BO_FIRE_1:
			case ACT_CONFUSE:
			case ACT_SLEEP:
			case ACT_QUAKE:
			case ACT_CURE_LW:
			case ACT_CURE_MW:
			case ACT_CURE_POISON:
			case ACT_BERSERK:
			case ACT_LIGHT:
			case ACT_MAP_LIGHT:
			case ACT_DEST_DOOR:
			case ACT_STONE_MUD:
			case ACT_TELEPORT:
				chance = 101;
				break;
			case ACT_BA_COLD_1:
			case ACT_BA_FIRE_1:
			case ACT_DRAIN_1:
			case ACT_TELE_AWAY:
			case ACT_ESP:
			case ACT_RESIST_ALL:
			case ACT_DETECT_ALL:
			case ACT_RECALL:
			case ACT_SATIATE:
			case ACT_RECHARGE:
				chance = 85;
				break;
			case ACT_TERROR:
			case ACT_PROT_EVIL:
			case ACT_ID_PLAIN:
				chance = 75;
				break;
			case ACT_DRAIN_2:
			case ACT_VAMPIRE_1:
			case ACT_BO_MISS_2:
			case ACT_BA_FIRE_2:
			case ACT_REST_LIFE:
				chance = 66;
				break;
			case ACT_BA_FIRE_3:
			case ACT_BA_COLD_3:
			case ACT_BA_ELEC_3:
			case ACT_WHIRLWIND:
			case ACT_TORNADO:
			case ACT_VAMPIRE_2:
			case ACT_CHARM_ANIMAL:
				chance = 50;
				break;
			case ACT_SUMMON_ANIMAL:
				chance = 40;
				break;
			case ACT_DISP_EVIL:
			case ACT_BA_MISS_3:
			case ACT_DISP_GOOD:
			case ACT_BANISH_EVIL:
			case ACT_GENOCIDE:
			case ACT_MASS_GENO:
			case ACT_CHARM_UNDEAD:
			case ACT_CHARM_OTHER:
			case ACT_SUMMON_PHANTOM:
			case ACT_REST_ALL:
			case ACT_RUNE_EXPLO:
			case ACT_DAZZLE:
			case ACT_CURE_500:
				chance = 33;
				break;
			case ACT_CALL_CHAOS:
			case ACT_ROCKET:
			case ACT_CHARM_ANIMALS:
			case ACT_CHARM_OTHERS:
			case ACT_SUMMON_ELEMENTAL:
			case ACT_CURE_700:
			case ACT_SPEED:
			case ACT_ID_FULL:
			case ACT_RUNE_PROT:
				chance = 25;
				break;
			case ACT_CURE_1000:
			case ACT_XTRA_SPEED:
			case ACT_DETECT_XTRA:
			case ACT_DIM_DOOR:
			case ACT_DETECT_ARTIFACT:
				chance = 10;
				break;
			case ACT_SUMMON_UNDEAD:
			case ACT_SUMMON_DEMON:
			case ACT_WRAITH:
			case ACT_INVULN:
			case ACT_ALCHEMY:
				chance = 5;
				break;

			case ACT_SWORD_DANCE:
				if (o_ptr->tval >= TV_KNIFE && o_ptr->tval <= TV_KATANA)
					chance = 30;
				else
					chance = 0;
				break;

			//v1.1.74
			case ACT_TIM_ADD_DICES:
			case ACT_PSY_SPEAR:
			case ACT_LONG_RANGE_ATTACK:
				if (object_is_melee_weapon(o_ptr))
					chance = 50;
				else
					chance = 0;
				break;

			case ACT_HIT_AND_AWAY:
				if (object_is_melee_weapon(o_ptr) || o_ptr->tval == TV_BOOTS)
					chance = 75;
				else
					chance = 0;
				break;
			case ACT_SAINT_STAR_ARROW:
				if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
					chance = 50;
				else
					chance = 0;
				break;
			case ACT_TIM_SUPER_STEALTH:
				if (o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_BOOTS || o_ptr->tval == TV_LITE)
					chance = 30;
				else
					chance = 10;
				break;
			case ACT_DUAL_CAST:
				if (o_ptr->tval == TV_HEAD || o_ptr->tval == TV_AMULET || o_ptr->tval == TV_RING)
					chance = 50;
				else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_WIZSTAFF)
					chance = 50;
				else if (object_is_melee_weapon(o_ptr))
					chance = 30;
				else
					chance = 5;
				break;


			default:
				chance = 0;
		}
	}

	/* A type was chosen... */
	o_ptr->xtra2 = type;
	add_flag(o_ptr->art_flags, TR_ACTIVATE);
	o_ptr->timeout = 0;
}

/*:::☆が呪われているときのフラグ設定*/
///class
static void curse_artifact(object_type * o_ptr)
{
	if (o_ptr->pval > 0) o_ptr->pval = 0 - (o_ptr->pval + randint1(4));
	if (o_ptr->to_a > 0) o_ptr->to_a = 0 - (o_ptr->to_a + randint1(4));
	if (o_ptr->to_h > 0) o_ptr->to_h = 0 - (o_ptr->to_h + randint1(4));
	if (o_ptr->to_d > 0) o_ptr->to_d = 0 - (o_ptr->to_d + randint1(4));

	o_ptr->curse_flags |= (TRC_HEAVY_CURSE | TRC_CURSED);
	remove_flag(o_ptr->art_flags, TR_BLESSED);

	if (one_in_(4)) o_ptr->curse_flags |= TRC_PERMA_CURSE;
	if (one_in_(3)) add_flag(o_ptr->art_flags, TR_TY_CURSE);
	if (one_in_(2)) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
	if (one_in_(3)) add_flag(o_ptr->art_flags, TR_DRAIN_EXP);
	if (one_in_(6)) add_flag(o_ptr->art_flags, TR_ADD_L_CURSE);
	if (one_in_(9)) add_flag(o_ptr->art_flags, TR_ADD_H_CURSE);
	if (one_in_(2)) add_flag(o_ptr->art_flags, TR_TELEPORT);
	else if (one_in_(3)) add_flag(o_ptr->art_flags, TR_NO_TELE);

	if ((p_ptr->pclass != CLASS_WARRIOR) && (p_ptr->pclass != CLASS_ARCHER) && (p_ptr->pclass != CLASS_CAVALRY) && one_in_(3))
		add_flag(o_ptr->art_flags, TR_NO_MAGIC);
}

/*:::アーティファクト生成時の能力値アップ　バイアス値に依存*/
///class?
static void random_plus(object_type * o_ptr)
{
	//v1.1.99 拡張
	//int this_type = (object_is_weapon_ammo(o_ptr) ? 25 : 21);
	int this_type = (object_is_weapon_ammo(o_ptr) ? 29 : 25);

	switch (artifact_bias)
	{
	case BIAS_WARRIOR:
		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_MAGE:
		if (!(have_flag(o_ptr->art_flags, TR_INT)))
		{
			add_flag(o_ptr->art_flags, TR_INT);
			if (one_in_(2)) return;
		}
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY)))
		{
			add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		if (!(have_flag(o_ptr->art_flags, TR_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_WIS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SAVING)))
		{
			add_flag(o_ptr->art_flags, TR_SAVING);
			if (one_in_(2)) return;
		}

		break;

	case BIAS_RANGER:
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_DISARM)))
		{
			add_flag(o_ptr->art_flags, TR_DISARM);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_DISARM)))
		{
			add_flag(o_ptr->art_flags, TR_DISARM);
			if (one_in_(4)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_STEALTH)))
		{
			add_flag(o_ptr->art_flags, TR_STEALTH);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SEARCH)))
		{
			add_flag(o_ptr->art_flags, TR_SEARCH);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if (!(have_flag(o_ptr->art_flags, TR_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_WIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if (!(have_flag(o_ptr->art_flags, TR_INT)))
		{
			add_flag(o_ptr->art_flags, TR_INT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if (!(have_flag(o_ptr->art_flags, TR_CHR)))
		{
			add_flag(o_ptr->art_flags, TR_CHR);
			if (one_in_(2)) return;
		}
		break;
	}

	if ((artifact_bias == BIAS_MAGE || artifact_bias == BIAS_PRIESTLY) && (o_ptr->tval == TV_CLOTHES) && (o_ptr->sval == SV_CLOTH_ROBE))
	{
		if (!(have_flag(o_ptr->art_flags, TR_DEC_MANA)) && one_in_(3))
		{
			add_flag(o_ptr->art_flags, TR_DEC_MANA);
			if (one_in_(2)) return;
		}
	}
	///mod131229 アミュと手の☆には魔道具支配が付きやすくした
	if((o_ptr->tval == TV_GLOVES || o_ptr->tval == TV_AMULET || o_ptr->tval == TV_RIBBON) && one_in_(100) && !(have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY))){
		add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
		return;
	}

	///mod150205 光源は1/2の確率で別のルーチンにする
	if(o_ptr->tval == TV_LITE && one_in_(2))
	{
		switch(randint1(10))
		{
		case 1: case 6: case 10:
			add_flag(o_ptr->art_flags, TR_SEARCH);
			break;
		case 2: case 7:
			add_flag(o_ptr->art_flags, TR_INFRA);
			break;
		case 3:
			add_flag(o_ptr->art_flags, TR_INT);
			break;
		case 4: case 8:
			add_flag(o_ptr->art_flags, TR_WIS);
			break;
		case 5: case 9:
			add_flag(o_ptr->art_flags, TR_CHR);
			break;
		}
		return;
	}




	switch (randint1(this_type))
	{
	case 1: case 2:
		add_flag(o_ptr->art_flags, TR_STR);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_STR;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_WARRIOR;
		break;
	case 3: case 4:
		add_flag(o_ptr->art_flags, TR_INT);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_INT;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_MAGE;
		break;
	case 5: case 6:
		add_flag(o_ptr->art_flags, TR_WIS);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_WIS;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_PRIESTLY;
		break;
	case 7: case 8:
		add_flag(o_ptr->art_flags, TR_DEX);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_DEX;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_ROGUE;
		break;
	case 9: case 10:
		add_flag(o_ptr->art_flags, TR_CON);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CON;
		else if (!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 11: case 12:
		add_flag(o_ptr->art_flags, TR_CHR);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CHR;
		break;
	case 13: case 14:
		add_flag(o_ptr->art_flags, TR_STEALTH);
		if (!artifact_bias && one_in_(3))
			artifact_bias = BIAS_ROGUE;
		break;
	case 15: case 16:
		add_flag(o_ptr->art_flags, TR_SEARCH);
		if (!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 17: case 18:
		add_flag(o_ptr->art_flags, TR_INFRA);
		break;
	case 19:
		add_flag(o_ptr->art_flags, TR_SPEED);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_ROGUE;
		break;

	case 20:case 21:
		add_flag(o_ptr->art_flags, TR_DISARM);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_ROGUE;
		break;
	case 22:case 23:
		add_flag(o_ptr->art_flags, TR_SAVING);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_PRIESTLY;
		break;




	///mod131230 TVALから見てそれっぽいPVALが付きやすくなるようにしてみる
	case 24: case 25:
		switch(o_ptr->tval)
		{
		case TV_KNIFE:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_STEALTH);
			else 				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_SWORD:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_CON);
			else 				add_flag(o_ptr->art_flags, TR_STR);
			break;
		case TV_KATANA:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_HAMMER: case TV_AXE: case TV_POLEARM:
			add_flag(o_ptr->art_flags, TR_TUNNEL);
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_CON);
			break;
		case TV_STICK:
			if(o_ptr->sval == SV_WEAPON_WIZSTAFF || o_ptr->sval == SV_WEAPON_QUARTERSTAFF  || o_ptr->sval == SV_WEAPON_SYAKUJYOU) {
				if(one_in_(3))		add_flag(o_ptr->art_flags, TR_INT);
				else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_WIS);
				else				add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
			}
			else{
				if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
				else				add_flag(o_ptr->art_flags, TR_BLOWS);
			}
			break;
		case TV_SPEAR:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_DEX);
			else				add_flag(o_ptr->art_flags, TR_SPEED);
			break;
		case TV_BOW: case TV_CROSSBOW:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_DEX);
			else				add_flag(o_ptr->art_flags, TR_STEALTH);
			break;
		case TV_SHIELD: case TV_ARMOR: case TV_DRAG_ARMOR:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_CHR);

			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_CON);
			break;
		case TV_CLOAK:
			add_flag(o_ptr->art_flags, TR_STEALTH);
			if(one_in_(2)) break;

			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_CHR);
			else				add_flag(o_ptr->art_flags, TR_SEARCH);
			break;

		case TV_CLOTHES:
			add_flag(o_ptr->art_flags, TR_SAVING);
			if (one_in_(2))		add_flag(o_ptr->art_flags, TR_CHR);

			break;
		case TV_HEAD:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_SEARCH);
			else				add_flag(o_ptr->art_flags, TR_CHR);
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_INT);
			else				add_flag(o_ptr->art_flags, TR_WIS);
			break;
		case TV_GLOVES:
			if (one_in_(3))		add_flag(o_ptr->art_flags, TR_DISARM);
			else if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_BOOTS:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STEALTH);
			else				add_flag(o_ptr->art_flags, TR_SPEED);
			break;

		case TV_GUN:
			if(one_in_(2)) add_flag(o_ptr->art_flags, TR_DEX);
			else add_flag(o_ptr->art_flags, TR_STEALTH);
			break;

		default:
			random_plus(o_ptr);
			break;
		}
		break;
	case 26: case 27:
		add_flag(o_ptr->art_flags, TR_TUNNEL);
		break;
	case 28: case 29:
		if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW || o_ptr->tval == TV_GUN)
			random_plus(o_ptr);
		else
		{
			add_flag(o_ptr->art_flags, TR_BLOWS);
			if (!artifact_bias && one_in_(11))
				artifact_bias = BIAS_WARRIOR;
		}
		break;
	}
}

/*:::アーティファクト生成時のランダム耐性　バイアスに依存*/
///res item ☆生成時の耐性付与
///mod131228 TR_??フラグ変更　因果混乱を削除し水、神聖、時空耐性追加
///pend アーティファクトバイアスの調整
static void random_resistance(object_type * o_ptr)
{
	switch (artifact_bias)
	{
	case BIAS_ACID:
		if (!(have_flag(o_ptr->art_flags, TR_RES_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			if (one_in_(2)) return;
		}
		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_IM_ACID);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		///mod131228 tval

		if (!(have_flag(o_ptr->art_flags, TR_RES_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			if (one_in_(2)) return;
		}

		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) && !(have_flag(o_ptr->art_flags, TR_SH_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_SH_ELEC);
			if (one_in_(2)) return;
		}

		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_IM_ELEC);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}

			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_RES_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			if (one_in_(2)) return;
		}
		///mod131228 tval
		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) &&
		    !(have_flag(o_ptr->art_flags, TR_SH_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_SH_FIRE);
			if (one_in_(2)) return;
		}

		if (one_in_(BIAS_LUCK) &&
		    !(have_flag(o_ptr->art_flags, TR_IM_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_IM_FIRE);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if (!(have_flag(o_ptr->art_flags, TR_RES_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			if (one_in_(2)) return;
		}
		///mod131228 tval

		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) &&
		    !(have_flag(o_ptr->art_flags, TR_SH_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_SH_COLD);
			if (one_in_(2)) return;
		}

		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_IM_COLD);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WARRIOR:
		if (!one_in_(3) && (!(have_flag(o_ptr->art_flags, TR_RES_FEAR))))
		{
			add_flag(o_ptr->art_flags, TR_RES_FEAR);
			if (one_in_(2)) return;
		}
		if (one_in_(3) && (!(have_flag(o_ptr->art_flags, TR_NO_MAGIC))))
		{
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_NECROMANTIC:
		if (!(have_flag(o_ptr->art_flags, TR_RES_NETHER)))
		{
			add_flag(o_ptr->art_flags, TR_RES_NETHER);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_DARK)))
		{
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_RES_CHAOS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_CHAOS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_CONF)))
		{
			add_flag(o_ptr->art_flags, TR_RES_CONF);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_DISEN)))
		{
			add_flag(o_ptr->art_flags, TR_RES_DISEN);
			if (one_in_(2)) return;
		}
		break;
	}

	///mod150205 光源は1/2の確率で別のルーチンにする
	if(o_ptr->tval == TV_LITE && one_in_(2))
	{
		int cnt;
		switch(randint1(28))
		{
			//v1.1.30 ちょっと変更
		case 1: case 13:
			add_flag(o_ptr->art_flags, TR_ESP_EVIL);
			break;
		case 2: case 14:
			add_flag(o_ptr->art_flags, TR_ESP_NONLIVING);
			break;
		case 3: case 15:
			add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
			break;
		case 4: case 16:
			add_flag(o_ptr->art_flags, TR_TELEPATHY);
			break;
		case 5: case 6: case 17: case 24:
			for(cnt=randint1(3);cnt>0;cnt--) one_low_esp(o_ptr);
			break;
		case 7: case 18: case 25:
			add_flag(o_ptr->art_flags, TR_WARNING);
			break;
		case 8:
			add_flag(o_ptr->art_flags, TR_EASY_SPELL);
			break;
		case 9: case 19: case 26:
			add_flag(o_ptr->art_flags, TR_RES_LITE);
			break;
		case 10: case 20: case 23:
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			break;
		case 11: case 21: case 27:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);
			break;
		case 12: case 22: case 28:
			add_flag(o_ptr->art_flags, TR_SEE_INVIS);
			break;

		}
		return;
	}

	///res
	switch (randint1(41))
	{
		case 1:
			/*:::免疫は付きにくい*/
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_ACID);
				if (!artifact_bias)
					artifact_bias = BIAS_ACID;
			}
			break;
		case 2:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_ELEC);
				if (!artifact_bias)
					artifact_bias = BIAS_ELEC;
			}
			break;
		case 3:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_COLD);
				if (!artifact_bias)
					artifact_bias = BIAS_COLD;
			}
			break;
		case 4:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_FIRE);
				if (!artifact_bias)
					artifact_bias = BIAS_FIRE;
			}
			break;
		case 5:
		case 6:
		case 13:
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			if (!artifact_bias)
				artifact_bias = BIAS_ACID;
			break;
		case 7:
		case 8:
		case 14:
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 9:
		case 10:
		case 15:
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 11:
		case 12:
		case 16:
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
		case 17:
		case 18:
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (!artifact_bias && !one_in_(4))
				artifact_bias = BIAS_POIS;
			else if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_NECROMANTIC;
			else if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_ROGUE;
			break;
		case 19:
		case 20:
			add_flag(o_ptr->art_flags, TR_RES_LITE);
			break;
		case 21:
		case 22:
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			break;

		case 23:
		case 24:
			add_flag(o_ptr->art_flags, TR_RES_SOUND);
			break;
		case 25:
		case 26:
			add_flag(o_ptr->art_flags, TR_RES_SHARDS);
			break;
		case 27:
		case 28:
			add_flag(o_ptr->art_flags, TR_RES_NETHER);
			if (!artifact_bias && one_in_(3))
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 29:
		case 30:
			add_flag(o_ptr->art_flags, TR_RES_WATER);
			break;
		case 31:
		case 32:
			add_flag(o_ptr->art_flags, TR_RES_CHAOS);
			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_CHAOS;
			break;
		case 33:
		case 34:
			add_flag(o_ptr->art_flags, TR_RES_DISEN);
			break;
		case 35:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_ELEC);
			else

				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 36:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_FIRE);
			else
				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 37:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_COLD);
			else
				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;

		case 38:
			///mod31228 反射が付く確率を減らした
			if ( (o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_ARMOR) && one_in_(2))
				add_flag(o_ptr->art_flags, TR_REFLECT);
			else
				random_resistance(o_ptr);
			break;
		case 39:
			add_flag(o_ptr->art_flags, TR_RES_HOLY);
			break;
		case 40:
			add_flag(o_ptr->art_flags, TR_RES_TIME);
			break;
///mod131229 盾・胴・クロークには全耐性が付く可能性を高めてみた
		case 41:
		if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES ||  o_ptr->tval == TV_SHIELD){
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			add_flag(o_ptr->art_flags, TR_RES_COLD);
		}
		else
				random_resistance(o_ptr);

		break;
	}
}


/*:::☆生成時の能力付加*/
//低層で麻痺知らずが出やすく反射が出にくいような処理もここに入れたい
///mod131229 ☆生成能力追加　アイテムフラグ入れ替えなどで大幅変更
static void random_misc(object_type * o_ptr)
{
	switch (artifact_bias)
	{
	case BIAS_RANGER:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_STR)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_WIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_INT)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_INT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CHR)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CHR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_TELEPORT)))
		{
			add_flag(o_ptr->art_flags, TR_TELEPORT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_LITE)))
		{
			add_flag(o_ptr->art_flags, TR_LITE); /* Freebie */
		}
		break;
	}

	switch (randint1(40))
	{
		case 1:
			add_flag(o_ptr->art_flags, TR_SUST_STR);
			if (!artifact_bias)
				artifact_bias = BIAS_STR;
			break;
		case 2:
			add_flag(o_ptr->art_flags, TR_SUST_INT);
			if (!artifact_bias)
				artifact_bias = BIAS_INT;
			break;
		case 3:
			add_flag(o_ptr->art_flags, TR_SUST_WIS);
			if (!artifact_bias)
				artifact_bias = BIAS_WIS;
			break;
		case 4:
			add_flag(o_ptr->art_flags, TR_SUST_DEX);
			if (!artifact_bias)
				artifact_bias = BIAS_DEX;
			break;
		case 5:
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (!artifact_bias)
				artifact_bias = BIAS_CON;
			break;
		case 6:
			add_flag(o_ptr->art_flags, TR_SUST_CHR);
			if (!artifact_bias)
				artifact_bias = BIAS_CHR;
			break;
		case 7:
		case 8:
		case 9:
			add_flag(o_ptr->art_flags, TR_FREE_ACT);
			break;
		case 10:
		case 11:
			add_flag(o_ptr->art_flags, TR_RES_CONF);
			break;
		case 12:
		case 13:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);
			break;
		case 14:
		case 15:
			add_flag(o_ptr->art_flags, TR_RES_FEAR);
			break;
		case 16:
		case 17:
			add_flag(o_ptr->art_flags, TR_LITE);
			break;
		case 18:
		case 19:
			add_flag(o_ptr->art_flags, TR_LEVITATION);
			break;
		case 20:
		case 21:
		case 22:
			add_flag(o_ptr->art_flags, TR_SEE_INVIS);
			break;
		case 23:
		case 24:
			add_flag(o_ptr->art_flags, TR_SLOW_DIGEST);
			break;
		case 25:
		case 26:
			add_flag(o_ptr->art_flags, TR_REGEN);
			break;
		case 27:
			add_flag(o_ptr->art_flags, TR_TELEPORT);
			break;
		case 28:
		case 29:
			if (object_is_armour(o_ptr))
				random_misc(o_ptr);
			else
			{
				o_ptr->to_a = 4 + randint1(11);
			}
			break;
		case 30:
		case 31:
		case 32:
		{
			int bonus_h, bonus_d;
			add_flag(o_ptr->art_flags, TR_SHOW_MODS);
			bonus_h = 4 + (randint1(11));
			bonus_d = 4 + (randint1(11));

			/*:::Hack - 銃へのダメージボーナスを増加　その増加値はk_infoベースアイテム値に準ずる*/
			if(o_ptr->tval == TV_GUN)
			{
				int mult_h = 100 + k_info[o_ptr->k_idx].to_h * 5;
				int mult_d = 100 + k_info[o_ptr->k_idx].to_d * 5;

				bonus_h = bonus_h * mult_h / 100 + randint1(bonus_h);
				bonus_d = bonus_d * mult_h / 100 + randint1(bonus_d);
			}
			else if ((o_ptr->tval != TV_RING) && (o_ptr->tval != TV_GLOVES) && !(object_is_melee_weapon(o_ptr)))
			{
				bonus_h /= 2;
				bonus_d /= 2;
			}

			o_ptr->to_h += bonus_h;
			o_ptr->to_d += bonus_d;
			break;
		}
		case 33:
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			break;
		case 34:
			add_flag(o_ptr->art_flags, TR_NO_TELE);
			break;
		case 35:
			add_flag(o_ptr->art_flags, TR_WARNING);
			break;

		case 36:
			switch (randint1(4))
			{
			case 1:
				add_flag(o_ptr->art_flags, TR_ESP_EVIL);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_LAW;
				break;
			case 2:
				add_flag(o_ptr->art_flags, TR_ESP_NONLIVING);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_MAGE;
				break;
			case 3:
				add_flag(o_ptr->art_flags, TR_TELEPATHY);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_MAGE;
				break;
			case 4:
				add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_MAGE;
				break;

			}
			break;

		case 37:
		{
			int idx[3];
			int n = randint1(3);
			int i;

			/* v1.1.30 何となく見直したら凄く気持ち悪いコードなので変更　どうせ1~3のランダム個数なので被っても気にしない
			idx[0] = randint1(8);

			idx[1] = randint1(7);
			if (idx[1] >= idx[0]) idx[1]++;

			idx[2] = randint1(6);
			if (idx[2] >= idx[0]) idx[2]++;
			if (idx[2] >= idx[1]) idx[2]++;
			*/
			for(i=0;i<n;i++)
				idx[i] = randint1(8);

			for (i = 0; i < n; i++) switch (idx[i])
			{
			case 1:
				add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);
				if (!artifact_bias && one_in_(4))
					artifact_bias = BIAS_RANGER;
				break;
			case 2:
				add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_PRIESTLY;
				else if (!artifact_bias && one_in_(6))
					artifact_bias = BIAS_NECROMANTIC;
				break;
			case 3:
				add_flag(o_ptr->art_flags, TR_ESP_DEMON);
				break;
			case 4:
				add_flag(o_ptr->art_flags, TR_ESP_DEITY);
				break;
			case 5:
				add_flag(o_ptr->art_flags, TR_ESP_KWAI);
				break;
			case 6:
				add_flag(o_ptr->art_flags, TR_ESP_GOOD);
				break;
			case 7:
				add_flag(o_ptr->art_flags, TR_ESP_HUMAN);
				if (!artifact_bias && one_in_(6))
					artifact_bias = BIAS_ROGUE;
				break;
			case 8:
				add_flag(o_ptr->art_flags, TR_ESP_DRAGON);
				break;
			}
			break;
		}
		case 38:
		case 39:
		{
			if (object_is_melee_weapon(o_ptr))
			{
				switch (randint1(7))
				{
				case 1:	case 2:
				case 3:	case 4:
					{
						u32b flgs[TR_FLAG_SIZE];
						object_flags(o_ptr, flgs);
						if(have_flag(flgs, TR_THROW)) add_flag(o_ptr->art_flags, TR_BOOMERANG);
						else add_flag(o_ptr->art_flags, TR_THROW);
					}
					break;
				case 5:
					add_flag(o_ptr->art_flags, TR_RIDING);
					break;
				case 6:
					add_flag(o_ptr->art_flags, TR_EASY_SPELL);
					break;
				case 7:
					add_flag(o_ptr->art_flags, TR_RES_INSANITY);
					break;
				}
			}
			else if (object_is_ammo(o_ptr))
			{
				o_ptr->to_h += 5 + randint1(15);
				o_ptr->to_d += 5 + randint1(15);
			}
			else if (o_ptr->tval == TV_GUN )
			{
			 	add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
			}
			else if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW  )
			{
				if(one_in_(2))	add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				else 	add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
			}
			else if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES || o_ptr->tval == TV_DRAG_ARMOR)
			{
				switch (randint1(4))
				{
				case 1:
					add_flag(o_ptr->art_flags, TR_SUST_CHR);
					add_flag(o_ptr->art_flags, TR_SUST_INT);
					add_flag(o_ptr->art_flags, TR_SUST_WIS);
					break;
				case 2:
					add_flag(o_ptr->art_flags, TR_SUST_STR);
					add_flag(o_ptr->art_flags, TR_SUST_CON);
					add_flag(o_ptr->art_flags, TR_SUST_DEX);
					break;
				case 3:
					add_flag(o_ptr->art_flags, TR_DEC_MANA);
					break;
				case 4:
					add_flag(o_ptr->art_flags, TR_RES_INSANITY);
					break;
				}
			}
			else if (o_ptr->tval == TV_HEAD )
			{
				add_flag(o_ptr->art_flags, TR_RES_INSANITY);
			}
			else if (o_ptr->tval == TV_GLOVES )
			{
				if(one_in_(3))	add_flag(o_ptr->art_flags, TR_GENZI);
				else{
					o_ptr->to_h += 3 + randint1(7);
					o_ptr->to_d += 3 + randint1(7);
				}
			}
			else if (o_ptr->tval == TV_BOOTS )
			{
				add_flag(o_ptr->art_flags, TR_SPEEDSTER);
			}
			else if (o_ptr->tval == TV_RING )
			{
				if(one_in_(2))	add_flag(o_ptr->art_flags, TR_DEC_MANA);
				else			add_flag(o_ptr->art_flags, TR_EASY_SPELL);
			}
			else	add_flag(o_ptr->art_flags, TR_RES_INSANITY);

			break;
		}
		//狂気耐性が流石に出にくすぎる気がするので追加
		case 40:
			add_flag(o_ptr->art_flags, TR_RES_INSANITY);
			break;
	}
}

/*:::アーティファクト生成時のスレイや武器能力付加*/
///item mon
///mod131229 ☆生成スレイなど付加　アイテムフラグ入れ替えなどで大幅変更
static void random_slay(object_type *o_ptr)
{
	///mod160504 銃が☆化したときはダメージ増加や射撃種類変化が起こる
	if (o_ptr->tval == TV_GUN)
	{
		int change_mode_chance;

		//属性変更の起こる確率設定
		switch(o_ptr->sval)
		{
		case SV_FIRE_GUN_CRIMSON:
			change_mode_chance = 70;
			break;
		case SV_FIRE_GUN_LASER:
		case SV_FIRE_GUN_MUSKET:
			change_mode_chance = 50;
			break;
		case SV_FIRE_GUN_LUNATIC:
		case SV_FIRE_GUN_SHOTGUN:
		case SV_FIRE_GUN_ROCKET:
			change_mode_chance = 33;
			break;
		default:
			change_mode_chance = 10;
			break;
		}
		//銃の属性を変更する
		if(randint0(100) < change_mode_chance)
		{
			int new_type = 0;
			int old_type = o_ptr->xtra1;
			if(!artifact_bias)  artifact_bias = randint1(ARTIFACT_BIAS_MAX);

			switch(artifact_bias)
			{
			case BIAS_ELEC:
				if(old_type == GF_PLASMA || one_in_(3)) new_type = GF_PLASMA;
				else new_type = GF_ELEC;
				break;
			case BIAS_POIS:
				if(old_type == GF_POLLUTE || one_in_(5)) new_type = GF_POLLUTE;
				else new_type = GF_POIS;
				break;
			case BIAS_FIRE:
				if(old_type == GF_NUKE || one_in_(4))new_type = GF_NUKE;
				else new_type = GF_FIRE;
				break;
			case BIAS_ACID:
				if(old_type == GF_WATER || one_in_(6)) new_type = GF_WATER;
				else new_type = GF_ACID;
				break;
			case BIAS_COLD:
				if(old_type == GF_ICE || one_in_(4)) new_type = GF_ICE;
				else new_type = GF_COLD;
				break;
			case BIAS_CHAOS:
				if(old_type == GF_DISENCHANT || one_in_(4)) new_type = GF_DISENCHANT;
				else new_type = GF_CHAOS;
				break;
			case BIAS_PRIESTLY:
				if(old_type == GF_PSY_SPEAR || one_in_(7)) new_type = GF_PSY_SPEAR;
				else new_type = GF_LITE;
				break;
			case BIAS_NECROMANTIC:
				if(old_type == GF_DARK || one_in_(5)) new_type = GF_DARK;
				else new_type = GF_NETHER;
				break;
			case BIAS_LAW:
				if(old_type == GF_HOLY_FIRE || one_in_(10)) new_type = GF_HOLY_FIRE;
				else new_type = GF_LITE;
				break;
			case BIAS_ROGUE:
				if(old_type == GF_SHARDS || one_in_(6))new_type = GF_SHARDS;
				else new_type = GF_MISSILE;
				break;
			case BIAS_MAGE:
				if(old_type == GF_METEOR || one_in_(4)) new_type = GF_METEOR;
				else if(old_type == GF_MANA || one_in_(3)) new_type = GF_MANA;
				else new_type = GF_MISSILE;
				break;
			case BIAS_WARRIOR:
				if(old_type == GF_FORCE || one_in_(7)) new_type = GF_FORCE;
				else new_type = GF_ARROW;
				break;
			case BIAS_RANGER:
				if(old_type == GF_DISINTEGRATE || one_in_(9)) new_type = GF_DISINTEGRATE;
				else if(old_type == GF_TORNADO || one_in_(3)) new_type = GF_TORNADO;
				else new_type = GF_ARROW;
				break;
			default:
				//何もしない
				break;
			}

			if(!new_type && object_is_cursed(o_ptr) && weird_luck()) new_type = GF_HELL_FIRE;

			if(new_type)
			{
				o_ptr->xtra1 = new_type;
				//再び判定し、通ったら射撃方法変更
				if(randint0(100) < change_mode_chance)
				{
					int new_mode = 0;
					switch(o_ptr->xtra4)
					{
					case GUN_FIRE_MODE_DEFAULT:
					case GUN_FIRE_MODE_BOLT:
						if(one_in_(4)) new_mode = GUN_FIRE_MODE_BEAM;
						else new_mode = GUN_FIRE_MODE_BALL;
						break;
					case GUN_FIRE_MODE_BALL:
						if(one_in_(3)) new_mode = GUN_FIRE_MODE_ROCKET;
						else new_mode = GUN_FIRE_MODE_BREATH;
						break;
					case GUN_FIRE_MODE_BEAM:
						if(one_in_(3)) new_mode = GUN_FIRE_MODE_SPARK;
						else new_mode = GUN_FIRE_MODE_BREATH;
						break;
					default:
						new_mode = GUN_FIRE_MODE_SPARK;
						break;
					}
					o_ptr->xtra4 = new_mode;
				}
				//new_typeが指定されたらここで終わる。判定が通らなかったり関係ないバイアスでnew_typeが指定されない場合終わらず次へ
				return;
			}
		}

		if(one_in_(3)) //ダイス増加
		{
			do
			{
				int new_dd = o_ptr->dd + randint1(o_ptr->dd / 2 + 1) + m_bonus(5,object_level);
				if(new_dd > 99) new_dd = 99;
				o_ptr->dd = new_dd;
			}while(!randint0(o_ptr->dd));
		}
		else if(one_in_(2)) //ダイス面増加　あまり増えない
		{
			int new_ds = o_ptr->ds + randint1((o_ptr->ds + 3)/4 + 1);
			if(new_ds > 99) new_ds = 99;
			o_ptr->ds = new_ds;
		}
		else //修正値増加 ベースアイテム修正値に準ずる
		{
			int bonus_h, bonus_d;
			int mult_d = 100 + k_info[o_ptr->k_idx].to_d * 10;

			if(mult_d > 500) mult_d = 500;
			bonus_h = m_bonus(7,object_level) + (randint1(7));
			bonus_d = m_bonus(7,object_level) + (randint1(7));

			bonus_d = bonus_d * mult_d / 100 + randint1(bonus_d);

			o_ptr->to_h += bonus_h;
			o_ptr->to_d += bonus_d;

		}


		return;
	}
	else if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
	///tvsv131223
	//if (o_ptr->tval == TV_BOW)
	{
		switch (randint1(7))
		{
			///mod131229 スレイに来る確率を増やしたら少し倍射が出やすすぎる気がするので調整
			case 1:
				random_resistance(o_ptr);
				break;
			case 2:
				random_misc(o_ptr);
				break;
			case 3:
				random_plus(o_ptr);
				///mod 弓の☆でpval=0になることがあるので追加
				if(!o_ptr->pval) o_ptr->pval = randint1(3);
				break;
			case 4:case 5:
				add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
				break;
			default:
				add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
				if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
			break;
		}

		return;
	}

	switch (artifact_bias)
	{
	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_CHAOTIC)))
		{
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		//v2.0.16 武器種別の制限をなくした
		//if((o_ptr->tval == TV_SWORD || o_ptr->tval == TV_POLEARM || o_ptr->tval == TV_KATANA|| o_ptr->tval == TV_AXE || o_ptr->tval == TV_SPEAR ) &&
		 if(  !(have_flag(o_ptr->art_flags, TR_BLESSED)))
		{
			/* A free power for "priestly" random artifacts */
			add_flag(o_ptr->art_flags, TR_BLESSED);
		}
		break;

	case BIAS_NECROMANTIC:
		if (!(have_flag(o_ptr->art_flags, TR_VAMPIRIC)))
		{
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)) && one_in_(2))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_RANGER:
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if ( object_is_melee_weapon(o_ptr) && !(have_flag(o_ptr->art_flags, TR_THROW)))
		{
			/* Free power for rogues... */
			add_flag(o_ptr->art_flags, TR_THROW);
		}
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ACID:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_LAW:
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_EVIL)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_DEMON)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			if (one_in_(2)) return;
		}
		break;
	}

	switch (randint1(40))
	{
		case 1:
		case 2:
			if(have_flag(o_ptr->art_flags, TR_SLAY_EVIL))
				add_flag(o_ptr->art_flags, TR_KILL_EVIL);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_EVIL);

			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_LAW;
			else if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;
		case 3:
		case 4:
			if(have_flag(o_ptr->art_flags, TR_SLAY_GOOD))
				add_flag(o_ptr->art_flags, TR_KILL_GOOD);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_GOOD);

			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_CHAOS;
			else if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 5:
		case 6:
		case 7:
			if(have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL))
				add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			break;
		case 8:
		case 9:
		case 10:
			if(have_flag(o_ptr->art_flags, TR_SLAY_HUMAN))
				add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
			break;
		case 11:
		case 12:
		case 13:
			if(have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD))
				add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;
		case 14:
		case 15:
		case 16:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DRAGON))
				add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
			break;
		case 17:
		case 18:
		case 19:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DEITY))
				add_flag(o_ptr->art_flags, TR_KILL_DEITY);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
			break;
		case 20:
		case 21:
		case 22:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DEMON))
				add_flag(o_ptr->art_flags, TR_KILL_DEMON);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;
		case 23:
		case 24:
		case 25:
			if(have_flag(o_ptr->art_flags, TR_SLAY_KWAI))
				add_flag(o_ptr->art_flags, TR_KILL_KWAI);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
			break;
		case 26:
		case 27:
			if (object_has_a_blade(o_ptr))
			{
				if(have_flag(o_ptr->art_flags, TR_VORPAL))
					add_flag(o_ptr->art_flags, TR_EX_VORPAL);
				else
					add_flag(o_ptr->art_flags, TR_VORPAL);
			}
			else if( o_ptr->tval == TV_SPEAR )
			{
				o_ptr->dd++;
			}
			else
			{
				if(have_flag(o_ptr->art_flags, TR_IMPACT))
					o_ptr->to_d += damroll(2,6);
				else
				add_flag(o_ptr->art_flags, TR_IMPACT);
			}

			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_WARRIOR;
			break;
		case 28:
		case 29:
			add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 30:
		case 31:
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
		case 32:
		case 33:
			add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 34:
		case 35:
			add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			if (!artifact_bias)
				artifact_bias = BIAS_ACID;
			break;
		case 36:
		case 37:
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (!artifact_bias && !one_in_(3))
				artifact_bias = BIAS_POIS;
			else if (!artifact_bias && one_in_(6))
				artifact_bias = BIAS_NECROMANTIC;
			else if (!artifact_bias)
				artifact_bias = BIAS_ROGUE;
			break;
		case 38:
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			if (!artifact_bias)
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 39:
			add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
			if (!artifact_bias)
				artifact_bias = (one_in_(2) ? BIAS_MAGE : BIAS_PRIESTLY);
			break;
		default:
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			if (!artifact_bias)
				artifact_bias = BIAS_CHAOS;
			break;
	}
}

/*:::☆へのランダム発動付加　バイアス影響*/
static void give_activation_power(object_type *o_ptr)
{
	int type = 0, chance = 0;

	switch (artifact_bias)
	{
		case BIAS_ELEC:
			if (!one_in_(3))
			{
				type = ACT_BO_ELEC_1;
			}
			else if (!one_in_(5))
			{
				type = ACT_BA_ELEC_2;
			}
			else
			{
				type = ACT_BA_ELEC_3;
			}
			chance = 101;
			break;

		case BIAS_POIS:
			type = ACT_BA_POIS_1;
			chance = 101;
			break;

		case BIAS_FIRE:
			if (!one_in_(3))
			{
				type = ACT_BO_FIRE_1;
			}
			else if (!one_in_(5))
			{
				type = ACT_BA_FIRE_1;
			}
			else
			{
				type = ACT_BA_FIRE_2;
			}
			chance = 101;
			break;

		case BIAS_COLD:
			chance = 101;
			if (!one_in_(3))
				type = ACT_BO_COLD_1;
			else if (!one_in_(3))
				type = ACT_BA_COLD_1;
			else if (!one_in_(3))
				type = ACT_BA_COLD_2;
			else
				type = ACT_BA_COLD_3;
			break;

		case BIAS_CHAOS:
			chance = 50;
			if (one_in_(6))
				type = ACT_SUMMON_DEMON;
			else
				type = ACT_CALL_CHAOS;
			break;

		case BIAS_PRIESTLY:
			chance = 101;

			if (one_in_(13))
				type = ACT_CHARM_UNDEAD;
			else if (one_in_(12))
				type = ACT_BANISH_EVIL;
			else if (one_in_(11))
				type = ACT_DISP_EVIL;
			else if (one_in_(10))
				type = ACT_PROT_EVIL;
			else if (one_in_(9))
				type = ACT_CURE_1000;
			else if (one_in_(8))
				type = ACT_CURE_700;
			else if (one_in_(7))
				type = ACT_REST_ALL;
			else if (one_in_(6))
				type = ACT_REST_LIFE;
			else if (one_in_(5))
				type = ACT_CURE_500;
			else
				type = ACT_CURE_MW;
			break;

		case BIAS_NECROMANTIC:
			chance = 101;
			if (one_in_(66))
				type = ACT_WRAITH;
			else if (one_in_(13))
				type = ACT_DISP_GOOD;
			else if (one_in_(9))
				type = ACT_MASS_GENO;
			else if (one_in_(8))
				type = ACT_GENOCIDE;
			else if (one_in_(13))
				type = ACT_SUMMON_UNDEAD;
			else if (one_in_(9))
				type = ACT_VAMPIRE_2;
			else if (one_in_(6))
				type = ACT_CHARM_UNDEAD;
			else
				type = ACT_VAMPIRE_1;
			break;

		case BIAS_LAW:
			chance = 101;
			if (one_in_(8))
				type = ACT_BANISH_EVIL;
			else if (one_in_(4))
				type = ACT_DISP_EVIL;
			else
				type = ACT_PROT_EVIL;
			break;

		case BIAS_ROGUE:
			chance = 101;
			if (one_in_(50))
				type = ACT_SPEED;
			else if (one_in_(4))
				type = ACT_SLEEP;
			else if (one_in_(3))
				type = ACT_DETECT_ALL;
			else if (one_in_(8))
				type = ACT_ID_FULL;
			else
				type = ACT_ID_PLAIN;
			break;

		case BIAS_MAGE:
			chance = 66;
			if (one_in_(20))
				type = ACT_SUMMON_ELEMENTAL;
			else if (one_in_(10))
				type = ACT_SUMMON_PHANTOM;
			else if (one_in_(5))
				type = ACT_RUNE_EXPLO;
			else
				type = ACT_ESP;
			break;

		case BIAS_WARRIOR:
			chance = 80;
			if (one_in_(100))
				type = ACT_INVULN;
			else
				type = ACT_BERSERK;
			break;

		case BIAS_RANGER:
			chance = 101;
			if (one_in_(20))
				type = ACT_CHARM_ANIMALS;
			else if (one_in_(7))
				type = ACT_SUMMON_ANIMAL;
			else if (one_in_(6))
				type = ACT_CHARM_ANIMAL;
			else if (one_in_(4))
				type = ACT_RESIST_ALL;
			else if (one_in_(3))
				type = ACT_SATIATE;
			else
				type = ACT_CURE_POISON;
			break;
	}

	///mod160213 低確率かつ武器限定でファイナルストライク追加してみた
	if(object_is_melee_weapon(o_ptr) && one_in_(128))
	{
		type = ACT_FINAL_STRIKE;
		chance = 101;
	}

	if (!type || (randint1(100) >= chance))
	{
		one_activation(o_ptr);
		return;
	}

	/* A type was chosen... */
	o_ptr->xtra2 = type;
	add_flag(o_ptr->art_flags, TR_ACTIVATE);
	o_ptr->timeout = 0;
}

/*:::ランダムアーティファクトの命名*/
///mod160504 引数armorを削除　o_ptrを渡して内部判定
static void get_random_name(char *return_name, object_type *o_ptr, int power)
{
	int prob = randint1(100);

	if (prob <= SINDARIN_NAME)
	{
		get_table_sindarin(return_name, TRUE);
	}
	else if (prob <= TABLE_NAME)
	{
		get_table_name(return_name);
	}
	else
	{
		cptr filename;

		int name_type = 0;

		//v1.1.60 特殊収集アイテムが武器防具銃のどのランダム銘もつくようにした
		if (o_ptr->tval == TV_GUN)
			name_type = 0;
		else if (object_is_armour(o_ptr) || o_ptr->tval == TV_RIBBON || o_ptr->tval == TV_LITE)
			name_type = 1;
		else
			name_type = 2;

		if (o_ptr->tval == TV_ANTIQUE)
			name_type = randint0(3);

		switch (name_type)
		{
		case 0:
			//射撃武器命名
			{
				switch (power)
				{
				case 0:
					filename = _("f_cursed_j.txt", "f_cursed.txt");
					break;
				case 1:
					filename = _("f_low_j.txt", "f_low.txt");
					break;
				case 2:
					filename = _("f_med_j.txt", "f_med.txt");
					break;
				default:
					filename = _("f_high_j.txt", "f_high.txt");
				}
			}
			break;
		case 1:
			//防具類命名
			{
				switch (power)
				{
				case 0:
					filename = _("a_cursed_j.txt", "a_cursed.txt");
					break;
				case 1:
					filename = _("a_low_j.txt", "a_low.txt");
					break;
				case 2:
					filename = _("a_med_j.txt", "a_med.txt");
					break;
				default:
					filename = _("a_high_j.txt", "a_high.txt");
				}
			}
			break;
		default:
			{
				switch (power)
				{
				case 0:
					filename = _("w_cursed_j.txt", "w_cursed.txt");
					break;
				case 1:
					filename = _("w_low_j.txt", "w_low.txt");
					break;
				case 2:
					filename = _("w_med_j.txt", "w_med.txt");
					break;
				default:
					filename = _("w_high_j.txt", "w_high.txt");
				}
			}
			break;
		}
		/*:::artifact_biasを考慮し上で設定したファイルからランダムに一行得る。*/
		/*:::生成でない☆もバイアス名が適用されて名前が付いているが、random_plus()やrandom_resistance()などで確率でartifact_biasが設定されていることによる。*/

		(void)get_rnd_line(filename, artifact_bias, return_name);
#ifdef JP
		 if (return_name[0] == 0) get_table_name(return_name);
#endif
	}
}

/*:::アーティファクト生成 (scroll:巻物使用)*/
//指輪やアミュをapply_magic()を通さずにここに飛ばした場合pvalが0のままになっている。

bool create_artifact(object_type *o_ptr, bool a_scroll)
{
	char    new_name[1024];
	int     has_pval = 0;/*:::boolにしない理由がわからない*/
	int     powers = randint1(5) + 1;
	int     max_type = (object_is_weapon_ammo(o_ptr) ? 8 : 6);
	int     power_level;
	s32b    total_flags;
	bool    a_cursed = FALSE;
	int     warrior_artifact_bias = 0;
	int i;

	if(o_ptr->tval == TV_LITE) max_type = 5;

	/* Reset artifact bias */
	artifact_bias = 0;

	/* Nuke enchantments */
	o_ptr->name1 = 0;
	o_ptr->name2 = 0;

	/*:::ベースアイテムのフラグをo_ptrのart_flagsとして書き込み直している*/
	for (i = 0; i < TR_FLAG_SIZE; i++)
		o_ptr->art_flags[i] |= k_info[o_ptr->k_idx].flags[i];


	if (o_ptr->pval) has_pval = TRUE;

	/*:::巻物使用時には1/4の確率でその職業向きの性能になるらしい 面倒なので消してしまおうか？*/
	/*:::でも戦士向きや魔術師向きのバイアスってのは面白いので簡易に書き直そう　class_info[p_ptr->pclass]から算出するようにすればクラス追加のたびに書き換えなくていい*/
	/// sysdel class
	///pend　アーティファクトバイアス　一時無効化
/*
	if (a_scroll && one_in_(4))
	{
		switch (p_ptr->pclass)
		{
			case CLASS_WARRIOR:
			case CLASS_BERSERKER:
			case CLASS_ARCHER:
			case CLASS_SAMURAI:
			case CLASS_CAVALRY:
			case CLASS_SMITH:
				artifact_bias = BIAS_WARRIOR;
				break;
			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_SORCERER:
			case CLASS_MAGIC_EATER:
			case CLASS_BLUE_MAGE:
				artifact_bias = BIAS_MAGE;
				break;
			case CLASS_PRIEST:
				artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_ROGUE:
			case CLASS_NINJA:
				artifact_bias = BIAS_ROGUE;
				warrior_artifact_bias = 25;
				break;
			case CLASS_RANGER:
			case CLASS_SNIPER:
				artifact_bias = BIAS_RANGER;
				warrior_artifact_bias = 30;
				break;
			case CLASS_PALADIN:
				artifact_bias = BIAS_PRIESTLY;
				warrior_artifact_bias = 40;
				break;
			case CLASS_WARRIOR_MAGE:
			case CLASS_RED_MAGE:
				artifact_bias = BIAS_MAGE;
				warrior_artifact_bias = 40;
				break;
			case CLASS_CHAOS_WARRIOR:
			//case CLASS_MAID:
				artifact_bias = BIAS_CHAOS;
				warrior_artifact_bias = 40;
				break;
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
				artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_MINDCRAFTER:
			case CLASS_BARD:
				if (randint1(5) > 2) artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_TOURIST:
				if (randint1(5) > 2) artifact_bias = BIAS_WARRIOR;
				break;
			case CLASS_IMITATOR:
				if (randint1(2) > 1) artifact_bias = BIAS_RANGER;
				break;
			case CLASS_BEASTMASTER:
				artifact_bias = BIAS_CHR;
				warrior_artifact_bias = 50;
				break;
			case CLASS_MIRROR_MASTER:
				if (randint1(4) > 1)
				{
				    artifact_bias = BIAS_MAGE;
				}
				else
				{
				    artifact_bias = BIAS_ROGUE;
				}
				break;
		}
	}
	if (a_scroll && (randint1(100) <= warrior_artifact_bias))
		artifact_bias = BIAS_WARRIOR;
*/

	strcpy(new_name, "");

	if (!a_scroll && one_in_(A_CURSED))/*:::1/13*/
		a_cursed = TRUE;
	if (((o_ptr->tval == TV_AMULET) || (o_ptr->tval == TV_RING)) && object_is_cursed(o_ptr))
		a_cursed = TRUE;

	while (one_in_(powers) || one_in_(7) || weird_luck())
		powers++;

	///mod131229 深層ほど良い☆を出やすくしてみる
	powers += m_bonus(4,object_level);

	if (!a_cursed && weird_luck())
		powers *= 2;

	//v1.1.60
	if (o_ptr->tval == TV_ANTIQUE) powers += randint1(5) + randint0(o_ptr->sval);


	if (a_cursed) powers /= 2;

	if(powers > 100) powers = 100;

	/*:::powersは初期2-6,低確率で10を超えるくらいか*/
	/*:::テストしてみたら平均5 95%が10未満で20越えは10000回に数度*/
	/*:::色々いじくった結果超級の☆が少し出やすくなった。*/
	/* Main loop */
	while (powers--)
	{
		switch (randint1(max_type))
		{
			/*:::pval能力アップフラグ　隠密や魔道具もある*/
			case 1: case 2:
				if (object_is_ammo(o_ptr))
				{
					; //何もせずcase3,4へ
				}
				else
				{
					random_plus(o_ptr);
					has_pval = TRUE;
					break;
				}
			/*:::近接武器のときダイスブーストのチャンス　そうでないとき耐性を得る*/
			//v2.0.16 矢などもダイスブーストする
			case 3: case 4:

				if (one_in_(2) && object_is_melee_weapon(o_ptr) || object_is_ammo(o_ptr))
				{
					if (a_cursed && !one_in_(13)) break;
					if (one_in_(13))
					{
						if (one_in_(o_ptr->ds+4)) o_ptr->ds++;
					}
					else
					{
						if (one_in_(o_ptr->dd+1)) o_ptr->dd++;
					}
				}
				else
					random_resistance(o_ptr);
				break;
			/*:::能力付加　維持やテレパスなど*/
			case 5:
				//矢玉は低確率でダメージブースト
				//random_misc()でのブースト処理のみを雑に反映したもの
				if (object_is_ammo(o_ptr))
				{
					if (one_in_(7))
					{
						o_ptr->to_h += 4 + randint1(11);
						o_ptr->to_d += 4 + randint1(11);
					}
				}
				else
				{
					random_misc(o_ptr);
				}
				break;
				///mod131229 武器にスレイが付きやすく、防具に能力が付きやすくした
			case 6:
				if(object_is_weapon_ammo(o_ptr)) random_slay(o_ptr);
				else random_misc(o_ptr);
				break;
			case 7: case 8:
			/*:::スレイ　追加射撃や吸血攻撃などもここ*/
				random_slay(o_ptr);
				break;
			default:
				if (p_ptr->wizard) msg_print("Switch error in create_artifact!");
				powers++;
		}
	};

	//銃パラメータ補正
	if(o_ptr->tval == TV_GUN)
	{
		int tmp_dd;
		int need_turn = gun_base_param_table[o_ptr->sval].charge_turn;
		int base_dd = k_info[o_ptr->k_idx].dd;
		int base_ds = k_info[o_ptr->k_idx].ds;
		int base_to_d = k_info[o_ptr->k_idx].to_d;
		int base_to_h = k_info[o_ptr->k_idx].to_h;

		//あまり威力が上がりすぎないようある程度制限をかける必要がある。
		if(o_ptr->dd > 99) o_ptr->dd = 99;
		if(o_ptr->dd > (need_turn * 3 + 24) && !weird_luck()) o_ptr->dd = (need_turn * 3 + 24);
		if(o_ptr->dd > (base_dd * 2 + 8) && !weird_luck()) o_ptr->dd = base_dd * 2 + 8 + randint0(o_ptr->dd - base_dd * 2 - 8);

		if(o_ptr->ds > 99) o_ptr->ds = 99;
		if(o_ptr->ds > base_ds * 2 && !weird_luck()) o_ptr->ds = base_ds * 2;

		if(o_ptr->to_d > 999) o_ptr->to_d = 999;
		if(o_ptr->to_d > 30 && o_ptr->to_d > base_to_d * 3 && !weird_luck()) o_ptr->to_d = base_to_d * 3 + randint0(o_ptr->to_d - base_to_d * 3);

		if(o_ptr->to_h > 50) o_ptr->to_h = 50;
		if(o_ptr->to_h > 30 && o_ptr->to_h > base_to_h * 3 && !weird_luck()) o_ptr->to_h = base_to_h * 3 + randint0(o_ptr->to_h - base_to_h * 3);

		//属性によってダイスに補正
		tmp_dd = o_ptr->dd;
		switch(o_ptr->xtra1)
		{
		//元素系は強くなる
		case GF_FIRE: case GF_ELEC: case GF_ACID:case GF_COLD:case GF_POIS:
		case GF_NUKE: case GF_PLASMA:
			tmp_dd += (tmp_dd) + randint1(tmp_dd);
			break;
		case GF_METEOR: case GF_CHAOS: case GF_NETHER: case GF_POLLUTE:
			tmp_dd += (tmp_dd+2)/3 + randint0(tmp_dd/3);
			break;
		//上位属性
		case GF_LITE: case GF_DARK: case GF_SHARDS: case GF_MANA: case GF_DISENCHANT:
		case GF_TORNADO:
			tmp_dd += (tmp_dd+3)/4 + randint0(tmp_dd/4);
			break;
		//水や轟音など有用なものは変化なし
		default:
			break;
		}
		if(tmp_dd > 255) tmp_dd = 255;
		o_ptr->dd = tmp_dd;

		//射撃がARROWでないとき魔法分類をボルトにする
		if(o_ptr->xtra1 != GF_ARROW && o_ptr->xtra4 == GUN_FIRE_MODE_DEFAULT)
			o_ptr->xtra4 = GUN_FIRE_MODE_BOLT;

		//射撃がボルトでなくなったとき属性をMISSILEにする
		if(o_ptr->xtra1 == GF_ARROW && o_ptr->xtra4 != GUN_FIRE_MODE_DEFAULT)
			o_ptr->xtra1 = GF_MISSILE;

	}


	if (has_pval)
	{
#if 0
		add_flag(o_ptr->art_flags, TR_SHOW_MODS);

		/* This one commented out by gw's request... */
		if (!a_scroll)
			add_flag(o_ptr->art_flags, TR_HIDE_TYPE);
#endif
		/*:::追加攻撃のときはpvalを減らされる*/
		if (have_flag(o_ptr->art_flags, TR_BLOWS))
		{
			if(o_ptr->pval > 2) o_ptr->pval = 2;
			else if(o_ptr->pval > 1 && one_in_(2)) o_ptr->pval = 2;
			else o_ptr->pval = randint1(2);
			///mod131223
			//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_HAYABUSA))
			if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_HAYABUSA))
				o_ptr->pval++;
		}
		else
		{
			bool flag_no_limit = FALSE;
			//スピ指の☆生成処理変えた。高pvalのスピ指はpvalが維持される代わりに他のpvalフラグが消える、もしくはpval4になる
			if (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SPEED)
			{
				if(have_flag(o_ptr->art_flags, TR_STR) || have_flag(o_ptr->art_flags, TR_INT) ||
				   have_flag(o_ptr->art_flags, TR_WIS) || have_flag(o_ptr->art_flags, TR_DEX) ||
				   have_flag(o_ptr->art_flags, TR_CON) || have_flag(o_ptr->art_flags, TR_CHR) ||
				   have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY) || have_flag(o_ptr->art_flags, TR_STEALTH))
				{
					if(randint1(o_ptr->pval) > 4)
					{
						remove_flag(o_ptr->art_flags,TR_STR);
						remove_flag(o_ptr->art_flags,TR_INT);
						remove_flag(o_ptr->art_flags,TR_WIS);
						remove_flag(o_ptr->art_flags,TR_DEX);
						remove_flag(o_ptr->art_flags,TR_CON);
						remove_flag(o_ptr->art_flags,TR_CHR);
						remove_flag(o_ptr->art_flags,TR_MAGIC_MASTERY);
						remove_flag(o_ptr->art_flags,TR_STEALTH);
						flag_no_limit = TRUE;
					}
					else
					{
						if(o_ptr->pval > 4) o_ptr->pval = 4;
					}
				}
				else
				{
						flag_no_limit = TRUE;
				}
			}
			//賢者アミュの生成規則も変更　探索や赤外線以外のpvalがついてるとpvalを4に
			else if(o_ptr->tval == TV_AMULET && o_ptr->sval == SV_AMULET_THE_MAGI)
			{
				if(have_flag(o_ptr->art_flags, TR_STR) || have_flag(o_ptr->art_flags, TR_INT) ||
				   have_flag(o_ptr->art_flags, TR_WIS) || have_flag(o_ptr->art_flags, TR_DEX) ||
				   have_flag(o_ptr->art_flags, TR_CON) || have_flag(o_ptr->art_flags, TR_CHR) ||
				   have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY) || have_flag(o_ptr->art_flags, TR_STEALTH) ||
				   have_flag(o_ptr->art_flags, TR_SPEED)  )
				{
					if(o_ptr->pval > 4) o_ptr->pval = 4;
				}
				else
				{
					flag_no_limit = TRUE;
				}
			}
			//他の装備品でpvalが4を超えているときは4にする。腕力の指輪など
			else
			{
				if(o_ptr->pval > 4) o_ptr->pval = 4;
			}

			do
			{
				o_ptr->pval++;
			}
			while (o_ptr->pval < randint1(5) || one_in_(o_ptr->pval));

			if ((o_ptr->pval > 4) && !(weird_luck()) && !flag_no_limit) o_ptr->pval = 4;
		}

	}

	/* give it some plusses... */
	/*:::20以下の修正値は20まで上がるチャンスがある*/
	if (object_is_armour(o_ptr))
		o_ptr->to_a += randint1(o_ptr->to_a > 19 ? 1 : 20 - o_ptr->to_a);
	else if (object_is_weapon_ammo(o_ptr))
	{
		o_ptr->to_h += randint1(o_ptr->to_h > 19 ? 1 : 20 - o_ptr->to_h);
		o_ptr->to_d += randint1(o_ptr->to_d > 19 ? 1 : 20 - o_ptr->to_d);
		if ((have_flag(o_ptr->art_flags, TR_WIS)) && (o_ptr->pval > 0)) add_flag(o_ptr->art_flags, TR_BLESSED);
	}

	/* Just to be sure */
	/*:::元素から保護*/
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

	/*:::つけられたフラグの価値を数値化*/
	total_flags = flag_cost(o_ptr, o_ptr->pval,FALSE);
	if (cheat_peek) msg_format("%ld", total_flags);

	if (a_cursed) curse_artifact(o_ptr);


	/*:::発動能力付加　武器1/3,防具1/6　矢にもつくのか？*/
	///mod160504 銃にはつかないようにする
	//v2.0.16 矢にもつかないようにする
	if (!a_cursed && o_ptr->tval != TV_GUN && !object_is_ammo(o_ptr) &&
	    one_in_(object_is_armour(o_ptr) ? ACTIVATION_CHANCE * 2 : ACTIVATION_CHANCE))
	{
		o_ptr->xtra2 = 0;
		give_activation_power(o_ptr);
	}

	/*:::防具への殺戮修正付加・・・？不等号が逆のような気がするが*/
	/*:::と思ってよく見たら付きすぎた殺戮修正を減少させる処理だった。*/
	if (object_is_armour(o_ptr))
	{
		while ((o_ptr->to_d+o_ptr->to_h) > 20)
		{
			if (one_in_(o_ptr->to_d) && one_in_(o_ptr->to_h)) break;
			o_ptr->to_d -= (s16b)randint0(3);
			o_ptr->to_h -= (s16b)randint0(3);
		}
		while ((o_ptr->to_d+o_ptr->to_h) > 10)
		{
			if (one_in_(o_ptr->to_d) || one_in_(o_ptr->to_h)) break;
			o_ptr->to_d -= (s16b)randint0(3);
			o_ptr->to_h -= (s16b)randint0(3);
		}
	}

	if (((artifact_bias == BIAS_MAGE) || (artifact_bias == BIAS_INT)) && (o_ptr->tval == TV_GLOVES)) add_flag(o_ptr->art_flags, TR_FREE_ACT);

	/*:::毒針の多くのフラグを消去*/
	///mod131223 dokubari
	//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI))
	if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI))

	{
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		remove_flag(o_ptr->art_flags, TR_BLOWS);
		remove_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
		remove_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
		remove_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		remove_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		remove_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		//remove_flag(o_ptr->art_flags, TR_SLAY_ORC);
		//remove_flag(o_ptr->art_flags, TR_SLAY_TROLL);
		//remove_flag(o_ptr->art_flags, TR_SLAY_GIANT);
		remove_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		remove_flag(o_ptr->art_flags, TR_KILL_DRAGON);
		remove_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		remove_flag(o_ptr->art_flags, TR_VORPAL);
		remove_flag(o_ptr->art_flags, TR_BRAND_POIS);
		remove_flag(o_ptr->art_flags, TR_BRAND_ACID);
		remove_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		remove_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		remove_flag(o_ptr->art_flags, TR_BRAND_COLD);
	}

	//v2.1.0 瑞霊はアイテムの呪いを無効化 呪いフラグはobject_flags()で無視するのでここではpvalなどのマイナスを0にする
	if (p_ptr->pclass == CLASS_MIZUCHI && object_is_equipment(o_ptr))
	{
		if (o_ptr->to_a < 0) o_ptr->to_a = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->pval < 0) o_ptr->pval = 0;
	}



	/*:::power_levelはランダム命名のランクに使われる*/
	if (!object_is_weapon_ammo(o_ptr))
	{
		/* For armors */
		if (a_cursed) power_level = 0;
		else if (total_flags < 15000) power_level = 1;
		else if (total_flags < 35000) power_level = 2;
		else power_level = 3;
	}
	else if (o_ptr->tval == TV_GUN)
	{
		/* For armors */
		if (a_cursed) power_level = 0;
		else if (total_flags < 25000) power_level = 1;
		else if (total_flags < 60000) power_level = 2;
		else power_level = 3;
	}
	else
	{
		/* For weapons */
		if (a_cursed) power_level = 0;
		else if (total_flags < 20000) power_level = 1;
		else if (total_flags < 45000) power_level = 2;
		else power_level = 3;
	}

	if (a_scroll)
	{
		char dummy_name[80] = "";
#ifdef JP
		cptr ask_msg = "このアーティファクトを何と名付けますか？";
#else
		cptr ask_msg = "What do you want to call the artifact? ";
#endif

		/* Identify it fully */
		/*:::生成☆は*鑑定*済み　*/
		object_aware(o_ptr);
		object_known(o_ptr);

		/* Mark the item as fully known */
		o_ptr->ident |= (IDENT_MENTAL);

		/* For being treated as random artifact in screen_object() */
		o_ptr->art_name = quark_add("");

		(void)screen_object(o_ptr, 0L);

		if (!get_string(ask_msg, dummy_name, sizeof dummy_name)
		    || !dummy_name[0])
		{
			/* Cancelled */
			if (one_in_(2))
			{
				get_table_sindarin_aux(dummy_name);
			}
			else
			{
				get_table_name_aux(dummy_name);
			}
		}

#ifdef JP
		sprintf(new_name, "《%s》", dummy_name);
#else
		sprintf(new_name, "'%s'", dummy_name);
#endif
		/*:::☆生成をしたら一部の徳が増加　削除予定*/
		///del131223 virtue
		//chg_virtue(V_INDIVIDUALISM, 2);
		//chg_virtue(V_ENCHANT, 5);
	}
	else
	{
		/*:::シンダリン、テーブル以外のランダムアーティファクト名を決定*/
		get_random_name(new_name, o_ptr, power_level);
	}

	if (cheat_xtra)
	{
#ifdef JP
		if (artifact_bias) msg_format("運の偏ったアーティファクト: %d。", artifact_bias);
		else msg_print("アーティファクトに運の偏りなし。");
#else
		if (artifact_bias) msg_format("Biased artifact: %d.", artifact_bias);
		else msg_print("No bias in artifact.");
#endif
	}

	/* Save the inscription */
	o_ptr->art_name = quark_add(new_name);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	//v1.1.25 入手時レベルとターンを記録
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;


	return TRUE;
}


int activation_index(object_type *o_ptr)
/*:::アイテムの発動インデックス(activation_info[]の添え字)を取得する*/
/*:::魔法の笛とモンスターボールはここでインデックスが返らない*/
{
	/* Give priority to weaponsmith's essential activations */
	/*:::鍛冶師用追加発動があるとき*/
	if (object_is_smith(o_ptr))
	{
		switch (o_ptr->xtra3-1)
		{
		case ESSENCE_TMP_RES_ACID: return ACT_RESIST_ACID;
		case ESSENCE_TMP_RES_ELEC: return ACT_RESIST_ELEC;
		case ESSENCE_TMP_RES_FIRE: return ACT_RESIST_FIRE;
		case ESSENCE_TMP_RES_COLD: return ACT_RESIST_COLD;
		case TR_IMPACT: return ACT_QUAKE;

			//v1.1.74
		case ESSENCE_TMP_RES_ALL: return ACT_RESIST_ALL;
		case ESSENCE_WHIRL_WIND: return ACT_WHIRLWIND;
		case ESSENCE_PSY_SPEAR: return ACT_PSY_SPEAR;
		case ESSENCE_L_RANGE_ATTACK: return ACT_LONG_RANGE_ATTACK;
		case ESSENCE_HIT_AND_AWAY: return ACT_HIT_AND_AWAY;
		case ESSENCE_HEAL_500: return ACT_CURE_500;
		case ESSENCE_INVULN: return ACT_INVULN;
		case ESSENCE_SS_ARROW: return ACT_SAINT_STAR_ARROW;
		case ESSENCE_SUPER_STEALTH: return ACT_TIM_SUPER_STEALTH;
		case ESSENCE_DIM_DOOR: return ACT_DIM_DOOR;
		case ESSENCE_DUAL_CAST: return ACT_DUAL_CAST;
		case ESSENCE_TIM_INC_DICE: return ACT_TIM_ADD_DICES;

		}
	}

	/*:::固定アーティファクトはa_infoに発動インデックスが格納済み*/
	if (object_is_fixed_artifact(o_ptr))
	{
		if (have_flag(a_info[o_ptr->name1].flags, TR_ACTIVATE))
		{
			return a_info[o_ptr->name1].act_idx;
		}
	}
	if (object_is_ego(o_ptr))
	{
		if (have_flag(e_info[o_ptr->name2].flags, TR_ACTIVATE))
		{
			return e_info[o_ptr->name2].act_idx;
		}
	}
	/*:::火炎の指輪など通常アイテム？*/
	if (!object_is_random_artifact(o_ptr))
	{
		if (have_flag(k_info[o_ptr->k_idx].flags, TR_ACTIVATE))
		{
			return k_info[o_ptr->k_idx].act_idx;
		}
	}
	/*:::ランダムアーティファクト*/
	return o_ptr->xtra2;
}
/*:::発動の名前、難易度などの情報を得る。activation_info[]の中から一致するものを一つ返す。*/
const activation_type* find_activation_info(object_type *o_ptr)
{
	/*:::k_info,a_infoなどに書かれた発動タグ SUNLIGHTなど*/
	const int index = activation_index(o_ptr);

	const activation_type* p;

	for (p = activation_info; p->flag != NULL; ++ p) {
		if (p->index == index)
		{
			return p;
		}
	}

	return NULL;
}


/* Dragon breath activation */
/*:::ドラゴン鎧の発動効果*/
static bool activate_dragon_breath(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE]; /* for resistance flags */
	int type[20];
	cptr name[20];
	int i, dir, t, n = 0;

	if (!get_aim_dir(&dir)) return FALSE;

	object_flags(o_ptr, flgs);

	for (i = 0; dragonbreath_info[i].flag != 0; i++)
	{
		if (have_flag(flgs, dragonbreath_info[i].flag))
		{
			type[n] = dragonbreath_info[i].type;
			name[n] = dragonbreath_info[i].name;
			n++;
		}
	}

	/* Paranoia */
	if (n == 0) return FALSE;

	/* Stop speaking */
	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();

	t = randint0(n);
	msg_format(_("あなたは%sのブレスを吐いた。", "You breathe %s."), name[t]);
	fire_ball(type[t], dir, 250, -4);

	return TRUE;
}


//v1.1.85 アーティファクト発動によるモンスター捕獲
//do_cmd_activate_aux()に記述されたTV_CAPTUREの発動と処理は同じだがパラメータが保存される場所が違う
//xtra6:モンスターidx
//xtra7:モンスター加速値
//xtra4:モンスターHP
//xtra5:モンスターMHP
static bool activate_artifact_capture(object_type *o_ptr)
{

	//r_idxが保管されていないとき捕獲処理
	if(!o_ptr->xtra6)
	{
		int dir;
		bool old_target_pet = target_pet;
		target_pet = TRUE;
		if (!get_aim_dir(&dir))
		{
			target_pet = old_target_pet;
			return TRUE;
		}
		target_pet = old_target_pet;

		if (fire_ball(GF_CAPTURE, dir, 0, 0))
		{
			/*:::モンスターのパラメータをモンスターボールに格納　専用グローバル変数使用*/
			o_ptr->xtra6 = cap_mon;
			o_ptr->xtra7 = cap_mspeed;
			o_ptr->xtra4 = cap_hp;
			o_ptr->xtra5 = cap_maxhp;
			if (cap_nickname)
			{
				cptr t;
				char *s;
				char buf[160] = "";

				if (o_ptr->inscription)
					strcpy(buf, quark_str(o_ptr->inscription));
				s = buf;
#ifdef JP
				for (s = buf; *s && (*s != '#'); s++)
				{
					if (iskanji(*s)) s++;
				}
#endif
				*s = '#';
				s++;

				t = quark_str(cap_nickname);
				while (*t)
				{
					*s = *t;
					s++;
					t++;
				}

				*s = '\0';
				o_ptr->inscription = quark_add(buf);
			}
		}


	}
	else
	{
		int dir;
		bool release_success = FALSE;
		//v1.1.16 妖魔本パラメータ不正に対応
		if (o_ptr->xtra6 < 0 || o_ptr->xtra6 >= max_r_idx)
		{
#ifdef JP
			msg_format("ERROR:発動捕獲アイテムのpval値がおかしい(%d)", o_ptr->xtra6);
#else
            msg_format("ERROR: Pval of the activated capture item is incorrect (%d) ", o_ptr->xtra6);
#endif
			return FALSE;
		}

		if (!get_rep_dir2(&dir)) return TRUE;

		if (monster_can_enter(py + ddy[dir], px + ddx[dir], &r_info[o_ptr->xtra6], 0))
		{
			if (place_monster_aux(0, py + ddy[dir], px + ddx[dir], o_ptr->xtra6, (PM_FORCE_PET | PM_NO_KAGE)))
			{
				if (o_ptr->xtra7) m_list[hack_m_idx_ii].mspeed = o_ptr->xtra7;
				if (o_ptr->xtra5) m_list[hack_m_idx_ii].max_maxhp = o_ptr->xtra5;
				if (o_ptr->xtra4) m_list[hack_m_idx_ii].hp = o_ptr->xtra4;
				m_list[hack_m_idx_ii].maxhp = m_list[hack_m_idx_ii].max_maxhp;
				if (o_ptr->inscription)
				{
					char buf[160];
					cptr t;

					t = quark_str(o_ptr->inscription);
#ifdef JP
					for (t = quark_str(o_ptr->inscription); *t && (*t != '#'); t++)
					{
						if (iskanji(*t)) t++;
					}
#endif
					if (*t)
					{
						char *s = buf;
						t++;

						while (*t)
						{
							*s = *t;
							t++;
							s++;
						}
						*s = '\0';
						m_list[hack_m_idx_ii].nickname = quark_add(buf);
						t = quark_str(o_ptr->inscription);
						s = buf;
						while (*t && (*t != '#'))
						{
							*s = *t;
							t++;
							s++;
						}
						*s = '\0';
						o_ptr->inscription = quark_add(buf);
					}
				}
				o_ptr->xtra6 = 0;
				o_ptr->xtra7 = 0;
				o_ptr->xtra4 = 0;
				o_ptr->xtra5 = 0;
				release_success = TRUE;
			}
		}
		if (!release_success)
#ifdef JP
			msg_print("おっと、解放に失敗した。");
#else
            msg_print("Whoops, release failed.");
#endif
	}


	return TRUE;


}



//v1.1.85 アーティファクト「プロテウスリング」でのランダム変身用
static bool mon_hook_random_metamorphose(int r_idx)
{

	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->level > p_ptr->lev + 10 || r_ptr->level < p_ptr->lev - 10) return FALSE;

	if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2 || r_ptr->flags7 & RF7_VARIABLE) return FALSE;

	if (r_ptr->rarity >= 50) return FALSE;

	return TRUE;


}



/*:::アイテム発動効果　別にランダムアーティファクト専用ではないらしい*/
///item class
///mod160213 ファイナルストライク用にitemを取得
bool activate_random_artifact(object_type *o_ptr, int item)
{
	int plev = p_ptr->lev;
	int k, dir, dummy = 0;
	///mod160122 k_nameのベース名だと指輪などの発動で「○○の指輪」と表示されないので修正
	//cptr name = k_name + k_info[o_ptr->k_idx].name;

	char name[80];
	const activation_type* const act_ptr = find_activation_info(o_ptr);

	/* Paranoia */
	if (!act_ptr) {
		/* Maybe forgot adding information to activation_info table ? */
		msg_print("Activation information is not found.");
		return FALSE;
	}

#ifdef JP
	object_desc(name,o_ptr,OD_NAME_ONLY|OD_NO_PLURAL);
#else
    object_desc(name,o_ptr,OD_NAME_ONLY|OD_NO_PLURAL|OD_OMIT_PREFIX);
#endif

	/* Activate for attack */
	switch (act_ptr->index)
	{
		case ACT_SUNLIGHT:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("太陽光線が放たれた。");
#else
			msg_print("A line of sunlight appears.");
#endif
			(void)lite_line(dir, damroll(6, 8));
			break;
		}

		case ACT_BO_MISS_1:
		{
#ifdef JP
			msg_print("それは眩しいくらいに明るく輝いている...");
#else
			msg_print("It glows extremely brightly...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_MISSILE, dir, damroll(2, 6));
			break;
		}

		case ACT_BA_POIS_1:
		{
#ifdef JP
			msg_print("それは濃緑色に脈動している...");
#else
			msg_print("It throbs deep green...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_POIS, dir, 12, 3);
			break;
		}

		case ACT_BO_ELEC_1:
		{
#ifdef JP
			msg_print("それは火花に覆われた...");
#else
			msg_print("It is covered in sparks...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ELEC, dir, damroll(4, 8));
			break;
		}

		case ACT_BO_ACID_1:
		{
#ifdef JP
			msg_print("それは酸に覆われた...");
#else
			msg_print("It is covered in acid...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ACID, dir, damroll(5, 8));
			break;
		}

		case ACT_BO_COLD_1:
		{
#ifdef JP
			msg_print("それは霜に覆われた...");
#else
			msg_print("It is covered in frost...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_COLD, dir, damroll(6, 8));
			break;
		}

		case ACT_BO_FIRE_1:
		{
#ifdef JP
			msg_print("それは炎に覆われた...");
#else
			msg_print("It is covered in fire...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_FIRE, dir, damroll(9, 8));
			break;
		}

		case ACT_BA_COLD_1:
		{
#ifdef JP
			msg_print("それは霜に覆われた...");
#else
			msg_print("It is covered in frost...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 48, 2);
			break;
		}

		case ACT_BA_FIRE_1:
		{
#ifdef JP
			msg_print("それは赤く激しく輝いた...");
#else
			msg_print("It glows an intense red...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 72, 2);
			break;
		}

		case ACT_DRAIN_1:
		{
#ifdef JP
			///msg131214
			//msg_format("あなたは%sに敵を締め殺すよう命じた。", name);
			msg_format("あなたは%sに敵を締め付けるよう命じた。", name);

#else
			msg_format("You order the %s to choke your opponent.", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			if (drain_life(dir, 100))
			break;
		}

		case ACT_BA_COLD_2:
		{
#ifdef JP
			msg_print("それは青く激しく輝いた...");
#else
			msg_print("It glows an intense blue...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 100, 2);
			break;
		}

		case ACT_BA_ELEC_2:
		{
#ifdef JP
			msg_print("電気がパチパチ音を立てた...");
#else
			msg_print("It crackles with electricity...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ELEC, dir, 100, 3);
			break;
		}

		case ACT_BA_FIRE_2:
		{
#ifdef JP
			msg_format("%sから炎が吹き出した...", name);
#else
			msg_format("The %s rages in fire...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 120, 3);
			break;
		}

		case ACT_DRAIN_2:
		{
#ifdef JP
			msg_print("黒く輝いている...");
#else
			msg_print("It glows black...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			drain_life(dir, 120);
			break;
		}

		case ACT_VAMPIRE_1:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if (drain_life(dir, 50))
				hp_player(50);
			}
			break;
		}

		case ACT_BO_MISS_2:
		{
#ifdef JP
			msg_print("魔法のトゲが現れた...");
#else
			msg_print("It grows magical spikes...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ARROW, dir, 150);
			break;
		}

		case ACT_BA_FIRE_3:
		{
#ifdef JP
			msg_print("深赤色に輝いている...");
#else
			msg_print("It glows deep red...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 300, 3);
			break;
		}

		case ACT_BA_COLD_3:
		{
#ifdef JP
			msg_print("明るく白色に輝いている...");
#else
			msg_print("It glows bright white...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 400, 3);
			break;
		}

		case ACT_BA_ELEC_3:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ELEC, dir, 500, 3);
			break;
		}

		case ACT_WHIRLWIND:
		{
			//v1.1.33 別関数に分離
			whirlwind_attack(0);
			break;
		}

		case ACT_VAMPIRE_2:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if (drain_life(dir, 100))
				hp_player(100);
			}
			break;
		}


		case ACT_CALL_CHAOS:
		{
#ifdef JP
			msg_print("様々な色の火花を発している...");
#else
			msg_print("It glows in scintillating colours...");
#endif
			call_chaos();
			break;
		}

		case ACT_ROCKET:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("ロケットを発射した！");
#else
			msg_print("You launch a rocket!");
#endif
			fire_ball(GF_ROCKET, dir, 250 + plev*3, 2);
			break;
		}

		case ACT_DISP_EVIL:
		{
#ifdef JP
			msg_print("神聖な雰囲気が充満した...");
#else
			msg_print("It floods the area with goodness...");
#endif
			dispel_evil(p_ptr->lev * 5);
			break;
		}

		case ACT_BA_MISS_3:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("あなたはエレメントのブレスを吐いた。");
#else
			msg_print("You breathe the elements.");
#endif
			fire_ball(GF_MISSILE, dir, 300, -4);
			break;
		}

		case ACT_DISP_GOOD:
		{
#ifdef JP
			msg_print("邪悪な雰囲気が充満した...");
#else
			msg_print("It floods the area with evil...");
#endif
			dispel_good(p_ptr->lev * 5);
			break;
		}

		case ACT_BO_MANA:
		{
#ifdef JP
			msg_format("%sに魔法のトゲが現れた...", name);
#else
			msg_format("The %s grows magical spikes...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ARROW, dir, 150);
			break;
		}

		case ACT_BA_WATER:
		{
#ifdef JP
			msg_format("%sが深い青色に鼓動している...", name);
#else
			msg_format("The %s throbs deep blue...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_WATER, dir, 200, 3);
			break;
		}

		case ACT_BA_DARK:
		{
#ifdef JP
			msg_format("%sが深い闇に覆われた...", name);
#else
			msg_format("The %s is coverd in pitch-darkness...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_DARK, dir, 250, 4);
			break;
		}

		case ACT_BA_MANA:
		{
#ifdef JP
			msg_format("%sが青白く光った．．．", name);
#else
			msg_format("The %s glows pale...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_MANA, dir, 250, 4);
			break;
		}

		case ACT_PESTICIDE:
		{
#ifdef JP
			msg_print("あなたは害虫を一掃した。");
#else
			msg_print("You exterminate small life.");
#endif
			(void)dispel_monsters(4);
			break;
		}

		case ACT_BLINDING_LIGHT:
		{
#ifdef JP
			msg_format("%sが眩しい光で輝いた...", name);
#else
			msg_format("The %s gleams with blinding light...", name);
#endif
			fire_ball(GF_LITE, 0, 300, 6);
			confuse_monsters(3 * p_ptr->lev / 2);
			break;
		}

		case ACT_BIZARRE:
		{
#ifdef JP
			msg_format("%sは漆黒に輝いた...", name);
#else
			msg_format("The %s glows intensely black...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			ring_of_power(dir);
			break;
		}

		case ACT_CAST_BA_STAR:
		{
			int num = damroll(5, 3);
			int y, x;
			int attempts;
#ifdef JP
			msg_format("%sが稲妻で覆われた...", name);
#else
			msg_format("The %s is surrounded by lightning...", name);
#endif
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(&y, &x, py, px, 4, 0);

					if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

					if (!player_bold(y, x)) break;
				}

				project(0, 3, y, x, 150, GF_ELEC,
							(PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}

			break;
		}

		case ACT_BLADETURNER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("あなたはエレメントのブレスを吐いた。");
#else
			msg_print("You breathe the elements.");
#endif
			fire_ball(GF_MISSILE, dir, 300, -4);
#ifdef JP
			msg_print("鎧が様々な色に輝いた...");
#else
			msg_print("Your armor glows many colours...");
#endif
			(void)set_afraid(0);
			(void)set_hero(randint1(50) + 50, FALSE);
			(void)hp_player(10);
			(void)set_blessed(randint1(50) + 50, FALSE);
			(void)set_oppose_acid(randint1(50) + 50, FALSE);
			(void)set_oppose_elec(randint1(50) + 50, FALSE);
			(void)set_oppose_fire(randint1(50) + 50, FALSE);
			(void)set_oppose_cold(randint1(50) + 50, FALSE);
			(void)set_oppose_pois(randint1(50) + 50, FALSE);
			break;
		}
		case ACT_BA_ACID_1:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ACID, dir, 100, 2);
			break;
		}

		case ACT_BR_FIRE:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 200, -2);
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			{
				(void)set_oppose_fire(randint1(20) + 20, FALSE);
			}
			break;
		}
		case ACT_BR_COLD:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 200, -2);
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			{
				(void)set_oppose_cold(randint1(20) + 20, FALSE);
			}
			break;
		}
		case ACT_BR_DRAGON:
		{
			if (!activate_dragon_breath(o_ptr)) return FALSE;
			break;
		}

		/* Activate for other offensive action */

		case ACT_CONFUSE:
		{
#ifdef JP
			msg_print("様々な色の火花を発している...");
#else
			msg_print("It glows in scintillating colours...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			confuse_monster(dir, 20);
			break;
		}

		case ACT_SLEEP:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif
			sleep_monsters_touch();
			break;
		}

		case ACT_QUAKE:
		{
			earthquake(py, px, 5);
			break;
		}

		case ACT_TERROR:
		{
			turn_monsters(40 + p_ptr->lev);
			break;
		}

		case ACT_TELE_AWAY:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)fire_beam(GF_AWAY_ALL, dir, plev);
			break;
		}

		case ACT_BANISH_EVIL:
		{
			if (banish_evil(100))
			{
#ifdef JP
				msg_print("アーティファクトの力が邪悪を打ち払った！");
#else
				msg_print("The power of the artifact banishes evil!");
#endif
			}
			break;
		}

		case ACT_GENOCIDE:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif
			(void)symbol_genocide(200, TRUE,0);
			break;
		}

		case ACT_MASS_GENO:
		{
#ifdef JP
			msg_print("ひどく鋭い音が流れ出た...");
#else
			msg_print("It lets out a long, shrill note...");
#endif
			(void)mass_genocide(200, TRUE);
			break;
		}

		case ACT_SCARE_AREA:
		{
			if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
			if (hex_spelling_any()) stop_hex_spell_all();
#ifdef JP
			msg_print("あなたは力強い突風を吹き鳴らした。周囲の敵が震え上っている!");
#else
			msg_print("You wind a mighty blast; your enemies tremble!");
#endif
			(void)turn_monsters((3 * p_ptr->lev / 2) + 10);
			break;
		}

		case ACT_AGGRAVATE:
		{
			if (o_ptr->name1 == ART_HYOUSIGI)
			{
#ifdef JP
				msg_print("拍子木を打った。");
#else
				msg_print("You beat Your wooden clappers.");
#endif
			}
			else
			{
#ifdef JP
				msg_format("%sは不快な物音を立てた。", name);
#else
				msg_format("The %s sounds an unpleasant noise.", name);
#endif
			}
			aggravate_monsters(0,FALSE);
			break;
		}

		/* Activate for summoning / charming */

		case ACT_CHARM_ANIMAL:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)charm_animal(dir, plev * 2);
			break;
		}

		case ACT_CHARM_UNDEAD:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)control_one_undead(dir, plev * 2);
			break;
		}

		case ACT_CHARM_OTHER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)charm_monster(dir, plev * 2);
			break;
		}

		case ACT_CHARM_ANIMALS:
		{
			(void)charm_animals(plev * 2);
			break;
		}

		case ACT_CHARM_OTHERS:
		{
			charm_monsters(plev * 2);
			break;
		}

		case ACT_SUMMON_ANIMAL:
		{
			(void)summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_PHANTOM:
		{
#ifdef JP
			msg_print("幻霊を召喚した。");
#else
			msg_print("You summon a phantasmal servant.");
#endif
			(void)summon_specific(-1, py, px, dun_level, SUMMON_PHANTOM, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_ELEMENTAL:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), SUMMON_ELEMENTAL, mode))
			{
#ifdef JP
				msg_print("エレメンタルが現れた...");
#else
				msg_print("An elemental materializes...");
#endif
				if (pet)
#ifdef JP
					msg_print("あなたに服従しているようだ。");
#else
					msg_print("It seems obedient to you.");
#endif
				else
#ifdef JP
					msg_print("それをコントロールできなかった！");
#else
					msg_print("You fail to control it!");
#endif
			}

			break;
		}

		case ACT_SUMMON_DEMON:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), SUMMON_DEMON, mode))
			{
#ifdef JP
				msg_print("硫黄の悪臭が充満した。");
#else
				msg_print("The area fills with a stench of sulphur and brimstone.");
#endif
				if (pet)
#ifdef JP
					msg_print("「ご用でございますか、ご主人様」");
#else
					msg_print("'What is thy bidding... Master?'");
#endif
				else
#ifdef JP
					msg_print("「NON SERVIAM! Wretch! お前の魂を頂くぞ！」");
#else
					msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
			}

			break;
		}

		case ACT_SUMMON_UNDEAD:
		{
			bool pet = one_in_(3);
			int type;
			u32b mode = 0L;

			type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

			if (!pet || ((plev > 24) && one_in_(3))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), type, mode))
			{
#ifdef JP
				msg_print("冷たい風があなたの周りに吹き始めた。それは腐敗臭を運んでいる...");
#else
				msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif
				if (pet)
#ifdef JP
					msg_print("古えの死せる者共があなたに仕えるため土から甦った！");
#else
					msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
				else
#ifdef JP
					msg_print("死者が甦った。眠りを妨げるあなたを罰するために！");
#else
					msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
			}

			break;
		}

		case ACT_SUMMON_HOUND:
		{
			u32b mode = PM_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((p_ptr->lev * 3) / 2), SUMMON_HOUND, mode))
			{

				if (pet)
#ifdef JP
					msg_print("ハウンドがあなたの下僕として出現した。");
#else
				msg_print("A group of hounds appear as your servant.");
#endif

				else
#ifdef JP
					msg_print("ハウンドはあなたに牙を向けている！");
#else
					msg_print("A group of hounds appear as your enemy!");
#endif

			}

			break;
		}

		case ACT_SUMMON_DAWN:
		{
#ifdef JP
			msg_print("暁の師団を召喚した。");
#else
			msg_print("You summon the Legion of the Dawn.");
#endif
			(void)summon_specific(-1, py, px, dun_level, SUMMON_DAWN, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_OCTOPUS:
		{
			u32b mode = PM_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if (pet) mode |= PM_FORCE_PET;

			if (summon_named_creature(0, py, px, MON_JIZOTAKO, mode))
			{
				if (pet)
#ifdef JP
					msg_print("蛸があなたの下僕として出現した。");
#else
					msg_print("A group of octopuses appear as your servant.");
#endif
				else
#ifdef JP
					msg_print("蛸はあなたを睨んでいる！");
#else
					msg_print("A group of octopuses appear as your enemy!");
#endif
			}

			break;
		}

		case ACT_SUMMON_MONSTERS:
		{
			int i;
			int summon_num = (plev+5) / 6;
			u32b mode = PM_ALLOW_GROUP | PM_FORCE_PET;
			bool flag_success = FALSE;

			for (i = 0; i < summon_num; i++)
			{
				if (summon_specific(-1, py, px, (plev + dun_level) / 2, SUMMON_ALL, (PM_ALLOW_GROUP | PM_FORCE_PET))) flag_success = TRUE;

			}
			if(flag_success)
			{
				msg_print(_("輝く杖の下に配下たちが馳せ参じた。", "Your followers run in beneath the shining staff."));
			}


			break;
		}


		/* Activate for healing */

		case ACT_CHOIR_SINGS:
		{
#ifdef JP
			msg_print("天国の歌が聞こえる...");
#else
			msg_print("A heavenly choir sings...");
#endif
			(void)set_poisoned(0);
			(void)set_cut(0);
			(void)set_stun(0);
			(void)set_confused(0);
			(void)set_blind(0);
			(void)set_afraid(0);
			(void)set_hero(randint1(25) + 25, FALSE);
			(void)hp_player(777);
			break;
		}

		case ACT_CURE_LW:
		{
			(void)set_afraid(0);
			(void)hp_player(30);
			break;
		}

		case ACT_CURE_MW:
		{
#ifdef JP
			msg_print("深紫色の光を発している...");
#else
			msg_print("It radiates deep purple...");
#endif
			hp_player(damroll(4, 8));
			(void)set_cut((p_ptr->cut / 2) - 50);
			break;
		}

		case ACT_CURE_POISON:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
#else
			msg_print("It glows deep blue...");
#endif
			(void)set_afraid(0);
			(void)set_poisoned(0);
			break;
		}

		case ACT_REST_LIFE:
		{
#ifdef JP
			msg_print("深紅に輝いている...");
#else
			msg_print("It glows a deep red...");
#endif
			restore_level();
			break;
		}

		case ACT_REST_ALL:
		{
#ifdef JP
			msg_print("濃緑色に輝いている...");
#else
			msg_print("It glows a deep green...");
#endif
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_CHR);
			(void)restore_level();
			break;
		}

		case ACT_CURE_700:
		{
#ifdef JP
			msg_print("深青色に輝いている...");
			msg_print("体内に暖かい鼓動が感じられる...");
#else
			msg_print("It glows deep blue...");
			msg_print("You feel a warm tingling inside...");
#endif
			(void)hp_player(700);
			(void)set_cut(0);
			break;
		}
		//v1.1.74
		case ACT_CURE_500:
		{
#ifdef JP
			msg_print("青色に輝いている...");
#else
            msg_print("It glows blue...");
#endif
			(void)hp_player(500);
			(void)set_cut(0);
			break;
		}
		case ACT_CURE_1000:
		{
#ifdef JP
			msg_print("白く明るく輝いている...");
			msg_print("ひじょうに気分がよい...");
#else
			msg_print("It glows a bright white...");
			msg_print("You feel much better...");
#endif
			(void)hp_player(1000);
			(void)set_cut(0);
			break;
		}

		case ACT_CURING:
		{
#ifdef JP
			msg_format("%sの優しさに癒される...", name);
#else
			msg_format("%s cures you affectionately ...", name);
#endif
			(void)set_poisoned(0);
			(void)set_confused(0);
			(void)set_blind(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)set_image(0);

			break;
		}

		case ACT_CURE_MANA_FULL:
		{
#ifdef JP
			msg_format("%sが青白く光った．．．", name);
#else
			msg_format("The %s glows pale...", name);
#endif
			///class 魔道具術士のMP回復
			if (p_ptr->pclass == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT*2; i++)
				{
					p_ptr->magic_num1[i] += (p_ptr->magic_num2[i] < 10) ? EATER_CHARGE * 3 : p_ptr->magic_num2[i]*EATER_CHARGE/3;
					if (p_ptr->magic_num1[i] > p_ptr->magic_num2[i]*EATER_CHARGE) p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT*3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i-EATER_EXT*2);
					p_ptr->magic_num1[i] -= ((p_ptr->magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : p_ptr->magic_num2[i]*EATER_ROD_CHARGE/3)*k_info[k_idx].pval;
					if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				p_ptr->window |= (PW_PLAYER);
			}
			else if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
			}
			break;
		}

		/* Activate for timed effect */

		case ACT_ESP:
		{
			(void)set_tim_esp(randint1(30) + 25, FALSE);
			break;
		}

		case ACT_BERSERK:
		{
			if(o_ptr->name2 == EGO_HEAD_BEAST)
				msg_format(_("ワオーーーーーン！", "Hoooooooooowl!"), name);

			(void)set_afraid(0);
			(void)set_shero(randint1(25) + 25, FALSE);
			/* (void)set_afraid(0);
			(void)set_hero(randint1(50) + 50, FALSE);
			(void)set_blessed(randint1(50) + 50, FALSE);
			o_ptr->timeout = 100 + randint1(100); */
			break;
		}

		case ACT_PROT_EVIL:
		{
#ifdef JP
			msg_format("%sから鋭い音が流れ出た...", name);
#else
			msg_format("The %s lets out a shrill wail...", name);
#endif
			k = 3 * p_ptr->lev;
			(void)set_protevil(randint1(25) + k, FALSE);
			break;
		}

		case ACT_RESIST_ALL:
		{
#ifdef JP
			if (o_ptr->name1 == ART_IKKYU)
				msg_print("全てを受け入れる境地に至った気がする...");
			else
				msg_print("様々な色に輝いている...");
#else
			if (o_ptr->name1 == ART_IKKYU)
				msg_print("You feel as if you've reached the state when you can accept everything...");
			else
				msg_print("It glows many colours...");
#endif
			(void)set_oppose_acid(randint1(40) + 40, FALSE);
			(void)set_oppose_elec(randint1(40) + 40, FALSE);
			(void)set_oppose_fire(randint1(40) + 40, FALSE);
			(void)set_oppose_cold(randint1(40) + 40, FALSE);
			(void)set_oppose_pois(randint1(40) + 40, FALSE);
			break;
		}

		case ACT_SPEED:
		{
#ifdef JP
			msg_print("明るく緑色に輝いている...");
#else
			msg_print("It glows bright green...");
#endif
			(void)set_fast(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_XTRA_SPEED:
		{
#ifdef JP
			msg_print("明るく輝いている...");
#else
			msg_print("It glows brightly...");
#endif
			(void)set_fast(randint1(75) + 75, FALSE);
			break;
		}

		case ACT_WRAITH:
		{
			set_wraith_form(randint1(plev / 2) + (plev / 2), FALSE);
			break;
		}

		case ACT_INVULN:
		{
			(void)set_invuln(randint1(8) + 8, FALSE);
			break;
		}

		case ACT_HELO:
		{
			(void)set_afraid(0);
			set_hero(randint1(25)+25, FALSE);
			hp_player(10);
			break;
		}

		case ACT_HELO_SPEED:
		{
			(void)set_fast(randint1(50) + 50, FALSE);
			hp_player(10);
			set_afraid(0);
			set_hero(randint1(50) + 50, FALSE);
			break;
		}
		///item 火炎の指輪などの特殊発動
		case ACT_RESIST_ACID:
		{
#ifdef JP
			msg_format("%sが黒く輝いた...", name);
#else
			msg_format("The %s glows black.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ACID))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_ACID, dir, 100, 2);
			}
			(void)set_oppose_acid(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_FIRE:
		{
			//v1.1.31 村雨追加
			if(o_ptr->name1 == ART_MURASAME)
				msg_format(_("%sから水が噴き出した！", "Water spouts from %s!"), name);
			else
				msg_format(_("%sが赤く輝いた...", "The %s glows red..."), name);

			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_FIRE, dir, 100, 2);
			}

			(void)set_oppose_fire(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_COLD:
		{
#ifdef JP
			msg_format("%sが白く輝いた...", name);
#else
			msg_format("The %s glows white.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_COLD, dir, 100, 2);
			}
			(void)set_oppose_cold(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_ELEC:
		{
#ifdef JP
			msg_format("%sが青く輝いた...", name);
#else
			msg_format("The %s glows blue.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ELEC))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_ELEC, dir, 100, 2);
			}
			(void)set_oppose_elec(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_POIS:
		{
#ifdef JP
			msg_format("%sが緑に輝いた...", name);
#else
			msg_format("The %s glows green.", name);
#endif
			(void)set_oppose_pois(randint1(20) + 20, FALSE);
			break;
		}

		/* Activate for general purpose effect (detection etc.) */

		case ACT_LIGHT:
		{
#ifdef JP
			msg_format("%sから澄んだ光があふれ出た...", name);
#else
			msg_format("The %s wells with clear light...", name);
#endif
			lite_area(damroll(2, 15), 3);
			break;
		}

		case ACT_MAP_LIGHT:
		{
#ifdef JP
			msg_print("眩しく輝いた...");
#else
			msg_print("It shines brightly...");
#endif
			map_area(DETECT_RAD_MAP);
			lite_area(damroll(2, 15), 3);
			break;
		}

		case ACT_DETECT_ALL:
		{
#ifdef JP
			msg_print("白く明るく輝いている...");
			msg_print("心にイメージが浮かんできた...");
#else
			msg_print("It glows bright white...");
			msg_print("An image forms in your mind...");
#endif
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}

		case ACT_DETECT_XTRA:
		{
#ifdef JP
			msg_print("明るく輝いている...");
#else
			msg_print("It glows brightly...");
#endif
			detect_all(DETECT_RAD_DEFAULT);
			probing();
			identify_fully(FALSE);
			break;
		}

		case ACT_ID_FULL:
		{
#ifdef JP
			msg_print("黄色く輝いている...");
#else
			msg_print("It glows yellow...");
#endif
			identify_fully(FALSE);
			break;
		}

		case ACT_ID_PLAIN:
		{
			if (!ident_spell(FALSE)) return FALSE;
			break;
		}

		case ACT_RUNE_EXPLO:
		{
			if(p_ptr->csp < 20)
			{
#ifdef JP
				msg_format("罠を仕掛ける力が残っていない・・");
#else
                msg_format("You don't have energy left to set traps...");
#endif
				break;
			}
#ifdef JP
			msg_format("あなたの足元にルーンが輝いた。");
#else
            msg_format("Runes start glowing at your feet.");
#endif
			explosive_rune();
			p_ptr->csp -= 20;
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();
			break;
		}

		case ACT_RUNE_PROT:
		{
#ifdef JP
			msg_print("ブルーに明るく輝いている...");
#else
			msg_print("It glows light blue...");
#endif
			warding_glyph();
			break;
		}

		case ACT_SATIATE:
		{
			(void)set_food(PY_FOOD_MAX - 1);
			break;
		}

		case ACT_DEST_DOOR:
		{
#ifdef JP
			msg_print("明るい赤色に輝いている...");
#else
			msg_print("It glows bright red...");
#endif
			destroy_doors_touch();
			break;
		}

		case ACT_STONE_MUD:
		{
#ifdef JP
			msg_print("鼓動している...");
#else
			msg_print("It pulsates...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			wall_to_mud(dir, 20 + randint1(30));
			break;
		}

		case ACT_RECHARGE:
		{
			recharge(130);
			break;
		}

		case ACT_ALCHEMY:
		{
#ifdef JP
			msg_print("明るい黄色に輝いている...");
#else
			msg_print("It glows bright yellow...");
#endif
			(void)alchemy(0);
			break;
		}

		case ACT_DIM_DOOR:
		{
#ifdef JP
			msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
			msg_print("You open a dimensional gate. Choose a destination.");
#endif
			if (!dimension_door(D_DOOR_NORMAL)) return FALSE;
			break;
		}


		case ACT_TELEPORT:
		{
#ifdef JP
			msg_print("周りの空間が歪んでいる...");
#else
			msg_print("It twists space around you...");
#endif
			teleport_player(100, 0L);
			break;
		}

		case ACT_RECALL:
		{
#ifdef JP
			msg_print("やわらかな白色に輝いている...");
#else
			msg_print("It glows soft white...");
#endif
			if (!word_of_recall()) return FALSE;
			break;
		}

		case ACT_JUDGE:
		{
#ifdef JP
			msg_format("%sは赤く明るく光った！", name);
#else
			msg_format("The %s flashes bright red!", name);
#endif
		///del131214 virtue
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 1);
			wiz_lite(FALSE);
#ifdef JP
			msg_format("%sはあなたの体力を奪った...", name);
			take_hit(DAMAGE_LOSELIFE, damroll(3,8), "審判の宝石", -1);
#else
			msg_format("The %s drains your vitality...", name);
			take_hit(DAMAGE_LOSELIFE, damroll(3, 8), "the Jewel of Judgement", -1);
#endif
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
#ifdef JP
			if (get_check("帰還の力を使いますか？"))
#else
			if (get_check("Activate recall? "))
#endif
			{
				(void)word_of_recall();
			}

			break;
		}

		case ACT_TELEKINESIS:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_format("%sを伸ばした。", name);
#else
			msg_format("You stretched your %s.", name);
#endif
			fetch(dir, 500, TRUE);
			break;
		}

		case ACT_DETECT_UNIQUE:
		{
			int i;
			monster_type *m_ptr;
			monster_race *r_ptr;
#ifdef JP
			msg_print("奇妙な場所が頭の中に浮かんだ．．．");
#else
			msg_print("Some strange places show up in your mind. And you see ...");
#endif
			/* Process the monsters (backwards) */
			for (i = m_max - 1; i >= 1; i--)
			{
				/* Access the monster */
				m_ptr = &m_list[i];

				/* Ignore "dead" monsters */
				if (!m_ptr->r_idx) continue;

				r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags1 & RF1_UNIQUE)
				{
#ifdef JP
					msg_format("%s． ",r_name + r_ptr->name);
#else
					msg_format("%s. ",r_name + r_ptr->name);
#endif
				}
			}
			break;
		}

		case ACT_ESCAPE:
		{
			switch (randint1(13))
			{
			case 1: case 2: case 3: case 4: case 5:
				teleport_player(10, 0L);
				break;
			case 6: case 7: case 8: case 9: case 10:
				teleport_player(222, 0L);
				break;
			case 11: case 12:
				(void)stair_creation();
				break;
			default:
#ifdef JP
				if (get_check("この階を去りますか？"))
#else
				if (get_check("Leave this level? "))
#endif
				{
					if (autosave_l) do_cmd_save_game(TRUE);

					//即時フロア再構成。クエストダンジョンまで再構成されるなどかなり奇妙な挙動をする。
					/* Leaving */
					p_ptr->leaving = TRUE;
				}
			}
			break;
		}

		case ACT_DISP_CURSE_XTRA:
		{
#ifdef JP
			msg_format("%sが真実を照らし出す...", name);
#else
			msg_format("The %s exhibits the truth...", name);
#endif
			if (remove_all_curse())
			{
#ifdef JP
				msg_print("誰かに見守られているような気がする。");
#else
				msg_print("You feel as if someone is watching over you.");
#endif
			}
			(void)probing();
			break;
		}

		case ACT_BRAND_FIRE_BOLTS:
		{
#ifdef JP
			msg_format("%sが深紅に輝いた...", name);
#else
			msg_format("Your %s glows deep red...", name);
#endif
			(void)brand_bolts();
			break;
		}

		case ACT_RECHARGE_XTRA:
		{
#ifdef JP
			msg_format("%sが白く輝いた．．．", name);
#else
			msg_format("The %s gleams with blinding light...", name);
#endif
			if (!recharge(1000)) return FALSE;
			break;
		}

		case ACT_LORE:
		{
#ifdef JP
			msg_print("石が隠された秘密を写し出した．．．");
#else
			msg_print("The stone reveals hidden mysteries...");
#endif
			if (!ident_spell(FALSE)) return FALSE;

			//if (mp_ptr->spell_book)
			//if(p_ptr->msp > 20)
			{
				/* Sufficient mana */
				if (20 <= p_ptr->csp)
				{
					/* Use some mana */
					p_ptr->csp -= 20;
				}

				/* Over-exert the player */
				else
				{
					int oops = 20 - p_ptr->csp;

					/* No mana left */
					p_ptr->csp = 0;
					p_ptr->csp_frac = 0;

					//v1.1.56 麻痺でなくsanity_blastで済ませることにした
					if (randint1(100) < oops)
					{
#ifdef JP
						msg_print("石を制御できない！");
#else
						msg_print("You are unable to control the stone!");
#endif
						sanity_blast(0, TRUE);
					}
				}

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);
			}
#ifdef JP
			take_hit(DAMAGE_LOSELIFE, damroll(1, 12), "危険な秘密", -1);
#else
			take_hit(DAMAGE_LOSELIFE, damroll(1, 12), "perilous secrets", -1);
#endif
			/* Confusing. */
			if (one_in_(5)) (void)set_confused(p_ptr->confused +
				randint1(10));

			/* Exercise a little care... */
			if (one_in_(20))
#ifdef JP
				take_hit(DAMAGE_LOSELIFE, damroll(4, 10), "危険な秘密", -1);
#else
				take_hit(DAMAGE_LOSELIFE, damroll(4, 10), "perilous secrets", -1);
#endif
			break;
		}

///del131223
#if 0
		case ACT_SHIKOFUMI:
		{
#ifdef JP
			msg_print("力強く四股を踏んだ。");
#else
			msg_print("You stamp. (as if you are in a ring.)");
#endif
			(void)set_afraid(0);
			(void)set_hero(randint1(20) + 20, FALSE);
			dispel_evil(p_ptr->lev * 3);
			break;
		}
#endif
		case ACT_PHASE_DOOR:
		{
			teleport_player(10, 0L);
			break;
		}

		case ACT_DETECT_ALL_MONS:
		{
			(void)detect_monsters_invis(255);
			(void)detect_monsters_normal(255);
			break;
		}

		case ACT_ULTIMATE_RESIST:
		{
			int v = randint1(25)+25;
			(void)set_afraid(0);
			(void)set_hero(v, FALSE);
			(void)hp_player(10);
			(void)set_blessed(v, FALSE);
			(void)set_oppose_acid(v, FALSE);
			(void)set_oppose_elec(v, FALSE);
			(void)set_oppose_fire(v, FALSE);
			(void)set_oppose_cold(v, FALSE);
			(void)set_oppose_pois(v, FALSE);
			(void)set_ultimate_res(v, FALSE);
			break;
		}
///del131214 ブロンズクロス発動
#if 0
		/* Unique activation */
		///class 練気術
		case ACT_CAST_OFF:
		{
			int inv, o_idx, t;
			char o_name[MAX_NLEN];
			object_type forge;

			/* Cast off activated item */
			for (inv = INVEN_RARM; inv <= INVEN_FEET; inv++)
			{
				if (o_ptr == &inventory[inv]) break;
			}

			/* Paranoia */
			if (inv > INVEN_FEET) return FALSE;

			object_copy(&forge, o_ptr);
			inven_item_increase(inv, (0 - o_ptr->number));
			inven_item_optimize(inv);
			o_idx = drop_near(&forge, 0, py, px);
			o_ptr = &o_list[o_idx];

			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			msg_format(_("%sを脱ぎ捨てた。", "You cast off %s."), o_name);

			/* Get effects */
			msg_print(_("「燃え上がれ俺の小宇宙！」", "You say, 'Burn up my cosmo!"));
			t = 20 + randint1(20);
			(void)set_blind(p_ptr->blind + t);
			(void)set_afraid(0);
			(void)set_tim_esp(p_ptr->tim_esp + t, FALSE);
			(void)set_tim_regen(p_ptr->tim_regen + t, FALSE);
			(void)set_hero(p_ptr->hero + t, FALSE);
			(void)set_blessed(p_ptr->blessed + t, FALSE);
			(void)set_fast(p_ptr->fast + t, FALSE);
			(void)set_shero(p_ptr->shero + t, FALSE);
			if (p_ptr->pclass == CLASS_FORCETRAINER)
			{
				p_ptr->magic_num1[0] = plev * 5 + 190;
				msg_print(_("気が爆発寸前になった。", "Your force are immediatly before explosion."));
			}

			break;
		}
#endif
		case ACT_FISHING:
		{
			int x, y;

			if (!get_rep_dir2(&dir)) return FALSE;
			y = py+ddy[dir];
			x = px+ddx[dir];
			tsuri_dir = dir;
			if (!cave_have_flag_bold(y, x, FF_WATER))
			{
#ifdef JP
				msg_print("そこは水辺ではない。");
#else
				msg_print("There is no fishing place.");
#endif
				return FALSE;
			}
			else if (cave[y][x].m_idx)
			{
				char m_name[80];
				monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
				msg_format("%sが邪魔だ！", m_name);
#else
				msg_format("%^s is stand in your way.", m_name);
#endif
				energy_use = 0;
				return FALSE;
			}
			set_action(ACTION_FISH);
			p_ptr->redraw |= (PR_STATE);
			break;
		}

///del131224 印籠発動
#if 0
		case ACT_INROU:
		{
			int count = 0, i;
			monster_type *m_ptr;
#ifndef JP
			cptr kakusan = "";
#endif
			if (summon_named_creature(0, py, px, MON_SUKE, PM_FORCE_PET))
			{
#ifdef JP
				msg_print("『助さん』が現れた。");
#else
				msg_print("Suke-san apperars.");
				kakusan = "Suke-san";
#endif
				count++;
			}
			if (summon_named_creature(0, py, px, MON_KAKU, PM_FORCE_PET))
			{
#ifdef JP
				msg_print("『格さん』が現れた。");
#else
				msg_print("Kaku-san appears.");
				kakusan = "Kaku-san";
#endif
				count++;
			}
			if (!count)
			{
				for (i = m_max - 1; i > 0; i--)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					if (!((m_ptr->r_idx == MON_SUKE) || (m_ptr->r_idx == MON_KAKU))) continue;
					if (!los(m_ptr->fy, m_ptr->fx, py, px)) continue;
					if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
					count++;
					break;
				}
			}

			if (count)
			{
#ifdef JP
				msg_print("「者ども、ひかえおろう！！！このお方をどなたとこころえる。」");
#else
				msg_format("%^s says 'WHO do you think this person is! Bow your head, down your knees!'", kakusan);
#endif
				sukekaku = TRUE;
				stun_monsters(120);
				confuse_monsters(120);
				turn_monsters(120);
				stasis_monsters(120);
				sukekaku = FALSE;
			}
			else
			{
#ifdef JP
				msg_print("しかし、何も起きなかった。");
#else
				msg_print("Nothing happen.");
#endif
			}
			break;
		}
#endif
		///item 村正発動　もっとほかの効果に変えようか？
		case ACT_MURAMASA:
		{
			/* Only for Muramasa */
			if (o_ptr->name1 != ART_MURAMASA) return FALSE;
#ifdef JP
			if (get_check("本当に使いますか？"))
#else
			if (get_check("Are you sure?!"))
#endif
			{
#ifdef JP
				msg_print("村正が震えた．．．");
#else
				msg_print("The Muramasa pulsates...");
#endif
				do_inc_stat(A_STR);
				if (one_in_(2))
				{
#ifdef JP
					msg_print("村正は壊れた！");
#else
					msg_print("The Muramasa is destroyed!");
#endif
					curse_weapon_object(TRUE, o_ptr);
				}
			}
			break;
		}

		case ACT_BLOODY_MOON:
		{
			/* Only for Bloody Moon */
			if (o_ptr->name1 != ART_BLOOD) return FALSE;
#ifdef JP
			msg_print("鎌が明るく輝いた...");
#else
			msg_print("Your scythe glows brightly!");
#endif
			get_bloody_moon_flags(o_ptr);
			if (p_ptr->prace == RACE_ANDROID) calc_android_exp();
			p_ptr->update |= (PU_BONUS | PU_HP);
			break;
		}
		case ACT_CRIMSON:
		{
			int num = 1;
			int i;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int tx, ty;

			/* Only for Crimson */
			if (o_ptr->name1 != ART_CRIMSON) return FALSE;

#ifdef JP
				msg_print("せっかくだから『クリムゾン』をぶっぱなすぜ！");
#else
			msg_print("I'll fire CRIMSON! SEKKAKUDAKARA!");
#endif

			if (!get_aim_dir(&dir)) return FALSE;

			/* Use the given direction */
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			///class アーチャーのクリムゾン発動
			if (p_ptr->pclass == CLASS_ARCHER || p_ptr->pclass == CLASS_KOGASA)
			{
				/* Extra shot at level 10 */
				if (p_ptr->lev >= 10) num++;

				/* Extra shot at level 30 */
				if (p_ptr->lev >= 30) num++;

				/* Extra shot at level 45 */
				if (p_ptr->lev >= 45) num++;
			}

			for (i = 0; i < num; i++)
				project(0, p_ptr->lev/20+1, ty, tx, p_ptr->lev*p_ptr->lev*6/50, GF_ROCKET, flg, -1);
			break;
		}
		case ACT_NERUNERUNERUNE:
			msg_print(NULL);
			msg_print(_("練れば練るほど…色が変わって…美味い!!!",
                        "You knead more and more... its color changes... Delicious!!!"));
			msg_print(NULL);
			if(p_ptr->pclass == CLASS_KOGASA || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_HINA)
				msg_print(_("…しかしあなたの腹は膨れなかった。",
                            "...But it doesn't provide you sustenance."));
			else
				(void)set_food(PY_FOOD_MAX - 1);
			if (!p_ptr->resist_chaos && one_in_(3))	(void)set_image(p_ptr->image + randint0(50) + 50);
			break;

		case ACT_ALTER_REALITY:
			msg_print(_("ランプが別世界の景色を映し出す…",
                        "The lantern reflects scenery of another world..."));
			alter_reality();
			break;


		case ACT_KOGASA_LASER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("傘からレーザーが放たれた！");
#endif
			fire_beam(GF_LITE, dir, p_ptr->lev + damroll(p_ptr->lev / 3, 10));
			break;
		}
		/*:::小傘がトリガーハッピーのときのみ発動可　アーチャークリムゾンと同じ仕様*/
		case ACT_KOGASA_ROCKET:
		{
			int num = 1;
			int i;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int tx, ty;

			if (o_ptr->tval != TV_STICK || o_ptr->sval != SV_WEAPON_KOGASA) return FALSE;
			if (!get_aim_dir(&dir)) return FALSE;

			/* Use the given direction */
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}

			if (p_ptr->lev >= 10) num++;
			if (p_ptr->lev >= 30) num++;
			if (p_ptr->lev >= 45) num++;
			msg_format(_("傘から%d発のロケット弾が放たれた！",
                        "You fire %d rocket(s) from your umbrella!"), num);

			for (i = 0; i < num; i++)
				project(0, p_ptr->lev/20+1, ty, tx, p_ptr->lev*p_ptr->lev*6/50, GF_ROCKET, flg, -1);
			break;
		}

		case ACT_HYPER_BERSERK:
		{
			int dam = randint1(200);
			if (dam <= p_ptr->csp)
			{
			p_ptr->csp -= dam;
			}
			else
			{
				msg_format(_("あなたの体は狂戦士化の反動に悲鳴を上げた！",
                            "You let out a scream as you receive backlash from going berserk!"), name);
				dam -= p_ptr->csp;
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				take_hit(DAMAGE_LOSELIFE,dam,_("狂戦士化の反動", "backlash from going berserk"),-1);
			}
			msg_format(_("黒い獣のような激情があなたの意識を塗り潰した・・",
                        "Your mind is flooded with black, bestial, violent emotions..."), name);
			(void)set_afraid(0);
			(void)set_shero(randint1(125) + 125, FALSE);
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();
			break;
		}


		case ACT_DRINK:
		{
			//v1.1.49 処理忘れてたので追加
			if (p_ptr->pclass == CLASS_BYAKUREN)
			{
				msg_print(_("あなたは戒律により酒を飲めない。",
                            "Your religion prohibits you from consuming alcohol."));
				break;
			}


			///mod151108 警告追加
			if(!(p_ptr->muta2 & MUT2_ALCOHOL)
				&& (p_ptr->alcohol + calc_alcohol_mod(6000,TRUE)) >= DRANK_4
				&& !process_warning2() ) return FALSE;

			msg_format(_("あなたは瓢箪から酒を飲んだ。",
                        "You drink sake from the gourd."));
			set_alcohol(p_ptr->alcohol + calc_alcohol_mod(6000,FALSE));
			set_food(p_ptr->food + 200);
			break;
		}
		case ACT_ROBERTA:
		{
			msg_format(_("「では皆様、御機嫌よう。」",
                        "'Farewell, everyone; stay in good health.'"));

			project_hack2(GF_SHARDS,0,0,200);
			teleport_player(50,TELEPORT_NONMAGICAL);
			break;
		}
		case ACT_WALTER:
		{
			project_hack2(GF_MISSILE,0,0,200);
			msg_format(_("「失敬!!」", "'Disrespect!!'"));
			break;
		}
		case ACT_DETECT_ITEM:
		{
			msg_format(_("あなたはダウジングを始めた・・",
                        "You start dowsing..."));
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}

		case ACT_SANAGI:
		{
			msg_format(_("あなたは鉄鐸を楽しげにガラガラと鳴らした！",
                        "You happily rattle the iron!"));
			(void)set_afraid(0);
			(void)set_hero(randint1(25) + 25, FALSE);
			aggravate_monsters(0,FALSE);
			break;
		}
		case ACT_DER_FREISCHUTZ:
		{
			//別ルーチンを通ることにしたのでここには来ないはず
			msg_format(_("ERROR:魔弾の射手がactivate_random_artifact経由で呼ばれた",
                        "ERROR: Freischutz called in activate_random_artifact"));
			return FALSE;

		}
		break;

		case ACT_GOKASHITIKIN:
		{
			if(p_ptr->csp < 40)
			{
				msg_format(_("宝具を扱う力が残っていない・・",
                            "You don't have enough power to handle the treasured item."));
				break;
			}
			msg_format(_("あなたは扇を大きく振り、衝撃波を放った！",
                        "You swing the fan, releasing a shockwave!"));
			project_hack2(GF_SOUND,0,0,150);
			p_ptr->csp -= 40;
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();

			break;
		}
		case ACT_SUMMON_FOUNTAIN:
		{
			msg_format(_("地面に杖をつくと、泉が湧き出した！",
                        "Water springs forth as you slam the staff on the ground!"));
			project(0,2,py,px,3,GF_WATER_FLOW,PROJECT_GRID,-1);
			break;
		}
		case ACT_TORNADO:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_TORNADO, dir, 200, 3,_("竜巻を呼び起こした！",
                                                        "You call forth a tornado!"))) return FALSE;

			break;
		}
		case ACT_TRAPEZOHEDRON:
		{
			wiz_lite(TRUE);

			if(one_in_(6))
			{
				u32b mode = PM_ALLOW_UNIQUE;
				msg_print(_("何かが隣に佇んでいる。",
                            "Someone's standing next to you."));
				if(!summon_named_creature(0,py,px,MON_NYARLATHOTEP,mode))
				{
					int num = randint1(3);
					bool flag = FALSE;
					for(;num>0;num--)
					{
						if(summon_named_creature(0,py,px,MON_LESSER_O_G,mode)) flag = TRUE;
					}
					if(!flag) msg_print(_("・・ような気がしたが、気のせいだった。",
                                        "...looks like that was just your imagination."));
				}
			}

			break;
		}
		case ACT_DETECT_ARTIFACT:
		{
			msg_format(_("%sが光った！", "%s glows brightly!"),name);
			search_specific_object(2);
			break;
		}
		case ACT_LUNATIC_TIME:
		{
			int time = randint1(20) + 20;
			msg_format(_("地獄の松明が煌々と輝いた！",
                        "The Hell Torch shines brightly!"));
			(void)set_afraid(0);
			(void)set_fast(time,FALSE);

			if (p_ptr->pclass == CLASS_LARVA)
			{
				msg_format(_("あなたは松明に込められた生命力を暴走させた！",
                            "You cause the life energy held in the torch to run wild!"));
				project_hack(GF_OLD_POLY, p_ptr->lev * 2);
				project(0, 3 + randint1(3), py, px, 10, GF_MAKE_FLOWER, PROJECT_JUMP | PROJECT_GRID, -1);
				//しばらく使えなくする
				msg_print(_("松明が消えてしまった...",
                            "The torch is extinguished..."));
				p_ptr->update |= PU_TORCH;
				o_ptr->timeout += 20 + randint1(20);
				return TRUE;
			}
			else
			{
				aggravate_monsters(-1, TRUE);
				if (one_in_(3))sanity_blast(0, TRUE);
				else (void)set_shero(time, FALSE);
			}

			break;
		}
		case ACT_SUMMON_GHOSTS:
			{
				int num = 1 + plev / 20;
				bool flag = FALSE;

				for(;num>0;num--)
				{
					if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_GHOST,(PM_ALLOW_GROUP | PM_FORCE_PET))) flag = TRUE;
				}
				if(flag)
					msg_print(_("行灯に幽霊たちが寄ってきた。",
                                "Ghosts approach the lantern."));
				else
					msg_print(_("特に何も起こらなかった。",
                                "Nothing special happens."));

			}
			break;
		case ACT_TRANSPORT_ITEM:
			{
				msg_print(_("日傘を開くと中が自分の部屋に繋がっていた。",
                            "You open up the parasol; it is connected to your house."));
				item_recall(0);
			}
			break;
		case ACT_TOYOHIME:
			{
				int tx,ty;
				int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
				if (!get_aim_dir(&dir)) return FALSE;
				msg_format(_("あなたは扇を開き、大きく打ち振った！",
                            "You unfold the fan and make a sweeping motion!"));
				fire_spark(GF_DISINTEGRATE,dir,999,2);

			break;
			}
		case ACT_MIGHTY_THROW:
			{
				msg_print(_("あなたは投げるものを探し始めた・・",
                            "You start looking for something to throw..."));
				if (!do_cmd_throw_aux(4, FALSE, -1)) return FALSE;

			}
			break;

		case ACT_FINAL_STRIKE:
			{
				if( !get_check_strict(_("武器は破壊されます。よろしいですか? ",
                                        "This will destroy your weapon. Are you sure? "), CHECK_NO_HISTORY)) return FALSE;
				final_strike(item, TRUE);
			}
			break;

		case ACT_SHIOMITSUTAMA:
			{
				msg_format(_("突如潮が満ちて渦を巻いた！",
                            "A tidal whirlpool suddenly appears!"));
				project(0, 7, py, px, 7, GF_WATER_FLOW, PROJECT_GRID, -1);
				project(0, 7, py, px, (100+randint1(100)), GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			}
			break;

		case ACT_DAZZLE:
			{
				int power = 100 + plev * 4;
				if(o_ptr->name1 == ART_BEPPIN)
					msg_format(_("「キリステ・ゴーメン...」",
                                "'Kirisute gomen...'"));
				else
					msg_print(_("あなたの周りに様々な幻が踊った...",
                                "Weird visions dance around you..."));
				confuse_monsters(power);
				stun_monsters(power);

			}
			break;
			//v1.1.31 武器を伸ばす　射撃属性ビーム扱い
		case ACT_MID_RANGE_ATTACK:
			{
				int dam = 100 + randint1(100);
				project_length = 5;

				if(p_ptr->csp < 30)
				{
					msg_print(_("しかしMPがたりない！", "Not enough MP!"));
					return FALSE;
				}

				if (!get_aim_dir(&dir)) return FALSE;

				msg_print(_("ブラックロッドが伸びた！",
                            "The black rod extends forth!"));
				p_ptr->csp -= 30;
				p_ptr->redraw |= PR_MANA;
				fire_beam(GF_ARROW,dir,dam);

			}
			break;
		case ACT_BO_INACT:
			{

				if (!get_aim_dir(&dir)) return FALSE;
				msg_print(_("銀の刃が飛んだ！",
                            "Silver blade flies forth!"));
				fire_bolt(GF_INACT, dir, damroll(4, 6));
			}
			break;

			//v1.1.32
		case ACT_MATARA:
			{
				int		v = 25 + randint1(25);
				msg_print(_("小気味良い鼓の音が響き渡った...",
                            "Drumbeats start echoing..."));
				set_blessed(v,FALSE);
				set_resist_magic(v,FALSE);
				set_protevil(v,FALSE);
			}
			break;
			//v1.1.36
		case ACT_HYAKKIYAKOU:
			{
				bool flag = FALSE;
				int num = 6+randint1(6);
				for(;num>0;num--)
				{
					if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_TSUKUMO,(PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))) flag = TRUE;
				}
				if(flag)
					msg_print(_("どこからともなく付喪神たちが現れた。",
                                "Tsukumogami appear out of nowhere."));
				else
					msg_print(_("特に何も起こらなかった。",
                                "Nothing happens."));
			}
			break;
		case ACT_SWORD_DANCE:
			{
				int num;
				cave_type       *c_ptr;
				monster_type    *m_ptr;

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
					msg_print(_("武器を持っていないとこの発動は使えない。",
                                "You cannot activate it if you're not wielding a weapon."));
					return FALSE;
				}

				msg_print(_("あなたは剣の舞いを踊った！",
                            "Your weapon starts dancing in the air!"));
				//隣接グリッドのランダムなモンスターに4回攻撃する
				for (num = 0; num < 4; num++)
				{
					int cnt = 0;
					int tx, ty;
					for (dir = 0; dir < 9; dir++)
					{
						int y = py + ddy_ddd[dir];
						int x = px + ddx_ddd[dir];
						c_ptr = &cave[y][x];
						m_ptr = &m_list[c_ptr->m_idx];
						if (c_ptr->m_idx && m_ptr->ml && !is_pet(m_ptr) && !is_friendly(m_ptr))
						{
							cnt++;
							if (one_in_(cnt))
							{
								ty = y;
								tx = x;
							}
						}
					}
					if (cnt)
						py_attack(ty, tx, 0);
					else
						break;

				}

			}
			break;

		case ACT_BA_LITE://v1.1.57 アイテム「七星剣」発動でスターバーストを使えるようにしてみた
			{
				if (!get_aim_dir(&dir)) return FALSE;
				msg_format(_("%sが閃光を放った！",
                            "%s releases a flash of light!"), name);
				fire_ball(GF_LITE, dir, (50 + plev * 4 + damroll(10,10)), 4);
				break;
			}
			//v1.1.62 新アイテム「魔法の矢筒」の発動
		case ACT_ACCESS_HOME_ONLY_ARROW:
			{

				hack_flag_access_home_only_arrow = TRUE;
				do_cmd_store();
				hack_flag_access_home_only_arrow = FALSE;


			}

			break;

		case ACT_SUMMON_HANIWA:
		{
			if(summon_specific(-1, py, px, dun_level, SUMMON_HANIWA, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				msg_print(_("無尽の兵団を召喚した。", "You summon the inexhaustible corps."));
			else
				msg_print(_("何も現れなかった...", "Nobody appears..."));

			break;
		}

		//v1.1.74
		case ACT_TIM_ADD_DICES:
		{
			int base = 4 + p_ptr->lev / 8;
			int sides = 4 + p_ptr->lev / 8;

			set_ele_attack(ATTACK_INC_DICES, base + randint1(sides));
			break;
		}

		case ACT_PSY_SPEAR:
		{
			msg_print(_("眩く輝き始めた...", "It starts shining brightly..."));
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(GF_PSY_SPEAR, dir, randint1(150) + 150);
			break;
		}

		case ACT_LONG_RANGE_ATTACK:
		{
			monster_type *m_ptr;
			int x, y;

			project_length = 3 + plev / 12;
			if (!get_aim_dir(&dir)) return FALSE;
			if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
			{
				msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                            "You have to pick a target you can see."));
				return FALSE;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print(_("そこには何もいない。", "There's nobody here."));
				return FALSE;
			}
			else
			{
				py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_NO_AURA);
			}
			break;

		}
		case ACT_HIT_AND_AWAY:
		{
			msg_print(_("あなたは素早く動いた。", "You move quickly."));
			if (!hit_and_away()) return FALSE;

		}
		break;
		case ACT_SAINT_STAR_ARROW:
		{
			msg_print(_("弓は力強く輝いている...", "The bow is shining powerfully..."));
			if (p_ptr->shoot_penalty < SHOOT_HIGH_PENALTY)
			{
				msg_print(_("しかし今の装備では射撃ができない。",
                            "However, you cannot shoot with your current equipment."));
				return FALSE;
			}

			snipe_type = SP_FINAL;
			do_cmd_fire();
			snipe_type = 0;

			break;
		}
		case ACT_TIM_SUPER_STEALTH:
		{
			int base = 2 + p_ptr->lev / 12;
			int dice = 2 + p_ptr->lev / 12;

			msg_print(_("あなたの気配が消えた。", "You conceal your presence."));

			set_tim_superstealth(base + randint1(dice), FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}

		case ACT_DUAL_CAST:
		{

			if (!can_do_cmd_cast()) return FALSE;
			do_cmd_cast();
			handle_stuff();
			if (can_do_cmd_cast())
				do_cmd_cast();

			break;
		}
		case ACT_ART_CAPTURE:
		{
			if (!activate_artifact_capture(o_ptr)) return FALSE;


			break;
		}
		case ACT_TRANSFORM:
		{
			int r_idx = 0;

			get_mon_num_prep(mon_hook_random_metamorphose, NULL);
			r_idx = get_mon_num(p_ptr->lev);
			get_mon_num_prep(NULL, NULL);

			metamorphose_to_monster(r_idx, 100);
			break;

		}

		case ACT_MAKE_MAGMA:
		{

			msg_format(_("%sが真っ赤に輝き始めた...",
                        "%s starts glowing deep red..."),name);

			if (!get_aim_dir(&dir)) return FALSE;
			if (!fire_ball_jump(GF_LAVA_FLOW, dir, 3 + randint1(2), 4, NULL)) return FALSE;
			fire_ball_jump(GF_FIRE, dir, 300, 5, NULL);
			break;
		}

		case ACT_MAKE_BLIZZARD:
		{

			msg_format(_("%sが青白く輝き始めた...",
                        "%s starts glowing blue-white..."), name);

			if (!get_aim_dir(&dir)) return FALSE;
			if (!fire_ball_jump(GF_MAKE_BLIZZARD, dir, 3 + randint1(2), 4, NULL)) return FALSE;
			fire_ball_jump(GF_COLD, dir, 300, 5, NULL);
			break;
		}

		case ACT_DIG_OIL:
		{
			msg_print(_("あなたは石油を掘り始めた...", "You start digging for oil..."));
			//v1.1.94 モンスターへのダメージはないが、紫特技で酒属性に変わるのでパワーを少し上げておく
			project(0, 4, py, px, plev*10, GF_DIG_OIL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
			break;
		}

		break;

		case ACT_BLAST_MISSILES:
		{
			int dice = 4;
			int sides = 4+plev/6;
			int num = 5 + plev / 12;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format(_("あなたは魔法の矢を乱射した！", "You rapidly fire magic missiles!"));
			fire_blast(GF_MISSILE, dir, dice, sides, 10, 3, 0);
			break;
		}

		case ACT_TRANSPORTATION_TRAP:
		{
			int v = 10 + randint1(10);

			set_tim_transportation_trap(v, FALSE);

		}
		break;

		case ACT_MUGENSYUKU:
		{
			int	dam = p_ptr->lev * 6;

			//巫女系職業は威力2倍
			if (p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_SANAE || (CHECK_MIZUCHI_RESURRECT))
				dam *= 2;
			msg_print(_("空間が清浄な鈴の音に閉ざされた。", "Purifying sound of bells seals off the area."));
			project_hack2(GF_HOLY_FIRE, 0, 0, dam);
			project_hack(GF_NO_MOVE, dam / 10);

		}
		break;



		default:
		{
#ifdef JP
			msg_format("Unknown activation effect: %d.", act_ptr->index);
#else
			msg_format("Unknown activation effect: %d.", act_ptr->index);
#endif
			return FALSE;
		}
	}

	/* Set activation timeout */
	if (act_ptr->timeout.constant >= 0)
	{
		o_ptr->timeout = act_ptr->timeout.constant;
		if (act_ptr->timeout.dice > 0) {
			o_ptr->timeout += randint1(act_ptr->timeout.dice);
		}
	}
	else
	{
		/* Activations that have special timeout */
		switch (act_ptr->index) {
		case ACT_BR_FIRE:
			o_ptr->timeout = ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES)) ? 200 : 250;
			break;
		case ACT_BR_COLD:
			o_ptr->timeout = ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE)) ? 200 : 250;
			break;
		case ACT_TERROR:
			o_ptr->timeout = 3 * (p_ptr->lev + 10);
			break;
		case ACT_MURAMASA:
			/* Nothing to do */
			break;
		default:
			msg_format("Special timeout is not implemented: %d.", act_ptr->index);
			return FALSE;
		}
	}

	return TRUE;
}

#define BLOODY_MOON_TYPE_NUM 18
const int bloody_moon_type[BLOODY_MOON_TYPE_NUM][2] =
{
	{TR_CHAOTIC,-1},
	{TR_VAMPIRIC,-1},
	{TR_VORPAL,TR_EX_VORPAL},
	{TR_IMPACT,-1},
	{TR_BRAND_POIS,-1},
	{TR_BRAND_ACID,-1},
	{TR_BRAND_ELEC,-1},
	{TR_BRAND_FIRE,-1},
	{TR_BRAND_COLD,-1},
	{TR_SLAY_EVIL,-1},
	{TR_SLAY_GOOD,-1},
	{TR_SLAY_ANIMAL,TR_KILL_ANIMAL},
	{TR_SLAY_HUMAN,TR_KILL_HUMAN},
	{TR_SLAY_UNDEAD,TR_KILL_UNDEAD},
	{TR_SLAY_DRAGON,TR_KILL_DRAGON},
	{TR_SLAY_DEITY,TR_KILL_DEITY},
	{TR_SLAY_DEMON,TR_KILL_DEMON},
	{TR_SLAY_KWAI,TR_KILL_KWAI},
};

/*:::ブラッディムーンの属性変更*/
///item ブラッディムーン　スレイとパラメータ順、アイテムフラグに関するハードコーディング有
///mod150411 新しいアイテムフラグ値に合わせて修正。今の今まで忘れてた。
void get_bloody_moon_flags(object_type *o_ptr)
{
	int dummy, i;

	for (i = 0; i < TR_FLAG_SIZE; i++)
		o_ptr->art_flags[i] = a_info[ART_BLOOD].flags[i];

	dummy = randint1(2) + randint1(3);//本家と違いKILL_??が一度でつかなくなった分数を増やした
	for (i = 0; i < dummy; i++)
	{
		int type = randint0(BLOODY_MOON_TYPE_NUM);

		add_flag(o_ptr->art_flags, bloody_moon_type[type][0]);
		if(bloody_moon_type[type][1] >= 0 && one_in_(4))
		{
			add_flag(o_ptr->art_flags, bloody_moon_type[type][1]);
		}
	}

	dummy = randint1(2);
	for (i = 0; i < dummy; i++) one_resistance(o_ptr);

	for (i = 0; i < 2; i++)
	{
		int tmp = randint0(11);
		if (tmp < 6) add_flag(o_ptr->art_flags, TR_STR + tmp);
		else add_flag(o_ptr->art_flags, TR_STEALTH + tmp - 6);
	}
}

/*:::固定アーティファクトに対し追加耐性や能力を付加。職業依存の特殊処理もここで行う*/
///item class race 職業、性別依存などのアイテム生成特殊処理
void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr)
{
	bool give_resistance = FALSE, give_power = FALSE;

	//v2.1.0 瑞霊はアイテムの呪いを無効化 呪いフラグはobject_flags()で無視するのでここではpvalなどのマイナスを0にする
	if (p_ptr->pclass == CLASS_MIZUCHI && object_is_equipment(o_ptr))
	{
		if (o_ptr->to_a < 0) o_ptr->to_a = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->pval < 0) o_ptr->pval = 0;
	}



	if (o_ptr->name1 == ART_TERROR) /* Terror Mask is for warriors... */
	{
		if (p_ptr->pclass == CLASS_PARSEE || p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_ARCHER || p_ptr->pclass == CLASS_CAVALRY || p_ptr->pseikaku == SEIKAKU_BERSERK || p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			give_power = TRUE;
			give_resistance = TRUE;
		}
		else
		{
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
			add_flag(o_ptr->art_flags, TR_TY_CURSE);
			o_ptr->curse_flags |=
			    (TRC_CURSED | TRC_HEAVY_CURSE);
			o_ptr->curse_flags |= get_curse(2, o_ptr);
			return;
		}
	}
	/*:::神々の黄昏はフランのとき太古の怨念が付かないことにした*/
	else if (o_ptr->name1 == ART_TWILIGHT)
	{
		if (p_ptr->pclass == CLASS_FLAN)
		{
				o_ptr->to_d = 20;
				o_ptr->to_h = 20;
		}
		else
		{
			add_flag(o_ptr->art_flags, TR_TY_CURSE);
		}
		return;
	}
	//v2.0.20 楼観剣は妖夢プレイ時以外にはダメージ増加・AC大幅減少がつくようにした
	else if (o_ptr->name1 == ART_ROUKANKEN)
	{
		if (p_ptr->pclass != CLASS_YOUMU)
		{
			o_ptr->to_d = 25;
			o_ptr->to_a = -25;
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		}
		return;

	}
	//v2.1.0 瑞霊のとき地獄の首輪に永遠の呪い
	else if (o_ptr->name1 == ART_HARNESS_HELL)
	{
		if (p_ptr->pclass == CLASS_MIZUCHI)
		{
			o_ptr->curse_flags |= TRC_PERMA_CURSE;
		}
		return;

	}

	else if (o_ptr->name1 == ART_MURAMASA)
	{
		if (p_ptr->pclass != CLASS_SAMURAI && p_ptr->pclass != CLASS_YOUMU)
		{
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		}
	}
	else if (o_ptr->name1 == ART_XIAOLONG)
	{
		if (p_ptr->pclass == CLASS_MONK)
			add_flag(o_ptr->art_flags, TR_BLOWS);
	}
	else if (o_ptr->name1 == ART_BLOOD)
	{
		get_bloody_moon_flags(o_ptr);
	}
	else if (o_ptr->name1 == ART_HEAVENLY_MAIDEN)
	{
		if (p_ptr->psex != SEX_FEMALE)
		{
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		}
	}
	//一部の銃に特殊パラメータ付加
	else if(o_ptr->name1 == ART_FREEZER)
	{
		o_ptr->xtra1 = GF_COLD;
		o_ptr->xtra4 = GUN_FIRE_MODE_BEAM;
	}
	else if(o_ptr->name1 == ART_RAT)
	{
		o_ptr->xtra1 = GF_ACID;
		o_ptr->xtra4 = GUN_FIRE_MODE_BOLT;

	}


	if (a_ptr->gen_flags & (TRG_XTRA_POWER)) give_power = TRUE;
	if (a_ptr->gen_flags & (TRG_XTRA_H_RES)) give_resistance = TRUE;
	if (a_ptr->gen_flags & (TRG_XTRA_RES_OR_POWER))
	{
		/* Give a resistance OR a power */
		if (one_in_(2)) give_resistance = TRUE;
		else give_power = TRUE;
	}

	if (give_power)
	{
		one_ability(o_ptr);
	}

	if (give_resistance)
	{
		one_high_resistance(o_ptr);
	}


}


/*
 * Create the artifact of the specified number
 */
/*:::指定したidの★を指定位置に生成*/
/*:::すでに出現済みかどうかは判定済みであること*/
bool create_named_art(int a_idx, int y, int x)
{
	object_type forge;
	object_type *q_ptr;
	int i;

	artifact_type *a_ptr = &a_info[a_idx];

	/* Get local object */
	q_ptr = &forge;

	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	/*:::a_infoにあるが存在しないベースアイテムのとき*/
	if (!i) return FALSE;

	/* Create the artifact */
	object_prep(q_ptr, i);

	/* Save the name */
	q_ptr->name1 = a_idx;

	/* Extract the fields */
	q_ptr->pval = a_ptr->pval;
	q_ptr->ac = a_ptr->ac;
	q_ptr->dd = a_ptr->dd;
	q_ptr->ds = a_ptr->ds;
	q_ptr->to_a = a_ptr->to_a;
	q_ptr->to_h = a_ptr->to_h;
	q_ptr->to_d = a_ptr->to_d;
	q_ptr->weight = a_ptr->weight;

	///mod150908 クラウンピースの松明の特殊処理
	if(q_ptr->name1 == ART_CLOWNPIECE) q_ptr->xtra4 = -1;


	/* Hack -- extract the "cursed" flag */
	if (a_ptr->gen_flags & TRG_CURSED) q_ptr->curse_flags |= (TRC_CURSED);
	if (a_ptr->gen_flags & TRG_HEAVY_CURSE) q_ptr->curse_flags |= (TRC_HEAVY_CURSE);
	if (a_ptr->gen_flags & TRG_PERMA_CURSE) q_ptr->curse_flags |= (TRC_PERMA_CURSE);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) q_ptr->curse_flags |= get_curse(0, q_ptr);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) q_ptr->curse_flags |= get_curse(1, q_ptr);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) q_ptr->curse_flags |= get_curse(2, q_ptr);

	random_artifact_resistance(q_ptr, a_ptr);

	/*
	 * drop_near()内で普通の固定アーティファクトが重ならない性質に依存する.
	 * 仮に2個以上存在可能かつ装備品以外の固定アーティファクトが作成されれば
	 * この関数の返り値は信用できなくなる.
	 */

	//月都万象展の特殊アイテム
	if(p_ptr->inside_quest == QUEST_MOON_VAULT && a_idx == ART_HAGOROMO_MOON)
		q_ptr->marked |= OM_SPECIAL_FLAG;

	/* Drop the artifact from heaven */
	return drop_near(q_ptr, -1, y, x) ? TRUE : FALSE;
}

/*:::アイテムの種類で分岐しフラグを付加する。*/
/*:::ベースアイテムに一定確率で付加されるフラグを実装するために作ってみた。*/
/*:::apply_magic()の中の★判定の後で行う。（前に行っても無視されるだけのはずだが）*/
/*:::「|切が重なったら|X切になる」みたいな処理は☆生成では機能するが、エゴ生成では機能しない。別処理が必要。*/
void add_art_flags(object_type * o_ptr)
{

	if(o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_EVIL_KATANA && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
		//if(one_in_(4)) 	add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		if(one_in_(4))
		{
			o_ptr->to_a -= damroll(3,10);
			o_ptr->to_d += 5;
		}
	}
	else if(o_ptr->tval == TV_SWORD && o_ptr->sval == SV_WEAPON_CLAYMORE && one_in_(4))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
	}
	else if(o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_KATANA && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
	}
	else if(o_ptr->tval == TV_SPEAR && o_ptr->sval == SV_WEAPON_TRIFURCATE_SPEAR)
	{
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_THROW);

		if(one_in_(3)) add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		else add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
	}
	else if(o_ptr->tval == TV_POLEARM && o_ptr->sval == SV_WEAPON_SCYTHE_OF_SLICING)
	{
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		if(one_in_(2)) add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
	}

	else if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_MIKO && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_RES_HOLY);
	}
	else if(o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_CLOAK_FUR && one_in_(3))
	{
		add_flag(o_ptr->art_flags, TR_RES_COLD);
	}
	else if(o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_MASK && one_in_(3))
	{
		add_flag(o_ptr->art_flags, TR_RES_BLIND);
	}
	else if(o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_KASA && one_in_(5))
	{
		add_flag(o_ptr->art_flags, TR_RES_BLIND);
	}
}
