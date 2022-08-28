/* File: mutation.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mutation effects (and racial powers) */

#include "angband.h"




/*:::突然変異が不自然な重複をしないようにするための配列。配列の添え字がMUT????の「順番」に対応し配列の値はグループ番号(0はグループ無し)、lose_mutation用引数*/
/*:::何か変異を得るときこの配列が参照され、十の桁まで同じで一の桁が違うグループ番号の変異が他にあるとlose_mutation()で消される。*/
/*:::グループ内に永続変異があると排他変異は得られない。*/

int exclusive_muta_lis[128][2]=
{
	{0,1},//MUT1_BR_ACID
	{0,5},//MUT1_BR_FIRE
	{1,8},//MUT1_HYPN_GAZE
	{0,10},//MUT1_TELEKINES;
	{0,12},//MUT1_VTELEPORT;
	{0,15},//MUT1_MIND_BLST;
	{0,17},//MUT1_BR_ELEC;
	{141,19},//MUT1_VAMPIRISM;
	{143,21},//MUT1_ACCELERATION;
	{0,24},//MUT1_BEAST_EAR;
	{0,28},//MUT1_BLINK;
	{142,31},//MUT1_EAT_ROCK;
	{0,33},//MUT1_SWAP_POS;
	{52,35},//MUT1_BR_SOUND;
	{0,38},//MUT1_BR_COLD;
	{0,41},//MUT1_DET_ITEM;
	{0,43},//MUT1_BERSERK;
	{0,46},//MUT1_BR_POIS;
	{0,47},//MUT1_MIDAS_TCH;
	{151,49},//MUT1_GROW_MOLD;
	{0,50},//MUT1_RESIST;
	{0,53},//MUT1_EARTHQUAKE;
	{0,56},//MUT1_EAT_MAGIC;
	{152,57},//MUT1_RAISE_WOOD;
	{0,59},//MUT1_STERILITY;
	{0,60},//MUT1_PANIC_HIT;
	{1,62},//MUT1_DAZZLE;
	{1,65},//MUT1_LASER_EYE;
	{0,68},//MUT1_RECALL;
	{0,70},//MUT1_ID_MUSHROOM;
	{0,71},//MUT1_DANMAKU;
	{0,0}, //dummy

	{0,75},//MUT2_BERS_RAGE;
	{102,76},//MUT2_COWARDICE;
	{0,77},//MUT2_RTELEPORT;
	{0,78},//MUT2_ALCOHOL;
	{0,79},//MUT2_HALLU;
	{0,80},//MUT2_YAKU;
	{21,81},//MUT2_HARDHEAD;
	{22,83},//MUT2_HORNS;
	{23,85},//MUT2_BIGHORN;
	{0,87},//MUT2_ATT_DEMON;
	{0,89},//MUT2_PROD_MANA;
	{0,90},//MUT2_SPEED_FLUX;
	{0,92},//MUT2_BANISH_ALL;
	{0,94},//MUT2_EAT_LIGHT;
	{31,95},//MUT2_TAIL;
	{0,97},//MUT2_ATT_ANIMAL;
	{32,98},//MUT2_BIGTAIL;
	{0,99},//MUT2_RAW_CHAOS;
	{0,100},//MUT2_NORMALITY;
	{0,103},//MUT2_WRAITH;
	{0,104},//MUT2_ATT_GHOST;
	{0,105},//MUT2_ATT_KWAI;
	{0,106},//MUT2_ATT_DRAGON;
	{41,107},//MUT2_XOM;
	{11,109},//MUT2_BIRDBRAIN;
	{42,110},//MUT2_CHAOS_GIFT;
	{0,112},//MUT2_WALK_SHAD;
	{0,113},//MUT2_WARNING;
	{0,115},//MUT2_INVULN;
	{51,116},//MUT2_ASTHMA;
	{0,118},//MUT2_THE_WORLD;
	{132,119},//MUT2_DISARM;

	{61,120},//MUT3_HYPER_STR;
	{62,123},//MUT3_PUNY;
	{12,126},//MUT3_HYPER_INT;
	{11,129},//MUT3_MORONIC;
	{71,132},//MUT3_RESILIENT;
	{0,134},//MUT3_WARNING;
	{72,136},//MUT3_ALBINO;
	{61,138},//MUT3_SPEEDSTER;
	{0,141},//MUT3_RES_WATER;
	{2,143},//MUT3_BLANK_FAC;
	{0,145},//MUT3_HURT_LITE;
	{1,146},//MUT3_XTRA_EYES;
	{0,149},//MUT3_MAGIC_RES;
	{0,151},//MUT3_XTRA_NOIS;
	{0,154},//MUT3_BYDO;
	{33,157},//MUT3_FISH_TAIL;
	{23,159},//MUT3_PEGASUS;
	{91,161},//MUT3_ACID_SCALES;
	{92,163},//MUT3_ELEC_SCALES;
	{93,165},//MUT3_FIRE_SCALES;
	{94,168},//MUT3_COLD_SCALES;
	{95,171},//MUT3_POIS_SCALES;
	{0,173},//MUT3_WINGS;
	{101,175},//MUT3_FEARLESS;
	{0,178},//MUT3_FEVER;
	{0,180},//MUT3_ESP;
	{111,182},//MUT3_LIMBER;
	{112,185},//MUT3_ARTHRITIS;
	{121,188},//MUT3_BAD_LUCK;
	{0,189},//MUT3_VULN_ELEM;
	{131,190},//MUT3_MOTION;
	{122,193},//MUT3_GOOD_LUCK;

	{ 161,194}, //動物霊憑依(オオカミ)
	{ 162,200 }, //カワウソ
	{ 163,206 }, //オオワシ
	{ 164,212 }, //クラゲ
	{ 165,213 }, //ウシ
	{ 166,214 }, //ヒヨコ
	{ 167,215 }, //カメ
	{ 168,216 }, //ハニワ
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装
	{ 0,0 }, //未実装

};


/*:::ランダムな突然変異を得る。狂戦士はウィザードモードでの指定を除いて発動型変異を得られない*/
///sys race mutation class 突然変異処理
bool gain_random_mutation(int choose_mut)
{
	int     attempts_left = 20;
	cptr    muta_desc = "";
	bool    muta_chosen = FALSE;
	u32b    muta_which = 0;
	u32b    *muta_class = NULL;

	bool	no_wield_change = FALSE;

	//三月精と変身中は装備品に影響が出る変異を受けない
	if(p_ptr->mimic_form || p_ptr->pclass == CLASS_3_FAIRIES) no_wield_change = TRUE;

	//v1.1.32 建物内では装備品に影響の出る変異を受けないようにしておく
	if (character_icky) no_wield_change = TRUE;

	//蓬莱人は変異無効 箱によるオーラ除く
	//v1.1.98 瑞霊の憑依も受ける
	if(p_ptr->prace == RACE_HOURAI && choose_mut != 193 && choose_mut != 217)
	{
		if(one_in_(3))msg_print(_("一瞬髪が逆立った気がしたが何も起こらなかった。",
                                    "You feel your hair stand up for a moment, but nothing happens."));
		else if(one_in_(2))msg_print(_("一瞬首筋がムズムズした気がしたが何も起こらなかった。",
                                        "The back of your neck feels stuffy for a moment, but nothing happens."));
		else msg_print(_("一瞬頭がぼうっとしたが何も起こらなかった。",
                            "Your head feels dull for a moment, but nothing happens."));
		return TRUE;
	}

	if (choose_mut) attempts_left = 1;

	set_deity_bias(DBIAS_COSMOS, -1);

	while (attempts_left--)
	{
		//性格狂気も発動型変異を得られるようにする。まあいいか
		//switch (choose_mut ? choose_mut : (p_ptr->pseikaku == SEIKAKU_BERSERK ? 74+randint1(119) : randint1(193)))
		switch (choose_mut ? choose_mut : randint1(216))
		{
		case 1: case 2: case 3:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ACID;
#ifdef JP
muta_desc = "酸を吐く能力を得た。";
#else
			muta_desc = "You gain the ability to breathe acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "火を吐く能力を得た。";
#else
			muta_desc = "You gain the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "催眠眼の能力を得た。";
#else
			muta_desc = "Your eyes look mesmerizing...";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "物体を念動力で動かす能力を得た。";
#else
			muta_desc = "You gain the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "自分の意思でテレポートする能力を得た。";
#else
			muta_desc = "You gain the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "精神攻撃の能力を得た。";
#else
			muta_desc = "You gain the power of Mind Blast.";
#endif

			break;
		case 17: case 18:case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ELEC;
#ifdef JP
muta_desc = "雷を吐く能力を得た。";
#else
			muta_desc = "You gain the ability to breathe lightning.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "人間の血を啜りたくなった…";
#else
			muta_desc = "You develop a taste for human blood...";
#endif

			break;
		case 21: case 22:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ACCELERATION;
#ifdef JP
muta_desc = "奥歯に加速装置が搭載された！";
#else
			muta_desc = "You gain the ability to accelerat!";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BEAST_EAR;
#ifdef JP
muta_desc = "獣耳が生えてきた。";
#else
			muta_desc = "You grow beast ears.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "近距離テレポートの能力を得た。";
#else
			muta_desc = "You gain the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "壁が美味しそうに見える。";
#else
			muta_desc = "The walls look delicious.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "他人の靴で一マイル歩くような気分がする。";
#else
			muta_desc = "You feel like walking a mile in someone else's shoes.";
#endif

			break;
		case 35: case 36:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_SOUND;
#ifdef JP
muta_desc = "岩をも砕く大声を出せるようになった！";
#else
			muta_desc = "You become able to shout loud enough to shatter stone!";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_COLD;
#ifdef JP
muta_desc = "吹雪を吐く能力を得た。";
#else
			muta_desc = "You gan the ability to breathe frost.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_ITEM;
#ifdef JP
muta_desc = "突然ダウジングのコツを掴んだ気がする。";
#else
			muta_desc = "You suddenly get the hang of dowsing.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "制御可能な激情を感じる。";
#else
			muta_desc = "You feel a controlled rage.";
#endif

			break;
		case 46:case 37:case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_POIS;
#ifdef JP
muta_desc = "毒ガスを吐く能力を得た。";
#else
			muta_desc = "You gain the ability to breathe poison gas.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "「ミダス王の手」の能力を得た。";/*トゥームレイダースにありましたね。 */
#else
			muta_desc = "You gain the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "突然カビに親しみを覚えた。";
#else
			muta_desc = "You feel a sudden affinity for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "あなたは元素の攻撃から自分自身を守れる気がする。";
#else
			muta_desc = "You feel like you can protect yourself.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "ダンジョンで地震を起こす能力を得た。";
#else
			muta_desc = "You gain the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "魔法のアイテムが美味そうに見える。";
#else
			muta_desc = "Your magic items look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RAISE_WOOD;
#ifdef JP
muta_desc = "あなたは植物と友達になった気がする。";
#else
			muta_desc = "You feel like a friend to plants.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "嫉妬深くなった気がする。";
#else
			muta_desc = "You feel very jealous.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "突然一撃離脱のコツを掴んだ気がする。";
#else
			muta_desc = "You suddenly get the hang of 'Hit and Away' technique.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "狂気をもたらす魔眼を得た。";
#else
			muta_desc = "You gain the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "眼からビームを撃てるようになった！";
#else
			muta_desc = "You can shoot beams from your eyes!";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "ダンジョンと地上の間をテレポートできるようになった。";
#else
			muta_desc = "You gain the ability to teleport between dungeon and surface.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ID_MUSHROOM;
#ifdef JP
muta_desc = "キノコの種類を嗅ぎ分けることができるようになった。";
#else
			muta_desc = "You gain the ability to tell mushroom apart by smell.";
#endif

			break;
		case 71: case 72:
		case 73: case 74:

			if (is_special_seikaku(SEIKAKU_SPECIAL_CIRNO) || is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			{
				;//チルノと成美の特殊性格はすでに背中に扉が開いてる設定なので何もしない
			}
			else
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_DANMAKU;
#ifdef JP
				//v1.1.39 変更
				muta_desc = "背中の辺りから力があふれてくる気がする！";
#else
				muta_desc = "You feel power pouring out of your back!";
#endif
			}
			break;
			/*:::アイテム投げ変異を削除。種族レイシャル個数制限を取り払ったのでもう使えないはず*/
/*
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "あなたの物を投げる手はかなり強くなった気がする。";
#else
			muta_desc = "Your throwing arm feels much stronger.";
#endif

			break;
*/
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "制御不能な激情を感じる！";
#else
			muta_desc = "You become subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "あなたは閉所恐怖症になった！";
#else
			muta_desc = "You become claustrophobic!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "少し意識が曖昧になった。";
#else
			muta_desc = "Your consciousness feels a little vague.";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "あなたはアルコール中毒になった。";
#else
			muta_desc = "You became addicted to alcohol.";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "あなたは幻覚を引き起こす精神錯乱に冒された。";
#else
			muta_desc = "You are afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_YAKU;
#ifdef JP
muta_desc = "あなたの周りに厄が集まるようになった…";
#else
			muta_desc = "Misfortune starts to gather around you...";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HARDHEAD;
#ifdef JP
muta_desc = "大変な石頭になった！";
#else
			muta_desc = "You became very hardheaded!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "額に角が生えた！";
#else
			muta_desc = "Horns pop forth into your forehead!";
#endif

			break;
		case 85: case 86:
			/*::: -Hack- 変身時は装備が吹き飛ぶような変異を受けないようにしておく*/
			if(!no_wield_change)
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIGHORN;
				muta_desc = _("鬼のような立派な角が生えてきた！",
                                "You grow splendid, oni-like horns!");
			}
			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "悪魔を引き付けるようになった。";
#else
			muta_desc = "You start attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "あなたは制御不能な魔法のエネルギーを発生するようになった。";
#else
			muta_desc = "You start producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "あなたは躁鬱質になった。";
#else
			muta_desc = "You become manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "恐ろしい力があなたの背後に潜んでいる気がする。";
#else
			muta_desc = "You feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "あなたはウンゴリアントに奇妙な親しみを覚えるようになった。";
#else
			muta_desc = "You feel a strange kinship with Ungoliant.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TAIL;
#ifdef JP
muta_desc = "しなやかな尻尾が生えてきた。";
#else
			muta_desc = "You grow a flexible tail.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "動物を引き付けるようになった。";
#else
			muta_desc = "You start attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "竜のような大きな尻尾が生えてきた。";
#else
			muta_desc = "You grow a massive dragon-like tail.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "周囲の空間が不安定になった気がする。";
#else
			muta_desc = "You feel the universe is less stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "あなたは通常の体に近づいていく気がする。";
#else
			muta_desc = "You feel strangely normal.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "あなたは幽体化したり実体化したりするようになった。";
#else
			muta_desc = "You start to fade in and out of the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_GHOST;
#ifdef JP
muta_desc = "幽霊たちがあなたの周りで囁きあうのを感じた。";
#else
			muta_desc = "You feel ghosts whispering around you.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_KWAI;
#ifdef JP
muta_desc = "どこからともなく妖怪達の視線を感じた。";
#else
			muta_desc = "You feel youkai observing you from somewhere.";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "あなたはドラゴンを引きつけるようになった。";
#else
			muta_desc = "You start attracting dragons.";
#endif

			break;
		case 107: case 108:
			if (p_ptr->pclass == CLASS_MAID)
				msg_format(_("ゾムがあなたに興味をもったが主が追い払った。",
                            "Xom gets interested in you, but gets driven away by your master."));
			else if (p_ptr->prace == RACE_STRAYGOD || p_ptr->prace == RACE_DEITY)
				msg_format(_("ゾムはあなたを見たがすぐ興味を失ったようだ。",
                            "Xom quickly loses interest in you."));
			else
			{


			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_XOM;
#ifdef JP
muta_desc = "ゾムは告げた。「お前が新しいオモチャだ！」";
#else
			muta_desc = "Xom speaks. 'A new plaything!'";
#endif
			}
			break;
		case 109:
			///mod150121 慧音は鳥頭にならない
			if(p_ptr->pclass != CLASS_KEINE)
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIRDBRAIN;
				muta_desc = _("物忘れがひどくなった気がする。", "You feel very forgetful.");
			}
			break;
		case 110: case 111:
			/* Chaos warriors already have a chaos deity */
			//if (p_ptr->pclass != CLASS_CHAOS_WARRIOR)
			if (p_ptr->pclass == CLASS_MAID)
				msg_format(_("カオスの守護悪魔があなたに興味をもったが主が追い払った。",
                            "A chaos patron becomes interested in you, but gets driven away by your master."));
			else if (p_ptr->prace == RACE_STRAYGOD || p_ptr->prace == RACE_DEITY)
				msg_format(_("カオスの守護悪魔はあなたに興味を持たなかった。",
                            "You are of no interest to chaos patrons."));
			else
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "あなたはカオスの守護悪魔の注意を惹くようになった。";
#else
				muta_desc = "You attract the notice of a chaos patron!";
#endif

			}
			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "あなたは現実が紙のように薄いと感じるようになった。";
#else
			muta_desc = "You feel like reality is as thin as paper.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "あなたは突然パラノイアになった気がする。";
#else
			muta_desc = "You suddenly feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "あなたは祝福され、無敵状態になる発作を起こすようになった。";
#else
			muta_desc = "You are blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			//クタカは喘息にならないことにする。厳密には喘息は喉の病気ではないけどニワタリ神の御利益は咳止めだしたぶんギリギリ権能の範囲内だろう
			if (p_ptr->pclass == CLASS_KUTAKA) break;

			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ASTHMA;
#ifdef JP
muta_desc = "喘息持ちになった。";
#else
			muta_desc = "You became asthmatic.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_THE_WORLD;
#ifdef JP
muta_desc = "止まった時の世界に迷い込むようになった。";
#else
			muta_desc = "You occasionally get drawn into a world where time stand still.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "あなたはなぜかよく転ぶようになった。";
#else
			muta_desc = "You become prone to tripping and falling.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "超人的に強くなった！";
#else
			muta_desc = "You turn into a superhuman!";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "筋肉が弱ってしまった...";
#else
			muta_desc = "Your muscles wither away...";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "あなたの脳は生体コンピュータに進化した！";
#else
			muta_desc = "Your brain evolves into a living computer!";
#endif

			break;
		case 129: case 130: case 131:


			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "脳みそがちょっと変色した！";
#else
			muta_desc = "Your brain withers away...";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "並外れてタフになった。";
#else
			muta_desc = "You become extraordinarily resilient.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WARNING;
#ifdef JP
muta_desc = "凄まじく勘が鋭くなった！";
#else
			muta_desc = "Your intuition gets extremely sharp!";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "虚弱体質になった気がする...";
#else
			muta_desc = "You feel very frail...";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SPEEDSTER;
#ifdef JP
muta_desc = "あなたの脚力は並外れて強くなった！";
#else
			muta_desc = "Your legs grow extraordinarily strong!";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RES_WATER;
#ifdef JP
muta_desc = "丸一日でも息を止めていられる気がする！";
#else
			muta_desc = "You feel like you can hold your breath for a whole day!";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "のっぺらぼうになった！";
#else
			muta_desc = "Your face becomes completely featureless!";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HURT_LITE;
#ifdef JP
muta_desc = "明るい光が苦手になった気がする…";
#else
			muta_desc = "You become vulnerable to bright light.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				muta_desc = "額に第四の目が開眼した！";
			else
				muta_desc = "額に第三の目が開眼した！";
#else
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				muta_desc = "A fourth eye opens on your forehead!";
			else
				muta_desc = "A third eye opens on your forehead!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "魔法への耐性がついた。";
#else
			muta_desc = "You become resistant to magic.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "あなたは奇妙な音を立て始めた！";
#else
			muta_desc = "You start making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BYDO;
#ifdef JP
muta_desc = "バイド細胞に浸食された！";
#else
			muta_desc = "You are infected by Bydo cells!";
#endif
			//v1.1.58
			flag_update_floor_music = TRUE;
			break;
		case 157: case 158:
			/*::: -Hack- 変身時は装備が吹き飛ぶような変異を受けないようにしておく*/
			if(!no_wield_change)
			{
				muta_class = &(p_ptr->muta3);
				muta_which = MUT3_FISH_TAIL;
				muta_desc = _("下半身が人魚の尻尾になった！", "Your lower body turns into a merfolk tail!");
			}
			break;
		case 159: case 160:
			/*::: -Hack- 変身時は装備が吹き飛ぶような変異を受けないようにしておく*/
			if(!no_wield_change)
			{

				muta_class = &(p_ptr->muta3);
				muta_which = MUT3_PEGASUS;
				muta_desc = _("髪型が昇天ペガサスMIX盛りになった！",
                            "Your now have the Ascension Pegasus MIX Prime hairstyle!");
			}
			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ACID_SCALES;
#ifdef JP
muta_desc = "肌が黒い鱗に覆われた！";
#else
			muta_desc = "Black scales cover your skin!";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_SCALES;
#ifdef JP
muta_desc = "肌が青い鱗に覆われた！";
#else
			muta_desc = "Blue scales cover your skin!";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_SCALES;
#ifdef JP
muta_desc = "肌が赤い鱗に覆われた！";
#else
			muta_desc = "Red scales cover your skin!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_COLD_SCALES;
#ifdef JP
muta_desc = "肌が白い鱗に覆われた！";
#else
			muta_desc = "White scales cover your skin!";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_POIS_SCALES;
#ifdef JP
muta_desc = "肌が緑の鱗に覆われた！";
#else
			muta_desc = "Green scales cover your skin!";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "背中に羽が生えた。";
#else
			muta_desc = "You grow a pair of wings.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "完全に怖れ知らずになった。";
#else
			muta_desc = "You become completely fearless.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEVER;
#ifdef JP
muta_desc = "原因不明の熱病に冒された。";
#else
			muta_desc = "You start suffering from an unexplained fever.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "テレパシーの能力を得た！";
#else
			muta_desc = "You develop a telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "筋肉がしなやかになった。";
#else
			muta_desc = "Your muscles become limber.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "関節が突然痛み出した。";
#else
			muta_desc = "Your joints suddenly hurt.";
#endif

			break;
		case 188:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "悪意に満ちた黒いオーラがあなたをとりまいた...";
#else
			muta_desc = "There is a malignant black aura surrounding you...";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "妙に無防備になった気がする。";
#else
			muta_desc = "You feel strangely exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "体の動作がより正確になった。";
#else
			muta_desc = "You move with new assurance.";
#endif

			break;
		case 193:
			///箱開封からもここに来るので変更時注意
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "慈悲深い白いオーラがあなたをとりまいた...";
#else
			muta_desc = "There is a benevolent white aura surrounding you...";
#endif

			break;

		case 194:	case 195:	case 196:
		case 197:	case 198:	case 199:
			//種族神格と種族幽霊と職業プリズムリバーは憑依されないことにする。
			//設定的な不自然さを避けるためもあるが、プリズムリバーとヘカーティアの突然変異保持処理追加が面倒なのが一番の理由。
			//今後muta4を追加したときどうしよう。
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))
			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_WOLF;
			muta_desc = _("オオカミの動物霊に憑依された！", "You get possessed by a wolf animal ghost!");
			break;

		case 200:	case 201:	case 202:
		case 203:	case 204:	case 205:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_OTTER;
			muta_desc = _("カワウソの動物霊に憑依された！", "You get possessed by an otter animal ghost!");
			break;

		case 206:	case 207:	case 208:
		case 209:	case 210:	case 211:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_EAGLE;
			muta_desc = _("オオワシの動物霊に憑依された！", "You get possessed by an eagle animal ghost!");
			break;

		case 212:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_JELLYFISH;
			muta_desc = _("クラゲの動物霊に憑依された！", "You get possessed by a jellyfish animal spirit!");
			break;

		case 213:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_COW;
			muta_desc = _("ウシの動物霊に憑依された！", "You get possessed by a cow animal spirit!");
			break;

		case 214:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHICKEN;
			muta_desc = _("ヒヨコの動物霊に憑依された！", "You get possessed by a chicken animal spirit!");
			break;

		case 215:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_TORTOISE;
			muta_desc = _("カメの動物霊に憑依された！", "You get possessed by a tortoise animal spirit!");
			break;

		case 216:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print(_("動物霊が寄ってきたがあなたを見て逃げていった。",
                            "An animal ghost approached and ran away upon seeing you."));
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANIWA;
			muta_desc = _("ハニワの……動物？……霊？に憑依された！",
                            "You get possessed by a haniwa...... animal?...... spirit?!");
			break;

		case 217://v1.1.84 反獄王に取り憑かれる
			 //この変異は変異指定でこの関数に来たときしか得られない
			if (!choose_mut)
			{
				break;
			}
			//ヘカとプリズムリバーは弾く。mut4を扱えないため
			if (p_ptr->pclass == CLASS_HECATIA)
			{
				msg_print(_("強力な怨霊が寄ってきたがあなたを見て逃げていった。",
                            "A powerful vengeful spirit approached and ran away upon seeing you."));
				break;
			}
			if(CLASS_IS_PRISM_SISTERS)
			{
				msg_print(_("強力な怨霊が寄ってきたが姉妹たちが頑張って追い払った。",
                            "A powerful vengeful spirit approached, but your sisters drove it away."));
				break;
			}
			//動物霊が永久変異として憑いてる場合は取り憑かれない(今の所LUNATICで稀に起こるだけ)
			if (p_ptr->muta4_perma & (MUT4_GHOST_WOLF | MUT4_GHOST_OTTER | MUT4_GHOST_EAGLE | MUT4_GHOST_JELLYFISH | MUT4_GHOST_COW | MUT4_GHOST_CHICKEN | MUT4_GHOST_TORTOISE | MUT4_GHOST_HANIWA))
			{
				break;
			}
			//動物霊が通常変異として憑いてる場合は消える
			p_ptr->muta4 &= ~(MUT4_GHOST_WOLF | MUT4_GHOST_OTTER | MUT4_GHOST_EAGLE | MUT4_GHOST_JELLYFISH | MUT4_GHOST_COW | MUT4_GHOST_CHICKEN | MUT4_GHOST_TORTOISE | MUT4_GHOST_HANIWA);


			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANGOKUOH;
			muta_desc = _("あなたは大怨霊に体を乗っ取られてしまった！",
                            "Your body has been taken over by a great vengeful spirit!");
			break;




		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (!(*muta_class & muta_which))
			{
				muta_chosen = TRUE;
			}
		}

		///mod140319 相反する変異の重なりの抑止、永続変異の保護などのルーチンを追加
		if(muta_chosen)
		{
			int mut_num,i;
			/*:::変異候補を0～127の値に変換*/
			if(muta_class == &p_ptr->muta1)      mut_num = 0;
			else if(muta_class == &p_ptr->muta2) mut_num = 32;
			else if(muta_class == &p_ptr->muta3) mut_num = 64;
			else								 mut_num = 96;

			for(i=0;i<32;i++) if((muta_which>>i) & 1L) mut_num += i;

			/*:::変異候補に何か排他的な他の変異があるとき*/
			if(exclusive_muta_lis[mut_num][0] != 0)
			{
				for(i=0;i<128;i++)
				{
					/*:::ダミー行は飛ばす*/
					if(exclusive_muta_lis[i][1] == 0)continue;
					if(exclusive_muta_lis[i][0] == 0)continue;


					/*:::変異候補に、グループidの10の桁以上が一致し1の桁が一致しない番号がある場合排他処理*/
					if((exclusive_muta_lis[i][0]/10) == (exclusive_muta_lis[mut_num][0]/10) && exclusive_muta_lis[i][0] != exclusive_muta_lis[mut_num][0])
					{
						/*:::排他処理の相手方が永続変異の場合今の変異候補をリセット*/
						if((i < 32        && p_ptr->muta1_perma >> (i%32) &1L)
						 ||(i>=32 && i<64 && p_ptr->muta2_perma >> (i%32) &1L)
						 ||(i>=64 && i<96 && p_ptr->muta3_perma >> (i % 32) & 1L)
							||(i>=96         && p_ptr->muta4_perma >> (i%32) &1L))
						{
							if(cheat_xtra) msg_format(_("TESTMSG:変異%dが排他規則により取得不可",
                                                        "TESTMSG: Exclusive rule prevents acquiring mutation %d"),mut_num);
							muta_chosen = FALSE;
							break;
						}
					}
				}

				/*:::永続変異などがないと確認したら排他相手変異の削除*/
				if(muta_chosen) for(i=0;i<128;i++)
				{
					/*:::ダミー行は飛ばす*/
					if(exclusive_muta_lis[i][1] == 0)continue;
					if(exclusive_muta_lis[i][0] == 0)continue;
					/*:::変異候補に、グループidの10の桁以上が一致し1の桁が一致しない番号がある場合排他処理*/
					if((exclusive_muta_lis[i][0]/10) == (exclusive_muta_lis[mut_num][0]/10) && exclusive_muta_lis[i][0] != exclusive_muta_lis[mut_num][0])
					{
						/*:::排他処理の相手方の変異を消す*/
						if((i < 32        && p_ptr->muta1 >> (i%32) &1L)
						||(i>=32 && i<64 && p_ptr->muta2 >> (i%32) &1L)
						||(i >= 64 && i<96 && p_ptr->muta3 >> (i % 32) & 1L)
							||(i>=96         && p_ptr->muta4 >> (i%32) &1L))
						{
							lose_mutation(exclusive_muta_lis[i][1]);
						}
					}
				}
			}
		}



		if (muta_chosen == TRUE) break;

		//muta_chosenがFALSEのときはwhileの再抽選ループに戻る
	}

	if (!muta_chosen)
	{
#ifdef JP
//msg_print("普通になった気がする。");
if(one_in_(4))msg_print("一瞬頭がぼうっとしたが何も起こらなかった。");
else if(one_in_(3))msg_print("一瞬背中がムズムズしたが何も起こらなかった。");
else if(one_in_(2))msg_print("一瞬体中が痒くなったが何も起こらなかった。");
else msg_print("一瞬髪が逆立ったが何も起こらなかった。");
#else
		if(one_in_(4))msg_print("Your head feels dull for a moment, but nothing happens.");
        else if(one_in_(3))msg_print("The back of your neck feels stuffy for a moment, but nothing happens.");
        else if(one_in_(2))msg_print("You suddenly feel very itchy for a moment, but nothing happens.");
        else msg_print("You feel your hair stand up for a moment, but nothing happens.");
#endif

		return FALSE;
	}
	else
	{
		//chg_virtue(V_CHANCE, 1);

		/*
		  some races are apt to gain specified mutations
		  This should be allowed only if "choose_mut" is 0.
							--- henkma
		*/
			///pend 特定種族で特定の突然変異を得やすい処理　一時的に無効化
		if(!choose_mut)
		{
/*
			if (p_ptr->prace == RACE_VAMPIRE &&
			  !(p_ptr->muta1 & MUT1_HYPN_GAZE) &&
			   (randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "眼が幻惑的になった...";
#else
				muta_desc = "Your eyes look mesmerizing...";
#endif

			}
*/
/*
			else if (p_ptr->prace == RACE_IMP &&
				 !(p_ptr->muta2 & MUT2_HORNS) &&
				 (randint1(10) < 7))
			  {
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "角が額から生えてきた！";
#else
				muta_desc = "Horns pop forth into your forehead!";
#endif

			}
*/
/*
			else if (p_ptr->prace == RACE_MIND_FLAYER &&
				!(p_ptr->muta2 & MUT2_BIGTAIL) &&
				(randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "大きな尻尾が生えてきた。";
#else
				muta_desc = "Evil-looking tentacles sprout from your mouth.";
#endif

			}
*/

		}

#ifdef JP
msg_print("突然変異した！");
#else
		msg_print("You mutate!");
#endif

		msg_print(muta_desc);
		*muta_class |= muta_which;

		///mod141124 難易度LUNATICのとき変異が永続化（新生などで治る)
		if(difficulty == DIFFICULTY_LUNATIC  && one_in_(13) )
		{
			if (muta_class == &(p_ptr->muta3)) p_ptr->muta3_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta2)) p_ptr->muta2_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta1)) p_ptr->muta1_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta4)) p_ptr->muta4_perma |= muta_which;
			else msg_print(_("ERROR:LUNATICの永続変異処理がおかしい",
                            "ERROR: Weird permanent mutation logic for LUNATIC"));

		}


		///mod140319 特定の変異を得たとき装備が外れる。落とすのはコード面倒だしちょっと酷いことになるかもしれないのでやめておく。ザックが一杯ならどうせ落ちるが。
		if (muta_class == &(p_ptr->muta3) && muta_which == MUT3_FISH_TAIL)
		{
			if(inventory[INVEN_FEET].k_idx)
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_FEET], 0);

				msg_format(_("%sが脱げてしまった！", "%s was taken off!"),obj_name);
				inven_takeoff(INVEN_FEET,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}
		if (muta_class == &(p_ptr->muta3) && muta_which == MUT3_PEGASUS)
		{
			if(inventory[INVEN_HEAD].k_idx)
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format(_("%sが脱げてしまった！", "%s was taken off!"),obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}
		if (muta_class == &(p_ptr->muta2) && muta_which == MUT2_BIGHORN)
		{
			if(inventory[INVEN_HEAD].k_idx &&
			(	inventory[INVEN_HEAD].sval == SV_HEAD_METALCAP
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STARHELM
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STEELHELM
			||  inventory[INVEN_HEAD].sval == SV_HEAD_DRAGONHELM  ))
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format(_("%sが脱げてしまった！", "%s was taken off!"),obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}


		/*:::相反する変異を一度に持たなくする処理　別処理で実装したので無効化*/
#if 0

		if (muta_class == &(p_ptr->muta3))
		{
			if (muta_which == MUT3_PUNY)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR)
				{
#ifdef JP
msg_print("あなたはもう超人的に強くはない！");
#else
					msg_print("You no longer feel super-strong!");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_STR);
				}
			}
			else if (muta_which == MUT3_HYPER_STR)
			{
				if (p_ptr->muta3 & MUT3_PUNY)
				{
#ifdef JP
msg_print("あなたはもう虚弱ではない！");
#else
					msg_print("You no longer feel puny!");
#endif

					p_ptr->muta3 &= ~(MUT3_PUNY);
				}
			}
			else if (muta_which == MUT3_MORONIC)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT)
				{
#ifdef JP
msg_print("あなたの脳はもう生体コンピュータではない。");
#else
					msg_print("Your brain is no longer a living computer.");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_INT);
				}
			}
			else if (muta_which == MUT3_HYPER_INT)
			{
				if (p_ptr->muta3 & MUT3_MORONIC)
				{
#ifdef JP
msg_print("あなたはもう精神薄弱ではない。");
#else
					msg_print("You are no longer moronic.");
#endif

					p_ptr->muta3 &= ~(MUT3_MORONIC);
				}
			}
			else if (muta_which == MUT3_POIS_SCALES)
			{
				if (p_ptr->muta3 & MUT3_COLD_SCALES)
				{
#ifdef JP
msg_print("鱗がなくなった。");
#else
					msg_print("You lose your scales.");
#endif

					p_ptr->muta3 &= ~(MUT3_COLD_SCALES);
				}
				if (p_ptr->muta3 & MUT3_SPEEDSTER)
				{
#ifdef JP
msg_print("脚力が並に戻った。");
#else
					msg_print("Your flesh rots no longer.");
#endif

					p_ptr->muta3 &= ~(MUT3_SPEEDSTER);
				}
				if (p_ptr->muta3 & MUT3_FIRE_SCALES)
				{
#ifdef JP
msg_print("肌が元に戻った。");
#else
					msg_print("You lose your warts.");
#endif

					p_ptr->muta3 &= ~(MUT3_FIRE_SCALES);
				}
			}
			else if (muta_which == MUT3_COLD_SCALES)
			{
				if (p_ptr->muta3 & MUT3_POIS_SCALES)
				{
#ifdef JP
msg_print("鱗が消えた。");
#else
					msg_print("Your skin is no longer made of steel.");
#endif

					p_ptr->muta3 &= ~(MUT3_POIS_SCALES);
				}
			}
			else if (muta_which == MUT3_FEARLESS)
			{
				if (p_ptr->muta2 & MUT2_COWARDICE)
				{
#ifdef JP
msg_print("閉所恐怖症が消えた。");
#else
					msg_print("You are no longer cowardly.");
#endif

					p_ptr->muta2 &= ~(MUT2_COWARDICE);
				}
			}

			else if (muta_which == MUT3_LIMBER)
			{
				if (p_ptr->muta3 & MUT3_ARTHRITIS)
				{
#ifdef JP
msg_print("関節が痛くなくなった。");
#else
					msg_print("Your joints stop hurting.");
#endif

					p_ptr->muta3 &= ~(MUT3_ARTHRITIS);
				}
			}
			else if (muta_which == MUT3_ARTHRITIS)
			{
				if (p_ptr->muta3 & MUT3_LIMBER)
				{
#ifdef JP
msg_print("あなたはしなやかでなくなった。");
#else
					msg_print("You no longer feel limber.");
#endif

					p_ptr->muta3 &= ~(MUT3_LIMBER);
				}
			}
		}
		else if (muta_class == &(p_ptr->muta2))
		{
			if (muta_which == MUT2_COWARDICE)
			{
				if (p_ptr->muta3 & MUT3_FEARLESS)
				{
#ifdef JP
msg_print("恐れ知らずでなくなった。");
#else
					msg_print("You no longer feel fearless.");
#endif

					p_ptr->muta3 &= ~(MUT3_FEARLESS);
				}
			}
			if (muta_which == MUT2_BIGHORN)
			{
				if (p_ptr->muta2 & MUT2_TAIL)
				{
#ifdef JP
msg_print("尻尾が消えた。");
#else
					msg_print("Your nose is no longer elephantine.");
#endif

					p_ptr->muta2 &= ~(MUT2_TAIL);
				}
			}
			if (muta_which == MUT2_TAIL)
			{
				if (p_ptr->muta2 & MUT2_BIGHORN)
				{
#ifdef JP
msg_print("角が消えた。");
#else
					msg_print("You no longer have a hard beak.");
#endif

					p_ptr->muta2 &= ~(MUT2_BIGHORN);
				}
			}
		}
#endif
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


/*:::突然変異をランダムに失う*/
/*:::20回試行し今持っている変異が選ばれればそれを消して終了しTRUEを返す。一度も選ばれなければFALSEを返す。*/
/*:::choose_mut:失う突然変異を指定(IDではない！）しそれだけを失う*/
bool lose_mutation(int choose_mut)
{
	int attempts_left = 20;
	cptr muta_desc = "";
	bool muta_chosen = FALSE;
	u32b muta_which = 0;
	u32b *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	/*:::基本20回試行　選ばれた変異を持っていればbreak*/
	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(217))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ACID;
#ifdef JP
muta_desc = "酸のブレスを吐く能力を失った。";
#else
			muta_desc = "You lose the ability to breathe acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "炎のブレスを吐く能力を失った。";
#else
			muta_desc = "You lose the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "催眠眼の能力を失った。";
#else
			muta_desc = "You lose your hypnotic ability.";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "念動力で物を動かす能力を失った。";
#else
			muta_desc = "You lose the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "自分の意思でテレポートする能力を失った。";
#else
			muta_desc = "You lose the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "精神攻撃の能力を失った。";
#else
			muta_desc = "You lose the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ELEC;
#ifdef JP
muta_desc = "雷のブレスを吐く能力を失った。";
#else
			muta_desc = "You lose the ability to breathe lightning.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "吸血の能力を失った。";
#else
			muta_desc = "You are no longer vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ACCELERATION;
#ifdef JP
muta_desc = "奥歯の加速装置が機能を失った気がする。";
#else
			muta_desc = "You feel like you've lost your ability to accelerate.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BEAST_EAR;
#ifdef JP
muta_desc = "獣耳が引っ込んだ。";
#else
			muta_desc = "Your beast ears recede.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "近距離テレポートの能力を失った。";
#else
			muta_desc = "You lose the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "壁は美味しそうに見えなくなった。";
#else
			muta_desc = "The walls look unappetizing.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "あなたは自分の靴に留まる感じがする。";
#else
			muta_desc = "You feel like staying in your own shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_SOUND;
#ifdef JP
muta_desc = "あなたの声は弱くなった。";
#else
			muta_desc = "Your vocal cords get much weaker.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_COLD;
#ifdef JP
muta_desc = "吹雪を吐く能力を失った。";
#else
			muta_desc = "You lose the ability to breathe frost.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_ITEM;
#ifdef JP
muta_desc = "ダウジングのコツを忘れた。";
#else
			muta_desc = "Your forgot how to perform dowsing.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "制御できる激情を感じなくなった。";
#else
			muta_desc = "You no longer feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_POIS;
#ifdef JP
muta_desc = "毒ガスを吐く能力を失った。";
#else
			muta_desc = "You lose the ability to breathe toxic gas.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "ミダスの手の能力を失った。";
#else
			muta_desc = "You lose the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "突然カビが嫌いになった。";
#else
			muta_desc = "You feel a sudden dislike for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "元素から身を守る方法を忘れた。";
#else
			muta_desc = "You feel like you might be vulnerable.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "ダンジョンを壊す能力を失った。";
#else
			muta_desc = "You lose the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "魔法のアイテムはもう美味しそうに見えなくなった。";
#else
			muta_desc = "Your magic items no longer look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RAISE_WOOD;
#ifdef JP
muta_desc = "植物に嫌われた気がする。";
#else
			muta_desc = "You feel hated by plants.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "たくさんの安堵の吐息が聞こえた。";
#else
			muta_desc = "You hear a massed sigh of relief.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "あちこちへ跳べる気分がなくなった。";
#else
			muta_desc = "You no longer feel jumpy.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "あなたの眼から狂気が失せた。";
#else
			muta_desc = "You lose the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "眼からビームを撃てなくなった。";
#else
			muta_desc = "You lose the ability to shoot beams from your eyes.";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "ダンジョンと地上の間をテレポートする方法を忘れた。";
#else
			muta_desc = "You forgot how to teleport between dungeon and surface.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ID_MUSHROOM;
#ifdef JP
muta_desc = "キノコの匂いの区別がつかなくなった。";
#else
			muta_desc = "You no longer can tell mushrooms apart by smell.";
#endif

			break;
		case 71: case 72:
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DANMAKU;
#ifdef JP
muta_desc = "背中からの力を感じなくなった。";
#else
			muta_desc = "You no longer feel power from your back.";
#endif

			break;
/*
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "物を投げる手が弱くなった気がする。";
#else
			muta_desc = "Your throwing arm feels much weaker.";
#endif

			break;
*/
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "激情が消えた感じがする。";
#else
			muta_desc = "You feel less violent.";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "閉所恐怖症が治った！";
#else
			muta_desc = "Your claustrophobia is cured!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "あなたの意識は明瞭さを取り戻した。";
#else
			muta_desc = "Your consciousness is clear once again.";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "アルコール中毒が治った！";
#else
			muta_desc = "You are cured from alcohol addiction!";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "幻覚をひき起こす精神障害を起こさなくなった！";
#else
			muta_desc = "You are no longer afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_YAKU;
#ifdef JP
muta_desc = "厄から解放された気がする。";
#else
			muta_desc = "You feel free from misfortune.";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HARDHEAD;
#ifdef JP
muta_desc = "頭が柔らかくなった！";
#else
			muta_desc = "Your head grows softer!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "額から角が消えた！";
#else
			muta_desc = "Your horns vanish from your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGHORN;
#ifdef JP
muta_desc = "角が抜け落ちた。";
#else
			muta_desc = "Your horns fall off.";
#endif

			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "デーモンを引き寄せなくなった。";
#else
			muta_desc = "You stop attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "制御不能な魔法のエネルギーを発生しなくなった。";
#else
			muta_desc = "You stop producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "躁鬱質でなくなった。";
#else
			muta_desc = "You are no longer manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "背後に恐ろしい力を感じなくなった。";
#else
			muta_desc = "You no longer feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "世界が明るいと感じる。";
#else
			muta_desc = "You feel the world's a brighter place.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TAIL;
#ifdef JP
muta_desc = "尻尾が消えた。";
#else
			muta_desc = "Your tail disappears.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "動物を引き寄せなくなった。";
#else
			muta_desc = "You stop attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "尻尾が消えた。";
#else
			muta_desc = "Your tail disappears.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "周囲の空間が安定した気がする。";
#else
			muta_desc = "You feel the universe is more stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "通常の体に戻っていく感じがなくなった。";
#else
			muta_desc = "You feel normally strange.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "あなたは物質世界にしっかり存在している。";
#else
			muta_desc = "You are firmly in the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_GHOST;
#ifdef JP
muta_desc = "幽霊の興味を惹かなくなった気がする。";
#else
			muta_desc = "You feel ghosts are no longer interested in you.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_KWAI;
#ifdef JP
muta_desc = "妖怪の興味を惹かなくなった気がする。";
#else
			muta_desc = "You feel youkai are no longer interested in you.";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "ドラゴンを引き寄せなくなった。";
#else
			muta_desc = "You stop attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_XOM;
#ifdef JP
muta_desc = "ゾムはあなたに興味を失ったようだ。";
#else
			muta_desc = "Look like Xom lost interest in you.";
#endif

			break;
		case 109:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIRDBRAIN;
#ifdef JP
muta_desc = "鳥頭が治った。";
#else
			muta_desc = "You are no longer a birdbrain.";
#endif

			break;
		case 110: case 111:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "混沌の神々の興味を惹かなくなった。";
#else
			muta_desc = "You lose the attention of the chaos deities.";
#endif

			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "物質世界に捕らわれている気がする。";
#else
			muta_desc = "You feel like you're trapped in reality.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "パラノイアでなくなった。";
#else
			muta_desc = "You no longer feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "無敵状態の発作を起こさなくなった。";
#else
			muta_desc = "You are no longer blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ASTHMA;
#ifdef JP
muta_desc = "喘息が治った。";
#else
			muta_desc = "Your asthma is cured.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_THE_WORLD;
#ifdef JP
muta_desc = "止まった時の世界に迷い込まなくなった。";
#else
			muta_desc = "You no longer wander into a world of frozen time.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "転びやすくなくなった気がする。";
#else
			muta_desc = "You feel you're not going to stumble and fall that easily.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "筋肉が普通に戻った。";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "筋肉が普通に戻った。";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "脳が普通に戻った。";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "脳が普通に戻った。";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "普通の丈夫さに戻った。";
#else
			muta_desc = "You become ordinarily resilient again.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WARNING;
#ifdef JP
muta_desc = "勘が悪くなった気がする…";
#else
			muta_desc = "You feel your intuition grow duller...";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "虚弱体質が治った！";
#else
			muta_desc = "You are no longer frail!";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SPEEDSTER;
#ifdef JP
muta_desc = "脚力が並に戻った。";
#else
			muta_desc = "Your leg strength is back to normal.";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RES_WATER;
#ifdef JP
muta_desc = "肺活量が普通に戻った。";
#else
			muta_desc = "Your lung capacity returns to normal.";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "顔に目鼻が戻った。";
#else
			muta_desc = "Your facial features return.";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HURT_LITE;
#ifdef JP
muta_desc = "光が苦手ではなくなった。";
#else
			muta_desc = "You are no longer bothered by light.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = "額の目が消えてしまった！";
#else
			muta_desc = "The eye on your forehead vanishes!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "魔法に弱くなった。";
#else
			muta_desc = "You become susceptible to magic again.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "奇妙な音を立てなくなった！";
#else
			muta_desc = "You stop making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BYDO;
#ifdef JP
muta_desc = "バイド細胞の侵食が止まった。";
#else
			muta_desc = "The Bydo cell infection has stopped.";
#endif

			break;
		case 157: case 158:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FISH_TAIL;
#ifdef JP
muta_desc = "下半身が常態に戻った。";
#else
			muta_desc = "Your lower body returns to normal.";
#endif

			break;
		case 159: case 160:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PEGASUS;
#ifdef JP
muta_desc = "髪型が普通に戻った。";
#else
			muta_desc = "Your hairstyle returns to normal.";
#endif

			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ACID_SCALES;
#ifdef JP
muta_desc = "肌が元に戻った。";
#else
			muta_desc = "Your skin returns to normal.";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_SCALES;
#ifdef JP
muta_desc = "肌が元に戻った。";
#else
			muta_desc = "Your skin returns to normal.";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_SCALES;
#ifdef JP
muta_desc = "肌が元に戻った。";
#else
			muta_desc = "Your skin returns to normal.";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_COLD_SCALES;
#ifdef JP
muta_desc = "肌が元に戻った。";
#else
			muta_desc = "Your skin returns to normal.";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_POIS_SCALES;
#ifdef JP
muta_desc = "肌が元に戻った。";
#else
			muta_desc = "Your skin returns to normal.";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "背中の羽根が取れ落ちた。";
#else
			muta_desc = "Your wings fall off.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "再び恐怖を感じるようになった。";
#else
			muta_desc = "You begin to feel fear again.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEVER;
#ifdef JP
muta_desc = "熱病が治った。";
#else
			muta_desc = "Your fever is cured.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "テレパシーの能力を失った！";
#else
			muta_desc = "You lose your telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "筋肉が硬くなった。";
#else
			muta_desc = "Your muscles stiffen.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "関節が痛くなくなった。";
#else
			muta_desc = "Your joints stop hurting.";
#endif

			break;
		case 188:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "黒いオーラは渦巻いて消えた。";
#else
			muta_desc = "Your black aura swirls and fades.";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "無防備な感じはなくなった。";
#else
			muta_desc = "You feel less exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "動作の正確さがなくなった。";
#else
			muta_desc = "You move with less assurance.";
#endif

			break;
		case 193:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "白いオーラは輝いて消えた。";
#else
			muta_desc = "Your white aura shimmers and fades.";
#endif

			break;


		case 194:	case 195:	case 196:
		case 197:	case 198:	case 199:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_WOLF;
			muta_desc = _("オオカミの動物霊は出ていった。", "The wolf animal spirit leaves your body.");
			break;

		case 200:	case 201:	case 202:
		case 203:	case 204:	case 205:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_OTTER;
			muta_desc = _("カワウソの動物霊は出ていった。", "The otter animal spirit leaves your body.");
			break;

		case 206:	case 207:	case 208:
		case 209:	case 210:	case 211:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_EAGLE;
			muta_desc = _("オオワシの動物霊は出ていった。", "The eagle animal spirit leaves your body.");
			break;

		case 212:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_JELLYFISH;
			muta_desc = _("クラゲの動物霊は出ていった。", "The jellyfish animal spirit leaves your body.");
			break;

		case 213:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_COW;
			muta_desc = _("ウシの動物霊は出ていった。", "The cow animal spirit leaves your body.");
			break;

		case 214:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHICKEN;
			muta_desc = _("ヒヨコの動物霊は出ていった。", "The chicken animal spirit leaves your body.");
			break;

		case 215:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_TORTOISE;
			muta_desc = _("カメの動物霊は出ていった。", "The tortoise animal spirit leaves your body.");
			break;

		case 216:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANIWA;
			muta_desc = _("ハニワの……動物？……霊？は出ていった。", "The haniwa...... animal?......spirit? Whatever it is, it leaves your body.");
			break;

		case 217:

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANGOKUOH;
			muta_desc = _("ここはどこだ？記憶が飛んでいる気がする…", "Where am I? I feel my memories flowing...");
			break;



		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (*(muta_class) & muta_which)
			{
				muta_chosen = TRUE;
			}

			///mod140318 永続変異、生来変異は突然変異治療の対象にならない
			if(muta_class == &(p_ptr->muta1) && (muta_which & p_ptr->muta1_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta2) && (muta_which & p_ptr->muta2_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta3) && (muta_which & p_ptr->muta3_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta4) && (muta_which & p_ptr->muta4_perma))	muta_chosen = FALSE;

		}

		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
		return FALSE;
	}
	/*:::突然変異を失う。フラグを操作し再生速度を再計算。*/
	else
	{
		msg_print(muta_desc);
		*(muta_class) &= ~(muta_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		return TRUE;
	}
}



///mutation 突然変異一覧を表示する
void dump_mutations(FILE *OutFile)
{
	if (!OutFile) return;

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_BR_ACID)
		{
#ifdef JP
fprintf(OutFile, " あなたは酸のブレスを吐くことができる。(ダメージ レベルX2)\n");
#else
			fprintf(OutFile, " You can breathe acid (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
fprintf(OutFile, " あなたは炎のブレスを吐くことができる。(ダメージ レベルX2)\n");
#else
			fprintf(OutFile, " You can breathe fire (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
fprintf(OutFile, " あなたの睨みは魅了効果をもつ。\n");
#else
			fprintf(OutFile, " Your gaze is hypnotic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
fprintf(OutFile, " あなたは念動力をもっている。\n");
#else
			fprintf(OutFile, " You are telekinetic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " あなたは自分の意思でテレポートできる。\n");
#else
			fprintf(OutFile, " You can teleport at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
fprintf(OutFile, " あなたは敵を精神攻撃できる。\n");
#else
			fprintf(OutFile, " You can Mind Blast your enemies.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_ELEC)
		{
#ifdef JP
fprintf(OutFile, " あなたは雷のブレスを吐くことができる。(ダメージ レベルX2)\n");
#else
			fprintf(OutFile, " You can breathe lightning (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
fprintf(OutFile, " あなたは吸血鬼のように敵から生命力を吸収することができる。\n");
#else
			fprintf(OutFile, " You can drain life from a foe like a vampire.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ACCELERATION)
		{
#ifdef JP
fprintf(OutFile, " あなたは一時的に加速することができる。\n");
#else
			fprintf(OutFile, " You can temporarily accelerate.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BEAST_EAR)
		{
#ifdef JP
fprintf(OutFile, " あなたは鋭敏な聴覚で近くのモンスターを感知できる。\n");
#else
			fprintf(OutFile, " Your keen hearing lets you detect nearby monsters.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
fprintf(OutFile, " あなたは短い距離をテレポートできる。\n");
#else
			fprintf(OutFile, " You can teleport yourself short distances.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
fprintf(OutFile, " あなたは硬い岩を食べることができる。\n");
#else
			fprintf(OutFile, " You can consume solid rock.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
fprintf(OutFile, " あなたは他の者と場所を入れ替わることができる。\n");
#else
			fprintf(OutFile, " You can switch locations with another being.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_SOUND)
		{
#ifdef JP
fprintf(OutFile, " あなたは大声で敵を攻撃できる。(ダメージ 最大でレベルX2)\n");
#else
			fprintf(OutFile, " You can attack enemies with a loud shout (dam up to lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_COLD)
		{
#ifdef JP
fprintf(OutFile, " あなたは吹雪を吐くことができる。(ダメージ レベルX2)\n");
#else
			fprintf(OutFile, " You can breathe frost (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_ITEM)
		{
#ifdef JP
fprintf(OutFile, " あなたは近くに落ちているアイテムを感知することができる。\n");
#else
			fprintf(OutFile, " You can feel the presence of nearby items.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
fprintf(OutFile, " あなたは自分の意思で狂乱戦闘状態になることができる。\n");
#else
			fprintf(OutFile, " You can drive yourself into a berserk frenzy.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_POIS)
		{
#ifdef JP
fprintf(OutFile, " あなたはガスのブレスを吐くことができる。(ダメージ レベルX2)\n");
#else
			fprintf(OutFile, " You can breathe poison (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
fprintf(OutFile, " あなたは通常アイテムを金に変えることができる。\n");
#else
			fprintf(OutFile, " You can turn ordinary items to gold.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
fprintf(OutFile, " あなたは周囲にキノコを生やすことができる。\n");
#else
			fprintf(OutFile, " You can cause mushrooms to grow near you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
fprintf(OutFile, " あなたは元素の攻撃に対して身を硬くすることができる。\n");
#else
			fprintf(OutFile, " You can harden yourself to the ravages of the elements.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
fprintf(OutFile, " あなたは周囲のダンジョンを崩壊させることができる。\n");
#else
			fprintf(OutFile, " You can bring down the dungeon around your ears.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
fprintf(OutFile, " あなたは魔法のエネルギーを自分の物として使用できる。\n");
#else
			fprintf(OutFile, " You can consume magic energy for your own use.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RAISE_WOOD)
		{
#ifdef JP
fprintf(OutFile, " あなたは自分のいる所に森林を生成することができる。\n");
#else
			fprintf(OutFile, " You can create forest at the place you're standing.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
fprintf(OutFile, " あなたは今いるフロアでモンスターの増殖を阻止することができる。\n");
#else
			fprintf(OutFile, " You can prevent monsters on your floor from multiplying.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
fprintf(OutFile, " あなたは攻撃した後身を守るため逃げることができる。\n");
#else
			fprintf(OutFile, " You can run for your life after hitting something.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
fprintf(OutFile, " あなたの眼は狂気をもたらす。 \n");
#else
			fprintf(OutFile, " Your eyes can cause insanity.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
fprintf(OutFile, " あなたは目からビームを撃つことができる。\n");
#else
			fprintf(OutFile, " Your eyes can fire laser beams.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
fprintf(OutFile, " あなたは街とダンジョンの間を行き来することができる。\n");
#else
			fprintf(OutFile, " You can travel between town and the depths.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ID_MUSHROOM)
		{
#ifdef JP
fprintf(OutFile, " あなたはキノコの鑑定ができる。\n");
#else
			fprintf(OutFile, " You can identify mushrooms.\n");
#endif

		}
		//v1.1.39
		if (p_ptr->muta1 & MUT1_DANMAKU)
		{
#ifdef JP
fprintf(OutFile, " あなたは季節の力を強力な弾幕として放つことができる。(魅力x5)\n");
#else
			fprintf(OutFile, " You can release power of seasons as powerful danmaku (CHR * 5).\n");
#endif

		}
/*
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
fprintf(OutFile, " あなたはアイテムを力強く投げることができる。\n");
#else
			fprintf(OutFile, " You can hurl objects with great force.\n");
#endif

		}
*/
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
fprintf(OutFile, " あなたは狂戦士化の発作を起こす。\n");
#else
			fprintf(OutFile, " You are subject to berserker fits.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
fprintf(OutFile, " あなたは時々恐慌に襲われる。\n");
#else
			fprintf(OutFile, " You are subject to cowardice.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " あなたは時々意識が曖昧になる。\n");
#else
			fprintf(OutFile, " You are teleporting randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
fprintf(OutFile, " あなたはアルコール中毒だ。(飲み過ぎても倒れないが酒が切れると全能力-3 )\n");
#else
			fprintf(OutFile, " You are addicted to alcohol (Won't collapse if you drink too much, -3 to all stats if sober).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
fprintf(OutFile, " あなたは幻覚を引き起こす精神錯乱に侵されている。\n");
#else
			fprintf(OutFile, " You have a hallucinatory insanity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_YAKU)
		{
#ifdef JP
fprintf(OutFile, " あなたの周りには厄が集まってくる。\n");
#else
			fprintf(OutFile, " You are gathering misfortune in your vicinity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
fprintf(OutFile, " あなたは制御不能な魔法のエネルギーを発している。\n");
#else
			fprintf(OutFile, " You are producing magical energy uncontrollably.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
fprintf(OutFile, " あなたはデーモンを引きつける。\n");
#else
			fprintf(OutFile, " You attract demons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HARDHEAD)
		{
#ifdef JP
fprintf(OutFile, " あなたは石頭だ。(知能-1、賢さ+1、AC+5)\n");
#else
			fprintf(OutFile, " You are hardheaded (INT -1, WIS +1, AC +5).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
fprintf(OutFile, " あなたは角が生えている。\n");
#else
			fprintf(OutFile, " You have horns.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGHORN)
		{
#ifdef JP
fprintf(OutFile, " あなたは立派な角を持っている。\n");
#else
			fprintf(OutFile, " You have splendid horns.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
fprintf(OutFile, " あなたはランダムに早く動いたり遅く動いたりする。\n");
#else
			fprintf(OutFile, " You move faster or slower randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
fprintf(OutFile, " あなたは時々近くのモンスターを消滅させる。\n");
#else
			fprintf(OutFile, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
fprintf(OutFile, " あなたは時々周囲の光を吸収して栄養にする。\n");
#else
			fprintf(OutFile, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TAIL)
		{
#ifdef JP
fprintf(OutFile, " あなたはしなやかな尻尾を持っている。(器用+4)\n");
#else
			fprintf(OutFile, " You have a prehensile tail (DEX +4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
fprintf(OutFile, " あなたは動物を引きつける。\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGTAIL)
		{
#ifdef JP
fprintf(OutFile, " あなたは竜のような大きな尻尾を持っている。\n");
#else
			fprintf(OutFile, " You have a large dragon-like tail.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
fprintf(OutFile, " あなたはしばしば純カオスに包まれる。\n");
#else
			fprintf(OutFile, " You occasionally are surrounded with raw chaos.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
fprintf(OutFile, " あなたはときどき変異から回復する。\n");
#else
			fprintf(OutFile, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
fprintf(OutFile, " あなたの肉体は幽体化したり実体化したりする。\n");
#else
			fprintf(OutFile, " You fade in and out of physical reality.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_GHOST)
		{
#ifdef JP
fprintf(OutFile, " あなたは幽霊を引きつける。\n");
#else
			fprintf(OutFile, " You attract ghosts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_KWAI)
		{
#ifdef JP
fprintf(OutFile, " あなたは妖怪を引きつける。\n");
#else
			fprintf(OutFile, " You attract youkai.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
fprintf(OutFile, " あなたはドラゴンを引きつける。\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_XOM)
		{
#ifdef JP
if(p_ptr->lev > 24) fprintf(OutFile, " あなたはゾムのお気に入りの玩具だ。\n");
else fprintf(OutFile, " あなたはゾムの玩具だ。\n");
#else
			if(p_ptr->lev > 24) fprintf(OutFile, " You are Xom's favorite plaything.\n");
            else fprintf(OutFile, " You are Xom's plaything.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIRDBRAIN)
		{
#ifdef JP
fprintf(OutFile, " あなたはすぐに物事を忘れる。\n");
#else
			fprintf(OutFile, " You are quick to forget things.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
fprintf(OutFile, " あなたはカオスの守護悪魔から褒美をうけとる。\n");
#else
			fprintf(OutFile, " You receive gifts from a Chaos patron.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
fprintf(OutFile, " あなたはしばしば他の「影」に迷い込む。\n");
#else
			fprintf(OutFile, " You occasionally stumble into other shadows.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
fprintf(OutFile, " あなたは敵に関する警告を感じる。\n");
#else
			fprintf(OutFile, " You receive warnings about your foes.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
fprintf(OutFile, " あなたは時々負け知らずな気分になる。\n");
#else
			fprintf(OutFile, " You occasionally feel invincible.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ASTHMA)
		{
#ifdef JP
fprintf(OutFile, " あなたは喘息持ちだ。\n");
#else
			fprintf(OutFile, " You are asthmatic.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_THE_WORLD)
		{
#ifdef JP
fprintf(OutFile, " あなたの周りで時々時間が停止する。\n");
#else
			fprintf(OutFile, " Time occasionally stops in your vicinity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
fprintf(OutFile, " あなたはよくつまづいて物を落とす。\n");
#else
			fprintf(OutFile, " You occasionally stumble and drop things.\n");
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
			fprintf(OutFile, " あなたは超人的に強い。(腕力+4)\n");
#else
			fprintf(OutFile, " You are superhumanly strong (+4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
			fprintf(OutFile, " あなたは虚弱だ。(腕力-4)\n");
#else
			fprintf(OutFile, " You are puny (-4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
			fprintf(OutFile, " あなたの脳は生体コンピュータだ。(知能＆賢さ+4)\n");
#else
			fprintf(OutFile, " Your brain is a living computer (+4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
			fprintf(OutFile, " あなたは精神薄弱だ。(知能＆賢さ-4)\n");
#else
			fprintf(OutFile, " You are moronic (-4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
			fprintf(OutFile, " あなたの体は弾力性に富んでいる。(耐久+4)\n");
#else
			fprintf(OutFile, " You are very resilient (+4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WARNING)
		{
#ifdef JP
			fprintf(OutFile, " あなたは勘が鋭く身に迫った危険を察知できる。\n");
#else
			fprintf(OutFile, " Your sharp intuition warns you of danger.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
			fprintf(OutFile, " あなたは虚弱体質だ。(耐久-4)\n");
#else
			fprintf(OutFile, " You are frail (-4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
#ifdef JP
			fprintf(OutFile, " あなたは強靭な脚力をもっている。\n");
#else
			fprintf(OutFile, " You have strong legs (grants swift movement).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RES_WATER)
		{
#ifdef JP
			fprintf(OutFile, " あなたは水攻撃に耐性を持つ。\n");
#else
			fprintf(OutFile, " You are resistant to water attacks.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
			fprintf(OutFile, " あなたはのっぺらぼうだ。(魅力-3)\n");
#else
			fprintf(OutFile, " Your face is featureless (-3 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HURT_LITE)
		{
#ifdef JP
			fprintf(OutFile, " あなたは明るい光が苦手だ。\n");
#else
			fprintf(OutFile, " You are hurt by bright light.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
			if (p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				fprintf(OutFile, " あなたは額に第四の目を持っている。\n");
			else
				fprintf(OutFile, " あなたは額に第三の目を持っている。\n");
#else
			if (p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				fprintf(OutFile, " You have a fourth eye on your forehead.\n");
			else
				fprintf(OutFile, " You have a third eye on your forehead.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
			fprintf(OutFile, " あなたは魔法への耐性をもっている。\n");
#else
			fprintf(OutFile, " You are resistant to magic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
			fprintf(OutFile, " あなたは変な音を発している。(隠密-3)\n");
#else
			fprintf(OutFile, " You make a lot of strange noise (-3 stealth).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BYDO)
		{
#ifdef JP
			fprintf(OutFile, " あなたはバイド細胞に浸食されている。(全能力上昇、魅力大幅減少、反感　三日以内に治療しないと・・？)\n");
#else
			fprintf(OutFile, " You are infected by Bydo cells. (+all, ---CHR, aggravation If you don't get cured in 3 days...?)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
#ifdef JP
			fprintf(OutFile, " あなたの下半身は人魚の尻尾だ。\n");
#else
			fprintf(OutFile, " You have a merfolk tail in place of your lower body.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PEGASUS)
		{
#ifdef JP
			fprintf(OutFile, " あなたの頭は昇天ペガサスMIX盛りだ。(AC+5 頭防具を装備できない)\n");
#else
			fprintf(OutFile, " You have the Ascension Pegasus MIX Prime hairstyle. (AC +5, cannot wear headgear)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ACID_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " あなたの肌は黒い鱗に覆われている。(AC+5 酸耐性 魅力-2)\n");
#else
			fprintf(OutFile, " Your skin is covered in black scales. (AC +5 Resist acid CHR -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " あなたの肌は青い鱗に覆われている。(AC+5 電耐性 魅力-2)\n");
#else
			fprintf(OutFile, " Your skin is covered in blue scales. (AC +5 Resist lightning CHR -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FIRE_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " あなたの肌は赤い鱗に覆われている。(AC+5 火耐性 魅力-2)\n");
#else
			fprintf(OutFile, " Your skin is covered in red scales. (AC +5 Resist fire CHR -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_COLD_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " あなたの肌は白い鱗に覆われている。(AC+5 冷耐性 魅力-2)\n");
#else
			fprintf(OutFile, " Your skin is covered in white scales. (AC +5 Resist cold CHR -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_POIS_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " あなたの肌は緑の鱗に覆われている。(AC+5 毒耐性 魅力-2)\n");
#else
			fprintf(OutFile, " Your skin is covered in green scales. (AC +5 Resist poison CHR -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
			fprintf(OutFile, " あなたは羽を持っている。\n");
#else
			fprintf(OutFile, " You have wings.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
#ifdef JP
			fprintf(OutFile, " あなたは全く恐怖を感じない。\n");
#else
			fprintf(OutFile, " You are completely fearless.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEVER)
		{
#ifdef JP
			fprintf(OutFile, " あなたは原因不明の熱病に冒されている。(知能器用-2)\n");
#else
			fprintf(OutFile, " You are struck with unidentified fever. (INT/DEX -2)\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
#ifdef JP
			fprintf(OutFile, " あなたはテレパシーを持っている。\n");
#else
			fprintf(OutFile, " You are telepathic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
			fprintf(OutFile, " あなたの体は非常にしなやかだ。(器用+3)\n");
#else
			fprintf(OutFile, " Your body is very limber (+3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
			fprintf(OutFile, " あなたはいつも関節に痛みを感じている。(器用-3)\n");
#else
			fprintf(OutFile, " Your joints ache constantly (-3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
			fprintf(OutFile, " あなたは元素の攻撃に弱い。\n");
#else
			fprintf(OutFile, " You are susceptible to damage from the elements.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
			fprintf(OutFile, " あなたの動作は正確で力強い。(隠密+1)\n");
#else
			fprintf(OutFile, " Your movements are precise and forceful (Stealth +1).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " あなたは白いオーラにつつまれている。\n");
#else
			fprintf(OutFile, " There is a white aura surrounding you.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " あなたは黒いオーラにつつまれている。\n");
#else
			fprintf(OutFile, " There is a black aura surrounding you.\n");
#endif
		}

	}
	if(p_ptr->muta4)
	{
		//v1.1.64
		if (p_ptr->muta4 & MUT4_GHOST_WOLF)
		{
			fprintf(OutFile, _(" あなたはオオカミの動物霊に憑依されている。(肉体強化/知能賢さ低下)\n",
                                " You are possessed by a wolf animal spirit. (Phys+ / INT, WIS-)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_OTTER)
		{
			fprintf(OutFile, _(" あなたはカワウソの動物霊に憑依されている。(技能上昇/全能力やや低下)\n",
                                " You are possessed by an otter animal spirit. (Skills+ / All stats a bit-)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
		{
			fprintf(OutFile, _(" あなたはオオワシの動物霊に憑依されている。(知能賢さ探索上昇/腕力耐久低下\n",
                                " You are possessed by an eagle animal spirit. (INT, WIS, Srh+ / STR, CON-)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH)
		{
			fprintf(OutFile, _(" あなたはクラゲの動物霊に憑依されている。(毒麻痺水耐性/腕力大幅低下\n",
                                " You are possessed by a jellyfish animal spirit. (Res pois/wtr, FA / STR--)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_COW)
		{
			fprintf(OutFile, _(" あなたはウシの動物霊に憑依されている。(腕力耐久大幅上昇/隠密大幅低下)\n",
                                " You are possessed by a cow animal spirit. (STR, CON++ / Stealth--)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_CHICKEN)
		{
			fprintf(OutFile, _(" あなたはヒヨコの動物霊に憑依されている。(魅力大幅上昇/AC低下)\n",
                                " You are possessed by a chicken animal spirit. (CHR++ / AC-)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_TORTOISE)
		{
			fprintf(OutFile, _(" あなたはカメの動物霊に憑依されている。(AC大幅上昇/加速低下)\n",
                                " You are possessed by a tortoise animal spirit. (AC++ / Speed-)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
		{
			fprintf(OutFile, _(" あなたはハニワの動物…？霊…？に憑依されている。(魔法防御大幅上昇/轟音破片弱点25%%)\n",
                                " You are possessed by a haniwa animal...? spirit...? (MR++ / Snd,shard vul 25%%)\n"));
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
		{
			fprintf(OutFile, _(" あなたは大怨霊に乗っ取られている...(地獄耐性/破邪弱点/発動エナジードレイン)\n",
                                " You are taken over by a vengeful spirit... (Res nether/Vul holy/Energy drain abl)\n"));
		}
	}
}


/*
 * List mutations we have...
 */
void do_cmd_knowledge_mutations(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_mutations(fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "突然変異", 0, 0);
#else
	show_file(TRUE, file_name, "Mutations", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


int count_mutations(void)
{
	return (count_bits(p_ptr->muta1) +
		count_bits(p_ptr->muta2) +
		count_bits(p_ptr->muta3) +
		count_bits(p_ptr->muta4));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
/*:::突然変異による再生速度変化を計算　最大値が100としている*/
int calc_mutant_regenerate_mod(void)
{
	int regen;
	int mod = 10;
	int count = count_mutations();

	int count_perma;

	count_perma = (count_bits(p_ptr->muta1_perma) + count_bits(p_ptr->muta2_perma) + count_bits(p_ptr->muta3_perma) + count_bits(p_ptr->muta4_perma));

	///mod140318 再生速度計算に永続変異や生来型変異を含まない
	count -= count_perma;

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if (p_ptr->pseikaku == SEIKAKU_LUCKY) count--;
	/*:::獣人はペナルティが少ない　10個目以降もペナルティ半分*/
	/*
	if (p_ptr->prace == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}
	*/
	//v1.1.39 ラルバは突然変異に影響されない
	if (p_ptr->pclass == CLASS_LARVA) count = 0;

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}

/*:::口を使う何かのコマンドでこれを実行　歌と詠唱を中断する*/
void mutation_stop_mouth()
{
	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();
}

/*:::発動型変異の実行*/
//注意:MUT1以外の発動変異はcmd_racial_power_aux()へ行く
bool mutation_power_aux(u32b power)
{
	int     dir = 0;
	int     lvl = p_ptr->lev;

	if (hack_flag_powerup_mutation) lvl *= 2;

	switch (power)
	{
		case MUT1_BR_ACID:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("あなたは酸のブレスを吐いた...");
#else
			msg_print("You breathe acid...");
#endif
			//v1.1.39 ブレス形状に
			fire_ball(GF_ACID, dir, lvl * 2, -2);
			break;

		case MUT1_BR_FIRE:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("あなたは火炎のブレスを吐いた...");
#else
			msg_print("You breathe fire...");
#endif

			//v1.1.39 ブレス形状に
			fire_ball(GF_FIRE, dir, lvl * 2, -2);
			break;

		case MUT1_HYPN_GAZE:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("あなたは幻惑的な眼で睨みつけた...");
#else
			msg_print("Your eyes look mesmerizing...");
#endif

			(void)charm_monster(dir, lvl);
			break;

		case MUT1_TELEKINES:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			fetch(dir, lvl * 10, TRUE);
			break;

		case MUT1_VTELEPORT:
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			teleport_player(10 + 4 * lvl, 0L);
			break;

		case MUT1_MIND_BLST:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("集中している...");
#else
			msg_print("You concentrate...");
#endif

			fire_bolt(GF_PSI, dir, damroll(3 + ((lvl - 1) / 5), 3));
			break;

		case MUT1_BR_ELEC:
			if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
			msg_print("あなたは雷のブレスを吐いた...");
#else
			msg_print("You breathe lightning...");
#endif
			mutation_stop_mouth();

			//v1.1.39 ブレス形状に
			fire_ball(GF_ELEC, dir, lvl * 2, -2);
			break;

		case MUT1_VAMPIRISM:
			{
				int x, y, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				mutation_stop_mouth();

				if (!(c_ptr->m_idx))
				{
#ifdef JP
					msg_print("何もない場所に噛みついた！");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("あなたはニヤリとして牙をむいた...");
#else
				msg_print("You grin and bare your fangs...");
#endif


				dummy = lvl * 2;

				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(dummy);
					else
#ifdef JP
						msg_print("あなたは空腹ではありません。");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = p_ptr->food + MIN(5000, 100 * dummy);
					if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
				}
				else
#ifdef JP
					msg_print("げぇ！ひどい味だ。");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case MUT1_ACCELERATION:
			mutation_stop_mouth();
		//	(void)detect_treasure(DETECT_RAD_DEFAULT);
			set_fast(30, FALSE);

			break;

		case MUT1_BEAST_EAR:
			msg_print(_("あなたはじっと耳を澄ませた...", "You carefully listen..."));
			mutation_stop_mouth();
			(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
			(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			break;

		case MUT1_BLINK:
			teleport_player(10, 0L);
			break;
			/*:::壁食い*/
		case MUT1_EAT_ROCK:
			if(!eat_rock()) return FALSE;

			break;

		case MUT1_SWAP_POS:
			project_length = -1;
			if (!get_aim_dir(&dir))
			{
				project_length = 0;
				return FALSE;
			}
			(void)teleport_swap(dir);
			project_length = 0;
			break;

		case MUT1_BR_SOUND:
			mutation_stop_mouth();

#ifdef JP
			if(one_in_(128))msg_format("「　　ほう　　」");
			else if(one_in_(128))msg_format("「なんでそーなるの！」");
			else if(one_in_(2))	msg_format("「ば　か　も　の　－　！」");
			else 	msg_format("「Yah○o !」");
#else
            msg_format("[  Yahoo!  ]");
#endif

			(void)fire_ball(GF_SOUND, 0, lvl * 4, 8);
			//	(void)fire_ball(GF_SOUND, 0, 2 * lvl, 8);
			(void)aggravate_monsters(0,FALSE);
			break;

		case MUT1_BR_COLD:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("あなたは冷気のブレスを吐いた...");
#else
			msg_print("You breathe frost...");
#endif

			fire_ball(GF_COLD, dir, lvl * 2, -2);
			break;
		case MUT1_DET_ITEM:
			{
			msg_print(_("あなたはダウジングを始めた...", "You begin dowsing..."));
			detect_objects_normal(DETECT_RAD_DEFAULT);
				/*
				int i;
				for (i = 0; i < INVEN_TOTAL; i++)
				{
					object_type *o_ptr = &inventory[i];

					if (!o_ptr->k_idx) continue;
					if (!object_is_cursed(o_ptr)) continue;

					o_ptr->feeling = FEEL_CURSED;
				}
				*/
			}
			break;

		case MUT1_BERSERK:
			(void)set_shero(randint1(25) + 25, FALSE);
			(void)hp_player(30);
			(void)set_afraid(0);
			break;

		case MUT1_BR_POIS:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("あなたはガスのブレスを吐いた...");
#else
			msg_print("You breathe gas...");
#endif

			fire_ball(GF_POIS, dir, lvl * 2, -2);
			break;


/*
#ifdef JP
			if (!get_check("変身します。よろしいですか？")) return FALSE;
#else
			if (!get_check("You will polymorph your self. Are you sure? ")) return FALSE;
#endif
			do_poly_self();
			break;
*/
		case MUT1_MIDAS_TCH:
			if (!alchemy()) return FALSE;
			break;

		/* Summon pet molds around the player */
		case MUT1_GROW_MOLD:
			{
				int i;
				for (i = 0; i < 8; i++)
				{
					summon_specific(-1, py, px, lvl, SUMMON_BIZARRE1, PM_FORCE_PET);
				}
			}
			break;

		case MUT1_RESIST:
			{
				int num = lvl / 10;
				int dur = randint1(20) + 20;

				if (num > 5) num = 5;

				if (randint0(5) < num)
				{
					(void)set_oppose_acid(dur, FALSE);
					num--;
				}
				if (randint0(4) < num)
				{
					(void)set_oppose_elec(dur, FALSE);
					num--;
				}
				if (randint0(3) < num)
				{
					(void)set_oppose_fire(dur, FALSE);
					num--;
				}
				if (randint0(2) < num)
				{
					(void)set_oppose_cold(dur, FALSE);
					num--;
				}
				if (num)
				{
					(void)set_oppose_pois(dur, FALSE);
					num--;
				}
			}
			break;

		case MUT1_EARTHQUAKE:
			(void)earthquake(py, px, 10);
			break;

		case MUT1_EAT_MAGIC:
			if (!eat_magic(p_ptr->lev * 2)) return FALSE;
			break;

		case MUT1_RAISE_WOOD:
			{

			if (cave_clean_bold(py, px)){
				msg_print(_("あなたの足元から木が生えてきた！", "A tree grows from beneath your feet!"));
				cave_set_feat(py, px, feat_tree);
			}
			else
				msg_print(_("ここでは植物が育たない！", "You can't grow plants here!"));

		//	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
		//	project(0, 1, py, px, 0, GF_MAKE_TREE, flg, -1);

			//report_magics();
			break;
			}
			/*:::集団的生殖不良変異*/
		case MUT1_STERILITY:
			/* Fake a population explosion. */
#ifdef JP
			msg_print("あなたの怨嗟の声がダンジョン中に響き渡った・・・");
			take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "禁欲を強いた疲労", -1);
#else
			msg_print("The voice of your grudge resounds through the dungeon...");
			take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "the strain of forcing abstinence", -1);
#endif

			num_repro += MAX_REPRO;
			break;
			/*:::H&A変異*/
		case MUT1_PANIC_HIT:
			{
				int x, y;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (cave[y][x].m_idx)
				{
					py_attack(y, x, 0);
					if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
						msg_print("うまく逃げられなかった。");
#else
						msg_print("You failed to teleport.");
#endif
					else teleport_player(30, 0L);
				}
				else
				{
#ifdef JP
					msg_print("その方向にはモンスターはいません。");
#else
					msg_print("You don't see any monster in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;

		case MUT1_DAZZLE:
			stun_monsters(lvl * 4);
			confuse_monsters(lvl * 4);
			turn_monsters(lvl * 4);
			break;

		case MUT1_LASER_EYE:
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(GF_LITE, dir, 3 * lvl);
			break;

		case MUT1_RECALL:
			if (!word_of_recall()) return FALSE;
			break;

			/*:::邪悪消滅変異*/
		case MUT1_ID_MUSHROOM:
			{

				if (!ident_spell_2(1)) return FALSE;

#if 0
				int x, y;
				cave_type *c_ptr;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("邪悪な存在を感じとれません！");
#else
					msg_print("You sense no evil there!");
#endif

					break;
				}

				m_ptr = &m_list[c_ptr->m_idx];
				r_ptr = &r_info[m_ptr->r_idx];

				if ((r_ptr->flags3 & RF3_ANG_CHAOS) &&
				    !(r_ptr->flags1 & RF1_QUESTOR) &&
				    !(r_ptr->flags1 & RF1_UNIQUE) &&
				    !p_ptr->inside_arena && !p_ptr->inside_quest &&
					(r_ptr->level < randint1(p_ptr->lev+50)) &&
					!(m_ptr->mflag2 & MFLAG2_NOGENO))
				{
					if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
					}

					/* Delete the monster, rather than killing it. */
					delete_monster_idx(c_ptr->m_idx);
#ifdef JP
					msg_print("その邪悪なモンスターは硫黄臭い煙とともに消え去った！");
#else
					msg_print("The evil creature vanishes in a puff of sulfurous smoke!");
#endif

				}
				else
				{
#ifdef JP
					msg_print("祈りは効果がなかった！");
#else
					msg_print("Your invocation is ineffectual!");
#endif

					if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
				}
#endif
			}
			break;
		//v1.1.39
		case MUT1_DANMAKU:
			{
				int dam_tmp;
				if (!get_aim_dir(&dir)) return FALSE;
				msg_print(_("あなたは強力な弾幕を放った！", "You shoot powerful danmaku!"));
				dam_tmp = (p_ptr->stat_ind[A_CHR] + 3) * 5;

				if (hack_flag_powerup_mutation) dam_tmp *= 2;

				fire_ball(GF_DISP_ALL, dir, dam_tmp, 3);

			}
			break;
#if 0
		/* XXX_XXX_XXX Hack!  MUT1_LAUNCHER is negative, see above */

		case 3: /* MUT1_LAUNCHER */
			/* Gives a multiplier of 2 at first, up to 3 at 40th */
			if (!do_cmd_throw_aux(2 + lvl / 40, FALSE, -1)) return FALSE;
			break;
#endif
		default:
			energy_use = 0;
#ifdef JP
			msg_format("能力 %s は実装されていません。", power);
#else
			msg_format("Power %s not implemented. Oops.", power);
#endif

	}

	return TRUE;
}

//肉体的なランダム突然変異を獲得する。変容魔法の「肉体変容」を使ったときと魔法の森の謎の豆を食べたとき呼ばれる
void gain_physical_mutation(void)
{
	int attempt = 10;

	//肉体的な突然変異のリスト
	//gain_random_mutation()に渡すパラメータ
	int muta_lis[] =
	{
		1,5,8,17,19,24,31,35,38,46,62,65,81,83,85,95,98,100,109,120,123,126,129,136,138,141,143,146,154,157,161,163,165,168,171,173,182,185
	};

	while ((attempt-- > 0) && !gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int)) - 1]));


}
