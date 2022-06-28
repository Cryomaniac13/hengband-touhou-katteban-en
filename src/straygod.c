#include "angband.h"

/*:::種族「野良神様」用の関数やデータなど*/
//ついでに妖精用の追加特性もここに

/*:::野良神様用の徳性変更ルーチン*/
/*:::p_ptr->race_multipur_val[0]～[2]を使う。[0]が秩序度、[1]が好戦度、[2]が名声度　範囲は0-255*/
void set_deity_bias(int type,int num)
{

	if(p_ptr->prace != RACE_STRAYGOD) return;
	if (p_ptr->inside_arena) return; //v1.1.51 新アリーナ内で野良神様の徳性値は変動しない
	if(!num) return;
	if(p_ptr->max_plv > 44) return; //レベル45以降はもう変化しない

	if(type < DBIAS_COSMOS || type > DBIAS_REPUTATION)
	{
		msg_format(_("ERROR:set_deity_bias()に定義されていない徳性値が入った(%d)",
                    "ERROR: Undefined bias value (%d) in set_deity_bias()"),type);
		return;
	}

	/*:::あまり端っこの数値にはなりにくいように補正する*/
	if(p_ptr->race_multipur_val[type] > 192 && num > 0 || p_ptr->race_multipur_val[type] < 64 && num < 0)
	{
		//v1.1.28 条件式修正
		if( num==1 || num==-1)
		{
			if( one_in_(3)) return;
		}
		else
		{
			num = (num * 2 + randint0(3))/ 3;
		}
	}

	p_ptr->race_multipur_val[type] += num;
	if(p_ptr->race_multipur_val[type] < 0) p_ptr->race_multipur_val[type] = 0;
	else if(p_ptr->race_multipur_val[type] > 255) p_ptr->race_multipur_val[type] = 255;

	return;

}

/*:::野良神様用　現在の徳性値に応じて新たな神性タイプを得る*/
//p_ptr->race_multipur_val[0-2]:現在の特性値
//p_ptr->race_multipur_val[3]:現在の神格タイプ
//p_ptr->race_multipur_val[4]:ランク1のときのタイプ + ランク2のときのタイプ * 128
void change_deity_type(int rank)
{
	int i;
	int new_type = -1;
	int cosmos = 999;
	int noble = 999;
	int reputation = 999;

	if(p_ptr->prace != RACE_STRAYGOD) return;

	if(rank < 1 || rank > 3)
	{	msg_format(_("ERROR:change_deity_type()に定義されていないrank値が入った(%d)",
                    "ERROR: Undefined rank (%d) in change_deity_type()"),rank);	return;	}

	for(i=0;deity_table[i].rank != 0;i++)
	{
		if(rank != deity_table[i].rank) continue;

		if(new_type == -1) new_type = i;
		else
		{
			int dis_c1,dis_c2,dis_w1,dis_w2,dis_n1,dis_n2;

			dis_c1 = deity_table[new_type].dbias_cosmos - p_ptr->race_multipur_val[DBIAS_COSMOS];
			if(dis_c1 < 0) dis_c1 *= -1;
			dis_c2 = deity_table[i].dbias_cosmos - p_ptr->race_multipur_val[DBIAS_COSMOS];
			if(dis_c2 < 0) dis_c2 *= -1;

			dis_w1 = deity_table[new_type].dbias_warlike - p_ptr->race_multipur_val[DBIAS_WARLIKE];
			if(dis_w1 < 0) dis_w1 *= -1;
			dis_w2 = deity_table[i].dbias_warlike - p_ptr->race_multipur_val[DBIAS_WARLIKE];
			if(dis_w2 < 0) dis_w2 *= -1;

			dis_n1 = deity_table[new_type].dbias_reputation - p_ptr->race_multipur_val[DBIAS_REPUTATION];
			if(dis_n1 < 0) dis_n1 *= -1;
			dis_n2 = deity_table[i].dbias_reputation - p_ptr->race_multipur_val[DBIAS_REPUTATION];
			if(dis_n2 < 0) dis_n2 *= -1;

			//一番近い特性値の神格タイプに変化　名声値の影響は他の二倍
			if( (dis_c1 + dis_w1 + dis_n1 * 2) > (dis_c2 + dis_w2 + dis_n2 * 2)) new_type = i;
		}

	}
	if(rank == 2) p_ptr->race_multipur_val[4] = p_ptr->race_multipur_val[3];
	else if(rank == 3) p_ptr->race_multipur_val[4] += p_ptr->race_multipur_val[3] * 128;
	p_ptr->race_multipur_val[3] = new_type;

	//if(rank != 1)
	{
		msg_print(_("新たな信仰が集まってきたのを感じる・・", "You feel you've gathered more faith..."));
		msg_format(_("あなたは「%s」となった！", "You have became a %s!"),deity_table[new_type].deity_name);
	}
	p_ptr->update |= (PU_BONUS);

	return;
}


/*:::野良神様の耐性付与ルーチン　calc_bonuses()から呼ぶ　apply_deity_ability_flags()と一致するよう注意*/
void apply_deity_ability(int deity_type, int *new_speed)
{
	if(p_ptr->prace != RACE_STRAYGOD) return;
	if(deity_type < 0 || deity_type >= MAX_DEITY_TABLE_ENTRY)
	{
		msg_format(_("ERROR:apply_deity_ability()がテーブル外の値(%d)で呼ばれた",
                    "ERROR: apply_deity_ability() called with value (%d) out of table bounds"),deity_type);
		return;
	}

	switch(deity_type)
	{
		case 0: //p_ptr->race_multipur_val[4]の空白部
		break;

		case 1: //実りの神様
		p_ptr->slow_digest = TRUE;
		break;
		case 2: //紅葉の神様
		p_ptr->sustain_chr = TRUE;
		break;
		case 3: //地蔵
		p_ptr->free_act = TRUE;
		break;
		case 4: //忘れられた道祖神
		p_ptr->see_inv = TRUE;
		break;
		case 5: //古い梅の木の神様
		p_ptr->sustain_wis = TRUE;
		break;
		case 6: //雪隠神
		p_ptr->resist_disen = TRUE;
		break;
		case 7: //変な岩
		p_ptr->resist_fire = TRUE;
		break;
		case 8: //祭具
		p_ptr->resist_pois = TRUE;
		break;
		case 9: //沼
		p_ptr->resist_water = TRUE;
		break;
		case 10: //大鷲
		p_ptr->levitation = TRUE;
		break;
		case 11: //産土神
		p_ptr->esp_human = TRUE;
		break;
		case 12: //氏神(何もない)
		break;
		case 13: //神霊
		p_ptr->resist_holy = TRUE;
		break;
		case 14: //霊獣
		p_ptr->esp_animal = TRUE;
		break;

		//rank2
		case 15: //騎士
		p_ptr->resist_fear = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_SWORD) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_SWORD) p_ptr->to_ds[1]+=2;
		break;

		case 16: //刀
		p_ptr->resist_disen = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_KATANA) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_KATANA) p_ptr->to_ds[1]+=2;
		break;

		case 17: //火山
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_fire = TRUE;
		break;

		case 18: //商売
		p_ptr->dec_mana = TRUE;
		break;
		case 19: //無病息災
		p_ptr->resist_pois = TRUE;
		p_ptr->free_act = TRUE;
		break;
		case 20: //ロック
		p_ptr->resist_sound = TRUE;
		p_ptr->resist_chaos = TRUE;
		break;
		case 21: //縁結び
		p_ptr->esp_human = TRUE;
		p_ptr->esp_unique = TRUE;
		break;
		case 22: //学業成就
		p_ptr->resist_conf = TRUE;
		p_ptr->sustain_int = TRUE;
		break;
		case 23: //舞踏
		p_ptr->free_act = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;

		case 24: //武道
		if(p_ptr->do_martialarts)
		{
			p_ptr->to_dd[0] +=1;
		}
		break;
		case 25: //木こり
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_AXE) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_AXE) p_ptr->to_ds[1]+=2;
		case 26: //ジャングル
		p_ptr->resist_pois = TRUE;
		p_ptr->regenerate = TRUE;
		break;
		case 27: //焼き芋
		p_ptr->resist_fire = TRUE;
		break;
		case 28: //狩人
		p_ptr->xtra_might = TRUE;
		break;
		case 29: //乱数
		p_ptr->easy_spell = TRUE;
		break;
		case 30: //機織り
		p_ptr->sustain_dex = TRUE;
		p_ptr->resist_disen = TRUE;
		break;
		case 31: //旅人
		p_ptr->slow_digest = TRUE;
		break;
		case 32: //大道芸
		p_ptr->mighty_throw = TRUE;
		break;
		case 33: //旱魃
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_water = TRUE;
		break;
		case 34: //吹雪
		p_ptr->resist_cold = TRUE;
		p_ptr->sh_cold = TRUE;
		break;
		case 35: //野分
		*new_speed += 2;
		p_ptr->levitation = TRUE;
		break;
		case 36: //暗殺者
		p_ptr->resist_fear = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_KNIFE) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_KNIFE) p_ptr->to_ds[1]+=2;
		break;
		case 37: //火災
		p_ptr->resist_fire = TRUE;
		p_ptr->sh_fire = TRUE;
		break;
		case 38: //泥棒
		p_ptr->warning = TRUE;
		break;
		case 39: //藁人形
		p_ptr->resist_neth = TRUE;
		break;
		case 40: //キノコ
		p_ptr->regenerate = TRUE;
		p_ptr->resist_pois = TRUE;
		break;
		case 41: //疫病
		p_ptr->resist_pois = TRUE;
		break;

		//rank3
		case 42: //破壊神
			p_ptr->resist_conf = TRUE; //v1.1.50 忘れてたので追加
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->kill_wall = TRUE;
		break;
		case 43: //大地神
		p_ptr->pass_wall = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_sound = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->slow_digest = TRUE;
		break;

		case 44: //太陽神
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_lite = TRUE;//免疫
		p_ptr->resist_blind = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->levitation = TRUE;
		break;

		case 45: //天帝
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->levitation = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		case 46: //福の神
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->warning = TRUE;
		break;

		case 47: //海神
		p_ptr->resist_water = TRUE; //免疫処理追加
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_pois = TRUE;
		break;

		case 48: //鬼神
		p_ptr->resist_fear = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_elec = TRUE;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_con = TRUE;
		break;

		case 49: //豊穣神
		p_ptr->resist_water = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_pois = TRUE;
		break;


		case 50: //叡智の神
		p_ptr->telepathy = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		break;

		case 51: //軍神
		p_ptr->resist_fear = TRUE;
		p_ptr->warning = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->reflect = TRUE;
		break;

		case 52: //愛欲の神
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_disen = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;

		case 53: //酒神
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 54: //風神
		p_ptr->speedster = TRUE;
		p_ptr->levitation = TRUE;
		*new_speed += 5;
		break;

		case 55: //雷神
		p_ptr->resist_elec = TRUE;
		p_ptr->immune_elec = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->resist_sound = TRUE;
		break;

		case 56: //閻魔
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		case 57: //祟り神
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;
		case 58: //混沌の神
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->esp_evil = TRUE;
		break;
		case 59: //魔神
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->sh_fire = TRUE;
		break;

		case 60: //恐怖の大王
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 61: //死者の神
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->esp_undead = TRUE;
		break;

		case 62: //邪神
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->esp_demon = TRUE;
		p_ptr->sh_cold = TRUE;
		break;

		case 63: //外なる神
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_fear = TRUE;
		break;

		case 64: //蹴鞠
		p_ptr->sustain_dex = TRUE;
		break;
		case 65: //安産
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_neth = TRUE;
		break;
		case 66: //花火
		p_ptr->resist_fire = TRUE;
		break;
		case 67: //シェフ
		p_ptr->regenerate = TRUE;
		p_ptr->slow_digest = TRUE;
		break;
		case 68: //かまど
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_blind = TRUE;
		break;
		case 69: //筋トレ
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_con = TRUE;
		break;
		case 70: //狼 何もない
		break;
		case 71: //大滝
		p_ptr->resist_water = TRUE;
		break;
		case 72: //治水
		p_ptr->resist_water = TRUE;
		p_ptr->resist_disen = TRUE;
		break;
		case 73: //夕焼け
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		break;

		case 74: //狩猟神
		p_ptr->see_inv = TRUE;
		break;

		case 75: //月
		p_ptr->see_inv = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 76: //夢
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_time = TRUE;
		break;

		case 77: //鍛冶神
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_lite = TRUE;
		break;

		case 78: //鉱山
		p_ptr->resist_dark = TRUE;
		break;

		case 79: //水子霊(瓔花用)
		p_ptr->resist_water = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		default:
		msg_format(_("ERROR:apply_deity_ability()で未登録の神格タイプ(%d)が呼ばれた",
                    "ERROR: apply_deity_ability() called for an unlisted deity type (%d)"),deity_type);
		return;
	}

}


/*:::野良神様の耐性記述ルーチン　player_flags()から呼ぶ*/
void apply_deity_ability_flags(int deity_type, u32b flgs[TR_FLAG_SIZE])
{
	if(p_ptr->prace != RACE_STRAYGOD) return;
	if(deity_type < 0 || deity_type >= (MAX_DEITY_TABLE_ENTRY))
	{
		msg_format(_("ERROR:apply_deity_ability_flags()がテーブル外の値(%d)で呼ばれた",
                    "ERROR: apply_deity_ability_flags() called with value (%d) out of table bounds"),deity_type);
		return;
	}

	switch(deity_type)
	{
		case 0: break;

		case 1: //実りの神様
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 2: //紅葉の神様
		add_flag(flgs, TR_SUST_CHR);
		break;
		case 3: //地蔵
		add_flag(flgs, TR_FREE_ACT);
		break;
		case 4: //忘れられた道祖神
		add_flag(flgs, TR_SEE_INVIS);
		break;
		case 5: //古い梅の木の神様
		add_flag(flgs, TR_SUST_WIS);
		break;
		case 6: //雪隠神
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 7: //変な岩
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 8: //祭具
		add_flag(flgs, TR_RES_POIS);
		break;
		case 9: //沼
		add_flag(flgs, TR_RES_WATER);
		break;
		case 10: //鷲
		add_flag(flgs, TR_LEVITATION);
		break;
		case 11: //産土神
		add_flag(flgs, TR_ESP_HUMAN);
		break;
		case 12: //氏神(何もない)
		break;
		case 13: //神霊
		add_flag(flgs, TR_RES_HOLY);
		break;
		case 14: //霊獣
		add_flag(flgs, TR_ESP_ANIMAL);
		break;

		case 15: //騎士
		add_flag(flgs, TR_RES_FEAR);
		break;
		case 16: //刀
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 17: //火山
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_SHARDS);
		break;
		case 18: //商売
		add_flag(flgs, TR_DEC_MANA);
		break;
		case 19: //無病息災
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		break;
		case 20: //ロック
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_CHAOS);
		break;
		case 21: //縁結び
		add_flag(flgs, TR_ESP_HUMAN);
		add_flag(flgs, TR_ESP_UNIQUE);
		break;
		case 22: //学業成就
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_SUST_INT);
		break;
		case 23: //舞踏
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SUST_CHR);
		break;
		case 24: //武道(何もない)
		case 25: //木こり(何もない)
		break;
		case 26: //ジャングル
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_REGEN);
		break;
		case 27: //焼き芋
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 28: //狩人
		add_flag(flgs, TR_XTRA_MIGHT);
		break;
		case 29: //乱数
		add_flag(flgs, TR_EASY_SPELL);
		break;
		case 30: //機織り
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 31: //旅人
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 32: //大道芸
		add_flag(flgs, TR_THROW);
		break;
		case 33: //旱魃
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_WATER);
		break;
		case 34: //吹雪
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_SH_COLD);
		break;
		case 35: //野分
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_LEVITATION);
		break;
		case 36: //暗殺者
		add_flag(flgs, TR_RES_FEAR);
		break;
		case 37: //火災
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_SH_FIRE);
		break;
		case 38: //泥棒
		add_flag(flgs, TR_WARNING);
		break;
		case 39: //藁人形
		add_flag(flgs, TR_RES_NETHER);
		break;
		case 40: //キノコ
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 41: //疫病
		add_flag(flgs, TR_RES_POIS);
		break;

		//rank3
		case 42: //破壊神
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_HOLY);
		break;
		case 43: //大地神
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 44: //太陽神
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		break;
		case 45: //天帝
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SEE_INVIS);
		break;
		case 46: //福の神
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_WARNING);
		break;
		case 47: //海神
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 48: //鬼神
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_CON);
		break;

		case 49: //豊穣神
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 50: //叡智の神
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		break;

		case 51: //軍神
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_REFLECT);
		break;

		case 52: //愛欲
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_SUST_CHR);
		break;

		case 53: //酒
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 54: //風神
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SPEEDSTER);
		add_flag(flgs, TR_LEVITATION);
		break;

		case 55: //雷神
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_SOUND);
		break;

		case 56: //閻魔
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		break;

		case 57: //祟り神
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FEAR);
		break;

		case 58: //混沌の神
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_ESP_EVIL);
		break;

		case 59: //魔神
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SH_FIRE);
		break;

		case 60: //恐怖の大王
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 61: //死者の神
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_ESP_UNDEAD);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 62: //邪神
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_ESP_DEMON);
		add_flag(flgs, TR_SH_COLD);
		break;

		case 63: //外なる神
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FEAR);
		break;

		case 64: //蹴鞠
		add_flag(flgs, TR_SUST_DEX);
		break;
		case 65: //安産
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_NETHER);
		break;
		case 66: //花火
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 67: //シェフ
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 68: //かまど
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_BLIND);
		break;
		case 69: //筋トレ
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_CON);
		break;
		case 70: //狼 何もない
		break;
		case 71: //大滝
		add_flag(flgs, TR_RES_WATER);
		break;
		case 72: //治水
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 73: //夕焼け
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		break;

		case 74: //狩猟神
		add_flag(flgs, TR_SEE_INVIS);
		break;

		case 75: //月
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 76: //夢
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_TIME);
		break;

		case 77: //鍛冶神
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_LITE);
		break;

		case 78: //鉱山
		add_flag(flgs, TR_RES_DARK);
		break;

		case 79: //瓔花
			add_flag(flgs, TR_RES_WATER);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_NETHER);
		break;

		default:
		msg_format(_("ERROR:apply_deity_ability_flags()に未登録の値(%d)が入力された",
                    "ERROR: Unlisted value (%d) entered in apply_deity_ability_flags()"),deity_type);
		break;
	}

}

//発動変異を得てnumがちゃんと処理されているかチェックすること
/*:::野良神様用レイシャル設定ルーチン do_cmd_racial_power()からのみ呼ばれる*/
void deity_racial_power(power_desc_type *power_desc, int *num_return)
{

	int rank1,rank2,rank3;
	int num = *num_return;

	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_format(_("ERROR:deity_racial_power()が野良神様以外のとき呼ばれた",
                    "ERROR: deity_racial_power() called for non-stray-god"));
		return;
	}

	//これまでの神格履歴全て適用　このrankの番号はレベルアップ順番とは関係ない
	rank1 = p_ptr->race_multipur_val[3];
	rank2 = p_ptr->race_multipur_val[4] / 128;
	rank3 = p_ptr->race_multipur_val[4] % 128;

			if(rank1 == 17 || rank2 == 17 || rank3 == 17 ) //火山の神様
			{
				strcpy(power_desc[num].name, _("狂戦士化", "Berserk Rage"));
				power_desc[num].level = 25;
				power_desc[num].cost = 15;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 15;
				power_desc[num].info = _("一時的に狂戦士化する。", "Grants temporary berserk rage.");
				power_desc[num++].number = -1;
			}
			if(rank1 == 18 || rank2 == 18 || rank3 == 18 ) //商売の神様
			{
				strcpy(power_desc[num].name, _("目利き", "Appraise"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("アイテムを鑑定する。", "Identifies an item.");
				power_desc[num++].number = -2;
			}
			if(rank1 == 19 || rank2 == 19 || rank3 == 19 ) //無病息災の神様
			{
				strcpy(power_desc[num].name, _("癒し", "Curing"));

				power_desc[num].level = 25;
				power_desc[num].cost = 24;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("毒・切り傷・朦朧・盲目・幻覚を治療しHPを30回復する。",
                                        "Heals 30 HP and cures poison, cuts, stunning, blindness and hallucination.");
				power_desc[num++].number = -3;
			}
			if(rank1 == 20 || rank2 == 20 || rank3 == 20 ) //ロックの神様
			{
				strcpy(power_desc[num].name, _("シャウト", "Shout"));

				power_desc[num].level = 20;
				power_desc[num].cost = 18;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 20;
				power_desc[num].info = _("自分の周囲に轟音属性攻撃を行う。",
                                        "Performs a sonic attack around you.");
				power_desc[num++].number = -4;
			}
			if(rank1 == 21 || rank2 == 21 || rank3 == 21 ) //縁結びの神様
			{
				strcpy(power_desc[num].name, _("チャームモンスター", "Charm Monster"));

				power_desc[num].level = 20;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 20;
				power_desc[num].info = _("敵一体を懐柔し配下にしようと試みる。",
                                        "Attempts to charm an enemy into being your follower.");
				power_desc[num++].number = -5;

			}

			if(rank1 == 22 || rank2 == 22 || rank3 == 22 ) //学業成就の神様
			{
				strcpy(power_desc[num].name, _("精神集中", "Concentration"));

				power_desc[num].level = 20;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("一時的に知能を上昇させる。", "Temporarily raises intelligence.");
				power_desc[num++].number = -6;

			}
			if(rank1 == 23 || rank2 == 23 || rank3 == 23 ) //舞踏の神様
			{
				strcpy(power_desc[num].name, _("魅惑の舞", "Alluring Dance"));

				power_desc[num].level = 20;
				power_desc[num].cost = 24;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 30;
				power_desc[num].info = _("一時的に魅力を上昇させる。", "Temporarily raises charisma.");
				power_desc[num++].number = -7;
			}
			if(rank1 == 24 || rank2 == 24 || rank3 == 24 ) //武道の神様
			{
				strcpy(power_desc[num].name, _("士気高揚", "Raise Morale"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 10;
				power_desc[num].info = _("一時的にヒーロー状態になる。", "Grants temporary heroism.");
				power_desc[num++].number = -8;

			}
			if(rank1 == 26 || rank2 == 26 || rank3 == 26 ) //密林の神様
			{
				strcpy(power_desc[num].name, _("動物召喚", "Summon Animal"));

				power_desc[num].level = 20;
				power_desc[num].cost = 15;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("獣や自然界の生物を配下として一体召喚する。",
                                        "Summons a beast or a natural being as a follower.");
				power_desc[num++].number = -9;

			}
			if(rank1 == 27 || rank2 == 27 || rank3 == 27 ) //焼き芋の神様
			{
				strcpy(power_desc[num].name, _("焼き芋", "Roast Sweet Potato"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("おいしい「焼き芋」を生成する。周囲のモンスターが起きる。",
                                        "Creates a delicious roast sweet potato. Wakes up nearby monsters.");
				power_desc[num++].number = -10;
			}

			if(rank1 == 34 || rank2 == 34 || rank3 == 34 ) //吹雪の神様
			{
				strcpy(power_desc[num].name, _("冷気のブレス", "Frost Breath"));

				power_desc[num].level = 20;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HPの1/5の威力の冷気のブレスを吐く。",
                                        "Breathes frost. (damage: 1/5 of current HP)");
				power_desc[num++].number = -11;

			}


			if(rank1 == 37 || rank2 == 37 || rank3 == 37 ) //火災の神様
			{
				strcpy(power_desc[num].name, _("火炎のブレス", "Fire Breath"));

				power_desc[num].level = 20;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HPの1/5の威力の火炎のブレスを吐く。",
                                        "Breathes fire. (damage: 1/5 of current HP)");
				power_desc[num++].number = -13;

			}

			if(rank1 == 38 || rank2 == 38 || rank3 == 38 ) //泥棒の神様
			{
				strcpy(power_desc[num].name, _("ヒット＆アウェイ", "Hit and Away"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 15;
				power_desc[num].info = _("攻撃後瞬時に離脱する。失敗することもある。",
                                        "Teleport away after attacking. Has a chance to fail.");
				power_desc[num++].number = -14;

			}
			if(rank1 == 39 || rank2 == 39 || rank3 == 39 ) //藁人形の神様
			{
				strcpy(power_desc[num].name, _("呪い", "Curse"));

				power_desc[num].level = 20;
				power_desc[num].cost = 16;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 25;
				power_desc[num].info = _("敵一体を呪いで攻撃し15d15のダメージを与える。抵抗されると無効。",
                                        "Curses a single enemy, dealing 15d15 damage. Does nothing if resisted.");
				power_desc[num++].number = -15;

			}
			if(rank1 == 40 || rank2 == 40 || rank3 == 40 ) //キノコの神様
			{
				strcpy(power_desc[num].name, _("キノコ召喚", "Summon Mushrooms"));

				power_desc[num].level = 20;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 5;
				power_desc[num].info = _("周囲にキノコ型モンスターを召喚する。", "Summons mushroom monsters.");
				power_desc[num++].number = -16;

			}

			if(rank1 == 42 || rank2 == 42 || rank3 == 42 ) //破壊神
			{
				strcpy(power_desc[num].name, _("*破壊*", "*Destruction*"));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("周囲のダンジョンを破壊する。", "Destroys the dungeon in your vicinity.");
				power_desc[num++].number = -17;

				strcpy(power_desc[num].name, _("光の剣", "Psycho-Spear"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 50;
				power_desc[num].info = _("(レベル*3)+1d(レベル*3)の威力の、敵のバリアなどを貫通する万能属性のビームを放つ。",
                                        "Fires an almighty beam that pierces any enemy barriers. (damage: (level * 3) + 1d(level * 3))");
				power_desc[num++].number = -18;

			}

			if(rank1 == 43 || rank2 == 43 || rank3 == 43 ) //大地神
			{

				strcpy(power_desc[num].name, _("地震", "Earthquake"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 20;
				power_desc[num].info = _("地震を起こす。", "Causes an earthquake.");
				power_desc[num++].number = -19;
			}

			if(rank1 == 44 || rank2 == 44 || rank3 == 44 ) //太陽神
			{
				strcpy(power_desc[num].name, _("巨大レーザー", "Huge Laser"));
				power_desc[num].level = 35;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("核熱属性の巨大なレーザーを放つ。威力はHPの1/3になる。",
                                        "Fires a huge laser of nuclear energy. (damage: 1/3 of current HP)");
				power_desc[num++].number = -20;

				strcpy(power_desc[num].name, _("ホーリーファイア", "Holy Fire"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 50;
				power_desc[num].info = _("破邪属性の巨大な球を放つ。威力は(レベル*5)になる。",
                                        "Fires a huge ball of holy energy. (damage: level * 5)");
				power_desc[num++].number = -21;
			}
			if(rank1 == 45 || rank2 == 45 || rank3 == 45 ) //天帝
			{
				strcpy(power_desc[num].name, _("魔力回復", "Restore Mana"));
				power_desc[num].level = 30;
				power_desc[num].cost = 1;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("僅かにMPを回復する。", "Slightly restores MP.");
				power_desc[num++].number = -22;

				strcpy(power_desc[num].name, _("次元の扉", "Dimension Door."));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 50;
				power_desc[num].info = _("指定した場所へテレポートする。失敗することもある。",
                                        "Teleports to a specified location. Might fail.");
				power_desc[num++].number = -23;
			}
			if(rank1 == 46 || rank2 == 46 || rank3 == 46 ) //福の神
			{

				strcpy(power_desc[num].name, _("万福招来", "Invite Fortune"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("一時的に幸運が身に付く。フロアを移動すると解除される。",
                                        "Temporarily increases luck. Effect disappear if you move to another level.");
				power_desc[num++].number = -24;
			}
			if(rank1 == 47 || rank2 == 47 || rank3 == 47 ) //海神
			{

				strcpy(power_desc[num].name, _("アクアブレス", "Water Breath"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HPの1/4の威力の水属性のブレスを吐く。",
                                        "Breathes water. (damage: 1/4 of your current HP)");
				power_desc[num++].number = -25;

				strcpy(power_desc[num].name, _("メイルシュトロム", "Maelstrom"));
				power_desc[num].level = 40;
				power_desc[num].cost = 45;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 50;
				power_desc[num].info = _("大渦を起こして周囲の敵に最大(レベル*4)ダメージの水属性攻撃をし、さらに周囲の地形を水にする。",
                                        "Creates a huge whirpool, dealing water damage (up to level * 4) to nearby enemies; also turns terrain into water.");
				power_desc[num++].number = -26;
			}
			if(rank1 == 48 || rank2 == 48 || rank3 == 48 ) //鬼神
			{

				strcpy(power_desc[num].name, _("狂乱", "Madness"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 25;
				power_desc[num].info = _("一定時間加速し狂戦士化する。", "Temporarily hastes you and makes you berserk.");
				power_desc[num++].number = -27;

				strcpy(power_desc[num].name, _("無双", "Peerless"));
				power_desc[num].level = 40;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("周囲の8マス全てに通常攻撃を行う。", "Attacks all 8 adjacent tiles.");
				power_desc[num++].number = -28;
			}

			if(rank1 == 49 || rank2 == 49 || rank3 == 49 ) //豊穣神
			{

				strcpy(power_desc[num].name, _("空腹充足", "Satisfy Hunger"));
				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 10;
				power_desc[num].info = _("満腹状態になる。", "Makes you full.");
				power_desc[num++].number = -29;

				strcpy(power_desc[num].name, _("成長促進", "Rapid Growth"));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("周囲の地面を森や花畑に変化させる。",
                                        "Change nearby terrain into forest/flowers.");
				power_desc[num++].number = -30;
			}


			if(rank1 == 50 || rank2 == 50 || rank3 == 50 ) //叡智の神
			{
				strcpy(power_desc[num].name, _("明鏡止水", "Clear Mind"));
				power_desc[num].level = 30;
				power_desc[num].cost = 1;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("僅かにMPを回復する。", "Slightly restores MP.");
				power_desc[num++].number = -22; //天神のと重複
			}


			if(rank1 == 51 || rank2 == 51 || rank3 == 51 ) //軍神
			{
				strcpy(power_desc[num].name, _("援軍召喚", "Summon Reinforcements"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("救援を召喚する。", "Summons aid.");
				power_desc[num++].number = -31;
			}

			if(rank1 == 52 || rank2 == 52 || rank3 == 52 ) //愛欲
			{
				strcpy(power_desc[num].name, _("魅了", "Allure"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("視界内の敵全てを配下にしようと試みる。",
                                        "Attempts to make all enemies in sight your followers.");
				power_desc[num++].number = -32;

				strcpy(power_desc[num].name, _("治癒", "Healing"));
				power_desc[num].level = 40;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 45;
				power_desc[num].info = _("レベル*6のHPを回復する。", "Recovers (level * 6) HP.");
				power_desc[num++].number = -33;
			}


			if(rank1 == 53 || rank2 == 53 || rank3 == 53 ) //酒神
			{
				strcpy(power_desc[num].name, _("霊酒生成", "Create Spirit Sake"));
				power_desc[num].level = 30;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("「霊酒」を生成する。なお、酒神はいくら酒を飲んでも倒れない。",
                                        "Creates Spirit sake. A Sake God won't collapse from drinking.");
				power_desc[num++].number = -34;
			}


			if(rank1 == 54 || rank2 == 54 || rank3 == 54 ) //風神
			{
				strcpy(power_desc[num].name, _("竜巻", "Tornado"));
				power_desc[num].level = 30;
				power_desc[num].cost = 27;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("ターゲットの位置に(レベル*4)の威力の竜巻を起こして攻撃する。",
                                        "Creates a tornado in target location. (damage: level * 4)");
				power_desc[num++].number = -35;
			}

			if(rank1 == 55 || rank2 == 55 || rank3 == 55 ) //雷神
			{
				strcpy(power_desc[num].name, _("召雷", "Call Lightning"));
				power_desc[num].level = 30;
				power_desc[num].cost = 27;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("ターゲットの位置に(レベル*4)の威力の雷を起こして攻撃する。",
                                        "Makes lightning strike down at target position. (damage: level * 4)");
				power_desc[num++].number = -36;
			}

			if(rank1 == 56 || rank2 == 56 || rank3 == 56 ) //閻魔
			{
				strcpy(power_desc[num].name, _("退去命令", "Dismissal Order"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("指定シンボルのモンスターを現在のフロアから追放する。失敗することもある。",
                                        "Banishes all monsters of chosen symbol from current level. Might fail.");
				power_desc[num++].number = -37;
			}

			if(rank1 == 57 || rank2 == 57 || rank3 == 57 ) //祟り神
			{
				strcpy(power_desc[num].name, _("周辺抹殺", "Massacre Nearby"));
				power_desc[num].level = 35;
				power_desc[num].cost = 60;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 40;
				power_desc[num].info = _("周囲のモンスターを現在のフロアから追放する。失敗することもある。",
                                        "Banishes all nearby monsters from current level. Might fail.");
				power_desc[num++].number = -38;

				strcpy(power_desc[num].name, _("祟り", "Curse of Retribution"));
				power_desc[num].level = 40;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("一定時間、自分が攻撃を受けたとき同じダメージを相手に返す。",
                                        "Temporarily, makes you respond to received attacks by dealing the same damage to the enemy.");
				power_desc[num++].number = -39;

			}
			if(rank1 == 58 || rank2 == 58 || rank3 == 58 ) //混沌の神
			{
				strcpy(power_desc[num].name, _("純ログルス", "Raw Logrus"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("レベル*5の威力のカオス属性の球を放って攻撃する。",
                                        "Fires a ball of chaos. (damage: level * 5)");
				power_desc[num++].number = -40;

				strcpy(power_desc[num].name, _("現実変容", "Alter Reality"));
				power_desc[num].level = 32;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("一定時間経過後、現在のフロアが再生成される。地上やクエストダンジョンでは無効。",
                                        "Recreates current level after some delay. Does not work on surface or on quest levels.");
				power_desc[num++].number = -41;

			}

			if(rank1 == 59 || rank2 == 59 || rank3 == 59 ) //魔神
			{
				strcpy(power_desc[num].name, _("魔剣", "Demonblade"));
				power_desc[num].level = 30;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("装備している武器が秩序の勢力に大ダメージを与え、さらに生物からHPを吸収するようになる。",
                                        "Makes currently wielded weapon deal large damage to lawfully aligned enemies, and also drain HP from living beings.");
				power_desc[num++].number = -42;

				strcpy(power_desc[num].name, _("ヘルファイア", "Hellfire"));
				power_desc[num].level = 40;
				power_desc[num].cost = 60;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 60;
				power_desc[num].info = _("レベル*5の威力の地獄の劫火を放って攻撃する。天使などの聖なる存在には大ダメージを与える。",
                                        "Fires a ball of hellfire that deals large damage to angels or other holy beings. (damage: level * 5)");
				power_desc[num++].number = -43;

			}
			if(rank1 == 60 || rank2 == 60 || rank3 == 60 ) //恐怖の大王
			{
				strcpy(power_desc[num].name, _("メテオストライク", "Meteor Strike"));
				power_desc[num].level = 40;
				power_desc[num].cost = 64;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("ターゲットとその周辺に強力な隕石属性攻撃を行う。",
                                        "Launches powerful meteor attacks near the target.");
				power_desc[num++].number = -44;
			}
			if(rank1 == 61 || rank2 == 61 || rank3 == 61 ) //死者の神
			{
				strcpy(power_desc[num].name, _("死者召喚", "Summon Dead"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("アンデッドの配下を一体～複数召喚する。", "Summons one or more undead as your followers.");
				power_desc[num++].number = -45;
			}
			if(rank1 == 62 || rank2 == 62 || rank3 == 62 ) //邪神
			{
				strcpy(power_desc[num].name, _("幻惑", "Bedazzle"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("視界内の敵を朦朧・恐怖・混乱させる。", "Stuns, terrifies and confuses all enemies in sight.");
				power_desc[num++].number = -46;

				strcpy(power_desc[num].name, _("魔力吸収", "Drain Mana"));
				power_desc[num].level = 30;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("敵一体に精神属性ダメージを与え、自分のMPを回復する。",
                                        "Deals mental damage to a single enemy and restores your MP.");
				power_desc[num++].number = -47;
			}
			if(rank1 == 63 || rank2 == 63 || rank3 == 63 ) //外なる神
			{
				strcpy(power_desc[num].name, _("コズミック・ホラー", "Cosmic Horror"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("敵一体に強烈な精神攻撃を行う。アンデッドや悪魔には効きにくい。",
                                        "Performs an intense mental attack against a single enemy. Has little effect on undead and demons.");
				power_desc[num++].number = -48;
			}
			if(rank1 == 66 || rank2 == 66 || rank3 == 66 ) //花火職人
			{
				strcpy(power_desc[num].name, _("ファイア・ボール", "Fire Ball"));
				power_desc[num].level = 20;
				power_desc[num].cost = 12;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 10;
				power_desc[num].info = _("威力(レベル*2)の火炎属性の球を放つ。",
                                        "Fires a ball of fire. (damage: level * 2)");
				power_desc[num++].number = -49;
			}
			if(rank1 == 67 || rank2 == 67 || rank3 == 67 ) //シェフ
			{
				strcpy(power_desc[num].name, _("食料生成", "Create Food"));
				power_desc[num].level = 20;
				power_desc[num].cost = 12;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 20;
				power_desc[num].info = _("食料を生成する。", "Creates a food ration.");
				power_desc[num++].number = -50;
			}
			if(rank1 == 70 || rank2 == 70 || rank3 == 70 ) //狼
			{
				strcpy(power_desc[num].name, _("モンスター感知", "Detect Monsters"));
				power_desc[num].level = 10;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 10;
				power_desc[num].info = _("近くのモンスターを感知する。", "Detects nearby monsters.");
				power_desc[num++].number = -51;
			}
			if(rank1 == 75 || rank2 == 75 || rank3 == 75 ) //月の神
			{
				strcpy(power_desc[num].name, _("幻惑", "Bedazzle"));
				power_desc[num].level = 30;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("視界内の敵を朦朧・恐怖・混乱させる。", "Stuns, terrifies and confuses all enemies in sight.");
				power_desc[num++].number = -46;
			}

			if(rank1 == 76 || rank2 == 76 || rank3 == 76 ) //夢の神
			{
				strcpy(power_desc[num].name, _("次元の扉", "Dimension Door"));
				power_desc[num].level = 35;
				power_desc[num].cost = 26;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("指定した場所へテレポートする。失敗することもある。",
                                        "Teleports to specified location. Might fail.");
				power_desc[num++].number = -23;
			}
			if(rank1 == 77 || rank2 == 77 || rank3 == 77 ) //鍛冶神
			{
				strcpy(power_desc[num].name, _("武器強化", "Enchant Weapon"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 50;
				power_desc[num].info = _("指定した武器の修正値を強化する。16を超えない。",
                                        "Increases bonuses on specified weapon. Cannot exceed 16.");
				power_desc[num++].number = -52;

				strcpy(power_desc[num].name, _("防具強化", "Enchant Armor"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 50;
				power_desc[num].info = _("指定した防具の修正値を強化する。16を超えない。",
                                        "Increases AC bonus on specified armor. Cannot exceed 16.");
				power_desc[num++].number = -53;
			}
			if(rank1 == 78 || rank2 == 78 || rank3 == 78 ) //鉱山
			{
				strcpy(power_desc[num].name, _("素材感知", "Detect Materials"));
				power_desc[num].level = 20;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("フロアに素材カテゴリのアイテムがあればそれを察知する。",
                                        "Detects material items on this level.");
				power_desc[num++].number = -54;
			}



	*num_return = num;
}



/*:::野良神様用レイシャルパワー実行部　行動順消費するときTRUE*/
bool deity_racial_power_aux(s32b command)
{
	int dir;
	int plev = p_ptr->lev;
	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_format(_("deity_racial_power_aux()が野良神様以外から呼ばれた",
                    "deity_racial_power_aux() called for a non-stray-god."),command);
		 return FALSE;
	}

	switch(command)
	{
		case -1: //狂戦士化
		{
			int base = 20;
			msg_print(_("怒りの力を呼び覚ました！", "You awaken the power of anger!"));
			set_shero(randint1(base) + base, FALSE);
			hp_player(30);
			set_afraid(0);

		}
		break;
		case -2: //目利き
		{
			if (!ident_spell(FALSE)) return FALSE;
		}
		break;
		case -3: //癒し
		{
			msg_print(_("癒しの力を使った・・", "You use the power of curing..."));
			set_poisoned(0);
			set_stun(0);
			set_cut(0);
			set_image(0);
			set_blind(0);
			hp_player(30);
		}
		break;
		case -4: //シャウト
		{
			msg_print(_("地の果てまで届くシャウトを放った！",
                        "You shout loud enough to be heard on ends of the earth!"));
			project(0, 7, py, px, plev * 3 + 50, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			aggravate_monsters(0,FALSE);
		}
		break;
		case -5: //チャームモンスター
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("あなたは懐柔を試みた・・", "You show your soft side..."));
			charm_monster(dir, plev * 2);
		}
		break;
		case -6: //精神集中
		{
			int base = 10;
			msg_print(_("精神を集中すると、頭が冴えてきた！", "You concentrate your mind, and your head clears!"));
			set_tim_addstat(A_INT,105,base + randint1(base),FALSE);
		}
		break;
		case -7: //ダンス
		{
			int base = 10;
			msg_format(_("%sは、ぐるぐるとおどった。", "%s dances round and round!"),player_name);
			set_tim_addstat(A_CHR,105,base + randint1(base),FALSE);
		}
		break;
		case -8: //士気高揚
		{
			int base = 25;
			msg_print(_("気合の息吹を発した！", "You take a deep breath and feel full of energy!"));
			set_hero(randint1(base) + base, FALSE);
			hp_player(10);
			set_afraid(0);
		}
		break;
		case -9: //動物召喚
		{
			if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				msg_print(_("動物は現れなかった。", "No animals appear."));
			else
				msg_print(_("動物を呼び寄せた。", "You call forth animals."));

		}
		break;
		case -10: //焼き芋
		{
			object_type forge;
			object_type *q_ptr = &forge;

			msg_print(_("あなたは芋を焼き始めた。芳しい香りが立ち込めた・・",
                        "You start roasting sweet potatoes. There is an appetizing scent..."));
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			drop_near(q_ptr, -1, py, px);
			aggravate_monsters(0,FALSE);
		}
		break;
		case -11: //冷気のブレス
		{
			int dam = p_ptr->chp / 5;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("あなたは吹雪を巻き起こした！", "You summon a blizzard!"));
			fire_ball(GF_COLD, dir, dam, -2);
		}
		break;
		case -13: //火炎のブレス
		{
			int dam = p_ptr->chp / 5;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("あなたは火を吹いた！", "You breathe fire!"));
			fire_ball(GF_FIRE, dir, dam, -2);
		}
		break;
		case -14: //ヒット＆アウェイ
		{
			if(!hit_and_away()) return FALSE;
		}
		break;

		case -15: //呪い
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball_hide(GF_CAUSE_4, dir, damroll(15, 15), 0);
		}
		break;

		case -16: //キノコ召喚
		{
			int num = randint1(3) + plev / 7;
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, plev, SUMMON_MUSHROOM, (PM_FORCE_PET)))	flag = TRUE;
			}
			if(flag) msg_print(_("奇妙な配下達が現れた。", "Some weird followers have appeared."));
			else msg_print(_("配下は現れなかった。", "No followers appears."));
		}
		break;
		case -17: //*破壊*
		{
			msg_print(_("破壊の力を解き放った！", "You unleash destructive power!"));
			destroy_area(py, px, 12 + randint1(4), FALSE,FALSE,FALSE);
		}
		break;
		case -18: //光の剣
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("光の剣を投げ放った！", "You throw a Psycho-Spear!"));
			fire_beam(GF_PSY_SPEAR, dir, randint1(plev*3)+plev*3);
		}
		break;
		case -19: //地震
		{

			msg_print(_("地面が激しく揺れ始めた・・", "The earth starts to rumble violently..."));
			earthquake(py, px, 8);
		}
		break;
		case -20: //巨大レーザー
		{
			int damage = p_ptr->chp / 3;
			if(!damage) damage = 1;
			if (!get_aim_dir(&dir)) return FALSE;

			msg_print(_("あなたは光を放った！", "You unleash light!"));
			fire_spark(GF_NUKE,dir,damage,2);
		}
		break;

		case -21: //ホーリーファイア
		{
			int damage = plev * 5;
			if (!get_aim_dir(&dir)) return FALSE;

			if(!fire_ball_jump(GF_HOLY_FIRE, dir, damage, 4,_("眩い光球が出現した！", "A sphere of dazzling light appears!"))) return FALSE;
		}
		break;
		case -22: //明鏡止水
		{
			msg_print(_("大気の魔力を取り込んだ。", "You absorb ambient mana."));
			p_ptr->csp += (3 + plev/10);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
		case -23: //次元の扉
		{
				msg_print(_("次元の扉が開いた。目的地を選んで下さい。",
                            "You open a dimensional door. Choose your destination."));
				if (!dimension_door(D_DOOR_NORMAL)) return FALSE;
		}
		break;
		case -24: //万福招来
		{
			msg_print(_("懐から小槌を取出し、大きく振った！", "You pull out a small mallet and swing it in a wide arc!"));
			set_tim_lucky(20,FALSE);
		}
		break;
		case -25: //アクアブレス
		{
			int dam = p_ptr->chp / 4;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("魔法の泡を吐きだした。", "You blow magical bubbles."));
			fire_ball(GF_WATER, dir, dam, -2);
		}
		break;
		case -26: //メイルシュトロム
		{
			int rad = 6;
			msg_print(_("大渦を巻き起こした！", "You create a maelstrom!"));
			project(0, rad, py, px, rad, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, plev * 8, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);

		}
		break;
		case -27: //狂乱
		{
			int v = 20 + randint1(20);
			msg_print(_("あなたの怒りが爆発した！", "You are bursting with rage!"));
			set_shero(v, FALSE);
			hp_player(30);
			set_afraid(0);
			set_fast(v, FALSE);
		}
		break;
		case -28: //無双
		{
			int y = 0, x = 0;
			cave_type       *c_ptr;
			monster_type    *m_ptr;

			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
					py_attack(y, x, 0);
			}
		}
		break;
		case -29: //空腹充足
		{
			msg_print(_("豊穣の力をその身に集めた・・", "You gather power of fertility in your body..."));
			set_food(PY_FOOD_MAX - 1);
		}
		break;
		case -30: //森林&花生成
		{
			msg_print(_("豊穣の力が大地に満ちた！", "The power of fertility fills the earth!"));
			project(0, 7, py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
		}
		break;
		case -31: //援軍召喚
		{
			if (summon_kin_player(p_ptr->lev, py, px, (PM_FORCE_PET | PM_ALLOW_GROUP)))
				msg_print(_("あなたは援軍を招集した！", "You call for reinforcements!"));
			else
				msg_print(_("援軍は来なかった。", "No reinforcements arrive."));
		}
		break;
		case -32: //魅了
		{
			msg_print(_("あなたは見事な肉体を見せつけた！", "You show off your beautiful body!"));
			charm_monsters(p_ptr->lev * 2);
		}
		break;
		case -33: //治癒
		{
			msg_print(_("あなたは自分の傷に集中した。", "You concentrate on your wounds."));
			hp_player(plev * 6);
		}
		break;
		case -34: //霊酒生成
		{
			object_type forge;
			object_type *q_ptr = &forge;
			msg_print(_("あなたの手の中にお酒が出現した。", "Sake appears in your hand."));
			object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_REISYU));
			apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
			(void)drop_near(q_ptr, -1, py, px);
		}
		break;
		case -35: //竜巻
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_TORNADO, dir, plev * 4, 4,_("竜巻を起こした！", "A tornado appears!"))) return FALSE;
		}
		break;
		case -36: //召雷
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_ELEC, dir, plev * 4, 4,_("雷を落とした！", "Lightning strikes!"))) return FALSE;
		}
		break;
		case -37: //抹殺
		{
			symbol_genocide(plev * 3, TRUE,0);
		}
		break;

		case -38: //周辺抹殺
		{
			mass_genocide(plev * 2, TRUE);
		}
		break;
		case -39: //祟り
		{
			int base = 20;
			set_tim_eyeeye(randint1(base) + base, FALSE);
		}
		break;
		case -40: //純ログルス
		{

			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("無秩序の波動を放った！", "You release a wave of anarchy!"));
			fire_ball(GF_CHAOS, dir, plev * 5, 4);
		}
		break;
		case -41: //現実変容
		{

			alter_reality();
		}
		break;
		case -42: //魔剣
		{
			int base = 10;
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_format(_("武器を持っていない。", "You're not wielding a weapon."));
				return FALSE;
			}
			set_ele_attack(ATTACK_DARK, base + randint1(base));
		}
		break;
		case -43: //ヘルファイア
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_HELL_FIRE, dir, plev * 5, 4,_("地獄の焔を呼び出した！", "Fires of hell come forth!"))) return FALSE;
		}
		break;
		case -44: //メテオストライク
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_METEOR, dir, plev * 6 + randint1(plev * 6), 6,_("隕石が降ってきた！", "A meteor crashes down!"))) return FALSE;
		}
		break;
		case -45: //死者召喚
		{
			int num = randint1(3);
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, plev, SUMMON_UNDEAD, (PM_FORCE_PET)))	flag = TRUE;
			}
			if(flag) msg_print(_("死者たちが地の底から這い出してきた。", "The dead rise out of the depths of the earth."));
			else msg_print(_("配下は現れなかった。", "No followers appear."));
		}
		break;
		case -46: //幻惑
		{
			int power = plev * 4;
			msg_format(_("精神波を放った。", "You release a mental wave."));
			stun_monsters(power);
			confuse_monsters(power);
			turn_monsters(power);
		}
		break;
		case -47: //魔力吸収
		{
			int dice= 1;
			int sides= plev * 2;
			int base = plev;
			if (!get_aim_dir(&dir)) return FALSE;

			fire_ball_hide(GF_PSI_DRAIN, dir, damroll(dice,sides)+base , 0);
		}
		break;
		case -48: //コズミック・ホラー
		{
			int dam = damroll(10,10) + plev * 7;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format(_("あなたは名状しがたき宇宙的恐怖を発現させた！",
                        "You reveal unspeakable cosmic horrors!"));
			fire_ball_hide(GF_COSMIC_HORROR, dir, dam, 0);
		}
		break;
		case -49: //ファイア・ボール
		{
			int dam = plev * 2;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format(_("花火を投げた！", "You throw fireworks!"));
			fire_ball(GF_FIRE, dir, dam, 3);
		}
		break;
		case -50: //食料生成
		{
			object_type forge, *q_ptr = &forge;
			msg_print(_("近くの食べられそうなものを食料に仕立てた。",
                        "You prepare some food out of nearby edible stuff."));
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
		break;
		case -51: //モンスター感知
		{
			msg_print(_("じっと耳を澄ませた・・", "You listen carefully..."));
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			detect_monsters_normal(DETECT_RAD_DEFAULT);
		}
		break;
		case -52: //武器強化
		{
			if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return FALSE;
		}
		break;
		case -53: //防具強化
		{
			if (!enchant_spell(0, 0, randint0(3) + 2)) return FALSE;
		}
		break;
		case -54: //素材感値
		{
			msg_print(_("あなたはフロアの地相に集中した・・",
                        "You concentrate on local geology..."));
			search_specific_object(3);
			break;
		}
		break;

		default:
		msg_format(_("deity_racial_power_aux()に不正なコマンド値(%d)が渡された",
                    "Incorrect command (%d) passed to deity_racial_power_aux()"),command);
		return FALSE;
	}

	return TRUE;
}



const deity_type deity_table[MAX_DEITY_TABLE_ENTRY]={
	{99,_("始点用ダミー", "starting dummy"),0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0,0},

//初期ランク　上２つは秋姉妹用  そうでないならキャラメイク時に[3]～[14]のランダムに決まる
	//遅消化
	{1,_("実りの神様", "God of Fruit"),
	128,80,80,
	{-1,-2,0,-1,0,1},-3,0,2,2,2,0,0,0},
	//魅力維持
	{1,_("紅葉の神様", "Autumn God"),
	80,140,80,
	{1,-1,-1,1,0,-1},7,2,8,3,2,5,7,-4},
	//r麻
	{1,_("苔むしたお地蔵様", "Mossy Jizo"),
	180, 60,80,
	{-2,-1,1,-1,1,0},-3,4,4,3,-3,0,-5,-5},
	//透明視認
	{1,_("忘れられた道祖神", "Forgotten God"),
	100,60, 80,
	{-1,0,1,-1,-1,-1},0,0,3,2,3,0,0,-10},
	//(賢
	{1,_("古い梅の木の神様", "Plum Tree God"),
	128,20,80,
	{0,-1,1,-3,0,1},-2,4,5,4,3,0,0,0},
	//r劣
	{1,_("ある廃屋の雪隠神", "Lavatory God"),
	160,60,80,
	{0,-1,1,-2,1,-3},-3,-2,0,2,-2,-5,0,0},
	//r火
	{1,_("変な形の岩の神様", "Odd Stone God"),
	30,180,80,
	{1,-2,-1,-2,2,-2},-5,-2,3,2,0,0,10,-5},
	//r毒
	{1,_("古い祭具の付喪神", "Ceremony Tsukumo"),
	160,140,80,
	{-1,-1,-1,-1,-1,-1},-2,8,-2,0,0,-5,-5,-5},
	//r水
	{1,_("人里離れた沼の主", "Lord of Swamp"),
	100,120,80,
	{1,-2,0,-1,0,-2},3,-3,0,2,2,5,2,2},
	//浮
	{1,_("神格化された大鷲", "Deified Eagle"),
	50,180,80,
	{0,-2,-1,2,-1,-1},-3,-2,3,2,4,0,10,-10},
	//~人
	{1,_("ある廃村の産土神", "Ubusunagami"),
	120,70,80,
	{-1,-1,1,-1,-1,1},0,0,0,2,2,0,-3,-3},
	//なし
	{1,_("衰えた名家の氏神", "Old Family God"),
	128,128,80,
	{0,0,0,0,0,0},0,0,0,0,0,0,0,0},
	//r聖
	{1,_("神霊化した元人間", "Divine Spirit"),
	200,120,80,
	{-2,-1,0,-1,-2,-1},-2,3,-3,0,0,0,0,0},
	//~動
	{1,_("年経た霊獣の化身", "Beast Avatar"),
	60,150,80,
	{0,-2,-1,0,1,-2},-2,-3,3,2,3,6,9,-6},


//第二ランク　3*3*3準備しておく
//[15]
	//高秩序　高好戦　高名声
	//r恐
	//剣装備時ダイスボーナス
	{2,_("騎士の神様", "Knight God"),
	230,230,200,
	{3,0,3,0,2,2},0,2,6,0,0,10,25,10},

	//中秩序　高好戦　高名声
	//r劣 刀装備時ダイスボーナス
	{2,_("刀の神様", "Blade God"),
	140,255,200,
	{3,-2,0,3,2,2},0,5,3,1,0,5,30,0},

	//低秩序　高好戦　高名声(狂戦士化)
	//r火破
	{2,_("火山の神様", "Volcano God"),
	40,230,200,
	{5,-1,-2,0,6,0},0,5,5,-1,3,10,25,25},

	//やや高秩序　中好戦　高名声(目利き)
	//消費魔力減少
	{2,_("商売の神様", "Trade God"),
	200,80,200,
	{0,2,1,1,0,7},3,8,0,0,5,0,-5,-5},

	//中秩序　中好戦　高名声(癒し)
	//r毒麻
	{2,_("無病息災の神様", "Health God"),
	135,100,200,
	{1,0,0,0,4,2},0,3,8,2,4,0,5,5},

	//低秩序　中好戦　高名声(轟音攻撃)
	//r轟沌
	{2,_("ロックの神様", "Rock God"),
	30,140,180,
	{1,0,0,1,1,2},-5,3,2,-2,0,6,10,5},

	//中秩序　低好戦　高名声(チャームモンスター)
	//~人個
	{2,_("縁結びの神様", "Marriage God"),
	160,50,180,
	{-2,1,2,0,2,4},0,5,5,1,5,0,5,5},

	//高秩序　低好戦　高名声(知能一時増加)
	//r乱 (知
	{2,_("学業成就の神様", "Education God"),
	200,10,180,
	{-1,4,3,1,0,3},5,8,6,3,6,0,0,0},

	//低秩序　低好戦　高名声(魅力一時上昇)
	//r麻(魅
	{2,_("舞踊の神様", "Dance God"),
	40,40,180,
	{1,0,1,3,1,4},0,5,6,1,3,3,10,0},

	//高秩序　高好戦　中名声(士気高揚)
	//格闘時ダイス増加、攻撃回数上昇
	{2,_("武道の神様", "Bushido God"),
	200,230,130,
	{2,0,0,2,2,2},0,-5,-5,0,0,10,30,10},

	//中秩序　高好戦　中名声
	//斧装備時ダイスボーナス
	{2,_("木こりの神様", "Woodcutter God"),
	120,200,120,
	{0,0,2,0,2,0},1,5,5,4,5,0,0,10},

	//低秩序　高好戦　中名声(動物召喚)
	//r毒 急回復
	{2,_("密林の神様", "Deep Forest God"),
	50,230,100,
	{2,-2,-1,2,3,-1},-3,-3,0,3,0,10,15,5},


	//高秩序　中好戦　中名声
	//r火
	//焼き芋(焼き芋生成　お菓子扱い　満腹度大幅回復、MPも回復、生成時周囲の敵が起きる)
	{2,_("焼き芋の神様", "Sweet Potato God"),
	175,100,120,
	{1,0,2,0,1,2},2,3,10,2,5,2,6,6},

	//中秩序　中好戦　中名声
	//弓装備時強力射
	{2,_("狩人の神様", "Huntsman God"),
	115,150,110,
	{0,1,1,3,0,1},7,7,3,3,6,5,10,30},

	//低秩序　中好戦　中名声
	//魔法難度低下
	{2,_("乱数の神様", "Randomness God"),
	0,128,128,
	{0,0,0,0,0,0},10,10,20,0,10,0,0,0},


	//高秩序　低好戦　中名声
	//(器 r劣
	{2,_("機織りの神様", "Weaver God"),
	240,30,110,
	{0,2,0,2,0,2},9,8,7,3,4,0,12,0},

	//中秩序　低好戦　中名声
	//遅消化
	{2,_("旅人の神様", "Traveller God"),
	130,60,90,
	{0,0,2,1,2,1},2,3,4,3,8,0,5,10},

	//低秩序　低好戦　中名声
	//強力投擲
	{2,_("大道芸の神様", "Performer God"),
	50,50,110,
	{0,2,0,3,-1,2},15,15,0,-2,6,20,20,20},


	//高秩序　高好戦　低名声
	//r火水
	{2,_("旱魃の神様", "Drought God"),
	210,230,30,
	{2,1,-1,1,3,-3},0,3,4,1,2,10,15,5},

	//中秩序　高好戦　低名声(冷気のブレス)
	//r冷[冷
	{2,_("吹雪の神様", "Blizzard God"),
	150,230,65,
	{1,1,-2,1,3,-2},0,3,4,1,-2,10,5,5},

	//低秩序　高好戦　低名声
	//浮 加速+2
	{2,_("野分の神様", "Nowaki God"),
	20,240,60,
	{2,-2,-2,2,2,-2},-2,-4,-2,3,4,10,20,10},


	//高秩序　中好戦　低名声
	//短剣にダイスボーナス
	//r恐
	{2,_("暗殺者の神様", "Assassin God"),
	190,160,30,
	{1,-2,-2,4,1,-2},3,5,-2,5,8,5,20,5},

	//中秩序　中好戦　低名声(火炎のブレス)
	//r火[火
	{2,_("火災の神様", "Flame God"),
	120,150,30,
	{1,-1,-2,1,2,-3},-5,3,6,2,2,10,15,0},

	//低秩序　中好戦　低名声(H&A)
	//警
	{2,_("泥棒の神様", "Thief God"),
	20,128,20,
	{1,1,-2,3,0,-3},10,3,-3,5,8,0,4,-4},

	//高秩序　低好戦　低名声(死の言霊)
	//r獄
	{2,_("藁人形の神様", "Straw Doll God"),
	220,10,30,
	{-1,3,0,1,1,-2},3,12,7,4,2,0,5,-5},

	//中秩序　低好戦　低名声(茸召喚)
	//r毒 急回復
	{2,_("キノコの神様", "Mushroom God"),
	100,30,60,
	{-1,-1,-1,-1,4,-2},3,3,3,4,0,0,20,0},

	//低秩序　低好戦　低名声
	//r毒
	{2,_("疫病の神様", "Pestilence God"),
	10,30,45,
	{0,0,1,0,2,-3},3,3,3,2,6,0,7,7},


//第三ランク
	//[42]
	//低秩序　高好戦　高名声（*破壊*、光の剣、壁堀移動）
	//r沌恐狂聖
	{3,_("破壊神", "Destroyer God"),
	0,255,255,
	{9,2,2,3,4,-3},-8,-3,10,-3,0,10,50,25},

	//中秩序　やや低好戦　高名声（地震、常時壁抜け）
	//r聖破轟麻遅急
	{3,_("大地神", "Earth God"),
	128,80,255,
	{3,1,3,-2,5,3},5,10,15,3,10,5,20,10},

	//高秩序　やや高好戦　高名声（巨大レーザー、ホーリーファイア）
	//*火r閃盲聖浮視 [火 自前光源
	{3,_("太陽神", "Sun God"),
	225,200,255,
	{2,2,2,2,3,5},5,12,10,-2,10,0,10,20},

	//高秩序　中好戦　高名声（明鏡止水、次元の扉)
	//r聖時乱浮視
	{3,_("天帝", "Heavenly King"),
	255,90,255,
	{2,5,4,3,3,5},10,10,10,3,10,10,25,25},


	//やや高秩序　低好戦　高名声（万福招来(一時的に白オーラ以上の幸運)）
	//r麻乱盲恐狂警
	{3,_("福の神", "Fortune God"),
	180,50,225,
	{1,2,3,2,2,6},5,8,12,2,20,5,5,5},

	//やや低秩序　中好戦　高名声（アクアブレス、メイルシュトロム）
	//*水 r酸火冷毒水
	{3,_("海神", "Sea God"),
	70,145,225,
	{3,2,3,2,3,2},0,6,12,3,7,5,20,15},


	//中秩序　高好戦　高名声（狂乱、全周囲攻撃）
	//r恐急火電(腕耐
	{3,_("鬼神", "Kishin"),
	100,255,215,
	{7,-3,2,1,5,1},0,0,10,-1,5,10,25,25},

	//中秩序　中好戦　やや高名声（空腹充足、森林生成）
	//r水毒急
	{3,_("豊穣神", "Fertility God"),
	140,90,160,
	{1,1,3,1,3,3},0,4,8,2,12,0,5,5},


	//高秩序　低好戦　中名声（明鏡止水）
	//~感 (知賢r乱
	{3,_("叡智の神", "Wisdom God"),
	225,25,160,
	{-2,5,5,1,0,3},5,10,10,2,10,5,-5,-5},


	//やや高秩序　高好戦　中名声　救援召喚
	//r恐警破　反射
	{3,_("軍神", "War God"),
	180,240,140,
	{4,2,2,2,3,2},0,0,10,-1,5,10,25,25},


	//低秩序　低好戦　中名声(高威力チャームモンスター、治癒)
	//r恐乱沌劣(魅
	{3,_("愛欲の神", "Love God"),
	60,0,140,
	{0,0,3,1,3,7},3,7,10,0,12,5,10,10},

	//やや低秩序　中好戦　中名声
	//r狂乱沌
	//霊酒醸成(水不要)
	{3,_("酒神", "Sake God"),
	55,120,120,
	{2,-2,2,1,2,3},-2,3,10,0,3,0,10,0},

	//中秩序　高好戦　中名声（加速+5、浮、高速移動、竜巻）
	{3,_("風神", "Wind God"),
	100,200,120,
	{1,1,0,5,0,2},3,5,8,1,5,10,15,10},

	//中秩序　高好戦　中名声（召雷）
	//*電r麻轟[電
	{3,_("雷神", "Thunder God"),
	95,205,120,
	{4,0,1,2,2,2},3,5,8,-2,10,10,30,10},


	//高秩序　低好戦　中名声(抹殺)
	//乱恐狂盲麻視獄
	{3,_("閻魔", "Yama"),
	255,80,120,
	{1,3,4,-1,2,1},5,10,12,1,10,10,0,0},


	//中秩序　高好戦　やや低名声（周辺抹殺、目には目）
	//r暗獄恐狂
	{3,_("祟り神", "Curse God"),
	120,180,50,
	{2,-1,1,1,4,-3},0,3,20,1,10,5,15,15},

	//低秩序　中好戦　低名声 (純ログルス、現実変容)
	//r乱沌狂 ~沌
	{3,_("混沌の神", "Chaos God"),
	0,120,50,
	{3,2,-2,2,3,-5},-5,-3,15,4,10,5,5,5},

	//低秩序　高好戦　低名声 (魔剣、ヘルファイア）
	//r火暗獄狂恐 v聖[火
	{3,_("魔神", "Demon God"),
	0,255,50,
	{4,2,0,3,3,-8},0,7,12,-2,0,5,25,0},


	//やや高秩序　高好戦　低名声 (メテオストライク、視界内恐慌)
	//r閃暗恐狂
	{3,_("恐怖の大王", "Lord of Terror"),
	160,225,30,
	{3,-2,-2,0,5,-5},-5,3,10,-3,10,5,15,35},

	//高秩序　低好戦　低名声 (地獄免疫、アンデッド召喚）
	//*獄 r狂視 ~死 v聖
	{3,_("死者の神", "God of the Dead"),
	200,20,30,
	{1,2,2,-2,2,-5},0,10,16,4,10,5,15,10},

	//中秩序　低好戦　低名声 (視界内幻惑、魔力吸収)
	//r冷暗獄狂恐 v聖 ~悪[冷
	{3,_("邪神", "Evil God"),
	130,120,30,
	{2,2,3,0,2,-6},5,12,10,0,10,5,5,5},

	//63
	//中秩序　中好戦　超低名声 (コズミックホラー)
	//*狂 r酸暗恐時 v聖
	{3,_("外なる神", "Outer God"),
	50,120,0,
	{5,5,-5,0,5,-10},0,0,20,4,5,10,25,15},


	///mod150228 追加

	//(器
	{2,_("蹴鞠の神様", "Kemari God"),
	200,95,70,
	{0,1,1,2,-1,2},3,2,3,0,2,10,12,10},

	//r恐獄
	{2,_("安産祈願の神様", "Delivery God"),
	120,30,145,
	{-2,1,3,-2,3,2},3,4,9,1,3,10,0,0},

	//r火　ファイアボール
	{2,_("花火職人の神様", "Fireworks God"),
	70,150,120,
	{1,1,0,1,1,0},3,10,0,0,0,10,0,20},

	//急遅　食料生成
	{2,_("シェフの神様", "Chef God"),
	120, 80,150,
	{2,2,-2,2,-1,1},10,5,-2,1,5,10,12,0},

	//r火盲
	{2,_("かまどの神様", "Kamado God"),
	90, 100,80,
	{1,1,0,0,2,1},2,5,2,2,3,10,5,0},

	//(腕耐
	{2,_("筋トレの神様", "Training God"),
	160, 150,80,
	{5,0,-2,0,2,-1},0,0,-2,1,0,10,20,0},

	//モンスター感知
	{2,_("狼の神様", "Wolf God"),
	100, 140,60,
	{2,-2,-1,2,1,-2},0,-2,-2,3,4,10,10,0},

	//r水
	{2,_("大滝の神様", "Waterfall God"),
	80, 130,90,
	{2,-1,0,0,1,0},0,2,4,3,1,2,10,10},

	//r水劣
	{2,_("治水の神様", "Flood God"),
	140, 80,150,
	{1,1,2,1,3,1},0,2,7,3,1,2,10,10},

	//r閃暗
	{2,_("夕焼けの神様", "Sunset God"),
	70, 100,85,
	{0,0,1,1,1,1},0,2,7,3,2,2,3,3},


	//視 完全ESP
	{3,_("狩猟神", "Pursuit God"),
	80, 180,110,
	{3,1,0,3,2,1},7,2,2,4,10,10,10,20},

	//r暗狂視
	//幻惑
	{3,_("月の神", "Moon God"),
	80, 90,120,
	{1,2,1,2,3,3},3,12,12,3,0,3,5,8},

	//r麻視沌時
	//次元の扉
	{3,_("夢の神", "Dream God"),
	30, 60,150,
	{-1,2,4,1,2,3},3,5,10,2,0,3,3,3},

	//r火閃
	//武器強化、防具強化
	{3,_("鍛冶神", "Blacksmith God"),
	150, 140,80,
	{3,2,-1,3,2,1},6,12,3,1,0,7,22,3},

	//78
	//r暗
	//素材感知
	{2,_("鉱山の神", "Mining God"),
	100, 120,100,
	{2,0,-1,1,2,0},3,5,5,2,6,3,8,2},

	//79 瓔花用
	//r水獄;視
	{ 1,_("水子霊のリーダー", "Mizuko Spirit"),
		180,50,200,
		{ -1,-1,1,1,-1,1 },4,0,-3,2,6,0,-10,5 },

//今後増やすにしても125種類までにしておく

	{0,_("終端用ダミー", "terminator dummy"),0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0,0}
};

///mod150315 妖精用特技実装
/*:::妖精能力表　配列添字をp_ptr->race_multipur_val[0]に保存する*/
const fairy_type fairy_table[FAIRY_TYPE_MAX+2] =
{
	{100,_("始端ダミー兼能力付与なし", "starting dummy - no ability")}, //ユニーククラスなどはこの値になる
//1
	{20,_("ドアを開かなくする程度の能力", "Capable of making doors unopenable")},
	{40,_("ときどきラッキー避けする程度の能力", "Capable of occasional lucky dodges")}, //ボルトを中確率で無効化
	{30,_("キノコを見分ける程度の能力", "Capable of telling mushrooms apart")},
	{90,_("猫に好かれる程度の能力", "Capable of being loved by cats")}, //fシンボルが友好的
	{65,_("暑い所で元気が出る程度の能力", "Capable of being energetic in warm places")},

	{80,_("壁をドアに変える程度の能力", "Capable of turning walls into doors")},
	{90,_("巨大な敵から見つかりにくい程度の能力", "Capable of hiding from gigantic enemies")},
	{55,_("花を持っていると強くなる程度の能力", "Capable of being stronger while holding a flower")}, //武器「花」のsides+3
	{50,_("財布の紐が固い程度の能力", "Capable of having a strong purse string")},//金を盗まれない
	{80,_("竜言語で子守歌を歌う程度の能力", "Capable of singing lullabies in dragon tongue")},//眠らない敵にも有効？
//11
	{30,_("逃げ足が速い程度の能力", "Capable of running away quickly")},//HPが1/2以下のとき高速移動
	{80,_("眠ると記憶を失う程度の能力", "Capable of losing memories when falling asleep")},
	{70,_("寝坊を誘う程度の能力", "Capable of inducing oversleeping")},
	{50,_("ゴーウェストする程度の能力", "Capable of going west")},
	{40,_("隣接した敵と位置交換する程度の能力", "Capable of swapping positions with adjacent enemies")},

	{50,_("おまじないが得意な程度の能力", "Capable at charms and wards")},//魔法防御+
	{50,_("かくれんぼが得意な程度の能力", "Capable at playing hide-and-seek")},//隠密+
	{40,_("宝探しが得意な程度の能力", "Capable at searching for treasure")}, //探索+
	{30,_("ダーツが得意な程度の能力", "Capable at playing darts")}, //射撃+
	{20,_("パズルが得意な程度の能力", "Capable at solving puzzles")}, //解除+
//21
	{50,_("同胞の気配を感知する程度の能力", "Capable of detecting fellow fairies")},//妖精感知
	{60,_("屋外で速く飛べる程度の能力", "Capable of quickly flying while outdoors")},//ワールドマップで移動速度が速い
	{70,_("水鉄砲が得意な程度の能力", "Capable at using water guns")},//低威力ウォーターボルト　水地形のみ
	{95,_("3グリッド以内の狙った場所にテレポートする程度の能力", "Capable of teleporting to a specified location within 3 grids")},
	{30,_("光る程度の能力", "Capable of producing light")}, //ライトエリア

	{60,_("近くのツタや木の枝を操る程度の能力", "Capable at manipulating nearby vines and branches")},//隣接足がらめ
	{30,_("植物に囲まれているとちょっと元気が出る程度の能力", "Capable of being more energetic while surrounded by plants")},
	{45,_("放電する程度の能力", "Capable of discharging electricity")}, //短射程電撃ビーム
	{30,_("催眠術が得意な程度の能力", "Capable at hypnotism")},//スリープモンスター
	{50,_("月の光を浴びると元気が出る程度の能力", "Capable of being energetic while in moonlight")},//屋外夜間でパワーアップ
//31
	{90,_("妖精社会で顔が広い程度の能力", "Capable of being popular in fairy society")}, //妖精が友好的
	{40,_("泳ぎが得意な程度の能力", "Capable at swimming")}, //水上高速移動
	{80,_("近くにカビを生やす程度の能力", "Capable of making moss grow nearby")},//友好的mシンボル召喚
	{60,_("短距離テレポートをする程度の能力", "Capable of teleporting short distances")}, //ショートテレポ
	{35,_("犬を手懐ける程度の能力", "Capable of taming dogs")}, //Cシンボルにのみ有効な強力なチャームモンスター

	{30,_("蛇を操る程度の能力", "Capable of manipulating snakes")}, //Jシンボルにのみ有効な強力なチャームモンスター
	{65,_("光合成をする程度の能力", "Capable of photosynthesis")},//昼間の屋外で満腹度が増える。閃光ブレスでも増える。(上限処理要)
	{60,_("周囲の水場を感知する程度の能力", "Capable of detecting bodies of water")},
	{70,_("水上で元気が出る程度の能力", "Capable of being energetic while in water")},
	{80,_("重力攻撃を中和する程度の能力", "Capable of neutralizing gravity attacks")},
//41
	{95,_("岩石の隙間を潜り抜ける程度の能力", "Capable of slipping through gaps in stone")},
	{45,_("幻影を出す程度の能力", "Capable of conjuring illusions")},//ターンモンスター
	{75,_("木の上で元気が出る程度の能力", "Capable of being energetic in forest")},
	{50,_("テレポート・バックからすり抜ける程度の能力", "Capable of slipping through 'teleport to' spells")},
	{60,_("酒に強い程度の能力", "Capable at handling alcohol")},//泥酔度が一定以上に進みにくい？

	{70,_("幻覚状態になると大幅にパワーアップする程度の能力", "Capable of greatly powering up while hallucinating")},
	{20,_("少しだけ弾幕を放つ程度の能力", "Capable of shooting weak danmaku")},//ダメージ=plev/2 消費1
	{70,_("それなりに弾幕を放つ程度の能力", "Capable of shooting danmaku")}, //ダメージ=plev 消費2
	{80,_("鳥寄せが得意な程度の能力", "Capable of taming birds")},//Bシンボルに効きやすいチャームモンスター
	{100,_("ダミー", "Dummy")},// 旧バージョンのキャラ(クイックスタート含む)はp_ptr->scが50のはずなので未適応の判定にする
//51
	{85,_("地面に穴を開ける程度の能力","Capable of opening chasms in the ground")},//足元の地面を深い穴にする。アイテムなどがあるとできない。騎乗中でもできない。
	{60,_("植物を成長させる程度の能力", "Capable of making plants grow")},//花や草地形が木になる
	{70,_("光線を逸らす程度の能力", "Capable of deflecting beams of light")}, //レーザーが効かない。巨大レーザーや閃光ブレスは効く。
	{90,_("隕石をグレイズする程度の能力", "Capable of grazing meteors")},//GF_METEORが効かない
	{90,_("つむじ風を起こす程度の能力", "Capable of causing whirlwinds")},

//	{0,"程度の能力"},
//	{0,"程度の能力"},

//風に飛ばされて切ないことになる程度の能力　竜巻を受けると大きくテレポート

//友達の場所を知る：friendly感知？
//	{0,"花の蜜を集める程度の能力"},//花地形上で「キャンディ」を作る？MP微回復？
	{100,_("終端ダミー", "terminator dummy")},
	{100,_("終端ダミー", "terminator dummy")},
};


/*:::妖精専用レイシャルパワー情報部*/
void fairy_racial_power(power_desc_type *power_desc, int *num_return)
{

	int num = *num_return;

	if(p_ptr->prace != RACE_FAIRY)
	{
		msg_format(_("ERROR:fairy_racial_power()が妖精以外のとき呼ばれた",
                    "ERROR: fairy_racial_power() called for a non-fairy"));
		return;
	}

	switch(CHECK_FAIRY_TYPE)
	{
	case 1:
		strcpy(power_desc[num].name, _("施錠", "Lock Doors"));
		power_desc[num].level = 1;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 10;
		power_desc[num].info = _("閉じられたドア一つを固定するビームを放つ。固定されたドアは開錠できないが叩き壊すことはできる。",
                                "Fires a beam that locks closed doors. Locked door cannot be unlocked, but can be bashed down.");
		power_desc[num++].number = -1;
		break;

	case 3:
		strcpy(power_desc[num].name, _("キノコ鑑定", "Identify Mushrooms"));
		power_desc[num].level = 1;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num].info = _("キノコの種類を判別する。", "Tells different mushrooms apart.");
		power_desc[num++].number = -2;
		break;

	case 6:
		strcpy(power_desc[num].name, _("ドア変化", "Change to Door"));
		power_desc[num].level = 12;
		power_desc[num].cost = 8;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 25;
		power_desc[num].info = _("壁をドアに変化させる。", "Changes a wall into a door.");
		power_desc[num++].number = -3;
		break;

	case 10:
		strcpy(power_desc[num].name, _("子守歌(ドラゴン語)", "Lullaby (dragon tongue)"));
		power_desc[num].level = 20;
		power_desc[num].cost = 12;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 30;
		power_desc[num].info = _("周囲のドラゴンを眠らせる。通常眠らせられない相手にも効くがユニークモンスターには効かない。",
                                "Sleeps nearby dragons. Works against opponents that normally resist sleep, but doesn't work on unique monsters.");
		power_desc[num++].number = -4;
		break;


	case 13:
		strcpy(power_desc[num].name, _("安眠ケア", "Good Sleep"));
		power_desc[num].level = 10;
		power_desc[num].cost = 7;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 20;
		power_desc[num].info = _("隣接した眠っているモンスター一体をより深く眠らせる。",
                                "Makes one adjacent sleeping monster sleep for longer.");
		power_desc[num++].number = -6;
		break;

	case 14:
		strcpy(power_desc[num].name, "Go West!");
		power_desc[num].level = 3;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 10;
		power_desc[num].info = _("マップ左方向へ向けて吹き飛ぶ。障害物に当たると止まる。",
                                "Charges to the left side of the map. You stop if you hit an obstacle.");
		power_desc[num++].number = -7;
		break;
	case 15:
		strcpy(power_desc[num].name, _("隣接位置交換", "Adjacent Swap Positions"));
		power_desc[num].level = 3;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 20;
		power_desc[num].info = _("隣接した敵と位置を交換する。", "Swaps positions with an adjacent enemy.");
		power_desc[num++].number = -8;
		break;

	case 23:
		strcpy(power_desc[num].name, _("水鉄砲", "Water Gun"));
		power_desc[num].level = 3;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("威力1d(レベル)の水属性ボルトを放つ。近くに水がないと使えない。",
                                "Fires a bolt of water (damage 1d(level)). Cannot be used unless there's water nearby.");
		power_desc[num++].number = -9;
		break;

	case 24:
		strcpy(power_desc[num].name, _("ショート・次元の扉", "Short Dimension Door"));
		power_desc[num].level = 20;
		power_desc[num].cost = 9;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 30;
		power_desc[num].info = _("距離3以内の狙ったところにテレポートする。失敗すると全くテレポートしない。",
                                "Teleports to specified location within 3 squares. Does not teleport at all if it fails.");
		power_desc[num++].number = -10;
		break;
	case 25:
		strcpy(power_desc[num].name, _("発光", "Emit Light"));
		power_desc[num].level = 1;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CON;
		power_desc[num].fail = 5;
		power_desc[num].info = _("明るく光って自分の居る部屋を照らす。",
                                "Illuminates nearby area.");
		power_desc[num++].number = -11;
		break;
	case 26:
		strcpy(power_desc[num].name, _("植物操作", "Control Vegetation"));
		power_desc[num].level = 1;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 15;
		power_desc[num].info = _("隣接する敵を減速させる。空を飛ぶ敵、力強い敵、ユニークには効果が薄い。自分が木の上に居ないと使えない",
                                "Slows an adjacent enemy. Less effective against flying, powerful or unique enemies. Cannot be used unless you are in woods.");
		power_desc[num++].number = -12;
		break;
	case 28:
		strcpy(power_desc[num].name, _("放電", "Discharge"));
		power_desc[num].level = 3;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_CON;
		power_desc[num].fail = 10;
		power_desc[num].info = _("威力1d(レベル*2)の電撃属性ボルトを放つ。",
                                "Fires a bolt of lightning. (damage 1d(level * 2))");
		power_desc[num++].number = -13;
		break;
	case 29:
		strcpy(power_desc[num].name, _("催眠術", "Hypnotism"));
		power_desc[num].level = 2;
		power_desc[num].cost = 4;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("敵一体を眠らせる。", "Tries to put an enemy to sleep.");
		power_desc[num++].number = -22;
		break;

	case 33:
		strcpy(power_desc[num].name, _("カビ増殖", "Grow Mold"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 15;
		power_desc[num].info = _("友好的なmシンボルのモンスターを召喚する。", "Summon friendly 'm' monsters.");
		power_desc[num++].number = -14;
		break;

	case 34:
		strcpy(power_desc[num].name, _("瞬き移動", "Blink Movement"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num].info = _("距離10程度のランダムな位置にテレポートする。", "Teleports to a random position within 10 squares.");
		power_desc[num++].number = -15;
		break;

	case 35:
		strcpy(power_desc[num].name, _("犬馴らし", "Tame Dogs"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("隣接したCシンボルのモンスター一体を魅了し配下にする。",
                                "Charms an adjacent 'C' monster into being your follower.");
		power_desc[num++].number = -16;
		break;
	case 36:
		strcpy(power_desc[num].name, _("蛇使い", "Snake Whisperer"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("隣接したJシンボルのモンスター一体を魅了し配下にする。",
                                "Charms an adjacent 'J' monster into being your follower.");
		power_desc[num++].number = -17;
		break;
	case 42:
		strcpy(power_desc[num].name, _("幻影", "Illusion"));
		power_desc[num].level = 1;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 10;
		power_desc[num].info = _("敵一体を恐怖させる。", "Terrifies an enemy.");
		power_desc[num++].number = -18;
		break;
	case 47:
		strcpy(power_desc[num].name, _("小弾幕", "Weak Danmaku"));
		power_desc[num].level = 1;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("威力1d(レベル)の無属性ボルトを放つ。", "Fires a non-elemental bolt. (damage: 1d(level))");
		power_desc[num++].number = -19;
		break;
	case 48:
		strcpy(power_desc[num].name, _("弾幕", "Danmaku"));
		power_desc[num].level = 5;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 15;
		power_desc[num].info = _("威力=レベルの無属性ボルトを放つ。", "Fires a non-elemental bolt. (damage: level)");
		power_desc[num++].number = -20;
		break;
	case 49:
		strcpy(power_desc[num].name, _("鳥寄せ", "Tame Birds"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("Bシンボルのモンスターを魅了し配下にする。",
                                "Charms a 'B' monster into being your follower.");
		power_desc[num++].number = -21;
		break;
	case 51:
		strcpy(power_desc[num].name, _("陥没", "Create Chasm"));
		power_desc[num].level = 10;
		power_desc[num].cost = 7;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 25;
		power_desc[num].info = _("自分の居る場所を深い穴にする。アイテムや特殊地形上では使えない。",
                                "Creates a deep chasm on your position. Cannot be used on items or special terrain.");
		power_desc[num++].number = -23;
		break;
	case 52:
		strcpy(power_desc[num].name, _("成長促進", "Rapid Growth"));
		power_desc[num].level = 16;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 35;
		power_desc[num].info = _("今いる地形が草地か花のとき地形を木に変える。",
                                "If current terrain has flowers or grass, turn it into a tree.");
		power_desc[num++].number = -24;
		break;
	case 55:
		strcpy(power_desc[num].name, _("つむじ風", "Whirlwind"));
		power_desc[num].level = 15;
		power_desc[num].cost = 16;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 25;
		power_desc[num].info = _("周囲の敵にレベル/2のダメージを与え吹き飛ばす。巨大な敵には効果が薄い。",
                                "Deals damage (level / 2) to nearby enemies and blows them away. Less effective against gigantic enemies.");
		power_desc[num++].number = -26;
		break;

	default:
		break;

	}

	*num_return = num;

}

/*:::妖精用レイシャルパワー実行部　行動順消費するときTRUE*/
bool fairy_racial_power_aux(s32b command)
{
	int dir;
	int plev = p_ptr->lev;
	if(p_ptr->prace != RACE_FAIRY)
	{
		msg_format(_("fairy_racial_power_aux()が妖精以外から呼ばれた", "fairy_racial_power_aux() was called for someone other than a fairy"));
		 return FALSE;
	}

	switch(command)
	{
		case -1: //施錠
		{
			if (!get_aim_dir(&dir)) return FALSE;
			wizard_lock(dir);
		}
		break;
		case -2: //キノコ鑑定
		{
			if (!ident_spell_2(1)) return FALSE;
		}
		break;
		case -3: //ドア変化
		{
			int x,y;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 0, GF_WALL_TO_DOOR, PROJECT_GRID, -1);
		}
		break;
		case -4: //子守歌
		{
			msg_print(_("あなたはドラゴンの気持ちになって子守唄を歌った・・",
                        "You think of yourself as a dragon and sing a lullaby..."));
			project_hack(GF_STASIS_DRAGON, plev);
		}
		break;

		case -6:  //周囲の敵の睡眠値上昇
		{
			int x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if(!MON_CSLEEP(m_ptr))
			{
				msg_format(_("あなたの特技は眠っている者にしか効果がない。",
                            "Your special ability works only on those who are already asleep."));
			}
			else if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				msg_format(_("%sは深い寝息を立て始めた・・", "%s starts snoring loudly..."),m_name);
				if(m_ptr->mtimed[MTIMED_CSLEEP]<5000) m_ptr->mtimed[MTIMED_CSLEEP] += 200;
			}
			else
			{
				msg_format(_("そこには何もいない。", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -7: //西へ吹き飛ぶ
		{
			go_west();
		}
		break;
		case -8: //隣接位置交換
		{
			int x,y;
			monster_type *m_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				teleport_swap(dir);
			}
			else
			{
				msg_format(_("そこには何もいない。", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -9: //水鉄砲
		{
			if(!cave_have_flag_bold(py,px,FF_WATER))
			{
				msg_print(_("ここには水がない。", "There's no water here."));
				return FALSE;
			}

			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("水鉄砲を放った。", "You fire a water gun."));
			fire_bolt(GF_WATER, dir, randint1(plev));
		}
		break;
		case -10: //ミニ次元の扉
		{
			msg_print(_("距離3以内の目的地を選んで下さい。", "Select destination within 3 squares."));
			if (!dimension_door(D_DOOR_MINI)) return FALSE;
			break;
		}
		break;
		case -11: //発光
		{
			msg_print(_("あなたは眩しく光った。", "You shine brightly."));
			lite_area(0, 0);
		}
		break;
		case -12: //足がらめ
		{
			if(!cave_have_flag_bold(py,px,FF_TREE))
			{
				msg_print(_("ここには木がない。", "There's no trees here."));
				return FALSE;
			}
				msg_print(_("あなたは蔦や木の枝を操った！", "You manipulate vines and branches!"));
				project(0,1,py,px,plev * 2+20,GF_SLOW_TWINE, PROJECT_KILL | PROJECT_HIDE,-1);
		}
		break;
		case -13: //放電
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("電撃を放った！", "You discharge electricity!"));
			fire_bolt(GF_ELEC, dir, randint1(plev*2));
		}
		break;
		case -14: //カビ増殖
		{
			int i;
			for (i = randint1(3); i > 0; i--)
			{
				summon_specific(-1, py, px, plev, SUMMON_BIZARRE1, PM_FORCE_FRIENDLY);
			}
		}
		break;

		case -15: //ショートテレポ
		{
			teleport_player(10L, 0);
		}
		break;

		case -16: //犬馴らし
		case -17: //蛇使い
		case -21: //鳥寄せ
		{
			int x,y;
			int power = plev + 25;
			monster_type *m_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				if(command == -16) project(0,0,y,x,power,GF_CHARM_C,PROJECT_HIDE | PROJECT_KILL,-1);
				else if(command == -17) project(0,0,y,x,power,GF_CHARM_J,PROJECT_HIDE | PROJECT_KILL,-1);
				else  project(0,0,y,x,power,GF_CHARM_B,PROJECT_HIDE | PROJECT_KILL,-1);
			}
			else
			{
				msg_format(_("そこには何もいない。", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -18: //幻影
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("恐ろしげな幻影を作り出した・・", "You create a frightening illusion..."));
			fear_monster(dir, plev+20);
		}
		break;
		case -19: //弾幕
		case -20:
		{
			int dam;
			if(command == -19) dam = randint1(plev);
			else dam = plev;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("弾幕を放った！", "You shoot danmaku!"));
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
		case -22: //催眠術
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("指をぐるぐる回した・・", "You spin your finger in circles..."));
			sleep_monster(dir, plev+10);
		}
		break;
		case -23: //陥没
		{
			if(!cave_clean_bold(py, px))
			{
				msg_format(_("そこに穴を開けることはできない。", "You can't open a chasm here."));
				return FALSE;
			}
			msg_format(_("足元に大穴を開けた！", "A large chasm opens up beneath you!"));
			cave_set_feat(py, px, feat_dark_pit);
			p_ptr->redraw |= (PR_MAP);

		}
		break;
		case -24: //成長促進
		{
			if(!cave_have_flag_bold(py, px, FF_PLANT))
			{
				msg_format(_("ここには植物がない。", "There's no plants here."));
				return FALSE;
			}
			else if(cave_have_flag_bold(py, px, FF_TREE))
			{
				msg_format(_("すでに木の上だ。", "There's already a tree here."));
				return FALSE;
			}
			msg_format(_("足元の植物がみるみるうちに育っていく！","The plants beneath you start growing rapidly!"));
			cave_set_feat(py, px, feat_tree);
			p_ptr->redraw |= (PR_MAP);


		}
		break;
		case -25: //隆起
		{


		}
		break;
		case -26: //つむじ風
		{
			msg_print(_("旋風を起こした！", "You summon a whirlwind!"));
			project(0, 1, py, px, plev, GF_TORNADO, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;

		default:
		msg_format(_("fairy_racial_power_aux()に不正なコマンド値(%d)が渡された",
                    "Incorrent command value (%d) passed to fairy_racial_power_aux()"),command);
		return FALSE;
	}

	return TRUE;
}

///mod150315 妖精用特性を得る
/*:::memo
 *妖精特技はfairy_table[]のどれかが選ばれ、テーブルのインデックス値がp_ptr->race_multipur_val[0]に格納される。
 *また、クイックスタート時にこの能力が保持されるようにp_ptr->scとprevious_char.scにも同じ値を保存しておく。
 *クイックスタート後にここから値を読んできてp_ptr->race_multipur_val[0]に格納し直す。
 */
void get_fairy_type(bool birth)
{
	int new_type;

	//非ユニーククラス妖精のみ設定する
	if(p_ptr->prace != RACE_FAIRY) return;
	if(cp_ptr->flag_only_unique) return;

	//レアリティに応じて表から選ぶ
	do
	{
		new_type = randint1(FAIRY_TYPE_MAX);
	}while(fairy_table[new_type].rarity >= randint1(100));
		//v1.1.31c上の式の >を>=にした。1%の確率でダミーが判定を抜けてしまう

	p_ptr->race_multipur_val[0] = new_type;


	//妖精でキャラメイク時、もしくは妖精でプレイ中にバージョンアップした場合
	//if(birth && previous_char.prace == RACE_FAIRY)
	//v1.1.31c修正 生まれつきでなく種族変容で妖精になったときに妖精特技idxが保存されたsc(社会的地位)値がいじられないように判定付けてたが不具合のもとになったので消した。そういうときには条件式によりここに来ないので大丈夫なはず？
	if(birth)
	{
		p_ptr->sc = new_type;
		previous_char.sc = new_type; //クイックスタート時に同じ能力になるよう保存
	}

}

//v1.1.32 EXモード専用　野良神様にランダムな徳性値を設定する。
bool straygod_job_adviser(void)
{
	int rank;
	int new_idx;
	int dummy_idx=-1;
	int i;
	int cnt;
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;

	if (p_ptr->prace != RACE_STRAYGOD)
	{
		msg_print(_("あなたは野良神様ではないので関係ないようだ。",
                    "You're not a stray god, so this has nothing to do with you."));
		return FALSE;
	}
	else if (p_ptr->max_plv > 44)
	{
		msg_print(_("あなたの在り方はもう揺らぐことはない。",
                    "You're not going to change the way you are anymore."));
		return FALSE;
	}
	else if (ex_buildings_param[ex_bldg_num] == 255)
	{
		msg_print(_("分社はもう問いかけに答えない。", "You call out to the branch shrine, but nobody answers."));
		return FALSE;
	}

	if (p_ptr->max_plv > 29) rank = 3;
	else rank = 2;

	clear_bldg(4, 18);

	prt(_("八坂の神があなたの身の振り方について助言してくれるらしい。",
        "Goddess Yasaka is willing to give you advice on how to develop further."), 5, 20);

	if (!get_check_strict(_("聞き入れますか？", "Do you listen? "), CHECK_OKAY_CANCEL))
	{
		clear_bldg(4, 18);
		return FALSE;
	}

	clear_bldg(4, 18);
	prt(_("分社の扉の中から声が響いてくる...",
            "A voice echoes from beyond the door of branch shrine..."), 6, 20);
	inkey();

	cnt = 0;

	for (i = 0; deity_table[i].rank != 0; i++)
	{
		if (rank != deity_table[i].rank) continue;
		cnt++;
		if (one_in_(cnt)) new_idx = i;
	}
	cnt = 0;
	for (i = 0; deity_table[i].rank != 0; i++)
	{
		if (rank != deity_table[i].rank) continue;
		if (new_idx == dummy_idx) continue;
		cnt++;
		if (one_in_(cnt)) dummy_idx = i;
	}

	switch (randint1(10))
	{
	case 1: case 2: case 3: case 4: case 5:
    #ifdef JP
		prt(format("「あなたは%sを目指すのです。", deity_table[new_idx].deity_name), 7, 22);
		prt(format("さすれば人々の信仰篤く、末永き繁栄を得られることでしょう。」"), 8, 22);
    #else
        prt(format("'You should aim to become a %s.", deity_table[new_idx].deity_name), 7, 22);
		prt(format("If you do so, your amount of worshippers will increase, "), 8, 22);
		prt(format("and you shall be prosperous for aeons to come.'"), 9, 22);
    #endif
		break;
	case 6: case 7:
    #ifdef JP
		prt(format("「ふむふむ、嫌じゃなければ%sはどうかしら？", deity_table[new_idx].deity_name), 7, 22);
		prt(format("この辺りって実は案外ブルーオーシャンだと思うのよね。」"), 8, 22);
    #else
        prt(format("'Hmm, hmm, how does being a %s sound?", deity_table[new_idx].deity_name), 7, 22);
		prt(format("I think that's a surprisingly promising position.'"), 8, 22);
    #endif
		break;
	case 8: case 9:
    #ifdef JP
		prt(format("「うーん……"), 7, 22);
		prt(format("いっそ%sになったほうが信仰が集まるんじゃない？」", deity_table[new_idx].deity_name), 8, 22);
    #else
        prt(format("'Umm......"), 7, 22);
		prt(format("Wouldn't you be able to gather more faith if you become a %s?'", deity_table[new_idx].deity_name), 8, 22);
    #endif
		break;
	default:
    #ifdef JP
		prt(format("「あなたは%sを目指すのです。さすれば・・", deity_table[new_idx].deity_name), 7, 22);
		prt(format("「やれやれ分かってないねぇ。絶対%sのほうがいいって！」", deity_table[dummy_idx].deity_name), 8, 22);
		prt(format("「何ですって？私の仕事に横から口を出すんじゃないわよ。」"), 9, 22);
		prt(format("「よーし、久々にやるかい？」"), 10, 22);
		prt("", 11, 22);
		if (one_in_(2)) new_idx = dummy_idx;
		prt(format("弾幕決闘の末、%sを目指すのがよいだろうということになった。", deity_table[new_idx].deity_name), 12, 22);
    #else
        prt(format("'You should aim to become a %s. If you do so...'", deity_table[new_idx].deity_name), 7, 22);
		prt(format("'You just don't get it, do you? Being a %s is way better!'", deity_table[dummy_idx].deity_name), 8, 22);
		prt(format("'What? Are you telling me how to do my job?'"), 9, 22);
		prt(format("'Well.. We haven't fought for a long time, did we?'"), 10, 22);
		prt("", 11, 22);
		if (one_in_(2)) new_idx = dummy_idx;
		prt(format("As the result of the danmaku battle, you were told to become a %s.", deity_table[new_idx].deity_name), 12, 22);
    #endif
		break;

	}

	//神格値リセット
	p_ptr->race_multipur_val[DBIAS_COSMOS] = deity_table[new_idx].dbias_cosmos;
	p_ptr->race_multipur_val[DBIAS_WARLIKE] = deity_table[new_idx].dbias_warlike;
	p_ptr->race_multipur_val[DBIAS_REPUTATION] = deity_table[new_idx].dbias_reputation;

	inkey();
	clear_bldg(4, 18);

	ex_buildings_param[ex_bldg_num] = 255;
	return TRUE;

}

//v1.1.70 種族「動物霊」の所属勢力を決定する
//キャラメイク時とクイックスタート時に呼ばれる
//クイックスタート時にキャラメイク時と同じ値にするため、妖精の特技同様にp_ptr->sc(社会的地位)を使ってしのぐ
void get_animal_ghost_family(bool birth)
{

	int tmp;

	if (p_ptr->prace != RACE_ANIMAL_GHOST) return;

	//キャラメイク時はランダムに設定し決定値をp_ptr->scに保存
	if (birth)
	{
		//ユニーククラスは固定
		if (p_ptr->pclass == CLASS_SAKI)
			tmp = ANIMAL_GHOST_STRIFE_KEIGA;
		else 	if (p_ptr->pclass == CLASS_YACHIE)
			tmp = ANIMAL_GHOST_STRIFE_KIKETSU;
		else
		{
			switch (randint1(10))
			{
			case 1: case 2: case 3:
				tmp = ANIMAL_GHOST_STRIFE_KEIGA;
				break;
			case 4: case 5: case 6:
				tmp = ANIMAL_GHOST_STRIFE_KIKETSU;
				break;
			case 7: case 8: case 9:
				tmp = ANIMAL_GHOST_STRIFE_GOUYOKU;
				break;

			default:
				tmp = ANIMAL_GHOST_STRIFE_NONE;
				break;
			}
		}
		p_ptr->sc = tmp;
		previous_char.sc = tmp;
	}
	//クイックスタート時はp_ptr->scに保存されている値をロールバック
	else
	{
		tmp = p_ptr->sc;
	}

	//種族用変数に記録
	p_ptr->race_multipur_val[0] = tmp;

}



