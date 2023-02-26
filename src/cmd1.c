/* File: cmd1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 1) */

#include "angband.h"
#define MAX_VAMPIRIC_DRAIN 50

//v1.1.94 理力が効いたかどうか確認するためのグローバルフラグ
bool gflag_force_effect = FALSE;





//v1.1.94
//刀の特殊クリティカル「必殺の一太刀」が出るかどうかの判定
//刀を装備していることは外で判定済みであること
//r_ptr:ターゲット種族
//mode:HISSATSU_XXXなどの攻撃モード
bool check_katana_critical(monster_type *m_ptr, int mode)
{

	int power;
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	int katana_skill_exp = ref_skill_exp(TV_KATANA);

	if (randint1(katana_skill_exp) < 3200) return FALSE;

	//見えない敵には出ない
	if (!m_ptr->ml) return FALSE;

	//lev40:178 lev50:210
	power = p_ptr->lev + katana_skill_exp / 50;

	//寝ている敵や朦朧としている敵には出やすい
	if (MON_CSLEEP(m_ptr) || MON_STUNNED(m_ptr)) power += 60;

	//二刀流の時出づらい
	if (p_ptr->migite && p_ptr->hidarite) power -= 30;

	//巨大な敵には出づらい
	if (r_ptr->flags2 & RF2_GIGANTIC) power -= 30;

	//普通の生物の形をしていない敵には出づらい
	if (r_ptr->flags1 & RF1_SHAPECHANGER || r_ptr->flags2 & RF2_PASS_WALL || !my_strchr("abcdfghklnopqrstuwyzABCDFGHIJKLMOPRSTUVWXYZ", r_ptr->d_char))
		power -= 30;

	if (power <= 0) return FALSE;

	switch (mode)
	{
		//捨て身や居合の型のとき出やすい
	case HISSATSU_IAI:
	case HISSATSU_SUTEMI:
	case HISSATSU_SEKIRYUKA:
	case HISSATSU_UNDEAD:
	case HISSATSU_MAJIN: //刀では使えないが将来なにか仕様変えたときのために
	case HISSATSU_ATTACK_ONCE:
		power *= 2;
		break;

		//普通の攻撃や属性攻撃系は修正なし
	case HISSATSU_NONE:
	case HISSATSU_FIRE:
	case HISSATSU_COLD:
	case HISSATSU_POISON:
	case HISSATSU_ELEC:
	case HISSATSU_DRAIN:
	case HISSATSU_DOUBLE:
	case HISSATSU_ZANMA:
	case HISSATSU_QUAKE:
	case HISSATSU_COUNTER:
		break;

		//遠距離からの攻撃は出づらい
	case HISSATSU_ISSEN:
	case HISSATSU_2:
	case HISSATSU_NYUSIN:
	case HISSATSU_HARAI:
	case HISSATSU_TRIANGLE:
	case HISSATSU_KAPPA:
		power -= 50;
		break;

		//ほか広範囲攻撃など特殊な技では出ない
	default:
		return FALSE;
	}

	if (cheat_xtra) msg_format("katana critical:power%d/rlev%d",power,r_ptr->level);

	if (randint1(power) > r_ptr->level) return TRUE;
	else return FALSE;

}



//v1.1.94 モンスターに対する行動遅延
//add_energy:energy_need値に加算される値
bool monster_delay(int m_idx, int add_energy)
{

	if (m_idx <= 0 || m_idx >= m_max)
	{
		msg_format(_("ERROR:monster_delay()に渡されたm_idxがおかしい(%d)",
                    "ERROR: Weird m_idx (%d) passed to monster_delay()"), m_idx);
		return FALSE;
	}

	//200以上加算されない。平均的には一行動に100かかる
	add_energy = (add_energy > 200) ? 200 : (add_energy < 0) ? 0 : add_energy;

	//すでに行動遅延しているモンスターはそのモンスターが再び行動するまで行動遅延を受けない
	if (m_list[m_idx].mflag & MFLAG_ALREADY_DELAYED)
	{
		if (cheat_xtra) msg_format("ALREADY DELAYED - m_idx:%d", m_idx);
		return FALSE;
	}

	if (cheat_xtra) msg_format("DELAY - m_idx:%d energy_need %d -> %d", m_idx, m_list[m_idx].energy_need, m_list[m_idx].energy_need + add_energy);

	m_list[m_idx].energy_need += add_energy;

	m_list[m_idx].mflag |= MFLAG_ALREADY_DELAYED;

	return TRUE;

}







//近接攻撃追加効果の朦朧付与やAC低下などの判定と適用を行う。
//critical_rank:会心や不意打ちなどが決まったとき増える　最大5
//effect_type: ATTACK_EFFECT_??? 混乱や睡眠は処理がややこしくなるので外で別にやっている
//skill_exp:この攻撃に関係する技能の現在値 0~8000
//ref_effect_turns: 効果が持続するターン数 0なら中で適当に決める
void apply_special_effect(int m_idx, int special_effect_type, int critical_rank, int skill_exp, int ref_effect_turns, bool flag_test)
{
	monster_type *m_ptr;
	monster_race *r_ptr;
	int mon_resist = 0;
	int effect_turns = ref_effect_turns;
	char m_name[160];

	bool flag_not_usual_shape = FALSE;

	int power;

	if (m_idx <= 0 || m_idx >= m_max)
	{
		msg_format(_("ERROR:apply_special_effect()に渡されたm_idxがおかしい(%d)",
                    "ERROR: Weird m_idx (%d) passed to apply_special_effect()"), m_idx);
		return;
	}
	if (critical_rank < 0 || critical_rank > 5)
	{
		msg_format(_("ERROR:apply_special_effect()に渡されたcritical_rankがおかしい(%d)",
                    "ERROR: Weird critical_rank (%d) passed to apply_special_effect()"), critical_rank);
		return;
	}

	if (!special_effect_type) return;



	m_ptr = &m_list[m_idx];

	if (!m_ptr->r_idx) return;

	r_ptr = &r_info[m_ptr->r_idx];

	if(flag_test) gv_test1++;//朦朧付与とかの発生率チェック用 関数に入った数

	//低熟練ではクリティカル時除き発生しないことにしておく
	if (!critical_rank && skill_exp < (4000 + randint1(4000))) return;

	if (r_ptr->flagsr & RFR_RES_ALL) return;

	//攻撃タイプごとのモンスターの抵抗力計算
	mon_resist = 100 + r_ptr->level * 2;
	//v1.1.95 防御低下状態なら抵抗力が下がることにした
	if (MON_DEC_DEF(m_ptr)) mon_resist -= mon_resist / 4;

	if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) mon_resist *= 2;
	if (r_ptr->flags2 & RF2_POWERFUL) mon_resist += 200;

	if (r_ptr->flags1 & RF1_SHAPECHANGER || r_ptr->flags2 & RF2_PASS_WALL || !my_strchr("abcdfghklnopqrstuwyzABCDFGHIJKLMOPRSTUVWXYZ", r_ptr->d_char)) flag_not_usual_shape = TRUE;

	switch (special_effect_type)
	{
	case	ATTACK_EFFECT_STUN:

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags3 & RF3_NO_STUN)  mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || !monster_living(r_ptr)) mon_resist += 200;

		break;
	case ATTACK_EFFECT_SLOW:

		//ユニークに減速格闘は効かない
		if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) return;

		if (r_ptr->flags1 & RF1_NEVER_MOVE) mon_resist += 200;

		if (flag_not_usual_shape) mon_resist += 200;

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flagsr & RFR_RES_INER) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_ATK:
		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;

		break;

	case ATTACK_EFFECT_DEC_DEF:
		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_MAG:
		if (r_ptr->flags2 & RF2_SMART) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (r_ptr->flags3 & RF3_DEITY) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_ALL:
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (r_ptr->flags3 & RF3_DEITY) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DELAY:
		//モンスターの行動までに二回以上の行動遅延はかからない
		if (!flag_test && (m_ptr->mflag & MFLAG_ALREADY_DELAYED)) return;

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		break;

	default:
		msg_format(_("ERROR:未定義のspecial_effect_type(%d)",
                    "ERROR: Undefined special_effect_type (%d)"), special_effect_type);
		return;
	}

	//memo
	//旧格闘朦朧付与率
	//if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
	//STUN1,2,3とも付与率は同じで重量や会心も無関係
	//resist_stun値はラーヴァDが187,オベ187,モル275,J407
	//plev50でD19%,オベ16%,モル12.5%,J9%

	//格闘会心確率(plev=50,plus=45,to_h=100)
	//重量8  (     2:10% 3:6% 4:4%)
	//重量12 (     2:4%  3:7% 4:13%)
	//重量16 (           3:4% 4:18% 5:7%)

	//武器会心確率(plev=50,plus=20,to_h=100)
	//重量50  12%	(1:6.5% 2:5.5%                        )
	//重量100 13%	(1:5.5% 2:6.5%                        )
	//重量200 15%	(1:4.6% 2:6.9% 3:3.5%                 )
	//重量400 19%	(       2:8.8% 3:5.8% 4:4.4%          )
	//重量800 27%	(              3:4.1% 4:16.6% 5:6.2%  )
	//重量1k  31%	(                     4:14.3% 5:16.7% )
	//重量2k  51%	(                             5:51.0% )


	//該当熟練レベルが30ならレベル60、40なら80、MAXならレベル100相当の判定になるように
	power = MAX((skill_exp / 80), p_ptr->lev);

	//クリティカルのときモンスターのレジスト値低下
	switch (critical_rank)
	{
	case 1://手応え
		mon_resist = mon_resist * 75 / 100;
		break;
	case 2://かなりの手応え
		mon_resist = mon_resist * 60 / 100;
		break;
	case 3://会心
		mon_resist = mon_resist * 40 / 100;
		break;
	case 4://最高の会心
		mon_resist = mon_resist * 25 / 100;
		break;
	case 5://比類なき最高の会心
		mon_resist = mon_resist * 10 / 100;
		break;
	}

	if (cheat_xtra) msg_format("special_effect%d: pow:%d/resist:%d",special_effect_type,power,mon_resist);

	if (flag_test) gv_test3 += MIN(1000, (power * 1000 / mon_resist));//朦朧付与とかの発生率チェック用 成功率の値を合計してあとで平均を出す　なんかこの計算の仕方おかしい気もする

	//判定に失敗すると終了
	if (power <= randint1(mon_resist)) return;

	if (flag_test) gv_test2++;//朦朧付与とかの発生率チェック用 判定を通った数

	if (effect_turns <= 0) effect_turns = 4 + randint1(4);

	monster_desc(m_name, m_ptr, 0);

	//判定に通ったので追加効果付与処理
	switch (special_effect_type)
	{

	case	ATTACK_EFFECT_STUN:
		if (set_monster_stunned(m_idx, MON_STUNNED(m_ptr) + effect_turns))
			msg_format(_("%^sはフラフラになった。", "%^s is dazed."), m_name);
		else
			msg_format(_("%^sはさらにフラフラになった。", "%^s is even more dazed."), m_name);

		break;
	case ATTACK_EFFECT_SLOW:
		if (set_monster_slow(m_idx, MON_SLOW(m_ptr) + effect_turns))
			msg_format(_("%^sの動きが遅くなった。", "%^s starts moving slower."), m_name);

		break;

	case ATTACK_EFFECT_DEC_ATK:

		if(set_monster_timed_status_add(MTIMED2_DEC_ATK,m_idx,effect_turns))
			msg_format(_("%^sの攻撃力が下がった！", "The attack power of %s is decreased!"), m_name);

		break;

	case ATTACK_EFFECT_DEC_DEF:

		if (set_monster_timed_status_add(MTIMED2_DEC_DEF, m_idx, effect_turns))
			msg_format(_("%^sの防御力が下がった！", "The defense power of %s is decreased!"), m_name);

		break;

	case ATTACK_EFFECT_DEC_MAG:

		if (set_monster_timed_status_add(MTIMED2_DEC_MAG, m_idx, effect_turns))
		msg_format(_("%^sの魔法力が下がった！", "The magic power of %s is decreased!"), m_name);
		break;

	case ATTACK_EFFECT_DEC_ALL:
	{
		bool flag_ok = FALSE;
		if (set_monster_timed_status_add(MTIMED2_DEC_ATK, m_idx, effect_turns)) flag_ok = TRUE;
		if (set_monster_timed_status_add(MTIMED2_DEC_DEF, m_idx, effect_turns)) flag_ok = TRUE;
		if (set_monster_timed_status_add(MTIMED2_DEC_MAG, m_idx, effect_turns)) flag_ok = TRUE;

		if(flag_ok)
			msg_format(_("%^sの全能力が下がった！", "All of the abilities of %s are decreased!"), m_name);

		break;

	}
	case ATTACK_EFFECT_DELAY:

		if (monster_delay(m_idx, 25 + randint1(25)))
			msg_format(_("%^sは体勢を崩したようだ。", "The posture of %s looks broken."), m_name);

		break;

	default:
		msg_format(_("ERROR:未定義のspecial_effect_type(%d)",
                    "ERROR: Undefined special_effect_type (%d)"), special_effect_type);
		return;
	}


}


/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
/*:::射撃と投擲の命中判定
 *:::chance:do_cmd_fire_auxなどで計算した命中値
 *:::AC:敵AC スナイパーのとき減衰処理あり
 *:::vis m_ptr->ml */
bool test_hit_fire(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	///seikaku なまけ射撃失敗
	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (FALSE);

	/* Never hit */
	if (chance <= 0) return (FALSE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a monster (normal combat).
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
 /*:::通常攻撃命中判定:::*/
 /*:::chance:命中値 種族職業性格値+(アイテム修正+パラメータ修正+祝福ヒーローその他)*3:::*/
bool test_hit_norm(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	if (cheat_xtra) msg_format("test_hit_norm - chance:%d ac:%d" , chance,ac);

	/* Hack -- Instant miss or hit */
	/*:::もしk<10なら50%の確率でTrueかFalseを返す。命中保証値の5%と絶対失敗率の5%を二行で済ませるテクニックらしい:::*/
	if (k < 10) return (k < 5);



	///seikaku なまけもの命中失敗
	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
/*:::射撃と投擲のクリティカル判定　矢玉/投擲物の重量、命中修正に依存　スナイパーの集中にボーナス*/
s16b critical_shot(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = ((p_ptr->to_h_b + plus) * 4) + (p_ptr->lev * 2);

	/* Snipers can shot more critically with crossbows */
	if (p_ptr->concent) i += ((i * p_ptr->concent) / 5);
	///class スナイパー
	if ((p_ptr->pclass == CLASS_SNIPER) && (p_ptr->tval_ammo == TV_BOLT)) i *= 2;

	//v1.1.74 セイントスターアローを発動効果として復活したがconcentがないのでクリティカル率を少し補正
	if (snipe_type == SP_FINAL)
	{
		i *= 4;
		if (p_ptr->wizard) msg_format("i:%d", i);
	}


	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if (k < 900)
		{
#ifdef JP
			msg_print("手ごたえがあった！");
#else
			msg_print("It was a good hit!");
#endif

			dam += (dam / 2);
		}
		else if (k < 1350)
		{
#ifdef JP
			msg_print("かなりの手ごたえがあった！");
#else
			msg_print("It was a great hit!");
#endif

			dam *= 2;
		}
		else
		{
#ifdef JP
			msg_print("会心の一撃だ！");
#else
			msg_print("It was a superb hit!");
#endif

			dam *= 3;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
/*クリティカルヒットによるdam値補正計算*/
/*weight:武器重量
 *plus:武器ヒット修正値
 *dam:ダイスダメージ
 *meichuu:＠のヒット修正値
 *mode:剣術などの攻撃のとき
 *critical_level クリティカルのとき度合いによって値が入る
 *:::武器が重く武器命中修正が高いほどクリティカルになりやすい。武器が重いほどよいクリティカルになりやすい。
 */
s16b critical_norm(int weight, int plus, int dam, s16b meichuu, int mode, int *critical_level)
{
	int i, k;
	int chance = 5000;

	int rank = 0;

	if(p_ptr->pclass == CLASS_NINJA) chance = 4444;
	else if(p_ptr->pclass == CLASS_YUGI) chance = 3500;

	//v1.1.69 潤美特技による重量ボーナス
	if(mode == HISSATSU_URUMI)
	{
		int old_weight = weight;
		weight = weight * p_ptr->lev * 8 / 50;

	}

	//v1.1.94 重量2000を上限に(キスメ飛び上がり時除く)
	if (!(p_ptr->pclass == CLASS_KISUME && p_ptr->concent) && weight > 2000) weight = 2000;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3));

	if (cheat_xtra)
		msg_format("(critical_norm) weight:%d plus:%d meichuu:%d i:%d", weight, plus,meichuu,i);

	/* Chance */
	///class クリティカル判定　忍者と剣術家の特殊処理
	if ((randint1(chance) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		//v1.1.94
		//k = weight + randint1(650);
		k = randint1(weight) + randint1(650);

		if (mode == HISSATSU_COUNTER_SPEAR) k += randint1(ref_skill_exp(TV_SPEAR) / 10);

		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);
		if(mode == HISSATSU_MAJIN && k < 700) k = 700;

		if (k < 400)
		{
#ifdef JP
			msg_print("手ごたえがあった！");
#else
			msg_print("It was a good hit!");
#endif

			rank = 1;
			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("かなりの手ごたえがあった！");
#else
			msg_print("It was a great hit!");
#endif

			rank = 2;
			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("会心の一撃だ！");
#else
			msg_print("It was a superb hit!");
#endif

			rank = 3;
			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("最高の会心の一撃だ！");
#else
			msg_print("It was a *GREAT* hit!");
#endif

			rank = 4;
			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("比類なき最高の会心の一撃だ！");
#else
			msg_print("It was a *SUPERB* hit!");
#endif

			rank = 5;
			dam = ((7 * dam) / 2) + 25;
		}
	}

	if (*critical_level < rank) *critical_level = rank;

	return (dam);
}


/*:::スレイ倍率適用　詳細未読*/
static int mult_slaying(int mult, const u32b* flgs, const monster_type* m_ptr)
{
	bool yukari = FALSE;
	static const struct slay_table_t {
		int slay_flag;
		u32b affect_race_flag;
		int slay_mult;
		size_t flag_offset;
		size_t r_flag_offset;
	} slay_table[] = {

#define OFFSET(X) offsetof(monster_race, X)
		/*:::正直ここの構文よく分からん　適当に入れ替えれば大丈夫だと思うが*/
		///sys mon スレイフラグ適用部分
		{TR_SLAY_ANIMAL, RF3_ANIMAL, 25, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_ANIMAL, RF3_ANIMAL, 40, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_EVIL,   RF3_ANG_CHAOS,   20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_EVIL,   RF3_ANG_CHAOS,   35, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_GOOD,   RF3_ANG_COSMOS,   20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_GOOD,   RF3_ANG_COSMOS,   35, OFFSET(flags3), OFFSET(r_flags3)},

///mod131231 モンスターフラグ変更
		{TR_SLAY_HUMAN,  RF3_HUMAN,  25, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_HUMAN,  RF3_HUMAN,  40, OFFSET(flags3), OFFSET(r_flags3)},
//		{TR_SLAY_HUMAN,  RF2_HUMAN,  25, OFFSET(flags2), OFFSET(r_flags2)},
//		{TR_KILL_HUMAN,  RF2_HUMAN,  40, OFFSET(flags2), OFFSET(r_flags2)},
		{TR_SLAY_UNDEAD, RF3_UNDEAD, 30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_UNDEAD, RF3_UNDEAD, 50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DEMON,  RF3_DEMON,  30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DEMON,  RF3_DEMON,  50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_KWAI,   RF3_KWAI,   30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_KWAI,   RF3_KWAI,   50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DEITY,  RF3_DEITY,  30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DEITY,  RF3_DEITY,  50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_HUMAN,  RF3_DEMIHUMAN,  20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_HUMAN,  RF3_DEMIHUMAN,  35, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DRAGON, RF3_DRAGON, 30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DRAGON, RF3_DRAGON, 50, OFFSET(flags3), OFFSET(r_flags3)},
/*:::undefはdefineを無効にするらしい　OFFSET()はあちこちで使われがちなのでここだけに限定しているのだろう*/
#undef OFFSET
	};
	int i;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];

	//紫「人間と妖怪の境界」によるシフト
	if(p_ptr->pclass == CLASS_YUKARI && p_ptr->tim_general[2]) yukari = TRUE;

	for (i = 0; i < sizeof(slay_table) / sizeof(slay_table[0]); ++ i)
	{
		const struct slay_table_t* p = &slay_table[i];
		u32b arg = p->affect_race_flag;

		if(yukari) arg <<= 1L;

		if ((have_flag(flgs, p->slay_flag)) &&
//		    (*(u32b*)(((char*)r_ptr) + p->flag_offset) & p->affect_race_flag))
		    (*(u32b*)(((char*)r_ptr) + p->flag_offset) & arg))
		{
			if (is_original_ap_and_seen(m_ptr) && !yukari)
			{
				*(u32b*)(((char*)r_ptr) + p->r_flag_offset) |= p->affect_race_flag;
			}

			mult = MAX(mult, p->slay_mult);
		}
	}

	return mult;
}
/*:::攻撃属性適用*/
///system mon HURT_ELECを実装するにはここに追加する必要があると思われる
///mod131231 モンスターフラグ変更 属性弱点をflag3からflagrに移動。ここ処理ややこしいので注意
static int mult_brand(int mult, const u32b* flgs, const monster_type* m_ptr)
{
	static const struct brand_table_t {
		int brand_flag;
		u32b resist_mask;
		u32b hurt_flag;
	} brand_table[] = {
		{TR_BRAND_ACID, RFR_EFF_IM_ACID_MASK, 0U           },
		{TR_BRAND_ELEC, RFR_EFF_IM_ELEC_MASK, RFR_HURT_ELEC},
		{TR_BRAND_FIRE, RFR_EFF_IM_FIRE_MASK, RFR_HURT_FIRE},
		{TR_BRAND_COLD, RFR_EFF_IM_COLD_MASK, RFR_HURT_COLD},
//		{TR_BRAND_FIRE, RFR_EFF_IM_FIRE_MASK, RF3_HURT_FIRE},
//		{TR_BRAND_COLD, RFR_EFF_IM_COLD_MASK, RF3_HURT_COLD},
		{TR_BRAND_POIS, RFR_EFF_IM_POIS_MASK, 0U           },
	};
	int i;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];

	for (i = 0; i < sizeof(brand_table) / sizeof(brand_table[0]); ++ i)
	{
		const struct brand_table_t* p = &brand_table[i];

		if (have_flag(flgs, p->brand_flag))
		{
			bool brand_hit = FALSE;
			int mult_effect = 10;

			/* Notice immunity */
			if (r_ptr->flagsr & p->resist_mask)
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flagsr |= (r_ptr->flagsr & p->resist_mask);
				}
			}

			/* Otherwise, take the damage */
			//else if (r_ptr->flags3 & p->hurt_flag)
			else if (r_ptr->flagsr & p->hurt_flag)
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flagsr |= p->hurt_flag;
				}
				brand_hit = TRUE;

				//mult = MAX(mult, 50);
				mult_effect = 50;
			}
			else
			{
				brand_hit = TRUE;
				mult_effect = 25;
				//mult = MAX(mult, 25);
			}

			//v1.1.91 石油地形上のダメージ増加
			//mult値にダメージ補正値を適用するためにbrand_hitという属性がヒットしたフラグとmult_effectという仮倍率みたいな数値を作った
			//この処理の場合、火炎ブレスとかのproject()経由の攻撃は耐性があろうがなかろうがダメージが増えるがここの武器属性処理は耐性抜けにしかダメージが増えない。
			//まあ武器属性は元々そういうものだしそれでいいか。なんか想像すると妙な絵面だが。
			if (brand_hit && p->brand_flag == TR_BRAND_FIRE && cave_have_flag_bold(m_ptr->fy, m_ptr->fx, FF_OIL_FIELD))
			{
				mult_effect = oil_field_damage_mod(mult_effect, m_ptr->fy, m_ptr->fx);
			}

			if (cheat_xtra) msg_format("MULT:%d", mult_effect);

			if (brand_hit)
			{
				mult = MAX(mult, mult_effect);
			}

		}
	}

	return mult;
}
/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
/*:::ダイスダメージ、武器情報、モンスター情報を得てスレイなどを考慮しダメージを増加させる*/
///item mon スレイによるダイス増加処理
s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, int mode, bool thrown)
{
	int mult = 10;

	u32b flgs[TR_FLAG_SIZE];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Extract the flags */
	object_flags(o_ptr, flgs);
	torch_flags(o_ptr, flgs); /* torches has secret flags */

	if (!thrown)
	{
		/* Magical Swords */
		if (p_ptr->special_attack & (ATTACK_ACID)) add_flag(flgs, TR_BRAND_ACID);
		if (p_ptr->special_attack & (ATTACK_COLD)) add_flag(flgs, TR_BRAND_COLD);
		if (p_ptr->special_attack & (ATTACK_ELEC)) add_flag(flgs, TR_BRAND_ELEC);
		if (p_ptr->special_attack & (ATTACK_FIRE)) add_flag(flgs, TR_BRAND_FIRE);
		if (p_ptr->special_attack & (ATTACK_POIS)) add_flag(flgs, TR_BRAND_POIS);

		///mod150415 ヤマメの毒攻撃
		if (p_ptr->pclass == CLASS_YAMAME && p_ptr->lev > randint0(100)) add_flag(flgs, TR_BRAND_POIS);

		//v1.1.89 百々世
		if (p_ptr->pclass == CLASS_MOMOYO)
		{
			if(p_ptr->lev > 34) add_flag(flgs, TR_BRAND_POIS);
			if (p_ptr->tim_general[2]) add_flag(flgs, TR_SLAY_DRAGON);
		}

		//v1.1.47 ミスティア「ヒューマンケージ」
		if (p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE))
			add_flag(flgs, TR_SLAY_HUMAN);
		///mod151115 雛の特殊処理
		if(p_ptr->pclass == CLASS_HINA)
		{
			if(p_ptr->magic_num1[0] > 27000) add_flag(flgs, TR_KILL_GOOD);
			else if(p_ptr->magic_num1[0] > 18000) add_flag(flgs, TR_SLAY_GOOD);
			if(p_ptr->magic_num1[0] > 18000) add_flag(flgs, TR_KILL_HUMAN);
			else if(p_ptr->magic_num1[0] > 12000) add_flag(flgs, TR_SLAY_HUMAN);
		}
		if (p_ptr->special_attack & (ATTACK_DARK))
		{
			if(have_flag(flgs,TR_SLAY_GOOD))  add_flag(flgs, TR_KILL_GOOD);
			else add_flag(flgs, TR_SLAY_GOOD);
		}
		if (p_ptr->special_attack & (ATTACK_FORCE))
		{
			add_flag(flgs, TR_FORCE_WEAPON);
		}
		if (p_ptr->kamioroshi & KAMIOROSHI_IZUNA)
			add_flag(flgs, TR_SLAY_EVIL);
		else if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
			add_flag(flgs, TR_BRAND_FIRE);

		if(p_ptr->pclass == CLASS_MEDICINE && p_ptr->lev > 34) add_flag(flgs, TR_BRAND_POIS);
	}

	/* Hex - Slay Good (Runesword) */
	if (hex_spelling(HEX_RUNESWORD)) add_flag(flgs, TR_SLAY_GOOD);



	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		///item TVAL weapon
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:
		case TV_OTHERWEAPON:
		case TV_LITE:
		{
			/* Slaying */
			mult = mult_slaying(mult, flgs, m_ptr);

			/* Elemental Brand */
			mult = mult_brand(mult, flgs, m_ptr);

			/* Hissatsu */
			///class 剣術家
			if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
			{
				mult = mult_hissatsu(mult, flgs, m_ptr, mode);
			}

			/*:::特殊処理　クロノスの大鎌は男に対しXスレイ相当*/
			if(o_ptr->name1 == ART_KRONOS)
			{
				if((r_ptr->flags1 & RF1_MALE) && mult < 40) mult = 40;
			}

			//v1.1.89 百々世のスネークイーターによるヘビスレイ
			if (p_ptr->pclass == CLASS_MOMOYO)
			{
				if (p_ptr->tim_general[1] && r_ptr->d_char == 'J' && mult < 30) mult = 30;


			}

			//v1.1.14 祇園様の怒り　女性特攻にしてみる
			if(mode == HISSATSU_GION)
			{
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if((r_ptr->flags1 & RF1_FEMALE) && mult < 30) mult = 30;
			}

			/* Force Weapon */
			///mod150808 アリスは無効化
			if ((p_ptr->pclass != CLASS_ALICE) && (p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
			{
				p_ptr->csp -= (1+(o_ptr->dd * o_ptr->ds / 5));
				p_ptr->redraw |= (PR_MANA);
				mult = mult * 3 / 2 + 20;

				gflag_force_effect = TRUE;//理力が効いたかどうか確認するためのフラグ
			}




			/*:::特殊処理*/
			/* Hack -- The Nothung cause special damage to Fafner */
			///item mon ノートゥングがファフナーに大ダメージを与える処理
			if ((o_ptr->name1 == ART_NOTHUNG) && (m_ptr->r_idx == MON_FAFNER))
				mult = 150;
			break;
		}
	}
	if (mult > 150) mult = 150;

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Search for hidden things
 */
/*:::床トラップ、箱トラップ、隠しドア発見処理*/
static void discover_hidden_things(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;

	/* Access the grid */
	c_ptr = &cave[y][x];

	/* Invisible trap */
	if (c_ptr->mimic && is_trap(c_ptr->feat))
	{
		/* Pick a trap */
		disclose_grid(y, x);

		/* Message */
		msg_print(_("トラップを発見した。", "You have found a trap."));

		/* Disturb */
		disturb(0, 1);
	}

	/* Secret door */
	if (is_hidden_door(c_ptr))
	{
		/* Message */
		msg_print(_("隠しドアを発見した。", "You have found a secret door."));

		/* Disclose */
		disclose_grid(y, x);

		/* Disturb */
		disturb(0, 0);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Skip non-chests */
		if (o_ptr->tval != TV_CHEST) continue;

		/* Skip non-trapped chests */
		/*:::箱に罠があるかどうかは箱のpvalで決まるらしい*/
		if (!chest_traps[o_ptr->pval]) continue;

		/* Identify once */
		if (!object_is_known(o_ptr))
		{
			/* Message */
			if(o_ptr->pval < 0 && !chest_traps[0-o_ptr->pval]) msg_print(_("これは開けても安全そうだ。", "It looks safe to open."));
			else msg_print(_("箱に仕掛けられたトラップを発見した！", "You have discovered a trap on the chest!"));

			/* Know the trap */
			object_known(o_ptr);

			/* Notice it */
			disturb(0, 0);
		}
	}
}
/*:::探索技能により隠し地形発見処理*/
void search(void)
{
	int i, chance;


	/* Start with base search ability */
	chance = p_ptr->skill_srh;

	/* Penalize various conditions */
	if (p_ptr->blind || no_lite()) chance = chance / 10;
	if (p_ptr->confused || p_ptr->image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (i = 0; i < 9; ++ i)
	{
		bool flag_auto_success = FALSE;

		//v1.1.57 隠岐奈は隠されたドアと落とし戸を必ず見破る(必ず毎ターンsearch()が呼ばれるとは言っていない)
		if (p_ptr->pclass == CLASS_OKINA)
		{
			cave_type *c_ptr = &cave[py + ddy_ddd[i]][px + ddx_ddd[i]];
			feature_type *f_ptr = &f_info[c_ptr->feat];
			int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

			if(is_hidden_door(c_ptr)) flag_auto_success = TRUE;
			if(trap_feat_type == TRAP_TRAPDOOR) flag_auto_success = TRUE;
		}

		/* Sometimes, notice things */
		if (flag_auto_success || randint0(100) < chance)
		{
			discover_hidden_things(py + ddy_ddd[i], px + ddx_ddd[i]);
		}
	}
}


/*
 * Helper routine for py_pickup() and py_pickup_floor().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
/*:::アイテムを床上からザックに入れる処理　ザックに空きがあると確認済みであること*/
void py_pickup_aux(int o_idx)
{
	int slot;

#ifdef JP
/*
 * アイテムを拾った際に「２つのケーキを持っている」
 * "You have two cakes." とアイテムを拾った後の合計のみの表示がオリジナル
 * だが、違和感が
 * あるという指摘をうけたので、「～を拾った、～を持っている」という表示
 * にかえてある。そのための配列。
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
	bool moon_vault_trap = FALSE;
#else
	char o_name[MAX_NLEN];
	bool moon_vault_trap = FALSE;
#endif

	object_type *o_ptr;

	o_ptr = &o_list[o_idx];

	//月都万象展用特殊アイテム
	if(p_ptr->inside_quest == QUEST_MOON_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
		moon_vault_trap = TRUE;
	//宝物庫★なし時処理(inven_carry()のときmarkedがクリアされるのでcheck_find_art_quest_completion()は使えない)
	else if(p_ptr->inside_quest == QUEST_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
		complete_quest(QUEST_VAULT);



#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	/*:::ザックにアイテムを入れる*/
	slot = inven_carry(o_ptr);

	/* Get the object again */
	o_ptr = &inventory[slot];

	/* Delete the object */
	/*:::足元のアイテムを消す*/
	delete_object_idx(o_idx);

		///system 性格関連処理など
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		bool old_known = identify_item(o_ptr);

		/* Auto-inscription/destroy */
		autopick_alter_item(slot, (bool)(destroy_identify && !old_known));

		/* If it is destroyed, don't pick it up */
		if (o_ptr->marked & OM_AUTODESTROY) return;
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, 0);

	/* Message */

#ifdef JP
///msg131223
#if 0
	if ((o_ptr->name1 == ART_CRIMSON) && (p_ptr->pseikaku == SEIKAKU_COMBAT))
	{
		msg_format("こうして、%sは『クリムゾン』を手に入れた。", player_name);
		msg_print("しかし今、『混沌のサーペント』の放ったモンスターが、");
		msg_format("%sに襲いかかる．．．", player_name);
	}
	else
#endif
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)を持っている。",o_name, index_to_label(slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s拾って、%s(%c)を持っている。",
			       kazu_str, o_name, index_to_label(slot));
			} else {
				msg_format("%s(%c)を拾った。", o_name, index_to_label(slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(slot));
	strcpy(record_o_name, o_name);
#endif
	record_turn = turn;



	check_find_art_quest_completion(o_ptr);

	if(moon_vault_trap) activate_moon_vault_trap();


}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_player()" for handling of other things.
 */
/*:::足元アイテムに対する処理*/
void carry(bool pickup)
{
	cave_type *c_ptr = &cave[py][px];

	s16b this_o_idx, next_o_idx = 0;

	char	o_name[MAX_NLEN];

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Automatically pickup/destroy/inscribe items */
	autopick_pickup_items(c_ptr);


#ifdef ALLOW_EASY_FLOOR

	if (easy_floor)
	{
		py_pickup_floor(pickup);
		return;
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Scan the pile of objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

/*:::ここ未定義*/
#ifdef ALLOW_EASY_SENSE /* TNB */

		/* Option: Make item sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void)sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE -- TNB */

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		///sys item 金を自動的に拾う処理　いずれPアイテムとか実装するならここか
		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld の価値がある%sを見つけた。",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			p_ptr->au += value;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}

		/* Pick up objects */
		else
		{
			/* Hack - some objects were handled in autopick_pickup_items(). */
			if (o_ptr->marked & OM_NOMSG)
			{
				/* Clear the flag. */
				o_ptr->marked &= ~OM_NOMSG;
			}
			/* Describe the object */
			else if (!pickup)
			{
#ifdef JP
				msg_format("%sがある。", o_name);
#else
				msg_format("You see %s.", o_name);
#endif

			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(o_ptr))
			{
#ifdef JP
				msg_format("ザックには%sを入れる隙間がない。", o_name);
#else
				msg_format("You have no room for %s.", o_name);
#endif

			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				int okay = TRUE;

				/* Hack -- query every item */
				if (carry_query_flag)
				{
					char out_val[MAX_NLEN+20];
#ifdef JP
					sprintf(out_val, "%sを拾いますか? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(this_o_idx);
				}
			}
		}
	}
}


/*
* Determine if a trap affects the player.
* Always miss 5% of the time, Always hit 5% of the time.
* Otherwise, match trap power against player armor.
*/
//トラップの命中判定
//v1.1.96 同名の関数があって紛らわしいのでcheck_hit()から名称変更した
static int check_hit_trap(int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


//v1.1.97
//床上トラップの発動。hit_trap()をこれに変更する。主にproject()経由で＠にもモンスターにも効果が出るようにする。
//発動するトラップの座標を指定する。トラップの上に＠でなくモンスターがいることも誰もいないこともある。
//GF_ACTIVATE_TRAP属性でトラップを発動する場合project()が再帰呼び出しされるのでスタックオーバーフローと地形変化と敵消滅に注意
//mpe_mode:MPE_BREAK_TRAPのとき発動後のトラップを粉砕し、MPE_ACTIVATE_TRAPのとき浮遊があっても落とし戸に落ちる
void activate_floor_trap(int y, int x, u32b mpe_mode)
{
	bool flag_break_trap;
	bool trap_player = FALSE;
	bool trap_monster = FALSE;
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;
	char            m_name[80] = "";
	monster_type *m_ptr;
	monster_race *r_ptr;
	int damage;

	flag_break_trap = (mpe_mode & MPE_BREAK_TRAP);


	if (trap_feat_type == NOT_TRAP) return;

	if (world_player || SAKUYA_WORLD)
	{
		msg_print(_("トラップは作動しなかった。", "The trap didn't go off."));
		return;
	}

	//トラップの上に＠がいるときtrap_playerがTRUE
	if (player_bold(y, x))
	{
		trap_player = TRUE;

		if (p_ptr->pclass == CLASS_FLAN) flag_break_trap = TRUE;
		if (p_ptr->pseikaku == SEIKAKU_BERSERK && randint1(p_ptr->lev) > 10) flag_break_trap = TRUE;

	}
	//トラップの上に乗馬以外のモンスターがいるときtrap_monsterがTRUE
	else if (c_ptr->m_idx)
	{
		trap_monster = TRUE;

		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_info[m_ptr->r_idx];

		monster_desc(m_name, m_ptr, 0);

	}

	disturb(0, 1);


	switch (trap_feat_type)
	{
		//落とし戸　＠はテレポレベル、モンスターはFLYING無効、GIGANTICとQUESTORは行動遅延、それ以外フロアから消滅
	case TRAP_TRAPDOOR:
	{
		if (trap_player)
		{
			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし戸を飛び越えた。");
#else
				msg_print("You fly over a trap door.");
#endif
				return;
			}
			else
			{
				cptr trap_name;
				if(mpe_mode & MPE_ACTIVATE_TRAP)
					msg_print(_("落とし戸に飛び込んだ！", "You jump through a trap door!"));
				else
					msg_print(_("落とし戸に落ちた！", "You have fallen through a trap door!"));

				sound(SOUND_FALL);
				damage = damroll(2, 8);
#ifdef JP
				trap_name = "落とし戸";
#else
				trap_name = "a trap door";
#endif
				take_hit(DAMAGE_NOESCAPE, damage, trap_name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "落とし戸に落ちた");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door.");
#endif
				///mod160813 クエストフロアでトラップドアに落ちたときの処理を追加　サグメ用
				if (EXTRA_MODE)
				{

					prepare_change_floor_mode(CFM_DOWN | CFM_RAND_PLACE | CFM_NO_RETURN);
					if (INSIDE_EXTRA_QUEST)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}
				else
				{
					prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
					if (p_ptr->inside_quest)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
		else if (trap_monster)
		{
			msg_print(_("落とし戸が開いた！", "A trap door opens!"));
			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				msg_format(_("%sは悠々と落とし戸の上を飛んでいる。",
                            "%^s is leisurely flying above the trap door."), m_name);
			}
			else if (r_ptr->flags1 & RF1_QUESTOR || r_ptr->flags2 & RF2_GIGANTIC)
			{
				msg_format(_("%sは落とし戸から抜け出した！",
                            "%^s escapes from the trap door!"), m_name);
				monster_delay(c_ptr->m_idx, randint1(100));
			}
			else
			{
				msg_format(_("%sが落とし戸に落ちた！",
                            "%^s has fallen through a trap door!"), m_name);
				monster_drop_carried_objects(m_ptr);//拾ったアイテムは律儀に落としてから消えることにしておく
				delete_monster_idx(c_ptr->m_idx);
			}

		}
		else
		{
			msg_print(_("落とし戸が開いた。しかし誰も落とし戸の上にいない。", "A trap door opens. However, no one was on top of it."));
		}
	}
	break;


	//落とし穴のダメージ処理はGF_PIT_FALL属性として実装した。いずれ魔法の「深い穴生成」やてゐの落とし穴とも統合したい
	case TRAP_PIT:

	{
		int d = dun_level / 8 + 1;
		damage = damroll(d, d);

		if (trap_player)
		{

			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし穴を飛び越えた。");
#else
				msg_print("You fly over a trap pit.");
#endif
				return;

			}
			else
			{
				msg_print(_("落とし穴だ！", "It's a trap pit!"));
				project(PROJECT_WHO_TRAP, 0, y, x, damage, GF_PIT_FALL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER), -1);
			}
		}
		else if (trap_monster)
		{
			project(PROJECT_WHO_TRAP, 0, y, x, damage, GF_PIT_FALL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER), -1);
		}
		else
		{
			msg_print(_("落とし穴が開いた。しかし誰も落とし穴の上にいない。", "A trap pit opens. However, no one was on top of it."));
		}

	}
	break;

	//defineのラベルだけそのままで穴の機能を水地形生成と石油地形生成に変えた
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (trap_player)
		{
			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
				msg_print(_("亀裂の上を飛び越えた。", "You fly above a crack in the ground."));
				return;
			}
			else
			{
				if (mpe_mode & MPE_ACTIVATE_TRAP)
					msg_print(_("あなたは足元の亀裂を踏み割った！", "You deliberately stomp on a crack beneath you!"));

				if(trap_feat_type == TRAP_SPIKED_PIT)
					msg_print(_("足元が崩れた！下に水が溜まっている！", "The ground beneath you cracks! Water pours forth!"));
				else
					msg_print(_("足元から石油が噴き出した！", "Oil spouts out from below you!"));

			}
		}
		//地形変更する前にこのグリッドのトラップを消滅させる
		cave_alter_feat(y, x, FF_HIT_TRAP);

		//ここは＠が飛び越えたとき以外無条件に発動するようにしている。
		//いずれトラップをモンスターが踏むように改造する機能を実装したとして、
		//飛ぶモンスターが踏み入ったときに発動しないよう設定する必要がある。
		if (trap_feat_type == TRAP_SPIKED_PIT)
			project(PROJECT_WHO_TRAP, (2 + randint1(5) / 4) , y, x, 4, GF_WATER_FLOW, PROJECT_JUMP | PROJECT_GRID, -1);
		else
			project(PROJECT_WHO_TRAP, (3 + randint1(3)/3), y, x, 100+dun_level, GF_DIG_OIL, PROJECT_JUMP | PROJECT_GRID, -1);


		break;

	//邪悪なルーン	モンスターが踏んだら全能力低下+移動禁止でいいだろう
	case TRAP_TY_CURSE:
	{
		if (trap_player)
		{
			int i;
			int num = 2 + randint1(3);
#ifdef JP
			msg_print("何かがピカッと光った！");
#else
			msg_print("There is a flash of shimmering light!");
#endif
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				} while (one_in_(6));
			}
		}
		else if (trap_monster)
		{
			msg_format(_("%sの足元で何かがピカッと光った！", "Something beneath %s gives off a flash of light!"), m_name);

			project(0, 0, y, x, 100 + dun_level * 3, GF_DEC_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(0, 0, y, x, 10 + dun_level / 2, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);
		}
		else
		{
			msg_print(_("何かがピカッと光った。しかし呪いは不発のようだ。", "Something gives off a flash of light. However, the curse wasn't activated."));
		}


	}
	break;

	//奇妙なルーン　モンスターが踏んだらAWAY_ALLでいいだろう
	case TRAP_TELEPORT:
	{
		if (trap_player)
		{
#ifdef JP
			msg_print("テレポート・トラップにひっかかった！");
#else
			msg_print("You hit a teleport trap!");
#endif
			teleport_player(100, TELEPORT_PASSIVE);
		}
		else if (trap_monster)
		{
			msg_format(_("%sはテレポート・トラップにひっかかった！", "%^s hits a teleport trap!"), m_name);
			project(0, 0, y, x, 100, GF_AWAY_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
		}
		else
		{
			msg_print(_("奇妙なルーンが起動した。しかし不発のようだ。", "A strange rune is activated. However, nothing happens."));
		}


	}
	break;

	//炎　単純に火炎ボールを起こして範囲内全員ダメージ
	case TRAP_FIRE:
	{

#ifdef JP
		msg_print("地面から炎が噴き出した！");
#else
		msg_print("Flames shoot out from the ground!");
#endif

		damage = MAX(5, dun_level) * 3;
		project(PROJECT_WHO_TRAP, 2, y, x, damage, GF_FIRE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;


	case TRAP_ACID:
	{
#ifdef JP
		msg_print("地面から酸が噴き出した！");
#else
		msg_print("Acid shoots out from the ground!");
#endif

		damage = MAX(5, dun_level) * 3;

		project(PROJECT_WHO_TRAP, 2, y, x, damage, GF_ACID, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;

	case TRAP_POISON:
	{
#ifdef JP
		msg_print("地面から緑色のガスが噴き出した！");
#else
		msg_print("Green gas pours out from the ground!");
#endif

		damage = MAX(5, dun_level) * 3;
		project(PROJECT_WHO_TRAP, 3, y, x, damage, GF_POIS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;


	//ダーツ　モンスターには能力低下属性攻撃
	case TRAP_SLOW:
	case TRAP_LOSE_STR:
	case TRAP_LOSE_DEX:
	case TRAP_LOSE_CON:
	{
		if (trap_player)
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				damage = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, damage, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, damage, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW())
				{
					if (trap_feat_type == TRAP_SLOW) set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
					if (trap_feat_type == TRAP_LOSE_STR) do_dec_stat(A_STR);
					if (trap_feat_type == TRAP_LOSE_DEX) do_dec_stat(A_DEX);
					if (trap_feat_type == TRAP_LOSE_CON) do_dec_stat(A_CON);
				}
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}

		}
		else if (trap_monster)
		{
			int power = 100 + dun_level * 2;
			msg_format(_("小さなダーツが飛んだ！", "A small dart flies out!"));

			if (trap_feat_type == TRAP_SLOW)
				project(0, 0, y, x, power, GF_OLD_SLOW, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_STR)
				project(0, 0, y, x, power, GF_DEC_ATK, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_DEX)
				project(0, 0, y, x, power, GF_DEC_ATK, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_CON)
				project(0, 0, y, x, power, GF_DEC_DEF, (PROJECT_JUMP | PROJECT_KILL), -1);

		}
		else
		{
			msg_print(_("小さなダーツが飛んで床に刺さった。", "A small dart flies out and hits the floor."));
		}

	}
	break;

	//盲目ガス　モンスターには盲目状態がないので混乱として扱う。
	//まず＠に影響を及ぼさない混乱属性ボールを発生させ、＠が範囲内にいたら盲目処理を行う。
	case TRAP_BLIND:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("地面から黒いガスが噴き出した！");
#else
		msg_print("Black gas pours out from the ground!");
#endif
		project(0, 3, y, x, power, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

		if (!p_ptr->resist_blind && distance(y, x, py, px) < 3)
		{
			(void)set_blind(p_ptr->blind + randint0(50) + 25);
		}
	}
	break;
	//混乱ガス　単純に混乱ボールを発生させる
	case TRAP_CONFUSE:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("地面からきらめくガスが噴き出した！");
#else
		msg_print("Gas of scintillating colors pours out from the ground!");
#endif
		project(PROJECT_WHO_TRAP, 3, y, x, power, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER), -1);
	}
	break;

	//睡眠ガス　悪夢処理はproject_p()で行われるのでここで書かなくて済む
	case TRAP_SLEEP:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("地面から白いガスが噴き出した！");
#else
		msg_print("White gas pours out from the ground!");
#endif
		project(PROJECT_WHO_TRAP, 3, y, x, power, GF_OLD_SLEEP, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER), -1);

	}
	break;

	//コンパクトルーン(トラップ追加生成)　上に何がいようと同じ効果でいいだろう
	case TRAP_TRAPS:
	{
#ifdef JP
		msg_print("まばゆい閃光が走った！");
#else
		msg_print("There is a bright flash of light!");
#endif
		/* Make some new traps */
		project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

	}
	break;

	//警報　加えてSTUN属性のボール発生
	//とはいえスピードモンスターがあるから利用することはないだろう。
	//スピードモンスターなしで起こすだけにする？それだと特技の警報と紛らわしいか。
	case TRAP_ALARM:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("けたたましい音が鳴り響いた！");
#else
		msg_print("There is a loud, shrill noise!");
#endif

		aggravate_monsters(0, FALSE);
		project(PROJECT_WHO_TRAP, 1, y, x, power, GF_STUN, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

	}
	break;

	//開門トラップ
	//とりあえず誰が踏んでも同じ効果にしておく。
	//モンスターに踏ませたらそいつをターゲットにするよう設定するのも面白いが、ハルマゲドンと違い開門ではすでにモンスターがいるから改めてターゲット設定し直すのが難しい。
	//開門トラップで配置されるモンスターの生成時に特殊フラグを立てておき、トラップを踏んだらターゲットを切り替える？フロアに開門トラップが二つあったら？
	case TRAP_OPEN:
	{
#ifdef JP
		msg_print("大音響と共にまわりの壁が崩れた！");
#else
		msg_print("Suddenly, surrounding walls are opened!");
#endif
		(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		aggravate_monsters(0, TRUE);

		break;
	}

	//ハルマゲドントラップ
	//モンスターに踏ませたらそいつをターゲットにするよう設定してみる
	case TRAP_ARMAGEDDON:
	{
		static int levs[10] = { 0, 0, 20, 10, 5, 3, 2, 1, 1, 1 };
		int evil_idx = 0, good_idx = 0;

		int lev;

		if (trap_player)
		{
#ifdef JP
			msg_print("突然天界の戦争に巻き込まれた！");
#else
			msg_print("Suddenly, you are in the middle of a heavenly war!");
#endif
		}
		else if (trap_monster)
		{
			msg_format(_("%sは天界の戦争に巻き込まれた！", "%s is in the middle of a heavenly war!"), m_name);

		}
		else
		{
			msg_print(_("突然天界の戦争が巻き起こった！", "Suddenly, a heavenly war breaks out!"));

		}

		/* Summon Demons and Angels */
		for (lev = dun_level; lev >= 20; lev -= 1 + lev / 16)
		{
			int i;
			int num = levs[MIN(lev / 10, 9)];
			for (i = 0; i < num; i++)
			{
				monster_type *evil_ptr;
				monster_type *good_ptr;

				int x1 = rand_spread(x, 7);
				int y1 = rand_spread(y, 5);

				/* Skip illegal grids */
				if (!in_bounds(y1, x1)) continue;

				/* Require line of projection */
				if (!projectable(py, px, y1, x1)) continue;

				if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
				{
					evil_idx = hack_m_idx_ii;
					evil_ptr = &m_list[evil_idx];
				}

				if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
				{
					good_idx = hack_m_idx_ii;
					good_ptr = &m_list[good_idx];
				}

				/* Let them fight each other */
				if (evil_idx && good_idx)
				{
					evil_ptr->target_y = good_ptr->fy;
					evil_ptr->target_x = good_ptr->fx;
					good_ptr->target_y = evil_ptr->fy;
					good_ptr->target_x = evil_ptr->fx;
				}
				//モンスターに踏ませた場合、所属勢力でないほうから集中攻撃を受けることにしてみる。＠が踏むときに比べて遥かに大ダメージを受けるかもしれない
				if (trap_monster && !(r_ptr->flags3 & RF3_ANG_CHAOS) && evil_idx)
				{
					evil_ptr->target_y = y;
					evil_ptr->target_x = x;
				}
				if (trap_monster && !(r_ptr->flags3 & RF3_ANG_COSMOS) && good_idx)
				{
					good_ptr->target_y = y;
					good_ptr->target_x = x;
				}


			}
		}
	}
	break;

	//ピラニアトラップ　誰が踏んでも同じでいいだろう
	case TRAP_PIRANHA:
	{
		int num, i;
#ifdef JP
		msg_print("突然壁から水が溢れ出した！ピラニアがいる！");
#else
		msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

		//地形変更する前にこのグリッドのトラップを消滅させる
		cave_alter_feat(y, x, FF_HIT_TRAP);

		/* Water fills room */
		fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

		/* Summon Piranhas */
		num = 1 + dun_level / 20;
		for (i = 0; i < num; i++)
		{
			(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
		}
	}
	break;


	//ビーム罠
	case TRAP_BEAM:
	{
		int tmp_y, tmp_x;
		int i,j, k, dir;
		byte dir_table[8] = { 1,2,3,4,6,7,8,9 };
		byte dir_init = randint0(8);
		bool flag = FALSE;

		for (i = 0; i<8; i++)
		{
			//ランダムな方角を決める
			k = dir_init + i;
			if (k >= 8) k -= 8;
			dir = dir_table[k];

			//ターゲットグリッドから1グリッド以上開けて壁などがある位置を探す
			for (j = 1; j<16; j++)
			{
				tmp_x = x + ddx[dir] * j;
				tmp_y = y + ddy[dir] * j;

				if (!in_bounds(tmp_y, tmp_x)) break;

				if (!cave_have_flag_bold(tmp_y, tmp_x, FF_PROJECT))
				{
					if (j == 1) break;
					else
					{
						tmp_x = x + ddx[dir] * (j - 1);
						tmp_y = y + ddy[dir] * (j - 1);
						flag = TRUE;
						break;
					}
				}

			}
			if (flag) break;
		}

		//丁度良い場所があった場合そこからレーザー発射
		if (flag)
		{
			int typ;
			int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

			damage = dun_level * 2 + randint1(50);
			if (randint1(dun_level + 50) > 50) typ = GF_NUKE;
			else typ = GF_FIRE;

			//-hack- 通常project()はモンスターか＠の位置からしか撃たないのでグローバル変数使って例外処理　違法建築がまた一つ :)
			hack_project_start_x = tmp_x;
			hack_project_start_y = tmp_y;

			if (one_in_(7))
				msg_print(_("*上手に焼けましたー*", "* WELL BURNED *"));
			else if (trap_player)
				msg_print(_("どこからかビームが飛んできた！", "A beam shoots out of somewhere!"));
			else
				msg_print(_("壁からビームが撃ち出された！", "A beam shoots out of a wall!"));

			(void)project(PROJECT_WHO_TRAP, 0, y, x, damage, typ, flg, -1);
			hack_project_start_x = 0;
			hack_project_start_y = 0;

		}
		else
		{
			msg_print(_("しかし何も起こらなかったようだ。", "Doesn't look like anything happened."));
		}

		break;
	}


	//↓実装を検討していたが保留。
	//f_infoのインデックスが255を超えるのでどこかに問題が起こるかもしれない。今後もう少し落ち着いたときにやる
	/*
	case TRAP_CEILING: //吊り天井トラップ 部屋中に150+1d150のダメージ
	{

		msg_print("轟音を立てて天井が落ちてきた！");
		project_hack3(PROJECT_WHO_TRAP, y, x, GF_DISP_ALL, 1, 150, 150);

	}
	break;

	case TRAP_OIL: //石油
	{

#ifdef JP
		msg_print("地面から石油が噴き出した！");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif
		project(PROJECT_WHO_TRAP, 6, y, x, 250, GF_DIG_OIL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;

	case TRAP_SUIKI: //水鬼鬼神長の罠　水ダメージ+水地形生成+長時間移動禁止
	{

#ifdef JP
		msg_print("地面から超高圧の水が噴き出した！");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif

		project(PROJECT_WHO_TRAP, 4, y, x, 200, GF_WATER_FLOW, (PROJECT_JUMP | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 4, y, x, 200, GF_WATER, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 1, y, x, 100, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);
	}
	break;


	//間欠泉トラップ　上のフロアに飛ばされる
	//クエストダンジョンや地下一階や夢日記フロアなどに生成されないよう注意
	case TRAP_GEYSER:
	{

		msg_print("間欠泉が噴き出した！");
		damage = damroll(10, 10) + dun_level * 2;
		project(PROJECT_WHO_TRAP, 3, y, x, damage, GF_STEAM, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

		//＠は上のフロアに飛ばされる
		if (trap_player && p_ptr->chp >= 0 && dun_level > 1 && !p_ptr->inside_arena && !(EXTRA_MODE) && !p_ptr->anti_tele)
		{
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "間欠泉に吹き上げられた");
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			msg_print("あなたは上のフロアに吹き飛ばされた！");

			//クエストフロアには生成されないようにする必要がある
			if (p_ptr->inside_quest)
			{
				leave_quest_check();
				p_ptr->inside_quest = 0;
			}
			p_ptr->leaving = TRUE;

		}
		//モンスターはQUESTORとGIGANTICは行動遅延、それ以外はフロアから追放
		//蒸気ボールで倒したときは何もしない
		else if (trap_monster && m_ptr->r_idx)
		{
			if (r_ptr->flags1 & RF1_QUESTOR || r_ptr->flags2 & RF2_GIGANTIC)
			{
				msg_format("%sは吹き飛ばされなかった。", m_name);
				monster_delay(c_ptr->m_idx, randint1(100));
			}
			else
			{
				msg_format("%sは間欠泉に吹き飛ばされてこのフロアから消えた。", m_name);
				monster_drop_carried_objects(m_ptr);//拾ったアイテムは律儀に落としてから消えることにしておく
				delete_monster_idx(c_ptr->m_idx);
			}

		}

	}
	break;

	//普通に地雷も作ろう。
	//モンスター「地雷」を削除する？その場合新聞の号数を詰めたり幽霊洋館クエの地雷を置き換えたりする必要がある。
	case TRAP_MINE:
	{
	msg_print("地雷だ！");

	msg_print("(未実装)");

	}

	case TRAP_MEGACRASH: //メガクラッシュ地雷 MEGACRASHとMEGACRUSHのどっちが正しいんだろう
	{
		msg_print("巨大な爆発が起こった！");
		destroy_area(y, x, 10, FALSE, FALSE, FALSE);
		if (distance(py, px, y, x)<10)
		{
			if (check_activated_nameless_arts(JKF1_DESTRUCT_DEF))
			{
				msg_format("あなたは破壊の力を物ともしなかった！");
				break;
			}
			damage = p_ptr->chp / 2;
			if (p_ptr->pclass != CLASS_CLOWNPIECE)
			{
				//v1.1.86 NOESCAPE→ATTACKに
				take_hit(DAMAGE_ATTACK, damage, m_name, -1);
			}
			if (p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
			{
				msg_format("あなたは吹き飛ばされた！");
				teleport_player_away(0, 100);
			}
		}
	}
	break;


	//大岩トラップ？罠仕様変更memo2.txtに検討内容


	//謎の呪文が刻まれた床に鳥居の彫られた隕石が落ちてくるとか？


	*/

	default:
		msg_format(_("ERROR:activate_trap()にてtrap_feat_type%dの発動効果が未登録",
                    "ERROR: Unregistered effect for trap_feat_type %d in activate_trap()"), trap_feat_type);
		return;

	}



	//罠発動後の消滅処理
	if (is_trap(c_ptr->feat))
	{
		//トラップ粉砕移動などのときトラップ解除
		if (flag_break_trap)
		{
			cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
			msg_print("トラップを粉砕した。");
#else
			msg_print("You destroyed the trap.");
#endif
			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);
				/* Redraw */
				lite_spot(y, x);
			}
		}
		//それ以外の発動のときも1/6でトラップ解除
		else if (one_in_(6))
		{
			cave_alter_feat(y, x, FF_DISARM);
		}
		//そうでないときはトラップごとに設定された処理。
		else
		{
			cave_alter_feat(y, x, FF_HIT_TRAP);
		}
	}


}


//v1.1.96 hit_trap()をactivate_floor_trap()に置き換えた
#if 0

/*
 * Handle player hitting a real trap
 */
/*:::＠がトラップにかかった場合*/
///mod160813 サグメの処理のためexternした
void hit_trap(bool break_trap)
{
	int i, num, dam;
	int x = px, y = py;

	/* Get the cave grid */
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

#ifdef JP
	cptr name = "トラップ";
#else
	cptr name = "a trap";
#endif

	/* Disturb the player */
	disturb(0, 1);

	if(world_player || SAKUYA_WORLD)
	{
		msg_print(_("トラップは作動しなかった。", "The trap didn't go off."));
		return;
	}


	cave_alter_feat(y, x, FF_HIT_TRAP);

	if(p_ptr->pclass == CLASS_FLAN) break_trap = TRUE;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK && randint1(p_ptr->lev) > 10 )break_trap = TRUE;

	/* Analyze XXX XXX XXX */
	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし戸を飛び越えた。");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし戸に落ちた！");
				///msg131223
				//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				//	msg_print("くっそ～！");
#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = damroll(2, 8);
#ifdef JP
				name = "落とし戸";
#else
				name = "a trap door";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "落とし戸に落ちた");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				///mod160813 クエストフロアでトラップドアに落ちたときの処理を追加　サグメ用
				if(EXTRA_MODE)
				{
					prepare_change_floor_mode(CFM_DOWN | CFM_RAND_PLACE | CFM_NO_RETURN);
					if(INSIDE_EXTRA_QUEST)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}
				else
				{
					prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
					if(p_ptr->inside_quest)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			break;
		}

		case TRAP_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし穴を飛び越えた。");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし穴に落ちてしまった！");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = damroll(2, 6);
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_SPIKED_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				dam = damroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("スパイクが刺さった！");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_POISON_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = damroll(2, 6);

#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("毒を塗られたスパイクが刺さった！");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));

					if (p_ptr->resist_pois || IS_OPPOSE_POIS())
					{
#ifdef JP
						msg_print("しかし毒の影響はなかった！");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(p_ptr->poisoned + randint1(dam));
					}
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}

			break;
		}

		case TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("何かがピカッと光った！");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("テレポート・トラップにひっかかった！");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
#ifdef JP
			msg_print("炎に包まれた！");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)fire_dam(dam, "炎のトラップ", -1);
#else
			(void)fire_dam(dam, "a fire trap", -1);
#endif

			break;
		}

		case TRAP_ACID:
		{
#ifdef JP
			msg_print("酸が吹きかけられた！");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)acid_dam(dam, "酸のトラップ", -1);
#else
			(void)acid_dam(dam, "an acid trap", -1);
#endif

			break;
		}

		case TRAP_SLOW:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_STR:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_STR);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_DEX);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_CON);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_BLIND:
		{
#ifdef JP
			msg_print("黒いガスに包み込まれた！");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + randint0(50) + 25);
			}
			break;
		}

		case TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("きらめくガスに包み込まれた！");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!p_ptr->resist_conf)
			{
				(void)set_confused(p_ptr->confused + randint0(20) + 10);
			}
			break;
		}

		case TRAP_POISON:
		{
#ifdef JP
			msg_print("刺激的な緑色のガスに包み込まれた！");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (!p_ptr->resist_pois && !IS_OPPOSE_POIS())
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(20) + 10);
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 3000);
			}
			break;
		}

		case TRAP_SLEEP:
		{
#ifdef JP
			msg_print("奇妙な白い霧に包まれた！");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!p_ptr->free_act)
			{
#ifdef JP
msg_print("あなたは眠りに就いた。");
#else
				msg_print("You fall asleep.");
#endif


				if (ironman_nightmare)
				{
#ifdef JP
msg_print("身の毛もよだつ光景が頭に浮かんだ。");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(get_mon_num(MAX_DEPTH));

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(p_ptr->paralyzed + randint0(10) + 5);
			}
			break;
		}

		case TRAP_TRAPS:
		{
#ifdef JP
msg_print("まばゆい閃光が走った！");
#else
			msg_print("There is a bright flash of light!");
#endif

			/* Make some new traps */
			project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

			break;
		}

		case TRAP_ALARM:
		{
#ifdef JP
			msg_print("けたたましい音が鳴り響いた！");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_monsters(0,TRUE);

			break;
		}

		case TRAP_OPEN:
		{
#ifdef JP
			msg_print("大音響と共にまわりの壁が崩れた！");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_monsters(0,TRUE);

			break;
		}

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
#ifdef JP
			msg_print("突然天界の戦争に巻き込まれた！");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Summon Demons and Angels */
			for (lev = dun_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(py, px, y1, x1)) continue;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
					{
						good_idx = hack_m_idx_ii;
					}

					/* Let them fight each other */
					if (evil_idx && good_idx)
					{
						monster_type *evil_ptr = &m_list[evil_idx];
						monster_type *good_ptr = &m_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
					}
				}
			}
			break;
		}

		case TRAP_PIRANHA:
		{
#ifdef JP
			msg_print("突然壁から水が溢れ出した！ピラニアがいる！");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + dun_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}



		//ビーム罠
		case TRAP_BEAM:
		{
			int tmp_y,tmp_x;
			int j,k,dir;
			byte dir_table[8] = {1,2,3,4,6,7,8,9};
			byte dir_init = randint0(8);
			bool flag=FALSE;

			for(i=0;i<8;i++)
			{
				//ランダムな方角を決める
				k = dir_init + i;
				if(k>=8) k -= 8;
				dir=dir_table[k];

				//x,y(＠の位置のはず)から1グリッド以上開けて壁などがある位置を探す
				for(j=1;j<16;j++)
				{
					tmp_x = x + ddx[dir] * j;
					tmp_y = y + ddy[dir] * j;

					if(!in_bounds(tmp_y,tmp_x)) break;

					if(!cave_have_flag_bold(tmp_y,tmp_x,FF_PROJECT))
					{
						if(j==1) break;
						else
						{
							tmp_x = x + ddx[dir] * (j-1);
							tmp_y = y + ddy[dir] * (j-1);
							flag = TRUE;
							break;
						}
					}

				}
				if(flag) break;
			}

			//丁度良い場所があった場合そこから＠の位置へレーザー発射
			if(flag)
			{
				int typ;
				int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

				dam = dun_level * 2 + randint1(50);
				if(randint1(dun_level+50) > 50) typ = GF_NUKE;
				else typ = GF_FIRE;

				//-hack- 通常project()はモンスターか＠の位置からしか撃たないのでグローバル変数使って例外処理　違法建築がまた一つ :)
				hack_project_start_x = tmp_x;
				hack_project_start_y = tmp_y;

				if(one_in_(7))
					msg_print(_("*上手に焼けましたー*", "*Well roasted*"));
				else
					msg_print(_("どこからかビームが飛んできた！", "A beam shoots out of nowhere!"));

				(void)project(PROJECT_WHO_TRAP, 0, py, px, dam, typ, flg, -1);
				hack_project_start_x = 0;
				hack_project_start_y = 0;

			}
			else
			{
				msg_print(_("しかし何も起こらなかったようだ。", "However, nothing happens."));
			}

			break;
		}




	}

	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
		msg_print("トラップを粉砕した。");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}

#endif

/*:::オーラによるダメージを受ける処理*/
///mon オーラダメージ
///mod140211 他のソースファイルから使うためにstaticを外した。
void touch_zap_player(monster_type *m_ptr)
{
	int aura_damage = 0;
	int momoyo_gain_mana = 0;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	//ドッペルゲンガー特殊処理
	if(r_ptr->flags7 & RF7_DOPPELGANGER)
	{
		msg_print(_("あなたは自分のドッペルゲンガーに触れてしまった！",
                    "Oh no! You've touched your doppelganger!"));
		take_hit(DAMAGE_LOSELIFE,p_ptr->chp,_("ドッペルゲンガーとの接触", "contact with doppelganger"),-1);
		set_stun(100+randint0(3));

		return;
	}

	if (r_ptr->flags2 & RF2_AURA_FIRE)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

		//v1.1.89 百々世ドラゴンイーターによるオーラ吸収
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}
		else if (!p_ptr->immune_fire)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても熱くなった！");
#else
			msg_print("You are suddenly very hot!");
#endif
			/*
			if (prace_is_(RACE_ENT)) aura_damage += aura_damage / 3;
			if (IS_OPPOSE_FIRE()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_fire) aura_damage = (aura_damage + 2) / 3;
			*/

			///mod140302 元素弱点、耐性処理統合
			aura_damage = mod_fire_dam(aura_damage);

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_FIRE;
			handle_stuff();
		}
	}
	///mod131231 モンスターフラグ変更
	if (r_ptr->flags2 & RF2_AURA_COLD)
	//if (r_ptr->flags3 & RF3_AURA_COLD)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
		//v1.1.89 百々世ドラゴンイーターによるオーラ吸収
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}
		else if (!p_ptr->immune_cold)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("突然とても寒くなった！");
#else
			msg_print("You are suddenly very cold!");
#endif
			/*
			if (IS_OPPOSE_COLD()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_cold) aura_damage = (aura_damage + 2) / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			aura_damage = mod_cold_dam(aura_damage);

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
///mod131231 モンスターフラグ変更
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_COLD;
			//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_AURA_COLD;
			handle_stuff();
		}
	}

	if (r_ptr->flags2 & RF2_AURA_ELEC)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
		//v1.1.89 百々世ドラゴンイーターによるオーラ吸収
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}

		else if (!p_ptr->immune_elec)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

			/*
			if (IS_OPPOSE_ELEC()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_elec) aura_damage = (aura_damage + 2) / 3;
			*/
			///mod140302 元素弱点、耐性処理統合
			aura_damage = mod_elec_dam(aura_damage);


#ifdef JP
			msg_print("電撃をくらった！");
#else
			msg_print("You get zapped!");
#endif

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_ELEC;
			handle_stuff();
		}
	}

	momoyo_gain_mana /= 3;

	if (momoyo_gain_mana && !p_ptr->is_dead)
	{
		msg_print(_("あなたは浴びせられたオーラから魔力を錬成した！",
                    "You absorb magical power from the aura!"));
		player_gain_mana(momoyo_gain_mana);

	}


}

/*:::変異の部位による攻撃:::*/
/*:::m_idx:モンスターm_list[]のID
 *:::attack:部位の種類
 *:::fear モンスターが恐怖したかどうか
 *:::mdeath モンスターが死んだかどうか
 */
///sys mutation 変異部位による攻撃　追加格闘と統合予定
///mod140323 変異部位攻撃は別ルーチンで実装し、いまはここは使っていない
#if 0
static void natural_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	int             dss, ddd;

	cptr            atk_desc;

	/*:::攻撃部位によりダイスと重量を決定*/
	switch (attack)
	{
		case MUT2_HARDHEAD:
			dss = 3;
			ddd = 7;
			n_weight = 5;
#ifdef JP
			atk_desc = "石頭";
#else
			atk_desc = "tail";
#endif

			break;
		case MUT2_HORNS:
			dss = 2;
			ddd = 6;
			n_weight = 15;
#ifdef JP
			atk_desc = "角";
#else
			atk_desc = "horns";
#endif

			break;
		case MUT2_BIGHORN:
			dss = 4;
			ddd = 6;
			n_weight = 30;
#ifdef JP
			atk_desc = "大きな角";
#else
			atk_desc = "beak";
#endif

			break;
		case MUT2_TAIL:
			dss = 1;
			ddd = 4;
			n_weight = 35;
#ifdef JP
			atk_desc = "尻尾";
#else
			atk_desc = "trunk";
#endif

			break;
		case MUT2_BIGTAIL:
			dss = 2;
			ddd = 5;
			n_weight = 5;
#ifdef JP
			atk_desc = "大きな尻尾";
#else
			atk_desc = "tentacles";
#endif

			break;
		default:
			dss = ddd = n_weight = 1;
#ifdef JP
			atk_desc = "未定義の部位";
#else
			atk_desc = "undefined body part";
#endif

	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h_m;
	bonus += (p_ptr->lev * 6 / 5);
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	/*:::命中判定*/
	if ((!(r_ptr->flags2 & RF2_QUANTUM) || !randint0(2)) && test_hit_norm(chance, r_ptr->ac, m_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

#ifdef JP
		msg_format("%sを%sで攻撃した。", m_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", m_name, atk_desc);
#endif

		/*:::ダイスダメージ、クリティカルダメージ計算*/
		k = damroll(ddd, dss);
		k = critical_norm(n_weight, bonus, k, (s16b)bonus, 0);

		/* Apply the player damage bonuses */
		k += p_ptr->to_d_m;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(m_ptr, k, FALSE);
		/*:::ウィザードはダメージを表示する*/
		/* Complex message */
		if (p_ptr->wizard)
		{
#ifdef JP
				msg_format("%d/%d のダメージを与えた。", k, m_ptr->hp);
#else
			msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif

		}

		/* Anger the monster */
		/*:::敵を怒らせる*/
		if (k > 0) anger_monster(m_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case MUT2_HARDHEAD:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_HORNS:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_BIGHORN:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_TAIL:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_BIGTAIL:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			default:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
		}

		touch_zap_player(m_ptr);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
#ifdef JP
			msg_format("ミス！ %sにかわされた。", m_name);
#else
		msg_format("You miss %s.", m_name);
#endif

	}
}
#endif

//銃装備時に特殊格闘が行われるとき、装備している銃の種類によってMELEE_MODE_*を返す
int select_gun_melee_mode(void)
{
	object_type *o_ptr;
	int melee_mode[2] = {0,0};
	int slot;
	bool flag_ammo = TRUE;
	int timeout_base, timeout_max;

	for(slot = INVEN_RARM;slot <= INVEN_LARM;slot++)
	{
		o_ptr = &inventory[slot];
		if(o_ptr->tval != TV_GUN) continue;

		//v1.1.70 装備スロット無効化時
		if (check_invalidate_inventory(slot)) continue;

		switch(o_ptr->sval)
		{
		case SV_FIRE_GUN_LUNATIC:
		case SV_FIRE_GUN_HANDGUN:
		case SV_FIRE_GUN_CRIMSON:
		case SV_FIRE_GUN_MAGNUM:
		case SV_FIRE_GUN_LUNATIC_2:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_HANDGUN;
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if(o_ptr->timeout > timeout_max) flag_ammo = FALSE;

			break;
		case SV_FIRE_GUN_HUNTING:
		case SV_FIRE_GUN_MUSKET:
		case SV_FIRE_GUN_SHOTGUN:
		case SV_FIRE_GUN_LASER:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_LONGBARREL;
			break;
		case SV_FIRE_GUN_INFANTRY:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_BAYONET;
			break;
		case SV_FIRE_GUN_ROCKET:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_LAUNCHER;
			break;
		default:
			msg_print(_("ERROR:select_gun_melee_mode()にこの銃の処理が登録されていない",
                        "ERROR: this gun doesn't have proper logic in select_gun_melee_mode()"));
			return 0;
		}
	}
	//二挺拳銃で格闘と銃の技能が高いとき特殊格闘が出るようにしてみる
	//v1.1.21 残弾があるときしかやらないことにした
	if(melee_mode[0] == MELEE_MODE_GUN_HANDGUN && melee_mode[1] == MELEE_MODE_GUN_HANDGUN
		&& ref_skill_exp(SKILL_MARTIALARTS) >= 4800 && ref_skill_exp(TV_GUN) >= 4800)
	{
		if(flag_ammo) return MELEE_MODE_GUN_TWOHAND;
	}

	if(melee_mode[0] && (!melee_mode[1] || one_in_(2)))
		return melee_mode[0];
	else
		return melee_mode[1];

}

/*:::武器で攻撃したときのメッセージを「攻撃した」から変更する。*/
///pend 熟練度や武器ダイスやmodeに応じてもっと派手な表現にしてもいいかも
cptr msg_py_atk(object_type *o_ptr,int mode)
{
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	if(!o_ptr->k_idx)
	{
		msg_print(_("ERROR:武器を持っていないのにmsg_py_atk()が呼ばれた",
                    "ERROR: msg_py_atk() called while not wielding a weapon"));
		return "";
	}

	if(!ex_attack_msg) return _("を攻撃した。", "hit");

	if(tv == TV_KNIFE)
	{
		if(sv == SV_WEAPON_DOKUBARI)
			 return _("を毒液滴る針で刺した。", "use your poisoned needle to stab");
		else
		{
			if(one_in_(2)) return _("を斬った。", "cut");
			return _("を突いた。", "stab");
		}
	}

	if(tv == TV_SWORD)
	{
		//大剣
		if(sv == SV_WEAPON_CLAYMORE || sv == SV_WEAPON_GREAT_SWORD || sv == SV_WEAPON_EXECUTIONERS_SWORD)
		{
			if(one_in_(4)) return _("を斬った。", "cut");
			if(one_in_(3)) return _("へ刃を叩き付けた。", "slam your blade into");
			 return _("へ刃を振り下ろした。", "swing your blade down on");

		}
		//刺突剣
		else if(sv == SV_WEAPON_RAPIER)
		{
			if(one_in_(4)) return _("を斬った。", "cut");
			return _("を突いた。", "stab");

		}
		else
		{
			if(one_in_(4)) return _("を斬った。", "cut");
			if(one_in_(3)) return _("へ斬りつけた。", "slash");
			if(one_in_(2)) return _("を突いた。", "stab");
			return _("へ斬りかかった。", "slash");
		}


	}
	if(tv == TV_KATANA)
	{
			if(one_in_(6)) return _("へ刀を振り下ろした。", "swing your blade down on");
			if(one_in_(5)) return _("を斬り裂いた。", "cut down");
			if(one_in_(4)) return _("を斬った。", "cut");
			if(one_in_(3)) return _("へ斬りつけた。", "slash");
			if(one_in_(2)) return _("へ突きかかった。", "charge at");
			return _("へ斬りかかった。", "slash");
	}
	if(tv == TV_HAMMER)
	{
		if(sv == SV_WEAPON_FLAIL || sv == SV_WEAPON_BALL_AND_CHAIN)
		{
			if(one_in_(4)) return _("へ鉄球を直撃させた！", "slam the iron ball into");
			if(one_in_(3)) return _("へ武器を振り回した。", "swing the iron ball into");
			return _("を殴った。", "bash");
		}
		else
		{
			if(one_in_(4)) return _("を殴った。", "bash");
			if(one_in_(3)) return _("へ武器を振り下ろした。", "bring down your weapon on");
			if(one_in_(2)) return _("を殴りつけた。", "assault");
			return _("を打った。", "bludgeon");
		}
	}
	if(tv == TV_STICK)
	{
		if (o_ptr->name1 == ART_MEGUMU)
		{

			if (one_in_(3)) return _("を三脚でぶん殴った！", "use your tripod to bludgeon");
			if (one_in_(2)) return _("を三脚でぶっ叩いた！", "use your tripod to slam");
			return _("に三脚を突き刺した！", "use your tripod to stab");
		}
		else
		{
		    if(one_in_(4)) return _("を殴った。", "bash");
            if(one_in_(3)) return _("を打ち据えた。", "beat up");
            if(one_in_(2)) return _("を突いた。", "thrust at");
            return _("を薙ぎ払った。", "sweep");
		}
	}

	if(tv == TV_AXE)
	{
		if(one_in_(2)) return _("を叩き斬った。", "cleave");
		return _("へ武器を叩きつけた。", "bring down your weapon on");
	}

	if(tv == TV_SPEAR)
	{
		if(!one_in_(4)) return _("を突いた。", "impale");
		return _("を薙ぎ払った。", "sweep");
	}

	if(tv == TV_POLEARM)
	{
		if(sv == SV_WEAPON_WAR_SCYTHE || sv == SV_WEAPON_SCYTHE_OF_SLICING)
		{
			if(one_in_(2)) return _("を斬り裂いた。", "cut down");
			return _("を薙ぎ払った。", "sweep");
		}
		if(sv == SV_WEAPON_GLAIVE || sv == SV_WEAPON_NAGINATA)
		{
			if(one_in_(3)) return _("を斬った。", "cut");
			if(one_in_(2)) return _("へ刃を振り下ろした。", "bring down your blade on");
			return _("を突いた。", "impale");
		}
		if(sv == SV_WEAPON_LUCERNE_HAMMER)
		{
			if(one_in_(3)) return _("を殴った。", "bash");
			if(one_in_(2)) return _("を突いた。", "impale");
			return _("へハンマーを叩き付けた。", "slam your hammer down on");
		}
		if(sv == SV_WEAPON_HALBERD)
		{
			if(one_in_(3)) return _("を薙ぎ払った。", "sweep");
			if(one_in_(2)) return _("を突いた。", "impale");
			return _("へ刃を叩き付けた。", "slam your blade into");
		}


	}

	if(tv == TV_OTHERWEAPON)
	{
		if(sv == SV_OTHERWEAPON_FISHING)
		{
			if(one_in_(10)) return _("を吊り上げた！", "lift up");
			if(one_in_(2)) return _("を引っ叩いた。", "drag");
			return _("を打ち据えた。", "beat up");
		}
		else if(sv == SV_OTHERWEAPON_NEEDLE)
		{
			if(one_in_(3)) return _("を刺した！", "stab");
			if(one_in_(2)) return _("を突いた！", "thrust at");
			return _("に針を突き立てた！", "drive your needle into");
		}
	}

	return _("を攻撃した。", "hit");

}


/*:::@の現在の状態から素手攻撃の「分類」を決定する*/
//findmode:特殊攻撃モード 今のところ二つだけ
//  MELEE_FIND_NOHAND　手が使えずキックしか出ない　武器攻撃時の追加攻撃、両手に荷物を持っている時の格闘
//  MELEE_FIND_LONGARM 離れたマスまで攻撃する特殊攻撃　変容魔法「伸腕」か河童レイシャルの通背

///mod160111 v1.0.89で優先順位など見直し
///mod160325 v1.0.99でEXTRAモード対応のためまた見直し
int find_martial_arts_method(int findmode)
{
	int plev = p_ptr->lev;
	bool kick = FALSE;

	if(findmode == MELEE_FIND_NOHAND) kick = TRUE;

	//if (cheat_xtra)msg_format("findmode:%d", findmode);

/*優先1:特殊変身 装備品にかかわらず専用の攻撃をする*/
	switch(p_ptr->mimic_form)
	{
	case MIMIC_NUE://ぬえ変身
		return MELEE_MODE_NUE;
	case MIMIC_BEAST://巨大な獣
		return MELEE_MODE_BEAST;
	case MIMIC_SLIME://スライム化
		return MELEE_MODE_SLIME;
	case MIMIC_CAT://猫化
		return MELEE_MODE_CLAW;
	case MIMIC_DRAGON://竜化
		return MELEE_MODE_DRAGON;
	case MIMIC_MIST://霧化
		return MELEE_MODE_MIST;
	case MIMIC_KOSUZU_HYAKKI://v1.1.37 小鈴変身
		return MELEE_MODE_HYAKKI;
	default:
		break;
	}

	if (SUPER_SHION) return MELEE_MODE_SUPER_SHION;

/*優先2:特殊な一時効果*/
	//アイテムカード「毒爪「死なない殺人鬼」」をキョンシー以外が使った時
	if(findmode == MELEE_FIND_EX_CLAW) return MELEE_MODE_EX_CLAW;

	//職業「兵士」によるスニーキング即死攻撃
	if(findmode == MELEE_FIND_SNEAKING_KILL) return MELEE_MODE_SNEAKING;

	//愛宕様
	if(p_ptr->kamioroshi & KAMIOROSHI_ATAGO) return MELEE_MODE_ATAGO;
	//光速移動
	if(p_ptr->lightspeed) return MELEE_MODE_LIGHTSPEED;
	//弁々ダブルスコア演奏中
	if(p_ptr->pclass == CLASS_BENBEN && music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE)) return MELEE_MODE_BENBEN;
	//爪強化　このときkick限定にはならないはず
	if(p_ptr->clawextend) return MELEE_MODE_EX_CLAW;


	//銃装備時格闘
	//v2.0 特定の攻撃手段のときガン=カタのダメージボーナスを得られてしまうので不整合をなくすため優先度を少し上げた
	if (!kick && (inventory[INVEN_RARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_GUN))
	{
		//v1.1.21 ガン=カタ以外のとき1/2でキック攻撃
		int tmp = select_gun_melee_mode();
		//if (cheat_xtra) msg_format("gun_melee_mode:%d", tmp);
		if (tmp == MELEE_MODE_GUN_TWOHAND || !one_in_(2)) return tmp;
		else kick = TRUE;

	}



	//伸腕攻撃　変容魔法や河童レイシャル　追加格闘が出ないのでパンチのみ 一部クラスは専用攻撃優先
	if(findmode == MELEE_FIND_LONGARM)
	{

		if(p_ptr->pclass == CLASS_REMY) return MELEE_MODE_REMY_OTHER;
		else if(p_ptr->pclass == CLASS_MINDCRAFTER) return MELEE_MODE_MIND;
		else if(p_ptr->pclass == CLASS_MAMIZOU) return MELEE_MODE_MAMIZOU;
		else if(p_ptr->pclass == CLASS_SUWAKO) return MELEE_MODE_SUWAKO;
		else if(p_ptr->pclass == CLASS_YUKARI) return MELEE_MODE_YUKARI;
		else return MELEE_MODE_LONGARM;
	}
	//v1.1.26 菫子が銃格闘状態で遠隔攻撃をした時に普通の格闘効果しか出なくするための分岐
	if(findmode == MELEE_FIND_SUMIREKO_DISTANT)
	{
		return MELEE_MODE_SUMIREKO;
	}

	//v1.1.59 船に乗っているときは舟で体当りすることにした
	if (p_ptr->riding)
	{
		int riding_r_idx = m_list[p_ptr->riding].r_idx;

		if (riding_r_idx == MON_YOUR_BOAT || riding_r_idx == MON_ANCIENT_SHIP)
		{
			return MELEE_MODE_BOAT;
		}

	}

/*優先3:一部の種族特性や変異攻撃*/
	//キスメ
	if(p_ptr->pclass == CLASS_KISUME)
	{
		if(p_ptr->concent == 0) return MELEE_MODE_KISUME1;
		if(p_ptr->concent == 1) return MELEE_MODE_KISUME2;
		if(p_ptr->concent == 2) return MELEE_MODE_KISUME3;
		if(p_ptr->concent == 3) return MELEE_MODE_KISUME4;
		else return MELEE_MODE_KISUME5;
	}


	//v1.1.70 巨大化時は強力な攻撃(ガン=カタより優先度を下げた)
	if (p_ptr->mimic_form == MIMIC_GIGANTIC)
	{
		if (kick || one_in_(3))	return MELEE_MODE_GIGA_KICK;
		else			return MELEE_MODE_GIGA_PUNCH;
	}

		//魔王変化時1/3で強力な攻撃
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD && one_in_(3))
		return MELEE_MODE_D_LORD;

	//吸血鬼(レミリアは別処理)が1/4で吸血攻撃
	if((prace_is_(RACE_VAMPIRE) || p_ptr->mimic_form == MIMIC_VAMPIRE) && p_ptr->pclass != CLASS_REMY && one_in_(4))
		return MELEE_MODE_VAMP;

	//種族ニンジャのとき1/3
	if(p_ptr->prace == RACE_NINJA && one_in_(3))
	{
		if(kick || one_in_(3)) return MELEE_MODE_KICK_NINJA;
		else return MELEE_MODE_PUNCH_NINJA;
	}
	//種族究極生命体のとき1/3
	else if(p_ptr->prace == RACE_ULTIMATE && one_in_(3))
	{
		if(one_in_(10)) return MELEE_MODE_FISH_TAIL;
		else if(one_in_(9)) return MELEE_MODE_BIGHORN;
		else if(one_in_(8)) return MELEE_MODE_BIGTAIL;
		else if(one_in_(7)) return MELEE_MODE_VAMP;
		else 	return MELEE_MODE_ULTIMATE;
	}

	//変異
	if(p_ptr->muta2 & MUT2_BIGHORN && one_in_(10)) return MELEE_MODE_BIGHORN;
	if(p_ptr->muta2 & MUT2_BIGTAIL && one_in_(8)) return MELEE_MODE_BIGTAIL;
	if(p_ptr->muta2 & MUT2_HORNS && one_in_(14)) return MELEE_MODE_HORNS;
	if(p_ptr->muta2 & MUT2_HARDHEAD && one_in_(7)) return MELEE_MODE_HEADBUTT;

	//v1.1.78 酔拳
	if (p_ptr->special_attack & ATTACK_SUIKEN)
		return MELEE_MODE_SUIKEN;


/*優先4:専用攻撃が出る職業 */
	///pend 多くの職業は手が塞がっててもパンチとか出てしまうので暇があれば直したい
	switch(p_ptr->pclass)
	{
	case CLASS_MINDCRAFTER:
		return MELEE_MODE_MIND;
	case CLASS_NINJA:
		return MELEE_MODE_NINJA;

	//格闘家の専用攻撃は素手時のみ
	case CLASS_MARTIAL_ARTIST:
		if((randint0(100) < plev+30) && !kick) return MELEE_MODE_MA;
		else break;
	case CLASS_YUKARI:
		return MELEE_MODE_YUKARI;

	case CLASS_KOISHI:
		return MELEE_MODE_KOISHI;
	//v1.1.23
	case CLASS_NITORI:
		return MELEE_MODE_NITORI;
	case CLASS_CIRNO:
		return MELEE_MODE_CIRNO;
	case CLASS_YOSHIKA:
		return MELEE_MODE_YOSHIKA;
	case CLASS_HINA:
		return MELEE_MODE_HINA;
	case CLASS_MEDICINE:
		return MELEE_MODE_MEDI;
	case CLASS_ICHIRIN:
		return MELEE_MODE_ICHIRIN;
	case CLASS_SUMIREKO:
		return MELEE_MODE_SUMIREKO;
	case CLASS_FUTO:
		return MELEE_MODE_FUTO;
	case CLASS_TOZIKO:
		return MELEE_MODE_TOZIKO;
	case CLASS_YUYUKO:
		return MELEE_MODE_YUYUKO;
	case CLASS_MOKOU:
		return MELEE_MODE_MOKOU;
	case CLASS_MEIRIN:
		return MELEE_MODE_MEIRIN;
	case CLASS_3_FAIRIES:
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			return MELEE_MODE_3_FAIRIES_2; //v1.1.68
		else
			return MELEE_MODE_3_FAIRIES;
	case CLASS_KANAKO:
		return MELEE_MODE_KANAKO;
	case CLASS_RAIKO:
		return MELEE_MODE_RAIKO;
	case CLASS_MARISA:
		return MELEE_MODE_MARISA;
	case CLASS_MAMIZOU:
		return MELEE_MODE_MAMIZOU;

	case CLASS_MURASA:
		{ //村紗は自分のいる地形で攻撃が変わる
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) && !one_in_(3)) return MELEE_MODE_MURASA2;
			else  return MELEE_MODE_MURASA1;
		}
	case CLASS_MIKO:
		return MELEE_MODE_MIKO;
	case CLASS_KOKORO:
		return MELEE_MODE_KOKORO;
	case CLASS_UTSUHO:
		return MELEE_MODE_UTSUHO;
	case CLASS_TENSHI:
		return MELEE_MODE_TENSHI;
	case CLASS_SUWAKO:
		return MELEE_MODE_SUWAKO;
	case CLASS_SHINMYOU_2:
		//アイテム「天人の丹」で種族仙人になったらお椀の格闘が使えないことにする
		if(p_ptr->prace != RACE_SENNIN)
			return MELEE_MODE_SHINMYOU2;
		else break;
	case CLASS_REIMU: //霊夢は賽銭ランクで攻撃の種類が変わる
		if(1 + randint1(10) < osaisen_rank()) return MELEE_MODE_REIMU2;
		else return MELEE_MODE_REIMU1;
	case CLASS_RAN: //藍の回転攻撃は1/4
		if(one_in_(4)) return MELEE_MODE_RAN;
		else break;
	case CLASS_CHEN: //橙の特殊攻撃は濡れていない時にしか出ない
		if( !p_ptr->magic_num1[0]) return MELEE_MODE_CHEN;
		else break;
	case CLASS_BYAKUREN: //白蓮の特殊攻撃は強化中のみ
		if(p_ptr->tim_general[0]) return MELEE_MODE_HIZIRI;
		else break;
	case CLASS_IKU:
		if(!kick) return MELEE_MODE_DRILL;
		else break;

	case CLASS_REMY:
		if(one_in_(3) && !kick) return MELEE_MODE_REMY_HAND;
		else if(one_in_(2)) return MELEE_MODE_REMY_KICK;
		else return MELEE_MODE_REMY_OTHER;
	case CLASS_KASEN:
		if(is_special_seikaku(SEIKAKU_SPECIAL_KASEN))//v1.1.65
			return MELEE_MODE_ONI_KASEN;
		else
			return MELEE_MODE_KASEN;

	case CLASS_UDONGE:
		return MELEE_MODE_UDONGE;

	case CLASS_ORIN:
		//v1.1.77 お燐の特殊格闘(猫車)は特殊性格でないときのみ
		if(one_in_(3)&& !is_special_seikaku(SEIKAKU_SPECIAL_ORIN)) return MELEE_MODE_ORIN;
		else break;

	case CLASS_DOREMY:
		return MELEE_MODE_DOREMY;
		break;

	case CLASS_JYOON:
		if(is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
			return MELEE_MODE_JYOON_3;
		else if (p_ptr->special_attack & ATTACK_WASTE_MONEY)
			return MELEE_MODE_JYOON_2;
		else
			return MELEE_MODE_JYOON_1;




	}




/*優先5:一般的な攻撃手段から選ぶ*/

	if(!kick)//パンチもキックも使えるとき、どちらになるか選定
	{
		int kick_chance;

		if(p_ptr->pclass == CLASS_SHIZUHA || p_ptr->pclass == CLASS_ALICE || p_ptr->pclass == CLASS_SAKI) //静葉とアリスはキックしか出ない v1.1.71 早鬼追加
			kick_chance = 100;
		else if(p_ptr->pclass == CLASS_WRIGGLE || p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_KUTAKA)//リグルと弁々はキックが出やすい
			kick_chance = 70;
		else if(p_ptr->prace == RACE_NINGYO) //人魚はキック(=人魚の尾)が出やすい
			kick_chance = 50;
		else
			kick_chance = 35;

		if( randint0(100) < kick_chance ) kick = TRUE;

	}

	if(kick)
	{
		//人魚の尾があるとき蹴り攻撃は必ず尾攻撃になる
		if(p_ptr->muta3 & MUT3_FISH_TAIL) return MELEE_MODE_FISH_TAIL;
		//キョンシーの蹴り攻撃は全部噛み付きになる
		if(prace_is_(RACE_ZOMBIE)) return MELEE_MODE_BITE;

		//v1.1.71 早鬼 強力なキックが出やすい
		if (p_ptr->pclass == CLASS_SAKI)
		{
			if(randint1(60) < p_ptr->lev)
				return MELEE_MODE_KICK2;
			else
				return MELEE_MODE_KICK1;
		}

		//格闘の強い職は強力なキック
		if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000 || p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			return MELEE_MODE_KICK2;

		//鬼の蹴りは確率で角攻撃になる
		if(prace_is_(RACE_ONI) && one_in_(2)) return MELEE_MODE_BIGHORN;

		//白狼天狗、獣人の蹴りも確率で噛み付き攻撃になる(噛まなそうなイメージのキャラは除外)
		if(prace_is_(RACE_HAKUROU_TENGU) && one_in_(2)) return MELEE_MODE_BITE;
		if(p_ptr->prace == RACE_WARBEAST && one_in_(2)
			&& p_ptr->pclass != CLASS_KEINE && p_ptr->pclass != CLASS_ORIN && p_ptr->pclass != CLASS_NAZRIN && p_ptr->pclass != CLASS_SHOU)
			return MELEE_MODE_BITE;

		return MELEE_MODE_KICK1; //普通の蹴り
	}
	else
	{

		//八橋のパンチは琴の爪で攻撃
		if(p_ptr->pclass == CLASS_YATSUHASHI )	return MELEE_MODE_CLAW;

		//パンチせず引っ掻きそうなイメージのキャラ
		if(p_ptr->pclass == CLASS_YAMAME || p_ptr->pclass == CLASS_MYSTIA || p_ptr->pclass == CLASS_KAGEROU || p_ptr->pclass == CLASS_ORIN)
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000) return MELEE_MODE_EX_CLAW;
			else return MELEE_MODE_CLAW;
		}
		//たまに引っ掻いたり噛み付いたりしそうなキャラ
		if(p_ptr->pclass == CLASS_RUMIA)
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 5000) return MELEE_MODE_EX_CLAW;
			else if(one_in_(3)) return MELEE_MODE_CLAW;
			else if(one_in_(2)) return MELEE_MODE_PUNCH1;
			else return MELEE_MODE_BITE;
		}


		//格闘の強い職は強力なパンチ
		if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000 || p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			return MELEE_MODE_PUNCH2;

		//キョンシー特殊処理
		if(prace_is_(RACE_ZOMBIE))
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000) return MELEE_MODE_YOSHIKA;
			else if(one_in_(2)) return MELEE_MODE_BITE;
			else return MELEE_MODE_CLAW;
		}

		if(prace_is_(RACE_ONI)) return MELEE_MODE_PUNCH_ONI;
		if(prace_is_(RACE_DAIYOUKAI) && one_in_(2)) return MELEE_MODE_EX_CLAW;
		if(prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE && one_in_(2)) return MELEE_MODE_CLAW;

		if (prace_is_(RACE_ANIMAL_GHOST) && one_in_(2)) return MELEE_MODE_CLAW;

		return MELEE_MODE_PUNCH1; //普通の殴り
	}

}


/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
/*:::手による攻撃 素手の時hand=0で一度呼ばれる
 *:::x,y:対象モンスターがいる位置
 *:::hand:0なら右手、1なら左手
 *:::mode:必殺剣など特殊な通常攻撃のとき以外0*/

///class sys item 格闘処理　手による攻撃処理
static void py_attack_aux(int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance;
	//int vir;

	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	/* Access the weapon */
	object_type     *o_ptr = &inventory[INVEN_RARM + hand];

	char            m_name[80];

	bool            success_hit = FALSE;
	bool            backstab = FALSE;
	bool            sneakingkill = FALSE; //職業「兵士」用バックスタッブ
	bool            vorpal_cut = FALSE;
	int             chaos_effect = 0;
	bool            stab_fleeing = FALSE;
	bool            fuiuchi = FALSE;
	bool            monk_attack = FALSE;
	bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE; //吸血できるか　
	bool            can_feed = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */ /*←吸血だけでなく切れ味にも使うことにした*/
	bool            is_human = (r_ptr->d_char == 'p');
	bool            is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));
	bool            zantetsu_mukou, e_j_mukou;

	bool			weapon_invalid = FALSE;

	//刀熟練度ボーナスの「必殺の一太刀」が出たときのフラグ　扱いはfuiuchi+忍者ヒットと同じ
	bool			flag_katana_critical = FALSE;

	//二丁拳銃格闘　残弾消費
	bool		flag_gun_kata = FALSE;

	//v1.1.42 ドレミー・スイートの格闘攻撃でモンスターを眠らせるフラグカウント
	bool		counting_sheep = 0;

	bool	flag_vamp_armour = FALSE; //防具による吸血があるかどうか

	int			martial_arts_method = 0;//格闘のときにランダムに選ばれる格闘分類タイプ MELEE_MODE_***

	//v1.1.94
	int		skill_type = 0;	//該当するスキル ref_skill_exp()に使う

	int i;

	//v1.1.69 潤美特技発動のとき武器攻撃が加重モードになる
	// py_attack()中に記述すると追加格闘が出なくなるのでここに書いた
	if (p_ptr->pclass == CLASS_URUMI && !mode && p_ptr->tim_general[0] && !p_ptr->do_martialarts)
	{
		mode = HISSATSU_URUMI;
	}

	//v2.0.2　特技「刃こぼれするまで切り刻め」は刃の付いた武器でないと発動しない
	if (mode == HISSATSU_HAKOBORE && !object_has_a_blade(o_ptr)) return;

	//v1.1.94 槍によるカウンター攻撃は槍でないと発動しない
	if (mode == HISSATSU_COUNTER_SPEAR && o_ptr->tval != TV_SPEAR) return;

	///mod140914 剣術家の必殺技では向いていない武器の攻撃が無効化される
	if((mode == HISSATSU_2 || mode == HISSATSU_ISSEN || mode == HISSATSU_UNDEAD) && o_ptr->tval != TV_KATANA) return;
	if(mode == HISSATSU_MAJIN && o_ptr->tval != TV_AXE) return;
	if((mode == HISSATSU_3WAY || mode == HISSATSU_HAGAN )&& !object_has_a_blade(o_ptr)) return;
	if((mode == HISSATSU_ZANMA || mode == HISSATSU_FUKI || mode == HISSATSU_COLD || mode == HISSATSU_ELEC || mode == HISSATSU_DOUBLE) && o_ptr->tval != TV_KATANA && o_ptr->tval != TV_SWORD) return;
	if(mode == HISSATSU_QUAKE && o_ptr->weight < 100) return;
	if(mode == HISSATSU_3DAN && o_ptr->tval != TV_SPEAR && o_ptr->tval != TV_POLEARM) return;

	//v1.1.14 祇園様特殊処理
	if(mode == HISSATSU_GION && o_ptr->tval != TV_KATANA && o_ptr->tval != TV_SWORD) return;

	///mod140216 格闘処理変更
	//if(!o_ptr->k_idx && !p_ptr->riding) monk_attack = TRUE;
	//if(empty_hands(TRUE) & EMPTY_HAND_RARM) monk_attack = TRUE;
	if(p_ptr->do_martialarts) monk_attack = TRUE;

	if(check_invalidate_inventory(INVEN_RARM + hand)) weapon_invalid = TRUE;


	if (check_equip_specific_fixed_art(ART_ECSEDI, TRUE)) flag_vamp_armour = TRUE;

	//v1.1.99 吸血フラグ付きの防具を着けているとき両手に吸血効果が乗る
	//もとは★エリーザベトだけだったが大怨霊エゴグローブも追加したので扱いを整理
	for (i = INVEN_RIBBON; i <= INVEN_FEET; i++)
	{
		object_type *tmp_o_ptr = &inventory[i];
		u32b tmp_flgs[TR_FLAG_SIZE];

		object_flags(tmp_o_ptr, tmp_flgs);

		if (have_flag(tmp_flgs, TR_VAMPIRIC))
		{
			if (cheat_xtra) msg_print("TESTMEG:VAMP ARMOUR");
			flag_vamp_armour = TRUE;
			break;
		}
	}

	//職業ごとに不意打ち判定などの特殊処理
	switch (p_ptr->pclass)
	{
	case CLASS_KOISHI:
		{
			int tmp = p_ptr->lev * 6;
			if(r_ptr->flags2 & RF2_EMPTY_MIND) tmp = 0;
			else if(r_ptr->flags2 & RF2_WEIRD_MIND) tmp /= 4;
			else if(r_ptr->flags2 & RF2_ELDRITCH_HORROR) tmp /= 2;
			else if(!monster_living(r_ptr)) tmp = tmp * 2 / 3;
			if(randint0(tmp) > r_ptr->level) fuiuchi = TRUE;

		}
		break;

	case CLASS_SOLDIER:

		//v1.1.21 兵士(スニーキング技能持ち)の不意打ち判定 失敗したら普通のバックスタッブ
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_SNEAKING))
		{
			int tmp = r_ptr->level;
			bool flag_fail = FALSE;

			//寝てる敵でないと無効
			//念のために何らかの攻撃モード中は無効にしとく
			if (MON_CSLEEP(m_ptr) && m_ptr->ml  && !mode)
			{

				if(((r_ptr->flags1 & RF1_UNIQUE) ||(r_ptr->flags7 & RF7_UNIQUE2))) flag_fail = TRUE;
				if(!monster_living(r_ptr)) flag_fail = TRUE;

				if(r_ptr->flags2 & RF2_GIGANTIC) tmp *= 2;
				if(!(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN))) tmp *= 3;

				if(flag_fail || randint1(tmp) > randint1(p_ptr->lev))
					backstab = TRUE;
				else
					sneakingkill = TRUE;
			}
		}
		//v1.1.22 暗視技能のときの不意打ち判定
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_NOCTO_VISION) && !(cave[py][px].info & CAVE_GLOW) && p_ptr->cur_lite <= 0 )
		{

			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
				backstab = TRUE;
			else if(check_mon_blind(c_ptr->m_idx))
				fuiuchi = TRUE;

		}
		//v1.1.22 潜伏技能の不意打ち判定
		if(CHECK_CONCEALMENT)
		{
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
				backstab = TRUE;
			else if (randint1(r_ptr->level * 2) < (p_ptr->lev + p_ptr->skill_stl * 2))
				fuiuchi = TRUE;
		}

		break;

	/*:::盗賊と忍者の特殊打撃*/
		///mod141221 忍者特殊判定を書き換え
	case CLASS_NINJA:
		if (monk_attack || o_ptr->tval == TV_KNIFE)
		{
			int tmp = p_ptr->lev * 6 + (p_ptr->skill_stl + 10) * 4;
			if (p_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
			if (p_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
			if (r_ptr->level > (p_ptr->lev * p_ptr->lev / 20 + 10)) tmp /= 3;
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
			{
				/* Can't backstab creatures that we can't see, right? */
				backstab = TRUE;
			}
			else if (((p_ptr->special_defense & NINJA_S_STEALTH) || p_ptr->tim_superstealth) && (randint0(tmp) > (r_ptr->level+20)) && m_ptr->ml && !(r_ptr->flagsr & RFR_RES_ALL))
			{
				fuiuchi = TRUE;
			}
			else if (MON_MONFEAR(m_ptr) && m_ptr->ml)
			{
				stab_fleeing = TRUE;
			}
		}
		break;
	case CLASS_CHEN:
			if ( (mode == HISSATSU_NYUSIN && randint1(r_ptr->level * 2) < p_ptr->lev || MON_CSLEEP(m_ptr)) && m_ptr->ml)
			{
				fuiuchi = TRUE;
			}
			break;

		///mod140817 ミスティア鳥目処理
	case CLASS_MYSTIA:
		if(check_mon_blind(c_ptr->m_idx)) fuiuchi = TRUE;
		break;

	case CLASS_NUE:
			if ( (p_ptr->cur_lite <= 0 && !(cave[py][px].info & CAVE_GLOW) && randint1(r_ptr->level * 2) < p_ptr->lev || MON_CSLEEP(m_ptr)) && m_ptr->ml)
			{
				fuiuchi = TRUE;
			}
			break;
			//雷鼓プリスティンビート中の攻撃速度上昇処理
	case CLASS_RAIKO:
		if(music_singing(MUSIC_NEW_RAIKO_PRISTINE) && !hand)
			do_cmd_concentrate(0);
		break;


	/*:::格闘技能者の格闘判定*/
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
	//case CLASS_BERSERKER:
		if ((empty_hands(TRUE) & EMPTY_HAND_RARM) && !p_ptr->riding) monk_attack = TRUE;
		break;
	}

	//職業以外による不意打ち判定など
	if (!fuiuchi && !backstab && !stab_fleeing)
	{
		if (p_ptr->tim_unite_darkness)
		{
			if (!(cave[py][px].info & CAVE_GLOW) && m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
		}

		if (o_ptr->tval == TV_KATANA && check_katana_critical(m_ptr, mode))
		{
			fuiuchi = TRUE;
			flag_katana_critical = TRUE;
		}
		if (mode == HISSATSU_FUIUCHI)
			fuiuchi = TRUE;

		//稠密の隠形鬼　隠密能力に応じて不意打ち判定　「闇との融和」と同じ判定でいいか
		if (mode == HISSATSU_ONGYOU)
		{
			if (m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
		}

	}


	/*:::素手のとき格闘技能を得る*/
	//if (!o_ptr->k_idx) /* Empty hand */
	if(monk_attack)
	{
		skill_type = SKILL_MARTIALARTS;

		if(p_ptr->pclass == CLASS_MEIRIN)
			do_cmd_concentrate(0);
		///mod131226 skill 技能と武器技能の統合
		gain_skill_exp(skill_type, m_ptr);
		/*
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			if (p_ptr->skill_exp[GINOU_SUDE] < s_info[p_ptr->pclass].s_max[GINOU_SUDE])
			{
				if (p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_BEGINNER)
					p_ptr->skill_exp[GINOU_SUDE] += 40;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_SKILLED))
					p_ptr->skill_exp[GINOU_SUDE] += 5;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19))
					p_ptr->skill_exp[GINOU_SUDE] += 1;
				else if ((p_ptr->lev > 34))
					if (one_in_(3)) p_ptr->skill_exp[GINOU_SUDE] += 1;
				p_ptr->update |= (PU_BONUS);
			}
		}
		*/
	}
	/*:::武器のとき対応する経験値を得る*/
	//v1.1.14 祇園様発動効果は除く
	else if (object_is_melee_weapon(o_ptr) && mode != HISSATSU_GION)
	{

		skill_type = o_ptr->tval;

		gain_skill_exp(skill_type,m_ptr);
	/*
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			int tval = inventory[INVEN_RARM+hand].tval - TV_WEAPON_BEGIN;
			int sval = inventory[INVEN_RARM+hand].sval;
			int now_exp = p_ptr->weapon_exp[tval][sval];
			if (now_exp < s_info[p_ptr->pclass].w_max[tval][sval])
			{
				int amount = 0;
				if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
				else if (now_exp < WEAPON_EXP_SKILLED) amount = 10;
				else if ((now_exp < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19)) amount = 1;
				else if ((p_ptr->lev > 34) && one_in_(2)) amount = 1;
				p_ptr->weapon_exp[tval][sval] += amount;
				p_ptr->update |= (PU_BONUS);
			}
		}
	*/
		///mod140714 付喪神使いがしばらく武器を使っていると武器が妖器化する処理
		///mod151219 雷鼓追加
		//v1.1.52 新アリーナ内では妖器化しないようにする
		if((p_ptr->pclass == CLASS_TSUKUMO_MASTER || p_ptr->pclass == CLASS_RAIKO)
			&& !object_is_fixed_artifact(o_ptr) && o_ptr->xtra1 < 100 && !p_ptr->inside_arena)
		{
			if( (r_ptr->level > o_ptr->xtra1 / 2) && (randint0(o_ptr->xtra1) < r_ptr->level) && one_in_(5))
			{
				if(p_ptr->wizard) o_ptr->xtra1 = (o_ptr->xtra1 < 50) ? 50 : 100;
				else o_ptr->xtra1++;

				if(o_ptr->xtra1 == 50 || o_ptr->xtra1 == 100) make_evilweapon(o_ptr,r_ptr);
			}
		}


	}

	/* Disturb the monster */
	(void)set_monster_csleep(c_ptr->m_idx, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);
	/*:::命中判定など*/
	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h[hand] + o_ptr->to_h;
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	if (cheat_xtra)
	{
		msg_format("chance:%d bonus : %d",chance,bonus);

	}

	if (mode == HISSATSU_COUNTER_SPEAR) chance += 60;

	if (mode == HISSATSU_IAI) chance += 60;
	if (mode == HISSATSU_GION) chance += 60;
	if (p_ptr->special_defense & KATA_KOUKIJIN) chance += 150;

	if(mode == HISSATSU_EIGOU)
	{
		chance += 100;
		if(!heavy_armor()) chance *= 3;
	}
	if (p_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	//msg_format("chance:%d",chance);

	///sysdel virtue
	//vir = virtue_number(V_VALOUR);
	/*
	if (vir)
	{
		chance += (p_ptr->virtues[vir - 1]/10);
	}
	*/
	///item 斬鉄剣とEJ処理
	///del131223 斬鉄剣制限解除
	//zantetsu_mukou = ((o_ptr->name1 == ART_ZANTETSU) && (r_ptr->d_char == 'j'));
	zantetsu_mukou = 0;

	//シンボル変更S→c
	e_j_mukou = ((o_ptr->name1 == ART_EXCALIBUR_J) && (r_ptr->d_char == 'c'));

	//ここから攻撃回数設定

	//必殺剣、毒針
	if ((mode == HISSATSU_COUNTER_SPEAR) || (mode == HISSATSU_ATTACK_ONCE) || (mode == HISSATSU_KYUSHO) || (mode == HISSATSU_MINEUCHI) || (mode == HISSATSU_3DAN) || (mode == HISSATSU_IAI))
	{
		num_blow = 1;
	}
	else if (mode == HISSATSU_COLD) num_blow = p_ptr->num_blow[hand]+2;
	/*:::河童レイシャルの通背　攻撃回数が距離に応じ減少*/
	else if (mode == HISSATSU_KAPPA)
	{
			num_blow = p_ptr->num_blow[hand] / m_ptr->cdis;
			if(num_blow < 1) num_blow=1;
	}
	/*:::変異術の伸腕　距離に応じ少し減少*/
	else if (mode == HISSATSU_LONGARM)
	{
			num_blow = p_ptr->num_blow[hand] - m_ptr->cdis / 2;
			if(num_blow < 1) num_blow=1;
	}
	//ソウルスカルプチュア　敵との距離で攻撃回数が増減
	else if(mode == HISSATSU_SOULSCULPTURE)
	{
		if(p_ptr->pclass == CLASS_SAKUYA) num_blow = p_ptr->num_blow[hand] * 7 / (m_ptr->cdis+2);
		else if (p_ptr->pclass == CLASS_NEMUNO)
		{
			if(IS_NEMUNO_IN_SANCTUARY)
				num_blow = p_ptr->num_blow[hand] * 4 / (m_ptr->cdis + 1);
			else
				num_blow = p_ptr->num_blow[hand] * 3 / (m_ptr->cdis + 1);
		}
		else if (p_ptr->pclass == CLASS_KEIKI )
		{
			num_blow = p_ptr->num_blow[hand];
		}
		else if (p_ptr->pclass == CLASS_MEGUMU)
		{
			num_blow = (p_ptr->num_blow[hand]+1)/2;
		}

		else
		{
			msg_print(_("ERROR:予期しないクラスでGF_SOULSCULPTUREが使われた",
                        "ERROR: GF_SOULSCULPTURE used by wrong class"));
			num_blow = p_ptr->num_blow[hand];
		}

		if(num_blow < 1) num_blow=1;
	}
	else num_blow = p_ptr->num_blow[hand];


	if(mode == HISSATSU_SHINMYOU) num_blow = num_blow * 3 / 2;
	else if(mode == HISSATSU_UNZAN) num_blow = num_blow * (20+randint0(11)) / 10;
	else if (mode == HISSATSU_HAKOBORE) num_blow *= 2;


	/* Hack -- DOKUBARI always hit once */
	///mod131223 tval
	//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) num_blow = 1;
	///変身時に毒針装備してたら攻撃回数1にならないようにしておく
	if (!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) num_blow = 1;

	/* Attack once for each legal blow */
	/*:::攻撃回数ごとに判定*/
	while ((num++ < num_blow) && !p_ptr->is_dead)
	{
		//v1.1.94
		int		attack_effect_type = ATTACK_EFFECT_NONE; //朦朧など敵にステータス異常を付与するとき
		int		critical_rank = 0; //クリティカルや切れ味や不意打ちなどが決まったとき増える
		int		effect_turns = 0; //朦朧などのステータス異常を与えるターン数

		bool dokubari = FALSE;
		bool sinker = FALSE;

		int mon_ac = r_ptr->ac;

		//v1.1.94 モンスター防御力低下中はAC25%カット
		if (MON_DEC_DEF(m_ptr))
		{
			mon_ac = MONSTER_DECREASED_AC(mon_ac);
		}

		///sys item 毒針命中判定
		///mod131223 tval
		if ((!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
		{
			int n = 1;
			dokubari = TRUE;

			if (p_ptr->migite && p_ptr->hidarite)
			{
				n *= 2;
			}
			if (mode == HISSATSU_3DAN)
			{
				n *= 2;
			}
			///mod141129 LUNATICで毒針が少し当たりにくい

			if(difficulty == DIFFICULTY_LUNATIC && one_in_(2)) success_hit = one_in_(n*2);
			else success_hit = one_in_(n);
		}
		///class 忍者の不意打ちは必ず命中
		//else if ((p_ptr->pclass == CLASS_NINJA) && ((backstab || fuiuchi) && !(r_ptr->flagsr & RFR_RES_ALL))) success_hit = TRUE;
		else if (((backstab || fuiuchi) && !(r_ptr->flagsr & RFR_RES_ALL))) success_hit = TRUE;
		else if(sneakingkill) success_hit = TRUE;
		///映姫は攻撃が必中
		else if(p_ptr->pclass == CLASS_EIKI) success_hit = TRUE;
		/*:::通常の命中処理*/
		else success_hit = test_hit_norm(chance, mon_ac, m_ptr->ml);

		//v1.1.94
		if (backstab || fuiuchi || stab_fleeing) critical_rank = 3;

		///mod141102 さとり命中率特殊補正..しようかと思ったけどやめた
		//if(p_ptr->pclass == CLASS_SATORI && !p_ptr->blind && !p_ptr->image && p_ptr->confused && m_ptr->ml && !(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
		//	&& (randint1(r_ptr->level*3 + 100) < (p_ptr->lev + p_ptr->stat_ind[A_INT]+3) )) success_hit = TRUE;

		/*
		if (mode == HISSATSU_MAJIN)
		{
			if (one_in_(2))
				success_hit = FALSE;
		}
		*/

		/* Test for hit */
		if (success_hit)
		{
			///sys item *切れ味*処理 フラグ処理にした
			//int vorpal_chance = ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)) ? 2 : 4;
			int vorpal_chance;

			/* Sound */
			sound(SOUND_HIT);

			if(p_ptr->prace == RACE_STRAYGOD && (backstab || stab_fleeing || fuiuchi || sneakingkill))
			{
				if(fuiuchi)
				{
					if(one_in_(10)) set_deity_bias(DBIAS_COSMOS, -1);
				}
				else
				{
					if(one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);
					if(one_in_(2)) set_deity_bias(DBIAS_REPUTATION, -1);
				}
			}

			/* Message */
			///sys 攻撃時メッセージ　武器ごとに変更しようか？
			if (p_ptr->pclass == CLASS_KISUME && p_ptr->concent && !monk_attack)
			{
				 msg_format(_("あなたは武器を構えて%sへ向けて落下した！",
                                "You hold your weapon and drop down on %s!"), m_name);
			}
			else if(mode == HISSATSU_GION)
            {
				msg_format(_("祇園様の剣が%sを切り裂いた！",
                                "Sword of Gion cuts down %s!"), m_name);
            }
			else if(sneakingkill && monk_attack)
            {
                ; //兵士の格闘不意打ちはここではメッセージを出さない
            }
			else if (backstab || sneakingkill)
            {
				msg_format(_("あなたは冷酷にも眠っている無力な%sへ奇襲を仕掛けた！",
                                "You cruelly attack the helpless sleeping %s!"), m_name);
            }
			else if (fuiuchi)
			{
				if(flag_katana_critical)
					msg_format(_("%sに気合の一撃を放った！",
                                "You focus and strike at %s!"), m_name);
				else
                    msg_format(_("不意を突いて%sに強烈な一撃を喰らわせた！",
                                "You deliver a sudden, powerful blow to %s!"), m_name);
			}
			else if (stab_fleeing)
				msg_format(_("逃げる%sへ背中から襲いかかった！",
                                "You backstab the fleeing %s!"), m_name);
			//else if (!monk_attack) msg_format("%sを攻撃した。", m_name);
			else if (!monk_attack)
#ifdef JP
				msg_format("%s%s", m_name,msg_py_atk(o_ptr,mode));
#else
                msg_format("You %s %s.",msg_py_atk(o_ptr,mode), m_name);
#endif
			//else msg_format("%sを攻撃した。", m_name);

			//雛の厄消費
			if(p_ptr->pclass == CLASS_HINA)
				hina_gain_yaku(-(p_ptr->to_dd[hand]) / 2);

			/* Hack -- bare hands do one damage */
			k = 1;

			object_flags(o_ptr, flgs);



			/*:::混沌武器の特殊効果計算*/
			/* Select a chaotic effect (50% chance) */
			if ((have_flag(flgs, TR_CHAOTIC)) && one_in_(2) && !weapon_invalid)
			{
				///sysdel virtue
				//if (one_in_(10))
				//chg_virtue(V_CHANCE, 1);

				if (randint1(5) < 3)
				{
					/* Vampiric (20%) */
					chaos_effect = 1;
				}
				else if (one_in_(250))
				{
					/* Quake (0.12%) */
					chaos_effect = 2;
				}
				else if (!one_in_(10))
				{
					/* Confusion (26.892%) */
					chaos_effect = 3;
				}
				else if (one_in_(2))
				{
					/* Teleport away (1.494%) */
					chaos_effect = 4;
				}
				else
				{
					/* Polymorph (1.494%) */
					chaos_effect = 5;
				}
			}

			/* Vampiric drain */
			/*:::吸血処理*/
			///mod140410 Lv45以上で魔剣を使うと吸血付加*/
			if ((have_flag(flgs, TR_VAMPIRIC) ) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || hex_spelling(HEX_VAMP_BLADE) || ((p_ptr->special_attack & (ATTACK_DARK)) && p_ptr->lev > 44))
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;

				if(weapon_invalid) can_drain = FALSE;
			}
			//v1.1.99 吸血防具フラグ
			if(monster_living(r_ptr) && flag_vamp_armour)
			{
					can_drain = TRUE;
			}

			//芳香特殊攻撃
			if(monster_living(r_ptr) && mode == HISSATSU_YOSHIKA)
			{
					can_drain = TRUE;
			}

			//v1.1.46 切れ味判定(vorpal_cut)処理がここにあったが下の方に移した


			/*:::素手攻撃時のメッセージ、威力、効果再計算*/
			///class 素手攻撃エフェクト
			if (monk_attack)
			{
				int mult = 100;
				int special_effect = 0, times = 0, max_times;
				//int stun_effect = 0;
				int min_level = 1;
				//const martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
				const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
				//int resist_stun = 0;
				int weight = 8;
				int findmode = 0;

				if(mode == HISSATSU_KAPPA || mode == HISSATSU_LONGARM) findmode = MELEE_FIND_LONGARM;
				else if(sneakingkill) findmode = MELEE_FIND_SNEAKING_KILL;
				else if(mode == HISSATSU_YOSHIKA && p_ptr->prace != RACE_ZOMBIE) findmode = MELEE_FIND_EX_CLAW;
				//v1.1.26 菫子が銃格闘状態で遠隔攻撃を選択したときは普通の格闘しか出ないようにする
				else if(mode == HISSATSU_NO_AURA && p_ptr->pclass == CLASS_SUMIREKO) findmode = MELEE_FIND_SUMIREKO_DISTANT;

				/*:::種族職業レベル突然変異などに応じ攻撃分類を決定する*/
				martial_arts_method = find_martial_arts_method(findmode);

				//ガンカタフラグ
				if(martial_arts_method == MELEE_MODE_GUN_TWOHAND)
					flag_gun_kata = TRUE;

				if(mode == HISSATSU_UNZAN) martial_arts_method = MELEE_MODE_ICHIRIN2;

				/* v1.1.94 ここの処理はapply_special_effect()に移動
				if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += 88;
				if (r_ptr->flags2 & RF2_GIGANTIC) resist_stun += 88;
				if (r_ptr->flags3 & RF3_NO_STUN) resist_stun += 66;
				if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
				if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
				if (r_ptr->flags2 & RF2_POWERFUL) resist_stun += 33;
				if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING))
					resist_stun += 66;
				*/
				/*
				if (p_ptr->special_defense & KAMAE_BYAKKO)
					max_times = (p_ptr->lev < 3 ? 1 : p_ptr->lev / 3);
				else if (p_ptr->special_defense & KAMAE_SUZAKU)
					max_times = 1;
				else if (p_ptr->special_defense & KAMAE_GENBU)
					max_times = 1;
				else
					max_times = (p_ptr->lev < 7 ? 1 : p_ptr->lev / 7);
				*/
				max_times = ref_skill_exp(SKILL_MARTIALARTS) / 2000 + 1;

				/* Attempt 'times' */
				/*:::素手攻撃エフェクトから数回選定し一番高レベルなのを決定*/
				for (times = 0; times < max_times; times++)
				{
					if(!martial_arts_method)
					{
						msg_print(_("ERROR:素手攻撃分類が決まっていない",
                                    "ERROR: Failed to pick barehanded attack"));
						return;
					}


					do
					{
						int mode_pick[50];
						int mode_cnt=0;
						int i;
						for(i = 0;(ma_blows_new[i].method != 0 && mode_cnt < 50);i++)
						{
							if(ma_blows_new[i].method == martial_arts_method)
							{
								mode_pick[mode_cnt++] = i;
							}
						}
						if(!mode_cnt)
						{
							msg_format(_("ERROR:指定された分類(%d)の素手攻撃が一つも登録されていない",
                                        "ERROR: No barehanded attacks found for chosen kind (%d)"),martial_arts_method);
							return;
						}

						if(p_ptr->pclass == CLASS_SUMIREKO && mode == HISSATSU_NO_AURA)
						{
							if(mode_cnt < 3)
							{
								msg_print(_("ERROR:菫子遠隔攻撃候補が足りない",
                                            "ERROR: Not enough ranked attacks for Sumireko"));
								return;
							}
							ma_ptr = &ma_blows_new[ mode_pick[2 + randint0(mode_cnt-2)]];

						}
						else
						{
							//ma_ptr = &ma_blows[randint0(MAX_MA)];
							ma_ptr = &ma_blows_new[ mode_pick[randint0(mode_cnt)]];
							//if ((p_ptr->pclass == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
							//else
						}
							min_level = ma_ptr->min_level;
					}
					while ((min_level > p_ptr->lev) ||
					       (randint1(p_ptr->lev) < ma_ptr->chance));



					/* keep the highest level attack available we found */
					/*::: Hack - old_ptrとの比較が>だと、素手攻撃候補がレベル1のみのとき殴る以外の分類でも殴るが選択されるので>=にした*/
					//if ((ma_ptr->min_level > old_ptr->min_level) &&
					if ((ma_ptr->min_level >= old_ptr->min_level) &&
					    !p_ptr->stun && !p_ptr->confused)
					{
						old_ptr = ma_ptr;

						if (p_ptr->wizard && cheat_xtra)
						{
#ifdef JP
							msg_print("攻撃を再選択しました。");
#else
							msg_print("Attack re-selected.");
#endif
						}
					}
					else
					{
						ma_ptr = old_ptr;
					}
				}
				/*:::格闘攻撃msg*/
				msg_format(ma_ptr->desc, m_name);


				//if (p_ptr->pclass == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
				//else
					min_level = ma_ptr->min_level;
					weight = ma_ptr->weight;



				/*:::格闘攻撃のダメージ処理*/
				k = damroll(ma_ptr->dd + p_ptr->to_dd[hand], ma_ptr->ds + p_ptr->to_ds[hand]);
				///mod141221 忍者用にbackstabとstab_fleeingを追加
				if (backstab)
				{
					k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
						k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}
				/*:::特殊処理　地獄爪殺法*/
				if( martial_arts_method == MELEE_MODE_EX_CLAW && mode == HISSATSU_EXCLAW && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS)))
					if(mult < 200) mult = 200;
				/*:::特殊処理　飯綱権現降臨*/
				if(p_ptr->kamioroshi & KAMIOROSHI_IZUNA && (r_ptr->flags3 & (RF3_ANG_CHAOS)) )
					if(mult < 150) mult = 150;

				/*:::特殊処理　ヤマメ格闘*/
				if( p_ptr->pclass == CLASS_YAMAME && p_ptr->lev > randint0(50) && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS)))
					if(mult < 200) mult = 200;

				//v1.1.89 百々世
				if (p_ptr->pclass == CLASS_MOMOYO)
				{
					if(p_ptr->lev > 34 && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS)) && mult < 200)
						mult = 200;

					if (p_ptr->tim_general[1] && (r_ptr->d_char == 'J') && mult < 300)//スネークイーター
						mult = 300;

					if (p_ptr->tim_general[2] && (r_ptr->flags3 & RF3_DRAGON) && mult < 300)//ドラゴンイーター
						mult = 300;
				}

				switch(ma_ptr->effect)
				{
				case MELEE_STUN:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 3 + randint0(3);
					break;
				case MELEE_STUN2:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);
					break;
				case MELEE_STUN3:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(10);
					break;
				//霊夢格闘など
				case MELEE_SLAY_EVIL:
					if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
					{
						if(mult < 250) mult = 250;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 5 + randint0(5);

					}
					break;
				///mod140217 衣玖の羽衣格闘など　電撃耐性のない敵にスレイ+朦朧
				case MELEE_ELEC:
					if(!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ELEC)))
					{
						if(mult<200) mult=200;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 10 + randint0(5);

					}
					break;
				///mod140309 華扇の包帯格闘　アンデッドスレイ+朦朧
				///mod141231 動物スレイも追加
				case MELEE_KASEN:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);
					if(r_ptr->flags3 & (RF3_UNDEAD) || r_ptr->flags3 & (RF3_ANIMAL))
					{
						if(mult < 300) mult = 300;
						effect_turns = 10 + randint0(10);
					}
					break;
				/*:::Hack 格闘吸血処理*/
				case MELEE_VAMP:
					if (monster_living(r_ptr))
					{
						can_drain = TRUE;
						can_feed = TRUE;
					}
					break;
				//溶かす　酸属性+吸収
				case MELEE_MELT:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_ACID))
					{
						if(mult<200) mult=200;
					}
					if (monster_living(r_ptr))
					{
						can_drain = TRUE;
						can_feed = TRUE;
					}
					break;
				//強力な火炎攻撃
				case MELEE_ATAGO:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_FIRE))
					{
						if(r_ptr->flagsr & RFR_HURT_FIRE && mult<500) mult = 500;
						else if(mult<300) mult=300;

						//v1.1.91 石油地形火炎ダメージ
						if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

					}
					break;
					//火炎攻撃
				case MELEE_FIRE:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_FIRE))
					{
						if(r_ptr->flagsr & RFR_HURT_FIRE && mult<300) mult = 300;
						else if(mult<200) mult=200;

						//v1.1.91 石油地形火炎ダメージ
						if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

					}
					break;
					//冷気、氷攻撃
				case MELEE_COLD:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_COLD))
					{
						if(r_ptr->flagsr & RFR_HURT_COLD && mult<300) mult = 300;
						else if(mult<200) mult=200;
					}
					break;

				case MELEE_SLOW:
					attack_effect_type = ATTACK_EFFECT_SLOW;
					effect_turns = 5 + randint0(5);

					break;

				case MELEE_POIS: //毒耐性のない敵に大ダメージ
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_POIS))
					{
						if(monster_living(r_ptr)) mult = MAX(300,mult);
						else mult = MAX(200,mult);
					}
					break;
				case MELEE_SINK: //村紗水没攻撃
						if(monster_living(r_ptr) && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)))
							sinker = TRUE;
						//breakしない
				case MELEE_WATER:
					if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE))
					{
						mult = 50;
					}
					else
					{
						if (r_ptr->flagsr & RFR_HURT_WATER) mult = 200;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 10 + randint0(10);
					}
					break;
				case MELEE_CONFUSE: //混乱付与
					if(r_ptr->flags3 & RF3_NO_CONF)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
					}
					else if(randint0(100) > r_ptr->level)
					{
						msg_format(_("%^sは混乱したようだ。", "%^s is confused."), m_name);
						(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
					}
					break;

				//v1.1.20　お燐の猫車攻撃は重量が猫車の中身の重量になる
				case MELEE_NEKOGURUMA:
					{
						int inven2_num = calc_inven2_num();
						int nekoguruma_weight = 0;
						int i;

						for (i = 0; i < inven2_num; i++)
						{
							object_type *tmp_o_ptr = &inven_add[i];
							if(!tmp_o_ptr->k_idx) continue;

							nekoguruma_weight += tmp_o_ptr->weight * tmp_o_ptr->number;
						}

						// -hack- 重量値変更　あとでplevが掛けられるぶん減らしとく
						weight = nekoguruma_weight / p_ptr->lev;
						if(weight < 1) weight = 1;
						if(weight > 20) weight = 20;
						if(cheat_xtra) msg_format("weight:%d",weight);
						if(nekoguruma_weight > 200)
						{
							attack_effect_type = ATTACK_EFFECT_STUN;
							effect_turns = nekoguruma_weight / 100 + randint1(nekoguruma_weight / 100);

						}
					}
					break;

					//v1.1.42 ドレミー睡眠攻撃 夢の世界で無条件スレイ、夢の世界以外では睡眠判定フラグ追加
				case MELEE_SLEEP:
					if (IN_DREAM_WORLD)
					{
						mult = 300;
					}
					else
					{
						counting_sheep++;
					}
					break;

				case MELEE_DEC_ATK:
					attack_effect_type = ATTACK_EFFECT_DEC_ATK;
					break;
				case MELEE_DEC_DEF:
					attack_effect_type = ATTACK_EFFECT_DEC_DEF;
					break;
				case MELEE_DEC_MAG:
					attack_effect_type = ATTACK_EFFECT_DEC_MAG;
					break;
				case MELEE_DEC_ALL:
					attack_effect_type = ATTACK_EFFECT_DEC_ALL;
					break;
				case MELEE_DELAY:
					attack_effect_type = ATTACK_EFFECT_DELAY;
					break;

				default:

					//v1.1.47 ミスティア「ヒューマンケージ」
					if (p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE) && (r_ptr->flags3 & RF3_HUMAN))
					{
						if(mult < 200) mult = 200;
					}

					break;

				}
				k = k * mult / 100;//格闘擬似スレイ

				/*:::キスメ特殊処理 桶のACなどをダメージに加算*/
				if(p_ptr->pclass == CLASS_KISUME) k += calc_kisume_bonus();


#if 0
				if (p_ptr->special_attack & ATTACK_SUIKEN) k *= 2;

				if (ma_ptr->effect == MA_KNEE)
				{
					if (r_ptr->flags1 & RF1_MALE)
					{
#ifdef JP
						msg_format("%sに金的膝蹴りをくらわした！", m_name);
#else
						msg_format("You hit %s in the groin with your knee!", m_name);
#endif

						sound(SOUND_PAIN);
						special_effect = MA_KNEE;
					}
					else
						msg_format(ma_ptr->desc, m_name);
				}

				else if (ma_ptr->effect == MA_SLOW)
				{
					if (!((r_ptr->flags1 & RF1_NEVER_MOVE) ||
					    my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
					{
#ifdef JP
						msg_format("%sの足首に関節蹴りをくらわした！", m_name);
#else
						msg_format("You kick %s in the ankle.", m_name);
#endif

						special_effect = MA_SLOW;
					}
					else msg_format(ma_ptr->desc, m_name);
				}
				else
				{
					if (ma_ptr->effect)
					{
						stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
					}

					msg_format(ma_ptr->desc, m_name);
				}
#endif

				//if (p_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
				/*:::練気術士に気が溜まってる場合重量判定増加*/
				if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
				{
					weight += (p_ptr->magic_num1[0]/30);
					if (weight > 20) weight = 20;
				}
				//v1.1.71 早鬼のキックにクリティカルボーナス
				if (p_ptr->pclass == CLASS_SAKI && (martial_arts_method == MELEE_MODE_KICK1 || martial_arts_method == MELEE_MODE_KICK2))
				{
					weight += 5 + p_ptr->lev / 10;
				}

				/*:::格闘攻撃のクリティカル処理*/

				k = critical_norm(p_ptr->lev * weight, min_level, k, p_ptr->to_h[0], 0, &critical_rank);


				//v1.1.46 女苑の特殊処理　モードによって金を奪ったり消費したりする
				if (p_ptr->pclass == CLASS_JYOON)
				{
					if (martial_arts_method == MELEE_MODE_JYOON_1)
					{
						int tmp, gain_money;

						tmp = MAX(1, r_ptr->level / 3);

						gain_money = k * tmp + randint1(k * tmp);
						//倒した時(正確にはダイス値だけで敵HPを上回ったとき)は取得金額5倍にしてみる
						if (k > m_ptr->hp) gain_money = gain_money * 3 + randint1(gain_money * 4);
						//配下からは金を奪えない
						if (is_pet(m_ptr)) gain_money = 0;
						//ドロップのないモンスターは金を持ってないことにしておく
						if (!(r_ptr->flags1 & (RF1_DROP_60| RF1_DROP_90 | RF1_DROP_1D2| RF1_DROP_2D2| RF1_DROP_3D2| RF1_DROP_4D2)))
							gain_money = 0;

						if (gain_money > 0)
						{
							msg_format(_("$%d奪った。", "You stole $%d."), gain_money);
							p_ptr->au += gain_money;
							p_ptr->redraw |= PR_GOLD;
						}


					}
					else if (martial_arts_method == MELEE_MODE_JYOON_2)
					{
						int tmp = (p_ptr->lev - 15);
						int waste_money = k * tmp + randint1(k * tmp);

						jyoon_record_money_waste(waste_money);

						if (waste_money > p_ptr->au)
						{

							p_ptr->au = 0;
						}
						else
							p_ptr->au -= waste_money;
						p_ptr->redraw |= PR_GOLD;
						//金がなくなったら特技コマンドによる散財モード中断
						if (!p_ptr->au)
						{
							msg_print(_("散財を止めた。", "You used up your money."));
							p_ptr->special_attack &= ~(ATTACK_WASTE_MONEY);
							p_ptr->redraw |= PR_STATUS;

						}


					}
				}


#if 0	//v1.1.94 格闘の状態異常付与処理は apply_special_effect()に持っていった
				if ((special_effect == MA_KNEE) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
#ifdef JP
					msg_format("%^sは苦痛にうめいている！", m_name);
#else
					msg_format("%^s moans in agony!", m_name);
#endif

					stun_effect = 7 + randint1(13);
					resist_stun /= 3;
				}

				else if ((special_effect == MA_SLOW) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (!(r_ptr->flags1 & RF1_UNIQUE) &&
					    (randint1(p_ptr->lev) > r_ptr->level) &&
					    m_ptr->mspeed > 60)
					{
#ifdef JP
						msg_format("%^sの速度が遅くなった。", m_name);
#else
						msg_format("%^s starts limping slower.", m_name);
#endif

						m_ptr->mspeed -= 10;
					}
				}
				if (stun_effect && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
					{
						if (set_monster_stunned(c_ptr->m_idx, stun_effect + MON_STUNNED(m_ptr)))
						{
#ifdef JP
							msg_format("%^sはフラフラになった。", m_name);
#else
							msg_format("%^s is stunned.", m_name);
#endif
						}
						else
						{
#ifdef JP
							msg_format("%^sはさらにフラフラになった。", m_name);
#else
							msg_format("%^s is more stunned.", m_name);
#endif
						}
					}
				}
#endif

			//格闘攻撃のダイス・スレイ・クリティカルなど終了
			}

			/* Handle normal weapon */
			/*:::通常武器による攻撃*/
			else if (o_ptr->k_idx)
			{
				int wgt = o_ptr->weight;

				//武器への追加ダイス処理
				/*:::武器ダイス+追加ダイス（今のところ乗馬のランスのみ？）*/
				///mod150510 映姫武器の特殊処理
				if(o_ptr->name1 == ART_EIKI)
					k = damroll(r_ptr->level / 10 + o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);
				else
					k = damroll(o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);

				//tot_dam_aux()で理力が効いたかどうか確認するためのフラグ
				gflag_force_effect = FALSE;

				/*:::ダイスにスレイなど適用*/
				k = tot_dam_aux(o_ptr, k, m_ptr, mode, FALSE);

				if (gflag_force_effect && critical_rank < 1) critical_rank = 1;
				gflag_force_effect = FALSE;

				if (backstab)
				{
					k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
						k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}

				//if ((p_ptr->impact[hand] && ((k > 50) || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				if ((p_ptr->impact[hand] && (randint1(1000)<wgt || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				{
					do_quake = TRUE;
					if (critical_rank < 2) critical_rank = 2;
				}
				//キスメ飛び上がりの重量加算
				if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent)
					wgt += p_ptr->concent * (wgt + 100);

				/*:::クリティカル処理*/
			//	if ((!(o_ptr->tval == TV_SWORD) || !(o_ptr->sval == SV_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
				if ((!(o_ptr->tval == TV_KNIFE) || !(o_ptr->sval == SV_WEAPON_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
					k = critical_norm(wgt * 3 / 2 + p_ptr->lev * 2, o_ptr->to_h * 2, k, p_ptr->to_h[hand], mode,&critical_rank);

				drain_result = k;

				//v1.1.46 切れ味判定を上の方から持ってきて内容も整頓した。斬鉄無効処理と呪術切れ味処理は勝手版では使わないので削除
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					vorpal_chance = 2;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr))
						vorpal_chance = 2;
					else if (p_ptr->special_attack & ATTACK_UNSUMMON)
						vorpal_chance = 3;
					else
						vorpal_chance = 4;
				}
				else
				{
					if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr))
						vorpal_chance = 4;
					else if (p_ptr->special_attack & ATTACK_UNSUMMON)
						vorpal_chance = 4;
					else
						vorpal_chance = 0;
				}

				if(vorpal_chance && (randint1(vorpal_chance * 3 / 2) == 1))
					vorpal_cut = TRUE;
				else
					vorpal_cut = FALSE;

				if (weapon_invalid) vorpal_cut = FALSE;

				/*:::ヴォーパルヒット処理*/
				if (vorpal_cut)
				{
					int mult = 2;

					///del msg チェーンソー
#if 0
					///sydel item チェーンソーのメッセージ処理　消しとこう　変わりにストブリのメッセージを外に入れようか
					if ((o_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
					{
						char chainsword_noise[1024];
#ifdef JP
						if (!get_rnd_line("chainswd_j.txt", 0, chainsword_noise))
#else
						if (!get_rnd_line("chainswd.txt", 0, chainsword_noise))
#endif
						{
							msg_print(chainsword_noise);
						}
					}
#endif
					if (o_ptr->name1 == ART_VORPAL_BLADE)
					{
#ifdef JP
						msg_print("目にも止まらぬヴォーパルブレード、手錬の早業！");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif
					}
					else
					{
#ifdef JP
						msg_format("刃が%sを鋭く切り裂いた！", m_name);
#else
						msg_format("Your weapon cuts deep into %s!", m_name);
#endif
					}

					/* Try to increase the damage */
					while (one_in_(vorpal_chance))
					{
						mult++;
					}

					k *= mult;

					/* Ouch! */
					/*:::切れ味メッセージ　変更しよう*/
					if (((r_ptr->flagsr & RFR_RES_ALL) ? k/100 : k) > m_ptr->hp)
					{
#ifdef JP
				//		msg_format("%sを真っ二つにした！", m_name);
#else
				//		msg_format("You cut %s in half!", m_name);
#endif
					}
					else
					{
						switch (mult)
						{
						case 2: //msg_format("%sを斬った！", m_name); break;
						case 3: msg_format(_("%sをぶった斬った！", "%s received a deep cut!"), m_name); break;
						case 4: //msg_format("%sをメッタ斬りにした！", m_name); break;
						case 5: msg_format(_("%sを滅多斬りにした！", "%s is severely cut up!"), m_name); break;
						case 6: //msg_format("%sを刺身にした！", m_name); break;
						case 7: default:msg_format(_("%sを斬って斬って斬りまくった！", "%s is cut up all over!"), m_name); break;
						//default: msg_format("%sを細切れにした！", m_name); break;

						}
					}
					drain_result = drain_result * 3 / 2;

					//v1.1.94
					if (critical_rank < mult-1) critical_rank = mult-1;
					if (critical_rank > 5) critical_rank = 5;

				}

				k += o_ptr->to_d;
				drain_result += o_ptr->to_d;
			/*:::武器攻撃処理ここまで。ここから格闘/武器共通*/
			}

			//v1.1.94
			if ((p_ptr->special_attack & ATTACK_CHAOS) && !(r_ptr->flagsr & RFR_RES_CHAO))
			{
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 10 + randint0(10);
			}


			//パルスィダイスブースト
			if(p_ptr->pclass == CLASS_PARSEE && r_ptr->level > p_ptr->lev)
			{
				int k_old = k;

				if(r_ptr->flags1 & RF1_FEMALE) k *= 3;
				else k = k * 3 / 2;

				if(cheat_xtra) msg_format(_("パルスィのダイスブースト:%d→%d", "Parsee dice boost: %d->%d"),k_old, k);

				if (one_in_(2)) critical_rank++;
				if (critical_rank > 5) critical_rank = 5;

			}

			/* Apply the player damage bonuses */
			k += p_ptr->to_d[hand];
			drain_result += p_ptr->to_d[hand];

			///class 剣術家の必殺剣処理
			if ((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 3; ///mod140914 2→3
			if ((mode == HISSATSU_SEKIRYUKA) && !monster_living(r_ptr)) k = 0;
			if ((mode == HISSATSU_SEKIRYUKA) && !p_ptr->cut) k /= 2;

			/* No negative damage */
			if (k < 0) k = 0;

			if ((mode == HISSATSU_ZANMA) && monster_living(r_ptr))
			{
				k = 0;
			}

			///item 斬鉄剣
			if (zantetsu_mukou)
			{
#ifdef JP
				msg_print("こんな軟らかいものは切れん！");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
				k = 0;
				critical_rank = 0;//paranoia
			}

			if (e_j_mukou)
			{
#ifdef JP
				msg_print("蜘蛛は苦手だ！");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
				k /= 2;
			}

			if (mode == HISSATSU_MINEUCHI )
			{
				int tmp = (10 + randint1(15) + p_ptr->lev / 5);

				if(mode == HISSATSU_MINEUCHI) k = 0;
				anger_monster(m_ptr);

				if (!(r_ptr->flags3 & (RF3_NO_STUN)))
				{
					/* Get stunned */
					if (MON_STUNNED(m_ptr))
					{
#ifdef JP
						msg_format("%sはひどくもうろうとした。", m_name);
#else
						msg_format("%s is more dazed.", m_name);
#endif

						tmp /= 2;
					}
					else
					{
#ifdef JP
						msg_format("%s はもうろうとした。", m_name);
#else
						msg_format("%s is dazed.", m_name);
#endif
					}

					/* Apply stun */
					(void)set_monster_stunned(c_ptr->m_idx, MON_STUNNED(m_ptr) + tmp);
				}
				else
				{
#ifdef JP
					msg_format("%s には効果がなかった。", m_name);
#else
					msg_format("%s is not effected.", m_name);
#endif
				}
			}

			/* Modify the damage */
			/*:::無傷球とはぐれメタルの処理*/
			///item 死の大鎌 class 狂戦士　無傷球無効処理
			///del131223 死の大鎌削除
			//k = mon_damage_mod(m_ptr, k, (bool)(((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE)) || ((p_ptr->pclass == CLASS_BERSERKER) && one_in_(2))));
			k = mon_damage_mod(m_ptr, k, (bool)((p_ptr->pseikaku == SEIKAKU_BERSERK) && one_in_(2)));

			///item 毒針
			///mod131223 tval
			if ((!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
			{
				if ((randint1(randint1(r_ptr->level / 7) + 5) == 1) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
#ifdef JP
					msg_format("%sの急所を突き刺した！", m_name);
#else
					msg_format("You hit %s on a critical spot!", m_name);
#endif
					set_deity_bias(DBIAS_REPUTATION, -3);

				}
				else k = 1;
			}
			//v1.1.94 無傷球やはぐれメタル相手のときこれらのダメージや特殊効果は発生しないようにする
			else if (k > 1)
			{
				/*:::忍者ヒット*/
				///class 忍者ヒット
				///mod140511 こいしも似たようなことする
				if ((p_ptr->pclass == CLASS_KOISHI && fuiuchi) ||(flag_katana_critical) ||
					(p_ptr->pclass == CLASS_NINJA) && (o_ptr->tval == TV_KNIFE || monk_attack) && ((p_ptr->cur_lite <= 0) || one_in_(7)))
				{
					int maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
					//v1.1.94 少し出やすく
					//if (one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
					if (one_in_(backstab ? 9 : (stab_fleeing || fuiuchi) ? 12 : 24))
                    {
						k *= 5;
                        drain_result *= 2;
#ifdef JP
                        if(p_ptr->pclass == CLASS_KOISHI)
                            msg_format("%sは全く無防備に一撃を受けた！", m_name);
                        else if (flag_katana_critical)
							msg_print("必殺の一太刀だ！");
						else if(o_ptr->k_idx)
                            msg_format("刃が%sに深々と突き刺さった！", m_name);
                        else
                            msg_format("鋭い一撃が%sに突き刺さった！", m_name);
#else
                        if(p_ptr->pclass == CLASS_KOISHI)
                            msg_format("You caught %s totally off guard!", m_name);
                        else if (flag_katana_critical)
							msg_print("That was a critical blow!");
						else if(o_ptr->k_idx)
                            msg_format("Your blade deeply pierces %s!", m_name);
                        else
                            msg_format("%s is pierced by a sharp blow!", m_name);
#endif
                    }
					//else if (((m_ptr->hp < maxhp / 2) && one_in_((p_ptr->num_blow[0] + p_ptr->num_blow[1] + 1) * 10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
					else if (((m_ptr->hp < maxhp / 2) && one_in_((p_ptr->num_blow[0] + p_ptr->num_blow[1] + 1) * 6)) || ((one_in_(66) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
                    {
						if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (m_ptr->hp >= maxhp / 2) && !one_in_(4))
						{
							k = MAX(k * 5, m_ptr->hp / 2);
							drain_result *= 2;
#ifdef JP
                            if(p_ptr->pclass == CLASS_KOISHI)
                                msg_format("%sは全く無防備に一撃を受けた！", m_name);
                            else if (flag_katana_critical)
								msg_print("必殺の一太刀だ！");
							else if(o_ptr->k_idx)
                                msg_format("刃が%sの急所を貫いた！", m_name);
                            else
                                msg_format("%sの急所へ強烈な一撃を加えた！", m_name);
#else
                            if(p_ptr->pclass == CLASS_KOISHI)
                                msg_format("You caught %s totally off guard!", m_name);
                            else if (flag_katana_critical)
                                msg_print("That was a critical blow!");
                            else if(o_ptr->k_idx)
                                msg_format("Your blade hits %s in a critical spot!", m_name);
                            else
                                msg_format("You powerfully hit %s in a critical spot!", m_name);
#endif
                        }
                        else
                        {
                            k = m_ptr->hp + 1;
#ifdef JP
                            if(p_ptr->pclass == CLASS_KOISHI)
                                msg_format("%sの意識は途切れた。", m_name);
                            else if (flag_katana_critical)
								msg_format("%sを一刀のもとに斬り伏せた！", m_name);
							else if(o_ptr->k_idx)
                                msg_format("刃が%sの急所を貫いた！", m_name);
                            else
                                msg_format("%sを昏倒させた。", m_name);
#else
                            if(p_ptr->pclass == CLASS_KOISHI)
                                msg_format("%s loses consciousness.", m_name);
                            else if (flag_katana_critical)
								msg_format("You strike %s down with your katana!", m_name);
							else if(o_ptr->k_idx)
                                msg_format("Your blade hits %s in a critical spot!", m_name);
                            else
                                msg_format("You have taken %s down.", m_name);
#endif
                        }
					}
				}
				//村紗特殊攻撃　ユニーク、水棲、水耐性のときこのフラグは立たない
                else if(sinker)
                {
					int resist = 100 + r_ptr->level * 6;
					if (r_ptr->flags2 & RF2_POWERFUL) resist = resist * 3 / 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) resist *= 2;
					if (r_ptr->flags7 & RF7_CAN_FLY) resist *= 2;

					if (randint1(resist) < p_ptr->lev)
					{
                            k = m_ptr->hp + 1;
                            msg_format(_("%sを溺れさせた。", "You drown %s."), m_name);

                    }
                }

                //v1.1.21 兵士(スニーキング判定成功)
                else if(sneakingkill)
                {
                    k = m_ptr->hp + 1;
                    msg_format(_("%sを無力化した。", "You incapacitate %s."), m_name);

                }
                //v1.1.94 攻撃後の朦朧付与などをここで行う
				else
				{
					if (skill_type == TV_HAMMER) attack_effect_type = ATTACK_EFFECT_STUN;
					if (skill_type == TV_AXE) attack_effect_type = ATTACK_EFFECT_DEC_DEF;
					if (skill_type == TV_POLEARM) attack_effect_type = ATTACK_EFFECT_DELAY;

					apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,TRUE);
				}
			}


			if( k > 9999) k = 9999;

			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
#ifdef JP
				msg_format("%d/%d のダメージを与えた。", k, m_ptr->hp);
#else
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif
			}

			if (k <= 0)
			{
				can_drain = FALSE;
				can_feed = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;
				///class バーサーカー　倒したとき攻撃回数が残ってたら消費エネルギーが減る？
				///sys アリスの連鎖攻撃とかこの処理流用で良いんではなかろうか
				/*
				if ((p_ptr->pclass == CLASS_BERSERKER) && energy_use)
				{
					if (p_ptr->migite && p_ptr->hidarite)
					{
						if (hand) energy_use = energy_use*3/5+energy_use*num*2/(p_ptr->num_blow[hand]*5);
						else energy_use = energy_use*num*3/(p_ptr->num_blow[hand]*5);
					}
					else
					{
						energy_use = energy_use*num/p_ptr->num_blow[hand];
					}
				}
				*/
				///item 斬鉄剣
				if ((o_ptr->name1 == ART_ZANTETSU) && is_lowlevel && !weapon_invalid)
#ifdef JP
					msg_print("またつまらぬものを斬ってしまった．．．");
#else
					msg_print("Sigh... Another trifling thing I've cut....");
#endif

				///mod140714 付喪神使いの妖器化処理　敵を倒した時は妖器化しやすい
				//v1.1.29 雷鼓の追加と☆の妖器化が抜けてたので追加
				if((p_ptr->pclass == CLASS_TSUKUMO_MASTER || p_ptr->pclass == CLASS_RAIKO) &&
					!object_is_fixed_artifact(o_ptr) && o_ptr->xtra1 < 100)
				{
					if(randint0(o_ptr->xtra1) < (r_ptr->level-o_ptr->xtra1 / 4) || (r_ptr->flags1 & RF1_UNIQUE))
					{
						o_ptr->xtra1++;
						if((r_ptr->flags1 & RF1_UNIQUE) && (o_ptr->xtra1 % 50)) o_ptr->xtra1++; //ユニークを倒した時は2ポイントボーナス
						if(o_ptr->xtra1 == 50 || o_ptr->xtra1 == 100) make_evilweapon(o_ptr,r_ptr);
					}
				}

				/*:::モンスターを倒したときも吸血処理が行われるようにするため、このbreakを吸血の後ろに移動*/
				//break;
			}

			/* Are we draining it?  A little note: If the monster is
			dead, the drain does not work... */
			/*:::吸血処理 モンスターを倒した時にも効くようにした*/
			if (can_drain && (drain_result > 0))
			{
				///item 村正の特殊強化処理
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int i, flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
#ifdef JP
							msg_print("妖刀は血を吸って強くなった！");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 敵に止めを刺した時はHPが大幅に回復することにしてみる
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if(p_ptr->pclass == CLASS_YOSHIKA) drain_heal *= 2;
						/* Hex */
						if (hex_spelling(HEX_VAMP_BLADE)) drain_heal *= 2;

						if (cheat_xtra)
						{
#ifdef JP
							msg_format("Draining left: %d", drain_left);
#else
							msg_format("Draining left: %d", drain_left);
#endif

						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
#ifdef JP
								msg_format("%sから生命力を吸い取った！", m_name);
#else
								msg_format("Your weapon drains life from %s!", m_name);
#endif
								if(one_in_(100)) set_deity_bias(DBIAS_COSMOS, -1);

								drain_msg = FALSE;
							}

							/*:::突然変異が多いと吸血の回復量が減るらしい*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
							/* We get to keep some of it! */
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			if (can_feed && (drain_result > 0))
			{
				if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX-1,p_ptr->food + drain_result*5));
			}

			can_drain = FALSE;
			can_feed = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			///mod140412 この二行は吸血処理の前だったが後ろに持ってきた
			if (k > 0) anger_monster(m_ptr);

			//オーラダメージ　針妙丸特殊攻撃はオーラを受けない
			//一輪格闘時、ソウルスカルプチュア、ダブルスコアもオーラを受けない
			//v1.1.14 祇園様の剣も
			if(mode != HISSATSU_SHINMYOU &&
				mode != HISSATSU_NO_AURA &&
				mode != HISSATSU_GION &&
				mode != HISSATSU_SOULSCULPTURE &&
				mode != HISSATSU_DOUBLESCORE &&
				!(p_ptr->pclass == CLASS_ICHIRIN && p_ptr->do_martialarts ))
				touch_zap_player(m_ptr);


			/* Confusion attack */
			/*:::混乱の手処理*/
			///sys ぬえの混乱の手処理とかこの辺に乗せればいいか
			if ((p_ptr->special_attack & ATTACK_CONFUSE)
				|| (p_ptr->special_attack & ATTACK_CHAOS) && !(r_ptr->flagsr & RFR_RES_CHAO) && p_ptr->lev > randint1(r_ptr->level * 4)
				|| (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(HEX_CONFUSION))
			{
				bool flg= FALSE;
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					flg = TRUE;
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("手の輝きがなくなった。");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;

#ifdef JP
					if(flg) msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					if(flg) msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは混乱したようだ。", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}
//v1.1.20 歪曲魔法剣の効果変更
#if 0
			///mod140725 歪曲魔法剣の抹殺処理　通常の抹殺と違う判定にするためplayer_castをFALSEで渡す。
			if(p_ptr->special_attack & ATTACK_UNSUMMON)
			{
				if((r_ptr->flagsr & ( RFR_RES_TIME)) && !one_in_(6)) ;
				else if((r_ptr->flagsr & (RFR_RES_TELE)) && !one_in_(3)) ;
				else if(randint1(r_ptr->level * 3 + 100) > p_ptr->lev);
				else if(genocide_aux(c_ptr->m_idx,0, FALSE, 0,"(このメッセージは出ないはず)"))
				{
						msg_format("%^sはこの階から消えた。", m_name);
						num = num_blow + 1; /* Can't hit it anymore! */
						*mdeath = TRUE;
				}

			}

			else
#endif
			if (chaos_effect == 4)
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if (r_ptr->flags1 & RF1_UNIQUE)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected!", m_name);
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^sは抵抗力を持っている！", m_name);
#else
						msg_format("%^s resists!", m_name);
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
#ifdef JP
					msg_format("%^sは消えた！", m_name);
#else
					msg_format("%^s disappears!", m_name);
#endif

					teleport_away(c_ptr->m_idx, 50, TELEPORT_PASSIVE);
					num = num_blow + 1; /* Can't hit it anymore! */
					*mdeath = TRUE;
				}
			}

			else if ((chaos_effect == 5) && (randint1(90) > r_ptr->level))
			{
				if (!(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) &&
				    !(r_ptr->flagsr & RFR_EFF_RES_CHAO_MASK))
				{
					if (polymorph_monster(y, x))
					{
#ifdef JP
						msg_format("%^sは変化した！", m_name);
#else
						msg_format("%^s changes!", m_name);
#endif

						*fear = FALSE;
						weak = FALSE;
					}
					else
					{
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected.", m_name);
#endif
					}

					/* Hack -- Get new monster */
					m_ptr = &m_list[c_ptr->m_idx];

					/* Oops, we need a different name... */
					monster_desc(m_name, m_ptr, 0);

					/* Hack -- Get new race */
					r_ptr = &r_info[m_ptr->r_idx];
				}
			}
			///sys item ゴールデンハンマー　トレジャーハンターとかで同じことができてもいいかも
			/*:::ゴールデンハンマーで殴ったときアイテムを奪う処理*/
			else if (o_ptr->name1 == ART_G_HAMMER && !weapon_invalid)
			{
				monster_type *tmp_m_ptr = &m_list[c_ptr->m_idx];

				if (tmp_m_ptr->hold_o_idx && (tmp_m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || tmp_m_ptr->r_idx > MON_TSUKUMO_WEAPON5))
				{
					object_type *q_ptr = &o_list[tmp_m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					tmp_m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
#ifdef JP
					msg_format("%sを奪った。", o_name);
#else
					msg_format("You snatched %s.", o_name);
#endif
					inven_carry(q_ptr);
				}
			}

			if((mode == HISSATSU_EIGOU) && !dokubari) num--;
		}

		/* Player misses */
		else
		{
			backstab = FALSE; /* Clumsy! */
			fuiuchi = FALSE; /* Clumsy! */
			///del131223 死の大鎌削除
#if 0
			/*:::死の大鎌*/
			///sysdel item race 死の大鎌が失敗したときの処理
			if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
			{
				u32b flgs[TR_FLAG_SIZE];

				/* Sound */
				sound(SOUND_HIT);

				/* Message */
#ifdef JP
				msg_format("ミス！ %sにかわされた。", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
				/* Message */
#ifdef JP
				msg_print("振り回した大鎌が自分自身に返ってきた！");
#else
				msg_print("Your scythe returns to you!");
#endif

				/* Extract the flags */
				object_flags(o_ptr, flgs);

				k = damroll(o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);
				{
					int mult;
					switch (p_ptr->mimic_form)
					{
					case MIMIC_NONE:
						switch (p_ptr->prace)
						{
						//	case RACE_YEEK:
						//	case RACE_KLACKON:
							case RACE_HUMAN:
							//case RACE_AMBERITE:
							case RACE_DUNADAN:
							//case RACE_BARBARIAN:
							case RACE_BEASTMAN:
								mult = 25;break;
						//	case RACE_HALF_ORC:
						//	case RACE_HALF_TROLL:
						//	case RACE_HALF_OGRE:
							//case RACE_HALF_GIANT:
							//case RACE_HALF_TITAN:
							//case RACE_CYCLOPS:
						//	case RACE_IMP:
							case RACE_SKELETON:
							case RACE_ZOMBIE:
							case RACE_VAMPIRE:
							case RACE_SPECTRE:
							//case RACE_DEMON:
						//	case RACE_DRACONIAN:
								mult = 30;break;
							default:
								mult = 10;break;
						}
						break;
					case MIMIC_DEMON:
					case MIMIC_DEMON_LORD:
					case MIMIC_VAMPIRE:
						mult = 30;break;
					default:
						mult = 10;break;
					}

					if (p_ptr->align < 0 && mult < 20)
						mult = 20;
					if (!(p_ptr->resist_acid || IS_OPPOSE_ACID() || p_ptr->immune_acid) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_elec || IS_OPPOSE_ELEC() || p_ptr->immune_elec) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_fire || IS_OPPOSE_FIRE() || p_ptr->immune_fire) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_cold || IS_OPPOSE_COLD() || p_ptr->immune_cold) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()) && (mult < 25))
						mult = 25;

					if ((p_ptr->pclass != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (p_ptr->msp / 30)))
					{
						p_ptr->csp -= (1+(p_ptr->msp / 30));
						p_ptr->redraw |= (PR_MANA);
						mult = mult * 3 / 2 + 20;
					}
					k *= mult;
					k /= 10;
				}

				k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[hand], mode);
				if (one_in_(6))
				{
					int mult = 2;
#ifdef JP
					msg_format("グッサリ切り裂かれた！");
#else
					msg_format("Your weapon cuts deep into yourself!");
#endif
					/* Try to increase the damage */
					while (one_in_(4))
					{
						mult++;
					}

					k *= mult;
				}
				k += (p_ptr->to_d[hand] + o_ptr->to_d);

				if (k < 0) k = 0;

#ifdef JP
				take_hit(DAMAGE_FORCE, k, "死の大鎌", -1);
#else
				take_hit(DAMAGE_FORCE, k, "Death scythe", -1);
#endif

				redraw_stuff();
			}
			else
#endif
			{
				/* Sound */
				sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("ミス！ %sにかわされた。", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
			}

			if(mode == HISSATSU_EIGOU) break;
		}
		backstab = FALSE;
		fuiuchi = FALSE;
		flag_katana_critical = FALSE;

	}	//←攻撃回数ループ終了


	//ドレミー格闘攻撃で眠らせる処理
	if (counting_sheep && !(*mdeath))
	{
		int sleep_power = (p_ptr->lev+10) * counting_sheep;

		if (r_ptr->flagsr & RFR_RES_ALL) sleep_power = 0;
		if (r_ptr->flags1 & RF1_QUESTOR) sleep_power = 0;
		if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) sleep_power /= 2;
		if ((r_ptr->flags3 & RF3_NO_SLEEP)) sleep_power /= 3;

		if (randint1(sleep_power) > randint1(MAX(r_ptr->level,10)))
		{
			msg_format(_("%sは眠った。", "%s falls asleep."), m_name);
			set_monster_csleep(c_ptr->m_idx, randint1(3));
		}

	}

	//ガンカタをしたとき残弾消費
	if(flag_gun_kata)
	{
		object_type *tmp_o_ptr;
		int i;
		int timeout_base,timeout_max;
		for(i=INVEN_RARM;i<=INVEN_LARM;i++)
		{
			tmp_o_ptr = &inventory[i];

			if(tmp_o_ptr->tval != TV_GUN)
				msg_print(_("ERROR:銃を装備していないのに残弾減少処理が呼ばれている",
                            "ERROR: Logic for reducing ammo called without a gun being equipped"));
			else
			{
				timeout_base = calc_gun_timeout(tmp_o_ptr) * 1000;
				timeout_max = timeout_base * (calc_gun_load_num(tmp_o_ptr) - 1);
				tmp_o_ptr->timeout += timeout_base;
				p_ptr->window |= PW_EQUIP;

				//ガンカタ残弾判定のため弾切れチェック
				if(o_ptr->timeout > timeout_max)
					p_ptr->update |= PU_BONUS;
			}

		}
	}

	if (weak && !(*mdeath))
	{
#ifdef JP
		msg_format("%sは弱くなったようだ。", m_name);
#else
		msg_format("%^s seems weakened.", m_name);
#endif
	}
	//if (drain_left != MAX_VAMPIRIC_DRAIN)
	//{
		///sysdel virtue
	//	if (one_in_(4))
	//	{
	//		chg_virtue(V_UNLIFE, 1);
	//	}
	//}
	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		earthquake(py, px, 10);
		if (!cave[y][x].m_idx) *mdeath = TRUE;
	}
	///mod140828 フランの攻撃の武器劣化処理
	if(p_ptr->pclass == CLASS_FLAN && o_ptr->k_idx)
	{
		int blast;
		object_flags(o_ptr, flgs);
		if(object_is_artifact(o_ptr) && have_flag(flgs,TR_RES_DISEN)) blast = 0;
		else if(object_is_artifact(o_ptr) || have_flag(flgs,TR_RES_DISEN)) blast = 1;
		else blast = 13;

		if(randint0(169) < blast)
		{
			msg_print(_("破壊の力に武器が耐えられない！", "Your weapon can't withstand your destructive power!"));
			apply_disenchant(0,INVEN_RARM + hand);

		}

	}
	//ネムノ「刃こぼれするまで切り刻め」による武器劣化
	else if (mode == HISSATSU_HAKOBORE)
	{
		apply_disenchant(0, INVEN_RARM + hand);
	}


}




/*:::変異部位や盾などによる追加格闘攻撃用ルーチン　*/
static void py_attack_aux2(int y, int x, bool *fear, bool *mdeath)
{
	int		num = 0, k, bonus_h, bonus_d , chance;

	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	object_type     *o_ptr;

	char            m_name[80];

//	bool            success_hit = FALSE;
	bool            monk_attack = FALSE;
	//bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	bool            can_feed = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	//u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */
	//bool            is_human = (r_ptr->d_char == 'p');
	//bool            is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));
	bool flag_gain_exp = FALSE;

	int skill_type;

	//int				martial_arts_method = 0;
	int i;

	int method[10];

	num_blow=0;
	for(i=0;i<10;i++) method[i]=0;


	/*:::ここに来る前のpy_attack_aux()で格闘したときTRUEになるはず */
	//if(!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && empty_hands(TRUE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM)) monk_attack = TRUE;
	if(p_ptr->do_martialarts) monk_attack = TRUE;

	/*:::まず攻撃可能な手段をmethod[]に格納*/

	/*:::盾熟練度に応じシールドバッシュ 熟練度20から出始めて50で100%*/
	//v1.1.94 二発まで出るようにしよう
	if(	(!check_invalidate_inventory(INVEN_RARM) &&inventory[INVEN_RARM].tval == TV_SHIELD || !check_invalidate_inventory(INVEN_LARM) &&inventory[INVEN_LARM].tval == TV_SHIELD))
	{
		if(ref_skill_exp(SKILL_SHIELD) > 3200 + randint0(4800))
			method[num_blow++] = MELEE_MODE_SHIELD;
		if (ref_skill_exp(SKILL_SHIELD) > 4800 + randint0(3200))
			method[num_blow++] = MELEE_MODE_SHIELD;

	}
	/*:::武器攻撃をした時か両手に荷物を持っているとき、体術熟練度に応じ追加で格闘 格闘熟練度が20以上か石頭変異所持時*/
	///mod141116 キスメには武器攻撃時の追加格闘がない
	///mod141221 忍者も同じ
	if(!monk_attack && p_ptr->pclass != CLASS_KISUME && p_ptr->pclass != CLASS_NINJA)
	{
		if(p_ptr->muta2 & MUT2_HARDHEAD && randint0(100) < p_ptr->lev + 25)	method[num_blow++] = MELEE_MODE_HEADBUTT;
		else if(randint0(4800) < ref_skill_exp(SKILL_MARTIALARTS)-3200)
		{
			method[num_blow++] = find_martial_arts_method(MELEE_FIND_NOHAND);
		}
		//棒を両手持ちしていて格闘技能が高いとき、さらに追加格闘が出る
		///mod150428 少し判定修正し、装備が重い時回数上がりにくくした
		//v1.1.94 格闘技能だけでなく棒技能と格闘技能の平均で判定することにした
		if(inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].tval == TV_STICK && p_ptr->ryoute)
		{
			int num_xtra = 0;
			int skill_exp;

			skill_exp = (ref_skill_exp(SKILL_MARTIALARTS) + ref_skill_exp(TV_STICK)) / 2;
			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST) num_xtra++;
			if(randint1(skill_exp) > 3000) num_xtra++;
			if(randint1(skill_exp) > 4500) num_xtra++;
			if(randint1(skill_exp) > 6000) num_xtra++;
			if(heavy_armor()) num_xtra /= 2;
			for(;num_xtra>0;num_xtra--) method[num_blow++] = find_martial_arts_method(MELEE_FIND_NOHAND);
		}

	}
	/*:::角を持っているときレベルに応じて角攻撃*/
	if(p_ptr->muta2 & (MUT2_HORNS) && randint0(100) < p_ptr->lev + 20)	method[num_blow++] = MELEE_MODE_HORNS;
	if(p_ptr->muta2 & (MUT2_BIGHORN) && randint0(100) < p_ptr->lev + 30)	method[num_blow++] = MELEE_MODE_BIGHORN;
	/*:::尻尾を持っているときレベルに応じて尻尾攻撃*/
	if(p_ptr->muta2 & (MUT2_BIGTAIL) && randint0(100) < p_ptr->lev + 40)	method[num_blow++] = MELEE_MODE_BIGTAIL;
	if(p_ptr->muta3 & (MUT3_FISH_TAIL) && randint0(100) < p_ptr->lev + 50)	method[num_blow++] = MELEE_MODE_FISH_TAIL;

	//銃装備時の特殊追加格闘
	if(!monk_attack && (inventory[INVEN_RARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_GUN)
		&& !check_invalidate_inventory(INVEN_RARM) && randint0(8000) < ref_skill_exp(TV_GUN))
	{
		method[num_blow++] = select_gun_melee_mode();
	}


	if(num_blow==0) return;


	//msg_format("BLOW:%d",num_blow);
	//for(i=0;i<10;i++)
	//	msg_format("%d:%d",i,method[i]);

	/* Disturb the monster */
	(void)set_monster_csleep(c_ptr->m_idx, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/*:::攻撃回数ごとに判定 whileだとnumを中で扱うのに不都合なのでforにした*/
	//while ((num++ < num_blow) && !p_ptr->is_dead)
	for (num=0; num < num_blow && !p_ptr->is_dead; num++ )
	{
		bool sinker = FALSE;
		int mult = 100;
		int mon_ac = r_ptr->ac;

		//v1.1.94
		int		attack_effect_type = ATTACK_EFFECT_NONE; //朦朧など敵にステータス異常を付与するとき
		int		critical_rank = 0; //クリティカルや切れ味や不意打ちなどが決まったとき増える
		int		effect_turns = 0; //朦朧などのステータス異常を与えるターン数

		//v1.1.94 モンスター防御力低下中はAC25%カット
		if (MON_DEC_DEF(m_ptr))
		{
			mon_ac = MONSTER_DECREASED_AC(mon_ac);
		}

		/*:::経験値を得る　今の所盾以外は全て格闘*/
		if (method[num] == MELEE_MODE_SHIELD)
		{
			skill_type = SKILL_SHIELD;
		}
		else if(!flag_gain_exp)
		{
			skill_type = SKILL_MARTIALARTS;
			flag_gain_exp = TRUE; //盾格闘は一度しか出ないがそれ以外は何度も出るので一度しか経験値を得られないようにしている
		}
		gain_skill_exp(skill_type, m_ptr);

		/*:::装備品などの修正を再計算する*/
		bonus_h = ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
		bonus_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		{
			int tmp =  p_ptr->lev / 2 * ref_skill_exp(skill_type) / 8000;
			if(heavy_armor() && method[num] != MELEE_MODE_SHIELD) tmp /= 2;
			bonus_h += tmp;
			bonus_d += tmp/2;
		}
		if (p_ptr->stun > 50 || p_ptr->drowning){	bonus_h -= 20;	bonus_d -= 20;	}
		else if (p_ptr->stun){	bonus_h -= 5;	bonus_d -= 5;	}
		if (IS_HERO()) bonus_h += 12;
		if (p_ptr->shero) bonus_d += 3+(p_ptr->lev/5);

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			o_ptr = &inventory[i];
			if (!o_ptr->k_idx) continue;
			if(check_invalidate_inventory(i)) continue;
			//武器の殺戮修正は無視
			if(object_is_melee_weapon(o_ptr)) continue;
			if(object_is_shooting_weapon(o_ptr)) continue;
			//盾を持っている手を除き指輪の修正は無視　(両手に盾と指輪を装備して盾攻撃するとき両手の指輪の修正が適用されるがまあどうでもいいだろう)
			if(i == INVEN_RIGHT && !(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_RARM].tval == TV_SHIELD)) continue;
			if(i == INVEN_LEFT && !(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_LARM].tval == TV_SHIELD)) continue;
			//装備品の修正を適用　1/2にしておく
			bonus_h += o_ptr->to_h/2;
			bonus_d += o_ptr->to_d/2;
		}
		chance = (p_ptr->skill_thn + (bonus_h * BTH_PLUS_ADJ));

		/* Test for hit */
		if ( test_hit_norm(chance, mon_ac, m_ptr->ml))
		{
			//int resist_stun = 0;
			//int stun_effect = 0;
			int special_effect = 0, times = 0, max_times;
			int min_level = 1;
			const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
			int weight = 8;

			k = 1;
			/*
			if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += 88;
			if (r_ptr->flags2 & RF2_GIGANTIC) resist_stun += 88;
			if (r_ptr->flags3 & RF3_NO_STUN) resist_stun += 66;
			if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
			if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
			if (r_ptr->flags2 & RF2_POWERFUL) resist_stun += 33;
			if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING)) resist_stun += 66;
			*/

			max_times = ref_skill_exp(skill_type) / 2000 + 1;

			/*:::指定攻撃エフェクトから数回選定し一番高レベルなのを決定*/
			for (times = 0; times < max_times; times++)
			{
				do
				{
					int mode_pick[20];
					int mode_cnt=0;
					for(i = 0;(ma_blows_new[i].method != 0 && mode_cnt < 20);i++)
					{
						if(ma_blows_new[i].method == method[num])
						{
							mode_pick[mode_cnt++] = i;
						}
					}
					if(!mode_cnt)
					{
						msg_format(_("ERROR:追加格闘処理にて指定された分類(%d)の素手攻撃が一つも登録されていない",
                                    "ERROR: No barehanded attacks found for given kind (%d) in additional martial art logic"),method[num]);
						return;
					}
					ma_ptr = &ma_blows_new[ mode_pick[randint0(mode_cnt)]];
					min_level = ma_ptr->min_level;
				}
				while ((min_level > p_ptr->lev) || (randint1(p_ptr->lev) < ma_ptr->chance));

				if ((ma_ptr->min_level >= old_ptr->min_level) && !p_ptr->stun && !p_ptr->confused)	old_ptr = ma_ptr;
				else ma_ptr = old_ptr;
			}
			min_level = ma_ptr->min_level;
			weight = ma_ptr->weight;
			//シールドバッシュのダメージや重量を色々細工　両手に盾を持ってる時の計算が少し不自然になるが面倒なので無視
			if(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_RARM].tval == TV_SHIELD)
			{
				bonus_d += (inventory[INVEN_RARM].ac + inventory[INVEN_RARM].to_a)/2;
				weight =  inventory[INVEN_RARM].weight / 10;
			}
			else if(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_LARM].tval == TV_SHIELD)
			{
				bonus_d += (inventory[INVEN_LARM].ac + inventory[INVEN_LARM].to_a)/2;
				weight =  inventory[INVEN_LARM].weight / 10;
			}

			k = damroll(ma_ptr->dd , ma_ptr->ds );

			switch(ma_ptr->effect)
			{
			case MELEE_STUN:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 3 + randint0(3);
				break;
			case MELEE_STUN2:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 5 + randint0(5);
				break;
			case MELEE_STUN3:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 10 + randint0(10);
				break;

			//霊夢格闘など
			case MELEE_SLAY_EVIL:
				if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
				{
					if(mult < 250) mult = 250;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);

				}
				break;

			case MELEE_ELEC:
				if (!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ELEC)))
				{
					if (mult<200) mult = 200;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(5);

				}
				break;
			case MELEE_KASEN:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 5 + randint0(5);
				if (r_ptr->flags3 & (RF3_UNDEAD) || r_ptr->flags3 & (RF3_ANIMAL))
				{
					if (mult < 300) mult = 300;
					effect_turns = 10 + randint0(10);
				}
				break;


			case MELEE_VAMP:
				if (monster_living(r_ptr))
				{
					can_drain = TRUE;
					can_feed = TRUE;
				}
				break;

			case MELEE_MELT:
				if(!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ACID)))
					if(mult<200) mult=200;
				if (monster_living(r_ptr))
				{
					can_drain = TRUE;
					can_feed = TRUE;
				}
				break;
			//強力な火炎攻撃
			case MELEE_ATAGO:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_FIRE))
				{
					if(r_ptr->flagsr & RFR_HURT_FIRE && mult<500) mult = 500;
					else if(mult<300) mult=300;

					//v1.1.91 石油地形火炎ダメージ
					if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

				}
				break;
			//火炎攻撃
			case MELEE_FIRE:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_FIRE))
				{
					if(r_ptr->flagsr & RFR_HURT_FIRE && mult<300) mult = 300;
					else if(mult<200) mult=200;

					//v1.1.91 石油地形火炎ダメージ
					if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

				}
				break;
			//冷気、氷攻撃
			case MELEE_COLD:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_COLD))
				{
					if(r_ptr->flagsr & RFR_HURT_COLD && mult<300) mult = 300;
					else if(mult<200) mult=200;
				}
				break;
			case MELEE_SLOW:
				attack_effect_type = ATTACK_EFFECT_SLOW;
				effect_turns = 5 + randint0(5);

				break;
			case MELEE_POIS: //毒耐性のない敵に大ダメージ
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_POIS))
				{
					if(monster_living(r_ptr)) mult = MAX(300,mult);
					else mult = MAX(200,mult);
				}
				break;
			case MELEE_SINK: //村紗水没攻撃
					if(monster_living(r_ptr) && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)))
						sinker = TRUE;
					//breakしない
			case MELEE_WATER:
				if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE))
				{
					mult = 50;
				}
				else
				{
					if (r_ptr->flagsr & RFR_HURT_WATER) mult = 200;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(10);
				}
				break;
			case MELEE_CONFUSE: //混乱付与
				if(r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
				}
				else if(randint0(100) > r_ptr->level)
				{
					msg_format(_("%^sは混乱したようだ。", "%^s is confused."), m_name);
					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
				break;

			//v1.1.20　お燐の猫車攻撃は重量が猫車の中身の重量になる
			case MELEE_NEKOGURUMA:
				{
					int inven2_num = calc_inven2_num();
					int nekoguruma_weight = 0;
				//	int i;

					for (i = 0; i < inven2_num; i++)
					{
						object_type *tmp_o_ptr = &inven_add[i];
						if(!tmp_o_ptr->k_idx) continue;

						nekoguruma_weight += tmp_o_ptr->weight * tmp_o_ptr->number;
					}

					// -hack- 重量値変更　あとでplevが掛けられるぶん減らしとく
					weight = nekoguruma_weight / p_ptr->lev;
					if(weight < 1) weight = 1;
					if(weight > 40) weight = 40;
					if(cheat_xtra) msg_format("weight:%d",weight);
					if(nekoguruma_weight > 200)
					{
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = nekoguruma_weight / 100 + randint1(nekoguruma_weight / 100);

					}
					break;
				}

			case MELEE_DEC_ATK:
				attack_effect_type = ATTACK_EFFECT_DEC_ATK;
				break;
			case MELEE_DEC_DEF:
				attack_effect_type = ATTACK_EFFECT_DEC_DEF;
				break;
			case MELEE_DEC_MAG:
				attack_effect_type = ATTACK_EFFECT_DEC_MAG;
				break;
			case MELEE_DEC_ALL:
				attack_effect_type = ATTACK_EFFECT_DEC_ALL;
				break;
			case MELEE_DELAY:
				attack_effect_type = ATTACK_EFFECT_DELAY;
				break;
			}
			k = k * mult / 100;//格闘擬似スレイ
			/*:::攻撃によるメッセージ*/
			msg_format(ma_ptr->desc, m_name);
			k = critical_norm(p_ptr->lev * weight, min_level, k, bonus_h, 0, &critical_rank);
			k += bonus_d;
			drain_result = k;

			/*
			if (stun_effect && k < m_ptr->hp)
			{
				if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
				{
					if (set_monster_stunned(c_ptr->m_idx, stun_effect + MON_STUNNED(m_ptr)))
					{
#ifdef JP
						msg_format("%^sはフラフラになった。", m_name);
#else
						msg_format("%^s is stunned.", m_name);
#endif
					}
					else
					{
#ifdef JP
						msg_format("%^sはさらにフラフラになった。", m_name);
#else
						msg_format("%^s is more stunned.", m_name);
#endif
					}
				}
			}
			*/

			/* No negative damage */
			if (k < 0) k = 0;

			/* Modify the damage */
			/*:::無傷球とはぐれメタルの処理*/
			k = mon_damage_mod(m_ptr, k, (bool)(( p_ptr->pseikaku == SEIKAKU_BERSERK) && one_in_(2)));


            if (k > 1)
            {
                //村紗特殊攻撃　ユニーク、水棲、水耐性のときこのフラグは立たない
                if(sinker)
                {
					int resist = 100 + r_ptr->level * 6;
					if (r_ptr->flags2 & RF2_POWERFUL) resist = resist * 3 / 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) resist *= 2;
					if (r_ptr->flags7 & RF7_CAN_FLY) resist *= 2;

					if (randint1(resist) < p_ptr->lev)
					{
                            k = m_ptr->hp + 1;
                            msg_format(_("%sを溺れさせた。", "You drown %s."), m_name);

                    }
                }
				//v1.1.94 攻撃後の朦朧付与などをここで行う
				else
				{
					apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,FALSE);
				}
            }

			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
#ifdef JP
				msg_format("%d/%d のダメージを与えた。", k, m_ptr->hp);
#else
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif
			}
			if (k <= 0)
			{
				can_drain = FALSE;
				can_feed = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;
			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;
				break;
			}

			/*:::吸血処理*/
			if (can_drain && (drain_result > 0))
			{

					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 敵に止めを刺した時はHPが大幅に回復することにしてみる
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if (cheat_xtra)
							msg_format("Draining left: %d", drain_left);

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
								msg_format(_("%sから生命力を吸い取った！", "You drain life force from %s!"), m_name);
								drain_msg = FALSE;
							}

							/*:::突然変異が多いと吸血の回復量が減るらしい*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
							/* We get to keep some of it! */
						}
					}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			if (can_feed && (drain_result > 0))
			{
				if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX-1,p_ptr->food + drain_result*5));
			}

			can_drain = FALSE;
			can_feed = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			if (k > 0) anger_monster(m_ptr);

			touch_zap_player(m_ptr);

			/* Confusion attack */
			/*:::混乱の手処理*/
			///sys ぬえの混乱の手処理とかこの辺に乗せればいいか
#if 0
			if ((p_ptr->special_attack & ATTACK_CONFUSE) || (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(HEX_CONFUSION))
			{
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("手の輝きがなくなった。");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;

#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは混乱したようだ。", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}
#endif

		}

		/* Player misses */
		else
		{
			sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("ミス！ %sにかわされた。", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
		}
	}


	if (weak && !(*mdeath))
	{
		msg_format(_("%sは弱くなったようだ。", "%s looks weaker."), m_name);
	}

}


/*:::アリス専用　人形による攻撃ルーチン*/
static void py_attack_aux_alice(int y, int x, bool *fear, bool *mdeath, int mode)
{
	int		num = 0, k, chance, bonus;

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	object_type     *o_ptr;

	char            m_name[80];

	bool            success_hit = FALSE;
	bool            vorpal_cut = FALSE;
	bool            fuiuchi = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; //切れ味
	bool            is_human = (r_ptr->d_char == 'p');

	//刀熟練度ボーナスの「必殺の一太刀」が出たときのフラグ
	bool			flag_katana_critical = FALSE;

	int total_blow_num = 0;
	int attack_doll_array[100];
	int i;
	int mon_ac = r_ptr->ac;


	if(p_ptr->pclass != CLASS_ALICE)
	{
	    msg_print(_("ERROR:クラスがアリス以外の時alice_attack_aux()が呼ばれた",
                    "ERROR: alice_attack_aux() called for non-Alice"));
        return;
    }

	//v1.1.94 モンスター防御力低下中はAC25%カット
	if (MON_DEC_DEF(m_ptr))
	{
		mon_ac = MONSTER_DECREASED_AC(mon_ac);
	}

	/*:::人形の攻撃回数を求め、順番をランダムに設定する*/
	for(i=0;i<100;i++) attack_doll_array[i]=0;
	for(i=0;i<INVEN_DOLL_NUM_MAX;i++)//全人形ループ
	{
		int j;
		int tmp_blow_num = calc_doll_blow_num(i);//人形一体の攻撃回数計算

		 //v1.1.94 槍による迎撃
		if (mode == HISSATSU_COUNTER_SPEAR)
		{
			if (inven_add[i].tval != TV_SPEAR) continue;
			tmp_blow_num = 1;
		}

		for(j=0;j<tmp_blow_num;j++)
		{

			if(!i)
			{
				attack_doll_array[total_blow_num] = i;//一番上の上海人形は普通に格納
			}
			else//それ以外の時確率で入れ替える
			{
				if(one_in_(total_blow_num+1)) attack_doll_array[total_blow_num] = i;
				else
				{
					int move = randint0(total_blow_num);
					attack_doll_array[total_blow_num] = attack_doll_array[move];
					attack_doll_array[move] = i;
				}
			}
			total_blow_num++;
			if(total_blow_num == 100)
			{msg_print(_("ERROR:alice_attack_aux()の攻撃回数が100以上",
                        "ERROR: blow count in alice_attack_aux() is over 100")); return; }
		}
	}
	if(!total_blow_num)
	{msg_print(_("ERROR:alice_attack_aux()で攻撃回数が0",
                "ERROR: blow count in alice_attack_aux() is 0")); return;}


//特定の特技のときは無効にする？
	if(p_ptr->tim_unite_darkness)
	{
		if(!(cave[py][px].info & CAVE_GLOW)  && m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
	}

	//刀の一撃目に必中とボーナスを乗せる
	if(check_katana_critical(m_ptr, mode))
	{
		flag_katana_critical = TRUE;
	}

	(void)set_monster_csleep(c_ptr->m_idx, 0);
	monster_desc(m_name, m_ptr, 0);

	/* Attack once for each legal blow */
	/*:::攻撃回数ごとに判定*/
	while ((num < total_blow_num) && !p_ptr->is_dead)
	{
		bool dokubari = FALSE;
		int doll_num = attack_doll_array[num];

		bool flag_katana_critical_done = FALSE;

		//v1.1.94
		int		skill_type = 0;	//該当するスキル ref_skill_exp()に使う
		int		attack_effect_type = ATTACK_EFFECT_NONE; //朦朧など敵にステータス異常を付与するとき
		int		critical_rank = 0; //クリティカルや切れ味や不意打ちなどが決まったとき増える
		int		effect_turns = 0; //朦朧などのステータス異常を与えるターン数

		o_ptr =  &inven_add[doll_num];
		if(!object_is_melee_weapon(o_ptr))
		{msg_format(_("ERROR:武器を持たない人形が呼ばれた(%d)",
                    "ERROR: Called doll without a weapon (%d)"),doll_num); return;}

		skill_type = o_ptr->tval;

		if (flag_katana_critical && o_ptr->tval == TV_KATANA) flag_katana_critical_done = TRUE;

		//得られる熟練度は1/10程度にしておく
		if(one_in_(10)) gain_skill_exp(skill_type,m_ptr);

		//calc_bonuses()で計算していない武器熟練度をここで反映
		//v1.1.94 剣の命中率に熟練度ボーナス反映
		if (o_ptr->tval == TV_SWORD)
		{
			bonus = p_ptr->to_h[0] + o_ptr->to_h + (ref_skill_exp(skill_type) - 2500) / 125;
		}
		else
		{
			bonus = p_ptr->to_h[0] + o_ptr->to_h + (ref_skill_exp(skill_type) - WEAPON_EXP_BEGINNER) / 200;
		}


		chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

		if (mode == HISSATSU_COUNTER_SPEAR) chance += 60;


		///sys item 毒針命中判定
		///mod131223 tval
		if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI))
		{
			dokubari = TRUE;
			success_hit = one_in_(3);
		}

		else if ((fuiuchi && !(r_ptr->flagsr & RFR_RES_ALL)))
		{
			success_hit = TRUE;
		}
		else if (flag_katana_critical_done)
		{
			success_hit = TRUE;
		}
		else
		{
			/*:::通常の命中処理*/
			success_hit = test_hit_norm(chance, mon_ac, m_ptr->ml);
			//オーラによる命中率減少
			if(r_ptr->flags2 & RF2_AURA_FIRE && one_in_(20)) success_hit = FALSE;
			if(r_ptr->flags2 & RF2_AURA_COLD && one_in_(20)) success_hit = FALSE;
			if(r_ptr->flags2 & RF2_AURA_ELEC && one_in_(20)) success_hit = FALSE;
		}

		/* Test for hit */
		if (success_hit)
		{
			int vorpal_chance;

			//v1.1.94
			if ( fuiuchi ) critical_rank = 3;

			/* Sound */
			sound(SOUND_HIT);

			if (fuiuchi)
				msg_format(_("%sは不意を突いて%sに強烈な一撃を喰らわせた！",
                            "%s catches %s unaware, delivering a powerful blow!"),alice_doll_name[doll_num], m_name);
			else if (flag_katana_critical_done)
				msg_format(_("%sは%sにからくり抜刀術を披露した！",
                            "%s hits %s with a mechanical katana strike!"), alice_doll_name[doll_num], m_name);
#ifdef JP
			else if(ex_attack_msg)
				msg_format("%sが%s%s",alice_doll_name[doll_num], m_name, msg_py_atk(o_ptr,mode));
			else
				msg_format("%s%s", m_name, msg_py_atk(o_ptr,mode));
#else
				msg_format("%s hits %s.", alice_doll_name[doll_num], m_name);
#endif

			k = 1;

			object_flags(o_ptr, flgs);
			//v1.1.46 アリスがEXTRAモードで砥石を使った時の処理を追加
			if (have_flag(flgs, TR_EX_VORPAL))
				vorpal_chance = 2;
			else if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr) && have_flag(flgs, TR_VORPAL))
				vorpal_chance = 2;
			else if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr) || have_flag(flgs, TR_VORPAL))
				vorpal_chance = 4;
			else
				vorpal_chance = 0;

			/* Vampiric drain */
			/*:::吸血処理*/
			//魔剣使用時を除き無効化する

			if ((p_ptr->special_attack & (ATTACK_DARK)) && p_ptr->lev > 44 || o_ptr->name1 == ART_MURAMASA)
			{
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;
			}

			if (vorpal_chance && randint1(vorpal_chance*3/2) == 1 )
				vorpal_cut = TRUE;
			else
				vorpal_cut = FALSE;

			/*:::武器ダイス(追加ダイス無視）*/
			k = damroll(o_ptr->dd , o_ptr->ds );
			/*:::ダイスにスレイなど適用*/
			k = tot_dam_aux(o_ptr, k, m_ptr, mode, FALSE);

			if (fuiuchi || flag_katana_critical_done)
			{
				k = k*(5+(p_ptr->lev*2/25))/2;
			}
			/* 地震は起こらない*/

			/*:::クリティカル処理*/
			if (!dokubari)
			k = critical_norm(o_ptr->weight * 3 / 2 + p_ptr->lev * 2, o_ptr->to_h*2, k, p_ptr->to_h[0], mode, &critical_rank);
			//v1.1.94 重量アップ
			//k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[0], mode, &critical_rank);



			drain_result = k;
			/*:::ヴォーパルヒット処理*/
			if (vorpal_cut)
			{
				int mult = 2;
				if (o_ptr->name1 == ART_VORPAL_BLADE)
					msg_print(_("目にも止まらぬヴォーパルブレード、手錬の早業！", "The vorpal blade goes snicker-snack!"));
				else
					msg_format(_("刃が%sを鋭く切り裂いた！", "The blade cuts deeply into %s!"), m_name);
				while (one_in_(vorpal_chance))
				{
					mult++;
				}

				k *= mult;

				switch (mult)
				{
					case 2: //msg_format("%sを斬った！", m_name); break;
					case 3: msg_format(_("%sをぶった斬った！", "%s received a deep cut!"), m_name); break;
                    case 4: //msg_format("%sをメッタ斬りにした！", m_name); break;
                    case 5: msg_format(_("%sを滅多斬りにした！", "%s is severely cut up!"), m_name); break;
                    case 6: //msg_format("%sを刺身にした！", m_name); break;
                    case 7: default:msg_format(_("%sを斬って斬って斬りまくった！", "%s is cut up all over!"), m_name); break;
					//default: msg_format("%sを細切れにした！", m_name); break;
				}
				drain_result = drain_result * 3 / 2;

				//v1.1.94
				if (critical_rank < mult - 1) critical_rank = mult - 1;
				if (critical_rank > 5) critical_rank = 5;

			}

			k += o_ptr->to_d;
			drain_result += o_ptr->to_d;
			k += p_ptr->to_d[0];
			drain_result += p_ptr->to_d[0];

			if (flag_katana_critical_done && weird_luck())
			{
				msg_print(_("必殺の一太刀だ！", "That was a critical blow!"));
				k *= 5;
			}

			/* No negative damage */
			if (k < 0) k = 0;

			/* Modify the damage */
			/*:::無傷球とはぐれメタルの処理*/
			k = mon_damage_mod(m_ptr, k, FALSE);

			///item 毒針
			if (dokubari)
			{
				if ((randint1(randint1(r_ptr->level/7)+5) == 1) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
					msg_format(_("%sの急所を突き刺した！", "%s got hit on a critical spot!"), m_name);
				}
				else k = 1;
			}
			//v1.1.94 攻撃後の朦朧付与などをここで行う
			else
			{
				if (skill_type == TV_HAMMER) attack_effect_type = ATTACK_EFFECT_STUN;
				if (skill_type == TV_AXE) attack_effect_type = ATTACK_EFFECT_DEC_DEF;
				if (skill_type == TV_POLEARM) attack_effect_type = ATTACK_EFFECT_DELAY;

				apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,TRUE);
			}



			if( k > 9999) k = 9999;
			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
				msg_format(_("%d/%d のダメージを与えた。", "Dealt %d/%d damage."), k, m_ptr->hp);
			}

			if (k <= 0)
			{
				can_drain = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;

				//敵を倒したときまだ人形の攻撃回数が残っていればその分行動力の消費が少し下がる
				if (energy_use && p_ptr->lev > 24)
				{
					energy_use = 25 + 75 * (num+1) / total_blow_num;
				}
			}

			/*:::吸血処理 モンスターを倒した時にも効くようにした*/
			if (can_drain && (drain_result > 0))
			{
				///item 村正の特殊強化処理
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
							msg_print(_("妖刀は血を吸って強くなった！", "The spectral katana drinks blood and grows stronger!"));

							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 敵に止めを刺した時はHPが大幅に回復することにしてみる
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if (cheat_xtra)
						{
							msg_format("Draining left: %d", drain_left);
						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}
							if (drain_msg)
							{
								msg_format(_("%sから生命力を吸い取った！", "Life force is drained from %s!"), m_name);
								drain_msg = FALSE;
							}

							/*:::突然変異が多いと吸血の回復量が減るらしい*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			can_drain = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			///mod140412 この二行は吸血処理の前だったが後ろに持ってきた
			if (k > 0) anger_monster(m_ptr);
			//アリスの人形攻撃はオーラダメージを受けない
			//touch_zap_player(m_ptr);


			/* Confusion attack */
			/*:::混乱の手処理*/
			///sys ぬえの混乱の手処理とかこの辺に乗せればいいか
			if (p_ptr->special_attack & ATTACK_CONFUSE )
			{
				p_ptr->special_attack &= ~(ATTACK_CONFUSE);
				msg_print(_("手の輝きがなくなった。", "Your hands stop glowing."));
				p_ptr->redraw |= (PR_STATUS);

				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
					msg_format(_("%^sには効果がなかった。", "%^s is unaffected."), m_name);

				}
				else if (randint0(100) < r_ptr->level)
				{
					msg_format(_("%^sには効果がなかった。", "%^s is unaffected."), m_name);
				}
				else
				{
					msg_format(_("%^sは混乱したようだ。", "%^s is confused."), m_name);
					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}

		}

		/* Player misses */
		else
		{
			fuiuchi = FALSE; /* Clumsy! */
			{
				/* Sound */
				sound(SOUND_MISS);
				if(ex_attack_msg)
					msg_format(_("%sの攻撃は%sにかわされた。", "%s misses %^s."),alice_doll_name[doll_num], m_name);
				else
#ifdef JP
					msg_format("ミス！ %sにかわされた。", m_name);
#else
                    msg_format("%s misses %^s.",alice_doll_name[doll_num], m_name);
#endif
			}

		}
		fuiuchi = FALSE;
		if(flag_katana_critical_done) flag_katana_critical = FALSE;


		num++;
	}

	if (weak && !(*mdeath))
	{
		msg_format(_("%sは弱くなったようだ。", "%s looks weaker."), m_name);
	}

}




/*:::指定グリッドへ攻撃する。x,yにはモンスターがいること。*/
bool py_attack(int y, int x, int mode)
{
	bool            fear = FALSE;
	bool            mdeath = FALSE;
	bool            stormbringer = FALSE;

	//アリス人形攻撃用フラグ　反魔法洞窟で攻撃不可
	bool			alice_doll_attack = (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts);

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	if(!c_ptr->m_idx)
	{
		msg_print(_("ERROR:py_attack()で指定グリッドにモンスターがいない",
                    "ERROR: No monster in selected grid in py_attack()"));
		return FALSE;
	}

	if(m_ptr->r_idx < 1 || m_ptr->r_idx >= max_r_idx)
	{
		msg_format(_("ERROR:py_attack()で指定グリッドのモンスターのr_idxがおかしい(%d)",
                    "ERROR: Weird r_idx (%d) for monster in selected grid in py_attack()"),m_ptr->r_idx);
		delete_monster_idx(c_ptr->m_idx);
		return FALSE;
	}

	/* Disturb the player */
	disturb(0, 1);

	//v1.1.14 依姫の祇園様がリセットされる処理。通常はprocess_playerのループ後に行うがメッセージに違和感あるので最初にやっとく
	if(p_ptr->pclass == CLASS_YORIHIME && mode != HISSATSU_GION && (p_ptr->kamioroshi & KAMIOROSHI_GION))
		reset_concentration(TRUE);

	if(SAKUYA_WORLD)
	{
		msg_print(_("今は攻撃ができない。", "You can't attack right now."));
		return FALSE;
	}

	//美鈴格闘の消費行動力減少処理
	if(p_ptr->pclass == CLASS_MEIRIN && p_ptr->do_martialarts)
	{
		energy_use = p_ptr->magic_num1[0];
		if(cheat_xtra) msg_format("Energy_use:%d",energy_use);
	}
	//雷鼓プリスティンビートの消費行動力減少処理
	else if(p_ptr->pclass == CLASS_RAIKO && music_singing(MUSIC_NEW_RAIKO_PRISTINE))
	{
		energy_use = 75 - p_ptr->concent * 5;
		if(energy_use < 25) energy_use = 25;

		if(cheat_xtra) msg_format("Energy_use:%d",energy_use);
	}
	else if(mode == HISSATSU_GION || mode == HISSATSU_DOUBLESCORE || mode == HISSATSU_COUNTER_SPEAR)
	{
		; //カウンター系の攻撃は行動消費なし
	}
	else
		energy_use = 100;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags1 & RF1_NEVER_BLOW)
	{
		msg_print(_("この身体は直接攻撃に向かないようだ。", "This body isn't suitable for attacking directly."));
		return FALSE;
	}

	/*:::両手が武器以外で塞がっており攻撃用変異も持ってない場合、攻撃できない*/
	///sys 両手がふさがってると攻撃できない処理　追加格闘の1～2回くらい出てもいいかもしれない
	if (!p_ptr->migite && !p_ptr->hidarite &&
	    !(p_ptr->muta2 & (MUT2_HORNS | MUT2_BIGHORN | MUT2_HARDHEAD | MUT2_BIGTAIL)))
	{
#ifdef JP
		msg_format("%s攻撃できない。", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "両手がふさがって" : "");
#else
		msg_print("You cannot do attacking.");
#endif
		return FALSE;
	}

	//九十九姉妹の演奏中断(ダブルスコア演奏中の格闘攻撃除く)
	if((p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI) && music_singing_any())
	{
		if(music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE) && p_ptr->do_martialarts) mode = HISSATSU_DOUBLESCORE;
		else stop_tsukumo_music();
	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);
	/*:::モンスターが見えてる場合、思い出やHPバーの情報更新フラグを立てる*/
	if (m_ptr->ml)
	{
		/* Auto-Recall if possible and visible */
		if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

		/* Track a new monster */
		health_track(c_ptr->m_idx);
	}

	///del131223 斬鉄剣制限解除
#if 0
	if ((r_ptr->flags1 & RF1_FEMALE) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image || !m_ptr->ml))
	{
		if ((inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (inventory[INVEN_LARM].name1 == ART_ZANTETSU))
		{
#ifdef JP
			msg_print("拙者、おなごは斬れぬ！");
#else
			msg_print("I can not attack women!");
#endif
			return FALSE;
		}
	}
#endif
	if ((d_info[dungeon_type].flags1 & DF1_NO_MELEE) && !alice_doll_attack)
	{
#ifdef JP
		msg_print("なぜか攻撃することができない。");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return FALSE;
	}
	if(alice_doll_attack)
	{
		if (d_info[dungeon_type].flags1 & DF1_NO_MAGIC || p_ptr->anti_magic)
		{
			msg_print(_("人形が動かない！", "Your dolls aren't moving!"));
			return FALSE;
		}
		if(p_ptr->confused)
		{
			msg_print(_("混乱していて人形を操れない！", "You cannot manipulate your dolls while confused!"));
			return FALSE;
		}

	}

	///sys class item フレンドリーアタック　ストブリ
	/* Stop if friendly */
	if (!is_hostile(m_ptr) && !flag_friendly_attack &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image ||
	    p_ptr->shero || !m_ptr->ml))
	{
		if (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (!check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (stormbringer)
		{
#ifdef JP
			msg_format("黒い刃は強欲に%sを攻撃した！", m_name);
#else
			msg_format("Your black blade greedily attacks %s!", m_name);
#endif
			///del131223 virtue
			//chg_virtue(V_INDIVIDUALISM, 1);
			//chg_virtue(V_HONOUR, -1);
			//chg_virtue(V_JUSTICE, -1);
			//chg_virtue(V_COMPASSION, -1);
		}
		else if ( p_ptr->pseikaku != SEIKAKU_BERSERK && mode != HISSATSU_EXCLAW && mode != HISSATSU_YOSHIKA )
		{
#ifdef JP
			if (get_check("本当に攻撃しますか？"))
#else
			if (get_check("Really hit it? "))
#endif
			{
				///del131223 virtue
				//chg_virtue(V_INDIVIDUALISM, 1);
				//chg_virtue(V_HONOUR, -1);
				//chg_virtue(V_JUSTICE, -1);
				//chg_virtue(V_COMPASSION, -1);
			}
			else
			{
#ifdef JP
				msg_format("%sを攻撃するのを止めた。", m_name);
#else
				msg_format("You stop to avoid hitting %s.", m_name);
#endif
				return FALSE;
			}
		}
	}


	/* Handle player fear */
	if (p_ptr->afraid && mode != HISSATSU_EXCLAW && mode != HISSATSU_YOSHIKA && mode != HISSATSU_GION && !alice_doll_attack && mode != HISSATSU_DOUBLESCORE
		|| p_ptr->pclass == CLASS_KOMACHI && m_ptr->r_idx == MON_EIKI)
	{
		/* Message */
		if (m_ptr->ml)
#ifdef JP
			msg_format("恐くて%sを攻撃できない！", m_name);
#else
			msg_format("You are too afraid to attack %s!", m_name);
#endif

		else
#ifdef JP
			msg_print ("そっちには何か恐いものがいる！");
#else
			msg_print ("There is something scary in your way!");
#endif

		/* Disturb the monster */
		(void)set_monster_csleep(c_ptr->m_idx, 0);

		/* Done */
		return FALSE;
	}

	//ドッペルゲンガー特殊処理
	if(r_ptr->flags7 & RF7_DOPPELGANGER)
	{
		msg_print(_("あなたは自分のドッペルゲンガーに触れてしまった！",
                    "Oh no! You've touched your doppelganger!"));
		take_hit(DAMAGE_LOSELIFE,p_ptr->chp,_("ドッペルゲンガーとの接触", "contact with doppelganger"),-1);
		set_stun(100+randint0(3));
		delete_monster_idx(c_ptr->m_idx);
		return TRUE;
	}


	/*:::敵の寝込みを襲ったときの徳減少*/
	///sysdel virtue
#if 0
	if (MON_CSLEEP(m_ptr)) /* It is not honorable etc to attack helpless victims */
	{
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}
#endif
	/*:::二刀流技能経験値を得る*/
	///mod151205 三月精を除く
	if (p_ptr->migite && p_ptr->hidarite && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		///mod131226 skill 技能と武器技能の統合
		gain_skill_exp(SKILL_2WEAPONS,m_ptr);
		/*
		if ((p_ptr->skill_exp[GINOU_NITOURYU] < s_info[p_ptr->pclass].s_max[GINOU_NITOURYU]) && ((p_ptr->skill_exp[GINOU_NITOURYU] - 1000) / 200 < r_ptr->level))
		{
			if (p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_BEGINNER)
				p_ptr->skill_exp[GINOU_NITOURYU] += 80;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_SKILLED)
				p_ptr->skill_exp[GINOU_NITOURYU] += 4;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_EXPERT)
				p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_MASTER)
				if (one_in_(3)) p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			p_ptr->update |= (PU_BONUS);
		}
		*/
	}
	/*:::乗馬技能経験値を得る*/
	///sys 乗馬経験値処理
	/* Gain riding experience */
	if (p_ptr->riding && !CLASS_RIDING_BACKDANCE)
	{
		///mod131226 skill 技能と武器技能の統合
		gain_skill_exp(SKILL_RIDING,m_ptr);
#if 0
		int cur = p_ptr->skill_exp[GINOU_RIDING];
		int max = s_info[p_ptr->pclass].s_max[GINOU_RIDING];

		if (cur < max)
		{
			int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
			int targetlevel = r_ptr->level;
			int inc = 0;

			if ((cur / 200 - 5) < targetlevel)
				inc += 1;

			/* Extra experience */
			if ((cur / 100) < ridinglevel)
			{
				if ((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			p_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);

			p_ptr->update |= (PU_BONUS);
		}
#endif
	}

	riding_t_m_idx = c_ptr->m_idx;

	/*:::右手、左手、変異部位の順に攻撃　ターゲットが死んだら中止*/
	/*:::格闘のときはmigiteのみTRUEになってるはず*/
	/*:::↑弓枠変更により、クロスボウか銃を持っているときはhidariteがTRUEになって格闘をすることになった。*/
	///mod150808 アリス特殊処理追加

	//モンスター変身時隣接攻撃
	if(IS_METAMORPHOSIS)
	{
		monplayer_attack_monst(c_ptr->m_idx);
	}
	//アリス人形隣接攻撃
	else if(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
	{
		py_attack_aux_alice(y, x, &fear, &mdeath, mode);
	}
    //咲夜ソウルスカルプチュア
	else if(mode == HISSATSU_SOULSCULPTURE && p_ptr->pclass == CLASS_SAKUYA)
	{
		if (p_ptr->migite && inventory[INVEN_RARM].tval == TV_KNIFE)
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);
		if (p_ptr->hidarite && !mdeath && inventory[INVEN_RARM].tval == TV_KNIFE)
			py_attack_aux(y, x, &fear, &mdeath, 1, mode);
	}
	else
	{
		if (p_ptr->migite)
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);
		//追加処理　針妙丸特殊攻撃は右手分のみ
		if (p_ptr->hidarite && !mdeath && mode != HISSATSU_SHINMYOU)
			py_attack_aux(y, x, &fear, &mdeath, 1, mode);

	}

	///mod140323 追加格闘処理 特殊攻撃の時は出ない
	//ここに条件追加したらdisplay_player_various()の「+α」表示条件式にも追加する
	if(!mdeath && !(IS_METAMORPHOSIS) && !mode && !(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_EXATACK)
		&& p_ptr->pclass != CLASS_3_FAIRIES )
		py_attack_aux2(y,x,&fear,&mdeath);

	/* Mutations which yield extra 'natural' attacks */
	///sys 変異部位攻撃　ここの内容は↑のpy_attack_aux2()に移した
#if 0
	if (!mdeath)
	{
		if ((p_ptr->muta2 & MUT2_HORNS) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HORNS, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BIGHORN) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BIGHORN, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_HARDHEAD) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HARDHEAD, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TAIL, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BIGTAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BIGTAIL, &fear, &mdeath);
	}
#endif

	/* Hack -- delay fear messages */
	if (fear && m_ptr->ml && !mdeath)
	{
		/* Sound */
		sound(SOUND_FLEE);

		/* Message */
#ifdef JP
		msg_format("%^sは恐怖して逃げ出した！", m_name);
#else
		msg_format("%^s flees in terror!", m_name);
#endif

	}

	//尤魔は攻撃するたびに満腹度を消費する
	if (p_ptr->pclass == CLASS_YUMA)
	{
		set_food(p_ptr->food - (100 + p_ptr->lev * 8));
	}

	if ((p_ptr->special_defense & KATA_IAI) && ((mode != HISSATSU_IAI) || mdeath))
	{
		set_action(ACTION_NONE);
	}

	return mdeath;
}

/*:::二点を入力し、パターンの上を歩くときの歩行可能判定　*/
bool pattern_seq(int c_y, int c_x, int n_y, int n_x)
{
	feature_type *cur_f_ptr = &f_info[cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &f_info[cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if (!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if (pattern_type_new == PATTERN_TILE_START)
	{
		if (!is_pattern_tile_cur && !p_ptr->confused && !p_ptr->stun && !p_ptr->image)
		{
#ifdef JP
			if (get_check("パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"))
#else
			if (get_check("If you start walking the Pattern, you must walk the whole way. Ok? "))
#endif
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((pattern_type_new == PATTERN_TILE_OLD) ||
		 (pattern_type_new == PATTERN_TILE_END) ||
		 (pattern_type_new == PATTERN_TILE_WRECKED))
	{
		if (is_pattern_tile_cur)
		{
			return TRUE;
		}
		else
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_new == PATTERN_TILE_TELEPORT) ||
		 (pattern_type_cur == PATTERN_TILE_TELEPORT))
	{
		return TRUE;
	}
	else if (pattern_type_cur == PATTERN_TILE_START)
	{
		if (is_pattern_tile_new)
			return TRUE;
		else
		{
#ifdef JP
			msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
			msg_print("You must walk the Pattern in correct order.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_cur == PATTERN_TILE_OLD) ||
		 (pattern_type_cur == PATTERN_TILE_END) ||
		 (pattern_type_cur == PATTERN_TILE_WRECKED))
	{
		if (!is_pattern_tile_new)
		{
#ifdef JP
			msg_print("パターンを踏み外してはいけません。");
#else
			msg_print("You may not step off from the Pattern.");
#endif

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!is_pattern_tile_cur)
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
		else
		{
			byte ok_move = PATTERN_TILE_START;
			switch (pattern_type_cur)
			{
				case PATTERN_TILE_1:
					ok_move = PATTERN_TILE_2;
					break;
				case PATTERN_TILE_2:
					ok_move = PATTERN_TILE_3;
					break;
				case PATTERN_TILE_3:
					ok_move = PATTERN_TILE_4;
					break;
				case PATTERN_TILE_4:
					ok_move = PATTERN_TILE_1;
					break;
				default:
					if (p_ptr->wizard)
#ifdef JP
						msg_format("おかしなパターン歩行、%d。", pattern_type_cur);
#else
						msg_format("Funny Pattern walking, %d.", pattern_type_cur);
#endif

					return TRUE; /* Goof-up */
			}

			if ((pattern_type_new == ok_move) ||
			    (pattern_type_new == pattern_type_cur))
				return TRUE;
			else
			{
				if (!is_pattern_tile_new)
#ifdef JP
					msg_print("パターンを踏み外してはいけません。");
#else
					msg_print("You may not step off from the Pattern.");
#endif
				else
#ifdef JP
					msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif

				return FALSE;
			}
		}
	}
}

/*:::＠が入れる地形かどうか判断する*/
bool player_can_enter(s16b feature, u16b mode)
{
	feature_type *f_ptr = &f_info[feature];

	if (p_ptr->riding) return monster_can_cross_terrain(feature, &r_info[m_list[p_ptr->riding].r_idx], mode | CEM_RIDING);

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && p_ptr->levitation) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && p_ptr->can_swim) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS) && p_ptr->pass_wall) return TRUE;

	if (!have_flag(f_ptr->flags, FF_WALL) && have_flag(f_ptr->flags, FF_CAN_PASS) && CHECK_FAIRY_TYPE == 41) return TRUE;

	//v1.1.57 隠岐奈はドアを抜けられる
	if (p_ptr->pclass == CLASS_OKINA && have_flag(f_ptr->flags, FF_DOOR) ) return TRUE;

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	return TRUE;
}


/*
 * Move the player
 */
/*:::＠の場所を変更する。nx,ny:移動先 mpe_mode:移動モード MPE_????の形でdefines.hに定義*/
/*:::この関数が呼ばれた時点でnx,nyは移動可能であると確認されていること*/
/*:::＠が階移動したり死んだりせずそのグリッドに居ればTRUEを返す*/
bool move_player_effect(int ny, int nx, u32b mpe_mode)
{
	cave_type *c_ptr = &cave[ny][nx];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	//v1.1.14 依姫の祇園様がリセットされる処理。通常はprocess_playerのループ後に行うがメッセージに違和感あるので最初にやっとく
	if(p_ptr->pclass == CLASS_YORIHIME && (p_ptr->kamioroshi & KAMIOROSHI_GION) )
	{
		if(py != ny || px != nx) reset_concentration(TRUE);
		else reset_concent = FALSE;
	}


	if (!(mpe_mode & MPE_STAYING))
	{
		int oy = py;
		int ox = px;
		cave_type *oc_ptr = &cave[oy][ox];
		int om_idx = oc_ptr->m_idx; //＠がいた場所のモンスター(騎乗中モンスター 一緒に移動)
		int nm_idx = c_ptr->m_idx; //行先のモンスター(場所入れ替え)

		/* Move the player */
		py = ny;
		px = nx;
		/*:::モンスターと場所入れ替え？*/
		/* Hack -- For moving monster or riding player's moving */
		if (!(mpe_mode & MPE_DONT_SWAP_MON))
		{
			/* Swap two monsters */
			c_ptr->m_idx = om_idx;
			oc_ptr->m_idx = nm_idx;

			if (om_idx > 0) /* Monster on old spot (or p_ptr->riding) */
			{
				monster_type *om_ptr = &m_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_mon(om_idx, TRUE);
			}

			if (nm_idx > 0) /* Monster on new spot */
			{
				monster_type *nm_ptr = &m_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_mon(nm_idx, TRUE);
			}
		}

		/* Redraw old spot */
		lite_spot(oy, ox);

		/* Redraw new spot */
		lite_spot(ny, nx);

		/* Check for new panel (redraw map) */
		verify_panel();

		if (mpe_mode & MPE_FORGET_FLOW)
		{
			forget_flow();

			/* Mega-Hack -- Forget the view */
			p_ptr->update |= (PU_UN_VIEW);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
		}

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_DISTANCE);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if ((!p_ptr->blind && !no_lite()) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		/*:::迷宮などで地形を忘れる処理らしい*/
		if (dun_level && (d_info[dungeon_type].flags1 & DF1_FORGET)) wiz_dark();

		/* Handle stuff */
		if (mpe_mode & MPE_HANDLE_STUFF) handle_stuff();





		/*:::地形によりパラメータなどが変更されるときのメッセージ処理*/
		if(c_ptr->feat != oc_ptr->feat)
		{
			bool flag_update = FALSE;
			feature_type *of_ptr = &f_info[oc_ptr->feat];

			p_ptr->redraw |= PR_STATUS | PR_STATE;


			///mod140316 水地形パワーアップ処理などのためにPU_BONUSも更新するようにした

			if(p_ptr->pclass == CLASS_MURASA || p_ptr->pclass == CLASS_WAKASAGI ||  p_ptr->prace == RACE_KAPPA || p_ptr->prace == RACE_YAMAWARO )
			{
				/* 冗長なのでメッセージ削除
				if(have_flag(f_ptr->flags, FF_WATER) && !have_flag(of_ptr->flags, FF_WATER) )
					msg_print("力が湧き出る気がする！");
				if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
					msg_print("力が抜ける気がする・・");
				*/
				flag_update = TRUE;
			}
			else if(p_ptr->prace == RACE_NINGYO )
			{
				/*
				if(have_flag(f_ptr->flags, FF_WATER) && !have_flag(of_ptr->flags, FF_WATER) )
					msg_print("水に入った。");
				if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
					msg_print("陸に上がった。");
				*/
				flag_update = TRUE;
			}
			else if(CHECK_FAIRY_TYPE == 5 || CHECK_FAIRY_TYPE == 27 || CHECK_FAIRY_TYPE == 32 || CHECK_FAIRY_TYPE == 39 ||  CHECK_FAIRY_TYPE == 40 ||  CHECK_FAIRY_TYPE == 43)
			{
				flag_update = TRUE;
			}
			//v1.1.22 兵士潜伏技能 潜伏地形から出るときのことも考えてCHECK_CONCEALは使わない
			else if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_CONCEALMENT))
			{
				flag_update = TRUE;
			}
			//v1.1.47 ヤマメの蜘蛛の巣
			else if (p_ptr->pclass == CLASS_YAMAME)
			{
				flag_update = TRUE;
			}


			///mod140402 溺れ処理
			if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) )
			{

				if(prace_is_(RACE_VAMPIRE) && !p_ptr->levitation && !p_ptr->can_swim)
				{
					msg_print(_("あなたの体は瞬く間に水底へと沈んでいった・・",
                                "You quickly sink to the bottom..."));
					if(p_ptr->wild_mode)
					{
						/*:::-Hack- 全体マップ移動中はenergy_useが数倍になってて水に入った瞬間死にかねないのでここで直しておく*/
						energy_use = MIN(energy_use,100);
						change_wild_mode();
					}
					set_action(ACTION_NONE);
					flag_update = TRUE;
				}
				else if(!p_ptr->resist_water && !p_ptr->levitation && p_ptr->total_weight > weight_limit() && !p_ptr->can_swim)
				{
					msg_print(_("荷物が重くて泳げない！溺れている！",
                                "You are carrying too much and can't swim! You're drowning!"));
					if(p_ptr->wild_mode)
					{
						energy_use = MIN(energy_use,100);
						change_wild_mode();
					}
					set_action(ACTION_NONE);
					flag_update = TRUE;
				}
			}
			///mod150413 溺れ処理関連判定条件修正
		//	else if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
			else if((have_flag(f_ptr->flags, FF_WATER) || have_flag(of_ptr->flags, FF_WATER)) && f_ptr != of_ptr)
			{
					flag_update = TRUE;
			}

			p_ptr->redraw |= (PR_STATE); //地形情報表示のため外に出した
			if(flag_update)
			{
				p_ptr->update |= (PU_BONUS);
				handle_stuff();
			}
		}
		///mod150110 橙水没
		if(p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0] &&
			have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) && !p_ptr->levitation)
		{
			msg_print(_("水に入って式が剥がれてしまった！",
                        "You lose your shikigami power upon entering water!"));
			p_ptr->magic_num1[0] = 1;
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
		}

		//v1.1.35 ネムノ縄張り
		if((c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO) != (oc_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO))
		{
			if(c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
				msg_print(_("あなたは縄張りに戻った。", "You return to your sanctuary."));
			else
			{
				msg_print(_("あなたは縄張りから出た。", "You leave your sanctuary."));
				//v1.1.54 ネムノ召喚阻害特技の解除
				p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
				p_ptr->redraw |= (PR_STATUS);

			}
			disturb(0, 1);
			p_ptr->update |= (PU_BONUS | PU_HP);
			handle_stuff();
		}
		if (WINTER_LETTY1)
		{
			if (!cave_have_flag_bold(oy, ox, FF_COLD_PUDDLE))
			{
				msg_print(_("ここは寒気に満ちている。力が湧き出る！",
                            "It's really chilly in here. Your power grows!"));

			}

		}
		if (p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE) && !cave_have_flag_bold(oy, ox, FF_ELEC_PUDDLE))
		{
			msg_print(_("ここでは特技を使う負担が少なく済みそうだ。",
                        "Your special abilities will be easier to use here."));

		}


		/*:::超隠密*/
		///class 忍者の超隠密
		if (p_ptr->pclass == CLASS_NINJA)
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(FALSE);
			else if (p_ptr->cur_lite <= 0) set_superstealth(TRUE);
		}

		///sys 早駆けは深い水や溶岩では機能しないらしい
		if ((p_ptr->action == ACTION_HAYAGAKE) &&
		    (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!p_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP))))
		{
#ifdef JP
			msg_print("ここでは素早く動けない。");
#else
			msg_print("You cannot run in here.");
#endif
			set_action(ACTION_NONE);
		}
	}

	/*:::探索処理など*/
	if (mpe_mode & MPE_ENERGY_USE)
	{
		/*:::壁掘りの歌*/
		if (music_singing(MUSIC_WALL))
		{
			(void)project(0, 0, py, px, (60 + p_ptr->lev), GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM, -1);

			if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
		}

		/* Spontaneous Searching */
		///sys 知覚処理　探索と一本化しよう
		if ((p_ptr->skill_fos >= 50) || (0 == randint0(50 - p_ptr->skill_fos)))
		{
			search();
		}

		/* Continuous Searching */
		if (p_ptr->action == ACTION_SEARCH)
		{
			search();
		}
	}



	/* Handle "objects" */
	/*:::足元のアイテムに対する処理*/
	if (!(mpe_mode & MPE_DONT_PICKUP))
	{
		carry((mpe_mode & MPE_DO_PICKUP) ? TRUE : FALSE);
	}

	///mod140428 @の移動時およびアイテム拾い時に足元アイテム一覧を更新
	if(!running)
	{
		p_ptr->window |= PW_FLOOR_ITEM_LIST;
		window_stuff();
	}

	/* Handle "store doors" */
	if (have_flag(f_ptr->flags, FF_STORE))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter store */
		command_new = SPECIAL_KEY_STORE;
	}

	/* Handle "building doors" -KMW- */
	else if (have_flag(f_ptr->flags, FF_BLDG))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter building */
		command_new = SPECIAL_KEY_BUILDING;
	}

	/* Handle quest areas -KMW- */
	else if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter quest level */
		command_new = SPECIAL_KEY_QUEST;
	}
	/*:::出口到達でクリアになるタイプのクエストの場合？*/
	else if (have_flag(f_ptr->flags, FF_QUEST_EXIT))
	{
		if (quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_EXIT)
		{
			complete_quest(p_ptr->inside_quest);
		}

		leave_quest_check();

		p_ptr->inside_quest = c_ptr->special;
		dun_level = 0;
		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;

		p_ptr->leaving = TRUE;
	}

	/* Set off a trap */
	else if (have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MPE_STAYING))
	{
		/* Disturb */
		disturb(0, 1);

		/* Hidden trap */
		/*:::隠しトラップに引っかかった場合トラップ発見処理　ここを通らないときはトラップ解除に失敗したとき*/
		if (c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			/* Message */
#ifdef JP
			msg_print("トラップだ！");
#else
			msg_print("You found a trap!");
#endif

			/* Pick a trap */
			disclose_grid(py, px);
		}

		/* Hit the trap */
		//hit_trap((mpe_mode & MPE_BREAK_TRAP) ? TRUE : FALSE);

		activate_floor_trap(py,px,mpe_mode );


		if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if (!(mpe_mode & MPE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(c_ptr->info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect) disturb(0, 1);
		}
	}

	return player_bold(ny, nx) && !p_ptr->is_dead && !p_ptr->leaving;
}

/*:::無視していいトラップを判定する*/
bool trap_can_be_ignored(int feat)
{
	feature_type *f_ptr = &f_info[feat];

	if (!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (p_ptr->levitation) return TRUE;
		break;
	case TRAP_TELEPORT:
		if (p_ptr->anti_tele) return TRUE;
		break;
	case TRAP_FIRE:
		if (p_ptr->immune_fire) return TRUE;
		break;
	case TRAP_ACID:
		if (p_ptr->immune_acid) return TRUE;
		break;
	case TRAP_BLIND:
		if (p_ptr->resist_blind) return TRUE;
		break;
	case TRAP_CONFUSE:
		if (p_ptr->resist_conf) return TRUE;
		break;
	case TRAP_POISON:
		if (p_ptr->resist_pois) return TRUE;
		break;
	case TRAP_SLEEP:
		if (p_ptr->free_act) return TRUE;
		break;
	}

	return FALSE;
}


/*
 * Determine if a "boundary" grid is "floor mimic"
 */
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && \
	 !have_flag((MF)->flags, FF_OPEN))

/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
/*:::＠を指定した方向へ移動する。
 *:::移動は常にターンを消費し、そのグリッドにモンスターが居たら常に攻撃する。
 *:::壁に向かって移動するのはターンを消費せず壁の中の見えないモンスターは攻撃しない？
 *:::do_pickup:アイテムを拾う
 *:::break_trap:トラップを破壊する（狂戦士用？）
 *:::oktomove:移動に成功したかどうかのフラグ
 */
//v1.1.97 activate_trapフラグ追加。これがTRUEのとき罠を解除しないし浮遊があっても落とし穴や落とし戸に落ちる
void move_player(int dir, bool do_pickup, bool break_trap, bool activate_trap)
{
	/* Find the result of moving */
	/*:::移動先グリッドの位置を計算*/
	int y = py + ddy[dir];
	int x = px + ddx[dir];

	/* Examine the destination */
	/*:::移動先地形情報を得る*/
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	monster_type *m_ptr;

	/*:::騎乗中モンスターのmonster_typeとmonster_raceを得る*/
	monster_type *riding_m_ptr = &m_list[p_ptr->riding];
	monster_race *riding_r_ptr = &r_info[p_ptr->riding ? riding_m_ptr->r_idx : 0]; /* Paranoia */

	char m_name[80];

	bool p_can_enter = player_can_enter(c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool p_can_kill_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;




	if(IS_METAMORPHOSIS && !c_ptr->m_idx)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

		if(r_ptr->flags1 & RF1_NEVER_MOVE)
		{
			msg_print(_("あなたは動けない。", "You cannot move."));
			return;
		}
		if(cave_have_flag_bold(y,x,FF_WATER) && cave_have_flag_bold(y,x,FF_DEEP) && !(r_ptr->flags7 & (RF7_CAN_SWIM|RF7_CAN_FLY)))
		{
			msg_print(_("あなたは泳げない。", "You cannot swim."));
			return;
		}
		if(!cave_have_flag_bold(y,x,FF_WATER) && (r_ptr->flags7 & (RF7_AQUATIC)) && !(r_ptr->flags7 & (RF7_CAN_FLY)))
		{
			msg_print(_("あなたは陸上で動けない。", "You cannot move on land."));
			return;
		}
	}

	/* Exit the area */
	/*:::荒野で隣のマップに移るとき*/
	if (!dun_level && !p_ptr->wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		/* Can the player enter the grid? */
			/*:::＠が入れるかどうか確認する*/
		if (c_ptr->mimic && player_can_enter(c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				p_ptr->wilderness_y--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				p_ptr->wilderness_y++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				p_ptr->wilderness_x--;
				p_ptr->oldpx = cur_wid - 2;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				p_ptr->wilderness_x++;
				p_ptr->oldpx = 1;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}
			/*:::マップから出るときleaving*/
			p_ptr->leaving = TRUE;
			energy_use = 100;

			return;
		}

		/* "Blocked" message appears later */
		/* oktomove = FALSE; */
		/*:::通れないときのフラグ？*/
		p_can_enter = FALSE;
	}

	/* Get the monster */
	m_ptr = &m_list[c_ptr->m_idx];

	///item ストームブリンガー装備時のフラグ
	if (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (!check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	p_can_kill_walls = p_ptr->kill_wall && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!p_can_enter || !have_flag(f_ptr->flags, FF_LOS)) &&
		!have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack monsters */
	/*:::移動先にモンスターがいて、「見えているor見えてないがそのグリッドに移動できる」とき、攻撃処理*/
	if (c_ptr->m_idx && (m_ptr->ml || p_can_enter || p_can_kill_walls))
	{

		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		//咲夜の世界特殊処理
		if(SAKUYA_WORLD)
		{
			monster_desc(m_name, m_ptr, 0);
			msg_format(_("%^sの体が邪魔でそちらに行けない。",
                        "You cannot move here as %s is in your way."), m_name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		/* Attack -- only if we can see it OR it is not in a wall */
		/*:::モンスターが友好的で見えており、＠が混乱・幻覚・朦朧・狂戦士化変異でなく、＠がその場所に入れる場合押しのけ処理*/
		else if (!is_hostile(m_ptr) &&
		    !(p_ptr->confused || p_ptr->image || !m_ptr->ml || p_ptr->stun ||
		    ((p_ptr->muta2 & MUT2_BERS_RAGE) && p_ptr->shero)) &&
		    pattern_seq(py, px, y, x) && (p_can_enter || p_can_kill_walls))
		{
			/* Disturb the monster */
			/*:::起こす*/
			(void)set_monster_csleep(c_ptr->m_idx, 0);

			/* Extract monster name (or "it") */
			monster_desc(m_name, m_ptr, 0);

			if (m_ptr->ml)
			{
				/* Auto-Recall if possible and visible */
				if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

				/* Track a new monster */
				health_track(c_ptr->m_idx);
			}

			/* displace? */
			/*:::ストームブリンガーを装備しているとき、モンスターが友好的でもペットでも攻撃する？*/
			///class 狂戦士が一定確率で友好的な敵にも攻撃する処理
			if ((stormbringer && (randint1(1000) > 666)) || ( p_ptr->pseikaku == SEIKAKU_BERSERK))
			{
				py_attack(y, x, 0);
				oktomove = FALSE;
			}
			else if (monster_can_cross_terrain(cave[py][px].feat, r_ptr, 0))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^sが邪魔だ！", m_name);
#else
				msg_format("%^s is in your way!", m_name);
#endif

				energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			py_attack(y, x, 0);
			oktomove = FALSE;
		}
	}

	//v1.1.24 特殊処理　クエスト「急流下り」の水の中では左方に移動できない
	if(oktomove && QRKDR)
	{
		if(cave_have_flag_bold(py,px,FF_WATER) && x < px)
		{
			msg_print(_("急流に押し戻された！", "You get pushed back into the rapids!"));
			oktomove = FALSE;
			disturb(0, 1);
		}

	}

	/*:::騎乗時の処理さまざま*/
	if (oktomove && p_ptr->riding)
	{

		if (riding_r_ptr->flags1 & RF1_NEVER_MOVE)
		{
#ifdef JP
			msg_print("動けない！");
#else
			msg_print("Can't move!");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (MON_MONFEAR(riding_m_ptr))
		{

			/* Acquire the monster name */
			monster_desc(m_name, riding_m_ptr, 0);

			/* Dump a message */
#ifdef JP
			msg_format("%sが恐怖していて制御できない。", m_name);
#else
			msg_format("%^s is too scared to control.", m_name);
#endif
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (p_ptr->riding_ryoute && !CLASS_RIDING_BACKDANCE)
		{
			oktomove = FALSE;
			disturb(0, 1);
		}
		//v1.1.24 急流下りクエスト用特殊処理
		else if(QRKDR && have_flag(f_ptr->flags, FF_WATER) && !((riding_r_ptr->flags7 & (RF7_CAN_SWIM | RF7_AQUATIC))))
		{
			monster_desc(m_name, riding_m_ptr, 0);

			msg_format(_("%sが水に入りたがらない。", "%s doesn't want to go into the water."), m_name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_CAN_FLY) && (riding_r_ptr->flags7 & RF7_CAN_FLY))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_CAN_SWIM) && (riding_r_ptr->flags7 & RF7_CAN_SWIM))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_WATER) &&
			!(riding_r_ptr->flags7 & RF7_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || (riding_r_ptr->flags2 & RF2_AURA_FIRE)))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Can't swim.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (!have_flag(f_ptr->flags, FF_WATER) && (riding_r_ptr->flags7 & RF7_AQUATIC))
		{
#ifdef JP
			msg_format("%sから上がれない。", f_name + f_info[get_feat_mimic(&cave[py][px])].name);
#else
			msg_print("Can't land.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_LAVA) && !(riding_r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Too hot to go through.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		//v1.1.85 酸の沼と毒の沼も追加
		else if (have_flag(f_ptr->flags, FF_ACID_PUDDLE) && !(riding_r_ptr->flagsr & RFR_EFF_IM_ACID_MASK))
		{
			msg_format(_("%sの上に行けない。", "You can't move on %s."), f_name + f_info[get_feat_mimic(c_ptr)].name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_POISON_PUDDLE) && !(riding_r_ptr->flagsr & RFR_EFF_IM_POIS_MASK))
		{
			msg_format(_("%sの上に行けない。", "You can't move on %s."), f_name + f_info[get_feat_mimic(c_ptr)].name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}




		if (oktomove && MON_STUNNED(riding_m_ptr) && one_in_(2))
		{
			monster_desc(m_name, riding_m_ptr, 0);
#ifdef JP
			msg_format("%sが朦朧としていてうまく動けない！",m_name);
#else
			msg_format("You cannot control stunned %s!",m_name);
#endif
			oktomove = FALSE;
			disturb(0, 1);
		}
	}

	if (!oktomove)
	{
	}

	else if (!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !p_ptr->levitation)
	{

		//v1.1.86 山童は浮遊がなくても山脈を越えられることにした
		if (prace_is_(RACE_YAMAWARO) && have_flag(f_ptr->flags, FF_MOUNTAIN))
		{
			;
		}
		else
		{
			msg_format(_("空を飛ばないと%sの上には行けない。",
                        "You can't move on %s without flying."), f_name + f_info[get_feat_mimic(c_ptr)].name);

			energy_use = 0;
			running = 0;
			oktomove = FALSE;
		}

	}

	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	/*:::木の上で減速する処理*/
	//v1.1.86 山童は減速しないことにした
	else if (have_flag(f_ptr->flags, FF_TREE) && !p_can_kill_walls && !p_ptr->levitation)
	{
		if ((p_ptr->pclass != CLASS_RANGER && !prace_is_(RACE_YAMAWARO))  && (!p_ptr->riding || !(riding_r_ptr->flags8 & RF8_WILD_WOOD))) energy_use *= 2;
	}

	//v1.1.91 石油地形上で減速する処理
	else if (have_flag(f_ptr->flags, FF_OIL_FIELD) && !p_ptr->levitation)
	{
		if ((p_ptr->pclass != CLASS_YUMA )) energy_use *= 2;
	}


#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic && !activate_trap)
	{
		if (!trap_can_be_ignored(c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(y, x, dir);
			return;
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Player can not walk through "walls" unless in wraith form...*/
	else if (!p_can_enter && !p_can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		s16b feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &f_info[feat];
		cptr name = f_name + mimic_f_ptr->name;

		oktomove = FALSE;

		/* Notice things in the dark */
		if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めないようだ。");
#else
				msg_print("You feel you cannot go any more.");
#endif
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_format("%sが行く手をはばんでいるようだ。", name);
#else
				msg_format("You feel %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めない。");
#else
				msg_print("You cannot go any more.");
#endif

				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
#ifdef ALLOW_EASY_OPEN
				/* Closed doors */
				if (easy_open && is_closed_door(feat) && easy_open_door(y, x)) return;
#endif /* ALLOW_EASY_OPEN */

#ifdef JP
				msg_format("%sが行く手をはばんでいる。", name);
#else
				msg_format("There is %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}
		}

		/* Disturb the player */
		disturb(0, 1);

		/* Sound */
		if (!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}

	/* Normal movement */
	if (oktomove && !pattern_seq(py, px, y, x))
	{
		if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
		{
			energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(0, 1);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		u32b mpe_mode = MPE_ENERGY_USE;
		/*:::警告処理*/
		//v1.1.97 罠発動移動のとき警告を出なくする。危険なモンスターの方の警告も出なくなるがまあそれほど奇妙な挙動でもないだろう
		if (p_ptr->warning && !activate_trap)
		{
			if (!process_warning(x, y,FALSE))
			{
				energy_use = 25;
				return;
			}
		}


		//ドッペルゲンガー特殊処理
		if(do_past && r_info[m_ptr->r_idx].flags7 & RF7_DOPPELGANGER)
		{
			msg_print(_("あなたは自分のドッペルゲンガーに触れてしまった！",
                    "Oh no! You've touched your doppelganger!"));
			take_hit(DAMAGE_LOSELIFE,p_ptr->chp,_("ドッペルゲンガーとの接触", "contact with doppelganger"),-1);
			set_stun(100+randint0(3));
			energy_use = 100;
			delete_monster_idx(cave[y][x].m_idx);

			return;
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%sを押し退けた。", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (p_ptr->wild_mode)
		{
			if (ddy[dir] > 0)  p_ptr->oldpy = 1;
			if (ddy[dir] < 0)  p_ptr->oldpy = MAX_HGT - 2;
			if (ddy[dir] == 0) p_ptr->oldpy = MAX_HGT / 2;
			if (ddx[dir] > 0)  p_ptr->oldpx = 1;
			if (ddx[dir] < 0)  p_ptr->oldpx = MAX_WID - 2;
			if (ddx[dir] == 0) p_ptr->oldpx = MAX_WID / 2;
		}

		if (p_can_kill_walls)
		{
			cave_alter_feat(y, x, FF_HURT_DISI);

			/* Update some things -- similar to GF_KILL_WALL */
			p_ptr->update |= (PU_FLOW);
		}

		/* Sound */
		/* sound(SOUND_WALK); */

#ifdef ALLOW_EASY_DISARM /* TNB */

		if (do_pickup != always_pickup) mpe_mode |= MPE_DO_PICKUP;

#else /* ALLOW_EASY_DISARM -- TNB */

		if (do_pickup) mpe_mode |= MPE_DO_PICKUP;

#endif /* ALLOW_EASY_DISARM -- TNB */

		if (activate_trap) mpe_mode |= MPE_ACTIVATE_TRAP;
		else if (break_trap) mpe_mode |= MPE_BREAK_TRAP;

		/* Move the player */
		(void)move_player_effect(y, x, mpe_mode);
	}


}


static bool ignore_avoid_run;

/*
 * Hack -- Check for a "known wall" (see below)
 */
/*:::対象のグリッドが「記憶している壁」かどうか判定*/
static int see_wall(int dir, int y, int x)
{
	cave_type   *c_ptr;

	/* Get the new location */
	/*:::dirに入った1-9の数字によりxとyに-1～+1の計算をする*/
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are not known walls */
	if (!in_bounds2(y, x)) return (FALSE);

	/* Access grid */
	c_ptr = &cave[y][x];

	/* Must be known to the player */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Feature code (applying "mimic" field) */
		s16b         feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &f_info[feat];

		/* Wall grids are known walls */
		if (!player_can_enter(feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		/* Don't run on a tree unless explicitly requested */
		if (have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		/* Don't run in a wall */
		if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int y, int x)
{
	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_bounds2(y, x)) return (TRUE);

	/* Memorized grids are always known */
	if (cave[y][x].info & (CAVE_MARK)) return (FALSE);

	/* Viewable door/wall grids are known */
	if (player_can_see_bold(y, x)) return (FALSE);

	/* Default */
	return (TRUE);
}





/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner. (find_examine option is removed and always is TRUE.)
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

/*
 * The direction we are running
 */
static byte find_current;

/*
 * The direction we came from
 */
static byte find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
/*:::走るための初期化？詳細未読*/
static void run_init(int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;


	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	p_ptr->run_py = py;
	p_ptr->run_px = px;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	ignore_avoid_run = cave_have_flag_bold(row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(cycle[i+1], py, px))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(cycle[i-1], py, px))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], row, col))
		{
			if (shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
/*:::走ってるときの次の一歩の方向を決め、find_currentに格納しているらしい。詳細未読*/
static bool run_test(void)
{
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	s16b        feat;
	feature_type *f_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;


	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		s16b this_o_idx, next_o_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Visible object */
			if (o_ptr->marked & OM_FOUND) return (TRUE);
		}

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if (notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if (find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Stairs */
				else if (find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Lava */
				else if (have_flag(f_ptr->flags, FF_LAVA) && (p_ptr->immune_fire || IS_INVULN()))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (p_ptr->levitation || p_ptr->can_swim || (p_ptr->total_weight <= weight_limit())))
				{
					/* Ignore */
					notice = FALSE;
				}
				//v1.1.85 ここに酸の沼などに免疫があるときのnotice無視処理を書くべきなんだろうけどうまくまとまらんし多分そうそう使わんだろうからパス


			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if (inv || !see_wall(0, row, col))
		{
			/* Looking for open area */
			if (find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (find_openarea)
			{
				if (i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if (find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if (!find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, row, col) ||
			    !see_wall(check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (see_nothing(option, row, col) &&
				    see_nothing(option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}

	/* About to hit a known wall, stop */
	if (see_wall(find_current, py, px))
	{
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
/*:::与えられた方向へ走る*/
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if (see_wall(dir, py, px))
		{
			/* Message */
#ifdef JP
			msg_print("その方向には走れません。");
#else
			msg_print("You cannot run in that direction.");
#endif

			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	//v3.0.2 走るコマンドのenergy_use値に高速移動などを適用する
	walk_energy_modify();

	/* Move the player, using the "pickup" flag */
#ifdef ALLOW_EASY_DISARM /* TNB */

	move_player(find_current, FALSE, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

	move_player(find_current, always_pickup, FALSE, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */

	if (player_bold(p_ptr->run_py, p_ptr->run_px))
	{
		p_ptr->run_py = 0;
		p_ptr->run_px = 0;
		disturb(0, 0);
	}
}


#ifdef TRAVEL
/*
 * Test for traveling
 */
/*:::トラベルコマンドで進む方向を決める*/
static int travel_test(int prev_dir)
{
	int new_dir = 0;
	int i, max;
	const cave_type *c_ptr;
	int cost;

	/* Cannot travel when blind */
	if (p_ptr->blind || no_lite())
	{
		msg_print(_("目が見えない！", "You cannot see!"));
		return (0);
	}

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return (0);
			}
		}
	}

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		int dir = cycle[chome[prev_dir] + i];

		/* New location */
		int row = py + ddy[dir];
		int col = px + ddx[dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (0);
		}

	}

	/* Travel cost of current grid */
	cost = travel.cost[py][px];

	/* Determine travel direction */
	for (i = 0; i < 8; ++ i) {
		int dir_cost = travel.cost[py+ddy_ddd[i]][px+ddx_ddd[i]];

		if (dir_cost < cost)
		{
			new_dir = ddd[i];
			cost = dir_cost;
		}
	}

	if (!new_dir) return (0);

	/* Access newly move grid */
	c_ptr = &cave[py+ddy[new_dir]][px+ddx[new_dir]];

	/* Close door abort traveling */
	if (!easy_open && is_closed_door(c_ptr->feat)) return (0);

	/* Visible and unignorable trap abort tarveling */
	if (!c_ptr->mimic && !trap_can_be_ignored(c_ptr->feat)) return (0);

	/* Move new grid */
	return (new_dir);
}


/*
 * Travel command
 */
/*:::トラベルコマンドによる移動*/
//travel.runに値がある時呼ばれる
//travel_test()で目的地に近い隣のグリッドを決定しそこへ移動しtravel.runを1減らす
void travel_step(void)
{
	/* Get travel direction */
	travel.dir = travel_test(travel.dir);

	/* disturb */
	if (!travel.dir)
	{
		if (travel.run == 255)
		{
#ifdef JP
			msg_print("道筋が見つかりません！");
#else
			msg_print("No route is found!");
#endif
		}
		disturb(0, 1);
		return;
	}

	energy_use = 100;

	//v2.0.3 トラベルコマンドのenergy_use値に高速移動などを適用する
	walk_energy_modify();

	move_player(travel.dir, always_pickup, FALSE,FALSE);

	if ((py == travel.y) && (px == travel.x))
		travel.run = 0;
	else if (travel.run > 0)
		travel.run--;

	/* Travel Delay */
	/*:::Term->xtra_hookが設定されていればそれを実行する*/
	Term_xtra(TERM_XTRA_DELAY, delay_factor);
}
#endif

///mod131226 skill 技能と武器技能の統合のために関数追加
///mod140130 武器、技能経験値　計算部実装
/*:::格闘、盾、乗馬、二刀、投擲、武器経験値*/
/*:::skill_numには格闘0,乗馬1,盾2,二刀3,投擲4,5-9予備、武器の場合TVAL値が入り10(knife)～20(gun)に変換される*/
/*:::m_ptrはこの関数が呼ばれるとき攻撃した/された相手*/
void gain_skill_exp(int skill_num , monster_type* m_ptr)
{

	int n = skill_num;
	int amount;
	int old_skill_lev, new_skill_lev, max_skill_lev;
	cptr skill_desc;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];
	int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
	int targetlevel = r_ptr->level;
	int skill_max;

	//v1.1.41 舞と里乃の特殊騎乗は熟練度が入らない
	if (CLASS_RIDING_BACKDANCE && skill_num == SKILL_RIDING) return;


	/*:::武器TVAL(23～)を対応する技能値配列項目(10～)に変換する*/
	if(skill_num >= TV_KNIFE) n = skill_num -= SKILL_WEAPON_SHIFT;

	/*:::職業ごとのその技能の最大値を得る 適性値*1600 */
	skill_max = cp_ptr->skill_aptitude[n] * SKILL_LEV_TICK * 10;
	max_skill_lev = cp_ptr->skill_aptitude[n] * 10;
	old_skill_lev = p_ptr->skill_exp[n] / SKILL_LEV_TICK;

	if(n==0) skill_desc = _("格闘", "Martial Arts");
	else if(n==1) skill_desc = _("盾", "Shields");
	else if(n==2) skill_desc = _("乗馬", "Riding");
	else if(n==3) skill_desc = _("二刀流", "Dual Wielding");
	else if(n==4) skill_desc = _("投擲", "Throwing");
	else if(n==10) skill_desc = _("短剣", "Short Blades");
	else if(n==11) skill_desc = _("長剣", "Long Blades");
	else if(n==12) skill_desc = _("刀", "Katanas");
	else if(n==13) skill_desc = _("鈍器", "Blunt Weapons");
	else if(n==14) skill_desc = _("棒", "Staves");
	else if(n==15) skill_desc = _("斧", "Axes");
	else if(n==16) skill_desc = _("槍", "Spears");
	else if(n==17) skill_desc = _("長柄武器", "Polearms");
	else if(n==18) skill_desc = _("弓", "Bows");
	else if(n==19) skill_desc = _("クロスボウ", "Crossbows");
	else if(n==20) skill_desc = _("銃", "Guns");
	else if(n>=21&&n<=23) return; /*MAGICBOOK/MAGICWEAPON/OTHERWEAPONには熟練度がない*/
	else{
		msg_format(_("ERROR:gain_skill_exp()が現在使われていない技能(%d)で呼ばれた。",
                    "ERROR: Unkown skill called (%d) in gain_skill_exp()"),skill_num);
		return;
	}

	//既に上限の場合終了
	if(p_ptr->skill_exp[n] >= skill_max) return;


	//msg_format("TEST:RIDINGLEV:%d",ridinglevel);

	/*:::得られる技能経験値を計算　伸びしろが大きいと上がりやすい*/
	if(max_skill_lev - old_skill_lev > 30) amount = 20;
	else if(max_skill_lev - old_skill_lev > 20) amount = 10;
	else if(max_skill_lev - old_skill_lev > 10) amount = 5;
	else amount = 1;

	/*:::技能が低いと上がりやすい*/
	if(old_skill_lev < 10) amount += 4;
	else if(old_skill_lev < 20) amount += 3;
	else if(old_skill_lev < 30) amount += 2;
	else if(old_skill_lev < 40) amount += 1;

	//v1.1.94 銃は熟練度が上がりにくいとよく聞くのでちょっと調整しよう
	if (skill_num == TV_GUN) amount += randint1(amount);

	/*:::ターゲットや＠が弱いと上がりにくい*/
	if((targetlevel < old_skill_lev / 4) ) amount /= 10;
	else if((targetlevel < old_skill_lev / 2) ) amount /= 5;
	else if((targetlevel < old_skill_lev) && one_in_(2)) amount /= 2;
	if((p_ptr->lev < old_skill_lev ) && one_in_(2)) amount /= 2;

	/*:::騎乗モンスターが弱いと騎乗技能は上がりにくい*/
	if(n == SKILL_RIDING && (ridinglevel < old_skill_lev) && one_in_(2)) amount /= 2;
	/*:::盾は少し上がりにくい*/
	if(n == SKILL_SHIELD && one_in_(2)) amount /= 2;

	/*:::ペットによる技能値上げを抑止*/
	if(is_pet(m_ptr) && p_ptr->skill_exp[n] > 4000)
	{
		if(one_in_(10)) msg_format(_("もう配下が相手では%sの練習にはならない。",
                                    "You cannot train %s on your followers any more."),skill_desc);
		amount = 0;
	}
	/*:::主に増殖モンスターによる技能値上げを抑止*/
	if(r_ptr->r_akills > 100 && targetlevel < old_skill_lev)
	{
		if(one_in_(10)) msg_format(_("もうこの敵が相手では%sの練習にはならない。",
                                    "You cannot train %s on this enemy any more."),skill_desc);
		amount = 0;
	}
	//msg_format("GAIN_SKILL:(n:%d)%s:amount:%d",n,skill_desc,amount);

	p_ptr->skill_exp[n] += amount;
	/*::: 上限処理*/
	if(p_ptr->skill_exp[n] > skill_max) p_ptr->skill_exp[n] = skill_max;
	new_skill_lev = p_ptr->skill_exp[n] / SKILL_LEV_TICK;
	if(new_skill_lev > old_skill_lev){
		msg_format(_("%sの技能レベルが%dにアップした!", "Your skill of %s is now at level %d!"), skill_desc,new_skill_lev);
	}
	p_ptr->update |= (PU_BONUS);

}


/*:::武器、技能の経験値を参照する*/
s16b ref_skill_exp(int skill_num){
	int n = skill_num;
	if(skill_num >= TV_KNIFE) n = skill_num -= SKILL_WEAPON_SHIFT;

	if(n >= 5 && n <= 9 || n >= 24){
		msg_format(_("ERROR:ref_skill_exp()が現在使われていない技能(%d)で呼ばれた。",
                    "ERROR: Unused skill number (%d) called in ref_skill_exp()"),skill_num);
	}

	//隠岐奈「無縁の芸能者」実行中
	if (p_ptr->pclass == CLASS_OKINA && p_ptr->tim_general[0])
	{
		return ((p_ptr->stat_ind[A_CHR] + 3) * SKILL_LEV_TICK);
	}


	return p_ptr->skill_exp[n];
}

/*:::ヒット＆アウェイを独立ルーチンにした。ターゲットが選択されなかった時FALSE*/
bool hit_and_away(void)
{
	int x, y,dir;

	if (!get_rep_dir(&dir, FALSE)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	if (cave[y][x].m_idx)
	{
		py_attack(y, x, 0);
		//v1.1.30 オーラなどで倒れたらテレポしないようにしとく
		if (p_ptr->is_dead || randint0(p_ptr->skill_dis) < 7)
#ifdef JP
			msg_print("離脱失敗！");
#else
            msg_print("You failed to disengage!");
#endif
		else
		{
#ifdef JP
			msg_print("あなたは攻撃後離脱した。");
#else
            msg_print("You disengage after attacking.");
#endif
			teleport_player(30, 0L);
		}
	}
	else
	{
#ifdef JP
		msg_print("その方向にはモンスターはいません。");
#else
		msg_print("You don't see any monster in this direction");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}




