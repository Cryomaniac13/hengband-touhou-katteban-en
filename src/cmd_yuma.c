#include "angband.h"

/*
 *cmd_yuma.c
 *v2.0.6
 *主に饕餮尤魔関係の処理
 */


 //yuma_gain_itemflag()のフラグ記録部分だけ切り分ける。武具以外のアイテムを食べたときにも個別に指定すればスキルや耐性を得られるように
 //s_idxはTR_***の値と一致
 //何かを新しく得たらTRUE
bool yuma_gain_itemflag_aux(int s_idx)
{
	bool flag_new = FALSE;

	if (p_ptr->pclass != CLASS_YUMA || s_idx < 0 || s_idx >= TR_FLAG_MAX)
	{
		msg_format("ERROR:yuma_gain_itemflag_aux(%d)", s_idx);
		return FALSE;
	}

	//スレイや武器属性などは除外
	if (IS_WEAPON_FLAG(s_idx)) return FALSE;
	//強力射撃などは除外
	if (IS_BOW_FLAG(s_idx)) return FALSE;
	//呪いや反テレポなどは除外
	if (IS_BAD_FLAG(s_idx)) return FALSE;
	//元素保護やSHOW_MODSなど特殊フラグは除外
	if (IS_SUB_FLAG(s_idx)) return FALSE;

	if (!p_ptr->magic_num2[s_idx])
	{
		flag_new = TRUE;
		msg_format(_("%sの特性を得た！", "You gain the special ability '%s'!"), equipment_tr_flag_names[s_idx]);
	}

	p_ptr->magic_num2[s_idx] = YUMA_FLAG_DELETE_TURN;

	return flag_new;

}


 //尤魔が「食べる」コマンドか「吸い込む」特技で装備品を食べたときそのアイテムが持つフラグ値を使えるようになる
 //p_ptr->magic_num2[idx](idxはTR_***と共通)に50を設定する。
 //これを1000turn毎に1減らし0になったら使えなくなる。ゲーム内で約半日。
 //何らかのフラグを新たに得たらTRUEを返す。
bool	yuma_gain_itemflag(object_type *o_ptr)
{

	u32b flgs[TR_FLAG_SIZE];
	int i, j, idx;
	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_gain_itemflag()"); return FALSE; }

	object_flags(o_ptr, flgs);

	//アイテムからTR_***のフラグがあるかどうか一つずつ調べ、あったらyuma_gain_itemflag_aux()に渡してmagic_num2[]に記録している
	for (i = 0; i<TR_FLAG_SIZE; i++)
	{
		for (j = 0; j<32; j++)
		{
			int s_idx = 32 * i + j;

			if (s_idx >= TR_FLAG_MAX) break;

			if ((1L << j) & flgs[i])
			{
				if (yuma_gain_itemflag_aux(s_idx)) flag_gain = TRUE;
			}
		}
	}

	if (flag_gain) p_ptr->update |= PU_BONUS;

	return flag_gain;


}




 //yuma_gain_monspell()のフラグ記録部だけ切り分ける。
 //s_idxはmonspell_list2[]のインデックスと一致
 //何かを新しく得たらTRUE
bool yuma_gain_monspell_aux(int s_idx)
{
	int i, j, idx;
	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA || s_idx <= 0 || s_idx > MAX_MONSPELLS2)
	{
		msg_print("ERROR:yuma_gain_monspell_aux()");
		return FALSE;
	}

	//「呪われた血液の追憶」を習得するまで特技を得られない
	if (p_ptr->lev < 30) return FALSE;

	//未実装特技と特別行動を除外
	if (!monspell_list2[s_idx].level) return FALSE;

	//レベルの足りない特技は習得できない？どうせ使えないので問題ないか
	//if (monspell_list2[s_idx].level > p_ptr->lev) return;

	if (!p_ptr->magic_num1[s_idx])
	{
		flag_gain = TRUE;
		msg_format(_("%sの能力を吸収した！", "You absorb the ability '%s'!"), monspell_list2[s_idx].name);

	}

	p_ptr->magic_num1[s_idx] = YUMA_FLAG_DELETE_TURN;

	return flag_gain;

}


 //尤魔が「吸い込む」コマンドでモンスターを倒したときそのモンスターがもつ特技を使えるようになる
 //p_ptr->magic_num1[idx](idxはmonspell_list2[]と共通)に50を設定する。
 //これを1000turn毎に1減らし0になったら使えなくなる。ゲーム内で約半日。
 //何らかの技を新たに得たらTRUEを返す。
bool	yuma_gain_monspell(monster_race *r_ptr)
{

	int i, j, s_idx;

	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_gain_monspell()"); return FALSE; }

	for (i = 0; i<4; i++)
	{
		u32b flgs;

		//r_infoの所有特技フラグをmonspell_list2[]インデックスに変換
		if (i == 0)
			flgs = r_ptr->flags4;
		else if (i == 1)
			flgs = r_ptr->flags5;
		else if (i == 2)
			flgs = r_ptr->flags6;
		else
			flgs = r_ptr->flags9;

		for (j = 0; j<32; j++)
		{

			s_idx = 1 + 32 * i + j;

			if (s_idx > MAX_MONSPELLS2) break;

			//その特技を持っていたらプレイヤーの使用可能魔法リスト(p_ptr->magic_num1[idx])に記録
			if ((1L << j) & flgs)
			{
				if (yuma_gain_monspell_aux(s_idx)) flag_gain = TRUE;

			}
		}
	}

	return flag_gain;

}



//尤魔がモンスターや武具から得たパワーが時間経過により失われる
//1000turnごとに実行し1を受け取って内部カウント値から減らす。宿屋などで時間経過した場合は経過turn/1000を受け取る
void	yuma_lose_extra_power(int dec_val)
{

	int i;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_lose_extra_power()"); return; }

	if (cheat_xtra) msg_format("yuma_lose_%d", dec_val);

	//モンスターから得たスキル
	for (i = 0; i <= MAX_MONSPELLS2; i++)
	{
		if (p_ptr->magic_num1[i] > 0)
		{
			if (dec_val > p_ptr->magic_num1[i]) dec_val = p_ptr->magic_num1[i];
			p_ptr->magic_num1[i] -= dec_val;

			if (p_ptr->magic_num1[i] == 0)
			{
				msg_format(_("%sの能力が失われた。", "You no longer have the ability '%s'."), monspell_list2[i].name);

			}
		}
	}
	//武具から得た装備品フラグ
	for (i = 0; i<TR_FLAG_MAX; i++)
	{
		if (p_ptr->magic_num2[i] > 0)
		{
			if (dec_val > p_ptr->magic_num2[i]) dec_val = p_ptr->magic_num2[i];
			p_ptr->magic_num2[i] -= dec_val;

			if (p_ptr->magic_num2[i] == 0)
			{
				msg_format(_("%sの能力が失われた。", "You no longer have the ability '%s'."), equipment_tr_flag_names[i]);
				p_ptr->update |= (PU_BONUS | PU_LITE);
			}
		}
	}

}


//尤魔が通常Eコマンドで食べないような物を食べるときの処理 do_cmd_eat_food_aux()から呼ばれる
//TV_FOOD,TV_MUSHROOM,TV_SWEETS,TV_MATERIALはdo_cmd_eat_food_aux()で処理が済んでいる
//TV_POTION,TV_COMPOUND,TV_ALCOHOL,TV_SOFTDRINKはここでなくquaff_potion_aux()に渡される
//この食物で得られる満腹度を返す
//0が返されたとき食べるのに失敗したとみなし外でもアイテムを減らさず処理終了する
int yuma_eat_other_things(object_type *o_ptr)
{
	int food_val = 0;
	int price;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_eat_other_thing()"); return 0; }

	//通常のたいまつやランタンは燃料の残量(o_ptr->xtra4)を計算に入れる
	if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH && object_is_nameless(o_ptr))
	{
		msg_print(_("火のついたままの松明を呑み込んだ！", "You swallow the lit torch!"));
		food_val = o_ptr->xtra4 + 100;
	}
	else if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_LANTERN && object_is_nameless(o_ptr))
	{
		msg_print(_("ランタンを噛み砕いて油を飲んだ！", "You chew up the lantern and drink the oil!"));
		food_val = o_ptr->xtra4 + 1000;
	}
	//装備品は価格をそのまま食料にし、さらにそれが持つ耐性などを一時的に獲得する
	else if (object_is_equipment(o_ptr))
	{
		switch (o_ptr->tval)
		{
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:
			msg_print(_("危険な武器を手品のように呑み込んだ！", "You swallow the dangerous weapon, as if performing a magic trick!")); break;
		case TV_GUN:
			msg_print(_("銃器をへし折って飲み下した！腹の中から爆発音がする！", "You snap the firearm and swallow it! There's an explosion in your stomach!")); break;
		case TV_RIBBON:
			msg_print(_("美しい飾り布を一息に吸い込んだ！", "You inhale the beautiful ribbon decoration, consuming it!")); break;
		case TV_LITE:
			msg_print(_("眩く光る何かが腹に吸い込まれた！", "You drop the brightly shining item into your stomach!")); break;
		case TV_CLOTHES:
		case TV_CLOAK:
			msg_print(_("丈夫な布地をパイのように食いちぎった！", "You eat the tough fabric like a pie!")); break;
		case TV_ARMOR:
			msg_print(_("大きな鎧を一呑みにした！", "You swallow the large armor in a single gulp!")); break;
		case TV_DRAG_ARMOR:
			msg_print(_("龍の鱗は中々の珍味だ！", "Dragon scales are a rare delicacy!")); break;
		case TV_BOOTS:
			msg_print(_("靴の味もそれほど悪くないものだ。", "Boots don't taste that bad.")); break;
		case TV_RING:
		case TV_AMULET:
			msg_print(_("綺麗な装飾品を砂糖菓子のように噛み砕いた！", "You chew on the beautiful accessory as if it was candy!")); break;
		default:
			msg_print(_("武具をバリボリ食べた！", "You crunch the piece of equipment down!")); break;

		}

		food_val = object_value_real(o_ptr);
		//呪われていると栄養を増やす
		if (!food_val)
		{
			food_val = 1;
			if (object_is_cursed(o_ptr))
			{
				msg_print(_("この呪いは実に美味だ！", "This curse is really delicious!"));
				food_val += 5000;
				if (!object_is_nameless(o_ptr)) food_val += 15000;
			}
		}
		yuma_gain_itemflag(o_ptr);
	}
	else if (o_ptr->tval == TV_MAGICITEM || o_ptr->tval == TV_MAGICWEAPON)
	{
		msg_print(_("珍しい魔法道具を惜しげもなく食べた！", "You just ate a rare magical item!"));
		food_val = MAX(object_value_real(o_ptr), 5000);
		yuma_gain_itemflag(o_ptr);

	}

	//魔法書と巻物 とりあえず価格をそのまま食料に。高位魔法書はなにか特技を獲得するか？
	else if (o_ptr->tval <= TV_BOOK_END || o_ptr->tval == TV_SCROLL)
	{
		msg_print(_("魔力の染みた紙を貪り食った！", "You devour the mana-imbued paper!"));

		//高位魔法書は経験値を得られることにする
		if (o_ptr->sval > 1) gain_exp(object_value_real(o_ptr)/2);

		food_val = 100;
	}
	//金　額面と同額を食料に
	else if (o_ptr->tval == TV_GOLD)
	{
		msg_print(_("金銀財宝があなたの口に吸い込まれた！", "Valuable treasure gets sucked into your mouth!"));
		food_val = o_ptr->pval;

	}
	//杖・魔法棒・ロッド MPも回復
	else if (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND || o_ptr->tval == TV_ROD)
	{

		msg_print(_("魔法の杖を砂糖菓子のように噛み砕いた！", "You chew on the magic device as if it was candy!"));
		food_val = object_value_real(o_ptr);

		if (!food_val) food_val = 1;

		if (player_gain_mana(k_info[o_ptr->k_idx].level * (o_ptr->tval == TV_ROD ? 4 : o_ptr->pval / 2)))
			msg_print(_("魔道具の魔力を吸収した。", "You have absorbed the magic device's mana."));

	}
	//新聞と写真　そのモンスターの情報を得る
	else if (o_ptr->tval == TV_PHOTO || o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
	{
		if (o_ptr->pval > 0 && o_ptr->pval < max_r_idx)
		{
			if (lore_do_probe(o_ptr->pval))
				msg_format(_("%sの情報を得た。", "You learn about %s."), r_name + r_info[o_ptr->pval].name);

		}

		//栄養はない。エラー扱いにならんよう1だけ返す
		food_val = 1;
	}
	else if (o_ptr->tval == TV_CHEST)
	{
		if (!o_ptr->pval)
		{
			msg_print(_("空き箱を食べた。", "You ate an empty container."));
			food_val = 100;
		}
		else
		{
			msg_print(_("何かの入っている箱を中身ごと食べた！", "You ate a container with something inside!"));
			food_val = 10000;

		}
	}
	//妖魔本と人形　そのモンスターの特技を獲得する
	else if (o_ptr->tval == TV_FIGURINE || o_ptr->tval == TV_CAPTURE)
	{
		food_val = object_value_real(o_ptr);
		if (!food_val) food_val = 5000;
		if (o_ptr->pval > 0 && o_ptr->pval < max_r_idx)
		{
			yuma_gain_monspell(&r_info[o_ptr->pval]);
		}
	}
	else if (o_ptr->tval == TV_ITEMCARD)
	{
		msg_print(_("カードの魔力を吸い取った！", "You have absorbed mana from the card!"));

		food_val = support_item_list[o_ptr->pval].cost * 300;
	}
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		msg_print(_("カードから強い魔力を感じたのだが、噛んだ途端に消えてしまった。",
                    "You feel strong magical power from the card, but it goes away as you bite down."));
		food_val = 1;

	}
	//薬、酒はここに入る前にquaff_potion_aux()に渡す。そこで効果発動とアイテム減少処理も行われる。
	/*
	else if (o_ptr->tval == TV_POTION || o_ptr->tval == TV_COMPOUND || o_ptr->tval == TV_ALCOHOL || o_ptr->tval == TV_SOFTDRINK)
	{
	}
	*/
	//燃料　尤魔プレイ時に手に入るのは油壷のみのはず
	else if (o_ptr->tval == TV_FLASK)
	{
		msg_print(_("油つぼを瓶ごと食べた！", "You ate the whole flask of oil!"));
		food_val = 5000;
	}
	//珍品　とりあえず経験値でも得ておく。あるいは何か特技獲得とかしてもいいかもしれない
	else if (o_ptr->tval == TV_SOUVENIR)
	{
		int exp = calc_korin_sell_price(o_ptr);
		int val = 1000 + object_value_real(o_ptr);

		gain_exp(MAX(exp, 1));
		food_val = val;

	}
	//何でも食べる設定だが、*真・勝利*後のトロフィー的なものは食べなくてもよかろう
	else if (o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ANTIQUE || o_ptr->tval == TV_ANTIQUE)
	{
		msg_print(_("これを食べるのは勿体ない。", "There's nothing to gain from eating this."));
		food_val = 0;
	}

	else
	{
		msg_format(_("ERROR:yuma_eat_other_things()に未定義のアイテム(TV:%d SV:%d)が渡された",
                    "ERROR: unknown item in yuma_eat_other_things() (TV: %d SV: %d)"), o_ptr->tval, o_ptr->sval);
		return 0;
	}

	return food_val;

}




//尤魔特技「吸い込む」で特定グリッドのアイテムを吸い込む
//rad:＠から吸い込める有効距離
//何か食べたらTRUE
bool yuma_swallow_item(int y, int x, int rad)
{
	bool flag_eat_sth = FALSE;
	int tmp_o_idx;

	//マップ範囲外
	if (!in_bounds(y, x)) return FALSE;
	//アイテムがないグリッド
	if (!cave[y][x].o_idx) return FALSE;
	//距離
	if (distance(py, px, y, x) > rad) return FALSE;
	//射線
	if (!projectable(py, px, y, x)) return FALSE;

	tmp_o_idx = cave[y][x].o_idx;

	while (tmp_o_idx)
	{

		int item = 0 - tmp_o_idx;
		int next_o_idx = o_list[tmp_o_idx].next_o_idx;

		//eat_food_aux()にo_list[]のインデックスをマイナスにしたものを渡す
		if (do_cmd_eat_food_aux(item))
		{
			flag_eat_sth = TRUE;
			//食べてその枠が空になった場合スタックの次のアイテムへ
			if (!o_list[tmp_o_idx].k_idx) tmp_o_idx = next_o_idx;
		}
		else //eat_food_aux()が食用不可と判断しパスしたときはスタックの次のアイテムへ
		{
			tmp_o_idx = next_o_idx;
		}

	}

	return flag_eat_sth;

}



 //尤魔特技「吸い込む」で特定グリッドのモンスターを引き寄せてダメージを与え、倒したら能力を吸収しときどき性格が変わる
 //rad:＠から吸い込める有効距離
 //モンスターから栄養を得たらTRUEを返す
bool yuma_swallow_mon(int y, int x, int rad)
{
	int m_idx,r_idx;
	int mon_resist;
	monster_type *m_ptr;
	monster_race *r_ptr;
	int dam;
	bool dummy = FALSE;
	char m_name[120];

	//マップ範囲外
	if (!in_bounds(y, x)) return FALSE;

	m_idx = cave[y][x].m_idx;

	//モンスターがいないグリッド
	if (!m_idx) return FALSE;
	//距離
	if (distance(py, px, y, x) > rad) return FALSE;
	//射線
	if (!projectable(py, px, y, x)) return FALSE;

	dam = p_ptr->lev * 5;
	if (YUMA_ULTIMATE_MODE) dam *= 2;

	m_ptr = &m_list[m_idx];
	r_idx = m_ptr->r_idx;
	r_ptr = &r_info[r_idx];

	//はぐりん無効
	if (r_ptr->flagsr & RFR_RES_ALL) return FALSE;

	//騎乗モンスターは対象外 ＠と同じグリッドでは呼ばないが一応
	if (p_ptr->riding == m_idx) return FALSE;

	//モンスターを周囲に引き寄せる 力強い敵と巨大な敵には効きづらい 飛んでいる敵には効きやすい
	mon_resist = r_ptr->level;
	if (r_ptr->flags2 & (RF2_POWERFUL | RF2_GIGANTIC))mon_resist *= 2;
	if (r_ptr->flags7 & RF7_CAN_FLY) mon_resist /= 2;

	monster_desc(m_name, m_ptr, 0);

	if (mon_resist / 2 + randint0(mon_resist / 2) < p_ptr->lev && m_ptr->cdis > 1)
	{
		msg_format(_("%sを吸い寄せた！", "You drag %s in!"), m_name);
		teleport_monster_to(m_idx, py, px, 100, TELEPORT_FORCE_NEXT);
	}


	//引き寄せ後に隣接しているときのみダメージ判定
	if (m_ptr->cdis == 1)
	{
		msg_format(_("%sの力を吸収している！", "You absorb the power of %s!"), m_name);
		//ダメージを与える。倒したとき{}内へ。このときm_ptrはもうクリアされているはずなので注意
		if (mon_take_hit(m_idx, dam, &dummy, extract_note_dies(real_r_ptr(m_ptr))))
		{

			//満腹度上昇　数値は種族動物霊と同じ計算式だが敵種族を問わない
			int feed = 1000 + r_ptr->level * 50 + r_ptr->level * r_ptr->level;
			set_food(p_ptr->food + feed);

			//そのモンスターの能力を得る
			yuma_gain_monspell(r_ptr);

			//ユニークモンスターを倒したときたまに性格変更
			if (!ap_ptr->flag_strange_select)
			{
				int new_seikaku = p_ptr->pseikaku;
				switch (r_idx)
				{
				case MON_REIMU:
					new_seikaku = SEIKAKU_SHIAWASE; break;//霊夢のんき
				case MON_MARISA:
					new_seikaku = SEIKAKU_FUTUU; break;//魔理沙極めて普通
				case MON_SAKUYA:
					new_seikaku = SEIKAKU_ELEGANT; break;//咲夜瀟洒
				case MON_CIRNO:
					new_seikaku = SEIKAKU_INOCHI; break;//チルノおてんば
				case MON_RAIKO:
					new_seikaku = SEIKAKU_KIREMONO; break;//雷鼓聡明

					//TODO:ほか誰か居たか？

				default:

					if (!(r_ptr->flags1 & RF1_UNIQUE)) break;
					//キャノンボウラーやラッキーガールなど特殊な処理が必要な性格を除いてランダムに選ぶ
					do
					{
						new_seikaku = randint0(MAX_SEIKAKU);
					} while (seikaku_info[new_seikaku].flag_strange_select);
					break;

				}
				if (new_seikaku != p_ptr->pseikaku)
				{
					msg_print(_("性格が変わった気がする...", "You feel your personality changing..."));
					p_ptr->pseikaku = new_seikaku;
					ap_ptr = &seikaku_info[new_seikaku];
					p_ptr->update |= PU_BONUS;

				}
			}


		}
		//倒さなくても栄養を得る
		else
		{
			set_food(p_ptr->food + dam);
		}

		//栄養を得たときTRUE
		return TRUE;

	}

	return FALSE;

}




//yuma_swallow()から呼ばれる
//特定グリッドの地形を消滅させて満腹度上昇
//rad:＠から吸い込める有効距離
//なにか食べたらTRUEを返す
bool yuma_swallow_terrain(int y, int x, int rad)
{
	int m_idx;
	feature_type *f_ptr;

	int prob = 0;//吸い込みに成功する確率
	int food = 0;//吸い込んだときに得る満腹度

	//マップ範囲外
	if (!in_bounds(y, x)) return FALSE;

	f_ptr = &f_info[cave[y][x].feat];

	//永久地形は除外
	if (cave_perma_bold(y, x)) return FALSE;

	//距離
	if (distance(py, px, y, x) > rad) return FALSE;
	//射線
	if (!projectable(py, px, y, x)) return FALSE;

	if (cave_have_flag_bold(y, x, FF_DOOR))//ドア・カーテン
	{
		prob = 75;
		food = 1000;
	}
	else if (cave_have_flag_bold(y, x, FF_TREE))//木
	{
		prob = 30;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_PLANT))//草地・草むら・花
	{
		prob = 80;
		food = 1000;
	}
	else if (cave_have_flag_bold(y, x, FF_GLYPH))//守りのルーン
	{
		prob = 30;
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_MINOR_GLYPH))//爆発ルーン
	{
		prob = 70;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_LAVA))//溶岩
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_fire(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_fire(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_ACID_PUDDLE))//酸
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_acid(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_acid(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_COLD_PUDDLE))//魔法吹雪
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_cold(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_cold(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_ELEC_PUDDLE))//魔力の雷雲
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_elec(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_elec(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_POISON_PUDDLE))//毒沼
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_pois(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_pois(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_WATER))//水・沼
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 40;
			food = 2000;
		}
		else
		{
			prob = 20;
			food = 4000;

		}
	}
	else if (cave_have_flag_bold(y, x, FF_OIL_FIELD))//石油
	{
		prob = 35;
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_ICE_WALL))//氷塊
	{
		prob = 50;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_NEEDLE))//針
	{
		prob = 10;
		food = 2000;
	}
	else if (cave_have_flag_bold(y, x, FF_WALL))//壁
	{
		prob = 0;//飢餓モード以外では吸い込まない
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_PLATE)) //皿
	{
		prob = 90;
		food = 100;
	}
	else if (cave_have_flag_bold(y, x, FF_BOMB)) //爆弾
	{
		prob = 90;
		food = 5000;
	}

	else if (cave_have_flag_bold(y, x, FF_TRAP))//トラップ
	{
		//落とし穴を吸い込んで床になるのも変な話だがそもそも普通のトラップ解除もそうか
		prob = 75;
		food = 2000;
	}
	else
	{
		return FALSE;
	}

	if (YUMA_ULTIMATE_MODE) prob += 30;

	if (randint1(100) > prob) return FALSE;

	msg_format(_("%sを吸い込んだ！", "You consume %s!"), f_name + f_info[get_feat_mimic(&cave[y][x])].name);

	//地形変更と再描画
	cave_set_feat(y, x, feat_dirt);

	set_food(p_ptr->food + food);

	return TRUE;
}






 //尤魔特技「この世に存在してはならない暴食」
 //＠の位置を中心に渦巻状に1グリッドずつ座標を決めてそこのアイテム・モンスター・地形をyuma_swallow_***()で処理
 //何か一つでも吸い込んだらTRUEを返す
bool yuma_swallow(int rad)
{

	bool flag_eat_sth = FALSE;

	int xx, yy, i, j;

	//@の位置 モンスターはいるとしても騎乗中なので処理しない
	if (yuma_swallow_item(py, px, rad)) flag_eat_sth = TRUE;
	if (yuma_swallow_terrain(py, px, rad)) flag_eat_sth = TRUE;


	//@の周囲を渦巻き状に処理していく
	//モンスターを倒した直後にそのモンスターのドロップアイテムが吸われると使いづらいので、
	//アイテム→モンスター→地形の順に処理する

	//アイテム
	for (i = 1; i <= rad; i++)
	{
		//右下→右上から1グリッド下まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//右上→左上から1グリッド右まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左上→左下から1グリッド上まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左下→右下から1グリッド左まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
	}


	//モンスター
	for (i = 1; i <= rad; i++)
	{
		//右下→右上から1グリッド下まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//右上→左上から1グリッド右まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左上→左下から1グリッド上まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左下→右下から1グリッド左まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;

		}
	}

	//地形
	for (i = 1; i <= rad; i++)
	{
		//右下→右上から1グリッド下まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//右上→左上から1グリッド右まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左上→左下から1グリッド上まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//左下→右下から1グリッド左まで
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;

		}
	}


	return flag_eat_sth;

}



//v2.0.6 尤魔の最終モード発動
//process_player()の行動順判定からコマンド入力の間に行う。
//満腹度が飢餓状態になったとき、p_ptr->special_defenseにSD_UNIQUE_CLASS_POWERがセットされ操作不能になる。
//その間は吸い込み特技が勝手に発動し続ける。
//吸い込み特技が発動するとき行動順消費してTRUEを返す
bool check_yuma_ultimate_mode(void)
{
	int rad;

	static int countdown = 0;

	if (p_ptr->pclass != CLASS_YUMA) return FALSE;

	if (p_ptr->is_dead) return FALSE;

	//賭け試合の最中はなにもしない
	if (p_ptr->inside_battle) return FALSE;

	//飢餓状態のとき特殊フラグを立てる
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		if (!(p_ptr->special_defense & SD_UNIQUE_CLASS_POWER))
		{
			msg_print(_("あなたは周囲のものを無差別に吸い込み始めた！", "You start indiscriminately consuming everything in your vicinity!"));
			countdown = 100;
		}
		else
			msg_print(_("周囲のものを吸い込んでいる...", "You consume nearby items..."));

		p_ptr->special_defense |= SD_UNIQUE_CLASS_POWER;
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
	}

	//飢餓が解消されても特殊フラグが立っている間は先へ進む
	if (!(p_ptr->special_defense & SD_UNIQUE_CLASS_POWER)) return FALSE;

	Term_flush();
	p_ptr->redraw |= PR_MAP;
	redraw_stuff();

	set_cut(0);
	set_stun(0);

	//通常の倍の範囲
	rad = (3 + p_ptr->lev / 7) * 2;

	//尤魔の吸い込み特技が勝手に発動する。この特技で吸い込むものがなくなり、かつ満腹度が通常以上になったときフラグが外れる
	//満腹度が回復しなかったときはゲームオーバーまでこのまま
	//召喚持ちがたくさんいると無限ループになるので100行動で戻ることにする
	if (!yuma_swallow(rad) && p_ptr->food >= PY_FOOD_FAINT || (--countdown) <= 0)
	{
		countdown = 0;
		p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
		msg_print(_("あなたは意識を取り戻した。周囲は惨憺たる有様だ...", "You regain consciousness. Everything around you is in shambles..."));

		return FALSE;
	}

	p_ptr->energy_need = 100;
	return TRUE;

}






