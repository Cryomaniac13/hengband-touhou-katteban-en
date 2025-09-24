/* File: spells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell code (part 2) */

#include "angband.h"
#include "grid.h"


/*
 * self-knowledge... idea from nethack.  Useful for determining powers and
 * resistences of items.  It saves the screen, clears it, then starts listing
 * attributes, a screenful at a time.  (There are a LOT of attributes to
 * list.  It will probably take 2 or 3 screens for a powerful character whose
 * using several artifacts...) -CFT
 *
 * It is now a lot more efficient. -BEN-
 *
 * See also "identify_fully()".
 *
 * XXX XXX XXX Use the "show_file()" method, perhaps.
 */
/*:::自己分析*/
void self_knowledge(void)
{
	int i = 0, j, k;

	int v_nr = 0;
	char v_string [8] [128];
	char s_string [6] [128];

	u32b flgs[TR_FLAG_SIZE];

	object_type *o_ptr;

	char Dummy[80];
	char buf[2][80];

	cptr info[220];

	int plev = p_ptr->lev;

	int percent;

	for (j = 0; j < TR_FLAG_SIZE; j++)
		flgs[j] = 0L;

	p_ptr->knowledge |= (KNOW_STAT | KNOW_HPRATE);

	strcpy(Dummy, "");

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * p_ptr->hitdie +
		((PY_MAX_LEVEL - 1+3) * (p_ptr->hitdie + 1))));

#ifdef JP
sprintf(Dummy, "現在の体力ランク : %d/100", percent);
#else
	sprintf(Dummy, "Your current Life Rating is %d/100.", percent);
#endif

	strcpy(buf[0], Dummy);
	info[i++] = buf[0];
	info[i++] = "";

	//chg_virtue(V_KNOWLEDGE, 1);
	//chg_virtue(V_ENLIGHTEN, 1);

	/* Acquire item flags from equipment */
	for (k = INVEN_RARM; k < INVEN_TOTAL; k++)
	{
		u32b tflgs[TR_FLAG_SIZE];

		o_ptr = &inventory[k];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the flags */
		object_flags(o_ptr, tflgs);

		/* Extract flags */
		for (j = 0; j < TR_FLAG_SIZE; j++)
			flgs[j] |= tflgs[j];
	}

///mod140104 パラメータ上限と表記を変更 自己分析で能力最大値を表示しないようにした
#if 0
#ifdef JP
	info[i++] = "能力の最大値";
#else
	info[i++] = "Limits of maximum stats";
#endif

	for (v_nr = 0; v_nr < 6; v_nr++)
	{
		char stat_desc[80];

		sprintf(stat_desc, "%s 18/%d", stat_names[v_nr], p_ptr->stat_max_max[v_nr]-18);

		strcpy(s_string[v_nr], stat_desc);

		info[i++] = s_string[v_nr];
	}
	info[i++] = "";
#endif



	///del131229 align virtue
#if 0
#ifdef JP
	sprintf(Dummy, "現在の属性 : %s(%ld)", your_alignment(), (long int)p_ptr->align);
#else
	sprintf(Dummy, "Your alighnment : %s(%ld)", your_alignment(), (long int)p_ptr->align);
#endif
	strcpy(buf[1], Dummy);
	info[i++] = buf[1];
	for (v_nr = 0; v_nr < 8; v_nr++)
	{
		char v_name [20];
		char vir_desc[80];
		int tester = p_ptr->virtues[v_nr];

		strcpy(v_name, virtue[(p_ptr->vir_types[v_nr])-1]);

#ifdef JP
		sprintf(vir_desc, "おっと。%sの情報なし。", v_name);
#else
		sprintf(vir_desc, "Oops. No info about %s.", v_name);
#endif
		if (tester < -100)
#ifdef JP
			sprintf(vir_desc, "[%s]の対極 (%d)",
#else
			sprintf(vir_desc, "You are the polar opposite of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -80)
#ifdef JP
			sprintf(vir_desc, "[%s]の大敵 (%d)",
#else
			sprintf(vir_desc, "You are an arch-enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -60)
#ifdef JP
			sprintf(vir_desc, "[%s]の強敵 (%d)",
#else
			sprintf(vir_desc, "You are a bitter enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -40)
#ifdef JP
			sprintf(vir_desc, "[%s]の敵 (%d)",
#else
			sprintf(vir_desc, "You are an enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -20)
#ifdef JP
			sprintf(vir_desc, "[%s]の罪者 (%d)",
#else
			sprintf(vir_desc, "You have sinned against %s (%d).",
#endif
				v_name, tester);
		else if (tester < 0)
#ifdef JP
			sprintf(vir_desc, "[%s]の迷道者 (%d)",
#else
			sprintf(vir_desc, "You have strayed from the path of %s (%d).",
#endif
				v_name, tester);
		else if (tester == 0)
#ifdef JP
			sprintf(vir_desc, "[%s]の中立者 (%d)",
#else
			sprintf(vir_desc,"You are neutral to %s (%d).",
#endif
				v_name, tester);
		else if (tester < 20)
#ifdef JP
			sprintf(vir_desc, "[%s]の小徳者 (%d)",
#else
			sprintf(vir_desc,"You are somewhat virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 40)
#ifdef JP
			sprintf(vir_desc, "[%s]の中徳者 (%d)",
#else
			sprintf(vir_desc,"You are virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 60)
#ifdef JP
			sprintf(vir_desc, "[%s]の高徳者 (%d)",
#else
			sprintf(vir_desc,"You are very virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 80)
#ifdef JP
			sprintf(vir_desc, "[%s]の覇者 (%d)",
#else
			sprintf(vir_desc,"You are a champion of %s (%d).",
#endif
				v_name, tester);
		else if (tester < 100)
#ifdef JP
			sprintf(vir_desc, "[%s]の偉大な覇者 (%d)",
#else
			sprintf(vir_desc,"You are a great champion of %s (%d).",
#endif
				v_name, tester);
		else
#ifdef JP
			sprintf(vir_desc, "[%s]の具現者 (%d)",
#else
			sprintf(vir_desc,"You are the living embodiment of %s (%d).",
#endif
		v_name, tester);

		strcpy(v_string[v_nr], vir_desc);

		info[i++] = v_string[v_nr];
	}
#endif
	info[i++] = "";

	/* Racial powers... */
	///race(変身含む) class mutation 自己分析
	if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
			case MIMIC_DEMON:
			case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの地獄か火炎のブレスを吐くことができる。(%d MP)", 3 * plev, 10+plev/3);
#else
				sprintf(Dummy, "You can breathe nether or fire, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

				info[i++] = Dummy;
			break;
		case MIMIC_VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは敵から %d-%d HP の生命力を吸収できる。(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
		}
	}
	else
	{
	switch (p_ptr->prace)
	{
		/*
		case RACE_NIBELUNG:
		//case RACE_DWARF:
			if (plev > 4)
#ifdef JP
info[i++] = "あなたは罠とドアと階段を感知できる。(5 MP)";
#else
				info[i++] = "You can find traps, doors and stairs (cost 5).";
#endif

			break;
			*/
			/*
		case RACE_HOBBIT:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは食料を生成できる。(10 MP)";
#else
				info[i++] = "You can produce food (cost 10).";
#endif

			}
			break;
*/
/*
		case RACE_GNOME:
			if (plev > 4)
			{
#ifdef JP
sprintf(Dummy, "あなたは範囲 %d 以内にテレポートできる。(%d MP)",
#else
				sprintf(Dummy, "You can teleport, range %d (cost %d).",
#endif

				    (1 + plev), (5 + (plev / 5)));
				info[i++] = Dummy;
			}
			break;
*/
			/*
		case RACE_HALF_ORC:
			if (plev > 2)
#ifdef JP
info[i++] = "あなたは恐怖を除去できる。(5 MP)";
#else
				info[i++] = "You can remove fear (cost 5).";
#endif

			break;
			*/
			/*
			case RACE_HALF_TROLL:
			if (plev > 9)
#ifdef JP
info[i++] = "あなたは狂暴化することができる。(12 MP) ";
#else
				info[i++] = "You enter berserk fury (cost 12).";
#endif

			break;
			*/
			/*
		case RACE_AMBERITE:
			if (plev > 29)
#ifdef JP
info[i++] = "あなたはシャドウシフトすることができる。(50 MP)";
#else
				info[i++] = "You can Shift Shadows (cost 50).";
#endif

			if (plev > 39)
#ifdef JP
info[i++] = "あなたは「パターン」を心に描いて歩くことができる。(75 MP)";
#else
				info[i++] = "You can mentally Walk the Pattern (cost 75).";
#endif

			break;
			*/
			/*
		case RACE_BARBARIAN:
			if (plev > 7)
#ifdef JP
info[i++] = "あなたは狂暴化することができる。(10 MP) ";
#else
				info[i++] = "You can enter berserk fury (cost 10).";
#endif

			break;

		case RACE_HALF_OGRE:
			if (plev > 24)
#ifdef JP
info[i++] = "あなたは爆発のルーンを仕掛けることができる。(35 MP)";
#else
				info[i++] = "You can set an Explosive Rune (cost 35).";
#endif

			break;
		case RACE_HALF_GIANT:
			if (plev > 19)
#ifdef JP
info[i++] = "あなたは石の壁を壊すことができる。(10 MP)";
#else
				info[i++] = "You can break stone walls (cost 10).";
#endif

			break;
			*/
			/*
		case RACE_HALF_TITAN:
			if (plev > 34)
#ifdef JP
info[i++] = "あなたはモンスターをスキャンすることができる。(20 MP)";
#else
				info[i++] = "You can probe monsters (cost 20).";
#endif

			break;
		case RACE_CYCLOPS:
			if (plev > 19)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの岩石を投げることができる。(15 MP)",
#else
				sprintf(Dummy, "You can throw a boulder, dam. %d (cost 15).",
#endif

				    3 * plev);
				info[i++] = Dummy;
			}
			break;
			*/
/*
		case RACE_YEEK:
			if (plev > 14)
#ifdef JP
info[i++] = "あなたは恐怖を呼び起こす叫び声を発することができる。(15 MP)";
#else
				info[i++] = "You can make a terrifying scream (cost 15).";
#endif

			break;
*/
/*
			case RACE_KLACKON:
			if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの酸を吹きかけることができる。(9 MP)", plev);
#else
				sprintf(Dummy, "You can spit acid, dam. %d (cost 9).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_KOBOLD:
			if (plev > 11)
			{
				sprintf(Dummy,
#ifdef JP
    "あなたは %d ダメージの毒矢を投げることができる。(8 MP)", plev);
#else
				    "You can throw a dart of poison, dam. %d (cost 8).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_DARK_ELF:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのマジック・ミサイルの呪文を使える。(2 MP)",
#else
				sprintf(Dummy, "You can cast a Magic Missile, dam %d (cost 2).",
#endif

				    (3 + ((plev-1) / 5)));
				info[i++] = Dummy;
			}
			break;
*/
/*
			case RACE_DRACONIAN:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのブレスを吐くことができる。(%d MP)", 2 * plev, plev);
#else
			sprintf(Dummy, "You can breathe, dam. %d (cost %d).", 2 * plev, plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_MIND_FLAYER:
			if (plev > 14)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの精神攻撃をすることができる。(12 MP)", plev);
#else
				sprintf(Dummy, "You can mind blast your enemies, dam %d (cost 12).", plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_IMP:
			if (plev > 29)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのファイア・ボールの呪文を使える。(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Ball, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			else if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのファイア・ボルトの呪文を使える。(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Bolt, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_GOLEM:
			if (plev > 19)
#ifdef JP
info[i++] = "あなたは d20+30 ターンの間肌を石に変化させられる。(15 MP)";
#else
				info[i++] = "You can turn your skin to stone, dur d20+30 (cost 15).";
#endif

			break;
*/
/*
		case RACE_ZOMBIE:
		case RACE_SKELETON:
			if (plev > 29)
#ifdef JP
info[i++] = "あなたは失った生命力を回復することができる。(30 MP)";
#else
				info[i++] = "You can restore lost life forces (cost 30).";
#endif

			break;
*/
		case RACE_VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは敵から %d-%d HP の生命力を吸収できる。(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
			/*
		case RACE_SPECTRE:
			if (plev > 3)
			{
#ifdef JP
info[i++] = "あなたは泣き叫んで敵を恐怖させることができる。(3 MP)";
#else
				info[i++] = "You can wail to terrify your enemies (cost 3).";
#endif

			}
			break;
			*/
			/*
		case RACE_SPRITE:
			if (plev > 11)
			{
#ifdef JP
info[i++] = "あなたは敵を眠らせる魔法の粉を投げることができる。(12 MP)";
#else
				info[i++] = "You can throw magical dust which induces sleep (cost 12).";
#endif

			}
			break;
			*/
			/*
		case RACE_DEMON:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの地獄か火炎のブレスを吐くことができる。(%d MP)", 3 * plev, 10+plev/3);
#else
			sprintf(Dummy, "You can breathe nether, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

			info[i++] = Dummy;
			break;
		case RACE_KUTAR:
			if (plev > 19)
#ifdef JP
info[i++] = "あなたは d20+30 ターンの間横に伸びることができる。(15 MP)";
#else
				info[i++] = "You can expand horizontally, dur d20+30 (cost 15).";
#endif

			break;
		case RACE_ANDROID:
			if (plev < 10)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのレイガンを撃つことができる。(7 MP)", (plev + 1) / 2);
#else
				sprintf(Dummy, "You can fire a ray gun with damage %d (cost 7).", (plev+1)/2);
#endif
			else if (plev < 25)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのブラスターを撃つことができる。(13 MP)", plev);
#else
				sprintf(Dummy, "You can fire a blaster with damage %d (cost 13).", plev);
#endif
			else if (plev < 35)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのバズーカを撃つことができる。(26 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a bazooka with damage %d (cost 26).", plev * 2);
#endif
			else if (plev < 45)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのビームキャノンを撃つことができる。(40 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a beam cannon with damage %d (cost 40).", plev * 2);
#endif
			else
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのロケットを撃つことができる。(60 MP)", plev * 5);
#else
				sprintf(Dummy, "You can fire a rocket with damage %d (cost 60).", plev * 5);
#endif

			info[i++] = Dummy;
			break;
			*/
		default:
			break;
	}
	}
#if 0
	switch(p_ptr->pclass)
	{
		case CLASS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "あなたはランダムな方向に対して数回攻撃することができる。(75 MP)";
#else
				info[i++] = "You can attack some random directions at a time (cost 75).";
#endif
			}
			break;
		case CLASS_HIGH_MAGE:
			if (p_ptr->realm1 == REALM_HEX) break;
		case CLASS_MAGE:
		case CLASS_SORCERER:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはアイテムの魔力を吸収することができる。(1 MP)";
#else
				info[i++] = "You can absorb charges from an item (cost 1).";
#endif
			}
			break;
		case CLASS_PRIEST:
			if (is_good_realm(p_ptr->realm1))
			{
				if (plev > 34)
				{
#ifdef JP
info[i++] = "あなたは武器を祝福することができる。(70 MP)";
#else
					info[i++] = "You can bless a weapon (cost 70).";
#endif
				}
			}
			else
			{
				if (plev > 41)
				{
#ifdef JP
info[i++] = "あなたは周りのすべてのモンスターを攻撃することができる。(40 MP)";
#else
					info[i++] = "You can damages all monsters in sight (cost 40).";
#endif
				}
			}
			break;
		case CLASS_ROGUE:
			if (plev > 7)
			{
#ifdef JP
info[i++] = "あなたは攻撃して即座に逃げることができる。(12 MP)";
#else
				info[i++] = "You can hit a monster and teleport at a time (cost 12).";
#endif
			}
			break;
		case CLASS_RANGER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは怪物を調査することができる。(20 MP)";
#else
				info[i++] = "You can prove monsters (cost 20).";
#endif
			}
			break;
		case CLASS_PALADIN:
			if (is_good_realm(p_ptr->realm1))
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "あなたは聖なる槍を放つことができる。(30 MP)";
#else
					info[i++] = "You can fires a holy spear (cost 30).";
#endif
				}
			}
			else
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "あなたは生命力を減少させる槍を放つことができる。(30 MP)";
#else
					info[i++] = "You can fires a spear which drains vitality (cost 30).";
#endif
				}
			}
			break;
		case CLASS_WARRIOR_MAGE:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはＨＰをＭＰに変換することができる。(0 MP)";
#else
				info[i++] = "You can convert HP to SP (cost 0).";
#endif
#ifdef JP
info[i++] = "あなたはＭＰをＨＰに変換することができる。(0 MP)";
#else
				info[i++] = "You can convert SP to HP (cost 0).";
#endif
			}
			break;
		case CLASS_CHAOS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "あなたは周囲に怪物を惑わす光を発生させることができる。(50 MP)";
#else
				info[i++] = "You can radiate light which confuses nearby monsters (cost 50).";
#endif
			}
			break;
		case CLASS_MONK:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたは構えることができる。(0 MP)";
#else
				info[i++] = "You can assume a posture of special form (cost 0).";
#endif
			}
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは通常の2倍の攻撃を行うことができる。(30 MP)";
#else
				info[i++] = "You can perform double attacks in a time (cost 30).";
#endif
			}
			break;
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは精神を集中してＭＰを回復させることができる。(0 MP)";
#else
				info[i++] = "You can concentrate to regenerate your mana (cost 0).";
#endif
			}
			break;
		case CLASS_TOURIST:
#ifdef JP
info[i++] = "あなたは写真を撮影することができる。(0 MP)";
#else
				info[i++] = "You can take a photograph (cost 0).";
#endif
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはアイテムを完全に鑑定することができる。(20 MP)";
#else
				info[i++] = "You can *identify* items (cost 20).";
#endif
			}
			break;
		case CLASS_IMITATOR:
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは怪物の特殊攻撃をダメージ2倍でまねることができる。(100 MP)";
#else
				info[i++] = "You can imitate monster's special attacks with double damage (cost 100).";
#endif
			}
			break;
		case CLASS_BEASTMASTER:
#ifdef JP
info[i++] = "あなたは1体の生命のあるモンスターを支配することができる。(レベル/4 MP)";
#else
			info[i++] = "You can dominate a monster (cost level/4).";
#endif
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは視界内の生命のあるモンスターを支配することができる。((レベル+20)/2 MP)";
#else
				info[i++] = "You can dominate living monsters in sight (cost (level+20)/4).";
#endif
			}
			break;
		case CLASS_MAGIC_EATER:
#ifdef JP
info[i++] = "あなたは杖/魔法棒/ロッドの魔力を自分のものにすることができる。";
#else
			info[i++] = "You can absorb a staff, wand or rod itself.";
#endif
			break;
		case CLASS_RED_MAGE:
			if (plev > 47)
			{
#ifdef JP
info[i++] = "あなたは1ターンに2回魔法を唱えることができる。(20 MP)";
#else
				info[i++] = "You can cast two spells in one time (cost 20).";
#endif
			}
			break;
		case CLASS_SAMURAI:
			{
#ifdef JP
info[i++] = "あなたは精神を集中して気合いを溜めることができる。";
#else
				info[i++] = "You can concentrate to regenerate your mana.";
#endif
			}
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたは特殊な型で構えることができる。";
#else
				info[i++] = "You can assume a posture of special form.";
#endif
			}
			break;
		case CLASS_BLUE_MAGE:
#ifdef JP
info[i++] = "あなたは相手に使われた魔法を学ぶことができる。";
#else
			info[i++] = "You can study spells which your enemy casts on you.";
#endif
			break;
		case CLASS_CAVALRY:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "あなたはモンスターに乗って無理矢理ペットにすることができる。";
#else
				info[i++] = "You can ride on a hostile monster forcibly to turn it into pet.";
#endif
			}
			break;
		case CLASS_BERSERKER:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "あなたは街とダンジョンの間を行き来することができる。";
#else
			info[i++] = "You can travel between town and the depths.";
#endif
			}
			break;
		case CLASS_MIRROR_MASTER:
#ifdef JP
info[i++] = "あなたは鏡を作り出すことができる。(2 MP)";
#else
				info[i++] = "You can create a Mirror (cost 2).";
#endif
#ifdef JP
info[i++] = "あなたは鏡を割ることができる。(0 MP)";
#else
				info[i++] = "You can break distant Mirrors (cost 0).";
#endif
			break;
		case CLASS_NINJA:
			if (plev > 19)
			{
#ifdef JP
info[i++] = "あなたは素早く移動することができる。";
#else
				info[i++] = "You can walk extremery fast.";
#endif
			}
			break;
	}
#endif

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_BR_ACID)
		{
#ifdef JP
info[i++] = "あなたは酸を吹きかけることができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can breathe acid (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
info[i++] = "あなたは炎のブレスを吐くことができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can breathe fire (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
info[i++] = "あなたの睨みは催眠効果をもつ。";
#else
			info[i++] = "Your gaze is hypnotic.";
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
info[i++] = "あなたは念動力をもっている。";
#else
			info[i++] = "You are telekinetic.";
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
info[i++] = "あなたは自分の意思でテレポートできる。";
#else
			info[i++] = "You can teleport at will.";
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
info[i++] = "あなたは精神攻撃を行える。(ダメージ 3～12d3)";
#else
			info[i++] = "You can Mind Blast your enemies (3 to 12d3 dam).";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_ELEC)
		{
#ifdef JP
info[i++] = "あなたは雷のブレスを吐くことができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can breathe lightning (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
info[i++] = "あなたは吸血鬼のように敵から生命力を吸収することができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can drain life from a foe like a vampire (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_ACCELERATION)
		{
#ifdef JP
info[i++] = "あなたは一時的に加速することができる。";
#else
			info[i++] = "You can temporarily accelerate.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BEAST_EAR)
		{
#ifdef JP
info[i++] = "あなたは鋭敏な聴覚で近くのモンスターを感知できる。";
#else
			info[i++] = "Your keen hearing lets you detect nearby monsters.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
info[i++] = "あなたは短い距離をテレポートできる。";
#else
			info[i++] = "You can teleport yourself short distances.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
info[i++] = "あなたは硬い岩を食べることができる。";
#else
			info[i++] = "You can consume solid rock.";
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
info[i++] = "あなたは他の者と場所を入れ替わることができる。";
#else
			info[i++] = "You can switch locations with another being.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_SOUND)
		{
#ifdef JP
info[i++] = "あなたは大声で敵を攻撃できる。(ダメージ レベルX2)";
#else
			info[i++] = "You can attack enemies with a loud shout (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_COLD)
		{
#ifdef JP
info[i++] = "あなたは吹雪を吐くことができる。";
#else
			info[i++] = "You can breathe frost (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_ITEM)
		{
#ifdef JP
info[i++] = "あなたは近くに落ちているアイテムを感知することができる。";
#else
			info[i++] = "You can feel the presence of nearby items.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
info[i++] = "あなたは自分の意思で狂乱戦闘状態になることができる。";
#else
			info[i++] = "You can drive yourself into a berserk frenzy.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_POIS)
		{
#ifdef JP
info[i++] = "あなたはガスのブレスを吐くことができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can breathe poison (dam lvl * 2)";
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
info[i++] = "あなたは通常アイテムを金に変えることができる。";
#else
			info[i++] = "You can turn ordinary items to gold.";
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
info[i++] = "あなたは周囲にキノコを生やすことができる。";
#else
			info[i++] = "You can cause mushrooms to grow near you.";
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
info[i++] = "あなたは元素の攻撃に対して身を硬くすることができる。";
#else
			info[i++] = "You can harden yourself to the ravages of the elements.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
info[i++] = "あなたは周囲のダンジョンを崩壊させることができる。";
#else
			info[i++] = "You can bring down the dungeon around your ears.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
info[i++] = "あなたは魔法のエネルギーを自分の物として使用できる。";
#else
			info[i++] = "You can consume magic energy for your own use.";
#endif

		}
		if (p_ptr->muta1 & MUT1_RAISE_WOOD)
		{
#ifdef JP
info[i++] = "あなたは自分のいる所に森林を生成することができる。";
#else
			info[i++] = "You can create forest at the place you're standing.";
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
info[i++] = "あなたは今いるフロアでモンスターの増殖を阻止することができる。";
#else
			info[i++] = "You can prevent monsters on your floor from multiplying.";
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
info[i++] = "あなたは攻撃した後身を守るため逃げることができる。";
#else
			info[i++] = "You can run for your life after hitting something.";
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
info[i++] = "あなたの眼は狂気をもたらす。 ";
#else
			info[i++] = "Your eyes can cause insanity.";
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
info[i++] = "あなたは目からビームを撃つことができる。(ダメージ レベルX3)";
#else
			info[i++] = "Your eyes can fire laser beams (dam 3 * lvl).";
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
info[i++] = "あなたは街とダンジョンの間を行き来することができる。";
#else
			info[i++] = "You can travel between town and the depths.";
#endif

		}
		if (p_ptr->muta1 & MUT1_ID_MUSHROOM)
		{
#ifdef JP
info[i++] = "あなたはキノコの鑑定ができる。";
#else
			info[i++] = "You can identify mushrooms.";
#endif

		}
		if (p_ptr->muta1 & MUT1_DANMAKU)
		{
#ifdef JP
info[i++] = "あなたは強力な弾幕を放つことができる。";
#else
			info[i++] = "You can fire powerful danmaku.";
#endif

		}
		/*
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
info[i++] = "あなたはアイテムを力強く投げることができる。";
#else
			info[i++] = "You can hurl objects with great force.";
#endif

		}
		*/
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
info[i++] = "あなたは狂戦士化の発作を起こす。";
#else
			info[i++] = "You are subject to berserker fits.";
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
info[i++] = "あなたは時々恐慌に襲われる。";
#else
			info[i++] = "You are subject to cowardice.";
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
info[i++] = "あなたは時々意識が曖昧になる。";
#else
			info[i++] = "You are teleporting randomly.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
info[i++] = "あなたはアルコール中毒だ。";
#else
			info[i++] = "You are addicted to alcohol.";
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
info[i++] = "あなたは幻覚を引き起こす精神錯乱に侵されている。";
#else
			info[i++] = "You have a hallucinatory insanity.";
#endif

		}
		if (p_ptr->muta2 & MUT2_YAKU)
		{
#ifdef JP
info[i++] = "あなたの周りには厄が集まってくる。";
#else
			info[i++] = "You are gathering nearby misfortune.";
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
info[i++] = "あなたは制御不能な魔法のエネルギーを発している。";
#else
			info[i++] = "You are producing magical energy uncontrollably.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
info[i++] = "あなたはデーモンを引きつける。";
#else
			info[i++] = "You attract demons.";
#endif

		}
		if (p_ptr->muta2 & MUT2_HARDHEAD)
		{
#ifdef JP
info[i++] = "あなたは石頭だ。(知能-1、賢さ+1、AC+5)";
#else
			info[i++] = "You are hardheaded. (INT -1, WIS +1 AC +5)";
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
info[i++] = "あなたは角が生えている。";
#else
			info[i++] = "You have horns.";
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGHORN)
		{
#ifdef JP
info[i++] = "あなたは立派な角を持っている。";
#else
			info[i++] = "You have splendid horns.";
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
info[i++] = "あなたはランダムに早く動いたり遅く動いたりする。";
#else
			info[i++] = "You move faster or slower randomly.";
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
info[i++] = "あなたは時々近くのモンスターを消滅させる。";
#else
			info[i++] = "You sometimes cause nearby creatures to vanish.";
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
info[i++] = "あなたは時々周囲の光を吸収して栄養にする。";
#else
			info[i++] = "You sometimes feed off of the light around you.";
#endif

		}
		if (p_ptr->muta2 & MUT2_TAIL)
		{
#ifdef JP
info[i++] = "あなたはしなやかな尻尾を持っている。(器用+4)";
#else
			info[i++] = "You have a prehensile tail. (DEX +4)";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
info[i++] = "あなたは動物を引きつける。";
#else
			info[i++] = "You attract animals.";
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGTAIL)
		{
#ifdef JP
info[i++] = "あなたは大きな尻尾を持っている。(ダメージ 2d5)";
#else
			info[i++] = "You have large tail.";
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
info[i++] = "あなたはしばしば純カオスに包まれる。";
#else
			info[i++] = "You occasionally are surrounded with raw chaos.";
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
info[i++] = "あなたはときどき変異から回復する。";
#else
			info[i++] = "You may be mutated, but you're recovering.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
info[i++] = "あなたの肉体は幽体化したり実体化したりする。";
#else
			info[i++] = "You fade in and out of physical reality.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_GHOST)
		{
#ifdef JP
info[i++] = "あなたは幽霊を引きつける。";
#else
			info[i++] = "Your attract ghosts.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_KWAI)
		{
#ifdef JP
info[i++] = "あなたは妖怪を引きつける。";
#else
			info[i++] = "You attract youkai.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
info[i++] = "あなたはドラゴンを引きつける。";
#else
			info[i++] = "You attract dragons.";
#endif

		}
		if (p_ptr->muta2 & MUT2_XOM)
		{
#ifdef JP
if(p_ptr->lev > 24) info[i++] = "あなたはゾムのお気に入りの玩具だ。";
else info[i++] = "あなたはゾムの玩具だ。";
#else
			if(p_ptr->lev > 24) info[i++] = " You are Xom's favorite plaything.";
            else info[i++] = " You are Xom's plaything.";
#endif

		}
		if (p_ptr->muta2 & MUT2_BIRDBRAIN)
		{
#ifdef JP
info[i++] = "あなたはすぐに物事を忘れる。";
#else
			info[i++] = "You are quick to forget things.";
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
info[i++] = "あなたはカオスの守護悪魔から褒美をうけとる。";
#else
			info[i++] = "You receive gifts from a Chaos patron.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
info[i++] = "あなたはしばしば他の「影」に迷い込む。";
#else
			info[i++] = "You occasionally stumble into other shadows.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
info[i++] = "あなたは敵に関する警告を感じる。";
#else
			info[i++] = "You receive warnings about your foes.";
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
info[i++] = "あなたは時々負け知らずな気分になる。";
#else
			info[i++] = "You occasionally feel invincible.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ASTHMA)
		{
#ifdef JP
info[i++] = "あなたは喘息持ちだ。";
#else
			info[i++] = "You are asthmatic.";
#endif

		}
		if (p_ptr->muta2 & MUT2_THE_WORLD)
		{
#ifdef JP
info[i++] = "あなたの周りで時々時間が停止する。";
#else
			info[i++] = "Time occasionally stops in your vicinity.";
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
info[i++] = "あなたはよくつまづいて物を落とす。";
#else
			info[i++] = "You occasionally stumble and drop things.";
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
info[i++] = "あなたは超人的に強い。(腕力+4)";
#else
			info[i++] = "You are superhumanly strong (+4 STR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
info[i++] = "あなたは虚弱だ。(腕力-4)";
#else
			info[i++] = "You are puny (-4 STR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
info[i++] = "あなたの脳は生体コンピュータだ。(知能＆賢さ+4)";
#else
			info[i++] = "Your brain is a living computer (+4 INT/WIS).";
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
info[i++] = "あなたは精神薄弱だ。(知能＆賢さ-4)";
#else
			info[i++] = "You are moronic (-4 INT/WIS).";
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
info[i++] = "あなたは非常にタフだ。(耐久+4)";
#else
			info[i++] = "You are very resilient (+4 CON).";
#endif

		}
		if (p_ptr->muta3 & MUT3_WARNING)
		{
#ifdef JP
info[i++] = "あなたは勘が鋭く身に迫った危険を察知できる。";
#else
			info[i++] = "Your sharp intuition warns you of danger.";
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
info[i++] = "あなたは虚弱体質だ。(耐久-4)";
#else
			info[i++] = "You are frail (-4 CON).";
#endif

		}
		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
#ifdef JP
info[i++] = "あなたは強靭な脚力をもっている。";
#else
			info[i++] = "You have strong legs (grants swift movement).";
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
info[i++] = "あなたはのっぺらぼうだ。(魅力-3)";
#else
			info[i++] = "Your face is featureless (-3 CHR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_HURT_LITE)
		{
#ifdef JP
info[i++] = "あなたは光が苦手だ。";
#else
			info[i++] = "You are hurt by bright light.";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				info[i++] = "あなたは額に第四の目を持っている。";
			else

				info[i++] = "あなたは額に第三の目を持っている。";
#else
			if (p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				info[i++] = " You have a fourth eye on your forehead.";
			else
				info[i++] = " You have a third eye on your forehead.";
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
info[i++] = "あなたは魔法への耐性をもっている。";
#else
			info[i++] = "You are resistant to magic.";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
info[i++] = "あなたは変な音を発している。(隠密-3)";
#else
			info[i++] = "You make a lot of strange noise (-3 stealth).";
#endif

		}
		if (p_ptr->muta3 & MUT3_BYDO)
		{
#ifdef JP
info[i++] = "あなたはバイド細胞に浸食されている。(全能力上昇、魅力大幅減少、反感)";
#else
			info[i++] = "You are infected by Bydo cells. (+all stats, dramatically reduces CHR, aggravate)";
#endif

		}
		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
#ifdef JP
info[i++] = "あなたの下半身は人魚の尻尾だ。";
#else
			info[i++] = "You have a merfolk tail in place of your lower body.";
#endif

		}
		if (p_ptr->muta3 & MUT3_PEGASUS)
		{
#ifdef JP
info[i++] = "あなたの頭は昇天ペガサスMIX盛りだ。(AC+5 頭防具を装備できない)";
#else
			info[i++] = "You have the Ascension Pegasus MIX Prime hairstyle. (AC +5, cannot wear headgear";
#endif

		}
		if (p_ptr->muta3 & MUT3_ACID_SCALES)
		{
#ifdef JP
info[i++] = "あなたの肌は黒い鱗に覆われている。(AC+5 酸耐性 魅力-2)";
#else
			info[i++] = "Your skin is covered in black scales. (AC +5 Resist acid CHR -2)";
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_SCALES)
		{
#ifdef JP
info[i++] = "あなたの肌は青い鱗に覆われている。(AC+5 電耐性 魅力-2)";
#else
			info[i++] = "Your skin is covered in blue scales. (AC +5 Resist lightning CHR -2)";
#endif
		}
		if (p_ptr->muta3 & MUT3_FIRE_SCALES)
		{
#ifdef JP
info[i++] = "あなたの肌は赤い鱗に覆われている。(AC+5 火耐性 魅力-2)";
#else
			info[i++] = "Your skin is covered in red scales. (AC +5 Resist fire CHR -2)";
#endif

		}
		if (p_ptr->muta3 & MUT3_COLD_SCALES)
		{
#ifdef JP
info[i++] = "あなたの肌は白い鱗に覆われている。(AC+5 冷耐性 魅力-2)";
#else
			info[i++] = "Your skin is covered in white scales. (AC +5 Resist cold CHR -2)";
#endif

		}
		if (p_ptr->muta3 & MUT3_POIS_SCALES)
		{
#ifdef JP
info[i++] = "あなたの肌は緑の鱗に覆われている。(AC+5 毒耐性 魅力-2)";
#else
			info[i++] = "Your skin is covered in green scales. (AC +5 Resist poison CHR -2)";
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
info[i++] = "あなたは羽を持っている。";
#else
			info[i++] = "You have wings.";
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			/* Unnecessary */
		}
		if (p_ptr->muta3 & MUT3_FEVER)
		{
#ifdef JP
info[i++] = "あなたは原因不明の熱病に冒されている。(知能器用-2)";
#else
            info[i++] = "You are struck with unidentified fever. (INT/DEX -2)";
#endif

		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
			/* Unnecessary */
		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
info[i++] = "あなたの体は非常にしなやかだ。(器用+3)";
#else
			info[i++] = "Your body is very limber (+3 DEX).";
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
info[i++] = "あなたはいつも関節に痛みを感じている。(器用-3)";
#else
			info[i++] = "Your joints ache constantly (-3 DEX).";
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
info[i++] = "あなたは元素の攻撃に弱い。";
#else
			info[i++] = "You are susceptible to damage from the elements.";
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
info[i++] = "あなたの動作は正確で力強い。(隠密+1)";
#else
			info[i++] = "Your movements are precise and forceful (+1 STL).";
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
info[i++] = "あなたは白いオーラにつつまれている。";
#else
			info[i++] = "There is a white aura surrounding you.";
#endif
		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
info[i++] = "あなたは黒いオーラにつつまれている。";
#else
			info[i++] = "There is a black aura surrounding you.";
#endif
		}
	}
	if (p_ptr->muta4)
	{
		//v1.1.64
		if (p_ptr->muta4 & MUT4_GHOST_WOLF)
		{
			info[i++] = _(" あなたはオオカミの動物霊に憑依されている。(肉体強化/知能賢さ低下)",
                            " You are possessed by a wolf animal spirit. (Phys+ / INT, WIS-)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_OTTER)
		{
			info[i++] = _(" あなたはカワウソの動物霊に憑依されている。(技能上昇/全能力やや低下)",
                            " You are possessed by an otter animal spirit. (Skills+ / All stats a bit-)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
		{
			info[i++] = _(" あなたはオオワシの動物霊に憑依されている。(知能賢さ探索上昇/腕力耐久低下)",
                            " You are possessed by an eagle animal spirit. (INT, WIS, Srh+ / STR, CON-)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH)
		{
			info[i++] = _(" あなたはクラゲの動物霊に憑依されている。(毒麻痺水耐性/腕力大幅低下)",
                            " You are possessed by a jellyfish animal spirit. (Res pois/wtr, FA / STR--)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_COW)
		{
			info[i++] = _(" あなたはウシの動物霊に憑依されている。(腕力耐久大幅上昇/隠密大幅低下)",
                            " You are possessed by a cow animal spirit. (STR, CON++ / Stealth--)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_CHICKEN)
		{
			info[i++] = _(" あなたはヒヨコの動物霊に憑依されている。(魅力大幅上昇/AC低下)",
                             " You are possessed by a chicken animal spirit. (CHR++ / AC-)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_TORTOISE)
		{
			info[i++] = _(" あなたはカメの動物霊に憑依されている。(AC大幅上昇/加速低下)",
                                " You are possessed by a tortoise animal spirit. (AC++ / Speed-)");
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
		{
			info[i++] = _(" あなたはハニワの動物…？霊…？に憑依されている。(魔法防御大幅上昇/轟音破片弱点25%)",
                                " You are possessed by a haniwa animal...? spirit...? (MR++ / Snd,shard vul 25%%)");
		}

	}

	if (p_ptr->blind)
	{
#ifdef JP
info[i++] = "あなたは目が見えない。";
#else
		info[i++] = "You cannot see.";
#endif

	}
	if (p_ptr->confused)
	{
#ifdef JP
info[i++] = "あなたは混乱している。";
#else
		info[i++] = "You are confused.";
#endif

	}
	if (p_ptr->afraid)
	{
#ifdef JP
info[i++] = "あなたは恐怖に侵されている。";
#else
		info[i++] = "You are terrified.";
#endif

	}
	if (p_ptr->cut)
	{
#ifdef JP
info[i++] = "あなたは出血している。";
#else
		info[i++] = "You are bleeding.";
#endif

	}
	if (p_ptr->stun)
	{
#ifdef JP
info[i++] = "あなたはもうろうとしている。";
#else
		info[i++] = "You are stunned.";
#endif

	}
	if (p_ptr->poisoned)
	{
#ifdef JP
info[i++] = "あなたは毒に侵されている。";
#else
		info[i++] = "You are poisoned.";
#endif

	}
	if (p_ptr->image)
	{
#ifdef JP
info[i++] = "あなたは幻覚を見ている。";
#else
		info[i++] = "You are hallucinating.";
#endif

	}
	if (p_ptr->cursed & TRC_TY_CURSE)
	{
#ifdef JP
info[i++] = "あなたは邪悪な怨念に包まれている。";
#else
		info[i++] = "You carry an ancient foul curse.";
#endif

	}
	if (p_ptr->cursed & TRC_AGGRAVATE)
	{
#ifdef JP
info[i++] = "あなたはモンスターを怒らせている。";
#else
		info[i++] = "You aggravate monsters.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_EXP)
	{
#ifdef JP
info[i++] = "あなたは経験値を吸われている。";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (p_ptr->cursed & TRC_SLOW_REGEN)
	{
#ifdef JP
info[i++] = "あなたの回復力は非常に遅い。";
#else
		info[i++] = "You regenerate slowly.";
#endif

	}
	if (p_ptr->cursed & TRC_ADD_L_CURSE)
	{
#ifdef JP
info[i++] = "あなたの弱い呪いは増える。"; /* 暫定的 -- henkma */
#else
		info[i++] = "Your weak curses multiply.";
#endif

	}
	if (p_ptr->cursed & TRC_ADD_H_CURSE)
	{
#ifdef JP
info[i++] = "あなたの強い呪いは増える。"; /* 暫定的 -- henkma */
#else
		info[i++] = "Your heavy curses multiply.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_ANIMAL)
	{
#ifdef JP
info[i++] = "あなたは動物に狙われている。";
#else
		info[i++] = "You attract animals.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_DEMON)
	{
#ifdef JP
info[i++] = "あなたは悪魔に狙われている。";
#else
		info[i++] = "You attract demons.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_DRAGON)
	{
#ifdef JP
info[i++] = "あなたはドラゴンに狙われている。";
#else
		info[i++] = "You attract dragons.";
#endif

	}
	if (p_ptr->cursed & TRC_COWARDICE)
	{
#ifdef JP
info[i++] = "あなたは時々臆病になる。";
#else
		info[i++] = "You are subject to cowardice.";
#endif

	}
	if (p_ptr->cursed & TRC_TELEPORT)
	{
#ifdef JP
info[i++] = "あなたの位置はひじょうに不安定だ。";
#else
		info[i++] = "Your position is very uncertain.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_MELEE)
	{
#ifdef JP
info[i++] = "あなたの武器は攻撃を外しやすい。";
#else
		info[i++] = "Your weapon causes you to miss blows.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_AC)
	{
#ifdef JP
info[i++] = "あなたは攻撃を受けやすい。";
#else
		info[i++] = "You are subject to be hit.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_MAGIC)
	{
#ifdef JP
info[i++] = "あなたは魔法を失敗しやすい。";
#else
		info[i++] = "You are subject to fail spellcasting.";
#endif

	}
	if (p_ptr->cursed & TRC_FAST_DIGEST)
	{
#ifdef JP
info[i++] = "あなたはすぐお腹がへる。";
#else
		info[i++] = "You have a good appetite.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_HP)
	{
#ifdef JP
info[i++] = "あなたは体力を吸われている。";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_MANA)
	{
#ifdef JP
info[i++] = "あなたは魔力を吸われている。";
#else
		info[i++] = "You brain is drained.";
#endif

	}
	if (IS_BLESSED())
	{
#ifdef JP
info[i++] = "あなたは何か大いなるものの祝福を受けている。";
#else
		info[i++] = "You feel rightous.";
#endif

	}
	if (IS_HERO())
	{
#ifdef JP
info[i++] = "あなたはヒーロー気分だ。";
#else
		info[i++] = "You feel heroic.";
#endif

	}
	if (p_ptr->shero)
	{
#ifdef JP
info[i++] = "あなたは戦闘狂だ。";
#else
		info[i++] = "You are in a battle rage.";
#endif

	}
	if (p_ptr->protevil)
	{
#ifdef JP
info[i++] = "あなたは邪悪なる存在から守られている。";
#else
		info[i++] = "You are protected from evil.";
#endif

	}
	if (p_ptr->shield)
	{
#ifdef JP
info[i++] = "あなたは神秘のシールドで守られている。";
#else
		info[i++] = "You are protected by a mystic shield.";
#endif

	}
	if (IS_INVULN())
	{
#ifdef JP
info[i++] = "あなたは現在傷つかない。";
#else
		info[i++] = "You are temporarily invulnerable.";
#endif

	}
	if (p_ptr->wraith_form)
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_RUMIA)
			info[i++] = "あなたは一時的に闇に溶けている。";
		else
			info[i++] = "あなたは一時的に幽体化している。";


#else
		if(p_ptr->pclass == CLASS_RUMIA)
			info[i++] = "You have temporarily melted into darkness.";
		else
			info[i++] = "You are temporarily incorporeal.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
#ifdef JP
info[i++] = "あなたの手は赤く輝いている。";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_DARK)
	{
#ifdef JP
info[i++] = "あなたの手は妖しく光っている。";
#else
		info[i++] = "You hands are glowing in a bewitching light.";
#endif

	}


	if (p_ptr->special_attack & ATTACK_FIRE)
	{
#ifdef JP
info[i++] = "あなたの手は火炎に覆われている。";
#else
		info[i++] = "You can strike the enemy with flame.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_COLD)
	{
#ifdef JP
info[i++] = "あなたの手は冷気に覆われている。";
#else
		info[i++] = "You can strike the enemy with cold.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_ACID)
	{
#ifdef JP
info[i++] = "あなたの手は酸に覆われている。";
#else
		info[i++] = "You can strike the enemy with acid.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_ELEC)
	{
#ifdef JP
info[i++] = "あなたの手は電撃に覆われている。";
#else
		info[i++] = "You can strike the enemy with electric shock.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_POIS)
	{
#ifdef JP
info[i++] = "あなたの手は毒に覆われている。";
#else
		info[i++] = "You can strike the enemy with poison.";
#endif

	}
	switch (p_ptr->action)
	{
		case ACTION_SEARCH:
#ifdef JP
info[i++] = "あなたはひじょうに注意深く周囲を見渡している。";
#else
			info[i++] = "You are looking around very carefully.";
#endif
			break;
	}
	if (p_ptr->new_spells)
	{
#ifdef JP
info[i++] = "あなたは呪文や祈りを学ぶことができる。";
#else
		info[i++] = "You can learn some spells/prayers.";
#endif

	}
	if (p_ptr->word_recall)
	{
#ifdef JP
info[i++] = "あなたはすぐに帰還するだろう。";
#else
		info[i++] = "You will soon be recalled.";
#endif

	}
	if (p_ptr->alter_reality)
	{
#ifdef JP
		info[i++] = "あなたはすぐにこの世界を離れるだろう。";
#else
		info[i++] = "You will soon be altered.";
#endif

	}
	if (p_ptr->see_infra)
	{
#ifdef JP
info[i++] = "あなたの瞳は赤外線に敏感である。";
#else
		info[i++] = "Your eyes are sensitive to infrared light.";
#endif

	}
	if (p_ptr->see_inv)
	{
#ifdef JP
info[i++] = "あなたは透明なモンスターを見ることができる。";
#else
		info[i++] = "You can see invisible creatures.";
#endif

	}
	if (p_ptr->levitation)
	{
#ifdef JP
info[i++] = "あなたは飛ぶことができる。";
#else
		info[i++] = "You can fly.";
#endif

	}
	if (p_ptr->free_act)
	{
#ifdef JP
info[i++] = "あなたは麻痺知らずの効果を持っている。";
#else
		info[i++] = "You have free action.";
#endif

	}
		if (p_ptr->resist_fear)
	{
#ifdef JP
	if(IS_VULN_FEAR)
		info[i++] = "あなたはあまり恐怖を感じない。";
	else
		info[i++] = "あなたは全く恐怖を感じない。";
#else
    if(IS_VULN_FEAR)
		info[i++] = "You don't feel too much fear.";
	else
		info[i++] = "You are completely fearless.";
#endif

	}
	if (p_ptr->resist_blind)
	{
#ifdef JP
info[i++] = "あなたの目は盲目への耐性を持っている。";
#else
		info[i++] = "Your eyes are resistant to blindness.";
#endif

	}

	if (p_ptr->resist_conf)
	{
#ifdef JP
info[i++] = "あなたは混乱への耐性を持っている。";
#else
		info[i++] = "You are resistant to confusion.";
#endif
	}
	if (p_ptr->resist_insanity)
	{
#ifdef JP
info[i++] = "あなたは名状しがたき宇宙的恐怖への耐性を持っている。";
#else
		info[i++] = "You are resistant to unspeakable cosmic horrors.";
#endif
	}
	if (p_ptr->regenerate)
	{
#ifdef JP
info[i++] = "あなたは素早く体力を回復する。";
#else
		info[i++] = "You regenerate quickly.";
#endif

	}
	if (p_ptr->slow_digest)
	{
#ifdef JP
info[i++] = "あなたは食欲が少ない。";
#else
		info[i++] = "Your appetite is small.";
#endif

	}
	if (p_ptr->telepathy)
	{
#ifdef JP
info[i++] = "あなたはテレパシー能力を持っている。";
#else
		info[i++] = "You have ESP.";
#endif

	}
	if (p_ptr->esp_animal)
	{
#ifdef JP
info[i++] = "あなたは自然界の生物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense natural creatures.";
#endif

	}
	if (p_ptr->esp_undead)
	{
#ifdef JP
info[i++] = "あなたはアンデッドの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense undead.";
#endif

	}
	if (p_ptr->esp_demon)
	{
#ifdef JP
info[i++] = "あなたは悪魔の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense demons.";
#endif

	}

	if (p_ptr->esp_kwai)
	{
#ifdef JP
info[i++] = "あなたは妖怪の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense youkai.";
#endif

	}
	if (p_ptr->esp_deity)
	{
#ifdef JP
info[i++] = "あなたは神格の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense deities.";
#endif

	}
	if (p_ptr->esp_dragon)
	{
#ifdef JP
info[i++] = "あなたはドラゴンの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense dragons.";
#endif

	}
	if (p_ptr->esp_human)
	{
#ifdef JP
info[i++] = "あなたは人間の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense humans.";
#endif

	}
	if (p_ptr->esp_evil)
	{
#ifdef JP
info[i++] = "あなたは邪悪な生き物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense evil creatures.";
#endif

	}
	if (p_ptr->esp_good)
	{
#ifdef JP
info[i++] = "あなたは善良な生き物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense good creatures.";
#endif

	}
	if (p_ptr->esp_nonliving)
	{
#ifdef JP
info[i++] = "あなたは活動する無生物体の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense non-living creatures.";
#endif

	}
	if (p_ptr->esp_unique)
	{
#ifdef JP
info[i++] = "あなたは特別な強敵の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense unique monsters.";
#endif

	}
///mod131228 生命力維持削除
/*
	if (p_ptr->hold_life)
	{
#ifdef JP
info[i++] = "あなたは自己の生命力をしっかりと維持する。";
#else
		info[i++] = "You have a firm hold on your life force.";
#endif

	}
*/
	if (p_ptr->reflect)
	{
#ifdef JP
info[i++] = "あなたは矢やボルトを反射する。";
#else
		info[i++] = "You reflect arrows and bolts.";
#endif

	}
	if (p_ptr->sh_fire)
	{
#ifdef JP
info[i++] = "あなたは炎のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a fiery aura.";
#endif

	}
	if (p_ptr->sh_elec)
	{
#ifdef JP
info[i++] = "あなたは電気に包まれている。";
#else
		info[i++] = "You are surrounded with electricity.";
#endif

	}
	if (p_ptr->sh_cold)
	{
#ifdef JP
info[i++] = "あなたは冷気のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with an aura of coldness.";
#endif

	}
	if (p_ptr->tim_sh_holy)
	{
#ifdef JP
info[i++] = "あなたは聖なるオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a holy aura.";
#endif

	}
	if (p_ptr->tim_sh_touki)
	{
#ifdef JP
info[i++] = "あなたは闘気のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a energy aura.";
#endif

	}
	if (p_ptr->anti_magic)
	{
#ifdef JP
info[i++] = "あなたは反魔法シールドに包まれている。";
#else
		info[i++] = "You are surrounded by an anti-magic shell.";
#endif

	}
	if (p_ptr->anti_tele)
	{
#ifdef JP
info[i++] = "あなたはテレポートできない。";
#else
		info[i++] = "You cannot teleport.";
#endif

	}
	if (p_ptr->lite)
	{
#ifdef JP
info[i++] = "あなたの身体は光っている。";
#else
		info[i++] = "You are carrying a permanent light.";
#endif

	}
	if (p_ptr->warning)
	{
#ifdef JP
info[i++] = "あなたは行動の前に危険を察知することができる。";
#else
		info[i++] = "You will be warned before dangerous actions.";
#endif

	}
	if (p_ptr->dec_mana)
	{
#ifdef JP
info[i++] = "あなたは少ない消費魔力で魔法を唱えることができる。";
#else
		info[i++] = "You can cast spells with fewer mana points.";
#endif

	}
	if (p_ptr->easy_spell)
	{
#ifdef JP
info[i++] = "あなたは低い失敗率で魔法を唱えることができる。";
#else
		info[i++] = "Fail rate of your magic is decreased.";
#endif

	}
	if (p_ptr->heavy_spell)
	{
#ifdef JP
info[i++] = "あなたは高い失敗率で魔法を唱えなければいけない。";
#else
		info[i++] = "Fail rate of your magic is increased.";
#endif

	}
	if (p_ptr->mighty_throw)
	{
#ifdef JP
info[i++] = "あなたは強く物を投げる。";
#else
		info[i++] = "You can throw objects powerfully.";
#endif

	}

	if (p_ptr->speedster)
	{
#ifdef JP
info[i++] = "あなたは素早く移動することができる。";
#else
		info[i++] = "You can move swiftly.";
#endif

	}

	if (p_ptr->immune_acid)
	{
#ifdef JP
info[i++] = "あなたは酸に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to acid.";
#endif

	}
	else if (p_ptr->resist_acid && IS_OPPOSE_ACID())
	{
#ifdef JP
info[i++] = "あなたは酸への強力な耐性を持っている。";
#else
		info[i++] = "You resist acid exceptionally well.";
#endif

	}
	else if (p_ptr->resist_acid || IS_OPPOSE_ACID())
	{
#ifdef JP
info[i++] = "あなたは酸への耐性を持っている。";
#else
		info[i++] = "You are resistant to acid.";
#endif

	}

	if (p_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "あなたは電撃に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to lightning.";
#endif

	}
	else if (p_ptr->resist_elec && IS_OPPOSE_ELEC())
	{
#ifdef JP
info[i++] = "あなたは電撃への強力な耐性を持っている。";
#else
		info[i++] = "You resist lightning exceptionally well.";
#endif

	}
	else if (p_ptr->resist_elec || IS_OPPOSE_ELEC())
	{
#ifdef JP
info[i++] = "あなたは電撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to lightning.";
#endif

	}

	if (prace_is_(RACE_KAPPA) && !p_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "あなたは電撃に弱い。";
#else
		info[i++] = "You are susceptible to damage from lightning.";
#endif

	}

	if (p_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "あなたは火に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to fire.";
#endif

	}
	else if (p_ptr->resist_fire && IS_OPPOSE_FIRE())
	{
#ifdef JP
info[i++] = "あなたは火への強力な耐性を持っている。";
#else
		info[i++] = "You resist fire exceptionally well.";
#endif

	}
	else if (p_ptr->resist_fire || IS_OPPOSE_FIRE())
	{
#ifdef JP
info[i++] = "あなたは火への耐性を持っている。";
#else
		info[i++] = "You are resistant to fire.";
#endif

	}

	if (prace_is_(RACE_ENT) && !p_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "あなたは火に弱い。";
#else
		info[i++] = "You are susceptible to damage from fire.";
#endif

	}

	if (p_ptr->immune_cold)
	{
#ifdef JP
info[i++] = "あなたは冷気に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to cold.";
#endif

	}
	else if (p_ptr->resist_cold && IS_OPPOSE_COLD())
	{
#ifdef JP
info[i++] = "あなたは冷気への強力な耐性を持っている。";
#else
		info[i++] = "You resist cold exceptionally well.";
#endif

	}
	else if (p_ptr->resist_cold || IS_OPPOSE_COLD())
	{
#ifdef JP
info[i++] = "あなたは冷気への耐性を持っている。";
#else
		info[i++] = "You are resistant to cold.";
#endif

	}

	if (p_ptr->resist_pois && IS_OPPOSE_POIS())
	{
#ifdef JP
info[i++] = "あなたは毒への強力な耐性を持っている。";
#else
		info[i++] = "You resist poison exceptionally well.";
#endif

	}
	else if (p_ptr->resist_pois || IS_OPPOSE_POIS())
	{
#ifdef JP
info[i++] = "あなたは毒への耐性を持っている。";
#else
		info[i++] = "You are resistant to poison.";
#endif

	}

	if (p_ptr->resist_lite)
	{
#ifdef JP
info[i++] = "あなたは閃光への耐性を持っている。";
#else
		info[i++] = "You are resistant to bright light.";
#endif

	}

	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
#ifdef JP
info[i++] = "あなたは閃光に弱い。";
#else
		info[i++] = "You are susceptible to damage from bright light.";
#endif

	}

	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE) || p_ptr->wraith_form)
	{
#ifdef JP
info[i++] = "あなたは暗黒に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to darkness.";
#endif
	}

	else if (p_ptr->resist_dark)
	{
#ifdef JP
info[i++] = "あなたは暗黒への耐性を持っている。";
#else
		info[i++] = "You are resistant to darkness.";
#endif

	}
	if (p_ptr->resist_sound)
	{
#ifdef JP
info[i++] = "あなたは音波の衝撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to sonic attacks.";
#endif

	}
	if (p_ptr->resist_disen)
	{
#ifdef JP
info[i++] = "あなたは劣化への耐性を持っている。";
#else
		info[i++] = "You are resistant to disenchantment.";
#endif

	}
	if (p_ptr->resist_chaos)
	{
#ifdef JP
info[i++] = "あなたはカオスの力への耐性を持っている。";
#else
		info[i++] = "You are resistant to chaos.";
#endif

	}
	if (p_ptr->resist_shard)
	{
#ifdef JP
info[i++] = "あなたは破片の攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to blasts of shards.";
#endif

	}
	///del131228 因果混乱耐性削除
/*
	if (p_ptr->resist_nexus)
	{
#ifdef JP
info[i++] = "あなたは因果混乱の攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to nexus attacks.";
#endif

	}
*/

	if (p_ptr->resist_water)
	{
#ifdef JP
info[i++] = "あなたは激流による攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to torrential attacks.";
#endif

	}
	if (p_ptr->resist_holy)
	{
#ifdef JP
info[i++] = "あなたは破邪攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to holy attacks.";
#endif

	}

	if (p_ptr->pclass == CLASS_YOSHIKA || prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST))
	{
#ifdef JP
info[i++] = "あなたは地獄の力を吸収できる。";
#else
		info[i++] = "You can drain nether forces.";
#endif

	}
	else if (p_ptr->resist_neth)
	{
#ifdef JP
		info[i++] = "あなたは地獄の力とエナジードレインへの耐性を持っている。";
		//info[i++] = "あなたは地獄の力への耐性を持っている。";
#else
		info[i++] = "You are resistant to nether forces and energy draining.";
#endif

	}

	if (p_ptr->resist_time)
	{
#ifdef JP
		///msg131228
//info[i++] = "あなたは時間逆転への耐性を持っている。";
info[i++] = "あなたは時空攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to time and space attacks.";
#endif

	}

	if (p_ptr->sustain_str)
	{
#ifdef JP
info[i++] = "あなたの腕力は維持されている。";
#else
		info[i++] = "Your strength is sustained.";
#endif

	}
	if (p_ptr->sustain_int)
	{
#ifdef JP
info[i++] = "あなたの知能は維持されている。";
#else
		info[i++] = "Your intelligence is sustained.";
#endif

	}
	if (p_ptr->sustain_wis)
	{
#ifdef JP
info[i++] = "あなたの賢さは維持されている。";
#else
		info[i++] = "Your wisdom is sustained.";
#endif

	}
	if (p_ptr->sustain_con)
	{
#ifdef JP
info[i++] = "あなたの耐久力は維持されている。";
#else
		info[i++] = "Your constitution is sustained.";
#endif

	}
	if (p_ptr->sustain_dex)
	{
#ifdef JP
info[i++] = "あなたの器用さは維持されている。";
#else
		info[i++] = "Your dexterity is sustained.";
#endif

	}
	if (p_ptr->sustain_chr)
	{
#ifdef JP
info[i++] = "あなたの魅力は維持されている。";
#else
		info[i++] = "Your charisma is sustained.";
#endif

	}

	if (have_flag(flgs, TR_STR))
	{
#ifdef JP
info[i++] = "あなたの腕力は装備によって影響を受けている。";
#else
		info[i++] = "Your strength is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INT))
	{
#ifdef JP
info[i++] = "あなたの知能は装備によって影響を受けている。";
#else
		info[i++] = "Your intelligence is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_WIS))
	{
#ifdef JP
info[i++] = "あなたの賢さは装備によって影響を受けている。";
#else
		info[i++] = "Your wisdom is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_DEX))
	{
#ifdef JP
info[i++] = "あなたの器用さは装備によって影響を受けている。";
#else
		info[i++] = "Your dexterity is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CON))
	{
#ifdef JP
info[i++] = "あなたの耐久力は装備によって影響を受けている。";
#else
		info[i++] = "Your constitution is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CHR))
	{
#ifdef JP
info[i++] = "あなたの魅力は装備によって影響を受けている。";
#else
		info[i++] = "Your charisma is affected by your equipment.";
#endif

	}

	if (have_flag(flgs, TR_STEALTH))
	{
#ifdef JP
info[i++] = "あなたの隠密行動能力は装備によって影響を受けている。";
#else
		info[i++] = "Your stealth is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SEARCH))
	{
#ifdef JP
info[i++] = "あなたの探索能力は装備によって影響を受けている。";
#else
		info[i++] = "Your searching ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INFRA))
	{
#ifdef JP
info[i++] = "あなたの赤外線視力は装備によって影響を受けている。";
#else
		info[i++] = "Your infravision is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_TUNNEL))
	{
#ifdef JP
info[i++] = "あなたの採掘能力は装備によって影響を受けている。";
#else
		info[i++] = "Your digging ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SPEED))
	{
#ifdef JP
info[i++] = "あなたのスピードは装備によって影響を受けている。";
#else
		info[i++] = "Your speed is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_BLOWS))
	{
#ifdef JP
info[i++] = "あなたの攻撃速度は装備によって影響を受けている。";
#else
		info[i++] = "Your attack speed is affected by your equipment.";
#endif

	}


	/* Access the current weapon */
	o_ptr = &inventory[INVEN_RARM];

	/* Analyze the weapon */
	if (o_ptr->k_idx && !check_invalidate_inventory(INVEN_RARM))
	{
		/* Indicate Blessing */
		if (have_flag(flgs, TR_BLESSED))
		{
#ifdef JP
info[i++] = "あなたの武器は神の祝福を受けている。";
#else
			info[i++] = "Your weapon has been blessed by the gods.";
#endif

		}

		if (have_flag(flgs, TR_CHAOTIC))
		{
#ifdef JP
info[i++] = "あなたの武器はログルスの徴の属性をもつ。";
#else
			info[i++] = "Your weapon is branded with the Sign of Logrus.";
#endif

		}

		/* Hack */
		if (have_flag(flgs, TR_IMPACT))
		{
#ifdef JP
info[i++] = "あなたの武器は打撃で地震を発生することができる。";
#else
			info[i++] = "The impact of your weapon can cause earthquakes.";
#endif

		}

		if (have_flag(flgs, TR_VORPAL))
		{
#ifdef JP
info[i++] = "あなたの武器は非常に鋭い。";
#else
			info[i++] = "Your weapon is very sharp.";
#endif

		}

		if (have_flag(flgs, TR_VAMPIRIC))
		{
#ifdef JP
info[i++] = "あなたの武器は敵から生命力を吸収する。";
#else
			info[i++] = "Your weapon drains life from your foes.";
#endif

		}

		/* Special "Attack Bonuses" */
		if (have_flag(flgs, TR_BRAND_ACID))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を溶かす。";
#else
			info[i++] = "Your weapon melts your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_ELEC))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を感電させる。";
#else
			info[i++] = "Your weapon shocks your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_FIRE))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を燃やす。";
#else
			info[i++] = "Your weapon burns your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_COLD))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を凍らせる。";
#else
			info[i++] = "Your weapon freezes your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_POIS))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を毒で侵す。";
#else
			info[i++] = "Your weapon poisons your foes.";
#endif

		}

		/* Special "slay" flags */
		if (have_flag(flgs, TR_KILL_ANIMAL))
		{
#ifdef JP
info[i++] = "あなたの武器は動物の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of animals.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ANIMAL))
		{
#ifdef JP
info[i++] = "あなたの武器は動物に対して強い力を発揮する。";
#else
			info[i++] = "Your weapon strikes at animals with extra force.";
#endif

		}
		if (have_flag(flgs, TR_KILL_EVIL))
		{
#ifdef JP
info[i++] = "あなたの武器は邪悪なる存在の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of evil.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_EVIL))
		{
#ifdef JP
info[i++] = "あなたの武器は邪悪なる存在に対して強い力を発揮する。";
#else
			info[i++] = "Your weapon strikes at evil with extra force.";
#endif

		}
		if (have_flag(flgs, TR_KILL_HUMAN))
		{
#ifdef JP
info[i++] = "あなたの武器は人間の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of humans.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_HUMAN))
		{
#ifdef JP
info[i++] = "あなたの武器は人間に対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against humans.";
#endif

		}
		if (have_flag(flgs, TR_KILL_UNDEAD))
		{
#ifdef JP
info[i++] = "あなたの武器はアンデッドの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of undead.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_UNDEAD))
		{
#ifdef JP
info[i++] = "あなたの武器はアンデッドに対して神聖なる力を発揮する。";
#else
			info[i++] = "Your weapon strikes at undead with holy wrath.";
#endif

		}
		if (have_flag(flgs, TR_KILL_DEMON))
		{
#ifdef JP
info[i++] = "あなたの武器はデーモンの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of demons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DEMON))
		{
#ifdef JP
info[i++] = "あなたの武器はデーモンに対して神聖なる力を発揮する。";
#else
			info[i++] = "Your weapon strikes at demons with holy wrath.";
#endif

		}
/*
		if (have_flag(flgs, TR_KILL_ORC))
		{
#ifdef JP
info[i++] = "あなたの武器はオークの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of orcs.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ORC))
		{
#ifdef JP
info[i++] = "あなたの武器はオークに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against orcs.";
#endif

		}
		if (have_flag(flgs, TR_KILL_TROLL))
		{
#ifdef JP
info[i++] = "あなたの武器はトロルの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of trolls.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_TROLL))
		{
#ifdef JP
info[i++] = "あなたの武器はトロルに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against trolls.";
#endif

		}
		if (have_flag(flgs, TR_KILL_GIANT))
		{
#ifdef JP
info[i++] = "あなたの武器はジャイアントの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of giants.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_GIANT))
		{
#ifdef JP
info[i++] = "あなたの武器はジャイアントに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against giants.";
#endif

		}
*/
		/* Special "kill" flags */
		if (have_flag(flgs, TR_KILL_DRAGON))
		{
#ifdef JP
info[i++] = "あなたの武器はドラゴンの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of dragons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DRAGON))
		{
#ifdef JP
info[i++] = "あなたの武器はドラゴンに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against dragons.";
#endif

		}

		if (have_flag(flgs, TR_FORCE_WEAPON))
		{
#ifdef JP
info[i++] = "あなたの武器はMPを使って攻撃する。";
#else
			info[i++] = "Your weapon causes greate damages using your MP.";
#endif

		}
		if (have_flag(flgs, TR_THROW))
		{
#ifdef JP
info[i++] = "あなたの武器は投げやすい。";
#else
			info[i++] = "Your weapon can be thrown well.";
#endif
		}
	}


	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 13);

	/* Label the information */
#ifdef JP
prt("        あなたの状態:", 1, 15);
#else
	prt("     Your Attributes:", 1, 15);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == 22) && (j+1 < i))
		{
#ifdef JP
prt("-- 続く --", k, 15);
#else
			prt("-- more --", k, 15);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Pause */
#ifdef JP
prt("[何かキーを押すとゲームに戻ります]", k, 13);
#else
	prt("[Press any key to continue]", k, 13);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}


static int report_magics_aux(int dur)
{
	if (dur <= 5)
	{
		return 0;
	}
	else if (dur <= 10)
	{
		return 1;
	}
	else if (dur <= 20)
	{
		return 2;
	}
	else if (dur <= 50)
	{
		return 3;
	}
	else if (dur <= 100)
	{
		return 4;
	}
	else if (dur <= 200)
	{
		return 5;
	}
	else
	{
		return 6;
	}
}

static cptr report_magic_durations[] =
{
#ifdef JP
"ごく短い間",
"少しの間",
"しばらくの間",
"多少長い間",
"長い間",
"非常に長い間",
"信じ難いほど長い間",
"モンスターを攻撃するまで"
#else
	"for a short time",
	"for a little while",
	"for a while",
	"for a long while",
	"for a long time",
	"for a very long time",
	"for an incredibly long time",
	"until you hit a monster"
#endif

};


/*
 * Report all currently active magical effects.
 */
/*:::周囲にある魔法をよりよく理解できる気がする、の変異*/
///sysdel　magic 魔法を理解する変異　削除予定
#if 0
void report_magics(void)
{
	int     i = 0, j, k;
	char    Dummy[80];
	cptr    info[128];
	int     info2[128];


	if (p_ptr->blind)
	{
		info2[i]  = report_magics_aux(p_ptr->blind);
#ifdef JP
info[i++] = "あなたは目が見えない";
#else
		info[i++] = "You cannot see";
#endif

	}
	if (p_ptr->confused)
	{
		info2[i]  = report_magics_aux(p_ptr->confused);
#ifdef JP
info[i++] = "あなたは混乱している";
#else
		info[i++] = "You are confused";
#endif

	}
	if (p_ptr->afraid)
	{
		info2[i]  = report_magics_aux(p_ptr->afraid);
#ifdef JP
info[i++] = "あなたは恐怖に侵されている";
#else
		info[i++] = "You are terrified";
#endif

	}
	if (p_ptr->poisoned)
	{
		info2[i]  = report_magics_aux(p_ptr->poisoned);
#ifdef JP
info[i++] = "あなたは毒に侵されている";
#else
		info[i++] = "You are poisoned";
#endif

	}
	if (p_ptr->image)
	{
		info2[i]  = report_magics_aux(p_ptr->image);
#ifdef JP
info[i++] = "あなたは幻覚を見ている";
#else
		info[i++] = "You are hallucinating";
#endif

	}
	if (p_ptr->blessed)
	{
		info2[i]  = report_magics_aux(p_ptr->blessed);
#ifdef JP
info[i++] = "あなたは高潔さを感じている";
#else
		info[i++] = "You feel rightous";
#endif

	}
	if (p_ptr->hero)
	{
		info2[i]  = report_magics_aux(p_ptr->hero);
#ifdef JP
info[i++] = "あなたはヒーロー気分だ";
#else
		info[i++] = "You feel heroic";
#endif

	}
	if (p_ptr->shero)
	{
		info2[i]  = report_magics_aux(p_ptr->shero);
#ifdef JP
info[i++] = "あなたは戦闘狂だ";
#else
		info[i++] = "You are in a battle rage";
#endif

	}
	if (p_ptr->protevil)
	{
		info2[i]  = report_magics_aux(p_ptr->protevil);
#ifdef JP
info[i++] = "あなたは邪悪なる存在から守られている";
#else
		info[i++] = "You are protected from evil";
#endif

	}
	if (p_ptr->shield)
	{
		info2[i]  = report_magics_aux(p_ptr->shield);
#ifdef JP
info[i++] = "あなたは神秘のシールドで守られている";
#else
		info[i++] = "You are protected by a mystic shield";
#endif

	}
	if (p_ptr->invuln)
	{
		info2[i]  = report_magics_aux(p_ptr->invuln);
#ifdef JP
info[i++] = "あなたは無敵だ";
#else
		info[i++] = "You are invulnerable";
#endif

	}
	if (p_ptr->wraith_form)
	{
		info2[i]  = report_magics_aux(p_ptr->wraith_form);
#ifdef JP
info[i++] = "あなたは幽体化している";
#else
		info[i++] = "You are incorporeal";
#endif

	}
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
		info2[i]  = 7;
#ifdef JP
info[i++] = "あなたの手は赤く輝いている";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (p_ptr->word_recall)
	{
		info2[i]  = report_magics_aux(p_ptr->word_recall);
#ifdef JP
		info[i++] = "この後帰還の詔が発動する";
#else
		info[i++] = "You are waiting to be recalled";
#endif

	}
	if (p_ptr->alter_reality)
	{
		info2[i]  = report_magics_aux(p_ptr->alter_reality);
#ifdef JP
		info[i++] = "この後現実変容が発動する";
#else
		info[i++] = "You waiting to be altered";
#endif

	}
	if (p_ptr->oppose_acid)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_acid);
#ifdef JP
info[i++] = "あなたは酸への耐性を持っている";
#else
		info[i++] = "You are resistant to acid";
#endif

	}
	if (p_ptr->oppose_elec)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_elec);
#ifdef JP
info[i++] = "あなたは電撃への耐性を持っている";
#else
		info[i++] = "You are resistant to lightning";
#endif

	}
	if (p_ptr->oppose_fire)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_fire);
#ifdef JP
info[i++] = "あなたは火への耐性を持っている";
#else
		info[i++] = "You are resistant to fire";
#endif

	}
	if (p_ptr->oppose_cold)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_cold);
#ifdef JP
info[i++] = "あなたは冷気への耐性を持っている";
#else
		info[i++] = "You are resistant to cold";
#endif

	}
	if (p_ptr->oppose_pois)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_pois);
#ifdef JP
info[i++] = "あなたは毒への耐性を持っている";
#else
		info[i++] = "You are resistant to poison";
#endif

	}

	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 13);

	/* Label the information */
#ifdef JP
prt("    現在かかっている魔法     :", 1, 15);
#else
	prt("     Your Current Magic:", 1, 15);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
#ifdef JP
		sprintf(Dummy, "%-28s : 期間 - %s ", info[j],
#else
		sprintf(Dummy, "%s %s.", info[j],
#endif

			report_magic_durations[info2[j]]);
		prt(Dummy, k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == 22) && (j + 1 < i))
		{
#ifdef JP
prt("-- 続く --", k, 15);
#else
			prt("-- more --", k, 15);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Pause */
#ifdef JP
prt("[何かキーを押すとゲームに戻ります]", k, 13);
#else
	prt("[Press any key to continue]", k, 13);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}
#endif

static bool detect_feat_flag(int range, int flag, bool known)
{
	int       x, y;
	bool      detect = FALSE;
	cave_type *c_ptr;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x <= cur_wid - 1; x++)
		{
			int dist = distance(py, px, y, x);
			if (dist > range) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Hack -- Safe */
			if (flag == FF_TRAP)
			{
				/* Mark as detected */
				if (dist <= range && known)
				{
					if (dist <= range - 1) c_ptr->info |= (CAVE_IN_DETECT);

					c_ptr->info &= ~(CAVE_UNSAFE);

					/* Redraw */
					lite_spot(y, x);
				}
			}

			/* Detect flags */
			if (cave_have_flag_grid(c_ptr, flag))
			{
				/* Detect secrets */
				disclose_grid(y, x);

				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	/* Result */
	return detect;
}


/*
 * Detect all traps on current panel
 */
/*:::トラップ感知　詳細未読*/
bool detect_traps(int range, bool known)
{
	bool detect = detect_feat_flag(range, FF_TRAP, known);

	if (known) p_ptr->dtrap = TRUE;

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 0) detect = FALSE;
	if(p_ptr->radar_sense) detect = FALSE;
	if (music_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI) && p_ptr->lev > 34) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
		msg_print("トラップの存在を感じとった！");
#else
		msg_print("You sense the presence of traps!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect all doors on current panel
 */
bool detect_doors(int range)
{
	bool detect = detect_feat_flag(range, FF_DOOR, TRUE);

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 0) detect = FALSE;
	if(p_ptr->radar_sense) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
		msg_print("ドアの存在を感じとった！");
#else
		msg_print("You sense the presence of doors!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect all stairs on current panel
 */
bool detect_stairs(int range)
{
	bool detect = detect_feat_flag(range, FF_STAIRS, TRUE);
	if(p_ptr->radar_sense) detect = FALSE;

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
		msg_print("階段の存在を感じとった！");
#else
		msg_print("You sense the presence of stairs!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect any treasure on the current panel
 */
bool detect_treasure(int range)
{
	bool detect = detect_feat_flag(range, FF_HAS_GOLD, TRUE);

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
		msg_print("埋蔵された財宝の存在を感じとった！");
#else
		msg_print("You sense the presence of buried treasure!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect all "gold" objects on the current panel
 */
bool detect_objects_gold(int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		/* Detect "gold" objects */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 6) detect = FALSE;
	if(p_ptr->radar_sense) detect = FALSE;
	if (music_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI) && p_ptr->lev > 29) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("財宝の存在を感じとった！");
#else
		msg_print("You sense the presence of treasure!");
#endif

	}

	if (detect_monsters_string(range, "$"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "normal" objects on the current panel
 */
bool detect_objects_normal(int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		/* Detect "real" objects */
		if (o_ptr->tval != TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 6) detect = FALSE;
	if(p_ptr->radar_sense) detect = FALSE;
	if (music_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI) && p_ptr->lev > 29) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("アイテムの存在を感じとった！");
#else
		msg_print("You sense the presence of objects!");
#endif

	}

	if (detect_monsters_string(range, "!=?|/`"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


//v2.0.20 特定のアイテムをフロアから探す
// (似たような関数をすでに作ってたような気がする)
bool detect_specific_objects(int range, int mode)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		bool flag_ok = FALSE;

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		//modeに応じた発見判定
		switch (mode)
		{
		case 1://本探し
			if (o_ptr->tval >= MIN_MAGIC && o_ptr->tval <= TV_BOOK_END) flag_ok = TRUE;
			if (o_ptr->tval == TV_MAGICITEM && o_ptr->sval == SV_MAGICITEM_MARISA) flag_ok = TRUE;
			if (o_ptr->tval == TV_MAGICITEM && o_ptr->sval == SV_MAGICITEM_HYAKKI) flag_ok = TRUE;
			if (o_ptr->tval == TV_CAPTURE) flag_ok = TRUE;

			break;

		default:
			msg_format(_("ERROR:detect_specific_objects()に未設定のmode値(%d)が渡された",
                    "ERROR: undefined mode (%d) passed as an argument to detect_specific_objects()"),mode);
			return FALSE;


		}


		/* Detect "real" objects */
		if (flag_ok)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}


	/* Describe */
	if (detect)
	{
		if(mode == 1) msg_print(_("フロアに落ちている本の場所を把握した！",
                                "You find out books lying around on the floor!"));

	}

	/* Result */
	return (detect);
}


/*
 * Detect all "magic" objects on the current panel.
 *
 * This will light up all spaces with "magic" items, including artifacts,
 * ego-items, potions, scrolls, books, rods, wands, staves, amulets, rings,
 * and "enchanted" items of the "good" variety.
 *
 * It can probably be argued that this function is now too powerful.
 */
/*:::魔法感知*/
bool detect_objects_magic(int range)
{
	int i, y, x, tv;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan all objects */
	for (i = 1; i < o_max; i++)
	{

		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		//v2.0.19 魔法アイテムの判定を別関数に括り出し
		if (!object_is_magic_item(o_ptr)) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range) continue;

		/* Memorize the item */
		o_ptr->marked |= OM_FOUND;

		/* Redraw */
		lite_spot(y, x);

		/* Detect */
		detect = TRUE;
	}

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("魔法のアイテムの存在を感じとった！");
#else
		msg_print("You sense the presence of magic objects!");
#endif

	}

	/* Return result */
	return (detect);
}


/*
 * Detect all "normal" monsters on the current panel
 */
/*:::モンスター感知　詳細未読*/
bool detect_monsters_normal(int range)
{
	int i, y, x;

	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;


	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		//v1.1.43 サニー特技専用フラグ 別の場所で計算したflying_dist値のあるところだけモンスター感知する
		if (flag_sunny_refraction)
		{
			if (!cave[y][x].flying_dist) continue;
		}


		/* Detect all non-invisible monsters */
		if (!(r_ptr->flags2 & RF2_INVISIBLE) || p_ptr->see_inv)
		{
			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 3) flag = FALSE;
	if(p_ptr->radar_sense) flag = FALSE;
	if (music_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI) ) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("モンスターの存在を感じとった！");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all "invisible" monsters around the player
 */
/*:::透明体感知　詳細未読*/
bool detect_monsters_invis(int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect invisible monsters */
		if (r_ptr->flags2 & RF2_INVISIBLE)
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 3) flag = FALSE;
	if(p_ptr->radar_sense) flag = FALSE;
	if (music_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI) ) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("透明な生物の存在を感じとった！");
#else
		msg_print("You sense the presence of invisible creatures!");
#endif

	}

	/* Result */
	return (flag);
}



/*
 * Detect all "evil" monsters on current panel
 */
bool detect_monsters_evil(int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect evil monsters */
		if (r_ptr->flags3 & RF3_ANG_CHAOS)
		{
			if (is_original_ap(m_ptr))
			{
				/* Take note that they are evil */
				r_ptr->r_flags3 |= (RF3_ANG_CHAOS);

				/* Update monster recall window */
				if (p_ptr->monster_race_idx == m_ptr->r_idx)
				{
					/* Window stuff */
					p_ptr->window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("邪悪なる生物の存在を感じとった！");
#else
		msg_print("You sense the presence of evil creatures!");
#endif

	}

	/* Result */
	return (flag);
}


//v1.1.46 倒すとアイテムや財宝を落とすモンスター　女苑専用
bool detect_monsters_rich(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		bool flag_special_item = FALSE;

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		//普通にはアイテムを落とさないがmonster_death()でアイテムを落とす特殊処理するモンスターにフラグON。
		//今後monster_death()に特殊アイテムドロップ処理を追加するときにはここにも追加する必要があるだろう。汚いコードだが今更RF1_DROP_SPECIAL_ITEMとかフラグ実装するのも面倒。
		switch (m_ptr->r_idx)
		{
		case MON_BLOODLETTER:
		case MON_RAAL:
		case MON_B_DEATH_SWORD:
		case MON_MAGIC_MUSHROOM_RED:
		case MON_MAGIC_MUSHROOM_WHITE:
		case MON_MAGIC_MUSHROOM_BLUE:
		case MON_MAGIC_MUSHROOM_BLACK:
		case MON_MAGIC_MUSHROOM_GREEN:
		case MON_MAGIC_MUSHROOM_GAUDY:
		case MON_MAGIC_MUSHROOM_GRAY:
		case MON_MAGIC_MUSHROOM_CLEAR:
		case MON_MAGIC_MUSHROOM:
		case  MON_MITHRIL_GOLEM:
		case MON_CHAOS_BLADE:
		case MON_NIGHTMARE_F:
			flag_special_item = TRUE;
			break;
		}

		switch (r_ptr->d_char)
		{
		case '(':
		case '/':
		case '[':
		case '\\':
		case '|':
		case 'D':
		case '$':
			flag_special_item = TRUE;
			break;

		}

		//アイテムドロップがあるモンスター、もしくはアイテムを拾ったモンスターを感知
		if ((r_ptr->flags1 & (RF1_DROP_60 | RF1_DROP_90 | RF1_DROP_1D2 | RF1_DROP_2D2 | RF1_DROP_3D2 | RF1_DROP_4D2))
			|| m_ptr->hold_o_idx
			|| flag_special_item)
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print(_("モンスターのもつ財貨の匂いを嗅ぎつけた！",
                    "You sense the smell of monsters' goods!"));

	}

	/* Result */
	return (flag);
}




/*
 * Detect all "nonliving", "undead" or "demonic" monsters on current panel
 */
bool detect_monsters_nonliving(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!monster_living(r_ptr))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("自然でないモンスターの存在を感じた！");
#else
		msg_print("You sense the presence of unnatural beings!");
#endif

	}

	/* Result */
	return (flag);
}

///mod140324 生命体感知追加
/*
 * 生命体感知 nonlivingの逆
 */
bool detect_monsters_living(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect non-living monsters */
		if (monster_living(r_ptr))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("近くの生命の存在を感じた！");
#else
		msg_print("You sense the presence of living beings!");
#endif

	}

	/* Result */
	return (flag);
}



/*
 * Detect all monsters it has mind on current panel
 */
/*:::殺気感知　WEIRD_MINDの敵も全て感知できる　暗闇の洞窟では範囲1/3*/
bool detect_monsters_mind(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!(r_ptr->flags2 & RF2_EMPTY_MIND))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
		if(p_ptr->pclass == CLASS_KOKORO)
			msg_print("周囲に漂う感情を感じとった。");
		else
			msg_print("殺気を感じとった！");

#else
		if(p_ptr->pclass == CLASS_KOKORO)
			msg_print("You sense emotions in the nearby area.");
		else
			msg_print("You sense killing intent!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all (string) monsters on current panel
 */
bool detect_monsters_string(int range, cptr Match)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect monsters with the same symbol */
		if (my_strchr(Match, r_ptr->d_char))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(MUSIC_DETECT) && p_ptr->magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("モンスターの存在を感じとった！");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * A "generic" detect monsters routine, tagged to flags3
 */
bool detect_monsters_xxx(int range, u32b match_flag)
{
	int  i, y, x;
	bool flag = FALSE;
#ifdef JP
cptr desc_monsters = "変なモンスター";
#else
	cptr desc_monsters = "weird monsters";
#endif

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect evil monsters */
		if (r_ptr->flags3 & (match_flag))
		{
			if (is_original_ap(m_ptr))
			{
				/* Take note that they are something */
				r_ptr->r_flags3 |= (match_flag);

				/* Update monster recall window */
				if (p_ptr->monster_race_idx == m_ptr->r_idx)
				{
					/* Window stuff */
					p_ptr->window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		switch (match_flag)
		{
			case RF3_DEMON:
#ifdef JP
desc_monsters = "デーモン";
#else
				desc_monsters = "demons";
#endif

				break;
			case RF3_UNDEAD:
#ifdef JP
desc_monsters = "アンデッド";
#else
				desc_monsters = "the undead";
#endif

				break;

			case RF3_DEITY:
				desc_monsters = _("神格", "deities");
				break;

			case RF3_KWAI:
				desc_monsters = _("妖怪", "youkai");
				break;

		}

		/* Describe result */
#ifdef JP
msg_format("%sの存在を感じとった！", desc_monsters);
#else
		msg_format("You sense the presence of %s!", desc_monsters);
#endif

		msg_print(NULL);
	}

	/* Result */
	return (flag);
}


/*
 * Detect everything
 */
/*:::全感知*/
bool detect_all(int range)
{
	bool detect = FALSE;

	/* Detect everything */
	if (detect_traps(range, TRUE)) detect = TRUE;
	if (detect_doors(range)) detect = TRUE;
	if (detect_stairs(range)) detect = TRUE;

	/* There are too many hidden treasure.  So... */
	/* if (detect_treasure(range)) detect = TRUE; */

	if (detect_objects_gold(range)) detect = TRUE;
	if (detect_objects_normal(range)) detect = TRUE;
	if (detect_monsters_invis(range)) detect = TRUE;
	if (detect_monsters_normal(range)) detect = TRUE;

	/* Result */
	return (detect);
}


/*
 * Apply a "project()" directly to all viewable monsters
 *
 * Note that affected monsters are NOT auto-tracked by this usage.
 *
 * To avoid misbehavior when monster deaths have side-effects,
 * this is done in two passes. -- JDL
 */

/*:::視界内攻撃　ターゲットやHPバーは変化しない*/
bool project_hack(int typ, int dam)
{
	int     i, x, y;
	int     flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;
	bool    obvious = FALSE;



	/* Mark all (nearby) monsters */
	/*:::視界内のモンスターを探しフラグを立てる*/
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Require line of sight */
		if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;

		/* Mark the monster */
		m_ptr->mflag |= (MFLAG_TEMP);
	}

	/* Affect all marked monsters */
	/*:::フラグの立ったモンスターにproject()でダメージ処理*/
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip unmarked monsters */
		if (!(m_ptr->mflag & (MFLAG_TEMP))) continue;

		/* Remove mark */
		m_ptr->mflag &= ~(MFLAG_TEMP);

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Jump directly to the target monster */
		if (project(0, 0, y, x, dam, typ, flg, -1)) obvious = TRUE;
	}

	/* Result */
	return (obvious);
}

/*:::主に敵による視界内攻撃*/
///mod160102 呼び出すときに中心座標を指定。who=0で＠の攻撃でも使えるようにした
bool project_hack3(int who, int cy, int cx, int typ, int dice, int sides, int base)
{
	int     i, x, y, dam;
	bool    flag_in_sight[MAX_HGT][MAX_WID];

	/*:::最初に視界内グリッドを判定しフラグを立てる。森などがまとめて溶けないように*/
	for (y = 0; y < cur_hgt-1; y++)
	{
		for (x = 0; x < cur_wid-1; x++)
		{
			if ( !projectable(cy, cx, y, x))
				flag_in_sight[y][x] = FALSE;
			else
				flag_in_sight[y][x] = TRUE;
		}
	}

	/*:::画面効果*/
	Term_fresh();
	for (y = 0; y < cur_hgt-1; y++)
		for (x = 0; x < cur_wid-1; x++)
			if(flag_in_sight[y][x] && player_can_see_bold(y,x))
			{
				print_rel('#', (0x30 + spell_color(typ)), y, x);
			}
	Term_fresh();
	Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor * 2);

	/*:::地形、アイテム、モンスターと＠の順に処理*/
	for (y = 0; y < cur_hgt-1; y++)
		for (x = 0; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(dice<1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;
				project_f(who, 0, y, x, dam, typ);
			}
	for (y = 0; y < cur_hgt-1; y++)
		for (x = 0; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(dice<1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;
				project_o(who, 0, y, x, dam, typ);
			}
	for (y = 0; y < cur_hgt-1; y++)
		for (x = 0; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(dice < 1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;
				if(cave[y][x].m_idx)
				{
					//モンスターが使った場合そのモンスター自身にダメージなし
					if(!who || cave[y][x].m_idx != who)
						project_m(who, 0, y, x, dam, typ, 0, TRUE);
				}

				if(who && player_bold(y,x))
				{
					char who_name[80];
					int tmp_dam = dam;
					monster_desc(who_name, &m_list[who], MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
					if(check_activated_nameless_arts(JKF1_EXPLOSION_DEF))
					{
						msg_print(_("あなたは攻撃に対しいくらかの耐性を示した。",
                                    "You somewhat resist the attack."));
						tmp_dam /= 2;
					}
					project_p(who, who_name, 0, y, x, tmp_dam, typ, PROJECT_PLAYER, 0);

					//v1.1.53 クラウンピースが火を見ると楽しくなる処理
					if (typ == GF_FIRE || typ == GF_NUKE)
						clownpiece_likes_fire(typ);

				}
			}

	/* Result */
	return (TRUE);
}




/*:::視界内攻撃2 地形とアイテムにも影響を与える。それぞれの対象にダメージ値のバラつきを適用する。'#'で視界効果を表示する*/
/*:::typ:GF_???属性　もし0ならランダムに決まる。混沌召来専用*/
///mod151013 x,yの初期値0→1
///mod160215 typ_baseが-1のとき閃光と暗黒属性の市松模様にする
bool project_hack2(int typ_base, int dice, int sides, int base)
{
	int     i, x, y, dam, typ;
	bool    flag_in_sight[MAX_HGT][MAX_WID];
	bool	flag_kinkakuzi = FALSE;

	int hurt_types[30] =
	{
		GF_ELEC,      GF_POIS,    GF_ACID,    GF_COLD,
		GF_FIRE,      GF_PLASMA,  GF_DISTORTION, GF_TORNADO,
		GF_HOLY_FIRE, GF_WATER,   GF_LITE,    GF_DARK,
		GF_FORCE,     GF_INACT,   GF_MANA,    GF_METEOR,
		GF_ICE,       GF_CHAOS,   GF_NETHER,  GF_DISENCHANT,
		GF_SHARDS,    GF_SOUND,   GF_NEXUS,   GF_POLLUTE,
		GF_TIME,      GF_GRAVITY, GF_ROCKET,  GF_NUKE,
		GF_HELL_FIRE, GF_DISINTEGRATE
	};
	//嵐のアンサンブル
	int typ_list_tsukumo[4] = {GF_ELEC,GF_WATER,GF_TORNADO,GF_SOUND};

	//金閣寺特殊処理
	if(typ_base == GF_KINKAKUJI)
	{
		typ = GF_DISINTEGRATE;
		flag_kinkakuzi = TRUE;
	}
	else
	{
		typ = typ_base;
	}

	/*:::最初に視界内グリッドを判定しフラグを立てる。森などがまとめて溶けないように*/
	for (y = 1; y < cur_hgt-1; y++)
	{
		for (x = 1; x < cur_wid-1; x++)
		{
			//金閣寺特殊処理 ＠中心の7グリッド正方形が無条件で対象
			if(flag_kinkakuzi)
			{
				if( py-y>3 || y-py>3 || px-x>3 || x-px>3)
					flag_in_sight[y][x] = FALSE;
				else
					flag_in_sight[y][x] = TRUE;
			}
			else
			{
				///mod151231 PERMANENTフラグのある地形はPROJECTがないときのみ対象にするようにした。階段上の敵が対象にならないため。
				if (!player_has_los_bold(y, x) || !projectable(py, px, y, x) || cave_have_flag_bold(y,x,FF_PERMANENT) && !cave_have_flag_bold(y,x,FF_PROJECT))
					flag_in_sight[y][x] = FALSE;
				else
					flag_in_sight[y][x] = TRUE;
			}
		}
	}

	/*:::画面効果*/
	Term_fresh();
	for (y = 1; y < cur_hgt-1; y++)
		for (x = 1; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(typ_base == GF_TSUKUMO_STORM) typ = typ_list_tsukumo[randint0(4)];
				else if(typ_base == 0 )typ =hurt_types[randint0(30)];
				else if(typ_base == GF_YUKARI_STORM )
				{
					if((x+y)%2) typ = GF_LITE;
					else typ = GF_DARK;
				}
				//v1.1.20 小町の投げ銭用特殊処理
				if(p_ptr->pclass == CLASS_KOMACHI && typ_base == GF_ARROW)
					print_rel('$', (0x30 + spell_color(typ)), y, x);
				//v1.1.63 虹属性の色をy座標によって変えてみる
				else if (typ == GF_RAINBOW)
				{
					int a;
					switch (y % 7)
					{
					case 0:a = TERM_VIOLET; break;
					case 1:a = TERM_BLUE; break;
					case 2:a = TERM_L_BLUE; break;
					case 3:a = TERM_L_GREEN; break;
					case 4:a = TERM_YELLOW; break;
					case 5:a = TERM_ORANGE; break;
					default:a = TERM_RED; break;

					}
					print_rel('#', a, y, x);

				}

				else
					print_rel('#', (0x30 + spell_color(typ)), y, x);



			}
	Term_fresh();
	Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor * 2);

	/*:::地形、アイテム、モンスターの順に処理*/
	for (y = 1; y < cur_hgt-1; y++)
		for (x = 1; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(typ_base == GF_TSUKUMO_STORM) typ = typ_list_tsukumo[randint0(4)];
				else if(typ_base == 0 )typ =hurt_types[randint0(30)];
				else if(typ_base == GF_YUKARI_STORM )
				{
					if((x+y)%2) typ = GF_LITE;
					else typ = GF_DARK;
				}

				if(dice<1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;
				project_f(0, 0, y, x, dam, typ);
			}
	for (y = 1; y < cur_hgt-1; y++)
		for (x = 1; x < cur_wid-1; x++)
			if(flag_in_sight[y][x])
			{
				if(typ_base == GF_TSUKUMO_STORM) typ = typ_list_tsukumo[randint0(4)];
				else if(typ_base == 0 )typ =hurt_types[randint0(30)];
				else if(typ_base == GF_YUKARI_STORM )
				{
					if((x+y)%2) typ = GF_LITE;
					else typ = GF_DARK;
				}
				if(dice<1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;
				project_o(0, 0, y, x, dam, typ);
			}
	/*:::乗馬中モンスターは保護されるはず？*/
	for (y = 1; y < cur_hgt-1; y++)
		for (x = 1; x < cur_wid-1; x++)
			if(flag_in_sight[y][x] && !player_bold(y,x))
			{
				if(typ_base == GF_TSUKUMO_STORM) typ = typ_list_tsukumo[randint0(4)];
				else if(typ_base == 0 )typ =hurt_types[randint0(30)];
				else if(typ_base == GF_YUKARI_STORM )
				{
					if((x+y)%2) typ = GF_LITE;
					else typ = GF_DARK;
				}
				if(dice<1 || sides < 1) dam = base;
				else dam = damroll(dice,sides)+base;

				project_m(0, 0, y, x, dam, typ, 0, TRUE);
			}
	/* Result */
	return (TRUE);
}


/*
 * Speed monsters
 */
bool speed_monsters(void)
{
	return (project_hack(GF_OLD_SPEED, p_ptr->lev));
}

/*
 * Slow monsters
 */
bool slow_monsters(int power)
{
	return (project_hack(GF_OLD_SLOW, power));
}

/*
 * Sleep monsters
 */
bool sleep_monsters(int power)
{
	return (project_hack(GF_OLD_SLEEP, power));
}


/*
 * Banish evil monsters
 */
bool banish_evil(int dist)
{
	return (project_hack(GF_AWAY_EVIL, dist));
}


/*
 * Turn undead
 */
///mod140324 powerを1.5倍にした
//v1.1.90 3倍にした
bool turn_undead(void)
{
	bool tester = (project_hack(GF_TURN_UNDEAD, p_ptr->lev * 3));
	///del131214 virtue
	//if (tester)
	//	chg_virtue(V_UNLIFE, -1);
	return tester;
}


/*
 * Dispel undead monsters
 */
bool dispel_undead(int dam)
{
	bool tester = (project_hack(GF_DISP_UNDEAD, dam));
	///del131214 virtue
	//if (tester)
	//	chg_virtue(V_UNLIFE, -2);
	return tester;
}

/*
 * Dispel evil monsters
 */
bool dispel_evil(int dam)
{
	return (project_hack(GF_DISP_EVIL, dam));
}

/*
 * Dispel good monsters
 */
bool dispel_good(int dam)
{
	return (project_hack(GF_DISP_GOOD, dam));
}

/*
 * Dispel all monsters
 */
bool dispel_monsters(int dam)
{
	return (project_hack(GF_DISP_ALL, dam));
}

/*
 * Dispel 'living' monsters
 */
bool dispel_living(int dam)
{
	return (project_hack(GF_DISP_LIVING, dam));
}

/*
 * Dispel demons
 */
bool dispel_demons(int dam)
{
	return (project_hack(GF_DISP_DEMON, dam));
}

bool dispel_kwai(int dam)
{
	return (project_hack(GF_DISP_KWAI, dam));
}

/*
 * Crusade
 */
bool crusade(void)
{
	return (project_hack(GF_CRUSADE, p_ptr->lev*4));
}

/*:::エキサイトモンスター？　詳細未読*/
/*:::mod140227 近くの敵を起こす「悲鳴」と、遠くの敵まで起こして視界内の敵を加速させる「アラーム」に分けることにした*/
///mod150911 who=-1で呼んだとき配下も加速される
/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void aggravate_monsters(int who, bool alarm)
{
	int     i;
	bool    sleep = FALSE;
	bool    speed = FALSE;
	int		speed_pet = 0;
	int		rad;

	if(alarm) rad = MAX_SIGHT * 2;
	else rad = MAX_SIGHT;

	/* Aggravate everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type    *m_ptr = &m_list[i];
/*		monster_race    *r_ptr = &r_info[m_ptr->r_idx]; */

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip aggravating monster (or player) */
		if (i == who) continue;

		/* Wake up nearby sleeping monsters */
		if (m_ptr->cdis < rad)
		{
			/* Wake up */
			if (MON_CSLEEP(m_ptr))
			{
				(void)set_monster_csleep(i, 0);
				sleep = TRUE;
			}
			if (!is_pet(m_ptr)) m_ptr->mflag2 |= MFLAG2_NOPET;
		}

		/* Speed up monsters in line of sight */
		if (player_has_los_bold(m_ptr->fy, m_ptr->fx) && alarm)
		{
			if(is_pet(m_ptr) && who == -1)
			{
				(void)set_monster_fast(i, MON_FAST(m_ptr) + 100);
				speed_pet++;
			}

			if (!is_pet(m_ptr))
			{
				(void)set_monster_fast(i, MON_FAST(m_ptr) + 100);
				speed = TRUE;
			}
		}
	}

	/* Messages */
#ifdef JP
	if(speed_pet)
		msg_format("あなたの配下%sはスピードを上げた!",(speed_pet > 1)?"達":"");

	if (speed) msg_print("付近の敵が弾かれたように素早く動いた！");
	else if (sleep) msg_print("周辺の寝ている者が起きた感じがする・・");
#else
    if(speed_pet)
		msg_format("The speed of your follower%s goes up!",(speed_pet > 1)?"s":"");

	if (speed) msg_print("Nearby enemies suddenly start moving faster!");
	else if (sleep) msg_print("You feel nearby sleeping monsters wake up...");
#endif
	if (p_ptr->riding) p_ptr->update |= PU_BONUS;
}


/*
 * Delete a non-unique/non-quest monster
 */
/*:::抹殺の処理の続き　指定した敵に消滅を試みる*/
bool genocide_aux(int m_idx, int power, bool player_cast, int dam_side, cptr spell_name)
{
	int          msec = delay_factor * delay_factor * delay_factor;
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	bool         resist = FALSE;

	if (is_pet(m_ptr) && !player_cast) return FALSE;

	if(m_ptr->r_idx == MON_MASTER_KAGUYA) return FALSE;

	/* Hack -- Skip Unique Monsters or Quest Monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) resist = TRUE;
	else if (r_ptr->flagsr & RFR_RES_ALL) resist = TRUE;

	else if (r_ptr->flags7 & RF7_UNIQUE2) resist = TRUE;
	else if (r_ptr->flags7 & RF7_VARIABLE) resist = TRUE;

	else if (m_idx == p_ptr->riding) resist = TRUE;

	else if(EXTRA_QUEST_FLOOR) resist = TRUE;

	else if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle) resist = TRUE;

	else if (player_cast && (r_ptr->level > randint0(power))) resist = TRUE;

	else if (player_cast && (m_ptr->mflag2 & MFLAG2_NOGENO)) resist = TRUE;

	/* Delete the monster */
	else
	{
		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
		}

		delete_monster_idx(m_idx);
	}

	if (resist && player_cast)
	{
		bool see_m = is_seen(m_ptr);
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
		if (see_m)
		{
#ifdef JP
			msg_format("%^sには効果がなかった。", m_name);
#else
			msg_format("%^s is unaffected.", m_name);
#endif
		}
		if (MON_CSLEEP(m_ptr))
		{
			(void)set_monster_csleep(m_idx, 0);
			if (m_ptr->ml)
			{
#ifdef JP
				msg_format("%^sが目を覚ました。", m_name);
#else
				msg_format("%^s wakes up.", m_name);
#endif
			}
		}
		if (is_friendly(m_ptr) && !is_pet(m_ptr) && m_ptr->r_idx != MON_MASTER_KAGUYA)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%sは怒った！", m_name);
#else
				msg_format("%^s gets angry!", m_name);
#endif
			}
			set_hostile(m_ptr);
		}
		if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
	}

	if (player_cast && dam_side)
	{

		char death_reason[160];

		//v1.1.92 take_hitにformat()の戻り地を渡すのはまずいらしいので修正
		sprintf(death_reason, _("%sの呪文を唱えた疲労", "the strain of casting %s"), spell_name);

		/* Take damage */
		take_hit(DAMAGE_GENO, randint1(dam_side), death_reason, -1);
	}

	/* Visual feedback */
	move_cursor_relative(py, px);

	/* Redraw */
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle */
	handle_stuff();

	/* Fresh */
	Term_fresh();

	/* Delay */
	Term_xtra(TERM_XTRA_DELAY, msec);

	return !resist;
}


/*
 * Delete all non-unique/non-quest monsters of a given "type" from the level
 */
/*:::抹殺　player_castはカオスパトロン報酬の抹殺のときのみFALSE*/
///mod141231 文字指定して呼び出せるようにする
bool symbol_genocide(int power, bool player_cast, int default_char)
{
	int  i;
	char typ;
	bool result = FALSE;

	/* Prevent genocide in quest levels */
	if ((p_ptr->inside_quest && !random_quest_number(dun_level)) ||
		EXTRA_QUEST_FLOOR || p_ptr->inside_arena || p_ptr->inside_battle)
	{
		return (FALSE);
	}


	if(default_char)
	{
		typ = default_char;
	}
	else
	{
	/* Mega-Hack -- Get a monster symbol */
#ifdef JP
		///mod150122 抹殺→追放
		while (!get_com("どの種類(文字)のモンスターを追放しますか: ", &typ, FALSE)) ;
#else
	while (!get_com("Choose a monster race (by symbol) to banish: ", &typ, FALSE)) ;
#endif
	}
	/* Delete the monsters of that "type" */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip "wrong" monsters */
		if (r_ptr->d_char != typ) continue;

		/* Take note */
#ifdef JP
		result |= genocide_aux(i, power, player_cast, 4, "抹殺");
#else
		result |= genocide_aux(i, power, player_cast, 4, "Banishment");
#endif
	}

	if (result)
	{
		if(one_in_(2))set_deity_bias(DBIAS_WARLIKE, -1);
		if(one_in_(2))set_deity_bias(DBIAS_REPUTATION, -1);
		//chg_virtue(V_VITALITY, -2);
		//chg_virtue(V_CHANCE, -1);
	}

	return result;
}


/*
 * Delete all nearby (non-unique) monsters
 */
/*:::周辺抹殺*/
bool mass_genocide(int power, bool player_cast)
{
	int  i;
	bool result = FALSE;

	/* Prevent mass genocide in quest levels */
	if ((p_ptr->inside_quest && !random_quest_number(dun_level)) ||
		EXTRA_QUEST_FLOOR || p_ptr->inside_arena || p_ptr->inside_battle)
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		/* Note effect */
#ifdef JP
		result |= genocide_aux(i, power, player_cast, 3, "周辺抹殺");
#else
		result |= genocide_aux(i, power, player_cast, 3, "Banish Nearby");
#endif
	}

	if (result)
	{
		if(one_in_(2))set_deity_bias(DBIAS_WARLIKE, -1);
		if(one_in_(2))set_deity_bias(DBIAS_REPUTATION, -1);
		//chg_virtue(V_VITALITY, -2);
		//chg_virtue(V_CHANCE, -1);
	}

	return result;
}

//周辺抹殺の改造ルーチン
//mode0:鈴瑚の浄化作戦　妖怪、悪魔、獣にのみ有効　ノーダメージ
//mode1:久侘歌の全霊鬼渡し　幻想郷の住人フラグのない敵に有効　ノーダメージ
//mode2:アビリティカード「純粋な自己肯定感」　ノーダメージ 野良神様の評判悪化なし
//mode3:美宵の記憶消去　ノーダメージ、酔っているモンスターに効果大
bool mass_genocide_2(int power, int rad, int mode)
{
	int  i;
	bool result = FALSE;

	/* Prevent mass genocide in quest levels */
	if ((p_ptr->inside_quest && !random_quest_number(dun_level)) ||
		EXTRA_QUEST_FLOOR || p_ptr->inside_arena || p_ptr->inside_battle)
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;
		/* Skip distant monsters */
		if (rad && m_ptr->cdis > rad) continue;

		if(mode == 0)
		{
			if(!(r_ptr->flags3 & ( RF3_KWAI | RF3_ANIMAL | RF3_DEMON))) continue;
			result |= genocide_aux(i, power, TRUE, 0, "");
		}
		else if (mode == 1)
		{
			if (r_ptr->flags3 & (RF3_GEN_MASK)) continue;
			result |= genocide_aux(i, power, TRUE, 0, "");

		}
		else if (mode == 2)
		{
			result |= genocide_aux(i, power, TRUE, 0, "");

		}
		else if (mode == 3)
		{
			if (r_ptr->flags2 & (RF2_EMPTY_MIND|RF2_WEIRD_MIND)) continue;

			if (MON_DRUNK(m_ptr)) power *= 2;
			result |= genocide_aux(i, power, TRUE, 0, "");

		}
		else
		{
			msg_format(_("ERROR:想定外のmodeでmass_genocide_2()が呼ばれた",
                        "ERROR: mass_genocide_2() called with an unexpected mode"));
			return FALSE;
		}
	//	result |= genocide_aux(i, power, player_cast, 3, "周辺抹殺");

	}

	if (result && mode != 2)
	{
		if(one_in_(2))set_deity_bias(DBIAS_WARLIKE, -1);
		if(one_in_(2))set_deity_bias(DBIAS_REPUTATION, -1);
	}

	return result;
}


//v1.1.32 専用ルーチンに分けた
//抵抗されない抹殺。
//QUESTORが近くにいれば距離100の強制テレポ
//永琳の主の輝夜は対象にならない
//黒服の男達、オープンパンドラ、摩多羅ハウスの星の加護
//v1.1.57 消したモンスターの数を返すことにした
int mass_genocide_3(int rad, bool geno_friend, bool geno_unique)
{
	int i;
	int geno_count = 0;

	if (p_ptr->inside_quest || p_ptr->inside_arena || p_ptr->inside_battle)
	{
		msg_print(_("ここでは使えない。", "You can't use it here."));
		return 0;
	}

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (!m_ptr->r_idx) continue;

		//rad0なら全モンスター対象
		if (rad && m_ptr->cdis > rad) continue;

		//乗馬中ペット、永琳の主を保護
		if (i == p_ptr->riding) continue;
		if (m_ptr->r_idx == MON_MASTER_KAGUYA) continue;

		//geno_friendでないとき友好とペットを保護
		if (!geno_friend && is_pet(m_ptr)) continue;
		if (!geno_friend && is_friendly(m_ptr)) continue;

		//QUESTORは抹殺されないが近くにいたら飛ばす
		if (r_ptr->flags1 & (RF1_QUESTOR))
		{
			if (m_ptr->cdis <= MAX_SIGHT)
				teleport_away(i, 100, TELEPORT_PASSIVE);
		}
		//geno_uniqueでないとき、ユニークは抹殺されないが近くにいたら飛ばす
		else if (!geno_unique && (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2)))
		{
			if (m_ptr->cdis <= MAX_SIGHT)
				teleport_away(i, 100, TELEPORT_PASSIVE);
		}
		//それ以外のモンスターを無抵抗で消す
		else
		{

			if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
			}
			delete_monster_idx(i);
			geno_count++;
		}
	}

	return geno_count;
}

/*
 * Delete all nearby (non-unique) undead
 */
bool mass_genocide_undead(int power, bool player_cast)
{
	int  i;
	bool result = FALSE;

	/* Prevent mass genocide in quest levels */
	if ((p_ptr->inside_quest && !random_quest_number(dun_level)) ||
		EXTRA_QUEST_FLOOR || p_ptr->inside_arena || p_ptr->inside_battle)
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		if (!(r_ptr->flags3 & RF3_UNDEAD)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		/* Note effect */
#ifdef JP
		result |= genocide_aux(i, power, player_cast, 3, "アンデッド消滅");
#else
		result |= genocide_aux(i, power, player_cast, 3, "Annihilate Undead");
#endif
	}

	if (result)
	{
		chg_virtue(V_UNLIFE, -2);
		chg_virtue(V_CHANCE, -1);
	}

	return result;
}



/*
 * Probe nearby monsters
 */
/*:::調査*/
///mon sys 調査
bool probing(void)
{
	int     i, speed;
	int cu, cv;
	bool    probe = FALSE;
	char buf[256];
	cptr align;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Probe all (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		int mon_ac;

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Require line of sight */
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible monsters */
		if (m_ptr->ml)
		{
			char m_name[80];

			/* Start the message */
			if (!probe)
			{
#ifdef JP
				if(satori_reading)
					msg_print("読心中...");
				else
					msg_print("調査中...");
#else
				if(satori_reading)
					msg_print("Reading mind...");
				else
					msg_print("Probing...");
#endif
			}

			msg_print(NULL);

			if(satori_reading && (r_ptr->flags2 & (RF2_EMPTY_MIND)))
			{
				monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_INDEF_HIDDEN);
				msg_format(_("%sの心は読み取れなかった。", "You fail to read the mind of %s."),m_name);
				continue;
			}
			else if(satori_reading && (r_ptr->flags2 & (RF2_WEIRD_MIND)))
			{
				monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_INDEF_HIDDEN);
				if(turn % 2) //turnが偶数のときWEIRD_MINDの読み取りに成功する　make_magic_list_satori()でも同じ判定を使う
				{
					msg_format(_("%sの心は読み取れなかった。", "You fail to read the mind of %s."),m_name);
					continue;
				}
				else msg_format(_("%sの心は読み取りづらいがどうにか読み取った。",
                                "The mind of %s was difficult to read, but you managed it."),m_name);
			}

			if (!is_original_ap(m_ptr))
			{
				if (m_ptr->mflag2 & MFLAG2_KAGE)
					m_ptr->mflag2 &= ~(MFLAG2_KAGE);

				///mod151111 あやしい影を調査したら既知カウント加算
				if(r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;

				m_ptr->ap_r_idx = m_ptr->r_idx;
				lite_spot(m_ptr->fy, m_ptr->fx);
			}
			/* Get "the monster" or "something" */
			monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_INDEF_HIDDEN);

			speed = m_ptr->mspeed - 110;
			if (MON_FAST(m_ptr)) speed += 10;
			if (MON_SLOW(m_ptr)) speed -= 10;
			///mod140929 悪夢モードのときの速度上昇を適用
			if(ironman_nightmare && i != p_ptr->riding) speed += 5;

			/* Get the monster's alignment */
#ifdef JP
			if ((r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)) == (RF3_ANG_CHAOS | RF3_ANG_COSMOS)) align = "善悪";
			else if (r_ptr->flags3 & RF3_ANG_CHAOS) align = "混沌";
			else if (r_ptr->flags3 & RF3_ANG_COSMOS) align = "秩序";
			else if ((m_ptr->sub_align & (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD)) == (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD)) align = "中立(善悪)";
			else if (m_ptr->sub_align & SUB_ALIGN_EVIL) align = "中立(混沌)";
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD) align = "中立(秩序)";
			else align = "中立";
#else
			if ((r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)) == (RF3_ANG_CHAOS | RF3_ANG_COSMOS)) align = "good&evil";
			else if (r_ptr->flags3 & RF3_ANG_CHAOS) align = "chaotic";
			else if (r_ptr->flags3 & RF3_ANG_COSMOS) align = "lawful";
			else if ((m_ptr->sub_align & (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD)) == (SUB_ALIGN_EVIL | SUB_ALIGN_GOOD)) align = "neutral (good&evil)";
			else if (m_ptr->sub_align & SUB_ALIGN_EVIL) align = "neutral (chaotic)";
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD) align = "neutral (lawful)";
			else align = "neutral";
#endif

			/* Describe the monster */
			mon_ac = r_ptr->ac;
			if(MON_DEC_DEF(m_ptr)) mon_ac = MONSTER_DECREASED_AC(mon_ac);

            sprintf(buf,_("%s ... 属性:%s HP:%d/%d AC:%d 速度:%s%d 経験:",
                        "%s ... align:%s HP:%d/%d AC:%d speed:%s%d exp:"), m_name, align, m_ptr->hp, m_ptr->maxhp, mon_ac, (speed > 0) ? "+" : "", speed);

			if (r_ptr->next_r_idx)
			{
				strcat(buf, format("%d/%d ", m_ptr->exp, r_ptr->next_exp));
			}
			else
			{
				strcat(buf, "xxx ");
			}

#ifdef JP
			if (MON_CSLEEP(m_ptr)) strcat(buf,"睡眠 ");
			if (MON_STUNNED(m_ptr)) strcat(buf,"朦朧 ");
			if (MON_MONFEAR(m_ptr)) strcat(buf,"恐怖 ");
			if (MON_CONFUSED(m_ptr)) strcat(buf,"混乱 ");
			if (MON_INVULNER(m_ptr)) strcat(buf,"無敵 ");
#else
			if (MON_CSLEEP(m_ptr)) strcat(buf,"sleeping ");
			if (MON_STUNNED(m_ptr)) strcat(buf,"stunned ");
			if (MON_MONFEAR(m_ptr)) strcat(buf,"scared ");
			if (MON_CONFUSED(m_ptr)) strcat(buf,"confused ");
			if (MON_INVULNER(m_ptr)) strcat(buf,"invulnerable ");
#endif
			buf[strlen(buf)-1] = '\0';
			prt(buf,0,0);

			/* HACK : Add the line to message buffer */
			message_add(buf);
			p_ptr->window |= (PW_MESSAGE);
			window_stuff();

			if (m_ptr->ml) move_cursor_relative(m_ptr->fy, m_ptr->fx);
			inkey();

			Term_erase(0, 0, 255);

			/*:::Hack - エンジニアがスカウターを使ったとき＠の倍以上のレベルの敵を見ると壊れる*/
			if(scouter_use && r_ptr->level > p_ptr->lev * 2)
			{
				scouter_use = FALSE;
				msg_print(NULL);
				Term->scr->cu = cu;
				Term->scr->cv = cv;
				Term_fresh();
				return TRUE;
			}

			/* Learn everything about this monster */
			if (lore_do_probe(m_ptr->r_idx))
			{
				char buf[160];

				/* Get base name of monster */
				strcpy(buf, (r_name + r_ptr->name));


				if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
				{
					object_desc(buf,&o_list[m_ptr->hold_o_idx],OD_NAME_ONLY);
				}
				else if(IS_RANDOM_UNIQUE_IDX(m_ptr->r_idx))
				{
					sprintf(buf,"%s", random_unique_name[m_ptr->r_idx - MON_RANDOM_UNIQUE_1]);
				}


#ifdef JP
				/* Note that we learnt some new flags  -Mogami- */
				msg_format("%sについてさらに詳しくなった気がする。", buf);
#else
				if (!(r_ptr->flags1 & RF1_UNIQUE) && !((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE))) {
					/* Pluralize it */
					plural_aux(buf);
				}

				/* Note that we learnt some new flags  -Mogami- */
				msg_format("You now know more about %s.", buf);
#endif

				/* Clear -more- prompt */
				msg_print(NULL);
			}

			/* Probe worked */
			probe = TRUE;
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	/* Done */
	if (probe)
	{
		///del131216 virtue
		//chg_virtue(V_KNOWLEDGE, 1);

#ifdef JP
msg_print("これで全部です。");
#else
		msg_print("That's all.");
#endif

	}
	else
		msg_print(_("モンスターの情報を得られなかった。", "You didn't get new information about monsters."));


	/* Result */
	return (probe);
}



/*
 * The spell of destruction
 *
 * This spell "deletes" monsters (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
/*:::*破壊*の魔法の処理　ダンジョン生成時の破壊地形もここ(in_generate)*/
///mod140510 破邪領域用にforce_floor引数を追加した
///mod140829 フラン用にフラグ追加 中心点を破壊の対象に含める
bool destroy_area(int y1, int x1, int r, bool in_generate, bool force_floor, bool flan)
{
	int       y, x, k, t;
	cave_type *c_ptr;
	bool      flag = FALSE;
	bool	flag_kaguya_away = FALSE;

	/* Prevent destruction of quest levels and town */
	//アリーナ内はdun_level0なのでinside_arena判定は不要
	if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
	{
		if(force_floor) msg_print(_("大地に門が開いたように見えたが、ただの弱い幻覚だった。",
                                    "It looks like there's a gate opening in the ground... but it wass only a weak illusion."));
		return (FALSE);
	}

	/* Lose monster light */
	if (!in_generate) clear_mon_lite();

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if (!in_bounds(y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if (k > r) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW | CAVE_KNOWN);

			//v1.1.35 ネムノ縄張り消去
			if(player_bold(y, x) && IS_NEMUNO_IN_SANCTUARY )
			{
				msg_print(_("聖域が侵された！", "Your sanctuary is destroyed!"));
				p_ptr->update |= (PU_BONUS|PU_HP);
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
			}
			c_ptr->cave_xtra_flag &= ~(CAVE_XTRA_FLAG_NEMUNO);


			if (!in_generate) /* Normal */
			{
				/* Lose unsafety */
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Hack -- Notice player affect */
				/*:::破壊の中心以外に＠が居ても巻き込まれないらしい*/
				if (player_bold(y, x))
				{
					/* Hurt the player later */
					flag = TRUE;

					/* Do not hurt this grid */
					continue;
				}
			}

			/* Hack -- Skip the epicenter */
			if (!flan && (y == y1) && (x == x1)) continue;
			/*:::破壊地点にモンスターがいたとき*/
			if (c_ptr->m_idx)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				if (in_generate) /* In generation */
				{
					/* Delete the monster (if any) */
					delete_monster(y, x);
				}
				/*:::クエストボスはHP全快でテレポアウェイ*/
				else if ((r_ptr->flags1 & RF1_QUESTOR) && !(m_ptr->mflag & MFLAG_EPHEMERA))
				{
					/* Heal the monster */
					m_ptr->hp = m_ptr->maxhp;

					/* Try to teleport away quest monsters */
					if (!teleport_away(c_ptr->m_idx, (r * 2) + 1, TELEPORT_DEC_VALOUR)) continue;
				}
				///mod140712 付喪神使い
				/*:::☆の付喪神は破壊で消滅しない*/
				else if((m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 ) && object_is_artifact(&o_list[m_ptr->hold_o_idx]))
				{
					//破壊処理が止まって邪魔なのでメッセージ削除した
					//char m_name[80];
					//monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					m_ptr->hp /= 2;
					teleport_away(c_ptr->m_idx, (r * 3) + 1, TELEPORT_NONMAGICAL);
					//msg_format("%sはどこかへ吹き飛ばされた！",m_name);
				}
				//永琳の主
				/*:::☆の付喪神は破壊で消滅しない*/
				else if(m_ptr->r_idx == MON_MASTER_KAGUYA)
				{
					m_ptr->hp /= 2;
					teleport_away(c_ptr->m_idx, randint1(r * 3) + (r * 3) , TELEPORT_NONMAGICAL);
					flag_kaguya_away = TRUE;
				}
				else
				{
					/*:::名前をつけたペットがいたら破壊された旨日記に書く*/
					if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_DESTROY, m_name);
					}

					/* Delete the monster (if any) */
					delete_monster(y, x);
				}
			}

			/* During generation, destroyed artifacts are "preserved" */
			/*:::未鑑定、もしくはダンジョン生成中に★が破壊されたとき再生成可能にする*/
			if (preserve_mode || in_generate)
			{
				s16b this_o_idx, next_o_idx = 0;

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Hack -- Preserve unknown artifacts */
					if (object_is_fixed_artifact(o_ptr) && (!object_is_known(o_ptr) || in_generate))
					{
						/* Mega-Hack -- Preserve the artifact */
						a_info[o_ptr->name1].cur_num = 0;

						if (in_generate && cheat_peek)
						{
							char o_name[MAX_NLEN];
							object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));
#ifdef JP
							msg_format("伝説のアイテム (%s) は生成中に*破壊*された。", o_name);
#else
							msg_format("Artifact (%s) was *destroyed* during generation.", o_name);
#endif
						}
					}
					else if (in_generate && cheat_peek && o_ptr->art_name)
					{
#ifdef JP
						msg_print("ランダム・アーティファクトの1つは生成中に*破壊*された。");
#else
						msg_print("One of the random artifacts was *destroyed* during generation.");
#endif
					}
				}
			}

			/* Delete objects */
			delete_object(y, x);

			/* Destroy "non-permanent" grids */
			/*:::永久壁以外の地形は岩か床になる*/
			if (!cave_perma_grid(c_ptr))
			{
				/* Wall (or floor) type */
				t = randint0(200);

				if (!in_generate) /* Normal */
				{
					if(force_floor)
					{
						/* Create floor */
						cave_set_feat(y, x, floor_type[randint0(100)]);
					}
					else if (t < 20)
					{
						/* Create granite wall */
						cave_set_feat(y, x, feat_granite);
					}
					else if (t < 70)
					{
						/* Create quartz vein */
						cave_set_feat(y, x, feat_quartz_vein);
					}
					else if (t < 100)
					{
						/* Create magma vein */
						cave_set_feat(y, x, feat_magma_vein);
					}
					else
					{
						/* Create floor */
						cave_set_feat(y, x, floor_type[randint0(100)]);
					}
				}
				else /* In generation */
				{
					if (t < 20)
					{
						/* Create granite wall */
						place_extra_grid(c_ptr);
					}
					else if (t < 70)
					{
						/* Create quartz vein */
						c_ptr->feat = feat_quartz_vein;
					}
					else if (t < 100)
					{
						/* Create magma vein */
						c_ptr->feat = feat_magma_vein;
					}
					else
					{
						/* Create floor */
						place_floor_grid(c_ptr);
					}

					/* Clear garbage of hidden trap or door */
					c_ptr->mimic = 0;
				}
			}
		}
	}

	if (!in_generate)
	{
		/* Process "re-glowing" */
		/*:::照らされたところを新たに光らせる？*/
		for (y = (y1 - r); y <= (y1 + r); y++)
		{
			for (x = (x1 - r); x <= (x1 + r); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k > r) continue;

				/* Access the grid */
				c_ptr = &cave[y][x];

				if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
				else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
				{
					int i, yy, xx;
					cave_type *cc_ptr;

					for (i = 0; i < 9; i++)
					{
						yy = y + ddy_ddd[i];
						xx = x + ddx_ddd[i];
						if (!in_bounds2(yy, xx)) continue;
						cc_ptr = &cave[yy][xx];
						if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							c_ptr->info |= CAVE_GLOW;
							break;
						}
					}
				}
			}
		}

		/* Hack -- Affect player */
		/*:::メッセージと盲目処理*/
		if (flag && !flan && !force_floor)
		{
			/* Message */
#ifdef JP
			msg_print("燃えるような閃光が発生した！");
#else
			msg_print("There is a searing blast of light!");
#endif

			/* Blind the player */
			if (!p_ptr->resist_blind && !p_ptr->resist_lite)
			{
				/* Become blind */
				(void)set_blind(p_ptr->blind + 10 + randint1(10));
			}
		}
		else if(force_floor) msg_print(_("大地に門が開き、全てを呑み込んだ！", "A gate opens up in the ground, swallowing everything!"));

		/*:::フロアの全マスのdist,cost,whenをクリアしてるらしい*/
		forget_flow();

		if(flag_kaguya_away)msg_print(_("輝夜がどこかへ吹き飛ばされてしまった！", "Oh no! Kaguya was blown away to somewhere!"));

		if(one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);

		/* Mega-Hack -- Forget the view and lite */
		p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		if (p_ptr->special_defense & NINJA_S_STEALTH)
		{
			if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
		}


	}

	/* Success */
	return (TRUE);
}


/*
 * Induce an "earthquake" of the given radius at the given location.
 *
 * This will turn some walls into floors and some floors into walls.
 *
 * The player will take damage and "jump" into a safe grid if possible,
 * otherwise, he will "tunnel" through the rubble instantaneously.
 *
 * Monsters will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when genocided.
 *
 * Note that thus the player and monsters (except eaters of walls and
 * passers through walls) will never occupy the same grid as a wall.
 * Note that as of now (2.7.8) no monster may occupy a "wall" grid, even
 * for a single turn, unless that monster can pass_walls or kill_walls.
 * This has allowed massive simplification of the "monster" code.
 */
/*:::地震処理　詳細未読
 *:::範囲内の地形をランダムな壁と床にし、壁になった地形に＠やモンスターが居たらダメージか退避処理
 *:::（壁抜け可能なキャラクターを除く）
 *:::r:半径 m_idx:モンスターが地震を起こしたときそのモンスターのインデックス　＠は0*/
//v2.0 r値を上限30にした。map[][]を32→64に
//v2.1 modeパラメータを追加
// 0:通常
// 1:クエストダンジョン内でも使用可能
bool earthquake_aux(int cy, int cx, int r, int m_idx, int mode)
{
	int             i, t, y, x, yy, xx, dy, dx;
	int             damage = 0;
	int             sn = 0, sy = 0, sx = 0;
	bool            hurt = FALSE;
	cave_type       *c_ptr;
	bool            map[64][64];

	if(p_ptr->pclass == CLASS_TENSHI && kanameishi_check(randint1(r))) return (FALSE);

	/* Prevent destruction of quest levels and town */
	if (mode != 1 && ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR))
	{
		return (FALSE);
	}

	/* Paranoia -- Enforce maximum range */
	if (r > 30) r = 30;

	if (cheat_xtra) msg_format("earthquake:rad%d", r);

	/* Clear the "maximal blast" area */
	for (y = 0; y < 64; y++)
	{
		for (x = 0; x < 64; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			int idx, idy;

			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];
			/*:::vault荒らし防止したいならこの辺か*/
			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK | CAVE_KNOWN);

			/* Skip the epicenter */
			if (!dx && !dy) continue;

			/* Skip most grids */
			if (randint0(100) < 85) continue;

			if (player_bold(yy, xx) && (p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI))
			{
				msg_print(_("落石はあなたを傷つけなかった。", "The falling rocks didn't harm you."));
				continue;
			}
			else if (player_bold(yy, xx) && p_ptr->pclass == CLASS_ICHIRIN && one_in_(p_ptr->do_martialarts? 2:4))
			{
				msg_print(_("雲山が落石からあなたを守った。", "Unzan protects you from falling rocks."));
				continue;
			}

			idy = 32 + yy - cy;
			idx = 32 + xx - cx;

			if (idx < 0 || idx >= 64 || idy < 0 || idy >= 64)
			{
				msg_format(_("ERROR:earthquake_aux()で配列外のXY(%d,%d)",
                            "ERROR: Out of bounds XY (%d, %d) in earthquake_aux()"), idx, idy);
				return FALSE;
			}

			/* Damage this grid */
			map[idy][idx] = TRUE;

			/* Hack -- Take note of player damage */
			if (player_bold(yy, xx)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if (hurt && !p_ptr->pass_wall && !p_ptr->kill_wall)
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Access the location */
			y = py + ddy_ddd[i];
			x = px + ddx_ddd[i];

			/* Skip non-empty grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Important -- Skip "quake" grids */
			if (map[32+y-cy][32+x-cx]) continue;

			if (cave[y][x].m_idx) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if (randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}

		/* Random message */
		switch (randint1(3))
		{
			case 1:
			{
#ifdef JP
				msg_print("ダンジョンの壁が崩れた！");
#else
				msg_print("The cave ceiling collapses!");
#endif
				break;
			}
			case 2:
			{
#ifdef JP
				msg_print("ダンジョンの床が不自然にねじ曲がった！");
#else
				msg_print("The cave floor twists in an unnatural way!");
#endif
				break;
			}
			default:
			{
#ifdef JP
				msg_print("ダンジョンが揺れた！崩れた岩が頭に降ってきた！");
#else
				msg_print("The cave quakes!  You are pummeled with debris!");
#endif
				break;
			}
		}

		if(p_ptr->mimic_form == MIMIC_MIST)
		{
			msg_print(_("あなたは霧になって落石をかわした。", "You turned into mist and slipped through the falling rocks."));
			damage = 0;
		}
		//諏訪子冬眠中　通常無敵だが一部攻撃は通る
		else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
		{
			msg_print(_("落石はあなたがいる場所までは届かなかった。", "The falling rocks did not reach where you were."));
			damage = 0;
		}

		/* Hurt the player a lot */
		else if (!sn)
		{
			/* Message and damage */
#ifdef JP
			msg_print("逃げ場がない！あなたは巨岩の下敷きになった！");
#else
			msg_print("There's nowhere to run! You get pinned under a massive rock!");
#endif
			damage = 200;
			//v2.0 逃げ場がない大ダメージのときはスタンも増やそう
			(void)set_stun(p_ptr->stun + 30 + randint1(30));
		}

		/* Destroy the grid, and push the player to safety */
		else
		{
			/* Calculate results */
			switch (randint1(3))
			{
				case 1:
				{
#ifdef JP
					msg_print("降り注ぐ岩をうまく避けた！");
#else
					msg_print("You nimbly dodge the blast!");
#endif
					damage = 0;
					break;
				}
				case 2:
				{
#ifdef JP
					msg_print("岩石があなたに直撃した!");
#else
					msg_print("You are bashed by rubble!");
#endif
					//v2.0 スタン値が増加するのとダメージが大きいのに分けてみる
					damage = damroll(10, 4);
					(void)set_stun(p_ptr->stun + 15 + randint1(15));
					break;
				}
				case 3:
				{
#ifdef JP
					msg_print("あなたは床と壁との間に挟まれてしまった！");
#else
					msg_print("You are crushed between the floor and ceiling!");
#endif
					damage = damroll(15, 15);
					//(void)set_stun(p_ptr->stun + randint1(50));
					break;
				}
			}

			/* Move the player to the safe location */
			(void)move_player_effect(sy, sx, MPE_DONT_PICKUP);
		}

		/* Important -- no wall on player */
		map[32+py-cy][32+px-cx] = FALSE;

		/* Take some damage */
		if (damage)
		{
			char death_reason[160] = "";
			//char *killer;

			if (m_idx)
			{
				char m_name[80];
				monster_type *m_ptr = &m_list[m_idx];

				/* Get the monster's real name */
				monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

				//v1.1.92 take_hit()にformat()の戻り値を渡したらまずいらしいので修正
				sprintf(death_reason, _("%sの起こした地震", "an earthquake caused by %s"), m_name);
				//killer = format("%sの起こした地震", m_name);
			}
			else
			{
				sprintf(death_reason, _("地震", "an earthquake"));
            }

			take_hit(DAMAGE_ATTACK, damage, death_reason, -1);
		}
	}

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[32+yy-cy][32+xx-cx]) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (c_ptr->m_idx == p_ptr->riding) continue;

			/* Process monsters */
			if (c_ptr->m_idx)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Quest monsters */
				if (r_ptr->flags1 & RF1_QUESTOR)
				{
					/* No wall on quest monsters */
					map[32+yy-cy][32+xx-cx] = FALSE;

					continue;
				}
				///mod140712
				/*:::☆の付喪神は地震で埋まらない*/
				if((m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 ) && object_is_artifact(&o_list[m_ptr->hold_o_idx]))
				{
					map[32+yy-cy][32+xx-cx] = FALSE;
					continue;
				}
				//輝夜も地震で埋まらない
				if((m_ptr->r_idx == MON_MASTER_KAGUYA))
				{
					map[32+yy-cy][32+xx-cx] = FALSE;
					continue;
				}

				//v1.1.99 すでに倒れているモンスターは埋まらないことにした(アンバーの王族の血の呪い)
				if (m_ptr->hp < 0)
				{
					map[32 + yy - cy][32 + xx - cx] = FALSE;
					if (cheat_xtra) msg_print(_("TESTMSG:埋まらない", "TESTMSG: Don't get buried"));
					continue;
				}

				/* Most monsters cannot co-exist with rock */
				if (!(r_ptr->flags2 & (RF2_KILL_WALL)) &&
				    !(r_ptr->flags2 & (RF2_PASS_WALL)))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (is_glyph_grid(&cave[y][x])) continue;
							if (is_explosive_rune_grid(&cave[y][x])) continue;

							/* ... nor on the Pattern */
							if (pattern_tile(y, x)) continue;

							/* Important -- Skip "quake" grids */
							if (map[32+y-cy][32+x-cx]) continue;

							if (cave[y][x].m_idx) continue;
							if (player_bold(y, x)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^sは悲鳴を上げた！", m_name);
#else
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s wails out in pain!", m_name);
#endif

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					(void)set_monster_csleep(c_ptr->m_idx, 0);

					/* Apply damage directly */
					m_ptr->hp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
#ifdef JP
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^sは岩石に埋もれてしまった！", m_name);
#else
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s is embedded in the rock!", m_name);
#endif

						if (c_ptr->m_idx)
						{
							if (record_named_pet && is_pet(&m_list[c_ptr->m_idx]) && m_list[c_ptr->m_idx].nickname)
							{
								char m2_name[80];

								monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
								do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_EARTHQUAKE, m2_name);
							}
						}

						//v2.1.1 ウバメの特技でクエストダンジョンでも地震を起こせるようにしたので追加
						check_quest_completion(m_ptr);

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						int m_idx = cave[yy][xx].m_idx;

						/* Update the old location */
						cave[yy][xx].m_idx = 0;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the monster (new location) */
						update_mon(m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(yy, xx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					}
				}
			}
		}
	}

	/* Lose monster light */
	clear_mon_lite();

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[32+yy-cy][32+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &cave[yy][xx];

			/* Paranoia -- never affect player */
/*			if (player_bold(yy, xx)) continue; */

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = cave_have_flag_bold(yy, xx, FF_PROJECT) ? randint0(100) : 200;

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(yy, xx, feat_granite);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(yy, xx, feat_quartz_vein);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(yy, xx, feat_magma_vein);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(yy, xx, floor_type[randint0(100)]);
				}
			}
		}
	}


	/* Process "re-glowing" */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < 9; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if (!in_bounds2(yyy, xxx)) continue;
					cc_ptr = &cave[yyy][xxx];
					if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
					{
						c_ptr->info |= CAVE_GLOW;
						break;
					}
				}
			}
		}
	}


	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
	}

	/* Success */
	return (TRUE);
}

bool earthquake(int cy, int cx, int r)
{
	return earthquake_aux(cy, cx, r, 0, 0);
}


void discharge_minion(void)
{
	int i;
	bool okay = TRUE;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
		if (m_ptr->nickname) okay = FALSE;
	}
	if (!okay || p_ptr->riding)
	{
#ifdef JP
		if (!get_check("本当に全ペットを爆破しますか？"))
#else
		if (!get_check("You will blast all pets. Are you sure? "))
#endif
			return;
	}
	for (i = 1; i < m_max; i++)
	{
		int dam;
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;

		if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		/* Uniques resist discharging */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0x00);
#ifdef JP
			msg_format("%sは爆破されるのを嫌がり、勝手に自分の世界へと帰った。", m_name);
#else
			msg_format("%^s resists to be blasted, and run away.", m_name);
#endif
			delete_monster_idx(i);
			continue;
		}
		dam = m_ptr->maxhp / 2;
		if (dam > 100) dam = (dam-100)/2 + 100;
		if (dam > 400) dam = (dam-400)/2 + 400;
		if (dam > 800) dam = 800;
		project(i, 2+(r_ptr->level/20), m_ptr->fy,
			m_ptr->fx, dam, GF_PLASMA,
			PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);

		if (record_named_pet && m_ptr->nickname)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_BLAST, m_name);
		}

		delete_monster_idx(i);
	}
}


/*
 * This routine clears the entire "temp" set.
 *
 * This routine will Perma-Lite all "temp" grids.
 *
 * This routine is used (only) by "lite_room()"
 *
 * Dark grids are illuminated.
 *
 * Also, process all affected monsters.
 *
 * SMART monsters always wake up when illuminated
 * NORMAL monsters wake up 1/4 the time when illuminated
 * STUPID monsters wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(void)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		cave_type *c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update only non-CAVE_GLOW grids */
		/* if (c_ptr->info & (CAVE_GLOW)) continue; */

		/* Perma-Lite */
		c_ptr->info |= (CAVE_GLOW);

		/* Process affected monsters */
		if (c_ptr->m_idx)
		{
			int chance = 25;

			monster_type    *m_ptr = &m_list[c_ptr->m_idx];

			monster_race    *r_ptr = &r_info[m_ptr->r_idx];

			/* Update the monster */
			update_mon(c_ptr->m_idx, FALSE);

			/* Stupid monsters rarely wake up */
			if (r_ptr->flags2 & (RF2_STUPID)) chance = 10;

			/* Smart monsters always wake up */
			if (r_ptr->flags2 & (RF2_SMART)) chance = 100;

			/* Sometimes monsters wake up */
			if (MON_CSLEEP(m_ptr) && (randint0(100) < chance))
			{
				/* Wake up! */
				(void)set_monster_csleep(c_ptr->m_idx, 0);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					char m_name[80];

					/* Acquire the monster name */
					monster_desc(m_name, m_ptr, 0);
					/* Dump a message */
#ifdef JP
					msg_format("%^sが目を覚ました。", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}
		}

		/* Note */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);

		update_local_illumination(y, x);
	}

	/* None left */
	temp_n = 0;
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will "darken" all "temp" grids.
 *
 * In addition, some of these grids will be "unmarked".
 *
 * This routine is used (only) by "unlite_room()"
 *
 * Also, process all affected monsters
 */
static void cave_temp_room_unlite(void)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];
		int j;

		cave_type *c_ptr = &cave[y][x];
		bool do_dark = !is_mirror_grid(c_ptr);

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Darken the grid */
		if (do_dark)
		{
			if (dun_level || !is_daytime())
			{
				for (j = 0; j < 9; j++)
				{
					int by = y + ddy_ddd[j];
					int bx = x + ddx_ddd[j];

					if (in_bounds2(by, bx))
					{
						cave_type *cc_ptr = &cave[by][bx];

						if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							do_dark = FALSE;
							break;
						}
					}
				}

				if (!do_dark) continue;
			}

			c_ptr->info &= ~(CAVE_GLOW);

			/* Hack -- Forget "boring" grids */
			if (!have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_REMEMBER))
			{
				/* Forget the grid */
				if (!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

				/* Notice */
				note_spot(y, x);
			}

			/* Process affected monsters */
			if (c_ptr->m_idx)
			{
				/* Update the monster */
				update_mon(c_ptr->m_idx, FALSE);
			}

			/* Redraw */
			lite_spot(y, x);

			update_local_illumination(y, x);
		}
	}

	/* None left */
	temp_n = 0;
}


/*
 * Determine how much contiguous open space this grid is next to
 */
static int next_to_open(int cy, int cx, bool (*pass_bold)(int, int))
{
	int i;

	int y, x;

	int len = 0;
	int blen = 0;

	for (i = 0; i < 16; i++)
	{
		y = cy + ddy_cdd[i % 8];
		x = cx + ddx_cdd[i % 8];

		/* Found a wall, break the length */
		if (!pass_bold(y, x))
		{
			/* Track best length */
			if (len > blen)
			{
				blen = len;
			}

			len = 0;
		}
		else
		{
			len++;
		}
	}

	return (MAX(len, blen));
}


static int next_to_walls_adj(int cy, int cx, bool (*pass_bold)(int, int))
{
	int i;

	int y, x;

	int c = 0;

	for (i = 0; i < 8; i++)
	{
		y = cy + ddy_ddd[i];
		x = cx + ddx_ddd[i];

		if (!pass_bold(y, x)) c++;
	}

	return c;
}


/*
 * Aux function -- see below
 */
static void cave_temp_room_aux(int y, int x, bool only_room, bool (*pass_bold)(int, int))
{
	cave_type *c_ptr;

	/* Get the grid */
	c_ptr = &cave[y][x];

	/* Avoid infinite recursion */
	if (c_ptr->info & (CAVE_TEMP)) return;

	/* Do not "leave" the current room */
	if (!(c_ptr->info & (CAVE_ROOM)))
	{
		if (only_room) return;

		/* Verify */
		if (!in_bounds2(y, x)) return;

		/* Do not exceed the maximum spell range */
		if (distance(py, px, y, x) > MAX_RANGE) return;

		/* Verify this grid */
		/*
		 * The reason why it is ==6 instead of >5 is that 8 is impossible
		 * due to the check for cave_bold above.
		 * 7 lights dead-end corridors (you need to do this for the
		 * checkboard interesting rooms, so that the boundary is lit
		 * properly.
		 * This leaves only a check for 6 bounding walls!
		 */
		if (in_bounds(y, x) && pass_bold(y, x) &&
		    (next_to_walls_adj(y, x, pass_bold) == 6) && (next_to_open(y, x, pass_bold) <= 1)) return;
	}

	/* Paranoia -- verify space */
	if (temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

/*
 * Aux function -- see below
 */
static bool cave_pass_lite_bold(int y, int x)
{
	return cave_los_bold(y, x);
}

/*
 * Aux function -- see below
 */
static void cave_temp_lite_room_aux(int y, int x)
{
	cave_temp_room_aux(y, x, FALSE, cave_pass_lite_bold);
}

/*
 * Aux function -- see below
 */
static bool cave_pass_dark_bold(int y, int x)
{
	return cave_have_flag_bold(y, x, FF_PROJECT);
}

/*
 * Aux function -- see below
 */
static void cave_temp_unlite_room_aux(int y, int x)
{
	cave_temp_room_aux(y, x, TRUE, cave_pass_dark_bold);
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(int y1, int x1)
{
	int i, x, y;

	if (p_ptr->wizard) msg_format("temp_n:%d", temp_n);
	//v1.1.66 特定条件のグリッドをカウントするtemp_n,temp_x[],temp_y[]をリセットしておく
	temp_n = 0;


	/* Add the initial grid */
	cave_temp_lite_room_aux(y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if (!cave_pass_lite_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_lite_room_aux(y + 1, x);
		cave_temp_lite_room_aux(y - 1, x);
		cave_temp_lite_room_aux(y, x + 1);
		cave_temp_lite_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_lite_room_aux(y + 1, x + 1);
		cave_temp_lite_room_aux(y - 1, x - 1);
		cave_temp_lite_room_aux(y - 1, x + 1);
		cave_temp_lite_room_aux(y + 1, x - 1);
	}

	/* Now, lite them all up at once */
	cave_temp_room_lite();

	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
	}
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(int y1, int x1)
{
	int i, x, y;

	if (p_ptr->wizard) msg_format("temp_n:%d", temp_n);
	//v1.1.66 特定条件のグリッドをカウントするtemp_n,temp_x[],temp_y[]をリセットしておく
	temp_n = 0;

	/* Add the initial grid */
	cave_temp_unlite_room_aux(y1, x1);

	/* Spread, breadth first */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get dark, but stop darkness */
		if (!cave_pass_dark_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_unlite_room_aux(y + 1, x);
		cave_temp_unlite_room_aux(y - 1, x);
		cave_temp_unlite_room_aux(y, x + 1);
		cave_temp_unlite_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_unlite_room_aux(y + 1, x + 1);
		cave_temp_unlite_room_aux(y - 1, x - 1);
		cave_temp_unlite_room_aux(y - 1, x + 1);
		cave_temp_unlite_room_aux(y + 1, x - 1);
	}

	/* Now, darken them all at once */
	cave_temp_room_unlite();
}



/*
 * Hack -- call light around the player
 * Affect all monsters in the projection radius
 */
/*:::ライトエリア*/
bool lite_area(int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS)
	{
#ifdef JP
		msg_print("ダンジョンが光を吸収した。");
#else
		msg_print("The darkness of this dungeon absorb your light.");
#endif
		return FALSE;
	}

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
#ifdef JP
msg_print("白い光が辺りを覆った。");
#else
		msg_print("You are surrounded by a white light.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(0, rad, py, px, dam, GF_LITE_WEAK, flg, -1);

	/* Lite up the room */
	lite_room(py, px);

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all monsters in the projection radius
 */
/*:::暗闇生成　詳細未読*/
bool unlite_area(int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
#ifdef JP
msg_print("暗闇が辺りを覆った。");
#else
		msg_print("Darkness surrounds you.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(0, rad, py, px, dam, GF_DARK_WEAK, flg, -1);

	/* Lite up the room */
	unlite_room(py, px);

	/* Assume seen */
	return (TRUE);
}



/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
/*:::ボール魔法を打つ。radが負だとブレスになる*/
bool fire_ball(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	if (typ == GF_CONTROL_LIVING) flg|= PROJECT_HIDE;
	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_rocket(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
/*:::ボール系汎用ルーチン　詳細未読*/
bool fire_ball_hide(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}
/*:::軌跡のないボール発射ルーチンを追加　召雷とか*/
///mod140906 視界外でも発生するようになっていたので修正。
//失敗のとき行動順を消費しない判定のためにFALSEを返すようにする。また効果用メッセージを引数として渡す。
bool fire_ball_jump(int typ, int dir, int dam, int rad, cptr msg)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}
	if(!in_bounds(ty,tx) || !projectable(py,px,ty,tx))
	{
		msg_print(_("視界内の一点を狙わないといけない。", "You have to aim at location in your sight."));
		return (FALSE);
	}
	//v1.1.53 壁の中は狙えないことにした(半径ありのボールだと壁の向こうを一方的に攻撃できるので)
	if(rad > 0 && cave_have_flag_bold(ty,tx,FF_WALL))
	{
		msg_print(_("壁の中を狙うことはできない。", "You cannot aim at a wall."));
		return (FALSE);
	}

	if(msg) msg_format("%s",msg);

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	project(0, rad, ty, tx, dam, typ, flg, -1);
	return (TRUE);
}


/*
 * Cast a meteor spell, defined as a ball spell cast by an arbitary monster,
 * player, or outside source, that starts out at an arbitrary location, and
 * leaving no trail from the "caster" to the target.  This function is
 * especially useful for bombardments and similar. -LM-
 *
 * Option to hurt the player.
 */
bool fire_meteor(int who, int typ, int y, int x, int dam, int rad)
{
	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Analyze the "target" and the caster. */
	return (project(who, rad, y, x, dam, typ, flg, -1));
}


bool fire_blast(int typ, int dir, int dd, int ds, int num, int dev, int flg_add)
{
	int ly, lx, ld;
	int ty, tx, y, x;
	int i;
	/* Assume okay */
	bool result = TRUE;

	///mod160503 ARROWを反射されなくした
	int flg = PROJECT_FAST | PROJECT_THRU | PROJECT_STOP | PROJECT_KILL | PROJECT_GRID;
	if (typ != GF_ARROW) flg |= PROJECT_REFLECTABLE;

	///mod140202 フラグを変更できるようにした。レーザーにしたいときなどに。
	if(flg_add != 0) flg = flg_add;

	/* Use the given direction */
	if (dir != 5)
	{
		ly = ty = py + 20 * ddy[dir];
		lx = tx = px + 20 * ddx[dir];
	}

	/* Use an actual "target" */
	else /* if (dir == 5) */
	{
		tx = target_col;
		ty = target_row;

		lx = 20 * (tx - px) + px;
		ly = 20 * (ty - py) + py;
	}

	ld = distance(py, px, ly, lx);

	/* Blast */
	for (i = 0; i < num; i++)
	{
		while (1)
		{
			/* Get targets for some bolts */
			y = rand_spread(ly, ld * dev / 20);
			x = rand_spread(lx, ld * dev / 20);

			if (distance(ly, lx, y, x) <= ld * dev / 20) break;
		}

		/* Analyze the "dir" and the "target". */
		if (!project(0, 0, y, x, damroll(dd, ds), typ, flg, -1))
		{
			result = FALSE;
		}
	}

	return (result);
}


/*
 * Switch position with a monster.
 */
/*:::位置交換*/
bool teleport_swap(int dir)
{
	int tx, ty;
	cave_type * c_ptr;
	monster_type * m_ptr;
	monster_race * r_ptr;

	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}
	else
	{
		tx = px + ddx[dir];
		ty = py + ddy[dir];
	}
	c_ptr = &cave[ty][tx];

	if (p_ptr->anti_tele)
	{
#ifdef JP
msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	if (!c_ptr->m_idx || (c_ptr->m_idx == p_ptr->riding))
	{
#ifdef JP
msg_print("それとは場所を交換できません。");
#else
		msg_print("You can't trade places with that!");
#endif


		/* Failure */
		return FALSE;
	}

	if ((c_ptr->info & CAVE_ICKY) || (distance(ty, tx, py, px) > p_ptr->lev * 3 / 2 + 10))
	{
#ifdef JP
msg_print("失敗した。");
#else
		msg_print("Failed to swap.");
#endif


		/* Failure */
		return FALSE;
	}

	m_ptr = &m_list[c_ptr->m_idx];
	r_ptr = &r_info[m_ptr->r_idx];

	(void)set_monster_csleep(c_ptr->m_idx, 0);

	if (r_ptr->flagsr & RFR_RES_TELE)
	{
#ifdef JP
		msg_print("位置交換を邪魔された！");
#else
		msg_print("Your teleportation is blocked!");
#endif

		if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;

		/* Failure */
		return FALSE;
	}

	sound(SOUND_TELEPORT);

	/* Swap the player and monster */
	(void)move_player_effect(ty, tx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);

	/* Success */
	return TRUE;
}


/*
 * Hack -- apply a "projection()" in a direction (or at the target)
 */
bool project_hook(int typ, int dir, int dam, int flg)
{
	int tx, ty;

	/* Pass through the target if needed */
	flg |= (PROJECT_THRU);

	/* Use the given direction */
	tx = px + ddx[dir];
	ty = py + ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target", do NOT explode */
	return (project(0, 0, ty, tx, dam, typ, flg, -1));
}



///mod140328 大型レーザー
/*:::大型レーザー専用 radは1か2*/
bool fire_spark(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_FAST | PROJECT_MASTERSPARK;
	if(typ == GF_DISINTEGRATE) flg |= PROJECT_DISI;

	if(rad<1) rad = 1;
	else if(rad>2) rad = 2;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	///mod140907 自分に撃ったらフリーズするため修正
	if(tx == px && ty == py)
		return (project(0, rad, ty, tx, dam, typ, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1));
	else
		return (project(0, rad, ty, tx, dam, typ, flg, -1));
}

/*
 * Cast a bolt spell.
 * Stop if we hit a monster, as a "bolt".
 * Affect monsters and grids (not objects).
 */
bool fire_bolt(int typ, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_GRID;
	if (typ != GF_ARROW && typ != GF_SPECIAL_SHOT) flg |= PROJECT_REFLECTABLE;
	return (project_hook(typ, dir, dam, flg));
}


/*
 * Cast a beam spell.
 * Pass through monsters, as a "beam".
 * Affect monsters, grids and objects.
 */

bool fire_beam(int typ, int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(typ, dir, dam, flg));
}


/*
 * Cast a bolt spell, or rarely, a beam spell
 */
bool fire_bolt_or_beam(int prob, int typ, int dir, int dam)
{
	if (randint0(100) < prob)
	{
		return (fire_beam(typ, dir, dam));
	}
	else
	{
		return (fire_bolt(typ, dir, dam));
	}
}

//v1.1.21 視界内のランダムなターゲットを設定する
bool get_random_target(int mode, int range)
{
	int i;
	monster_type *m_ptr;
	int tmp_idx_cnt=0;
	int target_who_tmp=0;

	if(m_max < 2) return (FALSE);
	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr)) continue;
		if (is_friendly(m_ptr)) continue;
		if (!m_ptr->ml) continue;
		if (!projectable(py, px, m_ptr->fy,m_ptr->fx)) continue;
		//if (!los(py, px, m_ptr->fy,m_ptr->fx)) continue;
		if (range && m_ptr->cdis > range) continue;

		tmp_idx_cnt++;
		if(one_in_(tmp_idx_cnt)) target_who_tmp = i;
	}
	if(!tmp_idx_cnt) return (FALSE);

	target_who = target_who_tmp;
	m_ptr = &m_list[target_who];
	target_row = m_ptr->fy;
	target_col = m_ptr->fx;

	return TRUE;

}

///mod140222
/* 視界内の敵をランダムに選定しそれに対して攻撃を放つ。＠が幻覚や盲目でも影響しない。*/
/* ペットや味方は狙わないが巻き込む。*/
/* method:1-ボルト 2-ビーム 3-ボール 4-軌跡の出ないボール*/
/* 5-大凶おみくじ　ボールだが自分も食らう 6-OPTフラグ設定ビーム
/* 7-一番近くの敵のみを狙う
/* range:@との距離 0なら通常視界範囲*/
/* 視界内にターゲットがいないときFALSE*/
/* これを連打したとき途中でモンスターが全て倒れてFALSEが返ってもちゃんと行動順消費するよう気をつける*/
bool fire_random_target(int typ, int dam, int method, int rad, int range)
{
	int i;
	monster_type *m_ptr;
	int tmp_idx_cnt=0;
	int target_who_tmp=0;
	int min_dist = 999;

	if(m_max < 2) return (FALSE);
	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr)) continue;
		if (is_friendly(m_ptr)) continue;
		if (!m_ptr->ml) continue;
		if (!projectable(py, px, m_ptr->fy,m_ptr->fx)) continue;
		//if (!los(py, px, m_ptr->fy,m_ptr->fx)) continue;
		if (range && m_ptr->cdis > range) continue;

		//v1.1.53 ボールが直接発生する系の攻撃は壁の中に出ない
		if (method == 4 && rad > 0 && cave_have_flag_bold(m_ptr->fy, m_ptr->fx, FF_WALL)) continue;

		//一番距離の近い敵からランダムに選定
		if (method == 7)
		{
			if (m_ptr->cdis > min_dist)
				continue;
			else if (m_ptr->cdis == min_dist)
			{
				tmp_idx_cnt++;
				if (one_in_(tmp_idx_cnt)) target_who_tmp = i;
			}
			else
			{
				min_dist = m_ptr->cdis;
				tmp_idx_cnt = 1;
				target_who_tmp = i;
			}
		}
		//視界内からランダムに選定
		else
		{
			tmp_idx_cnt++;
			if (one_in_(tmp_idx_cnt)) target_who_tmp = i;
		}

	}
	if(!tmp_idx_cnt) return (FALSE);

	target_who = target_who_tmp;
	m_ptr = &m_list[target_who];
	target_row = m_ptr->fy;
	target_col = m_ptr->fx;


	if(method==1 || method == 7) return fire_bolt(typ,5,dam);
	else if(method==2)
	{
		if(range)	project_length = range;
		return fire_beam(typ,5,dam);
	}
	else if(method==3) return fire_ball(typ,5,dam,rad);
	else if(method==4) return fire_ball_jump(typ,5,dam,rad,NULL);
	else if(method == 5)
	{
		int tx,ty;
		int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

		if (target_okay())
		{
			flg &= ~(PROJECT_STOP);
			tx = target_col;
			ty = target_row;
			return project(PROJECT_WHO_OMIKUJI_HELL, rad, ty, tx, dam, typ, flg, -1);
		}

	}
	else if(method == 6)
	{
		int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_OPT;
		return (project_hook(typ, 5, dam, flg));
	}
	else
	{
		msg_format(_("ERROR:fire_random_targetが変なmethod(%d)で呼ばれた", "ERROR:fire_random_target() was called with an invalid method, %d"), method);
	}
	return (FALSE);

}


/*
 * Some of the old functions
 */
bool lite_line(int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (project_hook(GF_LITE_WEAK, dir, dam, flg));
}


bool drain_life(int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_DRAIN, dir, dam, flg));
}


bool wall_to_mud(int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(GF_KILL_WALL, dir, dam, flg));
}


bool wizard_lock(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(GF_JAM_DOOR, dir, 20 + randint1(30), flg));
}


bool destroy_door(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_DOOR, dir, 0, flg));
}


bool disarm_trap(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_TRAP, dir, 0, flg));
}


bool heal_monster(int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_HEAL, dir, dam, flg));
}


bool speed_monster(int dir, int power)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SPEED, dir, power, flg));
}


bool slow_monster(int dir, int power)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLOW, dir, power, flg));
}


bool sleep_monster(int dir, int power)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLEEP, dir, power, flg));
}

/*:::金縛り*/
bool stasis_monster(int dir)
{
	return (fire_ball_hide(GF_STASIS, dir, p_ptr->lev*2, 0));
}


bool stasis_evil(int dir)
{
	return (fire_ball_hide(GF_STASIS_EVIL, dir, p_ptr->lev*2, 0));
}


bool confuse_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CONF, dir, plev, flg));
}


bool stun_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_STUN, dir, plev, flg));
}


bool poly_monster(int dir, int power)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	bool tester = (project_hook(GF_OLD_POLY, dir, power, flg));
	//if (tester)
	//	chg_virtue(V_CHANCE, 1);
	return(tester);
}


bool clone_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CLONE, dir, 0, flg));
}


bool fear_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_TURN_ALL, dir, plev, flg));
}


bool death_ray(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_DEATH_RAY, dir, plev * 200, flg));
}


bool teleport_monster(int dir, int distance)
{
	int flg = PROJECT_BEAM | PROJECT_KILL;
	return (project_hook(GF_AWAY_ALL, dir, distance, flg));
}

/*
 * Hooks -- affect adjacent grids (radius 1 ball attack)
 */
///mod140327 半径設定するようにした
bool door_creation(int rad)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, rad, py, px, 0, GF_MAKE_DOOR, flg, -1));
}


bool trap_creation(int y, int x)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, y, x, 0, GF_MAKE_TRAP, flg, -1));
}


bool tree_creation(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_MAKE_TREE, flg, -1));
}


bool glyph_creation(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM;
	return (project(0, 1, py, px, 0, GF_MAKE_GLYPH, flg, -1));
}


bool wall_stone(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;

	bool dummy = (project(0, 1, py, px, 0, GF_STONE_WALL, flg, -1));

	/* Update stuff */
	p_ptr->update |= (PU_FLOW);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	return dummy;
}


bool destroy_doors_touch(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_KILL_DOOR, flg, -1));
}

bool disarm_traps_touch(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_KILL_TRAP, flg, -1));
}

bool sleep_monsters_touch(void)
{
	int flg = PROJECT_KILL | PROJECT_HIDE;
	return (project(0, 1, py, px, p_ptr->lev, GF_OLD_SLEEP, flg, -1));
}

///sysdel 死人返し
bool animate_dead(int who, int y, int x)
{
	int flg = PROJECT_ITEM | PROJECT_HIDE;
	return (project(who, 5, y, x, 0, GF_ANIM_DEAD, flg, -1));
}


/*混沌招来*/
///mod140211 威力や動作を少し変更
void call_chaos(void)
{
	project_hack2(0, 1, 777, 0);

/*

	int Chaos_type, dummy, dir;
	int plev = p_ptr->lev;
	bool line_chaos = FALSE;
	int hurt_types[31] =
	{
		GF_ELEC,      GF_POIS,    GF_ACID,    GF_COLD,
		GF_FIRE,      GF_MISSILE, GF_ARROW,   GF_PLASMA,
		GF_HOLY_FIRE, GF_WATER,   GF_LITE,    GF_DARK,
		GF_FORCE,     GF_INACT, GF_MANA,    GF_METEOR,
		GF_ICE,       GF_CHAOS,   GF_NETHER,  GF_DISENCHANT,
		GF_SHARDS,    GF_SOUND,   GF_NEXUS,   GF_POLLUTE,
		GF_TIME,      GF_GRAVITY, GF_ROCKET,  GF_NUKE,
		GF_HELL_FIRE, GF_DISINTEGRATE, GF_PSY_SPEAR
	};


	Chaos_type = hurt_types[randint0(31)];
	if (one_in_(4)) line_chaos = TRUE;

	if (one_in_(6))
	{
		for (dummy = 1; dummy < 10; dummy++)
		{
			if (dummy - 5)
			{
				if (line_chaos)
					fire_beam(Chaos_type, dummy, 150);
				else
					fire_ball(Chaos_type, dummy, 150, 2);
			}
		}
	}
	else if (one_in_(3))
	{
		fire_ball(Chaos_type, 0, 500, 8);
	}
	else
	{
		if (!get_aim_dir(&dir)) return;
		if (line_chaos)
			fire_beam(Chaos_type, dir, 250);
		else
			fire_ball(Chaos_type, dir, 250, 3 + (plev / 35));
	}
	*/
}


/*
 * Activate the evil Topi Ylinen curse
 * rr9: Stop the nasty things when a Cyberdemon is summoned
 * or the player gets paralyzed.
 */
/*:::太古の呪い処理　stop_ty:繰り返しストップフラグ*/
bool activate_ty_curse(bool stop_ty, int *count)
{
	int     i = 0;

	int flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);

	///mod160119 闘技場では終了
	if(p_ptr->inside_battle) return TRUE;

	set_deity_bias(DBIAS_REPUTATION, -1);
	set_deity_bias(DBIAS_COSMOS, -1);

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_print(_("あなたは太古の呪いを吸収した！", "You absorb the Ancient Foul Curse!"));
		hina_gain_yaku(250+randint1(250));
		return TRUE;
	}

	//v1.1.48 紫苑は呪いを不運パワーとして吸収する
	if (p_ptr->pclass == CLASS_SHION)
	{
		msg_print(_("あなたは太古の呪いを吸収した！", "You absorb the Ancient Foul Curse!"));
		p_ptr->magic_num1[1] += 10000 + randint1(10000);
		return TRUE;
	}

	do
	{
		switch (randint1(34))
		{
		case 28: case 29:
			/*:::地震*/
			if (!(*count))
			{
#ifdef JP
msg_print("地面が揺れた...");
#else
				msg_print("The ground trembles...");
#endif

				earthquake(py, px, 5 + randint0(10));
				if (!one_in_(6)) break;
			}
		case 30: case 31:
			/*:::半径8,ダメージ10d10の魔力の球*/
			if (!(*count))
			{
				int dam = damroll(10, 10);
#ifdef JP
msg_print("純粋な魔力の次元への扉が開いた！");
#else
				msg_print("A portal opens to a plane of raw mana!");
#endif

				project(0, 8, py, px, dam, GF_MANA, flg, -1);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, dam, "純粋な魔力の解放", -1);
#else
				take_hit(DAMAGE_NOESCAPE, dam, "released pure mana", -1);
#endif
				if (!one_in_(6)) break;
			}
		case 32: case 33:
			if (!(*count))
			{
#ifdef JP
msg_print("周囲の空間が歪んだ！");
#else
				msg_print("Space warps about you!");
#endif

				teleport_player(damroll(10, 10), TELEPORT_PASSIVE);
				if (randint0(13)) (*count) += activate_hi_summon(py, px, FALSE);
				if (!one_in_(6)) break;
			}
		case 34:
#ifdef JP
msg_print("エネルギーのうねりを感じた！");
#else
			msg_print("You feel a surge of energy!");
#endif

			wall_breaker();
			if (!randint0(7))
			{
				project(0, 7, py, px, 50, GF_KILL_WALL, flg, -1);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, 50, "エネルギーのうねり", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 50, "surge of energy", -1);
#endif
			}
			if (!one_in_(6)) break;
		case 1: case 2: case 3: case 16: case 17:
			aggravate_monsters(0,TRUE);
			if (!one_in_(6)) break;
		/*:::ハイサモン*/
		case 4: case 5: case 6:
			(*count) += activate_hi_summon(py, px, FALSE);
			if (!one_in_(6)) break;
		case 7: case 8: case 9: case 18:
		/*:::モンスター一体召喚？*/
			(*count) += summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			if (!one_in_(6)) break;
		case 10: case 11: case 12:
#ifdef JP
msg_print("生命力が体から吸い取られた気がする！");
#else
			msg_print("You feel your life draining away...");
#endif

			lose_exp(p_ptr->exp / 16);
			if (!one_in_(6)) break;
		case 13: case 14: case 15: case 19: case 20:
			if (stop_ty || (p_ptr->free_act && (randint1(125) < p_ptr->skill_sav)) || (RACE_RESIST_STATUE) || ( p_ptr->pseikaku == SEIKAKU_BERSERK) || (p_ptr->pclass == CLASS_AUNN))
			{
				/* Do nothing */ ;
			}
			else
			{
#ifdef JP
msg_print("彫像になった気分だ！");
#else
				msg_print("You feel like a statue!");
#endif

				if (p_ptr->free_act)
					set_paralyzed(p_ptr->paralyzed + randint1(3));
				else
					set_paralyzed(p_ptr->paralyzed + randint1(13));
				stop_ty = TRUE;
			}
			if (!one_in_(6)) break;
		case 21: case 22: case 23:
			(void)do_dec_stat(randint0(6));
			if (!one_in_(6)) break;
		case 24:
#ifdef JP
msg_print("ほえ？私は誰？ここで何してる？");
#else
			msg_print("Huh? Who am I? What am I doing here?");
#endif

			lose_all_info();
			if (!one_in_(6)) break;
		case 25:
			/*
			 * Only summon Cyberdemons deep in the dungeon.
			 */
			if ((dun_level > 65) && !stop_ty)
			{
				(*count) += summon_cyber(-1, py, px);
				stop_ty = TRUE;
				break;
			}
			if (!one_in_(6)) break;
		default:
			while (i < 6)
			{
				do
				{
					(void)do_dec_stat(i);
				}
				while (one_in_(2));

				i++;
			}
		}
	}
	while (one_in_(3) && !stop_ty);

	return stop_ty;
}

/*:::ハイ＝サモン　詳細未読*/
///sys mon ハイサモン　summon_specificのモードの変更が必要かもしれない
int activate_hi_summon(int y, int x, bool can_pet)
{
	int i;
	int count = 0;
	int summon_lev;
	u32b mode = PM_ALLOW_GROUP;
	bool pet = FALSE;

	if (can_pet)
	{
		if (one_in_(4))
		{
			mode |= PM_FORCE_FRIENDLY;
		}
		else
		{
			mode |= PM_FORCE_PET;
			pet = TRUE;
		}
	}

	if (!pet) mode |= PM_NO_PET;

	summon_lev = (pet ? p_ptr->lev * 2 / 3 + randint1(p_ptr->lev / 2) : dun_level);

	for (i = 0; i < (randint1(7) + (dun_level / 40)); i++)
	{
		switch (randint1(25) + (dun_level / 20))
		{
			case 1: case 2:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_ANT, mode);
				break;
			case 3: case 4:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_SPIDER, mode);
				break;
			case 5: case 6:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HOUND, mode);
				break;
			case 7: case 8:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HYDRA, mode);
				break;
			case 9: case 10:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_ANGEL, mode);
				break;
			case 11: case 12:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_UNDEAD, mode);
				break;
			case 13: case 14:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_DRAGON, mode);
				break;
			case 15: case 16:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_DEMON, mode);
				break;
			case 17:
				if (can_pet) break;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE));
				break;
			case 18: case 19:
				if (can_pet) break;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE));
				break;
			case 20: case 21:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HI_UNDEAD, mode);
				break;
			case 22: case 23:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HI_DRAGON, mode);
				break;
			case 24:
				count += summon_specific((pet ? -1 : 0), y, x, 100, SUMMON_CYBER, mode);
				break;
			default:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x,pet ? summon_lev : (((summon_lev * 3) / 2) + 5), 0, mode);
		}
	}

	return count;
}


/* ToDo: check */
int summon_cyber(int who, int y, int x)
{
	int i;
	int max_cyber = (easy_band ? 1 : (dun_level / 50) + randint1(2));
	int count = 0;
	u32b mode = PM_ALLOW_GROUP;

	/* Summoned by a monster */
	if (who > 0)
	{
		monster_type *m_ptr = &m_list[who];
		if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
	}

	if (max_cyber > 4) max_cyber = 4;

	for (i = 0; i < max_cyber; i++)
	{
		count += summon_specific(who, y, x, 100, SUMMON_CYBER, mode);
	}

	return count;
}

/*:::壁破壊のビームをランダムに数発放つ。血の呪い、カオス効果、シャッフルなどから呼ばれる*/
/*:::詳細未読*/
void wall_breaker(void)
{
	int i;
	int y, x;
	int attempts = 1000;

	if (randint1(80 + p_ptr->lev) < 70)
	{
		while (attempts--)
		{
			scatter(&y, &x, py, px, 4, 0);

			if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

			if (!player_bold(y, x)) break;
		}

		project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
				  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
	}
	else if (randint1(100) > 30)
	{
		earthquake(py, px, 1);
	}
	else
	{
		int num = damroll(5, 3);

		for (i = 0; i < num; i++)
		{
			while (1)
			{
				scatter(&y, &x, py, px, 10, 0);

				if (!player_bold(y, x)) break;
			}

			project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
					  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
		}
	}
}


/*
 * Confuse monsters
 */
bool confuse_monsters(int dam)
{
	return (project_hack(GF_OLD_CONF, dam));
}


/*
 * Charm monsters
 */
/*:::モンスター魅了*/
bool charm_monsters(int dam)
{
	return (project_hack(GF_CHARM, dam));
}


/*
 * Charm animals
 */
bool charm_animals(int dam)
{
	return (project_hack(GF_CONTROL_ANIMAL, dam));
}

/*
 * Charm animals
 */
bool charm_undead(int dam)
{
	return (project_hack(GF_CONTROL_UNDEAD, dam));
}


/*
 * Stun monsters
 */
bool stun_monsters(int dam)
{
	return (project_hack(GF_STUN, dam));
}


/*
 * Stasis monsters
 */
bool stasis_monsters(int dam)
{
	return (project_hack(GF_STASIS, dam));
}


/*
 * Mindblast monsters
 */
/*:::精神波動専用　視界内精神攻撃*/
bool mindblast_monsters(int dam)
{
	return (project_hack(GF_PSI, dam));
}


/*
 * Banish all monsters
 */
/*:::怪物追放、招魂、突然孤独になる変異*/
bool banish_monsters(int dist)
{
	return (project_hack(GF_AWAY_ALL, dist));
}


/*
 * Turn evil
 */
bool turn_evil(int dam)
{
	return (project_hack(GF_TURN_EVIL, dam));
}


/*
 * Turn everyone
 */
bool turn_monsters(int dam)
{
	return (project_hack(GF_TURN_ALL, dam));
}


/*
 * Death-ray all monsters (note: OBSCENELY powerful)
 */
bool deathray_monsters(void)
{
	return (project_hack(GF_DEATH_RAY, p_ptr->lev * 200));
}


bool charm_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CHARM, dir, plev, flg));
}


bool control_one_undead(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_UNDEAD, dir, plev, flg));
}


bool control_one_demon(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_DEMON, dir, plev, flg));
}


bool charm_animal(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_ANIMAL, dir, plev, flg));
}


bool charm_living(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_LIVING, dir, plev, flg));
}


/*:::変わり身処理　これを呼び出す前に失敗と決めることもできる*/
bool kawarimi(bool success)
{
	object_type forge;
	object_type *q_ptr = &forge;
	int y, x;

	if (p_ptr->is_dead) return FALSE;
	if (p_ptr->confused || p_ptr->blind || p_ptr->paralyzed || p_ptr->image) return FALSE;
	if (randint0(200) < p_ptr->stun) return FALSE;

	if (!success && one_in_(3))
	{
#ifdef JP
		msg_print("失敗！逃げられなかった。");
#else
		msg_print("Failed! You couldn't run away.");
#endif
		p_ptr->special_defense &= ~(NINJA_KAWARIMI);
		p_ptr->redraw |= (PR_STATUS);
		return FALSE;
	}

	y = py;
	x = px;

	if(p_ptr->pclass == CLASS_SAKUYA)
		teleport_player(20 + randint1(10), 0L);
	else
		teleport_player(10 + randint1(90), 0L);


	if(p_ptr->pclass != CLASS_SEIJA && p_ptr->pclass != CLASS_LYRICA && p_ptr->pclass != CLASS_SAKUYA && !(p_ptr->mimic_form == MIMIC_KOSUZU_GHOST))
	{
		object_wipe(q_ptr);
		///mod141207 像→石塊
		object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_STONE));
		(void)drop_near(q_ptr, -1, y, x);
	}
	if (success)
	{
		if(p_ptr->pclass == CLASS_SEIJA) msg_print(_("人形を身代わりにその場を逃れた！", "You use a doll as a substitute and run away!"));
		else if(p_ptr->prace == RACE_BAKEDANUKI) msg_print(_("人形を身代わりにその場を逃れた！", "You use a jizo statue as a substitute and run away!"));
		else if(p_ptr->pclass == CLASS_LYRICA) msg_print(_("あなたは雑音を残してその場から消えた！", "You disappear from the spot with only noise left behind!"));
		else if(p_ptr->mimic_form == MIMIC_KOSUZU_GHOST) msg_print(_("あなたは高笑いと共にその場から消えた！", "You disappear from the spot while laughing!"));
		else if(p_ptr->pclass == CLASS_SAKUYA)
		{
			int dam = 50 + p_ptr->lev * 3;
			msg_print(_("あなたはナイフをばら撒いて消えた！", "You disappear in a burst of knives!"));
			project(0,2,y,x,dam,GF_SHARDS,(PROJECT_GRID|PROJECT_KILL|PROJECT_JUMP|PROJECT_ITEM),-1);
		}
		else msg_print(_("石像を身代わりにその場を逃れた！", "You use a stone statue as a substitute and run away!"));
	}
	else msg_print(_("失敗！攻撃を受けてしまった。", "Failed! You get hit by the attack."));

	p_ptr->special_defense &= ~(NINJA_KAWARIMI);
	p_ptr->redraw |= (PR_STATUS);

	/* Teleported */
	return TRUE;
}


///mod140222
/*:::入身2　通常攻撃でなく指定属性による固定ダメージ*/
/*:::typeが0の場合移動だけで攻撃しない*/
//v1.1.92 ボールの半径を指定できるようにした
bool rush_attack2(int len, int type, int dam, int rad)
{
	int dir;
	int tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool moved = FALSE;

	if(len > 0) project_length = len;
	else project_length = 5;

	if (!get_aim_dir(&dir)) return FALSE;

	/* Use the given direction */
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) tm_idx = cave[ty][tx].m_idx;

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if (!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		monster_type *m_ptr;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		//溶岩や暗い穴にも入れるようにした
		//if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
		if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;

			/* Go to next grid */
			continue;
		}

		if (!cave[ny][nx].m_idx)
		{
			if (tm_idx)
			{
#ifdef JP
				msg_print("失敗！");
#else
				msg_print("Failed!");
#endif
			}
			else
			{
#ifdef JP
				msg_print("ここには入れない。");
#else
				msg_print("You can't move to that place.");
#endif
			}

			/* Exit loop */
			break;
		}

		/* Move player before updating the monster */
		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

		/* Update the monster */
		update_mon(cave[ny][nx].m_idx, TRUE);

		/* Found a monster */
		m_ptr = &m_list[cave[ny][nx].m_idx];

		if (tm_idx != cave[ny][nx].m_idx)
		{
#ifdef JP
			msg_format("何かに当たった！");
#else
			msg_format("There is %s in the way!", m_ptr->ml ? (tm_idx ? "another monster" : "a monster") : "someone");
#endif
		}
		/*:::teleport_playerした後なので必ずty,txに移動しているはず*/
	//	else if (!player_bold(ty, tx))
		else
		{
			/* Hold the monster name */
			char m_name[80];

			/* Get the monster name (BEFORE polymorphing) */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
		if(type)msg_format("あなたは%sへ飛びかかった！", m_name);
#else
			if(type)msg_format("You quickly leap at %s!", m_name);
#endif
		}
		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
		moved = TRUE;

		if(type) project(0, rad, ny, nx, dam, type, PROJECT_KILL | PROJECT_JUMP, -1);

		break;
	}

	if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

	return TRUE;
}

/*
 * "Rush Attack" routine for Samurai or Ninja
 * Return value is for checking "done"
 */
/*:::入身　詳細未読*/
///mod140213 射程距離を選べるようにした
bool rush_attack(bool *mdeath, int len, int mode)
{
	int dir;
	int tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool tmp_mdeath = FALSE;
	bool moved = FALSE;
	bool flag_attack = FALSE;

	if (mdeath) *mdeath = FALSE;

	if(len > 0) project_length = len;
	else project_length = 5;

	if (!get_aim_dir(&dir)) return FALSE;

	/* Use the given direction */
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) tm_idx = cave[ty][tx].m_idx;

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if (!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		monster_type *m_ptr;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;

			/* Go to next grid */
			continue;
		}

		if (!cave[ny][nx].m_idx)
		{
			if (tm_idx)
			{
#ifdef JP
				msg_print("失敗！");
#else
				msg_print("Failed!");
#endif
			}
			else
			{
				msg_print(_("そこには入れない。", "You can't go here."));
			}

			/* Exit loop */
			break;
		}

		/* Move player before updating the monster */
		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

		/* Update the monster */
		update_mon(cave[ny][nx].m_idx, TRUE);

		/* Found a monster */
		m_ptr = &m_list[cave[ny][nx].m_idx];

		if (tm_idx != cave[ny][nx].m_idx)
		{
#ifdef JP
			msg_format("%s%sが立ちふさがっている！", tm_idx ? "別の" : "",
				   m_ptr->ml ? "モンスター" : "何か");
#else
			msg_format("There is %s in the way!", m_ptr->ml ? (tm_idx ? "another monster" : "a monster") : "someone");
#endif
		}
		else if (!player_bold(ty, tx))
		{
			/* Hold the monster name */
			char m_name[80];

			/* Get the monster name (BEFORE polymorphing) */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("素早く%sの懐に入り込んだ！", m_name);
#else
			msg_format("You quickly jump in and attack %s!", m_name);
#endif
		}

		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
		moved = TRUE;
		tmp_mdeath = py_attack(ny, nx, (mode)?mode:HISSATSU_NYUSIN);
		flag_attack = TRUE;

		break;
	}

	//攻撃後離脱。離脱判定はhit_and_away()と同じに
	if (mode == HISSATSU_TRIANGLE && flag_attack)
	{
		//v1.1.30 オーラなどで倒れたらテレポしないようにしとく
		if (p_ptr->is_dead || randint0(p_ptr->skill_dis) < 7)
			msg_print(_("離脱失敗！", "Failed to disengage!"));
		else
		{
			msg_print(_("あなたは攻撃後離脱した。", "You disengage after attacking."));
			teleport_player(30, 0L);
		}
	}
	else if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

	if (mdeath) *mdeath = tmp_mdeath;
	return TRUE;
}


/*
 * Remove all mirrors in this floor
 */
void remove_all_mirrors(bool explode)
{
	int x, y;

	for (x = 0; x < cur_wid; x++)
	{
		for (y = 0; y < cur_hgt; y++)
		{
			if (is_mirror_grid(&cave[y][x]))
			{
				remove_mirror(y, x);
				if (explode)
					project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS,
						(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
	}
}

///mod140222
/*:::テレポート・バックを独立ルーチンにした。キャンセルしたときFALSEを返す。*/
/*:::引寄せをしたとき*idxにm_list[]の添字が入るが必ず隣に来るとは限らない。*/
/*:::modeにはTELEPORT_PASSIVEが自動的に付加される。*/
bool teleport_back(int *idx, u32b mode)
{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];

		if (!target_set(TARGET_KILL)) return FALSE;
		if (!cave[target_row][target_col].m_idx) return FALSE;
		if (!player_has_los_bold(target_row, target_col)) return FALSE;
		if (!projectable(py, px, target_row, target_col)) return FALSE;

		*idx = cave[target_row][target_col].m_idx;

		m_ptr = &m_list[*idx];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);
		///mod150211 騎乗中ペットを対象にしない
		if(p_ptr->riding == *idx)
		{
			if (CLASS_RIDING_BACKDANCE)
				msg_format(_("あなたはすでに%sの背後にいる。", "You are already behind %s."), m_name);
			else
				msg_format(_("あなたはすでに%sに乗っている！", "You are already riding %s!"), m_name);

				return FALSE;

		}

		if (r_ptr->flagsr & RFR_RES_TELE)
		{
			if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
				msg_format(_("%sには効果がなかった！", "%s is unaffected!"), m_name);
				return TRUE;
			}
			else if (r_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
				msg_format(_("%sを引き寄せるのに失敗した！", "You fail to command %s to return!"), m_name);
				return TRUE;
			}
		}
		msg_format(_("%sを引き寄せた。", "You command %s to return."), m_name);
		mode |= TELEPORT_PASSIVE;
		teleport_monster_to(cave[target_row][target_col].m_idx, py, px, 100,mode);
	return TRUE;
}


/*:::サモンマテリアル、ファフロッキーズの奇跡用 m_idxのモンスターに効果発生*/
void summon_material(int m_idx)
{
	cptr msg;
	int summon = randint1(p_ptr->lev);
	int typ,rad,dam;
	monster_type *m_ptr = &m_list[m_idx];
	if(!m_ptr->r_idx) return;
	else if(m_idx == p_ptr->riding)
	{
		msg_format(_("ERR:summon_material()で乗馬中モンスターがターゲットされた",
                    "ERR: summon_material() targeted a riding monster"));
		return;
	}
	else if(m_idx < 1)
	{
		msg_format(_("ERR:summon_material()引数のm_idx値がおかしい",
                    "ERR: Weird argument value for m_idx in summon_material()"));
		return;
	}

	if(summon < 5)
	{
		rad = 0;
		typ = GF_MISSILE;
		dam = damroll(1,3);
		msg = _("空からタライが降ってきた。", "A basin falls from the sky.");
	}
	else if(summon < 15)
	{
		rad = 0;
		typ = GF_MISSILE;
		dam = damroll(3,4);
		msg = _("空から中華ナベが降ってきた。", "A pot falls from the sky.");
	}
	else if(summon < 20)
	{
		rad = 0;
		typ = GF_ICE;
		dam = damroll(9,9);
		msg = _("空から氷塊が降ってきた。", "A block of ice falls from the sky.");
	}
	else if(summon < 30)
	{
		rad = 0;
		typ = GF_ARROW;
		dam = damroll(3,30);
		msg = _("空から要石が降ってきた。", "A keystone falls from the sky.");
	}
	else if(summon < 40)
	{
		rad = 0;
		typ = GF_MISSILE;
		dam = damroll(3,45);
		msg = _("空から墓石が降ってきた。", "A gravestone falls from the sky.");
	}
	else if(summon < 41 && a_info[ART_HOUTOU].cur_num == 0)
	{
		rad = 2;
		typ = GF_HOLY_FIRE;
		dam = damroll(15,15);
		msg = _("空から宝塔が降ってきた！", "A pagoda falls from the sky!");
		if (create_named_art(ART_HOUTOU,m_ptr->fy,m_ptr->fx))
		{
			a_info[ART_HOUTOU].cur_num = 1;
			if (character_dungeon) a_info[ART_HOUTOU].floor_id = p_ptr->floor_id;
		}
		else if (!preserve_mode) a_info[ART_HOUTOU].cur_num = 1;
	}
	else if(summon < 47)
	{
		rad = 1;
		typ = GF_ARROW;
		dam = damroll(20,20);
		msg = _("空から16tハンマーが降ってきた！", "A 16 ton hammer falls from the sky!");
	}
	else if(summon < 50)
	{
		rad = 2;
		typ = GF_METEOR;
		dam = damroll(12,50);
		msg = _("空からラピュタ帝国の機械兵が降ってきた！", "A Laputan Mechanical Soldier falls from the sky!");
		//ラピュタのアレを出したい
	}
	else
	{
		rad = 7;
		typ = GF_METEOR;
		dam = damroll(50,50);
		msg = _("『げ、減速できません！シャア少佐ー！』", "'I-I can't slow down! Mo-ove!'");
	}

	msg_format("%s",msg);
	project(0,rad,m_ptr->fy,m_ptr->fx,dam,typ,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP),-1);
	//氷塊のときたまにアマガエルが出てくる
	if(summon > 14 && summon < 20 && one_in_(3)) (void)summon_named_creature(0, target_row, target_col, MON_G_FROG, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));


}

//西方向へ吹き飛ぶ rush_attack2()から改変
bool go_west(void)
{
	int dir=4;
	int tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool moved = FALSE;

	project_length = 16;

	/* Use the given direction */
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];

	if (in_bounds(ty, tx)) tm_idx = cave[ty][tx].m_idx;

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if (!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		monster_type *m_ptr;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		//溶岩や暗い穴にも入れるようにした
		if (!cave[ny][nx].m_idx && player_can_enter(cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;

			/* Go to next grid */
			continue;
		}

		if (!cave[ny][nx].m_idx)
		{
			msg_format(_("%sに激突した！", "You crash into %s!"), f_name + f_info[get_feat_mimic(&cave[ny][nx])].name);
			/* Exit loop */
			break;
		}

		/* Move player before updating the monster */
		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

		/* Found a monster */
		m_ptr = &m_list[cave[ny][nx].m_idx];

		msg_format(_("何かに激突した！", "You crash into something!"));

		if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
		(void)set_monster_csleep(cave[ny][nx].m_idx, 0);
		moved = TRUE;

		break;
	}

	if (!moved && !player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

	return TRUE;
}

//v1.1.33 全周囲攻撃 ACT_WHIRLWINDからコピー
//v2.0.2 py_attackに使うmodeを指定できるようにした
void whirlwind_attack(int mode)
{
	int y = 0, x = 0;
	cave_type       *c_ptr;
	monster_type    *m_ptr;
	int dir;

	for (dir = 0; dir <= 9; dir++)
	{
		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];
		m_ptr = &m_list[c_ptr->m_idx];
		if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
			py_attack(y, x, mode);
	}

}


//v1.1.35 フロア全体に自重しない攻撃を行う。
//魔理沙の特技に同じルーチンがあるがそちらはまだ置き換えてない
//mode:1 ネムノ「囚われの秋雨」など
void floor_attack(int typ, int dice, int sides, int base, int mode)
{
	int y, x;
	cave_type *c_ptr;
	for (y = 1; y < cur_hgt - 1; y++)	for (x = 1; x < cur_wid - 1; x++)
	{
		c_ptr = &cave[y][x];
		if (cave_have_flag_bold(y, x, FF_PERMANENT)) continue;

		//ネムノ　縄張りにしか効果がない
		if (mode == 1 && !(c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)) continue;

		print_rel('#', (0x30 + spell_color(typ)), y, x);
	}
	Term_fresh();
	Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor * 2);

	for (y = 1; y < cur_hgt - 1; y++)	for (x = 1; x < cur_wid - 1; x++)
	{
		int dam;

		c_ptr = &cave[y][x];
		if (cave_have_flag_bold(y, x, FF_PERMANENT)) continue;

		//ネムノ　縄張りにしか効果がない
		if (mode == 1 && !(c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)) continue;

		dam = ((dice > 0 && sides > 0) ? damroll(dice, sides) : 0) + base;
		project_f(0, 0, y, x, dam, typ);
		project_o(0, 0, y, x, dam, typ);
		project_m(0, 0, y, x, dam, typ, (PROJECT_KILL), FALSE);

	}
}


//v1.1.44 再発動可能なビームやボールの処理。
//project()内で実装したら再帰処理の階層が深くなったときフリーズ(スタックオーバーフロー？)したので作り直した。
/* dir:設定しなかった場合近くの敵(モードによっては皿)からランダムに標的が決まる
*mode設定：
*0:通常モード。発動「魔弾の射手」、霊夢「よくばり大幣」、空「フィクストスター」など。
*  ボールを放つ。着弾後、restart_chanceに応じた確率で着弾地点から見えるモンスター（着弾したモンスター含む）へ再発動する。
*  hack_der_freischutz_basenumが設定されている場合は強制的に再発動する。そのとき周囲のモンスター数に応じて値を減らす。
*1:布都「六壬神火」「貴竜の矢」で使う。　
*  ターゲットを指定しない場合近くの皿がターゲットとして設定される。
*  皿の場所でビームを止めて皿を破壊し他の皿に向けて再発動する。
*  皿が無くなると近くのモンスターに飛んで終了。このときもモンスターの場所でビームが止まる。
*  rad値がある場合斜め四方向にのみ爆風が飛ぶ。
*/
bool	execute_restartable_project(int mode, int dir, int restart_chance, int dice, int sides, int base, int typ, int rad)
{
	int tx, ty; //ターゲット座標
	int ox, oy; //射出開始位置
	int flg;
	int dam;
	bool flag_first_attack = TRUE;

	bool flag_target_ok = FALSE;
	bool flag_restart = TRUE;

	oy = py;
	ox = px;

	//動作モードによりproject()に渡すフラグ値などを設定
	switch (mode)
	{
	case 1:
		flg = (PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO);
		break;
	default:
		flg = (PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL);
		break;
	}

	//清蘭特殊射撃など無効化
	if (cheat_xtra) msg_format("ssm:%d", special_shooting_mode);
	special_shooting_mode = 0;

	//最初にターゲットを決める場合の処理
	if (dir)
	{
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		flag_target_ok = TRUE;
	}

	while (TRUE)
	{
		int monster_count = 0;
		//ターゲットを決める。dirでターゲットが指定された場合は最初のループのときのみここに入らない
		if (!flag_target_ok)
		{

			//六壬神火と貴竜の矢 今の地点からの皿あるいは敵の位置を探す
			if (mode == 1)
			{
				if (futo_determine_target(oy, ox, &ty, &tx)) flag_target_ok = TRUE;
			}
			//魔弾の射手など 今の地点から届く敵を探す
			else
			{
				monster_type *m_ptr_tmp;
				int j;
				for (j = 1; j < m_max; j++)
				{
					m_ptr_tmp = &m_list[j];
					if (!m_ptr_tmp->r_idx) continue;
					if (is_pet(m_ptr_tmp)) continue;
					if (is_friendly(m_ptr_tmp)) continue;
					if (!projectable(oy, ox, m_ptr_tmp->fy, m_ptr_tmp->fx)) continue;
					monster_count++;
					if (!one_in_(monster_count)) continue;
					tx = m_ptr_tmp->fx;
					ty = m_ptr_tmp->fy;
					flag_target_ok = TRUE;
				}
			}
		}

		//ターゲットがない場合終了　一度もターゲットが決まらなかった場合FALSEを、一度でもproject()してればTRUEを返す
		if (!flag_target_ok)
		{
			if (flag_first_attack) return FALSE;
			else return TRUE;
		}

		//project()の実処理
		dam = base + ((dice>0 && sides>0) ? damroll(dice, sides) : 0);
		hack_project_start_x = ox;
		hack_project_start_y = oy;

		project(0, rad, ty, tx, dam, typ, flg, -1);

		//再発動判定
		//六壬神火と貴竜の矢は皿に当たったときだけ皿を壊して再発動する
		if (mode == 1)
		{
			if (flag_futo_broken_plate)
			{
				futo_break_plate(hack_project_end_y, hack_project_end_x);
				flag_futo_broken_plate = FALSE;
			}
			else
			{
				flag_restart = FALSE;
			}
		}
		//魔弾技能高い兵士が「魔弾の射手」を使ったときは一定回数強制再発動。
		else if (hack_der_freischutz_basenum)
		{
			if (cheat_xtra) msg_format("basenum:%d", hack_der_freischutz_basenum);
			//周囲のモンスターが少ない場合あまり一体に威力が集中しないよう回数減少量を大きくする。0のときはどうせ終了するので何もしない。
			if(monster_count > 0) hack_der_freischutz_basenum -= MAX(1, (7 / monster_count));
			if (hack_der_freischutz_basenum<0) hack_der_freischutz_basenum = 0;

		}
		//それ以外は指定した確率で再発動する
		else
		{
			if (one_in_(restart_chance)) flag_restart = FALSE;
		}

		//血の呪いとかで＠が倒れたら終了
		if (p_ptr->is_dead) flag_restart = FALSE;

		if (!flag_restart) return TRUE;

		//再発動準備して次のループへ
		ox = hack_project_end_x;
		oy = hack_project_end_y;
		flag_target_ok = FALSE;
		flag_first_attack = FALSE;

	}


}

//v1.1.77 お燐専用性格の特技「追跡」の離脱処理 kawarimi()を改変
bool orin_escape(int m_idx)
{
	int y, x;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (!is_special_seikaku(SEIKAKU_SPECIAL_ORIN)) return FALSE;
	if (p_ptr->lev < 30) return FALSE;

	if (!m_idx) return FALSE; //v1.1.77b 自分の使うテレポアウェイとかで飛び退いてしまっていたので修正

	if (p_ptr->is_dead) return FALSE;
	if (p_ptr->confused || p_ptr->blind || p_ptr->paralyzed || p_ptr->image) return FALSE;
	if (randint0(200) < p_ptr->stun) return FALSE;

	//if (p_ptr->magic_num1[0] != m_idx) return FALSE;
	//v2.0.15 管理方法変更
	if (!(m_ptr->mflag & MFLAG_SPECIAL)) return FALSE;

	if (p_ptr->csp < 30 || one_in_(6) || randint1(r_ptr->level) > (p_ptr->lev+ p_ptr->stat_ind[A_DEX]+3))
	{
		msg_print(_("失敗！逃げられなかった。", "Failed! You couldn't run away."));
		return FALSE;
	}

	p_ptr->csp -= 30;
	p_ptr->redraw |= PR_MANA;

	y = py;
	x = px;

	msg_print(_("あなたは敵の攻撃から跳び退いた！", "You jumped away from the attack!"));
	teleport_player(10, 0L);

	/* Teleported */
	return TRUE;
}
