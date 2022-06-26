/* File: cmd6.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object commands */

#include "angband.h"
#include "player/player-log.h"

/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * In all cases, if the player becomes "aware" of the item's use
 * by testing it, mark it as "aware" and reward some experience
 * based on the object's level, always rounding up.  If the player
 * remains "unaware", mark that object "kind" as "tried".
 *
 * This code now correctly handles the unstacking of wands, staffs,
 * and rods.  Note the overly paranoid warning about potential pack
 * overflow, which allows the player to use and drop a stacked item.
 *
 * In all "unstacking" scenarios, the "used" object is "carried" as if
 * the player had just picked it up.  In particular, this means that if
 * the use of an item induces pack overflow, that item will be dropped.
 *
 * For simplicity, these routines induce a full "pack reorganization"
 * which not only combines similar items, but also reorganizes various
 * items to obey the current "sorting" method.  This may require about
 * 400 item comparisons, but only occasionally.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "o_ptr" to
 * no longer point at the correct item, with horrifying results.
 *
 * Note that food/potions/scrolls no longer use bit-flags for effects,
 * but instead use the "sval" (which is also used to sort the objects).
 */


 //v1.1.49 紫苑が何かを飲み食いしたときにべた褒めしまくる クラスが紫苑でないときFALSEを返す
 //high_quality:TRUEの場合普通に美味いもの
 //kind:「名も知らない○○さん」の部分
static bool shion_comment_food(bool high_quality, cptr kind)
{
	int num = 3;

	if (kind) num++;

	if (p_ptr->pclass != CLASS_SHION) return FALSE;

	if (high_quality)
	{
		switch (randint1(num))
		{
		case 1:
			msg_format(_("これは本当に美味しい！あなたは感涙に咽びつつご馳走を貪った。",
                        "This is truly delicious! You devour it as you break down in tears."));
			break;
		case 2:
			msg_format(_("これは本当に美味しい！これこそ贅を極めた天界の料理に違いない。",
                        "This is truly delicious! This has to be some extremely luxurious heavenly dish."));
			break;
		default:
			msg_format(_("これは本当に美味しい！あなたは我を忘れて至福のひとときに浸った。",
                        "This is truly delicious! Your mind goes into a blank state of total bliss for a moment."));
			break;
		}

	}
	else
	{
		switch (randint1(num))
		{
		case 1:
			msg_format(_("「ああー！なんて美味しいの！」", "'Aaah! It's so tasty!"));
			break;
		case 2:
			msg_format(_("「生きててよかった！生きてることに感謝だわー！」", "'I'm glad to be alive! What a wonderful time to be alive!'"));
			break;
		case 3:
			msg_format(_("「ああ、なんて豪華なの！」", "'Aaah, it's so gorgeous!'"));
			break;
		default:
			msg_format(_("「名前も知らない%sさん、その命ありがとー！」", "'Oh, thank you, unknown %s, for giving your life!'"), kind);
			break;
		}
	}
	return TRUE;
}

/*:::食物摂取効果　今の＠に食べられるアイテムがすでに選択済み*/
///item race sys 何かを食べたときの効果
///mod131223 食糧摂取　全面変更 tval,sval
static void do_cmd_eat_food_aux(int item)
{
	int ident, lev;
	object_type *o_ptr;

	bool no_bad_effect = FALSE;

	if(p_ptr->pclass == CLASS_EIRIN) no_bad_effect = TRUE;

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();

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

	/* Sound */
	sound(SOUND_EAT);

	/* Take a turn */
	energy_use = 100;

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	if (o_ptr->tval == TV_MUSHROOM)
	{
		shion_comment_food(FALSE, _("キノコ", "mushroom"));


		/* Analyze the food */
		switch (o_ptr->sval)
		{
			case SV_MUSHROOM_POISON:
			{
				if(no_bad_effect){ident=TRUE;break;}

				if(p_ptr->pclass == CLASS_MEDICINE)
				{
					int damage = damroll(8,8);
					ident = TRUE;
					hp_player(damage);
					set_food(p_ptr->food + 5000);
					break;
				}
				else if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
				{
					if (set_poisoned(p_ptr->poisoned + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
				break;
			}

			case SV_MUSHROOM_BLINDNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_blind)
				{
					if (set_blind(p_ptr->blind + randint0(200) + 200))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_PARANOIA:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_fear)
				{
					if (set_afraid(p_ptr->afraid + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_CONFUSION:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_conf)
				{
					if (set_confused(p_ptr->confused + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_HALLUCINATION:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_chaos)
				{
					if (set_image(p_ptr->image + randint0(250) + 250))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_PARALYSIS:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->free_act)
				{
					if (set_paralyzed(p_ptr->paralyzed + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_WEAKNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_STR);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_SICKNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_CON);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_STUPIDITY:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_INT);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_NAIVETY:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_WIS);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_UNHEALTH:
			{
				if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_CON);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_DISEASE:
			{
				if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "毒入り食料", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_STR);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_POISON:
			{
				if(no_bad_effect){ident=TRUE;}

				if(p_ptr->pclass == CLASS_YAMAME)
				{
					ident = TRUE;
					msg_print(_("これは毒消しだ！あなたは苦しくなってきた。",
                                "This is an antidote! It hurts you!"));
					take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), _("解毒のキノコ", "Mushroom of Cure Poison"), -1);
				}
				else if(p_ptr->pclass == CLASS_MEDICINE)
				{
					ident = TRUE;
					msg_print(_("これは毒消しだ！体から力が抜ける！", "This is an antidote! Power drains from your body!"));
					(void)set_food(PY_FOOD_STARVE - 1);
				}
				else if (set_poisoned(0)) ident = TRUE;

				break;
			}

			case SV_MUSHROOM_CURE_BLINDNESS:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_blind(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_PARANOIA:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_afraid(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_CONFUSION:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_confused(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_SERIOUS:
			{
				if(no_bad_effect){ident=TRUE;}
				if (hp_player(damroll(4, 8))) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORE_STR:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_STR)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORE_CON:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_CON)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORING:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_STR)) ident = TRUE;
				if (do_res_stat(A_INT)) ident = TRUE;
				if (do_res_stat(A_WIS)) ident = TRUE;
				if (do_res_stat(A_DEX)) ident = TRUE;
				if (do_res_stat(A_CON)) ident = TRUE;
				if (do_res_stat(A_CHR)) ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MANA:
			{
				if(no_bad_effect){ident=TRUE;}
				if(p_ptr->csp < p_ptr->msp)
				{
					ident = TRUE;
					msg_print(_("少し頭がハッキリした。", "Your head clears up a bit."));
				}
				p_ptr->csp += damroll(8,8);
				if (p_ptr->csp >= p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}
				p_ptr->redraw |= (PR_MANA);
				break;

			}
			case SV_MUSHROOM_BERSERK:
			{
				if(no_bad_effect){ident=TRUE;break;}
				msg_print(_("む？このキノコは・・", "Mmm? This mushroom..."));
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				break;
			}
			case SV_MUSHROOM_MON_RED:
			{
				set_oppose_fire(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_WHITE:
			{
				set_oppose_cold(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_BLUE:
			{
				set_oppose_elec(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_BLACK:
			{
				set_oppose_acid(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GREEN:
			{
				set_oppose_pois(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GAUDY:
			{
				set_tim_res_chaos(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GRAY:
			{
				set_tim_res_nether(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_CLEAR:
			{
				set_shield(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_SUPER:
			{
				int percentage;

				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(20 + randint1(20), MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_PUFFBALL:
			{
				if(p_ptr->pclass != CLASS_SHION)
					msg_print(_("味はともかく食べ応えは抜群だ。", "Taste aside, it's pretty fullfilling."));
				ident = TRUE;

			}
			break;

		}
	}
	else if (o_ptr->tval == TV_FOOD)
	{



		/* Analyze the food */
		switch (o_ptr->sval)
		{

#ifdef JP
			case SV_FOOD_VENISON:
			{
				if (shion_comment_food(FALSE, "動物")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("あなたは戒律により肉を食べられない。");
					return;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("この食物は穢れに満ちていて食べられない。");
					return;
				}
				msg_print("歯ごたえがあっておいしい。");
				ident = TRUE;
				break;
			}

			case SV_FOOD_SLIMEMOLD:
			{
				if (shion_comment_food(FALSE, "粘菌")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("これは肉なのか植物なのか？念のために止めておこう。");
					return;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("こんな気持ちの悪いものを食べたくない。");
					return;
				}
				msg_print("これはなんとも形容しがたい味だ。");
				ident = TRUE;
				break;
			}
			case SV_FOOD_HARDBUSC:
			{
				if (shion_comment_food(FALSE, "穀物")) break;

				msg_print("香ばしいが味気ない。");
				ident = TRUE;
				break;
			}
			case SV_FOOD_CUCUMBER:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_KAPPA) msg_print("むむ・・このきゅうりは最高に美味しい！");
				else msg_print("ポリポリして香ばしいが少し味気ない。");
				ident = TRUE;
				break;
			}
			case SV_FOOD_RATION:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				msg_print("これはおいしい。");
				ident = TRUE;
				break;
			}
			case SV_FOOD_ABURAAGE:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_YOUKO) msg_print("むむ・・この油揚げは最高に美味しい！");
				else msg_print("香ばしいが少し物足りない。");
				ident = TRUE;
				break;
			}
#else
			case SV_FOOD_VENISON:
			{
				if (shion_comment_food(FALSE, "animal")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("Your religion prohibits eating meat.");
					return;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("This food is full of impurity and is not fit for consumption.");
					return;
				}
				msg_print("Chewy and delicious.");
				ident = TRUE;
				break;
			}

			case SV_FOOD_SLIMEMOLD:
			{
				if (shion_comment_food(FALSE, "mold")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("Is this meat or plant matter? You decline to eat just in case.");
					return;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("You have no intention of eating such unpleasant things.");
					return;
				}
				msg_print("Indescribable taste.");
				ident = TRUE;
				break;
			}
			case SV_FOOD_HARDBUSC:
			{
				if (shion_comment_food(FALSE, "grain")) break;

				msg_print("Fragrant but bland.");
				ident = TRUE;
				break;
			}
			case SV_FOOD_CUCUMBER:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_KAPPA) msg_print("Mmm... This cucumber really hit the spot!");
				else msg_print("Fragrant but a little dull.");
				ident = TRUE;
				break;
			}
			case SV_FOOD_RATION:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				msg_print("Tastes delicious.");
				ident = TRUE;
				break;
			}
			case SV_FOOD_ABURAAGE:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_YOUKO) msg_print("Mmm... This fried tofu really hit the spot!");
				else msg_print("Fragrant but a little unsatisfactory.");
				ident = TRUE;
				break;
			}
#endif


			case SV_FOOD_SENTAN:
			{
#ifdef JP
				msg_print("小さな丸薬を飲み込んだ。");
#else
				msg_print("That tastes good.");
#endif

				(void)set_poisoned(0);
				(void)hp_player(damroll(4, 8));
				ident = TRUE;
				break;
			}
			//v1.1.49 うなぎを食べると病気が治ることにしてみる
			case SV_FOOD_EEL:
			{
				if (shion_comment_food(TRUE, NULL)) break;

				if (p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print(_("あなたは戒律により肉を食べられない。",
                                "Your religion prohibits you from eating meat."));
					return;
				}
				else
				{
					msg_print(_("脂が乗っていて実に美味だ！", "It's fat and really delicious!"));
					restore_level();
					(void)do_res_stat(A_STR);
					(void)do_res_stat(A_INT);
					(void)do_res_stat(A_WIS);
					(void)do_res_stat(A_CON);
					(void)do_res_stat(A_DEX);
					(void)do_res_stat(A_CHR);
					update_stuff();
				}
			}
			break;

			//天人の丹　仙人に種族変更
			case SV_FOOD_TENTAN:
			{
				if (RACE_TENTAN_EFFECT)
				{
					msg_print(_("あなたは天人の霊薬を口に運んだ...", "You put the celestial tentan in your mouth..."));
					change_race(RACE_SENNIN, "");
					update_stuff();
					if (p_ptr->pclass == CLASS_SHINMYOUMARU || p_ptr->pclass == CLASS_SHINMYOU_2)
					{
						msg_print(_("ついにあなたは人間サイズの体を手に入れた！",
                                    "You're finally human-sized!"));
					}

				}
				else
				{
					msg_print(_("...何も起こらない。ただの不味いお団子だ。",
                                "...Nothing happens. It's just a tasteless dumpling."));

				}
			}
			break;

			//v1.1.68 魔法スイカ
			case SV_FOOD_MAGIC_WATERMELON:
			{

				msg_print(_("これはなんとも形容しがたい味だ。", "Indescribable taste."));
				ident = TRUE;
				break;

			}

			case SV_FOOD_DATURA:
			{
				if (p_ptr->pclass == CLASS_MEDICINE)
				{
					msg_print(_("この葉っぱはとても美味しい！", "Those leaves are very delicious!"));

				}
				else if(!throw_up())
					msg_print(_("これは毒草だがあなたの胃は物ともしなかった。",
                                "That's a poisonous herb, but it didn't affect you."));

			}
			break;
			//v1.1.91
			case SV_FOOD_FORBIDDEN_FRUIT:
			{
				msg_print(_("甘美なる死と背徳の味がする。", "It has the sweet taste of death and immorality."));

				if (do_inc_stat(A_INT));
				set_tim_addstat(A_INT, 110, 100, FALSE);
				set_tim_addstat(A_WIS, 110, 100, FALSE);
				player_gain_mana(999);

				//野良神様が食べたら激烈にカオス寄りになる
				set_deity_bias(DBIAS_COSMOS, -250);
				set_deity_bias(DBIAS_REPUTATION, 100);

			}
			break;


		}
	}

	///mod140730 ゴーレムと岩食い変異持ちが鉱石を食べた時の処理実装
	else if (o_ptr->tval == TV_MATERIAL)
	{

		switch (o_ptr->sval)
		{
			int base;

			//ヘマタイト、マグネタイト　一時AC増加
			case SV_MATERIAL_HEMATITE:
			case SV_MATERIAL_MAGNETITE:
			{

				msg_print(_("何だか体が丈夫になった気がする！", "You feel your body getting stronger!"));
				base = 50;
				set_shield(randint1(base) + base, FALSE);
			}
			break;
			//砒素鉱石、一時毒耐性
			case SV_MATERIAL_ARSENIC:
			{
				msg_print(_("毒が体に馴染んだ気がする。", "You feel like your body got used to poison."));
				base = 100;
				set_oppose_pois(randint1(base) + base, FALSE);
			}
			break;
			//石桜、一時カオス耐性
			case SV_MATERIAL_ISHIZAKURA:
			{
				msg_print(_("あなたの体は邪悪な色彩を帯びた・・", "Foul color covers your body..."));
				base = 100;
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
			break;
			//水銀鉱石　一時加速
			case SV_MATERIAL_MERCURY:
			{

				msg_print(_("何だか体の動きが滑らかになった気がする！",
                            "You feel your movements getting smoother!"));
				base = 50;
				set_fast(randint1(base) + base, FALSE);
			}
			break;
			//ミスリル　魔力復活
			//氷の鱗も同じ
			case SV_MATERIAL_MITHRIL:
			case SV_MATERIAL_ICESCALE:
			{
				//v1.1.78 画面更新がないので修正
				if (player_gain_mana(p_ptr->msp))
					msg_print(_("体に魔力が満ちた！", "Magical power flows through your body!"));

			}
			break;
			//アダマンタイト　一時全耐性+石肌
			case SV_MATERIAL_ADAMANTITE:
			{
				msg_print(_("ものすごく体が丈夫になった気がする！",
                            "You feel your body getting extremely resilient!"));
				base = 50;
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
				set_shield(randint1(base) + base, FALSE);
			}
			break;
			//宝石低位　体力回復+経験値復活

			case SV_MATERIAL_GARNET:
			case SV_MATERIAL_AMETHYST:
			case SV_MATERIAL_AQUAMARINE:
			case SV_MATERIAL_MOONSTONE:
			case SV_MATERIAL_PERIDOT:
			case SV_MATERIAL_OPAL:
			case SV_MATERIAL_TOPAZ:
			case SV_MATERIAL_LAPISLAZULI:
			{
				msg_print(_("これは実に甘くておいしい！体に活力が戻ってきた。",
                            "Very sweet and delicious! You feel your life energy returning."));
				restore_level();
				hp_player(p_ptr->mhp / 3);
			}
			break;

			//ダイヤモンド　無敵
			case SV_MATERIAL_DIAMOND:
			case SV_MATERIAL_HOPE_FRAGMENT:
			{
				msg_print(_("体が眩く輝いた！", "Your body shines brightly!"));
				base = 10;
				set_invuln(randint1(base) + base, FALSE);
			}
			break;
			//エメラルド　器用魅力上昇
			case SV_MATERIAL_EMERALD:
			{
				msg_print(_("体から気品がにじみ出る気がする！", "You feel you're getting more elegant!"));
				do_inc_stat(A_DEX);
				do_inc_stat(A_CHR);
			}
			break;
			//ルビー　腕力耐久上昇

			case SV_MATERIAL_RUBY:
			{
				msg_print(_("体の奥底から力が湧き出てきた！", "Strength springs forth through your body!"));
				do_inc_stat(A_STR);
				do_inc_stat(A_CON);
			}
			break;
			//サファイア 知能賢さ上昇
			case SV_MATERIAL_SAPPHIRE:
			{
				msg_print(_("頭が澄み渡るようにすっきりした！", "You feel refreshed, and your head is clear!"));
				do_inc_stat(A_INT);
				do_inc_stat(A_WIS);
			}
			break;

			//隕鉄、ミステリウム　全能力一時上昇
			case SV_MATERIAL_METEORICIRON:
			case SV_MATERIAL_MYSTERIUM:
			{
				int i;
				msg_print(_("何やら宇宙的なパワーを感じる！", "You feel some cosmic power!"));
				for(i=0;i<6;i++)set_tim_addstat(i,110,30,FALSE);
			}
			break;
			//ヒヒイロカネ　巨大化+究極の耐性
			//伊弉諾物質も
			case SV_MATERIAL_HIHIIROKANE:
			case SV_MATERIAL_IZANAGIOBJECT:
			{
				int v;
				v = randint1(25) + 25;
				msg_print(_("あなたは神の力をその身に取り込んだ！", "You absorb divine power into your body!"));
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
				set_mimic(v, MIMIC_GIGANTIC, FALSE);
			}
			break;
			//龍珠　経験値上昇
			case SV_MATERIAL_RYUUZYU:
			{

				if (p_ptr->prace == RACE_ANDROID) break;
				chg_virtue(V_ENLIGHTEN, 1);
				if (p_ptr->exp < PY_MAX_EXP)
				{
					s32b ee = (p_ptr->exp / 10) + 10;
					if (ee > 30000L) ee = 30000L;
					msg_print(_("あなたは龍珠を喰らって力を得た。", "You gain power by eating the dragon orb."));

					gain_exp(ee);
					ident = TRUE;
				}
				break;






			}
			case SV_MATERIAL_COAL:
				msg_print(_("苦っ！", "Bitter!"));
				break;

			default:
			{
				if(one_in_(3)) msg_print(_("これは香ばしくて後引く味だ。", "It has a savory, lingering taste."));
				else if(one_in_(2)) msg_print(_("むむ・・ビタースイートな大人の味だ。", "Mmm... a bittersweet adult taste."));
				else  msg_print(_("悠久を物語る深い味わいだ。", "It has a deep taste that speaks of eternity."));
			}
			break;


		}
	}

	else if (o_ptr->tval == TV_SWEETS)
	{
		//v1.1.76
		int yuyuko_get_mana = 0;
		/* Analyze the food */
		switch (o_ptr->sval)
		{
			case SV_SWEET_COOKIE:
			{
				if (shion_comment_food(TRUE, NULL)) break;
				yuyuko_get_mana = 10 + randint1(10);

				msg_print(_("サクサクして甘くておいしい。", "Crispy, sweet and delicious."));
				ident = TRUE;
				break;
			}
			case SV_SWEET_POTATO:
			{
				if (!shion_comment_food(TRUE, NULL))
					msg_print(_("これはおいしい焼き芋だ。身も心も暖まるようだ。",
                                "Delicious roasted sweet potato. It warms your body and soul."));

				if(p_ptr->csp < p_ptr->msp)
				{
					p_ptr->csp += p_ptr->msp / 2;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
				yuyuko_get_mana = p_ptr->msp / 4;

				ident = TRUE;
				break;
			}
			case SV_SWEETS_MIRACLE_FRUIT:
			{
				shion_comment_food(FALSE, NULL);

				if(p_ptr->csp < p_ptr->msp)
				{
					msg_print(_("ミラクルな効果で魔力が回復した！", "You recover mana through a miraclous effect!"));
					p_ptr->csp += p_ptr->msp / 3;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
				yuyuko_get_mana = p_ptr->msp / 3;

				ident = TRUE;
				break;
			}
			case SV_SWEETS_DANGO:
			{
				//鈴瑚パワーアップ
				if(p_ptr->pclass == CLASS_RINGO)
				{
					int perc =  p_ptr->chp * 100 / p_ptr->mhp;
					msg_print("MGMG..");
					//　カウンタ開始
					set_tim_general(20,FALSE,0,0);
					//HP再計算
					p_ptr->update |= (PU_BONUS);
					handle_stuff();
					p_ptr->chp = p_ptr->mhp * perc / 100;
					p_ptr->redraw |= (PR_HP );
					redraw_stuff();
					//これまで食べた団子の数カウント増加
					p_ptr->magic_num1[0] += 1;
					if(p_ptr->magic_num1[0] > 10000) p_ptr->magic_num1[0] = 10000;
				}
				else
				{

					switch(randint1(10))
					{
					case 1:
						msg_print(_("香ばしい黒胡麻団子だ。", "That was a fragrant black sesame dumpling."));break;
					case 2:
						msg_print(_("甘～いみたらし団子だ。", "That was a sweet mitarashi dumpling."));break;
					case 3:
						msg_print(_("瑞々しい香りのヨモギ団子だ。", "That was a fresh mugwort dumpling."));break;
					case 4:
						msg_print(_("独特のコクのあるずんだ団子だ。", "That was a zunda dumpling with unique richness."));break;
					case 5:
						msg_print(_("葡萄味の団子だ。", "That was a grape flavored dumpling.")); break;
					case 6:
						msg_print(_("苺味の団子だ。", "That was a strawberry flavored dumpling.")); break;
					case 7:
						msg_print(_("きな粉をまぶした団子だ。", "That was a dumpling sprinkled with soybean flour.")); break;
					case 8:
						msg_print(_("鈴瑚屋の新作だ！", "That was Suzuya's new work!")); break;
					default:
						msg_print(_("餡の入った月見団子だ。", "That was a moon dumpling with red bean paste."));break;
					}
					shion_comment_food(TRUE, NULL);
				}
				yuyuko_get_mana = 50 + randint1(50);
				ident = TRUE;
				break;
			}
			case SV_SWEETS_PEACH:
				msg_print(_("馥郁たる桃の香りが口いっぱいに広がった。",
                            "The taste of peach fills your mouth."));
				shion_comment_food(TRUE, NULL);
				restore_level();
				(void)set_poisoned(0);
				(void)set_blind(0);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_DEX);
				set_alcohol(0);
				update_stuff();

				if(one_in_(3))
				{
					int time = 5 + randint1(5);
					set_shield(time,FALSE);
				}
				yuyuko_get_mana = p_ptr->msp;

				break;
			case SV_SWEETS_MANJYU:
			{
				msg_print(_("頬が落ちそうなほど甘くておいしいお饅頭だ。",
                            "Very sweet and delicious manju."));
				shion_comment_food(TRUE, NULL);
				ident = TRUE;
				yuyuko_get_mana = 100 + randint1(100);
				break;
			}

		}

		if (p_ptr->pclass == CLASS_YUYUKO && yuyuko_get_mana > 0)
		{
			msg_print(_("あなたは甘味を堪能して上機嫌だ。",
                        "You enjoy the sweet taste; your mood lightens."));
			player_gain_mana(yuyuko_get_mana);

		}


	}
	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_KNOWLEDGE, -1);
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
	}
	*/
	/* We have tried it */
	if (o_ptr->tval == TV_MUSHROOM) object_tried(o_ptr);

	/* The player is now aware of the object */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	///item race 各種族の食糧食べたときの処理
	/* Food can feed the player */
	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		/* Reduced nutritional benefit */
		(void)set_food(p_ptr->food + (o_ptr->pval / 10));
#ifdef JP
msg_print("あなたのような者にとって食糧など僅かな栄養にしかならない。");
#else
		msg_print("Mere victuals hold scant sustenance for a being such as yourself.");
#endif

		if (p_ptr->food < PY_FOOD_ALERT)   /* Hungry */
#ifdef JP
msg_print("あなたの飢えは新鮮な血によってのみ満たされる！");
#else
			msg_print("Your hunger can only be satisfied with fresh blood!");
#endif

	}
	else if ((
		//prace_is_(RACE_SKELETON) ||
		  prace_is_(RACE_GOLEM) ||
		 prace_is_(RACE_ANDROID) ||
		 prace_is_(RACE_TSUKUMO) ||
		p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29

		 )
		 && (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND))
	{
		cptr staff;

		if (o_ptr->tval == TV_STAFF &&
		    (item < 0) && (o_ptr->number > 1))
		{
#ifdef JP
			msg_print("まずは杖を拾わなければ。");
#else
			msg_print("You must first pick up the staffs.");
#endif
			return;
		}

#ifdef JP
		staff = (o_ptr->tval == TV_STAFF) ? "杖" : "魔法棒";
#else
		staff = (o_ptr->tval == TV_STAFF) ? "staff" : "wand";
#endif

		/* "Eat" charges */
		if (o_ptr->pval == 0)
		{
#ifdef JP
			msg_format("この%sにはもう魔力が残っていない。", staff);
#else
			msg_format("The %s has no charges left.", staff);
#endif

			o_ptr->ident |= (IDENT_EMPTY);

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			p_ptr->window |= (PW_INVEN);

			return;
		}

#ifdef JP
		msg_format("あなたは%sの魔力をエネルギー源として吸収した。", staff);
#else
		msg_format("You absorb mana of the %s as your energy.", staff);
#endif

		/* Use a single charge */
		o_ptr->pval--;

		/* Eat a charge */
		set_food(p_ptr->food + 5000);

		/* XXX Hack -- unstack if necessary */
		if (o_ptr->tval == TV_STAFF &&
		    (item >= 0) && (o_ptr->number > 1))
		{
			object_type forge;
			object_type *q_ptr;

			/* Get local object */
			q_ptr = &forge;

			/* Obtain a local object */
			object_copy(q_ptr, o_ptr);

			/* Modify quantity */
			q_ptr->number = 1;

			/* Restore the charges */
			o_ptr->pval++;

			/* Unstack the used item */
			o_ptr->number--;
			p_ptr->total_weight -= q_ptr->weight;
			item = inven_carry(q_ptr);

			/* Message */
#ifdef JP
			msg_format("杖をまとめなおした。");
#else
			msg_print("You unstack your staff.");
#endif
		}

		/* Describe charges in the pack */
		if (item >= 0)
		{
			inven_item_charges(item);
		}

		/* Describe charges on the floor */
		else
		{
			floor_item_charges(0 - item);
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Don't eat a staff/wand itself */
		return;
	}
	///del131221 item バルログの死体食い
#if 0
	else if ((prace_is_(RACE_DEMON) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)) &&
		 (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_CORPSE &&
		  my_strchr("pht", r_info[o_ptr->pval].d_char)))
	{
		/* Drain vitality of humanoids */
		char o_name[MAX_NLEN];

		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
		msg_format("%sは燃え上り灰になった。精力を吸収した気がする。", o_name);
#else
		msg_format("%^s is burnt to ashes.  You absorb its vitality!", o_name);
#endif
		(void)set_food(PY_FOOD_MAX - 1);
	}
#endif

#if 0
	///sys item 骸骨は食べたものが落ちる（キノコ除く）
	else if (prace_is_(RACE_SKELETON))
	{
#if 0
		if (o_ptr->tval == TV_SKELETON ||
		    (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_SKELETON))
		{
#ifdef JP
			msg_print("あなたは骨で自分の体を補った。");
#else
			msg_print("Your body absorbs the bone.");
#endif
			set_food(p_ptr->food + 5000);
		}
		else
#endif

		if (o_ptr->tval == TV_FOOD && o_ptr->sval != SV_FOOD_SENTAN)
		{
			object_type forge;
			object_type *q_ptr = &forge;

#ifdef JP
msg_print("食べ物がアゴを素通りして落ちた！");
#else
			msg_print("The food falls through your jaws!");
#endif


			/* Create the item */
			object_prep(q_ptr, lookup_kind(o_ptr->tval, o_ptr->sval));

			/* Drop the object from heaven */
			(void)drop_near(q_ptr, -1, py, px);
		}
		else
		{
#ifdef JP
msg_print("食べ物がアゴを素通りして落ち、消えた！");
#else
			msg_print("The food falls through your jaws and vanishes!");
#endif

		}
	}
#endif
	else if(o_ptr->tval == TV_MATERIAL)
	{
		(void)set_food(p_ptr->food + o_ptr->pval);
	}

	else if(p_ptr->pclass == CLASS_KOGASA || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_HINA)
	{
		msg_print(_("こんな物では全然腹が膨れない。", "This hardly satiates you."));
		set_food(p_ptr->food + ((o_ptr->pval) / 20));
	}
	else if(p_ptr->pclass == CLASS_MEDICINE)
	{
		//v1.1.71 メディスンがチョウセンアサガオを食べたら満腹になる
		if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_DATURA)
		{
			/* Waybread is always fully satisfying. */
			set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
		}
		else if (o_ptr->tval != TV_MUSHROOM && o_ptr->sval != SV_MUSHROOM_POISON)
		{
			msg_print(_("こんな物では全然腹が膨れない。", "This hardly satiates you."));
			set_food(p_ptr->food + ((o_ptr->pval) / 50));
		}
	}
	else if (prace_is_(RACE_HANIWA))
	{
		msg_print(_("あなたは栄養を必要としない。", "You don't require food."));


	}
	else if (prace_is_(RACE_GOLEM) ||
		// prace_is_(RACE_ZOMBIE) ||
		 prace_is_(RACE_ENT) ||
		// prace_is_(RACE_DEMON) ||
		 prace_is_(RACE_ANDROID) ||
		 prace_is_(RACE_MAGIC_JIZO) ||
		 prace_is_(RACE_TSUKUMO) ||
		 prace_is_(RACE_ANIMAL_GHOST) ||
		 // prace_is_(RACE_SPECTRE) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING))
	{

		msg_print(_("生者の食物はあなたにとってほとんど栄養にならない。",
                    "Food for living beings hardly satiates you."));

		if(prace_is_(RACE_ANIMAL_GHOST) && p_ptr->food < PY_FOOD_ALERT)
			msg_print(_("敵を倒して力を奪わなければ。",
                        "You have to feed on power of your defeated enemies."));

		set_food(p_ptr->food + ((o_ptr->pval) / 20));
	}


	/*:::通常の食事*/
	///item エルフの口糧
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_SENTAN)
	{
		/* Waybread is always fully satisfying. */
		set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
		msg_print(_("一瞬で満腹になった!", "You are full!"));
	}
	//v1.1.43 オニフスベとスーパーキノコは満腹になる
	else if (o_ptr->tval == TV_MUSHROOM && (o_ptr->sval == SV_MUSHROOM_MON_SUPER || o_ptr->sval == SV_MUSHROOM_PUFFBALL))
	{
		/* Waybread is always fully satisfying. */
		set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
	}

	//v1.1.86 山童はキュウリを食べても満腹にならないことにした
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_CUCUMBER && (p_ptr->prace == RACE_KAPPA))
	{
		/*:::河童がきゅうりを食べると満腹になる*/
		set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
	}
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_ABURAAGE && (p_ptr->prace == RACE_YOUKO))
	{
		/*:::妖狐が油揚げを食べるとMPが20%回復する*/
		/*:::↑10%まで減らした*/
		///mod150504 藍は20%
		if(p_ptr->csp < p_ptr->msp)
		{
			if(p_ptr->pclass == CLASS_RAN)
			{
				msg_print(_("むむ・・この油揚げは最高に美味しい！",
                            "Mmm... This fried tofu is the best!"));
				p_ptr->csp += p_ptr->msp / 5;
			}
			else
			{
				msg_print(_("少し妖力が回復した気がする。",
                            "You feel your sorcerous power coming back."));
				p_ptr->csp += p_ptr->msp / 10;
			}
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			/* Redraw mana */
			p_ptr->redraw |= (PR_MANA);
		}
		(void)set_food(p_ptr->food + o_ptr->pval);
	}
	//鈴瑚がだんごを食べても食べ過ぎにはならない
	else if(p_ptr->pclass == CLASS_RINGO && o_ptr->tval == TV_SWEETS && o_ptr->sval == SV_SWEETS_DANGO)
	{
		int food = MIN(p_ptr->food + o_ptr->pval,PY_FOOD_MAX - 1);
		if(p_ptr->food < PY_FOOD_MAX) set_food(food);
	}
	else
	{
		/* Food can feed the player */
		(void)set_food(p_ptr->food + o_ptr->pval);
	}

#ifdef NEW_PLAYER_LOG
	if ((o_ptr->tval == TV_FOOD || o_ptr->tval == TV_MUSHROOM) && object_is_aware(o_ptr))
    {
        player_log_record_food_use(p_log_ptr, o_ptr->k_idx);
    }
#endif

	/* Destroy a food in the pack */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Destroy a food on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	//飲食は「エイボンの霧の車輪」の妨げにならない
	break_eibon_flag = FALSE;

}


/*
 * Hook to determine if an object is eatable
 */
/*:::食べられるアイテムを判別する　通常の食物、一部種族の杖、バルログ、悪魔変化の死体*/
///race item 食べられるアイテムのhook
///mod131223 食べられるアイテム
static bool item_tester_hook_eatable(object_type *o_ptr)
{
	if (o_ptr->tval==TV_FOOD) return TRUE;
	if (o_ptr->tval==TV_MUSHROOM) return TRUE;
	if (o_ptr->tval==TV_SWEETS) return TRUE;


	/*:::昔は骸骨が骨を食べることができたらしい*/
#if 0
	if (prace_is_(RACE_SKELETON))
	{
		if (o_ptr->tval == TV_SKELETON ||
		    (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_SKELETON))
			return TRUE;
	}
	else
#endif

		//ゴーレムは岩を食べることにする
	if(p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29
		|| prace_is_(RACE_GOLEM) || p_ptr->muta1 & MUT1_EAT_ROCK)
	{
		if(o_ptr->tval == TV_MATERIAL &&(
			o_ptr->sval == SV_MATERIAL_STONE
			|| o_ptr->sval == SV_MATERIAL_HEMATITE
			|| o_ptr->sval == SV_MATERIAL_MAGNETITE
			|| o_ptr->sval == SV_MATERIAL_ARSENIC
			|| o_ptr->sval == SV_MATERIAL_MERCURY
			|| o_ptr->sval == SV_MATERIAL_MITHRIL
			|| o_ptr->sval == SV_MATERIAL_ADAMANTITE
			|| o_ptr->sval == SV_MATERIAL_GARNET
			|| o_ptr->sval == SV_MATERIAL_AMETHYST
			|| o_ptr->sval == SV_MATERIAL_AQUAMARINE
			|| o_ptr->sval == SV_MATERIAL_DIAMOND
			|| o_ptr->sval == SV_MATERIAL_EMERALD
			|| o_ptr->sval == SV_MATERIAL_MOONSTONE
			|| o_ptr->sval == SV_MATERIAL_RUBY
			|| o_ptr->sval == SV_MATERIAL_PERIDOT
			|| o_ptr->sval == SV_MATERIAL_SAPPHIRE
			|| o_ptr->sval == SV_MATERIAL_OPAL
			|| o_ptr->sval == SV_MATERIAL_TOPAZ
			|| o_ptr->sval == SV_MATERIAL_LAPISLAZULI
			|| o_ptr->sval == SV_MATERIAL_METEORICIRON
			|| o_ptr->sval == SV_MATERIAL_HIHIIROKANE
			|| o_ptr->sval == SV_MATERIAL_SCRAP_IRON
			|| o_ptr->sval == SV_MATERIAL_ICESCALE
			|| o_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT
			|| o_ptr->sval == SV_MATERIAL_COAL
			|| o_ptr->sval == SV_MATERIAL_MYSTERIUM
			|| o_ptr->sval == SV_MATERIAL_IZANAGIOBJECT
			|| o_ptr->sval == SV_MATERIAL_ISHIZAKURA
			|| o_ptr->sval == SV_MATERIAL_RYUUZYU
			)) return TRUE;

	}

	if (
		//prace_is_(RACE_SKELETON) ||
		p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29 ||
		prace_is_(RACE_ANDROID) && p_ptr->pclass != CLASS_MEDICINE ||
	    prace_is_(RACE_TSUKUMO) && p_ptr->pclass != CLASS_KOGASA
	    //prace_is_(RACE_ZOMBIE) ||
	    //prace_is_(RACE_SPECTRE)
		)
	{
		if (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND)
			return TRUE;
	}
	///del131221 死体
#if 0
	else if (prace_is_(RACE_DEMON) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON))
	{
		if (o_ptr->tval == TV_CORPSE &&
		    o_ptr->sval == SV_CORPSE &&
		    my_strchr("pht", r_info[o_ptr->pval].d_char))
			return TRUE;
	}
#endif

	/* Assume not */
	return (FALSE);
}


/*
 * Eat some food (from the pack or floor)
 */
///item Eコマンド
void do_cmd_eat_food(void)
{
	int         item;
	cptr        q, s;


	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to food */
	item_tester_hook = item_tester_hook_eatable;

	/* Get an item */
#ifdef JP
	q = "どれを食べますか? ";
	s = "食べ物がない。";
#else
	q = "Eat which item? ";
	s = "You have nothing to eat.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Eat the object */
	do_cmd_eat_food_aux(item);
}


/*:::摂取したアルコールをどれくらいp_ptr->alcoholに反映させるかの計算式 ±20%程度乱数絡めて種族補正*/
///mod151108 引数check追加　警告用のテスト値で常に最大が返る
///v1.1.19 計算式変更　プレイヤーHDとレベルで酔いにくくなるようにし、泥酔度基準値も見直して悪酔いしづらくした
int calc_alcohol_mod(int num, bool check)
{
	int mul = 100 - p_ptr->lev;

	mul -= (p_ptr->hitdie - 15) * 2;

	//天狗と鬼(と華扇)をウワバミに設定
	if(p_ptr->prace == RACE_ONI) mul /= 4;
	if(p_ptr->pclass == CLASS_KASEN) mul /= 8; //v1.1.49 4→8
	if(p_ptr->prace == RACE_KARASU_TENGU) mul /= 2;
	if(p_ptr->prace == RACE_HAKUROU_TENGU) mul /= 2;

	if(mul < 1) mul = 1;
	if(mul > 100) mul = 100;

	if(check)
	{
		return (num * 6 / 5 * mul / 100 );
	}
	else
	{
		return (num * (80 + randint0(41)) / 100 * mul / 100);
	}
}


/*
 * Quaff a potion (from the pack or the floor)
 */
/*:::薬を飲む処理　薬選択済み*/
///item 薬を飲む
//v1.1.19 化け狸が油を飲む処理に対応
static void do_cmd_quaff_potion_aux(int item)
{
	int         ident, lev, power;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr;

	bool hoshiguma = FALSE;
	bool hyakuyakumasu = FALSE; //v1.1.49
	bool no_bad_effect = FALSE;

	if(p_ptr->pclass == CLASS_EIRIN) no_bad_effect = TRUE;

	if(inventory[INVEN_RARM].name1 == ART_HOSHIGUMA || inventory[INVEN_LARM].name1 == ART_HOSHIGUMA) hoshiguma = TRUE;


	/* Take a turn */
	energy_use = 100;
	if(p_ptr->pclass == CLASS_CHEMIST) energy_use -= (p_ptr->lev*2/3); //薬師は薬を飲むのが早い

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("瓶から液体が流れ出てこない！");
#else
		msg_print("The potion doesn't flow out from a bottle.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any())
	{
		if (!hex_spelling(HEX_INHAIL)) stop_hex_spell_all();
	}

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

	//v1.1.49 華扇が酒を呑むときは百薬枡を使うことにした(星熊盃装備時除く)
	if (p_ptr->pclass == CLASS_KASEN && o_ptr->tval == TV_ALCOHOL && !hoshiguma)
	{
		no_bad_effect = TRUE;
		hyakuyakumasu = TRUE;
	}



	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Single object */
	q_ptr->number = 1;

	if(p_ptr->pclass == CLASS_BYAKUREN && o_ptr->tval == TV_ALCOHOL)
	{
		msg_print(_("あなたは戒律により酒を飲めない。",
                    "Your religion prohibits you from consuming alcohol."));
		return;
	}

	if(o_ptr->tval == TV_ALCOHOL && o_ptr->sval == SV_ALCOHOL_MARISA && !hoshiguma)
	{
		msg_print(_("・・嫌な予感がする。", "...You have a bad feeling about this."));
		if (!get_check(_("本当に飲みますか？", "Are you really sure?"))) return;
	}

	if(p_ptr->prace == RACE_LUNARIAN && o_ptr->tval == TV_POTION && o_ptr->sval == SV_POTION_LIFE)
	{
		char i;
		msg_print(_("この薬はあなたにとって耐え難い穢れに満ちている。",
                    "This potion is full of impurity you can't withstand."));
		if (!get_check(_("本当に飲みますか？", "Are you really sure?"))) return;
		prt(_("確認のため '@' を押して下さい。", "Press '@' to confirm.'"), 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;
	}

	//v1.1.84 反獄王の入った酒　特殊★
	if (o_ptr->name1 == ART_HANGOKU_SAKE)
	{
		if (p_ptr->prace == RACE_LUNARIAN)
		{
			char i;
			msg_print(_("この酒はあなたにとって耐え難い穢れに満ちている。",
                    "This sake is full of impurity you can't withstand."));
			if (!get_check(_("本当に飲みますか？", "Are you really sure?"))) return;
			prt(_("確認のため '@' を押して下さい。", "Press '@' to confirm.'"), 0, 0);

			flush();
			i = inkey();
			prt("", 0, 0);
			if (i != '@') return;
		}
		else if (p_ptr->pclass == CLASS_REIMU)
		{
			msg_print(_("もの凄く嫌な予感がする。この酒を飲んではいけない。",
                        "You have a very bad feeling about this. Better not drink this sake."));
			return;
		}
		else
		{
			msg_print(_("・・嫌な予感がする。", "...You have a bad feeling about this."));
            if (!get_check(_("本当に飲みますか？", "Are you really sure?"))) return;
		}

	}

	if(o_ptr->tval == TV_COMPOUND && (o_ptr->sval == SV_COMPOUND_YOKOSHIMA || o_ptr->sval == SV_COMPOUND_ICHIKORORI || o_ptr->sval == SV_COMPOUND_HOURAI))
	{
		int i;
		if( !get_check_strict(_("本当に飲みますか？", "Are you really sure?"), CHECK_NO_HISTORY)) return;
		prt(_("確認のため '@' を押して下さい。", "Press '@' to confirm.'"), 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;

	}

	///mod151108 警告追加
	if((o_ptr->tval == TV_ALCOHOL || o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL)
		&& !(p_ptr->muta2 & MUT2_ALCOHOL))
	{
		int tmp = q_ptr->pval;
		if(hoshiguma && tmp < 5000) tmp = 5000;
		tmp = calc_alcohol_mod(tmp,TRUE);

		if((p_ptr->alcohol + tmp) >= DRANK_4 && !process_warning2() ) return;

	}

	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Sound */
	sound(SOUND_QUAFF);


	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[q_ptr->k_idx].level;

	///mod140326 アルコール効果実装
	if (q_ptr->tval == TV_ALCOHOL)
	{
		int i;


		if (hoshiguma)
		{
			msg_print(_("星熊盃に酒を注いで飲んだ。", "You pour the alcohol into Hoshiguma dish and drink it."));
		}
		else if (hyakuyakumasu)
		{
			msg_print(_("『茨木の百薬枡』に酒を注いだ。", "You pour the alchol into Ibaraki Box of 100 Medicines."));
		}

		//v1.1.84 特殊★の処理は別にやる
		if (q_ptr->name1 == ART_HANGOKU_SAKE)
		{
			if (p_ptr->prace == RACE_LUNARIAN)
			{
				take_hit(DAMAGE_LOSELIFE, 5000, _("反獄王", "Hangoku-ou"), -1);
			}
			else
			{
				msg_print(_("突然、大量の魔力を奪われた！",
                            "You suddenly feel your magical power being drained away!"));
				process_over_exert(999 - p_ptr->csp);

				gain_random_mutation(217);//反獄王に取り憑かれる変異

			}
		}
		else
			switch (q_ptr->sval)
		{
		case SV_ALCOHOL_SUZUME:
			shion_comment_food(TRUE, NULL);

#ifdef JP
			msg_print(("「らんらーら　らららら♪」");
			msg_print("「ららたった　ららら～♪」");
#endif
			break;
		case SV_ALCOHOL_1420:
			shion_comment_food(TRUE, NULL);
			msg_print(_("「ああ！これぞ一四二〇もの、まちがいなし！」",
                        "'Aah! It's 1420 brand, no mistake about it!'"));
			break;
		case SV_ALCOHOL_TUTINOKO:
			if(hoshiguma)
				msg_print(_("これは珍しい味だ！", "What an unusual taste!"));
			else if(!shion_comment_food(FALSE, NULL))
				msg_print(_("むむ、この味は！・・微妙だ。", "Mmm, this taste! ...it's strange."));
 			break;
		case SV_ALCOHOL_MANZAIRAKU:
			if(hoshiguma)
				msg_print(_("どこにでもある安酒だが美味い。", "It's your usual cheap sake, but it's delicious."));
			else if (!shion_comment_food(FALSE, NULL))
				msg_print(_("・・どこにでもある安酒だ。", "...It's your usual cheap sake."));
			break;
		case SV_ALCOHOL_GOLDEN_MEAD:
			msg_print(_("精神がどこまでも広がっていく気がする！", "You feel your mind vastly expanding!"));
			msg_print(NULL);
			wiz_lite(FALSE);
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			(void)detect_treasure(DETECT_RAD_DEFAULT);
			(void)detect_objects_gold(DETECT_RAD_DEFAULT);
			(void)detect_objects_normal(DETECT_RAD_DEFAULT);
			identify_pack();
			set_tim_addstat(A_INT,110,100, FALSE);
			set_tim_addstat(A_WIS,110,100, FALSE);
			(void)set_oppose_acid(100, FALSE);
			(void)set_oppose_elec(100, FALSE);
			(void)set_oppose_fire(100, FALSE);
			(void)set_oppose_cold(100, FALSE);
			(void)set_oppose_pois(100, FALSE);

			ignore_summon_align = TRUE;
			summon_specific(0, py, px, 50, SUMMON_BYAKHEE, PM_FORCE_PET | PM_ALLOW_GROUP);
			ignore_summon_align = FALSE;
			break;
		case SV_ALCOHOL_ORC:
			shion_comment_food(FALSE, NULL);

			set_fast(30, FALSE);
			set_hero(30, FALSE);
			(void)set_afraid(0);

			if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))	(void)set_poisoned(p_ptr->poisoned + 30);
			break;

		case SV_ALCOHOL_REISYU:
			msg_print(_("霊酒を飲んだ。心が澄み渡るようだ。", "You drink the spirit sake. Your mind clears."));
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp += 100;
				if(p_ptr->csp >= p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}

				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
				ident = TRUE;
			}
			break;

		case SV_ALCOHOL_NECTAR:
			shion_comment_food(TRUE, NULL);
			msg_print(_("体中に生命力が満ちあふれてきた！", "Vitality flows though your body!"));

			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_CHR);
			(void)set_shero(0,TRUE);
			set_alcohol(0);
			update_stuff();
			hp_player(5000);
			ident = TRUE;
			break;
		case SV_ALCOHOL_MARISA:
			if(hoshiguma)
			{
				msg_print(_("・・全然旨くない。盃の力で上質になっているはずなのだが。",
                            "...It doesn't look good at all. Even though the sake dish should've made it high quality."));
			}
			else
			{
				if(no_bad_effect){ident=TRUE;break;}

				msg_print(_("身も心も弱ってきて、精気が抜けていくようだ。",
                            "Your nerves and muscles feel weak and lifeless!"));
				take_hit(DAMAGE_LOSELIFE, damroll(10, 10), _("茸焼酎", "mushroom shochu"), -1);
				(void)dec_stat(A_DEX, 25, TRUE);
				(void)dec_stat(A_WIS, 25, TRUE);
				(void)dec_stat(A_CON, 25, TRUE);
				(void)dec_stat(A_STR, 25, TRUE);
				(void)dec_stat(A_CHR, 25, TRUE);
				(void)dec_stat(A_INT, 25, TRUE);
				ident = TRUE;
			}
				break;

		case SV_ALCOHOL_MOON:
			if(p_ptr->prace == RACE_LUNARIAN || p_ptr->pclass == CLASS_EIRIN || p_ptr->pclass == CLASS_KAGUYA)
			{
				msg_print(_("これは間違いなく月の都で千年以上熟成させた超超古酒だ！",
                            "This is super-ancient sake that has been aged for over a thousand years in the Lunar Capital!"));
			}
			else if (!shion_comment_food(TRUE, NULL))
			{
				msg_print(_("洗練されすぎて何だか寂しい味だ。", "It's way too refined, and somehow it tastes lonely."));
			}

			 break;

		case SV_ALCOHOL_MAMUSHI:
			if (!shion_comment_food(FALSE, NULL))
				msg_print(_("..味はさておき、元気が出てくる気がする！", "...Taste aside, you're feeling better!"));
			break;

		default:
			if(hoshiguma)
				msg_print(_("これは実に旨い酒だ！", "The sake was very delicious!"));
			else if (!shion_comment_food(FALSE, NULL))
				msg_print(_("この酒はおいしい。", "That was a tasty drink."));

			break;
		}
		//泥酔度上昇　種族など影響しランダム増減
		power = q_ptr->pval;
		if(hoshiguma && power < 5000) power = 5000; //星熊盃の力で酒がパワーアップ

		//百薬枡　癒やしの薬と同じ効果にする
		if (hyakuyakumasu)
		{
			set_asthma(p_ptr->asthma - 3000);
			hp_player(50);
			set_blind(0);
			set_poisoned(0);
			set_confused(0);
			set_stun(0);
			set_cut(0);
			set_image(0);
		}

		set_alcohol(p_ptr->alcohol + calc_alcohol_mod(power, FALSE));
		if(p_ptr->alcohol >= DRANK_1) set_afraid(0);
		if(cheat_xtra) msg_format("alcohol:%d",p_ptr->alcohol);
	}
	else if (q_ptr->tval == TV_FLASK)
	{
		switch (q_ptr->sval)
		{
		case SV_FLASK_OIL:
			if(p_ptr->alcohol >= DRANK_3)
				msg_print(_("これは実にコクがあって美味い！いくらでも呑めそうだ！",
                            "This is truly delicious! You can't stop drinking!"));
			else
				msg_print(_("これは油だ！油は狸を泥酔させてしまうということをあなたは思い出した。",
                            "This is oil! You remembered that tanuki can get drunk on oil."));

			power = q_ptr->xtra4;
			set_alcohol(p_ptr->alcohol + calc_alcohol_mod(power, FALSE));
			if(p_ptr->alcohol >= DRANK_1) set_afraid(0);
			if(cheat_xtra) msg_format("alcohol:%d",p_ptr->alcohol);

			break;
		default:
			msg_print(_("ERROR:このアイテムを飲んだときの効果が設定されていない",
                        "ERROR: Undefined effect when drinking this item."));
			return;

		}
	}
	else if (q_ptr->tval == TV_COMPOUND)
	{
		int time;
		switch (q_ptr->sval)
		{
		case SV_COMPOUND_MUSCLE_DRUG:
			{
				time = 25 + randint1(25);
				msg_print(_("薬を飲むと力が湧き出してきた！", "Strength flows through your body!"));
				set_tim_addstat(A_STR,5,time,FALSE);
				set_tim_addstat(A_DEX,5,time,FALSE);
				set_tim_addstat(A_CON,5,time,FALSE);
			}
			break;
		case SV_COMPOUND_BREATH_OF_FIRE:
			{
				int dir;
				msg_print(_("薬を飲むと喉の奥から熱いものが込み上げてきた。", "Your throat is suddenly very hot."));
				(void)get_hack_dir(&dir);
				msg_print(_("あなたは火炎のブレスを吐いた！", "You breathe fire!"));
				fire_ball(GF_FIRE, dir, 250, -2);

			}
			break;
		case SV_COMPOUND_NINGYO:
			{
				if(p_ptr->muta3 & MUT3_FISH_TAIL)
				{
					msg_print(_("薬を飲んだが、あなたはすでに人魚の体だ。", "You are already a merfolk."));
				}
				else
				{
					msg_print(_("薬を飲むと下半身がムズムズし始めた・・", "The lower half of your body starts to warp..."));
					gain_random_mutation(157);//人魚変異
				}
			}
			break;
		case SV_COMPOUND_JEKYLL:
			{
				int percentage;
				time = 50 + randint1(50);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(time, MIMIC_DEMON, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_HAPPY:
			{
				msg_print(_("あなたは自らに幸福薬を投与した。", "You gave yourself some happiness medicine."));
				set_hero(100,FALSE);
				if(no_bad_effect){ident=TRUE;break;}

				if(!p_ptr->resist_chaos)
					set_image(p_ptr->image + 50);
				if(!p_ptr->resist_conf)
					set_confused(p_ptr->confused + 50);

			}
			break;

		case SV_COMPOUND_GELSEMIUM_TEA:
			{
				int dir;
				(void)get_hack_dir(&dir);
				msg_print(_("「ぶー！猛毒！」", "'Blergh! Poison!'"));
				fire_ball(GF_POIS, dir, 800, -2);
			}
			break;
		case SV_COMPOUND_NIGHTMARE:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->free_act)
			{
				msg_print(_("あなたは恐ろしい悪夢を見た・・", "You see a terrible nightmare..."));
				get_mon_num_prep(get_nightmare, NULL);
				have_nightmare(get_mon_num(MAX_DEPTH));
				get_mon_num_prep(NULL, NULL);
				set_paralyzed(p_ptr->paralyzed + randint1(16) + 16);
				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print(_("頭がスッキリした！でも何か忘れているような。",
                                                                        "Your head feels refreshed! But looks like you've forgotten something."));
			}
			break;
		case SV_COMPOUND_NECOMIMI:
			{
				if(p_ptr->muta1 & MUT1_BEAST_EAR)
				{
					msg_print(_("薬を飲んだが、あなたはすでに獣耳だ。", "You already have beast ears."));
				}
				else
				{
					msg_print(_("薬を飲むと頭がムズムズし始めた・・", "Your head feels funny..."));
					gain_random_mutation(24);//獣耳変異
				}
			}
			break;
			//国士無双の薬　うどんげ特技ルーチンからコピー
		case SV_COMPOUND_KOKUSHI:
			{
				time = 25 + randint1(25);

				msg_format(_("薬を服用した。", "You drink the medicine."));
				if(!p_ptr->hero)
				{
					set_afraid(0);
					set_hero(time, FALSE);
				}
				else if(!p_ptr->shield)
				{
					set_shield(time, FALSE);
				}
				else if(!p_ptr->tim_addstat_count[A_STR] || p_ptr->tim_addstat_num[A_STR] < 105 ||
					!p_ptr->tim_addstat_count[A_DEX] || p_ptr->tim_addstat_num[A_DEX] < 105 ||
					!p_ptr->tim_addstat_count[A_CON] || p_ptr->tim_addstat_num[A_CON] < 105)
				{
					int percentage = p_ptr->chp * 100 / p_ptr->mhp;
					msg_format(_("強大な力が湧き出してきた！", "Strength flows through your body!"));
					set_tim_addstat(A_STR,105,time,FALSE);
					set_tim_addstat(A_DEX,105,time,FALSE);
					set_tim_addstat(A_CON,105,time,FALSE);
					p_ptr->chp = p_ptr->mhp * percentage / 100;
					p_ptr->redraw |= (PR_HP );
					redraw_stuff();

				}
				else
				{
					int dam = p_ptr->mhp / 2;
					if(dam<1) dam=1;

					if(no_bad_effect)
					{
						msg_print(_("これ以上は危険だ。飲んではいけない。", "Going beyond that is dangerous. You shouldn't drink it."));
						return;
					}

					msg_format(_("あなたは大爆発した！", "You explode!"));
					project(0, 7, py, px, dam * 5 + randint1(dam*5), GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
					take_hit(DAMAGE_LOSELIFE, dam, _("国士無双の薬の飲み過ぎ", "drinking too much Grand Patriot Elixir"), -1);
					set_hero(0,TRUE);
					set_shield(0,TRUE);
					set_tim_addstat(A_STR,0,0,TRUE);
					set_tim_addstat(A_DEX,0,0,TRUE);
					set_tim_addstat(A_CON,0,0,TRUE);
				}
			}
			break;
		case SV_COMPOUND_METAMOR_BEAST:
			{
				int percentage;
				time = 25 + randint1(25);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				if(percentage < 50) percentage = 50;
				dispel_player();
				set_mimic(time, MIMIC_BEAST, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_YOKOSHIMA:
			{
				if(p_ptr->resist_time || IS_INVULN() || no_bad_effect)
				{
					world_monster = -1;
					do_cmd_redraw();
					Term_xtra(TERM_XTRA_DELAY, 3000);
					world_monster = 0;
					p_ptr->redraw |= (PR_MAP);
					handle_stuff();

					msg_print(_("..あなたは過去への旅から無事帰還した。", "...You return from your trip to the past unharmed."));
				}
				else
				{
					p_ptr->playing = FALSE;
					p_ptr->is_dead = TRUE;
					p_ptr->leaving = TRUE;
					strcpy(p_ptr->died_from, "lost");
				}
			}
			break;
		case SV_COMPOUND_TITAN:
			{
				int percentage;
				time = 50 + randint1(50);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				set_mimic(time, MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_ICHIKORORI:
			if(no_bad_effect)
			{
				msg_print(_("あなたはすんでのところで自分が何を飲もうとしたのか気付いて捨てた。",
                            "Realizing what you're about to drink, you throw it away."));
				ident=TRUE;
				break;
			}

			else
			{
				msg_print(_("あなたは飲んではいけない薬をうっかり飲んでしまった！",
                            "Oh no! You've drunk something you definitely shouldn't have!"));
				if(!IS_INVULN())
				{
					take_hit(DAMAGE_LOSELIFE,9999,_("判断能力の低下", "lapse of judgment"),-1);
				}
			}
			break;
		case SV_COMPOUND_SUPER_SPEED:
			{
				time = 5 + randint1(5);
				set_lightspeed(time,FALSE);
			}
			break;
		case SV_COMPOUND_METAMOR_DRAGON:
			{
				int percentage;
				time = 50 + randint1(50);
				p_ptr->mimic_dragon_rank = 120+randint1(30);

				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				if(percentage < 50) percentage = 50;
				dispel_player();
				set_mimic(time, MIMIC_DRAGON, FALSE);

				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_HOURAI:
			{
				//将来的なアイテム引継ぎ追加のため入れておく
				if(p_ptr->prace == RACE_HOURAI)
				{
					msg_print(_("あなたはすでに蓬莱人だ。", "You are already a person of Hourai."));
					return;
				}

				if (last_words)
				{
					char buf[1024] = "";

					do
					{
						while (!get_string(_("*勝利*メッセージ: ", "***WINNER*** message: "), buf, sizeof buf)) ;
					}
					while (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_NO_HISTORY));
					if (buf[0])
					{
						p_ptr->last_message = string_make(buf);
						msg_print(p_ptr->last_message);
					}
				}
				//種族変容処理　引退直前なのでアンドロイドだろうと例外なし
				if (p_ptr->prace < 32)
					p_ptr->old_race1 |= 1L << p_ptr->prace;
				else
					p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
				p_ptr->prace = RACE_HOURAI;
				rp_ptr = &race_info[p_ptr->prace];
				gain_perma_mutation();

				p_ptr->redraw |= (PR_BASIC);
				p_ptr->update |= (PU_BONUS);
				handle_stuff();

				p_ptr->total_winner = TRUE;
				finish_the_game = TRUE;
				p_ptr->playing = FALSE;
				p_ptr->is_dead = TRUE;
				p_ptr->leaving = TRUE;
				strcpy(p_ptr->died_from, "hourai");
			}
			break;


		default:
			msg_print(_("それは飲むものじゃない。", "You can't drink that."));
			return;
		}

	}
	else if (q_ptr->tval == TV_SOFTDRINK)
	{
		switch (q_ptr->sval)
		{
		case SV_DRINK_WATER:
			msg_print(_("一息ついた。", "You take a break."));
			//v1.1.78 水を飲んだら泥酔度低下
			set_alcohol(p_ptr->alcohol - 500);
			break;
		case SV_DRINK_APPLE_JUICE:
		case SV_DRINK_GRAPE_JUICE:
			msg_print(_("甘くておいしい。", "Sweet and tasty."));
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 2000);
			break;
		case SV_DRINK_SLIME_MOLD:
			msg_print(_("なんとも不気味な味だ。", "What a weird taste."));
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 10000);
			break;
		case SV_DRINK_SALT_WATER:

#ifdef JP
			msg_print("うぇ！思わず吐いてしまった。");
#else
			msg_print("The potion makes you vomit!");
#endif
			if(no_bad_effect){ident=TRUE;break;}

			///race item 塩水の薬
			if (!(prace_is_(RACE_GOLEM) ||
			      //prace_is_(RACE_ZOMBIE) ||
			    //  prace_is_(RACE_DEMON) ||
			      prace_is_(RACE_ANDROID) ||
			      prace_is_(RACE_MAGIC_JIZO) ||
			      prace_is_(RACE_TSUKUMO) ||
				prace_is_(RACE_HANIWA) ||
				prace_is_(RACE_ANIMAL_GHOST) ||
				//prace_is_(RACE_SPECTRE) ||
			      (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)))
				{
				/* Only living creatures get thirsty */
				(void)set_food(PY_FOOD_STARVE - 1);
				}

			(void)set_poisoned(0);
			(void)set_paralyzed(p_ptr->paralyzed + 4);
			ident = TRUE;
			break;

		}


	}

	/* Analyze the potion */
	else if (q_ptr->tval == TV_POTION)
	{
		if(no_bad_effect)ident=TRUE;

		switch (q_ptr->sval)
		{

		case SV_POTION_SLOWNESS:
			if(no_bad_effect){ident=TRUE;break;}
			if (set_slow(randint1(25) + 15, FALSE)) ident = TRUE;
			break;


		case SV_POTION_POISON:
			if(no_bad_effect){ident=TRUE;break;}
			if(p_ptr->pclass == CLASS_MEDICINE)
			{
				int damage = damroll(4,8);
				ident = TRUE;
				hp_player(damage);
				set_food(p_ptr->food + 5000);
				break;
			}
			else if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
			{
				if (set_poisoned(p_ptr->poisoned + randint0(15) + 10))
				{
					ident = TRUE;
				}
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
			}
			break;
			///mod140907 猛毒の薬　メディスンが飲むとHP,MP,満腹度回復
		case SV_POTION_POISON2:
			{
				int damage = damroll(20,20);
				if(no_bad_effect){ident=TRUE;break;}

				if(p_ptr->pclass == CLASS_MEDICINE)
				{
					ident = TRUE;
					hp_player(damage);
					if (p_ptr->csp < p_ptr->msp)
					{
						p_ptr->csp += damage;
						if(p_ptr->csp >= p_ptr->msp)
						{
							p_ptr->csp = p_ptr->msp;
							p_ptr->csp_frac = 0;
						}
						msg_print(_("頭がハッキリとした。", "Your head clears."));

						p_ptr->redraw |= (PR_MANA);
						p_ptr->window |= (PW_PLAYER);
						p_ptr->window |= (PW_SPELL);
					}
					set_food(PY_FOOD_MAX - 1);
					break;
				}
				else if (p_ptr->resist_pois && IS_OPPOSE_POIS())	damage /= 9;
				else if (p_ptr->resist_pois || IS_OPPOSE_POIS()) damage /= 3;
				else set_poisoned(p_ptr->poisoned + randint0(15) + 10);

				ident = TRUE;
				take_hit(DAMAGE_NOESCAPE, damage, _("猛毒の薬", "potion of Poison"), -1);
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
			}
			break;
		case SV_POTION_BLINDNESS:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + randint0(100) + 100))
				{
					ident = TRUE;
				}
			}
			break;
		case SV_POTION_LOVE: //惚れ薬
			{
				msg_print(_("あなたは魔性のカリスマに満ち溢れた！", "You are overflowing with bewitching charisma!"));
				set_tim_addstat(A_CHR,120,30,FALSE);
				charm_monsters(200);
			}
			break;

		case SV_POTION_SLEEP:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->free_act)
			{
#ifdef JP
		msg_print("あなたは眠ってしまった。");
#else
		msg_print("You fall asleep.");
#endif

				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print(_("頭がスッキリした！でも何か忘れているような。", "Ah, such visions!  But, you feel like you've forgotten something."));

				if (ironman_nightmare)
				{
#ifdef JP
msg_print("恐ろしい光景が頭に浮かんできた。");
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
				if (set_paralyzed(p_ptr->paralyzed + randint0(4) + 4))
				{
					ident = TRUE;
				}
			}
			break;

		case SV_POTION_LOSE_MEMORIES:
			///mod131228 生命力維持削除
			//if (!p_ptr->hold_life && (p_ptr->exp > 0))
			if(no_bad_effect){ident=TRUE;break;}

			if (p_ptr->exp > 0)
			{
#ifdef JP
				msg_print("過去の記憶が薄れていく気がする。");
#else
				msg_print("You feel your memories fade.");
#endif
				chg_virtue(V_KNOWLEDGE, -5);

				lose_exp(p_ptr->exp / 4);
				ident = TRUE;
			}
			break;

		case SV_POTION_RUINATION:
			if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
			msg_print("身も心も弱ってきて、精気が抜けていくようだ。");
			take_hit(DAMAGE_LOSELIFE, damroll(10, 10), "破滅の薬", -1);
#else
			msg_print("Your nerves and muscles feel weak and lifeless!");
			take_hit(DAMAGE_LOSELIFE, damroll(10, 10), "a potion of Ruination", -1);
#endif

			(void)dec_stat(A_DEX, 25, TRUE);
			(void)dec_stat(A_WIS, 25, TRUE);
			(void)dec_stat(A_CON, 25, TRUE);
			(void)dec_stat(A_STR, 25, TRUE);
			(void)dec_stat(A_CHR, 25, TRUE);
			(void)dec_stat(A_INT, 25, TRUE);
			ident = TRUE;
			break;

		case SV_POTION_DEC_STR:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_DEC_INT:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_DEC_WIS:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_DEC_DEX:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_DEC_CON:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_DEC_CHR:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_DETONATIONS:
			if(no_bad_effect)
			{
				msg_print(_("ペロッ、この味は..ニトログリセリン！", "This taste... that's nitroglycerine!"));
				ident=TRUE;
				break;
			}
			else if(check_activated_nameless_arts(JKF1_EXPLOSION_DEF))
			{
				msg_print(_("腹の中で何かが爆発したが、あなたは煙を吐いただけで済んだ。",
                            "Something explodes in your stomach, but you just blow out a puff of smoke."));
				ident=TRUE;
				break;
			}
#ifdef JP
			msg_print("体の中で激しい爆発が起きた！");
			take_hit(DAMAGE_NOESCAPE, damroll(50, 20), "爆発の薬", -1);
#else
			msg_print("Massive explosions rupture your body!");
			take_hit(DAMAGE_NOESCAPE, damroll(50, 20), "a potion of Detonation", -1);
#endif

			//v1.1.66「破損」状態になる種族はメッセージが二度出て変なので朦朧処理をしないことにした。切り傷5000に比べれば誤差。
			if(!RACE_BREAKABLE)
				(void)set_stun(p_ptr->stun + 75);
			(void)set_cut(p_ptr->cut + 5000);
			ident = TRUE;
			break;

		case SV_POTION_DEATH:
			if(no_bad_effect){ident=TRUE;break;}
			//chg_virtue(V_VITALITY, -1);
			//chg_virtue(V_UNLIFE, 5);
#ifdef JP
			///msg131215 death
			msg_print("凄まじい悪寒が体中を駆けめぐった。");
			//msg_print("死の予感が体中を駆けめぐった。");
			take_hit(DAMAGE_LOSELIFE, 5000, "死の薬", -1);
#else
			msg_print("A terrible chill flows through your body.");
			take_hit(DAMAGE_LOSELIFE, 5000, "a potion of Death", -1);
#endif

			ident = TRUE;
			break;

		case SV_POTION_INFRAVISION:
			if (set_tim_infra(p_ptr->tim_infra + 100 + randint1(100), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_DETECT_INVIS:
			if (set_tim_invis(p_ptr->tim_invis + 12 + randint1(12), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_SLOW_POISON:
			if (set_poisoned(p_ptr->poisoned / 2)) ident = TRUE;
			break;

		case SV_POTION_CURE_POISON:
			if(p_ptr->pclass == CLASS_YAMAME)
			{
				ident = TRUE;
				msg_print(_("これは毒消しだ！あなたは苦しくなってきた。",
                            "This is an antidote! It hurts you!"));
				take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), "解毒の薬", -1);
			}
			else if(p_ptr->pclass == CLASS_MEDICINE)
			{
				ident = TRUE;
				msg_print(_("これは毒消しだ！体から力が抜ける！", "This is an antidote! Power drains from your body!"));
				(void)set_food(PY_FOOD_STARVE - 1);
			}
			else if (set_poisoned(0)) ident = TRUE;
			break;

		case SV_POTION_BOLDNESS:
			if (set_afraid(0)) ident = TRUE;
			break;

		case SV_POTION_SPEED:
			if (!p_ptr->fast)
			{
				if (set_fast(randint1(25) + 15, FALSE)) ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5, FALSE);
			}
			break;

		case SV_POTION_RESIST_HEAT:
			if (set_oppose_fire(p_ptr->oppose_fire + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_RESIST_COLD:
			if (set_oppose_cold(p_ptr->oppose_cold + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_HEROISM:
			if (set_afraid(0)) ident = TRUE;
			if (set_hero(p_ptr->hero + randint1(25) + 25, FALSE)) ident = TRUE;
			if (hp_player(10)) ident = TRUE;
			break;

		case SV_POTION_BESERK_STRENGTH:
			if (set_afraid(0)) ident = TRUE;
			if (set_shero(p_ptr->shero + randint1(25) + 25, FALSE)) ident = TRUE;
			if (hp_player(30)) ident = TRUE;
			break;

		case SV_POTION_CURE_LIGHT:
			if (hp_player(damroll(2, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_cut(p_ptr->cut - 10)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_SERIOUS:
			if (hp_player(damroll(4, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_cut((p_ptr->cut / 2) - 50)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_CRITICAL:
			if (hp_player(damroll(6, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_HEALING:
			if (hp_player(300)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_STAR_HEALING:
			if (hp_player(1200)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_LIFE:
			if(p_ptr->prace == RACE_LUNARIAN)
			{
				msg_print(_("体中に生命力が満ちあふれてきた！",
                            "Life force spreads through your body!"));
				take_hit(DAMAGE_LOSELIFE, 5000, _("生命の薬", "a potion of Life"), -1);

			}
			else
			{
				msg_print(_("体中に生命力が満ちあふれてきた！",
                            "Life force spreads through your body!"));

				restore_level();
				(void)set_poisoned(0);
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_CHR);
				(void)set_shero(0,TRUE);
				set_alcohol(0);
				update_stuff();
				hp_player(5000);
				set_asthma(0);
				ident = TRUE;


			}
			break;





		case SV_POTION_RESTORE_MANA:
			///class　魔道具術士の魔力回復
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
				ident = TRUE;
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
				ident = TRUE;
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_RESTORE_EXP:
			if (restore_level()) ident = TRUE;
			break;

		case SV_POTION_RES_STR:
			if (do_res_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_RES_INT:
			if (do_res_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_RES_WIS:
			if (do_res_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_RES_DEX:
			if (do_res_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_RES_CON:
			if (do_res_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_RES_CHR:
			if (do_res_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_INC_STR:
			if (do_inc_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_INC_INT:
			if (do_inc_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_INC_WIS:
			if (do_inc_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_INC_DEX:
			if (do_inc_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_INC_CON:
			if (do_inc_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_INC_CHR:
			if (do_inc_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_AUGMENTATION:
			if (do_inc_stat(A_STR)) ident = TRUE;
			if (do_inc_stat(A_INT)) ident = TRUE;
			if (do_inc_stat(A_WIS)) ident = TRUE;
			if (do_inc_stat(A_DEX)) ident = TRUE;
			if (do_inc_stat(A_CON)) ident = TRUE;
			if (do_inc_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_ENLIGHTENMENT:
#ifdef JP
			msg_print("自分の置かれている状況が脳裏に浮かんできた...");
#else
			msg_print("An image of your surroundings forms in your mind...");
#endif
			///del131214 virtue
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 1);
			wiz_lite(FALSE);
			ident = TRUE;
			break;

		case SV_POTION_STAR_ENLIGHTENMENT:
#ifdef JP
			msg_print("更なる啓蒙を感じた...");
#else
			msg_print("You begin to feel more enlightened...");
#endif
			///del131214 virtue
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 2);
			msg_print(NULL);
			wiz_lite(FALSE);
			(void)do_inc_stat(A_INT);
			(void)do_inc_stat(A_WIS);
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			(void)detect_treasure(DETECT_RAD_DEFAULT);
			(void)detect_objects_gold(DETECT_RAD_DEFAULT);
			(void)detect_objects_normal(DETECT_RAD_DEFAULT);
			identify_pack();
			self_knowledge();
			ident = TRUE;
			break;

		case SV_POTION_SELF_KNOWLEDGE:
#ifdef JP
			msg_print("自分自身のことが少しは分かった気がする...");
#else
			msg_print("You begin to know yourself a little better...");
#endif

			msg_print(NULL);
			self_knowledge();
			ident = TRUE;
			break;

		case SV_POTION_EXPERIENCE:
			if (p_ptr->prace == RACE_ANDROID) break;
			chg_virtue(V_ENLIGHTEN, 1);
			if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
				msg_print("更に経験を積んだような気がする。");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(ee);
				ident = TRUE;
			}
			break;

		case SV_POTION_RESISTANCE:
			(void)set_oppose_acid(p_ptr->oppose_acid + randint1(20) + 20, FALSE);
			(void)set_oppose_elec(p_ptr->oppose_elec + randint1(20) + 20, FALSE);
			(void)set_oppose_fire(p_ptr->oppose_fire + randint1(20) + 20, FALSE);
			(void)set_oppose_cold(p_ptr->oppose_cold + randint1(20) + 20, FALSE);
			(void)set_oppose_pois(p_ptr->oppose_pois + randint1(20) + 20, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_CURING:
			set_asthma(p_ptr->asthma - 3000);
			if (hp_player(50)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 3000);

			break;

		case SV_POTION_INVULNERABILITY:
			msg_print(_("あなたは何百通りもの未来を体験した！",
                        "You're experiencing hundreds of possible futures!"));
			(void)set_invuln(p_ptr->invuln + randint1(4) + 4, FALSE);
			ident = TRUE;
			break;

			/*:::item 新生の薬*/
		case SV_POTION_NEW_LIFE:
			set_deity_bias(DBIAS_REPUTATION, 5);
			do_cmd_rerate(FALSE);
			get_max_stats();
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			set_alcohol(0);
			if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print(_("全ての突然変異が治った。",
                                                    "All your mutations have been cured."));

				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				///mod140324 新生の薬でも生来型変異は消えなくする
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;
				p_ptr->update |= PU_BONUS;
				handle_stuff();
				mutant_regenerate_mod = calc_mutant_regenerate_mod();
			}
			ident = TRUE;
			break;

		case SV_POTION_NEO_TSUYOSHI:
			if(no_bad_effect)
			{
				/*msg_print(_("色々とそれっぽいのが入っているようだ。",
                            "Looks like there's something really weird mixed in here.")); ←これはネオつよしでなく漢方マサルダイナミックだった */
				ident=TRUE;
				break;
			}
			set_deity_bias(DBIAS_COSMOS, -1);

			(void)set_image(0);
			(void)set_tsuyoshi(p_ptr->tsuyoshi + randint1(100) + 100, FALSE);
			ident = TRUE;
			break;
			///item つよしスペシャル
		case SV_POTION_TSUYOSHI:
			if(no_bad_effect){ident=TRUE;break;}
			set_deity_bias(DBIAS_COSMOS, -1);
#ifdef JP
msg_print("「オクレ兄さん！」");
#else
			msg_print("Brother OKURE!");
#endif
			msg_print(NULL);
			p_ptr->tsuyoshi = 1;
			(void)set_tsuyoshi(0, TRUE);
			if (!p_ptr->resist_chaos)
			{
				(void)set_image(50 + randint1(50));
			}
			ident = TRUE;
			break;

		case SV_POTION_NANIKASUZUSHIKUNARU:
			if(no_bad_effect){ident=TRUE;break;}
			if(p_ptr->immune_cold) break;
			msg_print(_("体の中から物凄く冷えた！", "You are suddenly extremely cold!"));
			take_hit(DAMAGE_NOESCAPE, mod_cold_dam(1600), "cooling medicine", -1);
			break;

		case SV_POTION_POLYMORPH:
			if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4) && one_in_(23))
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print(_("全ての突然変異が治った。",
                                                    "All your mutations have been cured."));

				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				///mod141204 永続変異適用
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;
				p_ptr->update |= PU_BONUS;
				handle_stuff();
			}
			else
			{
				do
				{
					if (one_in_(2))
					{
						if(gain_random_mutation(0)) ident = TRUE;
					}
					else if (lose_mutation(0)) ident = TRUE;
				} while(!ident || one_in_(2));
			}
			break;
		}
	}
#if 0
	if (prace_is_(RACE_SKELETON))
	{
#ifdef JP
msg_print("液体の一部はあなたのアゴを素通りして落ちた！");
#else
		msg_print("Some of the fluid falls through your jaws!");
#endif

		(void)potion_smash_effect(0, py, px, q_ptr->k_idx);
	}
#endif
	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	/*
	if (!(object_is_aware(q_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* The item has been tried */
	object_tried(q_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(q_ptr))
	{
		object_aware(q_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		set_deity_bias(DBIAS_COSMOS, -2);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(q_ptr))
    {
        player_log_record_potion_use(p_log_ptr, q_ptr->k_idx);
    }
#endif

	/* Potions can feed the player */
	///race 薬を飲んだ時の満腹度増加処理
	///mod140326 酒を飲むと種族にかかわらずpval/3ぶんの食事
	//v1.1.19 化狸処理追加
	if(q_ptr->tval == TV_ALCOHOL) (void)set_food(p_ptr->food + q_ptr->pval / 3);
	else if(q_ptr->tval == TV_FLASK)
	{
		switch(q_ptr->sval)
		{
		case SV_FLASK_OIL:
			(void)set_food(p_ptr->food + 3000);
			break;
		default:
			msg_print(_("ERROR:このアイテムを飲んだときの満腹度処理が書かれていない",
                        "ERROR: Satiation logic not defined for drinking this item"));
			break;
		}
	}
	else
	switch (p_ptr->mimic_form)
	{
	case MIMIC_NONE:
		switch (p_ptr->prace)
		{
			case RACE_VAMPIRE:
				(void)set_food(p_ptr->food + (q_ptr->pval / 10));
				break;
			//case RACE_SKELETON:
				/* Do nothing */
			//	break;
			case RACE_GOLEM:
			//case RACE_ZOMBIE:
			case RACE_ANDROID:
			case RACE_TSUKUMO:
			//case RACE_DEMON:
			//case RACE_SPECTRE:
				set_food(p_ptr->food + ((q_ptr->pval) / 20));
				break;
				/*
			case RACE_ANDROID:
				if (q_ptr->tval == TV_FLASK)
				{
#ifdef JP
					msg_print("オイルを補給した。");
#else
					msg_print("You replenish yourself with the oil.");
#endif
					set_food(p_ptr->food + 5000);
				}
				else
				{
					set_food(p_ptr->food + ((q_ptr->pval) / 20));
				}
				*/
				break;
			case RACE_ENT:
#ifdef JP
				msg_print("水分を取り込んだ。");
#else
				msg_print("You are moistened.");
#endif
				set_food(MIN(p_ptr->food + q_ptr->pval + MAX(0, q_ptr->pval * 10) + 2000, PY_FOOD_MAX - 1));
				break;
			default:
				(void)set_food(p_ptr->food + q_ptr->pval);
				break;
		}
		break;
	case MIMIC_DEMON:
	case MIMIC_DEMON_LORD:
		set_food(p_ptr->food + ((q_ptr->pval) / 20));
		break;
	case MIMIC_VAMPIRE:
		(void)set_food(p_ptr->food + (q_ptr->pval / 10));
		break;
	default:
		(void)set_food(p_ptr->food + q_ptr->pval);
		break;
	}
}


/*
 * Hook to determine if an object can be quaffed
 */
/*:::qコマンドで飲める物の判定　アンドロイドで油つぼを飲めるかどうか以外ふつう*/
///item 飲めるアイテムの判定　DRINKABLEフラグを追加予定　アンドロイドは削除
///mod131223 qコマンド　飲み物と酒追加
///mod151227 薬師用合成薬追加
//v1.1.19 化狸は油を飲めるようにした
static bool item_tester_hook_quaff(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION) return TRUE;
	if (o_ptr->tval == TV_SOFTDRINK) return TRUE;
	if (o_ptr->tval == TV_ALCOHOL) return TRUE;
	//v1.1.19 化狸は油を飲めるようにした
	if (prace_is_(RACE_BAKEDANUKI))
	{
		if (o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL)
			return TRUE;
	}
	if (o_ptr->tval == TV_COMPOUND)//quaff_potionは効果発生の前に消費するので飲めない薬は先に弾いておく必要がある
	{
		switch(o_ptr->sval)
		{
		case SV_COMPOUND_MUSCLE_DRUG:
		case SV_COMPOUND_BREATH_OF_FIRE:
		case SV_COMPOUND_NINGYO:
		case SV_COMPOUND_JEKYLL:
		case SV_COMPOUND_HAPPY:
		case SV_COMPOUND_GELSEMIUM_TEA:
		case SV_COMPOUND_NIGHTMARE:
		case SV_COMPOUND_NECOMIMI:
		case SV_COMPOUND_KOKUSHI:
		case SV_COMPOUND_METAMOR_BEAST:
		case SV_COMPOUND_YOKOSHIMA:
		case SV_COMPOUND_TITAN:
		case SV_COMPOUND_ICHIKORORI:
		case SV_COMPOUND_SUPER_SPEED:
		case SV_COMPOUND_METAMOR_DRAGON:
		case SV_COMPOUND_HOURAI:
			return TRUE;
		default:
			return FALSE;
		}
	}

/*
	if (prace_is_(RACE_ANDROID))
	{
		if (o_ptr->tval == TV_FLASK)
			return TRUE;
	}
*/
	return FALSE;
}


/*
 * Quaff some potion (from the pack or floor)
 */
/*:::薬を飲む*/
void do_cmd_quaff_potion(void)
{
	int  item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_QUAFF)
	{
		msg_print(_("この姿では薬を飲めない。", "You cannot drink potions in your current form."));
		return ;
	}


	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to potions */
	item_tester_hook = item_tester_hook_quaff;

	/* Get an item */
#ifdef JP
	///msg131215
	//q = "どの薬を飲みますか? ";
	//s = "飲める薬がない。";
	q = "何を飲みますか? ";
	s = "飲める物を持っていない。";
#else
	q = "Quaff which potion? ";
	s = "You have no potions to quaff.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Quaff the potion */
	do_cmd_quaff_potion_aux(item);

	//飲食は「エイボンの霧の車輪」の妨げにならない
	break_eibon_flag = FALSE;

}



/*:::サグメの「読む」コマンドの効果発生*/
static void sagume_read_scroll_aux(int item, bool known)
{
	int         k, used_up, ident, lev;
	object_type *o_ptr;

	int scroll_k_idx,item2;

	/* Get the item (in the pack) */
	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	energy_use = 100;

	if (world_player)
	{
		if (flush_failure) flush();
		msg_print(_("止まった時の中ではうまく働かないようだ。",
                    "You can't do it while time is stopped."));
		sound(SOUND_FAIL);
		return;
	}

	if (p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		msg_print(_("巻物の文字が踊って見える。うまく読めない。",
                    "Writing on the scrolls dances before your eyes. You can't read it."));
		return;
	}

	if (p_ptr->asthma >= ASTHMA_3)
	{
		if(randint0(p_ptr->asthma) > 6600)
		{
			msg_print(_("あなたは咳き込んで文字を読むのに失敗した！",
                        "You have a coughing fit and fail to read the scroll!"));
			return;
		}
	}

	//後でo_ptrを設定し直すために使う
	scroll_k_idx = o_ptr->k_idx;

	ident = FALSE;
	used_up = TRUE;
	lev = k_info[o_ptr->k_idx].level;


	if (o_ptr->tval == TV_SCROLL)
	{
	switch (o_ptr->sval)
	{
		//暗闇：視界内先攻攻撃、視界内幻惑
		case SV_SCROLL_DARKNESS:
		{
			msg_format(_("突然巻物が眩く光った！", "The scroll suddenly shines brightly!"));
			project_hack2(GF_LITE,3,6, 0);
			confuse_monsters(200);
			ident = TRUE;
			break;
		}
		//光:視界内暗黒、盲目、光源減少

		case SV_SCROLL_LIGHT:
		{
			object_type *tmp_o_ptr = &inventory[INVEN_LITE];

			msg_format(_("突然辺りは闇に包まれた。", "Suddenly it becomes dark."));
			project_hack2(GF_DARK,3,6,0);

			//光源減少
			if (tmp_o_ptr->k_idx && (tmp_o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(tmp_o_ptr)))
			{
				//Hack - ここで0にすると処理がややこしそうなので1にして通常の光源寿命減少処理で0にする
				tmp_o_ptr->xtra4 = 1;
				p_ptr->window |= (PW_EQUIP);
			}
			ident = TRUE;
			break;
		}

		//エキサイトモンスター：金縛り
		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
			msg_print(_("辺りの空気が凍るように張りつめた・・",
                        "The atmosphere feels almost frozen..."));

			stasis_monsters(1000);
			ident = TRUE;
			break;
		}

		//防具呪縛：*防具強化*と入れ替える
		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (curse_armor()) ident = TRUE;
			break;
		}
		case SV_SCROLL_CURSE_ARMOR:
		{
			if (!enchant_spell(0, 0, randint1(10) + 10)) used_up = FALSE;
			ident = TRUE;
			break;
		}
		//武器呪縛：*武器強化*と入れ替える
		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			k = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				k = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) k = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) k = INVEN_LARM;
			if (k && curse_weapon(FALSE, k)) ident = TRUE;
			break;
		}
		case SV_SCROLL_CURSE_WEAPON:
		{
			if (!enchant_spell(randint1(10)+10, randint1(10)+10, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		//サモンモンスター：周辺抹殺と入れ替える
		case SV_SCROLL_MASS_GENOCIDE:
		{
			int num = 3 + randint1(3);
			for (k = 0; k < num; k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}
		case SV_SCROLL_SUMMON_MONSTER:
		{
			(void)mass_genocide(300, TRUE);
			ident = TRUE;
			break;
		}

		//トラップ破壊：トラップ創造かドア生成
		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if(one_in_(2))
			{
				if (trap_creation(py, px)) ident = TRUE;
			}
			else
			{
				door_creation(1);
				ident = TRUE;
			}
			break;
		}

		//トラップ創造：広範囲のトラップ破壊
		case SV_SCROLL_TRAP_CREATION:
		{
			(void)project(0,7, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);
			ident = TRUE;
			break;
		}

		//サモンアンデッド：アンデッド退散と入れ替える
		case SV_SCROLL_DISPEL_UNDEAD:
		{
			int num = 3 + randint1(3);
			for (k = 0; k < num; k++)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			if(ident) msg_print(_("不浄な気配が辺りに満ちた・・", "You sense an unclean presence..."));
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			if (dispel_undead(300)) ident = TRUE;
			break;
		}

		//聖唱歌　太古の怨念1
		case SV_SCROLL_HOLY_CHANT:
		{
			bool stop_ty = FALSE;
			int count = 0;

			msg_print(_("巻物がピカッと光った！", "The scroll lets out a bright flash!"));

			do
			{
				stop_ty = activate_ty_curse(stop_ty, &count);
			}
			while (one_in_(6));
			ident = TRUE;
		}

		//フロアリセット：シャドウシフトする
		case SV_SCROLL_RESET_RECALL:
		{
			if (p_ptr->inside_arena || ironman_downward)
			{
				msg_print(_("一瞬辺りの景色が歪んだ気がするが、何も起こらなかった。",
                            "For a moment, your surroundings seem to distort, but nothing happens."));
			}
			else
			{
				/*:::0ではまずいはず*/
				p_ptr->alter_reality = 1;
				p_ptr->redraw |= (PR_STATUS);
				ident = TRUE;
			}
			break;
		}
		//魔法の地図：地震
		case SV_SCROLL_MAPPING:
		{
			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level)
			{
				msg_print(_("一瞬地面が揺れたが、何も起こらなかった。",
                            "The ground shakes for a moment, but nothing happens."));
			}
			else
			{
				msg_print(_("突如、ダンジョンの天井が崩落した！",
                            "The dungeon ceiling suddenly collapses!"));
				earthquake_aux(py,px,12,0);
			}
			ident = TRUE;
			break;
		}
		//防具強化：防具が劣化する
		case SV_SCROLL_ENCHANT_ARMOR:
		{
			int inven_list[6] = {INVEN_RIBBON,INVEN_BODY,INVEN_OUTER,INVEN_HEAD,INVEN_HANDS,INVEN_FEET};
			apply_disenchant(0L,inven_list[randint0(6)]);
			ident = TRUE;
			break;
		}
		//武器強化：武器が劣化する
		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			apply_disenchant(0L,INVEN_RARM);
			ident = TRUE;
			break;
		}

		//魔力充填：魔力食い
		case SV_SCROLL_RECHARGING:
		{
			if (!eat_magic(130)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		//凡庸を☆生成にしてみる（！）
		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if (!artifact_scroll()) used_up = FALSE;
			break;
		}


		//獲得：混沌招来
		case SV_SCROLL_ACQUIREMENT:
		{
			call_chaos();
			ident = TRUE;
			break;
		}
		//*獲得*：虚無招来
		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			call_the_();
			ident = TRUE;
			break;
		}

		//救援召喚：ハルマゲドントラップ
		case SV_SCROLL_SUMMON_KIN:
		{
			static int levs[10] = {1, 3, 5, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;
			bool flag = FALSE;

			int mon_lev;

			/* Summon Demons and Angels */
			for (mon_lev = MAX(30,dun_level); mon_lev >= 20; mon_lev -= 1 + mon_lev /16)
			{
				int i;
				int num = levs[MIN(mon_lev /10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(px, 7);
					int y1 = rand_spread(py, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(py, px, y1, x1)) continue;

					if (summon_specific(0, y1, x1, mon_lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, mon_lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
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
						flag = TRUE;
					}
				}
			}
			if(flag)
			{
				msg_print(_("突然天界の戦争に巻き込まれた！",
                            "You are suddenly drawn into a heavenly conflict!"));
				ident = TRUE;
			}
			else
			{
				msg_print(_("一瞬何か只ならぬ気配を感じたが、何も起こらなかった。",
                            "You sense supernatural presence for a moment, but nothing happens."));
			}
			break;
		}

		//財宝感知：金が減る
		case SV_SCROLL_DETECT_GOLD:
		{
			if(p_ptr->au > 4)
			{
				msg_print(_("...お財布が軽くなった気がする。", "...Your wallet feels lighter."));
				p_ptr->au -= p_ptr->au / 4;
				p_ptr->redraw |= (PR_GOLD);
				ident = TRUE;
			}
			break;
		}
		//解呪　装備品が呪われる
		case SV_SCROLL_REMOVE_CURSE:
		{
			curse_equipment(100, 10);
			ident = TRUE;
			break;
		}

		//*解呪*　全能力値低下
		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			int dummy;
			msg_print(_("恐怖の暗黒オーラがあなたを包み込んだ・・", "A terrible black aura surrounds you..."));
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), TRUE);
			}

			ident = TRUE;
		}

		//パニックモンスター：混乱と幻覚
		case SV_SCROLL_MONSTER_CONFUSION:
		{
			msg_print(_("手が輝き始めた。そして輝きが体中に広がっていく・・",
                        "Your hands start glowing. Then the glow spreads to your body..."));
			if(!p_ptr->resist_conf) (void)set_confused(p_ptr->confused + randint0(10) + 10);
			if(!p_ptr->resist_chaos) (void)set_image(p_ptr->image + randint0(100) + 100);

			break;
		}

		//ペット召喚：全ての配下が消滅する
		case SV_SCROLL_SUMMON_PET:
		{
			int i;
			bool flag = FALSE;

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
				delete_monster_idx(i);
				flag = TRUE;

				if (record_named_pet && m_ptr->nickname)
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_DISMISS, m_name);
				}
			}
			if(flag)
			{
				msg_print(_("配下の気配が消えた・・", "You feel your followers disappear..."));
				ident = TRUE;
			}
			break;
		}

		//鑑定：アイテムが未鑑定に戻る
		case SV_SCROLL_IDENTIFY:
		{
			if(!ident_spell_2(3)) used_up = FALSE;
			else ident = TRUE;

			break;
		}
		//*鑑定*　凡庸
		case SV_SCROLL_STAR_IDENTIFY:
		{
			if (!mundane_spell(FALSE)) used_up = FALSE;
			break;
		}

		//呪文の巻物　フロアの敵を黙らせる
		case SV_SCROLL_SPELL:
		{
			msg_print(_("ダンジョンに異質な魔力が満ちた。",
                        "The dungeon is filled with some extraordinary magical power."));
			p_ptr->silent_floor = 1;
			ident = TRUE;
			break;
		}

		//噂の巻物：自分が読むわけではないので何も起こらない
		case SV_SCROLL_RUMOR:
		{
			msg_print(_("巻物にはメッセージが書かれている:",
                        "There's something written on the scroll. It says: "));

			display_rumor(TRUE);
			msg_print(_("巻物は煙を立てて消え去った！", "The scroll vanishes in a puff of smoke!"));

			ident = TRUE;
			break;
		}
		//*破壊*　周囲の壁を永久壁にしボス以外のモンスターを眠らせる
		case SV_SCROLL_STAR_DESTRUCTION:
		{
			int x, y;
			int r = 13 + randint0(5);

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print(_("何も起こらなかった。", "Nothing happens."));
				break;
			}

			for (y = (py - r); y <= (py + r); y++)
			{
				for (x = (px - r); x <= (px + r); x++)
				{
					bool flag_mark = FALSE;
					if (!in_bounds(y, x)) continue;
					if(player_bold(y,x)) continue;
					if(distance(py, px, y, x) > r) continue;

					if(cave[y][x].m_idx)//モンスターがいればボス以外眠る
					{
						if(!(r_info[m_list[cave[y][x].m_idx].r_idx].flags1 & RF1_QUESTOR))
							set_monster_csleep(cave[y][x].m_idx, 5000);
						continue;
					}

					if(!cave_have_flag_bold((y), (x), FF_WALL)) continue;
					if(cave_have_flag_bold((y), (x), FF_PERMANENT)) continue;

					if(cave[y][x].info & (CAVE_KNOWN| CAVE_MARK)) flag_mark = TRUE;

					cave_set_feat(y, x, feat_permanent);

					if(flag_mark)
					{
						cave[y][x].info |= (CAVE_KNOWN | CAVE_MARK);
//						lite_spot(y,x);
					}

				}
			}
			msg_print(_("凍りつくような閃光が発生した！", "There is a freezing flash!"));
			ident = TRUE;

			break;
		}
		//帰還の詔：一時テレポ阻害
		case SV_SCROLL_WORD_OF_RECALL:
		{
			int base = 15;
			msg_print(_("回りの大気が粘ついてきた・・",
                        "Air around you feels somehow sticky..."));
			set_nennbaku(randint1(base) + base, FALSE);
			ident = TRUE;
			break;
		}

		//炎の巻物：水に囲まれる
		case SV_SCROLL_FIRE:
		{
			msg_format(_("辺りは一瞬にして深い水の底になった。",
                        "Deep water suddenly surrounds you."));
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,600);

			ident = TRUE;
			break;
		}

		//氷の巻物：溶岩に囲まれる
		case SV_SCROLL_ICE:
		{
			msg_format(_("急に辺りが熱気に包まれた・・",
                        "It suddenly becomes extremely hot..."));
			project_hack2(GF_LAVA_FLOW,0,0,2);
			project_hack2(GF_FIRE,0,0,1200);

			ident = TRUE;
			break;
		}

		//ログルスの巻物：一部を除くモンスターがフロアから消える
		case SV_SCROLL_CHAOS:
		{
			int i;
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;
				if (i == p_ptr->riding) continue;
				if (r_ptr->flags1 & RF1_QUESTOR)
				{
					if(distance(m_ptr->fy,m_ptr->fx,py,px) <= MAX_SIGHT) teleport_away(i,100,TELEPORT_PASSIVE);
					continue;
				}

				if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
				}
				/* Delete this monster */
				delete_monster_idx(i);
			}

			msg_print(_("急に辺りが静かになった。",
                        "Suddenly it is very quiet."));
			ident = TRUE;
			break;
		}

		//☆生成→ファイナルストライクとかどうだろう
		case SV_SCROLL_ARTIFACT:
		{
			if(final_strike(0,TRUE))ident = TRUE;
			else used_up = FALSE;
			break;
		}

		//テレポレベル：フロアの壁が消える
		case SV_SCROLL_TELEPORT_LEVEL:
		{
			if(p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) break;
			msg_print(_("ダンジョンの壁が轟音を響かせて床に沈み込んでいく！",
                        "Dungeon walls sink into the floor with a grating sound!"));
			vanish_dungeon();
			ident = TRUE;
			break;
		}
		//テレポート：視界内のランダムな敵にテレポートアウェイ
		case SV_SCROLL_TELEPORT:
		{
			int i, cnt=0, idx = 0;
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!projectable(py, px, y, x)) continue;
				cnt++;
				if(one_in_(cnt)) idx = i;
			}

			if(idx)
			{
				project(0, 0, m_list[idx].fy, m_list[idx].fx, 300, GF_AWAY_ALL, (PROJECT_JUMP|PROJECT_KILL), -1);
				ident = TRUE;
			}
			break;
		}
		//ショートテレポ：減速
		case SV_SCROLL_PHASE_DOOR:
		{
			msg_print(_("急に体が重くなった気がする・・", "Your body suddenly feels heavier..."));
			(void)set_slow(randint1(15) + 15, FALSE);
			ident = TRUE;
			break;
		}

		//トラップ感知:深い穴生成
		case SV_SCROLL_DETECT_TRAP:
		{
			if(!cave_clean_bold(py,px))
			{
				msg_print(_("少し足元が揺れた気がしたが何も起こらなかった。",
                            "Your feet shake a bit, but nothing else happens."));
				break;
			}

			cave_set_feat(py, px, feat_dark_pit);
			msg_print(_("巻物を読むと、足元に大穴が空いた！",
                        "As you read the scroll, a chasm opens beneath you!"));
			ident = TRUE;

			break;
		}

		//ドア感知：足元がトラップドアになる
		//v1.1.14 ランクエ階で効果が出ないようにした
		case SV_SCROLL_DETECT_DOOR:
		{
			int x,y,dir;
			if(!cave_clean_bold(py,px) || (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest))
				|| !dun_level || (dun_level >= d_info[dungeon_type].maxdepth)
				|| quest_number(dun_level))
			{
				msg_print(_("少し足元が揺れた気がしたが何も起こらなかった。",
                            "Your feet shake a bit, but nothing else happens."));
				break;
			}
			cave_set_feat(py, px, f_tag_to_index_in_init("TRAP_TRAPDOOR"));
			if(!p_ptr->levitation) hit_trap(FALSE);
			ident = TRUE;

			break;
		}

		//アイテム感知：サモンマテリアル
		case SV_SCROLL_DETECT_ITEM:
		{
			int i, cnt=0, idx = 0;
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!projectable(py, px, y, x)) continue;
				cnt++;
				if(one_in_(cnt)) idx = i;
			}

			if(idx)
			{
				summon_material(idx);
				ident = TRUE;
			}
			break;
		}

		//透明視認　盲目になる
		case SV_SCROLL_DETECT_INVIS:
		{
			if(!p_ptr->resist_blind)
			{
				set_blind(p_ptr->blind + 5 + randint1(5));
				ident = TRUE;
			}

			break;
		}

		//対邪悪結界 フロアに混沌がばらまかれる
		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			bool flag = FALSE;
			int num = 5 + randint1(5);
			for(;num>0;num--)
			{
				int xx = randint1(cur_wid-1);
				int yy = randint1(cur_hgt-1);

				if(!cave_naked_bold(yy,xx)) continue;

				if(summon_named_creature(-1,yy,xx,MON_UNMAKER,PM_NO_PET)) flag = TRUE;

			}
			if(flag)
			{
				msg_print(_("..何か不穏な気配がする。", "...You sense a disturbing presence."));
				ident = TRUE;
			}

			break;
		}

		//守りのルーン　爆弾をばらまく
		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			int x, y;
			int r = 9;


			for (y = (py - r); y <= (py + r); y++)
			{
				for (x = (px - r); x <= (px + r); x++)
				{
					if (!in_bounds(y, x)) continue;
					if(!futo_can_place_plate(y,x) || cave[y][x].special) continue;
					if(distance(py, px, y, x) > r) continue;
					if(!projectable(py,px,y,x)) continue;

					cave[y][x].info |= CAVE_OBJECT;
					cave[y][x].mimic = feat_bomb;
					cave[y][x].special = 9;
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			msg_print(_("辺りから一斉にパチパチいう音が聞こえ始めた...",
                        "You hear cracking sounds all around you..."));
			ident = TRUE;

			break;
		}

		//抹殺の巻物　フロアの生物が分裂する
		case SV_SCROLL_GENOCIDE:
		{
			msg_print(_("フロアが胸のざわつくような気配に満たされた..",
                        "The level suddenly feels far more livelier..."));
			raising_game();
			ident = TRUE;
			break;
		}

		default:
		msg_print(_("ERROR:この巻物をサグメが読んだ時の処理が未実装",
                    "ERROR: Unimplemented logic for Sagume reading this scroll"));
		return;
	}

	///item 巻物以外の物を読んだ時の効果
	}
	else if (o_ptr->name1 == ART_POWER)
	{
		msg_print(_("「一つの指輪は全てを統べ、", "'One Ring to rule them all, "));
		msg_print(NULL);
		msg_print(_("一つの指輪は全てを見つけ、", "One Ring to find them, "));
		msg_print(NULL);
		msg_print(_("一つの指輪は全てを捕らえて", "One Ring to bring them all "));
		msg_print(NULL);
		msg_print(_("暗闇の中に繋ぎとめる。」", "and in the darkness bind them.'"));
		used_up = FALSE;

		{
			byte iy = o_ptr->iy;
			byte ix = o_ptr->ix;
			s16b next_o_idx = o_ptr->next_o_idx;
			byte marked = o_ptr->marked;
			s16b weight = o_ptr->number * o_ptr->weight;
			u16b inscription = o_ptr->inscription;

			object_prep(o_ptr, o_ptr->k_idx);
			o_ptr->iy = iy;
			o_ptr->ix = ix;
			o_ptr->next_o_idx = next_o_idx;
			o_ptr->marked = marked;
			o_ptr->inscription = inscription;
			if (item >= 0) p_ptr->total_weight += (o_ptr->weight - weight);
			p_ptr->window |= (PW_INVEN | PW_EQUIP);
			window_stuff();
		}
		msg_print(_("あなたが光る文字を読み上げると、一つの指輪から光が消えた・・",
                    "As you read the glowing runes, The One Ring stops glowing..."));

	}
	//READABLEフラグをもつアイテムがここに来るはず
	else
	{
		///mod151213 新聞追加
		if(o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			msg_print(_("新聞を読んだ。", "You read the newspaper."));
			msg_format(_("%sについての記事だ・・", "It's an article about %s..."),r_name + r_ptr->name);
			if(lore_do_probe(o_ptr->pval))
				msg_format(_("%sの情報を得た。", "You learn about %s."),r_name + r_ptr->name);

			o_ptr->ident |= (IDENT_MENTAL);
			used_up = FALSE;
		}

		//v1.1.78 追加
		//v1.1.86 アビリティカード追加
		else if (o_ptr->tval == TV_ITEMCARD || o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ABILITY_CARD)
		{
			o_ptr->ident |= (IDENT_MENTAL);
			screen_object(o_ptr, SCROBJ_FORCE_DETAIL);

			used_up = FALSE;
		}
		else if (o_ptr->name1 == ART_HYAKKI)
		{
			used_up = FALSE;
			msg_print(_("おどろおどろしい絵が描かれた絵巻だ。書かれている文字は全く読めない。",
                        "Terrifying pictures are drawn on this scroll. You can't read the writing at all."));
		}
		else
		{
			msg_print(_("ERROR:このアイテムをサグメが読んだ時の処理が設定されていない",
                        "ERROR: Undefined logic for Sagume reading this item."));
			return;
		}
	}

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item was tried */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(o_ptr))
    {
        player_log_record_scroll_use(p_log_ptr, o_ptr->k_idx);
    }
#endif

	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up)
	{
		return;
	}

	sound(SOUND_SCROLL);

	/*:::使った巻物の数を減らす。*/
	// -Hack- ファイナルストライクなどアイテムを消費したときインベントリ番号がずれるのでもう一度使った巻物を探してから減らす
	if(item >= 0)
	{
		for(item2=0;item2<INVEN_PACK;item2++)
		{
			if(inventory[item2].k_idx == scroll_k_idx) break;
		}
		if(item2 < INVEN_PACK)
		{
			inven_item_increase(item2, -1);
			inven_item_describe(item2);
			inven_item_optimize(item2);
		}
	}
	else
	{
		s16b next_o_idx = 0;

		for(item2 = cave[py][px].o_idx; item2; item2 = next_o_idx)
		{
			if(o_list[item2].k_idx == scroll_k_idx)break;
			next_o_idx = o_list[item2].next_o_idx;
		}
		if(item2)
		{
			floor_item_increase(item2, -1);
			floor_item_describe(item2);
			floor_item_optimize(item2);
		}
	}
}




/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
/*:::「読む」コマンドの効果発生*/
static void do_cmd_read_scroll_aux(int item, bool known)
{
	int         k, used_up, ident, lev;
	object_type *o_ptr;


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


	/* Take a turn */
	energy_use = 100;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen.");
#endif

		sound(SOUND_FAIL);
		return;
	}
	///class 狂戦士は巻物を読めない
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		if(one_in_(3))msg_print(_("文字が勝手に踊り出して巻物を読めない。",
                                    "The letters dance on their own; you can't read the scroll."));
		else if(one_in_(2))msg_print(_("文字が頭に入ってこなくて巻物を読めない。",
                                    "The writing simply refuses to enter your head; you can't read the scroll."));
		else msg_print(_("巻物に何が書いてあるかわからない。",
                        "You don't understand what's written here."));
		return;
	}

	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0]))
	{
		msg_print(_("静寂能力を解除した。", "You dispel your silence ability."));
		set_tim_general(0,TRUE,0,0);
		energy_use = 125;
	}


	if (p_ptr->asthma >= ASTHMA_3)
	{
		if(randint0(p_ptr->asthma) > 6600)
		{
			msg_print(_("あなたは咳き込んで文字を読むのに失敗した！",
                        "You have a coughing fit and fail to read the scroll!"));
			return;
		}
	}

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();

	/* Hex */
	//if (hex_spelling_any() && ((p_ptr->lev < 35) || hex_spell_fully())) stop_hex_spell_all();

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* Assume the scroll will get used up */
	used_up = TRUE;

	if (o_ptr->tval == TV_SCROLL)
	{
	/* Analyze the scroll */
	switch (o_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if (!(p_ptr->resist_blind) && !(p_ptr->resist_dark) && p_ptr->pclass != CLASS_NINJA)
			{
				(void)set_blind(p_ptr->blind + 3 + randint1(5));
			}
			if (unlite_area(10, 3)) ident = TRUE;
			break;
		}

		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
#ifdef JP
			msg_print("カン高くうなる様な音が辺りを覆った。");
#else
			msg_print("There is a high pitched humming noise.");
#endif

			aggravate_monsters(0,TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if (curse_armor()) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			k = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				k = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) k = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) k = INVEN_LARM;
			if (k && curse_weapon(FALSE, k)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SUMMON_MONSTER:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_PET:
		{
			if (summon_specific(-1, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_FORCE_PET)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_SUMMON_KIN:
		{
			if (summon_kin_player(p_ptr->lev, py, px, (PM_FORCE_PET | PM_ALLOW_GROUP)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if (trap_creation(py, px)) ident = TRUE;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			teleport_player(10, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			teleport_player(100, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			(void)teleport_level(0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_WORD_OF_RECALL:
		{
			if (!word_of_recall()) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_IDENTIFY:
		{
			if (!ident_spell(FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			if (!identify_fully(FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_REMOVE_CURSE:
		{
			if (remove_curse())
			{
#ifdef JP
				///msg131215
				//msg_print("誰かに見守られているような気がする。");
				msg_print("装備品の呪縛が消えた。");

#else
				msg_print("The curse on your equipment is removed.");
#endif

				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			if (remove_all_curse())
			{
#ifdef JP
				///msg131215
				//msg_print("誰かに見守られているような気がする。");
				msg_print("装備品の呪縛が消えた。");
#else
				msg_print("The curse on your equipment is removed.");
#endif
			}
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			ident = TRUE;
			if (!enchant_spell(0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if (!enchant_spell(1, 0, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if (!enchant_spell(0, 1, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (!enchant_spell(0, 0, randint1(3) + 2)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if (!enchant_spell(randint1(3), randint1(3), 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_RECHARGING:
		{
			if (!recharge(130)) used_up = FALSE;
			ident = TRUE;
			break;
		}
		/*:::凡庸の巻物*/
		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if (!mundane_spell(FALSE)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_LIGHT:
		{
			if (lite_area(damroll(2, 8), 2)) ident = TRUE;
			break;
		}

		case SV_SCROLL_MAPPING:
		{
			map_area(DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if (detect_treasure(DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_objects_gold(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if (detect_objects_normal(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_TRAP:
		{
			if (detect_traps(DETECT_RAD_DEFAULT, known)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if (detect_doors(DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_stairs(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if (detect_monsters_invis(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}
/*
		case SV_SCROLL_SATISFY_HUNGER:
		{
			if (set_food(PY_FOOD_MAX - 1)) ident = TRUE;
			break;
		}
*/
/*
		case SV_SCROLL_BLESSING:
		{
			if (set_blessed(p_ptr->blessed + randint1(12) + 6, FALSE)) ident = TRUE;
			break;
		}
*/
		case SV_SCROLL_HOLY_CHANT:
		{
			if (set_blessed(p_ptr->blessed + randint1(24) + 12, FALSE)) ident = TRUE;
			break;
		}
/*
		case SV_SCROLL_HOLY_PRAYER:
		{
			if (set_blessed(p_ptr->blessed + randint1(48) + 24, FALSE)) ident = TRUE;
			break;
		}
*/
		case SV_SCROLL_MONSTER_CONFUSION:
		{
			if (!(p_ptr->special_attack & ATTACK_CONFUSE))
			{
#ifdef JP
				msg_print("手が輝き始めた。");
#else
				msg_print("Your hands begin to glow.");
#endif

				p_ptr->special_attack |= ATTACK_CONFUSE;
				p_ptr->redraw |= (PR_STATUS);
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint1(25) + k, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			warding_glyph();
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if (destroy_doors_touch()) ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_DESTRUCTION:
		{
			if (destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE))
				ident = TRUE;
			else
#ifdef JP
msg_print("ダンジョンが揺れた...");
#else
				msg_print("The dungeon trembles...");
#endif


			break;
		}

		case SV_SCROLL_DISPEL_UNDEAD:
		{
			if (dispel_undead(80)) ident = TRUE;
			break;
		}
		///class item 呪文の巻物　廃止予定
		case SV_SCROLL_SPELL:
		{
			/*
			if ((p_ptr->pclass == CLASS_WARRIOR) ||
				(p_ptr->pclass == CLASS_IMITATOR) ||
				(p_ptr->pclass == CLASS_MINDCRAFTER) ||
				(p_ptr->pclass == CLASS_SORCERER) ||
				(p_ptr->pclass == CLASS_ARCHER) ||
				(p_ptr->pclass == CLASS_MAGIC_EATER) ||
				(p_ptr->pclass == CLASS_RED_MAGE) ||
				(p_ptr->pclass == CLASS_SAMURAI) ||
				(p_ptr->pclass == CLASS_BLUE_MAGE) ||
				(p_ptr->pclass == CLASS_CAVALRY) ||
				(p_ptr->pclass == CLASS_BERSERKER) ||
				(p_ptr->pclass == CLASS_SMITH) ||
				(p_ptr->pclass == CLASS_MIRROR_MASTER) ||
				(p_ptr->pclass == CLASS_NINJA) ||
				(p_ptr->pclass == CLASS_SNIPER)) break;
				*/
			if(!p_ptr->realm1 || p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) break;
			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA) break;

			p_ptr->add_spells++;
			p_ptr->update |= (PU_SPELLS);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_GENOCIDE:
		{
			(void)symbol_genocide(300, TRUE,0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_GENOCIDE:
		{
			(void)mass_genocide(300, TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ACQUIREMENT:
		{
			acquirement(py, px, 1, TRUE, FALSE);
			set_deity_bias(DBIAS_REPUTATION, 1);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);
			set_deity_bias(DBIAS_REPUTATION, 3);
			ident = TRUE;
			break;
		}

		/* New Hengband scrolls */
		case SV_SCROLL_FIRE:
		{
			fire_ball(GF_FIRE, 0, 666, 4);
			/* Note: "Double" damage since it is centered on the player ... */
			if (!(IS_OPPOSE_FIRE() || p_ptr->resist_fire || p_ptr->immune_fire))
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 50+randint1(50), "炎の巻物", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), "a Scroll of Fire", -1);
#endif

			ident = TRUE;
			break;
		}


		case SV_SCROLL_ICE:
		{
			fire_ball(GF_ICE, 0, 777, 4);
			if (!(IS_OPPOSE_COLD() || p_ptr->resist_cold || p_ptr->immune_cold))
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 100+randint1(100), "氷の巻物", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "a Scroll of Ice", -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_CHAOS:
		{
			fire_ball(GF_CHAOS, 0, 1000, 4);
			if (!p_ptr->resist_chaos)
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 111+randint1(111), "ログルスの巻物", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 111 + randint1(111), "a Scroll of Logrus", -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_RUMOR:
		{
#ifdef JP
			msg_print("巻物にはメッセージが書かれている:");
#else
			msg_print("There is message on the scroll. It says:");
#endif

			msg_print(NULL);
			display_rumor(TRUE);
			msg_print(NULL);
#ifdef JP
			msg_print("巻物は煙を立てて消え去った！");
#else
			msg_print("The scroll disappears in a puff of smoke!");
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_ARTIFACT:
		{
			ident = TRUE;
			if (!artifact_scroll()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_RESET_RECALL:
		{
			ident = TRUE;
			if (!reset_recall()) used_up = FALSE;
			break;
		}
		///del131216 誰得の巻物を無効に
#if 0
		case SV_SCROLL_AMUSEMENT:
		{
			ident = TRUE;
			amusement(py, px, 1, FALSE);
			break;
		}

		case SV_SCROLL_STAR_AMUSEMENT:
		{
			ident = TRUE;
			amusement(py, px,  randint1(2) + 1, FALSE);
			break;
		}
#endif
	}

	///item 巻物以外の物を読んだ時の効果
	}
///del131216 GHBシャツ
#if 0
	else if (o_ptr->name1 == ART_GHB)
	{
#ifdef JP
		msg_print("私は苦労して『グレーター・ヘル=ビースト』を倒した。");
		msg_print("しかし手に入ったのはこのきたないＴシャツだけだった。");
#else
		msg_print("I had a very hard time to kill the Greater hell-beast, ");
		msg_print("but all I got was this lousy t-shirt!");
#endif
		used_up = FALSE;
	}
#endif
	else if (o_ptr->name1 == ART_POWER)
	{
#ifdef JP
		msg_print("「一つの指輪は全てを統べ、");
		msg_print(NULL);
		msg_print("一つの指輪は全てを見つけ、");
		msg_print(NULL);
		msg_print("一つの指輪は全てを捕らえて");
		msg_print(NULL);
		msg_print("暗闇の中に繋ぎとめる。」");
#else
		msg_print("'One Ring to rule them all, ");
		msg_print(NULL);
		msg_print("One Ring to find them, ");
		msg_print(NULL);
		msg_print("One Ring to bring them all ");
		msg_print(NULL);
		msg_print("and in the darkness bind them.'");
#endif
		used_up = FALSE;
	}
	//READABLEフラグをもつアイテムがここに来るはず
	else
	{
		///mod151213 新聞追加
		if(o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			msg_print(_("新聞を読んだ。", "You read the newspaper."));
			msg_format(_("%sについての記事だ・・", "It's an article about %s..."),r_name + r_ptr->name);

			if(lore_do_probe(o_ptr->pval))
				msg_format(_("%sの情報を得た。", "You learn about %s."),r_name + r_ptr->name);


			o_ptr->ident |= (IDENT_MENTAL);
			used_up = FALSE;
		}
		//EXTRAモードのアイテムカード
		//v1.1.56 スペカも同じようにする
		//v1.1.86 アビリティカードも追加
		else if(o_ptr->tval == TV_ITEMCARD || o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ABILITY_CARD)
		{
			o_ptr->ident |= (IDENT_MENTAL);
			screen_object(o_ptr,SCROBJ_FORCE_DETAIL);

			used_up = FALSE;
		}

		//v1.1.36 百鬼夜行絵巻
		else if(o_ptr->name1 == ART_HYAKKI)
		{
			bool can_read = FALSE;
			used_up = FALSE;

			//読めそうな種族と職業(適当)
			switch(p_ptr->pclass)
			{
			case CLASS_MAMIZOU:
			case CLASS_YUKARI:
			case CLASS_KASEN:
			case CLASS_RAN:
			case CLASS_AYA:
			case CLASS_SH_DEALER:
			case CLASS_BYAKUREN:
			case CLASS_KEINE:
			case CLASS_EIRIN:
				can_read = TRUE;
			}

			switch(p_ptr->prace)
			{
			case RACE_ONI:
			case RACE_DAIYOUKAI:
			case RACE_DEITY:
			case RACE_TSUKUMO:
				can_read = TRUE;
			}

			//小鈴特殊処理？
			if(p_ptr->pclass == CLASS_KOSUZU)
				msg_print(_("あなたは絵巻を開き、ビンビンの妖気にゾクゾクした。",
                            "A chill runs down your spine from evil energy as you open the scroll."));
			else if(can_read)
				msg_print(_("この絵巻には古い邪鬼が封印されている。うかつに読み上げると危険だ。",
                            "There's an old wicked demon sealed in this scroll. Reading it aloud would be dangerous."));
			else
				msg_print(_("おどろおどろしい絵が描かれた絵巻だ。書かれている文字は全く読めない。",
                            "Terrifying pictures are drawn on this scroll. You can't read the writing at all."));

		}
		else
		{
			msg_print(_("ERROR:このアイテムを読んだ時の処理が設定されていない",
                        "ERROR: Undefined logic for reading this item."));
			return;
		}
	}


///del131216 羊皮紙
#if 0
	else if (o_ptr->tval==TV_PARCHMENT)
	{
		cptr q;
		char o_name[MAX_NLEN];
		char buf[1024];

		/* Save screen */
		screen_save();

		q=format("book-%d_jp.txt",o_ptr->sval);

		/* Display object description */
		object_desc(o_name, o_ptr, OD_NAME_ONLY);

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, q);

		/* Peruse the help file */
		(void)show_file(TRUE, buf, o_name, 0, 0);

		/* Load screen */
		screen_load();

		used_up=FALSE;
	}
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	///del131216 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* The item was tried */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(o_ptr))
    {
        player_log_record_scroll_use(p_log_ptr, o_ptr->k_idx);
    }
#endif

	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up)
	{
		return;
	}

	sound(SOUND_SCROLL);

	/* Destroy a scroll in the pack */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Hook to determine if an object is readable
 */
/*:::読めるアイテム　巻物、Tシャツ、一つの指輪、羊皮紙が含まれる*/
///mod131223 シャツと羊皮紙削除
///mod151213 READABLEフラグ処理追加
static bool item_tester_hook_readable(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if ((o_ptr->tval==TV_SCROLL) || (o_ptr->name1 == ART_POWER)) return (TRUE);
	object_flags(o_ptr, flgs);

	if (have_flag(flgs, TR_READABLE)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*:::読む*/
void do_cmd_read_scroll(void)
{
	object_type *o_ptr;
	int  item;
	cptr q, s;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Check some conditions */
	if (p_ptr->blind)
	{
#ifdef JP
		msg_print("目が見えない。");
#else
		msg_print("You can't see anything.");
#endif

		return;
	}
	if (no_lite())
	{
#ifdef JP
		msg_print("明かりがないので、暗くて読めない。");
#else
		msg_print("You have no light to read by.");
#endif

		return;
	}
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて読めない。");
#else
		msg_print("You are too confused!");
#endif

		return;
	}
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
#ifdef JP
		msg_print("この体では巻物を読めない。");
#else
		msg_print("You can't read scrolls in your current form.");
#endif
		return;
	}




	/* Restrict choices to scrolls */
	item_tester_hook = item_tester_hook_readable;

	/* Get an item */
#ifdef JP
	q = "何を読みますか? ";
	s = "読める巻物がない。";
#else
	q = "Read which scroll? ";
	s = "You have no scrolls to read.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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
	if(p_ptr->pclass == CLASS_SAGUME)
		sagume_read_scroll_aux(item, object_is_aware(o_ptr));
	else
	/* Read the scroll */
		do_cmd_read_scroll_aux(item, object_is_aware(o_ptr));
}

/*:::杖の効果発生　杖使用と魔導具術師の技*/
static int staff_effect(int sval, bool *use_charge, bool powerful, bool magic, bool known)
{
	int k;
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int detect_rad = powerful ? DETECT_RAD_DEFAULT * 3 / 2 : DETECT_RAD_DEFAULT;

	/* Analyze the staff */
	switch (sval)
	{
		case SV_STAFF_DARKNESS:
		{
			if (!(p_ptr->resist_blind) && !(p_ptr->resist_dark) && p_ptr->pclass != CLASS_NINJA)
			{
				if (set_blind(p_ptr->blind + 3 + randint1(5))) ident = TRUE;
			}
			if (unlite_area(10, (powerful ? 6 : 3))) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint1(30) + 15, FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_monsters()) ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			const int times = randint1(powerful ? 8 : 4);
			for (k = 0; k < times; k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			teleport_player((powerful ? 150 : 100), 0L);
			ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (powerful) {
				if (!identify_fully(FALSE)) *use_charge = FALSE;
			} else {
				if (!ident_spell(FALSE)) *use_charge = FALSE;
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_REMOVE_CURSE:
		{
			bool result = powerful ? remove_all_curse() : remove_curse();
			if (result)
			{
				if (magic)
				{
#ifdef JP
					///msg131216
					//msg_print("誰かに見守られているような気がする。");
					msg_print("装備品の呪縛が消えた。");

#else
					msg_print("The curse on your equipment is removed.");
#endif
				}
				else if (!p_ptr->blind)
				{
#ifdef JP
					msg_print("杖は一瞬ブルーに輝いた...");
#else
					msg_print("The staff glows blue for a moment...");
#endif

				}
				ident = TRUE;
			}
			break;
		}

		case SV_STAFF_STARLITE:
		{
			int num = damroll(5, 3);
			int y, x;
			int attempts;

			if (!p_ptr->blind && !magic)
			{
#ifdef JP
				msg_print("杖の先が明るく輝いた...");
#else
				msg_print("The end of the staff glows brightly...");
#endif

			}
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(&y, &x, py, px, 4, 0);

					if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

					if (!player_bold(y, x)) break;
				}

				project(0, 0, y, x, damroll(6 + lev / 8, 10), GF_LITE_WEAK,
						  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			if (lite_area(damroll(2, 8), (powerful ? 4 : 2))) ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area(powerful ? DETECT_RAD_MAP * 3 / 2 : DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure(detect_rad)) ident = TRUE;
			if (detect_objects_gold(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_objects_normal(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_traps(detect_rad, known)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_doors(detect_rad)) ident = TRUE;
			if (detect_stairs(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_monsters_invis(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_monsters_evil(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (hp_player(damroll((powerful ? 4 : 2), 8))) ident = TRUE;
			if (powerful) {
				if (set_blind(0)) ident = TRUE;
				if (set_poisoned(0)) ident = TRUE;
				if (set_cut(p_ptr->cut - 10)) ident = TRUE;
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			set_asthma(p_ptr->asthma - 1500);

			break;
		}

		case SV_STAFF_HEALING:
		{
			if (hp_player(powerful ? 500 : 300)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(A_INT)) ident = TRUE;
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				ident = TRUE;
#ifdef JP
				msg_print("頭がハッキリとした。");
#else
				msg_print("You feel your head clear.");
#endif

				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			//v1.1.90 +10
			if (sleep_monsters(lev+10)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			//v1.1.90 +10
			if (slow_monsters(lev+10)) ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (set_fast(randint1(30) + (powerful ? 30 : 15), FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing();
			ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(powerful ? 120 : 80)) ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_monsters(powerful ? 225 : 150)) ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(powerful ? 225 : 150)) ident = TRUE;
			k = 3 * lev;
			if (set_protevil((magic ? 0 : p_ptr->protevil) + randint1(25) + k, FALSE)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_afraid(0)) ident = TRUE;
			if (hp_player(50)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			break;
		}

		case SV_STAFF_GENOCIDE:
		{
			(void)symbol_genocide((magic ? lev + 50 : 200), TRUE,0);
			ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			if (earthquake(py, px, (powerful ? 15 : 10)))
				ident = TRUE;
			else
#ifdef JP
msg_print("ダンジョンが揺れた。");
#else
				msg_print("The dungeon trembles.");
#endif


			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			if (destroy_area(py, px, (powerful ? 18 : 13) + randint0(5), FALSE,FALSE,FALSE))
				ident = TRUE;

			break;
		}
		///del131223死人返しの杖
		/*
		case SV_STAFF_ANIMATE_DEAD:
		{
			if (animate_dead(0, py, px))
				ident = TRUE;

			break;
		}
		*/
		case SV_STAFF_MSTORM:
		{
#ifdef JP
			msg_print("強力な魔力が敵を引き裂いた！");
#else
			msg_print("Mighty magics rend your enemies!");
#endif
			project(0, (powerful ? 7 : 5), py, px,
				(randint1(200) + (powerful ? 500 : 300)) * 2, GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			//if ((p_ptr->pclass != CLASS_MAGE) && (p_ptr->pclass != CLASS_HIGH_MAGE) && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_MAGIC_EATER) && (p_ptr->pclass != CLASS_BLUE_MAGE))
			if(!(cp_ptr->magicmaster))
			{
#ifdef JP
				(void)take_hit(DAMAGE_NOESCAPE, 50, "コントロールし難い強力な魔力の解放", -1);
#else
				(void)take_hit(DAMAGE_NOESCAPE, 50, "unleashing magics too mighty to control", -1);
#endif
			}
			ident = TRUE;

			break;
		}

		case SV_STAFF_NOTHING:
		{
#ifdef JP
			msg_print("何も起らなかった。");
#else
			msg_print("Nothing happen.");
#endif
			if (prace_is_(RACE_GOLEM) || prace_is_(RACE_TSUKUMO) || prace_is_(RACE_ANDROID))
#ifdef JP
				msg_print("もったいない事をしたような気がする。食べ物は大切にしなくては。");
#else
				msg_print("What a waste.  It's your food!");
#endif
			break;
		}
	}
	return ident;
}

/*
 * Use a staff.			-RAK-
 *
 * One charge of one staff disappears.
 *
 * Hack -- staffs of identify can be "cancelled".
 */
static void do_cmd_use_staff_aux(int item)
{
	int         ident, chance, lev;
	object_type *o_ptr;
	bool success = TRUE;
	bool powerful = FALSE;


	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;


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


	/* Mega-Hack -- refuse to use a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは杖を拾わなければ。");
#else
		msg_print("You must first pick up the staffs.");
#endif

		return;
	}


	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this staff is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || ( p_ptr->pseikaku == SEIKAKU_BERSERK)) success = FALSE;
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if(!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("杖をうまく使えなかった。");
#else
		msg_print("You failed to use the staff properly.");
#endif

		///mod140829 フランはうっかり色々壊す
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			msg_print(_("杖が手の中で爆発した！", "The staff explodes in your hand!"));
			if(item >= 0)
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



		sound(SOUND_FAIL);
		return;
	}

	/* Notice empty staffs */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("この杖にはもう魔力が残っていない。");
#else
		msg_print("The staff has no charges left.");
#endif

		o_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);

		return;
	}


	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = staff_effect(o_ptr->sval, &use_charge, powerful, FALSE, object_is_aware(o_ptr));
	///del131216 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the item */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(o_ptr))
    {
        player_log_record_staff_use(p_log_ptr, o_ptr->k_idx);
    }
#endif

	/* Hack -- some uses are "free" */
	if (!use_charge) return;


	/* Use a single charge */
	o_ptr->pval--;

	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
	{
		object_type forge;
		object_type *q_ptr;

		/* Get local object */
		q_ptr = &forge;

		/* Obtain a local object */
		object_copy(q_ptr, o_ptr);

		/* Modify quantity */
		q_ptr->number = 1;

		/* Restore the charges */
		o_ptr->pval++;

		/* Unstack the used item */
		o_ptr->number--;
		p_ptr->total_weight -= q_ptr->weight;
		item = inven_carry(q_ptr);

		/* Message */
#ifdef JP
		msg_print("杖をまとめなおした。");
#else
		msg_print("You unstack your staff.");
#endif

	}

	/* Describe charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(item);
	}

	/* Describe charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}



/*::: USABLEフラグのついた特殊アイテムを使った効果 */
//今のところ使ってもアイテムは消費されない
static void do_cmd_use_special_item(int item)
{
	object_type *o_ptr;
	bool disposable = FALSE;

	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];


	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print(_("止まった時の中ではうまく働かないようだ。", "You can't do it while time is stopped."));
		sound(SOUND_FAIL);
		return;
	}
	//ウルトラソニック眠り猫　rシンボルへの抹殺
	if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_ULTRASONIC)
	{

		if(p_ptr->prace == RACE_WARBEAST || p_ptr->prace == RACE_GYOKUTO ||  p_ptr->prace == RACE_HAKUROU_TENGU
			|| p_ptr->prace == RACE_YOUKO || p_ptr->prace == RACE_BAKEDANUKI || p_ptr->muta1 & MUT1_BEAST_EAR
			|| p_ptr->pclass == CLASS_MIKO)
		{
			msg_print(_("耳をつんざくような高音が鳴り響いた！",
                        "There is a deafening high pitched sound!"));
		}
		else
		{
			msg_print(_("何かかすかな音が聞こえた気がする・・",
                        "You think you hear some faint sound..."));
		}

		(void)symbol_genocide(300, FALSE,'r');

	}
	//ウィジャボード　ランダム感知かGシンボル敵対召喚
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_OUIJA_BOARD)
	{
		int tmp = randint0(100);
		msg_print(_("あなたはプランシェットに手を置いて念じた・・",
                    "You put your hands on the planchette and concentrate your thoughts..."));

		if(tmp < 80)
		{
			msg_print(_("しかし何も起こらなかった。", "However, nothing happens."));
		}
		else if(tmp < 95)
		{
			msg_print(_("低俗霊が近くの情報を教えてくれた！",
                        "Spirits give you information about nearby area!"));
			if(one_in_(3))
				detect_objects_normal(DETECT_RAD_DEFAULT);
			else if(one_in_(2))
			{
				detect_monsters_normal(DETECT_RAD_DEFAULT);
				detect_monsters_invis(DETECT_RAD_DEFAULT);
			}
			else
				detect_traps(DETECT_RAD_DEFAULT,TRUE);
		}
		else
		{
			msg_print(_("敵対的な霊を呼び出してしまった！",
                        "You've summoned a hostile ghost!"));
			summon_specific(0,py,px,dun_level+20,SUMMON_GHOST,(PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}
	//ケセランパサラン　神や神職が触ると白いオーラ獲得
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_KESERAN_PASARAN)
	{
		if(p_ptr->pclass == CLASS_REIMU  || p_ptr->pclass == CLASS_PRIEST
			|| p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_STRAYGOD|| p_ptr->prace == RACE_DEMIGOD
			|| REALM_SPELLMASTER && cp_ptr->realm_aptitude[TV_BOOK_PUNISH] > 1
			|| p_ptr->realm1 == TV_BOOK_PUNISH || p_ptr->realm2 == TV_BOOK_PUNISH)
		{
			msg_print(_("ケセランパサランはあなたになついている。幸せな気持ちになった。",
                        "The keseran-pasaran attaches to your. You feel lucky."));
			(void)gain_random_mutation(193);//白いオーラ
		}
		else
		{
			msg_print(_("触ろうとしたら静電気のようなもので弾かれた。あまり好かれていないようだ・・",
                        "You get zapped with static electricity as you touch it. Doesn't look like it likes you..."));
		}
	}
	//v1.1.18 魔理沙が売ってた怪しい壺　フロアの妖怪狸とあやしい影の変身を解除するようにしてみた
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MARISA_TUBO)
	{
		int i;
		bool flag = FALSE;

		for(i=1;i<m_max;i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!is_original_ap(m_ptr))
			{
				if (m_ptr->mflag2 & MFLAG2_KAGE) m_ptr->mflag2 &= ~(MFLAG2_KAGE);

				m_ptr->ap_r_idx = m_ptr->r_idx;
				lite_spot(m_ptr->fy, m_ptr->fx);
				flag = TRUE;
			}
		}
		if(flag)
		{
			if(one_in_(3))
				msg_print(_("一瞬壺が僅かに膨れた気がした。", "The jar feels swollen for a moment."));
			else if(one_in_(2))
				msg_print(_("何かが壺に吸い込まれたような気がする。", "Something seems to get sucked up into the jar."));
			else
				msg_print(_("何かが慌てるような気配が伝わってきた。", "There's a sense of panic coming from the jar."));
		}
		else
		{
			if(one_in_(7))
				msg_print(_("壺を叩いてみたが何も起こらなかった..", "You tried hitting the jar, but nothing happened..."));
			else if(one_in_(6))
				msg_print(_("壺を抱えてみたが何も起こらなかった..", "You tried holding the jar, but nothing happened..."));
			else if(one_in_(5))
				msg_print(_("壺を被ってみたが何も起こらなかった..", "You tried wearing the jar, but nothing happened..."));
			else if(one_in_(4))
				msg_print(_("壺を覗いてみたが何も起こらなかった..", "You tried looking into the jar, but nothing happened..."));
			else if(one_in_(3))
				msg_print(_("壺を掲げてみたが何も起こらなかった..", "You tried raising the jar, but nothing happened..."));
			else if(one_in_(2))
				msg_print(_("壺を磨いてみたが何も起こらなかった..", "You tried polishing the jar, but nothing happened..."));
			else
				msg_print(_("壺を撫でてみたが何も起こらなかった..", "You tried stroking the jar, but nothing happened..."));
		}
	}
	//v1.1.19
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
	{
		object_type forge, *q_ptr = &forge;
		msg_print(_("壺から油を汲み出した。", "Oil comes out of the jar."));
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_FLASK_OIL));
		drop_near(q_ptr, -1, py, px);
	}
	//v1.1.42　銀の鍵
	else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_SHILVER_KEY)
	{
		msg_print(_("鍵を触っているとカチリと何かに嵌まる感触がした。",
                    "As you touch the key, you feel as if it has snapped into something."));

		if (dun_level)
		{
			alter_reality();
		}
		else
		{
			tele_town(TRUE);
		}

	}
	//v1.1.43 今更だがスマホ
	else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_SMARTPHONE)
	{
		if(p_ptr->pclass == CLASS_OUTSIDER || p_ptr->pclass == CLASS_SUMIREKO)
			msg_print(_("スマートフォンで辺りを照らした。", "You illuminate the area with the smartphone."));
		else
			msg_print(_("スマートフォンを弄っていると光りだした！", "As you were playing with the smartphone, it starts shining!"));
		lite_area(damroll(1, 2), 2);
	}
	//v1.1.56 スペカ
	else if (o_ptr->tval == TV_SPELLCARD)
	{
		use_spellcard(o_ptr);
	}
	//v1.1.86 アビリティカード
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		//ターゲットキャンセルなどした場合FALSEが返り行動順消費しない
		if(!use_ability_card(o_ptr)) return;
	}
	else
	{
		msg_format(_("ERROR:このアイテムの特殊使用処理が記述されていない",
                    "ERROR: No logic found for special use of this item."));
		return;
	}

	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Take a turn */
	energy_use = 100;


}



///mod141231 uコマンドで使用可能なアイテム（uで全部を使うオプションではない)
/*:::杖のほかにUSABLEフラグを持つアイテムを抽出*/
static bool item_tester_hook_cmd_use(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

 	if (o_ptr->tval == TV_STAFF) return TRUE;

	object_flags(o_ptr, flgs);

	if (have_flag(flgs, TR_USABLE)) return TRUE;

	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		if(o_ptr->pval >= 0 && o_ptr->pval < ABILITY_CARD_LIST_LEN && ability_card_list[o_ptr->pval].activate) return TRUE;
	}


	return FALSE;
}

/*:::杖を使う*/
void do_cmd_use_staff(void)
{
	int  item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print(_("うまく物を持てない。", "You can't hold items very well in your current form."));
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	///sys item uコマンド　USABLEフラグのアイテムの処理を追加
	/* Restrict choices to wands */
	//item_tester_tval = TV_STAFF;

	item_tester_hook = item_tester_hook_cmd_use;


	/* Get an item */
#ifdef JP
	q = "何を使いますか? ";
	s = "使えるアイテムがない。";//v1.1.86 杖→アイテム　アビリティカードとか使うようになったので
#else
	q = "Use which staff? ";
	s = "You have no staff to use.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;


	///mod141231 使用可能特殊アイテム実装
	if (item >= 0 && inventory[item].tval == TV_STAFF || item < 0 && o_list[0 - item].tval == TV_STAFF)
	{
		do_cmd_use_staff_aux(item);
	}
	else
	{
		do_cmd_use_special_item(item);

	}




}

/*:::魔法棒の効果発生　魔法棒使用と魔導具術師の術で呼ばれる*/
static int wand_effect(int sval, int dir, bool powerful, bool magic)
{
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int rad = powerful ? 3 : 2;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER)
	{
		///del131214 virtue
		//int vir = virtue_number(V_CHANCE);
		sval = randint0(SV_WAND_WONDER);
		/*
		if (vir)
		{
			if (p_ptr->virtues[vir - 1] > 0)
			{
				while (randint1(300) < p_ptr->virtues[vir - 1]) sval++;
				if (sval > SV_WAND_COLD_BALL) sval = randint0(4) + SV_WAND_ACID_BALL;
			}
			else
			{
				while (randint1(300) < (0-p_ptr->virtues[vir - 1])) sval--;
				if (sval < SV_WAND_HEAL_MONSTER) sval = randint0(3) + SV_WAND_HEAL_MONSTER;
			}
		}
		if (sval < SV_WAND_TELEPORT_AWAY)
			chg_virtue(V_CHANCE, 1);
		*/
	}

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
		{
			int dam = damroll((powerful ? 20 : 10), 10);
			if (heal_monster(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			int distance = MAX_SIGHT * (powerful ? 8 : 5);
			if (teleport_monster(dir, distance)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(dir)) ident = TRUE;
			if (powerful && disarm_traps_touch()) ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(dir)) ident = TRUE;
			if (powerful && destroy_doors_touch()) ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			int dam = powerful ? 40 + randint1(60) : 20 + randint1(30);
			if (wall_to_mud(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
			int dam = damroll((powerful ? 12 : 6), 8);
#ifdef JP
			msg_print("青く輝く光線が放たれた。");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(dir, dam);
			ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			//v1.1.90 +20
			if (sleep_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			//v1.1.90 +20
			if (slow_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			//v1.1.90 +20
			if (confuse_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			//v1.1.90 +20
			if (fear_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(dir, 80 + lev)) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(GF_POIS, dir, 12 + lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(2 + lev / 10, 6));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(20, GF_ACID, dir, damroll(6 + lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_CHARM_MONSTER:
		{
			if (charm_monster(dir, MAX(20, lev)))
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(20, GF_FIRE, dir, damroll(7 + lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(20, GF_COLD, dir, damroll(5 + lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 60 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 40 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 70 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 50 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
#ifdef JP
			msg_print("おっと、謎の魔法棒を始動させた。");
#else
			msg_print("Oops.  Wand of wonder activated.");
#endif

			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			fire_ball(GF_FIRE, dir, (powerful ? 300 : 200), -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_ball(GF_COLD, dir, (powerful ? 270 : 180), -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			int dam;
			int typ;

			switch (randint1(5))
			{
				case 1:
					dam = 240;
					typ = GF_ACID;
					break;
				case 2:
					dam = 210;
					typ = GF_ELEC;
					break;
				case 3:
					dam = 240;
					typ = GF_FIRE;
					break;
				case 4:
					dam = 210;
					typ = GF_COLD;
					break;
				default:
					dam = 180;
					typ = GF_POIS;
					break;
			}

			if (powerful) dam = (dam * 3) / 2;

			fire_ball(typ, dir, dam, -3);

			ident = TRUE;
			break;
		}

		case SV_WAND_DISINTEGRATE:
		{
			fire_ball(GF_DISINTEGRATE, dir, 200 + randint1(lev * 2), rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ROCKETS:
		{
#ifdef JP
msg_print("ロケットを発射した！");
#else
			msg_print("You launch a rocket!");
#endif

			fire_rocket(GF_ROCKET, dir, 250 + lev * 3, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_STRIKING:
		{
			fire_bolt(GF_METEOR, dir, damroll(15 + lev / 3, 13));
			ident = TRUE;
			break;
		}

		case SV_WAND_GENOCIDE:
		{
			fire_ball_hide(GF_GENOCIDE, dir, magic ? lev + 50 : 250, 0);
			ident = TRUE;
			break;
		}
	}
	return ident;
}


/*
 * Aim a wand (from the pack or floor).
 *
 * Use a single charge from a single item.
 * Handle "unstacking" in a logical manner.
 *
 * For simplicity, you cannot use a stack of items from the
 * ground.  This would require too much nasty code.
 *
 * There are no wands which can "destroy" themselves, in the inventory
 * or on the ground, so we can ignore this possibility.  Note that this
 * required giving "wand of wonder" the ability to ignore destruction
 * by electric balls.
 *
 * All wands can be "cancelled" at the "Direction?" prompt for free.
 *
 * Note that the basic "bolt" wands do slightly less damage than the
 * basic "bolt" rods, but the basic "ball" wands do the same damage
 * as the basic "ball" rods.
 */
/*:::魔法棒の使用　魔法棒選択済み　成功判定はまだ*/
static void do_cmd_aim_wand_aux(int item)
{
	int         lev, ident, chance, dir;
	object_type *o_ptr;
	bool old_target_pet = target_pet;
	bool success = TRUE;
	bool powerful = FALSE;

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

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずは魔法棒を拾わなければ。");
#else
		msg_print("You must first pick up the wands.");
#endif

		return;
	}


	/* Allow direction to be cancelled for free */
	if (object_is_aware(o_ptr) && (o_ptr->sval == SV_WAND_HEAL_MONSTER
				      || o_ptr->sval == SV_WAND_HASTE_MONSTER))
			target_pet = TRUE;
	if (!get_aim_dir(&dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	/* Take a turn */
	energy_use = 100;

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/*:::USE_DEVICE=3*/
	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this wand is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || ( p_ptr->pseikaku == SEIKAKU_BERSERK)) success = FALSE;
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if(!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("魔法棒をうまく使えなかった。");
#else
		msg_print("You failed to use the wand properly.");
#endif
		///mod140829 フランはうっかり色々壊す
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			distribute_charges(o_ptr, q_ptr, 1);

			msg_print(_("魔法棒が手の中で爆発した！", "The wand explodes in your hand!"));
			if(item >= 0)
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
		sound(SOUND_FAIL);
		return;
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("この魔法棒にはもう魔力が残っていない。");
#else
		msg_print("The wand has no charges left.");
#endif

		o_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);

		return;
	}

	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = wand_effect(o_ptr->sval, dir, powerful, FALSE);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/

	/* Mark it as tried */
	object_tried(o_ptr);

	/* Apply identification */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(o_ptr))
    {
        player_log_record_wand_use(p_log_ptr, o_ptr->k_idx);
    }
#endif

	/* Use a single charge */
	o_ptr->pval--;

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}


void do_cmd_aim_wand(void)
{
	int     item;
	cptr    q, s;


	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print(_("うまく魔法棒を持てない。", "You can't hold wands very well in your current form."));
		return ;
	}

	/* Restrict choices to wands */
	item_tester_tval = TV_WAND;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "どの魔法棒で狙いますか? ";
	s = "使える魔法棒がない。";
#else
	q = "Aim which wand? ";
	s = "You have no wand to aim.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Aim the wand */
	do_cmd_aim_wand_aux(item);
}

/*:::ロッドの効果発生　*/
static int rod_effect(int sval, int dir, bool *use_charge, bool powerful, bool magic)
{
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int detect_rad = powerful ? DETECT_RAD_DEFAULT * 3 / 2 : DETECT_RAD_DEFAULT;
	int rad = powerful ? 3 : 2;

	/* Unused */
	(void)magic;

	/* Analyze the rod */
	switch (sval)
	{
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_traps(detect_rad, (bool)(dir ? FALSE : TRUE))) ident = TRUE;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_doors(detect_rad)) ident = TRUE;
			if (detect_stairs(detect_rad)) ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			if (powerful) {
				if (!identify_fully(FALSE)) *use_charge = FALSE;
			} else {
				if (!ident_spell(FALSE)) *use_charge = FALSE;
			}
			ident = TRUE;
			break;
		}

		case SV_ROD_RECALL:
		{
			if (!word_of_recall()) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			if (lite_area(damroll(2, 8), (powerful ? 4 : 2))) ident = TRUE;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area(powerful ? DETECT_RAD_MAP * 3 / 2 : DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all(detect_rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing();
			ident = TRUE;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			set_asthma(p_ptr->asthma - 3000);
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(powerful ? 750 : 500)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level()) ident = TRUE;
			if (do_res_stat(A_STR)) ident = TRUE;
			if (do_res_stat(A_INT)) ident = TRUE;
			if (do_res_stat(A_WIS)) ident = TRUE;
			if (do_res_stat(A_DEX)) ident = TRUE;
			if (do_res_stat(A_CON)) ident = TRUE;
			if (do_res_stat(A_CHR)) ident = TRUE;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (set_fast(randint1(30) + (powerful ? 30 : 15), FALSE)) ident = TRUE;
			break;
		}

		case SV_ROD_PESTICIDE:
		{
			if (dispel_monsters(powerful ? 8 : 4)) ident = TRUE;
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			int distance = MAX_SIGHT * (powerful ? 8 : 5);
			if (teleport_monster(dir, distance)) ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(dir)) ident = TRUE;
			if (powerful && disarm_traps_touch()) ident = TRUE;
			break;
		}

		case SV_ROD_LITE:
		{
			int dam = damroll((powerful ? 12 : 6), 8);
#ifdef JP
			msg_print("青く輝く光線が放たれた。");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(dir, dam);
			ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			//v1.1.90 +30
			if (sleep_monster(dir, lev+30)) ident = TRUE;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			//v1.1.90 +30
			if (slow_monster(dir, lev+30)) ident = TRUE;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(dir, 70 + 3 * lev / 2)) ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			fire_bolt_or_beam(10, GF_ACID, dir, damroll(6 + lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			fire_bolt_or_beam(10, GF_ELEC, dir, damroll(4 + lev / 9, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			fire_bolt_or_beam(10, GF_FIRE, dir, damroll(7 + lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			fire_bolt_or_beam(10, GF_COLD, dir, damroll(5 + lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 60 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 40 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 70 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 50 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_HAVOC:
		{
			call_chaos();
			ident = TRUE;
			break;
		}

		case SV_ROD_STONE_TO_MUD:
		{
			int dam = powerful ? 40 + randint1(60) : 20 + randint1(30);
			if (wall_to_mud(dir, dam)) ident = TRUE;
			break;
		}

		case SV_ROD_AGGRAVATE:
		{
			aggravate_monsters(0,TRUE);
			ident = TRUE;
			break;
		}
	}
	return ident;
}

/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 *
 * pvals are defined for each rod in k_info. -LM-
 */
/*:::ロッドを振る詳細処理*/
static void do_cmd_zap_rod_aux(int item)
{
	int ident, chance, lev, fail;
	int dir = 0;
	object_type *o_ptr;
	bool success;
	bool powerful = FALSE;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;

	object_kind *k_ptr;

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


	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("まずはロッドを拾わなければ。");
#else
		msg_print("You must first pick up the rods.");
#endif

		return;
	}


	/* Get a direction (unless KNOWN not to need it) */
	if (((o_ptr->sval >= SV_ROD_MIN_DIRECTION) && (o_ptr->sval != SV_ROD_HAVOC) && (o_ptr->sval != SV_ROD_AGGRAVATE) && (o_ptr->sval != SV_ROD_PESTICIDE)) ||
	     !object_is_aware(o_ptr))
	{
		/* Get a direction, allow cancel */
		if (!get_aim_dir(&dir)) return;
	}


	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("Nothing happen. Maybe this rod is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("うまくロッドを使えなかった。");
#else
		msg_print("You failed to use the rod properly.");
#endif
		///mod140829 フランはうっかり色々壊す
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			distribute_charges(o_ptr, q_ptr, 1);

			msg_print(_("ロッドが手の中で爆発した！", "The rod explodes in your hand!"));
			if(item >= 0)
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
		sound(SOUND_FAIL);
		return;
	}

	k_ptr = &k_info[o_ptr->k_idx];

	/* A single rod is still charging */
	if ((o_ptr->number == 1) && (o_ptr->timeout))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("このロッドはまだ魔力を充填している最中だ。");
#else
		msg_print("The rod is still charging.");
#endif

		return;
	}
	/* A stack of rods lacks enough energy. */
	else if ((o_ptr->number > 1) && (o_ptr->timeout > k_ptr->pval * (o_ptr->number - 1)))
	{
		if (flush_failure) flush();
#ifdef JP
msg_print("そのロッドはまだ充填中です。");
#else
		msg_print("The rods are all still charging.");
#endif

		return;
	}

	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = rod_effect(o_ptr->sval, dir, &use_charge, powerful, FALSE);

	/* Increase the timeout by the rod kind's pval. -LM- */
	if (use_charge) o_ptr->timeout += k_ptr->pval;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	//if (!(object_is_aware(o_ptr)))
	//{
	//	chg_virtue(V_PATIENCE, -1);
	//	chg_virtue(V_CHANCE, 1);
	//	chg_virtue(V_KNOWLEDGE, -1);
	//}

	/* Tried the object */
	object_tried(o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

#ifdef NEW_PLAYER_LOG
	if (object_is_aware(o_ptr))
    {
        player_log_record_rod_use(p_log_ptr, o_ptr->k_idx);
    }
#endif
}

/*:::ロッドを振る*/
void do_cmd_zap_rod(void)
{
	int item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print(_("うまくロッドを持てない。", "You can't hold rods very well in your current form."));
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to rods */
	item_tester_tval = TV_ROD;

	/* Get an item */
#ifdef JP
	q = "どのロッドを振りますか? ";
	s = "使えるロッドがない。";
#else
	q = "Zap which rod? ";
	s = "You have no rod to zap.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Zap the rod */
	do_cmd_zap_rod_aux(item);
}


/*
 * Hook to determine if an object is activatable
 */
/*:::発動可能なアイテムを判定*/
//v1.1.46 女苑の指輪発動で使うためにstaticを外してexternした
bool item_tester_hook_activate(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Not known */
	/*:::未鑑定アイテムは発動できない*/
	if (!object_is_known(o_ptr)) return (FALSE);

	///mod160504 銃は発動でなく射撃から扱うことにした
	if (o_ptr->tval == TV_GUN) return (FALSE);

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Check activation flag */
	if (have_flag(flgs, TR_ACTIVATE)) return (TRUE);

	/* Assume not */
	return (FALSE);
}


/*
 * Hack -- activate the ring of power
 */
/*:::信じがたいこと*/
void ring_of_power(int dir)
{
	/* Pick a random effect */
	switch (randint1(10))
	{
		case 1:
		case 2:
		{

			/* Message */
#ifdef JP
			msg_print("あなたは悪性のオーラに包み込まれた。");
#else
			msg_print("You are surrounded by a malignant aura.");
#endif
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format(_("しかし指輪の呪いを吸い取った！", "You absorb the ring's curse!"));
				hina_gain_yaku(500 + randint1(500));
				return;
			}
			//v1.1.48 紫苑は呪いを不運パワーとして吸収する
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは強大な呪いを吸収した！", "You absorb the powerful curse!"));
				p_ptr->magic_num1[1] += 25000 + randint1(25000);
				return ;
			}


			sound(SOUND_EVIL);

			/* Decrease all stats (permanently) */
			(void)dec_stat(A_STR, 50, TRUE);
			(void)dec_stat(A_INT, 50, TRUE);
			(void)dec_stat(A_WIS, 50, TRUE);
			(void)dec_stat(A_DEX, 50, TRUE);
			(void)dec_stat(A_CON, 50, TRUE);
			(void)dec_stat(A_CHR, 50, TRUE);

			/* Lose some experience (permanently) */
			p_ptr->exp -= (p_ptr->exp / 4);
			p_ptr->max_exp -= (p_ptr->exp / 4);
			check_experience();

			break;
		}

		case 3:
		{
			/* Message */
#ifdef JP
			msg_print("あなたは強力なオーラに包み込まれた。");
#else
			msg_print("You are surrounded by a powerful aura.");
#endif


			/* Dispel monsters */
			dispel_monsters(1000);

			break;
		}

		case 4:
		case 5:
		case 6:
		{
			/* Mana Ball */
			fire_ball(GF_MANA, dir, 600, 3);

			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			/* Mana Bolt */
			fire_bolt(GF_MANA, dir, 500);

			break;
		}
	}
}


bool ang_sort_comp_pet(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	monster_type *m_ptr1 = &m_list[w1];
	monster_type *m_ptr2 = &m_list[w2];
	monster_race *r_ptr1 = &r_info[m_ptr1->r_idx];
	monster_race *r_ptr2 = &r_info[m_ptr2->r_idx];

	/* Unused */
	(void)v;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->hp > m_ptr2->hp) return TRUE;
	if (m_ptr2->hp > m_ptr1->hp) return FALSE;

	return w1 <= w2;
}



/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *:::装備中のアイテムを発動する。発動可能なアイテムは装備中でなくてもまとめられない。
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *:::(古い記述らしい。無視)
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 *:::発動後、方向指定時にキャンセルしたとしてもターンを消費する。
 */
/*:::アイテム発動（発動可能なアイテムを選択後）*/
//v1.1.46 女苑の指輪発動から使うためにstatic外してexternした。
//itemが負の場合inven_add[]からアイテム情報を取得して発動することにした
void do_cmd_activate_aux(int item)
{
	int         dir, lev, chance, fail;
	object_type *o_ptr;
	bool success;

	/*:::床上のアイテムも処理してるが装備品でしかここには来ない*/

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		//v1.1.46 HACK - itemがマイナスのとき床上でなく追加インベントリinven_add[]からアイテム情報を取得することにした。
		o_ptr = &inven_add[-1 - item];
	}

	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Hack -- use artifact level instead */
	if (object_is_fixed_artifact(o_ptr)) lev = a_info[o_ptr->name1].level;
	else if (object_is_random_artifact(o_ptr))
	{
		/*:::元々発動可能なアイテムが☆になった場合、普段はk_info[]のlevelが使われるがオリジナルの発動が付加された場合act_ptrがFALSEでなくなりここで情報が上書きされるらしい*/
		const activation_type* const act_ptr = find_activation_info(o_ptr);
		if (act_ptr) {
			lev = act_ptr->level;
		}
	}
	else if (((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) && o_ptr->name2) lev = e_info[o_ptr->name2].level;

	/*:::成功率を計算*/
	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("止まった時の中ではうまく働かないようだ。");
#else
		msg_print("It shows no reaction as the time has stopped.");
#endif
		sound(SOUND_FAIL);
		return;
	}
	if (cheat_xtra) msg_format("ACTIVATE: k_idx:%d lev:%d chance:%d fail:%d", o_ptr->k_idx,lev, chance, fail);
	///sys class 狂戦士は必ず発動を失敗する
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	///mod140821 特殊処理　エンジニアは必ずクリムゾンの発動に成功する
	///mod160503 廃止
	//if(p_ptr->pclass == CLASS_ENGINEER && o_ptr->name1 == ART_CRIMSON) success = TRUE;

	//高レベルな付与ハイメイジは必ず成功する
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_ENCHANT && p_ptr->lev > 39) success = TRUE;

	///mod150910 クラウンピースは必ず松明の発動に成功する
	if(p_ptr->pclass == CLASS_CLOWNPIECE && o_ptr->name1 == ART_CLOWNPIECE) success = TRUE;
	if (p_ptr->pclass == CLASS_VFS_CLOWNPIECE && o_ptr->name1 == ART_CLOWNPIECE) success = TRUE;


	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("うまく始動させることができなかった。");
#else
		msg_print("You failed to activate it properly.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	/* Check the recharge */
	if (o_ptr->timeout)
	{
#ifdef JP
		msg_print("それは微かに音を立て、輝き、消えた...");
#else
		msg_print("It whines, glows and fades...");
#endif
		return;
	}

	/* Some lights need enough fuel for activation */
	/*:::たいまつやランタンの発動付アイテムは、発動時に燃料を消費？
	/*:::・・はしない。燃料0でも発動できる。tvalの指定を間違えてるんじゃなかろうか。バグ？*/
	///item TVAL ランタンなどの発動時燃料消費？
	if (!o_ptr->xtra4 && (o_ptr->tval == TV_FLASK) &&
		((o_ptr->sval == SV_LITE_TORCH) || (o_ptr->sval == SV_LITE_LANTERN)))
	{
#ifdef JP
		msg_print("燃料がない。");
#else
		msg_print("It has no fuel.");
#endif
		energy_use = 0;
		return;
	}

	/* Activate the artifact */
	msg_print(_("始動させた...", "You activate it..."));

	/* Sound */
	sound(SOUND_ZAP);

	/* Activate object */
	if (activation_index(o_ptr))
	{
		/*:::アイテム発動効果　☆専用関数というわけではない*/
		(void)activate_random_artifact(o_ptr, item);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
	}

	/* Special items */
	/*:::魔法の笛とモンスターボールの記述はここ*/
	else if (o_ptr->tval == TV_WHISTLE)
	{
		if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
		if (hex_spelling_any()) stop_hex_spell_all();

#if 0
		if (object_is_cursed(o_ptr))
		{
#ifdef JP
			msg_print("カン高い音が響き渡った。");
#else
			msg_print("You produce a shrill whistling sound.");
#endif
			aggravate_monsters(0,TRUE);
		}
		else
#endif
		{
			int pet_ctr, i;
			u16b *who;
			int max_pet = 0;
			u16b dummy_why;

			/* Allocate the "who" array */
			C_MAKE(who, max_m_idx, u16b);

			/* Process the monsters (backwards) */
			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				if (is_pet(&m_list[pet_ctr]) && (p_ptr->riding != pet_ctr))
				  who[max_pet++] = pet_ctr;
			}

			/* Select the sort method */
			ang_sort_comp = ang_sort_comp_pet;
			ang_sort_swap = ang_sort_swap_hook;

			ang_sort(who, &dummy_why, max_pet);

			/* Process the monsters (backwards) */
			for (i = 0; i < max_pet; i++)
			{
				pet_ctr = who[i];
				teleport_monster_to(pet_ctr, py, px, 100, TELEPORT_PASSIVE);
			}

			/* Free the "who" array */
			C_KILL(who, max_m_idx, u16b);
		}
		o_ptr->timeout = 100+randint1(100);
		return;
	}
	else if (o_ptr->tval == TV_CAPTURE)
	{
		/*:::モンスターボール（捕獲時）*/
		if(!o_ptr->pval)
		{
			bool old_target_pet = target_pet;
			target_pet = TRUE;
			if (!get_aim_dir(&dir))
			{
				target_pet = old_target_pet;
				return;
			}
			target_pet = old_target_pet;

			if(fire_ball(GF_CAPTURE, dir, 0, 0))
			{
				/*:::モンスターのパラメータをモンスターボールに格納　専用グローバル変数使用*/
				o_ptr->pval = cap_mon;
				o_ptr->xtra3 = cap_mspeed;
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
					for (s = buf;*s && (*s != '#'); s++)
					{
#ifdef JP
						if (iskanji(*s)) s++;
#endif
					}
					*s = '#';
					s++;
#ifdef JP
 /*nothing*/
#else
					*s++ = '\'';
#endif
					t = quark_str(cap_nickname);
					while (*t)
					{
						*s = *t;
						s++;
						t++;
					}
#ifdef JP
 /*nothing*/
#else
					*s++ = '\'';
#endif
					*s = '\0';
					o_ptr->inscription = quark_add(buf);
				}
			}
		}
		/*:::モンスターボール（解放時）*/
		else
		{
			bool release_success = FALSE;
			//v1.1.16 妖魔本パラメータ不正に対応
			if(o_ptr->pval < 0 || o_ptr->pval >= max_r_idx)
			{
				msg_format(_("ERROR:妖魔本のpval値がおかしい(%d)",
                            "ERROR: Weird pval (%d) for demon book"),o_ptr->pval);
				return;
			}

			if (!get_rep_dir2(&dir)) return;

			if (monster_can_enter(py + ddy[dir], px + ddx[dir], &r_info[o_ptr->pval], 0))
			{
				if (place_monster_aux(0, py + ddy[dir], px + ddx[dir], o_ptr->pval, (PM_FORCE_PET | PM_NO_KAGE)))
				{
					if (o_ptr->xtra3) m_list[hack_m_idx_ii].mspeed = o_ptr->xtra3;
					if (o_ptr->xtra5) m_list[hack_m_idx_ii].max_maxhp = o_ptr->xtra5;
					if (o_ptr->xtra4) m_list[hack_m_idx_ii].hp = o_ptr->xtra4;
					m_list[hack_m_idx_ii].maxhp = m_list[hack_m_idx_ii].max_maxhp;
					if (o_ptr->inscription)
					{
						char buf[160];
						cptr t;
#ifndef JP
						bool quote = FALSE;
#endif

						t = quark_str(o_ptr->inscription);
						for (t = quark_str(o_ptr->inscription);*t && (*t != '#'); t++)
						{
#ifdef JP
							if (iskanji(*t)) t++;
#endif
						}
						if (*t)
						{
							char *s = buf;
							t++;
#ifdef JP
							/* nothing */
#else
							if (*t =='\'')
							{
								t++;
								quote = TRUE;
							}
#endif
							while(*t)
							{
								*s = *t;
								t++;
								s++;
							}
#ifdef JP
							/* nothing */
#else
							if (quote && *(s-1) =='\'')
								s--;
#endif
							*s = '\0';
							m_list[hack_m_idx_ii].nickname = quark_add(buf);
							t = quark_str(o_ptr->inscription);
							s = buf;
							while(*t && (*t != '#'))
							{
								*s = *t;
								t++;
								s++;
							}
							*s = '\0';
							o_ptr->inscription = quark_add(buf);
						}
					}
					o_ptr->pval = 0;
					o_ptr->xtra3 = 0;
					o_ptr->xtra4 = 0;
					o_ptr->xtra5 = 0;
					release_success = TRUE;
				}
			}
			if (!release_success)
#ifdef JP
				msg_print("おっと、解放に失敗した。");
#else
				msg_print("Oops.  You failed to release your follower.");
#endif
		}
		return;
	}

	/* Mistake */
#ifdef JP
	msg_print("おっと、このアイテムは始動できない。");
#else
	msg_print("Oops.  That object cannot be activated.");
#endif

}

/*:::アイテムを発動*/
void do_cmd_activate(void)
{
	int     item;
	cptr    q, s;


	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print(_("この姿ではアイテムの発動ができない。", "You cannot activate items in your current form."));
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Prepare the hook */
	item_tester_hook = item_tester_hook_activate;

	/* Get an item */
#ifdef JP
	q = "どのアイテムを始動させますか? ";
	s = "始動できるアイテムを装備していない。";
#else
	q = "Activate which item? ";
	s = "You have nothing to activate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP))) return;

	/* Activate the item */
	do_cmd_activate_aux(item);
}


/*
 * Hook to determine if an object is useable
 */
static bool item_tester_hook_use(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Ammo */
	//if (o_ptr->tval == p_ptr->tval_ammo)
	//	return (TRUE);



	//v1.1.38
	object_flags(o_ptr, flgs);
	if(have_flag(flgs,TR_USABLE))
		return TRUE;


	/* Useable object */
	switch (o_ptr->tval)
	{
		//case TV_SPIKE:
		case TV_STAFF:
		case TV_WAND:
		case TV_ROD:
		case TV_SCROLL:
		case TV_POTION:
		case TV_FOOD:
		{
			return (TRUE);
		}
		//v1.1.86
		case TV_ABILITY_CARD:
		{
			if (ability_card_list[o_ptr->pval].activate) return TRUE;
			else return FALSE;
		}

		default:
		{
			int i;

			/* Not known */
			if (!object_is_known(o_ptr)) return (FALSE);

			/* HACK - only items from the equipment can be activated */
			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				if (&inventory[i] == o_ptr)
				{
					/* Extract the flags */
					//object_flags(o_ptr, flgs); //上にコピーした

					/* Check activation flag */
					//mod160504 銃は除く
					if (o_ptr->tval != TV_GUN && have_flag(flgs, TR_ACTIVATE)) return (TRUE);
				}
			}
		}
	}

	/* Assume not */
	return (FALSE);
}


/*
 * Use an item
 * XXX - Add actions for other item types
 */
void do_cmd_use(void)
{
	int         item;
	object_type *o_ptr;
	cptr        q, s;
	u32b flgs[TR_FLAG_SIZE];

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Prepare the hook */
	item_tester_hook = item_tester_hook_use;

	/* Get an item */
#ifdef JP
q = "どれを使いますか？";
s = "使えるものがありません。";
#else
	q = "Use which item? ";
	s = "You have nothing to use.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return;

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


	/* Ammo */
	//if (o_ptr->tval == p_ptr->tval_ammo)
	//	return (TRUE);

	//v1.1.38
	object_flags(o_ptr, flgs);
	if(have_flag(flgs,TR_USABLE))
	{

		do_cmd_use_special_item(item);
		return;
	}

	switch (o_ptr->tval)
	{
		/* Spike a door */
		//くさびを打つコマンドを削除
	/*
		case TV_SPIKE:
		{

			do_cmd_spike();
			break;
		}
	*/
		/* Eat some food */
		case TV_FOOD:
		{
			do_cmd_eat_food_aux(item);
			break;
		}

		/* Aim a wand */
		case TV_WAND:
		{
			do_cmd_aim_wand_aux(item);
			break;
		}

		/* Use a staff */
		case TV_STAFF:
		{
			do_cmd_use_staff_aux(item);
			break;
		}

		/* Zap a rod */
		case TV_ROD:
		{
			do_cmd_zap_rod_aux(item);
			break;
		}

		case TV_ABILITY_CARD:
		{
			do_cmd_use_special_item(item);
			break;

		}

		/* Quaff a potion */
		case TV_POTION:
		{
			do_cmd_quaff_potion_aux(item);
			break;
		}

		/* Read a scroll */
		case TV_SCROLL:
		{
			/* Check some conditions */
			if (p_ptr->blind)
			{
#ifdef JP
msg_print("目が見えない。");
#else
				msg_print("You can't see anything.");
#endif

				return;
			}
			if (no_lite())
			{
#ifdef JP
msg_print("明かりがないので、暗くて読めない。");
#else
				msg_print("You have no light to read by.");
#endif

				return;
			}
			if (p_ptr->confused)
			{
#ifdef JP
msg_print("混乱していて読めない！");
#else
				msg_print("You are too confused!");
#endif

				return;
			}
			if(p_ptr->pclass == CLASS_SAGUME)
				sagume_read_scroll_aux(item, object_is_aware(o_ptr));
			else
			  do_cmd_read_scroll_aux(item, TRUE);
		  break;
		}

		/* Fire ammo */
		///mod140308 uコマンドでの射撃廃止
		/*
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			do_cmd_fire_aux(item, &inventory[INVEN_BOW]);
			break;
		}
		*/
		/* Activate an artifact */
		default:
		{
			do_cmd_activate_aux(item);
			break;
		}
	}
}

/*:::魔導具術師　吸収したアイテムから選択する*/
///mod150401 充填フラグを付けてextern
int select_magic_eater(bool only_browse, bool recharge)
{
	int ext=0;
	char choice;
	bool flag, request_list;
	int tval = 0;
	int             ask = TRUE, i = 0;
	char            out_val[160];

	int menu_line = (use_menu ? 1 : 0);

#ifdef ALLOW_REPEAT
	int sn;
	if (repeat_pull(&sn) )
	{
		if(recharge) return sn;//追加

		/* Verify the spell */
		if (sn >= EATER_EXT*2 && !(p_ptr->magic_num1[sn] > k_info[lookup_kind(TV_ROD, sn-EATER_EXT*2)].pval * (p_ptr->magic_num2[sn] - 1) * EATER_ROD_CHARGE))
			return sn;
		else if (sn < EATER_EXT*2 && !(p_ptr->magic_num1[sn] < EATER_CHARGE))
			return sn;
	}

#endif /* ALLOW_REPEAT */

	for (i = 0; i < (EATER_EXT * 3); i++)
	{
		if (p_ptr->magic_num2[i]) break;
	}
	if (i == (EATER_EXT * 3))
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("魔道具を隠し持っていない！");
		else
			msg_print("魔法を覚えていない！");
#else
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("You're not hiding magic devices!");
		else
			msg_print("You don't have any magic!");
#endif
		return -1;
	}

	if (use_menu)
	{
		screen_save();

		while(!tval)
		{
#ifdef JP
			prt(format(" %s 杖", (menu_line == 1) ? "》" : "  "), 2, 14);
			prt(format(" %s 魔法棒", (menu_line == 2) ? "》" : "  "), 3, 14);
			prt(format(" %s ロッド", (menu_line == 3) ? "》" : "  "), 4, 14);
#else
			prt(format(" %s staff", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s wand", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s rod", (menu_line == 3) ? "> " : "  "), 4, 14);
#endif

			if(p_ptr->pclass == CLASS_SEIJA)
			{
				if (only_browse) prt(_("どの種類の魔道具を見ますか？", "Which kind of magic device do you browse?"), 0, 0);
				else if (recharge) prt(_("どの種類の魔道具を充填しますか？", "Which kind of magic device do you recharge?"), 0, 0);
				else prt(_("どの種類の魔道具を使いますか？", "Which kind of magic device do you use?"), 0, 0);
			}
			else
			{
				if (only_browse) prt(_("どの種類の魔法を見ますか？", "Which type of magic do you browse?"), 0, 0);
				else prt(_("どの種類の魔法を使いますか？", "Which type of magic do you use?"), 0, 0);
			}
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return -1;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 2;
				break;
			case '\r':
			case 'x':
			case 'X':
				ext = (menu_line-1)*EATER_EXT;
				if (menu_line == 1) tval = TV_STAFF;
				else if (menu_line == 2) tval = TV_WAND;
				else tval = TV_ROD;
				break;
			}
			if (menu_line > 3) menu_line -= 3;
		}
		screen_load();
	}
	else
	{
	while (TRUE)
	{
#ifdef JP
		if (!get_com("[A] 杖, [B] 魔法棒, [C] ロッド:", &choice, TRUE))
#else
		if (!get_com("[A] staff, [B] wand, [C] rod:", &choice, TRUE))
#endif
		{
			return -1;
		}
		if (choice == 'A' || choice == 'a')
		{
			ext = 0;
			tval = TV_STAFF;
			break;
		}
		if (choice == 'B' || choice == 'b')
		{
			ext = EATER_EXT;
			tval = TV_WAND;
			break;
		}
		if (choice == 'C' || choice == 'c')
		{
			ext = EATER_EXT*2;
			tval = TV_ROD;
			break;
		}
	}
	}
	for (i = ext; i < ext + EATER_EXT; i++)
	{
		if (p_ptr->magic_num2[i])
		{
			if (use_menu) menu_line = i-ext+1;
			break;
		}
	}
	if (i == ext+EATER_EXT)
	{
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print(_("その種類の魔道具を持っていない！", "You don't have magic devices of this kind!"));
		else
			msg_print(_("その種類の魔法は覚えていない！", "You haven't learned magic of this kind!"));
		return -1;
	}

	/* Nothing chosen yet */
	flag = FALSE;

	if(p_ptr->pclass == CLASS_SEIJA)
	{
		if (only_browse) strnfmt(out_val, 78, _("('*'で一覧, ESCで中断) どの魔道具を見ますか？",	"(*=List, ESC=exit) Browse which magic device? "));
		else if (recharge) strnfmt(out_val, 78, _("('*'で一覧, ESCで中断) どの魔道具を充填しますか？",	"(*=List, ESC=exit) Recharge which magic device? "));
		else strnfmt(out_val, 78, _("('*'で一覧, ESCで中断) どの魔道具を使いますか？","(*=List, ESC=exit) Use which magic device? "));

	}
	else
	{
		if (only_browse) strnfmt(out_val, 78, _("('*'で一覧, ESCで中断) どの魔力を見ますか？","(*=List, ESC=exit) Browse which power? "));
		else strnfmt(out_val, 78, _("('*'で一覧, ESCで中断) どの魔力を使いますか？","(*=List, ESC=exit) Use which power? "));
	}

	/* Build a prompt */

	/* Save the screen */
	screen_save();

	request_list = always_show_list;

	/* Get a spell from the user */
	while (!flag)
	{
		/* Show the list */
		if (request_list || use_menu)
		{
			byte y, x = 0;
			int ctr, chance;
			int k_idx;
			char dummy[80];
			int x1, y1, level;
			byte col;

			strcpy(dummy, "");

			for (y = 1; y < 20; y++)
				prt("", y, x);

			y = 1;

			/* Print header(s) */
#ifdef JP
			prt(format("                           %s 失率                           %s 失率", (tval == TV_ROD ? "  状態  " : "使用回数"), (tval == TV_ROD ? "  状態  " : "使用回数")), y++, x);
#else
			prt(format("                           %s Fail                           %s Fail", (tval == TV_ROD ? "  Stat  " : " Charges"), (tval == TV_ROD ? "  Stat  " : " Charges")), y++, x);
#endif

			/* Print list */
			for (ctr = 0; ctr < EATER_EXT; ctr++)
			{
				if (!p_ptr->magic_num2[ctr+ext]) continue;

				k_idx = lookup_kind(tval, ctr);

				if (use_menu)
				{
					if (ctr == (menu_line-1))
#ifdef JP
						strcpy(dummy, "》");
#else
					strcpy(dummy, "> ");
#endif
					else strcpy(dummy, "  ");

				}
				/* letter/number for power selection */
				else
				{
					char letter;
					if (ctr < 26)
						letter = I2A(ctr);
					else
						letter = '0' + ctr - 26;
					sprintf(dummy, "%c)",letter);
				}
				x1 = ((ctr < EATER_EXT/2) ? x : x + 40);
				y1 = ((ctr < EATER_EXT/2) ? y + ctr : y + ctr - EATER_EXT/2);
				level = (tval == TV_ROD ? k_info[k_idx].level * 5 / 6 - 5 : k_info[k_idx].level);
				chance = level * 4 / 5 + 20;
				chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);
				level /= 2;
				if (p_ptr->lev > level)
				{
					chance -= 3 * (p_ptr->lev - level);
				}
				chance = mod_spell_chance_1(chance);
				chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]]);
				/* Stunning makes spells harder */
				if (p_ptr->stun > 50) chance += 25;
				else if (p_ptr->stun) chance += 15;

				if (chance > 95) chance = 95;

				chance = mod_spell_chance_2(chance);

				col = TERM_WHITE;

				if (k_idx)
				{
					if (tval == TV_ROD)
					{
						strcat(dummy, format(
#ifdef JP
							       " %-22.22s 充填:%2ld/%2d%3d%%",
#else
							       " %-22.22s   (%2ld/%2d) %3d%%",
#endif
							       k_name + k_info[k_idx].name,
							       p_ptr->magic_num1[ctr+ext] ?
							       (p_ptr->magic_num1[ctr+ext] - 1) / (EATER_ROD_CHARGE * k_info[k_idx].pval) +1 : 0,
							       p_ptr->magic_num2[ctr+ext], chance));
						if (p_ptr->magic_num1[ctr+ext] > k_info[k_idx].pval * (p_ptr->magic_num2[ctr+ext]-1) * EATER_ROD_CHARGE) col = TERM_RED;
					}
					else
					{
						strcat(dummy, format(" %-22.22s    %2d/%2d %3d%%", k_name + k_info[k_idx].name, (s16b)(p_ptr->magic_num1[ctr+ext]/EATER_CHARGE), p_ptr->magic_num2[ctr+ext], chance));
						if (p_ptr->magic_num1[ctr+ext] < EATER_CHARGE) col = TERM_RED;
					}
				}
				else
					strcpy(dummy, "");
				c_prt(col, dummy, y1, x1);
			}
		}

		if (!get_com(out_val, &choice, FALSE)) break;

		if (use_menu && choice != ' ')
		{
			switch (choice)
			{
				case '0':
				{
					screen_load();
					return 0;
				}

				case '8':
				case 'k':
				case 'K':
				{
					do
					{
						menu_line += EATER_EXT - 1;
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!p_ptr->magic_num2[menu_line+ext-1]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!p_ptr->magic_num2[menu_line+ext-1]);
					break;
				}

				case '4':
				case 'h':
				case 'H':
				case '6':
				case 'l':
				case 'L':
				{
					bool reverse = FALSE;
					if ((choice == '4') || (choice == 'h') || (choice == 'H')) reverse = TRUE;
					if (menu_line > EATER_EXT/2)
					{
						menu_line -= EATER_EXT/2;
						reverse = TRUE;
					}
					else menu_line+=EATER_EXT/2;
					while(!p_ptr->magic_num2[menu_line+ext-1])
					{
						if (reverse)
						{
							menu_line--;
							if (menu_line < 2) reverse = FALSE;
						}
						else
						{
							menu_line++;
							if (menu_line > EATER_EXT-1) reverse = TRUE;
						}
					}
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
		}

		/* Request redraw */
		if (use_menu && ask) continue;

		/* Request redraw */
		if (!use_menu && ((choice == ' ') || (choice == '*') || (choice == '?')))
		{
			/* Hide the list */
			if (request_list)
			{
				/* Hide list */
				request_list = FALSE;

				/* Restore the screen */
				screen_load();
				screen_save();
			}
			else
				request_list = TRUE;

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			if (isalpha(choice))
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if (ask) choice = tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}
			else
			{
				ask = FALSE; /* Can't uppercase digits */

				i = choice - '0' + 26;
			}
		}

		/* Totally Illegal */
		if ((i < 0) || (i > EATER_EXT) || !p_ptr->magic_num2[i+ext])
		{
			bell();
			continue;
		}

		if (!only_browse)
		{
			/* Verify it */
			if (ask)
			{
				char tmp_val[160];

				/* Prompt */
#ifdef JP
				(void) strnfmt(tmp_val, 78, "%sを使いますか？ ", k_name + k_info[lookup_kind(tval ,i)].name);
#else
				(void) strnfmt(tmp_val, 78, "Use %s?", k_name + k_info[lookup_kind(tval ,i)].name);
#endif

				/* Belay that order */
				if (!get_check(tmp_val)) continue;
			}
			if (tval == TV_ROD)
			{
				if (p_ptr->magic_num1[ext+i]  > k_info[lookup_kind(tval, i)].pval * (p_ptr->magic_num2[ext+i] - 1) * EATER_ROD_CHARGE)
				{
					if(!recharge)
					{

						if(p_ptr->pclass == CLASS_SEIJA)
							msg_print(_("その魔道具はまだ使えない。", "You can't use this magic device yet."));
						else
							msg_print(_("その魔法はまだ充填している最中だ。", "This power is still charging."));
						msg_print(NULL);
						if (use_menu) ask = TRUE;
						continue;
					}
				}
			}
			else
			{
				if (p_ptr->magic_num1[ext+i] < EATER_CHARGE)
				{

					if(!recharge)
					{

						if(p_ptr->pclass == CLASS_SEIJA)
							msg_print(_("その魔道具は使用回数が切れている。", "This magic device is out of charges."));
						else
							msg_print(_("その魔法は使用回数が切れている。", "This power is out of charges."));
						msg_print(NULL);
						if (use_menu) ask = TRUE;
						continue;
					}
				}
			}
		}

		/* Browse */
		else
		{
			int line, j;
			char temp[70 * 20];

			/* Clear lines, position cursor  (really should use strlen here) */
			Term_erase(7, 23, 255);
			Term_erase(7, 22, 255);
			Term_erase(7, 21, 255);
			Term_erase(7, 20, 255);

			roff_to_buf(k_text + k_info[lookup_kind(tval, i)].text, 62, temp, sizeof(temp));
			for (j = 0, line = 21; temp[j]; j += 1 + strlen(&temp[j]))
			{
				prt(&temp[j], line, 10);
				line++;
			}

			continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	screen_load();

	if (!flag) return -1;

#ifdef ALLOW_REPEAT
	repeat_push(ext+i);
#endif /* ALLOW_REPEAT */
	return ext+i;
}


/*
 *  Use eaten rod, wand or staff
 */
/*:::魔導具術士の技*/
bool do_cmd_magic_eater(bool only_browse, bool powerful)
{
	int item, chance, level, k_idx, tval, sval;
	bool use_charge = TRUE;

	/* Not when confused */
	if (!only_browse && p_ptr->confused)
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("混乱していて魔道具を取り出せない！");
		else
			msg_print("混乱していて唱えられない！");
#else
		msg_print("You are too confused!");
#endif

		return FALSE;
	}

	item = select_magic_eater(only_browse, FALSE);
	if (item == -1)
	{
		energy_use = 0;
		return FALSE;
	}
	if (item >= EATER_EXT*2) {tval = TV_ROD;sval = item - EATER_EXT*2;}
	else if (item >= EATER_EXT) {tval = TV_WAND;sval = item - EATER_EXT;}
	else {tval = TV_STAFF;sval = item;}
	k_idx = lookup_kind(tval, sval);

	level = (tval == TV_ROD ? k_info[k_idx].level * 5 / 6 - 5 : k_info[k_idx].level);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);
	level /= 2;
	if (p_ptr->lev > level)
	{
		chance -= 3 * (p_ptr->lev - level);
	}
	chance = mod_spell_chance_1(chance);
	chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]]);
	/* Stunning makes spells harder */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	if (chance > 95) chance = 95;

	chance = mod_spell_chance_2(chance);

	if (randint0(100) < chance)
	{
		if (flush_failure) flush();

#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("隠し持った魔道具をうまく使えなかった！");
		else
			msg_print("呪文をうまく唱えられなかった！");
#else
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("You failed to use your magic device!");
		else
			msg_format("You failed to get the magic off!");
#endif

		sound(SOUND_FAIL);
//		if (randint1(100) >= chance)
//			chg_virtue(V_CHANCE,-1);

		energy_use = 100;

		return TRUE;
	}
	else
	{
		int dir = 0;

		if (tval == TV_ROD)
		{
			if ((sval >= SV_ROD_MIN_DIRECTION) && (sval != SV_ROD_HAVOC) && (sval != SV_ROD_AGGRAVATE) && (sval != SV_ROD_PESTICIDE))
				if (!get_aim_dir(&dir)) return FALSE;
			rod_effect(sval, dir, &use_charge, powerful, TRUE);
			if (!use_charge) return FALSE;
		}
		else if (tval == TV_WAND)
		{
			if (!get_aim_dir(&dir)) return FALSE;
			wand_effect(sval, dir, powerful, TRUE);
		}
		else
		{
			staff_effect(sval, &use_charge, powerful, TRUE, TRUE);
			if (!use_charge) return FALSE;
		}
		if (randint1(100) < chance)
			chg_virtue(V_CHANCE,1);
	}
	energy_use = 100;
	if (tval == TV_ROD) p_ptr->magic_num1[item] += k_info[k_idx].pval * EATER_ROD_CHARGE;
	else p_ptr->magic_num1[item] -= EATER_CHARGE;

        return TRUE;
}

