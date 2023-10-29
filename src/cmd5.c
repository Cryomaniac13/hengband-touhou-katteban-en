/* File: cmd5.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell/Prayer commands */

#include "angband.h"
#include "player/player-log.h"

/*:::魔法書使用のための補助コード*/
///realm
///mod140208 魔法変更
cptr spell_category_name(int tval)
{
	switch (tval)
	{

#ifdef JP
	case TV_BOOK_HISSATSU:
		return "必殺技";
	case TV_BOOK_MEDICINE:
		return "レシピ";
	case TV_BOOK_OCCULT:
		if(cp_ptr->realm_aptitude[0] == 1)
			return "オカルト";
		else
			return "呪文";
	default:
		return "呪文";

/*
	case TV_HISSATSU_BOOK:
		return "必殺技";
	case TV_LIFE_BOOK:
		return "祈り";
	case TV_MUSIC_BOOK:
		return "歌";
	default:
		return "呪文";
*/
#else
	case TV_BOOK_HISSATSU:
		return "technique";
	case TV_BOOK_MEDICINE:
		return "recipe";
	case TV_BOOK_OCCULT:
		if(cp_ptr->realm_aptitude[0] == 1)
			return "occultism";
		else
			return "spell";
	default:
		return "spell";
#endif
	}
}

///mod140208 呪文を使うときのルーチン少し変更
//それに伴い、魔法書使用可能判定をitem_tester_tval指定からの特殊処理でなくitem_tester_hookを使うようにした
static bool item_tester_cast_spell(object_type *o_ptr)
{
	if(flag_spell_consume_book)
	{
		if(o_ptr->tval >= MIN_MAGIC && o_ptr->tval <= MAX_MAGIC) return TRUE;
		else if(o_ptr->tval == TV_BOOK_OCCULT) return TRUE;
		else return FALSE;
	}

	//v1.1.15 ヘカーティア例外処理　今の体によって領域変更
	if(p_ptr->pclass == CLASS_HECATIA)
	{
		if(hecatia_body_is_(HECATE_BODY_OTHER) &&
			o_ptr->tval >= TV_BOOK_TRANSFORM && o_ptr->tval <= TV_BOOK_CHAOS) return TRUE;
		if(hecatia_body_is_(HECATE_BODY_EARTH) &&
			o_ptr->tval >= TV_BOOK_ELEMENT && o_ptr->tval <= TV_BOOK_SUMMONS) return TRUE;
		if(hecatia_body_is_(HECATE_BODY_MOON) &&
			o_ptr->tval >= TV_BOOK_MYSTIC && o_ptr->tval <= TV_BOOK_NATURE) return TRUE;
		return FALSE;
	}

	return check_book_realm(o_ptr->tval, o_ptr->sval);
}


/*
 * Allow user to choose a spell/prayer from the given book.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */
/*:::選択した本から魔法を選ぶ*/
/*:::sval:選択した魔法書のsval値*/
/*:::prompt:「学ぶ」などの文字が入ってる*/
/*:::sn:選択された魔法番号 何も選択しなければ-1,不正な選択をしたら-2が入る*/
/*:::learned:既に学習済みならTRUE*/
/*:::use_realm:使用領域を示す REALM_???の定数*/
///realm
///mod140207 魔法変更
bool select_the_force = FALSE;

static int get_spell(int *sn, cptr prompt, int sval, bool learned, int use_realm)
{
	int         i;
	int         spell = -1;
	int         num = 0;
	int         ask = TRUE;
	int         need_mana;
	byte        spells[64];
	bool        flag, redraw, okay;
	char        choice;
	magic_type forge, *s_ptr = &forge;

	char        out_val[160];
	cptr        p;
#ifdef JP
	char jverb_buf[128];
#endif
	int menu_line = (use_menu ? 1 : 0);

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if (spell_okay(*sn, learned, FALSE, use_realm))
		{
			/* Success */
			return (TRUE);
		}
	}

	//v1.1.67 -hack- 選択的記憶除去
	if (flag_spell_forget)
	{
		prompt = _("忘れる", "forget");
	}

#endif /* ALLOW_REPEAT -- TNB */
	/*:::必殺技、祈り、歌、呪文いずれかの文字列を返す*/
	p = spell_category_name(p_ptr->realm1);

	/* Extract spells */
	/*:::spells[0-7]に対し、一冊目なら1-8,二冊目なら9-16,三冊目なら17-24,四冊目なら25-31を格納しているらしい*/
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}

	/* Assume no usable spells */
	okay = FALSE;

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		/*:::選択した魔法書の8つの魔法を一つずつチェックし、＠が学ぶ/使うことができる魔法が一つもなければokayはFALSEのまま*/
		if (spell_okay(spells[i], learned, FALSE, use_realm)) okay = TRUE;
	}
	///class realm
	/* No "okay" spells */
	if (!okay) return (FALSE);
	/*:::今使おうとしている領域が第一領域でも第二領域でもなくスペマスでも赤魔でもなければreturn FALSE(ここに来ることはあるのか？)*/
	//v1.1.32 パチュリー特殊性格追加
	if (((use_realm) != p_ptr->realm1) && ((use_realm) != p_ptr->realm2)
		&& !(REALM_SPELLMASTER && cp_ptr->realm_aptitude[use_realm]!=0)
		&& !(is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && use_realm <= MAX_MAGIC)
		&& !flag_spell_consume_book) return FALSE;
	/*:::ソーサラーか赤魔でも呪術、歌、剣術は使用不可*/
	//if (((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)) && !is_magic(use_realm)) return FALSE;
	/*:::赤魔は秘術以外の三冊目を使用不可*/
	//if ((p_ptr->pclass == CLASS_RED_MAGE) && ((use_realm) != REALM_ARCANE) && (sval > 1)) return FALSE;

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Show choices */
	p_ptr->window |= (PW_SPELL);

	/* Window stuff */
	window_stuff();

	/* Build a prompt (accept all spells) */
#ifdef JP
	/*:::動詞変換処理らしい*/
	jverb1( prompt, jverb_buf );
	(void) strnfmt(out_val, 78, "(%^s:%c-%c, '*'で一覧, ESCで中断) どの%sを%^sますか? ",
		p, I2A(0), I2A(num - 1), p, jverb_buf );
#else
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);
#endif

	/* Get a spell from the user */
	/*:::プロンプトを出して魔法を選択させる処理*/
	choice = (always_show_list || use_menu) ? ESCAPE : 1;
	while (!flag)
	{
		if (choice == ESCAPE) choice = ' ';
		else if (!get_com(out_val, &choice, TRUE))break;

		if (use_menu && choice != ' ')
		{
			switch (choice)
			{
				case '0':
				{
					screen_load();
					return FALSE;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (num - 1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > num) menu_line -= num;
			/* Display a list of spells */
			print_spells(menu_line, spells, num, 1, 15, use_realm);
			if (ask) continue;
		}
		else
		{
			/* Request redraw */
			if ((choice == ' ') || (choice == '*') || (choice == '?'))
			{
				/* Show the list */
				if (!redraw)
				{
					/* Show list */
					redraw = TRUE;

					/* Save the screen */
					screen_save();

					/* Display a list of spells */
					print_spells(menu_line, spells, num, 1, 15, use_realm);
				}

				/* Hide the list */
				else
				{
					if (use_menu) continue;

					/* Hide list */
					redraw = FALSE;

					/* Restore the screen */
					screen_load();
				}

				/* Redo asking */
				continue;
			}


			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = spells[i];

		/* Require "okay" spells */
		if (!spell_okay(spell, learned, FALSE, use_realm))
		{
			bell();
#ifdef JP
			msg_format("その%sを%sことはできません。", p, prompt);
#else
			msg_format("You may not %s that %s.", prompt, p);
#endif

			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Access the spell */
			/*
			if (!is_magic(use_realm))
			{
				s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
			}
			*/
			if(use_realm == TV_BOOK_HISSATSU)
				s_ptr = &hissatsu_info[spell];
			else
			{
				//s_ptr = &mp_ptr->info[use_realm - 1][spell];
				calc_spell_info(s_ptr,use_realm,spell);
			}

			/* Extract mana consumption rate */
			if (use_realm == TV_BOOK_HISSATSU)
			{
				need_mana = s_ptr->smana;
			}
			else
			{
				need_mana = mod_need_mana(s_ptr->smana, spell, use_realm);
			}

			/* Prompt */
#ifdef JP
			jverb1( prompt, jverb_buf );
			/* 英日切り替え機能に対応 */
			(void) strnfmt(tmp_val, 78, "%s(MP%d, 失敗率%d%%)を%sますか? ",
				do_spell(use_realm, spell, SPELL_NAME), need_mana,
				       spell_chance(spell, use_realm),jverb_buf);
#else
			(void)strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
				prompt, do_spell(use_realm, spell, SPELL_NAME), need_mana,
				spell_chance(spell, use_realm));
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}


	/* Restore the screen */
	if (redraw) screen_load();


	/* Show choices */
	p_ptr->window |= (PW_SPELL);

	/* Window stuff */
	window_stuff();


	/* Abort if needed */
	if (!flag) return FALSE;

	/* Save the choice */
	(*sn) = spell;

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return TRUE;
}

/*:::使用・学習可能な魔法書の場合TRUE browse()とstudy()で呼ばれる*/
/*:::check_book_realm()と違い未習得領域でも学習可能ならTRUE*/
///realm class
static bool item_tester_learn_spell(object_type *o_ptr)
{
	//s32b choices = realm_choices2[p_ptr->pclass];

	if(o_ptr->tval > TV_BOOK_END) return (FALSE);

	///mod151230
	if(CHECK_MAKEDRUG_CLASS && o_ptr->tval == TV_BOOK_MEDICINE) return TRUE;

	if(o_ptr->tval == TV_BOOK_HISSATSU)
	{
		if(p_ptr->pclass == CLASS_SAMURAI) return (TRUE);
		else if(p_ptr->pclass == CLASS_YOUMU) return (TRUE);
		else return (FALSE);
	}
	//魔理沙は全ての領域を学習できる
	if(p_ptr->pclass == CLASS_MARISA && o_ptr->tval >= MIN_MAGIC && o_ptr->tval <= MAX_MAGIC) return (TRUE);

	/*:::神官・巫女は第一領域に合わない第二領域を選択できない*/
	if (p_ptr->pclass == CLASS_PRIEST)
	{
		if(same_category_realm(p_ptr->realm1, o_ptr->tval)) return (TRUE);
		else return (FALSE);
		/*
		if (is_good_realm(p_ptr->realm1))
		{
			choices &= ~(CH_DEATH | CH_DAEMON);
		}
		else
		{
			choices &= ~(CH_LIFE | CH_CRUSADE);
		}
		*/
	}

	//if ((o_ptr->tval < TV_LIFE_BOOK) || (o_ptr->tval > (TV_LIFE_BOOK + MAX_REALM - 1))) return (FALSE);
	//if ((o_ptr->tval == TV_MUSIC_BOOK) && (p_ptr->pclass == CLASS_BARD)) return (TRUE);
	//else if (!is_magic(tval2realm(o_ptr->tval))) return FALSE;
	if ((p_ptr->realm1 == o_ptr->tval) || (p_ptr->realm2 == o_ptr->tval)) return (TRUE);
	//if (choices & (0x0001 << (tval2realm(o_ptr->tval) - 1))) return (TRUE);
	/*:::二領域習得可能な職の場合、習得してない領域でも習得可能な領域ならTRUE*/
	if(cp_ptr->realm_aptitude[0] == 3 && cp_ptr->realm_aptitude[o_ptr->tval] != 0) return (TRUE);

	if(REALM_SPELLMASTER && cp_ptr->realm_aptitude[o_ptr->tval] != 0) return (TRUE);

	//v1.1.32 パチュリー専用性格
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && o_ptr->tval <= MAX_MAGIC)
		return TRUE;

	return (FALSE);
}

/*:::ザックと足元に＠が使用可能な魔法書がなければTRUE*/
static bool player_has_no_spellbooks(void)
{
	int         i;
	object_type *o_ptr;

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		if (o_ptr->k_idx && check_book_realm(o_ptr->tval, o_ptr->sval)) return FALSE;
	}

	for (i = cave[py][px].o_idx; i; i = o_ptr->next_o_idx)
	{
		o_ptr = &o_list[i];
		if (o_ptr->k_idx && (o_ptr->marked & OM_FOUND) && check_book_realm(o_ptr->tval, o_ptr->sval)) return FALSE;
	}

	return TRUE;
}


static void confirm_use_force(bool browse_only)
{
	int  item;
	char which;

#ifdef ALLOW_REPEAT

	/* Get the item index */
	if (repeat_pull(&item) && (item == INVEN_FORCE))
	{
		browse_only ? do_cmd_mind_browse() : do_cmd_mind();
		return;
	}

#endif /* ALLOW_REPEAT */

	/* Show the prompt */
#ifdef JP
	prt("('w'練気術, ESC) 'w'かESCを押してください。 ", 0, 0);
#else
	prt("(w for the Force, ESC) Hit 'w' or ESC. ", 0, 0);
#endif

	while (1)
	{
		/* Get a key */
		which = inkey();

		if (which == ESCAPE) break;
		else if (which == 'w')
		{

#ifdef ALLOW_REPEAT

			repeat_push(INVEN_FORCE);

#endif /* ALLOW_REPEAT */

			break;
		}
	}

	/* Clear the prompt line */
	prt("", 0, 0);

	if (which == 'w')
	{
		browse_only ? do_cmd_mind_browse() : do_cmd_mind();
	}
}


/*
 * Peruse the spells/prayers in a book
 *:::（peruse:精読する）
 *
 * Note that *all* spells in the book are listed
 *
 * Note that browsing is allowed while confused or blind,
 * and in the dark, primarily to allow browsing in stores.
 */
/*:::本を読む。錬気術師の気の説明もここから*/
///system bコマンドでレイシャル、クラス技能、魔法を全て参照したい
///class realm
void do_cmd_browse(void)
{
	int		item, sval, use_realm = 0, j, line;
	int		spell = -1;
	int		num = 0;

	byte		spells[64];
	char            temp[62*5];

	object_type	*o_ptr;

	cptr q, s;

	/* Warriors are illiterate */
	///class realm
	//if (!(p_ptr->realm1 || p_ptr->realm2) && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE))
	if (!(p_ptr->realm1 || p_ptr->realm2 || REALM_SPELLMASTER || CHECK_MAKEDRUG_CLASS))
	{
#ifdef JP
		msg_print("本を読むことができない！");
#else
		msg_print("You cannot read spellbooks.");
#endif

		return;
	}
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_CAST | MIMIC_GIGANTIC))
	{
		msg_print(_("今は本を読めない。", "You cannot read books in your current form."));
		return ;
	}
	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}
	/*
	if (p_ptr->pclass == CLASS_FORCETRAINER)
	{
		if (player_has_no_spellbooks())
		{
			confirm_use_force(TRUE);
			return;
		}
		select_the_force = TRUE;
	}
	*/

	/* Restrict choices to "useful" books */
	//if (p_ptr->realm2 == REALM_NONE) item_tester_tval = mp_ptr->spell_book;
	//else item_tester_hook = item_tester_learn_spell;
	///mod140207 魔法書選択ルーチン色々書き換えた
	item_tester_hook = item_tester_learn_spell;

	/* Get an item */
#ifdef JP
	q = "どの本を読みますか? ";
#else
	q = "Browse which book? ";
#endif

#ifdef JP
	s = "読める本がない。";
#else
	s = "You have no books that you can read.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		select_the_force = FALSE;
		return;
	}
	select_the_force = FALSE;

	/*:::アイテム選択のときwを押したときのためのダミーアイテム　錬気術師専用*/
	///class
#if 0
	if (item == INVEN_FORCE) /* the_force */
	{
		do_cmd_mind_browse();
		return;
	}

	/* Get the item (in the pack) */
	else
#endif

		if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	///mod151230 薬師レシピ確認
	if(o_ptr->tval == TV_BOOK_MEDICINE)
	{
		check_drug_recipe_aux(o_ptr);

		return;
	}

	/* Access the item's sval */
	sval = o_ptr->sval;

	//use_realm = tval2realm(o_ptr->tval);
	use_realm = o_ptr->tval;

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/*:::spells[0-7]に対し、一冊目なら1-8,二冊目なら9-16,三冊目なら17-24,四冊目なら25-31を格納しているらしい*/
	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Save the screen */
	screen_save();

	/* Clear the top line */
	prt("", 0, 0);

	/* Keep browsing spells.  Exit browsing on cancel. */
	while(TRUE)
	{
		/*:::選択した本の中から呪文を一つ選ばせ、それが今の＠に使えるのかチェックする*/
		/* Ask for a spell, allow cancel */
#ifdef JP
		if (!get_spell(&spell, "読む", o_ptr->sval, TRUE, use_realm))
#else
		if (!get_spell(&spell, "browse", o_ptr->sval, TRUE, use_realm))
#endif
		{
			/* If cancelled, leave immediately. */
			if (spell == -1) break;

			/* Display a list of spells */
			print_spells(0, spells, num, 1, 15, use_realm);

			/* Notify that there's nothing to see, and wait. */
			//if (use_realm == REALM_HISSATSU)
			if (use_realm == TV_BOOK_HISSATSU)
#ifdef JP
				prt("読める技がない。", 0, 0);
#else
				prt("No techniques to browse.", 0, 0);
#endif
			else
#ifdef JP
				prt("読める呪文がない。", 0, 0);
#else
				prt("No spells to browse.", 0, 0);
#endif
			(void)inkey();


			/* Restore the screen */
			screen_load();

			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(14, 15, 255);
		Term_erase(14, 14, 255);
		Term_erase(14, 13, 255);
		Term_erase(14, 12, 255);
		Term_erase(14, 11, 255);

		roff_to_buf(do_spell(use_realm, spell, SPELL_DESC), 62, temp, sizeof(temp));

		for (j = 0, line = 11; temp[j]; j += 1 + strlen(&temp[j]))
		{
			prt(&temp[j], line, 15);
			line++;
		}
	}

	/* Restore the screen */
	screen_load();
}

/*:::第二領域を変更する*/
static void change_realm2(int next_realm)
{
	int i, j = 0;
	char tmp[80];

	/*:::古い第二領域のスペル番号をspell_order（魔法習得順記録配列）から削除し空いた部分を詰めて末尾を99で埋めている。*/
	for (i = 0; i < 64; i++)
	{
		p_ptr->spell_order[j] = p_ptr->spell_order[i];
		if (p_ptr->spell_order[i] < 32) j++;
	}
	for (; j < 64; j++)
		p_ptr->spell_order[j] = 99;
	/*:::第二領域の経験値と各フラグをリセットしている*/
	for (i = 32; i < 64; i++)
	{
		p_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}
	p_ptr->spell_learned2 = 0L;
	p_ptr->spell_worked2 = 0L;
	p_ptr->spell_forgotten2 = 0L;

#ifdef JP
	sprintf(tmp,"魔法の領域を%sから%sに変更した。", realm_names[p_ptr->realm2], realm_names[next_realm]);
#else
	sprintf(tmp,"change magic realm from %s to %s.", realm_names[p_ptr->realm2], realm_names[next_realm]);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
	/*:::過去使えた領域のビット毎フラグ　「あなたはかつて～領域を使えた」のダンプのためか*/
	p_ptr->old_realm |= 1 << (p_ptr->realm2-1);
	/*:::領域変更*/
	p_ptr->realm2 = next_realm;
	/*:::更新処理関係*/
	p_ptr->notice |= (PN_REORDER);
	p_ptr->update |= (PU_SPELLS);
	handle_stuff();

	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}

/*:::第一領域を変更する。魔理沙専用*/
static void change_realm1(int next_realm)
{
	int i, j = 0;
	char tmp[80];

	if(p_ptr->pclass != CLASS_MARISA)
	{
		msg_print(_("ERROR:魔理沙以外でchange_realm1()が呼ばれた",
                    "ERROR: change_realm1() called for non-Marisa"));
		return;
	}

	/*:::第一領域の経験値と各フラグをリセットしている*/
	for(i=0;i<32;i++)
	{
		p_ptr->spell_order[i] = 99;
		p_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;

	}
	p_ptr->spell_learned1 = 0L;
	p_ptr->spell_worked1 = 0L;
	p_ptr->spell_forgotten1 = 0L;
	sprintf(tmp,_("魔法の領域を%sから%sに変更した。",
                    "You change your realm from %s to %s."), realm_names[p_ptr->realm1], realm_names[next_realm]);

	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
	/*:::過去使えた領域のビット毎フラグ　「あなたはかつて～領域を使えた」のダンプのためか*/
	p_ptr->old_realm |= 1 << (p_ptr->realm1-1);
	/*:::領域変更*/
	p_ptr->realm1 = next_realm;
	/*:::更新処理関係*/
	p_ptr->notice |= (PN_REORDER);
	p_ptr->update |= (PU_SPELLS);
	handle_stuff();

	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
 * Study a book to gain a new spell/prayer
 */
/*:::本から魔法や歌を習得する（剣術は別処理）*/
///class realm
void do_cmd_study(void)
{
	int	i, item, sval;
	int	increment = 0;
	bool    learned = FALSE;

	/* Spells of realm2 will have an increment of +32 */
	int	spell = -1;
	/*:::「祈り」「呪文」などの行動名を取得*/
	cptr p = spell_category_name(p_ptr->realm1);

	object_type *o_ptr;

	cptr q, s;

	if (!p_ptr->realm1)
	{
#ifdef JP
msg_print("あなたは魔法書を読むことができない。");
#else
		msg_print("You cannot read spellbooks.");
#endif

		return;
	}
	if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
	{
		msg_print(_("呪文を習得できるのは長姉だけだ。", "Only your older sister can learn spells."));
		return;
	}

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_CAST))
	{
		msg_print(_("今は本を読めない。", "You cannot read books in your current form."));
		return ;
	}
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
msg_print("目が見えない！");
#else
		msg_print("You cannot see!");
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

	if (!(p_ptr->new_spells))
	{
#ifdef JP
msg_format("新しい%sを覚えることはできない！", p);
#else
		msg_format("You cannot learn any new %ss!", p);
#endif

		return;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef JP
	if( p_ptr->new_spells < 10 ){
		msg_format("あと %d つの%sを学べる。", p_ptr->new_spells, p);
	}else{
		msg_format("あと %d 個の%sを学べる。", p_ptr->new_spells, p);
	}
#else
	msg_format("You can learn %d new %s%s.", p_ptr->new_spells, p,
		(p_ptr->new_spells == 1?"":"s"));
#endif

	msg_print(NULL);


	/* Restrict choices to "useful" books */
	//if (p_ptr->realm2 == REALM_NONE) item_tester_tval = mp_ptr->spell_book;
	//else item_tester_hook = item_tester_learn_spell;
	///mod140207 魔法書選択関連の処理を色々変更
	item_tester_hook = item_tester_learn_spell;

	/* Get an item */
#ifdef JP
q = "どの本から学びますか? ";
#else
	q = "Study which book? ";
#endif

#ifdef JP
s = "読める本がない。";
#else
	s = "You have no books that you can read.";
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

	/* Access the item's sval */
	sval = o_ptr->sval;
	/*:::第一領域でも第二領域でもない場合領域変更処理*/
	if (o_ptr->tval == p_ptr->realm2) increment = 32;
	else if (o_ptr->tval != p_ptr->realm1)
	{
		//魔理沙は第一領域を変更できる
		if(p_ptr->pclass == CLASS_MARISA)
		{
			char c;

			if (!get_check(_("その領域の魔法は習得していない。領域を変更しますか？",
                            "You're not studying magic of this realm. Change your realm?"))) return;

			screen_save();
			while(1)
			{
				for(i=0;i<5;i++) Term_erase(17, 10+i, 255);
				prt(_("どちらの領域を忘却しますか？(ESC:キャンセル)",
                        "Which realm do you want to forget? (ESC: Cancel)"),11,18);
				prt(format("a) %s ",realm_names[p_ptr->realm1]),12,20);
				prt(format("b) %s ",realm_names[p_ptr->realm2]),13,20);
				c = inkey();

				if(c == ESCAPE)
				{
					screen_load();
					return;
				}

				if(c == 'a' || c == 'b') break;
			}
			screen_load();


			if(c == 'a')
			{
				if (!same_category_realm(p_ptr->realm2,o_ptr->tval))
					if (!get_check(_("第ニ領域と相性の悪い領域です。よろしいですか？",
                                    "This realm isn't compatible with your second one. Are you sure?"))) return;
				change_realm1(o_ptr->tval);
			}
			if(c == 'b')
			{
				if (!same_category_realm(p_ptr->realm1,o_ptr->tval))
					if (!get_check(_("第一領域と相性の悪い領域です。よろしいですか？",
                                    "This realm isn't compatible with your first one. Are you sure?"))) return;
				change_realm2(o_ptr->tval);
				increment = 32;
			}


		}
		else
		{
			if(rp_ptr->realm_aptitude[o_ptr->tval] == 0)
			{
				msg_format(_("あなたにはその領域を修得することはできそうにない。",
                            "You can't study this realm."));
			}

			if (!get_check(_("本当に魔法の領域を変更しますか？",
                            "Are you certain you want to change your magic realm?"))) return;
			if (!same_category_realm(p_ptr->realm1,o_ptr->tval))
				if (!get_check(_("第一領域と相性の悪い領域です。よろしいですか？",
                                "This realm isn't compatible with your first one. Are you sure?"))) return;

			change_realm2(o_ptr->tval);
			increment = 32;
		}
	}

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();
	///system realm もう全職業自由に習得魔法を選べるようにしよう
	/*:::メイジ系はget_spell()で魔法を選択、プリースト系は選択した本から習得可能な魔法が無作為に選ばれる*/
	/* Mage -- Learn a selected spell */
//	if (mp_ptr->spell_book != TV_LIFE_BOOK)
	{
		/* Ask for a spell, allow cancel */
#ifdef JP
//		if (!get_spell(&spell, "学ぶ", sval, FALSE, o_ptr->tval - TV_LIFE_BOOK + 1)
		if (!get_spell(&spell, "学ぶ", sval, FALSE, o_ptr->tval)
			&& (spell == -1)) return;
#else
		if (!get_spell(&spell, "study", sval, FALSE, o_ptr->tval)
			&& (spell == -1)) return;
#endif

	}
#if 0
	/* Priest -- Learn a random prayer */
	else
	{
		int k = 0;

		int gift = -1;

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
		{
			/* Check spells in the book */
			if ((fake_spell_flags[sval] & (1L << spell)))
			{
				/* Skip non "okay" prayers */
				if (!spell_okay(spell, FALSE, TRUE,
					(increment ? p_ptr->realm2 : p_ptr->realm1))) continue;

				/* Hack -- Prepare the randomizer */
				k++;

				/* Hack -- Apply the randomizer */
				if (one_in_(k)) gift = spell;
			}
		}

		/* Accept gift */
		spell = gift;
	}
#endif

	/* Nothing to study */
	if (spell < 0)
	{
		/* Message */
#ifdef JP
msg_format("その本には学ぶべき%sがない。", p);
#else
		msg_format("You cannot learn any %ss in that book.", p);
#endif


		/* Abort */
		return;
	}


	if (increment) spell += increment;

	/* Learn the spell */
	if (spell < 32)
	{
		if (p_ptr->spell_learned1 & (1L << spell)) learned = TRUE;
		else p_ptr->spell_learned1 |= (1L << spell);
	}
	else
	{
		if (p_ptr->spell_learned2 & (1L << (spell - 32))) learned = TRUE;
		else p_ptr->spell_learned2 |= (1L << (spell - 32));
	}

	if (learned)
	{
		int max_exp = (spell < 32) ? SPELL_EXP_MASTER : SPELL_EXP_EXPERT;
		int old_exp = p_ptr->spell_exp[spell];
		int new_rank = EXP_LEVEL_UNSKILLED;
		cptr name = do_spell(increment ? p_ptr->realm2 : p_ptr->realm1, spell%32, SPELL_NAME);

		if (old_exp >= max_exp)
		{
#ifdef JP
			msg_format("その%sは完全に使いこなせるので学ぶ必要はない。", p);
#else
			msg_format("You don't need to study this %s anymore.", p);
#endif
			return;
		}
#ifdef JP
		if (!get_check(format("%sの%sをさらに学びます。よろしいですか？", name, p)))
#else
		if (!get_check(format("You will study a %s of %s again. Are you sure? ", p, name)))
#endif
		{
			return;
		}
		else if (old_exp >= SPELL_EXP_EXPERT)
		{
			p_ptr->spell_exp[spell] = SPELL_EXP_MASTER;
			new_rank = EXP_LEVEL_MASTER;
		}
		else if (old_exp >= SPELL_EXP_SKILLED)
		{
			if (spell >= 32) p_ptr->spell_exp[spell] = SPELL_EXP_EXPERT;
			else p_ptr->spell_exp[spell] += SPELL_EXP_EXPERT - SPELL_EXP_SKILLED;
			new_rank = EXP_LEVEL_EXPERT;
		}
		else if (old_exp >= SPELL_EXP_BEGINNER)
		{
			p_ptr->spell_exp[spell] = SPELL_EXP_SKILLED + (old_exp - SPELL_EXP_BEGINNER) * 2 / 3;
			new_rank = EXP_LEVEL_SKILLED;
		}
		else
		{
			p_ptr->spell_exp[spell] = SPELL_EXP_BEGINNER + old_exp / 3;
			new_rank = EXP_LEVEL_BEGINNER;
		}
#ifdef JP
		msg_format("%sの熟練度が%sに上がった。", name, exp_level_str[new_rank]);
#else
		msg_format("Your proficiency of %s is now %s rank.", name, exp_level_str[new_rank]);
#endif
	}
	else
	{
		/* Find the next open entry in "p_ptr->spell_order[]" */
		for (i = 0; i < 64; i++)
		{
			/* Stop at the first empty space */
			if (p_ptr->spell_order[i] == 99) break;
		}

		/* Add the spell to the known list */
		p_ptr->spell_order[i++] = spell;

		/* Mention the result */
#ifdef JP
		/* 英日切り替え機能に対応 */
		/*
		if (mp_ptr->spell_book == TV_MUSIC_BOOK)
		{
			msg_format("%sを学んだ。",
				    do_spell(increment ? p_ptr->realm2 : p_ptr->realm1, spell % 32, SPELL_NAME));
		}
		else
		*/
		msg_format("%sの%sを学んだ。",do_spell(increment ? p_ptr->realm2 : p_ptr->realm1, spell % 32, SPELL_NAME) ,p);
		//二冊目以降、野良神様の徳性変更
		if(p_ptr->prace == RACE_STRAYGOD && spell % 32 > 7)
		{
			int point = (spell % 32) / 8;
			switch(increment ? p_ptr->realm2 : p_ptr->realm1)
			{
			case TV_BOOK_FORESEE:
			case TV_BOOK_SUMMONS:
				point = -point;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_MYSTIC:
			case TV_BOOK_NATURE:
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			case TV_BOOK_ELEMENT:
			case TV_BOOK_ENCHANT:
				set_deity_bias(DBIAS_WARLIKE, point);
				break;

			case TV_BOOK_LIFE:
				point = -1 - point;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_PUNISH:
				point++;
				set_deity_bias(DBIAS_COSMOS, point);
				break;
			case TV_BOOK_DARKNESS:
				point++;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_NECROMANCY:
			case TV_BOOK_TRANSFORM:
				point = -point;
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			case TV_BOOK_CHAOS:
			case TV_BOOK_OCCULT:
				point = -1 - point;
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			}
		}
#else
		msg_format("You have learned the %s of %s.",
			p, do_spell(increment ? p_ptr->realm2 : p_ptr->realm1, spell % 32, SPELL_NAME));

        //二冊目以降、野良神様の徳性変更
		if(p_ptr->prace == RACE_STRAYGOD && spell % 32 > 7)
		{
			int point = (spell % 32) / 8;
			switch(increment ? p_ptr->realm2 : p_ptr->realm1)
			{
			case TV_BOOK_FORESEE:
			case TV_BOOK_SUMMONS:
				point = -point;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_MYSTIC:
			case TV_BOOK_NATURE:
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			case TV_BOOK_ELEMENT:
			case TV_BOOK_ENCHANT:
				set_deity_bias(DBIAS_WARLIKE, point);
				break;

			case TV_BOOK_LIFE:
				point = -1 - point;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_PUNISH:
				point++;
				set_deity_bias(DBIAS_COSMOS, point);
				break;
			case TV_BOOK_DARKNESS:
				point++;
				set_deity_bias(DBIAS_WARLIKE, point);
				break;
			case TV_BOOK_NECROMANCY:
			case TV_BOOK_TRANSFORM:
				point = -point;
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			case TV_BOOK_CHAOS:
			case TV_BOOK_OCCULT:
				point = -1 - point;
				set_deity_bias(DBIAS_COSMOS, point);
				break;

			}
		}
#endif
	}

	/* Take a turn */
	energy_use = 100;
	///virtue
	/*
	switch (mp_ptr->spell_book)
	{
	case TV_LIFE_BOOK:
		chg_virtue(V_FAITH, 1);
		break;
	case TV_DEATH_BOOK:
		chg_virtue(V_UNLIFE, 1);
		break;
	case TV_NATURE_BOOK:
		chg_virtue(V_NATURE, 1);
		break;
	default:
		chg_virtue(V_KNOWLEDGE, 1);
		break;
	}
	*/

	/* Sound */
	sound(SOUND_STUDY);

	/* One less spell available */
	p_ptr->learned_spells++;
#if 0
	/*:::Q020　習得可能魔法数らしいが、どのタイミングでこの変数は更新されるのか？*/
	/* Message if needed */
	if (p_ptr->new_spells)
	{
		/* Message */
#ifdef JP
		if (p_ptr->new_spells < 10) msg_format("あと %d つの%sを学べる。", p_ptr->new_spells, p);
		else msg_format("あと %d 個の%sを学べる。", p_ptr->new_spells, p);
#else
		msg_format("You can learn %d more %s%s.", p_ptr->new_spells, p,
		           (p_ptr->new_spells != 1) ? "s" : "");
#endif
	}
#endif

	/* Update Study */
	p_ptr->update |= (PU_SPELLS);
	update_stuff();

	/* Redraw object recall */
	p_ptr->window |= (PW_OBJECT);
}


/*:::性格「狂気の」で魔法に失敗したときのペナルティ*/
void berserk_spell_fail_effect(int spell)
{

	int power = randint1(spell * 2 + 15); //1-77


	if(power < 10)
	{
		msg_print(_("暴走した魔力が体を傷つけた。", "You are wounded by wild magic."));
		set_cut(p_ptr->cut + 10 + randint1(10));
	}

	else if(power < 15)
	{
		msg_print(_("激しく頭が痛んだ。", "You have a severe headache."));
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
	}

	else if(power < 30)
	{
		msg_print(_("暴走した魔力が体を切り裂いた！", "You are severely wounded by wild magic!"));
		set_cut(p_ptr->cut + 100 + randint1(100));
	}

	else if(power < 33)
	{
		if(!p_ptr->resist_chaos) (void)set_image(50 + randint1(50));
	}
	else if(power < 35)
	{
		if (lose_all_info()) msg_print(_("頭の中が真っ白になった。", "Your mind becomes blank."));
	}

	else if(power < 40)
	{
		msg_print(_("狂気のエネルギーが周囲を侵食した！",
                    "Maddening energy permeates the area!"));
		project_hack2(GF_REDEYE,1,p_ptr->lev * 5,p_ptr->lev * 5);
	}

	else if(power < 45)
	{
		u32b mode;
		bool flag = FALSE;

		if(one_in_(2))	mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_FORCE_PET);
		else		mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET);

		if(summon_specific(0, py, px, dun_level, SUMMON_INSANITY, mode)) flag = TRUE;

		if(flag) msg_print(_("あなたの狂気が悍ましいものを呼び寄せた。",
                            "Your madness has called forth something horrible."));
	}
	else if(power < 50)
	{
		if (earthquake(py, px, 8+randint1(8)))
			msg_print(_("ダンジョンの天井が崩落した！", "The dungeon ceiling collapses!"));
		else
			msg_print(_("地面が揺れた。", "The ground shakes."));
	}

	else if(power < 55)
	{
		msg_print(_("暴走した魔力が周囲を蹂躙した！", "Wild magic bursts forth!"));
		call_chaos();
	}
	else if(power < 60)
	{
		msg_print(_("暴走した魔力が体を引き裂いた！", "Wild magic tears your body apart!"));
		set_cut(p_ptr->cut + 1000 + randint1(1000));
	}

	else if(power < 65)
	{
		msg_print(_("暴走した次元の扉があなたを呑み込んだ！", "You are swallowed by a rogue dimensional door!"));
		teleport_player(50+randint1(250),TELEPORT_ANYPLACE);
		if(cave_have_flag_bold(py,px,FF_WALL))
			msg_print(_("いしのなかにいる。", "You are in a wall."));

	}


	else if(power < 72)
	{
		int rad = p_ptr->csp / 50;
		int dam = p_ptr->csp * 2;

		if(rad < 2) rad=2;
		if(rad > 8) rad=8;
		if(dam < 100) dam = 100;
		p_ptr->csp = 0;
		p_ptr->redraw |= (PR_MANA);
		msg_print(_("暴走した魔力が大爆発を起こした！", "There is an explosion of wild magic!"));
		project(0, rad, py, px, dam*3, GF_MANA,(PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		take_hit(DAMAGE_LOSELIFE, dam, _("魔法の暴発", "magical explosion"), -1);
	}

	else
	{
		int num = randint1(3);
		u32b mode;
		bool flag = FALSE;

		if(one_in_(3))	mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_FORCE_FRIENDLY);
		else		mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET);

		for (; num>0; num--)
		{
			if(summon_specific(0, py, px, dun_level+50, SUMMON_INSANITY2, mode)) flag = TRUE;
		}
		if(flag) msg_print(_("あなたの狂気は名状しがたき異世界への扉を開いてしまった！",
                            "Your madness has opened a gate to a nameless world!"));
	}

}


/*:::カオス領域失敗ペナルティ、トランプ領域シャッフル効果など・・というかこの関数cmd5.cとdo-spell.cの両方にある？*/
static void wild_magic(int spell)
{
	int counter = 0;
	int type = SUMMON_BIZARRE1 + randint0(6);

	if (type < SUMMON_BIZARRE1) type = SUMMON_BIZARRE1;
	else if (type > SUMMON_BIZARRE6) type = SUMMON_BIZARRE6;

	switch (randint1(spell) + randint1(8) + 1)
	{
	case 1:
	case 2:
	case 3:
		teleport_player(10, TELEPORT_PASSIVE);
		break;
	case 4:
	case 5:
	case 6:
		teleport_player(100, TELEPORT_PASSIVE);
		break;
	case 7:
	case 8:
		teleport_player(200, TELEPORT_PASSIVE);
		break;
	case 9:
	case 10:
	case 11:
		unlite_area(10, 3);
		break;
	case 12:
	case 13:
	case 14:
		lite_area(damroll(2, 3), 2);
		break;
	case 15:
		destroy_doors_touch();
		break;
	case 16: case 17:
		wall_breaker();
	case 18:
		sleep_monsters_touch();
		break;
	case 19:
	case 20:
		trap_creation(py, px);
		break;
	case 21:
	case 22:
		door_creation(1);
		break;
	case 23:
	case 24:
	case 25:
		aggravate_monsters(0,TRUE);
		break;
	case 26:
		earthquake(py, px, 5);
		break;
	case 27:
	case 28:
		(void)gain_random_mutation(0);
		break;
	case 29:
	case 30:
		apply_disenchant(1,0);
		break;
	case 31:
		lose_all_info();
		break;
	case 32:
		fire_ball(GF_CHAOS, 0, spell + 5, 1 + (spell / 10));
		break;
	case 33:
		wall_stone();
		break;
	case 34:
	case 35:
		while (counter++ < 8)
		{
			(void)summon_specific(0, py, px, (dun_level * 3) / 2, type, (PM_ALLOW_GROUP | PM_NO_PET));
		}
		break;
	case 36:
	case 37:
		activate_hi_summon(py, px, FALSE);
		break;
	case 38:
		(void)summon_cyber(-1, py, px);
		break;
	default:
		{
			int count = 0;
			(void)activate_ty_curse(FALSE, &count);
			break;
		}
	}

	return;
}


/*
 * Cast a spell
 */
/*:::魔法書から習得済みの呪文を唱える*/
void do_cmd_cast(void)
{
	int	item, sval, spell, realm;
	int	chance;
	int	increment = 0;
	int	use_realm;
	int	need_mana;

	cptr prayer;

	object_type	*o_ptr;

	//v1.1.36 魔法書リピートバグ対応
	static int previous_book_k_idx = 0;

	//const magic_type *s_ptr;
	magic_type forge, *s_ptr = &forge;


	cptr q, s;

	int over_exerted = 0; //v1.1.84 MP不足呪文強行フラグだったが不足分を記録するint値に変更

	/*:::スペマスでも赤魔でもなくp_ptr->realm1が0の職はreturn
     *:::盲目、混乱はreturn
     *:::呪術領域で詠唱数が上限(hex_spell_fully)の場合return
	 */
	/* Require spell ability */
	///class realm 魔法使用可否判定
	//if (!p_ptr->realm1 && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE))

	if(!flag_spell_consume_book)
	{
		if(p_ptr->pclass == CLASS_LYRICA || p_ptr->pclass == CLASS_MERLIN)
		{
			msg_print(_("呪文を使えるのは長姉だけだ。", "Only your older sister can use magic."));
			return;
		}

		if (!p_ptr->realm1 && !(REALM_SPELLMASTER))
		{
			msg_print(_("呪文を唱えられない！", "You cannot cast spells!"));
			return;
		}
		if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0]))
		{
			msg_print(_("今は声を出せない。", "You can't speak right now."));
			return ;
		}
	}

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_CAST)
	{
		msg_print(_("この姿では本を読めない。", "You cannot read books in your current form."));
		return ;
	}

	/* Require lite */
	///class 盲目でmコマンドを入力したとき練気術士の使用魔法を気に限定する処理
	if (p_ptr->blind || no_lite())
	{
		if (p_ptr->pclass == CLASS_FORCETRAINER) confirm_use_force(FALSE);
		else
		{
#ifdef JP
			msg_print("目が見えない！");
#else
			msg_print("You cannot see!");
#endif
			flush();
		}
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて唱えられない！");
#else
		msg_print("You are too confused!");
#endif
		flush();
		return;
	}

	///sysdel hex
	/* Hex */
	/*
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spell_fully())
		{
			bool flag = FALSE;
#ifdef JP
			msg_print("これ以上新しい呪文を詠唱することはできない。");
#else
			msg_print("Can not spell new spells more.");
#endif
			flush();
			if (p_ptr->lev >= 35) flag = stop_hex_spell();
			if (!flag) return;
		}
	}
	*/
	///class 練気
	/*
	if (p_ptr->pclass == CLASS_FORCETRAINER)
	{
		if (player_has_no_spellbooks())
		{
			confirm_use_force(FALSE);
			return;
		}
		select_the_force = TRUE;
	}
	*/
	prayer = spell_category_name(p_ptr->realm1);

	/* Restrict choices to spell books */
	//item_tester_tval = mp_ptr->spell_book;

	///mod140207 魔法変更に伴い、item_tester_tvalからの特殊処理でなく普通にhook使うようにした
	item_tester_hook = item_tester_cast_spell;

	/* Get an item */
#ifdef JP
	q = "どの呪文書を使いますか? ";
#else
	q = "Use which book? ";
#endif

#ifdef JP
	s = "呪文書がない！";
#else
	s = "You have no spell books!";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		select_the_force = FALSE;
		return;
	}
	select_the_force = FALSE;

#if 0
	/*:::選択した呪文書が錬気術用のダミーアイテムのとき*/
	if (item == INVEN_FORCE) /* the_force */
	{
		do_cmd_mind();
		return;
	}

	/* Get the item (in the pack) */
	else
#endif
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	//v1.1.36 魔法書リピートバグ対応 'n'でリピート中に魔法書のk_idxが前回と違っていたらもう一度探す。
	//弥縫策極まるが他にスマートなやり方を思いつかない。
	//魔法書がなくなっている場合再選択から始めるべきかもしれないがまあ中断でも問題ないだろう。
	if (command_repeat_flag && previous_book_k_idx != o_ptr->k_idx)
	{
		int item2;
		if (item >= 0)
		{
			for (item2 = 0; item2<INVEN_PACK; item2++)
			{
				if (inventory[item2].k_idx == previous_book_k_idx) break;
			}
			if (item2 == INVEN_PACK)
			{
				msg_print(_("さっきまで読んでいた本がない。", "You don't have the book you were reading."));
				return;
			}
			o_ptr = &inventory[item2];
		}
		else//床上のアイテムはget_item_floor内の処理で対応されてるらしいが一応書いとく
		{
			s16b next_o_idx = 0;

			for (item2 = cave[py][px].o_idx; item2; item2 = next_o_idx)
			{
				if (o_list[item2].k_idx == previous_book_k_idx)break;
				next_o_idx = o_list[item2].next_o_idx;
			}
			if (!item2)
			{
				msg_print(_("さっきまで読んでいた本がない。", "You don't have the book you were reading."));
				return;
			}
			o_ptr = &o_list[0 - item2];
		}
	}
	previous_book_k_idx = o_ptr->k_idx;


	/* Access the item's sval */
	sval = o_ptr->sval;

	///class realm スペマスと赤魔
	//if ((p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE) && (o_ptr->tval == REALM2_BOOK)) increment = 32;
	if (!(REALM_SPELLMASTER) && (o_ptr->tval == p_ptr->realm2)) increment = 32;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	///class realm スペマスと赤魔
	//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
	//	realm = o_ptr->tval - TV_LIFE_BOOK + 1;
	if (REALM_SPELLMASTER)	realm = o_ptr->tval;
	else if(flag_spell_consume_book) realm = o_ptr->tval;
	else if (increment) realm = p_ptr->realm2;
	else realm = p_ptr->realm1;

	/* Ask for a spell */
#ifdef JP
//	if (!get_spell(&spell,
//				((mp_ptr->spell_book == TV_LIFE_BOOK) ? "詠唱する" : (mp_ptr->spell_book == TV_MUSIC_BOOK) ? "歌う" : "唱える"),
//		       sval, TRUE, realm))
	if (!get_spell(&spell,"唱える", sval, TRUE, realm))
	{
		if (spell == -2) msg_format("その本には知っている%sがない。", prayer);
		return;
	}
#else
	if (!get_spell(&spell,"cast", sval, TRUE, realm))
	{
		if (spell == -2)
			msg_format("You don't know any %ss in that book.", prayer);
		return;
	}
#endif


	//use_realm = tval2realm(o_ptr->tval);
	use_realm = o_ptr->tval;

	/* Hex */
#if 0
	if (use_realm == REALM_HEX)
	{
		if (hex_spelling(spell))
		{
#ifdef JP
			msg_print("その呪文はすでに詠唱中だ。");
#else
			msg_print("You are already casting it.");
#endif
			return;
		}
	}
#endif
	/*:::選択した呪文の情報を得る*/

	//if (!is_magic(use_realm))
	//{
	//	s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	//}
	if(use_realm == TV_BOOK_HISSATSU)
		s_ptr = &hissatsu_info[spell];
	else
	{
		//s_ptr = &mp_ptr->info[realm - 1][spell];
		calc_spell_info(s_ptr,realm,spell);
	}

	/* Extract mana consumption rate */
	need_mana = mod_need_mana(s_ptr->smana, spell, realm);


	//v1.1.67 魔法を忘れる特殊処理
	if (flag_spell_forget)
	{
		cptr spell_name;
		int add_spell_amt = 1;


		if (REALM_SPELLMASTER)
		{
			msg_print(_("ERROR:スペマス系の職でflag_spell_forgetがTRUEになった",
                        "ERROR: flag_spell_forget was TRUE for a spellmaster"));
			flag_spell_forget = FALSE;
			return;
		}

		spell_name = do_spell(use_realm, spell, SPELL_NAME);
		if (!get_check_strict(format(_("「%s」の呪文を忘れてもよろしいですか？ ",
                                        " Do you want to forget the spell of %s?"),spell_name), CHECK_OKAY_CANCEL)) return;

		if (realm == p_ptr->realm1)
		{
			//習得済み魔法を忘れる処理
			p_ptr->spell_learned1 &= ~(1L << spell);
			//習得しているがINTやレベルの低下で忘却した魔法を完全に忘れる処理
			p_ptr->spell_forgotten1 &= ~(1L << spell);
			//魔法を「使ったことがある」フラグの消去..はしないでおく。消去するとマクロで無限経験値取得ができてしまうため。
			//p_ptr->spell_worked1 &= ~(1L << spell);
			//熟練度が高かった場合学習数がさらに1増えることにする
			if (p_ptr->spell_exp[(increment ? 32 : 0) + spell] >= SPELL_EXP_EXPERT) add_spell_amt++;
			//呪文熟練度のリセット
			p_ptr->spell_exp[(increment ? 32 : 0) + spell] = 0;
		}
		else
		{
			p_ptr->spell_learned2 &= ~(1L << spell);
			p_ptr->spell_forgotten2 &= ~(1L << spell);
			if (p_ptr->spell_exp[(increment ? 32 : 0) + spell] >= SPELL_EXP_EXPERT) add_spell_amt++;
			p_ptr->spell_exp[(increment ? 32 : 0) + spell] = 0;

		}
		msg_format(_("%sの呪文を忘却した。", "You forgot the spell of %s."),spell_name);
		//学習数増加 一応上限つけとく
		if(p_ptr->add_spells < 9999) p_ptr->add_spells += add_spell_amt;
		p_ptr->update |= (PU_SPELLS);

		return;
	}



	/* Verify "dangerous" spells */
	if (need_mana > p_ptr->csp)
	{
		if (flush_failure) flush();

		/* Warning */
#ifdef JP
//msg_format("その%sを%sのに十分なマジックポイントがない。",prayer,
// ((mp_ptr->spell_book == TV_LIFE_BOOK) ? "詠唱する" : (mp_ptr->spell_book == TV_LIFE_BOOK) ? "歌う" : "唱える"));
msg_format("その%sを%sのに十分なマジックポイントがない。",prayer,
 "唱える");
#else
		msg_format("You do not have enough mana to cast this %s.",
			prayer);
#endif


		if (!over_exert) return;

		/* Verify */
#ifdef JP
		if (!get_check_strict("それでも挑戦しますか? ", CHECK_OKAY_CANCEL)) return;
#else
		if (!get_check_strict("Attempt it anyway? ", CHECK_OKAY_CANCEL)) return;
#endif

	}

	/* Spell failure chance */
	/*:::失敗率計算*/
	chance = spell_chance(spell, use_realm);
	///mod140211 魔法をキャンセルした時もMPが消費されてるので、MP減少部を最後に持っていった
#if 0
	/* Sufficient mana */
	if (need_mana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= need_mana;
	}
	else over_exerted = TRUE;
#endif
	if (need_mana > p_ptr->csp) over_exerted = need_mana - p_ptr->csp;


	/*:::失敗判定とペナルティ、徳減少など*/
	/* Failed spell */
	if (randint0(100) < chance )
	{
		if (flush_failure) flush();

#ifdef JP
msg_format("%sをうまく唱えられなかった！", prayer);
#else
		msg_format("You failed to get the %s off!", prayer);
#endif

		sound(SOUND_FAIL);

		if(p_ptr->pseikaku == SEIKAKU_BERSERK)
		{
			 berserk_spell_fail_effect(spell);
			 if(p_ptr->is_dead) return;
		}

///del131214 virtue
/*
		switch (realm)
		{
		case REALM_LIFE:
			if (randint1(100) < chance) chg_virtue(V_VITALITY, -1);
			break;
		case REALM_DEATH:
			if (randint1(100) < chance) chg_virtue(V_UNLIFE, -1);
			break;
		case REALM_NATURE:
			if (randint1(100) < chance) chg_virtue(V_NATURE, -1);
			break;
		case REALM_DAEMON:
			if (randint1(100) < chance) chg_virtue(V_JUSTICE, 1);
			break;
		case REALM_CRUSADE:
			if (randint1(100) < chance) chg_virtue(V_JUSTICE, -1);
			break;
		case REALM_HEX:
			if (randint1(100) < chance) chg_virtue(V_COMPASSION, -1);
			break;
		default:
			if (randint1(100) < chance) chg_virtue(V_KNOWLEDGE, -1);
			break;
		}
*/
		/* Failure casting may activate some side effect */
		do_spell(realm, spell, SPELL_FAIL);



		///realm 呪文失敗時の領域ごとのペナルティ
		if ((o_ptr->tval == TV_BOOK_CHAOS) && (randint1(100) < spell))
		{
#ifdef JP
msg_print("カオス的な効果を発生した！");
#else
			msg_print("You produce a chaotic effect!");
#endif

			wild_magic(spell);
		}

		else if ((o_ptr->tval == TV_BOOK_DARKNESS) && (randint1(100) < spell))
		{
			if ((sval == 3) && one_in_(2))
			{
				sanity_blast(0, TRUE);
			}
			else
			{
#ifdef JP
				msg_print("痛い！");
#else
				msg_print("It hurts!");
#endif

#ifdef JP
				take_hit(DAMAGE_LOSELIFE, damroll(o_ptr->sval + 1, 6), "暗黒魔法の逆流", -1);
#else
				take_hit(DAMAGE_LOSELIFE, damroll(o_ptr->sval + 1, 6), "a miscast Darkness spell", -1);
#endif

				///mod131228 生命力維持削除
				//if ((spell > 15) && one_in_(6) && !p_ptr->hold_life)
				if ((spell > 15) && one_in_(6) && !p_ptr->resist_neth)
					lose_exp(spell * 250);
			}
		}
#if 0

		else if ((o_ptr->tval == TV_MUSIC_BOOK) && (randint1(200) < spell))
		{
#ifdef JP
msg_print("いやな音が響いた");
#else
msg_print("An infernal sound echoed.");
#endif

			aggravate_monsters(0);
		}
#endif

		/*:::フランは魔法失敗時魔法書が壊れることがある*/
		if(p_ptr->pclass == CLASS_FLAN && sval < 2 && one_in_(6))
		{
			msg_print(_("魔法書が手の中で爆発した！", "The spellbook explodes in your hand!"));
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

		//if (randint1(100) >= chance)
		//	chg_virtue(V_CHANCE,-1);
	}

	/* Process spell */
	/*:::成功時　効果発生、徳増加、経験値獲得など*/
	else
	{
		int useup_k_idx;
		/* Canceled spells cost neither a turn nor mana */
		/*:::ここで魔法の効果が発生*/

		if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO))
		{
			msg_print(_("あなたの詠唱は力強く響いた！", "Your chanting echoes powerfully!"));
		}

		if(flag_spell_consume_book)
		{
			msg_print(_("本に書かれた魔法が飛び出してきた！", "The spell written in the books leaps out!"));
			useup_k_idx = o_ptr->k_idx;
		}

		/*:::魔法使用実体部分*/
		if (!do_spell(realm, spell, SPELL_CAST)) return;

		/*:::紫は魔法書を消費する*/
		///mod160301 アイテムを消費する魔法によってアイテム欄がずれることを考慮し、使った本を探し直すようにした
		if(flag_spell_consume_book)
		{
			int item2 = 0;
			msg_print(_("魔法書は白紙になってしまった。", "The spellbook becomes blank."));
			if(item >= 0)
			{
				for(item2=0;item2<INVEN_PACK;item2++)
				{
					if(inventory[item2].k_idx == useup_k_idx) break;
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
					if(o_list[item2].k_idx == useup_k_idx)break;
					next_o_idx = o_list[item2].next_o_idx;
				}
				if(item2)
				{
					floor_item_increase(item2, -1);
					floor_item_describe(item2);
					floor_item_optimize(item2);
				}
			}
			p_ptr->window |= (PW_INVEN);
			energy_use = 100;

			return;
		}

		///del131214 virtue
		//if (randint1(100) < chance)
		//	chg_virtue(V_CHANCE,1);

		/* A spell was cast */
		/*:::習得した呪文を最初に使ったとき*/
		if (!(increment ?
		    (p_ptr->spell_worked2 & (1L << spell)) :
		    (p_ptr->spell_worked1 & (1L << spell)))
		    && !(REALM_SPELLMASTER))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (realm == p_ptr->realm1)
			{
				p_ptr->spell_worked1 |= (1L << spell);
			}
			else
			{
				p_ptr->spell_worked2 |= (1L << spell);
			}

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			/* Redraw object recall */
			p_ptr->window |= (PW_OBJECT);

///del131214 virtue
/*
			switch (realm)
			{
			case REALM_LIFE:
				chg_virtue(V_TEMPERANCE, 1);
				chg_virtue(V_COMPASSION, 1);
				chg_virtue(V_VITALITY, 1);
				chg_virtue(V_DILIGENCE, 1);
				break;
			case REALM_DEATH:
				chg_virtue(V_UNLIFE, 1);
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_FAITH, -1);
				chg_virtue(V_VITALITY, -1);
				break;
			case REALM_DAEMON:
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_FAITH, -1);
				chg_virtue(V_HONOUR, -1);
				chg_virtue(V_TEMPERANCE, -1);
				break;
			case REALM_CRUSADE:
				chg_virtue(V_FAITH, 1);
				chg_virtue(V_JUSTICE, 1);
				chg_virtue(V_SACRIFICE, 1);
				chg_virtue(V_HONOUR, 1);
				break;
			case REALM_NATURE:
				chg_virtue(V_NATURE, 1);
				chg_virtue(V_HARMONY, 1);
				break;
			case REALM_HEX:
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_FAITH, -1);
				chg_virtue(V_HONOUR, -1);
				chg_virtue(V_COMPASSION, -1);
				break;
			default:
				chg_virtue(V_KNOWLEDGE, 1);
				break;
			}
			*/
		}

///del131214 virtue
/*
		switch (realm)
		{
		case REALM_LIFE:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_TEMPERANCE, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_COMPASSION, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_VITALITY, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_DILIGENCE, 1);
			break;
		case REALM_DEATH:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_UNLIFE, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_JUSTICE, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_FAITH, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_VITALITY, -1);
			break;
		case REALM_DAEMON:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_JUSTICE, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_FAITH, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_HONOUR, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_TEMPERANCE, -1);
			break;
		case REALM_CRUSADE:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_FAITH, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_JUSTICE, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_SACRIFICE, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_HONOUR, 1);
			break;
		case REALM_NATURE:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_NATURE, 1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_HARMONY, 1);
			break;
		case REALM_HEX:
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_JUSTICE, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_FAITH, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_HONOUR, -1);
			if (randint1(100 + p_ptr->lev) < need_mana) chg_virtue(V_COMPASSION, -1);
			break;
		}
*/
		///sys 呪文熟練度　
//		if (mp_ptr->spell_xtra & MAGIC_GAIN_EXP)
		if(!(REALM_SPELLMASTER) )
		{
			s16b cur_exp = p_ptr->spell_exp[(increment ? 32 : 0)+spell];
			s16b exp_gain = 0;

			if (cur_exp < SPELL_EXP_BEGINNER)
				exp_gain += 60;
			else if (cur_exp < SPELL_EXP_SKILLED)
			{
				if ((dun_level > 4) && ((dun_level + 10) > p_ptr->lev))
					exp_gain = 8;
			}
			else if (cur_exp < SPELL_EXP_EXPERT)
			{
				if (((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel))
					exp_gain = 2;
			}
			else if ((cur_exp < SPELL_EXP_MASTER) && !increment)
			{
				if (((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel))
					exp_gain = 1;
			}
			///mod160206 妖狐の妖術習得にボーナス
			if(p_ptr->prace == RACE_YOUKO && is_evil_realm(p_ptr->realm1) && is_evil_realm(realm) && cur_exp < (SPELL_EXP_MASTER-1))
			{
				exp_gain *= 2;
			}

			p_ptr->spell_exp[(increment ? 32 : 0) + spell] += exp_gain;
		}

#ifdef NEW_PLAYER_LOG
		{
		    cptr spell_name;
		    spell_name = do_spell(use_realm, spell, SPELL_NAME);

            player_log_record_spell_use(p_log_ptr, spell_name);
		}
#endif
	}

	/* Take a turn */
	energy_use = 100;

	if(p_ptr->pclass == CLASS_HIGH_MAGE && (p_ptr->realm1 == TV_BOOK_ELEMENT || p_ptr->realm1 == TV_BOOK_CHAOS)) energy_use -= p_ptr->lev / 2;

	/* Over-exert the player */
	/*:::呪文強行ペナルティ*/
	if(over_exerted)
	{

		//v1.1.84 呪文強行ペナルティの変更
		process_over_exert(over_exerted);

#if 0
		int oops = need_mana;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
#ifdef JP
msg_print("精神を集中しすぎて気を失ってしまった！");
#else
		msg_print("You faint from the effort!");
#endif


		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint1(5 * oops + 1));

///del131214 virtue
/*
		switch (realm)
		{
		case REALM_LIFE:
			chg_virtue(V_VITALITY, -10);
			break;
		case REALM_DEATH:
			chg_virtue(V_UNLIFE, -10);
			break;
		case REALM_DAEMON:
			chg_virtue(V_JUSTICE, 10);
			break;
		case REALM_NATURE:
			chg_virtue(V_NATURE, -10);
			break;
		case REALM_CRUSADE:
			chg_virtue(V_JUSTICE, -10);
			break;
		case REALM_HEX:
			chg_virtue(V_COMPASSION, 10);
			break;
		default:
			chg_virtue(V_KNOWLEDGE, -10);
			break;
		}
*/
		/* Damage CON (possibly permanently) */
		if (randint0(100) < 50)
		{
			bool perm = (randint0(100) < 25);

			/* Message */
#ifdef JP
msg_print("体を悪くしてしまった！");
#else
			msg_print("You have damaged your health!");
#endif


			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint1(10), perm);
		}

#endif
	}

	else
	{
		p_ptr->csp -= need_mana;
		///mod150327 変身魔法でMPが大幅に減った時MPがマイナスになるのを修正するために追加
		if(p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}

	//尤魔は魔法を使うたびに満腹度を消費する
	if (p_ptr->pclass == CLASS_YUMA)
	{
		set_food(p_ptr->food - (300 + (9+spell)*30));
	}


	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}


static bool ang_sort_comp_pet_dismiss(vptr u, vptr v, int a, int b)
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

	if (w1 == p_ptr->riding) return TRUE;
	if (w2 == p_ptr->riding) return FALSE;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if (!m_ptr1->parent_m_idx && m_ptr2->parent_m_idx) return TRUE;
	if (!m_ptr2->parent_m_idx && m_ptr1->parent_m_idx) return FALSE;

	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->hp > m_ptr2->hp) return TRUE;
	if (m_ptr2->hp > m_ptr1->hp) return FALSE;

	return w1 <= w2;
}

///sys align
///pend mod131231 モンスターフラグ変更　ペットによる勢力処理？いずれ変更要
void check_pets_num_and_align(monster_type *m_ptr, bool inc)
{
	s32b old_friend_align = friend_align;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (inc)
	{
		total_friends++;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) friend_align += r_ptr->level;
		if (r_ptr->flags3 & RF3_ANG_CHAOS) friend_align -= r_ptr->level;
	}
	else
	{
		total_friends--;
		if (r_ptr->flags3 & RF3_ANG_COSMOS) friend_align -= r_ptr->level;
		if (r_ptr->flags3 & RF3_ANG_CHAOS) friend_align += r_ptr->level;
	}

	if (old_friend_align != friend_align) p_ptr->update |= (PU_BONUS);
}

/*:::ペット維持コストの計算*/
/*:::100を超えるとMPがターン毎に減る*/
/*:::80/ペット維持係数*levelが維持可能なモンスター合計レベル・・かと思ったがそうでもないらしい*/
/*:::MPがない職でも維持上限*2までは大丈夫らしい？*/
///sys ペット維持コストを厳しめに、でも特定アイテム装備時や高魅力時には緩和する予定

///mod140929 高レベルペナルティ、支配の王錫、魅力などの要素を考慮
int calculate_upkeep(void)
{
	s32b old_friend_align = friend_align;
	int m_idx;
	bool have_a_unique = FALSE;
	s32b total_friend_levels = 0;
	bool oujaku = FALSE;
	bool jinkon = FALSE;

	if(inventory[INVEN_RARM].name1 == ART_OUJAKU || inventory[INVEN_LARM].name1 == ART_OUJAKU) oujaku = TRUE;
	if(inventory[INVEN_LITE].name1 == ART_JINKONTOU) jinkon = TRUE;

	total_friends = 0;
	friend_align = 0;

	for (m_idx = m_max - 1; m_idx >=1; m_idx--)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;

		m_ptr = &m_list[m_idx];
		if (!m_ptr->r_idx) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		if (is_pet(m_ptr))
		{
			int tmp_lev=0;
			total_friends++;
			///mod140929 ペットコスト計算方法変更
			/*
			if (r_ptr->flags1 & RF1_UNIQUE && !(m_ptr->mflag & MFLAG_EPHEMERA))
			{
				///class　騎兵はペット維持コストに特殊計算
				if (p_ptr->pclass == CLASS_CAVALRY)
				{
					if (p_ptr->riding == m_idx)
						total_friend_levels += (r_ptr->level+5)*2;
					else if (!have_a_unique && (r_info[m_ptr->r_idx].flags7 & RF7_RIDING))
						total_friend_levels += (r_ptr->level+5)*7/2;
					else
						total_friend_levels += (r_ptr->level+5)*10;
					have_a_unique = TRUE;
				}
				else
					total_friend_levels += (r_ptr->level+5)*10;

			}
			else
				total_friend_levels += r_ptr->level;
			*/
			//高レベル配下にペナルティ
			///mod141101 高レベルペットペナルティ *5→*3
			if(r_ptr->level > p_ptr->lev) tmp_lev = (r_ptr->level - p_ptr->lev) * 3 + p_ptr->lev;
			else tmp_lev = r_ptr->level;
			//魅力で維持可能数増減　adj_chrは0-85,限界突破150
			tmp_lev = tmp_lev * 100 / (adj_chr_chm[p_ptr->stat_ind[A_CHR]]+65);
			//騎乗中のモンスターに技能に応じボーナス
			//v1.1.41 舞と里乃の特殊騎乗除く
			if (p_ptr->riding == m_idx && !CLASS_RIDING_BACKDANCE)
			{
				tmp_lev -= tmp_lev * (10000 - ref_skill_exp(SKILL_RIDING)) / 10000;
			}

			//ユニークにペナルティ　一時的存在にボーナス
			///mod141101 ユニークペナルティ *5→*3
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				tmp_lev *= 3;
				if (p_ptr->pclass == CLASS_YACHIE) tmp_lev /= 2;
			}
			if(m_ptr->mflag & MFLAG_EPHEMERA) tmp_lev /= 5;

			//守護者の混沌勢力ペットにペナルティ
			if(p_ptr->pclass == CLASS_PALADIN && (r_ptr->flags3 & RF3_ANG_CHAOS)) tmp_lev *= 10;

			//ほか一部種族や職業にボーナス
			if((r_ptr->flags3 & RF3_UNDEAD) && p_ptr->prace == RACE_LICH) tmp_lev /= 3;
			else if((r_ptr->flags3 & RF3_UNDEAD) && p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_NECROMANCY) tmp_lev /= 3;

			if((r_ptr->flags3 & RF3_DEMON) && p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_DARKNESS) tmp_lev /= 3;

			if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_NATURE)
			{
				if(r_ptr->flags3 & RF3_ANIMAL) tmp_lev /= 3;
				if( m_ptr->r_idx == MON_FENGHUANG || m_ptr->r_idx == MON_KIRIN || m_ptr->r_idx == MON_OURYU || m_ptr->r_idx == MON_REIKI) tmp_lev /= 3;
			}

			if((r_ptr->flags3 & RF3_UNDEAD) && p_ptr->pclass == CLASS_YUYUKO) tmp_lev /= 3;

			if ((r_ptr->flags3 & RF3_UNDEAD) && is_special_seikaku(SEIKAKU_SPECIAL_KASEN)) tmp_lev /= 2;

			if((r_ptr->flags3 & RF3_UNDEAD) && jinkon) tmp_lev /= 3;

			if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_SUMMONS) tmp_lev /= 3;

			if ((r_ptr->flags3 & RF3_HANIWA) && p_ptr->pclass == CLASS_MAYUMI) tmp_lev /= 3;

			if(p_ptr->prace == RACE_LUNARIAN && (r_ptr->flags3 & RF3_GEN_MOON)) tmp_lev /= 3;//月の民は月面勢力の配下のコストが安い

			if (p_ptr->prace == CLASS_URUMI && (r_ptr->flags7 & RF7_AQUATIC)) tmp_lev /= 2;//潤美は水生生物のコストが安い


			//v1.1.15 ヘカーティアは地獄の配下のコスト低い
			if(p_ptr->pclass == CLASS_HECATIA && (r_ptr->flags7 & RF7_FROM_HELL)) tmp_lev /= 7;
			else if(p_ptr->pclass == CLASS_HECATIA && (r_ptr->flags3 & RF3_UNDEAD)) tmp_lev /= 4;
			else if(p_ptr->pclass == CLASS_HECATIA && (r_ptr->flags3 & RF3_ANG_CHAOS)) tmp_lev /= 2;




			if(difficulty == DIFFICULTY_LUNATIC) tmp_lev *= 2;
			if(oujaku) tmp_lev /= 4;

			if(tmp_lev < 1) tmp_lev = 1;
			/* Determine pet alignment */
			///mon
			if (r_ptr->flags3 & RF3_ANG_COSMOS) friend_align += r_ptr->level;
			if (r_ptr->flags3 & RF3_ANG_CHAOS) friend_align -= r_ptr->level;

			total_friend_levels += tmp_lev;
		}
	}
	if (old_friend_align != friend_align) p_ptr->update |= (PU_BONUS);
	if (total_friends)
	{
		int upkeep_factor;
		upkeep_factor = (total_friend_levels - (p_ptr->lev * 80 / (cp_ptr->pet_upkeep_div)));


		if (upkeep_factor < 0) upkeep_factor = 0;
		if (upkeep_factor > 1000) upkeep_factor = 1000;

		return upkeep_factor;
	}
	else
		return 0;
}

/*:::ペットを放す　ペットコマンドとコスト超過時に呼ばれる*/
void do_cmd_pet_dismiss(void)
{
	monster_type	*m_ptr;
	bool		all_pets = FALSE;
	int pet_ctr, i;
	int Dismissed = 0;

	u16b *who;
	u16b dummy_why;
	int max_pet = 0;
	int cu, cv;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Allocate the "who" array */
	C_MAKE(who, max_m_idx, u16b);

	/* Process the monsters (backwards) */
	for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		if (is_pet(&m_list[pet_ctr]))
			who[max_pet++] = pet_ctr;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_pet_dismiss;
	ang_sort_swap = ang_sort_swap_hook;

	ang_sort(who, &dummy_why, max_pet);

	/* Process the monsters (backwards) */
	for (i = 0; i < max_pet; i++)
	{
		bool delete_this;
		char friend_name[80];
		//mod150618 長い名前のモンスターを開放したとき落ちるのでバッファサイズを変更した。80→160
		//でもなぜ本家で落ちないのにこっちで落ちるのか原因がわからない。
		char buf[160];
		bool kakunin;

		/* Access the monster */
		pet_ctr = who[i];
		m_ptr = &m_list[pet_ctr];

		delete_this = FALSE;
		kakunin = ((pet_ctr == p_ptr->riding) || (m_ptr->nickname));
		monster_desc(friend_name, m_ptr, MD_ASSUME_VISIBLE);
		if (!all_pets)
		{
			/* Hack -- health bar for this monster */
			health_track(pet_ctr);

			/* Hack -- handle stuff */
			handle_stuff();

#ifdef JP
			sprintf(buf, "%sを放しますか？ [Yes/No/Unnamed (%d体)]", friend_name, max_pet - i);
#else
			sprintf(buf, "Dismiss %s? [Yes/No/Unnamed (%d remain)]", friend_name, max_pet - i);
#endif
			prt(buf, 0, 0);

			if (m_ptr->ml)
				move_cursor_relative(m_ptr->fy, m_ptr->fx);

			while (TRUE)
			{
				char ch = inkey();

				if (ch == 'Y' || ch == 'y')
				{
					delete_this = TRUE;

					if (kakunin)
					{
#ifdef JP
						sprintf(buf, "本当によろしいですか？ (%s) ", friend_name);
#else
						sprintf(buf, "Are you sure? (%s) ", friend_name);
#endif
						if (!get_check(buf))
							delete_this = FALSE;
					}
					break;
				}

				if (ch == 'U' || ch == 'u')
				{
					all_pets = TRUE;
					break;
				}

				if (ch == ESCAPE || ch == 'N' || ch == 'n')
					break;

				bell();
			}
		}

		if ((all_pets && !kakunin) || (!all_pets && delete_this))
		{
			if (record_named_pet && m_ptr->nickname)
			{
				char m_name[80];

				monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_DISMISS, m_name);
			}

			if (pet_ctr == p_ptr->riding)
			{
#ifdef JP
				if (CLASS_RIDING_BACKDANCE)
					msg_format("%sから離れた。", friend_name);
				else
					msg_format("%sから降りた。", friend_name);

#else
				msg_format("You have got off %s. ", friend_name);
#endif

				p_ptr->riding = 0;

				/* Update the monsters */
				p_ptr->update |= (PU_BONUS | PU_MONSTERS);
				p_ptr->redraw |= (PR_EXTRA | PR_UHEALTH);
			}


			if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
			{
				sprintf(buf, _("%sは元の武器に戻った。",
                                "%s returns to its weapon form."), friend_name);
				monster_drop_carried_objects(m_ptr);
			}
			/* HACK : Add the line to message buffer */
#ifdef JP
			else
			{
				if(is_gen_unique(m_ptr->r_idx))
					sprintf(buf, "%s と別れた。", friend_name);
				else
					sprintf(buf, "%s を放した。", friend_name);
			}
#else
			else
			{
				if(is_gen_unique(m_ptr->r_idx))
					sprintf(buf, "You part ways with %s.", friend_name);
				else
					sprintf(buf, "You dismiss %s.", friend_name);
			}
#endif
			message_add(buf);
			p_ptr->window |= (PW_MESSAGE);
			window_stuff();



			delete_monster_idx(pet_ctr);
			Dismissed++;
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	C_KILL(who, max_m_idx, u16b);

#ifdef JP
	msg_format("%d 体の配下を放しました。", Dismissed);
#else
	msg_format("You have dismissed %d follower%s.", Dismissed,
		   (Dismissed == 1 ? "" : "s"));
#endif
	if (Dismissed == 0 && all_pets)
#ifdef JP
		msg_print("'U'nnamed は、乗馬以外の名前のない配下だけを全て解放します。");
#else
		msg_print("'U'nnamed means all your followers except named followers and your mount.");
#endif
}

static bool player_can_ride_aux(cave_type *c_ptr, bool now_riding)
{
	bool p_can_enter;
	bool old_character_xtra = character_xtra;
	int  old_riding = p_ptr->riding;
	bool old_riding_ryoute = p_ptr->riding_ryoute;
	bool old_old_riding_ryoute = p_ptr->old_riding_ryoute;
	bool old_pf_ryoute = (p_ptr->pet_extra_flags & PF_RYOUTE) ? TRUE : FALSE;

	/* Hack -- prevent "icky" message */
	character_xtra = TRUE;

	if (now_riding) p_ptr->riding = c_ptr->m_idx;
	else
	{
		p_ptr->riding = 0;
		p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		p_ptr->riding_ryoute = p_ptr->old_riding_ryoute = FALSE;
	}

	calc_bonuses();

	p_can_enter = player_can_enter(c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);

	p_ptr->riding = old_riding;
	if (old_pf_ryoute) p_ptr->pet_extra_flags |= (PF_RYOUTE);
	else p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	p_ptr->riding_ryoute = old_riding_ryoute;
	p_ptr->old_riding_ryoute = old_old_riding_ryoute;

	calc_bonuses();

	character_xtra = old_character_xtra;

	return p_can_enter;
}

/*:::落馬判定　落馬でダメージを受けたらTRUEが返る。落馬したときではない。*/
///sys 落馬判定　技能によりもう少し判定を緩和
//乗馬中モンスターが死んだり変身したりモンスターボール反射されたらdam=-1で呼ばれる。そのときも強制落馬になる。FORCEオプションとの違いは@の位置が変わらないことだけ？
bool rakuba(int dam, bool force)
{
	int i, y, x, oy, ox;
	int sn = 0, sy = 0, sx = 0;
	char m_name[80];
	monster_type *m_ptr = &m_list[p_ptr->riding];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	bool fall_dam = FALSE;

	if (!p_ptr->riding) return FALSE;
	if (p_ptr->wild_mode) return FALSE;


	// -HACK- 舞と里乃のバックダンスは通常の騎乗と違い落馬しないが、モンスターが倒されたりモンスターボール反射食らって消えたときだけ落馬扱いにする。
	//そのときはdam=-1でこの関数が呼ばれるのでそれ以外は落馬処理終了にしておく。
	if (CLASS_RIDING_BACKDANCE && dam >= 0)
		return FALSE;

	if (dam >= 0 || force)
	{
		if (!force)
		{
			///mod131226 skill 技能と武器技能の統合
			//int cur = p_ptr->skill_exp[GINOU_RIDING];
			int cur = ref_skill_exp(SKILL_RIDING);
			//int max = s_info[p_ptr->pclass].s_max[GINOU_RIDING];
			int max = 8000;
			int ridinglevel = r_ptr->level;

			/* 落馬のしやすさ */
			int rakubalevel = r_ptr->level;
			if (p_ptr->riding_ryoute) rakubalevel += 20;

			//v1.1.95 狂戦士化した乗馬は乗りこなしにくいことにする
			if (MON_BERSERK(m_ptr))
			{
				rakubalevel = rakubalevel * 3 / 2;
			}


			///mod140705 乗馬スキルが極めて高いと落馬回避判定にボーナス　騎兵とかでレベルの上がったパワーDに乗るための修正
			///mod141011 やはり中止。落馬計算式で調整する
			//if(cur > 6400) rakubalevel = MAX(rakubalevel-20,0);

/*:::落馬しそうになると経験値を得る処理があったが削除した*/
#if 0
			if ((cur < max) && (max > 1000) &&
			    (dam / 2 + ridinglevel) > (cur / 30 + 10))
			{
				int inc = 0;

				if (ridinglevel > (cur / 100 + 15))
					inc += 1 + (ridinglevel - cur / 100 - 15);
				else
					inc += 1;
				/*:::落馬しそうになると経験値を得るらしい*/
				//p_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);
			}
#endif
			/* レベルの低い乗馬からは落馬しにくい */
			/*:::落馬処理 damは最大200のはず*/
			///mod141011 cur/30→cur/25
			if (randint0(dam / 2 + rakubalevel * 2) < cur / 25 + 10)
			{
					return FALSE;
				///mod140705 騎兵と魔獣使い以外は常に低確率で落馬する判定を消した
				/*
				if ((((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY)) && !p_ptr->riding_ryoute) || !one_in_(p_ptr->lev*(p_ptr->riding_ryoute ? 2 : 3) + 30))
				{
					return FALSE;
				}
				*/
			}
		}

		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			cave_type *c_ptr;

			/* Access the location */
			y = py + ddy_ddd[i];
			x = px + ddx_ddd[i];

			c_ptr = &cave[y][x];

			if (c_ptr->m_idx) continue;

			/* Skip non-empty grids */
			if (!cave_have_flag_grid(c_ptr, FF_MOVE) && !cave_have_flag_grid(c_ptr, FF_CAN_FLY))
			{
				if (!player_can_ride_aux(c_ptr, FALSE)) continue;
			}

			if (cave_have_flag_grid(c_ptr, FF_PATTERN)) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if (randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}
		if (!sn)
		{
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%sから振り落とされそうになって、壁にぶつかった。",m_name);
			take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "壁への衝突", -1);
#else
			msg_format("You have nearly fallen from %s, but bumped into wall.",m_name);
			take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "bumping into wall", -1);
#endif
			return FALSE;
		}

		oy = py;
		ox = px;

		py = sy;
		px = sx;

		/* Redraw the old spot */
		lite_spot(oy, ox);

		/* Redraw the new spot */
		lite_spot(py, px);

		/* Check for new panel */
		verify_panel();
	}

	p_ptr->riding = 0;
	p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	p_ptr->riding_ryoute = p_ptr->old_riding_ryoute = FALSE;

	calc_bonuses();

	p_ptr->update |= (PU_BONUS);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	p_ptr->redraw |= (PR_EXTRA);

	/* Update health track of mount */
	p_ptr->redraw |= (PR_UHEALTH);

	//v1.1.41 バックダンスの落馬(モンスター死亡時など)は落馬ダメージなし
	if (CLASS_RIDING_BACKDANCE)
	{
		if(p_ptr->pclass == CLASS_TSUKASA)
			msg_format(_("あなたは寄生先から逃れた。", "You disengage from your current host."));
		else
            msg_format(_("踊りを止めた。", "You stop dancing."));
	}

	else if (p_ptr->levitation && !force)
	{
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%sから落ちたが、空中でうまく体勢を立て直して着地した。", m_name);
#else
		msg_format("You are thrown from %s, but make a good landing.", m_name);
#endif
	}
	else
	{
#ifdef JP
		take_hit(DAMAGE_NOESCAPE, r_ptr->level + 3, "落馬", -1);
#else
		take_hit(DAMAGE_NOESCAPE, r_ptr->level + 3, "Falling from riding", -1);
#endif
		fall_dam = TRUE;
	}

	/* Move the player */
	if (sy && !p_ptr->is_dead)
		(void)move_player_effect(py, px, MPE_DONT_PICKUP | MPE_DONT_SWAP_MON);


	return fall_dam;
}

/*:::ペットに騎乗する/降りる*/
bool do_riding(bool force)
{
	int x, y, dir = 0;
	cave_type *c_ptr;
	monster_type *m_ptr;
	char m_name[80];

	if (!get_rep_dir2(&dir)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	c_ptr = &cave[y][x];

	if (p_ptr->special_defense & KATA_MUSOU) set_action(ACTION_NONE);

	if (p_ptr->pclass == CLASS_ENOKO)
	{
		msg_print(_("トラバサミが邪魔で騎乗ができない。", "You cannot ride, your beartraps get in the way."));
		return FALSE;
	}

	if(p_ptr->pclass == CLASS_SHINMYOU_2)
	{
		msg_print(_("お椀に乗ったまま騎乗はできない。", "You cannot ride while you're in your bowl."));
		return FALSE;
	}

	if (p_ptr->riding)
	{
		/* Skip non-empty grids */
		if (!player_can_ride_aux(c_ptr, FALSE))
		{
#ifdef JP

			if(CLASS_RIDING_BACKDANCE)
				msg_print("そちらには行けない。");
			else
				msg_print("そちらには降りられません。");
#else
			msg_print("You cannot go to that direction.");
#endif
			return FALSE;
		}

		if (!pattern_seq(py, px, y, x)) return FALSE;

		if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("モンスターが立ちふさがっている！");
#else
			msg_print("There is a monster in the way!");
#endif

			py_attack(y, x, 0);
			return FALSE;
		}

		if (CLASS_RIDING_BACKDANCE)
		{
			m_ptr = &m_list[p_ptr->riding];
			monster_desc(m_name, m_ptr, 0);
			msg_format(_("%sの背後から離れた。", "You step away from behind %s."), m_name);

		}

		p_ptr->riding = 0;
		p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		p_ptr->riding_ryoute = p_ptr->old_riding_ryoute = FALSE;

	}
	else
	{
		if (p_ptr->confused && !CLASS_RIDING_BACKDANCE)
		{
#ifdef JP
			msg_print("混乱していて乗れない！");
#else
			msg_print("You are too confused!");
#endif
			return FALSE;
		}

		if (p_ptr->pclass == CLASS_ALICE)
		{
			msg_print(_("騎乗しながらでは人形を操れない。", "You won't be able to manipulate your dolls while riding."));
			return FALSE;
		}

		if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_RIDING || p_ptr->clawextend)
		{
			msg_print(_("この姿では騎乗ができない。", "You cannot ride in your current form."));
			return FALSE;
		}



		m_ptr = &m_list[c_ptr->m_idx];

		if (!c_ptr->m_idx || !m_ptr->ml)
		{
#ifdef JP
			msg_print("その場所にはモンスターはいません。");
#else
			msg_print("There is no monster.");
#endif

			return FALSE;
		}
		if (!is_pet(m_ptr) && !force)
		{
#ifdef JP
			msg_print("そのモンスターは配下ではありません。");
#else
			msg_print("That monster is not a follower.");
#endif

			return FALSE;
		}
		if (!(r_info[m_ptr->r_idx].flags7 & RF7_RIDING ) && !CLASS_RIDING_BACKDANCE)
		{
#ifdef JP
			msg_print("そのモンスターには乗れなさそうだ。");
#else
			msg_print("This monster doesn't seem suitable for riding.");
#endif

			return FALSE;
		}

		if (!pattern_seq(py, px, y, x)) return FALSE;

		if (!player_can_ride_aux(c_ptr, TRUE))
		{
			/* Feature code (applying "mimic" field) */
			feature_type *f_ptr = &f_info[get_feat_mimic(c_ptr)];
#ifdef JP
			msg_format("そのモンスターは%sの%sにいる。", f_name + f_ptr->name,
			           ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
			            (!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
			           "中" : "上");
#else
			msg_format("This monster is %s the %s.",
			           ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
			            (!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
			           "in" : "on", f_name + f_ptr->name);
#endif

			return FALSE;
		}
		///mod131226 skill 技能と武器技能の統合
		//if (r_info[m_ptr->r_idx].level > randint1((p_ptr->skill_exp[GINOU_RIDING] / 50 + p_ptr->lev / 2 + 20)))

		//v1.1.41 バックダンスは乗馬失敗しない
		if (!CLASS_RIDING_BACKDANCE &&
			(r_info[m_ptr->r_idx].level > randint1((ref_skill_exp(SKILL_RIDING) / 50 + p_ptr->lev / 2 + 20))))
		{
#ifdef JP
			msg_print("うまく乗れなかった。");
#else
			msg_print("You failed to ride.");
#endif

			energy_use = 100;

			return FALSE;
		}

		if (MON_CSLEEP(m_ptr))
		{
			monster_desc(m_name, m_ptr, 0);
			(void)set_monster_csleep(c_ptr->m_idx, 0);
#ifdef JP
			msg_format("%sを起こした。", m_name);
#else
			msg_format("You have woken %s up.", m_name);
#endif
		}

		if (p_ptr->action == ACTION_KAMAE) set_action(ACTION_NONE);

		p_ptr->riding = c_ptr->m_idx;

		/* Hack -- remove tracked monster */
		if (p_ptr->riding == p_ptr->health_who) health_track(0);

		if (CLASS_RIDING_BACKDANCE)
		{
			monster_desc(m_name, m_ptr, 0);
			if(p_ptr->pclass == CLASS_TSUKASA)
				msg_format(_("あなたは%sにすり寄って背後から囁き始めた...", "You slip behind %s and start whispering..."), m_name);
			else
				msg_format(_("あなたは%sの背後で踊り始めた...", "You started dancing behind %s..."), m_name);

			if (p_ptr->pclass == CLASS_SATONO)
				make_magic_list_satono();

		}
	}

	energy_use = 100;

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_BONUS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP | PR_EXTRA);

	p_ptr->redraw |= (PR_UHEALTH);
	//v1.1.41追加
	p_ptr->redraw |= (PR_SPEED);

	/* Move the player */
	(void)move_player_effect(y, x, MPE_HANDLE_STUFF | MPE_ENERGY_USE | MPE_DONT_PICKUP | MPE_DONT_SWAP_MON);

	return TRUE;
}

static void do_name_pet(void)
{
	monster_type *m_ptr;
	char out_val[20];
	char m_name[80];
	bool old_name = FALSE;
	bool old_target_pet = target_pet;

	target_pet = TRUE;
	if (!target_set(TARGET_KILL))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	if (cave[target_row][target_col].m_idx)
	{
		m_ptr = &m_list[cave[target_row][target_col].m_idx];

		if (!is_pet(m_ptr))
		{
			/* Message */
#ifdef JP
			msg_print("そのモンスターは配下ではない。");
#else
			msg_format("This monster is not a follower.");
#endif
			return;
		}
		if (r_info[m_ptr->r_idx].flags1 & RF1_UNIQUE)
		{
#ifdef JP
			msg_print("そのモンスターの名前は変えられない！");
#else
			msg_format("You cannot change name of this monster!");
#endif
			return;
		}
		monster_desc(m_name, m_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%sに名前をつける。", m_name);
#else
		msg_format("Name %s.", m_name);
#endif

		msg_print(NULL);

		/* Start with nothing */
		strcpy(out_val, "");

		/* Use old inscription */
		if (m_ptr->nickname)
		{
			/* Start with the old inscription */
			strcpy(out_val, quark_str(m_ptr->nickname));
			old_name = TRUE;
		}

		/* Get a new inscription (possibly empty) */
#ifdef JP
		if (get_string("名前: ", out_val, 15))
#else
		if (get_string("Name: ", out_val, 15))
#endif

		{
			if (out_val[0])
			{
				/* Save the inscription */
				/*:::モンスターにつける名前をquarkに記憶している・？*/
				m_ptr->nickname = quark_add(out_val);
				if (record_named_pet)
				{

					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_NAME, m_name);
				}
			}
			else
			{
				if (record_named_pet && old_name)
				{

					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_UNNAME, m_name);
				}
				m_ptr->nickname = 0;
			}
		}
	}
}

/*
 * Issue a pet command
 */
///sys ペットコマンド　「攻撃するな」の命令を追加したい
void do_cmd_pet(void)
{
	int			i = 0;
	int			num;
	int			powers[36];
	cptr			power_desc[36];
	bool			flag, redraw;
	char			choice;
	char			out_val[160];
	int			pet_ctr;
	monster_type	*m_ptr;
	bool martialart = FALSE;

	int mode = 0;

	char buf[160];
	char target_buf[160];

	int menu_line = use_menu ? 1 : 0;

	num = 0;

	/*:::命令一覧のための文字を代入　変数を使うために関数内で処理しているらしい*/
#ifdef JP
	power_desc[num] = "配下を放す";
#else
	power_desc[num] = "dismiss followers";
#endif

	powers[num++] = PET_DISMISS;

#ifdef JP
	sprintf(target_buf, "配下のターゲットを指定 (現在：%s)",
		(pet_t_m_idx ? (p_ptr->image ? "何か奇妙な物" : (r_name + r_info[m_list[pet_t_m_idx].ap_r_idx].name)) : "指定なし"));
#else
	sprintf(target_buf, "specify a target of follower (now:%s)",
		(pet_t_m_idx ? (p_ptr->image ? "something strange" : (r_name + r_info[m_list[pet_t_m_idx].ap_r_idx].name)) : "nothing"));
#endif
	power_desc[num] = target_buf;

	powers[num++] = PET_TARGET;

#ifdef JP
	power_desc[num] = "近くにいろ";
#else
	power_desc[num] = "stay close";
#endif

	if (p_ptr->pet_follow_distance == PET_CLOSE_DIST) mode = num;
	powers[num++] = PET_STAY_CLOSE;

#ifdef JP
	power_desc[num] = "ついて来い";
#else
	power_desc[num] = "follow me";
#endif

	if (p_ptr->pet_follow_distance == PET_FOLLOW_DIST) mode = num;
	powers[num++] = PET_FOLLOW_ME;

#ifdef JP
	power_desc[num] = "敵を見つけて倒せ";
#else
	power_desc[num] = "seek and destroy";
#endif

	if (p_ptr->pet_follow_distance == PET_DESTROY_DIST) mode = num;
	powers[num++] = PET_SEEK_AND_DESTROY;

#ifdef JP
	power_desc[num] = "少し離れていろ";
#else
	power_desc[num] = "give me space";
#endif

	if (p_ptr->pet_follow_distance == PET_SPACE_DIST) mode = num;
	powers[num++] = PET_ALLOW_SPACE;

#ifdef JP
	power_desc[num] = "離れていろ";
#else
	power_desc[num] = "stay away";
#endif

	if (p_ptr->pet_follow_distance == PET_AWAY_DIST) mode = num;
	powers[num++] = PET_STAY_AWAY;

	if (p_ptr->pet_extra_flags & PF_OPEN_DOORS)
	{
#ifdef JP
		power_desc[num] = "ドアを開ける (現在:ON)";
#else
		power_desc[num] = "followers open doors (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "ドアを開ける (現在:OFF)";
#else
		power_desc[num] = "followers open doors (now Off)";
#endif
	}
	powers[num++] = PET_OPEN_DOORS;

	if (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
	{
#ifdef JP
		power_desc[num] = "アイテムを拾う (現在:ON)";
#else
		power_desc[num] = "followers pick up items (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "アイテムを拾う (現在:OFF)";
#else
		power_desc[num] = "followers pick up items (now Off)";
#endif
	}
	powers[num++] = PET_TAKE_ITEMS;

	if (p_ptr->pet_extra_flags & PF_TELEPORT)
	{
#ifdef JP
		power_desc[num] = "テレポート系魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow teleport (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "テレポート系魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow teleport (now Off)";
#endif
	}
	powers[num++] = PET_TELEPORT;

	if (p_ptr->pet_extra_flags & PF_ATTACK_SPELL)
	{
#ifdef JP
		power_desc[num] = "攻撃魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow cast attack spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "攻撃魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow cast attack spell (now Off)";
#endif
	}
	powers[num++] = PET_ATTACK_SPELL;

	if (p_ptr->pet_extra_flags & PF_SUMMON_SPELL)
	{
#ifdef JP
		power_desc[num] = "召喚魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow cast summon spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "召喚魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow cast summon spell (now Off)";
#endif
	}
	powers[num++] = PET_SUMMON_SPELL;

	if (p_ptr->pet_extra_flags & PF_BALL_SPELL)
	{
#ifdef JP
		power_desc[num] = "プレイヤーを巻き込む範囲魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow involve player in area spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "プレイヤーを巻き込む範囲魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow involve player in area spell (now Off)";
#endif
	}
	powers[num++] = PET_BALL_SPELL;

	if (p_ptr->riding)
	{
#ifdef JP
		if (CLASS_RIDING_BACKDANCE)
			power_desc[num] = "踊りを止める";
		else
			power_desc[num] = "配下から降りる";
#else
		if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
			power_desc[num] = "stop parasitising";
		else if (CLASS_RIDING_BACKDANCE)
			power_desc[num] = "stop dancing";
		else
			power_desc[num] = "get off a follower";
#endif
	}
	else
	{
#ifdef JP
		if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
			power_desc[num] = "配下に寄生する";
		else if(CLASS_RIDING_BACKDANCE)
			power_desc[num] = "配下の背後で踊る";
		else
			power_desc[num] = "配下に乗る";

#else
		if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
			power_desc[num] = "parasitise a follower";
		else if(CLASS_RIDING_BACKDANCE)
			power_desc[num] = "dance behind a follower";
		else
			power_desc[num] = "ride a follower";
#endif
	}
	powers[num++] = PET_RIDING;

#ifdef JP
	power_desc[num] = "配下に名前をつける";
#else
	power_desc[num] = "name followers";
#endif

	powers[num++] = PET_NAME;

	if (p_ptr->riding && !CLASS_RIDING_BACKDANCE)
	{
		if ((p_ptr->migite && (empty_hands(FALSE) == EMPTY_HAND_LARM) &&
		     object_allow_two_hands_wielding(&inventory[INVEN_RARM])) ||
		    (p_ptr->hidarite && (empty_hands(FALSE) == EMPTY_HAND_RARM) &&
			 object_allow_two_hands_wielding(&inventory[INVEN_LARM]))
			 || (object_is_shooting_weapon(&inventory[INVEN_RARM]) && !(inventory[INVEN_LARM].k_idx))
			 || (object_is_shooting_weapon(&inventory[INVEN_LARM]) && !(inventory[INVEN_RARM].k_idx))
			 )
		{
			if (p_ptr->pet_extra_flags & PF_RYOUTE)
			{
#ifdef JP
				power_desc[num] = "武器を片手で持つ";
#else
				power_desc[num] = "use one hand to control a riding follower";
#endif
			}
			else
			{
#ifdef JP
				power_desc[num] = "今の武器を両手で扱う";
#else
				power_desc[num] = "use both hands for a weapon";
#endif
			}

			powers[num++] = PET_RYOUTE;
		}
		///mod140309 馬上格闘処理を全職業適用にする
		else
		{
			martialart = TRUE;
	//switch (p_ptr->pclass)
			//{
			//case CLASS_MONK:
			//case CLASS_FORCETRAINER:
			//case CLASS_BERSERKER:
				if (empty_hands(FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					if (p_ptr->pet_extra_flags & PF_RYOUTE)
					{
#ifdef JP
						power_desc[num] = "片手で格闘する";
#else
						power_desc[num] = "use one hand to control a riding follower";
#endif
					}
					else
					{
#ifdef JP
						power_desc[num] = "両手で格闘する";
#else
						power_desc[num] = "use both hands for melee";
#endif
					}

					powers[num++] = PET_RYOUTE;
				}
				else if ((empty_hands(FALSE) != EMPTY_HAND_NONE) && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
					if (p_ptr->pet_extra_flags & PF_RYOUTE)
					{
#ifdef JP
						power_desc[num] = "格闘を行わない";
#else
						power_desc[num] = "use one hand to control a riding follower";
#endif
					}
					else
					{
#ifdef JP
						power_desc[num] = "格闘を行う";
#else
						power_desc[num] = "use one hand for melee";
#endif
					}

					powers[num++] = PET_RYOUTE;
				}
				//break;
			//}
		}
	}

#ifdef ALLOW_REPEAT
	if (!(repeat_pull(&i) && (i >= 0) && (i < num)))
	{
#endif /* ALLOW_REPEAT */

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	if (use_menu)
	{
		/* Save the screen */
		screen_save();

		/* Build a prompt */
#ifdef JP
		strnfmt(out_val, 78, "(コマンド、ESC=終了) コマンドを選んでください:");
#else
		strnfmt(out_val, 78, "(Command, ESC=exit) Choose command from menu.");
#endif
	}
	else
	{
		/* Build a prompt */
		strnfmt(out_val, 78,
#ifdef JP
		        "(コマンド %c-%c、'*'=一覧、ESC=終了) コマンドを選んでください:",
#else
		        "(Command %c-%c, *=List, ESC=exit) Select a command: ",
#endif
		        I2A(0), I2A(num - 1));
	}

	choice = (always_show_list || use_menu) ? ESCAPE : 1;

	/* Get a command from the user */
	while (!flag)
	{
		int ask = TRUE;

		if (choice == ESCAPE) choice = ' ';
		else if (!get_com(out_val, &choice, TRUE)) break;

		if (use_menu && (choice != ' '))
		{
			switch (choice)
			{
			case '0':
				screen_load();
				return;

			case '8':
			case 'k':
			case 'K':
				menu_line += (num - 1);
				break;

			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;

			case '4':
			case 'h':
			case 'H':
				menu_line = 1;
				break;

			case '6':
			case 'l':
			case 'L':
				menu_line = num;
				break;

			case 'x':
			case 'X':
			case '\r':
			case '\n':
				i = menu_line - 1;
				ask = FALSE;
				break;
			}
			if (menu_line > num) menu_line -= num;
		}

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y = 1, x = 0;
				int ctr = 0;

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				prt("", y++, x);

				/* Print list */
				/*:::ペットコマンドリスト表示部*/
				for (ctr = 0; ctr < num; ctr++)
				{
					/* Letter/number for power selection */
#ifdef JP
					if (use_menu) sprintf(buf, "%c%s ", (ctr == mode) ? '*' : ' ', (ctr == (menu_line - 1)) ? "》" : "  ");
#else
					if (use_menu) sprintf(buf, "%c%s ", (ctr == mode) ? '*' : ' ', (ctr == (menu_line - 1)) ? "> " : "  ");
#endif
					else sprintf(buf, "%c%c) ", (ctr == mode) ? '*' : ' ', I2A(ctr));

					strcat(buf, power_desc[ctr]);

					prt(buf, y + ctr, x);
				}

				prt("", y + MIN(ctr, 17), x);
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			/* Prompt */
#ifdef JP
			strnfmt(buf, 78, "%sを使いますか？ ", power_desc[i]);
#else
			strnfmt(buf, 78, "Use %s? ", power_desc[i]);
#endif

			/* Belay that order */
			if (!get_check(buf)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}

#ifdef ALLOW_REPEAT
	repeat_push(i);
	}
#endif /* ALLOW_REPEAT */

	/*:::コマンド解釈部*/
	switch (powers[i])
	{
		case PET_DISMISS: /* Dismiss pets */
		{
			/* Check pets (backwards) */
			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				/* Player has pet */
				if (is_pet(&m_list[pet_ctr])) break;
			}

			if (!pet_ctr)
			{
#ifdef JP
				msg_print("配下がいない！");
#else
				msg_print("You have no followers!");
#endif
				break;
			}


			/*:::ペットを放す*/
			do_cmd_pet_dismiss();
			/*:::ペット維持コスト計算*/
			(void)calculate_upkeep();
			break;
		}
		/*:::ペットのターゲット指定*/
		case PET_TARGET:
		{
			project_length = -1;
			if (!target_set(TARGET_KILL)) pet_t_m_idx = 0;
			else
			{
				cave_type *c_ptr = &cave[target_row][target_col];
				if (c_ptr->m_idx && (m_list[c_ptr->m_idx].ml))
				{
					pet_t_m_idx = cave[target_row][target_col].m_idx;
					p_ptr->pet_follow_distance = PET_DESTROY_DIST;
				}
				else pet_t_m_idx = 0;
			}
			project_length = 0;

			break;
		}
		/* Call pets */
		case PET_STAY_CLOSE:
		{
			p_ptr->pet_follow_distance = PET_CLOSE_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Follow Me" */
		case PET_FOLLOW_ME:
		{
			p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Seek and destoy" */
		case PET_SEEK_AND_DESTROY:
		{
			p_ptr->pet_follow_distance = PET_DESTROY_DIST;
			break;
		}
		/* "Give me space" */
		case PET_ALLOW_SPACE:
		{
			p_ptr->pet_follow_distance = PET_SPACE_DIST;
			break;
		}
		/* "Stay away" */
		case PET_STAY_AWAY:
		{
			p_ptr->pet_follow_distance = PET_AWAY_DIST;
			break;
		}
		/* flag - allow pets to open doors */
		case PET_OPEN_DOORS:
		{
			if (p_ptr->pet_extra_flags & PF_OPEN_DOORS) p_ptr->pet_extra_flags &= ~(PF_OPEN_DOORS);
			else p_ptr->pet_extra_flags |= (PF_OPEN_DOORS);
			break;
		}
		/* flag - allow pets to pickup items */
		case PET_TAKE_ITEMS:
		{
			if (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
			{
				p_ptr->pet_extra_flags &= ~(PF_PICKUP_ITEMS);
				for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
				{
					/* Access the monster */
					m_ptr = &m_list[pet_ctr];

					if (is_pet(m_ptr))
					{
						monster_drop_carried_objects(m_ptr);
					}
				}
			}
			else p_ptr->pet_extra_flags |= (PF_PICKUP_ITEMS);

			break;
		}
		/* flag - allow pets to teleport */
		case PET_TELEPORT:
		{
			if (p_ptr->pet_extra_flags & PF_TELEPORT) p_ptr->pet_extra_flags &= ~(PF_TELEPORT);
			else p_ptr->pet_extra_flags |= (PF_TELEPORT);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_ATTACK_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) p_ptr->pet_extra_flags &= ~(PF_ATTACK_SPELL);
			else p_ptr->pet_extra_flags |= (PF_ATTACK_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_SUMMON_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) p_ptr->pet_extra_flags &= ~(PF_SUMMON_SPELL);
			else p_ptr->pet_extra_flags |= (PF_SUMMON_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_BALL_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_BALL_SPELL) p_ptr->pet_extra_flags &= ~(PF_BALL_SPELL);
			else p_ptr->pet_extra_flags |= (PF_BALL_SPELL);
			break;
		}

		case PET_RIDING:
		{
			(void)do_riding(FALSE);
			break;
		}

		case PET_NAME:
		{
			do_name_pet();
			break;
		}

		case PET_RYOUTE:
		{
			if (p_ptr->pet_extra_flags & PF_RYOUTE)
			{
				p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
			}
			else
			{
				///mod140309 騎乗モンスターから手を離した時の乗馬技能に応じたモンスターのリアクション
				p_ptr->pet_extra_flags |= (PF_RYOUTE);
				if(ref_skill_exp(SKILL_RIDING) >= RIDING_PERFECT)
				{
					if(martialart)msg_print(_("あなたはモンスターの上に立ち上がった。モンスターはあなたの意のままに動いた！",
                                            "You stand up on top of the monster. The monster is still following your orders!"));
					else msg_print(_("あなたはモンスターから手を離した。モンスターはあなたの意のままに動いた！",
                                    "You let go of the monster. The monster is still following your orders!"));
				}

				else if(ref_skill_exp(SKILL_RIDING) >= RIDING_STOP)
					msg_print(_("あなたはモンスターから手を離し、なだめてその場に留まらせた。",
                                "You let go of the monster, and tell it to stay in place."));
				else
					msg_print(_("あなたはモンスターから手を離した。モンスターは勝手に動き始めた・・",
                                "You let go of the monster. The monster starts moving around..."));

			}
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
			break;
		}
	}
}

///mod140208 magic_typeを種族や職業に合わせて計算する関数
void calc_spell_info(magic_type *s_ptr, int realm, int spell)
{
	magic_type table;
	int lev;
	int c_apt;

	if(flag_spell_consume_book)
	{
		table.slevel = 0;
		table.smana = 0;
		table.sfail = 0;
		table.sexp  = 0;
		(void)COPY(s_ptr, &table, magic_type);
		return;
	}
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && realm <= MAX_MAGIC)
	{
		//v1.1.32 書痴パチュリーは本来適正のない仙術や妖術も使う
	}
	else if(cp_ptr->realm_aptitude[realm] == 0)
	{
		table.slevel = 99;
		table.smana = 0;
		table.sfail = 0;
		table.sexp  = 0;
		(void)COPY(s_ptr, &table, magic_type);
		return;
	}

	//職業による適性から基礎情報を得る。霊夢は賽銭によるランク上昇有り
	c_apt = cp_ptr->realm_aptitude[realm]-1;

	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && realm <= MAX_MAGIC)
	{
		c_apt = 4;
	}
	if(p_ptr->pclass == CLASS_REIMU)
	{
		if(osaisen_rank() == 9) c_apt += 2;
		else if(osaisen_rank() > 4) c_apt += 1;
		if(c_apt > 5) c_apt = 5;
	}
	if(realm == TV_BOOK_OCCULT)
	{
		lev         = occult_spell_base_info[spell][c_apt].slevel;
		table.smana = occult_spell_base_info[spell][c_apt].smana;
		table.sfail = occult_spell_base_info[spell][c_apt].sfail;
		table.sexp  = occult_spell_base_info[spell][c_apt].sexp;
	}
	else
	{
		lev         = spell_base_info[realm-1][spell][c_apt].slevel;
		table.smana = spell_base_info[realm-1][spell][c_apt].smana;
		table.sfail = spell_base_info[realm-1][spell][c_apt].sfail;
		table.sexp  = spell_base_info[realm-1][spell][c_apt].sexp;
	}

	/*:::一部の職業は基本テーブルから数値を変更する*/
	/*:::影狼は爪強化や変身が得意*/
	if(p_ptr->pclass == CLASS_KAGEROU)
	{
		if(realm == TV_BOOK_TRANSFORM && spell == 12)
		{
			lev = 22;
			table.smana = 30;
			table.sfail = 50;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 22)
		{
			lev = 32;
			table.smana = 40;
			table.sfail = 60;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 27)
		{
			lev = 40;
			table.smana = 80;
			table.sfail = 70;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 20)
		{
			lev = 35;
			table.smana = 45;
			table.sfail = 80;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 31)
		{
			lev = 50;
			table.smana = 100;
			table.sfail = 80;
		}
	}
	else if(p_ptr->pclass == CLASS_MAMIZOU)
	{
		if(realm == TV_BOOK_TRANSFORM && spell == 12)
		{
			lev = 22;
			table.smana = 30;
			table.sfail = 50;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 18)
		{
			lev = 25;
			table.smana = 35;
			table.sfail = 60;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 22)
		{
			lev = 35;
			table.smana = 40;
			table.sfail = 60;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 24)
		{
			lev = 24;
			table.smana = 50;
			table.sfail = 60;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 27)
		{
			lev = 37;
			table.smana = 80;
			table.sfail = 70;
		}
		else if(realm == TV_BOOK_TRANSFORM && spell == 30)
		{
			lev = 45;
			table.smana = 100;
			table.sfail = 80;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 20)
		{
			lev = 30;
			table.smana = 40;
			table.sfail = 75;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 31)
		{
			lev = 47;
			table.smana = 90;
			table.sfail = 80;
		}


	}
	//アリスは魔法剣などの魔法の難易度が高い
	else if(p_ptr->pclass == CLASS_ALICE)
	{
		if(realm == TV_BOOK_ENCHANT && spell == 22) //魔法剣
		{
			lev = 47;
			table.smana = 120;
			table.sfail = 80;
		}
		else if(realm == TV_BOOK_ENCHANT && spell == 27) //匠の技
		{
			lev = 99;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 9) //すずらんの毒
		{
			lev = 30;
			table.smana = 48;
			table.sfail = 80;
		}
		else if(realm == TV_BOOK_DARKNESS && spell == 26) //魔剣
		{
			lev = 50;
			table.smana = 180;
			table.sfail = 90;
		}
	}

	/*:::魔法レベルを種族適正値で調整する　変身時はどう処理しよう？*/
	///pend 種族変容した時は使えなくなった呪文を未習得に戻さないといけない？何もしなくても忘却として処理されるか？
	if(lev != 99)
	{
		///mod140509 第二領域の魔法が第一領域と合わないとき、レベルとコストに大幅ペナルティ
		///mod150901 魔理沙はペナルティ軽い
		if(p_ptr->realm1 != REALM_NONE && !same_category_realm(p_ptr->realm1,realm))
		{
			if(p_ptr->pclass == CLASS_MARISA) lev += 3;
			else lev += 6;

			table.smana = table.smana * 120 / 100;
		}
		//それ以外の第二領域は習得レベルが3上昇
		else if(p_ptr->realm1 != REALM_NONE && realm != p_ptr->realm1 && p_ptr->pclass != CLASS_MARISA) lev += 3;


		/*:::呪文に向かないグローブを装備しているとき失敗率にペナルティ　器用さでペナルティ軽減*/
		if(p_ptr->cumber_glove || p_ptr->clawextend)	table.sfail += MAX(0,(30-adj_general[p_ptr->stat_ind[A_DEX]])/2);
		/*:::装備が重いときコストにペナルティ*/
		table.smana += table.smana * p_ptr->heavy_cast / 100;


		//適性によりレベルと失敗率を増減
		if(rp_ptr->realm_aptitude[realm] == 9 || rp_ptr->realm_aptitude[realm] == 8) lev -= 2;
		if(rp_ptr->realm_aptitude[realm] == 7 || rp_ptr->realm_aptitude[realm] == 6) lev -= 1;
		if(rp_ptr->realm_aptitude[realm] == 4 || rp_ptr->realm_aptitude[realm] == 3) lev += 1;
		if(rp_ptr->realm_aptitude[realm] == 2 || rp_ptr->realm_aptitude[realm] == 1) lev += 2;
		table.sfail -= (rp_ptr->realm_aptitude[realm] - 5) * 3;

		//一部の野良神様は特定領域にボーナス
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			//大地神　自然にボーナス
			if(p_ptr->race_multipur_val[3] == 43 && realm == TV_BOOK_NATURE)
			{
				lev -= 3;
				table.sfail -= 5;
			}
			//天帝　妖術以外にボーナス
			if(p_ptr->race_multipur_val[3] == 45 && realm < TV_BOOK_TRANSFORM)
			{
				lev -= 2;
				table.sfail -= 5;
			}
			//叡智の神　全てにボーナス
			if(p_ptr->race_multipur_val[3] == 50)
			{
				lev -= 2;
				table.sfail -= 3;
			}
			//風神・雷神　自然にボーナス
			if((p_ptr->race_multipur_val[3] == 54 || p_ptr->race_multipur_val[3] == 55) && realm == TV_BOOK_NATURE)
			{
				lev -= 2;
				table.sfail -= 3;
			}
			//混沌の神　混沌に大幅ボーナス
			if(p_ptr->race_multipur_val[3] == 58 && realm == TV_BOOK_CHAOS)
			{
				lev -= 5;
				table.sfail -= 12;
			}
			//魔神・邪神　暗黒にボーナス
			if((p_ptr->race_multipur_val[3] == 59 || p_ptr->race_multipur_val[3] == 62)  && realm == TV_BOOK_DARKNESS)
			{
				lev -= 2;
				table.sfail -= 5;
			}
			//死者の神　死霊に大幅ボーナス
			if(p_ptr->race_multipur_val[3] == 61 && realm == TV_BOOK_NECROMANCY)
			{
				lev -= 4;
				table.sfail -= 10;
			}

		}

		if(lev<1) lev = 1;
		if(lev>50) lev = 50;
		if(rp_ptr->realm_aptitude[realm] == 0) lev = 99;//適性0の領域の呪文は習得できない
	}
	table.slevel = lev;
	///mod140814 高速詠唱中は消費MPが増える
	if(quick_cast) table.smana += table.smana * 3 / 10;

	//v2.0.1 アビリティカード「魔法使いの基礎勉強」による失敗率低下
	{
		int abl_card_num;
		abl_card_num = count_ability_card(ABL_CARD_PATHE_STUDY);

		if (abl_card_num)
		{
			int bonus = calc_ability_card_prob(ABL_CARD_PATHE_STUDY, abl_card_num);

			table.sfail -= bonus;
		}
	}

	(void)COPY(s_ptr, &table, magic_type);
	return;
}


