/* File: bldg.c */

/*
 * Purpose: Building commands
 * Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
 * -KMW-
 *
 * Rewritten for Kangband 2.8.3i using Kamband's version of
 * bldg.c as written by Ivan Tkatchev
 *
 * Changed for ZAngband by Robert Ruehlmann
 */
/*:::町の建物関連　大改造予定*/


#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

/*:::種族、職業、領域が一致してるかどうか判定？詳細不明　削除予定*/
static bool is_owner(building_type *bldg)
{

	//現状全てのプレイヤーをメンバー扱いとしている
	return TRUE;
/*
	if (bldg->member_class[p_ptr->pclass] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && (bldg->member_realm[p_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(p_ptr->realm2) && (bldg->member_realm[p_ptr->realm2] == BUILDING_OWNER)))
	{
		return (TRUE);
	}

	return (FALSE);
*/
}

/*:::種族、職業、領域が一致してるかどうか判定？　削除予定*/
/*:::だが食堂で食事可能かどうかもここで判定されているらしい*/
static bool is_member(building_type *bldg)
{
	return (TRUE);
/*
	if (bldg->member_class[p_ptr->pclass])
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace])
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && bldg->member_realm[p_ptr->realm1]) ||
	    (is_magic(p_ptr->realm2) && bldg->member_realm[p_ptr->realm2]))
	{
		return (TRUE);
	}


	if (p_ptr->pclass == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (bldg->member_realm[i+1]) OK = TRUE;
		}
		return OK;
	}
	return (FALSE);
	*/
}


/*
 * Clear the building information
 */
 /*:::施設の中にいるときのための指定範囲画面クリア min_row～max_rowの全ての行を行末まで空白にする*/
void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}
/*:::手持ちの金額を表示（一般建物用）*/
///mod151114 bldg2.cでも使うようにexternした
void building_prt_gold(void)
{
	char tmp_str[80];

#ifdef JP
prt("手持ちのお金: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)p_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
/*:::建物で選べる行動一覧表示。種族職業ごとのコストの違いや利用可否など区別*/
///sys 建物関係
///mod160306 外からも参照できるようにした
void show_building(building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color;
	char tmp_str[80];

	Term_clear();
	//sprintf(tmp_str, "%s (%s) %35s", bldg->owner_name, bldg->owner_race, bldg->name);
	///mod140117 建物表記からオーナー種族を削除しオーナー名と建物名を逆にした

	//v1.1.85 深夜に鯢呑亭に入ったら店名が変わる
	if (p_ptr->town_num == TOWN_HITOZATO && f_info[cave[py][px].feat].subtype == 12 && is_midnight())
	{
		sprintf(tmp_str, _("【蚕喰】鯢呑亭  %35s", " [Silkmoth] Geidontei %35s"), bldg->owner_name);
	}
	else
	{
		sprintf(tmp_str, "%s  %35s", bldg->name, bldg->owner_name);
	}
	prt(tmp_str, 2, 1);


	for (i = 0; i < 8; i++)
	{
		if (bldg->letters[i])
		{
			/*:::誰でも利用できる設備*/
			if (bldg->action_restr[i] == 0)
			{
				/*:::無料のとき*/
				if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				/*:::有料/メンバー*/
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				/*:::有料/メンバー以外*/
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->other_costs[i]);
#endif

				}
			}
			/*:::restr=1のとき、メンバーしか利用できない。宿の食事など*/
			else if (bldg->action_restr[i] == 1)
			{
				if (!is_member(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(閉店)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(is_member(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->other_costs[i]);
#endif

				}
			}
			else
			{
				if (!is_owner(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(閉店)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if (bldg->member_costs[i] != 0)
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
			}

			sprintf(tmp_str," %c) %s %s", bldg->letters[i], bldg->act_names[i], buff);
#ifdef JP
			c_put_str(action_color, tmp_str, 19+(i/2), 35*(i%2));
#else
            c_put_str(action_color, tmp_str, 19+(i/2), 42*(i%2));
#endif
		}
	}

#ifdef JP
prt(" ESC) 建物を出る", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
/*:::アリーナ処理　*/
/*:::アリーナに入るときはいくつかのフラグ立てた上で階から出る操作*/
//v1.1.51でnightmare_diary()に新アリーナ作った。ここはもう使わないのでコメントアウトする。
#if 0
static void arena_comm(int cmd)
{
	monster_race    *r_ptr;
	cptr            name;


	switch (cmd)
	{
		case BACT_ARENA:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
prt("アリーナの優勝者！", 5, 0);
prt("おめでとう！あなたは全ての敵を倒しました。", 7, 0);
prt("賞金として $1,000,000 が与えられます。", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				p_ptr->au += 1000000L;
#ifdef JP
msg_print("スペースキーで続行");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				p_ptr->arena_number++;
			}
			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
				if (p_ptr->arena_number < MAX_ARENA_MONS+2)
				{
#ifdef JP
msg_print("君のために最強の挑戦者を用意しておいた。");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("挑戦するかね？"))
#else
					if (get_check("Do you fight? "))
#endif
					{
#ifdef JP
                        msg_print("死ぬがよい。");
#else
					    msg_print("Die, maggots.");
#endif
						msg_print(NULL);

						p_ptr->exit_bldg = FALSE;
						reset_tim_flags();

						/* Save the surface floor as saved floor */
						prepare_change_floor_mode(CFM_SAVE_FLOORS);

						p_ptr->inside_arena = TRUE;
						p_ptr->leaving = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
msg_print("残念だ。");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
msg_print("あなたはアリーナに入り、しばらくの間栄光にひたった。");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			///class building 魔獣使いと騎兵以外は乗馬のままアリーナに入れない
			else if (p_ptr->riding && (p_ptr->pclass != CLASS_BEASTMASTER) && (p_ptr->pclass != CLASS_CAVALRY))
			{
#ifdef JP
msg_print("ペットに乗ったままではアリーナへ入れさせてもらえなかった。");
#else
				msg_print("You don't have permission to enter with pet.");
#endif

				msg_print(NULL);
			}
			else
			{
				p_ptr->exit_bldg = FALSE;
				reset_tim_flags();

				/* Save the surface floor as saved floor */
				prepare_change_floor_mode(CFM_SAVE_FLOORS);

				p_ptr->inside_arena = TRUE;
				p_ptr->leaving = TRUE;
				leave_bldg = TRUE;
			}
			break;
		case BACT_POSTER:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
#ifdef JP
msg_print("あなたは勝利者だ。 アリーナでのセレモニーに参加しなさい。");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
msg_print("あなたはすべての敵に勝利した。");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &r_info[arena_info[p_ptr->arena_number].r_idx];
				name = (r_name + r_ptr->name);
#ifdef JP
msg_format("%s に挑戦するものはいないか？", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
///del131213 building アリーナのルール表示 使われてないらしい
#if 0
		case BACT_ARENA_RULES:

			/* Save screen */
			screen_save();

			/* Peruse the arena help file */
#ifdef JP
(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif


			/* Load screen */
			screen_load();

			break;
#endif
	}
}
#endif


/*
 * display fruit for dice slots
 */
static void display_fruit(int row, int col, int fruit)
{
	switch (fruit)
	{
		case 0: /* lemon */
#ifdef JP
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " レモン ", row + 8, col);
#else
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " Lemon  ", row + 8, col);
#endif

			break;
		case 1: /* orange */
#ifdef JP
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   "オレンジ", row + 8, col);
#else
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   " Orange ", row + 8, col);
#endif

			break;
		case 2: /* sword */
#ifdef JP
			c_put_str(TERM_SLATE, "   Λ   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=亜=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   目   " , row + 7, col);
			prt(                  "   剣   " , row + 8, col);
#else
			c_put_str(TERM_SLATE, "   /\\   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			prt(                  " Sword  " , row + 8, col);
#endif

			break;
		case 3: /* shield */
#ifdef JP
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  "   盾   ", row + 8, col);
#else
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  " Shield ", row + 8, col);
#endif

			break;
		case 4: /* plum */
#ifdef JP
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   " プラム ", row + 8, col);
#else
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   "  Plum  ", row + 8, col);
#endif

			break;
		case 5: /* cherry */
#ifdef JP
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                "チェリー", row + 8, col);
#else
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                " Cherry ", row + 8, col);
#endif

			break;
	}
}

/*
 * kpoker no (tyuto-hannpa na)pakuri desu...
 * joker ha shineru node haitte masen.
 *
 * TODO: donataka! tsukutte!
 *  - agatta yaku no kiroku (like DQ).
 *  - kakkoii card no e.
 *  - sousa-sei no koujyo.
 *  - code wo wakariyasuku.
 *  - double up.
 *  - Joker... -- done.
 *
 * 9/13/2000 --Koka
 * 9/15/2000 joker wo jissou. soreto, code wo sukosi kakikae. --Habu
 */
#define SUIT_OF(card)  ((card) / 13)
#define NUM_OF(card)   ((card) % 13)
#define IS_JOKER(card) ((card) == 52)

static int cards[5]; /* tefuda no card */

static void reset_deck(int deck[])
{
	int i;
	for (i = 0; i < 53; i++) deck[i] = i;

	/* shuffle cards */
	for (i = 0; i < 53; i++){
		int tmp1 = randint0(53 - i) + i;
		int tmp2 = deck[i];
		deck[i] = deck[tmp1];
		deck[tmp1] = tmp2;
	}
}

static bool have_joker(void)
{
	int i;

	for (i = 0; i < 5; i++){
	  if(IS_JOKER(cards[i])) return TRUE;
	}
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++)
		if (NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++){
		if (SUIT_OF(cards[i]) != suit){
		  if(have_joker() && !joker_is_used)
		    joker_is_used = TRUE;
		  else
		    return FALSE;
		}
	}

	return TRUE;
}

static int yaku_check_straight(void)
{
	int i, lowest = 99;
	bool joker_is_used = FALSE;

	/* get lowest */
	for (i = 0; i < 5; i++)
	{
		if (NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}

	if (yaku_check_flush())
	{
	  if( lowest == 0 ){
		for (i = 0; i < 4; i++)
		{
			if (!find_card_num(9 + i)){
				if( have_joker() && !joker_is_used )
				  joker_is_used = TRUE;
				else
				  break;
			}
		}
		if (i == 4) return 3; /* Wow! Royal Flush!!! */
	  }
	  if( lowest == 9 ){
		for (i = 0; i < 3; i++)
		{
			if (!find_card_num(10 + i))
				break;
		}
		if (i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
	  }
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if (!find_card_num(lowest + i)){
		  if( have_joker() && !joker_is_used )
		    joker_is_used = TRUE;
		  else
		    return 0;
		}
	}

	if (yaku_check_flush())
		return 2; /* Straight Flush */

	return 1;
}

/*
 * 0:nopair 1:1 pair 2:2 pair 3:3 cards 4:full house 6:4cards
 */
static int yaku_check_pair(void)
{
	int i, i2, matching = 0;

	for (i = 0; i < 5; i++)
	{
		for (i2 = i+1; i2 < 5; i2++)
		{
			if (IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if (NUM_OF(cards[i]) == NUM_OF(cards[i2]))
				matching++;
		}
	}

	if(have_joker()){
	  switch(matching){
	  case 0:
	    matching = 1;
	    break;
	  case 1:
	    matching = 3;
	    break;
	  case 2:
	    matching = 4;
	    break;
	  case 3:
	    matching = 6;
	    break;
	  case 6:
	    matching = 7;
	    break;
	  default:
	    /* don't reach */
	    break;
	  }
	}

	return matching;
}

#define ODDS_5A 3000
#define ODDS_5C 400
#define ODDS_RF 200
#define ODDS_SF 80
#define ODDS_4C 16
#define ODDS_FH 12
#define ODDS_FL 8
#define ODDS_ST 4
#define ODDS_3C 1
#define ODDS_2P 1

static int yaku_check(void)
{
	prt("                            ", 4, 3);

	switch(yaku_check_straight()){
	case 3: /* RF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "ロイヤルストレートフラッシュ",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレートフラッシュ",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレート",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight",  4,  3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if (yaku_check_flush())
	{

#ifdef JP
	c_put_str(TERM_YELLOW, "フラッシュ",  4,  3);
#else
	c_put_str(TERM_YELLOW, "Flush",  4,  3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ワンペア",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "ツーペア",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "スリーカード",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "フルハウス",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "フォーカード",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブエース",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブカード",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind",  4,  3);
#endif
			return ODDS_5C;
		}
	default:
		break;
	}
	return 0;
}

static void display_kaeruka(int hoge, int kaeruka[])
{
	int i;
	char col = TERM_WHITE;
	for (i = 0; i < 5; i++)
	{
		if (i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "かえる", 14,  5+i*16);
		else
			c_put_str(col, "のこす", 14,  5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14,  5+i*16);
		else
			c_put_str(col, " Stay ", 14,  5+i*16);
#endif
	}
	if (hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "決定", 16,  38);
#else
	c_put_str(col, "Sure", 16,  38);
#endif

	/* Hilite current option */
	if (hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"★", "●", "¶", "†"};
	cptr card_grph[13][7] = {{"Ａ   %s     ",
				  "     変     ",
				  "     愚     ",
				  "     蛮     ",
				  "     怒     ",
				  "     %s     ",
				  "          Ａ"},
				 {"２          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          ２"},
				 {"３          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          ３"},
				 {"４          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          ４"},
				 {"５          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          ５"},
				 {"６          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ６"},
				 {"７          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ７"},
				 {"８          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          ８"},
				 {"９ %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s ９"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"Ｊ   Λ     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=亜=| %s",
				  "     目   Ｊ"},
				 {"Ｑ ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   Ｑ"},
				 {"Ｋ          ",
				  "%s ｀⌒´   ",
				  "  γγγλ  ",
				  "  ο ο ι  ",
				  "   υ    ∂ ",
				  "    σ ノ %s",
				  "          Ｋ"}};
	cptr joker_grph[7] = {    "            ",
				  "     Ｊ     ",
				  "     Ｏ     ",
				  "     Ｋ     ",
				  "     Ｅ     ",
				  "     Ｒ     ",
				  "            "};

#else

	cptr suit[4] = {"[]", "qp", "<>", "db"};
	cptr card_grph[13][7] = {{"A    %s     ",
				  "     He     ",
				  "     ng     ",
				  "     ba     ",
				  "     nd     ",
				  "     %s     ",
				  "           A"},
				 {"2           ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           2"},
				 {"3           ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "           3"},
				 {"4           ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "           4"},
				 {"5           ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "           5"},
				 {"6           ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           6"},
				 {"7           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           7"},
				 {"8           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "           8"},
				 {"9  %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s  9"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"J    /\\     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=HH=| %s",
				  "     ][    J"},
				 {"Q  ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##    Q"},
				 {"K           ",
				  "%s _'~~`_   ",
				  "   jjjjj$&  ",
				  "   q q uu   ",
				  "   c    &   ",
				  "    v__/  %s",
				  "           K"}};
	cptr joker_grph[7] = {    "            ",
				  "     J      ",
				  "     O      ",
				  "     K      ",
				  "     E      ",
				  "     R      ",
				  "            "};
#endif

	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("┏━━━━━━┓",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("┃",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("┃",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("┗━━━━━━┛", 13,  i*16);
#else
		prt(" +------------+ ", 13,  i*16);
#endif
	}
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int deck_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[deck_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}

#if 0
	/* debug:RF */
	cards[0] = 12;
	cards[1] = 0;
	cards[2] = 9;
	cards[3] = 11;
	cards[4] = 10;
#endif
#if 0
	/* debug:SF */
	cards[0] = 3;
	cards[1] = 2;
	cards[2] = 4;
	cards[3] = 6;
	cards[4] = 5;
#endif
#if 0
	/* debug:Four Cards */
	cards[0] = 0;
	cards[1] = 0 + 13 * 1;
	cards[2] = 0 + 13 * 2;
	cards[3] = 0 + 13 * 3;
	cards[4] = 51;
#endif
#if 0
	/* debug:Straight */
	cards[0] = 1;
	cards[1] = 0 + 13;
	cards[2] = 3;
	cards[3] = 2 + 26;
	cards[4] = 4;
#endif
#if 0
	/* debug */
	cards[0] = 52;
	cards[1] = 0;
	cards[2] = 1;
	cards[3] = 2;
	cards[4] = 3;
#endif

	/* suteruno wo kimeru */
#ifdef JP
	prt("残すカードを決めて下さい(方向で移動, スペースで選択)。", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if (kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if (!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if (!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if (!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if (kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if (kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}

	prt("",0,0);

	for (i = 0; i < 5; i++)
		if (kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();

	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
/* end of poker codes --Koka */

/*
 * gamble_comm
 */
#if 0
static bool gamble_comm(int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if (cmd == BACT_GAMBLE_RULES)
	{
		/* Peruse the gambling help file */
#ifdef JP
(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if (p_ptr->au < 1)
		{
#ifdef JP
			msg_print("おい！おまえ一文なしじゃないか！こっから出ていけ！");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"賭け金 (1-%ld)？", (long int)maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", (long int)maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("おい！金が足りないじゃないか！出ていけ！");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ldゴールドだけ受けよう。残りは取っときな。", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", (long int)maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("ＯＫ、１ゴールドからはじめよう。");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = p_ptr->au;

#ifdef JP
sprintf(tmp_str, "ゲーム前の所持金: %9ld", (long int)oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", (long int)oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
sprintf(tmp_str, "現在の掛け金:     %9ld", (long int)wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", (long int)wager);
#endif

			prt(tmp_str, 21, 2);

			do
			{
				p_ptr->au -= wager;
				switch (cmd)
				{
				 case BACT_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
c_put_str(TERM_GREEN, "イン・ビトイーン",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
sprintf(tmp_str, "黒ダイス: %d        黒ダイス: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
sprintf(tmp_str, "赤ダイス: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if (((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BACT_CRAPS:  /* Game of Craps */
#ifdef JP
c_put_str(TERM_GREEN, "クラップス", 5, 2);
#else
					c_put_str(TERM_GREEN, "Craps", 5, 2);
#endif

					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;
#ifdef JP
sprintf(tmp_str, "１振りめ: %d %d      Total: %d", roll1,
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if ((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if ((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
msg_print("なにかキーを押すともう一回振ります。");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
sprintf(tmp_str, "出目: %d %d          合計:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if (roll3 == choice)
								win = TRUE;
							else if (roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BACT_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
c_put_str(TERM_GREEN, "ルーレット", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
get_string("何番？ (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif
/*:::///patch131222本家rev3510の修正を適用？*/
#ifdef PATCH_ORIGIN
					for (p = out_val; iswspace(*p); p++);
#else
					for (p = out_val; isspace(*p); p++);
#endif
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
msg_print("0番にしとくぜ。");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
msg_print("ＯＫ、9番にしとくぜ。");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
sprintf(tmp_str, "ルーレットは回り、止まった。勝者は %d番だ。",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if (roll1 == choice)
						win = TRUE;
					break;

				case BACT_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
c_put_str(TERM_GREEN, "ダイス・スロット", 5, 2);
					c_put_str(TERM_YELLOW, "レモン   レモン            2", 6, 37);
					c_put_str(TERM_YELLOW, "レモン   レモン   レモン   5", 7, 37);
					c_put_str(TERM_ORANGE, "オレンジ オレンジ オレンジ 10", 8, 37);
					c_put_str(TERM_UMBER, "剣       剣       剣       20", 9, 37);
					c_put_str(TERM_SLATE, "盾       盾       盾       50", 10, 37);
					c_put_str(TERM_VIOLET, "プラム   プラム   プラム   200", 11, 37);
					c_put_str(TERM_RED, "チェリー チェリー チェリー 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8,  3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if ((roll1 == roll2) && (roll2 == choice))
					{
						win = TRUE;
						switch(roll1)
						{
						case 1:
							odds = 5;break;
						case 2:
							odds = 10;break;
						case 3:
							odds = 20;break;
						case 4:
							odds = 50;break;
						case 5:
							odds = 200;break;
						case 6:
							odds = 1000;break;
						}
					}
					else if ((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BACT_POKER:
					win = FALSE;
					odds = do_poker();
					if (odds) win = TRUE;
					break;
				}

				if (win)
				{
#ifdef JP
prt("あなたの勝ち", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					p_ptr->au += odds * wager;
#ifdef JP
sprintf(tmp_str, "倍率: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
prt("あなたの負け", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
sprintf(tmp_str, "現在の所持金:     %9ld", (long int)p_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", (long int)p_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
prt("もう一度(Y/N)？", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if (wager > p_ptr->au)
				{
#ifdef JP
msg_print("おい！金が足りないじゃないか！ここから出て行け！");
#else
					msg_print("Hey! You don't have the gold - get out of here!");
#endif

					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			prt("", 18, 37);
			if (p_ptr->au >= oldgold)
			{
#ifdef JP
msg_print("「今回は儲けたな！でも次はこっちが勝ってやるからな、絶対に！」");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
				chg_virtue(V_CHANCE, 3);
			}
			else
			{
#ifdef JP
msg_print("「金をスッてしまったな、わはは！うちに帰った方がいいぜ。」");
#else
				msg_print("You lost gold! Haha, better head home.");
#endif
				chg_virtue(V_CHANCE, -3);
			}
		}
		msg_print(NULL);
	}
	screen_load();
	return (TRUE);
}
#endif

///del 闘技場関連
#if 0
static bool vault_aux_battle(int r_idx)
{
	int i;
	int dam = 0;

	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
/*	if (!mon_hook_dungeon(r_idx)) return FALSE; */

	/* Decline unique monsters */
/*	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE); */
/*	if (r_ptr->flags7 & (RF7_NAZGUL)) return (FALSE); */

	if (r_ptr->flags1 & (RF1_NEVER_MOVE)) return (FALSE);
	if (r_ptr->flags2 & (RF2_MULTIPLY)) return (FALSE);
	if (r_ptr->flags2 & (RF2_QUANTUM)) return (FALSE);
	if (r_ptr->flags7 & (RF7_AQUATIC)) return (FALSE);
	if (r_ptr->flags7 & (RF7_CHAMELEON)) return (FALSE);

	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE) return (FALSE);
		if (r_ptr->blow[i].effect != RBE_DR_MANA) dam += r_ptr->blow[i].d_dice;
	}
	if (!dam && !(r_ptr->flags4 & (RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK | RF4_BREATH_MASK)) && !(r_ptr->flags5 & (RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_BREATH_MASK)) && !(r_ptr->flags6 & (RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK | RF6_BREATH_MASK))) return (FALSE);

	/* Okay */
	return (TRUE);
}
#endif

///mod160305 雛人形無人販売所
static void buy_hinaningyou()
{
	char c;
	int price = 100;
	bool flag = FALSE;

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_print(_("雛人形の売れ行きはどうだろうか？", "You wonder how well the hina dolls are selling."));
		return;
	}

	clear_bldg(4,18);

	prt(format(_("雛人形の無人販売所だ。粗末な紙人形($%d)が売れ残っている。",
                "It's an unmanned hina doll store. There are some unsold poor paper dolls ($%d)."),price), 5,10);

	if (!get_check_strict(_("買いますか？", "Purchase?"), CHECK_DEFAULT_Y))
	{
		clear_bldg(4,18);
		return;
	}
	clear_bldg(4,18);
	if(p_ptr->au < price)
	{
		msg_print(_("お金が足りない。", "You don't have enough money."));
		return;
	}

	msg_print(_("雛人形を手に取った・・", "You purchase a hina doll..."));
	if(restore_level()) flag = TRUE;
	else if(remove_curse())
	{
		msg_print(_("体が軽くなった気がする。", "You feel your body become lighter."));
		flag = TRUE;
	}

	if(!flag) msg_print(_("特に何も起こらなかった。", "Nothing special happens."));
	msg_print(_("人形は崩れて消えた。", "The doll breaks and vanishes."));

	p_ptr->au -= price;
	building_prt_gold();

}


//v2.0.3 アビリティカード売上げ記録変数に加算
//性格いかさまやチートオプションを弾く処理を忘れていた。今更ながら追加するために別関数に分離。
static void add_cardshop_profit(int price)
{

	if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return;

	if (total_pay_cardshop < 100000000) total_pay_cardshop += price;


}


//v1.1.87 山如の賭場の10連ガチャ
static void buy_gacha_box()
{
	int price;
	object_type forge;
	object_type *o_ptr = &forge;
	char o_name[MAX_NLEN];

	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		msg_print(_("「おい入るな。奥には何もないぞ。」",
                    "'Hey, don't go in. There's nothing in here.'"));

		return;
	}


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print(_("「おっと、この奥はカードの売人専用だ。」",
                    "'Sorry, card traders only.'"));
		return;

	}

	//ザックに空きがないとき
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		if(p_ptr->pclass == CLASS_SANNYO)
			msg_print(_("ザックに空きがない。",
                        "You don't have free space in your inventory."));
		else
			msg_print(_("「ふん、ザックに空きを作ってからまた来な。」",
                        "'Hmph, come back once you've freed up some space in your backpack.'"));
		return;
	}

	//ガチャの価格は高騰度*1000 上限10万
	//出てくるカードの期待値は高騰度*1100～1200くらいなので多少は割に合う
	price = p_ptr->abilitycard_price_rate * 1000;
	if (price > 100000) price = 100000;

	clear_bldg(4, 18);

	if (p_ptr->pclass == CLASS_SANNYO)
	{
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt(_("たかね「あんたが帰るまでは私達がここで商売をさせてもらおう。",
                "Takane - 'Leave trading to us while you're away."), 5, 10);
			prt(format(_("　　　さて、今なら初回大サービスで一箱$%dだよ！",
                        "          By the way, it's $%d per box for your first purchase!"), price), 6, 10);
			prt(_("　　　 ", "       "), 7, 10);
			prt(_("　　　え？もちろん金を取るに決まってるじゃないか！」",
                "          Eh? Of course I'm going to charge money!'"), 8, 10);
		}
		else
		{
			prt(_("たかね「いらっしゃい！",
                "Takane - 'Welcome!"), 5, 10);
			prt(format(_("　　　そうだねえ。今の相場なら一箱$%dって所かな。」",
                        "          That's right. At current market price, one box costs $%d.'"), price), 6, 10);
		}
	}
	else
	{
		prt(_("山如「よく来たな。実はカードを使った面白い賭けを始めたところだ。",
            "Sannyo - 'Ah, welcome. I've started running an interesting game of chance."), 5, 10);
		prt(_("　　　この虹色の箱にはアビリティカードが10枚入っている。",
            "          This rainbow-colored box contains 10 ability cards."), 6, 10);
		prt(format(_("　　　こいつを一箱＄%dで売ってやろう。",
                    "          It sells for $%d."), price), 7, 10);
		prt(_("　　　運良くレアなカードを引ければ大儲けってわけだ。",
            "          If you're lucky, you might obtain rare cards and make lots of money."), 8, 10);
		prt(_("　　　無論、箱の中身にこっちが手を加えたりなどしちゃいない。」",
            "          Naturally, I haven't touched the contents of the box.'"), 9, 10);
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt(_("　　　 ", "       "), 10, 10);
			prt(format(_("　　「そしてお前は幸運だ。初回だけは大サービスの＄%dにしてやる。」",
                        "          'You're in luck. For your first purchase, it's going to cost only $%d.'"), price), 11, 10);
		}
	}



	if (!get_check_strict(_("買いますか？", "Do you want to buy it?"), CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}
	clear_bldg(4, 18);
	if (p_ptr->au < price)
	{
		msg_print(_("お金が足りない。", "You don't have enough money."));
		return;
	}


	if (p_ptr->pclass == CLASS_SANNYO)
	{
		prt(_("あなたは箱を購入した。", "You purchase a box."), 5, 10);
	}
	else if (!buy_gacha_box_count)
	{
		prt(_("山如「今後ともご贔屓にな。ふっふっふ……」",
            "Sannyo - 'I'll be pleased to work with you in the future. Fufufu...'"), 5, 10);
	}
	else
	{
		if (buy_gacha_box_count > 10 && one_in_(4))prt(_("山如「目が血走っているな。一服どうだ？」",
                                                        "Sannyo - 'Your eyes are all bloodshot. How about taking a break?'"), 5, 10);
		else if(one_in_(10))	prt(_("山如「この商売は山の上のほうの連中に睨まれるかもしれん。早めに買ったほうがいいぞ？」",
                                    "Sannyo - 'The higher-ups on the mountain might take displeasure with this business. Can you buy faster?'"), 5, 10);
		else if(one_in_(7))		prt(_("山如「ダンジョンの奥で開けたらレアカードが出やすい？そんな訳ないだろうが。」",
                                        "Sannyo - 'Is it easier to get rare cards if you open the box deep in the dungeon? No, I don't think so.'"), 5, 10);
		else if(one_in_(3))		prt(_("山如「いいカードが入っているといいな？」",
                                        "Sannyo - 'Are you looking to get some good cards?'"), 5, 10);
		else 					prt(_("山如「よし、毎度あり。」", "Sannyo - 'Please come again.'"), 5, 10);

	}
	buy_gacha_box_count++;
	//カードの合計支払額に加算
	//if(total_pay_cardshop < 100000000) total_pay_cardshop += price;
	add_cardshop_profit(price);


	object_prep(o_ptr, lookup_kind(TV_CHEST, SV_CHEST_10_GACHA));
	o_ptr->pval = -6; //(非施錠の箱 )

	object_desc(o_name, o_ptr, 0);

	inven_carry(o_ptr);
	msg_format(_("%sを受け取った。", "You receive %s."), o_name);

	p_ptr->au -= price;
	building_prt_gold();

}


/*:::霧雨魔法店ポイント交換対象品目テーブル*/
struct marisa_store_type marisa_wants_table[] =
{
	{TV_BOOK_ELEMENT,2,30},
	{TV_BOOK_ELEMENT,3,300},
	{TV_BOOK_FORESEE,2,30},
	{TV_BOOK_FORESEE,3,300},
	{TV_BOOK_ENCHANT,2,30},
	{TV_BOOK_ENCHANT,3,300},
	{TV_BOOK_SUMMONS,2,30},
	{TV_BOOK_SUMMONS,3,300},
	{TV_BOOK_MYSTIC,2,30},
	{TV_BOOK_MYSTIC,3,300},
	{TV_BOOK_LIFE,2,30},
	{TV_BOOK_LIFE,3,300},
	{TV_BOOK_PUNISH,2,30},
	{TV_BOOK_PUNISH,3,300},
	{TV_BOOK_NATURE,2,30},
	{TV_BOOK_NATURE,3,300},
	{TV_BOOK_TRANSFORM,2,30},
	{TV_BOOK_TRANSFORM,3,300},
	{TV_BOOK_DARKNESS,2,30},
	{TV_BOOK_DARKNESS,3,300},
	{TV_BOOK_NECROMANCY,2,30},
	{TV_BOOK_NECROMANCY,3,300},
	{TV_BOOK_CHAOS,2,30},
	{TV_BOOK_CHAOS,3,300},
	{TV_BOOK_OCCULT,2,10},
	{TV_BOOK_OCCULT,3,300},

	{TV_MATERIAL,SV_MATERIAL_HIKARIGOKE ,35 },
	{TV_MATERIAL,SV_MATERIAL_STONE  ,1 },
	{TV_MATERIAL,SV_MATERIAL_HEMATITE ,50 },
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE ,60 },
	{TV_MATERIAL,SV_MATERIAL_ARSENIC ,45 },
	{TV_MATERIAL,SV_MATERIAL_MERCURY ,90 },
	{TV_MATERIAL,SV_MATERIAL_MITHRIL ,150 },
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE ,200 },
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL ,1000 },
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE ,300 },
	{TV_MATERIAL,SV_MATERIAL_GELSEMIUM ,50 },
	{TV_MATERIAL,SV_MATERIAL_GARNET,30 },
	{TV_MATERIAL,SV_MATERIAL_AMETHYST,40 },
	{TV_MATERIAL,SV_MATERIAL_AQUAMARINE ,35 },
	{TV_MATERIAL,SV_MATERIAL_DIAMOND ,1500 },
	{TV_MATERIAL,SV_MATERIAL_EMERALD ,600 },
	{TV_MATERIAL,SV_MATERIAL_MOONSTONE ,35 },
	{TV_MATERIAL,SV_MATERIAL_RUBY ,650 },
	{TV_MATERIAL,SV_MATERIAL_PERIDOT ,35 },
	{TV_MATERIAL,SV_MATERIAL_SAPPHIRE ,550 },
	{TV_MATERIAL,SV_MATERIAL_OPAL ,40 },
	{TV_MATERIAL,SV_MATERIAL_TOPAZ ,50 },
	{TV_MATERIAL,SV_MATERIAL_LAPISLAZULI ,35 },
	{TV_MATERIAL,SV_MATERIAL_METEORICIRON ,1200 },
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE ,3000 },
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON ,5 },
	{TV_MATERIAL,SV_MATERIAL_ICESCALE ,300 },
	{TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT ,300 },
	{TV_MATERIAL,SV_MATERIAL_HOPE_FRAGMENT ,1200 },
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM ,2000 },
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI ,400 },
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT ,1250 },
	{TV_MATERIAL,SV_MATERIAL_ISHIZAKURA ,75 },
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU ,500 },

	{TV_MATERIAL,SV_MATERIAL_BROKEN_NEEDLE ,150 },
	{TV_MATERIAL,SV_MATERIAL_SKULL ,30 },	//v1.1.65

	{TV_MUSHROOM,SV_MUSHROOM_POISON ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_BLINDNESS  ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_PARANOIA ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_CONFUSION ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_HALLUCINATION  ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_PARALYSIS ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_WEAKNESS ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_SICKNESS ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_STUPIDITY ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_NAIVETY  ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_UNHEALTH ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_DISEASE  ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_POISON ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_BLINDNESS ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_PARANOIA ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_CONFUSION ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_SERIOUS ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORE_STR ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORE_CON ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORING ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MANA ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_BERSERK ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_RED ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_WHITE ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_BLUE ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_BLACK ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_L_BLUE ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GREEN ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GAUDY ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GRAY  ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_CLEAR ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_SUPER ,2000 },

	{ TV_MUSHROOM,SV_MUSHROOM_PUFFBALL ,30 },

	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER ,3000 },
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY ,5000 },

	{ TV_FOOD,SV_FOOD_DATURA,10 },
	{ TV_SOUVENIR,SV_SOUVENIR_MIRROR_OF_RUFFNOR ,2000 },

	{ TV_SOUVENIR,SV_SOUVENIR_ELDER_THINGS_CRYSTAL ,800 },

	{ TV_SOUVENIR,SV_SOUVENIR_KODOKUZARA ,1200 },

	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB ,300 },

	{ TV_SWEETS,SV_SWEETS_HONEY ,30 },

	{0,0,0}//終端用ダミー
};

/*:::霧雨魔法店ポイント交換景品テーブル*/
struct marisa_store_type marisa_present_table[] =
{
	{TV_CHEST,SV_CHEST_MARIPO_01 , 20},//耐火の薬
	{TV_CHEST,SV_CHEST_MARIPO_02 , 20},//耐冷の薬
	{TV_CHEST,SV_CHEST_MARIPO_03 , 30},//スピードの薬
	{TV_CHEST,SV_CHEST_MARIPO_04 , 40},//岩石溶解魔法棒
	{TV_CHEST,SV_CHEST_MARIPO_05 , 50},//テレポアウェイ魔法棒
	{TV_CHEST,SV_CHEST_MARIPO_06 , 70},//耐性の薬
	{TV_CHEST,SV_CHEST_MARIPO_07 , 100},//体力回復の薬
	{TV_CHEST,SV_CHEST_MARIPO_08 , 150},//啓蒙の薬

	{TV_CHEST,SV_CHEST_MARIPO_09 , 200},//スピードの杖
	{TV_CHEST,SV_CHEST_MARIPO_10 , 300},//氷の指輪
	{TV_CHEST,SV_CHEST_MARIPO_11 , 500},//ドラゴン・ブレスの魔法棒
	{TV_CHEST,SV_CHEST_MARIPO_12 , 700},//耐毒の指輪
	{TV_CHEST,SV_CHEST_MARIPO_13 , 800},//鑑定のロッド
	{TV_CHEST,SV_CHEST_MARIPO_14 , 1000},//破壊の杖
	{TV_CHEST,SV_CHEST_MARIPO_15 , 1200},//黄金の蜂蜜酒
	{TV_CHEST,SV_CHEST_MARIPO_16 , 1500},//無敵の薬

	{TV_CHEST,SV_CHEST_MARIPO_17 , 2000},//魔力の嵐の杖
	{TV_CHEST,SV_CHEST_MARIPO_18 , 2500},//サイバーデーモンの人形
	{TV_CHEST,SV_CHEST_MARIPO_19 , 4000},//妖刀（75階相当高級ドロップ)
	{TV_CHEST,SV_CHEST_MARIPO_20 , 5000},//ヌメノール冠
	{TV_CHEST,SV_CHEST_MARIPO_21 , 6000},//ソウルソード
	{TV_CHEST,SV_CHEST_MARIPO_22 , 10000},//ローエングリン鎧
	{TV_CHEST,SV_CHEST_MARIPO_23 , 15000},//月の羽衣
	{TV_CHEST,SV_CHEST_MARIPO_24 , 30000},//白いオーラ永久付与

	{0,0,0}//終端用ダミー
};


/*:::霧雨魔法店にて魔理沙が受け取るアイテムを判定*/
///mod150811 魔理沙職業の時の魔力抽出hookにも使う
bool marisa_will_buy(object_type *o_ptr)
{
	int i;
	if(p_ptr->pclass == CLASS_MARISA && o_ptr->tval < TV_BOOK_END) return FALSE;

	for(i=0;marisa_wants_table[i].tval;i++)
	{
		if(o_ptr->tval == marisa_wants_table[i].tval && o_ptr->sval == marisa_wants_table[i].sval) return TRUE;
	}

	//v2.0.13 伊吹瓢を高額買取することにする。職業魔理沙のときの魔力抽出の対象にはならない。
	if (p_ptr->pclass != CLASS_MARISA && o_ptr->name1 == ART_IBUKI) return TRUE;

	return FALSE;

}
//EX建物用魔理沙トレードアイテムhook
bool marisa_will_trade(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ITEMCARD) return TRUE;
	if(marisa_will_buy(o_ptr)) return TRUE;
	return FALSE;
}



//EX建物用尤魔トレードアイテムhook
bool yuma_will_trade(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD) return TRUE;

	if (o_ptr->tval == TV_SWEETS) return TRUE;

	if (o_ptr->tval == TV_ALCOHOL) return TRUE;

	if (o_ptr->tval == TV_MUSHROOM) return TRUE;

	if (o_ptr->tval == TV_FLASK) return TRUE;//油壷とエンジニア用エネルギーパック

	if (o_ptr->name1 == ART_IBUKI) return TRUE;//伊吹瓢

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG) return TRUE; //巨大な卵

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU) return TRUE; //道寿の壺

	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI) return TRUE; //高草郡の竹

	return FALSE;
}

static void building_prt_maripo(void)
{
	char tmp_str[80];
	prt("MARIPO: ", 22,53);

	sprintf(tmp_str, "%7ld", (long)p_ptr->maripo);
	prt(tmp_str, 22, 63);
}

/*:::アイテムを渡しポイントを取得する*/
///mod150219 さらに魔理沙用魔力変数に加算
static void material_2_maripo(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point=0, total_point;

	building_prt_maripo();

	q = _("何を持ってきたんだ？", "What do you have?");
	s = _("そのザックの中に私が欲しい物はないぜ。", "You don't have anything that I want.");

	item_tester_hook = marisa_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	//v2.0.13 伊吹瓢は1500MARIPOと交換してくれるようにする
	if (o_ptr->name1 == ART_IBUKI)
	{
		base_point = 1500;
	}
	else
	{
		for (i = 0; marisa_wants_table[i].tval; i++)
			if (q_ptr->tval == marisa_wants_table[i].tval && q_ptr->sval == marisa_wants_table[i].sval) base_point = marisa_wants_table[i].maripo;
	}

	total_point = base_point * amt;

	if(total_point < 1) msg_format("ERROR:Maripo<1"); //paranoia

	if(base_point < 30) msg_format(_("「その程度の物か。まあ%dポイントって所だな。」",
                                    "'Something like that? Well, it's worth %d points.'"),total_point);
	else if(base_point < 300) msg_format(_("「ほう、中々の物を持ってきたな。%dポイント出すぜ。」",
                                            "'Hmm, looks interesting. I'll give you %d points.'"),total_point);
	else if(base_point < 1000) msg_format(_("「かなりの値打ちものじゃないか。%dポイントでどうだ？」",
                                            "'That looks very valuable! How about %d points?'"),total_point);
	else msg_format(_("「ゆずってくれたのむ！%dポイント出してもいいぜ！」",
                    "'Give that to me, I beg you! I'll give %d points!'"),total_point);

	if(!get_check(format(_("%sを渡しますか？", "Hand over %s?"),o_name))) return;

	p_ptr->maripo += total_point;
	if(p_ptr->maripo > 999999) p_ptr->maripo = 999999;
	building_prt_maripo();
	msg_format(_("「よし、商談成立だぜ！」", "'Alright, it's a deal!'"));
	msg_format(_("%dポイントのMARIPOを獲得した。", "You obtained %d MARIPO."),total_point);

	///mod150219 魔理沙魔力獲得
	//v2.0.13 伊吹瓢を渡したときには魔力を得られないはず
	marisa_gain_power(q_ptr, 100);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);

}

/*:::ポイントを消費しプレゼントを受け取る*/
static void maripo_2_present(void)
{

	int cs=0,page=0,choice=-1;
	int i;
	char c;
	object_type forge;
	object_type *o_ptr = &forge;
	int slot_new;
	char o_name[MAX_NLEN];

	object_wipe(o_ptr);

	building_prt_maripo();
	if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("「おいおい、荷物が一杯だぜ。」",
                    "'Hey, your backpack is already full.'"));
		return;
	}

	while(1)
	{

		clear_bldg(5, 18);
		c_put_str(TERM_WHITE,_("魔理沙「日頃のご愛顧に応えて、どれでも好きなものを選んでくれ！」",
                            "Marisa - 'Thank you for your patronage! Pick the one you like!'"),5 , 6);
		c_put_str(TERM_WHITE,_("(a～h:プレゼントを選択 / n,スペース:次のページへ / ESC:キャンセル)",
                            "(a-h: Choose present / n, Spacebar: Next page / ESC: Cancel)"),6 , 6);
		for(i=0;i<8;i++)
		{

			int color;
			int table_num = page * 8 + i;

			if((p_ptr->maripo < marisa_present_table[table_num].maripo) || ( (1L << table_num) & (p_ptr->maripo_presented_flag)) ) color = TERM_L_DARK;
			else if(cs==i) color = TERM_YELLOW;
			else color = TERM_WHITE;
			object_prep(o_ptr, lookup_kind(marisa_present_table[table_num].tval,marisa_present_table[table_num].sval) );
			object_desc(o_name, o_ptr, 0);
			if( ( (1L << table_num) & (p_ptr->maripo_presented_flag))) c_put_str(color,format(_("%c) [   品切れ   ] %s",
                                                                                            "%c) [  Sold out  ] %s"),'a'+i,o_name),8 + i, 10);
			else c_put_str(color,format("%c) [%6d MARIPO] %s",'a'+i,marisa_present_table[table_num].maripo,o_name),8 + i, 10);
		}
		prt("",8+i,10);

		c = inkey();
		if (c == ESCAPE)
		{
			clear_bldg(5, 18);
			return;
		}
		if (c == '2' && cs < 7) cs++;
		if (c == '8' && cs > 0) cs--;
		if (c == '\r' || c == '\n') c=cs+'a';
		if (c == ' ' || c == 'n') page++;
		if (page > 2) page = 0;

		if(c >= 'a' && c <= 'h')
		{
			choice = page*8 + c - 'a';

			if( (1L << choice) & (p_ptr->maripo_presented_flag))
			{
				msg_format(_("「それはもう品切れだぜ。」", "'This one is already sold out.'"));
				continue;
			}
			else if(p_ptr->maripo < marisa_present_table[choice].maripo)
			{
				msg_format(_("「悪いな。それが欲しいならもっとポイントを貯めてくれ。」",
                            "'Sorry, but you have to save up more points for this one.'"));
				continue;
			}

			if (get_check_strict(_("「それが欲しいのか？」", "'Do you want this?'"), CHECK_DEFAULT_Y))
			{
			 	break;
			}
		}
	}

	p_ptr->maripo_presented_flag |= (1L << choice);
	p_ptr->maripo -= marisa_present_table[choice].maripo;
	building_prt_maripo();

	object_prep(o_ptr, lookup_kind(marisa_present_table[choice].tval,marisa_present_table[choice].sval) );
	if(o_ptr->tval == TV_CHEST) o_ptr->pval = -6; //(非施錠の箱 )
	else apply_magic(o_ptr,50,AM_NO_FIXED_ART); //今のところ箱しかプレゼントがないが念のため
	object_desc(o_name, o_ptr, 0);

	slot_new = inven_carry(o_ptr);
	msg_format(_("%sを受け取った。", "You have received %s."),o_name);
	autopick_alter_item(slot_new, FALSE);
	handle_stuff();
	clear_bldg(5, 18);

}



/*:::香霖堂へ珍品を売りつけるための表　霧雨魔法店のテーブルを流用 TV,SV,買い取り価格*/
struct marisa_store_type korin_wants_table[] =
{
	{TV_SOUVENIR,SV_SOUVENIR_SMARTPHONE,100000},
	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER,300000},
	{TV_SOUVENIR,SV_SOUVENIR_GAMEBOY,1000000},
	{TV_SOUVENIR,SV_SOUVENIR_ILMENITE,100000},
	{TV_SOUVENIR,SV_SOUVENIR_ULTRASONIC,5000},

	{TV_SOUVENIR,SV_SOUVENIR_KAPPA_5,300000},
	{TV_SOUVENIR,SV_SOUVENIR_PRISM,30000},
	{TV_SOUVENIR,SV_SOUVENIR_KAPPA_ARM,100000},
	{TV_SOUVENIR,SV_SOUVENIR_BIG_EGG,200000},
	{TV_SOUVENIR,SV_SOUVENIR_ASIA,700000},
	{TV_SOUVENIR,SV_SOUVENIR_STARS_AND_STRIPES,100000},
	{TV_SOUVENIR,SV_SOUVENIR_OUIJA_BOARD,20000},
	{TV_SOUVENIR,SV_SOUVENIR_KESERAN_PASARAN,250000},
	{TV_SOUVENIR,SV_SOUVENIR_MARISA_TUBO,50000},
	{TV_SOUVENIR,SV_SOUVENIR_DOUJU,120000},
	{ TV_SOUVENIR,SV_SOUVENIR_MORINJI,80000 },
	{TV_SOUVENIR,SV_SOUVENIR_KINKAKUJI,700000},
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY,1000000 },

	{ TV_SOUVENIR,SV_SOUVENIR_FOUR_MONKEYS,40000 },

	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,600000},
	{TV_MATERIAL,SV_MATERIAL_METEORICIRON,300000},
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL,100000},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE,20000},
	{TV_MATERIAL,SV_MATERIAL_ICESCALE,15000},
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM,140000},
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI,16000},
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT,120000},

	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU,50000 },

	//	{TV_SOFTDRINK,SV_SOFTDRINK_COLA,500},

	{ TV_SOUVENIR,SV_SOUVENIR_STONE_BABY,100 },
	{ TV_SOUVENIR,SV_SOUVENIR_MIRROR_OF_RUFFNOR ,200000 },
	{ TV_SOUVENIR,SV_SOUVENIR_TRUMPS_OF_DOOM ,300000 },
	{ TV_SOUVENIR,SV_SOUVENIR_SKIDBLADNIR ,500000 },
	{ TV_SOUVENIR,SV_SOUVENIR_NODENS_CHARIOT ,400000 },

	{ TV_SOUVENIR,SV_SOUVENIR_ELDER_THINGS_CRYSTAL ,30000 },

	{ TV_SOUVENIR,SV_SOUVENIR_KODOKUZARA ,66000 },
	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB ,20000 },

	{0,0,0}//終端用ダミー
};

/*:::香霖堂にて珍品として引き取られるアイテムを判定*/
static bool korin_will_buy(object_type *o_ptr)
{
	int i;
	for(i=0;korin_wants_table[i].tval;i++)
	{
		if(o_ptr->tval == korin_wants_table[i].tval && o_ptr->sval == korin_wants_table[i].sval) return TRUE;
	}
	return FALSE;

}

//v2.0.6
//香霖堂に珍品などを売ったときの価格
//尤魔が食べたときの栄養計算にも使いたくなったので関数を分けた
int calc_korin_sell_price(object_type *o_ptr)
{
	int base_point = 0;
	int i;

	for (i = 0; korin_wants_table[i].tval; i++)
	{
		if (o_ptr->tval == korin_wants_table[i].tval && o_ptr->sval == korin_wants_table[i].sval) base_point = korin_wants_table[i].maripo;
	}

	return base_point;

}


/*::香霖堂に珍品を売りつける*/
static void sell_curiosity(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point=0, total_point;

	building_prt_gold();

	q = _("何か興味深いものを持っているようだね？",
        "Looks like you have something interesting, right?");
	s = _("特に興味を引くようなものはないようだね。",
        "You don't seem to have anything particularly interesting.");

	item_tester_hook = korin_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	base_point = calc_korin_sell_price(q_ptr);

	total_point = base_point * amt;

	if(total_point < 1) msg_format("ERROR:Maripo<1"); //paranoia

	if(base_point < 10000) msg_format(_("「それほど珍しいものでもないな。まあ $%dといったところか。」",
                                        "'It's not that rare. Well, I'll give you $%d.'"),total_point);
	else if(base_point < 100000) msg_format(_("「ふむ、なかなかの品だ。$%d出そう。」",
                                            "'Hmm, looks pretty good. Let's settle at $%d.'"),total_point);
	else if(base_point < 1000000) msg_format(_("「ほう、実に興味深い逸品だ。$%dで引き取らせてほしい。」",
                                            "'Ohh, that looks very interesting. I'll take it for $%d.'"),total_point);
	else
	{
		msg_format(_("「馬鹿な、そんな物が世界を・・！",
                    "'It can't be! Just to think that something like that..."));
		msg_print(NULL);
		msg_format(_("いや失礼した。こちらには$%dを支払う用意がある。」",
                    "...I'm sorry. I'm ready to pay $%d.'"),total_point);
	}

	if(!get_check(format(_("%sを渡しますか？", "Hand over %s?"),o_name))) return;

	p_ptr->au += total_point;
	building_prt_gold();
	msg_format(_("「よし、商談成立だ。」", "'Alright, it's a deal.'"));
	msg_format(_("$%dで売り払った。", "You sold it for $%d."),total_point);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);

}






/*:::闘技場処理。モンスターを選定して戦わせるところまで？*/
///sysdel tougi inside_battleは面倒なので残すがこの関数は削除する予定
#if 0
void battle_monsters(void)
{
	int total, i;
	int max_dl = 0;
	int mon_level;
	int power[4];
	bool tekitou;
	bool old_inside_battle = p_ptr->inside_battle;

	for (i = 0; i < max_d_idx; i++)
		if (max_dl < max_dlv[i]) max_dl = max_dlv[i];

	mon_level = randint1(MIN(max_dl, 122))+5;
	if (randint0(100) < 60)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}
	if (randint0(100) < 30)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}

	while (1)
	{
		total = 0;
		tekitou = FALSE;
		for(i=0;i<4;i++)
		{
			int r_idx, j;
			while (1)
			{
				get_mon_num_prep(vault_aux_battle, NULL);
				p_ptr->inside_battle = TRUE;
				r_idx = get_mon_num(mon_level);
				p_ptr->inside_battle = old_inside_battle;
				if (!r_idx) continue;

				if ((r_info[r_idx].flags1 & RF1_UNIQUE) || (r_info[r_idx].flags7 & RF7_UNIQUE2))
				{
					if ((r_info[r_idx].level + 10) > mon_level) continue;
				}

				for (j = 0; j < i; j++)
					if(r_idx == battle_mon[j]) break;
				if (j<i) continue;

				break;
			}
			battle_mon[i] = r_idx;
			if (r_info[r_idx].level < 45) tekitou = TRUE;
		}

		for (i=0;i<4;i++)
		{
			monster_race *r_ptr = &r_info[battle_mon[i]];
			int num_taisei = count_bits(r_ptr->flagsr & (RFR_IM_ACID | RFR_IM_ELEC | RFR_IM_FIRE | RFR_IM_COLD | RFR_IM_POIS));

			if (r_ptr->flags1 & RF1_FORCE_MAXHP)
				power[i] = r_ptr->hdice * r_ptr->hside * 2;
			else
				power[i] = r_ptr->hdice * (r_ptr->hside + 1);
			power[i] = power[i] * (100 + r_ptr->level) / 100;
			if (r_ptr->speed > 110)
				power[i] = power[i] * (r_ptr->speed * 2 - 110) / 100;
			if (r_ptr->speed < 110)
				power[i] = power[i] * (r_ptr->speed - 20) / 100;
			if (num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if (r_ptr->flags6 & RF6_INVULNER)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags6 & RF6_HEAL)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags5 & RF5_DRAIN_MANA)
				power[i] = power[i] * 11 / 10;
			if (r_ptr->flags1 & RF1_RAND_25)
				power[i] = power[i] * 9 / 10;
			if (r_ptr->flags1 & RF1_RAND_50)
				power[i] = power[i] * 9 / 10;

			switch (battle_mon[i])
			{
				case MON_GREEN_G:
				case MON_THAT_BAT:
				case MON_GHOST_Q:
					power[i] /= 4;
					break;
				case MON_LOST_SOUL:
				case MON_GHOST:
					power[i] /= 2;
					break;
				case MON_UND_BEHOLDER:
				case MON_SANTACLAUS:
				case MON_ULT_BEHOLDER:
				case MON_UNGOLIANT:
				case MON_ATLACH_NACHA:
				case MON_Y_GOLONAC:
					power[i] = power[i] * 3 / 5;
					break;
				case MON_ROBIN_HOOD:
				case MON_RICH:
				case MON_LICH:
				case MON_COLOSSUS:
				case MON_CRYPT_THING:
				case MON_MASTER_LICH:
				case MON_DREADMASTER:
				case MON_DEMILICH:
				case MON_SHADOWLORD:
				case MON_ARCHLICH:
				case MON_BLEYS:
				case MON_CAINE:
				case MON_JULIAN:
				case MON_VENOM_WYRM:
				case MON_MASTER_MYS:
				case MON_G_MASTER_MYS:
					power[i] = power[i] * 3 / 4;
					break;
				case MON_VORPAL_BUNNY:
				case MON_SHAGRAT:
				case MON_GORBAG:
				case MON_LOG_MASTER:
				case MON_JURT:
				case MON_GRAV_HOUND:
				case MON_SHIM_VOR:
				case MON_JUBJUB:
				case MON_CLUB_DEMON:
				case MON_LLOIGOR:
				case MON_NIGHTCRAWLER:
				case MON_NIGHTWALKER:
				case MON_RAPHAEL:
				case MON_SHAMBLER:
				case MON_SKY_DRAKE:
				case MON_GERARD:
				case MON_G_CTHULHU:
				case MON_SPECT_WYRM:
				case MON_BAZOOKER:
				//case MON_GCWADL:
				case MON_KIRIN:
				case MON_FENGHUANG:
					power[i] = power[i] * 4 / 3;
					break;
				case MON_UMBER_HULK:
				case MON_FIRE_VOR:
				case MON_WATER_VOR:
				case MON_COLD_VOR:
				case MON_ENERGY_VOR:
				case MON_GACHAPIN:
				case MON_REVENANT:
				//case MON_NEXUS_VOR:
				case MON_PLASMA_VOR:
				case MON_TIME_VOR:
				case MON_MANDOR:
				case MON_KAVLAX:
				//case MON_RINALDO:
				case MON_STORMBRINGER:
				case MON_TIME_HOUND:
				case MON_PLASMA_HOUND:
				case MON_TINDALOS:
				case MON_CHAOS_VOR:
				case MON_AETHER_VOR:
				case MON_AETHER_HOUND:
				case MON_CANTORAS:
				case MON_GODZILLA:
				case MON_TARRASQUE:
				case MON_DESTROYER:
				case MON_MORGOTH:
				case MON_SERPENT:
				case MON_OROCHI:
				case MON_D_ELF_SHADE:
				case MON_MANA_HOUND:
				case MON_SHARD_VOR:
				case MON_BANORLUPART:
				case MON_BOTEI:
				case MON_JAIAN:
				case MON_BAHAMUT:
				case MON_WAHHA:
					power[i] = power[i] * 3 / 2;
					break;
				case MON_ROLENTO:
				case MON_CYBER:
				case MON_CYBER_KING:
				case MON_UNICORN_ORD:
					power[i] = power[i] * 5 / 3;
					break;
				case MON_ARCH_VILE:
				case MON_PHANTOM_B:
				case MON_WYRM_POWER:
					power[i] *= 2;
					break;
				case MON_NODENS:
				case MON_CULVERIN:
					power[i] *= 3;
					break;
				case MON_ECHIZEN:
					power[i] *= 9;
					break;
				case MON_HAGURE:
					power[i] *= 100000;
					break;
				default:
					break;
			}
			total += power[i];
		}
		for (i=0;i<4;i++)
		{
			power[i] = total*60/power[i];
			if (tekitou && ((power[i] < 160) || power[i] > 1500)) break;
			if ((power[i] < 160) && randint0(20)) break;
			if (power[i] < 101) power[i] = 100 + randint1(5);
			mon_odds[i] = power[i];
		}
		if (i == 4) break;
	}
}
#endif

#if 0
static bool kakutoujou(void)
{
	s32b maxbet;
	s32b wager;
	char out_val[160], tmp_str[80];
	cptr p;

	if ((turn - old_battle) > TURNS_PER_TICK*250)
	{
		battle_monsters();
		old_battle = turn;
	}

	screen_save();

	/* No money */
	if (p_ptr->au < 1)
	{
#ifdef JP
		msg_print("おい！おまえ一文なしじゃないか！こっから出ていけ！");
#else
		msg_print("Hey! You don't have gold - get out of here!");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}
	else
	{
		int i;

		clear_bldg(4, 10);

#ifdef JP
		prt("モンスター                                                     倍率", 4, 4);
#else
		prt("Monsters                                                       Odds", 4, 4);
#endif
		for (i=0;i<4;i++)
		{
			char buf[80];
			monster_race *r_ptr = &r_info[battle_mon[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld倍", i+1, format("%s%s",r_name + r_ptr->name, (r_ptr->flags1 & RF1_UNIQUE) ? "もどき" : "      "), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", (r_ptr->flags1 & RF1_UNIQUE) ? "Fake " : "", r_name + r_ptr->name), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("どれに賭けますか:", 0, 0);
#else
		prt("Which monster: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
			if (i >= '1' && i <= '4')
			{
				sel_monster = i-'1';
				battle_odds = mon_odds[sel_monster];
				break;
			}
			else bell();
		}

		clear_bldg(4,4);
		for (i=0;i<4;i++)
			if (i !=sel_monster) clear_bldg(i+5,i+5);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"賭け金 (1-%ld)？", (long int)maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", (long int)maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("おい！金が足りないじゃないか！出ていけ！");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ldゴールドだけ受けよう。残りは取っときな。", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.",(long int) maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("ＯＫ、１ゴールドでいこう。");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			battle_odds = MAX(wager+1, wager * battle_odds / 100);
			kakekin = wager;
			p_ptr->au -= wager;
			reset_tim_flags();

			/* Save the surface floor as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS);

			p_ptr->inside_battle = TRUE;
			p_ptr->leaving = TRUE;

			leave_bldg = TRUE;
			screen_load();

			return (TRUE);
		}
	}
	screen_load();

	return (FALSE);
}
#endif


/*:::本日の賞金首を表示する。決定するのはここではない*/
///del131221 賞金首削除
#if 0
static void today_target(void)
{
	char buf[160];
	monster_race *r_ptr = &r_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "本日の賞金首", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"ターゲット： %s",r_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",r_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"死体 ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"骨   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
	p_ptr->today_mon = today_mon;
}
#endif

///del131231 賞金首削除
#if 0
static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "一獲千金の大チャンス！！！", 5, 10);
c_put_str(TERM_YELLOW, "ターゲット：幻の珍獣「ツチノコ」", 6, 10);
c_put_str(TERM_WHITE, "生け捕り ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "死体     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "骨       ----   $100,000", 10, 10);
#else
c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
c_put_str(TERM_YELLOW, "target: the rarest animal 'Tsuchinoko'", 6, 10);
c_put_str(TERM_WHITE, "catch alive ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "corpse      ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}
#endif

///del131231 賞金首削除
#if 0

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("死体を持ち帰れば報酬を差し上げます。",4 ,10);
c_put_str(TERM_YELLOW, "現在の賞金首", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse",4 ,10);
c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		byte color;
		cptr done_mark;
		/*:::既に換金済みの場合idxに+10000されているらしい*/
		monster_race *r_ptr = &r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])];

		if (kubi_r_idx[i] > 10000)
		{
			color = TERM_RED;
#ifdef JP
			done_mark = "(済)";
#else
			done_mark = "(done)";
#endif
		}
		else
		{
			color = TERM_WHITE;
			done_mark = "";
		}

		c_prt(color, format("%s %s", r_name + r_ptr->name, done_mark), y+7, 10);

		y = (y+1) % 10;
		if (!y && (i < MAX_KUBI -1))
		{
#ifdef JP
			prt("何かキーを押してください", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}
#endif

/* List of prize object */
static struct {
	s16b tval;
	s16b sval;
} prize_list[MAX_KUBI] =
{
	{TV_POTION, SV_POTION_CURING},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_RESISTANCE},
	{TV_POTION, SV_POTION_ENLIGHTENMENT},

	{TV_POTION, SV_POTION_HEALING},
	{TV_POTION, SV_POTION_RESTORE_MANA},
	{TV_SCROLL, SV_SCROLL_STAR_DESTRUCTION},
	{TV_POTION, SV_POTION_STAR_ENLIGHTENMENT},
	{TV_SCROLL, SV_SCROLL_SUMMON_PET},

	{TV_SCROLL, SV_SCROLL_GENOCIDE},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_NEW_LIFE},
	{TV_SCROLL, SV_SCROLL_MASS_GENOCIDE},

	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_AUGMENTATION},
	{TV_POTION, SV_POTION_INVULNERABILITY},
	{TV_SCROLL, SV_SCROLL_ARTIFACT},
};


/* Get prize */
/*:::賞金首を金やアイテムに引き換える*/
///del131221 死体削除に伴い換金所を削除
#if 0
static bool kankin(void)
{
	int i, j;
	bool change = FALSE;
	char o_name[MAX_NLEN];
	object_type *o_ptr;

	/* Loop for inventory and right/left arm */
	for (i = 0; i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];

		/* Living Tsuchinoko worthes $1000000 */
		if ((o_ptr->tval == TV_CAPTURE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (long int)(1000000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(1000000L * o_ptr->number));
#endif
				p_ptr->au += 1000000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Corpse of Tsuchinoko worthes $200000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (long int)(200000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(200000L * o_ptr->number));
#endif
				p_ptr->au += 200000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Bones of Tsuchinoko worthes $100000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (long int)(100000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(100000L * o_ptr->number));
#endif
				p_ptr->au += 100000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
#endif
				p_ptr->au += (r_info[today_mon].level * 50 + 100) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
#endif
				p_ptr->au += (r_info[today_mon].level * 30 + 60) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_KUBI; j++)
	{
		/* Need reverse order --- Positions will be changed in the loop */
		for (i = INVEN_PACK-1; i >= 0; i--)
		{
			o_ptr = &inventory[i];
			if ((o_ptr->tval == TV_CORPSE) && (o_ptr->pval == kubi_r_idx[j]))
			{
				char buf[MAX_NLEN+20];
				int num, k, item_new;
				object_type forge;

				object_desc(o_name, o_ptr, 0);
#ifdef JP
				sprintf(buf, "%sを渡しますか？",o_name);
#else
				sprintf(buf, "Hand %s over? ",o_name);
#endif
				if (!get_check(buf)) continue;

#if 0 /* Obsoleted */
#ifdef JP
				msg_format("賞金 %ld＄を手に入れた。", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#endif
				p_ptr->au += (r_info[kubi_r_idx[j]].level+1) * 300 * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				chg_virtue(V_JUSTICE, 5);
				kubi_r_idx[j] += 10000;

				change = TRUE;
#endif /* Obsoleted */

				/* Hand it first */
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);

				chg_virtue(V_JUSTICE, 5);
				kubi_r_idx[j] += 10000;

				/* Count number of unique corpses already handed */
				for (num = 0, k = 0; k < MAX_KUBI; k++)
				{
					if (kubi_r_idx[k] >= 10000) num++;
				}

#ifdef JP
				msg_format("これで合計 %d ポイント獲得しました。", num);
#else
				msg_format("You earned %d point%s total.", num, (num > 1 ? "s" : ""));
#endif

				/* Prepare to make a prize */
				object_prep(&forge, lookup_kind(prize_list[num-1].tval, prize_list[num-1].sval));
				apply_magic(&forge, object_level, AM_NO_FIXED_ART);

				/* Identify it fully */
				object_aware(&forge);
				object_known(&forge);

				/*
				 * Hand it --- Assume there is an empty slot.
				 * Since a corpse is handed at first,
				 * there is at least one empty slot.
				 */
				item_new = inven_carry(&forge);

				/* Describe the object */
				object_desc(o_name, &forge, 0);
#ifdef JP
				msg_format("%s(%c)を貰った。", o_name, index_to_label(item_new));
#else
				msg_format("You get %s (%c). ", o_name, index_to_label(item_new));
#endif

				/* Auto-inscription */
				autopick_alter_item(item_new, FALSE);

				/* Handle stuff */
				handle_stuff();

				change = TRUE;
			}
		}
	}

	if (!change)
	{
#ifdef JP
		msg_print("賞金を得られそうなものは持っていなかった。");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}
#endif


/*:::悪夢を見るときに出てくるモンスター　エルドリッチホラー持ちを選出*/
bool get_nightmare(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Require eldritch horrors */
	if (!(r_ptr->flags2 & (RF2_ELDRITCH_HORROR))) return (FALSE);

	/* Require high level */
	if (r_ptr->level <= p_ptr->lev) return (FALSE);

	/* Accept this monster */
	return (TRUE);
}

/*:::眠ったときに悪夢を見る処理　悪夢モードで宿屋で止まったり眠らされたり麻痺したとき*/
void have_nightmare(int r_idx)
{
	bool happened = FALSE;
	monster_race *r_ptr = &r_info[r_idx];
	int power = r_ptr->level + 10;
	char m_name[80];
	cptr desc = r_name + r_ptr->name;

	if(p_ptr->pclass == CLASS_DOREMY)
	{
		msg_print(_("あなたは悪夢を美味しく頂いた。",
                    "You've enjoyed the delicious nightmare."));
		(void)set_food(PY_FOOD_MAX - 1);
		return;
	}

	if(r_idx == MON_CLOWNPIECE || weird_luck())
	{
		msg_print(_("何か「貴方の悪夢は私が美味しく頂きました。」",
                    "Someone - 'Your nightmare was delicious.'"));
		return;
	}

	/* Describe it */
#ifndef JP
	if (!(r_ptr->flags1 & RF1_UNIQUE))
		sprintf(m_name, "%s %s", (is_a_vowel(desc[0]) ? "an" : "a"), desc);
	else
#endif
		sprintf(m_name, "%s", desc);

	if (!(r_ptr->flags1 & RF1_UNIQUE))
	{
		if (r_ptr->flags1 & RF1_FRIENDS) power /= 2;
	}
	else power *= 2;

	if (saving_throw(p_ptr->skill_sav * 100 / power))
	{

		if(one_in_(7) || p_ptr->pclass == CLASS_REIMU)
			msg_format(_("夢の中で初見殺しの弾幕に囲まれた。",
                        "You dream of running into a blind run killer danmaku pattern."), m_name);
		else
			msg_format(_("夢の中で%sに追いかけられた。",
                        "You dream of being pursued by %s."), m_name);

		/* Safe */
		return;
	}

	if (p_ptr->image)
	{
		/* Something silly happens... */
#ifdef JP
		msg_format("%s%sの顔を見てしまった！",
#else
		msg_format("You behold the %s visage of %s!",
#endif

					  funny_desc[randint0(MAX_SAN_FUNNY)], m_name);

		if (one_in_(3))
		{
			msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
			p_ptr->image = p_ptr->image + randint1(r_ptr->level);
		}

		/* Never mind; we can't see it clearly enough */
		return;
	}

	/* Something frightening happens... */
#ifdef JP
	msg_format("%s%sの顔を見てしまった！",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

	///mod140126 悪夢処理を狂気免疫と耐性含めて書きなおした
#if 0
	if (!p_ptr->mimic_form)
	{
		///race 悪夢を見たときのエルドリッチホラー耐性
		///sys エルドリッチホラー耐性　いずれ耐性の一つとして判定する予定
		switch (p_ptr->prace)
		{
		/* Demons may make a saving throw */
		case RACE_IMP:
		//case RACE_DEMON:
			if (saving_throw(20 + p_ptr->lev)) return;
			break;
		/* Undead may make a saving throw */
		case RACE_SKELETON:
		case RACE_ZOMBIE:
		case RACE_SPECTRE:
		case RACE_VAMPIRE:
			if (saving_throw(10 + p_ptr->lev)) return;
			break;
		}
	}
	///mod131228 狂気耐性で悪夢に抵抗
	else if(p_ptr->resist_insanity && saving_throw(30 + p_ptr->lev)){
		return;
	}
	else
	{
		/* Demons may make a saving throw */
		if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)
		{
			if (saving_throw(20 + p_ptr->lev)) return;
		}
		/* Undead may make a saving throw */
		else if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD)
		{
			if (saving_throw(10 + p_ptr->lev)) return;
		}
	}
#endif
	if(immune_insanity()) return;
	else if(p_ptr->resist_insanity && saving_throw(15 + p_ptr->lev)) return;


	/* Mind blast */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->resist_chaos && one_in_(3))
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	/* Lose int & wis */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
		return;
	}

	/* Brain smash */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_INT);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_WIS);
		}
		if (!p_ptr->resist_chaos)
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	///mutation 悪夢を見たときの悪性突然変異
	/* Amnesia */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (lose_all_info())
		{
#ifdef JP
msg_print("あまりの恐怖に全てのことを忘れてしまった！");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}
	//蓬莱人は変異無効
	if(p_ptr->prace == RACE_HOURAI) return;

	get_bad_mental_mutation();

#if 0

	/* Else gain permanent insanity */
	if ((p_ptr->muta3 & MUT3_MORONIC) && (p_ptr->muta2 & MUT2_BERS_RAGE) &&
		((p_ptr->muta2 & MUT2_COWARDICE) || (p_ptr->resist_fear)) &&
		((p_ptr->muta2 & MUT2_HALLU) || (p_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(4))
		{
			case 1:
			{
				if (!(p_ptr->muta3 & MUT3_MORONIC))
				{
					if ((p_ptr->stat_use[A_INT] < 4) && (p_ptr->stat_use[A_WIS] < 4))
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になったような気がした。しかしそれは元々だった。");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になった！");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (p_ptr->muta3 & MUT3_HYPER_INT)
					{
#ifdef JP
msg_print("あなたの脳は生体コンピュータではなくなった。");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						p_ptr->muta3 &= ~(MUT3_HYPER_INT);
					}
					p_ptr->muta3 |= MUT3_MORONIC;
					happened = TRUE;
				}
				break;
			}
			case 2:
			{
				if (!(p_ptr->muta2 & MUT2_COWARDICE) && !p_ptr->resist_fear)
				{
#ifdef JP
msg_print("あなたは閉所恐怖症になった！");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (p_ptr->muta3 & MUT3_FEARLESS)
					{
#ifdef JP
msg_print("あなたはもう恐れ知らずではなくなった。");
#else
						msg_print("You are no longer fearless.");
#endif

						p_ptr->muta3 &= ~(MUT3_FEARLESS);
					}

					p_ptr->muta2 |= MUT2_COWARDICE;
					happened = TRUE;
				}
				break;
			}
			case 3:
			{
				if (!(p_ptr->muta2 & MUT2_HALLU) && !p_ptr->resist_chaos)
				{
#ifdef JP
msg_print("幻覚をひき起こす精神錯乱に陥った！");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					p_ptr->muta2 |= MUT2_HALLU;
					happened = TRUE;
				}
				break;
			}
			default:
			{
				if (!(p_ptr->muta2 & MUT2_BERS_RAGE))
				{
#ifdef JP
msg_print("激烈な感情の発作におそわれるようになった！");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					p_ptr->muta2 |= MUT2_BERS_RAGE;
					happened = TRUE;
				}
				break;
			}
		}
	}
#endif
	p_ptr->update |= PU_BONUS;
	handle_stuff();
}


/*
 * inn commands
 * Note that resting for the night was a perfect way to avoid player
 * ghosts in the town *if* you could only make it to the inn in time (-:
 * Now that the ghosts are temporarily disabled in 2.8.X, this function
 * will not be that useful.  I will keep it in the hopes the player
 * ghost code does become a reality again. Does help to avoid filthy urchins.
 * Resting at night is also a quick way to restock stores -KMW-
 */
/*:::宿屋コマンド*/
///sys 宿屋　食事時に食事不可の＠をここで判別してメッセージ出すようにする予定
static bool inn_comm(int cmd)
{
	switch (cmd)
	{
		case BACT_FOOD: /* Buy food & drink */

			if(    p_ptr->prace == RACE_TSUKUMO
				|| p_ptr->prace == RACE_ANDROID
				|| p_ptr->prace == RACE_GOLEM
				|| p_ptr->prace == RACE_VAMPIRE
				|| p_ptr->pclass == CLASS_HINA
				|| p_ptr->prace == RACE_MAGIC_JIZO
				|| p_ptr->prace == RACE_ANIMAL_GHOST//v1.1.85　忘れてたので追加
				|| p_ptr->prace == RACE_HANIWA
				|| p_ptr->pclass == CLASS_CHIYARI
				)
			{
				msg_print(_("あなた向けの食事はここにはないようだ。",
                            "Doesn't look like there's food suitable for you here."));
				return FALSE;
			}
			else if(p_ptr->pclass == CLASS_DOREMY)
			{
				msg_print(_("普通の食事には興味がない。まず宿に泊まろう…",
                            "You're not interested in ordinary food. Let's stay at the inn first..."));
				return FALSE;
			}

			//Exモードのミスティアの屋台にいるとき
			if(EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_MYSTIA)
			{
				if(p_ptr->food < PY_FOOD_FULL -1 || p_ptr->blind || p_ptr->pclass == CLASS_YUMA)
				{
					msg_print(_("店主は八目鰻の蒲焼きを振舞ってくれた。",
                                "The owner gives you some roast lamprey."));
					(void)set_food(PY_FOOD_MAX - 1);
					set_blind(0);
					//v2.0.10追加
					set_tim_invis(5000, FALSE);

					break;
				}
				else
				{
					msg_print(_("店主は一曲披露してくれた。",
                                "The owner shows off and signs a song."));
					if(!p_ptr->resist_blind) set_blind(p_ptr->blind + 50 + randint1(50));
					return FALSE;
				}

			}
			//v1.1.53 Exモードのラルバの部屋にいるとき
			if (EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_LARVA)
			{
				if (p_ptr->food < PY_FOOD_FULL - 1 )
				{
					msg_print(_("あなたはお茶とお菓子を楽しんだ。",
                                "You enjoy the tea and sweets."));
					(void)set_food(PY_FOOD_MAX - 1);
					break;
				}

			}

			if (p_ptr->food >= PY_FOOD_FULL && p_ptr->pclass != CLASS_YUMA)
			{
				msg_print(_("今は満腹だ。", "You are already full."));

				return FALSE;
			}

			if(EXTRA_MODE)
				msg_print(_("あなたは手早く食事を済ませた。", "You quickly finish your meal."));
			else
				msg_print(_("店主は料理と飲み物を振舞ってくれた。",
                        "The owner serves you food and drink."));

			(void)set_food(PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			if ((p_ptr->poisoned) || (p_ptr->cut))
			{
#ifdef JP
				msg_print("あなたに必要なのは部屋ではなく、治療者です。");
#else
				msg_print("You need a healer, not a room.");
#endif

				msg_print(NULL);
#ifdef JP
				msg_print("すみません、でもうちで誰かに死なれちゃ困りますんで。");
#else
				msg_print("Sorry, but don't want anyone dying in here.");
#endif
			}
			else
			{
				s32b oldturn = turn;
				int prev_day, prev_hour, prev_min;

				extract_day_hour_min(&prev_day, &prev_hour, &prev_min);
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "宿屋に泊まった。");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stayed at the inn.");
#endif
				turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);

				if(p_ptr->muta3 & MUT3_BYDO) p_ptr->bydo += turn - oldturn;
				if(p_ptr->bydo > 100000 * 3) p_ptr->bydo = 100000 * 3+1;

				//v1.1.59 サニーは眠ると蓄積した日光を失う
				sunny_charge_sunlight(-1*(SUNNY_CHARGE_SUNLIGHT_MAX));

				if (dungeon_turn < dungeon_turn_limit)
				{
					dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK * 250);
					if (dungeon_turn > dungeon_turn_limit) dungeon_turn = dungeon_turn_limit;
				}

				//v2.0.6 尤魔が得ている能力や耐性が宿に泊ったとき失われる処理
				if (p_ptr->pclass == CLASS_YUMA && (turn - oldturn) >= YUMA_FLAG_DELETE_TICK)
				{
					yuma_lose_extra_power((turn - oldturn) / YUMA_FLAG_DELETE_TICK);
				}

				prevent_turn_overflow();

				//宿に泊まって日付が変わるとき
				if ((prev_hour >= 18) && (prev_hour <= 23))
				{
					do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

					p_ptr->today_mon = 0; //霊夢の占いリセット

					//v2.05 はたての人探しリセット EXTRAでは日数経過でなくフロア通過でリセットされるのでここでは何もしない
					if (!EXTRA_MODE)
					{
						p_ptr->hatate_mon_search_ridx = 0;
						p_ptr->hatate_mon_search_dungeon = 0;

					}

				}

				p_ptr->chp = p_ptr->mhp;

				engineer_malfunction(0, 5000); //エンジニアの不調の機械が回復する

				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print(_("頭がスッキリした！でも何か忘れているような。",
                                                                        "Your head feels refreshed! But you feel like you've forgotten something."));

				if (ironman_nightmare && !diehardmind() && p_ptr->pclass != CLASS_DOREMY)
				{
#ifdef JP
					msg_print("眠りに就くと恐ろしい光景が心をよぎった。");
#else
					msg_print("Horrible visions flit through your mind as you sleep.");
#endif

					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					while(1)
					{
						have_nightmare(get_mon_num(MAX_DEPTH));

						if (!one_in_(3)) break;
					}

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);

#ifdef JP
					msg_print("あなたは絶叫して目を覚ました。");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "悪夢にうなされてよく眠れなかった。");
#else
					msg_print("You awake screaming.");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
				}
				else
				{
					///mod150920 ドレミー特殊処理
					if(p_ptr->pclass == CLASS_DOREMY)
					{
						if(ironman_nightmare)
							msg_print(_("宿泊客のみる夢はどれも極上の悪夢だ！",
                                        "All the other guests are having delicious nightmares!"));
						else
							msg_print(_("あなたは他の宿泊客の夢を美味しく頂いた。",
                                        "You enjoyed the delicious dreams of other guests"));
						(void)set_food(PY_FOOD_MAX - 1);

					}
					set_blind(0);
					set_confused(0);
					p_ptr->stun = 0;
					p_ptr->chp = p_ptr->mhp;
					p_ptr->csp = p_ptr->msp;
					if(p_ptr->pclass == CLASS_SEIJA || p_ptr->pclass == CLASS_MAGIC_EATER)
					{
						restore_seija_item();
					}

					//v1.1.86 宿屋に連続で泊まったらprocess_world()の6:00/18:00の処理を通らないことがあるのでここにも書いとく
					//v1.1.87　トレードカウントリセットを6:00と18:00の両方で行うことにしたので不要になった。
					//連続で宿屋に泊まったら高騰度セットが宿から出たときの一度だけになってしまうがまあ問題はないだろう。
					//月虹市場トレードカウントをリセット
					//ability_card_trade_count = 0;

					if ((prev_hour >= 6) && (prev_hour <= 17))
					{
						msg_print(_("あなたはリフレッシュして目覚め、夕方を迎えた。",
                                    "You wake up refreshed in the evening."));
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("夕方を迎えた。", "woke up in evening."));

					}
					else
					{
						msg_print(_("あなたはリフレッシュして目覚め、新たな日を迎えた。",
                                    "You wake up refreshed and welcome a new day."));
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("すがすがしい朝を迎えた。", "had a refreshing morning."));


					}

					set_asthma(p_ptr->asthma - 6000);
				}

				//v1.1.29 宿に泊まったら酔いが覚めることにした
				set_alcohol(0);
			}
			break;

		case BACT_RUMORS: /* Listen for rumors */
			{
				display_rumor(TRUE);
				break;
			}
	}

	return (TRUE);
}



//v1.1.85 一部クエストの人間側勢力/妖怪側勢力の分岐のために追加。外見が人間側勢力のときTRUE
//草の根妖怪ネットワークや羽衣婚活クエの人間側判定とは違うので注意
//「人間から見て人間側に見える」と「妖怪から見て人間側に見える」は微妙に違うので悩ましいが、とりあえず人里の一般人から見て仲間っぽければTRUE
//TODO:「人間側に見える」と「妖怪側に見える」を両方満たす種族やキャラもいるしそのうち妖怪側判定用の関数を別に作るべきかもしれん
bool	player_looks_human_side()
{

	//変身中
	//人間に化けている場合は通り、それ以外に化けているとき通らないことにしてみる。
	if (p_ptr->tim_mimic)
	{
		if (p_ptr->mimic_form == MIMIC_MARISA) return TRUE;
		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL)
		{
			monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

			if ((r_ptr->flags3 & (RF3_HUMAN)) && !(r_ptr->flags3 & (RF3_KWAI | RF3_DEMIHUMAN | RF3_DRAGON | RF3_DEMON | RF3_UNDEAD | RF3_ANIMAL | RF3_FAIRY | RF3_HANIWA))) return TRUE;

		}
		return FALSE;
	}

	//下の種族判定の例外 人里で味方だとして名が通ってそうな面々と避けられてそうな面々

	if (p_ptr->pclass == CLASS_BYAKUREN) return TRUE;
	if (p_ptr->pclass == CLASS_SHOU) return TRUE;
	if (p_ptr->pclass == CLASS_KEINE) return TRUE;
	if (p_ptr->pclass == CLASS_AUNN) return TRUE;
	if (p_ptr->pclass == CLASS_UDONGE) return TRUE;
	if (p_ptr->pclass == CLASS_TEWI) return TRUE;

	if (p_ptr->pclass == CLASS_HINA) return FALSE;
	if (p_ptr->pclass == CLASS_SHION) return FALSE;

	//種族による基本判定
	switch (p_ptr->prace)
	{
	case RACE_HUMAN:
	case RACE_KOBITO:
	case RACE_GREATER_LILLIPUT:
	case RACE_HALF_YOUKAI:
	case RACE_SENNIN:
	case RACE_TENNIN:
	case RACE_DEATH:
	case RACE_STRAYGOD:
	case RACE_DEMIGOD:
	case RACE_HALF_GHOST:
	case RACE_DEITY:
	case RACE_JK:
	case RACE_HOURAI:
	case RACE_ZASHIKIWARASHI:
	case RACE_MAGIC_JIZO:
		return TRUE;

	}

	return FALSE;

}





/*
 * Display quest information
 */
/*:::クエストの文章を表示する*/
static void get_questinfo(int questnum, bool do_init)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		quest_text[i][0] = '\0';
	}

	quest_text_line = 0;

	/* Set the quest number temporary */
	old_quest = p_ptr->inside_quest;
	p_ptr->inside_quest = questnum;

	/* Get the quest text */
	init_flags = INIT_SHOW_TEXT;
	if (do_init) init_flags |= INIT_ASSIGN;

	/*:::該当クエストの情報を解析。フラグがINIT_SHOW_TEXTなのでquest_text[][]のみ更新される？*/
	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	///mod140512 特殊ルーチン追加　＠が特定のユニークプレイヤーのときクエスト文章が不自然になるので書き換える
	(void) check_quest_unique_text();

	/* Reset the old quest number */
	p_ptr->inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "クエスト情報 (危険度: %d 階相当)", quest[questnum].level);
#else
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
#endif

	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, quest_text[i], i + 8, 0);
	}
}


/*:::京丸牡丹指定　クエスト用*/
static bool item_tester_hook_kyomaru(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_HEMATITE)
		return (TRUE);
	else
		return (FALSE);
}
/*:::ミスリル指定　クエスト用*/
static bool item_tester_hook_mithril(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_MITHRIL)
		return (TRUE);
	else
		return (FALSE);
}

/*:::焼いた鰻　クエスト用*/
static bool item_tester_hook_eel(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_EEL)
		return (TRUE);
	else
		return (FALSE);
}

/*:::特別性の羽衣　クエスト用*/
static bool item_tester_hook_special_hagoromo(object_type *o_ptr)
{
	if (o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_CLOAK_HAGOROMO && object_is_artifact(o_ptr) && !object_is_cursed(o_ptr))
		return (TRUE);
	else
		return (FALSE);
}

/*:::強い酒指定　クエスト用*/
static bool item_tester_hook_alcohol(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ALCOHOL && o_ptr->sval == SV_ALCOHOL_MARISA)
		return (FALSE);
	if (o_ptr->tval == TV_ALCOHOL && o_ptr->pval >  6000)
		return (TRUE);
	else
		return (FALSE);
}

/*:::輝夜五つの難題　クエスト用*/
///mod151219 全部達成したあとに金閣寺追加
static bool item_tester_hook_kaguya(object_type *o_ptr)
{
	if(kaguya_quest_level >= KAGUYA_QUEST_MAX)
	{
		if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_KINKAKUJI) return TRUE;
		return FALSE;
	}
	else
	{
		if(object_is_fixed_artifact(o_ptr) && o_ptr->name1 == kaguya_quest_art_list[kaguya_quest_level]) return (TRUE);
		return (FALSE);
	}
}

/*:::反獄王入りの酒　クエスト用*/
static bool item_tester_hook_hangoku(object_type *o_ptr)
{
	if (o_ptr->name1 == ART_HANGOKU_SAKE)
		return (TRUE);
	else
		return (FALSE);
}

/*:::アイテム探索クエストの対象アイテム確認ルーチン。
 *:::対象アイテムが複数の時や★処理などを考えるとq_info.txtを使うのは面倒なのでこの中にハードコーディングしてしまう。*/
bool check_quest_deliver_item(int q_index)
{
	cptr q, s;
	char o_name[MAX_NLEN];
	object_type *o_ptr;
	int item;

	if(q_index == QUEST_KYOMARU)
	{
		item_tester_hook = item_tester_hook_kyomaru;
	}
	else if(q_index == QUEST_MITHRIL)
	{
		item_tester_hook = item_tester_hook_mithril;
	}
	else if(q_index == QUEST_HANAMI)
	{
		item_tester_hook = item_tester_hook_alcohol;
	}
	else if(q_index == QUEST_KAGUYA)
	{
		item_tester_hook = item_tester_hook_kaguya;
	}
	else if (q_index == QUEST_DELIVER_EEL)
	{
		item_tester_hook = item_tester_hook_eel;
	}
	else if (q_index == QUEST_KONNKATSU)
	{
		item_tester_hook = item_tester_hook_special_hagoromo;
	}
	else if (q_index == QUEST_HANGOKU1)
	{
		item_tester_hook = item_tester_hook_hangoku;
	}

	else
	{
		msg_format(_("ERROR:check_quest_deliver_item()にクエストNo.(%d)が指定されていない",
                    "ERROR: Quest number (%d) isn't set in check_quest_deliver_item()"),q_index);
		return FALSE;
	}
	q = _("アイテムを選択してください。", "Choose an item.");
	s = _("あなたは求められたものを持って来ていない。", "You haven't brought what you were asked for.");

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return FALSE;
	o_ptr = &inventory[item];
	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if(o_ptr->number > 1)
	{
		if(!get_check(format(_("%sから一つ渡しますか？", "Hand over one of %s?"),o_name))) return FALSE;
	}
	else
	{
		if(!get_check(format(_("%sを渡しますか？", "Hand over %s?"),o_name))) return FALSE;
	}

	inven_item_increase(item, -1);
	inven_item_describe(item);
	inven_item_optimize(item);

	if(item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	return TRUE;

}



//ヤクザ戦争2クエストでターゲットを選ぶ
//選択したターゲットのr_idxを返す。キャンセルは0
//castle_quest()でクエスト受領文章が出ているときに実行され追加の選択肢を表示する
int	select_quest_yakuza2_target()
{
	char c;
	int i;
	int r_idx;

	bool flag_enoko = TRUE;
	bool flag_biten = TRUE;
	bool flag_chiyari = TRUE;

	//選択肢から自分に関係ある勢力を除外
	//新入り3人
	if (p_ptr->pclass == CLASS_ENOKO) flag_enoko = FALSE;
	if (p_ptr->pclass == CLASS_BITEN) flag_biten = FALSE;
	if (p_ptr->pclass == CLASS_CHIYARI) flag_chiyari = FALSE;

	//動物霊の所属
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA)  flag_enoko = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU)  flag_biten = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU)  flag_chiyari = FALSE;

	//動物霊憑依
	if (p_ptr->muta4 & (MUT4_GHOST_WOLF)) flag_enoko = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_OTTER)) flag_biten = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_EAGLE)) flag_chiyari = FALSE;

	//ヤクザクエスト1のときの選択肢
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA) flag_enoko = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU) flag_biten = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU) flag_chiyari = FALSE;

	//すでに倒していたら除外(過去バージョンで倒してからバージョンアップしたとき)
	if(r_info[MON_ENOKO].r_akills) flag_enoko = FALSE;
	if (r_info[MON_BITEN].r_akills) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].r_akills) flag_chiyari = FALSE;

	//ランクエ対象だったら除外
	//v2.0.13以降はこの三人はランクエ選定されない。過去バージョンでランクエ選定されて倒さずにバージョンアップしたときのための処理
	if (r_info[MON_ENOKO].flags1 & RF1_QUESTOR) flag_enoko = FALSE;
	if (r_info[MON_BITEN].flags1 & RF1_QUESTOR) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].flags1 & RF1_QUESTOR) flag_chiyari = FALSE;

	//ターゲットが残らない場合キャンセル
	if (!flag_enoko && !flag_biten && !flag_chiyari)
	{
		c_put_str(TERM_WHITE, _("しかしあなたの受けられそうな手配書は残っていなかった。", "However, there are no suitable wanted person posters left for you."), 15, 0);
		c_put_str(TERM_WHITE, _("このクエストを受領することはできないようだ。", "You are unable to accept this quest."), 16, 0);
		return 0;
	}

	//残ったターゲットを一覧表示
	c_put_str(TERM_WHITE, _("誰を狙いますか？(ESC:キャンセル)", "Which target do you pick? (ESC: cancel)"), 14, 2);
	if (flag_enoko)
		c_put_str(TERM_WHITE, _("a) 『三頭 慧ノ子』  ($50,000+腕力の薬)",   "a) Enoko Mitsugashira ($50,000 + Potion of Strength)"), 15, 5);
	if (flag_biten)
		c_put_str(TERM_WHITE, _("b) 『孫 美天』      ($60,000+知能の薬)",   "b) Son Biten          ($60,000 + Potion of Intelligence)"), 16, 5);
	if (flag_chiyari)
		c_put_str(TERM_WHITE, _("c) 『天火人 ちやり』($70,000+耐久力の薬)", "c) Chiyari Tenkajin   ($70,000 + Potion of Constitution)"), 17, 5);

	//ターゲットを選択
	//ターゲットが属する勢力から反感を買うフラグを記録
	while (TRUE)
	{
		r_idx = 0;
		c = inkey();

		if (c == ESCAPE)
		{
			clear_bldg(4, 18);
			return 0;
		}

		if (c == 'a' && flag_enoko)
		{
			r_idx = MON_ENOKO;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_KEIGA;
		}
		if (c == 'b' && flag_biten)
		{
			r_idx = MON_BITEN;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_KIKETSU;
		}
		if (c == 'c' && flag_chiyari)
		{
			r_idx = MON_CHIYARI;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_GOUYOKU;
		}

		if (r_idx) break;
	}

	//選択したモンスターに賞金首フラグを立て、すでに倒してたら復活させる
	r_info[r_idx].flags3 |= RF3_WANTED;
	if (!r_info[r_idx].max_num)
	{
		if (cheat_hear) msg_print(_("選択したモンスターが打倒済みなので復活させる", "The chosen monster was already defeated, and is now reborn"));
		r_info[r_idx].max_num = 1;
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, _("あなたは手配書の一枚を手に取った。", "You take one of the wanted person posters."), 5, 5);


	return r_idx;

}


/*
 * Request a quest from the Lord.
 */
/*:::建物内でのクエスト関連処理*/
static void castle_quest(void)
{
	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	/*:::クエストインデックスを得る。*/
	q_index = cave[py][px].special;

	//v2.0.13 未解決バグmemo
	//ヤクザ戦争2クエスト(QUEST_YAKUZA2)で選択した賞金首がランクエ対象でもあった場合、
	//ランクエフロアに行って該当モンスターを倒すと両方のクエストが完了状態になるのだが、
	//その後ヤクザ戦争2クエストの建物に来るとなぜかq_indexが0になって報酬生成処理に行かない。
	//ランクエフロア以外で倒したらちゃんと完了する。
	//何がどうなったらt0000014.txtに直打ちされた数字に影響が出るのか全く不明。
	//非常に不気味だがとりあえずランクエ選定済みの敵は賞金首に選択できないようにしておく
	if(cheat_xtra)msg_format("idx:%d", q_index);

	//v1.1.85
	// -Mega Hack- 鯢呑亭クエスト(人間用)を深夜に受けると鯢呑亭クエスト(妖怪用)になる処理
	if (q_index == QUEST_GEIDON_HUMAN && is_midnight()) q_index = QUEST_GEIDON_KWAI;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
put_str("今ここで受けられるクエストはないようだ。", 8, 0);
#else
		put_str("It doesn't look like you can receive a quest here right now.", 8, 0);
#endif

		return;
	}

	//v1.1.18以降、特定のクエストは特定種族やクラス以外を弾く
	switch (q_index)
	{
	case QUEST_TYPHOON:
	{
		bool flag_ng = TRUE;


		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL)
		{
			//v1.1.85 天狗に変身していると受けられるようにしてみる
			switch (p_ptr->metamor_r_idx)
			{
			case MON_KARASU_TENGU:
			case MON_HAKUROU_TENGU:
			case MON_HANA_TENGU:
			case MON_MOMIZI:
			case MON_AYA:
			case MON_TENGU_HAWK:
			case MON_G_TENGU:
				flag_ng = FALSE;

				break;
			default:
				break;

			}

		}
		else
		{
			if (p_ptr->prace == RACE_HAKUROU_TENGU) flag_ng = FALSE;
			if (p_ptr->prace == RACE_KARASU_TENGU) flag_ng = FALSE;
		}


		if (flag_ng)
		{
			put_str(_("このクエストは天狗しか受けられないようだ。",
                    "Only tengu can take on this request."), 8, 0);
			return;
		}
	}
	break;

	case QUEST_MIYOI:
	{
		if (p_ptr->pclass == CLASS_BYAKUREN)
		{
			put_str(_("あなたは戒律によりこのクエストに参加できない。",
                    "Your religion prohibits you from taking on this request."), 8, 0);
			return;
		}
	}
	break;


	//v1.1.32
	case QUEST_OKINA:
	{
		if (!r_info[MON_MAI].r_akills && !r_info[MON_SATONO].r_akills)
		{
			put_str(_("このクエストは「爾子田 里乃」か「丁礼田 舞」を倒さないと受けられないようだ。",
                    "You have to defeat Satono Nishida or Mai Teireida to take on this request."), 8, 0);
			return;
		}
	}
	break;
	//v1.1.65
	case QUEST_KONNKATSU:
	{
		bool flag_ng = TRUE;


		if ((RACE_TENTAN_EFFECT)) flag_ng = FALSE;
		if (p_ptr->mimic_form == MIMIC_MARISA) flag_ng = FALSE;

		//v1.1.85 人間に変身していると受けられるようにしてみる
		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL && r_info[p_ptr->metamor_r_idx].flags3 & RF3_HUMAN) flag_ng = FALSE;

		if(flag_ng)
		{
			put_str(_("このクエストは人間かそれに近い種族でないと受けられないようだ。",
                    "This request can be taken on only by humans or similar races."), 8, 0);
			return;
		}
	}
	break;

	case QUEST_SATORI:
	case QUEST_SHOOTINGSTAR:
	case QUEST_CRYPT:
	{
		if (p_ptr->pclass != CLASS_SATORI && (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH))
		{
			put_str(_("あなたはなぜか地霊殿の主と顔を合わせる気にならない。",
                    "For some reason, you just can't bring yourself to face the ruler of Palace of Earth Spirits."), 8, 8);
			return;
		}

	}
	break;

	//v1.1.85 鯢呑亭クエストは片方受領するともう片方を受領することはできなくなる
	case QUEST_GEIDON_HUMAN:
		if (p_ptr->pclass == CLASS_MIYOI)
		{
			put_str(_("店主には何か悩みがあるようだが、あなたに話してはくれなかった。",
                    "The owner seems to be troubled by something, but refuses to speak to you."), 8, 0);
			put_str(_("深夜にお客さんから聞いてみよう。",
                    "You can try asking the customers here at night."), 9, 0);
			return;
		}
		if (quest[QUEST_GEIDON_KWAI].status != QUEST_STATUS_UNTAKEN)
		{
			put_str(_("もうこの時間帯のクエストを受けることはできない。",
                    "You can no longer receive quests during this time."), 8, 0);
			return;
		}
		break;
	case QUEST_GEIDON_KWAI:
		if (quest[QUEST_GEIDON_HUMAN].status != QUEST_STATUS_UNTAKEN)
		{
			put_str(_("もうこの時間帯のクエストを受けることはできない。",
                    "You can no longer receive quests during this time."), 8, 0);
			return;
		}
		break;
	case QUEST_REIMU_ATTACK:

		//千亦はアビリティカードが報酬のクエストを受けられない。
		//check_ignoring_quest()を使うとアジトが半壊してダークエルフクエが受けられなくなるのでここで処理
		if (p_ptr->pclass == CLASS_CHIMATA)
		{
			put_str(_("今ここで受けられるクエストはないようだ。",
                    "It doesn't look like you can receive a quest here right now."), 8, 0);
			return;
		}
		break;



	}

	q_ptr = &quest[q_index];


	//v1.1.98 連続昏睡事件Ⅱのクエストをすでに怨霊に憑依された状態で受けると特殊フラグを立てる
	if (q_index == QUEST_HANGOKU2 && q_ptr->status == QUEST_STATUS_UNTAKEN && p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
	{
		p_ptr->quest_special_flag |= QUEST_SP_FLAG_HANGOKU2_POSSESSED;
		if (cheat_xtra) msg_print("SPECIAL FLAG ON");

	}

	//151124 v1.0.82bでうっかりquest53～.txtを入れ忘れてアップロード。クエスト受領に失敗し、quest[53]の内容が空のままQUEST_STATUS_TAKENになった。
	//それを修正するために該当状況でクエストパラメータを読み直す。
	if(q_index == QUEST_KAGUYA && q_ptr->status == QUEST_STATUS_TAKEN && q_ptr->type != QUEST_TYPE_DELIVER_ITEM)
	{
		get_questinfo(q_index, TRUE);
		return;
	}

	/* Quest is completed */
	/*:::クエスト終了なら報酬受け取り待ちにする*/
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index, FALSE);

		/*:::荒野再生成フラグ？　クエスト報酬はREWARDEDの状態で町が再生成されたとき置かれるらしい*/
		reinit_wilderness = TRUE;
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index, FALSE);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -3);
		set_deity_bias(DBIAS_COSMOS, -1);
		reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
		/*:::アイテム探索クエスト*/
		if(q_ptr->type == QUEST_TYPE_DELIVER_ITEM)
		{
			/*:::アイテムを持っていて渡せばここに入る。クエスト完了処理。*/
			if(check_quest_deliver_item(q_index))
			{

				//v1.1.70 クエスト達成時刻を正確にするため追加
				update_playtime();

				//v1.1.25 クエスト達成時刻追加
				q_ptr->comptime = playtime;

				q_ptr->complev = (byte)p_ptr->lev;
				q_ptr->status = QUEST_STATUS_REWARDED;
				get_questinfo(q_index, FALSE);
				reinit_wilderness = TRUE;

				// Hack - 輝夜クエスト達成の場合進行度を進める(いかさま、チートオプション除く)
				if(q_index == QUEST_KAGUYA && !(p_ptr->noscore) && p_ptr->pseikaku != SEIKAKU_MUNCHKIN && kaguya_quest_level < KAGUYA_QUEST_MAX)
					kaguya_quest_level++;
			}
			/*:::アイテムがないか渡すのを断った。クエスト放棄確認。放棄の場合失敗メッセージと失敗処理。*/
			///mod151112 輝夜クエストの場合放棄されない
			else if(q_index != QUEST_KAGUYA && get_check_strict(_("このクエストを放棄しますか？",
                                                                "Abandon this quest?"), CHECK_OKAY_CANCEL))
			{
				q_ptr->complev = (byte)p_ptr->lev;
				q_ptr->status = QUEST_STATUS_FAILED;
				get_questinfo(q_index, FALSE);
				if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -3);
				set_deity_bias(DBIAS_COSMOS, -1);
				q_ptr->status = QUEST_STATUS_FAILED_DONE;
				reinit_wilderness = TRUE;
			}

		}

		else
		{
			put_str(_("ここで受けたクエストをまだ終了させていない。",
                    "You haven't finished the quest you've taken here yet."), 8, 0);
			put_str(_("(CTRL-Qを使えばクエストの状態をチェックできます)",
                    "(You can check quest status with Ctrl-Q)"), 9, 0);
		}
	}

	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		q_ptr->status = QUEST_STATUS_TAKEN;
		///sys ここをFALSEにすればクエスト受領の時点では街の再描画がされなくなると思うが、何か問題出るのだろうか。
		reinit_wilderness = TRUE;

		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->r_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->r_idx = get_mon_num(q_ptr->level + 4 + randint1(6));
			}

			r_ptr = &r_info[q_ptr->r_idx];

			while ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->rarity != 1))
			{
				q_ptr->r_idx = get_mon_num(q_ptr->level) + 4 + randint1(6);
				r_ptr = &r_info[q_ptr->r_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = randint1(3) + 1;
			}

			q_ptr->cur_num = 0;
			name = (r_name + r_ptr->name);
#ifdef JP
msg_format("クエスト: %sを %d体倒す", name,q_ptr->max_num);
#else
			msg_format("Your quest: defeat %d %s", q_ptr->max_num, name);
#endif

		}
		//v2.0.13 ヤクザ戦争2クエストの特殊処理
		//クエスト文章を表示したあとどの賞金首を狙うか選択肢を表示し、選ばれたモンスターをquest[].r_idxに記録する。
		else if (q_index == QUEST_YAKUZA_2)
		{

			int tmp_r_idx;
			get_questinfo(q_index, TRUE);

			tmp_r_idx = select_quest_yakuza2_target();

			if (tmp_r_idx)
			{
				q_ptr->r_idx = tmp_r_idx;
			}
			else
			{
				//キャンセルした場合クエストを未受領に戻してまたクエスト文章と選択肢が出るようにする
				q_ptr->status = QUEST_STATUS_UNTAKEN;
			}
		}
		//通常のクエスト　クエスト文章表示
		else
		{
			get_questinfo(q_index, TRUE);
		}


	}
}


/*
 * Display town history
 */
/*:::町の概要を表示　図書館*/
static void town_history(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the building help file */
#ifdef JP
(void)show_file(TRUE, "tbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}

///mod141116 s16b→s32bへ変更
///mod150624 ↑がうっかりu32bになってて負のときオーバーフローしてたのでs32bに直した
s32b calc_expect_crit(int weight, int plus, int dam, s16b meichuu, bool dokubari)
{
	s32b i,k, num;
	int count;

	if(dokubari) return dam;

	//v1.1.69 潤美特技による重量ボーナス
	if (p_ptr->pclass == CLASS_URUMI && p_ptr->tim_general[0])
	{
		int old_weight = weight;
		weight = weight * p_ptr->lev * 8 / 50;
	}

	//v1.1.94 武器クリティカル率上昇と重量上限2000を反映
	weight = weight * 2 + p_ptr->lev * 2;
	if (weight > 2000) weight = 2000;



	i = weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3);
	num = 0;

	//会心のときの1d650を1~650まで全部ダメージ計算して足しあわせて最後に650で割ることで会心平均ダメージを計算している

	//v1.1.94
	//kをrandint1(weight)+randint1(650)にしたので同じような計算ができなくなった。
	//randint1(weight)が1から最大まで10%ずつ増やして全部合計するか。完全な数字ではなくなるがそれほど外れてはいないと思う
	for (count = 0; count <= 10; count++)
	{
		k = (weight+9)/10*count;
		if (k < 400)						num += (2 * dam + 5) * (400 - k);
		if (k < 700)						num += (2 * dam + 10) * (MIN(700, k + 650) - MAX(400, k));
		if (k >(700 - 650) && k < 900)		num += (3 * dam + 15) * (MIN(900, k + 650) - MAX(700, k));
		if (k >(900 - 650) && k < 1300)		num += (3 * dam + 20) * (MIN(1300, k + 650) - MAX(900, k));
		if (k >(1300 - 650))					num += (7 * dam / 2 + 25) * MIN(650, k - (1300 - 650));
	}
	num /= 650*11;

	/*
	k = weight;
	if (k < 400)						num += (2 * dam + 5) * (400 - k);
	if (k < 700)						num += (2 * dam + 10) * (MIN(700, k + 650) - MAX(400, k));
	if (k > (700 - 650) && k < 900)		num += (3 * dam + 15) * (MIN(900, k + 650) - MAX(700, k));
	if (k > (900 - 650) && k < 1300)		num += (3 * dam + 20) * (MIN(1300, k + 650) - MAX(900, k));
	if (k > (1300 - 650))					num += (7 * dam / 2 + 25) * MIN(650, k - (1300 - 650));
	num /= 650;

	*/

	if(p_ptr->pclass == CLASS_NINJA)
	{
		num *= i;
		num += (4444 - i) * dam;
		num /= 4444;
	}
	else
	{
		num *= i;
		num += (5000 - i) * dam;
		num /= 5000;
	}

	return num;
}

/*:::武器匠の武器比較サブルーチン4　スレイ計算*/
static s16b calc_slaydam(int dam, int mult, int div, bool force)
{
	int tmp;
	if(force)
	{
		tmp = dam * 60;
		tmp *= mult * 3;
		tmp /= div * 2;
		tmp += dam * 60 * 2;
		tmp /= 60;
	}
	else
	{
		tmp = dam * 60;
		tmp *= mult;
		tmp /= div;
		tmp /= 60;
	}
	return tmp;
}

/*
 * Display the damage figure of an object
 * (used by compare_weapon_aux1)
 *
 * Only accurate for the current weapon, because it includes
 * the current +dam of the player.
 */
/*:::武器匠の武器比較サブルーチン3 ダメージ表示*/
static void compare_weapon_aux2(int r, int c, int mindam, int maxdam, cptr attr, byte color)
{
	char tmp_str[80];


	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
	sprintf(tmp_str, "１ターン: %d-%d ダメージ",
#else
	sprintf(tmp_str, "1 turn: %d-%d damage",
#endif
			mindam, maxdam);

	/* Print the damage */
	put_str(tmp_str, r, c + 8);
}


/*
 * Show the damage figures for the various monster types
 *
 * Only accurate for the current weapon, because it includes
 * the current number of blows for the player.
 */
/*:::武器匠の武器比較のサブルーチン2 スレイ込み攻撃力表示*/
static void compare_weapon_aux1(object_type *o_ptr, int col, int r)
{
	u32b flgs[TR_FLAG_SIZE];
	int blow = p_ptr->num_blow[0];
	bool force = FALSE;
	bool dokubari = FALSE;

	/* Effective dices */
	int eff_dd = o_ptr->dd + p_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + p_ptr->to_ds[0];

	int mindice = eff_dd;
	int maxdice = eff_ds * eff_dd;
	int mindam = 0;
	int maxdam = 0;
	int slaydice_min = 0;
	int slaydice_max = 0;
	int critdice_min = 0;
	int critdice_max = 0;
	int vorpal_mult = 1;
	int vorpal_div = 1;



	/* Get the flags of the weapon */
	object_flags(o_ptr, flgs);

	if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;


	/* Show Critical Damage*/
	critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, mindice, p_ptr->to_h[0], dokubari);
	critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, maxdice, p_ptr->to_h[0], dokubari);

	mindam = blow * (critdice_min+ o_ptr->to_d + p_ptr->to_d[0]);
	maxdam = blow * (critdice_max+ o_ptr->to_d + p_ptr->to_d[0]);
	if(mindam<0) mindam=0;
	if(maxdam<0) maxdam=0;

#ifdef JP
	compare_weapon_aux2(r++, col, mindam, maxdam, "会心:", TERM_L_RED);
#else
	compare_weapon_aux2(r++, col, mindam, maxdam, "Critical:", TERM_L_RED);
#endif

	/* Vorpal Hit*/
	///mod151101 *切れ味*のみフラグがついていたとき(鍛冶など)に対象となるよう条件式追加
	if ((have_flag(flgs, TR_VORPAL) || have_flag(flgs, TR_EX_VORPAL) || hex_spelling(HEX_RUNESWORD)))
	{
		if(have_flag(flgs, TR_EX_VORPAL))
		{
			vorpal_mult = 5;
			vorpal_div = 3;
		}
		else
		{
			vorpal_mult = 11;
			vorpal_div = 9;
		}

		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, mindice, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, maxdice, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;

		#ifdef JP
			compare_weapon_aux2(r++, col, mindam, maxdam, "切れ味:", TERM_L_RED);
		#else
			compare_weapon_aux2(r++, col, mindam, maxdam, "Vorpal:", TERM_L_RED);
		#endif
	}

	if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && have_flag(flgs, TR_FORCE_WEAPON) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
	{
		force = TRUE;

		slaydice_min = calc_slaydam(mindice, 1, 1, force);
		slaydice_max = calc_slaydam(maxdice, 1, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;

		/* Print the relevant lines */
#ifdef JP
		compare_weapon_aux2(r++, col, mindam, maxdam, "理力:", TERM_L_BLUE);
#else
		compare_weapon_aux2(r++, col, mindam, maxdam, "Force  :", TERM_L_BLUE);
#endif
	}

	/* Print the relevant lines */
	if (have_flag(flgs, TR_KILL_EVIL))
	{
		slaydice_min = calc_slaydam(mindice, 7, 2, force);
		slaydice_max = calc_slaydam(maxdice, 7, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("混沌勢力:", "Chaotic:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_EVIL))
	{
		slaydice_min = calc_slaydam(mindice, 2, 1, force);
		slaydice_max = calc_slaydam(maxdice, 2, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("混沌勢力:", "Chaotic:"), TERM_YELLOW);
	}

	if (have_flag(flgs, TR_KILL_GOOD))
	{
		slaydice_min = calc_slaydam(mindice, 7, 2, force);
		slaydice_max = calc_slaydam(maxdice, 7, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("秩序勢力:", "Lawful:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_GOOD))
	{
		slaydice_min = calc_slaydam(mindice, 2, 1, force);
		slaydice_max = calc_slaydam(maxdice, 2, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("秩序勢力:", "Lawful:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_HUMAN))
	{
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("人間:", "Human:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_HUMAN))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("人間:", "Human:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_ANIMAL))
	{
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("動物:", "Animal:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_ANIMAL))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("動物:", "Animal:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_UNDEAD))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("不死:", "Undead:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_UNDEAD))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("不死:", "Undead:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_DEMON))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("悪魔:", "Demon:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DEMON))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("悪魔:", "Demon:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_DEITY))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("神格:", "Deity:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DEITY))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("神格:", "Deity:"), TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_KWAI))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("妖怪:", "Youkai:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_KWAI))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("妖怪:", "Youkai:"), TERM_YELLOW);
	}

	if (have_flag(flgs, TR_KILL_DRAGON))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("竜:", "Dragon:"), TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DRAGON))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("竜:", "Dragon:"), TERM_YELLOW);
	}
	if (o_ptr->name1 == ART_KRONOS)
	{
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("男:", "Male:"), TERM_YELLOW);
	}


	if (have_flag(flgs, TR_BRAND_ACID))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("酸属性:", "Acid:"), TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_ELEC))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("電属性:", "Elec:"), TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_FIRE))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("炎属性:", "Fire:"), TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_COLD))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("冷属性:", "Cold:"), TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_POIS))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0;
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, _("毒属性:", "Pois:"), TERM_RED);
	}


}

static int hit_chance(int to_h, int ac)
{
	int chance = 0;
	int meichuu = p_ptr->skill_thn + (p_ptr->to_h[0] + to_h) * BTH_PLUS_ADJ;

	if (meichuu <= 0) return 5;

	chance = 100 - ((ac * 75) / meichuu);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
//	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)	chance = (chance*19+9)/20;
	return chance;
}



/*::: -Mega Hack- ＠が特定のクラスや種族の時、特定のクエストの文章を不自然にならないように変更する。*/
bool check_quest_unique_text(void)
{
	int qnum = p_ptr->inside_quest;
	bool comp = FALSE;
	bool fail = FALSE;
	bool accept = FALSE;
	int pc = p_ptr->pclass;
	int pr = p_ptr->prace;
	int line = 0;

	if(quest[qnum].status == QUEST_STATUS_FAILED) fail = TRUE;
	else if(quest[qnum].status == QUEST_STATUS_REWARDED) comp = TRUE;
	else accept = TRUE;

	switch(qnum)
	{
		//盗賊のアジト
	case QUEST_HOME1:
		if(pc == CLASS_BANKI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「あら貴女は・・丁度良いところに来てくれました。");
				strcpy(quest_text[line++], "外れの空き家をご存知ですよね？");
				strcpy(quest_text[line++], "あそこが見覚えのないならず者の溜まり場になっているのです。");
				strcpy(quest_text[line++], "彼らは里で盗みを働いているようです。大事になる前に追い出して下さい。");
				strcpy(quest_text[line++], "普通の人間たちです。貴女なら問題ないでしょう・・」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Oh, you... You came at the right time.");
				strcpy(quest_text[line++], "You know there's a vacant house on the outskirts?");
				strcpy(quest_text[line++], "It has became a bandit hideout. They keep robbing our village.");
				strcpy(quest_text[line++], "Please deal with them before it gets out of hand.");
				strcpy(quest_text[line++], "They're normal humans. You shouldn't have much trouble...'");
#endif
			}

		}
		else if(pc == CLASS_SEIJA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「……外れの空き家に大切な客人をお招きしています。");
				strcpy(quest_text[line++], "いいですか、絶対に嫌がらせをしてはだめですよ？」");
#else
                strcpy(quest_text[line++], "Akyuu - '...I've invited some precious guests into the vacant");
				strcpy(quest_text[line++], "house on the outskirts. I'd like you to not cause any trouble");
				strcpy(quest_text[line++], "for them. Understood?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「あらあらまあまあ、なんて乱暴なことをなさるのでしょう。");
				strcpy(quest_text[line++], "どうか二度とあの家に近寄らないでください。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Oh, my, you're so violent. Please don't approach that");
				strcpy(quest_text[line++], "house ever again.'");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「閻魔様！？何のご用でしょうか、まさか・・");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "ああ成程、今は休暇で異変調査中なのですか。これは正に天の助けです。");
				strcpy(quest_text[line++], "実は外界のならず者が外れの空き家を根城に盗みを働いているのです。");
				strcpy(quest_text[line++], "どうか助けて頂けませんか？」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Miss Yama!? What are you doing here, it can't be...");
				strcpy(quest_text[line++], "  ");
				strcpy(quest_text[line++], "Ah, you're on a vacation and investigating this incident?");
				strcpy(quest_text[line++], "That's a great help. Truth to be told, some bandits from the");
				strcpy(quest_text[line++], "other world have made a vacant house into their hideout. Couldn't");
				strcpy(quest_text[line++], "you help us?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「有難うございました。");
				strcpy(quest_text[line++], "しかし最近里の周りが物騒で監視の目が足りていないのです。");
				strcpy(quest_text[line++], "できれば異変の間だけでもあの空き家を管理して頂けませんか？");
				strcpy(quest_text[line++], "物置としてでも使ってくだされば十分ですので。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Thank you very much.");
				strcpy(quest_text[line++], "However, lately there has been a lot of turmoil around the village,");
				strcpy(quest_text[line++], "and we can't properly monitor the situation. Could you manage that");
				strcpy(quest_text[line++], "vacant house during this incident? There's enough space to use it as a");
				strcpy(quest_text[line++], "storeroom.'");
#endif
			}
		}
		//v1.1.37
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「あら小鈴じゃない。随分大荷物だけど貸本回収の帰りなの？");
				strcpy(quest_text[line++], "ところで、外れの空き家にならず者が住み着いたと報告があったわ。");
				strcpy(quest_text[line++], "対処が終わるまで絶対に近付かないようにね。いい？」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Oh, it's you, Kosuzu. That's a large bag you're carrying;");
				strcpy(quest_text[line++], "are you collecting the books you've rented? By the way, I've heard");
				strcpy(quest_text[line++], "there are bandits living in the vacant house on the outskirts.");
				strcpy(quest_text[line++], "Don't approach it until we've dealt with it. Understood?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「小鈴が退治した…ですって？");
				strcpy(quest_text[line++], "無事だったからいいようなものの、そんなに無茶しなくていいのに。");
				strcpy(quest_text[line++], "あの空き家？引き取り手が決まるまで倉庫にでも使っていいわよ。");
#else
                strcpy(quest_text[line++], "Akyuu - 'You... cleaned them out?");
				strcpy(quest_text[line++], "I'm glad you're okay, but you shouldn't be so reckless.");
				strcpy(quest_text[line++], "That vacant house? Feel free to use it as a storeroom until someone");
				strcpy(quest_text[line++], "settles in.'");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「あらこんな所に妖精が。");
				strcpy(quest_text[line++], "いい子だから外で遊んでなさい。");
				strcpy(quest_text[line++], "外れの空き家には怖い人がいるから近寄っちゃ駄目よ。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Oh, a fairy? Be a good kid and play outside.");
				strcpy(quest_text[line++], "There are scary people in the vacant house on the outskirts,");
				strcpy(quest_text[line++], "so stay away from there.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「あのならず者を追い出したですって？驚いたわね。");
				strcpy(quest_text[line++], "ご褒美にあの空き家をしばらく貸してあげる。");
				strcpy(quest_text[line++], "秘密基地にでも使いなさい。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'You drove those bandits away? I'm surprised.");
				strcpy(quest_text[line++], "As a reward, I'm letting you use that house for a while.");
				strcpy(quest_text[line++], "Feel free to set up your secret base.'");
#endif
			}
		}

		break;
		//オークのキャンプ
	case QUEST_ORC:
		if(pc == CLASS_KEINE)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "オークと呼ばれる凶暴な獣人が武装して西の洞窟に集まっている。");
				strcpy(quest_text[line++], "放置しておくと里が脅かされるかもしれない。今のうちに叩いておこう。");
				strcpy(quest_text[line++], "オーク一体一体は大して強くないようだが数が多く、");
				strcpy(quest_text[line++], "それに力強いリーダー格のオークがいる。油断は禁物だ。");
#else
                strcpy(quest_text[line++], "There are violent beastman called orcs gathering in a cave to the west.");
				strcpy(quest_text[line++], "They might end up threatening the village if you let them be. Better");
				strcpy(quest_text[line++], "deal with them right now. Orcs aren't that strong on their own, but");
				strcpy(quest_text[line++], "there's a lot of them, and they have a powerful leader. You have");
				strcpy(quest_text[line++], "to exercise caution.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "オーク達は散り散りに逃げ去っていった。当分は近寄ってこないだろう。");
				strcpy(quest_text[line++], "稗田家の者がお礼の品を届けてくれた。");
				strcpy(quest_text[line++], "いわゆる隠れ蓑だ。これを纏えば洞窟探索が捗るだろう。");
#else
                strcpy(quest_text[line++], "The orcs have ran away and probably aren't coming back any time soon.");
				strcpy(quest_text[line++], "You've received a reward from the Hieda House.");
				strcpy(quest_text[line++], "It's a cloak of concealment that'll come in use during dungeon exploration.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "オーク達は思った以上に強く、不覚を取ってしまった。");
				strcpy(quest_text[line++], "幸いな事に、里に来ていた妹紅が話を聞いてオークを全員吹き飛ばしてくれた。");
#else
                strcpy(quest_text[line++], "The orcs were tougher than you expected, taking you by surprise.");
				strcpy(quest_text[line++], "Fortunately, you talked to Mokou in the village, and she blew them all away.");
#endif
			}

		}
		else if(pc == CLASS_EIKI)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「何と、閻魔様ではないですか。今回の異変の調査中なのですね。");
				strcpy(quest_text[line++], "実は異界の凶暴な獣人らしき連中が近くの洞窟に棲み着いております。");
				strcpy(quest_text[line++], "よろしければ退治に力を貸していただけませんか？」");
#else
                strcpy(quest_text[line++], "Keine - 'Oh, Miss Yama? I assume you're investigating the incident.");
				strcpy(quest_text[line++], "Truth to be told, some violent beastmen from other world are living in a");
				strcpy(quest_text[line++], "nearby cave. Could you lend us a hand and deal with them?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「お力添えありがとうございました。");
				strcpy(quest_text[line++], "これを今後の調査にお役立てください。」");
#else
                strcpy(quest_text[line++], "Keine - 'Thank you for your support.");
				strcpy(quest_text[line++], "Take this, it will help you in your quest.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「・・閻魔様に頼らず独力で火の粉を払えというお心なのですね。");
				strcpy(quest_text[line++], "これより討伐隊を組織いたします。」");
#else
                strcpy(quest_text[line++], "Keine - '...Very well, we'll handle it by ourselves without relying");
				strcpy(quest_text[line++], "on you, Miss Yama. I'm going to organise a subjugation team.'");
#endif
			}

		}
		else if(pc == CLASS_KOSUZU)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「おや小鈴か。物々しい格好でどうした？");
				strcpy(quest_text[line++], "ああ、言っておくことがあった。");
				strcpy(quest_text[line++], "いま里の近くにオークという凶暴な亜人が寄り集まっているんだ。");
				strcpy(quest_text[line++], "危険だから護衛なしでは里から離れないようにな。」");
#else
                strcpy(quest_text[line++], "Keine - 'Oh, it's you, Kosuzu. Is anything wrong?");
				strcpy(quest_text[line++], "Ah, I have to tell you something.");
				strcpy(quest_text[line++], "There are humanoids called orcs gathering near the village.");
				strcpy(quest_text[line++], "It's dangerous, so don't go outside on your own.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「何、オークを全て追い散らしてきただと？");
				strcpy(quest_text[line++], "驚いたな。いつの間にそこまでの使い手になったのやら。");
				strcpy(quest_text[line++], "ともあれ礼としてこの隠れ蓑を受け取ってくれ。");
#else
                strcpy(quest_text[line++], "Keine - 'What, you drove them all away? I'm surprised.");
				strcpy(quest_text[line++], "Since when you've became that skilled? Very well, take this");
				strcpy(quest_text[line++], "cloak of concealment as your reward.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「よしよし、怪我はないか？");
				strcpy(quest_text[line++], "怖がりなくせに無鉄砲なところは昔から変わらないな。」");
#else
                strcpy(quest_text[line++], "Keine - 'Alright, are you hurt?");
				strcpy(quest_text[line++], "I see you're still fearless to the point of recklessness.'");
#endif
			}

		}
		else if(pc == CLASS_MOKOU)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "寺子屋に行くと慧音に頼み事をされた。");
				strcpy(quest_text[line++], "オークとかいう凶暴な亜人が近くの洞窟に屯しているらしい。");
				strcpy(quest_text[line++], "最近体も鈍っているし軽い運動をするのもいいだろう。");
#else
				strcpy(quest_text[line++], "Keine gave you a request at the temple school.");
				strcpy(quest_text[line++], "It's about dealing with some violent humanoid called orcs");
				strcpy(quest_text[line++], "that are living in a nearby cave. You feel your skills are");
				strcpy(quest_text[line++], "getting somewhat dull, so it'll be good exercise.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "オークを追っ払って寺子屋に報告に戻ると、慧音が礼の品をくれた。");
				strcpy(quest_text[line++], "敵から身を隠す隠れ蓑らしい。");
				strcpy(quest_text[line++], "別に隠れる必要もないのだが、慧音なりの優しさなのだろうか。");
#else
                strcpy(quest_text[line++], "You drove the orcs away and came back to the temple school;");
				strcpy(quest_text[line++], "Keine gave you a reward. It's a cloak that conceal your presence");
				strcpy(quest_text[line++], "from enemies. While you don't feel the need to hide, you");
				strcpy(quest_text[line++], "understand Keine's being kind.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「貴方が後れを取るほどの敵にはとても見えませんでしたが。");
				strcpy(quest_text[line++], "まさか体調でもお悪いのですか？」");
#else
                strcpy(quest_text[line++], "Keine - 'I don't recall seeing many enemies that could beat you down.");
				strcpy(quest_text[line++], "Are you really alright?'");
#endif
			}

		}
		else if(pc == CLASS_REIMU)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「博麗の巫女が勝てないような相手には見えなかったが。");
				strcpy(quest_text[line++], "こう言っては悪いが、少したるんでいるのではないか？」");
#else
                strcpy(quest_text[line++], "Keine - 'I haven't seen any enemies that could match the Hakurei");
				strcpy(quest_text[line++], "shrine maiden. It might be rude of me to say this, but aren't");
				strcpy(quest_text[line++], "you slacking off?'");
#endif
			}

		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「妖精か。西の洞窟には近寄らない方がいいぞ。");
				strcpy(quest_text[line++], "凶暴な亜人が根城にしていて危険なんだ。近々討伐隊を組織しようと考えている。");
				strcpy(quest_text[line++], "それまであの辺では遊ぶなよ。」");
#else
                strcpy(quest_text[line++], "Keine - 'A fairy? Stay away from the cave in the west.");
				strcpy(quest_text[line++], "There are violent humanoids living here, it's dangerous. I'm");
				strcpy(quest_text[line++], "thinking of organizing a subjugation team to deal with them.");
				strcpy(quest_text[line++], "Don't play around there until we're finished.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「何、オークを全て追い散らしてきただと？");
				strcpy(quest_text[line++], "最近は妖精といえども馬鹿にできないな。");
				strcpy(quest_text[line++], "お礼にこれをあげよう。里で悪戯に使うんじゃないぞ？」");
#else
                strcpy(quest_text[line++], "Keine - 'Oh, you drove the orcs away?");
				strcpy(quest_text[line++], "You can't underestimate even fairies these days.");
				strcpy(quest_text[line++], "Take this as your reward. Just don't use it to pull pranks");
				strcpy(quest_text[line++], "in the village, okay?'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「そら言わないことじゃない。");
				strcpy(quest_text[line++], "怪我はないか？」");
#else
                strcpy(quest_text[line++], "Keine - 'Don't tell me I haven't warned you.");
				strcpy(quest_text[line++], "Are you hurt?'");
#endif
			}
		}

		break;

		//ログルス使い+α　椛、鴉天狗、白狼天狗
	case QUEST_HOME2:
		if(pc == CLASS_MOMIZI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "文「ふうむ、あっさり済ませてしまいましたか。");
				strcpy(quest_text[line++], "　実はちょっと期・・いえいえ心配してましたが余計だったようですね。");
				strcpy(quest_text[line++], "　あの空き家はみんな忘れることにしたようですし、物置にでも使えばどうです？");
				strcpy(quest_text[line++], "　この程度は役得ってものですよ。");
#else
                strcpy(quest_text[line++], "Aya - 'Hmm, you cleaned them up pretty quickly.");
				strcpy(quest_text[line++], " I was getting a bit worried... though looks like it was for nothing.");
				strcpy(quest_text[line++], " How about we all forget about that house, and you'll use it as a ");
				strcpy(quest_text[line++], " storeroom? It'll be pretty handy.'");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "文「あら椛その戦支度はどうしたんです？");
				strcpy(quest_text[line++], "　ははあ、例の空き家の余所者の件ですね？");
				strcpy(quest_text[line++], "　何だかんだ言ってもあの件は白狼天狗の管轄ですからねぇ。");
				strcpy(quest_text[line++], "　さっさと始末して幕引きを図ろうと密命を受けたってところですか。");
				strcpy(quest_text[line++], "　・・そう睨まないで下さいよ。ご武運をお祈りします。");
				strcpy(quest_text[line++], "　失敗してもスクープしたりしないかもしれないからご安心くださいな。」");
#else
                strcpy(quest_text[line++], "Aya - 'Hey, Momiji, are you preparing to fight?");
				strcpy(quest_text[line++], " Do you know about those strangers in the vacant house?");
				strcpy(quest_text[line++], " You know, this falls into white wolf tengu jurisdiction.");
				strcpy(quest_text[line++], " Did you receive secret orders to clean them up?");
				strcpy(quest_text[line++], " ...Please don't look at me like that. I'm wishing you good luck.");
				strcpy(quest_text[line++], " Don't worry, even if you fail, that'll be fine material");
				strcpy(quest_text[line++], " for my newspaper.'");
#endif
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "文「おやおや小鈴さんじゃないですか。");
				strcpy(quest_text[line++], "先日申し上げましたが、ここでは鈴奈庵の小鈴さんとしては扱いませんよ。");
				strcpy(quest_text[line++], "ちょっと頼みがあるのですが、外れの空き家を訪ねてくれませんか？");
				strcpy(quest_text[line++], "まあ面倒な手続きの一環というか、我々以外の手が必要なのですよ。");
#else
                strcpy(quest_text[line++], "Aya - 'Oh, it's you, Kosuzu?");
				strcpy(quest_text[line++], "I've said it before, I'm not going to treat you as Kosuzu from");
				strcpy(quest_text[line++], "Suzunaan here. I have a little request - can't you take a look at");
				strcpy(quest_text[line++], "the vacant house on the outskirts? Due to some bothersome procedures,");
				strcpy(quest_text[line++], "we need outside help.'");
#endif
			}
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "文「いやはや驚きましたね。");
				strcpy(quest_text[line++], "騒ぎになったところを踏み込むつもりでしたが、");
				strcpy(quest_text[line++], "まさか貴方が全部倒してしまうとは。");
				strcpy(quest_text[line++], "ああ、あの家は差し上げましょう。自由に使ってください。");
				strcpy(quest_text[line++], "いずれ人里を統べるかもしれない人間にはこれくらい当たり前ですよ…」");
#else
                strcpy(quest_text[line++], "Aya - 'I'm surprised.");
				strcpy(quest_text[line++], "It was about to become quite the mess, and then you've");
				strcpy(quest_text[line++], "cleaned them all up.");
				strcpy(quest_text[line++], "I'm letting you have that house. Feel free to use it as you want.");
				strcpy(quest_text[line++], "It's only fitting for a human who might lead the Human Village at");
				strcpy(quest_text[line++], "some point...'");
#endif
			}
		}
		else if(pc == CLASS_SEIJA)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "首尾よく家を奪ってやった。余所者も天狗の連中もいい気味だ。");
#else
                strcpy(quest_text[line++], "You've successfully taken the house for yourself. It feels good");
                strcpy(quest_text[line++], "getting on the nerves of both those strangers and tengu.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "余所者が空き家に棲み着いて妙なしがらみで手が出せないらしい。");
				strcpy(quest_text[line++], "いっそ自分がその余所者になり代わってあの家を奪ってやろう。");
#else
                strcpy(quest_text[line++], "Looks like some strangers have settled in a vacant house,");
				strcpy(quest_text[line++], "and the tengu somehow have their hands tied. Well, it's time");
				strcpy(quest_text[line++], "to steal that house from the strangers for your own use.");
#endif
			}
		}
		else if(pc == CLASS_YUGI || pc == CLASS_SUIKA)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "　戻ってくると文は消えていた。");
				strcpy(quest_text[line++], "　とりあえず件の空き家は拠点として接収することにした。");
#else
                strcpy(quest_text[line++], " As you return, Aya's not here.");
				strcpy(quest_text[line++], " In any case, you take that vacant house as your exploration");
				strcpy(quest_text[line++], " base.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "文「あや・・ど、どうも。");
				strcpy(quest_text[line++], "　里が騒がしい？いえいえそんな滅相もない。");
				strcpy(quest_text[line++], "　お山は至って平和でございますよ。");
				strcpy(quest_text[line++], "　怪しい余所者が空き家を占拠したり、");
				strcpy(quest_text[line++], "　そいつらそっちのけで責任押し付けあったりなんて、");
				strcpy(quest_text[line++], "　ぜーんぜんしておりません。」");
#else
				strcpy(quest_text[line++], "Aya - 'Aya... Oh, nice to meet you.");
				strcpy(quest_text[line++], " The village is in turmoil? No, it's nothing like that.");
				strcpy(quest_text[line++], " Everything's peaceful at the mountain.");
				strcpy(quest_text[line++], " There's no suspicious strangers settling in vacant");
				strcpy(quest_text[line++], " houses, and we tengu don't keep pushing the responsibility");
				strcpy(quest_text[line++], " for that on each other.'");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "　戻ってくると射命丸は消えていた。");
				strcpy(quest_text[line++], "　とりあえず件の空き家は拠点として接収することにした。");
#else
                strcpy(quest_text[line++], " As you return, Aya's not here.");
				strcpy(quest_text[line++], " In any case, you take that vacant house as your exploration");
				strcpy(quest_text[line++], " base.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "文「これはこれは、閻魔様ではありませんか・・");
				strcpy(quest_text[line++], "　里が騒がしい？いえいえそんな滅相もない。");
				strcpy(quest_text[line++], "　お山は至って平和でございますよ。");
				strcpy(quest_text[line++], "　怪しい余所者が南の空き家を占拠したり、");
				strcpy(quest_text[line++], "　そいつらそっちのけで責任押し付けあったりなんて、");
				strcpy(quest_text[line++], "　ぜーんぜんしておりません。」");
#else
                strcpy(quest_text[line++], "Aya - 'Oh my, it's Miss Yama...");
				strcpy(quest_text[line++], " The village is in turmoil? No, it's nothing like that.");
				strcpy(quest_text[line++], " Everything's peaceful at the mountain.");
				strcpy(quest_text[line++], " There's no suspicous strangers settling in the vacant");
				strcpy(quest_text[line++], " house to the south, and we tengu don't keep pushing the");
				strcpy(quest_text[line++], " responsibility for that on each other.'");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "文「なんと、事もあろうに妖精に出し抜かれるとは。");
				strcpy(quest_text[line++], "いやはや天狗の威厳も何もあったもんじゃありませんねえ。");
				strcpy(quest_text[line++], "あの家は当分好きに使っていいですよ。");
				strcpy(quest_text[line++], "空き家のままにしておくよりは幾らかましでしょう。」");
#else
                strcpy(quest_text[line++], "Aya - 'Well, looks like I was overtaken by a fairy.");
				strcpy(quest_text[line++], "No, it's not about tengu pride or anything.");
				strcpy(quest_text[line++], "Use that house as you like.");
				strcpy(quest_text[line++], "It's better than leaving it vacant.'");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "文「あやや、ここは貴方のような妖精には危険ですよ。");
				strcpy(quest_text[line++], "特に今は里にちょっとした面倒事が起こっているのですから。");
				strcpy(quest_text[line++], "捕まって怖い目に遭う前に家に帰りなさい。」");
#else
                strcpy(quest_text[line++], "Aya - 'Ayaya, it's dangerous here for a fairy like you.");
				strcpy(quest_text[line++], "There's a particularly nasty incident going on here");
				strcpy(quest_text[line++], "right now. Better go back before you get in trouble.'");
#endif
			}
		}

		else if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU)
		{
			//v2.0.5 建物「はたての家」を追加したのではたてのときセリフを変えておく
			if (p_ptr->pclass == CLASS_HATATE)
			{
				if (comp)
				{
#ifdef JP
					strcpy(quest_text[line++], "侵入者に突撃取材を敢行し、文の家を取り戻した。");
					strcpy(quest_text[line++], "文の悔しがる顔が目に浮かぶようだ。");
#else
					strcpy(quest_text[line++], "You've driven the intruders out of Aya's house.");
					strcpy(quest_text[line++], "You can picture the look of frustration on her face.");
#endif
				}
				else if (fail)
				{
					msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "しばらく留守にしている文の家に余所者が住み着いていた。");
					strcpy(quest_text[line++], "余所者は夜な夜な怪しい儀式を繰り返し、近所に被害が出ているようだ。");
					strcpy(quest_text[line++], "これは面白くなってきた。");
#else
                    strcpy(quest_text[line++], "While you were away, some strangers have settled in Aya's house.");
                    strcpy(quest_text[line++], "They're performing suspicious rituals at night and harm those who get");
                    strcpy(quest_text[line++], "too close. This is getting interesting.");
#endif
				}

			}
			else
			{
                if(comp)
                {
#ifdef JP
                    strcpy(quest_text[line++], "首尾良く侵入者を片付け、我が家を取り戻した。");
                    strcpy(quest_text[line++], "危うくゴシップのネタになるところだったが、とりあえず面子は保ったようだ。");
#else
                    strcpy(quest_text[line++], "You've cleaned up the intruders and got your house back.");
                    strcpy(quest_text[line++], "It was about to become a gossip story, but you've managed to save your face.");
#endif

                }
                else if(fail)
                {
                    msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
                }
                else
                {
#ifdef JP
                    strcpy(quest_text[line++], "しばらく留守にしているうちに家に余所者が住み着いていた。");
                    strcpy(quest_text[line++], "余所者は夜な夜な怪しい儀式を繰り返し、近所に被害が出ているようだ。");
                    strcpy(quest_text[line++], "速やかに一人で始末をつけないと天狗の誇りに関わる。");
#else
                    strcpy(quest_text[line++], "While you were away, some strangers have settled in your house.");
                    strcpy(quest_text[line++], "They're performing suspicious rituals at night and harm those who get");
                    strcpy(quest_text[line++], "too close. Your tengu pride tells you to quickly deal with it");
                    strcpy(quest_text[line++], "by yourself.");
#endif
                }
			}


		}
		break;

	case QUEST_HOME3: //幽霊屋敷β　さとり
		if(pc == CLASS_SATORI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "見覚えのない連中だったが、ともかく全ての怨霊を懲らしめた。");
				strcpy(quest_text[line++], "空き家のままではまた妙なのが出るかもしれない。");//一行40字まで
				strcpy(quest_text[line++], "物置にでも使わせてもらおう。表立っては誰からも文句は出なかった。");//一行40字まで
#else
                strcpy(quest_text[line++], "You don't recall seeing those spirits before, but you've disciplined");
				strcpy(quest_text[line++], "them all. Something else weird might come up if you leave the");//一行40字まで
				strcpy(quest_text[line++], "house vacant, so you've decided to use it as a storeroom.");//一行40字まで
				strcpy(quest_text[line++], "Nobody's complaining.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "家の隣の空き家に奇妙な怨霊が屯しているらしい。");
				strcpy(quest_text[line++], "怨霊の管理は自分の役割だ。");
				strcpy(quest_text[line++], "様子を見に行ってみよう。");
#else
                strcpy(quest_text[line++], "Some strange vengeful spirits have settled in a vacant house");
				strcpy(quest_text[line++], "next to you. Since dealing with spirits is your job, you");
				strcpy(quest_text[line++], "decide to check it out.");
#endif
			}

		}
		else if(pc == CLASS_YUGI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "絡んできて目障りだったので全部叩き潰した。");
				strcpy(quest_text[line++], "近所の者から空き家の管理を頼み込まれた。");
				strcpy(quest_text[line++], "物置代わりに使ってやろう。");
#else
                strcpy(quest_text[line++], "It was annoying, but you ended up crushing all of them.");
				strcpy(quest_text[line++], "You were requested to manage that vacant house.");
				strcpy(quest_text[line++], "You're going to use it as a storeroom.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "地霊殿の隣の空き家に奇妙な怨霊が屯しているらしい。");
				strcpy(quest_text[line++], "折角だから見物に行ってみよう。");
#else
                strcpy(quest_text[line++], "Some strange vengeful spirits have settled in a vacant house");
				strcpy(quest_text[line++], "next to the Palace of Earth Spirits.");
                strcpy(quest_text[line++], "It's about time you've checked it out.");
#endif
			}

		}
		break;

	case QUEST_KAPPA:	//謎の瘴気　妖夢・幽々子
		///mod151001 クエスト内容変更
		if(pc == CLASS_YOUMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "ここは河童たちの巣窟になっていたらしい。とりあえず全て倒した。");
				strcpy(quest_text[line++], "一息ついたとき、白玉楼を出る際に主が妙な道具を預けてくれたことを思い出した。");//一行40字まで
				strcpy(quest_text[line++], "どうやらこれを使えば周囲を全て知覚できるらしい。");//一行40字まで
				strcpy(quest_text[line++], "主の心遣いを無駄にしてしまったようだ。せめて今後の探索に役立てよう。");//一行40字まで
#else
                strcpy(quest_text[line++], "Looks like it was a kappa hideout. In any case, you defeated them all.");
				strcpy(quest_text[line++], "As you took a break, you remembered that your mistress gave you an");//一行40字まで
				strcpy(quest_text[line++], "odd device as you left Hakugyokurou. Looks like you can detect");//一行40字まで
				strcpy(quest_text[line++], "everything around you if you use it. You're not letting your");//一行40字まで
				strcpy(quest_text[line++], "mistress's thought go to waste; you'll use it for your exploration");//一行40字まで
				strcpy(quest_text[line++], "from now on.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "廃屋に棲んでいた者達は、あなたの手には負えなかった。");//一行40字まで
				strcpy(quest_text[line++], "主から預かった魔道具のことを思い出したが、");//一行40字まで
				strcpy(quest_text[line++], "どうやらどさくさに無くしてしまったらしい・・。");//一行40字まで
#else
                strcpy(quest_text[line++], "You were no match for those who live in that building.");//一行40字まで
				strcpy(quest_text[line++], "You remember your mistress giving you a magic device,");//一行40字まで
				strcpy(quest_text[line++], "but looks like you've lost it at some point...");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "霧雨魔法店に幽霊があふれていると魔理沙から苦情があった。");
				strcpy(quest_text[line++], "幽霊たちに話を聞いてみると、近所の廃屋「弟切荘」に住んでいたらしい。");
				strcpy(quest_text[line++], "何者かから攻撃を受けて逃げ出してきたそうだ。");
				strcpy(quest_text[line++], "様子を見に行ってみよう。");
#else
                strcpy(quest_text[line++], "Marisa has been complaining about a large amount of ghosts");
				strcpy(quest_text[line++], "around her magic shop. You've talked to them; looks like they're living");
				strcpy(quest_text[line++], "in a nearby abandoned building, but someone attacked and drove them out.");
				strcpy(quest_text[line++], "You've decided to take a look.");
#endif
			}


		}
		else if(pc == CLASS_YUYUKO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「見事なお手並みでした。というか私は何のために・・");
				strcpy(quest_text[line++], "あ、これお返ししますね。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Youmu - 'That was splendid. Just what am I supposed to be doing, though...");
				strcpy(quest_text[line++], "Ah, I'm returning you this.'");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「ええ、暇だったのですね。");//一行40字まで
				strcpy(quest_text[line++], "それでは私が行ってきます。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Youmu - 'Oh, so you had some free time.");
				strcpy(quest_text[line++], "Very well, I'm going in now.'");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「あ、幽々子様。ええ、仰る通りでした。");
				strcpy(quest_text[line++], "あの廃屋に何か妙な物が出現して幽霊たちを追い出してしまったそうです。");
				strcpy(quest_text[line++], "え？幽々子様も見に行ってみるのですか？」");
#else
                strcpy(quest_text[line++], "Youmu - 'Ah, Lady Yuyuko. It's exactly as you said.");
				strcpy(quest_text[line++], "Something weird has appeared in that abandoned building, and");
				strcpy(quest_text[line++], "it chased the ghosts away. What? You're going to check it");
				strcpy(quest_text[line++], "out as well?'");
#endif
			}


		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「何と、ここは河童たちの倉庫だったのですか。");
				strcpy(quest_text[line++], "こんな時の為に心眼を鍛えていたのに、まだまだ修行が足りません…");
				strcpy(quest_text[line++], "あ、そういえば幽々子様がこれを持たせてくれていたんです。");
				strcpy(quest_text[line++], "どういう物なのかよく分からなかったのですが、お礼に閻魔様に差し上げますね。」");
#else
                strcpy(quest_text[line++], "Youmu - 'Oh, so that was a kappa warehouse?");
				strcpy(quest_text[line++], "I've been training my mind's eye, but looks like I still have ways to go...");
				strcpy(quest_text[line++], "By the way, Lady Yuyuko gave me this. I don't understand what it is,");
				strcpy(quest_text[line++], "but I'm giving it as a reward to you, Miss Yama.'");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「この近くの空き家に住んでいる幽霊たちが、");
				strcpy(quest_text[line++], "妙なものに脅かされているらしいのです。");
				strcpy(quest_text[line++], "私もさっき行ってみたんですが別に何もいないみたいで");
				strcpy(quest_text[line++], "でも確かに妙な気配はするんです。閻魔様は何かわかりますか？」");
#else
                strcpy(quest_text[line++], "Youmu - 'The ghosts living in a nearby vacant house");
				strcpy(quest_text[line++], "are being attacked by something weird.");
				strcpy(quest_text[line++], "I've checked it out, but I didn't notice anything. But there certainly");
				strcpy(quest_text[line++], "is some odd presence there. Miss Yama, maybe you can tell what that is?'");
#endif
			}


		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「そこの廃屋に近寄っては駄目ですよ。妙なものが潜んでいるようなのです。」");
#else
                strcpy(quest_text[line++], "Youmu - 'Don't get too close to that abandoned house. There's something");
                strcpy(quest_text[line++], "weird living inside.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖夢「え、河童が屯していたの？というかあなたが全部倒しちゃったの？");
				strcpy(quest_text[line++], "困ったな。これはこれで幽々子様に何と報告していいものやら・・");
				strcpy(quest_text[line++], "とりあえずこれをあげますね。」");
#else
                strcpy(quest_text[line++], "Youmu - 'Eh, that was kappas? And you defeated them all?");
				strcpy(quest_text[line++], "I'm troubled. How can I even tell about this to Lady Yuyuko?");
				strcpy(quest_text[line++], "In any case, I'm giving you this.'");
#endif
			}
		}
		break;
		//ミミッククエ
	case QUEST_MIMIC:

		break;

		//デスソード
	case QUEST_DEATHSWORD:
		break;

		//下水道
	case QUEST_SEWER:
		if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「最近下水道の様子がおかしいの。");
				strcpy(quest_text[line++], "ひょっとすると妖怪か何かが棲みついているのかもしれないわ。");
				strcpy(quest_text[line++], "前みたいに無茶をしたら駄目よ。本当にどうなっても知らないからね。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'There's something weird about the sewer lately.");
				strcpy(quest_text[line++], "There might be some youkai or something else living inside.");
				strcpy(quest_text[line++], "Please don't charge in like you did before. I honestly don't");
				strcpy(quest_text[line++], "know what could happen.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「だから言ったでしょう。");
				strcpy(quest_text[line++], "これに懲りたら少しはおとなしくしていることね。");
				strcpy(quest_text[line++], "大事に至らなくてよかったわ。");
#else
                strcpy(quest_text[line++], "Akyuu - 'Didn't I tell you?");
				strcpy(quest_text[line++], "I hope this'll teach you to stay quiet.");
				strcpy(quest_text[line++], "I'm glad you're alright.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「死者だの大ワニだのを倒してきた…ですって？");
				strcpy(quest_text[line++], "貴方、本当に人の道を踏み外しかけてるんじゃないの？」");
#else
                strcpy(quest_text[line++], "Akyuu - 'You... defeated some undead and a giant crocodile?");
				strcpy(quest_text[line++], "You're really not an ordinary human, are you?'");
#endif
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「そこの穴？あれは下水道に通じる人孔よ。");
				strcpy(quest_text[line++], "最近何かが棲み着いてるみたいで近々調査する予定よ。");
				strcpy(quest_text[line++], "危ないから入らないようにね。」");
#else
                strcpy(quest_text[line++], "Akyuu - 'That hole? That's a manhole leading to the sewers.");
				strcpy(quest_text[line++], "Looks like something started living there recently,");
				strcpy(quest_text[line++], "so I'm planning to investigate.");
				strcpy(quest_text[line++], "It's dangerous, so don't go in.'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「そう、そこまでひどいことになっていたのね。");
				strcpy(quest_text[line++], "最近本当に物騒なんだから気をつけなさい。」");
#else
				strcpy(quest_text[line++], "Akyuu - 'Oh, it has gotten that bad.");
				strcpy(quest_text[line++], "It's getting dangerous out here lately, so be careful.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「死者だの大ワニだのを倒してきた…ですって？");
				strcpy(quest_text[line++], "貴方、妖精から妖怪になりかけてない？」");
#else
				strcpy(quest_text[line++], "Akyuu - 'You... defeated the undead and a giant crocodile?");
				strcpy(quest_text[line++], "Are you about to turn into a youkai from a fairy?");
#endif
			}
		}
		break;

		//破滅1　チルノ
	case QUEST_DOOM1:
		if(pc == CLASS_CIRNO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "何だか思っていた奴と違ったが、ともかく全部倒した。");
				strcpy(quest_text[line++], "きれいな指輪を持っている奴が居たので戦利品として徴発してやった。");//一行40字まで
#else
                strcpy(quest_text[line++], "It wasn't who you thought it was, but you defeated them all.");
				strcpy(quest_text[line++], "One of them had a pretty ring, so you take it as a trophy.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "ちょっと溶けそうになったので一時撤退することにした。");//一行40字まで
				strcpy(quest_text[line++], "というかあれはどう見ても妖精じゃない。何が起こっているんだろう？");//一行40字まで
#else
                strcpy(quest_text[line++], "You were starting to melt, so you made a tactical retreat.");//一行40字まで
				strcpy(quest_text[line++], "No matter how you look at it, they weren't fairies. Just what");//一行40字まで
				strcpy(quest_text[line++], "has happened?");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "家の近くを飛んでいるといきなり火の玉をぶつけられた。");
				strcpy(quest_text[line++], "火の玉を撃った奴は近くの洞窟に逃げ込んだようだ。");
				strcpy(quest_text[line++], "またあの三人の仕業に違いない。");
				strcpy(quest_text[line++], "今度こそまとめて凍らせてやろう。");
#else
                strcpy(quest_text[line++], "As you were flying around your house, you got hit by a ball of fire.");
				strcpy(quest_text[line++], "Looks like your attacker ran away into a nearby cave.");
				strcpy(quest_text[line++], "It has to be the work of those three.");
				strcpy(quest_text[line++], "You're going to freeze them all this time.");
#endif
			}
		}
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「へへん、妖精を舐めるからこうなるんだ！");
				strcpy(quest_text[line++], "気分がいいからこれをお前たちにやるぞ！ちょっと前に拾ったお宝だ！」");
#else
                strcpy(quest_text[line++], "Cirno - 'Hehe, don't underestimate us fairies!");
				strcpy(quest_text[line++], "I'm in a good mood, so take this! It's a treasure I've picked up");
				strcpy(quest_text[line++], "a bit earlier!'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「うがー！この数でかかっても勝てないなんてー！");
				strcpy(quest_text[line++], "次こそはまとめて凍らせてやる！」");
#else
                strcpy(quest_text[line++], "Cirno - 'Ugh! We couldn't win even with our numbers!");
				strcpy(quest_text[line++], "Next time, I'm going to freeze them all!'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「そういえば変な奴に火の玉をぶつけられたことがあるんだ。");
				strcpy(quest_text[line++], "たしかこっちの方だ！みんなで報復しに行こう！」");
#else
                strcpy(quest_text[line++], "Cirno - 'By the way, some weirdo hit me with a fireball.");
				strcpy(quest_text[line++], "They ran away there! Let's beat them up!'");
#endif
			}

		}
		else if(pc == CLASS_SUNNY || pc == CLASS_LUNAR || pc == CLASS_STAR || pc == CLASS_3_FAIRIES)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「ふーん、変な奴らがいたもんね。");
				strcpy(quest_text[line++], "濡れ衣着せて悪かったわね！これをあげるわ！」");
#else
                strcpy(quest_text[line++], "Cirno - 'Hmm, so there were some weirdos there.");
				strcpy(quest_text[line++], "I'm sorry for blaming you! I'm giving you this!'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「うーん、何だったんだろ？");
				strcpy(quest_text[line++], "悪かったってば！そんなに睨まないでよ！」");
#else
                strcpy(quest_text[line++], "Cirno - 'Uhh, what was that?");
				strcpy(quest_text[line++], "I'm sorry! Don't glare at me like that!'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「さっきはよくもやってくれたわね！");
				strcpy(quest_text[line++], "ここで会ったが百年目！いざ尋常に死ねい！");
				strcpy(quest_text[line++], "…え？何のことだかわからないって？");
				strcpy(quest_text[line++], "あっちの洞窟の近くであたいに火の玉ぶつけたのはあんたたちじゃないの？」");
#else
                strcpy(quest_text[line++], "Cirno - 'Oh, so it was you who struck me down earlier!");
				strcpy(quest_text[line++], "This is it! Just go ahead and drop dead already!");
				strcpy(quest_text[line++], "...What? You don't understand what I'm talking about?");
				strcpy(quest_text[line++], "It wasn't you who shot me with a fireball from that cave");
				strcpy(quest_text[line++], "over there?'");
#endif
			}
		}

		break;

	case QUEST_TUPAI: //ツパイ　パチュリー、フラン
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "首尾良くペットを捕まえた。");
				strcpy(quest_text[line++], "レミィが礼状を送ってよこした。妙にかしこまった真似をする。");//一行40字まで
#else
                strcpy(quest_text[line++], "You've successfully captured the pet.");
				strcpy(quest_text[line++], "Remy has sent you a reward.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "不覚にも取り逃がしてしまった。");//一行40字まで
				strcpy(quest_text[line++], "ああいう仕事は苦手だ。");//一行40字まで
#else
                strcpy(quest_text[line++], "You were caught unaware, and the pet escaped.");//一行40字まで
				strcpy(quest_text[line++], "You're not good at jobs like this.");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "レミィのペットの妙な生き物が逃げ出したらしい。");
				strcpy(quest_text[line++], "折角なので探すのを手伝ってあげることにした。");
#else
                strcpy(quest_text[line++], "Looks like Remy's pet - some weird creature - has ran away.");
				strcpy(quest_text[line++], "You decide to go look for it.");
#endif
			}
		}
		if(pc == CLASS_MEIRIN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "咲夜「お疲れ様。これお嬢様からご褒美ですって。");
				strcpy(quest_text[line++], "何を始めたのか知らないけど、早く終わらせて帰ってきなさい。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Sakuya - 'Well done. Here's a reward from the mistress.");
				strcpy(quest_text[line++], "I don't know what you're doing, but I'd like you to finish it");//一行40字まで
				strcpy(quest_text[line++], "quickly and come back to us.'");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "咲夜「そこの路地で取り逃がしたですって？」");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "(咲夜の姿が消えて数秒後再び出現した)");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "咲夜「大体の場所が分かったので今捕まえてお嬢様にお届けしてきたわ。");
				strcpy(quest_text[line++], "貴女もさっさと持ち場に戻ったら？」");
#else
                strcpy(quest_text[line++], "Sakuya - 'Did you lose in that area?'");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "(Sakuya vanishes and then reappears seconds later)");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "Sakuya - 'Now that I knew where it is, I've caught it and");
				strcpy(quest_text[line++], "returned to Remilia. You also should return to your post.'");
#endif

			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "咲夜「あら美鈴、こんな所で何をしているの？");
				strcpy(quest_text[line++], "まあいいわ。");
				strcpy(quest_text[line++], "ツパイが逃げ出してしまったの。貴女も探すのを手伝いなさい。」");
#else
                strcpy(quest_text[line++], "Sakuya - 'Oh, Meiling? What are you doing here?");
				strcpy(quest_text[line++], "Ah, very well.");
				strcpy(quest_text[line++], "Our tupai has ran away, so I'd like you to help us search for it.'");
#endif
			}
		}
		if(pc == CLASS_SAKUYA)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "無事ペットが戻りお嬢様は満足されたようだ。");
				strcpy(quest_text[line++], "褒美として最近気に入られているリボンを頂いた。");//一行40字まで
#else
                strcpy(quest_text[line++], "You managed to safely bring the pet back; your mistress is happy.");
				strcpy(quest_text[line++], "As a reward, she gave you a ribbon you've taken a liking to.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "一度は見つけたが不覚にも取り逃がしてしまった。");
				strcpy(quest_text[line++], "あるまじき失態だ。");
#else
                strcpy(quest_text[line++], "You've found it, but you were caught unaware, and it escaped.");
				strcpy(quest_text[line++], "That's a serious failure.");
#endif

			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "留守にしているうちにお嬢様のペットが逃げ出してしまった。");
				strcpy(quest_text[line++], "すぐに捕まえないとお嬢様が悲しんでしまう。");
#else
                strcpy(quest_text[line++], "While you were away, your mistress's pet has escaped.");
				strcpy(quest_text[line++], "Your mistress is going to be sad if you won't capture it.");
#endif
			}
		}


		if(pc == CLASS_FLAN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "どうにかペットを壊さず捕まえた。");
				strcpy(quest_text[line++], "お姉様が礼状を送ってよこした。妙にかしこまった真似をする。");//一行40字まで
#else
                strcpy(quest_text[line++], "You've managed to capture the pet without breaking it.");
				strcpy(quest_text[line++], "Your sister has sent you a reward.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "不覚にも取り逃がしてしまった。");//一行40字まで
				strcpy(quest_text[line++], "ああいう仕事は苦手だ。");//一行40字まで
#else
				strcpy(quest_text[line++], "You were caught unaware, and the pet escaped.");//一行40字まで
				strcpy(quest_text[line++], "You're not good at jobs like this.");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お姉様のペットの妙な生き物が逃げ出したらしい。");
				strcpy(quest_text[line++], "折角なので探すのを手伝ってあげることにした。");
#else
                strcpy(quest_text[line++], "Looks like your sisters's pet (some weird creature)");
				strcpy(quest_text[line++], "has ran away. You decide to go look for it.");
#endif
			}


		}
		break;

		//クローン地獄
	case QUEST_CLONE:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "霖之助「少々まずいことが起こりました。");
				strcpy(quest_text[line++], "例のクローン実験場でクローン達が暴走してしまいましてね。」");
				strcpy(quest_text[line++], "恐れ入りますが何とか始末をお願いしたい。」");
#else
                strcpy(quest_text[line++], "Rinnosuke - 'Something bad has happened.");
				strcpy(quest_text[line++], "The clones at that experimental cloning site have gone berserk.");
				strcpy(quest_text[line++], "I'm sorry, but couldn't you clean it up?'");
#endif
			}
		}
		break;
		//ドラゴン退治　妖精、三月精、アリス
	case QUEST_CHAOS_WYRM:
		if(pc == CLASS_SUNNY || pc == CLASS_LUNAR || pc == CLASS_STAR || pc == CLASS_3_FAIRIES )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "本当にドラゴンを倒せてしまった。");
				strcpy(quest_text[line++], "家の近くを調べていると綺麗な指輪を見つけた。私たちの宝物にしよう。");//一行40字まで
#else
                strcpy(quest_text[line++], "You really managed to defeat the dragon!");
				strcpy(quest_text[line++], "You also found a pretty ring near your house. It's going to be");//一行40字まで
				strcpy(quest_text[line++], "your treasure.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "危うく一回休みになるところだった。");//一行40字まで
				strcpy(quest_text[line++], "素直に霊夢さんに頼みに行こう・・");//一行40字まで
#else
                strcpy(quest_text[line++], "Whew, you almost were done for.");//一行40字まで
				strcpy(quest_text[line++], "Looks like you'll have to visit Reimu...");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "しばらく留守にしていると家の近くに見たこともないドラゴンがうろついていた。");
#else
                strcpy(quest_text[line++], "While you were away, some dragon you've never seen before has appeared");
                strcpy(quest_text[line++], "close to your house.");
#endif
				if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
#ifdef JP
					strcpy(quest_text[line++], "でも今ならこんなに仲間がいる！ドラゴンにでも勝てそうな気がする！");
#else
                    strcpy(quest_text[line++], "But with all the friends you have right now, even a dragon is no match for you!");
#endif
				else
#ifdef JP
					strcpy(quest_text[line++], "でも何だか今は元気一杯だ！ドラゴンにでも勝てそうな気がする！");
#else
                    strcpy(quest_text[line++], "But you feel yourself full of energy today! Even a dragon is no match for you!");
#endif
			}
		}
		else if(pr == RACE_FAIRY )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "サニーミルク「すごーい！妖精たちの夜明けも近いわ！」");
				strcpy(quest_text[line++], "ルナチャイルド「というか貴方本当に妖精なの？」");
				strcpy(quest_text[line++], "スターサファイア「お礼にこれを差し上げますわ。私たちの宝物よ。」");
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Awesome! The dawn of us fairies is at hand!'");
				strcpy(quest_text[line++], "Luna Child - 'Are you really a fairy?'");
				strcpy(quest_text[line++], "Star Sapphire - 'Take this as your reward. It's our treasure.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "サニーミルク「見た？凄かったでしょ。」");
				strcpy(quest_text[line++], "ルナチャイルド「あーあ、また引っ越しかぁ。」");//一行40字まで
				strcpy(quest_text[line++], "スターサファイア「今度の盆栽は出来が良かったのに残念ねえ。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Did you see it? Was it cool?'");
				strcpy(quest_text[line++], "Luna Child - 'We'll have to move again.'");//一行40字まで
				strcpy(quest_text[line++], "Star Sapphire - 'It's pity I'm leaving my bonsai behind.'");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "サニーミルク「待ちなさい、森の方に行くと危ないわ！」");
				strcpy(quest_text[line++], "ルナチャイルド「強そうなドラゴンに一回休みにされちゃうよ。」");
				strcpy(quest_text[line++], "スターサファイア「頼りの巫女も昼寝ばっかり。困ったものねぇ。」");
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Wait, it's dangerous to go into the forest!'");
				strcpy(quest_text[line++], "Luna Child - 'There's a strong dragon who'll take you down.'");
				strcpy(quest_text[line++], "Star Sapphire - 'And that shrine maiden is still sleeping...'");
#endif
			}
		}

		break;

		//柳じじい
	case QUEST_OLD_WILLOW:
		break;

		//火竜討伐　こいし,お燐、さとり
	case QUEST_SHOOTINGSTAR:
		if(pc == CLASS_YUGI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「貴方ですか・・丁度いいところに来てくれました。");
				strcpy(quest_text[line++], "実は旧灼熱地獄の最深部に凶暴な火竜が棲み着いて、");
				strcpy(quest_text[line++], "かなり手を焼いているのです。");
				strcpy(quest_text[line++], "済みませんが、何とかあれを倒して頂けませんか？」");
#else
                strcpy(quest_text[line++], "Satori - 'It's you... You came at a good time.");
				strcpy(quest_text[line++], "There's a violent fire dragon living in the depths of");
				strcpy(quest_text[line++], "Former Hell. It's very powerful.");
				strcpy(quest_text[line++], "I'm sorry, but couldn't you take it out?'");
#endif
			}
		}
		else if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妙に服があちこち焦げてるがなぜだったか思い出せない。");
				strcpy(quest_text[line++], "いつの間にか奇妙な杖を手に持っていた。");//一行40字まで
#else
                strcpy(quest_text[line++], "That's odd. Your clothes are scorched in places, but you");
                strcpy(quest_text[line++], "can't remember why. Also, you're holding some weird staff");
				strcpy(quest_text[line++], "you don't remember picking up.");//一行40字まで
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お燐「旧灼熱地獄に強い竜が棲み着いたんだってさ。");
				strcpy(quest_text[line++], "　さとり様が困ってるからあたい達で何とかしないと。」");
				strcpy(quest_text[line++], "お空「火を吐くドラゴンかあ。久しぶりに暴れ甲斐がありそうね。」");
				strcpy(quest_text[line++], "お燐「あれ？今誰かそこに居なかったかい？」");
#else
                strcpy(quest_text[line++], "Orin - 'There's a strong dragon living in the Former Hell.");
				strcpy(quest_text[line++], " It's troubling Satori, so we have to do something about it.");
				strcpy(quest_text[line++], "Okuu - 'A fire-breathing dragon? It has been a long time");
				strcpy(quest_text[line++], " since I went on a rampage.'");
				strcpy(quest_text[line++], "Orin - 'Oh? Was there anyone else here?'");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「お手を煩わせて申し訳ありません。");
				strcpy(quest_text[line++], "重ねて恐縮なのですが、これをそちらで管理して頂けませんか？");//一行40字まで
				strcpy(quest_text[line++], "危険な魔道具なので、あまり地底においておきたくはないのです。」");
#else
                strcpy(quest_text[line++], "Satori - 'I'm terribly sorry for troubling you.");
				strcpy(quest_text[line++], "Again, I'm sorry, but couldn't you take care of this as well?");//一行40字まで
				strcpy(quest_text[line++], "It's a dangerous magic device I don't like leaving here underground.");
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「貴女は・・ああ何てこと。");
				strcpy(quest_text[line++], "この期に及んで隠しても仕方ありませんね。");
				strcpy(quest_text[line++], "旧灼熱地獄の最深部に異界の凶暴な火竜が棲み付きまして、");
				strcpy(quest_text[line++], "私のペット達が手を焼いているのです。");
				strcpy(quest_text[line++], "管理責任については重々承知しておりますが、");
				strcpy(quest_text[line++], "少々手伝って頂くわけには参りませんか？」");
#else
                strcpy(quest_text[line++], "Satori - 'You are... oh, what.");
				strcpy(quest_text[line++], "I guess I'm not hiding anything from you.");
				strcpy(quest_text[line++], "There's a violent dragon from other world living in the depths");
				strcpy(quest_text[line++], "of Former Hell; my pets were no match for it.");
				strcpy(quest_text[line++], "I am very well aware of my responsibilities,");
				strcpy(quest_text[line++], "but couldn't you lend me a hand?'");
#endif
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "何とか強力な竜を倒すことに成功した。");
				strcpy(quest_text[line++], "竜はなかなか面白そうな杖を持っていた。");//一行40字まで
#else
                strcpy(quest_text[line++], "You managed to defeat the powerful dragon.");
				strcpy(quest_text[line++], "It was holding a fairly interesting staff.");//一行40字まで
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "旧灼熱地獄に異界の火竜が棲み着いたとペットから報告があった。");
				strcpy(quest_text[line++], "放っておくのも危険なので討伐に向かわないといけない。");
#else
                strcpy(quest_text[line++], "Your pets have warned you of a fire dragon from other world");
				strcpy(quest_text[line++], "living in Former Hell. It's dangerous to leave it be, so you");
				strcpy(quest_text[line++], "have to go and deal with it.");
#endif
			}

		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "どうにか強力な竜を倒した。");
				strcpy(quest_text[line++], "溶岩の中に杖が浮いている。珍しいので拾ってきた。");//一行40字まで
#else
                strcpy(quest_text[line++], "You somehow managed to defeat the powerful dragon.");
				strcpy(quest_text[line++], "You found a staff floating in lava. It looked pretty rare,");//一行40字まで
				strcpy(quest_text[line++], "so you picked it up.");//一行40字まで
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お燐「旧灼熱地獄に強い竜が棲み着いたんだってさ。");
				strcpy(quest_text[line++], "　さとり様が困ってるからあたい達で何とかしないと。」");
				strcpy(quest_text[line++], "お空「火を吐くドラゴンかあ。久しぶりに暴れ甲斐がありそうね。」");
#else
                strcpy(quest_text[line++], "Orin - 'There's a strong dragon living in the Former Hell.");
				strcpy(quest_text[line++], " It's troubling Satori, so we have to do something about it.");
				strcpy(quest_text[line++], "Okuu - 'A fire-breathing dragon? It has been a long time");
				strcpy(quest_text[line++], " since I went on a rampage.'");
#endif
			}
		}
		else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「あなたたちがあの火竜を倒したですって？");
				strcpy(quest_text[line++], "　妖精も群れると馬鹿にならないわね。");
				strcpy(quest_text[line++], "ともあれ困っていたのよ。礼を言うわ。");
				strcpy(quest_text[line++], "報酬にこれをあげましょう。でも悪用すると身を滅ぼすわよ？」");
#else
                strcpy(quest_text[line++], "Satori - 'You defeated that fire dragon?");
				strcpy(quest_text[line++], " I guess fairies aren't to be trifled with if they act together.");
				strcpy(quest_text[line++], "You were a big help. I'm grateful.");
				strcpy(quest_text[line++], "Take this as your reward. But remember, if you abuse it, there's");
				strcpy(quest_text[line++], "going to be big trouble!'");
#endif

			}
			else if (fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お燐「旧灼熱地獄に強い竜が棲み着いたんだってさ。");
				strcpy(quest_text[line++], "　さとり様が困ってるからあたい達で何とかしないと。」");
				strcpy(quest_text[line++], "お空「火を吐くドラゴンかあ。久しぶりに暴れ甲斐がありそうね。」");
				strcpy(quest_text[line++], "お燐「...って何だいお前さんたちはゾロゾロと。");
				strcpy(quest_text[line++], "　聞いてただろ？中庭の入り口から中に入るんじゃないよ？」");
#else
                strcpy(quest_text[line++], "Orin - 'There's a strong dragon living in the Former Hell.");
				strcpy(quest_text[line++], " It's troubling Satori, so we have to do something about it.");
				strcpy(quest_text[line++], "Okuu - 'A fire-breathing dragon? It has been a long time");
				strcpy(quest_text[line++], " since I went on a rampage.'");
				strcpy(quest_text[line++], "Orin - '...Hey, what are you doing here?");
				strcpy(quest_text[line++], " Were you listening? Did you enter through the entrance in");
				strcpy(quest_text[line++], " the courtyard?'");
#endif
			}

		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「あなたがあの火竜を倒したですって？");
				strcpy(quest_text[line++], "・・嘘でも妄想でもないようね。");
				strcpy(quest_text[line++], "俄かには信じ難いけれど、礼を言います。");
				strcpy(quest_text[line++], "報酬にこれをあげましょう。でも悪用すると身を滅ぼすわよ？」");
#else
                strcpy(quest_text[line++], "Satori - 'You defeated that fire dragon?");
				strcpy(quest_text[line++], "...Doesn't look like that's a lie or some delusion.");
				strcpy(quest_text[line++], "I have hard time believing this, but I'm grateful.");
				strcpy(quest_text[line++], "Take this as your reward. But remember, if you abuse it, there's");
				strcpy(quest_text[line++], "going to be big trouble!'");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:ここには来ないはず", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お燐「旧灼熱地獄に強い竜が棲み着いたんだってさ。");
				strcpy(quest_text[line++], "　さとり様が困ってるからあたい達で何とかしないと。」");
				strcpy(quest_text[line++], "お空「火を吐くドラゴンかあ。久しぶりに暴れ甲斐がありそうね。」");
				strcpy(quest_text[line++], "お燐「あれ？今誰かそこに居なかったかい？」");
#else
                strcpy(quest_text[line++], "Orin - 'There's a strong dragon living in the Former Hell.");
				strcpy(quest_text[line++], " It's troubling Satori, so we have to do something about it.");
				strcpy(quest_text[line++], "Okuu - 'A fire-breathing dragon? It has been a long time");
				strcpy(quest_text[line++], " since I went on a rampage.'");
				strcpy(quest_text[line++], "Orin - 'Oh? Was there anyone else here?'");
#endif
			}
		}

		break;

		//京丸牡丹　レミリア
	case QUEST_KYOMARU:
		if(pc == CLASS_REMY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "パチュリー「それは・・京丸牡丹？");
				strcpy(quest_text[line++], "私のためにあんな水だらけのところに取りに行ってくれたの？");
				strcpy(quest_text[line++], "ありがとう。お礼にこれをあげるわ。」");
#else
                strcpy(quest_text[line++], "Patchouli - 'That's... the kyoumarubotan?");
				strcpy(quest_text[line++], "You went wading through water for my sake?");
				strcpy(quest_text[line++], "Thank you very much. Here's a reward.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "パチュリー「そのうち咲夜に頼んで持ってきてもらおうかしら。」");
#else
                strcpy(quest_text[line++], "Patchouli - 'I wonder if Sakuya could bring it for me.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "パチュリー「その本に載っている花が気になるの？");
				strcpy(quest_text[line++], "それは京丸牡丹。玄武の沢に咲く珍しい花だそうよ。」");
#else
                strcpy(quest_text[line++], "Patchouli - 'Are you curious about the flowers in this book?");
				strcpy(quest_text[line++], "That's a kyoumarubotan. Looks like it's rare flower growing at");
				strcpy(quest_text[line++], "Genbu Lake.'");
#endif
			}
		}
		else if(pc == CLASS_MARISA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "パチュリー「前話していた京丸牡丹、私にも一つ分けてくれない？");
				strcpy(quest_text[line++], "いつも勝手に本を持って行ってるんだから、たまには何か役に立ちなさい。」");
#else
                strcpy(quest_text[line++], "Patchouli - 'Couldn't you bring me a kyoumarubotan we were talking");
                strcpy(quest_text[line++], "about earlier? You're always taking away my books, so at least try to");
                strcpy(quest_text[line++], "be of use once in a while.'");
#endif
			}

		}
		else if(pc == CLASS_SAKUYA)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "パチュリー「あなたの猫度は6点。もうすぐ素数になってしまうわ。」");
#else
                strcpy(quest_text[line++], "Patchouli - 'Your cat-likeness is at 6 points. It's going to be a");
                strcpy(quest_text[line++], "prime number soon.'");
#endif
			}

		}

		break;



		//仙人なんてこわくない　影狼
	case QUEST_SENNIN:
		if(pc == CLASS_KAGEROU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "聖人「豊聡耳　神子」の復活以降、仙人として修業を始める者が増えている。");
				strcpy(quest_text[line++], "仙人は妖怪の大敵だ。特に自分たちのような力の弱い妖怪たちにとっては。");//一行40字まで
				strcpy(quest_text[line++], "これ以上縄張りを脅かされる前に叩いておくべきかもしれない。");
				strcpy(quest_text[line++], "あなたはそう考え、神社の参道から仙人の住居へ単身潜入することにした。");
#else
                strcpy(quest_text[line++], "With the resurrection of the saint Toyosatomimi no Miko, hermits are");
				strcpy(quest_text[line++], "growing in number. They're the enemies of you youkai - especially of");//一行40字まで
				strcpy(quest_text[line++], "weak youkai like you. You probably have to beat them up before they start");
				strcpy(quest_text[line++], "threatening your territory. You decide to go alone and infiltrate the");
				strcpy(quest_text[line++], "hermit residence.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは高名な聖人を打ち倒し、仙人集団に痛撃を与えることに成功した。");
				strcpy(quest_text[line++], "仲間たちがあなたに感謝の贈り物を用意してくれた。");
				strcpy(quest_text[line++], "歓喜の声と共にありがたく受け取った。");
#else
                strcpy(quest_text[line++], "You've defeated their saint, dealing a painful blow to the hermits.");
				strcpy(quest_text[line++], "Your allies have presented you with a reward in gratitude.");
				strcpy(quest_text[line++], "You accepted it with a joyous howl.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "かの聖人の力は予想を遥かに超えていた。あなたは命からがら逃げ出した。");
				strcpy(quest_text[line++], "暗い塒で自分と仲間達の前途を案じつつ月に向けて一声鳴くのであった。");
#else
                strcpy(quest_text[line++], "That saint's power was beyond your expectations, and you ran away for your life.");
				strcpy(quest_text[line++], "You then were howling at the moon, thinking of the future awaiting you and");
				strcpy(quest_text[line++], "your friends.");
#endif
			}
		}
		break;

		//墓地警備員　命蓮寺メンバー
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_KYOUKO ||pc == CLASS_NAZRIN || pc == CLASS_ICHIRIN ||pc == CLASS_MURASA || pc == CLASS_SHOU )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "白蓮「おかげで無事に葬儀を終えることができました。本当にありがとう。」");
				strcpy(quest_text[line++], "ところで、しばらく前から作っていたものが完成したの。");
				strcpy(quest_text[line++], "これからの貴方の助けになると思うわ。よければ使ってみてね。」");
#else
                strcpy(quest_text[line++], "Byakuren - 'Thanks to you, we're able to properly conduct funeral rites.");
				strcpy(quest_text[line++], "I'm really grateful. By the way, I've finished making that thing I've");
				strcpy(quest_text[line++], "been working. I think it'll come in use. Please take it.'");
#endif
			}
		}
		break;

	//大亀退治　わかさぎ姫
	case QUEST_OOGAME:
		if(pc == CLASS_WAKASAGI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは見事二匹の大亀を懲らしめた。");
				strcpy(quest_text[line++], "大亀は綺麗な指輪をあなたにくれた。あなたのことを湖の主だと思っているらしい。");//一行40字まで
#else
                strcpy(quest_text[line++], "You have disciplined the two giant turtles.");
				strcpy(quest_text[line++], "They gave a pretty ring. Looks like they now see you as the ruler of the lake.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは命からがら湖から逃げだした。");
				strcpy(quest_text[line++], "当分は神社の裏の池にでも厄介になるしかないだろう・・");
#else
                strcpy(quest_text[line++], "You ran away for your life, fleeing the lake.");
				strcpy(quest_text[line++], "For the time being, looks like you'll have to live in the pond at the back");
				strcpy(quest_text[line++], "of the shrine...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "湖を泳いでいると凶悪な大亀のモンスターに遭遇した。");
				strcpy(quest_text[line++], "このままでは夜もおちおち眠れない。何とかしなくては。");
#else
                strcpy(quest_text[line++], "As you were swimming in the lake, you ran into a violent giant");
				strcpy(quest_text[line++], "turtle monster. Now you can't peacefully sleep at night. You have to do");
				strcpy(quest_text[line++], "something about it.");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "わかさぎ姫「今湖で遊んじゃダメよ！");
				strcpy(quest_text[line++], "火を吐く恐ろしい怪物が暴れているんだから！」");
#else
                strcpy(quest_text[line++], "Wakasagihime - 'Don't play at the lake right now!");
				strcpy(quest_text[line++], "There's a fire-breathing monster on a rampage!'");
#endif
			}
			else if(comp)
			{
				if (pc == CLASS_3_FAIRIES)
				{
#ifdef JP
					strcpy(quest_text[line++], "わかさぎ姫「あなたたちがあの怪物を倒したの？");
					strcpy(quest_text[line++], "妖怪の端くれとして立場がないわ・・");
					strcpy(quest_text[line++], "お礼にこれをあげるわね。」");
#else
                    strcpy(quest_text[line++], "Wakasagihime - 'You defeated the monsters?");
					strcpy(quest_text[line++], "You'd make a better youkai than me...");
					strcpy(quest_text[line++], "Take this as your reward.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "わかさぎ姫「嘘でしょう、あなたがあの怪物を倒したの？");
					strcpy(quest_text[line++], "妖怪の端くれとして立場がないわ・・");
					strcpy(quest_text[line++], "お礼にこれをあげるわね。」");
#else
                    strcpy(quest_text[line++], "Wakasagihime - 'It can't be, you defeated those monsters?");
					strcpy(quest_text[line++], "You'd make a better youkai than me...");
					strcpy(quest_text[line++], "Take this as your reward.'");
#endif

				}
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "わかさぎ姫「しばらく神社の裏の池にでも引っ越そうかしら・・」");
#else
                strcpy(quest_text[line++], "Wakasagihime - 'I guess I'll have to move to the pond at the back");
                strcpy(quest_text[line++], "of the shrine for a while...'");
#endif
			}
		}

		break;

	//古い城　椛、はたて
	case QUEST_OLDCASTLE:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「やるねー。流石は博麗の巫女ってとこ？");
				strcpy(quest_text[line++], "巫女に借りを作りっぱなしってのも何だかだし、");
				strcpy(quest_text[line++], "報酬にこれ持って行ってよ。」");
#else
                strcpy(quest_text[line++], "Hatate - 'Not bad! Just what you'd expect from the Hakurei shrine");
				strcpy(quest_text[line++], "maiden, right? I have no intention of staying in your debt, so");
				strcpy(quest_text[line++], "here's your reward.'");
#endif
			}
		}
		else if(pc == CLASS_MOMIZI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「あ、椛。例の城の掃討作戦の尖兵を申し付けられたんだって？");
				strcpy(quest_text[line++], "巨人とか竜とか厄介そうな連中ばかりいるのに大天狗様も酷ねえ。");
				strcpy(quest_text[line++], "ま、骨は拾ってあげるから頑張ってね。」");
#else
                strcpy(quest_text[line++], "Hatate - 'Oh, Momiji. Are you prepared to clean up that old castle?");
				strcpy(quest_text[line++], "There's plenty of nasty enemies like giants and dragons, that's");
				strcpy(quest_text[line++], "enough to give even the Great Tengu paause. Don't worry, I'll");
				strcpy(quest_text[line++], "pick you up if you drop dead.'");
#endif
			}
		}
		else if(pr == RACE_KARASU_TENGU)//v2.0.3 鴉天狗全般同じメッセージにする
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "余所者たちを単騎殲滅し、大いに面目を施した。");
				strcpy(quest_text[line++], "さらに褒賞品として以前から目をつけていた宝物を譲り受けることに成功した。");
#else
                strcpy(quest_text[line++], "You've cleaned up those strangers by yourself, getting lots of attention.");
				strcpy(quest_text[line++], "You also managed to obtain a treasure you've set your eyes upon");
				strcpy(quest_text[line++], "as a reward.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "またも失態を演じてしまった。");
				strcpy(quest_text[line++], "当分の間新聞大会には顔を出せそうにない……");
#else
                strcpy(quest_text[line++], "You've splendidly failed again.");
				strcpy(quest_text[line++], "You'd rather avoid meeting other newspaper reporters for the time being...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "南東の古い城塞にモンスターたちが巣食っているという情報を得た。");
				strcpy(quest_text[line++], "以前の侵入事件と違い、今回の余所者はかなり凶悪らしい。");
				strcpy(quest_text[line++], "皆に先んじてこれを解決し、汚名返上とスクープ獲得の一挙両得を狙うのも悪くない。");
#else
                strcpy(quest_text[line++], "You've received news that the old stronghold in the southeast has been taken");
				strcpy(quest_text[line++], "over by monsters. Unlike those strangers settling in the village earlier,");
				strcpy(quest_text[line++], "this time the enemies are far nastier. It'd be a good idea to resolve");
				strcpy(quest_text[line++], "this issue, maintaining your reputation and getting newspaper material");
				strcpy(quest_text[line++], "at the same time.");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「流石は閻魔様ねー。");
				strcpy(quest_text[line++], "これ大天狗様から、今回の件の感謝の印にお納めくださいって。");
				strcpy(quest_text[line++], "それじゃ、なんか大天狗様に呼ばれてるから行かなきゃ。");
				strcpy(quest_text[line++], "助かったわありがとー。」");
#else
                strcpy(quest_text[line++], "Hatate - 'Just what I expected from Miss Yama!");
				strcpy(quest_text[line++], "Please accept this gift from Great Tengu as a sign of gratitude.");
				strcpy(quest_text[line++], "Great Tengu's calling me, so I have to go now. Thanks for your help!'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「うーん、閻魔様って死人相手じゃないとそんな強くないのね。");
				strcpy(quest_text[line++], "まあ仕方ないか。山の問題は山で何とかしないとねー。」");
#else
                strcpy(quest_text[line++], "Hatate - 'Uhh, didn't think they'd be too strong for Miss Yama.");
				strcpy(quest_text[line++], "I guess we have no choice. It's up to us mountain residents");
				strcpy(quest_text[line++], "to resolve our own problems.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「あれ、なんで閻魔様がこんなとこに居るの？");
				strcpy(quest_text[line++], "ああそう休暇中なんだ。");
				strcpy(quest_text[line++], "それならちょっと頼みごとしていいかなー。");
				strcpy(quest_text[line++], "西の外れの古い城塞が厄介な怪物の巣になっちゃってるの。");
				strcpy(quest_text[line++], "まずは哨戒の天狗達が城攻めするんだけど、");
				strcpy(quest_text[line++], "まあどう見ても勝てそうにないし、できれば手伝ってあげて欲しいな。」");
#else
                strcpy(quest_text[line++], "Hatate 'Oh, what's Miss Yama doing at a place like this?");
				strcpy(quest_text[line++], "Ah, so you're on a vacation.");
				strcpy(quest_text[line++], "I wonder if I can give you a request...");
				strcpy(quest_text[line++], "An old stronghold has became infeseted by nasty monsters.");
				strcpy(quest_text[line++], "Our patrol tengu tried to clean it up, but it doesn't look");
				strcpy(quest_text[line++], "like they're having success. I'd like you to lend us");
				strcpy(quest_text[line++], "a hand if you can.'");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "はたて「聞いたよ。あんたが例の空き家の余所者をのしたんだって？");
				strcpy(quest_text[line++], "妖精のくせにやるのねー。");
				strcpy(quest_text[line++], "ついでにもう一仕事頼まれてみない？");
				strcpy(quest_text[line++], "今度は東の古い城塞よ。あの空き家とは比べ物にならない強敵がぎっしり。");
				strcpy(quest_text[line++], "まあ無理にとは言わないけど、腕に自信があるならやってみなよ。」");
#else
                strcpy(quest_text[line++], "Hatate - 'I've heard about up. You cleaned up that vacant house, right?");
				strcpy(quest_text[line++], "Not bad for a fairy!");
				strcpy(quest_text[line++], "Wouldn't you like to take on another job?");
				strcpy(quest_text[line++], "It's about an old castle to the west. The enemies are far stronger");
				strcpy(quest_text[line++], "this time, though. Don't go there if it seems too dangerous,");
				strcpy(quest_text[line++], "but you might want to test out your strength.'");
#endif
			}
		}
		break;

	case QUEST_LOOKING_GLASS:
		if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				if(pc == CLASS_SUNNY ||pc == CLASS_LUNAR ||pc == CLASS_STAR || pc == CLASS_3_FAIRIES)
				{
#ifdef JP
					strcpy(quest_text[line++], "アリス「あら、いつぞやの三人組じゃないの。");
					strcpy(quest_text[line++], "また紅茶を振る舞ってあげたいところだけど、今ちょっと都合が悪いの。");
					strcpy(quest_text[line++], "危ないから私の家に行っちゃ駄目よ。」");
#else
                    strcpy(quest_text[line++], "Alice - 'Oh, it's you three?");
					strcpy(quest_text[line++], "I'd like to make some red tea, but you came at a bad time.");
					strcpy(quest_text[line++], "Please stay away from my house, it's dangerous.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "アリス「妖精？・・の割に妙に強そうね。");
					strcpy(quest_text[line++], "ねえ貴方、腕に自信はあるかしら？");
					strcpy(quest_text[line++], "ここから奥に行ったところにある家の中に怪物が居座っているの。");
					strcpy(quest_text[line++], "そいつをどうにかできない？」");
#else
                    strcpy(quest_text[line++], "Alice - 'A fairy? ...You look fairly strong, though.");
					strcpy(quest_text[line++], "Hey, do you want to test your strength?");
					strcpy(quest_text[line++], "My house is located deeper inside, and there's a monster");
					strcpy(quest_text[line++], "dwelling in it. Could you try doing something about it?'");
#endif
				}
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "アリス「妖精があれを倒すなんて、あれが鏡から出てきた時より驚いたわ。");
				strcpy(quest_text[line++], "幻想郷に一体何が起こっているのかしら。");
				strcpy(quest_text[line++], "でもまずはお礼をしないとね。これをあげるわ。」");
#else
                strcpy(quest_text[line++], "Alice - 'So, a fairy defeated that monster? I'm even more surprised");
				strcpy(quest_text[line++], "than when it came out of the mirror in first place. Just what");
				strcpy(quest_text[line++], "is going on in Gensoukyou? Anyway, I have to thank you. Here's");
				strcpy(quest_text[line++], "your reward.'");
#endif
			}
		}
		else if(pc == CLASS_ALICE )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "奇妙な怪物を倒した。一体どこから入ってきたのだろう？");
				strcpy(quest_text[line++], "家を調べていると鏡台に見覚えのない装飾品が落ちていた。");
#else
                strcpy(quest_text[line++], "You defeated the weird monster. Just how did it get in here?");
				strcpy(quest_text[line++], "As you looked around your house, you noticed an accessory you");
				strcpy(quest_text[line++], "haven't seen before on your mirror stand.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "怪物は予想以上に強力で手に負えなかった。");
				strcpy(quest_text[line++], "魔理沙の援護を得て何とかなったが、");
				strcpy(quest_text[line++], "この借りはいずれ相当高くつくに違いない・・");
#else
                strcpy(quest_text[line++], "The monster was far stronger than you expected.");
				strcpy(quest_text[line++], "You managed to get Marisa to help you, but paying");
				strcpy(quest_text[line++], "her back is going to cost you quite a bit...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "しばらく留守にしているうちに家に奇妙な怪物が棲み付いていた。");
				strcpy(quest_text[line++], "かなりの強敵のようだが一刻も早く追い払わないといけない。");
#else
                strcpy(quest_text[line++], "While you were away, a weird monster has invaded your house.");
				strcpy(quest_text[line++], "It's very powerful, but you'll have to deal with it as soon");
				strcpy(quest_text[line++], "as possible.");
#endif
			}
		}
		else if(pc == CLASS_MARISA )
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "アリス「何よ口ほどにもないのね。");
				strcpy(quest_text[line++], "それにしても困ったわ。他に誰かこういう事を頼めそうな人なんて居たかしら・・」");
#else
                strcpy(quest_text[line++], "Alice - 'Sigh, you're just all talk.");
				strcpy(quest_text[line++], "This is getting troublesome. I wonder if there's anyone else");
				strcpy(quest_text[line++], "I can ask for help...'");
#endif
			}
		}

		break;

		//妹様　パチュリー
	case QUEST_FLAN:
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "何とか妹様をなだめることに成功した。");
				strcpy(quest_text[line++], "もう少しで十年前から精製中の喘息治療薬がお釈迦になるところだった。");
				strcpy(quest_text[line++], "まだ完全な効果はないが半分飲んでしまおう。リスクマネジメントは大事だ。");//一行40字まで
#else
                strcpy(quest_text[line++], "You somehow have managed to calm her down.");
				strcpy(quest_text[line++], "You almost had your asthma remedy you've been preparing for 10 years");
				strcpy(quest_text[line++], "destroyed. While it's still not perfect, you decide to take half of");//一行40字まで
				strcpy(quest_text[line++], "of it with you. Risk management is an important matter.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "図書館が半壊してしまった。様々な研究や実験も全てパアだ。");
				strcpy(quest_text[line++], "今日は生涯最大の厄日に違いない・・");
#else
                strcpy(quest_text[line++], "The library is half destroyed, and many of your studies and");
				strcpy(quest_text[line++], "experiments are gone. This is the worst day of your life...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "メイド妖精たちがパニックを起こしている。");
				strcpy(quest_text[line++], "話を聞いても要領を得ないが、妹様が癇癪を起こして館の中で暴れているらしい。");
				strcpy(quest_text[line++], "レミィと咲夜は神社の宴会で当分帰ってこない。");
				strcpy(quest_text[line++], "図書館に被害が出る前に何とか止めないといけない。");
#else
                strcpy(quest_text[line++], "The fairy maids are in panic.");
				strcpy(quest_text[line++], "Looks like the little sister is throwing a tantrum and rampging");
				strcpy(quest_text[line++], "through the mansion. Remy and Sakuya are at a banquest at the");
				strcpy(quest_text[line++], "shrine and haven't came back yet. You have to stop her before");
				strcpy(quest_text[line++], "something happens to the library.");
#endif
			}
		}
		else if(pc == CLASS_REMY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妹に一しきり説教して部屋へ帰した。");
				strcpy(quest_text[line++], "パチェがお礼にと何やら怪しい薬をくれた。");
				strcpy(quest_text[line++], "飲むと健康になるらしい。何の冗談だろう。");
#else
                strcpy(quest_text[line++], "After receiving a strict lecture, your sister returned");
				strcpy(quest_text[line++], "to her room. Patchy gave you some suspicious potions");
				strcpy(quest_text[line++], "as a reward. Apparently drinking it will make you stronger.");
				strcpy(quest_text[line++], "Is this some sort of a joke?");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "妹は一晩中暴れまわったあと部屋へ帰って行った。");
				strcpy(quest_text[line++], "それにしても派手にぶち壊してくれたものだ。");
				strcpy(quest_text[line++], "特にパチェには気の毒なことになった。");
#else
                strcpy(quest_text[line++], "After rampaging all night, your sister returned to her");
				strcpy(quest_text[line++], "room. However, she caused a lot of destruction. You");
				strcpy(quest_text[line++], "feel sorry for Patchy in particular.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "数日ぶりに館に帰ると、メイド妖精たちがパニックを起こしている。");
				strcpy(quest_text[line++], "話を聞いても要領を得ないが、妹が癇癪を起こして暴れているらしい。");
				strcpy(quest_text[line++], "姉として折檻をくれてやる必要があるようだ。");
#else
                strcpy(quest_text[line++], "You return to your mansion after being away for several days,");
				strcpy(quest_text[line++], "and your fairy maids are in panic. Looks like your little sister");
				strcpy(quest_text[line++], "is throwing a tantrum and rampaging through the mansion. Time");
				strcpy(quest_text[line++], "to discipline her a bit.");
#endif
			}
		}
		else if(pc == CLASS_SAKUYA)
		{
			 if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "もはや自分では手に負えそうにない。");
				strcpy(quest_text[line++], "お嬢様が折檻に向かったのでそのうち片は付くだろうが、");
				strcpy(quest_text[line++], "図書館を壊滅させられたパチュリー様には合わせる顔がない。");
#else
                strcpy(quest_text[line++], "You couldn't handle her at all.");
				strcpy(quest_text[line++], "Your mistress is going to come and deal with her, but with");
				strcpy(quest_text[line++], "the library destroyed, you can't bring yourself to face");
				strcpy(quest_text[line++], "Lady Patchouli anymore.");
#endif
			}
		}
		break;

		//妹紅への刺客　永夜抄で面識ある連中全て　受諾時のみ
		//永琳も
	case QUEST_KILL_MOKO:
		if(pc == CLASS_YOUMU || pc == CLASS_YUYUKO || pc == CLASS_REIMU || pc == CLASS_REMY || pc == CLASS_MARISA || pc == CLASS_ALICE || pc == CLASS_SAKUYA || pc == CLASS_YUKARI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "輝夜「ねえ、もう一度肝試しをやってみる気はない？");
				strcpy(quest_text[line++], "今度は一人で妹紅のところに行って適当にのして来なさい。");
				strcpy(quest_text[line++], "報酬ははずむわよ？」");
#else
                strcpy(quest_text[line++], "Kaguya - 'Hey, are you interested in taking the trail of guts");
				strcpy(quest_text[line++], "once more? This time, go alone to Mokou's place and beat her up.");
				strcpy(quest_text[line++], "Of course, there'll be a reward.'");
#endif
			}

		}
		else if(pc == CLASS_TEWI || pc == CLASS_UDONGE)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "輝夜「あらイナバ、丁度いいところに来たわね。");
				strcpy(quest_text[line++], "そろそろ妹紅にまた刺客でも送ろうと思うの。行ってらっしゃい。」");
#else
                strcpy(quest_text[line++], "Kaguya - 'Oh, Inaba, you came at a good time.");
				strcpy(quest_text[line++], "I was thinking of sending someone to assassinate Mokou.");
				strcpy(quest_text[line++], "Go and deal with her.'");
#endif
			}

		}
		else if(pc == CLASS_EIRIN)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "輝夜「ねえ永琳、ちょっと妹紅のところにも寄って行かない？」");
#else
                strcpy(quest_text[line++], "Kaguya - 'Hey, Eirin, would you like to visit Mokou?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "不死鳥の娘から珍しい素材を採取した。");
				strcpy(quest_text[line++], "大事に保管しておこう。");
#else
                strcpy(quest_text[line++], "You got a rare material from that phoenix girl.");
				strcpy(quest_text[line++], "You decide to keep it in a safe place.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "輝夜「もう、二人がかりで負けちゃったじゃないの！");
				strcpy(quest_text[line++], "何を遊んでいるのよ永琳！」");
#else
                strcpy(quest_text[line++], "Kaguya - 'Come on, there were two of us, and we still lost!");
				strcpy(quest_text[line++], "Stop playing around, Eirin!'");
#endif
			}

		}
		else if(pc == CLASS_KAGUYA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "そういえば最近妹紅とやり合っていない。");
				strcpy(quest_text[line++], "そろそろ直々に出向くのもいいだろう。");
#else
                strcpy(quest_text[line++], "To think of it, it has been a while since you played with");
				strcpy(quest_text[line++], "Mokou. You decide to go visit her in person.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "妹紅から珍しい素材を採取した。");
				strcpy(quest_text[line++], "新しい難題に使えないだろうか。");
#else
                strcpy(quest_text[line++], "You obtained a rare material from Mokou.");
				strcpy(quest_text[line++], "Could this be a new impossible request?");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "襲撃に失敗した。妹紅も色々揉まれて腕を上げているようだ。");
#else
                strcpy(quest_text[line++], "Your assault failed. Looks like Mokou hasn't been neglecting");
                strcpy(quest_text[line++], "her training either.");
#endif
			}

		}
		break;

	case QUEST_KILL_GUYA: //輝夜への刺客　妹紅
		if(pc == CLASS_MOKOU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "そういえば最近輝夜とやり合っていない。");
				strcpy(quest_text[line++], "たまにはこちらから出向くのもいいだろう。");
#else
                strcpy(quest_text[line++], "To think of it, it has been a while since you played with");
				strcpy(quest_text[line++], "Kaguya. You decide to go visit her in person.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "永遠亭を壊滅させてやった。");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "輝夜が最後に使おうとした宝物を勢いで奪ってきてしまった。");
				strcpy(quest_text[line++], "まあどうせそのうち奪い返しに来るだろう。");
#else
                strcpy(quest_text[line++], "You ended up destroying half of Eientei.");
				strcpy(quest_text[line++], " ");
				strcpy(quest_text[line++], "You've stolen a precious item Kaguya tried to use at the end.");
				strcpy(quest_text[line++], "Well, she'll probably come to try and get it back later.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "襲撃に失敗した。どうやって察知したのか敵の備えは万全だった。");
				strcpy(quest_text[line++], "どうも最近負けが込んでいる気がする。");
#else
                strcpy(quest_text[line++], "Your assault failed. The enemy was fully prepared.");
				strcpy(quest_text[line++], "You feel as if you've been on a losing streak lately.");
#endif
			}
		}
		else if (pc == CLASS_UDONGE) //v1.1.44 専用性格
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "ついにお師匠様を超える時が来た。");
				strcpy(quest_text[line++], "あなたは単身永遠亭へと向かった…");
#else
                strcpy(quest_text[line++], "Time has finally come for you to overcome your master.");
				strcpy(quest_text[line++], "You head out to Eientei on your own...");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは激戦の末にお師匠様と姫様を打倒した。");
				strcpy(quest_text[line++], "これで現の自分もストレスから開放されて気持ちよく目覚められるだろう。");
				strcpy(quest_text[line++], "あなたはとても満足げであった。");
#else
                strcpy(quest_text[line++], "After a fierce battle, you have defeated both your");
				strcpy(quest_text[line++], "master and the princess. That's going to be good stress");
				strcpy(quest_text[line++], "relief for the real you. You're very happy.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "お師匠様の力はあなたの想像以上だった。");
				strcpy(quest_text[line++], "まだ暴れ足りないが憂さ晴らしはここまでにしておこう。");
#else
                strcpy(quest_text[line++], "Your master's power was beyond your expectations.");
				strcpy(quest_text[line++], "You still haven't rampaged enough yet, but that'll be");
				strcpy(quest_text[line++], "enough stress relief for now.");
#endif
			}
		}


		break;
	case QUEST_MITHRIL: //ミスリル探索　ナズーリン
		if(pc == CLASS_NAZRIN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "見事「ミスリル」を発見した。");
				strcpy(quest_text[line++], "早速一輪に加工してもらったが・・");
#else
                strcpy(quest_text[line++], "You did well in finding some mithril.");
				strcpy(quest_text[line++], "You quickly handed it over to Ichirin...");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "やはり単独での深層探索は無理があるようだ。");//一行40字まで
				strcpy(quest_text[line++], "いずれご主人の力で何とかしてもらおう・・");//一行40字まで
#else
                strcpy(quest_text[line++], "Just as you expected, exploring deep dungeons is too much");//一行40字まで
				strcpy(quest_text[line++], "for you to handle on your own. Maybe your master could");//一行40字まで
				strcpy(quest_text[line++], "do something...");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "鉄獄には神秘の金属「ミスリル」があるらしい。");
				strcpy(quest_text[line++], "これを使ってロッドを作れば宝探しが捗るに違いない。");
#else
                strcpy(quest_text[line++], "Looks like there's a mystic ore called mithril in Angband.");
				strcpy(quest_text[line++], "If you'd make a rod out of it, it'll be very useful for");
				strcpy(quest_text[line++], "treasure seeking.");
#endif
			}
		}
		break;
		//破滅2　聖
	case QUEST_DOOM2:
		if(pc == CLASS_BYAKUREN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "悪鬼たちは退散していった。まずは一安心だ。");
				strcpy(quest_text[line++], "あなたの身を案じた弟子たちが癒しの力を持つ杖を手に入れてくれた。");
				strcpy(quest_text[line++], "ありがたく使わせてもらおう。");
#else
                strcpy(quest_text[line++], "You've banished the demons; that's a relief.");
				strcpy(quest_text[line++], "Your thoughtful disciples presented you with a staff with");
				strcpy(quest_text[line++], "healing power. You accepted it with gratitude.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "悪鬼たちは予想以上に強力で手に負えなかった。");
				strcpy(quest_text[line++], "幻想郷の他の実力者たちを頼って何とか押さえ込んだが、");
				strcpy(quest_text[line++], "しばらくは彼女たちに頭が上がりそうにない・・");
#else
                strcpy(quest_text[line++], "Those demons were too strong, and ended up too much for");
				strcpy(quest_text[line++], "you to handle. You ended up asking other influental");
				strcpy(quest_text[line++], "Gensoukyou residents to clean them up, but now you'll");
				strcpy(quest_text[line++], "be ashamed to face them for a while...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "異界の悪鬼が命蓮寺に侵攻を企てている。");
				strcpy(quest_text[line++], "話は通じない。戦うしかないようだ。");
				strcpy(quest_text[line++], "悪鬼の棲家に一人で乗り込めば被害は最小限で済むはずだ。");
#else
                strcpy(quest_text[line++], "Demons from other world are planning to invade Myouren Temple.");
				strcpy(quest_text[line++], "They're not listening to you; your only option is to fight.");
				strcpy(quest_text[line++], "The damage will be kept to minimum if you infiltrate");
				strcpy(quest_text[line++], "their hideout alone.");
#endif
			}
		}
		else if(pc == CLASS_KYOUKO ||pc == CLASS_NAZRIN || pc == CLASS_ICHIRIN ||pc == CLASS_MURASA || pc == CLASS_SHOU )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "白蓮「お見事です。貴女はしばらく見ないうちに大変な修行を積んでいたのね。");
				strcpy(quest_text[line++], "これは里の方が寄進して下さった強力な治癒の杖です。");
				strcpy(quest_text[line++], "扱いの難しい品ですが、今の貴女ならたやすく使いこなせるはずよ。」");
#else
                strcpy(quest_text[line++], "Byakuren - 'Well done. I see you've been training a lot while you");
				strcpy(quest_text[line++], "were away. Here's a powerful healing staff donated by the village.");
				strcpy(quest_text[line++], "It's difficult to use, but you should be able to handle it.'");
#endif
			}
		}
		break;

		//ゲリラライブ　慧音
	case QUEST_LIVE:
		if(pc == CLASS_REIMU)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「博麗の巫女でも敵わないとは。");
				strcpy(quest_text[line++], "これは由々しき問題だぞ。いずれは里の存続にかかわるかもしれん・・」");
#else
                strcpy(quest_text[line++], "Keine - 'They're too much even for the Hakurei shrine maiden?");
				strcpy(quest_text[line++], "That's a serious problem. I'm worried what's going to happen");
				strcpy(quest_text[line++], "to the village...'");
#endif
			}

		}
		else if(pc == CLASS_KEINE)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは夜中に騒音を撒き散らすゲリラライブにもう我慢がならない。");
				strcpy(quest_text[line++], "里の者は不眠に悩まされているし、");
				strcpy(quest_text[line++], "このままではあなたの歴史編纂にも差し障りが出るかもしれない。");
				strcpy(quest_text[line++], "少々乱暴だが、殴り込みをかけて仕置きをくれてやることにした。");
				strcpy(quest_text[line++], "しかしライブには最近の異変で力を付けた強力な付喪神も参加しているようだ。");
				strcpy(quest_text[line++], "さらにライブの観客たちも全て相手にしないといけない。");
				strcpy(quest_text[line++], "入念な準備をしてから向かう必要があるだろう。");
#else
                strcpy(quest_text[line++], "You can no longer stand that Guerilla Live concert making noise");
				strcpy(quest_text[line++], "in the middle of night. The villagers are having hard time sleeping,");
				strcpy(quest_text[line++], "and at this rate, you'll have trouble recording history.");
				strcpy(quest_text[line++], "It might be a bit rough, but you decide to beat them up to make them");
				strcpy(quest_text[line++], "behave properly. However, some tsukumogami that were granted a lot of");
				strcpy(quest_text[line++], "power in a recent incident also are attending that concert. You");
				strcpy(quest_text[line++], "also might end up getting in combat with the spectators.");
				strcpy(quest_text[line++], "This will require proper preparations.");
#endif

			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは大激戦の末にゲリラライブを壊滅させた。");
				strcpy(quest_text[line++], "寺子屋に戻ったら稗田家から礼品が届いていた。");
				strcpy(quest_text[line++], "これは魔力を回復させる強力な杖だ。");
				strcpy(quest_text[line++], "しかし他の杖と同様、簡単に燃えたり壊れたりするはずだ。");
				strcpy(quest_text[line++], "大事に使わせてもらおう。");
#else
                strcpy(quest_text[line++], "You ended up destroying Guerilla Live after a fierce battle.");
				strcpy(quest_text[line++], "When you came back to the temple school, you noticed a reward");
				strcpy(quest_text[line++], "left for you by Hieda House. It's a powerful staff that can");
				strcpy(quest_text[line++], "recover mana. It's one of a kind, so you'll have to be careful");
				strcpy(quest_text[line++], "not to get it burnt up.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "興奮した強力な妖怪たちはとてもあなたの手には負えなかった。");
				strcpy(quest_text[line++], "やむなく博麗神社へ急使を出した。");
				strcpy(quest_text[line++], "博麗霊夢なら何とかしてくれることだろう・・");
#else
                strcpy(quest_text[line++], "Those excited and powerful youkai were too much for you to");
				strcpy(quest_text[line++], "handle. You had no choice but to send a courier to Hakurei");
				strcpy(quest_text[line++], "shrine. Reimu Hakurei should do something about it...");
#endif
			}

		}
		else if(pc == CLASS_KOSUZU)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「全く、ゲリラライブとかいう連中にも困ったものだな。");
				strcpy(quest_text[line++], "人里にも奴らの隠れファンがいるらしいが、お前は大丈夫だろうな？");
				strcpy(quest_text[line++], "あんな所に忍び込んだら命がいくつあっても足りないぞ。」");
#else
                strcpy(quest_text[line++], "Keine - 'This Guerilla Live group is causing so much trouble!");
				strcpy(quest_text[line++], "Human Village also has several of their fans; are you alright?");
				strcpy(quest_text[line++], "Don't risk your life sneaking into a place like this.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「あの騒ぎの中で大暴れして力づくで黙らせてきたというのか？");
				strcpy(quest_text[line++], "もはや英雄の器だな。後世畏るべしとはこのことか。");
				strcpy(quest_text[line++], "これを持っていくといい。」");
#else
                strcpy(quest_text[line++], "Keine - 'You're saying you subdued that noisy group by force?");
				strcpy(quest_text[line++], "You're already becoming quite the hero. Please, take this.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「だから妖怪は危険だと何度言えば…");
				strcpy(quest_text[line++], "先生はお前の将来が心配だぞ。」");
#else
                strcpy(quest_text[line++], "Keine - 'And I've been telling you that youkai are dangerous,");
				strcpy(quest_text[line++], "over and over... I'm getting worried about your future.'");
#endif
			}

		}
		else if(pc == CLASS_MOKOU)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "最近慧音は随分寝不足のようだ。話を聞いてみると、");
				strcpy(quest_text[line++], "里の近くでゲリラライブなる乱痴気騒ぎをする妖怪達がいるらしい。");
				strcpy(quest_text[line++], "それで里の住人が不眠に悩んでいるそうだ。");
				strcpy(quest_text[line++], "快く妖怪退治を引き受けてあげた。");
				strcpy(quest_text[line++], "ライブには強力な妖怪も混じっているらしい。");
				strcpy(quest_text[line++], "久しぶりに腕が鳴る。");
#else
                strcpy(quest_text[line++], "Keine has been having trouble sleeping lately, so you talked to");
				strcpy(quest_text[line++], "her about it; looks like there's a noisy group of youkai called");
				strcpy(quest_text[line++], "Guerilla Live near the village. The villagers also have hard time");
				strcpy(quest_text[line++], "sleeping. You took on the youkai extermination request.");
				strcpy(quest_text[line++], "Looks like there will be fairly strong youkai there; you haven't");
				strcpy(quest_text[line++], "tested your strength in a while.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "大激戦の末にゲリラライブを壊滅させた。");
				strcpy(quest_text[line++], "寺子屋に戻ったら稗田家から礼品が届いていた。");
				strcpy(quest_text[line++], "これは魔力を回復させる強力な杖らしい。");
				strcpy(quest_text[line++], "しかし他の杖と同様、簡単に燃えたり壊れたりするはずだ。");
				strcpy(quest_text[line++], "使い所には気をつけなくては。");
#else
                strcpy(quest_text[line++], "After an intense battle, you brought down Guerilla Live.");
				strcpy(quest_text[line++], "When you came back to the temple school, you noticed a reward");
				strcpy(quest_text[line++], "left to you by Hieda House. It's a powerful staff that can");
				strcpy(quest_text[line++], "recover mana. However, it's one of a kind, and can easily get");
				strcpy(quest_text[line++], "burned up. You'll have to be very careful with it.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "妖怪たちは予想以上に強力で、撤退を余儀なくされた。");
				strcpy(quest_text[line++], "妖怪退治は飽きるほどやったが、まだまだ修行が足りないようだ。");
#else
                strcpy(quest_text[line++], "The youkai were stronger than you expected, and you had to");
				strcpy(quest_text[line++], "withdraw. You probably didn't train enough if you can't handle");
				strcpy(quest_text[line++], "some youkai extermination.");
#endif
			}

		}
		else if(pc == CLASS_EIKI)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「閻魔様、申し訳ないのですがまた力を貸して頂けませんか？");
				strcpy(quest_text[line++], "夜中に近くの森で大騒ぎをする困った奴らがいまして、");
				strcpy(quest_text[line++], "里の者達が不眠に悩んでおります。");
				strcpy(quest_text[line++], "特に新顔の付喪神の連中がかなりの強さでどうにも手を出せないでいるのです。」");
#else
                strcpy(quest_text[line++], "Keine - 'Miss Yama, I'm terribly sorry, but couldn't you lend us a hand?");
				strcpy(quest_text[line++], "There's a troublesome group making noise in nearby forest in middle of");
				strcpy(quest_text[line++], "night, and villagers are having trouble sleeping.");
				strcpy(quest_text[line++], "In particular, their new tsukumogami is very strong, so I'd appreciate");
				strcpy(quest_text[line++], "your help in this matter.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「里の危機を救って頂き、感謝の言葉もございません。");
				strcpy(quest_text[line++], "これは魔力を回復させる強力な魔道具です。お礼の印にお受け取りください。」");
#else
                strcpy(quest_text[line++], "Keine - 'I can't find proper words of gratitude for saving our village");
				strcpy(quest_text[line++], "from this crisis. Here is a powerful magic device that can restore mana.");
				strcpy(quest_text[line++], "Please accept it as a sign of gratitude.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「何と、荒ぶる妖怪の力がこれほどのものとは。");
				strcpy(quest_text[line++], "急ぎ巫女に使いを出しましょう。総力であたらねばなりませんね。」");
#else
                strcpy(quest_text[line++], "Keine - 'Are those rampaging youkai really that strong?");
				strcpy(quest_text[line++], "I'm going to send for the shring maiden as soon as possible.");
				strcpy(quest_text[line++], "We'll have to act together.'");
#endif
			}

		}
		else if(pr == RACE_FAIRY)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「おっと済まない、うたた寝していたようだ。");
				strcpy(quest_text[line++], "最近夜になると大音量でライブを始める奴らがいてな。");
				strcpy(quest_text[line++], "皆寝不足でほとほと困っているんだよ。」");
#else
                strcpy(quest_text[line++], "Keine - 'Oh, sorry, I fell asleep for a bit.");
				strcpy(quest_text[line++], "Lately, there have been noisy live concerts in the middle");
				strcpy(quest_text[line++], "of night. Everyone's having trouble sleeping.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				if(pc == CLASS_3_FAIRIES)
					strcpy(quest_text[line++], "慧音「ライブ会場を壊滅させたのはお前達だったのか。");
				else
					strcpy(quest_text[line++], "慧音「ライブ会場を壊滅させたのはお前だったのか。");
#else
                strcpy(quest_text[line++], "Keine - 'You managed to bring down that live concert venue?");
#endif
#ifdef JP
				strcpy(quest_text[line++], "私達も手を焼いていたのに、妖精とは思えない強さだな。");
				strcpy(quest_text[line++], "礼の印にこれを使ってくれ。魔力を回復する強力な杖だ。");
				strcpy(quest_text[line++], "二本とない貴重品だからな。うっかり壊したり燃やされたりしないでくれよ。」");
#else
                strcpy(quest_text[line++], "They were too much even for me; I didn't think fairies could be");
				strcpy(quest_text[line++], "this strong. Please take this as a sign of gratitude. It's a powerful");
				strcpy(quest_text[line++], "staff that can restore mana. It's a one of a kind precious item. Be");
				strcpy(quest_text[line++], "careful and don't get it broken or burnt up.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "慧音「やはりこれ以上奴らを放置はできないな。");
				strcpy(quest_text[line++], "急ぎ巫女に使いを出すことにしよう。」");
#else
                strcpy(quest_text[line++], "Keine - 'I guess I can't leave them be anymore.");
				strcpy(quest_text[line++], "I'll send for the shrine maiden as soon as possible.'");
#endif
			}

		}

		break;


		//ダークエルフの王　河童、山童、天狗
	case QUEST_DARKELF:
		if(pr == RACE_KAPPA || pr == RACE_YAMAWARO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「む、お前か。");
				strcpy(quest_text[line++], "例の余所者連中のアジトがこの近くにあるらしい。");
				strcpy(quest_text[line++], "しかしこちらの戦力は心許ない。");
				strcpy(quest_text[line++], "私が囮になってるうちにアジトに侵入して親玉を倒せるか？」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Hey, you");
				strcpy(quest_text[line++], "There are some stranagers living in a hideout nearby.");
				strcpy(quest_text[line++], "However, we don't have enough power to deal with them on our own.");
				strcpy(quest_text[line++], "How about you sneak into their hideout and deal with their");
				strcpy(quest_text[line++], "bosses while I distract them?'");
#endif
			}

		}
		else if(pr == RACE_KARASU_TENGU || pr == RACE_HAKUROU_TENGU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「助かりました。これをお納め下さい。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'You were a big help. Please accept this.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「天狗様・・真面目にやって下さいよ。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Can't you tengu get your job done?'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「む、天狗様・・");
				strcpy(quest_text[line++], "・・実はこの近くに山を荒らす余所者のアジトがあるのです。");
				strcpy(quest_text[line++], "何とかして下さいませんか。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Ugh, a tengu...");
				strcpy(quest_text[line++], "...You know, there are some strangers living in a nearby hideout");
				strcpy(quest_text[line++], "that cause a ruckus on the mountain. Could you do something about it?'");
#endif
			}
		}
		else if(pr == RACE_ONI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「助かりました。これをお納め下さい。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'You were a big help. Please accept this.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「真面目にやって頂けませんか・・」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Were you even trying...'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「あ、貴方は・・");
				strcpy(quest_text[line++], "実はこの近くに山を荒らす余所者のアジトがあるのです。");
				strcpy(quest_text[line++], "何とかして下さいませんか。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Ah, you...");
				strcpy(quest_text[line++], "...You know, there are some strangers living in a nearby hideout");
				strcpy(quest_text[line++], "that cause a ruckus on the mountain. Could you do something about it?'");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "「何、全部倒しただと？");
				strcpy(quest_text[line++], "妖精にやられるとは、所詮奴らも見かけ倒しだったということか。");
				strcpy(quest_text[line++], "ご苦労だった。褒美にこれをやろう。」");
#else
                strcpy(quest_text[line++], "'You defeated them all?");
				strcpy(quest_text[line++], "Well, they probably weren't expecting to get done in by a");
				strcpy(quest_text[line++], "fairy either. Thank you for your hard work. Accept this is");
				strcpy(quest_text[line++], "your reward.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「敵にもそれなりの備えはあるようだな。");
				strcpy(quest_text[line++], "もう行っていいぞ。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Looks like our enemies are getting prepared.");
				strcpy(quest_text[line++], "You're free to go.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "山童の娘「なんだ妖精か。丁度いい。");
				strcpy(quest_text[line++], "西に洞穴が見えるな？あの中に忍び込んで、");
				strcpy(quest_text[line++], "中にどんな奴がいるか偵察してきてくれ。」");
#else
                strcpy(quest_text[line++], "Yamawaro - 'A fairy? You came at a good time.");
				strcpy(quest_text[line++], "You see that cave to the west? Go inside and check out");
				strcpy(quest_text[line++], "who's hiding there.'");
#endif
			}
		}
		break;

	//王家の墓	こいし,お燐,さとり
	case QUEST_CRYPT:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは黴臭い墓所に興味をなくした。");
				strcpy(quest_text[line++], "ふと横を見ると鎧のようなものが浮いている。");//一行40字まで
#else
                strcpy(quest_text[line++], "You've lost interest in exploring the moldy crypt.");
				strcpy(quest_text[line++], "Looks like you found some armor at some point.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "穴から大量の悪霊が溢れだし旧地獄の街は大騒ぎだ。");
				strcpy(quest_text[line++], "あなたはしばらく見ていたがやがて興味をなくしてその場を後にした。");
#else
                strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. You watched it for a bit, but then you lost");
				strcpy(quest_text[line++], "interest and walked away.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "姉のペット達が地下に向けて深く穴を掘っているのを見かけた。");
				strcpy(quest_text[line++], "あなたは何となく一緒に入ってみることにした。");
#else
                strcpy(quest_text[line++], "You saw your sister's pets digging a hole deep underground.");
				strcpy(quest_text[line++], "You decided to go along with them.");
#endif
			}
		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "どうにか強力な屍術師を倒した。");
				strcpy(quest_text[line++], "主から臨時ボーナスとして不思議な鎧をもらった。");//一行40字まで
#else
                strcpy(quest_text[line++], "You have managed to defeat the powerful necromancers.");
				strcpy(quest_text[line++], "You received a mysterious armor from your owner as a bonus.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "穴から大量の悪霊が溢れだし旧地獄の街は大騒ぎだ。");
				strcpy(quest_text[line++], "当分は主ともども針のむしろに座ることになりそうだ・・");
#else
                strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. Looks like your owner will have troubled times");
				strcpy(quest_text[line++], "ahead...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "最近夢見が悪いという主人の命で地下への穴を掘っていた。");
				strcpy(quest_text[line++], "主人の考えたとおり、地下に霊廟が見つかり中には怨念が渦巻いている。");
				strcpy(quest_text[line++], "ペットの中でも戦いの得意な自分が潜ることになった。");
				strcpy(quest_text[line++], "『アーチリッチ』という術師を五体倒してほしいそうだ。");
#else
                strcpy(quest_text[line++], "Your owner has been having bad dreams lately, and she gave you");
				strcpy(quest_text[line++], "orders to dig a tunnel deeper underground. Just as she thought,");
				strcpy(quest_text[line++], "you discovered an underground mausoleum filled with evil spirits.");
				strcpy(quest_text[line++], "You might be just a pet, but you still have combat skills, so");
				strcpy(quest_text[line++], "you're going inside. Defeating five Archliches should get the");
				strcpy(quest_text[line++], "job done.");
#endif
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "どうにか強力な屍術師を倒した。");
				strcpy(quest_text[line++], "部屋に戻ると見覚えのない鎧が浮いていた。");//一行40字まで
#else
                strcpy(quest_text[line++], "You have managed to defeat the powerful necromancers.");
                strcpy(quest_text[line++], "As you return to your room, you notice a suit of armor you");//一行40字まで
                strcpy(quest_text[line++], "don't recall seeing before.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "穴から大量の悪霊が溢れだし旧地獄の街は大騒ぎだ。");
				strcpy(quest_text[line++], "当分は針のむしろに座ることになりそうだ・・");
#else
				strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. Looks like you will have troubled times ahead...");
#endif // JP
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "地下に霊廟が見つかり、中に強力な怨念が渦巻いている。");
				strcpy(quest_text[line++], "このままでは夢見が悪い。何とか倒してしまいたい。");
#else
                strcpy(quest_text[line++], "You have discovered an underground mausoleum filled with");
				strcpy(quest_text[line++], "powerful evil spirits. They keep causing you bad dreams.");
				strcpy(quest_text[line++], "You have decided to go in and deal with them.");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「閻魔様とはいえ異界の死者は管轄外でしたか。");
				strcpy(quest_text[line++], "この件の始末は是非曲直庁との共同ということでお願いしますね。」");
#else
                strcpy(quest_text[line++], "Satori - 'Those otherworldly undead were out of your");
				strcpy(quest_text[line++], "jurisdiction, Miss Yama? I'd like to cooperate with Ministry");
				strcpy(quest_text[line++], "of Right and Wrong to get this incident resolved.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「何度も手を煩わせて済みません。");
				strcpy(quest_text[line++], "墓所の宝物は全て彼岸の運営費にお納めください。");
				strcpy(quest_text[line++], "それからこちらも差し上げます。");
				strcpy(quest_text[line++], "いつからここにあったのか誰にも分からないのですが、高価なものではあるようです。」");
#else
                strcpy(quest_text[line++], "Satori - 'I'm terribly sorry to have bothered you.");
				strcpy(quest_text[line++], "You're free take all the treasures in the crypt to finance");
				strcpy(quest_text[line++], "Higan operations. Also, I'm giving you this.");
				strcpy(quest_text[line++], "I don't know since when it has been here, but it looks");
				strcpy(quest_text[line++], "very valuable.'");
#endif
			}
		}

		break;
		//神への挑戦　霊夢・早苗
	case QUEST_KANASUWA:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "早苗「相変わらずの強さですね。悔しいですがお見事です！");
				strcpy(quest_text[line++], "それではこちらが賞品になります。良い物当たるといいですね。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Sanae - 'You're as strong as I expected. I don't like saying it,");
				strcpy(quest_text[line++], "but well done! Here's your prize. Hope you'll get lucky.'");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "早苗「あらいつもの冴えはどうしたんです？");
				strcpy(quest_text[line++], "そんなことじゃいずれ私達が幻想郷の信仰全部頂いちゃいますよ？」");
#else
                strcpy(quest_text[line++], "Sanae - 'Have you been neglecting your training lately?");
				strcpy(quest_text[line++], "At this rate, all of the faith in Gensoukyou will be ours!'");
#endif
			}
		}
		else if(pc == CLASS_SAGUME)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "早苗「むむ…本気出したら強いじゃないですか。");
				strcpy(quest_text[line++], "やっぱりまだ地上侵略を企ててるんじゃないですか？」");//一行40字まで
#else
                strcpy(quest_text[line++], "Sanae - 'Uhh... You're pretty strong when not holding back.");
				strcpy(quest_text[line++], "Are you planning to invade Earth again?'");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "早苗「あらあら大丈夫ですか？");
				strcpy(quest_text[line++], "どうです、うちの神様はすごいんですよ！」");
#else
                strcpy(quest_text[line++], "Sanae - 'Oh, my, are you alright?");
				strcpy(quest_text[line++], "How did you like my gods? Aren't they awesome?'");
#endif
			}
		}
		else if(pc == CLASS_SANAE)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "見事修行を完遂した。ご褒美として高価そうな巻物を賜った。");
				strcpy(quest_text[line++], "やはり例の企画の賞品のようだが、信仰心を発揮して黙っていることにした。");//一行40字まで
#else
                strcpy(quest_text[line++], "You splendidly finished your training and received a valuable scroll");
				strcpy(quest_text[line++], "as a reward. As expected, it looks like that was the planned prize");//一行40字まで
				strcpy(quest_text[line++], "for the festival, but you decide to keep quiet.");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "神々の力は圧倒的であった。まだまだ若輩者と自らを戒めた。");
#else
                strcpy(quest_text[line++], "The power of your gods truly is overwhelming...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "神奈子様から「たまには直々に修行をつけてやろう」との有難いお言葉を頂いた。");
				strcpy(quest_text[line++], "きっと例の募集への挑戦者が今年もゼロだったから退屈していらっしゃるのだろう。");
				strcpy(quest_text[line++], "諏訪子様も最近退屈そうなのでひょっとしたら乱入して来られるかもしれない。");
				strcpy(quest_text[line++], "しっかり準備を済ませてから帰ろう。");
#else
                strcpy(quest_text[line++], "Lady Kanako gave a request about 'doing some one-on-one training'.");
				strcpy(quest_text[line++], "She's probably bored because nobody's daring to challenge her at");
				strcpy(quest_text[line++], "current festival. Lady Suwako also seems to be bored lately, so she might");
				strcpy(quest_text[line++], "jump in as well. You'll have to be prepared.");
#endif
			}
		}
		break;
		//宴会幹事
	case QUEST_HANAMI:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "霊夢「あら魔理沙。次の宴会の幹事はあんたよ。忘れてないでしょうね？");
				strcpy(quest_text[line++], "できれば飛び切り強いお酒を持ってきてよ。");
				strcpy(quest_text[line++], "ちょっと酔い潰してやりたい天狗が居るの。」");
#else
                strcpy(quest_text[line++], "Reimu - 'Oh, Marisa. You haven't forgot you're responsible for arranging");
				strcpy(quest_text[line++], "the next banquet? I'd like you to bring some really strong sake.");
				strcpy(quest_text[line++], "There's a certain tengu I want to knock out.'");
#endif
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "霊夢「次の宴会の幹事はあんたよ。忘れてないでしょうね？");
				strcpy(quest_text[line++], "できれば飛び切り強いお酒を持ってきてよ。");
				strcpy(quest_text[line++], "ちょっと酔い潰してやりたい天狗が居るの。」");
#else
                strcpy(quest_text[line++], "Reimu - 'You haven't forgot you're responsible for arranging");
				strcpy(quest_text[line++], "the next banquet? I'd like you to bring some really strong sake.");
				strcpy(quest_text[line++], "There's a certain tengu I want to knock out.'");
#endif
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "霊夢「あら小鈴ちゃんじゃない。次の宴会に参加してみない？");
				strcpy(quest_text[line++], "できれば小鈴ちゃんにもお酒を持ち寄って欲しいわね。");
				strcpy(quest_text[line++], "なるべく強いのを一本お願いするわ。」");
#else
                strcpy(quest_text[line++], "Reimu - 'Oh, it's you, Kosuzu. Do you want to visit the next");
				strcpy(quest_text[line++], "banquet? I'd like you to bring some sake as well if possible.");
				strcpy(quest_text[line++], "Bring something particularly strong.'");
#endif
			}
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "霊夢「そう？まあ無理強いはしないわ。");
				strcpy(quest_text[line++], "早苗から聞いたけど、『あるはら』は良くないらしいし。」");
#else
                strcpy(quest_text[line++], "Reimu - 'Well, I'm not forcing you to do it.");
				strcpy(quest_text[line++], "Sanae told me that's called 'alcohol harassment', and that's");
				strcpy(quest_text[line++], "a bad thing to do.'");
#endif
			}
		}

		break;

	case QUEST_KORI: //狐狸戦争　成功時セリフを書き換える
			if(comp)
			{
				if((quest[31].flags & QUEST_FLAG_TEAM_A) && p_ptr->pclass != CLASS_RAN)
				{
					if(p_ptr->pclass == CLASS_CHEN)
					{
#ifdef JP
						strcpy(quest_text[line++], "藍「よくやったな。お前の成長に私も鼻が高いぞ。");
						strcpy(quest_text[line++], "ご褒美にこれをあげよう。大事に使ってくれよ。」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Ran - 'Well done! I'm proud of how you've grown.");
						strcpy(quest_text[line++], "Please take this as your reward. Use it well.'");//一行40字まで
#endif
					}
					else if(p_ptr->pclass == CLASS_YUKARI)
					{
#ifdef JP
						strcpy(quest_text[line++], "藍「ゆ、紫様！？");
						strcpy(quest_text[line++], "いえこの決闘は止むに止まれぬ事情があってのことで、");//一行40字まで
						strcpy(quest_text[line++], "けして命令に背くつもりは・・」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Ran - 'L-lady Yukari!?");
						strcpy(quest_text[line++], "Uh, this battle was due to unavoidable circumstances,");//一行40字まで
						strcpy(quest_text[line++], "I've had no intention of disobeying you...'");//一行40字まで
#endif
					}
					else if(p_ptr->pclass == CLASS_YUYUKO)
					{
#ifdef JP
						strcpy(quest_text[line++], "藍「お力添え有難うございました。");
						strcpy(quest_text[line++], "お礼の印としてこれをお納め下さい。");//一行40字まで
#else
                        strcpy(quest_text[line++], "Ran - 'Thank you for your support.");
						strcpy(quest_text[line++], "Please accept this as a sign of gratitude.'");//一行40字まで
#endif
					}
					else
					{
#ifdef JP
						strcpy(quest_text[line++], "藍「ふん、これで狸共も身の程を思い知っただろう。");
						strcpy(quest_text[line++], "お前もなかなか良い働きだったぞ。これは褒美だ。」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Ran - 'Hmph, I hope those tanuki have learned their place.");
						strcpy(quest_text[line++], "You did pretty good. Here's your reward.'");//一行40字まで
#endif
					}
				}
				else if((quest[31].flags & QUEST_FLAG_TEAM_B) && p_ptr->pclass != CLASS_MAMIZOU)
				{
					if(pc == CLASS_NUE)
					{
#ifdef JP
						strcpy(quest_text[line++], "マミゾウ「どうじゃ狐共め、狸の力を見たか！");
						strcpy(quest_text[line++], "助太刀感謝するぞい。これは礼じゃ。」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Mamizou - 'See, foxes? That's the power of the tanuki for you!");
						strcpy(quest_text[line++], "Thank you for your assistance. Here's your reward.'");//一行40字まで
#endif
					}
					else if(pc == CLASS_YUKARI)
					{
#ifdef JP
						strcpy(quest_text[line++], "マミゾウ「あの狐はあんさんの手下じゃなかったんか？");
						strcpy(quest_text[line++], "まああっちに付かれてもそれはそれで興醒めじゃが・・");//一行40字まで
						strcpy(quest_text[line++], "これを持って行け。これで貸し借りなしじゃ。」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Mamizou - 'Wasn't that fox one of your servants?");
						strcpy(quest_text[line++], "Well, I don't mind that if you're fighting on our side...");//一行40字まで
						strcpy(quest_text[line++], "Take this with you. That'll be your payment.'");//一行40字まで
#endif
					}
					else
					{
#ifdef JP
						strcpy(quest_text[line++], "マミゾウ「どうじゃ狐共め、狸の力を見たか！");
						strcpy(quest_text[line++], "お主も意外にやるのう。これを取っておけ。」");//一行40字まで
#else
                        strcpy(quest_text[line++], "Mamizou - 'See, foxes? That's the power of the tanuki for you!");
						strcpy(quest_text[line++], "You did pretty good! Take this with you.'");//一行40字まで
#endif
					}
				}
				else if(pc == CLASS_NITORI)
				{
#ifdef JP
					strcpy(quest_text[line++], "狐も狸も口ほどにもない。あなたは全員をのしてしまった。");
					strcpy(quest_text[line++], "あなたは飛び入りの前に《無効試合になる》という大穴に賭けていた。");
					strcpy(quest_text[line++], "賭けはあなたの一人勝ちだ。仲間からの怨嗟の声も心地よい。");
#else
                    strcpy(quest_text[line++], "Both foxes and tanuki are fools. You're going to come out on top.");
					strcpy(quest_text[line++], "Before going in, you made a large bet on the match ending up");
					strcpy(quest_text[line++], "inconclusive - and you're the only one to make this bet.");
					strcpy(quest_text[line++], "Hearing the anguished voices of your friends is also a plus.");
#endif
				}
				else
				{
#ifdef JP
					if(p_ptr->prace == RACE_HUMAN)
						strcpy(quest_text[line++], "にとり「いやっほう！最高だよ盟友！");
					else
						strcpy(quest_text[line++], "にとり「いやっほう！貴方最高だよ！");

					strcpy(quest_text[line++], "おかげで私の一人勝ち・・いやいやこっちの話。");//一行40字まで
					strcpy(quest_text[line++], "お祝いにこれあげる。自信作の斥力発生装置だ。");
					strcpy(quest_text[line++], "なあに気にするなって。私の気持ちだよ。」");
#else
                    if(p_ptr->prace == RACE_HUMAN)
						strcpy(quest_text[line++], "Nitori - 'Yahoo! You're the best, my friend!");
					else
						strcpy(quest_text[line++], "Nitori - 'Yahoo! You're the best!");

					strcpy(quest_text[line++], "Thanks to you, I've won... wait, I'm getting sidetracked.");//一行40字まで
					strcpy(quest_text[line++], "Take this as your reward. It's a forcefield generator I've");
					strcpy(quest_text[line++], "made myself.'");
#endif
				}
			}
			if(pc == CLASS_NITORI)
			{
				if(fail)
				{
#ifdef JP
					strcpy(quest_text[line++], "あなたは狐と狸の大決戦から這う這うの体で逃げ出した。");
					strcpy(quest_text[line++], "少し見通しが甘かったようだ。");
#else
                    strcpy(quest_text[line++], "You somehow managed to run away from the battle of foxes and tanuki.");
					strcpy(quest_text[line++], "Looks like you underestimated the situation a bit.");
#endif
				}
				if(accept)
				{
#ifdef JP
					strcpy(quest_text[line++], "久しぶりにアジトに戻ると河童の仲間達が賭けで盛り上がっている。");
					strcpy(quest_text[line++], "話を聞いてみると、狐と狸が麓の林で大規模な決闘をするらしい。");
					strcpy(quest_text[line++], "うまくやれば一儲けできるかもしれない。");
#else
                    strcpy(quest_text[line++], "As you return to your hideout, you see your kappa friends taking bets.");
					strcpy(quest_text[line++], "Looks like it's about a battle between foxes and tanuki that's");
					strcpy(quest_text[line++], "going to take place in a forest at the bottom of the mountain.");
					strcpy(quest_text[line++], "This might be a good chance to make a big profit.");
#endif
				}
			}
			else if(pc == CLASS_KOSUZU)
			{
				if(accept)
				{
#ifdef JP
					strcpy(quest_text[line++], "にとり「あれ？たしかあんたは貸本屋で…おっと。");
					strcpy(quest_text[line++], "まあいいや。これから麓の林で狐と狸の決闘が始まるんだ。");
					strcpy(quest_text[line++], "面白いから見ていくといいよ。");
					strcpy(quest_text[line++], "飛び入りしたいなら止めないけど、命が惜しけりゃ止しとくんだね。」");
#else
                    strcpy(quest_text[line++], "Nitori - 'Ah? You're from that bookstore...");
					strcpy(quest_text[line++], "Oh, well. A battle between foxes and tanuki is about to start");
					strcpy(quest_text[line++], "in a forest at the bottom of the mountain. Feel free to watch if");
					strcpy(quest_text[line++], "you're interested. I'm not going to stop you from jumping in,");
					strcpy(quest_text[line++], "but you shouldn't do it if you value your life.'");
#endif
				}
				if(fail)
				{
#ifdef JP
					strcpy(quest_text[line++], "にとり「まさかほんとに飛び入りするとは思わなかったね。");
					strcpy(quest_text[line++], "怪我してるなら薬を塗ってやろうか？５％引でいいよ。」");
#else
                    strcpy(quest_text[line++], "Nitori - 'Wow, I didn't expect you to actually charge in.");
					strcpy(quest_text[line++], "If you're wounded, I have some healing potions with me;");
					strcpy(quest_text[line++], "I'll even give you a 5% discount.'");
#endif
				}

			}

		break;

	case QUEST_WANTED_SEIJA: //お尋ね者　紫
		if(pc == CLASS_YUKARI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "賞金首の手配書を剥がした。");
				strcpy(quest_text[line++], "折角苦労して準備した豪華賞品も自分で使うことになりそうだ。");
#else
                strcpy(quest_text[line++], "Bounty hunt complete.");
				strcpy(quest_text[line++], "You're now free to use the valuable rewards that have been");
				strcpy(quest_text[line++], "prepared for this occassion.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "かの天邪鬼は幻想郷中の住人を敵に回して未だ逃げおおせている。");
				strcpy(quest_text[line++], "このまま放置してはおけない。そろそろ直々に引導を渡してくれよう。");
#else
                strcpy(quest_text[line++], "Looks like that amanojaku is still running around even after");
				strcpy(quest_text[line++], "having all the residents of Gensoukyou turn against her. You can't");
				strcpy(quest_text[line++], "leave her be like that. Time to confront her directly.");
#endif
			}
		}

		break;

	case QUEST_SPAWNING_PIT: //増殖地獄　にとり
		if(pc == CLASS_NITORI || pr == RACE_KAPPA)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは大激戦の末に巣食っていた怪物を全て倒し、河童の里の英雄となった。");
				strcpy(quest_text[line++], "河童仲間から里の宝物の強力な鎧を預かった。");
#else
                strcpy(quest_text[line++], "After a fierce battle, you defeated all those weird monsters living");
				strcpy(quest_text[line++], "here, becoming a hero among the kappas. Your kappa friends gave");
				strcpy(quest_text[line++], "you a powerful suit of armor from the village treasury.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "健闘むなしく、水源は汚染されてしまった。");//一行40字まで
				strcpy(quest_text[line++], "あなたも今後の身の振り方を考えないといけないだろう・・");//一行40字まで
#else
                strcpy(quest_text[line++], "You didn't fight well enough, and your water source got polluted.");//一行40字まで
				strcpy(quest_text[line++], "Looks like you'll have to change your way of living from now on...");//一行40字まで
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "大事な水源に異変が起こっている。");
				strcpy(quest_text[line++], "謎の肉塊のような怪物が次々と新たな怪物を生み出しているのだ。");
				strcpy(quest_text[line++], "このままでは水源が枯れてしまう。何とか水源を浄化せねばならない。");
#else
                strcpy(quest_text[line++], "An incident has occured at your water source.");
				strcpy(quest_text[line++], "There are mysterious flesh mound monsters that keep summoning new");
				strcpy(quest_text[line++], "monsters one after another. At this, your water source is going to");
				strcpy(quest_text[line++], "wither away. You have to do everything you can to purify it.");
#endif
			}
		}
		break;
	//サトリコンフリクト	こいし,お燐
	case QUEST_SATORI:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「こいし、貴方だったの・・助かったわ。");
				strcpy(quest_text[line++], "何、その本が欲しいの？");
				strcpy(quest_text[line++], "ええ、その本なら好きに持って行っていいわ。」");
#else
                strcpy(quest_text[line++], "Satori - 'Koishi, you... you helped me.");
				strcpy(quest_text[line++], "What, do you want that book?");
				strcpy(quest_text[line++], "Very well, take that book with you if that's what you'd like.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "魔理沙は沢山の本を持って高笑いしながら逃げていった。");
				strcpy(quest_text[line++], "たしかあの中には姉が厳重に保管している本もあった。今度見せてもらおう。");
#else
                strcpy(quest_text[line++], "Marisa run away laughing, carrying plenty of books.");
				strcpy(quest_text[line++], "You're certain one of them was that book your sister was very intent");
				strcpy(quest_text[line++], "on keeping safe.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "何だか地霊殿が物々しい感じだ。何かを迎え撃つつもりらしい。");
				strcpy(quest_text[line++], "何が来るのだろうか。あなたも一緒に待ってみることにした。");
#else
                strcpy(quest_text[line++], "Palace of Earth Spirits is busy for some reason. Looks like they're");
				strcpy(quest_text[line++], "preparing for an attack. Who it could be? You decide to wait for it");
				strcpy(quest_text[line++], "along with everyone else.");
#endif
			}
		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「まさかあんな物を持ち出してくるとはね・・");
				strcpy(quest_text[line++], "でもどうにか撃退できたようね。ありがとう。");//一行40字まで
				strcpy(quest_text[line++], "貴方が欲しがってた本をあげるわ。持っていきなさい。");//一行40字まで
#else
                strcpy(quest_text[line++], "Satori - 'I can't believe she brought something like this...");
				strcpy(quest_text[line++], "Even then, we managed to drive her away. Thank you.");//一行40字まで
				strcpy(quest_text[line++], "Here's that book you wanted. Take it with you.'");//一行40字まで
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「丁度良いところに帰ってきたわね。");
				strcpy(quest_text[line++], "あの白黒魔法使いが書斎を襲撃しに来るかもしれないの。");
				strcpy(quest_text[line++], "貴方も防衛を手伝ってちょうだい。」");
#else
                strcpy(quest_text[line++], "Satori - 'You came back at a good time.");
				strcpy(quest_text[line++], "That black and white magician might come to attack my study");
				strcpy(quest_text[line++], "I'd like you to lend a hand in defending it.'");
#endif
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「お世話になりました。お礼にこの本を差し上げます。");
				strcpy(quest_text[line++], "あの魔法使いの目当ての品だったようですが、");//一行40字まで
				strcpy(quest_text[line++], "もう手放してしまった方がよいだろうと思いまして。」");//一行40字まで
#else
                strcpy(quest_text[line++], "Satori - 'You were a big help. As a reward, please accept this book.");
				strcpy(quest_text[line++], "Looks like that magician was trying to get it,");//一行40字まで
				strcpy(quest_text[line++], "so I'm deciding to let go of it.'");//一行40字まで
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "さとり「あら閻魔様。丁度良いところにいらっしゃいました。");
				strcpy(quest_text[line++], "地霊殿が泥棒・・というより強盗の襲撃を受ける恐れがあるのです。");
				strcpy(quest_text[line++], "よろしければ賊の捕獲に力を貸していただけませんか？」");
#else
                strcpy(quest_text[line++], "Satori - 'Oh, Miss Yama. I'm grateful you came at a good time.");
				strcpy(quest_text[line++], "I'm afraid Palace of Earth Spirits is going to be robbed...");
				strcpy(quest_text[line++], "Or, to be more precise, burglarized. Could you lend us you");
				strcpy(quest_text[line++], "strength and defend it?'");
#endif
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "物言わぬゴーレムはかなりの強敵だったが、どうにか賊の撃退に成功した。");
				strcpy(quest_text[line++], "この高額な魔法書が目当てだったようだが、もう売り払ってしまおうか？");
#else
                strcpy(quest_text[line++], "Those silent golems were very strong, but you managed to drive");
				strcpy(quest_text[line++], "the burglar away. She was trying to get this valuable spellbook;");
				strcpy(quest_text[line++], "was she going to sell it?");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "本泥棒は高笑いしながら逃げていった。");
				strcpy(quest_text[line++], "被害を調査したところ、人に見せられない習作を入れておいた");
				strcpy(quest_text[line++], "鍵付きの箱が引き出しの奥からいつの間にか消えていた。");
				strcpy(quest_text[line++], "何ということだろう。さっさと燃やしてしまうべきだったのだ。");
#else
                strcpy(quest_text[line++], "The book thief ran away laughing.");
				strcpy(quest_text[line++], "As you were assessing the damage, you've noticed that the locked chest");
				strcpy(quest_text[line++], "in which you were keeping one of your works you'd rather not let");
				strcpy(quest_text[line++], "anyone see has gone missing. What is going on? You knew you");
				strcpy(quest_text[line++], "should've burned it.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "あの白黒魔法使いが書斎を襲撃しに来るかもしれない。");
				strcpy(quest_text[line++], "ペット達を集めて防衛戦を始めることにした。");
#else
                strcpy(quest_text[line++], "That white and black magician might come and attack your study.");
				strcpy(quest_text[line++], "You've gathered your pets and are preparing to repel the attack.");
#endif
			}
		}

		break;

	case QUEST_WANTED_SUMIREKO: //ようこそ幻想郷へ　深秘録の面々 ぬえも追加
		if(pc == CLASS_MAMIZOU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "華扇「計画はうまく行ったわ。");
				strcpy(quest_text[line++], "今頃例の超能力者は皆に追い回されているだろう。");
				strcpy(quest_text[line++], "何ならお前も参加するといい。」");
#else
                strcpy(quest_text[line++], "Kasen - 'Everything's going according to plan.");
				strcpy(quest_text[line++], "Everyone's chasing after that mindcrafter girl.");
				strcpy(quest_text[line++], "Feel free to go after her as well.'");
#endif
			}
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "華扇「まさかボールにあんな秘密があったなんて。");
				strcpy(quest_text[line++], "今霊夢が外に行ったが事によっては一大事だ。");
				strcpy(quest_text[line++], "今のうちにこれを飲んでおけ。」");
#else
                strcpy(quest_text[line++], "Kasen - 'I didn't expect those balls to hide that kind");
				strcpy(quest_text[line++], "of secret. Reimu has gone outside right now, she'll");
				strcpy(quest_text[line++], "finish it. As for you, drink this.'");
#endif
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "マミゾウ「おや、絵巻を貸してくれる気になったか？");
				strcpy(quest_text[line++], "違うのか。そりゃ残念じゃ。");
				strcpy(quest_text[line++], "それはさておき、お前さんは超能力というものを知っておるな？");
				strcpy(quest_text[line++], "今その超能力者があちこちを彷徨っておる。");
				strcpy(quest_text[line++], "出会ったら襲い掛かってくるかもしれんから気をつけることじゃ。」");
#else
                strcpy(quest_text[line++], "Mamizou - 'Hey, wouldn't you lend me that picture scroll?");
				strcpy(quest_text[line++], "No? Oh, well.");
				strcpy(quest_text[line++], "That aside, have you heard about mindcrafters?");
				strcpy(quest_text[line++], "There's a mindcrafter girl running around right now.");
				strcpy(quest_text[line++], "She might attack you if you run into her, so be careful.'");
#endif
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "マミゾウ「なんじゃ、お前さんまであの女学生に懸想しておるのか？");
				strcpy(quest_text[line++], "まあ事が結界破りとあっては止むを得んか。");
				strcpy(quest_text[line++], "仕置きは程々にしてやるんじゃぞ。」");
#else
                strcpy(quest_text[line++], "Mamizou - 'What, even you're getting concerned about that");
				strcpy(quest_text[line++], "schoolgirl? Well, we can't let her break the barrier down.");
				strcpy(quest_text[line++], "Discipline her as you see fit.'");
#endif
			}
		}
		else if(pc == CLASS_MARISA || pc == CLASS_KASEN || pc == CLASS_SHINMYOUMARU || pc == CLASS_SHINMYOU_2
			|| pc == CLASS_BYAKUREN || pc == CLASS_MIKO || pc == CLASS_NITORI || pc == CLASS_KOISHI
			|| pc == CLASS_KOKORO || pc == CLASS_NUE || pc == CLASS_ICHIRIN || pc == CLASS_MOKOU
			|| pc == CLASS_FUTO || pc == CLASS_REIMU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "マミゾウ「計画通り、件の女学生はこちらに来たようじゃな。");
				strcpy(quest_text[line++], "さ、幻想郷の恐ろしさをたっぷり教えてやろうぞ。」");
#else
                strcpy(quest_text[line++], "Mamizou - 'Everything's going according to plan - that schoolgirl");
				strcpy(quest_text[line++], "is going to come by soon. Let's teach her about how scary");
				strcpy(quest_text[line++], "Gensoukyou can be.'");
#endif
			}
			if(comp && pc == CLASS_REIMU)
			{
#ifdef JP
				strcpy(quest_text[line++], "マミゾウ「何やら最後に想定外のことが起こったようじゃが、");
				strcpy(quest_text[line++], "うまくやってくれたらしいの。");
				strcpy(quest_text[line++], "ご苦労じゃった。これを取っておけ。」");
#else
                strcpy(quest_text[line++], "Mamizou - 'I didn't expect that turn of events, but it looks");
				strcpy(quest_text[line++], "like you've done well. Thank you for your hard work. Take");
				strcpy(quest_text[line++], "this with you.'");
#endif
			}
		}
		break;


		//切り札はいつだって悪手　魔理沙
	case QUEST_HECATIA:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "サグメ「月の民が夢の世界から戻ってこないの。");
				strcpy(quest_text[line++], "どうやら純狐の仲間が月の民を夢に縛り付けているらしいわ。");
				strcpy(quest_text[line++], "もう一度夢の世界に向かい、敵を打ち倒しなさい。");
				strcpy(quest_text[line++], "言うまでもないけど、私が語ったからには拒否しても無駄よ。」");
#else
                strcpy(quest_text[line++], "Sagume - 'The Lunarians haven't come back from the Dream World.");
				strcpy(quest_text[line++], "They might be held there by an ally of Junko.");
				strcpy(quest_text[line++], "Head out to the Dream World once more and defeat the enemy.");
				strcpy(quest_text[line++], "I don't think I need to say this, but you can't refuse");
				strcpy(quest_text[line++], "now that I've told you about it.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "疲労困憊して家に戻ると玄関に見覚えのない箱が置かれていた。");
				strcpy(quest_text[line++], "どうやらこれが報酬のつもりらしい。");
				strcpy(quest_text[line++], "これだから月の連中は虫が好かないんだ。");
#else
                strcpy(quest_text[line++], "As you come back to your house exhausted, you notice some box");
				strcpy(quest_text[line++], "in front of your door. Looks like it's supposed to be your reward.");
#endif
			}
		}
		else if(pc == CLASS_SAGUME)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "月の民が夢の世界から戻ってこない。また地上の人間を使って事態を収拾しよう。");
				strcpy(quest_text[line++], "そう考えたあなたは白黒の魔法使いの所に向かった。");
				strcpy(quest_text[line++], "しかしそこにいた氷精の挑発に乗せられてうっかり余計な一言を口走ってしまった。");
				strcpy(quest_text[line++], "もはや月の民を救うためには自分が夢の世界に行くしかない。");
#else
                strcpy(quest_text[line++], "The Lunarians haven't come back from the Dream World. You once");
				strcpy(quest_text[line++], "again decide to get things done by using one of the Earth residents.");
				strcpy(quest_text[line++], "With that in mind, you head to the black and white magician's house.");
				strcpy(quest_text[line++], "However, after being provoked by some ice fairy, you accidentally blurt");
				strcpy(quest_text[line++], "out something you shouldn't have said. Now you have no choice but to");
				strcpy(quest_text[line++], "head to the Dream World yourself to rescue Lunarians.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "月の民を夢の世界から救うことに成功した。");
				strcpy(quest_text[line++], "月の都から礼品が送られてきた。");
#else
                strcpy(quest_text[line++], "You've successfully rescued your fellow Lunarians.");
				strcpy(quest_text[line++], "Lunar Capital sent you a reward.");
#endif
			}
		}
		else if(pc == CLASS_TOYOHIME || pc == CLASS_YORIHIME)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "月の民が夢の世界から戻ってこない。");
				strcpy(quest_text[line++], "どうやら純狐の一味から攻撃を受けているようだ。");
				strcpy(quest_text[line++], "今地上で自由に動ける戦力はあなたしかいない。");
				strcpy(quest_text[line++], "槐安通路から夢の世界に行き、純狐の一味を打倒しなければならない。");
#else
                strcpy(quest_text[line++], "The Lunarians haven't come back from the Dream World.");
				strcpy(quest_text[line++], "Looks like they got attacked by one of Junko's allies.");
				strcpy(quest_text[line++], "Right now, you're the only one of Lunarian forces able");
				strcpy(quest_text[line++], "to freely move on Earth. You have to infiltrate the Dream World");
                strcpy(quest_text[line++], "and defeat that ally of Junko.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "月の民を夢の世界から救うことに成功した。");
				strcpy(quest_text[line++], "月の都から礼品が送られてきた。");
#else
                strcpy(quest_text[line++], "You've successfully rescued your fellow Lunarians.");
				strcpy(quest_text[line++], "Lunar Capital sent you a reward.");
#endif
			}
		}
		else if(pc == CLASS_DOREMY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "月の民がまだ夢の世界に居座っている。");
				strcpy(quest_text[line++], "どうやら純狐の仲間がまだ何か手を回しているらしい。");
				strcpy(quest_text[line++], "夢の世界の監視者として自分が一肌脱ぐことにした。");
#else
                strcpy(quest_text[line++], "The Lunarians are still residing in the Dream World.");
				strcpy(quest_text[line++], "Looks like one of Junko's allies is planning something.");
				strcpy(quest_text[line++], "Since you're responsible for maintaining the Dream World,");
				strcpy(quest_text[line++], "you decide to go and deal with it.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "どうにか強敵を倒し、月の民を夢の世界から追い出した。");
				strcpy(quest_text[line++], "なぜか魔理沙が礼の品を寄越してきた。");
				strcpy(quest_text[line++], "あの月人が裏でまた何か動いていたのかもしれない。");
#else
                strcpy(quest_text[line++], "You've defeated a powerful enemy and rescued the Lunarians.");
				strcpy(quest_text[line++], "For some reason, Marisa brought you a reward.");
				strcpy(quest_text[line++], "Those Lunarians might have been planning something");
				strcpy(quest_text[line++], "behind the scenes.");
#endif
			}
		}
		break;

		//紫打倒メインクエスト
	case QUEST_YUKARI:
		if (pc == CLASS_EIKI)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "幻想郷に深刻な異変が起こっている。");
				strcpy(quest_text[line++], "鉄獄と呼ばれる巨大なダンジョンが出現し異界の怪物が溢れ出したのだ。");
				strcpy(quest_text[line++], "鉄獄の中層で八雲紫が対応に追われているようだが、");
				strcpy(quest_text[line++], "そろそろあの者にも厳しい説教をくれてやらねばなるまい。");
#else
                strcpy(quest_text[line++], "A serious incident has occured in Gensoukyou.");
				strcpy(quest_text[line++], "A huge dungeon called Angband has appeared out of another world,");
				strcpy(quest_text[line++], "and monsters from other world are pouring out of it. Yukari Yakumo");
				strcpy(quest_text[line++], "has ran away into the middle levels of that dungeon; time to give her");
				strcpy(quest_text[line++], "a stern lecture.");
#endif
			}
		}

		break;

		//太歳星君打倒メインクエスト
	case QUEST_TAISAI:
		if(pc == CLASS_SEIJA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "鉄獄の底から強力な神が這い出そうとしているらしい。");
				strcpy(quest_text[line++], "あなたはそれを聞き、何としてもそれを埋め直してやろうと決意した。");
#else
                strcpy(quest_text[line++], "Looks like some powerful deity has crawled out from the depths of Angband.");
				strcpy(quest_text[line++], "After hearing this, you've decided to drive it back into the ground.");
#endif
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "アンバーの王の力で幻想郷の地下が鉄獄と繋がってしまった。");
				strcpy(quest_text[line++], "その影響であろうことか地中の太歳星君が掘り出されてしまった。");
				strcpy(quest_text[line++], "手遅れになる前に埋め直さないといけない。");
#else
                strcpy(quest_text[line++], "The Lords of Amber have connected the depths of Gensoukyou with");
				strcpy(quest_text[line++], "Angband - and they unearthed Taisui Xingjun from the depths in");
				strcpy(quest_text[line++], "the process. You have to bury it again before it's too late.");
#endif
			}
		}
		else if (pc == CLASS_MEIRIN)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "幻想郷が太歳星君の脅威に晒されている！");
				strcpy(quest_text[line++], "あのときの夢はやはりこの予兆だった！");
				strcpy(quest_text[line++], "この私こそが最前線で戦わなければ！");
#else
				strcpy(quest_text[line++], "Taisui Xingjun threatens Gensoukyou!");
				strcpy(quest_text[line++], "The dream you saw back then was an omen after all!");
				strcpy(quest_text[line++], "You absolutely have to go forth and fight!");
#endif
			}
		}

		break;

		//サーペント打倒メインクエスト
	case QUEST_SERPENT:
		//v1.1.18
		if(pc == CLASS_JUNKO || pc == CLASS_HECATIA || pc == CLASS_CLOWNPIECE || pc == CLASS_VFS_CLOWNPIECE)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "今回の異変の元凶は「混沌のサーペント」という名の強大な存在だった。");
				strcpy(quest_text[line++], "月の海にサーペントの棲む領域からの門が開き、いま月の都が侵略を受けている。");
				strcpy(quest_text[line++], "あなたはサーペントを倒しに行ってもいいし、月の都が滅ぶのをただ眺めていてもいい。");
#else
                strcpy(quest_text[line++], "The cause of this incident was a great being called Serpent of Chaos.");
				strcpy(quest_text[line++], "A gate to the Serpent's realm has opened at the Lunar Mare, and Lunar");
				strcpy(quest_text[line++], "Capital is being invaded. You can go and defeat the Serpent, or just");
				strcpy(quest_text[line++], "laugh as you watch Lunar Capital being destroyed.");
#endif
			}
		}

		break;

		//クエスト40-48はランクエ
		//クエスト49は空き？

		//クエスト50,51は魔理沙専用

		//廃洋館の宝物庫
	case QUEST_VAULT:
		if(CLASS_IS_PRISM_SISTERS)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは館の蔵に古い武具が眠っていることを思い出した。");
				strcpy(quest_text[line++], "今回の異変は手強い冒険になりそうだが、");
				strcpy(quest_text[line++], "あれがあれば助けになるかもしれない。折を見て回収しに行こう。");
#else
                strcpy(quest_text[line++], "You remember there's an old tool lying around in the warehouse");
				strcpy(quest_text[line++], "at your mansion. Looks like your quest is going to be tough, and");
				strcpy(quest_text[line++], "that item might come in handy. You decide to go in and collect it.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "予想以上の難行だったが、ともかく武具の回収に成功した。");
#else
                strcpy(quest_text[line++], "It was unexpectedly tough, but you managed to retrieve your item.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "武具の回収に失敗した。");
				strcpy(quest_text[line++], "異変の影響なのか自分たちの影響なのかわからないが、");
				strcpy(quest_text[line++], "しばらく見ないうちに蔵は魔境になっていた。");
#else
                strcpy(quest_text[line++], "You have failed to retrieve your item.");
				strcpy(quest_text[line++], "You don't know if it's because of the incident or because of you three,");
				strcpy(quest_text[line++], "but the warehouse became a monster lair while you were away.");
#endif
			}

		}
		break;

	case QUEST_KAGUYA: //五つの難題　永琳
		if(pc == CLASS_EIRIN)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "輝夜「ねえ永琳、ダンジョンの奥にはあの難題が本当にあるかもしれないわ。");
				strcpy(quest_text[line++], "一緒に探しに行きましょうよ。」");
#else
                strcpy(quest_text[line++], "Kaguya - 'Hey, Eirin, my impossible requests might actually exist");
				strcpy(quest_text[line++], "deep in the dungeon. Let's go look for them together.'");
#endif
			}
		}

		break;
	case QUEST_SHELOB: //シェロブ退治　鬼、ヤマメ
		if(pr == RACE_ONI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "ヤマメ「その腕っ節、流石は鬼だねえ。");
				strcpy(quest_text[line++], "分かってるよ。お目当てはこれだろう？」");//一行40字まで
#else
                strcpy(quest_text[line++], "Yamame 'What a feat of strength! Just what you'd expect");
				strcpy(quest_text[line++], "from an oni. I know. Are you looking for this?'");//一行40字まで
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "ヤマメ「おやおや鬼ともあろう者がどうしたってのさ。");//一行40字まで
				strcpy(quest_text[line++], "悪い酒でも飲んだのかい？");//一行40字まで
#else
                strcpy(quest_text[line++], "Yamame - 'Well, I didn't expect a display like that from an oni.");//一行40字まで
				strcpy(quest_text[line++], "Did you drink some bad sake or what?'");//一行40字まで
#endif
			}
		}
		else if(pc == CLASS_YAMAME)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは最近近所に住み着いた乱暴者の蜘蛛に我慢がならない。");
				strcpy(quest_text[line++], "シェロブと呼ばれるその蜘蛛は血の気と食い気しか頭にないようだ。");
				strcpy(quest_text[line++], "嫌われ者が集まる地底といえど通すべき筋はある。ここの流儀を教えてやろう。");
#else
                strcpy(quest_text[line++], "You can no longer stand that violent spider who has settled nearby.");
				strcpy(quest_text[line++], "Shelob's her name, and there's no end to her bloodthirst and hunger.");
				strcpy(quest_text[line++], "Even in the underground where hated beings gather, there are lines");
				strcpy(quest_text[line++], "you shouldn't cross. You decide to teach her that.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "余所者は這々の体でどこかへ逃げていった。");
				strcpy(quest_text[line++], "蜘蛛の毒針が地面に落ちているのを見つけた。");
				strcpy(quest_text[line++], "これを加工すれば強力な武器が出来るだろう。");
				strcpy(quest_text[line++], "あなたは早速作業に取り掛かった・・");
#else
                strcpy(quest_text[line++], "The stranger crawled away to somewhere.");
				strcpy(quest_text[line++], "You discover a poisonous spider's fang lying on the ground.");
				strcpy(quest_text[line++], "You probably could make a good weapon out of it, so you");
				strcpy(quest_text[line++], "quickly get to work...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "余所者を相手に不覚を取ってしまった。");
				strcpy(quest_text[line++], "ここは恥を忍んで地底の荒くれ者たちを頼るしかないだろう・・");
#else
                strcpy(quest_text[line++], "The stranger has caught you unaware.");
				strcpy(quest_text[line++], "You have no choice but to plead other violent beings");
				strcpy(quest_text[line++], "underground for help...");
#endif
			}

		}
		break;

		//天狗専用　防風対策任務
	case QUEST_TYPHOON:

		if (pc == CLASS_MEGUMU)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "例年にない暴風が人里に近づいている。");
				strcpy(quest_text[line++], "これは台風と呼ばれる外界の妖怪だ。");
				strcpy(quest_text[line++], "密約に基づきこれを秘密裏に撃退しなければならない。");
#else
				strcpy(quest_text[line++], "An unusually powerful storm is approaching Human Village.");
				strcpy(quest_text[line++], "It is a typhoon youkai from the Outside World.");
				strcpy(quest_text[line++], "You have to secretly drive it away.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "秘密任務は問題なく完了した。");
				strcpy(quest_text[line++], "もっと高級な頭襟を皆に支給してやるだけの予算も確保できた。");
#else
				strcpy(quest_text[line++], "You completed your secret mission without problems.");
				strcpy(quest_text[line++], "Your budget now allows you to give everyone more valuable");
				strcpy(quest_text[line++], "tokins.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "人里に被害を出す失態を犯してしまった。");
				strcpy(quest_text[line++], "他の大天狗にも協力してもらいどうにか事なきを得たが、");
				strcpy(quest_text[line++], "今後百年くらいは陰口を言われそうである。");
#else
				strcpy(quest_text[line++], "Your failure put Human Village into peril.");
				strcpy(quest_text[line++], "You still managed to save the day by working together with");
				strcpy(quest_text[line++], "other Great Tengu, but they'll be whispering behind your back");
				strcpy(quest_text[line++], "for years to come.");
#endif
			}

		}
		break;


		//急流下りクエ　順位を貼り出す特殊処理
	case QUEST_TORRENT:
		{
			if(comp)
			{
				int i;
				int j=0;
				cptr name_list[3];
				bool flag = FALSE;

				//リストに記録されたターン数と競って順位を決める
				for(i=0;i<3;i++)
				{
					//＠と同じ人はリストから飛ばす
					if(torrent_quest_score_list[j].class_id == p_ptr->pclass || torrent_quest_score_list[j].class_id2 == p_ptr->pclass )
						j++;
					//＠がまだランクインしてない場合リストのターンと比較、勝った方の名前をリストに入れる
					if(!flag && (int)qrkdr_rec_turn < torrent_quest_score_list[j].turn)
					{
						name_list[i] = player_name;
						flag = TRUE;//入賞したフラグ
						qrkdr_rec_turn = 1 + i;//記録ターンの変数を「何位だったか」に変える
					}
					else
					{
						name_list[i] = torrent_quest_score_list[j].name;
						j++;
					}
				}

#ifdef JP
				strcpy(quest_text[line++], "大会の結果が張り出されている。");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], format("優勝　：%s",name_list[0]));
				strcpy(quest_text[line++], format("準優勝：%s",name_list[1]));
				strcpy(quest_text[line++], format("第三位：%s",name_list[2]));
#else
                strcpy(quest_text[line++], "The tournament results are posted.");
				strcpy(quest_text[line++], " ");
				strcpy(quest_text[line++], format("1st place  : %s",name_list[0]));
				strcpy(quest_text[line++], format("2nd place  : %s",name_list[1]));
				strcpy(quest_text[line++], format("3rd place  : %s",name_list[2]));
#endif

			}

		}
		break;

		//守矢　ヒダル神退治
	case QUEST_MORIYA_1:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "山の中腹の大蝦蟇の池がヒダル神に襲われている。");
				strcpy(quest_text[line++], "放っておいても大蝦蟇が何とかするだろうが、");
				strcpy(quest_text[line++], "ここは手を貸して恩を売っておこう。");
#else
                strcpy(quest_text[line++], "The Great Toad Pond at the mountain is under attack by");
				strcpy(quest_text[line++], "hidarugami. Great Toad probably will do something about it");
				strcpy(quest_text[line++], "if you leave it be, but there might be a profit in lending a hand.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "大蝦蟇の池に分社を置くことに成功した。");
				strcpy(quest_text[line++], "僅かだが人里から信仰が集まってくるのを感じる。");
#else
                strcpy(quest_text[line++], "You managed to establish a branch shrine at the Great Toad Pond.");
				strcpy(quest_text[line++], "Even if it's only a slight amount, you still feel faith from the");
				strcpy(quest_text[line++], "villagers growing.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "あのような格の低い神霊に不覚を取るとは情けない。");
				strcpy(quest_text[line++], "古来より、腹が減っては戦は出来ないものだ。");
#else
                strcpy(quest_text[line++], "How pitiful of you to get bested by a lowly divine spirit like this.");
				strcpy(quest_text[line++], "An army fights on its stomachs, after all.");
#endif
			}
		}
		if(pc == CLASS_SANAE || pc == CLASS_SUWAKO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "神奈子「山の中腹の大蝦蟇の池がヒダル神に襲われているんですって。");
				strcpy(quest_text[line++], "ここは手を貸してあげて、あわよくばかるーく分社にして来てよ。」");
#else
                strcpy(quest_text[line++], "Kanako - 'The Great Toad Pond at the mountain is under attack");
				strcpy(quest_text[line++], "by hidarugami. If we lend them a hand, we might establish a new");
				strcpy(quest_text[line++], "branch shrine.'");
#endif
			}
		}
		break;

		//採用試験二次募集　隠岐奈打倒クエ
	case QUEST_OKINA:
		//霊夢、魔理沙、文　　チルノ　　成美　　一般天狗　　でセリフ変える必要があるか
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "季節異変の主犯との戦いは唐突に終わってしまった。");
				strcpy(quest_text[line++], "あなたは準備を整えて再戦に挑むことを決意した。");
				strcpy(quest_text[line++], "どうすれば強制排除を防ぐことができるだろう。");
#else
                strcpy(quest_text[line++], "Your battle with the mastermind behind the seasons");
				strcpy(quest_text[line++], "incident has ended abruptly. You took your time preparing");
				strcpy(quest_text[line++], "and resolved to challenge her again. You'll have to");
				strcpy(quest_text[line++], "protect yourself against her forced expulsion somehow.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "またも不覚を取ってしまった。");
				strcpy(quest_text[line++], "絶対秘神の力はあまりにも強大であった。");
#else
                strcpy(quest_text[line++], "You got caught off guard once again.");
				strcpy(quest_text[line++], "That Absolute Secret God is pretty powerful.");
#endif
			}
		}
		else if(pc == CLASS_CIRNO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "椅子に座った偉そうな奴に逃げられてしまった。");
				strcpy(quest_text[line++], "あちこち探していると、見覚えのある地蔵を見つけた。");
				strcpy(quest_text[line++], "こいつに突撃すればまたあの場所に行けるかもしれない。");
#else
                strcpy(quest_text[line++], "That lady in the chair who was so full of herself has made");
				strcpy(quest_text[line++], "you run away. You were looking around for her, and discovered");
				strcpy(quest_text[line++], "a familiar jizo. You might end up in the same place if you go");
				strcpy(quest_text[line++], "into her.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "なぜかぜんぜん敵わなかった。");
				strcpy(quest_text[line++], "やっぱりあれは椅子に座っていたのとは別の奴に違いない。");
#else
                strcpy(quest_text[line++], "For some reason, you couldn't fight back at all.");
				strcpy(quest_text[line++], "That has to be some other chair lady.");
#endif
			}
		}
		else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "チルノ「こいつの背中に扉が開いているな。");
				strcpy(quest_text[line++], "中に入ればまた背中ヤローのところへ行けるぞ。」");
				strcpy(quest_text[line++], "ピース「それならあの秘神にちょっと用があるんだ。");
				strcpy(quest_text[line++], "みんな付き合ってくれない？」");
				strcpy(quest_text[line++], "サニー「よーし、突入よ！」");
#else
                strcpy(quest_text[line++], "Cirno - 'Here's an open door on her back.");
				strcpy(quest_text[line++], "If we go inside, we might end up at that lady-in-back's");
				strcpy(quest_text[line++], "place again.'");
				strcpy(quest_text[line++], "Piece - 'I have some interested in that Secret God.");
				strcpy(quest_text[line++], "Shall we go together?'");
				strcpy(quest_text[line++], "Sunny - 'Very well, in we go!'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "ルナ「し、死ぬかと思ったわ……」");
				strcpy(quest_text[line++], "チルノ「おかしいな？前は何とかなったのに。」");
				strcpy(quest_text[line++], "ピース「ああ、ボスに怒られちまう……」");
#else
                strcpy(quest_text[line++], "Luna - 'I-I thought I'd die...'");
				strcpy(quest_text[line++], "Cirno - 'That's weird. I could deal with her before.'");
				strcpy(quest_text[line++], "Piece - 'My boss is going to get angry...'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "ラルバ「私たちがこんなに強くなってたなんて夢みたいね。」");
				strcpy(quest_text[line++], "スター「ご褒美貰っちゃったわ。皆の宝物にしましょう！」");
				strcpy(quest_text[line++], "ピース「いつでも遊びに来いって言ってくれたし、任務完遂だぜ！」");
#else
                strcpy(quest_text[line++], "Larva - 'We're all so strong! It's like a dream!'");
				strcpy(quest_text[line++], "Star - 'I got a reward! Let's make it our treasure!'");
				strcpy(quest_text[line++], "Piece - 'She told me I can go out and play as I like,");
				strcpy(quest_text[line++], " mission complete!'");
#endif
			}
		}
		else if(pr == RACE_HAKUROU_TENGU || pr == RACE_KARASU_TENGU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "里乃「ちょっとー、舞ったらまた天狗を引き入れる気？」");
				strcpy(quest_text[line++], "舞「ふっ、予想外は人生の最高のスパイスさ！」");
#else
                strcpy(quest_text[line++], "Satono - 'Hold on, Mai, did you get a tengu again?'");
				strcpy(quest_text[line++], "Mai - 'You know, the unexpected is the spice of life!'");
#endif
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "里乃「ちょっとー、舞ったらまた妖精を引き入れる気？」");
				strcpy(quest_text[line++], "舞「ふっ、予想外は人生の最高のスパイスさ！」");
#else
                strcpy(quest_text[line++], "Satono - 'Hold on, Mai, did you get a fairy again?'");
				strcpy(quest_text[line++], "Mai - 'You know, the unexpected is the spice of life!'");
#endif
			}
		}
		else if(pc == CLASS_NARUMI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "久しぶりに森に帰ったら、見覚えのない怪しげな二人組がいる。");
				strcpy(quest_text[line++], "二人はあなたをスカウトしたいと言い出した。");
				strcpy(quest_text[line++], "あなたはこの二人の素性に心当たりがあった・・・");
#else
                strcpy(quest_text[line++], "You returned to the forest after being away for a while,");
				strcpy(quest_text[line++], "and ran into two suspicious people. They told you they're");
				strcpy(quest_text[line++], "trying to recruit you. You think you remember those two...");
#endif
			}
		}
		break;
		//v1.1.36 小鈴失踪　最後の宴会に出た面子の失敗セリフ変更　あと一人だれか分からん人がいるが
	case QUEST_KOSUZU:
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA || pc == CLASS_MAMIZOU
			|| pc == CLASS_SANAE || pc == CLASS_YUYUKO || pc == CLASS_YOUMU || pc == CLASS_REMY
			 || pc == CLASS_SAKUYA	)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「先日の宴会ではお世話になりました。");
				strcpy(quest_text[line++], "ひとまず丸く収まって何よりというところでしょうか。");
				strcpy(quest_text[line++], "それにしても小鈴の先が思いやられるわ…」");
#else
                strcpy(quest_text[line++], "Akyuu - 'Thanks for your help at the banquet the other day.");
				strcpy(quest_text[line++], "I'm worried about Kosuzu's future...'");
#endif
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「ねえ、貸本屋の小鈴をどこかで見なかった？");
				strcpy(quest_text[line++], "　いま行方不明になってて皆で探してるの。");
				strcpy(quest_text[line++], "　できればお友達にも聞いてみてほしいわ。」");
#else
				strcpy(quest_text[line++], "Akyuu - 'Hey, have you seen Kosuzu from the bookstore?");
				strcpy(quest_text[line++], "  She's gone missing, and we're searching for her.");
				strcpy(quest_text[line++], "  Please ask your friends about her if you can.'");
#endif
			}
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「そう、そんな事になっていたなんて...");
				strcpy(quest_text[line++], "　妖精のあなたが連れ戻してくれたのは僥倖というものね。");
				strcpy(quest_text[line++], "　そのリュックはあげるからできればあまり噂にしないであげて。」");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "　　　　　　　　(妖精の危険度を上方修正したほうがいいかしら...)");
#else
				strcpy(quest_text[line++], "Akyuu - 'Oh, so that's what happened...");
				strcpy(quest_text[line++], "  But luckily, you managed to bring her back.");
				strcpy(quest_text[line++], "  I'll give you this backpack, please don't spread any rumors.");
				strcpy(quest_text[line++], "  ");
				strcpy(quest_text[line++], "  ");
				strcpy(quest_text[line++], "      (I probably should raise the danger level of fairies...)");
#endif

			}
			if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "阿求「神社で小鈴に襲われたですって？");
				strcpy(quest_text[line++], "　ありがとう、すぐに保護させるわ。");
				strcpy(quest_text[line++], "　小鈴が迷惑かけてごめんなさいね。");
				strcpy(quest_text[line++], "　でもこのことはできれば秘密にしてあげて？」");
#else
				strcpy(quest_text[line++], "Akyuu - 'You got attacked by Kosuzu at the shrine?");
				strcpy(quest_text[line++], "  Thank you, we'll secure her right away.");
				strcpy(quest_text[line++], "  Sorry for the trouble she caused you.");
				strcpy(quest_text[line++], "  Could you please keep this a secret?'");
#endif

			}

		}



		break;

		//夢の世界の＠打倒クエ　v1.1.52 菫子特殊性格セリフ変更
	case QUEST_DREAMDWELLER:
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "隠岐奈「ついにお前にとって最大の悪夢との対決の時が来た。");
				strcpy(quest_text[line++], "見事打ち勝ってみせよ！私を失望させてくれるなよ！」");
#else
                strcpy(quest_text[line++], "Okina - 'It's time for you to confront your worst nightmare.");
				strcpy(quest_text[line++], "Show me a splendid victory! Don't let me down!'");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "隠岐奈「天晴れである！よくぞ自分自身に打ち勝った！");
				strcpy(quest_text[line++], "さて、このまま目覚めるとお前と幻想郷との縁は切れてしまうだろう。");
				strcpy(quest_text[line++], "そこで私が後戸の力を使ってお前と幻想郷の繋がりを維持してやろう。");
				strcpy(quest_text[line++], "これでお前は名実共に本物だ！");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "ところで、良かったら私の所で働くつもりはない？」");
#else
                strcpy(quest_text[line++], "Okina - 'Well done! You've overcome yourself!");
				strcpy(quest_text[line++], "If you wake up like this, you'll lose your bonds with Gensokyou.");
				strcpy(quest_text[line++], "I'll use the power of Back Door to sustain those bonds.");
				strcpy(quest_text[line++], "Now you are truly real in both worlds!");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "By the way, would you like to work for me?'");
#endif

			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "ドレミー「あなたはもう悪夢から逃れられません。");
				strcpy(quest_text[line++], "可哀想に……強く生きて……」");
#else
                strcpy(quest_text[line++], "Doremy - 'You can no longer escape from your nightmares.");
				strcpy(quest_text[line++], "Pitiful... Stay strong...'");
#endif
			}

		}
		break;

		//鰻の素焼き納入クエ
	case QUEST_DELIVER_EEL:
		break;


		//水際作戦　死神と閻魔のときちょっとセリフ変える 久侘歌のときも変える
	case QUEST_SHORELINE2:

		if (pc == CLASS_KUTAKA)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "久しぶりに職場に戻って一息ついていると、上司から緊急の呼び出しを受けた。");
				strcpy(quest_text[line++], "用件は水際作戦の再発令であった。");
				strcpy(quest_text[line++], "件の動物霊が今度は地獄の別のエリアへの侵攻を企てているらしい。");
				strcpy(quest_text[line++], "地獄の第66階層へ向かおう。」");
#else
                strcpy(quest_text[line++], "You came back to your office to take a break, but you received");
				strcpy(quest_text[line++], "an urgent call from your higher ups. It's about repeating the coastal");
				strcpy(quest_text[line++], "operation. That animal ghost is planning to invade another area of Hell.");
				strcpy(quest_text[line++], "She should be on level 66 in Hell.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "凶悪な強さを誇る『驪駒 早鬼』を打倒した。");
				strcpy(quest_text[line++], "この功績により臨時ボーナスが出るが、");
				strcpy(quest_text[line++], "残念ながら予算の都合で現物支給となった。");
#else
                strcpy(quest_text[line++], "You've taken down the powerful Saki Kurokuma.");
				strcpy(quest_text[line++], "You received a bonus for your achievement, but, unfortunately,");
				strcpy(quest_text[line++], "it was limited due to budget restraints.");
#endif

			}
		}
		else if (pr == RACE_DEATH || pc == CLASS_EIKI)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "久侘歌「件の動物霊が今度は地獄の別のエリアへの侵攻を企てています。");
				strcpy(quest_text[line++], "事態を重く見た上層部は水際作戦の再発令に踏み切りました。");
				strcpy(quest_text[line++], "手の空いている方は封じ込めあるいは討伐に向かうようにとのことです。」");
#else
                strcpy(quest_text[line++], "Kutaka - 'That animal spirit is planning to invade another area");
				strcpy(quest_text[line++], "of Hell. Upper management is taking this seriously and have ordered");
				strcpy(quest_text[line++], "repeating the coastal operation to cut her off. If you're free, you");
				strcpy(quest_text[line++], "should go and detain or subdue her.'");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "久侘歌「彼女もようやく懲りたようで、");
				strcpy(quest_text[line++], "当分は畜生界の支配に注力するつもりのようです。");
				strcpy(quest_text[line++], "まあ畜生界内部のことならこちらの関知することではありませんよね。");
				strcpy(quest_text[line++], "貴方への臨時ボーナスが出ていますのでこちらに受領印をお願いします。」");
#else
                strcpy(quest_text[line++], "Kutaka - 'She has been disciplined, and for now it looks ");
				strcpy(quest_text[line++], "like she's focused on taking over Animal Realm.");
				strcpy(quest_text[line++], "That being said, we're not interested in internal affairs ");
				strcpy(quest_text[line++], "of that realm. There's a bonus payment waiting for you, so place");
				strcpy(quest_text[line++], "your stamp here.'");
#endif

			}
		}
		break;

		//破滅クエ3 袿姫依頼でサイバーデーモン退治
	case QUEST_DOOM3:
		if (pr == RACE_HANIWA) //v1.1.66
		{
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "袿姫「外の世界ではあんな凶悪な武器が使われているのね。");
				strcpy(quest_text[line++], "　それをも打ち倒してくるとは、あなたの忠誠を嬉しく思うわ。");
				strcpy(quest_text[line++], "　さて、何か褒美を取らさないとね。");
				strcpy(quest_text[line++], "　私の新作だったんだけど貴方にあげるわ。」");
#else
                strcpy(quest_text[line++], "Keiki - 'So, that's the kind of foul weapons they're using");
				strcpy(quest_text[line++], " in the Outside World. But you still managed to take it down; I'm");
				strcpy(quest_text[line++], " so glad you're loyal to me. Well, I have to reward you with");
				strcpy(quest_text[line++], " something. Here's my newest work, please take it.'");
#endif

			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "袿姫「外の世界ではあんな武器が使われているのか。");
				strcpy(quest_text[line++], "　強力だけど美しさに欠けているわね。");
				strcpy(quest_text[line++], "　急いで防爆構造の開発を進めましょう。");
				strcpy(quest_text[line++], "　貴方は陣形の見直しをしなさい。");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "　それにしてもあの畜生ども、");
				strcpy(quest_text[line++], "　あんなものを呼び込んで手綱を握れるつもりでいるのかしら？」");
#else
                strcpy(quest_text[line++], "Keiki - 'That's the kind of weapons they use in the");
				strcpy(quest_text[line++], " Outside World? It's powerful, but it's lacking in beauty.");
				strcpy(quest_text[line++], " I'll have to work fast and develop an explosion-proof");
				strcpy(quest_text[line++], " Also, I'd like you to revise your formations.");
				strcpy(quest_text[line++], " ");
				strcpy(quest_text[line++], " Even so, did those animal ghosts really call in");
				strcpy(quest_text[line++], " something like this to take over us?'");
#endif

			}
		}

		break;

		//v1.1.65 羽衣婚活 特定種族専用　ユニーククラスのときだけ台詞を変えておく
	case QUEST_KONNKATSU:
		if (cp_ptr->flag_only_unique)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "天子「ちょうどいいところに来たな。ちょっと頼まれて頂戴。");
				strcpy(quest_text[line++], "実は婚活に失敗して羽衣を無くした間抜けな天女が居てね。 ");
				strcpy(quest_text[line++], "そいつに泣き付かれて放っておけなくなった衣玖が私に頼んできたんだ。 ");
				strcpy(quest_text[line++], "でも布切れ一枚のために地上や地底を探し回るなんて私はまっぴらだから、 ");
				strcpy(quest_text[line++], "見込みの有りそうな奴を探してたってわけ。 ");
				strcpy(quest_text[line++], "そういうわけで羽衣を一枚探してきなさい。 ");
				strcpy(quest_text[line++], "当然そんじょそこらの羽衣じゃ駄目よ。 ");
				strcpy(quest_text[line++], "天界にふさわしい【特別製】の羽衣じゃないとね。」 ");
#else
                strcpy(quest_text[line++], "Tenshi - 'You came at a good time. I have a small request for you.");
				strcpy(quest_text[line++], "There's a celestial maiden who failed to marry and lost her hagoromo.");
				strcpy(quest_text[line++], "Iku couldn't stand to watch her crying, so she asked me for help.");
				strcpy(quest_text[line++], "So I'm looking for that piece of cloth both above and under ground,");
				strcpy(quest_text[line++], "and I'd like you to find me one. ");
				strcpy(quest_text[line++], "Naturally, any ordinary hagoromo won't do.");
				strcpy(quest_text[line++], "You'll have to bring in a 'special' hagoromo fitting for Heaven.'");
#endif
			}
		}
		break;
	case QUEST_MIYOI: //v1.1.78 ドリームイーターの酔災　ドレミー
		if (pc == CLASS_DOREMY)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "もう酔っ払い達の酒臭い夢は食べ飽きました。");
				strcpy(quest_text[line++], "かくなる上は神社の宴会に参加して悪戯者を懲らしめるしかありません。");
				strcpy(quest_text[line++], "鬼の酒のもたらす幻影は凶悪極まりないですが、");
				strcpy(quest_text[line++], "何か酔い醒ましを飲めば消えるかもしれませんね。");
#else
                strcpy(quest_text[line++], "You've grown tired of eating drunken dreams of all those people");
				strcpy(quest_text[line++], "getting drunk. You have no choice but to join the banquet");
				strcpy(quest_text[line++], "at the shrine and discipline whoever is causing it. The visions");
				strcpy(quest_text[line++], "that devilish drink brings look very real, but they might disappear");
				strcpy(quest_text[line++], "if you somehow come to your senses.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "酔魔と思しき力の使い手はどこかに消えてしまいました。");
				strcpy(quest_text[line++], "どうも鬼の瓢箪の中に逃げ込んだようですが、");
				strcpy(quest_text[line++], "とりあえずこれを預かっておけば悪さはできないでしょう。");
#else
                strcpy(quest_text[line++], "Looks like whoever was inflicting drunkedness has fled away");
				strcpy(quest_text[line++], "somewhere. It's probably inside that oni gourd, but you decide");
				strcpy(quest_text[line++], "it can't do anything if you take it with you.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "悪夢の怪物に遅れを取るとは夢の支配者の名折れです。");
				strcpy(quest_text[line++], "このままでは酔夢で肝臓をやられてしまいます……");
#else
                strcpy(quest_text[line++], "Losing to that monster from nightmares is going to hurt");
				strcpy(quest_text[line++], "your reputation as the ruler of dreams. Your liver is going to");
				strcpy(quest_text[line++], "get damaged from those drunken dreams at this rate...");
#endif

			}

		}
		break;
	case QUEST_HANGOKU1://v1.1.84 連続昏睡事件Ⅰ　藍・橙
		if (pc == CLASS_CHEN || pc == CLASS_RAN)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "『反獄王』と呼ばれる永久罪人の大怨霊の行方は杳として知れない。");
				strcpy(quest_text[line++], "もしかすると再び魔理沙を操って他の誰かに取り憑く機会を伺っているのかも。");
				strcpy(quest_text[line++], "ダンジョンで魔理沙を見かけたら伸してしまおう。");
				strcpy(quest_text[line++], "もし怪しい酒を持っていたら大当たりだ。");
#else
                strcpy(quest_text[line++], "Hangoku-ou, a vengeful spirit of an eternal sinner, has gone missing.");
				strcpy(quest_text[line++], "It might be looking for another chance to possess Marisa or someone");
				strcpy(quest_text[line++], "else. Be prepared if you run into Marisa in the dungeon. If she's");
				strcpy(quest_text[line++], "holding some suspicious sake, that's your culprit.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "駄目で元々だったが、怪しい魔理沙を見つけて倒し例の酒を奪った。");
				strcpy(quest_text[line++], "地底からの使いに引き渡して返礼を受け取った。");
				strcpy(quest_text[line++], "これで片がつくといいのだが。");
#else
                strcpy(quest_text[line++], "It might have seemed futile at first, but you found Marisa");
				strcpy(quest_text[line++], "acting suspicious, defeated her, and obtained that sake bottle.");
				strcpy(quest_text[line++], "You gave it to a messenger from underground and received a reward.");
				strcpy(quest_text[line++], "You hope this will get rid of it.");
#endif
			}
			else if (fail)
			{

#ifdef JP
				strcpy(quest_text[line++], "あなたはダンジョンでの捜索を諦めた。");
				strcpy(quest_text[line++], "やはりあの狡猾な大怨霊がそんなに簡単に見つかるはずはない。");
#else
                strcpy(quest_text[line++], "You gave up on exploring the dungeon.");
				strcpy(quest_text[line++], "You knew you won't find such a cunning vengeful spirit");
				strcpy(quest_text[line++], "that easily.");
#endif

				//魔理沙を倒してドロップアイテムの★が生成されている場合追加メッセージ
				if (a_info[ART_HANGOKU_SAKE].cur_num)
				{
#ifdef JP
					strcpy(quest_text[line++], "　");
					strcpy(quest_text[line++], "それにしてもさっきから頭に靄がかかったような気がする。");
					strcpy(quest_text[line++], "式の調子が悪いのだろうか？");
#else
                    strcpy(quest_text[line++], " ");
					strcpy(quest_text[line++], "By the way, your head feels a bit hazy.");
					strcpy(quest_text[line++], "Is your shikigami power faltering?");
#endif

				}

			}

		}
		break;

	case QUEST_GEIDON_HUMAN:

		break;

	case QUEST_GEIDON_KWAI:
		if (pc == CLASS_SUIKA || pc == CLASS_MAMIZOU)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "里に放った子狸たちが不穏な知らせをよこしてきた。");
				strcpy(quest_text[line++], "外界から来たと思われる凶悪な妖物が里に侵入したらしい。");
				strcpy(quest_text[line++], "被害者が出る前に速やかに始末しないといけない。");
				strcpy(quest_text[line++], "この里は我々の縄張りなのだから。");
#else
                strcpy(quest_text[line++], "You've received some disturbing news from the tanuki in");
				strcpy(quest_text[line++], "the village. Looks like evil monsters from outer worlds");
				strcpy(quest_text[line++], "are invading the village. You have to deal with them");
				strcpy(quest_text[line++], "before they cause any harm. The village is your territory,");
				strcpy(quest_text[line++], "after all.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "被害が出る前に件の連中を退治することに成功した。");
				strcpy(quest_text[line++], "目撃者が出てしまったようだが、子狸たちを使って誤魔化しておこう。");
#else
                strcpy(quest_text[line++], "You've managed to clean them up before something bad happened.");
				strcpy(quest_text[line++], "There were some eyewitnesses, but you send baby raccoons to");
				strcpy(quest_text[line++], "mislead and beguile them.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "情けないことに外敵に遅れを取ってしまった。");
				strcpy(quest_text[line++], "たまたま通りがかった霊夢が吹き飛ばしてくれたが、");
				strcpy(quest_text[line++], "当分は頭が上がりそうにない...");
#else
                strcpy(quest_text[line++], "Unfortunately, those outer world enemies were too much for you.");
				strcpy(quest_text[line++], "Fortunately, Reimu was passing through and blew them away,");
				strcpy(quest_text[line++], "but looks like you'll have to stay low for the time being...");
#endif

			}

		}

		break;

	case QUEST_MORIYA_2:
		if (pc == CLASS_SANAE)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "市場の神と守矢神社共催のアビリティカード交換会が開催された。");
				strcpy(quest_text[line++], "全く気が進まないが、自分も神社の風祝として参加しないわけにはいかない。");
#else
                strcpy(quest_text[line++], "The Marketplace God and Moriya Shrine have cooperated to hold an");
				strcpy(quest_text[line++], "ability card exchange event. You're not that interested, but as the");
				strcpy(quest_text[line++], "wind celebrant of the shrine, you have to participate.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "千亦「どうにも参加者の集まりが悪いわね。こんなに非日常の空間なのに。」");
				strcpy(quest_text[line++], "神奈子「ふうむ、丸太落としは非日常というには少し人為的すぎたのかも。");
				strcpy(quest_text[line++], "　次は間欠泉地下センターの溶岩エリアで開催するというのはどうかしら？」");
				strcpy(quest_text[line++], "千亦「それよ！やっぱりあなたは市場を分かっているわね！」」");
#else
                strcpy(quest_text[line++], "Chimata - 'There's so few people here, even though we've set up ");
                strcpy(quest_text[line++], " such an extraordinary space.'");
				strcpy(quest_text[line++], "Kanako - 'Hmm, while dropping logs down is certainly extraordinary,");
				strcpy(quest_text[line++], " it also feels a bit too artifical. How about next time we'll hold");
				strcpy(quest_text[line++], " it at the lava area in the underground geyser center?'");
				strcpy(quest_text[line++], "Chimata - 'That's it! You certainly do understand how the");
				strcpy(quest_text[line++], " marketplace works!'");
#endif
			}
		}
		else if (pc == CLASS_KANAKO || pc == CLASS_SUWAKO)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "市場の神と守矢神社共催のアビリティカード交換会が開催された。");
				strcpy(quest_text[line++], "やはり神社の祭神として自分も顔を出すべきだろう。");
#else
                strcpy(quest_text[line++], "The Marketplace God and Moriya Shrine have cooperated to hold an");
				strcpy(quest_text[line++], "ability card exchange event. As the festival god of the shrine,");
				strcpy(quest_text[line++], "you have to show up.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "古代の祭りを思い出して血が騒ぐ、なかなか楽しい催しであった。");
				strcpy(quest_text[line++], "しかしなぜかあまり信仰が増えた感じはしない。");
#else
                strcpy(quest_text[line++], "That was a very enjoyable event; it reminded you of ancient");
				strcpy(quest_text[line++], "festivals and made your blood boil. For some reason, though, you");
				strcpy(quest_text[line++], "don't feel faith in you growing.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "あなたは交換所に辿り着けなかった。");
				strcpy(quest_text[line++], "これでは主催者として立場がない。信仰も全然増えなかった...");
#else
                strcpy(quest_text[line++], "You failed to arrive at the event venue.");
				strcpy(quest_text[line++], "That certainly doesn't suit you as the event organizer. And, of");
				strcpy(quest_text[line++], "course, you haven't gained any faith...");
#endif
			}
		}

		break;

	case QUEST_REIMU_ATTACK:

		if(pc == CLASS_KASEN)
		{
			if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "霊夢は山童のアジトを破壊して去っていった。");
				strcpy(quest_text[line++], "元々の原因が自分にある以上、山童たちを放っておくわけにもいかない。");
				strcpy(quest_text[line++], "支援を申し出たらいたく感謝されたが、");
				strcpy(quest_text[line++], "彼女たちの目を真っ直ぐ見ることができなかった...");
#else
                strcpy(quest_text[line++], "Reimu destroyed the yamawaro hideout and left.");
				strcpy(quest_text[line++], "As you were the original cause behind this, you couldn't leave");
				strcpy(quest_text[line++], "them be. You offered them your support and received gratitude,");
				strcpy(quest_text[line++], "but now you can't bring yourself to look them straight into");
				strcpy(quest_text[line++], "the eyes...");
#endif
			}
		}
		else if (pc == CLASS_SANNYO)
		{
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "たかね「いやあ助かったよ。アジトを台無しにされるところだった。");
				strcpy(quest_text[line++], "じゃあ例の通貨を使って報酬を渡そう。");
				strcpy(quest_text[line++], "これからも持ちつ持たれつで頼むよ。");
#else
                strcpy(quest_text[line++], "Takane - 'Thanks for the help! Our hideout was about to get");
				strcpy(quest_text[line++], "destroyed. As a reward, let's conduct the usual exchange.");
				strcpy(quest_text[line++], "I'm looking forward to further working with you in the future.'");
#endif
			}

		}
		else if (pc == CLASS_TAKANE)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "博麗の巫女が、");
				strcpy(quest_text[line++], "「あの嘘吐きどもを全滅させないと気が済まないわ！」と");
				strcpy(quest_text[line++], "怒り狂って乗り込んできた。");
				strcpy(quest_text[line++], "何がなんだか分からないが、まずは迎撃しないといけない！");
#else
                strcpy(quest_text[line++], "The Hakurei shrine maiden furiously barged in, screaming");
				strcpy(quest_text[line++], "'I won't rest until I exterminate all of you damn liars!'");
				strcpy(quest_text[line++], "You don't know what happened, but first you have to");
				strcpy(quest_text[line++], "repel the attack!");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "巫女はあれこれ文句を言いながら帰っていった。");
				strcpy(quest_text[line++], "こちらが妖怪だからって、全く迷惑な話である。");
#else
                strcpy(quest_text[line++], "The shrine maiden left, muttering some complaints.");
				strcpy(quest_text[line++], "She kept telling some nonsense like 'But you're youkai!'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "巫女はアジトを半壊させた末にようやく帰っていった。");
				strcpy(quest_text[line++], "我々が一体何をしたというのか？");
#else
                strcpy(quest_text[line++], "The shrine maiden has destroyed the half of your");
				strcpy(quest_text[line++], "hideout and finally went back. Just what is going to happen");
				strcpy(quest_text[line++], "to you?");
#endif
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "たかね「なんだ妖精か。いや今は妖精の手も借りたい。");
				strcpy(quest_text[line++], "　今からみんなで博麗霊夢と弾幕ごっこをやるんだ！");
				strcpy(quest_text[line++], "　パワーアップアイテムは持ったか？よし突っ込め！」");
#else
                strcpy(quest_text[line++], "Takane - 'Huh, a fairy? I'd like you to lend us a hand.");
				strcpy(quest_text[line++], "  We're going to play danmaku with Reimu Hakurei!");
				strcpy(quest_text[line++], "  Do you have power up items? Alright, go ahead!'");
#endif
			}
		}

		break;


	case QUEST_YAKUZA_1:

		//このクエストは所属勢力がややこしいので受領時・成功時・失敗時でまず分ける
		if (accept)
		{
			//動物霊憑依時
			if (p_ptr->muta4 & (MUT4_GHOST_WOLF | MUT4_GHOST_EAGLE | MUT4_GHOST_OTTER | MUT4_GHOST_HANIWA))
			{
#ifdef JP
				strcpy(quest_text[line++], "畜生界の動物霊が旧血の池地獄で抗争を始めるそうだ。");
				strcpy(quest_text[line++], "しかしどうしたことだろう？");
				strcpy(quest_text[line++], "なぜかあの勢力に助太刀しなければいけない気がして仕方がない！");
#else
                strcpy(quest_text[line++], "The animal ghosts from Animal Realm are going to start a war at Former");
                strcpy(quest_text[line++], "Lake of Blood Hell. But wait, what's going on? For some reason, you feel");
				strcpy(quest_text[line++], "strangely compelled to help their forces!");
#endif
			}
			//早鬼
			else if (pc == CLASS_SAKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "最近見かけない饕餮尤魔の行方に関する情報提供者が現れた。");
				strcpy(quest_text[line++], "尤魔は旧血の池地獄に引き篭もってせっせと力を蓄えているらしい。");
				strcpy(quest_text[line++], "さっそく蹴り倒しに行こう。");
#else
                strcpy(quest_text[line++], "An informant has arrived, bringing news about Yuuma Toutetsu's location.");
				strcpy(quest_text[line++], "She's hiding at Former Lake of Blood Hell, building up her strength.");
				strcpy(quest_text[line++], "Time to kick her down.");
#endif
			}
			//八千慧
			else if (pc == CLASS_YACHIE)
			{
#ifdef JP
				strcpy(quest_text[line++], "最近見かけない饕餮尤魔の行方に関する情報提供者が現れた。");
				strcpy(quest_text[line++], "饕餮は旧血の池地獄に引き篭もって石油を貪っているらしい。");
				strcpy(quest_text[line++], "饕餮に力を蓄えられるのは好ましくないし資源を我々が手にすれば有利になる。");
				strcpy(quest_text[line++], "放置する手はないだろう。");
				strcpy(quest_text[line++], "しかし恐らくこの者は他の組織にも同様に情報を提供している。");
				strcpy(quest_text[line++], "最悪四つ巴の抗争になるだろう。入念に準備をして臨まねばならない。");
				strcpy(quest_text[line++], "とくに石油はよく燃えるので火炎対策が重要だ。");
				strcpy(quest_text[line++], "傭兵として雇った強力な火竜を連れて行こう。");
#else
                strcpy(quest_text[line++], "An informant has arrived, bringing news about Yuuma Toutetsu's location.");
				strcpy(quest_text[line++], "She's hiding at Former Lake of Blood Hell, consuming oil.");
				strcpy(quest_text[line++], "You don't like the idea of Yuuma gathering strength, and obtaining the");
				strcpy(quest_text[line++], "resources would be profitable for you. You can't leave this issue alone.");
				strcpy(quest_text[line++], "However, this information might also have been provided to other families;");
				strcpy(quest_text[line++], "in the worst case, it'll be a four-way conflict. You'll have to carefully");
				strcpy(quest_text[line++], "prepare before going in. Since oil burns well, you'll have to protect");
				strcpy(quest_text[line++], "yourself against fire. You've hired a powerful fire wyrm as your mercenary.");
#endif
			}
			//尤魔
			else if (pc == CLASS_YUMA)
			{
#ifdef JP
				strcpy(quest_text[line++], "ようやく落ち着いた血の池地獄に畜生界の連中がちょっかいをかけてきた。");
				strcpy(quest_text[line++], "強力な火竜を引き連れた勢力までいるようだ。");
				strcpy(quest_text[line++], "しかし何もせずみすみす石油を明け渡すのはあまりにも惜しい。");
				strcpy(quest_text[line++], "不本意だが防衛戦をせねばならない。");
#else
				strcpy(quest_text[line++], "Just as the Lake of Blood Hell has calmed down, forces from Animal Realm");
				strcpy(quest_text[line++], "started interfering. They even have brought a powerful fire wyrm.");
				strcpy(quest_text[line++], "However, you're not going to just hand over your oil. You don't like it,");
				strcpy(quest_text[line++], "but you'll have to fight a defensive war.");
#endif
			}
			//埴輪
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "畜生界の動物霊が旧血の池地獄で抗争を始めるとの情報が届いた。");
				strcpy(quest_text[line++], "すぐに行って動物霊を掃討しなければならない。");
				strcpy(quest_text[line++], "無敵の埴輪兵団の力をもう一度見せつけてやろう。");
#else
				strcpy(quest_text[line++], "You've received information that animal ghosts from Animal Realm are");
				strcpy(quest_text[line++], "going to start a war at Former Lake of Blood Hell. You must quickly go");
				strcpy(quest_text[line++], "there and deal with the ghosts. You're going to show them the power of");
				strcpy(quest_text[line++], "your inexhaustible Haniwa Corps once again.");
#endif
			}
			//ほか一般動物霊　所属勢力で分岐
			//v2.0.11 新入り3人も同じメッセージを表示するため分岐を変えた
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
#ifdef JP
                strcpy(quest_text[line++], "早鬼「丁度いいところに来たな！今から抗争に行くぞ！");
                strcpy(quest_text[line++], "行き先は旧血の池地獄だ。尤魔の奴が引き篭もって何かしているらしい！");
                strcpy(quest_text[line++], "いつもすぐ逃げ出す卑怯者を蹴倒してやるチャンスだ！みんな私に続け！」");
#else
                strcpy(quest_text[line++], "Saki - 'You came at a good time! We're going to war!");
                strcpy(quest_text[line++], "We're heading to Former Lake of Blood Hell. Yuuma's hiding there,");
                strcpy(quest_text[line++], "planning to do something! It's our chance to kick some cowards who");
                strcpy(quest_text[line++], "keep on running away! Everyone, follow me!'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
			{
#ifdef JP
                strcpy(quest_text[line++], "八千慧「緊急の任務よ！");
                strcpy(quest_text[line++], "旧血の池地獄で石油という有用な資源が大量に発見された。");
                strcpy(quest_text[line++], "現在は剛欲同盟に支配されていて、放置するわけにはいかない。");
                strcpy(quest_text[line++], "この地を奪取すべく急いで兵力を集めているけど、");
                strcpy(quest_text[line++], "勁牙組や霊長園も我々と同じことを考えているみたい。");
                strcpy(quest_text[line++], "おそらく一大抗争になるからあなたもこれに加わって。");
                strcpy(quest_text[line++], "あと石油はよく燃えるから石油の上では炎攻撃の威力が増すわ。");
				strcpy(quest_text[line++], "強力な火竜を雇って連れて行くから巻き込まれないように立ち回ってね。」");
#else
                strcpy(quest_text[line++], "Yachie - 'There's an urgent mission!");
                strcpy(quest_text[line++], "We've discovered a large amount of useful resource called 'oil' in");
                strcpy(quest_text[line++], "Former Lake of Blood Hell. It's under control of Gouyoku Alliance");
                strcpy(quest_text[line++], "right now; we can't leave it like that. We're moving our forces");
                strcpy(quest_text[line++], "to that location, and Keiga Family and Primate Garden are doing the");
                strcpy(quest_text[line++], "same. It's likely to end up in a major conflict, so I'd like you to");
                strcpy(quest_text[line++], "take part in it. Oil burns well, so fire deals more damage on oil");
                strcpy(quest_text[line++], "terrain. We've brought a powerful fire wyrm, so avoid getting in");
                strcpy(quest_text[line++], "the way of its attacks.'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
			{
#ifdef JP
                strcpy(quest_text[line++], "地面から聞こえる尤魔の声「おいお前。すぐに地下の旧血の池地獄まで来い。");
                strcpy(quest_text[line++], "我々の石油を狙って他勢力の連中が攻めてきた。");
                strcpy(quest_text[line++], "どう転ぶにしろ少しは敵に出血を強いなければ利権に食い込むこともできん。");
                strcpy(quest_text[line++], "お前も加わってなるべく敵の数を減らせ。");
                strcpy(quest_text[line++], "とくに敵の援軍の火竜が厄介だ。火炎対策をしておけ。");
                strcpy(quest_text[line++], "それにしても防衛戦など全く割に合わん。ああ腹が減る...」");
#else
                strcpy(quest_text[line++], "Yuuma voice from the ground - 'Hey, you. Come quickly to the Former");
                strcpy(quest_text[line++], "Lake of Blood Hell underground. Other forces are attacking, trying");
                strcpy(quest_text[line++], "to obtain out oil. No matter how you look at it, we'll have to make");
                strcpy(quest_text[line++], "them bleed to maintain profits. Join in and reduce enemy forces as");
                strcpy(quest_text[line++], "much as you can. Their fire wyrm in particular is a nasty one.");
                strcpy(quest_text[line++], "Protect yourself against fire attacks. Still, I really don't like");
                strcpy(quest_text[line++], "being on the defense. I'm so hungry...'");
#endif
			}
			else if (pr == RACE_ANIMAL_GHOST)
			{
#ifdef JP
                strcpy(quest_text[line++], "畜生界の動物霊がこの下の旧血の池地獄で抗争を始めるようだ。");
                strcpy(quest_text[line++], "自分には関係のない話だが、どこかに味方して恩を売ってもいい。");
                strcpy(quest_text[line++], "あるいは全員倒してしまえば実に痛快だろう。");
#else
                strcpy(quest_text[line++], "The animal ghosts from Animal Realm are going to start a war at Former");
                strcpy(quest_text[line++], "Lake of Blood Hell. It's none of your business, but you might join");
                strcpy(quest_text[line++], "someone to win their favor. Or you could just beat them all up.");
#endif
			}

			//ほか一般埴輪
			else if (pr == RACE_HANIWA)
			{
#ifdef JP
				strcpy(quest_text[line++], "磨弓「動物霊どもが旧血の池地獄で抗争を始めるとの情報が入ったわ！");
				strcpy(quest_text[line++], "人間霊を虐げるのに飽き足らず畜生界の外にまで戦禍を広げようとは許しがたい！");
				strcpy(quest_text[line++], "私達もすぐに向かい、奴等を掃討しましょう！」");
#else
				strcpy(quest_text[line++], "Mayumi - 'We've received information that animal ghosts are going to start");
				strcpy(quest_text[line++], "a war at Former Lake of Blood Hell! Oppressing human spirits was bad enough,");
				strcpy(quest_text[line++], "and now they're waging war outside their realm! We're heading there right");
				strcpy(quest_text[line++], "now to clean them up!'");
#endif
			}
			//それ以外
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "空「なんで動物霊とか竜とか埴輪の群れが");
				strcpy(quest_text[line++], "喧嘩しながら灼熱地獄をぞろぞろ通り抜けて行くの？");
				strcpy(quest_text[line++], "もう撃ち落とすのに疲れたよー！」");
				strcpy(quest_text[line++], "　");
				strcpy(quest_text[line++], "燐「旧血の池地獄で畜生界の連中が抗争をおっ始めるみたいだねえ。");
				strcpy(quest_text[line++], "まあ決着がつくまで放っておきなよ。");
				strcpy(quest_text[line++], "でも腕に自信があるならひと暴れして何処かに恩を売れるかもしれないね。");
				if (pc == CLASS_SATORI)
					strcpy(quest_text[line++], "さとり様も見物に行ってみますか？」");
				else if (pc == CLASS_KOISHI)
					strcpy(quest_text[line++], "...ん？いまそこに誰かいたような？」");
				else if (pc == CLASS_ORIN)
					strcpy(quest_text[line++], "あたいもちょいと見物に行こうかな？」");
				else if (pc == CLASS_UTSUHO)
					strcpy(quest_text[line++], "何ならちょいと見物に行ってみるかい？」");
				else if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "そこのお兄さんも行ってみるかい？」");
				else
					strcpy(quest_text[line++], "そこのお姉さんも行ってみるかい？」");
#else
				strcpy(quest_text[line++], "Utsuho - 'Why are there animal ghosts, dragons, and haniwa fighting");
				strcpy(quest_text[line++], "each other as they march through Hell of Blazing Fires? I'm tired of");
				strcpy(quest_text[line++], "shooting them down!'");
				strcpy(quest_text[line++], "  ");
				strcpy(quest_text[line++], "Rin - 'Looks like groups of Animal Realm are starting a war at Former");
				strcpy(quest_text[line++], "Lake of Blood Hell. Let's leave them alone until they're done.");
				strcpy(quest_text[line++], "But if you're confident in your skills, you might join them and win");
				strcpy(quest_text[line++], "someone's favor.");
				if (pc == CLASS_SATORI)
					strcpy(quest_text[line++], "Lady Satori, are you going, too?'");
				else if (pc == CLASS_KOISHI)
					strcpy(quest_text[line++], "...Hm? Is somebody here?'");
				else if (pc == CLASS_ORIN)
					strcpy(quest_text[line++], "I think I'll go and take a look.'");
				else if (pc == CLASS_UTSUHO)
					strcpy(quest_text[line++], "Why don't you go and take a look?'");
				else
					strcpy(quest_text[line++], "Would you like to go and take a look?'");
#endif
			}
		}
		//成功時 味方した勢力で分岐
		else if (comp)
		{

			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA)
			{
				if (pc == CLASS_SAKI)
				{
#ifdef JP
					strcpy(quest_text[line++], "畜生界の他勢力を全て蹴散らし、実に気分がいい！");
					strcpy(quest_text[line++], "さらに情報提供者から面白い品が献上された。");
#else
					strcpy(quest_text[line++], "It felt so good to kick around the other forces from Animal Realm!");
					strcpy(quest_text[line++], "You also have received an interesting item from your informant.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
				{
#ifdef JP
					strcpy(quest_text[line++], "早鬼「見たか！やはり最後に勝つのは暴力だ！");
					strcpy(quest_text[line++], "お前も感動的な強さだったぞ！褒美にこれをやろう！」");
#else
					strcpy(quest_text[line++], "Saki - 'Did you see it? Brute force triumphs in the end! You also were");
					strcpy(quest_text[line++], "impressingly strong! Let me reward you!'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "早鬼「見たか！やはり最後に勝つのは暴力だ！");
					strcpy(quest_text[line++], "お前も感動的な強さだったぞ！褒美にこれをやろう！");
					strcpy(quest_text[line++], "ところで勁牙組に入る気はないか？」");
#else
					strcpy(quest_text[line++], "Saki - 'Did you see it? Brute force triumphs in the end! You also were");
					strcpy(quest_text[line++], "impressingly strong! Let me reward you!'");
					strcpy(quest_text[line++], "By the way, would you like to join the Keiga Family?'");
#endif
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU)
			{
				if (pc == CLASS_YACHIE)
				{
#ifdef JP
					strcpy(quest_text[line++], "四つ巴の抗争を制することに成功した。");
					strcpy(quest_text[line++], "地上の神達が色々と交渉を持ちかけてきたのでうまく利益を引き出してやろう。");
#else
					strcpy(quest_text[line++], "You've won the four-way conflict.");
					strcpy(quest_text[line++], "You also managed to gain some profits by negotiating with the gods on");
					strcpy(quest_text[line++], "surface.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
				{
#ifdef JP
					strcpy(quest_text[line++], "八千慧「素晴らしい。思っていたより遥かに使えるようね。");
					strcpy(quest_text[line++], "あとは私に任せなさい。あなたには充分な報酬を与えましょう。」");
#else
					strcpy(quest_text[line++], "Yachie - 'That was splendid. You're far more useful than I thought. Leave");
					strcpy(quest_text[line++], "the rest to me. I'll reward you handsomely.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "八千慧「自分の力の売り込み方をよく心得ているようですね。");
					strcpy(quest_text[line++], "あなたとはよい関係でありたいものです。");
					strcpy(quest_text[line++], "今回の報酬としてこれを差し上げましょう。」");
#else
					strcpy(quest_text[line++], "Yachie - 'You sure know how to sell your own strength. We'd like to");
					strcpy(quest_text[line++], "maintain good relationship with you. For now, take this as your reward.'");
#endif
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU)
			{
				if (pc == CLASS_YUMA)
				{
#ifdef JP
					strcpy(quest_text[line++], "剛欲同盟長の本気の力の前に他勢力は逃げ去っていった。");
					strcpy(quest_text[line++], "役に立たない同盟者の神から今更支援品の提供の申し出があったが、");
					strcpy(quest_text[line++], "いつかこいつも呑み込んでやろう。");
#else
					strcpy(quest_text[line++], "The other forces fled from the true power of Gouyoku Alliance.");
					strcpy(quest_text[line++], "That useless ally of a god offered you an item; you're going to");
					strcpy(quest_text[line++], "consume it someday.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
				{
#ifdef JP
					strcpy(quest_text[line++], "尤魔「クックック...天晴な強さじゃないか。気に入ったぞ。");
					strcpy(quest_text[line++], "褒美としてこれをくれてやる。");
					strcpy(quest_text[line++], "いつか食うつもりだったが、お前に使わせたほうが利益になりそうだ。」");
#else
					strcpy(quest_text[line++], "Yuuma - 'Hehehe... You're so strong. I like you!");
					strcpy(quest_text[line++], "Take this as your reward.");
					strcpy(quest_text[line++], "I was planning on eating it, but it might be of more use to you.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "尤魔「はて、なぜお前が我々に味方した？同盟に参加したいのか？");
					strcpy(quest_text[line++], "まあいい、次も頼むぞ。そのぶん報酬に色を付けてやる。」");
#else
					strcpy(quest_text[line++], "Yuuma - 'Huh, why did you join us? Are you planning on joining");
					strcpy(quest_text[line++], "the Alliance? Very well, that's for next time. Take this as your");
					strcpy(quest_text[line++], "reward.'");
#endif
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA)
			{
				if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
				{
#ifdef JP
					strcpy(quest_text[line++], "埴輪兵団の無限の力の前に動物霊たちは散り散りに逃げていった。");
					strcpy(quest_text[line++], "情報提供者から協力の謝礼として珍しい品が届けられた。");
#else
					strcpy(quest_text[line++], "The animal ghosts have scattered before the endless power of");
					strcpy(quest_text[line++], "Haniwa Corps. Your informant sent you an interesting item as a");
					strcpy(quest_text[line++], "reward for cooperation.");
#endif
				}
				else if (pr == RACE_HANIWA)
				{
#ifdef JP
					strcpy(quest_text[line++], "磨弓「我々の勝利よ！");
					strcpy(quest_text[line++], "見事な働きをした貴方には褒美の品が下賜されます！");
					strcpy(quest_text[line++], "今後の奮闘に期待しているわ！」");
#else
					strcpy(quest_text[line++], "Mayumi - 'Victory is ours!");
					strcpy(quest_text[line++], "You were outstanding! Here's your reward.");
					strcpy(quest_text[line++], "We're looking forward to working with you in the future!'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "磨弓「動物霊どもの掃討にご協力感謝致します！");
					strcpy(quest_text[line++], "見事な働きをした貴方には褒美の品が下賜されます！");
					strcpy(quest_text[line++], "それと、我々のような完璧な体をお望みならいつでも言ってくださいね！」");
#else
					strcpy(quest_text[line++], "Mayumi - 'Thank you for helping us clean up those animal ghosts!");
					strcpy(quest_text[line++], "You were outstanding! Here's your reward.");
					strcpy(quest_text[line++], "Also, if you want a perfect body like ours, just let us know!'");
#endif
					//TODO:袿姫の店で埴輪に種族変更できるようになるというのはどうだろう？
				}
			}
			else //全滅ルート
			{
#ifdef JP
				strcpy(quest_text[line++], "全ての勢力を単独で殲滅して戻ると、背中に妙なむず痒さを感じた。");
				strcpy(quest_text[line++], "振り向いてみるが誰もおらず、しかしいつの間にか小さな箱が置かれていた。");
				strcpy(quest_text[line++], "どうやらあなたの行動はどこかの誰かを非常に満足させたらしい。");
#else
				strcpy(quest_text[line++], "After defeating all of the forces by yourself and returning, you feel");
				strcpy(quest_text[line++], "someone standing behind you. As you turn around, there's nobody here,");
				strcpy(quest_text[line++], "but there's a small box lying on the ground. Looks like your actions");
				strcpy(quest_text[line++], "have made someone very happy.");
#endif
			}
		}

		//失敗時
		else if (fail)
		{

			if (pc == CLASS_SAKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "またも敵対勢力に暴力で後れを取ってしまった。");
				strcpy(quest_text[line++], "部下の手前気丈に振る舞うが、食事も喉を通らない...");
#else
				strcpy(quest_text[line++], "Once again, your enemies were more violent than you.");
				strcpy(quest_text[line++], "You try to look strong for your subordinates, but you can't");
				strcpy(quest_text[line++], "get a meal down your throat...");
#endif
			}
			else if (pc == CLASS_YACHIE)
			{
#ifdef JP
				strcpy(quest_text[line++], "抗争に敗れてしまった。");
				strcpy(quest_text[line++], "石油利権を得た他勢力を相手にどう巻き返すか頭が痛い...");
#else
				strcpy(quest_text[line++], "You've lost the war.");
				strcpy(quest_text[line++], "Your head hurts thinking about how to deal with other forces, now that the");
				strcpy(quest_text[line++], "oil is in their hands...");
#endif
			}
			else if (pc == CLASS_YUMA)
			{
#ifdef JP
				strcpy(quest_text[line++], "折角の石油利権を奪われてしまった。");
				strcpy(quest_text[line++], "今は大人しく引き下がるのみだ。");
				strcpy(quest_text[line++], "いずれこっそり忍び込んで全部つまみ食いしてやろう。");
#else
				strcpy(quest_text[line++], "Your oil resources were taken away from you.");
				strcpy(quest_text[line++], "You'll have to lay down for a while.");
				strcpy(quest_text[line++], "Some day, you're going to come out and consume them all.");
#endif
			}
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "組長達の本気の抗争は想像以上の激しさだった。");
				strcpy(quest_text[line++], "不覚にも敗走の憂き目にあってしまった。");
#else
				strcpy(quest_text[line++], "The matriarchs were stronger in battle than you expected.");
				strcpy(quest_text[line++], "You were painfully defeated in conflict.");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
#ifdef JP
                strcpy(quest_text[line++], "早鬼「ええい、なんだあの鬱陶しい油は！");
                strcpy(quest_text[line++], "悔しくなんか無いぞ！でも次は絶対に勝つからな！」");
#else
                strcpy(quest_text[line++], "Saki - 'Hey, what's up with that annoying oil?");
                strcpy(quest_text[line++], "I have no regrets! Next time, we'll win for sure!'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
			{
#ifdef JP
                strcpy(quest_text[line++], "八千慧「まあこの手の争いは戦闘が終わってからが本番だ。");
                strcpy(quest_text[line++], "あとは私がやっておく。あなたは元の任務に戻りなさい。」");
#else
                strcpy(quest_text[line++], "Yachie - 'Well, the real conflict starts after a battle like this");
                strcpy(quest_text[line++], "is over. Leave it to me and resume your original mission.'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
			{
#ifdef JP
                strcpy(quest_text[line++], "尤魔「まあいい。あれが誰のものになろうが勝手に忍び込んで食うまでだ。");
                strcpy(quest_text[line++], "しかしなぜこんなに早く連中に石油のことが漏れた？");
                strcpy(quest_text[line++], "やはりあの胡散臭い奴の差し金か？」");
#else
                strcpy(quest_text[line++], "Yuuma - 'Oh well; I don't care who controls it, I'll sneak in and");
                strcpy(quest_text[line++], "devour it anyway. However, how did they find out about the oil so");
                strcpy(quest_text[line++], "quickly?'");
#endif
			}
			else if (pr == RACE_ANIMAL_GHOST)
			{
#ifdef JP
					strcpy(quest_text[line++], "あなたは畜生界住人たちの熾烈な抗争から命からがら逃げ出した。");
#else
					strcpy(quest_text[line++], "You ran for your life from the violent battle between the");
					strcpy(quest_text[line++], "residents of Animal Realm.");
#endif
			}
			else if (pr == RACE_HANIWA)
			{
#ifdef JP
				strcpy(quest_text[line++], "磨弓「動物霊にあれほどのことができるなんて...");
				strcpy(quest_text[line++], "　夢のような現実だわ！」");
#else
				strcpy(quest_text[line++], "Mayumi - 'I never knew animal ghosts could be this powerful...");
				strcpy(quest_text[line++], "  It's like a dream come true!'");
#endif
			}
			else if (pc == CLASS_SATORI || pc == CLASS_KOISHI || pc == CLASS_ORIN || pc == CLASS_UTSUHO)
			{
#ifdef JP
                strcpy(quest_text[line++], "あなたは畜生界住人たちの熾烈な抗争から命からがら逃げ出した。");
#else
                strcpy(quest_text[line++], "You ran for your life from the violent battle between the");
                strcpy(quest_text[line++], "residents of Animal Realm.");
#endif
			}
			else if (player_looks_human_side())//少々乱暴だが人間勢力っぽい外見かどうかで分岐
			{
#ifdef JP
				if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "燐「おやおやお兄さん大丈夫かい？");
				else
					strcpy(quest_text[line++], "燐「おやおやお姉さん大丈夫かい？");

				strcpy(quest_text[line++], "もし大丈夫じゃなくても安心してよ。");
				strcpy(quest_text[line++], "あたいがちゃんと死体の面倒を見てあげるからね。」");
#else
				strcpy(quest_text[line++], "Rin - 'Hey, are you okay?");
				strcpy(quest_text[line++], "If you're not, don't worry!");
				strcpy(quest_text[line++], "I'll take good care of your corpse.'");
#endif
			}
			else
			{
#ifdef JP
				if (p_ptr->psex == SEX_MALE)
				{
					strcpy(quest_text[line++], "燐「おやおやお兄さん大丈夫かい？");
					strcpy(quest_text[line++], "でもお兄さんの死体は面白くなさそうだね。");
				}
				else
				{
					strcpy(quest_text[line++], "燐「おやおやお姉さん大丈夫かい？");
					strcpy(quest_text[line++], "でもお姉さんの死体は面白くなさそうだね。");
				}
				strcpy(quest_text[line++], "ま、温泉に浸かったらさっさと帰った帰った。");
#else
				strcpy(quest_text[line++], "Rin - 'Hey, are you okay?");
                strcpy(quest_text[line++], "Hmm, your corpse wouldn't be interesting to me.");
				strcpy(quest_text[line++], "Just take a dip in the hot springs and go back.'");
#endif
			}

		}
		break;


	case QUEST_HANGOKU2://v1.1.98 連続昏睡事件Ⅱ
		if (accept)
		{
			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{
#ifdef JP
				strcpy(quest_text[line++], "妖精会議の会場を怨霊から護衛してほしいと頼まれた。");
				strcpy(quest_text[line++], "全く愚かなことだ。");
				strcpy(quest_text[line++], "その怨霊は目の前にいるというのに。");
#else
				strcpy(quest_text[line++], "You were asked to protect a gathering of fairies from");
				strcpy(quest_text[line++], "vengeful spirits.");
				strcpy(quest_text[line++], "How foolish!");
				strcpy(quest_text[line++], "You are that vengeful spirit they mention.");
#endif
			}
			else if (pc == CLASS_3_FAIRIES || pc == CLASS_LARVA || pr == RACE_FAIRY)
			{
#ifdef JP
				strcpy(quest_text[line++], "太陽の畑にミステリーサークルが出現した。");
				strcpy(quest_text[line++], "今こそ妖精たちのの大会議が開かれるときだ。");
				strcpy(quest_text[line++], "しかしミステリーサークルの中に強力な怨霊が居座っているらしい。");
				strcpy(quest_text[line++], "妖精の未来のために自分が頑張って追い払わないと。");
#else
				strcpy(quest_text[line++], "A mystery circle has appeared in the Garden of Sun.");
				strcpy(quest_text[line++], "Fairies are gathering there right now.");
				strcpy(quest_text[line++], "However, it looks like there's a vengeful spirit there.");
				strcpy(quest_text[line++], "You have to drive it away for the sake of the fairies.");
#endif
			}
		}

		if (comp)
		{

			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{
#ifdef JP
				strcpy(quest_text[line++], "太陽の畑で妖怪に正体を見破られて戦いになった。");
				strcpy(quest_text[line++], "しかしやはり最近の妖怪は腑抜けている。");
				strcpy(quest_text[line++], "妖精たちがなかなか心惹かれる呪具を持っていたので頂戴しておいた。");
#else
				strcpy(quest_text[line++], "The youkai in Garden of Sun have noticed your true");
				strcpy(quest_text[line++], "identity and attacked. However, it looks like youkai");
				strcpy(quest_text[line++], "right now aren't much to speak of.");
				strcpy(quest_text[line++], "The fairies had some interesting magic items in their");
				strcpy(quest_text[line++], "possession.");
#endif
			}
			//あまりないと思うが、受領時には憑依されていたがその後どこかで新生の薬を飲むなどして憑依解除したとき
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
#ifdef JP
				strcpy(quest_text[line++], "気がつくと足元に大妖怪が倒れ伏しており、");
				strcpy(quest_text[line++], "辺りには怯えきった妖精たちがいた。");
				strcpy(quest_text[line++], "何が起こっているのだろう？最近の記憶がない。");
				strcpy(quest_text[line++], "でもそれはそれとしてくれるものは貰っておこう。");
#else
				strcpy(quest_text[line++], "As you come to your senses, you see a great youkai");
				strcpy(quest_text[line++], "lying down defeated, surrounded by frightened fairies.");
				strcpy(quest_text[line++], "You have no recollection of what has happened, but");
				strcpy(quest_text[line++], "you'll gladly take the items they're giving you.");
#endif
			}
			else if (pc == CLASS_3_FAIRIES || pc == CLASS_LARVA || pr == RACE_FAIRY)
			{
#ifdef JP
				strcpy(quest_text[line++], "我々の勝利だ！");
				strcpy(quest_text[line++], "仲間たちから称賛とともに賞品を送られた。");
				strcpy(quest_text[line++], "この前の出店の売れ残りのような気がするが、");
				strcpy(quest_text[line++], "そんなことはこの達成感の前では些細な問題だ。");
#else
				strcpy(quest_text[line++], "Victory is ours!");
				strcpy(quest_text[line++], "Your friends praised you and sent you rewards.");
				strcpy(quest_text[line++], "It looks like some leftovers from a sale, but");
				strcpy(quest_text[line++], "you don't really care with your sense of accomplishment.");
#endif
			}

		}

		if (fail)
		{
			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{

				//クエスト受領時から怨霊に憑依されているとき
				if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
				{
#ifdef JP
					strcpy(quest_text[line++], "太陽の畑で妖怪に正体を見破られて戦いになった。");
					strcpy(quest_text[line++], "なかなかの強敵で撤退を余儀なくされた。");
					strcpy(quest_text[line++], "最近の妖怪もそう捨てたものではないのかもしれない。");
#else
					strcpy(quest_text[line++], "The youkai in Garden of Sun have noticed your true");
                    strcpy(quest_text[line++], "identity and attacked, forcing you to retreat.");
                    strcpy(quest_text[line++], "Looks like youkai still haven't lost their touch.");
#endif
				}
				//瑞霊の特殊行動で強制失敗になったらここに来るはず
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "「何の策もなしに私の前に立つとは全く見くびられたものだな。");
					strcpy(quest_text[line++], "さて、この体をどう使ってくれようか？」");
#else
                    strcpy(quest_text[line++], "'You have totally underestimated me, going against me");
                    strcpy(quest_text[line++], "without a plan. Now, how shall I use this body?'");
#endif
				}

			}
			//クエスト受領時に怨霊憑依されていたが途中で新生の薬などで憑依解除したとき
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
#ifdef JP
				strcpy(quest_text[line++], "気がつくと太陽の畑で大妖怪に追いかけ回されていた。");
				strcpy(quest_text[line++], "何が起こっているのだろう？最近の記憶がない。");
#else
				strcpy(quest_text[line++], "As you come to your senses, you are chased out from");
				strcpy(quest_text[line++], "Garden of Sun by a great youkai. You have no recollection");
				strcpy(quest_text[line++], "of what has happened.");
#endif
			}

		}

		break;

		//v2.0.13 ヤクザ戦争2クエスト
	case QUEST_YAKUZA_2:
		if (pc == CLASS_ENOKO)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "早鬼「いよいよ始めるぞ。準備はいいな？");
				strcpy(quest_text[line++], "まずはあそこの手配書から好きな奴を選んで叩きのめして来い。");
				strcpy(quest_text[line++], "お前が地上進出の一番槍だ。任せたぞ！」");
#else
				strcpy(quest_text[line++], "Saki - 'Well, let's get started. Are you ready?");
				strcpy(quest_text[line++], "First of all, pick any target out of those wanted posters you like");
				strcpy(quest_text[line++], "and beat them up. You'll be the first to go on the surface.");
				strcpy(quest_text[line++], "I'm leaving this to you!'");
#endif
			}
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "早鬼「よくやった！まずはライバルをひとつ蹴落としたな。");
				strcpy(quest_text[line++], "これを飲んでもっと強くなれよ。」");
#else
				strcpy(quest_text[line++], "Saki - 'Well done! That's one rival kicked down.");
				strcpy(quest_text[line++], "Drink this, and you'll become even stronger.'");
#endif
			}
		}
		if (pc == CLASS_BITEN)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "八千慧「機は熟した。そろそろ始めよう。");
				strcpy(quest_text[line++], "まずは敵勢力の橋頭堡を潰さないとね。");
				strcpy(quest_text[line++], "そこの手配書から好きなほうを選びなさい。");
				strcpy(quest_text[line++], "三頭慧ノ子は魔法の森最深部、");
				strcpy(quest_text[line++], "天火人ちやりは旧灼熱地獄を根城にしているそうよ。」");
#else
				strcpy(quest_text[line++], "Yachie - 'The time is right. Let's get started.");
				strcpy(quest_text[line++], "First of all, we'll have to destroy the enemy forces.");
				strcpy(quest_text[line++], "Pick anyone you like out of those wanted posters.");
				strcpy(quest_text[line++], "Enoko Mitsugashira is in the depths of Forest of Magic,");
				strcpy(quest_text[line++], "and Chiyari Tenkajin resides in Former Hell.'");
#endif
			}
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "八千慧「まずは一歩リードね。");
				strcpy(quest_text[line++], "その報酬で戦力を整えなさい。」");
#else
				strcpy(quest_text[line++], "Yachie - 'That's a step in the right direction.");
				strcpy(quest_text[line++], "Use this reward to enhance your combat prowess.'");
#endif
			}
		}
		if (pc == CLASS_CHIYARI)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "尤魔「やっと来たか。さっそく他の組の地上勢力を追い払うぞ。");
				strcpy(quest_text[line++], "ちょうど奴らは互いに賞金を懸け合ってるからうちで頂いてしまおう。");
				strcpy(quest_text[line++], "勁牙組の犬肉は魔法の森でよく見かける。");
				strcpy(quest_text[line++], "鬼傑組の猿肉はあちこちで活動しているようだ。");
				strcpy(quest_text[line++], "天狗の里には人探しが得意な鳥肉がいるらしいから情報を買うのもいいな。」");
#else
				strcpy(quest_text[line++], "Yuuma - 'Finally, you're here. Let's clean up the other forces.");
				strcpy(quest_text[line++], "They're going to put bounties on each other, so we'll take those.");
				strcpy(quest_text[line++], "Keiga Family's dog is frequently seen in Forest of Magic.");
				strcpy(quest_text[line++], "Kiketsu Family's monkey seems to wander around everywhere.");
				strcpy(quest_text[line++], "There's a bird in Tengu Village who's good at tracking people down,");
				strcpy(quest_text[line++], "so you might want to buy info from her.'");
#endif
			}
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "「スッキリした顔をしているな。");
				strcpy(quest_text[line++], "報酬を受け取るのを忘れるなよ。」");
#else
				strcpy(quest_text[line++], "'You look refreshed.");
				strcpy(quest_text[line++], "Don't forget about your reward.'");
#endif
			}
		}





		break;


	default:
		break;
	}

	/*:::クエスト文章が書き換えられている場合配列の残りを全て空白にする*/
	if(line)
	{
		while(line < 10) quest_text[line++][0] = '\0';

		return TRUE;
	}
	else return FALSE;
}

/*:::Mega Hack - 一部ユニーククラスは特定のクエストを最初から受けられない。*/
/*:::今の＠で受けられないクエストのときTRUEを返す。*/
/*:::処理としては、該当クエストをゲームスタート時に全て失敗済にし、ダンプやステータスに表示しない。*/
/*:::↑後日ミスに気がついた。これだとここでTRUEになったクエストが失敗終了の状態で初期町生成される。つまり例えばフランは妹様クエだけでなく京丸牡丹クエも受けられない。*/
/*:::どうしたものか。とりあえず放置。*/
/*:::↑txxxxxxx.txtの条件分岐に細工して凌ぐしかないか。*/
//ここの他にもcastle_quest()のところで受注の可能不可能を決めている処理がある。
//暴風対策や羽衣婚活など。
//あちらでは種族変容とかしたら受けられるが受けられないときにその後のクエスト(今はないはず)も受けられない。
//こちらではその後のクエストを受けられるが、種族変容したあと受けに来てもすでに内部的に失敗済み。

bool check_ignoring_quest(int questnum)
{

	int pc = p_ptr->pclass;
	int pr;

	//v1.1.80 キャラ生成後は今の種族でなくキャラメイク時の種族を参照する。
	//種族変容したとき内部的失敗のはずのクエストがレベル0失敗としてダンプとかに出てきて変なので
	if (!character_generated)
		pr = p_ptr->prace;
	else
		pr = previous_char.prace;

	switch(questnum)
	{
	case QUEST_SEWER:
	case QUEST_KORI:
		if(pc == CLASS_EIKI) return TRUE;
		break;

		///mod151001 謎の瘴気クエスト改変
	case QUEST_KAPPA:
		if(pr == RACE_KAPPA) return TRUE;
		break;
		//ツパイクエ　レミリア
	case QUEST_TUPAI:
		if(pc == CLASS_REMY) return TRUE;
		break;

		//柳じじい　映姫 妹紅　永琳
	case QUEST_OLD_WILLOW:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_EIRIN) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_KAGUYA) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;

	//墓地警備員急募　お燐、聖
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_ORIN) return TRUE;
		if(pc == CLASS_BYAKUREN) return TRUE;
		break;

	//妹紅への刺客　慧音 妹紅
	case QUEST_KILL_MOKO:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_KEINE) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;

	//輝夜への刺客　うどんげ、てゐ、慧音、永琳、綿月姉妹
	case QUEST_KILL_GUYA:
		if(pc == CLASS_EIRIN) return TRUE;
		if(pc == CLASS_KAGUYA) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_TEWI) return TRUE;
		if(pc == CLASS_UDONGE && !is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) return TRUE;
		if(pc == CLASS_KEINE) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		if(pc == CLASS_TOYOHIME) return TRUE;
		if(pc == CLASS_YORIHIME) return TRUE;
		break;

	case QUEST_MITHRIL: //ミスリル探索　聖、映姫
		if(pc == CLASS_BYAKUREN) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;

		//妹様を止めて　フラン
	case QUEST_FLAN:
		if(pc == CLASS_FLAN) return TRUE;
		break;

		//京丸牡丹　パチュリー
	case QUEST_KYOMARU:
		if(pc == CLASS_PATCHOULI) return TRUE;
		break;
		//守谷神社　神奈子,諏訪子,映姫
	case QUEST_KANASUWA:
		if(pc == CLASS_SUWAKO) return TRUE;
		if(pc == CLASS_KANAKO) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;
		//宴会幹事　霊夢
	case QUEST_HANAMI:
		if(pc == CLASS_REIMU) return TRUE;
		break;
		//お尋ね者　正邪
	case QUEST_WANTED_SEIJA:
		if(pc == CLASS_SEIJA) return TRUE;
		break;
	case QUEST_WANTED_SUMIREKO: //ようこそ幻想郷へ 霊夢と菫子 鈴瑚と清蘭も
		if(pc == CLASS_REIMU || pc == CLASS_SUMIREKO || pc == CLASS_RINGO || pc == CLASS_SEIRAN) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		if(pc == CLASS_TOYOHIME) return TRUE;
		if(pc == CLASS_YORIHIME) return TRUE;

		break;
		//ヘカーティア打倒　クラウンピース、ヘカーティア、純狐
	case QUEST_HECATIA:
		if(pc == CLASS_CLOWNPIECE) return TRUE;
		if (pc == CLASS_VFS_CLOWNPIECE) return TRUE;
		if(pc == CLASS_HECATIA) return TRUE;
		if(pc == CLASS_JUNKO) return TRUE;
		break;
		//輝夜クエスト 5つとも達成したら終わり
		///mod151219 終わらなくした
		///mod151229 ★なしオプションのときには出なくした
		//輝夜除外追加
	case QUEST_KAGUYA:
		if(pc == CLASS_KAGUYA) return TRUE;
		if(ironman_no_fixed_art) return TRUE;
		break;

	case QUEST_OKINA:
		//v1.1.32 とりあえず紫とオキナは不干渉関係っぽいのでパス
		if(pc == CLASS_YUKARI) return TRUE;
		if(pc == CLASS_RAN) return TRUE;
		//v1.1.41 舞と里乃もパス　
		if (pc == CLASS_MAI) return TRUE;
		if (pc == CLASS_SATONO) return TRUE;
		//v1.1.57 隠岐奈
		if (pc == CLASS_OKINA) return TRUE;

		break;

	//v1.1.36
	case QUEST_KOSUZU:
		if(pc == CLASS_YUKARI) return TRUE;
		if(pc == CLASS_KOSUZU) return TRUE;
		break;


	//v1.1.42
	case QUEST_DREAMDWELLER:
		if (pc == CLASS_DOREMY) return TRUE;
		//v1.1.52 こいしは憑依華で「心がスカスカで夢を見ない？」とか言われてたので無効にしてたが秘封ナイトメアダイアリーで夢の世界のこいしが出てきたので有効にする。
		//こいしプレイ中にセーブデータをアップデートするとクエスト失敗扱いになっているのでrd_savefile_new_aux()にも細工した。
		//	if (pc == CLASS_KOISHI) return TRUE;
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) return TRUE;
		//v1.1.57 隠岐奈も夢の中に普通に出て来てたっぽいのでパス
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//水際作戦再発令　動物霊は受けられない
	case QUEST_SHORELINE2:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;

		//v2.0.14 忘れてたので追加
		if (pc == CLASS_YUMA) return TRUE;
		if (pc == CLASS_ENOKO) return TRUE;
		if (pc == CLASS_BITEN) return TRUE;
		if (pc == CLASS_CHIYARI) return TRUE;

		break;

		//破滅のクエスト3　動物霊は受けられない
	case QUEST_DOOM3:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;
		//袿姫も付加　すでに報酬アイテムを持っているので
		if (pc == CLASS_KEIKI) return TRUE;

		break;

	case QUEST_MIYOI: //ドリームイーターの酔災　美宵、萃香、マミゾウは受領不可

		if (pc == CLASS_MIYOI || pc == CLASS_SUIKA || pc == CLASS_MAMIZOU) return TRUE;
		//クラピもゲームオーバーになったとき復活できないので弾いとく
		if (pc == CLASS_CLOWNPIECE) return TRUE;
		break;

	case QUEST_HANGOKU1: //v1.1.84 連続昏睡事件Ⅰ　魔理沙・紫・★が出ないモードのとき受領不可
		if (pc == CLASS_MARISA || pc == CLASS_YUKARI) return TRUE;
		if (ironman_no_fixed_art) return TRUE;

		break;

	case QUEST_MORIYA_2:
		if (pc == CLASS_CHIMATA) return TRUE;//千亦はアビリティカードが報酬のクエストを受けられない
		break;

	case QUEST_REIMU_ATTACK:
		if (pc == CLASS_REIMU) return TRUE;

		break;

	case QUEST_YAKUZA_1:
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//v2.0.13 賞金首クエスト　各勢力組長とどこにも肩入れしない人は受領不可
	case QUEST_YAKUZA_2:
		if (pc == CLASS_SAKI || pc == CLASS_YACHIE || pc == CLASS_YUMA || pc == CLASS_ZANMU || pc == CLASS_HISAMI || pc == CLASS_EIKI || pc == CLASS_REIMU)
			return TRUE;
		break;

	}



	return FALSE;
}

/*
 * Displays all info about a weapon
 *
 * Only accurate for the current weapon, because it includes
 * various info about the player's +to_dam and number of blows.
 */
/*:::武器匠の武器比較サブルーチン　武器名、攻撃回数、命中率、通常攻撃力を表示*/
static void list_weapon(object_type *o_ptr, int row, int col)
{
	char o_name[MAX_NLEN];
	char tmp_str[80];

	/* Effective dices */
	int eff_dd = o_ptr->dd + p_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + p_ptr->to_ds[0];

	int mindam,maxdam;

	/* Print the weapon name */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	c_put_str(TERM_YELLOW, o_name, row, col);

	/* Print the player's number of blows */
#ifdef JP
sprintf(tmp_str, "攻撃回数: %d", p_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", p_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
sprintf(tmp_str, "命中率:  0  50 100 150 200 (敵のAC)");
#else
sprintf(tmp_str, "To Hit:  0  50 100 150 200 (AC)");
#endif

	put_str(tmp_str, row+2, col);

	/* Print the weapons base damage dice */
#ifdef JP
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#else
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#endif

	put_str(tmp_str, row+3, col);

#ifdef JP
c_put_str(TERM_YELLOW, "可能なダメージ:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	mindam = MAX((eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);
	maxdam = MAX((eff_ds * eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);

	/* Damage for one blow (if it hits) */
sprintf(tmp_str, _("攻撃一回につき %d-%d", "1 blow: %d-%d"), mindam, maxdam);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
sprintf(tmp_str, _("１ターンにつき %d-%d", "1 turn: %d-%d"), p_ptr->num_blow[0] * mindam, p_ptr->num_blow[0] * maxdam);
	put_str(tmp_str, row+7, col+1);
}


//武器匠での比較用 object_is_melee_weaponから毒針を除いたもの
//v1.1.93 全面書き換え TV_MAGICWEAPONを追加で指定できるようになった
bool item_tester_hook_compare_weapon(object_type *o_ptr)
{
//	if(item_tester_hook_melee_weapon(o_ptr)) return TRUE;
//	if(o_ptr->tval == TV_MAGICWEAPON) return TRUE;

	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI) return (FALSE);

	return (object_is_melee_weapon(o_ptr));

}


/*
 * Hook to specify "ammo"
 */
static bool item_tester_hook_ammo(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		//case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

static bool item_tester_hook_bow_crossbow(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_BOW:
		case TV_CROSSBOW:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}



/*
 * Compare weapons
 *
 * Copies the weapons to compare into the weapon-slot and
 * compares the values for both weapons.
 */
/*:::武器匠による武器比較*/

static int compare_weapons(int bcost)
{
	int i, n;
	int item, item2;
	object_type *o_ptr[2];
	object_type orig_weapon;
	object_type *i_ptr;
	cptr q, s;
	int row = 2;
	int wid = 38, mgn = 2;
	bool old_character_xtra = character_xtra;
	char ch;
	int total = 0;
	int cost = 0; /* First time no price */

	if(p_ptr->pclass == CLASS_ALICE)
	{
		msg_print(_("あなたの戦闘スタイルは武器匠の理解を超えているらしい。",
                    "Your fighting style is beyond the blacksmith's comprehension."));
		return 0;
	}

	/* Save the screen */
	screen_save();

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	/*:::一時的に武器を退避*/
	i_ptr = &inventory[INVEN_RARM];
	object_copy(&orig_weapon, i_ptr);

	/* Only compare melee weapons */
	item_tester_no_ryoute = TRUE;
	item_tester_hook = item_tester_hook_compare_weapon;

	/* Get the first weapon */
	q = _("第一の武器は？", "What is your first weapon? ");
	s = _("比べるものがありません。", "You have nothing to compare.");

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (0);
	}

	/* Get the item (in the pack) */
	o_ptr[0] = &inventory[item];
	n = 1;
	total = bcost;

	while (TRUE)
	{
		/* Clear the screen */
		clear_bldg(0, 22);

		/* Only compare melee weapons */
		item_tester_no_ryoute = TRUE;
		//v1.1.93 melee_weapon→compare_weaponに修正
		item_tester_hook = item_tester_hook_compare_weapon;

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Diaplay selected weapon's infomation */
		for (i = 0; i < n; i++)
		{
			int col = (wid * i + mgn);

			/* Copy i-th weapon into the weapon slot (if it's not already there) */
			if (o_ptr[i] != i_ptr) object_copy(i_ptr, o_ptr[i]);

			/* Get the new values */
			calc_bonuses();

			/* List the new values */
			list_weapon(o_ptr[i], row, col);
			compare_weapon_aux1(o_ptr[i], col, row + 8);

			/* Copy back the original weapon into the weapon slot */
			object_copy(i_ptr, &orig_weapon);
		}

		/* Reset the values for the old weapon */
		calc_bonuses();

		character_xtra = old_character_xtra;

#ifdef JP
		put_str(format("[ 比較対象: 's'で変更 ($%d) ]", cost), 1, (wid + mgn));
		put_str("(一番高いダメージが適用されます。複数の倍打効果は足し算されません。)", row + 4, 0);
		prt("現在の技量から判断すると、あなたの武器は以下のような威力を発揮します:", 0, 0);
#else
		put_str(format("[ 's' Select secondary weapon($%d) ]", cost), row + 1, (wid * i + mgn));
		put_str("(Only highest damage applies per monster. Special damage not cumulative.)", row + 4, 0);
		prt("Based on your current abilities, here is what your weapons will do", 0, 0);
#endif

		flush();
		ch = inkey();

		if (ch == 's')
		{
			if (total + cost > p_ptr->au)
			{
				msg_print(_("お金が足りません！", "You don't have enough money!"));
				msg_print(NULL);
				continue;
			}

			q = _("第二の武器は？", "What is your second weapon? ");
			s = _("比べるものがありません。", "You have nothing to compare.");

			/* Get the second weapon */
			if (!get_item(&item2, q, s, (USE_EQUIP | USE_INVEN))) continue;

			total += cost;
			cost = bcost / 2;

			/* Get the item (in the pack) */
			o_ptr[1] = &inventory[item2];
			n = 2;
		}
		else
		{
			///mod140430 hookをクリア
			item_tester_no_ryoute = FALSE;
			item_tester_hook = NULL;
			break;
		}
	}

	/* Restore the screen */
	screen_load();

	/* Done */
	return (total);
}

/*:::武器匠の「防御について調べる」コマンド*/
/*
 * Evaluate AC
 *
 * ACから回避率、ダメージ減少率を計算し表示する
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"ダメージ軽減率とは、敵の攻撃が当たった時そのダメージを\n"
		"何パーセント軽減するかを示します。\n"
		"ダメージ軽減は通常の直接攻撃に対してのみ効果があります。\n"
		"(種類が「攻撃する」「怪我を負わせる」「粉砕する」の物)\n \n"
		"敵のレベルとは、その敵が通常何階に現れるかを示します。\n \n"
		"回避率は敵の直接攻撃を何パーセントの確率で避けるかを示し、\n"
		"敵のレベルとあなたのACによって決定されます。\n \n"
		"ダメージ期待値とは、敵の１００ポイントの通常攻撃に対し、\n"
		"回避率とダメージ軽減率を考慮したダメージの期待値を示します。\n";
#else
	const char memo[] =
		"'Protection Rate' means how much damage is reduced by your armor.\n"
		"Note that the Protection rate is effective only against normal "
		"'attack' and 'shatter' type melee attacks, "
		"and has no effect against any other types such as 'poison'.\n \n"
		"'Dodge Rate' indicates the success rate on dodging the "
		"monster's melee attacks.  "
		"It is depend on the level of the monster and your AC.\n \n"
		"'Average Damage' indicates the expected amount of damage "
		"when you are attacked by normal melee attacks with power=100.";
#endif

	int protection;
	int col, row = 2;
	int lvl;
	char buf[80*20], *t;

	/* AC lower than zero has no effect */
	if (iAC < 0) iAC = 0;

	/* ダメージ軽減率を計算 */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("あなたの現在のAC: %3d", iAC), row++, 0);
	put_str(format("ダメージ軽減率  : %3d%%", protection), row++, 0);
	row++;

	put_str("敵のレベル      :", row + 0, 0);
	put_str("回避率          :", row + 1, 0);
	put_str("ダメージ期待値  :", row + 2, 0);
#else
	put_str(format("Your current AC : %3d", iAC), row++, 0);
	put_str(format("Protection rate : %3d%%", protection), row++, 0);
	row++;

	put_str("Level of Monster:", row + 0, 0);
	put_str("Dodge Rate      :", row + 1, 0);
	put_str("Average Damage  :", row + 2, 0);
#endif

	for (col = 17 + 1, lvl = 0; lvl <= 100; lvl += 10, col += 5)
	{
		int quality = 60 + lvl * 3; /* attack quality with power 60 */
		int dodge;   /* 回避率(%) */
		int average; /* ダメージ期待値 */

		put_str(format("%3d", lvl), row + 0, col);

		/* 回避率を計算 */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;
		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100点の攻撃に対してのダメージ期待値を計算 */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("現在のあなたの装備からすると、あなたの防御力は"
		   "これくらいです:", 0, 0);
#else
	prt("Defense abilities from your current Armor Class are evaluated below.", 0, 0);
#endif

	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Hook to specify "broken weapon"
 */
///mod131224  壊れた武器
static bool item_tester_hook_broken_weapon(object_type *o_ptr)
{
 	if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_WEAPON_BROKEN_SWORD) return TRUE;
 	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_BROKEN_DAGGER) return TRUE;
 	if (o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_BROKEN_KATANA) return TRUE;
 	if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_BROKEN_STICK) return TRUE;


	return FALSE;
}

/*:::折れた武器修復 from_ptrから能力を一つto_ptrへコピー*/
static void give_one_ability_of_object(object_type *to_ptr, object_type *from_ptr)
{
	int i, n = 0;
	int cand[TR_FLAG_MAX];
	u32b to_flgs[TR_FLAG_SIZE];
	u32b from_flgs[TR_FLAG_SIZE];

	object_flags(to_ptr, to_flgs);
	object_flags(from_ptr, from_flgs);

	for (i = 0; i < TR_FLAG_MAX; i++)
	{
		switch (i)
		{
		case TR_IGNORE_ACID:
		case TR_IGNORE_ELEC:
		case TR_IGNORE_FIRE:
		case TR_IGNORE_COLD:
		case TR_ACTIVATE:
		case TR_RIDING:
		case TR_SHOW_MODS:
		case TR_HIDE_TYPE:
		//case TR_ES_ATTACK:
		//case TR_ES_AC:
		case TR_FULL_NAME:
		case TR_FIXED_FLAVOR:
		case TR_TY_CURSE:
		case TR_AGGRAVATE:
		case TR_DRAIN_EXP:
		case TR_ADD_L_CURSE:
		case TR_ADD_H_CURSE:
		case TR_MATERIAL:
		case TR_USABLE:
		case TR_READABLE:

			break;
		default:

			//v1.1.73 切れ味や地震を引き継ぐのに武器の種類を考慮するよう修正
			if (i == TR_IMPACT && object_has_a_blade(to_ptr)) break;
			if (i == TR_VORPAL && !object_has_a_blade(to_ptr)) break;
			if (i == TR_EX_VORPAL && !object_has_a_blade(to_ptr)) break;


			if (have_flag(from_flgs, i) && !have_flag(to_flgs, i))
			{
				if (!(is_pval_flag(i) && (from_ptr->pval < 1))) cand[n++] = i;
			}
		}
	}

	if (n > 0)
	{
		int bmax;
		int tr_idx = cand[randint0(n)];
		add_flag(to_ptr->art_flags, tr_idx);
		if (is_pval_flag(tr_idx)) to_ptr->pval = MAX(to_ptr->pval, 1);
		bmax = MIN(3, MAX(1, 40 / (to_ptr->dd * to_ptr->ds)));
		if (tr_idx == TR_BLOWS) to_ptr->pval = MIN(to_ptr->pval, bmax);
		if (tr_idx == TR_SPEED) to_ptr->pval = MIN(to_ptr->pval, 4);
	}

	return;
}

///mod140909 武器修復素材
bool item_tester_hook_repair_material(object_type *o_ptr)
{

	//v1.1.64 EXTRA部屋の霊長園でも使うが、このときは武器を選択できない
	if (object_is_melee_weapon_except_strange_kind(o_ptr))
	{
		if (building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_KEIKI)
			return FALSE;
		else
			return TRUE;
	}
	else if(o_ptr->tval == TV_MATERIAL)
	{
		switch(o_ptr->sval)
		{
		case SV_MATERIAL_HEMATITE:
		case SV_MATERIAL_MAGNETITE:
		case SV_MATERIAL_ARSENIC:
		case SV_MATERIAL_MERCURY:
		case SV_MATERIAL_MITHRIL:
		case SV_MATERIAL_ADAMANTITE:
		case SV_MATERIAL_DRAGONNAIL:
		case SV_MATERIAL_DRAGONSCALE:
		case SV_MATERIAL_METEORICIRON:
		case SV_MATERIAL_HIHIIROKANE:
		case SV_MATERIAL_SCRAP_IRON:
		case SV_MATERIAL_ICESCALE:
		case SV_MATERIAL_NIGHTMARE_FRAGMENT:
		case SV_MATERIAL_HOPE_FRAGMENT:
		case SV_MATERIAL_MYSTERIUM:
		case SV_MATERIAL_IZANAGIOBJECT:
		case SV_MATERIAL_ISHIZAKURA:
		case SV_MATERIAL_BROKEN_NEEDLE:
		case SV_MATERIAL_RYUUZYU:

			return TRUE;
		}
	}
	else if(o_ptr->tval == TV_SOUVENIR)
	{
		switch(o_ptr->sval)
		{
		case SV_SOUVENIR_ILMENITE:
		case SV_SOUVENIR_PHOENIX_FEATHER:
		case SV_SOUVENIR_ASIA:
		case SV_SOUVENIR_SHILVER_KEY:
		case SV_SOUVENIR_MOON_ORB:
			return TRUE;
		}
	}

	return FALSE;
}


/*:::武器修復素材用テーブル TV,SV,素材用power,このアイテムになるための必要power(-1は作成不可)*/
//修復結果決定時は条件を満たす一番下のものが選定されるので、同tvalの武器は必要power値が低い順に並んでいないといけない
const struct guild_arts_type repair_weapon_power_table[] =
{
	{TV_KNIFE,SV_WEAPON_BROKEN_DAGGER ,0,-1," "},
	{TV_KNIFE,SV_WEAPON_KNIFE ,0,0," "},
	{TV_KNIFE,SV_WEAPON_DAGGER ,1,2," "},
	{TV_KNIFE,SV_WEAPON_MAIN_GAUCHE ,2,4," "},
	{TV_KNIFE,SV_WEAPON_YAMAGATANA ,4,6," "},
	{TV_KNIFE,SV_WEAPON_TANTO ,6,8," "},
	{TV_KNIFE,SV_WEAPON_NINJA_SWORD ,7,10," "},
	{TV_KNIFE,SV_WEAPON_HAYABUSA ,15,17," "},
	{TV_KNIFE,SV_WEAPON_DRAGON_DAGGER ,16,-1," "},
	{TV_KNIFE,SV_WEAPON_VAJRA ,20,-1," "},//v1.1.31
	//毒針は選択対象外

	{TV_SWORD,SV_WEAPON_BROKEN_SWORD ,0,-1," "},
	{TV_SWORD,SV_WEAPON_RAPIER ,1,0," "},
	{TV_SWORD,SV_WEAPON_SHORT_SWORD ,2,2," "},
	{TV_SWORD,SV_WEAPON_SABRE ,3,3," "},
	{TV_SWORD,SV_WEAPON_SCIMITAR ,4,5," "},
	{TV_SWORD,SV_WEAPON_LONG_SWORD ,5,6," "},
	{TV_SWORD,SV_WEAPON_BASTARD_SWORD ,7,8," "},
	{TV_SWORD,SV_WEAPON_CLAYMORE ,7,10," "},
	{TV_SWORD,SV_WEAPON_GREAT_SWORD ,9,12," "},
	{TV_SWORD,SV_WEAPON_EXECUTIONERS_SWORD ,12,14," "},
	{TV_SWORD,SV_WEAPON_BLADE_OF_CHAOS ,16,18," "},
	{TV_SWORD,SV_WEAPON_HIHIIROKANE ,20,-1," "},

	{TV_KATANA,SV_WEAPON_BROKEN_KATANA ,0,-1," "},
	{TV_KATANA,SV_WEAPON_SHORT_KATANA ,3,0," "},
	{TV_KATANA,SV_WEAPON_KATANA ,6,5," "},
	{TV_KATANA,SV_WEAPON_GREAT_KATANA ,8,10," "},
	{TV_KATANA,SV_WEAPON_ANCIENT_KATANA ,12,15," "},
	{TV_KATANA,SV_WEAPON_EVIL_KATANA ,18,20," "},

	{TV_HAMMER,SV_WEAPON_SPANNER ,0,0," "},
	{TV_HAMMER,SV_WEAPON_MACE ,2,2," "},
	{TV_HAMMER,SV_WEAPON_KINE ,3,3," "},
	{TV_HAMMER,SV_WEAPON_MORNING_STAR ,4,6," "},
	{TV_HAMMER,SV_WEAPON_FLAIL ,5,8," "},
	{TV_HAMMER,SV_WEAPON_WAR_HAMMER ,7,9," "},
	{TV_HAMMER,SV_WEAPON_BALL_AND_CHAIN ,10,12," "},
	{TV_HAMMER,SV_WEAPON_KANASAIBOU ,16,18," "},

	{TV_STICK,SV_WEAPON_BROKEN_STICK ,0,-1," "},
	{TV_STICK,SV_WEAPON_FLOWER ,2,-1," "},
	{TV_STICK,SV_WEAPON_WIZSTAFF ,10,-1," "},
	{TV_STICK,SV_WEAPON_QUARTERSTAFF ,3,0," "},
	{TV_STICK,SV_WEAPON_SYAKUJYOU ,5,3," "},
	{TV_STICK,SV_WEAPON_SIXFEET ,7,5," "},
	{TV_STICK,SV_WEAPON_NUNCHAKU ,8,7," "},
	{TV_STICK,SV_WEAPON_IRONSTICK ,10,10," "},
	{TV_STICK,SV_WEAPON_OONUSA ,18,18," "},
	{TV_STICK,SV_WEAPON_GROND ,30,-1," "},
	{TV_STICK,SV_WEAPON_KOGASA ,20,-1," "},

	{TV_AXE,SV_WEAPON_PICK ,1,-1," "},
	{TV_AXE,SV_WEAPON_FARANCESCA ,1,0," "},
	{TV_AXE,SV_WEAPON_HATCHET ,2,1," "},
	{TV_AXE,SV_WEAPON_NATA ,3,3," "},
	{TV_AXE,SV_WEAPON_BROAD_AXE ,4,5," "},
	{TV_AXE,SV_WEAPON_BATTLE_AXE ,6,7," "},
	{TV_AXE,SV_WEAPON_LOCHABER_AXE ,9,9," "},
	{TV_AXE,SV_WEAPON_GREAT_AXE ,12,11," "},
	{TV_AXE,SV_WEAPON_MASAKARI ,18,19," "},

	{TV_SPEAR,SV_WEAPON_JAVELIN ,1,0," "},
	{TV_SPEAR,SV_WEAPON_SPEAR ,2,2," "},
	{TV_SPEAR,SV_WEAPON_AWL_PIKE ,3,4," "},
	{TV_SPEAR,SV_WEAPON_LONG_SPEAR ,6,6," "},
	{TV_SPEAR,SV_WEAPON_LANCE ,8,9," "},
	{TV_SPEAR,SV_WEAPON_HEAVY_LANCE ,10,12," "},
	{TV_SPEAR,SV_WEAPON_TRIFURCATE_SPEAR ,17,18," "},

	{TV_POLEARM,SV_WEAPON_GLAIVE ,3,0," "},
	{TV_POLEARM,SV_WEAPON_NAGINATA ,4,3," "},
	{TV_POLEARM,SV_WEAPON_LUCERNE_HAMMER ,7,5," "},
	{TV_POLEARM,SV_WEAPON_WAR_SCYTHE ,8,8," "},
	{TV_POLEARM,SV_WEAPON_HALBERD ,10,10," "},
	{TV_POLEARM,SV_WEAPON_SCYTHE_OF_SLICING ,15,16," "},


	{TV_OTHERWEAPON,SV_OTHERWEAPON_FISHING ,3,-1," "},
	{TV_OTHERWEAPON,SV_OTHERWEAPON_NEEDLE ,6,-1," "},

	{TV_MATERIAL,SV_MATERIAL_HEMATITE,3,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE,5,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ARSENIC,6,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MERCURY,7,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MITHRIL,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE,12,-1," "},
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL,13,-1," "},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE,10,-1," "},
	{TV_MATERIAL, SV_MATERIAL_METEORICIRON ,16,-1," "},
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,25,-1," "},
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON,0,-1," "}, //sv値がランダムで1-18
	{TV_MATERIAL,SV_MATERIAL_ICESCALE,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_HOPE_FRAGMENT,14,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM,20,-1," "},
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT,17,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ISHIZAKURA,5,-1," "},
	{TV_MATERIAL,SV_MATERIAL_BROKEN_NEEDLE,8,-1," "},
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU,12,-1," " },

	{TV_SOUVENIR,SV_SOUVENIR_ILMENITE,13,-1," "},
	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER,20,-1," "},
	{TV_SOUVENIR,SV_SOUVENIR_ASIA,25,-1," "},
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY,25,-1," " },
	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB,10,-1," " },

	{0,0,0,0,_("終端用ダミー", "dummy terminator")}
};





/*:::武器にスレイや武器能力をランダムに付与する。o_ptrは近接武器であること。*/
//これってよく考えたらエゴ武器修復してもスレイがパワーアップしない。まあいいか。
void gain_random_slay_2(object_type *o_ptr)
{

	int tester;

	for(tester = 500; tester > 0; tester--)
	{
		switch(randint1(14))
		{
		case 1:
			if (have_flag(o_ptr->art_flags, TR_CHAOTIC)) break;
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			return;
		case 2:
			if (have_flag(o_ptr->art_flags, TR_VAMPIRIC)) break;
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			return;
		case 3:
			if (have_flag(o_ptr->art_flags, TR_FORCE_WEAPON)) break;
			add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
			return;
		case 4:
			if(!object_has_a_blade(o_ptr)) break;
			if (have_flag(o_ptr->art_flags, TR_EX_VORPAL)) break;
			if (have_flag(o_ptr->art_flags, TR_VORPAL)) add_flag(o_ptr->art_flags, TR_EX_VORPAL);
			else add_flag(o_ptr->art_flags, TR_VORPAL);
			return;
		case 5:
			if(object_has_a_blade(o_ptr)) break;
			if (have_flag(o_ptr->art_flags, TR_IMPACT)) break;
			add_flag(o_ptr->art_flags, TR_IMPACT);
			return;
		case 6:
			if (have_flag(o_ptr->art_flags, TR_KILL_EVIL)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL)) add_flag(o_ptr->art_flags, TR_KILL_EVIL);
			else add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
			return;
		case 7:
			if (have_flag(o_ptr->art_flags, TR_KILL_GOOD)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_GOOD)) add_flag(o_ptr->art_flags, TR_KILL_GOOD);
			else add_flag(o_ptr->art_flags, TR_SLAY_GOOD);
			return;
		case 8:
			if (have_flag(o_ptr->art_flags, TR_KILL_ANIMAL)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL)) add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
			else add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			return;
		case 9:
			if (have_flag(o_ptr->art_flags, TR_KILL_HUMAN)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
			return;
		case 10:
			if (have_flag(o_ptr->art_flags, TR_KILL_UNDEAD)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD)) add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
			else add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			return;
		case 11:
			if (have_flag(o_ptr->art_flags, TR_KILL_DRAGON)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DRAGON)) add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
			else add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
			return;
		case 12:
			if (have_flag(o_ptr->art_flags, TR_KILL_DEITY)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DEITY)) add_flag(o_ptr->art_flags, TR_KILL_DEITY);
			else add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
			return;
		case 13:
			if (have_flag(o_ptr->art_flags, TR_KILL_DEMON)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DEMON)) add_flag(o_ptr->art_flags, TR_KILL_DEMON);
			else add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			return;
		case 14:
			if (have_flag(o_ptr->art_flags, TR_KILL_KWAI)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_KWAI)) add_flag(o_ptr->art_flags, TR_KILL_KWAI);
			else add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
			return;
		}

	}
}


/*:::武器修復ルーチン書き換え*/
static int repair_broken_weapon_aux(int bcost)
{
	s32b cost;
	int item, mater;
	object_type *o_ptr, *mo_ptr; /* broken weapon and material weapon */
	object_kind *k_ptr;
	int i, k_idx, dd_bonus, ds_bonus;
	char basenm[MAX_NLEN];
	cptr q, s; /* For get_item prompt */
	int r_sv=0,r_tv=0,sv_power;
	int row = 7;
	char c;
	int old_weight;

	bool use_weapon;

	/* Clear screen */
	clear_bldg(0, 22);

	/* Notice */
	prt(_("武器の修復には材料として鉱石か別の武器を使う。",
        "Weapon repair uses ore or another weapon as materal."), row, 2);

	/* Get an item */
	q = _("どの折れた武器を修復したい？", "Repair which broken weapon?");
	s = _("修復の必要な物を持っていないようだな。", "You don't have anything that needs repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	if(!(wield_check(item,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	/* 鉱石を材料にするときには無駄とも言えないのでエゴ以上でなくても使える
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("それは直してもしょうがないぜ。", "It is worthless to repair."));
		return (0);
	}
	*/
	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format(_("持ってくるのは一度に一つにしてもらおう。", "One at a time, please."));
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format(_("修復する武器　： %s", "Repairing weapon : %s"), basenm), row+3, 2);

	/* Get an item */
	q = _("どれを修復の材料に使う？", "Use what as repair material?");
	s = _("修復の材料を持っていないようだな。", "You don't have suitables materials for repairing.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_repair_material;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print(_("私の話を聞いていなかったのか？", "Are you listening to me?"));
		return (0);
	}
	if(!(wield_check(mater,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format(_("材料とする武器： %s", "Used as material: %s"), basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + (object_value_real(o_ptr) + object_value_real(mo_ptr)) ;

#ifdef JP
	if (!get_check(format("＄%d申し受ける。よいか？ ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		msg_format(_("持っていないではないか。出直して来い。",
                    "Come back when you have the money."));
		return (0);
	}


	use_weapon = object_is_melee_weapon(mo_ptr);
	old_weight = o_ptr->weight;

	//完成武器Tval決定
	switch(o_ptr->tval)
	{
	case TV_KNIFE:
		r_tv = TV_KNIFE;
		break;
	case TV_SWORD:
		r_tv = TV_SWORD;
		break;
	case TV_KATANA:
		r_tv = TV_KATANA;
		break;
	case TV_STICK:

		screen_save();
//		for(i=1;i<8;i++)Term_erase(12, i, 255);
		Term_clear();

		msg_print(_("この柄をどんな武器にして欲しいのだ？", "What kind of weapon do you want?"));
		put_str(_("a) 鈍器がいい", "a) Blunt weapon"), 2 , 40);
		put_str(_("b) 棒がいい", "b) Staff"), 3 , 40);
		put_str(_("c) 斧がいい", "c) Axe"), 4 , 40);
		put_str(_("d) 槍がいい", "d) Spear"), 5 , 40);
		put_str(_("e) 長柄武器がいい", "e) Polearm"), 6 , 40);

		while(1)
		{
			c = inkey();
			if(c >= 'a' && c <= 'e') break;
		}
		screen_load();
		if(c == 'a') r_tv = TV_HAMMER;
		else if(c == 'b') r_tv = TV_STICK;
		else if(c == 'c') r_tv = TV_AXE;
		else if(c == 'd') r_tv = TV_SPEAR;
		else if(c == 'e') r_tv = TV_POLEARM;
		break;

	default:
		msg_format(_("ERROR:武器修復でTval(%d)の処理が未登録",
                    "ERROR: Unkown tval (%d) in weapon repair logic"),o_ptr->tval);
		return (0);

	}
	if(!r_tv){ msg_format(_("ERROR:武器修復で修復後tvalが未決定",
                            "ERROR: Undefined tval after weapon repair")); return (0);}


	//完成武器Sval決定
	//特殊処理　短剣修復時に竜の鱗を使うと竜鱗の短剣になる
	if(r_tv == TV_KNIFE && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		 r_sv = SV_WEAPON_DRAGON_DAGGER;
	}
	//特殊処理　剣修復時にヒヒイロカネを使うとヒヒイロカネの剣になる
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_SWORD && mo_ptr->sval == SV_WEAPON_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}

	//通常処理　sv_powerを素材から決定、適当に増加処理、値に応じてSval決定
	else
	{
		sv_power = -1;
		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == mo_ptr->tval && repair_weapon_power_table[i].sval == mo_ptr->sval )
				sv_power = repair_weapon_power_table[i].type;
		if(sv_power == -1){ msg_format(_("ERROR:武器修復でこの修復素材のsv_powerが登録されていない",
                                        "ERROR: Unkown sv_power for weapon repair material")); return (0);}

		//鉄屑のパワーはランダム
		if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_SCRAP_IRON) sv_power = randint1(18);
		//アーティファクトは良い物になりやすい
		else if(object_is_artifact(mo_ptr)) sv_power += 3 + randint0(3);

		//+3:42% +6:18% +10:6%
		///mod160206 4→6に変更 +3:57% +6:33% +10:15%
		while(!one_in_(6)) sv_power++;

		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == r_tv && sv_power >= repair_weapon_power_table[i].num &&  repair_weapon_power_table[i].num >= 0)
				r_sv = repair_weapon_power_table[i].sval;

	}
	if(!r_sv){ msg_format(_("ERROR:武器修復で修復後svalが未決定",
                            "ERROR: Undefined sval after weapon repair")); return (0);}
	k_idx = lookup_kind(r_tv,r_sv);
	if(!k_idx){ msg_format(_("ERROR:武器修復完成品k_idxがおかしい",
                            "ERROR: Weird k_idx after weapon repair was finished")); return (0);}


	/* Calculate dice bonuses */
	dd_bonus = o_ptr->dd - k_info[o_ptr->k_idx].dd;
	ds_bonus = o_ptr->ds - k_info[o_ptr->k_idx].ds;
	dd_bonus += mo_ptr->dd - k_info[mo_ptr->k_idx].dd;
	ds_bonus += mo_ptr->ds - k_info[mo_ptr->k_idx].ds;

	/* Change base object */
	k_ptr = &k_info[k_idx];
	o_ptr->k_idx = k_idx;
	o_ptr->weight = k_ptr->weight;
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Copy base object's ability */
	for (i = 0; i < TR_FLAG_SIZE; i++) o_ptr->art_flags[i] |= k_ptr->flags[i];
	///mod150403 MAX()の中にrandint1を入れてたので修正
	if (k_ptr->pval)
	{
		int tester = randint1(k_ptr->pval);
		if(o_ptr->pval < tester) o_ptr->pval = tester;
	}
	if (have_flag(k_ptr->flags, TR_ACTIVATE)) o_ptr->xtra2 = k_ptr->act_idx;

	add_art_flags(o_ptr); //刀に確率で切れ味付与するなどの処理


	//修復素材が武器のとき、素材武器の能力を一部引き継ぐ
	if(object_is_melee_weapon(mo_ptr))
	{
		/* Add one random ability from material weapon */
		give_one_ability_of_object(o_ptr, mo_ptr);

		/* Add to-dam, to-hit and to-ac from material weapon */
		o_ptr->to_d += MAX(0, (mo_ptr->to_d / 3));
		o_ptr->to_h += MAX(0, (mo_ptr->to_h / 3));
		o_ptr->to_a += MAX(0, (mo_ptr->to_a));
	}
	//修復素材がそれ以外のアイテムのとき、独自処理
	//京丸牡丹　ランダム能力付与、修正値少し増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HEMATITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		one_ability(o_ptr);
	}
	//マグネタイト　ランダム属性、修正値少し増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MAGNETITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

	}
	//砒素鉱石　毒殺、修正値少し増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ARSENIC)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_BRAND_POIS);
	}
	//水銀鉱石　ランダムスレイ、修正値少し増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MERCURY)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		gain_random_slay_2(o_ptr);
	}
	//ミスリル　修正値中増加、重量30%減少、ランダム耐性
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MITHRIL)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		o_ptr->weight -= o_ptr->weight * 3 / 10;
		one_resistance(o_ptr);
	}
	//アダマンタイト　ダメージ大幅増加、命中減少、重量50%増加、ダイスボーナス
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ADAMANTITE)
	{
		o_ptr->to_d += 10;
		o_ptr->to_h -= 10;
		o_ptr->weight += o_ptr->weight / 2;
		dd_bonus += 1;
		ds_bonus += 1;
	}
	//竜の鱗 上位耐性+修正値中増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		one_high_resistance(o_ptr);
	}
	//隕鉄　修正値大幅増加、切れ味もしくは地震、ランダム属性とランダムスレイ
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_METEORICIRON)
	{
		if(object_has_a_blade(o_ptr))
		{
			if (have_flag(o_ptr->art_flags, TR_VORPAL)) add_flag(o_ptr->art_flags, TR_EX_VORPAL);
			else add_flag(o_ptr->art_flags, TR_VORPAL);
		}
		else add_flag(o_ptr->art_flags, TR_IMPACT);

		gain_random_slay_2(o_ptr);
		if(one_in_(5)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_COLD);
		else add_flag(o_ptr->art_flags, TR_BRAND_POIS);

		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);

	}
	//竜の爪　修正値大幅増加、ダイスブースト、ランダムスレイ、pvalブースト
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONNAIL)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		gain_random_slay_2(o_ptr);
		dd_bonus++;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//ヒヒイロカネ　準最強武器以上になる　剣でないときダイスブーストと切れ味　修正値大幅増加　ランダムスレイ2つ、ランダム耐性、ランダム属性　
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HIHIIROKANE)
	{
		if(o_ptr->tval != TV_SWORD)
		{
			if(object_has_a_blade(o_ptr))
			{
				add_flag(o_ptr->art_flags, TR_VORPAL);
			}
			dd_bonus++;
		}
		gain_random_slay_2(o_ptr);
		gain_random_slay_2(o_ptr);
		one_high_resistance(o_ptr);
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

		o_ptr->to_d += 10;
		o_ptr->to_h += 10;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//氷の鱗 冷気攻撃+冷気耐性+水耐性+修正値小増加　エゴでない場合水竜エゴになる
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ICESCALE)
	{
		if(object_is_artifact(o_ptr) || object_is_ego(o_ptr))
		{
			o_ptr->to_d += 2 + randint1(3);
			o_ptr->to_h += 2 + randint1(3);
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			add_flag(o_ptr->art_flags, TR_RES_WATER);
		}
		else
		{
			o_ptr->name2 = EGO_WEAPON_SUI_RYU;
			o_ptr->pval = randint1(2);
			o_ptr->to_d += 10 + randint1(4);
			o_ptr->to_h += 10 + randint1(4);
			one_high_resistance(o_ptr);
		}

	}
	//悪夢のかけら　修正値増加、人間スレイ
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_NIGHTMARE_FRAGMENT)
	{
		o_ptr->to_d += 5 + randint1(3);
		o_ptr->to_h += 5 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
		else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);

	}
	//石桜 地獄耐性、たまに吸血や人間スレイ
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ISHIZAKURA)
	{
		o_ptr->to_d += 2 + randint1(3);
		o_ptr->to_h += 2 + randint1(3);

		add_flag(o_ptr->art_flags, TR_RES_NETHER);
		if(one_in_(2))
		{
			if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		}
		if(one_in_(3)) add_flag(o_ptr->art_flags, TR_VAMPIRIC);

	}
	//折れた針　投擲、ランダムスレイ2つ、修正値増加
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
	{
		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_THROW);
		gain_random_slay_2(o_ptr);
		gain_random_slay_2(o_ptr);
	}
	//希望の結晶　修正値大幅増加、対邪スレイ、祝福、光源
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL)) add_flag(o_ptr->art_flags, TR_KILL_EVIL);
		else add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		add_flag(o_ptr->art_flags, TR_BLESSED);
		add_flag(o_ptr->art_flags, TR_LITE);

	}

	//鉄屑 全くランダム
	//ミステリウム　全くランダム強判定、重量二倍
	else if(mo_ptr->tval == TV_MATERIAL && (mo_ptr->sval == SV_MATERIAL_SCRAP_IRON || mo_ptr->sval == SV_MATERIAL_MYSTERIUM))
	{
		do
		{
			if(one_in_(5))gain_random_slay_2(o_ptr);
			if(one_in_(4))one_resistance(o_ptr);
			if(one_in_(3))one_ability(o_ptr);
			if(one_in_(9)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			else if(one_in_(8)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			else if(one_in_(7)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			else if(one_in_(6)) add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			else if(one_in_(5)) add_flag(o_ptr->art_flags, TR_BRAND_POIS);

			if(one_in_(8)) dd_bonus++;
			if(one_in_(8)) ds_bonus++;
			if(one_in_(3)) o_ptr->to_d += randint1(5);
			if(one_in_(3)) o_ptr->to_h += randint1(5);
			if(one_in_(5)) o_ptr->to_a += randint1(7);
			if(o_ptr->pval && one_in_(7)) o_ptr->pval++;

		}while(weird_luck() || mo_ptr->sval == SV_MATERIAL_MYSTERIUM && one_in_(2));

		if(mo_ptr->sval == SV_MATERIAL_MYSTERIUM) o_ptr->weight *= 2;
	}
	//月のイルメナイト　修正値中増加、ダイスブースト、pvalブースト
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_ILMENITE)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		dd_bonus += 2;
		ds_bonus += 2;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//不死鳥の羽根 |焼*火r閃+耐 急食
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_PHOENIX_FEATHER)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_IM_FIRE);
		add_flag(o_ptr->art_flags, TR_RES_FIRE);
		add_flag(o_ptr->art_flags, TR_RES_LITE);
		add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		add_flag(o_ptr->art_flags, TR_CON);
		if(!o_ptr->pval) o_ptr->pval = 3 + randint0(2);

	}
	//エイジャの赤石 /X死悪 /神 r聖閃 pvalブースト
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_ASIA)
	{

		o_ptr->to_d += 10;
		o_ptr->to_h += 10;
		add_flag(o_ptr->art_flags, TR_RES_HOLY);
		add_flag(o_ptr->art_flags, TR_RES_LITE);
		add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
		add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		add_flag(o_ptr->art_flags, TR_KILL_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(o_ptr->pval) o_ptr->pval++;
	}
	//伊弉諾物質 /神 r時 視 pvalブースト
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_IZANAGIOBJECT)
	{

		o_ptr->to_d += 5+randint1(5);
		o_ptr->to_h += 5+randint1(5);
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		add_flag(o_ptr->art_flags, TR_SEE_INVIS);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(o_ptr->pval) o_ptr->pval++;
	}
	//龍珠 修正値増加、/竜 ~竜 ランダム属性 ダイスブースト
	else if (mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_RYUUZYU)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		add_flag(o_ptr->art_flags, TR_ESP_DRAGON);

		if (one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if (one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if (one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

		dd_bonus++;

	}

	//銀の鍵 /Xデ神 r時 魔法難度減少 ダイスブースト
	else if (mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_SHILVER_KEY)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		add_flag(o_ptr->art_flags, TR_FREE_ACT);
		add_flag(o_ptr->art_flags, TR_SEE_INVIS);
		add_flag(o_ptr->art_flags, TR_KILL_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_KILL_DEITY);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		add_flag(o_ptr->art_flags, TR_EASY_SPELL);
		dd_bonus += 2;
		ds_bonus += 2;
	}
	//月の宝玉 /死悪妖
	else if (mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_MOON_ORB)
	{

		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
		add_flag(o_ptr->art_flags, TR_BLESSED);
	}

	else { msg_format(_("ERROR:武器修復でこの素材の処理が書かれていない",
                        "ERROR: Undefined logic for this material for weapon repair")); return (0);}



	/* Dice up */
	if (dd_bonus > 0)
	{
		o_ptr->dd++;
		dd_bonus--;
		for (i = 0; i < dd_bonus; i++)
		{
			if (one_in_(o_ptr->dd + i)) o_ptr->dd++;
		}
	}
	if (ds_bonus > 0)
	{
		o_ptr->ds++;
		ds_bonus--;
		for (i = 0; i < ds_bonus; i++)
		{
			if (one_in_(o_ptr->ds + i)) o_ptr->ds++;
		}
	}

	//高ダイス武器の追加攻撃はpvalが減る
	//鍛冶師が追加攻撃付加したときここに入らない。いずれ入るようにしとくべきか。
	if (have_flag(o_ptr->art_flags, TR_BLOWS)  || o_ptr->name1 == ART_NARSIL )
	{
		if(o_ptr->dd * o_ptr->ds > 30)
		{
			if(o_ptr->pval > 2) o_ptr->pval = 1 + randint1(10) / 7;
			else o_ptr->pval = 1;
		}
		else if(o_ptr->dd * o_ptr->ds > 20)
		{
			if(o_ptr->pval > 2) o_ptr->pval = 2;
			else o_ptr->pval = 1;
		}
		else
		{
			if(o_ptr->pval > 3) o_ptr->pval = 3;
		}
	}

	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

//	if ((o_ptr->name1 == ART_NARSIL) ||
	//v1.1.31b ベッピンもこの方式に当てはまるようにした
	if ((object_is_fixed_artifact(o_ptr)) ||
		(object_is_random_artifact(o_ptr)) ||
		(object_is_ego(o_ptr) && one_in_(7)))
	{

		/* Add one random ability from material weapon */
		if(object_is_melee_weapon(mo_ptr)) give_one_ability_of_object(o_ptr, mo_ptr);
		else
		{
			o_ptr->to_d += randint1(5);
			o_ptr->to_h += randint1(5);
		}

		/* Add one random activation */
		if (!activation_index(o_ptr)) one_activation(o_ptr);

		/* Narsil */
		if (o_ptr->name1 == ART_NARSIL)
		{
			one_high_resistance(o_ptr);
			one_ability(o_ptr);
		}
		if (o_ptr->name1 == ART_ONIKIRIMARU)
			msg_print(_("まさかこの刀を再び見る日が来ようとはな。", "Finally, the day I see this sword once again has came."));
		else
			msg_print(_("これはかなりの業物だったようだ。", "This weapon seems to be exceptionally well made."));
	}

	//戻り武器には投擲適性も付く
	if (have_flag(o_ptr->art_flags, TR_BOOMERANG)) add_flag(o_ptr->art_flags, TR_THROW);

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("よし。修復が済んだぞ。", cost, basenm);
#else
	msg_format("Alright, repair complete.", basenm, cost);
#endif
	msg_print(NULL);
	//鍛冶アイテム扱いにする
	//鍛冶済みアイテムや付喪神使いの妖器(xtra3=SPECIAL_ESSENCE_OTHER)の場合xtra3が保持される。
	if(!object_is_smith(o_ptr)) o_ptr->xtra3 = SPECIAL_ESSENCE_ONI;

	///mod150311 修復武器エッセンス消去バグに対応
	/*:::Hack - 鍛冶師と宝飾師のときのみ特殊フラグON 武器修復とエッセンス付加を両方行ったあと、エッセンスを消去したときにも武器修復された事実がわかるように*/
	if(p_ptr->pclass == CLASS_JEWELER || p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_MISUMARU) o_ptr->xtra1 = 1;

	/* Remove BROKEN flag */
	o_ptr->ident &= ~(IDENT_BROKEN);

	//v1.1.25 入手時レベルとターンを記録
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;

	/* Add repaired flag */
	o_ptr->discount = 99;

	///mod141129 重量再計算
	p_ptr->total_weight -= old_weight;
	p_ptr->total_weight += o_ptr->weight;

	/* Decrease material object */
	inven_item_increase(mater, -1);
	inven_item_optimize(mater);

	/* Copyback */
	p_ptr->update |= PU_BONUS;
	handle_stuff();

	/* Something happened */
	return (cost);
}


/*
 * Repair broken weapon
 */
///sys 武器修復　しばらくは無効
#if 0
static int repair_broken_weapon_aux(int bcost)
{
	s32b cost;
	int item, mater;
	object_type *o_ptr, *mo_ptr; /* broken weapon and material weapon */
	object_kind *k_ptr;
	int i, k_idx, dd_bonus, ds_bonus;
	char basenm[MAX_NLEN];
	cptr q, s; /* For get_item prompt */
	int row = 7;

	/* Clear screen */
	clear_bldg(0, 22);

	/* Notice */
	prt(_("修復には材料となるもう1つの武器が必要です。", "Hand one material weapon to repair a broken weapon."), row, 2);
	prt(_("材料に使用した武器はなくなります！", "The material weapon will disappear after repairing!!"), row+1, 2);

	/* Get an item */
	q = _("どの折れた武器を修復しますか？", "Repair which broken weapon? ");
	s = _("修復できる折れた武器がありません。", "You have no broken weapon to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("それは直してもしょうがないぜ。", "It is worthless to repair."));
		return (0);
	}

	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format(_("一度に複数を修復することはできません！", "They are too many to repair at once!"));
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format(_("修復する武器　： %s", "Repairing: %s"), basenm), row+3, 2);

	/* Get an item */
	q = _("材料となる武器は？", "Which weapon for material? ");
	s = _("材料となる武器がありません。", "You have no material to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_melee_weapon;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print(_("クラインの壷じゃない！", "This is not a klein bottle!"));
		return (0);
	}

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format(_("材料とする武器： %s", "Material : %s"), basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + object_value_real(o_ptr) * 2;

#ifdef JP
	if (!get_check(format("＄%dかかりますがよろしいですか？ ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		object_desc(basenm, o_ptr, OD_NAME_ONLY);
		msg_format(_("%sを修復するだけのゴールドがありません！",
			"You do not have the gold to repair %s!"), basenm);
		msg_print(NULL);
		return (0);
	}

	if (o_ptr->sval == SV_BROKEN_DAGGER)
	{
		int i, n = 1;

		/* Suppress compiler warning */
		k_idx = 0;

		for (i = 1; i < max_k_idx; i++)
		{
			object_kind *k_ptr = &k_info[i];

			if (k_ptr->tval != TV_SWORD) continue;
			if ((k_ptr->sval == SV_BROKEN_DAGGER) ||
				(k_ptr->sval == SV_BROKEN_SWORD) ||
				(k_ptr->sval == SV_DOKUBARI)) continue;
			if (k_ptr->weight > 99) continue;

			if (one_in_(n))
			{
				k_idx = i;
				n++;
			}
		}
	}
	else /* TV_BROKEN_SWORD */
	{
		/* Repair to a sword or sometimes material's type weapon */
		int tval = (one_in_(5) ? mo_ptr->tval : TV_SWORD);

		while(1)
		{
			object_kind *ck_ptr;

			k_idx = lookup_kind(tval, SV_ANY);
			ck_ptr = &k_info[k_idx];

			if (tval == TV_SWORD)
			{
				if ((ck_ptr->sval == SV_BROKEN_DAGGER) ||
					(ck_ptr->sval == SV_BROKEN_SWORD) ||
					(ck_ptr->sval == SV_DIAMOND_EDGE) ||
					(ck_ptr->sval == SV_DOKUBARI)) continue;
			}
			if (tval == TV_POLEARM)
			{
				if ((ck_ptr->sval == SV_DEATH_SCYTHE) ||
					(ck_ptr->sval == SV_TSURIZAO)) continue;
			}
			if (tval == TV_HAFTED)
			{
				if ((ck_ptr->sval == SV_GROND) ||
					(ck_ptr->sval == SV_WIZSTAFF) ||
					(ck_ptr->sval == SV_NAMAKE_HAMMER)) continue;
			}

			break;
		}
	}

	/* Calculate dice bonuses */
	dd_bonus = o_ptr->dd - k_info[o_ptr->k_idx].dd;
	ds_bonus = o_ptr->ds - k_info[o_ptr->k_idx].ds;
	dd_bonus += mo_ptr->dd - k_info[mo_ptr->k_idx].dd;
	ds_bonus += mo_ptr->ds - k_info[mo_ptr->k_idx].ds;

	/* Change base object */
	k_ptr = &k_info[k_idx];
	o_ptr->k_idx = k_idx;
	o_ptr->weight = k_ptr->weight;
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Copy base object's ability */
	for (i = 0; i < TR_FLAG_SIZE; i++) o_ptr->art_flags[i] |= k_ptr->flags[i];
	if (k_ptr->pval) o_ptr->pval = MAX(o_ptr->pval, randint1(k_ptr->pval));
	if (have_flag(k_ptr->flags, TR_ACTIVATE)) o_ptr->xtra2 = k_ptr->act_idx;

	/* Dice up */
	if (dd_bonus > 0)
	{
		o_ptr->dd++;
		for (i = 0; i < dd_bonus; i++)
		{
			if (one_in_(o_ptr->dd + i)) o_ptr->dd++;
		}
	}
	if (ds_bonus > 0)
	{
		o_ptr->ds++;
		for (i = 0; i < ds_bonus; i++)
		{
			if (one_in_(o_ptr->ds + i)) o_ptr->ds++;
		}
	}

	/* */
	if (have_flag(k_ptr->flags, TR_BLOWS))
	{
		int bmax = MIN(3, MAX(1, 40 / (o_ptr->dd * o_ptr->ds)));
		o_ptr->pval = MIN(o_ptr->pval, bmax);
	}

	/* Add one random ability from material weapon */
	give_one_ability_of_object(o_ptr, mo_ptr);

	/* Add to-dam, to-hit and to-ac from material weapon */
	o_ptr->to_d += MAX(0, (mo_ptr->to_d / 3));
	o_ptr->to_h += MAX(0, (mo_ptr->to_h / 3));
	o_ptr->to_a += MAX(0, (mo_ptr->to_a));

	if ((o_ptr->name1 == ART_NARSIL) ||
		(object_is_random_artifact(o_ptr) && one_in_(1)) ||
		(object_is_ego(o_ptr) && one_in_(7)))
	{
		/* Forge it */
		if (object_is_ego(o_ptr))
		{
			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
		}

		/* Add one random ability from material weapon */
		give_one_ability_of_object(o_ptr, mo_ptr);

		/* Add one random activation */
		if (!activation_index(o_ptr)) one_activation(o_ptr);

		/* Narsil */
		if (o_ptr->name1 == ART_NARSIL)
		{
			one_high_resistance(o_ptr);
			one_ability(o_ptr);
		}

		msg_print(_("これはかなりの業物だったようだ。", "This blade seems to be exceptionally."));
	}

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("＄%dで%sに修復しました。", cost, basenm);
#else
	msg_format("Repaired into %s for %d gold.", basenm, cost);
#endif
	msg_print(NULL);

	/* Remove BROKEN flag */
	o_ptr->ident &= ~(IDENT_BROKEN);

	/* Add repaired flag */
	o_ptr->discount = 99;

	/* Decrease material object */
	inven_item_increase(mater, -1);
	inven_item_optimize(mater);

	/* Copyback */
	p_ptr->update |= PU_BONUS;
	handle_stuff();

	/* Something happened */
	return (cost);
}
#endif

static int repair_broken_weapon(int bcost)
{
	int cost;

	screen_save();
	cost = repair_broken_weapon_aux(bcost);
	screen_load();
	return cost;
}

/*
 * Enchant item
 */
/*:::店による武器防具修正値上昇(弓矢含む)
 *:::これを呼ぶ前にitem_tester_hookを設定しておく
 */
static bool enchant_item(int cost, int to_hit, int to_dam, int to_ac)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *o_ptr;
	cptr        q, s;
	int         maxenchant = (p_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
#ifdef JP
	prt(format("現在のあなたの技量だと、+%d まで改良できます。", maxenchant), 5, 0);
	prt(format(" 改良の料金は一個につき＄%d です。", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "どのアイテムを改良しますか？";
	s = "改良できるものがありません。";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif
	/*:::武器防具のインベントリ番号を得る。item_tester_hook使用*/
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* Check if the player has enough money */
	if (p_ptr->au < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%sを改良するだけのゴールドがありません！", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	/*:::引数で指定した回数分アイテムの修正値を増加
　  /*:::（ここバグくさい。breakのせいでto_???が2以上あっても1しか上がらないと思うが）*/
	/* Enchant to hit */
	for (i = 0; i < to_hit; i++)
	{
		if (o_ptr->to_h < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to damage */
	for (i = 0; i < to_dam; i++)
	{
		if (o_ptr->to_d < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if (o_ptr->to_a < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("改良に失敗した。");
#else
		msg_print("The improvement failed.");
#endif

		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("＄%dで%sに改良しました。", cost * o_ptr->number, tmp_str);
#else
		msg_format("Improved into %s for %d gold.", tmp_str, cost * o_ptr->number);
#endif

		/* Charge the money */
		p_ptr->au -= (cost * o_ptr->number);

		if (item >= INVEN_RARM) calc_android_exp();

		/* Something happened */
		return (TRUE);
	}
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
/*:::アイテムと充填数を指定　失敗しない　*/
static void building_recharge(void)
{
	int         item;
	object_type *o_ptr;
	object_kind *k_ptr;
	cptr        q, s;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
#ifdef JP
prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif


	/* Only accept legal items */
	//item_tester_hook = item_tester_hook_recharge;
	//v1.1.86 アビリティカード充填用のルーチンを組んだが、充填有りにすると強すぎるので禁止にした
	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
q = "どのアイテムに魔力を充填しますか? ";
s = "魔力を充填すべきアイテムがない。";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif
	/*:::アイテムを指定する。 床上のアイテムも指定できるが建物入り口なので実質無効　こう書かないとまずい理由でもあるのか？*/
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

	k_ptr = &k_info[o_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	/*鑑定されてなければ50$で鑑定　全充填のときは鑑定料も入ってる*/
	if (!object_is_known(o_ptr))
	{
#ifdef JP
msg_format("充填する前に鑑定されている必要があります！");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((p_ptr->au >= 50) &&
#ifdef JP
get_check("＄50で鑑定しますか？ "))
#else
			get_check("Identify for 50 gold? "))
#endif

		{
			/* Pay the price */
			p_ptr->au -= 50;

			/* Identify it */
			identify_item(o_ptr);

			/* Description */
			object_desc(tmp_str, o_ptr, 0);

#ifdef JP
msg_format("%s です。", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif

			/* Auto-inscription */
			autopick_alter_item(item, FALSE);

			/* Update the gold display */
			building_prt_gold();
		}
		else
		{
			return;
		}
	}


	/*:::充填価格計算*/
	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		int lev = k_info[o_ptr->k_idx].level;
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			/*:::ロッドの充填価格計算　ロッドのpvalは充填ターン数、timeoutは残り充填ターン数と思われる*/
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("それは再充填する必要はありません。");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}

	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		int rank = ability_card_list[o_ptr->pval].rarity_rank;
		int base_charge = ability_card_list[o_ptr->pval].charge_turn;

		if (o_ptr->timeout > 0)
		{
			//普通こんな事起こらんがゲームのバージョンアップでチャージ時間設定を派手に弄ったら起こるかもしれん
			if (base_charge < 1)
			{
				msg_format(_("ERROR:このアイテムのタイムアウト値設定がおかしい",
                            "ERROR: Weird timeout setting for this item"));
				o_ptr->timeout = 0;
				return;
			}

			price = (rank * rank * 500 * o_ptr->timeout) / base_charge;
		}
		else
		{
			price = 0;
			msg_format(_("それは再充填する必要はありません。",
                        "There's no need to recharge this."));
			return;
		}


	}

	else if (o_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if (o_ptr->tval == TV_WAND
		&& (o_ptr->pval / o_ptr->number >= k_ptr->pval))
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if (o_ptr->tval == TV_STAFF && o_ptr->pval >= k_ptr->pval)
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < price)
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%sを再充填するには＄%d 必要です！", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	/*:::ロッドなら充填するかどうか確認*/
	if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
	{
		if (get_check(format(_("＄%d で再充填しますか？", "Recharge for $%d?"), price)))
		{
			/* Recharge fully */
			/*:::ロッドの充填処理　タイムアウト0に*/
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	/*:::杖、棒は回数を確認*/
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("一回分＄%d で何回分充填しますか？",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(p_ptr->au / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		/*:::杖の充填処理と（空）の銘を外す*/
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, 0);
#ifdef JP
msg_format("%sを＄%d で再充填しました。", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((o_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= price;

	/* Finished */
	return;
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
/*:::塔による全ての魔道具充填　インベントリ全てが対象になる以外は単体のときと同じ*/
static void building_recharge_all(void)
{
	int         i;
	object_type *o_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* skip non magic device */
		//v1.1.86 アビリティカード実装で書き換え　勝手版実装当初からこの判定式は合ってなかったが後の分岐で変なのは弾かれるので問題なかったらしい
		//if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		//やはりアビリティカードの充填は中止
		//if (!item_tester_hook_recharge(o_ptr)) continue;
		if (!item_tester_hook_staff_rod_wand(o_ptr)) continue;

		/* need identified */
		if (!object_is_known(o_ptr)) total_cost += 50;


		k_ptr = &k_info[o_ptr->k_idx];

		switch (o_ptr->tval)
		{
		case TV_ROD:
			{
				int lev = k_info[o_ptr->k_idx].level;
				price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
				break;
			}

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - o_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (o_ptr->number * k_ptr->pval - o_ptr->pval) * price;
			break;

			//v1.1.86 アビリティカードの充填...だが中止した
			/*
		case TV_ABILITY_CARD:
			{

				int rank = ability_card_list[o_ptr->pval].rarity_rank;
				int base_charge = ability_card_list[o_ptr->pval].charge_turn;

				if (o_ptr->timeout > 0)
				{
					//普通こんな事起こらんがゲームのバージョンアップで発動型アイテムを所持型アイテムに変えたらここに来る
					if (base_charge < 1)
					{
						msg_format("ERROR:このアイテムのタイムアウト値設定がおかしい");
						o_ptr->timeout = 0;
						return;
					}

					price = (rank * rank * 500 * o_ptr->timeout) / base_charge;
				}

			}
			break;
			*/
		}

		/* if price <= 0 then item have enough charge */
		if (price > 0) total_cost += price;
	}

	if (!total_cost)
	{
#ifdef JP
		msg_print("充填する必要はありません。");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < total_cost)
	{
#ifdef JP
		msg_format("すべてのアイテムを再充填するには＄%d 必要です！", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("すべてのアイテムを ＄%d で再充填しますか？",  total_cost))) return;
#else
	if (!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];

		/* skip non magic device */
		//if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;
		if (!item_tester_hook_recharge(o_ptr)) continue;

		/* Identify it */
		if (!object_is_known(o_ptr))
		{
			identify_item(o_ptr);

			/* Auto-inscription */
			autopick_alter_item(i, FALSE);
		}

		/* Recharge */
		switch (o_ptr->tval)
		{
		case TV_ROD:
		//case TV_ABILITY_CARD:
			o_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if (o_ptr->pval < k_ptr->pval) o_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if (o_ptr->pval < o_ptr->number * k_ptr->pval)
				o_ptr->pval = o_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("＄%d で再充填しました。", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= total_cost;

	/* Finished */
	return;
}

/*:::街移動　宿屋と仙術魔法*/
///mod140403 行き先うまく指定できなかったので修正　ついでに仙術魔法の街移動ではどの街にも行けるようにした。
//v1.1.87 神霊廟出張所から使う場合追加料金を払えば未登録の場所に行けることにした
//is_magic:魔法のときTRUE,建物コマンドのときFALSE 魔法のときは行った場所のある街全てに行ける
bool tele_town(bool is_magic)
{
	int i, x, y;
	int new_town_idx = 0;
	int num = 0;
	bool town_ok[TOWN_MAX];

	//v1.1.87 未登録の街に行くときの追加コスト 0は不可
	//インデックスはTOWN_***に一致
	//v2.0.12 旧地獄街道は魔法の森から行けるようになったし30000から10000に値下げする
	int town_cost[TOWN_MAX] = { 0,0,5000,10000,0,3000,0,0,1000,0,0,0,0,0,0 };

	for(i=0;i<TOWN_MAX;i++) town_ok[i] = FALSE;

	if (dun_level)
	{
#ifdef JP
		msg_print("この魔法は地上でしか使えない！");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if(p_ptr->word_recall)
	{
		msg_print(_("今は帰還待ち中だ。", "You are waiting to be recalled."));
		return FALSE;
	}

	if (p_ptr->inside_arena || p_ptr->inside_battle)
	{
#ifdef JP
		msg_print("この魔法は外でしか使えない！");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	//行ける場所の判定と街一覧の表示
	for (i=1;i<TOWN_MAX;i++)
	{
		char buf[80];
		///sys 町移動で移動できない街を判定している部分
		///mod140329 ハードコーディングにした。さらに仙術の街移動ではどの街にも行けるように。
		//そのうちtown_cost[]とかと統合してフラグをまとめたい
		//if ((i == NO_TOWN) || (i == SECRET_TOWN) || (i == p_ptr->town_num) || !(p_ptr->visit & (1L << (i-1)))) continue;
		if( (!is_magic && i != TOWN_HITOZATO && i != TOWN_TENGU && i != TOWN_CHITEI && i != TOWN_HAKUREI  && i != TOWN_KOURIN )
			&& !(p_ptr->pclass == CLASS_MARISA && i == TOWN_KIRISAME)
			&& !(CLASS_IS_PRISM_SISTERS && i == TOWN_UG)
			) continue;
		//今＠のいる街はパス
		if (i == p_ptr->town_num) continue;

		//v1.1.87 行ったことのない場所を弾く。ただし施設利用の場合追加料金を払えば行ける場所はリストに入れる
		if (!(p_ptr->visit & (1L << (i - 1))))
		{
			if (is_magic) continue;

			if (!town_cost[i]) continue;
			sprintf(buf, _("%c) %-20s (追加料金$%d)", "%c) %-20s (Extra fee $%d)"), I2A(i - 1), town[i].name,town_cost[i]);
		}
		else
		{
			sprintf(buf, "%c) %-20s", I2A(i - 1), town[i].name);
		}

		town_ok[i]= TRUE;
		prt(buf, 5+i, 5);
		num++;
	}

	if (!num)
	{
#ifdef JP
		msg_print("まだ行けるところがない。");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

	//移動可能フラグの立った街を選択する
	while(1)
	{
		prt(_("どこに行きますか:", "Where do you want to go:"), 0, 0);

		i = inkey();

		if (i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		new_town_idx = i - 'a' + 1;

		if (new_town_idx <= 0 || new_town_idx >= TOWN_MAX) continue;
		if(!town_ok[new_town_idx])continue;

		if (!(p_ptr->visit & (1L << (new_town_idx - 1))))
		{
			prt(_("この街には行ったことがない。追加料金を払えば連れて行ってくれるらしい。",
                "You haven't visited this location yet, but you can be transported there for an extra fee."), 0, 0);
			inkey();
			if (get_check_strict(_("支払いますか？", "Pay?"), CHECK_DEFAULT_Y))
			{
				if (p_ptr->au < 300 + town_cost[new_town_idx])//本来の利用料$300をハードコーディング
				{
					msg_print(_("お金が足りない。", "You don't have enough money."));
					continue;
				}
				p_ptr->au -= town_cost[new_town_idx];
			}
			else
			{
				continue;
			}
		}

		break;
	}


	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == new_town_idx)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}

	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


//v1.1.32 索道に乗るコマンド 人里⇔守矢神社
bool riding_ropeway(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if(p_ptr->word_recall)
	{
		msg_print(_("今は帰還待ち中だ。", "You are waiting to be recalled."));
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if(p_ptr->town_num == TOWN_HITOZATO)
	{
		prt(_("「守矢神社までの優雅な空の旅は如何ですか？",
                "'How about an elegant trip through the sky to Moriya Shrine?") , 4, 0);
		prt(_("　護衛も付いて安全安心！お代は往復わずか$500です！」",
                "It's safe and secure! Only $500 for a round trip!'"), 5, 0);
		new_town = TOWN_MORIYA;
	}
	else
	{
		prt(_("「人里にお帰りですか？",
                "'Do you want to return to Human Village?"), 4, 0);
		prt(_("　帰路は無料となっております。」",
                "The return trip is free of charge.'"), 5, 0);
		new_town = TOWN_HITOZATO;
	}

	if (!get_check_strict(_("乗りますか？", "Ride?"), CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == new_town)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}
	clear_bldg(4, 20);

	//フレーバーメッセージ
	switch(randint1(7))
	{
	case 1:
		if(p_ptr->lev > 30)
		{
			prt(_("護衛の白狼天狗が、", "The white wolf tengu escorting you"), 10, 10);
			prt(_("「この人護衛要らないよね？」と言いたげにあなたを見ている...",
                "looks at you as if saying 'Does this person even need an escort?'"), 11, 10);
			break;
		}
		//lev30以下は2へ
	case 2: case 3:
		prt(_("あなたは束の間の空の旅を楽しんだ。", "You enjoyed your air travel."), 10, 10);
		break;
	case 4:
		prt(_("天狗風がいくつも巻き起こったが、", "A lot of tengu winds happened,"), 10, 10);
		prt(_("あなたのゴンドラに届く前に阻まれて散った。", "but they were stopped before reaching the gondola."), 11, 10);
		break;
	default:
		prt(_("あなたはゴンドラから望む美しい景色を楽しんだ。",
            "You enjoyed the beautiful view from the gondola."), 10, 10);
		break;
	}


	prt(_("何かキーを押してください", "Press any key"), 0, 0);
	(void)inkey();
	prt("", 0, 0);


	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	teleport_town_mode = TELE_TOWN_MODE_ROPEWAY;
	screen_load();
	return TRUE;
}



//地霊虹洞
//v1.1.91 魔法の森⇔旧地獄市街
bool pass_through_chirei_koudou(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if (p_ptr->word_recall)
	{
		msg_print(_("今は帰還待ち中だ。", "You are waiting to be recalled."));
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if (p_ptr->town_num == TOWN_KIRISAME)
	{
		prt(_("広い洞窟が奥へと続いている。", "There's a path leading deep into a cave."), 4, 0);
		prt(_("　ここを通れば地底に行けるはずだ...", "  You should be able to reach underground if you go through here..."), 5, 0);
		new_town = TOWN_CHITEI;
	}
	else
	{
		prt(_("地上の魔法の森へと通じている洞窟だ。", "This cave connects to Forest of Magic on surface."), 4, 0);
		new_town = TOWN_KIRISAME;
	}

	if (!get_check_strict(_("洞窟に入りますか？", "Enter the cave? "), CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if (wilderness[y][x].town == new_town)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}
	clear_bldg(4, 20);

	//フレーバーメッセージ
	switch (randint1(5))
	{
	case 1:
		if (p_ptr->pclass != CLASS_MARISA)
		{
			prt(_("何やら大荷物を抱えて逃亡中の魔法使いとすれ違った..",
                "You meet a fleeing magician carrying a lot of baggage..."), 10, 10);
			break;
		}
		//go through
	case 2:
		if (p_ptr->pclass != CLASS_YAMAME)
		{
			prt(_("土蜘蛛が昼寝をしている...", "The tsuchigumo is sleeping..."), 10, 10);
			break;
		}
		//go through
	default:
		prt(_("あなたは薄暗い洞窟の中を歩き始めた...", "You start walking through the dark cave..."), 10, 10);
		break;
	}


	prt(_("何かキーを押してください", "Press any key"), 0, 0);
	(void)inkey();
	prt("", 0, 0);


	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	teleport_town_mode = TELE_TOWN_MODE_CHIREIKOUDOU;
	screen_load();
	return TRUE;
}

/*
 *  research_mon
 *  -KMW-
 */
/*:::モンスター仙人 */
static bool research_mon(void)
{
	int i, n, r_idx;
	char sym, query;
	char buf[128];

	bool notpicked;

	bool recall = FALSE;

	u16b why = 0;

	u16b	*who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBER_IDX */
	static int old_sym = '\0';
	static int old_i = 0;


	/* Save the screen */
	screen_save();

	/* Get a character, or abort */
	/*:::一文字入力を受ける*/
#ifdef JP
if (!get_com("モンスターの文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^M名前):", &sym, FALSE))
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	/*:::このfor文でiが最大以降まで増えるとあとでマッチなしとして弾かれる。^文字は別。*/
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
		/*:::特殊な文字指定のときはフラグを立てる*/
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "全モンスターのリスト");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク・モンスターのリスト");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク外モンスターのリスト");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	/*:::名前検索*/
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("名前(英語の場合小文字で可)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;

			/* Restore */
			screen_load();

			return FALSE;
		}
#ifdef JP
		sprintf(buf, "名前:%sにマッチ",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
sprintf(buf, "%c - %s", sym, "無効な文字");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	/*:::*whoの領域をモンスターの数にあわせて確保、初期化しているらしい？*/
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	/*:::r_info[]から指定したフラグか文字に合うモンスターを選定し、who[]にモンスターインデックスをリストアップしている*/
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];


		///mod140712 可変パラメータモンスターはモンスター仙人から参照できないようにする
		if(r_ptr->flags7 & RF7_VARIABLE) continue;
		//テスト妖精と特殊フィールドは出さない
		if(i == MON_TEST || i == MON_EXTRA_FIELD) continue;

		/* Empty monster */
		if (!r_ptr->name) continue;

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* 名前検索 */
		if (temp[0])
		{
			int xx;
			char temp2[80];

			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
#ifdef JP
				if (iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
#endif
				if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}

#ifdef JP
			strcpy(temp2, r_name + r_ptr->E_name);
#else
			strcpy(temp2, r_name + r_ptr->name);
#endif
			for (xx = 0; temp2[xx] && xx < 80; xx++)
				if (isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);

#ifdef JP
			if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp))
#else
			if (my_strstr(temp2, temp))
#endif
				who[n++] = i;
		}
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Sort by level */
	why = 2;
	query = 'y';

	/* Sort if needed */
	/*:::モンスターをレベル順にソート？このifには必ず入ると思うが何の意味があるのか*/
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBER_IDX */
	if (old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the monster memory */
	/*:::モンスター名を表示し、-とスペースで切り替え、rを押したらそのモンスターの情報を全て表示し記憶する。*/
	while (notpicked)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
Term_addstr(-1, TERM_WHITE, " ['r'思い出, ' 'で続行, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/*** Recall on screen ***/

				/* Get maximal info about this monster */
				/*:::指定したモンスターの全ての情報を記憶する*/
				lore_do_probe(r_idx);

				/* Save this monster ID */
				/*:::思い出ウィンドウに表示*/
				monster_race_track(r_idx);

				/* Hack -- Handle stuff */
				handle_stuff();

				/* know every thing mode */
				/*:::メインウィンドウ上部にモンスターの思い出を表示する*/
				screen_roff(r_idx, 0x01);
				notpicked = FALSE;

				/* XTRA HACK REMEMBER_IDX */
				old_sym = sym;
				old_i = i;
			}

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}


	/* Re-display the identity */
	/* prt(buf, 5, 5);*/

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Restore */
	screen_load();

	return (!notpicked);
}

///mod140112 噂が長くなったので複数行表示のために作ってみた。特定設備の説明表示にも使う予定。
/*:::rumors_new.txtから一行読み込んで\で改行し複数行に分けて表示する。*/
void display_rumor_new(int town_num)
{
	cptr rumor_ptr;
	char buf[1024];
	//char buf2[1024];
	bool err;
	int i;

	//v1.1.92 処理を変えてex_rumor_new()で数値設定することにした
		//bool extra_grassroot_rumor = FALSE;
		//v1.1.26 EXTRA用噂の判定にcharacter_ickyを追加。玉兎のレイシャルでも噂を聞けるの忘れてた
		//v1.1.27 icky>1にしたら全部が玉兎用噂になったので修正。
		//if(EXTRA_MODE && character_icky)
		//	extra_grassroot_rumor = TRUE;

	//v1.1.18 EXTRA用噂
	if(town_num == 23 && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print(_("草の根妖怪ネットワークの者達はもういない。",
                "The Grassroots Network youkai have left."));
		return;
	}

	//LUNATICでは変な噂になる
	if (difficulty == DIFFICULTY_LUNATIC && (town_num < 7)) town_num = 99;

	screen_save();

	//v1.1.18 EXTRA噂追加
	//v1.1.92 処理を変えてex_rumor_new()で数値設定することにした
	//else if(extra_grassroot_rumor) town_num = 23;


	err = get_rnd_line(_("rumors_new.txt", "rumors_new_e.txt"), town_num, buf);
	if (err) strcpy(buf, _("ERROR:その番号の噂は定義されていない。",
                            "ERROR: Undefined rumor number."));

	//roff_to_buf(buf, 74, buf2, sizeof(buf2));
	//rumor_ptr = buf2;
	rumor_ptr = buf;

	/*:::Mega Hack ←一度書いてみたかった:-) */
	/*:::元の文章に'\'が入ってると'\0'に変換し、行末は\0\0にする。*/
	/*:::手動改行のroff_to_buf()みたいなもん。*/
	for(i=0;i < sizeof(buf)-2;i++)
	{
		if(buf[i] == '\0')
			break;
		/*:::一部漢字が改行扱いされてしまうのを抑止。これで正しいのかどうかまったく自信がない。*/
		else if(iskanji(buf[i]))
		{
			i++;
			continue;
		}
		else if(buf[i] == '\\') buf[i] = '\0';
	}
	buf[i] = '\0';
	buf[i+1] = '\0';

	Term_erase(14, 3, 255);
	/*:::15行まで表示する*/
	for (i = 0; i< 15; i++)
	{
		if(rumor_ptr[0] == 0)
			break;
		else
		{
			if(town_num == 0) prt(rumor_ptr, 4+i, 15);
			else prt(rumor_ptr, 4+i, 5);
			Term_erase(14, 5+i, 255);
			rumor_ptr += strlen(rumor_ptr) + 1;
		}
	}

	prt(_("何かキーを押してください", "Press any key"), 0, 0);
	(void)inkey();
	prt("", 0, 0);

	screen_load();

}

/*:::輝夜と話をする　勝利後、サーペントクエ受領専用*/
static void kaguya_quest(void)
{
		int tmp,i;
		quest_type      *q_ptr;

		//v2.0.19 難易度NORMALで紫打倒後に領域に行けてしまうのでフラグ処理を見直す
		bool flag_enter_serpen = FALSE;

		if (quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED)
			flag_enter_serpen = TRUE;

		q_ptr = &quest[QUEST_SERPENT];

		if(q_ptr->status > QUEST_STATUS_UNTAKEN) return;

		if(!flag_enter_serpen)
		{
			if(p_ptr->pclass == CLASS_EIRIN)
				msg_format(_("輝夜「楽しみね永琳！珍しいものが沢山見られそうだわ。」",
                            "Kaguya - 'It's fun, isn't it, Eirin? We can find so many rare items.'"));
			else if(p_ptr->pclass == CLASS_KAGUYA)
				msg_format(_("盆栽はまだ実をつけていない。",
                            "Bonsai hasn't bear fruit yet. "));
			else
				msg_format(_("輝夜「今考えごとをしているの。もう少しあとで訪ねてきて。」",
                            "Kaguya - 'I'm thinking now. Come visit us later.'"));
			return;
		}

		clear_bldg(4, 18);
		//永琳と輝夜のときセリフを変える
		if(p_ptr->pclass == CLASS_EIRIN)
			display_rumor_new(98);
		else if(p_ptr->pclass == CLASS_KAGUYA)
		{
			bool flag = FALSE;

			for(i=0;i<KAGUYA_QUEST_MAX;i++)
			{
				if(a_info[kaguya_quest_art_list[i]].cur_num) flag = TRUE;
			}
			if(flag)
				display_rumor_new(97);
			else
				display_rumor_new(96);
		}
		//v1.1.18 純狐勢力はセリフ変更
		else if(p_ptr->pclass == CLASS_CLOWNPIECE || p_ptr->pclass == CLASS_VFS_CLOWNPIECE || p_ptr->pclass == CLASS_JUNKO || p_ptr->pclass == CLASS_HECATIA)
			display_rumor_new(94);
		//v1.1.13 種族が月の民のとき台詞変更
		else if(p_ptr->prace == RACE_LUNARIAN)
			display_rumor_new(95);
		else
			display_rumor_new(p_ptr->town_num);
		tmp = p_ptr->inside_quest;
		p_ptr->inside_quest = QUEST_SERPENT;
		init_flags |= INIT_ASSIGN;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		q_ptr->status = QUEST_STATUS_TAKEN;
		p_ptr->inside_quest = tmp;

}



/*:::影狼との会話。@が草の根ネットワークに加入できるかどうかも判定する*/
static void talk_kagerou()
{
	bool flag_ok;
	int count;

	/*
	if(p_ptr->pclass == CLASS_BANKI)
	{
		msg_print("あなたは自分の頭の分身を出して話しかけてみた。しかし目新しい反応は返ってこなかった。");
		return;
	}
	*/
	if(EXTRA_MODE && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print(_("草の根妖怪ネットワークの者達はもういない。",
                "The Grassroots Network youkai have left."));
		return;
	}

	/*:::加入済み　一度加入したら参加資格を失っても加入しっぱなしにしておく*/
	if(p_ptr->grassroots)
	{
		if(p_ptr->pclass == CLASS_KAGEROU)
			msg_print(_("わかさぎ姫と世間話をした。", "You make small talk with Wakasagihime."));
		else if(EXTRA_MODE)
			msg_print(_("影狼「みんなで頑張って下を目指しましょう。交換会でもなるべく良い物出すわ。」",
                        "Kagerou - 'Let's do our best and aim for the bottom. I'll give some good items in exchange."));
		else
			msg_print(_("あなたと影狼は神社に似つかわしくない世間話をした。", "You make small talk unsuitable for the shrine with Kagerou."));
		return;
	}

	/*:::加入可能判定*/

	//草の根独自の追加判定
	switch(p_ptr->prace)
	{
	/*:::人間は加入不可　ただし変異が10以上あるとOKされたり*/
	//v1.1.85 ↑加入条件変更。変異の数でなく鯢呑亭と似た判定を使うことにした
		/*
	case RACE_HUMAN:
	case RACE_KOBITO:
		if(count_mutations() > 9) flag_ok=TRUE;
		else flag_ok = FALSE;
		break;
	case RACE_HALF_YOUKAI:
	case RACE_HALF_GHOST:
		if(count_mutations() > 4) flag_ok=TRUE;
		else flag_ok = FALSE;
		break;
		*/

		//v1.1.70 動物霊は「無所属」のみ参加可能
	case RACE_ANIMAL_GHOST:
		if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_NONE)
			flag_ok = TRUE;
		else
			flag_ok = FALSE;
		break;

	/*:::人外は基本加入可*/
	case RACE_YOUKAI:
	case RACE_FAIRY:
	case RACE_MAGICIAN:
	case RACE_WARBEAST:
	case RACE_TSUKUMO:
	case RACE_ANDROID:
	case RACE_YOUKO:
	case RACE_BAKEDANUKI:
	case RACE_NINGYO:
	case RACE_NYUDOU:
	case RACE_IMP:
	case RACE_GOLEM:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		flag_ok = TRUE;
		break;

	//人間側は基本加入不可だが、変身とかしたら加入できるようにする。
	case RACE_HUMAN:
	case RACE_KOBITO:
	case RACE_GREATER_LILLIPUT:
	case RACE_JK:
	case RACE_DEMIGOD:
	case RACE_HALF_YOUKAI:
	case RACE_HALF_GHOST:
	case RACE_ZASHIKIWARASHI://座敷わらしは妖怪勢力からのスパイだが一般妖怪からは人間側と認識されているはず？
	case RACE_MAGIC_JIZO:
		if (!player_looks_human_side()) flag_ok = TRUE;
		else flag_ok = FALSE;

	break;

	//ほか、強大な存在として名が知れてる種族や普通妖怪側になりそうにない種族や通常既に何らかの勢力に所属してる種族は加入不可。
	//TODO:性格「はぐれ物」を追加し、このへんの制限を取っ払ってもいいかもしれない
	default:
		flag_ok = FALSE;
		break;

	}
	//ユニーククラスは一部を除き加入不可
	if(!(cp_ptr->flag_join_grassroots)) flag_ok = FALSE;

	if(flag_ok)
	{
		msg_print(_("あなたには草の根妖怪ネットワークへの参加資格があるらしい。",
                    "You can enter the Grassroots Youkai Network."));
		if (get_check_strict(_("参加しますか？", "Become a member?"), CHECK_DEFAULT_Y))
		{
			if(EXTRA_MODE)
			{
				int ex_bldg_num = f_info[cave[py][px].feat].subtype;
				msg_print(_("影狼「みんな新しい仲間よ！これからもよろしくね。」",
                            "Kagerou - 'Let's welcome our new companion, everyone!"));

				//-Hack- 建物に噂コマンドを追加して再描画する
				sprintf(building[ex_bldg_num].act_names[6], _("噂を聞く", "Listen to rumors"));
				building[ex_bldg_num].letters[6] = 'r';
				building[ex_bldg_num].actions[6] = BACT_RUMORS_NEW;

				show_building(&building[ex_bldg_num]);
			}
			else
			{
				msg_print(_("影狼「皆には話を通しておくわ。まあちょくちょく顔を出してね。」",
                            "Kagerou - 'I'll inform everyone. Drop by here often.'"));
			}
			p_ptr->grassroots = 1;
			//霊夢の占いリセット
			p_ptr->today_mon = 0;
		}
	}
	else
	{
		if(EXTRA_MODE)
			msg_print(_("影狼「要らないアイテムがあったら交換していかない？」",
                        "Kagerou - 'Would you like to trade items you don't need?"));
		else if(p_ptr->pclass == CLASS_EIKI)
			msg_print(_("あなたは影狼に説教をした。", "You give a lecture to Kagerou."));
		else
			msg_print(_("あなたと影狼はどこか空々しい世間話をした。", "You make some small talk with Kagerou."));
	}
	return;
}


/*:::野良神様の神性タイプを確認する*/
static void reimu_deity_check()
{
#ifdef JP
	cptr comment_cosmos[7] = {"混沌の極致","放埒","寛容","普通","堅物","偏執的","秩序の権化"};
	cptr comment_warlike[7] = {"慈愛の示現","友愛","温厚","普通","乱暴","激情","破壊の化身"};
	cptr comment_noble[7] = {"恐怖の顕現","厄介者","無名","普通","好評","人気者","希望の象徴"};
#else
    cptr comment_cosmos[7] = {"Extremely chaotic","Self-serving","Tolerant","Ordinary","Rigid","Devout","Avatar of Law"};
	cptr comment_warlike[7] = {"Generous","Fraternal","Kind","Ordinary","Rough","Violent","Avatar of Destruction"};
	cptr comment_noble[7] = {"Fear Itself","Troublesome","Nameless","Ordinary","Well-liked","Popular","Symbol of Hope"};
#endif

	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_print(_("ERROR:野良神様以外でdeity_check()が呼ばれた",
                    "ERROR: deity_check() called for someone not a stray god"));
		return;
	}
	msg_print(_("霊夢「ええと、貴柱の評判はね・・」",
                "Reimu - 'Hmm, your reputation is...'"));

	c_put_str(TERM_WHITE,format(_("秩序度：%s", "Alignment: %s"),comment_cosmos[p_ptr->race_multipur_val[DBIAS_COSMOS] / 37]),8 , 6);
	c_put_str(TERM_WHITE,format(_("好戦度：%s", "Love of combat: %s"),comment_warlike[p_ptr->race_multipur_val[DBIAS_WARLIKE] / 37]),9 , 6);
	c_put_str(TERM_WHITE,format(_("人気度：%s", "Popularity: %s"),comment_noble[p_ptr->race_multipur_val[DBIAS_REPUTATION] / 37]),10 , 6);

	return;
}


/*:::霊夢の占い　草の根ネットワーク加入後は利用不可*/
/*使用変数：
 *p_ptr->today_mon 占いの結果の番号を格納　毎日0:00に0にリセット
 *p_ptr->barter_value ターゲットダンジョン、モンスター、階数などを格納　お宝は一度生成されたら0になる
 */
static bool reimu_fortune_teller()
{
	/*:::草の根ネットワーク加入後は利用不可*/
	if(p_ptr->grassroots)
	{
		msg_print(_("霊夢は占ってくれなかった。気が乗らないらしい。",
                    "Reimu won't tell you your fortune. Looks like she doesn't feel like doing it."));
		return FALSE;
	}
	/*:::一日一度のみ*/
	if(p_ptr->today_mon)
	{
		msg_print(_("霊夢「さっき占ったじゃない。今日はもう店仕舞いよ。」",
                    "Reimu - 'I've already told you your fortune. My services are done for today.'"));
		return FALSE;
	}
	msg_print(_("霊夢はケープと水晶球を引っ張り出してきた。",
                "Reimu takes out a cape and a crystal ball."));

	if(one_in_(25)) c_put_str(TERM_WHITE,_("面倒臭いわね・・", "So troublesome..."),8 , 6);
	else if(one_in_(4)) c_put_str(TERM_WHITE,_("なになに・・", "What is it..."),8 , 6);
	else if(one_in_(3)) c_put_str(TERM_WHITE,_("ほうほう・・", "Let's see..."),8 , 6);
	else if(one_in_(2)) c_put_str(TERM_WHITE,_("あらあら・・", "Oh, my..."),8 , 6);
	else				c_put_str(TERM_WHITE,_("ふむふむ・・", "Hmmm..."),8 , 6);

	(void)inkey();

	while(1)
	{
		bool fail = FALSE;
		int tester = randint1(10);

		if(tester < 4) p_ptr->today_mon = FORTUNETELLER_TREASURE;
		else if(tester < 7) p_ptr->today_mon = FORTUNETELLER_TROPHY;
		else if(tester < 10) p_ptr->today_mon = FORTUNETELLER_GOODLUCK;
		else  p_ptr->today_mon = FORTUNETELLER_BADLUCK;

		switch(p_ptr->today_mon)
		{
		//特定ダンジョンの特定階になにか良い物生成
		case FORTUNETELLER_TREASURE:
			{
				int target_dungeon;
				int target_level;
				int lev_check = p_ptr->lev + (p_ptr->lev * p_ptr->lev / 50);
				int count = 0;
				do
				{
					if(one_in_(3))
					{
						target_dungeon = DUNGEON_ANGBAND;
						if(max_dlv[target_dungeon])
						{
							target_level = max_dlv[target_dungeon] + randint1(16) - 10;
							if(target_level > d_info[target_dungeon].maxdepth) target_level = d_info[target_dungeon].maxdepth;
							if(target_level < 1) target_level = randint1(5);
						}
						else target_level = randint1(5);
					}
					else
					{
						target_dungeon = 1 + randint1((p_ptr->total_winner)?11:10);
						target_level = rand_range(d_info[target_dungeon].mindepth,d_info[target_dungeon].maxdepth);
					}
					count++;
					if(count>100)
					{
						fail = TRUE;
						break;
					}

					//v1.1.55 ターゲットが鉄獄以外のとき、1/2の確率でフロア制限を無視するようにする

				}while((target_dungeon == DUNGEON_ANGBAND || one_in_(2)) && (target_level > lev_check * 12 / 10 || target_level < lev_check * 4 / 5));
				if(!fail)
				{
					p_ptr->barter_value = target_dungeon * 1000 + target_level;
#ifdef JP
					c_put_str(TERM_WHITE,format("%sの%d階にお宝あり、と出たわ！",(d_name + d_info[target_dungeon].name),target_level),9 , 6);
#else
					c_put_str(TERM_WHITE,format("You'll find treasure on dungeon level %d of %s!", target_level, (d_name + d_info[target_dungeon].name)),9 , 6);
#endif
				}
			}
			break;
		//その日のうちに特定ユニークを倒すと高級品数個ドロップ
		//m_idxをp_ptr->barter_valueへ格納
		//＠のレベル～レベル*2の未打倒で特殊フラグのないユニークモンスター
		case FORTUNETELLER_TROPHY:
			{
				int i;
				int count = 0;
				int target_idx = 0;
				int lev_check = p_ptr->lev + (p_ptr->lev * p_ptr->lev / 50) + 5;
				for(i=1;i<max_r_idx;i++)
				{
					monster_race *r_ptr = &r_info[i];
					if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;
					if (r_ptr->r_akills) continue;
					if (r_ptr->flags1 & RF1_QUESTOR) continue;
					if (r_ptr->flags1 & RF1_FORCE_DEPTH) continue;
					if (r_ptr->level > lev_check) continue;
					if (r_ptr->level < p_ptr->lev) continue;
					if (r_ptr->rarity > 100) continue; //v2.0.11追加 特殊モンスターや登場禁止モンスター
					count++;
					if(one_in_(count)) target_idx = i;

				}
				if(!target_idx)
				{
					fail = TRUE;
				}
				else
				{
					char m_name[80];
					strcpy(m_name, (r_name + r_info[target_idx].name));
					p_ptr->barter_value = target_idx;
					c_put_str(TERM_WHITE,format(_("%sを倒すと幸福が訪れる、と出たわ！",
                                                "Defeating %s will bring you good fortune!"),m_name),9 , 6);
				}
			}
			break;
		//白いオーラよりちょっと良いアイテム生成、金アイテムの金額少し増加
		case FORTUNETELLER_GOODLUCK:
			c_put_str(TERM_WHITE,_("今日の貴方の運気は絶好調よ！", "Today is your lucky day!"),9 , 6);
			break;
		//黒いオーラと同等のアイテム生成、敵生成レベル時々ブースト、HORDE生成率五倍
		case FORTUNETELLER_BADLUCK:
			c_put_str(TERM_WHITE,_("・・今日は家で大人しくしておいたほうがいいかもね。",
                                "...You might want to stay at home for today."),9 , 6);
			break;
		default:
			msg_print(_("ERROR:未定義の占い結果", "ERROR: Undefined fortune telling result"));
			return FALSE;
		}
		if(!fail) break;
	}
	return TRUE;
}



//v1.1.86
//建物コマンド:アビリティカードの交換会
//いずれgrassroots_trading_cards()と統合する？
void trading_ability_cards(void)
{

	int ref_pval, ref_num, ref_cost, ref_totalcost; //プレイヤーが渡したカードの情報
	int cs;
	char c;
	cptr q, s;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	cptr msg_select; //カードを選ぶときのメッセージ
	cptr msg_reject; //提示されたカードに合うカードが見つからなかったときのメッセージ
	cptr msg_finish; //交換終了時メッセージ
	char o_name[MAX_NLEN];
	int item;

	int i, j, k;

	int trade_chance, trade_num; //選定される交換カード候補数,実際に選定されたカード数
	object_type barter_list[10];

	//v2.0.7 千亦プレイ時にはカードランクだけ表示する
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		chimata_comment_card_rank();
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print(_("今は荷物が一杯だ。", "Your inventory is full."));
		return;
	}

	if (!(EXTRA_MODE) && is_daytime())
	{
		if (one_in_(3))
			c_put_str(TERM_WHITE, _("「異世界からの侵略だなんて！もう市場が毎晩成立しっぱなしよ。」",
                                    "'It came from the outside world! The marketplace is still open every night.'"), 10, 20);
		else if (one_in_(2))
			c_put_str(TERM_WHITE, _("「偽天棚のイベントには参加してくれたかしら？」",
                                    "'Have you participated in the event at False Heaven Shelf?'"), 10, 20);
		else
			c_put_str(TERM_WHITE, _("「市場が開くのは夜になってからよ。」",
                                    "'The marketplace will open at night.'"), 10, 20);

		return;
	}
	else if (ability_card_trade_count > 2 && !p_ptr->wizard)
	{
		if(EXTRA_MODE)
			c_put_str(TERM_WHITE, _("「今回の交換は終了よ。また別のフロアで会いましょう。」",
                                    "'Exchanging is over for now. Let's meet again on another floor.'"), 10, 20);
		else
			c_put_str(TERM_WHITE, _("「今日の市場は終了よ。また明日の晩に来てね。」",
                                    "'The marketplace is closed. Come back next night.'"), 10, 20);
		return;

	}

	//交換に出すカードを選び情報取得

	q = _("「さあ、貴方が交換に出すカードを選ぶのです。」",
        "'Now, choose the card you want to exchange.'");
	s = _("「あら、アビリティカードを持っていないのね。」",
        "'Oh, you don't have ability cards.'");
	msg_select = _("参加者たちはカードを提示した。",
                    "Participants presented their cards.");
	msg_reject = _("「残念だけど、誰も貴方の出したカードに見合うカードを持っていないようね。」",
                    "'Unfortunately, no one else has a card to match yours.'");

	if(EXTRA_MODE)
		msg_finish = _("虹色に塗りたくられた部屋でカードの交換が行われた。",
                        "You exchanged cards in a rainbow-colored room.");
	else
		msg_finish = _("月光の下でカードの交換が行われた。",
                        "You exchanged cards in the moonlight.");


	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	ref_num = 1; //アビリティカード交換は常に一枚ずつ
	ref_pval = o_ptr->pval;

	if (ref_pval < 0 || ref_pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:trading_ability_cards()に不正なカードidx(%d)を持ったアビリティカードが渡された",
                    "ERROR: Ability card with incorrect card idx (%d) was passed to trading_ability_cards()"), ref_pval);
		return;
	}

	for (i = 0; i<10; i++) object_wipe(&barter_list[i]);

	//交換候補のカードを生成する
	//高ランクのカードは交換相手が少ない
	trade_chance = 5 + (p_ptr->lev / 2 + randint1(p_ptr->lev / 2)) / 9;
	if (ability_card_list[ref_pval].rarity_rank == 4) trade_chance = trade_chance * 3 / 4;
	else if (ability_card_list[ref_pval].rarity_rank >= 3) trade_chance = trade_chance * 4 / 5;


	trade_num = 0;

	//カード生成数繰り返す
	for (i = 0; i<trade_chance && i<10; i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//カードリストを全部チェック
		for (j = 0; j<ABILITY_CARD_LIST_LEN; j++)
		{
			int k;
			bool flag_ok = TRUE;
			//通常生成されないカードは弾く
			if (ability_card_list[j].rarity_rank<1 || ability_card_list[j].rarity_rank>4) continue;
			//＠が提示したカードと同じのは弾く
			if (j == ref_pval) continue;
			//すでに選定されたカードと同じのは弾く
			for (k = 0; k<trade_num; k++) if (j == barter_list[k].pval) flag_ok = FALSE;
			if (!flag_ok) continue;

			//出したカードの半分以下の交換価値のカードは弾く
			if (ability_card_list[j].trade_value < ability_card_list[ref_pval].trade_value / 2) continue;
			//出したカードより上の交換価値のカードは確率で弾く
			if (randint1(ability_card_list[ref_pval].trade_value / 2) < (ability_card_list[j].trade_value - ability_card_list[ref_pval].trade_value)) continue;

			//出したカードより上のランクのカードは*非常に*出にくくする
			//ランク3→4を狙う場合、交換価値判定で約80%,この判定で0.8%,通ったカードからランダムに選ぶところで7%くらいの確率を通る必要があるはず。6枚候補があるとしても出現率0.3%。一晩3回トレードしても1%弱。
			if (ability_card_list[ref_pval].rarity_rank < ability_card_list[j].rarity_rank)
			{
				if (ability_card_list[ref_pval].rarity_rank == 3)
				{
					if(!one_in_(128)) continue;
				}
				else if (ability_card_list[ref_pval].rarity_rank == 2)
				{
					if (!one_in_(64)) continue;
				}
				else
				{
					if (!one_in_(8)) continue;
				}
			}

			//ここまで通ったカードからランダムに選ぶ
			cnt++;
			if (!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//選択されたカードがある場合barter_listに登録
		if (cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
			tmp_o_ptr->pval = tmp_pv;

			//*鑑定*済にする
			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			tmp_o_ptr->ident |= (IDENT_MENTAL);


			trade_num++;
		}
	}

	//2つ以下の場合場合終了
	if (trade_num < 2)
	{
		c_put_str(TERM_WHITE, msg_reject, 4, 6);
		inkey();
		ability_card_trade_count++;
		return;
	}

	//カードを表示し選ぶ
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, _("(交換しない：ESC)", "(Don't exchange: ESC)"), 5, 6);
		for (i = 0; i<trade_num; i++)
		{
			int color = TERM_WHITE;
			object_type *tmp_o_ptr = &barter_list[i];
			if (cs == i) color = TERM_YELLOW;
			else if (ability_card_list[barter_list[i].pval].rarity_rank > ability_card_list[ref_pval].rarity_rank)
				color = TERM_VIOLET;
			else if (ability_card_list[barter_list[i].pval].rarity_rank < ability_card_list[ref_pval].rarity_rank)
				color = TERM_BLUE;

			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			object_desc(o_name, tmp_o_ptr, 0);
			//msg_format("%d:%s",o_value,o_name);
			c_put_str(color, format("%c) %s", 'a' + i, o_name), 6 + i, 10);
		}
		c = inkey();
		if (c == ESCAPE)
		{
			//if (get_check_strict("このフロアではもうトレードできません。よろしいですか？", CHECK_OKAY_CANCEL))
				break;
		}
		else if (c == '2' && cs < (trade_num - 1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c = cs + 'a';
		else if (c >= 'a' && c <= 'a' + trade_num - 1)
		{
			object_desc(o_name, &barter_list[c - 'a'], 0);

			if (ability_card_list[barter_list[c - 'a'].pval].rarity_rank < ability_card_list[ref_pval].rarity_rank)
				msg_format(_("%sはあなたが出したカードよりランクが低い。",
                            "%s has lower rank than the card you're trading away."),o_name);
			else
				msg_format(_("%sを受け取る。", "You will receive %s."), o_name);

			if (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

			//選択したら交換処理
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c - 'a']);
			msg_format(_("%sを受け取った。", "You have received %s."), o_name);

			break;
		}
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, msg_finish, 10, 10);
	ability_card_trade_count++;
	inkey();
	clear_bldg(4, 18);


	//実行済みフラグ立てる
	//ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}



//v1.1.87
//建物コマンド:アビリティカードの販売所
//examine:カードの説明文を読む
void buy_ability_card(bool examine)
{
	int trade_num = 10;
	int cs, item_new;
	char c;
	object_type *o_ptr;
	cptr msg_select; //カードを選ぶときのメッセージ
	char o_name[MAX_NLEN];
	bool flag_no_item = TRUE;

	bool browse_mode = FALSE;

	int i, j, k;

	object_type barter_list[10];


	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		int card_rank = CHIMATA_CARD_RANK;

		if (card_rank < 3)
		{
			c_put_str(TERM_WHITE, _("「こちらはすでに準備万端ですよ。", "'I'm ready as well."), 10, 20);
			c_put_str(TERM_WHITE, _("商品の確保は貴方に懸かっているんですから頑張ってくださいね？」",
                                "It's up to you to secure the goods, so good luck with that!'"), 11, 20);
		}
		else if (card_rank < 5)
		{
			c_put_str(TERM_WHITE, _("「売れ始めましたよ！", "'The sales are starting!"), 10, 20);
			c_put_str(TERM_WHITE, _("面白くなってきたわー！」", "'Now it's getting interesting!'"), 11, 20);
		}
		else if (card_rank < 8)
		{
			c_put_str(TERM_WHITE, _("「大繁盛ですよ！", "'We're making a great profit!"), 10, 20);
			c_put_str(TERM_WHITE, _("やはり飯綱丸様の目に狂いはありませんでしたね。」",
                                    "As I knew, Miss Iizunamaru was right on point.'"), 11, 20);
		}
		else
		{
			c_put_str(TERM_WHITE, _("「い、忙しい...", "'S-so busy..."), 10, 20);
			c_put_str(TERM_WHITE, _("そろそろ利益確定して手仕舞いにしませんか...？」",
                                    "Isn't it time to lock in the profits we got and close the business?'"), 11, 20);
		}

		return;
	}
	if (p_ptr->pclass == CLASS_TSUKASA)
	{
		msg_print(_("主はカードで十分に稼いだ。もうここに用はない。",
                    "Your master already has plenty of cards. There's nothing for you to do here."));
		return;
	}


	if (p_ptr->pclass == CLASS_MIKE)
	{
		msg_print(_("カード販売所には招き猫たちが群れていて近寄りたくない。",
                    "You don't want to approach the card trading office swarming with maneki-neko."));
		return;
	}

	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		c_put_str(TERM_WHITE, _("「済みませんねえ、ここはカードの売人専用の市場なんですよ。」",
                                "'Sorry, but this market is for card traders only.'"), 10, 20);
		return;
	}


	if (inventory[INVEN_PACK - 1].k_idx && !examine)
	{
		msg_print(_("今は荷物が一杯だ。", "Your inventory is full."));
		return;
	}

	if(examine)
		msg_select = _("「ふむふむ、どれをご確認なさいますか？」",
                        "'Hmmm, which one would you like to examine?'");
	else
		msg_select = _("「さてさて、どれをお買い上げになりますか？」",
                        "'Well, which one would you like to purchase?'");

	//make_ability_card_store_list()で決まったカードidxリストを使って在庫リストを作る
	for (i = 0; i<10; i++)
	{
		int card_idx = p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT];

		if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
		{
			msg_format(_("ERROR:buy_ability_cards()に渡されたcard_idx(%d)が範囲外",
                        "ERROR: card_idx (%d) passed to buy_ability_cards() is out of range"), card_idx);
			return;
		}

		o_ptr = &barter_list[i];
		object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(o_ptr, card_idx, 0, 0);
		flag_no_item = FALSE;
	}

	//時間経過で在庫の一部を売り切れにする？

	//if (flag_no_item)
	//{
	//	c_put_str(TERM_WHITE, "「お陰様で在庫が全て捌けちゃいました。また今度来てください。」", 10, 20);
	//	return;
	//}
	//↑フラグの処理をミスってて全部在庫買い取ってもこのメッセージは出ない。まあ大した問題でもないので消そう


	//カードを表示し選ぶ
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, _("(キャンセル：ESC)", "(Cancel: ESC)"), 5, 6);
		for (i = 0; i<trade_num; i++)
		{
			int value;
			int color;
			cptr desc;
			int price = 0;
			o_ptr = &barter_list[i];


			//pval=0(空白のカード)は売り切れ扱い
			if (!o_ptr->pval)
			{
				color = TERM_L_DARK;
				desc = format(_("(売り切れ)", "(Sold out)"));

			}
			else
			{
				color = TERM_WHITE;
				price = calc_ability_card_price(o_ptr->pval);

				//「資本主義のジレンマ」のカードによる価格乱高下
				//p_ptr->magic_num2[10-19)が50を元値として1ごとに2%上下する
				if (p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT])
				{
					int old_price = price;
					price += price * (p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT] - 50) * 2 / 100;
					if (cheat_peek) msg_format("price:%d -> %d", old_price, price);
				}

				object_desc(o_name, o_ptr, 0);
				desc = format("%c) %-50s  $%d", 'a' + i, o_name, price);

			}
			if (cs == i) color = TERM_YELLOW;

			c_put_str(color, desc, 6 + i, 10);
		}
		c = inkey();
		if (c == ESCAPE)
		{
			//if (get_check_strict("このフロアではもうトレードできません。よろしいですか？", CHECK_OKAY_CANCEL))
			break;
		}
		else if (c == '2' && cs < (trade_num - 1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c = cs + 'a';
		else if (c >= 'a' && c <= 'a' + trade_num - 1)
		{

			int price;
			int pos = c - 'a';
			o_ptr = &barter_list[pos];

			if (!o_ptr->pval)
			{
				msg_print(_("それは売り切れだ。", "It's sold out."));
				continue;
			}

			if (examine)//調査
			{

				hack_flag_store_examine = TRUE;
				screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
				hack_flag_store_examine = FALSE;

			}
			else //購入
			{
				price = calc_ability_card_price(o_ptr->pval);

				//「資本主義のジレンマ」のカードによる価格乱高下
				//p_ptr->magic_num2[10-19)が50を元値として1離れるごとに2%上下する
				if (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT])
				{
					price += price * (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT] - 50) * 2 / 100;
				}


				if (price > p_ptr->au)
				{
					msg_print(_("お金が足りない。", "You don't have enough money."));
					continue;
				}

				object_desc(o_name, o_ptr, 0);
				msg_format(_("%sを購入する。", "Purchasing %s."), o_name);
				if (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

				inven_carry(o_ptr);
				msg_format(_("%sを購入した。", "You have purchased %s."), o_name);
				p_ptr->au -= price;
				building_prt_gold();

				//合計支払額に加算
				//if (total_pay_cardshop < 100000000) total_pay_cardshop += price;
				add_cardshop_profit(price);

				//カード在庫リストの該当箇所を購入済みの0にする
				p_ptr->magic_num2[c - 'a' + ABLCARD_MAGICNUM_SHIFT] = 0;

				break;
			}

		}
	}

	clear_bldg(4, 18);



}

//v1.1.87
//アビリティカードを売却する
void	sell_ability_card(void)
{
	int item;
	cptr q, s;
	object_type *o_ptr;
	int amt;
	int price = 0;




	q = _("「アビリティカードを換金するのね？」", "'You're selling ability cards?'");
	s = _("アビリティカードを持っていない。", "You don't have ability cards.");

	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	if (o_ptr->number > 1)
	{
		amt = get_quantity(_("何枚ですか？:", "How many?:"), o_ptr->number);
		if (amt <= 0) return;
	}
	else amt = 1;

	//価格計算　販売所の1/3
	price = (calc_ability_card_price(o_ptr->pval) + 2) / 3 * amt;


#ifdef JP
	msg_format("「%s」のカード%d枚を＄%dで買い取ってくれるらしい。", ability_card_list[o_ptr->pval].card_name, amt, price);
#else
    msg_format("%d card(s) '%s' will sell for $%d.", amt, ability_card_list[o_ptr->pval].card_name, price);
#endif

	if (!get_check_strict(_("売りますか？", "Sell?"), CHECK_DEFAULT_Y))
	{
		return;
	}

	p_ptr->au += price;

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
	building_prt_gold();

}


/*:::アイテム交換会*/
///mod160307 EXTRAモードで使えるよう書き直した
void grassroots_barter()
{
	int cs, item_new;
	char c;
	bool flag_1=FALSE,flag_2 = FALSE, flag_3 = FALSE;//まだアイテムを渡してないフラグ、まだ時間が立っていないフラグ、アイテム受け取り開始フラグ
	//未加入時　ここには来ないはず
	if(!p_ptr->grassroots && !EXTRA_MODE)
	{
		 msg_print(_("ERROR:grassroots未加入でbarter()が呼ばれている",
                    "ERROR: barter() was called for a non grassroots member"));
		 return;
	}

	if(EXTRA_MODE)
	{
		if(ex_buildings_param[f_info[cave[py][px].feat].subtype]==255)
		{
			msg_print(_("草の根妖怪ネットワークの者達はもういない。",
                "The Grassroots Network youkai have left."));
			return;
		}
		flag_1 = TRUE;
		flag_3 = TRUE;
	}
	else
	{
		if(p_ptr->grassroots == 1) flag_1 = TRUE;
		else if((s32b)(p_ptr->grassroots + 50000) > turn) flag_2 = TRUE; //50000:半日
		else flag_3 = TRUE;
	}

	//アイテム引渡し時に荷物がいっぱいだったら終了
	if(flag_3 && inventory[INVEN_PACK-1].k_idx)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("わかさぎ姫「あの、荷物が一杯のようですが・・」",
                                                        "Wakasagihime - 'Um, looks like your inventory is already full...'"));
		else msg_print(_("影狼「貴方荷物が一杯よ。」", "Kagerou - 'Your inventory is full.'"));
		return;
	}



	/*:::まだアイテムを渡していないとき*/
	if(flag_1)
	{
		object_type forge;
		object_type *q_ptr = &forge;
		object_type *o_ptr;
		cptr q, s;
		char o_name[MAX_NLEN];
		int item, value;

		if(p_ptr->pclass == CLASS_KAGEROU)
		{
			q = _("何を交換に出しますか？", "What do you want to offer for exchange?");
			s = _("もう少し交換できそうなものをお願いします・・",
                "I'd like something more suitable for exchange...");
		}
		else
		{
			q = _("どのアイテムを出すの？", "Trade which item?");
			s = _("貴方碌な物持ってないじゃない。", "You don't have valuable items.");
		}
		item_tester_hook = object_is_not_worthless;//価格が1以上のアイテムを判定するhookを作る

		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP)))
		{
			//flush();
			return;
		}
		if(!(wield_check(item,INVEN_PACK,0)))
		{
			//flush();
			return;
		}
		o_ptr = &inventory[item];

		if (object_is_cursed(o_ptr))//paranoia
		{
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("わかさぎ姫「あの、それ呪われてますよ・・」",
                                                            "Wakasagihime - 'Uh, it's cursed..."));
			else msg_print(_("影狼「ちょっと。それ呪われてるわ。」",
                            "Kagerou - 'Hold on. It's cursed.'"));
			return;
		}

		object_copy(q_ptr, o_ptr);
		q_ptr->number = 1;
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
		object_desc(o_name, q_ptr, 0);

		msg_format(_("%s(%c)を選択した。", "%s(%c) was selected."), o_name, index_to_label(item));
		msg_print("");
		value = object_value_real(q_ptr);

		if(value < 100)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("わかさぎ姫「あまり良い交換は期待できないかもしれませんね。」",
                                                            "Wakasagihime - 'You probably won't get something good in exchange.'"));
			else msg_print(_("影狼「その程度のものだとあまり期待できないわよ？」",
                            "Kagerou - 'Don't expect too much in exchange.'"));
		}
		else if(value < 1000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("わかさぎ姫「これを交換に出しますよ？」",
                                                            "Wakasagihime - 'Do you want to trade this?'"));
			else msg_print(_("影狼「これを交換に出すのね？」",
                            "Kagerou - 'Do you want to trade this?'"));
		}
		else if(value < 10000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("わかさぎ姫「これは面白い品ですね。皆さん張り切りそうです。」",
                                                            "Wakasagihime - 'Looks interesting. Everyone's enthusiastic.'"));
			else msg_print(_("影狼「中々の品ね。皆張り切るわ。」",
                            "Kagerou - 'Not bad. Everyone's enthusiastic.'"));
		}
		else if(value < 100000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("わかさぎ姫「素晴らしい品ですね。私も頑張ります！」",
                                                            "Wakasagihime - 'That's an amazing item. I won't fall behind!'"));
			else msg_print(_("影狼「これはかなりの逸品ね。皆これほどの物を出せるかしら。」",
                            "Kagerou - 'Looks very valuable. I wonder if everyone else can bring out something like that.'"));
		}
		else
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("わかさぎ姫「皆さんこんなすごい物持ってこれないかも・・」",
                                                            "Wakasagihime - 'Everyone else might not have something that amazing...'"));
			else msg_print(_("影狼「・・本当にいいの？貴方大損するかもしれないけど。」",
                            "Kagerou - '...Are you certain? You might take a big loss in this exchange.'"));
		}

		if (!get_check_strict(_("出品したアイテムは戻ってきません。よろしいですか？",
                                "The item you're trading cannot be brought back. Are you sure?"), CHECK_OKAY_CANCEL))
		{
			return;
		}

		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);

		if(!EXTRA_MODE)
		{
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("わかさぎ姫「また半日後に集まりましょう。頑張っていい物探して来ます！」",
                                                            "Wakasagihime - 'We'll get back together in half a day. I'll do my best to find something good!'"));
			else msg_print(_("影狼「それじゃ、皆に知らせて交換会するから半日程度経ったらまた来てね。」",
                            "Kagerou - 'I'll inform everyone and we'll hold an exchange meeting; come back in half a day.'"));

		}
		msg_print(NULL);
		p_ptr->barter_value = value;
		if(item > INVEN_PACK) calc_android_exp();
		handle_stuff();

		//現在ターンを記録
		//Exモードでは待ち時間がなくネットワーク未加入でも交換できるのでフラグ保持のために記録しない
		if(!EXTRA_MODE) p_ptr->grassroots = turn;
	}

	/*:::まだ半日経ってないとき*/
	if(flag_2)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("わかさぎ姫「すみません。まだ皆さんからの品物が揃ってないんです。」",
                                                        "Wakasagihime - 'Sorry, I haven't gathered the goods from everyone yet.'"));
		else msg_print(_("影狼「まだ皆からの品物が出揃っていないの。もう少し待ってて。」",
                        "Kagerou - 'I haven't received goods from everyone yet. Wait a bit longer.'"));
		return;
	}
	/*:::アイテムを生成し一覧表示してどれかを得る*/
	if(flag_3)
	{
		object_type barter_list[10];
		int i;
		char o_name[MAX_NLEN];

		for(i=0;i<10;i++)
		{
			int k_idx;
			int j;

			for(j=0;j<30;j++)
			{
				object_type forge;
				object_type *o_ptr = &forge;
				object_type *q_ptr = &barter_list[i];
				int o_value,q_value, p_value;
				int margin_o, margin_q;

				if(j==0) object_wipe(q_ptr);

				if(EXTRA_MODE)
					k_idx = get_obj_num(dun_level+randint1(20));
				else
					k_idx = get_obj_num(p_ptr->lev);

				//msg_format("%d]k_idx:%d",j,k_idx);
				if(!k_idx)
				{
					msg_print(_("ERROR:barterアイテム生成中にk_idxが無効な値",
                                "ERROR: invalid value for k_idx during barter item generation"));
					return;
				}
				object_prep(o_ptr,k_idx);
				if(EXTRA_MODE)
					apply_magic(o_ptr,dun_level+randint1(20),AM_NO_FIXED_ART);
				else
					apply_magic(o_ptr,p_ptr->lev * 3 / 2,AM_NO_FIXED_ART);

				if((o_ptr->tval == TV_BULLET || o_ptr->tval ==TV_ARROW || o_ptr->tval == TV_BOLT) && !o_ptr->name1)
					o_ptr->number = (byte)damroll(6, 7);
				else if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && !object_is_artifact(o_ptr))
				{
					o_ptr->number = 8 + (byte)damroll(2, 6);
				}

//object_desc(o_name, o_ptr, 0);

				o_value = object_value_real(o_ptr) * o_ptr->number;
				q_value = object_value_real(q_ptr) * q_ptr->number;
				/*:::わらしべ防止に20%引き*/
				//140812 20%→30%
				///mod141129 LUNATICではもっと安くする
				if(difficulty == DIFFICULTY_LUNATIC)
					p_value = p_ptr->barter_value * 5 / 10;
				else if(EXTRA_MODE)
				{
					//ネットワーク未加入では魅力で出物が変わるようにしてみる
					if(!p_ptr->grassroots)
						p_value = p_ptr->barter_value * (adj_general[p_ptr->stat_ind[A_CHR]] + 40) / 100;
					else
						p_value = p_ptr->barter_value * 8 / 10;
				}
				else
					p_value = p_ptr->barter_value * 7 / 10;
				margin_o = (o_value > p_value) ? o_value - p_value : p_value - o_value;
				margin_q = (q_value > p_value) ? q_value - p_value : p_value - q_value;
//msg_format("new:%s (%d)",o_name,o_value);
//msg_format("value_list:%d mer:%d",q_value,margin_q);
//msg_format("value_new:%d mer:%d",o_value,margin_o);
				if(j==0 || margin_o < margin_q)
				{
					object_copy(q_ptr, o_ptr);
				}
			}
		}
		cs = 0;

		if(EXTRA_MODE && !p_ptr->grassroots)
			c_put_str(TERM_WHITE,_("草の根の妖怪たちからの出物が揃ったようだ。欲しいものを選ぼう。",
                                    "The goods from Grassroots Network youkai have been collected. Pick the one you like."),4 , 6);
		else if(p_ptr->pclass == CLASS_KAGEROU)
			c_put_str(TERM_WHITE,_("わかさぎ姫「それでは交換会の開始です！まず影狼さんから選んでくださいね。」",
                                    "Wakasagihime - 'With that, let's begin exchange meeting! Kagerou, you take the first pick.'"),4 , 6);
		else
			c_put_str(TERM_WHITE,_("影狼「皆お待たせー！さあ貴方から選んで。どれがいい？」",
                                    "Kagerou - 'Sorry for the wait! Let's start with you. Which one do you like?'"),4 , 6);
		while(1)
		{
			for(i=0;i<10;i++)
			{
				//int o_value = object_value_real(&barter_list[i]) * barter_list[i].number;
				int color = TERM_WHITE;
				object_type *o_ptr = &barter_list[i];
				if(cs==i) color = TERM_YELLOW;
				object_aware(o_ptr);
				object_known(o_ptr);
				object_desc(o_name, o_ptr, 0);
				//msg_format("%d:%s",o_value,o_name);
				c_put_str(color,format("%c) %s",'a'+i,o_name),6 + i, 10);

			}
			c = inkey();
			if (c == '2' && cs < 9) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == ' ' || c == '\r' || c == '\n') c=cs+'a';

			if(c >= 'a' && c <= 'j')
			{
				object_desc(o_name, &barter_list[c-'a'], 0);
				msg_format(_("%sを受け取る。", "You will receive %s."),o_name);
				if (get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_DEFAULT_Y)) break;
			}
		}
		clear_bldg(4,18);
		item_new = inven_carry(&barter_list[c-'a']);
		autopick_alter_item(item_new, FALSE);
		handle_stuff();
		if(EXTRA_MODE)
		{
			msg_format(_("取引が終了した。妖怪たちは荷物をまとめて次のフロアへと向かった。",
                        "Transaction complete. The youkai pack their belongings and head to lower floors."));
			//建物使用済みフラグ
			ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;
			return;
		}
		else
			msg_format(_("アイテム交換が終了した。皆は交換したアイテムを持って帰っていった。",
                        "Item exchange complete. Everyone left with items they've exchanged."));

		/*:::grassrootsをデフォの1へ戻す。0だとネットワーク未加入。*/
		p_ptr->grassroots = 1;

	}
	return;

}









/*:::カード交換会*/
void grassroots_trading_cards(void)
{
	int i;
	int ref_pval=0,ref_num,ref_cost,ref_totalcost; //プレイヤーが渡したカードの情報
	char c;
	int cs;
	cptr q, s;
	cptr msg_select; //カードを選ぶときのメッセージ
	cptr msg_reject; //提示されたカードに合うカードが見つからなかったときのメッセージ
	cptr msg_finish; //交換終了時メッセージ
	char o_name[MAX_NLEN];
	int item;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int mult; //カードの価値選定時の補正
	int trade_chance, trade_num; //選定される交換カード候補数,実際に選定されたカード数
	object_type barter_list[10];
	int base_rarity; //カード選定でレアカードの出やすさ

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

//実行済みフラグ立ってる場合は終了
	if(ex_buildings_param[ex_bldg_num] == 255)
	{
		msg_print(_("取引は終了した。", "Trading is over."));
		return;
	}

	if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("今は荷物が一杯だ。", "Your inventory is full"));
		return;
	}

	//草の根ネットワークでの交換の初期化
	if(ex_bldg_idx == BLDG_EX_GRASSROOTS)
	{
		if(p_ptr->grassroots)
		{
			mult = 120;
			base_rarity = 80;
			trade_chance = 6 + randint1(4);
		}
		else
		{
			mult = 80 + adj_general[p_ptr->stat_ind[A_CHR]]/2;
			base_rarity = 20 + adj_general[p_ptr->stat_ind[A_CHR]];
			trade_chance = 3 + randint1(5);
		}
		if(p_ptr->pclass == CLASS_KAGEROU)
		{
			q = _("「どんなカードを出してくれるんですか？」", "'Which card do you want to trade?'");
			s = _("「あら、カードを持っていませんね。」", "'Oh, you don't have cards.'");
		}
		else
		{
			q = _("「さあ、貴方が出すカードは何かしら？」", "'So, which card are you going to trade?'");
			s = _("「ちょっと、貴方カード持ってないじゃない。」", "'Hold on, you don't have cards.'");
		}
		msg_select = _("草の根妖怪ネットワークの妖怪たちはカードを提示した。",
                        "The Grassroots Network youkai present their cards.");
		msg_reject = _("誰もあなたの出したカードに見合うカードを持っていなかった・・",
                        "No one had a card to match yours...");
		msg_finish = _("トレードは終了した。妖怪たちは荷物をまとめて次のフロアへ向かった。",
                        "Trade complete. The youkai pack their belongings and head to lower floors.");
	}
	else if(ex_bldg_idx == BLDG_EX_REIMU)
	{
		//パラメータ(0-100)
		int rank = ex_buildings_param[ex_bldg_num];
		//msg_format("rank:%d",rank);
		if(!rank)
		{
			msg_print(_("「トレード？何だか知らないけれど素敵なお賽銭箱はここよ。」",
                        "'Trading? No idea what you're talking about, but here's a nice donation box.'"));
			return;
		}

		mult = 70 + rank/2;
		base_rarity = 50 + rank;
		trade_chance = 3 + rank / 15;
		q = _("どのカードを出しますか？", "Which card do you want to trade?");
		s = _("カードを持っていない。", "You don't have cards.");
		msg_select = _("「カードってこれのこと？さっき懲らしめた妖怪が持っていたんだけど」",
                        "'So, that's a card? The youkai I've disciplined earlier had those.'");
		msg_reject = _("「うーん、そんな凄そうなのは持ってないわ。」",
                        "'Well, I don't have anything that amazing.'");
		msg_finish = _("「これで取引終了ね。ところで素敵なお賽銭箱はそこよ。」",
                        "'It's a deal, then. By the way, here's a nice donation box.'");

	}
	else if(ex_bldg_idx == BLDG_EX_MARISA)
	{

		mult = 100;
		base_rarity = 70;
		trade_chance = 5 ;
		q = _("「何のカードを出すんだ？宝石や魔法書でもいいぜ。」",
                "'What card do you want to trade? Gems or magic books are fine too.'");
		s = _("交換できるものを持っていない。", "You don't have anything to exchange.");
		msg_select = _("魔理沙はカードを提示した。", "Marisa shows you cards.");
		msg_reject = _("「悪いな。それに見合うカードを持ってないぜ。」",
                        "'Sorry, I don't have any cards to match that.'");
		msg_finish = _("「さて、またどこかで商品を仕入れてこなきゃな。」",
                        "'Well, I'll have to obtain some products again.'");

	}
	else if (ex_bldg_idx == BLDG_EX_YUMA)
	{

		mult = 80;
		base_rarity = 60;
		trade_chance = 4;
		q = _("「酒か食い物をよこせ。」", "'Give me sake or food.'");
		s = _("交換できるものを持っていない。", "You don't have anything to exchange.");
		//msg_select = ""; あとで設定する
		msg_reject = _("「悪いな。それに見合うカードを持ってないぜ。」",
                        "'Sorry, I don't have any cards to match that.'");
		msg_finish = _("「次はもっと旨いものを持ってこい。」",
                        "'Bring something better next time.'");

	}
	else
	{
		msg_print(_("ERROR:この建物でカード交換するときの基本パラメータが設定されていない",
                    "ERROR: Base parameter for card trading not set for this building."));
		return;
	}

	//カード選ぶ
	if(ex_bldg_idx == BLDG_EX_MARISA)
		item_tester_hook = marisa_will_trade;
	else if(ex_bldg_idx == BLDG_EX_YUMA)
		item_tester_hook = yuma_will_trade;
	else
		item_tester_tval = TV_ITEMCARD;

	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];
	if (o_ptr->number > 1)
	{
		if(o_ptr->tval == TV_ITEMCARD) ref_num = get_quantity(_("何枚ですか？:", "How many?:"), o_ptr->number);
		else ref_num = get_quantity(NULL, o_ptr->number);
		if (ref_num <= 0) return;
	}
	else ref_num = 1;

	//カード選定前の初期化など
	if(o_ptr->tval == TV_ITEMCARD)
	{
		ref_pval = o_ptr->pval;
		ref_cost = support_item_list[ref_pval].cost;

		ref_totalcost = ref_cost * ref_num * mult / 100;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA && o_ptr->name1 == ART_IBUKI) //v2.0.13 移動魔法店で伊吹瓢を渡したとき
	{
		ref_cost = 300;
		ref_totalcost = 300;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA) //移動魔法店でそれ以外のアイテムを渡したとき
	{
		ref_pval = -1;

		for(i=0;marisa_wants_table[i].tval;i++)
		{
			if(o_ptr->tval == marisa_wants_table[i].tval && o_ptr->sval == marisa_wants_table[i].sval)
			{
				if(o_ptr->tval < TV_BOOK_END && o_ptr->sval == 2)
					ref_cost = 20;
				else if(o_ptr->tval < TV_BOOK_END && o_ptr->sval == 3)
				{
					ref_cost = 50;
					base_rarity += 20;
				}
				else
				{
					ref_cost = marisa_wants_table[i].maripo / 10;
					if(ref_cost > 50) base_rarity += 20;
				}

				if(ref_cost > 30)
				{

					//ref_cost = 30 + (ref_cost - 30) / 2;
					trade_chance += ref_cost / 30;
					if(trade_chance > 10) trade_chance = 10;
				}

				ref_totalcost = ref_cost * ref_num * mult / 100;

				break;
			}
		}


	}
	else if (ex_bldg_idx == BLDG_EX_YUMA)
	{

		//巨大な卵
		if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG)
		{
			msg_select = _("クックック...こいつは珍しいものを持ってきたな。",
                            "Hehehe... You've brought in something rare.");
			ref_cost = 300;
		}
		//伊吹瓢
		else if (o_ptr->name1 == ART_IBUKI)
		{
			msg_select = _("ほう、いくらでも酒が出るのか？この私に打って付けじゃないか。",
                            "Oh, it produces as much sake as you want? It's perfect for me!");
			ref_cost = 250;
		}
		//道寿の壺
		else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
		{
			msg_select = _("ほう、いくらでも油が出るのか？この私に打って付けじゃないか。",
                            "Oh, it produces as much oil as you want? It's perfect for me!");
			ref_cost = 200;
		}
		else if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI)
		{
			msg_select = _("これは霊薬の材料か。悪くない。",
                            "That's a material for elixirs? Not bad.");
			ref_cost = 50;
		}
		else if (o_ptr->tval == TV_FOOD)
		{

			switch (o_ptr->sval)
			{
			case SV_FOOD_SLIMEMOLD:
			case SV_FOOD_SENTAN:
			case SV_FOOD_MAGIC_WATERMELON:
				msg_select = _("まあ腹の足しにはなるか。",
                                "I guess that'll do.");
				ref_cost = 5;
				break;

			case SV_FOOD_EEL:
			case SV_FOOD_TENTAN:
			case SV_FOOD_STRANGE_BEAN:
				msg_select = _("中々滋養がありそうじゃないか。",
                                "That looks quite nourishing!");
				ref_cost = 20;
				break;

			default:
				msg_select = _("つまらんな。他に何も持ってないのか？",
                                "Meh. Do you have anything else?");
				ref_cost = 1;
				break;
			}
		}
		else if (o_ptr->tval == TV_MUSHROOM)
		{

			switch (o_ptr->sval)
			{
			case SV_MUSHROOM_MANA:
			case SV_MUSHROOM_PUFFBALL:
			case SV_MUSHROOM_MON_L_BLUE:
				msg_select = _("まあ腹の足しにはなるか。",
                                "I guess that'll do.");
				ref_cost = 5;
				break;

				//実際EXTRAでは手に入らないがそのうち何かで手に入るようになるかもしれんので設定しておく
			case SV_MUSHROOM_MON_SUPER:
				msg_select = _("こいつは魔法の森の主か？なぜこんなところに？",
                                "Isn't that the Lord of Forest of Magic? What is it doing here?");
				ref_cost = 300;
				break;

			default:
				msg_select = _("腹の足しにもならんな。", "That doesn't satisfy me.");
				ref_cost = 1;
				break;
			}
		}
		else if (o_ptr->tval == TV_SWEETS)
		{
			switch (o_ptr->sval)
			{
			case SV_SWEET_POTATO:
				msg_select = _("美味そうな焼き芋だな。早くよこせ。",
                                "That's a delicious sweet potato. Hand it over!");
				ref_cost = 10;
				break;

			case SV_SWEETS_MIRACLE_FRUIT:
				msg_select = _("中々滋養がありそうじゃないか。",
                                "That looks quite nourishing!");
				ref_cost = 20;
				break;

			case SV_SWEETS_PEACH:
				msg_select = _("それは仙桃じゃないか。懐かしいな。",
                                "Isn't that a hermit peach? I miss those.");
				ref_cost = 50;
				break;

			default:
				msg_select = _("つまらんな。他に何も持ってないのか？",
                                "Meh. Do you have anything else?");
				ref_cost = 2;
				break;
			}
		}
		else if (o_ptr->tval == TV_FLASK)
		{
			switch (o_ptr->sval)
			{
				//ロケット燃料が一番コスパがいいが使い道と重量を考えれば大量に買って持ち歩くのは現実的ではないだろう
			case SV_FLASK_ROCKET:
			case SV_FLASK_ENERGY_RED:
			case SV_FLASK_ENERGY_BLUE:
			case SV_FLASK_ENERGY_WHITE:
			case SV_FLASK_ENERGY_BLACK:
				msg_select = _("まあ腹の足しにはなるか。",
                                "I guess that'll do.");
				ref_cost = 5;
				break;
			case SV_FLASK_ENERGY_PARTICLE:
				msg_select = _("何だそれは？見たこともない力を感じるぞ。",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 50;
				break;
			case SV_FLASK_ENERGY_WAVEMOTION:
				msg_select = _("何だそれは？見たこともない力を感じるぞ。",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 100;
				break;
			case SV_FLASK_ENERGY_HYPERSPACE:
				msg_select = _("何だそれは？見たこともない力を感じるぞ。",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 200;
				break;

			default:
				msg_select = _("つまらんな。他に何も持ってないのか？",
                                "Meh. Do you have anything else?");
				ref_cost = 1;
				break;
			}

		}
		else if (o_ptr->tval == TV_ALCOHOL)
		{
			switch (o_ptr->sval)
			{



			case SV_ALCOHOL_ORC:
			case SV_ALCOHOL_REISYU:
				msg_select = _("なんか薬臭い酒だな。", "What's that medicinal liquor?");
				ref_cost = 10;
				break;

			case SV_ALCOHOL_STRONGWINE:
			case SV_ALCOHOL_MAGGOT_BEER:
			case SV_ALCOHOL_1420:
			case SV_ALCOHOL_SCARLET:
			case SV_ALCOHOL_MAMUSHI:
				msg_select = _("なかなか悪くない酒だ。もっと持ってこい。", "That wasn't bad. Bring some more.");
				ref_cost = 30;
				break;

			case SV_ALCOHOL_MARISA:
				msg_select = _("何だ？何をやったらただの安酒がこんな呪われた代物になるんだ？",
                                "What? Just what did you do to turn cheap sake into a cursed item like this?");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_90:
			case SV_ALCOHOL_SYUTYUU:
			case SV_ALCOHOL_TRUE_ONIKOROSHI:
				msg_select = _("ほう、こいつは美味そうだ。", "Ohh, that's delicious!");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KOKO:
			case SV_ALCOHOL_EIRIN:
				msg_select = _("随分古そうだが妙に味気ない酒だな。",
                                "Looks very old, but it's strangely tasteless.");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KUSHINADA:
				msg_select = _("クックック...この程度の毒酒でこの私が潰れると思ったか？",
                                "Hehehe... Did you think you could knock me out with that much poisoned sake?");
				ref_cost = 100;
				break;

			case SV_ALCOHOL_GOLDEN_MEAD:
				msg_select = _("私の知らない古く強い力を感じるな。なかなか興味深いぞ。",
                                "I'm feeling some unknown ancient power. That's very interesting.");
				ref_cost = 100;
				break;

			case SV_ALCOHOL_NECTAR:
				msg_select = _("ほう、異国の神酒か！こいつは栄養満点だ！",
                                "Ohh, an exotic divine liquor! It's so fullfilling!");
				ref_cost = 200;
				break;

			default:
				msg_select = _("もっと強い酒はないのか？", "Don't you have anything stronger?");
				ref_cost = 5;
				break;
			}
		}
		else
		{
			msg_print(_("ERROR:このアイテムを尤魔に渡したときの評価値が設定されていない",
                        "ERROR: Undefined value for this item given to Yuuma"));
			return;
		}

		if (ref_cost > 30)
		{
			//ref_cost = 30 + (ref_cost - 30) / 2;
			trade_chance += ref_cost / 30;
			if (trade_chance > 10) trade_chance = 10;
			base_rarity += ref_cost / 5;
		}

		//食料生成99個とかで高級なカードを入手することを防ぐためにアイテムの数が多すぎるとtotalcostを減らしておく

		if (ref_num == 1)
			ref_totalcost = ref_cost * mult / 100;
		else if (ref_num <= 10)
			ref_totalcost = ref_cost * (1 + (ref_num - 1) * 2 / 3) * mult / 100;
		else if (ref_num <= 30)
			ref_totalcost = ref_cost * (7 + (ref_num - 10) / 2) * mult / 100;
		else if (ref_num <= 70)
			ref_totalcost = ref_cost * (17 + (ref_num - 30) / 4) * mult / 100;
		else
			ref_totalcost = ref_cost * (27 + (ref_num - 70) / 8) * mult / 100;



	}
	else
	{
		msg_print(_("ERROR:この建物でカード交換するときのコスト計算が設定されていない",
                    "ERROR: Undefined cost calculation for card trading in this building"));
		return;
	}

	for(i=0;i<10;i++) object_wipe(&barter_list[i]);
	trade_num = 0;

	//カード生成数繰り返す
	for(i=0;i<trade_chance && i<10;i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//カードリスト全部チェック
		for(j=0;support_item_list[j].rarity;j++)
		{
			int k;
			bool flag_ok = TRUE;
			//＠が提示したカードと同じのは弾く
			if(j == ref_pval) continue;
			//すでに選定されたカードと同じのは弾く
			for(k=0;k<trade_num;k++) if(j == barter_list[k].pval) flag_ok = FALSE;
			if(!flag_ok) continue;
			//一枚のコストが＠の提示したカード全部のコストより高いのは弾く(コスト1のカードは弾かない)
			if(support_item_list[j].cost > 1 && support_item_list[j].cost > ref_totalcost) continue;

			//50枚出しても＠の提示したカード全部のコストに足りないのは弾く
			if( support_item_list[j].cost * 50 < ref_totalcost) continue;

			//最低生成階が今いる階より深いカードは高確率で弾く
			if(dun_level < support_item_list[j].min_lev && randint1(support_item_list[j].min_lev - dun_level) < 5 && one_in_(2)) continue;
			//最高生成階が今いる階より浅いカードは中確率で弾く
			if(dun_level > support_item_list[j].max_lev && randint1(dun_level - support_item_list[j].max_lev) < 10 ) continue;
			//珍しいカードは確率で弾く
			if(randint1(support_item_list[j].rarity) > base_rarity) continue;

			//ここまで通ったカードからランダムに選ぶ
			cnt++;
			if(!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//選択されたカードがある場合枚数を決めてbarter_listに登録
		if(cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr,lookup_kind(TV_ITEMCARD,SV_ITEMCARD));
			tmp_o_ptr->pval = tmp_pv;

			//基本枚数　ちょっと乱数適用
			tmp_num = (ref_totalcost*2/3 + randint1(ref_totalcost*2/3) + 1) / support_item_list[tmp_pv].cost;
			//通常1枚しか出ないカードは数が増えにくい
			if(support_item_list[tmp_pv].prod_num == 1)
			{
				if(tmp_num>10) tmp_num -= (tmp_num-9) / 2;
				tmp_num /= 2;
			}
			//あまり沢山出るカードは減らす
			if(tmp_num > 40) tmp_num = 40 + randint1(tmp_num-40);
			if(tmp_num > 30) tmp_num = 30 + randint1(tmp_num-30);
			if(tmp_num > 20) tmp_num = 20 + randint1(tmp_num-20);
			if(tmp_num > 10) tmp_num = 10 + randint1(tmp_num-10);

			if(tmp_num > 99) tmp_num = 99;
			else if(tmp_num < 1) tmp_num = 1;
			tmp_o_ptr->number = tmp_num;

			trade_num++;
		}
	}

	//ひとつも選定されなかった場合終了
	if(!trade_num)
	{
		c_put_str(TERM_WHITE,msg_reject,4 , 6);
		inkey();
		return;
	}

	//カードを表示し選ぶ
	cs = 0;
	while(1)
	{
		c_put_str(TERM_WHITE,msg_select,4 , 6);
		c_put_str(TERM_WHITE,_("(トレードをやめる：ESC)", "(Stop trading: ESC)"),5 , 6);
		for(i=0;i<trade_num;i++)
		{
			int color = TERM_WHITE;
			object_type *tmp_o_ptr = &barter_list[i];
			if(cs==i) color = TERM_YELLOW;
			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			object_desc(o_name, tmp_o_ptr, 0);
			//msg_format("%d:%s",o_value,o_name);
			c_put_str(color,format("%c) %s",'a'+i,o_name),6 + i, 10);
		}
		c = inkey();
		if(c == ESCAPE)
		{
			if (get_check_strict(_("このフロアではもうトレードできません。よろしいですか？",
                                    "You won't be able to trade on this floor anymore. Are you sure?"), CHECK_OKAY_CANCEL)) break;
		}
		else if (c == '2' && cs < (trade_num-1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c=cs+'a';
		else if(c >= 'a' && c <= 'a'+trade_num-1)
		{
			object_desc(o_name, &barter_list[c-'a'], 0);
			msg_format(_("%sを受け取る。", "You will receive %s."),o_name);
			if (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

			//選択したら交換処理
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c-'a']);
			msg_format(_("%sを受け取った。", "You have received %s."),o_name);

			break;
		}
	}

	clear_bldg(4,18);
	c_put_str(TERM_WHITE,msg_finish,4 , 6);
	inkey();
	clear_bldg(4,18);

	//実行済みフラグ立てる
	ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}




/*:::矢玉エゴ付加用素材テーブル*/
const struct guild_arts_type archer_guild_table[] =
{
	{TV_MUSHROOM,SV_MUSHROOM_MON_RED,EGO_ARROW_FIRE,5,_("火炎の矢", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_RUBY,EGO_ARROW_FIRE,99,_("火炎の矢", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_GARNET ,EGO_ARROW_FIRE,30,_("火炎の矢", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_HEMATITE,EGO_ARROW_FIRE,20,_("火炎の矢", "Flame Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_BLUE,EGO_ARROW_ELEC,5,_("電撃の矢", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_SAPPHIRE,EGO_ARROW_ELEC,99,_("電撃の矢", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_AMETHYST ,EGO_ARROW_ELEC,30,_("電撃の矢", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_LAPISLAZULI,EGO_ARROW_ELEC,20,_("電撃の矢", "Lightning Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_WHITE, EGO_ARROW_COLD,5,_("冷気の矢", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_EMERALD , EGO_ARROW_COLD,99,_("冷気の矢", "Frost Arrows")},
	{TV_MATERIAL, SV_MATERIAL_OPAL , EGO_ARROW_COLD,30,_("冷気の矢", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_AQUAMARINE, EGO_ARROW_COLD,20,_("冷気の矢", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_ICESCALE, EGO_ARROW_COLD,50,_("冷気の矢", "Frost Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_GREEN, EGO_ARROW_POIS,5,_("毒の矢", "Poison Arrows")},
	{TV_MATERIAL,SV_MATERIAL_GELSEMIUM , EGO_ARROW_POIS,99,_("毒の矢", "Poison Arrows")},
	{TV_MATERIAL,SV_MATERIAL_ARSENIC, EGO_ARROW_POIS,30,_("毒の矢", "Poison Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_MERCURY, EGO_ARROW_POIS,50,_("毒の矢", "Poison Arrows")},


	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER  , EGO_ARROW_HOLY,99,_("聖なる矢", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_DIAMOND, EGO_ARROW_HOLY,50,_("聖なる矢", "Holy Arrows")},
	{TV_MATERIAL,SV_MATERIAL_MITHRIL , EGO_ARROW_HOLY,5,_("聖なる矢", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_HOPE_FRAGMENT, EGO_ARROW_HOLY,50,_("聖なる矢", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_BROKEN_NEEDLE, EGO_ARROW_HOLY,10,_("聖なる矢", "Holy Arrows")},

	{TV_MATERIAL,SV_MATERIAL_TOPAZ , EGO_ARROW_HUNTER,30,_("狩人の矢", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_PERIDOT , EGO_ARROW_HUNTER,30,_("狩人の矢", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_MOONSTONE , EGO_ARROW_HUNTER,30,_("狩人の矢", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI , EGO_ARROW_HUNTER,50,_("狩人の矢", "Hunter Arrows")},

	{TV_MATERIAL,SV_MATERIAL_HIKARIGOKE , EGO_ARROW_DRAGONSLAY,5,_("竜殺しの矢", "Dragonslayer Arrows")},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE , EGO_ARROW_DRAGONSLAY,20,_("竜殺しの矢", "Dragonslayer Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU , EGO_ARROW_DRAGONSLAY,30,_("竜殺しの矢", "Dragonslayer Arrows") },
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL , EGO_ARROW_DRAGONSLAY,50,_("竜殺しの矢", "Dragonslayer Arrows")},

	//v1.1.56 追加
	{ TV_MUSHROOM,SV_MUSHROOM_MON_BLACK, EGO_ARROW_ACID,5,_("腐蝕の矢", "Corrosion Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT , EGO_ARROW_ACID,50,_("腐蝕の矢", "Corrosion Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_ISHIZAKURA, EGO_ARROW_ACID,30,_("腐蝕の矢", "Corrosion Arrows")},

	{ TV_MATERIAL, SV_MATERIAL_IZANAGIOBJECT, EGO_ARROW_SLAY_DEITY,50,_("天探女の矢", "Arrows of Ame-no-Sugume")},

	{ TV_MATERIAL, SV_MATERIAL_MYSTERIUM, EGO_ARROW_AMANOZYAKU,99,_("天邪鬼の矢", "Arrows of Amanojaku")},


	{0,0,0,0,_("終端用ダミー", "dummy terminator")}

};

/*:::矢玉強化用素材テーブル*/
const struct guild_arts_type archer_guild_table2[] =
{
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON ,3,99," "},
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE,7,99," "},
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE,10,99," "},
	{TV_MATERIAL, SV_MATERIAL_METEORICIRON ,15,99," "},
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,25,99," "},
	{0,0,0,0,_("終端用ダミー", "dummy terminator")}
};



/*:::アーチャーギルドにて矢玉強化の素材アイテムを判定*/
static bool archer_guild_hook2(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table2[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table2[i].tval && o_ptr->sval == archer_guild_table2[i].sval) return TRUE;
	}
	return FALSE;
}

/*:::アーチャーギルドにてエゴ付加の素材アイテムを判定*/
static bool archer_guild_hook(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table[i].tval && o_ptr->sval == archer_guild_table[i].sval) return TRUE;
	}
	return FALSE;
}



/*:::素材を使って矢を強化する*/
static void archer_guild_arts2(void)
{
	int i, item, item_m;
	int thtd=0;
	object_type *o_ptr;
	object_type *om_ptr;
	cptr        q, s;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print(_("あなたは工房に入れてもらえなかった。",
                    "You weren't let inside the workshop."));
		return;
	}

	item_tester_hook = item_tester_hook_ammo;
	q = _("どの矢を強化しますか？", "Improve which arrows?");
	s = _("あなたは強化できる矢を持っていない。", "You don't have arrows to improve.");
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook2;
	q = _("どの素材を使って強化しますか？", "Use which material to improve?");
	s = _("しかしあなたは強化に使う素材を持っていなかった。",
            "You don't have suitable materials to use.");
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table2[i].tval;i++)
		if(archer_guild_table2[i].tval == om_ptr->tval && archer_guild_table2[i].sval == om_ptr->sval)
		{ thtd=archer_guild_table2[i].type; break;}

	if(!thtd) {msg_format(_("ERROR:archer_guild_arts2()で素材が適切に登録されていない？",
                            "ERROR: material not properly defined in archer_guild_arts2()?")); return;}

	object_desc(o_name, o_ptr, 0);

	if(o_ptr->to_d >= thtd && o_ptr->to_h >= thtd)
	{
		msg_format(_("その素材では%sを強化できない。", "You cannot improve %s with this material."),o_name);
		return;
	}

	msg_format(_("%sを+%dまで強化できる。", "%s can be enchanted up to +%d."),o_name,thtd);

	if(!get_check(_("強化しますか？", "Enchant?"))) return;

	if(o_ptr->to_d < thtd) o_ptr->to_d = thtd;
	if(o_ptr->to_h < thtd) o_ptr->to_h = thtd;
	msg_format(_("強化が完了した。", "Complete."));

	if (object_is_cursed(o_ptr))
	{
		msg_format(_("矢の呪いが消えた。", "Your arrows are no longer cursed."));
		o_ptr->curse_flags = 0L;
	}

	inven_item_increase(item_m, -1);
	inven_item_describe(item_m);
	inven_item_optimize(item_m);

	p_ptr->window |= (PW_INVEN);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return;
}


static bool item_tester_hook_ammo_make_ego(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ARROW || o_ptr->tval == TV_BOLT)
	{
		if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr)) return (TRUE);
	}

	return (FALSE);
}



/*:::素材を使って矢に属性付与する*/
static void archer_guild_arts(void)
{
	int i, item, item_m,slot_new;
	object_type *o_ptr;
	object_type *om_ptr;
	object_type forge;
	object_type *new_o_ptr = &forge;

	cptr        q, s;
	char o_name[MAX_NLEN];
	int num;

	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print(_("あなたは工房に入れてもらえなかった。",
                    "You weren't let inside the workshop."));
		return;
	}

	item_tester_hook = item_tester_hook_ammo_make_ego;
	q = _("どの矢に属性付与しますか？", "Brand which arrows?");
	s = _("あなたは属性付与できる矢を持っていない。", "You don't have arrows to brand.");
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook;
	q = _("どの素材を使って強化しますか？", "Use which material to improve?");
	s = _("しかしあなたは強化に使う素材を持っていなかった。",
            "You don't have suitable materials to use.");
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table[i].tval;i++)
	{
		if(archer_guild_table[i].tval == om_ptr->tval && archer_guild_table[i].sval == om_ptr->sval) break;
	}
	if(!archer_guild_table[i].tval) {msg_format(_("ERROR:archer_guild_arts()で素材が適切に登録されていない？",
                                                "ERROR: material not properly registered in archer_guild_arts()?")); return;}


	object_desc(o_name, o_ptr, 0);

	num = archer_guild_table[i].num;

	if(inventory[INVEN_PACK-1].k_idx && om_ptr->number != 1 && o_ptr->number > num)
	{
		msg_print(_("属性付与した矢を持てる余裕がないようだ。", "You don't have free space for branded arrows."));
		return;
	}


	if(num < o_ptr->number)
#ifdef JP
		msg_format("%sのうち%d本を%sにすることができる。",o_name,num, archer_guild_table[i].name);
#else
        msg_format("You can make %d %s from %s.",num, archer_guild_table[i].name, o_name);
#endif
	else
		msg_format(_("%sを%sにすることができる。",
                    "You can turn %s into %s."),o_name, archer_guild_table[i].name);

	if(!get_check(_("属性付与しますか？", "Perform branding?"))) return;

	object_prep(new_o_ptr,o_ptr->k_idx);

	new_o_ptr->number = MIN(num,o_ptr->number);
	new_o_ptr->to_d = (o_ptr->to_d<0)?0:o_ptr->to_d;
	new_o_ptr->to_h = (o_ptr->to_h<0)?0:o_ptr->to_h;
	new_o_ptr->name2 = archer_guild_table[i].type;
	object_aware(new_o_ptr);
	object_known(new_o_ptr);

	if (object_is_cursed(new_o_ptr))
	{
		msg_format(_("矢の呪いが消えた。", "Your arrows are no longer cursed."));
		new_o_ptr->curse_flags = 0L;
	}

	msg_format(_("属性付与が完了した。",
              "Branding complete."));

	/*:::最初のアイテム減少処理でインベントリがずれて次のアイテム減少処理対象が変わる恐れがあるので苦肉の策で分岐。もっとまともな方法がありそうなものだが思いつかなかった。*/
	if(item_m > item)
	{
		inven_item_increase(item_m, -1);
		inven_item_describe(item_m);
		inven_item_optimize(item_m);
		inven_item_increase(item, -num);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		inven_item_increase(item, -num);
		inven_item_describe(item);
		inven_item_optimize(item);
		inven_item_increase(item_m, -1);
		inven_item_describe(item_m);
		inven_item_optimize(item_m);
	}
	slot_new = inven_carry(new_o_ptr);
	autopick_alter_item(slot_new, FALSE);

	p_ptr->window |= (PW_INVEN);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return;
}






/*:::アーチャーギルドの説明を聞く*/
static void archer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print(_("あなたは射手ではないのでここでできることはないようだ。",
                    "You can't do anything here since you're not an archer."));
		return;
	}
	display_rumor_new(20);
}

/*:::外来人専用　博麗神社から外界へ帰還する*/
static bool escape_from_gensoukyou(void)
{
	int i;
	if(p_ptr->pclass != CLASS_OUTSIDER)
	{
		msg_print(_("あなたは幻想郷出身だ。", "You are a resident of Gensoukyou."));
		return FALSE;
	}

	if( !get_check_strict(_("外界へ帰りますか? ", "Return to the outside world?"), CHECK_NO_HISTORY)) return FALSE;
	if (!p_ptr->noscore)
	{
	prt(_("確認のため '@' を押して下さい。", "Press '@' to confirm."), 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return FALSE;

		if(p_ptr->total_winner) finish_the_game = TRUE;
	}

	/* Initialize "last message" buffer */
	if (p_ptr->last_message) string_free(p_ptr->last_message);
	p_ptr->last_message = NULL;

	/*:::last_words:遺言オプション*/
	if (last_words)
	{
		char buf[1024] = "";
		cptr tmp_msg;

		if(p_ptr->total_winner)
			tmp_msg = _("*勝利*メッセージ: ", "***WINNER*** message: ");
		else
			tmp_msg = _("何か一言: ", "Say something: ");

		do
		{
			while (!get_string(tmp_msg, buf, sizeof buf)) ;
		}
		while (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_NO_HISTORY));

		if (buf[0])
		{
			p_ptr->last_message = string_make(buf);
			msg_print(p_ptr->last_message);
		}
	}
	p_ptr->playing = FALSE;
	p_ptr->is_dead = TRUE;
	p_ptr->leaving = TRUE;

	if(p_ptr->total_winner) strcpy(p_ptr->died_from, "triumph");
	else strcpy(p_ptr->died_from, "escape");

	return TRUE;
}


//v1.1.92
//回復施設で近くの配下も回復することにする
//ratio:HPの回復率
static void bldg_heal_all_pets(int ratio)
{
	int i;

	//フロアのモンスターをループ
	for (i = 1; i < m_max; i++)
	{
		int heal;
		bool flag_done = FALSE;

		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;
		char m_name[240];
		r_ptr = &r_info[m_ptr->r_idx];
		if (!m_ptr->r_idx) continue;
		if (!is_pet(m_ptr)) continue;
		if (m_ptr->cdis > 3) continue;

		monster_desc(m_name, m_ptr, 0);
		r_ptr = &r_info[m_ptr->r_idx];
		heal = m_ptr->max_maxhp * ratio / 100;
		if (heal < 300) heal = 300;

		if (m_ptr->hp < m_ptr->max_maxhp)
		{
			int tmp;
			flag_done = TRUE;

			tmp = m_ptr->hp + heal;
			if (tmp > m_ptr->max_maxhp) tmp = m_ptr->max_maxhp;
			if (tmp > m_ptr->maxhp) m_ptr->maxhp = tmp;
			m_ptr->hp = tmp;

		}
		if (MON_STUNNED(m_ptr))
		{
			flag_done = TRUE;
			set_monster_stunned(i, 0);
		}
		if (MON_CONFUSED(m_ptr))
		{
			flag_done = TRUE;
			set_monster_confused(i, 0);

		}
		if (MON_SLOW(m_ptr))
		{
			flag_done = TRUE;
			set_monster_slow(i, 0);

		}
		if (MON_MONFEAR(m_ptr))
		{
			flag_done = TRUE;
			set_monster_monfear(i, 0);

		}

		if(flag_done) msg_format(_("%sは回復した。", "%s is healed."), m_name);
	}



	return;

}






/*:::博麗神社　解呪*/
/*:::*解呪*とは違い永遠の呪い付き装備の解呪は全く行われないが処理面倒なので放置*/
//v1.1.32 守矢神社も追加したのでメッセージ追加
bool bldg_remove_curse( void)
{
	int i;
	u32b flag_curse=0L;
	u32b flag_perma=0L;

	//1:霊夢 2:守矢 3:座敷わらし
	int msg_mode = 0;

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

	if (p_ptr->town_num == TOWN_HAKUREI)
		msg_mode = 1;
	else if (p_ptr->town_num == TOWN_MORIYA)
		msg_mode = 2;
	else if(EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_REIMU)
		msg_mode = 1;
	else if (EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_MORIYA)
		msg_mode = 2;
	else if(EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_ZASHIKI)
		msg_mode = 3;

	for(i=INVEN_RARM;i<INVEN_TOTAL;i++)
	{
		object_type *o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		if (!object_is_cursed(o_ptr)) continue;
		flag_curse |= 1L<<(i-INVEN_RARM);
		if(o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
			flag_perma  |= 1L<<(i-INVEN_RARM);
		}
	}
	if(!flag_curse)
	{
		msg_print(_("あなたは呪われていない。", "You're not cursed."));
		return FALSE;
	}
	else if(flag_curse & ~flag_perma)
	{
		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print(_("分社の戸が開き諏訪子が出てきて呪いを吸い取ってくれた。",
                            "Suwako comes out of the branch shrine and absorbs the curse."));
			else
				msg_print(_("霊夢はあなたの目の前に手をかざして妙な手つきで指を動かした。",
                            "Reimu holds her hand in front of you, making strange movements."));
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print(_("諏訪子がニヤニヤしながら出てきて呪いを吸い取った。",
                            "Suwako comes out and absorbs the curse with a grin."));
			else
				msg_print(_("神奈子が出てきて「ハァーー！！」と気合を放った！",
                            "Kanako comes out and yells out 'Haaaah!!'"));
			break;

		case 3:
			msg_print(_("座敷わらしたちが手際よく解呪してくれた。",
                        "The zashiki-warashi skillfully remove your curses."));
			break;

		default:
			msg_print(_("ERROR:この建物の解呪成功メッセージが設定されていない",
                        "ERROR: Undefined messages for removing curses in this building"));
		}

		if(remove_all_curse())
		{
			msg_print(_("装備品の呪いが解けた！", "Your equipment is uncursed!"));

		}
		return TRUE;
	}
	else
	{

		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print(_("分社の戸が開き諏訪子が出てきて、何も言わずに引っ込んだ。",
                            "Suwako comes out of the branch shrine and goes back in without saying a word."));
			else
				msg_print(_("霊夢「ごめんなさい。その呪いは私の手にも負えないわ。」",
                            "Reimu - 'I'm sorry, but removing this curse is beyond my abilities.'"));
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print(_("諏訪子「ありゃまぁ、ひどい祟りに見初められたもんだねえ。まあ仲良く付き合うんだね。」",
                            "Suwako - 'Wow, that's the first time I'm seeing a curse like this. Well, let's get along.'"));
			else
				msg_print(_("神奈子「済まぬがその呪いは神の手にも負えぬ。『凡庸の巻物』を探せ。」",
                            "Kanako - 'I'm sorry, but this curse is beyond my powers. Look for a Scroll of Mundanity."));
			break;
		case 3:
			msg_print(_("座敷わらしたちがヒソヒソと相談しあっている。我が家からの引っ越しを検討しているようだ...",
                        "The zashiki-warashi talk with each other in whispers. Looks like they're considering leaving your house..."));
			break;
		default:
			msg_print(_("ERROR:この建物の解呪失敗メッセージが設定されていない",
                        "ERROR: Undefined message for failing to remove curses in this building"));
		}

		return FALSE;
	}


}


/*エンジニアギルド：機械を作る/消耗品を調達する*/
static void bact_engineer_guild(bool make_machine)
{

	int cs=0,page=0,choice=-1;
	int i;
	char c;
	object_type forge;
	object_type *q_ptr = &forge;
	int slot_new;
	char o_name[MAX_NLEN];
	int price;
	int supply_price_list[MAX_MACHINE_SUPPLY];

	int power = p_ptr->lev + p_ptr->stat_ind[A_INT] + p_ptr->stat_ind[A_DEX] + 6;

	object_wipe(q_ptr);
	building_prt_gold();

	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI)
	{
		msg_print(_("あなたはエンジニアではないので奥に入れてもらえなかった。",
                    "You weren't let inside since you're not an engineer."));
		return;
	}

	if(make_machine && inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("荷物が一杯だ。減らしてから来よう。",
                    "Your inventory is full. Come back when you've freed up some space."));
		return;
	}
	//価格変動を考慮した消耗品値段テーブルを作る
	for(i=0;i<MAX_MACHINE_SUPPLY;i++)
	{
		object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[i][0], engineer_guild_supply_table[i][1]));
		supply_price_list[i] = k_info[q_ptr->k_idx].cost;
		if(supply_price_list[i] > 100)
		{
			int mod = p_ptr->magic_num1[i];
			if(p_ptr->town_num == TOWN_KAPPA) mod = MAX(-50,mod-10);
			supply_price_list[i] = supply_price_list[i] / 100 * (100+mod);
		}
	}

	while(1)
	{

		clear_bldg(5, 18);
		if(make_machine)
		{
			c_put_str(TERM_WHITE,_("「何を作るの？」", "'What do you want to make?'"),5 , 6);
			c_put_str(TERM_WHITE,_("(a～j:品目を選択 /x:説明を見る / n,スペース:次のページへ / ESC:キャンセル)",
                                    "(a-j: Pick item / x: Explanation / n, Space: Next page / ESC: Cancel"),6 , 6);
		}
		else
		{
			 c_put_str(TERM_WHITE,_("「どれをご注文？」", "'What do you want to order?'"),5 , 6);
			c_put_str(TERM_WHITE,_("(a～j:品目を選択 / n,スペース:次のページへ / ESC:キャンセル)",
                                    "(a-j: Pick item / n, Space: Next page / ESC: Cancel"),6 , 6);
		}
		for(i=0;i<10;i++)
		{

			int color;
			int table_num = page * 10 + i;
			if(make_machine)
			{
				object_prep(q_ptr, lookup_kind(TV_MACHINE, table_num));

				if(cs==i) color = TERM_YELLOW;
				else if(p_ptr->au < k_info[q_ptr->k_idx].cost) color = TERM_L_DARK;
				else color = TERM_WHITE;
				object_desc(o_name, q_ptr, 0);
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					c_put_str(color,format(_("%c) [ 設備不足 ] ", "%c) [ not in this workshop ]"),'a'+i),7 + i, 10);
				}
				else if(machine_table[table_num].difficulty > power)
				{
					c_put_str(color,format(_("%c) [ 作成不可 ] ", "%c) [ cannot create ]"),'a'+i),7 + i, 10);
				}
				else
					c_put_str(color,format("%c) [$ %8d] %s",'a'+i, k_info[q_ptr->k_idx].cost,o_name),7 + i, 10);

			}
			else //消耗品調達テーブル とりあえず全てを表示
			{
				if(table_num >= MAX_MACHINE_SUPPLY) break;
				object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[table_num][0], engineer_guild_supply_table[table_num][1]));

				if(cs==i) color = TERM_YELLOW;
				else if(p_ptr->au < supply_price_list[table_num]) color = TERM_L_DARK;
				else color = TERM_WHITE;
				object_desc(o_name, q_ptr, 0);
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					c_put_str(color,format(_("%c) [ 在庫無し ] ", "%c) [ out of stock ]"),'a'+i),7 + i, 10);
				}
				else
					c_put_str(color,format("%c) [$ %8d] %s",'a'+i, supply_price_list[table_num],o_name),7 + i, 10);
			}
		}

		prt("",0,0);
		c = inkey();
		if (c == ESCAPE)
		{
			clear_bldg(5, 18);
			return;
		}
		if (c == '2' && cs < 9) cs++;
		if (c == '8' && cs > 0) cs--;
		if (c == '\r' || c == '\n') c=cs+'a';
		if (c == ' ' || c == 'n') page++;
		if (make_machine && page > 2) page = 0;
		else if (!make_machine && page > 1) page = 0;

		if (make_machine && c == 'x')
		{
			choice = page*10 + cs;
			if(p_ptr->town_num == TOWN_HITOZATO && page)
			{
				msg_print(_("「ここの設備じゃ無理だね。悪いけど私たちのバザーまで来てよ。」",
                            "'Sorry, but you can't do it in this workshop. Come to our bazaar.'"));
				msg_print(NULL);
				continue;
			}
			else if(machine_table[choice].difficulty > power)
			{
				msg_print(_("今のあなたにはそれを作るのは難しいようだ。",
                            "Creating this is too difficult for you right now."));
				msg_print(NULL);
				continue;
			}
			object_prep(q_ptr, lookup_kind(TV_MACHINE, choice));
			screen_object(q_ptr, SCROBJ_FORCE_DETAIL);

		}

		if(c >= 'a' && c <= 'j')
		{
			int number;
			choice = page*10 + (c - 'a');
			if(!make_machine && choice >= MAX_MACHINE_SUPPLY) continue; //消耗品調達で空欄を選んだ場合

			if(make_machine)
			{
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					msg_print(_("「ここの設備じゃ無理だね。悪いけど私たちのバザーまで来てよ。」",
                                "'Sorry, but you can't do it in this workshop. Come to our bazaar.'"));
					msg_print(NULL);
					continue;
				}
				else if(machine_table[choice].difficulty > power)
				{
					msg_print(_("今のあなたにはそれを作るのは難しいようだ。",
                                "Creating this is too difficult for you right now."));
					msg_print(NULL);
					continue;
				}

				object_prep(q_ptr, lookup_kind(TV_MACHINE, choice));
				number = 1;
				break;
			}
			else
			{
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					msg_print(_("「この支部には置いてないよ。悪いけど私たちのバザーまで来てよ。」",
                                "'Sorry, not available at this branch. Come to our bazaar.'"));
					msg_print(NULL);
					continue;
				}
				else
				{
					object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[choice][0], engineer_guild_supply_table[choice][1]));
					number = get_quantity(_("いくつ注文しますか？", "Order how many?"),99);
					if(!number) continue;
					q_ptr->number = number;
					if(!inven_carry_okay(q_ptr))
					{
						msg_print(_("そんなに持ちきれない。", "You can't carry that much."));
						clear_bldg(5, 18);
						return;
					}
					break;
				}
			}
		}
	}

	clear_bldg(5, 18);
	if(make_machine) price = k_info[q_ptr->k_idx].cost;
	else price = supply_price_list[choice] * q_ptr->number;
	object_desc(o_name, q_ptr, 0);
	if(p_ptr->au < price)
	{
		msg_format(_("お金が足りない。", "You don't have enough money."));
		return;
	}

	if(make_machine) msg_format(_("$%d使って%sを作る。", "You'll spend $%d to build %s."),price,o_name);
	else	msg_format(_("$%dで%sを調達する。", "You'll spend $%d to obtain %s."),price,o_name);
	if (!get_check_strict(_("よろしいですか？", "Are you sure?"), CHECK_DEFAULT_Y)) return;

	//今のところ油壺用
	apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
	//アイテムを*鑑定*状態にする
	identify_item(q_ptr);
	q_ptr->ident |= (IDENT_MENTAL);

	p_ptr->au -= price;
	building_prt_gold();

	if(make_machine)
	{
		msg_print(_("あなたは機械の製作に取り掛かった・・", "You started building the machine..."));
		msg_print(NULL);
		msg_format(_("%sが完成した！", "%s is complete!"),o_name);
	}
	else  msg_format(_("%sを受け取った。", "You receive %s."),o_name);

	slot_new = inven_carry(q_ptr);
	autopick_alter_item(slot_new, FALSE);

	handle_stuff();
	clear_bldg(5, 18);

}


/*:::エンジニアギルドの説明を聞く*/
static void engineer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI)
	{
		msg_print(_("あなたはエンジニアではないのでここでできることはないようだ。",
                    "You can't do anything here since you're not an engineer."));
		return;
	}
	display_rumor_new(21);
}


//v1.1.92 EXダンジョン建物用の噂コマンド 建物説明とか
static void ex_rumor_new(void)
{

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

	switch(ex_bldg_idx)
	{
	case BLDG_EX_GRASSROOTS:
		display_rumor_new(23);
		break;

	case BLDG_EX_YUMA:
		display_rumor_new(24);
		break;
	default:
		msg_print(_("ERROR:この建物のex_rumor_new()処理が登録されていない",
                    "ERROR: ex_rumor_new() logic undefined for this building"));


	}
}


//v1.1.92 尤魔が呪われたアイテムを食べて消滅させる
static void yuma_eats_cursed_item(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point, total_point;

	q = _("「どれを喰ってほしい？」", "'What do you want me to eat?'");
	s = _("「呪いの匂いはしないな。つまらん。」", "'Meh, I don't smell any curses.'");

	item_tester_hook = item_tester_hook_cursed;
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return;
	o_ptr = &inventory[item];

	if (!(wield_check(item, INVEN_PACK,0)))
	{
		return;
	}

	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if (!get_check(format(_("「%sを喰うぞ？いいんだな？」", "'I'm going to eat %s; are you sure?' "), o_name))) return;

	if (o_ptr->curse_flags & TRC_PERMA_CURSE)
	{
		prt(_("「こいつは千年ものの呪いだな！栄養満点だ！」",
            "'That's a thousand-year curse! It's so fullfilling!'"),10,20);
	}
	else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
	{
		prt(_("「なかなか悪くない味の呪いだな。」",
            "'This curse tasted pretty well.'"), 10, 20);
	}
	else
	{
		prt(_("「貧相な呪いだな。喰い足りん。」",
            "'What a lousy curse. I need more food.'"), 10, 20);
	}


	inven_item_increase(item, (0-o_ptr->number));
	inven_item_describe(item);
	inven_item_optimize(item);

}



///mod150811 魔理沙魔法作成ルールを読む
static void marisa_read_memo(void)
{
	if(p_ptr->pclass != CLASS_MARISA)
	{
		msg_print(_("ERROR:魔理沙以外でmarisa_read_memo()が呼ばれた",
                    "ERROR: marisa_read_memo() called for non-Marisa"));
		return;
	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("今は闇市場の調査中だ。アビリティカードを探しに行こう。",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return;
	}

	display_rumor_new(22);
}


/*
 * Execute a building command
 */
/*:::建物内でのコマンド処理*/
///sys 建物内コマンド処理
static void bldg_process_command(building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);



	//v2.0.8 いつからか知らんがEXTRA建物で価格をmember_costsに設定してるのにここでother_costsで処理してるから全部の価格が0になってた
	//元の処理に戻す。is_owner()は現状常にTRUEなのでmember_costs[]のみが参照される
	if (is_owner(bldg))
		bcost = bldg->member_costs[i];
	else

		bcost = bldg->other_costs[i];

	if(cheat_room) msg_format("cost:%d", bcost);

	/* action restrictions */
	///building ここ変更しないといけない
	if (((bldg->action_restr[i] == 1) && !is_member(bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(bldg)))
	{
#ifdef JP
msg_print("それを選択する権利はありません！");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	/*:::お金が足りない場合return ただし魔道具充填は別処理*/
	//v1.1.48 所持金マイナスだと無料の行為もできなかったので条件式追加
	if ((bact != BACT_RECHARGE) &&
	    ((bldg->member_costs[i] > 0 && (bldg->member_costs[i] > p_ptr->au) && is_owner(bldg)) ||
	     (bldg->other_costs[i] > 0 && (bldg->other_costs[i] > p_ptr->au) && !is_owner(bldg))))
	{
#ifdef JP
msg_print("お金が足りません！");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	clear_bldg(4, 18);

	///sys building 建物の各コマンド処理 BACT_???に一致
	switch (bact)
	{
	case BACT_NOTHING:
		/* Do nothing */
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(FALSE);
		break;
		///sysdel building 削除設備
	case BACT_TOWN_HISTORY:
		town_history();
		break;
		///sysdel building 削除設備
	case BACT_RACE_LEGENDS:
		race_legends();
		break;
	case BACT_QUEST:
		castle_quest();
		break;
	case BACT_KING_LEGENDS:
	case BACT_ARENA_LEGENDS:
	case BACT_LEGENDS:
		show_highclass();
		break;

	case BACT_POSTER:
	///del131213
	case BACT_TALK_KAGUYA:
		kaguya_quest();
		break;
	case BACT_ARENA:
		msg_print(_("ERROR:arena_comm()はもうコメントアウトした",
                    "ERROR: arena_comm() already commented out"));
		//arena_comm(bact);
		break;
	///sysdel カジノ
	//case BACT_IN_BETWEEN:
	//case BACT_CRAPS:
	//case BACT_SPIN_WHEEL:
	//case BACT_DICE_SLOTS:
	//case BACT_GAMBLE_RULES:
	//case BACT_POKER:
	//	gamble_comm(bact);
	//	break;
	case BACT_REST:
	case BACT_RUMORS:
	case BACT_FOOD:
		paid = inn_comm(bact);
		break;
	case BACT_RESEARCH_MONSTER:
		paid = research_mon();
		break;
	case BACT_COMPARE_WEAPONS:
		paid = TRUE;
		bcost = compare_weapons(bcost);
		break;
	case BACT_ENCHANT_WEAPON:
		item_tester_hook = object_allow_enchant_melee_weapon;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_ARMOR:
		item_tester_hook = object_is_armour;
		enchant_item(bcost, 0, 0, 1);
		break;
	case BACT_RECHARGE:
		building_recharge();
		break;
	case BACT_RECHARGE_ALL:
		building_recharge_all();
		break;
	case BACT_IDENTS: /* needs work */
#ifdef JP
		if (!get_check("持ち物を全て鑑定してよろしいですか？")) break;
		identify_pack();
		msg_print(" 持ち物全てが鑑定されました。");
#else
		if (!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack();
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BACT_IDENT_ONE: /* needs work */
		paid = ident_spell(FALSE);
		break;
	//case BACT_LEARN:
		/*:::呪文を学習する。だが現在この設備を持つ町はないらしい。*/
	//	do_cmd_study();
	//	break;
	case BACT_HEALING: /* needs work */
		if (dun_level)
			msg_print(_("治療を受けた。", "You are healed."));
		else
			msg_print(_("あなたは温泉で体を癒やした...", "You heal your body in the hot springs..."));

		//v2.0.3 埴輪や人形などの破損も直ることにする
		if (RACE_BREAKABLE)
		{
			set_broken(-(BROKEN_MAX));
			handle_stuff();
		}

		hp_player(200);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);

		//v1.1.92 配下も回復する
		bldg_heal_all_pets(30);

		paid = TRUE;
		break;

		///mod140113 永遠亭用に強化した
	case BACT_HEALING_PLUS:
		msg_print(_("治療を受けた。", "You are healed."));

		//v2.0.3 埴輪や人形などの破損も直ることにする
		if (RACE_BREAKABLE)
		{
			set_broken(-(BROKEN_MAX));
			handle_stuff();
		}

		hp_player(5000);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);
		set_image(0);
		set_asthma(0);
		restore_level();
		set_food(MAX(p_ptr->food, PY_FOOD_FULL - 1));

		//v1.1.92 配下も回復する
		bldg_heal_all_pets(100);

		paid = TRUE;
		break;
	case BACT_RESTORE: /* needs work */
		if (do_res_stat(A_STR)) paid = TRUE;
		if (do_res_stat(A_INT)) paid = TRUE;
		if (do_res_stat(A_WIS)) paid = TRUE;
		if (do_res_stat(A_DEX)) paid = TRUE;
		if (do_res_stat(A_CON)) paid = TRUE;
		if (do_res_stat(A_CHR)) paid = TRUE;
		if(!paid) msg_print(_("能力回復の必要はないようだ。", "Your abilities don't need restoring."));
		break;
	case BACT_ENCHANT_ARROWS:
		item_tester_hook = item_tester_hook_ammo;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_BOW:
		//item_tester_tval = TV_BOW;
		item_tester_hook = item_tester_hook_bow_crossbow;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_RECALL:
		if (recall_player(1)) paid = TRUE;
		break;
	///sysdel トランプ塔の階指定帰還 とりあえず無効にしておく予定
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int max_depth;

		clear_bldg(4, 20);
#ifdef JP
		select_dungeon = choose_dungeon("にテレポート", 4, 0);
#else
		select_dungeon = choose_dungeon("teleport", 4, 0);
#endif
		show_building(bldg);
		if (!select_dungeon) return;

		max_depth = d_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		///quest トランプ塔のテレポでオベロンを飛ばさないようにする部分
		if (select_dungeon == DUNGEON_ANGBAND)
		{
			///mod140118 最終ボスをオベロンにする
			if (quest[QUEST_YUKARI].status != QUEST_STATUS_FINISHED) max_depth = 50;
			//easyでは紫を倒しても50階までしか行けない
			if (difficulty == DIFFICULTY_EASY ) max_depth = 50;

			//if (quest[QUEST_TAISAI].status != QUEST_STATUS_FINISHED) max_depth = 98;
			//else if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%sの何階にテレポートしますか？", d_name + d_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", d_name + d_info[select_dungeon].name), max_depth);
#endif

		if (amt > 0)
		{
			p_ptr->word_recall = 1;
			p_ptr->recall_dungeon = select_dungeon;
			max_dlv[p_ptr->recall_dungeon] = ((amt > d_info[select_dungeon].maxdepth) ? d_info[select_dungeon].maxdepth : ((amt < d_info[select_dungeon].mindepth) ? d_info[select_dungeon].mindepth : amt));
			if (record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "トランプタワーで");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("回りの大気が張りつめてきた...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}

	case BACT_LOSE_MUTATION:
	/*:::突然変異治療　ラッキーマンの白オーラ除く*/
		///mod140318 永続型変異、生来型変異の判定を加えて少し書き換えた。
		{
			///mod160225 バグ対応のため永続変異を付け直す
			p_ptr->muta1 |= p_ptr->muta1_perma;
			p_ptr->muta2 |= p_ptr->muta2_perma;
			p_ptr->muta3 |= p_ptr->muta3_perma;
			p_ptr->muta4 |= p_ptr->muta4_perma;

			if(muta_erasable_count())
			{
				/*:::突然変異をランダムに失う。20回試行して今持ってる変異に当たらないとFALSEになるが何かに当たるまでここで繰り返している*/
				while(!lose_mutation(0));
				paid = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("治すべき突然変異が無い。");
#else
				msg_print("You have no mutations.");
#endif
				msg_print(NULL);
			}
			p_ptr->update |= PU_BONUS;
		}
		break;
	///sysdel 闘技場
	case BACT_BATTLE:
		msg_print(_("闘技場は正直よく分からない。", "You don't really understand the arena."));
		//kakutoujou();
		break;
	//龍神像を見る
	case BACT_RYUUJIN:

		if(difficulty == DIFFICULTY_LUNATIC)
		{
			msg_print(_("巨大な像が鎮座している。タコに似た頭部、無数の触腕、禍々しい鉤爪・・？",
                        "A giant statue stand here. It has an octopus-like head, countless tentacles, terrifying claws...?"));
			if(!p_ptr->resist_chaos && !p_ptr->resist_insanity)set_image(p_ptr->image + 100);
		}
		else if(difficulty == DIFFICULTY_HARD)
			msg_print(_("龍神像の目は赤く光っている・・",
                        "Dragon God Statue's eyes are glowing red..."));
		else
			msg_print(_("龍神像の目は青く光っている。今日はいい天気らしい。",
                        "Dragon God Statue's eyes are glowing blue. Looks like weather is going to be nice today."));
		//tsuchinoko();
		break;
	//case BACT_KUBI:
		///del131221 死体削除
		//msg_print("事務所は閉店した。");
		//shoukinkubi();
		break;
	//case BACT_TARGET:
		///del131221 死体削除
		//msg_print(_("事務所は閉店した。", "This office has closed."));
		//today_target();
		break;
	//case BACT_KANKIN:
		///del131221 死体削除
		//kankin();
		//break;
		///sysdel virtue
	case BACT_GACHA:
		buy_gacha_box();

		break;
	case BACT_GLASSROOTS_RUMOR:
		msg_print(_("未実装だ。", "Unimplemented."));
		//kankin();
		break;
		/*:::平衡化*/
	case BACT_HEIKOUKA:
	msg_print(_("徳など知らぬ。", "You know nothing about those 'virtues'."));
#if 0
#ifdef JP
		msg_print("平衡化の儀式を行なった。");
#else
		msg_print("You received an equalization ritual.");
#endif
		set_virtue(V_COMPASSION, 0);
		set_virtue(V_HONOUR, 0);
		set_virtue(V_JUSTICE, 0);
		set_virtue(V_SACRIFICE, 0);
		set_virtue(V_KNOWLEDGE, 0);
		set_virtue(V_FAITH, 0);
		set_virtue(V_ENLIGHTEN, 0);
		set_virtue(V_ENCHANT, 0);
		set_virtue(V_CHANCE, 0);
		set_virtue(V_NATURE, 0);
		set_virtue(V_HARMONY, 0);
		set_virtue(V_VITALITY, 0);
		set_virtue(V_UNLIFE, 0);
		set_virtue(V_PATIENCE, 0);
		set_virtue(V_TEMPERANCE, 0);
		set_virtue(V_DILIGENCE, 0);
		set_virtue(V_VALOUR, 0);
		set_virtue(V_INDIVIDUALISM, 0);
		get_virtues();
		paid = TRUE;
#endif

		break;

		/*:::街移動*/
	case BACT_TELE_TOWN:
		paid = tele_town(FALSE);
		break;
		//v1.1.32
	case BACT_ROPE_WAY:
		paid = riding_ropeway();
		break;

		//v1.1.91
	case BACT_GO_UNDERGROUND:
		paid = pass_through_chirei_koudou();
		break;

	case BACT_EX_RUMOR_NEW:
		ex_rumor_new();
		break;


	/*:::防御について調べる（武器匠）*/
	case BACT_EVAL_AC:
		paid = eval_ac(p_ptr->dis_ac + p_ptr->dis_to_a);
		break;
	/*:::折れた武器修復*/
	case BACT_BROKEN_WEAPON:
		paid = TRUE;
		bcost = repair_broken_weapon(bcost);
		break;
	///mod140112 噂　複数行に修正
	case BACT_RUMORS_NEW:
		display_rumor_new(p_ptr->town_num);
		paid = TRUE;
		break;
	///mod140113 霧雨魔法店　詳細未実装
	case BACT_MARIPO_GAIN:
		material_2_maripo();
		break;
	case BACT_MARIPO_CHANGE:
		maripo_2_present();
		break;
	case BACT_KORIN_MAKEOVER:
		msg_print(_("済まないが、まだ準備ができていないんだよ。",
                    "Sorry, I'm not ready yet."));
		break;
	case BACT_TALK_KAGEROU:
		talk_kagerou();
		break;
	case BACT_BARTER:
		grassroots_barter();
		break;
	case BACT_TRADING_ABILITY_CARD:
		trading_ability_cards();
		break;
	case BACT_BUY_ABILITY_CARD:
		buy_ability_card(FALSE);
		break;
	case BACT_SELL_ABILITY_CARD:
		sell_ability_card();
		break;

	case BACT_CHECK_ABILITY_CARD:
		buy_ability_card(TRUE);
		break;

	case BACT_ARCHER_GUILD_GUIDE:
		archer_guild_guide();
		break;
	case BACT_ARCHER_GUILD_ARROW_ENC:
		archer_guild_arts2();
		break;
	case BACT_ARCHER_GUILD_ARROW_EGO:
		archer_guild_arts();
		break;
	case BACT_REMOVE_CURSE:
		paid = bldg_remove_curse();
		break;
	case BACT_ESCAPE_FROM_GENSOUKYOU:
		if(escape_from_gensoukyou()) return;
		break;
	case	BACT_SELL_CURIOSITY:
		sell_curiosity();
		break;
	case BACT_MAKE_MACHINE:
		bact_engineer_guild(TRUE);
		break;
	case BACT_BUY_MACHINE_SUPPLY:
		bact_engineer_guild(FALSE);
		break;

	case BACT_ENGINEER_GUILD_GUIDE:
		engineer_guild_guide();
		break;

	case BACT_SMITH_GUILD:
		do_cmd_kaji(FALSE);
		break;
	case BACT_SMITH_GUILD_GUIDE:
		do_cmd_kaji(TRUE);
		break;
	case BACT_REIMU_FORTUNETELLER:
		paid = reimu_fortune_teller();
		break;
	case	BACT_REIMU_DEITY_CHECK:
		reimu_deity_check();
		break;
	///mod150129 お賽銭箱
	case	BACT_THROW_OSAISEN:
		throw_osaisen();
		break;
	case	BACT_CHECK_OSAISEN:
		check_osaisen();
		break;
	case BACT_BATTLE_MON_GAMBLING:
		battle_mon_gambling();
		break;
	case BACT_MARISA_MAKE_MAGIC_RECIPE:
		marisa_make_magic_recipe();
		break;
	case BACT_MARISA_READ_MEMO:
		marisa_read_memo();
		break;
	case BACT_MARISA_EXTRACT_MATERIAL:
		marisa_extract_material(TRUE);
		break;
	case BACT_MARISA_CHECK_ESSENCE:
		marisa_check_essence();

		break;
	case BACT_MARISA_MAKE_MAGIC:
		make_marisa_magic();
		break;
	case BACT_MARISA_CARRY_MAGIC:
		carry_marisa_magic();
		break;
	case BACT_MARISA_CHECK_MAGIC:
		check_marisa_recipe();
		break;
	case BACT_PRISM_CLASS_CHANGE:
		prism_change_class(FALSE);
		break;
	case BACT_HINA_MAKE_MONEY:
		hina_at_work();
		break;
	case BACT_COMPOUND_DRUG:
		compound_drug();
		break;

	case BACT_CHECK_DRUG_RECIPE:
		check_drug_recipe();
		break;

	case BACT_KOGASA_SMITH:
		kogasa_smith();
		break;

	case BACT_EX_SEARCH_AROUND:
		exbldg_search_around();
		break;

	case BACT_DESTROY_ITEM:
		yuma_eats_cursed_item();
		break;


	case BACT_BUY_HINA_NINGYOU:
		buy_hinaningyou();
		break;
		//Exモード限定　アイテムを自宅に運んでもらう
	case BACT_ITEM_TRANSFER:
		{
			int ex_bldg_num = f_info[cave[py][px].feat].subtype;
			int ex_bldg_idx = building_ex_idx[ex_bldg_num];
			int time_div = 8;
			//Hack - Exダンジョンのお燐にアイテムを運んでもらう処理は、アイテムを自宅から持ってきてもらっている最中には無効。
			if(ex_bldg_idx == BLDG_EX_ORIN &&
				(town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
			{
				msg_print(_("今は誰もいない。", "There's nobody here."));
				break;
			}

			paid = item_recall(4);
			break;

		}

	case BACT_ITEMCARD_TRADING:
		grassroots_trading_cards();
		break;
	case BACT_ORIN_DELIVER_ITEM:
		hack_flag_access_home_orin = TRUE;
		paid = orin_deliver_item();
		hack_flag_access_home_orin = FALSE;
		break;

	case BACT_HELLO_WORK:
		paid = straygod_job_adviser();
		break;
	case BACT_NIGHTMARE_DIARY:
		if(nightmare_diary()) 	leave_bldg = TRUE;
		break;
	case BACT_BUY_STRANGE_OBJ:
		bact_buy_strange_object();
		break;

	case BACT_MAKE_SPELLCARD:
		bact_marisa_make_spellcard();
		break;

	case BACT_HATATE_SEARCH_MON:
		paid = hatate_search_unique_monster();
		break;

	case BACT_GEIDONTEI_COOKING:
		geidontei_cooking(p_ptr->pclass == CLASS_MIYOI);//美宵のときのみ「自分で作るフラグ」ON
		break;

	default:
		msg_format(_("ERROR:BACT(%d)が未定義", "ERROR: unhandled BACT_ constant, %d"), bact);

	}

	if (paid)
	{
		p_ptr->au -= bcost;
	}
}


/*
 * Enter quest level
 */
void do_cmd_quest(void)
{
	energy_use = 100;

	if (!cave_have_flag_bold(py, px, FF_QUEST_ENTER))
	{
#ifdef JP
msg_print("ここにはクエストの入口はない。");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
		int quest_num = cave[py][px].special;


#ifdef JP
		///v1.1.12 クエスト名表記
		if(quest_num)
		{
			//起動時点ではquest[]に名前が入っていないのでp_ptr->inside_questを一時的に書き換えてクエスト初期化を行う必要がある
			int old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = quest_num;
			init_flags = INIT_NAME_ONLY;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			msg_format("ここにはクエスト「%s」(レベル%d)への入口があります。",quest[quest_num].name,quest[quest_num].level);
			p_ptr->inside_quest = old_quest;
		}
		else
			msg_print("ここにはクエストへの入口があります。");

		if (!get_check("クエストに入りますか？")) return;
		///del131213  msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("『とにかく入ってみようぜぇ。』");
#else
		if(quest_num)
		{
			//起動時点ではquest[]に名前が入っていないのでp_ptr->inside_questを一時的に書き換えてクエスト初期化を行う必要がある
			int old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = quest_num;
			init_flags = INIT_NAME_ONLY;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			msg_format("This entrance leads to the quest '%s' (Level: %d).",quest[quest_num].name,quest[quest_num].level);
			p_ptr->inside_quest = old_quest;
		}
		else
			msg_print("This entrance leads to a quest.");

		if (!get_check("Enter the quest?")) return;
#endif

		/* Player enters a new quest */
		p_ptr->oldpy = 0;
		p_ptr->oldpx = 0;
		/*:::クエストの終了状態をチェック？なぜ今？*/
		leave_quest_check();

		if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		p_ptr->inside_quest = cave[py][px].special;

		p_ptr->leaving = TRUE;

		///mod140629 狐狸戦争
		/*:::-Hack- 狐狸戦争クエに入るとき、どちらの勢力につくか選択してフラグを保存する*/
		if(cave[py][px].special == QUEST_KORI)
		{
			bool enable_a = TRUE;
			bool enable_b = TRUE;
			bool enable_c = TRUE;
			int i;
			char c;

			screen_save();
			//藍は狐陣営のみ
			if(p_ptr->pclass == CLASS_RAN)
			{
				 enable_b = FALSE;
				 enable_c = FALSE;
			}
			//マミゾウは狸陣営のみ
			else if(p_ptr->pclass == CLASS_MAMIZOU)
			{
				 enable_a = FALSE;
				 enable_c = FALSE;
			}
			else if(p_ptr->prace == RACE_YOUKO) enable_b = FALSE;
			else if(p_ptr->prace == RACE_BAKEDANUKI) enable_a = FALSE;
			///mod150110 橙は狸陣営に付けない
			else if(p_ptr->pclass == CLASS_CHEN) enable_b = FALSE;
			///mod150328 ぬえは狸陣営のみ
			else if(p_ptr->pclass == CLASS_NUE) enable_a = FALSE;
			//霊夢と映姫は殲滅のみ
			//v2.0.17 残無も全滅のみにしとく
			else if(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_ZANMU)
			{
				 enable_a = FALSE;
				 enable_b = FALSE;
			}

			if(!enable_a && !enable_b && !enable_c)
			{
				msg_format(_("ERROR:狐狸戦争クエストの選択肢が全て無効",
                            "ERROR: No valid options for Fox-Tanuki War quest"));
				return;
			}

			msg_print(_("どちらの勢力につきますか？", "Which side are you on?"));
			for(i=1;i<7;i++)Term_erase(12, i, 255);
			if(enable_a) put_str(_("a) 狐陣営に味方する", "a) Ally with the foxes"), 3 , 40);
			if(enable_b) put_str(_("b) 狸陣営に味方する", "b) Ally with the tanuki"), 4 , 40);
			if(enable_c) put_str(_("c) 全員倒す", "c) Beat everyone up"), 5 , 40);

			while(1)
			{
				c = inkey();
				if(c == 'a' && !enable_a) continue;
				if(c == 'b' && !enable_b) continue;
				if(c == 'c' && !enable_c) continue;
				if(c >= 'a' && c <= 'c') break;
			}
			screen_load();

			if(c == 'a') quest[QUEST_KORI].flags |= QUEST_FLAG_TEAM_A;
			else if(c == 'b') quest[QUEST_KORI].flags |= QUEST_FLAG_TEAM_B;

		}

		//v1.1.91 抗争クエスト1
        //抗争クエに入るとき、どちらの勢力につくか選択してフラグをp_ptr->animal_ghost_align_flagに記録する
		else if (cave[py][px].special == QUEST_YAKUZA_1)
			{
				int i;
				char c;
				u32b selectable_flags = 0L;

				//選択可能な勢力をフラグで設定
				//プロテウスリングとかで変身中のときも考慮する？

				//動物霊に憑依されている場合最優先でその所属に固定
				if (p_ptr->muta4 & MUT4_GHOST_WOLF)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_OTTER)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_HANIWA);
				}
			//早鬼
				else if (p_ptr->pclass == CLASS_SAKI || p_ptr->pclass == CLASS_ENOKO)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
				}
			//八千慧
				else if (p_ptr->pclass == CLASS_YACHIE || p_ptr->pclass == CLASS_BITEN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
				}
			//尤魔
				else if (p_ptr->pclass == CLASS_YUMA || p_ptr->pclass == CLASS_CHIYARI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
				}
			//埴輪と袿姫
				else if (p_ptr->prace == RACE_HANIWA || p_ptr->pclass == CLASS_KEIKI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_HANIWA);
				}
			//ほか種族動物霊は所属勢力で決まる。無所属(あるいは今後実装されるかもしれない正体不明の集団)は埴輪以外全て選択可能
				else if (p_ptr->prace == RACE_ANIMAL_GHOST)
				{
					switch (CHECK_ANIMAL_GHOST_STRIFE)
					{
					case ANIMAL_GHOST_STRIFE_KEIGA:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
						break;

					case ANIMAL_GHOST_STRIFE_KIKETSU:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
						break;

					case ANIMAL_GHOST_STRIFE_GOUYOKU:
						selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
						break;
					default:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA | ANIMAL_GHOST_ALIGN_KIKETSU | ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
						break;
					}
				}
			//神奈子とフランは剛欲か全滅
				else if (p_ptr->pclass == CLASS_KANAKO || p_ptr->pclass == CLASS_FLAN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
			//さとまいは？埴輪か全滅？
			//霊夢・映姫・久侘歌・種族死神は全滅オンリー
				else if (p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_KUTAKA || p_ptr->prace == RACE_DEATH)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
				else //上に挙げた条件に当てはまらないとき全て選択可能
				{
					selectable_flags |= (ANIMAL_GHOST_ALIGN_KEIGA | ANIMAL_GHOST_ALIGN_KIKETSU | ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_HANIWA | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}

				//v2.0.13 ヤクザクエスト2で倒した賞金首の属する組織には味方できない。もし味方できる組織がいなくなったら全員倒すの選択肢しか出ない
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KEIGA) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KEIGA);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KIKETSU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KIKETSU);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_GOUYOKU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_GOUYOKU);
				if (!selectable_flags) selectable_flags = ANIMAL_GHOST_ALIGN_KILLTHEMALL;

				screen_save();
				for (i = 1; i<9; i++)Term_erase(12, i, 255);

				put_str(_("どの勢力につきますか？", "Join which forces?"), 1, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA) put_str(_("a) 勁牙組", "a) Keiga Family"), 3, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU) put_str(_("b) 鬼傑組", "b) Kiketsu Family"), 4, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU) put_str(_("c) 剛欲同盟", "c) Gouyouku Alliance"), 5, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA) put_str(_("d) 埴輪兵団", "d) Haniwa Corps"), 6, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL) put_str(_("e) 全員倒す", "e) Defeat them all"), 7, 40);

				while (1)
				{
					c = inkey();
					//v1.1.92 間違えて&&でつないでたので修正
					if (c < 'a' || c > 'e') continue;

					if (c == 'a' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA)) continue;
					if (c == 'b' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU)) continue;
					if (c == 'c' && !(selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU)) continue;
					if (c == 'd' && !(selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA)) continue;
					if (c == 'e' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL)) continue;

					break;
				}
				screen_load();

				//フラグをp_ptr->animal_ghost_align_flagに記録
				if (c == 'a')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KEIGA;
				else if (c == 'b')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KIKETSU;
				else if (c == 'c')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_GOUYOKU;
				else if (c == 'd')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_HANIWA;
				else if (c == 'e')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KILLTHEMALL;
				else
					msg_print(_("ERROR:不正な選択が行われた", "ERROR: Incorrect choice"));

			}


		//v1.1.24 クエスト「急流下り」の開始時ターンを記録
		if(QRKDR)
		{
			qrkdr_rec_turn = turn;

		}

	}
}


/*
 * Do building commands
 */
/*:::店でない建物での処理など*/
void do_cmd_bldg(void)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;


	energy_use = 100;
#if 0
	//特殊一時処理 闘技場でテレポートしてフリーズしたセーブデータ救出用
	if (p_ptr->inside_battle)
	{
		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->leaving = TRUE;
		p_ptr->inside_battle = FALSE;

		/* Re-enter the monster arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
#endif

	if (!cave_have_flag_bold(py, px, FF_BLDG))
	{
		msg_print(_("ERROR:建物ではない場所でdo_cmd_bldg()が呼ばれている",
                    "ERROR: do_cmd_bldg() called outside of building"));

		return;
	}

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
	{
		msg_print(_("建物に入れない。", "You can't enter buildings."));
		return ;
	}

	if(SUPER_SHION)
	{
		msg_print(_("今建物に入ったら大変だ。", "You really shouldn't enter buildings right now."));
		return;
	}


	if(world_player || SAKUYA_WORLD)
	{
		msg_print(_("今入っても意味がない。", "No point in entering right now."));
		return ;
	}

	which = f_info[cave[py][px].feat].subtype;

	bldg = &building[which];

	//雛は厄が多すぎると建物に入れない(無人販売所のみ可)
	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > (HINA_YAKU_LIMIT2+5000) && (p_ptr->town_num != TOWN_HITOZATO || which != 9))
	{
		msg_print(_("入ろうとしたが追い払われてしまった…",
                    "You tried to enter, but were chased out..."));
		return ;
	}



	//v1.1.85 鯢呑亭
	if (p_ptr->town_num == TOWN_HITOZATO && which == 12)
	{
		if (is_daytime()) //日中は閉店
		{
			if(p_ptr->pclass == CLASS_MIYOI)
				msg_print(_("まだ開店前だ。",
                        "It's before opening time."));
			else
				msg_print(_("「ごめんなさい。まだ開店前なんですよー。」",
                        "'Sorry, we're not open yet.'"));
			return;
		}
		else if (is_midnight() && p_ptr->pclass != CLASS_MIYOI) //深夜は妖怪専用
		{
			if (player_looks_human_side())
			{
				if (one_in_(2))msg_print(_("店は閉まっている。中からは賑やかな気配が伝わってくるのだが...",
                                        "The store is closed. But you sense lively presence from inside..."));
				else msg_print(_("店は閉まっている。看板に「蚕喰」と書かれた妙な札が掛かっている...?",
                                "The store is closed. There's a scrap of paper on the sign that says 'Silkmoth'...?"));
				return;
			}
		}
		else //深夜以外の夜間は人間専用
		{
			if (!player_looks_human_side() && p_ptr->pclass != CLASS_MIYOI)
			{
				if (one_in_(2))msg_print(_("美宵「ちょっ！貴方はまだ駄目ですよ！」",
                                        "Miyou - 'Wait! You can't go in here!'"));
				else msg_print(_("人間たちが談笑している。割って入るのはやめておこう。",
                                "The humans are chatting lively. Better not break in."));
				return;
			}
		}
	}





	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;
	///sys store アリーナはB:2で記述されていること
#if 0
	//本家アリーナで敗北後にアリーナ利用不可になる処理
	if ((which == 2) && (p_ptr->arena_number < 0))
	{
#ifdef JP
		msg_print("「敗者に用はない。」");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	//本家アリーナでモンスターを倒すまでアリーナから出られなくなる処理
	else if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
#ifdef JP
			prt("ゲートは閉まっている。モンスターがあなたを待っている！", 0, 0);
#else
			prt("The gates are closed.  The monster awaits!", 0, 0);
#endif
		}
		/*:::アリーナでモンスターに勝って出口に到達したとき*/
		else
		{
			/* Don't save the arena as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

			p_ptr->inside_arena = FALSE;
			p_ptr->leaving = TRUE;

			/* Re-enter the arena */
			/*:::アリーナの建物画面へ戻る*/
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			energy_use = 0;
		}

		return;
	}
	else
#endif

	//v1.1.51　新アリーナでモンスター未打倒でも出られるようにする
	if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
			if (!get_check_strict(_("目を覚ましますか？", "Wake up?"), CHECK_OKAY_CANCEL)) 	return;
		}

		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->inside_arena = FALSE;
		p_ptr->leaving = TRUE;
		//v1.1.52 出るときもフラグリセットする。紫苑で憑依先を拾ってこれるのは色々まずい
		reset_tim_flags();

		//v1.1.54
		if (p_ptr->food < PY_FOOD_ALERT) p_ptr->food = PY_FOOD_ALERT - 1;

		//v1.1.55 アリーナから出るとき、アリーナに入ったときのHPに戻す
		p_ptr->chp = MIN(nightmare_record_hp, p_ptr->mhp);

		if (p_ptr->chp < 0) p_ptr->chp = 0; //paranoia

		/* Re-enter the arena */
		/*:::アリーナの建物画面へ戻る*/
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	/*:::闘技場で決着が着いたらここに来る*/
	else if (p_ptr->inside_battle)
	{
		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->leaving = TRUE;
		p_ptr->inside_battle = FALSE;

		/* Re-enter the monster arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	/*:::建物から出るときのために元の＠の位置を保存しておく*/
	else
	{
		p_ptr->oldpy = py;
		p_ptr->oldpx = px;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(bldg);
	leave_bldg = FALSE;

	//v1.1.59
	if (!EXTRA_MODE || play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD_EX))
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD);

	/*:::建物から出るまでループ*/
	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold();

		command = inkey();

		if (command == ESCAPE)
		{
			leave_bldg = TRUE;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if (bldg->letters[i])
			{
				if (bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if (validcmd)
			bldg_process_command(bldg, i);


		//EXモードの建物内で特定の行動をとった時強制的に建物から出される。process_command()後に敵召喚
		if(hack_ex_bldg_summon_idx || hack_ex_bldg_summon_type ) leave_bldg = TRUE;

		if(p_ptr->inside_battle && p_ptr->leaving) leave_bldg = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/*:::外来人引退用*/
		if(p_ptr->is_dead) break;
	}

	select_floor_music(); //v1.1.58


	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	/*:::クエスト報酬生成など、街を再描画する処理らしい*/
	if (reinit_wilderness)
	{
		p_ptr->leaving = TRUE;
	}

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_BONUS | PU_LITE | PU_MON_LITE);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
"床にメッセージが刻まれている:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"壁にメッセージが刻まれている:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"メッセージを見つけた:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"何かが階段の上に書いてある:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"巻物を見つけた。メッセージが書いてある:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
/*:::ランクエ階に降りてきたときのメッセージ*/
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	monster_race    *r_ptr = &r_info[q_ptr->r_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	///mod160223 Extra用クエスト特殊処理
	if(EXTRA_MODE)
	{
//		msg_print("エキストラクエストだ。あなたは戦ってもいいし逃げてもいい。");
		return;
	}


	strcpy(name, (r_name + r_ptr->name));
	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		//v1.1.92 place_quest_monsters()の仕様変更によりここには来なくなるはず
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("この階は以前は誰かによって守られていたようだ…。");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
			if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[q_idx].level));
			set_deity_bias(DBIAS_REPUTATION, 2);
			set_deity_bias(DBIAS_COSMOS, 1);
		}
		else
		{
#ifdef JP
			msg_format("注意せよ！この階は%sによって守られている！", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
msg_format("注意しろ！この階は%d体の%sによって守られている！", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
/*:::現在クエストダンジョンに居るか現在の階でクエストが行われているときクエストIDを返す。*/
///quest
int quest_number(int level)
{
	int i;

	/* Check quests */
	if (p_ptr->inside_quest)
		return (p_ptr->inside_quest);

	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status != QUEST_STATUS_TAKEN) continue;

		if ((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == dungeon_type))
			return (i);
	}

	/* Check for random quest */
	return (random_quest_number(level));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
///quest ランクエ
///mod160223 条件式追加
int random_quest_number(int level)
{
	int i;

	if (dungeon_type != DUNGEON_ANGBAND) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if ((quest[i].type == QUEST_TYPE_RANDOM || EXTRA_MODE) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == DUNGEON_ANGBAND))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}

