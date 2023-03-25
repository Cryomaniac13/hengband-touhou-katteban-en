#include "angband.h"
//v1.1.86
//新魔道具「アビリティカード」関係の関数



//千亦専用
//r_idxを渡して「このモンスターから生成できるアビリティカードはあるか」を判定する。
//複数あるときは最初のcard_idxを返す。
//なければ0を返す。0は「空白のカード」のcard_idxだが千亦の初期所持カードなので失敗値として使っても問題ない
int	chimata_can_copy(int r_idx)
{

	int i;
	monster_race *r_ptr;

	if (r_idx < 1 || r_idx >= max_r_idx) { msg_format("ERROR:chimata_can_copy(%d)", r_idx); return 0; }

	r_ptr = &r_info[r_idx];

	//カードidxでループ
	for (i = 1; i<ABILITY_CARD_LIST_LEN; i++)
	{
		//対象モンスターかどうか
		if (r_idx != ability_card_list[i].r_idx) continue;

		//magic_num2[card_idx]が0以外なら所有済みなのでパス
		if (p_ptr->magic_num2[i]) continue;

		//ゆのみレイマリはレベル90以上限定とする
		if (i == ABL_CARD_YUNOMI_REIMU && r_ptr->level < 90) continue;
		if (i == ABL_CARD_YUNOMI_MARISA && r_ptr->level < 90) continue;

		break;
	}

	if (i == ABILITY_CARD_LIST_LEN) return 0;
	return i;


}

//v2.0.7 千亦が空白のカードを使用してアビリティカードを作成する
//作ったカードはp_ptr->magic_num2[card_idx]を1にするという形で記録する
//行動順消費したときTRUE
static bool	chimata_use_blank_card(void)
{

	int i;
	monster_type *m_ptr;
	int y, x;
	int m_idx, dir;
	int new_card_idx;
	char m_name[120];
	int prob;

	int old_rank, new_rank;

	if (p_ptr->inside_arena)
	{
		msg_print(_("ここでは使えない。", "You can't use this here."));
		return FALSE;
	}

	//隣接モンスターを指定
	if (!get_rep_dir2(&dir) || dir == 5) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	m_idx = cave[y][x].m_idx;
	m_ptr = &m_list[m_idx];
	if (!m_idx || !m_ptr->ml)
	{
		msg_print(_("そこには何もいない。", "There's nobody here."));
		return FALSE;
	}

	//指定したモンスターからカードを生成可能か？
	new_card_idx = chimata_can_copy(m_ptr->r_idx);
	if (!new_card_idx)
	{
		msg_print(_("この者では今一つ意欲が湧かない。", "You're not feeling very motivated copying this person's ability."));
		return FALSE;
	}

	//成功率計算
	//(知能+器用+レベル差*2)% 寝てると+50%、起きてて恐怖混乱朦朧どれかなら+25%、防御低下状態なら+25%、最低保証5%
	//常に起きてて異常耐性完備でクエストにしか出ないフランと隠岐奈が一番の強敵か？あとマミゾウとぬえも相当大変。
	//lev70の隠岐奈の場合lev,INT,DEX全て40で成功率20%、破滅の薬とか投げつけて防御低下が入れば45%まで上がる計算
	//lev90レイマリはlev50INTDEX40からINTに限界突破入れて防御低下を通してようやく35%

	prob = (p_ptr->stat_ind[A_INT] + 3) + (p_ptr->stat_ind[A_DEX] + 3) + (p_ptr->lev - r_info[m_ptr->r_idx].level) * 2;
	if (MON_CSLEEP(m_ptr)) prob += 50;
	else if (MON_CONFUSED(m_ptr) || MON_STUNNED(m_ptr) || MON_MONFEAR(m_ptr)) prob += 25;
	if (MON_DEC_DEF(m_ptr)) prob += 25;

	if (prob > 100) prob = 100;
	if (prob < 5) prob = 5;

	if (!get_check_strict(format(_("実行しますか？(成功率%d%%)", "Attempt? (success rate %d%%)"), prob), CHECK_DEFAULT_Y)) return FALSE;

	monster_desc(m_name, m_ptr, 0);

	msg_format(_("あなたは%sの力を模倣し始めた...", "You start imitating the ability of %s..."),m_name);

	//成功
	if (randint0(100) < prob)
	{
		msg_format(_("成功だ！「%s」が完成した！", "Success! '%s' was created!"), ability_card_list[new_card_idx].card_name);

		old_rank = (CHIMATA_CARD_RANK);

		//入手済みフラグ
		p_ptr->magic_num2[new_card_idx] = 1;

		chimata_calc_card_rank();

		new_rank = (CHIMATA_CARD_RANK);

		if (old_rank != new_rank)
		{
			msg_print(_("アビリティカードの流通が新たな段階に入った気がする！",
               "You feel the popularity of ability cards reaching a new level!"));
			p_ptr->update |= (PU_BONUS | PU_LITE | PU_HP | PU_MANA);
			update_stuff();
		}

		//他にもある場合通知しとく
		if (chimata_can_copy(m_ptr->r_idx))
		{
			msg_print(_("まだ他にも作れそうだ！", "Looks like there's another ability card you could create!"));
		}

	}
	//失敗
	else
	{
		msg_print(_("力のコピーに失敗した！", "You failed to copy the ability!"));
	}

	//起きて敵対
	set_monster_csleep(cave[y][x].m_idx, 0);
	anger_monster(m_ptr);

	return TRUE;

}

//アビリティカード生成時にapply_magic()の代わりに呼ぶ。apply_magic()に放り込んでもそこから呼ばれるがランクなどの指定はできない。
//カードのIDXがo_ptr->pvalに記録される。IDXはability_card_list[]の添字に一致する。
//ability_card_list[]にカード名、カードの発動難度、チャージ時間などが設定されている。
//card_idx:生成するカードのidx idxが有効範囲外のときランダム生成
//card_rank_min/max:生成するカードのランクを指定したいとき。0ならランダム生成
//戻り値:生成されたカードのランク(乱数テスト用)
int apply_magic_abilitycard(object_type *o_ptr, int card_idx, int card_rank_min, int card_rank_max)
{

	int i;
	int new_rank = -1, new_idx = -1;
	bool flag_extra_bonus = FALSE;

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_print(_("ERROR:apply_magic_abilitycard()にTV_ABILITY_CARD以外が渡された",
                    "ERROR: Tval that isn't TV_ABILITY_CARD was passed to apply_magic_abilitycard()"));
		return 0;
	}

	//カードidxが指定されているとき
	if (card_idx >= 0 && card_idx < ABILITY_CARD_LIST_LEN)
	{
		new_idx = card_idx;
	}
	else
	{
		int candi_num = 0;

		//指定されていないときカードをランダム生成。まずランクを決める

		if (card_rank_min < 2 && !one_in_(5)) new_rank = 1;//コモン 80%
		else if (card_rank_min < 3 && !one_in_(5)) new_rank = 2;//レア 16%
		else if (card_rank_min < 4 && !one_in_(5)) new_rank = 3;//ユニーク？スーパーレア？ 3.2%
		else new_rank = 4; //レジェンダリーとかUSSRとかそんなの 0.8%

		//weird_luckを通るとランクアップすることにしてみる。グラフィックがあるなら特殊演出を出したいところだ。
		//通常は1/12、白オーラか占い幸運は1/9、両方重複するか幸運のメカニズムで1/7で通る。
		//weird_luck込みの排出率は↓のようになるはず。
		// 1/12 73%/22%/4.3%/1.1%
		// 1/7  69%/30%/5%/1.25%
		if (weird_luck()) new_rank++;

		if (card_rank_max > 0 && new_rank > card_rank_max) new_rank = card_rank_max;

		//ランク4にさらにweird_luck()を通ってランク5になってしまったら4に戻す。
		if (new_rank > 4)
		{
			//箱を開けたときのみ、ランク5になったらボーナスでもう一枚。箱の中から箱が出てこないようにするグローバルフラグを再利用
			if(opening_chest ) flag_extra_bonus = TRUE;
			new_rank = 4;
		}

		//カードリストの中からレアリティが一致したカードをランダムに抽選
		for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
		{
			if (ability_card_list[i].rarity_rank != new_rank) continue;

			//EXTRAモードでは帰還用カードが出ない
			if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;

			//現実変容カードも出ない
			if (EXTRA_MODE && i == ABL_CARD_DOREMY) continue;

			//市場破壊カードも出ない
			if (EXTRA_MODE && i == ABL_CARD_NITORI) continue;


			candi_num++;
			if (!one_in_(candi_num)) continue;
			new_idx = i;
		}

		if (new_idx < 0)
		{
			msg_format(_("ERROR:カードが一枚も選定されなかった(min:%d max:%d)",
                        "ERROR: No cards were chosen (min:%d max:%d)"), card_rank_min, card_rank_max);
			return 0;
		}
	}

	//カードの種類をpvalに格納
	o_ptr->pval = new_idx;

	//当たりが出たらもう一枚
	if (flag_extra_bonus) o_ptr->number++;

	//*鑑定*済にする
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	return new_rank;
}


//特定のアビリティカードをザック内に何枚保有しているか確認する。
//職によっては追加インベントリも確認する。
//card_idxはability_card_list[]の添字
//戻り値はカードの合計枚数(複数箇所に持っているときは全ての合計枚数)。仕様上10以上のときも9が返る
int	count_ability_card(int card_idx)
{
	int num = 0;
	int i;
	int inven2_num;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:count_ability_card()に不正なidx(%d)が渡された",
                    "ERROR: Incorrect idx (%d) passed to count_abiltiy_card()"), card_idx);
		return 0;

	}

	//千亦はp_otr->magic_num2[card_idx]に保有の有無を、カード流通ランクで枚数を記録
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		if (!CHECK_CHIMATA_HAVE_CARD(card_idx)) return 0;

		return (CHIMATA_CARD_RANK);

	}

	//character_ickyがTRUEのときは、店や自宅に置かれたアイテムを調べているものと見なして個数を常に1にする
	//if (character_icky) return 1;
	//v1.1.87 ↑店で買い物とかしたときにcalc_bonuses()が呼ばれてるらしく＠のパラメータが一時的に変になるのでグローバル変数のフラグに置き換えた
	if (hack_flag_store_examine) return 1;


	//ザック内(インベントリ0～22)をチェック
	for (i = 0; i<INVEN_PACK; i++)
	{
		if (!inventory[i].k_idx) continue;
		if (inventory[i].tval != TV_ABILITY_CARD) continue;
		//カードの種類はpvalやsvalでなくxtra1で管理している
		if (inventory[i].pval == card_idx) num += inventory[i].number;
	}

	inven2_num = calc_inven2_num();

	//v1.1.87 追加インベントリがあればそれもチェック 菫子のは所有といえるかどうかちょっと怪しいがまあ良し
	for (i = 0; i < inven2_num; i++)
	{
		if(!inven_add[i].k_idx) continue;
		if (inven_add[i].tval != TV_ABILITY_CARD) continue;
		if (inven_add[i].pval == card_idx) num += inven_add[i].number;
	}

	//所有型のカードを10枚以上持っても効果は上昇しない
	if (num>9) num = 9;

	return num;

}

//カードによるHP,SP上昇などのパラメータ計算
int calc_ability_card_mod_param(int card_idx, int card_num)
{
	int bonus = 0;
	int plev = p_ptr->lev;
	switch (card_idx)
	{
		case ABL_CARD_LIFE:
		{
			bonus = plev;

			if (card_num > 1) bonus += plev * (card_num - 1) /10;
			if (card_num == 9) bonus = plev * 2;

		}
		break;
		case ABL_CARD_SPELL:
		{
			bonus = plev / 2;

			if (card_num > 1) bonus += plev * (card_num - 1) / 20;
			if (card_num == 9) bonus = plev;

		}
		break;
		case ABL_CARD_FUTO:
		{
			bonus = plev;

			if (card_num > 1) bonus += plev * (card_num - 1) / 10;
			if (card_num == 9) bonus = plev * 2;

		}
		break;
		case ABL_CARD_FEAST:
		{
			bonus = plev * 2;

			if (card_num > 1) bonus += plev * (card_num - 1) / 5;
			if (card_num == 9) bonus = plev * 4;

		}
		break;

		case ABL_CARD_PHOENIX:
		{

			if (card_num == 9)
				bonus = plev * 8;
			else
				bonus = plev * 3 + (card_num - 1) * plev/2;
		}
		break;

		//vault出現率(%)と強敵出現率(get_mon_num())へのブースト
		case ABL_CARD_JYOON:
		case ABL_CARD_SHION:
		{
			bonus = 200;

			if (card_num > 1) bonus += (card_num - 1) * 30;
			if (card_num >= 9) bonus = 500;
		}
		break;

		default:
			msg_format(_("ERROR:calc_ability_card_mod_param()に想定外のカード(idx:%d)が渡された",
                        "ERROR: Unexpected card (idx: %d) passed to calc_ability_card_mod_param()"),card_idx);

	}

	return bonus;


}

//背中の扉などカードに関する計算値を返す
//当初防御確率だけだったが別に他のパラメータに使わん理由もない
//probとmod_paramに分けたんだからこの関数には確率値の計算だけを入れるべきだったがそうなっていないのは反省
int calc_ability_card_prob(int card_idx, int card_num)
{
	int result_param = 0;
	switch (card_idx)
	{
	case ABL_CARD_BACKDOOR:
	case ABL_CARD_DANMAKUGHOST:
	case ABL_CARD_BASS_DRUM:
	case ABL_CARD_PSYCHOKINESIS:

	{
		result_param = 25;

		if (card_num > 1) result_param += (card_num-1)*5;

		if (card_num >= 9) result_param = 70;

	}
	break;

	case ABL_CARD_UFO:
	{
		result_param = 10;
		if (card_num > 1) result_param += (card_num - 1) * 2;
		if (card_num >= 9) result_param = 30;

	}
	break;

	case ABL_CARD_KAGUYA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;
	case ABL_CARD_MAGATAMA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 8;
		if (card_num >= 9) result_param = 100;

	}
	break;
	case ABL_CARD_HANIWA:
	{
		result_param = 20;
		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;

	case ABL_CARD_JIZO:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_PATHE_SPELL:
		result_param = 30;
		if (card_num > 1) result_param += (card_num - 1) * 7;
		if (card_num >= 9) result_param = 100;

		break;

	case ABL_CARD_KAMIYAMA:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_EIKI:
		result_param = 100;
		if (card_num > 1) result_param -= (card_num - 1) * 5;
		if (card_num >= 9) result_param = 50;
		break;

	case ABL_CARD_PATHE_STUDY:
		result_param = 4 + card_num;
		if (card_num >= 9) result_param = 15;
		break;

	case ABL_CARD_SEIRAN:
		result_param = 20;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 75;

		break;

	case ABL_CARD_JELLYFISH:
		result_param = 30;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 80;
		break;

	case ABL_CARD_100TH_MARKET:
		result_param = 40;
		if (card_num > 1) result_param += (card_num - 1) * 5;
		if (card_num >= 9) result_param = 90;
		break;


	default:
		msg_format(_("ERROR:calc_ability_card_prob()に想定外のカード(idx:%d)が渡された",
                    "ERROR: Unexpected card (idx: %d) passed to calc_ability_card_prob()"), card_idx);

	}


	//↓テスト用に100%にする機能をつけていたが、場合によってはフリーズするので無くした。必要に応じて戻す
	//if (p_ptr->wizard && result_param < 100) result_param = 100;

	return result_param;

}



//新魔道具「アビリティカード」のカード発動/説明文生成
//カード選択や成功判定はすでに済んでいる
//カードをIで調べるときはonly_infoをTRUEにしてこれを呼び出すと説明文文字列へのアドレスが返る。
//only_infoでの呼び出しは発動不可能なカードでも呼ばれる。

cptr use_ability_card_aux(object_type *o_ptr, bool only_info)
{
	int dir;
	int card_num; //カードのスタックされた枚数　保有カードの効果に影響　10枚以上持ってても最大値は9
	int card_idx;
	int card_charge;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format(_("ERROR:use_ability_card_aux()にアビリティカード以外が渡された(Tval:%d)",
                    "ERROR: Non-ability card passed to use_ability_card_aux()"), o_ptr->tval);
		return NULL;
	}
	card_idx = o_ptr->pval;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:use_ability_card()に不正なカードidx(%d)を持ったアビリティカードが渡された",
                    "ERROR: Ability card with incorrect idx (%d) passed to use_ability_card()"), card_idx);
		return NULL;
	}

	card_num = count_ability_card(card_idx);

	if (!ability_card_list[card_idx].activate && !only_info)
	{
		msg_format(_("ERROR:発動不可能なカードが発動された(idx:%d)",
                    "ERROR: Unactivateble card was activated (idx: %d)"), card_idx);
		return NULL;
	}

	//card_charge = ability_card_list[o_ptr->pval].charge_turn;



	switch (card_idx)
	{

	case ABL_CARD_BLANK:
	{
		if (p_ptr->pclass != CLASS_CHIMATA)
		{
			if (only_info) return format(_(" このカードはなんの役にも立たない。",
                                        " This card doesn't look useful at all."));
            msg_print(_("使うと装備品が全て足元に落ちそうになった！慌てて装備し直した。",
                    "All of your equipment falls down at your feet! You rush and re-equip your items."));
        }
		else
		{
			if (only_info) return format(_(" このカードを使って新たなカードを作ることができる。対象のモンスターに隣接して使わなければならない。成功率はレベル差・知能・器用で決まる。睡眠・混乱・朦朧・恐怖・防御低下状態のモンスターには成功しやすい。この行為はモンスターを怒らせる。",
                                    " Using this card lets you create a new card. You must target an adjacent monster. Success rate depends on level difference, intelligence and dexterity. Works better on sleeping, confused, stunned, terrified monsters or monsters with lowered defense. Makes the monster hostile."));

			if (!chimata_use_blank_card()) return NULL;
		}
	}
	break;

	case ABL_CARD_LIFE:
	case ABL_CARD_FEAST:
	case ABL_CARD_PHOENIX:
	{
		int add_hp;

		add_hp = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(_(" このカードを所持していると最大HPが%d増加する。同じ効果のカードを複数種類所持している場合一つだけが有効。",
                                        " Carrying this card increases maximum HP by %d. If you have multiple kinds of cards with the same effect, only one of them applies."), add_hp);

	}
	break;
	case ABL_CARD_SPELL:
	case ABL_CARD_FUTO:
	{
		int add_sp;

		add_sp = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(_(" このカードを所持していると最大MPが%d増加する。MPの非常に低い一部の職には無効。同じ効果のカードを複数種類所持している場合一つだけが有効。",
                                        " Carrying this card increases maximum MP by %d. Not effective for several classes with extremely low MP. If you have multiple kinds of cards with the same effect, only one of them applies."), add_sp);

	}
	break;
	case ABL_CARD_NAZ:
	{

		if (only_info) return format(_(" このカードには何の使い道もないが交換価値が高い。",
                                        " This card doesn't have an use, but its exchange rate is high."));

	}
	break;
	case ABL_CARD_RINGO:
	{
		int v,base = 10;
		{

			if (only_info) return format(_(" このカードを発動すると%d+1d%dターン腕力・器用・耐久が3ポイント上昇する。通常の限界値を超えない。",
                                            " Activating this card increases strength, dexterity and constitution by 3 points for %d+1d%d turns. Cannot exceed ordinary limits."),base,base);

			v = base + randint1(base);

			set_tim_addstat(A_STR, 3, v, FALSE);
			set_tim_addstat(A_CON, 3, v, FALSE);
			set_tim_addstat(A_DEX, 3, v, FALSE);

		}
		break;

	}
	case ABL_CARD_ONMYOU1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(_(" このカードを発動すると%dダメージの破邪属性のボールを放つ。",
                                        " Activating this card fires a ball of holy energy (damage: %d)."), dam);

		if (!get_aim_dir(&dir)) return NULL;

		fire_ball(GF_HOLY_FIRE, dir, dam, 1);

	}
	break;
	case ABL_CARD_ONMYOU2:
	{
		int dam = 40 + plev + chr_adj;

		if (only_info) return format(_(" このカードを発動すると%dダメージの破邪属性のボルトを放つ。",
                                        " Activating this card fires a bolt of holy energy (damage: %d)."), dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_bolt(GF_HOLY_FIRE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(_(" このカードを発動すると%dダメージの閃光属性のビームを放つ。",
                                        " Activating this card fires a beam of light (damage: %d)."), dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_beam(GF_LITE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE2:
	{
		int dam = 65 + plev * 5 / 2 + chr_adj * 2;

		if (only_info) return format(_(" このカードを発動すると%dダメージのロケットを放つ。",
                                        " Activating this card fires a rocket (damage: %d)."), dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_rocket(GF_ROCKET, dir, dam, 2);

	}
	break;
	case ABL_CARD_KNIFE1:
	{
		//咲夜特技のページから丸コピーした。こういうのは行儀が悪いんだが
		int tx, ty;
		int quality = plev / 5;
		object_type forge;
		object_type *q_ptr = &forge;
		if (only_info) return format(_("このカードを発動すると1d3(+%d,+%d)のナイフを生成して投擲する。",
                                        " Activating this card creates and throws a Knife (1d3) (+%d, +%d)."), quality, quality);

		if (!get_aim_dir(&dir)) return NULL;
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		else
		{
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
		}
		object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_KNIFE));
		q_ptr->to_d = quality;
		q_ptr->to_h = quality;
		q_ptr->discount = 99;
		object_known(q_ptr);
		msg_print(_("あなたはナイフを投げた。", "You throw a knife."));
		do_cmd_throw_aux2(py, px, ty, tx, 1, q_ptr, 0);

		break;
	}
	break;

	case ABL_CARD_KNIFE2:
	{
		int dam = 20 + plev * 2 + chr_adj;

		int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_OPT;

		if (only_info) return format(_(" このカードを発動すると%dダメージの破片属性ボルトを放つ。このボルトが壁に当たるとそこから一度だけランダムな敵のほうへ飛ぶ。",
                                        " Activating this card fires a bolt of shards (damage: %d). If this bolt hits a wall, it ricochets towards a random enemy (but only once)."), dam);

		if (!get_aim_dir(&dir)) return NULL;

		project_hook(GF_SHARDS, dir, dam, flg);

	}
	break;

	case ABL_CARD_SANAE1:
	{
		int base = 3;
		if (only_info) return format(_(" このカードを発動すると%d+1d%dターン後にダンジョンから帰還できる。地上からダンジョンに戻るときには使えない。",
                                        " Activating this card recalls you from the dungeon to surface in %d+1d%d turns. Cannot be used to return from surface to dungeon."),base,base);

		if (!dun_level)
		{
			msg_print(_("地上では使えない。", "You cannot use it on surface."));
			return NULL;
		}

		if (EXTRA_MODE)
		{
			msg_print(_("今は使えない。", "You cannot use it now."));
			return NULL;

		}

		recall_player(base+randint1(base));

	}
	break;

	case ABL_CARD_EIKI:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info) return format(_(" このカードを所持していると、麻痺や意識不明になったときに即座に復帰できる可能性がある。ただしその時には有り金の%d%%を奪い取られる。",
                                        " If you are carrying this card, you can immediately recovers after getting paralyzed or knocked unconscious. However, %d%% of your money is taken away in this case."),prob );

	}
	break;

	case ABL_CARD_SANAE2:
	{

		int base = 20;

		if (only_info)	return format(_(" このカードを発動すると%d+1d%dターン毒に対する一時的な耐性を得る。",
                                        " Activating this card grants temporary resistance to poison for %d+1d%d turns."), base, base);

		set_oppose_pois(base + randint1(base), FALSE);

	}
	break;


	case ABL_CARD_ICE_FAIRY:
	{
		int dam = 10 + plev * 3 + chr_adj * 3;

		if (only_info) return format(_(" このカードを発動すると隣接%dダメージの巨大な極寒属性のボールを発生させる。",
                                        " Activating this card generates a huge ball of ice centered on you (damage: %d if adjacent)."), dam );

		project(0, 6, py, px, dam*2, GF_ICE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);


	}
	break;



	case ABL_CARD_TANUKI:
	{
		int power = plev + p_ptr->stat_ind[A_CHR]+3;

		if (only_info) return format(_(" このカードを発動すると周囲の情報を知ることができる。レベルと魅力が高いほど多くの情報が集まる。",
                                        " Activating this card give you information about nearby area. The higher your level and charisma, the more information you gather."));
		msg_print(_("子狸たちが奔走して情報を集めてきた！", "Small tanuki scatter about to gather information!"));

		map_area(DETECT_RAD_DEFAULT);
		if (power>30) detect_monsters_normal(DETECT_RAD_DEFAULT);
		if (power>40) detect_monsters_invis(DETECT_RAD_DEFAULT);
		if (power>50) detect_objects_gold(DETECT_RAD_DEFAULT);
		if (power>60) detect_objects_normal(DETECT_RAD_DEFAULT);
		if (power>70) detect_traps(DETECT_RAD_DEFAULT,TRUE);
		if (power>80) p_ptr->feeling_turn = 0;
	}
	break;



	case ABL_CARD_QUARTER_GHOST:
	case ABL_CARD_SPARE_GHOST:
	{
		int dice = card_num+1;
		int sides = 9;
		if (card_num >= 9) sides=10;

		if (only_info) return format(_(" このカードを所持していると敵の隣接攻撃に対して地獄属性で%dd%dダメージの反撃を行う。",
                                        " If you are carrying this card, you counter enemy melee attacks with %dd%d nether damage."), dice,sides);
	}
	break;


	case ABL_CARD_BACKDOOR:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(_(" このカードを所持していると敵のボルト・ビーム攻撃を%d%%の確率で無効化する。",
                                        " Carrying this card lets you negate enemy bolt/beam attacks with %d%% chance."), prob);
	}
	break;
	case ABL_CARD_DANMAKUGHOST:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(_(" このカードを所持していると敵のボール攻撃を%d%%の確率で無効化する。",
                                        " Carrying this card lets you negate enemy ball attacks with %d%% chance."), prob);
	}
	break;
	case ABL_CARD_BASS_DRUM:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(_(" このカードを所持していると敵のロケット攻撃を%d%%の確率で無効化する。",
                                        " Carrying this card lets you negate enemy rockets with %d%% chance."), prob);
	}
	break;
	case ABL_CARD_PSYCHOKINESIS:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(_(" このカードを所持していると敵のブレス攻撃を%d%%の確率で無効化する。",
                                        " Carrying this card lets you negate enemy breath attacks with %d%% chance."), prob);
	}
	break;
	case ABL_CARD_UFO:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(_(" このカードを所持していると敵の投射系攻撃魔法やブレスなどを%d%%の確率で無効化する。",
                                        " Carrying this card lets you negate enemy projectiles, offensive magic and breath attacks with %d%% chance."), prob);
	}
	break;


	case ABL_CARD_MANEKINEKO:
	{
		if (only_info)return format(_(" このカードを所持していると店の商品が入れ替わりやすくなり、商品の質が少し良くなる。",
                                        " Carrying this card makes shops change their inventory more often, and the quality of wares slightly increases."));
	}
	break;

	case ABL_CARD_YAMAWARO:
	{
		if (only_info)return format(_(" このカードを所持していると店に新たに並ぶ商品が値引きされやすくなる。",
                                        " Carrying this card makes shops give discounts on their wares more often."));
	}
	break;

	case ABL_CARD_SYUSENDO:
	{
		if (only_info)
		{
			if(card_num == 1)
				return format(_(" このカードを所持していると泥棒に物を盗まれなくなる。",
                                " Carrying this card prevents thieves from stealing from you."));
			else if (card_num < 9)
				return format(_(" このカードを所持していると泥棒に物を盗まれなくなる。さらに泥棒が逃げ出すことを%d%%の確率で妨害する。",
                                " Carrying this card prevents thieves from stealing from you. Also has %d%% chance of stopping them from running away."), (card_num-1) * 10);
			else
				return format(_(" このカードを所持していると泥棒に物を盗まれなくなる。さらに泥棒が逃げ出すことを妨害する。",
                                " Carrying this card prevents thieves from stealing from you. Also prevents them from running away."));
		}
	}
	break;

	case ABL_CARD_HANIWA:
	{
		int sav_bonus = calc_ability_card_prob(card_idx, card_num);
		if (only_info)
			return format(_(" このカードを所持していると魔法防御能力が%d上昇する。",
                            " Carrying this card increases saving throw by %d."),sav_bonus);

	}
	break;

	case ABL_CARD_MAGATAMA:
	{
		int ac_bonus = calc_ability_card_prob(card_idx, card_num);

		if (only_info)
			return format(_(" このカードを所持しているとACが%d上昇する。",
                            " Carrying this card increases AC by %d."),ac_bonus);
	}
	break;

	case ABL_CARD_YACHIE:
	{
		int percen=10;

		if(card_num>1)percen += (card_num-1) * 2;
		if (card_num >= 9) percen = 30;

		if (only_info)
			return format(_(" このカードを所持していると敵を倒したとき得られる金やアイテムが%d%%増加する。金やアイテムを落とさない敵には効果がない。",
                            " Carrying this card increases money and items you receive upon defeating enemies by %d%%. No effect on enemies that don't drop money or items."),percen);

	}
	break;

	case ABL_CARD_SYANHAI:
	{

		if (only_info)	return format(_(" このカードを発動すると「人形」を一体配下として召喚する。",
                                        " Activating this card summons a doll as your follower."));

		if (summon_named_creature(0, py, px, MON_ALICE_DOLL, PM_FORCE_PET))
			msg_print(_("人形を放った。", "You throw out a doll."));

	}
	break;

	case ABL_CARD_LILYWHITE:
	{
		bool flag = FALSE;
		if (only_info)	return format(_(" このカードを発動すると敵対的な『リリーホワイト』を召喚する。リリーホワイトは呼ぶたびに強くなる。",
                                        " Activating this card summons a hostile Lily White. She becomes stronger every time you summon her."));

		flag_generate_lilywhite = o_ptr->xtra3+1;
		if (summon_named_creature(0, py, px, MON_RANDOM_UNIQUE_2, (PM_FORCE_ENEMY|PM_NO_ENERGY))) flag=TRUE;
		flag_generate_lilywhite = 0;

		if (flag)
		{
			msg_print(_("リリーホワイトが襲ってきた！", "Lily White attacks you!"));
			if (o_ptr->xtra3 < 99) o_ptr->xtra3++;
		}
		else
		{
			msg_print(_("何も起こらなかった。", "Nothing happens."));
		}

	}
	break;

	case ABL_CARD_EIRIN_DRUG:
	{
		int base = 4;

		if (only_info)	return format(_(" このカードを発動すると%d+1d%dターン無敵化する。MP200を消費する。",
                                        " Activating this card makes you invulnerable for %d+1d%d turns. Costs 200 MP."),base,base);

		if (p_ptr->csp < 200)
		{
			msg_print(_("MPが足りない。", "You don't have enough MP."));
			return NULL;
		}
		p_ptr->csp -= 200;
		p_ptr->redraw |= PR_MANA;

		set_invuln(base + randint1(base), FALSE);


	}
	break;
	case ABL_CARD_INABA_LEG:
	{
		int prob = 25;

		if (card_num > 1) prob += (card_num - 1) * 8;
		if (card_num >= 9) prob = 100;

		if (only_info)	return format(_(" このカードを所持しているとザックの中のアイテムが%d%%の確率で属性攻撃による破壊から守られる。",
                                        " If you are carrying this card, there is a %d%% chance to protect items in your pack from being destroyed by elemental attacks."), prob);

	}
	break;

	case ABL_CARD_SAKI:
	{
		int bonus = 10;
		if (card_num > 1) bonus += (card_num - 1) * 2;
		if (card_num >= 9) bonus = 32;

		if (only_info)	return format(_(" このカードを所持していると隣接攻撃に(+%d,+%d)のボーナスを得る。",
                                        " Carrying this card gives you (+%d, +%d) in melee."), bonus,bonus);


	}
	break;

	case ABL_CARD_KOISHI:
	{
		int base = 20;

		if (only_info)	return format(_(" このカードを発動すると%d+1d%dターン敵に認識されづらくなる。",
                                        " Activating this card makes it harder for enemies to notice you for %d+1d%d turns."), base, base);

		set_tim_superstealth(base + randint1(20), FALSE, SUPERSTEALTH_TYPE_NORMAL);

	}
	break;

	case ABL_CARD_KISERU:
	{
		int base = 15;
		int v;

		if (only_info)	return format(_(" このカードを発動すると%d+1d%dターン士気高揚、加速、狂戦士化する。",
                                        " Activating this card grants you heroism, increased speed and berserk rage for %d+1d%d turns."), base, base);

		v = base + randint1(base);
		set_hero(v, FALSE);
		set_fast(v, FALSE);
		set_shero(v, FALSE);
	}
	break;


	case ABL_CARD_JIZO:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(_(" このカードを所持しているとHPの自然回復量が%d%%増加する。急回復に累積して効果が出る。すでに同様の能力を持っている場合は効果がない。",
                                        " Carrying this card increases your natural HP regeneration by %d%%. Stacks with regeneration. Has no effect if you already have similar abilities."), percen);
	}

		break;

	case ABL_CARD_PATHE_SPELL:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(_(" このカードを所持しているとMPの自然回復量が%d%%増加する。急回復に累積して効果が出る。すでに同様の能力を持っている場合は効果がない。",
                                        " Carrying this card increases your natural MP regeneration by %d%%. Stacks with regeneration. Has no effect if you already have similar abilities."), percen);
	}
		break;


	case ABL_CARD_AKAGAERU:
	{
		int mult= 75;
		int rad = 1;

		if (card_num > 1) mult += (card_num - 1) * 7;
		if (card_num >= 9)
		{
			rad = 2;
			mult = 150;
		}
		if (only_info) return format(_("このカードを所持しているとプレイヤーが使用したボルト系攻撃が半径%dのロケットに変化する。威力は%d%%になる。",
                                        "If you are carrying this card, bolt attacks you use turn into rockets with radius %d. Their power becomes %d%%."),rad,mult);
	}
	break;

	case ABL_CARD_TENGU_GETA:
	{
		int len = 3 + plev / 7;

		if (only_info) return format(_("このカードを発動すると%dグリッドを一瞬で移動し、敵に当たったらそのまま通常攻撃を行う。",
                                        "Activating this card moves you %d spaces. If you hit an enemy, you attack them."), len);
		if (!rush_attack(NULL, len, 0)) return NULL;
	}
	break;

	case ABL_CARD_MUKADE:
	{
		int border_lis[10] = { 100,95,92,89,86,86,83,80,77,75 };
		int add_lis[10]		= { 1,1,1,1,1,2,2,2,2,3 };

		if (only_info) return format(_("このカードを所持していると、HPが最大値の%d%%以上のときに限り隣接攻撃回数が%d増える。",
                                        "As long as your HP is at least %d%% of its maximum value, carrying this card increases your amount of melee blows per round by %d."), border_lis[card_num], add_lis[card_num]);

	}
	break;

	case ABL_CARD_SAKI_2:
	{
		int border_lis[10] = { 50,55,58,61,64,64,67,70,73,75 };
		int add_lis[10] = { 1,1,1,1,1,2,2,2,2,3 };

		if (only_info) return format(_("このカードを所持していると、HPが最大値の%d%%未満のときに限り隣接攻撃回数が%d増える。",
                                        "As long as your HP is at %d%% of its maximum value or lower, carrying this card increases your amount of melee blows per round by %d."), border_lis[card_num], add_lis[card_num]);


	}
	break;

	case ABL_CARD_SUKIMA:
	{

		if (only_info) return format(_("このカードを発動すると隣接した壁などの向こうへ瞬間移動する。マップ端に到達すると反対側へ出る。通常のテレポートで移動できない場所には出られない。",
                                        "Activating this card lets you instantly moves to the opposite side of adjacent wall. If you hit a map edge, you come out on the opposite side of map. Cannot exit in a location normally inaccesible through teleportation."));

		if (!wall_through_telepo(0)) return NULL;

	}
	break;

	case ABL_CARD_KODUCHI:
	{

		if (only_info) return format(_("このカードを発動するとアイテムをひとつ選択して1/3の価値の＄に変えることができる。",
                                        "Activating this card turns a chosen item into gold equal to 1/3 of its value."));

		alchemy(0);
	}
	break;

	case ABL_CARD_KEIKI:
	{

		if (only_info) return format(_("このカードを発動すると鉱石・宝石系アイテムをひとつ選択して3倍の価値の＄に変えることができる。",
                                        "Activating this card turns a chosen ore/gemstone item into gold equal to three times its value."));

		alchemy(1);
	}
	break;


	case ABL_CARD_KANAMEISHI:
	{

		int y, x;
		monster_type *m_ptr;
		int damage = plev * 3;

		if (only_info) return format(_("このカードを発動すると視界内の床一箇所を「岩石」地形に変える。指定位置にモンスターがいた場合%dのダメージを与える。",
                                        "Activating this card changes terrain in one location in sight to rubble. Deals %d damage if a monster is present there."),damage);


		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                        "You must pick a target in sight."));
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx)
		{
			if (!cave_naked_bold(y, x))
			{
				msg_print(_("そこには何かがあるようだ。やめておこう。", "Something is here. Better not do it."));
				return NULL;
			}
			msg_print(_("地面に石塊を積み重ねた。", "Stones pile up on the floor."));
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format(_("%sの頭上に要石が落ちてきた！", "A keystone hits %s on the head!"), m_name);
			project(0, 0, y, x, damage, GF_ARROW, PROJECT_KILL, -1);
		}

		break;



	}
	break;

	case ABL_CARD_LUNATIC:
	{
		int base = 100 + plev * 6;
		int sides = 100 + chr_adj * 10;

		if (only_info) return format(_("このカードを発動すると指定したグリッドに%d+1d%dダメージの隕石属性のボールで攻撃する。",
                                        "Activating this card fires a meteor at target location (damage: %d+1d%d)."), base,sides);

		if (!get_aim_dir(&dir)) return NULL;
		if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 3, NULL)) return NULL;

	}
	break;

	case ABL_CARD_MIKO:
	{
		int base = 50 + plev + chr_adj * 5;
		int time = 15 + randint1(15);
		if (only_info) return format(_("このカードを発動すると視界内すべてに対し%dダメージの閃光属性攻撃を行う。さらに一時的に対邪悪結界と聖なるオーラを得る。",
                                        "Activating this card hits everything in sight with light (damage: %d), and grants temporary protection from evil and holy aura."), base);

		project_hack2(GF_LITE, 0, 0, base);
		set_protevil(time, FALSE);
		set_tim_sh_holy(time, FALSE);

	}
	break;

	case ABL_CARD_VAMPIRE_FANG:
	{
		int dice = plev+10;
		int sides = 10 + chr_adj / 5;
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format(_("このカードを発動すると隣接したモンスター一体に%dd%dの無属性ダメージを与え、さらに耐性を無視して朦朧とさせる。",
                                        "Activating this card deals %dd%d non-elemental damage to an adjacent monster and stuns them, ignoring resistance."), dice,sides);

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_format(_("そこには何もいない。", "There's nobody here."));
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			msg_format(_("%sへ牙を突き立てた！", "%s is stabbed by the fang!"), m_name);

			project(0, 0, y, x, damroll(dice, sides), GF_MISSILE, PROJECT_KILL | PROJECT_JUMP, -1);
			if (m_ptr->r_idx)
			{
				msg_format(_("%sはフラフラになった。", "%s looks dizzy."), m_name);
				(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + 4 + randint0(4));
			}
		}

	}
	break;

	case ABL_CARD_SUBTERRANEAN:
	{
		int dam = 500 + plev * 10 + chr_adj * 20;

		if (only_info) return format(_("このカードを発動すると視界内すべてに対し%dダメージの核熱属性攻撃を行う。",
                                        "Activating this card hits everything in sight with nuclear energy (damage: %d)."), dam);

		project_hack2(GF_NUKE, 0, 0, dam);

	}
	break;

	case ABL_CARD_MUGIMESHI:
	{
		int heal = 400;

		if (only_info) return format(_("このカードを発動するとHPを%d回復し、毒と切り傷を治療し、満腹になり、低下している肉体能力を復活させる。",
                                        "Activating this card heals %d HP, cures poison and wounds, satisfies hunger and recovers lowered physical stats."),heal);

		hp_player(400);
		set_poisoned(0);
		set_cut(0);
		set_food(PY_FOOD_MAX - 1);
		do_res_stat(A_STR);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);

	}
	break;

	case ABL_CARD_KAMIYAMA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format(_("このカードを持っていると、特定の固定アーティファクトを持つモンスターを倒したときにそのアーティファクトを落とす確率が%d.%02d倍になる。",
                                        "Carrying this card makes monsters that can drop specific artifacts drop them %d.%02d times as often."),(prob/100+1),(prob%100));

	}
	break;

	case ABL_CARD_TSUKASA:
	{

		if (only_info) return format(_("このカードを発動すると最大HPの1/3のHPを消費して同量のMPを回復する。一時的に腕力・器用・耐久が低下することがある。",
                                        "Activating this card reduces your HP by 1/3 of its maximum value and recovers the same amount of MP. Might lower strength, dexterity and constitution."));

		if (p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			msg_print(_("あなたはこのカードを使えない。", "You cannot use this card."));
			return NULL;
		}

		if (p_ptr->chp < p_ptr->mhp/3)
		{
			msg_print(_("カードを使えない。体力が少なすぎる！", "You cannot use this card. You don't have enough HP!"));
			return NULL;
		}
		take_hit(DAMAGE_USELIFE, p_ptr->mhp / 3, _("管狐のカード", "kuda-gitsune card"), -1);
		player_gain_mana(p_ptr->mhp/3);

		if (one_in_(2)) do_dec_stat(A_STR);
		if (one_in_(2)) do_dec_stat(A_DEX);
		if (one_in_(2)) do_dec_stat(A_CON);

	}
	break;

	case ABL_CARD_KYOUTEN:
	{
		int percen = CALC_ABL_KYOUTEN_RECHARGE_BONUS(card_num);


		if (only_info) return format(_("このカードを所持していると魔道具の自然充填が%d%%早くなる。",
                                        "Carrying this card speeds up magic item recharge rate by %d%%."),percen);
	}
	break;

	case ABL_CARD_KAGUYA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format(_("このカードを所持していると、敵の攻撃でダメージを受けたときその%d%%のMPを回復する。またMP減少打撃に対して%d%%の耐性を得る。",
                                        "If you are carrying this card, receiving damage from enemy attacks recovers MP by %d%% of that damage. Also give %d%% chance to resist mana draining attacks."),prob,prob*2);


	}
	break;

	case ABL_CARD_PATHE_STUDY:
	{

		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info)
			return format(_("このカードを所持していると魔法の失敗率を%d%%低下させる。最低失敗率があるときはそれを%d%%低下させる。",
                            "If you are carrying this card, you spellcasting failure rate is reduced by %d%%, and your minimum failure rate is reduced by %d%% if applicable."), prob, prob / 3);

	}
	break;

	case ABL_CARD_SUNNY:
	case ABL_CARD_LUNAR:
	case ABL_CARD_STAR:
	{
		int dam = 40 + plev * 2 + chr_adj * 2;
		int typ;
		int tx, ty;

		if (only_info)
		{
			if(card_idx == ABL_CARD_SUNNY)
				return format(_(" このカードを発動すると%dダメージの閃光属性のビームを放つ。ただし斜め方向にしか撃てない。",
                                " Activating this card fires a light beam (dam: %d). Can only be fired diagonally."), dam);
			else if (card_idx == ABL_CARD_LUNAR)
				return format(_(" このカードを発動すると%dダメージの暗黒属性のビームを放つ。ただし上下方向にしか撃てない。",
                                " Activating this card fires a darkness beam (dam: %d). Can only be fired vertically."), dam);
			else
				return format(_(" このカードを発動すると%dダメージの隕石属性のビームを放つ。ただし左右方向にしか撃てない。",
                                " Activating this card fires a meteor beam (dam: %d). Can only be fired horizontally."), dam);
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;


		if (card_idx == ABL_CARD_SUNNY)
		{
			typ = GF_LITE;
			if (dir != 1 && dir != 3 && dir != 7 && dir != 9)
			{
				msg_print(_("その方向には撃てない。", "You can't fire in this direction."));
				return NULL;
			}
		}
		else if (card_idx == ABL_CARD_LUNAR)
		{
			typ = GF_DARK;
			if (dir != 2 && dir != 8)
			{
				msg_print(_("その方向には撃てない。", "You can't fire in this direction."));
				return NULL;
			}
		}
		else
		{
			typ = GF_METEOR;
			if (dir != 4 && dir != 6)
			{
				msg_print(_("その方向には撃てない。", "You can't fire in this direction."));
				return NULL;
			}
		}

		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		msg_print(_("ビームを放った！", "You shoot a beam!"));
		project(0, 0, ty, tx, dam, typ, (PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;
	case ABL_CARD_AUNN:
	{

		//明鏡止水と同じ
		int gain_mana = 3 + plev / 20;

		if (only_info) return format(_("このカードを発動するとMPを僅かに回復する。", "Activating this card recovers a small amount of MP."));

		msg_print(_("あなたは呼吸を整えた...", "You take a deep breath..."));

		if(player_gain_mana(gain_mana))
			msg_print(_("少し頭がハッキリした。", "Your head clears up a bit."));

	}
	break;

	case ABL_CARD_FLAN:
	{
		int base = 12, sides = 4;

		if (only_info) return format(_("このカードを発動すると周辺%d+1d%dグリッドを*破壊*する。",
                                        "Activating this card causes *destruction* (rad: %d+1d%d)"),base,sides);

		destroy_area(py, px, base + randint1(sides), FALSE, FALSE, FALSE);

	}
	break;
	case ABL_CARD_LARVA:
	{
		int dam = 50 + plev + chr_adj * 2;

		if (only_info) return format(_(" このカードを発動すると周囲のモンスターを最大効力%dで混乱・攻撃力低下させる。",
                                        " Activating this card confuses and lowers attack power of nearby monsters (power: up to %d)"), dam);

		msg_print(_("鱗粉を振りまいた！", "You scatter scales around!"));
		project(0, 6, py, px, dam * 2, GF_OLD_CONF, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
		project(0, 6, py, px, dam * 2, GF_DEC_ATK, PROJECT_HIDE | PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
	}
	break;

	case ABL_CARD_URUMI:
	{
		int dam = plev / 2 + 5;

		if (only_info) return format(_(" このカードを発動すると周囲のモンスターの移動を短時間妨害する。",
                                        " Activating this card prevents nearby monsters from moving for a short period of time."), dam);

		msg_print(_("濡れた蛇のようなものが辺りに絡みついた...", "Something like a drenched snake entangles nearby area..."));
		project(0, 4, py, px, dam * 2, GF_NO_MOVE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);
	}
	break;

	case ABL_CARD_JYOON:
	{
		int bonus = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(_(" このカードを所持しているとvaultなどの特殊地形が%d.%02d倍出やすくなる。",
                                        " Carrying this card makes you %d.%02d times as likely to find vaults or other special dungeon features."), bonus/100,bonus%100);
	}
	break;

	case ABL_CARD_SHION:
	{
		int bonus = calc_ability_card_mod_param(card_idx, card_num);

		if (only_info) return format(_(" このカードを所持しているとフロアより高いレベルのモンスターが%d.%02d倍出にくくなる。",
                                        " Carrying this card makes it %d.%02d times as less likely to encounter out-of-depth monsters."), bonus / 100, bonus % 100);
	}
	break;

	case ABL_CARD_DOREMY:
	{
		if (only_info) return format(_(" このカードを発動するとフロアを一瞬で再構成する。地上やクエストダンジョンでは効果がない。",
                                        " Activating this card instantly recreates the current level. Cannot be used on surface or in quests."));
		if (p_ptr->inside_arena)
		{
			msg_print(_("ここでは使えない。", "You can't use it here."));
			return NULL;
		}
		//1ターン後時現実変容
		msg_print(_("現実の感覚が曖昧になっていく...", "Your sense of reality becomes blurred..."));
		p_ptr->alter_reality = 1;
		p_ptr->redraw |= (PR_STATUS);

	}
	break;

	case ABL_CARD_LIFE_EXPLODE:
	{
		if (only_info) return format(_(" このカードを発動すると敵からの攻撃を一度だけ無効化するようになる。そのときHP/2の威力の気属性の大爆発を起こし周囲の敵にダメージを与える。",
                                        " Activating this card negates a single attack from an enemy. It then creates a large Ki explosion with power equal to half of your HP, dealing damage to nearby enemies."));

		msg_print(_("体から生命力が迸る気がする！", "You feel lifeforce gushing forth from your body!"));
		p_ptr->special_defense |= SD_LIFE_EXPLODE;
		p_ptr->redraw |= (PR_STATUS);

	}
	break;

	case ABL_CARD_JUNKO:
	{
		int power = plev * 8;
		if (only_info) return format(_(" このカードを発動すると広範囲の敵に対しパワー%dのフロア追放を試みる。",
                                        " Activating this card attempts to banish nearby enemies from the level. (power: %d)"),power);

		msg_print(_("あなたは圧倒的な威圧感を醸し出した！", "You exude an overwhelmingly intimidating aura!"));
		mass_genocide_2(power, 40, 2);
	}
	break;

	case ABL_CARD_LUNATIC_TORCH:
	{
		if (only_info) return format(_(" このカードを発動すると連続で魔法を詠唱することができる。ただし使用後にHPを100消費する。",
                                        " Activating this card lets you cast two spells in a row, but it drains 100 HP."));

		if (!cp_ptr->realm_aptitude[0])
		{
			msg_print(_("あなたは呪文を使えない。", "You can't cast spells."));
			return NULL;
		}

		if (!can_do_cmd_cast()) return NULL;
		do_cmd_cast();
		handle_stuff();
		if (can_do_cmd_cast()) do_cmd_cast();

		msg_print(_("連続詠唱の反動が体を襲った！", "Your body receives backlash from consecutive spellcasting!"));
		if(!p_ptr->is_dead) take_hit(DAMAGE_USELIFE, 100, _("命を焚く松明", "Life-Burning Torch"), -1);

	}
	break;

	case ABL_CARD_SEIRAN:
	{

		int prob = calc_ability_card_prob(card_idx, card_num);
		int dam = plev*2;

		if (only_info) return format(_(" このカードを所持していると、戦闘中にターゲットにしたモンスターに対し%d%%の確率でロケットによる%dダメージの追撃が行われる。",
                                        " Carrying this card gives you a %d%% chance of dealing extra %d damage with a rocket to targeted monsters."), prob,dam);
	}
	break;


	case ABL_CARD_TEWI2:
	{
		int x, y;
		if (only_info) return format(_(" このカードを発動するとターゲットの足元にあるトラップを発動させる。トラップがなければランダムなトラップを生成して発動を試みる。プレイヤーもトラップの影響を受ける。",
                                        " Activating this card triggers a trap at target location. If there's no trap there, it randomly generates one and triggers it. You can get affected by that trap as well."));

		if (!get_aim_dir(&dir)) return NULL;

		x = target_col;
		y = target_row;

		if (!projectable(y, x, py, px))
		{
			msg_print(_("視界内の一点を指定しないといけない。", "You have to pick a location in sight."));
			return NULL;
		}

		//罠がなければ作る
		if (!is_trap(cave[y][x].feat))
		{
				place_trap(y, x);
		}

		project(0, 0, y, x, 0, GF_ACTIV_TRAP, (PROJECT_GRID | PROJECT_JUMP | PROJECT_HIDE), -1);

	}
	break;



	case ABL_CARD_KOMACHI:
	{
		int x, y;
		int dist = 2 + plev / 24;

		if (only_info) return format(_(" このカードを発動すると距離%dグリッド以内の床に一瞬で移動できる。ドアを経由すると少し距離が狭くなる。地形が不明な場所には行けず、壁をすり抜けることはできない。",
                                        " Activating this card lets you instantenously move to a location within %d grids. Passing through doors slightly reduces the distance. You cannot move to an unknown location or go through walls."),dist);

		if (!teleport_walk(dist)) return NULL;

	}
	break;

	case ABL_CARD_WAKASAGI:
	{
		int base = 5;

		if (only_info) return format(_(" このカードを発動すると%d+1d%dターンの間モンスターを起こしにくくなる。",
                                        " Activating this card makes monsters less likely to wake up for %d+1d%d turns."), base,base);

		set_tim_stealth(randint1(base) + base, FALSE);


	}
	break;

	case ABL_CARD_NEMUNO:
	{
		int base = 100 + plev * 4;
		int sides = chr_adj * 4;

		if (only_info) return format(_(" このカードを発動すると%d+1d%dダメージの射撃属性のボルトを放つ。このボルトは反射されない。",
                                        " Activating this card fires a non-elemental bolt that can't be reflected. (dam: %d+1d%d)"), base,sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print(_("包丁をぶん投げた！", "You throw a knife!"));
		fire_bolt(GF_ARROW, dir, base+randint1(sides));

	}
	break;

	case ABL_CARD_SUIKA:
	{
		int base = 20;
		int v;
		int percen;
		if (only_info) return format(_(" このカードを発動すると泥酔状態になり、さらに%d+1d%dターン腕力と耐久力が5ポイント上昇する。この上昇は通常の限界を超える。",
                                        " Activating this card makes you drunk and increases your strength and constitution by 5 points (exceeding normal limits) for %d+1d%d turns."), base, base);

		if (p_ptr->pclass == CLASS_BYAKUREN)
		{
			msg_print(_("あなたは戒律により酒を飲めない。",
                        "Your religion prohibits you from consuming alcohol."));
			return NULL;
		}
		msg_print(_("あなたは鬼の酒を一息に呷った！", "You down the oni sake in a single gulp!"));
		percen = p_ptr->chp * 100 / p_ptr->mhp;
		v = base + randint1(base);
		if(p_ptr->alcohol < DRANK_3-1) set_alcohol(DRANK_3-1);

		set_tim_addstat(A_STR, 105, v, FALSE);
		set_tim_addstat(A_CON, 105, v, FALSE);
		p_ptr->chp = p_ptr->mhp * percen / 100;
		p_ptr->redraw |= PR_HP;
		redraw_stuff();

	}
	break;

	case ABL_CARD_HOUTOU:
	{
		int dam = plev * 6 + chr_adj * 6;

		if (only_info) return format(_(" このカードを発動すると%dダメージの無属性の強力なレーザーを放つ。このレーザーは岩地形を宝石に変える。",
                                        " Activating this card fires a powerful non-elemental laser (dam: %d). It changes rocks into gemstones."), dam);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print(_("宝塔が輝いた！", "The Jeweled Pagoda shines!"));
		fire_spark(GF_HOUTOU, dir, dam, 2);

	}
	break;

	case ABL_CARD_KANAME_MISSILE:
	{

		int dam = plev * 7 + chr_adj * 5;

		if (only_info) return format(_(" このカードを発動すると%dダメージの隕石属性のロケットを放つ。モンスターに命中しなかった場合は着弾地点を岩地形にする。",
                                        " Activating this card fires a meteor rocket (dam: %d). If it doesn't hit a monster, it turns the terrain into rubble."), dam);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print(_("要石が飛んでいった！", "A keystone flies forth!"));
		fire_rocket(GF_KANAMEISHI, dir, dam, 1);

	}
	break;

	case ABL_CARD_ROKURO_HEAD:
	{

		int wait = 30;

		if (only_info) return format(_(" このカードを発動すると%dターン後にフロアのアイテムをひとつ足元に持ってくる。遠くのアイテムや特殊な地形に置かれたアイテムは持ってこれない。",
                                        " Activating this card fetches a random item on this level after %d turns. Items that are far away or located in special terrain can't be retrieved in this manner."), wait);

		rokuro_head_search_item(wait, FALSE);

	}
	break;


	case ABL_CARD_KUTAKA:
	{
		int tx, ty;

		if (only_info)
				return format(_(" このカードを発動すると非常に長距離のトンネル生成ビームを放つ。ただし8方向一直線にしか撃てない。",
                                " Activating this card fires an extremely long digging beam. Can only be fired in one of eight cardinal directions."));

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;


		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		msg_print(_("あなたは前方に道を拓いた！", "Road gets cleared in front of you!"));
		project(0, 0, ty, tx, 0, GF_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI | PROJECT_LONG_RANGE), -1);

	}
	break;

	case ABL_CARD_JELLYFISH:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format(_(" このカードを所持していると敵のボルト攻撃を%d%%の確率で無効化する。",
                                        " Carrying this card gives you %d%% chance of negating enemy bolt attacks."),prob);
	}
	break;


	case ABL_CARD_YUNOMI_REIMU:
	{
		int power = 100 + plev * 3 + chr_adj * 5;

		if (only_info) return format(_(" このカードを発動するとパワー%dの全能力低下属性のボールを放つ。",
                                        " Activating this card fires a ball that reduces all abilities. (power: %d)"), power);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print(_("巨大な座布団をぶん投げた！", "You throw a giant seat cushion!"));
		fire_ball(GF_DEC_ALL, dir, power, 1);
	}
	break;

	case ABL_CARD_YUNOMI_MARISA:
	{
		int power = 100 + plev * 3 + chr_adj * 5;

		if (only_info) return format(_(" このカードを発動するとパワー%dの減速属性のボールを放つ。",
                                        " Activating this card fires a ball that reduces speed. (power: %d)"), power);

		if (!get_aim_dir(&dir)) return NULL;
		msg_print(_("巨大な星型ぬいぐるみを押し付けた！", "You throw a giant star plushie!"));
		fire_ball(GF_OLD_SLOW, dir, power, 1);
	}
	break;



	case ABL_CARD_100TH_MARKET:
	{

		if (only_info) return format(_(" このカードを6:00もしくは18:00に所持しているとカード取引所で未所持のカードが出やすくなる。またこのカードは交換価値が非常に高い。",
                                        " Carrying this card at 6:00 or 18:00 makes it more likely to find new cards at card exchange. Also, this card's exchange rate is extremely high."));

	}
	break;

	case ABL_CARD_HIRARI:
	{
		int base = 10;

		if (only_info) return format(_(" このカードを発動すると%dターン身を隠して敵の攻撃のターゲットから外れる。他を狙った攻撃に巻き込まれることはある。またその場に留まる、休憩、飲食、装備変更以外の行動をすると効果が切れる。",
                                        " Activating this card conceals you for %d turns, making you untargetable by attacks. You can get hit by attacks aimed at someone else. Effect ends if you do anything other than waiting, resting, eating or changing equipment."), base);

		set_hirarinuno_card(base, FALSE);
		break_eibon_flag = FALSE;
	}
	break;

	case ABL_CARD_NITORI:
	{

		if (only_info) return format(_(" このカードを6:00もしくは18:00に所持しているとカード取引所に売られているカードの価格が乱高下する。",
                                        " Carrying this card at 6:00 or 18:00 randomizes value of cards being offered at card exchange."));


	}


	default:
		if (only_info) return _("未実装", "unimplemented");

		msg_format(_("ERROR:実装されていないアビリティカードが使われた (idx:%d)",
                    "ERROR: Unimplemented ability card was used (idx: %d)"), card_idx);
		return NULL;
	}

	return ""; //発動成功するとNULLでない適当な空文字を返す

}




//アビリティカードを使用する。カードはすでに選択されている。成功判定、チャージ減少処理を行い実行部分に渡す。
//行動順消費しないときのみFALSEを返す。
//v1.1.94 発動成功後にターゲットキャンセルした場合行動力を消費しないものとする
bool use_ability_card(object_type *o_ptr)
{
	ability_card_type *ac_ptr;

	int chance = p_ptr->skill_dev;
	int fail;
	bool success;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print(_("止まった時の中ではうまく働かないようだ。", "That doesn't work while time is stopped."));

		sound(SOUND_FAIL);
		return FALSE;
	}

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format(_("ERROR:use_ability_card()にtval%dのアイテムが渡された",
                    "ERROR: Item with tval %d passed to use_ability_card()"), o_ptr->tval);
		return FALSE;
	}

	if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:use_ability_card()に不正なカードidx(%d)を持ったアビリティカードが渡された",
                    "ERROR: Ability card with incorrect card idx (%d) passed to use_ability_card()"), o_ptr->pval);
		return FALSE;
	}

	ac_ptr = &ability_card_list[o_ptr->pval];


	//難易度計算　ロッドと同じ計算を使う
	if (p_ptr->confused) chance /= 2;
	fail = ac_ptr->difficulty + 5;
	if (chance > fail) fail -= (chance - fail) * 2;
	else chance -= (fail - chance) * 2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;


	//成功判定　ロッドと同じ計算を使う
	//カード売人(仮)のみ性格狂気でもカードを使えるようにしてもいい
	if (p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance * 2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail * 2) < chance) success = TRUE;
		else success = FALSE;
	}

	if (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if (!success)
	{
		if (flush_failure) flush();
		msg_print(_("カードの発動に失敗した。", "You fail to activate the card."));

		sound(SOUND_FAIL);
		return TRUE;
	}

	if (o_ptr->timeout > ac_ptr->charge_turn * (o_ptr->number - 1))
	{
		if (flush_failure) flush();
		msg_print(_("このカードはまだチャージが終わっていない。", "This card is still charging."));

		return FALSE;
	}

	if ((o_ptr->timeout + ac_ptr->charge_turn) > 32767)
	{
		msg_format(_("ERROR:timeout値がオーバーフローする。", "ERROR: timeout value overflowed"));
		return FALSE;
	}

	/* Sound */
	sound(SOUND_ZAP);


	//(未鑑定ロッドの判明に相当する処理はない。カードは生成時に常に*鑑定*されているため)


	//msg_format("あなたは『%s』のアビリティカードを発動した！", ac_ptr->card_name);
	//↑メッセージが冗長なので消す

	//実行部分。ターゲットキャンセルなどしたらNULLが返り行動消費しない
	if (!use_ability_card_aux(o_ptr, FALSE)) return FALSE;

	//チャージタイム加算
	o_ptr->timeout += ac_ptr->charge_turn;
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	return TRUE;

}


//アビリティカードの「高騰度」を設定する。
//ゲーム開始直後を含む6:00と18 : 00に呼ばれる。
//宿に連続で泊まったら宿を出る瞬間に一度だけしか呼ばれないが多分実用上の問題はないだろう
void set_abilitycard_price_rate()
{

	int mod = 0;
	int prev_rate = p_ptr->abilitycard_price_rate;

	//ゲーム開始から半日経過ごとにカウント
	int half_days = turn / ((TURNS_PER_TICK * TOWN_DAWN) / 2);

	//EXTRAモードでは一律階層/2
	if (EXTRA_MODE)
	{
		p_ptr->abilitycard_price_rate = (dun_level + 1) / 2;
		return;
	}

	//高騰度10からゲーム開始
	if (!half_days)
	{
		mod = 10;
	}
	//3日目晩までは+1
	else if (half_days < 6)
	{
		mod = 1;
	}
	//7日目晩までは+1+1d2
	else if (half_days < 14)
	{
		mod = 1+randint1(2);
	}
	//14日目晩までは+15%～20%(14日終了時点で300くらいになる。その頃にはガチャを引いたほうが割がいいはず)
	else if (half_days < 28)
	{
		mod = prev_rate * 14 / 100 + randint1(prev_rate * 6 / 100);
	}
	//その後は高騰度300～600くらいの範囲内で乱高下。
	else
	{
		mod = damroll(3, 100) - 150;

		if ((prev_rate + mod) < 300)
		{
			if (mod < 0 && !one_in_(3)) mod *= -1; //あまり下には張り付きにくい
			//else mod = prev_rate - 300; 派手に計算間違ってた
			else mod = 300 - prev_rate;
		}
		else if ((prev_rate + mod) > 600)
		{
			mod = 600 - prev_rate;
		}

	}



	p_ptr->abilitycard_price_rate += mod;

	if (p_ptr->abilitycard_price_rate < 0)
	{
		p_ptr->abilitycard_price_rate = 300;
		msg_print(_("ERROR:abilitycard_price_rateがマイナスになった", "ERROR: abilitycard_price_rate was negative"));

	}

	if (CHECK_ABLCARD_DEALER_CLASS && cheat_xtra)
		msg_format(_("testmsg:アビリティカード高騰度更新:%d(%d)",
                    "testmsg: Ability card inflation update: %d(%d)"), p_ptr->abilitycard_price_rate, mod);


}

//カード販売所のリストを作る
//ゲーム開始直後を含む6:00と18:00に呼ばれる
//リストはp_ptr->magic_num2[0-9]に格納
//↑職によっては使用箇所をずらす
void make_ability_card_store_list(void)
{
	object_type forge;
	object_type *o_ptr = &forge;
	int i;

	bool owned_card_list[ABILITY_CARD_LIST_LEN];
	int bm100th_card_num;


	//カード売人系のクラスのみ
	if (!CHECK_ABLCARD_DEALER_CLASS) return;

	for (i = 0; i < ABILITY_CARD_LIST_LEN; i++) owned_card_list[i] = FALSE;

	bm100th_card_num = count_ability_card(ABL_CARD_100TH_MARKET);

	//「100回目のブラックマーケット」のカードがあるとき所持しているカードのフラグを立てる。
	//インベントリ、追加インベントリ、自宅、床上が対象。
	if (bm100th_card_num)
	{
		store_type  *st_ptr;

		//ザック
		for (i = 0; i < INVEN_PACK; i++)
		{
			if (inventory[i].tval == TV_ABILITY_CARD) owned_card_list[inventory[i].pval] = TRUE;
		}
		//追加インベントリ
		for (i = 0; i < INVEN_ADD_MAX; i++)
		{
			if (inven_add[i].tval == TV_ABILITY_CARD) owned_card_list[inven_add[i].pval] = TRUE;
		}
		//自宅
		st_ptr = &town[1].store[STORE_HOME];
		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if(st_ptr->stock[i].tval == TV_ABILITY_CARD)owned_card_list[st_ptr->stock[i].pval] = TRUE;
		}
		//床上
		//わざと敵対的な妖精とかを召喚してそいつにカードを拾わせたら所持に含まれなくなるが、理屈でいえばそうなるほうが正しい気がする
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];

			/* Skip dead objects */
			if (!o_ptr->k_idx) continue;

			/* Skip held objects */
			if (o_ptr->held_m_idx) continue;

			if (o_ptr->tval == TV_ABILITY_CARD) owned_card_list[o_ptr->pval] = TRUE;
		}

		/* フラグテスト */
		if (cheat_peek)
		{
			for (i = 0; i < ABILITY_CARD_LIST_LEN; i++)
			{
				//if (ability_card_list[i].rarity_rank < 4) owned_card_list[i] = TRUE;
				if (owned_card_list[i])	msg_format("idx%d:TRUE", i);
			}
		}
	}


	for (i = 0; i < 10; i++)
	{
		int tmp;

		while (1)
		{

			//ダミーカード生成 ＠のレベルが高いほど高ランクのものが出やすい
			object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
			tmp = randint1(p_ptr->lev);
			if (tmp >= 45)
				apply_magic_abilitycard(o_ptr, -1, 4, 0);
			else if (tmp >= 30)
				apply_magic_abilitycard(o_ptr, -1, 3, 0);
			else if (tmp >= 15)
				apply_magic_abilitycard(o_ptr, -1, 2, 0);
			else
				apply_magic_abilitycard(o_ptr, -1, 0, 0);

			//「100回目のブラックマーケット」のカードを持っているとき、所有済みのカードが生成されようとしたら確率でやり直し
			if (bm100th_card_num)
			{
				int prob = calc_ability_card_prob(ABL_CARD_100TH_MARKET, bm100th_card_num);

				if (owned_card_list[o_ptr->pval] && randint0(100) < prob)
				{
					if(cheat_peek) msg_print("continue");
					continue;
				}
			}

			break;
		}
		//カードの種別(pval値)を記録
		p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT] = o_ptr->pval;

	}


}

//アビリティカードの販売価格を計算
int calc_ability_card_price(int card_idx)
{
	int price;
	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:calc_ability_card_price()に渡されたcard_idx(%d)が範囲外",
                    "ERROR: card_idx (%d) passed to calc_ability_card_price() is out of range"), card_idx);
		return 0;
	}

	price = 300 + p_ptr->abilitycard_price_rate * ability_card_list[card_idx].trade_value;

	if (ability_card_list[card_idx].rarity_rank == 4) price *= 30;
	else if (ability_card_list[card_idx].rarity_rank == 3) price *= 10;
	else if (ability_card_list[card_idx].rarity_rank == 2) price *= 3;

	return price;

}



//隣接したモンスター一体を指定し、そのモンスターにちなんだカードがあれば買い取る。
//買えるのはカード一種類につき一枚まで。p_ptr->magic_num1[]に買取済みのカードを記録する。
//行動順消費するときTRUE
bool	buy_abilitycard_from_mon(void)
{
	int i;
	int price = 0;
	int dir;
	monster_type *m_ptr;
	int y, x;
	char m_name[160];

	object_type forge;
	object_type *o_ptr = &forge;


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print(_("ERROR:カード売人系以外のクラスでbuy_abilitycard_from_mon()が呼ばれた",
                    "ERROR: buy_abilitycard_from_mon() called for a non-card trader class"));
		return FALSE;
	}
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print(_("今は荷物が一杯だ。", "Your pack is full."));
		return FALSE;
	}

	//隣接モンスター指定
	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir];
	x = px + ddx[dir];
	m_ptr = &m_list[cave[y][x].m_idx];

	if (!cave[y][x].m_idx || !m_ptr->ml)
	{
		msg_format(_("そこには何もいない。", "There's nobody here."));
		return FALSE;
	}

	//起こす
	set_monster_csleep(cave[y][x].m_idx, 0);

	monster_desc(m_name, m_ptr, 0);
	if (p_ptr->inside_arena)
	{
		msg_format(_("夢の世界の%sは好戦的だ！交渉どころではない！",
                    "%s of the Dream World wants to fight! No time for negotiations!"), m_name);
		return TRUE;
	}
	msg_format(_("あなたは%sにカードの買取りを持ち掛けた...",
                "You propose to buy a card from %s..."), m_name);

	//カードリストループ。購入可能なカードがあればiにability_card_list[]のインデックスが入ってループから出る
	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		//カードリストに登録された特定モンスターのみ有効
		if (ability_card_list[i].r_idx != m_ptr->r_idx) continue;

		//EXTRAモードでは帰還用カードが出ない
		if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;
		//現実変容カードも出ない
		if (EXTRA_MODE && i == ABL_CARD_DOREMY) continue;

		//v2.0.7 ゆのみ霊夢、魔理沙はレベル90以降限定で買取可能に
		if (i == ABL_CARD_YUNOMI_REIMU && dun_level < 90) continue;
		if (i == ABL_CARD_YUNOMI_MARISA && dun_level < 90) continue;

		//売却済みフラグが立っているカードはもう出ない。符号付き変数を無理やりビットフラグに使うのはなんか不安なので下位16bitだけフラグに使う。
		if ((p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] >> (i % 16)) & 1L) continue;

		break;
	}

	if (i >= ABILITY_CARD_LIST_LEN)
	{
		msg_print(_("カードを持っていないようだ。", "Not holding any cards."));
		return TRUE;
	}

	//カードを持っているとき、価格を決定して確認を取る
	//価格は典から買う2/3
	price = calc_ability_card_price(i) * 2 / 3;

	//v1.1.93 ミケが特技でフロアに呼んだ客は価格を吹っかけてくる ランダムに二倍から三倍
	if (p_ptr->pclass == CLASS_MIKE && m_ptr->mflag & MFLAG_SPECIAL)
	{
		if (cheat_xtra) msg_print(_("(価格上昇)", "(price raised)"));
		price = price * (200 + m_ptr->mon_random_number % 100) / 100;

	}

	if (!get_check_strict(format(_("「%s」のカードを$%dで売ってくれるようだ。買いますか？",
                                    "'%s' is available for $%d. Purchase it?"), ability_card_list[i].card_name, price), CHECK_DEFAULT_Y))
	{
		msg_print(_("交渉は決裂した！", "Negotiations failed!"));
		return TRUE;
	}

	if (p_ptr->au < price)
	{
		msg_print(_("お金が足りない！", "You don't have enough money."));
		return TRUE;
	}

	//所持金を減らしてカード入手
	p_ptr->au -= price;
	p_ptr->redraw |= (PR_GOLD);
	object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
	apply_magic_abilitycard(o_ptr, i, 0, 0);
	inven_carry(o_ptr);
	msg_format(_("%sからカードを買い取った！", "You bought a card from %s!"), m_name);

	//入手済みフラグを立てる
	p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] |= ((1L) << (i % 16));

	//売買が成立したときモンスターを友好的にする。ただしクエストダンジョン内やクエスト討伐対象やダンジョンボスの場合敵対したまま
	//↑護衛持ちモンスターが売買成立した途端配下からタコ殴りにされるのがなんか変なので中止
	//if (!(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) && !p_ptr->inside_quest)
	//{
	//		set_friendly(m_ptr);
	//}

	return TRUE;
}


//アビリティカード「資本主義のジレンマ」の効果
//カード販売所の在庫10種それぞれに対し、価格を乱高下させる係数を計算する
//6:00と18:00にprocess_world()から呼ばれる
void break_market()
{
	int card_num;
	int i;

	//カード売人のみ
	if (!(CHECK_ABLCARD_DEALER_CLASS)) return;

	//カード乱高下度はmagic_num2[10-19]に記録される
	//0は影響なし、1～250のランダムで50を基準値として1増減するごとに価格が2%上下する

	//まずカード有無に関わらず効果リセット
	for (i = 0; i < 10; i++)
	{
		p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT]=0;
	}

	//カードを所持していないなら効果リセットしたまま終了
	card_num = count_ability_card(ABL_CARD_NITORI);
	if (!card_num) return;

	for (i = 0; i < 10; i++)
	{
		int tmp;

		//価格が下がる
		if (weird_luck())
		{
			tmp = 50 - randint1(5)*card_num - randint1(10);
			if (tmp < 1) tmp = 1;
		}
		//価格が上がる
		else
		{
			tmp = 50 + randint1(20)*card_num + randint1(40);
			if (tmp > 250) tmp = 250;

		}

		if (cheat_peek) msg_format("mult%d:%d", i, tmp);

		p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT] = (byte)tmp;

	}


}


//千亦がアビリティカードを発動するときの失敗率計算　魔道具術師とほぼ同じ
int calc_chimata_activate_fail_rate(int card_idx)
{
	int level, chance;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:calc_chimata_activate_fail_rate(%d)", card_idx);
		return 0;
	}

	//成功率はロッドと同じ計算を使う。k_info[].levelに当たる値としてability_card_list[card_idx].difficultyを使用する
	level = (ability_card_list[card_idx].difficulty * 5 / 6 - 5);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[A_INT]] - 1);
	level /= 2;
	if (p_ptr->lev > level)
	{
		chance -= 3 * (p_ptr->lev - level);
	}
	chance = mod_spell_chance_1(chance);
	chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[A_INT]]);
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;
	if (chance > 95) chance = 95;
	chance = mod_spell_chance_2(chance);

	return chance;
}



//千亦がカードを確認または発動する
//カードの所持未所持はp_ptr->magic_num2[card_idx]で判定され、
//発動済みのカードのチャージターンはp_ptr->magic_num1[card_idx]に記録される
//ターゲットキャンセルなど行動順消費しなかったときFALSE
bool	chimata_activate_ability_card_power(bool only_info)
{

	int card_idx_list[100];//5ページ分
	int col_max = 20;//1ページに20枚まで表示
	int page_num = 0;

	int i;
	int fail_rate;
	int card_idx;//選択されたカード
	int card_idx_list_len = 0;//使用可能なカードのリスト
	bool flag_choice = FALSE;//カードが選択されたフラグ
	bool flag_need_redraw = TRUE;//カードリスト再描画フラグ

	object_type dummyitem;
	object_type *o_ptr = &dummyitem;

	int card_number; //保有カードの一種類ごとの枚数　流通ランクと同じ値になる

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_activate_ability_card_power(class:%d)", p_ptr->pclass);
		return FALSE;
	}
	//ないとは思うけど今後さらにアビリティカードが大量に追加されたときに備えて一応
	if (ABILITY_CARD_LIST_LEN >= 100)
	{
		msg_format(_("ERROR:chimata_activate_ability_card_power()のページ数が足りない",
                    "ERROR: Not enough pages amount in chimata_activate_ability_card_power()"));
		return FALSE;
	}

	card_number = (CHIMATA_CARD_RANK);

	//card_idx_listにability_card_list[]のidx値を格納する。
	//そのまま使わないのは発動用選択のとき発動用でないカードを飛ばすため
	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		if (!only_info && !ability_card_list[i].activate) continue;
		card_idx_list[card_idx_list_len++] = i;
	}

#ifdef ALLOW_REPEAT
	//リピートのときカードidxを取得する
	if (repeat_pull(&card_idx))
	{
		flag_choice = TRUE;
		flag_need_redraw = FALSE;
	}
#endif

	if (flag_need_redraw) screen_save();

	//選択画面を表示してカードを選択するループ
	while (!flag_choice)
	{

		char c;
		int tmp;

		//カードリストを表示
		if (flag_need_redraw)
		{

			for (i = 2; i<25; i++) Term_erase(17, i, 255);

			c_put_str(TERM_WHITE, _("　　　　       名称　　　　　　　　失率",
                                    "               Name                Fail"), 3, 20);
			for (i = 0; i<col_max; i++)
			{
				int tmp_card_idx;
				if (page_num * col_max + i >= card_idx_list_len) break;

				tmp_card_idx = card_idx_list[page_num * col_max + i];

				//未所持
				if (!CHECK_CHIMATA_HAVE_CARD(tmp_card_idx))
				{
					if (tmp_card_idx == ABL_CARD_100TH_MARKET)
						c_put_str(TERM_L_DARK, _("(*勝利*すると開放される)",
                                                "(unlocked for *WINNERS*)"), 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_YUNOMI_REIMU)
						c_put_str(TERM_L_DARK, _("(レベル90以上の？？からのみ入手できる)",
                                                "(obtain from level 90+ ?????)"), 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_YUNOMI_MARISA)
						c_put_str(TERM_L_DARK, _("(レベル90以上の？？？からのみ入手できる)",
                                                "(obtain from level 90+ ??????)"), 4 + i, 20);
					else if (tmp_card_idx == ABL_CARD_SHION)
						c_put_str(TERM_L_DARK, _("(本気を出した？？からのみ入手できる)",
                                                "(obtain from true form of ?????)"), 4 + i, 20);
					else
						c_put_str(TERM_L_DARK, _("？？？？？？？",
                                                "????????????"), 4 + i, 20);
				}
				//発動用カード
				else if (ability_card_list[tmp_card_idx].activate)
				{
					int valid_card_num;//チャージ中でない使用可能カードの枚数

					fail_rate = calc_chimata_activate_fail_rate(tmp_card_idx);

					valid_card_num = (card_number * ability_card_list[tmp_card_idx].charge_turn - p_ptr->magic_num1[tmp_card_idx]) / ability_card_list[tmp_card_idx].charge_turn;

					//充填中のカードは灰色にする
					if (valid_card_num)
						c_put_str(TERM_WHITE, format("%c) %25s(%d/%d)  %3d%%", ('a' + i), ability_card_list[tmp_card_idx].card_name, valid_card_num, card_number, fail_rate), 4 + i, 20);
					else
						c_put_str(TERM_L_DARK, format("%c) %25s(%d/%d)  %3d%%", ('a' + i), ability_card_list[tmp_card_idx].card_name, valid_card_num, card_number, fail_rate), 4 + i, 20);


				}
				//所有効果カード
				else
				{
					c_put_str(TERM_WHITE, format("%c) %30s  ---", ('a' + i), ability_card_list[tmp_card_idx].card_name), 4 + i, 20);

				}
			}

			c_put_str(TERM_WHITE, _("(スペース:次のページ ESC:キャンセル)",
                                    "(Spacebar: Next page ESC: Cancel)"), 4 + i, 20);

			flag_need_redraw = FALSE;
		}

		//入力を受け付けてチェック

		c = inkey();

		//キャンセル
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}

		//スペースはページ番号を増やし再描画
		if (c == ' ')
		{
			page_num++;
			//ページがリスト数を超えたら最初のページへ
			if (page_num * col_max >= card_idx_list_len) page_num = 0;
			flag_need_redraw = TRUE;
			continue;
		}
		//範囲外　入力し直し
		if (c < 'a' || c >= ('a' + col_max))
		{
			continue;
		}

		//ページ番号と入力値からリストのインデックスを計算
		tmp = page_num * col_max + (c - 'a');

		//範囲内だがリストの終端以降　入力し直し
		if (tmp >= card_idx_list_len)
		{
			continue;
		}
		card_idx = card_idx_list[tmp];

		//未習得のカードを選択　入力し直し
		if (!CHECK_CHIMATA_HAVE_CARD(card_idx))
		{
			continue;
		}

#ifdef ALLOW_REPEAT
		repeat_push(card_idx);
#endif

		flag_need_redraw = TRUE; //ループを出てすぐscreen_load()をするためTRUEにする
		break;
	}//カード選択ループ終了

	if (flag_need_redraw) screen_load();

	//カードの力を使うためのダミーアイテム生成
	object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
	o_ptr->pval = card_idx;
	o_ptr->ident |= (IDENT_MENTAL);
	o_ptr->timeout = p_ptr->magic_num1[card_idx];
	o_ptr->number = card_number;


	//カードを確認するだけの場合は行動順消費なしで終了
	if (only_info)
	{
		screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
		return FALSE;
	}

	//充填中のカードはメッセージを出してFALSEで終了　リピートで呼ばれることもあるのでループ外で処理する
	if (o_ptr->timeout > ability_card_list[card_idx].charge_turn * (o_ptr->number - 1))
	{
		if (flush_failure) flush();
		msg_print(_("まだチャージが終わっていない。", "Still charging."));

		return FALSE;
	}

	//発動失敗判定
	fail_rate = calc_chimata_activate_fail_rate(card_idx);
	if (randint0(100) < fail_rate)
	{
		msg_print(_("カードの力の発動に失敗した。", "You failed to activate the card's ability."));
		return TRUE;
	}

	if (card_idx != ABL_CARD_BLANK)	msg_format(_("「%s」の力を開放した！", "You unleash the power of '%s'!"), ability_card_list[card_idx].card_name);

	//ダミーカードを実行部分に渡す
	if (!use_ability_card_aux(o_ptr, FALSE)) return FALSE;

	//チャージ値を設定
	p_ptr->magic_num1[card_idx] += ability_card_list[card_idx].charge_turn;

	return TRUE;

}



//千亦がカードの流通ランクを計算する
//カードの所持非所持はp_ptr->magic_num2[card_idx]に記録されており、
//所持している種類によって流通ランクが決められp_ptr->magic_num2[255]に保存される
//このランクは各種のカードを持っている「枚数」として扱われる
//戻り値はランクでなく所有枚数。枚数を表示したい時に使えるように
int 	chimata_calc_card_rank(void)
{

	int card_num = 0;
	int card_rank = 0;
	int i;

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_calc_card_rank(class:%d)", p_ptr->pclass);
		return 0;
	}


	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		if (p_ptr->magic_num2[i]) card_num++;
	}

	if (card_num == ABILITY_CARD_LIST_LEN)
		card_rank = 9;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 2)
		card_rank = 8;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 5)
		card_rank = 7;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 10)
		card_rank = 6;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 20)
		card_rank = 5;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 35)
		card_rank = 4;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 55)
		card_rank = 3;
	else if (card_num >= ABILITY_CARD_LIST_LEN - 75)
		card_rank = 2;
	else
		card_rank = 1;

	p_ptr->magic_num2[255] = card_rank;

	if (p_ptr->wizard) msg_format("card num:%d rank:%d", card_num, card_rank);

	return card_num;
}


//千亦がカードの流通ランクによってメッセージを表示する
void chimata_comment_card_rank(void)
{

	int card_rank, card_num;
	int i;

	if (p_ptr->pclass != CLASS_CHIMATA)
	{
		msg_format("ERROR:chimata_comment_card_rank(class:%d)", p_ptr->pclass);
		return;
	}


	//カードを数えてランクを計算
	card_num = chimata_calc_card_rank();

	//計算したランクをp_ptr->magic_num[255]から取得
	card_rank = CHIMATA_CARD_RANK;

	screen_save();

	for (i = 10; i<16; i++) Term_erase(17, i, 255);

	prt(format(_("流通ランク:%d/9", "Popularity Rank: %d/9"), card_rank), 11, 20);
	switch (card_rank)
	{
	case 1:

		prt(_("まだほとんど誰もアビリティカードのことを知らない。",
                "Hardly anyone is aware of the ability cards."), 12, 20);
		prt(_("この体が消えてしまう前にもっとカードを流通させないといけない。",
            "You have to make them more popular before you fade away."), 13, 20);
		prt(_("「空白のカード」を使い新たなカードを作って広めよう。",
            "Use your 'Blank Card' to create new ones."), 14, 20);
		break;

	case 2:
		prt(_("まだまだカードの知名度は低い。",
            "The cards are still an obscure concept."), 12, 20);
		prt(_("気を抜くと風に飛ばされそうになる...",
            "You might get scattered by the wind if you're not careful..."), 13, 20);
		break;

	case 3:
		prt(_("山師たちが密かにカードを買い漁っているようだ。",
            "Looks like some opportunists are secretly purchasing your cards."), 12, 20);
		prt(_("まだ少し体が透き通っている。",
            "Your body is still fairly transparent."), 13, 20);
		break;

	case 4:
		prt(_("カードが知る人ぞ知る最先端ホビーとして認知されてきた。",
            "The cards have been recognized as a popular hobby."), 12, 20);
		prt(_("挨拶を返されることが増えてきたように感じる。",
            "You feel being more and more worshipped."), 13, 20);
		break;

	case 5:
		prt(_("カードが広く流通し始めた。", "The cards are widely popular."), 12, 20);
		prt(_("立振舞に力強さが戻ってきた。",
            "Strength has returned to your body."), 13, 20);
		break;

	case 6:
		prt(_("妖怪の山でカード集めが一大ブームだ！",
            "There's a card collector boom at Youkai Mountain!"), 12, 20);
		prt(_("行き交う人々がみな好意的な視線を向けてくる。",
            "All those visitors are looking at you favorably."), 13, 20);
		break;

	case 7:
		prt(_("妖精たちもみんなカードで遊んでいる！", "All of the fairies are playing cards!"), 12, 20);
		prt(_("なんだか周りが明るいと思ったら自分の後光だった。",
            "You thought it was getting bright, but it's just your radiant backglow."), 13, 20);
		break;

	case 8:
		prt(_("人里の住人も先を争ってカードを買い求めている！",
            "Human Village residents are scrambling to buy more cards!"), 12, 20);
		prt(_("膨大な信仰の力が体を満たすのを感じる！",
            "You feel an immense amount of faith!"), 13, 20);
		break;

	case 9:
		prt(_("いまや幻想郷のあらゆる人妖がアビリティカードに夢中だ！",
            "All residents of Gensoukyou are obsessed over collecting cards!"), 12, 20);
		prt(_("ついにかつての輝かしい日々が戻ってきた！",
            "Your glorious days of former power are finally back!"), 13, 20);
		break;

	default:
		msg_format("ERROR:shimata_comment_card_rank(%d)", card_rank);
		break;
	}

	inkey();

	screen_load();

}

