#include "angband.h"

//アイテムカード使用時にTRUE
static bool use_itemcard = FALSE;

/*:::職業ごとの技はmコマンドで使うのとUコマンドで使うのを統合してJコマンドで使うようにする。*/
/*:::class_power_type型の配列で失敗率などの情報を記述し職業ごとに独自の関数で実行処理する。*/

/*:::
	class_power_type: 必ずレベル順にし、最後にレベル50より大きいダミー項目を入れる
	lev,cost,fail,use_hp,is_magic, stat,
	heavy_armor_fail,lev_inc_cost,name,
	tips
*/




//v1.1.90 山如



//カード販売所のリストにmagic_num2[32-41]を、カード買取の売却済みフラグにmagic_num1[32-35]を使っている.
//吟遊詩人系特技と被らないよう使うインデックスを+32している。

class_power_type class_power_sannyo[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード収納",
	"カードケースにアビリティカードを8種類まで収納する。カードケースに入れたカードを発動することはできないが、所持タイプのカードはケースの中でも効果を発揮する。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード確認",
	"カードケースの中を確認する。行動順を消費しない。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード取出し",
	"カードケースからカードを取り出す。" },

	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"煙草を消す",
	"煙草を吸っているときそれを止める。行動力を消費しない。" },

	{ 5,0,0,FALSE,TRUE,A_CHR,0,0,"一服",
	"煙草を吸い始める。吸っている間はMPの回復が早まる。" },

	{ 10,6,25,FALSE,TRUE,A_CHR,0,0,"動天の雲間草",
	"煙草を吸い始める。吸っている間は毎ターン近くのモンスターを恐怖させる。長く吸うほど効果が上昇する(最大400)" },

	{ 18,12,40,FALSE,TRUE,A_CHR,0,0,"驚愕の雲間草",
	"煙草を吸い始める。吸っている間は毎ターン近くのモンスターを混乱させる。長く吸うほど効果が上昇する(最大400)" },

	{ 24,20,50,FALSE,TRUE,A_CHR,0,0,"妖光輝く薄雪草",
	"煙草を吸い始める。吸っている間は毎ターン近くのモンスターを朦朧とさせる。長く吸うほど効果が上昇する(最大400)" },

	{ 30,0,75,FALSE,FALSE,A_CHR,0,0,"カード買い取り",
	"隣接したモンスター一体からカードを買い取る。カードを持っているモンスターはそのカードの題材となった幻想郷の住人のみ。また同じカードを買えるのは一度のみ。" },

	{ 35,30,60,FALSE,TRUE,A_CHR,0,0,"妖魔犇めく薄雪草",
	"煙草を吸い始める。吸っている間は毎ターン近くのモンスターを減速させる。長く吸うほど効果が上昇する(最大400)" },

	{ 40,48,75,FALSE,TRUE,A_CHR,0,0,"殺戮の駒草",
	"煙草を吸い始める。吸っている間は毎ターン近くのモンスターに強力な精神攻撃を行う。長く吸うほど効果が上昇する(最大400)" },

	{ 45,80,85,FALSE,TRUE,A_CHR,0,0,"殺戮の山の女王",
	"煙草を吸い始める。吸っている間は狂戦士状態になり、さらに身体能力が大幅に上昇する。吸い始めのときに限りHPが大幅に回復する。" },




	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_sannyo(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;

	int power_add = 5 + plev / 2;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //見るだけなので行動順消費なし

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}

	//煙草を止める　行動順消費しない
	case 3:
	{
		if (only_info) return "";
		stop_singing();
		return NULL;

	}
	break;
	case 4://一服

	{
		if (only_info) return format("回復強化");
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_SMOKE, SPELL_CAST);

		break;
	}
	break;

	case 5:
	{
		int power = 20 + plev * 2;
		if (only_info) return format("効力:%d+%d/turn",power,power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_FEAR, SPELL_CAST);

		break;
	}
	break;

	case 6:
	{
		int power = 30 + plev * 2;
		if (only_info) return format("効力:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_CONF, SPELL_CAST);

		break;
	}
	break;
	case 7:
	{
		int power = 40 + plev * 2;
		if (only_info) return format("効力:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_STUN, SPELL_CAST);

		break;
	}
	break;




	case 8:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;


	case 9:
	{
		int power = 40 + plev * 2;
		if (only_info) return format("効力:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_SLOW, SPELL_CAST);

		break;
	}
	break;
	case 10:
	{
		int power = 50 + plev * 2;
		if (only_info) return format("効力:%d+%d/turn", power, power_add);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_MINDBLAST, SPELL_CAST);

		break;
	}
	break;
	case 11:
	{
		if (only_info) return format("");
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_SANNYO_BERSERK, SPELL_CAST);

		break;
	}
	break;


	default:
		if (only_info) return format("未実装");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}





//v1.1.89 百々世
//妖怪達のシールドメソッドにtim_general[0],スネークイーターに[1],ドラゴンイーターに[2],蠱毒のグルメに[3]を使用

class_power_type class_power_momoyo[] =
{


	{ 10,20,30,FALSE,TRUE,A_CHR,0,0,"ケイブスウォーマー",
	"大量の虫を友好的な状態で召喚する。" },

	{ 15,24,50,FALSE,TRUE,A_DEX,0,0,"スネークイーター",
	"一時的にヘビ系モンスターに対する特効能力を獲得し、さらにモンスターの噛みつき・締め付け攻撃を回避するようになる。同系統の特技と併用できない。" },

	{ 20,40,45,TRUE,FALSE,A_CON,0,20,"マインブラスト",
	"自分の周囲に対し汚染属性攻撃を行う。威力はHPに比例する。レベル40以上で視界内全てが対象になる。" },

	{ 25,40,60,FALSE,TRUE,A_STR,0,0,"ドラゴンイーター",
	"一時的にドラゴン系モンスターに対する特効能力を獲得し、さらにモンスターからのオーラダメージをMPとして吸収するようになる。同系統の特技と併用できない。" },

	{ 30,64,70,FALSE,TRUE,A_DEX,0,0,"妖怪達のシールドメソッド",
	"一時的に壁を掘りながら移動し、さらに自分が受けるダメージを2/3に軽減するようになる。" },

	{ 35,80,75,FALSE,TRUE,A_CON,0,0,"蠱毒のグルメ",
	"一時的にモンスターを倒したときHPと満腹度を大幅に吸収し、さらに経験値を多く得られるようになる。同系統の特技と併用できない。" },

	{ 40,120,85,FALSE,TRUE,A_STR,0,0,"積もり続けるマインダンプ",
	"周囲の地形を岩石にする。" },

	{ 45,-100,80,FALSE,TRUE,A_CHR,0,0,"蟲姫さまの輝かしく落ち着かない毎日",
	"MPを全て(最低100)消費し、視界内すべてに強力な魔力属性攻撃を行う。" },



	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_momoyo(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{


	case 0:
	{
		bool flag = FALSE;
		int count = 2 + plev / 6;
		if (only_info) return "";


		for (i = 0; i<count; i++)
		{
			int s_mode;
			s_mode = one_in_(4) ? SUMMON_ANT : SUMMON_SPIDER;

			if ((summon_specific(-1, py, px, p_ptr->lev, s_mode, (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))))flag = TRUE;
		}
		if (flag)
			msg_print("大量の蟲達を呼び出した！");
		else
			msg_print("何も現れなかった。");

		break;
	}

	case 1:
	case 3:
	case 5:
	{
		int v;
		int base = plev / 3;
		if (only_info) return format("期間:%d+1d%d", base, base);

		v = base + randint1(base);

		if (num == 1)
		{
			set_tim_general(0, TRUE, 2, 0);
			set_tim_general(0, TRUE, 3, 0);
			set_tim_general(v, FALSE, 1, 0);
		}
		else if(num == 3)
		{
			set_tim_general(0, TRUE, 1, 0);
			set_tim_general(0, TRUE, 3, 0);
			set_tim_general(v, FALSE, 2, 0);
		}
		else
		{
			set_tim_general(0, TRUE, 1, 0);
			set_tim_general(0, TRUE, 2, 0);
			set_tim_general(v, FALSE, 3, 0);

		}

		break;
	}


	case 2:
	{
		int damage;

		if (p_ptr->lev < 40 )
		{
			damage = p_ptr->chp;

			if (damage > 1600) damage = 1600;

			if (only_info) return format("損傷:～%d", damage/2);

			msg_format("あなたの体から毒素が噴き出した！");
			project(0, 6, py, px, damage, GF_POLLUTE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

		}
		else
		{
			damage = p_ptr->chp / 3;
			if (only_info) return format("損傷:%d", damage);

			msg_print("ムカデの毒が大地を侵した！");
			project_hack2(GF_POLLUTE, 0, 0, damage);
		}

	}
	break;


	case 4:
	{
		int v;
		int base = 20;
		if (only_info) return format("期間:%d+1d%d", base, base);

		v = base + randint1(base);

		set_tim_general(v, FALSE, 0, 0);
		set_magicdef(v, FALSE);

		break;
	}


	case 6:
	{
		int rad = 2;
		if (only_info) return format("半径:%d", rad);

		msg_print("大量の岩石が積み上がった！");
		project(0, rad, py, px, 10, GF_STONE_WALL, (PROJECT_GRID), -1);
		break;
	}

	case 7:
	{
		int dam = p_ptr->csp * 2;
		if (only_info) return format("損傷:%d", dam);

		msg_print("膨大な魔力を解き放った！");
		project_hack2(GF_MANA, 0, 0, dam);

		break;
	}


	default:
		if (only_info) return format("未実装");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.88 たかね
//カード販売所のリストにmagic_num2[0-9]を、カード買取の売却済みフラグにmagic_num1[0-3]を使っている.
class_power_type class_power_takane[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード収納",
	"カードケースにアビリティカードを16種類まで収納する。カードケースに入れたカードを発動することはできないが、所持タイプのカードはケースの中でも効果を発揮する。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード確認",
	"カードケースの中を確認する。行動順を消費しない。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード取出し",
	"カードケースからカードを取り出す。" },

	{ 15,20,50,FALSE,FALSE,A_INT,0,0,"鑑識",
	"アイテムをひとつ鑑定する。" },

	{ 20,0,50,FALSE,FALSE,A_CHR,0,0,"カード買い取り",
	"隣接したモンスター一体からカードを買い取る。カードを持っているモンスターはそのカードの題材となった幻想郷の住人のみ。また同じカードを買えるのは一度のみ。" },

	{ 25,20,55,FALSE,FALSE,A_DEX,0,0,"木隠れの技術",
	"一時的に隠密能力が少し上がる。森の中にいると大幅に上がる。" },

	{ 30,30,65,FALSE,TRUE,A_INT,0,0,"最奥の森域",
	"自分のいるグリッドの地形を森にする。" },

	{ 35,30,60,FALSE,TRUE,A_CHR,0,0,"グリーンスパイラル",
	"隣接したモンスター全てに遅鈍属性の攻撃を行う。自分が森の中にいないと使えない。" },

	{ 40,75,75,FALSE,FALSE,A_INT,0,0,"真・最奥の森域",
	"自分を中心とした広範囲の地形を森にする。" },

	{ 45,90,85,FALSE,FALSE,A_DEX,0,0,"真・木隠れの技術",
	"一時的に周囲の敵から認識されづらい超隠密状態になる。ただし森の中にいないと効果がない。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_takane(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //見るだけなので行動順消費なし

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		if (!ident_spell(FALSE)) return NULL;

		break;
	}

	case 4:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;



	case 5://木隠れの技術 tim_general[0]を使う
	{
		int base = plev;
		if (only_info) return format("期間:%d+1d%d", base,base);

		set_tim_general(base+randint1(base), FALSE, 0, 0);

	}
	break;



	case 6:
	case 8:
	{
		int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
		int rad=0;

		if (num == 8) rad = 2;
		if (only_info) return format("半径:%d", rad);

		//GF_MAKE_TREEはなぜか知らんが＠のいる場所に木を生やさないのでここで生やす
		if (cave_clean_bold(py, px))
		{
			cave_set_feat(py, px, feat_tree);
			msg_print("辺りは瑞々しい森の空気に包まれた。");
		}

		if(num == 8) project(0, rad, py, px, 0, GF_MAKE_TREE, flg, -1);

		//森地形に入ったことによるパラメータ再計算用
		p_ptr->update |= PU_BONUS;

	}
	break;


	//グリーンスパイラル
	case 7:
	{
		int base = p_ptr->lev * 2 + chr_adj * 10 / 3;

		if (only_info) return format("損傷:%d", base / 2);

		//技使用条件関数にも同じこと書いたはずなんだが、働かないことがある？ウィザードモードでもないのによくわからん
		if (!cave_have_flag_bold((py), (px), FF_TREE))
		{
			msg_print("この特技は森の中でないと使えない。");
			return NULL;
		}

		msg_format("木の葉が嵐のように渦巻いた！");
		project(0, 1, py, px, base, GF_INACT, PROJECT_KILL, -1);

	}
	break;


	case 9:
	{
		int base = 25;

		if (only_info) return format("期間:%d+1d%d", base, base);

		set_tim_superstealth(base + randint1(base), FALSE, SUPERSTEALTH_TYPE_FOREST);

	}
	break;


	default:
		if (only_info) return format("未実装");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.87 カード売人
//カード販売所のリストにmagic_num2[0-9]を、カード買取の売却済みフラグにmagic_num1[0-3]を使っている.
class_power_type class_power_card_dealer[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード収納",
	"カードケースにアビリティカードを8種類まで収納する。カードケースに入れたカードを発動することはできないが、所持タイプのカードはケースの中でも効果を発揮する。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード確認",
	"カードケースの中を確認する。行動順を消費しない。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"カード取出し",
	"カードケースからカードを取り出す。" },

	{ 20,20,70,FALSE,FALSE,A_INT,0,0,"鑑識",
	"アイテムをひとつ鑑定する。" },

	{ 30,0,75,FALSE,FALSE,A_CHR,0,0,"カード買い取り",
	"隣接したモンスター一体からカードを買い取る。カードを持っているモンスターはそのカードの題材となった幻想郷の住人のみ。また同じカードを買えるのは一度のみ。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_card_dealer(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //見るだけなので行動順消費なし

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}


	case 3:
	{
		if (only_info) return format("");

		if (!ident_spell(FALSE)) return NULL;

		break;
	}

	case 4:
	{
		if (only_info) return format("");

		if (!buy_abilitycard_from_mon()) return NULL;

	}
	break;

	default:
		if (only_info) return format("未実装");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}






/*:::v1.1.82 弾幕研究家専用技*/
//magic_num1[]の1-96とmagic_num2[]の1-32を特技習得度に、
//magic_num1[0]を集中のモードに使用する。

class_power_type class_power_researcher[] =
{

	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"特技",
	"モンスターから習得した特技を使用する。特技を習得するにはモンスターが特技を使うところを何度も近くで見ればよい。ただしモンスターが自分をターゲットせずに使った特技では習熟度が一定以上に上がらない。" },

	{ 20,20,30,FALSE,FALSE,A_INT,0,0,"モンスター調査",
	"周囲のモンスターの能力や耐性などを知る。" },

	{ 30,30,60,TRUE,FALSE,A_CON,0,10,"気合い溜め",
	"次のターンに使用されたHP依存の特技の威力が2倍になる。" },

	{ 40,30,80,FALSE,TRUE,A_WIS,0,0,"魔力集中",
	"次のターンに使用された一部の特技が力強くなりダメージが上昇する。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_researcher(int num, bool only_info)
{

	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		if (only_info) return "";

		if (!cast_monspell_new()) return NULL;
		break;
	}

	case 1:
	{
		if (only_info) return format("");
		msg_print("あなたは周囲のモンスターの調査を始めた...");
		probing();
		break;
	}
	case 2:
	{

		if (only_info) return format("");

		do_cmd_concentrate(CONCENT_KANA);//モード2をブレス強化、モード3をPOWERFULフラグ付与にする
		break;
	}
	case 3:
	{

		if (only_info) return format("");

		do_cmd_concentrate(CONCENT_SUWA);
		break;
	}


	default:
		{
			if (only_info) return "未実装";

			msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
			return NULL;
		}

	}

return "";

}



/*:::v1.1.77 お燐(専用性格)専用技*/
//追跡モンスターのm_idxを格納するためにp_ptr->magic_num1[0]を使用する
class_power_type class_power_orin2[] =
{
	{ 5,10,20,FALSE,FALSE,A_INT,0,1,"聞き込み",
	"周囲のモンスターを感知する。レベル15でトラップ、レベル25でアイテム、レベル35で地形も感知する。効果範囲はレベルアップに伴い広がる。" },

	{ 10,0,0,FALSE,TRUE,A_CON,0,0,"猫化",
	"猫の姿になる。変身中は移動速度と隠密能力が飛躍的に上昇する。しかし戦闘能力が低下し、ほとんどの装備品が一時無効化され、巻物・魔道具・魔法・特技を使えなくなる。Uコマンドから人型に戻ることができる。" },

	{ 20,20,40,FALSE,FALSE,A_DEX,0,1,"追跡",
	"隣接したモンスター一体を指定し、そのモンスターを常に感知し続ける。レベル30以上のときそのモンスターからの攻撃を受ける直前に自動的に短距離テレポートを試みる。テレポートにはMP30を消費し、モンスターのレベルが高いとテレポートに失敗しやすい。" },




	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



cptr do_cmd_class_power_aux_orin2(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
		{
			int rad = 15;
			if (plev > 24) rad += 10;
			if (plev > 34) rad += 15;
			if (only_info) return format("範囲:%d", rad);

			if(plev < 25)
				msg_print("あなたは周囲に漂う幽霊に聞き込みを行った。");
			else if(plev < 35)
				msg_print("あなたは配下の霊たちを偵察に出した。");
			else
				msg_print("無数の霊たちがあなたの元に集い情報を届けた！");

			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			if (plev > 14) detect_traps(rad, TRUE);
			if (plev > 24) detect_objects_normal(rad);
			if (plev > 34) map_area(rad);
		}
		break;


	case 1:
	{
		if (only_info) return format("期間:最長1000ターン");
		if (!set_mimic(1000, MIMIC_CAT, FALSE)) return NULL;

		break;
	}

	//追跡
	case 2:
	{
		int i;
		monster_type *m_ptr;
		int y, x;

		if (only_info) return  format("");

		//隣接モンスター指定
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int damage;
			int o_idx;
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);

			if (cave[y][x].m_idx == p_ptr->magic_num1[0])
			{
				msg_format("すでに追跡中だ。");
				return NULL;
			}
			else
			{
				msg_format("あなたは%sの追跡を開始した。", m_name);
				p_ptr->magic_num1[0] = cave[y][x].m_idx;
			}

		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}


	}
	break;



	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}




/*:::v1.1.74 袿姫特技*/
class_power_type class_power_keiki[] =
{

	{ 5,0,0,FALSE,TRUE,A_DEX,0,0,"造形術",
	"装備品をひとつ消費し、付与された能力をエッセンスとして抽出する。また抽出したエッセンスを別の装備品にひとつ付与する。" },
	{ 10,20,40,FALSE,FALSE,A_INT,0,0,"鑑識",
	"アイテムをひとつ鑑定する。レベル30以降はアイテムの完全な情報を得る。" },
	{ 15,30,45,FALSE,TRUE,A_CHR,0,0,"解呪",
	"装備中の呪われた装備品を解呪する。レベル35で強力な呪いも解呪するようになる。" },
	{ 20,0,60,FALSE,FALSE,A_DEX,0,0,"偶像人馬造形術",
	"周囲に埴輪の配下を召喚する。防御エッセンスを500消費する。" },

	{ 24,0,65,FALSE,FALSE,A_DEX,0,0,"真円造形術",
	"自分を中心とした8グリッドに隣接攻撃を行う。オーラによる反撃を受けない。攻撃エッセンスを500消費する。" },

	{ 28,20,70,FALSE,FALSE,A_INT,0,0,"造形神の目",
	"周辺の地形とアイテム・モンスター・トラップを感知し、さらに視界内のモンスターを調査する。" },

	{ 32,0,70,FALSE,FALSE,A_DEX,0,0,"方形造形術",
	"自分を中心とした8グリッドに「岩の壁」を生成する。防御エッセンスを1000消費する。" },

	{ 36,0,75,FALSE,FALSE,A_DEX,0,0,"線型造形術",
	"直線状のモンスターにダメージを与える。威力は隣接攻撃力と同等。攻撃エッセンスを1000消費する。刃のついた武器を装備していないと使えない。" },

	{ 40,0,80,FALSE,TRUE,A_DEX,0,0,"鬼形造形術",
	"耐性エッセンスをランダムに50消費し、このフロア限定の特別な配下を生成する。配下の能力は消費したエッセンスにより決まる。" },

	{ 45,0,80,FALSE,TRUE,A_DEX,0,0,"イドラディアボルス",
	"一定時間全能力を強化し、さらに種族が一時的に「魔王」になる。能力エッセンス6種類を50ずつ消費する。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_keiki(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return "";
		screen_save();
		clear_bldg(2, 22);
		do_cmd_kaji(FALSE);

		screen_load();

	}
	break;
	case 1:
	{
		if (only_info) return format("");

		if (plev < 30)
		{
			if (!ident_spell(FALSE)) return NULL;
		}
		else
		{
			if (!identify_fully(FALSE)) return NULL;
		}
		break;
	}
	case 2:
	{
		bool success = FALSE;
		if (only_info) return format("");

		if (plev < 35)
		{
			if (remove_curse()) success = TRUE;
		}
		else
		{
			if (remove_all_curse())  success = TRUE;
		}
		if (success) msg_print("装備品の呪縛を消し去った。");
		else msg_print("何も起こらなかった。");

		break;
	}

	case 3:
	{
		bool flag = FALSE;
		int i;
		int count = 2 + plev / 8;
		int mode;

		if (only_info) return "";

		if (p_ptr->magic_num1[TR_ES_AC] < 500)
		{
			msg_print("その特技を使う準備が整っていない。");
			return NULL;
		}
		p_ptr->magic_num1[TR_ES_AC] -= 500;

		for (i = 0; i<count; i++)
		{
			int r_idx;

			switch (randint1(3))
			{
			case 1:
				r_idx = plev > 39 ? MON_HANIWA_F2 : MON_HANIWA_F1;
				break;
			case 2:
				r_idx = plev > 39 ? MON_HANIWA_A2 : MON_HANIWA_A1;
				break;
			default:
				r_idx = plev > 39 ? MON_HANIWA_C2 : MON_HANIWA_C1;
				break;
			}

			if (summon_named_creature(0, py, px, r_idx, PM_FORCE_PET)) flag = TRUE;

		}
		if (flag)
			msg_print("埴輪たちを創り出した。");
		else
			msg_print("埴輪を作れる場所がない。");

		break;
	}

	case 4:
	{
		int rad = 1;
		if (only_info) return format("");

		if (p_ptr->magic_num1[TR_ES_ATTACK] < 500)
		{
			msg_print("その特技を使う準備が整っていない。");
			return NULL;
		}
		p_ptr->magic_num1[TR_ES_ATTACK] -= 500;

		flag_friendly_attack = TRUE;

		if(p_ptr->do_martialarts)
			msg_format("あなたの腕は美しい弧を描いた。");
		else
			msg_format("あなたの武器が美しい弧を描いた。");


		project(0, rad, py, px, 100, GF_SOULSCULPTURE, PROJECT_KILL, -1);
		flag_friendly_attack = FALSE;
	}
	break;

	case 5:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return "";

		msg_format("あなたは周囲を見回した。");
		map_area(rad);
		detect_traps(rad, TRUE);
		detect_objects_normal(rad);
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
		probing();
		break;
	}

	case 6:
	{
		if (only_info) return format("");

		if (!use_itemcard)
		{
			if (p_ptr->magic_num1[TR_ES_AC] < 1000)
			{
				msg_print("その特技を使う準備が整っていない。");
				return NULL;
			}
			p_ptr->magic_num1[TR_ES_AC] -= 1000;
		}

		msg_format("あなたは瞬く間に壁を築いた。");
		wall_stone();
	}
	break;


	case 7:
	{
		int damage = 0;
		int range = 5 + plev / 10;

		if (object_has_a_blade(&inventory[INVEN_RARM])) damage += calc_weapon_dam(0);
		if (object_has_a_blade(&inventory[INVEN_LARM])) damage += calc_weapon_dam(1);

		if (only_info) return format("射程:%d 損傷:%d", range, damage);

		if (!damage)
		{
			msg_print("刃のついた武器を装備していない。");
			return NULL;
		}
		if (p_ptr->magic_num1[TR_ES_ATTACK] < 1000)
		{
			msg_print("その特技を使う準備が整っていない。");
			return NULL;
		}

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;

		p_ptr->magic_num1[TR_ES_ATTACK] -= 1000;


		msg_print("大地を一直線に切り裂いた！");
		fire_beam(GF_MISSILE, dir, damage);
		break;
	}

	case 8: //ランダムユニーク配下生成
			//フロアをまたいで連れ歩けるようにしたかったが、それだとフロア移動のときにランダムユニークが生成されたら上書きされて消えるらしい。対応がちょっとややこしそうなのでパス
	{
		int r_idx;
		int i;
		bool flag1 = TRUE;
		bool flag2 = TRUE;
		bool flag3 = TRUE;
		monster_type *m_ptr;
		monster_race *r_ptr;
		u32b mode = PM_EPHEMERA;
		int cand_num = 0;

		if (only_info) return "";

		//使われていないランダムユニークIDXを探す
		for (i = 1; i < m_max; i++)
		{
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_1) flag1 = FALSE;
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_2) flag2 = FALSE;
			if (m_list[i].r_idx == MON_RANDOM_UNIQUE_3) flag3 = FALSE;

		}
		if (flag1)
			r_idx = MON_RANDOM_UNIQUE_1;
		else if (flag2)
			r_idx = MON_RANDOM_UNIQUE_2;
		else if (flag3)
			r_idx = MON_RANDOM_UNIQUE_3;
		else
			r_idx = 0;

		//消費するエッセンスを決定し、エッセンスを消費してランダムユニーク生成タイプ決定
		if (r_idx)
		{
			int tmp_sum = 0;


			for (i = TR_RES_ACID; i <= TR_RES_TIME; i++)
			{
				if (p_ptr->magic_num1[i] < 50) continue;
				tmp_sum += p_ptr->magic_num1[i];
				cand_num++;
			}
			tmp_sum = randint1(tmp_sum);
			for (i = TR_RES_ACID; i < TR_RES_TIME; i++)
			{
				if (p_ptr->magic_num1[i] < 50) continue;
				tmp_sum -= p_ptr->magic_num1[i];

				if (tmp_sum <= 0) break;
			}

			if(cand_num)special_idol_generate_type = i;

		}

		if(!cand_num)
		{
			msg_print("その特技を使う準備が整っていない。");
			return NULL;
		}

		msg_print("あなたは新作の製作に取り掛かった...");

		if (r_idx && special_idol_generate_type && summon_named_creature(0, py, px, r_idx, mode))
		{
			cptr adj;
			char name_total[256];

			switch (special_idol_generate_type)
			{
			case TR_RES_ACID:
				adj = "爛れた"; break;
			case TR_RES_ELEC:
				adj = "火花を散らす"; break;
			case TR_RES_FIRE:
				adj = "燃え盛る"; break;
			case TR_RES_COLD:
				adj = "凍り付いた"; break;
			case TR_RES_POIS:
				adj = "毒々しい"; break;
			case TR_RES_LITE:
				adj = "眩い"; break;
			case TR_RES_DARK:
				adj = "漆黒の"; break;
			case TR_RES_SHARDS:
				adj = "水晶の"; break;
			case TR_RES_SOUND:
				adj = "震える"; break;
			case TR_RES_NETHER:
				adj = "不吉な"; break;
			case TR_RES_WATER:
				adj = "湿った"; break;
			case TR_RES_CHAOS:
				adj = "ねじれた"; break;
			case TR_RES_DISEN:
				adj = "さえない"; break;
			case TR_RES_HOLY:
				adj = "神々しい"; break;
			default:
				adj = "奇妙な";
			}
			msg_format("あなたは%s偶像を造り上げた！",adj);
			//ランダムユニークの名前変更
			sprintf(name_total, "%s偶像", adj);
			my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1], name_total, sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]) - 2);

			p_ptr->magic_num1[special_idol_generate_type] -= 50;
		}
		else
		{
			msg_print("偶像の制作に失敗した。");
		}
		special_idol_generate_type = 0;

		break;

	}




	case 9:
	{
		int i;
		int percentage;
		int base = 20;
		int time;
		bool flag_ok = TRUE;
		if (only_info) return format("期間:%d+1d%d", base, base);


		//腕力～魅力のエッセンスの必要数をチェック
		for (i = 0; i < 6; i++)
		{
			if (p_ptr->magic_num1[i] < 50)flag_ok = FALSE;
		}
		if (!flag_ok)
		{
			msg_print("その特技を使う準備が整っていない。");
			return NULL;
		}
		for (i = 0; i < 6; i++)
		{
			p_ptr->magic_num1[i] -= 50;
		}

		msg_print("あなたは戦いの準備を整えた。");

		percentage = p_ptr->chp * 100 / p_ptr->mhp;
		time = base + randint1(base);
		set_tim_addstat(A_STR, 107, time, FALSE);
		set_tim_addstat(A_INT, 107, time, FALSE);
		set_tim_addstat(A_WIS, 107, time, FALSE);
		set_tim_addstat(A_DEX, 107, time, FALSE);
		set_tim_addstat(A_CON, 107, time, FALSE);
		set_tim_addstat(A_CHR, 107, time, FALSE);
		set_mimic(time, MIMIC_DEMON_LORD, FALSE);

		p_ptr->chp = p_ptr->mhp * percentage / 100;
		p_ptr->redraw |= (PR_HP);
		redraw_stuff();


		break;
	}
	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}



/*:::v1.1.73 八千慧特技*/
class_power_type class_power_yachie[] =
{

	{ 1,5,20,FALSE,FALSE,A_DEX,0,0,"殺気感知",
	"精神を持つ周囲のモンスターを感知する。" },

	{ 7,10,20,FALSE,TRUE,A_CHR,0,0,"勧誘",
	"隣接したモンスター一体を勧誘して配下にする。何度も実行するとユニークモンスターにも効く可能性があるがクエストの打倒対象モンスターには無効。" },

	{ 15,20,30,FALSE,FALSE,A_INT,0,0,"搦手の畜生",
	"指定したモンスターを他のモンスターから敵対されるように仕向ける。ユニークモンスターや高レベルなモンスターには効きにくい。" },

	{ 20,20,40,FALSE,FALSE,A_CHR,0,0,"配下召喚",
	"配下の動物霊を召喚する。レベル35以上で複数体召喚する。" },

	{ 25,20,50,FALSE,FALSE,A_CON,0,5,"火炎のブレス",
	"現在HPの1/4の威力の火炎属性のブレスを吐く。" },

	{ 30,45,50,FALSE,FALSE,A_INT,0,0,"亀甲地獄",
	"自分を中心に遅鈍属性のボールを発生させる。" },

	{ 35,160,80,FALSE,FALSE,A_INT,0,0,"搦手の犬畜生",
	"このフロアで自分に対して敵対召喚されてくるモンスターを買収しようと試みる。買収に成功すると所持金が減少する。" },

	{ 40,50,70,FALSE,TRUE,A_STR,0,0,"龍紋弾",
	"水属性の強力なボールを放つ。" },

	{ 45,66,75,FALSE,TRUE,A_INT,0,0,"搦手の鬼畜生",
	"フロアにいる複数の配下を指定したモンスターの隣にテレポートさせ、さらにその配下をすぐに行動できるようにする。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_yachie(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("範囲:%d", rad);
		detect_monsters_mind(rad);
	}
	break;


	case 1: //勧誘
	{
		int y, x;
		int power = plev + chr_adj;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("効力:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);


			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			msg_format("あなたは%sを勧誘した。", m_name);

			if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags1 & RF1_QUESTOR)
			{
				msg_format("%sは聞く耳を持たない！", m_name);
			}
			else if (!(m_ptr->mflag & MFLAG_SPECIAL) && (randint1(power) < r_ptr->level || r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2))
			{
				msg_format("%sに断られた。", m_name);

				//確率で次の勧誘が確定成功
				if (randint1((r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) ? r_ptr->level * 3 : r_ptr->level) < plev)
				{
					msg_format("しかし%sの意志が弱くなってきたのを感じる。", m_name);
					m_ptr->mflag |= MFLAG_SPECIAL;
				}
			}
			else
			{
				msg_format("%sはあなたに従った。", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;

	case 2://搦手の畜生
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("");


		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("視界内のターゲットを明示的に指定しないといけない。");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			int chance = plev * 2 + chr_adj * 4;
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			if (r_ptr->flags1 & RF1_QUESTOR) chance /= 2;
			else if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 2 / 3;

			if (m_ptr->mflag & MFLAG_ISOLATION)
			{
				msg_format("既に効いている。");

			}
			else if (chance <= r_ptr->level)
			{
				msg_format("%sに対して付け込む余地はないようだ。", m_name);
			}
			else if (randint1(chance) < r_ptr->level)
			{
				msg_format("%sに対する工作に失敗した！", m_name);
			}
			else
			{
				if(chance > 200)
					msg_format("%sを巨大な陰謀の黒幕に仕立て上げた！", m_name);
				else if(chance > 100)
					msg_format("%sの事実無根のスキャンダルを流布した！", m_name);
				else
					msg_format("フロアに%sの悪評を流した！", m_name);

				m_ptr->mflag |= MFLAG_ISOLATION;
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}

		break;
	}


	case 3:
	{
		int i;
		int summon_num = 1;
		if (plev > 34) summon_num += 1 + (plev - 35) / 2;
		if (only_info) return format("召喚数:%d", summon_num);
		int mode = PM_FORCE_PET;

		msg_print("あなたは配下の動物霊を招集した！");

		for (i = 0; i < summon_num; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx = MON_ANIMAL_G_KIKETSU;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 3, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (mode));
		}

	}
	break;

	case 4:
	{
		int dam;
		dam = p_ptr->chp / 4;
		if (dam<1) dam = 1;
		if (dam > 1600) dam = 1600;

		if (only_info) return format("損傷:%d", dam);
		if (!get_aim_dir(&dir)) return FALSE;
		msg_print("あなたは炎を吐いた！");
		fire_ball(GF_FIRE, dir, dam, (p_ptr->lev > 35 ? -3 : -2));
		break;
	}

	case 5:
	{
		int rad = plev / 7 - 1;
		int base = p_ptr->lev * 3 + chr_adj * 3;

		if (use_itemcard && base < 100) base = 200;

		if (only_info) return format("損傷:%d", base / 2);
		msg_format("あなたは周囲の動きを封じようと試みた。");
		project(0, rad, py, px, base, GF_INACT, PROJECT_KILL, -1);
	}
	break;

	case 6:
	{
		if (only_info) return format("");

		if (p_ptr->magic_num1[0])
		{
			msg_print("買収工作を止めた。");
			p_ptr->magic_num1[0] = 0;
		}
		else
		{
			msg_print("あなたは配下の動物霊たちに買収工作を命じた...");
			p_ptr->magic_num1[0] = 1;
		}
		p_ptr->redraw |= PR_STATUS;
	}
	break;

	case 7:
	{
		int dice = 5 + chr_adj / 7;
		int sides = plev * 2;

		if (only_info) return format("損傷:%dd%d", dice,sides);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("巨大な水弾を叩きつけた！");

		fire_ball(GF_WATER, dir, damroll(dice,sides), 4);

		break;
	}

	case 8:
	{
		int mon_max = 6 ;//最大6体
		int pick_num = 0;
		int i;
		int cnt = 0;
		int idx[6]; //最大6体
		if (only_info) return format("対象:%d体",mon_max);

		//do_new_spell_necromancy()の「使嗾」の魔法をコピペして少し改変
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!is_pet(m_ptr)) continue;
			pick_num++;
			//呼び出せる以上の数の配下がいたら候補から適当に入れ替える
			if (cnt<mon_max)
				idx[cnt++] = i;
			else if(randint1(pick_num) > cnt)
				idx[randint0(mon_max)] = i;
		}
		if (!pick_num)
		{
			msg_format("フロアに配下がいない。");
			return NULL;
		}
		if (!get_aim_dir(&dir)) return NULL;

		msg_format("あなたは配下たちに奇襲を命じた！");
		for (i = 0; i<cnt&&i<mon_max; i++)
		{
			monster_type *m_ptr = &m_list[idx[i]];
			teleport_monster_to(idx[i], target_row, target_col, 100, TELEPORT_PASSIVE);
			m_ptr->energy_need = 0;

		}

	}
	break;

	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::v1.1.71 早鬼特技*/
class_power_type class_power_saki[] =
{

	{ 1,5,20,FALSE,FALSE,A_DEX,0,0,"殺気感知",
	"精神を持つ周囲のモンスターを感知する。" },

	{ 8,10,22,FALSE,FALSE,A_CHR,0,0,"フォロミーアンアフライド",
	"隣接したモンスター一体を勧誘して配下にする。恐怖状態のモンスターに効果が高い。ユニークモンスターには無効。" },

	{ 14,18,35,FALSE,TRUE,A_DEX,0,0,"ライトニングネイ",
	"視界内のランダムなモンスターへ全弾射撃を行う。射撃は電撃属性ビームに変化する。一部の特殊な銃器には使用できない。" },

	{ 20,30,45,FALSE,TRUE,A_CON,0,0,"デンスクラウド",
	"一時的に元素耐性を得る。レベル40以上ではACも上昇する。" },

	{ 25,30,50,FALSE,FALSE,A_CHR,0,0,"鬼形のホイポロイ",
	"配下の動物霊を複数体召喚する。レベル40以上では加速された状態で呼び出す。" },

	{ 30,70,60,FALSE,FALSE,A_STR,0,0,"町移動",
	"行ったことのある町に移動する。地上でしか使えない。" },

	{ 35,50,70,FALSE,FALSE,A_DEX,0,0,"スリリングショット",
	"視界内のランダムなモンスターへ全弾射撃を行い、さらに恐怖させる。一部の特殊な銃器には使用できない。" },

	//案としては去年から考えていたのだが一応自粛
	//{ 35,66,70,FALSE,TRUE,A_CON,0,0,"ビーストエピデミシティ",
	//"視界内のモンスターに対し毒属性の攻撃を行いさらに混乱・朦朧させる。視界内のモンスターが密集しているほど威力が高くなる。" },

	{ 40,80,75,TRUE,FALSE,A_DEX,50,20,"トライアングルチェイス",
	"指定したモンスターの隣まで一瞬で移動し、さらに攻撃して離脱する。離脱に失敗することがある。装備品が重いと特技の使用に失敗しやすい。" },

	{ 43,120,80,FALSE,FALSE,A_STR,0,0,"マッスルエクスプロージョン",
	"視界内の全てに対し気属性で攻撃する。さらに腕力を一時的に限界以上に上昇させる。威力は現在HPの半分になる。" },

	{ 47,160,90,FALSE,TRUE,A_CHR,0,0,"ブラックペガサス流星弾",
	"威力2倍の射撃を行う。射撃は隕石属性ロケットに変化する。一部の特殊な銃器では効果がない。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_saki(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("範囲:%d", rad);
		detect_monsters_mind(rad);
	}
	break;

	case 1: //勧誘
	{
		int y, x;
		int power = plev * 2 + chr_adj * 2;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (use_itemcard && power < 100) power = 100;

		if (only_info) return format("効力:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//恐怖しているモンスターに効果増
			if (MON_MONFEAR(m_ptr)) power *= 3;

			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			if(!one_in_(3) || use_itemcard)
				msg_format("あなたは%sを勧誘した。", m_name);
			else
				msg_print("どうだ、勁牙組に入って貰えないだろうか？");

			if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
			{
				msg_format("%sは聞く耳を持たない！", m_name);
			}
			else if (randint1(power) < r_ptr->level)
			{
				msg_format("%sに断られた。", m_name);
			}
			else
			{
				msg_format("%sはあなたに従った。", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;

	//ライトニングネイ
	case 2:
	{
		if (only_info) return format("");

		if (!rapid_fire(99,1))
		{
			return NULL;
		}

	}
	break;

	case 3: //デンスクラウド
	{
		int base = 15;
		int v;
		if (only_info) return format("期間:%d+1d%dターン", base, base);

		msg_format("濃密な雲が現れ、あなたを包み込んだ...");
		v = base + randint1(base);
		set_oppose_acid(v, FALSE);
		set_oppose_elec(v, FALSE);
		set_oppose_fire(v, FALSE);
		set_oppose_cold(v, FALSE);
		set_oppose_pois(v, FALSE);
		if (plev > 39) set_shield(v, FALSE);

		break;
	}

	case 4:
	{
		int i;
		int summon_num = plev / 6;
		if (only_info) return format("召喚数:%d", summon_num);
		int mode = PM_FORCE_PET;

		if (plev > 39) mode |= PM_HASTE;

		msg_print("あなたは配下の動物霊を招集した！");

		for (i = 0; i < summon_num; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx = MON_ANIMAL_G_KEIGA;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 3, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (mode));
		}

	}
	break;


	case 5: //町移動
	{
		if (only_info) return format("");




		if (!dun_level)
		{
			msg_print("あなたは天高く飛び上がった！");

			if (!tele_town(TRUE)) return NULL;

		}
		else
		{
			msg_print("その技は地上でしか使えない。");
			return NULL;

		}

	}
	break;

	//スリリングショット　内部的にはラピッドファイアと同じにする
	case 6:
	{
		int power = plev * 3 + chr_adj * 5;
		if (only_info) return format("効力:%d",power);

		msg_print("あなたは威圧的に銃を構えた...");

		if (!rapid_fire(99, 0))
		{
			return NULL;
		}
		turn_monsters(power);

	}
	break;
	//トライアングルチェイス
	case 7:
	{
		int len = plev / 6;
		if (only_info) return format("射程:%d", len);
		if (!rush_attack(NULL, len, HISSATSU_TRIANGLE)) return NULL;
		break;
	}
	break;

	//マッスルエクスプロージョン
	case 8:
	{
		int dam = p_ptr->chp / 2;
		if (only_info) return format("損傷:%d",dam);

		msg_print("必殺の気合いが大地を砕いた！");
		project_hack2(GF_FORCE, 0, 0, dam);
		set_tim_addstat(A_STR, 120, 10, FALSE);

		break;
	}


	case 9:
	{
		if (only_info) return "倍率:2.0";

		special_shooting_mode = SSM_BLACK_PEGASUS;

		if (!do_cmd_fire())
		{
			special_shooting_mode = 0L;
			return NULL;
		}
	}
	break;



	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::v1.1.69 潤美特技*/
class_power_type class_power_urumi[] =
{

	{ 7,7,30,FALSE,TRUE,A_DEX,0,0,"重量操作Ⅰ",
	"強力な投擲を行う。" },

	{ 13,20,40,FALSE,TRUE,A_CHR,0,0,"ストーンベイビー",
	"隣接したモンスター一体を水没させようと試みる。「石の赤子」を使用する。周囲に水辺がないと使えず、ユニークモンスターと水耐性をもつモンスターには無効。" },

	{ 20,50,60,FALSE,FALSE,A_DEX,0,0,"石の赤子生成",
	"アイテム「石の赤子」を作る。石塊をひとつ消費する。" },

	{ 25,30,60,FALSE,TRUE,A_STR,0,0,"重量操作Ⅱ",
	"一定時間、武器攻撃が命中したときの重量ボーナスが増加し会心の一撃が出やすくなる。" },

	{ 32,40,60,FALSE,TRUE,A_CHR,0,0,"デーモンシージ",
	"指定したターゲット周辺に水棲系の配下モンスターを複数召喚する。水のないところには召喚できない。" },

	{ 40,45,60,FALSE,TRUE,A_STR,0,0,"重量操作Ⅲ",
	"隣接したモンスター全てに強力な遅鈍属性攻撃を行う。" },

	{ 48,120,90,FALSE,TRUE,A_CHR,0,0,"ヘビーストーンベイビー",
	"隣接したモンスター一体を、耐性を無視して大幅に減速させる。「石の赤子」を使用する。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

//石の赤子作成用石塊指定ルーチン
static bool item_tester_hook_stone(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_STONE) return TRUE;
	else return FALSE;
}

cptr do_cmd_class_power_aux_urumi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
		{
			int mult = 1 + (plev + 10) / 20;
			if (only_info) return format("倍率:%d", mult);
			if (!do_cmd_throw_aux(mult, FALSE, -2)) return NULL;
		}
		break;
	case 1:
		{
			int i;
			int inven_idx;
			bool flag_ok = FALSE;
			monster_type *m_ptr;
			int y, x;
			int power = plev * 3 + chr_adj * 3 + 10;

			if (use_itemcard && power < 150) power = 150;

			if (only_info) return  format("効力:%d", power);

			//周囲に水があるかどうか確認
			for (i = 1; i <= 9; i++)
			{
				y = py + ddy[i];
				x = px + ddx[i];

				if (!in_bounds(y, x)) continue;
				if (cave_have_flag_bold(y, x, FF_WATER)) flag_ok = TRUE;
			}

			if (!flag_ok)
			{
				msg_print("この技は水辺でないと使えない。");
				return NULL;
			}

			//石の赤子を持っているか確認
			inven_idx = check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY);
			if (!use_itemcard && inven_idx < 0)
			{
				msg_print("石の赤子が必要だ。");
				return NULL;
			}

			//隣接モンスター指定
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			//モンスターを沈める
			if (cave[y][x].m_idx && (m_ptr->ml))
			{
				int damage;
				int o_idx;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);

				if (use_itemcard || !one_in_(4))
					msg_format("%sに石の赤子を押し付けた。",m_name);
				else
					msg_format("「ふっふっふ、私の代わりにこの子を見といてくれないか？」");

				//モンスターが石の赤子を持っている扱いにする(アイテムカード使用時も同じ)
				o_idx = o_pop();
				if (o_idx)
				{
					object_type *o_ptr = &o_list[o_idx];

					object_prep(o_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));

					/* Memorize monster */
					o_ptr->held_m_idx = cave[y][x].m_idx;
					/* Build a stack */
					o_ptr->next_o_idx = m_ptr->hold_o_idx;
					/* Carry object */
					m_ptr->hold_o_idx = o_idx;
				}

				//水没判定
				if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE) ||
					(r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					power = -1;
				}
				else
				{
					if (r_ptr->flags7 & RF7_CAN_SWIM) power /= 2;
					if (r_ptr->flags2 & RF2_POWERFUL) power /= 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) power /= 2;
				}
				if (randint1(power) > r_ptr->level)
				{
					msg_format("%sを水底へ沈めた。", m_name);
					damage = m_ptr->hp + 1;
					project(0, 0, m_ptr->fy, m_ptr->fx, damage, GF_WATER, PROJECT_KILL, -1);
				}
				else
				{
					msg_format("%sは水に沈まなかった！", m_name);
				}
				anger_monster(m_ptr);
				set_monster_csleep(cave[y][x].m_idx, 0);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			//アイテムカード使用時でないとき、石の赤子消費
			if (!use_itemcard)
			{
				inven_item_increase(inven_idx, -1);
				inven_item_describe(inven_idx);
				inven_item_optimize(inven_idx);
			}

		}
		break;

		//石の赤子生成
		case 2:
		{
			cptr q, s;
			int item;
			object_type forge;
			object_type *q_ptr = &forge;

			if (only_info) return format("");
			item_tester_hook = item_tester_hook_stone;

			q = "どれを加工しますか? ";
			s = "石塊が必要だ。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			msg_print("あなたは石を削り始めた..");
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));
			drop_near(q_ptr, -1, py, px);

			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
		}
		break;

		case 3://武器重量操作 tim_general[0]を使う
		{
			int mult = plev * 8 / 5;
			int time = 10 + plev / 5;
			if (only_info) return format("期間:%d 倍率:x%d.%d", time,mult/10,mult%10);

			set_tim_general(time, FALSE, 0, 0);

		}
		break;
		case 4: //デーモンシージ(水棲モンスター召喚)
		{
			int i;
			int y, x;
			int summon_num = 2 + plev / 10;
			int type = SUMMON_AQUATIC;
			bool flag_ok = FALSE;
			u32b mode = PM_FORCE_PET;

			if (only_info) return format("召喚回数:%d", summon_num);

			if (!get_aim_dir(&dir)) return NULL;
			if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			for (i = 0; i < summon_num; i++)
			{
				if (summon_specific(-1, y, x, plev+20, type, mode)) flag_ok = TRUE;
			}

			if (flag_ok)
				msg_print("飼い馴らした魚たちを嗾けた！");
			else
				msg_print("何も現れなかった...");

		}
		break;

		case 5:
		{
			int rad = 1;
			int base = p_ptr->lev * 10 + chr_adj * 10;
			if (only_info) return format("損傷:%d", base / 2);
			msg_format("近くの全てのものを超重量にした！");
			project(0, rad, py, px, base, GF_INACT, PROJECT_KILL, -1);
		}
		break;

		//ヘビーストーンベイビー
		case 6:
		{
			int i;
			int inven_idx;
			int dec_speed = 15;
			monster_type *m_ptr;
			int y, x;

			if (only_info) return  format("減速:%d",dec_speed);

			//石の赤子を持っているか確認
			inven_idx = check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY);
			if (!use_itemcard && inven_idx < 0)
			{
				msg_print("石の赤子が必要だ。");
				return NULL;
			}

			//隣接モンスター指定
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			//モンスターを減速させる
			if (cave[y][x].m_idx && (m_ptr->ml))
			{
				int damage;
				int o_idx;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);

				if (m_ptr->mflag & MFLAG_SPECIAL)
				{
					msg_format("すでに効いている。");
					return NULL;
				}
				else
				{
					msg_format("%sに石の赤子が貼り付き、どんどん重くなっていく！", m_name);
				}

				//モンスターが石の赤子を持っている扱いにする
				o_idx = o_pop();
				if (o_idx)
				{
					object_type *o_ptr = &o_list[o_idx];

					object_prep(o_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));

					/* Memorize monster */
					o_ptr->held_m_idx = cave[y][x].m_idx;
					/* Build a stack */
					o_ptr->next_o_idx = m_ptr->hold_o_idx;
					/* Carry object */
					m_ptr->hold_o_idx = o_idx;
				}

				m_ptr->mflag |= MFLAG_SPECIAL; //特殊フラグを立てて二度目以降を無効に
				if(m_ptr->mspeed > 20) m_ptr->mspeed -= dec_speed;

				anger_monster(m_ptr);
				set_monster_csleep(cave[y][x].m_idx, 0);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			//アイテムカード使用時でないとき、石の赤子消費
			if (!use_itemcard)
			{
				inven_item_increase(inven_idx, -1);
				inven_item_describe(inven_idx);
				inven_item_optimize(inven_idx);
			}

		}
		break;


	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}


/*:::三月精専用性格*/
//静寂1にtim_general[0]、静寂2にtim_general[1]を使う
//サニーの日光ポイントにmagic_num1[0]を使う
class_power_type class_power_sangetsu_2[] =
{
	{ 1,2,3,FALSE,TRUE,A_DEX,0,5,"アイシクルシュート",
	"冷気属性のボルトを放つ。レベルが上がると威力と本数が上がるが消費MPも上がる。" },

	{ 5,5,20,FALSE,FALSE,A_CHR,0,0,"狂気の松明",
	"隣接したモンスター一体を狂気に冒そうと試みる。狂気に冒されたモンスターは敵味方の区別なく周囲のモンスターと戦い始める。ユニークモンスターには効きづらく、通常の精神を持たないモンスターには効かない。「★クラウンピースの松明」を所持していないと使えない。" },

	{ 8,6,20,FALSE,TRUE,A_STR,0,3,"アイスディゾルバー",
	"火炎属性のビームを放つ。" },
	{ 10,10,20,FALSE,FALSE,A_DEX,0,2,"纏わりつく鱗粉",
	"自分を中心に遅鈍属性のボールを発生させる。" },
	{ 15,5,25,FALSE,TRUE,A_INT,0,0,"幻惑Ⅰ",
	"敵一体を混乱させる。賢い敵には効かない。夜間は効果が落ちる。" },
	{ 18,16,40,FALSE,TRUE,A_INT,0,0,"静寂",
	"一定時間、自分たちと近くのモンスターが魔法と一部の行動を使えなくなる。巻物を読むと効果が解除され少し消費行動時間が増える。" },
	{ 18,0,0,FALSE,TRUE,A_INT,0,0,"静寂解除",
	"特技「静寂」の効果を解除する。この行動によるターン消費は通常の半分。" },
	{ 20,10,40,FALSE,TRUE,A_INT,0,0,"ルチルフレクション",
	"光を屈折させて離れた場所を視認する。ただし閉じたドアやカーテンで遮られた所を見ることはできない。" },
	{ 24,25,40,FALSE,TRUE,A_STR,0,0,"氷塊生成",
	"隣接した床一か所の地形を「氷塊」にする。氷塊は通り抜けられないが叩き壊したり火炎で溶かしたりできる。また閃光属性の攻撃が当たった場合大爆発することがある。この爆発はプレイヤーもダメージを受ける。" },

	{ 25,0,0,FALSE,FALSE,A_INT,0,0,"作戦会議",
	"MPをわずかに回復し士気高揚状態になる。通常時の2倍の行動力を消費する。" },

	{ 27,20,45,FALSE,TRUE,A_DEX,0,0,"エルフィンキャノピー",
	"一定時間反射能力を得る。" },
	{ 30,20,0,FALSE,FALSE,A_DEX,0,0,"爆弾設置",
	"足元に爆弾を設置する。爆弾は「通常は」10ターンで爆発し半径3の破片属性の強力なボールを発生させる。この爆発ではプレイヤーもダメージを受ける。爆弾は一部の属性攻撃に晒されると誘爆したり爆発タイミングが変わることがある。" },
	{ 32,30,50,FALSE,TRUE,A_INT,0,0,"幻惑Ⅱ",
	"一定時間、周囲の敵を惑わせる。視界に頼らない敵に対しては効果が薄い。夜間は効果時間が短くなる。" },
	{ 33,30,60,FALSE,TRUE,A_DEX,0,0,"アゲハの鱗粉",
	"自分を中心にカオス属性のボールを発生させる。" },
	{ 37,20,60,FALSE,TRUE,A_CON,0,0,"オリオンベルト",
	"一定時間士気高揚と身体能力上昇を得る。" },
	{ 40,33,75,FALSE,TRUE,A_INT,0,0,"ブレイクキャノピー",
	"ターゲットモンスターにかかっている魔法効果を解除する。" },

	{ 40,120,45,TRUE,TRUE,A_CON,0,30,"グレイズインフェルノ",
	"視界内の全てに対し火炎属性攻撃を行う。威力はHPの1/3になる。" },

	{ 45,80,80,FALSE,TRUE,A_WIS,0,0,"静寂Ⅱ",
	"一定時間、フロア全域のモンスターが魔法と一部の行動を使えなくなる。自分たちは制限を受けない。" },
	{ 45,50,80,FALSE,TRUE,A_CHR,0,0,"パーフェクトフリーズ",
	"視界内の全てに対し強力な極寒属性攻撃を放つ。" },
	{ 48,72,80,FALSE,TRUE,A_CHR,0,0,"究極の必殺技",
	"閃光属性の強力なレーザーを放つ。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};

cptr do_cmd_class_power_aux_sangetsu_2(int num, bool only_info)
{
	int dir, dice, sides, base, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

		//アイシクルシュート
	case 0:
	{
		int dice = 2 + plev / 7;
		int dir;
		int sides = 3 + chr_adj / 15;
		int num = MIN(10, (1 + plev / 4));
		if (only_info) return format("損傷:(%dd%d) * %d", dice, sides, num);
		if (!get_aim_dir(&dir)) return NULL;
		msg_print("チルノが氷の弾を放った！");
		for (i = 0; i < num; i++) fire_bolt(GF_COLD, dir, damroll(dice, sides));

	}
	break;
	//狂気の松明
	case 1:
	{
		monster_type *m_ptr;
		int y, x;
		int power = plev * 2 + chr_adj * 2 + 20;
		bool torch = FALSE;

		if (only_info) return  format("効力:%d", power);

		for (i = 0; i<INVEN_TOTAL; i++) if (inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
		if (!torch)
		{
			msg_print("その技は地獄の松明を所持していないと使えない。");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			if (use_itemcard)
				msg_format("篝火が狂気の光を放った！");
			else
				msg_format("ピースが%sの前に松明を近づけた……", m_name);

			lunatic_torch(cave[y][x].m_idx, power);
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;
	//アイスディソルバー
	case 2:
	{
		int dice = 1 + plev / 4;
		int sides = 7;
		int base = plev / 2 + chr_adj / 2;

		if (only_info) return format("損傷:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("熱線を放った。");
		fire_beam(GF_FIRE, dir, base + damroll(dice, sides));
		break;
	}
	//纏わりつく鱗粉
	case 3:
	{
		int dam = 10 + plev + chr_adj;
		int rad = 1 + plev / 30;
		if (only_info) return format("損傷:～%d", dam / 2);

		msg_print("ラルバが鱗粉をまき散らした。");
		project(0, rad, py, px, dam, GF_INACT, (PROJECT_KILL | PROJECT_JUMP), -1);
	}
	break;
	//幻覚1
	case 4:
	{
		int x, y;
		int dist = 2 + plev / 5;
		monster_type *m_ptr;
		int power = 20 + plev;
		if (is_daytime()) power += 30;

		if (only_info) return format("射程:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("視界内のターゲットを明示的に指定しないといけない。");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("そこには何もいない。");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("サニーは%sの周囲の景色を歪めた・・", m_name);

			if (r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF ||
				r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
			{
				msg_format("しかし効果がなかった。");
			}
			else
			{
				msg_format("%sは混乱した！", m_name);
				(void)set_monster_confused(cave[y][x].m_idx, MON_CONFUSED(m_ptr) + 10 + randint1(plev / 3));
			}
			set_monster_csleep(cave[y][x].m_idx, 0);
			anger_monster(m_ptr);
		}

		break;
	}

	case 5://静寂1 tim_general[0]を使う
	{
		int time = 10 + plev / 5;
		int rad = plev / 5;
		if (only_info) return format("期間:%d 範囲:%d", time, rad);
		if (p_ptr->tim_general[0] || p_ptr->tim_general[1])
		{
			msg_print("すでに能力を使っている。");
			return NULL;
		}

		msg_print("ルナは指を鳴らした。周囲の音が消えた。");
		set_tim_general(time, FALSE, 0, 0);

	}
	break;
	case 6://静寂解除
	{
		if (only_info) return format("");
		if (!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
		{
			msg_print("能力を使っていない。");
			return NULL;
		}

		msg_print("ルナは能力を解除した。");
		set_tim_general(0, TRUE, 0, 0);
		set_tim_general(0, TRUE, 1, 0);

	}
	break;
	case 7:
	{
		//v1.1.34 サニーの「光を屈折させて離れた場所の光景を見る」能力を追加。
		//まず特殊フラグを立ててからflying_dist値を閉じたドアやカーテンに遮られるようにして再計算する。
		//次にこのflying_dist値が設定されたグリッドのみを啓蒙、モンスター感知する。
		//最後にフラグを戻してflying_dist値を再計算する。
		if (only_info) return format("範囲:%d移動グリッド", MONSTER_FLOW_DEPTH);

		if (p_ptr->blind)
		{
			msg_print("サニーは目が見えない。");
			return NULL;
		}

		msg_print("離れた場所の映像がサニーの目の前に広がった……");
		flag_sunny_refraction = TRUE;
		update_flying_dist();
		wiz_lite(FALSE);
		detect_monsters_normal(255);
		flag_sunny_refraction = FALSE;
		update_flying_dist();

	}
	break;

	//v1.1.68 氷塊生成
	case 8:
	{
		int y, x, dir;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("この場所には特技を使えない。");
			return NULL;
		}
		msg_print("チルノが強固な氷を作り出した。");
		cave_set_feat(y, x, f_tag_to_index_in_init("ICE_WALL"));

		break;
	}
	//作戦会議
	case 9:
	{
		int mana = plev / 5;
		if (only_info) return format("回復:%d", mana);

		if (!one_in_(5))
			msg_print("みんなで作戦会議を始めた..");
		else if (p_ptr->prace == RACE_NINJA)
		{
			if(one_in_(3))	msg_print("「囲んで棒で叩く！」");
			else if (one_in_(2))	msg_print("「何事も暴力で解決するのが一番だ」");
			else msg_print("「ガンバルゾー！」");

		}
		else
		{
			if (one_in_(3))	msg_print("チルノとピースが喧嘩を始めた。");
			else if (one_in_(2))	msg_print("スターはティータイムに入った。");
			else msg_print("「みんな一緒に思いっきりかかれば負ける筈がない！」");


		}

		player_gain_mana(mana);
		set_hero(10, FALSE);
		//消費行動力2倍
		new_class_power_change_energy_need = 200;
		break;
	}



	case 10:
	{
		int time;
		int base = 15 + p_ptr->lev / 5;

		if (only_info) return format("期間:%d+1d%d", base, base);
		time = base + randint1(base);
		msg_format("サニーは身構えた。");
		set_tim_reflect(time, FALSE);

		break;
	}

	case 11:
	{
		int dam = 100 + plev * 4;
		if (only_info) return format("損傷:%d", dam);
		if (!futo_can_place_plate(py, px) || cave[py][px].special)
		{
			msg_print("ここには爆弾を置けない。");
			return NULL;
		}
		msg_print("スターは爆弾を仕掛けた。");
		/*:::Mega Hack - cave_type.specialを爆弾のカウントに使用する。*/
		cave[py][px].info |= CAVE_OBJECT;
		cave[py][px].mimic = feat_bomb;
		cave[py][px].special = 10;
		while (one_in_(5)) cave[py][px].special += randint1(20);
		note_spot(py, px);
		lite_spot(py, px);
	}
	break;
	case 12:
	{
		int base = 5 + plev / 5;
		if (is_daytime()) base *= 2;
		if (only_info) return format("期間:%d+1d%d", base, base);
		msg_format("サニーは光を操って皆の姿を消した。");
		set_tim_superstealth(base + randint1(base), FALSE, SUPERSTEALTH_TYPE_OPTICAL);
		break;
	}

	case 13://アゲハの鱗粉
	{
		int dam = plev * 4 + chr_adj * 4;
		int rad = 1 + plev / 15;
		if (only_info) return format("損傷:～%d", dam / 2);

		msg_print("ラルバが妖しく輝く鱗粉をまき散らした...");
		project(0, rad, py, px, dam, GF_CHAOS, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;

	//オリオンベルト
	case 14:
	{
		int base = 10 + plev / 5;
		int time;
		if (only_info) return format("期間:%d+1d%d", base, base);
		time = base + randint1(base);
		msg_format("スターが皆を激励した！");
		set_hero(time, FALSE);
		set_tim_addstat(A_STR, 4, time, FALSE);
		set_tim_addstat(A_CON, 4, time, FALSE);
		set_tim_addstat(A_DEX, 4, time, FALSE);

		break;
	}
	//ブレイクキャノピー
	case 15:
	{

		int m_idx;

		if (only_info) return format("");
		if (!target_set(TARGET_KILL)) return NULL;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) return NULL;
		if (!player_has_los_bold(target_row, target_col)) return NULL;
		if (!projectable(py, px, target_row, target_col)) return NULL;
		msg_print("ルナは指を鳴らした。");
		dispel_monster_status(m_idx);

		break;
	}

	case 16:
	{
		int base = p_ptr->chp / 3;

		if (base < 1) base = 1;

		if (only_info) return format("損傷:～%d", base);

		msg_print("ピースが放った熱波が周囲を焼き払った！");
		project_hack2(GF_FIRE, 0, 0, base);

	}
	break;



	case 17://静寂2 tim_general[1]を使う
	{
		int time = 30;
		if (only_info) return format("期間:%d", time);
		if (p_ptr->tim_general[1])
		{
			msg_print("すでに能力を使っている。");
			return NULL;
		}

		msg_print("ルナはフロア全体の音に干渉した。");
		set_tim_general(time, FALSE, 1, 0);
		p_ptr->tim_general[0] = 0;
	}
	break;

	//パーフェクトフリーズ
	case 18:
	{
		int dam = plev * 4 + chr_adj * 10;
		if (only_info) return format("損傷:%d", dam);
		msg_print("チルノが全力を解放して辺りを凍らせた！");
		project_hack2(GF_ICE, 0, 0, dam);
	}
	break;
	//協力技
	case 19:
	{
		int dice = 6 + chr_adj / 5;
		int sides = plev;
		int base = plev * 2 + chr_adj * 5;

		if (only_info) return format("損傷:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("皆がレーザーを放ち、サニーがそれらを収束させた！");
		fire_beam(GF_LITE, dir, base + damroll(dice, sides));
		break;
	}



	default:
		if (only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}






/*:::v1.1.67 久侘歌特技*/
class_power_type class_power_kutaka[] =
{
	{ 5,3,20,FALSE,FALSE,A_INT,0,0,"周辺調査",
	"周囲のモンスターを感知する。さらにレベル15でトラップ、レベル25でアイテム、レベル35で地形を感知する。" },

	{ 10,0,20,FALSE,FALSE,A_INT,0,0,"鳥頭発動Ⅰ",
	"恐怖状態を治癒し、知能や賢さが低下している場合回復する。レベル30以降ではMPをわずかに回復する。ただし所持品や周囲の地形に関する記憶を失うことがある。" },

	{ 15,24,40,FALSE,TRUE,A_INT,0,0,"水配りの試練",
	"地面を水地形に変化させる射程の短いビームを放つ。" },

	{ 20,20,40,FALSE,TRUE,A_INT,0,0,"見渡しの試練",
	"部屋を明るくし、さらに視界内のモンスターを混乱させようと試みる。" },

	{ 25,36,50,FALSE,TRUE,A_INT,0,0,"鬼渡の試練",
	"視界内のモンスターをテレポートさせようと試みる。" },

	{ 30,50,75,FALSE,FALSE,A_INT,0,0,"鳥頭発動Ⅱ",
	"選択した習得済みの魔法を一つ忘却し、呪文学習可能数を一つ増やす。忘却した魔法の熟練度が高いと二つ増やす。" },

	{ 35,40,60,FALSE,TRUE,A_WIS,0,0,"血の分水嶺",
	"一時的に士気高揚・肉体強化を得る。また自分と近くのモンスターのテレポートを阻害する。" },

	{ 40,55,70,FALSE,TRUE,A_CHR,0,0,"獄界視線",
	"視界内のモンスター全てに強力な精神攻撃を行い、さらに金縛りにしようと試みる。" },

	{ 45,96,80,FALSE,TRUE,A_CHR,0,0,"全霊鬼渡り",
	"自分の周囲にいる幻想郷の存在ではないモンスターをフロアから追放しようと試みる。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};


cptr do_cmd_class_power_aux_kutaka(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0://周辺調査
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("範囲:%d", rad);

		msg_print("あなたは敵の気配を探った...");
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);
		if (plev > 14)
		{
			detect_traps(rad, TRUE);
		}
		if (plev > 24)
		{
			detect_objects_gold(rad);
			detect_objects_normal(rad);
		}
		if (plev > 34)
		{
			map_area(rad);
		}


		break;
	}
	case 1://鳥頭1
	{
		if (only_info) return "";


		if (p_ptr->afraid || p_ptr->stat_cur[A_INT] != p_ptr->stat_max[A_INT] || p_ptr->stat_cur[A_WIS] != p_ptr->stat_max[A_WIS])
		{
			msg_format("あなたは嫌なことを忘れてしまうことにした。");
			set_afraid(0);
			do_res_stat(A_INT);
			do_res_stat(A_WIS);

		}
		else if(plev > 29)
		{
			msg_format("あなたは頭を空っぽにした...");
			if (player_gain_mana(randint1(3 + plev / 7))) msg_print("頭がスッキリした。");
		}

		if (randint1(plev+5) > plev)
		{
			if (lose_all_info()) msg_print("...？　うっかり色々忘れすぎたようだ。");
		}

		break;
	}


	case 2://水配り
	{
		int dist = 2 + plev / 5;
		if (only_info) return format(" 射程:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		msg_format("あなたはこの地に水路を引いた。");
		fire_beam(GF_WATER_FLOW, dir, 10);
		break;
	}
	case 3://見渡し
	{
		int power = 10 + p_ptr->lev * 3;
		if (only_info) return format("効力:%d", power);

		msg_format("あなたは幻惑的な光を放った。");
		lite_area(damroll(2,15), 3);
		confuse_monsters(power);
		break;
	}

	case 4://鬼渡し
	{
		int power = p_ptr->lev * 4;
		if (only_info) return format("効力:%d", power);

		msg_format("あなたは無法者たちを追い払った。");
		banish_monsters(power);

	}
	break;

	case 5://選択的記憶除去
	{
		if (only_info) return "";

		msg_print("あなたは複雑な忘却のために集中を始めた...");
		flag_spell_forget = TRUE;

		do_cmd_cast();

		flag_spell_forget = FALSE;

	}
	break;


	case 6: //血の分水嶺
	{
		int v;
		bool base = 10;
		int percentage = p_ptr->chp * 100 / p_ptr->mhp;
		if (only_info) return format("期間:%d+1d%d", base, base);

		v = base + randint1(base);
		if(one_in_(3))
			msg_print("あなたは不退転の覚悟を決めた！");
		else if (one_in_(2))
			msg_print("あなたはここを死守する決意を固めた！");
		else
			msg_print("「コケー！」「ピヨ！」");

		set_hero(v, FALSE);
		set_tim_addstat(A_STR, 104, v, FALSE);
		set_tim_addstat(A_CON, 104, v, FALSE);
		set_tim_addstat(A_DEX, 104, v, FALSE);
		set_nennbaku(v, FALSE);
		p_ptr->chp = p_ptr->mhp * percentage / 100;
		p_ptr->redraw |= (PR_HP);
		redraw_stuff();

		break;
	}

	case 7: //獄界視線
	{

		int power = p_ptr->lev * 2 + chr_adj * 5;
		if (only_info) return format("効力:%d", power);

		msg_format("あなたの鋭い眼光が周囲を射竦めた！");
		project_hack(GF_REDEYE, power);
		stasis_monsters(power);

		break;
	}
	case 8://全霊鬼渡り
	{

		int rad = plev;
		int power = p_ptr->lev * 2 + chr_adj * 5;
		if (only_info) return format("範囲:%d 効力:%d",rad, power);

		msg_format("あなたは招かれざる者たちをこの地から放逐した！");
		mass_genocide_2(power, rad, 1);

		break;
	}

	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}



/*:::v1.1.66 磨弓用特技*/
class_power_type class_power_mayumi[] =
{
	{ 8,5,20,FALSE,FALSE,A_INT,0,0,"索敵",
	"周囲のモンスターを感知する。" },

	{ 15,20,40,FALSE,FALSE,A_INT,0,0,"モンスター調査",
	"視界内のモンスターの情報を得る。" },

	{ 20,30,50,FALSE,TRUE,A_CHR,0,0,"剣士埴輪",
	"モンスター「剣士埴輪」を召喚する。レベル40以降は「熟練剣士埴輪」を召喚する。" },
	{ 25,30,50,FALSE,TRUE,A_CHR,0,0,"弓兵埴輪",
	"モンスター「弓兵埴輪」を召喚する。レベル40以降は「熟練弓兵埴輪」を召喚する。" },
	{ 30,30,50,FALSE,TRUE,A_CHR,0,0,"騎馬兵埴輪",
	"モンスター「騎馬兵埴輪」を召喚する。レベル40以降は「熟練騎馬兵埴輪」を召喚する。" },

	{ 45,80,70,FALSE,TRUE,A_CHR,0,0,"不敗の無尽兵団",
	"自分の体力と損傷を治癒し、視界内の敵対モンスターを恐怖させ、さらに多数の加速された埴輪を召喚し、ヒーロー、祝福、加速、対邪悪結界を得る。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};


cptr do_cmd_class_power_aux_mayumi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("範囲:%d", rad);

		msg_print("あなたは敵の気配を探った...");
		detect_monsters_normal(rad);
		detect_monsters_invis(rad);

		break;
	}
	case 1:
	{
		if (only_info) return "";

		msg_format("あなたは眼前の敵を注意深く見定めた...");
		probing();
		break;
	}
	case 2:
	case 3:
	case 4:
	{
		bool flag = FALSE;
		int i;
		int count = 2 + plev / 8;
		int mode;


		if (only_info) return "";
		for (i = 0; i<count; i++)
		{
			int r_idx;

			if (num == 2) r_idx = plev > 39 ? MON_HANIWA_F2 : MON_HANIWA_F1;
			else if (num == 3) r_idx = plev > 39 ? MON_HANIWA_A2 : MON_HANIWA_A1;
			else r_idx = plev > 39 ? MON_HANIWA_C2 : MON_HANIWA_C1;

			if (summon_named_creature(0, py, px, r_idx, PM_FORCE_PET)) flag = TRUE;

		}
		if (flag)
			msg_print("部下達を召集した。");
		else
			msg_print("誰も現れなかった...");



		break;
	}

	case 5:
	{
		int i,v;
		if (only_info) return "";

		v = 100 + randint1(100);

		msg_print("あなたは兵団に突撃命令を下した！");
		//周囲の配下の埴輪を加速、それ以外を恐怖にする
		crusade();
		//周囲に埴輪軍団を召喚する
		for (i = 0; i < 30; i++)
		{
			int attempt = 10;
			int my, mx;
			int r_idx;

			if ((i % 3) == 0) r_idx = MON_HANIWA_F2;
			else if ((i % 3) == 1) r_idx = MON_HANIWA_A2;
			else r_idx = MON_HANIWA_C2;

			while (attempt--)
			{
				scatter(&my, &mx, py, px, 5, 0);

				/* Require empty grids */
				if (cave_empty_bold2(my, mx)) break;
			}
			if (attempt < 0) continue;
			summon_named_creature(0, my, mx, r_idx, (PM_FORCE_PET | PM_HASTE));
		}
		set_hero(v, FALSE);
		set_blessed(v, FALSE);
		set_fast(v, FALSE);
		set_protevil(v, FALSE);
		set_afraid(0);
		set_broken(-10000);
		hp_player(5000);

	}
	break;

	default:
	{
		if (only_info) return "未実装";

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}

	}
	return "";
}






/*:::華扇専用性格専用技*/
class_power_type class_power_oni_kasen[] =
{

	{ 5,10,20,TRUE,FALSE,A_STR,0,0,"岩投げ",
	"アイテム「石塊」を投げつける。" },

	{ 10,10,30,TRUE,FALSE,A_DEX,0,0,"強力投擲",
	"アイテム一つを強力に投げつける。" },

	{ 20,20,40,FALSE,TRUE,A_INT,0,0,"スケルトン召喚",
	"スケルトン系の配下を複数体召喚する。" },

	{ 25,50,50,TRUE,FALSE,A_STR,0,20,"衝撃波",
	"自分の周囲に対し轟音属性攻撃を行う。威力は隣接攻撃のターンダメージと同じ。レベル45を超えると視界内全てが対象になる。周囲のモンスターが起きる。" },

	{ 30,30,50,FALSE,TRUE,A_INT,0,0,"アンデッド支配",
	"視界内のアンデッド全てを配下にしようと試みる。" },

	{ 35,66,70,FALSE,TRUE,A_INT,0,0,"骸骨の嵐",
	"地獄属性の巨大なボールを放つ。" },

	{ 40,96,80,FALSE,TRUE,A_INT,0,0,"無間地獄の罠",
	"近くのモンスターを全て倒し、その現在HPに等しい量の体力を回復する。抵抗されると無効。生命のないモンスターにも無効。また力強い敵や賢い敵には効きにくい。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};
cptr do_cmd_class_power_aux_oni_kasen(int num, bool only_info)
{
	int dir, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	switch (num)
	{
	case 0:
		{
			object_type forge;
			object_type *tmp_o_ptr = &forge;
			int ty, tx;

			if (only_info) return "";

			if (!get_aim_dir(&dir)) return NULL;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			msg_print("地面から石塊を掘り出して投げつけた！");
			object_prep(tmp_o_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_STONE));
			do_cmd_throw_aux2(py, px, ty, tx, 3, tmp_o_ptr, 0);

		}
		break;
	case 1: //強力投擲
		{
			int mult = 2 + plev / 15;
			if (only_info) return format("倍率:%d", mult);
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
		}
		break;
	case 2: //スケルトン召喚
		{
			int i;
			int summon_num = 2 + plev / 10;
			int type = SUMMON_SKELETON;
			bool flag_ok = FALSE;
			u32b mode = PM_FORCE_PET;
			if (plev > 34) mode |= PM_ALLOW_GROUP;

			if (only_info) return format("召喚回数:%d", summon_num);


			for (i = 0; i < summon_num; i++)
			{
				if (summon_specific(-1, py, px, plev, type, mode)) flag_ok = TRUE;
			}

			if(flag_ok)
				msg_print("地面から骸骨が這い出し、あなたに従った。");
			else
				msg_print("地面が少し動いた気がするが、何も起こらなかった。");

		}
		break;
	case 3:
		{
			int damage;

			int str_adj = adj_general[p_ptr->stat_ind[A_STR]];


			if (p_ptr->do_martialarts)
			{
				damage = (calc_martialarts_dam_x100(0) * p_ptr->num_blow[0] + calc_martialarts_dam_x100(1) * p_ptr->num_blow[1]) / 100;
			}
			else
			{
				damage = (calc_weapon_dam(0) + calc_weapon_dam(1));
			}

			if (p_ptr->lev < 45)
			{
				if (only_info) return format("損傷:およそ～%d", damage);

				msg_format("あなたは地面に%sを打ち付けた！", (p_ptr->do_martialarts ? "拳":"武器"));
				project(0, 3, py, px, damage, GF_SOUND, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

			}
			else
			{
				if (only_info) return format("損傷:およそ%d", damage);
				msg_print("怪腕の一撃が大地を揺るがした！");
				project_hack2(GF_SOUND, 0, 0, damage);
			}
			aggravate_monsters(0,FALSE);
		}
		break;
	case 4:
		{
			int power = plev + chr_adj * 5;

			if (only_info) return format("効力:%d", power);

			msg_print("あなたは冷たい目で周囲を睨みつけた...");
			charm_undead(power);

		}
		break;
	case 5:
	{

		int dir,damage;
		int rad = 6;

		damage = plev * 4 + chr_adj * 10;

		if (only_info) return format("損傷:%d", damage);

		if (!get_aim_dir(&dir)) return NULL;
		if (!fire_ball_jump(GF_NETHER, dir, damage, rad, "山のような骸骨が降り注いだ！")) return NULL;

	}
	break;
	case 6:
	{
		int power = plev * 7 + chr_adj * 5;
		if (power < 300) power = 300;
		if (only_info) return format("効力:%d", power / 2);


		msg_print("無間地獄への門が開いた！");

		project(0, 1, py, px, power, GF_SOULSTEAL, PROJECT_KILL, -1);


	}
	break;


	default:
		if (only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



//瓔花用特技
class_power_type class_power_eika[] =
{
	{ 10,5,25,FALSE,TRUE,A_DEX,0,0,"石積みⅠ",
	"自分のいるグリッドに「守りのルーン」を配置する。" },

	{ 20,20,30,FALSE,TRUE,A_WIS,0,0,"ストーンウッズ",
	"ダンジョン内に地震を起こして岩石を降らせる。" },

	{ 30,30,50,TRUE,FALSE,A_DEX,0,0,"石積みⅡ",
	"指定方向の隣接グリッドを「岩石」地形にする。" },

	{ 40,50,80,FALSE,FALSE,A_WIS,0,0,"チルドレンズリンボ",
	"自分を中心とした広範囲に「守りのルーン」を配置する。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



/*:::瓔花用特技*/
cptr do_cmd_class_power_aux_eika(int num, bool only_info)
{
	int dir, dice, sides, base, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{
	case 0:
	{
		if (only_info) return "";
		msg_print("あなたは心を込めて一つ一つ石を積んだ...");
		warding_glyph();
	}
	break;
	case 1:
	{
		int rad = plev / 5;
		int time;
		if (only_info) return format("半径:%d", rad);
		msg_format("ダンジョンが揺れた！");
		earthquake(py, px, rad);

		break;
	}

	case 2:
	{
		int y, x;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("ここには石を積めない。");
			return NULL;
		}
		msg_print("あなたは瞬く間に石壁を積み上げた！");
		cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

		break;
	}


	case 3:
	{
		int rad = plev / 5 - 6;

		if (only_info) return format("半径:%d", rad);
		msg_format("あなたは子供霊たちと石積みコンテストを始めた...");
		project(0, rad, py, px, rad*2, GF_MAKE_GLYPH, PROJECT_GRID, -1);
		break;
	}
	break;



	default:
		if (only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}




/*:::隠岐奈特技*/
class_power_type class_power_okina[] =
{

	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"後戸の力",
	"後戸を通してモンスター達から奪った特技を使う。レベルアップにより新たな魔法や特技を習得する。" },

	{ 5,5,10,FALSE,FALSE,A_INT,0,0,"後戸を覗く",
	"周囲の扉を感知する。レベルアップに伴い感知できるものの種類と感知範囲が広がる。" },

	{ 10,20,25,FALSE,TRUE,A_DEX,0,0,"後戸の狂言",
	"自分の周囲の地面にドアを生成する。レベル40以降は視界内全ての地面が対象になる。" },

	{ 12,25,30,FALSE,TRUE,A_INT,0,0,"後戸帰還",
	"後戸の国を通って地上へ帰還する。この帰還は「ほぼ」即時に発動する。EXTRAモードでは下の階へレベルテレポートを行う。" },

	{ 15,30,30,FALSE,FALSE,A_CHR,0,0,"秘神の後光",
	"周囲のモンスターを魅了し配下にしようと試みる。距離が近いほど効果が高い。" },

	{ 18,50,50,FALSE,TRUE,A_INT,0,0,"後戸から放逐する",
	"自分の周囲の敵対モンスターをフロアから追放する。クエスト打倒対象モンスターはフロアの別の場所に強制テレポートする。追放したモンスターの数に応じてHPが回復する。クエストダンジョンでは使えない。" },

	{ 20,180,70,FALSE,FALSE,A_DEX,0,0,"秘神の暗躍弾幕",
	"「パターン」に干渉して特定の勢力に対する攻撃を行う。これ以降に新たなフロアに入るとその勢力のモンスターは体力が減少して最初から起きている状態になる。レベルが上昇すると攻撃できる勢力の種類が増える。この特技は「パターン」の中心にいないと使用できない。" },

	{ 23,40,50,FALSE,TRUE,A_WIS,0,0,"神秘の玉繭",
	"一時的に元素攻撃に対する耐性を得る。レベル45以降は魔法の鎧効果も同時に得る。" },

	{ 27,56,50,FALSE,TRUE,A_DEX,0,0,"七星の剣",
	"一時的に「七星剣」を生成して装備する。どちらかの腕が空いているときにしか使えない。この武器を装備から外すことはできない。" },

	{ 30,48,60,FALSE,TRUE,A_WIS,0,0,"マターラスッカ",
	"一時的に知能・賢さ・魅力を強化する。" },

	{ 35,50,80,FALSE,TRUE,A_INT,0,0,"真・後戸移動",
	"後戸の国を通って指定したモンスターの隣にテレポートする。このときフロア全体のモンスターの位置を感知することができる。通常の1.5倍の行動力を消費する。テレポート不可能な地形には出られない。地上で使うと行ったことのある街へ移動できる。" },

	{ 38,65,70,FALSE,TRUE,A_INT,0,0,"リバースインヴォーカー",
	"一定時間、自分の周囲で行われた敵対的な召喚魔法を高確率で阻害する。" },

	{ 40,128,80,FALSE,FALSE,A_DEX,0,0,"後戸への扉を開放する",
	"探索拠点への入り口を作り出す。周囲にやや広いスペースがないと使えない。クエスト実行中のフロアでは使えない。" },

	{ 43,50,80,FALSE,FALSE,A_CHR,0,0,"無縁の芸能者",
	"一時的にあらゆる技能レベルが現在の魅力値と同じとして扱われる。" },

	{ 47,100,80,FALSE,TRUE,A_CHR,0,0,"背面の暗黒猿楽",
	"視界内の全てに対し、極めて強力な暗黒属性攻撃と狂気属性攻撃を行う。暗い所でしか使えない。" },


{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};



cptr do_cmd_class_power_aux_okina(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0: //後戸の力
	{
		if (only_info) return "";

		if (!cast_monspell_new()) return NULL;
		break;
	}

	case 1: //後戸を覗く
	{
		int rad = 10 + plev;
		if (only_info) return format("範囲:%d", rad);

		msg_print("あなたは後戸を覗き込んだ...");
		if (plev < 15)
			detect_doors(rad);
		else
			map_area(rad);

		if (plev > 9)
		{
			if( plev < 20)
				detect_monsters_normal(rad);
			else
				detect_all(rad);
		}

		break;
	}

	case 2://後戸の狂言
	{
		if (plev < 40)
		{
			int rad = plev / 10 + 1;
			if (only_info)				return format("半径:%d", rad);

			project(0, rad, py, px, 100, GF_MAKE_DOOR, (PROJECT_JUMP | PROJECT_GRID), -1);

		}
		else
		{
			if (only_info)
				return format(" ");

			project_hack2(GF_MAKE_DOOR, 0, 0, 100);
		}
	}
	break;

	case 3: //後戸帰還
	{
		if (only_info) return "";
		int m_idx;

		msg_print("後戸の国への扉が開いた。");

		if (EXTRA_MODE)
		{
			teleport_level(0);
		}
		else
		{
			recall_player(1);
		}
	}
	break;

	case 4://秘神の後光
	{
		int power = plev * 4 + chr_adj * 10;
		int rad = plev / 7;
		if (only_info) return format("効力:～%d", power / 2);

		if (one_in_(3))
			msg_print("あなたは尊大な調子で語り始めた...");
		else if (one_in_(2))
			msg_print("あなたは眩い光を放った！");
		else
			msg_print("あなたは威圧的な雰囲気を醸し出した。");

		project(0, rad, py, px, power, GF_CHARM, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);

	}
	break;


	case 5: //後戸から放逐する
	{
		int rad = plev / 2;
		int geno_count;
		if (only_info) return format("範囲:%d", rad);

		geno_count = mass_genocide_3(rad, FALSE, TRUE);

		if (geno_count)
		{
			msg_format("%d体のモンスターを背中の扉から追い払った。", geno_count);
			hp_player(damroll(geno_count, plev));
		}
		break;
	}

	case 6: //秘神の暗躍弾幕
	{
		if (only_info) return "";

		if (!pattern_attack()) return NULL;

		break;
	}


	case 7: //神秘の玉繭
	{
		int time;
		int base = 25;
		if (only_info) return format("期間:%d+1d%d", base, base);

		msg_print("白く輝く糸があなたを包んだ...");
		time = base + randint1(base);
		set_oppose_acid(time, FALSE);
		set_oppose_elec(time, FALSE);
		set_oppose_fire(time, FALSE);
		set_oppose_cold(time, FALSE);
		set_oppose_pois(time, FALSE);

		if (plev > 44)
			set_magicdef(time, FALSE);

		break;
	}

	case 8: //七星の剣
	{
		if (only_info) return format("期間:25");
		if (!generate_seven_star_sword()) return NULL;
		break;
	}

	case 9: //マターラスッカ
	{
		int v;
		bool base = 10;
		if (only_info) return format("期間:%d+1d%d", base, base);

		v = base + randint1(base);
		msg_print("あなたの顔は叡智と慈しみに満ち溢れた...");
		set_tim_addstat(A_INT, 100 + plev / 5, v, FALSE);
		set_tim_addstat(A_WIS, 100 + plev / 5, v, FALSE);
		set_tim_addstat(A_CHR, 100 + plev / 5, v, FALSE);

		break;
	}

	case 10: //後戸移動
	{
		if (only_info) return format("距離:なし");

		msg_print("あなたは後戸の国を覗き込んだ...");

		if (!dun_level)
		{
			if (!tele_town(TRUE)) return NULL;

		}
		else
		{

			detect_monsters_normal(255);

			if (p_ptr->paralyzed || p_ptr->confused || p_ptr->image)
			{
				msg_print("あなたは移動に失敗した！");
			}
			else
			{
				//テレポートに成功したらTRUEが帰って行動力1.5倍
				if (dimension_door(D_DOOR_BACKDOOR))
					new_class_power_change_energy_need = 150;
			}
		}

	}
	break;

	case 11: //リバースインヴォーカー
	{
		int base = 25;
		if (only_info) return format("期間:%d+1d%d", base, base);

		set_deportation(randint1(base) + base, FALSE);
		break;
	}

	case 12: //後戸の国への扉を開く 紫の「至る所に青山あり」からコピー
	{
		int dx, dy, i;
		bool flag_ok = TRUE;
		if (only_info) return format("");

		if (p_ptr->inside_quest
			|| quest[QUEST_OBERON].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_ANGBAND && dun_level == 100
			|| quest[QUEST_SERPENT].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_CHAOS && dun_level == 127)
		{
			msg_print("今は使えない。");
			return NULL;
		}

		for (i = 1; i<10; i++)
		{
			dx = px + ddx[i];
			dy = py + ddy[i];
			if (!in_bounds(dy, dx) || !cave_clean_bold(dy, dx)) flag_ok = FALSE;
		}

		if (!flag_ok)
		{

			msg_print("ここでは使えない。");
			return NULL;
		}

		msg_print("後戸の国への扉が開いた！");

		cave_set_feat(py, px, f_tag_to_index_in_init("HOME"));

	}
	break;


	case 13: //無縁の芸能者
	{
		int base = 20;
		int time;

		if (only_info) return format("期間:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;

	case 14://背面の暗黒猿楽
	{
		int dam = plev * 5 + chr_adj * 10;
		if (only_info) return format("損傷:%d * 2", dam);


		if (p_ptr->cur_lite > 0 || cave[py][px].info & CAVE_GLOW)
		{
			msg_print("この技は明るい場所では使えない。");
			return NULL;
		}
		msg_print("あなたは闇の中で独り踊り始めた...");
		project_hack2(GF_DARK, 0, 0, dam);
		project_hack2(GF_COSMIC_HORROR, 0, 0, dam);

		break;
	}


	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}


//v1.1.52 夢の世界の菫子
class_power_type class_power_sumireko_d[] =
{

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"新たな力の使用",
	"秘神に与えられた新たな力を使用する。技ごとに追加のMPを消費する。" },

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"新たな力について調べる",
	"秘神に与えられた新たな力の詳細を確認する。" },


	{ 6,4,25,FALSE,TRUE,A_WIS,0,2,"パイロキネシス",
	"やや射程が短い火炎属性のビームを放つ。" },
	{ 16,10,40,FALSE,TRUE,A_INT,0,0,"指向性テレポート",
	"指定した方向へ短距離のテレポートを行う。通常の半分の行動力しか消費しない。テレポートできない場所にテレポートしようとした場合、その周辺のランダムな場所へ出るか元の場所に戻る。" },
	{ 25,0,75,FALSE,TRUE,A_WIS,0,0,"精神集中",
	"MPをわずかに回復する。" },


	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_sumireko_d(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!activate_nameless_art(FALSE)) return NULL;
	}
	break;
	case 1:
	{
		if (only_info) return format("");
		if (!activate_nameless_art(TRUE)) return NULL;
	}
	break;

	case 2:
	{
		int dist = 4 + plev / 8;
		int dice = 8 + plev / 2;
		int sides = 5;
		if (only_info) return format("損傷:%dd%d 射程:%d", dice, sides, dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		msg_format("指を鳴らすと空気が燃え上がった。");
		fire_beam(GF_FIRE, dir, damroll(dice, sides));
		break;
	}
	case 3:
	{
		int dist = 5 + plev / 10;
		if (only_info) return format("距離:%d", dist);

		if(!teleport_to_specific_dir(dist, 7, 0)) return NULL;

		new_class_power_change_energy_need = 50;
		break;
	}
	case 4:
	{
		if (only_info) return format("");
		msg_print("あなたはスプーンを取り出し、精神を集中した・・");
		p_ptr->csp += (4 + randint1(plev / 10));
		if (p_ptr->csp >= p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
		p_ptr->redraw |= (PR_MANA);
	}
	break;


	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}





/*:::依神紫苑専用技*/
class_power_type class_power_shion[] =
{
	{ 1,0,0,FALSE,TRUE,A_INT,0,0,"強制完全憑依",
	"モンスターに憑依して操る。ユニークモンスターには成功しにくく、眠っているモンスターには成功しやすい。憑依されたモンスターは打倒済み扱いになる。憑依に時間制限はなく魔力消去でも解除されない。" },

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"物乞い",
	"モンスターから食物や物資を得ようと試みる。成功したらアイテムをくれたモンスターはフロアから去る。クエストダンジョンでは使えない。" },

	{ 6,20,30,FALSE,TRUE,A_DEX,0,0,"帰還",
	"ダンジョンと地上の間を行き来する。発動までにはタイムラグがある。" },

	{ 13,20,40,TRUE,FALSE,A_DEX,0,0,"逃走",
	"75%の確率で1d200の距離のテレポートをし、25%の確率でレベルテレポートする。" },

	{ 19,30,50,FALSE,TRUE,A_INT,0,0,"透明化",
	"一時的に隠密能力を高める。レベル40以降は壁抜け能力も獲得する。" },

	{ 33,0,0,FALSE,FALSE,A_CHR,0,0,"最凶最悪の極貧不幸神",
	"凶悪なスーパー貧乏神に変化する。変身可能なターン数は蓄積した「不運パワー」により増え、4ターン以下では変身できない。あまりターン数が溜まりすぎると勝手に変身することがある。" },

	{ 37,36,55,FALSE,TRUE,A_CON,0,0,"超貧乏玉",
	"蓄積した「不運パワー」の一部を消費し、強力な暗黒属性のボールを放つ。" },

	{ 42,96,60,FALSE,TRUE,A_CHR,0,0,"ミスチャンススキャッター",
	"視界内のモンスター全てに対し、現在HPの40%～60%のダメージを与える。抵抗されると無効。ユニークモンスターには効果がない。スーパー貧乏神の時にしか使えない。" },



	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

// スーパー貧乏神への変身カウンタにtim_general[0]を使用する
// 前回変身したときのターン数をmagic_num1[0]に記録する
// 前回変身してから周囲で倒れたモンスターの経験値をmagic_num1[1]に記録する
// モンスターに憑依した回数をmagic_num1[2]に記録する
// スーパー貧乏神の状態でJを倒したフラグにmagic_num2[0]を使う
cptr do_cmd_class_power_aux_shion(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:

	{
		int x, y;
		int dist = 6 + plev / 8;
	//	int time = 50 + plev;
		monster_type *m_ptr;
		char m_name[80];


		if (only_info) return format("射程:%d", dist);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;

		x = target_col;
		y = target_row;

		if (dir != 5 || !target_okay() || !projectable(y, x, py, px))
		{
			msg_print("視界内のターゲットを明示的に指定しないといけない。");
			return NULL;
		}
		project_length = 0;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("そこには何もいない。");
			return NULL;
		}
		else
		{
			int r_idx = m_ptr->r_idx;
			int chance;
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);


			chance = (plev + chr_adj) * 100 / MAX(r_ptr->level,1);
			if (MON_CSLEEP(m_ptr)) chance = chance * 2;
			if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 2;

			if (p_ptr->wizard) chance = 100;
			if (chance > 100) chance = 100;
			if (r_ptr->flags1 & RF1_QUESTOR) chance = 0;
			if (r_ptr->flagsr & RFR_RES_ALL) chance = 0;

			if (m_ptr->r_idx == MON_TENSHI) chance = 0; //天子は心に隙がないそうなので成功率0にしとく。それなら他の天人月人神様連中はどうなのと思うが気にしないことにする

			if (!get_check_strict(format("取り憑きますか？(成功率:%d%%)", chance), CHECK_DEFAULT_Y)) return NULL;
			//成功するとモンスターを削除しそのモンスターに変身
			if (randint0(100) < chance)
			{
				int mon_old_hp = m_ptr->hp;

				check_quest_completion(m_ptr);
				delete_monster_idx(cave[y][x].m_idx);
				move_player_effect(y, x, MPE_DONT_PICKUP);
				metamorphose_to_monster(r_idx,-1);

				if (p_ptr->chp < mon_old_hp)
				{
					p_ptr->chp = MIN(p_ptr->mhp,mon_old_hp);
					p_ptr->redraw |= PR_HP;
					redraw_stuff();
				}

				//憑依成功したら打倒済みにする
				if (r_ptr->r_akills < MAX_SHORT) r_ptr->r_akills++;
				if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;
				if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 0;


				gain_exp(r_ptr->mexp);
				//運気を吸い取ったものとし、不運パワーを蓄積する
				p_ptr->magic_num1[1] += r_ptr->mexp * 3;
				//憑依回数カウント
				p_ptr->magic_num1[2]++;



			}
			else
			{
				if (chance < 1)
					msg_format("%sの心には全く隙がない！", m_name);
				else
					msg_print("憑依に失敗した！");
				//起こす
				set_monster_csleep(cave[y][x].m_idx, 0);
				if (is_friendly(m_ptr))
				{
					msg_format("%sは怒った!",m_name);
					set_hostile(m_ptr);
				}

			}
		}
	}
	break;

	case 1:
	{
		if (only_info) return "";
		if (!shion_begging()) return NULL;
		break;
	}
	case 2:
	{
		if (only_info) return "";
		if (!word_of_recall()) return NULL;
		break;
	}

	case 3:
	{
		if (only_info) return format("効果:不定");

		if (one_in_(4) && !p_ptr->inside_quest && dun_level)
		{
			teleport_level(0);
		}
		else
		{
			teleport_player(randint1(200), 0L);
		}

		break;
	}

	case 4:
	{
		base = plev / 5;
		int v;
		if (only_info) return format("期間:%d+1d%d", base,base);

		msg_print("あなたの姿がぼやけた...");
		v = randint1(base) + base;
		set_tim_stealth(v, FALSE);
		if (plev > 39) set_kabenuke(v, FALSE);

	}
	break;

	case 5:
	{
		int v = CALC_SUPER_SHION_TURNS;


		if (only_info) return format("期間:%d turn", v);

		if(SUPER_SHION)
		{
			msg_print("すでに変身している。");
			return NULL;
		}

		if (v < 5)
		{
			msg_print("まだ十分な不運パワーが溜まっていない。");
			return NULL;
		}

		set_tim_general(v, FALSE, 0, 0);

		//変身時にmagic_num1[0]を現在ターン数にし[1]をリセット
		p_ptr->magic_num1[0] = turn;
		p_ptr->magic_num1[1] = 0;

		//HP全回復
		hp_player(9999);


		break;
	}
	case 6:
	{
		int pows = p_ptr->magic_num1[1] / SHION_BINBOUDAMA;
		int dam = 250 + plev * 5;


		if (only_info) return format("損傷:%d 使用回数:%d", dam,pows);

		if (!pows)
		{
			msg_print("十分な不幸パワーが溜まっていない。");
			return NULL;
		}

		if (!get_aim_dir(&dir)) return NULL;

		msg_format("皆から集めた不幸パワーを叩き付けた！");
		fire_ball(GF_DARK, dir, dam, 4);

		p_ptr->magic_num1[1] -= SHION_BINBOUDAMA;

	}
	break;

	case 7:
	{
		int power = p_ptr->lev * 10;
		if (power < 300) power = 300;

		if (only_info) return format("効力:%d", power);

		if (use_itemcard) msg_print("カードの中の鉢から破滅のオーラが噴き出した！");
		else msg_print("あなたの持つ鉢から破滅のオーラが噴き出した！");

		project_hack(GF_HAND_DOOM, power);

		break;
	}





	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



/*:::依神女苑専用技*/
class_power_type class_power_jyoon[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"指輪装備",
	"専用スロットに指輪を8つまで装備できる。ただし装備した指輪のパラメータは半減(端数切り捨て)として扱われる。呪われた指輪は装備できない。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"指輪解除",
	"専用スロットに装備している指輪を外す。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"指輪発動",
	"専用スロットに装備している指輪を発動する。" },

	{ 10,5,50,FALSE,FALSE,A_INT,0,0,"目利き",
	"アイテム一つの価値を知ることができる。鑑定や判別はできない。" },
	{ 15,10,45,FALSE,FALSE,A_INT,0,0,"財貨感知",
	"フロアのアイテムと財宝を感知する。レベル30以降はアイテムや財宝を持っているモンスターも感知する。" },
	{ 20,0,0,FALSE,FALSE,A_CHR,0,0,"散財",
	"散財を始める。所持金が$1000以上必要。散財中は格闘攻撃の威力が大幅に上昇するが攻撃のたびに所持金が減少する。もう一度このコマンドを実行すると元に戻る。このコマンドの実行には行動順を消費しない。" },
	{ 25,25,0,TRUE,FALSE,A_DEX,30,10,"突進",
	"数グリッドを一瞬で移動する。途中にモンスターがいたら気属性ダメージを与えて止まる。" },
	{ 32,20,60,FALSE,TRUE,A_STR,0,0,"スレイブロバー",
	"隣接したモンスター一体が加速しているときその効果を奪い取って減速させる。レベル45以降はさらに無敵化の効果を奪い取ってわずかに行動遅延させる。" },
	{ 38,80,70,FALSE,TRUE,A_CHR,0,0,"プラックピジョン",
	"一定時間、周囲のモンスターの魔法成功率を低下させる。アイテムや財宝を多く持つモンスターほど効果が高い。" },
	{ 43,120,80,FALSE,TRUE,A_INT,0,0,"アブソリュートルーザー",
	"モンスター一体に貧乏神の姉を強制的に憑依させる。姉に憑依されたモンスターはそのフロア限定の一時的な配下としてあなたの支配を受ける。クエスト打倒対象のモンスターには効果がない。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_jyoon(int num, bool only_info)
{
	int dir, dice, sides, base, damage, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch (num)
	{

	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;

		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		p_ptr->update |= PU_BONUS;
		update_stuff();
		break;
	}
	case 2:
	{
		if (only_info) return format("");
		if (!activate_inven2()) return NULL;
		break;
	}

	case 3:
	{
		cptr q, s;
		object_type *o_ptr;
		int item;
		int value;
		if (only_info) return "";

		q = "どのアイテムを確認しますか？";
		s = "アイテムがない。";
		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return NULL;
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}
		else
		{
			o_ptr = &o_list[0 - item];
		}
		value = object_value_real(o_ptr);

		if (!value)
		{
			msg_format("このアイテムは無価値だ。");
			if (object_is_cursed(o_ptr))
				msg_format("その上呪われている。");
		}
		else
		{
			msg_format("このアイテムには%s$%dの価値があるようだ。", ((o_ptr->number>1) ? "一つあたり" : ""), value);
			if (object_is_cursed(o_ptr))
				msg_format("しかし呪われている。");

		}

		//if (object_is_cheap_to_jyoon(o_ptr))
		//	msg_format("あなたにとっては身につけるに値しない安物だ。");

		break;
	}


	case 4:
	{
		int range = 25 + plev / 2;
		if (only_info) return format("範囲:%d", range);

		detect_objects_gold(range);
		detect_objects_normal(range);

		if (plev > 29)
			detect_monsters_rich(range);

		break;
	}


	case 5:
	{
		if (only_info) return format("");

		if (p_ptr->special_attack & ATTACK_WASTE_MONEY)
		{
			msg_print("散財を止めた。");
			p_ptr->special_attack &= ~(ATTACK_WASTE_MONEY);
		}
		else if (p_ptr->au < 1000) //v1.1.47 条件追加
		{
			msg_print("先立つものがない。もっと稼がないと。");

		}
		else
		{
			msg_print("あなたは黄金色のオーラをまとった！");
			p_ptr->special_attack |= ATTACK_WASTE_MONEY;
		}
		p_ptr->redraw |= PR_STATUS;

		return NULL; //行動順消費なし
	}
	break;

	case 6:
	{
		int len = p_ptr->lev / 5;
		if (len < 5) len = 5;
		damage = p_ptr->lev * 3;
		if (damage < 50) damage = 50;
		if (only_info) return format("射程:%d 損傷:%d", len, damage);
		if (!rush_attack2(len, GF_FORCE, damage)) return NULL;
		break;
	}


	case 7://スレイブロバー
	{
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format(" ");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			msg_format("あなたは%sへ飛び掛かった！", m_name);

			tim = MON_FAST(m_ptr);

			if (tim)
			{
				msg_print("加速効果を奪い取った！");
				set_monster_fast(cave[y][x].m_idx, 0);
				set_monster_slow(cave[y][x].m_idx, tim);
				set_fast(tim, FALSE);
				flag_success = TRUE;

			}

			tim = MON_INVULNER(m_ptr);
			if (tim && (use_itemcard || plev > 44))
			{
				msg_print("無傷の球を奪い取った！");
				set_monster_invulner(cave[y][x].m_idx, 0,TRUE);
				set_invuln(tim, FALSE);
				flag_success = TRUE;
			}

			if (!flag_success) msg_print("...しかし何も奪えなかった。");

		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;

	case 8: //プラックピジョン
	{
		int base = 10;
		int time;

		if (only_info) return format("期間:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;

	case 9://アブソリュートルーザー
	{
		int y, x;
		int i;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format(" ");

		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (m_ptr->mflag & MFLAG_POSSESSED_BY_SHION)
			{
				msg_print("今姉は他のモンスターに憑りついている。");
				return NULL;
			}
		}
		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			if ((r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flagsr & RFR_RES_ALL))
			{
				msg_format("姉は%sに憑依できなかった！", m_name);
			}
			else
			{
				msg_format("姉が%sに憑依し支配した！", m_name);
				set_pet(m_ptr);
				m_ptr->mflag |= (MFLAG_EPHEMERA | MFLAG_POSSESSED_BY_SHION);
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;



	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.44 うどんげ(夢)
/*:::夢の世界のうどんげ専用技*/
class_power_type class_power_udonge_d[] =
{
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"メディスンチェスト(収納)",
	"どこかに隠し持っている大きな薬籠にアイテムを入れる。アイテム欄の数はレベルアップで上昇する。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"メディスンチェスト(確認)",
	"薬籠の中を見る。" },
	{ 1,0,0,FALSE,FALSE,A_DEX,0,0,"メディスンチェスト(取出し)",
	"薬籠からアイテムを出す。" },

	{ 5,10,25,FALSE,FALSE,A_STR,0,0,"ルナティッククランプ",
	"利き腕に装備中の「ルナティックガン」の残弾を1つ消費し、銃と同じ威力の低射程の轟音属性ブレスを放つ。" },
	{ 12,20,55,FALSE,FALSE,A_INT,0,0,"波長分析",
	"周囲にあるものを感知する。レベルが上がると感知できるものの種類と範囲が増える。" },
	{ 16,18,30,FALSE,TRUE,A_WIS,0,0,"マインドウェーブ",
	"半物理半精神攻撃のレーザーを放つ。精神の希薄な敵や狂気をもたらす敵、ユニークモンスターには効果が薄い。" },
	{ 20,30,75,FALSE,FALSE,A_INT,0,0,"波長診断",
	"視界内のモンスターの波長を読み取って能力やパラメータなどを調査する。" },

	{ 25,40,60,FALSE,FALSE,A_STR,0,0,"メディスンチェスト(投擲)",
	"薬籠を豪快に投げつける。薬籠の中にあるアイテムは全てばら撒かれて薬は割れる。「*爆発*の薬」は視界内ダメージでなく通常の爆発になる。" },

	{ 30,72,80,FALSE,TRUE,A_DEX,0,0,"ルナティックダブル",
	"視界内のランダムなターゲットに連射する。連射可能数はレベルアップで増加する。銃の命中率が上昇する。一部の特殊な銃器では使用できない。" },

	{ 35,40,70,FALSE,TRUE,A_INT,0,0,"ディスオーダーアイ",
	"近距離視界内の好きな場所に瞬間移動する。このとき次の行動までにかかる時間が少し減少する。" },

	{ 40,80,75,FALSE,TRUE,A_CHR,0,0,"ルナティックレッドアイズ",
	"視界内の全てに対して強力な精神攻撃を行う。" },

	{ 43,64,70,FALSE,TRUE,A_STR,0,0,"ルナティックエコー",
	"利き腕に装備中の「ルナティックガン」の残弾を全て消費し、銃威力*残弾の威力の強力な轟音属性ビームを放つ。" },
	{ 48,180,90,FALSE,TRUE,A_CHR,0,0,"イビルアンジュレーション",
	"敵からの攻撃を三回無効化するバリアを張る。ただしアイテムの破壊や光の剣属性の攻撃は防げない。通常の無敵化と違い解除時に行動遅延しない。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy",	"" },
};
cptr do_cmd_class_power_aux_udonge_d(int num, bool only_info)
{
	int dir, dice, sides, base, damage;
	int plev = p_ptr->lev;
	switch (num)
	{


	case 0:
	{
		if (only_info) return format("");
		if (!put_item_into_inven2()) return NULL;
		break;

	}
	case 1:
	{
		if (only_info) return format("");
		display_list_inven2();
		return NULL; //見るだけなので行動順消費なし

	}
	case 2:
	{
		if (only_info) return format("");
		if (!takeout_inven2()) return NULL;
		break;
	}

	case 3:
	case 11:
	//ルナティッククランプとルナティックエコー
	{
		int hand;
		int dam = 0;
		int range = 2 + plev / 16;

		//両手のルナティックガンのダメージ合計値を計算 しようと思ったがやはり片腕だけにする
		for (hand = 0; hand <= 0; hand++)
		{
			int mult;
			object_type *o_ptr = &inventory[INVEN_RARM + hand];
			int timeout_base, bullets;

			if (o_ptr->tval != TV_GUN || o_ptr->sval != SV_FIRE_GUN_LUNATIC) continue;

			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			bullets = calc_gun_load_num(o_ptr);

			mult = (timeout_base * bullets - o_ptr->timeout) / timeout_base; //銃の残弾数
			if (num == 3) mult = MIN(1,mult); //ルナティッククランプは一発しか使わない

			if (!mult) continue;

			dam += (o_ptr->dd * (o_ptr->ds + 1) / 2 + o_ptr->to_d) * mult;

		}
		if (dam > 9999) dam = 9999;


		if (only_info)
		{
			if (num == 3)
				return format("射程:%d 損傷:%d",range, dam);
			else
				return format("損傷:%d", dam);
		}

		if (dam <= 0)
		{
			msg_print("この装備では技を使えない。");
			return NULL;
		}


		if (num == 3)
		{
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("ルナティックガンを口元に構えて大声を出した！");
			fire_ball(GF_SOUND, dir, dam, -2);
		}
		else
		{
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("狂気の叫びがダンジョンを震わせた！");
			fire_spark(GF_SOUND, dir, dam, 2);
		}

		//残弾減少処理
		for (hand = 0; hand <= 0; hand++)
		{
			int mult;
			object_type *o_ptr = &inventory[INVEN_RARM + hand];
			int timeout_base, bullets, timeout_max;

			if (o_ptr->tval != TV_GUN || o_ptr->sval != SV_FIRE_GUN_LUNATIC) continue;

			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			bullets = calc_gun_load_num(o_ptr);
			timeout_max = timeout_base * (bullets - 1);

			mult = (timeout_base * bullets - o_ptr->timeout) / timeout_base; //銃の残弾数
			if (num == 3) mult = MIN(1, mult); //ルナティッククランプは一発しか使わない

			if (!mult) continue;

			o_ptr->timeout += timeout_base * mult;
			p_ptr->window |= PW_EQUIP;
			//ガンカタ残弾判定のため弾切れチェック
			if (o_ptr->timeout > timeout_max)
				p_ptr->update |= PU_BONUS;


		}


	}
	break;
	case 4:
	{
		int rad = p_ptr->lev / 2 + 10;
		if (only_info) return format("範囲:%d", rad);

		if (use_itemcard)
			msg_format("周囲の色々なことが分かった気がする...");
		else
			msg_format("周囲の波長を分析した・・");
		detect_doors(rad);
		if (p_ptr->lev > 4) detect_monsters_normal(rad);
		if (p_ptr->lev > 9) detect_traps(rad, TRUE);
		if (p_ptr->lev < 15)detect_stairs(rad);
		else map_area(rad);
		if (p_ptr->lev > 19) detect_objects_normal(rad);
		break;
	}
	case 5:
	{
		dice = p_ptr->lev / 5;
		sides = 5 + p_ptr->lev / 10;
		base = p_ptr->lev + 10;
		if (only_info) return format("損傷:%d+%dd%d", base, dice, sides);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("あなたは精神波を放った。");
		fire_beam(GF_REDEYE, dir, base + damroll(dice, sides));
		break;
	}
	case 6:
	{
		if (only_info) return "";

		msg_format("周囲の存在から放たれる波長を読み取った。");
		probing();
		break;
	}
	case 7:
	{
		object_type forge;
		object_type *tmp_o_ptr = &forge;
		int ty, tx;
		if (only_info) return "";

		if (!get_aim_dir(&dir)) return NULL;
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		//ダミーアイテムを生成し特殊投擲ルーチンへ渡す
		msg_print("あなたは薬籠をぶん投げた！");
		object_prep(tmp_o_ptr, lookup_kind(TV_COMPOUND, SV_COMPOUND_MEDICINE_CHEST));

		do_cmd_throw_aux2(py, px, ty, tx, 2, tmp_o_ptr, 0);

		break;
	}


	case 8: //ルナティックダブル
	{
		int shoot_num = (plev -1) / 7;
		if (only_info) return format("連射数:最大%d", shoot_num);

		if (!rapid_fire(shoot_num,0))
		{
			return NULL;
		}

	}
	break;
	case 9:
	{
		int x, y;
		int range = plev / 6 + 2;
		if (only_info) return format("距離:%d",range);

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		x = px + range * ddx[dir];
		y = py + range * ddy[dir];
		if ((dir == 5) && target_okay())
		{
			x = target_col;
			y = target_row;
		}

		project_length = 0;
		if (!cave_player_teleportable_bold(y, x, 0L) ||
			(distance(y, x, py, px) > range) ||
			!projectable(py, px, y, x))
		{
			msg_print("そこへは移動できない。");
			return NULL;
		}
		else if (p_ptr->anti_tele)
		{
			msg_print("不思議な力がテレポートを防いだ！");
		}
		else
		{
			msg_print("あなたは離れた場所に一瞬で現れた。");
			teleport_player_to(y, x, TELEPORT_NONMAGICAL);
			new_class_power_change_energy_need = 100 - plev;
		}
		break;
	}
	case 10:
	{
		damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (only_info) return format("損傷:%d", damage);
		msg_format("あなたの視界は赤く染まった！");
		project_hack2(GF_REDEYE, 0, 0, damage);

		break;
	}
	case 12:
	{
		if (only_info) return format("効力:3回");

		evil_undulation(TRUE, FALSE);

	}
	break;

	default:
		if (only_info) return format("");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}




/*:::クラウンピース(新三月精版)専用技*/
class_power_type class_power_vfs_clownpiece[] =
{

	{ 5,5,20,FALSE,FALSE,A_CHR,0,0,"狂気の松明Ⅰ",
	"隣接したモンスター一体を狂気に冒そうと試みる。狂気に冒されたモンスターは敵味方の区別なく周囲のモンスターと戦い始める。ユニークモンスターには効きづらく、通常の精神を持たないモンスターには効かない。「★クラウンピースの松明」を所持していないと使えない。" },

	{ 16,20,30,FALSE,TRUE,A_DEX,0,3,"ヘルエクリプス",
	"今いる部屋を明るくし、視界内の敵を混乱させる。レベルが上がると朦朧と恐怖も追加される。「★クラウンピースの松明」を所持していないと使えない。" },

	{ 24,72,45,TRUE,TRUE,A_CON,0,30,"グレイズインフェルノ",
	"自分を中心に火炎属性のボールを発生させる。威力はHPの1/3になる。レベル40以降は視界内攻撃になる。" },

	{ 33,30,70,FALSE,TRUE,A_WIS,0,0,"ストライプドアビス",
	"地獄の劫火属性のビームを放つ。" },

	{ 40,45,80,FALSE,TRUE,A_CHR,0,0,"狂気の松明Ⅱ",
	"視界内のモンスター全てを狂気に冒そうと試みる。他の仕様は「狂気の松明」と同じ。" },

	{ 46,66,85,FALSE,TRUE,A_CHR,0,0,"フェイクアポロ",
	"視界内のターゲットの位置に隕石属性のボールを発生させる。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};


cptr do_cmd_class_power_aux_vfs_clownpiece(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		monster_type *m_ptr;
		int y, x;
		int power = plev * 2 + chr_adj * 2 + 20;

		if (use_itemcard) power += 60;

		if (only_info) return  format("効力:%d", power);
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);

			if(use_itemcard)
				msg_format("篝火が狂気の光を放った！");
			else
				msg_format("あなたは%sの前に松明を近づけた……", m_name);

			lunatic_torch(cave[y][x].m_idx, power);
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;
	case 1:
	{
		int power = 10 + p_ptr->lev * 3;
		if (only_info) return format("効力:%d", power);
		msg_format("松明が明るさを増した。");

		lite_area(randint1(power), 3);
		confuse_monsters(power);
		if (plev > 24) stun_monsters(power);
		if (plev > 34) turn_monsters(power);
	}
	break;


	case 2:
	{
		int base = p_ptr->chp / 3;

		if (base < 1) base = 1;

		if (only_info) return format("損傷:～%d", base);

		if (plev < 40)
		{
			msg_format("周囲に火を放った。");
			project(0, 4, py, px, base * 2, GF_FIRE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
		}
		else
		{
			msg_print("熱波が周囲を焼き払った！");
			project_hack2(GF_FIRE, 0, 0, base);
		}

	}
	break;
	case 3:
	{
		int base = plev * 2 + chr_adj * 2 - 10;

		if (only_info) return format("損傷:%d+1d%d", base, base);

		if (!get_aim_dir(&dir)) return NULL;
		msg_format("地獄のパワーを放った！");
		fire_beam(GF_HELL_FIRE, dir, base + randint1(base));
		break;
	}


	case 4:
	{
		int y, x;
		int power = plev * 2 + chr_adj * 2;

		if (only_info) return  format("効力:%d", power);

		msg_format("地獄の松明が煌々と輝いた！");
		project_hack2(GF_LUNATIC_TORCH, 0, 0, power);

	}
	break;

	case 5:
	{
		int x, y;
		int dist = 7;
		int rad = 5;
		monster_type *m_ptr;
		int dice = 5 + chr_adj / 10;
		int sides = plev * 2;

		if (only_info) return format("射程:%d 損傷:%dd%d", dist, dice, sides);
		project_length = dist;
		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("そこを狙うことはできない。");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_print("そこには何もいない。");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("%sを目掛けて隕石が落ちた！", m_name);
			project(0, rad, y, x, damroll(dice, sides), GF_METEOR, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		break;
	}




	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}










//v1.1.45 里乃　実行部は舞のと共有する
class_power_type class_power_satono[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"踊る",
	"配下一体の背後で踊る。背後で踊っている間はそのモンスターの位置に重なり一緒に移動する。モンスターの魔法使用率が魅力に応じて強化され、モンスターの攻撃でプレイヤーが経験値やアイテムを得ることができる。装備が重いときに踊っているとプレイヤーの自然回復速度が低下する。" },

	{ 10,0,20,FALSE,FALSE,A_CHR,0,0,"スカウト",
	"隣接するモンスター一体を配下にしようと試みる。成功率はレベル差と魅力で決まる。ユニークモンスターにはかなり効きづらく、特殊ユニークモンスター、クエスト打倒対象モンスター、精神を持たないモンスターには効果がない。" },

	{ 15,23,30,FALSE,FALSE,A_INT,0,2,"フォゲットユアネーム",
	"隣接するモンスター全てを高確率でフロアから追放する。ユニークモンスターには効果がない。地上やクエストダンジョンでは効果がない。" },

	{ 20,30,40,FALSE,TRUE,A_INT,0,0,"後戸帰還",
	"後戸の国を通って地上へ帰還する。この帰還は「ほぼ」即時に発動する。EXTRAモードでは下の階へレベルテレポートを行う。" },

	{ 25,0,50,FALSE,TRUE,A_INT,0,0,"ビハインドユー",
	"あなたが背後で踊っているモンスターに魔法を使わせる。ブレスやロケットなどは指定できない。魔法の成功率はプレイヤーの魅力とモンスターのレベルで決まる。追加コストの消費はない。治癒・加速・テレポートなどの補助魔法はプレイヤーに対して使われる。プレイヤーとモンスターの両方が行動したものとみなされる。" },

	{ 30,48,60,FALSE,TRUE,A_CHR,0,0,"テングオドシ",
	"周囲のアンデッド・悪魔・妖怪に大ダメージを与えて朦朧とさせる。" },

	{ 34,50,70,FALSE,TRUE,A_CHR,0,0,"メスメリズムダンス",
	"周囲のモンスターに朦朧、混乱、魅了をもたらす。配下の背後で踊っているときには使えない。" },

	{ 37,60,75,FALSE,TRUE,A_CHR,0,0,"パワフルチアーズ",
	"一時的に魅力が大幅に上昇する。通常の限界値を超える。" },

	{ 40,50,85,FALSE,TRUE,A_INT,0,0,"後戸移動",
	"後戸の国を通って指定したモンスターの隣にテレポートする。このときフロア全体のモンスターの位置を感知することができる。通常の倍の行動力を消費する。テレポート不可能な地形には出られない。" },

	{ 45,75,90,FALSE,TRUE,A_CHR,0,0,"クレイジーバックダンス",
	"自分が背後で踊っているモンスターを一時的に無敵化する。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

//v1.1.41 舞
class_power_type class_power_mai[] =
{

	{ 1,0,0,FALSE,FALSE,A_CHR,0,0,"踊る",
	"配下一体の背後で踊る。背後で踊っている間はそのモンスターの位置に重なり一緒に移動する。モンスターの隣接攻撃、AC、HP回復速度が魅力に応じて強化され、モンスターの攻撃でプレイヤーが経験値やアイテムを得ることができる。装備が重いときに踊っているとプレイヤーの自然回復速度が低下する。" },

	{ 10,0,20,FALSE,FALSE,A_CHR,0,0,"スカウト",
	"隣接するモンスター一体を配下にしようと試みる。成功率はレベル差と魅力で決まる。ユニークモンスターにはかなり効きづらく、特殊ユニークモンスター、クエスト打倒対象モンスター、精神を持たないモンスターには効果がない。" },

	{ 15,5,30,TRUE,FALSE,A_DEX,0,3,"バンブースピアダンス",
	"射撃属性のビームを放つ。槍を装備していないと使えない。威力は装備中の槍によって変動する。" },

	{ 20,30,40,FALSE,TRUE,A_INT,0,0,"後戸帰還",
	"後戸の国を通って地上へ帰還する。この帰還は「ほぼ」即時に発動する。EXTRAモードでは下の階へレベルテレポートを行う。" },

	{ 25,30,50,FALSE,TRUE,A_CHR,0,0,"タナバタスターフェスティバル",
	"自分自身と自分が背後で踊っているモンスターの体力を回復しステータス異常を治療する。" },

	{ 30,48,60,FALSE,TRUE,A_CHR,0,0,"テングオドシ",
	"周囲のアンデッド・悪魔・妖怪に大ダメージを与えて朦朧とさせる。" },

	{ 34,50,70,FALSE,TRUE,A_DEX,0,0,"バンブーラビリンス",
	"周囲に森地形を生成する。配下の背後で踊っているときには使えない。" },

	{ 37,60,75,FALSE,TRUE,A_CHR,0,0,"パワフルチアーズ",
	"一時的に魅力が大幅に上昇する。通常の限界値を超える。" },

	{ 40,50,85,FALSE,TRUE,A_INT,0,0,"後戸移動",
	"後戸の国を通って指定したモンスターの隣にテレポートする。このときフロア全体のモンスターの位置を感知することができる。通常の倍の行動力を消費する。テレポート不可能な地形には出られない。" },

	{ 45,75,90,FALSE,TRUE,A_CHR,0,0,"クレイジーバックダンス",
	"自分が背後で踊っているモンスターを一時的に無敵化する。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_mai(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		int chr = p_ptr->stat_ind[A_CHR] + 3;
		int mult = 100 + MAX(0, 100 * (chr - 5) / 15);
		if (only_info)
		{
			if(p_ptr->pclass == CLASS_MAI)
				return format("近接強化:x%d.%02d", mult / 100, mult % 100);
			else
				return format("魔法使用率:+%d", chr);
		}

		if(!do_riding(FALSE)) return NULL;


	}
	break;
	case 1://スカウト
	{
		int y, x;
		int power = 20 + plev + chr_adj;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("効力:%d", power);

		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}


		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			if (one_in_(64))
			{
				msg_format("あなたは、ぐるぐるとおどった。");
				power *= 3;
			}
			else if (one_in_(4))
			{
				if(p_ptr->pclass == CLASS_MAI)
					msg_format("「君のようなアグレッシブな人材を探していたんだ！」");
				else
					msg_format("「ようこそ後戸の国へ。」");
			}
			else
				msg_format("あなたは%sを後戸の国へスカウトした。", m_name);

			if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				msg_format("%sは無反応だ。", m_name);
			}
			else if (is_pet(m_ptr))
			{
				msg_format("%sはすでにスカウト済みだ。", m_name);
			}
			else if (r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_QUESTOR) || r_ptr->flags7 & RF7_VARIABLE || power < r_ptr->level)
			{
				msg_format("%sはあなたの誘いに耳を貸さない！", m_name);
			}
			//ユニークモンスターを仲間にするにはweird_luck要
			else if (randint1(power) < r_ptr->level || (r_ptr->flags1 & RF1_UNIQUE) && !weird_luck())
			{
				msg_format("%sはあなたの誘いに乗らなかった。", m_name);
			}
			else
			{
				msg_format("%sはあなたに従った。", m_name);
				set_pet(m_ptr);
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;
	case 2:
	if(!use_itemcard && p_ptr->pclass == CLASS_MAI)//バンブースピアダンス
	{
		int damage = 0;
		int range = 2 + plev / 15;

		if(inventory[INVEN_RARM].tval == TV_SPEAR)
			damage = calc_weapon_dam(0) / p_ptr->num_blow[0];
		else if (inventory[INVEN_LARM].tval == TV_SPEAR)
			damage = calc_weapon_dam(1) / p_ptr->num_blow[1];

		if (only_info) return format("射程:%d 損傷:%d", range, damage);

		if (damage < 1)
		{
			msg_print("槍を装備していないとこの技を使うことはできない。");
			return NULL;
		}

		project_length = range;
		if (!get_aim_dir(&dir)) return NULL;
		//msg_print("突きかかった！");
		fire_beam(GF_ARROW, dir, damage);
	}
	else//フォゲットユアネーム、もしくはアイテムカードの茗荷
	{
		int power = 100 + plev * 4 + chr_adj * 10;
		if (only_info) return format("効力:%d",power/2);

		msg_print("あなたは茗荷を揺らして踊った...");
		project(0, 1, py, px, power, GF_GENOCIDE, PROJECT_HIDE | PROJECT_JUMP | PROJECT_KILL, -1);

	}
	break;
	case 3:
	{
		if (only_info) return "";
		int m_idx;

		msg_print("後戸の国への扉が開いた。");

		if (EXTRA_MODE)
		{
			teleport_level(0);
		}
		else
		{
			recall_player(1);
		}
	}
	break;
	case 4:
	if(use_itemcard || p_ptr->pclass == CLASS_MAI) //舞のタナバタスター、あるいはアイテムカードの笹
	{
		int heal = 50 + chr_adj * 5;
		int heal_percen = 10 + chr_adj / 5;

		if (use_itemcard && heal < 100) heal = 100;

		if (only_info) return format("回復:%d/%d%%",heal,heal_percen);

		msg_print("星神の力があなたを癒やす...");

		hp_player(heal);
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);

		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			int heal2;
			char m_name[80];

			set_monster_stunned(p_ptr->riding, 0);
			set_monster_confused(p_ptr->riding, 0);
			set_monster_slow(p_ptr->riding, 0);
			if (m_ptr->hp < m_ptr->maxhp)
			{
				monster_desc(m_name, m_ptr, 0);
				heal2 = m_ptr->maxhp * heal_percen / 100;
				if (!heal2) heal2 = 1;
				m_ptr->hp += heal2;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				msg_format("%^sの傷が少し癒えた。", m_name);
				p_ptr->redraw |= (PR_HEALTH);
				p_ptr->redraw |= (PR_UHEALTH);
			}
		}

	}
	else //里乃のビハインドユー
	{
		if (only_info) return format("　");

		if (!p_ptr->riding)
		{
			msg_print("今は踊っていない。");
			return NULL;
		}

		if (!cast_monspell_new()) return NULL;

	}
	break;
	case 5:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("損傷:%d", dam);

		msg_print("あなたは狂おしく跳ね踊った！");
		dispel_undead(dam);
		dispel_demons(dam);
		dispel_kwai(dam);

	}
	break;

	case 6:
	{
		if (p_ptr->pclass == CLASS_MAI)
		{
			if (only_info) return "";

			msg_print("あなたは地面を踏み鳴らした。竹が次々に生えてきて生垣になった！");

			tree_creation();
		}
		else
		{
			int power = plev + chr_adj * 5;
			if (only_info) return format("効力:%d", power);

			msg_print("あなたは幻惑的な踊りを披露した。");
			confuse_monsters(power);
			stun_monsters(power);
			charm_monsters(power);

		}



	}
	break;

	case 7:
	{
		int v;
		bool base = 10;
		if (only_info) return format("期間:%d+1d%d", base, base);

		v = base + randint1(base);

		msg_print("あなたの踊りは激しさを増した！");
		set_tim_addstat(A_CHR, 100 + plev / 5, v, FALSE);

		break;
	}

	case 8:
	{
		if (only_info) return format("距離:なし");

		msg_print("あなたは後戸の国を覗き込んだ...");
		detect_monsters_normal(255);

		if (p_ptr->paralyzed || p_ptr->confused || p_ptr->image)
		{
			msg_print("あなたは移動に失敗した！");
		}
		else
		{
			//テレポートに成功したらTRUEが帰って行動力二倍
			if (dimension_door(D_DOOR_BACKDOOR))
				new_class_power_change_energy_need = 200;
		}



	}
	break;


	case 9:
	{
		int v;
		bool base = 6;
		if (only_info) return format("期間:%d+1d%d", base, base);
		v = base + randint1(base);

		if (p_ptr->riding)
		{
			char m_name[80];
			monster_desc(m_name, &m_list[p_ptr->riding], 0);
			msg_format("%sは潜在能力が開放されて無敵になった！",m_name);
			set_monster_invulner(p_ptr->riding, v, FALSE);
		}

	}
	break;

	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}





//v1.1.39 ラルバ
class_power_type class_power_larva[] =
{

	{ 6, 7,10,FALSE,FALSE,A_CON,0,3,"悪臭攻撃",
	"自分を中心に毒属性のボールを発生させる。体力が半分以下になると威力が大幅に上がり、さらに周囲の敵を混乱・朦朧・恐怖させる。" },
	{ 10,10,20,FALSE,FALSE,A_DEX,0,2,"纏わりつく鱗粉",
	"自分を中心に遅鈍属性のボールを発生させる。" },
	{ 15, 0,20,TRUE,FALSE,A_WIS,0,0,"再生",
	"HPと状態異常をわずかに回復する。満腹度が少し減少する。" },
	{ 24,24,30,TRUE,FALSE,A_DEX,0,10,"ミニットスケールス",
	"自分を中心に混乱属性のボールを発生させる。" },
	{ 30,20,50,FALSE,TRUE,A_CON,0,0,"変異強力発動",
	"変異によるレイシャルパワーを二倍の威力で発動する。" },
	{ 33,30,60,FALSE,TRUE,A_DEX,0,0,"アゲハの鱗粉",
	"自分を中心にカオス属性のボールを発生させる。" },
	{ 36,50,50,FALSE,TRUE,A_INT,0,0,"バタフライドリーム",
	"一定時間経過後に現在のフロアを再構成する。" },
	{ 40,150,90,FALSE,TRUE,A_CON,0,0,"変異",
	"ランダムな突然変異を得る。" },
	{ 48,64,80,FALSE,TRUE,A_CHR,0,0,"真夏の羽ばたき",
	"一時的に種族が変わり大幅にパワーアップする。" },

	{ 99,0,0,FALSE,FALSE,0,0,0,"dummy","" },
};

cptr do_cmd_class_power_aux_larva(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch (num)
	{

	case 0:
	{
		bool flag = FALSE;
		int power = 5 + plev/2;

		if (p_ptr->chp <= p_ptr->mhp / 2)
		{
			power *= 4;
			flag = TRUE;
		}

		if (only_info) return format("損傷:～%d", power);

		if (flag)
		{
			msg_print("あなたは鼻が曲がりそうな悪臭を放った！");
			project(0, 5, py, px, power*2, GF_POIS, (PROJECT_KILL | PROJECT_JUMP), -1);
			confuse_monsters(power);
			stun_monsters(power);
			turn_monsters(power);
		}
		else
		{
			msg_print("あなたは頭の臭角から嫌な匂いを出した...");
			project(0, 3, py, px, power*2, GF_POIS, (PROJECT_KILL | PROJECT_JUMP), -1);
		}

	}
	break;
	case 1:
	{
		int dam = 10 + plev + chr_adj ;
		int rad = 1 + plev / 30;
		if (only_info) return format("損傷:～%d", dam / 2);

		msg_print("鱗粉をまき散らした。");
		project(0, rad, py, px, dam, GF_INACT, (PROJECT_KILL | PROJECT_JUMP), -1);

	}
	break;

	case 2://再生
	{
		int base = p_ptr->lev / 5;
		if (only_info) return format("回復:%d+1d%d", base, base);

		if (p_ptr->food < PY_FOOD_WEAK)
		{
			msg_print("お腹が空いて集中できない。");
			return NULL;
		}

		msg_print("あなたは自分の体に意識を集中した..");
		hp_player(base + randint1(base));
		set_poisoned(p_ptr->poisoned - base);
		set_stun(p_ptr->stun - base);
		set_cut(p_ptr->cut - base);
		set_image(p_ptr->image - base);
		set_food(p_ptr->food - 50);

	}
	break;
	case 3://ミニットスケールス
	{
		int dam = plev * 2 + chr_adj * 2;
		int rad = plev / 15;
		if (only_info) return format("損傷:～%d", dam / 2);

		msg_print("鱗粉をまき散らした。");
		project(0, rad, py, px, dam, GF_CONFUSION, (PROJECT_KILL | PROJECT_JUMP), -1);

	}
	break;

	case 4://変異強力発動
	{
		if (only_info) return "";

		msg_print("あなたは力強いオーラをまとった..");

		hack_flag_powerup_mutation = TRUE;
		do_cmd_racial_power(FALSE);
		hack_flag_powerup_mutation = FALSE;

		//-hack- レイシャル発動をキャンセルしたかどうかを行動力消費で判定
		if (!energy_use) return NULL;

	}
	break;

	case 5://アゲハの鱗粉
	{
		int dam = plev * 4 + chr_adj * 4;
		int rad = 1 + plev / 15;
		if (only_info) return format("損傷:～%d", dam / 2);

		msg_print("妖しく輝く鱗粉をまき散らした...");
		project(0, rad, py, px, dam, GF_CHAOS, (PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM), -1);

	}
	break;

	case 6:
	{
		if (only_info) return "";

		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}

		msg_print("あなたは自分の存在に疑問を持った...");

		alter_reality();
	}
	break;


	case 7:
	{
		if (only_info) return "";

		msg_print("あなたは体を丸めて目を閉じた...");
		gain_random_mutation(0);

	}
	break;
	case 8:
	{
		int base = 7;
		int time;
		if (only_info) return format("期間:%d+1d%d", base, base);

		time = randint1(base) + base;

		set_mimic(time, MIMIC_GOD_OF_NEW_WORLD, FALSE);
		set_oppose_fire(time, FALSE);
		set_hero(time, FALSE);
	}
	break;

	default:
		if (only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d", num);
		return NULL;
	}
	return "";
}



//v1.1.37 邪鬼に乗っ取られたときの小鈴
class_power_type class_power_kosuzu_hyakki[] =
{

	{10,10,30,FALSE,TRUE,A_WIS,0,0,"百鬼夜行",
		"多数の付喪神を召喚する。"},
	{20,20,40,FALSE,TRUE,A_INT,0,0,"百鬼の力",
		"周囲の付喪神(小)(中)(古)を吸収し、HPを大幅に回復する。"},
	{30,20,50,FALSE,TRUE,A_STR,0,0,"付喪神ブレス",
		"現在HPの1/6の威力の破片属性のブレスを吐く。"},





	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu_hyakki(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //付喪神召喚
		{
			int i;
			bool flag = FALSE;
			int base = plev / 10;
			if (only_info) return "";

			i = base + randint1(base);

			for(;i>0;i--)
			{
				if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_TSUKUMO,(PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))) flag = TRUE;
			}
			if(flag)
				msg_print("どこからともなく付喪神たちが現れた。");
			else
				msg_print("特に何も起こらなかった。");

		}
		break;
	case 1://付喪神吸収
		{
			if(only_info) return "";

			msg_print("影の鬼が大口を開けて吠えた！");
			absorb_tsukumo(0);
		}
		break;
	case 2:
		{
			int dam = p_ptr->chp / 6;
			if(dam<1) dam = 1;
			if(dam > 600) dam=600;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;

			msg_print("付喪神たちを吹きつけた！");

			fire_ball(GF_SHARDS, dir, dam, -2);
			break;
		}



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

//v1.1.37 怨霊に乗っ取られたときの小鈴
class_power_type class_power_kosuzu_ghost[] =
{

	{10,20,30,FALSE,TRUE,A_CHR,0,0,"魅了",
		"隣接したモンスター一体を魅了して配下にしようと試みる。特に男性や眠っているモンスターに効果が高い。"},
	{20,10,40,FALSE,TRUE,A_INT,0,0,"瞬間移動",
		"長距離をテレポートする。"},
	{30,20,50,FALSE,TRUE,A_CHR,0,0,"生命力吸収",
		"隣接した生物一体からHPを吸収し腹を満たす。"},
	{35,30,60,FALSE,TRUE,A_DEX,0,0,"変わり身",
		"攻撃を受けたとき中距離テレポートで逃れるようになる。失敗することもある。"},
	{40,50,70,FALSE,TRUE,A_DEX,0,0,"壁抜け",
		"一定時間、壁を抜けられるようになる。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu_ghost(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //魅了
		{
			int y, x;
			int power = 10 + plev + chr_adj * 2;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("効力:%d",power);

			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags1 & RF1_MALE) power *= 2;
				if(MON_CSLEEP(m_ptr)) power *= 2;
				if(r_ptr->flags3 & RF3_NO_CONF) power /= 2;

				msg_format("あなたは%sに向けて微笑んで手招きした...",m_name);

				if(r_ptr->flagsr & RFR_RES_ALL || r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
				{
					msg_format("%sには効果がない！",m_name);
				}
				else if(randint1(power) < r_ptr->level)
				{
					msg_format("%sは抵抗した。",m_name);
				}
				else
				{
					msg_format("%sはあなたに従った。",m_name);
					set_pet(m_ptr);
				}
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;
	case 1://テレポート
		{
			int dist = 25 + plev / 2;
			if(only_info) return format("距離:%d",dist);
			msg_print("あなたは瞬時に消えた。");
			teleport_player(dist, 0L);
		}
		break;

	case 2: //生命力吸収
		{
			int y, x;
			int dam = plev + chr_adj * 5;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("効力:%d",dam);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int food = 0;
				char m_name[80];

				r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);
				if(!monster_living(r_ptr))
				{
					msg_format("%sからは生命力を奪えなかった。",m_name);
				}
				else
				{
					msg_format("あなたは%sへ触れ、生命力を吸収した！",m_name);

					if(m_ptr->hp < dam) dam = m_ptr->hp+1;

					if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX - 1, (p_ptr->food + dam * 10)));
					hp_player(dam);
					project(0,0,y,x,dam,GF_DISP_ALL,PROJECT_KILL,0);
				}
				touch_zap_player(m_ptr);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;
	case 3:
		{
			if(only_info) return "";
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("あなたの周囲に恋文が舞い始めた...");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 4:
		{
			int base = 20;
			if(only_info) return format("期間:%d+1d%d",base,base);


			set_kabenuke(base + randint1(base), FALSE);

			break;
		}
	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


//v1.1.37 小鈴
class_power_type class_power_kosuzu[] =
{

	{1,0,0,FALSE,FALSE,A_INT,0,0,"巻物鑑定",
		"巻物を鑑定する。"},
	{7,0,25,FALSE,TRUE,A_INT,0,0,"魔導書胡瓜遣",
		"満腹度を回復する。"},
	{14,10,50,FALSE,TRUE,A_INT,0,0,"著者不明の易書",
		"フロアの雰囲気を感知する。レベル20で近くのトラップ、レベル30で近くのモンスターも感知する。"},
	{21,0,50,FALSE,TRUE,A_INT,0,0,"今昔百鬼拾遺稗田写本",
		"このフロア限定でモンスター「煙々羅」を配下として召喚する。"},
	{28,0,0,FALSE,TRUE,A_INT,0,0,"怨霊の艶書",
		"怨霊に取り憑かれる。取り憑かれている間は種族や特技が変化する。"},
	{35,20,60,FALSE,TRUE,A_CHR,0,0,"狐文字のノート",
		"このフロア限定でモンスター「妖怪狐」を配下として一体召喚する。"},
	{42,80,70,FALSE,TRUE,A_INT,0,0,"私家版百鬼夜行絵巻最終章補遺",
		"絵巻物の邪鬼に取り憑かれる。取り憑かれている間は種族や特技が変化する。★私家版百鬼夜行絵巻最終章補遺を装備していないと使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kosuzu(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0: //巻物鑑定
		{
			if (only_info) return format("");
			if (!ident_spell_2(4)) return NULL;
		}
		break;

	case 1: //
		{
			if(only_info) return "";
			if(p_ptr->food >= PY_FOOD_FULL)
			{
				msg_print("今は満腹だ。");
				return NULL;
			}

			msg_print("どこからともなく「絶品野菜コロッケ」が出現した！");
			set_food(PY_FOOD_MAX - 1);

			break;
		}
	case 2:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたは筮竹を取り出し、記憶を頼りに卦を立て始めた...");
			p_ptr->feeling_turn = 0;

			if(plev > 19) detect_traps(rad,TRUE);
			if(plev > 29) detect_monsters_normal(rad);
			if(plev > 29) detect_monsters_invis(rad);

			break;
		}
	case 3:
		{
			int max_num = (plev-10) / 10;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("最大:%d",max_num);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_ENENRA) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("もう本は空白だ。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_ENENRA, PM_EPHEMERA))
			{
				msg_print("本の中から朦々と煙が湧き出した...");

			}

		}
		break;

	case 4:
		{
			int base = 1000;
			if(only_info) return format("期間:%d+1d%d",base,base);

			set_mimic(base + randint1(base), MIMIC_KOSUZU_GHOST, FALSE);

			break;
		}
	case 5:
		{
			int max_num = 1;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("最大:%d",max_num);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				//カメレオンフラグと一時的フラグを持つモンスターで小狐を判定。何かアイテムカード使えば他のルートでもこのモンスターが出るかもしれないがまあ解釈上問題はないだろう
				if( (m_ptr->mflag2 & MFLAG2_CHAMELEON) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("もう小狐を呼んでいる。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_CHAMELEON, PM_EPHEMERA))
			{
				msg_print("馴染みの小狐が変身してあなたを助けに来た！");
			}

		}
		break;

	case 6:
		{
			int base = 100;
			if(only_info) return format("期間:%d+1d%d",base,base);

			set_mimic(base + randint1(base), MIMIC_KOSUZU_HYAKKI, FALSE);

			break;
		}




	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


//成美「理由なく好戦的な」のときの特技テーブル
class_power_type class_power_narumi2[] =
{

	{7,3,20,FALSE,TRUE,A_WIS,0,0,"生命感知",
		"広範囲の生命を持つモンスターを感知する。"},

	{15,5,25,FALSE,TRUE,A_INT,0,0,"魔力感知",
		"近くの魔法がかかったアイテム(魔道具、上質以上の装備品など)を感知する。"},

	{20,12,45,FALSE,TRUE,A_INT,0,4,"生命操作α",
		"生命を持つモンスター一体にダメージを与え、ダメージの一部をMPとして吸収する。"},

	{25,20,40,FALSE,TRUE,A_WIS,0,6,"即席菩提",
		"周囲に破邪属性攻撃を行い、朦朧、恐怖させようと試みる。抵抗されると無効。破邪弱点でないモンスターには効果がない。"},

	{30,40,50,FALSE,TRUE,A_WIS,0,0,"生命操作β",
		"生命を持つモンスター一体の体力を半分にする。妖精の場合一撃で倒す。抵抗されると無効。"},

	{ 33,33,40,TRUE,FALSE,A_STR,30,10,"菩薩ストンプ",
		"近くの指定したグリッドに落下し、そこにモンスターがいればダメージを与える。威力は自分のACに依存する。" },

	{37,35,60,FALSE,TRUE,A_INT,0,10,"ペットの巨大弾生命体",
		"近くのランダムなモンスターに対し、巨大な気属性のボールを数回放つ。"},

	{40,72,75,FALSE,TRUE,A_WIS,0,0,"生命操作γ",
		"視界内の生命を持つモンスターに大ダメージを与える。"},

	{45,100,90,FALSE,TRUE,A_CHR,0,0,"業火救済",
		"ターゲットの位置に強力な破邪属性のボールを発生させる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_narumi2(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたはその場に佇んで生命の気配を探した..");
			detect_monsters_living(rad);

			break;
		}
	case 1:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたは周囲の魔力を探った..");
			detect_objects_magic(rad);
			break;
		}
	case 2:
		{
			int y, x;
			int dam = plev * 3;
			int gain_mana;
			if(only_info) return format("効果:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;

			/* v1.1.36 fire_ball_jumpの戻り値はproject()の戻り値ではなく、成否にかかわらずMPが回復していたので修正
			if(fire_ball_jump(GF_OLD_DRAIN,dir,dam,0,"あなたは敵から生命力を吸い取ろうと試みた.."))
			{
				if(player_gain_mana(dam/3))
					msg_print("あなたの魔力が回復した！");
			}
			*/
			x = target_col;
			y = target_row;

			if (dir != 5 || !target_okay() || !projectable(y, x, py, px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			if (!cave[y][x].m_idx)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}


			//GF_OLD_DRAINは効果がなかったときobvious=FALSEとなりproject_hookがFALSEを返す
			if (project_hook(GF_OLD_DRAIN, dir, dam, (PROJECT_KILL | PROJECT_HIDE | PROJECT_JUMP)))
			{
				if (player_gain_mana(dam/3))
					msg_print("あなたの魔力が回復した！");

			}


			break;
		}
	case 3:
		{
			int dam = 160 + plev * 4 + chr_adj * 3;

			if(only_info) return format("損傷:最大%d",dam);
			msg_print("あなたは念仏を唱え始めた...");
			project(0,6,py,px,dam,GF_PUNISH_4,(PROJECT_KILL|PROJECT_JUMP|PROJECT_HIDE),-1);

			break;
		}

	case 4:
		{
			int y, x, base;
			int m_idx;
			monster_type *m_ptr;
			monster_race *r_ptr;
			char m_name[80];
			int dam=0;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;

			x = target_col;
			y = target_row;

			if(dir != 5 || !target_okay() || !projectable(y,x,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			m_idx = cave[y][x].m_idx;
			if(!m_idx)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			m_ptr = &m_list[m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);
			msg_format("あなたは%sを本気で無力化しようとした..",m_name);

			if(r_ptr->flagsr & RFR_RES_ALL)
			{
				msg_format("%sには完全な耐性がある！",m_name);
				break;
			}

			//妖精には無条件で効く
			if(r_ptr->flags3 & RF3_FAIRY)
			{

				dam = m_ptr->hp +1;
			}
			//ユニークと無生物には効かない
			else if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2 || !monster_living(r_ptr))
			{
				;
			}
			//破滅の手と同じ判定にしておく
			else if(((plev*2 ) + randint1(plev*2)) > (r_ptr->level  + randint1(120)))
			{
				dam = m_ptr->hp / 2 + 1;
			}

			if(dam)
			{
				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_MISSILE,(PROJECT_JUMP|PROJECT_KILL),-1);
			}
			else
			{
				msg_format("%sは抵抗した！",m_name);
				//起こす
				set_monster_csleep(m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}

	case 5:

		{
			int range = 4 + plev / 40;
			int base = p_ptr->ac + p_ptr->to_a;
			int tx, ty, dam;

			if (base < 0) base = 0;

			if (only_info) return format("損傷:%d+1d%d", base / 2, base / 2);

			if (base < 1)
			{
				msg_print("今のあなたの柔らかさでは虫一匹潰せないだろう。");
				return NULL;
			}

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base / 2);
			ty = target_row;
			tx = target_col;
			if (!fire_ball_jump(GF_ARROW, dir, dam, 0, "あなたは飛び上がり、標的目掛けて落下した！")) return NULL;
			teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
			//周りがみっしり敵のときどうなるのかチェック要
			break;
		}


	case 6:
		{
			bool flag = FALSE;
			int i;
			int dice = 4 + plev / 6;
			int sides = 11 + chr_adj/ 6;
			int num = (plev+12) / 10;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("巨大な弾幕のゴーレムが辺りの敵を攻撃した！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_FORCE, damroll(dice,sides),4, 2,3)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし近くに敵がいなかった。");
					break;
				}
			}

		}
		break;

	case 7:
		{
			int base = (plev * 5 + chr_adj * 5) / 2;
			if(only_info) return format("損傷:%d+1d%d",base,base);

			msg_print("あなたは自重せずに能力を開放した！");
			dispel_living(base+randint1(base));

		}
		break;
	case 8:
		{

			int base = plev * 6;
			int sides = chr_adj * 12;
			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_jump(GF_HOLY_FIRE,dir,base+randint1(sides),5,(randint0(4)?"あなたは有り余る力の全てをぶつけた！":"「残念無念、また来世ー！」"));

		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




class_power_type class_power_narumi[] =
{

	{7,3,20,FALSE,TRUE,A_WIS,0,0,"生命感知",
		"広範囲の生命を持つモンスターを感知する。"},

	{15,5,25,FALSE,TRUE,A_INT,0,0,"魔力感知",
		"近くの魔法がかかったアイテム(魔道具、上質以上の装備品など)を感知する。"},

	{20,4,45,TRUE,TRUE,A_INT,0,2,"生命操作Ⅰ",
		"HPを少し消費して同量のMPを回復する。"},

	{25,20,40,FALSE,TRUE,A_WIS,0,0,"インスタントボーディ",
		"一時的に知能と賢さが上昇する。レベル40以降は祝福の効果も追加される。"},

	{30,24,50,FALSE,TRUE,A_WIS,0,0,"生命操作Ⅱ",
		"周囲の生物を減速、金縛り状態にしようと試みる。妖精には特に効きやすい。"},

	{33,33,40,TRUE,TRUE,A_CHR,0,10,"慈愛の地蔵",
		"周囲のモンスターの体力を回復し状態異常を治療する。自分に近いほど回復量が多い。効果は魅力に大きく依存する。" },

	{37,30,60,FALSE,TRUE,A_INT,0,0,"バレットゴーレム",
		"モンスター「バレットゴーレム」をこのフロア限定で呼び出す。"},

	{40,50,75,FALSE,TRUE,A_WIS,0,0,"生命操作Ⅲ",
		"自分のHPと経験値を全回復する。"},

	{45,80,85,FALSE,TRUE,A_CHR,0,0,"クリミナルサルヴェイション",
		"一度だけ地獄の劫火属性の攻撃を無効化する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_narumi(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたはその場に佇んで生命の気配を探した..");
			detect_monsters_living(rad);

			break;
		}
	case 1:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたは周囲の魔力を探った..");
			detect_objects_magic(rad);
			break;
		}
	case 2:
		{
			int mana = plev / 5;
			if(only_info) return format("回復:%d",mana);
			msg_print("あなたは生命力を魔力に変換した。");
			player_gain_mana(mana);
			break;
		}
	case 3:
		{
			int v;
			bool base = 20;
			if(only_info) return format("期間:%d+1d%d",base,base);

			v = base + randint1(base);

			msg_print("あなたは瞑目合掌した..");
			set_tim_addstat(A_INT,102+plev/40,v,FALSE);
			set_tim_addstat(A_WIS,102+plev/40,v,FALSE);
			if(plev > 39) set_blessed(v,FALSE);

			break;
		}

	case 4:
		{
			int power = 20 + plev * 2;
			if(only_info) return format("");

			msg_print("あなたは生命あるものを無力化しようと試みた...");
			project_hack(GF_STASIS_LIVING,power);

		}
		break;

	case 5:
		{
			int rad = 5;
			int heal = plev * 4 + chr_adj * 10;

			if (only_info) return format("回復:～%d", heal / 2);

			msg_print("あなたの慈悲の心が辺りを優しく包んだ…");
			project(0, rad, py, px, heal, GF_STAR_HEAL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE),-1);
			break;

		}
		break;

	case 6:
		{
			int max_num = 1 + (plev-35)/5;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(plev > 40) max_num++;
			if(only_info) return format("最大:%d",max_num);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_BULLET_GOLEM) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("これ以上作り出せない。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_BULLET_GOLEM, PM_EPHEMERA))
			{
				msg_print("あなたはゴーレムのような弾幕を作り出した。");

			}

		}
		break;

	case 7:
		{
			if(only_info) return format("");

			msg_print("生命力がみなぎってきた！");
			hp_player(5000);
			restore_level();

		}
		break;
	case 8:
		{

			if(only_info) return format("");

			msg_print("地蔵菩薩の力があなたを守っている気がする…");

			p_ptr->special_defense |= SD_HELLFIRE_BARRIER;
			p_ptr->redraw |= (PR_STATUS);

		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



class_power_type class_power_aunn[] =
{
	{5,5,15,FALSE,TRUE,A_WIS,0,0,"*神格感知*",
		"フロアにいる神格をもつモンスターを全て感知する。"},

	{12,10,20,FALSE,TRUE,A_WIS,0,0,"妖怪・邪悪感知",
		"広範囲の妖怪と混沌勢力モンスターを感知する。"},

	{20,20,30,TRUE,FALSE,A_DEX,20,0,"山狗の散歩",
		"ターゲットに向けて短距離を移動し、そのまま攻撃する。装備が重いと失敗しやすい。"},

	{30,25,40,FALSE,FALSE,A_CON,0,0,"石像化",
		"モンスターから認識されなくなる。視界内にモンスターがいる状態では使用できない。ダメージを受けたり待機、休憩、飲食など以外の行動をすると効果が切れる。"},

	{35,35,60,TRUE,FALSE,A_STR,50,10,"コマ犬回し",
		"周囲のモンスター全てに攻撃する。装備が重いと失敗しやすい。"},

	{40,50,70,FALSE,FALSE,A_CHR,0,0,"１０１匹の野良犬",
		"友好的な犬系モンスターを大量に召喚する。" },

	{43,72,85,FALSE,TRUE,A_CHR,0,0,"一人阿吽の呼吸",
		"ごく短期間分身し、モンスターからの攻撃の半分を回避する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_aunn(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int rad = 255;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたはその場に佇んで感覚を研ぎ澄ませた..");
			detect_monsters_xxx(rad,RF3_DEITY);

			break;
		}
	case 1:
		{
			int rad = 25 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたは敵の存在を探った..");
			detect_monsters_xxx(rad,RF3_KWAI);
			detect_monsters_evil(rad);
			break;
		}
	case 2:
		{
			int len = plev / 12;
			if(len < 2) len = 2;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
		//石像化　オカルト魔法のエイボンの処理に便乗する。アイテムカードからも使われる。
	case 3:
		{
			int i;
			bool flag_ng = FALSE;
			if(only_info) return format("期間:--");

			for (i = m_max - 1; i >= 1; i--)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!projectable(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if (!is_hostile(m_ptr)) continue;
				if(MON_CSLEEP(m_ptr)) continue;
				if(MON_CONFUSED(m_ptr)) continue;

				flag_ng = TRUE;
				break;
			}

			if(flag_ng)
			{
				msg_print("敵に見られているので石像化できない。");
				return NULL;
			}

			msg_print("あなたは石像に変身した！");
			p_ptr->special_defense |= (SD_EIBON_WHEEL | SD_STATUE_FORM);
			break_eibon_flag = FALSE;
			p_ptr->redraw |= (PR_STATUS);
			break;
		}

	case 4:
		{
			if(only_info) return format("");

			msg_print("あなたは目まぐるしく跳ね回った！");
			whirlwind_attack();
		}
		break;
		case 5:
		{
			int level;
			int num = 10;
			level = plev / 2 + chr_adj / 2;
			bool flag = FALSE;
			if (only_info) return format("召喚レベル:%d", level);
			for (; num>0; num--)
			{
				if (summon_specific(-1, py, px, level, SUMMON_HOUND, (PM_FORCE_FRIENDLY | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if (flag) msg_format("野良犬たちが現れた！");
			else msg_format("何も現れなかった・・");

		}
		break;

	case 6:
		{
			int base = 6;
			if(only_info) return format("期間:%d+1d%d",base,base);

			set_multishadow(base + randint1(base),FALSE);
			break;
		}


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


class_power_type class_power_nemuno[] =
{

	{7,5,10,FALSE,FALSE,A_DEX,0,0,"食料生成",
		"食料を調達する。"},

	{15,30,30,TRUE,FALSE,A_CHR,0,20,"聖域作成Ⅰ",
		"誰もいない部屋を「縄張り」にする。縄張りの中では能力が大幅に上昇し、恐怖耐性、麻痺耐性、急回復を得る。また縄張りに入ってきたモンスターを常に感知する。縄張りから離れている間に他のモンスターが縄張りに入ると縄張りを奪われることがある。"},

	{20,30,40,FALSE,TRUE,A_WIS,0,0,"囚われの秋雨",
		"縄張りの中全てに低威力の遅鈍属性攻撃を行う。" },

	{25,25,50,FALSE,FALSE,A_STR,0,0,"山姥の包丁研ぎ",
		"装備中の武器に一時的に「切れ味」を付加する。すでに切れ味のある武器は「*切れ味*」になる。刃のある武器を装備していないと使えない。装備を変更すると効果が消える。"},

	{30,30,60,FALSE,TRUE,A_CHR,0,0,"聖域作成Ⅱ",
	"足元に守りのルーンを設置する。" },

	{37,47,70,FALSE,TRUE,A_WIS,0,10,"呪われた柴榑雨",
		"縄張りの中全てに強力な水属性攻撃を行う。" },

	{40,30,70,FALSE,TRUE,A_INT,0,0,"窮僻の山姥",
		"縄張りの中で敵が使う召喚魔法を高確率で阻害する。縄張りから出るか縄張りが効力を失うと効果が切れる。" },

	{43,60,80,FALSE,FALSE,A_STR,0,0,"マウンテンマーダー",
		"周囲の広範囲のモンスターに対して無差別攻撃を行う。対象との距離によって攻撃回数が増減する。縄張り内にいるとき攻撃回数がさらに増える。この攻撃ではオーラダメージを受けない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_nemuno(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "食料を生成";
			msg_print("あなたは手際よく食物を集め始めた..");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}

	case 1:
		{
			if(only_info) return "";

			if (IS_NEMUNO_IN_SANCTUARY)
			{
				msg_print("ここはすでにあなたの縄張りだ。");
				return NULL;
			}
			if (!(cave[py][px].info & CAVE_ROOM))
			{
				msg_print("部屋でない場所を縄張りにすることはできない。");
				return NULL;
			}
			make_nemuno_sanctuary();

			break;
		}
	case 2:
		{
			int x, y;
			bool flag_ok = FALSE;
			int base = (plev + chr_adj) / 2;
			int sides = (plev + chr_adj) / 2;
			if (only_info) return format("損傷:%d+1d%d", base, sides);


			//縄張り内にしか効果がない
			msg_print("冷たい雨が降り注いだ...");
			floor_attack(GF_INACT, 1, sides, base, 1);
		}
		break;

	case 3:
		{
			int base = 4 + p_ptr->lev / 4;
			int sides = 4 + p_ptr->lev / 4;
			if (only_info) return format("期間:%d+1d%d", base, sides);

			if (!object_has_a_blade(&inventory[INVEN_RARM]) && !object_has_a_blade(&inventory[INVEN_LARM]))
			{
				msg_print("刃物を持っていないと使えない。");
				return NULL;
			}
			msg_print("あなたは一心不乱に武器を研ぎ始めた...");
			set_ele_attack(ATTACK_VORPAL, base + randint1(sides));

			break;

		}
	case 4: //聖域Ⅱ
	{
		if (only_info) return "";
		msg_print("あなたは奇怪な呪物を作り始めた..");
		warding_glyph();
		break;
	}

	case 5:
	{
		int base = plev * 4 + chr_adj * 5;
		if (only_info) return format("損傷:%d", base);

		//縄張り内にしか効果がない
		msg_print("豪雨がこの領域を外界から閉ざした。");
		floor_attack(GF_WATER, 0, 0, base, 1);
	}
	break;

	case 6:
	{
		if (only_info) return "";

		if (!IS_NEMUNO_IN_SANCTUARY)
		{
			msg_print("縄張りの中でないと使えない。");
			return NULL;
		}

		msg_print("辺りは人を寄せ付けない雰囲気に満ちた。");

		p_ptr->special_defense |= SD_UNIQUE_CLASS_POWER;
		p_ptr->redraw |= (PR_STATUS);


	}
	break;


	case 7:
	{
		int rad = 4;
		if (only_info) return format("");
		sakuya_time_stop(FALSE);
		flag_friendly_attack = TRUE;

		if(one_in_(3))
			msg_format("狂乱したように得物を振り回した！");
		else if (one_in_(2))
			msg_format("山姥のやり方でよそ者を迎えた！");
		else
			msg_format("手当たり次第に掻っ捌いた！");


		project(0,rad,py,px,100,GF_SOULSCULPTURE, PROJECT_KILL,-1);
		flag_friendly_attack = FALSE;
	}
	break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::弾丸作成用*/
static bool item_tester_hook_make_bullet(object_type *o_ptr)
{
	int i;

	for(i=0;soldier_bullet_making_table[i].shooting_mode;i++)
	{
		if(soldier_bullet_making_table[i].mat_tval == o_ptr->tval
			&& soldier_bullet_making_table[i].mat_sval == o_ptr->sval)
		{
			return TRUE;
		}
	}

	return FALSE;
}



/*:::兵士用特技*/
class_power_type class_power_soldier[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"技能の習得",
		"技能習得ポイントを使用して新しい技能を得る。技能習得ポイントは一定レベルに到達するごとに1ポイント得る。(最大15)"},
	{1,0,0,FALSE,FALSE,A_INT,0,0,"技能の確認",
		"習得済みの技能を確認する。行動力を消費しない。"},
	{5,5,0,FALSE,FALSE,A_DEX,0,0,"牽制射撃",
		"ダメージはないがターゲットを短時間減速させる射撃を行う。レベル20以降は混乱も追加する。"},
	{5,5,20,FALSE,FALSE,A_DEX,0,0,"トラップ・ドア破壊",
		"周囲のトラップやドアを破壊する。"},
	{5,16,30,FALSE,FALSE,A_INT,0,0,"スキャン・モンスター",
		"周囲のモンスターを調査する。"},
	{8,5,30,FALSE,FALSE,A_INT,0,0,"索敵",
		"周囲の敵の位置を感知する。"},
	{10,0,0,FALSE,FALSE,A_DEX,0,0,"ヘッドショット",
		"銃の射撃で頭などの弱点を狙う。もう一度使うと解除。射撃の成功率が25%低下するがクリティカルヒットが発生するようになる。相手が無生物の場合効果が薄い。この射撃モードの変更には行動力を消費しない。"},
	{10,10,30,FALSE,FALSE,A_INT,0,0,"周辺調査",
		"周囲のトラップを感知する。レベル20で地形、レベル25でアイテム、レベル30でモンスターを感知する。"},
	{10,15,50,FALSE,FALSE,A_DEX,0,0,"バリケード設置",
		"隣接した床一か所を岩石地形にする。"},
	{10,0,40,FALSE,FALSE,A_INT,0,0,"銃弾変更",
		"銃の属性や種類を変更して射撃を行う。変更する銃弾の種類によってMP消費や残弾消費が別途発生する。銃に「射撃」以外の属性がついていると銃の威力が元素1/2、それ以外2/3に低下する。ロケットランチャーや一部の特殊な銃はこの特技の対象にならない。"},
	{12,16,40,FALSE,FALSE,A_INT,0,0,"食料調達",
		"アイテム「食料」を生成する。"},
	{15,20,50,TRUE,FALSE,A_DEX,50,0,"ヒット＆アウェイ",
		"敵に攻撃し、その後一瞬で短距離テレポートをする。装備品が重いと難易度が上がる。攻撃後にテレポートに失敗することもある。"},
	{15,24,50,FALSE,FALSE,A_INT,0,0,"ポータル設置",
		"今いる床にポータルを設置する。'>'コマンドで二箇所のポータルの間を移動できる。この移動はテレポート妨害の影響を受けない。三つ以上のポータルを設置したら一番古いポータルが消える。もし移動先にモンスターがいたらテレポート耐性を無視して強制的に場所が入れ替わる。"},
	{20,1,0,FALSE,FALSE,A_INT,0,0,"精神集中",
		"精神を集中する。集中回数に応じて命中率とクリティカルヒット率が上昇する。射撃やボルト以外を発射する銃ではクリティカルヒットが発生しにくい。"},
	{20,20,50,FALSE,FALSE,A_DEX,0,0,"爆弾設置",
		"足元に爆弾を設置する。爆弾は10ターンで爆発し半径3の破片属性の強力なボールを発生させる。この爆発ではプレイヤーもダメージを受ける。爆弾は一部の属性攻撃に晒されると誘爆したり爆発タイミングが変わることがある。"},
	{24,30,70,FALSE,FALSE,A_DEX,0,0,"ラピッドファイア",
		"視界内のランダムなターゲットに連射する。連射可能数はレベルアップで増加する。射撃一発ごとの命中率は30%低下する。一部の特殊な銃器では使用できない。"},
	{25,25,60,FALSE,FALSE,A_INT,0,0,"自己治療",
		"HPと一部のステータス異常を回復する。"},
	{28,40,60,FALSE,FALSE,A_DEX,0,0,"メガクラッシュ",
		"周囲を*破壊*する。"},
	{35,20,60,FALSE,FALSE,A_DEX,0,0,"弾丸加工",
		"素材系アイテムを弾丸に加工する。加工した弾丸は「銃弾変更」から使用できる。"},
	{35,40,65,FALSE,FALSE,A_DEX,0,0,"全耐性",
		"一時的に元素耐性を得る。"	},
	{40,65,75,FALSE,FALSE,A_STR,0,0,"アドレナリン・コントロール",
		"一時的に加速、肉体強化、狂戦士化状態になる。"	},

	{45,0,50,FALSE,FALSE,A_INT,0,0,"起爆",
		"フロアに設置された爆弾を全て爆発させる。"	},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_soldier(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			if(!gain_soldier_skill()) return NULL;
		}
		break;
	case 1:
		{
			if(only_info) return "";
			check_soldier_skill();
			return NULL; //行動力を消費しない
		}
	case 3:
		{
			int range = 1 + plev / 35;
			if(only_info) return format("範囲:%d",range);
			msg_print("あなたは工具を取り出した...");
			project(0, range, py, px, 0, GF_KILL_DOOR, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);

		}
		break;
	case 2:
		{
			if(only_info) return "";

			if(num == 2)
				special_shooting_mode = SSM_CONFUSE;
			//else
			//	special_shooting_mode = SSM_HEAD_SHOT;

			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}

			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
				new_class_power_change_energy_need = 75;

		}
		break;
	case 4:
		{
			if(only_info) return "";
			probing();
		}
		break;

	case 5:
		{
			int range = DETECT_RAD_DEFAULT;
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_SCAN_MONSTER)) range += range / 2;

			if(only_info) return format("範囲:%d",range);
			msg_format("慎重に辺りを窺った..");
			detect_monsters_normal(DETECT_RAD_DEFAULT);
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 6:
		{
			if(only_info) return "";

			if(p_ptr->special_attack & ATTACK_HEAD_SHOT)
			{
				msg_print("弱点を狙うのを止めた。");
				p_ptr->special_attack &= ~(ATTACK_HEAD_SHOT);
			}
			else
			{
				msg_print("あなたは敵の弱点を狙うことにした。");
				p_ptr->special_attack |= ATTACK_HEAD_SHOT;
			}
			p_ptr->redraw |= PR_STATUS;
			return NULL; //行動力を消費しない

		}
		break;
	case 7:
		{
			int rad = 15 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("周辺の状況を調査した...");
			detect_traps(rad, TRUE);
			if(plev > 19) map_area(rad);
			if(plev > 24) detect_objects_normal(rad);
			if(plev > 29) detect_monsters_normal(rad);
			if(plev > 29) detect_monsters_invis(rad);
		}
		break;
	case 8:
		{
			int y,x,dir;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!cave_naked_bold(y, x))
			{
				msg_print("ここにはバリケードを作れない。");
				return NULL;
			}
			msg_print("バリケードを設置した！");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

			break;

		}
		//銃弾変更
	case 9:
		{
			int tmp;
			if(only_info) return "";

			//使用可能な中から銃弾を選ぶ
			tmp = soldier_change_bullet();
			if(!tmp) return NULL;

			//選んだ射撃モードをセットし銃射撃ルーチンへ
			special_shooting_mode = tmp;
			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}

			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
				new_class_power_change_energy_need = 75;


			break;
		}

	case 10:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "食料を生成";
			msg_print("食べられる野草などを集めた。");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}


	case 11:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
		}
		break;
	case 12:
		{
			int x,y,old_x=0,old_y=0;
			int num=0,max=0;
			if(only_info) return "";
			if(!cave_clean_bold(py,px))
			{
				msg_print("ここには設置できない。");
				return NULL;
			}
			//cave:specialを使って新旧管理
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if(cave_have_flag_bold(y,x,FF_PORTAL))
					{
						if(cheat_xtra) msg_format("1 y:%d x:%d num:%d old_x:%d old_y:%d max:%d",y,x,num,old_x,old_y,max);
						num++;
						if(!max || cave[y][x].special < max)
						{
							old_x = x;
							old_y = y;
						}
						if(cave[y][x].special > max)
						{
							max = cave[y][x].special;
						}
						if(cheat_xtra) msg_format("2 y:%d x:%d num:%d old_x:%d old_y:%d max:%d",y,x,num,old_x,old_y,max);

					}
				}
			}
			cave_set_feat(py, px, f_tag_to_index_in_init("PORTAL"));
			cave[py][px].special = max+1;
			msg_print("ポータルを設置した！");
			//元々2つあった場合、古いのを一つ消す
			if(num > 1)
			{
				cave_set_feat(old_y,old_x,feat_floor);
				note_spot(old_y,old_x);
				lite_spot(old_y,old_x);
				msg_print("古いポータルが消えた。");
			}
		}
		break;

	case 13:
		{
			if(only_info) return format("");
			do_cmd_concentrate(0);
			break;
		}

	case 14: //爆弾設置　布都の皿と同じ扱いにする
		{
			int dam = 100+plev*4;
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_POWERUP_BOMB)) dam += dam / 2;

			if(only_info) return format("損傷:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("ここには爆弾を置けない。");
				return NULL;
			}
			msg_print("爆弾を仕掛けた。");
			/*:::Mega Hack - cave_type.specialを爆弾のカウントに使用する。*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;

			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	//ラピッドファイア
	//v1.1.44 別関数に分けた
	case 15:
		{
			int shoot_num = (plev + 4) / 7;
			if (only_info) return format("連射数:最大%d", shoot_num);

			if (rapid_fire(shoot_num,0))
			{
				if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT, SS_S_QUICK_DRAW))
					new_class_power_change_energy_need = 75;
			}
			else
			{
				return NULL;

			}

		}
			/*
			{
				int dir=5;
				int timeout_base, timeout_max;
				int shoot_num = (plev+4) / 7;
				object_type *o_ptr;
				bool flag_ammo = FALSE;
				bool flag_mon = FALSE;
				if(only_info) return format("連射数:最大%d",shoot_num);

				special_shooting_mode = SSM_RAPID_FIRE;
				for(;shoot_num;shoot_num--)
				{
					bool righthand = FALSE;
					bool lefthand = FALSE;

					if(inventory[INVEN_RARM].tval == TV_GUN)
					{
						o_ptr = &inventory[INVEN_RARM];
						timeout_base = calc_gun_timeout(o_ptr) * 1000;
						timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
						if(o_ptr->timeout <= timeout_max) righthand = TRUE;
					}
					if(!get_random_target(0,0)) break;
					flag_mon = TRUE;
					if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);

					if(inventory[INVEN_LARM].tval == TV_GUN)
					{
						o_ptr = &inventory[INVEN_LARM];
						timeout_base = calc_gun_timeout(o_ptr) * 1000;
						timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
						if(o_ptr->timeout <= timeout_max) lefthand = TRUE;
					}
					if(!righthand && !lefthand) break;
					else flag_ammo = TRUE;

					if(!get_random_target(0,0)) break;
					if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);
				}

				if(!flag_mon) msg_print("..しかし周囲に標的がいない。");
				else if(!flag_ammo) msg_print("..しかし元々弾がなかった。");

				if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
					new_class_power_change_energy_need = 75;
			}
			*/
		break;
	case 16:
		{
			int base = 100;
			if(only_info) return format("回復:%d",base);
			msg_print("あなたは携帯医療キットを取り出した..");
			set_cut(0);
			set_stun(0);
			set_poisoned(0);
			hp_player(base);
			break;
		}

	case 17:
		{
			int sides = 5;
			int base = 12;
			if(only_info) return format("範囲:%d+1d%d",base,sides);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			break;
		}
	case 18:
		{
			int         item,amt,i,bullet_index=0;
			object_type *o_ptr;
			cptr        q, s;
			char            o_name[MAX_NLEN];
			int makenum;

			if(only_info) return format("");

			item_tester_hook = item_tester_hook_make_bullet;
			q = "どれを弾丸に加工しますか? ";
			s = "弾丸に加工できそうなものが見当たらない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else		o_ptr = &o_list[0 - item];

			for(i=0;soldier_bullet_making_table[i].shooting_mode;i++)
			{
				if(soldier_bullet_making_table[i].mat_tval == o_ptr->tval
					&& soldier_bullet_making_table[i].mat_sval == o_ptr->sval)
				{
					bullet_index = soldier_bullet_making_table[i].shooting_mode;
					break;
				}
			}

			if(bullet_index<=0 || bullet_index>100){msg_print("ERROR:弾丸加工ルーチンがなんか変");return NULL;}

			if(p_ptr->magic_num1[bullet_index] >= 99)
			{
				msg_print("もうその弾丸は十分に持っている。");
				return NULL;
			}
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

			if (!get_check_strict(format("「%s」を加工します。よろしいですか？",o_name), CHECK_DEFAULT_Y))
				return NULL;


			msg_format("あなたは素材の加工に取り掛かった..");

			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
			makenum = soldier_bullet_making_table[i].prod_num;
			if(weird_luck()) makenum += 2;
			else if(one_in_(3)) makenum+=1;

			msg_format("「%s」が%d発完成した！",soldier_bullet_table[bullet_index].name,makenum);
			p_ptr->magic_num1[bullet_index] += makenum;
			if(p_ptr->magic_num1[bullet_index]>99) p_ptr->magic_num1[bullet_index]=99;
		}
		break;
	case 19:
		{
			int base = 15;
			int time;
			if(only_info) return format("期間:%d+1d%d",base,base);

			time = base + randint1(base);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
		}
		break;
	case 20:
		{
			int base = 7;
			int time;
			int percentage = p_ptr->chp * 100 / p_ptr->mhp;
			if(only_info) return format("期間:%d+1d%d",base,base);

			time = randint1(base) + base;
			msg_format("様々な興奮物質が頭の中を駆け巡った！");
			set_tim_addstat(A_STR,104,time,FALSE);
			set_tim_addstat(A_DEX,104,time,FALSE);
			set_tim_addstat(A_CON,104,time,FALSE);
			set_fast(time,FALSE);
			set_shero(time,FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP );
			redraw_stuff();

		}
		break;
	case 21:
		{
			cave_type       *c_ptr;
			bool flag = FALSE;
			int x,y;
			if(only_info) return "";


			msg_print("起爆信号を発信した！");
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					c_ptr = &cave[y][x];
					if(!(c_ptr->info & CAVE_OBJECT) || !have_flag(f_info[c_ptr->mimic].flags, FF_BOMB))
						continue;

					bomb_count(y,x,-999);
					flag = TRUE;
				}
			}
			if(!flag) msg_print("...しかし反応がなかった。");

		}
		break;




	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}

	special_shooting_mode = 0L;

	return "";
}



/*:::純狐特技*/
class_power_type class_power_junko[] =
{
	{ 1,20,0,FALSE,FALSE,A_INT,0,0,"名も無き技生成",
		"自らが保持する純粋な力を使って即興の特技を作る。フロアを移動するたびにこの技と同じ効果が発動して技が入れ替わる。"},

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"名も無き技使用",
		"作り上げた「名もなき技」を使用する。技ごとに追加のMPを消費する。"},

	{ 1, 0,0,FALSE,FALSE,A_INT,0,0,"名も無き技調査",
		"作り上げた「名もなき技」の効果を確認する。" },

	{ 7,11,25,FALSE,FALSE,A_WIS,0,5,"掌の純光",
		"射程の短い「純化」属性のビームを放つ。無属性だが当たったモンスターが何らかの弱点を持っていればそれに作用してダメージが1.5倍になる。"},

	{12,20,30,FALSE,FALSE,A_INT,0,0,"原始の神霊界",
		"隣接したモンスター一体をランクアップさせる。"},

	{18, 0,45,FALSE,FALSE,A_DEX,0,0,"純化Ⅰ",
		"アイテムを消滅させ、その価値の1/100のMPに変換する。消費したアイテムは名もなき技の生成に影響を与えることがある。"},

	{25,30,40,FALSE,FALSE,A_CHR,0,0,"援軍召喚",
		"このフロア限定で地獄の妖精を多数呼び出す。"},

	{30,100,60,FALSE,FALSE,A_DEX,0,0,"純化Ⅱ",
		"装備品一種類に対し、そのベースアイテムにない全ての能力を抹消してACか攻撃修正かパラメータ上昇値に変換する。"},

	{34,34,55,FALSE,FALSE,A_CHR,0,10,"純粋なる狂気",
		"視界内の全てに強力な精神攻撃を行い、さらに魅了して配下にしようと試みる。狂戦士化していないと使えない。"},

	{41,80,75,FALSE,FALSE,A_INT,0,0,"現代の神霊界",
		"視界内のモンスター全てを最終段階までランクアップさせる。"},

	{44,88,80,FALSE,FALSE,A_CHR,0,20,"殺意の百合",
		"極めて強力な純化属性ビームを放つ。狂戦士化していないと使えない。"},

	{47,160,85,FALSE,FALSE,A_WIS,0,0,"地上穢の純化",
		"視界内のモンスターのHPを全回復させる。ただしHPが半分以下のモンスターは一撃で倒す。クエストボスには効果がない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_junko(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!junko_make_nameless_arts()) return NULL;
			msg_print("あなたは名もなき純粋な力を様々に操った..");

		}
		break;
	case 1:
		{
			if(only_info) return format("");
			if(!activate_nameless_art(FALSE)) return NULL;
		}
		break;
	case 2:
		{
			if (only_info) return format("");
			if (!activate_nameless_art(TRUE)) return NULL;
		}
		break;

	case 3:
		{
			int dice = 2 + plev / 3;
			int sides = 9;
			int base = plev + chr_adj * 2;
			int range = 2 + plev / 6;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたの掌から眩い光が放たれた。");
			fire_beam(GF_PURIFY, dir, base + damroll(dice,sides));
			break;
		}
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];

				if(!rankup_monster(cave[y][x].m_idx,1))
					msg_format("何も起こらなかった。");
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
	case 5:
		{
			if(only_info) return format(" ");
			if(!convert_item_to_mana()) return NULL;
		}
		break;
	case 6:
		{
			int i;
			bool flag = FALSE;
			int max = 1 + p_ptr->lev / 7;
			if(only_info) return format("");

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_HELL_FAIRY, (PM_EPHEMERA))) flag = TRUE;
			if(flag) msg_print("友人の配下が現れた。");
			else msg_print("何も現れなかった..");

			break;
		}

	case 7:
		{
			if(only_info) return format(" ");
			if(!purify_equipment()) return NULL;
		}
		break;

	case 8:
		{
			int base = plev * 3 + chr_adj * 5;

			if(only_info) return format("損傷:%d+1d%d",base,base);
			msg_format("純粋な狂気を解き放った。");
			project_hack2(GF_COSMIC_HORROR,1,base,base);
			charm_monsters(base);

			break;
		}
		break;

	case 9:
		{
			int i;
			bool flag = FALSE;
			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;

				//v1.1.38 視界内でなかったので修正
				if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;

				if(rankup_monster(i,0))	flag = TRUE;
			}

			if(!flag)
				msg_print("何も起こらなかった..");
		}
		break;

	case 10:
		{
			int dam = plev * 6 + chr_adj * 10;

			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("純光がダンジョンを埋め尽くした！");
			fire_spark(GF_PURIFY, dir, dam, 2);
			break;
		}

	case 11:
		{

			int i;
			bool flag = FALSE;
			if(only_info) return format("");
			msg_print("あなたは周囲のモンスターたちを純化しようと試みた...");

			for (i = 1; i < m_max; i++)
			{
				char m_name[80];

				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(!m_ptr->r_idx) continue;
				if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
				if(r_ptr->flags1 & RF1_QUESTOR) continue;

				if(m_ptr->hp > m_ptr->maxhp / 2)
				{
					monster_desc(m_name, m_ptr, 0);
					msg_format("%sは完全に治った！",m_name);
					m_ptr->hp = m_ptr->maxhp;
					if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
					if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
					redraw_stuff();
				}
				else
				{
					bool dummy;
					mon_take_hit(i,32767,&dummy,"は倒れた。");
				}
				flag = TRUE;

			}

			if(!flag)
				msg_print("何も起こらなかった。");

			break;
		}



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





//v1.1.15
/*:::ヘカーティア特技*/
//使う変数などはcmd7.cのselect_hecatia_body()のとこにメモした
//閃光免疫にtim_gen1を使う
class_power_type class_power_hecatia[] =
{

	{9,10,15,FALSE,FALSE,A_INT,0,0,"モンスター調査",
		"周囲のモンスターの情報を得る。"},
	{15,16,25,FALSE,TRUE,A_CHR,0,10,"配下召喚",
		"地獄の住人を配下として呼び出す。"},
	//残りの体のTシャツ生成がレベル20で行われるのでレベル変更時注意
	{21,30,0,FALSE,FALSE,A_DEX,0,0,"体を交代する",
		"別の体と交代する。交代するときにステータス異常や一時効果はすべて消える。それぞれの体は性格・装備品・HP/MP・使用可能魔法領域が別々になっている。両方の体がMPを消費する。"},
	{24,60,60,FALSE,TRUE,A_INT,0,0,"逢魔ガ刻",
		"その場で探索拠点にアクセスする。ただし夜の十字路か三叉路でないと使えない。"},
	//アイテムカード「女神の首輪」
	{27,45,45,FALSE,TRUE,A_WIS,0,0,"邪穢在身",
		"周囲のアンデッドを極めて高確率で魅了し配下にする。"},
	{30,43,55,FALSE,TRUE,A_DEX,0,0,"アポロ反射鏡",
		"一定時間、反射、魔法防御上昇、閃光免疫を得る。"},
	{36,60,70,FALSE,TRUE,A_INT,0,0,"地獄のノンイデアル弾幕",
		"視界内の全てに対し、地獄の業火属性の攻撃を行う。"},
	{39,66,70,FALSE,TRUE,A_DEX,0,0,"地獄に降る雨",
		"視界内のランダムな敵に対して水属性のビームを数回放つ。"},
	//アイテムカード「月」
	{42,90,75,FALSE,TRUE,A_STR,0,0,"ルナティックインパクト",
		"ターゲットに対し極めて強力な隕石攻撃を行う。"},
	{48,-100,80,FALSE,TRUE,A_CHR,0,0,"トリニタリアンラプソディ",
		"全ての体が全MP(最低100)を使用し、ターゲットに対し様々な攻撃を仕掛ける。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_hecatia(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			msg_print("あなたは周囲を無遠慮に睨め回した。");
			probing();
		}
		break;
	case 1:
		{
			int level = plev * 2;
			int num = 2 + plev / 12;
			bool flag = FALSE;
			if(only_info) return format("召喚レベル:%d",level);
			ignore_summon_align = TRUE;
			if(one_in_(3))
			{
				if(summon_named_creature(-1,py,px,MON_CLOWNPIECE,(PM_FORCE_PET))) flag = TRUE;
			}
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, level, SUMMON_DEATH, (PM_ALLOW_UNIQUE | PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;

			}
			if(flag) msg_format("地獄の住人を呼び出した。");
			else msg_format("何も現れなかった・・");
			ignore_summon_align = FALSE;

		}
		break;
	case 2://体交代
		{
			if(only_info) return format(" ");
			if(!hecatia_change_body()) return NULL;
		}
		break;
	case 3:
		{
			int dir;
			bool flag_ok = TRUE;
			int count_roads=0;
			if(only_info) return format(" ");

			if(is_daytime())
			{
				msg_print("昼間には使えない。");
				return NULL;
			}
			//十字路にいるかどうか判定
			for(dir = 1 ; dir <= 9 ; dir++)
			{
				int x,y;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!in_bounds(y,x))
				{
					flag_ok = FALSE;
				}
				else if(y==py && x==px)
				{
					if(!cave_have_flag_bold(y,x,FF_FLOOR)) flag_ok = FALSE;
				}
				else if(!ddx[dir] || !ddy[dir])
				{
					if(cave_have_flag_bold(y,x,FF_FLOOR)) count_roads++;
				}
				else
				{
					if(!cave_have_flag_bold(y,x,FF_WALL)) flag_ok = FALSE;
				}
			}
			if(count_roads < 3) flag_ok = FALSE;

			if(!flag_ok)
			{
				msg_print("十字路か三叉路でないと使えない。");
				return NULL;
			}

			msg_print("あなたは自分の居所から力を行使した..");
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
		}
		break;
	case 4:
		{
			int power = plev * 16;
			if(power < 200) power = 200;
			if(only_info) return format("効力:%d",power);

			msg_print("あなたは威圧的な雰囲気を発した。");
			charm_undead(power);

		}
		break;

	case 5:
		{
			int base = 9 + plev / 3;
			int time;

			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			set_tim_general(time,FALSE,0,0);
			set_tim_reflect(time,FALSE);
			set_resist_magic(time,FALSE);

		}
		break;

	case 6:
		{
			int dice = plev/2;
			int sides = 15 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("地獄の業火が周囲を焼き払った！");
			project_hack2(GF_HELL_FIRE,dice,sides,0);
			break;
		}

	case 7:
		{
			bool flag = FALSE;
			int i;
			int dice = 6 + plev / 12;
			int sides = 3 + chr_adj / 10;
			int num = 8 + plev / 6;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			sakuya_time_stop(FALSE);
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_WATER, damroll(dice,sides),2, 0,0)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("周囲に敵が見当たらなかった。");
					break;
				}
			}

		}
		break;

	case 8:
		{
			int base = plev * 10;
			int sides = chr_adj * 20;
			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_jump(GF_METEOR,dir,base+randint1(sides),6,"巨大隕石が大地を穿った！");


		}
		break;
	case 9:
		{
			int i;
			int sp[3];
			int x, y;
			monster_type *m_ptr;
			char m_name[80];

			if(only_info) return format("損傷:不定");
			//すべての体の今のMPを得る
			for(i=0;i<3;i++)
				sp[i] = p_ptr->magic_num1[9+i] * p_ptr->magic_num1[3+i] / 10000;
			sp[p_ptr->magic_num2[0]] = p_ptr->csp;

			if(sp[0] < 100 || sp[1] < 100 || sp[2] < 100)
			{
				msg_print("他の体のMPが足りない。");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}

			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}

			monster_desc(m_name, m_ptr, 0);
			msg_format("あなたは三体全てを使って%sを地獄に落とすことにした！",m_name);

			for(i=0;;i++)
			{
				int typ,dam;
				if(!m_ptr->r_idx) break;
				if(sp[i%3] < (i/3+1) * 100) break;//攻撃回数*100のMPを下回る体が出たら終了。最大18発出るはず

				switch(randint1(4))
				{
				case 1://巨大ボール発生
					{
						int rad = 5 + randint1(3);

						dam = 200+chr_adj*5+randint1(100);

						if(one_in_(2)) typ = GF_METEOR;
						else if(one_in_(2)) typ = GF_DISINTEGRATE;
						else if(one_in_(4)) typ = GF_DARK;
						else if(one_in_(3)) typ = GF_WATER;
						else if(one_in_(2)) typ = GF_HELL_FIRE;
						else typ = GF_TIME;

						if(typ == GF_WATER)
							project(0,4+randint1(3),y,x,10,GF_WATER_FLOW,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);

						project(0,rad,y,x,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);
					}
					break;
				case 2: //巨大レーザー
					{
						dam = 200+chr_adj*5+randint1(100);
						if(one_in_(7))		typ = GF_PSY_SPEAR;
						else if(one_in_(6))	typ = GF_LITE;
						else if(one_in_(5)) typ = GF_DISINTEGRATE;
						else if(one_in_(4)) typ = GF_DISENCHANT;
						else if(one_in_(3)) typ = GF_MANA;
						else if(one_in_(2)) typ = GF_NETHER;
						else  typ = GF_CHAOS;

						fire_spark(typ,dir,dam,randint1(2));

					}
					break;
				case 3: //小型ボール周囲連続発生
					{
						int num = 5 + randint1(3);
						int j;

						if(one_in_(2)) typ = GF_METEOR;
						else if(one_in_(2)) typ = GF_FIRE;
						else if(one_in_(3)) typ = GF_SHARDS;
						else if(one_in_(2)) typ = GF_CHAOS;
						else typ = GF_DARK;

						for(j=num;j>0;j--)
						{
							int rad = 1+randint1(2);
							int tx,ty;

							dam = 100 + chr_adj*2+ randint1(50);

							tx = rand_spread(x,4);
							ty = rand_spread(y,4);
							if(!in_bounds(ty,tx) || !cave_have_flag_bold(ty,tx,FF_PROJECT)) continue;
							if(!projectable(py,px,ty,tx)) continue;

							if(typ == GF_FIRE)
								project(0,randint1(3),ty,tx,5,GF_LAVA_FLOW,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);

							project(0,rad,ty,tx,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);

						}
					}
					break;
				case 4: //連続ロケット
					{
						int num = 2 + randint1(3);

						int j;

						if(one_in_(6)) typ = GF_METEOR;
						else if(one_in_(5)) typ = GF_DARK;
						else if(one_in_(4)) typ = GF_NETHER;
						else if(one_in_(3)) typ = GF_HELL_FIRE;
						else if(one_in_(2)) typ = GF_NUKE;
						else typ = GF_SOUND;

						for(j=num;j>0;j--)
						{
							dam = 50 + chr_adj + randint1(50);
							fire_rocket(typ,dir,dam,2);
						}
					}
					break;
				}
			}

			//全ての体のMPを0にする
			for(i=0;i<3;i++)p_ptr->magic_num1[3+i] = 0;

		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::依姫専用技*/
/*:::magic_num1[0-19]を八百万の神々降神時の魔法リストとして使用する*/
/*:::magic_num1[20]を八百万の神々降神時のr_idxとして使用する*/
//祇園様の剣使用フラグにconcentを使う
class_power_type class_power_yorihime[] =
{
	{7,0,25,FALSE,TRUE,A_CHR,0,0,"八百万の神々",
		"ランダムな神格系モンスターをその身に降ろし、その神格が持つ特技を行使する。特技の種類に応じた追加のMPが必要になる。キャンセルしたときにも行動力を消費する。"},
	{12,24,30,FALSE,TRUE,A_STR,0,3,"祇園様の剣",
		"発動後近くで移動か隣接攻撃をしようとしたモンスターにカウンター攻撃を行い、低確率で行動終了させる。剣か刀を装備していないと使えない。★天羽々斬を装備していると効果範囲が広がる。プレイヤーが待機と神降ろし以外の行動をすると解除される。"},

//アイテムカード：パラジウム合金の柱
	{18,30,35,FALSE,FALSE,A_INT,0,0,"金山彦命",
		"一定時間、モンスターによる武器攻撃と特技「射撃」を高確率で無効化する。"},
	{23,20,45,FALSE,TRUE,A_CON,0,6,"火雷神",
		"自分のHPの1/3の威力のプラズマ属性のブレスを放つ。"},
	{27,45,50,FALSE,TRUE,A_WIS,0,0,"愛宕様の炎",
		"一定時間、炎に対する完全な免疫と強力な火炎のオーラを得て火炎属性攻撃ができるようになる。"},
	{31,30,60,FALSE,TRUE,A_DEX,0,10,"石凝姥命",
		"一定時間、閃光攻撃に対する完全な免疫と反射能力を得る。"},
//アイテムカード:蕎麦焼酎
	{35,40,65,FALSE,FALSE,A_WIS,0,0,"天津甕星",
		"視界内の全てのモンスターを「死の光線」属性で攻撃する。"},
	{38,55,70,FALSE,TRUE,A_WIS,0,0,"伊豆能売",
		"視界内全てに対して強力な破邪属性攻撃を行い、さらに呪われた装備を解呪する。"},
	{42,80,80,FALSE,TRUE,A_CHR,0,0,"天宇受売",
		"一定時間、暗黒・地獄・地獄の劫火属性の攻撃への耐性、AC上昇、魔法防御上昇、魅力大幅上昇を得る。"},
	{45,96,90,FALSE,TRUE,A_WIS,0,0,"天照大御神",
		"周囲の敵に大打撃を与える。近くにいるほど効果が大きい。天宇受売命の降神中に行うと効果が強化される。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


/*:::依姫の「八百万の神々降神」で出てくる神格*/
static bool monster_hook_yorihime(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//自分と姉は呼ばない
	if(r_idx == MON_YORIHIME) return FALSE;
	if(r_idx == MON_TOYOHIME) return FALSE;
	//純狐とヘカーティアも止めとく
	if(r_idx == MON_JUNKO) return FALSE;
	if(r_idx == MON_HECATIA1) return FALSE;
	if(r_idx == MON_HECATIA2) return FALSE;
	if(r_idx == MON_HECATIA3) return FALSE;

	//神格フラグを持たないものは対象外
	if(!(r_ptr->flags3 & RF3_DEITY)) return FALSE;


	//特技を持たない神格はFALSE
	if(!(r_ptr->flags4 | r_ptr->flags5 | r_ptr->flags6 | r_ptr->flags9)) return FALSE;

	return TRUE;

}

cptr do_cmd_class_power_aux_yorihime(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://八百万の神々降神
		{
			int r_idx = 0;
			if(only_info) return format("");

			//神格のr_idxを選定
			get_mon_num_prep(monster_hook_yorihime,NULL);//打倒済み神格がちゃんと出るかチェック
			r_idx = get_mon_num(MAX(dun_level,p_ptr->lev));
			get_mon_num_prep(NULL, NULL);

			if(!r_idx)
			{
				msg_print("何も降りてこなかった...");
				break;
			}
			else
			{
				u32b rflags;
				int j,new_num;
				int mlist_num = 0;
				int memory = 20; //魔法記憶数　常に20
				int shift;
				//初期化
				for(j=0;j<20;j++)p_ptr->magic_num1[j]=0;

				//選定された神格r_idxのもつ特技を一覧にしてmagic_num1[0-19]に登録
				for(j=0;j<4;j++)
				{
					int spell;
					monster_race *r_ptr = &r_info[r_idx];

					if(j==0) rflags = r_ptr->flags4;
					else if(j==1) rflags = r_ptr->flags5;
					else if(j==2) rflags = r_ptr->flags6;
					else rflags = r_ptr->flags9;

					for(shift=0;shift<32;shift++)
					{
						int k;
						if((rflags >> shift) % 2 == 0) continue;
						spell = shift+1 + j * 32;

						if(spell < 1 || spell > 128)
						{
							msg_format("ERROR:依姫神格特技設定ルーチンで不正なspell値(%d)が出た",spell);
							return NULL;
						}
						if(!monspell_list2[spell].level) continue; //特別な行動や未実装の技は非対象

						if(mlist_num < memory) new_num = mlist_num;
						else
						{
							new_num = randint0(mlist_num+1);
							if(new_num >=memory) continue;
						}
						p_ptr->magic_num1[new_num] = spell;
						mlist_num++;
					}
				}
			}
			p_ptr->magic_num1[20] = r_idx;
			set_kamioroshi(KAMIOROSHI_YAOYOROZU, 0);

			 //キャンセルしても行動順を消費する
			if(!cast_monspell_new())
			{
				cptr mname;

				//v1.1.30 ランダムユニークの名前処理追加
				if(IS_RANDOM_UNIQUE_IDX(r_idx))
					mname = random_unique_name[r_idx - MON_RANDOM_UNIQUE_1];
				else
					mname = r_name + r_info[r_idx].name;

				//「中神霊の神霊は～」とかメッセージ出たら変なのでstreqで分岐しようとしたがうまくいかない。全角に対応していないのか。
				//if(streq(r_name + r_info[r_idx].name, "神霊"))
				if(r_idx == MON_C_SHINREI || r_idx == MON_D_SHINREI)
					msg_format("%sは何もせずに帰っていった。",mname);
				else
					msg_format("%sの神霊は何もせずに帰っていった。",mname);
			}

		}
		break;



	case 1: //祇園様の剣
		{
			int time = 25 + plev / 2;
			int range = plev/12;
			if(inventory[INVEN_RARM].name1 == ART_AMENOHABAKIRI ||inventory[INVEN_LARM].name1 == ART_AMENOHABAKIRI)
				range += 4;

			if(only_info) return format("期間:%d 範囲:%d",time,range);
			do_cmd_concentrate(0);
			set_kamioroshi(KAMIOROSHI_GION, time);

		}
		break;
	//SLASH,SPEARの攻撃全てとSTINGの攻撃のうちHUMAN,DEMIHUMAN,is_gen_unique()のモンスターによる攻撃を無効化
	//ARROW系のボルトを無効化
	case 2: //金山彦
		{
			int time = 10 + plev / 5;

			if(only_info) return format("期間:%d+1d%d",time,time);

			if(use_itemcard)
				msg_print("何やら金属質な神気があなたを守るのを感じる..");
			else
				msg_print("金山彦命があなたの身に降りてきた...");
			set_kamioroshi(KAMIOROSHI_KANAYAMAHIKO, time+randint1(time));

		}
		break;

	case 3: //火雷神
		{
			int dam = p_ptr->chp / 3;

			if(dam < 1) dam = 1;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			set_kamioroshi(KAMIOROSHI_HONOIKAZUTI, 0);

			msg_print("火雷神が猛火と落雷を巻き起こした！");
			fire_ball(GF_PLASMA, dir, dam, -2);
		}
		break;
	case 4: //愛宕様の炎　破邪魔法のと同じ
		{
			int base = 5 + plev / 5;

			if(only_info) return format("期間:%d+1d%d",base,base);

			msg_print("愛宕様の神火が身に宿った！");
			set_kamioroshi(KAMIOROSHI_ATAGO, randint1(base) + base);

		}
		break;
	case 5: //石凝姥命　破邪魔法のと同じ
		{
			int base = 15 + plev / 5;

			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_print("石凝姥命が現れ、神鏡を掲げた！");
			set_kamioroshi(KAMIOROSHI_ISHIKORIDOME, randint1(base) + base);
		}
		break;
	case 6: //天津甕星
		{
			int dam = plev * 200;
			if(dam > 9999) dam = 9999;
			if(only_info) return format("損傷:%d",dam);

			set_kamioroshi(KAMIOROSHI_AMATSUMIKABOSHI, 0);
			msg_print("天津甕星が明星の輝きを放った！");

			//使われてない関数があったのでいつか使いたかった:)
			deathray_monsters();

		}
		break;
	case 7: //伊豆能売　破邪魔法のと同じ
		{
			int dice = 1;
			int sides = plev * 3;
			int base = plev * 3;
			if (only_info) return format("損傷:%d+1d%d",dice, sides, base);

			set_kamioroshi(KAMIOROSHI_IZUNOME, 0);
			msg_print("鈴の音と共にダンジョンに清浄な空気が満ちた・・");
			project_hack2(GF_HOLY_FIRE, dice,sides,base);
			if (remove_all_curse())
			{
				msg_print("装備品にかかった呪縛が解けた。");
			}
		}
		break;
	case 8: //天宇受売命　破邪魔法のと同じ
		{
			int base = 10 + plev / 5;

			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_print("あなたは軽やかに舞い始めた。");
			set_kamioroshi(KAMIOROSHI_AMENOUZUME, randint1(base) + base);
		}
		break;

	case 9: //天照大御神　破邪魔法のと同じ
		{

			if(only_info) return format("損傷:不定");

			call_amaterasu();
		}
		break;



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::豊姫特技*/
class_power_type class_power_toyohime[] =
{

	{5,8,25,FALSE,FALSE,A_INT,0,0,"テレポート",
		"中距離のランダムテレポートを行う。"},
	{9,8,25,FALSE,FALSE,A_INT,0,0,"穢身感知",
		"周囲の広範囲の生物を感知する。"},
	{15,25,40,FALSE,FALSE,A_CHR,0,5,"月の軍勢召喚",
		"月面勢力のモンスターを配下として多数召喚する。"},
	{20,18,50,FALSE,FALSE,A_DEX,0,0,"テレポート・レベル",
		"別のフロアへテレポートする。"},
	{25,25,60,FALSE,TRUE,A_INT,0,0,"次元の扉",
		"指定したグリッドへテレポートする。距離が遠すぎると失敗することがある。"},
	{30,30,55,FALSE,TRUE,A_WIS,0,0,"*追放*",
		"指定した敵一体を高確率で現在のフロアから追い払う。レベル40以降は周囲の全ての敵が対象になる。クエストダンジョンでは使えず、ユニークモンスターには効果がない。"},
//アイテムカード：愚者の封書
	{35,20,65,FALSE,TRUE,A_INT,0,0,"次元の扉Ⅱ",
		"視界内の指定したモンスターを指定したグリッドへテレポートさせる。距離が遠すぎると失敗することがある。またテレポート耐性のある敵対的なモンスターには効果がない。"},
	{40,72,80,FALSE,TRUE,A_INT,0,0,"次元の扉Ⅲ",
		"フロア内の指定した位置の周辺に全ての配下モンスターをテレポートさせる。"},
//アイテムカード：賢者の封書
	{45,240,90,FALSE,TRUE,A_CHR,0,0,"月の公転周期の罠",
		"発動すると、以後このフロアでテレポートなどを使ったモンスターがフロアから消え、さらにモンスターが使う召喚魔法が阻害されるようになる。クエスト討伐対象モンスターには効果がなくクエストダンジョンでは使えない。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_toyohime(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://テレポート
		{
			int dist = 25 + plev / 2;
			if(only_info) return format("距離:%d",dist);
			msg_print("あなたは瞬時に消えた。");
			teleport_player(dist, 0L);
		}
		break;

	case 1: //穢身感知
		{
			int rad = 25 + plev * 3 / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("あなたは穢れの兆しを感じ取った・・");
			detect_monsters_living(rad);
		}
		break;
	case 2: //軍団召喚
		{
			bool flag = FALSE;
			int i;
			int count = 2 + plev / 5;
			int mode;


			if(only_info) return "";
			for(i=0;i<count;i++)
			{
				int r_idx;
				if(randint1(p_ptr->lev) > 29)
					r_idx = MON_MOON_SENTINEL;
				else if(randint1(p_ptr->lev) > 19)
					r_idx = MON_GYOKUTO;
				else
					r_idx = MON_EAGLE_RABBIT;

				if(summon_named_creature(0,py,px,r_idx,PM_FORCE_PET)) flag = TRUE;

			}
			if(flag)
				msg_print("部下達を召集した。");
			else
				msg_print("誰も現れなかった...");

		}
		break;

	case 3:
		{
			if(only_info) return format("");

			if (!get_check("本当に他の階にテレポートしますか？")) return NULL;
			teleport_level(0);
		}
		break;
	case 4:
		{
			int range = plev;
			if(only_info) return format("距離:%d",range);
			if (!dimension_door(D_DOOR_TOYOHIME)) return NULL;
		}
		break;

	case 5: //追放
		{
			int power = 100 + plev * 3;

			if(only_info) return format("効力:%d",power);

			if(plev < 40)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
			else
			{

				if(one_in_(7))
					msg_print("「穢土に生まれ、悪心に操られし穢身、お前の浄土はここではない！」");

				mass_genocide(power, TRUE);
			}


		}
		break;
	case 6: //次元の扉Ⅱ
		{
			int x, y;
			monster_type *m_ptr;
			int range = plev * 2;

			if(only_info) return format("距離:%d",range);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				int tx,ty;
				int dist;
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if (!tgt_pt(&tx, &ty)) return NULL;

				dist = distance(px,py,tx,ty);

				if(is_hostile(m_ptr) && r_ptr->flagsr & RFR_RES_TELE)
				{
					msg_format("%sには耐性がある！",m_name);
				}
				else if( randint1(dist) > range)
				{
					msg_print("空間操作に失敗した！");
					teleport_away(cave[y][x].m_idx, randint1(250), TELEPORT_PASSIVE);
				}
				else
				{

					msg_format("%sを離れた場所へ送り込んだ。",m_name);
					teleport_monster_to(cave[y][x].m_idx, ty, tx, range,TELEPORT_PASSIVE);
				}
			}
		}
		break;


	case 7:
		{
			int tx,ty,i;
			bool flag = FALSE;

			if(only_info) return format("距離:無制限");

			if (!tgt_pt(&tx, &ty)) return NULL;

			msg_print("あなたは彼方の景色を目の前のものとし、配下たちを送り込んだ！");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				teleport_monster_to(i, ty, tx, 100,0L);
				flag = TRUE;

			}

			if(!flag)
				msg_print("...しかし連れていく配下がいなかった。");

		}
		break;
	case 8: //公転周期の罠
		{
			if(only_info) return format("");

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || EXTRA_QUEST_FLOOR)
			{
				msg_print("ここでは使えない。");
				return NULL;
			}

			if(use_itemcard)
				msg_print("フロアの何かが僅かに欠けた気がする...");
			else
				msg_print("あなたは師匠が月に仕掛けたトラップを起動した。");

			p_ptr->special_flags |= SPF_ORBITAL_TRAP;
			p_ptr->redraw |= (PR_STATUS);
			redraw_stuff();

		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}







/*:::レイセン2専用技*/
class_power_type class_power_reisen2[] =
{

	{ 5,10,25,FALSE,FALSE,A_CHR,0,0,"餅つき",
		"アイテム「団子」を作る。ただし杵を装備していないと実行できない。"},
	{ 15,5,40,FALSE,FALSE,A_DEX,0,5,"アイテム奪取",
		"隣接したモンスターが拾って持っているアイテムをランダムに一つ奪う。"},
	{ 24,12,55,FALSE,TRUE,A_DEX,0,0,"加速",
		"短時間加速する。"},

	{ 30,20,60,FALSE,FALSE,A_DEX,0,3,"ヒット＆アウェイ",
		"隣接した敵に攻撃し、そのまま一瞬で離脱する。離脱に失敗することもある。"},

	{ 40,30,70,FALSE,FALSE,A_DEX,0,0,"フェムトファイバー",
		"極めて丈夫な紐でモンスターを縛り上げ移動を妨害し行動を中確率で妨害する。成功率はプレイヤーとモンスターの速度差に依存し、自分より素早い敵を捕縛することは難しい。眠っていたり混乱している敵には効きやすく、巨大な敵・力強い敵・ユニークモンスターには効きにくい。"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_reisen2(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "個数:1";
			msg_print("あなたは慣れた手つきで餅を搗き始めた...");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
		}
		break;
	case 1:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);

				if(m_ptr->hold_o_idx)
				{
					object_type *q_ptr = &o_list[m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
					msg_format("%sから%sを奪った！",m_name,o_name);
					inven_carry(q_ptr);

				}
				else
				{
					msg_format("%sは何も持っていないようだ。",m_name);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

	case 2:
		{
			int time = 5 + plev / 10;
			if(only_info) return format("期間:%d",time);
			set_fast(time, FALSE);

		}
		break;
	case 3:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
		}
		break;

	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				int chance;
				int spd = p_ptr->pspeed;
				int mspd = m_ptr->mspeed;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags2 & RF2_POWERFUL) mspd += 5;
				if(r_ptr->flags2 & RF2_GIGANTIC) mspd += 5;

				if(MON_CSLEEP(m_ptr)) mspd -= 10;
				if(MON_CONFUSED(m_ptr)) mspd -= 5;

				chance = plev/2;
				chance += (spd - mspd) * 3;

				if(use_itemcard) chance += 75;
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 4 / 5;

				monster_desc(m_name, m_ptr, 0);
				if(m_ptr->mflag & MFLAG_FEMTO_FIBER)
				{
					msg_format("%sはすでに捕獲済みだ。",m_name);
					return NULL;
				}

				else if(randint0(100) < chance)
				{
					msg_format("%sをフェムトファイバーで捕縛した！",m_name);
					m_ptr->mflag |= MFLAG_FEMTO_FIBER;

				}
				else
				{
					msg_format("%sの捕縛に失敗した！",m_name);
				}
				anger_monster(m_ptr);


			}

		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::サグメ専用技*/
//神々の弾冠にtim_general1使用
class_power_type class_power_sagume[] =
{

	{12,32,30,FALSE,FALSE,A_INT,0,0,"烏合の呪",
		"周囲の敵を配下あるいは友好的にする。ただし敵の攻撃でピンチになる可能性のある状況で使わないと効果が発生せず逆に周囲の敵が起きて加速する。クエストの討伐対象やダンジョンボスには効果がない。"},

	{25,30,40,FALSE,FALSE,A_DEX,0,0,"穢身探知型機雷",
		"機雷を呼び出す。機雷は分裂しながらやや不規則に動き破邪属性の爆発で攻撃する。フロアを移動すると消える。"},

	{32,50,65,FALSE,TRUE,A_WIS,0,0,"神々の弾冠",
		"一時的に敵からの攻撃をHPでなくMPで受けるようになる。MPが尽きると効果が消える。光の剣属性の攻撃や敵からの攻撃でないダメージには効果がない。"},

	{ 40,80,75,FALSE,TRUE,A_INT,0,0,"金城鉄壁の陰陽玉",
		"一時的に物理防御上昇、魔法防御上昇、反射を得る。" },

	{45,64,75,FALSE,TRUE,A_CHR,0,0,"片翼の白鷺",
		"自分のいる場所に強力な閃光属性のボールを発生させてその後中距離テレポートする。このテレポートは反テレポートに妨害されない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_sagume(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return "効力:不定";

			if(one_in_(10)) msg_print("「...そうでは無い。」");
			else if(one_in_(9)) msg_print("「運命は逆転し始めた。」");
			else if(one_in_(8)) msg_print("「これはいわば勅命よ。」");
			else if (one_in_(7)) msg_print("「穢れに満ちた狂宴を続けるが良い！」");
			else msg_print("あなたは敵に向けて語り始めた..");

			if(process_warning(px,py,TRUE))
			{
				char m_name[80];
				charm_monsters(999);
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if (!is_hostile(m_ptr)) continue;
					if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					monster_desc(m_name,m_ptr,0);

					if(    (r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR)
						|| (r_info[m_ptr->r_idx].flagsr & RFR_RES_ALL))
					{
						msg_format("%sは敵対的なままだ！",m_name);
					}
					else
					{
						msg_format("%sは友好的になったようだ。",m_name);
						set_friendly(m_ptr);
					}
				}

			}
			else
			{
				msg_print("事態はあなたの望まない方向へ進み始めたようだ！");
				aggravate_monsters(0,TRUE);
			}

			break;
		}

	case 1:
		{
			bool flag = FALSE;
			int num = 1;
			if(plev > 44) num = 3;
			if(only_info) return format("個数:%d",num);

			for(;num>0;num--)
			{
				if(summon_named_creature(0,py,px,MON_SAGUME_MINE,PM_EPHEMERA)) flag = TRUE;
			}

			if(flag)
				msg_print("機雷を設置した。");
			else
				msg_print("設置に失敗した。");

		}
		break;

	case 2:
		{
			int base = 25;
			if(only_info) return format("期間:%d+1d%d",base,base);

			set_tim_general(base+randint1(base),FALSE,0,0);

		}
		break;

	case 3:
		{
			int v;
			int base = plev / 4;

			if (only_info) return format("期間:%d+1d%d", base,base);
			v = base + randint1(base);

			msg_print("強固な防壁を張り巡らせた。");
			set_shield(v, FALSE);
			set_resist_magic(v, FALSE);
			set_tim_reflect(v, FALSE);

		}
		break;

	case 4:
		{
			int dam = plev * 16 + chr_adj * 16;

			if(only_info) return format("損傷:最大%d",dam/2);
			msg_print("あなたは光の翼をはためかせて飛翔した！");
			project(0,6,py,px,dam,GF_LITE,(PROJECT_KILL|PROJECT_JUMP),-1);
			teleport_player(50+randint1(50),TELEPORT_NONMAGICAL);
		}
		break;

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::輝夜専用技*/
//p_ptr->magic_num1[0-4]を神宝保有モンスターリスト(シャッフル済み)格納領域として使う
class_power_type class_power_kaguya[] =
{
	{ 8, 8,20,FALSE,TRUE,A_STR,0,3,"ブリリアントドラゴンバレッタ(赤)",
		"火炎属性の小さな球を放つ。「★龍の頸の玉」を装備している場合強力なブレスを放つ。"},
	{11, 9,20,FALSE,TRUE,A_STR,0,3,"ブリリアントドラゴンバレッタ(白)",
		"冷気属性の小さな球を放つ。「★龍の頸の玉」を装備している場合強力なブレスを放つ。"},
	{14,10,20,FALSE,TRUE,A_STR,0,3,"ブリリアントドラゴンバレッタ(青)",
		"電撃属性の小さな球を放つ。「★龍の頸の玉」を装備している場合強力なブレスを放つ。"},
	{17,11,20,FALSE,TRUE,A_STR,0,3,"ブリリアントドラゴンバレッタ(黄)",
		"毒属性の小さな球を放つ。「★龍の頸の玉」を装備している場合強力なブレスを放つ。"},
	{20,11,20,FALSE,TRUE,A_STR,0,3,"ブリリアントドラゴンバレッタ(黒)",
		"酸属性の小さな球を放つ。「★龍の頸の玉」を装備している場合強力なブレスを放つ。"},

	{23,30,30,FALSE,TRUE,A_WIS,0,0,"ブディストダイアモンド",
		"一時的に魔法防御上昇を得る。「★仏の御石の鉢」を装備している場合魔法の鎧を身にまとう。"},
	{25,25,40,FALSE,TRUE,A_DEX,0,0,"サラマンダーシールド",
		"一時的に火炎攻撃への耐性と火炎のオーラを得る。「★火鼠の皮衣」を装備している場合視界内全てに火炎攻撃を放つ。"},
	{28,29,50,FALSE,TRUE,A_DEX,0,5,"月のイルメナイト",
		"ターゲットに隕石属性の球が発生する。「月のイルメナイト」を所持していないと使えない。"},
	{30,30,60,FALSE,TRUE,A_CON,0,0,"ライフスプリングインフィニティ",
		"体力を中程度回復させ毒と切り傷を治癒する。「★燕の子安貝」を装備している場合体力を大幅に回復させ全復活と肉体強化の効果を得る。"},
	{32,0,80,FALSE,TRUE,A_WIS,0,0,"ミステリウム",
		"MPを少し回復する。「ミステリウム」を所持していないと使えない。"},
	{35,35,65,FALSE,TRUE,A_CHR,0,10,"蓬莱の玉の枝",
		"閃光あるいは暗黒属性の小さな球を放つ。「★蓬莱の玉の枝」を装備している場合様々な属性の巨大な球を放つ。"},
	{36,72,65,FALSE,TRUE,A_INT,0,0,"永遠の術Ⅰ",
		"隣接した壁を永久壁に変化させる。クエストダンジョンでは使えない。"},
	{40,40,75,FALSE,TRUE,A_INT,0,0,"須臾の術Ⅰ",
		"指定した場所に一瞬で現れる。テレポート妨害を無視するが現在値から既知の通路が通っていない場所には移動できない。"},

	{43,80,80,FALSE,TRUE,A_CHR,0,0,"エイジャの赤石",
		"核熱属性の強力なレーザーを放つ。「エイジャの赤石」を所持していないと使えない。"},
//project_hack2に特殊オプションをつければいいか
	{45,128,85,FALSE,TRUE,A_INT,0,0,"永遠の術Ⅱ",
		"5ターンの間無敵化する。"},
	{48,160,85,FALSE,TRUE,A_STR,0,0,"金閣寺の一枚天井",
		"プレイヤーを中心とする縦横7グリッドに対し極めて強力な分解属性攻撃を行う。「金閣寺の一枚天井」を所持していないと使えない。また範囲内に永久壁やマップ端や上り階段などがあると失敗する。"},
	{50,255,90,FALSE,TRUE,A_INT,0,0,"須臾の術Ⅱ",
		"5回連続で行動する。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_kaguya(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:	case 1:
	case 2:	case 3:
	case 4:
		{
			bool nandai = FALSE;
			int dam;
			//numを配列添え字に使っているので変更時注意
			int gf_list[5] = {GF_FIRE,GF_COLD,GF_ELEC,GF_POIS,GF_ACID};

			if(inventory[INVEN_LITE].name1 == ART_KAGUYA_DRAGON) nandai = TRUE;

			dam = 25 + plev + chr_adj * 3/2  ;
			if(nandai) dam *= 3;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(nandai)
			{
				msg_format("龍の頸の玉から%sのブレスが放たれた！",gf_desc_name[gf_list[num]]);
				fire_ball(gf_list[num], dir, dam,-2);
			}
			else
			{
				msg_format("%sの玉を撃った。",gf_desc_name[gf_list[num]]);
				fire_ball(gf_list[num], dir, dam,1);
			}



		}
		break;
	case 5:
		{
			bool nandai = FALSE;
			int base = 10 + p_ptr->lev/5;
			if(inventory[INVEN_RARM].name1 == ART_KAGUYA_HOTOKE || inventory[INVEN_LARM].name1 == ART_KAGUYA_HOTOKE) nandai = TRUE;

			if(only_info) return format("期間:%d+1d%d",base,base);
			if(nandai)
			{
				msg_print("仏の御石の鉢が強固な防壁を生み出した！");
				set_magicdef(randint1(base) + base, FALSE);
			}
			else
			{
				msg_print("あなたは心を落ち着かせた..");
				set_resist_magic(randint1(base) + base,FALSE);
			}

		}
		break;
	case 6:
		{
			bool nandai = FALSE;
			int base,dam;
			if(inventory[INVEN_OUTER].name1 == ART_KAGUYA_HINEZUMI) nandai = TRUE;

			if(nandai)
			{
				int dam = plev * 3 + chr_adj * 5;
				if(only_info) return format("損傷:%d",dam);
				msg_print("火鼠の皮衣から猛火が放たれた！");
				project_hack2(GF_FIRE,0,0,dam);
			}
			else
			{
				int base = 20;
				int time;
				if(only_info) return format("期間:%d+1d%d",base,base);
				time = base + randint1(base);
				set_tim_sh_fire(time, FALSE);
				set_oppose_fire(time, FALSE);
			}
		}
		break;

	case 7:	//新難題　持ってるかどうかはほかで判定する
		{
			int base = plev * 6;
			int sides = chr_adj * 6;

			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 2,"月のかけらが降ってきた。")) return NULL;
		}
		break;
	case 8:
		{
			bool nandai = FALSE;
			int heal;
			if(inventory[INVEN_NECK].name1 == ART_KAGUYA_TSUBAME) nandai = TRUE;

			if(nandai)
			{
				int v = 10 + randint1(10);
				heal = 300+plev*2+chr_adj*4;
				if(only_info) return format("回復:%d",heal);
				set_tim_addstat(A_STR,105,v,FALSE);
				set_tim_addstat(A_DEX,105,v,FALSE);
				set_tim_addstat(A_CON,105,v,FALSE);
				restore_level();
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				msg_print("燕の子安貝から生命の力がとめどなく湧き出してくる！");
			}
			else
			{
				heal = 100+plev + chr_adj*2;
				if(only_info) return format("回復:%d",heal);
				msg_print("優しい光があなたを包んだ。");

			}
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
		}
		break;
	case 9:	//新難題　持ってるかどうかはほかで判定する
		{
			int base = plev/5;

			if(only_info) return format("回復:%d+1d%d",base,base);

			msg_print("不思議な魔力が体に満ちる..");
			player_gain_mana(base + randint1(base));

		}
		break;
	case 10:
		{
			bool nandai = FALSE;
			int dice,sides,dam;
			int typ_list[7] = {GF_LITE,GF_DARK,GF_MANA,GF_WATER,GF_SHARDS,GF_FORCE,GF_DISP_ALL};

			if(inventory[INVEN_RARM].name1 == ART_KAGUYA_HOURAI || inventory[INVEN_LARM].name1 == ART_KAGUYA_HOURAI) nandai = TRUE;

			if(use_itemcard) nandai = TRUE;

			dice = 4 + plev / 4;
			sides = 25 + chr_adj / 2;

			if(nandai) sides *= 2;

			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;

			dam = damroll(dice,sides);

			if(nandai)
			{
				msg_format("蓬莱の玉の枝が七色の光を放った！");
				fire_ball(typ_list[randint0(7)], dir, dam,5);
			}
			else
			{
				msg_format("きらめく球を放った。");
				fire_ball(typ_list[randint0(2)], dir, dam,1);
			}
		}
		break;
	case 11:
		{
			int xx, yy,i;
			bool flag = FALSE;
			if(only_info) return "";

			if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
			{
				msg_print("ここでは使えない。");
				return NULL;
			}
			for(i=0;i<8;i++)
			{
				yy = py + ddy_cdd[i];
				xx = px + ddx_cdd[i];
				if(!in_bounds(yy,xx)) continue;
				if(!cave_have_flag_bold((yy), (xx), FF_WALL)) continue;
				if(cave_have_flag_bold((yy), (xx), FF_PERMANENT)) continue;
				if(cave[yy][xx].m_idx)continue;
				cave_set_feat(yy, xx, feat_permanent);
				flag = TRUE;
			}
			if(flag)
				msg_print("周囲の壁に永遠の術をかけた。");
			else
				msg_print("何も起こらなかった。");
		}
		break;
	case 12:
		{
			int x, y;
			int cost;
			int dist = plev * 2;
			if(dist < 30) dist = 30;
			if(only_info) return format("移動コスト:%d",dist);
			if (!tgt_pt(&x, &y)) return NULL;

			if (!player_can_enter(cave[y][x].feat, 0) || !(cave[y][x].info & CAVE_KNOWN))
			{
				msg_print("そこには行けない。");
				return NULL;
			}
			forget_travel_flow();
			travel_flow(y,x);
			if(dist < travel.cost[py][px])
			{
				if(travel.cost[py][px] >= 9999)
					msg_print("そこには道が通っていない。");
				else
					msg_print("そこは遠すぎる。");
				return NULL;
			}

			msg_print("あなたは一瞬で移動した！");
			teleport_player_to(y,x,TELEPORT_NONMAGICAL);
		}
		break;
	case 13:
		{
			int dice = p_ptr->lev;
			int sides = chr_adj;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("エイジャの赤石が太陽の輝きを放った！");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),1);

			break;
		}
	case 14:
		{
			int time = 5;

			if(only_info) return format("期間:%d",time);
			msg_format("永遠の魔法があなたを守った。");
			set_invuln(time,FALSE);

		}
		break;
	case 15:
		{
			int dam = 1600;
			bool flag_ok = TRUE;
			int i,j,tx,ty;

			if(only_info) return format("損傷:%d",dam);

			for (i = -3; i < 3; i++) for (j = -3; j < 3; j++)
			{
				ty = py + i;
				tx = px + j;
				if(!in_bounds2(ty,tx) || !cave_have_flag_bold(ty,tx,FF_HURT_DISI) && !cave_have_flag_bold(ty,tx,FF_PROJECT) || cave_have_flag_bold(ty,tx,FF_LESS))
				{
					flag_ok = FALSE;
					break;
				}
			}
			msg_print("金閣寺の一枚天井が上から降ってきた！");
			if(!flag_ok)
			{
				msg_format("...しかし何かにつっかえて止まった。");
				break;
			}

			project_hack2(GF_KINKAKUJI,0,0,dam);
		}
		break;
	case 16:
		{
			if(only_info) return format("行動:5回");
			if (world_player)
			{
				msg_print("既に時は止まっている。");
				return NULL;
			}
			msg_print("あなたは須臾の術を発動した。");
			world_player = TRUE;
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			p_ptr->energy_need -= 1500;
			p_ptr->redraw |= (PR_MAP);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			handle_stuff();
			msg_print("時が止まって見える！");
			break;
		}



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}






//p_ptr->magic_num1[0]:輝夜が倒されたときのr_idx値
/*:::永琳専用技*/
class_power_type class_power_eirin[] =
{

	{8,10,25,FALSE,TRUE,A_DEX,0,0,"胡蝶夢丸",
		"隣接した敵一体を高確率で眠らせる。通常眠らない敵にも効果があるがユニーク・モンスターには効果が薄い。"},
	{12,25,30,FALSE,TRUE,A_INT,0,0,"仙香玉兎",
		"視界内のモンスターを高確率で混乱、朦朧させる。ユニークモンスターには効果がない。"},
	{20,60,35,FALSE,TRUE,A_INT,0,0,"壺中の天地",
		"フロアを再構成する。切り替えは「ほぼ」一瞬で行われる。EXTRAモードではテレポート・レベルを行う。"},
	{23,36,50,FALSE,TRUE,A_INT,0,0,"神代の記憶",
		"所持品全てを鑑定し、さらに視界内のモンスターの情報を得る。"},
	{26,28,45,FALSE,TRUE,A_DEX,0,5,"アポロ１３",
		"隕石属性のビームを放つ。威力は弓の性能で変わる。"},
	{30,40,60,FALSE,TRUE,A_CHR,0,0,"ライフゲーム",
		"隣接した生物一体のHPを最大まで回復させる。"},
	{35,0,70,FALSE,TRUE,A_WIS,0,0,"オモイカネブレイン",
		"短時間知能と賢さを上昇させ、さらにMPを少し回復する。"},
	{37,80,70,FALSE,TRUE,A_CON,0,0,"水銀の海",
		"視界内全てに対し強力な無属性攻撃を仕掛ける。"},
	{40,40,80,FALSE,TRUE,A_INT,0,0,"天文密葬法",
		"主の輝夜と自分の場所を入れ替える。二人がフロアのどこにいても効果が発生する。"},
	{44,120,90,FALSE,TRUE,A_INT,0,0,"天網蜘網捕蝶の法",
		"ターゲットの位置周辺に「守りのルーン」を敷き詰める。"},
	{48,250,95,FALSE,TRUE,A_CHR,0,0,"ライジングゲーム",
		"フロアの全ての生物が分裂する。ユニーク・モンスターには効果がない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_eirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	//胡蝶夢丸
	case 0:
		{
			int x,y;
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int power = 90 + plev + chr_adj * 2;
			if(only_info) return format("基本効力:%d",power);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (!cave[y][x].m_idx || !m_ptr->ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);
			if((r_ptr->flagsr & RFR_RES_ALL) || (r_ptr->flags1 & RF1_QUESTOR) | (r_ptr->flags2 & RF2_EMPTY_MIND) )
			{
				msg_format("%sには薬に対する完全な耐性がある！",m_name);
				break;
			}
			else if(MON_CSLEEP(m_ptr)) power *= 2;

			if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 2;
			if(r_ptr->flags3 & RF3_NO_SLEEP) power /= 2;

			msg_print("特製の丸薬を投与した。");
			if(r_ptr->level >= power)
			{
				msg_format("%sには全く薬が効く様子がない！",m_name);
			}
			else if(r_ptr->level > randint1(power))
			{
				msg_format("%sには薬の効きが悪いようだ。",m_name);
			}
			else
			{
				if(MON_CSLEEP(m_ptr)) msg_format("%sはさらに深く眠ったようだ...",m_name);
				else msg_format("%sは眠った。",m_name);
				(void)set_monster_csleep(cave[y][x].m_idx, power);
			}

		}
		break;
	case 1:
		{
			int power = 50 + plev * 4;
			if(only_info) return format("効力:%d",power);

			msg_print("幻覚をもたらす香を焚いた...");
			stun_monsters(power);
			confuse_monsters(power);
		}
		break;
	case 2:
		{
			if(only_info) return "";

			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}

			msg_print("あなたは薬壺へと飛び込んだ！");

			if(EXTRA_MODE)
			{
				teleport_level(0);
			}
			else
			{
				p_ptr->alter_reality = 1;
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		break;
	case 3:
		{
			if(only_info) return "";
			msg_print("過去の記憶を探った...");
			identify_pack();
			probing();
		}
		break;
	//アポロ13
	case 4:
		{
			object_type *o_ptr;
			int mult;
			int dam;
			int ty,tx;
			bool dummy;

			if(inventory[INVEN_LARM].tval == TV_BOW || inventory[INVEN_RARM].tval == TV_BOW)
			{
				if(inventory[INVEN_LARM].tval == TV_BOW)
					o_ptr = &inventory[INVEN_LARM];
				else
					o_ptr = &inventory[INVEN_RARM];

				mult = bow_tmul(o_ptr->tval,o_ptr->sval);
				if (p_ptr->xtra_might) mult++;

				dam = (plev + o_ptr->to_d + chr_adj) * mult;

				if(dam < 1) dam=1;
			}
			else dam = 0;

			if(only_info) return  format("損傷:%d",dam);
			if(!dam)
			{
				msg_print("弓を装備していない。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_print("狙い澄ました一撃を放った。");
			fire_beam(GF_METEOR, dir, dam);

		}
		break;

			//ライフゲーム
	case 5:
		{
			int y, x;
			int dist;
			int damage;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int heal;
			char m_name[80];
			int target_m_idx;

			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			target_m_idx = cave[y][x].m_idx;
			if(!target_m_idx || !m_list[target_m_idx].ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			m_ptr = &m_list[target_m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			if(monster_living(r_ptr))
			{
				msg_format("あなたは%^sの治療を始めた...", m_name);
				m_ptr->hp = m_ptr->maxhp;
				(void)set_monster_slow(target_m_idx, 0);
				(void)set_monster_stunned(target_m_idx, 0);
				(void)set_monster_confused(target_m_idx, 0);
				(void)set_monster_monfear(target_m_idx, 0);
				if (p_ptr->health_who == target_m_idx) p_ptr->redraw |= (PR_HEALTH);
				if (p_ptr->riding == target_m_idx) p_ptr->redraw |= (PR_UHEALTH);
				redraw_stuff();
				msg_format("%^sは完全に治った！", m_name);
			}
			else
				msg_format("%^sを治療することはできない。", m_name);
			break;
		}

	case 6:
		{
			int base = plev / 10;
			int time;

			if(base < 3) base=3;
			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);

			msg_print("精神を集中した！");
			set_tim_addstat(A_INT,100+base,time,FALSE);
			set_tim_addstat(A_WIS,100+base,time,FALSE);
			calc_bonuses();
			player_gain_mana(plev/4 + randint1(plev/4));

		}
		break;

	case 7://水銀の海 DISP_ALL
		{
			int dam = plev * 3 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			project_hack2(GF_DISP_ALL,0,0,dam);

			break;
		}

	case 8://天文密葬法
		{
			monster_type *m_ptr;
			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (m_ptr->r_idx == MON_MASTER_KAGUYA) break;
			}
			if(i >= m_max)
			{
				msg_print("今は輝夜がいない。");
				return NULL;
			}
			if(i == p_ptr->riding)
			{
				msg_print("WARNING:そこまでよ！");
				return NULL;
			}
			msg_print("あなたは輝夜の身代わりになった！");
			//move_player_effectにモンスターのいる場所を指定すれば標準で位置交換する
			(void)move_player_effect(m_ptr->fy, m_ptr->fx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
			break;
		}
	case 9:
		{
			int rad = 3;
			if(only_info) return format("半径:%d",rad);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_MAKE_GLYPH, dir, 0, rad,"標的を強力な結界に閉じ込めた！")) return NULL;

		}
		break;
	//ライジングゲーム　全ての生物に限定
	case 10:
		{
			if(only_info) return format("");

			msg_print("フロアが異様なエネルギーに満たされた...");

			raising_game();
		}
		break;

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::清蘭専用技*/
class_power_type class_power_seiran[] =
{

	{6,6,25,FALSE,FALSE,A_DEX,0,0,"団子生成",
		"アイテム「団子」を作る。杵を装備していないと使えない。"},

	{12,10,30,FALSE,TRUE,A_INT,0,0,"異次元射撃Ⅰ",
		"視界内のターゲットの位置に直接銃弾が出現する射撃を行う。外れることがなく反射されない。一部の特殊な銃器はこの能力の対象にならない。"},

	{16,20,30,FALSE,FALSE,A_CHR,0,0,"救難信号",
		"「イーグルラヴィ」を友好的な状態で数体呼び出す。階層が深いと失敗しやすい。"},

	{20,15,35,FALSE,TRUE,A_INT,0,0,"異次元射撃Ⅱ",
		"射撃の当たった敵を短～中距離テレポートさせる。テレポート耐性や時空攻撃耐性をもつ敵には効果がない。一部の特殊な銃器はこの能力の対象にならない。"},

	{25,30,50,FALSE,TRUE,A_DEX,0,0,"ルナティックドリームショット",
		"人間に大ダメージを与える射撃を放つ。ロケットランチャーや一部の特殊な銃器はこの能力の対象にならない。" },

	{30,25,45,FALSE,TRUE,A_INT,0,0,"異次元射撃Ⅲ",
		"壁に当たると一度だけランダムな敵へ向かって跳ね返る射撃を放つ。ビームかボルトを放つ銃器でないと使えない。"},

	{36,40,60,FALSE,TRUE,A_DEX,0,0,"スピードストライク",
		"視界内のターゲット一体に向け、一行動で残弾全てを発射する。射撃の成功率がやや下がる。"},

	{ 40,50,75,FALSE,TRUE,A_DEX,0,0,"イーグルシューティング",
		"空を飛ぶモンスターに大ダメージを与える射撃を放つ。ロケットランチャーや一部の特殊な銃器はこの能力の対象にならない。" },

	{47,60,70,FALSE,TRUE,A_CHR,0,0,"鷹は撃ち抜いた",
		"壁を貫通する強力な時空属性ビームの射撃を放つ。永久壁や山脈は貫通しない。ロケットランチャーや一部の特殊な銃器はこの能力の対象にならない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_seiran(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			int dice = 1 + plev / 16;
			int sides = 3;
			object_type forge, *q_ptr = &forge;
			if(only_info) return format("個数:%dd%d",dice,sides);
			msg_print("あなたは団子を搗き始めた...");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			q_ptr->discount = 99;
			q_ptr->number = damroll(dice,sides);
			drop_near(q_ptr, -1, py, px);
		}
		break;
	case 1:
	case 3:
	case 4:
	case 5:
	case 7:
	case 8:
	{
			if(only_info) return "";

			if(num == 1)
				special_shooting_mode = SSM_SEIRAN1;
			else if(num == 3)
				special_shooting_mode = SSM_SEIRAN2;
			else if (num == 4) //v1.1.53追加
				special_shooting_mode = SSM_SLAY_HUMAN;
			else if(num == 5)
				special_shooting_mode = SSM_SEIRAN3;
			else if (num == 7) //v1.1.53追加
				special_shooting_mode = SSM_SLAY_FLYER;
			else if (num == 8)
				special_shooting_mode = SSM_SEIRAN5;

			if(!do_cmd_fire())
			{
				special_shooting_mode = 0L;
				return NULL;
			}
		}
		break;
	case 2:
		{
			int i;
			bool flag = FALSE;
			int num;
			if(only_info) return "";

			msg_print("あなたはメーデーを発信した！");
			num = randint1(6) + p_ptr->lev / 5 - dun_level / 3;
			if(p_ptr->inside_quest) num /= 3;

			for(i=0;i<num;i++)
			{
				if (summon_named_creature(0, py, px, MON_EAGLE_RABBIT, PM_FORCE_FRIENDLY)) flag = TRUE;
			}
			if(flag)
				msg_print("仲間が助けに現れた！");
			else
				msg_print("しかし誰も来なかった...");

		}
		break;
	case 6:
		{
			int timeout_base, timeout_max;
			object_type *o_ptr;
			bool flag = FALSE;
			if(only_info) return "";

			if (!get_aim_dir(&dir)) return NULL;

			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}

			if(one_in_(3))
				msg_print("「見せてやるわ、月面最速ガンナーの妙技を！」");
			else
				msg_print("残弾全てを乱射した！");

			special_shooting_mode = SSM_SEIRAN4;
			while(1)
			{
				bool righthand = FALSE;
				bool lefthand = FALSE;

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
				if(!righthand && !lefthand) break;
				else flag = TRUE;

				if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);
				if(!target_okay()) break;
				if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);
				if(!target_okay()) break;
			}

			if(!flag) msg_print("..しかし元々弾がなかった。");
		}
		break;

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}

	//特殊射撃フラグをリセット
	special_shooting_mode = 0L;

	return "";
}


//所持品から指定個数の団子を減らす。足りないとき何もせずFALSEを返す。
bool ringo_dec_dango(int num)
{
	int i,cnt=0;
	object_type *o_ptr;

	//まずザック全ての団子の数を数える。銘や割引で細かく分かれている可能性を考慮。
	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_SWEETS || o_ptr->sval != SV_SWEETS_DANGO) continue;
		cnt += o_ptr->number;
	}

	if(cnt < num)
	{
		msg_print("団子が足りない...");
		return FALSE;
	}
	if(num == 1)
		msg_format("あなたは団子をひとつ食べた。");
	else if(num < 6)
		msg_format("%d個の団子を頬張った！",num);
	else
		msg_format("%d個の団子を一気に呑み込んだ！",num);

	//これまで食べた団子の個数カウント
	p_ptr->magic_num1[0] += num;
	if(p_ptr->magic_num1[0] > 10000) p_ptr->magic_num1[0] = 10000;


	//団子を指定個数減らす
	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_SWEETS || o_ptr->sval != SV_SWEETS_DANGO) continue;
		if(o_ptr->number < num)
		{
			num -= o_ptr->number;
			inven_item_increase(i,-(o_ptr->number));
		}
		else
		{
			inven_item_increase(i,-num);
			num = 0;
		}

		if(!num) break;
	}
	//optimizeはまとめて行う。上のループで減らすたびに処理すると処理されるべきインベントリがずれるため。
	for(i=INVEN_PACK-1;i>=0;i--) inven_item_optimize(i);
	p_ptr->window |= (PW_INVEN);


	return TRUE;
}

//鈴瑚特技
//tim_general[0] 団子を食べた強化カウンタ　20turn 新たに食べれば20に設定し直し
//tim_general[1] ダンゴインフルーエンスの免疫カウンタ
//magic_num1[0] これまで食べた団子の個数カウント
//magic_num1[1] 現在の強化段階　数値分腕力器用耐久+ tim_general[0]のカウンタが切れたら0に戻る
//magic_num1[2] 最後に受けた属性攻撃のGF値を記録しておく
//magic_num1[3] ダンゴインフルーエンスで免疫を得ているGF値。tim_general[1]のカウンタが切れたら0に戻る
//magic_num1[4] 支援物資の箱の生成カウント。時間経過か最深到達階層ごとに加算され、一定値以上でフロア生成時に『「支援物資」と書かれた箱』を生成し0に戻る
//magic_num2[1～] 各ダンジョンの最深到達階層。救援物資箱の生成カウントに使う。既存の変数はリセット可能なので別管理する。各添字はDUNGEON_*値に対応

class_power_type class_power_ringo[] =
{

	{5,6,20,FALSE,FALSE,A_INT,0,1,"周辺調査",
		"周囲のモンスターを感知する。レベル10でトラップ、レベル20でアイテム、レベル30で地形も感知する。効果範囲はレベルアップに伴い広がる。"},
	{18,6,25,FALSE,TRUE,A_STR,0,2,"ストロベリーダンゴ",
		"団子を1つ消費し、気属性の球を放つ。"},
	{23,12,30,FALSE,FALSE,A_INT,0,0,"モンスター調査",
		"周囲のモンスターの情報を得る。"},

	{27,24,40,FALSE,TRUE,A_CON,0,0,"ダンゴインフリューエンス",
		"団子を3つ消費し、一時的に「この技を使う直前に受けた攻撃の属性」に対する完全な免疫を得る。ただし「冷気」と「極寒」、「火炎」と「核熱」などは全く別の属性として扱われる。光の剣属性の免疫は得られない。"},
	{30,25,60,FALSE,FALSE,A_INT,0,0,"鑑識",
		"アイテムをひとつ鑑定する。レベル45以降はアイテムの完全な情報を得る。"},
	{32,33,33,FALSE,TRUE,A_DEX,0,0,"ダンゴ三姉妹",
		"団子を3つ消費し、一時的に火炎、冷気、電撃攻撃への耐性を得る。" },
	{35,40,65,FALSE,FALSE,A_INT,0,5,"浄化作戦",
		"周囲にいる妖怪、悪魔、獣を現在のフロアから高確率で消滅させる。ユニークモンスターには効果がない。"},
	{40,24,75,FALSE,TRUE,A_STR,0,0,"ベリーベリーダンゴ",
		"団子を5つ消費し、極めて強力な気属性の球を放つ。"},

	{43,60,80,FALSE,TRUE,A_CHR,0,0,"ダンゴフラワー",
		"団子を7つ消費し、自分の居る場所を中心に巨大な気属性の球を発生させる。" },

	{45,30,85,FALSE,TRUE,A_CON,0,0,"セプテンバーフルムーン",
		"団子を9つ消費し、短時間無敵のバリアを張る。効果が切れるときに少し行動時間を消費するので注意。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
cptr do_cmd_class_power_aux_ringo(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0://周辺調査
		{
			int rad = 15 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			msg_print("周辺の情報を収集解析した...");
			detect_monsters_normal(rad);
			if(plev < 10) detect_monsters_invis(rad);
			if(plev >  9) detect_traps(rad, TRUE);
			if(plev > 19) detect_objects_normal(rad);
			if(plev > 29) map_area(rad);
		}
		break;
	case 1: //ストロベリーダンゴ
		{

			int dam = 50 + chr_adj * 10 / 3 + plev * 2;
			int rad = 1;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(!ringo_dec_dango(1)) return NULL;

			msg_format("団子のパワーをエネルギーにして投げつけた。");
			fire_ball(GF_FORCE, dir, dam,rad);

			break;
		}
		break;
	case 2:
		{
			if(only_info) return "";

			msg_format("周囲の敵を調査した...");
			probing();
			break;
		}
	case 3:
		{
			int base = 5 + plev/5;
			int get_typ = p_ptr->magic_num1[2];
			if(only_info) return format("期間:%d 効果:%s属性",base,gf_desc_name[get_typ]);
			if(!ringo_dec_dango(3)) return NULL;

			if(get_typ == GF_PSY_SPEAR)
			{
				msg_print("その攻撃への免疫は得られそうにない。");
				return NULL;
			}

			set_tim_general(base,FALSE,1,get_typ);
			p_ptr->magic_num1[2] = 0;

			break;
		}
	case 4:
		{
			if(only_info) return format("");

			if(plev < 45)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
			break;
		}
	case 5: //v1.1.53 ダンゴ三姉妹
	{
		int base = 20;
		int v;
		if (only_info) return format("期間:%d+1d%d", base, base);
		if (!ringo_dec_dango(3)) return NULL;
		v = base + randint1(base);
		set_oppose_elec(v, FALSE);
		set_oppose_fire(v, FALSE);
		set_oppose_cold(v, FALSE);


	}
	break;

	case 6://浄化作戦
		{
			int power = 50 + plev * 4;
			if(only_info) return format("効力:%d",power);
			msg_format("蜘蛛型探査機が周囲へ散った...");
			mass_genocide_2(power, MAX_SIGHT, 0);
		}
		break;
	case 7: //ベリーベリーダンゴ
		{

			int dam = 180 + chr_adj * 8 + plev * 6;
			int rad = 4;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(!ringo_dec_dango(5)) return NULL;

			msg_format("巨大なエネルギーの塊を叩き付けた！");
			fire_ball(GF_FORCE, dir, dam,rad);
		}
		break;
	case 8: //ダンゴフラワー
	{

		int dam = 400 + chr_adj * 16 + plev * 16;
		int rad = 8;

		if (only_info) return format("損傷:%d", dam/2);
		if (!ringo_dec_dango(7)) return NULL;

		msg_format("この団子は夢のように美味だ！");
		project(0, rad, py, px, dam, GF_FORCE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);
	}
	break;

	case 9: //セプテンバーフルムーン
		{
			int time = 9;

			if(only_info) return format("期間:%d",time);
			if(!use_itemcard && !ringo_dec_dango(9)) return NULL;
			msg_format("無傷の球があなたを覆った！");
			set_invuln(time,FALSE);

		}
		break;


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



//Extraモードのアイテムカード用の特殊特技欄　ここはレベル順に並んでなくても問題ないはず？
class_power_type class_power_extra[] =
{
	{50,100,80,FALSE,TRUE,A_CHR,0,0,"ファイナルスパーク",
		"分解属性の極めて強力なビームを放つ。"},
	{40,100,80,FALSE,TRUE,A_CHR,0,0,"新たなる希望の面",
		"体力大幅回復、全復活"},
	{40,100,80,FALSE,TRUE,A_CHR,0,0,"永琳の丹",
		"体力回復、全復活" },

	{ 40,100,80,FALSE,TRUE,A_CHR,0,0,"ニワタリ神の御利益",
		"毒回復、一時毒耐性、喘息治癒" },

	{ 30,100,80,FALSE,TRUE,A_CHR,0,0,"山如の煙草",
		"周辺デバフ" },


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_extra(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{


	case 0: //ファイナルマスパ1600
		{
			int dam = 1600;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_DISINTEGRATE,dir, dam,2);

			break;
		}
	case 1:
		{
			if(only_info) return format("回復:500+α");
			msg_print("希望の面が眩い光を放った！");
			hp_player(500);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			restore_level();
			do_res_stat(A_STR);
			do_res_stat(A_INT);
			do_res_stat(A_WIS);
			do_res_stat(A_DEX);
			do_res_stat(A_CON);
			do_res_stat(A_CHR);
			break;
		}
		break;

		//永琳の丹
	case 2:
	{
		if (only_info) return format("回復:100+α");
		msg_print("黒い丸薬を飲み込んだ。");
		hp_player(100);
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_asthma(0);
		break;
	}
	break;

	case 3://鶏の描かれた絵馬
	{

		if (only_info) return format(" ");

		if(p_ptr->pclass != CLASS_KUTAKA)
			msg_print("ニワタリ神の力を感じる...");

		if (p_ptr->muta2 & MUT2_ASTHMA)
		{
			//パチュリー特殊処理　久侘歌の能力で永久変異の喘息が治る
			//厳密には喘息は喉の病気ではないけどニワタリ神の御利益は咳止めだしたぶんギリギリセーフだと思う:)
			if (p_ptr->pclass == CLASS_PATCHOULI)
			{
				msg_print("なんと、長年苦しめられてきた喘息が快癒した！");
				p_ptr->muta2_perma &= ~(MUT2_ASTHMA);
				p_ptr->muta2 &= ~(MUT2_ASTHMA);
				p_ptr->magic_num2[0] = 1; //種族変更とかしたときに再び喘息にならないフラグ

			}
			else
			{
				lose_mutation(116);

			}
			p_ptr->asthma = 0;


		}
		else
		{
			msg_print("なんだか喉が丈夫になった気がする。");
		}

		set_oppose_pois(5000, FALSE);


		break;

	}

	case 4:
	{
		int power = plev * 6;
		u32b flg = (PROJECT_JUMP | PROJECT_KILL);
		if (power < 100) power = 100;
		if (only_info) return format("効力:%d",power);

		msg_print("周囲は濃密な煙草の煙に満たされた...");
		project(0, 7, py, px, power, GF_OLD_CONF, flg, -1);
		project(0, 7, py, px, power, GF_STUN, flg, -1);
		project(0, 7, py, px, power, GF_OLD_SLOW, flg, -1);


	}
	break;


	default:
		if(only_info) return format("");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}






/*紫専用技*/
//「動と静の均衡」にtim_general[0]
class_power_type class_power_yukari[] =
{
	{4,8,15,FALSE,FALSE,A_INT,0,0,"非常識の裏側",
		"指定方向の壁など移動不可地形の向こうへ瞬間移動する。マップ端に到達すると反対側へ出る。通常のテレポートで移動できない場所には出られない。"},
	{8,16,24,FALSE,FALSE,A_INT,0,4,"召喚八雲式",
		"周囲のランダムな敵一体に無属性ダメージを与える。この攻撃は6/7の確率で再発動する。フロアに「八雲　藍」がいると使えない。"},
	{10,0,0,FALSE,FALSE,A_INT,0,0,"弾幕結界",
		"様々な魔法や特技を使う。使える魔法や特技は一時間ごとに入れ替わる。"},
	{12,24,30,FALSE,FALSE,A_DEX,0,0,"動と静の均衡",
		"一定時間、近くのモンスターの移動を抑止する。"},
	{14,24,40,FALSE,FALSE,A_DEX,0,5,"無限の超高速飛行体",
		"指定したターゲットに対し、自宅に置かれた近接武器が数本投擲される。"},
	{16,32,30,FALSE,FALSE,A_WIS,0,0,"夢と現の呪",
		"一瞬のタイムラグの後、現在のフロアが再構築される。地上やクエストダンジョンでは使えない。"},
	{18,24,30,FALSE,FALSE,A_DEX,0,0,"八雲の巣",
		"視界内のアイテムをランダムに自宅に放り込む。"},
	{20,56,40,FALSE,FALSE,A_INT,0,0,"四重結界",
		"一時的に元素攻撃への耐性を得る。レベル30でAC上昇、レベル40で魔法の鎧も追加される。"},
	{22,24,30,FALSE,FALSE,A_INT,0,0,"波と粒の境界",
		"一定時間、プレイヤーが使う属性攻撃の属性値が1つずれる。"},
	{25,16,50,FALSE,FALSE,A_INT,0,0,"二次元と三次元の境界",
		"魔法書を一冊消費し、それに記された魔法をひとつ発動させる。キャンセルしても行動力は消費する。"},
	{28,48,50,FALSE,FALSE,A_INT,0,8,"光と闇の網目",
		"視界内に閃光攻撃か暗黒攻撃を行う。"},
	{30,48,60,FALSE,FALSE,A_INT,0,0,"色と空の境界",
		"HPとMPを入れ替える。HP50以下の時は使用できない。"},
	{32,64,70,FALSE,FALSE,A_INT,0,0,"幻想狂想穴",
		"指定した場所に瞬間移動する。テレポート禁止の地形には出られない。この特技は広域マップでも使うことができる。"},
	{34,32,70,FALSE,FALSE,A_INT,0,0,"ぶらり廃駅下車の旅",
		"「電車」属性のビームを放つ。無属性ダメージを与え敵を吹き飛ばすが巨大な敵には効果が薄い。"},
	{36,48,65,FALSE,FALSE,A_CHR,0,0,"式神「八雲藍」",
		"モンスター「八雲　藍」を配下として召喚する。フロアを移動すると消える。"},
	{38,48,50,FALSE,FALSE,A_INT,0,0,"人間と妖怪の境界",
		"一定時間、モンスターに武器スレイを適用するときの種族フラグが1つ前にずれる。"},

	{40,64,80,FALSE,FALSE,A_DEX,0,0,"至る処に青山あり",
		"どこからでも自宅にアクセスする。周囲にやや広いスペースがないと使えない。クエスト実行中のフロアでは使えない。"},

	{43,64,80,FALSE,FALSE,A_INT,0,0,"八雲紫の神隠し",
		"周囲の敵が高確率でフロアから消滅する。"},
//	{43,72,73,FALSE,FALSE,A_INT,0,0,"生と死の境界",
//		"一定時間、近くの敵の耐性フラグが反転する。「全ての攻撃に対する耐性」や種族由来の耐性は反転されない。"},
	{45,120,85,FALSE,FALSE,A_INT,0,0,"客観結界",
		"フロアの壁を床を入れ替える。地上やクエストダンジョンでは使えない。"},
	{48,96,80,FALSE,FALSE,A_INT,0,0,"ラプラスの魔",
		"フロアの全てを感知する。さらに一定時間周囲の全てを感知し続け知能と賢さが大幅に上昇しAC大幅上昇と警告を得る。"},
//	{49,128,85,FALSE,FALSE,A_INT,0,0,"深弾幕結界",
//		"周囲のランダムなターゲットに対し連続でランダムな攻撃魔法を放つ。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_yukari(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");

			if (!wall_through_telepo(0)) return NULL;

			break;
		}
	case 1:
		//v1.1.44 ルーチン見直し
		{
			int tx, ty;
			int dam = 10 + plev / 2 + chr_adj / 2;
			int rad = 0;
			monster_type *m_ptr;

			if (only_info) return format("損傷:%d * 不定", dam);

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (m_ptr->r_idx == MON_RAN)
				{
					if (m_ptr->mflag & MFLAG_EPHEMERA) msg_print("すでに藍を使役中だ。");
					else msg_print("藍は呼び出しに応じない。");
					return NULL;
				}
			}

			msg_format("藍が回転しながらスキマから飛び出した！");
			if(!execute_restartable_project(0,0,7,0,0,dam,GF_ARROW,rad))
				msg_format("藍「紫様、標的がいませんが。」");

		}
	/*
		{
			int tx,ty;
			int dam = 10 + plev/2 + chr_adj/2 ;
			int rad = 0;
			int flg = (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN);
			monster_type *m_ptr;

			if(only_info) return format("損傷:%d * 不定",dam);

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_RAN)
				{
					if(m_ptr->mflag & MFLAG_EPHEMERA) msg_print("すでに藍を使役中だ。");
					else msg_print("藍は呼び出しに応じない。");
					return NULL;
				}
			}

			if (!get_aim_dir(&dir)) return NULL;

			oonusa_dam = dam;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			msg_format("藍が回転しながら飛んだ！");
			project(0,  rad, ty, tx, oonusa_dam, GF_ARROW, flg, -1);
			oonusa_dam = 0;
		}
		*/
		break;
	case 2:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 3:
		{
			int rad = 2 + plev / 16;
			int time = 10+plev/5;
			if(only_info) return format("期間:%d 範囲:%d",time,rad);

			if(p_ptr->tim_general[0])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("あなたは境界を操作した。周囲に奇妙な場が形成された・・");
			set_tim_general(time,FALSE,0,0);


		}
		break;
	case 4:
		{
			int num = 2 + plev / 8;
			int     item;
			int ty,tx;
			int cnt;
			store_type *st_ptr = &town[TOWN_HITOZATO].store[STORE_HOME];
			if(only_info) return format("回数:%d",num);

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

			for(;num>0;num--)
			{
				object_type *o_ptr;
				object_type forge;
				object_type *q_ptr = &forge;
				char		o_name[MAX_NLEN];

				cnt = 0;
				for(i=0;i<st_ptr->stock_num;i++)
				{
					o_ptr = &st_ptr->stock[i];
					if(!object_is_melee_weapon(o_ptr)) continue;
					cnt += o_ptr->number;
				}

				if(!cnt)
				{
					msg_print("スキマを開いたが、何も出てこなかった。");
					break;
				}

				cnt = randint1(cnt);
				for(item=0;item<st_ptr->stock_num;item++)
				{
					o_ptr = &st_ptr->stock[item];
					if(!object_is_melee_weapon(o_ptr)) continue;
					cnt -= o_ptr->number;
					if(cnt <= 0) break; //このときのitemとo_ptrの武器を投擲する
				}
				if(!object_is_melee_weapon(o_ptr))
				{
					msg_print("ERROR:「無限の超高速飛行体」で選定されたアイテムが近接武器ではない");
					break;
				}

				object_copy(q_ptr,o_ptr);
				q_ptr->number = 1;
				object_desc(o_name,q_ptr,OD_NAME_ONLY);
				msg_format("スキマから%sが撃ち出された！",o_name);
				if(do_cmd_throw_aux2(py,px,ty,tx,1,q_ptr,4))
				{
					yukari_dec_home_item(item);
				}

			}
		}
		break;
	case 5:
		{
			if(only_info) return format("効果:現実変容");

			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}

			msg_print("夢から醒めるように感覚が曖昧になっていく・・");
			/*:::0ではまずいはず*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	case 6:
		{
			int j;
			int num = 2 + plev / 8;
			bool flag_pick = FALSE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("最大:%d個",num);
			if(use_itemcard)
				msg_print("周囲のあちこちにスキマが開いた!");
			else
				msg_print("あなたはあちこちにスキマを開いた・・");
			//フロアのアイテムを全て調べる
			for(i=0;i<num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				//視界内のアイテムをランダムに選定
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (o_ptr->tval == TV_GOLD) continue;
					if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				object_desc(o_name, q_ptr, 0);
				if(!yukari_send_item_to_home(q_ptr))
				{
					msg_print("スキマがうまく開かない。拠点がもうアイテムで一杯だ。");
					break;
				}

				msg_format("%sがスキマの中に落ちた。",o_name);
				floor_item_increase(item, -255);
				floor_item_describe(item);
				floor_item_optimize(item);

			}
			if(!flag_pick)
			{
				msg_print("目ぼしい物は見当たらない。");
				break;
			}

		}
		break;


	case 7:
		{
			int time = 5 + plev / 2;

			if(only_info) return format("期間:%d",time);

			msg_print("強固な結界を張り巡らせた。");
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);

			if(plev > 29) set_shield(time,FALSE);
			if(plev > 39) set_magicdef(time, FALSE);
		}
		break;
	case 8:
		{
			int time = 10+plev/5;
			if(only_info) return format("期間:%d",time);

			if(p_ptr->tim_general[1])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("あなたは境界を操作した。");
			set_tim_general(time,FALSE,1,0);


		}
		break;
	case 9:
		{
			/*:::Mega Hack - 魔法書を消費して習得していない魔法を発動する特殊フラグ*/
			if(only_info) return format("");
			flag_spell_consume_book = TRUE;
			do_cmd_cast();
			flag_spell_consume_book = FALSE;

		}
		break;


	case 10:
		{
			int base = plev * 3 + chr_adj * 5;

			if(only_info) return format("損傷:%d+1d%d",base,base);
			msg_format("空間を埋め尽くすようなレーザーが放たれた！");
			project_hack2(GF_YUKARI_STORM,1,base,base);
			break;
		}

	case 11:
		{
			int tmp;
			if(only_info) return format("");

			if(!use_itemcard && p_ptr->chp < 50)
			{
				msg_print("体力が少なすぎて使えない。");
				return NULL;
			}
			tmp = p_ptr->csp;
			p_ptr->csp = p_ptr->chp;
			p_ptr->chp = tmp;
			if(p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;
			if(p_ptr->chp > p_ptr->mhp) p_ptr->chp = p_ptr->mhp;
			p_ptr->csp_frac = 0;
			p_ptr->chp_frac = 0;
			p_ptr->redraw |= (PR_HP | PR_MANA);
			redraw_stuff();
			msg_print("体力と魔力が入れ替わった。");
		}
		break;
	case 12:
		{
			if(only_info) return format("距離:無制限");
			if (!dimension_door(D_DOOR_YUKARI)) return NULL;
		}
		break;
	case 13:
		{
			int base = 100 + plev * 3;

			if(only_info) return format("損傷:%d",base);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("大きく開いたスキマから電車が飛び出してきた！");
			fire_beam(GF_TRAIN, dir, base);
			break;
		}
	case 14:
		{
			int i;
			int flag_ran = TRUE;
			int flag_chen = TRUE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			u32b mode = PM_EPHEMERA;

			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_RAN) ) flag_ran = FALSE;
				if((m_ptr->r_idx == MON_CHEN) ) flag_chen = FALSE;
			}

			if(flag_ran && summon_named_creature(0, py, px, MON_RAN, PM_EPHEMERA))
			{
				msg_print("藍を呼び出した。");
			}
			else if(flag_chen && summon_named_creature(0, py, px, MON_CHEN, PM_EPHEMERA))
			{
				msg_print("橙を呼び出した。");
			}
			else
				msg_print("式の召喚に失敗した。");

			break;

		}
	case 15:
		{
			int time = 10+plev/5;
			if(only_info) return format("期間:%d",time);

			if(p_ptr->tim_general[2])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("あなたは境界を操作した。フロアの気配が変わった・・");
			set_tim_general(time,FALSE,2,0);
		}
		break;
	case 16:
		{
			int dx,dy;
			bool flag_ok = TRUE;
			if(only_info) return format("");

			if(p_ptr->inside_quest
				|| quest[QUEST_OBERON].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_ANGBAND && dun_level == 100
				|| quest[QUEST_SERPENT].status == QUEST_STATUS_TAKEN && dungeon_type == DUNGEON_CHAOS && dun_level == 127)
			{
				msg_print("今は使えない。");
				return NULL;
			}

			for(i=1;i<10;i++)
			{
				dx = px + ddx[i];
				dy = py + ddy[i];
				if(!in_bounds(dy,dx) || !cave_clean_bold(dy,dx)) flag_ok = FALSE;
			}

			if(!flag_ok)
			{

				msg_print("ここでは使えない。");
				return NULL;
			}
			if(use_itemcard)
				msg_print("足元に探索拠点への入り口が開いた！");
			else
				msg_print("あなたは足元にスキマを開いた！");
			cave_set_feat(py, px, f_tag_to_index_in_init("HOME"));

		}
		break;

	case 17:
		{
			int power = 200 + plev * 4;
			if(only_info) return format("効力:%d",power);
			mass_genocide(power, TRUE);
		}
		break;
	case 18:
		{
			int y,x,feat;
			feature_type *f_ptr;
			if(only_info) return format("");

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print("ここでは使えない。");
				return NULL;
			}

			msg_print("あなたはダンジョンの境界を操作した。");
			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					if (cave_clean_bold(y,x))
						cave_set_feat(y, x, feat_granite);
					else if(cave_have_flag_bold(y,x,FF_WALL) && !cave_have_flag_bold(y,x,FF_PERMANENT))
						cave_set_feat(y, x, feat_floor);
				}
			}

			p_ptr->redraw |= (PR_MAP);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();

		}
		break;
	case 19:
		{
			int base = 30;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("あなたはフロアの全てを計測して演算した！");
			set_tim_addstat(A_INT,120,time, FALSE);
			set_tim_addstat(A_WIS,120,time, FALSE);
			wiz_lite(FALSE);
			set_foresight(time, FALSE);
			set_radar_sense(time, FALSE);

			break;
		}








	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}







/*:::マミゾウ専用技*/
/*:::「二ッ岩家の裁き」にmagic_num1[0]とtim_general[0]を使用する*/
class_power_type class_power_mamizou[] =
{

	{5,5,25,FALSE,TRUE,A_WIS,0,3,"弾幕変化",
		"ターゲット周辺に向けて無属性のボルトを数発放つ。"},
	{12,16,30,FALSE,FALSE,A_INT,0,0,"看破",
		"卓越した洞察力で周囲のモンスターの能力や耐性などを知る。"},
	{20,24,35,FALSE,FALSE,A_WIS,0,0,"鳥獣琵琶法師",
		"動物系のモンスターを配下として召喚する。この召喚で出てきたモンスターは維持コストが低いが妖魔本に捕らえられずフロア移動で消える。"},
	{24,30,45,FALSE,TRUE,A_INT,0,0,"変化",
		"近くの指定したモンスターに一定時間変身する。変身中はリボン・アミュレット以外の装備が無効化され、変身したモンスターの能力を'U'コマンドで使うことができる。変身の成功率はレベル・知能・賢さ・対象モンスターのレベルで変わる。ユニークモンスターには少し変身しにくい。"},

	//v1.1.36 追加
	{28,60,65,FALSE,TRUE,A_INT,0,0,"太陽変化",
		"自分を中心に巨大な核熱属性のボールを発生させる。満月(15+30n日目)の晩に使うと威力が増加する。"},

	{32,32,50,FALSE,TRUE,A_CHR,0,10,"分福熱湯風呂",
		"「蒸気」属性のブレスを放つ。威力は魅力に大きく影響を受ける。火炎耐性か水耐性をもつ敵にはやや効きづらい。地形や床上のアイテムにほぼ影響を及ぼさない。"},
	{36,45,60,FALSE,TRUE,A_CHR,0,0,"百鬼妖界の門",
		"配下の狸達を召喚する。"},

	{40,50,70,FALSE,TRUE,A_INT,0,0,"二ッ岩家の裁き",
		"敵一体を短時間変身させ弱体化させる。変身させられた敵は隣接攻撃と特殊攻撃が使用不可能になる。フロアを移動したり別の敵に技をかけると解除される。高レベルな敵やユニークモンスターには効きづらく、カオス耐性を持つ敵には効かない。"},
	{45,120,80,FALSE,TRUE,A_CHR,0,0,"満月のポンポコリン",
		"視界内の全てに対し暗黒・轟音攻撃を仕掛ける。満月(15+30n日目)の晩に使うと威力が増加する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_mamizou(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{
	case 0:
		{
			int num = 3 + plev / 10;
			int dice = 3 + p_ptr->lev / 12;
			int sides = 5 + chr_adj / 8;

			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("ばら撒いた木の葉が鳥や蛙の形をとって飛んだ。");
			fire_blast(GF_MISSILE, dir, dice, sides, num, 2,(PROJECT_STOP | PROJECT_KILL | PROJECT_GRID));
		}
		break;
	case 1:
		{
			if(only_info) return format("");
			msg_print("あなたは辺りを睨めつけた…");
			probing();
			break;
		}
	case 2:
		{
			int level = 5 + plev / 2;
			int num = 2 + plev / 12;
			bool flag = FALSE;
			if(only_info) return format("召喚レベル:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_ANIMAL, (PM_EPHEMERA))) flag = TRUE;
			}
			if(flag) msg_format("鳥獣の幻が踊った。");
			else msg_format("何も現れなかった・・");

		}
		break;

	case 3:
		{
			int x, y;
			int dist = 1 + plev/16;
			int time = 50+plev;
			monster_type *m_ptr;
			char m_name[80];


			if(only_info) return format("射程:%d 効果時間:%dターン",dist,time);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("近くのターゲットを明示的に指定しないといけない。");
				project_length = 0; //v1.1.76 変身後の特技の射程が短い不具合対応　ここに必要あるのかは不明
				return NULL;
			}

			project_length = 0; //v1.1.76 変身後の特技の射程が短い不具合対応

			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				int r_idx = m_ptr->r_idx;
				int chance;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				chance = plev + p_ptr->stat_ind[A_INT]+3+p_ptr->stat_ind[A_WIS]+3 - r_ptr->level;
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance -= 20;
				if(r_ptr->flagsr & RFR_RES_ALL) chance = 0;

				msg_format("あなたは%sに化けようと試みた・・",m_name);
				if(p_ptr->wizard) msg_format("(chance:%d)",chance);
				if(randint0(100) < chance)
				{
					metamorphose_to_monster(r_idx,time);
				}
				else if(chance < 1)
					msg_format("%sにはとても化けられそうにない。",m_name);
				else
					msg_print("変化に失敗した！");

			}
		}
		break;


	case 4:
		{
			int rad = 3;
			int base = plev * 10 + chr_adj * 10;

			if(FULL_MOON)
			{
				rad *= 2;
				base *= 2;
			}

			if(only_info) return format("損傷:～%d",base / 2);
			msg_format("あなたは太陽に変身した！");
			project(0, rad, py, px, base, GF_NUKE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
		break;


	case 5:
		{
			int dam = 50 + plev * 2 + chr_adj * 10;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print("巨大な茶釜から蒸気が吐き出された！");

			fire_ball(GF_STEAM, dir, dam, -2);
			break;
		}

	case 6:
		{
			int level = plev / 2 + chr_adj;
			int num = 1 + chr_adj / 4;
			bool flag = FALSE;
			if(only_info) return format("召喚レベル:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_TANUKI, (PM_FORCE_PET))) flag = TRUE;
			}
			if(flag) msg_format("配下の狸を呼び出した！");
			else msg_format("誰も現れなかった・・");

		}
		break;

	case 7://二ッ岩家の裁き
		{
			int y, x;
			monster_type *m_ptr;
			int time = 3;

			if(only_info) return format("期間:%d+1d%d",time,time);

			if(p_ptr->tim_general[0])
			{
				msg_print("すでに術を実行中だ。");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];
					int chance = plev + (p_ptr->stat_ind[A_INT]+3) * 2;
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					msg_format("あなたは%sに煙を吹き付けた。",m_name);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance = chance*2/3;

					if(r_ptr->flagsr & (RFR_RES_CHAO | RFR_RES_ALL))
					{
						msg_format("%sには完全な耐性がある！",m_name);
					}
					else if(p_ptr->magic_num1[0] == cave[y][x].m_idx)
					{
						msg_format("既に効いている。");

					}
					else if( chance < r_ptr->level)
					{
						msg_format("%sには効きそうにない。",m_name);
					}
					else if( randint1(chance) < r_ptr->level)
					{
						msg_format("%sには効かなかった。",m_name);
					}
					else
					{
						p_ptr->magic_num1[0] = cave[y][x].m_idx;
						set_tim_general(time+randint1(time),FALSE,0,0);
						lite_spot(m_ptr->fy,m_ptr->fx);
						msg_format("%sはコミカルな姿に変身した！",m_name);
						(void)set_monster_csleep(cave[y][x].m_idx, 0);
						anger_monster(m_ptr);
					}
				}
				else
				{
					msg_format("そこには何もいない。");
					return NULL;
				}

			break;
		}
	case 8:
		{
			int dice = plev / 2;
			int sides = 5 + chr_adj / 2;

			if(FULL_MOON) sides *= 2;
			if(only_info) return format("損傷:%dd%d*2",dice,sides);
			msg_print("壮大な狸囃子が始まった！");
			project_hack2(GF_DARK,dice,sides,0);
			project_hack2(GF_SOUND,dice,sides,0);

		}
		break;


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::薬師専用技*/
class_power_type class_power_chemist[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"薬箱収納",
		"アイテムを薬箱に入れる。薬や素材系アイテムしか入れられない。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"薬箱確認",
		"薬箱の中を確認する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"薬箱取出し",
		"薬箱からアイテムを出す。"},
	{5,5,20,FALSE,FALSE,A_INT,0,0,"応急手当",
		"HPを少し回復し毒と切り傷を治療する。レベルが上がると朦朧と幻覚も治療する。"},
	{10,4,35,FALSE,FALSE,A_INT,0,0,"成分分析",
		"キノコや薬を鑑定する。"},
	{16,6,40,FALSE,FALSE,A_INT,0,0,"食料生成",
		"食料を調達する。"},
	{ 24,18,70,FALSE,FALSE,A_INT,0,0,"植生調査",
		"フロアにあるキノコや素材系アイテムの存在を大まかに感知する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_chemist(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //見るだけなので行動順消費なし

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			int dice = 4 + plev / 4;
			int sides = 7;
			if(only_info) return format("回復:%dd%d",dice,sides);
			set_cut(0);
			if(plev > 9)set_stun(0);
			set_poisoned(0);
			if(plev > 19)set_image(0);
			hp_player(damroll(dice,sides));
			break;
		}
	case 4: //成分分析
		{
			if (only_info) return format("");
			if (!ident_spell_2(2)) return NULL;
		}
		break;

	case 5:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "食料を生成";
			msg_print("食べられる野草などを集めた。");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}

	//高級品感知
	case 6:
		{
			if(only_info) return format("");
			msg_print("あなたは周囲の調査を始めた・・");
			search_specific_object(6);
			break;
		}




	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::雷鼓用特技*/
class_power_type class_power_raiko[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"演奏を止める",
		"今演奏中ならそれを止める。行動力を消費しない。"},
	{7,6,20,FALSE,TRUE,A_STR,0,2,"暴れ宮太鼓",
		"轟音属性のボールを放つ。"},
	{12,6,25,FALSE,TRUE,A_INT,0,0,"怨霊アヤノツヅミ",
		"演奏中は周囲のモンスターを感知する。レベルが上がると感知できるものが増える。"},
	{16,12,30,FALSE,TRUE,A_STR,0,0,"ランドパーカス",
		"地震を起こしてダンジョンを崩し、一時的に士気高揚を得る。"},
	{20,25,45,FALSE,TRUE,A_WIS,0,7,"雷神の怒り",
		"自分を中心に電撃属性の強力なボールを発生させる。演奏中の音楽があれば止まる。"},
	{25,0,0,FALSE,TRUE,A_DEX,0,0,"ブルーレディーショー",
		"装備中の武器に応じた特殊能力を使用する。ある程度使い込んだ武器でないと能力を発揮できない。固定アーティファクトはこの能力の対象にならない。演奏中の音楽があれば止まる。"},
	{30,30,55,TRUE,FALSE,A_STR,0,10,"オルタネイトスティッキング",
		"自分を中心に轟音属性の強力なボールを発生させる。ただし壁に隣接していないと使えない。演奏中の音楽があれば止まる。"},
	{35,32,60,FALSE,TRUE,A_CHR,0,0,"怒りのデンデン太鼓",
		"演奏中、視界内のランダムな敵へ電撃か轟音のビームを放ち続ける。"},
	{40,40,60,FALSE,TRUE,A_CON,0,10,"高速和太鼓ロケット",
		"破片属性のロケットを放つ。威力は現在HPの1/3になる。"},
	{43,88,70,FALSE,TRUE,A_STR,0,0,"ファンタジックウーファー",
		"視界内の全てに対し強力な轟音属性攻撃を行う。演奏中の音楽があれば止まる。"},
	{47,48,70,FALSE,TRUE,A_CHR,0,0,"プリスティンビート",
		"演奏中は加速と士気高揚を得て通常攻撃の消費行動時間が減少する。連続で攻撃を行えば行うほど減少幅が大きくなり、最高四倍速で攻撃する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::雷鼓特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
//p_ptr->magic_num1[10～29]が特殊魔法に使われる？
cptr do_cmd_class_power_aux_raiko(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_raiko_music();
			return NULL;

		}
		break;
	case 1:
		{
			int dist = 2 + plev / 6;
			int rad = 2 + plev / 20;
			int dice = 4 + plev / 4;
			int sides = 7 + chr_adj / 2;

			if(only_info) return  format("射程:%d 損傷:%dd%d",dist,dice,sides);
			project_length = dist;
			stop_tsukumo_music();
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("太鼓を投げつけた！");
			fire_ball(GF_SOUND, dir, damroll(dice,sides),rad);

		}
		break;
	case 2://アヤノツヅミ
		{
			if(only_info) return format("感知継続");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_AYANOTUDUMI, SPELL_CAST);

			break;
		}
		break;
	case 3:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("大地を激しく踏み鳴らした！");
			earthquake(py,px,5);
			set_afraid(0);
			set_hero(time,FALSE);

			break;
		}
		//v1.1.85 追加で雷雲生成する
	case 4:
		{
			int rad = 3 + plev / 12;
			int base = plev * 12 + chr_adj * 10;
			if(only_info) return format("損傷:～%d",base / 2);
			stop_raiko_music();
			msg_format("雷雲が巻き起こり、雷鳴が轟いた！");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			project(0, rad/2, py, px, rad, GF_MAKE_STORM, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}
	case 5:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 6:
		{
			int rad = 4 + plev / 16;
			int base = plev * 8 + adj_general[p_ptr->stat_ind[A_STR]] * 8;
			if(only_info) return format("損傷:～%d",base / 2);
			stop_raiko_music();
			msg_format("あなたは壁に向かって思い切り体当りした！");
			project(0, rad, py, px, base, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
	case 7://怒りのデンデン太鼓
		{
			int base = plev * 3;
			int sides = chr_adj * 5;
			if(only_info) return format("損傷:%d+1d%d",base,sides);

			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_DENDEN, SPELL_CAST);

			break;
		}
		break;
	case 8:
		{
			int dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if (dam > 800) dam = 800;

			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("太鼓が火を噴いて飛んだ！");
			fire_rocket(GF_SHARDS, dir, dam,2);
			break;
		}
	case 9:
		{
			int dam = plev * 5 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			stop_raiko_music();
			msg_print("重低音がダンジョンを揺らした！");
			project_hack2(GF_SOUND,0,0,dam);
			break;
		}
		break;
	case 10://プリスティンビート
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_RAIKO_PRISTINE, SPELL_CAST);

			break;
		}
		break;

	default:
		if(only_info) return  format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::三月精専用技*/
//静寂1にtim_general[0]、静寂2にtim_general[1]を使う
class_power_type class_power_sangetsu[] =
{
	{8,6,20,FALSE,TRUE,A_STR,0,3,"アイスディゾルバー",
		"火炎属性のビームを放つ。"},
	{15,5,25,FALSE,TRUE,A_INT,0,0,"幻惑Ⅰ",
		"敵一体を混乱させる。賢い敵には効かない。夜間は効果が落ちる。"},
	{18,16,40,FALSE,TRUE,A_INT,0,0,"静寂",
		"一定時間、自分たちと近くのモンスターが魔法と一部の行動を使えなくなる。巻物を読むと効果が解除され少し消費行動時間が増える。"},
	{18,0,0,FALSE,TRUE,A_INT,0,0,"静寂解除",
		"特技「静寂」の効果を解除する。この行動によるターン消費は通常の半分。"},
	{ 20,10,40,FALSE,TRUE,A_INT,0,0,"ルチルフレクション",
		"光を屈折させて離れた場所を視認する。ただし閉じたドアやカーテンで遮られた所を見ることはできない。" },
	{23,20,45,FALSE,TRUE,A_WIS,0,5,"トリプルメテオ",
		"周囲のランダムな位置に隕石属性のボールを連続で発生させる。"},
	{27,20,45,FALSE,TRUE,A_DEX,0,0,"エルフィンキャノピー",
		"一定時間反射能力を得る。"},
	{30,20,0,FALSE,FALSE,A_DEX,0,0,"爆弾設置",
		"足元に爆弾を設置する。爆弾は「通常は」10ターンで爆発し半径3の破片属性の強力なボールを発生させる。この爆発ではプレイヤーもダメージを受ける。爆弾は一部の属性攻撃に晒されると誘爆したり爆発タイミングが変わることがある。"},
	{32,30,50,FALSE,TRUE,A_INT,0,0,"幻惑Ⅱ",
		"一定時間、周囲の敵を惑わせる。視界に頼らない敵に対しては効果が薄い。夜間は効果時間が短くなる。"},
	{34,24,60,FALSE,TRUE,A_CHR,0,8,"トリプルライト",
		"指定位置に閃光属性の巨大なボールを発生させる。"},
	{37,20,60,FALSE,TRUE,A_CON,0,0,"オリオンベルト",
		"一定時間士気高揚と身体能力上昇を得る。"},
	{40,33,75,FALSE,TRUE,A_INT,0,0,"ブレイクキャノピー",
		"ターゲットモンスターにかかっている魔法効果を解除する。"},
	{43,60,70,FALSE,TRUE,A_STR,0,0,"サンバースト",
		"視界内の全てに対し、閃光属性の攻撃を行う。サニーが日光を蓄積していると威力が強化される。"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"静寂Ⅱ",
		"一定時間、フロア全域のモンスターが魔法と一部の行動を使えなくなる。自分たちは制限を受けない。"},
	{48,99,80,FALSE,TRUE,A_CHR,0,0,"スリーフェアリーズ",
		"自分たちを中心とした閃光、暗黒、隕石属性の巨大なボールを連続で発生させる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_sangetsu(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 1 + plev / 4;
			int sides = 7;
			int base = plev/2 + chr_adj/2 ;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("熱線を放った。");
			fire_beam(GF_FIRE, dir, base + damroll(dice,sides));
			break;
		}
	case 1:
		{
			int x, y;
			int dist = 2 + plev / 5;
			monster_type *m_ptr;
			int power = 20 + plev;
			if(is_daytime()) power += 30;

			if(only_info) return format("射程:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("サニーは%sの周囲の景色を歪めた・・",m_name);

				if(r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF ||
					r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
				{
					msg_format("しかし効果がなかった。");
				}
				else
				{
					msg_format("%sは混乱した！",m_name);
					(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/3));
				}
				set_monster_csleep(cave[y][x].m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}

	case 2://静寂1 tim_general[0]を使う
		{
			int time = 10+plev/5;
			int rad = plev / 5;
			if(only_info) return format("期間:%d 範囲:%d",time,rad);
			if(p_ptr->tim_general[0] || p_ptr->tim_general[1])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("ルナは指を鳴らした。周囲の音が消えた。");
			set_tim_general(time,FALSE,0,0);

		}
		break;
	case 3://静寂解除
		{
			if(only_info) return format("");
			if(!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
			{
				msg_print("能力を使っていない。");
				return NULL;
			}

			msg_print("ルナは能力を解除した。");
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);

		}
		break;
	case 4:
		{
			//v1.1.34 サニーの「光を屈折させて離れた場所の光景を見る」能力を追加。
			//まず特殊フラグを立ててからflying_dist値を閉じたドアやカーテンに遮られるようにして再計算する。
			//次にこのflying_dist値が設定されたグリッドのみを啓蒙、モンスター感知する。
			//最後にフラグを戻してflying_dist値を再計算する。
			if (only_info) return format("範囲:%d移動グリッド", MONSTER_FLOW_DEPTH);

			if (p_ptr->blind)
			{
				msg_print("サニーは目が見えない。");
				return NULL;
			}

			msg_print("離れた場所の映像がサニーの目の前に広がった……");
			flag_sunny_refraction = TRUE;
			update_flying_dist();
			wiz_lite(FALSE);
			detect_monsters_normal(255);
			flag_sunny_refraction = FALSE;
			update_flying_dist();

		}
		break;

	case 5:
		{
			int rad = 2 + plev / 24;
			int damage = plev  + chr_adj / 2 ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("三人で弾幕を降らせた！");
			cast_meteor(damage, rad, GF_METEOR);
			break;
		}
	case 6:
		{
			int time;
			int base = 15 + p_ptr->lev/5;

			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("サニーは身構えた。");
			set_tim_reflect(time, FALSE);

			break;
		}

	case 7:
		{
			int dam = 100+plev*4;
			if(only_info) return format("損傷:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("ここには爆弾を置けない。");
				return NULL;
			}
			msg_print("スターは爆弾を仕掛けた。");
			/*:::Mega Hack - cave_type.specialを爆弾のカウントに使用する。*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;
			while(one_in_(5)) cave[py][px].special += randint1(20);
			note_spot(py, px);
			lite_spot(py, px);
		}
		break;
	case 8:
		{
			int base = 5+plev/5;
			if(is_daytime()) base *= 2;
			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_format("サニーは光を操って三人の姿を消した。");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			break;
		}
	case 9:
		{
			int dice = plev/2 ;
			int sides = 10 + chr_adj / 3;
			int rad = 5;

			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE,dir,damroll(dice,sides),rad,"三人で巨大な光球を作り出した！")) return NULL;

			break;
		}
	case 10:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("スターはサニーとルナを激励した！");
			set_hero(time,FALSE);
			set_tim_addstat(A_STR,4,time,FALSE);
			set_tim_addstat(A_CON,4,time,FALSE);
			set_tim_addstat(A_DEX,4,time,FALSE);

			break;
		}
	case 11:
		{

			int m_idx;

			if(only_info) return format("");
			if (!target_set(TARGET_KILL)) return NULL;
			m_idx = cave[target_row][target_col].m_idx;
			if (!m_idx) return NULL;
			if (!player_has_los_bold(target_row, target_col)) return NULL;
			if (!projectable(py, px, target_row, target_col)) return NULL;
			msg_print("ルナは指を鳴らした。");
			dispel_monster_status(m_idx);

			break;
		}
	case 12:
		{
			int dam = 40 + plev*3 + chr_adj*2;

			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) dam += 100;

			if(only_info) return format("損傷:%d",dam);
			msg_format("サニーたちは眩く光った！");
			project_hack2(GF_LITE,0,0,dam);
			lite_room(py,px);
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-500);
			break;
		}

	case 13://静寂2 tim_general[1]を使う
		{
			int time = 30;
			if(only_info) return format("期間:%d",time);
			if(p_ptr->tim_general[1])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("ルナはフロア全体の音に干渉した。");
			set_tim_general(time,FALSE,1,0);
			p_ptr->tim_general[0]=0;
		}
		break;
	case 14:
		{
			int dam = plev * 4 + chr_adj * 10;
			int rad = 6;
			int i;
			if(only_info) return format("損傷:最大%d*3",dam/2);
			msg_print("三人で全力の一撃を放った！");
			project(0, rad, py, px, dam, GF_LITE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if(p_ptr->chp>=0) project(0, rad, py, px, dam, GF_DARK, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			if(p_ptr->chp>=0) project(0, rad, py, px, dam, GF_METEOR, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;


	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::八橋用特技*/
class_power_type class_power_yatsuhashi[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"演奏を止める",
		"今演奏中ならそれを止める。行動力を消費しない。"},
	{5,4,20,FALSE,TRUE,A_WIS,0,2,"諸行無常の琴の音",
		"敵一体を混乱、減速させる。レベル30以降は視界内すべての敵に対して効果が発揮される。"},
	{15,8,30,FALSE,TRUE,A_CHR,0,0,"平安の残響",
		"演奏中には反射能力を得る。演奏中はMPを消費し続ける。"},
	{20,12,35,FALSE,TRUE,A_WIS,0,0,"天の詔琴",
		"演奏中に隣接攻撃してきた敵に対しカウンターで轟音属性の大ダメージを与える。演奏中はMPを消費し続ける。"},
	{27,24,45,FALSE,TRUE,A_CHR,0,0,"嵐のアンサンブル",
		"演奏中に視界内の全てに対し、電撃、轟音、水、竜巻のいずれかの属性の攻撃が行われる。演奏中はMPを消費し続ける。"},
	{30,0,0,FALSE,TRUE,A_WIS,0,0,"浄瑠璃世界",
		"十二回続けて演奏するとHPとMPが中程度回復しステータス異常が治癒する。"},
	{36,24,65,FALSE,FALSE,A_WIS,0,0,"人琴ともに亡ぶ",
		"演奏中にダメージを受けるとそのダメージを相手に返す。演奏中はMPを消費し続ける。死霊魔法「復讐の契約」とは別処理。"},
	{40,32,55,FALSE,FALSE,A_INT,0,0,"エコーチェンバー",
		"演奏中に自分の使う魔法が強化されレベル判定値が1.5倍される。演奏中はMPを消費し続ける。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::八橋特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_yatsuhashi(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_tsukumo_music();
			return NULL;

		}
		break;
	case 1:
		{
			int power = 50 + plev * 3;
			if(only_info) return format("効果:%d",power);

			if(plev < 30)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				msg_print("か細い琴の音が響いた。");
				slow_monster(dir,power);
				confuse_monster(dir,power);
			}
			else
			{
				msg_print("物悲しい琴の音が響き渡った・・");
				slow_monsters(power);
				confuse_monsters(power);
			}
			break;
		}
	case 2://
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_ECHO2, SPELL_CAST);
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= (PR_STATUS);
			break;
		}
	case 3://天の詔琴
		{
			int dice = 8 + plev / 4;
			int sides = 10 + chr_adj / 3;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_NORIGOTO, SPELL_CAST);
			break;
		}
	case 4://弦楽「嵐のアンサンブル」
		{
			int base = plev * 2 + chr_adj * 2;
			if(only_info) return format("損傷:1d%d継続",base);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_STORM, SPELL_CAST);

			break;
		}
		break;
	case 5://浄瑠璃世界 concentを使う
		{
			if(only_info) return format("");
			stop_tsukumo_music();
			do_cmd_concentrate(0);
			if(p_ptr->concent == 12)
			{
				msg_print("あなたたちは七色の光に包まれた！");
				hp_player(plev * 6);
				player_gain_mana(plev * 2);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				set_alcohol(0);
				set_asthma(0);

				reset_concentration(FALSE);
			}
			break;

		}
		break;


	case 6://人琴ともに亡ぶ
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_JINKIN, SPELL_CAST);
			break;
		}
	case 7://エコーチェンバー
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_ECHO, SPELL_CAST);
			break;
		}

	default:
		if(only_info) return  format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::弁々用特技*/
class_power_type class_power_benben[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"演奏を止める",
		"今演奏中ならそれを止める。行動力を消費しない。"},
	{5,3,20,FALSE,TRUE,A_WIS,0,2,"祇園精舎の鐘の音",
		"射程の短い轟音属性のボールを放つ。"},
	{12,9,30,FALSE,TRUE,A_CHR,0,0,"平家の大怨霊",
		"周囲のランダムな敵に対し呪いで攻撃する演奏を行う。演奏中はMPを消費し続ける。"},
	{20,15,35,FALSE,TRUE,A_WIS,0,3,"邪悪な五線譜",
		"閃光属性のビームを放つ。"},
	{27,24,45,FALSE,TRUE,A_CHR,0,0,"嵐のアンサンブル",
		"演奏中に視界内の全てに対し、電撃、轟音、水、竜巻のいずれかの属性の攻撃が行われる。演奏中はMPを消費し続ける。"},
	{30,0,0,FALSE,TRUE,A_WIS,0,0,"浄瑠璃世界",
		"十二回続けて演奏するとHPとMPが中程度回復しステータス異常が治癒する。"},
	{34,25,55,FALSE,FALSE,A_CON,0,7,"大熱唱琵琶",
		"自分を中心とする巨大な轟音属性のボールを発生させる。周囲の敵が起きる。"},
	{40,36,65,FALSE,FALSE,A_CHR,0,0,"スコアウェブ",
		"演奏中、視界内のランダムな敵に対し閃光属性ビームを数発放つ。演奏中はMPを消費し続ける。"},
	{44,48,70,FALSE,TRUE,A_CHR,0,0,"ダブルスコア",
		"演奏中は格闘攻撃が強化される。攻撃回数や命中率にボーナスがつき、恐怖状態でも攻撃でき、オーラダメージを受けない。また敵からの隣接攻撃に対しカウンター攻撃を行う。この格闘攻撃のみ隣接攻撃をしても演奏が途切れない。演奏中はMPを消費し続ける。武器装備中には使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::弁々特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_benben(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_tsukumo_music();
			return NULL;

		}
		break;
	case 1:
		{
			int dist = 2 + plev / 16;
			int rad = 2 + plev / 20;
			int dice = 4 + plev / 6;
			int sides = 6 + chr_adj / 5;

			if(only_info) return  format("射程:%d 損傷:%dd%d",dist,dice,sides);
			project_length = dist;
			stop_tsukumo_music();
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("重厚な音が響いた・・");
			fire_ball(GF_SOUND, dir, damroll(dice,sides),rad);

		}
		break;
	case 2://平家の大怨霊
		{
			int dice = 7 + plev / 6;
			if(only_info) return format("損傷:%dd%d継続",dice,dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_HEIKE, SPELL_CAST);

			break;
		}
	case 3:
		{
			int dice = 2 + plev / 3;
			int sides = 8;
			int base = plev + chr_adj * 2;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			stop_tsukumo_music();
			msg_format("五条の光線を放った。");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}
	case 4://弦楽「嵐のアンサンブル」
		{
			int base = plev * 2 + chr_adj * 2;
			if(only_info) return format("損傷:1d%d継続",base);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_STORM, SPELL_CAST);

			break;
		}
		break;
	case 5://浄瑠璃世界 concentを使う
		{
			if(only_info) return format("");
			stop_tsukumo_music();
			do_cmd_concentrate(0);
			if(p_ptr->concent == 12)
			{
				msg_print("あなたたちは七色の光に包まれた！");
				hp_player(plev * 6);
				player_gain_mana(plev * 2);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				set_alcohol(0);
				set_asthma(0);

				reset_concentration(FALSE);
			}
			break;

		}
	case 6:
		{
			int dam = 300 + plev * 4 + chr_adj * 6;
			int rad = 4 + plev / 12;
			int i;
			if(only_info) return format("半径:%d 損傷:最大%d",rad,dam/2);
			msg_print("爆音が大地を震わせた！");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			aggravate_monsters(0,FALSE);
		}
		break;
	case 7://スコアウェブ
		{
			int dam = plev/2 + chr_adj / 2;
			int num = 5;
			if(only_info) return format("損傷:%d*%d継続",dam,num);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_SCOREWEB, SPELL_CAST);

			break;
		}
	case 8://ダブルスコア
		{
			if(only_info) return format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_TSUKUMO_DOUBLESCORE, SPELL_CAST);

			break;
		}

	default:
		if(only_info) return  format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::咲夜専用技*/
class_power_type class_power_sakuya[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"ナイフ収納",
		"ナイフホルダーへ短剣系アイテムを一本収納する。収納済みのスロットを選択することで装備中の武器と入れ替えることもできる。行動時間をほとんど消費しない。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"ナイフ取出し",
		"ナイフホルダーから一本取り出す。行動時間をほとんど消費しない。"},
	{1,0,20,FALSE,FALSE,A_DEX,0,0,"ナイフ回収",
		"足元に落ちている短剣系アイテムを適当に拾えるだけ拾ってナイフホルダーに収納する。「咲夜の世界」実行中は近くのナイフも拾う。ナイフホルダーの容量はレベルアップで増える。"},
	{5,3,15,FALSE,FALSE,A_DEX,0,0,"ナイフ投げⅠ",
		"どこかに隠し持っていたナイフを投げる。このナイフは投げると必ず消える。通常の投擲と同様に投擲熟練度で消費時間が減る。「咲夜の世界」実行中なら時間停止を解除する。"},

	{8,6,20,FALSE,TRUE,A_INT,0,0,"バニシングエブリシング",
		"中距離のランダムな位置へテレポートする。"},
	{12,16,30,FALSE,TRUE,A_WIS,0,0,"プライベートスクウェア",
		"一定時間加速する。"},
	{16,32,40,FALSE,FALSE,A_CHR,0,0,"咲夜の世界",
		"時間を停止する。この時間停止に期限はないが、時間停止中には敵へ攻撃したり道具などを使うことができない。射撃や投擲を行うと時間停止が解除される。また休憩コマンドでHPやステータス異常が回復するがMPは回復しない。"},
	{16,0,0,FALSE,FALSE,A_CHR,0,0,"咲夜の世界(解除)",
		"時間停止を解除する。行動時間をほとんど消費しない。"},

	{20,24,35,FALSE,TRUE,A_DEX,0,7,"殺人ドール",
		"周囲のランダムな敵に対し大量の破片属性ボルトを放つ。「咲夜の世界」実行中なら時間停止を解除する。"},
	{23,12,45,FALSE,TRUE,A_DEX,0,0,"チェンジリングマジック",
		"指定した敵と位置を交換する。テレポートが正常に働かない場所では失敗する。"},
	{27,32,55,FALSE,TRUE,A_DEX,0,0,"パーフェクトメイド",
		"実行後、一度だけ敵からの攻撃を回避して短距離をテレポートしさらに元居た場所周辺に破片属性攻撃を行う。テレポートや回避に失敗することがある。"},
	{30,50,60,FALSE,FALSE,A_DEX,0,0,"ナイフ投げⅡ",
		"ナイフホルダーに入っている短剣類を全て一度に投擲する。「咲夜の世界」実行中なら時間停止を解除する。投げたら戻ってくる能力を持つ武器は戻ってこない。投擲の指輪の効果は半減する。"},

	{32,40,85,FALSE,TRUE,A_INT,0,0,"トンネルエフェクト",
		"指定方向の壁など移動不可地形の向こうへ瞬間移動する。マップ端に到達すると反対側へ出る。通常のテレポートで移動できない場所には出られない。"},

	{35,44,65,FALSE,TRUE,A_CHR,0,0,"銀色のアナザーディメンジョン",
		"一定時間反射とAC上昇を得る。"},
	{38,128,70,FALSE,FALSE,A_STR,0,0,"ソウルスカルプチュア",
		"視界内の敵全てに通常攻撃を仕掛ける。この攻撃では敵からのオーラダメージを受けない。敵からの距離によって攻撃回数が増減する。短剣類を装備していないとこの技は使えない。「咲夜の世界」実行中なら時間停止を解除する。"},
	{41,64,75,FALSE,TRUE,A_INT,0,0,"インフレーションスクウェア",
		"視界内の全てに対し空間歪曲属性の攻撃を仕掛ける。「咲夜の世界」実行中なら時間停止を解除する。"},
	{43,64,75,FALSE,TRUE,A_CHR,0,0,"デフレーションワールド",
		"極めて強力な投擲を行う。投擲できるのは武器のみで使った武器は必ず消滅する。「咲夜の世界」実行中なら時間停止を解除する。"},
	{46,255,80,FALSE,TRUE,A_WIS,0,0,"ザ・ワールド",
		"時を止める。このときは敵への攻撃ができるが効果時間は極めて短い。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_sakuya(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int inven2_num = calc_inven2_num();

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("最大:%d個",inven2_num);
			put_item_into_inven2();
			new_class_power_change_energy_need = 20;
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			new_class_power_change_energy_need = 20;
			break;
		}

	case 2:
		{
			int j;
			bool flag_pick = FALSE;
			bool flag_max = TRUE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(!inven_add[i].k_idx)flag_max = FALSE;

			if(flag_max)
			{
				msg_print("これ以上持てない。");
				return NULL;
			}

			//フロアのアイテムを全て調べる
			for(i=0;i<inven2_num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				if(inven_add[i].k_idx) continue;
				flag_max = FALSE;
				//視界内のアイテムをランダムに選定
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (o_ptr->tval != TV_KNIFE) continue;

					if(SAKUYA_WORLD)
					{
						if(distance(py,px,o_ptr->iy,o_ptr->ix) > 2) continue;
						if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;
					}
					else
					{
						if(py != o_ptr->iy || px != o_ptr->ix) continue;
					}

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				//選ばれたアイテムを追加インベントリに1つだけ格納し床上アイテムを減らす。所持品重量は考慮しない。
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				q_ptr->number = 1;
				distribute_charges(o_ptr, q_ptr, 1);
				object_desc(o_name, q_ptr, 0);
				msg_format("%sを拾ってナイフホルダーに差した。",o_name);
				p_ptr->total_weight += q_ptr->weight * q_ptr->number;
				object_wipe(&inven_add[i]);
				object_copy(&inven_add[i], q_ptr);
				floor_item_increase(item, -1);
				floor_item_describe(item);
				floor_item_optimize(item);

			}
			if(!flag_pick)
			{
				msg_print("近くにナイフは落ちていない。");
				break;
			}


		}
		break;

	case 3:
		{
			int tx,ty;
			int quality = plev / 5;
			object_type forge;
			object_type *q_ptr = &forge;
			if(only_info) return format("性能:1d3(+%d,+%d)",quality,quality);

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
			object_known(q_ptr);
			msg_print("ナイフを投げた。");
			do_cmd_throw_aux2(py,px,ty,tx,1,q_ptr,1);
			if(ref_skill_exp(SKILL_THROWING) > 1600)
			{
				new_class_power_change_energy_need = 100 - (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
				if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need -= new_class_power_change_energy_need / 5;
			}

			break;
		}

	case 4:
		{
			int dist = 15 + plev / 2;
			if(only_info) return format("距離:%d",dist);
			msg_print("あなたは瞬時に消えた。");
			teleport_player(dist, 0L);
		}
		break;

	case 5:
		{
			int time = 20 + plev / 5;
			if(only_info) return format("期間:%d",time);
			msg_print("周囲の時間に干渉した・・");
			set_fast(time, FALSE);

			break;
		}

	case 6:
		{
			if(only_info) return format("");

			if(p_ptr->riding)
			{
				msg_print("騎乗中は実行できない。");
				return NULL;
			}
			if (world_player)
			{
				msg_print("既に時は止まっている。");
				return NULL;
			}
			sakuya_time_stop(TRUE);
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			sakuya_time_stop(FALSE);
			new_class_power_change_energy_need = 20;
			break;
		}


	case 8:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 9;
			int sides = 3 + chr_adj/ 12;
			int num = 9 + plev / 3;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("大量のナイフが飛んだ！");
			sakuya_time_stop(FALSE);
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_SHARDS, damroll(dice,sides),1, 0,0)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}

		}
		break;
	case 9:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
	case 10:
		{
			int dam = 50 + plev * 3;
			if(only_info) return format("損傷:～%d",dam/2);
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("あなたは周囲に気を配り始めた・・");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 11:
		{
			int ty,tx;
			int thrown_num = 0;
			object_type *o_ptr;
			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(inven_add[i].k_idx) thrown_num++;
			if(!thrown_num)
			{
				msg_print("ナイフホルダーに武器が差さっていない。");
				return NULL;
			}

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

			if(thrown_num > 1)
				msg_print("両手にナイフを掴んで一斉に投げた！");
			else
				msg_print("ナイフを投げた。");

			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				o_ptr = &inven_add[i];

				if (do_cmd_throw_aux2(py, px, ty, tx, 1, o_ptr, 0))
				{
					//v1.1.46 重量減少処理が抜けていたので追加
					p_ptr->total_weight -= inven_add[i].weight * inven_add[i].number;
					object_wipe(&inven_add[i]);
				}
			}
		}
		break;

	case 12:
		{
			int x,y;
			int attempt = 500;
			if(only_info) return format("");

			if (p_ptr->anti_tele)
			{
				msg_format("何かがテレポートを阻害している。");
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
			{
				msg_format("そこには壁がない。");
				return NULL;
			}
			while(1)
			{
				attempt--;
				y += ddy[dir];
				x += ddx[dir];

				if(y >= cur_hgt) y = 1;
				if(y < 1) y = cur_hgt - 1;
				if(x >= cur_wid) x = 1;
				if(x < 1) x = cur_wid - 1;
				if(attempt<0)
				{
					msg_format("トンネル効果の発動に失敗した！");
					teleport_player(200,0L);
					break;
				}
				if(!cave_player_teleportable_bold(y, x, 0L)) continue;
				msg_format("あなたはポテンシャル障壁を突き抜けた！");
				teleport_player_to(y,x,0L);
				break;
			}
			break;
		}
	case 13:
		{
			int time;
			int base = p_ptr->lev/2-5;

			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_format("銀色の光があなたを包んだ・・");
			set_tim_reflect(time, FALSE);
			set_shield(time, FALSE);
			break;
		}
	case 14:
		{
			if(only_info) return format("");
			sakuya_time_stop(FALSE);
			flag_friendly_attack = TRUE;
			msg_format("あなたの目が赤く光った・・");
			project_hack2(GF_SOULSCULPTURE,0,0,100);
			flag_friendly_attack = FALSE;
		}
		break;
	case 15:
		{
			int base = plev * 5;
			int sides = 100 + chr_adj * 5;
			if (only_info) return format("効力:%d+1d%d",base,sides);
			sakuya_time_stop(FALSE);
			msg_format("瞬間的に周囲の空間を拡大させた！");
			project_hack2(GF_DISTORTION,1,sides,base);
		}
	break;
	case 16:
		{
			int     item;
			cptr    q, s;
			object_type *o_ptr;
			object_type forge;
			object_type *q_ptr = &forge;
			int ty,tx;
			int mult = 16;

			if(only_info) return format("倍率:%d",mult);

			item_tester_hook = object_is_melee_weapon;

			q = "どの武器を投げますか? ";
			s = "武器を持っていない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

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
			object_copy(q_ptr,o_ptr);
			q_ptr->number = 1;
			if(do_cmd_throw_aux2(py,px,ty,tx,mult,q_ptr,3))
			{
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_optimize(0 - item);
				}
			}
			break;
		}
	case 17:
		{
			if(only_info) return format("効力:500");
			if (world_player)
			{
				msg_print("既に時は止まっている。");
				return NULL;
			}
			sakuya_time_stop(FALSE);
			world_player = TRUE;
			msg_print("「あなたの時間も私のもの…」");
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			p_ptr->energy_need -= 1500;
			p_ptr->redraw |= (PR_MAP);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			handle_stuff();
			msg_print("時が止まった！");
			break;
		}


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::ブロークンアミュレット用*/
static bool item_tester_hook_broken_amulet(object_type *o_ptr)
{
	if(o_ptr->tval != TV_AMULET) return FALSE;

	if(object_is_cursed(o_ptr) && (object_is_known(o_ptr) || (o_ptr->ident & IDENT_SENSE))) return TRUE;

	return FALSE;
}

/*:::雛特技*/
class_power_type class_power_hina[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"厄を確認する",
		"今どれくらい厄が溜まっていてどのような効果が発生しているかを確認する。"},
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"厄神様のバイオリズム",
		"厄を栄養として消費し空腹を満たす。厄500の消費で飢餓状態から満腹になる。"},
	{1,0,20,FALSE,TRUE,A_WIS,0,0,"厄を吸い取る",
		"装備中の呪われた品の呪いを解き、厄として吸収する。"},
	{7,7,25,FALSE,TRUE,A_WIS,0,0,"バッドフォーチュン",
		"ターゲット一体に厄を飛ばしてダメージを与える。抵抗されると無効。ダメージを与えたとき1/10の厄を消費する。"},
	{15,5,40,FALSE,TRUE,A_INT,0,0,"厄感知",
		"周囲のトラップを感知し呪われたアイテムがフロアにあれば大まかに感知する。レベル30以降は混沌勢力のモンスターも感知する。"},
	{23,12,30,FALSE,TRUE,A_DEX,0,0,"ブロークンアミュレット",
		"呪われたアミュレットをひとつ投げつけて破片属性ボールを発生させる。威力は呪いの強さで変わる。投げたアミュレットは必ず消滅する。"},
	{30,28,50,FALSE,TRUE,A_WIS,0,0,"ミスフォーチュンズホイール",
		"自分中心の巨大な厄のボールを発生させる。抵抗されると無効。ダメージを与えたとき1/10の厄を消費する。"},
	{35,22,60,FALSE,TRUE,A_CON,0,0,"ペインフロー",
		"一定時間、ダメージを受けた時に同じダメージを相手に返すようになる。与えたダメージの1/5の厄を消費する。"},
	{40,24,70,FALSE,TRUE,A_CON,0,0,"流刑人形",
		"敵一体の移動とテレポートを一定時間妨害する。二体以上同時に妨害することはできない。妨害のたびに厄を少し消費する。巨大な敵や力強い敵には効果が薄い。"},
	{47,66,70,FALSE,TRUE,A_CHR,0,0,"呪いの雛人形",
		"厄を撒き散らし、視界内のモンスターの魔法を中確率で妨害する。毎ターン厄を50+1d100消費する。もう一度実行するかフロアを移動すると技が解除される。地上では使えない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::雛特技 p_ptr->magic_num1[0]が厄値として使われる*/
//magic_num1[1]を雛人形無人販売所の稼ぎとして使う
//magic_num1[2]を流刑人形のIDXとして使う
cptr do_cmd_class_power_aux_hina(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int yaku = p_ptr->magic_num1[0];
	int yaku_max = HINA_YAKU_MAX;

	switch(num)
	{
	case 0:
		{
			int i,x,y;
			if(only_info) return format("厄:%d",yaku);
			screen_save();

			for(i=1;i<20;i++) Term_erase(16, i, 255);
			y = 1;
			x = 18;

			if(yaku > HINA_YAKU_LIMIT3)
			{
				c_put_str(TERM_RED,format("現在の厄:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_RED,format("これ以上の厄を扱いきれない！このままでは大変なことになる！"), y++, x);
			}
			else if(yaku > HINA_YAKU_LIMIT2)
			{
				c_put_str(TERM_ORANGE,format("現在の厄:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_ORANGE,format("厄が漏れ出し、周囲に溢れ出ている…"), y++, x);
			}
			else if(yaku > HINA_YAKU_LIMIT1)
			{
				c_put_str(TERM_YELLOW,format("現在の厄:%d/%d",yaku,yaku_max), y++, x);
				c_put_str(TERM_YELLOW,format("あなたは厄を少し持て余し気味だ。"), y++, x);
			}
			else
				c_put_str(TERM_WHITE,format("現在の厄:%d/%d",yaku,yaku_max), y++, x);
			y++;

			if(yaku > HINA_YAKU_LIMIT3)
			{
				c_put_str(TERM_ORANGE,format("厄の負担が体を蝕んでいる。"), y++, x);
				c_put_str(TERM_WHITE,format("周囲のものに無差別に災いが降りかかっている。"), y++, x);
				c_put_str(TERM_ORANGE,format("今のあなたの状態は人里で「厄神異変」と呼ばれているらしい。"), y++, x);
			}

			if(yaku > HINA_YAKU_LIMIT2+5000)
			{
				c_put_str(TERM_ORANGE,format("あなたは建物に入れてもらえない。"), y++, x);
			}

			if(yaku > HINA_YAKU_LIMIT2)
				c_put_str(TERM_YELLOW,format("あなたは反感を受けている。"), y++, x);
			else if(yaku > HINA_YAKU_LIMIT1 && yaku >= 1500)
				c_put_str(TERM_WHITE,format("あなたは隠密能力が低下している。(-%d)",yaku/1500), y++, x);

			if(yaku > 9999)
			{
				c_put_str(TERM_L_GREEN,format("あなたは暗黒・地獄攻撃を完全に無効化する。"), y++, x);
			}
			if(yaku > 14999)
			{
				c_put_str(TERM_L_GREEN,format("あなたは異界の悍ましい狂気への耐性をもつ。"), y++, x);
			}
			if(yaku > 19999)
			{
				c_put_str(TERM_L_GREEN,format("あなたは敵から受けたボルト魔法を跳ね返す。"), y++, x);
			}

			if(p_ptr->do_martialarts)
			{
				if(yaku > 443) c_put_str(TERM_WHITE,format("あなたは隣接攻撃に%dのダイスボーナスと(+%d,+%d)の修正を得る。",yaku/999,yaku/444,yaku/666), y++, x);
			}
			else
			{
				if(yaku > 1200) c_put_str(TERM_WHITE,format("あなたは隣接攻撃に%dのダイスボーナスと(+%d,+%d)の修正を得る。",yaku/4800,yaku/1200,yaku/1800), y++, x);
			}

			if(yaku > 200) c_put_str(TERM_WHITE,format("あなたはACに%dの修正を得る。",yaku/200), y++, x);

			if(yaku > 99) c_put_str(TERM_WHITE,format("あなたに触れた者は1d%dのダメージを受ける。",yaku/100), y++, x);
			if(yaku > 10000) c_put_str(TERM_L_GREEN,format("あなたに触れた者は混乱することがある。"), y++, x);
			if(yaku > 20000) c_put_str(TERM_L_GREEN,format("あなたに触れた者は朦朧とすることがある。"), y++, x);

			if(yaku > 27000) c_put_str(TERM_L_BLUE,format("あなたは秩序の勢力の天敵だ。"), y++, x);
			else if(yaku > 18000) c_put_str(TERM_L_GREEN,format("あなたは秩序の勢力に対して特に恐るべき力を発揮する。"), y++, x);
			if(yaku > 18000) c_put_str(TERM_L_BLUE,format("あなたは人間の天敵だ。"), y++, x);
			else if(yaku > 12000) c_put_str(TERM_L_GREEN,format("あなたは人間に対して特に恐るべき力を発揮する。"), y++, x);

			inkey();
			screen_load();

			return NULL; //行動順を消費しない
		}
		break;

	case 1:
		{
			int food;
			if(only_info) return format("");

			if(use_itemcard)
			{
				if(remove_all_curse())
				{
					msg_print("カードが厄を吸い取った。");
					set_food(PY_FOOD_MAX - 1);
				}
			}
			else
			{
				if(p_ptr->food >= PY_FOOD_FULL)
				{
					msg_print("活力は十分だ。");
					NULL;
				}
				if(!yaku)
				{
					msg_print("活力にする厄が全くない…");
					NULL;
				}

				food = PY_FOOD_MAX - 1 - p_ptr->food;
				if(food > yaku * 30) food = yaku * 30;

				set_food(p_ptr->food + food);

				hina_gain_yaku(-(food/30));
			}
		}
		break;


	case 2:
		{
			char o_name[MAX_NLEN];
			int i;
			int gain = 0;
			if(only_info) return format("");

			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				object_type *o_ptr = &inventory[i];
				if (!o_ptr->k_idx) continue;
				if (!object_is_cursed(o_ptr)) continue;
				object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

				if (o_ptr->curse_flags & TRC_PERMA_CURSE)
				{
					msg_format("%sの呪いはあまりにも強い！呪いを吸いきれなかった！",o_name);
					o_ptr->curse_flags &= (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE);
					gain += 1000 + randint1(1000);
				}
				else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					msg_format("%sの重い呪いを吸い取った！",o_name);
					gain += 500 + randint1(500);
					o_ptr->curse_flags = 0L;
				}
				else
				{
					msg_format("%sの呪いを吸い取った。",o_name);
					gain += 200 + randint1(200);
					o_ptr->curse_flags = 0L;
				}
				//未鑑定の場合簡易鑑定
				if(!object_is_known(o_ptr))
				{
					o_ptr->ident |= (IDENT_SENSE);
					o_ptr->feeling = value_check_aux1(o_ptr);
				}
				p_ptr->update |= PU_BONUS;
				p_ptr->window |= (PW_INVEN | PW_EQUIP);


			}
			if(gain)
				hina_gain_yaku(gain);
			else
				msg_print("厄を吸収できるものを装備していない。");

		}
		break;

	case 3:
		{
			int dice = 12;
			int sides = 10 + plev / 5;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("厄を放った。");
			fire_ball_hide(GF_YAKU, dir, damroll(dice,sides),0);
			break;
		}
	//厄感知
	case 4:
		{
			int rad = 20 + plev / 5;
			if(only_info) return format("範囲:%d+α",rad);
			msg_print("あなたは厄を探った・・");
			search_specific_object(5);

			detect_traps(rad,TRUE);
			if(plev>29) detect_monsters_evil(rad);

			break;
		}

	case 5:
		{
			cptr q,s;
			int item;
			object_type *o_ptr;
			int mag;
			int dam;

			if(only_info) return format("");
			item_tester_hook = item_tester_hook_broken_amulet;

			q = "どのアミュレットを投げますか? ";
			s = "壊れたアミュレットがない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0) o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			dam = count_bits(o_ptr->curse_flags) * 100;
			if(o_ptr->curse_flags & TRC_PERMA_CURSE) dam += 800;
			else if(o_ptr->curse_flags & TRC_HEAVY_CURSE) dam += 200;
			if(object_is_artifact(o_ptr)) dam += 800;
			else if(object_is_ego(o_ptr)) dam += 200;
			else dam += 100;

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("投げつけたアミュレットが弾けた！");
			fire_ball(GF_SHARDS, dir, dam,3);

			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}

		}
		break;

	case 6:
		{
			int dam = 200 + plev * 6;
			int rad = 4 + plev / 15;
			if(only_info) return format("損傷:～%d",dam/2);

			msg_print("厄があなたの周りを渦巻いた！");
			project(0,rad,py,px,dam,GF_YAKU,PROJECT_KILL,-1);
			break;
		}
	case 7:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);

			time = base + randint1(base);
			set_tim_eyeeye(time,FALSE);
			break;
		}

	case 8:
		{
			int y, x, base;
			monster_type *m_ptr;

			base = 10 + p_ptr->lev / 5;
			if(only_info) return format("期間:%d",base);
			if (!get_aim_dir(&dir)) return NULL;

			x = target_col;
			y = target_row;

			if(dir != 5 || !target_okay() || !projectable(y,x,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("厄が渦巻いて%sを捕えた！",m_name);
				p_ptr->magic_num1[2] = cave[y][x].m_idx;
				set_tim_general(base,FALSE,0,0);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;
		}

	case 9:
		{

			if(only_info) return format("");

			if (!(p_ptr->special_defense & SD_HINA_NINGYOU))
			{
				msg_print("あなたは厄を撒き散らし始めた…");

				p_ptr->special_defense |= SD_HINA_NINGYOU;
				p_ptr->redraw |= (PR_STATUS);
			}
			else
			{
				msg_print("あなたは厄を撒き散らすのを止めた。");

				p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
				p_ptr->redraw |= (PR_STATUS);
			}

			break;
		}



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::ドレミー・スイート専用技*/
class_power_type class_power_doremy[] =
{

	{1,10,0,FALSE,FALSE,A_CHR,0,0,"夢を食べる",
		"隣接した眠っている敵の夢を食べ腹を満たす。ダンジョン「夢の世界」では眠り状態でない敵にも効果が発生しその敵は消滅する。その場合抵抗されることがあり、ユニークモンスターには効かない。"},
	{7,7,30,FALSE,TRUE,A_WIS,0,2,"緋色の悪夢",
		"指定ターゲット一体を高確率で眠らせる。レベル25以降は睡眠耐性を持つ敵にも効く可能性がある。ダンジョン「夢の世界」では精神攻撃を行う。"},
	{16,25,35,FALSE,FALSE,A_INT,0,0,"夢診断",
		"広範囲の地形、アイテム、トラップ、モンスターを感知し、さらに視界内の敵の情報を得る。ダンジョン「夢の世界」でしか使えない。"},
	{20,18,45,FALSE,TRUE,A_WIS,0,4,"藍色の愁夢",
		"指定ターゲットに悪夢を見せて攻撃する。この攻撃では眠った敵を起こしにくい。敵が眠っているかダンジョン「夢の世界」で使ったときは威力が増す。"},
	{24,27,50,FALSE,TRUE,A_INT,0,5,"ドリームキャッチャー",
		"敵一体に無属性ダメージを与える。この攻撃で倒した敵に変身することができる。変身中はリボン・アミュレット以外の装備が無効化され、変身したモンスターの能力を'U'コマンドで使うことができる。ダンジョン「夢の世界」でないと使えない。"},
	{27,0,80,FALSE,FALSE,A_WIS,0,0,"夢治療",
		"HPとMPを僅かに回復する。ダンジョン「夢の世界」では回復量が増える。"},
	{30,27,60,FALSE,TRUE,A_WIS,0,5,"刈安色の迷夢",
		"視界内の敵を眠らせる。睡眠耐性を無視するがユニークモンスターには効果がない。ダンジョン「夢の世界」では高威力の混乱攻撃になる。"},
	{32,40,65,FALSE,TRUE,A_INT,0,0,"ドリームエクスプレス",
		"フロアの任意の場所へ移動する。通常のテレポートで侵入できない場所には移動できない。ダンジョン「夢の世界」でしか使えない。"},
	{35,50,65,FALSE,TRUE,A_WIS,0,0,"留紺色の逃走夢",
		"強力な悪夢属性のボールを放つ。ダンジョン「夢の世界」ではさらに威力が増す。" },
	{38,48,60,FALSE,TRUE,A_INT,0,0,"バタフライサプランテーション",
		"今いるフロアが再生成される。フロアの変化の前には一瞬の遅延が発生する。ダンジョン「夢の世界」でしか使えない。"},
	{42,60,70,FALSE,TRUE,A_WIS,0,0,"紺色の狂夢",
		"敵一体に強力な精神攻撃を仕掛ける。抵抗されると無効。ユニークモンスターにはやや抵抗されやすく、通常の精神を持たない敵には効果が薄い。ダンジョン「夢の世界」ではさらに威力が増す。"},
	{44,80,75,FALSE,TRUE,A_INT,0,0,"分身",
		"短時間分身して敵からの攻撃に当たりづらくなる。" },
	{47,90,80,FALSE,TRUE,A_CHR,0,0,"漆黒の宇宙夢",
		"自分を中心に強力な暗黒属性のボールを発生させる。ダンジョン「夢の世界」では視界内攻撃になる。" },

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_doremy(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;
	bool in_dream = (IN_DREAM_WORLD);

	switch(num)
	{

	case 0:
		{
			int x, y;
			char m_name[160];
			monster_type *m_ptr;
			int food = 5000;
			int power = 50 + plev;
			if(only_info) return format("効力:%d",power);

			if(p_ptr->food >= PY_FOOD_MAX) food = 0;
			else if(p_ptr->food + food >= PY_FOOD_MAX) food = PY_FOOD_MAX - 1 - p_ptr->food;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!cave[y][x].m_idx || !(m_ptr->ml) )
			{
				msg_print("そこには何もいない。");
				return NULL;
			}

			monster_desc(m_name,m_ptr,0);
			if(in_dream)
			{
				if(genocide_aux(cave[y][x].m_idx,power,TRUE,0,""))
				{
					msg_format("%sの夢を美味しく頂いた。",m_name);
					if(food) set_food(p_ptr->food + food);
				}
			}
			else
			{
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if (!MON_CSLEEP(m_ptr))
				{
					msg_format("%sは起きている。",m_name);
					return NULL;
				}
				else if(r_ptr->flags2 & (RF2_EMPTY_MIND))
				{
					msg_format("%sの夢はあなたの賞味できるようなものではなかった。",m_name);
					break;
				}
				else if(m_ptr->mflag & MFLAG_SPECIAL)
				{
					msg_format("%sの夢はもう賞味済みだ。",m_name);
					break;
				}
				else if(r_ptr->flags2 & (RF2_WEIRD_MIND))
				{
					msg_format("%sの夢はほとんど味がしない・・",m_name);
					food /= 10;
				}
				else if(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN) || is_gen_unique(m_ptr->r_idx))
				{
					msg_format("%sの夢を美味しく頂いた。",m_name);
				}
				else
				{
					msg_format("%sの夢はちょっと薄味で美味しくない。",m_name);
					food /= 3;
				}
				if(food) set_food(p_ptr->food + food);
				m_ptr->mflag |= MFLAG_SPECIAL; //特殊フラグを立てて二度目以降を無効に
				(void)set_monster_csleep(cave[y][x].m_idx, 0);

			}
		}
		break;

	case 1:
	case 3:
	case 10:
		{
			int x, y;
			int dice,sides;
			int power;
			int typ;
			monster_type *m_ptr;

			if(num == 1 && !in_dream)
			{
				power = 50 + plev * 4;
				if(only_info) return format("効力:%d",power);
			}
			else if(num == 1)
			{
				typ = GF_REDEYE;
				dice = 9 + plev / 3;
				sides = 8 + chr_adj / 6;

			}
			else if(num == 3)
			{
				typ = GF_NIGHTMARE;
				dice = 6 + plev / 4;
				sides = 13 + chr_adj / 4;

				if(in_dream) dice *= 2;

			}
			else
			{
				typ = GF_COSMIC_HORROR;

				dice = plev;
				sides = 20 + chr_adj / 3;

				if(in_dream) dice = dice * 3 / 2;


			}

			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags2 & RF2_EMPTY_MIND)
				{
					msg_format("%sは精神を持たないようだ。",m_name);
					break;
				}

				if(num == 1 && !in_dream)
				{
					msg_format("%sを夢の世界へ誘った・・",m_name);
					if(plev > 24) project(0,0,y,x,power,GF_STASIS,(PROJECT_KILL|PROJECT_JUMP),-1);
					else	project(0,0,y,x,power,GF_OLD_SLEEP,(PROJECT_KILL|PROJECT_JUMP),-1);
				}
				else
				{
					if(num == 1) msg_format("%sへ悪夢を見せた。",m_name);
					else if(num == 3) msg_format("%sへ愁夢を見せた。",m_name);
					else msg_format("%sへ狂夢を見せた。",m_name);

					project(0,0,y,x,damroll(dice,sides),typ,(PROJECT_KILL|PROJECT_JUMP),-1);
				}
			}
		}
		break;

	case 2:
		{
			int rad = 25 + plev / 2;
			if (only_info) return format("範囲:%d",rad);
			msg_print("辺りの様子を分析した・・");
			map_area(rad);
			detect_all(rad);
			detect_monsters_normal(rad);
			probing();
			break;
		}

	case 4:
		{
			int x, y;
			int dist = 7;
			int dam = plev * 2 + 100;
			monster_type *m_ptr;
			char m_name[80];

			if(only_info) return format("射程:%d 損傷:%d",dist,dam);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("近くのターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			monster_desc(m_name, m_ptr, 0);

			if(!in_dream && !MON_CSLEEP(m_ptr))
			{
				msg_format("%sは眠っていない。",m_name);
				return NULL;
			}
			else
			{
				int r_idx = m_ptr->r_idx;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags2 & RF2_EMPTY_MIND)
				{
					msg_format("%sは精神を持たないようだ。",m_name);
					break;
				}

				msg_format("%sの夢を取り込もうとした・・",m_name);
				project(0,0,y,x,dam,GF_MISSILE,(PROJECT_KILL|PROJECT_JUMP),-1);
				if(!m_ptr->r_idx)
				{
					metamorphose_to_monster(r_idx,-1);
				}
			}
		}
		break;

	case 5:
		{
			if(only_info) return format("");
			if(in_dream)
			{
				msg_print("あなたは自分の夢の修復を試みた・・");
				player_gain_mana(plev/10+randint1(plev/10));
				hp_player(plev/3+randint1(plev/3));
			}
			else
			{
				msg_print("あなたは寝息を立て始めた・・");
				player_gain_mana(1+randint1(plev/12));
				hp_player(plev/7+randint1(plev/7));
			}
		}
		break;



	case 6:
		{
			int power = 50 + plev * 2;
			if(in_dream) power += 90 + chr_adj * 2;
			if(only_info) return format("効果:%d",power);

			if(in_dream)
			{
				msg_print("周囲に混迷をもたらした。");
				project_hack2(GF_CONFUSION,0,0,power);
			}
			else
			{
				msg_print("錯綜した幻覚を生み出した。");
				stasis_monsters(power);
			}

		}
		break;

	case 7:
		{
			if(only_info) return format("距離:無制限");
			if (!dimension_door(D_DOOR_DREAM)) return NULL;
		}
		break;

	case 8:
	{
		int dice = 5 + plev / 2;
		int sides = 10 + chr_adj / 5;

		if (in_dream) dice *=2;

		if (only_info) return format("損傷:%dd%d", dice, sides);
		if (!get_aim_dir(&dir)) return NULL;

		msg_print("重苦しい悪夢を練り上げた...");
		fire_ball(GF_NIGHTMARE, dir, damroll(dice, sides), 3);

		break;
	}


	case 9:
		{
			if(only_info) return format("効果:現実変容");
			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}

			msg_print("周囲の景色がぼやけた・・");
			/*:::0ではまずいはず*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;

	case 11:
		{
			int dice = 8;
			int base = 8;
			if (only_info) return format("期間:%d+1d%d", base, dice);
			set_multishadow(base + randint1(dice), FALSE);
			break;
		}
	case 12:
		{
			int rad = 2;
			int base = p_ptr->lev * 15 + chr_adj * 25;
			if (only_info) return format("損傷:～%d", base / 2);

			msg_format("あなたはこの場を暗黒の宇宙空間へと変えた！");
			if (in_dream)
			{
				project_hack2(GF_DARK, 0, 0, base / 2);
			}
			else
			{
				project(0, rad, py, px, base, GF_DARK, PROJECT_KILL, -1);
			}
			break;
		}




	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::クラウンピース専用技*/
class_power_type class_power_clownpiece[] =
{

	{6,8,30,FALSE,FALSE,A_CHR,0,5,"仲間を呼ぶ",
		"妖精を配下として呼び出す。"},
	{12,20,35,TRUE,TRUE,A_WIS,0,0,"レーザー",
		"魔力属性のビームを放つ。"},
	{20,25,30,FALSE,FALSE,A_CHR,0,5,"ヘルエクリプス",
		"今いる部屋を明るくし、視界内の敵を混乱・朦朧・恐怖させる。レベル35以降は視界内の敵に精神攻撃を行う。「★クラウンピースの松明」を所持していないと使えない。"},
	{25,80,40,TRUE,TRUE,A_WIS,0,0,"フラッシュアンドストライプ",
		"大型の閃光属性ビームを放つ。"},
	{30,120,50,TRUE,TRUE,A_WIS,0,0,"グレイズインフェルノ",
		"自分のいる場所に巨大な火炎属性のボールを複数回発生させる。"},
	{35,66,55,FALSE,TRUE,A_CHR,0,0,"バースティンググラッジ",
		"自分を中心に巨大な地獄属性ボールを発生させる。威力はこれまで受けたダメージの累積値の半分。使うたびにダメージ累積値がリセットされる。" },

	{40,240,60,TRUE,TRUE,A_WIS,0,0,"ストライプドアビス",
		"視界内の全てに対し地獄の劫火属性の攻撃を行う。"},
	{45,320,70,TRUE,TRUE,A_CHR,0,0,"フェイクアポロ",
		"視界内のターゲットの位置に隕石属性の巨大な球を3回発生させる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_clownpiece(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int dir;

	switch(num)
	{

	case 0:
		{
			int level = plev + 10;
			int num = 1 + plev / 16;
			bool flag = FALSE;
			if(only_info) return format("召喚レベル:%d",level);
			for(;num>0;num--)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_FAIRY, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if(flag) msg_format("仲間を呼んだ！");
			else msg_format("誰も現れなかった・・");

		}
		break;
	case 1:
		{
			int dice = 3 + plev / 4;
			int sides = 8;
			int base = 20 + plev * 2 + chr_adj * 2;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			fire_beam(GF_MANA, dir, base + damroll(dice,sides));
			break;
		}
	case 2:
		{
			int power = 50 + p_ptr->lev * 4;
			if(power < 100) power = 100;
			if (only_info) return format("効力:%d",power);
			msg_format("狂気の光が周囲を染め上げた！");
			project_hack2(GF_REDEYE,0,0,power);
			confuse_monsters(power);
			stun_monsters(power);
			turn_monsters(power);
			lite_room(py, px);
		}
		break;
	case 3:
		{
			int damage = 50 + p_ptr->lev * 6 + chr_adj * 5;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("数条の光線が前方を薙ぎ払った！");
			fire_spark(GF_LITE,dir,damage,2);
		}
		break;
	case 4:
		{
			int num = 4 + (plev-3) / 11;
			int dam = 200 + plev * 2 + chr_adj * 2;
			int rad = 2 + plev / 12;
			int i;
			if(only_info) return format("損傷:最大%d * %d",dam/2,num);
			msg_print("熱波が周囲を焼き払った！");
			for (i = 0; i < num; i++) project(0, rad, py, px, dam, GF_FIRE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			break;
		}
		break;
	case 5:
		{
			int rad, dam;

			dam = p_ptr->magic_num1[0] - p_ptr->magic_num1[1];

			if (dam > 9999) dam = 9999;

			rad = 1 + dam / 1200;

			if (only_info) return format("損傷:最大%d",dam / 2);

			if (dam< 1000)
			{
				msg_print("この技はもっとダメージを受けないと使えない。");
				return NULL;
			}

			msg_format("怨みのパワーを解き放った！");
			project(0, rad, py, px, dam, GF_NETHER, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);

			p_ptr->magic_num1[1] = p_ptr->magic_num1[0];

			break;
		}


	case 6:
		{
			int dam = plev * 10 + chr_adj * 10;
			if(dam < 400) dam = 400;

			if(only_info) return format("損傷:%d",dam);
			msg_format("地獄の劫火が地面から噴き出した！");
			project_hack2(GF_HELL_FIRE,0,0,dam);
			break;
		}
	case 7:
		{
			int x, y;
			int dist = 7;
			int num = 3;
			int rad = 5;
			monster_type *m_ptr;
			int dice = 10;
			int sides = plev * 2;

			if(only_info) return format("射程:%d 損傷:%dd%d*%d",dist,dice,sides,num);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("そこを狙うことはできない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sを目掛けて巨大隕石が落ちた！",m_name);
				for(;num>0;num--)
				{
					project(0,rad,y,x,damroll(dice,sides),GF_METEOR,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
					if(!m_ptr->r_idx) break;
				}
			}

			break;
		}




	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::リリカ用特技*/
class_power_type class_power_lyrica[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"音を止める",
		"今奏でている音があれば止める。行動力を消費しない。"},
	{8,8,20,FALSE,TRUE,A_CHR,0,2,"ファツィオーリ冥奏",
		"自分を中心とした轟音属性のボールを発生させる。"},
	{17,12,35,FALSE,TRUE,A_DEX,0,0,"ファントムノイズ",
		"敵の攻撃を回避して短距離テレポートするようになる。回避に失敗することもある。"},
	{24,24,50,FALSE,TRUE,A_INT,0,0,"ソウルノイズフロー",
		"周囲のモンスター、扉、罠、アイテムを感知する。"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"ライブポルターガイスト",
		"視界内全てに対し強力な轟音属性攻撃を行う。行動時間を1.5倍消費し、周囲の眠っている敵が起きる。"},

	{35,8,70,FALSE,FALSE,A_WIS,50,0,"リリカ・ソロライブ",
		"魔法防御上昇と元素攻撃への耐性を得る音楽を奏でる。ターンごとにMPを4消費する。"},
	{ 40,60,75,FALSE,FALSE,A_CHR,0,10,"プリズムコンチェルト",
		"視界内全てに対し閃光と水の複合属性攻撃を行う。さらに閃光・混乱耐性のない敵を確実に混乱させる。行動時間を1.5倍消費する。" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"霊車コンチェルトグロッソ",
		"近くの指定したターゲットの位置に強力な無属性のボールを発生させる。威力は魅力に大きく依存する。行動時間を1.5倍消費する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::リリカ特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_lyrica(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int base = 30 + plev * 3 + chr_adj * 4;
			int rad = 2 + plev / 12;

			if(only_info) return format("範囲:%d 損傷:～%d",rad,base/2);
			msg_format("奇妙な音が辺りを包んだ・・");
			project(0,rad,py,px,base,GF_SOUND,(PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID|PROJECT_ITEM),-1);

			break;
		}
	case 2:
		{

			if(only_info) return format("");
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("あなたはこっそりと仕込みを済ませた・・");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}
			break;
		}
	case 3:
		{
			int rad = plev / 2;
			if(rad < 12) rad = 12;
			if (only_info) return format("範囲:%d",rad);
			detect_all(rad);

			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			msg_print("三姉妹が集まり、破壊的な騒音を起こした！");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("姉達は帰って行った。");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_LYRICA_SOLO, SPELL_CAST);

			break;
		}

	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("損傷:%d", dam);
		msg_print("三姉妹が集まり、幻惑的な光を放った！");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("姉達は帰って行った。");

		break;
	}

	case 7:
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("損傷:%dd%d 射程:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("三姉妹が集まり、%sのための葬送曲を奏でた！",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("姉達は帰って行った。");
			}
			break;
		}

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::メルラン用特技*/
class_power_type class_power_merlin[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"音を止める",
		"今奏でている音があれば止める。行動力を消費しない。"},
	{8,10,20,FALSE,TRUE,A_CHR,0,2,"ヒノファンタズム",
		"轟音属性のボールを放つ。"},
	{17,20,35,FALSE,TRUE,A_STR,0,0,"トランペットソウル",
		"一定時間士気高揚と物理防御上昇を得る。魔法防御能力が大幅に低下する。時々狂戦士化する。"},
	{24,30,50,FALSE,TRUE,A_WIS,0,0,"ソウルゴーハッピー",
		"一定時間加速と肉体強化を得る。ただし周囲の敵が起き視界内の敵も加速する。"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"ライブポルターガイスト",
		"視界内全てに対し強力な轟音属性攻撃を行う。行動時間を1.5倍消費し、周囲の眠っている敵が起きる。"},
	{35,36,70,FALSE,FALSE,A_WIS,50,0,"メルラン・ハッピーライブ",
		"周囲の敵の魔法成功率を低下させ、攻撃行動を低確率で妨害する音を鳴らし続ける。高レベルな敵には効きにくい。ターンごとにMPを18消費する。"},
	{ 40,60,75,FALSE,FALSE,A_CHR,0,10,"プリズムコンチェルト",
		"視界内全てに対し閃光属性攻撃を行う。さらに閃光・混乱耐性のない敵を確実に混乱させる。行動時間を1.5倍消費する。" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"霊車コンチェルトグロッソ",
		"近くの指定したターゲットの位置に強力な無属性のボールを発生させる。威力は魅力に大きく依存する。行動時間を1.5倍消費する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::メルラン特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_merlin(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int rad = 2 + plev / 16;
			int dice = 3 + plev / 4;
			int sides = 9;
			int base = plev + chr_adj * 2;

			if(only_info) return format("半径:%d 損傷:%d+%dd%d",rad, base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("トランペットから爆音が放たれた！");
			fire_ball(GF_SOUND, dir, base + damroll(dice,sides),rad);
			break;
		}
	case 2:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);
			msg_format("あなたはトランペットを高らかに吹き鳴らした！");
			time = base + randint1(base);

			if(one_in_(4))
				set_shero(time,FALSE);
			else
				set_hero(time,FALSE);
			set_shield(time,FALSE);
			break;
		}
	case 3:
		{
			int base = 10+plev/5;
			int time;
			if (only_info) return format("期間:%d+1d%d",base,base);
			msg_format("陽気な音楽がダンジョンに響き渡った！");
			time = base + randint1(base);

			set_tim_addstat(A_STR,1+plev/12,time,FALSE);
			set_tim_addstat(A_CON,1+plev/12,time,FALSE);
			set_tim_addstat(A_DEX,1+plev/12,time,FALSE);
			set_fast(time,FALSE);

			aggravate_monsters(0,TRUE);

			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			msg_print("三姉妹が集まり、破壊的な騒音を起こした！");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("姉と妹は帰って行った。");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MERLIN_SOLO, SPELL_CAST);

			break;
		}
	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("損傷:%d", dam);
		msg_print("三姉妹が集まり、幻惑的な光を放った！");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("姉と妹は帰って行った。");

		break;
	}


	case 7:
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("損傷:%dd%d 射程:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("三姉妹が集まり、%sのための葬送曲を奏でた！",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("姉と妹は帰って行った。");
			}
			break;
		}

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::ルナサ用特技*/
class_power_type class_power_lunasa[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"音を止める",
		"今奏でている音があれば止める。行動力を消費しない。"},
	{8,9,20,FALSE,TRUE,A_CHR,0,2,"グァルネリ・デル・ジェス",
		"轟音属性のビームを放つ。"},
	{17,16,35,FALSE,TRUE,A_DEX,0,4,"スローサウンド",
		"指定した場所に遅鈍属性の小型のボールを発生させる。"},
	{24,32,50,FALSE,TRUE,A_WIS,0,0,"ノイズメランコリー",
		"視界内の敵を減速、混乱、恐怖させる。レベル35で朦朧効果も追加する。"},
	{30,60,65,FALSE,FALSE,A_CHR,0,10,"ライブポルターガイスト",
		"視界内全てに対し強力な轟音属性攻撃を行う。行動時間を1.5倍消費し、周囲の眠っている敵が起きる。"},

	{35,24,70,FALSE,FALSE,A_WIS,50,0,"ルナサ・ソロライブ",
		"近くの敵の行動順をわずかに遅らせる音を鳴らし続ける。高レベルな敵には効きにくい。ターンごとにMPを12消費する。"},

	{40,60,75,FALSE,FALSE,A_CHR,0,10,"プリズムコンチェルト",
		"視界内全てに対し閃光属性攻撃を行う。さらに閃光・混乱耐性のない敵を確実に混乱させる。行動時間を1.5倍消費する。" },

	{44,99,80,FALSE,TRUE,A_CHR,0,0,"霊車コンチェルトグロッソ",
		"近くの指定したターゲットの位置に強力な無属性のボールを発生させる。威力は魅力に大きく依存する。行動時間を1.5倍消費する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::ルナサ特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_lunasa(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1:
		{
			int dice = 2 + plev / 4;
			int sides = 6;
			int base = plev + chr_adj;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("ヴァイオリンから重厚な音が流れだした。");
			fire_beam(GF_SOUND, dir, base + damroll(dice,sides));
			break;
		}
	case 2:
		{
			int base = p_ptr->lev * 3 + chr_adj * 3;
			int rad = plev / 15;

			if(only_info) return format("半径:%d 損傷:%d",rad,base);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_INACT, dir, base, rad,"重苦しい音が辺りを包んだ・・")) return NULL;

			break;
		}
	case 3:
		{
			int power = p_ptr->lev * 4;
			if(power < 100) power = 100;

			if (only_info) return format("効力:%d",power);
			msg_format("陰気な音が響き渡った・・");
			if(plev > 34) stun_monsters(power);
			confuse_monsters(power);
			slow_monsters(power);
			turn_monsters(power);
			break;
		}

	case 4:
		{
			int dam = plev * 4 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			msg_print("三姉妹が集まり、破壊的な騒音を起こした！");
			project_hack2(GF_SOUND,0,0,dam);
			aggravate_monsters(0,FALSE);
			new_class_power_change_energy_need = 150;
			msg_print("妹達は帰って行った。");
			break;
		}

		break;
	case 5:
		{
			if(only_info) return  format("");
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_LUNASA_SOLO, SPELL_CAST);

			break;
		}

	case 6:
	{
		int dam = plev * 3 + chr_adj * 3;
		if (only_info) return format("損傷:%d", dam);
		msg_print("三姉妹が集まり、幻惑的な光を放った！");
		project_hack2(GF_RAINBOW, 0, 0, dam);
		new_class_power_change_energy_need = 150;
		msg_print("妹達は帰って行った。");
		break;
	}

	break;
	case 7:
		{
			int x, y;
			int dist = 5;
			int rad = 6;
			int dice = 15+chr_adj/2;
			int sides = plev;
			monster_type *m_ptr;

			if(only_info) return format("損傷:%dd%d 射程:%d",dice,sides,dist);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("三姉妹が集まり、%sのための葬送曲を奏でた！",m_name);
				project(0,rad,y,x,damroll(dice,sides),GF_DISP_ALL,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL),-1);
				new_class_power_change_energy_need = 150;
				msg_print("妹達は帰って行った。");
			}
			break;
		}

	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::アリス専用技*/
class_power_type class_power_alice[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"人形装備",
		"人形に武器を持たせる。人形によって装備できる武器の種類と重量が変わり、盾を持てる人形もいる。武器の修正値・スレイ・属性・切れ味以外の能力は効果を発揮しない。盾の場合AC・耐性以外の能力は効果を発揮しない。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"人形武装解除",
		"人形の装備しているアイテムを外す。"},
	{10,16,30,FALSE,TRUE,A_INT,0,5,"魔彩光の上海人形",
		"閃光属性のビームを放つ。"},
	{16,20,40,FALSE,TRUE,A_DEX,0,0,"人形作成",
		"モンスター「人形」を配下として数体召喚する。"},
	{22,30,50,FALSE,TRUE,A_DEX,0,0,"アーティフルサクリファイス",
		"アイテム「人形」を投擲し火炎属性の爆発を発生させる。威力は人形のHPの1/2(最大1600)となる。"},
	{27,40,60,FALSE,TRUE,A_INT,0,0,"スーサイドパクト",
		"配下のモンスター「人形」を全てその場で爆発させる。"},
	{32,45,75,FALSE,TRUE,A_WIS,0,0,"ドールズウォー",
		"離れた場所に対して人形による通常攻撃を行う。"},

	{36,50,80,FALSE,TRUE,A_INT,0,10,"首吊り蓬莱人形",
		"暗黒属性の強力なビームを放つ。"},
	{40,50,85,FALSE,TRUE,A_DEX,0,0,"リターンイナニメトネス",
		"アイテム「人形」を投擲し魔力属性の大爆発を発生させる。威力は人形のHPの1/3(最大3200)となる。"},
	{46,70,85,FALSE,TRUE,A_CHR,0,0,"グランギニョル座の怪人",
		"視界内の敵一体を指定し、その敵に射線が通った配下の人形全てから無属性ビームが放たれる。ビーム一発ごとにMP3を消費する。配下の人形がこのビームで傷つくことはない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_alice(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;

			p_ptr->update |= PU_BONUS;
			update_stuff();
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			p_ptr->update |= PU_BONUS;
			update_stuff();
			break;
		}
	case 2:
		{
			dice = 2 + plev / 3;
			sides = 8;
			base = plev + chr_adj * 2;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("上海人形がビームを放った！");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}
	case 3:
		{
			bool flag = FALSE;
			int max = 1 + p_ptr->lev / 10;
			if(only_info) return format("最大:%d体",max);

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_ALICE_DOLL, PM_FORCE_PET)) flag = TRUE;
			if(flag) msg_print("人形を放った。");
			else msg_print("人形の作成に失敗した。");

			break;
		}
	case 4:
	case 8:
		{
			int     item;
			cptr    q, s;
			object_type *o_ptr;
			monster_race *r_ptr;
			int typ;
			int rad;

			if(only_info) return format("");

			item_tester_tval = TV_FIGURINE;


			q = "どの人形を投げますか? ";
			s = "人形を持っていない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			r_ptr = &r_info[o_ptr->pval];

			if(r_ptr->flags1 & RF1_FORCE_MAXHP)
				damage = r_ptr->hdice * r_ptr->hside;
			else
				damage = damroll(r_ptr->hdice,r_ptr->hside);
			if(num == 4)
			{
				damage /= 2;
				if(damage > 1600) damage = 1600;
				if(!damage) damage = 1;
				typ = GF_FIRE;
				rad = 3;
			}
			else
			{
				damage /= 3;
				if(damage > 3200) damage = 3200;
				if(!damage) damage = 1;
				typ = GF_MANA;
				rad = 6;

			}

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("人形を投げつけた！");
			fire_ball(typ, dir, damage,rad);
			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}

			break;
		}
	case 5:
		{
			monster_type *m_ptr;
			int xx,yy;
			int damage = 30 + plev * 2 + chr_adj;
			bool flag = FALSE;
			if(only_info) return format("損傷:一体につき%d",damage);
			msg_format("あなたは人形たちに起爆指令を出した！");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (m_ptr->r_idx != MON_ALICE_DOLL) continue;
				if (!is_pet(m_ptr)) continue;
				flag = TRUE;
				xx = m_ptr->fx;
				yy = m_ptr->fy;
				delete_monster_idx(i);
				project(0,3,yy,xx,damage,GF_FIRE,PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM,-1);

			}
			if(!flag) msg_print("しかし爆発させる人形がいなかった・・");

		}
		break;

	case 6:
		{
			int range = 2 + (plev-32) / 8;
			int x, y;
			monster_type *m_ptr, *m2_ptr;

			if(range < 2) range = 2; //paranoia

			if(only_info) return format("射程:%d",range);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("人形たちが%s目掛けて飛びかかった！",m_name);
				py_attack(m_ptr->fy,m_ptr->fx,0);

			}
		}
		break;



	case 7:
		{
			dice = plev / 2;
			sides = 10 + chr_adj / 10;
			base = plev * 3 + chr_adj * 3;

			if(dice < 15) dice = 15;
			if(base < 100) base = 100;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("蓬莱人形がビームを放った！");
			fire_spark(GF_DARK, dir, base + damroll(dice,sides),1);
			break;
		}
	case 9:
		{
			int dam = plev + chr_adj;
			int x, y;
			monster_type *m_ptr, *m2_ptr;
			bool flag = FALSE;

			if(only_info) return format("損傷:%d * 不定",dam);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				msg_print("惨劇の舞台の幕が上がった！");
				for (i = 1; (i < m_max && m_ptr->r_idx); i++)
				{
					m2_ptr = &m_list[i];
					if (m2_ptr->r_idx != MON_ALICE_DOLL) continue;
					if (!is_pet(m2_ptr)) continue;
					if(!projectable(m2_ptr->fy,m_ptr->fx,y,x)) continue;
					flag = TRUE;
					project(i,0,y,x,dam,GF_BANKI_BEAM2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU),-1);
					p_ptr->csp -= 3;
					if(p_ptr->csp <= 70) break;
				}
				if(!flag) msg_print("・・気がした。");
			}
		}
		break;
	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::サニー専用技*/
class_power_type class_power_sunny[] =
{
	{8,6,20,FALSE,TRUE,A_STR,0,2,"アグレッシブライト",
		"閃光属性のビームを放つ。夜間は威力が落ちる。"},
	{15,5,25,FALSE,TRUE,A_INT,0,0,"幻惑Ⅰ",
		"敵一体を混乱させる。賢い敵には効かない。夜間は効果が落ちる。"},
	{20,10,40,FALSE,TRUE,A_INT,0,0,"ルチルフレクション",
		"光を屈折させて離れた場所を視認する。ただし閉じたドアやカーテンで遮られた所を見ることはできない。" },

	{24,20,35,FALSE,TRUE,A_CON,0,4,"サンシャインブラスト",
		"今いる部屋を明るくし、視界内全てに閃光属性攻撃を行う。ただし日光の蓄積が不十分な場合は光に弱いモンスターにしかダメージを与えられない。夜間は威力が落ちる。"},
	{32,30,50,FALSE,TRUE,A_INT,0,0,"幻惑Ⅱ",
		"一定時間、周囲の敵を惑わせる。視界に頼らない敵に対しては効果が薄い。夜間は効果時間が短くなる。"},
	{40,45,70,FALSE,TRUE,A_STR,0,7,"フェイタルフラッシュ",
		"部屋を明るくし、隣接した敵一体に閃光属性ダメージを与え、朦朧、混乱させる。夜間は効果が落ちる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_sunny(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			dice = 3 + p_ptr->lev / 5;
			sides = 5;
			base = p_ptr->lev / 2 + chr_adj / 2;

			if(is_daytime())
			{
				sides *= 2;
				base *= 2;
			}
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) base += p_ptr->lev;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("ビームを放った！");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-100);

			break;
		}
	case 1:
		{
			int x, y;
			int dist = 2 + plev / 5;
			monster_type *m_ptr;
			int power = 20 + plev;
			if(is_daytime()) power += 30;

			if(only_info) return format("射程:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたは%sの周囲の景色を歪めた・・",m_name);

				if(r_ptr->flags2 & RF2_SMART || r_ptr->flags3 & RF3_NO_CONF ||
					r_ptr->flagsr & RFR_RES_ALL || randint1(power) < r_ptr->level)
				{
					msg_format("しかし効果がなかった。");
				}
				else
				{
					msg_format("%sは混乱した！",m_name);
					(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/3));
				}
				set_monster_csleep(cave[y][x].m_idx, 0);
				anger_monster(m_ptr);
			}

			break;
		}
	case 2:
		{
			//v1.1.34 サニーの「光を屈折させて離れた場所の光景を見る」能力を追加。
			//まず特殊フラグを立ててからflying_dist値を閉じたドアやカーテンに遮られるようにして再計算する。
			//次にこのflying_dist値が設定されたグリッドのみを啓蒙、モンスター感知する。
			//最後にフラグを戻してflying_dist値を再計算する。
			if (only_info) return format("範囲:%d移動グリッド", MONSTER_FLOW_DEPTH);

			if (p_ptr->blind)
			{
				msg_print("目が見えない！");
				return NULL;
			}

			msg_print("離れた場所の映像があなたの目の前に広がった……");
			flag_sunny_refraction = TRUE;
			update_flying_dist();
			wiz_lite(FALSE);
			detect_monsters_normal(255);
			flag_sunny_refraction = FALSE;
			update_flying_dist();

		}
		break;



	case 3:
		{
			int	typ = GF_LITE_WEAK;
			int dam = 40 + plev + chr_adj;
			if(!is_daytime()) dam /= 2;
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1)
			{
				typ = GF_LITE;
				dam += plev * 3 / 5;
			}

			if(only_info) return format("損傷:%d",dam);
			msg_format("あなたは眩く光った！");
			project_hack2(typ,0,0,dam);
			lite_room(py,px);
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-250);

			break;
		}
	case 4:
		{
			int base = 5+plev/5;
			if(is_daytime()) base *= 2;
			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_format("あなたは光を操って姿を消した！");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			break;
		}
	case 5:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 5 + chr_adj * 5;
			if(!is_daytime()) damage /= 2;
			if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) damage = damage * 3 / 2;

			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sに近寄り、突然太陽のように光った！",m_name);
				lite_room(py,px);

				if(!(r_ptr->flagsr & (RFR_RES_LITE | RFR_RES_ALL)))
				{
					if(!(r_ptr->flags3 & RF3_NO_CONF))
					{
						msg_format("%sは混乱した！",m_name);
						(void)set_monster_confused(cave[y][x].m_idx,  MON_CONFUSED(m_ptr)+10 + randint1(plev/2));
					}
					if(!(r_ptr->flags3 & RF3_NO_STUN))
					{
						msg_format("%sは朦朧とした！",m_name);
						(void)set_monster_stunned(cave[y][x].m_idx,  MON_STUNNED(m_ptr)+10 + randint1(plev/2));
					}
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_LITE,PROJECT_KILL,-1);

				if (p_ptr->magic_num1[0] > SUNNY_CHARGE_SUNLIGHT_1) sunny_charge_sunlight(-1000);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}



	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::ルナ専用技*/
class_power_type class_power_lunar[] =
{

	{9,6,20,FALSE,TRUE,A_INT,0,4,"ルナティックレイン",
		"隣接した敵全てに暗黒属性攻撃を行う。昼間は威力が落ちる。"},
	{18,16,40,FALSE,TRUE,A_INT,0,0,"静寂Ⅰ",
		"一定時間、自分自身と近くのモンスターが魔法と一部の行動を使えなくなる。巻物を読むと効果が解除され少し消費行動時間が増える。"},
	{18,0,0,FALSE,TRUE,A_INT,0,0,"静寂解除",
		"特技「静寂」の効果を解除する。この行動によるターン消費は通常の半分。"},
	{27,35,50,FALSE,TRUE,A_WIS,0,5,"ムーンライトウォール",
		"幅の広い暗黒属性のレーザー攻撃を放つ。昼間は威力が落ちる。"},
	{36,44,75,FALSE,TRUE,A_INT,0,7,"ルナサイクロン",
		"ターゲットの位置に暗黒属性のボールを発生させる。昼間は威力が落ちる。"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"静寂Ⅱ",
		"一定時間、フロア全域のモンスターが魔法と一部の行動を使えなくなる。自分は制限を受けない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_lunar(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dam = 10 + plev * 4 + chr_adj * 3;
			if(is_daytime()) dam /= 2;

			if(only_info) return format("損傷:%d ",dam/2);
			msg_print("月光が降り注いだ・・");
			project(0, 1, py, px, dam,GF_DARK, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);

			break;
		}
	case 1://静寂1 tim_general[0]を使う
		{
			int time = 10+plev/5;
			int rad = plev / 5;
			if(only_info) return format("期間:%d 範囲:%d",time,rad);
			if(p_ptr->tim_general[0] || p_ptr->tim_general[1])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("あなたは指を鳴らした。周囲の音が消えた。");
			set_tim_general(time,FALSE,0,0);

		}
		break;
	case 2://静寂解除
		{
			if(only_info) return format("");
			if(!p_ptr->tim_general[0] && !p_ptr->tim_general[1])
			{
				msg_print("能力を使っていない。");
				return NULL;
			}

			msg_print("あなたは能力を解除した。");
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);

		}
		break;

	case 3:
		{
			int damage = 30 + p_ptr->lev * 2 + chr_adj;
			if(is_daytime()) damage /= 2;

			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("月光の壁が前方を薙ぎ倒した。");
			fire_spark(GF_DARK,dir,damage,2);

			break;
		}
	case 4:
		{
			int base = p_ptr->lev * 5;
			int sides = chr_adj * 6;
			if(is_daytime()) base /= 2;

			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_DARK, dir, base + randint1(sides), 3,"月光が束になって渦を巻いた！")) return NULL;
			break;
		}

	case 5://静寂2 tim_general[1]を使う
		{
			int time = 30;
			if(only_info) return format("期間:%d",time);
			if(p_ptr->tim_general[1])
			{
				msg_print("すでに能力を使っている。");
				return NULL;
			}

			msg_print("あなたはフロア全体の音に干渉した。");
			set_tim_general(time,FALSE,1,0);
			p_ptr->tim_general[0]=0;
		}
		break;


	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::スター専用技*/
class_power_type class_power_star[] =
{
	{7,6,15,FALSE,TRUE,A_WIS,0,2,"プチコメット",
		"ターゲットの位置に半径0の隕石属性のボールを発生させる。"},
	{13,12,25,FALSE,TRUE,A_INT,0,3,"スターライトレイン",
		"ターゲット付近に閃光属性のボルトを連射する。"},
	{21,16,50,FALSE,TRUE,A_INT,0,5,"スターレーザー",
		"閃光属性のビームを放つ。"},
	{30,20,0,FALSE,FALSE,A_DEX,0,0,"爆弾設置",
		"足元に爆弾を設置する。爆弾は「通常は」10ターンで爆発し半径3の破片属性の強力なボールを発生させる。この爆発ではプレイヤーもダメージを受ける。爆弾は一部の属性攻撃に晒されると誘爆したり爆発タイミングが変わることがある。"},
	{42,60,80,FALSE,TRUE,A_WIS,0,0,"シューティングサファイア",
		"万能属性のロケット攻撃を放つ。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_star(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int base = 20+p_ptr->lev;
			int sides = chr_adj * 2;

			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 0,"星のかけらが落ちた。")) return NULL;
			break;
		}
	case 1:
		{
			int dice = 4 + plev / 10;
			int sides = 2 + chr_adj / 20;
			int num = 8 + plev / 9 ;
			if(only_info) return format("損傷:%dd%d * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("星屑が降り注いだ。");
			fire_blast(GF_LITE, dir, dice, sides, num, 3, 0);
			break;
		}
	case 2:
		{
			int damage = plev * 3 + chr_adj * 5 / 3;

			if(only_info) return format("損傷:%d",damage);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("星の光が流れた。");
			fire_beam(GF_LITE, dir, damage);
			break;
		}
	case 3: //爆弾設置　布都の皿と同じ扱いにする
		{
			int dam = 100+plev*4;
			if(only_info) return format("損傷:%d",dam);
			if(!futo_can_place_plate(py,px) || cave[py][px].special)
			{
				msg_print("ここには爆弾を置けない。");
				return NULL;
			}
			msg_print("爆弾を仕掛けた。");
			/*:::Mega Hack - cave_type.specialを爆弾のカウントに使用する。*/
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_bomb;
			cave[py][px].special = 10;
			while(one_in_(5)) cave[py][px].special += randint1(20);

			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	case 4:
		{
			base = plev * 5;
			sides = chr_adj * 6;

			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたは眩い煌きを放った！");
			fire_rocket(GF_PSY_SPEAR, dir, base+randint1(sides),2);
			break;
		}


	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::布都専用技*/
class_power_type class_power_futo[] =
{
	{5,2,0,FALSE,TRUE,A_DEX,0,0,"皿設置",
		"その場に皿を設置する。皿は攻撃やテレポートの基点として使われる。皿代として$20消費する。"},
	{5,7,20,FALSE,TRUE,A_INT,0,3,"六壬神火",
		"火炎属性のビームを放つ。ビームはターゲットを指定することができず、周囲に皿があれば皿へ飛び、周囲に皿がなければ近くの敵へと飛ぶ。"},
	{12,5,30,FALSE,TRUE,A_DEX,0,0,"皿投げ",
		"皿を投げて破片属性のダメージを与える。投げた皿はその場に残る。皿代として$50消費する。"},
	{16,50,60,FALSE,TRUE,A_DEX,0,0,"エンシェントシップ",
		"特殊モンスター「エンシェントシップ」を呼び出す。これに騎乗することができ、騎乗熟練度が低くても落馬や減速をしにくい。" },
	{20,20,40,FALSE,TRUE,A_INT,0,0,"風の凶穴",
		"近くの皿のある場所を選びテレポートする。テレポートした先で竜巻属性のボールを発生させ、その皿は破壊される。"},
	{24,22,40,FALSE,TRUE,A_WIS,0,5,"抱水皿",
		"周囲の皿を破壊し、皿のあった場所に水属性の小さなボールを発生させる。"},
	{30,24,50,FALSE,TRUE,A_DEX,0,0,"三輪の皿嵐",
		"風の刃で周囲を攻撃し、さらに周囲のランダムな場所に皿を数枚配置する。皿代として$500消費する。"},
	{36,36,65,FALSE,TRUE,A_WIS,0,10,"貴竜の矢",
		"無属性のビームを放つ。ビームが皿に当たると皿は特殊な爆発をして破壊され、元のビームは近くの皿へ向かい皿がなければ敵に向かう。弓を装備していないと使えないが矢を消費せず、弓が強力なほど威力が上がる。"},
	{40,80,75,FALSE,TRUE,A_DEX,0,0,"物部の八十平瓮",
		"視界内に破片属性攻撃を行い、大量の皿を配置する。皿代として$5000消費する。"},
	{44,96,80,FALSE,TRUE,A_INT,0,0,"破局の開門",
		"近くの皿が全て破壊され、近くのランダムな敵一体が不幸に見舞われる。破壊された皿の数が多いほど大変な不幸が降りかかる。"},
	{47,160,85,FALSE,TRUE,A_CHR,0,0,"大火の改新",
		"視界内の全てに対し強力な火炎攻撃を行い、さらに床を溶岩にする。視界内に皿があると全て破壊されその分炎の威力が増す。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

//-Mega Hack-
//六壬神火と貴竜の矢の止まるビームなどの表現のためPROJECT_FUTOを使う。
cptr do_cmd_class_power_aux_futo(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int charge = 20;
			if(only_info) return "";
			if(!futo_can_place_plate(py,px))
			{
				msg_print("ここには皿を置けない。");
				return NULL;
			}

			if(p_ptr->au < charge)
			{
				msg_print("皿代がない・・");
				return NULL;
			}
			else
			{
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;
			}
			msg_print("皿を置いた。");
			/* Create a glyph */
			cave[py][px].info |= CAVE_OBJECT;
			cave[py][px].mimic = feat_plate;
			note_spot(py, px);
			lite_spot(py, px);

		}
		break;
	case 1:
		{
			int dam = 15 + plev*3/2 + chr_adj;
			//int ty,tx;

			if(only_info) return  format("損傷:%d",dam);


			//v1.1.44 再帰処理のしすぎでフリーズしたので処理を見直した
			msg_print("火球を放った！");
			if (!execute_restartable_project(1, 0, 0, 0, 0, dam, GF_FIRE, 0))
				msg_print("しかしターゲットがいなかった。");

			/*
			if(futo_determine_target(py,px,&ty,&tx))
			{
				msg_print("火球を放った！");
				project(0,0,ty,tx,dam,GF_FIRE,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			}
			else
			{
				msg_print("火球を出したがターゲットがいなかった。");
			}
			*/

		}
		break;
	case 2:
		{
			int dam = 10 + plev * 3 / 2 + chr_adj / 2;
			int charge = 50;

			if(use_itemcard) charge = 0;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(p_ptr->au < charge)
			{
				msg_print("皿代がない・・");
				return NULL;
			}
			else
			{
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;
			}

			msg_print("皿を投げた！");
			fire_rocket(GF_THROW_PLATE, dir, dam,0);
		}
		break;
	case 3:
	{
		int i;
		u32b mode = PM_FORCE_PET;
		bool flag_exist = FALSE;
		if (only_info) return "";

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if ((m_ptr->r_idx == MON_ANCIENT_SHIP)) flag_exist = TRUE;
		}
		if (flag_exist)
		{
			msg_print("すでに舟を呼び出している。");
			return NULL;
		}
		if (summon_named_creature(0, py, px, MON_ANCIENT_SHIP, mode))
			msg_print("宙に浮く舟が現れた。");
		else
			msg_print("舟は出てこなかった。");

		break;

	}

	case 4:
		{
			int range = plev;
			int dam = plev * 4;
			if(only_info) return format("距離:%d 損傷:～%d",range,dam);
			if (p_ptr->anti_tele)
			{
				msg_format("今はうまく移動できないようだ。");
				return NULL;
			}
			if (!dimension_door(D_DOOR_FUTO)) return NULL;
			break;
		}
	case 5:
		{
			int x,y;
			int dam = plev * 2 + chr_adj * 5 / 3;
			int rad = plev / 6;
			bool flag = FALSE;
			if(only_info) return format("効果範囲:%d 損傷:%d",rad,dam);

			for(y=py-rad;y<=py+rad;y++)
			{
				for(x=px-rad;x<=px+rad;x++)
				{
					if (!in_bounds(y, x)) continue;
					if(distance(py, px, y, x) > rad) continue;
					if(!is_plate_grid(&cave[y][x])) continue;
					if(!projectable(py,px,y,x)) continue;
					futo_break_plate(y,x);
					flag = TRUE;
					project(0,1,y,x,dam,GF_WATER,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP),-1);
				}
			}
			if(!flag) msg_print("近くに皿がないので何も起こらなかった。");
		}
		break;

	case 6:
	{
		int charge = 500;
		int dam = 110 + plev * 4 + chr_adj * 3;
		int rad = 2 + plev / 40;
		int plate_num = 1 + (plev+5)/10;
		int j;

		if(only_info) return format("損傷:～%d",dam/2);

		if(p_ptr->au < charge)
		{
			msg_print("皿代がない・・");
			return NULL;
		}
		else
		{
			p_ptr->au -= charge;
			p_ptr->redraw |= PR_GOLD;
		}

		msg_print("数枚の皿が旋風を巻き起こした！");
		project(0,rad,py,px,dam,GF_WINDCUTTER,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP),-1);
		for(i=0;i<plate_num;i++)
		{
			int attempt = 100;
			int dy,dx;

			for(j=0;j<attempt;j++)
			{
				scatter(&dy, &dx, py, px, rad, 0);
				if(!futo_can_place_plate(dy,dx)) continue;

				cave[dy][dx].info |= CAVE_OBJECT;
				cave[dy][dx].mimic = feat_plate;
				note_spot(dy, dx);
				lite_spot(dy, dx);

				break;
			}
		}
	}
	break;
	case 7:
		{
			object_type *o_ptr;
			int mult;
			int dam;
			int ty,tx;
			int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_FUTO;
			bool dummy;

			if(inventory[INVEN_LARM].tval == TV_BOW || inventory[INVEN_RARM].tval == TV_BOW)
			{
				if(inventory[INVEN_LARM].tval == TV_BOW)
					o_ptr = &inventory[INVEN_LARM];
				else
					o_ptr = &inventory[INVEN_RARM];

				mult = bow_tmul(o_ptr->tval,o_ptr->sval);
				if (p_ptr->xtra_might) mult++;
				dam = (plev/3 + o_ptr->to_d + chr_adj / 5) * mult;
				if(dam < 1) dam=1;
			}
			else dam = 0;

			if(only_info) return  format("損傷:%d",dam);
			if(!dam)
			{
				msg_print("弓を装備していない。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			//v1.1.44 再帰処理のしすぎでフリーズしたので処理を見直した
			msg_print("光の矢を放った。");
			execute_restartable_project(1, dir, 0, 0, 0, dam, GF_DISP_ALL, 5);

/*

			msg_print("光の矢を放った。");
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			project(0, 5, ty, tx, dam, GF_DISP_ALL, flg, -1);
*/
		}
		break;
	case 8:
	{
		int x,y;
		int charge = 5000;
		int dam = plev * 2 + chr_adj *2;
		if(only_info) return format("損傷:1d%d",dam);

		if(p_ptr->au < charge)
		{
			msg_print("皿代がない・・");
			return NULL;
		}
		else
		{
			p_ptr->au -= charge;
			p_ptr->redraw |= PR_GOLD;
		}

		msg_print("あなたは大量の皿を投げ放った！");
		project_hack2(GF_SHARDS,1,dam,0);

		for(y=py-MAX_RANGE;y<=py+MAX_RANGE;y++)
		{
			for(x=px-MAX_RANGE;x<=px+MAX_RANGE;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!one_in_(6)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > MAX_RANGE) continue;
				if(!futo_can_place_plate(y,x)) continue;

				cave[y][x].info |= CAVE_OBJECT;
				cave[y][x].mimic = feat_plate;
				note_spot(y, x);
				lite_spot(y, x);

			}
		}
	}
	break;


	case 9:
	{
		int range = 9;
		int x, y, tx, ty;
		int count_plate = 0;
		int count_enemy = 0;

		if(only_info) return format("効果範囲:%d",range);

		for(y=py-range;y<=py+range;y++)
		{
			for(x=px-range;x<=px+range;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > range) continue;

				if(cave[y][x].m_idx)
				{
					count_enemy++;

					if(one_in_(count_enemy))
					{
						ty = y;
						tx = x;
					}
				}

				if(is_plate_grid(&cave[y][x]))
				{
					count_plate++;
					futo_break_plate(y,x);
				}
			}
		}

		if(!count_enemy)
		{
			msg_print("しかし周囲に敵がいなかった。");
		}
		else
		{
			int dam,typ,rad;
			cptr desc;
			char m_name[80];
			monster_type *m_ptr;
			m_ptr = &m_list[cave[ty][tx].m_idx];
			monster_desc(m_name, m_ptr, 0);

			if(count_plate < 5)
			{
				if(one_in_(3)) desc = "タライ";
				else if(one_in_(2)) desc = "花瓶";
				else desc = "オタマ";
				dam = damroll(3,3);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 10)
			{
				if(one_in_(3)) desc = "植木鉢";
				else if(one_in_(2)) desc = "鬼瓦";
				else desc = "一升瓶";
				dam = damroll(7,7);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 20)
			{
				if(one_in_(2)) desc = "タンス";
				else desc = "信楽焼の狸";
				dam = damroll(10,10);
				rad = 0;
				typ = GF_MISSILE;
			}
			else if(count_plate < 30)
			{
				if(one_in_(2))desc = "要石";
				else  desc = "墓石";
				dam = damroll(16,16);
				rad = 0;
				typ = GF_ARROW;
			}
			else if(count_plate < 60)
			{
				desc = "廃電車";
				dam = damroll(33,33);
				rad = 2;
				typ = GF_MISSILE;
			}
			else if(count_plate < 100)
			{
				desc = "隕石";
				dam = damroll(50,50);
				rad = 3;
				typ = GF_METEOR;
			}
			else if(count_plate < 200)
			{
				desc = "未確認飛行物体";
				dam = damroll(66,66);
				rad = 4;
				typ = GF_METEOR;
			}
			else
			{
				desc = "聖輦船";
				dam = damroll(88,88);
				rad = 7;
				typ = GF_DISINTEGRATE;
			}
			msg_format("%sの頭上から%sが降ってきた！",m_name,desc);
			project(0,rad,ty,tx,dam,typ,(PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL),-1);


		}

	}
	break;



	case 10:
	{
		int x, y;
		int count_plate = 0;
		int dam = plev * 6 + chr_adj * 10;

		if(only_info) return format("損傷:%d+α",dam);

		for(y=py-MAX_RANGE;y<=py+MAX_RANGE;y++)
		{
			for(x=px-MAX_RANGE;x<=px+MAX_RANGE;x++)
			{
				if (!in_bounds(y, x)) continue;
				if(!projectable(py,px,y,x)) continue;
				if(distance(py, px, y, x) > MAX_RANGE) continue;
				if(!is_plate_grid(&cave[y][x])) continue;
				count_plate++;
				futo_break_plate(y,x);
			}
		}

		dam += count_plate * 7;
		if(dam > 9999) dam = 9999;
		msg_print("辺り一面が火の海となった！");
		project_hack2(GF_LAVA_FLOW,1,4,0);
		project_hack2(GF_FIRE,0,0,dam);


	}
	break;

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::神奈子専用技*/
class_power_type class_power_kanako[] =
{
	{1,0,0,FALSE,TRUE,A_WIS,0,0,"信仰をチェックする",
		"あなたが今幻想郷でどの程度の信仰を集めているか確認する。"},
	{8,6,20,FALSE,TRUE,A_CHR,0,1,"蛇召喚",
		"蛇型のモンスターを配下として召喚する。"},
	{16,8,30,FALSE,TRUE,A_STR,0,6,"エクスパンデッド・オンバシラ",
		"敵を吹き飛ばす無属性のビームを放つ。テレポート耐性を無視するが巨大な敵は吹き飛ばない。信仰が集まると威力と射程が上昇する。"},
	{20,16,40,FALSE,TRUE,A_INT,0,0,"神の粥",
		"占いにより周囲のアイテムの場所を知る。信仰が集まると感知できるものが増える。また空腹のときは少し満腹度が回復する。"},
	{24,12,50,FALSE,TRUE,A_CHR,0,0,"神が歩かれた御神渡り",
		"トラップとドアを破壊するビームを放つ。信仰がかなり集まるとビームで壁を掘るようになる。"},
	{30,24,60,FALSE,TRUE,A_DEX,0,8,"御射山御狩神事",
		"周囲のランダムな敵に向け数発の無属性ボルト攻撃を行う。信仰が集まると弾数が増える。"},
	{36,48,65,FALSE,TRUE,A_WIS,0,0,"マウンテン・オブ・フェイス",
		"一定時間パワーアップする。信仰が集まるとさらに強力なパワーアップをするようになる。"},

	{40,72,70,FALSE,TRUE,A_CHR,0,1,"神格召喚",
		"配下として神格を召喚する。"},

	{45,96,80,FALSE,TRUE,A_STR,0,0,"メテオリックオンバシラ",
		"周囲のランダムな場所に隕石属性の強力なボールを連続で発生させる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_kanako(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int rank;

	if(use_itemcard) rank = 5;
	else rank = kanako_rank();

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			msg_format("%s",kanako_comment());
			return NULL; //行動順消費しない
		}
		break;
	case 1:
		{
			int num = 1 + plev / 5 + rank * 2;
			bool flag = FALSE;
			if(only_info) return "";
			if(num > 20) num = 20;
			for(i=0;i<num ;i++)
			{
				if ((summon_specific(-1, py, px, plev, SUMMON_SNAKE, (PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("蛇を呼び出した！");
			else
				msg_print("蛇は現れなかった。");
		}
		break;

	case 2:
		{
			int dist = MIN(15,(3 + rank * 2));

			dice = 4 + plev / 6;
			sides = 6 + chr_adj / 5;
			base = 30 + plev * rank / 2;

			if(only_info) return  format("射程:%d 損傷:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("カードから大きな柱が飛び出した！");
			else
				msg_format("御柱を投げ放った！");
			fire_beam(GF_TRAIN, dir, base + damroll(dice,sides));

		}
		break;
	case 3:
		{
			int rad = 10 + plev / 3;

			if(only_info) return  format("");
			if(use_itemcard)
				msg_format("筒粥を食べた。");
			else
				msg_format("あなたは筒粥を炊き始めた・・");

			if(rank > 6)
			{
				wiz_lite(FALSE);
			}
			else
			{
				if(rank > 0) detect_objects_normal(rad);
				if(rank > 4) map_area(rad);

			}
			detect_objects_gold(rad);
			if(rank > 1) detect_traps(rad, TRUE);
			if(rank > 2) detect_monsters_normal(rad);
//筒粥神事って食べるんだっけ？調べ直しとく
			if (p_ptr->food < PY_FOOD_FULL) set_food(p_ptr->food + 3000);
		}
		break;
	case 4:
		{
			int dist = MIN(15,(6 + rank * 2));
			if(only_info) return  format("射程:%d",dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("眼前に道が開かれた。");
			if(rank > 4) project_hook(GF_KILL_WALL, dir, 0, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI));
			project_hook(GF_KILL_TRAP, dir, 0, (PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE));
		}
		break;

	case 5:
		{
			bool flag = FALSE;
			int num;
			int i;
			dice = 1;
			sides = p_ptr->lev + chr_adj / 2;
			num = rank * rank / 3 + 4;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);

			msg_format("雨のように矢を降らせた！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MISSILE,damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
		}
		break;
	case 6:
		{
			int time = 30;
			int dec_hp;

			if(only_info) return format("期間:%d",time);

			if(one_in_(3)) msg_print("御柱が飛来し背に装着された！");
			else if(one_in_(2)) msg_print("皆の信仰の力が流れ込んでくる！");
			else msg_print("あなたの体は信仰の力で七色に輝いた！");

			if(rank > 0)
			{
				set_afraid(0);
				set_hero(time,FALSE);
			}
			if(rank > 1 && rank < 7)
			{
				set_tim_addstat(A_STR,5,time,FALSE);
			}
			if(rank > 2)
			{
				set_protevil(time,FALSE);
			}
			if(rank > 3)
			{
				set_shield(time,FALSE);
			}
			if(rank > 4)
			{
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
			if(rank > 5)
			{
				set_magicdef(time, FALSE);
			}
			if(rank > 6)
			{
				dec_hp = p_ptr->mhp - p_ptr->chp;
				set_tim_addstat(A_STR,107,time,FALSE);
				set_tim_addstat(A_DEX,107,time,FALSE);
				set_tim_addstat(A_CON,107,time,FALSE);
				p_ptr->chp = p_ptr->mhp - dec_hp;
				p_ptr->redraw |= PR_HP;
				handle_stuff();
			}
		}
		break;

	case 7:
		{
			int num = 1 + rank / 2;
			bool flag = FALSE;
			if(only_info) return "";
			if(rank == 7) num += 2;
			for(i=0;i<num ;i++)
			{
				if ((summon_specific(-1, py, px, plev * 3 / 2, SUMMON_DEITY, (PM_FORCE_PET)))) flag = TRUE;
			}
			if(flag)
				msg_print("神々があなたの呼びかけに応えた！");
			else
				msg_print("誰も現れなかった。");
		}
		break;


	case 8:
		{
			int rad = 1 + (rank + 1) / 2;
			int dam = 1 + rank * rank * 5;
			if(only_info) return format("損傷:%d*不定",dam);
			msg_print("巨大な御柱が流星のように天から降り注いだ！");
			cast_meteor(dam, rad,GF_METEOR);

		}
		break;

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::妹紅専用技*/
//magic_num1[0]に復活回数、magic_num2[0]にインペリシャブルシューティング用フラグ
class_power_type class_power_mokou[] =
{
	{1,50,0,FALSE,TRUE,A_CON,0,0,"リザレクション",
		"その場に巨大な火炎属性のボールを発生させ、HPと全ての状態異常を回復する。スコアが下がり、炎に弱い所持品が燃えることがある。この特技はHPが0未満になったときにも自動的に発動するが、MPが不足しているとHPの回復率が下がり、MP25以下だと発動できない。"},
	{5,20,40,TRUE,TRUE,A_DEX,0,0,"火焔鳥",
		"今いる場所を明るく照らし、一時的に浮遊する。"},
	{12,11,40,FALSE,TRUE,A_DEX,0,5,"鳳翼天翔",
		"敵や障害物に当たると爆発する火炎属性の球を放つ。レベル40以降は核熱属性になる。"},
	{16,12,30,FALSE,TRUE,A_WIS,0,0,"月のいはかさの呪い",
		"周囲の敵の動きを止める。ユニークモンスターには効果がない。"},
	{20,20,40,FALSE,TRUE,A_CON,0,10,"自滅火焔大旋風",
		"自分を中心に強力な火炎属性のボールを発生させる。自分もダメージを受ける。炎に弱い所持品が燃えることがある。"},
	{24,30,45,FALSE,TRUE,A_INT,0,4,"無差別発火の符",
		"周囲のランダムな場所に爆発のルーンを配置する。"},
	{28,50,50,FALSE,TRUE,A_CHR,0,0,"フェニックスの尾",
		"隣接した敵一体に火炎属性攻撃を行い、非ユニークのアンデッドを高確率で一撃で倒す。"},
	{33,100,60,FALSE,TRUE,A_INT,0,0,"パゼストバイフェニックス",
		"フロアの好きな場所に移動してリザレクションする。移動先に敵がいた場合核熱属性の大ダメージを与える。テレポートで入れない場所には侵入できない。スコアが下がる。"},
	{40,400,70,TRUE,TRUE,A_STR,0,100,"フジヤマヴォルケイノ",
		"視界内の全てに対し強力な火炎属性攻撃を行う。HPを大量に消費する。"},
	{44,160,80,FALSE,TRUE,A_CON,0,0,"フェニックス再誕",
		"リザレクションと同じ効果の後、一時的に全能力が大幅に上昇する。スコアが下がる。"},
	{47,0,80,FALSE,TRUE,A_CHR,0,0,"インペリシャブルシューティング",
		"近くの敵に強力な無属性攻撃をし続ける。攻撃のたびにMP30を消費し、MPが尽きるかフロアを移動するまで解除できない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_mokou(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dam = (p_ptr->lev * 8 + chr_adj * 10 + 300);
			if(only_info) return format("損傷:最大%d",dam/2);

			mokou_resurrection();

			break;
		}
	case 1:
		{
			int time;
			base = 15 + plev / 2;
			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_print("燃え盛る炎の翼が背に現れた。");
			lite_room(py,px);
			set_tim_levitation(time,FALSE);

		}
		break;
	case 2:
		{
			int dam = 30 + plev * 4 + chr_adj * 3 ;
			int rad = 1 + plev / 24;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("カードから火の鳥が現れて飛んで行った！");
			else
				msg_print("火の鳥が飛んだ！");
			fire_rocket((plev > 39)?GF_FIRE:GF_NUKE, dir, dam, rad);
		}
		break;
	case 3:
		{
			int power = 50 + plev * 2;
			if(only_info) return format("効果:%d",power);
			msg_print("ダンジョンが異質な雰囲気に包まれた。");
			stasis_monsters(power);
			break;

		}
	case 4:
		{
			int dam = plev * 15 + chr_adj * 15;
			int rad = plev / 10;

			if(only_info) return format("範囲:%d 損傷:中心地で%d ",rad,dam);

			msg_print("火炎の大旋風を巻き起こした！");
			project(PROJECT_WHO_SUICIDAL_FIRE, rad, py, px, dam,GF_FIRE, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
			inven_damage(set_fire_destroy, 2);

			break;
		}
	case 5:
		{
			int xx,yy;
			int num = plev / 5;
			int rad = plev / 10;
			if(num < 3) num = 3;
			if(rad < 2) rad = 2;
			if(only_info) return format("個数:最大%d ",num);

			msg_print("発火の符を撒き散らした！");
			for(i=0;i<num;i++)
			{
				scatter(&yy,&xx,py,px,rad,0);
				if(!cave_clean_bold(yy,xx)) continue;
				cave[yy][xx].info |= CAVE_OBJECT;
				cave[yy][xx].mimic = feat_explosive_rune;
				note_spot(yy, xx);
				lite_spot(yy, xx);
			}
		}
		break;
	case 6:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 3 + chr_adj * 5;
			if(damage < 100) damage = 100;
			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sに不死鳥の羽が突き刺さった。",m_name);

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags3 & RF3_UNDEAD)
					&& (randint1(r_ptr->level) + 20 < plev))
				{
					damage = m_ptr->hp + 1;
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}
				else
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_FIRE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
	case 7:
		{
			int damage = (p_ptr->lev * 6 + chr_adj * 10);
			if(only_info) return format("損傷:%d",damage);
			if (p_ptr->anti_tele)
			{
				msg_format("今はうまく移動できないようだ。");
				return NULL;
			}
			if (!dimension_door(D_DOOR_MOKOU)) return NULL;
			break;
		}

	case 8:
		{
			int dam = plev * 12 + chr_adj * 20;
			if(dam < 600) dam = 600;
			if(only_info) return format("損傷:%d",dam);
			msg_format("爆音と共に周囲が炎に包まれた！");
			project_hack2(GF_FIRE,0,0,dam);
			break;
		}
	case 9:
		{
			int time,percentage;
			base = 25 + plev / 2;
			if(only_info) return format("期間:%d+1d%d",base,base);

			mokou_resurrection();
			time = base + randint1(base);
			msg_print("あなたはより強くなって蘇った！");
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			time = base + randint1(base);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_tim_addstat(A_INT,110,time,FALSE);
			set_tim_addstat(A_WIS,110,time,FALSE);
			set_tim_addstat(A_DEX,110,time,FALSE);
			set_tim_addstat(A_CON,110,time,FALSE);
			set_tim_addstat(A_CHR,110,time,FALSE);
			set_fast(time, FALSE);
			set_hero(time,FALSE);
			set_tim_speedster(time, FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP | PR_MANA);
			redraw_stuff();

		}
		break;
	case 10:
		{
			int dam = plev * 5 + chr_adj * 5;

			if(only_info) return format("損傷:毎時最大%d+1d%d ",dam/2,dam/2);
			p_ptr->magic_num2[0] = 1;//フラグ

			break;
		}



	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::一輪専用技*/
class_power_type class_power_ichirin[] =
{
	{5,8,10,TRUE,TRUE,A_STR,0,5,"問答無用の妖怪拳",
		"少し離れた場所に攻撃する。通常攻撃の威力が上がるとダメージが上がる。"},
	{12,8,20,FALSE,TRUE,A_WIS,0,4,"時代親父大目玉",
		"閃光属性のビームを放つ。"},
	{18,20,30,FALSE,TRUE,A_DEX,0,3,"殴殺の流雲",
		"自分の隣に竜巻属性のボールを発生させる。巨大な敵や空を飛ぶ敵には効果が薄い。"},
	{25,35,45,TRUE,TRUE,A_STR,0,10,"雲上地獄突き",
		"敵や障害物に当たると爆発する気属性のボールを放つ。"},
	{31,35,60,FALSE,TRUE,A_WIS,0,2,"見越入道雲",
		"隣接した敵全てを攻撃し吹き飛ばす。テレポート耐性を無視するが巨大な敵は吹き飛ばない。"},
	{36,50,70,FALSE,TRUE,A_WIS,0,10,"天空鉄槌落とし",
		"隕石属性のボールを発生させる。通常攻撃の威力が上がるとダメージが上がる。"},
	{40,200,75,TRUE,TRUE,A_STR,0,50,"雲海クラーケン殴り",
		"攻撃回数2～3倍の通常攻撃を行う。HPを大幅に消費する。"},
	{45,72,85,FALSE,TRUE,A_CON,0,0,"華麗なる親父時代",
		"技使用後、全ての耐性を獲得し、全ての攻撃のダメージを半減し、敵からの隣接攻撃に対し反撃を行うようになる。ただしターン経過時と反撃時にMPを消費し、待機以外の行動をすると効果が切れる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_ichirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int x, y;
			int dist = 2 + plev / 15;
			monster_type *m_ptr;

			dice = plev / 3;
			sides = 10 + chr_adj / 10;
			base = MAX(p_ptr->to_d[0],p_ptr->to_d[1]);
			if(only_info) return format("射程:%d 損傷:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sの上に雲山の拳が落ちた！",m_name);
				project(0,0,y,x,damroll(dice,sides)+base,GF_ARROW,PROJECT_KILL,-1);
			}

			break;
		}


	case 1:
		{
			dice = 3 + p_ptr->lev / 5;
			sides = 10;
			base = p_ptr->lev + chr_adj;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("雲山が鋭い眼光を放った！");
			fire_beam(GF_LITE, dir, base + damroll(dice,sides));
			break;
		}

	case 2:
		{
			int y, x;
			int rad = 3 + plev / 16;

			base = plev * 3 + chr_adj * 2 - 10;
			if(base < 50) base = 50;

			if(only_info) return format("損傷:最大%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			if(in_bounds(y,x) && cave_have_flag_bold(y, x, FF_PROJECT))
			{
				if(use_itemcard)
					msg_format("暴風が起こった！");
				else
					msg_format("雲山が暴風を起こした！");
				project(0, rad, y, x, base, GF_TORNADO, PROJECT_KILL , -1);

			}
			else
			{
				msg_format("そこでは使えない。");
				return NULL;
			}
			break;
		}
	case 3:
		{
			int dam = 30 + plev * 3 + chr_adj * 2 ;
			int rad = 1 + plev / 40;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;

			if(use_itemcard)
				msg_format("カードから拳のような雲が現れ飛んでいった！");
			else
				msg_print("雲山の拳が飛んだ！");
			fire_rocket(GF_FORCE, dir, dam, rad);
		}
		break;
	case 4:
		{
			int dam = plev * 7 + chr_adj * 5;
			int rad = 1;

			if(only_info) return format("損傷:%d ",dam/2);

			msg_print("雲山と共に激しく回転した！");
			project(0, rad, py, px, dam, GF_TRAIN, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			break;
		}
	case 5:
		{
			int x, y;
			int dist = plev / 4;
			int rad = 2;
			monster_type *m_ptr;

			dice = plev / 2;
			sides = 10;
			base = MAX(p_ptr->to_d[0],p_ptr->to_d[1]) * 3 + chr_adj * 3;
			if(only_info) return format("射程:%d 損傷:%d+%dd%d",dist,base,dice,sides);
			project_length = dist;

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}
			else
			{
				msg_format("雲山の巨大な拳が大地を穿った！");
				project(0,rad,y,x,damroll(dice,sides)+base,GF_METEOR,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
			}

			break;
		}
	case 6:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたと雲山は気合を溜めた…",m_name);
				py_attack(y,x,HISSATSU_UNZAN);
			}

			break;
		}
	case 7:
		{
			if(only_info) return format("");
			set_action(ACTION_KATA);
			p_ptr->special_defense |= KATA_MUSOU;
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			msg_format("あなたは仁王立ちで敵の攻撃を待ち構えた！");

		}
		break;


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::菫子専用技*/
class_power_type class_power_sumireko[] =
{
	{1,3,10,FALSE,TRUE,A_INT,0,2,"テレキネシス(投擲)",
		"アイテムを投擲する。射程距離はアイテムの重量に関わらず最大となる。レベルが上がると威力が上昇し、レベル35以降は投げた武器が戻ってくる。"},
	{6,4,25,FALSE,TRUE,A_WIS,0,2,"パイロキネシス",
		"やや射程が短い火炎属性のビームを放つ。"},
	{13,5,30,FALSE,TRUE,A_DEX,0,3,"遠隔攻撃",
		"少し離れた場所へ通常攻撃を行う。　武器を装備していると使えないが敵のオーラによるダメージを受けない。"},
	{16,10,40,FALSE,TRUE,A_INT,0,0,"指向性テレポート",
		"指定した方向へ短距離のテレポートを行う。通常の半分の行動力しか消費しない。テレポートできない場所にテレポートしようとした場合、その周辺のランダムな場所へ出るか元の場所に戻る。"},
	{20,12,40,FALSE,TRUE,A_INT,0,4,"アーバンサイコキネシス(吸引)",
		"視界内のアイテム数個を引き寄せ自分の周りを浮遊させる。浮遊しているアイテムの種類によって敵にオーラダメージを与えたりACが上昇する。"},
	{20,0,0,FALSE,TRUE,A_INT,0,0,"アーバンサイコキネシス(確認)",
		"周囲を浮遊しているアイテムの一覧を見る。行動力を消費しない。"},
	{20,0,0,FALSE,TRUE,A_INT,0,0,"アーバンサイコキネシス(中断)",
		"周囲を浮遊しているアイテムを全て地面に降ろす。"},
	{20,15,50,FALSE,TRUE,A_INT,0,5,"アーバンサイコキネシス(放出)",
		"周囲を浮遊しているアイテムをターゲットへ向けて全て投擲する。投げたら戻ってくる能力を持つアイテムは戻ってこない。"},
	{25,0,75,FALSE,TRUE,A_WIS,0,0,"精神集中",
		"MPをわずかに回復する。"},
	{30,22,65,FALSE,TRUE,A_WIS,0,6,"サイコプロ―ジョン",
		"自分の周囲のランダムな位置へ続けざまに爆発を起こし、敵を朦朧とさせたりテレポートさせる。"},
	{36,32,60,FALSE,TRUE,A_INT,0,0,"テレキネシス(不法投棄)",
		"指定した位置にいる敵に無属性攻撃を行う。敵がいない場合岩を生成する。"},
	{40,64,80,FALSE,TRUE,A_INT,0,0,"サイコキネシスアプリ",
		"視界内の全てに時空属性攻撃を行いどこかへ吹き飛ばす。スマートフォンを所持していないと使えないが、スマートフォンは幻想郷の妖怪達に追い掛け回されたときに落としてしまった。"},
	{45,96,90,FALSE,TRUE,A_CHR,0,0,"現し世のオカルティシャン",
		"視界内のランダムな敵に向け、強力な魔力属性のビームを数発放つ。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_sumireko(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int inven2_num = calc_inven2_num();

	switch(num)
	{

	case 0:
		{
			int mult = 1 + (plev + 10)/ 20;
			if(only_info) return format("倍率:%d",mult);
			if (!do_cmd_throw_aux(mult, FALSE, -2)) return NULL;
		}
		break;
	case 1:
		{
			int dist = 4 + plev / 8;
			int dice = 8 + plev / 2;
			int sides = 5;
			if(only_info) return format("損傷:%dd%d 射程:%d",dice,sides,dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("指を鳴らすと空気が燃え上がった。");
			fire_beam(GF_FIRE, dir, damroll(dice,sides));
			break;
		}
	case 2:
		{
			if(only_info) return format("");
			project_length = 2 + plev / 24;
			if (!get_aim_dir(&dir)) return FALSE;
			project_hook(GF_ATTACK, dir, HISSATSU_NO_AURA, PROJECT_STOP | PROJECT_KILL);
			break;
		}
	case 3:
	{
		int dist = 5 + plev / 10;
		if (only_info) return format("距離:%d", dist);

		if (!teleport_to_specific_dir(dist, 7, 0)) return NULL;

		new_class_power_change_energy_need = 50;
		break;
	}

	case 4:
		{
			int j;
			bool flag_pick = FALSE;
			bool flag_max = TRUE;
			object_type forge;
			object_type *o_ptr;
			object_type *q_ptr;
			char o_name[MAX_NLEN];

			if(only_info) return format("最大:%d個",inven2_num);
			//フロアのアイテムを全て調べる
			for(i=0;i<inven2_num;i++)
			{
				int item = 0;
				int item_count_temp = 0;
				if(inven_add[i].k_idx) continue;
				flag_max = FALSE;
				//視界内のアイテムをランダムに選定
				for(j=0;j<o_max;j++)
				{
					o_ptr = &o_list[j];
					if (!o_ptr->k_idx) continue;
					if (!projectable(o_ptr->iy,o_ptr->ix,py,px)) continue;

					item_count_temp++;
					if(one_in_(item_count_temp)) item = j;
				}
				if(!item) break;
				//選ばれたアイテムを追加インベントリに1つだけ格納し床上アイテムを減らす。所持品重量は考慮しない。
				flag_pick = TRUE;
				o_ptr = &o_list[item];
				q_ptr = &forge;
				object_copy(q_ptr, o_ptr);
				q_ptr->number = 1;
				distribute_charges(o_ptr, q_ptr, 1);
				object_desc(o_name, q_ptr, 0);
				msg_format("%sを吸い寄せた。",o_name);
				object_wipe(&inven_add[i]);
				object_copy(&inven_add[i], q_ptr);
				floor_item_increase(item, -1);
				floor_item_describe(item);
				floor_item_optimize(item);

				p_ptr->update |= PU_BONUS;
			}
			if(!flag_pick)
			{
				msg_print("近くには動かせる物がないようだ・・");
				break;
			}
			else if(flag_max)
			{
				msg_print("これ以上の数は扱いきれない。");
				return NULL;
			}


		}
		break;
	case 5:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //見るだけなので行動順消費なし
		}
		break;
	case 6:
		{
			object_type *o_ptr;
			bool flag_drop = FALSE;
			char o_name[MAX_NLEN];
			if(only_info) return format("");
			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				flag_drop = TRUE;
				o_ptr = &inven_add[i];
				object_desc(o_name, o_ptr, 0);
				msg_format("%sが落ちた。",o_name);
				drop_near(o_ptr,0,py,px);
				object_wipe(&inven_add[i]);
				p_ptr->update |= PU_BONUS;
			}
			if(!flag_drop)
			{
				msg_print("降ろすものがない。");
				return NULL;
			}
		}
		break;
	case 7:
		{
			int ty,tx;
			bool thrown = FALSE;
			object_type *o_ptr;
			char o_name[MAX_NLEN];
			if(only_info) return format("");

			for(i=0;i<inven2_num;i++)
				if(inven_add[i].k_idx) thrown = TRUE;
			if(!thrown)
			{
				msg_print("飛ばすものがない。");
				return NULL;
			}

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
			msg_print("周囲のアイテムを標的目掛けて一斉に飛ばした！");

			for(i=0;i<inven2_num;i++)
			{
				if(!inven_add[i].k_idx) continue;
				thrown = TRUE;
				o_ptr = &inven_add[i];
				p_ptr->update |= PU_BONUS;

				if(do_cmd_throw_aux2(py,px,ty,tx,1,o_ptr,0))
					object_wipe(&inven_add[i]);
			}
		}
		break;
	case 8:
		{
			if(only_info) return format("");
			msg_print("あなたはスプーンを取り出し、精神を集中した・・");
			p_ptr->csp += (4 + randint1(plev/10));
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;

	case 9:
		{
			int rad = 2 + plev / 40;
			int damage = plev / 2 + 35 ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("念動力の爆発を起こした！");
			cast_meteor(damage, rad, GF_TELEKINESIS);

			break;
		}
	case 10:
		{
			int y, x;
			monster_type *m_ptr;
			int damage = plev * 5;

			if(only_info) return format("損傷:%d",damage);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx)
			{
				if (!cave_naked_bold(y, x))
				{
					msg_print("そこには何かがあるようだ。やめておこう。");
					return NULL;
				}
				msg_print("念動力で岩塊や土塊を積み重ねた。");
				cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sへ向けて瓦礫が降り注いだ！",m_name);
				project(0,0,y,x,damage,GF_ARROW,PROJECT_KILL,-1);
			}

			break;
		}


	case 11:
		{
			int base = plev * 4;
			if(base < 150) base = 150;
			if(only_info) return format("損傷:%d+1d%d",base,base);
			if(use_itemcard)
				msg_format("カードをなぞるとそれに従って周囲のものが吹き飛んだ！");
			else
				msg_format("周りの一切合財を引き寄せて掻き回して吹っ飛ばした！");
			project_hack2(GF_DISTORTION,1,base,base);
			break;
		}
	case 12:
		{
			bool flag = FALSE;
			int i;
			int dice = plev / 5;
			int sides = 10 + chr_adj / 4;
			int num = 7;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("全身全霊の超能力を放った！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MANA, damroll(dice,sides),2, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}


	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}

	return "";
}












/*:::針妙丸2専用技*/
class_power_type class_power_shinmyou2[] =
{
	{8,4,20,FALSE,FALSE,A_DEX,0,3,"溜め突き",
		"射程の短いビームを放つ。威力は利き腕の通常攻撃力の1/3程度。利き腕に短剣・剣・刀・針を装備していないと使えない。"},
	{14,8,35,FALSE,FALSE,A_STR,0,0,"小人の一本釣り",
		"視界内の指定した敵にダメージを与えて近くのどこかへ飛ばす。テレポート耐性を無視するが巨大な敵は飛ばせない。釣りをすることができる武器を装備または所持していないと使えない。"},
	{20,11,40,FALSE,FALSE,A_DEX,0,3,"絵羽縫直し",
		"隣接したターゲットに攻撃し、さらに敵の次の行動をわずかに遅らせる。巨大な敵、力強い敵、ユニークモンスターには効果が薄い。利き腕に短剣・剣・刀を装備していないと使えない。"},
	{24,27,50,FALSE,TRUE,A_CHR,0,0,"可愛い太公望",
		"自分のいる場所に水属性のボールを発生させ、さらに地形を水にする。釣りをすることができる武器を装備または所持していないと使えない。"},

	{27,40,60,FALSE,TRUE,A_CHR,0,0,"飛び入り参加",
		"幻想郷の友好的なモンスターをその場に召喚する。特定の性格でのみ使用可能。反感状態では使えない。" },

	{30,20,60,FALSE,FALSE,A_DEX,0,6,"鬼ごろし両目突きの針",
		"近くのランダムなモンスターに向けて混乱属性のビームを連射する。ビームの威力と本数は通常攻撃に準じる。利き腕に短剣・剣・刀・針を装備していないと使えない。" },

	{35,35,60,FALSE,FALSE,A_DEX,0,10,"天衣百縫",
		"隣接した敵に、攻撃回数が1.5倍になりオーラダメージを受けない通常攻撃を行う。利き腕に短剣・剣・刀・針を装備していないと使えず、利き腕以外の攻撃は行われない。"},
	{40,75,75,FALSE,TRUE,A_WIS,0,0,"輝針剣",
		"ターゲット周辺に向けて防御不可能なビーム攻撃を連続で放つ。"},
	{45,80,80,FALSE,TRUE,A_CON,0,0,"一寸法師にも五分の魂",
		"小槌の力で短時間小型化する。小型化しているときは移動速度とACが低下するが隠密能力が大幅に上昇し、魔法やブレスなどの特殊攻撃を中確率で回避し、さらに巨大な敵から認識されにくくなる。道具類は通常通り使用できる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_shinmyou2(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int str_adj = adj_general[p_ptr->stat_ind[A_STR]];

	switch(num)
	{
	case 0:
		{
			int damage = 0;
			int range = 2 + plev / 20;

			damage = calc_weapon_dam(0) / 3;
			if (only_info) return format("射程:%d 損傷:%d",range,damage);

			if(!p_ptr->num_blow[0])
			{
				msg_print("ERROR:num_blow[0]が0");
				return NULL;
			}

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("鋭い突きを放った！");
			fire_beam(GF_MISSILE, dir, damage);
			break;
		}
	case 1:
		{
			int y, x;
			monster_type *m_ptr;
			int damage = 10 + plev + str_adj * 2;

			if(only_info) return format("損傷:%d",damage);

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}

			m_ptr = &m_list[cave[target_row][target_col].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sに針を引っ掛けた！",m_name);
				project(0,0,target_row,target_col,damage,GF_TRAIN,PROJECT_KILL,-1);
			}

			break;
		}



	case 3:
		{
			int dam = 40 + plev * 4 + chr_adj * 2;
			int rad = 1 + plev / 16;

			if(only_info) return format("損傷:～%d ",dam/2);

			msg_print("足元から水と魚が噴き出した！");

			project(0, rad, py, px, rad, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, dam, GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			break;
		}
		//v1.1.59　専用性格にて追加
	case 4:
		{
			int snum = plev / 7;
			int i;
			bool flag = FALSE;

			if (only_info) return format("");


			for (i = 0; i<snum; i++)
			{
				if (summon_specific(-1, py, px, plev+20, SUMMON_GENSOUKYOU, (PM_FORCE_FRIENDLY | PM_ALLOW_UNIQUE))) flag = TRUE;
			}
			if (flag) msg_format("あなたの呼びかけに幻想郷の住人たちが応えた！");
			else msg_format("誰もあなたの呼びかけに応えなかった...");

		}
		break;

	case 5:
	{
		int i;
		int damage = 0;
		int num_blow = p_ptr->num_blow[0];
		int range = 2 + plev / 20;
		int flag_attack_done = FALSE;

		if (num_blow < 1) num_blow = 1;

		damage = calc_weapon_dam(0) / num_blow;
		if (only_info) return format("射程:%d 損傷:%d*%d", range, damage,num_blow);

		msg_print("目を狙って突きかかった！");
		for (i = 0; i < num_blow; i++)
		{
			if(fire_random_target(GF_CONFUSION, damage, 2, 0, range)) flag_attack_done = TRUE;
		}
		if (!flag_attack_done)
			msg_print("しかし標的がいなかった。");
		break;
	}


	case 6:
	case 2:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else if(num == 2)
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sへ目にも留まらぬ連撃を放った！",m_name);
				py_attack(y,x,0);
				if(m_ptr->r_idx)
				{
					int delay = plev + randint1(plev);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) delay /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) delay -= 30;
					if(r_ptr->flags2 & RF2_POWERFUL) delay -= 20;
					if(delay < 0) delay = 0;
					m_ptr->energy_need += delay;

					if(!delay)
						msg_format("%sは全く怯んでいない！",m_name);
					else if(delay < 30)
						msg_format("%sの動きが一瞬止まった！",m_name);
					else
						msg_format("%sは攻撃に怯んでいる！",m_name);
				}
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sへ猛然と斬りかかった！",m_name);
				py_attack(y,x,HISSATSU_SHINMYOU);
			}

			break;
		}
	case 7: //輝針剣
		{
			int dice = p_ptr->lev / 5;
			int sides = 3 + chr_adj / 5;
			int num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 6;
			if(p_ptr->mimic_form == MIMIC_GIGANTIC) dice *= 2;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("眩く輝く%s刃を放った！",(p_ptr->mimic_form==MIMIC_GIGANTIC)?"巨大な":"");
			fire_blast(GF_PSY_SPEAR, dir, dice, sides, num, 2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));

			break;
		}

	case 8:
		{
			int base = 12;
			int sides = 12;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			msg_print("あなたは小槌で自分の頭を叩いた。");
			if(!set_mimic(base+randint1(base), MIMIC_SHINMYOU, FALSE)) return NULL;
			break;
		}



	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::映姫専用技*/
class_power_type class_power_eiki[] =
{
	{5,3,0,FALSE,TRUE,A_INT,0,0,"浄玻璃の鏡",
		"周囲の敵を調査し情報を得る。"},
	{12,5,0,FALSE,TRUE,A_INT,0,0,"閻魔の目",
		"周囲のトラップとモンスターを感知する。"},
	{18,13,40,FALSE,TRUE,A_WIS,0,7,"彷徨える大罪",
		"視界内のランダムな敵に対し破邪属性のボルトを数回放つ。"},
	{23,30,80,FALSE,TRUE,A_INT,0,0,"*鑑識*",
		"指定したアイテムの性能を完全に知る。"},
	{30,45,65,FALSE,TRUE,A_DEX,0,0,"タン・オブ・ウルフ",
		"隣接した敵一体の魔法を封じる。人間・アンデッド・混沌勢力の敵・泥棒に効きやすく、高レベルな敵・力強い敵・ユニークモンスターには効きにくい。アンデッド以外の無生物には効果がない。なお、魔法を封じられた敵もブレスやロケットは使える。"},
	{36,56,70,FALSE,TRUE,A_WIS,0,20,"十王裁判",
		"ターゲットの周辺に分解属性のボールを連続で発生させる。"},
	{42,80,75,FALSE,TRUE,A_INT,0,0,"浄頗梨審判",
		"対象の敵の幻影を配下として召喚する。幻影は1フロアに一体しか呼べず、命令に関わらず元になった敵のみを狙い、プレイヤーの視界から出たり元の敵が倒れると消える。アンデッド以外の無生物には効果がない。"},
/* やっぱ止めた
	{45,180,80,FALSE,TRUE,A_WIS,0,0,"ギルティ・オワ・ノットギルティ",
		"隣接した敵一体を高確率で倒す。ユニークモンスターはHPを半分にする。混沌勢力の敵に効きやすく、高レベルな敵や秩序勢力の敵には効きにくく、アンデッド以外の無生物には全く効果がない。同じフロアで同じ敵に二度以上使うことはできない。"},
*/
	{47,160,80,FALSE,TRUE,A_CHR,0,0,"ラストジャッジメント",
		"極めて強力な破邪属性のビームを放つ。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_eiki(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");
			msg_print("あなたは手鏡を取り出した。");
			probing();
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			msg_print("あなたは周囲を見渡した。");
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			break;
		}
	case 2:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 10;
			int sides = 3 + chr_adj / 10;
			int num = 3 + plev / 7;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("空間が歪み、悔悟の棒が何本も飛び出した。");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_HOLY_FIRE, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if (!identify_fully(FALSE)) return NULL;
			break;
		}
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				int chance = plev * 2;
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 3;
				if(r_ptr->flags3 & (RF3_HUMAN | RF3_UNDEAD | RF3_DEMON | RF3_KWAI)) chance = chance * 3 / 2;
				if(r_ptr->flags3 & RF3_ANG_COSMOS) chance /= 2;
				if(r_ptr->flags2 & RF2_POWERFUL) chance /= 2;
				if(r_ptr->flags3 & RF3_ANG_CHAOS) chance = chance * 3 / 2;
				if(r_ptr->level > plev) chance = chance * 2 / 3;
				if(r_ptr->level > plev * 2) chance /= 2;

				if ((r_ptr->blow[0].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[0].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[1].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[1].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[2].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[2].effect == RBE_EAT_GOLD)||
					(r_ptr->blow[3].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[3].effect == RBE_EAT_GOLD))
					chance *= 3;

				if(r_ptr->flagsr & (RFR_RES_ALL) || !(r_ptr->flags3 & RF3_UNDEAD) && !monster_living(r_ptr))
				{
					msg_format("%sには効果がなかった。",m_name);
				}
				else if( randint1(chance) < randint1(r_ptr->level) || m_ptr->r_idx == MON_SUIKA || m_ptr->r_idx == MON_YUGI || m_ptr->r_idx == MON_EIKI)
				{
					msg_format("%sの舌を封じるのに失敗した。",m_name);
				}
				else
				{
					msg_format("%sの舌を封じた！",m_name);
					m_ptr->mflag |= MFLAG_NO_SPELL;
				}
			}

			break;
		}

	case 5:
		{
			int dam = plev * 2 + chr_adj * 2;
			int rad = 2 + plev / 45;
			if(only_info) return format("損傷:%d * 不定",dam);

			if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			break;
		}
//magic_num1[0]に術実行中の敵のm_idxを保持する。m_list圧縮でm_idxが変わることもあるのか？
	case 6:
		{
			monster_type *m_ptr;
			int i;

			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				//magic_num[0]にセットされたm_idxの敵と同じr_idxのEPHEMERAがいれば既に術実行中とみなす。
				//もし将来EPHEMERAフラグ付きでモンスターを召喚する仙術やアイテムを実装したらこの辺でトラブル起こるかも
				if(m_list[p_ptr->magic_num1[0]].r_idx == m_ptr->r_idx && (m_ptr->mflag & MFLAG_EPHEMERA) )
				{
					msg_print("すでに術を実行中だ。");
					return NULL;
				}
			}

			if (!get_aim_dir(&dir)) return NULL;

			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}

			m_ptr = &m_list[cave[target_row][target_col].m_idx];

			if (!m_ptr->r_idx)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			else
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flagsr & (RFR_RES_ALL) || !(r_ptr->flags3 & RF3_UNDEAD) && !monster_living(r_ptr))
				{
					msg_format("%sには効果がなかった。",m_name);
				}
				else
				{
					p_ptr->magic_num1[0] = cave[target_row][target_col].m_idx;
//					if(cheat_xtra) msg_format("浄頗梨審判r_idx:%d",m_ptr->r_idx);
					if(summon_named_creature(0, py, px, m_ptr->r_idx, PM_EPHEMERA))
						msg_format("%sの幻影が出現した！",m_name);
					else
						msg_print("術に失敗した。");

				}
			}

			break;
		}


	case 7:
		{
			int damage = p_ptr->lev * 7 + chr_adj * 10;
			if(damage < 300) damage = 300;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("カードから眩い光が放たれた！");
			else
				msg_format("断罪の光が放たれた！");
			fire_spark(GF_HOLY_FIRE,dir,damage,2);

			break;
		}

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}






/*:::パルスィ専用技*/
class_power_type class_power_parsee[] =
{
	{5,5,10,FALSE,FALSE,A_WIS,0,0,"嫉視",
		"敵一体を睨み付けて混乱、恐怖させる。高レベルな敵に効きやすいが、自分より低レベルな敵やユニークモンスターには効果がない。"},
	{12,16,35,FALSE,TRUE,A_DEX,0,2,"華やかなる仁者への嫉妬",
		"視界内の敵を混乱させる。周囲に敵が多いほど効果が高くなる。"},
	{20,20,40,FALSE,TRUE,A_CHR,0,0,"嫉妬心操作Ⅰ",
		"モンスター一体の嫉妬心を煽る。嫉妬心を煽られたモンスターは自分より高レベルなモンスター全てに敵対する状態になる。ユニークモンスターと通常の精神を持たないモンスターには効果がない。" },
	{24,32,50,FALSE,TRUE,A_INT,0,0,"謙虚なる富者への片恨",
		"分身が現れる。分身は命令に関わらず自分について来ようとし、自分の間近にいないと消滅する。また敵に倒されたときに無属性の大爆発を起こす。(自分はこの爆発のダメージを受けない)"},
	{30,24,60,FALSE,TRUE,A_CHR,0,6,"ジェラシーボンバー",
		"精神攻撃属性のボールを放つ。威力は魅力に大きく依存し、また自分の体力が低いほど威力が上がる。精神を持たない敵やユニークモンスターには効果が薄い。"},
	{35,60,75,FALSE,TRUE,A_CHR,0,0,"嫉妬心操作Ⅱ",
		"このフロアでモンスターが増殖しなくなる。" },
	{39,50,70,FALSE,TRUE,A_WIS,0,0,"グリーンアイドモンスター",
		"嫉妬の怪物を呼び出す。怪物は標的が強いほど強くなり、命令に関わらず標的を追いかけ、標的が倒れると消える。自分の分身と同時には呼べない。"},
	{43,120,80,FALSE,TRUE,A_CHR,0,0,"嫉妬心操作Ⅲ",
		"短時間、自分の周囲の全てのモンスター同士が敵対するようになる。通常の精神を持たないモンスターには効果がない。" },
	{48,160,75,FALSE,TRUE,A_WIS,0,0,"積怨返し",
		"自分のいる場所に地獄の劫火属性の巨大なボールを発生させる。威力はこのフロアで自分が受けたダメージの合計になる。一度使うとダメージの合計はリセットされる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

// p_ptr->magic_num1[0]をグリーンアイドモンスターのターゲットIDXに使用
// p_ptr->magic_num1[1]を積怨返しのダメージカウントに使用　←parsee_damage_countを使うことにした
cptr do_cmd_class_power_aux_parsee(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int power;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("効力:不定");

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px) || !cave[target_row][target_col].m_idx)
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			m_ptr = &m_list[cave[target_row][target_col].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			power = (r_ptr->level - plev);
			if(power < 1)
			{
				msg_print("何だか妬む気持ちにならない。");
				return NULL;
			}
			power = power * 10 + plev * 2;
			msg_print("あなたの目が緑色に爛々と輝いた...");
			fire_ball_hide(GF_OLD_CONF,dir,power,0);
			fire_ball_hide(GF_TURN_ALL,dir,power,0);

			break;
		}
	case 1:
		{
			int power, target_num=0;
			if(only_info) return format("効力:不定");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr;
				m_ptr = &m_list[i];
				if(!m_ptr->r_idx) continue;
				if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
				if (!is_hostile(m_ptr)) continue;
				target_num++;
			}
			if(!target_num)
			{
				msg_format("誰もいない・・");
				return NULL;
			}
			power = 20 + target_num * 20;
			msg_format("嫉妬の念が周囲の者を狂わせた・・");
			confuse_monsters(power);
			break;
		}

	case 2:
	{
		int power = p_ptr->lev * 2 + chr_adj * 2;
		int y, x;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format("効力:%d", power);
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];

			monster_desc(m_name, m_ptr, 0);
			msg_format("あなたは%sに近寄って囁きかけた…", m_name);
			(void)set_monster_csleep(cave[y][x].m_idx, 0);

			if (r_ptr->flags3 & RF3_NO_CONF) power /= 2;
			if (MON_CONFUSED(m_ptr) || MON_MONFEAR(m_ptr)) power *= 2;

			if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flagsr & RFR_RES_ALL)
				msg_format("%sは無反応だ。", m_name);
			else if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR))
				msg_format("%sはあなたの声に耳を貸さない！", m_name);
			else if (m_ptr->mflag & MFLAG_SPECIAL)
				msg_format("%sにはすでに効いている。", m_name);
			else if (r_ptr->level > randint1(power) || is_pet(m_ptr))
				msg_format("%sは惑わされなかった。", m_name);
			else
			{
				msg_format("%sの目に緑色の光が宿った！", m_name);
				m_ptr->mflag |= MFLAG_SPECIAL;
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
		break;
	}
	case 3:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			bool flg_g_mon = FALSE;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_GREEN_EYED)) flg_g_mon = TRUE;
				if((m_ptr->r_idx == MON_PARSEE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("すでに術を実行している。",num);
				return NULL;
			}
			if(flg_g_mon)
			{
				msg_format("既にフロアに怪物を呼んでいる。");
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_PARSEE, PM_EPHEMERA))
				msg_print("あなたの幻影が現れた。");
			else
				msg_print("術に失敗した。");


			break;
		}
	case 4:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = (5 + p_ptr->lev / 10) * (100 + (p_ptr->mhp - p_ptr->chp) * 600 / p_ptr->mhp) / 100;
			sides = 20 + chr_adj/ 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("心張り裂ける一撃を放った。");
			fire_ball(GF_REDEYE, dir, damroll(dice,sides), rad);
			break;
		}

	case 5:
	{
		if (only_info) return format("");

		msg_print("あなたの怨嗟の念が大地に満ちた・・");
		num_repro += MAX_REPRO;
		break;
	}

	case 6:
		{
			int i;
			bool flg_success = FALSE;
			bool flg_g_mon = FALSE;
			bool flg_parsee = FALSE;
			monster_type *m_ptr;
			if(only_info) return format("");

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_GREEN_EYED)) flg_g_mon = TRUE;
				if((m_ptr->r_idx == MON_PARSEE) && m_ptr->mflag & MFLAG_EPHEMERA) flg_parsee = TRUE;
			}
			if(flg_g_mon)
			{
				msg_format("既にフロアに怪物を呼んでいる。");
				return NULL;
			}
			else if(flg_parsee)
			{
				msg_format("今は怪物を呼べない。");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px) || !cave[target_row][target_col].m_idx)
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			p_ptr->magic_num1[0] = cave[target_row][target_col].m_idx;

			if(summon_named_creature(0, py, px, MON_GREEN_EYED, PM_EPHEMERA)) flg_success = TRUE;

			if(flg_success)	msg_print("緑目の怪物が現れた！");
			else msg_print("怪物は現れなかった。。");

		break;
		}

	case 7:
	{
		int base = 5;
		if (only_info) return format("範囲:%d 期間:%d+1d%d",MAX_SIGHT,base,base);

		set_tim_general(base+randint1(base), FALSE, 0, 0);

	}
	break;

	///mod160319 積怨返しのダメージカウントを外部変数にした
	case 8:
		{
			int damage = parsee_damage_count;
			int rad;
			if(damage > 9999) damage = 9999;
			rad = 4 + damage / 2000;

			if(only_info) return format("損傷:最大%d",damage / 2);

			if(damage < 1)
			{
				msg_print("まだ恨みが積もっていない。");
				return NULL;
			}
			if(damage > 1000) msg_format("積もりに積もった恨みの念が大爆発を起こした！");
			else msg_format("恨みの念が炎となって燃え上がった！");
			project(0, rad, py, px, damage, GF_HELL_FIRE, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			parsee_damage_count = 0L ; //Be happy
			break;
		}

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::美鈴専用技*/
class_power_type class_power_meirin[] =
{
	{5,3,30,FALSE,FALSE,A_WIS,0,0,"気配察知",
		"周囲の生物を感知する。レベル20になると地形を感知し、レベル30になると全てのモンスターを感知するようになる。"},
	{11,4,40,FALSE,FALSE,A_WIS,0,1,"虎勁",
		"体内で気を練り、僅かに体力を回復しごく短時間士気高揚と腕力上昇を得る。"},
	{17,14,50,FALSE,TRUE,A_CHR,0,3,"華光玉",
		"気属性の球を放つ。球は敵や障害物に当たると爆発する。威力は魅力に大きく依存する。"},
	{22,23,55,FALSE,TRUE,A_DEX,0,6,"極彩颱風",
		"近くの敵に複数回のダメージを与える。ACが高い敵には当たりにくい。"},
	{27,20,50,FALSE,FALSE,A_DEX,30,0,"螺光歩",
		"短距離を一瞬で移動し敵がいれば攻撃して小ダメージを与える。障害物に当たると止まる。レベルが上がると射程が少し伸びる。この攻撃は連撃のカウントに含まれ、消費行動力も同様に減少する。"},
	{33,36,80,FALSE,TRUE,A_CON,0,0,"虹色太極拳",
		"一時的に電撃・火炎・冷気に対する耐性を得て物理防御力が上昇する。"},
	{37,60,65,FALSE,FALSE,A_DEX,0,0,"地龍天龍脚",
		"隣接した敵に大ダメージを与え後ろに吹き飛ばす。通常は巨大な敵は吹き飛ばない。これまでの連撃の回数に応じて威力が上昇する。"},
	{40,-80,70,FALSE,TRUE,A_CHR,0,0,"華厳明星",
		"MPを全て(最低80)消費して気属性の巨大な球を放つ。球は敵や障害物に当たると爆発する。消費したMPにより威力が上昇する。"},
	{43,72,75,FALSE,FALSE,A_WIS,0,0,"猛虎内勁",
		"低下した腕力を復活させ、短時間大幅な腕力上昇を得て、士気を高揚し、さらに連撃のカウントを5増加した扱いにする。ただしこの技の消費行動力は通常通りである。"},
	{47,-120,80,FALSE,TRUE,A_CHR,0,0,"彩光蓮華掌",
		"MPを全て(最低120)消費し、隣接した敵を中心に巨大な気属性の爆発を起こす。消費したMPとこれまでの連撃の回数に応じて威力が上昇する。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



//連撃カウントとしてconcent使用
//p_ptr->magic_num1[0]を消費行動力として使用

cptr do_cmd_class_power_aux_meirin(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;
	int use_energy = p_ptr->magic_num1[0];

	switch(num)
	{

	case 0://気配察知
		{
			int rad = 15 + plev / 5;
			if(only_info) return format("範囲:%d",rad);

			msg_print("気の流れを感じ取った・・");
			if(plev > 19) map_area(rad);
			if(plev > 29) detect_monsters_normal(rad);
			else detect_monsters_living(rad);
		}
		break;
	case 1:
		{
			int time;
			int heal;
			heal = plev / 5 + 5;
			if(only_info) return format("回復:%d+1d%d",heal,heal);
			time = 2 + randint1(3);
			msg_print("気を高めた。");
			hp_player(heal + randint1(heal));
			set_stun(p_ptr->stun - 10);
			set_cut(p_ptr->cut - 10);
			set_afraid(0);
			set_hero(time,FALSE);
			set_tim_addstat(A_STR,(2 + plev / 10),time,FALSE);

		}
		break;
	case 2:
		{
			int rad = 1 + plev / 30;
			int dice = 1;
			int sides = 50 + chr_adj * 5;
			int base = plev * 2;
			if(base < 30) base = 30;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("カードから虹色の弾が放たれた！");
			else
				msg_format("気弾を放った！");
			fire_rocket(GF_FORCE, dir, base + damroll(dice,sides), rad);
			break;
		}

	case 3:
		{
			int base = plev + chr_adj;
			int num = 3 + plev / 24 + chr_adj / 12;
			int rad = 2 + plev / 15;
			int i;
			if(only_info) return format("損傷:最大%d * %d",base/2,num);
			msg_print("虹色の竜巻を巻き起こした！");
			for (i = 0; i < num; i++) project(0, rad, py, px, base, GF_WINDCUTTER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;

		}
		break;
	case 4:
		{
			int len = 3 + (plev-25) / 6;
			int damage = 50 + plev * 2;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack2(len,GF_MISSILE,damage)) return NULL;

			//連撃カウント増加、行動消費減少
			new_class_power_change_energy_need = use_energy;
			do_cmd_concentrate(0);

			break;
		}

	case 5:
		{
			int time;
			int base = 15;
			if(only_info) return format("期間:1d%d + %d",base,base);
			time = base + randint1(base);
			msg_print("虹色の気を纏った！");
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_shield(time,FALSE);
		}
		break;
	case 6:
		{
			int y, x;
			int dam = (plev + chr_adj + p_ptr->to_d[0]) * (7 + cons) / 7;
			if(only_info) return format("損傷:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				msg_print("なぜか攻撃できない。");
				break;
			}
			else if (cave[y][x].m_idx)
			{
				msg_print("天を駆けるような飛び蹴りを放った！");
				project(0,0,y,x,dam,GF_ARROW,PROJECT_KILL,-1);
			}
			else
			{
				msg_print("その方向にはモンスターはいません。");
				return NULL;
			}

			if (cave[y][x].m_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				char m_name[80];

				if(!(r_ptr->flags2 & RF2_GIGANTIC) || cons > 9)
				{


					monster_desc(m_name, m_ptr, 0);

					for (i = 0; i < 5; i++)
					{
						y += ddy[dir];
						x += ddx[dir];
						if (cave_empty_bold(y, x))
						{
							ty = y;
							tx = x;
						}
						else break;
					}
					if ((ty != oy) || (tx != ox))
					{
						msg_format("%sを吹き飛ばした！", m_name);
						cave[oy][ox].m_idx = 0;
						cave[ty][tx].m_idx = m_idx;
						m_ptr->fy = ty;
						m_ptr->fx = tx;

						update_mon(m_idx, TRUE);
						lite_spot(oy, ox);
						lite_spot(ty, tx);

						if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
							p_ptr->update |= (PU_MON_LITE);
					}
				}
			}
		}
		break;

	case 7:
		{
			int rad = 5;
			int dice = 1;
			int sides = p_ptr->csp * 2;
			int base = p_ptr->csp;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な気弾を放った！");
			fire_rocket(GF_FORCE, dir, base + damroll(dice,sides), rad);
			break;
		}

	case 8:
		{
			int time;
			int base = 5;
			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			if(use_itemcard)
				msg_print("力が溢れてくる！");
			else
				msg_print("気を爆発的に高めた！");
			do_res_stat(A_STR);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_afraid(0);
			set_hero(time,FALSE);
			if(!use_itemcard) do_cmd_concentrate(1);

		}
		break;
	case 9:
		{
			int y, x;
			int dam = (plev + chr_adj * 2 + p_ptr->to_d[0]) * (10 + cons) / 10 * p_ptr->csp / 120;
			if(only_info) return format("損傷:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
			{
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sの体から虹色の光が漏れ出し、大爆発を起こした！",m_name);
				project(0,7,y,x,dam,GF_FORCE,PROJECT_KILL,-1);
			}
			else
			{
				msg_print("その方向にはモンスターはいません。");
				return NULL;
			}

		}
		break;


	default:
		if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}

	if(!use_itemcard && (use_energy < 20 || use_energy > 100))
	{
		msg_format("ERROR:use_energy計算値がおかしい(%d)",use_energy);
		new_class_power_change_energy_need = 100;

	}

	return "";
}



/*:::天子専用技*/
class_power_type class_power_tenshi[] =
{
	{ 6,10,30,FALSE,TRUE,A_STR,0,0,"坤儀の剣",
		"地震を起こす。"},
	{ 11,11,45,FALSE,TRUE,A_DEX,0,4,"非想の威光",
		"閃光属性のビームを放つ。"},
	{ 16,25,45,FALSE,TRUE,A_INT,0,0,"天罰の石柱",
		"隣接した敵一体にダメージを与える。レベル30以降に敵のいない場所に使うと岩石地形を生成する。"},
	{ 20,20,50,FALSE,TRUE,A_WIS,0,0,"要石設置",
		"このフロアで地震が起こらなくなる。ただし防いだ地震は要石に蓄積され、あまり多くの地震が蓄積されると要石が破壊される。要石が破壊されたとき大地震が起こって周囲にダメージを与える。フロアを移動すると要石は消え、蓄積された地震も無効化される。"},
	{ 20,0,0,FALSE,TRUE,A_WIS,0,0,"要石除去",
		"フロアに設置した要石を取り去る。蓄積された地震があると地震が起こる。"},
	{ 26,20,70,FALSE,FALSE,A_STR,0,5,"勇気凛々の剣",
		"プラズマ属性の強力なビームを放つ。"},
	{ 30,30,60,FALSE,TRUE,A_WIS,0,0,"無念無想の境地",
		"一定時間物理防御力と魔法防御力が上昇する。"},
	{ 35,60,60,FALSE,TRUE,A_INT,0,0,"カナメファンネル",
		"宙を舞う要石を召喚する。要石は捕獲など一部行動の対象にならずフロア移動で消える。"},
	{ 40,66,80,FALSE,TRUE,A_CON,0,0,"荒々しくも母なる大地よ",
		"視界内の全てに対し重力属性の攻撃を行う。"},
	{ 44,60,70,FALSE,TRUE,A_STR,0,0,"不譲土壌の剣",
		"周囲の狭い範囲を*破壊*する。"},
	{ 48,160,80,FALSE,TRUE,A_CHR,0,0,"全人類の緋想天",
		"極めて強力な気属性のレーザーを放つ。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

//要石設置座標と地震吸収回数にp_ptr->magic_num1[0-3]を使う
cptr do_cmd_class_power_aux_tenshi(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev / 3;
			if(only_info) return format("範囲:%d",rad);

			msg_print("剣を地面に突き立てた。");
			earthquake(py,px,rad);

			break;
		}
	case 1:
		{
			int damage = (20 + plev * 2  + chr_adj * 2);
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("レーザーを放った。");
			fire_beam(GF_LITE, dir, damage);

			break;
		}
	case 2:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = 20 + plev * 3;
			if(plev > 29) damage += plev * 2;
			if(only_info) return format("損傷:%d",damage);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if(!cave_have_flag_bold(y,x,FF_MOVE) && !cave_have_flag_bold(y,x,FF_CAN_FLY))
			{
				msg_print("その場所に対しては使えない。");
				return NULL;
			}
			if(plev < 30) msg_print("石柱が空から降ってきた。");
			else msg_print("大岩が空から降ってきた。");

			if (cave[y][x].m_idx)
			{
				project(0,0,y,x,damage,GF_ARROW,PROJECT_KILL,-1);
			}
			else if(plev > 29 && cave_naked_bold(y, x))
			{
				cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
			}

			break;
		}
	case 3:
		{
			if(only_info) return format("");

			if(p_ptr->magic_num1[0])
			{
				msg_format("このフロアにはすでに要石を設置している。");
				return NULL;
			}
			else if(!cave_have_flag_bold(py,px,FF_FLOOR))
			{
				msg_format("ここに要石を設置することはできない。");
				return NULL;
			}

			msg_format("あなたは要石を設置した。要石は地面に沈み込んでいった・・");
			p_ptr->magic_num1[0] = py;
			p_ptr->magic_num1[1] = px;

		}
		break;
	case 4:
		{
			if(only_info) return format("");
			if(!p_ptr->magic_num1[0])
			{
				msg_format("このフロアにはまだ要石を設置していない。");
				return NULL;
			}

			msg_print("このフロアの要石を大地から引っこ抜いた！");
			kanameishi_break();

		}
		break;

	case 5:
		{
			dice = p_ptr->lev / 3;
			sides = 20 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("剣から熱線が放たれた！");
			fire_spark(GF_PLASMA,dir, damroll(dice,sides),1);

			break;
		}
	case 6:
		{
			int time;
			base = plev/2;
			sides = plev/2;

			if(base < 15) base = 15;
			if(sides < 15) sides = 15;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			time = base + randint1(sides);
			if(use_itemcard)
				msg_format("体が鉄のように丈夫になった気がする！");
			else
				msg_format("あなたは気合を入れた！");
			set_shield(time, FALSE);
			set_resist_magic(time,FALSE);
			break;
		}
		break;
	case 7:
		{
			int max_kaname = (plev - 20) / 3;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if(max_kaname < 3) max_kaname = 3;

			if(only_info) return format("最大:%d",max_kaname);

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_KANAME) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_kaname)
			{
				msg_format("これ以上配置できない。",num);
				return NULL;
			}

			for(i=0;i<(max_kaname / 3);i++)(void)summon_named_creature(0, py, px, MON_KANAME, PM_EPHEMERA);

		}
		break;
	case 8:
		{
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("損傷:%d",dam);
			msg_format("大地が広範囲に隆起した！");
			project_hack2(GF_GRAVITY,0,0,dam);
			break;
		}
	case 9:
		{
			int rad = 3;
			if(only_info) return format("範囲:%d",rad);
			if(!dun_level)
			{
				msg_format("ここでは使えない。");
				return NULL;
			}

			msg_print("地殻変動が起こった！");
			destroy_area(py,px,rad,FALSE,FALSE,TRUE);
		}
		break;
	case 10:
		{
			int damage = p_ptr->lev * 9 + chr_adj * 10;
			if(damage < 400) damage = 400;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("緋想の剣から凝縮された気質が放たれた！");
			fire_spark(GF_FORCE,dir,damage,2);

			break;
		}
		break;

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::藍専用技*/
class_power_type class_power_ran[] =
{
	{ 10,8,40,FALSE,TRUE,A_CHR,0,0,"狐狗狸さんの契約",
		"悪魔・妖怪・アンデッド一体を配下として呼び出す。"},
	{ 15,16,25,FALSE,TRUE,A_WIS,0,5,"アルティメットブディスト",
		"自分の周囲に複数回の閃光属性攻撃を行う。レベルが上がると範囲が広がる。"},
	{ 20,20,35,FALSE,TRUE,A_INT,0,0,"式神橙",
		"橙を配下として召喚する。召喚された橙は捕獲などが不可能で階移動すると消える。レベル30以上になると加速した状態で呼び出す。"},
	{ 25,24,45,FALSE,TRUE,A_INT,0,0,"憑依荼吉尼天",
		"一定時間、浮遊・加速を得る。レベル35以上になると高速移動も追加される。"},
	{ 30,40,40,FALSE,TRUE,A_INT,0,0,"ユーニラタルコンタクト",
		"敵一体を高確率で魅了し配下にする。ユニークモンスターには効かない。"},
	{ 35,32,70,FALSE,FALSE,A_DEX,0,0,"プリンセス天狐",
		"ターゲットの隣に一瞬で移動してそのまま攻撃する。視界外の敵にも有効。"},
	{ 40,64,80,FALSE,TRUE,A_WIS,0,0,"八千万枚護摩",
		"視界内の全てに対して破邪属性攻撃を行う。"},
	{ 45,128,80,FALSE,TRUE,A_INT,0,0,"飛翔役小角",
		"短時間、身体能力が大幅に上昇し加速しヒーロー化する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_ran(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int typ;
			if(only_info) return format("");

			if(one_in_(3)) typ = SUMMON_DEMON;
			else if(one_in_(2)) typ = SUMMON_UNDEAD;
			else typ = SUMMON_KWAI;

			if(summon_specific(-1, py, px, plev, typ, (PM_FORCE_PET)))
				msg_format("何かが招かれてきた！");
			else
				msg_format("何も現れなかった・・");

			break;
		}
	case 1:
		{
			int rad = 1 + plev / 16;
			int num = (plev+5) / 6;
			int damage = (10 + plev  + chr_adj) / 2 ;
			int flg = (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL);

			if(only_info) return format("損傷:最大%d*%d",damage,num);
			msg_format("卍の印が高速回転した！");
			for(;num>0;num--) project(0,rad,py,px,damage,GF_LITE,flg,-1);

			break;
		}
	case 2:
		{
			int i;
			bool flag = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			u32b mode = PM_EPHEMERA;

			if(only_info) return "";
			if(plev > 29) mode |= PM_HASTE;

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_CHEN) ) flag = TRUE;
			}
			if(flag)
			{
				msg_print("橙はすでにこのフロアにいる。");
				return NULL;
			}
			if(summon_named_creature(0, py, px, MON_CHEN, PM_EPHEMERA))
				msg_print("橙を呼び出した。");
			else
				msg_print("式の召喚に失敗した。");

			break;

		}
	case 3:
		{
			int time;
			base = 25;
			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			set_fast(time,FALSE);
			set_tim_levitation(time,FALSE);
			if(plev > 34) set_tim_speedster(time,FALSE);

		}
		break;
	case 4:
		{
			int power = 50 + plev * 2 + chr_adj * 5;
			if(power < 120) power = 120;

			if(only_info) return format("効力:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay())
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			msg_print("あなたは敵を支配しようと試みた・・");
			fire_ball_hide(GF_CHARM,dir,power,0);
			break;
		}
	case 5:
		{
			int x, y;
			int dist = plev / 2;
			if(only_info) return format("範囲:%d",dist);
			if (!tgt_pt(&x, &y)) return NULL;

			if(!cave[y][x].m_idx || !m_list[cave[y][x].m_idx].ml)
			{
				msg_print("そこには何もいない。");
				return NULL;
			}

			else if (distance(y, x, py, px) > dist)
			{
				teleport_player(30, 0L);
				msg_print("瞬間移動に失敗した。");
			}
			else
			{
				teleport_player_to(y, x, 0L);
				if(distance(py,px,y,x) > 1)
				{
					msg_print("うまく標的の場所に行けなかった。");
				}
				else
				{
					msg_print("あなたは一瞬で標的の隣に現れた！");
					py_attack(y,x,0L);
				}
			}
			break;
		}
	case 6:
		{
			int dice = 5 + p_ptr->lev / 2;
			int sides = 10 + chr_adj / 5;
			if(dice < 20) dice = 20;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if(use_itemcard)
				msg_format("カードから大量の札が噴き出して周囲を埋め尽くした!");
			else
				msg_format("大量の札で周囲を埋め尽くした！");
			project_hack2(GF_HOLY_FIRE,dice,sides,0);
			break;
		}
		break;
	case 7:
		{
			int percentage;
			int base = 7;
			int time;
			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_format("伝説の超人の力が身に宿った気がする！");
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			time = base + randint1(base);
			set_tim_addstat(A_STR,110,time,FALSE);
			set_tim_addstat(A_DEX,110,time,FALSE);
			set_tim_addstat(A_CON,110,time,FALSE);
			set_fast(time, FALSE);
			set_hero(time,FALSE);
			set_tim_speedster(time, FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= (PR_HP );
			redraw_stuff();


			break;
		}

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::諏訪子専用技*/
class_power_type class_power_suwako[] =
{
	{6,4,20,FALSE,TRUE,A_DEX,0,1,"水蛙神",
		"射程の短い水属性のボールを放つ。"},
	{12,15,35,FALSE,FALSE,A_WIS,0,4,"洩矢の鉄の輪",
		"無属性のボールを放つ。この攻撃は7/8の確率で再発動する。"},
	{16,32,40,TRUE,FALSE,A_CON,0,20,"諏訪清水",
		"水属性のブレスを吐く。威力はHPの1/4になる。"},
	{20,30,60,TRUE,FALSE,A_DEX,0,0,"蛙石神",
		"指定した方向の隣接グリッドに岩を生成する。"},
	{24,27,65,FALSE,TRUE,A_WIS,0,5,"手長足長さま",
		"特殊な軌道のレーザーで周囲のランダムな敵を攻撃する。"},
	{28,0,0,FALSE,FALSE,A_CHR,0,0,"オールウェイズ冬眠できます",
		"一定時間冬眠し、その間HPとMPが十倍の早さで回復する。冬眠開始時に全ての一時効果が解除される。冬眠中はほとんどの攻撃から無敵だがダメージを受けると途中で起こされる。冬眠から目覚める時に最大で通常行動時の倍程度の隙ができる。"},
	{32,43,60,FALSE,TRUE,A_CON,0,6,"ケロちゃん風雨に負けず",
		"周囲のランダムな場所に水属性のボールを連続で発生させる。"},
	{35,35,70,FALSE,TRUE,A_DEX,0,0,"大地の湖",
		"一定時間壁を抜けられるようになる。"},
	{40,80,70,FALSE,TRUE,A_WIS,0,0,"祟られた大地",
		"視界内の全てを汚染属性で攻撃する。"},
	{44,90,80,FALSE,TRUE,A_CHR,0,0,"祟り神召喚",
		"配下の祟り神を一体呼び出す。"},
	{48,-100,90,FALSE,TRUE,A_CHR,0,0,"赤口さま",
		"MPを全て(最低100)消費し、隣接した敵に無属性のダメージを与える。消費MP量と現在のHPの低下度により威力が変化する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


/*:::諏訪子用特技*/
cptr do_cmd_class_power_aux_suwako(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int point = p_ptr->magic_num1[0] / 100; //最大999

	switch(num)
	{
	case 0:
		{
			int damage = 20 + plev + chr_adj;
			int dist = 2 + plev / 10;
			if(only_info) return  format("射程:%d 損傷:%d",dist,damage);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("透き通った蛙が飛びかかった。");
			fire_ball(GF_WATER, dir, damage,1);
			break;
		}
	case 1://洩矢の鉄の輪
		//v1.1.44 ルーチン見直し
		{
			int tx,ty;
			int dam = 10 + plev/2 + chr_adj/2 ;
			int rad = 0;

			if(only_info) return format("損傷:%d * 不定",dam);

			if (!get_aim_dir(&dir)) return NULL;

			msg_format("鉄輪を投擲した！");
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_ARROW, rad);

		}
		break;
	case 2:
		{
			int dam = p_ptr->chp / 4;
			if(dam<1) dam = 1;
			if(dam > 800) dam=800;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;
			if(use_itemcard)
				msg_print("カードから濁流が噴き出した！");
			else
				msg_print("あなたは水流を吐いた！");

			fire_ball(GF_WATER, dir, dam, -2);
			break;
		}
	case 3:
		{
			int y,x;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!cave_naked_bold(y, x))
			{
				msg_print("ここには岩を作れない。");
				return NULL;
			}
			msg_print("地面から蛙のような岩が出現した！");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));

			break;

		}
	case 4:
		{
			int dam = plev + chr_adj/2 ;
			int cnt;

			if(only_info) return format("損傷:%d*不定",dam);
			msg_format("手長足長の力を借りた。");
			for(cnt=0;cnt<4;cnt++) fire_random_target(GF_MISSILE,dam,6,0,0);

		}
		break;
	case 5:
		{
			base = 10;
			if(only_info) return format("期間:%d",base);

			if (p_ptr->inside_arena)
			{
				msg_print("あなたはすでに夢の中にいる。");
				return NULL;
			}

			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			dispel_player();
			reset_tim_flags();
			set_tim_general(base,FALSE,0,0);
			do_cmd_redraw();

		}
		break;
	case 6:
		{
			int rad = 2 + plev / 24;
			int damage = plev*2  + chr_adj*2 ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("天から多数の水塊が降り注いだ。");
			cast_meteor(damage, rad, GF_WATER);

			break;
		}
	case 7:
		{
			int base = plev / 2;
			if(base < 10) base = 10;
			if(only_info) return format("期間:%d+1d%d",base,base);

			set_kabenuke(base + randint1(base), FALSE);

			break;
		}
	case 8:
		{
			int dam = plev * 3 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);
			msg_print("大地が黒く染まり、瘴気を噴き出した！");
			project_hack2(GF_POLLUTE,0,0,dam);

			break;
		}
	case 9:
		{
			if(only_info) return format("");
			if(summon_named_creature(0, py, px, MON_TATARI, PM_FORCE_PET))
				msg_print("祟り神を呼び出した！");
			else
				msg_print("祟り神は現れなかった。");

		}
		break;
	case 10:
		{
			int dam = 5 * (p_ptr->csp+1) + chr_adj * 10;
			dam = dam * (p_ptr->mhp + (p_ptr->mhp - p_ptr->chp) * 2) / p_ptr->mhp;
			if(only_info) return format("損傷:%d",dam/2);
			//Hack 画面を黒にするためworld_monsterを使う
			world_monster = -1;
			do_cmd_redraw();
			Term_xtra(TERM_XTRA_DELAY, 1500);
			project(0,1,py,px,dam,GF_DISP_ALL,PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE,-1);
			world_monster = 0;
			p_ptr->redraw |= (PR_MAP);
			handle_stuff();

			break;
		}

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::魔道具使い専用技*/
//p_ptr->magic_num1(2)[0-95?]が魔道具に使われる。
class_power_type class_power_magic_eater[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"魔道具吸収",
		"魔道具（杖・魔法棒・ロッド）を体に取り込む。取り込んだ魔道具はmコマンドから使用できる。"},

	{10,15,30,FALSE,TRUE,A_INT,0,0,"魔力充填Ⅰ",
		"体に取り込んでいない魔道具の使用回数を回復させる。充填に失敗して魔道具が壊れることがある。魔道具の回数が空に近いほど失敗しにくい。"},

	{20,20,45,FALSE,TRUE,A_INT,0,0,"魔力充填Ⅱ",
		"体に取り込んだ杖・魔法棒・ロッドの使用回数を充填する。充填量にはレベル・知能・アイテムレベル・最大使用可能回数が影響し、高難度な魔道具は一度の充填では使用回数が増えないことがある。"},

	{30,30,60,FALSE,TRUE,A_INT,0,0,"魔力充填Ⅲ",
		"発動済みの装備品の充填時間を短縮する。"},

	{40,40,80,FALSE,TRUE,A_INT,0,0,"強力発動",
		"体に取り込んだ魔道具の威力や効果を高めて使用する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_magic_eater(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return "";
			gain_magic();
			break;
		}
	case 1:
		{
			int power = 50 + plev * 3;
			if(only_info)return format("効力:%d",power);
			if (!recharge(power)) return NULL;
			break;
		}
	case 2:
		{
			int power = (50+plev*2) * (20+adj_general[p_ptr->stat_ind[A_INT]]*3) * (150+randint1(100));
			if(only_info) return "";
			if(!recharge_magic_eater(power)) return NULL;
			break;
		}
	case 3:
		{
			int base = plev*2;
			if(only_info)return format("短縮ターン:%d+1d%d",base,base);
			if(!do_cmd_recharge_equipment(base+randint1(base))) return NULL;
			break;
		}

	case 4:
		{
			if(only_info) return "";
			if (!do_cmd_magic_eater(FALSE, TRUE)) return NULL;
			break;
		}

	default:
		if(only_info) return "未実装";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::格闘家用特技*/
class_power_type class_power_martial_artist[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"気合い溜め",
		"一時的にMPを大幅に増加させる。MPが通常より増加している状態だと格闘の通常攻撃や他の技の威力が高くなる。"},
	{4,12,20,FALSE,FALSE,A_DEX,0,0,"太陽拳",
		"周囲を明るく照らし、光に弱い敵にダメージを与える。パワーが上がると視界内の敵を混乱させるようになる。"},
	{8,17,25,FALSE,FALSE,A_STR,0,0,"波動拳",
		"気属性のボルトを放つ。パワーが上がるとビームになる。"},
	{12,12,50,FALSE,FALSE,A_DEX,30,0,"二重の極み",
		"隣接した場所に分解属性の攻撃を行う。壁などを掘ることもできるが地面のアイテムが壊れる。パワーが上がると分解の半径が広がる。"},
	{15,24,35,FALSE,FALSE,A_WIS,60,0,"舞空術",
		"一時的に浮遊する。レベル40以降は高速移動できるようになる。装備が重いと失敗しやすい。"},

	{18,30,50,FALSE,TRUE,A_WIS,0,0,"獅子咆哮弾",
		"射程の短い気属性のビームを放つ。HPの最大値からの減少量に応じて威力が上昇し、HPが半分以下のときには自分を中心とする大型のボールに変化する。"},
	{20,20,75,FALSE,TRUE,A_INT,0,4,"円",
		"周囲の地形・アイテム・トラップ・モンスターを全て感知する。パワーが上がると範囲が広がる。"},
	{22,66,75,FALSE,TRUE,A_CHR,0,0,"ドレインタッチ",
		"隣接した生物からHPを大幅に吸収する。無生物の種族にしか使えない。"},
	{24,-30,55,FALSE,FALSE,A_STR,30,0,"真空波",
		"MPを全て(最低30)使用し、周囲の全てに対し風の刃で攻撃を行う。ACが高い敵には当たりにくい。"},
	{27,25,50,FALSE,FALSE,A_DEX,80,0,"縮地",
		"ターゲットの隣まで一瞬で移動しそのまま攻撃する。障害物に当たると止まる。装備が重いと失敗しやすい。"},
	{30,45,60,FALSE,TRUE,A_WIS,20,3,"レイ・ガン",
		"敵や障害物に当たると爆発する強力な気属性の球を放つ。破邪に弱い種族の場合は暗黒属性の「ブラック・レイガン」になる。"},

	{32,50,65,FALSE,TRUE,A_CON,0,0,"八門遁甲",
		"短時間腕力と器用さを増加させ加速するが、50+1d100のダメージを受ける。"},
	{35,-70,65,FALSE,TRUE,A_DEX,0,0,"山吹色の波紋疾走",
		"MPを全て(最低70)消費し、隣接した敵に閃光属性のダメージを与える。MPの量に応じてダメージが大幅に上昇し、とくにアンデッドには大ダメージを与える。無生物の種族には使えない。"},

	{37,-50,80,FALSE,TRUE,A_CON,0,0,"集気法",
		"MPを全て(最低50)消費し、消費したMPの分のHPを回復して切り傷を治す。"},
	{40,-100,85,FALSE,TRUE,A_STR,0,0,"カメハメ波",
		"MPを全て(最低100)消費し、気属性の強力なビームを放つ。"},
	{44,-60,70,FALSE,TRUE,A_STR,0,0,"地母の晩餐",
		"MPを全て(最低60)消費し、周囲を*破壊*する。"},
	{48,-150,75,FALSE,TRUE,A_WIS,0,0,"界王拳",
		"MPを全て(最低150)消費し、ごく短時間スピード・身体能力・攻撃力を大幅に上昇させる。ただし効果が切れたとき減速と朦朧状態になり、一度使うとしばらくは使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//界王拳にtim_general[0,1]を使う
cptr do_cmd_class_power_aux_martial_artist(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int ex_power = (p_ptr->csp > p_ptr->msp)?(p_ptr->csp - p_ptr->msp):0;
	int dir;

	if(p_ptr->tim_general[0]) ex_power += 200;

	switch(num)
	{
	case 0:
		{
			//int max_csp = MAX(p_ptr->msp*3, p_ptr->lev*5+5);
			int max_csp = MIN(p_ptr->msp*4, 255);
			if(only_info) return format("最大:%d",max_csp);

			if (total_friends)
			{
				msg_print("今は配下のことに集中していないと。");
				return NULL;
			}
			if(p_ptr->tim_general[0])
			{
				msg_print("気を爆発的に高めた！");
				p_ptr->csp += p_ptr->msp;
			}
			else
			{
				msg_print("気を高めた！");
				p_ptr->csp += p_ptr->msp / 2;
			}
			if (p_ptr->csp >= max_csp)
			{
				p_ptr->csp = max_csp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			p_ptr->update |= (PU_BONUS);

			break;
		}
	case 1:
		{
			int power = plev + ex_power;
			int rad = MIN(10,(3 + power / 20));
			if(only_info) return format("");
			msg_format("あなたは突然眩く光った！");
			lite_area(damroll(2, power / 5), rad);
			if(power >= 50) confuse_monsters(power);
		}
		break;
	case 2:
		{
			int dice = 3 + ex_power / 7;
			int sides = 4 + plev / 8;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return FALSE;
			if(dice > 9)
			{
				msg_format("気砲を放った！");
				fire_beam(GF_FORCE,dir,damroll(dice,sides));
			}
			else
			{
				msg_format("気弾を放った！");
				fire_bolt(GF_FORCE,dir,damroll(dice,sides));
			}
		}
		break;
	case 3:
		{
			int y,x;
			int rad = plev / 30 + ex_power / 50;
			int damage = 20 + plev + ex_power / 3;

			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			msg_format("二重の衝撃を与えた！");
			project(0,rad,y,x,damage,GF_DISINTEGRATE,PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM,-1);
			break;

		}
	case 4:
		{
			int time = 15 + plev / 2;
			if(only_info) return format("期間:%d",time);

			if(plev > 39)
			{
				set_tim_speedster(time, FALSE);
			}
			else
			{
				msg_format("気の力で身体が浮いた！");
				set_tim_levitation(time, FALSE);
			}

			break;
		}
	case 5:
		{
			int dist = 2 + ex_power / 60 + plev / 35;
			int damage=(plev + ex_power / 2) * (p_ptr->mhp - p_ptr->chp) / 100;

			if(dist > 7) dist = 7;
			if(p_ptr->hero) damage /= 3;

			if(!damage)
			{
				if(only_info) return  format("損傷:%d",damage);
				msg_format("気を放とうとしたが気が軽かった。");

			}
			else if(p_ptr->chp > p_ptr->mhp / 2)
			{
				if(only_info) return  format("射程:%d 損傷:%d",dist,damage);
				project_length = dist;
				if (!get_aim_dir(&dir)) return NULL;
				msg_format("あなたは重い気を放った！");
				fire_beam(GF_FORCE, dir, damage);
			}
			else
			{
				damage *= 4;
				if(only_info) return  format("損傷:最大%d",damage/2);
				msg_format("巨大な気が沈み地面に大穴を開けた！");
				project(0,dist,py,px,damage,GF_FORCE,PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM,-1);
			}

			break;
		}
	case 6:
		{
			int rad = 5 + plev / 10 + ex_power / 5;
			if(rad > 50) rad = 50;
			if(only_info) return  format("範囲:%d",rad);
			msg_format("周囲を濃密な念で満たした・・");
			map_area(rad);
			detect_all(rad);
			break;

		}
	case 7: //ドレインタッチ
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 2 + ex_power;
			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたは冷たい手で%sに触れた・・",m_name);
				if(r_ptr->flagsr & RFR_RES_ALL || !monster_living(r_ptr))
				{
					msg_format("%sには全く効果がないようだ。",m_name);
					break;
				}
				if(r_ptr->flagsr & RFR_RES_NETH)
				{
					msg_format("%sはいくらかの耐性を示した。",m_name);
					damage /= 2;
				}

				if(m_ptr->hp < damage) damage = m_ptr->hp + 1;

				if(damage>0)
				{
					msg_format("%sから体力を吸い取った！",m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					hp_player(damage);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
	case 8:
		{
			int dam = plev * 3 + ex_power ;
			if(only_info) return format("損傷:%d",dam);
			msg_print("あなたは激しく回転し始めた！");
			project_hack2(GF_WINDCUTTER,0,0,dam);

			break;
		}
	case 9:
		{
			int len = plev / 7 + ex_power / 20;
			if(len > 16) len = 16;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 10://レイ・ガン
		{
			int dam = 50 + plev + ex_power;
			int rad = 1 + plev / 35 + ex_power / 50;
			int dist = MIN(15,5 + ex_power / 20);
			if(rad > 5) rad = 5;
			if(only_info) return format("損傷:%d",dam);

			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(is_hurt_holy() > 0)
			{
				msg_print("あなたの拳から黒い光が放たれた！");
				fire_rocket(GF_DARK, dir, dam, rad);
			}
			else
			{
				msg_print("あなたの拳から眩い光が放たれた！");
				fire_rocket(GF_FORCE, dir, dam, rad);
			}
			break;
		}
	case 11:
		{
			int time = plev / 2;

			if(only_info) return format("期間:%d",time);
			if(p_ptr->chp < 150 && !get_check_strict("体力が少なすぎる。本当に使いますか？", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("身体の潜在能力を引き出した！");
			set_tim_addstat(A_STR,105,time,FALSE);
			set_tim_addstat(A_DEX,105,time,FALSE);
			set_fast(time,FALSE);
			take_hit(DAMAGE_USELIFE, 50 + randint1(100), "八門遁甲の負荷", -1);

			break;
		}
	case 12: //山吹色の波紋疾走
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 4 + ex_power * 2;
			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flagsr & RFR_RES_ALL)
				{
					msg_format("%sには全く効果がないようだ。",m_name);
					break;
				}
				if(r_ptr->flags3 & RF3_UNDEAD)
				{
					msg_format("「ふるえるぞハート！燃え尽きるほどヒート！刻むぞ血液のビート！」");
					damage *= 2;
				}
				msg_format("%sに波紋エネルギーを叩き込んだ！",m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_LITE,PROJECT_KILL,-1);

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
	case 13:
		{
			int heal = p_ptr->csp;
			if(p_ptr->tim_general[0]) heal += 200;
			if(only_info) return format("回復:%d",heal);
			msg_format("気を身体に巡らせた・・");
			hp_player(heal);
			set_cut(0);
			break;

		}
	case 14:
		{
			int dice = 10 + ex_power / 3;
			int sides = plev / 3;
			int rad = 1;
			if(p_ptr->tim_general[0]) rad = 2;

			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return FALSE;
			msg_format("「波ァ――――！」");
			fire_spark(GF_FORCE,dir,damroll(dice,sides),rad);
		}
		break;
	case 15:
		{
			int rad = 7 + ex_power / 15;
			if(only_info) return format("範囲:%d",rad);
			msg_print("地面に亀裂が入り光が漏れだした・・");
			destroy_area(py,px,rad,FALSE,FALSE,FALSE);
		}
		break;
	case 16://界王拳　技自体のタイマーと技の再使用禁止タイマーの二つのtim_generalを使う
		{
			int time = p_ptr->csp / 50;
			int percentage = p_ptr->chp * 100 / p_ptr->mhp;
			if(only_info) return format("期間:%d",time);

			msg_print("瞬間的に戦闘力を数倍に高めた！");
			set_tim_general(time,FALSE,0,0);
			set_tim_general(100+randint1(200),FALSE,1,0);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			p_ptr->update |= PU_BONUS;
			handle_stuff();
		}
		break;


	default:
		if(only_info) return format("未実装");
		//msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::ヤマメ専用技*/
class_power_type class_power_yamame[] =
{

	{1,5,0,TRUE,FALSE,A_DEX,0,0,"巣を張る",
		"地形「蜘蛛の巣」を作る。巣の上にいるとACボーナスを得られる。レベル30以降はさらに高速移動能力を得られ、また巣の上にいるモンスターを感知する。"},
	{12,10,35,FALSE,TRUE,A_STR,0,4,"キャプチャーウェブ",
		"周囲に蜘蛛の巣を張り巡らせる。レベル25以上になるとさらに周囲の敵にダメージを与え減速させようとする。"},
	{19,22,40,FALSE,FALSE,A_CON,0,0,"原因不明の熱病",
		"視界内の敵を混乱、朦朧させる。"},
	{25,16,55,FALSE,FALSE,A_DEX,50,0,"カンダタロープ",
		"近くの蜘蛛の巣を指定し、そこへ一瞬で移動する。もしその蜘蛛の巣の場所にモンスターがいた場合自分は移動せずそのモンスターを自分のところまで引き寄せる。ただし巨大な敵には効果がない。また装備品が重いと失敗しやすい。"},
	{30,32,60,FALSE,FALSE,A_WIS,0,10,"フィルドミアズマ",
		"視界内のすべてに汚染属性の攻撃を仕掛ける。"},
	{35,30,65,TRUE,FALSE,A_CON,0,0,"樺黄小町",
		"隣接した敵に噛みついてダメージを与える。毒耐性を持たない敵には三倍のダメージを与え、低確率で一撃で倒す。"},
	{40,40,70,FALSE,FALSE,A_STR,0,0,"階段生成",
		"今いるところに階段を作る。上り階段になることもある。地上やクエストダンジョンでは使えない。" },

	{43,48,80,FALSE,FALSE,A_CHR,0,0,"石窟の蜘蛛の巣",
		"視界内の床に蜘蛛の巣を張り巡らせる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_yamame(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //巣を張る
		{
			if(only_info) return format("");

			if(cave_have_flag_bold(py,px,FF_SPIDER_NEST_1))
			{
				msg_print("ここには既に巣を張っている。");
				return NULL;
			}

			project(0, 0, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID, -1);
			if (cave_have_flag_bold(py, px, FF_SPIDER_NEST_1))
			{
				msg_print("巣を張った。");

				//v1.1.75 処理忘れ追加
				p_ptr->redraw |= (PR_STATE);
				p_ptr->update |= (PU_BONUS);
			}
			else
				msg_print("ここには巣を張れない。");




			break;
		}
	case 1: //キャプチャーウェブ
		{
			int rad =  1 + (plev-10) / 15;
			if(plev < 25)
			{
				if(only_info) return format("");
				msg_print("蜘蛛の糸を網状に広げた。");
				project(0, rad, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID, -1);

			}
			else
			{
				int dam = plev * 2 + chr_adj * 2;
				if(only_info) return format("損傷:～%d",dam/2);
				msg_print("光る網を放った！");
				project(0, rad, py, px, 0, GF_MAKE_SPIDER_NEST, PROJECT_GRID | PROJECT_HIDE, -1);
				project(0, rad, py, px, dam, GF_INACT, PROJECT_STOP | PROJECT_GRID | PROJECT_KILL, -1);

			}

			break;
		}

	case 2: //原因不明の熱病
		{
			int power = plev * 3;
			if(power < 50) power = 50;
			if(only_info) return format("効果:%d",power);
			if(use_itemcard)
				msg_print("カードが何かを撒き散らしている...");
			else
				msg_print("あなたは病原菌を撒き散らした。");
			confuse_monsters(power);
			stun_monsters(power);

			break;
		}
	case 3: //カンダタロープ
		{
			int range = plev / 2;
			int x = 0, y = 0;
			if(only_info) return format("範囲:%d",range);

			if (!tgt_pt(&x, &y)) return NULL;

			if(!cave_have_flag_bold((y), (x), FF_SPIDER_NEST_1))
			{
				msg_print("そこには巣がない。");
				return NULL;
			}
			else if(distance(py,px,y,x) > range)
			{
				msg_print("そこは遠すぎる。");
				return NULL;
			}
			else if(cave[y][x].m_idx)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);

				if(r_ptr->flags2 & RF2_GIGANTIC)
				{
					msg_format("%sは巨大すぎて引き寄せられなかった！",m_name);
					break;
				}
				else
				{
					msg_format("%sに糸を絡め、引っ張り寄せた！",m_name);
					teleport_monster_to(cave[y][x].m_idx, py, px, 100,TELEPORT_PASSIVE);
				}
			}
			else
			{
				msg_format("あなたは糸を手繰って飛んだ！");
				teleport_player_to(y, x, 0L);
			}

			break;
		}


	case 4: //フィルドミアズマ
		{
			int dam = plev * 3 + chr_adj * 3;
			if(only_info) return format("損傷:%d",dam);
			msg_print("あなたは瘴気を撒き散らした。");
			project_hack2(GF_POLLUTE,0,0,dam);

			break;
		}

	case 5: //樺黄小町
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			damage = 10 + plev + chr_adj * 2;
			if(damage < 50) damage = 50;
			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(use_itemcard) msg_format("%sに牙を突き立てた！",m_name);
				else msg_format("あなたは%sに噛み付いた！",m_name);

				if(r_ptr->flagsr & RFR_RES_ALL)
				{
					msg_format("%sには全く効果がないようだ。",m_name);
					break;
				}

				if(!(r_ptr->flagsr & RFR_IM_POIS))
				{
					damage *= 3;
					if(!((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
						&& (randint1(randint1(r_ptr->level/7)+3) == 1))
					{
						msg_format("%sの全身に毒が回った！",m_name);
						damage = m_ptr->hp + 1;
					}
				}

				if(damage>0)
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

	case 6: //階段生成
	{
		if (only_info) return format("");

		stair_creation();

		break;
	}

	case 7: //石窟の蜘蛛の巣
		{
			if(only_info) return format("");
			msg_print("周囲が光る網に埋め尽くされた！");
			project_hack2(GF_MAKE_SPIDER_NEST,0,0,0);

			//v1.1.75 処理忘れ追加
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);

			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::空専用技*/
class_power_type class_power_utsuho[] =
{

	{1,0,0,FALSE,FALSE,A_WIS,0,0,"核融合",
		"体内で無尽蔵にエネルギーを生み出しMPを大幅に回復させる。また自分の周囲を明るく照らす。"},
	{1,8,20,FALSE,FALSE,A_DEX,0,0,"レーザー",
		"閃光属性のビームを放つ。"},
	{7,12,30,FALSE,FALSE,A_STR,0,0,"プチフレア",
		"火炎属性のボールを放つ。射程はやや短い。"},
	{12,18,35,FALSE,FALSE,A_DEX,0,0,"ハイテンションブレード",
		"プラズマ属性のビームを放つ。"},
	{16,21,50,TRUE,FALSE,A_STR,0,15,"ロケットダイブ",
		"一直線に突進し、小さな敵は吹き飛ばす。レベルが上がると射程が伸びる。"},
	{20,24,50,FALSE,FALSE,A_STR,0,0,"メガフレア",
		"核熱属性の大きなボールを放つ。"},
	{25,30,65,FALSE,FALSE,A_DEX,0,0,"核熱バイザー",
		"酸・電撃・火炎・冷気への一時的な耐性を得る。"},

	{27,38,60,FALSE,FALSE,A_DEX,0,0,"地獄波動砲",
		"障害物に当たると炸裂する分解属性の球を放つ。"},
	{30,50,60,FALSE,FALSE,A_STR,0,0,"ニュークリアフュージョン",
		"視界内のすべてに対して核熱属性攻撃を放つ。"},
	{34,65,70,FALSE,FALSE,A_WIS,0,0,"十凶星",
		"周囲のランダムな位置に強力な火炎属性ボールを連続で発生させる。"},
	{36,80,55,FALSE,FALSE,A_CON,0,0,"ヘルズトカマク",
		"周囲を*破壊*する。レベルが上がると範囲が広がる。"},
	{40,120,75,FALSE,FALSE,A_INT,0,0,"フィクストスター",
		"閃光属性の球を放つ。それは着弾地点から7/8の確率でランダムなターゲットへ再発動する。"},
	{44,240,85,FALSE,FALSE,A_STR,0,0,"ギガフレア",
		"極めて強力な核熱属性のレーザーを放つ。"},

//	{48,450,80,FALSE,TRUE,A_STR,0,0,"アビスノヴァ",
//		"周囲の全てに対して極めて強力な核熱属性攻撃を放つ。ただし使用から発動までに15行動程度のタイムラグがあり、発動まで他の技が使えない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


/*:::空専用特技*/
cptr do_cmd_class_power_aux_utsuho(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //核融合
		{
			if(only_info) return format("");

			if(one_in_(10)) msg_print("元気な水素が絶賛融合中だ！");
			else if(one_in_(9)) msg_print("究極のエネルギーが身体に満ちる！");
			else msg_print("体内で核反応が活発化した！");

			p_ptr->csp += p_ptr->msp / 8 + randint1(p_ptr->msp / 8);
			if(p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			lite_room(py, px);
			p_ptr->redraw |= (PR_MANA);
			p_ptr->window |= (PW_PLAYER);
			break;
		}
	case 1: //レーザー
		{
			int dam = 20 + plev + chr_adj;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print("ビームを撃った。");
			fire_beam(GF_LITE,dir, dam);
			break;
		}

	case 2: //プチフレア
		{
			int range = 5 + plev / 10;
			int dice = 7 + plev / 3;
			int sides = 10 + chr_adj / 6;

			if(only_info) return format("射程:%d 損傷:%dd%d",range,dice,sides);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("火球を撃ち出した。");
			fire_ball(GF_FIRE,dir,damroll(dice,sides),1);

			break;
		}
	case 3: //ハイテンションブレード
		{
			int dam = 30 + plev * 2 + chr_adj * 3;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print("高エネルギー荷電粒子を発生させた！");
			fire_beam(GF_PLASMA,dir, dam);
			break;
		}


	case 4: //ロケットダイブ
		{
			int len = 5 + p_ptr->lev / 10;
			damage = plev + adj_general[p_ptr->stat_ind[A_STR]] *5;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack3(len,"あなたは砲弾のように飛んだ！",damage)) return NULL;
			break;
		}

	case 5: //メガフレア
		{
			int dice = 15 + plev / 2;
			int sides = 10 + chr_adj / 4;

			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("カードから巨大な火球が放たれた！");
			else
				msg_print("巨大な火球を放った！");
			fire_ball(GF_NUKE,dir,damroll(dice,sides),4);

			break;
		}
	case 6: //核熱バイザー
		{
			int base = 5 + plev / 5 ;
			int v;
			if(only_info) return format("期間:%d+1d%dターン",base,base);
			msg_format("エネルギーの障壁を張った！");
			v = base + randint1(base);
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);

			break;
		}


	case 7: //地獄波動砲
		{
			int base = plev*3 + chr_adj * 3;

			if(only_info) return format("損傷:%d",base);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("原子分解の球を放った！");
			fire_ball(GF_DISINTEGRATE,dir,base,3);

			break;
		}
	case 8: //ニュークリアフュージョン
		{
			int damage = 50 + plev*6 + chr_adj * 5;
			if(only_info) return format("損傷:%d",damage);
			msg_print("閃光と光熱が周囲を蹂躙した！");
			project_hack2(GF_NUKE,0,0,damage);
			break;
		}
	case 9: //十凶星
		{
			int rad = 2 + plev / 16;
			int damage = plev*3  + chr_adj*3 ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("天から幾つもの巨大な火球が降り注いだ。");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}
	case 10: //ヘルズトカマク
		{
			int sides = plev / 5;
			int base = 12;
			if(only_info) return format("範囲:%d+1d%d",base,sides);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			break;
		}

	case 11://フィクストスター
		//v1.1.44 ルーチン見直し
		{

			int tx,ty;
			int dam = plev * 2 + chr_adj * 2;
			int rad = 3;

			if(only_info) return format("損傷:%d * 不定",dam);

			if (!get_aim_dir(&dir)) return NULL;

			msg_format("眩く輝く光球を放った！");
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_LITE, rad);

		}
		break;
	case 12://ギガフレア
		{
			dice = p_ptr->lev;
			sides = 35 + chr_adj / 2;
			if(dice < 30) dice = 30;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("神の火が前方の全てを焼き払った！");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),2);

			break;
		}







	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::正邪専用技*/
//ここのレベルや順番を触った時calc_mana()の特殊処理部も変更要
//p_ptr->magic_num1(2)[97～105]を使う。ソレ以前の部分は魔道具術師スキル用に割り振る
class_power_type class_power_seija[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"魔道具隠匿",
		"杖・魔法棒・ロッドをどこかに隠し持つ。隠し持った魔道具はmコマンドから使用できる。ロッドのみは時間経過で使用回数が回復する。"},
	{3,2,30,FALSE,TRUE,A_INT,0,5,"魔力食わせ",
		"隠し持った杖・魔法棒・ロッドの使用回数を充填する。一度の充填量はレベル・知能・アイテムレベル・最大保有数が影響し、高レベルな魔道具は一度の充填では使用回数が増えないことがある。"},
	{5,0,0,FALSE,FALSE,A_DEX,0,0,"ひらり布",
		"不思議道具の一つを使う。敵から攻撃を受けたとき自動的に6/7の確率でダメージを0にする。ステータス異常などは受ける。"},
	{7,12,30,FALSE,TRUE,A_INT,0,0,"イビルインザミラー",
		"周囲の敵を高確率で混乱させる。"},
	{9,0,0,FALSE,FALSE,A_DEX,0,0,"隙間の折りたたみ傘",
		"不思議道具の一つを使う。壁に向けて使うと壁の向こうへテレポートする。マップ端へ向けた場合マップの反対側へ出る。通常のテレポートで侵入できない地形には出てこれない。"},
	{13,0,0,FALSE,FALSE,A_DEX,0,0,"呪いのデコイ人形",
		"不思議道具の一つを使う。発動後に一度だけ敵からの攻撃をデコイ人形が受けそのとき自分はテレポートする。"},
	{16,0,0,FALSE,FALSE,A_DEX,0,0,"四尺マジックボム",
		"不思議道具の一つを使う。近くに巨大な分解属性のボールを発生させる。"},
	{20,16,40,FALSE,TRUE,A_DEX,0,0,"リバースヒエラルキー",
		"敵一体と位置を交換する。視界外の敵にも効果がある。"},
	{24,0,0,FALSE,FALSE,A_DEX,0,0,"天狗のトイカメラ",
		"不思議道具の一つを使う。敵からボルト魔法を受けたとき自動的に9/10の確率で回避する。"},
	{28,0,0,FALSE,FALSE,A_DEX,0,0,"血に飢えた陰陽玉",
		"不思議道具の一つを使う。視界内のターゲットの位置へテレポートする。"},
	{32,0,0,FALSE,FALSE,A_DEX,0,0,"打出の小槌(レプリカ)",
		"不思議道具の一つを使う。隣接した敵一体に大ダメージを与える。"},
	{36,0,0,FALSE,FALSE,A_DEX,0,0,"身代わり地蔵",
		"不思議道具の一つを使う。敵からの攻撃で体力が半分を下回った時に自動的に発動し、体力を完全に回復させる。体力が0未満になったときには発動しない。"},
	{40,0,0,FALSE,FALSE,A_DEX,0,0,"亡霊の送り提灯",
		"不思議道具の一つを使う。一定時間幽体化し、壁を抜けることが可能になり敵からの攻撃のほとんどの威力が半減する。閃光や破邪の攻撃を受けると幽体化が解除される。"},
	{45,72,70,FALSE,TRUE,A_CHR,0,0,"天下転覆",
		"視界内の全てに強力な重力属性攻撃を行う。浮遊していない巨大な敵に効果が大きい。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_seija(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			gain_magic();
			break;
		}
	case 1: //魔道具充填
		{
			int power = (50+plev*2) * (10+adj_general[p_ptr->stat_ind[A_INT]]*3) * (100+randint1(200));

			if(only_info) return "";
			if(!recharge_magic_eater(power)) return NULL;

			break;
		}
	case 2:
		{
			if(only_info) return format("残り回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_HIRARI],p_ptr->magic_num2[SEIJA_ITEM_HIRARI]);
			msg_print("この道具は自動発動する");
			return NULL;
			break;
		}
	case 3:
		{
			int power = 50 + plev * 4 + chr_adj * 5;
			if(only_info) return format("効力:%d",power);
			msg_format("あなたは周囲の敵の視界を狂わせた！");
			confuse_monsters(power);
			break;
		}
	case 4:
		{
			int x,y;
			int attempt = 500;
			if(only_info)
			{
				if(use_itemcard)
					return "";
				else
					return format("残り回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_SUKIMA],p_ptr->magic_num2[SEIJA_ITEM_SUKIMA]);
			}

			if (p_ptr->anti_tele)
			{
				msg_format("今はスキマによる移動ができない。");
				return NULL;
			}


			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
			{
				msg_format("そこには壁がない。");
				return NULL;
			}
			while(1)
			{
				attempt--;
				y += ddy[dir];
				x += ddx[dir];

				if(y >= cur_hgt) y = 1;
				if(y < 1) y = cur_hgt - 1;
				if(x >= cur_wid) x = 1;
				if(x < 1) x = cur_wid - 1;

				if(attempt<0)
				{
					msg_format("スキマ移動に失敗した！");
					teleport_player(200,0L);
					break;
				}
				if(!cave_player_teleportable_bold(y, x, 0L)) continue;
				msg_format("あなたはスキマを開いて反対側に抜けた！");
				teleport_player_to(y,x,0L);
				break;
			}

			if(!use_itemcard) p_ptr->magic_num1[SEIJA_ITEM_SUKIMA] -= 1;

			break;
		}
	case 5:
		{
			if(only_info) return format("使用回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_DECOY],p_ptr->magic_num2[SEIJA_ITEM_DECOY]);
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("あなたはこっそりと仕込みを済ませた・・");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->magic_num1[SEIJA_ITEM_DECOY] -= 1;
			}
			else
			{
				msg_print("既にデコイ人形の準備は済んでいる。");
				return NULL;
			}
			break;
		}
	case 6:
		{
			int rad = 3 + plev / 15;
			base = 100 + plev * 3;
			if(only_info) return format("使用回数:%d/%d 損傷:%d",p_ptr->magic_num1[SEIJA_ITEM_BOMB],p_ptr->magic_num2[SEIJA_ITEM_BOMB],base);

			project_length = 4;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("あなたは爆弾に点火し、投げつけた！");
			fire_ball(GF_DISINTEGRATE,dir,base,rad);
			p_ptr->magic_num1[SEIJA_ITEM_BOMB] -= 1;

			break;
		}
	case 7:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
	case 8:
		{
			if(only_info) return format("残り回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_CAMERA],p_ptr->magic_num2[SEIJA_ITEM_CAMERA]);
			msg_print("この道具は自動発動する");
			return NULL;
			break;
		}
	case 9:
		{
			int x, y;
			if(only_info) return format("残り回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_ONMYOU],p_ptr->magic_num2[SEIJA_ITEM_ONMYOU]);
			//if (!tgt_pt(&x, &y)) return NULL;
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			if (p_ptr->anti_tele)
			{
				msg_print("不思議な力がテレポートを防いだ！");
			}
			else
			{
				msg_print("あなたは離れた場所に一瞬で現れた。");
				teleport_player_to(target_row, target_col, 0L);
			}
			p_ptr->magic_num1[SEIJA_ITEM_ONMYOU] -= 1;
			break;
		}
	case 10://小槌レプリカ
		{
			int y, x;
			monster_type *m_ptr;
			base = plev * 6;
			sides = plev * 6;

			if(base < 150) base = 150;
			if(sides < 150) sides = 150;

			if(only_info)
			{
				if(use_itemcard)
					return format("損傷:1d%d + %d",base,sides);
				else
					return format("残り回数:%d/%d 損傷:1d%d + %d",p_ptr->magic_num1[SEIJA_ITEM_KODUCHI],(int)p_ptr->magic_num2[SEIJA_ITEM_KODUCHI],base,sides);

			}

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("あなたは小槌を振りかぶり、%sに向けて叩き付けた！",m_name);
				project(0, 0, y, x, randint1(sides) + base, GF_DISP_ALL, flg, -1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			if(!use_itemcard) p_ptr->magic_num1[SEIJA_ITEM_KODUCHI] -= 1;

			break;
		}
	case 11:
		{
			if(only_info) return format("残り回数:%d/%d",p_ptr->magic_num1[SEIJA_ITEM_JIZO],p_ptr->magic_num2[SEIJA_ITEM_JIZO]);
			msg_print("この道具は自動発動する");
			return NULL;
			break;
		}
	case 12:
		{
			base = 15;
			if(only_info) return format("残り回数:%d/%d 期間:%d + 1d%d",p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN],p_ptr->magic_num2[SEIJA_ITEM_CHOCHIN],base,base);
			set_wraith_form(randint1(base) + base, FALSE);
			p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN] -= 1;
			break;
		}
	case 13:
		{
			int dam = plev * 5 + chr_adj * 5;

			if(only_info) return format("損傷:%d",dam);
			msg_format("あなたは天地をひっくり返した！");
			project_hack2(GF_GRAVITY,0,0,dam);
			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::ぬえ専用技*/
class_power_type class_power_nue[] =
{
	{3,3,10,FALSE,TRUE,A_CON,0,0,"アンディファインドダークネス",
		"今いる部屋を暗くする。"},
	{12,12,30,FALSE,TRUE,A_INT,0,3,"軌道不明の鬼火",
		"周囲のランダムな位置に火炎属性のボールを連続で発生させる。"},
	{20,30,50,FALSE,TRUE,A_CHR,0,0,"正体不明の種",
		"敵一体に正体不明の種を仕込む。正体不明になった敵は他の敵から攻撃されるようになる。カオス耐性を持つ敵には効果がなく、ユニークモンスターや高レベルな敵には効きにくい。"},
	{28,36,60,FALSE,TRUE,A_INT,0,2,"原理不明の妖怪玉",
		"指定した位置にプラズマ属性の巨大なボールを発生させる。"},
	{34,45,65,FALSE,TRUE,A_CON,0,0,"平安のダーククラウド",
		"視界内の全てに対し暗黒属性攻撃をする。"},
	{42,70,75,FALSE,TRUE,A_CHR,0,0,"紫鏡",
		"一定時間分身を作り出し攻撃の半分を回避する。"},
	{47,90,75,FALSE,TRUE,A_CHR,0,0,"遊星よりの弾幕Ｘ",
		"視界内の全てに対し、完全にランダムな属性の強力な攻撃を行う。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


cptr do_cmd_class_power_aux_nue(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return "";
			msg_format("あなたの身体から黒い霧が湧き出した。");
			unlite_room(py,px);
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 24;
			int damage = 10 + plev  + chr_adj / 2 ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("あなたの周りを火球が飛び回った。");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}

	case 2://正体不明の種
		{
			int y, x;
			monster_type *m_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];
					int chance = 40 + plev * 2 + chr_adj * 2;
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					if(use_itemcard && chance < 100) chance=100;

					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance /= 2;

					if(r_ptr->flagsr & (RFR_RES_CHAO | RFR_RES_ALL))
					{
						msg_format("%sには効果がない！",m_name);
					}
					else if(m_ptr->mflag & MFLAG_NUE_UNKNOWN)
					{
						msg_format("既に効いている。");

					}
					else if( randint1(chance) < r_ptr->level)
					{
						msg_format("%sに正体不明の種を仕込むのに失敗した！",m_name);
					}
					else
					{

						msg_format("%sに正体不明の種を仕込んだ！",m_name);
						m_ptr->mflag |= MFLAG_NUE_UNKNOWN;
						lite_spot(m_ptr->fy,m_ptr->fx);

					}
				}
				else
				{
					msg_format("そこには何もいない。");
					return NULL;
				}

			break;
		}
	case 3://原理不明の妖怪玉
		{
			int rad = 2 + plev / 15;
			dice = 8 + p_ptr->lev / 3;
			sides = 12 + chr_adj / 2 ;
			base = p_ptr->lev*4;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_PLASMA, dir, base + damroll(dice,sides), rad,"光球の群れが襲いかかった！")) return NULL;
			break;
		}
	case 4:
		{
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("損傷:%d",dam);
			msg_format("辺りは濃密な闇に包まれた・・");
			project_hack2(GF_DARK,0,0,dam);
			break;
		}
	case 5:
		{
			dice = base = 8;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			set_multishadow(base + randint1(dice),FALSE);
			break;
		}
	case 6:
		{
			int base = plev * 5 + chr_adj * 5;

			if(base < 300) base=300;
			if(only_info) return format("損傷:%d+1d%d",base,base);
			msg_format("あなたは異界の正体不明のパワーを呼び出した！");
			project_hack2(0,1,base,base);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::白蓮専用技*/
class_power_type class_power_byakuren[] =
{

	{3,6,50,FALSE,TRUE,A_INT,0,20,"転読",
		"肉体強化魔法の記された自動読上げ巻物を発動させる。身体能力と格闘技能が上昇し、レベルによって様々な能力や耐性を得る。"},
	{10,10,35,FALSE,TRUE,A_WIS,0,3,"インドラの雷",
		"ターゲットの位置に雷を落として電撃攻撃をする。"},
	{15,0,0,FALSE,TRUE,A_WIS,0,0,"瞑想",
		"精神を研ぎ澄ませ、MPを僅かに回復する。"},
	{18,20,50,FALSE,TRUE,A_INT,0,0,"ハリの制縛",
		"周囲の敵を朦朧、金縛りにしようと試みる。"},
	{23,30,60,TRUE,FALSE,A_DEX,0,0,"スカンダの脚",
		"離れた敵に一瞬で近寄って攻撃する。転読による強化中にしか使えない。"},
	{27,18,55,FALSE,TRUE,A_INT,0,3,"スターメイルシュトロム",
		"近くの敵に閃光属性攻撃を行う。レベルが上がると威力が上がり効果範囲が広がる。"},
	{30,30,45,FALSE,TRUE,A_INT,0,0,"スターソードの護法",
		"短時間の間、直接攻撃をしてきた敵に破片属性ダメージを与えるようになる。"},
	{36,48,70,FALSE,TRUE,A_INT,0,0,"ブラフマーの瞳",
		"広範囲の地形、アイテム、モンスター、トラップを感知する。"},
	{40,90,80,FALSE,TRUE,A_INT,0,0,"アーンギラサヴェーダ",
		"閃光属性の強力なビームを放つ。"},
	{45,120,80,FALSE,TRUE,A_INT,0,0,"超人聖白蓮",
		"転読による強化中にしか使えない。短時間大幅に加速し攻撃回数が上昇する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_byakuren(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int wis_adj = adj_general[p_ptr->stat_ind[A_WIS]];

	switch(num)
	{

	case 0:
		{
			int time = 10 + plev / 5;
			int dec_hp = p_ptr->mhp - p_ptr->chp;
			if(only_info) return format("期間:%d",time);

			if(one_in_(3))msg_print("光る文字が空中を高速で流れた。");
			else if(one_in_(2))msg_print("エア巻物を起動させた。");
			else msg_print("「いざ、南無三ー！」");
			if(plev > 29) set_fast(time,FALSE);
			if(plev > 34)
			{
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
			if(!use_itemcard) set_tim_general(time,FALSE,0,0);
			set_tim_addstat(A_STR,105+plev/10,time,FALSE);
			set_tim_addstat(A_DEX,105+plev/10,time,FALSE);
			set_tim_addstat(A_CON,105+plev/10,time,FALSE);
			p_ptr->chp = p_ptr->mhp - dec_hp;
			p_ptr->redraw |= PR_HP;
			handle_stuff();
			break;
		}

	case 1:
		{
			int rad = 2;
			if(plev > 39) rad = 3;
			dice = 2 + plev / 7;
			sides = 10 + chr_adj / 6;
			base = 20 + plev + chr_adj ;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC, dir, base + damroll(dice,sides), rad,"雷が落ちた！")) return NULL;
			break;

		}
	case 2:
		{
			if(only_info) return format("");
			msg_print("精神を研ぎ澄ませた・・");
			p_ptr->csp += (3 + plev/15);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
	case 3:
		{
			int power = plev * 2;
			if(power < 30) power=30;
			if(only_info) return format("効果:%d",power);
			msg_print("光が降り注ぎ、周囲の者の動きを縫い止めた。");
			stun_monsters(power);
			stasis_monsters(power);
			break;

		}
	case 4:
		{
			int len = 5 + p_ptr->lev / 7;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 5:
		{
			int base = 20 + plev + chr_adj;
			int num = 3 + plev / 15 + chr_adj / 20;
			int rad = 2;
			int i;
			if(only_info) return format("損傷:最大%d * %d",base/2,num);
			msg_print("あなたの周りに光線が渦を巻いた！");
			for (i = 0; i < num; i++)
			{
				project(0, rad, py, px, base, GF_LITE, PROJECT_KILL, -1);
				rad = MIN(7,rad+1);
			}
			break;
		}
	case 6:
		{
			int time = 20;
			if(only_info) return format("期間:%d",time);
			msg_format("あなたの周りに無数の護符が浮かんだ。");
			set_dustrobe(time,FALSE);
		}
		break;
	case 7:
		{
			int range = plev;
			if(only_info) return format("範囲:%d",range);
			map_area(plev);
			detect_all(plev);

		}
		break;
	case 8:
		{
			int damage = p_ptr->lev * 6 + chr_adj * 5 + wis_adj * 5;
			if(damage < 450) damage = 450;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("調伏の光が前方を焼き払った！");
			fire_spark(GF_LITE,dir,damage,2);

			break;
		}
	case 9:
		{
			int time = 5;
			if(only_info) return format("期間:%d",time);
			msg_print("強化の魔法を極限まで高めた！");
			if(p_ptr->tim_general[0] < time) p_ptr->tim_general[0] = time;
			set_lightspeed(time, FALSE);
			redraw_stuff();
		}
		break;


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::にとり専用技*/
class_power_type class_power_nitori[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械格納",
		"リュックに機械を格納する。欄の数はレベルアップで上昇する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械確認",
		"リュックの機械を確認したり説明を見る。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"装備解除",
		"リュックから機械を出す。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械使用",
		"リュックに格納した機械を使用する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"弾薬補充",
		"リュックに格納した機械に弾薬などを補充し使用回数を回復させる。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"水妖エネルギー補充",
		"一部特技で消費するエネルギーを補充する。水上にいないと使えない。"},
	{5,0,0,FALSE,FALSE,A_DEX,0,0,"大型水鉄砲",
		"ターゲット周辺に水属性のボルトを乱射する。レベルが上がると数が増える。射撃数分の水妖エネルギーを消費する。"},
	{12,16,30,FALSE,TRUE,A_WIS,0,3,"河童のポロロッカ",
		"自分を中心とした巨大な水属性のボールを発生させる。水上にいないと使えない。"},
	{16,0,0,FALSE,FALSE,A_DEX,0,0,"クリミナルギア",
		"短時間、直接攻撃してきた敵に破片属性ダメージを与えるようになる。水妖エネルギー50を消費する。"},
	{21,0,0,FALSE,FALSE,A_DEX,0,0,"キューリサウンドシステム",
		"轟音属性のビームを発射する。水妖エネルギー30を消費する。"},
	{27,0,0,FALSE,FALSE,A_DEX,0,0,"菊一文字コンプレッサー",
		"特殊な形状に爆発する水属性攻撃を放つ。威力は高いが射程は短い。水妖エネルギー200を消費する。"},
	{33,40,60,FALSE,TRUE,A_WIS,0,5,"シライトフォール",
		"自分を中心としたランダムな位置に水属性の球を連続で発生させる。"},
	{38,0,0,FALSE,TRUE,A_INT,0,0,"オプティカルカモフラージュ",
		"一定時間、透明になって敵から認識されにくくなる。ただし視覚に頼らない敵には効果が薄い。水妖エネルギー300を消費する。"},
	{44,0,0,FALSE,TRUE,A_INT,0,0,"ケゴンガン",
		"敵や障害物に当たると爆発する水属性の弾を連射する。水妖エネルギー400を消費する。"},
	{48,96,80,FALSE,TRUE,A_WIS,0,0,"光り輝く水底のトラウマ",
		"視界内全てに強力な水属性攻撃を行い、地形を水にする。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//にとり特技の必要水妖エネルギー　添字がclass_power_nitoriと一致し、エネルギー(p_ptr->magic_num1[20])がこの値未満の技は使えない
int nitori_energy[] =
{
	0,0,0,0,0,0,
	1,0,50,30,200,
	0,300,400,0

};

cptr do_cmd_class_power_aux_nitori(int skillnum, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(skillnum)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			(void) use_machine(0);
			return NULL; //見るだけなので行動順消費なし

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if( !use_machine(2)) return NULL;

			break;
		}
	case 4: //弾薬補充
		{
			if(only_info) return format("");
			if( !use_machine(1)) return NULL;
			break;
		}
	case 5:
		{
			if(only_info) return format("現在エネルギー:%d",p_ptr->magic_num1[20]);
			if(p_ptr->magic_num1[20] == NITORI_ENERGY_MAX)
			{
				msg_print("既に十分に水を入れている。");
				return NULL;
			}
			msg_print("あなたはリュックに水を汲めるだけ汲んだ！");
			p_ptr->magic_num1[20] = NITORI_ENERGY_MAX;
			break;
		}
	case 6:
		{
			int num = 2 + plev / 5;
			int dice = 1;
			int sides = 3 + plev / 8 + chr_adj / 10;
			if(!use_itemcard && p_ptr->magic_num1[20] < num) num = p_ptr->magic_num1[20];

			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("水鉄砲を乱射した！");
			fire_blast(GF_WATER,dir,dice,sides,num,3,( PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID));
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
			if(!use_itemcard) p_ptr->magic_num1[20] -= num;

			break;
		}
	case 7:
		{
			int base = 50 + plev * 4 + chr_adj * 3;
			int rad = 4 + plev / 12;

			if(only_info) return format("損傷:最大%d",base/2);
			msg_print("突然水が盛り上がり噴き出した！");

			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;
	case 8:
		{
			int time = 20;
			if(only_info) return format("期間:%d",time);
			msg_format("リュックからギアが飛び出し回転を始めた。");
			set_dustrobe(time,FALSE);
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
		}
		break;
	case 9:
		{
			int dice = 1;
			int sides = plev*2;

			if(only_info) return format("損傷:1d%d",sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("リュックの横からスピーカーが飛び出し、音波を放った！");
			fire_beam(GF_SOUND,dir,randint1(sides));
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
		}
		break;
	case 10:
		{
			int range = 5;
			int dam = 50 + plev * 3;
			int tx, ty;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_COMPRESS;

			if(dam < 120) dam = 120;
			if(only_info) return format("射程:%d 損傷:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("外部圧縮機から高圧の水が噴射された！");
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				flg &= ~(PROJECT_STOP);
				tx = target_col;
				ty = target_row;
			}
			project(0, 5, ty, tx, dam, GF_WATER, flg, -1);
			if(!use_itemcard) p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;

			break;
		}
	case 11:
		{
			int dam = 20 + plev + chr_adj;
			if(only_info) return format("損傷:%d*不定",dam);
			msg_print("多量の水塊が滝のように降り注いだ！");
			cast_meteor(dam, 3,GF_WATER);
		}
		break;
	case 12:
		{
			int base = 20;
			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_format("光学迷彩を起動した！");
			set_tim_superstealth(base + randint1(base),FALSE, SUPERSTEALTH_TYPE_OPTICAL);
			if(!use_itemcard) p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			break;
		}
	case 13:
		{
			int num = 8;
			int dice = 3;
			int sides = plev/2;
			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("激しく放水した！");
			for (i = 0; i < num; i++) fire_rocket(GF_WATER, dir, damroll(dice, sides),3);
			p_ptr->magic_num1[20] -= nitori_energy[skillnum];
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) new_class_power_change_energy_need = 80;
			break;

		}


	case 14:
		{
			int damage = plev * 5 + chr_adj * 5;
			if(only_info) return format("損傷：%d",damage);
			msg_format("辺りは一瞬にして深い水の底になった。");
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,damage);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

		}
		break;
	}

	return "";
}


/*:::レミリア用特技*/
/*:::ミゼラブルフェイトにmagic_num1[0]とtim_general[0]を使用する*/
class_power_type class_power_remy[] =
{
	{10,10,30,FALSE,TRUE,A_INT,0,6,"スターオブダビデ",
		"自分の周辺のランダムな場所に複数回の無属性攻撃を行う。"},
	{15,15,30,FALSE,FALSE,A_DEX,30,0,"マイハートブレイク",
		"武器などを強力に投擲する。"},
	{20,16,40,FALSE,FALSE,A_STR,0,7,"スカーレットシュート",
		"敵や障害物に当たると炸裂する球を投げつける。清浄な体を持つ敵に大きなダメージを与える。"},
	{23,36,50,FALSE,TRUE,A_CON,0,0,"ヴァンピリッシュナイト",
		"大量の吸血蝙蝠を出現させる。吸血蝙蝠の攻撃によりHPを回復することができる。蝙蝠はフロア移動で消滅し捕獲などが不可能。"},
	{27,20,45,TRUE,FALSE,A_DEX,0,10,"バッドレディスクランブル",
		"指定した敵へ突進しダメージを与え朦朧とさせる。障害物に当たると止まる。"},
	{32,40,65,FALSE,TRUE,A_STR,0,0,"ミゼラブルフェイト",
		"敵一体を自分の近くに引き寄せ、さらに移動とテレポートを阻害する。束縛から脱出されることもあり、巨大な敵や力強い敵には効きにくい。"},
	{36,50,75,FALSE,TRUE,A_CHR,0,0,"ミレニアムの吸血鬼",
		"隣接した敵から大幅にHPを吸収し、満腹度を回復させる。人間型の敵にしか使えない。"},
	{40,80,80,FALSE,TRUE,A_STR,0,0,"スピア・ザ・グングニル",
		"バリアを貫通する強力なビーム攻撃を放つ。『グングニル』を装備していると威力が上がる。"},
	{45,60,70,FALSE,TRUE,A_DEX,0,0,"きゅうけつ鬼ごっこ",
		"近くのランダムな敵の所に移動し魔力属性の球を発生させる。6/7の確率で再発動し、そのたびに少しづつ爆発が強力になる。"},
	{48,180,85,FALSE,TRUE,A_CHR,0,0,"レッドマジック",
		"強力な視界内攻撃を行う。威力は魅力に大きく依存し、清浄な体を持つ敵に大きなダメージを与える。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_remy(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //スターオブダビデ
		{
			int dam = plev*2 + chr_adj + 20;
			if(only_info) return format("損傷:%d*不定",dam);
			cast_meteor(dam, 2,GF_DISP_ALL);
		}
		break;

	case 1: //マイハートブレイク
		{
			int mult = 2 + plev / 15;
			if(only_info) return format("倍率:%d",mult);
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
		}
		break;

	case 2://スカーレットシュート
		{
			int dam = 50 + plev * 2 + chr_adj * 10 / 3;
			int rad = 1 + plev / 40;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("カードがロケットのように飛んでいった！");
			else
				msg_print("真紅の魔力弾を放った！");

			fire_rocket(GF_REDMAGIC, dir, dam, rad);
		}
		break;

	case 3: //ヴァンピリッシュナイト
		{
			int num = plev / 3;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_REMY_BAT, (PM_FORCE_PET | PM_EPHEMERA))) flag = TRUE;
			}
			if(flag) msg_format("体の一部を大量の蝙蝠に変えた！");
			else msg_format("蝙蝠を出すのに失敗した。");

		}
		break;
	case 4: //バッドレディスクランブル
		{
			int len = 5 + plev / 10;
			int damage = 50 + plev * 2;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack2(len,GF_FORCE,damage)) return NULL;
			break;
		}
		break;

	case 5: //ミゼラブルフェイト
		{
			int idx;
			monster_type *m_ptr;
			char m_name[80];
			if(only_info) return "";

			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;
			m_ptr = &m_list[idx];
			monster_desc(m_name, m_ptr, 0);
			if(distance(py,px,m_ptr->fy,m_ptr->fx) > 1) msg_format("%sを引き寄せるのに失敗した。",m_name);
			else
			{
				int base = 20;
				msg_format("紅い鎖が飛び、%sを束縛した！",m_name);
				p_ptr->magic_num1[0] = idx;
				set_tim_general(base,FALSE,0,0);
			}
		}
		break;
	case 6: //ミレニアムの吸血鬼
		{
			int y, x;
			int dam = plev * 4 + chr_adj * 5;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("効力:%d",dam);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int food = 0;
				char m_name[80];

				r_ptr = &r_info[m_ptr->r_idx];

				monster_desc(m_name, m_ptr, 0);
				if(!monster_living(r_ptr) || r_ptr->flags2 & (RF2_COLD_BLOOD) || !(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) )
				{
					msg_format("それは美味しくなさそうだ。");
					return NULL;
				}
				else
				{
					msg_format("あなたは%sへ抱きつき、首筋へ噛み付いた！",m_name);

					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
					{
						if(r_ptr->flags3 & RF3_HUMAN)
							msg_format("この血は最高に美味だ！");
						else
							msg_format("意外に悪くない味だ。",m_name);
					}
					else
					{
						cptr msg[12] = {
							"好物のB型だ！",
							"ビタースイートな大人の味わいだ。",
							"痩せた味だ。意外に禁欲的な生活のようだ。",
							"やや貧血気味だ。",
							"まったりとしてそれでいてしつこくない。",
							"人生の妙味を感じさせる複雑な味わいだ。",
							"血中インスリン濃度が高めだ。",
							"やや高血糖気味だ。",
							"酒臭い。一杯やったばかりらしい。",
							"少し脂っこいが悪くない。",
							"喉越し爽やかだ。コクがあるのにキレがある。",
							"・・不味い。この者には食生活の改善が必要だ。"
						};
						msg_format("%s",msg[cave[y][x].m_idx % 12]);
					}

					if(m_ptr->hp < dam) dam = m_ptr->hp+1;

					if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX - 1, (p_ptr->food + dam * 10)));
					hp_player(dam);
					project(0,0,y,x,dam,GF_DISP_ALL,PROJECT_KILL,0);
				}
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;

	case 7: //スピアザグングニル
		{
			int base = plev*3 + chr_adj * 3;
			if(base < 150) base = 150;
			if(inventory[INVEN_RARM].name1 == ART_GUNGNIR || inventory[INVEN_LARM].name1 == ART_GUNGNIR) base *= 2;
			if(only_info) return format("損傷:%d+1d%d",base,base);

			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_format("カードが光の槍になって飛んで行った！");
			else
				msg_format("光の槍を投げ放った！");
			fire_beam(GF_GUNGNIR,dir, base+randint1(base));

			break;
		}

	case 8: //きゅうけつ鬼ごっこ スターファングからさらに改変
		{
			int basedam = (plev + chr_adj) / 2;
			int baserad = 1;
			int range = 12;
			bool flag_msg = FALSE;
			if(only_info) return format("損傷:%d*不定*不定",basedam);

			msg_print("あなたは蝙蝠に変身して飛んだ・・");
			do
			{
				int i, path_n;
				int tx,ty;
				int target_m_idx = 0;
				int target_num = 0;
				u16b path_g[32];
				bool moved = FALSE;
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if(m_ptr->cdis > range) continue;
					if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					if (!is_hostile(m_ptr)) continue;
					target_num++;
					if(!one_in_(target_num)) continue;
 					target_m_idx = i;
				}
				if(!target_m_idx) break;
				tx = m_list[target_m_idx].fx;
				ty = m_list[target_m_idx].fy;
				project_length = 14;
				path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
				project_length = 0;
				if (!path_n) break;
				ty = py;
				tx = px;

				/* Project along the path */
				for (i = 0; i < path_n; i++)
				{
					monster_type *m_ptr;
					int ny = GRID_Y(path_g[i]);
					int nx = GRID_X(path_g[i]);

					if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
					{
						ty = ny;
						tx = nx;
						continue;
					}
					if (!cave[ny][nx].m_idx)
					{
						msg_print("壁に激突した。");
						break;
					}

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					update_mon(cave[ny][nx].m_idx, TRUE);

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					moved = TRUE;
					project(0, baserad, ny, nx, basedam, GF_MANA, PROJECT_KILL | PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM, -1);
					flag_msg = TRUE;
					break;
				}
				if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
				basedam = basedam * 11 / 10;
				if(one_in_(2) && baserad < 6) baserad++;

				if(p_ptr->confused || p_ptr->chp < 0 || p_ptr->paralyzed ) break;
			}while(!one_in_(7));

			if(!flag_msg) msg_print("・・しかし近くに誰も見当たらなかった。");

		}
		break;

	case 9: //レッドマジック
		{
			int dice = plev;
			int sides =  chr_adj;
			if(sides < 10) sides = 10;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if(use_itemcard)
				msg_format("カードから濃密な真紅の霧が噴き出した...");
			else
				msg_format("あなたは胸の前で何かを包み込むような仕草をした・・");
			project_hack2(GF_REDMAGIC,dice,sides,0);
			break;
		}
		break;



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}













/*:::穣子特技*/
class_power_type class_power_minoriko[] =
{
	{	10,10,35,FALSE,TRUE,A_CHR,0,2,"オータムスカイ",
		"自分を中心とした大きなボールを発生させる。威力は魅力に大きく依存し、属性は水、風、冷気、電撃のいずれかランダムになる。"	},
	{	24,24,50,TRUE,FALSE,A_DEX,0,10,"オヲトシハーベスター",
		"隣接したモンスター全てに攻撃する。大鎌を装備していないと使えず、威力は大鎌による通常攻撃力に準ずる。"},
	{	32,30,70,TRUE,FALSE,A_WIS,0,0,"スイートポテトルーム",
		"「焼き芋」を生成する。食べると満腹度とMPが回復する。"	},
	{	40,48,80,FALSE,TRUE,A_CHR,0,0,"ウォームカラーハーヴェスト",
		"周囲に気属性のボールを多数発生させる。威力は魅力に大きく依存する。"},
	{	99,0,0,FALSE,FALSE,0,0,0,"dummy",	""	},
};

cptr do_cmd_class_power_aux_minoriko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	switch(num)
	{
	case 0:
		{
			int rad = 2 + plev / 20 + chr_adj / 13;
			base = plev * 2 + chr_adj * 10;
			if(only_info) return format("損傷:～%d",base/2);
			if(one_in_(4))
			{
				msg_format("局地的豪雨が発生した！");
				project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else if(one_in_(3))
			{
				msg_format("強烈な旋風が発生した！");
				project(0, rad, py, px, base, GF_TORNADO, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else if(one_in_(2))
			{
				msg_format("冷たい風が吹き荒れた！");
				project(0, rad, py, px, base, GF_COLD, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
			else
			{
				msg_format("雷が落ちた！");
				project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			}
		}
		break;
	case 1:
		{
			int damage=0;
			//鎌を持っている手しかダメージに算入しない。
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING))
				damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING))
				damage += calc_weapon_dam(1);
			if(only_info) return  format("損傷:%d",damage);
			if(damage==0)
			{
				msg_format("ERROR:damage0");
				return NULL;
			}
			msg_format("あなたは鎌を大きく振った！");
			project(0, 1, py, px, damage * 2, GF_MISSILE, PROJECT_KILL, -1);

			break;
		}
	case 2:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "";
			msg_print("甘い匂いと共に焼き芋が出現した！");
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			drop_near(q_ptr, -1, py, px);
			break;
		}

	case 3:
		{
			int rad = 2 + chr_adj / 15;
			int damage = 10 + chr_adj * 3;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("祝祭だ！荒々しく踊り狂った！");
			cast_meteor(damage, rad, GF_FORCE);

			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;

	}
	return "";
}


/*:::早苗専用技*/
class_power_type class_power_sanae[] =
{
	{ 1,0,0,FALSE,FALSE,A_INT,0,0,"詠唱(秘術)",
		"一子相伝の秘術を行使するための詠唱を行う。"},
	{ 1,1,0,FALSE,FALSE,A_WIS,0,0,"詠唱(神奈子)",
		"神奈子様の力を借りるための詠唱を行う。"},
	{ 1,2,0,FALSE,FALSE,A_WIS,0,0,"詠唱(諏訪子)",
		"諏訪子様の力を借りるための詠唱を行う。"},

	{ 2,2,20,FALSE,TRUE,A_INT,0,0,"ウィンドブラスト",
		"風属性のボルトで敵を攻撃する。ACの高い敵には当たりにくい。詠唱は不要だが詠唱(秘術)が長いと威力がやや上がる。"},
	{ 5,4,20,FALSE,TRUE,A_WIS,0,1,"スカイサーペント",
		"連続で無属性のボルトを放つ。詠唱(神奈子)が3段階以上必要。詠唱が長いほど数が増える。"},
	{ 7,5,20,FALSE,TRUE,A_WIS,0,1,"コバルトスプレッド",
		"障害物に当たると爆発する汚染属性の蛙の弾を放つ。詠唱(諏訪子)が3段階以上必要。詠唱が長いほど威力と爆発半径が上がる。"},
	{ 10,3,45,FALSE,FALSE,A_INT,0,0,"霊的知覚",
		"周囲のモンスターを感知する。レベルが上がると範囲が広がる。この技によって詠唱が途切れることはない。(失敗すると途切れる)"},
	{ 12,8,50,FALSE,TRUE,A_INT,0,3,"グレイソーマタージ",
		"自分の周りのランダムな位置にランダムな元素属性のボールを連続で発生させる。詠唱(秘術)が5段階以上必要。詠唱が長いほど威力と爆発半径が上がる。"},
	{ 16,10,40,FALSE,TRUE,A_WIS,0,0,"ファフロッキーズの奇跡",
		"視界内の敵の頭上に様々なものを降らせる。詠唱(諏訪子)が3段階以上必要。"},
	{ 18,32,55,FALSE,TRUE,A_CHR,0,5,"乱れおみくじ連続引き",
		"自分の近くのランダムなターゲットに向けて閃光(吉50%)、暗黒(凶40%)、破邪(大吉3倍9%)、地獄の劫火(大凶5倍1%)のいずれかのボールを数発放つ。大凶は自分が受けることもあり、一度に二つ以上出ることはない。"},
	{ 20,23,70,FALSE,TRUE,A_INT,0,3,"白昼の客星",
		"ターゲットの場所に閃光属性のボールを発生させる。詠唱(秘術)が5段階以上必要。詠唱が長いほど威力と爆発半径が上がる。"},
	{ 23,18,50,FALSE,TRUE,A_WIS,0,0,"弘安の神風",
		"竜巻属性の強力なビームを放つ。巨大な敵や空を飛ぶ敵には効果が薄い。詠唱(神奈子)が4段階以上必要。詠唱が長いほど威力が上がる。"},
	{ 25,5,80,FALSE,TRUE,A_INT,0,0,"ミラクルフルーツ",
		"食べるとMPが回復する「ミラクルフルーツ」を生成する。詠唱(秘術)が25段階以上必要。"},

	{ 27,12,45,FALSE,TRUE,A_WIS,0,0,"忘却の祭儀",
		"障壁を展開し、一時的に元素への耐性を得る。詠唱(諏訪子)が5段階以上必要。詠唱が長いほど効果時間が延び、10段階でAC、20段階で魔法防御も上昇する。"},
	{ 29,36,70,FALSE,TRUE,A_INT,0,5,"海が割れる日",
		"水棲の敵に大ダメージを与え、その他の敵には水属性ダメージを与えるビームを放つ。詠唱(秘術)が7段階以上必要。詠唱が長いと威力が上がる。"},

	{ 32,30,60,FALSE,TRUE,A_DEX,0,0,"結界術",
		"自分のいる場所に「守りのルーン」を敷設する。" },

	{ 34,18,60,FALSE,TRUE,A_INT,0,0,"九字刺し",
		"攻撃的な結界を展開し、一定時間直接攻撃をしてきた敵に破片属性ダメージを与える。詠唱(秘術)が9段階以上必要。詠唱が長いほど効果時間が伸びる。"},
	{ 36,24,50,FALSE,TRUE,A_WIS,0,0,"神代大蛇",
		"空間歪曲属性のビームを放つ。詠唱(神奈子)が6段階以上必要。詠唱が長いほど威力がやや上がる。"},

	{ 39,36,55,FALSE,TRUE,A_WIS,0,0,"手管の蝦蟇",
		"カオス属性の強力なブレスを放つ。詠唱(諏訪子)が12段階以上必要。詠唱が長いと威力が上がる。"},
	{ 42,60,80,FALSE,TRUE,A_INT,0,0,"妖力スポイラー",
		"周囲のモンスターが魔力属性の爆発を起こす。強力な敵ほど大爆発が起こり、妖怪の場合特に効果が高い。詠唱(秘術)が1段階以上必要。詠唱が長いと威力が上がり有効範囲が広がる。"},
	{ 48,50,90,FALSE,TRUE,A_INT,0,0,"神風を喚ぶ星の儀式",
		"このフロアで奇跡「神の風」が使えるようになる。詠唱(秘術)が48段階以上必要。"},
	{ 48,64,75,FALSE,TRUE,A_WIS,0,0,"神の風",
		"極めて強力な魔力属性の大型レーザーを放つ。詠唱(神奈子)が16段階以上必要。「神風を喚ぶ星の儀式」が完了していないと使えない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


static byte sanae_minimum_cons[22] ={
	0,0,0,
	0,3,3,
	0,5,3,0,
	5,4,25,5,
	7,0,9,6,12,
	1,48,16
};

	//p_ptr->magic_num1[0]に詠唱中モード、[1]に神の風使用可能フラグを格納
cptr do_cmd_class_power_aux_sanae(int num, bool only_info)
{
	int dir,i;
	int dice = 0, sides = 0, base = 0, rad = 0;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons_soma = (p_ptr->magic_num1[0] == CONCENT_SOMA) ? p_ptr->concent : 0;
	int cons_kana = (p_ptr->magic_num1[0] == CONCENT_KANA) ? p_ptr->concent : 0;
	int cons_suwa = (p_ptr->magic_num1[0] == CONCENT_SUWA) ? p_ptr->concent : 0;
	int min_cons = sanae_minimum_cons[num];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("詠唱:%d段階",cons_soma);
			do_cmd_concentrate(CONCENT_SOMA);
		}
		break;
	case 1:
		{
			if(only_info) return format("詠唱:%d段階",cons_kana);
			do_cmd_concentrate(CONCENT_KANA);
		}
		break;
	case 2:
		{
			if(only_info) return format("詠唱:%d段階",cons_suwa);
			do_cmd_concentrate(CONCENT_SUWA);
		}
		break;
	case 3://ウィンドブラスト
		{
			dice = 2 + plev / 10 + cons_soma / 5;
			sides = 5 + chr_adj / 10;
			base = 5 + cons_soma * 3;
			if(only_info) return format("損傷:%dd%d+%d",dice,sides,base);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("鋭く渦巻く風を放った。");
			fire_bolt(GF_WINDCUTTER, dir, damroll(dice, sides) + base);
		}
		break;

	case 4://スカイサーペント
		{
			num = MIN(16,(1 + cons_kana / 2));
			if(cons_kana >= min_cons)
			{
				dice = 3 + plev / 15 + cons_kana / 15;
				sides = 5 + chr_adj / 10;
			}
			if(only_info) return format("損傷:%dd%d * %d",dice,sides,num);
			if(cons_kana < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("数条の蛇が宙を舞った！");
			for (i = 0; i < num; i++) fire_bolt(GF_MISSILE, dir, damroll(dice, sides) + base);

		}
		break;

	case 5://コバルトスプレッド
		{
			if(cons_suwa >= min_cons)
			{
				rad = MIN(5, 1 + plev / 24 + cons_suwa / 7);
				dice = 5 + plev / 5;
				sides = 4 + chr_adj / 5 + cons_suwa / 2;
				base = 10 + plev + cons_suwa * 5;
			}
			if(only_info) return format("損傷:%dd%d + %d",dice,sides,base);
			if(cons_suwa < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("妖しく光る蛙が飛びかかった！");
			fire_rocket(GF_POLLUTE, dir, damroll(dice, sides)+base,rad);

		}
		break;
	case 6: //霊的知覚
		{
			rad = 5 + plev / 2;
			if(only_info) return format("範囲:%d",rad);
			msg_format("精神を研ぎ澄ませた・・");
			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			reset_concent = FALSE; //詠唱は途切れない
		}
		break;
	case 7://グレイソーマタージ
		{
			rad = 2  + cons_soma / 15;
			if(cons_soma >= min_cons)
			{
				base = plev / 2 + chr_adj + cons_soma * 2 - 5;
			}
			if(only_info) return format("損傷:%d*(10+1d10)",base);
			if(cons_soma <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			msg_format("色とりどりの星が舞い散った！");
			cast_meteor(base, rad, 0);

		}
		break;

	case 8://ファフロッキーズの奇跡 サモンマテリアルのルーチンを分離独立共有する。
		{
			if(only_info) return format("損傷:不定");
			if(cons_suwa <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			msg_format("様々な雑多な物が降ってきた！");
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr;
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
				summon_material(i);
			}

		}
		break;

	case 9://乱れおみくじ連続引き
		{
			int basedam = plev * 2;
			int typ,rad;
			int num = 4 + plev / 40;
			int range = 5 + plev / 10;
			bool nasty_flag = FALSE;
			if(only_info) return format("基本威力:%d*%d",basedam,num);
			msg_format("懐からおみくじを一掴み取出して投げ放った！");
			while(num > 0 && p_ptr->chp >= 0)
			{
				int roll = randint0(100);

				if(!roll && !nasty_flag)
				{
					nasty_flag = TRUE; //大凶は一回に二度は出ない
					typ = GF_HELL_FIRE;
					rad = 7;
					if(one_in_(2))
					{
						msg_format("***大凶***");
						project(PROJECT_WHO_OMIKUJI_HELL, rad, py, px, basedam*5,typ, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
					}

					else
					{
						if(fire_random_target(typ, basedam*5, 5, rad, range))
							msg_format("***大凶***");
					}
				}
				else if(roll < 10)
				{
					typ = GF_HOLY_FIRE;
					rad = 4;
					if(fire_random_target(typ, basedam*3, 3, rad, range))
						msg_format("*大吉*");
				}
				else if(roll < 50)
				{
					typ = GF_DARK;
					rad = 3;
					if(fire_random_target(typ, basedam, 3, rad, range))
						msg_format("-凶-");
				}
				else
				{
					typ = GF_LITE;
					rad = 3;
					if(fire_random_target(typ, basedam, 3, rad, range))
						msg_format("-吉-");
				}
				num--;
			}



		}
		break;
	case 10://白昼の客星
		{
			if(use_itemcard) cons_soma = 5 + plev / 10;

			rad = 1 + plev / 20 + cons_soma / 10;
			if(cons_soma >= min_cons)
			{
				base = (plev * 3 + chr_adj * 2) * (cons_soma + 10) / 10;

			}
			if(only_info) return format("損傷:%d",base);
			if(cons_soma <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE, dir, base, rad,"突如、眩い光球が出現した！")) return NULL;
		}
		break;
	case 11://弘安の神風
		{
			if(cons_kana >= min_cons)
			{
				dice = 2 + plev / 10 + chr_adj / 9;
				sides = (cons_kana + 15) * 2;
			}
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("突如、暴風が巻き起こった！");
			fire_spark(GF_TORNADO,dir,damroll(dice,sides),1);	//v1.1.83 base→dice ずっと気づかなかった
		}
		break;
	case 12://ミラクルフルーツ
		{
			object_type forge, *q_ptr = &forge;
			if(use_itemcard) cons_soma = 30;

			if(only_info) return format("");
			if(cons_soma <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if(use_itemcard)
				msg_print("箱の中に何か入っている。");
			else
				msg_print("奇跡の力でフルーツが降ってきた！");

			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);

		}
		break;
	case 13: //忘却の祭儀
		{
			int time;
			if(cons_suwa >= min_cons)
			{
				base = 15 + cons_suwa;
			}
			if(only_info) return format("期間:1d%d + %d",base,base);
			if(cons_suwa < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			time = base + randint1(base);
			msg_format("防壁が現れた！");
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
			if(cons_suwa > 9) set_shield(time, FALSE);
			if(cons_suwa > 19) set_resist_magic(time, FALSE);
		}
		break;
	case 14://海が割れる日
		{
			if(use_itemcard) cons_soma = 7 + plev / 10;

			if(cons_soma >= min_cons)
			{
				dice = 1;
				sides = cons_soma * (15 + cons_soma / 2);
				base = plev * 3 + chr_adj * 5;
				if(base < 100) base = 100;
			}
			if(only_info) return format("損傷:%dd%d+%d",dice,sides,base);
			if(cons_soma <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("海をも割る一撃が放たれた！");
			fire_beam(GF_MOSES, dir, damroll(dice, sides) + base);
		}
		break;

		case 15: //結界術
		{
			if (only_info) return "";
			msg_print("あなたは縄を綯い周囲に張り巡らせた。");
			warding_glyph();
			break;
		}


	case 16: //九字刺し
		{
			int time;
			if(cons_soma >= min_cons)
			{
				base = 10 + cons_soma;
			}
			if(only_info) return format("期間:1d%d + %d",base,base);
			if(cons_soma < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			time = base + randint1(base);
			msg_format("空中に力強く九字を切った。");
			set_dustrobe(time,FALSE);
		}
		break;
	case 17://神代大蛇
		{
			if(use_itemcard) cons_kana = 15 + plev / 5;
			if(cons_kana >= min_cons)
			{
				dice = 3 + plev / 4 + chr_adj / 6;
				sides = (cons_kana + 5) * 2;
			}
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("何か大いなるものが去来するのを感じた！");
			fire_spark(GF_DISTORTION, dir, damroll(dice, sides),1);

		}
		break;

	case 18://手管の蝦蟇
		{
			if(use_itemcard) cons_suwa = 15 + plev / 10;
			if(cons_suwa >= min_cons)
			{
				base = (12 + cons_suwa) * (30 + chr_adj) * plev / 100;
			}
			if(only_info) return format("損傷:%d",base);
			if(cons_suwa < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("背後に巨大な何かが現れた・・");
			fire_ball(GF_CHAOS, dir, base,-3);

			//v1.1.59 この技を使うとカエルが降ってくるらしい
			for (i = 10; i>0; i--)
			{
				int r_idx, tmp_x, tmp_y;

				r_idx = one_in_(3) ? MON_G_FROG : one_in_(2) ? MON_GAMAGAERU : MON_G_PINK_FROG;

				scatter(&tmp_y, &tmp_x, py, px, 20, 0);
				//地形判定は↓のルーチン内で行われる
				summon_named_creature(0, tmp_y, tmp_x, r_idx, 0L);


			}

		}
		break;
	case 19://妖力スポイラー
		{
			int range = 0;
			int flag_msg = FALSE;
			if(cons_soma >= min_cons)
			{
				range = MIN(MAX_SIGHT,cons_soma * 2);

			}
			if(only_info) return format("範囲:%d",range);
			if(cons_soma <  min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				int dam,rad;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr;
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
				if(m_ptr->cdis > range) continue;
				if(i == p_ptr->riding) continue;

				rad = 1 + r_ptr->level / 20;
				dam = plev + r_ptr->level * 2;
				if(cons_soma > 5) dam = dam * (cons_soma+15) / 20;
				if(r_ptr->flags3 & RF3_KWAI) dam *= 2;
				if(r_ptr->flags2 & RF2_POWERFUL) dam = dam * 3 / 2;

				if(!flag_msg)
				{
					msg_format("周囲の敵の妖力を吸い出し、攻撃に転用した！");
					flag_msg = TRUE;
				}
				project(0,rad,y,x,dam,GF_MANA,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP),-1);

			}
			if(!flag_msg) msg_format("周囲に丁度いい相手がいないようだ。");
		}
		break;
	case 20: //神風を喚ぶ星の儀式
		{
			if(only_info) return format("");
			if(cons_soma < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			else if(p_ptr->magic_num1[1])
			{
				msg_format("既に準備は完了している。");
				return NULL;
			}
			msg_format("長い準備が完了した。神の力がこの領域に満ちているのを感じる！");
			p_ptr->magic_num1[1] = 1;
		}
		break;

	case 21://神の風
		{
			if(cons_kana >= min_cons)
			{
				dice = plev + chr_adj * 2 / 3;
				sides = cons_kana + 50;
			}
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if(cons_kana < min_cons)
			{
				msg_format("力が足りない。詠唱をし直さないと。");
				return NULL;
			}
			if (!get_aim_dir(&dir)) return NULL;

			msg_format("神の風が全てを吹き飛ばした！");
			fire_spark(GF_MANA,dir,damroll(dice, sides),2);

		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}

	if(reset_concent) reset_concentration(FALSE);

	return "";
}

/*:::静葉専用技*/
class_power_type class_power_shizuha[] =
{
	{7,5,30,FALSE,TRUE,A_DEX,0,0,"ロストウィンドロウ",
		"トラップを解除するビームを放つ。"},
	{20,12,50,FALSE,TRUE,A_STR,0,0,"フォーリンブラスト",
		"突風のビームを放ち敵を吹き飛ばす。巨大な敵や空を飛ぶ敵には効果が薄い。"},
	{40,55,90,FALSE,TRUE,A_CHR,0,0,"狂いの落葉",
		"短時間存在感が希薄になり、敵に見失われやすくなる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_shizuha(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int plev = p_ptr->lev;
	switch(num)
	{

	case 0:
		{
			if (only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("一陣の風が吹き抜けた。");
			disarm_trap(dir);
			break;
		}
	case 1:
		{
			damage = plev + chr_adj;
			if(damage < 30) damage = 30;
			if (only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("激しい突風が吹き荒れた！");
			fire_spark(GF_TORNADO, dir,damage,1);
			break;
		}
	case 2:
		{
			dice = base = 10;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			msg_format("あなたの色彩が失われていった・・");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::影狼用特技*/
class_power_type class_power_kagerou[] =
{
	{5,10,0,FALSE,TRUE,A_CON,0,0,"狼変身",
		"巨大な狼に変身する。満月の晩にしか使えない。変身中は格闘能力が飛躍的に上昇するがほとんどの装備が無効化され魔法や巻物などが使用不能になる。Uコマンドから変身を解除できる。"},
	{15,25,40,FALSE,TRUE,A_STR,0,0,"フルムーンロア",
		"遠吠えで視界内の敵を恐怖させ、自分自身を士気高揚させる。周囲の敵が起きる。満月時にはさらに視界内の敵に轟音ダメージを与える。"},

	{24,18,60,TRUE,FALSE,A_DEX,50,5,"ハイスピードバウンス",
		"離れた敵に一瞬で飛びかかり攻撃する。レベルが上がると射程距離が延びる。障害物があると止まる。装備品が重いと失敗しやすい。"},

	{30,30,75,FALSE,FALSE,A_DEX,50,10,"スターファング",
		"近くのランダムな敵に数回の突進攻撃を仕掛ける。レベルが上がると射程と回数が増える。装備品が重いと失敗しやすい。"},

	{42,100,85,TRUE,FALSE,A_STR,0,0,"血に餓えたウルフファング",
		"狂戦士化し指定方向に三回連続攻撃する。行動力を1.5倍消費する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


/*:::影狼特技*/
cptr do_cmd_class_power_aux_kagerou(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//狼変身
	case 0:
		{
			int time = 0;
			if(only_info) return format("期間:夜明けまで");
			time = ((turn / (TURNS_PER_TICK * TOWN_DAWN) + 1) * (TURNS_PER_TICK * TOWN_DAWN) - turn) / TURNS_PER_TICK;
			if(time < 5)
			{
				msg_print("変身できない。夜明けが近いようだ。");
				return NULL;
			}

			if(!set_mimic(time, MIMIC_BEAST, FALSE)) return NULL;

		}
		break;

	case 1: //フルムーンロア
		{
			if(FULL_MOON)
			{
				int power = plev * 3 + 50;
				if(p_ptr->mimic_form == MIMIC_BEAST) power += plev * 2;
				if(only_info) return format("損傷:%d",power);
				msg_print("あなたは月に向けて大咆哮を放った！");
				project_hack2(GF_SOUND,0,0,power);
				turn_monsters(power);
				set_afraid(0);
				set_hero(30 + randint1(30),FALSE);
				aggravate_monsters(0,FALSE);

			}
			else
			{
				int power = plev * 3;
				if(p_ptr->mimic_form == MIMIC_BEAST) power += plev;
				if(only_info) return format("効力:%d",power);
				msg_print("あなたは恐ろしげに遠吠えした。");
				turn_monsters(power);
				set_afraid(0);
				set_hero(20 + randint1(10),FALSE);
				aggravate_monsters(0,FALSE);
			}
			break;
		}
	case 2: //ハイスピードバウンス
		{
			int len = p_ptr->lev / 5 + 5;
			if(len < 5) len = 5;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}

	case 3://スターファング rush_attack2()を改変
		{
			int num = plev / 9;
			int cnt;
			int range = plev / 10;
			int dam = plev  + chr_adj * 5 / 3;
			if(p_ptr->mimic_form == MIMIC_BEAST)
			{
				range += 2;
				dam += plev;
			}
			if(only_info) return  format("射程:%d 損傷:%d*%d",range,dam,num);

			msg_print("あなたは地を蹴って跳んだ！");
			for(cnt=0;cnt < num;cnt++)
			{
				int i, path_n;
				int tx,ty;
				int target_m_idx = 0;
				int target_num = 0;
				u16b path_g[32];
				bool moved = FALSE;
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					m_ptr = &m_list[i];
					if(!m_ptr->r_idx) continue;
					if(m_ptr->cdis > range) continue;
					if(!m_ptr->ml || !projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					if (!is_hostile(m_ptr)) continue;
					target_num++;
					if(!one_in_(target_num)) continue;
 					target_m_idx = i;
				}
				if(!target_m_idx) break;
				tx = m_list[target_m_idx].fx;
				ty = m_list[target_m_idx].fy;
				project_length = 10;
				path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
				project_length = 0;
				if (!path_n) break;
				ty = py;
				tx = px;

				/* Project along the path */
				for (i = 0; i < path_n; i++)
				{
					monster_type *m_ptr;
					int ny = GRID_Y(path_g[i]);
					int nx = GRID_X(path_g[i]);

					if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
					{
						ty = ny;
						tx = nx;
						continue;
					}
					if (!cave[ny][nx].m_idx)
					{
						msg_print("壁に激突した。");
						break;
					}

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					update_mon(cave[ny][nx].m_idx, TRUE);
					msg_format("「ワオーン！」");

					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
					moved = TRUE;
					project(0, 0, ny, nx, dam, GF_ARROW, PROJECT_KILL | PROJECT_JUMP, -1);

					break;
				}
				if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

			}
			if(!cnt) msg_print("・・しかし周囲に敵がいなかった。");

			break;
		}
	case 4://血に餓えたウルフファング
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if(only_info) return format("");
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int num = 3;
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				set_afraid(0);
				set_shero(randint1(20) + 20, FALSE);
				msg_format("あなたは%sに飛びかかった！",m_name);
				for(num=3; (num>1 && cave[y][x].m_idx);num--) py_attack(y, x, 0);
				new_class_power_change_energy_need = 150;
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::霊夢用特技*/
class_power_type class_power_reimu[] =
{
	{1,2,15,FALSE,FALSE,A_DEX,0,3,"パスウェイジョンニードル",
		"無属性のボルトを放つ。レベルが上がると本数が増える。"},
	{4,4,25,FALSE,TRUE,A_DEX,0,0,"封印解除",
		"隣接した罠や箱へ開錠と罠解除を行い、閉ざされたドアを破壊する。"},
	{8,16,35,FALSE,TRUE,A_WIS,0,0,"解呪", //rank2
		"装備中の呪われた装備を解呪する。レベル30で強力な呪いも解呪するようになる。"},
	{10,5,30,FALSE,FALSE,A_WIS,0,4,"気配探知", //rank4
		"周囲のモンスターを感知する。"},
	{14,10,40,FALSE,TRUE,A_WIS,0,4,"陰陽玉", //rank2
		"破邪属性の球を放つ。レベルが上がると爆発半径が広がる。"},
	{18,24,60,FALSE,TRUE,A_INT,0,0,"亜空穴", //rank5
		"視界内の指定した位置にテレポートする。この行動には通常の1/2の行動力しか使われない。指定した場所に出るのに失敗するとランダムな場所に飛ぶ。"},
	{24,32,65,FALSE,TRUE,A_CHR,0,12,"夢想封印",//rank4
		"周囲のランダムな敵に無属性のボールを連続で放つ。レベルが上がると威力、数、爆発半径が上昇する。壁の隣にいると使えない。"},
	{27,24,55,FALSE,TRUE,A_WIS,0,0,"治癒", //rank7
		"レベル*5の体力を回復させ、切り傷を治療する。"},
	{30,20,50,FALSE,TRUE,A_INT,0,0,"二重結界", //rank3
		"一時的に元素攻撃への耐性を得る。レベル40になるとACも上昇する。"},
	{33,1,65,FALSE,FALSE,A_WIS,0,0,"瞑想", //rank6
		"精神を研ぎ澄ませ、MPをわずかに回復する。"},
	{35,45,75,FALSE,TRUE,A_WIS,0,0,"八方鬼縛陣", //rank4
		"隣接したターゲットとその周辺に破邪属性ダメージを与え、さらにターゲットの移動とテレポートを短時間阻害する。二体以上を同時に縛ることはできない。脱出されることもある。"},
	{38,72,85,FALSE,TRUE,A_CHR,0,0,"よくばり大幣", //rank7
		"大幣装備時にのみ使用可能。大幣が勝手に飛んで行って敵を薙ぎ倒す。"},
	{40,64,85,FALSE,TRUE,A_STR,0,0,"陰陽飛鳥井", //rank5
		"高威力の破邪属性ロケットを放つ。"},
	{47,250,90,FALSE,TRUE,A_DEX,0,0,"博麗幻影", //rank8
		"自分の分身を出現させる。分身は捕獲不可能で階移動したり倒されると消滅し、強さは現在いる階のレベルで変わる。"},
	{50,100,80,FALSE,TRUE,A_CHR,0,0,"夢想天生", //rank9
		"あらゆる束縛から開放され無敵化する。この無敵化は魔力消去されないが、無敵化している間は毎ターンMP50を消費する。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_reimu(int num, bool only_info)
{
	int dir, i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int rank;

	if(use_itemcard) rank = 5;
	else rank = osaisen_rank();

	switch(num)
	{

	case 0:
		{
			int num = 3 + plev / 7;
			int dice = 3 + plev / 20;
			int sides = 3 + chr_adj / 10;

			if(rank > 6) num += rank - 4;
			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("針を投擲した！");
			for (i = 0; i < num; i++) fire_bolt(GF_MISSILE, dir, damroll(dice, sides));
			break;
		}
	case 1:
		{

			if(only_info) return format("");
			msg_format("精神を集中し、大きく手を振った。");
			(void) destroy_doors_touch();

			break;
		}
	case 2:
		{
			bool success = FALSE;
			if(only_info) return format("");
			msg_format("その場に座り、手を合わせて祝詞を唱えた。");
			if(plev < 30)
			{
				if(remove_curse()) success = TRUE;
			}
			else
			{
				if(remove_all_curse())  success = TRUE;
			}
			if(success) msg_print("装備品の呪縛が消えた。");
			else msg_print("何も起こらなかった。");

			break;
		}
	case 3:
		{
			if(only_info) return format("");

			msg_print("周囲の気配を探った・・");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 4:
		{
			int dam = (plev * 3 + chr_adj * 5) / 2;
			int rad =  plev / 20 + rank / 3;
			if(dam < 25) dam = 25;

			if(only_info) return format("損傷:%d 半径:%d",dam,rad);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("陰陽玉を取り出し、投げつけた！");
			fire_ball(GF_HOLY_FIRE,dir,dam,rad);

			break;
		}

	case 5:
		{
			int x, y;
			if(only_info) return format("");
			if (!tgt_pt(&x, &y)) return NULL;

			if (p_ptr->anti_tele)
			{
				msg_print("不思議な力がテレポートを防いだ！");
			}
			else if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT ) ||
			    !projectable(py, px, y, x))
			{
				teleport_player(30, 0L);
				msg_print("ちょっと移動に失敗した。");
			}
			else
			{
				msg_print("あなたは離れた場所に一瞬で現れた。");
				teleport_player_to(y, x, 0L);
				new_class_power_change_energy_need = 50;
			}
			break;
		}
	case 6:
		{
			int rad = 1 + plev / 40 + rank / 4;
			bool flag = FALSE;
			int num = 3 + plev / 15 + rank / 2;
			int dam = plev + chr_adj;

			if(dam < 30) dam = 30;

			if(only_info) return format("損傷:%d * %d",dam,num);

			if (!in_bounds2(py + 1,px) || !cave_have_flag_grid( &cave[py + 1][px],FF_PROJECT)
				||  !in_bounds2(py - 1,px) || !cave_have_flag_grid( &cave[py - 1][px],FF_PROJECT)
				||  !in_bounds2(py ,px + 1) || !cave_have_flag_grid( &cave[py ][px + 1],FF_PROJECT)
				||  !in_bounds2(py ,px - 1) || !cave_have_flag_grid( &cave[py ][px - 1],FF_PROJECT))
				{
					msg_print("壁が邪魔だ！");
					return NULL;
				}

			msg_format("あなたの周りに幾つもの光球が現れた・・");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_DISP_ALL, dam, 3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;

		}

	case 7:
		{
			int heal = plev * 5;
			if(only_info) return format("回復:%d",heal);
			msg_format("治癒の札を取り出して貼った。");
			hp_player(heal);
			set_cut(0);

			break;
		}
	case 8:
		{
			int base = 15 + plev / 5 + rank * 2;
			int v;
			if(only_info) return format("期間:%d+1d%dターン",base,base);
			msg_format("防壁を張り巡らせた！");
			v = base + randint1(base);
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			set_oppose_pois(v, FALSE);
			if(plev > 39) set_shield(v,FALSE);

			break;
		}
	case 9:
		{
			if(only_info) return format("");

			if (p_ptr->csp < p_ptr->msp) msg_print("少し頭がハッキリした。");

			p_ptr->csp += (3 + randint1(1 + plev/10));
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}

			p_ptr->redraw |= (PR_MANA);
		}
		break;
	/*::: -Hack- 八方鬼縛陣　magic_num1[0]とtim_general[0]を使用する*/
	case 10:
		{
			int y, x;
			int dam = plev * 2 + chr_adj * 10 / 3 + (rank+1) * plev / 5;
			monster_type *m_ptr;

			int base = 5 + p_ptr->lev / 10 + rank * 2;
			if(only_info) return format("損傷:%d 期間:%d",dam, base);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("地面の陣から光が噴き上がった！",m_name);

				project(0,1,m_ptr->fy,m_ptr->fx,dam,GF_HOLY_FIRE,PROJECT_KILL,-1);
				if(!m_ptr->r_idx) break;

				msg_format("%sは光の柱に囚われた！",m_name);
				p_ptr->magic_num1[0] = cave[y][x].m_idx;
				set_tim_general(base,FALSE,0,0);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

	/*::: -Hack - グローバル変数oonusa_dam=0にダメージ値を設定し、project()中でこれが0でなければそれをダメージ値として再射撃する*/
	case 11: //よくばり大幣
		//v1.1.44 ルーチン見直し
		{

			int tx,ty;
			int dam = 0;
			int restart_chance = osaisen_rank();

			if (restart_chance < 7) restart_chance = 7; //テスト用の再発動率最低値

			if(inventory[INVEN_RARM].tval == TV_STICK && inventory[INVEN_RARM].sval == SV_WEAPON_OONUSA )
				dam += calc_weapon_dam(0) / p_ptr->num_blow[0];
			if(inventory[INVEN_LARM].tval == TV_STICK && inventory[INVEN_LARM].sval == SV_WEAPON_OONUSA )
				dam += calc_weapon_dam(1) / p_ptr->num_blow[1];
			if(only_info) return format("損傷:%d * 不定",dam);

			if (dam <= 0)
			{
				msg_print("こんなものを投げても無駄だ。");
				return NULL;

			}

			msg_format("大幣が手から離れて飛んで行った！");
			execute_restartable_project(0, 0, restart_chance, 0, 0, dam, GF_ARROW, 0);
			msg_format("大幣は飛んで帰ってきて再び手の中に納まった。");

		}
		break;
	case 12:
		{
			int rad = 2 + rank / 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = p_ptr->lev * rank / 10;
			int base = p_ptr->lev * (1 + rank / 3);

			if(dice < 10) dice = 10;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な陰陽玉が唸りを上げて飛んでいった！");
			fire_rocket(GF_HOLY_FIRE, dir, base + damroll(dice,sides), rad);
			break;
		}
	case 13: //博麗幻影
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_REIMU) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("すでに術を実行している。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_REIMU, PM_EPHEMERA))
			{
				if(use_itemcard)
					msg_print("博麗の巫女の幻影が出現した！");
				else
					msg_print("自分の幻影が出現した！");

			}
			else
				msg_print("術に失敗した。");


			break;
		}
	case 14:
		{
			if(only_info) return "";

			msg_print("あなたは全てのものから解き放たれた！");
			p_ptr->special_defense |= MUSOU_TENSEI;
			p_ptr->redraw |= (PR_MAP | PR_STATUS);
			p_ptr->update |= (PU_MONSTERS | PU_BONUS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			break;
		}




	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::勇儀専用技*/
class_power_type class_power_yugi[] =
{

	{ 5,10,25,TRUE,FALSE,A_STR,0,0,"地震",
		"ダンジョンの地形を崩す。"},
	{ 12,20,40,TRUE,FALSE,A_CON,0,5,"壊滅の咆哮",
		"大声で周囲に轟音属性攻撃を行う。周辺の敵が起きる。"},
	{ 16,20,45,FALSE,TRUE,A_DEX,0,0,"地獄の苦輪",
		"短時間の間、自分と周囲の敵のテレポートを阻害する。"},
	{ 21,27,85,FALSE,TRUE,A_STR,0,3,"大江山嵐",
		"視界内の敵に風属性ダメージを与える。威力は腕力に依存する。"},
	{ 25,50,80,TRUE,FALSE,A_STR,0,0,"鬼気狂瀾",
		"一定時間狂戦士化し加速する。"},
	{ 32,32,80,FALSE,FALSE,A_STR,0,0,"怪力乱神",
		"周囲を*破壊*する。レベルが上がると範囲が広がる。"},
	{ 39,48,90,FALSE,TRUE,A_STR,0,0,"金剛螺旋",
		"視界内の敵に核熱属性ダメージを与える。威力は腕力に依存する。"},
	{ 45,33,80,FALSE,FALSE,A_STR,0,0,"三歩必殺",
		"周囲の敵にダメージを与える。実行するたびに属性と威力と範囲が変化する。素手時のみ使用可能。"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_yugi(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int str_adj = adj_general[p_ptr->stat_ind[A_STR]];
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev / 10;
			if(only_info) return format("範囲:%d",rad);
			if(use_itemcard)
				msg_print("カードが地面を激しく揺らした！");
			else
				msg_print("地面を踏み鳴らした。");
			earthquake(py, px, rad);

			break;
		}
	case 1:
		{
			int dam = plev * 3 + str_adj * 5;
			int rad = 3 + plev / 16 + str_adj / 20;
			if(only_info) return format("損傷:～%d",dam/2);
			msg_format("大きく息を吸い込み、大音声を放った！");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL|PROJECT_GRID , -1);
			aggravate_monsters(0,FALSE);
			break;

		}
	case 2:
		{
			int base = 10;
			if(only_info) return format("期間:%d+1d%d",base,base);
			set_nennbaku(randint1(base) + base, FALSE);
			break;
		}
	case 3:
		{
			int sides = plev * 3 + str_adj * 5;

			if(only_info) return format("損傷:1d%d",sides);
			msg_format("冷たい風が吹き荒れた！");
			project_hack2(GF_TORNADO,1,sides,0);
			break;
		}
	case 4:
		{
			int base = 25;
			int v;
			if(only_info) return format("期間:%d+1d%d",base,base);
			v = base + randint1(base);
			set_shero(v, FALSE);
			hp_player(30);
			set_afraid(0);
			set_fast(v, FALSE);
			break;
		}
	case 5:
		{
			int rad = plev / 5 + str_adj / 5;
			if(only_info) return format("範囲:%d",rad);
			if(!destroy_area(py, px, rad, FALSE,FALSE,FALSE)) msg_print("ここでは使えない。");

			break;
		}

	case 6:
		{
			int base = plev * 3 + str_adj * 5;

			if(only_info) return format("損傷:%d+1d%d",base/2,base/2);
			msg_format("掌から閃光と熱風が吹き荒れた！");
			project_hack2(GF_NUKE,1,base/2,base/2);
			break;
		}
	case 7:
		{
			int dam = 0;

			if(!p_ptr->concent) dam = plev * 3 + str_adj * 3;
			else if(p_ptr->concent == 1) dam = plev * 5 + str_adj * 5;
			else dam = plev * 10 + str_adj * 10 + chr_adj * 10;

			if(only_info)
			{
				if(p_ptr->concent < 2)
					return format("損傷:～%d",dam/2);
				else
					return format("損傷:%d",dam);
			}
			if(p_ptr->concent == 0)
			{
				do_cmd_concentrate(0);
				project(0, 3, py, px, dam, GF_DISP_ALL, PROJECT_KILL|PROJECT_GRID , -1);

			}
			else if(p_ptr->concent == 1)
			{
				do_cmd_concentrate(0);
				project(0, 6, py, px, dam, GF_FORCE, PROJECT_KILL|PROJECT_GRID , -1);
			}
			else
			{
				msg_format("あなたが力強く三歩目を踏み出すと、周囲の全てのものが吹き飛んだ！");
				project_hack2(GF_DISINTEGRATE,0,0,dam);
			}


			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::慧音用特技(通常時)*/
class_power_type class_power_keine1[] =
{
	{8,10,30,FALSE,TRUE,A_INT,0,2,"三種の神器　剣",
		"無属性のボルトを放つ。「草薙之剣」を装備していると高威力の光の剣になる。"},
	{13,16,40,FALSE,TRUE,A_INT,0,3,"三種の神器　玉",
		"閃光属性の球を放つ。「八尺瓊勾玉」を装備していると高威力の破邪属性の球になる。"},
	{20,40,55,FALSE,TRUE,A_INT,0,0,"三種の神器　鏡",
		"一時的に反射能力を得る。「八咫鏡」を装備していると一時的に究極の耐性を得る。"},
	{26,10,30,FALSE,TRUE,A_WIS,0,0,"歴史隠蔽Ⅰ",
		"HPを少し回復し、毒と切り傷を治療する。"},
	{30,20,45,FALSE,TRUE,A_WIS,0,0,"歴史隠蔽Ⅱ",
		"周囲のトラップを消滅させる。"},
	{35,52,70,FALSE,TRUE,A_WIS,0,0,"歴史隠蔽Ⅲ",
		"指定したシンボルの敵をフロアから消し去る。消える敵のレベルに応じてダメージを受ける。抵抗されることもある。"},
	{42,65,80,FALSE,TRUE,A_WIS,0,0,"歴史隠蔽Ⅳ",
		"短時間自らの姿を隠し、敵に認識されにくくなる。"},
	{45,90,85,FALSE,TRUE,A_CHR,0,0,"日出国の天子",
		"視界内の全てに対し高威力の閃光属性攻撃を行う。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::慧音用特技(ハクタク時)*/
class_power_type class_power_keine2[] =
{
	{8,10,30,FALSE,TRUE,A_INT,0,2,"三種の神器　剣",
		"無属性のボルトを放つ。「草薙之剣」を装備していると高威力の光の剣になる。"},
	{13,16,40,FALSE,TRUE,A_INT,0,3,"三種の神器　玉",
		"閃光属性の球を放つ。「八尺瓊勾玉」を装備していると高威力の破邪属性の球になる。"},
	{20,40,55,FALSE,TRUE,A_INT,0,0,"三種の神器　鏡",
		"一時的に反射能力を得る。「八咫鏡」を装備していると一時的に全ての耐性を得る。"},
	{25,40,60,FALSE,TRUE,A_INT,0,0,"オールドヒストリー",
		"今いるフロアの地形とアイテムを全て感知し、視界内の敵の情報を知る。"},
	{30,55,75,FALSE,TRUE,A_INT,0,0,"一条戻り橋",
		"HPを完全に回復し、全ての状態異常、能力低下、経験値減少、一時効果、突然変異を消す。"},
	{35,48,75,FALSE,TRUE,A_INT,0,0,"ネクストヒストリー",
		"フロアを再構成する。再構成の発動までにはごく僅かなタイムラグがある。"},
	{43,180,90,FALSE,TRUE,A_INT,0,0,"無何有浄化",
		"自分の近くにいる幻想郷の存在ではない敵に対し、極めて高威力の時空属性ダメージを与える。"},
	{45,90,85,FALSE,TRUE,A_CHR,0,0,"日出国の天子",
		"視界内の全てに対し高威力の閃光属性攻撃を行う。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


/*:::慧音特技*/
cptr do_cmd_class_power_aux_keine(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	bool hakutaku = FALSE;

	if(use_itemcard)
	{
		if(num != 5) hakutaku = TRUE;

	}
	else if(p_ptr->magic_num1[0]) hakutaku = TRUE;

	switch(num)
	{
	case 0:
		{
			int dice = 5 + plev / 5;
			int sides = 6 + chr_adj / 6;
			bool hyper = FALSE;
			if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_KUSANAGI
				|| buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_KUSANAGI)
				hyper = TRUE;

			if(hyper) dice *= 3;

			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			if(hyper)
			{
				msg_print("神剣が鋭く煌めいた！");
				fire_beam(GF_PSY_SPEAR, dir,damroll(dice,sides));
			}
			else
			{
				msg_print("剣型の弾を放った。");
				fire_bolt(GF_MISSILE,dir,damroll(dice,sides));
			}
			break;
		}
	case 1:
		{
			int dam = 10 + plev + chr_adj * 2;
			int rad = 2;
			bool hyper = FALSE;

			if(inventory[INVEN_NECK].name1 == ART_MAGATAMA)	hyper = TRUE;

			if(hyper)
			{
				dam *= 2;
				rad = 5;
			}
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			if(hyper)
			{
				msg_print("神宝から光が溢れだした！");
				fire_ball(GF_HOLY_FIRE, dir, dam, rad);
			}
			else
			{
				msg_print("光弾を放った。");
				fire_ball(GF_LITE, dir, dam, rad);
			}
			break;

		}
	case 2:
		{
			int base = 10;
			int sides = 10;
			int v;
			if(only_info) return format("期間:%d+1d%d",base,sides);
			v = randint1(sides) + base;

			if(inventory[INVEN_RARM].name1 == ART_KAGAMI || inventory[INVEN_LARM].name1 == ART_KAGAMI )
			{
				msg_format("神鏡が眩く光った！");
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
			else
			{
				msg_format("目の前に光る鏡が現れた。");
				set_tim_reflect(v, FALSE);
			}

			break;
		}
	case 3://歴史隠蔽1、オールドヒストリー
		if(hakutaku)
		{
			if(only_info) return format("");
			msg_print("秘された歴史が頭の中に流れ込んでくる・・");
			wiz_lite(FALSE);
			probing();

		}
		else
		{
			int heal = (plev + 20 + chr_adj) / 2;
			if(only_info) return format("治癒:%d",heal);

			msg_print("傷を受けた歴史を隠した。");
			hp_player(heal);
			set_cut(0);
			set_poisoned(0);

		}
		break;
	case 4:
		if(hakutaku)
		{
			if(only_info) return format("");
			msg_print("自分史の再構築を開始した・・");
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
				if(muta_erasable_count()) msg_print("全ての突然変異が治った。");
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
			msg_print("再構築が完了した。");

		}
		else
		{
			int rad = 2 + plev / 10;
			if(only_info) return format("範囲:%d",rad);
			msg_print("周囲の罠をなかったことにした！");
			//v1.1.70 箱の罠が対象から抜けていたので修正
			project(0, rad, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID|PROJECT_ITEM), -1);

		}
		break;

	case 5:
		if(hakutaku)
		{
				if(only_info) return format("");
				if (!get_check("このフロアの歴史の再構築を試みますか？")) return NULL;

				if (p_ptr->inside_arena || ironman_downward)
				{
					msg_print("何も起こらなかった。");
				}
				else
				{
					/*:::0ではまずいはず*/
					p_ptr->alter_reality = 1;
					p_ptr->redraw |= (PR_STATUS);
				}

		}
		else
		{
			int power = plev * 4;
			if(power < 120) power = 120;
			if(only_info) return format("効力:%d",power);
			symbol_genocide(power, TRUE,0);

		}
		break;


	case 6:
		if(hakutaku)
		{
			int dam = plev * plev;
			if(dam < 1000) dam = 1000;
			if(only_info) return format("損傷:～%d",dam/2);
			msg_print("因果律の修正が発動した！");
			project(0, 4, py, px, dam, GF_N_E_P, PROJECT_KILL, -1);

		}
		else
		{
			int base = 10;
			int sides = 5;
			if(only_info) return format("期間:%d+1d%d",base,sides);

			set_tim_superstealth(randint1(sides) + base, FALSE, SUPERSTEALTH_TYPE_NORMAL);

		}
		break;

	case 7:
		{
			int dice = 5 + p_ptr->lev / 2;
			int sides = 15 + chr_adj / 2;
			if(sides < 1) sides = 1;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("あなたは太陽のように輝き始めた！");
			project_hack2(GF_LITE,dice,sides,0);
			break;
		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}








/*:::村紗用特技*/
class_power_type class_power_murasa[] =
{
	{3,3,20,FALSE,TRUE,A_WIS,0,1,"水弾",
		"低威力の水属性ボルトを放つ。"},
	{14,21,40,TRUE,TRUE,A_STR,0,15,"撃沈アンカー",
		"錨を投げつけ、命中した敵とその周辺に無属性ダメージを与える。腕力が高いと威力が上がる。"},
	{20,20,55,FALSE,TRUE,A_WIS,0,0,"ディープヴォーテックス",
		"自分の周辺に水属性攻撃を行い、さらに地形を水にする。すでに水地形上の場合威力が上がる。"},
	{32,30,65,FALSE,TRUE,A_DEX,0,0,"シンカーゴースト",
		"指定した水地形へテレポートする。通常の1/3の行動時間しか使わない。水地形にいないと使えない。"},
	{40,100,70,FALSE,TRUE,A_CHR,0,0,"船底のヴィーナス",
		"視界内全ての床を水没させ、敵に水属性ダメージを与える。"},
	{46,72,75,FALSE,TRUE,A_CHR,0,0,"ディープシンカー",
		"隣接した敵に強力な水属性攻撃を行い、非ユニークで水耐性のない生物を高確率で一撃で倒す。自分が深い水にいないと使えず、空を飛んでいる敵・力強い敵・巨大な敵には効きにくい。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::村紗特技*/
cptr do_cmd_class_power_aux_murasa(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 3 + plev / 5 + chr_adj / 7;
			int sides = 4;

			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("柄杓から水を飛ばした。");
			fire_bolt(GF_WATER,dir,damroll(dice,sides));
			break;
		}
	case 1:
		{
			int sides = plev * 3;
			int base = 30 + (adj_general[p_ptr->stat_ind[A_STR]] * 4);

			if(only_info) return format("損傷:%d+1d%d",base,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("錨を投げつけた！");
			fire_rocket(GF_ARROW,dir,base + randint1(sides),1);

			break;
		}
	case 2:
		{
			int dam = 20 + plev + chr_adj;
			int rad = 1 + plev / 24;
			if(cave_have_flag_bold((py), (px), FF_WATER)) dam *= 4;

			if(only_info) return format("損傷:～%d ",dam/2);

			if(cave_have_flag_bold((py), (px), FF_WATER))
			{
				msg_print("大渦が発生した！");
			}
			else
				msg_print("地面から水が噴き出した！");

			project(0, rad, py, px, rad+1, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, dam, GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();


			break;
		}
	case 3:
		{
			if(only_info) return format("");

			msg_print("あなたは水の中に消えた・・");
			if (!dimension_door(D_DOOR_MURASA)) return NULL;
			new_class_power_change_energy_need = 33;
			break;
		}
	case 4:
		{
			int damage = plev + chr_adj * 5;
			if(damage < 80) damage = 80;
			if(only_info) return format("損傷：%d",damage);
			msg_format("どこからともなく水が湧き出し、水位がみるみる上がっていく！");
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,damage);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

			break;
		}
	case 5:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = plev * 3 + chr_adj * 15;
			if(only_info) return format("損傷：%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int chance = 100;
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたは%sと共に水底へと沈んでいった・・",m_name);
				if((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE)
					|| !monster_living(r_ptr) || (r_ptr->flags1 & RF1_QUESTOR)
					|| (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					chance = 0;
				}
				else
				{
					if(r_ptr->flags7 & RF7_CAN_SWIM) chance = chance * 2 / 3;
					if(r_ptr->flags7 & RF7_CAN_FLY) chance /= 2;
					if(r_ptr->flags2 & RF2_POWERFUL) chance /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) chance /= 2;
				}
				if(randint1(r_ptr->level) < chance)
				{
					msg_format("%sを水底へ沈めた。", m_name);
					damage = m_ptr->hp + 1;
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_WATER,PROJECT_KILL,-1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::橙用特技 濡れている(p_ptr->magic_num1[0]が0でない)とき二つ目以外使用不可*/
class_power_type class_power_chen[] =
{
	{7,10,20,FALSE,TRUE,A_INT,0,0,"鳳凰卵",
		"混乱効果のあるボールを放つ。"},
	{15,20,40,TRUE,TRUE,A_DEX,50,0,"化猫「橙」",
		"離れた場所の敵へ一瞬で駆け寄り攻撃する。障害物に当たると止まる。装備が重いと失敗しやすい。"},
	{20,10,50,FALSE,TRUE,A_CHR,0,5,"天仙鳴動",
		"視界内全てに対し低威力の無属性攻撃を行う。"},
	{27,20,30,FALSE,FALSE,A_DEX,0,0,"飛び重ね鱗",
		"隣接した敵に攻撃し、そのまま一瞬で離脱する。離脱に失敗することもある。"},
	{33,30,50,FALSE,TRUE,A_INT,0,0,"飛翔韋駄天",
		"一時的に浮遊と加速を得る。"},
	{40,36,72,FALSE,TRUE,A_INT,0,0,"奇門遁甲",
		"一時的に酸・電撃・火炎・冷気に対する耐性を得る。"},
	{45,72,85,FALSE,TRUE,A_STR,0,0,"飛翔毘沙門天",
		"周囲に魔力属性のボールを撒き散らし、その後一瞬で離脱する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::橙特技*/
cptr do_cmd_class_power_aux_chen(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int range = 10 + plev / 10;
			int dam = 20 + plev + chr_adj;

			if(only_info) return format("射程:%d 損傷:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("仕込み卵を投げつけた。");
			fire_ball(GF_CONFUSION,dir,dam,2);

			break;
		}
	case 1:
		{
			int len = 3 + p_ptr->lev / 10;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 2:
		{
			int base = p_ptr->lev;
			int dice = 1;
			int sides = chr_adj * 2;
			if(only_info) return format("損傷:%dd%d+%d",dice,sides,base);
			if(one_in_(2))
				msg_format("あなたは腕を天に突き上げ奇声を上げた。");
			else
				msg_format("周囲に衝撃が走った！");
			project_hack2(GF_MISSILE,dice,sides,base);
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
			break;
		}
	case 4:
		{
			int base = 25;
			int sides = 25;
			int v;
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if(!use_itemcard) msg_format("あなたは妖術で宙を舞った！");
			v = randint1(sides) + base;

			set_tim_levitation(v, FALSE);
			set_fast(v, FALSE);
			break;
		}
	case 5:
		{
			int base = 25;
			int sides = 25;
			int v;
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if(!use_itemcard) msg_format("あなたは激しく複雑な動きで駆け回った。");
			v = randint1(sides) + base;
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			break;
		}
	case 6:
		{
			int rad = 2;
			int len = 8;
			int damage = 10 + plev + chr_adj * 3 ;
			if(damage < 75) damage = 75;

			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("鬼神の力が周囲を蹂躙した！");
			cast_meteor(damage, rad, GF_MANA);
			teleport_player(20,0L);

			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::幽香専用技*/
class_power_type class_power_yuuka[] =
{

	{20,25,50,FALSE,TRUE,A_CHR,0,0,"幻想郷の開花",
		"周囲に花を咲かせる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_yuuka(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			msg_print("周囲に花が咲き乱れた！");
			project(0, 3 + (plev / 10), py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::忍者専用技*/
class_power_type class_power_ninja[] =
{
	{ 3,3,25,FALSE,FALSE,A_WIS,0,0,"殺気感知",
		"周囲の精神を持つモンスターを感知する。"},
	{ 5,3,30,FALSE,FALSE,A_INT,0,0,"食料生成",
		"食料を調達する。"},
	{ 8,5,30,FALSE,TRUE,A_DEX,0,0,"金縛りの術",
		"敵一体の動きを止める。睡眠耐性のある敵やユニークモンスターには効果がない。レベル30になると視界内全てに金縛りを仕掛ける。"},
	{ 12,25,55,FALSE,TRUE,A_DEX,0,0,"毒液塗布",
		"装備中の並あるいは上質の武器に毒属性を付与する。武器の価格は99%減になる。"},
	{ 15,15,50,TRUE,FALSE,A_DEX,50,0,"一撃離脱",
		"敵に攻撃したあと、一瞬で短距離のテレポートをする。失敗することもある。"},
	{ 18,20,50,FALSE,FALSE,A_INT,0,0,"諜報術",
		"周囲の地形、アイテム、トラップ、モンスターを感知する。レベルが上がると効果範囲が広がる。"},
	{ 20,10,50,TRUE,FALSE,A_DEX,0,0,"投擲術",
		"アイテムを高威力で投げつける。(注：倍率はダイス部分にのみ掛かる)"},
	{ 23,10,45,FALSE,FALSE,A_DEX,0,0,"罠・扉破壊",
		"一直線上のトラップと閉じたドアを破壊する。"},
	{ 25,15,55,FALSE,TRUE,A_DEX,0,0,"火遁",
		"自分のいる場所に火炎の球を発生させ、短距離のテレポートをし、一時的に火炎に対する耐性を得る。"},
	{ 27,16,75,FALSE,FALSE,A_INT,0,0,"鑑識",
		"アイテムを鑑定する。"},
	{ 30,25,60,FALSE,FALSE,A_DEX,50,0,"変わり身の術",
		"攻撃を受けた時一度だけダメージを受けずにテレポートするようになる。失敗することもある。"},
	{ 33,20,70,FALSE,FALSE,A_INT,0,0,"看破",
		"視界内の敵のパラメータや能力を知る。"},
	{ 36,8,50,FALSE,FALSE,A_DEX,0,0,"煙玉",
		"自分のいる部屋を暗くする。"},
	{ 40,20,65,TRUE,FALSE,A_DEX,50,0,"縮地の術",
		"敵に一瞬で近寄り、そのまま攻撃する。障害物に当たるとそこで止まる。"},
	{ 44,50,70,FALSE,TRUE,A_INT,50,0,"分身の術",
		"一定時間、敵からの攻撃の半分を無効化する。「隠形の術」と同時には使えない。"},
	{ 48,72,75,FALSE,FALSE,A_DEX,50,0,"隠形の術",
		"一定時間、明るい場所でも敵から発見されにくくなる。「分身の術」と同時には使えない。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_ninja(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			int rad = DETECT_RAD_DEFAULT;
			if(only_info) return format("範囲:%d",rad);
			detect_monsters_mind(rad);
			break;
		}
	case 1:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "食料を生成";
			msg_print("食料を調達した。");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
	case 2:
		{
			int power = plev * 2;
			if(only_info) return format("効果:%d",power);

			if(plev < 30)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				msg_print("あなたは標的を指さし術をかけた。");
				(void)stasis_monster(dir);
				break;
			}
			else
			{
				msg_print("必殺の念を放った！");
				stasis_monsters(power);
			}
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			brand_weapon(4); //鈴蘭エゴ
			break;
		}
	case 4:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
			break;
		}

	case 5:
		{
			int rad = 5 + plev / 2;
			if(only_info) return format("範囲:%d",rad);
			msg_print("周辺を調査した・・");
			map_area(rad);
			detect_all(rad);
			break;

		}
	case 6:
		{
			int mult = 2 + plev / 15;
			if (only_info) return format("倍率：%d",mult);
			msg_print("あなたは全身を撓めた・・");
			if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			destroy_door(dir);
			break;

		}

	case 8:
		{
			base = 10 + plev / 5;
			if(only_info) return format("");
			fire_ball(GF_FIRE, 0, 50+plev, plev/10+2);
			teleport_player(30, 0L);
			set_oppose_fire(base + randint1(base), FALSE);
			break;

		}
	case 9:
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;
			break;
		}
	case 10:
		{
			if(only_info) return format("");
			if (!(p_ptr->special_defense & NINJA_KAWARIMI))
			{
				msg_print("あなたはこっそりと仕込みを済ませた・・");

				p_ptr->special_defense |= NINJA_KAWARIMI;
				p_ptr->redraw |= (PR_STATUS);
			}

			break;
		}
	case 11:
		{
			if(only_info) return format("");
			probing();
			break;
		}
	case 12:
		{
			if(only_info) return format("");
			msg_print("部屋が煙に包まれた！");
			unlite_room(py,px);
			break;
		}
	case 13:
		{
			int len = p_ptr->lev / 5;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 14:
		{
			dice = base = 6;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			set_tim_superstealth(0,TRUE,0);
			set_multishadow(base+randint1(dice), FALSE);
			break;
		}
	case 15:
		{
			dice = base = 15;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			set_multishadow(0, TRUE);
			msg_format("あなたの姿は半ば宙に溶けた・・");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::リグル専用技*/
class_power_type class_power_wriggle[] =
{

	{ 5,5,25,FALSE,TRUE,A_INT,0,0,"虫の知らせ",
		"周囲のモンスターの位置を感知する。"},
	{ 10,8,40,FALSE,TRUE,A_CHR,0,3,"蟲召喚",
		"周囲に蟲のモンスターを召喚する。"},
	{ 16,7,45,FALSE,TRUE,A_CHR,0,4,"地上の流星",
		"視界内のランダムな敵に無属性のボルトを放つ。レベルが上がると数が増える。"},
	{ 22,20,50,FALSE,FALSE,A_CHR,0,0,"蟲懐柔",
		"視界内の虫系の敵を配下か友好にしようと試みる。ユニークモンスターや高レベルな敵には効きにくい。"},
	{ 28,18,55,FALSE,TRUE,A_STR,0,2,"ファイヤフライフェノメノン",
		"周囲の敵に閃光ダメージを与え、さらに視界内の敵を混乱させる。"},
	{ 36,50,70,FALSE,TRUE,A_CHR,0,0,"ツツガムシ召喚",
		"周囲に「恙虫の群れ」を召喚する。"},
	{ 45,54,80,FALSE,TRUE,A_WIS,0,0,"ナイトバグトルネード",
		"ターゲットの位置に強力な閃光属性のボールを発生させる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_wriggle(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 9 + plev / 3;
			if(only_info) return format("範囲:%d",rad);
			msg_print("蟲たちが周りの様子を知らせてくれた。");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);

			break;
		}
	case 1:
		{
			bool flag = FALSE;
			if(only_info) return "";
			for(i=0;i<(2 + p_ptr->lev / 15) ;i++)
			{
				if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_SPIDER, (PM_ALLOW_GROUP | PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("蟲達を呼び出した！");
			else
				msg_print("蟲は現れなかった。");
			break;

		}
	case 2:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 10;
			int sides = 3 + chr_adj / 10;
			int num = 4 + plev / 8;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("蟲たちを放った！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_ARROW, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}
	case 3:
		{
			int j;
			int chance = plev * 2 + chr_adj * 5;
			if(only_info) return format("");

			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}


			if(one_in_(4))msg_print("あなたは特殊な香りを放った・・");
			else if(one_in_(3))msg_print("あなたは蟲の言葉で話しかけた・・");
			else if(one_in_(2))msg_print("あなたは妖しい光を放った・・");
			else msg_print("あなたは触覚を誘惑的に動かした・・");

			for (j = 1; j < m_max; j++)
			{
				char m_name[80];
				monster_type *m_ptr;
				monster_race *r_ptr;
				bool success = FALSE;

				//視界内の虫系の敵が対象
				m_ptr = &m_list[j];
				if(!m_ptr->r_idx) continue;
				if(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) continue;
				if(!projectable(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if(is_pet(m_ptr) || is_friendly(m_ptr)) continue;
				if(!m_ptr->ml) continue;

				r_ptr = &r_info[m_ptr->r_idx];
				if(r_ptr->d_char != 'I' && r_ptr->d_char != 'c' && r_ptr->d_char != 'w') continue;

				monster_desc(m_name, m_ptr, 0);
				(void)set_monster_csleep(j, 0);
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || r_ptr->level > plev)
				{
					chance /= 2;
					if(r_ptr->level < randint1(chance))
					{
						msg_format("%sと仲良くなった！",m_name);
						set_friendly(m_ptr);
						success = TRUE;
					}
				}
				else
				{
					if(r_ptr->level < randint1(chance))
					{
						msg_format("%sはあなたに魅せられた！",m_name);
						set_pet(m_ptr);
						success = TRUE;
					}
				}
				if(!success) msg_format("%sはあなたに敵対している。",m_name);

			}

			break;
		}
	case 4:
		{
			int rad = 2 + plev / 12;
			base = p_ptr->lev * 3 + chr_adj * 5;
			if(only_info) return format("損傷:～%d",base / 2);
			msg_format("あなたは眩い光を放った！");
			project(0, rad, py, px, base, GF_LITE, PROJECT_KILL | PROJECT_ITEM, -1);
			confuse_monsters(plev * 3);
			break;
		}
	case 5:
		{
			bool flag = FALSE;
			int max = p_ptr->lev / 10;
			if(max < 2) max = 2;
			if(only_info) return format("最大:%d体",max);

			for(i=0;i<max;i++) if(summon_named_creature(0, py, px, MON_TUTUGAMUSHI, PM_FORCE_PET)) flag = TRUE;
			if(flag) msg_print("ツツガムシを呼び出した！");
			else msg_print("蟲は現れなかった。");

			break;
		}
	case 6:
		{
			int dice = plev/2 ;
			int sides = 15 + chr_adj / 2;
			int rad = 4;

			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LITE,dir,damroll(dice,sides),rad,"光の群れが標的へと襲い掛かった！")) return NULL;

			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::こころ専用技*/
class_power_type class_power_kokoro[] =
{
	{ 3,4,30,FALSE,TRUE,A_WIS,0,0,"感情探知",
		"周囲の精神を持つモンスターを感知する。"},
	{ 5,10,0,FALSE,TRUE,A_DEX,0,0,"面生成",
		"隣接した敵一体に攻撃を行う。攻撃の命中率は格闘攻撃に準ずる。この攻撃で倒した敵は面を落とし、装備するとその敵の能力を再現できる。精神を持たない敵には効かない。"},
	{ 5,0,0,FALSE,TRUE,A_DEX,0,0,"再演",
		"装備している面の敵が持っていた技や魔法を再現する。難易度や消費MPの判定は個別に行われる。威力は面の元となった敵の能力に依存するがブレスなどのHPは1/6として計算される。"},
	{ 12,12,35,FALSE,TRUE,A_WIS,0,5,"喜怒哀楽ポゼッション",
		"敵一体を高確率で恐怖、混乱させる。レベル30になると少し効果が落ちるが視界内全ての敵が対象になり朦朧効果が追加される。"},
	{ 17,16,40,FALSE,TRUE,A_DEX,0,0,"こころのルーレット",
		"恐怖、祝福、ヒーロー、狂戦士化のいずれかの状態になる。レベル30で全耐性、レベル45で体力回復+全復活の効果が候補に加わる。"},
	{ 22,20,50,FALSE,TRUE,A_CHR,0,2,"狂喜の火男面 ",
		"歓喜の舞とともに花火が炸裂し、周囲に複数回の火炎属性攻撃を行う。"},
	{ 30,25,50,FALSE,TRUE,A_DEX,0,0,"怒れる忌狼の面",
		"短距離を突進し、敵に当たると気属性ダメージを与えて止まる。レベルが上がると距離が延びる。"},
	{ 36,40,80,FALSE,TRUE,A_STR,0,0,"昂揚の神楽獅子",
		"プラズマ属性の強力なレーザーを放つ。"},
	{ 45,96,85,FALSE,TRUE,A_CHR,0,0,"モンキーポゼッション",
		"視界内全てに魔力・暗黒・閃光・気属性の強力な攻撃を行う。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_kokoro(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int rad = 10 + plev / 2;
			if(only_info) return format("範囲:%d",rad);

			detect_monsters_mind(DETECT_RAD_DEFAULT);
			break;
		}

	case 1:
		//面に加える
		{

			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int dir, damage;

			damage= plev * 4;

			if(only_info) return  format("損傷:1d%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				int chance;
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				chance = (p_ptr->skill_thn + (p_ptr->to_h[0] + p_ptr->lev * ref_skill_exp(SKILL_MARTIALARTS) / 8000)* BTH_PLUS_ADJ);
				msg_format("あなたは%sへ飛びかかった！",m_name);

				if(MON_CSLEEP(m_ptr) || test_hit_norm( chance, r_ptr->ac, m_ptr->ml))
				{
					project(0,0,y,x,randint1(damage),GF_KOKORO,(PROJECT_KILL | PROJECT_HIDE),0);
				}
				else
				{
					msg_format("しかし躱された。",m_name);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;

	case 2:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}

	case 3:
		{
			int power = plev * 5 + chr_adj * 5;
			if(plev > 29) power -= 100;
			if(only_info) return format("効力:%d",power);

			if(plev < 30)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("視界内のターゲットを明示的に指定しないといけない。");
					return NULL;
				}
				msg_print("感情の波動を放った！");
				fire_ball_hide(GF_OLD_CONF,dir,power,0);
				fire_ball_hide(GF_TURN_ALL,dir,power,0);
			}
			else
			{
				msg_print("感情の波動を周囲に放った！");
				confuse_monsters(power);
				stun_monsters(power);
				turn_monsters(power);
			}


			break;
		}

	case 4:
		{
			int time;
			int base = 20;
			int choice;
			if(only_info) return format("期間:%d + 1d%d",base,base);

			if(plev > 44) choice = 6;
			else if(plev > 29) choice = 5;
			else choice = 4;
			time = base + randint1(base);

			switch(randint1(choice))
			{
				case 1:
					msg_print("嘆く老婆の面を引いた！");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					if (!p_ptr->resist_fear) set_afraid(p_ptr->afraid + time);
					break;
				case 2:
					msg_print("笑う老爺の面を引いた！");
					set_hero(time,FALSE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					set_afraid(0);
					break;
				case 3:
					msg_print("お多福の面を引いた！");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(time,FALSE);
					set_afraid(0);
					break;

				case 4:
					msg_print("般若の面を引いた！");
					set_hero(0,TRUE);
					set_shero(time,FALSE);
					set_blessed(0,TRUE);
					set_afraid(0);
					break;
				case 5:
					msg_print("天狗の面を引いた！");
					set_hero(0,TRUE);
					set_shero(0,TRUE);
					set_blessed(0,TRUE);
					set_afraid(0);
					set_oppose_acid(time, FALSE);
					set_oppose_elec(time, FALSE);
					set_oppose_fire(time, FALSE);
					set_oppose_cold(time, FALSE);
					set_oppose_pois(time, FALSE);
					break;
				case 6:
					msg_print("新たな希望の面を引いた！");
					hp_player(500);
					set_stun(0);
					set_cut(0);
					set_poisoned(0);
					set_image(0);
					restore_level();
					do_res_stat(A_STR);
					do_res_stat(A_INT);
					do_res_stat(A_WIS);
					do_res_stat(A_DEX);
					do_res_stat(A_CON);
					do_res_stat(A_CHR);
					break;
				default:
					msg_print("ERROR:こころルーレットで未定義の数値が出た");
					return NULL;
			}



		}
		break;
	case 5:
		{
			int rad = 2 + plev / 24;
			int damage = plev  + chr_adj;
			if(damage < 20) damage = 20;

			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			if(use_itemcard)
				msg_format("カードから花火が舞い散った！");
			else
				msg_format("歓喜の舞を舞った！");
			cast_meteor(damage, rad, GF_FIRE);

			break;
		}
	case 6:
		{
			int len = plev / 7;
			int damage = 50 + plev * 2;
			if(len < 4) len = 4;

			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack2(len,GF_FORCE,damage)) return NULL;
			break;
		}
	case 7:
		{
			dice = p_ptr->lev / 3;
			sides = 10 + chr_adj;
			if(dice < 10) dice = 10;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("獅子の面の口から熱線が放たれた！");
			fire_spark(GF_PLASMA,dir, damroll(dice,sides),1);

			break;
		}
	case 8:
		{
			base = p_ptr->lev;
			dice = 1;
			sides = chr_adj * 5;
			if(only_info) return format("損傷:(%dd%d+%d) * 4",dice,sides,base);
			msg_format("憑かれたように踊り狂った！");
			project_hack2(GF_MANA,dice,sides,base);
			project_hack2(GF_DARK,dice,sides,base);
			project_hack2(GF_LITE,dice,sides,base);
			project_hack2(GF_FORCE,dice,sides,base);
		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::神子専用技*/
class_power_type class_power_miko[] =
{
	{ 1,0,0,FALSE,FALSE,A_INT,0,0,"モンスター感知",
		"卓越した聴覚で周囲のモンスターを感知する。レベルが上がると範囲が広がる。"},
	{ 7,12,30,FALSE,TRUE,A_WIS,0,5,"日出ずる処の天子",
		"自分の周囲に閃光属性のボールを複数発生させる。"},
	{ 12,16,45,FALSE,FALSE,A_INT,0,0,"看破",
		"卓越した眼力で視界内のモンスターのパラメータや特技などを知る。"},
	{ 18,18,50,FALSE,TRUE,A_WIS,0,3,"十七条のレーザー",
		"破邪属性のビームを放つ。"},
	{ 24,30,85,FALSE,TRUE,A_DEX,0,0,"縮地のマント",
		"視界内の任意の場所にテレポートする。この特技は(100-レベル)%の行動時間しか消費しない。"},
	{ 29,32,70,FALSE,TRUE,A_CHR,0,8,"グセフラッシュ",
		"視界内の全てに対し閃光属性の攻撃を行う。"},
	{ 36,100,80,FALSE,TRUE,A_CHR,0,0,"豪族乱舞",
		"「物部布都」「蘇我屠自古」を召喚する。召喚された二人は捕獲などの対象にならず階移動すると消える。"},
	{ 41,170,85,FALSE,TRUE,A_STR,0,0,"十七条の憲法爆弾",
		"自分の周囲に破邪属性の強力なボールを複数発生させる。"},
	{ 48,240,80,FALSE,TRUE,A_CHR,0,0,"詔を承けては必ず慎め",
		"指定した方向へ強力な攻撃を行う。威力は魅力に大きく依存する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_miko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int rad = 5 + plev * 2 / 5;
			if(only_info) return format("範囲:%d",rad);

			msg_format("あなたは耳当てをはずした。");
			detect_monsters_normal(rad);
			detect_monsters_invis(rad);
			msg_format("耳当てをつけた。");
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 20;
			int damage = plev  + chr_adj ;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("多数の光弾を放った！");
			cast_meteor(damage, rad, GF_LITE);

			break;
		}
	case 2:
		{
			if(only_info) return format("");
			msg_format("周囲を見渡した・・");
			probing();
			break;
		}
	case 3:
		{
			dice = p_ptr->lev / 3;
			sides = chr_adj / 3 + p_ptr->lev / 10;
			base = p_ptr->lev + chr_adj * 3;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("十七条の光線が放たれた！");
			fire_beam(GF_HOLY_FIRE, dir, base + damroll(dice,sides));
			break;
		}
	case 4:
		{
			int x, y;
			if(only_info) return format("");
			if (!tgt_pt(&x, &y)) return NULL;
			if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT ) ||
			    !projectable(py, px, y, x))
			{
				msg_print("そこへは移動できない。");
				return NULL;
			}
			else if (p_ptr->anti_tele)
			{
				msg_print("不思議な力がテレポートを防いだ！");
			}
			else
			{
				msg_print("あなたは離れた場所に一瞬で現れた。");
				teleport_player_to(y, x, 0L);
				new_class_power_change_energy_need = 100 - plev;
			}
			break;
		}
	case 5:
		{
			dice = p_ptr->lev / 2;
			sides = 10 + chr_adj / 2;
			if(dice < 15) dice=15;
			if(sides < 1) sides = 1;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("あなたは眩い光を放った！");
			project_hack2(GF_LITE,dice,sides,0);
			break;
		}
	case 6:
		{
			int i;
			bool huto = FALSE, toziko = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_FUTO) ) huto = TRUE;
				if((m_ptr->r_idx == MON_TOZIKO)) toziko = TRUE;
			}
			if(!huto && summon_named_creature(0, py, px, MON_FUTO, PM_EPHEMERA))
			{
				if(one_in_(3))msg_print("「われにおまかせを！」");
				else if(one_in_(2))msg_print("「はなしにもならぬわ！」");
				else msg_print("「まだまだぁ！」");
			}
			if(!toziko && summon_named_creature(0, py, px, MON_TOZIKO, PM_EPHEMERA))
			{
				if(one_in_(3))msg_print("「やってやんよ！」");
				else if(one_in_(2))msg_print("「おろかものめが！」");
				else msg_print("「またですか？」");
			}

			if(huto && toziko) msg_print("すでに二人ともこのフロアにいる。");
			break;

		}

	case 7:
		{
			int rad = 4;
			int damage = plev * 2 + chr_adj * 2;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_format("光の爆弾が降り注いだ！");
			cast_meteor(damage, rad, GF_HOLY_FIRE);

			break;
		}
	case 8:
		{
			dice = p_ptr->lev / 2;
			sides = chr_adj * 3;
			if(dice < 15) dice = 15;
			if(sides < 30) sides = 30;

			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な光の剣が大地を切り裂いた！");
			fire_spark(GF_PSY_SPEAR,dir, damroll(dice,sides),1);

			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::はたて専用技*/
class_power_type class_power_hatate[] =
{
	{ 5,6,35,FALSE,TRUE,A_INT,0,2,"念写Ⅰ",
		"周囲の地形・アイテム・モンスターを感知する。トラップは感知できない。レベルが上がると感知半径が広がる。"},
	{ 12,18,45,FALSE,TRUE,A_DEX,0,3,"ラピッドショット",
		"自分の近くのランダムな位置に閃光属性攻撃を行う。"},
	{ 20,24,50,FALSE,TRUE,A_INT,0,0,"念写Ⅱ",
		"キーワードを入力し、それに一致したアイテムやモンスターがフロアに存在すればその周辺を念写する。複数の候補があった場合適当な一箇所が選ばれる。"},
	{ 27,10,60,FALSE,TRUE,A_DEX,0,0,"籠もりパパラッチ",
		"指定した敵の写真を撮る。射線が通っていない敵にも発動できる。"},
	{ 36,45,70,FALSE,TRUE,A_DEX,0,0,"フルパノラマショット",
		"視界内全てに閃光属性攻撃を行う。"},
	{ 48,100,70,FALSE,TRUE,A_INT,0,0,"念写Ⅲ",
		"フロア全ての地形・アイテム・モンスターを感知する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_hatate(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;

	switch(num)
	{

	case 0:
		{
			int rad = 9 + plev / 3;
			if(only_info) return format("範囲:%d",rad);
			if(use_itemcard)
				msg_print("カメラに周囲の光景が写った！");
			else
				msg_print("あなたはカメラに念を込め、シャッターを押した。");

			map_area(rad);
			detect_objects_normal(rad);
			detect_objects_gold(rad);
			detect_monsters_normal(rad);
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 30;
			int damage = 10 + plev + chr_adj * 2;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_print("周囲を無差別に撮影した！");
			cast_meteor(damage, rad, GF_LITE);

			break;
		}
	case 2:
		{
			if(only_info) return format("");
			if(!hatate_psychography()) return NULL;
			break;

		}
	case 3:
		{
			if(only_info) return format("");

			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !cave[target_row][target_col].m_idx || !m_list[cave[target_row][target_col].m_idx].ml)
			{
				msg_print("ターゲットを指定しないといけない。");
				return NULL;
			}

			msg_print("あなたは被写体を思い浮かべ、シャッターを押した。");
			project(0, 0, target_row, target_col, 1, GF_PHOTO,  PROJECT_JUMP | PROJECT_KILL, -1);

			break;
		}
	case 4:
		{
			int damage = plev*2 + chr_adj * 2;
			if(only_info) return format("損傷:%d",damage);
			msg_print("周囲を広角モードで連続撮影した！");
			project_hack2(GF_LITE,0,0,damage);
			break;
		}

	case 5:
		{
			if(only_info) return format("");
			msg_print("全力でカメラに集中した・・");
			wiz_lite(FALSE);
			(void)detect_monsters_normal(255);

			break;

		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::キスメ専用技*/
class_power_type class_power_kisume[] =
{
	{ 5,0,0,FALSE,FALSE,A_DEX,0,0,"飛び上がる",
		"高く飛び上がる。飛び上がっている時は次の攻撃が空中からの落下攻撃になり、攻撃回数が1になるが大幅なダイスボーナスが付く。また高度に応じて敵からの攻撃が当たりにくくなるが、巨大な敵・空を飛ぶ敵・賢い敵・力強い敵には効果が薄い。"},
	{ 10,3,25,FALSE,TRUE,A_CHR,0,3,"鬼火",
		"空中からターゲットの位置に火炎属性の球を落とす。射程距離は短いが高く飛び上がっていると射程が伸びる。この技の使用時には高度が下がらない。"},
	{ 18,18,35,TRUE,FALSE,A_STR,0,10,"釣瓶落としの怪",
		"指定位置に落下しそこにいる敵にダメージを与える。高く飛び上がっていると射程が伸び威力が上がる。また桶が丈夫だと威力が上がる。"},
	{ 27,16,50,FALSE,TRUE,A_DEX,0,0,"飛んで井の中",
		"指定位置に落下し物理攻撃ダメージと水ダメージを与え、さらに周囲を水地形にし、一時的な火炎耐性を得る。高く飛び上がっていると射程が伸び威力が上がる。また桶が丈夫だと威力が上がる。"},
	{ 40,80,70,FALSE,TRUE,A_CON,0,0,"ウェルディストラクター",
		"指定位置に落下し隕石属性の巨大な球を発生させ、さらに地震を起こす。高く飛び上がっていると射程が伸び威力が上がる。また桶が丈夫だと威力が上がる。騒音が発生し周囲の敵が起きる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_kisume(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int cons = p_ptr->concent;

	if(use_itemcard) cons = 3 + plev / 20;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");

			do_cmd_concentrate(0);
			break;
		}
	case 1:
		{
			int range = 2 + cons * 2;
			int dam = 20 + plev * 2 + chr_adj;

			if(only_info) return format("射程:%d 損傷:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("空中から鬼火を落とした。");
			fire_ball(GF_FIRE,dir,dam,2);

			reset_concent = FALSE; //高度を維持する
			break;
		}
	case 2:
		{
			int range = 4 + cons * 2;
			int base = (plev) * (cons+1) + calc_kisume_bonus();
			int tx,ty,dam;

			if(only_info) return format("損傷:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_ARROW,dir,dam,0,"あなたは飛び上がり、標的目掛けて落下した！")) return NULL;
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			//周りがみっしり敵のときどうなるのかチェック要
			break;
		}
	case 3:
		{
			int range = 3 + cons * 2;
			int base;
			int tx,ty,dam;

			if(use_itemcard)
				base = plev * (cons+1) / 2 + 20 * cons;
			else
				base = plev * (cons+1) / 2 + calc_kisume_bonus();

			if(only_info) return format("損傷:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_WATER_FLOW,dir,3,3,"地面にヒビが入り、水が噴き出した！")) return NULL;
			fire_ball_jump(GF_WATER,dir,dam,3,NULL);
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			set_oppose_fire(randint1(20) + 20, FALSE);

			break;
		}
	case 4:
		{
			int range = 1 + cons;
			int base = plev * (1 + cons*2) + calc_kisume_bonus() * 3;
			int tx,ty,dam;

			if(only_info) return format("損傷:%d + 1d%d",base/2,base/2);

			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			dam = base / 2 + randint1(base/2);
			ty = target_row;
			tx = target_col;
			if(!fire_ball_jump(GF_METEOR,dir,dam,5,"あなたは隕石のように大地を穿った！")) return NULL;
			teleport_player_to(ty,tx,TELEPORT_NONMAGICAL);
			earthquake(ty, tx, 5);
			aggravate_monsters(0,FALSE);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::リリーホワイト専用技*/
class_power_type class_power_lilywhite[] =
{
	{1,1,0,FALSE,TRUE,A_CHR,0,0,"弾幕",
		"無属性のボルトを放つ。魅力が高いと威力が上がる。"},

	{25,10,45,FALSE,TRUE,A_CHR,0,4,"サプライズスプリング",
		"ターゲットの位置周辺に向け大量の無属性ボルトを放つ。魅力が高いと威力が上がる。"},

	{40,40,60,FALSE,TRUE,A_CHR,0,0,"春の訪れ",
		"周囲に春が訪れる。"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_lilywhite(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int dam = plev + chr_adj;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("弾幕を放った。");
			fire_bolt(GF_MISSILE, dir, dam);
			break;
		}
	case 1:
		{
			int num = plev / 2;
			int sides = (plev + chr_adj)/3;
			if(only_info) return format("損傷:1d%d * %d",sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("大量の花びらが散った！");

			fire_blast(GF_MISSILE, dir, 1, sides, num, 3, 0);
			break;
		}
	case 2:
		{
			int rad = plev / 7;
			if(rad < 4) rad = 4;
			if(only_info) return format("");
			msg_print("あなたは高らかに春の到来を告げた！");
			project(0, rad, py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::刀自古専用技*/
class_power_type class_power_toziko[] =
{
	{5,2,25,FALSE,TRUE,A_STR,0,3,"雷の矢",
		"電撃属性のボルトを放つ。レベルが上がると弾数が増える。"},

	{20,20,50,FALSE,TRUE,A_WIS,0,5,"ガゴウジサイクロン",
		"自分のいる位置に強力な電撃属性のボールを発生させる。"	},

	{25,50,80,FALSE,TRUE,A_DEX,0,0,"矢生成",
		"「電撃の矢」を数本生成する。"	},

	{36,54,65,FALSE,TRUE,A_WIS,0,15,"ガゴウジトルネード",
		"視界内の全てに対し電撃攻撃を行う。"	},

	{45,88,75,FALSE,TRUE,A_CHR,0,0,"入鹿の雷",
		"ターゲットの場所に強力な電撃のボールを発生させる。HPが低いほど威力が上がる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},

};


cptr do_cmd_class_power_aux_toziko(int num, bool only_info)
{
	int dir,dice,sides,base,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int num = 1 + plev / 8;

			dice = 3;
			sides = 6 + plev / 15 + chr_adj / 10 ;
			if(only_info) return format("損傷:%dd%d * %d",dice,sides,num);

			if (!get_aim_dir(&dir)) return NULL;
			msg_print("雷の矢を放った。");
			for (i = 0; i < num; i++) fire_bolt(GF_ELEC, dir, damroll(dice, sides));
			break;
		}
	case 1:
		{
			int rad = 2 + plev / 15;
			base = plev * 8 + chr_adj * 10;
			if(base < 200) base = 200;
			if(only_info) return format("損傷:～%d",base / 2);
			if(use_itemcard)
				msg_format("カードが激しく放電した！");
			else
				msg_format("あなたは激しく放電した。");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}
	case 2:
		{
			int xx,yy;
			object_type forge;
			object_type *o_ptr = &forge;

			if(only_info) return format("");

			object_prep(o_ptr, (int)lookup_kind(TV_ARROW, SV_AMMO_LIGHT));
			o_ptr->number = plev / 10 + randint1(plev/10);
			o_ptr->to_d = p_ptr->lev / 5;
			o_ptr->to_h = p_ptr->lev / 5;
			o_ptr->name2 = EGO_ARROW_ELEC;
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->discount = 99;
			msg_format("稲妻を矢の形にした。");
			(void)drop_near(o_ptr, -1, py, px);

			break;
		}
	case 3:
		{
			dice = p_ptr->lev / 2;
			sides = 15 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("稲妻の嵐が周囲を焼き払った！");
			project_hack2(GF_ELEC,dice,sides,0);
			break;
		}

	case 4:
		{
			int dice = plev/2 + chr_adj/2;
			int sides = plev/2;
			int rad ;
			int mult = 100 + 300 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;

			if(dice < 20) dice = 20;
			if(sides < 16) sides = 16;
			sides = sides * mult / 100;
			rad = mult / 50;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC,dir,damroll(dice,sides),rad,"雷の鉄槌が下された！")) return NULL;

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::響子用特技*/
class_power_type class_power_kyouko[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"黙る",
		"唱えている念仏や歌っている歌があれば止める。行動力を消費しない。"},
	{6,12,25,TRUE,FALSE,A_STR,0,15,"チャージドクライ",
		"自分の周囲に轟音属性の攻撃を行う。威力は現在HPに依存する。周囲の寝ている敵が起きる。"},
	{10,20,0,FALSE,FALSE,A_DEX,0,0,"マウンテンエコー",
		"このフロアであなたが最後に聞いた呪文を繰り返す。威力や効果はあなたの能力に依存する。"},
	{18,12,50,FALSE,TRUE,A_DEX,0,2,"ロングレンジエコー",
		"ターゲットの位置に轟音属性の球を発生させる。"},
	{24,32,60,FALSE,TRUE,A_DEX,0,7,"パワーレゾナンス",
		"視界内全てに対し轟音で攻撃する。周囲の寝ている敵が起きる。"},
	{30,24,65,FALSE,TRUE,A_WIS,0,0,"無限念仏",
		"ひたすら大声で念仏を唱え続け、唱えている間周囲の敵にダメージを与え朦朧、恐怖させる。破邪を弱点とする敵にしか効かず、高レベルな敵には効きにくい。"},
	{38,36,75,FALSE,FALSE,A_STR,0,0,"プライマルスクリーム",
		"絶叫のような歌を歌い、周囲の敵に轟音属性攻撃をし続ける。周囲の寝ている敵が起きる。"},
	{44,60,80,FALSE,FALSE,A_CON,0,0,"チャージドヤッホー",
		"轟音属性の強力なビームを放つ。周囲の寝ている敵が起きる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::響子特技*/
/*::: p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
/*::: p_ptr->magic_num1[5]がオウム返し候補の呪文番号格納に使われる*/
///mod160305 オウム返し呪文番号をnum1[5]でなくmonspell_yamabikoに格納することにした
cptr do_cmd_class_power_aux_kyouko(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//黙る　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1: //チャージドクライ
		{
			int rad = 1 + plev / 10;
			int dam = p_ptr->chp * 2 / 3;
			if(dam > 1600) dam=1600;
			if(only_info) return format("損傷:～%d",dam / 2);

			stop_singing();
			if(one_in_(2)) msg_format("「おはよーございます！」");
			else msg_format("「YAH○O！」");
			project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL|PROJECT_GRID , -1);

			aggravate_monsters(0,FALSE);
			break;
		}
	case 2://マウンテンエコー
		{
			cptr mname;
			if(monspell_yamabiko) mname = monspell_list2[monspell_yamabiko].name;
			else mname = "なし";

			if(only_info) return mname;

			if(!monspell_yamabiko)
			{
				msg_format("返す言葉がない。");
				return NULL;
			}

			if(!use_itemcard) stop_singing();
			if(!cast_monspell_new_aux(monspell_yamabiko,FALSE,FALSE,CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO)) return NULL;

			kyouko_echo(TRUE,0);//一度オウム返ししたら消去
			break;
		}
	case 3://ロングレンジエコー
		{
			int dir;
			int rad = 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = 5 + p_ptr->lev / 8;
			int base = p_ptr->lev ;

			if(dice < 5) dice = 5;

			if(p_ptr->lev > 39) rad = 4;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_SOUND, dir, base + damroll(dice,sides), rad, "木霊が一点に収束した！")) return NULL;
			break;
		}


	case 4://パワーレゾナンス
		{
			int base = 50 + plev * 2;

			if(only_info) return format("損傷:%d",base);
			stop_singing();
			if(dun_level) msg_format("あなたの大声がダンジョンに響き渡った！");
			else msg_format("あなたの大声が野山に響き渡った！");
			project_hack2(GF_SOUND,0,0,base);
			aggravate_monsters(0,FALSE);
			break;
		}
	case 5://無限念仏
		{
			int dice = plev/5 +5;
			if(only_info) return  format("損傷:%dd%d/ターン",dice,dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_KYOUKO_NENBUTSU, SPELL_CAST);

		}
		break;
	case 6://プライマルスクリーム
		{
			int dice = plev * 2;
			if(only_info) return  format("損傷:1d%d/ターン",dice);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_KYOUKO_SCREAM, SPELL_CAST);
		}
		break;
	case 7://チャージドヤッホー
		{
			int dice = p_ptr->lev / 2;
			int sides = 20 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			stop_singing();
			msg_format("全身全霊の叫びを放った！");
			fire_spark(GF_SOUND,dir, damroll(dice,sides),1);
			aggravate_monsters(0,FALSE);

			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::さとり用特技*/
class_power_type class_power_satori[] =
{
	{1,0,0,FALSE,TRUE,A_INT,0,0,"読心",
		"視界内の敵について調査し、さらに敵の持つ特技を記憶する。記憶している特技は読心をするたびに書き換えられ、フロアを出るとリセットされる。通常の精神を持たない敵には効果がない。"},
	{1,0,0,FALSE,TRUE,A_INT,0,0,"想起",
		"読心で記憶したモンスターの魔法や技を再現する。威力はプレイヤーの能力に依存し、再現するものにより難易度や消費MPが変化する。"},
	{16,20,50,FALSE,TRUE,A_WIS,0,10,"テリブルスーヴニール",
		"ターゲットのトラウマを抉り出して強力な精神攻撃を行う。アンデッドに効きやすく、ユニークモンスターや力強い敵には効きにくく、通常の精神を持たない敵には効かない。レベル35以降は視界内攻撃になる。"},

	{20,10,55,FALSE,TRUE,A_INT,0,0,"残留思念解読",
		"アイテムの大まかな価値を判定する。レベル30以上で通常の鑑定と同じ効果になる。" },

	{32,50,70,FALSE,TRUE,A_INT,0,0,"恐怖催眠術",
		"恐怖している隣接モンスター一体を強制的に配下にする。通常の精神を持たないモンスターとクエスト打倒対象モンスターには効果がない。ユニークモンスターやパラメータが特殊なモンスターはこのフロアのみの配下になる。" },

	{40,72,100,FALSE,TRUE,A_CHR,0,0,"ブレインフィンガープリント",
		"視界内全てに精神攻撃を行う。相手の体力が減っているほど効果が高い。アンデッドに効きやすく、ユニークモンスターや力強い敵には効きにくく、通常の精神を持たない敵には効かない。" },

		{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_satori(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");

			satori_reading = TRUE;
			probing();
			satori_reading = FALSE;
			make_magic_list_satori(FALSE);


			break;
		}
	case 1:
		{
			if(only_info) return "";

			if(!cast_monspell_new()) return NULL;
			break;
		}
	case 2:
		{
			int dam = plev * 3 + chr_adj * 5;
			if(dam < 100) dam = 100;
			if(only_info) return format("損傷:%d",dam);

			if(plev < 35 && !use_itemcard)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("視界内のターゲットを明示的に指定しないといけない。");
					return NULL;
				}
				msg_print("あなたの第三の眼が妖しい光を放った・・");
				fire_ball_hide(GF_SATORI,dir,dam,0);
			}
			else
			{
				if(!use_itemcard)
					msg_print("あなたの第三の眼が妖しい光を放った・・");
				else
					msg_print("カードに描かれた目が妖しい光を放った・・");
				project_hack2(GF_SATORI,0,0,dam);
			}


			break;
		}

	case 3: //v1.1.84 残留思念解読
	{

		if (only_info) return format(" ");

		if (plev < 30)
		{
			if (!psychometry()) return NULL;
		}
		else
		{
			if (!ident_spell(FALSE)) return NULL;
		}

	}
	break;


	case 4://恐怖催眠術(アブソリュートルーザーから少し改変)
	{
		int y, x;
		int i;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (only_info) return format(" ");

		if (p_ptr->inside_arena)
		{
			msg_print("今その特技は使えない。");
			return NULL;
		}

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (cave[y][x].m_idx && (m_ptr->ml))
		{
			int tim;
			bool flag_success = FALSE;
			char m_name[80];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			//起こす
			set_monster_csleep(cave[y][x].m_idx, 0);

			if (is_pet(m_ptr))
			{
				msg_format("%sはすでにあなたの下僕だ。", m_name);
			}
			else if ((r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->flagsr & RFR_RES_ALL) || r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				msg_format("%sには効果がなかった！", m_name);
			}
			else if (!MON_MONFEAR(m_ptr))
			{
				msg_format("%sの心には隙がない。もっと恐怖を与える必要がある。", m_name);
			}
			else
			{
				msg_format("第三の目が%sをじっと見つめた...", m_name);
				set_pet(m_ptr);

				//ユニークや可変パラメータモンスターはフロアから出ると消えることにしておく
				if (r_ptr->flags7 & RF7_VARIABLE || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
					m_ptr->mflag |= (MFLAG_EPHEMERA);
				msg_format("あなたは%sの精神を支配した。", m_name);
			}
		}
		else
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
	}
	break;

	case 5:
	{
		if (only_info) return format("損傷:不定");

		msg_print("あなたの第三の目が煌々と輝いた。");
		project_hack2(GF_BRAIN_FINGER_PRINT, 0, 0, 1);

		break;
	}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::宝石(珍品は含まない)*/
bool item_tester_hook_jewel(object_type *o_ptr)
{
	int sv = o_ptr->sval;
	if(o_ptr->tval != TV_MATERIAL) return FALSE;

	if(sv == SV_MATERIAL_GARNET || sv == SV_MATERIAL_AMETHYST || sv == SV_MATERIAL_AQUAMARINE ||
	   sv == SV_MATERIAL_DIAMOND || sv == SV_MATERIAL_EMERALD || sv == SV_MATERIAL_MOONSTONE ||
	   sv == SV_MATERIAL_RUBY  || sv == SV_MATERIAL_PERIDOT || sv == SV_MATERIAL_SAPPHIRE ||
	   sv == SV_MATERIAL_OPAL  || sv == SV_MATERIAL_TOPAZ || sv == SV_MATERIAL_LAPISLAZULI) return TRUE;

	return FALSE;
}


/*:::宝飾師の魔力食い*/
class_power_type class_power_jeweler[] =
{
	{25,1,60,FALSE,TRUE,A_INT,0,0,"魔力食い(宝石)",
		"宝石から魔力を吸い取りMPを回復する。吸い取れる魔力の量は宝石の種類で変わる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_jeweler(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			cptr q,s;
			int item;
			object_type *o_ptr;
			int mag;

			if(only_info) return format("");
			item_tester_hook = item_tester_hook_jewel;

			q = "どの宝石から魔力を吸収しますか? ";
			s = "あなたは魔力を吸収する宝石を持っていない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0) o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			if(o_ptr->tval != TV_MATERIAL)
			{
				msg_print("ERROR:魔力食いで素材以外が選択された");
				return NULL;
			}
			switch(o_ptr->sval)
			{
				case SV_MATERIAL_GARNET:
				case SV_MATERIAL_AMETHYST:
				case SV_MATERIAL_AQUAMARINE:
				case SV_MATERIAL_MOONSTONE:
				case SV_MATERIAL_PERIDOT:
				case SV_MATERIAL_OPAL:
				case SV_MATERIAL_TOPAZ:
				case SV_MATERIAL_LAPISLAZULI:
					mag = 100 + randint1(100);
					break;
				case SV_MATERIAL_SAPPHIRE:
				case SV_MATERIAL_EMERALD:
					mag = 300 + randint1(300);
					break;
				case SV_MATERIAL_RUBY:
				case SV_MATERIAL_DIAMOND:
					mag = 1000;
					break;
				default:
				msg_print("ERROR:魔力食いで登録されていないアイテムが選択された");
				return NULL;
			}

			msg_print("あなたは宝石を手に取り、魔力を吸収した。");
			p_ptr->csp += mag;

			if(p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			p_ptr->window |= (PW_PLAYER);
			p_ptr->window |= (PW_SPELL);

			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}

		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}






/*:::古道具屋専用技*/
class_power_type class_power_sh_dealer[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"道具箱収納",
		"腰の道具箱にアイテムを入れる。5種類までのアイテムを入れられる。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"道具箱確認",
		"道具箱の中を確認する。行動順を消費しない。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"道具取出し",
		"道具箱からアイテムを出す。"},
	{5,7,40,FALSE,FALSE,A_INT,0,0,"目利き",
		"アイテムを鑑定する。レベル30で完全な鑑定を行える。"},
	{20,30,60,FALSE,FALSE,A_INT,0,0,"掘り出し物感知",
		"このフロアにエゴアイテム・アーティファクト・素材・珍品などが落ちていたらそれを大まかに察知できる。"},
	{40,50,75,FALSE,FALSE,A_INT,0,0,"真・目利き",
		"近くに落ちているアイテムを全て鑑定し、高級なアイテムや特別な装備品をレポートする。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_sh_dealer(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //見るだけなので行動順消費なし

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if(plev < 30)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}

		}
		break;

	case 4:
		{
			if(only_info) return format("");
			msg_print("あなたは周囲を丹念に調べ始めた・・");
			search_specific_object(4);
		}
		break;

	case 5:
		{
			int i;
			int count_ego = 0;
			int count_art = 0;
			int count_valuable = 0;
			bool no_use = TRUE;
			int value_thre = 100000;

			int rad = 4;
			if(plev > 44) rad = 5;
			if(only_info) return format("範囲:%d",rad);
			msg_print("あなたの眼がキラリと光った！");
			for (i = 1; i < o_max; i++)
			{
				object_type *o_ptr = &o_list[i];
				if (!o_ptr->k_idx) continue;
				if (o_ptr->held_m_idx) continue;
				if(object_is_known(o_ptr) && object_value_real(o_ptr) < value_thre) continue;
				if(distance(py,px,o_ptr->iy,o_ptr->ix) > rad || !los(py,px,o_ptr->iy,o_ptr->ix)) continue;
				identify_item(o_ptr);
				no_use = FALSE;
				if(object_is_ego(o_ptr)) count_ego++;
				else if(object_is_artifact(o_ptr)) count_art++;
				else if(object_value_real(o_ptr) >= value_thre) count_valuable++;
			}

			if(no_use)	msg_print("・・しかし目新しいものはなかった。");

			if(count_art)
			{
				msg_format("これはまさしく伝説の品だ！");
				if(count_art > 1) msg_format("しかも%dつもある！",count_art);
			}
			if(count_ego)
			{
				if(count_art) msg_format("さらに、");
				if(count_ego > 1) msg_format("高級な品が%dつほどあるようだ。",count_ego);
				else msg_format("高級な品があるようだ。");
			}
			if(count_valuable)
			{
				if(count_art || count_ego) msg_format("他にも、");
				if(count_valuable > 1) msg_format("極めて貴重な品が%dつほどあるようだ。",count_valuable);
				else msg_format("極めて貴重な品があるようだ。");
			}

			fix_floor_item_list(py,px);

		}
		break;


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::守護者用特技*/
class_power_type class_power_paladin[] =
{
	{5,5,15,FALSE,FALSE,A_DEX,0,0,"応急手当",
		"毒と切り傷を治療し、HPをある程度回復する。"},
	{10,10,36,FALSE,FALSE,A_INT,0,0,"索敵",
		"周囲のモンスターを感知する。"},
	{17,18,35,FALSE,FALSE,A_CHR,0,0,"説得",
		"敵を説得して配下にする。混沌勢力の敵、通常の精神を持たない敵には効果がない。"},
	{24,25,40,FALSE,TRUE,A_WIS,0,0,"加護",
		"一定時間祝福を得る。レベルが上昇すると耐火、耐冷、耐邪悪結界も付加される。"},
	{30,30,60,FALSE,TRUE,A_WIS,0,0,"ホーリーランス",
		"破邪属性のビームを放つ。"},
	{34,24,55,FALSE,FALSE,A_INT,0,0,"治療",
		"隣接した生命のあるモンスターのHPを10～15%回復させる。"},
	{40,40,70,FALSE,TRUE,A_INT,0,0,"結界",
		"自分のいる場所に、モンスターが攻撃して来たり召喚されたりしにくくなるルーンを描く。"},
	{47,80,75,FALSE,TRUE,A_WIS,0,0,"防壁",
		"一定時間ACと魔法防御が上昇し、さらにあなたが受けるほぼ全てのダメージを2/3にする。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_paladin(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int dice = 3 + plev / 5;
			int sides = 6;
			if(only_info) return format("回復:%dd%d",dice,sides);
			set_cut((p_ptr->cut / 2) - 50);
			set_poisoned(0);
			hp_player(damroll(dice,sides));
			break;
		}
	case 1:
		{
			if(only_info) return "";
			msg_format("辺りの気配に気を配った・・");
			detect_monsters_normal(DETECT_RAD_DEFAULT);
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 2: //説得
		{
			int dam = plev * 2 / 3;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			if(buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM)) msg_format("あなたは武器を下ろし、話し掛けた・・");
			else msg_format("あなたは構えを解き、話し掛けた・・");
			fire_ball_hide(GF_CHARM_PALADIN,dir,dam,0);
			break;

		}
	case 3: //加護
		{
			int base = p_ptr->lev/2;
			int sides = p_ptr->lev/2;
			int v;
			if(only_info) return format("期間:%d+1d%d",base,sides);
			msg_print("精神を集中した・・");
			v = damroll(base,sides);

			set_blessed(v, FALSE);
			if(plev > 29) set_oppose_fire(v, FALSE);
			if(plev > 34) set_oppose_cold(v, FALSE);
			if(plev > 39) set_protevil(v, FALSE);

			break;
		}

	case 4: //ホーリーランス
		{
			int dam = plev * 3;
			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print("光の槍を生み出し、投げつけた！");
			fire_beam(GF_HOLY_FIRE,dir, dam);
			break;
		}

	case 5: //治療
		{
			int y, x;
			int dist;
			int damage;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int heal;
			char m_name[80];

			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if(!cave[y][x].m_idx || !m_list[cave[y][x].m_idx].ml)
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			m_ptr = &m_list[cave[y][x].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			heal = m_ptr->maxhp * (10+randint0(6)) / 100;
			if(heal < 1) heal = 1;
			if(monster_living(r_ptr))
			{
				m_ptr->hp += heal;
				if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				set_monster_stunned(cave[y][x].m_idx,0);
				msg_format("%^sの傷を治療した。", m_name);
				p_ptr->redraw |= (PR_HEALTH);
			}
			else
				msg_format("%^sの傷を治療することはできなかった。", m_name);
			break;
		}
	case 6: //結界
		{
			if(only_info) return "";
			msg_print("あなたは足元に紋様を描き始めた・・");
			warding_glyph();
			break;
		}
	case 7: //防壁
		{
			int base = p_ptr->lev/3;
			if(only_info) return format("期間:%d+1d%d",base,base);
			msg_print("防壁を張った！");
			set_magicdef(randint1(base) + base, FALSE);

			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}









/*:::幽々子用特技*/
class_power_type class_power_yuyuko[] =
{
	{6,7,20,FALSE,FALSE,A_CHR,0,2,"誘霊の甘蜜",
		"指定したアンデッドを高確率で魅了して配下にする。"},
	{11,12,30,FALSE,TRUE,A_CHR,0,7,"ゴーストバタフライ",
		"自分を中心とする地獄属性の巨大なボールを放つ。レベルが上がると効果範囲が広がる。"},
	{17,30,35,FALSE,TRUE,A_CHR,0,5,"ゴーストスポット",
		"指定したターゲットの周辺に大量の幽霊系モンスターを呼び出す。"},
	{20,24,40,FALSE,TRUE,A_CHR,0,3,"ギャストリドリーム",
		"隣接した敵一体に対し、強烈な精神攻撃を仕掛ける。ユニークモンスター・デーモン・アンデッドには効きづらく狂気をもたらす敵には効かない。"},
	{23,27,50,FALSE,TRUE,A_CHR,0,0,"无寿国への約束手形",
		"敵一体を高確率で現在のフロアから追い払う。追い払った敵のレベルにより少しダメージを受ける。ユニークモンスターには効かない。"},
	{28,24,55,FALSE,TRUE,A_CHR,0,5,"鳳蝶紋の死槍",
		"地獄属性の強力なビームを放つ。"},
	{34,50,65,FALSE,TRUE,A_CHR,0,0,"華胥の永眠",
		"周囲の敵をフロアから消し去る。抵抗されることもあり、ユニークモンスターには効かない。追い払った敵のレベルにより少しダメージを受ける。"},

	{40,60,75,FALSE,TRUE,A_CHR,0,0,"桜吹雪地獄", //v1.1.76
		"自分を中心に無属性の巨大なボールを発生させる。" },

	{43,64,80,FALSE,TRUE,A_CHR,0,0,"死出の誘蛾灯",
		"短時間、視界内の敵の魔法使用を確率で妨害する。ユニークモンスターや高レベルな敵には効きにくい。"},

	{48,100,75,FALSE,TRUE,A_CHR,0,0,"西行寺無余涅槃",
		"技使用後、全ての耐性を獲得し、全ての攻撃のダメージを半減し、敵からの隣接攻撃に対し地獄属性の強力な反撃を行い、毎ターン視界内に様々な属性の強力な攻撃を行う。ターン経過時にMPを50消費する。使用後に待機以外の行動をすると効果が切れる。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


cptr do_cmd_class_power_aux_yuyuko(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0:
		{
			int power = 50 + plev * 2 + chr_adj*5;
			if(only_info) return format("効力:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			control_one_undead(dir, power);
		}
		break;

	case 1: //ゴーストバタフライ
		{
			int rad = 3 + plev / 10;
			damage = 50 + plev*4 + chr_adj * 5;
			if(only_info) return format("損傷:最大%d 範囲:%d",damage/2,rad);
			msg_print("妖しく光る蝶たちが舞い踊った。");
			project(0, rad, py, px, damage, GF_NETHER, PROJECT_KILL, -1);

			break;
		}
	case 2: //ゴーストスポット
		{
			int num = 5 + plev / 10;
			int i;
			bool flag = FALSE;

			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, target_row, target_col, plev, SUMMON_GHOST, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;
			}
			if(flag) msg_format("幽霊を呼んだ。");
			else msg_format("幽霊は現れなかった・・");

		}
		break;


	case 3: //ギャストリドリーム 耐性とか抵抗処理とか見直す？
		{
			int x,y;
			dice = 1;
			sides = chr_adj * 20 + plev*3 +50;
			if(sides < 200) sides = 200;
			if(only_info) return format("損傷:1d%d",sides);

			if (!get_rep_dir2(&dir)) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if(!in_bounds(y,x)) return NULL;

			if (cave[y][x].m_idx)
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				if(!use_itemcard) msg_format("あなたは%sに向けて指をくるくる回した。",m_name);
				project(0, 0, y, x, damroll(dice,sides) , GF_COSMIC_HORROR, flg, -1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;

		}

	case 4://无寿国への約束手形
		{
			int power = 50 + plev * 4 + chr_adj*5;
			if(only_info) return format("効力:%d",power);
			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_hide(GF_GENOCIDE, dir, power, 0);

		}
		break;
	case 5: //鳳蝶紋の死槍
		{
			int num = 2;
			dice = 10 + p_ptr->lev / 5;
			sides = 5 + chr_adj / 2;

			if(dice < 15) dice = 15;

			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な扇から二条の光線が放たれた！");
			for (i = 0; i < num; i++) if(dir != 5 || target_okay()) fire_beam(GF_NETHER, dir, damroll(dice, sides));
			break;
		}
	case 6://華胥の永眠
		{
			int power = 50 + plev * 2 + chr_adj*5;
			if(power < 200) power = 200;

			if(only_info) return format("効力:%d",power);
			msg_format("蝶たちが周囲へ散った・・");
			mass_genocide(power, TRUE);
		}
		break;

	case 7: //桜吹雪地獄 v1.1.76
		{

		int rad = 7;
		base = plev * 10 + chr_adj * 20;
		if (only_info) return format("損傷:～%d", base / 2);

		msg_format("嵐のような桜吹雪が巻き起こった。");
		project(0, rad, py, px, base, GF_DISP_ALL, PROJECT_KILL | PROJECT_JUMP, -1);

		}
		break;

	case 8://死出の誘蛾灯 v1.1.76 仕様変更
	{
		int time;
		int base = plev / 8;

		if (only_info) return format("期間:%d+1d%d", base, base);
		time = base + randint1(base);
		set_tim_general(time, FALSE, 0, 0);

	}
	break;


/*
	case 8://死出の誘蛾灯 敵にNICEフラグを付与する
		{
			int j;
			if(only_info) return format("");

			msg_format("妖しい光が放たれた・・");
			for (j = 1; j < m_max; j++)
			{
				int tx,ty;
				int chance;
				char m_name[80];
				monster_type *m_ptr;
				monster_race *r_ptr;

				m_ptr = &m_list[j];
				if(!m_ptr->r_idx) continue;
				if(!los(py,px,m_ptr->fy,m_ptr->fx)) continue;
				if(is_pet(m_ptr)) continue;
				monster_desc(m_name, m_ptr, 0);
				r_ptr = &r_info[m_ptr->r_idx];
				chance = plev * 3 + chr_adj * 5;
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) chance /= 2;

				if(r_ptr->level < randint1(chance))
				{
					msg_format("%sは光に誘われた。",m_name);
					m_ptr->mflag |= (MFLAG_SPECIAL);
				}
				else if(!(m_ptr->mflag & MFLAG_SPECIAL)) msg_format("%sは惑わされなかった。",m_name);
			}
			new_class_power_change_energy_need = 25;
		}
		break;
		*/


	//西行寺無余涅槃 v1.1.76で仕様変更
	//夢想の型+毎ターン視界内攻撃
	case 9:
	{
		if (only_info) return format("");
		set_action(ACTION_KATA);
		p_ptr->special_defense |= KATA_MUSOU;
		p_ptr->update |= (PU_BONUS);
		p_ptr->update |= (PU_MONSTERS);
		msg_format("あなたの背後に巨大な扇が開いた...");
		yuyuko_nehan();

	}
	break;
	/*
	case 9:
		{
			base = (plev * 4 + chr_adj * 10) * p_ptr->csp / 100;

			if(only_info) return format("損傷:%d",base);
			msg_format("あなたは全ての力を解き放った！");
			project_hack2(GF_DISP_ALL,0,0,base);
			new_class_power_change_energy_need = 200;
			p_ptr->csp = 1;
			break;
		}
		*/

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}












/*:::騎兵用特技*/
class_power_type class_power_cavalry[] =
{
	{10,0,25,FALSE,FALSE,A_STR,0,0,"荒馬馴らし",
		"敵モンスターに飛び乗って無理矢理手懐け、配下にする。乗馬不可能なモンスターには無効。敵HPが減っているほど効きやすい。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_cavalry(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int rlev;

			if(only_info) return format("");
			if (p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("今は乗馬中だ。");
#else
				msg_print("You ARE riding.");
#endif
				return NULL;
			}
			if (!do_riding(TRUE)) return "";
			m_ptr = &m_list[p_ptr->riding];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%sに乗った。",m_name);
#else
			msg_format("You ride on %s.",m_name);
#endif
			if (is_pet(m_ptr)) break;
			rlev = r_ptr->level;
			if (r_ptr->flags1 & RF1_UNIQUE) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(ref_skill_exp(SKILL_RIDING) / 120 + p_ptr->lev * 2 / 3) > rlev)
				&& one_in_(2) && !p_ptr->inside_arena && !p_ptr->inside_battle
			    && !(r_ptr->flags7 & (RF7_GUARDIAN)) && !(r_ptr->flags1 & (RF1_QUESTOR))
//			    && (rlev < p_ptr->lev * 3 / 2 + randint0(p_ptr->lev / 5)))
			    && (m_ptr->hp < (ref_skill_exp(SKILL_RIDING) / 100 * p_ptr->lev) ))
			{
#ifdef JP
				msg_format("%sを手なずけた。",m_name);
#else
				msg_format("You tame %s.",m_name);
#endif
				if(rlev > randint1(plev)) set_deity_bias(DBIAS_WARLIKE, -2);
				set_pet(m_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%sに振り落とされた！",m_name);
#else
				msg_format("You have thrown off by %s.",m_name);
#endif
				rakuba(1,TRUE);

				/* Paranoia */
				/* 落馬処理に失敗してもとにかく乗馬解除 */
				p_ptr->riding = 0;
			}
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::剣術家用特技*/
class_power_type class_power_samurai[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"気合い溜め",
		"一時的にMPを大幅に増加させる。"},
	{25,0,0,FALSE,FALSE,A_DEX,0,0,"型を構える",
		"特殊な構えをとって戦う。耐性やステータスなどが変化する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_samurai(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			int max_csp = MAX(p_ptr->msp*4, p_ptr->lev*5+5);
			if (max_csp > 255) max_csp = 255;
			if(only_info) return format("最大:%d",max_csp);

			if (total_friends)
			{
				msg_print("今は配下のことに集中していないと。");
				return NULL;
			}
			if (p_ptr->special_defense & KATA_MASK)
			{
				msg_print("今は構えに集中している。");
				return NULL;
			}
			msg_print("精神を集中して気合いを溜めた。");

			p_ptr->csp += p_ptr->msp / 2;
			if (p_ptr->csp >= max_csp)
			{
				p_ptr->csp = max_csp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);

			break;
		}
	case 1:
		{
			if(only_info) return format("");
			if (!choose_samurai_kata()) return FALSE;
			p_ptr->update |= (PU_BONUS);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::メディスン用特技*/
class_power_type class_power_medi[] =
{
	{4,10,20,FALSE,TRUE,A_DEX,0,0,"毒薬生成",
		"毒の薬を作る。花畑か毒の沼でないと使えない。"},
	{8,8,30,FALSE,TRUE,A_INT,0,0,"神経の毒",
		"隣接した敵を高確率で眠らせる。睡眠耐性のある敵、毒耐性のある敵、無生物には効かない。ユニークモンスターや力強い敵には効果が薄い。"},
	{12,12,45,TRUE,FALSE,A_CON,0,10,"ポイズンブレス",
		"現在HPの1/3の威力の毒のブレスを吐く。満腹度が減少する。"},
	{16,12,50,FALSE,TRUE,A_INT,0,0,"憂鬱の毒",
		"隣接した敵を高確率で減速させる。毒耐性のある敵、通常の精神を持たない敵、無生物には効かない。ユニークモンスターや力強い敵には効果が薄い。"},
	{20,30,65,FALSE,TRUE,A_CHR,0,0,"譫妄の毒",
		"隣接した敵を高確率で操って配下にする。無生物、毒耐性のある敵、通常の精神を持たない敵、ユニークモンスターには効果がない。"},
	{25,25,45,FALSE,FALSE,A_WIS,0,10,"ガシングガーデン",
		"視界内の全てに対し毒属性の攻撃を放つ。"},
	{32,50,75,FALSE,TRUE,A_DEX,0,0,"猛毒生成",
		"猛毒の薬を作る。猛毒の物質を一つ消費する。"},
	{40,80,70,FALSE,FALSE,A_CHR,0,0,"イントゥデリリウム",
		"視界内の全てに対し強力な精神攻撃属性の攻撃を行い朦朧・混乱・減速させる。精神の希薄な敵、狂気をもたらす敵、ユニークモンスターには効果が薄い。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::メディスン猛毒精製用*/
bool item_tester_hook_make_venom(object_type *o_ptr)
{
	if(o_ptr->tval == TV_MATERIAL && (o_ptr->sval == SV_MATERIAL_ARSENIC || o_ptr->sval == SV_MATERIAL_MERCURY || o_ptr->sval == SV_MATERIAL_GELSEMIUM)) return TRUE;
	if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI && !object_is_artifact(o_ptr)) return TRUE;
	if(object_is_melee_weapon(o_ptr) && o_ptr->name2 == EGO_WEAPON_BRANDPOIS) return TRUE;
	return FALSE;
}


cptr do_cmd_class_power_aux_medi(int num, bool only_info)
{
	int dir, i, dam;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return format("");
			if(cave[py][px].feat != 93 && !cave_have_flag_bold(py,px,FF_POISON_PUDDLE)) //花地形 v1.1.85 毒沼も追加
			{
				msg_format("ここには毒になりそうなものが見当たらない。");
				break;
			}
			msg_format("「コンパロコンパロ、毒よ集まれ～」");
			object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POISON ));
			drop_near(q_ptr, -1, py, px);

		}
		break;
	case 1:
	case 3:
	case 4:
		{

			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int power;
			if(num == 4)
				power = plev * 3 + 30;
			else
				power = plev * 4 + 50;

			if(only_info) return  format("効力:%d",power);

			if (num == 4 && p_ptr->inside_arena)
			{
				msg_print("今その特技は使えない。");
				return NULL;
			}


			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(num == 1)
				{
					msg_format("%sに向かって神経毒を振りまいた！",m_name);
					if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 3;
					if((r_ptr->flags2 & RF2_POWERFUL)) power /= 2;

					if((r_ptr->flagsr & RFR_IM_POIS) || (r_ptr->flags3 & RF3_NO_SLEEP) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("しかし効果がなかった。");
						if (r_ptr->flags3 & RF3_NO_SLEEP) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr)) msg_format("%sは眠らないようだ。",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%sは毒に抵抗した！",m_name);
					else
					{
						msg_format("%sは眠り込んだ。",m_name);
						(void)set_monster_csleep(cave[y][x].m_idx, power);
					}
				}
				else if(num == 3)
				{
					msg_format("%sに向かって憂鬱の毒を振りまいた！",m_name);
					if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 3;
					if((r_ptr->flags2 & RF2_POWERFUL)) power /= 2;

					if((r_ptr->flagsr & RFR_IM_POIS) || (r_ptr->flags2 & RF2_EMPTY_MIND) || (r_ptr->flags2 & RF2_WEIRD_MIND) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("しかし効果がなかった。");
						if (r_ptr->flags3 & RF3_NO_SLEEP) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr)) msg_format("%sは動じないようだ。",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%sは毒に抵抗した！",m_name);
					else
					{
						if (set_monster_slow(cave[y][x].m_idx, MON_SLOW(m_ptr) + 8 + randint1(8))) msg_format("%sの動きが遅くなった。",m_name);
					}
					anger_monster(m_ptr);
				}
				else
				{
					if(is_pet(m_ptr))
					{
						msg_format("%sはすでに配下だ。",m_name);
						break;
					}
					msg_format("%sに向かって譫妄の毒を振りまいた！",m_name);
					if((r_ptr->flags2 & RF2_POWERFUL)) power = power * 2 / 3;

					if((r_ptr->flagsr & RFR_IM_POIS) || (r_ptr->flags2 & RF2_EMPTY_MIND) || (r_ptr->flags2 & RF2_WEIRD_MIND) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						msg_format("しかし効果がなかった。");
						if (r_ptr->flags3 & RF3_NO_SLEEP) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
						if (r_ptr->flagsr & RFR_IM_POIS) r_ptr->r_flagsr |= (RFR_IM_POIS);
					}
					else if(!monster_living(r_ptr) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) )
						msg_format("%sは動じないようだ。",m_name);
					else if(randint1(power) < r_ptr->level) msg_format("%sは毒に抵抗した！",m_name);
					else
					{
						msg_format("%sはあなたに従った。",m_name);
						set_pet(m_ptr);
					}
					(void)set_monster_csleep(cave[y][x].m_idx, 0);
					anger_monster(m_ptr);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;
	case 2:
		{
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 1600) dam=1600;

			if(only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return FALSE;

			if(use_itemcard)
				msg_print("カードから毒ガスが噴き出した。");
			else
				msg_print("あなたは毒のブレスを吐いた。");

			fire_ball(GF_POIS, dir, dam, -2);
			if(!use_itemcard) (void)set_food(p_ptr->food -1500);
			break;
		}


	case 5:
		{
			int dam = 10 + plev * 3 + chr_adj * 8;
			if(dam < 100) dam = 100;
			if(only_info) return format("損傷:%d",dam);
			msg_print("部屋に毒ガスが充満した！");
			project_hack2(GF_POIS, 0,0,dam);
		}
		break;
	case 6:
		{
			int         item;
			object_type forge, *q_ptr = &forge;
			cptr        q, s;
			if(only_info) return format("");

			item_tester_hook = item_tester_hook_make_venom;
			q = "どれから毒を抽出しますか? ";
			s = "毒を抽出できそうなものが見当たらない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
			msg_format("「コンパロコンパロ、毒よ集まれ～」");
			object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POISON2 ));
			drop_near(q_ptr, -1, py, px);

			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
		}
		break;
	case 7:
		{
			int dam = plev * 4 + chr_adj * 15;
			if(only_info) return format("損傷:%d",dam);
			msg_print("狂乱の毒が充満した！");
			project_hack2(GF_REDEYE, 0,0,dam);
		}
		break;


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::星専用技*/
class_power_type class_power_shou[] =
{
	{5,9,20,FALSE,TRUE,A_WIS,0,2,"レーザー",
		"ターゲット周辺に閃光属性のビームを数発放つ。「毘沙門天の宝塔」を所持していないと使えない。"},
	{14,20,40,TRUE,FALSE,A_STR,0,5,"ハングリータイガー",
		"数グリッド分一直線に突進し、小さな敵は吹き飛ばす。レベルが上がると射程が伸びる。"},
	{21,24,50,FALSE,TRUE,A_WIS,0,2,"レイディアントトレジャー",
		"宝石を生み出す無属性レーザーを放つ。「毘沙門天の宝塔」を所持していないと使えない。"},
	{26,30,55,FALSE,TRUE,A_CHR,0,0,"黄金の震眩",
		"部屋を明るくし、視界内のモンスターを混乱・朦朧させ、さらにレベル40以降は魅了しようと試みる。「毘沙門天の宝塔」を所持していないと使えない。"},
	{32,30,60,FALSE,TRUE,A_WIS,0,10,"アブソリュートジャスティス",
		"破邪属性の強力なレーザーを放つ。「毘沙門天の宝塔」を所持していないと使えない。"},
	{36,50,70,FALSE,TRUE,A_CHR,0,0,"正義の威光",
		"一時的に祝福・聖なるオーラ・対邪悪結界を得る。「毘沙門天の宝塔」を所持していないと使えない。"},
	{40,75,75,FALSE,TRUE,A_WIS,0,0,"レイディアントトレジャーガン",
		"宝石を生み出す強力な無属性大型レーザーを放つ。「毘沙門天の宝塔」を所持していないと使えない。"},
	{47,160,80,FALSE,TRUE,A_WIS,0,0,"浄化の魔",
		"視界内全てに強力な破邪属性攻撃を行う。「毘沙門天の宝塔」を所持していないと使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

/*:::星専用特技*/
cptr do_cmd_class_power_aux_shou(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int wis_adj = adj_general[p_ptr->stat_ind[A_WIS]];

	switch(num)
	{
	case 0: //へにょりレーザー
		{
			int num = 3 + p_ptr->lev / 15;
			dice = 4 + p_ptr->lev / 20;
			sides = 5 + chr_adj / 10;
			if(only_info) return format("損傷:%dd%d * %d",dice,sides, num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("宝塔から光線が放たれた！");
			fire_blast(GF_LITE, dir, dice, sides, num, 3,(PROJECT_BEAM | PROJECT_KILL));
			break;
		}
	case 1: //ハングリータイガー
		{
			int len = 2 + p_ptr->lev / 8;
			damage = plev + adj_general[p_ptr->stat_ind[A_STR]] *5;
			if(damage < 50) damage = 50;

			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack3(len,"あなたは激しく突進した！",damage)) return NULL;
			break;
		}
	case 2: //レイディアントトレジャー
		{
			damage = p_ptr->lev * 2 + chr_adj * 3 + wis_adj * 2;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("宝塔が眩く光った！");
			fire_beam(GF_HOUTOU,dir,damage);

			break;
		}
	case 3: //v1.1.30追加　黄金の震眩
		{
			int power = p_ptr->lev * 4;
			if (only_info) return format("効力:%d",power);
			msg_format("宝塔が黄金の輝きを放った...");
			lite_room(py, px);
			stun_monsters(power);
			confuse_monsters(power);
			if(plev > 39)
				charm_monsters(power);
		}
		break;

	case 4://アブソリュートジャスティス
		{
			dice = p_ptr->lev / 3;
			sides = 5 + wis_adj / 5 + chr_adj / 5 ;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("宝塔から浄化の光が放たれた！");
			fire_spark(GF_HOLY_FIRE,dir, damroll(dice,sides),1);

			break;
		}

	case 5: //v1.1.30追加　正義の威光
		{
			int t;
			base = 15;
			if(only_info) return format("期間:%d+1d%d",base,base);
			t = base + randint1(base);
			msg_format("毘沙門天の加護を感じる！");
			set_tim_sh_holy(t,FALSE);
			set_protevil(t,FALSE);
			set_blessed(t,FALSE);
			break;
		}


	case 6: //レイディアントトレジャーガン
		{
			damage = p_ptr->lev * 4 + chr_adj * 5 + wis_adj * 5;
			if(damage < 300) damage = 300;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("至宝の輝きが溢れ出した！");
			fire_spark(GF_HOUTOU,dir,damage,2);

			break;
		}
	case 7: //浄化の魔
		{
			dice = 7 + p_ptr->lev / 2;
			sides = p_ptr->lev / 5 + chr_adj / 2 + wis_adj / 2;
			if(sides < 1) sides = 1;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("浄化の光が周囲を焼き尽くした！");
			project_hack2(GF_HOLY_FIRE,dice,sides,0);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::フランドール用特技*/
class_power_type class_power_flan[] =
{

	{ 7,10,25,FALSE,FALSE,A_INT,0,0,"殺気感知",
	"周囲の精神をもつモンスターを感知する。" },

	{16,12,30,FALSE,TRUE,A_INT,0,6,"スターボウブレイク",
		"自分の周辺のランダムな場所に複数回の隕石属性攻撃を行う。"},
	{24,18,50,FALSE,FALSE,A_DEX,0,0,"きゅっとしてドカーン",
		"ターゲットとその周辺3+1d3グリッドを*破壊*する。ターゲットは明示的に指定しなければならない。(25-レベル/2)%の確率で想定外の大破壊が起こる。地上やクエストダンジョンでは使えない。"},
	{30,32,60,FALSE,TRUE,A_INT,0,8,"カタディオプトリック",
		"特殊な軌道のビームを5発放つ。"},
	{36,64,70,FALSE,TRUE,A_DEX,0,0,"フォーオブアカインド",
		"分身を三体出現させる。ただし分身は命令に関わらずあなたを範囲攻撃に巻き込む。"},
	{42,72,70,FALSE,TRUE,A_STR,0,0,"レーヴァテイン",
		"自分の周囲を地獄の劫火属性で攻撃する。威力は武器攻撃力に依存する。"},
	{48,99,75,FALSE,TRUE,A_CHR,0,0,"495年の波紋",
		"自分の周辺のランダムな場所に強力な分解属性ボールを複数発生させる。体力が少ないほど威力が上がる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::フランドール特技*/
cptr do_cmd_class_power_aux_flan(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0://v1.1.77 殺気感知
	{
		int rad = DETECT_RAD_DEFAULT;
		if (only_info) return format("範囲:%d", rad);
		detect_monsters_mind(rad);
		break;
	}
	case 1: //スターボウブレイク
		{
			int dam = plev*2 + chr_adj + 20;
			if(only_info) return format("損傷:%d*不定",dam);
			cast_meteor(dam, 2,GF_METEOR);
		}
		break;
	case 2://きゅっとしてどかーん
		{
			if(only_info) return format("");
			if (!get_aim_dir(&dir)) return NULL;
			if(dir !=5 || !los(py,px,target_row,target_col))
			{
				msg_format("視界内の一点を指定しないといけない。");
				return NULL;
			}

			if(!use_itemcard)
				msg_format("あなたは標的を睨んで拳を握った・・");

			if( (25-plev/2) > randint0(100))
			{
				if (destroy_area(py, px, 15 + p_ptr->lev + randint0(11), FALSE,FALSE,FALSE))
				{
					msg_print("うっかりダンジョンそのものを壊してしまった！");
				}
				else msg_print("地面が大きく揺れた。");
			}
			else
			{
				if(destroy_area(target_row, target_col, 3+randint1(3),FALSE,FALSE,TRUE))
				{
					msg_print("爆発が起こった！");
				}
				else msg_print("ここでは使えない。");

			}

		}
		break;

	case 3: //カタディオプトリック
		{
			int dam = plev * 2 + chr_adj * 2 ;
			int table[] = {1,2,3,6,9,8,7,4,1,2,3,6,9,8};
			int ty,tx,i,p;
			int flg = PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_OPT;

			if(only_info) return format("損傷:%d*5",dam);
			msg_format("あなたは光弾を放った！");

			p = randint0(8);
			for(i=0;i<5;i++)
			{
				dir = table[p+i];
				tx = px + 99 * ddx[dir];
				ty = py + 99 * ddy[dir];

				project(0, 0, ty, tx, dam, GF_LITE, flg, -1);
			}
		}
		break;
	case 4: //フォーオブアカインド
		{
			int max = 3;
			bool flag = FALSE;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("最大:%d",max);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_F_SCARLET_4) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max)
			{
				msg_format("これ以上呼び出せない。",num);
				return NULL;
			}
			for(i=0;i<(max-cnt);i++) if(summon_named_creature(0, py, px, MON_F_SCARLET_4, PM_EPHEMERA)) flag = TRUE;

			if(flag && p_ptr->pclass != CLASS_FLAN)
				msg_print("宝石のような羽を持つ吸血鬼の幻影が現れた・・");
			else if(flag)
				msg_print("何処からともなくあなたの分身が現れた。");
			else
				msg_print("何も現れなかった。");
		}
		break;
	case 5: //レーヴァテイン
		{

			int dam =  calc_weapon_dam(0) + calc_weapon_dam(1);
			int rad = 4;
			dam = dam * (250 + chr_adj * 5 ) / 100;

			if(use_itemcard) dam = 500 + plev*20;

			if(only_info) return format("損傷:最大%d",dam/2);
			msg_print("災厄の炎が周囲を焼き払った！");
			project(0, rad, py, px, dam, GF_HELL_FIRE, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}
		break;
	case 6: //495年の波紋
		{
			int dam = plev * 3 + chr_adj * 5;
			int rad = 2;
			int mult = 50 + 450 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;
			dam = dam * mult / 100;
			rad = rad * mult / 100;
			if(rad > 7) rad = 7;
			if(rad < 2) rad = 2;
			if(only_info) return format("損傷基本値:%d",dam);

			if (!cast_wrath_of_the_god(dam, rad, FALSE)) return NULL;
			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::エンジニア専用技*/
class_power_type class_power_engineer[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械格納",
		"バックパックに機械を格納する。欄の数はレベルアップで上昇する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械確認",
		"バックパックの機械を確認したり説明を見る。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"装備解除",
		"バックパックから機械を外す。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"機械使用",
		"バックパックに格納した機械を使用する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"弾薬補充",
		"バックパックに格納した機械に弾薬などを補充し使用回数を回復させる。"},

	{30,20,60,FALSE,FALSE,A_DEX,0,0,"一斉射撃",
		"バックパックに装備された射撃装備全てを一回ずつ発射する。(1+発射回数*0.25)倍の行動時間がかかる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_engineer(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			(void) use_machine(0);
			return NULL; //見るだけなので行動順消費なし

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			if( !use_machine(2)) return NULL;

			break;
		}
	case 4: //弾薬補充
		{
			if(only_info) return format("");
			if( !use_machine(1)) return NULL;
			break;
		}
	case 5: //一斉射撃
		{
			if(only_info) return format("");
			if(!engineer_triggerhappy()) return NULL;
			break;
		}

	}

	return "";
}


/*:::ミスティア用特技*/
class_power_type class_power_mystia[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"歌を止める",
		"今歌っている歌があれば止める。行動力を消費しない。"},
	{3,3,20,FALSE,TRUE,A_CHR,0,0,"梟の夜鳴声",
		"あなたが今いる部屋を暗くする。"},
	{5,8,30,FALSE,TRUE,A_CHR,0,0,"夜雀の歌",
		"周囲の人間を鳥目にする歌を歌う。歌っている間はMPを消費し続ける。鳥目になった敵は高確率であなたの位置を見失い無防備にあなたの攻撃を受けるが、部屋が明るかったり敵が視覚に頼らない存在だと効果が薄い。"},
	{10,10,30,FALSE,TRUE,A_DEX,0,0,"毒蛾の鱗粉",
		"周辺の敵を混乱させる。"},
	{18,32,50,FALSE,FALSE,A_CHR,0,0,"リトルバタリオン",
		"配下の鳥を呼び寄せる。"},
	{25,30,50,FALSE,FALSE,A_DEX,50,0,"イルスタードダイブ",
		"離れた敵に突撃する。装備が重いと失敗しやすい。"},
	{30,24,60,FALSE,TRUE,A_CHR,0,0,"ミステリアスソング",
		"周囲の敵に精神攻撃を行い朦朧・混乱・減速させる歌を歌う。歌っている間はMPを消費し続ける。ユニークモンスターや通常の精神を持たない敵には効果が薄い。"},
	{35,30,65,FALSE,TRUE,A_CHR,0,0,"ヒューマンケージ",
		"周囲の人間の移動を封じる歌を歌う。また歌いながら人間に攻撃すると人間スレイを得る。歌っている間はMPを消費し続ける。" },
	{40,36,75,FALSE,TRUE,A_CHR,0,0,"真夜中のコーラスマスター",
		"周囲の敵を魅了し配下にする歌を歌う。歌っている間はMPを消費し続ける。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
/*:::ミスティア特技 p_ptr->magic_num1[0],[1],p_ptr->magic_num2[0]が歌に使われる*/
cptr do_cmd_class_power_aux_mystia(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

		//歌を止める　行動順消費しない
	case 0:
		{
			if(only_info) return "";
			stop_singing();
			return NULL;

		}
		break;
	case 1: //梟の夜鳴声
		{
			if(only_info) return "";
			msg_format("あなたが一声鳴くと、部屋の明かりが消えた。");
			unlite_room(py,px);
			break;
		}
	case 2://夜雀の歌
		{
			if(only_info) return "";
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_YAZYAKU, SPELL_CAST);

			break;
		}
	case 3://毒蛾の鱗粉
		{
			int power = 50 + plev * 4;
			int rad = 2 + plev / 10;

			if(only_info) return format("効果:%d 範囲:%d",power/2,rad);
			msg_format("刺激性の粉を振り撒いた！");
			project(0, rad, py, px, power, GF_OLD_CONF, PROJECT_KILL , -1);

		}
		break;
	case 4://リトルバタリオン
		{
			int i;
			int num = 2 + randint0(plev / 10);
			bool flag = FALSE;
			if(only_info) return format("召喚レベル:%d",plev);
			for(i=0;i<num;i++)
			{
				if(summon_specific(-1, py, px, plev, SUMMON_BIRD, (PM_FORCE_PET))) flag = TRUE;
			}
			if(use_itemcard)
			{
				if(flag) msg_format("鳥たちが現れた！");
				else msg_format("何も現れなかった・・");
			}
			else
			{
				if(flag) msg_format("配下の鳥を呼んだ！");
				else msg_format("配下は現れなかった・・");
			}

		}
		break;
	case 5:
		{
			int len = p_ptr->lev / 7;
			if(only_info) return format("射程:%d",len);
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 6://ミステリアスソング
		{
			int power = plev + chr_adj * 2;
			if(only_info) return  format("効果:%d",power);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_MYSTERIOUS, SPELL_CAST);

			break;
		}
	case 7://ヒューマンケージ
	{
		int range = plev / 3;
		if (only_info) return  format("範囲:%d",range);
		do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_HUMAN_CAGE, SPELL_CAST);

		break;
	}

	case 8://真夜中のコーラスマスター
		{
			int power = chr_adj * 5;
			if(only_info) return  format("効果:1d%d",power);
			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_MYSTIA_CHORUSMASTER, SPELL_CAST);

			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::赤蛮奇専用技*/
class_power_type class_power_banki[] =
{
	{ 5,16,25,FALSE,TRUE,A_DEX,0,0,"フライングヘッド",
		"頭の分身を出現させる。頭は敵を追いかけて体当たりや睨み攻撃をする。"},

	{ 10,10,20,FALSE,FALSE,A_INT,0,0,"散開指令",
		"頭の分身全てが短距離のテレポートを行う。" },
	{ 15,10,20,FALSE,FALSE,A_INT,0,0,"探査指令",
		"頭の分身全てが中-長距離のテレポートを行う。" },
	{ 20,10,20,FALSE,FALSE,A_INT,0,0,"集合指令",
		"頭の分身全てが自分の近くにテレポートしてくる。" },
	{ 25,40,65,FALSE,TRUE,A_DEX,0,0,"マルチプリケイティブヘッド",
		"頭の分身を複数出現させる。頭はその場を動かず無属性のレーザー攻撃をする。"},
	{ 32,32,70,FALSE,TRUE,A_CON,0,0,"ツインロクロヘッド",
		"周囲の頭の分身二体を消滅させ、無属性の強力なビーム攻撃を放つ。この攻撃は他の頭にダメージを与えない。" },
	{ 38,88,70,FALSE,TRUE,A_CHR,0,0,"デュラハンナイト",
		"視界内全てに対し「死の光線」属性の攻撃を複数回行う。攻撃回数は視界内のあなたの首の数によって増える。" },

	{ 44,60,75,FALSE,TRUE,A_DEX,0,0,"ヘルズレイ",
		"指定したターゲット周辺に向けて無属性のビームを数発放つ。頭の分身がいてそのターゲットに視線が通っている場合頭の分身からも同じようにビームを放つ。この攻撃であなたと分身がダメージを受けることはない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};



cptr do_cmd_class_power_aux_banki(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0://フライングヘッド
	case 4://マルチプリケイティブヘッド
		{
			int summon_num;
			int max_num = MIN(9,(p_ptr->max_plv / 5));
			int i,cnt=0;
			int summon_idx;
			bool flg_success = FALSE;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(max_num<1)max_num=1;

			if(only_info) return format("最大召喚可能数:%d",max_num);
			if(num == 0)
			{
				summon_idx = MON_BANKI_HEAD_1;
				summon_num = 1;
			}
			else
			{
				summon_idx = MON_BANKI_HEAD_2;
				summon_num = 1 + randint1(plev / 10);
			}

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_BANKI_HEAD_1) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
				if((m_ptr->r_idx == MON_BANKI_HEAD_2) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("これ以上分身を出せない。");
				return NULL;
			}
			if(cnt + summon_num > max_num) summon_num = max_num - cnt;
			for(i=0;i<summon_num;i++)
			{
				if(summon_named_creature(0, py, px, summon_idx, PM_EPHEMERA)) flg_success = TRUE;
			}

			if(flg_success)	msg_print("頭の分身を出した！");
			else msg_print("頭の分身に失敗した。");

		break;
		}

	case 1: //散開
	case 2: //探査
	case 3: //集合
		{
			int i;
			monster_type *m_ptr;

			if (only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				if (!(m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2)))continue;
				if (teleport_barrier(i)) continue;

				if (num == 1)
				{
					teleport_away(i, 10+randint1(5), 0L);
				}
				else if (num == 2)
				{
					teleport_away(i, 30+randint1(30), 0L);
				}
				else
				{
					teleport_monster_to(i, py, px, 100, TELEPORT_FORCE_NEXT);
				}
			}
		}
		break;

	case 5: //ツインロクロヘッド
		{
			int dir;
			int i;
			int m_idx1 = 0;
			int m_idx2 = 0;
			int damage = plev * 3 + chr_adj * 3;
			if (only_info) return format("損傷:%d", damage);

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				if (!(m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2)))continue;
				if (m_ptr->cdis > 1) continue;
				if(m_idx1) m_idx2 = i;
				else m_idx1 = i;
			}

			if (!m_idx1 || !m_idx2)
			{
				msg_print("近くに頭が二つ以上ないとこの技は使えない。");
				return NULL;
			}

			if (!get_aim_dir(&dir)) return NULL;

			msg_print("あなたの頭は螺旋を描いて飛んだ！");
			delete_monster_idx(m_idx1);
			delete_monster_idx(m_idx2);
			fire_spark(GF_BANKI_BEAM2, dir, damage, 1);

		}
		break;

	case 6:
	{
		int i;
		int dam = plev * 200;
		int cnt=0;
		if (dam > 9999) dam = 9999;
		if (only_info) return format("損傷:%d", dam);

		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!m_ptr->cdis) continue;
			if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
			if ((m_ptr->r_idx == MON_BANKI_HEAD_1) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			if ((m_ptr->r_idx == MON_BANKI_HEAD_2) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
		}


		msg_print("破滅の宣告がダンジョンに響き渡った。");
		for (i = 1 + cnt / 3; i > 0; i--)
		{
			deathray_monsters();
		}

	}
	break;



	case 7://ヘルズレイ
		{
			int damage =  10 + plev/5 + chr_adj;
			int num = 2;
			int dir;
			if(plev > 47) num++;
			if(only_info) return format("損傷:%d*%d",damage,num);
			if (!get_aim_dir(&dir)) return NULL;
			if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
			{
				msg_print("視界内のターゲットを明示的に指定しないといけない。");
				return NULL;
			}
			msg_print("恐ろしい眼光が放たれた！");
			for(;num>0;num--)
			{
				int j;
				fire_beam(GF_BANKI_BEAM2,dir,damage);
				for (j = 1; j < m_max; j++)
				{
					int tx,ty;
					monster_type *m_ptr;
					m_ptr = &m_list[j];
					if(!target_okay()) break;
					if((m_ptr->r_idx != MON_BANKI_HEAD_1) && (m_ptr->r_idx != MON_BANKI_HEAD_2)) continue;
					if(!projectable(m_ptr->fy,m_ptr->fx,target_row,target_col)) continue;
					do
					{
						tx = target_col + randint0(3) - 1;
						ty = target_row + randint0(3) - 1;
					}while(!in_bounds2(ty,tx));

					project(j,0,ty,tx,damage,GF_BANKI_BEAM2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU),-1);
				}
				if(!target_okay()) break;


			}

		}
		break;

	default:
		if (only_info) return "";
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::文用特技*/
class_power_type class_power_aya[] =
{
	{12,10,20,FALSE,TRUE,A_DEX,0,4,"鎌鼬ベーリング",
		"隣接している敵に対し風の刃で攻撃する。ACの高い敵には当たりにくい。レベル30で範囲が広がる。"},
	{20,16,30,FALSE,TRUE,A_STR,0,4,"天狗のマクロバースト",
		"プレーヤー周辺のランダムな位置に竜巻属性の球を連続して放つ。"},
	{27,30,50,FALSE,TRUE,A_STR,0,5,"天狗道の開風",
		"竜巻属性の大型のレーザー攻撃を放つ。"},
	{35,25,60,FALSE,TRUE,A_DEX,50,0,"猿田彦の先導",
		"風を纏って突進する。一直線上に竜巻属性の攻撃を行い敵を吹き飛ばしつつ移動し、壁か吹き飛ばせない敵に当たると止まる。装備が重いと難度が上がる。"},
	{40,45,60,FALSE,TRUE,A_CHR,0,0,"天孫降臨の道しるべ",
		"自分を中心にした巨大な竜巻を発生させる。"},
	{45,80,80,FALSE,TRUE,A_DEX,0,0,"幻想風靡",
		"短時間の間、限界まで加速し攻撃回数と格闘能力が上昇する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_aya(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int dam = 50 + plev * 2 + chr_adj * 5;
			int rad = 1;
			if(plev > 29) rad = 2;

			if(only_info) return format("損傷:%d 範囲:%d",dam/2,rad);
			msg_print("鎌鼬を起こした！");
			project(0, rad, py, px, dam, GF_WINDCUTTER, PROJECT_KILL|PROJECT_GRID , -1);

		}
		break;
	case 1:
		{
			int rad = 2 + plev / 20;
			int damage = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_print("上空から裂風を放った！");
			cast_meteor(damage, rad, GF_TORNADO);

			break;
		}
	case 2:
		{
			int damage = 50 + p_ptr->lev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("竜巻が前方を薙ぎ倒した！");
			fire_spark(GF_TORNADO,dir,damage,2);
			break;
		}
	case 3://レイシャルの「突進」のルーチンを少し改変
		{
			int dir;
			int tx, ty;
			u16b path_g[32];
			int path_n, i;
			bool moved = FALSE;
			int damage = plev * 3;
			project_length = MAX(5,plev / 3);
			if(damage < 100) damage=100;
			if(only_info) return format("損傷:%d 射程:%d",damage/2,project_length);

			if (!get_aim_dir(&dir)) return FALSE;
			tx = px + project_length * ddx[dir];
			ty = py + project_length * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_THRU | PROJECT_KILL);
			project_length = 0;
			if (!path_n) return NULL;
			if (in_bounds(ty, tx)) msg_format("あなたは風を纏って突進した！");
			ty = py;
			tx = px;

			/* Project along the path */
			for (i = 0; i < path_n; i++)
			{
				monster_type *m_ptr;
				monster_race *r_ptr;
				char m_name[80];
				bool stop = FALSE;

				int ny = GRID_Y(path_g[i]);
				int nx = GRID_X(path_g[i]);

				if (cave[ny][nx].m_idx)
				{
					m_ptr = &m_list[cave[ny][nx].m_idx];
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					if(r_ptr->flags2 & RF2_GIGANTIC)
					{
						msg_format("%sに突進を止められた！",m_name);
						(void)project(0, 1, ny, nx, damage, GF_TORNADO, PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID, -1);
						break;
					}
					else
					{
						(void)project(0, 1, ny, nx, damage, GF_TORNADO, PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID, -1);
					}

				}
				if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
				{
					ty = ny;
					tx = nx;
					if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
				}
				else
				{
					msg_print("ここには入れない。");
					break;
				}
			}
		}
		break;
	case 4:
		{
			int dam = 100 + plev * 6 + chr_adj * 20;
			int rad = 4 + plev / 15;

			if(only_info) return format("損傷:%d 範囲:%d",dam/2,rad);
			msg_print("巨大な竜巻を起こした！");
			project(0, rad, py, px, dam, GF_TORNADO, PROJECT_KILL , -1);
		}
		break;
	case 5:
		{
			if(only_info) return format("期間:10ターン");
			set_lightspeed(10,FALSE);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::司書用特技*/
class_power_type class_power_librarian[] =
{
	{40,0,50,FALSE,TRUE,A_DEX,0,0,"高速詠唱",
		"素早く魔法を詠唱する。ただし消費MPが30%増加し、魔法詠唱成功後にキャンセルしても行動順を消費する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_librarian(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			quick_cast = TRUE;
			do_cmd_cast();
			quick_cast = FALSE;
			new_class_power_change_energy_need = 100 - p_ptr->lev;
		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



//p_ptr->magic_num2[0]を「喘息が治った」フラグに使う
/*:::パチュリー専用技*/
class_power_type class_power_patchouli[] =
{
	{ 3,4,15,FALSE,TRUE,A_INT,0,2,"ドヨースピア",
		"ターゲットに複数回の無属性攻撃をする。射程は短い。"},
	{ 15,10,30,FALSE,TRUE,A_INT,0,4,"プリンセスウンディネ",
		"水属性のボルトを連射する。"},
	{ 20,20,35,FALSE,TRUE,A_INT,0,0,"エレメンタルハーベスター",
		"自分の周りに回転する刃を生み出し、近寄った敵に対して反撃する。破片属性オーラ扱いになる。"},
	{ 26,28,45,FALSE,TRUE,A_INT,0,0,"セントエルモピラー",
		"強力なプラズマ属性の球を放つ。"},
	{ 32,45,55,FALSE,TRUE,A_INT,0,0,"ラーヴァクロムレク",
		"指定した位置に火炎属性の巨大なボールを発生させ、地面を溶岩にする。"},
	{ 37,48,50,FALSE,TRUE,A_INT,0,0,"サイレントセレナ",
		"隣接した敵全てに暗黒属性の強力な攻撃を行う。"},

	//v1.1.75
	{ 40,40,80,FALSE,TRUE,A_INT,0,0,"同時詠唱",
		"魔法書による魔法を二連続で使用する。喘息の調子が悪いときには使えない。" },

	{ 45,128,75,FALSE,TRUE,A_INT,0,0,"ロイヤルフレア",
		"自分を中心とした轟音属性のボールを発生させ、さらに視界内全てを核熱属性で攻撃する。"},
	{ 48,96,80,FALSE,TRUE,A_INT,0,0,"賢者の石",
		"強力な元素攻撃を行う「賢者の石」を配下として召喚する。階移動すると消える。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};



cptr do_cmd_class_power_aux_patchouli(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0: //ドヨースピア
		{
			int dist = 3 + plev / 15;
			dice = 8 + plev / 3;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("損傷:%dd%d 射程:%d",dice,sides,dist);
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_MISSILE, dir, damroll(dice, sides),0,"地面から石錐が飛び出した！")) return NULL;
			break;
		}
	case 1: //プリンセスウンディネ
		{
			int num = 3 + plev / 20;
			dice = 3 + plev / 6;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("水の矢を放った！");
			for (i = 0; i < num; i++) fire_bolt(GF_WATER, dir, damroll(dice, sides));
			break;

		}
	case 2://エレメンタルハーベスター
		{
			if(only_info) return format("期間:20+1d20");
			msg_format("回転する刃が現れた。");
			set_dustrobe(20+randint1(20),FALSE);
			break;
		}
	case 3://セントエルモピラー
		{
			int rad = 3;
			if(p_ptr->lev > 39) rad = 4;
			dice = 8 + p_ptr->lev / 4;
			sides = 10 + chr_adj / 3 ;
			base = p_ptr->lev*4;
			if(base < 100) base = 100;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_PLASMA, dir, base + damroll(dice,sides), rad,"火花を散らす光の柱が立ち昇った！")) return NULL;
			break;
		}

	case 4: //ラーヴァクロムレク
		{
			int rad = 3;
			base = p_ptr->lev*5 + chr_adj * 5;
			if(only_info) return format("損傷:%d",base);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_LAVA_FLOW, dir, rad, rad,"溶岩が噴出した！")) return NULL;
			fire_ball_jump(GF_FIRE, dir, base, rad,NULL);
			break;

		}
		break;

	case 5: //サイレントセレナ
		{
			int rad = 1;
			base = p_ptr->lev * 10 + chr_adj * 20;
			if(base < 600) base = 600;
			if(only_info) return format("損傷:%d",base / 2);
			msg_format("月の光が降り注いだ。");
			project(0, rad, py, px, base, GF_DARK, PROJECT_KILL , -1);
			break;
		}

	case 6: //v1.1.75 同時詠唱
		{
			if (only_info) return format("");

			if (!can_do_cmd_cast()) return NULL;
			do_cmd_cast();
			handle_stuff();
			if (can_do_cmd_cast())
				do_cmd_cast();

			break;
		}


	case 7://ロイヤルフレア MAX500+1000 魅力47で570+1350辺り
		{
			base = p_ptr->lev * 14 + chr_adj * 10;
			dice = p_ptr->lev ;
			sides = 10 + chr_adj ;

			if(only_info) return format("損傷:～%d+%dd%d ",base/2 ,dice,sides);
			msg_format("強烈な閃光と衝撃が周囲を蹂躙した！");
			project(0, 7, py, px, base, GF_SOUND, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM , -1);
			project_hack2(GF_NUKE,dice,sides,0);

			break;
		}
	case 8://賢者の石
		{
			int max_num = 5;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return format("最大召喚可能数:%d",max_num);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_PHILOSOPHER_STONE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_num)
			{
				msg_format("これ以上配置できない。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_PHILOSOPHER_STONE, PM_EPHEMERA)) msg_print("賢者の石を呼び出した！");
			else msg_print("魔法に失敗した。");

		break;
		}
	default:


		if (only_info) return format("未実装");

		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::芳香用特技*/
class_power_type class_power_yoshika[] =
{
	{15,16,40,FALSE,TRUE,A_CHR,0,8,"ヒールバイデザイア",
		"友好的な小神霊が大量に現れる。小神霊は一定確率であなたに吸収されてHPを回復する。"},

	{24,30,50,FALSE,TRUE,A_CON,0,0,"ゾウフォルゥモォ",
		"HPとステータス異常を回復させる。"},

	{32,64,65,TRUE,FALSE,A_STR,0,20,"死なない殺人鬼",
		"自分の周囲全てに対し隣接攻撃を行う。この攻撃により生者から体力を吸収することができる。武器使用時には使えない。"},

	{45,80,80,FALSE,TRUE,A_CON,0,0,"過剰ゾウフォルゥモォ",
		"一時的に加速・狂戦士化・限界を越えた身体能力上昇を得る。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_yoshika(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			int i;
			bool flag = FALSE;
			int num = p_ptr->lev / 14;
			if(only_info) return "";
			for(i=0;i<num;i++)
			{
				if (summon_specific(-1, py, px, p_ptr->lev, SUMMON_DESIRE, PM_ALLOW_GROUP | PM_FORCE_FRIENDLY)) flag = TRUE;
			}
			if(flag)
				msg_print("小神霊が現れた！");
			else
				msg_print("小神霊は現れなかった。");

		}
		break;
	case 1:
		{
			int heal = 100 + plev * 3;
			if(only_info) return format("回復:%d",heal);

			msg_print("主が現れ、あなたの体の修復を始めた・・");
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			restore_level();
			set_alcohol(0);
			set_asthma(0);
			msg_print("主は再びどこかへ行った。");

		}
		break;
	case 2:
		{
			int x,y;
			cave_type       *c_ptr;
			monster_type    *m_ptr;
			monster_race	*r_ptr;
			if(only_info) return format("");

			if(!p_ptr->do_martialarts)
			{
				msg_print("素手でないとその技は使えない。");
				return NULL;
			}

			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				m_ptr = &m_list[c_ptr->m_idx];
				/*:::不可視のモンスターも攻撃するが、壁の中にいる不可視のモンスターは攻撃しない*/
				if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				{
					py_attack(y, x, HISSATSU_YOSHIKA);

				}
			}
		}
		break;
	case 3:
		{
			int base = 10;
			int time;
			if(only_info) return format("期間:%d+1d%d",base,base);
			time = base + randint1(base);
			msg_print("主が現れ、あなたの体に何やら剣呑な術を施した！");
			set_fast(time,FALSE);
			set_shero(time,FALSE);
			set_tim_addstat(A_STR,105,time,FALSE);
			set_tim_addstat(A_CON,105,time,FALSE);
			msg_print("主は愉しげに帰っていった。");

			break;
		}


	default:
			if(only_info) return format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::レティ用特技*/
class_power_type class_power_letty[] =
{
	{3,2,10,FALSE,TRUE,A_WIS,0,2,"冷気の矢",
		"ターゲットに向けて冷気属性のボルトかビームを放つ。レベルが上がると威力が大きく上がりビームが出やすくなるが消費MPも上がる。"},
	{8,10,30,FALSE,TRUE,A_WIS,0,0,"リンガリングコールド",
		"視界内の眠っている敵をさらに深く眠らせる。無生物や冷気耐性を持つ敵には効果が薄い。"},
	{14,15,35,FALSE,TRUE,A_WIS,0,7,"フラワーウィザラウェイ",
		"視界内の全てに冷気攻撃を放つ。レベルが上がると威力が大きく上がるが消費MPも上がる。"},
	{19,16,40,FALSE,TRUE,A_INT,0,0,"アンデュレイションレイ",
		"敵を高確率で眠らせるビームを放つ。睡眠耐性を持つ敵には効かないがレベル35以降は効くようになる。ユニークモンスターには効かない。"},
	{25,16,30,FALSE,TRUE,A_CHR,0,6,"ノーザンウィナー",
		"隣接した一体の敵に特殊な冷気攻撃を行う。氷耐性で軽減されにくく眠っている敵には大ダメージを与えさらに生物の場合は起こしにくい。ユニークモンスターや無生物には効果が薄い。"},
	{32,24,50,FALSE,TRUE,A_WIS,0,2,"コールドスナップ",
		"自分を中心にした極寒属性の巨大な球を放つ。"},
	{40,54,80,FALSE,TRUE,A_CHR,0,0,"テーブルターニング",
		"隣接したターゲット一体に強力な減速攻撃を行う。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

cptr do_cmd_class_power_aux_letty(int num, bool only_info)
{
	int dir;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{

	case 0:
		{
			int dice = 3 + plev / 4;
			int sides = 6 + chr_adj / 5;
			if (WINTER_LETTY1) sides = sides * 3 / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("あなたは冷気の塊を放った！");
			fire_bolt_or_beam(plev * 2, GF_COLD, dir, damroll(dice, sides));
		}
		break;
	case 1: //リンガリングコールド　視界内の寝ているモンスターをより深く眠らせる
		{
			int i;
			if(only_info) return format("");
			msg_print("部屋に寒気が染み渡った・・");
			for (i = 1; i < m_max; i++)
			{
				char m_name[80];

				int pow = plev * 3  + 50;
				int sleep;
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (WINTER_LETTY1) pow += 100;

				if (!m_ptr->r_idx) continue;
				if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
				sleep = MON_CSLEEP(m_ptr);
				if(!sleep) continue;

				monster_desc(m_name, m_ptr, 0);
				if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) pow /= 2;
				if(!monster_living(r_ptr)) pow /= 2;
				if(!(WINTER_LETTY2) && r_ptr->flagsr & RFR_IM_COLD ) pow /= 3; //v1.1.85 吹雪内で耐性貫通
				if(r_ptr->flags2 & RF2_POWERFUL) pow /= 2;
				(void)set_monster_csleep(i, sleep + pow);
				msg_format("%^sの眠りが深くなったようだ。", m_name);
			}


		}
		break;
	case 2://フラワーウィザラウェイ
		{
			int dam = plev * 2 + chr_adj * 5;
			if(dam < 50) dam = 50;

			if (WINTER_LETTY1) dam += plev * 2;

			if(only_info) return format("損傷:%d",dam);
			if(use_itemcard)
				msg_print("吹雪が巻き起こった！");
			else
				msg_print("あなたは吹雪を巻き起こした！");

			project_hack2(GF_COLD, 0,0,dam);
		}

		break;

	case 3://アンデュレイションレイ
		{
			int power = plev * 3 + chr_adj * 5;
			int typ;
			if(power < 80) power = 80;

			if(only_info) return format("効力:%d",power);

			if (!get_aim_dir(&dir)) return NULL;
			if(plev > 34 || use_itemcard)
			{
				typ = GF_STASIS;
				msg_format("青白く輝く光線が放たれた！");
			}
			else
			{
				typ = GF_OLD_SLEEP;
				msg_format("白く輝く光線が放たれた！");
			}
			fire_beam(typ, dir, power);
			break;
		}
	case 4://ノーザンウィナー
		{
			int base = chr_adj * 5;
			int dice = plev / 2;
			int sides = 10;
			int y, x;
			monster_type *m_ptr;
			if (WINTER_LETTY1) base *= 2;
			if(only_info) return format("損傷:%dd%d + %d",dice,sides,base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("あなたは%sに触れ、ゆっくりと寒気を吹き込んだ・・",m_name);
				project(0, 0, y, x, damroll(dice,sides) + base, GF_NORTHERN, flg, -1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;
		}
	case 5://コールドスナップ
		{
			if (WINTER_LETTY1)
			{
				int base = 50 + plev * 2 + chr_adj * 5;
				if (only_info) return format("損傷:%d", base);
				msg_print("吹雪が猛烈に激しさを増した！");
				project_hack2(GF_ICE, 0, 0, base);
				break;

			}
			else
			{
				int base = 100 + plev * 3 + chr_adj * 5;
				int rad = 2 + plev / 10;
				if (only_info) return format("損傷:最大%d", base / 2);
				msg_print("周囲の温度が急激に低下した！");
				project(0, rad, py, px, base, GF_ICE, PROJECT_KILL | PROJECT_ITEM, -1);
				break;

			}
		}

	case 6://テーブルターニング
		{
			int base = plev*3 + chr_adj * 10;
			int y, x;
			monster_type *m_ptr;
			if (WINTER_LETTY1) base = base * 3 / 2;
			if(only_info) return format("損傷:%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];
			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("謎の重圧が%sを押し潰した！",m_name);
				project(0, 0, y, x, base, GF_INACT, flg, -1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::魔法剣士（元素)用特技*/
class_power_type class_power_magic_knight_elem[] =
{
	{8,16,30,FALSE,TRUE,A_INT,0,0,"炎の魔法剣",
		"短時間の間、火炎属性で攻撃できるようになる。"},
	{16,16,30,FALSE,TRUE,A_INT,0,0,"氷の魔法剣",
		"短時間の間、冷気属性で攻撃できるようになる。"},
	{24,16,30,FALSE,TRUE,A_INT,0,0,"雷の魔法剣",
		"短時間の間、電撃属性で攻撃できるようになる。"},
	{32,16,30,FALSE,TRUE,A_INT,0,0,"酸の魔法剣",
		"短時間の間、酸属性で攻撃できるようになる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};


cptr do_cmd_class_power_aux_magic_knight_elem(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("手にしている武器が炎に覆われた。");
			set_ele_attack(ATTACK_FIRE, base + randint1(sides));
			break;
		}
		break;
	case 1:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("手にしている武器が冷気に覆われた。");
			set_ele_attack(ATTACK_COLD, base + randint1(sides));
			break;
		}
		break;
	case 2:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("手にしている武器が火花に覆われた。");
			set_ele_attack(ATTACK_ELEC, base + randint1(sides));
			break;
		}
		break;
	case 3:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			msg_format("手にしている武器が煙に覆われた。");
			set_ele_attack(ATTACK_ACID, base + randint1(sides));
			break;
		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::魔法剣士（予見)用特技*/
class_power_type class_power_magic_knight_fore[] =
{
	{20,24,50,FALSE,TRUE,A_INT,0,0,"予見の魔法剣",
		"短時間の間、武器攻撃の命中率がレベルに応じて大幅に上昇する。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_magic_knight_fore(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_FORESIGHT, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::魔法剣士（召喚)用特技*/
class_power_type class_power_magic_knight_summon[] =
{
	{20,24,50,FALSE,TRUE,A_INT,0,0,"歪曲の魔法剣",
		"短時間の間、武器に切れ味が付加される。鈍器にもつく。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};
//QUESTORやクエストダンジョンの敵が消滅しないように気を付ける

cptr do_cmd_class_power_aux_magic_knight_summon(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_UNSUMMON, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::魔法剣士（付与)用特技*/
class_power_type class_power_magic_knight_enchant[] =
{
	{25,30,50,FALSE,TRUE,A_INT,0,0,"強化の魔法剣",
		"短時間の間、武器のダイス値が増加する。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_enchant(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_INC_DICES, base + randint1(sides));
			break;
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::魔法剣士（暗黒)用特技*/
class_power_type class_power_magic_knight_darkness[] =
{
	{16,20,50,FALSE,TRUE,A_INT,0,0,"魔剣",
		"短時間の間、装備中の武器が秩序勢力の敵に大きなダメージを与えるようになる。レベルが上昇すると吸血効果も追加される。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_darkness(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_DARK, base + randint1(base));
			break;
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::魔法剣士（混沌)用特技*/
class_power_type class_power_magic_knight_chaos[] =
{
	{20,20,50,FALSE,TRUE,A_INT,0,0,"混沌の魔法剣",
		"短時間の間、装備中の武器で攻撃したときに敵を混乱、朦朧させることがある。カオス耐性を持つモンスターには効果がない。ユニークモンスター・巨大な敵・力強い敵には効果が薄い。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_magic_knight_chaos(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int base = 4 + p_ptr->lev/4;
	int sides = 4 + p_ptr->lev/4;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) return NULL;//paranoia
			set_ele_attack(ATTACK_CHAOS, base + randint1(base));
			break;
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::超能力者用特技*/
class_power_type class_power_mind[] =
{
	{15,0,70,FALSE,FALSE,A_WIS,0,0,"明鏡止水",
		"精神を集中して魔力を少し回復する。配下がいると使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

/*:::修験者用特技*/
class_power_type class_power_syugen[] =
{
	{30,1,70,FALSE,FALSE,A_WIS,0,0,"明鏡止水",
		"精神を集中して魔力を少し回復する。配下がいると使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_syugen(int num, bool only_info)
{
	int plev = p_ptr->lev;

	switch(num)
	{
	case 0:
		{
			if(only_info) return format("");

			msg_print("少し頭がハッキリした。");

			p_ptr->csp += (3 + plev/20);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}

			p_ptr->redraw |= (PR_MANA);
		}
		break;
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::ナズーリン専用技*/
class_power_type class_power_nazrin[] =
{
	{3,1,10,FALSE,FALSE,A_INT,0,0,"財宝探知",
		"周囲の金や鉱脈を発見する。"},
	{5,3,20,FALSE,FALSE,A_INT,0,0,"アイテム探知",
		"周囲のアイテムを発見する。"},
	{7,6,25,FALSE,FALSE,A_INT,0,0,"罠探知",
		"周囲に隠された罠を発見する。"},
	{9,8,30,FALSE,FALSE,A_INT,0,0,"モンスター探知",
		"周囲の敵の居場所を知る。"},
	{12,10,15,FALSE,TRUE,A_CHR,0,6,"鼠召喚",
		"配下のネズミ達を呼び出す。レベルが上がると消費MPと呼び出す数が増える。"},
	{15,20,50,FALSE,TRUE,A_INT,0,0,"発見！匠の輝き",
		"このフロアに「名のあるアイテム」があればそれを知ることができる。「伝説のアイテム」のことは分からない。"},
	{16,10,30,FALSE,FALSE,A_INT,0,0,"地形探知",
		"周囲の地形を知る。"},
	{18,25,20,TRUE,FALSE,A_DEX,40,0,"一撃離脱",
		"敵に攻撃し、その後一瞬で短距離テレポートをする。装備品が重いと難易度が上がる。攻撃後にテレポートに失敗することもある。"},
	{20,24,36,FALSE,FALSE,A_INT,0,0,"地域の特産物探訪",
		"このフロアに「素材」カテゴリのアイテムがあればそれを知ることができる。"},
	{23,12,60,FALSE,FALSE,A_INT,0,0,"鑑識",
		"アイテムを鑑定する。完全な情報を知ることはできない。"},
	{25,20,40,FALSE,FALSE,A_INT,0,0,"全感知",
		"周囲のアイテム、罠、モンスターの場所を知る。"},
	{30,30,50,FALSE,FALSE,A_WIS,0,0,"ペンデュラムガード",
		"フロアの雰囲気を察知し、雰囲気が非常に悪いときに「魔法の鎧」効果を得る。アイテム「ナズーリンペンデュラム」を所持していないと使えない。" },
	{32,36,60,FALSE,TRUE,A_INT,0,0,"伝説の品発見",
		"このフロアに「伝説のアイテム」があればそれを知ることができる。"},
	{36,40,70,FALSE,TRUE,A_INT,0,0,"全感知・改",
		"一定時間、周囲の地形・アイテム・トラップ・モンスターを知覚し続ける。"},
	{42,60,80,FALSE,TRUE,A_INT,0,0,"フロア探知",
		"このフロアの地形とアイテムを全て探知する。"},
	{45,80,80,FALSE,TRUE,A_WIS,0,0,"グレイテストトレジャー",
		"破邪属性の強力なビームを放つ。「毘沙門天の宝塔」を所持していないと使えない。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_nazrin(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return "";
			msg_print("あなたはダウジングを始めた・・");
			detect_treasure(DETECT_RAD_DEFAULT);
			detect_objects_gold(DETECT_RAD_DEFAULT);
			break;
		}
	case 1:
		{
			if(only_info) return "";
			msg_print("あなたはダウジングを始めた・・");
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}

	case 2:
		{
			if(only_info) return "";
			msg_print("あなたはダウジングを始めた・・");
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			break;
		}
	case 3:
		{
			if(only_info) return "";
			msg_print("あなたはダウジングを始めた・・");
			(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			(void)detect_monsters_invis(DETECT_RAD_DEFAULT);
			break;
		}
	case 4:
		{
			int max = 2 + p_ptr->lev / 6;
			if(only_info) return format("最大:%d体",max);

			if(use_itemcard)
				msg_print("どこからともなく妖怪鼠たちが現れた。");
			else
				msg_print("あなたは子ネズミを呼び出した。");

			for(i=0;i<max;i++) (void)summon_named_creature(0, py, px, MON_YOUKAI_MOUSE, PM_FORCE_PET);
			break;
		}
	//高級品感知
	case 5:
		{
			if(only_info) return format("");
			msg_print("あなたはダウジングを始めた・・");
			search_specific_object(1);
			break;
		}
	case 6:
		{
			if(only_info) return "";
			msg_print("あなたはダウジングを始めた・・");
			map_area(DETECT_RAD_MAP);
			break;
		}
	case 7:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
			break;
		}
	//素材感知
	case 8:
		{
			if(only_info) return format("");
			msg_print("あなたはダウジングを始めた・・");
			search_specific_object(3);
			break;
		}
	case 9: //鑑識
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;
			break;
		}
	case 10://全感知
		{
			if(only_info) return  format("");
			msg_print("あなたはダウジングを始めた・・");
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}
	case 11: //v1.1.59 ペンデュラムガード
		{
			int base = 20;
			if (only_info) return format("期間:%d+1d%d", base, base);

			msg_print("あなたはペンデュラムを回し始めた・・");
			update_dungeon_feeling(TRUE);
			//do_cmd_feeling_text[]参照
			if (p_ptr->feeling >= 2 && p_ptr->feeling <= 4)
			{
				msg_print("ペンデュラムが妖気に反応して防壁になった！");
				set_magicdef((base + randint1(base)), FALSE);
			}

		}
		break;
	//伝説の品感知
	case 12:
		{
			if(only_info) return format("");
			msg_print("あなたは伝説的な存在を頭に思い浮かべてダウジングを始めた・・");
			search_specific_object(2);
			break;
		}
	//全感知・改
	case 13:
		{
			int base = p_ptr->lev/2;
			if(only_info) return format("期間:%d+1d%d",base,base);

			msg_format("あなたは大量の子鼠を周囲に放った・・");
			set_radar_sense(randint1(base) + base, FALSE);
			break;
		}
	case 14: //フロア探知
		{
			if(only_info) return format("");
			msg_format("あなたは地中に住むネズミ達から情報を集めた・・");
			wiz_lite(FALSE);
			break;
		}
	case 15:
		{
			damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("毘沙門天の宝塔から眩い光が放たれた！");
			fire_spark(GF_HOLY_FIRE,dir,damage,1);

			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::付喪神使い用特技*/
class_power_type class_power_tsukumo_master[] =
{
	{3,20,25,FALSE,TRUE,A_INT,0,0,"付喪神化",
		"武器を付喪神にして戦わせる。付喪神は倒されたり解放されたりフロアを移動すると元の武器に戻る。倒されたとき元の武器は大幅に劣化する。付喪神が倒した敵の経験値とアイテムが手に入る。ランダムアーティファクトの付喪神は劣化しにくく地震や破壊で消滅しない。"},
	{9,12,30,FALSE,TRUE,A_INT,0,0,"アイテム・トラップ感知",
		"周囲の地面のアイテムとトラップを感知する。"},
	{15,18,40,FALSE,TRUE,A_INT,0,0,"トラップ破壊",
		"周囲のトラップに干渉し無効化する。レベルが上がると範囲が広がる。"},
	{24,24,55,FALSE,FALSE,A_INT,0,0,"鑑識",
		"指定したアイテムの働きを知る。レベル45以降でアイテムの働きを完全に知る。"},
	{30,48,70,FALSE,TRUE,A_CHR,0,0,"帰還命令",
		"配下の付喪神があなたの元に帰ってきて元の武器に戻ってザックに入る。ザックの容量が足りないとその分は足元に落ちる。付喪神が戻ってくるのにはやや時間がかかる。元の武器が呪われていると帰還命令に応じない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};

/*:::付喪神使いが付喪神にできる武器　★以外の直接戦闘武器*/
///mod150925 ★を床においてフロア移動しても消えなくなったので★も対象にした
bool item_tester_hook_make_tsukumo(object_type *o_ptr)
{
	//if(object_is_fixed_artifact(o_ptr)) return FALSE;
	return item_tester_hook_melee_weapon(o_ptr);

}



cptr do_cmd_class_power_aux_tsukumo_master(int num, bool only_info)
{
	int dir,dice,sides,base;
	switch(num)
	{
		//武器の付喪神化
	case 0:
		{
			int         item;
			object_type *o_ptr;
			cptr        q, s;
			int			r_idx;
			int		new_o_idx;
			int i;
			int cnt=0,max;
			u32b empty = 0x1F;

			max = 1 + (p_ptr->lev -5) / 10;

			if(only_info) return format("最大:%d体",max);

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
				empty -= (1L << (m_ptr->r_idx - MON_TSUKUMO_WEAPON1));
				cnt++;
			}

			if(cnt >= max)
			{
				msg_print("これ以上の付喪神を扱えない。");
				return NULL;
			}

			for(i=0;i<5;i++)
			{
				if((empty >> i) & 1L)
				{
					r_idx = MON_TSUKUMO_WEAPON1 + i;
					break;
				}
				if(i==4) msg_print("ERROR:付喪神idx管理がおかしい");
			}

			item_tester_hook = item_tester_hook_make_tsukumo;

			q = "どの武器を付喪神化しますか? ";
			s = "付喪神化できる武器がない。";

			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0) o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			apply_mon_race_tsukumoweapon(o_ptr,r_idx);

			new_o_idx = o_pop();

			if (!new_o_idx || !summon_named_creature(0, py, px, r_idx, PM_EPHEMERA))
			{
				msg_print("付喪神化に失敗した。");
			}

			else //ここに入る時summon_named_creature()がTRUEで終わり、付喪神のパラメータ処理と生成が済んでいる
			{
				char m_name[80];
				monster_race *r_ptr = &r_info[r_idx];
				object_copy(&o_list[new_o_idx], o_ptr);
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					if (m_ptr->r_idx == r_idx )
					{
						m_ptr->hold_o_idx = new_o_idx;
						break;
					}
				}
				/*:::新たな付喪神が元アイテムを持っている扱いにする*/
				o_list[new_o_idx].held_m_idx = i;
				o_list[new_o_idx].ix = 0;
				o_list[new_o_idx].iy = 0;
				o_list[new_o_idx].number = 1;

				/*:::思い出情報を全て得て打倒数カウントをリセット*/
				lore_do_probe(r_idx);
				r_ptr->r_sights = 0;
				r_ptr->r_deaths = 0;
				r_ptr->r_pkills = 0;
				r_ptr->r_akills = 0;
				r_ptr->r_tkills = 0;

				object_desc(m_name,&o_list[new_o_idx],OD_NAME_ONLY);

				msg_format("%sは付喪神となって宙に浮いた！",m_name);

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}
			break;
		}
		//アイテム・トラップ感知
	case 1:
		{
			if(only_info) return format("範囲:%d",DETECT_RAD_DEFAULT);
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}
		//トラップ破壊
	case 2:
		{
			int rad = 1 + p_ptr->lev / 7;
			if(only_info) return format("範囲:%d",rad);
			(void)project(0, rad, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);
			break;
		}
		//鑑定
	case 3:
		{
			if(only_info) return "";
			if ( p_ptr->lev < 45)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
			break;

		}
		//帰還命令
		//フロアの付喪神にMFLAG_SPECIALを設定する。フラグが設定された付喪神はprocess_monsterのとき1/7の確率で消滅しザックに入る。
	case 4:
		{
			int i;
			bool flag = FALSE;
			if(only_info) return "";

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
				if(object_is_cursed(&o_list[m_ptr->hold_o_idx])) continue;
				m_ptr->mflag |= MFLAG_SPECIAL;
				flag = TRUE;
			}

			msg_print("付喪神に向けて帰還命令を出した。");
			if(!flag) msg_print("しかし反応がない。");

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::針妙丸専用技*/
class_power_type class_power_shinmyoumaru[] =
{
	{1,0,0,FALSE,FALSE,A_WIS,0,0,"食べ物出てこい！",
		"食べ物が出てくる。代償として＜キノコ型モンスターに囲まれる(40%)・獣に襲われる(30%)＞のいずれかが起こることがある。"},
	{3,3,0,FALSE,FALSE,A_WIS,0,0,"大きくなあれ！",
		"30ターンの間体が大きくなり、腕力と耐久力が増加する。代償として＜満腹度減少(40%)・騒音(25%)＞のいずれかが起こることがある。"},
	{6,6,0,FALSE,FALSE,A_WIS,0,0,"暑さ寒さなくなれ！",
		"30ターンの間火炎と冷気に対する耐性を得る。代償として＜器用さ一時減少(30%)・魅力一時減少(20%)＞のいずれかが起こることがある。"},
	{9,9,0,FALSE,FALSE,A_WIS,0,0,"素早くなあれ！",
		"30ターンの間加速する。代償として＜満腹度減少(50%)・腕力一時減少(20%)＞のいずれかが起こることがある。"},
	{12,12,0,FALSE,FALSE,A_WIS,0,0,"怪我治れ！",
		"HPが最大の1/3回復し切り傷が治る。代償として＜MP半減(40%)・満腹度減少(30%)・経験値一時減少(20%)＞のいずれかが起こることがある。"},
	{15,15,0,FALSE,FALSE,A_WIS,0,0,"配下出てこい！",
		"モンスターを配下として一体召喚する。代償として＜サモンモンスター(30%)・騒音(20%)＞のいずれかが起こることがある。"},
	{18,18,0,FALSE,FALSE,A_WIS,0,0,"見えないもの見えろ！",
		"周囲の地形・アイテム・トラップ・モンスターを感知し一定時間透明視認とテレパシーを得る。代償として＜知能一時減少(30%)・賢さ一時減少(20%)・幻覚(10%)＞のいずれかが起こることがある。"},
	{20,20,0,FALSE,FALSE,A_WIS,0,0,"金銀財宝出てこい！",
		"$300000を得られる。代償として経験値を永久的に300000失う。マイナスにはならない。"},
	{23,23,0,FALSE,FALSE,A_WIS,0,0,"忍び足上手くなあれ！",
		"一定時間隠密能力が大幅に上がる。代償として＜耐久力一時減少(40%)・減速(30%)＞のいずれかが起こることがある。"},
	{25,25,0,FALSE,FALSE,A_WIS,0,0,"もっと大きくなあれ！",
		"一定時間体が倍くらいに大きくなり、腕力と耐久力が大幅に増加する。代償として＜エキサイトモンスター(30%)・減速(20%)＞のいずれかが起こることがある。"},
	{28,28,0,FALSE,FALSE,A_WIS,0,0,"周り全部壊れろ！",
		"12+1d5の範囲を*破壊*する。代償として、＜想定外の大破壊と100+1d150のダメージ(5%)＞が起こることがある。"},
	{30,30,0,FALSE,FALSE,A_WIS,0,0,"とにかく丈夫になあれ！",
		"一定時間、ACと魔法防御が上昇し、元素耐性を獲得し、プレーヤーが受けるほぼ全てのダメージを2/3に減少させる。代償として＜MP半減(50%)・経験値一時減少(30%)・減速(20%)＞のいずれかが起こる。"},
	{33,33,0,FALSE,FALSE,A_WIS,0,0,"お前にかかった魔法消えろ！",
		"ターゲットにかかった一時的効果を全て無効化する。代償として50%の確率で自分にかかった一時効果も全て消える。"},
	{35,35,0,FALSE,FALSE,A_WIS,0,0,"怪我とか色々全部治れ！",
		"HPが完全に回復し、経験値と能力値が回復し、全てのステータス異常が消える。代償として＜記憶喪失(50%)・装備劣化(20%)＞のいずれかが起こることがある。"},
	{37,37,0,FALSE,FALSE,A_WIS,0,0,"この階全部見えろ！",
		"マップを明るく照らし、全ての地形・アイテム・トラップ・モンスターを感知する。代償として＜幻覚(30%)・経験値大幅一時減少(20%)・ダンジョンの壁が消えてモンスターが起きる(10%)＞のいずれかが起こることがある。"},
	{40,75,80,FALSE,TRUE,A_WIS,0,0,"輝針剣",
		"ターゲット周辺に向けて防御不可能なビーム攻撃を連続で放つ。"},
	{43,43,0,FALSE,FALSE,A_WIS,0,0,"もっともっともっと大きくなあれ！",
		"一定時間巨大化する。巨大化中は身体能力と白兵戦能力が爆発的に上昇し一部の技が強力になるが巻物や魔道具が使用不能になる。Uコマンドで人間大に戻ることができる。代償として＜エキサイトモンスター(50%)・経験値大幅一時減少(20%)・悪性突然変異(10%)＞のいずれかが起こることがある。"},

	{45,140,85,FALSE,TRUE,A_CHR,0,0,"七人の一寸法師",
		"ごく短時間分身し、敵からの攻撃の6/7を回避する。" },

	{47,47,0,FALSE,FALSE,A_WIS,0,0,"強力な装備品出てこい！",
		"高級なアイテムが複数現れる。代償として＜強力なサモンモンスター(30%)・太古の怨念発動(20%)・経験値1～3%永久減少(10%)＞のいずれかが起こることがある。"},
	{50,50,0,FALSE,FALSE,A_WIS,0,0,"ラスボス倒れろ！",
		"ラスボスを問答無用で倒す。鉄獄100階でしか使えない。代償として＜魔力消去・ダンジョンの壁が消えてモンスターが起きる・防御不能のランダムテレポート・太古の怨念・強力なサモンモンスター＞が全て発生し、このゲーム中に小槌が使用不能になり、さらに小槌の効果が***全て***消える。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


void koduchi_payment(int kind)
{

	switch(kind)
	{
	case 0: //何もなし
		break;

	case 1: //キノコに囲まれる
		{
			int num = randint1(3) + 2;
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_MUSHROOM, (PM_NO_PET | PM_ALLOW_GROUP)) && !flag)
				{
					flag = TRUE;
					msg_print("食べ物の中に何か妙な物が紛れている。");
				}
			}
		}
		break;

	case 2: //動物に囲まれる
		if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL, (PM_NO_PET | PM_ALLOW_GROUP | PM_ALLOW_UNIQUE)))
		{
			msg_print("どこからともなく獣が寄ってきた！");
		}

		break;

	case 3: //満腹度減少
		msg_print("お腹が空いた・・");
		(void)set_food(p_ptr->food * (25+randint0(26)) / 100);

		break;

	case 4: //騒音
		msg_print("妙にコミカルな音が出た！");
		aggravate_monsters(0,FALSE);
		break;
	case 5: //魅力一時減少
		msg_print("お肌が荒れた。");
		if (!p_ptr->sustain_chr) (void)do_dec_stat(A_CHR);
		break;
	case 6: //器用一時減少
		msg_print("少し体の動きがぎこちない。");
		if (!p_ptr->sustain_dex) (void)do_dec_stat(A_DEX);
		break;
	case 7: //腕力一時減少
		msg_print("少し力が抜ける。");
		if (!p_ptr->sustain_str) (void)do_dec_stat(A_STR);
		break;
	case 8: //MP半減
		msg_print("疲労感に襲われた・・");
		p_ptr->csp /= 2;
		p_ptr->redraw |= (PR_MANA);
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
		break;
	case 9: //経験値一時減少
		drain_exp(200 + (p_ptr->exp / 100), 200 + (p_ptr->exp / 1000), 75);
		break;
	case 10: //サモンモンスター
		if( summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			msg_print("何かが襲い掛かってきた！");
		break;
	case 11: //知能一時減少
		msg_print("少し頭が痛い。");
		if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
		break;
	case 12: //賢さ一時減少
		msg_print("少し頭がボーっとする。");
		if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
		break;
	case 13: //幻覚
		if (!p_ptr->resist_chaos) (void)set_image(p_ptr->image + randint0(50) + 50);
		break;
	case 14: //減速
		msg_print("急に足が重くなった・・");
		(void)set_slow(randint1(15) + 15, FALSE);
		break;
	case 15: //耐久一時減少
		msg_print("少しフラフラする。");
		if (!p_ptr->sustain_con) (void)do_dec_stat(A_CON);
		break;
	case 16: //経験値30万消滅
		msg_print("今まで積んだ経験が消えていく気がする！");
		p_ptr->max_exp = MAX(0,p_ptr->max_exp - 300000);
		if(p_ptr->exp > p_ptr->max_exp ) p_ptr->exp = p_ptr->max_exp;
		check_experience();
		p_ptr->redraw |= (PR_GOLD | PR_EXP);
		redraw_stuff();

		break;
	case 17: //エキサイトモンスター
		msg_print("恐ろしい音がダンジョンに鳴り響いた！");
		aggravate_monsters(0,TRUE);
		break;

	case 18: //魔力消去
		dispel_player();
		break;
	case 19: //記憶喪失
		msg_print("何だか不自然なほど頭がスッキリした！");
		lose_all_info();
		break;
	case 20: //装備劣化
		msg_print("何かヒビが入るような音が聞こえた。");
		apply_disenchant(0,0);
		break;
	case 21: //経験値大幅一時減少
		msg_print("体から生命力が抜け落ちた気がする。");
		lose_exp(p_ptr->exp / 16);
		break;
	case 22: //壁がすべて消える
		vanish_dungeon();
		break;
	case 23: //悪性突然変異 肉体、精神関係
		{
			int muta_lis[] = {75,76,77,78,79,89,109,123,129,136,143,145,151,154,178,185,188};
			gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int))]);
		}
		break;
	case 24: //強力なモンスター
		activate_hi_summon(py,px,FALSE);
		msg_print("突如、強大なモンスターたちが襲い掛かってきた！");
		break;
	case 25: //太古の怨念
		{
			int count = 0;
			msg_print("太古の怨念が発動した！");
			activate_ty_curse(FALSE, &count);
		}
		break;
	case 26: //経験値1-3%永久減少
		msg_print("今まで積んだ経験が消えていく気がする！");
		p_ptr->max_exp -= (p_ptr->max_exp / 100 * randint1(3));
		if(p_ptr->exp > p_ptr->max_exp ) p_ptr->exp = p_ptr->max_exp;
		check_experience();
		break;


	default:
		msg_format("ERROR:小槌代償(%d)が定義されていない",kind);
	}


}

/*:::針妙丸用特技　小槌の充填カウント用としてtim_general[0]を使用する。また最終技で小槌が使用不能になるとmagic_num1[0]に1が入る。*/
/*:::代償の発生は別関数で処理する。*/
cptr do_cmd_class_power_aux_shinmyoumaru(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int payment = 0;
	int charge = 0;
	int i;
	int rnd = randint0(100);

	switch(num)
	{

	case 0: //食料出てこい
		{
			int num = 3;
			object_type forge, *q_ptr = &forge;
			int sval[6] = {SV_FOOD_RATION,SV_FOOD_HARDBUSC,SV_FOOD_VENISON,SV_FOOD_CUCUMBER,SV_FOOD_ABURAAGE,SV_FOOD_SLIMEMOLD};

			charge = 1;
			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると空から食べ物が降ってきた。");
			for(i=0;i<num;i++)
			{
				object_prep(q_ptr, lookup_kind(TV_FOOD, sval[randint0(6)] ));
				drop_near(q_ptr, -1, py, px);
			}
			if(rnd < 30) payment = 1;
			else if(rnd < 70) payment = 2;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);
			break;
		}
	case 1://大きくなあれ
		{
			int base = 30;
			charge = 5;

			if(only_info)
			{
				if(use_itemcard)
					return format("期間:%dターン",base);
				else
					return format("充填:%dターン",charge);
			}

			msg_print("小槌を振ると体が一回り大きくなった！");
			set_tim_addstat(A_STR,5,base,FALSE);
			set_tim_addstat(A_CON,5,base,FALSE);

			if(use_itemcard) break;//アイテムカード使用時は代償なし

			if(rnd < 40) payment = 3;
			else if(rnd < 65) payment = 4;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 2://暑さ寒さなくなれ
		{
			int base = 30;
			charge = 3;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると心地よい空気があなたを包んだ！");
			set_oppose_cold(base, FALSE);
			set_oppose_fire(base, FALSE);

			if(rnd < 30) payment = 6;
			else if(rnd < 50) payment = 5;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 3://素早くなあれ
		{
			int base = 30;
			charge = 5;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると体が軽くなった！");
			set_fast(base, FALSE);
			if(rnd < 50) payment = 3;
			else if(rnd < 70) payment = 7;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 4://怪我治れ
		{
			charge = 7;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると体が光に包まれた！");
			hp_player(p_ptr->mhp / 3);
			set_stun(0);
			set_cut(0);
			if(rnd < 40) payment = 8;
			else if(rnd < 70) payment = 3;
			else if(rnd < 90) payment = 9;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 5://配下出てこい
		{
			charge = 10;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると何かの気配を感じた・・");
			if (!summon_specific(-1, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_FORCE_PET)))
			{
				msg_print("しかし何も現れなかった。");
				set_tim_general(charge,FALSE,0,0);
				return ""; //召喚失敗したので代償なしだが充填はあり
			}
			if(rnd < 30) payment = 10;
			else if(rnd < 50) payment = 4;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 6://見えないもの見えろ
		{
			int base = 30;
			charge = 10;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると感覚が研ぎ澄まされた・・");

			map_area(DETECT_RAD_MAP);
			detect_all(DETECT_RAD_DEFAULT);
			set_tim_invis(base, FALSE);
			set_tim_esp(base, FALSE);

			if(rnd < 30) payment = 11;
			else if(rnd < 50) payment = 12;
			else if(rnd < 60) payment = 13;
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 7://金銀財宝出てこい
		{
			charge = 10000;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("この願いの代償は危険かもしれない。");
			if (!get_check_strict("本当に願いますか？ ", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("小槌を振ると天から金銀財宝が降ってきた！");
			p_ptr->au += 300000L;
			payment = 16;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 8://忍び足上手くなあれ
		{
			int base = 30;
			charge = 15;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("こっそり小槌を振った。");
			set_tim_stealth(randint1(base) + base, FALSE);

			if(rnd < 40) payment = 15;
			else if(rnd < 70) payment = 14;
			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 9://もっと大きくなあれ
		{
			int percentage;
			int base = 30;
			charge = 15;

			if(only_info)
			{
				if(use_itemcard)
					return format("期間:%dターン",base);
				else
					return format("充填:%dターン",charge);
			}

			percentage = p_ptr->chp * 100 / p_ptr->mhp;

			msg_print("小槌を振ると体が倍ほどに大きくなった！");
			set_tim_addstat(A_STR,110,base,FALSE);
			set_tim_addstat(A_CON,110,base,FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();

			if(use_itemcard) break;//アイテムカード使用時は代償なし

			if(rnd < 30) payment = 17;
			else if(rnd < 20) payment = 14;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 10://周り全部壊れろ
		{
			charge = 7;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("あなたは思い切り小槌を振った！");

			if(rnd < 95)
			{
				int base = 12;
				int sides = 4;
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
			else if((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print("しかし地面が少し揺れただけだった。");
			}
			else
			{
				if (destroy_area(py, px, 15 + p_ptr->lev + randint0(11), FALSE,FALSE,FALSE))
				{
					msg_print("強大な力がダンジョンをひっくり返した！");
					take_hit(DAMAGE_NOESCAPE, 100 + randint1(150), "小槌の巻き起こした大破壊", -1);
				}
				else msg_print("地面が大きく揺れた。");

			}
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 11://とにかく丈夫になれ
		{
			int base = 30;
			charge = 30;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると強靭な防壁があなたを包んだ！");
			set_oppose_acid(base, FALSE);
			set_oppose_elec(base, FALSE);
			set_oppose_fire(base, FALSE);
			set_oppose_cold(base, FALSE);
			set_oppose_pois(base, FALSE);
			set_magicdef(base, FALSE);

			if(rnd < 50) payment = 8;
			else if(rnd < 80) payment = 9;
			else payment = 14;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}

	case 12://お前の魔法消えろ
		{

			int m_idx;

			if(only_info) return format("充填:%dターン",charge);
			charge = 1;
			if (!target_set(TARGET_KILL)) return NULL;
			m_idx = cave[target_row][target_col].m_idx;
			if (!m_idx) return NULL;
			if (!player_has_los_bold(target_row, target_col)) return NULL;
			if (!projectable(py, px, target_row, target_col)) return NULL;
			dispel_monster_status(m_idx);
			if(rnd < 50) payment = 18;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}

	case 13://怪我とか色々全部治れ
		{
			charge = 16;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると体がまばゆい光に包まれた！");
			hp_player(p_ptr->mhp);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			set_image(0);
			restore_level();
			do_res_stat(A_STR);
			do_res_stat(A_INT);
			do_res_stat(A_WIS);
			do_res_stat(A_DEX);
			do_res_stat(A_CON);
			do_res_stat(A_CHR);
			set_alcohol(0);

			if(rnd < 50) payment = 19;
			else if(rnd < 70) payment = 20;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}


	case 14://この階全部見えろ
		{
			charge = 30;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振ると千里先をも見通せるようになった！");

			wiz_lite(FALSE);
			(void)detect_traps(255, TRUE);
			(void)detect_monsters_invis(255);
			(void)detect_monsters_normal(255);

			if(rnd < 30) payment = 13;
			else if(rnd < 50) payment = 21;
			else if(rnd < 60) payment = 22;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 15: //輝針剣
		{
			int dice = p_ptr->lev / 5;
			int dir;
			int sides = 3 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			int num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 6;
			if(p_ptr->mimic_form == MIMIC_GIGANTIC) dice *= 2;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("眩く輝く%s刃を放った！",(p_ptr->mimic_form==MIMIC_GIGANTIC)?"巨大な":"");
			fire_blast(GF_PSY_SPEAR, dir, dice, sides, num, 2,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));

			break;
		}

	case 16://もっともっともっと大きくなあれ
		{
			int percentage;
			int base = 30;
			charge = 15;

			if(only_info) return format("充填:%dターン",charge);

			percentage = p_ptr->chp * 100 / p_ptr->mhp;

			msg_print("小槌を振った！");
			if(!set_mimic(base+randint1(base), MIMIC_GIGANTIC, FALSE)) return NULL;
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();

			if(rnd < 50) payment = 17;
			else if(rnd < 70) payment = 21;
			else if(rnd < 80) payment = 23;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 17:
		{
			int base = 3;
			if (only_info) return format("期間:%d+1d%d", base, base);
			set_multishadow(base + randint1(base), FALSE);
			break;
		}
	case 18://強力な装備品出てこい
		{
			charge = 100;

			if(only_info) return format("充填:%dターン",charge);

			msg_print("小槌を振った！");
			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);

			if(rnd < 30) payment = 24;
			else if(rnd < 50) payment = 25;
			else if(rnd < 60) payment = 26;

			koduchi_payment(payment);
			set_tim_general(charge,FALSE,0,0);

			break;
		}
	case 19://ラスボス倒れろ！
		{

			bool dummy;
			int oberon_idx = 0;
			if(only_info) return format("充填:-----");
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (m_ptr->r_idx == MON_OBERON ) oberon_idx = i;
			}

			if(!oberon_idx)
			{
				msg_print("倒すべき者はここにはいない。");
				return NULL;
			}

			msg_print("恐らくこの願いの代償は凄まじく強大だ。");
			if (!get_check_strict("本当に願いますか？ ", CHECK_OKAY_CANCEL)) return NULL;

			koduchi_payment(18);
			koduchi_payment(22);
			teleport_player(200,TELEPORT_NONMAGICAL);
			mon_take_hit(oberon_idx,30001,&dummy,"はその場に倒れた！");
			koduchi_payment(25);
			koduchi_payment(24);

			p_ptr->magic_num1[0] = 1;
			msg_print("小槌にヒビが入った！");
			//ここで種族が小人に戻るのは現在の種族が「小人(人間大)」のときのみにする
			if(p_ptr->prace == RACE_GREATER_LILLIPUT)
				change_race(RACE_KOBITO,"");

			break;
		}




	default:
		//msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::お燐専用技*/
class_power_type class_power_orin[] =
{
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"猫車収納",
		"愛用の猫車にアイテムを入れる。アイテム欄の数はレベルアップで上昇する。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"猫車確認",
		"愛用の猫車の中を見る。"},
	{1,0,0,FALSE,FALSE,A_DEX,0,0,"猫車取出し",
		"愛用の猫車からアイテムを出す。"},
	{10,0,0,FALSE,TRUE,A_CON,0,0,"キャッツウォーク",
		"猫の姿になる。変身中は移動速度と隠密能力が飛躍的に上昇する。しかし戦闘能力が低下し、ほとんどの装備品が一時無効化され、巻物・魔道具・魔法・特技を使えなくなる。Uコマンドから人型に戻ることができる。"},
	{18,10,0,FALSE,TRUE,A_CON,0,12,"旧地獄の針山",
		"視界内のランダムな敵に地獄属性のボルトを数発放つ。レベルが上がると本数と消費MPが増える。"},
	{24,20,25,FALSE,TRUE,A_CHR,0,0,"ゾンビフェアリー",
		"ゾンビフェアリーを召喚する。"},
	{32,44,40,FALSE,TRUE,A_CHR,0,0,"食人怨霊",
		"大量の怨霊を召喚し、近くのランダムな敵に憑りつかせる。"},
	{45,70,50,FALSE,TRUE,A_CHR,0,0,"火焔の車輪",
		"視界内全てに対して地獄の業火属性の攻撃を行う。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};


cptr do_cmd_class_power_aux_orin(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if(!put_item_into_inven2()) return NULL;
			break;

		}
	case 1:
		{
			if(only_info) return format("");
			display_list_inven2();
			return NULL; //見るだけなので行動順消費なし

		}
	case 2:
		{
			if(only_info) return format("");
			if(!takeout_inven2()) return NULL;
			break;
		}
	case 3:
		{
			if(only_info) return format("期間:最長1000ターン");
			if(!set_mimic(1000, MIMIC_CAT, FALSE)) return NULL;

			break;
		}

	case 4:
		{
			bool flag = FALSE;
			int i;
			int dice = 2 + plev / 5;
			int sides = 4 + adj_general[p_ptr->stat_ind[A_CHR]] / 10;
			int num = 3 + plev / 4;
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			msg_print("地獄の針を放った！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_NETHER, damroll(dice,sides),1, 0,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
		}
		break;

	case 5:
		{
			int slev = p_ptr->lev;
			int k;
			if(slev < 20) slev=20;
			if(only_info) return format("");
			if (summon_specific(-1, py, px, slev, SUMMON_ZOMBIE_FAIRY, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				msg_print("ゾンビ姿の妖精たちを呼び出した！");

			break;
		}

	case 6:
		{
			int rad = 0;
			bool flag = FALSE;
			int num;
			int i;
			int power = p_ptr->lev * 3 / 2;
			num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;

			if(power < 50) power = 50;
			if(num < 4) num = 4;

			if(only_info) return format("効力:%d",power);

			if(use_itemcard)
				msg_format("カードから大量の怨霊が湧きだしてきた！");
			else
				msg_format("あなたは大量の怨霊を放った！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_POSSESSION, power,3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}
	case 7:
		{
			dice = 16;
			sides = 20 + adj_general[p_ptr->stat_ind[A_CHR]] ;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("地獄の火炎が放たれた！");
			project_hack2(GF_HELL_FIRE,dice,sides,0);
			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::チルノ専用技*/
class_power_type class_power_cirno[] =
{
	{1,2,3,FALSE,TRUE,A_DEX,0,5,"アイシクルシュート",
		"冷気属性のボルトを放つ。レベルが上がると威力と本数が上がるが消費MPも上がる。"},
	{4,4,10,FALSE,TRUE,A_CON,0,2,"フローズン冷凍法",
		"隣接した敵を凍らせ、極寒属性のダメージを与える。"},
	{8,8,15,FALSE,TRUE,A_WIS,0,0,"冷凍光線",
		"ターゲット周辺に冷気属性のビームを三発放つ。当たらないこともある。"},
	{12,12,20,FALSE,TRUE,A_DEX,0,5,"フェアリースピン",
		"周囲に対して数発の冷気属性攻撃を仕掛ける。自分に近いほどダメージが大きい。装備が重いと失敗しやすい。"},
	{15,14,30,FALSE,TRUE,A_INT,0,2,"フロストピラーズ",
		"敵が通過しようとすると冷気属性の爆発を起こす罠を仕掛ける。何箇所でも仕掛けられる。"},
	{18,20,30,FALSE,TRUE,A_DEX,0,0,"ソードフリーザー",
		"装備中の武器に対し一時的に冷気属性を付与する。"},
	{22,15,35,FALSE,TRUE,A_WIS,0,7,"アイシクルボム",
		"極寒属性の巨大なボールを放つ。ボールは何かに当たるとその場で爆発する。"},

	{24,25,40,FALSE,TRUE,A_STR,0,0,"氷塊生成",
		"隣接した床一か所の地形を「氷塊」にする。氷塊は通り抜けられないが叩き壊したり火炎で溶かしたりできる。また閃光属性の攻撃が当たった場合大爆発することがある。この爆発はプレイヤーもダメージを受ける。" },

	{27,30,45,FALSE,TRUE,A_CON,0,0,"瞬間冷凍ビーム",
		"極寒属性のレーザーを広範囲に放つ。威力はHPの1/3になる。"},
	{31,32,70,FALSE,TRUE,A_DEX,0,8,"アイシクルマシンガン",
		"冷気属性のボルトを大量に発射する。ただし着弾点は狙いより若干ばらつく。"},
	{36,27,50,FALSE,TRUE,A_STR,50,8,"グレートクラッシャー",
		"隣接した敵を巨大な氷のハンマーで殴りつけて朦朧とさせる。冷気耐性のない敵には1.5倍のダメージを与える。装備が重いと失敗しやすい。"},
	{40,75,80,FALSE,TRUE,A_INT,0,0,"フロストキング",
		"魔法の氷の結晶を出現させる。結晶は配下扱いとなり、移動せずに常に冷気属性攻撃を行う。また結晶は妖魔本には入らず階移動で消える。"},
	{45,50,80,FALSE,TRUE,A_CHR,0,0,"パーフェクトフリーズ",
		"視界内の全てに対し強力な極寒属性攻撃を放つ。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_cirno(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	//v1.1.32 日焼けしたチルノの特技パワーアップ
	if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
	{
		plev += 10;
		chr_adj += 10;
		if(chr_adj > 50) chr_adj = 50;
	}

	switch(num)
	{

	//アイシクルシュート
	case 0:
		{
			int i;
			int dice = 2 + plev / 7;
			int dir;
			int sides = 3 + chr_adj / 15;
			int num = MIN(10,(1 + plev / 4));
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("氷の弾を放った！");
			for (i = 0; i < num; i++) fire_bolt(GF_COLD, dir, damroll(dice, sides));

		}
		break;

	//フローズン冷凍法
	case 1:
		{

			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int dir, damage;

			damage=plev * 2 + chr_adj * 3 ;
			if(damage < 15) damage=15;

			if(only_info) return  format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(use_itemcard)
					msg_format("%sに触って氷漬けにした！",m_name);
				else
					msg_format("%sは突然氷漬けになった！",m_name);
				project(0,0,y,x,damage,GF_ICE,PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID,0);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
		}
		break;
	//冷凍光線
	case 2:
		{
			int dir;
			int dice = 2 + p_ptr->lev / 5;
			int sides = 10 + chr_adj / 5;
			if(only_info) return format("損傷:(%dd%d) * 3",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("冷気のビームを放った！");
			fire_blast(GF_COLD, dir, dice, sides, 3, 4,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));
			break;

		}
		break;
	//フェアリースピン
	case 3:
		{
			int base = 25 + plev + chr_adj * 2;
			int num = 2 + plev / 30 + chr_adj / 20;
			int rad = 2 + plev / 30;
			int i;
			if(only_info) return format("損傷:最大%d * %d",base/2,num);
			msg_print("氷の弾を放ちながら回転した。");
			for (i = 0; i < num; i++) project(0, rad, py, px, base, GF_COLD, PROJECT_KILL | PROJECT_ITEM, -1);
			break;

		}
		break;

	//フロストピラーズ
	case 4:
		{
			if(only_info) return format("損傷:%d + 7d7",plev);
			msg_print("足元に氷柱を仕込んだ・・");
			explosive_rune();
		}
		break;

	//ソードフリーザー
	case 5:
		{
			int base = p_ptr->lev/2;

			if(base < 10) base = 10;

			if(only_info) return format("期間:%d+1d%d",base,base);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_print("武器を持っていないとそれを使えない。");
				return NULL;
			}

			msg_format("手にしている武器が氷に覆われた。");
			set_ele_attack(ATTACK_COLD, base + randint1(base));
			break;
		}
		break;
	//アイシクルボム
	case 6:
		{
			int dir;
			int rad = 3;
			int dice = p_ptr->lev / 4 + chr_adj / 3;
			int sides = 10 + p_ptr->lev / 5;
			int base = p_ptr->lev * 2;
			if(p_ptr->lev > 39) rad = 4;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な氷塊を放った！");
			fire_rocket(GF_ICE, dir, base + damroll(dice,sides), rad);
			break;
		}
		//v1.1.68 氷塊生成
	case 7:
		{
		int y, x,dir;

		if (only_info) return format("");
		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave_naked_bold(y, x))
		{
			msg_print("この場所には特技を使えない。");
			return NULL;
		}
		msg_print("あなたは強固な氷を作り出した。");
		cave_set_feat(y, x, f_tag_to_index_in_init("ICE_WALL"));

		break;
		}

	//瞬間冷凍ビーム
	case 8:
		{
			int dir;
			int base = p_ptr->chp / 3;
			if(base<1) base = 1;
			if(base > 1600) base=1600;
			if(only_info) return format("損傷:%d",base);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("カードから強烈な冷気が放たれた！");
			else
				msg_print("全力の冷凍ビームを放った！");
			fire_spark(GF_ICE,dir,base,1);
			break;

		}
	//アイシクルマシンガン
	case 9:
		{
			int i;
			int dice = 2 + plev / 8;
			int dir;
			int sides = 3  + chr_adj / 20;
			int num = MIN(30,(10 + plev / 5 + chr_adj / 3));
			if(only_info) return format("損傷:(%dd%d) * %d",dice,sides,num);
			if (!get_aim_dir(&dir)) return NULL;
			if(use_itemcard)
				msg_print("カードから大量の氷塊が放たれた！");
			else
				msg_print("大量の氷弾幕を放った！");
			fire_blast(GF_COLD, dir, dice, sides, num, 4, 0);
			break;
		}

	//グレートクラッシャー
	case 10:
		{
			int y, x;
			int dist;
			int dir;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage=plev * 4 + adj_general[p_ptr->stat_ind[A_STR]] * 10 / 3 + chr_adj * 10 / 3  ;

			if(only_info) return  format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if(dir==5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				if(!(r_ptr->flagsr & RFR_IM_COLD)) damage = damage * 3 / 2;
				if(r_ptr->flagsr & RFR_HURT_COLD) damage *= 2;
				monster_desc(m_name, m_ptr, 0);
				msg_format("巨大な氷のハンマーを%sに叩きつけた！",m_name);
				project(0,0,y,x,damage,GF_MISSILE,PROJECT_KILL,0);
				if(cave[y][x].m_idx && !(r_ptr->flags3 & RF3_NO_STUN))
				{
					msg_format("%sはフラフラになった。",m_name);
					(void)set_monster_stunned(cave[y][x].m_idx,MON_STUNNED(m_ptr)+ 4 + randint0(4) );
				}
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;

		}
	//フロストキング
	case 11:
		{
			int max_ice = 2;
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(plev > 44) max_ice += (plev - 40) / 5;
			if(max_ice > 5) max_ice = 6;
			if(only_info) return format("最大:%d",max_ice);

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_CIRNO_ICE) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt >= max_ice)
			{
				msg_format("これ以上配置できない。",num);
				return NULL;
			}

			(void)summon_named_creature(0, py, px, MON_CIRNO_ICE, PM_EPHEMERA);

		}
		break;
	//パーフェクトフリーズ
	case 12:
		{
			int dam = plev * 4 + chr_adj * 10;
			if(only_info) return format("損傷:%d",dam);
			msg_print("一瞬、周囲の全ての物質が活動を停止した・・");
			project_hack2(GF_ICE, 0,0,dam);
		}

		break;


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::巫女・神官専用技*/
class_power_type class_power_priest[] =
{

	{1,5,0,FALSE,FALSE,A_WIS,0,0,"奉納",
		"神にアイテムを捧げて祈る。アイテムの価値とプレイヤーのレベルに応じて経験値を得られる。酒を捧げると価格の割に得られる経験値が多い。あまり変なものを捧げると罰を受けることがある。"},
	{30,60,90,FALSE,TRUE,A_WIS,0,0,"祝福",
		"武器一つに対し祝福を与える。"},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_priest(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{
		//奉納
		case 0:
		{
			int item;
			int amt;
			s32b price, value;
			bool goodrealm = is_good_realm(p_ptr->realm1);
			int exp;
			u32b flgs[TR_FLAG_SIZE];
			object_type forge;
			object_type *q_ptr;
			object_type *o_ptr;
			bool baditem = FALSE;
			cptr q, s;
			char o_name[MAX_NLEN];

			if(only_info) return format("");

			q = "どのアイテムを神に捧げますか? ";
			s = "神に捧げられそうなアイテムを持っていない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else	o_ptr = &o_list[0 - item];

			amt = 1;
			if (o_ptr->number > 1)
			{
				amt = get_quantity(NULL, o_ptr->number);
				if (amt <= 0) return NULL;
			}

			q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			q_ptr->number = amt;
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND)) q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			object_desc(o_name, q_ptr, 0);

			if(!get_check(format("%sを捧げますか？",o_name))) return NULL;

			if(goodrealm)
				msg_format("%sは天に昇って消えた。",o_name);
			else
				msg_format("%sは溶け崩れて地面へ消えた。",o_name);
			if (item >= 0)
			{
				inven_item_increase(item, -amt);
				inven_item_describe(item);
				inven_item_optimize(item);
			}
			else
			{
				floor_item_increase(0 - item, -amt);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
			object_flags(q_ptr,flgs);
			value = object_value_real(q_ptr) * q_ptr->number;
			exp = value * 10 / (100 - p_ptr->lev);
			/*:::神様は酒が好き*/
			if(q_ptr->tval == TV_ALCOHOL) exp *= 5;
			//アーティファクトは経験値が高い
			if(object_is_artifact(q_ptr)) exp *= 3;
			/*:::変なアイテムを捧げると怒る*/
			if( !goodrealm && have_flag(flgs, TR_BLESSED) ) baditem = TRUE;
			else if((q_ptr->curse_flags & TRC_HEAVY_CURSE) || (q_ptr->curse_flags & TRC_PERMA_CURSE)) baditem = TRUE;
			else if(q_ptr->tval == TV_ALCOHOL && q_ptr->sval == SV_ALCOHOL_MARISA) baditem = TRUE;
			else if(q_ptr->tval == TV_CHEST && !q_ptr->pval) baditem = TRUE;

			else if( !value && one_in_(13)) baditem = TRUE;

			if( !exp  ||  baditem)
			{
				if(goodrealm)
					msg_format("・・と思ったら天から落ちてきた。");
				else
					msg_format("・・と思ったら地面から吐き出された。");
				(void)drop_near(q_ptr, -1, py, px);
			}

			if( baditem )
			{
				bool stop_ty = FALSE;
				int count = 0;
				msg_format("あなたは神の怒りに触れてしまったようだ！");
				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			else if(exp)
			{
				msg_print("神の加護を感じる・・・");

				if(p_ptr->prace == RACE_ANDROID)
				{
					if(randint0(30000) < exp) acquirement(py,px,3,TRUE,FALSE);
					else if(randint0(10000) < exp) acquirement(py,px,1,TRUE,FALSE);
				}
				else
				{
					gain_exp(exp);
				}
			}

		}
		break;
		case 1:
		{
			if(only_info) return format("");
			if (!bless_weapon()) return NULL;
		}
		break;

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::青娥専用技*/
class_power_type class_power_seiga[] =
{
	{1,3,5,FALSE,TRUE,A_DEX,0,0,"壁堀り",
		"愛用の鑿で壁やドアに穴を開ける。"},
	{12,20,20,FALSE,TRUE,A_DEX,0,0,"床掘り",
		"愛用の鑿で床を掘り、一つ下のフロアへ行く。クエスト中やダンジョン最下層では使えない。"},
	{22,30,50,FALSE,TRUE,A_STR,0,0,"帰還",
		"愛用の鑿で長距離を掘り抜き、ダンジョンと地上の間を行き来する。発動までにはタイムラグがある。"},
	{30,45,10,FALSE,TRUE,A_INT,0,0,"トンリン芳香",
		"愛用のキョンシーを呼び出す。芳香は階移動で消え捕獲不可能で維持コストが安い以外は通常の配下と同じ扱いになる。倒されても何度でも呼び出せる。"},
	{36,32,20,FALSE,TRUE,A_WIS,0,16,"ヤンシャオグイ",
		"赤子の霊を使役し、視界内の生命のある存在に対しランダムに複数回の地獄属性攻撃を行う。"},
	{42,45,70,FALSE,TRUE,A_INT,0,0,"ウォールランナー",
		"広範囲の一直線上の床を壁にする。敵を挟んで倒したりはできない。"},


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",""},
};

cptr do_cmd_class_power_aux_seiga(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{

			int x, y;
			cave_type *c_ptr;
			feature_type *f_ptr, *mimic_f_ptr;
			if(only_info) return format("");

			if (!get_rep_dir2(&dir)) return FALSE;
			y = py + ddy[dir];
			x = px + ddx[dir];
			c_ptr = &cave[y][x];
			f_ptr = &f_info[c_ptr->feat];
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				msg_format("%sに穴を開けた。", f_name + f_info[get_feat_mimic(c_ptr)].name);
				cave_alter_feat(y, x, FF_HURT_ROCK);
				p_ptr->update |= (PU_FLOW);
			}
			else
			{
				msg_print("そこには穴を開けられない。");
				return NULL;
			}

			break;

		}
	case 1:
		{
			if(only_info) return format("");

			if(!dun_level)
			{
				msg_print("ダンジョンの中でないとこの技は使えない。");
				return NULL;

			}
			else if(quest_number(dun_level))
			{
				msg_print("・・？なぜか床に穴が開かない。");
				return NULL;
			}
			else if(dun_level == d_info[dungeon_type].maxdepth)
			{
				msg_print("ここは既にダンジョンの最深層のようだ。");
				return NULL;
			}
			else
			{
				msg_print("あなたは地面に穴を開け、下のフロアへと降りて行った・・");
				p_ptr->leaving = TRUE;
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
			}
			break;
		}
	case 2:
		{
			if(only_info) return format("");
			if (!word_of_recall()) return NULL;
		}
		break;
	case 3:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_YOSHIKA) && (m_ptr->mflag & MFLAG_EPHEMERA) ) cnt++;
			}
			if(cnt)
			{
				msg_format("すでに芳香はこのフロアにいる。",num);
				return NULL;
			}

			(void)summon_named_creature(0, py, px, MON_YOSHIKA, PM_EPHEMERA);


			break;
		}
	case 4:
		{
			monster_type *m_ptr;
			monster_race *r_ptr;

			bool flag_fire = FALSE;
			int i,j;
			int fire_num = p_ptr->lev / 5;
			sides = p_ptr->lev;
			base = adj_general[p_ptr->stat_ind[A_CHR]]+10;
			if(only_info) return format("損傷:(%d+1d%d)* %d",base,sides,fire_num);

			for(j=0;j<fire_num;j++)
			{
				int m_idx = 0;
				int cnt = 0;
				for(i=1;i<m_max;i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					if (is_pet(m_ptr)) continue;
					if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(!monster_living(r_ptr)) continue;

					cnt++;
					if(one_in_(cnt)) m_idx = i;

				}
				if(!m_idx) break;
				else
				{
					flag_fire = TRUE;
					target_who = m_idx;
					m_ptr = &m_list[m_idx];
					target_row = m_ptr->fy;
					target_col = m_ptr->fx;

					fire_ball(GF_NETHER,5,base + randint1(sides),0);
				}

			}
			if(!flag_fire) msg_print("近くには標的になる者がいないようだ。");
			break;
		}
	case 5:
		{
			if (only_info) return "";
			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_STONE_WALL,dir,0,1);

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::椛専用技*/
class_power_type class_power_momizi[] =
{
	{5,10,10,FALSE,FALSE,A_INT,0,0,"周辺感知",
		"周囲の地形を感知する。"},
	{15,20,20,FALSE,FALSE,A_INT,0,0,"全感知",
		"周囲のアイテム、トラップ、階段、ドア、モンスターを感知する。"},
	{24,25,35,FALSE,FALSE,A_STR,0,0,"レイビーズバイト",
		"隣接したモンスター一体にダメージを与える。毒耐性のない敵は混乱、朦朧することがある。"},
	{30,60,90,FALSE,FALSE,A_INT,0,0,"千里眼",
		"フロア全ての地形とアイテムを感知する。"},
	{40,45,75,FALSE,FALSE,A_CHR,0,0,"エクスペリーズカナン",
		"周囲の敵を現在のフロアから追放しようと試みる。一体追放するごとに僅かなダメージを受ける。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_momizi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

	case 0:
		{
			if(only_info) return  format("範囲:%d",DETECT_RAD_MAP);
			map_area(DETECT_RAD_MAP);
			break;
		}
	case 1:
		{
			if(only_info) return  format("範囲:%d",DETECT_RAD_DEFAULT);
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}
	case 2:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = 50 + plev * 2 + adj_general[p_ptr->stat_ind[A_STR]] * 5;

			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("%sに牙を突き立てた！",m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				if(!m_ptr->r_idx) break;

				if(!(r_ptr->flagsr & (RFR_IM_POIS | RFR_RES_ALL)))
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_OLD_CONF,PROJECT_KILL,-1);
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_STUN,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

	case 3:
		{
			if(only_info) return  format("");
			wiz_lite(FALSE);
			break;
		}
	case 4:
		{
			int rad = 3 + plev / 20;
			int power = plev * 8;
			if( power < 200) power = 200;

			if(only_info) return  format("範囲:%d 効力:最大%d",rad,power/2);

			msg_print("あなたは強烈な威圧を放った！");
			project(0,rad,py,px,power,GF_GENOCIDE,(PROJECT_KILL|PROJECT_HIDE),-1);

		}
		break;
	default:
			if(only_info) return  format("未実装");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::こいし専用技*/
class_power_type class_power_koishi[] =
{
	{7,10,10,TRUE,FALSE,A_STR,0,0,"キャッチアンドローズ",
		"近くの敵に攻撃し吹き飛ばす。巨大な敵には効果が薄い。"},
	{15,20,35,FALSE,TRUE,A_CHR,0,0,"スティンギングマインド",
		"周囲のランダムな地点に精神属性の爆発を複数起こす。通常の精神を持たない敵には効果が薄い。"},
	{26,48,45,FALSE,TRUE,A_CHR,0,0,"ご先祖様が見ているぞ",
		"タイムラグ後に視界内のランダムな敵一体に精神攻撃属性ダメージを与える。最大三回発動する。"},
	{33,30,60,FALSE,TRUE,A_DEX,0,0,"コンディションドテレポート",
		"一番近い敵の隣にテレポートし、そのまま一撃を加える。精神を持たない敵は対象にならない。装備が重いと使えない。"},
	{40,50,65,FALSE,TRUE,A_CHR,0,0,"イドの開放",
		"視界内全てに精神攻撃を仕掛け朦朧、混乱、減速させる。通常の精神を持たない敵には効果が薄い。"},
	{48,80,75,FALSE,TRUE,A_CHR,0,0,"胎児の夢",
		"隣接した敵一体に強力な精神攻撃と時間逆転攻撃を仕掛ける。通常の精神を持たない的には効果が薄い。"},

		/*
	{40,70,65,FALSE,TRUE,A_CHR,0,0,"リフレクスレーダー",
		"自分の真東から反時計回りに索敵を行い、精神属性のレーザー攻撃を仕掛ける。"},
		*/


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};


//p_ptr->magic_num1[0]を「ご先祖様が見ているぞ」用に使う
cptr do_cmd_class_power_aux_koishi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
		//キャッチアンドローズ　不可視の電車属性攻撃
	case 0:
		{
			damage = plev * 2;
			if(damage < 20) damage=20;
			if (only_info) return format("損傷:%d",damage);

			project_length = 2;
			if (!get_aim_dir(&dir)) return NULL;
			fire_ball_hide(GF_TRAIN, dir, damage, 0);

			break;
		}
		//スティンギングマインド
	case 1:
		{
			int rad = 2 + plev / 30;
			damage = plev + adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			cast_meteor(damage, rad, GF_REDEYE);

			break;
		}

		//ご先祖様が見ているぞ
	case 2:
		{
			base = 30 + plev + adj_general[p_ptr->stat_ind[A_CHR]] * 2;
			if(only_info) return format("損傷:%d*2(一回あたり)",base);
			msg_print("周囲に透き通った人影が何体も出現した・・");
			p_ptr->magic_num1[0] = 3;
			break;
		}

		//コンディションドテレポート
	case 3:
		{
			monster_type *m_ptr;
			monster_race *r_ptr;

			int dist = 99;
			int range = 25 + plev / 2;
			int tx,ty;
			if(only_info) return  format("距離:%d",range);
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				r_ptr = &r_info[m_ptr->r_idx];
				if (is_pet(m_ptr)) continue;
				if (is_friendly(m_ptr)) continue;
				if(r_ptr->flags2 & RF2_EMPTY_MIND) continue;
				dist_tmp = distance(py,px,m_ptr->fy,m_ptr->fx);
				if(dist_tmp > range) continue;
				if(dist_tmp < dist)
				{
					dist = dist_tmp;
					tx = m_ptr->fx;
					ty = m_ptr->fy;
				}
			}
			if(dist == 99)
			{
				msg_print("誰もあなたの近くにいない・・");
			}
			else
			{
				teleport_player_to(ty,tx,0L);
				if(distance(py,px,ty,tx)==1)
				{
					py_attack(ty,tx,HISSATSU_ATTACK_ONCE);
				}
				else
				{
					msg_print("失敗！");
				}
			}

			break;
		}

		//イドの開放
	case 4:
		{
			base = plev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			sides = plev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("損傷:%d+1d%d",base,sides);
			msg_format("強烈な衝動が開放された！");
			project_hack2(GF_REDEYE,1,sides,base);

			break;
		}
	case 5: //胎児の夢
		{
			int x,y;
			base = chr_adj * 20 + plev * 4 ;
			if(base < 300) base=300;
			if(only_info) return format("最大%d",base);

			if (!get_rep_dir2(&dir)) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if(!in_bounds(y,x)) return NULL;

			if (cave[y][x].m_idx)
			{
				int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				if(!use_itemcard)
					msg_format("あなたは%sに触れ、じっと見つめた・・",m_name);

				project(0, 0, y, x, base / 2 , GF_TIME, flg, -1);
				project(0, 0, y, x, base / 2 , GF_COSMIC_HORROR, flg, -1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}
			break;

		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::アーチャー専用技*/
class_power_type class_power_archer[] =
{
	{5,0,50,FALSE,FALSE,A_DEX,0,0,"矢生成",
		"矢を作る。近くに森林地形がないと作れない。"},
	{10,0,60,FALSE,FALSE,A_DEX,0,0,"ボルト生成",
		"クロスボウの矢を作る。近くに森林地形がないと作れない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_archer(int num, bool only_info)
{
	feature_type *f_ptr;
	int dir;

	switch(num)
	{

	case 0:
	case 1:
		{
			int xx,yy;
			object_type forge;
			object_type *o_ptr = &forge;

			if(only_info) return format("");

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			yy = py + ddy[dir];
			xx = px + ddx[dir];
			if(!cave_have_flag_bold((yy), (xx), FF_TREE))
			{
				msg_format("そこには木がない。");
				return NULL;
			}
			object_prep(o_ptr, (int)lookup_kind((num==0)?TV_ARROW:TV_BOLT, SV_AMMO_LIGHT));
			o_ptr->number = 1 + randint0(p_ptr->lev / 5);
			o_ptr->to_d = p_ptr->lev / 5;
			o_ptr->to_h = p_ptr->lev / 5;
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->discount = 99;
			msg_format("矢を作った。");
			(void)drop_near(o_ptr, -1, py, px);
			if(one_in_(10)) cave_set_feat(yy, xx, one_in_(3) ? feat_brake : feat_grass);

			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::レンジャー専用技*/
class_power_type class_power_ranger[] =
{
	{10,0,10,FALSE,FALSE,A_DEX,0,0,"早持ち替え",
		"通常の半分の行動力消費で装備変更を行う。ただし右手と左手の装備のみ。"},
	{25,16,50,FALSE,FALSE,A_INT,0,0,"調査",
		"視界内の敵の能力について詳細な情報を得る。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_ranger(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			do_cmd_wield(TRUE);
			new_class_power_change_energy_need = 50;
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			msg_format("周囲の敵を調査した・・");
			probing();
			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::探検家専用技*/
class_power_type class_power_rogue[] =
{
	{10,7,20,FALSE,FALSE,A_INT,0,0,"トラップ感知",
		"周辺のトラップを感知する。"},
	{20,15,20,TRUE,FALSE,A_DEX,30,0,"ヒット＆アウェイ",
		"敵に攻撃し、その後一瞬で離脱する。失敗することもある。装備が重いと失敗しやすい。"},
	{30,20,50,FALSE,FALSE,A_INT,0,0,"鑑定",
		"所持しているアイテムを鑑定する。全ての能力を知ることができるわけではない。"},
	{40,27,60,FALSE,FALSE,A_INT,0,0,"調査",
		"視界内の敵の能力について詳細な情報を得る。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_rogue(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool in_water = FALSE;

	if(have_flag(f_ptr->flags, FF_WATER)) in_water = TRUE;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("範囲:%d",DETECT_RAD_DEFAULT);
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			break;
		}
	case 1:
		{
			if(only_info) return format("");
			if(!hit_and_away()) return NULL;
			break;
		}
	case 2:
		{
			if(only_info) return format("");
			if (!ident_spell(FALSE)) return NULL;

			break;
		}
	case 3:
		{
			if(only_info) return format("");
			msg_format("周囲の敵を調査した・・");
			probing();
			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}
/*:::わかさぎ姫専用技*/
class_power_type class_power_wakasagi[] =
{
	{5,7,20,FALSE,FALSE,A_DEX,0,3,"テイルフィンスラップ",
		"水属性のボルトを放つ。レベルが上昇するとボールになる。水上で使用すると威力が上がる。"},
	{16,15,30,FALSE,TRUE,A_STR,0,5,"スケールウェイブ",
		"自分を中心に水属性のボールを発生させる。水上で使用すると威力が上がる。"},
	{25,20,50,FALSE,TRUE,A_CHR,0,0,"スクールオブフィッシュ",
		"周囲の水棲生物を魅了する歌を歌う。歌っている間はMPを消費し続ける。歌っているときにもう一度実行すると歌を止める。" },

	{33,33,50,TRUE,FALSE,A_STR,0,10,"ルナティックレッドスラップ",
		"隣接しているモンスター全てにダメージを与え、混乱耐性を持たない非ユニークモンスターを高確率で混乱させる。" },
	{40,90,80,FALSE,TRUE,A_CHR,0,0,"逆鱗の大荒波",
		"視界内に強力な水属性攻撃を行い周囲の地形を水にする。体力が減っていると威力が上がる。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_wakasagi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool in_water = FALSE;

	if(have_flag(f_ptr->flags, FF_WATER)) in_water = TRUE;

	switch(num)
	{

	case 0:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = 2 + p_ptr->lev / 6;
			sides = 8 ;
			base = p_ptr->lev / 2 + 1;
			if(in_water)
			{
				dice *= 2;
				base *= 2;
			}
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたは水の塊を飛ばした。");
			if(p_ptr->lev < 19) fire_bolt(GF_WATER, dir, base + damroll(dice,sides));
			else  fire_ball(GF_WATER, dir, base + damroll(dice,sides), rad);
			break;

		}
	case 1:
		{
			int rad = 2 + p_ptr->lev / 20;
			base = p_ptr->lev * 4;
			if(base < 60) base = 60;

			if(in_water)
			{
				rad *= 2;
				base *= 2;
			}
			if(only_info) return format("損傷:～%d",base / 2);
			msg_format("あなたは大きく水を跳ね飛ばした。");
			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}



	case 2://v1.1.75 スクールオブフィッシュ
	{
		int range = p_ptr->lev / 3;
		if (only_info) return  format("");


		//歌っているとき行動力コスト無消費で中断
		if (p_ptr->magic_num2[0])
		{
			stop_singing();
			return NULL;

		}

			do_spell(TV_BOOK_MUSIC, MUSIC_NEW_WAKASAGI_SCHOOLOFFISH, SPELL_CAST);

		break;
	}

	case 3: //v1.1.75 ルナティックレッドスラップ
	{
		damage = p_ptr->lev * 4 + adj_general[p_ptr->stat_ind[A_STR]] * 10;

		if (only_info) return format("損傷:%d", damage / 2);

		msg_print("あなたは力任せに尾を振り回した！");
		project(0, 1, py, px, damage, GF_MISSILE, PROJECT_HIDE | PROJECT_KILL, -1);
		project(0, 1, py, px, damage, GF_OLD_CONF, PROJECT_HIDE | PROJECT_KILL, -1);


	}
	break;

	case 4:
		{
			int mult = 1 + ((p_ptr->mhp - p_ptr->chp) * 100 / p_ptr->mhp) / 10;
			int rad = mult;
			base = p_ptr->lev * 2 * mult;
			if(base < 100) base = 100;
			dice = mult;
			if(rad>9) rad=9;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("損傷:%dd%d+%d",dice,sides,base);
			msg_format("あなたは大津波を引き起こした！");
			project(0, rad, py, px, mult+2, GF_WATER_FLOW, PROJECT_GRID, -1);

			project_hack2(GF_WATER,dice,sides,base);

			if(!in_water)
			{
				f_ptr = &f_info[cave[py][px].feat];
				if(have_flag(f_ptr->flags, FF_WATER) && p_ptr->pclass == CLASS_WAKASAGI)
					msg_print("力が湧き出る気がする！");
				p_ptr->update |= PU_BONUS;
				update_stuff();
			}

			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::魔理沙専用技*/
/*:::p_ptr->magic_num2[0-27]を魔法開発済みフラグ、*/
/*:::p_ptr->magic_num1[0-27]を家に作り置いてある魔法の個数、*/
/*:::p_ptr->magic_num2[40-67]を現在所持している魔法の個数に割り当てる。*/
/*:::魔法の開発にはmarisa_magic_power[]を消費する。*/
/*:::0-27はmarisa_magic_table[]の添字に一致する。*/
/*:::p_ptr->tim_general[0]をマジックアブソーバーのカウントに使う。*/
/*:::*/
///mod160103 p_ptr->magic_num1[30-37]をチートコマンド中のmarisa_magic_power[]にまとめることにする
class_power_type class_power_marisa[] =
{
	{1,0,0,FALSE,FALSE,A_INT,0,0,"魔法について確認する",
		"自作の魔法の効果と必要な魔力の種類を確認する。"},
	{1,0,0,FALSE,TRUE,A_INT,0,0,"魔法を使う",
		"自作の魔法を使う。失敗はしない。家から持ってきたものしか使えない。"},
	{1,1,0,FALSE,TRUE,A_INT,0,0,"マジック・ミサイル",
		"無属性のボルトを放つ。"},
	{5,0,20,FALSE,FALSE,A_INT,0,0,"キノコ鑑定",
		"未判明のキノコを鑑定する。"},
	{10,7,25,FALSE,FALSE,A_INT,0,0,"宝の匂い",
		"近くのアイテムを感知する。"},
	{15,8,30,FALSE,TRUE,A_INT,0,3,"イリュージョンレーザー",
		"閃光属性のビームを放つ。"},
	{20,10,50,FALSE,TRUE,A_DEX,0,0,"簡易魔力抽出",
		"素材類から魔法の素となる魔力を抽出する。ただし自宅で行うのに比べて抽出量が落ちる。"},
	{25,12,70,FALSE,FALSE,A_INT,0,0,"目利き",
		"アイテムを一つ鑑定する。レベル40以降は*鑑定*になる。"},
	{30,25,60,FALSE,TRUE,A_INT,0,5,"マジック・ナパーム",
		"魔力属性のロケットを放つ。"},

	{40,50,80,FALSE,TRUE,A_CHR,0,14,"マスタースパーク",
		"核熱属性の強力なビームを放つ。"},
	{45,250,75,FALSE,TRUE,A_DEX,0,0,"着せ替え魔法",
		"その場で探索拠点にアクセスし、アイテム入れ替えや装備の変更ができる。一度でも拠点から出たら終了するので注意。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};

cptr do_cmd_class_power_aux_marisa(int num, bool only_info)
{
	int dir,i;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];


	switch(num)
	{

	case 0:
		{
			if (only_info) return format("");
			check_marisa_recipe();
			return NULL;//確認のみなので行動順消費しない
			break;
		}
	case 1:
		{
			int spell_num;
			if (only_info) return format("");
			spell_num = choose_marisa_magic(CMM_MODE_CAST);
			if(spell_num < 0) return NULL;



			if(!use_marisa_magic(spell_num,FALSE)) return NULL;
			p_ptr->magic_num1[spell_num + MARISA_HOME_CARRY_SHIFT] -= 1;

			break;
		}

	case 2: //マジックミサイル
		{
			int dice = 3 + (plev / 5);
			int sides = 4 + chr_adj / 12;
			if (only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			fire_bolt(GF_MISSILE, dir, damroll(dice, sides));


		}
		break;

	case 3: //キノコ鑑定
		{
			if (only_info) return format("");
			if (!ident_spell_2(1)) return NULL;
		}
		break;
	case 4:
		{
			if(only_info) return "";
			msg_print("あなたはコレクターの勘を働かせた・・");
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}
	case 5:
		{
			int dam = 20 + plev * 2 + chr_adj * 2;
			if (only_info) return format("損傷:%d",dam);
			if (!get_aim_dir(&dir)) return NULL;
			fire_beam(GF_LITE, dir, dam);
			break;
		}
	case 6:
		{
			int rate = plev + 30;
			if (only_info) return format("効率:%d%%",rate);
			if(!marisa_extract_material(FALSE)) return NULL;

			break;
		}
	case 7:
		{
			if(only_info) return format("");
			if(plev < 40)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
			else
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 8: //マジックナパーム 200-400
		{
			int sides = 100 + chr_adj * 5;
			int base = 50+plev * 4;
			if(only_info) return format("損傷:%d+1d%d",base,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("特製の薬瓶を投げつけた。");
			fire_rocket(GF_MANA,dir, base + randint1(sides),3);

			break;
		}

	case 9: //マスパ400-800
		{
			int dice = 15 + p_ptr->lev / 2;
			int sides = 20 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("マスタースパークを放った！");
			fire_spark(GF_NUKE,dir, damroll(dice,sides),1);

			break;
		}
	case 10: //着せ替え魔法
		{
			if(only_info) return format("");

			msg_print("あなたは煙に包まれた・・");
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
			msg_print("煙が晴れるとあなたの装いは様変わりしていた！");

			break;
		}


	default:
		if(only_info) return format("");
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::萃香専用技*/
class_power_type class_power_suika[] =
{
	{3,10,20,FALSE,TRUE,A_DEX,0,0,"アイテム引き寄せ",
		"遠くにあるアイテムを足元に引き寄せる。"},
	{15,25,45,FALSE,FALSE,A_CON,0,5,"火炎のブレス",
		"現在HPの1/3の威力の炎のブレスを吐く。"},
	{20,30,40,FALSE,TRUE,A_WIS,0,0,"ミッシングパワー",
		"一定時間体が大きくなり、腕力と耐久力が増加する。"},
	{26,25,50,FALSE,FALSE,A_STR,0,3,"戸隠山投げ",
		"大岩を投げつけ、無属性ボール攻撃をする。威力は腕力に依存する。"},
	{32,50,60,FALSE,TRUE,A_INT,0,0,"濛々迷霧",
		"一定時間霧に変化する。物理攻撃、切り傷・落石・ボルトが効きにくくなり隠密能力が大幅に上昇する。しかし身体能力が大幅に低下し元素と閃光と劣化の攻撃に弱くなってしまう。"},
	{39,48,65,FALSE,TRUE,A_DEX,0,0,"インプスウォーム",
		"小型の分身を数体呼び出す。今いる階から出ると分身は消える。"},
	{45,80,70,FALSE,TRUE,A_WIS,0,0,"ミッシングパープルパワー",
		"一定時間巨大化する。巨大化中は身体能力が爆発的に上昇するが巻物と魔道具が使用できない。Uコマンドで巨大化を解除できる。"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};



cptr do_cmd_class_power_aux_suika(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			int weight = p_ptr->lev * 30;
			if (only_info) return format("重量:%d",weight);
			if (!get_aim_dir(&dir)) return NULL;
			fetch(dir, weight, FALSE);
			break;
		}
	case 1:
		{
			int dam;
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 1600) dam=1600;

			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			if(use_itemcard)
				msg_print("カードから炎が吐き出された！");
			else
				msg_print("あなたは炎を吐いた！");

			fire_ball(GF_FIRE, dir, dam, (p_ptr->lev > 35 ? -3 : -2));
			break;
		}
	case 2:
		{
			int time;
			int percentage;
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			time = base + randint1(sides);
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			msg_print("体が大きくなった！");
			set_tim_addstat(A_STR,5,time,FALSE);
			set_tim_addstat(A_CON,5,time,FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();
			break;
		}
	case 3:
		{
			int rad = p_ptr->stat_ind[A_STR] / 10;
			dice = p_ptr->lev / 5;
			sides = p_ptr->stat_ind[A_STR]+3;
			base = p_ptr->lev;

			if(rad < 1) rad = 1;
			if(base < 30) base = 30;
			if(dice < 5) dice = 5;

			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			damage = damroll(dice,sides) + base;
			msg_print("あなたは大岩を投げつけた！");
			fire_ball(GF_ARROW,dir,damage,rad);

			break;
		}

	case 4:
		{
			int time;
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			if(!set_mimic(base+randint1(base), MIMIC_MIST, FALSE)) return NULL;
			break;
		}

	case 5:
		{
			bool flag = FALSE;
			int i,num;
			if(only_info) return format("");
			num = 5 + randint0(5);
			msg_print("あなたは髪を数本抜いて吹いた・・");
			for(i=0;i<num;i++) if(summon_named_creature(0, py, px, MON_MINI_SUIKA, PM_EPHEMERA)) flag = TRUE;
			if(!flag) msg_print("しかし何も出てこなかった。");
			break;
		}

	case 6:
		{
			int time;
			int percentage;
			base = p_ptr->lev/2+10;
			sides = 25;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			percentage = p_ptr->chp * 100 / p_ptr->mhp;
			set_mimic(base + randint1(sides), MIMIC_GIGANTIC, FALSE);
			p_ptr->chp = p_ptr->mhp * percentage / 100;
			p_ptr->redraw |= PR_HP;
			redraw_stuff();
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::メイジ・ハイメイジの魔力食い*/
class_power_type class_power_mage[] =
{
	{25,1,70,FALSE,TRUE,A_INT,0,0,"魔力食い",
		"杖・魔法棒・ロッドから魔力を吸い取りMPを回復する。魔道具が壊れることがある。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_mage(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			if(only_info) return format("");
			if (!eat_magic(p_ptr->lev * 2)) return NULL;
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::華扇専用技*/
class_power_type class_power_kasen[] =
{
	{3,3,10,FALSE,TRUE,A_INT,0,0,"光の玉",
		"光る玉を作り出し、現在いる部屋を明るくする。"},
	{8,10,20,FALSE,TRUE,A_DEX,0,0,"アイテム引き寄せ",
		"遠くにあったはずのアイテムを懐から取り出す。"},
	{16,15,40,FALSE,TRUE,A_DEX,0,0,"モンスター引き寄せ",
		"遠くにいたはずのモンスターを懐から取り出す。"},
	{21,30,0,TRUE,FALSE,A_STR,0,0,"大喝",
		"大音声を放って攻撃する。周辺の敵が起きる。"},
	{27,20,30,FALSE,FALSE,A_CHR,0,0,"動物説得",
		"周囲の動物を説得して配下にしようと試みる。"},
	{31,30,55,FALSE,TRUE,A_CHR,0,0,"動物召喚",
		"動物の配下を召喚する。"},
	{37,74,65,TRUE,TRUE,A_STR,0,20,"ドラゴンズグロウル",
		"轟音属性のブレスを放つ。威力はHPの1/3になる。"},
	{45,70,75,FALSE,TRUE,A_INT,0,0,"方術",
		"空間操作の術を使い、周囲の敵が自分を見失いやすくなる。"},



	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_kasen(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	switch(num)
	{

	case 0:
		{
			int dice,sides,base;
			dice=2;sides=1+p_ptr->lev/10;base=2;
			if(only_info) return format("損傷:%dd%d+%d", dice, sides,base);
			msg_print("光球を作り出して設置した。");
			lite_area(damroll(dice, sides)+base, p_ptr->lev / 10 + 1);
			break;
		}
	case 1:
		{
			int weight = p_ptr->lev * 15;
			if (only_info) return format("重量:%d",weight);
			if (!get_aim_dir(&dir)) return NULL;
			fetch(dir, weight, FALSE);
			break;
		}
	case 2:
		{
			int idx;
			monster_type *m_ptr;
			if(only_info) return "";
			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;

			break;
		}
	case 3:
		{
			int dam = p_ptr->lev * 4;
			if(only_info) return format("損傷:～%d",dam/2);
			msg_format("「ば　か　も　の　ー　！！！」");
			project(0, 5 + p_ptr->lev / 15, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			aggravate_monsters(0,FALSE);
			break;
		}
	case 4:
		{
			int power = p_ptr->lev * 2 + adj_general[p_ptr->stat_ind[A_CHR]]*2;
			if(power<50) power = 50;
			if(only_info) return format("効力:%d",power);
			msg_format("あなたは慈悲のあるドヤ顔をした・・");
			charm_animals(power);
			break;
		}
	case 5:
		{
			bool flag = FALSE;
			if(only_info) return "";
			for(i=0;i<p_ptr->lev / 10;i++)
			{
				if ((summon_specific(-1, py, px, p_ptr->lev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))flag = TRUE;
			}
			if(flag)
				msg_print("動物達を呼び出した！");
			else
				msg_print("動物は現れなかった。");

			break;
		}
	case 6:
		{
			int dam;
			dam = p_ptr->chp / 3;
			if(dam<1) dam = 1;
			if(dam > 800) dam=800;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			else msg_print("包帯の腕が大きく膨れ、衝撃波が放たれた！");

			fire_ball(GF_SOUND, dir, dam, -3);
			break;
		}



	case 7:
		{
			dice = base = 15;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			msg_format("空間を操って身を隠した・・");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::小傘専用技*/
class_power_type class_power_kogasa[] =
{
	{1,0,20,FALSE,FALSE,A_STR,0,0,"驚かせる",
		"大声で叫び、隣接した敵一体を驚かせる。寝ている敵には効きやすい。周りの敵も起きる。"},
	{12,20,20,TRUE,FALSE,A_STR,0,0,"フルスイング",
		"傘を振り回し、隣接グリッド全てに攻撃する。"},
	{15,10,30,FALSE,FALSE,A_INT,0,0,"目利き",
		"武具を鑑定する。レベル40以降は武具の能力を完全に知ることができる。"},

	{20,20,30,TRUE,FALSE,A_DEX,0,0,"一本足ピッチャー返し",
		"傘を構え、一定時間自分を狙った矢を打ち返す。"},

	{25,30,25,FALSE,TRUE,A_CHR,0,0,"ハロウフォゴットンワールド",
		"自分を中心に水属性の巨大なボールを発生させる。" },

	{30,50,0,FALSE,FALSE,A_DEX,0,0,"鍛冶",
		"金属製の武具の修正値を強化する。通常の三倍の行動力を消費するので注意。"},
	{35,32,25,FALSE,TRUE,A_INT,0,0,"からかさ驚きフラッシュ",
		"傘から幻惑的な光を放ち、視界内の敵全てを惑わせる。"	},
	{40,55,65,FALSE,TRUE,A_CON,0,0,"ゲリラ台風",
		"視界内全てに対し水と風で攻撃する。"	},

	{44,77,75,FALSE,TRUE,A_CHR,0,0,"オーバー・ザ・レインボー",
		"閃光と水の複合属性の強力なビームを放つ。閃光・混乱耐性のないモンスターを確実に混乱させる。" },


	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_kogasa(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;

	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{

	case 0:
		{
			int power = p_ptr->lev;
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (only_info) return format("効力:%d",power);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int food = 0;
					char m_name[80];

					r_ptr = &r_info[m_ptr->r_idx];

					if(MON_CSLEEP(m_ptr)) power *= 3;
					monster_desc(m_name, m_ptr, 0);
					if(one_in_(2))	msg_format("「うらめしやー！」");
					else			msg_format("「驚けー！！」");
					msg_print("");
					if(r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flagsr & RFR_RES_ALL)
						msg_format("%sは無反応だ。",m_name);
					else if(r_ptr->flags3 & RF3_NO_CONF)
						msg_format("%sは惑わされない。",m_name);
					else if(MON_CONFUSED(m_ptr) || MON_MONFEAR(m_ptr))
						msg_format("%sはすでに度を失っている。",m_name);
					else if(m_ptr->mflag & MFLAG_SPECIAL)
						msg_format("%sはもう驚いてくれなかった。",m_name);
					else if(r_ptr->level > randint1(power) || is_pet(m_ptr))
						msg_format("%sは驚いてくれなかった。",m_name);
					else
					{
						if(r_ptr->flags3 & RF3_HUMAN)
						{
							msg_format("%sは驚いた！お腹が膨れた気がする！",m_name);
							food = 5000;
						}
						else if(r_ptr->flags2 & RF2_WEIRD_MIND)
						{
							msg_format("%sは驚いたらしい。でも全然腹の足しにならない。",m_name);
							food = 100;
						}
						else
						{
							msg_format("%sは驚いた！しかしやはり人間じゃないと美味しくない。",m_name);
							food = 500;
						}
						if(p_ptr->food + food > PY_FOOD_MAX-1) set_food(PY_FOOD_MAX - 1);
						else set_food(p_ptr->food + food);

						(void)set_monster_confused(cave[y][x].m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
						project(0,0,y,x,power,GF_TURN_ALL,PROJECT_KILL,0);
					}
					/*:::同じ敵を何度も驚かせないようにマークをつけておく*/
					m_ptr->mflag |= MFLAG_SPECIAL;
					aggravate_monsters(0,FALSE);
				}
				else
				{
					msg_format("そこには何もいない。");
					return NULL;
				}
			break;

		case 1:
		{
			int rad = 1;
			base =calc_weapon_dam(0);
			if(only_info) return format("損傷:%d",base / 2);
			msg_format("あなたは傘を大きく振り回した！");
			project(0, rad, py, px, base, GF_MISSILE, PROJECT_KILL , -1);
			break;
		}

		case 2:
		{
			if(only_info) return format("");
			if(p_ptr->lev < 40)
			{
				if (!ident_spell(TRUE)) return NULL;
			}
			else
			{
				if (!identify_fully(TRUE)) return NULL;
			}

		}
		break;

		case 3:
		{
			int time;
			base = p_ptr->lev/5+15;
			sides = p_ptr->lev/3+4;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			time = base + randint1(sides);
			msg_format("あなたは傘をたたんで構えた・・");
			set_tim_reflect(time, FALSE);
			break;
		}

		case 4://v1.1.63 ハロウフォゴットンワールド
		{
			int rad = 4 + p_ptr->lev / 15;
			base = p_ptr->lev * 4 + chr_adj * 7;

			if (only_info) return format("損傷:～%d", base / 2);
			msg_format("あなたは水弾をばら撒いた。");
			project(0, rad, py, px, base, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}

		//小傘鍛冶能力
		case 5:
		{
			int     item,amt;
			cptr    q, s;
			object_type *o_ptr;
			object_type forge;
			object_type *q_ptr = &forge;
			int ty,tx;
			int new_val = p_ptr->lev/5 + 5;
			char	o_name[MAX_NLEN];

			if(only_info) return format("強化:%d",new_val);

			item_tester_hook = object_is_metal;

			q = "どれを鍛え直しますか？ ";
			s = "ここにはあなたが鍛えられるものはない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

			if (item >= 0)	o_ptr = &inventory[item];
			else o_ptr = &o_list[0 - item];

			if(!object_is_known(o_ptr))
			{
				msg_print("まず鑑識しないと鍛えられない。");
				return NULL;
			}
			if((object_is_weapon_ammo(o_ptr) && o_ptr->to_h >= new_val && o_ptr->to_d >= new_val
			|| object_is_armour(o_ptr) && o_ptr->to_a >= new_val)
			 && !(object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) ))
			{
				msg_print("それをこれ以上鍛えるのは無理そうだ。");
				return NULL;
			}

			if (o_ptr->number > 1)
			{
				amt = get_quantity(NULL, o_ptr->number);
				if (amt <= 0) return NULL;
			}
			else amt = 1;

			msg_print("あなたは鍛冶道具を取り出した・・");
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			if(o_ptr->number > 1 && o_ptr->number > amt)
			{
				object_copy(q_ptr,o_ptr);
				q_ptr->number = amt;
				if(!kogasa_smith_aux(q_ptr)) return NULL;
				msg_format("%sを鍛え直した！",o_name);
				if (item >= 0)
				{
					inven_item_increase(item, -amt);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -amt);
					floor_item_optimize(0 - item);
				}
				inven_carry(q_ptr);
			}
			else
			{
				if(!kogasa_smith_aux(o_ptr)) return NULL;
				msg_format("%sを鍛え直した！",o_name);
			}

			new_class_power_change_energy_need = 300;
			p_ptr->window |= (PW_INVEN | PW_EQUIP);
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			break;
		}


		case 6:
			{
				int power = p_ptr->lev * 3;
				if (only_info) return format("効力:%d",power);
				msg_format("あなたの傘は突然眩く光った！");
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
				lite_room(py, px);
			}
				break;
		case 7:
			{
				base = p_ptr->lev*2;
				dice = 1;
				sides = 10 + chr_adj * 3;
				if(only_info) return format("損傷:(%dd%d+%d) * 2",dice,sides,base);
				msg_format("あなたは局地的な豪雨と暴風を起こした！");
				project_hack2(GF_WATER,dice,sides,base);
				project_hack2(GF_TORNADO,dice,sides,base);
				break;
			}
			break;
		}


		case 8://v1.1.63 オーバー・ザ・レインボー
		{
			base = p_ptr->lev * 5 / 2 + chr_adj * 8;
			if (only_info) return format("損傷:%d", base);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("ダンジョンに虹が架かった。");
			fire_spark(GF_RAINBOW, dir, base, 2);

			break;
		}



	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}





/*:::小町専用技*/
class_power_type class_power_komachi[] =
{
	{5,3,25,FALSE,FALSE,A_CHR,0,0,"死神の威圧",
		"敵一体を恐怖させる。生者とアンデッドには効果が高い。"	},
	{10,8,35,FALSE,TRUE,A_CHR,0,12,"何処にでもいる浮遊霊",
		"浮遊霊を呼び出し敵にぶつけて地獄属性の攻撃をする。レベルが上がると呼び出す霊が増える。ターゲットはランダムに決まる。"},
	{15,30,35,TRUE,TRUE,A_STR,0,0,"河の流れのように",
		"愛用の船に乗ってターゲットに突撃し、水属性ダメージを与える。"	},
	{20,16,40,FALSE,FALSE,A_INT,0,0,"距離を操る",
		"現在地から通路が通った既知の場所へ移動する。高速移動能力があるときはそれに応じて消費行動力が減る。"	},
	{24,30,55,FALSE,TRUE,A_WIS,0,0,"無間の狭間",
		"隣接した敵一体の移動とテレポートを短時間妨害する。二体以上同時に妨害することはできない。強力な敵には効果が薄い。"},
	{28,30,70,FALSE,TRUE,A_INT,0,0,"脱魂の儀",
		"指定した敵と位置を交換する。視界外の敵にも有効。"},
	{32,32,60,FALSE,FALSE,A_DEX,0,0,"お迎え体験版",
		"指定した敵を自分の隣に引き寄せ、そのまま攻撃する。"},
	{35,27,75,FALSE,TRUE,A_WIS,0,0,"余命幾許も無し",
		"隣接した敵を低確率で一撃で倒し、中確率でHPを半分にする。生者以外には効かず、抵抗されると無効。鎌を装備していないと使えない。"},
	{38,44,70,FALSE,TRUE,A_WIS,0,0,"死者選別の鎌",
		"敵の頭上から光の刃を落とす。アンデッドの場合4倍のダメージを与えて高確率で一撃で倒す。鎌を装備していないと使えない。"},

	{40,50,80,FALSE,FALSE,A_DEX,0,0,"距離を操るⅡ",
		"一グリッド分の壁や扉をすり抜ける。抜けた先にモンスターがいる場合失敗する。テレポート不可地形の中にも入れる。" },

	{43,50,75,FALSE,FALSE,A_STR,0,0,"宵越しの銭",
		"所持金を全て消費し、視界内の全てのモンスターにダメージを与える。"},
	{46,80,90,FALSE,TRUE,A_CHR,0,0,"生魂流離の鎌",
		"直線上の敵に強力な攻撃を仕掛ける。生者に当たった場合中確率で一撃で倒し、さらに大幅に体力を吸収する。鎌を装備していないと使えない。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_komachi(int num, bool only_info)
{
	int dir,dice,sides,base,damage,i;
	int plev = p_ptr->lev;

	switch(num)
	{

		/*:::威圧　生者アンデッド特攻はGF_TURN_ALL処理内で行っている*/
	case 0:
		{
			int power = p_ptr->lev;
			if (only_info) return format("効力:%d～",power);
			if (!get_aim_dir(&dir)) return NULL;
			fear_monster(dir, power);
			break;
		}
		/*:::怨霊弾　ターゲットランダム選定ルーチン初使用　条件判定足りないかもしれない*/
	case 1:
		{
			int rad = 2;
			bool flag = FALSE;
			int num;
			if(p_ptr->lev > 39) rad = 3;
			dice = 1;
			sides = p_ptr->lev;
			base = p_ptr->lev / 2 ;
			num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			if(only_info) return format("損傷:(%d+%dd%d) * %d",base,dice,sides,num);

			if(num<2)msg_format("あなたは浮遊霊を呼び出した。");
			if(num<5)msg_format("あなたは浮遊霊たちを呼び出した。");
			else msg_format("あなたは大量の浮遊霊を呼び出した！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_NETHER, base + damroll(dice,sides),3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}
		/*:::河の流れのように　GF_属性指定した入身ルーチン*/
	case 2:
		{
			int len=p_ptr->lev / 5;
			if(len < 5) len = 5;
			damage = adj_general[p_ptr->stat_ind[A_STR]] * 5 + p_ptr->lev;
			if(damage < 50) damage = 50;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack2(len,GF_WATER,damage)) return NULL;
			break;
		}
		//v1.1.20 視界外でも既知の通路が通っていれば行けることにした
	case 3:
		{
			int x, y;
			int cost;
			int dist = 8 + plev / 4;
			if(only_info) return format("移動コスト:%d",dist);
			if (!tgt_pt(&x, &y)) return NULL;

			if (!player_can_enter(cave[y][x].feat, 0) || !(cave[y][x].info & CAVE_KNOWN))
			{
				msg_print("そこには行けない。");
				return NULL;
			}
			forget_travel_flow();
			travel_flow(y,x);
			if(dist < travel.cost[py][px])
			{
				if(travel.cost[py][px] >= 9999)
					msg_print("そこには道が通っていない。");
				else
					msg_print("そこは遠すぎる。");
				return NULL;
			}

			msg_print("あなたは一瞬で移動した！");
			teleport_player_to(y,x,TELEPORT_NONMAGICAL);

			//高速移動がある時移動と同じように消費行動力が減少する
			if(p_ptr->speedster)
				new_class_power_change_energy_need = (75-p_ptr->lev/2);

			break;
		}
/*::: -Hack- 無間の狭間　magic_num1[0]とtim_general[0]を使用する*/
	case 4:
		{
			int y, x;
			monster_type *m_ptr;

			base = 5 + p_ptr->lev / 10;
			if(only_info) return format("期間:%d",base);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					msg_format("%sは狭間に囚われた！",m_name);
					p_ptr->magic_num1[0] = cave[y][x].m_idx;
					set_tim_general(base,FALSE,0,0);
				}
				else
				{
					msg_format("そこには何もいない。");
					return NULL;
				}

			break;
		}
		/*:::脱魂の儀　位置交換*/
	case 5:
		{
			bool result;
			if(only_info) return "";

			/* HACK -- No range limit */
			project_length = -1;
			result = get_aim_dir(&dir);
			/* Restore range to default */
			project_length = 0;
			if (!result) return NULL;
			teleport_swap(dir);
			break;
		}
		/*:::お迎え体験版　テレポート・バックを独立ルーチン化　テレポバック適用後ターゲットが隣にきてたら通常攻撃*/
	case 6:
		{
			int idx;
			monster_type *m_ptr;
			if(only_info) return "";

			if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return NULL;
			m_ptr = &m_list[idx];
			if(distance(py,px,m_ptr->fy,m_ptr->fx) > 1)	msg_format("うまく隣に来なかった。");
			else py_attack(m_ptr->fy,m_ptr->fx,0);

			break;
		}
		/*:::余命幾許も無し　アンデッド、デーモン、無生物、一部神格には即死無効　即死確率は毒針と同じにした　HP半減確率はやや高め　ユニークには効きにくい*/
	case 7:
		{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			damage = 0;
			if(only_info) return "";
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				int bonus = 25;
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(use_itemcard) bonus = 50;

				msg_format("あなたは大きく鎌を振り下ろした。");
				if(!monster_living(r_ptr) || r_ptr->flags1 & RF1_QUESTOR)
				{
					msg_format("%sには全く効果がないようだ。",m_name);
					break;
				}

				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < bonus + p_ptr->lev/2)
					{
						msg_format("鎌が%sに直撃した！",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
					else
						msg_format("鎌は空を切った。",m_name);
				}
				else
				{
					if(randint1(randint1(r_ptr->level/7)+5) == 1)
					{
						msg_format("鎌が%sの急所を貫いた！",m_name);
						damage = m_ptr->hp + 1;
					}
					else if(randint1(r_ptr->level * 3 ) < bonus + p_ptr->lev / 2)
					{
						msg_format("鎌が%sに直撃した！",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
					else
						msg_format("鎌は空を切った。",m_name);
				}
				if(damage>0)
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_MISSILE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
		/*:::死者選別の鎌　通常攻撃/2のDISP_ALL アンデッドにダメージ4倍、非ユニークアンデッドに高確率即死*/
		//v1.1.47 アンデッドにダメージ2倍→4倍にした
	case 8:
		{
			int y, x;
			int dist;
			monster_type *m_ptr;
			monster_race *r_ptr;

			dist = (p_ptr->lev > 44) ? 3 : 2;
			damage=0;
			//鎌を持っている手しかダメージに算入しない。両手に武器(鎌以外含む)を持っている場合ダメージ2/3
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(1);
			if(p_ptr->migite && p_ptr->hidarite) damage = damage * 2 / 3;
			damage /= 2;

			if(only_info) return  format("射程:%d 損傷:%d",dist,damage);
			if(damage<1) damage = 1;
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;

			y = target_row;
			x = target_col;
			m_ptr = &m_list[target_who];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたは大きく鎌を振り下ろした。天から光が降り注いだ！");
				if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && randint1(r_ptr->level * 3 ) < p_ptr->lev)
				{
					msg_format("%sは光の中に消えた。",m_name);
					damage = m_ptr->hp + 1;
				}
				else if(r_ptr->flags3 & RF3_UNDEAD)
				{
					msg_format("%sは甚大な損傷を受けた。",m_name);
					damage *= 4;
				}
				project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

		//v1.1.65 距離を操るⅡ
	case 9:
	{
		int x1, y1, x2, y2;
		bool flag_can_move = FALSE;

		if (only_info) return  format("距離:1グリッド");

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;

		y1 = py + ddy[dir];
		x1 = px + ddx[dir];
		y2 = y1 + ddy[dir];
		x2 = x1 + ddx[dir];

		//画面端に対しては使えない
		if (!in_bounds(y1, x1) || !in_bounds(y2, x2))
		{
			msg_print("そちらの方には進めない。");
			return NULL;
		}

		//まず隣接グリッドが閉じたドアか壁であることを確認
		if (!cave_have_flag_bold(y1, x1, FF_MOVE) && (cave_have_flag_bold(y1, x1, FF_DOOR) || cave_have_flag_bold(y1, x1, FF_WALL)))
		{
			//次に2グリッド目に侵入可能であることを確認
			if (player_can_enter(cave[y2][x2].feat, 0))
			{
				flag_can_move = TRUE;
			}
		}
		if (flag_can_move)
		{
			if (cave[y1][x1].m_idx || cave[y2][x2].m_idx)
			{
				msg_print("何かに邪魔された。");
				flag_can_move = FALSE;
			}

		}
		else
		{
			msg_print("その場所はこの能力の対象にはならない。");

		}


		if (flag_can_move)
		{

			if(one_in_(10))msg_print("「阿頼耶識を以って虚空を太空と為せ！」");
			else if (one_in_(9))msg_print("「この様な障壁で阻まれてしまうくらいでは仕事にならないんでねぇ」");
			else msg_print("あなたは壁の厚さを操作してすり抜けた。");

			move_player_effect(y2, x2, (MPE_FORGET_FLOW | MPE_DONT_PICKUP));
		}
	}
	break;

	case 10: //v1.1.20 宵越しの銭
		{
			int dam = p_ptr->au / 666;
			if(dam > 5000) dam = 5000;
			if(only_info) return  format("損傷:%d",dam);

			if(dam < 1)
			{
				msg_format("あなたはすでに文無しだ。");
				return NULL;
			}
			if (!get_check_strict("所持金が0になります。よろしいですか？ ", CHECK_OKAY_CANCEL)) return NULL;

			msg_print("あなたは気前よく有り金全てを投げうった！");
			project_hack2(GF_ARROW,0,0,dam);
			p_ptr->au = 0;
			p_ptr->redraw |= PR_GOLD;

		}
		break;
		/*:::生魂流離の鎌*/
	case 11:
		{
			int dist = 5; //v1.1.47 ちょっと強化
			damage=0;
			//鎌を持っている手しかダメージに算入しない。両手に武器(鎌以外含む)を持っている場合ダメージ2/3
			if(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING)) damage += calc_weapon_dam(1);
			if(p_ptr->migite && p_ptr->hidarite) damage = damage * 2 / 3;
			dice = 1;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * p_ptr->lev / 5;
			if(only_info) return  format("射程:%d 損傷:%d+%dd%d",dist,damage,dice,sides);
			if(damage < 1) damage = 1;
			project_length = dist;
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたは大きく鎌を振り上げた。地面から無数の刃が突き出した！");
			fire_beam(GF_RYUURI, dir, damage + damroll(dice,sides));

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::衣玖専用技*/
class_power_type class_power_iku[] =
{
	{10,12,25,FALSE,TRUE,A_CHR,0,0,"龍神の怒り",
		"指定したターゲットに雷を落として攻撃する。"	},
	{20,30,35,FALSE,TRUE,A_CHR,0,0,"雷雲棘魚",
		"一定時間トゲ状のオーラをまとい攻撃してきた敵に反撃する。破片耐性を持つ敵には無効。"	},
	{25,32,45,FALSE,TRUE,A_INT,0,0,"龍神の一撃",
		"装備している武器に雷をまとわせ、一時的に電撃属性を持たせる。"	},
	{30,40,55,FALSE,FALSE,A_DEX,30,0,"羽衣は空の如く",
		"敵の様々な攻撃を受け流すべく身構え、一時的に酸電火冷の耐性とAC上昇を得る。装備が重いと失敗しやすい。"	},
	{35,40,45,FALSE,TRUE,A_CHR,0,0,"エレキテルの龍宮",
		"自分を中心とした強力な電撃のボールを発生させる。"},
	{40,55,65,FALSE,TRUE,A_CHR,0,10,"龍宮の使い遊泳弾",
		"強力な電撃のボールを大量に放つ。ターゲットはランダムに決まる。"},
	{45,85,70,FALSE,TRUE,A_CHR,0,0,"玄雲海の雷庭",
		"視界内の全てを電撃で攻撃する。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_iku(int num, bool only_info)
{
	int dir,dice,sides,base;
	bool flag_storm = FALSE;


	switch(num)
	{

	case 0:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = 2 + p_ptr->lev / 5;
			sides = 10 ;
			base = p_ptr->lev ;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			if(!fire_ball_jump(GF_ELEC, dir, base + damroll(dice,sides), rad,"あなたは天に向けて指を差した。")) return NULL;
			break;
		}
	case 1:
		{
			if(only_info) return format("期間:20+1d20");
			msg_format("トゲのようなオーラをまとった。");
			  set_dustrobe(20+randint1(20),FALSE);
			break;
		}

	case 2:
		{
			base = p_ptr->lev/2;
			sides = p_ptr->lev/2;
			if(only_info) return format("期間:%d+1d%d",base,sides);
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_print("武器を持っていないと効果がない。");
				return NULL;//paranoia
			}

			msg_format("手にしている武器から火花が散った！");
			set_ele_attack(ATTACK_ELEC, base + randint1(sides));
			break;
		}
	case 3:
		{
			int time;
			base = p_ptr->lev/2+10;
			sides = p_ptr->lev/2;
			if(sides < 10) sides = 10;

			if(only_info) return format("期間:%d+1d%d",base,sides);
			time = base + randint1(sides);
			msg_format("あなたは気流を身に纏った・・");
			set_shield(time, FALSE);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			break;
		}

	case 4:
		{
			int rad = 5;
			base = p_ptr->lev * 10 + adj_general[p_ptr->stat_ind[A_CHR]] * 10;
			if(only_info) return format("損傷:～%d",base / 2);
			msg_format("あなたは天に向けて指を差した。");
			project(0, rad, py, px, base, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
			break;
		}

	case 5:
		{
			int rad = 1;
			bool flag = FALSE;
			int num;
			int i;
			dice = 1;
			sides = p_ptr->lev;
			base = p_ptr->lev;
			num = p_ptr->lev / 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 5;
			if(only_info) return format("損傷:(%d+%dd%d) * %d",base,dice,sides,num);

			else msg_format("あなたは大量の雷球を放った！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_ELEC, base + damroll(dice,sides),3, rad,0))flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}
			break;
		}
	case 6:
		{
			dice = p_ptr->lev / 5;
			sides = adj_general[p_ptr->stat_ind[A_CHR]] * 3;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			msg_format("あなたは宙に浮いて両手を広げた・・");
			project_hack2(GF_ELEC,dice,sides,0);
			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}




/*:::うどんげ専用技*/
class_power_type class_power_udonge[] =
{
	{5,10,25,FALSE,FALSE,A_CHR,0,0,"マインドシェイカー",
		"視界内のモンスター一体を睨んで精神を攻撃し混乱させる。レベル30以降でさらに朦朧と減速を与える。特異な精神を持つ敵やユニークモンスターには効果が薄い。"	},
	{12,20,55,FALSE,FALSE,A_INT,0,0,"波長分析",
		"周囲にあるものを感知する。レベルが上がると感知できるものの種類と範囲が増える。"	},
	{16,18,30,FALSE,TRUE,A_WIS,0,0,"イリュージョナリィブラスト",
		"半物理半精神攻撃のレーザーを放つ。精神の希薄な敵や狂気をもたらす敵、ユニークモンスターには効果が薄い。"	},
	{20,30,75,FALSE,FALSE,A_INT,0,0,"波長診断",
		"視界内のモンスターの波長を読み取って能力やパラメータなどを調査する。"	},
	{25,40,80,FALSE,FALSE,A_CHR,0,0,"マインドストッパー",
		"視界内のモンスター全てを睨んで動きを停止させる。"	},
	{30,36,50,FALSE,TRUE,A_WIS,0,0,"マインドエクスプロージョン",
		"半物理半精神攻撃のロケットを放つ。精神の希薄な敵や狂気をもたらす敵、ユニークモンスターには効果が薄い。"	},
	{35,33,50,TRUE,TRUE,A_CON,0,0,"国士無双の薬",
		"永琳印の強化薬を服用する。飲み過ぎると・・"	},
	{40,56,70,FALSE,TRUE,A_INT,0,0,"ビジョナリチューニング",
		"ごく一時的に周囲の様々な波長を狂わせ、敵が自分を見失いやすくする。"	},
	{45,65,75,FALSE,TRUE,A_WIS,0,0,"アイサイトクリーニング",
		"広範囲に対し半物理半精神攻撃を行う。精神の希薄な敵や狂気をもたらす敵、ユニークモンスターには効果が薄い。"	},
	{48,85,90,FALSE,TRUE,A_INT,0,0,"アキュラースペクトル",
		"ごく一時的に虚像とともに行動し、敵からの攻撃を受けにくくする。"	},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_udonge(int num, bool only_info)
{
	int dir,dice,sides,base,damage;
	switch(num)
	{
	case 0:
		{
			dice = p_ptr->lev / 5;
			sides = 7 + p_ptr->lev / 10;
			base = p_ptr->lev ;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			damage = damroll(dice,sides) + base;
			if(p_ptr->lev < 30) fire_ball_hide(GF_MIND_BLAST, dir, damage, 0);
			else				fire_ball_hide(GF_BRAIN_SMASH, dir, damage, 0);

			break;
		}
	case 1:
		{
			int rad = p_ptr->lev / 2 + 10;
			if(only_info) return format("範囲:%d",rad);

			if(use_itemcard)
				msg_format("周囲の色々なことが分かった気がする...");
			else
				msg_format("周囲の波長を分析した・・");
			detect_doors(rad);
			if(p_ptr->lev > 4) detect_monsters_normal(rad);
			if(p_ptr->lev > 9) detect_traps(rad, TRUE);
			if(p_ptr->lev < 15)detect_stairs(rad);
			else map_area(rad);
			if(p_ptr->lev > 19) detect_objects_normal(rad);
			break;
		}
	case 2:
		{
			dice = p_ptr->lev / 5;
			sides = 5 + p_ptr->lev / 10;
			base = p_ptr->lev + 10;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたは目からビームを発射した！");
			fire_beam(GF_REDEYE, dir, base + damroll(dice,sides));
			break;
		}
	case 3:
		{
			if(only_info) return "";

			msg_format("周囲の存在から放たれる波長を読み取った。");
			probing();
			break;
		}
	case 4:
		{
			int power = p_ptr->lev * 4;
			if(power < 70) power = 70;
			if(only_info) return format("効力:%d",power);

			if(use_itemcard)
				msg_format("カードが赤く光った！");
			else
				msg_format("あなたの目が赤く光った！");
			stasis_monsters(power);
			break;
		}
	case 5:
		{
			int rad = 2;
			if(p_ptr->lev > 39) rad = 3;
			dice = p_ptr->lev / 4;
			sides = 10 + p_ptr->lev / 10;
			base = p_ptr->lev * 2;
			if(only_info) return format("損傷:%d+%dd%d",base,dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_format("巨大な幻影の弾丸を発射した！");
			fire_rocket(GF_REDEYE, dir, base + damroll(dice,sides), rad);
			break;
		}
	case 6:
		{
			dice = base = 25;
			if(only_info) return format("期間:%d+1d%d",base,dice);

			msg_format("薬を服用した。");
			if(!p_ptr->hero)
			{
				set_afraid(0);
				set_hero(randint1(dice) + base, FALSE);
			}
			else if(!p_ptr->shield) set_shield(randint1(dice) + base, FALSE);
			//else if(!p_ptr->fast) set_fast(randint1(dice) + base, FALSE);
			else if(!p_ptr->tim_addstat_count[A_STR] || !p_ptr->tim_addstat_count[A_DEX] || !p_ptr->tim_addstat_count[A_CON])
			{
				int percentage = p_ptr->chp * 100 / p_ptr->mhp;
				int v = randint1(dice) + base;
				msg_format("強大な力が湧き出してきた！");
				set_tim_addstat(A_STR,105,v,FALSE);
				set_tim_addstat(A_DEX,105,v,FALSE);
				set_tim_addstat(A_CON,105,v,FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();
			}
			else
			{
				//int dam = p_ptr->chp * 2 / 3;
				int dam = p_ptr->mhp / 2;
				if(dam<1) dam=1;

				if(p_ptr->pclass == CLASS_EIRIN)
				{
					msg_print("...やはりやめておいた。これ以上の服用は危険だ。");
					return NULL;
				}

				msg_format("あなたは大爆発した！");
				project(0, 7, py, px, dam * 5 + randint1(dam*5), GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
				take_hit(DAMAGE_LOSELIFE, dam, "師匠の薬", -1);
				set_hero(0,TRUE);
				set_shield(0,TRUE);
				set_tim_addstat(A_STR,0,0,TRUE);
				set_tim_addstat(A_DEX,0,0,TRUE);
				set_tim_addstat(A_CON,0,0,TRUE);
			}

			break;
		}

	case 7:
		{
			dice = base = 15;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			msg_format("周囲の波長を狂わせた！");
			set_tim_superstealth(base + randint1(dice),FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}
	case 8:
		{
			damage = p_ptr->lev * 5 + adj_general[p_ptr->stat_ind[A_WIS]] * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			if(only_info) return format("損傷:%d",damage);
			if (!get_aim_dir(&dir)) return NULL;
			msg_format("あなたの視界は赤く染まった！");
			fire_spark(GF_REDEYE,dir,damage,3);

			break;
		}

	case 9:
		{
			dice = base = 6;
			if(only_info) return format("期間:%d+1d%d",base,dice);
			if(use_itemcard)
				msg_format("自分の存在する位相がずれた気がする...");
			else
				msg_format("あなたは自分の位相をずらした。");
			set_multishadow(base + randint1(dice),FALSE);
			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}


/*:::てゐ専用技*/
class_power_type class_power_tewi[] =
{

	{10,7,25,FALSE,FALSE,A_DEX,0,0,"落とし穴",
		"足元に落とし穴を掘る。誰も見ていない場所でないと実行できない。浮遊していない敵は落とし穴に落ちて気絶、朦朧、行動遅延することがある。"	},
	{20,12,25,FALSE,FALSE,A_INT,0,0,"爆発トラップ",
		"敵が通ったら爆発するルーンを床に仕掛ける。"	},
	{27,20,30,TRUE,FALSE,A_DEX,30,0,"フラスターエスケープ",
		"通常攻撃を行い、その後一瞬で離脱する。離脱に失敗することもある。"	},
	{32,30,65,FALSE,FALSE,A_WIS,0,0,"大穴牟遅様の薬",
		"神代の止血薬により、HPを回復させて切り傷を治す。"	},
	{38,45,70,FALSE,FALSE,A_INT,0,0,"レーザートラップ",
		"このフロア限定でレーザーを放つ魔法陣を設置する。"	},
	{44,64,80,FALSE,TRUE,A_CHR,0,0,"エンシェントデューパー",
		"視界内のモンスター全てを友好的にする。ただしこの特技の発動に失敗したときフロアのモンスター全てが激怒する。クエストの討伐対象モンスターには効果がない。"	},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_tewi(int num, bool only_info)
{
	int dir,dice,sides,base;
	switch(num)
	{
	case 0:
		{
			int i;
			if(only_info) return "";
			if(!cave_clean_bold(py,px))
			{
				msg_print("ここは掘れない。");
				return NULL;
			}

			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				monster_type *m_ptr = &m_list[i];
				if(!m_ptr->r_idx) continue;
				if (!player_has_los_bold(m_ptr->fy,m_ptr->fx)) continue;
				if(r_info[m_ptr->r_idx].flags2 & RF2_EMPTY_MIND) continue;
				if(MON_CONFUSED(m_ptr)) continue;
				if(MON_CSLEEP(m_ptr)) continue;

				msg_print("今は掘れない。誰かに見られている。");
				return NULL;
			}


			cave_set_feat(py, px, f_tag_to_index_in_init("TEWI_PIT"));
			msg_print("落とし穴を掘った！");

		}
		break;
	case 1:
		{
			if(only_info) return format("トラップ威力:%d+7d7",p_ptr->lev);
			msg_format("足元に罠を仕掛けた。");
			explosive_rune();
			break;
		}
	case 2:
		{
			if(only_info) return "";
			if(!hit_and_away()) return NULL;
			break;
		}

	case 3:
		{
			int heal = (adj_general[p_ptr->stat_ind[A_WIS]]/2 + 15) * (5+p_ptr->lev / 10);
			if(heal < 50) heal = 50;
			if(only_info) return format("回復:%d",heal);
			msg_print("ダイコク様の優しさに癒やされる...");
			hp_player(heal);
			set_cut(0);
			break;
		}
	case 4:
		{
			if(only_info) return format("");

			if(summon_named_creature(0,py,px,MON_SIGN_L,PM_EPHEMERA))
				msg_print("トラップを設置した。");
			else
				msg_print("設置に失敗した。");

		}
		break;

	case 5:
		{
			int i, count=0;
			monster_type *m_ptr;
			if(only_info) return "";


			if(one_in_(4))
				msg_format("あなたは一瀉千里の名口上を披露した！");
			else if(one_in_(3))
				msg_format("あなたは口から出任せをまくし立てた！");
			else if(one_in_(2))
				msg_format("あなたは壮大なハッタリをぶち上げた！");
			else
				msg_format("あなたは一世一代の啖呵を切った！");

			for (i = m_max - 1; i >= 1; i--)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) continue;
				if (!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
				if (!is_hostile(m_ptr)) continue;
				set_friendly(m_ptr);
				count++;
			}
			if(count)
				msg_format("モンスターたちは友好的になった！");
			else
				msg_format("...しかし相手がいなかった。");

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::妖夢専用技*/
//v1.1.30 色々調整
//tim_gen[0]を「修羅の血」に使用
class_power_type class_power_youmu[] =
{
	{12,21,25,TRUE,FALSE,A_DEX,50,5,"現世斬",
		"ターゲットの隣まで一瞬で移動し、そのまま攻撃する。装備が重いと失敗しやすい。"	},
	{17,24,35,FALSE,TRUE,A_WIS,0,0,"成仏得脱斬",
		"隣接したターゲットへフォース属性の攻撃を行い、アンデッドを低確率で一撃で倒す。白楼剣を装備していないと使えない。"	},
	{22,32,40,FALSE,TRUE,A_STR,0,0,"迷津慈航斬",
		"隣接したターゲットへ地獄属性の攻撃を行い、地獄耐性を持たない生物を低確率で一撃で倒す。楼観剣を装備していないと使えない。"},
	{25,50,50,TRUE,TRUE,A_STR,0,20,"修羅の血",
		"一定時間加速し、さらにモンスターを倒したときにMPが回復するようになる。"	},
	{28,22,55,FALSE,FALSE,A_STR,0,15,"地獄極楽滅多斬り",
		"近くのランダムなモンスターに無属性ビームを複数回放つ。刀を持っていないと使えず二刀流だと回数が倍になる。"	},
	{33,50,60,FALSE,TRUE,A_CHR,0,0,"天人の五衰",
		"自分の周辺三マスに対し、地獄の業火属性の攻撃を放つ。刀を装備していないと使えない。"	},
	{37,56,65,TRUE,FALSE,A_DEX,0,15,"未来永劫斬",
		"ターゲットの隣まで一瞬で移動し、かわされるまで攻撃し続ける。装備が軽い場合攻撃命中率が大幅に上昇する。"},
	{40,64,70,FALSE,TRUE,A_WIS,0,0,"幽明求聞持聡明の法",
		"自らの分身を出す。分身は階移動するか本体との距離が離れすぎると消滅する。分身の攻撃により敵を倒した場合も経験値とアイテムを得られる。"	},
	{43,72,75,FALSE,TRUE,A_DEX,0,0,"六根清浄斬",
		"視界内の全てのモンスターに通常攻撃を行う。オーラダメージを受けない。刀を装備していないと使えない。"	},
	{47,96,80,FALSE,TRUE,A_CHR,0,0,"一念無量劫",
		"自分を中心とした半径五マスに対し、極めて強力な攻撃を放つ。攻撃の属性は地獄、カオス、空間歪曲、地獄の業火、無属性、破邪のいずれかになる。刀を二本装備していないと使えない。"	},
	{99,0,0,FALSE,FALSE,0,0,0,"dummy",	""},
};
cptr do_cmd_class_power_aux_youmu(int num, bool only_info)
{
	int dir,dice,sides,base;
	int plev = p_ptr->lev;
	switch(num)
	{
	case 0:
		{
			int len = plev / 5;
			if(only_info) return format("射程:%d",len);
			if(p_ptr->do_martialarts)
			{
				msg_print("武器を持たないとこの技は使えない。");
				return NULL;
			}
			if (!rush_attack(NULL,len,0)) return NULL;
			break;
		}
	case 1:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage = (calc_weapon_dam(0) + calc_weapon_dam(1));
			//v1.1.30 二刀流だとちょっと強すぎるので弱体化
			if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx)
				damage = damage * 2 / 3;

			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;


			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("桜色の光の柱が立ち昇った！");

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && r_ptr->flags3 & RF3_UNDEAD)
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < p_ptr->lev)
					{
						msg_format("%sは光の中に消えた。",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
				}
				if(damage>0)
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_FORCE,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}
	case 2:
	{
			int y, x;
			monster_type *m_ptr;
			monster_race *r_ptr;

			int damage=0;
			if(inventory[INVEN_RARM].name1 == ART_ROUKANKEN) damage = calc_weapon_dam(0) * 2;
			else if(inventory[INVEN_LARM].name1 == ART_ROUKANKEN) damage = calc_weapon_dam(1) * 2;

			if(only_info) return format("損傷:%d",damage);
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				msg_format("あなたは剣を大きく振り下ろした！");

				if(!(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2) && monster_living(r_ptr) && !(r_ptr->flagsr & RFR_RES_NETH))
				{
					if((randint1(randint1(r_ptr->level/7)+5) == 1) && randint1(r_ptr->level * 4) < p_ptr->lev)
					{
						msg_format("楼観剣の刃が%sに直撃した！",m_name);
						damage = m_ptr->hp / 2 + 1;
					}
				}
				if(damage>0)
				{
					project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_NETHER,PROJECT_KILL,-1);
				}

			}
			else
			{
				msg_format("そこには何もいない。");
				return NULL;
			}

			break;
		}

		//v1.1.30追加
	case 3:
		{
			int time = plev / 2;
			if(only_info) return format("期間:%d",time);
			set_tim_general(time,FALSE,0,0);
			set_fast(time,FALSE);
		}
		break;

		//v1.1.30追加
	case 4:
		{
			bool flag = FALSE;
			int i;
			int num = 0;
			int dam = 0;
			int range = plev / 12;

			if(inventory[INVEN_RARM].tval == TV_KATANA)
			{
				num += plev / 15;
				dam += calc_weapon_dam(0) / p_ptr->num_blow[0];
			}
			if(inventory[INVEN_LARM].tval == TV_KATANA)
			{
				num += (plev - 5) / 15;
				if(dam)
					dam = (dam + calc_weapon_dam(1) / p_ptr->num_blow[1]) / 2;
				else
					dam = calc_weapon_dam(1) / p_ptr->num_blow[1];
			}
			num += 1;
			if(dam<1) dam = 1;

			if(only_info) return format("射程:%d 損傷:%d * %d",range,dam,num);
			msg_print("当たるを幸いに斬りまくった！");
			for(i=0;i<num;i++)
			{
				if(fire_random_target(GF_MISSILE, dam,2, 0,range)) flag = TRUE;
				if(i==0 && !flag)
				{
					msg_format("しかし敵が見当たらなかった。");
					break;
				}
			}

		}
		break;
	case 5:
		{
			int dam = 0;

			if(inventory[INVEN_RARM].tval == TV_KATANA)
				dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA)
				dam += calc_weapon_dam(1);

			//v1.1.30 二刀流だとちょっと強すぎるので弱体化
			if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx)
				dam = dam * 2 / 3;

			dam = dam * (20 + adj_general[p_ptr->stat_ind[A_CHR]]*2/3 ) / 20;
			if(only_info) return format("損傷:～%d",dam/2);
			project(0, (cave_have_flag_bold(py, px, FF_PROJECT) ? 3 : 0), py, px, dam, GF_HELL_FIRE, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);

			break;
		}

	case 6:
		{
			int len = p_ptr->lev / 10;
			if(only_info) return format("射程:%d",len);
			if(p_ptr->do_martialarts)
			{
				msg_print("武器を持たないとこの技は使えない。");
				return NULL;
			}
			if (!rush_attack(NULL,len, HISSATSU_EIGOU)) return NULL;
			break;
		}

		//EPHEMERAフラグ付きのユニークを分身として出す。ユニーク出現可能数を無視する。
	case 7:
		{
			int i,cnt=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if(only_info) return "";
			for (i = 1; i < m_max; i++)
			{
				int dist_tmp;
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if((m_ptr->r_idx == MON_YOUMU) && (m_ptr->mflag & MFLAG_EPHEMERA)) cnt++;
			}
			if(cnt)
			{
				msg_format("すでに術を実行している。",num);
				return NULL;
			}

			if(summon_named_creature(0, py, px, MON_YOUMU, PM_EPHEMERA))
				msg_print("あなたの半霊が実体化した！");
			else
				msg_print("術に失敗した。");


			break;
		}
		//v1.1.30追加
	case 8:
		{
			if(only_info) return format("");
			flag_friendly_attack = TRUE;
			msg_format("無数の分身が辺りに散った..");
			project_hack2(GF_YOUMU,0,0,100);
			flag_friendly_attack = FALSE;
		}
		break;

	case 9:
		{
			int dam=0;
			int typ[6] = {GF_NETHER, GF_CHAOS, GF_DISP_ALL, GF_HOLY_FIRE, GF_DISTORTION, GF_HELL_FIRE};
			if(inventory[INVEN_RARM].tval == TV_KATANA) dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA) dam += calc_weapon_dam(1);
			if(dam < 400) dam = 400;
			dam = dam * (60 + adj_general[p_ptr->stat_ind[A_CHR]]*2) / 30;
			if(only_info) return format("損傷:～%d",dam/2);
			project(0, (cave_have_flag_bold(py, px, FF_PROJECT) ? 5 : 0), py, px, dam, typ[randint0(6)], PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);

			break;
		}

	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}

/*:::ルーミア用特技*/
class_power_type class_power_rumia[] =
{
	{6,3,20,FALSE,TRUE,A_CHR,0,1,"ナイトバード",
		"暗黒属性のボルトを放つ。"	},
	{17,11,25,FALSE,TRUE,A_CHR,0,7,"ムーンライトレイ",
		"ターゲット周辺に暗黒属性のレーザーを複数放つ。ちゃんとターゲットにも当たる。"	},
	{25,20,35,FALSE,TRUE,A_WIS,0,0,"闇生成",
		"一時的に濃密な闇をまとい、ACと魔法防御を上昇させて閃光攻撃への耐性を得る。また周囲2グリッドのモンスターが時々あなたを見失う。ただしあなたも周囲が見えなくなる。"	},
	{25,0,0,FALSE,TRUE,A_WIS,0,0,"闇解除",
		"闇生成を解除する。通常の半分程度の時間しか消費しない。"	},
	{32,35,45,FALSE,TRUE,A_CON,0,0,"ディマーケイション",
		"視界内全てに暗黒攻撃を行い、さらに現在いる部屋を暗くする。"},
	{36,45,60,FALSE,TRUE,A_CHR,0,0,"ミッドナイトバード",
		"巨大な暗黒属性ボールを放つ。"},
	{40,80,85,FALSE,TRUE,A_INT,10,0,"ダークサイドオブザムーン",
		"暗闇と同化し、攻撃を受けた時のダメージを半減させる。光や破邪の攻撃を受けると大ダメージを受け効果が消える。「闇生成」は中断される。"},

	{99,0,0,FALSE,FALSE,0,0,0,"dummy",
		""},
};
cptr do_cmd_class_power_aux_rumia(int num, bool only_info)
{
	int plev = p_ptr->lev;
	int dir,dice,sides,base;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	switch(num)
	{
	case 0: //ナイトバード
		{
			dice = 3 + plev / 6;
			sides = 5 + chr_adj / 5;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			if (!get_aim_dir(&dir)) return NULL;
			fire_bolt(GF_DARK, dir, damroll(dice, sides));
			break;

		}
	case 1:
		{
			dice = 2 + p_ptr->lev / 12;
			sides = 5 + chr_adj / 6;
			if(only_info) return format("損傷:(%dd%d)*%d",dice,sides,(plev/5));
			if (!get_aim_dir(&dir)) return NULL;
			fire_blast(GF_DARK, dir, dice, sides, (p_ptr->lev / 5), 3,(PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM));
			break;
		}

	/*:::闇生成　p_ptr->tim_genaral[0]をカウンタとして使う*/
	case 2:
		{
			base = p_ptr->lev;
			if(only_info) return format("期間:%d",base);
			set_tim_general(base,FALSE,0,0);
			set_blind(1);
			break;
		}
	case 3:
		{
			if(only_info) return format("");
			set_tim_general(0,TRUE,0,0);
			new_class_power_change_energy_need = 50;
			break;
		}
	case 4:
		{
			base = chr_adj * 4 + plev * 2;
			if(only_info) return format("損傷:%d",base);
			msg_print("部屋が闇に覆われた！");
			project_hack2(GF_DARK,0,0,base);
			(void)unlite_room(py,px);
			break;
		}
	case 5: //v1.1.30 追加
		{
			int range = 7;
			int dam = plev * 3 + chr_adj * 5;

			if(only_info) return format("射程:%d 損傷:%d",range,dam);
			project_length = range;
			if (!get_aim_dir(&dir)) return NULL;
			msg_print("濃密な闇を叩きつけた！");
			fire_ball(GF_DARK,dir,dam,3);

			break;
		}

	case 6:
		{
			base = p_ptr->lev / 2;
			if(base < 10) base = 10;
			if(only_info) return format("期間:%d + 1d%d",base,base);
			set_tim_general(0,TRUE,0,0);
			set_wraith_form(randint1(base) + base, FALSE);
			break;
		}
	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;
	}
	return "";
}



/*:::メイド・執事用特技一覧*/
class_power_type class_power_maid[] =
{
	{	3,3,5,FALSE,TRUE,A_INT,0,0,"照明",
		"現在いる部屋を明るく照らす。"	},
	{	6,6,10,FALSE,TRUE,A_DEX,0,0,"施錠",
		"指定した扉を固く施錠する。施錠された扉は解錠できないが、叩き壊したり魔法で開けることはできる。"},
	{	8,10,10,TRUE,FALSE,A_INT,0,0,"アウトドアクッキング",
		"食べられるものを調達して携帯食料に仕上げる。"	},
	{	16,12,20,FALSE,FALSE,A_WIS,0,0,"応急手当",
		"持ち歩いている針、糸、香草などを使って傷や毒や朦朧を治療し、わずかにHPを回復する。"	},
	{	24,20,50,FALSE,FALSE,A_INT,0,0,"従者の眼力",
		"様々な来客で鍛えられた眼力により、視界内の敵を一瞬で調査する。"	},
	{	30,50,65,TRUE,FALSE,A_STR,0,0,"帰還",
		"ダンジョンの奥深くから手段を問わず帰投する。"	},
	{	40,1,70,FALSE,FALSE,A_CHR,0,0,"ティータイム",
		"リラックスしてMPを回復する。お茶菓子として菓子を消費する。"	},

	{	99,0,0,FALSE,FALSE,0,0,0,"dummy",	""	},
};

/*:::メイド・執事用特技実行部*/
/*:::ターゲット選択でキャンセルしたときなど行動順消費しない時はNULLを返す*/
cptr do_cmd_class_power_aux_maid(int num, bool only_info)
{
	int dir;
	switch(num)
	{
	case 0:
		{
			int dice,sides,base;
			dice=2;sides=p_ptr->lev/10;base=2;
			if(only_info) return format("損傷:%dd%d+%d", dice, sides,base);
			lite_area(damroll(dice, sides)+base, p_ptr->lev / 10 + 1);
			break;
		}
	case 1:
		{
			if(only_info) return "指定した扉を施錠";
			if (!get_aim_dir(&dir)) return NULL;
			wizard_lock(dir);
			break;
		}
	case 2:
		{
			object_type forge, *q_ptr = &forge;
			if(only_info) return "食料を生成";
			msg_print("食料を調達した。");
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
	case 3:
		{
			int heal = p_ptr->lev / 2;
			if(only_info) return format("回復:%d",heal);
			hp_player(heal);
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
			break;
		}
	case 4:
		{
			if(only_info) return "";
			probing();
			break;
		}

	case 5:
		{
			if(only_info) return "";
			if (!word_of_recall()) return NULL;
			break;
		}
	/*:::ティータイム　p_ptr->tim_genaral[0]を使う*/
	case 6:
		{
			cptr q,s;
			object_type *o_ptr;
			int item;
			if(only_info) return "";

			if(p_ptr->tim_general[0])
			{
				msg_format("お茶はさっき飲んだばかりだ。");
				return NULL;
			}

			item_tester_tval = TV_SWEETS;
			q = "どのお菓子を使いますか? ";
			s = "お茶菓子がないとティータイムを楽しめない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
				o_ptr = &inventory[item];
			}
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
				o_ptr = &o_list[0 - item];
			}
			/*:::菓子のpval/10のMPが回復する*/
			msg_format("あなたはお茶を淹れ、ひとときの安らぎに浸った。");
			p_ptr->csp += o_ptr->pval / 10;
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			set_tim_general(100,FALSE,0,0);
			if (!(prace_is_(RACE_GOLEM) || prace_is_(RACE_ANDROID) || prace_is_(RACE_TSUKUMO) || (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)))
				(void)set_food(p_ptr->food + o_ptr->pval + 500);
			break;
		}


	default:
		msg_format("ERROR:実装していない特技が呼ばれた num:%d",num);
		return NULL;

	}
	return "";
}





/*:::特技の個別使用可否判定関数 TRUEが返った時使用可能*/
bool check_class_skill_usable(char *errmsg,int skillnum, class_power_type *class_power_table)
{

	bool have_melee_weapon = (buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM));

	if(p_ptr->wizard)//デバッグ中はすべて使える
	{
		errmsg = "";
		return TRUE;
	}

	if( (class_power_table+skillnum)->is_magic && dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		my_strcpy(errmsg, "ダンジョンが魔力を吸収した！", 150);
		return FALSE;
	}
	if((class_power_table+skillnum)->is_magic && (p_ptr->anti_magic))
	{
		my_strcpy(errmsg, "反魔法バリアが邪魔で集中できない。", 150);
		return FALSE;
	}

	if(p_ptr->shero)
	{
		if( p_ptr->pseikaku == SEIKAKU_BERSERK); //性格狂気
		else if(p_ptr->pclass == CLASS_JUNKO) ; //v1.1.17 純狐は狂戦士化状態でも特技を使える
		else if(p_ptr->pclass == CLASS_SUIKA  && skillnum == 1);//ブレス
		else if(p_ptr->pclass == CLASS_WAKASAGI  && (skillnum == 3 || skillnum == 4));//逆鱗
		else if(p_ptr->pclass == CLASS_YOUMU  && (skillnum == 0 || skillnum == 3 || skillnum == 4 || skillnum == 6));//現世斬,未来永劫斬
		else if(p_ptr->pclass == CLASS_KOGASA  && skillnum == 1);//フルスイング
		else if(p_ptr->pclass == CLASS_KASEN && !is_special_seikaku(SEIKAKU_SPECIAL_KASEN) && skillnum == 3);//大喝
		else if(p_ptr->pclass == CLASS_KAGEROU);//影狼は狂戦士化中にも特技が使える
		else if(p_ptr->pclass == CLASS_YOSHIKA  && skillnum == 2);//死なない殺人鬼

		else
		{
			my_strcpy(errmsg, "狂戦士化していて複雑なことができない。", 150);
			return FALSE;
		}
	}
	/*:::特殊変身時*/
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		if(p_ptr->pclass == CLASS_KAGEROU && p_ptr->mimic_form)
		{
			//影狼は変身中にも特技が使える
		}
		else
		{
			my_strcpy(errmsg, "この姿ではその技は使えない。", 150);
			return FALSE;
		}
	}
	/*:::爪強化時*/
	if (p_ptr->clawextend)
	{
		//早持ち替え
		if(p_ptr->pclass == CLASS_RANGER  && skillnum == 0)
		{
			my_strcpy(errmsg, "今は武器の持ち替えができない。", 150);
			return FALSE;
		}
	}

	//以下、職業ごと特殊処理　正直switchにしとけばよかった
	if(p_ptr->pclass == CLASS_SANAE)
	{
		int cons_soma = (p_ptr->magic_num1[0] == CONCENT_SOMA) ? p_ptr->concent : 0;
		int cons_kana = (p_ptr->magic_num1[0] == CONCENT_KANA) ? p_ptr->concent : 0;
		int cons_suwa = (p_ptr->magic_num1[0] == CONCENT_SUWA) ? p_ptr->concent : 0;
		bool kana_unable = FALSE, suwa_unable = FALSE;
		int i;
		for (i = 1; i < m_max; i++)
		{
			if(m_list[i].r_idx == MON_KANAKO && is_hostile(&m_list[i])) kana_unable = TRUE;
			else if(m_list[i].r_idx == MON_SUWAKO && is_hostile(&m_list[i])) suwa_unable = TRUE;
		}


		switch(skillnum)
		{
		case 7: case 10: case 12: case 14: case 16: case 19: case 20:
			if(cons_soma < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "その奇跡を起こすには秘術の詠唱が足りない。", 150);
				return FALSE;
			}
			break;
		case 4: case 11: case 17: case 21:
			if(kana_unable)
			{
				my_strcpy(errmsg, "神奈子様は力を貸して下さりそうにない・・", 150);
				return FALSE;
			}
			if(cons_kana < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "その奇跡を起こすには神奈子様に捧げる詠唱が足りない。", 150);
				return FALSE;
			}
			break;
		case 5: case 8: case 13: case 18:
			if(suwa_unable)
			{
				my_strcpy(errmsg, "諏訪子様は力を貸して下さりそうにない・・", 150);
				return FALSE;
			}
			if(cons_suwa < sanae_minimum_cons[skillnum])
			{
				my_strcpy(errmsg, "その奇跡を起こすには諏訪子様に捧げる詠唱が足りない。", 150);
				return FALSE;
			}
			break;
		}
		if(skillnum == 21 && !p_ptr->magic_num1[1])
		{
				my_strcpy(errmsg, "その奇跡を起こすには準備が必要だ。", 150);
				return FALSE;
		}
	}
	//にとり　水地形限定、水妖エネルギー処理
	else if(p_ptr->pclass == CLASS_NITORI)
	{
		if((skillnum == 5 || skillnum == 7) && !cave_have_flag_bold(py,px,FF_WATER))
		{
			my_strcpy(errmsg, "ここには水がない。", 150);
			return FALSE;
		}

		if(nitori_energy[skillnum] > p_ptr->magic_num1[20])
		{
			my_strcpy(errmsg, "エネルギーが足りない。水を汲まないと。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_BYAKUREN) //白蓮
	{

		if( (skillnum == 4 || skillnum == 9) && !p_ptr->tim_general[0])
		{
			my_strcpy(errmsg, "まず身体を強化しないと危険だ。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_NAZRIN)
	{

		//ペンデュラムガード
		if (skillnum == 11 && !check_equip_specific_fixed_art(ART_NAZRIN, FALSE))
		{
			my_strcpy(errmsg, "その技はペンデュラムを所持していないと使えない。", 150);
			return FALSE;
		}
		//宝塔レーザー
		if(skillnum == 15 && !check_equip_specific_fixed_art(ART_HOUTOU,FALSE))
		{
			my_strcpy(errmsg, "その技は毘沙門天の宝塔を所持していないと使えない。", 150);
			return FALSE;
		}

	}
	else if(p_ptr->pclass == CLASS_YOUMU)
	{
		if(!have_melee_weapon && ( skillnum == 0 || skillnum == 6))
		{
			my_strcpy(errmsg, "その技は近接武器を装備していないと使えない。", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].name1 != ART_HAKUROUKEN) && (inventory[INVEN_LARM].name1 != ART_HAKUROUKEN) && ( skillnum == 1))
		{
			my_strcpy(errmsg, "その技は白楼剣を装備していないと使えない。", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].name1 != ART_ROUKANKEN) && (inventory[INVEN_LARM].name1 != ART_ROUKANKEN) && ( skillnum == 2))
		{
			my_strcpy(errmsg, "その技は楼観剣を装備していないと使えない。", 150);
			return FALSE;
		}

		if(inventory[INVEN_RARM].tval != TV_KATANA && inventory[INVEN_LARM].tval != TV_KATANA
			&& (skillnum == 4 || skillnum == 5 || skillnum == 8 ))
		{
			my_strcpy(errmsg, "その技は刀を装備していないと使えない。", 150);
			return FALSE;
		}
		if((inventory[INVEN_RARM].tval != TV_KATANA || inventory[INVEN_LARM].tval != TV_KATANA)
			&& (skillnum == 9 ))
		{
			my_strcpy(errmsg, "その技は刀を二本装備していないと使えない。", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_MAGIC_KNIGHT )
	{
		if( !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		{
			my_strcpy(errmsg, "武器を装備していないと魔法剣を使えない。", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_SYUGEN)
	{
		if(total_friends)
		{
			my_strcpy(errmsg, "配下の掌握に忙しくて瞑想ができない。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_TENSHI)
	{
		if((inventory[INVEN_RARM].name1 != ART_HISOU) && (inventory[INVEN_LARM].name1 != ART_HISOU)
			&& ( skillnum == 0 || skillnum == 5 || skillnum == 9 || skillnum == 10))
		{
			my_strcpy(errmsg, "その技は緋想の剣を装備していないと使えない。", 150);
			return FALSE;
		}
	}


	else if (p_ptr->pclass == CLASS_PRIEST)
	{
		if( skillnum == 1 &&  !is_good_realm(p_ptr->realm1))
		{
			my_strcpy(errmsg, "あなたの信仰する神は祝福と無縁だ。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_UDONGE)
	{
		if(p_ptr->blind && ( skillnum == 0 || skillnum == 2 || skillnum == 4 || skillnum == 8 ))
		{
			my_strcpy(errmsg, "その技は目が見えないと使えない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_FLAN)
	{
		if(skillnum == 2 && p_ptr->blind)
		{
			my_strcpy(errmsg, "その技は目が見えないと使えない。", 150);
			return FALSE;
		}
		else if(skillnum == 5 && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		{
			my_strcpy(errmsg, "その技は近接武器を持っていないと使えない。", 150);
			return FALSE;
		}
	}
	else if((p_ptr->pclass == CLASS_MOMIZI || p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_LIBRARIAN || p_ptr->pclass == CLASS_HATATE))
	{
		if(p_ptr->blind)
		{
			my_strcpy(errmsg, "目が見えないと特技が使えない。", 150);
			return FALSE;

		}
	}
	else if(p_ptr->pclass == CLASS_MIKO)
	{
		if( p_ptr->blind && skillnum == 2)
		{
			my_strcpy(errmsg, "その特技は目が見えないと使えない。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_SHINMYOUMARU)
	{
		if(p_ptr->magic_num1[0] && ( skillnum != 15 ))
		{
			my_strcpy(errmsg, "小槌は力を失っている。再び使えるのはいつになることか・・", 150);
			return FALSE;
		}

		else if(p_ptr->tim_general[0] && ( skillnum != 15 ))
		{
			my_strcpy(errmsg, format("小槌はまだ充填中だ。(あと%dターン)",p_ptr->tim_general[0]), 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_KOISHI)
	{
		if(heavy_armor() && ( skillnum == 3 ))
		{
			my_strcpy(errmsg, "何だか装備が重くて面倒だ。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_CHEN)
	{
		if(p_ptr->magic_num1[0] && ( skillnum != 1 ))
		{
			my_strcpy(errmsg, "式が剥がれているため術を使えない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SUMIREKO && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
	{
		bool smapho = FALSE;
		int i;
		if(skillnum == 11)
		{
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].tval == TV_SOUVENIR && inventory[i].sval == SV_SOUVENIR_SMARTPHONE) smapho = TRUE;
			if(!smapho)
			{
				my_strcpy(errmsg, "スマホを持ってないと使えない。", 150);
				return FALSE;
			}
		}
		else if(skillnum == 2 && !p_ptr->do_martialarts)
		{
			my_strcpy(errmsg, "武器を使ってそれをすることはできない。", 150);
			return FALSE;
		}


	}
	else if(p_ptr->pclass == CLASS_SHOU) //宝塔レーザー
	{
		if( skillnum != 1)
		{
			bool houtou = FALSE;
			int i;
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_HOUTOU) houtou = TRUE;

			if(!houtou)
			{
				my_strcpy(errmsg, "その技は毘沙門天の宝塔を所持していないと使えない。", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_CLOWNPIECE) //ヘルエクリプス
	{
		if(skillnum == 2)
		{
			bool torch = FALSE;
			int i;
			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
			if(!torch)
			{
				my_strcpy(errmsg, "その技は地獄の松明を所持していないと使えない。", 150);
				return FALSE;
			}
		}
	}
	else if (p_ptr->pclass == CLASS_VFS_CLOWNPIECE)
	{
		if (skillnum == 0 || skillnum == 1 || skillnum == 4)
		{
			bool torch = FALSE;
			int i;
			for (i = 0; i<INVEN_TOTAL; i++) if (inventory[i].name1 == ART_CLOWNPIECE) torch = TRUE;
			if (!torch)
			{
				my_strcpy(errmsg, "その技は地獄の松明を所持していないと使えない。", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_DOREMY)
	{
		if( (skillnum==2 || skillnum==4 ||skillnum==7 ||skillnum==9) && !(IN_DREAM_WORLD))
		{
			my_strcpy(errmsg, "その技は夢の世界でしか使えない。", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_IKU)
	{
			if(skillnum == 2 && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				my_strcpy(errmsg, "その技は武器を装備していないと使えない。", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_KOMACHI)
	{
		if( !(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
		 && !(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
	   	 &&  (skillnum == 7 || skillnum == 8 || skillnum == 11))
			{
				my_strcpy(errmsg, "その技は大鎌を装備していないと使えない。", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_MINORIKO)
	{
		if( !(inventory[INVEN_RARM].tval == TV_POLEARM && (inventory[INVEN_RARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_RARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
		 && !(inventory[INVEN_LARM].tval == TV_POLEARM && (inventory[INVEN_LARM].sval == SV_WEAPON_WAR_SCYTHE || inventory[INVEN_LARM].sval == SV_WEAPON_SCYTHE_OF_SLICING ))
	   	 &&  (skillnum == 1))
			{
				my_strcpy(errmsg, "その技は大鎌を装備していないと使えない。", 150);
				return FALSE;
			}
	}

	else if (p_ptr->pclass == CLASS_SAMURAI )
	{
		if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && skillnum == 1)
		{
				my_strcpy(errmsg, "武器を持たずに構えても意味がない。", 150);
				return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_SHINMYOU_2 )
	{
		switch(skillnum)
		{
		case 0:
		case 2:
		case 5:
		case 6:
			if(inventory[INVEN_RARM].tval != TV_KNIFE && inventory[INVEN_RARM].tval != TV_SWORD && inventory[INVEN_RARM].tval != TV_KATANA
				&& !(inventory[INVEN_RARM].tval == TV_OTHERWEAPON && inventory[INVEN_RARM].sval == SV_OTHERWEAPON_NEEDLE))
			{
				my_strcpy(errmsg, "その技はこの装備では使えない。", 150);
				return FALSE;
			}
			break;
		case 1:
		case 3:
			{
				bool fishing = FALSE;
				int i;
				for(i=0;i<INVEN_TOTAL;i++)
				{
					if(inventory[i].tval == TV_OTHERWEAPON && inventory[i].sval == SV_OTHERWEAPON_FISHING) fishing = TRUE;
					if(inventory[i].name1 == ART_SHINMYOUMARU) fishing = TRUE;
				}

				if(!fishing)
				{
					my_strcpy(errmsg, "その技は釣り竿か愛刀を持っていないと使えない。", 150);
					return FALSE;
				}
			}
			break;
		case 4:
			{
				if(!is_special_seikaku(SEIKAKU_SPECIAL_SHINMYOUMARU))
				{
					my_strcpy(errmsg, "この技は一部の性格のときにしか使えない。", 150);
					return FALSE;
				}
				else if (p_ptr->cursed & TRC_AGGRAVATE)
				{
					my_strcpy(errmsg, "この技は反感を持たれているときには使えない。", 150);
					return FALSE;

				}
			}
		break;

		}

	}
	else if (p_ptr->pclass == CLASS_FUTO)
	{
		if(skillnum == 7 && inventory[INVEN_LARM].tval != TV_BOW && inventory[INVEN_RARM].tval != TV_BOW)
			{
				my_strcpy(errmsg, "弓がないとその技は使えない。", 150);
				return FALSE;
			}

	}

	else if (p_ptr->pclass == CLASS_PATCHOULI)
	{
			if(skillnum > 4 && p_ptr->asthma > ASTHMA_2)
			{
				my_strcpy(errmsg, "喘息でスペルが唱えられない。", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_RUMIA)
	{
		if(skillnum == 2 && p_ptr->wraith_form)
		{
			my_strcpy(errmsg, "今はその特技を使えない。", 150);
			return FALSE;
		}
		if(skillnum == 3 && !p_ptr->tim_general[0])
		{
			my_strcpy(errmsg, "闇を出していない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_MURASA)
	{
		if(skillnum == 3 && !cave_have_flag_bold((py), (px), FF_WATER))
		{
				my_strcpy(errmsg, "その技は水の中でないと使えない。", 150);
				return FALSE;
		}
		if(skillnum == 5 && (!cave_have_flag_bold((py), (px), FF_WATER) || !cave_have_flag_bold((py), (px), FF_DEEP)))
		{
				my_strcpy(errmsg, "その技は深い水の中でないと使えない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_REIMU)
	{
		int rank = osaisen_rank();
		if(		skillnum == 2 && rank < 2
			||  skillnum == 3 && rank < 4
			||  skillnum == 4 && rank < 2
			||  skillnum == 5 && rank < 5
			||  skillnum == 6 && rank < 4
			||  skillnum == 7 && rank < 7
			||  skillnum == 8 && rank < 3
			||  skillnum == 9 && rank < 6
			||  skillnum ==10 && rank < 4
			||  skillnum ==11 && rank < 7
			||  skillnum ==12 && rank < 5
			||  skillnum ==13 && rank < 8
			||  skillnum ==14 && rank < 9
			)
		{
				my_strcpy(errmsg, "何だかそこまでやる気にはならない。", 150);
				return FALSE;
		}
		if(skillnum == 11 && (inventory[INVEN_RARM].tval != TV_STICK || inventory[INVEN_RARM].sval != SV_WEAPON_OONUSA) && (inventory[INVEN_LARM].tval != TV_STICK || inventory[INVEN_LARM].sval != SV_WEAPON_OONUSA))
		{
				my_strcpy(errmsg, "武器を撫でてみたが動き出す気配はない。", 150);
				return FALSE;
		}
		if(skillnum == 6)
		{
				if (!in_bounds2(py + 1,px) || !cave_have_flag_grid( &cave[py + 1][px],FF_PROJECT)
				||  !in_bounds2(py - 1,px) || !cave_have_flag_grid( &cave[py - 1][px],FF_PROJECT)
				||  !in_bounds2(py ,px + 1) || !cave_have_flag_grid( &cave[py ][px + 1],FF_PROJECT)
				||  !in_bounds2(py ,px - 1) || !cave_have_flag_grid( &cave[py ][px - 1],FF_PROJECT))
				{
					my_strcpy(errmsg, "壁が邪魔だ！", 150);
					return FALSE;
				}
		}
	}
	else if(p_ptr->pclass == CLASS_KAGEROU)
	{
		if(skillnum == 0 && !(FULL_MOON))
		{
				my_strcpy(errmsg, "満月の晩でないと自力で狼になれない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SEIJA)
	{
		if(skillnum == 2 || skillnum == 8 || skillnum == 11)
		{
				my_strcpy(errmsg, "その道具は受動的に発動するものだ。", 150);
				return FALSE;
		}
		if(skillnum == 4 && !p_ptr->magic_num1[SEIJA_ITEM_SUKIMA]
		|| skillnum == 5 && !p_ptr->magic_num1[SEIJA_ITEM_DECOY]
		|| skillnum == 6 && !p_ptr->magic_num1[SEIJA_ITEM_BOMB]
		|| skillnum == 9 && !p_ptr->magic_num1[SEIJA_ITEM_ONMYOU]
		|| skillnum == 10 && !p_ptr->magic_num1[SEIJA_ITEM_KODUCHI]
		|| skillnum == 12 && !p_ptr->magic_num1[SEIJA_ITEM_CHOCHIN]	)
		{
				my_strcpy(errmsg, "その道具は使用回数が切れている。", 150);
				return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_UTSUHO)
	{
		if( !empty_hands(TRUE))
		{
			my_strcpy(errmsg, "手が塞がっていて核融合を制御できない。", 150);
			return FALSE;

		}
	}

	else if (p_ptr->pclass == CLASS_SUWAKO)
	{
		if( skillnum == 5)
		{
			if(p_ptr->food < PY_FOOD_ALERT)
			{
				my_strcpy(errmsg, "お腹が空いて眠れない。", 150);
				return FALSE;
			}
			else if(!cave_have_flag_bold(py,px,FF_MOVE))
			{
				my_strcpy(errmsg, "ここでは落ち着いて眠れない。", 150);
				return FALSE;
			}
		}
	}
	else if(p_ptr->pclass == CLASS_ICHIRIN)
	{
		if((skillnum == 0 || skillnum == 3 || skillnum == 4 || skillnum == 5 || skillnum == 6 || skillnum == 7) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "雲山とうまく連携できない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YUGI)
	{
		if((skillnum == 7) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "その技は素手でないと使えない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YOSHIKA)
	{
		if((skillnum == 2) && !p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "その技は素手でないと使えない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_ALICE)
	{
		if((skillnum == 6) && p_ptr->do_martialarts)
		{
				my_strcpy(errmsg, "人形の装備ができていない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SEIRAN)
	{
		if(skillnum == 0
			&& !(inventory[INVEN_RARM].tval == TV_HAMMER  && inventory[INVEN_RARM].sval == SV_WEAPON_KINE)
			&& !(inventory[INVEN_LARM].tval == TV_HAMMER  && inventory[INVEN_LARM].sval == SV_WEAPON_KINE))
		{
			my_strcpy(errmsg, "杵を装備していない。", 150);
			return FALSE;
		}
		if(((skillnum == 1) ||(skillnum == 3) ||(skillnum == 4) ||(skillnum == 5) ||(skillnum == 6) || (skillnum == 7) || (skillnum == 8)) && !(CHECK_USE_GUN))
		{
			my_strcpy(errmsg, "銃を装備していない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_REISEN2)
	{
		if(skillnum == 0
			&& !(inventory[INVEN_RARM].tval == TV_HAMMER  && inventory[INVEN_RARM].sval == SV_WEAPON_KINE)
			&& !(inventory[INVEN_LARM].tval == TV_HAMMER  && inventory[INVEN_LARM].sval == SV_WEAPON_KINE))
		{
			my_strcpy(errmsg, "杵を装備していない。", 150);
			return FALSE;
		}
	}


	else if(p_ptr->pclass == CLASS_HINA)
	{
		if(((skillnum == 1) ||(skillnum == 3) ||(skillnum == 6) ||(skillnum == 7) ||(skillnum == 8) ||(skillnum == 9)) && !p_ptr->magic_num1[0])
		{
				my_strcpy(errmsg, "厄が足りなくてその特技を使えない。", 150);
				return FALSE;
		}
		if((skillnum == 9) && !dun_level)
		{
				my_strcpy(errmsg, "地上でそんなことをしてはいけない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		if(p_ptr->wild_mode && skillnum != 12)
		{
				my_strcpy(errmsg, "広域マップでその特技を使うことはできない。", 150);
				return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_RINGO)
	{
		if(skillnum == 3 && !p_ptr->magic_num1[2])
		{
			my_strcpy(errmsg, "属性攻撃を受けていない。", 150);
			return FALSE;
		}
	}

	else if(p_ptr->pclass == CLASS_MEIRIN)
	{
		if( (!p_ptr->do_martialarts || p_ptr->riding) && skillnum > 1)
		{
			my_strcpy(errmsg, "今は格闘の技が使えない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SAKUYA)
	{
		if((skillnum == 6 || skillnum == 7) && world_player)
		{
			my_strcpy(errmsg, "今それを行うことはできない。", 150);
			return FALSE;
		}
		if(skillnum == 6 && SAKUYA_WORLD)
		{
			my_strcpy(errmsg, "すでにここはあなたの世界だ。", 150);
			return FALSE;
		}
		if(skillnum == 7 && !SAKUYA_WORLD)
		{
			my_strcpy(errmsg, "今は時を止めていない。", 150);
			return FALSE;
		}
		if(skillnum == 14 && inventory[INVEN_LARM].tval != TV_KNIFE && inventory[INVEN_RARM].tval != TV_KNIFE)
		{
			my_strcpy(errmsg, "短剣を装備していないとその技は使えない。", 150);
			return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_BENBEN)
	{
			if(skillnum == 8 && (buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM)))
			{
				my_strcpy(errmsg, "その演奏は武器を装備していると使えない。", 150);
				return FALSE;
			}
	}
	else if (p_ptr->pclass == CLASS_KAGUYA)
	{
		//新難題保有制限
		if( skillnum ==  7 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_ILMENITE) < 0
		||	skillnum ==  9 && check_have_specific_item(TV_MATERIAL,SV_MATERIAL_MYSTERIUM) < 0
		||	skillnum == 13 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_ASIA) < 0
		||	skillnum == 15 && check_have_specific_item(TV_SOUVENIR,SV_SOUVENIR_KINKAKUJI) < 0)
		{
			my_strcpy(errmsg, "そのアイテムを持っていない。", 150);
			return FALSE;
		}
	}

	else if (p_ptr->pclass == CLASS_MARTIAL_ARTIST)
	{
		if(skillnum == 16 && p_ptr->tim_general[1])
		{
			my_strcpy(errmsg, "界王拳の連続使用は体が持たない。", 150);
			return FALSE;
		}
		if((skillnum == 1 || skillnum == 2 || skillnum == 3 || skillnum == 5 || skillnum == 7 || skillnum == 10
			|| skillnum == 12 || skillnum == 14 || skillnum == 15) && !p_ptr->do_martialarts)
		{
			my_strcpy(errmsg, "その技は格闘中でないと使えない。", 150);
			return FALSE;
		}
		switch(p_ptr->prace)
		{
		case RACE_HUMAN:
		case RACE_YOUKAI:
		case RACE_HALF_YOUKAI:
		case RACE_SENNIN:
		case RACE_TENNIN:
		case RACE_FAIRY:
		case RACE_KAPPA:
		case RACE_YAMAWARO:
		case RACE_MAGICIAN:
		case RACE_KARASU_TENGU:
		case RACE_HAKUROU_TENGU:
		case RACE_ONI:
		case RACE_WARBEAST:
		case RACE_GYOKUTO:
		case RACE_YOUKO:
		case RACE_BAKEDANUKI:
		case RACE_NINGYO:
		case RACE_HOFGOBLIN:
		case RACE_NYUDOU:
		case RACE_KOBITO:
		case RACE_DAIYOUKAI:
		case RACE_DEMIGOD:
		case RACE_NINJA:
		case RACE_ULTIMATE:
		case RACE_LUNARIAN:
			if(skillnum == 7)
			{
				my_strcpy(errmsg, "あなたの種族ではその技は使えない。", 150);
				return FALSE;
			}
			break;
		default:
			if(skillnum == 12)
			{
				my_strcpy(errmsg, "あなたの種族ではその技は使えない。", 150);
				return FALSE;
			}
			break;
		}
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		if(skillnum == 6)
		{
				if (!(in_bounds2(py + 1,px) && cave_have_flag_grid( &cave[py + 1][px],FF_WALL)
				||  in_bounds2(py - 1,px) && cave_have_flag_grid( &cave[py - 1][px],FF_WALL)
				||  in_bounds2(py ,px + 1) && cave_have_flag_grid( &cave[py ][px + 1],FF_WALL)
				||  in_bounds2(py ,px - 1) && cave_have_flag_grid( &cave[py ][px - 1],FF_WALL)))
				{
					my_strcpy(errmsg, "壁のとなりにいないとその技は使えない。", 150);
					return FALSE;
				}
		}
	}
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		//依姫の祇園様の剣は剣か刀を装備している必要がある
		if(skillnum == 1)
		{
			bool flag_ok = FALSE;
			if( inventory[INVEN_RARM].tval == TV_SWORD || inventory[INVEN_LARM].tval == TV_SWORD) flag_ok = TRUE;
			if( inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) flag_ok = TRUE;
			if(!flag_ok)
			{
				my_strcpy(errmsg, "その技は剣か刀を装備していないと使えない。", 150);
				return FALSE;
			}


		}

	}
	else if(p_ptr->pclass == CLASS_TEWI)
	{
		if(skillnum == 5 && (p_ptr->cursed & TRC_AGGRAVATE))
		{
			my_strcpy(errmsg, "今は何を言っても信用されそうにない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_HECATIA)
	{
		if((skillnum == 3 || skillnum == 6) && !hecatia_body_is_(HECATE_BODY_OTHER))
		{
				my_strcpy(errmsg, "その技は異界の体でないと使えない。", 150);
				return FALSE;
		}
		if((skillnum == 4 || skillnum == 7) && !hecatia_body_is_(HECATE_BODY_EARTH))
		{
				my_strcpy(errmsg, "その技は地球の体でないと使えない。", 150);
				return FALSE;
		}
		if((skillnum == 5 || skillnum == 8) && !hecatia_body_is_(HECATE_BODY_MOON))
		{
				my_strcpy(errmsg, "その技は月の体でないと使えない。", 150);
				return FALSE;
		}

	}
	else if(p_ptr->pclass == CLASS_JUNKO)
	{
		if( !p_ptr->shero && ( skillnum == 8 || skillnum == 10))
		{
			my_strcpy(errmsg, "今はそこまでするほどの戦意がない。", 150);
			return FALSE;
		}
	}
	else if(p_ptr->pclass == CLASS_SOLDIER)
	{
		if( skillnum == 2 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_CURBING_SHOT)
		||	skillnum == 3 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_SRH_DIS_PLUS)
		||	skillnum == 4 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_SCAN_MONSTER)
		||	skillnum == 6 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_HEAD_SHOT)

		||	skillnum == 7 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETECTING)
		||	skillnum == 8 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_BARRICADE)
		||	skillnum == 9 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_ELEM_BULLET)
		||	skillnum ==11 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_HIT_AND_AWAY)
		||	skillnum ==12 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_PORTAL)
		||	skillnum ==13 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_CONCENT)
		||	skillnum ==14 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_SET_BOMB)
		||	skillnum ==15 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_RAPID_FIRE)
		||	skillnum ==17 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_MEGA_CRASH)
		||	skillnum ==18 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_MATERIAL_BULLET)

		||	skillnum ==19 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_RESIST)
		||	skillnum ==20 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_ADRENALIN_CONTROL)
		||	skillnum ==21 && !HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR)

			)
		{
			my_strcpy(errmsg, "その特技のための技能を習得していない。", 150);
			return FALSE;
		}




	}

	else if (p_ptr->pclass == CLASS_NEMUNO)
	{
		if (!IS_NEMUNO_IN_SANCTUARY && (skillnum == 2 || skillnum == 5 || skillnum == 6))
		{
			my_strcpy(errmsg, "この特技は縄張りの中にいないと使えない。", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_KOSUZU)
	{
		if (skillnum == 6 && inventory[INVEN_RARM].name1 != ART_HYAKKI && inventory[INVEN_LARM].name1 != ART_HYAKKI )
		{
			my_strcpy(errmsg, "絵巻を手に取らないといけない。", 150);
			return FALSE;
		}
	}
	else if (p_ptr->pclass == CLASS_SHION)
	{
		if (SUPER_SHION && (skillnum == 0 || skillnum == 1))
		{
			my_strcpy(errmsg, "今はその特技を使えない。", 150);
			return FALSE;

		}
		else if (SUPER_SHION && (skillnum == 5))
		{
			my_strcpy(errmsg, "すでに変身している。", 150);
			return FALSE;

		}
		else if ((skillnum == 6) && p_ptr->magic_num1[1] < SHION_BINBOUDAMA)
		{
			my_strcpy(errmsg, "十分な不幸パワーが溜まっていない。", 150);
			return FALSE;

		}



		else if (!SUPER_SHION && (skillnum == 7))
		{
			my_strcpy(errmsg, "今はその特技を使えない。", 150);
			return FALSE;

		}

	}
	else if (p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO)
	{
		if (p_ptr->riding &&  skillnum == 6)
		{
			my_strcpy(errmsg, "配下の背後からではその特技を使えない。", 150);
			return FALSE;

		}
	}
	else if (p_ptr->pclass == CLASS_SAKI)
	{
		if (skillnum == 5 && dun_level)
		{
			my_strcpy(errmsg, "その技は地上でないと使えない。", 150);
			return FALSE;

		}

		if (!(CHECK_USE_GUN) && ((skillnum == 2) || (skillnum == 6) || (skillnum == 9)))
		{
			my_strcpy(errmsg, "銃を装備していない。", 150);
			return FALSE;
		}

	}
	else if (p_ptr->pclass == CLASS_TAKANE)
	{
		if (skillnum == 7 && !cave_have_flag_bold((py), (px), FF_TREE))
		{
			my_strcpy(errmsg, "その技は森の中にいないと使えない。", 150);
			return FALSE;

		}
	}





errmsg = "";
return TRUE;
}


/*:::職業特技の選択と参照と成功判定*/
void do_cmd_new_class_power(bool only_browse)
{
	class_power_type *class_power_table;
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);
	cptr power_desc;
	char errmsg[160];
	int num; //技番号 0から開始
    int cnt; //現在の＠で有効な技の数
	bool  flag, redraw; //flagは特技num選択済みフラグ
    char  out_val[160];
    char  comment[80];
    char  choice;
    int   y = 1;
    int   x = 16;
	int i;
	int chance = 0;
	int cost;
    int minfail = 0;
	bool anti_magic = FALSE;
	int ask = TRUE;

#ifndef JP
	return;
#endif

	//v1.1.37 小鈴の怨霊の特殊処理 アイテムカードで誰でも変身できるのでここに記述する
	if(p_ptr->mimic_form == MIMIC_KOSUZU_GHOST)
	{
		class_power_table = class_power_kosuzu_ghost;
		class_power_aux = do_cmd_class_power_aux_kosuzu_ghost;
		power_desc = "特技";
	}
	else		/*:::各職業ごとの特技の呼称、使用する配列、使用する関数を記述*/
	switch(p_ptr->pclass)
	{
	case CLASS_MAID:
		class_power_table = class_power_maid;
		class_power_aux = do_cmd_class_power_aux_maid;
		power_desc = "特技";
		break;

	case CLASS_RUMIA:
		class_power_table = class_power_rumia;
		class_power_aux = do_cmd_class_power_aux_rumia;
		power_desc = "特技";
		break;
	case CLASS_YOUMU:
		class_power_table = class_power_youmu;
		class_power_aux = do_cmd_class_power_aux_youmu;
		power_desc = "剣技";
		break;
	case CLASS_TEWI:
		class_power_table = class_power_tewi;
		class_power_aux = do_cmd_class_power_aux_tewi;
		power_desc = "特技";
		break;
	case CLASS_UDONGE:
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
		{
			class_power_table = class_power_udonge_d;
			class_power_aux = do_cmd_class_power_aux_udonge_d;
		}
		else
		{
			class_power_table = class_power_udonge;
			class_power_aux = do_cmd_class_power_aux_udonge;
		}
		power_desc = "特技";
		break;
	case CLASS_IKU:
		class_power_table = class_power_iku;
		class_power_aux = do_cmd_class_power_aux_iku;
		power_desc = "特技";
		break;
	case CLASS_KOMACHI:
		class_power_table = class_power_komachi;
		class_power_aux = do_cmd_class_power_aux_komachi;
		power_desc = "特技";
		break;
	case CLASS_KOGASA:
		class_power_table = class_power_kogasa;
		class_power_aux = do_cmd_class_power_aux_kogasa;
		power_desc = "特技";
		break;
	case CLASS_KASEN:
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			class_power_table = class_power_oni_kasen;
			class_power_aux = do_cmd_class_power_aux_oni_kasen;
		}
		else
		{
			class_power_table = class_power_kasen;
			class_power_aux = do_cmd_class_power_aux_kasen;
		}
		power_desc = "特技";
		break;
	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
		class_power_table = class_power_mage;
		class_power_aux = do_cmd_class_power_aux_mage;
		power_desc = "特技";
		break;
	case CLASS_SUIKA:
		class_power_table = class_power_suika;
		class_power_aux = do_cmd_class_power_aux_suika;
		power_desc = "特技";
		break;
	case CLASS_MARISA:
		class_power_table = class_power_marisa;
		class_power_aux = do_cmd_class_power_aux_marisa;
		power_desc = "特技";
		break;
	case CLASS_WAKASAGI:
		class_power_table = class_power_wakasagi;
		class_power_aux = do_cmd_class_power_aux_wakasagi;
		power_desc = "特技";
		break;
	case CLASS_ROGUE:
		class_power_table = class_power_rogue;
		class_power_aux = do_cmd_class_power_aux_rogue;
		power_desc = "特技";
		break;
	case CLASS_RANGER:
		class_power_table = class_power_ranger;
		class_power_aux = do_cmd_class_power_aux_ranger;
		power_desc = "特技";
		break;
	case CLASS_ARCHER:
		class_power_table = class_power_archer;
		class_power_aux = do_cmd_class_power_aux_archer;
		power_desc = "特技";
		break;
	case CLASS_KOISHI:
		class_power_table = class_power_koishi;
		class_power_aux = do_cmd_class_power_aux_koishi;
		power_desc = "特技";
		break;
	case CLASS_MOMIZI:
		class_power_table = class_power_momizi;
		class_power_aux = do_cmd_class_power_aux_momizi;
		power_desc = "特技";
		break;
	case CLASS_SEIGA:
		class_power_table = class_power_seiga;
		class_power_aux = do_cmd_class_power_aux_seiga;
		power_desc = "特技";
		break;
	case CLASS_PRIEST:
		class_power_table = class_power_priest;
		class_power_aux = do_cmd_class_power_aux_priest;
		power_desc = "特技";
		break;
	case CLASS_ORIN:
		//v1.1.77
		if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN))
		{
			class_power_table = class_power_orin2;
			class_power_aux = do_cmd_class_power_aux_orin2;
		}
		else
		{
			class_power_table = class_power_orin;
			class_power_aux = do_cmd_class_power_aux_orin;
		}
		power_desc = "特技";
		break;
	case CLASS_CIRNO:
		class_power_table = class_power_cirno;
		class_power_aux = do_cmd_class_power_aux_cirno;
		power_desc = "特技";
		break;
	case CLASS_SHINMYOUMARU:
		class_power_table = class_power_shinmyoumaru;
		class_power_aux = do_cmd_class_power_aux_shinmyoumaru;
		power_desc = "特技";
		break;
	case CLASS_TSUKUMO_MASTER:
		class_power_table = class_power_tsukumo_master;
		class_power_aux = do_cmd_class_power_aux_tsukumo_master;
		power_desc = "特技";
		break;
	case CLASS_NAZRIN:
		class_power_table = class_power_nazrin;
		class_power_aux = do_cmd_class_power_aux_nazrin;
		power_desc = "特技";
		break;
	case CLASS_SYUGEN:
		class_power_table = class_power_syugen;
		class_power_aux = do_cmd_class_power_aux_syugen;
		power_desc = "瞑想";
		break;
	case CLASS_MAGIC_KNIGHT:
		switch(p_ptr->realm1)
		{
		case TV_BOOK_ELEMENT:
			class_power_table = class_power_magic_knight_elem;
			class_power_aux = do_cmd_class_power_aux_magic_knight_elem;
			break;
		case TV_BOOK_FORESEE:
			class_power_table = class_power_magic_knight_fore;
			class_power_aux = do_cmd_class_power_aux_magic_knight_fore;
			break;
		case TV_BOOK_ENCHANT:
			class_power_table = class_power_magic_knight_enchant;
			class_power_aux = do_cmd_class_power_aux_magic_knight_enchant;
			break;
		case TV_BOOK_SUMMONS:
			class_power_table = class_power_magic_knight_summon;
			class_power_aux = do_cmd_class_power_aux_magic_knight_summon;
			break;
		case TV_BOOK_DARKNESS:
			class_power_table = class_power_magic_knight_darkness;
			class_power_aux = do_cmd_class_power_aux_magic_knight_darkness;
			break;
		case TV_BOOK_CHAOS:
			class_power_table = class_power_magic_knight_chaos;
			class_power_aux = do_cmd_class_power_aux_magic_knight_chaos;
			break;
		default:
			msg_format("ERROR:魔法剣士のこの領域は特技が登録されていない");
			return ;
		}
		power_desc = "魔法剣";
		break;

	case CLASS_LETTY:
		class_power_table = class_power_letty;
		class_power_aux = do_cmd_class_power_aux_letty;
		power_desc = "特技";
		break;
	case CLASS_YOSHIKA:
		class_power_table = class_power_yoshika;
		class_power_aux = do_cmd_class_power_aux_yoshika;
		power_desc = "特技";
		break;
	case CLASS_PATCHOULI:
		class_power_table = class_power_patchouli;
		class_power_aux = do_cmd_class_power_aux_patchouli;
		power_desc = "スペル";
		break;
	case CLASS_LIBRARIAN:
		class_power_table = class_power_librarian;
		class_power_aux = do_cmd_class_power_aux_librarian;
		power_desc = "特技";
		break;
	case CLASS_AYA:
		class_power_table = class_power_aya;
		class_power_aux = do_cmd_class_power_aux_aya;
		power_desc = "特技";
		break;
	case CLASS_BANKI:
		class_power_table = class_power_banki;
		class_power_aux = do_cmd_class_power_aux_banki;
		power_desc = "特技";
		break;
	case CLASS_MYSTIA:
		class_power_table = class_power_mystia;
		class_power_aux = do_cmd_class_power_aux_mystia;
		power_desc = "特技";
		break;
	case CLASS_ENGINEER:
		class_power_table = class_power_engineer;
		class_power_aux = do_cmd_class_power_aux_engineer;
		power_desc = "特技";
		break;
	case CLASS_FLAN:
		class_power_table = class_power_flan;
		class_power_aux = do_cmd_class_power_aux_flan;
		power_desc = "特技";
		break;
	case CLASS_SHOU:
		class_power_table = class_power_shou;
		class_power_aux = do_cmd_class_power_aux_shou;
		power_desc = "特技";
		break;
	case CLASS_MEDICINE:
		class_power_table = class_power_medi;
		class_power_aux = do_cmd_class_power_aux_medi;
		power_desc = "特技";
		break;
	case CLASS_SAMURAI:
		class_power_table = class_power_samurai;
		class_power_aux = do_cmd_class_power_aux_samurai;
		power_desc = "特技";
		break;

	case CLASS_MINDCRAFTER:
		class_power_table = class_power_mind;
		class_power_aux = do_cmd_class_power_aux_syugen;
		power_desc = "超能力";
		break;

	case CLASS_CAVALRY:
		class_power_table = class_power_cavalry;
		class_power_aux = do_cmd_class_power_aux_cavalry;
		power_desc = "特技";
		break;

	case CLASS_YUYUKO:
		class_power_table = class_power_yuyuko;
		class_power_aux = do_cmd_class_power_aux_yuyuko;
		power_desc = "特技";
		break;
	case CLASS_PALADIN:
		class_power_table = class_power_paladin;
		class_power_aux = do_cmd_class_power_aux_paladin;
		power_desc = "特技";
		break;
	case CLASS_SH_DEALER:
		class_power_table = class_power_sh_dealer;
		class_power_aux = do_cmd_class_power_aux_sh_dealer;
		power_desc = "特技";
		break;
	case CLASS_JEWELER:
		class_power_table = class_power_jeweler;
		class_power_aux = do_cmd_class_power_aux_jeweler;
		power_desc = "特技";
		break;
	case CLASS_SATORI:
		class_power_table = class_power_satori;
		class_power_aux = do_cmd_class_power_aux_satori;
		power_desc = "特技";
		break;
	case CLASS_KYOUKO:
		class_power_table = class_power_kyouko;
		class_power_aux = do_cmd_class_power_aux_kyouko;
		power_desc = "特技";
		break;
	case CLASS_TOZIKO:
		class_power_table = class_power_toziko;
		class_power_aux = do_cmd_class_power_aux_toziko;
		power_desc = "特技";
		break;
	case CLASS_LILY_WHITE:
		class_power_table = class_power_lilywhite;
		class_power_aux = do_cmd_class_power_aux_lilywhite;
		power_desc = "特技";
		break;
	case CLASS_KISUME:
		class_power_table = class_power_kisume;
		class_power_aux = do_cmd_class_power_aux_kisume;
		power_desc = "特技";
		break;
	case CLASS_HATATE:
		class_power_table = class_power_hatate;
		class_power_aux = do_cmd_class_power_aux_hatate;
		power_desc = "特技";
		break;
	case CLASS_MIKO:
		class_power_table = class_power_miko;
		class_power_aux = do_cmd_class_power_aux_miko;
		power_desc = "特技";
		break;
	case CLASS_KOKORO:
		class_power_table = class_power_kokoro;
		class_power_aux = do_cmd_class_power_aux_kokoro;
		power_desc = "特技";
		break;
	case CLASS_WRIGGLE:
		class_power_table = class_power_wriggle;
		class_power_aux = do_cmd_class_power_aux_wriggle;
		power_desc = "特技";
		break;
	case CLASS_NINJA:
		class_power_table = class_power_ninja;
		class_power_aux = do_cmd_class_power_aux_ninja;
		power_desc = "忍術";
		break;
	case CLASS_YUUKA:
		class_power_table = class_power_yuuka;
		class_power_aux = do_cmd_class_power_aux_yuuka;
		power_desc = "特技";
		break;
	case CLASS_CHEN:
		class_power_table = class_power_chen;
		class_power_aux = do_cmd_class_power_aux_chen;
		power_desc = "特技";
		break;
	case CLASS_MURASA:
		class_power_table = class_power_murasa;
		class_power_aux = do_cmd_class_power_aux_murasa;
		power_desc = "特技";
		break;
	case CLASS_KEINE:
		if(p_ptr->magic_num1[0]) class_power_table = class_power_keine2;
		else class_power_table = class_power_keine1;
		class_power_aux = do_cmd_class_power_aux_keine;
		power_desc = "特技";
		break;
	case CLASS_YUGI:
		class_power_table = class_power_yugi;
		class_power_aux = do_cmd_class_power_aux_yugi;
		power_desc = "特技";
		break;

	case CLASS_REIMU:
		class_power_table = class_power_reimu;
		class_power_aux = do_cmd_class_power_aux_reimu;
		power_desc = "術";
		break;
	case CLASS_KAGEROU:
		class_power_table = class_power_kagerou;
		class_power_aux = do_cmd_class_power_aux_kagerou;
		power_desc = "特技";
		break;
	case CLASS_SHIZUHA:
		class_power_table = class_power_shizuha;
		class_power_aux = do_cmd_class_power_aux_shizuha;
		power_desc = "特技";
		break;
	case CLASS_SANAE:
		class_power_table = class_power_sanae;
		class_power_aux = do_cmd_class_power_aux_sanae;
		power_desc = "特技";
		break;

	case CLASS_MINORIKO:
		class_power_table = class_power_minoriko;
		class_power_aux = do_cmd_class_power_aux_minoriko;
		power_desc = "特技";
		break;
	case CLASS_REMY:
		class_power_table = class_power_remy;
		class_power_aux = do_cmd_class_power_aux_remy;
		power_desc = "特技";
		break;
	case CLASS_NITORI:
		class_power_table = class_power_nitori;
		class_power_aux = do_cmd_class_power_aux_nitori;
		power_desc = "特技";
		break;
	case CLASS_BYAKUREN:
		class_power_table = class_power_byakuren;
		class_power_aux = do_cmd_class_power_aux_byakuren;
		power_desc = "法力";
		break;
	case CLASS_NUE:
		class_power_table = class_power_nue;
		class_power_aux = do_cmd_class_power_aux_nue;
		power_desc = "特技";
		break;
	case CLASS_SEIJA:
		class_power_table = class_power_seija;
		class_power_aux = do_cmd_class_power_aux_seija;
		power_desc = "特技";
		break;
	case CLASS_UTSUHO:
		class_power_table = class_power_utsuho;
		class_power_aux = do_cmd_class_power_aux_utsuho;
		power_desc = "特技";
		break;

	case CLASS_YAMAME:
		class_power_table = class_power_yamame;
		class_power_aux = do_cmd_class_power_aux_yamame;
		power_desc = "特技";
		break;

	case CLASS_MARTIAL_ARTIST:
		class_power_table = class_power_martial_artist;
		class_power_aux = do_cmd_class_power_aux_martial_artist;
		power_desc = "技";
		break;
	case CLASS_MAGIC_EATER:
		class_power_table = class_power_magic_eater;
		class_power_aux = do_cmd_class_power_aux_magic_eater;
		power_desc = "特技";
		break;
	case CLASS_SUWAKO:
		class_power_table = class_power_suwako;
		class_power_aux = do_cmd_class_power_aux_suwako;
		power_desc = "特技";
		break;
	case CLASS_RAN:
		class_power_table = class_power_ran;
		class_power_aux = do_cmd_class_power_aux_ran;
		power_desc = "特技";
		break;
	case CLASS_TENSHI:
		class_power_table = class_power_tenshi;
		class_power_aux = do_cmd_class_power_aux_tenshi;
		power_desc = "特技";
		break;
	case CLASS_MEIRIN:
		class_power_table = class_power_meirin;
		class_power_aux = do_cmd_class_power_aux_meirin;
		power_desc = "特技";
		break;
	case CLASS_PARSEE:
		class_power_table = class_power_parsee;
		class_power_aux = do_cmd_class_power_aux_parsee;
		power_desc = "特技";
		break;
	case CLASS_EIKI:
		class_power_table = class_power_eiki;
		class_power_aux = do_cmd_class_power_aux_eiki;
		power_desc = "特技";
		break;
	case CLASS_SHINMYOU_2:
		class_power_table = class_power_shinmyou2;
		class_power_aux = do_cmd_class_power_aux_shinmyou2;
		power_desc = "特技";
		break;
	case CLASS_SUMIREKO:
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			class_power_table = class_power_sumireko_d;
			class_power_aux = do_cmd_class_power_aux_sumireko_d;
			power_desc = "力";
		}
		else
		{
			class_power_table = class_power_sumireko;
			class_power_aux = do_cmd_class_power_aux_sumireko;
			power_desc = "超能力";
		}
		break;
	case CLASS_ICHIRIN:
		class_power_table = class_power_ichirin;
		class_power_aux = do_cmd_class_power_aux_ichirin;
		power_desc = "特技";
		break;
	case CLASS_MOKOU:
		class_power_table = class_power_mokou;
		class_power_aux = do_cmd_class_power_aux_mokou;
		power_desc = "特技";
		break;
	case CLASS_KANAKO:
		class_power_table = class_power_kanako;
		class_power_aux = do_cmd_class_power_aux_kanako;
		power_desc = "特技";
		break;
	case CLASS_FUTO:
		class_power_table = class_power_futo;
		class_power_aux = do_cmd_class_power_aux_futo;
		power_desc = "特技";
		break;
	case CLASS_SUNNY:
		class_power_table = class_power_sunny;
		class_power_aux = do_cmd_class_power_aux_sunny;
		power_desc = "特技";
		break;
	case CLASS_LUNAR:
		class_power_table = class_power_lunar;
		class_power_aux = do_cmd_class_power_aux_lunar;
		power_desc = "特技";
		break;
	case CLASS_STAR:
		class_power_table = class_power_star;
		class_power_aux = do_cmd_class_power_aux_star;
		power_desc = "特技";
		break;
	case CLASS_ALICE:
		class_power_table = class_power_alice;
		class_power_aux = do_cmd_class_power_aux_alice;
		power_desc = "特技";
		break;
	case CLASS_LUNASA:
		class_power_table = class_power_lunasa;
		class_power_aux = do_cmd_class_power_aux_lunasa;
		power_desc = "特技";
		break;
	case CLASS_MERLIN:
		class_power_table = class_power_merlin;
		class_power_aux = do_cmd_class_power_aux_merlin;
		power_desc = "特技";
		break;
	case CLASS_LYRICA:
		class_power_table = class_power_lyrica;
		class_power_aux = do_cmd_class_power_aux_lyrica;
		power_desc = "特技";
		break;
	case CLASS_CLOWNPIECE:
		class_power_table = class_power_clownpiece;
		class_power_aux = do_cmd_class_power_aux_clownpiece;
		power_desc = "特技";
		break;
	case CLASS_DOREMY:
		class_power_table = class_power_doremy;
		class_power_aux = do_cmd_class_power_aux_doremy;
		power_desc = "特技";
		break;
	case CLASS_HINA:
		class_power_table = class_power_hina;
		class_power_aux = do_cmd_class_power_aux_hina;
		power_desc = "特技";
		break;
	case CLASS_SAKUYA:
		class_power_table = class_power_sakuya;
		class_power_aux = do_cmd_class_power_aux_sakuya;
		power_desc = "特技";
		break;
	case CLASS_BENBEN:
		class_power_table = class_power_benben;
		class_power_aux = do_cmd_class_power_aux_benben;
		power_desc = "演奏";
		break;
	case CLASS_YATSUHASHI:
		class_power_table = class_power_yatsuhashi;
		class_power_aux = do_cmd_class_power_aux_yatsuhashi;
		power_desc = "演奏";
		break;
	case CLASS_3_FAIRIES:

		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			class_power_table = class_power_sangetsu_2;
			class_power_aux = do_cmd_class_power_aux_sangetsu_2;
		}
		else
		{
			class_power_table = class_power_sangetsu;
			class_power_aux = do_cmd_class_power_aux_sangetsu;
		}
		power_desc = "特技";
		break;
	case CLASS_RAIKO:
		class_power_table = class_power_raiko;
		class_power_aux = do_cmd_class_power_aux_raiko;
		power_desc = "特技";
		break;
	case CLASS_CHEMIST:
		class_power_table = class_power_chemist;
		class_power_aux = do_cmd_class_power_aux_chemist;
		power_desc = "特技";
		break;
	case CLASS_MAMIZOU:
		class_power_table = class_power_mamizou;
		class_power_aux = do_cmd_class_power_aux_mamizou;
		power_desc = "特技";
		break;
	case CLASS_YUKARI:
		class_power_table = class_power_yukari;
		class_power_aux = do_cmd_class_power_aux_yukari;
		power_desc = "特技";
		break;

	case CLASS_RINGO:
		class_power_table = class_power_ringo;
		class_power_aux = do_cmd_class_power_aux_ringo;
		power_desc = "特技";
		break;

	case CLASS_SEIRAN:
		class_power_table = class_power_seiran;
		class_power_aux = do_cmd_class_power_aux_seiran;
		power_desc = "特技";
		break;
	case CLASS_EIRIN:
		class_power_table = class_power_eirin;
		class_power_aux = do_cmd_class_power_aux_eirin;
		power_desc = "特技";
		break;
	case CLASS_KAGUYA:
		class_power_table = class_power_kaguya;
		class_power_aux = do_cmd_class_power_aux_kaguya;
		power_desc = "特技";
		break;
	case CLASS_SAGUME:
		class_power_table = class_power_sagume;
		class_power_aux = do_cmd_class_power_aux_sagume;
		power_desc = "特技";
		break;

	case CLASS_REISEN2:
		class_power_table = class_power_reisen2;
		class_power_aux = do_cmd_class_power_aux_reisen2;
		power_desc = "特技";
		break;
	case CLASS_TOYOHIME:
		class_power_table = class_power_toyohime;
		class_power_aux = do_cmd_class_power_aux_toyohime;
		power_desc = "特技";
		break;
	case CLASS_YORIHIME:
		class_power_table = class_power_yorihime;
		class_power_aux = do_cmd_class_power_aux_yorihime;
		power_desc = "神降ろし";
		break;
	case CLASS_HECATIA:
		class_power_table = class_power_hecatia;
		class_power_aux = do_cmd_class_power_aux_hecatia;
		power_desc = "特技";
		break;
	case CLASS_JUNKO:
		class_power_table = class_power_junko;
		class_power_aux = do_cmd_class_power_aux_junko;
		power_desc = "特技";
		break;
	case CLASS_SOLDIER:
		class_power_table = class_power_soldier;
		class_power_aux = do_cmd_class_power_aux_soldier;
		power_desc = "技能";
		break;

	case CLASS_NEMUNO:
		class_power_table = class_power_nemuno;
		class_power_aux = do_cmd_class_power_aux_nemuno;
		power_desc = "特技";
		break;

	case CLASS_AUNN:
		class_power_table = class_power_aunn;
		class_power_aux = do_cmd_class_power_aux_aunn;
		power_desc = "特技";
		break;
	case CLASS_NARUMI:
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
		{
			class_power_table = class_power_narumi2;
			class_power_aux = do_cmd_class_power_aux_narumi2;
		}
		else
		{
			class_power_table = class_power_narumi;
			class_power_aux = do_cmd_class_power_aux_narumi;
		}
		power_desc = "特技";
		break;
	case CLASS_KOSUZU:

		if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI)
		{
			class_power_table = class_power_kosuzu_hyakki;
			class_power_aux = do_cmd_class_power_aux_kosuzu_hyakki;
		}
		else
		{
			class_power_table = class_power_kosuzu;
			class_power_aux = do_cmd_class_power_aux_kosuzu;
		}
		power_desc = "特技";
		break;

	case CLASS_LARVA:
		class_power_table = class_power_larva;
		class_power_aux = do_cmd_class_power_aux_larva;
		power_desc = "特技";
		break;
	case CLASS_MAI:
		class_power_table = class_power_mai;
		class_power_aux = do_cmd_class_power_aux_mai;
		power_desc = "特技";
		break;
	case CLASS_SATONO:
		class_power_table = class_power_satono;
		class_power_aux = do_cmd_class_power_aux_mai;
		power_desc = "特技";
		break;
	case CLASS_VFS_CLOWNPIECE:
		class_power_table = class_power_vfs_clownpiece;
		class_power_aux = do_cmd_class_power_aux_vfs_clownpiece;
		power_desc = "特技";
		break;
	case CLASS_JYOON:
		class_power_table = class_power_jyoon;
		class_power_aux = do_cmd_class_power_aux_jyoon;
		power_desc = "特技";
		break;
	case CLASS_SHION:
		class_power_table = class_power_shion;
		class_power_aux = do_cmd_class_power_aux_shion;
		power_desc = "特技";
		break;
	case CLASS_OKINA:
		class_power_table = class_power_okina;
		class_power_aux = do_cmd_class_power_aux_okina;
		power_desc = "特技";
		break;
	case CLASS_EIKA:
		class_power_table = class_power_eika;
		class_power_aux = do_cmd_class_power_aux_eika;
		power_desc = "特技";
		break;
	case CLASS_MAYUMI:
		class_power_table = class_power_mayumi;
		class_power_aux = do_cmd_class_power_aux_mayumi;
		power_desc = "特技";
		break;
	case CLASS_KUTAKA:
		class_power_table = class_power_kutaka;
		class_power_aux = do_cmd_class_power_aux_kutaka;
		power_desc = "特技";
		break;

	case CLASS_URUMI:
		class_power_table = class_power_urumi;
		class_power_aux = do_cmd_class_power_aux_urumi;
		power_desc = "特技";
		break;

	case CLASS_SAKI:
		class_power_table = class_power_saki;
		class_power_aux = do_cmd_class_power_aux_saki;
		power_desc = "特技";
		break;

	case CLASS_YACHIE:
		class_power_table = class_power_yachie;
		class_power_aux = do_cmd_class_power_aux_yachie;
		power_desc = "特技";
		break;

	case CLASS_KEIKI:
		class_power_table = class_power_keiki;
		class_power_aux = do_cmd_class_power_aux_keiki;
		power_desc = "特技";
		break;

	case CLASS_RESEARCHER:
		class_power_table = class_power_researcher;
		class_power_aux = do_cmd_class_power_aux_researcher;
		power_desc = "特技";
		break;

	case CLASS_CARD_DEALER:
		class_power_table = class_power_card_dealer;
		class_power_aux = do_cmd_class_power_aux_card_dealer;
		power_desc = "特技";
		break;

	case CLASS_TAKANE:
		class_power_table = class_power_takane;
		class_power_aux = do_cmd_class_power_aux_takane;
		power_desc = "特技";
		break;

	case CLASS_MOMOYO:
		class_power_table = class_power_momoyo;
		class_power_aux = do_cmd_class_power_aux_momoyo;
		power_desc = "特技";
		break;

	case CLASS_SANNYO:
		class_power_table = class_power_sannyo;
		class_power_aux = do_cmd_class_power_aux_sannyo;
		power_desc = "特技";
		break;

	default:
		msg_print("あなたは職業による特技を持っていない。");
		return;
	}


	if (p_ptr->confused && !only_browse)
	{
		msg_print("混乱していて集中できない！");
		return;
	}

	screen_save();

	/*:::反魔法フラグ*/
	//if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) || (p_ptr->anti_magic)) anti_magic = TRUE;
	flag = FALSE;
    redraw = FALSE;

      num = (-1);
#ifdef ALLOW_REPEAT
	//リピートされた場合numを呼び出して「特技選択済み」フラグをTRUE
	if (repeat_pull(&num))
	{
		///リピートしたとき＠のレベルが前より下がってて技が使えなくなってたらキャンセルする処理らしい。流用。
		if ((class_power_table + num)->min_lev <=  p_ptr->lev) flag = TRUE;

		// -Hack- 依姫の「八百万の神々」は使う度に機能が変わるのでリピートキャンセル
	//まだなにかおかしい？
		if(p_ptr->pclass == CLASS_YORIHIME && num == 0) flag = FALSE;
	}
#endif

/*:::ここから、技の一覧を表示して選択を受け取り技番号を得る処理*/

	for(cnt=0;(class_power_table+cnt)->min_lev <= p_ptr->lev;cnt++);
	if(cnt == 0)
	{
		msg_print("まだ使える特技がない。");
		screen_load();
		return;
	}

	if (only_browse)
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'で一覧, ESC) どの%sについて知りますか？", power_desc, I2A(0), I2A(cnt - 1), power_desc);
	else
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'で一覧, ESC) どの%sを使いますか？",	power_desc, I2A(0), I2A(cnt - 1), power_desc);

	choice= (always_show_list ) ? ESCAPE:1;

	while (!flag)
	{
		if(choice==ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice, TRUE) )break;
		/*:::メニュー表示用*/

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') )
		{
			/* Show the list */
			if (!redraw)
			{
				char psi_desc[160];

				redraw = TRUE;
				/* Save the screen */
				if (!only_browse ) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str("名前", y, x + 5);

				//v1.1.89 37→40
				put_str(format("Lv   消費　 関連　失率 効果"), y, x + 40);

				/* Dump the spells */
				/*:::技などの一覧を表示*/
				for (i = 0; (class_power_table + i)->min_lev <= p_ptr->lev; i++)
				{
					spell = class_power_table + i;

					cost = spell->cost + (p_ptr->lev - spell->min_lev) * spell->lev_inc_cost / 10;

					//v1.1.85 衣玖が雷雲の中にいるとコスト半分
					if(p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE)) cost = (cost+1)/2;


					if(cost > 0)
					{
						/*:::消費魔力減少 mod_need_manaに定義されたMANA_DIVとDEC_MANA_DIVの値を使用*/
						if(!spell->use_hp && spell->is_magic && p_ptr->dec_mana) cost = cost * 3 / 4;
						if(cost < 1) cost = 1;
					}

					chance = spell->fail;

					/*:::失敗率計算　失敗率設定0の技はパス*/
					if (chance)
					{
						/*:::レベル差による失敗率減少*/
						chance -= 3 * (p_ptr->lev - spell->min_lev);
						/*:::パラメータによる失敗率減少*/
						chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[spell->stat]] - 1);

						/*:::装備による失敗率上昇　あれば*/
						if (heavy_armor()) chance += spell->heavy_armor_fail;

						/*:::性格や呪いによる成功率増減*/
						chance += p_ptr->to_m_chance;
						/*:::特技が魔法の時、消費魔力減少などの効果を受ける*/
						if(spell->is_magic) chance = mod_spell_chance_1(chance);

						/* Extract the minimum failure rate */
						minfail = adj_mag_fail[p_ptr->stat_ind[spell->stat]];

						/* Minimum failure rate */
						if (chance < minfail) chance = minfail;

						/* Stunning makes spells harder */
						if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
						else if (p_ptr->stun) chance += 15;

						/* Always a 5 percent chance of working */
						if (chance > 95) chance = 95;
						/*:::十分なhp/mpがないとき　どのみち実行できなくするが*/
						if (spell->use_hp && cost > p_ptr->chp) chance =100;
						if (!spell->use_hp && cost > p_ptr->csp) chance =100;
					}

					/*:::効果欄のコメントを得る*/
					sprintf(comment, "%s", (*class_power_aux)(i,TRUE));
//msg_format("len(%d):%d",i,strlen((*class_power_aux)(i,TRUE)));
					/*:::カーソルまたはアルファベットの表示*/
					sprintf(psi_desc, "  %c) ",I2A(i));

					/* Dump the spell --(-- */
					//Hack 幽々子最終奥義は全消費
//					if(p_ptr->pclass == CLASS_YUYUKO && i==9 || spell->cost < 0)
					if(spell->cost < 0)
					{
						strcat(psi_desc, format("%-35s%2d 全て%s  %s %3d%%  %s",
						      spell->name, spell->min_lev,(spell->use_hp)?"(HP)":"(MP)",
						      stat_desc[spell->stat], chance, comment));
					}
					else
					{
						strcat(psi_desc, format("%-35s%2d %4d%s  %s %3d%%  %s",
						      spell->name, spell->min_lev, cost,(spell->use_hp)?"(HP)":"(MP)",
						      stat_desc[spell->stat], chance, comment));
					}
					//if(anti_magic && spell->is_magic) c_prt(TERM_L_DARK,psi_desc,y+i+1,x);
					if(!check_class_skill_usable(errmsg,i,class_power_table))
					{
						if(p_ptr->pclass == CLASS_SOLDIER)
							c_prt(TERM_L_DARK,"---未習得---",y+i+1,x);
						else
							c_prt(TERM_L_DARK,psi_desc,y+i+1,x);
					}
					else prt(psi_desc, y + i + 1, x);
				}

				if(cp_ptr->magicmaster)
				{
					prt("", y + i + 1, x);
					i++;
					prt("(Ctrl+'g'コマンドで「結界ガード」が使用可能です。)", y + i + 1, x);
					i++;
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Hide the list */
			else if (!only_browse)
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		/* Note verify */
		ask = isupper(choice);

		/* Lowercase */
		if (ask) choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= cnt))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = class_power_table + i;

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, "%sを使いますか？", spell->name);

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}
		num = i;
		/* Stop the loop */
		flag = TRUE;

#ifdef ALLOW_REPEAT /* TNB */
		if(!only_browse) repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */

	}

	//この辺screen_load()や再描画関連でバグ残ってるかもしれない。

	/* Restore the screen */
	if (redraw && !only_browse) screen_load();
	/* Show choices */
	p_ptr->window |= (PW_SPELL);
	/* Window stuff */
	window_stuff();
	/* Abort if needed */
	if (!flag)
	{
		screen_load();
		return;
	}
/*:::技の選択終了。失敗率判定へ。*/


	screen_load();

	//v1.1.22 兵士に限り、使用不可の特技の説明を見られない
	if(only_browse && p_ptr->pclass == CLASS_SOLDIER && !check_class_skill_usable(errmsg,num,class_power_table))
	{
		msg_format("%s",errmsg);
		return;
	}

	///pend 技の説明見るときの処理をmind_browseから適当にコピー。　いずれrumor_new()みたいに\で手動改行して表示したい。
	if(only_browse)
	{
		int j, line;
		char temp[62*5];
		screen_save();

		for(j=16;j<23;j++)	Term_erase(12, j, 255);

		roff_to_buf(spell->tips, 62, temp, sizeof(temp));
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		prt("何かキーを押して下さい。",0,0);
		(void)inkey();
		screen_load();
		return;
	}

	/*:::選択された技についてコストや失敗率を再び計算する　リピート処理があるので再利用はできない*/
	spell = class_power_table + num;
	chance = spell->fail;
	if (chance)
	{
		chance -= 3 * (p_ptr->lev - spell->min_lev);
		chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[spell->stat]] - 1);
		if (heavy_armor()) chance += spell->heavy_armor_fail;
		chance += p_ptr->to_m_chance;
		if(spell->is_magic) chance = mod_spell_chance_1(chance);
		minfail = adj_mag_fail[p_ptr->stat_ind[spell->stat]];
		if (chance < minfail) chance = minfail;
		if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
		else if (p_ptr->stun) chance += 15;
		if (chance > 95) chance = 95;
	}
	cost = spell->cost + (p_ptr->lev - spell->min_lev) * spell->lev_inc_cost / 10;

	//v1.1.85 衣玖が雷雲の中にいるとコスト半分
	if (p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE)) cost = (cost + 1) / 2;

	if(cost > 0)
	{
		if(!spell->use_hp && spell->is_magic && p_ptr->dec_mana) cost = cost * 3 / 4;
		if(cost < 1) cost = 1;
	}
	/*:::HPやMPが足りない時は強制中断　どうせオプション使わないし処理めんどい*/
	if (spell->use_hp && cost > p_ptr->chp)
	{
		msg_print("ＨＰが足りません。");
		return;
	}
	if (!spell->use_hp && ((spell->cost > 0) ? cost : -(cost)) > p_ptr->csp)
	{
		msg_print("ＭＰが足りません。");
		return;
	}
	/*:::反魔法や特定武器非所持など特殊な条件が必要な技の使用不可判定メッセージ*/
	if(!check_class_skill_usable(errmsg,num,class_power_table))
	{
		msg_format("%s",errmsg);
		return;
	}

	/*:::特技失敗処理*/
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
		sound(SOUND_FAIL);

		/*:::特殊な失敗ペナルティがある場合ここに書く*/
		//v1.1.30 てゐのエンシェントデューパー失敗
		if(p_ptr->pclass == CLASS_TEWI && num == 5)
		{
			msg_print("あなたの大嘘がばれた！");
			for (i = m_max - 1; i >= 1; i--)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if (is_pet(m_ptr)) continue;
				set_hostile(m_ptr);
				(void)set_monster_csleep(i, 0);

			}
			aggravate_monsters(0,TRUE);
			msg_print("フロアのモンスター達から追われる身になった！");
		}
		else
			msg_format("%sの使用に失敗した！",power_desc);

	}

	/*:::特技成功処理*/
	else
	{
		/*:::成功判定後にターゲット選択でキャンセルしたときなどにはcptrにNULLが返り、そのまま行動順消費せず終了する*/
		if(!(*class_power_aux)(num,FALSE)) return;
	}

	/*:::ターンやコスト消費など*/

	if(new_class_power_change_energy_need)
	{
		energy_use = new_class_power_change_energy_need;
		if(p_ptr->wizard) msg_format("Energy_use:%d",energy_use);
		new_class_power_change_energy_need = 0;
	}
	else energy_use = 100;

	if (spell->use_hp)
	{
		/*:::技の使用のとき何らかの理由でダメージ受けてHPが足りなくなった時の救済処置*/
		if(p_ptr->chp < cost) cost = p_ptr->chp;
		take_hit(DAMAGE_USELIFE, cost, "技の使用による負担", -1);
		p_ptr->redraw |= (PR_HP);
	}
	else
	{
		if(spell->cost < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
		else
		{
			p_ptr->csp -= cost;
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
			}

		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);

	return;

}



//EXTRA用サポートアイテムのリスト
//「アイテムカード」のpvalにここの添字が格納されるので順番変更すると過去のセーブデータと互換性がなくなる。注意。
//{rarity,min_lev,max_lev,prod_num,cost,r_idx,*table,*func,func_idx,
//"name(一覧表示の都合で20文字まで)","info"}
const support_item_type support_item_list[] =
{
	//ムーンライトレイ
	{40, 10, 50,4,2,	MON_RUMIA,class_power_rumia,do_cmd_class_power_aux_rumia,1,
	"黒い球体","それはターゲット周辺に暗黒属性のレーザーを数本放つ。"},
	//ダークサイドオブザムーン
	{100, 50, 128,1,30,	MON_RUMIA,class_power_rumia,do_cmd_class_power_aux_rumia,6,
	"古ぼけたリボン","それを使うと一定時間幽体化する。"},

	//現世斬
	{30, 10, 100,5,5,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,0,
	"小さな切り花","それを使うとターゲットの隣まで一瞬で移動しそのまま攻撃する。近接武器を装備していないと使えない。"},
	//未来永劫斬
	{80, 40, 100,4,8,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,6,
	"黒いリボンタイ","それを使うとターゲットの隣まで一瞬で移動し、かわされるまで連続攻撃する。近接武器を装備していないと使えない。"},
	//一念無量劫
	{30, 60, 120,3,10,	MON_YOUMU,class_power_youmu,do_cmd_class_power_aux_youmu,9,
	"大小の刀","それは自分を中心とした半径五マスに対し、地獄、カオス、空間歪曲、地獄の業火、無属性、破邪のいずれかの属性の強力な攻撃を放つ。"},

	//落とし穴
	{30, 0, 50,8,3,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,0,
	"妖怪兎のスコップ","落とし穴を掘る。浮遊していないモンスターが落とし穴に落ちると朦朧、気絶、行動遅延することがある。誰かが見ている所では使用できない。"},
	//フラスターエスケープ
	{30, 20, 100,5,4,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,2,
	"人参のペンダント","通常攻撃を行い、その後一瞬で離脱する。離脱に失敗することもある。"},
	//大穴牟遅様の薬
	{30, 30,  90,5,5,	MON_TEWI,class_power_tewi,do_cmd_class_power_aux_tewi,3,
	"蒲の穂","それはHPを中程度回復させて切り傷を治す。"},

	//波長分析
	{20, 0,  90,5,4,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,1,
	"兎の耳飾り","それは周囲にあるものを感知する。レベルが上がると感知の範囲と種類が増える。"},
	//マインドストッパー
	{30, 10,  50,4,5,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,4,
	"人参のネクタイピン","それは視界内のモンスターの動きを止める。"},
	//国士無双の薬
	{35, 20,  110,7,3,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,6,
	"永琳印の薬瓶","それを使うと三段階パワーアップする。飲みすぎると・・"},
	//アキュラースペクトル
	{60, 60,  120,2,16,	MON_UDONGE,class_power_udonge,do_cmd_class_power_aux_udonge,9,
	"薬屋の編み笠","それを使うと短時間分身し敵からの攻撃を受けにくくなる。"},

	//龍神の怒り
	{25, 0, 40,6,2,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,0,
	"帯電リボン","それはターゲットの位置に電撃属性のボールを発生させる。"},
	//龍神の一撃
	{40, 20, 70,4,3,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,2,
	"火花を散らすネクタイ","それは一時的に武器に電撃属性を付与する。近接武器を装備していないと使えない。"},
	//羽衣は空の如く
	{45, 40, 110,4,6,	MON_IKU,class_power_iku,do_cmd_class_power_aux_iku,3,
	"天女の羽衣","それを使うと一時的に元素耐性とAC上昇を得る。"},

	//河の流れのように
	{50, 5, 70,5,6,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,2,
	"タイタニックと名付けられたボロ船","それを使うとターゲットに向けて短距離を突進し、水属性ダメージを与える。"},
	//脱魂の儀
	{40, 40, 100,4,7,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,5,
	"死神のぽっくり下駄","それは指定した敵と位置を交換する。視界外の敵にも有効。"},
	//お迎え体験版
	{40, 30, 90,6,5,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,6,
	"六道銭","それを指定した敵を自分の隣に引き寄せ、そのまま攻撃する。"},
	//余命幾許も無し
	{50, 30, 80,7,4,	MON_KOMACHI,class_power_komachi,do_cmd_class_power_aux_komachi,7,
	"波打つ大鎌","それは隣接した敵を低確率で一撃で倒し、中確率でHPを半分にする。生者以外には無効。"},

	//光球設置
	{15, 0, 20,7,1,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,0,
	"光る大玉","それは現在いる部屋を明るくする。"},
	//動物説得
	{25, 5, 40,4,2,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,4,
	"仙人のシニヨンキャップ","それは周囲の動物を説得し配下にする。"},
	//ドラゴンズグロウル
	{35, 40, 100,5,6,	MON_KASEN,class_power_kasen,do_cmd_class_power_aux_kasen,6,
	"包帯の竜頭","それは現在HPの1/3の威力の轟音属性ブレスを放つ。"},

	//火炎ブレス
	{25, 10, 70,5,4,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,1,
	"鬼の瓢箪","それは現在HPの1/3の威力の火炎属性ブレスを放つ。"},
	//戸隠山投げ
	{20, 15, 50,7,3,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,3,
	"大岩","それは無属性ボールを放つ。威力は腕力に依存する。"},
	//ミッシングパープルパワー
	{60, 50, 120,1,30,	MON_SUIKA,class_power_suika,do_cmd_class_power_aux_suika,6,
	"鬼の分銅","それを使うと一定時間巨大化する。巨大化中は身体能力が爆発的に上昇するが巻物と魔道具が使用できない。Uコマンドで巨大化を解除できる。"},

	//マジックミサイル
	{15, 0, 30,10,2,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,2,
	"魔法のキノコ","それは低威力な無属性ボルトを放つ。"},
	//イリュージョンレーザー
	{20, 20, 60,7,3,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,5,
	"キノコペースト","それは閃光属性のビームを放つ。"},
	//マジックナパーム
	{25, 40, 90,5,5,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,8,
	"キノコエキス","それは魔力属性のロケットを放つ。"},
	//マスパ
	{35, 50, 110,3,8,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,9,
	"八卦炉","それは強力な核熱属性のビームを放つ。"},
	//ファイナルマスパ
	{100, 80, 128,1,40,	MON_MARISA,class_power_extra,do_cmd_class_power_aux_extra,0,
	"緋緋色金製の八卦炉","それは極めて強力な分解属性のビームを放つ。"},
	//着せ替え魔法
	{75, 25, 128,1,40,	MON_MARISA,class_power_marisa,do_cmd_class_power_aux_marisa,10,
	"魔法の小瓶","それを使うとその場で探索拠点にアクセスし、アイテム入れ替えや装備の変更ができる。一度でも拠点から出たら終了するので注意。"},

	//ピッチャー返し
	{40, 15, 75,3,4,	MON_KOGASA,class_power_kogasa,do_cmd_class_power_aux_kogasa,3,
	"茄子色の雨傘","それは一定時間反射能力をもたらす。"},
	//鍛冶
	{50, 20, 100,1,10,	MON_KOGASA,class_power_kogasa,do_cmd_class_power_aux_kogasa,5,
	"一つ目小僧の金槌","それは金属製の武具の修正値を強化する。"},

	//スケールウェイブ
	{30, 0, 50,5,1,	MON_WAKASAGI,class_power_wakasagi,do_cmd_class_power_aux_wakasagi,1,
	"波模様の石","それは自分を中心に水属性のボールを発生させる。水上で使用すると威力が上がる。"},
	//逆鱗の大荒波
	{55, 40, 100,3,12,	MON_WAKASAGI,class_power_wakasagi,do_cmd_class_power_aux_wakasagi,4,
	"剥がれた鱗","それは視界内に強力な水属性攻撃を行い周囲の地形を水にする。体力が減っていると威力が上がる。"},

	//キャッチアンドローズ
	{35, 0, 50,4,3,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,0,
	"薔薇のつぼみ","それは近くの敵に攻撃し吹き飛ばす。巨大な敵には効果が薄い。"},
	//コンディションドテレポート
	{50, 20, 80,4,4,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,3,
	"受話器","それを使うと一番近い敵の隣にテレポートし、そのまま一撃を加える。精神を持たない敵は対象にならない。"},
	//胎児の夢
	{60, 40, 115,3,10,	MON_KOISHI,class_power_koishi,do_cmd_class_power_aux_koishi,5,
	"覚の閉じた瞳","それは隣接した敵一体に強力な精神攻撃と時間逆転攻撃を仕掛ける。通常の精神を持たない敵には効果が薄い。"},

	//レイビーズバイト
	{35, 10, 50,5,2,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,2,
	"白狼の牙","それは隣接したモンスター一体にダメージを与える。毒耐性のない敵は混乱、朦朧することがある。"},
	//千里眼
	{50, 40, 80,1,10,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,3,
	"大将棋の駒","それはフロア全ての地形とアイテムを感知する。"},
	//エクスペリーズカナン
	{70, 30, 100,4,10,	MON_MOMIZI,class_power_momizi,do_cmd_class_power_aux_momizi,4,
	"白狼天狗の大刀","それは近くの敵を現在のフロアから追放しようと試みる。"},

	//壁堀り
	{25, 0, 60,12,1,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,0,
	"邪仙の鑿","それは隣接した壁やドアに穴を開ける"},
	//トンリン芳香
	{70, 15, 50,2,5,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,3,
	"キョンシーの札","それはこのフロア限定でキョンシー「宮古　芳香」を呼び出す。"},
	//ウォールランナー
	{85, 40, 100,4,10,	MON_SEIGA,class_power_seiga,do_cmd_class_power_aux_seiga,5,
	"邪仙の羽衣","それは広範囲の一直線上の床を壁にする。"},

	//フローズン冷凍法
	{30, 0, 40,7,2,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,1,
	"凍った蛙","それは隣接した敵一体に極寒属性攻撃を行う。"},
	//ソードフリーザー
	{30, 20, 70,4,2,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,5,
	"つらら","それは装備中の武器に対し一時的に冷気属性を付与する。"},
	//瞬間冷凍ビーム
	{50, 30, 100,4,7,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,8,
	"英吉利牛","それは極寒属性レーザーを広範囲に放つ。威力はHPの1/3になる。"},
	//アイシクルマシンガン
	{45, 25, 85,8,4,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,9,
	"かき氷(みず味)","それは冷気属性のボルトをターゲット周辺に連射する。"},
	//フロストキング
	{80, 40, 100,1,15,	MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,11,
	"雪だるま","それはこのフロア限定で「氷晶」を召喚する。氷晶は配下扱いで、移動せずに強力な冷気攻撃を行う。"},

	//きゅっとしてドカーン
	{55, 40, 120,4,10,	MON_F_SCARLET,class_power_flan,do_cmd_class_power_aux_flan,2,
	"巨大隕石の欠片","それはターゲットとその周辺3+1d3グリッドを*破壊*する。ターゲットは明示的に指定しなければならない。(25-レベル/2)%の確率で想定外の大破壊が起こる。地上やクエストダンジョンでは使えない。"},
	//フォーオブアカインド
	{80, 20, 80,1,10,	MON_F_SCARLET,class_power_flan,do_cmd_class_power_aux_flan,4,
	"虹色の欠片","それを使うとこのフロア限定で「フランドール・スカーレット」の分身が三体出現する。分身は配下扱いだが命令に関わらずプレイヤーを範囲攻撃に巻き込む。"},
	//レーヴァテイン
	{90, 60, 128,4,18,	MON_F_SCARLET,class_power_flan,do_cmd_class_power_aux_flan,5,
	"捻じれた黒杖","それは自分を中心に強力な地獄の劫火属性のボールを発生させる。"},

	//ゾンビフェアリー
	{70,15, 45,3,3,	MON_ORIN,class_power_orin,do_cmd_class_power_aux_orin,5,
	"天使の輪(針金付き)","それはゾンビフェアリーを召喚する。"},
	//食人怨霊
	{60, 10, 70,8,4,	MON_ORIN,class_power_orin,do_cmd_class_power_aux_orin,6,
	"荷物満載の猫車","それは大量の怨霊を召喚し、近くのランダムな敵に憑りつかせる。"},

	//大きくなあれ
	{30, 15, 70,4,6,	MON_SHINMYOUMARU,class_power_shinmyoumaru,do_cmd_class_power_aux_shinmyoumaru,1,
	"打出の小槌","それを使うと30ターンの間腕力と耐久力が5増加する。通常の限界値を超えない。"},
	//もっと大きくなあれ
	{80, 50, 128,2,20,	MON_SHINMYOUMARU,class_power_shinmyoumaru,do_cmd_class_power_aux_shinmyoumaru,9,
	"打出の小槌Ｇ","それは30ターンの間腕力と耐久力の10増加をもたらす。通常の限界値を超える。"},

	//鼠召喚
	{50, 5, 40,6,3,	MON_NAZRIN,class_power_nazrin,do_cmd_class_power_aux_nazrin,4,
	"子鼠の籠","それは数体の妖怪鼠を配下として呼び出す。"},
	//全感知
	{30, 20, 75,5,3,	MON_NAZRIN,class_power_nazrin,do_cmd_class_power_aux_nazrin,10,
	"ダウジングロッド","それは周囲のアイテム・罠・モンスターを感知する。"},

	//フラワーウィザラウェイ
	{40, 20, 50,5,4,	MON_LETTY,class_power_letty,do_cmd_class_power_aux_letty,2,
	"萎れた花","それは視界内に冷気攻撃を放つ。"},
	//アンデュレイションレイ
	{35, 10, 70,5,3,MON_LETTY,class_power_letty,do_cmd_class_power_aux_letty,3,
	"雪女のブローチ","それは睡眠耐性を無視して敵を高確率で眠らせるビームを放つ。ユニークモンスターには効かない。"},

	//死なない殺人鬼
	{60, 20, 100,5,8,MON_YOSHIKA,class_power_yoshika,do_cmd_class_power_aux_yoshika,2,
	"血に餓えた毒爪","それを使うと周囲のモンスター全てに隣接攻撃し、さらに生者から体力を吸収する。武器使用時には使えない。"},
	//ドヨースピア
	{20, 0, 30,10,1,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,0,
	"かび臭い魔法書","それは近くのターゲット一体に複数回の無属性攻撃を行う。"},
	//セントエルモピラー
	{40, 25, 85,6,4,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,3,
	"焦げ臭い魔法書","それは指定した位置にプラズマ属性の巨大なボールを発生させる。"},
	//サイレントセレナ
	{40, 40, 100,4,10,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,5,
	"三日月の帽子飾り","それは隣接した敵に強力な暗黒属性攻撃を放つ。"},
	//賢者の石
	{150, 70, 128,5,20,	MON_PATCHOULI,class_power_patchouli,do_cmd_class_power_aux_patchouli,8,
	"賢者の石","それはこのフロア限定で「賢者の石」を召喚する。賢者の石は配下扱いで強力な元素魔法で攻撃する。"},

	//天狗道の開風
	{50, 20, 60,3,7,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,2,
	"天狗の葉団扇","それは竜巻属性の広範囲ビーム攻撃を放つ。"},
	//猿田彦の先導
	{65, 30, 90,4,9,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,3,
	"ブン屋の手帖","それを使うと一直線上に竜巻属性の攻撃を行い敵を吹き飛ばしつつ移動し、壁か吹き飛ばせない敵に当たると止まる。"},
	//幻想風靡
	{120, 75, 128,1,30,	MON_AYA,class_power_aya,do_cmd_class_power_aux_aya,5,
	"天狗の高下駄","それは短時間の間最高の加速をもたらす。"},

	//マルチプリケイティブヘッド
	{60, 20, 80,5,7,	MON_SEKIBANKI,class_power_banki,do_cmd_class_power_aux_banki,4,
	"ろくろ首のマント","それを使うと赤蛮奇の頭の分身が出現する。頭の強さはプレイヤーのレベルに依存する。"},

	//リトルバタリオン
	{40, 0, 60,5,4,	MON_MYSTIA,class_power_mystia,do_cmd_class_power_aux_mystia,4,
	"夜雀のピアス","それは鳥系のモンスターを配下として数体呼び寄せる。"},

	//ハングリータイガー
	{30, 15, 60,4,5,	MON_SHOU,class_power_shou,do_cmd_class_power_aux_shou,1,
	"虎縞のベスト","それを使うと数グリッド分一直線に突進し、小さな敵は吹き飛ばす。"},
	//レイディアントトレジャーガン
	{70, 30, 110,3,12,	MON_SHOU,class_power_shou,do_cmd_class_power_aux_shou,6,
	"毘沙門天の宝塔","それは強力な無属性のレーザーを広範囲に放つ。レーザーが当たった壁を宝石に変える。"},

	//毒ブレス
	{35, 10, 70,4,6,	MON_MEDICINE,class_power_medi,do_cmd_class_power_aux_medi,2,
	"鈴蘭の花","それは現在HPの1/3の威力の毒属性ブレスを放つ。"},
	//ガシングガーデン
	{50, 30, 100,3,9,	MON_MEDICINE,class_power_medi,do_cmd_class_power_aux_medi,5,
	"毒素の塊","それは視界内の全てに対し毒属性攻撃を放つ。"},

	//ゴーストスポット
	{30, 10, 50,3,4,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,2,
	"亡霊の提灯","それは指定したターゲットの周辺に大量の幽霊系モンスターを呼び出す。"},
	//ギャストリドリーム
	{40, 20, 70,4,7,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,3,
	"亡霊の額烏帽子","それは指定したターゲットに強力な精神攻撃を仕掛ける。"},
	//鳳蝶紋の死槍
	{25, 30, 80,4,5,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,5,
	"亡霊の大扇","それは地獄属性の強力なビームを放つ。"},
	//華胥の永眠
	{60, 30, 120,3,12,	MON_YUYUKO,class_power_yuyuko,do_cmd_class_power_aux_yuyuko,6,
	"西行妖の花弁","それは周囲の敵を高確率でフロアから消し去る。ユニークモンスターには効果がない。"},

	//目利き
	{30, 0, 100,8,4,	0,class_power_sh_dealer,do_cmd_class_power_aux_sh_dealer,3,
	"古道具屋の眼鏡","それはアイテムを一つ鑑定する。レベル30以降は完全な*鑑定*を行える。"},
	//テリブルスーヴニール
	{55, 20, 80,5,8,MON_SATORI,class_power_satori,do_cmd_class_power_aux_satori,2,
	"第三の目","それは視界内の敵全てに強力な精神攻撃を行う。アンデッドには特に効果が高い。"},

	//マウンテンエコー
	{50, 10, 100,5,4,MON_KYOUKO,class_power_kyouko,do_cmd_class_power_aux_kyouko,2,
	"山彦のチョーカー","それを使うと最後に聞いた呪文をオウム返しすることができる。"},
	//ロングレンジエコー
	{35, 20, 60,4,2,MON_KYOUKO,class_power_kyouko,do_cmd_class_power_aux_kyouko,3,
	"山彦の手袋","それはターゲットの位置に轟音属性のボールを発生させる。"},

	//ガゴウジサイクロン
	{30, 10, 70,5,4,MON_TOZIKO,class_power_toziko,do_cmd_class_power_aux_toziko,1,
	"怨霊の烏帽子","それは自分のいる位置に強力な電撃属性のボールを発生させる。"},
	//入鹿の雷
	{70, 60, 120,3,10,MON_TOZIKO,class_power_toziko,do_cmd_class_power_aux_toziko,4,
	"朽ちた素焼きの壺","それはターゲットの位置に強力な電撃属性のボールを発生させる。HPが低いほど威力が上がる。"},

	//春の訪れ
	{40, 20, 100,7,5,MON_LILY_WHITE,class_power_lilywhite,do_cmd_class_power_aux_lilywhite,2,
	"春告精の帽子","それは周囲の地面を花畑や森にする。"},
	//飛んで井の中
	{50, 10, 80,4,8,MON_KISUME,class_power_kisume,do_cmd_class_power_aux_kisume,3,
	"釣瓶落としの桶","それを使うとターゲット近くに移動し、周辺に物理ダメージと水ダメージを与え、さらに周辺を水地形にし、一時的な火炎耐性を得る。"},
	//念写Ⅰ
	{120, 10, 90,16,4,MON_HATATE,class_power_hatate,do_cmd_class_power_aux_hatate,0,
	"筆型ストラップ付きカメラ","それは周囲の地形・アイテム・モンスターを感知する。トラップは感知できない。"},

	//十七条のレーザー
	{60, 20, 80,6,5,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,3,
	"天子の笏","それは破邪属性のビームを放つ。"},
	//縮地のマント
	{40, 35, 128,4,6,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,4,
	"仙人のマント","それを使うと視界内の任意の場所へテレポートする。"},
	//詔を承けては必ず慎め
	{120, 80, 128,2,30,MON_MIKO,class_power_miko,do_cmd_class_power_aux_miko,8,
	"聖人の宝剣","それは極めて強力な万能属性のビームを放つ。"},

	//狂喜の火男面
	{35, 10, 40,3,4,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,5,
	"ひょっとこの面","それは周囲のランダムな位置に火炎属性のボールを数回発生させる。"},
	//怒れる忌狼の面
	{40, 30, 70,4,5,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,6,
	"狼の面","それは使うと短距離を突進し、敵に当たると気属性のダメージを与える。"},
	//昂揚の神楽獅子
	{50, 40, 100,4,7,MON_KOKORO,class_power_kokoro,do_cmd_class_power_aux_kokoro,7,
	"獅子舞の装束","それは強力なプラズマ属性のビームを放つ。"},

	//虫の知らせ
	{25, 0, 50,5,1,MON_WRIGGLE,class_power_wriggle,do_cmd_class_power_aux_wriggle,0,
	"蛍","それは周囲のモンスターを感知する。"},
	//ツツガムシ
	{70, 30, 60,2,7,MON_WRIGGLE,class_power_wriggle,do_cmd_class_power_aux_wriggle,5,
	"恙虫","それは「恙虫の群れ」を召喚する。"},

	//幻想郷の開花
	{70, 40, 120,8,8,MON_YUUKA,class_power_yuuka,do_cmd_class_power_aux_yuuka,0,
	"向日葵","それは周囲の地面を花畑や森にする。"},

	//鳳凰卵
	{35, 5, 40,4,3,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,0,
	"仕込み卵","それは使うと混乱効果のあるボールを放つ。"},
	//飛翔韋駄天
	{60, 20, 70,5,4,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,4,
	"またたび","それは使うと一時的に浮遊と加速を得る。"},
	//奇門遁甲
	{50, 20, 80,4,7,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,5,
	"マヨヒガの茶碗","それは使うと一時的に酸・電撃・火炎・冷気攻撃への耐性を得る。"},
	//飛翔毘沙門天
	{80, 40, 100,3,9,MON_CHEN,class_power_chen,do_cmd_class_power_aux_chen,6,
	"妖獣のピアス","それは使うと周囲に魔力属性のボールを撒き散らし、その後一瞬で離脱する。"},
//100
	//水弾
	{35, 0, 30,7,1,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,0,
	"ひしゃく","それは低威力の水属性ボルトを放つ。"},
	//ディープヴォーテックス
	{65, 20, 60,4,2,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,2,
	"バケツ","それは周辺に水属性攻撃を行い、さらに地形を水にする。水地形上で使うと威力が上がる。"},
	//船底のヴィーナス
	{85, 40, 100,4,8,MON_MURASA,class_power_murasa,do_cmd_class_power_aux_murasa,4,
	"舟幽霊の帽子","それは視界内全ての床を水没させ、敵に水属性ダメージを与える。"},

	//歴史隠蔽Ⅲ
	{80, 20, 100,5,10,MON_KEINE,class_power_keine1,do_cmd_class_power_aux_keine,5,
	"歴史家の筆","それは指定したシンボルの敵をフロアから消し去る。消える敵のレベルに応じてダメージを受ける。抵抗されることもある。"},
	//オールドヒストリー
	{40, 20, 100,1,12,MON_KEINE,class_power_keine2,do_cmd_class_power_aux_keine,3,
	"歴史家の帽子","それは今いるフロアの地形とアイテムを全て感知し、視界内の敵の情報を知る。"},
	//無何有浄化
	{120, 70, 128,2,16,MON_KEINE,class_power_keine2,do_cmd_class_power_aux_keine,6,
	"白沢の角","それは自分を中心に極めて強力な時空属性のボールを発生させる。ただし幻想郷の住人には効果がない。"},

	//地震
	{30, 10, 60,4,1,MON_YUGI,class_power_yugi,do_cmd_class_power_aux_yugi,0,
	"鬼の下駄","それを使うと地震が起こりダンジョンの壁が崩れる。"},
	//鬼気狂瀾
	{40, 25, 100,4,8,MON_YUGI,class_power_yugi,do_cmd_class_power_aux_yugi,4,
	"鬼の酒","それを使うと一定時間狂戦士化し加速する。"},

	//陰陽玉
	{40, 10, 70,6,4,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,4,
	"陰陽玉","それは破邪属性のボールを放つ。"},
	//夢想封印
	{80, 40, 128,5,12,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,6,
	"大幣","それは周囲のランダムな敵に無属性のボールを連続で放つ。レベルが上がると威力、数、爆発半径が上昇する。壁の隣にいると使えない。"},
	//結界札
	{50, 30, 100,5,9,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,8,
	"結界札","それを使うと一時的に元素攻撃への耐性を得る。レベル40以降はAC上昇も得る。"},
	//陰陽飛鳥井
	{70, 70, 128,3,10,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,12,
	"陰陽玉(大)","それは強力な破邪属性のロケットを放つ。"},
	//博麗幻影
	{200, 80, 128,1,30,	MON_REIMU,class_power_reimu,do_cmd_class_power_aux_reimu,13,
	"博麗の巫女のリボン","それを使うとこのフロア限定で『博麗　霊夢』の幻影が配下として現れる。"},

	//ハイスピードバウンス
	{60, 30, 110,5,6,	MON_KAGEROU,class_power_kagerou,do_cmd_class_power_aux_kagerou,2,
	"狼女の爪","それを使うと離れたターゲットに飛びかかって攻撃する。"},
	//フォーリンブラスト
	{50, 30, 60,3,4,	MON_SHIZUHA,class_power_shizuha,do_cmd_class_power_aux_shizuha,1,
	"落ち葉","それは突風のビームを放つ。"},
	//スイートポテトルーム
	{30, 0, 70,8,3,		MON_MINORIKO,class_power_minoriko,do_cmd_class_power_aux_minoriko,2,
	"お芋","それはアイテム「焼き芋」を生成する。"},

	//白昼の客星
	{75, 25, 110,5,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,10,
	"星が描かれたネクタイ","それはターゲットの場所に閃光属性のボールを発生させる。"},
	//ミラクルフルーツ
	{100, 35, 120,6,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,12,
	"「守矢神社」と書かれた箱","それはアイテム「ミラクルフルーツ」を生成する。"},
	//海が割れる日
	{50, 35, 110,4,7,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,14,
	"風祝のお祓い棒","それは特殊なビームを放つ。水棲の敵に大ダメージを与え、その他の敵には水属性ダメージを与える。"},
	//神代大蛇
	{80, 50, 100,3,12,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,17,
	"白蛇の首飾り","それは空間歪曲属性の強力なビームを放つ。"},
	//120 手管の蝦蟇
	{80, 50, 100,3,12,		MON_SANAE,class_power_sanae,do_cmd_class_power_aux_sanae,18,
	"蛙の首飾り","それはカオス属性の強力なブレスを放つ。"},

	//マイハートブレイク
	{50,5, 90,6,3,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,1,
	"吸血鬼の日傘","それを使うとアイテムを強力に投擲する。"},
	//スカーレットシュート
	{60,25, 90,5,6,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,2,
	"三段ロケットの模型","それは地獄の劫火属性のロケットを放つ。"},
	//スピアオブグングニル
	{70,60, 128,5,8,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,7,
	"血の色の槍","それは万能属性の強力なビームを放つ。"},
	//レッドマジック
	{50,80, 128,1,20,		MON_REMY,class_power_remy,do_cmd_class_power_aux_remy,9,
	"吸血鬼の紅茶","それは極めて強力な視界内攻撃を行う。威力は魅力に大きく依存し、清浄な体を持つ敵に大きなダメージを与える。"},

//125
	{60,25, 120,3,20,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,6,
	"スキマ妖怪の手袋","それは視界内に落ちているアイテム数個を探索拠点へ引っ張り込む。"},
	{80,70, 128,4,8,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,9,
	"胡散臭げな扇","それは魔法書を一冊消費し、それに書かれた魔法をひとつ発動する。"},
	{60,40, 100,5,7,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,11,
	"月の超古酒","それは現在HPとMPを反転させる。"},
	{75,50, 128,3,12,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,12,
	"リボンがあしらわれたスキマ","それを使うと指定した場所へテレポートできる。テレポートで侵入不可能な地形には出られない。"},
	{50,30, 110,3,8,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,13,
	"廃電車","それは一直線上の敵にダメージを与えて吹き飛ばす。巨大な敵には効果が薄い。"},
	{120,10, 128,1,50,		MON_YUKARI,class_power_yukari,do_cmd_class_power_aux_yukari,16,
	"捻じ曲がった日傘","それは現在位置に探索拠点の入り口を作り出す。広い場所でないと使えず、クエスト遂行中のフロア(100階と127階含む)では使えない。"},

	//希望の面効果
	{100, 50, 128,4,15,MON_KOKORO,class_power_extra,do_cmd_class_power_aux_extra,1,
	"希望の面(新)","それは体力を500回復し経験値・ステータスを復活させる。"},

	//大型水鉄砲
	{50,10, 50,8,2,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,6,
	"河童の水鉄砲","それは水属性のボルトを乱射する。"},
	//菊一文字コンプレッサー
	{60,30, 80,5,6,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,10,
	"エンジニアのリュック","それはマップ上下方向のみに爆発する短射程の水属性ボールを放つ。"},
	//オプティカルカモフラージュ
	{90,30, 120,1,18,	MON_NITORI,class_power_nitori,do_cmd_class_power_aux_nitori,12,
	"光学迷彩スーツ","それを使うと短時間透明になって敵から見えにくくなる。"},

	//転読
	{150,70, 128,3,25,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,0,
	"魔人経巻","それは一定時間身体能力を大幅に上昇させる。レベル30で加速、レベル35で元素一時耐性が追加される。"},
	//インドラの雷
	{20,5, 40,4,3,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,1,
	"魔住職の数珠","それはターゲットの位置に電撃属性ボールを発生させる。"},
	//ハリの制縛
	{30,10, 60,3,5,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,3,
	"独鈷杵","それは周囲の敵を朦朧、金縛りにしようと試みる。"},
	//アーンギラサヴェーダ
	{100,70, 128,3,20,	MON_BYAKUREN,class_power_byakuren,do_cmd_class_power_aux_byakuren,8,
	"蓮の花弁","それは強力な閃光属性レーザーを放つ。"},

	//正体不明の種
	{100,20, 80,4,10,	MON_NUE,class_power_nue,do_cmd_class_power_aux_nue,2,
	"謎の触手","それは敵一体を正体不明にする。正体不明になった敵は他の敵から攻撃される。ユニークモンスターにはやや効きづらく、カオス耐性を持つ敵には効かない。"},
	//遊星よりの弾幕Ｘ
	{120,30, 110,1,20,	MON_NUE,class_power_nue,do_cmd_class_power_aux_nue,6,
	"UFO","それは視界内の全てに対し完全にランダムな属性の強力な攻撃を行う。"},

	//隙間の折りたたみ傘
	{80, 20, 100,5,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,4,
	"隙間の折りたたみ傘","それは壁に向けて使うと壁の向こうへテレポートする。マップ端へ向けた場合マップの反対側へ出る。通常のテレポートで侵入できない地形には出てこれない。"},
	//リバースヒエラルキー
	{50, 0, 40,2,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,7,
	"天邪鬼のサンダル","それは敵一体と位置を交換する。視界外の敵にも効果がある。"},
	//小槌レプリカ
	{60, 30, 80,6,8,	MON_SEIJA,class_power_seija,do_cmd_class_power_aux_seija,10,
	"打ち出の小槌(レプリカ)","それは隣接した敵一体に大ダメージを与える。"},

	//メガフレア
	{90,30, 100,6,8,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,5,
	"核反応制御棒","それは強力な核熱属性のボールを放つ。"},
	//ヘルズトカマク
	{80,40, 100,3,10,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,10,
	"八咫烏の眼","それは周囲を*破壊*する。"},
	//ギガフレア
	{65,50, 128,1,25,	MON_UTSUHO,class_power_utsuho,do_cmd_class_power_aux_utsuho,12,
	"人工太陽","それは極めて強力な核熱属性のレーザーを放つ。"},

	//原因不明の熱病
	{60,10, 50,3,7,	MON_YAMAME,class_power_yamame,do_cmd_class_power_aux_yamame,2,
	"子蜘蛛の群れ","それは視界内の敵を混乱、朦朧させる。"},
	//樺黄小町
	{60,30, 80,6,5,	MON_YAMAME,class_power_yamame,do_cmd_class_power_aux_yamame,5,
	"土蜘蛛の牙","それは隣接した敵にダメージを与える。毒耐性を持たない敵には三倍のダメージを与え、低確率で一撃で倒す。"},

	//諏訪清水
	{35,20, 70,6,5,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,2,
	"目玉のついた笠","それは水属性のブレスを放つ。威力は現在HPの1/4になる。"},

	//idx150
	{25,10, 90,4,6,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,3,
	"蛙の石像","それは隣接した何もない場所1グリッドに岩を生成する。"},
	//大地の湖
	{50,30, 120,1,15,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,7,
	"翡翠","それを使うと一定時間壁を抜けられるようになる。"},
	//祟り神召喚
	{90,50, 100,2,15,	MON_SUWAKO,class_power_suwako,do_cmd_class_power_aux_suwako,9,
	"古代の鉄輪","それはモンスター「祟り神」を配下として一体呼び出す。"},

	//こっくりさん
	{30,10, 40,5,4,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,0,
	"プランシェット","それは悪魔・妖怪・アンデッド一体を配下として呼び出す。"},
	//ユーニラタルコンタクト
	{45,25, 80,4,8,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,4,
	"式の帽子","それは敵一体を高確率で魅了する。ユニークモンスターには効果がない。"},
	//八千万枚護摩
	{150,70, 128,8,16,	MON_RAN,class_power_ran,do_cmd_class_power_aux_ran,6,
	"三途の川幅の計算書","それは視界内全てに対し破邪属性攻撃を行う。"},

	//無念無想の境地
	{45,10, 90,3,6,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,6,
	"仙桃","それは一定時間物理防御力と魔法防御力を上昇させる。"},
	//カナメファンネル
	{55,30, 70,4,7,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,7,
	"浮遊要石","それはこのフロア限定で「カナメファンネル」を召喚する。"},
	//不譲土壌の剣
	{55,30, 100,3,12,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,9,
	"巨大な要石","それは周囲の狭い範囲を*破壊*する。"},
	//全人類の緋想天
	{60,40, 128,1,45,	MON_TENSHI,class_power_tenshi,do_cmd_class_power_aux_tenshi,10,
	"天界の宝剣","それは極めて強力な気属性のビームを放つ。"},

	//華光玉
	{30,20, 60,6,4,	MON_MEIRIN,class_power_meirin,do_cmd_class_power_aux_meirin,2,
	"じょうろ","それは気属性の玉を放つ。"},
	//猛虎内勁
	{40,40, 100,5,9,	MON_MEIRIN,class_power_meirin,do_cmd_class_power_aux_meirin,8,
	"武道家の帽子","それは低下した腕力を復活させ、一定時間の大幅な腕力上昇と士気高揚をもたらす。"},

	//ジェラシーボンバー
	{40,10, 70,6,5,MON_PARSEE,class_power_parsee,do_cmd_class_power_aux_parsee,4,
	"藁人形","それは精神攻撃属性のボールを放つ。自分の体力が低いほど威力が上昇する。"},
	//積怨返し
	{160,80, 128,1,50,MON_PARSEE,class_power_parsee,do_cmd_class_power_aux_parsee,8,
	"橋姫の鉄輪","それは自分のいる場所に地獄の劫火属性の巨大なボールを発生させる。威力はこのフロアで自分が受けたダメージの合計になり、一度使うとリセットされる。"},

	//*鑑識*
	{50,20, 90,5,7,MON_EIKI,class_power_eiki,do_cmd_class_power_aux_eiki,3,
	"是非曲直庁の記録簿","それは指定したアイテムの性能を完全に知る。"},
	//ラストジャッジメント
	{180,90, 128,3,30,MON_EIKI,class_power_eiki,do_cmd_class_power_aux_eiki,7,
	"悔悟の棒","それは極めて強力な破邪属性のビームを放つ。"},

	//指向性テレポート
	{65,20, 70,10,3,MON_SUMIREKO,class_power_sumireko,do_cmd_class_power_aux_sumireko,3,
	"秘術師のマント","それを使うと指定した方向へ一定距離のテレポートを行う。テレポート不可能な地形に出ようとしたら周辺のランダムな場所へ出る。"},
	//サイコキネシスアプリ
	{90,50, 110,4,15,MON_SUMIREKO,class_power_sumireko,do_cmd_class_power_aux_sumireko,11,
	"異世界のタブレット","それは視界内全てに強力な時空属性攻撃を仕掛ける。"},

	//殴殺の流雲
	{30,10, 50,8,3,MON_ICHIRIN,class_power_ichirin,do_cmd_class_power_aux_ichirin,2,
	"妖怪行者の袈裟","それは自分の隣に竜巻属性のボールを発生させる。"},
	//雲上地獄突き
	{35,20, 75,6,5,MON_ICHIRIN,class_power_ichirin,do_cmd_class_power_aux_ichirin,3,
	"雲入道の欠片","それは気属性のロケットを放つ。"},

	//鳳翼天翔
	{90,15, 75,8,5,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,2,
	"焼き鳥","それは核熱属性のロケットを放つ。"},
	//無差別発火の符
	{70,5, 40,8,5,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,5,
	"発火符の束","それは周囲のランダムな数か所に爆発のルーンを設置する。"},
	//フェニックスの尾
	{50,30, 80,8,7,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,6,
	"フェニックスの尾","それは隣接した敵一体に火炎属性攻撃を行い、非ユニークのアンデッドを高確率で一撃で倒す。"},
	//フジヤマヴォルケイノ
	{60,40, 120,1,25,MON_MOKOU,class_power_mokou,do_cmd_class_power_aux_mokou,8,
	"竹炭","それは視界内全てに対し強力な火炎属性攻撃を行う。"},

	//エクスパンデッド・オンバシラ
	{70,20, 80,4,8,MON_KANAKO,class_power_kanako,do_cmd_class_power_aux_kanako,2,
	"御柱","それは敵を吹き飛ばすビームを放つ。テレポート耐性を無視するが巨大な敵は吹き飛ばない。"},
	//神の粥
	{30,10, 60,4,6,MON_KANAKO,class_power_kanako,do_cmd_class_power_aux_kanako,3,
	"筒粥","それは周囲の地形・アイテム・トラップ・モンスターを感知し少し腹を満たす。"},

	//皿投げ
	{30,0, 50,12,2,MON_FUTO,class_power_futo,do_cmd_class_power_aux_futo,2,
	"皿","それは破片属性の反射されないボルトを放つ。"},
	//爆弾設置
	{60,30, 80,9,4,MON_3FAIRIES,class_power_star,do_cmd_class_power_aux_star,3,
	"爆弾","それは足元に爆弾を設置する。爆弾は「通常は」10ターンで爆発し半径3の破片属性の強力なボールを発生させる。この爆発ではプレイヤーもダメージを受ける。爆弾は一部の属性攻撃に晒されると誘爆したり爆発タイミングが変わることがある。"},

	//魔彩光の上海人形
	{50,15, 60,5,3,MON_ALICE,class_power_alice,do_cmd_class_power_aux_alice,2,
	"上海人形","それは閃光属性のビームを放つ。"},
	//首吊り蓬莱人形
	{50,40, 100,5,8,MON_ALICE,class_power_alice,do_cmd_class_power_aux_alice,7,
	"蓬莱人形","それは強力な暗黒属性のビームを放つ。"},
//180
	//ノイズメランコリー
	{60,25, 80,7,5,MON_LUNASA,class_power_lunasa,do_cmd_class_power_aux_lunasa,3,
	"ヴァイオリン","それは視界内の敵を減速、混乱、恐怖、朦朧させる。"},
	//ソウルゴーハッピー
	{50,20, 70,7,5,MON_MERLIN,class_power_merlin,do_cmd_class_power_aux_merlin,3,
	"トランペット","それは一定時間加速と肉体強化をもたらす。ただし周囲の敵が起き視界内の敵も加速する。"},
	//ソウルノイズフロー
	{50,15, 60,7,5,MON_LYRICA,class_power_lyrica,do_cmd_class_power_aux_lyrica,3,
	"キーボード","それは周囲のモンスター、扉、罠、アイテムを感知する。"},

	//ヘルエクリプス
	{50,25, 75,3,7,MON_CLOWNPIECE,class_power_clownpiece,do_cmd_class_power_aux_clownpiece,2,
	"地獄の松明","それは今いる部屋を明るくし、視界内の敵に精神攻撃を行う。"},
	//ストライプドアビス
	{120,85, 128,1,25,MON_CLOWNPIECE,class_power_clownpiece,do_cmd_class_power_aux_clownpiece,6,
	"地獄の道化の帽子","それは視界内の全てに対し地獄の劫火属性攻撃を放つ。"},

	//緋色の悪夢
	{25,0, 45,5,3,MON_DOREMY,class_power_doremy,do_cmd_class_power_aux_doremy,1,
	"夢塊","それは敵一体を高確率で眠らせる。睡眠耐性を無視するがユニークモンスターには効果がない。"},
	//藍色の愁夢
	{45,30, 85,12,4,MON_DOREMY,class_power_doremy,do_cmd_class_power_aux_doremy,3,
	"安眠枕","それは敵一体に精神攻撃を仕掛ける。眠っている敵に使うと威力が増し、また敵を起こしにくい。"},

	//厄神様のバイオリズム＋厄吸い
	{40,0, 40,4,5,MON_HINA,class_power_hina,do_cmd_class_power_aux_hina,1,
	"渦巻きアップリケ","それは呪われた品を装備しているときに使うと解呪して空腹を満たす。"},
	//ブロークンアミュレット
	{75,10, 75,2,5,MON_HINA,class_power_hina,do_cmd_class_power_aux_hina,5,
	"不吉な感じのするお守り","それは呪われたアミュレットをひとつ消費し、強力な破片属性のボールを発生させる。"},

	//バニシングエブリシング
	{25,10, 80,5,4,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,4,
	"トランプのカード","それを使うと中距離のテレポートをする。"},
	//殺人ドール
	{70,30, 100,3,10,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,8,
	"銀のナイフ","それは周囲のランダムな敵に対し大量の破片属性ボルトを放つ。"},
	//ザ・ワールド
	{160,75, 128,1,50,MON_SAKUYA,class_power_sakuya,do_cmd_class_power_aux_sakuya,17,
	"止まった銀時計","それを使うと時間が停止し、数回連続で行動できる。"},

	{30,5, 45,5,4,MON_YATSUHASHI,class_power_yatsuhashi,do_cmd_class_power_aux_yatsuhashi,1,
	"古びた琴","それは敵一体を混乱、減速させる。レベル30以降は視界内の敵全てに効果が及ぶ。"},
	{30,5, 45,5,4,MON_BENBEN,class_power_benben,do_cmd_class_power_aux_benben,3,
	"古びた琵琶","それは閃光属性のビームを放つ。"},

	//v1.1.85 ランドパーカス→雷神の怒り
	{60,25, 95,7,3,MON_RAIKO,class_power_raiko,do_cmd_class_power_aux_raiko,4,
	"ビーターブーツ","それは自分を中心とした巨大な電撃属性のボールを発生させ、周囲の地形を「魔法の雷雲」に変える。"},
	//高速和太鼓ロケット
	{30,25, 120,4,5,MON_RAIKO,class_power_raiko,do_cmd_class_power_aux_raiko,8,
	"和太鼓","それは破片属性のロケットを放つ。威力は現在HPの1/3になる。"},
	//変化
	{80,30, 128,5,8,MON_MAMIZOU,class_power_mamizou,do_cmd_class_power_aux_mamizou,3,
	"狸の葉っぱ","それを使うと近くの指定したモンスターに一定時間変身する。変身中はリボン・アミュレット以外の装備が無効化され、変身したモンスターの能力を'U'コマンドで使うことができる。変身の成功率はレベル、知能、賢さ、相手のレベルで決まる。"},

	//浄化作戦
	{80,30, 90,3,8,MON_RINGO,class_power_ringo,do_cmd_class_power_aux_ringo,6,
	"金属製の蜘蛛","それは周囲の妖怪、悪魔、獣を高確率でフロアから消滅させる。ユニークモンスターには効果がない。"},
	//セプテンバーフルムーン
	{200,90, 120,1,40,MON_RINGO,class_power_ringo,do_cmd_class_power_aux_ringo,9,
	"月の団子","それは9ターンの間無敵のバリアを張る。効果が切れるとき少し隙ができる。"},
	//団子作成
	{30,0, 50,7,3,MON_SEIRAN,class_power_seiran,do_cmd_class_power_aux_seiran,0,
	"赤く濡れた杵","それを使うとアイテム「団子」を作る。"},

	//idx200 仙香玉兎
	{60,20, 60,4,7,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,1,
	"月の香炉","それは周囲のモンスターを混乱・朦朧させる。"},
	//壺中の天地
	{80,10, 100,3,8,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,2,
	"大きな薬壺","それに飛び込むと下のフロアに行ける。すでに最下層の場合何も起きない。"},
	//ライフゲーム
	{50,40, 110,3,10,MON_EIRIN,class_power_eirin,do_cmd_class_power_aux_eirin,5,
	"八意印の薬箱","それは隣接した生物一体を完全に回復させる。"},

	//蓬莱の玉の枝
	{255,100, 128,5,25,MON_KAGUYA,class_power_kaguya,do_cmd_class_power_aux_kaguya,10,
	"優曇華の盆栽","それは様々な属性の巨大な球を放つ。"},
	//須臾の術Ⅰ
	{70,40, 100,5,8,MON_KAGUYA,class_power_kaguya,do_cmd_class_power_aux_kaguya,12,
	"月の牛車","それを使うと指定した場所まで一瞬で移動する。テレポートバリアなどを無視するが既知の通路が通った場所にしか行けない。"},

	//穢身探知型機雷
	{50,20, 80,6,5,MON_SAGUME,class_power_sagume,do_cmd_class_power_aux_sagume,1,
	"月の機雷","それを使うとこのフロア限定でモンスター「穢身探知型機雷」を呼び出す。機雷は分裂し破邪属性の爆発を起こす。"},
	//片翼の白鷺
	{90,60, 110,2,14,MON_SAGUME,class_power_sagume,do_cmd_class_power_aux_sagume,4,
	"白鷺の羽根","それを使うと強力な閃光属性のボールを発生させて中距離テレポートする。反テレポートに妨害されない。"},

	//フェムトファイバー
	{120,30, 120,1,30,MON_REISEN2,class_power_reisen2,do_cmd_class_power_aux_reisen2,4,
	"フェムトファイバーの組紐","それはモンスター一体を縛り上げて移動を妨害し、中確率で行動も妨害する。素早い敵・力強い敵・巨大な敵・ユニークモンスターには効きづらい。"},

	//次元の扉Ⅱ
	{60,20, 80,7,5,MON_REISEN2,class_power_toyohime,do_cmd_class_power_aux_toyohime,6,
	"愚者の封書","それは視界内のモンスター一体を指定した場所にテレポートさせる。テレポート耐性をもつ敵対的なモンスターには効果がない。"},
	//月の公転周期の罠
	{255,110, 127,1,100,MON_TOYOHIME,class_power_toyohime,do_cmd_class_power_aux_toyohime,8,
	"賢者の封書","それを発動すると以後モンスターがテレポートしたときフロアから消滅し、さらにモンスターによる召喚魔法が阻害されるようになる。"},

	//神降ろし「金山彦命」
	{65,10, 90,1,20,MON_SANAE,class_power_yorihime,do_cmd_class_power_aux_yorihime,2,
	"パラジウム合金の柱","それを使うと「金山彦命」が降臨し、一定時間敵の刃物による攻撃を防ぐ。"},
	//神降ろし「天津甕星」
	{120,60, 110,7,6,MON_YORIHIME,class_power_yorihime,do_cmd_class_power_aux_yorihime,6,
	"蕎麦焼酎","それを使うと「天津甕星」が降臨し、視界内のモンスターを死の光線で攻撃する。"},

	//邪穢在身
	{70,30, 90,3,10,	MON_HECATIA1,class_power_hecatia,do_cmd_class_power_aux_hecatia,4,
	"女神の首輪","それは視界内のアンデッドを高確率で魅了する。"},
	//ルナティックインパクト
	{240,90, 128,3,30,	MON_HECATIA1,class_power_hecatia,do_cmd_class_power_aux_hecatia,8,
	"月面クレーター","それは強力な隕石属性攻撃を行う。"},

	//原始の神霊界
	{70,20, 100,7,7,	MON_JUNKO,class_power_junko,do_cmd_class_power_aux_junko,4,
	"旧き仙霊の冠","それは隣接したモンスター一体を一段階ランクアップさせる。"},

	//v1.1.33 石像化
	{60,10,70,3,6,	MON_AUNN,class_power_aunn,do_cmd_class_power_aux_aunn,3,
	"狛犬の台座","それを使うと石像化しモンスターから認識されなくなる。視界内にモンスターがいる状態では使用できない。ダメージを受けたり待機、休憩、飲食など以外の行動をすると効果が切れる。"},

	//v1.1.34 クリミナルサルヴェイション
	{160,90, 127,1,30,MON_NARUMI,class_power_narumi,do_cmd_class_power_aux_narumi,8,
	"お地蔵様の前掛け","それは一度だけ地獄の業火属性の攻撃を無効化する。"},

	//v1.1.35 山姥の包丁研ぎ
	{ 120,30, 127,5,8,MON_NEMUNO,class_power_nemuno,do_cmd_class_power_aux_nemuno,3,
		"山姥の砥石","それは一時的に武器に切れ味をもたらす。既に切れ味の良い武器はさらに強化される。" },

	//v1.1.37 小鈴怨霊憑依
	{ 150,40, 80,1,15,MON_KOSUZU,class_power_kosuzu,do_cmd_class_power_aux_kosuzu,4,
		"古い恋文の束","それを使うと怨霊に取り憑かれる。取り憑かれている間は種族と特技が一時的に変化する。" },

	//v1.1.39 ラルバ変異
	{ 100,5, 80,4,8,MON_LARVA,class_power_larva,do_cmd_class_power_aux_larva,7,
	"アゲハ蝶の蛹","それを使うとランダムな突然変異を一つ得る。" },

	//v1.1.41 IDX220 タナバタスターフェスティバル
	{ 70,20, 90,6,6,MON_MAI,class_power_mai,do_cmd_class_power_aux_mai,4,
		"笹","それはHPを中程度回復しステータス異常を治療する。騎乗中(あるいはダンス中)の配下がいればそのHPも少し回復する。" },

	//v1.1.43 新クラウンピース
	{ 120,10, 60,10,1,MON_CLOWNPIECE,class_power_vfs_clownpiece,do_cmd_class_power_aux_vfs_clownpiece,0,
	"篝火","それはモンスター一体を狂気に冒す。狂気に冒されたモンスターは近くのモンスターを攻撃する。ユニークモンスターや通常の精神を持たないモンスターには効きづらい。" },

	//v1.1.44 新うどんげ
	{ 160,80, 120,1,25,MON_UDONGE,class_power_udonge_d,do_cmd_class_power_aux_udonge_d,12,
		"しなびた兎耳","それは敵からの攻撃のダメージを三回無効化するバリアを張る。" },

	//v1.1.45 フォゲットユアネーム
	{ 80,10, 70,3,10,MON_SATONO,class_power_satono,do_cmd_class_power_aux_mai,2,
	"茗荷","それは隣接したモンスター全てを現在のフロアから高確率で追放する。ユニークモンスターには効果がない。" },

	//v1.1.46 スレイブロバー
	{ 70,10, 90,2,8,MON_JYOON,class_power_jyoon,do_cmd_class_power_aux_jyoon,7,
		"疫病神の腕輪","それを使うと隣接したモンスター一体の加速と無敵化を奪い取って自分を強化することができる。" },

	//v1.1.48
	{ 120,30, 90,1,20,MON_SHION_2,class_power_shion,do_cmd_class_power_aux_shion,4,
		"黒猫の縫いぐるみ","それは一時的に隠密能力を高める。レベル40以降は壁抜け能力も追加する。" },
	{ 100,60, 120,1,25,MON_SHION_2,class_power_shion,do_cmd_class_power_aux_shion,7,
		"貧乏神の鉢","それは視界内のモンスターのHPを半減させる。抵抗されると無効。ユニークモンスターには効果がない。" },

	//v1.1.57 摩多羅隠岐奈「七星の剣」
	{ 120,30, 100,7,5,MON_OKINA,class_power_okina,do_cmd_class_power_aux_okina,8,
		"天龍の鱗","それを使うと一時的に武器「七星剣」を生成して装備する。手の装備が空いているときにしか使えない。" },

	//v1.1.59 エンシェントシップ
	{ 120,0, 40,1,20,MON_FUTO,class_power_futo,do_cmd_class_power_aux_futo,3,
		"古代の舟","それは騎乗用特殊モンスター「エンシェントシップ」を呼び出す。" },

	//v1.1.65 永琳の丹
	{ 70, 1, 50,7,4,MON_EIRIN,class_power_extra,do_cmd_class_power_aux_extra,2,
		"永琳の丹","それは体力を100回復し毒・切り傷・朦朧・幻覚・喘息発作を治療しステータスを復活させる。" },

	//v1.1.65 瓔花石積み
	{ 60,1, 20,1,10,MON_EIKA,class_power_eika,do_cmd_class_power_aux_eika,0,
		"賽の河原の積み石","それは現在地に守りのルーンを作る。" },

	//v1.1.65 鬼華扇　無間地獄の罠
	{ 150, 20, 120,1,30,MON_KASEN,class_power_oni_kasen,do_cmd_class_power_aux_oni_kasen,6,
		"鬼の腕と書かれた箱","それは周囲の生命のあるモンスターを一撃で倒し、あなたの体力を回復する。抵抗されると無効。" },

	//v1.1.66
	{ 90,15, 45,1,10,MON_MAYUMI,class_power_mayumi,do_cmd_class_power_aux_mayumi,2,
		"埴輪","それは埴輪の兵士を召喚する。" },

	//v1.1.67 特技ではないがニワタリ神の力を実装
	{ 200, 20, 70,4,10,MON_KUTAKA,class_power_extra,do_cmd_class_power_aux_extra,3,
		"鶏の描かれた絵馬","それは咳を止める。" },

	//v1.1.68 氷塊生成
	{ 90, 10, 70,7,4,MON_CIRNO,class_power_cirno,do_cmd_class_power_aux_cirno,7,
		"カープボール","それは隣接した床一か所の地形を「氷塊」にする。" },

	//v1.1.69 ストーンベイビー
	{ 100, 0, 60,1,20,MON_URUMI,class_power_urumi,do_cmd_class_power_aux_urumi,1,
		"赤子の石像","それはモンスター一体を水に沈める。水辺が近くにないと使えない。ユニークモンスターや水耐性をもつモンスターには無効。" },

	//v1.1.71 フォロミーアンアフライド
	{ 50, 0, 70,1,10,MON_SAKI,class_power_saki,do_cmd_class_power_aux_saki,1,
		"組長のスカーフ","それはモンスター一体を配下に勧誘する。恐怖状態のモンスターに効果が高い。ユニークモンスターには無効。" },

	//v1.1.71 トライアングルチェイス
	{ 90, 70, 127,3,15,MON_SAKI,class_power_saki,do_cmd_class_power_aux_saki,7,
		"天馬の翼","それを使うとモンスター一体の隣まで一瞬で移動し、さらに一撃離脱する。離脱に失敗することもある。" },

	//v1.1.73 亀甲地獄
	{ 60, 30, 60,6,5,MON_YACHIE,class_power_yachie,do_cmd_class_power_aux_yachie,5,
		"大きな甲羅","それを使うと周囲のモンスターに遅鈍属性攻撃を行う。" },

	//v1.1.74 方形造形術
		{ 150, 30, 90,7,7,MON_KEIKI,class_power_keiki,do_cmd_class_power_aux_keiki,6,
		"古墳","それは自分の周囲の地形を岩石にする。" },

	//idx:240
	//v1.1.88 真・木隠れの技術
		{ 120,20, 90,3,10,	MON_TAKANE,class_power_takane,do_cmd_class_power_aux_takane,9,
		"山童の迷彩服","それを使うと短時間敵から認識されにくくなる。ただし森の中にいないと効果がない。" },

	//v1.1.89 積もり続けるマインダンプ
		{ 160,50, 120,3,15,	MON_MOMOYO,class_power_momoyo,do_cmd_class_power_aux_momoyo,6,
		"鉱業廃棄物","それは周囲の地形を石壁に変える。" },

	//視界内デバフ　山如特技にそのまま他職業で流用できそうなものがないので新設
		{ 120, 30, 80,8,7,MON_SANNYO,class_power_extra,do_cmd_class_power_aux_extra,4,
		"天然１００％妖怪の山産の煙草","それは周囲のモンスターを朦朧、減速、混乱させる。" },

	{0,0,0,0,0,0,NULL,NULL,0,"終端ダミー",""},
};

//サポートアイテムのテストコマンド　デバッグコマンド'S'
bool support_item_test(void)
{
	char tmp_val[160];
	int num = 0;
	int table_len;
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);

	table_len = sizeof(support_item_list) / sizeof(support_item_type)-2;
	sprintf(tmp_val, "0");
	if (!get_string(format("itemcard_Pval(max:%d): ",table_len), tmp_val, 3)) return FALSE;
	num = atol(tmp_val);

	if(num > table_len) num = table_len;
	if(num < 0) num=0;

	class_power_aux = support_item_list[num].func_aux;
	spell = &(support_item_list[num].func_table[support_item_list[num].func_num]);

	use_itemcard = TRUE;

	msg_format("あなたは「%s」を使った！",support_item_list[num].name);
	msg_format("IDX:%d[%s](%s)",num,spell->name,class_power_aux(support_item_list[num].func_num,TRUE));
	class_power_aux(support_item_list[num].func_num,FALSE);

	use_itemcard = FALSE;

	return TRUE;

}


//EXTRAモード用の「アイテムカード」に対しパラメータ付与する。apply_magicの代わりに呼ぶ。
//apply_magic()を呼んだ場合もtvがTV_ITEMCARDの場合r_idx=0でここに飛ぶ。
//lev:生成レベル
//r_idx:モンスターidx このidxに一致するカードのうち生成レベルに応じて選ばれる。0のとき生成レベルとレアリティに応じてランダムに選ばれる。
void apply_magic_itemcard(object_type *o_ptr, int lev, int r_idx)
{

	int card_idx = -1;
	int i;

	if(o_ptr->tval != TV_ITEMCARD)
	{
		msg_print("ERROR:apply_magic_itemcard()にTV_ITEMCARD以外が渡された");
		return;
	}

	if(lev < 1) lev=1;
	if(lev > 128) lev=128;

	if(r_idx)
	{
		int tmp_cnt=0;
		int bonus = randint1(20);//r_idx指定時(該当モンスターを倒したとき)は良いカードが出やすい
		lev += bonus;
		if(lev>128) lev = 128;
		//アイテムカードリストを、レアリティ0(終端ダミー)に達するまでループ
		for(i=0;support_item_list[i].rarity;i++)
		{
			//idxが一致し、生成レベル+ボーナス値が条件を満たすカードからランダムに選ばれる
			if(support_item_list[i].r_idx != r_idx) continue;
			if(support_item_list[i].min_lev > lev) continue;
			if(support_item_list[i].max_lev < lev) continue;
			tmp_cnt++;
			if(one_in_(tmp_cnt)) card_idx = i;
		}
	}

	//idxが一致するカードが設定されなかった場合、全カードからランダムに選ばれる。レアリティが高いほど出づらい。min/maxに合わないカードもたまに出る
	if(card_idx < 0)
	{
		int tester = 0;

		for(i=0;support_item_list[i].rarity;i++)
		{
			int prob = 1000 / support_item_list[i].rarity;

			if(support_item_list[i].min_lev > lev)
				prob /=  (support_item_list[i].min_lev - lev) / 5 + 2;
			if(support_item_list[i].max_lev < lev)
				prob /=  (lev - support_item_list[i].max_lev) / 5 + 2;
			tester += prob;
		}

		tester = randint1(tester);

		for(i=0;support_item_list[i].rarity;i++)
		{
			int prob = 1000 / support_item_list[i].rarity;

			if(support_item_list[i].min_lev > lev)
				prob /=  (support_item_list[i].min_lev - lev) / 5 + 2;
			if(support_item_list[i].max_lev < lev)
				prob /=  (lev - support_item_list[i].max_lev) / 5 + 2;
			tester -= prob;
			if(tester <= 0) break;
		}
		card_idx = i;
	}
	//もし何一つ対象になるカードが設定されなかった場合,idx=0になるはず
	//念のため
	if(!support_item_list[card_idx].rarity)
	{
		msg_print("ERROR:apply_magic_itemcard()のtester値計算がおかしい");
		card_idx = 0;
	}

	//pvalにカード配列添字を格納
	o_ptr->pval = card_idx;

	//枚数処理
	if(support_item_list[card_idx].prod_num <= 1)
	{
		o_ptr->number = 1;
	}
	else
	{
		//基礎生成数±30%
		int num;
		int mult = 66 + damroll(4,16);

		if(lev > support_item_list[card_idx].max_lev) mult += (lev -  support_item_list[card_idx].max_lev) * 2;
		num = support_item_list[card_idx].prod_num * mult / 100;
		if(num > 99) num = 99;
		o_ptr->number = num;

	}

}

//アイテムカードスロット数を計算する。とりあえず8にしておくが今後条件次第で16まで増やす？
//17以降はinven_special[]が別のことに使われてる
int calc_itemcard_slot_size(void)
{
	int num = 8;

	if(inventory[INVEN_RARM].name1 == ART_GRIMOIRE_OF_MARISA ||inventory[INVEN_LARM].name1 == ART_GRIMOIRE_OF_MARISA )	num += 4;

	if(num > INVEN_SPECIAL_ITEM_SLOT_END){ msg_print("ERROR:アイテムカードスロット欄数が多すぎる"); return 8;}
	return num;
}

/*:::アイテムカードの一覧表示*/
void print_itemcard_list(bool show_info)
{
	int		i;
	char	tmp_val[40];
	char	o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //アイテムカードスロット欄数 inven_special[]添字に関係
	object_type *o_ptr;
	cptr (*class_power_aux)( int, bool);

	for(i=1;i<itemslot_max+3;i++) Term_erase(17, i, 255);

	/*:::追加インベントリ表示　常にすべての欄*/
	for (i = INVEN_SPECIAL_ITEM_SLOT_START; i <= itemslot_max; i++)
	{
		o_ptr = &inven_special[i];

		//ラベル
		sprintf(tmp_val, "%c)", index_to_label(i) - INVEN_SPECIAL_ITEM_SLOT_START);
		put_str(tmp_val, i + 1, 20);

		//アイテム名+枚数
		if(!o_ptr->k_idx)
		{
			c_put_str(TERM_L_DARK, "[空きスロット]", i + 1, 22);
		}
		else
		{
			class_power_aux = support_item_list[o_ptr->pval].func_aux;
			sprintf(o_name,"『%s』(%d)",support_item_list[o_ptr->pval].name,o_ptr->number);
			c_put_str(TERM_ORANGE, o_name, i + 1, 22);

			//追加情報
			if(show_info)
			{
				use_itemcard = TRUE;
				sprintf(tmp_val, "%s",class_power_aux(support_item_list[o_ptr->pval].func_num,TRUE) );
				put_str(tmp_val, i + 1, 70);
				use_itemcard = FALSE;
			}
		}
	}

	return ;
}

//アイテムカードを選択しinven_special[1-8]に格納　キャンセルしないときTRUE
bool set_itemcard(void)
{
	int i;
	object_type forge;
	object_type *o_ptr,*q_ptr;
	int  item,amt;
	cptr q, s;
	int slot;
	char    o_name[MAX_NLEN];
	bool flag_change = FALSE;
	int		itemslot_max = calc_itemcard_slot_size(); //アイテムカードスロット欄数 inven_special[]添字に関係

	q_ptr = &forge;

	//ここのスロットはアイテムカードにしか使わないが将来プログラムミスやらかしたときのためにチェックしとく
	for(i=INVEN_SPECIAL_ITEM_SLOT_START;i<=itemslot_max;i++)
	{
		bool flag_err = FALSE;
		o_ptr = &inven_special[i];
		if(o_ptr->k_idx && o_ptr->tval != TV_ITEMCARD)
		{
			msg_format("ERROR:inven_special[%d]にアイテムカード以外が入っている",i);
			flag_err = TRUE;
		}
		if(flag_err) return FALSE;
	}

	item_tester_tval = TV_ITEMCARD;
	q = "どのカードをセットしますか? ";
	s = "アイテムカードを持っていない。";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)	o_ptr = &inventory[item];
	else o_ptr = &o_list[0 - item];

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]は小傘鍛冶用なので使わない
		print_itemcard_list(TRUE);
		prt("どのスロットにセットしますか？",0,0);

		c = inkey();
		if(c == ESCAPE) break;
		if(c >= 'a' && c <= ('a'+itemslot_max-INVEN_SPECIAL_ITEM_SLOT_START))
		{
			slot = c - 'a'+INVEN_SPECIAL_ITEM_SLOT_START;
			break;
		}
	}
	screen_load();
	if(!slot) return FALSE;

	object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

	if(!inven_special[slot].k_idx)//選択したスロットが空
	{
		object_copy(&inven_special[slot], o_ptr);
		amt = o_ptr->number;
		msg_format("%sをカードスロットにセットした。",o_name);
	}
	else if(inven_special[slot].pval != o_ptr->pval)//選択したスロットが別種の場合入れ替える
	{
		object_copy(q_ptr, &inven_special[slot]);
		object_copy(&inven_special[slot], o_ptr);
		amt = o_ptr->number;
		msg_format("%sをカードスロットに入れ替えた。",o_name);
		flag_change = TRUE;
	}
	else//選択したスロットが同種の場合足す。99枚を超えたら入れられるだけ入れる
	{
		int sum = inven_special[slot].number + o_ptr->number;
		if(inven_special[slot].number == 99)
		{
			msg_print("そのカードスロットは既に一杯だ。");
			return FALSE;
		}
		if(sum > 99)
		{
			amt = 99 - inven_special[slot].number;
			inven_special[slot].number = 99;
		}
		else
		{
			inven_special[slot].number += o_ptr->number;
			amt = o_ptr->number;
		}
		msg_format("%sをカードスロットに追加した。",o_name);
	}

	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		//inven_item_describe(item); メッセージは冗長なので出さない
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -amt);
	//	floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	//入れ替えの場合退避していた古いアイテムをザックに入れなおす
	if(flag_change)
	{
		inven_carry(q_ptr);
	}

	return TRUE;
}


//アイテムカードスロットから取り出す　キャンセルしないときTRUE
bool remove_itemcard(void)
{
	int i;
	object_type forge;
	object_type *o_ptr,*q_ptr;
	int slot;
	char    o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //アイテムカードスロット欄数 inven_special[]添字に関係

	q_ptr = &forge;

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]は小傘鍛冶用なので使わない
		print_itemcard_list(TRUE);
		prt("どのスロットからカードを取り出しますか？",0,0);

		c = inkey();
		if(c == ESCAPE) break;
		if(c >= 'a' && c <= ('a'+itemslot_max-INVEN_SPECIAL_ITEM_SLOT_START))
		{
			slot = c - 'a'+INVEN_SPECIAL_ITEM_SLOT_START;
			break;
		}
	}
	screen_load();
	if(!slot) return FALSE;

	if(!inven_special[slot].k_idx)//選択したスロットが空
	{
		msg_format("そのスロットは空だ。");
		return FALSE;
	}
	else
	{
		object_copy(q_ptr, &inven_special[slot]);
		object_desc(o_name, q_ptr, OD_OMIT_INSCRIPTION);
		object_wipe(&inven_special[slot]);
		msg_format("%sをカードスロットから取り出した。",o_name);
		inven_carry(q_ptr);
	}

	return TRUE;
}


//アイテムカードを使用する　キャンセルしないときTRUE
bool activate_itemcard(void)
{
	int i;
	object_type *o_ptr;
	int slot;
	char    o_name[MAX_NLEN];
	class_power_type *spell;
	cptr (*class_power_aux)( int, bool);
	int power_idx;
	int		itemslot_max = calc_itemcard_slot_size(); //アイテムカードスロット欄数 inven_special[]添字に関係

	if (!repeat_pull(&slot))
	{
		screen_save();
		while(1)
		{
			char c;
			slot = 0; //inven_special[0]は小傘鍛冶用なので使わない
			print_itemcard_list(TRUE);
			prt("どのアイテムカードを使用しますか？",0,0);

			c = inkey();
			if(c == ESCAPE) break;
			if(c >= 'a' && c <= ('a'+itemslot_max-INVEN_SPECIAL_ITEM_SLOT_START))
			{
				slot = c - 'a'+INVEN_SPECIAL_ITEM_SLOT_START;
				break;
			}
		}
		repeat_push(slot);
		screen_load();
	}
	if(!slot) return FALSE;

	if(!inven_special[slot].k_idx)//選択したスロットが空
	{
		msg_format("そのスロットは空だ。");
		return FALSE;
	}
	else
	{
		bool flag_cancel = FALSE;
		o_ptr = &inven_special[slot];
		power_idx = o_ptr->pval;

		class_power_aux = support_item_list[power_idx].func_aux;
		spell = &(support_item_list[power_idx].func_table[support_item_list[power_idx].func_num]);
		msg_format("「%s」を発動した！",spell->name);
		use_itemcard = TRUE;
		if(!class_power_aux(support_item_list[power_idx].func_num,FALSE)) flag_cancel = TRUE;
		use_itemcard = FALSE;

		if(flag_cancel) return FALSE;

		o_ptr->number -= 1;
		if(!o_ptr->number) object_wipe(&inven_special[slot]);
	}

	//行動力変化処理変数をリセット
	new_class_power_change_energy_need = 0;
	return TRUE;
}



//セットしたアイテムカードを読む。行動力を消費しない。
void read_itemcard(void)
{
	int i;
	int slot;
	char    o_name[MAX_NLEN];
	int		itemslot_max = calc_itemcard_slot_size(); //アイテムカードスロット欄数 inven_special[]添字に関係

	screen_save();
	while(1)
	{
		char c;
		slot = 0; //inven_special[0]は小傘鍛冶用なので使わない
		print_itemcard_list(TRUE);
		prt("どのスロットのカードを読みますか？",0,0);

		c = inkey();
		if(c == ESCAPE) break;
		if(c >= 'a' && c <= ('a'+itemslot_max-INVEN_SPECIAL_ITEM_SLOT_START))
		{
			slot = c - 'a'+INVEN_SPECIAL_ITEM_SLOT_START;
			break;
		}
	}
	screen_load();
	if(!slot) return;

	if(!inven_special[slot].k_idx)//選択したスロットが空
	{
		msg_format("そのスロットは空だ。");
		return;
	}
	else
	{
		screen_object(&inven_special[slot],SCROBJ_FORCE_DETAIL);
	}

	return;
}










