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
/*:::���̌����֘A�@������\��*/


#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

/*:::�푰�A�E�ƁA�̈悪��v���Ă邩�ǂ�������H�ڍוs���@�폜�\��*/
static bool is_owner(building_type *bldg)
{

	//����S�Ẵv���C���[�������o�[�����Ƃ��Ă���
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

/*:::�푰�A�E�ƁA�̈悪��v���Ă邩�ǂ�������H�@�폜�\��*/
/*:::�����H���ŐH���\���ǂ����������Ŕ��肳��Ă���炵��*/
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
 /*:::�{�݂̒��ɂ���Ƃ��̂��߂̎w��͈͉�ʃN���A min_row�`max_row�̑S�Ă̍s���s���܂ŋ󔒂ɂ���*/
void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}
/*:::�莝���̋��z��\���i��ʌ����p�j*/
///mod151114 bldg2.c�ł��g���悤��extern����
void building_prt_gold(void)
{
	char tmp_str[80];

#ifdef JP
prt("�莝���̂���: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)p_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
/*:::�����őI�ׂ�s���ꗗ�\���B�푰�E�Ƃ��Ƃ̃R�X�g�̈Ⴂ�◘�p�ۂȂǋ��*/
///sys �����֌W
///mod160306 �O������Q�Ƃł���悤�ɂ���
void show_building(building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color;
	char tmp_str[80];

	Term_clear();
	//sprintf(tmp_str, "%s (%s) %35s", bldg->owner_name, bldg->owner_race, bldg->name);
	///mod140117 �����\�L����I�[�i�[�푰���폜���I�[�i�[���ƌ��������t�ɂ���

	//v1.1.85 �[����Ǔے��ɓ�������X�����ς��
	if (p_ptr->town_num == TOWN_HITOZATO && f_info[cave[py][px].feat].subtype == 12 && is_midnight())
	{
		sprintf(tmp_str, _("�y�\��z�Ǔے�  %35s", " [Silkmoth] Geidontei %35s"), bldg->owner_name);
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
			/*:::�N�ł����p�ł���ݔ�*/
			if (bldg->action_restr[i] == 0)
			{
				/*:::�����̂Ƃ�*/
				if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				/*:::�L��/�����o�[*/
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				/*:::�L��/�����o�[�ȊO*/
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
			/*:::restr=1�̂Ƃ��A�����o�[�������p�ł��Ȃ��B�h�̐H���Ȃ�*/
			else if (bldg->action_restr[i] == 1)
			{
				if (!is_member(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(�X)");
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
strcpy(buff, "(�X)");
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
prt(" ESC) �������o��", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
/*:::�A���[�i�����@*/
/*:::�A���[�i�ɓ���Ƃ��͂������̃t���O���Ă���ŊK����o�鑀��*/
//v1.1.51��nightmare_diary()�ɐV�A���[�i������B�����͂����g��Ȃ��̂ŃR�����g�A�E�g����B
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
prt("�A���[�i�̗D���ҁI", 5, 0);
prt("���߂łƂ��I���Ȃ��͑S�Ă̓G��|���܂����B", 7, 0);
prt("�܋��Ƃ��� $1,000,000 ���^�����܂��B", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				p_ptr->au += 1000000L;
#ifdef JP
msg_print("�X�y�[�X�L�[�ő��s");
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
msg_print("�N�̂��߂ɍŋ��̒���҂�p�ӂ��Ă������B");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("���킷�邩�ˁH"))
#else
					if (get_check("Do you fight? "))
#endif
					{
#ifdef JP
                        msg_print("���ʂ��悢�B");
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
msg_print("�c�O���B");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
msg_print("���Ȃ��̓A���[�i�ɓ���A���΂炭�̊ԉh���ɂЂ������B");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			///class building ���b�g���ƋR���ȊO�͏�n�̂܂܃A���[�i�ɓ���Ȃ�
			else if (p_ptr->riding && (p_ptr->pclass != CLASS_BEASTMASTER) && (p_ptr->pclass != CLASS_CAVALRY))
			{
#ifdef JP
msg_print("�y�b�g�ɏ�����܂܂ł̓A���[�i�֓��ꂳ���Ă��炦�Ȃ������B");
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
msg_print("���Ȃ��͏����҂��B �A���[�i�ł̃Z�����j�[�ɎQ�����Ȃ����B");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
msg_print("���Ȃ��͂��ׂĂ̓G�ɏ��������B");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &r_info[arena_info[p_ptr->arena_number].r_idx];
				name = (r_name + r_ptr->name);
#ifdef JP
msg_format("%s �ɒ��킷����̂͂��Ȃ����H", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
///del131213 building �A���[�i�̃��[���\�� �g���ĂȂ��炵��
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
			prt(                   " ������ ", row + 8, col);
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
			prt(                   "�I�����W", row + 8, col);
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
			c_put_str(TERM_SLATE, "   ��   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=��=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   ��   " , row + 7, col);
			prt(                  "   ��   " , row + 8, col);
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
			prt(                  "   ��   ", row + 8, col);
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
			prt(                   " �v���� ", row + 8, col);
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
			prt(                "�`�F���[", row + 8, col);
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
		c_put_str(TERM_YELLOW, "���C�����X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g",  4,  3);
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
	c_put_str(TERM_YELLOW, "�t���b�V��",  4,  3);
#else
	c_put_str(TERM_YELLOW, "Flush",  4,  3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�����y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "�c�[�y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X���[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t���n�E�X",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t�H�[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�G�[�X",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�J�[�h",  4,  3);
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
			c_put_str(col, "������", 14,  5+i*16);
		else
			c_put_str(col, "�̂���", 14,  5+i*16);
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
	c_put_str(col, "����", 16,  38);
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
	cptr suit[4] = {"��", "��", "��", "��"};
	cptr card_grph[13][7] = {{"�`   %s     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     �{     ",
				  "     %s     ",
				  "          �`"},
				 {"�Q          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          �Q"},
				 {"�R          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          �R"},
				 {"�S          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          �S"},
				 {"�T          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          �T"},
				 {"�U          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �U"},
				 {"�V          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �V"},
				 {"�W          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          �W"},
				 {"�X %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s �X"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"�i   ��     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=��=| %s",
				  "     ��   �i"},
				 {"�p ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   �p"},
				 {"�j          ",
				  "%s �M�܁L   ",
				  "  ��������  ",
				  "  �� �� ��  ",
				  "   ��    �� ",
				  "    �� �m %s",
				  "          �j"}};
	cptr joker_grph[7] = {    "            ",
				  "     �i     ",
				  "     �n     ",
				  "     �j     ",
				  "     �d     ",
				  "     �q     ",
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
		prt("����������������",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("��",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("��",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������", 13,  i*16);
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
	prt("�c���J�[�h�����߂ĉ�����(�����ňړ�, �X�y�[�X�őI��)�B", 0, 0);
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
			msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
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
sprintf(tmp_str,"�q���� (1-%ld)�H", (long int)maxbet);
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
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
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
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", (long int)maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h����͂��߂悤�B");
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
sprintf(tmp_str, "�Q�[���O�̏�����: %9ld", (long int)oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", (long int)oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
sprintf(tmp_str, "���݂̊|����:     %9ld", (long int)wager);
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
c_put_str(TERM_GREEN, "�C���E�r�g�C�[��",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
sprintf(tmp_str, "���_�C�X: %d        ���_�C�X: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
sprintf(tmp_str, "�ԃ_�C�X: %d", choice);
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
c_put_str(TERM_GREEN, "�N���b�v�X", 5, 2);
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
sprintf(tmp_str, "�P�U���: %d %d      Total: %d", roll1,
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
msg_print("�Ȃɂ��L�[�������Ƃ������U��܂��B");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
sprintf(tmp_str, "�o��: %d %d          ���v:      %d",
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
c_put_str(TERM_GREEN, "���[���b�g", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
get_string("���ԁH (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif
/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN
					for (p = out_val; iswspace(*p); p++);
#else
					for (p = out_val; isspace(*p); p++);
#endif
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
msg_print("0�Ԃɂ��Ƃ����B");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
msg_print("�n�j�A9�Ԃɂ��Ƃ����B");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
sprintf(tmp_str, "���[���b�g�͉��A�~�܂����B���҂� %d�Ԃ��B",
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
c_put_str(TERM_GREEN, "�_�C�X�E�X���b�g", 5, 2);
					c_put_str(TERM_YELLOW, "������   ������            2", 6, 37);
					c_put_str(TERM_YELLOW, "������   ������   ������   5", 7, 37);
					c_put_str(TERM_ORANGE, "�I�����W �I�����W �I�����W 10", 8, 37);
					c_put_str(TERM_UMBER, "��       ��       ��       20", 9, 37);
					c_put_str(TERM_SLATE, "��       ��       ��       50", 10, 37);
					c_put_str(TERM_VIOLET, "�v����   �v����   �v����   200", 11, 37);
					c_put_str(TERM_RED, "�`�F���[ �`�F���[ �`�F���[ 1000", 12, 37);
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
prt("���Ȃ��̏���", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					p_ptr->au += odds * wager;
#ifdef JP
sprintf(tmp_str, "�{��: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
prt("���Ȃ��̕���", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
sprintf(tmp_str, "���݂̏�����:     %9ld", (long int)p_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", (long int)p_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
prt("������x(Y/N)�H", 18, 37);
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
msg_print("�����I��������Ȃ�����Ȃ����I��������o�čs���I");
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
msg_print("�u����ׂ͖����ȁI�ł����͂������������Ă�邩��ȁA��΂ɁI�v");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
				chg_virtue(V_CHANCE, 3);
			}
			else
			{
#ifdef JP
msg_print("�u�����X�b�Ă��܂����ȁA��͂́I�����ɋA���������������B�v");
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

///del ���Z��֘A
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

///mod160305 ���l�`���l�̔���
static void buy_hinaningyou()
{
	char c;
	int price = 100;
	bool flag = FALSE;

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_print(_("���l�`�̔���s���͂ǂ����낤���H", "You wonder how well the hina dolls are selling."));
		return;
	}

	clear_bldg(4,18);

	prt(format(_("���l�`�̖��l�̔������B�e���Ȏ��l�`($%d)������c���Ă���B",
                "It's an unmanned hina doll store. There are some unsold poor paper dolls ($%d)."),price), 5,10);

	if (!get_check_strict(_("�����܂����H", "Purchase?"), CHECK_DEFAULT_Y))
	{
		clear_bldg(4,18);
		return;
	}
	clear_bldg(4,18);
	if(p_ptr->au < price)
	{
		msg_print(_("����������Ȃ��B", "You don't have enough money."));
		return;
	}

	msg_print(_("���l�`����Ɏ�����E�E", "You purchase a hina doll..."));
	if(restore_level()) flag = TRUE;
	else if(remove_curse())
	{
		msg_print(_("�̂��y���Ȃ����C������B", "You feel your body become lighter."));
		flag = TRUE;
	}

	if(!flag) msg_print(_("���ɉ����N����Ȃ������B", "Nothing special happens."));
	msg_print(_("�l�`�͕���ď������B", "The doll breaks and vanishes."));

	p_ptr->au -= price;
	building_prt_gold();

}


//v2.0.3 �A�r���e�B�J�[�h���グ�L�^�ϐ��ɉ��Z
//���i�������܂�`�[�g�I�v�V������e��������Y��Ă����B���X�Ȃ���ǉ����邽�߂ɕʊ֐��ɕ����B
static void add_cardshop_profit(int price)
{

	if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return;

	if (total_pay_cardshop < 100000000) total_pay_cardshop += price;


}


//v1.1.87 �R�@�̓q���10�A�K�`��
static void buy_gacha_box()
{
	int price;
	object_type forge;
	object_type *o_ptr = &forge;
	char o_name[MAX_NLEN];

	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		msg_print(_("�u��������ȁB���ɂ͉����Ȃ����B�v",
                    "'Hey, don't go in. There's nothing in here.'"));

		return;
	}


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print(_("�u�����ƁA���̉��̓J�[�h�̔��l��p���B�v",
                    "'Sorry, card traders only.'"));
		return;

	}

	//�U�b�N�ɋ󂫂��Ȃ��Ƃ�
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		if(p_ptr->pclass == CLASS_SANNYO)
			msg_print(_("�U�b�N�ɋ󂫂��Ȃ��B",
                        "You don't have free space in your inventory."));
		else
			msg_print(_("�u�ӂ�A�U�b�N�ɋ󂫂�����Ă���܂����ȁB�v",
                        "'Hmph, come back once you've freed up some space in your backpack.'"));
		return;
	}

	//�K�`���̉��i�͍����x*1000 ���10��
	//�o�Ă���J�[�h�̊��Ғl�͍����x*1100�`1200���炢�Ȃ̂ő����͊��ɍ���
	price = p_ptr->abilitycard_price_rate * 1000;
	if (price > 100000) price = 100000;

	clear_bldg(4, 18);

	if (p_ptr->pclass == CLASS_SANNYO)
	{
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt(_("�����ˁu���񂽂��A��܂ł͎��B�������ŏ����������Ă��炨���B",
                "Takane - 'Leave trading to us while you're away."), 5, 10);
			prt(format(_("�@�@�@���āA���Ȃ珉���T�[�r�X�ňꔠ$%d����I",
                        "          By the way, it's $%d per box for your first purchase!"), price), 6, 10);
			prt(_("�@�@�@ ", "       "), 7, 10);
			prt(_("�@�@�@���H�������������Ɍ��܂��Ă邶��Ȃ����I�v",
                "          Eh? Of course I'm going to charge money!'"), 8, 10);
		}
		else
		{
			prt(_("�����ˁu��������Ⴂ�I",
                "Takane - 'Welcome!"), 5, 10);
			prt(format(_("�@�@�@�������˂��B���̑���Ȃ�ꔠ$%d���ď����ȁB�v",
                        "          That's right. At current market price, one box costs $%d.'"), price), 6, 10);
		}
	}
	else
	{
		prt(_("�R�@�u�悭�����ȁB���̓J�[�h���g�����ʔ����q�����n�߂��Ƃ��낾�B",
            "Sannyo - 'Ah, welcome. I've started running an interesting game of chance."), 5, 10);
		prt(_("�@�@�@���̓��F�̔��ɂ̓A�r���e�B�J�[�h��10�������Ă���B",
            "          This rainbow-colored box contains 10 ability cards."), 6, 10);
		prt(format(_("�@�@�@�������ꔠ��%d�Ŕ����Ă�낤�B",
                    "          It sells for $%d."), price), 7, 10);
		prt(_("�@�@�@�^�ǂ����A�ȃJ�[�h��������Α�ׂ����Ă킯���B",
            "          If you're lucky, you might obtain rare cards and make lots of money."), 8, 10);
		prt(_("�@�@�@���_�A���̒��g�ɂ������������������Ȃǂ����Ⴂ�Ȃ��B�v",
            "          Naturally, I haven't touched the contents of the box.'"), 9, 10);
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt(_("�@�@�@ ", "       "), 10, 10);
			prt(format(_("�@�@�u�����Ă��O�͍K�^���B���񂾂��͑�T�[�r�X�́�%d�ɂ��Ă��B�v",
                        "          'You're in luck. For your first purchase, it's going to cost only $%d.'"), price), 11, 10);
		}
	}



	if (!get_check_strict(_("�����܂����H", "Do you want to buy it?"), CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}
	clear_bldg(4, 18);
	if (p_ptr->au < price)
	{
		msg_print(_("����������Ȃ��B", "You don't have enough money."));
		return;
	}


	if (p_ptr->pclass == CLASS_SANNYO)
	{
		prt(_("���Ȃ��͔����w�������B", "You purchase a box."), 5, 10);
	}
	else if (!buy_gacha_box_count)
	{
		prt(_("�R�@�u����Ƃ����ۛ��ɂȁB�ӂ��ӂ��Ӂc�c�v",
            "Sannyo - 'I'll be pleased to work with you in the future. Fufufu...'"), 5, 10);
	}
	else
	{
		if (buy_gacha_box_count > 10 && one_in_(4))prt(_("�R�@�u�ڂ��������Ă���ȁB�ꕞ�ǂ����H�v",
                                                        "Sannyo - 'Your eyes are all bloodshot. How about taking a break?'"), 5, 10);
		else if(one_in_(10))	prt(_("�R�@�u���̏����͎R�̏�̂ق��̘A�����ɂ܂�邩�������B���߂ɔ������ق����������H�v",
                                    "Sannyo - 'The higher-ups on the mountain might take displeasure with this business. Can you buy faster?'"), 5, 10);
		else if(one_in_(7))		prt(_("�R�@�u�_���W�����̉��ŊJ�����烌�A�J�[�h���o�₷���H����Ȗ�Ȃ����낤���B�v",
                                        "Sannyo - 'Is it easier to get rare cards if you open the box deep in the dungeon? No, I don't think so.'"), 5, 10);
		else if(one_in_(3))		prt(_("�R�@�u�����J�[�h�������Ă���Ƃ����ȁH�v",
                                        "Sannyo - 'Are you looking to get some good cards?'"), 5, 10);
		else 					prt(_("�R�@�u�悵�A���x����B�v", "Sannyo - 'Please come again.'"), 5, 10);

	}
	buy_gacha_box_count++;
	//�J�[�h�̍��v�x���z�ɉ��Z
	//if(total_pay_cardshop < 100000000) total_pay_cardshop += price;
	add_cardshop_profit(price);


	object_prep(o_ptr, lookup_kind(TV_CHEST, SV_CHEST_10_GACHA));
	o_ptr->pval = -6; //(��{���̔� )

	object_desc(o_name, o_ptr, 0);

	inven_carry(o_ptr);
	msg_format(_("%s���󂯎�����B", "You receive %s."), o_name);

	p_ptr->au -= price;
	building_prt_gold();

}


/*:::���J���@�X�|�C���g�����Ώەi�ڃe�[�u��*/
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

	{0,0,0}//�I�[�p�_�~�[
};

/*:::���J���@�X�|�C���g�����i�i�e�[�u��*/
struct marisa_store_type marisa_present_table[] =
{
	{TV_CHEST,SV_CHEST_MARIPO_01 , 20},//�ω΂̖�
	{TV_CHEST,SV_CHEST_MARIPO_02 , 20},//�ϗ�̖�
	{TV_CHEST,SV_CHEST_MARIPO_03 , 30},//�X�s�[�h�̖�
	{TV_CHEST,SV_CHEST_MARIPO_04 , 40},//��Ηn�𖂖@�_
	{TV_CHEST,SV_CHEST_MARIPO_05 , 50},//�e���|�A�E�F�C���@�_
	{TV_CHEST,SV_CHEST_MARIPO_06 , 70},//�ϐ��̖�
	{TV_CHEST,SV_CHEST_MARIPO_07 , 100},//�̗͉񕜂̖�
	{TV_CHEST,SV_CHEST_MARIPO_08 , 150},//�[�ւ̖�

	{TV_CHEST,SV_CHEST_MARIPO_09 , 200},//�X�s�[�h�̏�
	{TV_CHEST,SV_CHEST_MARIPO_10 , 300},//�X�̎w��
	{TV_CHEST,SV_CHEST_MARIPO_11 , 500},//�h���S���E�u���X�̖��@�_
	{TV_CHEST,SV_CHEST_MARIPO_12 , 700},//�ϓł̎w��
	{TV_CHEST,SV_CHEST_MARIPO_13 , 800},//�Ӓ�̃��b�h
	{TV_CHEST,SV_CHEST_MARIPO_14 , 1000},//�j��̏�
	{TV_CHEST,SV_CHEST_MARIPO_15 , 1200},//�����̖I����
	{TV_CHEST,SV_CHEST_MARIPO_16 , 1500},//���G�̖�

	{TV_CHEST,SV_CHEST_MARIPO_17 , 2000},//���̗͂��̏�
	{TV_CHEST,SV_CHEST_MARIPO_18 , 2500},//�T�C�o�[�f�[�����̐l�`
	{TV_CHEST,SV_CHEST_MARIPO_19 , 4000},//�d���i75�K���������h���b�v)
	{TV_CHEST,SV_CHEST_MARIPO_20 , 5000},//�k���m�[����
	{TV_CHEST,SV_CHEST_MARIPO_21 , 6000},//�\�E���\�[�h
	{TV_CHEST,SV_CHEST_MARIPO_22 , 10000},//���[�G���O�����Z
	{TV_CHEST,SV_CHEST_MARIPO_23 , 15000},//���̉H��
	{TV_CHEST,SV_CHEST_MARIPO_24 , 30000},//�����I�[���i�v�t�^

	{0,0,0}//�I�[�p�_�~�[
};


/*:::���J���@�X�ɂĖ��������󂯎��A�C�e���𔻒�*/
///mod150811 �������E�Ƃ̎��̖��͒��ohook�ɂ��g��
bool marisa_will_buy(object_type *o_ptr)
{
	int i;
	if(p_ptr->pclass == CLASS_MARISA && o_ptr->tval < TV_BOOK_END) return FALSE;

	for(i=0;marisa_wants_table[i].tval;i++)
	{
		if(o_ptr->tval == marisa_wants_table[i].tval && o_ptr->sval == marisa_wants_table[i].sval) return TRUE;
	}

	//v2.0.13 �ɐ��Z�����z���悷�邱�Ƃɂ���B�E�Ɩ������̂Ƃ��̖��͒��o�̑Ώۂɂ͂Ȃ�Ȃ��B
	if (p_ptr->pclass != CLASS_MARISA && o_ptr->name1 == ART_IBUKI) return TRUE;

	return FALSE;

}
//EX�����p�������g���[�h�A�C�e��hook
bool marisa_will_trade(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ITEMCARD) return TRUE;
	if(marisa_will_buy(o_ptr)) return TRUE;
	return FALSE;
}



//EX�����p�ޖ��g���[�h�A�C�e��hook
bool yuma_will_trade(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD) return TRUE;

	if (o_ptr->tval == TV_SWEETS) return TRUE;

	if (o_ptr->tval == TV_ALCOHOL) return TRUE;

	if (o_ptr->tval == TV_MUSHROOM) return TRUE;

	if (o_ptr->tval == TV_FLASK) return TRUE;//���قƃG���W�j�A�p�G�l���M�[�p�b�N

	if (o_ptr->name1 == ART_IBUKI) return TRUE;//�ɐ��Z

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG) return TRUE; //����ȗ�

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU) return TRUE; //�����̚�

	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI) return TRUE; //�����S�̒|

	return FALSE;
}

static void building_prt_maripo(void)
{
	char tmp_str[80];
	prt("MARIPO: ", 22,53);

	sprintf(tmp_str, "%7ld", (long)p_ptr->maripo);
	prt(tmp_str, 22, 63);
}

/*:::�A�C�e����n���|�C���g���擾����*/
///mod150219 ����ɖ������p���͕ϐ��ɉ��Z
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

	q = _("���������Ă����񂾁H", "What do you have?");
	s = _("���̃U�b�N�̒��Ɏ����~�������͂Ȃ����B", "You don't have anything that I want.");

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

	//v2.0.13 �ɐ��Z��1500MARIPO�ƌ������Ă����悤�ɂ���
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

	if(base_point < 30) msg_format(_("�u���̒��x�̕����B�܂�%d�|�C���g���ď����ȁB�v",
                                    "'Something like that? Well, it's worth %d points.'"),total_point);
	else if(base_point < 300) msg_format(_("�u�ق��A���X�̕��������Ă����ȁB%d�|�C���g�o�����B�v",
                                            "'Hmm, looks interesting. I'll give you %d points.'"),total_point);
	else if(base_point < 1000) msg_format(_("�u���Ȃ�̒l�ł����̂���Ȃ����B%d�|�C���g�łǂ����H�v",
                                            "'That looks very valuable! How about %d points?'"),total_point);
	else msg_format(_("�u�䂸���Ă��ꂽ�̂ށI%d�|�C���g�o���Ă��������I�v",
                    "'Give that to me, I beg you! I'll give %d points!'"),total_point);

	if(!get_check(format(_("%s��n���܂����H", "Hand over %s?"),o_name))) return;

	p_ptr->maripo += total_point;
	if(p_ptr->maripo > 999999) p_ptr->maripo = 999999;
	building_prt_maripo();
	msg_format(_("�u�悵�A���k���������I�v", "'Alright, it's a deal!'"));
	msg_format(_("%d�|�C���g��MARIPO���l�������B", "You obtained %d MARIPO."),total_point);

	///mod150219 ���������͊l��
	//v2.0.13 �ɐ��Z��n�����Ƃ��ɂ͖��͂𓾂��Ȃ��͂�
	marisa_gain_power(q_ptr, 100);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);

}

/*:::�|�C���g������v���[���g���󂯎��*/
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
		msg_print(_("�u���������A�ו�����t�����B�v",
                    "'Hey, your backpack is already full.'"));
		return;
	}

	while(1)
	{

		clear_bldg(5, 18);
		c_put_str(TERM_WHITE,_("�������u�����̂����ڂɉ����āA�ǂ�ł��D���Ȃ��̂�I��ł���I�v",
                            "Marisa - 'Thank you for your patronage! Pick the one you like!'"),5 , 6);
		c_put_str(TERM_WHITE,_("(a�`h:�v���[���g��I�� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",
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
			if( ( (1L << table_num) & (p_ptr->maripo_presented_flag))) c_put_str(color,format(_("%c) [   �i�؂�   ] %s",
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
				msg_format(_("�u����͂����i�؂ꂾ���B�v", "'This one is already sold out.'"));
				continue;
			}
			else if(p_ptr->maripo < marisa_present_table[choice].maripo)
			{
				msg_format(_("�u�����ȁB���ꂪ�~�����Ȃ�����ƃ|�C���g�𒙂߂Ă���B�v",
                            "'Sorry, but you have to save up more points for this one.'"));
				continue;
			}

			if (get_check_strict(_("�u���ꂪ�~�����̂��H�v", "'Do you want this?'"), CHECK_DEFAULT_Y))
			{
			 	break;
			}
		}
	}

	p_ptr->maripo_presented_flag |= (1L << choice);
	p_ptr->maripo -= marisa_present_table[choice].maripo;
	building_prt_maripo();

	object_prep(o_ptr, lookup_kind(marisa_present_table[choice].tval,marisa_present_table[choice].sval) );
	if(o_ptr->tval == TV_CHEST) o_ptr->pval = -6; //(��{���̔� )
	else apply_magic(o_ptr,50,AM_NO_FIXED_ART); //���̂Ƃ��딠�����v���[���g���Ȃ����O�̂���
	object_desc(o_name, o_ptr, 0);

	slot_new = inven_carry(o_ptr);
	msg_format(_("%s���󂯎�����B", "You have received %s."),o_name);
	autopick_alter_item(slot_new, FALSE);
	handle_stuff();
	clear_bldg(5, 18);

}



/*:::�������֒��i�𔄂���邽�߂̕\�@���J���@�X�̃e�[�u���𗬗p TV,SV,������艿�i*/
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

	{0,0,0}//�I�[�p�_�~�[
};

/*:::�������ɂĒ��i�Ƃ��Ĉ��������A�C�e���𔻒�*/
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
//�������ɒ��i�Ȃǂ𔄂����Ƃ��̉��i
//�ޖ����H�ׂ��Ƃ��̉h�{�v�Z�ɂ��g�������Ȃ����̂Ŋ֐��𕪂���
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


/*::�������ɒ��i�𔄂����*/
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

	q = _("���������[�����̂������Ă���悤���ˁH",
        "Looks like you have something interesting, right?");
	s = _("���ɋ����������悤�Ȃ��̂͂Ȃ��悤���ˁB",
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

	if(base_point < 10000) msg_format(_("�u����قǒ��������̂ł��Ȃ��ȁB�܂� $%d�Ƃ������Ƃ��납�B�v",
                                        "'It's not that rare. Well, I'll give you $%d.'"),total_point);
	else if(base_point < 100000) msg_format(_("�u�ӂށA�Ȃ��Ȃ��̕i���B$%d�o�����B�v",
                                            "'Hmm, looks pretty good. Let's settle at $%d.'"),total_point);
	else if(base_point < 1000000) msg_format(_("�u�ق��A���ɋ����[����i���B$%d�ň�����点�Ăق����B�v",
                                            "'Ohh, that looks very interesting. I'll take it for $%d.'"),total_point);
	else
	{
		msg_format(_("�u�n���ȁA����ȕ������E���E�E�I",
                    "'It can't be! Just to think that something like that..."));
		msg_print(NULL);
		msg_format(_("���⎸�炵���B������ɂ�$%d���x�����p�ӂ�����B�v",
                    "...I'm sorry. I'm ready to pay $%d.'"),total_point);
	}

	if(!get_check(format(_("%s��n���܂����H", "Hand over %s?"),o_name))) return;

	p_ptr->au += total_point;
	building_prt_gold();
	msg_format(_("�u�悵�A���k�������B�v", "'Alright, it's a deal.'"));
	msg_format(_("$%d�Ŕ��蕥�����B", "You sold it for $%d."),total_point);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);

}






/*:::���Z�ꏈ���B�����X�^�[��I�肵�Đ�킹��Ƃ���܂ŁH*/
///sysdel tougi inside_battle�͖ʓ|�Ȃ̂Ŏc�������̊֐��͍폜����\��
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
		msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
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
		prt("�����X�^�[                                                     �{��", 4, 4);
#else
		prt("Monsters                                                       Odds", 4, 4);
#endif
		for (i=0;i<4;i++)
		{
			char buf[80];
			monster_race *r_ptr = &r_info[battle_mon[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld�{", i+1, format("%s%s",r_name + r_ptr->name, (r_ptr->flags1 & RF1_UNIQUE) ? "���ǂ�" : "      "), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", (r_ptr->flags1 & RF1_UNIQUE) ? "Fake " : "", r_name + r_ptr->name), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("�ǂ�ɓq���܂���:", 0, 0);
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
sprintf(tmp_str,"�q���� (1-%ld)�H", (long int)maxbet);
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
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
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
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.",(long int) maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h�ł������B");
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


/*:::�{���̏܋����\������B���肷��̂͂����ł͂Ȃ�*/
///del131221 �܋���폜
#if 0
static void today_target(void)
{
	char buf[160];
	monster_race *r_ptr = &r_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "�{���̏܋���", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"�^�[�Q�b�g�F %s",r_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",r_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"���� ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"��   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
	p_ptr->today_mon = today_mon;
}
#endif

///del131231 �܋���폜
#if 0
static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "��l����̑�`�����X�I�I�I", 5, 10);
c_put_str(TERM_YELLOW, "�^�[�Q�b�g�F���̒��b�u�c�`�m�R�v", 6, 10);
c_put_str(TERM_WHITE, "�����߂� ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "����     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "��       ----   $100,000", 10, 10);
#else
c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
c_put_str(TERM_YELLOW, "target: the rarest animal 'Tsuchinoko'", 6, 10);
c_put_str(TERM_WHITE, "catch alive ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "corpse      ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}
#endif

///del131231 �܋���폜
#if 0

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("���̂������A��Ε�V�������グ�܂��B",4 ,10);
c_put_str(TERM_YELLOW, "���݂̏܋���", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse",4 ,10);
c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		byte color;
		cptr done_mark;
		/*:::���Ɋ����ς݂̏ꍇidx��+10000����Ă���炵��*/
		monster_race *r_ptr = &r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])];

		if (kubi_r_idx[i] > 10000)
		{
			color = TERM_RED;
#ifdef JP
			done_mark = "(��)";
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
			prt("�����L�[�������Ă�������", 0, 0);
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
/*:::�܋��������A�C�e���Ɉ���������*/
///del131221 ���̍폜�ɔ������������폜
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
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(1000000L * o_ptr->number));
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
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(200000L * o_ptr->number));
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
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(100000L * o_ptr->number));
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
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
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
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
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
				sprintf(buf, "%s��n���܂����H",o_name);
#else
				sprintf(buf, "Hand %s over? ",o_name);
#endif
				if (!get_check(buf)) continue;

#if 0 /* Obsoleted */
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
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
				msg_format("����ō��v %d �|�C���g�l�����܂����B", num);
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
				msg_format("%s(%c)�������B", o_name, index_to_label(item_new));
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
		msg_print("�܋��𓾂�ꂻ���Ȃ��͎̂����Ă��Ȃ������B");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}
#endif


/*:::����������Ƃ��ɏo�Ă��郂���X�^�[�@�G���h���b�`�z���[������I�o*/
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

/*:::�������Ƃ��Ɉ��������鏈���@�������[�h�ŏh���Ŏ~�܂����薰�炳�ꂽ�薃Ⴢ����Ƃ�*/
void have_nightmare(int r_idx)
{
	bool happened = FALSE;
	monster_race *r_ptr = &r_info[r_idx];
	int power = r_ptr->level + 10;
	char m_name[80];
	cptr desc = r_name + r_ptr->name;

	if(p_ptr->pclass == CLASS_DOREMY)
	{
		msg_print(_("���Ȃ��͈�������������������B",
                    "You've enjoyed the delicious nightmare."));
		(void)set_food(PY_FOOD_MAX - 1);
		return;
	}

	if(r_idx == MON_CLOWNPIECE || weird_luck())
	{
		msg_print(_("�����u�M���̈����͎����������������܂����B�v",
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
			msg_format(_("���̒��ŏ����E���̒e���Ɉ͂܂ꂽ�B",
                        "You dream of running into a blind run killer danmaku pattern."), m_name);
		else
			msg_format(_("���̒���%s�ɒǂ�������ꂽ�B",
                        "You dream of being pursued by %s."), m_name);

		/* Safe */
		return;
	}

	if (p_ptr->image)
	{
		/* Something silly happens... */
#ifdef JP
		msg_format("%s%s�̊�����Ă��܂����I",
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
	msg_format("%s%s�̊�����Ă��܂����I",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

	///mod140126 �������������C�Ɖu�Ƒϐ��܂߂ď����Ȃ�����
#if 0
	if (!p_ptr->mimic_form)
	{
		///race �����������Ƃ��̃G���h���b�`�z���[�ϐ�
		///sys �G���h���b�`�z���[�ϐ��@������ϐ��̈�Ƃ��Ĕ��肷��\��
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
	///mod131228 ���C�ϐ��ň����ɒ�R
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

	///mutation �����������Ƃ��̈����ˑR�ψ�
	/* Amnesia */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (lose_all_info())
		{
#ifdef JP
msg_print("���܂�̋��|�ɑS�Ă̂��Ƃ�Y��Ă��܂����I");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}
	//�H���l�͕ψٖ���
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
msg_print("���Ȃ��͊����Ȕn���ɂȂ����悤�ȋC�������B����������͌��X�������B");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("���Ȃ��͊����Ȕn���ɂȂ����I");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (p_ptr->muta3 & MUT3_HYPER_INT)
					{
#ifdef JP
msg_print("���Ȃ��̔]�͐��̃R���s���[�^�ł͂Ȃ��Ȃ����B");
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
msg_print("���Ȃ��͕����|�ǂɂȂ����I");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (p_ptr->muta3 & MUT3_FEARLESS)
					{
#ifdef JP
msg_print("���Ȃ��͂�������m�炸�ł͂Ȃ��Ȃ����B");
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
msg_print("���o���Ђ��N�������_�����Ɋׂ����I");
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
msg_print("����Ȋ���̔���ɂ�������悤�ɂȂ����I");
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
/*:::�h���R�}���h*/
///sys �h���@�H�����ɐH���s�́��������Ŕ��ʂ��ă��b�Z�[�W�o���悤�ɂ���\��
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
				|| p_ptr->prace == RACE_ANIMAL_GHOST//v1.1.85�@�Y��Ă��̂Œǉ�
				|| p_ptr->prace == RACE_HANIWA
				|| p_ptr->pclass == CLASS_CHIYARI
				)
			{
				msg_print(_("���Ȃ������̐H���͂����ɂ͂Ȃ��悤���B",
                            "Doesn't look like there's food suitable for you here."));
				return FALSE;
			}
			else if(p_ptr->pclass == CLASS_DOREMY)
			{
				msg_print(_("���ʂ̐H���ɂ͋������Ȃ��B�܂��h�ɔ��܂낤�c",
                            "You're not interested in ordinary food. Let's stay at the inn first..."));
				return FALSE;
			}

			//Ex���[�h�̃~�X�e�B�A�̉���ɂ���Ƃ�
			if(EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_MYSTIA)
			{
				if(p_ptr->food < PY_FOOD_FULL -1 || p_ptr->blind || p_ptr->pclass == CLASS_YUMA)
				{
					msg_print(_("�X��͔��ډV�̊��Ă���U�����Ă��ꂽ�B",
                                "The owner gives you some roast lamprey."));
					(void)set_food(PY_FOOD_MAX - 1);
					set_blind(0);
					//v2.0.10�ǉ�
					set_tim_invis(5000, FALSE);

					break;
				}
				else
				{
					msg_print(_("�X��͈�Ȕ�I���Ă��ꂽ�B",
                                "The owner shows off and signs a song."));
					if(!p_ptr->resist_blind) set_blind(p_ptr->blind + 50 + randint1(50));
					return FALSE;
				}

			}
			//v1.1.53 Ex���[�h�̃����o�̕����ɂ���Ƃ�
			if (EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_LARVA)
			{
				if (p_ptr->food < PY_FOOD_FULL - 1 )
				{
					msg_print(_("���Ȃ��͂����Ƃ��َq���y���񂾁B",
                                "You enjoy the tea and sweets."));
					(void)set_food(PY_FOOD_MAX - 1);
					break;
				}

			}

			if (p_ptr->food >= PY_FOOD_FULL && p_ptr->pclass != CLASS_YUMA)
			{
				msg_print(_("���͖������B", "You are already full."));

				return FALSE;
			}

			if(EXTRA_MODE)
				msg_print(_("���Ȃ��͎葁���H�����ς܂����B", "You quickly finish your meal."));
			else
				msg_print(_("�X��͗����ƈ��ݕ���U�����Ă��ꂽ�B",
                        "The owner serves you food and drink."));

			(void)set_food(PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			if ((p_ptr->poisoned) || (p_ptr->cut))
			{
#ifdef JP
				msg_print("���Ȃ��ɕK�v�Ȃ͕̂����ł͂Ȃ��A���Î҂ł��B");
#else
				msg_print("You need a healer, not a room.");
#endif

				msg_print(NULL);
#ifdef JP
				msg_print("���݂܂���A�ł������ŒN���Ɏ��Ȃꂿ�።��܂���ŁB");
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
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�h���ɔ��܂����B");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stayed at the inn.");
#endif
				turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);

				if(p_ptr->muta3 & MUT3_BYDO) p_ptr->bydo += turn - oldturn;
				if(p_ptr->bydo > 100000 * 3) p_ptr->bydo = 100000 * 3+1;

				//v1.1.59 �T�j�[�͖���ƒ~�ς�������������
				sunny_charge_sunlight(-1*(SUNNY_CHARGE_SUNLIGHT_MAX));

				if (dungeon_turn < dungeon_turn_limit)
				{
					dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK * 250);
					if (dungeon_turn > dungeon_turn_limit) dungeon_turn = dungeon_turn_limit;
				}

				//v2.0.6 �ޖ������Ă���\�͂�ϐ����h�ɔ������Ƃ������鏈��
				if (p_ptr->pclass == CLASS_YUMA && (turn - oldturn) >= YUMA_FLAG_DELETE_TICK)
				{
					yuma_lose_extra_power((turn - oldturn) / YUMA_FLAG_DELETE_TICK);
				}

				prevent_turn_overflow();

				//�h�ɔ��܂��ē��t���ς��Ƃ�
				if ((prev_hour >= 18) && (prev_hour <= 23))
				{
					do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

					p_ptr->today_mon = 0; //�얲�̐肢���Z�b�g

					//v2.05 �͂��Ă̐l�T�����Z�b�g EXTRA�ł͓����o�߂łȂ��t���A�ʉ߂Ń��Z�b�g�����̂ł����ł͉������Ȃ�
					if (!EXTRA_MODE)
					{
						p_ptr->hatate_mon_search_ridx = 0;
						p_ptr->hatate_mon_search_dungeon = 0;

					}

				}

				p_ptr->chp = p_ptr->mhp;

				engineer_malfunction(0, 5000); //�G���W�j�A�̕s���̋@�B���񕜂���

				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print(_("�����X�b�L�������I�ł������Y��Ă���悤�ȁB",
                                                                        "Your head feels refreshed! But you feel like you've forgotten something."));

				if (ironman_nightmare && !diehardmind() && p_ptr->pclass != CLASS_DOREMY)
				{
#ifdef JP
					msg_print("����ɏA���Ƌ��낵�����i���S���悬�����B");
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
					msg_print("���Ȃ��͐⋩���Ėڂ��o�܂����B");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����ɂ��Ȃ���Ă悭����Ȃ������B");
#else
					msg_print("You awake screaming.");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
				}
				else
				{
					///mod150920 �h���~�[���ꏈ��
					if(p_ptr->pclass == CLASS_DOREMY)
					{
						if(ironman_nightmare)
							msg_print(_("�h���q�݂̂閲�͂ǂ���ɏ�̈������I",
                                        "All the other guests are having delicious nightmares!"));
						else
							msg_print(_("���Ȃ��͑��̏h���q�̖�����������������B",
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

					//v1.1.86 �h���ɘA���Ŕ��܂�����process_world()��6:00/18:00�̏�����ʂ�Ȃ����Ƃ�����̂ł����ɂ������Ƃ�
					//v1.1.87�@�g���[�h�J�E���g���Z�b�g��6:00��18:00�̗����ōs�����Ƃɂ����̂ŕs�v�ɂȂ����B
					//�A���ŏh���ɔ��܂����獂���x�Z�b�g���h����o���Ƃ��̈�x�����ɂȂ��Ă��܂����܂����͂Ȃ����낤�B
					//�����s��g���[�h�J�E���g�����Z�b�g
					//ability_card_trade_count = 0;

					if ((prev_hour >= 6) && (prev_hour <= 17))
					{
						msg_print(_("���Ȃ��̓��t���b�V�����Ėڊo�߁A�[�����}�����B",
                                    "You wake up refreshed in the evening."));
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("�[�����}�����B", "woke up in evening."));

					}
					else
					{
						msg_print(_("���Ȃ��̓��t���b�V�����Ėڊo�߁A�V���ȓ����}�����B",
                                    "You wake up refreshed and welcome a new day."));
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("�����������������}�����B", "had a refreshing morning."));


					}

					set_asthma(p_ptr->asthma - 6000);
				}

				//v1.1.29 �h�ɔ��܂����琌�����o�߂邱�Ƃɂ���
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



//v1.1.85 �ꕔ�N�G�X�g�̐l�ԑ�����/�d�������͂̕���̂��߂ɒǉ��B�O�����l�ԑ����͂̂Ƃ�TRUE
//���̍��d���l�b�g���[�N��H�ߍ����N�G�̐l�ԑ�����Ƃ͈Ⴄ�̂Œ���
//�u�l�Ԃ��猩�Đl�ԑ��Ɍ�����v�Ɓu�d�����猩�Đl�ԑ��Ɍ�����v�͔����ɈႤ�̂ŔY�܂������A�Ƃ肠�����l���̈�ʐl���猩�Ē��Ԃ��ۂ����TRUE
//TODO:�u�l�ԑ��Ɍ�����v�Ɓu�d�����Ɍ�����v�𗼕��������푰��L���������邵���̂����d��������p�̊֐���ʂɍ��ׂ����������
bool	player_looks_human_side()
{

	//�ϐg��
	//�l�Ԃɉ����Ă���ꍇ�͒ʂ�A����ȊO�ɉ����Ă���Ƃ��ʂ�Ȃ����Ƃɂ��Ă݂�B
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

	//���̎푰����̗�O �l���Ŗ������Ƃ��Ė����ʂ��Ă����ȖʁX�Ɣ������Ă����ȖʁX

	if (p_ptr->pclass == CLASS_BYAKUREN) return TRUE;
	if (p_ptr->pclass == CLASS_SHOU) return TRUE;
	if (p_ptr->pclass == CLASS_KEINE) return TRUE;
	if (p_ptr->pclass == CLASS_AUNN) return TRUE;
	if (p_ptr->pclass == CLASS_UDONGE) return TRUE;
	if (p_ptr->pclass == CLASS_TEWI) return TRUE;

	if (p_ptr->pclass == CLASS_HINA) return FALSE;
	if (p_ptr->pclass == CLASS_SHION) return FALSE;

	//�푰�ɂ���{����
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
/*:::�N�G�X�g�̕��͂�\������*/
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

	/*:::�Y���N�G�X�g�̏�����́B�t���O��INIT_SHOW_TEXT�Ȃ̂�quest_text[][]�̂ݍX�V�����H*/
	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	///mod140512 ���ꃋ�[�`���ǉ��@��������̃��j�[�N�v���C���[�̂Ƃ��N�G�X�g���͂��s���R�ɂȂ�̂ŏ���������
	(void) check_quest_unique_text();

	/* Reset the old quest number */
	p_ptr->inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "�N�G�X�g��� (�댯�x: %d �K����)", quest[questnum].level);
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


/*:::���ۉ��O�w��@�N�G�X�g�p*/
static bool item_tester_hook_kyomaru(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_HEMATITE)
		return (TRUE);
	else
		return (FALSE);
}
/*:::�~�X�����w��@�N�G�X�g�p*/
static bool item_tester_hook_mithril(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_MITHRIL)
		return (TRUE);
	else
		return (FALSE);
}

/*:::�Ă����V�@�N�G�X�g�p*/
static bool item_tester_hook_eel(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_EEL)
		return (TRUE);
	else
		return (FALSE);
}

/*:::���ʐ��̉H�߁@�N�G�X�g�p*/
static bool item_tester_hook_special_hagoromo(object_type *o_ptr)
{
	if (o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_CLOAK_HAGOROMO && object_is_artifact(o_ptr) && !object_is_cursed(o_ptr))
		return (TRUE);
	else
		return (FALSE);
}

/*:::�������w��@�N�G�X�g�p*/
static bool item_tester_hook_alcohol(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ALCOHOL && o_ptr->sval == SV_ALCOHOL_MARISA)
		return (FALSE);
	if (o_ptr->tval == TV_ALCOHOL && o_ptr->pval >  6000)
		return (TRUE);
	else
		return (FALSE);
}

/*:::�P��܂̓��@�N�G�X�g�p*/
///mod151219 �S���B���������Ƃɋ��t���ǉ�
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

/*:::����������̎��@�N�G�X�g�p*/
static bool item_tester_hook_hangoku(object_type *o_ptr)
{
	if (o_ptr->name1 == ART_HANGOKU_SAKE)
		return (TRUE);
	else
		return (FALSE);
}

/*:::�A�C�e���T���N�G�X�g�̑ΏۃA�C�e���m�F���[�`���B
 *:::�ΏۃA�C�e���������̎��⁚�����Ȃǂ��l�����q_info.txt���g���͖̂ʓ|�Ȃ̂ł��̒��Ƀn�[�h�R�[�f�B���O���Ă��܂��B*/
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
		msg_format(_("ERROR:check_quest_deliver_item()�ɃN�G�X�gNo.(%d)���w�肳��Ă��Ȃ�",
                    "ERROR: Quest number (%d) isn't set in check_quest_deliver_item()"),q_index);
		return FALSE;
	}
	q = _("�A�C�e����I�����Ă��������B", "Choose an item.");
	s = _("���Ȃ��͋��߂�ꂽ���̂������ė��Ă��Ȃ��B", "You haven't brought what you were asked for.");

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return FALSE;
	o_ptr = &inventory[item];
	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if(o_ptr->number > 1)
	{
		if(!get_check(format(_("%s�����n���܂����H", "Hand over one of %s?"),o_name))) return FALSE;
	}
	else
	{
		if(!get_check(format(_("%s��n���܂����H", "Hand over %s?"),o_name))) return FALSE;
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



//���N�U�푈2�N�G�X�g�Ń^�[�Q�b�g��I��
//�I�������^�[�Q�b�g��r_idx��Ԃ��B�L�����Z����0
//castle_quest()�ŃN�G�X�g��̕��͂��o�Ă���Ƃ��Ɏ��s����ǉ��̑I������\������
int	select_quest_yakuza2_target()
{
	char c;
	int i;
	int r_idx;

	bool flag_enoko = TRUE;
	bool flag_biten = TRUE;
	bool flag_chiyari = TRUE;

	//�I�������玩���Ɋ֌W���鐨�͂����O
	//�V����3�l
	if (p_ptr->pclass == CLASS_ENOKO) flag_enoko = FALSE;
	if (p_ptr->pclass == CLASS_BITEN) flag_biten = FALSE;
	if (p_ptr->pclass == CLASS_CHIYARI) flag_chiyari = FALSE;

	//������̏���
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA)  flag_enoko = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU)  flag_biten = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU)  flag_chiyari = FALSE;

	//������߈�
	if (p_ptr->muta4 & (MUT4_GHOST_WOLF)) flag_enoko = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_OTTER)) flag_biten = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_EAGLE)) flag_chiyari = FALSE;

	//���N�U�N�G�X�g1�̂Ƃ��̑I����
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA) flag_enoko = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU) flag_biten = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU) flag_chiyari = FALSE;

	//���łɓ|���Ă����珜�O(�ߋ��o�[�W�����œ|���Ă���o�[�W�����A�b�v�����Ƃ�)
	if(r_info[MON_ENOKO].r_akills) flag_enoko = FALSE;
	if (r_info[MON_BITEN].r_akills) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].r_akills) flag_chiyari = FALSE;

	//�����N�G�Ώۂ������珜�O
	//v2.0.13�ȍ~�͂��̎O�l�̓����N�G�I�肳��Ȃ��B�ߋ��o�[�W�����Ń����N�G�I�肳��ē|�����Ƀo�[�W�����A�b�v�����Ƃ��̂��߂̏���
	if (r_info[MON_ENOKO].flags1 & RF1_QUESTOR) flag_enoko = FALSE;
	if (r_info[MON_BITEN].flags1 & RF1_QUESTOR) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].flags1 & RF1_QUESTOR) flag_chiyari = FALSE;

	//�^�[�Q�b�g���c��Ȃ��ꍇ�L�����Z��
	if (!flag_enoko && !flag_biten && !flag_chiyari)
	{
		c_put_str(TERM_WHITE, _("���������Ȃ��̎󂯂�ꂻ���Ȏ�z���͎c���Ă��Ȃ������B", "However, there are no suitable wanted person posters left for you."), 15, 0);
		c_put_str(TERM_WHITE, _("���̃N�G�X�g����̂��邱�Ƃ͂ł��Ȃ��悤���B", "You are unable to accept this quest."), 16, 0);
		return 0;
	}

	//�c�����^�[�Q�b�g���ꗗ�\��
	c_put_str(TERM_WHITE, _("�N��_���܂����H(ESC:�L�����Z��)", "Which target do you pick? (ESC: cancel)"), 14, 2);
	if (flag_enoko)
		c_put_str(TERM_WHITE, _("a) �w�O�� �d�m�q�x  ($50,000+�r�̖͂�)",   "a) Enoko Mitsugashira ($50,000 + Potion of Strength)"), 15, 5);
	if (flag_biten)
		c_put_str(TERM_WHITE, _("b) �w�� ���V�x      ($60,000+�m�\�̖�)",   "b) Son Biten          ($60,000 + Potion of Intelligence)"), 16, 5);
	if (flag_chiyari)
		c_put_str(TERM_WHITE, _("c) �w�V�ΐl �����x($70,000+�ϋv�̖͂�)", "c) Chiyari Tenkajin   ($70,000 + Potion of Constitution)"), 17, 5);

	//�^�[�Q�b�g��I��
	//�^�[�Q�b�g�������鐨�͂��甽���𔃂��t���O���L�^
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

	//�I�����������X�^�[�ɏ܋���t���O�𗧂āA���łɓ|���Ă��畜��������
	r_info[r_idx].flags3 |= RF3_WANTED;
	if (!r_info[r_idx].max_num)
	{
		if (cheat_hear) msg_print(_("�I�����������X�^�[���œ|�ς݂Ȃ̂ŕ���������", "The chosen monster was already defeated, and is now reborn"));
		r_info[r_idx].max_num = 1;
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, _("���Ȃ��͎�z���̈ꖇ����Ɏ�����B", "You take one of the wanted person posters."), 5, 5);


	return r_idx;

}


/*
 * Request a quest from the Lord.
 */
/*:::�������ł̃N�G�X�g�֘A����*/
static void castle_quest(void)
{
	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	/*:::�N�G�X�g�C���f�b�N�X�𓾂�B*/
	q_index = cave[py][px].special;

	//v2.0.13 �������o�Omemo
	//���N�U�푈2�N�G�X�g(QUEST_YAKUZA2)�őI�������܋��񂪃����N�G�Ώۂł��������ꍇ�A
	//�����N�G�t���A�ɍs���ĊY�������X�^�[��|���Ɨ����̃N�G�X�g��������ԂɂȂ�̂����A
	//���̌ツ�N�U�푈2�N�G�X�g�̌����ɗ���ƂȂ���q_index��0�ɂȂ��ĕ�V���������ɍs���Ȃ��B
	//�����N�G�t���A�ȊO�œ|�����炿���Ɗ�������B
	//�����ǂ��Ȃ�����t0000014.txt�ɒ��ł����ꂽ�����ɉe�����o��̂��S���s���B
	//���ɕs�C�������Ƃ肠���������N�G�I��ς݂̓G�͏܋���ɑI���ł��Ȃ��悤�ɂ��Ă���
	if(cheat_xtra)msg_format("idx:%d", q_index);

	//v1.1.85
	// -Mega Hack- �Ǔے��N�G�X�g(�l�ԗp)��[��Ɏ󂯂���Ǔے��N�G�X�g(�d���p)�ɂȂ鏈��
	if (q_index == QUEST_GEIDON_HUMAN && is_midnight()) q_index = QUEST_GEIDON_KWAI;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
put_str("�������Ŏ󂯂���N�G�X�g�͂Ȃ��悤���B", 8, 0);
#else
		put_str("It doesn't look like you can receive a quest here right now.", 8, 0);
#endif

		return;
	}

	//v1.1.18�ȍ~�A����̃N�G�X�g�͓���푰��N���X�ȊO��e��
	switch (q_index)
	{
	case QUEST_TYPHOON:
	{
		bool flag_ng = TRUE;


		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL)
		{
			//v1.1.85 �V��ɕϐg���Ă���Ǝ󂯂���悤�ɂ��Ă݂�
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
			put_str(_("���̃N�G�X�g�͓V�炵���󂯂��Ȃ��悤���B",
                    "Only tengu can take on this request."), 8, 0);
			return;
		}
	}
	break;

	case QUEST_MIYOI:
	{
		if (p_ptr->pclass == CLASS_BYAKUREN)
		{
			put_str(_("���Ȃ��͉����ɂ�肱�̃N�G�X�g�ɎQ���ł��Ȃ��B",
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
			put_str(_("���̃N�G�X�g�́u���q�c ���T�v���u����c ���v��|���Ȃ��Ǝ󂯂��Ȃ��悤���B",
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

		//v1.1.85 �l�Ԃɕϐg���Ă���Ǝ󂯂���悤�ɂ��Ă݂�
		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL && r_info[p_ptr->metamor_r_idx].flags3 & RF3_HUMAN) flag_ng = FALSE;

		if(flag_ng)
		{
			put_str(_("���̃N�G�X�g�͐l�Ԃ�����ɋ߂��푰�łȂ��Ǝ󂯂��Ȃ��悤���B",
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
			put_str(_("���Ȃ��͂Ȃ����n��a�̎�Ɗ�����킹��C�ɂȂ�Ȃ��B",
                    "For some reason, you just can't bring yourself to face the ruler of Palace of Earth Spirits."), 8, 8);
			return;
		}

	}
	break;

	//v1.1.85 �Ǔے��N�G�X�g�͕Е���̂���Ƃ����Е�����̂��邱�Ƃ͂ł��Ȃ��Ȃ�
	case QUEST_GEIDON_HUMAN:
		if (p_ptr->pclass == CLASS_MIYOI)
		{
			put_str(_("�X��ɂ͉����Y�݂�����悤�����A���Ȃ��ɘb���Ă͂���Ȃ������B",
                    "The owner seems to be troubled by something, but refuses to speak to you."), 8, 0);
			put_str(_("�[��ɂ��q���񂩂畷���Ă݂悤�B",
                    "You can try asking the customers here at night."), 9, 0);
			return;
		}
		if (quest[QUEST_GEIDON_KWAI].status != QUEST_STATUS_UNTAKEN)
		{
			put_str(_("�������̎��ԑт̃N�G�X�g���󂯂邱�Ƃ͂ł��Ȃ��B",
                    "You can no longer receive quests during this time."), 8, 0);
			return;
		}
		break;
	case QUEST_GEIDON_KWAI:
		if (quest[QUEST_GEIDON_HUMAN].status != QUEST_STATUS_UNTAKEN)
		{
			put_str(_("�������̎��ԑт̃N�G�X�g���󂯂邱�Ƃ͂ł��Ȃ��B",
                    "You can no longer receive quests during this time."), 8, 0);
			return;
		}
		break;
	case QUEST_REIMU_ATTACK:

		//�疒�̓A�r���e�B�J�[�h����V�̃N�G�X�g���󂯂��Ȃ��B
		//check_ignoring_quest()���g���ƃA�W�g�����󂵂ă_�[�N�G���t�N�G���󂯂��Ȃ��Ȃ�̂ł����ŏ���
		if (p_ptr->pclass == CLASS_CHIMATA)
		{
			put_str(_("�������Ŏ󂯂���N�G�X�g�͂Ȃ��悤���B",
                    "It doesn't look like you can receive a quest here right now."), 8, 0);
			return;
		}
		break;



	}

	q_ptr = &quest[q_index];


	//v1.1.98 �A�����������U�̃N�G�X�g�����łɉ���ɜ߈˂��ꂽ��ԂŎ󂯂�Ɠ���t���O�𗧂Ă�
	if (q_index == QUEST_HANGOKU2 && q_ptr->status == QUEST_STATUS_UNTAKEN && p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
	{
		p_ptr->quest_special_flag |= QUEST_SP_FLAG_HANGOKU2_POSSESSED;
		if (cheat_xtra) msg_print("SPECIAL FLAG ON");

	}

	//151124 v1.0.82b�ł�������quest53�`.txt�����Y��ăA�b�v���[�h�B�N�G�X�g��̂Ɏ��s���Aquest[53]�̓��e����̂܂�QUEST_STATUS_TAKEN�ɂȂ����B
	//������C�����邽�߂ɊY���󋵂ŃN�G�X�g�p�����[�^��ǂݒ����B
	if(q_index == QUEST_KAGUYA && q_ptr->status == QUEST_STATUS_TAKEN && q_ptr->type != QUEST_TYPE_DELIVER_ITEM)
	{
		get_questinfo(q_index, TRUE);
		return;
	}

	/* Quest is completed */
	/*:::�N�G�X�g�I���Ȃ��V�󂯎��҂��ɂ���*/
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index, FALSE);

		/*:::�r��Đ����t���O�H�@�N�G�X�g��V��REWARDED�̏�ԂŒ����Đ������ꂽ�Ƃ��u�����炵��*/
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
		/*:::�A�C�e���T���N�G�X�g*/
		if(q_ptr->type == QUEST_TYPE_DELIVER_ITEM)
		{
			/*:::�A�C�e���������Ă��ēn���΂����ɓ���B�N�G�X�g���������B*/
			if(check_quest_deliver_item(q_index))
			{

				//v1.1.70 �N�G�X�g�B�������𐳊m�ɂ��邽�ߒǉ�
				update_playtime();

				//v1.1.25 �N�G�X�g�B�������ǉ�
				q_ptr->comptime = playtime;

				q_ptr->complev = (byte)p_ptr->lev;
				q_ptr->status = QUEST_STATUS_REWARDED;
				get_questinfo(q_index, FALSE);
				reinit_wilderness = TRUE;

				// Hack - �P��N�G�X�g�B���̏ꍇ�i�s�x��i�߂�(�������܁A�`�[�g�I�v�V��������)
				if(q_index == QUEST_KAGUYA && !(p_ptr->noscore) && p_ptr->pseikaku != SEIKAKU_MUNCHKIN && kaguya_quest_level < KAGUYA_QUEST_MAX)
					kaguya_quest_level++;
			}
			/*:::�A�C�e�����Ȃ����n���̂�f�����B�N�G�X�g�����m�F�B�����̏ꍇ���s���b�Z�[�W�Ǝ��s�����B*/
			///mod151112 �P��N�G�X�g�̏ꍇ��������Ȃ�
			else if(q_index != QUEST_KAGUYA && get_check_strict(_("���̃N�G�X�g��������܂����H",
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
			put_str(_("�����Ŏ󂯂��N�G�X�g���܂��I�������Ă��Ȃ��B",
                    "You haven't finished the quest you've taken here yet."), 8, 0);
			put_str(_("(CTRL-Q���g���΃N�G�X�g�̏�Ԃ��`�F�b�N�ł��܂�)",
                    "(You can check quest status with Ctrl-Q)"), 9, 0);
		}
	}

	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		q_ptr->status = QUEST_STATUS_TAKEN;
		///sys ������FALSE�ɂ���΃N�G�X�g��̂̎��_�ł͊X�̍ĕ`�悪����Ȃ��Ȃ�Ǝv�����A�������o��̂��낤���B
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
msg_format("�N�G�X�g: %s�� %d�̓|��", name,q_ptr->max_num);
#else
			msg_format("Your quest: defeat %d %s", q_ptr->max_num, name);
#endif

		}
		//v2.0.13 ���N�U�푈2�N�G�X�g�̓��ꏈ��
		//�N�G�X�g���͂�\���������Ƃǂ̏܋����_�����I������\�����A�I�΂ꂽ�����X�^�[��quest[].r_idx�ɋL�^����B
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
				//�L�����Z�������ꍇ�N�G�X�g�𖢎�̂ɖ߂��Ă܂��N�G�X�g���͂ƑI�������o��悤�ɂ���
				q_ptr->status = QUEST_STATUS_UNTAKEN;
			}
		}
		//�ʏ�̃N�G�X�g�@�N�G�X�g���͕\��
		else
		{
			get_questinfo(q_index, TRUE);
		}


	}
}


/*
 * Display town history
 */
/*:::���̊T�v��\���@�}����*/
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

///mod141116 s16b��s32b�֕ύX
///mod150624 ������������u32b�ɂȂ��Ăĕ��̂Ƃ��I�[�o�[�t���[���Ă��̂�s32b�ɒ�����
s32b calc_expect_crit(int weight, int plus, int dam, s16b meichuu, bool dokubari)
{
	s32b i,k, num;
	int count;

	if(dokubari) return dam;

	//v1.1.69 �������Z�ɂ��d�ʃ{�[�i�X
	if (p_ptr->pclass == CLASS_URUMI && p_ptr->tim_general[0])
	{
		int old_weight = weight;
		weight = weight * p_ptr->lev * 8 / 50;
	}

	//v1.1.94 ����N���e�B�J�����㏸�Əd�ʏ��2000�𔽉f
	weight = weight * 2 + p_ptr->lev * 2;
	if (weight > 2000) weight = 2000;



	i = weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3);
	num = 0;

	//��S�̂Ƃ���1d650��1~650�܂őS���_���[�W�v�Z���đ������킹�čŌ��650�Ŋ��邱�Ƃŉ�S���σ_���[�W���v�Z���Ă���

	//v1.1.94
	//k��randint1(weight)+randint1(650)�ɂ����̂œ����悤�Ȍv�Z���ł��Ȃ��Ȃ����B
	//randint1(weight)��1����ő�܂�10%�����₵�đS�����v���邩�B���S�Ȑ����ł͂Ȃ��Ȃ邪����قǊO��Ă͂��Ȃ��Ǝv��
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

/*:::���폠�̕����r�T�u���[�`��4�@�X���C�v�Z*/
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
/*:::���폠�̕����r�T�u���[�`��3 �_���[�W�\��*/
static void compare_weapon_aux2(int r, int c, int mindam, int maxdam, cptr attr, byte color)
{
	char tmp_str[80];


	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
	sprintf(tmp_str, "�P�^�[��: %d-%d �_���[�W",
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
/*:::���폠�̕����r�̃T�u���[�`��2 �X���C���ݍU���͕\��*/
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
	compare_weapon_aux2(r++, col, mindam, maxdam, "��S:", TERM_L_RED);
#else
	compare_weapon_aux2(r++, col, mindam, maxdam, "Critical:", TERM_L_RED);
#endif

	/* Vorpal Hit*/
	///mod151101 *�؂ꖡ*�̂݃t���O�����Ă����Ƃ�(�b��Ȃ�)�ɑΏۂƂȂ�悤�������ǉ�
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
			compare_weapon_aux2(r++, col, mindam, maxdam, "�؂ꖡ:", TERM_L_RED);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_L_BLUE);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("���א���:", "Chaotic:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("���א���:", "Chaotic:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("��������:", "Lawful:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("��������:", "Lawful:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�l��:", "Human:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�l��:", "Human:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("����:", "Animal:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("����:", "Animal:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�s��:", "Undead:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�s��:", "Undead:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("����:", "Demon:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("����:", "Demon:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�_�i:", "Deity:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�_�i:", "Deity:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�d��:", "Youkai:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�d��:", "Youkai:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("��:", "Dragon:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("��:", "Dragon:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�j:", "Male:"), TERM_YELLOW);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�_����:", "Acid:"), TERM_RED);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�d����:", "Elec:"), TERM_RED);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("������:", "Fire:"), TERM_RED);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�⑮��:", "Cold:"), TERM_RED);
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
		compare_weapon_aux2(r++, col, mindam, maxdam, _("�ő���:", "Pois:"), TERM_RED);
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



/*::: -Mega Hack- ��������̃N���X��푰�̎��A����̃N�G�X�g�̕��͂�s���R�ɂȂ�Ȃ��悤�ɕύX����B*/
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
		//�����̃A�W�g
	case QUEST_HOME1:
		if(pc == CLASS_BANKI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u����M���́E�E���x�ǂ��Ƃ���ɗ��Ă���܂����B");
				strcpy(quest_text[line++], "�O��̋󂫉Ƃ������m�ł���ˁH");
				strcpy(quest_text[line++], "�����������o���̂Ȃ��Ȃ炸�҂̗��܂��ɂȂ��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ނ�͗��œ��݂𓭂��Ă���悤�ł��B�厖�ɂȂ�O�ɒǂ��o���ĉ������B");
				strcpy(quest_text[line++], "���ʂ̐l�Ԃ����ł��B�M���Ȃ���Ȃ��ł��傤�E�E�v");
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
				strcpy(quest_text[line++], "�����u�c�c�O��̋󂫉Ƃɑ�؂ȋq�l�����������Ă��܂��B");
				strcpy(quest_text[line++], "�����ł����A��΂Ɍ����点�����Ă͂��߂ł���H�v");
#else
                strcpy(quest_text[line++], "Akyuu - '...I've invited some precious guests into the vacant");
				strcpy(quest_text[line++], "house on the outskirts. I'd like you to not cause any trouble");
				strcpy(quest_text[line++], "for them. Understood?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u���炠��܂��܂��A�Ȃ�ė��\�Ȃ��Ƃ��Ȃ���̂ł��傤�B");
				strcpy(quest_text[line++], "�ǂ�����x�Ƃ��̉Ƃɋߊ��Ȃ��ł��������B�v");
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
				strcpy(quest_text[line++], "�����u腖��l�I�H���̂��p�ł��傤���A�܂����E�E");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "���������A���͋x�ɂňٕϒ������Ȃ̂ł����B����͐��ɓV�̏����ł��B");
				strcpy(quest_text[line++], "���͊O�E�̂Ȃ炸�҂��O��̋󂫉Ƃ�����ɓ��݂𓭂��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ǂ��������Ē����܂��񂩁H�v");
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
				strcpy(quest_text[line++], "�����u�L��������܂����B");
				strcpy(quest_text[line++], "�������ŋߗ��̎��肪�����ŊĎ��̖ڂ�����Ă��Ȃ��̂ł��B");
				strcpy(quest_text[line++], "�ł���Έٕς̊Ԃ����ł����̋󂫉Ƃ��Ǘ����Ē����܂��񂩁H");
				strcpy(quest_text[line++], "���u�Ƃ��Ăł��g���Ă�������Ώ\���ł��̂ŁB�v");
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
				strcpy(quest_text[line++], "�����u���珬�邶��Ȃ��B������ו������Ǒݖ{����̋A��Ȃ́H");
				strcpy(quest_text[line++], "�Ƃ���ŁA�O��̋󂫉ƂɂȂ炸�҂��Z�ݒ������ƕ񍐂���������B");
				strcpy(quest_text[line++], "�Ώ����I���܂Ő�΂ɋߕt���Ȃ��悤�ɂˁB�����H�v");
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
				strcpy(quest_text[line++], "�����u���邪�ގ������c�ł����āH");
				strcpy(quest_text[line++], "�������������炢���悤�Ȃ��̂́A����Ȃɖ������Ȃ��Ă����̂ɁB");
				strcpy(quest_text[line++], "���̋󂫉ƁH�������肪���܂�܂őq�ɂɂł��g���Ă������B");
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
				strcpy(quest_text[line++], "�����u���炱��ȏ��ɗd�����B");
				strcpy(quest_text[line++], "�����q������O�ŗV��łȂ����B");
				strcpy(quest_text[line++], "�O��̋󂫉Ƃɂ͕|���l�����邩��ߊ������ʖڂ�B�v");
#else
                strcpy(quest_text[line++], "Akyuu - 'Oh, a fairy? Be a good kid and play outside.");
				strcpy(quest_text[line++], "There are scary people in the vacant house on the outskirts,");
				strcpy(quest_text[line++], "so stay away from there.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u���̂Ȃ炸�҂�ǂ��o�����ł����āH��������ˁB");
				strcpy(quest_text[line++], "���J���ɂ��̋󂫉Ƃ����΂炭�݂��Ă�����B");
				strcpy(quest_text[line++], "�閧��n�ɂł��g���Ȃ����B�v");
#else
                strcpy(quest_text[line++], "Akyuu - 'You drove those bandits away? I'm surprised.");
				strcpy(quest_text[line++], "As a reward, I'm letting you use that house for a while.");
				strcpy(quest_text[line++], "Feel free to set up your secret base.'");
#endif
			}
		}

		break;
		//�I�[�N�̃L�����v
	case QUEST_ORC:
		if(pc == CLASS_KEINE)
		{

			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�I�[�N�ƌĂ΂�鋥�\�ȏb�l���������Đ��̓��A�ɏW�܂��Ă���B");
				strcpy(quest_text[line++], "���u���Ă����Ɨ�����������邩������Ȃ��B���̂����ɒ@���Ă������B");
				strcpy(quest_text[line++], "�I�[�N��̈�̂͑債�ċ����Ȃ��悤�������������A");
				strcpy(quest_text[line++], "����ɗ͋������[�_�[�i�̃I�[�N������B���f�͋֕����B");
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
				strcpy(quest_text[line++], "�I�[�N�B�͎U��U��ɓ��������Ă������B�����͋ߊ���Ă��Ȃ����낤�B");
				strcpy(quest_text[line++], "�B�c�Ƃ̎҂�����̕i��͂��Ă��ꂽ�B");
				strcpy(quest_text[line++], "������B�ꖪ���B�����Z���Γ��A�T�������邾�낤�B");
#else
                strcpy(quest_text[line++], "The orcs have ran away and probably aren't coming back any time soon.");
				strcpy(quest_text[line++], "You've received a reward from the Hieda House.");
				strcpy(quest_text[line++], "It's a cloak of concealment that'll come in use during dungeon exploration.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�I�[�N�B�͎v�����ȏ�ɋ����A�s�o������Ă��܂����B");
				strcpy(quest_text[line++], "�K���Ȏ��ɁA���ɗ��Ă������g���b�𕷂��ăI�[�N��S��������΂��Ă��ꂽ�B");
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
				strcpy(quest_text[line++], "�d���u���ƁA腖��l�ł͂Ȃ��ł����B����ٕ̈ς̒������Ȃ̂ł��ˁB");
				strcpy(quest_text[line++], "���͈يE�̋��\�ȏb�l�炵���A�����߂��̓��A�ɐ��ݒ����Ă���܂��B");
				strcpy(quest_text[line++], "��낵����Αގ��ɗ͂�݂��Ă��������܂��񂩁H�v");
#else
                strcpy(quest_text[line++], "Keine - 'Oh, Miss Yama? I assume you're investigating the incident.");
				strcpy(quest_text[line++], "Truth to be told, some violent beastmen from other world are living in a");
				strcpy(quest_text[line++], "nearby cave. Could you lend us a hand and deal with them?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u���͓Y�����肪�Ƃ��������܂����B");
				strcpy(quest_text[line++], "���������̒����ɂ��𗧂Ă��������B�v");
#else
                strcpy(quest_text[line++], "Keine - 'Thank you for your support.");
				strcpy(quest_text[line++], "Take this, it will help you in your quest.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u�E�E腖��l�ɗ��炸�Ɨ͂ŉ΂̕��𕥂��Ƃ������S�Ȃ̂ł��ˁB");
				strcpy(quest_text[line++], "�����蓢������g�D�������܂��B�v");
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
				strcpy(quest_text[line++], "�d���u���⏬�邩�B���X�����i�D�łǂ������H");
				strcpy(quest_text[line++], "�����A�����Ă������Ƃ��������B");
				strcpy(quest_text[line++], "���ܗ��̋߂��ɃI�[�N�Ƃ������\�Ȉ��l�����W�܂��Ă���񂾁B");
				strcpy(quest_text[line++], "�댯�������q�Ȃ��ł͗����痣��Ȃ��悤�ɂȁB�v");
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
				strcpy(quest_text[line++], "�d���u���A�I�[�N��S�Ēǂ��U�炵�Ă������ƁH");
				strcpy(quest_text[line++], "�������ȁB���̊Ԃɂ����܂ł̎g����ɂȂ����̂��B");
				strcpy(quest_text[line++], "�Ƃ������Ƃ��Ă��̉B�ꖪ���󂯎���Ă���B");
#else
                strcpy(quest_text[line++], "Keine - 'What, you drove them all away? I'm surprised.");
				strcpy(quest_text[line++], "Since when you've became that skilled? Very well, take this");
				strcpy(quest_text[line++], "cloak of concealment as your reward.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u�悵�悵�A����͂Ȃ����H");
				strcpy(quest_text[line++], "�|����Ȃ����ɖ��S�C�ȂƂ���͐̂���ς��Ȃ��ȁB�v");
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
				strcpy(quest_text[line++], "���q���ɍs���ƌd���ɗ��ݎ������ꂽ�B");
				strcpy(quest_text[line++], "�I�[�N�Ƃ��������\�Ȉ��l���߂��̓��A�ɓԂ��Ă���炵���B");
				strcpy(quest_text[line++], "�ŋߑ̂��݂��Ă��邵�y���^��������̂��������낤�B");
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
				strcpy(quest_text[line++], "�I�[�N��ǂ������Ď��q���ɕ񍐂ɖ߂�ƁA�d������̕i�����ꂽ�B");
				strcpy(quest_text[line++], "�G����g���B���B�ꖪ�炵���B");
				strcpy(quest_text[line++], "�ʂɉB���K�v���Ȃ��̂����A�d���Ȃ�̗D�����Ȃ̂��낤���B");
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
				strcpy(quest_text[line++], "�d���u�M�����������قǂ̓G�ɂ͂ƂĂ������܂���ł������B");
				strcpy(quest_text[line++], "�܂����̒��ł��������̂ł����H�v");
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
				strcpy(quest_text[line++], "�d���u����̛ޏ������ĂȂ��悤�ȑ���ɂ͌����Ȃ��������B");
				strcpy(quest_text[line++], "���������Ă͈������A���������ł���̂ł͂Ȃ����H�v");
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
				strcpy(quest_text[line++], "�d���u�d�����B���̓��A�ɂ͋ߊ��Ȃ������������B");
				strcpy(quest_text[line++], "���\�Ȉ��l������ɂ��Ă��Ċ댯�Ȃ񂾁B�߁X��������g�D���悤�ƍl���Ă���B");
				strcpy(quest_text[line++], "����܂ł��̕ӂł͗V�ԂȂ�B�v");
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
				strcpy(quest_text[line++], "�d���u���A�I�[�N��S�Ēǂ��U�炵�Ă������ƁH");
				strcpy(quest_text[line++], "�ŋ߂͗d���Ƃ����ǂ��n���ɂł��Ȃ��ȁB");
				strcpy(quest_text[line++], "����ɂ���������悤�B���ň��Y�Ɏg���񂶂�Ȃ����H�v");
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
				strcpy(quest_text[line++], "�d���u���猾��Ȃ����Ƃ���Ȃ��B");
				strcpy(quest_text[line++], "����͂Ȃ����H�v");
#else
                strcpy(quest_text[line++], "Keine - 'Don't tell me I haven't warned you.");
				strcpy(quest_text[line++], "Are you hurt?'");
#endif
			}
		}

		break;

		//���O���X�g��+���@���A��V��A���T�V��
	case QUEST_HOME2:
		if(pc == CLASS_MOMIZI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���u�ӂ��ށA��������ς܂��Ă��܂��܂������B");
				strcpy(quest_text[line++], "�@���͂�����Ɗ��E�E���������S�z���Ă܂������]�v�������悤�ł��ˁB");
				strcpy(quest_text[line++], "�@���̋󂫉Ƃ݂͂�ȖY��邱�Ƃɂ����悤�ł����A���u�ɂł��g���΂ǂ��ł��H");
				strcpy(quest_text[line++], "�@���̒��x�͖𓾂��Ă��̂ł���B");
#else
                strcpy(quest_text[line++], "Aya - 'Hmm, you cleaned them up pretty quickly.");
				strcpy(quest_text[line++], " I was getting a bit worried... though looks like it was for nothing.");
				strcpy(quest_text[line++], " How about we all forget about that house, and you'll use it as a ");
				strcpy(quest_text[line++], " storeroom? It'll be pretty handy.'");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���u���犑���̐�x�x�͂ǂ�������ł��H");
				strcpy(quest_text[line++], "�@�͂͂��A��̋󂫉Ƃ̗]���҂̌��ł��ˁH");
				strcpy(quest_text[line++], "�@�������񂾌����Ă����̌��͔��T�V��̊Ǌ��ł�����˂��B");
				strcpy(quest_text[line++], "�@�������Ǝn�����Ė�������}�낤�Ɩ������󂯂����ĂƂ���ł����B");
				strcpy(quest_text[line++], "�@�E�E�����ɂ܂Ȃ��ŉ�������B�����^�����F�肵�܂��B");
				strcpy(quest_text[line++], "�@���s���Ă��X�N�[�v�����肵�Ȃ���������Ȃ����炲���S���������ȁB�v");
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
				strcpy(quest_text[line++], "���u���₨�⏬�邳�񂶂�Ȃ��ł����B");
				strcpy(quest_text[line++], "����\���グ�܂������A�����ł͗�ވ��̏��邳��Ƃ��Ă͈����܂����B");
				strcpy(quest_text[line++], "������Ɨ��݂�����̂ł����A�O��̋󂫉Ƃ�K�˂Ă���܂��񂩁H");
				strcpy(quest_text[line++], "�܂��ʓ|�Ȏ葱���̈�Ƃ������A��X�ȊO�̎肪�K�v�Ȃ̂ł���B");
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
				strcpy(quest_text[line++], "���u����͂�����܂����ˁB");
				strcpy(quest_text[line++], "�����ɂȂ����Ƃ���𓥂ݍ��ނ���ł������A");
				strcpy(quest_text[line++], "�܂����M�����S���|���Ă��܂��Ƃ́B");
				strcpy(quest_text[line++], "�����A���̉Ƃ͍����グ�܂��傤�B���R�Ɏg���Ă��������B");
				strcpy(quest_text[line++], "������l���𓝂ׂ邩������Ȃ��l�Ԃɂ͂��ꂭ�炢������O�ł���c�v");
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
				strcpy(quest_text[line++], "����悭�Ƃ�D���Ă�����B�]���҂��V��̘A���������C�����B");
#else
                strcpy(quest_text[line++], "You've successfully taken the house for yourself. It feels good");
                strcpy(quest_text[line++], "getting on the nerves of both those strangers and tengu.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�]���҂��󂫉Ƃɐ��ݒ����Ė��Ȃ�����݂Ŏ肪�o���Ȃ��炵���B");
				strcpy(quest_text[line++], "���������������̗]���҂ɂȂ�����Ă��̉Ƃ�D���Ă�낤�B");
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
				strcpy(quest_text[line++], "�@�߂��Ă���ƕ��͏����Ă����B");
				strcpy(quest_text[line++], "�@�Ƃ肠�������̋󂫉Ƃ͋��_�Ƃ��Đڎ����邱�Ƃɂ����B");
#else
                strcpy(quest_text[line++], " As you return, Aya's not here.");
				strcpy(quest_text[line++], " In any case, you take that vacant house as your exploration");
				strcpy(quest_text[line++], " base.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���u����E�E�ǁA�ǂ����B");
				strcpy(quest_text[line++], "�@�������������H������������Ȗő����Ȃ��B");
				strcpy(quest_text[line++], "�@���R�͎����ĕ��a�ł������܂���B");
				strcpy(quest_text[line++], "�@�������]���҂��󂫉Ƃ�苒������A");
				strcpy(quest_text[line++], "�@�����炻�����̂��ŐӔC�����t����������Ȃ�āA");
				strcpy(quest_text[line++], "�@���[�񂺂񂵂Ă���܂���B�v");
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
				strcpy(quest_text[line++], "�@�߂��Ă���Ǝ˖��ۂ͏����Ă����B");
				strcpy(quest_text[line++], "�@�Ƃ肠�������̋󂫉Ƃ͋��_�Ƃ��Đڎ����邱�Ƃɂ����B");
#else
                strcpy(quest_text[line++], " As you return, Aya's not here.");
				strcpy(quest_text[line++], " In any case, you take that vacant house as your exploration");
				strcpy(quest_text[line++], " base.");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���u����͂���́A腖��l�ł͂���܂��񂩁E�E");
				strcpy(quest_text[line++], "�@�������������H������������Ȗő����Ȃ��B");
				strcpy(quest_text[line++], "�@���R�͎����ĕ��a�ł������܂���B");
				strcpy(quest_text[line++], "�@�������]���҂���̋󂫉Ƃ�苒������A");
				strcpy(quest_text[line++], "�@�����炻�����̂��ŐӔC�����t����������Ȃ�āA");
				strcpy(quest_text[line++], "�@���[�񂺂񂵂Ă���܂���B�v");
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
				strcpy(quest_text[line++], "���u�Ȃ�ƁA�������낤�ɗd���ɏo���������Ƃ́B");
				strcpy(quest_text[line++], "����͂�V��̈Ќ����������������񂶂Ⴀ��܂���˂��B");
				strcpy(quest_text[line++], "���̉Ƃ͓����D���Ɏg���Ă����ł���B");
				strcpy(quest_text[line++], "�󂫉Ƃ̂܂܂ɂ��Ă������͊�炩�܂��ł��傤�B�v");
#else
                strcpy(quest_text[line++], "Aya - 'Well, looks like I was overtaken by a fairy.");
				strcpy(quest_text[line++], "No, it's not about tengu pride or anything.");
				strcpy(quest_text[line++], "Use that house as you like.");
				strcpy(quest_text[line++], "It's better than leaving it vacant.'");
#endif

			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���u�����A�����͋M���̂悤�ȗd���ɂ͊댯�ł���B");
				strcpy(quest_text[line++], "���ɍ��͗��ɂ�����Ƃ����ʓ|�����N�����Ă���̂ł�����B");
				strcpy(quest_text[line++], "�߂܂��ĕ|���ڂɑ����O�ɉƂɋA��Ȃ����B�v");
#else
                strcpy(quest_text[line++], "Aya - 'Ayaya, it's dangerous here for a fairy like you.");
				strcpy(quest_text[line++], "There's a particularly nasty incident going on here");
				strcpy(quest_text[line++], "right now. Better go back before you get in trouble.'");
#endif
			}
		}

		else if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU)
		{
			//v2.0.5 �����u�͂��ẲƁv��ǉ������̂ł͂��Ă̂Ƃ��Z���t��ς��Ă���
			if (p_ptr->pclass == CLASS_HATATE)
			{
				if (comp)
				{
#ifdef JP
					strcpy(quest_text[line++], "�N���҂ɓˌ���ނ����s���A���̉Ƃ����߂����B");
					strcpy(quest_text[line++], "���̉�������炪�ڂɕ����Ԃ悤���B");
#else
					strcpy(quest_text[line++], "You've driven the intruders out of Aya's house.");
					strcpy(quest_text[line++], "You can picture the look of frustration on her face.");
#endif
				}
				else if (fail)
				{
					msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "���΂炭����ɂ��Ă��镶�̉Ƃɗ]���҂��Z�ݒ����Ă����B");
					strcpy(quest_text[line++], "�]���҂͖�Ȗ�ȉ������V�����J��Ԃ��A�ߏ��ɔ�Q���o�Ă���悤���B");
					strcpy(quest_text[line++], "����͖ʔ����Ȃ��Ă����B");
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
                    strcpy(quest_text[line++], "����ǂ��N���҂�Еt���A�䂪�Ƃ����߂����B");
                    strcpy(quest_text[line++], "�낤���S�V�b�v�̃l�^�ɂȂ�Ƃ��낾�������A�Ƃ肠�����ʎq�͕ۂ����悤���B");
#else
                    strcpy(quest_text[line++], "You've cleaned up the intruders and got your house back.");
                    strcpy(quest_text[line++], "It was about to become a gossip story, but you've managed to save your face.");
#endif

                }
                else if(fail)
                {
                    msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
                }
                else
                {
#ifdef JP
                    strcpy(quest_text[line++], "���΂炭����ɂ��Ă��邤���ɉƂɗ]���҂��Z�ݒ����Ă����B");
                    strcpy(quest_text[line++], "�]���҂͖�Ȗ�ȉ������V�����J��Ԃ��A�ߏ��ɔ�Q���o�Ă���悤���B");
                    strcpy(quest_text[line++], "���₩�Ɉ�l�Ŏn�������Ȃ��ƓV��̌ւ�Ɋւ��B");
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

	case QUEST_HOME3: //�H�쉮�~���@���Ƃ�
		if(pc == CLASS_SATORI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���o���̂Ȃ��A�����������A�Ƃ������S�Ẳ���𒦂炵�߂��B");
				strcpy(quest_text[line++], "�󂫉Ƃ̂܂܂ł͂܂����Ȃ̂��o�邩������Ȃ��B");//��s40���܂�
				strcpy(quest_text[line++], "���u�ɂł��g�킹�Ă��炨���B�\�����Ă͒N���������͏o�Ȃ������B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You don't recall seeing those spirits before, but you've disciplined");
				strcpy(quest_text[line++], "them all. Something else weird might come up if you leave the");//��s40���܂�
				strcpy(quest_text[line++], "house vacant, so you've decided to use it as a storeroom.");//��s40���܂�
				strcpy(quest_text[line++], "Nobody's complaining.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�Ƃׂ̗̋󂫉ƂɊ�ȉ��삪�Ԃ��Ă���炵���B");
				strcpy(quest_text[line++], "����̊Ǘ��͎����̖������B");
				strcpy(quest_text[line++], "�l�q�����ɍs���Ă݂悤�B");
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
				strcpy(quest_text[line++], "����ł��Ėڏ�肾�����̂őS���@���ׂ����B");
				strcpy(quest_text[line++], "�ߏ��̎҂���󂫉Ƃ̊Ǘ��𗊂ݍ��܂ꂽ�B");
				strcpy(quest_text[line++], "���u����Ɏg���Ă�낤�B");
#else
                strcpy(quest_text[line++], "It was annoying, but you ended up crushing all of them.");
				strcpy(quest_text[line++], "You were requested to manage that vacant house.");
				strcpy(quest_text[line++], "You're going to use it as a storeroom.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�n��a�ׂ̗̋󂫉ƂɊ�ȉ��삪�Ԃ��Ă���炵���B");
				strcpy(quest_text[line++], "�܊p�����猩���ɍs���Ă݂悤�B");
#else
                strcpy(quest_text[line++], "Some strange vengeful spirits have settled in a vacant house");
				strcpy(quest_text[line++], "next to the Palace of Earth Spirits.");
                strcpy(quest_text[line++], "It's about time you've checked it out.");
#endif
			}

		}
		break;

	case QUEST_KAPPA:	//���ᏋC�@�d���E�H�X�q
		///mod151001 �N�G�X�g���e�ύX
		if(pc == CLASS_YOUMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����͉͓������̑��A�ɂȂ��Ă����炵���B�Ƃ肠�����S�ē|�����B");
				strcpy(quest_text[line++], "�ꑧ�����Ƃ��A���ʘO���o��ۂɎ傪���ȓ����a���Ă��ꂽ���Ƃ��v���o�����B");//��s40���܂�
				strcpy(quest_text[line++], "�ǂ���炱����g���Ύ��͂�S�Ēm�o�ł���炵���B");//��s40���܂�
				strcpy(quest_text[line++], "��̐S�����𖳑ʂɂ��Ă��܂����悤���B���߂č���̒T���ɖ𗧂Ă悤�B");//��s40���܂�
#else
                strcpy(quest_text[line++], "Looks like it was a kappa hideout. In any case, you defeated them all.");
				strcpy(quest_text[line++], "As you took a break, you remembered that your mistress gave you an");//��s40���܂�
				strcpy(quest_text[line++], "odd device as you left Hakugyokurou. Looks like you can detect");//��s40���܂�
				strcpy(quest_text[line++], "everything around you if you use it. You're not letting your");//��s40���܂�
				strcpy(quest_text[line++], "mistress's thought go to waste; you'll use it for your exploration");//��s40���܂�
				strcpy(quest_text[line++], "from now on.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�p���ɐ���ł����ҒB�́A���Ȃ��̎�ɂ͕����Ȃ������B");//��s40���܂�
				strcpy(quest_text[line++], "�傩��a������������̂��Ƃ��v���o�������A");//��s40���܂�
				strcpy(quest_text[line++], "�ǂ����ǂ������ɖ������Ă��܂����炵���E�E�B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You were no match for those who live in that building.");//��s40���܂�
				strcpy(quest_text[line++], "You remember your mistress giving you a magic device,");//��s40���܂�
				strcpy(quest_text[line++], "but looks like you've lost it at some point...");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���J���@�X�ɗH�삪���ӂ�Ă���Ɩ�����������������B");
				strcpy(quest_text[line++], "�H�삽���ɘb�𕷂��Ă݂�ƁA�ߏ��̔p���u��ؑ��v�ɏZ��ł����炵���B");
				strcpy(quest_text[line++], "���҂�����U�����󂯂ē����o���Ă����������B");
				strcpy(quest_text[line++], "�l�q�����ɍs���Ă݂悤�B");
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
				strcpy(quest_text[line++], "�d���u�����Ȃ�����݂ł����B�Ƃ��������͉��̂��߂ɁE�E");
				strcpy(quest_text[line++], "���A���ꂨ�Ԃ����܂��ˁB�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Youmu - 'That was splendid. Just what am I supposed to be doing, though...");
				strcpy(quest_text[line++], "Ah, I'm returning you this.'");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u�����A�ɂ������̂ł��ˁB");//��s40���܂�
				strcpy(quest_text[line++], "����ł͎����s���Ă��܂��B�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Youmu - 'Oh, so you had some free time.");
				strcpy(quest_text[line++], "Very well, I'm going in now.'");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u���A�H�X�q�l�B�����A��ʂ�ł����B");
				strcpy(quest_text[line++], "���̔p���ɉ������ȕ����o�����ėH�삽����ǂ��o���Ă��܂��������ł��B");
				strcpy(quest_text[line++], "���H�H�X�q�l�����ɍs���Ă݂�̂ł����H�v");
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
				strcpy(quest_text[line++], "�d���u���ƁA�����͉͓������̑q�ɂ������̂ł����B");
				strcpy(quest_text[line++], "����Ȏ��ׂ̈ɐS���b���Ă����̂ɁA�܂��܂��C�s������܂���c");
				strcpy(quest_text[line++], "���A���������ΗH�X�q�l��������������Ă���Ă�����ł��B");
				strcpy(quest_text[line++], "�ǂ��������Ȃ̂��悭������Ȃ������̂ł����A�����腖��l�ɍ����グ�܂��ˁB�v");
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
				strcpy(quest_text[line++], "�d���u���̋߂��̋󂫉ƂɏZ��ł���H�삽�����A");
				strcpy(quest_text[line++], "���Ȃ��̂ɋ�������Ă���炵���̂ł��B");
				strcpy(quest_text[line++], "�����������s���Ă݂���ł����ʂɉ������Ȃ��݂�����");
				strcpy(quest_text[line++], "�ł��m���ɖ��ȋC�z�͂����ł��B腖��l�͉����킩��܂����H�v");
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
				strcpy(quest_text[line++], "�d���u�����̔p���ɋߊ���Ă͑ʖڂł���B���Ȃ��̂�����ł���悤�Ȃ̂ł��B�v");
#else
                strcpy(quest_text[line++], "Youmu - 'Don't get too close to that abandoned house. There's something");
                strcpy(quest_text[line++], "weird living inside.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u���A�͓����Ԃ��Ă����́H�Ƃ��������Ȃ����S���|����������́H");
				strcpy(quest_text[line++], "�������ȁB����͂���ŗH�X�q�l�ɉ��ƕ񍐂��Ă������̂��E�E");
				strcpy(quest_text[line++], "�Ƃ肠��������������܂��ˁB�v");
#else
                strcpy(quest_text[line++], "Youmu - 'Eh, that was kappas? And you defeated them all?");
				strcpy(quest_text[line++], "I'm troubled. How can I even tell about this to Lady Yuyuko?");
				strcpy(quest_text[line++], "In any case, I'm giving you this.'");
#endif
			}
		}
		break;
		//�~�~�b�N�N�G
	case QUEST_MIMIC:

		break;

		//�f�X�\�[�h
	case QUEST_DEATHSWORD:
		break;

		//������
	case QUEST_SEWER:
		if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u�ŋ߉������̗l�q�����������́B");
				strcpy(quest_text[line++], "�Ђ���Ƃ���Ɨd�������������݂��Ă���̂�������Ȃ���B");
				strcpy(quest_text[line++], "�O�݂����ɖ�����������ʖڂ�B�{���ɂǂ��Ȃ��Ă��m��Ȃ�����ˁB�v");
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
				strcpy(quest_text[line++], "�����u�����猾�����ł��傤�B");
				strcpy(quest_text[line++], "����ɒ��肽�班���͂��ƂȂ������Ă��邱�ƂˁB");
				strcpy(quest_text[line++], "�厖�Ɏ���Ȃ��Ă悩������B");
#else
                strcpy(quest_text[line++], "Akyuu - 'Didn't I tell you?");
				strcpy(quest_text[line++], "I hope this'll teach you to stay quiet.");
				strcpy(quest_text[line++], "I'm glad you're alright.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u���҂��̑像�j���̂�|���Ă����c�ł����āH");
				strcpy(quest_text[line++], "�M���A�{���ɐl�̓��𓥂݊O�������Ă�񂶂�Ȃ��́H�v");
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
				strcpy(quest_text[line++], "�����u�����̌��H����͉������ɒʂ���l�E��B");
				strcpy(quest_text[line++], "�ŋ߉��������ݒ����Ă�݂����ŋ߁X��������\���B");
				strcpy(quest_text[line++], "��Ȃ��������Ȃ��悤�ɂˁB�v");
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
				strcpy(quest_text[line++], "�����u�����A�����܂łЂǂ����ƂɂȂ��Ă����̂ˁB");
				strcpy(quest_text[line++], "�ŋߖ{���ɕ����Ȃ񂾂���C�����Ȃ����B�v");
#else
				strcpy(quest_text[line++], "Akyuu - 'Oh, it has gotten that bad.");
				strcpy(quest_text[line++], "It's getting dangerous out here lately, so be careful.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u���҂��̑像�j���̂�|���Ă����c�ł����āH");
				strcpy(quest_text[line++], "�M���A�d������d���ɂȂ肩���ĂȂ��H�v");
#else
				strcpy(quest_text[line++], "Akyuu - 'You... defeated the undead and a giant crocodile?");
				strcpy(quest_text[line++], "Are you about to turn into a youkai from a fairy?");
#endif
			}
		}
		break;

		//�j��1�@�`���m
	case QUEST_DOOM1:
		if(pc == CLASS_CIRNO)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������v���Ă����z�ƈ�������A�Ƃ������S���|�����B");
				strcpy(quest_text[line++], "���ꂢ�Ȏw�ւ������Ă���z�������̂Ő험�i�Ƃ��Ē������Ă�����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "It wasn't who you thought it was, but you defeated them all.");
				strcpy(quest_text[line++], "One of them had a pretty ring, so you take it as a trophy.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "������Ɨn�������ɂȂ����̂ňꎞ�P�ނ��邱�Ƃɂ����B");//��s40���܂�
				strcpy(quest_text[line++], "�Ƃ���������͂ǂ����Ă��d������Ȃ��B�����N�����Ă���񂾂낤�H");//��s40���܂�
#else
                strcpy(quest_text[line++], "You were starting to melt, so you made a tactical retreat.");//��s40���܂�
				strcpy(quest_text[line++], "No matter how you look at it, they weren't fairies. Just what");//��s40���܂�
				strcpy(quest_text[line++], "has happened?");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�Ƃ̋߂�����ł���Ƃ����Ȃ�΂̋ʂ��Ԃ���ꂽ�B");
				strcpy(quest_text[line++], "�΂̋ʂ��������z�͋߂��̓��A�ɓ������񂾂悤���B");
				strcpy(quest_text[line++], "�܂����̎O�l�̎d�ƂɈႢ�Ȃ��B");
				strcpy(quest_text[line++], "���x�����܂Ƃ߂ē��点�Ă�낤�B");
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
				strcpy(quest_text[line++], "�`���m�u�ււ�A�d�����r�߂邩�炱���Ȃ�񂾁I");
				strcpy(quest_text[line++], "�C�����������炱������O�����ɂ�邼�I������ƑO�ɏE�������󂾁I�v");
#else
                strcpy(quest_text[line++], "Cirno - 'Hehe, don't underestimate us fairies!");
				strcpy(quest_text[line++], "I'm in a good mood, so take this! It's a treasure I've picked up");
				strcpy(quest_text[line++], "a bit earlier!'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�`���m�u�����[�I���̐��ł������Ă����ĂȂ��Ȃ�ā[�I");
				strcpy(quest_text[line++], "�������͂܂Ƃ߂ē��点�Ă��I�v");
#else
                strcpy(quest_text[line++], "Cirno - 'Ugh! We couldn't win even with our numbers!");
				strcpy(quest_text[line++], "Next time, I'm going to freeze them all!'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�`���m�u���������Εςȓz�ɉ΂̋ʂ��Ԃ���ꂽ���Ƃ�����񂾁B");
				strcpy(quest_text[line++], "�������������̕����I�݂�Ȃŕ񕜂��ɍs�����I�v");
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
				strcpy(quest_text[line++], "�`���m�u�Ӂ[��A�ςȓz�炪��������ˁB");
				strcpy(quest_text[line++], "�G��ߒ����Ĉ���������ˁI������������I�v");
#else
                strcpy(quest_text[line++], "Cirno - 'Hmm, so there were some weirdos there.");
				strcpy(quest_text[line++], "I'm sorry for blaming you! I'm giving you this!'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�`���m�u���[��A���������񂾂�H");
				strcpy(quest_text[line++], "�����������Ă΁I����Ȃ��ɂ܂Ȃ��ł�I�v");
#else
                strcpy(quest_text[line++], "Cirno - 'Uhh, what was that?");
				strcpy(quest_text[line++], "I'm sorry! Don't glare at me like that!'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�`���m�u�������͂悭������Ă��ꂽ��ˁI");
				strcpy(quest_text[line++], "�����ŉ�������S�N�ځI�����q��Ɏ��˂��I");
				strcpy(quest_text[line++], "�c���H���̂��Ƃ����킩��Ȃ����āH");
				strcpy(quest_text[line++], "�������̓��A�̋߂��ł������ɉ΂̋ʂԂ����̂͂��񂽂�������Ȃ��́H�v");
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

	case QUEST_TUPAI: //�c�p�C�@�p�`�����[�A�t����
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "����ǂ��y�b�g��߂܂����B");
				strcpy(quest_text[line++], "���~�B�����𑗂��Ă悱�����B���ɂ������܂����^��������B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You've successfully captured the pet.");
				strcpy(quest_text[line++], "Remy has sent you a reward.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�s�o�ɂ���蓦�����Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���������d���͋�肾�B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You were caught unaware, and the pet escaped.");//��s40���܂�
				strcpy(quest_text[line++], "You're not good at jobs like this.");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���~�B�̃y�b�g�̖��Ȑ������������o�����炵���B");
				strcpy(quest_text[line++], "�܊p�Ȃ̂ŒT���̂���`���Ă����邱�Ƃɂ����B");
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
				strcpy(quest_text[line++], "���u�����l�B���ꂨ��l���炲�J���ł����āB");
				strcpy(quest_text[line++], "�����n�߂��̂��m��Ȃ����ǁA�����I��点�ċA���Ă��Ȃ����B�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Sakuya - 'Well done. Here's a reward from the mistress.");
				strcpy(quest_text[line++], "I don't know what you're doing, but I'd like you to finish it");//��s40���܂�
				strcpy(quest_text[line++], "quickly and come back to us.'");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���u�����̘H�n�Ŏ�蓦�������ł����āH�v");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "(���̎p�������Đ��b��Ăяo������)");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "���u��̂̏ꏊ�����������̂ō��߂܂��Ă���l�ɂ��͂����Ă�����B");
				strcpy(quest_text[line++], "�M�����������Ǝ�����ɖ߂�����H�v");
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
				strcpy(quest_text[line++], "���u�������A����ȏ��ŉ������Ă���́H");
				strcpy(quest_text[line++], "�܂�������B");
				strcpy(quest_text[line++], "�c�p�C�������o���Ă��܂����́B�M�����T���̂���`���Ȃ����B�v");
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
				strcpy(quest_text[line++], "�����y�b�g���߂肨��l�͖������ꂽ�悤���B");
				strcpy(quest_text[line++], "�J���Ƃ��čŋߋC�ɓ����Ă��郊�{���𒸂����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You managed to safely bring the pet back; your mistress is happy.");
				strcpy(quest_text[line++], "As a reward, she gave you a ribbon you've taken a liking to.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "��x�͌��������s�o�ɂ���蓦�����Ă��܂����B");
				strcpy(quest_text[line++], "����܂������Ԃ��B");
#else
                strcpy(quest_text[line++], "You've found it, but you were caught unaware, and it escaped.");
				strcpy(quest_text[line++], "That's a serious failure.");
#endif

			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "����ɂ��Ă��邤���ɂ���l�̃y�b�g�������o���Ă��܂����B");
				strcpy(quest_text[line++], "�����ɕ߂܂��Ȃ��Ƃ���l���߂���ł��܂��B");
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
				strcpy(quest_text[line++], "�ǂ��ɂ��y�b�g���󂳂��߂܂����B");
				strcpy(quest_text[line++], "���o�l�����𑗂��Ă悱�����B���ɂ������܂����^��������B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You've managed to capture the pet without breaking it.");
				strcpy(quest_text[line++], "Your sister has sent you a reward.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�s�o�ɂ���蓦�����Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���������d���͋�肾�B");//��s40���܂�
#else
				strcpy(quest_text[line++], "You were caught unaware, and the pet escaped.");//��s40���܂�
				strcpy(quest_text[line++], "You're not good at jobs like this.");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���o�l�̃y�b�g�̖��Ȑ������������o�����炵���B");
				strcpy(quest_text[line++], "�܊p�Ȃ̂ŒT���̂���`���Ă����邱�Ƃɂ����B");
#else
                strcpy(quest_text[line++], "Looks like your sisters's pet (some weird creature)");
				strcpy(quest_text[line++], "has ran away. You decide to go look for it.");
#endif
			}


		}
		break;

		//�N���[���n��
	case QUEST_CLONE:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���V���u���X�܂������Ƃ��N����܂����B");
				strcpy(quest_text[line++], "��̃N���[��������ŃN���[���B���\�����Ă��܂��܂��ĂˁB�v");
				strcpy(quest_text[line++], "�������܂������Ƃ��n�������肢�������B�v");
#else
                strcpy(quest_text[line++], "Rinnosuke - 'Something bad has happened.");
				strcpy(quest_text[line++], "The clones at that experimental cloning site have gone berserk.");
				strcpy(quest_text[line++], "I'm sorry, but couldn't you clean it up?'");
#endif
			}
		}
		break;
		//�h���S���ގ��@�d���A�O�����A�A���X
	case QUEST_CHAOS_WYRM:
		if(pc == CLASS_SUNNY || pc == CLASS_LUNAR || pc == CLASS_STAR || pc == CLASS_3_FAIRIES )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�{���Ƀh���S����|���Ă��܂����B");
				strcpy(quest_text[line++], "�Ƃ̋߂��𒲂ׂĂ�����Y��Ȏw�ւ��������B�������̕󕨂ɂ��悤�B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You really managed to defeat the dragon!");
				strcpy(quest_text[line++], "You also found a pretty ring near your house. It's going to be");//��s40���܂�
				strcpy(quest_text[line++], "your treasure.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�낤�����x�݂ɂȂ�Ƃ��낾�����B");//��s40���܂�
				strcpy(quest_text[line++], "�f���ɗ얲����ɗ��݂ɍs�����E�E");//��s40���܂�
#else
                strcpy(quest_text[line++], "Whew, you almost were done for.");//��s40���܂�
				strcpy(quest_text[line++], "Looks like you'll have to visit Reimu...");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���΂炭����ɂ��Ă���ƉƂ̋߂��Ɍ������Ƃ��Ȃ��h���S����������Ă����B");
#else
                strcpy(quest_text[line++], "While you were away, some dragon you've never seen before has appeared");
                strcpy(quest_text[line++], "close to your house.");
#endif
				if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
#ifdef JP
					strcpy(quest_text[line++], "�ł����Ȃ炱��Ȃɒ��Ԃ�����I�h���S���ɂł����Ă����ȋC������I");
#else
                    strcpy(quest_text[line++], "But with all the friends you have right now, even a dragon is no match for you!");
#endif
				else
#ifdef JP
					strcpy(quest_text[line++], "�ł����������͌��C��t���I�h���S���ɂł����Ă����ȋC������I");
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
				strcpy(quest_text[line++], "�T�j�[�~���N�u�����[���I�d�������̖閾�����߂���I�v");
				strcpy(quest_text[line++], "���i�`���C���h�u�Ƃ������M���{���ɗd���Ȃ́H�v");
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u����ɂ���������グ�܂���B�������̕󕨂�B�v");
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Awesome! The dawn of us fairies is at hand!'");
				strcpy(quest_text[line++], "Luna Child - 'Are you really a fairy?'");
				strcpy(quest_text[line++], "Star Sapphire - 'Take this as your reward. It's our treasure.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�T�j�[�~���N�u�����H���������ł���B�v");
				strcpy(quest_text[line++], "���i�`���C���h�u���[���A�܂������z�������B�v");//��s40���܂�
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u���x�̖~�͂͏o�����ǂ������̂Ɏc�O�˂��B�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Did you see it? Was it cool?'");
				strcpy(quest_text[line++], "Luna Child - 'We'll have to move again.'");//��s40���܂�
				strcpy(quest_text[line++], "Star Sapphire - 'It's pity I'm leaving my bonsai behind.'");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�T�j�[�~���N�u�҂��Ȃ����A�X�̕��ɍs���Ɗ�Ȃ���I�v");
				strcpy(quest_text[line++], "���i�`���C���h�u�������ȃh���S���Ɉ��x�݂ɂ��ꂿ�Ⴄ��B�v");
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u����̛ޏ������Q�΂�����B���������̂˂��B�v");
#else
                strcpy(quest_text[line++], "Sunny Milk - 'Wait, it's dangerous to go into the forest!'");
				strcpy(quest_text[line++], "Luna Child - 'There's a strong dragon who'll take you down.'");
				strcpy(quest_text[line++], "Star Sapphire - 'And that shrine maiden is still sleeping...'");
#endif
			}
		}

		break;

		//��������
	case QUEST_OLD_WILLOW:
		break;

		//�Η������@������,���ӁA���Ƃ�
	case QUEST_SHOOTINGSTAR:
		if(pc == CLASS_YUGI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u�M���ł����E�E���x�����Ƃ���ɗ��Ă���܂����B");
				strcpy(quest_text[line++], "���͋��ܔM�n���̍Ő[���ɋ��\�ȉΗ������ݒ����āA");
				strcpy(quest_text[line++], "���Ȃ����Ă��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ς݂܂��񂪁A���Ƃ������|���Ē����܂��񂩁H�v");
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
				strcpy(quest_text[line++], "���ɕ������������ł��Ă邪�Ȃ����������v���o���Ȃ��B");
				strcpy(quest_text[line++], "���̊Ԃɂ���ȏ����Ɏ����Ă����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "That's odd. Your clothes are scorched in places, but you");
                strcpy(quest_text[line++], "can't remember why. Also, you're holding some weird staff");
				strcpy(quest_text[line++], "you don't remember picking up.");//��s40���܂�
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu����H���N�������ɋ��Ȃ����������H�v");
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
				strcpy(quest_text[line++], "���Ƃ�u�����ς킹�Đ\���󂠂�܂���B");
				strcpy(quest_text[line++], "�d�˂ċ��k�Ȃ̂ł����A�����������ŊǗ����Ē����܂��񂩁H");//��s40���܂�
				strcpy(quest_text[line++], "�댯�Ȗ�����Ȃ̂ŁA���܂�n��ɂ����Ă��������͂Ȃ��̂ł��B�v");
#else
                strcpy(quest_text[line++], "Satori - 'I'm terribly sorry for troubling you.");
				strcpy(quest_text[line++], "Again, I'm sorry, but couldn't you take care of this as well?");//��s40���܂�
				strcpy(quest_text[line++], "It's a dangerous magic device I don't like leaving here underground.");
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u�M���́E�E�������Ă��ƁB");
				strcpy(quest_text[line++], "���̊��ɋy��ŉB���Ă��d������܂���ˁB");
				strcpy(quest_text[line++], "���ܔM�n���̍Ő[���ɈيE�̋��\�ȉΗ������ݕt���܂��āA");
				strcpy(quest_text[line++], "���̃y�b�g�B������Ă��Ă���̂ł��B");
				strcpy(quest_text[line++], "�Ǘ��ӔC�ɂ��Ă͏d�X���m���Ă���܂����A");
				strcpy(quest_text[line++], "���X��`���Ē����킯�ɂ͎Q��܂��񂩁H�v");
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
				strcpy(quest_text[line++], "���Ƃ����͂ȗ���|�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���͂Ȃ��Ȃ��ʔ������ȏ�������Ă����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You managed to defeat the powerful dragon.");
				strcpy(quest_text[line++], "It was holding a fairly interesting staff.");//��s40���܂�
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���ܔM�n���ɈيE�̉Η������ݒ������ƃy�b�g����񍐂��������B");
				strcpy(quest_text[line++], "�����Ă����̂��댯�Ȃ̂œ����Ɍ�����Ȃ��Ƃ����Ȃ��B");
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
				strcpy(quest_text[line++], "�ǂ��ɂ����͂ȗ���|�����B");
				strcpy(quest_text[line++], "�n��̒��ɏ񂪕����Ă���B�������̂ŏE���Ă����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You somehow managed to defeat the powerful dragon.");
				strcpy(quest_text[line++], "You found a staff floating in lava. It looked pretty rare,");//��s40���܂�
				strcpy(quest_text[line++], "so you picked it up.");//��s40���܂�
#endif
			}
			else if(fail)
			{
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
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
				strcpy(quest_text[line++], "���Ƃ�u���Ȃ����������̉Η���|�����ł����āH");
				strcpy(quest_text[line++], "�@�d�����Q���Ɣn���ɂȂ�Ȃ���ˁB");
				strcpy(quest_text[line++], "�Ƃ����ꍢ���Ă����̂�B���������B");
				strcpy(quest_text[line++], "��V�ɂ���������܂��傤�B�ł����p����Ɛg��łڂ����H�v");
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
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu...���ĉ��������O���񂽂��̓]���]���ƁB");
				strcpy(quest_text[line++], "�@�����Ă�����H����̓�������璆�ɓ���񂶂�Ȃ���H�v");
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
				strcpy(quest_text[line++], "���Ƃ�u���Ȃ������̉Η���|�����ł����āH");
				strcpy(quest_text[line++], "�E�E�R�ł��ϑz�ł��Ȃ��悤�ˁB");
				strcpy(quest_text[line++], "�₩�ɂ͐M�������ǁA��������܂��B");
				strcpy(quest_text[line++], "��V�ɂ���������܂��傤�B�ł����p����Ɛg��łڂ����H�v");
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
				msg_print(_("ERROR:�����ɂ͗��Ȃ��͂�", "ERROR: You shouldn't be seeing this"));
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu����H���N�������ɋ��Ȃ����������H�v");
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

		//���ۉ��O�@���~���A
	case QUEST_KYOMARU:
		if(pc == CLASS_REMY)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�p�`�����[�u����́E�E���ۉ��O�H");
				strcpy(quest_text[line++], "���̂��߂ɂ���Ȑ����炯�̂Ƃ���Ɏ��ɍs���Ă��ꂽ�́H");
				strcpy(quest_text[line++], "���肪�Ƃ��B����ɂ�����������B�v");
#else
                strcpy(quest_text[line++], "Patchouli - 'That's... the kyoumarubotan?");
				strcpy(quest_text[line++], "You went wading through water for my sake?");
				strcpy(quest_text[line++], "Thank you very much. Here's a reward.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�p�`�����[�u���̂������ɗ���Ŏ����Ă��Ă��炨��������B�v");
#else
                strcpy(quest_text[line++], "Patchouli - 'I wonder if Sakuya could bring it for me.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�p�`�����[�u���̖{�ɍڂ��Ă���Ԃ��C�ɂȂ�́H");
				strcpy(quest_text[line++], "����͋��ۉ��O�B�����̑�ɍ炭�������Ԃ�������B�v");
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
				strcpy(quest_text[line++], "�p�`�����[�u�O�b���Ă������ۉ��O�A���ɂ�������Ă���Ȃ��H");
				strcpy(quest_text[line++], "��������ɖ{�������čs���Ă�񂾂���A���܂ɂ͉������ɗ����Ȃ����B�v");
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
				strcpy(quest_text[line++], "�p�`�����[�u���Ȃ��̔L�x��6�_�B���������f���ɂȂ��Ă��܂���B�v");
#else
                strcpy(quest_text[line++], "Patchouli - 'Your cat-likeness is at 6 points. It's going to be a");
                strcpy(quest_text[line++], "prime number soon.'");
#endif
			}

		}

		break;



		//��l�Ȃ�Ă��킭�Ȃ��@�e�T
	case QUEST_SENNIN:
		if(pc == CLASS_KAGEROU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���l�u�L�����@�_�q�v�̕����ȍ~�A��l�Ƃ��ďC�Ƃ��n�߂�҂������Ă���B");
				strcpy(quest_text[line++], "��l�͗d���̑�G���B���Ɏ��������̂悤�ȗ͂̎ア�d�������ɂƂ��ẮB");//��s40���܂�
				strcpy(quest_text[line++], "����ȏ�꒣������������O�ɒ@���Ă����ׂ���������Ȃ��B");
				strcpy(quest_text[line++], "���Ȃ��͂����l���A�_�Ђ̎Q�������l�̏Z���֒P�g�������邱�Ƃɂ����B");
#else
                strcpy(quest_text[line++], "With the resurrection of the saint Toyosatomimi no Miko, hermits are");
				strcpy(quest_text[line++], "growing in number. They're the enemies of you youkai - especially of");//��s40���܂�
				strcpy(quest_text[line++], "weak youkai like you. You probably have to beat them up before they start");
				strcpy(quest_text[line++], "threatening your territory. You decide to go alone and infiltrate the");
				strcpy(quest_text[line++], "hermit residence.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͍����Ȑ��l��ł��|���A��l�W�c�ɒɌ���^���邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "���Ԃ��������Ȃ��Ɋ��ӂ̑��蕨��p�ӂ��Ă��ꂽ�B");
				strcpy(quest_text[line++], "����̐��Ƌ��ɂ��肪�����󂯎�����B");
#else
                strcpy(quest_text[line++], "You've defeated their saint, dealing a painful blow to the hermits.");
				strcpy(quest_text[line++], "Your allies have presented you with a reward in gratitude.");
				strcpy(quest_text[line++], "You accepted it with a joyous howl.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���̐��l�̗͂͗\�z��y���ɒ����Ă����B���Ȃ��͖����炪�瓦���o�����B");
				strcpy(quest_text[line++], "�Â��˂Ŏ����ƒ��ԒB�̑O�r���Ă����Ɍ����Ĉꐺ���̂ł������B");
#else
                strcpy(quest_text[line++], "That saint's power was beyond your expectations, and you ran away for your life.");
				strcpy(quest_text[line++], "You then were howling at the moon, thinking of the future awaiting you and");
				strcpy(quest_text[line++], "your friends.");
#endif
			}
		}
		break;

		//��n�x�����@���@�������o�[
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_KYOUKO ||pc == CLASS_NAZRIN || pc == CLASS_ICHIRIN ||pc == CLASS_MURASA || pc == CLASS_SHOU )
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���@�u�������Ŗ����ɑ��V���I���邱�Ƃ��ł��܂����B�{���ɂ��肪�Ƃ��B�v");
				strcpy(quest_text[line++], "�Ƃ���ŁA���΂炭�O�������Ă������̂����������́B");
				strcpy(quest_text[line++], "���ꂩ��̋M���̏����ɂȂ�Ǝv����B�悯��Ύg���Ă݂ĂˁB�v");
#else
                strcpy(quest_text[line++], "Byakuren - 'Thanks to you, we're able to properly conduct funeral rites.");
				strcpy(quest_text[line++], "I'm really grateful. By the way, I've finished making that thing I've");
				strcpy(quest_text[line++], "been working. I think it'll come in use. Please take it.'");
#endif
			}
		}
		break;

	//��T�ގ��@�킩�����P
	case QUEST_OOGAME:
		if(pc == CLASS_WAKASAGI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͌�����C�̑�T�𒦂炵�߂��B");
				strcpy(quest_text[line++], "��T���Y��Ȏw�ւ����Ȃ��ɂ��ꂽ�B���Ȃ��̂��Ƃ��΂̎傾�Ǝv���Ă���炵���B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You have disciplined the two giant turtles.");
				strcpy(quest_text[line++], "They gave a pretty ring. Looks like they now see you as the ruler of the lake.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͖����炪��΂��瓦���������B");
				strcpy(quest_text[line++], "�����͐_�Ђ̗��̒r�ɂł����ɂȂ邵���Ȃ����낤�E�E");
#else
                strcpy(quest_text[line++], "You ran away for your life, fleeing the lake.");
				strcpy(quest_text[line++], "For the time being, looks like you'll have to live in the pond at the back");
				strcpy(quest_text[line++], "of the shrine...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�΂��j���ł���Ƌ����ȑ�T�̃����X�^�[�ɑ��������B");
				strcpy(quest_text[line++], "���̂܂܂ł͖��������������Ȃ��B���Ƃ����Ȃ��ẮB");
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
				strcpy(quest_text[line++], "�킩�����P�u���΂ŗV�񂶂�_����I");
				strcpy(quest_text[line++], "�΂�f�����낵���������\��Ă���񂾂���I�v");
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
					strcpy(quest_text[line++], "�킩�����P�u���Ȃ����������̉�����|�����́H");
					strcpy(quest_text[line++], "�d���̒[����Ƃ��ė��ꂪ�Ȃ���E�E");
					strcpy(quest_text[line++], "����ɂ�����������ˁB�v");
#else
                    strcpy(quest_text[line++], "Wakasagihime - 'You defeated the monsters?");
					strcpy(quest_text[line++], "You'd make a better youkai than me...");
					strcpy(quest_text[line++], "Take this as your reward.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "�킩�����P�u�R�ł��傤�A���Ȃ������̉�����|�����́H");
					strcpy(quest_text[line++], "�d���̒[����Ƃ��ė��ꂪ�Ȃ���E�E");
					strcpy(quest_text[line++], "����ɂ�����������ˁB�v");
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
				strcpy(quest_text[line++], "�킩�����P�u���΂炭�_�Ђ̗��̒r�ɂł������z����������E�E�v");
#else
                strcpy(quest_text[line++], "Wakasagihime - 'I guess I'll have to move to the pond at the back");
                strcpy(quest_text[line++], "of the shrine for a while...'");
#endif
			}
		}

		break;

	//�Â���@���A�͂���
	case QUEST_OLDCASTLE:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�͂��āu���ˁ[�B���΂͔���̛ޏ����ĂƂ��H");
				strcpy(quest_text[line++], "�ޏ��Ɏ؂�������ςȂ����Ă̂������������A");
				strcpy(quest_text[line++], "��V�ɂ��ꎝ���čs���Ă�B�v");
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
				strcpy(quest_text[line++], "�͂��āu���A���B��̏�̑|�����̐땺��\���t����ꂽ�񂾂��āH");
				strcpy(quest_text[line++], "���l�Ƃ����Ƃ������ȘA���΂��肢��̂ɑ�V��l�����˂��B");
				strcpy(quest_text[line++], "�܁A���͏E���Ă����邩��撣���ĂˁB�v");
#else
                strcpy(quest_text[line++], "Hatate - 'Oh, Momiji. Are you prepared to clean up that old castle?");
				strcpy(quest_text[line++], "There's plenty of nasty enemies like giants and dragons, that's");
				strcpy(quest_text[line++], "enough to give even the Great Tengu paause. Don't worry, I'll");
				strcpy(quest_text[line++], "pick you up if you drop dead.'");
#endif
			}
		}
		else if(pr == RACE_KARASU_TENGU)//v2.0.3 ��V��S�ʓ������b�Z�[�W�ɂ���
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�]���҂�����P�R�r�ł��A�傢�ɖʖڂ��{�����B");
				strcpy(quest_text[line++], "����ɖJ�ܕi�Ƃ��ĈȑO����ڂ����Ă����󕨂�����󂯂邱�Ƃɐ��������B");
#else
                strcpy(quest_text[line++], "You've cleaned up those strangers by yourself, getting lots of attention.");
				strcpy(quest_text[line++], "You also managed to obtain a treasure you've set your eyes upon");
				strcpy(quest_text[line++], "as a reward.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�܂������Ԃ������Ă��܂����B");
				strcpy(quest_text[line++], "�����̊ԐV�����ɂ͊���o�������ɂȂ��c�c");
#else
                strcpy(quest_text[line++], "You've splendidly failed again.");
				strcpy(quest_text[line++], "You'd rather avoid meeting other newspaper reporters for the time being...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�쓌�̌Â���ǂɃ����X�^�[���������H���Ă���Ƃ������𓾂��B");
				strcpy(quest_text[line++], "�ȑO�̐N�������ƈႢ�A����̗]���҂͂��Ȃ若���炵���B");
				strcpy(quest_text[line++], "�F�ɐ�񂶂Ă�����������A�����ԏ�ƃX�N�[�v�l���̈ꋓ������_���̂������Ȃ��B");
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
				strcpy(quest_text[line++], "�͂��āu���΂�腖��l�ˁ[�B");
				strcpy(quest_text[line++], "�����V��l����A����̌��̊��ӂ̈�ɂ��[�߂����������āB");
				strcpy(quest_text[line++], "���ꂶ��A�Ȃ񂩑�V��l�ɌĂ΂�Ă邩��s���Ȃ���B");
				strcpy(quest_text[line++], "���������킠�肪�Ɓ[�B�v");
#else
                strcpy(quest_text[line++], "Hatate - 'Just what I expected from Miss Yama!");
				strcpy(quest_text[line++], "Please accept this gift from Great Tengu as a sign of gratitude.");
				strcpy(quest_text[line++], "Great Tengu's calling me, so I have to go now. Thanks for your help!'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�͂��āu���[��A腖��l���Ď��l���肶��Ȃ��Ƃ���ȋ����Ȃ��̂ˁB");
				strcpy(quest_text[line++], "�܂��d���Ȃ����B�R�̖��͎R�ŉ��Ƃ����Ȃ��Ƃˁ[�B�v");
#else
                strcpy(quest_text[line++], "Hatate - 'Uhh, didn't think they'd be too strong for Miss Yama.");
				strcpy(quest_text[line++], "I guess we have no choice. It's up to us mountain residents");
				strcpy(quest_text[line++], "to resolve our own problems.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�͂��āu����A�Ȃ��腖��l������ȂƂ��ɋ���́H");
				strcpy(quest_text[line++], "���������x�ɒ��Ȃ񂾁B");
				strcpy(quest_text[line++], "����Ȃ炿����Ɨ��݂��Ƃ��Ă������ȁ[�B");
				strcpy(quest_text[line++], "���̊O��̌Â���ǂ����ȉ����̑��ɂȂ�������Ă�́B");
				strcpy(quest_text[line++], "�܂��͏����̓V��B����U�߂���񂾂��ǁA");
				strcpy(quest_text[line++], "�܂��ǂ����Ă����Ă����ɂȂ����A�ł���Ύ�`���Ă����ė~�����ȁB�v");
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
				strcpy(quest_text[line++], "�͂��āu��������B���񂽂���̋󂫉Ƃ̗]���҂��̂����񂾂��āH");
				strcpy(quest_text[line++], "�d���̂����ɂ��̂ˁ[�B");
				strcpy(quest_text[line++], "���łɂ�����d�����܂�Ă݂Ȃ��H");
				strcpy(quest_text[line++], "���x�͓��̌Â���ǂ�B���̋󂫉ƂƂ͔�ו��ɂȂ�Ȃ����G����������B");
				strcpy(quest_text[line++], "�܂������ɂƂ͌���Ȃ����ǁA�r�Ɏ��M������Ȃ����Ă݂Ȃ�B�v");
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
					strcpy(quest_text[line++], "�A���X�u����A������̎O�l�g����Ȃ��́B");
					strcpy(quest_text[line++], "�܂��g����U�镑���Ă��������Ƃ��낾���ǁA��������Ɠs���������́B");
					strcpy(quest_text[line++], "��Ȃ����玄�̉Ƃɍs������ʖڂ�B�v");
#else
                    strcpy(quest_text[line++], "Alice - 'Oh, it's you three?");
					strcpy(quest_text[line++], "I'd like to make some red tea, but you came at a bad time.");
					strcpy(quest_text[line++], "Please stay away from my house, it's dangerous.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "�A���X�u�d���H�E�E�̊��ɖ��ɋ������ˁB");
					strcpy(quest_text[line++], "�˂��M���A�r�Ɏ��M�͂��邩����H");
					strcpy(quest_text[line++], "�������牜�ɍs�����Ƃ���ɂ���Ƃ̒��ɉ������������Ă���́B");
					strcpy(quest_text[line++], "�������ǂ��ɂ��ł��Ȃ��H�v");
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
				strcpy(quest_text[line++], "�A���X�u�d���������|���Ȃ�āA���ꂪ������o�Ă���������������B");
				strcpy(quest_text[line++], "���z���Ɉ�̉����N�����Ă���̂�����B");
				strcpy(quest_text[line++], "�ł��܂��͂�������Ȃ��ƂˁB������������B�v");
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
				strcpy(quest_text[line++], "��ȉ�����|�����B��̂ǂ���������Ă����̂��낤�H");
				strcpy(quest_text[line++], "�Ƃ𒲂ׂĂ���Ƌ���Ɍ��o���̂Ȃ������i�������Ă����B");
#else
                strcpy(quest_text[line++], "You defeated the weird monster. Just how did it get in here?");
				strcpy(quest_text[line++], "As you looked around your house, you noticed an accessory you");
				strcpy(quest_text[line++], "haven't seen before on your mirror stand.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����͗\�z�ȏ�ɋ��͂Ŏ�ɕ����Ȃ������B");
				strcpy(quest_text[line++], "�������̉���𓾂ĉ��Ƃ��Ȃ������A");
				strcpy(quest_text[line++], "���̎؂�͂����ꑊ���������ɈႢ�Ȃ��E�E");
#else
                strcpy(quest_text[line++], "The monster was far stronger than you expected.");
				strcpy(quest_text[line++], "You managed to get Marisa to help you, but paying");
				strcpy(quest_text[line++], "her back is going to cost you quite a bit...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���΂炭����ɂ��Ă��邤���ɉƂɊ�ȉ��������ݕt���Ă����B");
				strcpy(quest_text[line++], "���Ȃ�̋��G�̂悤�����ꍏ�������ǂ�����Ȃ��Ƃ����Ȃ��B");
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
				strcpy(quest_text[line++], "�A���X�u������قǂɂ��Ȃ��̂ˁB");
				strcpy(quest_text[line++], "����ɂ��Ă���������B���ɒN�������������𗊂߂����Ȑl�Ȃ�ċ���������E�E�v");
#else
                strcpy(quest_text[line++], "Alice - 'Sigh, you're just all talk.");
				strcpy(quest_text[line++], "This is getting troublesome. I wonder if there's anyone else");
				strcpy(quest_text[line++], "I can ask for help...'");
#endif
			}
		}

		break;

		//���l�@�p�`�����[
	case QUEST_FLAN:
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ����l���Ȃ��߂邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "���������ŏ\�N�O���琸�����̚b�����Ö򂪂��߉ނɂȂ�Ƃ��낾�����B");
				strcpy(quest_text[line++], "�܂����S�Ȍ��ʂ͂Ȃ�����������ł��܂����B���X�N�}�l�W�����g�͑厖���B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You somehow have managed to calm her down.");
				strcpy(quest_text[line++], "You almost had your asthma remedy you've been preparing for 10 years");
				strcpy(quest_text[line++], "destroyed. While it's still not perfect, you decide to take half of");//��s40���܂�
				strcpy(quest_text[line++], "of it with you. Risk management is an important matter.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�}���ق����󂵂Ă��܂����B�l�X�Ȍ�����������S�ăp�A���B");
				strcpy(quest_text[line++], "�����͐��U�ő�̖���ɈႢ�Ȃ��E�E");
#else
                strcpy(quest_text[line++], "The library is half destroyed, and many of your studies and");
				strcpy(quest_text[line++], "experiments are gone. This is the worst day of your life...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���C�h�d���������p�j�b�N���N�����Ă���B");
				strcpy(quest_text[line++], "�b�𕷂��Ă��v�̂𓾂Ȃ����A���l���ᛂ��N�����Ċق̒��Ŗ\��Ă���炵���B");
				strcpy(quest_text[line++], "���~�B�ƍ��͐_�Ђ̉���œ����A���Ă��Ȃ��B");
				strcpy(quest_text[line++], "�}���قɔ�Q���o��O�ɉ��Ƃ��~�߂Ȃ��Ƃ����Ȃ��B");
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
				strcpy(quest_text[line++], "���Ɉꂵ����������ĕ����֋A�����B");
				strcpy(quest_text[line++], "�p�`�F������ɂƉ���������������ꂽ�B");
				strcpy(quest_text[line++], "���ނƌ��N�ɂȂ�炵���B���̏�k���낤�B");
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
				strcpy(quest_text[line++], "���͈�Ӓ��\��܂�������ƕ����֋A���čs�����B");
				strcpy(quest_text[line++], "����ɂ��Ă��h��ɂԂ��󂵂Ă��ꂽ���̂��B");
				strcpy(quest_text[line++], "���Ƀp�`�F�ɂ͋C�̓łȂ��ƂɂȂ����B");
#else
                strcpy(quest_text[line++], "After rampaging all night, your sister returned to her");
				strcpy(quest_text[line++], "room. However, she caused a lot of destruction. You");
				strcpy(quest_text[line++], "feel sorry for Patchy in particular.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�����Ԃ�ɊقɋA��ƁA���C�h�d���������p�j�b�N���N�����Ă���B");
				strcpy(quest_text[line++], "�b�𕷂��Ă��v�̂𓾂Ȃ����A�����ᛂ��N�����Ė\��Ă���炵���B");
				strcpy(quest_text[line++], "�o�Ƃ��ĐܟB������Ă��K�v������悤���B");
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
				strcpy(quest_text[line++], "���͂⎩���ł͎�ɕ��������ɂȂ��B");
				strcpy(quest_text[line++], "����l���ܟB�Ɍ��������̂ł��̂����Ђ͕t�����낤���A");
				strcpy(quest_text[line++], "�}���ق���ł�����ꂽ�p�`�����[�l�ɂ͍��킹��炪�Ȃ��B");
#else
                strcpy(quest_text[line++], "You couldn't handle her at all.");
				strcpy(quest_text[line++], "Your mistress is going to come and deal with her, but with");
				strcpy(quest_text[line++], "the library destroyed, you can't bring yourself to face");
				strcpy(quest_text[line++], "Lady Patchouli anymore.");
#endif
			}
		}
		break;

		//���g�ւ̎h�q�@�i�鏴�Ŗʎ�����A���S�ā@������̂�
		//�i�Ԃ�
	case QUEST_KILL_MOKO:
		if(pc == CLASS_YOUMU || pc == CLASS_YUYUKO || pc == CLASS_REIMU || pc == CLASS_REMY || pc == CLASS_MARISA || pc == CLASS_ALICE || pc == CLASS_SAKUYA || pc == CLASS_YUKARI)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�P��u�˂��A������x�̎���������Ă݂�C�͂Ȃ��H");
				strcpy(quest_text[line++], "���x�͈�l�Ŗ��g�̂Ƃ���ɍs���ēK���ɂ̂��ė��Ȃ����B");
				strcpy(quest_text[line++], "��V�͂͂��ނ��H�v");
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
				strcpy(quest_text[line++], "�P��u����C�i�o�A���x�����Ƃ���ɗ�����ˁB");
				strcpy(quest_text[line++], "���낻�떅�g�ɂ܂��h�q�ł����낤�Ǝv���́B�s���Ă�����Ⴂ�B�v");
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
				strcpy(quest_text[line++], "�P��u�˂��i�ԁA������Ɩ��g�̂Ƃ���ɂ�����čs���Ȃ��H�v");
#else
                strcpy(quest_text[line++], "Kaguya - 'Hey, Eirin, would you like to visit Mokou?'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�s�����̖����璿�����f�ނ��̎悵���B");
				strcpy(quest_text[line++], "�厖�ɕۊǂ��Ă������B");
#else
                strcpy(quest_text[line++], "You got a rare material from that phoenix girl.");
				strcpy(quest_text[line++], "You decide to keep it in a safe place.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�P��u�����A��l������ŕ��������������Ȃ��́I");
				strcpy(quest_text[line++], "����V��ł���̂�i�ԁI�v");
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
				strcpy(quest_text[line++], "���������΍ŋߖ��g�Ƃ�荇���Ă��Ȃ��B");
				strcpy(quest_text[line++], "���낻�뒼�X�ɏo�����̂��������낤�B");
#else
                strcpy(quest_text[line++], "To think of it, it has been a while since you played with");
				strcpy(quest_text[line++], "Mokou. You decide to go visit her in person.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���g���璿�����f�ނ��̎悵���B");
				strcpy(quest_text[line++], "�V�������Ɏg���Ȃ����낤���B");
#else
                strcpy(quest_text[line++], "You obtained a rare material from Mokou.");
				strcpy(quest_text[line++], "Could this be a new impossible request?");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�P���Ɏ��s�����B���g���F�X���܂�Ęr���グ�Ă���悤���B");
#else
                strcpy(quest_text[line++], "Your assault failed. Looks like Mokou hasn't been neglecting");
                strcpy(quest_text[line++], "her training either.");
#endif
			}

		}
		break;

	case QUEST_KILL_GUYA: //�P��ւ̎h�q�@���g
		if(pc == CLASS_MOKOU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���������΍ŋߋP��Ƃ�荇���Ă��Ȃ��B");
				strcpy(quest_text[line++], "���܂ɂ͂����炩��o�����̂��������낤�B");
#else
                strcpy(quest_text[line++], "To think of it, it has been a while since you played with");
				strcpy(quest_text[line++], "Kaguya. You decide to go visit her in person.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�i��������ł����Ă�����B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�P�邪�Ō�Ɏg�����Ƃ����󕨂𐨂��ŒD���Ă��Ă��܂����B");
				strcpy(quest_text[line++], "�܂��ǂ������̂����D���Ԃ��ɗ��邾�낤�B");
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
				strcpy(quest_text[line++], "�P���Ɏ��s�����B�ǂ�����Ď@�m�����̂��G�̔����͖��S�������B");
				strcpy(quest_text[line++], "�ǂ����ŋߕ���������ł���C������B");
#else
                strcpy(quest_text[line++], "Your assault failed. The enemy was fully prepared.");
				strcpy(quest_text[line++], "You feel as if you've been on a losing streak lately.");
#endif
			}
		}
		else if (pc == CLASS_UDONGE) //v1.1.44 ��p���i
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���ɂ��t���l�𒴂��鎞�������B");
				strcpy(quest_text[line++], "���Ȃ��͒P�g�i�����ւƌ��������c");
#else
                strcpy(quest_text[line++], "Time has finally come for you to overcome your master.");
				strcpy(quest_text[line++], "You head out to Eientei on your own...");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͌���̖��ɂ��t���l�ƕP�l��œ|�����B");
				strcpy(quest_text[line++], "����Ō��̎������X�g���X����J������ċC�����悭�ڊo�߂��邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ��͂ƂĂ��������ł������B");
#else
                strcpy(quest_text[line++], "After a fierce battle, you have defeated both your");
				strcpy(quest_text[line++], "master and the princess. That's going to be good stress");
				strcpy(quest_text[line++], "relief for the real you. You're very happy.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "���t���l�̗͂͂��Ȃ��̑z���ȏゾ�����B");
				strcpy(quest_text[line++], "�܂��\�ꑫ��Ȃ����J�����炵�͂����܂łɂ��Ă������B");
#else
                strcpy(quest_text[line++], "Your master's power was beyond your expectations.");
				strcpy(quest_text[line++], "You still haven't rampaged enough yet, but that'll be");
				strcpy(quest_text[line++], "enough stress relief for now.");
#endif
			}
		}


		break;
	case QUEST_MITHRIL: //�~�X�����T���@�i�Y�[����
		if(pc == CLASS_NAZRIN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u�~�X�����v�𔭌������B");
				strcpy(quest_text[line++], "������ւɉ��H���Ă���������E�E");
#else
                strcpy(quest_text[line++], "You did well in finding some mithril.");
				strcpy(quest_text[line++], "You quickly handed it over to Ichirin...");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "��͂�P�Ƃł̐[�w�T���͖���������悤���B");//��s40���܂�
				strcpy(quest_text[line++], "�����ꂲ��l�̗͂ŉ��Ƃ����Ă��炨���E�E");//��s40���܂�
#else
                strcpy(quest_text[line++], "Just as you expected, exploring deep dungeons is too much");//��s40���܂�
				strcpy(quest_text[line++], "for you to handle on your own. Maybe your master could");//��s40���܂�
				strcpy(quest_text[line++], "do something...");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�S���ɂ͐_��̋����u�~�X�����v������炵���B");
				strcpy(quest_text[line++], "������g���ă��b�h�����Ε�T��������ɈႢ�Ȃ��B");
#else
                strcpy(quest_text[line++], "Looks like there's a mystic ore called mithril in Angband.");
				strcpy(quest_text[line++], "If you'd make a rod out of it, it'll be very useful for");
				strcpy(quest_text[line++], "treasure seeking.");
#endif
			}
		}
		break;
		//�j��2�@��
	case QUEST_DOOM2:
		if(pc == CLASS_BYAKUREN)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���S�����͑ގU���Ă������B�܂��͈���S���B");
				strcpy(quest_text[line++], "���Ȃ��̐g���Ă�����q�����������̗͂��������ɓ���Ă��ꂽ�B");
				strcpy(quest_text[line++], "���肪�����g�킹�Ă��炨���B");
#else
                strcpy(quest_text[line++], "You've banished the demons; that's a relief.");
				strcpy(quest_text[line++], "Your thoughtful disciples presented you with a staff with");
				strcpy(quest_text[line++], "healing power. You accepted it with gratitude.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���S�����͗\�z�ȏ�ɋ��͂Ŏ�ɕ����Ȃ������B");
				strcpy(quest_text[line++], "���z���̑��̎��͎҂����𗊂��ĉ��Ƃ����������񂾂��A");
				strcpy(quest_text[line++], "���΂炭�͔ޏ������ɓ����オ�肻���ɂȂ��E�E");
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
				strcpy(quest_text[line++], "�يE�̈��S�����@���ɐN�U����ĂĂ���B");
				strcpy(quest_text[line++], "�b�͒ʂ��Ȃ��B�키�����Ȃ��悤���B");
				strcpy(quest_text[line++], "���S�̐��ƂɈ�l�ŏ�荞�߂Δ�Q�͍ŏ����ōςނ͂����B");
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
				strcpy(quest_text[line++], "���@�u�������ł��B�M���͂��΂炭���Ȃ������ɑ�ςȏC�s��ς�ł����̂ˁB");
				strcpy(quest_text[line++], "����͗��̕�����i���ĉ����������͂Ȏ����̏�ł��B");
				strcpy(quest_text[line++], "�����̓���i�ł����A���̋M���Ȃ炽�₷���g�����Ȃ���͂���B�v");
#else
                strcpy(quest_text[line++], "Byakuren - 'Well done. I see you've been training a lot while you");
				strcpy(quest_text[line++], "were away. Here's a powerful healing staff donated by the village.");
				strcpy(quest_text[line++], "It's difficult to use, but you should be able to handle it.'");
#endif
			}
		}
		break;

		//�Q�������C�u�@�d��
	case QUEST_LIVE:
		if(pc == CLASS_REIMU)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u����̛ޏ��ł��G��Ȃ��Ƃ́B");
				strcpy(quest_text[line++], "����͗R�X������肾���B������͗��̑����ɂ�����邩�������E�E�v");
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
				strcpy(quest_text[line++], "���Ȃ��͖钆�ɑ������T���U�炷�Q�������C�u�ɂ����䖝���Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "���̎҂͕s���ɔY�܂���Ă��邵�A");
				strcpy(quest_text[line++], "���̂܂܂ł͂��Ȃ��̗��j�Ҏ[�ɂ�������肪�o�邩������Ȃ��B");
				strcpy(quest_text[line++], "���X���\�����A���荞�݂������Ďd�u��������Ă�邱�Ƃɂ����B");
				strcpy(quest_text[line++], "���������C�u�ɂ͍ŋ߂ٕ̈ςŗ͂�t�������͂ȕt�r�_���Q�����Ă���悤���B");
				strcpy(quest_text[line++], "����Ƀ��C�u�̊ϋq�������S�đ���ɂ��Ȃ��Ƃ����Ȃ��B");
				strcpy(quest_text[line++], "���O�ȏ��������Ă���������K�v�����邾�낤�B");
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
				strcpy(quest_text[line++], "���Ȃ��͑匃��̖��ɃQ�������C�u����ł������B");
				strcpy(quest_text[line++], "���q���ɖ߂�����B�c�Ƃ����i���͂��Ă����B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂ȏ񂾁B");
				strcpy(quest_text[line++], "���������̏�Ɠ��l�A�ȒP�ɔR�������ꂽ�肷��͂����B");
				strcpy(quest_text[line++], "�厖�Ɏg�킹�Ă��炨���B");
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
				strcpy(quest_text[line++], "�����������͂ȗd�������͂ƂĂ����Ȃ��̎�ɂ͕����Ȃ������B");
				strcpy(quest_text[line++], "��ނȂ�����_�Ђ֋}�g���o�����B");
				strcpy(quest_text[line++], "����얲�Ȃ牽�Ƃ����Ă���邱�Ƃ��낤�E�E");
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
				strcpy(quest_text[line++], "�d���u�S���A�Q�������C�u�Ƃ������A���ɂ����������̂��ȁB");
				strcpy(quest_text[line++], "�l���ɂ��z��̉B��t�@��������炵�����A���O�͑��v���낤�ȁH");
				strcpy(quest_text[line++], "����ȏ��ɔE�э��񂾂疽�����������Ă�����Ȃ����B�v");
#else
                strcpy(quest_text[line++], "Keine - 'This Guerilla Live group is causing so much trouble!");
				strcpy(quest_text[line++], "Human Village also has several of their fans; are you alright?");
				strcpy(quest_text[line++], "Don't risk your life sneaking into a place like this.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u���̑����̒��ő�\�ꂵ�ė͂Â��Ŗق点�Ă����Ƃ����̂��H");
				strcpy(quest_text[line++], "���͂�p�Y�̊킾�ȁB�㐢�؂�ׂ��Ƃ͂��̂��Ƃ��B");
				strcpy(quest_text[line++], "����������Ă����Ƃ����B�v");
#else
                strcpy(quest_text[line++], "Keine - 'You're saying you subdued that noisy group by force?");
				strcpy(quest_text[line++], "You're already becoming quite the hero. Please, take this.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u������d���͊댯���Ɖ��x�����΁c");
				strcpy(quest_text[line++], "�搶�͂��O�̏������S�z�����B�v");
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
				strcpy(quest_text[line++], "�ŋߌd���͐����Q�s���̂悤���B�b�𕷂��Ă݂�ƁA");
				strcpy(quest_text[line++], "���̋߂��ŃQ�������C�u�Ȃ闐�s�C����������d���B������炵���B");
				strcpy(quest_text[line++], "����ŗ��̏Z�l���s���ɔY��ł��邻�����B");
				strcpy(quest_text[line++], "�����d���ގ��������󂯂Ă������B");
				strcpy(quest_text[line++], "���C�u�ɂ͋��͂ȗd�����������Ă���炵���B");
				strcpy(quest_text[line++], "�v���Ԃ�ɘr����B");
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
				strcpy(quest_text[line++], "�匃��̖��ɃQ�������C�u����ł������B");
				strcpy(quest_text[line++], "���q���ɖ߂�����B�c�Ƃ����i���͂��Ă����B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂ȏ�炵���B");
				strcpy(quest_text[line++], "���������̏�Ɠ��l�A�ȒP�ɔR�������ꂽ�肷��͂����B");
				strcpy(quest_text[line++], "�g�����ɂ͋C�����Ȃ��ẮB");
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
				strcpy(quest_text[line++], "�d�������͗\�z�ȏ�ɋ��͂ŁA�P�ނ�]�V�Ȃ����ꂽ�B");
				strcpy(quest_text[line++], "�d���ގ��͖O����قǂ�������A�܂��܂��C�s������Ȃ��悤���B");
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
				strcpy(quest_text[line++], "�d���u腖��l�A�\����Ȃ��̂ł����܂��͂�݂��Ē����܂��񂩁H");
				strcpy(quest_text[line++], "�钆�ɋ߂��̐X�ő呛�������鍢�����z�炪���܂��āA");
				strcpy(quest_text[line++], "���̎ҒB���s���ɔY��ł���܂��B");
				strcpy(quest_text[line++], "���ɐV��̕t�r�_�̘A�������Ȃ�̋����łǂ��ɂ�����o���Ȃ��ł���̂ł��B�v");
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
				strcpy(quest_text[line++], "�d���u���̊�@���~���Ē����A���ӂ̌��t���������܂���B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂Ȗ�����ł��B����̈�ɂ��󂯎�肭�������B�v");
#else
                strcpy(quest_text[line++], "Keine - 'I can't find proper words of gratitude for saving our village");
				strcpy(quest_text[line++], "from this crisis. Here is a powerful magic device that can restore mana.");
				strcpy(quest_text[line++], "Please accept it as a sign of gratitude.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�d���u���ƁA�r�Ԃ�d���̗͂�����قǂ̂��̂Ƃ́B");
				strcpy(quest_text[line++], "�}���ޏ��Ɏg�����o���܂��傤�B���͂ł�����˂΂Ȃ�܂���ˁB�v");
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
				strcpy(quest_text[line++], "�d���u�����ƍς܂Ȃ��A�������Q���Ă����悤���B");
				strcpy(quest_text[line++], "�ŋߖ�ɂȂ�Ƒ剹�ʂŃ��C�u���n�߂�z�炪���ĂȁB");
				strcpy(quest_text[line++], "�F�Q�s���łقƂقƍ����Ă���񂾂�B�v");
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
					strcpy(quest_text[line++], "�d���u���C�u������ł������̂͂��O�B�������̂��B");
				else
					strcpy(quest_text[line++], "�d���u���C�u������ł������̂͂��O�������̂��B");
#else
                strcpy(quest_text[line++], "Keine - 'You managed to bring down that live concert venue?");
#endif
#ifdef JP
				strcpy(quest_text[line++], "���B������Ă��Ă����̂ɁA�d���Ƃ͎v���Ȃ��������ȁB");
				strcpy(quest_text[line++], "��̈�ɂ�����g���Ă���B���͂��񕜂��鋭�͂ȏ񂾁B");
				strcpy(quest_text[line++], "��{�ƂȂ��M�d�i������ȁB��������󂵂���R�₳�ꂽ�肵�Ȃ��ł����B�v");
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
				strcpy(quest_text[line++], "�d���u��͂肱��ȏ�z�����u�͂ł��Ȃ��ȁB");
				strcpy(quest_text[line++], "�}���ޏ��Ɏg�����o�����Ƃɂ��悤�B�v");
#else
                strcpy(quest_text[line++], "Keine - 'I guess I can't leave them be anymore.");
				strcpy(quest_text[line++], "I'll send for the shrine maiden as soon as possible.'");
#endif
			}

		}

		break;


		//�_�[�N�G���t�̉��@�͓��A�R���A�V��
	case QUEST_DARKELF:
		if(pr == RACE_KAPPA || pr == RACE_YAMAWARO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u�ށA���O���B");
				strcpy(quest_text[line++], "��̗]���ҘA���̃A�W�g�����̋߂��ɂ���炵���B");
				strcpy(quest_text[line++], "������������̐�͂͐S���Ȃ��B");
				strcpy(quest_text[line++], "�������ɂȂ��Ă邤���ɃA�W�g�ɐN�����Đe�ʂ�|���邩�H�v");
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
				strcpy(quest_text[line++], "�R���̖��u������܂����B��������[�߉������B�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'You were a big help. Please accept this.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u�V��l�E�E�^�ʖڂɂ���ĉ�������B�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Can't you tengu get your job done?'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u�ށA�V��l�E�E");
				strcpy(quest_text[line++], "�E�E���͂��̋߂��ɎR���r�炷�]���҂̃A�W�g������̂ł��B");
				strcpy(quest_text[line++], "���Ƃ����ĉ������܂��񂩁B�v");
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
				strcpy(quest_text[line++], "�R���̖��u������܂����B��������[�߉������B�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'You were a big help. Please accept this.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u�^�ʖڂɂ���Ē����܂��񂩁E�E�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Were you even trying...'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u���A�M���́E�E");
				strcpy(quest_text[line++], "���͂��̋߂��ɎR���r�炷�]���҂̃A�W�g������̂ł��B");
				strcpy(quest_text[line++], "���Ƃ����ĉ������܂��񂩁B�v");
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
				strcpy(quest_text[line++], "�u���A�S���|�������ƁH");
				strcpy(quest_text[line++], "�d���ɂ����Ƃ́A���F�z����������|���������Ƃ������Ƃ��B");
				strcpy(quest_text[line++], "����J�������B�J���ɂ������낤�B�v");
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
				strcpy(quest_text[line++], "�R���̖��u�G�ɂ�����Ȃ�̔����͂���悤���ȁB");
				strcpy(quest_text[line++], "�����s���Ă������B�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'Looks like our enemies are getting prepared.");
				strcpy(quest_text[line++], "You're free to go.'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���̖��u�Ȃ񂾗d�����B���x�����B");
				strcpy(quest_text[line++], "���ɓ�����������ȁH���̒��ɔE�э���ŁA");
				strcpy(quest_text[line++], "���ɂǂ�ȓz�����邩��@���Ă��Ă���B�v");
#else
                strcpy(quest_text[line++], "Yamawaro - 'A fairy? You came at a good time.");
				strcpy(quest_text[line++], "You see that cave to the west? Go inside and check out");
				strcpy(quest_text[line++], "who's hiding there.'");
#endif
			}
		}
		break;

	//���Ƃ̕�	������,����,���Ƃ�
	case QUEST_CRYPT:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ���ꀏL���揊�ɋ������Ȃ������B");
				strcpy(quest_text[line++], "�ӂƉ�������ƊZ�̂悤�Ȃ��̂������Ă���B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You've lost interest in exploring the moldy crypt.");
				strcpy(quest_text[line++], "Looks like you found some armor at some point.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "���Ȃ��͂��΂炭���Ă������₪�ċ������Ȃ����Ă��̏����ɂ����B");
#else
                strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. You watched it for a bit, but then you lost");
				strcpy(quest_text[line++], "interest and walked away.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�o�̃y�b�g�B���n���Ɍ����Đ[�������@���Ă���̂����������B");
				strcpy(quest_text[line++], "���Ȃ��͉��ƂȂ��ꏏ�ɓ����Ă݂邱�Ƃɂ����B");
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
				strcpy(quest_text[line++], "�ǂ��ɂ����͂Ȏr�p�t��|�����B");
				strcpy(quest_text[line++], "�傩��Վ��{�[�i�X�Ƃ��ĕs�v�c�ȊZ����������B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You have managed to defeat the powerful necromancers.");
				strcpy(quest_text[line++], "You received a mysterious armor from your owner as a bonus.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "�����͎�Ƃ��ǂ��j�̂ނ���ɍ��邱�ƂɂȂ肻�����E�E");
#else
                strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. Looks like your owner will have troubled times");
				strcpy(quest_text[line++], "ahead...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�ŋߖ����������Ƃ�����l�̖��Œn���ւ̌����@���Ă����B");
				strcpy(quest_text[line++], "��l�̍l�����Ƃ���A�n���ɗ�_�������蒆�ɂ͉��O���Q�����Ă���B");
				strcpy(quest_text[line++], "�y�b�g�̒��ł��킢�̓��ӂȎ��������邱�ƂɂȂ����B");
				strcpy(quest_text[line++], "�w�A�[�`���b�`�x�Ƃ����p�t���ܑ̓|���Ăق����������B");
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
				strcpy(quest_text[line++], "�ǂ��ɂ����͂Ȏr�p�t��|�����B");
				strcpy(quest_text[line++], "�����ɖ߂�ƌ��o���̂Ȃ��Z�������Ă����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You have managed to defeat the powerful necromancers.");
                strcpy(quest_text[line++], "As you return to your room, you notice a suit of armor you");//��s40���܂�
                strcpy(quest_text[line++], "don't recall seeing before.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "�����͐j�̂ނ���ɍ��邱�ƂɂȂ肻�����E�E");
#else
				strcpy(quest_text[line++], "A lot of evil spirits flew out of the hole into the streets");
				strcpy(quest_text[line++], "of Former Hell. Looks like you will have troubled times ahead...");
#endif // JP
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�n���ɗ�_��������A���ɋ��͂ȉ��O���Q�����Ă���B");
				strcpy(quest_text[line++], "���̂܂܂ł͖����������B���Ƃ��|���Ă��܂������B");
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
				strcpy(quest_text[line++], "���Ƃ�u腖��l�Ƃ͂����يE�̎��҂͊Ǌ��O�ł������B");
				strcpy(quest_text[line++], "���̌��̎n���͐���Ȓ����Ƃ̋����Ƃ������Ƃł��肢���܂��ˁB�v");
#else
                strcpy(quest_text[line++], "Satori - 'Those otherworldly undead were out of your");
				strcpy(quest_text[line++], "jurisdiction, Miss Yama? I'd like to cooperate with Ministry");
				strcpy(quest_text[line++], "of Right and Wrong to get this incident resolved.'");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u���x�����ς킹�čς݂܂���B");
				strcpy(quest_text[line++], "�揊�̕󕨂͑S�Ĕފ݂̉^�c��ɂ��[�߂��������B");
				strcpy(quest_text[line++], "���ꂩ�炱����������グ�܂��B");
				strcpy(quest_text[line++], "�����炱���ɂ������̂��N�ɂ�������Ȃ��̂ł����A�����Ȃ��̂ł͂���悤�ł��B�v");
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
		//�_�ւ̒���@�얲�E���c
	case QUEST_KANASUWA:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���c�u���ς�炸�̋����ł��ˁB�������ł����������ł��I");
				strcpy(quest_text[line++], "����ł͂����炪�ܕi�ɂȂ�܂��B�ǂ���������Ƃ����ł��ˁB�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Sanae - 'You're as strong as I expected. I don't like saying it,");
				strcpy(quest_text[line++], "but well done! Here's your prize. Hope you'll get lucky.'");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���c�u���炢���̍Ⴆ�͂ǂ�������ł��H");
				strcpy(quest_text[line++], "����Ȃ��Ƃ��Ⴂ���ꎄ�B�����z���̐M�S���������Ⴂ�܂���H�v");
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
				strcpy(quest_text[line++], "���c�u�ނށc�{�C�o�����狭������Ȃ��ł����B");
				strcpy(quest_text[line++], "����ς�܂��n��N������ĂĂ�񂶂�Ȃ��ł����H�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Sanae - 'Uhh... You're pretty strong when not holding back.");
				strcpy(quest_text[line++], "Are you planning to invade Earth again?'");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���c�u���炠����v�ł����H");
				strcpy(quest_text[line++], "�ǂ��ł��A�����̐_�l�͂�������ł���I�v");
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
				strcpy(quest_text[line++], "�����C�s�����������B���J���Ƃ��č��������Ȋ������������B");
				strcpy(quest_text[line++], "��͂��̊��̏ܕi�̂悤�����A�M�S�𔭊����Ėق��Ă��邱�Ƃɂ����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "You splendidly finished your training and received a valuable scroll");
				strcpy(quest_text[line++], "as a reward. As expected, it looks like that was the planned prize");//��s40���܂�
				strcpy(quest_text[line++], "for the festival, but you decide to keep quiet.");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�_�X�̗͈͂��|�I�ł������B�܂��܂���y�҂Ǝ�������߂��B");
#else
                strcpy(quest_text[line++], "The power of your gods truly is overwhelming...");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�_�ގq�l����u���܂ɂ͒��X�ɏC�s�����Ă�낤�v�Ƃ̗L������t�𒸂����B");
				strcpy(quest_text[line++], "�����Ɨ�̕�W�ւ̒���҂����N���[������������ދ����Ă���������̂��낤�B");
				strcpy(quest_text[line++], "�z�K�q�l���ŋߑދ������Ȃ̂łЂ���Ƃ����痐�����ė����邩������Ȃ��B");
				strcpy(quest_text[line++], "�������菀�����ς܂��Ă���A�낤�B");
#else
                strcpy(quest_text[line++], "Lady Kanako gave a request about 'doing some one-on-one training'.");
				strcpy(quest_text[line++], "She's probably bored because nobody's daring to challenge her at");
				strcpy(quest_text[line++], "current festival. Lady Suwako also seems to be bored lately, so she might");
				strcpy(quest_text[line++], "jump in as well. You'll have to be prepared.");
#endif
			}
		}
		break;
		//�����
	case QUEST_HANAMI:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�얲�u���疂�����B���̉���̊����͂��񂽂�B�Y��ĂȂ��ł��傤�ˁH");
				strcpy(quest_text[line++], "�ł���Δ�ѐ؂苭�������������Ă��Ă�B");
				strcpy(quest_text[line++], "������Ɛ����ׂ��Ă�肽���V�炪����́B�v");
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
				strcpy(quest_text[line++], "�얲�u���̉���̊����͂��񂽂�B�Y��ĂȂ��ł��傤�ˁH");
				strcpy(quest_text[line++], "�ł���Δ�ѐ؂苭�������������Ă��Ă�B");
				strcpy(quest_text[line++], "������Ɛ����ׂ��Ă�肽���V�炪����́B�v");
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
				strcpy(quest_text[line++], "�얲�u���珬�邿��񂶂�Ȃ��B���̉���ɎQ�����Ă݂Ȃ��H");
				strcpy(quest_text[line++], "�ł���Ώ��邿���ɂ���������������ė~������ˁB");
				strcpy(quest_text[line++], "�Ȃ�ׂ������̂���{���肢�����B�v");
#else
                strcpy(quest_text[line++], "Reimu - 'Oh, it's you, Kosuzu. Do you want to visit the next");
				strcpy(quest_text[line++], "banquet? I'd like you to bring some sake as well if possible.");
				strcpy(quest_text[line++], "Bring something particularly strong.'");
#endif
			}
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�얲�u�����H�܂����������͂��Ȃ���B");
				strcpy(quest_text[line++], "���c���畷�������ǁA�w����͂�x�͗ǂ��Ȃ��炵�����B�v");
#else
                strcpy(quest_text[line++], "Reimu - 'Well, I'm not forcing you to do it.");
				strcpy(quest_text[line++], "Sanae told me that's called 'alcohol harassment', and that's");
				strcpy(quest_text[line++], "a bad thing to do.'");
#endif
			}
		}

		break;

	case QUEST_KORI: //�ϒK�푈�@�������Z���t������������
			if(comp)
			{
				if((quest[31].flags & QUEST_FLAG_TEAM_A) && p_ptr->pclass != CLASS_RAN)
				{
					if(p_ptr->pclass == CLASS_CHEN)
					{
#ifdef JP
						strcpy(quest_text[line++], "���u�悭������ȁB���O�̐����Ɏ����@���������B");
						strcpy(quest_text[line++], "���J���ɂ���������悤�B�厖�Ɏg���Ă����B�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Ran - 'Well done! I'm proud of how you've grown.");
						strcpy(quest_text[line++], "Please take this as your reward. Use it well.'");//��s40���܂�
#endif
					}
					else if(p_ptr->pclass == CLASS_YUKARI)
					{
#ifdef JP
						strcpy(quest_text[line++], "���u��A���l�I�H");
						strcpy(quest_text[line++], "�������̌����͎~�ނɎ~�܂�ʎ�������Ă̂��ƂŁA");//��s40���܂�
						strcpy(quest_text[line++], "�����Ė��߂ɔw������́E�E�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Ran - 'L-lady Yukari!?");
						strcpy(quest_text[line++], "Uh, this battle was due to unavoidable circumstances,");//��s40���܂�
						strcpy(quest_text[line++], "I've had no intention of disobeying you...'");//��s40���܂�
#endif
					}
					else if(p_ptr->pclass == CLASS_YUYUKO)
					{
#ifdef JP
						strcpy(quest_text[line++], "���u���͓Y���L��������܂����B");
						strcpy(quest_text[line++], "����̈�Ƃ��Ă�������[�߉������B");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Ran - 'Thank you for your support.");
						strcpy(quest_text[line++], "Please accept this as a sign of gratitude.'");//��s40���܂�
#endif
					}
					else
					{
#ifdef JP
						strcpy(quest_text[line++], "���u�ӂ�A����ŒK�����g�̒����v���m�������낤�B");
						strcpy(quest_text[line++], "���O���Ȃ��Ȃ��ǂ��������������B����͖J�����B�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Ran - 'Hmph, I hope those tanuki have learned their place.");
						strcpy(quest_text[line++], "You did pretty good. Here's your reward.'");//��s40���܂�
#endif
					}
				}
				else if((quest[31].flags & QUEST_FLAG_TEAM_B) && p_ptr->pclass != CLASS_MAMIZOU)
				{
					if(pc == CLASS_NUE)
					{
#ifdef JP
						strcpy(quest_text[line++], "�}�~�]�E�u�ǂ�����ϋ��߁A�K�̗͂��������I");
						strcpy(quest_text[line++], "���������ӂ��邼���B����͗炶��B�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Mamizou - 'See, foxes? That's the power of the tanuki for you!");
						strcpy(quest_text[line++], "Thank you for your assistance. Here's your reward.'");//��s40���܂�
#endif
					}
					else if(pc == CLASS_YUKARI)
					{
#ifdef JP
						strcpy(quest_text[line++], "�}�~�]�E�u���̌ς͂��񂳂�̎艺����Ȃ������񂩁H");
						strcpy(quest_text[line++], "�܂��������ɕt����Ă�����͂���ŋ����߂��Ⴊ�E�E");//��s40���܂�
						strcpy(quest_text[line++], "����������čs���B����ő݂��؂�Ȃ�����B�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Mamizou - 'Wasn't that fox one of your servants?");
						strcpy(quest_text[line++], "Well, I don't mind that if you're fighting on our side...");//��s40���܂�
						strcpy(quest_text[line++], "Take this with you. That'll be your payment.'");//��s40���܂�
#endif
					}
					else
					{
#ifdef JP
						strcpy(quest_text[line++], "�}�~�]�E�u�ǂ�����ϋ��߁A�K�̗͂��������I");
						strcpy(quest_text[line++], "������ӊO�ɂ��̂��B���������Ă����B�v");//��s40���܂�
#else
                        strcpy(quest_text[line++], "Mamizou - 'See, foxes? That's the power of the tanuki for you!");
						strcpy(quest_text[line++], "You did pretty good! Take this with you.'");//��s40���܂�
#endif
					}
				}
				else if(pc == CLASS_NITORI)
				{
#ifdef JP
					strcpy(quest_text[line++], "�ς��K�����قǂɂ��Ȃ��B���Ȃ��͑S�����̂��Ă��܂����B");
					strcpy(quest_text[line++], "���Ȃ��͔�ѓ���̑O�Ɂs���������ɂȂ�t�Ƃ����匊�ɓq���Ă����B");
					strcpy(quest_text[line++], "�q���͂��Ȃ��̈�l�������B���Ԃ���̉��l�̐����S�n�悢�B");
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
						strcpy(quest_text[line++], "�ɂƂ�u������ق��I�ō����斿�F�I");
					else
						strcpy(quest_text[line++], "�ɂƂ�u������ق��I�M���ō�����I");

					strcpy(quest_text[line++], "�������Ŏ��̈�l�����E�E���₢�₱�����̘b�B");//��s40���܂�
					strcpy(quest_text[line++], "���j���ɂ��ꂠ����B���M��̐˗͔������u���B");
					strcpy(quest_text[line++], "�Ȃ��ɋC�ɂ���Ȃ��āB���̋C��������B�v");
#else
                    if(p_ptr->prace == RACE_HUMAN)
						strcpy(quest_text[line++], "Nitori - 'Yahoo! You're the best, my friend!");
					else
						strcpy(quest_text[line++], "Nitori - 'Yahoo! You're the best!");

					strcpy(quest_text[line++], "Thanks to you, I've won... wait, I'm getting sidetracked.");//��s40���܂�
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
					strcpy(quest_text[line++], "���Ȃ��͌ςƒK�̑匈�킩�甇�������̑̂œ����o�����B");
					strcpy(quest_text[line++], "�������ʂ����Â������悤���B");
#else
                    strcpy(quest_text[line++], "You somehow managed to run away from the battle of foxes and tanuki.");
					strcpy(quest_text[line++], "Looks like you underestimated the situation a bit.");
#endif
				}
				if(accept)
				{
#ifdef JP
					strcpy(quest_text[line++], "�v���Ԃ�ɃA�W�g�ɖ߂�Ɖ͓��̒��ԒB���q���Ő���オ���Ă���B");
					strcpy(quest_text[line++], "�b�𕷂��Ă݂�ƁA�ςƒK���[�̗тő�K�͂Ȍ���������炵���B");
					strcpy(quest_text[line++], "���܂����Έ�ׂ��ł��邩������Ȃ��B");
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
					strcpy(quest_text[line++], "�ɂƂ�u����H���������񂽂͑ݖ{���Łc�����ƁB");
					strcpy(quest_text[line++], "�܂�������B���ꂩ��[�̗тŌςƒK�̌������n�܂�񂾁B");
					strcpy(quest_text[line++], "�ʔ������猩�Ă����Ƃ�����B");
					strcpy(quest_text[line++], "��ѓ��肵�����Ȃ�~�߂Ȃ����ǁA�����ɂ������~���Ƃ��񂾂ˁB�v");
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
					strcpy(quest_text[line++], "�ɂƂ�u�܂����ق�Ƃɔ�ѓ��肷��Ƃ͎v��Ȃ������ˁB");
					strcpy(quest_text[line++], "���䂵�Ă�Ȃ���h���Ă�낤���H�T�����ł�����B�v");
#else
                    strcpy(quest_text[line++], "Nitori - 'Wow, I didn't expect you to actually charge in.");
					strcpy(quest_text[line++], "If you're wounded, I have some healing potions with me;");
					strcpy(quest_text[line++], "I'll even give you a 5% discount.'");
#endif
				}

			}

		break;

	case QUEST_WANTED_SEIJA: //���q�ˎҁ@��
		if(pc == CLASS_YUKARI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�܋���̎�z���𔍂������B");
				strcpy(quest_text[line++], "�܊p��J���ď����������؏ܕi�������Ŏg�����ƂɂȂ肻�����B");
#else
                strcpy(quest_text[line++], "Bounty hunt complete.");
				strcpy(quest_text[line++], "You're now free to use the valuable rewards that have been");
				strcpy(quest_text[line++], "prepared for this occassion.");
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���̓V�׋S�͌��z�����̏Z�l��G�ɉ񂵂Ė��������������Ă���B");
				strcpy(quest_text[line++], "���̂܂ܕ��u���Ă͂����Ȃ��B���낻�뒼�X�Ɉ�����n���Ă���悤�B");
#else
                strcpy(quest_text[line++], "Looks like that amanojaku is still running around even after");
				strcpy(quest_text[line++], "having all the residents of Gensoukyou turn against her. You can't");
				strcpy(quest_text[line++], "leave her be like that. Time to confront her directly.");
#endif
			}
		}

		break;

	case QUEST_SPAWNING_PIT: //���B�n���@�ɂƂ�
		if(pc == CLASS_NITORI || pr == RACE_KAPPA)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͑匃��̖��ɑ��H���Ă���������S�ē|���A�͓��̗��̉p�Y�ƂȂ����B");
				strcpy(quest_text[line++], "�͓����Ԃ��痢�̕󕨂̋��͂ȊZ��a�������B");
#else
                strcpy(quest_text[line++], "After a fierce battle, you defeated all those weird monsters living");
				strcpy(quest_text[line++], "here, becoming a hero among the kappas. Your kappa friends gave");
				strcpy(quest_text[line++], "you a powerful suit of armor from the village treasury.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����ނȂ����A�����͉�������Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���Ȃ�������̐g�̐U������l���Ȃ��Ƃ����Ȃ����낤�E�E");//��s40���܂�
#else
                strcpy(quest_text[line++], "You didn't fight well enough, and your water source got polluted.");//��s40���܂�
				strcpy(quest_text[line++], "Looks like you'll have to change your way of living from now on...");//��s40���܂�
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�厖�Ȑ����Ɉٕς��N�����Ă���B");
				strcpy(quest_text[line++], "��̓���̂悤�ȉ��������X�ƐV���ȉ����𐶂ݏo���Ă���̂��B");
				strcpy(quest_text[line++], "���̂܂܂ł͐������͂�Ă��܂��B���Ƃ��������򉻂��˂΂Ȃ�Ȃ��B");
#else
                strcpy(quest_text[line++], "An incident has occured at your water source.");
				strcpy(quest_text[line++], "There are mysterious flesh mound monsters that keep summoning new");
				strcpy(quest_text[line++], "monsters one after another. At this, your water source is going to");
				strcpy(quest_text[line++], "wither away. You have to do everything you can to purify it.");
#endif
			}
		}
		break;
	//�T�g���R���t���N�g	������,����
	case QUEST_SATORI:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u�������A�M���������́E�E����������B");
				strcpy(quest_text[line++], "���A���̖{���~�����́H");
				strcpy(quest_text[line++], "�����A���̖{�Ȃ�D���Ɏ����čs���Ă�����B�v");
#else
                strcpy(quest_text[line++], "Satori - 'Koishi, you... you helped me.");
				strcpy(quest_text[line++], "What, do you want that book?");
				strcpy(quest_text[line++], "Very well, take that book with you if that's what you'd like.'");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������͑�R�̖{�������č��΂����Ȃ��瓦���Ă������B");
				strcpy(quest_text[line++], "���������̒��ɂ͎o�����d�ɕۊǂ��Ă���{���������B���x�����Ă��炨���B");
#else
                strcpy(quest_text[line++], "Marisa run away laughing, carrying plenty of books.");
				strcpy(quest_text[line++], "You're certain one of them was that book your sister was very intent");
				strcpy(quest_text[line++], "on keeping safe.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�������n��a�����X�����������B�������}��������炵���B");
				strcpy(quest_text[line++], "��������̂��낤���B���Ȃ����ꏏ�ɑ҂��Ă݂邱�Ƃɂ����B");
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
				strcpy(quest_text[line++], "���Ƃ�u�܂�������ȕ��������o���Ă���Ƃ͂ˁE�E");
				strcpy(quest_text[line++], "�ł��ǂ��ɂ����ނł����悤�ˁB���肪�Ƃ��B");//��s40���܂�
				strcpy(quest_text[line++], "�M�����~�������Ă��{���������B�����Ă����Ȃ����B");//��s40���܂�
#else
                strcpy(quest_text[line++], "Satori - 'I can't believe she brought something like this...");
				strcpy(quest_text[line++], "Even then, we managed to drive her away. Thank you.");//��s40���܂�
				strcpy(quest_text[line++], "Here's that book you wanted. Take it with you.'");//��s40���܂�
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u���x�ǂ��Ƃ���ɋA���Ă�����ˁB");
				strcpy(quest_text[line++], "���̔������@�g�������ւ��P�����ɗ��邩������Ȃ��́B");
				strcpy(quest_text[line++], "�M�����h�q����`���Ă��傤�����B�v");
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
				strcpy(quest_text[line++], "���Ƃ�u�����b�ɂȂ�܂����B����ɂ��̖{�������グ�܂��B");
				strcpy(quest_text[line++], "���̖��@�g���̖ړ��Ă̕i�������悤�ł����A");//��s40���܂�
				strcpy(quest_text[line++], "����������Ă��܂��������悢���낤�Ǝv���܂��āB�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Satori - 'You were a big help. As a reward, please accept this book.");
				strcpy(quest_text[line++], "Looks like that magician was trying to get it,");//��s40���܂�
				strcpy(quest_text[line++], "so I'm deciding to let go of it.'");//��s40���܂�
#endif
			}
			else if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ƃ�u����腖��l�B���x�ǂ��Ƃ���ɂ�������Ⴂ�܂����B");
				strcpy(quest_text[line++], "�n��a���D�_�E�E�Ƃ�����苭���̏P�����󂯂鋰�ꂪ����̂ł��B");
				strcpy(quest_text[line++], "��낵����Α��̕ߊl�ɗ͂�݂��Ă��������܂��񂩁H�v");
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
				strcpy(quest_text[line++], "������ʃS�[�����͂��Ȃ�̋��G���������A�ǂ��ɂ����̌��ނɐ��������B");
				strcpy(quest_text[line++], "���̍��z�Ȗ��@�����ړ��Ă������悤�����A�������蕥���Ă��܂������H");
#else
                strcpy(quest_text[line++], "Those silent golems were very strong, but you managed to drive");
				strcpy(quest_text[line++], "the burglar away. She was trying to get this valuable spellbook;");
				strcpy(quest_text[line++], "was she going to sell it?");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�{�D�_�͍��΂����Ȃ��瓦���Ă������B");
				strcpy(quest_text[line++], "��Q�𒲍������Ƃ���A�l�Ɍ������Ȃ��K������Ă�����");
				strcpy(quest_text[line++], "���t���̔��������o���̉����炢�̊Ԃɂ������Ă����B");
				strcpy(quest_text[line++], "���Ƃ������Ƃ��낤�B�������ƔR�₵�Ă��܂��ׂ��������̂��B");
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
				strcpy(quest_text[line++], "���̔������@�g�������ւ��P�����ɗ��邩������Ȃ��B");
				strcpy(quest_text[line++], "�y�b�g�B���W�߂Ėh�q����n�߂邱�Ƃɂ����B");
#else
                strcpy(quest_text[line++], "That white and black magician might come and attack your study.");
				strcpy(quest_text[line++], "You've gathered your pets and are preparing to repel the attack.");
#endif
			}
		}

		break;

	case QUEST_WANTED_SUMIREKO: //�悤�������z���ց@�[��^�̖ʁX �ʂ����ǉ�
		if(pc == CLASS_MAMIZOU)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ؐ�u�v��͂��܂��s������B");
				strcpy(quest_text[line++], "������̒��\�͎҂͊F�ɒǂ��񂳂�Ă��邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ炨�O���Q������Ƃ����B�v");
#else
                strcpy(quest_text[line++], "Kasen - 'Everything's going according to plan.");
				strcpy(quest_text[line++], "Everyone's chasing after that mindcrafter girl.");
				strcpy(quest_text[line++], "Feel free to go after her as well.'");
#endif
			}
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ؐ�u�܂����{�[���ɂ���Ȕ閧���������Ȃ�āB");
				strcpy(quest_text[line++], "���얲���O�ɍs���������ɂ���Ă͈�厖���B");
				strcpy(quest_text[line++], "���̂����ɂ��������ł����B�v");
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
				strcpy(quest_text[line++], "�}�~�]�E�u����A�G����݂��Ă����C�ɂȂ������H");
				strcpy(quest_text[line++], "�Ⴄ�̂��B�����c�O����B");
				strcpy(quest_text[line++], "����͂��Ă����A���O����͒��\�͂Ƃ������̂�m���Ă���ȁH");
				strcpy(quest_text[line++], "�����̒��\�͎҂�����������f�r���Ă���B");
				strcpy(quest_text[line++], "�o�������P���|�����Ă��邩������񂩂�C�����邱�Ƃ���B�v");
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
				strcpy(quest_text[line++], "�}�~�]�E�u�Ȃ񂶂�A���O����܂ł��̏��w���Ɍ��z���Ă���̂��H");
				strcpy(quest_text[line++], "�܂��������E�j��Ƃ����Ă͎~�ނ𓾂񂩁B");
				strcpy(quest_text[line++], "�d�u���͒��X�ɂ��Ă��񂶂Ⴜ�B�v");
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
				strcpy(quest_text[line++], "�}�~�]�E�u�v��ʂ�A���̏��w���͂�����ɗ����悤����ȁB");
				strcpy(quest_text[line++], "���A���z���̋��낵���������Ղ苳���Ă�낤���B�v");
#else
                strcpy(quest_text[line++], "Mamizou - 'Everything's going according to plan - that schoolgirl");
				strcpy(quest_text[line++], "is going to come by soon. Let's teach her about how scary");
				strcpy(quest_text[line++], "Gensoukyou can be.'");
#endif
			}
			if(comp && pc == CLASS_REIMU)
			{
#ifdef JP
				strcpy(quest_text[line++], "�}�~�]�E�u�����Ō�ɑz��O�̂��Ƃ��N�������悤���Ⴊ�A");
				strcpy(quest_text[line++], "���܂�����Ă��ꂽ�炵���́B");
				strcpy(quest_text[line++], "����J��������B���������Ă����B�v");
#else
                strcpy(quest_text[line++], "Mamizou - 'I didn't expect that turn of events, but it looks");
				strcpy(quest_text[line++], "like you've done well. Thank you for your hard work. Take");
				strcpy(quest_text[line++], "this with you.'");
#endif
			}
		}
		break;


		//�؂�D�͂������Ĉ���@������
	case QUEST_HECATIA:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�T�O���u���̖������̐��E����߂��Ă��Ȃ��́B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̒��Ԃ����̖��𖲂ɔ���t���Ă���炵����B");
				strcpy(quest_text[line++], "������x���̐��E�Ɍ������A�G��ł��|���Ȃ����B");
				strcpy(quest_text[line++], "�����܂ł��Ȃ����ǁA�������������ɂ͋��ۂ��Ă����ʂ�B�v");
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
				strcpy(quest_text[line++], "��J���ނ��ĉƂɖ߂�ƌ��ւɌ��o���̂Ȃ������u����Ă����B");
				strcpy(quest_text[line++], "�ǂ���炱�ꂪ��V�̂���炵���B");
				strcpy(quest_text[line++], "���ꂾ���猎�̘A���͒����D���Ȃ��񂾁B");
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
				strcpy(quest_text[line++], "���̖������̐��E����߂��Ă��Ȃ��B�܂��n��̐l�Ԃ��g���Ď��Ԃ����E���悤�B");
				strcpy(quest_text[line++], "�����l�������Ȃ��͔����̖��@�g���̏��Ɍ��������B");
				strcpy(quest_text[line++], "�����������ɂ����X���̒����ɏ悹���Ă�������]�v�Ȉꌾ���������Ă��܂����B");
				strcpy(quest_text[line++], "���͂⌎�̖����~�����߂ɂ͎��������̐��E�ɍs�������Ȃ��B");
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
				strcpy(quest_text[line++], "���̖��𖲂̐��E����~�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���̓s�����i�������Ă����B");
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
				strcpy(quest_text[line++], "���̖������̐��E����߂��Ă��Ȃ��B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̈ꖡ����U�����󂯂Ă���悤���B");
				strcpy(quest_text[line++], "���n��Ŏ��R�ɓ������͂͂��Ȃ��������Ȃ��B");
				strcpy(quest_text[line++], "�ň��ʘH���疲�̐��E�ɍs���A���ς̈ꖡ��œ|���Ȃ���΂Ȃ�Ȃ��B");
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
				strcpy(quest_text[line++], "���̖��𖲂̐��E����~�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���̓s�����i�������Ă����B");
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
				strcpy(quest_text[line++], "���̖����܂����̐��E�ɋ������Ă���B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̒��Ԃ��܂���������񂵂Ă���炵���B");
				strcpy(quest_text[line++], "���̐��E�̊Ď��҂Ƃ��Ď������ꔧ�E�����Ƃɂ����B");
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
				strcpy(quest_text[line++], "�ǂ��ɂ����G��|���A���̖��𖲂̐��E����ǂ��o�����B");
				strcpy(quest_text[line++], "�Ȃ�������������̕i����z���Ă����B");
				strcpy(quest_text[line++], "���̌��l�����ł܂����������Ă����̂�������Ȃ��B");
#else
                strcpy(quest_text[line++], "You've defeated a powerful enemy and rescued the Lunarians.");
				strcpy(quest_text[line++], "For some reason, Marisa brought you a reward.");
				strcpy(quest_text[line++], "Those Lunarians might have been planning something");
				strcpy(quest_text[line++], "behind the scenes.");
#endif
			}
		}
		break;

		//���œ|���C���N�G�X�g
	case QUEST_YUKARI:
		if (pc == CLASS_EIKI)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���z���ɐ[���Ȉٕς��N�����Ă���B");
				strcpy(quest_text[line++], "�S���ƌĂ΂�鋐��ȃ_���W�������o�����يE�̉��������o�����̂��B");
				strcpy(quest_text[line++], "�S���̒��w�Ŕ��_�����Ή��ɒǂ��Ă���悤�����A");
				strcpy(quest_text[line++], "���낻�날�̎҂ɂ�����������������Ă��˂΂Ȃ�܂��B");
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

		//���ΐ��N�œ|���C���N�G�X�g
	case QUEST_TAISAI:
		if(pc == CLASS_SEIJA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�S���̒ꂩ�狭�͂Ȑ_�������o�����Ƃ��Ă���炵���B");
				strcpy(quest_text[line++], "���Ȃ��͂���𕷂��A���Ƃ��Ă�����𖄂ߒ����Ă�낤�ƌ��ӂ����B");
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
				strcpy(quest_text[line++], "�A���o�[�̉��̗͂Ō��z���̒n�����S���ƌq�����Ă��܂����B");
				strcpy(quest_text[line++], "���̉e���ł��낤���Ƃ��n���̑��ΐ��N���@��o����Ă��܂����B");
				strcpy(quest_text[line++], "��x��ɂȂ�O�ɖ��ߒ����Ȃ��Ƃ����Ȃ��B");
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
				strcpy(quest_text[line++], "���z�������ΐ��N�̋��ЂɎN����Ă���I");
				strcpy(quest_text[line++], "���̂Ƃ��̖��͂�͂肱�̗\���������I");
				strcpy(quest_text[line++], "���̎��������őO���Ő��Ȃ���΁I");
#else
				strcpy(quest_text[line++], "Taisui Xingjun threatens Gensoukyou!");
				strcpy(quest_text[line++], "The dream you saw back then was an omen after all!");
				strcpy(quest_text[line++], "You absolutely have to go forth and fight!");
#endif
			}
		}

		break;

		//�T�[�y���g�œ|���C���N�G�X�g
	case QUEST_SERPENT:
		//v1.1.18
		if(pc == CLASS_JUNKO || pc == CLASS_HECATIA || pc == CLASS_CLOWNPIECE || pc == CLASS_VFS_CLOWNPIECE)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "����ٕ̈ς̌����́u���ׂ̃T�[�y���g�v�Ƃ������̋���ȑ��݂������B");
				strcpy(quest_text[line++], "���̊C�ɃT�[�y���g�̐��ޗ̈悩��̖傪�J���A���܌��̓s���N�����󂯂Ă���B");
				strcpy(quest_text[line++], "���Ȃ��̓T�[�y���g��|���ɍs���Ă��������A���̓s���łԂ̂��������߂Ă��Ă������B");
#else
                strcpy(quest_text[line++], "The cause of this incident was a great being called Serpent of Chaos.");
				strcpy(quest_text[line++], "A gate to the Serpent's realm has opened at the Lunar Mare, and Lunar");
				strcpy(quest_text[line++], "Capital is being invaded. You can go and defeat the Serpent, or just");
				strcpy(quest_text[line++], "laugh as you watch Lunar Capital being destroyed.");
#endif
			}
		}

		break;

		//�N�G�X�g40-48�̓����N�G
		//�N�G�X�g49�͋󂫁H

		//�N�G�X�g50,51�͖�������p

		//�p�m�ق̕󕨌�
	case QUEST_VAULT:
		if(CLASS_IS_PRISM_SISTERS)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͊ق̑��ɌÂ���������Ă��邱�Ƃ��v���o�����B");
				strcpy(quest_text[line++], "����ٕ̈ς͎苭���`���ɂȂ肻�������A");
				strcpy(quest_text[line++], "���ꂪ����Ώ����ɂȂ邩������Ȃ��B�܂����ĉ�����ɍs�����B");
#else
                strcpy(quest_text[line++], "You remember there's an old tool lying around in the warehouse");
				strcpy(quest_text[line++], "at your mansion. Looks like your quest is going to be tough, and");
				strcpy(quest_text[line++], "that item might come in handy. You decide to go in and collect it.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�\�z�ȏ�̓�s���������A�Ƃ���������̉���ɐ��������B");
#else
                strcpy(quest_text[line++], "It was unexpectedly tough, but you managed to retrieve your item.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "����̉���Ɏ��s�����B");
				strcpy(quest_text[line++], "�ٕς̉e���Ȃ̂����������̉e���Ȃ̂��킩��Ȃ����A");
				strcpy(quest_text[line++], "���΂炭���Ȃ������ɑ��͖����ɂȂ��Ă����B");
#else
                strcpy(quest_text[line++], "You have failed to retrieve your item.");
				strcpy(quest_text[line++], "You don't know if it's because of the incident or because of you three,");
				strcpy(quest_text[line++], "but the warehouse became a monster lair while you were away.");
#endif
			}

		}
		break;

	case QUEST_KAGUYA: //�܂̓��@�i��
		if(pc == CLASS_EIRIN)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�P��u�˂��i�ԁA�_���W�����̉��ɂ͂��̓�肪�{���ɂ��邩������Ȃ���B");
				strcpy(quest_text[line++], "�ꏏ�ɒT���ɍs���܂��傤��B�v");
#else
                strcpy(quest_text[line++], "Kaguya - 'Hey, Eirin, my impossible requests might actually exist");
				strcpy(quest_text[line++], "deep in the dungeon. Let's go look for them together.'");
#endif
			}
		}

		break;
	case QUEST_SHELOB: //�V�F���u�ގ��@�S�A���}��
		if(pr == RACE_ONI)
		{
			if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���}���u���̘r���߁A���΂͋S���˂��B");
				strcpy(quest_text[line++], "�������Ă��B���ړ��Ă͂��ꂾ�낤�H�v");//��s40���܂�
#else
                strcpy(quest_text[line++], "Yamame 'What a feat of strength! Just what you'd expect");
				strcpy(quest_text[line++], "from an oni. I know. Are you looking for this?'");//��s40���܂�
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���}���u���₨��S�Ƃ����낤�҂��ǂ��������Ă̂��B");//��s40���܂�
				strcpy(quest_text[line++], "�������ł����񂾂̂����H");//��s40���܂�
#else
                strcpy(quest_text[line++], "Yamame - 'Well, I didn't expect a display like that from an oni.");//��s40���܂�
				strcpy(quest_text[line++], "Did you drink some bad sake or what?'");//��s40���܂�
#endif
			}
		}
		else if(pc == CLASS_YAMAME)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͍ŋߋߏ��ɏZ�ݒ��������\�҂̒w偂ɉ䖝���Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "�V�F���u�ƌĂ΂�邻�̒w偂͌��̋C�ƐH���C�������ɂȂ��悤���B");
				strcpy(quest_text[line++], "�����҂��W�܂�n��Ƃ����ǒʂ��ׂ��؂͂���B�����̗��V�������Ă�낤�B");
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
				strcpy(quest_text[line++], "�]���҂͔��X�̑̂łǂ����֓����Ă������B");
				strcpy(quest_text[line++], "�w偂̓Őj���n�ʂɗ����Ă���̂��������B");
				strcpy(quest_text[line++], "��������H����΋��͂ȕ��킪�o���邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ��͑�����ƂɎ��|�������E�E");
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
				strcpy(quest_text[line++], "�]���҂𑊎�ɕs�o������Ă��܂����B");
				strcpy(quest_text[line++], "�����͒p��E��Œn��̍r����҂����𗊂邵���Ȃ����낤�E�E");
#else
                strcpy(quest_text[line++], "The stranger has caught you unaware.");
				strcpy(quest_text[line++], "You have no choice but to plead other violent beings");
				strcpy(quest_text[line++], "underground for help...");
#endif
			}

		}
		break;

		//�V���p�@�h���΍��C��
	case QUEST_TYPHOON:

		if (pc == CLASS_MEGUMU)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "��N�ɂȂ��\�����l���ɋ߂Â��Ă���B");
				strcpy(quest_text[line++], "����͑䕗�ƌĂ΂��O�E�̗d�����B");
				strcpy(quest_text[line++], "����Ɋ�Â������閧���Ɍ��ނ��Ȃ���΂Ȃ�Ȃ��B");
#else
				strcpy(quest_text[line++], "An unusually powerful storm is approaching Human Village.");
				strcpy(quest_text[line++], "It is a typhoon youkai from the Outside World.");
				strcpy(quest_text[line++], "You have to secretly drive it away.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�閧�C���͖��Ȃ����������B");
				strcpy(quest_text[line++], "�����ƍ����ȓ��݂��F�Ɏx�����Ă�邾���̗\�Z���m�ۂł����B");
#else
				strcpy(quest_text[line++], "You completed your secret mission without problems.");
				strcpy(quest_text[line++], "Your budget now allows you to give everyone more valuable");
				strcpy(quest_text[line++], "tokins.");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�l���ɔ�Q���o�����Ԃ�Ƃ��Ă��܂����B");
				strcpy(quest_text[line++], "���̑�V��ɂ����͂��Ă��炢�ǂ��ɂ����Ȃ��𓾂����A");
				strcpy(quest_text[line++], "����S�N���炢�͉A��������ꂻ���ł���B");
#else
				strcpy(quest_text[line++], "Your failure put Human Village into peril.");
				strcpy(quest_text[line++], "You still managed to save the day by working together with");
				strcpy(quest_text[line++], "other Great Tengu, but they'll be whispering behind your back");
				strcpy(quest_text[line++], "for years to come.");
#endif
			}

		}
		break;


		//�}������N�G�@���ʂ�\��o�����ꏈ��
	case QUEST_TORRENT:
		{
			if(comp)
			{
				int i;
				int j=0;
				cptr name_list[3];
				bool flag = FALSE;

				//���X�g�ɋL�^���ꂽ�^�[�����Ƌ����ď��ʂ����߂�
				for(i=0;i<3;i++)
				{
					//���Ɠ����l�̓��X�g�����΂�
					if(torrent_quest_score_list[j].class_id == p_ptr->pclass || torrent_quest_score_list[j].class_id2 == p_ptr->pclass )
						j++;
					//�����܂������N�C�����ĂȂ��ꍇ���X�g�̃^�[���Ɣ�r�A���������̖��O�����X�g�ɓ����
					if(!flag && (int)qrkdr_rec_turn < torrent_quest_score_list[j].turn)
					{
						name_list[i] = player_name;
						flag = TRUE;//���܂����t���O
						qrkdr_rec_turn = 1 + i;//�L�^�^�[���̕ϐ����u���ʂ��������v�ɕς���
					}
					else
					{
						name_list[i] = torrent_quest_score_list[j].name;
						j++;
					}
				}

#ifdef JP
				strcpy(quest_text[line++], "���̌��ʂ�����o����Ă���B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], format("�D���@�F%s",name_list[0]));
				strcpy(quest_text[line++], format("���D���F%s",name_list[1]));
				strcpy(quest_text[line++], format("��O�ʁF%s",name_list[2]));
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

		//���@�q�_���_�ގ�
	case QUEST_MORIYA_1:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�R�̒����̑��寂̒r���q�_���_�ɏP���Ă���B");
				strcpy(quest_text[line++], "�����Ă����Ă����寂����Ƃ����邾�낤���A");
				strcpy(quest_text[line++], "�����͎��݂��ĉ��𔄂��Ă������B");
#else
                strcpy(quest_text[line++], "The Great Toad Pond at the mountain is under attack by");
				strcpy(quest_text[line++], "hidarugami. Great Toad probably will do something about it");
				strcpy(quest_text[line++], "if you leave it be, but there might be a profit in lending a hand.");
#endif
			}
			else if(comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "���寂̒r�ɕ��Ђ�u�����Ƃɐ��������B");
				strcpy(quest_text[line++], "�͂������l������M���W�܂��Ă���̂�������B");
#else
                strcpy(quest_text[line++], "You managed to establish a branch shrine at the Great Toad Pond.");
				strcpy(quest_text[line++], "Even if it's only a slight amount, you still feel faith from the");
				strcpy(quest_text[line++], "villagers growing.");
#endif
			}
			else if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���̂悤�Ȋi�̒Ⴂ�_��ɕs�o�����Ƃ͏�Ȃ��B");
				strcpy(quest_text[line++], "�×����A���������Ă͐�͏o���Ȃ����̂��B");
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
				strcpy(quest_text[line++], "�_�ގq�u�R�̒����̑��寂̒r���q�_���_�ɏP���Ă����ł����āB");
				strcpy(quest_text[line++], "�����͎��݂��Ă����āA����悭�΂���[�����Ђɂ��ė��Ă�B�v");
#else
                strcpy(quest_text[line++], "Kanako - 'The Great Toad Pond at the mountain is under attack");
				strcpy(quest_text[line++], "by hidarugami. If we lend them a hand, we might establish a new");
				strcpy(quest_text[line++], "branch shrine.'");
#endif
			}
		}
		break;

		//�̗p�����񎟕�W�@�B��ޑœ|�N�G
	case QUEST_OKINA:
		//�얲�A�������A���@�@�`���m�@�@�����@�@��ʓV��@�@�ŃZ���t�ς���K�v�����邩
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA)
		{
			if(accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�G�߈ٕς̎�ƂƂ̐킢�͓��˂ɏI����Ă��܂����B");
				strcpy(quest_text[line++], "���Ȃ��͏����𐮂��čĐ�ɒ��ނ��Ƃ����ӂ����B");
				strcpy(quest_text[line++], "�ǂ�����΋����r����h�����Ƃ��ł��邾�낤�B");
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
				strcpy(quest_text[line++], "�܂����s�o������Ă��܂����B");
				strcpy(quest_text[line++], "��Δ�_�̗͂͂��܂�ɂ�����ł������B");
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
				strcpy(quest_text[line++], "�֎q�ɍ������̂����ȓz�ɓ������Ă��܂����B");
				strcpy(quest_text[line++], "���������T���Ă���ƁA���o���̂���n�����������B");
				strcpy(quest_text[line++], "�����ɓˌ�����΂܂����̏ꏊ�ɍs���邩������Ȃ��B");
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
				strcpy(quest_text[line++], "�Ȃ������񂺂�G��Ȃ������B");
				strcpy(quest_text[line++], "����ς肠��͈֎q�ɍ����Ă����̂Ƃ͕ʂ̓z�ɈႢ�Ȃ��B");
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
				strcpy(quest_text[line++], "�`���m�u�����̔w���ɔ����J���Ă���ȁB");
				strcpy(quest_text[line++], "���ɓ���΂܂��w�������[�̂Ƃ���֍s���邼�B�v");
				strcpy(quest_text[line++], "�s�[�X�u����Ȃ炠�̔�_�ɂ�����Ɨp������񂾁B");
				strcpy(quest_text[line++], "�݂�ȕt�������Ă���Ȃ��H�v");
				strcpy(quest_text[line++], "�T�j�[�u��[���A�˓���I�v");
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
				strcpy(quest_text[line++], "���i�u���A���ʂ��Ǝv������c�c�v");
				strcpy(quest_text[line++], "�`���m�u���������ȁH�O�͉��Ƃ��Ȃ����̂ɁB�v");
				strcpy(quest_text[line++], "�s�[�X�u�����A�{�X�ɓ{��ꂿ�܂��c�c�v");
#else
                strcpy(quest_text[line++], "Luna - 'I-I thought I'd die...'");
				strcpy(quest_text[line++], "Cirno - 'That's weird. I could deal with her before.'");
				strcpy(quest_text[line++], "Piece - 'My boss is going to get angry...'");
#endif
			}
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "�����o�u������������Ȃɋ����Ȃ��Ă��Ȃ�Ė��݂����ˁB�v");
				strcpy(quest_text[line++], "�X�^�[�u���J��������������B�F�̕󕨂ɂ��܂��傤�I�v");
				strcpy(quest_text[line++], "�s�[�X�u���ł��V�тɗ������Č����Ă��ꂽ���A�C�����������I�v");
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
				strcpy(quest_text[line++], "���T�u������Ɓ[�A��������܂��V������������C�H�v");
				strcpy(quest_text[line++], "���u�ӂ��A�\�z�O�͐l���̍ō��̃X�p�C�X���I�v");
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
				strcpy(quest_text[line++], "���T�u������Ɓ[�A��������܂��d�������������C�H�v");
				strcpy(quest_text[line++], "���u�ӂ��A�\�z�O�͐l���̍ō��̃X�p�C�X���I�v");
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
				strcpy(quest_text[line++], "�v���Ԃ�ɐX�ɋA������A���o���̂Ȃ��������ȓ�l�g������B");
				strcpy(quest_text[line++], "��l�͂��Ȃ����X�J�E�g�������ƌ����o�����B");
				strcpy(quest_text[line++], "���Ȃ��͂��̓�l�̑f���ɐS�����肪�������E�E�E");
#else
                strcpy(quest_text[line++], "You returned to the forest after being away for a while,");
				strcpy(quest_text[line++], "and ran into two suspicious people. They told you they're");
				strcpy(quest_text[line++], "trying to recruit you. You think you remember those two...");
#endif
			}
		}
		break;
		//v1.1.36 ���鎸�H�@�Ō�̉���ɏo���ʎq�̎��s�Z���t�ύX�@���ƈ�l���ꂩ�������l�����邪
	case QUEST_KOSUZU:
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA || pc == CLASS_MAMIZOU
			|| pc == CLASS_SANAE || pc == CLASS_YUYUKO || pc == CLASS_YOUMU || pc == CLASS_REMY
			 || pc == CLASS_SAKUYA	)
		{
			if(fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u����̉���ł͂����b�ɂȂ�܂����B");
				strcpy(quest_text[line++], "�ЂƂ܂��ۂ����܂��ĉ����Ƃ����Ƃ���ł��傤���B");
				strcpy(quest_text[line++], "����ɂ��Ă�����̐悪�v��������c�v");
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
				strcpy(quest_text[line++], "�����u�˂��A�ݖ{���̏�����ǂ����Ō��Ȃ������H");
				strcpy(quest_text[line++], "�@���܍s���s���ɂȂ��ĂĊF�ŒT���Ă�́B");
				strcpy(quest_text[line++], "�@�ł���΂��F�B�ɂ������Ă݂Ăق�����B�v");
#else
				strcpy(quest_text[line++], "Akyuu - 'Hey, have you seen Kosuzu from the bookstore?");
				strcpy(quest_text[line++], "  She's gone missing, and we're searching for her.");
				strcpy(quest_text[line++], "  Please ask your friends about her if you can.'");
#endif
			}
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����u�����A����Ȏ��ɂȂ��Ă����Ȃ��...");
				strcpy(quest_text[line++], "�@�d���̂��Ȃ����A��߂��Ă��ꂽ�͙̂F��Ƃ������̂ˁB");
				strcpy(quest_text[line++], "�@���̃����b�N�͂����邩��ł���΂��܂�\�ɂ��Ȃ��ł����āB�v");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@�@�@�@�@�@�@�@(�d���̊댯�x������C�������ق�������������...)");
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
				strcpy(quest_text[line++], "�����u�_�Ђŏ���ɏP��ꂽ�ł����āH");
				strcpy(quest_text[line++], "�@���肪�Ƃ��A�����ɕی삳�����B");
				strcpy(quest_text[line++], "�@���邪���f�����Ă��߂�Ȃ����ˁB");
				strcpy(quest_text[line++], "�@�ł����̂��Ƃ͂ł���Δ閧�ɂ��Ă����āH�v");
#else
				strcpy(quest_text[line++], "Akyuu - 'You got attacked by Kosuzu at the shrine?");
				strcpy(quest_text[line++], "  Thank you, we'll secure her right away.");
				strcpy(quest_text[line++], "  Sorry for the trouble she caused you.");
				strcpy(quest_text[line++], "  Could you please keep this a secret?'");
#endif

			}

		}



		break;

		//���̐��E�́��œ|�N�G�@v1.1.52 俎q���ꐫ�i�Z���t�ύX
	case QUEST_DREAMDWELLER:
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�B��ށu���ɂ��O�ɂƂ��čő�̈����Ƃ̑Ό��̎��������B");
				strcpy(quest_text[line++], "�����ł������Ă݂���I�������]�����Ă����Ȃ�I�v");
#else
                strcpy(quest_text[line++], "Okina - 'It's time for you to confront your worst nightmare.");
				strcpy(quest_text[line++], "Show me a splendid victory! Don't let me down!'");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�B��ށu�V����ł���I�悭���������g�ɑł��������I");
				strcpy(quest_text[line++], "���āA���̂܂ܖڊo�߂�Ƃ��O�ƌ��z���Ƃ̉��͐؂�Ă��܂����낤�B");
				strcpy(quest_text[line++], "�����Ŏ�����˂̗͂��g���Ă��O�ƌ��z���̌q������ێ����Ă�낤�B");
				strcpy(quest_text[line++], "����ł��O�͖������ɖ{�����I");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "�Ƃ���ŁA�ǂ������玄�̏��œ�������͂Ȃ��H�v");
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
				strcpy(quest_text[line++], "�h���~�[�u���Ȃ��͂����������瓦����܂���B");
				strcpy(quest_text[line++], "���z�Ɂc�c���������āc�c�v");
#else
                strcpy(quest_text[line++], "Doremy - 'You can no longer escape from your nightmares.");
				strcpy(quest_text[line++], "Pitiful... Stay strong...'");
#endif
			}

		}
		break;

		//�V�̑f�Ă��[���N�G
	case QUEST_DELIVER_EEL:
		break;


		//���ۍ��@���_��腖��̂Ƃ�������ƃZ���t�ς��� �v�̉̂̂Ƃ����ς���
	case QUEST_SHORELINE2:

		if (pc == CLASS_KUTAKA)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�v���Ԃ�ɐE��ɖ߂��Ĉꑧ���Ă���ƁA��i����ً}�̌Ăяo�����󂯂��B");
				strcpy(quest_text[line++], "�p���͐��ۍ��̍Ĕ��߂ł������B");
				strcpy(quest_text[line++], "���̓����삪���x�͒n���̕ʂ̃G���A�ւ̐N�U����ĂĂ���炵���B");
				strcpy(quest_text[line++], "�n���̑�66�K�w�֌��������B�v");
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
				strcpy(quest_text[line++], "�����ȋ������ւ�w鋋� ���S�x��œ|�����B");
				strcpy(quest_text[line++], "���̌��тɂ��Վ��{�[�i�X���o�邪�A");
				strcpy(quest_text[line++], "�c�O�Ȃ���\�Z�̓s���Ō����x���ƂȂ����B");
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
				strcpy(quest_text[line++], "�v�̉́u���̓����삪���x�͒n���̕ʂ̃G���A�ւ̐N�U����ĂĂ��܂��B");
				strcpy(quest_text[line++], "���Ԃ��d��������w���͐��ۍ��̍Ĕ��߂ɓ��ݐ؂�܂����B");
				strcpy(quest_text[line++], "��̋󂢂Ă�����͕������߂��邢�͓����Ɍ������悤�ɂƂ̂��Ƃł��B�v");
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
				strcpy(quest_text[line++], "�v�̉́u�ޏ����悤�₭���肽�悤�ŁA");
				strcpy(quest_text[line++], "�����͒{���E�̎x�z�ɒ��͂������̂悤�ł��B");
				strcpy(quest_text[line++], "�܂��{���E�����̂��ƂȂ炱����̊֒m���邱�Ƃł͂���܂����ˁB");
				strcpy(quest_text[line++], "�M���ւ̗Վ��{�[�i�X���o�Ă��܂��̂ł�����Ɏ�̈�����肢���܂��B�v");
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

		//�j�ŃN�G3 �ݕP�˗��ŃT�C�o�[�f�[�����ގ�
	case QUEST_DOOM3:
		if (pr == RACE_HANIWA) //v1.1.66
		{
			if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ݕP�u�O�̐��E�ł͂���ȋ����ȕ��킪�g���Ă���̂ˁB");
				strcpy(quest_text[line++], "�@��������ł��|���Ă���Ƃ́A���Ȃ��̒������������v����B");
				strcpy(quest_text[line++], "�@���āA�����J������炳�Ȃ��ƂˁB");
				strcpy(quest_text[line++], "�@���̐V�삾�����񂾂��ǋM���ɂ������B�v");
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
				strcpy(quest_text[line++], "�ݕP�u�O�̐��E�ł͂���ȕ��킪�g���Ă���̂��B");
				strcpy(quest_text[line++], "�@���͂����ǔ������Ɍ����Ă����ˁB");
				strcpy(quest_text[line++], "�@�}���Ŗh���\���̊J����i�߂܂��傤�B");
				strcpy(quest_text[line++], "�@�M���͐w�`�̌����������Ȃ����B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@����ɂ��Ă����̒{���ǂ��A");
				strcpy(quest_text[line++], "�@����Ȃ��̂��Ăэ���Ŏ�j����������ł���̂�����H�v");
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

		//v1.1.65 �H�ߍ��� ����푰��p�@���j�[�N�N���X�̂Ƃ������䎌��ς��Ă���
	case QUEST_KONNKATSU:
		if (cp_ptr->flag_only_unique)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�V�q�u���傤�ǂ����Ƃ���ɗ����ȁB������Ɨ��܂�Ē��ՁB");
				strcpy(quest_text[line++], "���͍����Ɏ��s���ĉH�߂𖳂������Ԕ����ȓV�������ĂˁB ");
				strcpy(quest_text[line++], "�����ɋ����t����ĕ����Ă����Ȃ��Ȃ����ߋ肪���ɗ���ł����񂾁B ");
				strcpy(quest_text[line++], "�ł��z�؂�ꖇ�̂��߂ɒn���n���T�����Ȃ�Ď��͂܂��҂炾����A ");
				strcpy(quest_text[line++], "�����݂̗L�肻���ȓz��T���Ă����Ă킯�B ");
				strcpy(quest_text[line++], "���������킯�ŉH�߂��ꖇ�T���Ă��Ȃ����B ");
				strcpy(quest_text[line++], "���R���񂶂傻����̉H�߂���ʖڂ�B ");
				strcpy(quest_text[line++], "�V�E�ɂӂ��킵���y���ʐ��z�̉H�߂���Ȃ��ƂˁB�v ");
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
	case QUEST_MIYOI: //v1.1.78 �h���[���C�[�^�[�̐��Ё@�h���~�[
		if (pc == CLASS_DOREMY)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�������������B�̎��L�����͐H�זO���܂����B");
				strcpy(quest_text[line++], "�����Ȃ��͐_�Ђ̉���ɎQ�����Ĉ��Y�҂𒦂炵�߂邵������܂���B");
				strcpy(quest_text[line++], "�S�̎��̂����炷���e�͋����ɂ܂�Ȃ��ł����A");
				strcpy(quest_text[line++], "�����������܂������߂Ώ����邩������܂���ˁB");
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
				strcpy(quest_text[line++], "�����Ǝv�����͂̎g����͂ǂ����ɏ����Ă��܂��܂����B");
				strcpy(quest_text[line++], "�ǂ����S�̕Z�\�̒��ɓ������񂾂悤�ł����A");
				strcpy(quest_text[line++], "�Ƃ肠���������a�����Ă����Έ����͂ł��Ȃ��ł��傤�B");
#else
                strcpy(quest_text[line++], "Looks like whoever was inflicting drunkedness has fled away");
				strcpy(quest_text[line++], "somewhere. It's probably inside that oni gourd, but you decide");
				strcpy(quest_text[line++], "it can't do anything if you take it with you.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�����̉����ɒx������Ƃ͖��̎x�z�҂̖��܂�ł��B");
				strcpy(quest_text[line++], "���̂܂܂ł͐����Ŋ̑�������Ă��܂��܂��c�c");
#else
                strcpy(quest_text[line++], "Losing to that monster from nightmares is going to hurt");
				strcpy(quest_text[line++], "your reputation as the ruler of dreams. Your liver is going to");
				strcpy(quest_text[line++], "get damaged from those drunken dreams at this rate...");
#endif

			}

		}
		break;
	case QUEST_HANGOKU1://v1.1.84 �A�����������T�@���E��
		if (pc == CLASS_CHEN || pc == CLASS_RAN)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "�w�������x�ƌĂ΂��i�v�ߐl�̑剅��̍s���͝��Ƃ��Ēm��Ȃ��B");
				strcpy(quest_text[line++], "����������ƍĂі������𑀂��đ��̒N���Ɏ��߂��@����f���Ă���̂����B");
				strcpy(quest_text[line++], "�_���W�����Ŗ�����������������L���Ă��܂����B");
				strcpy(quest_text[line++], "�������������������Ă�����哖���肾�B");
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
				strcpy(quest_text[line++], "�ʖڂŌ��X���������A�������������������ē|����̎���D�����B");
				strcpy(quest_text[line++], "�n�ꂩ��̎g���Ɉ����n���ĕԗ���󂯎�����B");
				strcpy(quest_text[line++], "����ŕЂ����Ƃ����̂����B");
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
				strcpy(quest_text[line++], "���Ȃ��̓_���W�����ł̑{������߂��B");
				strcpy(quest_text[line++], "��͂肠�����ςȑ剅�삪����ȂɊȒP�Ɍ�����͂��͂Ȃ��B");
#else
                strcpy(quest_text[line++], "You gave up on exploring the dungeon.");
				strcpy(quest_text[line++], "You knew you won't find such a cunning vengeful spirit");
				strcpy(quest_text[line++], "that easily.");
#endif

				//��������|���ăh���b�v�A�C�e���́�����������Ă���ꍇ�ǉ����b�Z�[�W
				if (a_info[ART_HANGOKU_SAKE].cur_num)
				{
#ifdef JP
					strcpy(quest_text[line++], "�@");
					strcpy(quest_text[line++], "����ɂ��Ă����������瓪���ɂ����������悤�ȋC������B");
					strcpy(quest_text[line++], "���̒��q�������̂��낤���H");
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
				strcpy(quest_text[line++], "���ɕ������q�K�������s���Ȓm�点���悱���Ă����B");
				strcpy(quest_text[line++], "�O�E���痈���Ǝv���鋥���ȗd�������ɐN�������炵���B");
				strcpy(quest_text[line++], "��Q�҂��o��O�ɑ��₩�Ɏn�����Ȃ��Ƃ����Ȃ��B");
				strcpy(quest_text[line++], "���̗��͉�X�̓꒣��Ȃ̂�����B");
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
				strcpy(quest_text[line++], "��Q���o��O�Ɍ��̘A����ގ����邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "�ڌ��҂��o�Ă��܂����悤�����A�q�K�������g���Č떂�����Ă������B");
#else
                strcpy(quest_text[line++], "You've managed to clean them up before something bad happened.");
				strcpy(quest_text[line++], "There were some eyewitnesses, but you send baby raccoons to");
				strcpy(quest_text[line++], "mislead and beguile them.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "��Ȃ����ƂɊO�G�ɒx�������Ă��܂����B");
				strcpy(quest_text[line++], "���܂��ܒʂ肪�������얲��������΂��Ă��ꂽ���A");
				strcpy(quest_text[line++], "�����͓����オ�肻���ɂȂ�...");
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
				strcpy(quest_text[line++], "�s��̐_�Ǝ��_�Ћ��ẪA�r���e�B�J�[�h������J�Â��ꂽ�B");
				strcpy(quest_text[line++], "�S���C���i�܂Ȃ����A�������_�Ђ̕��j�Ƃ��ĎQ�����Ȃ��킯�ɂ͂����Ȃ��B");
#else
                strcpy(quest_text[line++], "The Marketplace God and Moriya Shrine have cooperated to hold an");
				strcpy(quest_text[line++], "ability card exchange event. You're not that interested, but as the");
				strcpy(quest_text[line++], "wind celebrant of the shrine, you have to participate.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�疒�u�ǂ��ɂ��Q���҂̏W�܂肪������ˁB����Ȃɔ����̋�ԂȂ̂ɁB�v");
				strcpy(quest_text[line++], "�_�ގq�u�ӂ��ށA�ۑ����Ƃ��͔����Ƃ����ɂ͏����l�דI�������̂����B");
				strcpy(quest_text[line++], "�@���͊Ԍ���n���Z���^�[�̗n��G���A�ŊJ�Â���Ƃ����̂͂ǂ�������H�v");
				strcpy(quest_text[line++], "�疒�u�����I����ς肠�Ȃ��͎s��𕪂����Ă����ˁI�v�v");
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
				strcpy(quest_text[line++], "�s��̐_�Ǝ��_�Ћ��ẪA�r���e�B�J�[�h������J�Â��ꂽ�B");
				strcpy(quest_text[line++], "��͂�_�Ђ̍Ր_�Ƃ��Ď���������o���ׂ����낤�B");
#else
                strcpy(quest_text[line++], "The Marketplace God and Moriya Shrine have cooperated to hold an");
				strcpy(quest_text[line++], "ability card exchange event. As the festival god of the shrine,");
				strcpy(quest_text[line++], "you have to show up.");
#endif
			}
			else if (comp)
			{
#ifdef JP
				strcpy(quest_text[line++], "�Ñ�̍Ղ���v���o���Č��������A�Ȃ��Ȃ��y�����Â��ł������B");
				strcpy(quest_text[line++], "�������Ȃ������܂�M�������������͂��Ȃ��B");
#else
                strcpy(quest_text[line++], "That was a very enjoyable event; it reminded you of ancient");
				strcpy(quest_text[line++], "festivals and made your blood boil. For some reason, though, you");
				strcpy(quest_text[line++], "don't feel faith in you growing.");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "���Ȃ��͌������ɒH�蒅���Ȃ������B");
				strcpy(quest_text[line++], "����ł͎�Î҂Ƃ��ė��ꂪ�Ȃ��B�M���S�R�����Ȃ�����...");
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
				strcpy(quest_text[line++], "�얲�͎R���̃A�W�g��j�󂵂ċ����Ă������B");
				strcpy(quest_text[line++], "���X�̌����������ɂ���ȏ�A�R������������Ă����킯�ɂ������Ȃ��B");
				strcpy(quest_text[line++], "�x����\���o���炢�������ӂ��ꂽ���A");
				strcpy(quest_text[line++], "�ޏ������̖ڂ�^���������邱�Ƃ��ł��Ȃ�����...");
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
				strcpy(quest_text[line++], "�����ˁu���₠����������B�A�W�g��䖳���ɂ����Ƃ��낾�����B");
				strcpy(quest_text[line++], "���Ⴀ��̒ʉ݂��g���ĕ�V��n�����B");
				strcpy(quest_text[line++], "���ꂩ�������������ŗ��ނ�B");
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
				strcpy(quest_text[line++], "����̛ޏ����A");
				strcpy(quest_text[line++], "�u���̉R�f���ǂ���S�ł����Ȃ��ƋC���ς܂Ȃ���I�v��");
				strcpy(quest_text[line++], "�{�苶���ď�荞��ł����B");
				strcpy(quest_text[line++], "�����Ȃ񂾂�������Ȃ����A�܂��͌}�����Ȃ��Ƃ����Ȃ��I");
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
				strcpy(quest_text[line++], "�ޏ��͂��ꂱ�ꕶ��������Ȃ���A���Ă������B");
				strcpy(quest_text[line++], "�����炪�d����������āA�S�����f�Șb�ł���B");
#else
                strcpy(quest_text[line++], "The shrine maiden left, muttering some complaints.");
				strcpy(quest_text[line++], "She kept telling some nonsense like 'But you're youkai!'");
#endif
			}
			else if (fail)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ޏ��̓A�W�g�𔼉󂳂������ɂ悤�₭�A���Ă������B");
				strcpy(quest_text[line++], "��X����̉��������Ƃ����̂��H");
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
				strcpy(quest_text[line++], "�����ˁu�Ȃ񂾗d�����B���⍡�͗d���̎���؂肽���B");
				strcpy(quest_text[line++], "�@������݂�ȂŔ���얲�ƒe�������������񂾁I");
				strcpy(quest_text[line++], "�@�p���[�A�b�v�A�C�e���͎��������H�悵�˂����߁I�v");
#else
                strcpy(quest_text[line++], "Takane - 'Huh, a fairy? I'd like you to lend us a hand.");
				strcpy(quest_text[line++], "  We're going to play danmaku with Reimu Hakurei!");
				strcpy(quest_text[line++], "  Do you have power up items? Alright, go ahead!'");
#endif
			}
		}

		break;


	case QUEST_YAKUZA_1:

		//���̃N�G�X�g�͏������͂���₱�����̂Ŏ�̎��E�������E���s���ł܂�������
		if (accept)
		{
			//������߈ˎ�
			if (p_ptr->muta4 & (MUT4_GHOST_WOLF | MUT4_GHOST_EAGLE | MUT4_GHOST_OTTER | MUT4_GHOST_HANIWA))
			{
#ifdef JP
				strcpy(quest_text[line++], "�{���E�̓����삪�����̒r�n���ōR�����n�߂邻�����B");
				strcpy(quest_text[line++], "�������ǂ��������Ƃ��낤�H");
				strcpy(quest_text[line++], "�Ȃ������̐��͂ɏ��������Ȃ���΂����Ȃ��C�����Ďd�����Ȃ��I");
#else
                strcpy(quest_text[line++], "The animal ghosts from Animal Realm are going to start a war at Former");
                strcpy(quest_text[line++], "Lake of Blood Hell. But wait, what's going on? For some reason, you feel");
				strcpy(quest_text[line++], "strangely compelled to help their forces!");
#endif
			}
			//���S
			else if (pc == CLASS_SAKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ŋߌ������Ȃ��b�W�ޖ��̍s���Ɋւ�����񋟎҂����ꂽ�B");
				strcpy(quest_text[line++], "�ޖ��͋����̒r�n���Ɉ����U�����Ă������Ɨ͂�~���Ă���炵���B");
				strcpy(quest_text[line++], "���������R��|���ɍs�����B");
#else
                strcpy(quest_text[line++], "An informant has arrived, bringing news about Yuuma Toutetsu's location.");
				strcpy(quest_text[line++], "She's hiding at Former Lake of Blood Hell, building up her strength.");
				strcpy(quest_text[line++], "Time to kick her down.");
#endif
			}
			//����d
			else if (pc == CLASS_YACHIE)
			{
#ifdef JP
				strcpy(quest_text[line++], "�ŋߌ������Ȃ��b�W�ޖ��̍s���Ɋւ�����񋟎҂����ꂽ�B");
				strcpy(quest_text[line++], "�b�W�͋����̒r�n���Ɉ����U�����ĐΖ����Â��Ă���炵���B");
				strcpy(quest_text[line++], "�b�W�ɗ͂�~������͍̂D�܂����Ȃ�����������X����ɂ���ΗL���ɂȂ�B");
				strcpy(quest_text[line++], "���u�����͂Ȃ����낤�B");
				strcpy(quest_text[line++], "���������炭���̎҂͑��̑g�D�ɂ����l�ɏ���񋟂��Ă���B");
				strcpy(quest_text[line++], "�ň��l�b�̍R���ɂȂ邾�낤�B���O�ɏ��������ėՂ܂˂΂Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "�Ƃ��ɐΖ��͂悭�R����̂ŉΉ��΍􂪏d�v���B");
				strcpy(quest_text[line++], "�b���Ƃ��Čق������͂ȉΗ���A��čs�����B");
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
			//�ޖ�
			else if (pc == CLASS_YUMA)
			{
#ifdef JP
				strcpy(quest_text[line++], "�悤�₭�������������̒r�n���ɒ{���E�̘A��������������������Ă����B");
				strcpy(quest_text[line++], "���͂ȉΗ��������A�ꂽ���͂܂ł���悤���B");
				strcpy(quest_text[line++], "���������������݂��݂��Ζ��𖾂��n���̂͂��܂�ɂ��ɂ����B");
				strcpy(quest_text[line++], "�s�{�ӂ����h�q������˂΂Ȃ�Ȃ��B");
#else
				strcpy(quest_text[line++], "Just as the Lake of Blood Hell has calmed down, forces from Animal Realm");
				strcpy(quest_text[line++], "started interfering. They even have brought a powerful fire wyrm.");
				strcpy(quest_text[line++], "However, you're not going to just hand over your oil. You don't like it,");
				strcpy(quest_text[line++], "but you'll have to fight a defensive war.");
#endif
			}
			//����
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "�{���E�̓����삪�����̒r�n���ōR�����n�߂�Ƃ̏�񂪓͂����B");
				strcpy(quest_text[line++], "�����ɍs���ē������|�����Ȃ���΂Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "���G�̏��֕��c�̗͂�������x�������Ă�낤�B");
#else
				strcpy(quest_text[line++], "You've received information that animal ghosts from Animal Realm are");
				strcpy(quest_text[line++], "going to start a war at Former Lake of Blood Hell. You must quickly go");
				strcpy(quest_text[line++], "there and deal with the ghosts. You're going to show them the power of");
				strcpy(quest_text[line++], "your inexhaustible Haniwa Corps once again.");
#endif
			}
			//�ق���ʓ�����@�������͂ŕ���
			//v2.0.11 �V����3�l���������b�Z�[�W��\�����邽�ߕ����ς���
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
#ifdef JP
                strcpy(quest_text[line++], "���S�u���x�����Ƃ���ɗ����ȁI������R���ɍs�����I");
                strcpy(quest_text[line++], "�s����͋����̒r�n�����B�ޖ��̓z�������U�����ĉ������Ă���炵���I");
                strcpy(quest_text[line++], "�������������o���ڋ��҂��R�|���Ă��`�����X���I�݂�Ȏ��ɑ����I�v");
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
                strcpy(quest_text[line++], "����d�u�ً}�̔C����I");
                strcpy(quest_text[line++], "�����̒r�n���ŐΖ��Ƃ����L�p�Ȏ�������ʂɔ������ꂽ�B");
                strcpy(quest_text[line++], "���݂͍��~�����Ɏx�z����Ă��āA���u����킯�ɂ͂����Ȃ��B");
                strcpy(quest_text[line++], "���̒n��D�悷�ׂ��}���ŕ��͂��W�߂Ă��邯�ǁA");
                strcpy(quest_text[line++], "����g��쒷������X�Ɠ������Ƃ��l���Ă���݂����B");
                strcpy(quest_text[line++], "�����炭���R���ɂȂ邩�炠�Ȃ�������ɉ�����āB");
                strcpy(quest_text[line++], "���ƐΖ��͂悭�R���邩��Ζ��̏�ł͉��U���̈З͂�������B");
				strcpy(quest_text[line++], "���͂ȉΗ����ق��ĘA��čs�����犪�����܂�Ȃ��悤�ɗ�������ĂˁB�v");
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
                strcpy(quest_text[line++], "�n�ʂ��畷������ޖ��̐��u�������O�B�����ɒn���̋����̒r�n���܂ŗ����B");
                strcpy(quest_text[line++], "��X�̐Ζ���_���đ����̘͂A�����U�߂Ă����B");
                strcpy(quest_text[line++], "�ǂ��]�Ԃɂ��돭���͓G�ɏo���������Ȃ���Η����ɐH�����ނ��Ƃ��ł���B");
                strcpy(quest_text[line++], "���O��������ĂȂ�ׂ��G�̐������点�B");
                strcpy(quest_text[line++], "�Ƃ��ɓG�̉��R�̉Η������B�Ή��΍�����Ă����B");
                strcpy(quest_text[line++], "����ɂ��Ă��h�q��ȂǑS�����ɍ����B������������...�v");
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
                strcpy(quest_text[line++], "�{���E�̓����삪���̉��̋����̒r�n���ōR�����n�߂�悤���B");
                strcpy(quest_text[line++], "�����ɂ͊֌W�̂Ȃ��b�����A�ǂ����ɖ������ĉ��𔄂��Ă������B");
                strcpy(quest_text[line++], "���邢�͑S���|���Ă��܂��Ύ��ɒɉ����낤�B");
#else
                strcpy(quest_text[line++], "The animal ghosts from Animal Realm are going to start a war at Former");
                strcpy(quest_text[line++], "Lake of Blood Hell. It's none of your business, but you might join");
                strcpy(quest_text[line++], "someone to win their favor. Or you could just beat them all up.");
#endif
			}

			//�ق���ʏ���
			else if (pr == RACE_HANIWA)
			{
#ifdef JP
				strcpy(quest_text[line++], "���|�u������ǂ��������̒r�n���ōR�����n�߂�Ƃ̏�񂪓�������I");
				strcpy(quest_text[line++], "�l�ԗ���s����̂ɖO�����炸�{���E�̊O�ɂ܂Ő�Ђ��L���悤�Ƃ͋����������I");
				strcpy(quest_text[line++], "���B�������Ɍ������A�z����|�����܂��傤�I�v");
#else
				strcpy(quest_text[line++], "Mayumi - 'We've received information that animal ghosts are going to start");
				strcpy(quest_text[line++], "a war at Former Lake of Blood Hell! Oppressing human spirits was bad enough,");
				strcpy(quest_text[line++], "and now they're waging war outside their realm! We're heading there right");
				strcpy(quest_text[line++], "now to clean them up!'");
#endif
			}
			//����ȊO
			else
			{
#ifdef JP
				strcpy(quest_text[line++], "��u�Ȃ�œ�����Ƃ����Ƃ����ւ̌Q�ꂪ");
				strcpy(quest_text[line++], "���܂��Ȃ���ܔM�n�������낼��ʂ蔲���čs���́H");
				strcpy(quest_text[line++], "�����������Ƃ��̂ɔ�ꂽ��[�I�v");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�Ӂu�����̒r�n���Œ{���E�̘A�����R���������n�߂�݂������˂��B");
				strcpy(quest_text[line++], "�܂����������܂ŕ����Ă����Ȃ�B");
				strcpy(quest_text[line++], "�ł��r�Ɏ��M������Ȃ�ЂƖ\�ꂵ�ĉ������ɉ��𔄂�邩������Ȃ��ˁB");
				if (pc == CLASS_SATORI)
					strcpy(quest_text[line++], "���Ƃ�l�������ɍs���Ă݂܂����H�v");
				else if (pc == CLASS_KOISHI)
					strcpy(quest_text[line++], "...��H���܂����ɒN�������悤�ȁH�v");
				else if (pc == CLASS_ORIN)
					strcpy(quest_text[line++], "�����������傢�ƌ����ɍs�������ȁH�v");
				else if (pc == CLASS_UTSUHO)
					strcpy(quest_text[line++], "���Ȃ炿�傢�ƌ����ɍs���Ă݂邩���H�v");
				else if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "�����̂��Z������s���Ă݂邩���H�v");
				else
					strcpy(quest_text[line++], "�����̂��o������s���Ă݂邩���H�v");
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
		//������ �����������͂ŕ���
		else if (comp)
		{

			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA)
			{
				if (pc == CLASS_SAKI)
				{
#ifdef JP
					strcpy(quest_text[line++], "�{���E�̑����͂�S�ďR�U�炵�A���ɋC���������I");
					strcpy(quest_text[line++], "����ɏ��񋟎҂���ʔ����i�����コ�ꂽ�B");
#else
					strcpy(quest_text[line++], "It felt so good to kick around the other forces from Animal Realm!");
					strcpy(quest_text[line++], "You also have received an interesting item from your informant.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
				{
#ifdef JP
					strcpy(quest_text[line++], "���S�u�������I��͂�Ō�ɏ��͖̂\�͂��I");
					strcpy(quest_text[line++], "���O�������I�ȋ������������I�J���ɂ������낤�I�v");
#else
					strcpy(quest_text[line++], "Saki - 'Did you see it? Brute force triumphs in the end! You also were");
					strcpy(quest_text[line++], "impressingly strong! Let me reward you!'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "���S�u�������I��͂�Ō�ɏ��͖̂\�͂��I");
					strcpy(quest_text[line++], "���O�������I�ȋ������������I�J���ɂ������낤�I");
					strcpy(quest_text[line++], "�Ƃ���ř���g�ɓ���C�͂Ȃ����H�v");
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
					strcpy(quest_text[line++], "�l�b�̍R���𐧂��邱�Ƃɐ��������B");
					strcpy(quest_text[line++], "�n��̐_�B���F�X�ƌ������������Ă����̂ł��܂����v�������o���Ă�낤�B");
#else
					strcpy(quest_text[line++], "You've won the four-way conflict.");
					strcpy(quest_text[line++], "You also managed to gain some profits by negotiating with the gods on");
					strcpy(quest_text[line++], "surface.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
				{
#ifdef JP
					strcpy(quest_text[line++], "����d�u�f���炵���B�v���Ă������y���Ɏg����悤�ˁB");
					strcpy(quest_text[line++], "���Ƃ͎��ɔC���Ȃ����B���Ȃ��ɂ͏[���ȕ�V��^���܂��傤�B�v");
#else
					strcpy(quest_text[line++], "Yachie - 'That was splendid. You're far more useful than I thought. Leave");
					strcpy(quest_text[line++], "the rest to me. I'll reward you handsomely.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "����d�u�����̗͂̔��荞�ݕ����悭�S���Ă���悤�ł��ˁB");
					strcpy(quest_text[line++], "���Ȃ��Ƃ͂悢�֌W�ł��肽�����̂ł��B");
					strcpy(quest_text[line++], "����̕�V�Ƃ��Ă���������グ�܂��傤�B�v");
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
					strcpy(quest_text[line++], "���~�������̖{�C�̗͂̑O�ɑ����͓͂��������Ă������B");
					strcpy(quest_text[line++], "���ɗ����Ȃ������҂̐_���獡�X�x���i�̒񋟂̐\���o�����������A");
					strcpy(quest_text[line++], "�����������ۂݍ���ł�낤�B");
#else
					strcpy(quest_text[line++], "The other forces fled from the true power of Gouyoku Alliance.");
					strcpy(quest_text[line++], "That useless ally of a god offered you an item; you're going to");
					strcpy(quest_text[line++], "consume it someday.");
#endif
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
				{
#ifdef JP
					strcpy(quest_text[line++], "�ޖ��u�N�b�N�b�N...�V���ȋ�������Ȃ����B�C�ɓ��������B");
					strcpy(quest_text[line++], "�J���Ƃ��Ă��������Ă��B");
					strcpy(quest_text[line++], "�����H�����肾�������A���O�Ɏg�킹���ق������v�ɂȂ肻�����B�v");
#else
					strcpy(quest_text[line++], "Yuuma - 'Hehehe... You're so strong. I like you!");
					strcpy(quest_text[line++], "Take this as your reward.");
					strcpy(quest_text[line++], "I was planning on eating it, but it might be of more use to you.'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "�ޖ��u�͂āA�Ȃ����O����X�ɖ��������H�����ɎQ���������̂��H");
					strcpy(quest_text[line++], "�܂������A�������ނ��B���̂Ԃ��V�ɐF��t���Ă��B�v");
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
					strcpy(quest_text[line++], "���֕��c�̖����̗͂̑O�ɓ����삽���͎U��U��ɓ����Ă������B");
					strcpy(quest_text[line++], "���񋟎҂��狦�͂̎ӗ�Ƃ��Ē������i���͂���ꂽ�B");
#else
					strcpy(quest_text[line++], "The animal ghosts have scattered before the endless power of");
					strcpy(quest_text[line++], "Haniwa Corps. Your informant sent you an interesting item as a");
					strcpy(quest_text[line++], "reward for cooperation.");
#endif
				}
				else if (pr == RACE_HANIWA)
				{
#ifdef JP
					strcpy(quest_text[line++], "���|�u��X�̏�����I");
					strcpy(quest_text[line++], "�����ȓ����������M���ɂ͖J���̕i����������܂��I");
					strcpy(quest_text[line++], "����̕����Ɋ��҂��Ă����I�v");
#else
					strcpy(quest_text[line++], "Mayumi - 'Victory is ours!");
					strcpy(quest_text[line++], "You were outstanding! Here's your reward.");
					strcpy(quest_text[line++], "We're looking forward to working with you in the future!'");
#endif
				}
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "���|�u������ǂ��̑|���ɂ����͊��Ӓv���܂��I");
					strcpy(quest_text[line++], "�����ȓ����������M���ɂ͖J���̕i����������܂��I");
					strcpy(quest_text[line++], "����ƁA��X�̂悤�Ȋ����ȑ̂����]�݂Ȃ炢�ł������Ă��������ˁI�v");
#else
					strcpy(quest_text[line++], "Mayumi - 'Thank you for helping us clean up those animal ghosts!");
					strcpy(quest_text[line++], "You were outstanding! Here's your reward.");
					strcpy(quest_text[line++], "Also, if you want a perfect body like ours, just let us know!'");
#endif
					//TODO:�ݕP�̓X�ŏ��ւɎ푰�ύX�ł���悤�ɂȂ�Ƃ����̂͂ǂ����낤�H
				}
			}
			else //�S�Ń��[�g
			{
#ifdef JP
				strcpy(quest_text[line++], "�S�Ă̐��͂�P�Ƃşr�ł��Ė߂�ƁA�w���ɖ��Ȃނ��y�����������B");
				strcpy(quest_text[line++], "�U������Ă݂邪�N�����炸�A���������̊Ԃɂ������Ȕ����u����Ă����B");
				strcpy(quest_text[line++], "�ǂ���炠�Ȃ��̍s���͂ǂ����̒N������ɖ����������炵���B");
#else
				strcpy(quest_text[line++], "After defeating all of the forces by yourself and returning, you feel");
				strcpy(quest_text[line++], "someone standing behind you. As you turn around, there's nobody here,");
				strcpy(quest_text[line++], "but there's a small box lying on the ground. Looks like your actions");
				strcpy(quest_text[line++], "have made someone very happy.");
#endif
			}
		}

		//���s��
		else if (fail)
		{

			if (pc == CLASS_SAKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "�܂����G�ΐ��͂ɖ\�͂Ō�������Ă��܂����B");
				strcpy(quest_text[line++], "�����̎�O�C��ɐU�镑�����A�H�����A��ʂ�Ȃ�...");
#else
				strcpy(quest_text[line++], "Once again, your enemies were more violent than you.");
				strcpy(quest_text[line++], "You try to look strong for your subordinates, but you can't");
				strcpy(quest_text[line++], "get a meal down your throat...");
#endif
			}
			else if (pc == CLASS_YACHIE)
			{
#ifdef JP
				strcpy(quest_text[line++], "�R���ɔs��Ă��܂����B");
				strcpy(quest_text[line++], "�Ζ������𓾂������͂𑊎�ɂǂ������Ԃ��������ɂ�...");
#else
				strcpy(quest_text[line++], "You've lost the war.");
				strcpy(quest_text[line++], "Your head hurts thinking about how to deal with other forces, now that the");
				strcpy(quest_text[line++], "oil is in their hands...");
#endif
			}
			else if (pc == CLASS_YUMA)
			{
#ifdef JP
				strcpy(quest_text[line++], "�܊p�̐Ζ�������D���Ă��܂����B");
				strcpy(quest_text[line++], "���͑�l��������������݂̂��B");
				strcpy(quest_text[line++], "�����ꂱ������E�э���őS���܂ݐH�����Ă�낤�B");
#else
				strcpy(quest_text[line++], "Your oil resources were taken away from you.");
				strcpy(quest_text[line++], "You'll have to lay down for a while.");
				strcpy(quest_text[line++], "Some day, you're going to come out and consume them all.");
#endif
			}
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
#ifdef JP
				strcpy(quest_text[line++], "�g���B�̖{�C�̍R���͑z���ȏ�̌������������B");
				strcpy(quest_text[line++], "�s�o�ɂ��s���̗J���ڂɂ����Ă��܂����B");
#else
				strcpy(quest_text[line++], "The matriarchs were stronger in battle than you expected.");
				strcpy(quest_text[line++], "You were painfully defeated in conflict.");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
#ifdef JP
                strcpy(quest_text[line++], "���S�u�������A�Ȃ񂾂��̟T���������́I");
                strcpy(quest_text[line++], "�������Ȃ񂩖������I�ł����͐�΂ɏ�����ȁI�v");
#else
                strcpy(quest_text[line++], "Saki - 'Hey, what's up with that annoying oil?");
                strcpy(quest_text[line++], "I have no regrets! Next time, we'll win for sure!'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
			{
#ifdef JP
                strcpy(quest_text[line++], "����d�u�܂����̎�̑����͐퓬���I����Ă��炪�{�Ԃ��B");
                strcpy(quest_text[line++], "���Ƃ͎�������Ă����B���Ȃ��͌��̔C���ɖ߂�Ȃ����B�v");
#else
                strcpy(quest_text[line++], "Yachie - 'Well, the real conflict starts after a battle like this");
                strcpy(quest_text[line++], "is over. Leave it to me and resume your original mission.'");
#endif
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
			{
#ifdef JP
                strcpy(quest_text[line++], "�ޖ��u�܂������B���ꂪ�N�̂��̂ɂȂ낤������ɔE�э���ŐH���܂ł��B");
                strcpy(quest_text[line++], "�������Ȃ�����Ȃɑ����A���ɐΖ��̂��Ƃ��R�ꂽ�H");
                strcpy(quest_text[line++], "��͂肠�̌ӎU�L���z�̍��������H�v");
#else
                strcpy(quest_text[line++], "Yuuma - 'Oh well; I don't care who controls it, I'll sneak in and");
                strcpy(quest_text[line++], "devour it anyway. However, how did they find out about the oil so");
                strcpy(quest_text[line++], "quickly?'");
#endif
			}
			else if (pr == RACE_ANIMAL_GHOST)
			{
#ifdef JP
					strcpy(quest_text[line++], "���Ȃ��͒{���E�Z�l����������ȍR�����疽���炪�瓦���o�����B");
#else
					strcpy(quest_text[line++], "You ran for your life from the violent battle between the");
					strcpy(quest_text[line++], "residents of Animal Realm.");
#endif
			}
			else if (pr == RACE_HANIWA)
			{
#ifdef JP
				strcpy(quest_text[line++], "���|�u������ɂ���قǂ̂��Ƃ��ł���Ȃ��...");
				strcpy(quest_text[line++], "�@���̂悤�Ȍ�������I�v");
#else
				strcpy(quest_text[line++], "Mayumi - 'I never knew animal ghosts could be this powerful...");
				strcpy(quest_text[line++], "  It's like a dream come true!'");
#endif
			}
			else if (pc == CLASS_SATORI || pc == CLASS_KOISHI || pc == CLASS_ORIN || pc == CLASS_UTSUHO)
			{
#ifdef JP
                strcpy(quest_text[line++], "���Ȃ��͒{���E�Z�l����������ȍR�����疽���炪�瓦���o�����B");
#else
                strcpy(quest_text[line++], "You ran for your life from the violent battle between the");
                strcpy(quest_text[line++], "residents of Animal Realm.");
#endif
			}
			else if (player_looks_human_side())//���X���\�����l�Ԑ��͂��ۂ��O�����ǂ����ŕ���
			{
#ifdef JP
				if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "�Ӂu���₨�₨�Z������v�����H");
				else
					strcpy(quest_text[line++], "�Ӂu���₨�₨�o������v�����H");

				strcpy(quest_text[line++], "�������v����Ȃ��Ă����S���Ă�B");
				strcpy(quest_text[line++], "�������������Ǝ��̖̂ʓ|�����Ă����邩��ˁB�v");
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
					strcpy(quest_text[line++], "�Ӂu���₨�₨�Z������v�����H");
					strcpy(quest_text[line++], "�ł����Z����̎��͖̂ʔ����Ȃ��������ˁB");
				}
				else
				{
					strcpy(quest_text[line++], "�Ӂu���₨�₨�o������v�����H");
					strcpy(quest_text[line++], "�ł����o����̎��͖̂ʔ����Ȃ��������ˁB");
				}
				strcpy(quest_text[line++], "�܁A����ɐZ�������炳�����ƋA�����A�����B");
#else
				strcpy(quest_text[line++], "Rin - 'Hey, are you okay?");
                strcpy(quest_text[line++], "Hmm, your corpse wouldn't be interesting to me.");
				strcpy(quest_text[line++], "Just take a dip in the hot springs and go back.'");
#endif
			}

		}
		break;


	case QUEST_HANGOKU2://v1.1.98 �A�����������U
		if (accept)
		{
			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{
#ifdef JP
				strcpy(quest_text[line++], "�d����c�̉������삩���q���Ăق����Ɨ��܂ꂽ�B");
				strcpy(quest_text[line++], "�S�������Ȃ��Ƃ��B");
				strcpy(quest_text[line++], "���̉���͖ڂ̑O�ɂ���Ƃ����̂ɁB");
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
				strcpy(quest_text[line++], "���z�̔��Ƀ~�X�e���[�T�[�N�����o�������B");
				strcpy(quest_text[line++], "�������d�������̂̑��c���J�����Ƃ����B");
				strcpy(quest_text[line++], "�������~�X�e���[�T�[�N���̒��ɋ��͂ȉ��삪�������Ă���炵���B");
				strcpy(quest_text[line++], "�d���̖����̂��߂Ɏ������撣���Ēǂ�����Ȃ��ƁB");
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
				strcpy(quest_text[line++], "���z�̔��ŗd���ɐ��̂����j���Đ킢�ɂȂ����B");
				strcpy(quest_text[line++], "��������͂�ŋ߂̗d�����D�����Ă���B");
				strcpy(quest_text[line++], "�d���������Ȃ��Ȃ��S�䂩�����������Ă����̂Œ��Ղ��Ă������B");
#else
				strcpy(quest_text[line++], "The youkai in Garden of Sun have noticed your true");
				strcpy(quest_text[line++], "identity and attacked. However, it looks like youkai");
				strcpy(quest_text[line++], "right now aren't much to speak of.");
				strcpy(quest_text[line++], "The fairies had some interesting magic items in their");
				strcpy(quest_text[line++], "possession.");
#endif
			}
			//���܂�Ȃ��Ǝv�����A��̎��ɂ͜߈˂���Ă��������̌�ǂ����ŐV���̖�����ނȂǂ��Ĝ߈ˉ��������Ƃ�
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
#ifdef JP
				strcpy(quest_text[line++], "�C�����Ƒ����ɑ�d�����|�ꕚ���Ă���A");
				strcpy(quest_text[line++], "�ӂ�ɂ͋����������d�������������B");
				strcpy(quest_text[line++], "�����N�����Ă���̂��낤�H�ŋ߂̋L�����Ȃ��B");
				strcpy(quest_text[line++], "�ł�����͂���Ƃ��Ă������͖̂���Ă������B");
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
				strcpy(quest_text[line++], "��X�̏������I");
				strcpy(quest_text[line++], "���Ԃ�������̎^�ƂƂ��ɏܕi�𑗂�ꂽ�B");
				strcpy(quest_text[line++], "���̑O�̏o�X�̔���c��̂悤�ȋC�����邪�A");
				strcpy(quest_text[line++], "����Ȃ��Ƃ͂��̒B�����̑O�ł͍��ׂȖ�肾�B");
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

				//�N�G�X�g��̎����牅��ɜ߈˂���Ă���Ƃ�
				if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
				{
#ifdef JP
					strcpy(quest_text[line++], "���z�̔��ŗd���ɐ��̂����j���Đ킢�ɂȂ����B");
					strcpy(quest_text[line++], "�Ȃ��Ȃ��̋��G�œP�ނ�]�V�Ȃ����ꂽ�B");
					strcpy(quest_text[line++], "�ŋ߂̗d���������̂Ă����̂ł͂Ȃ��̂�������Ȃ��B");
#else
					strcpy(quest_text[line++], "The youkai in Garden of Sun have noticed your true");
                    strcpy(quest_text[line++], "identity and attacked, forcing you to retreat.");
                    strcpy(quest_text[line++], "Looks like youkai still haven't lost their touch.");
#endif
				}
				//����̓���s���ŋ������s�ɂȂ����炱���ɗ���͂�
				else
				{
#ifdef JP
					strcpy(quest_text[line++], "�u���̍���Ȃ��Ɏ��̑O�ɗ��Ƃ͑S�������т�ꂽ���̂��ȁB");
					strcpy(quest_text[line++], "���āA���̑̂��ǂ��g���Ă���悤���H�v");
#else
                    strcpy(quest_text[line++], "'You have totally underestimated me, going against me");
                    strcpy(quest_text[line++], "without a plan. Now, how shall I use this body?'");
#endif
				}

			}
			//�N�G�X�g��̎��ɉ���߈˂���Ă������r���ŐV���̖�ȂǂŜ߈ˉ��������Ƃ�
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
#ifdef JP
				strcpy(quest_text[line++], "�C�����Ƒ��z�̔��ő�d���ɒǂ������񂳂�Ă����B");
				strcpy(quest_text[line++], "�����N�����Ă���̂��낤�H�ŋ߂̋L�����Ȃ��B");
#else
				strcpy(quest_text[line++], "As you come to your senses, you are chased out from");
				strcpy(quest_text[line++], "Garden of Sun by a great youkai. You have no recollection");
				strcpy(quest_text[line++], "of what has happened.");
#endif
			}

		}

		break;

		//v2.0.13 ���N�U�푈2�N�G�X�g
	case QUEST_YAKUZA_2:
		if (pc == CLASS_ENOKO)
		{
			if (accept)
			{
#ifdef JP
				strcpy(quest_text[line++], "���S�u���悢��n�߂邼�B�����͂����ȁH");
				strcpy(quest_text[line++], "�܂��͂������̎�z������D���ȓz��I��Œ@���̂߂��ė����B");
				strcpy(quest_text[line++], "���O���n��i�o�̈�ԑ����B�C�������I�v");
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
				strcpy(quest_text[line++], "���S�u�悭������I�܂��̓��C�o�����ЂƂR���Ƃ����ȁB");
				strcpy(quest_text[line++], "���������ł����Ƌ����Ȃ��B�v");
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
				strcpy(quest_text[line++], "����d�u�@�͏n�����B���낻��n�߂悤�B");
				strcpy(quest_text[line++], "�܂��͓G���͂̋����Ƃ�ׂ��Ȃ��ƂˁB");
				strcpy(quest_text[line++], "�����̎�z������D���Ȃق���I�тȂ����B");
				strcpy(quest_text[line++], "�O���d�m�q�͖��@�̐X�Ő[���A");
				strcpy(quest_text[line++], "�V�ΐl�����͋��ܔM�n��������ɂ��Ă��邻����B�v");
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
				strcpy(quest_text[line++], "����d�u�܂��͈�����[�h�ˁB");
				strcpy(quest_text[line++], "���̕�V�Ő�͂𐮂��Ȃ����B�v");
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
				strcpy(quest_text[line++], "�ޖ��u����Ɨ������B�����������̑g�̒n�㐨�͂�ǂ��������B");
				strcpy(quest_text[line++], "���傤�Ǔz��݂͌��ɏ܋������������Ă邩�炤���Œ����Ă��܂����B");
				strcpy(quest_text[line++], "����g�̌����͖��@�̐X�ł悭��������B");
				strcpy(quest_text[line++], "�S���g�̉����͂��������Ŋ������Ă���悤���B");
				strcpy(quest_text[line++], "�V��̗��ɂ͐l�T�������ӂȒ���������炵��������𔃂��̂������ȁB�v");
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
				strcpy(quest_text[line++], "�u�X�b�L������������Ă���ȁB");
				strcpy(quest_text[line++], "��V���󂯎��̂�Y���Ȃ�B�v");
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

	/*:::�N�G�X�g���͂������������Ă���ꍇ�z��̎c���S�ċ󔒂ɂ���*/
	if(line)
	{
		while(line < 10) quest_text[line++][0] = '\0';

		return TRUE;
	}
	else return FALSE;
}

/*:::Mega Hack - �ꕔ���j�[�N�N���X�͓���̃N�G�X�g���ŏ�����󂯂��Ȃ��B*/
/*:::���́��Ŏ󂯂��Ȃ��N�G�X�g�̂Ƃ�TRUE��Ԃ��B*/
/*:::�����Ƃ��ẮA�Y���N�G�X�g���Q�[���X�^�[�g���ɑS�Ď��s�ςɂ��A�_���v��X�e�[�^�X�ɕ\�����Ȃ��B*/
/*:::������~�X�ɋC�������B���ꂾ�Ƃ�����TRUE�ɂȂ����N�G�X�g�����s�I���̏�Ԃŏ��������������B�܂�Ⴆ�΃t�����͖��l�N�G�����łȂ����ۉ��O�N�G���󂯂��Ȃ��B*/
/*:::�ǂ��������̂��B�Ƃ肠�������u�B*/
/*:::��txxxxxxx.txt�̏�������ɍ׍H���ė��������Ȃ����B*/
//�����̑��ɂ�castle_quest()�̂Ƃ���Ŏ󒍂̉\�s�\�����߂Ă��鏈��������B
//�\���΍��H�ߍ����ȂǁB
//������ł͎푰�ϗe�Ƃ�������󂯂��邪�󂯂��Ȃ��Ƃ��ɂ��̌�̃N�G�X�g(���͂Ȃ��͂�)���󂯂��Ȃ��B
//������ł͂��̌�̃N�G�X�g���󂯂��邪�A�푰�ϗe�������Ǝ󂯂ɗ��Ă����łɓ����I�Ɏ��s�ς݁B

bool check_ignoring_quest(int questnum)
{

	int pc = p_ptr->pclass;
	int pr;

	//v1.1.80 �L����������͍��̎푰�łȂ��L�������C�N���̎푰���Q�Ƃ���B
	//�푰�ϗe�����Ƃ������I���s�̂͂��̃N�G�X�g�����x��0���s�Ƃ��ă_���v�Ƃ��ɏo�Ă��ĕςȂ̂�
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

		///mod151001 ���ᏋC�N�G�X�g����
	case QUEST_KAPPA:
		if(pr == RACE_KAPPA) return TRUE;
		break;
		//�c�p�C�N�G�@���~���A
	case QUEST_TUPAI:
		if(pc == CLASS_REMY) return TRUE;
		break;

		//���������@�f�P ���g�@�i��
	case QUEST_OLD_WILLOW:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_EIRIN) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_KAGUYA) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;

	//��n�x�����}��@���ӁA��
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_ORIN) return TRUE;
		if(pc == CLASS_BYAKUREN) return TRUE;
		break;

	//���g�ւ̎h�q�@�d�� ���g
	case QUEST_KILL_MOKO:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_KEINE) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;

	//�P��ւ̎h�q�@���ǂ񂰁A�Ă�A�d���A�i�ԁA�Ȍ��o��
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

	case QUEST_MITHRIL: //�~�X�����T���@���A�f�P
		if(pc == CLASS_BYAKUREN) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;

		//���l���~�߂ā@�t����
	case QUEST_FLAN:
		if(pc == CLASS_FLAN) return TRUE;
		break;

		//���ۉ��O�@�p�`�����[
	case QUEST_KYOMARU:
		if(pc == CLASS_PATCHOULI) return TRUE;
		break;
		//��J�_�Ё@�_�ގq,�z�K�q,�f�P
	case QUEST_KANASUWA:
		if(pc == CLASS_SUWAKO) return TRUE;
		if(pc == CLASS_KANAKO) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;
		//������@�얲
	case QUEST_HANAMI:
		if(pc == CLASS_REIMU) return TRUE;
		break;
		//���q�ˎҁ@����
	case QUEST_WANTED_SEIJA:
		if(pc == CLASS_SEIJA) return TRUE;
		break;
	case QUEST_WANTED_SUMIREKO: //�悤�������z���� �얲��俎q ���Ɛ�����
		if(pc == CLASS_REIMU || pc == CLASS_SUMIREKO || pc == CLASS_RINGO || pc == CLASS_SEIRAN) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		if(pc == CLASS_TOYOHIME) return TRUE;
		if(pc == CLASS_YORIHIME) return TRUE;

		break;
		//�w�J�[�e�B�A�œ|�@�N���E���s�[�X�A�w�J�[�e�B�A�A����
	case QUEST_HECATIA:
		if(pc == CLASS_CLOWNPIECE) return TRUE;
		if (pc == CLASS_VFS_CLOWNPIECE) return TRUE;
		if(pc == CLASS_HECATIA) return TRUE;
		if(pc == CLASS_JUNKO) return TRUE;
		break;
		//�P��N�G�X�g 5�Ƃ��B��������I���
		///mod151219 �I���Ȃ�����
		///mod151229 ���Ȃ��I�v�V�����̂Ƃ��ɂ͏o�Ȃ�����
		//�P�鏜�O�ǉ�
	case QUEST_KAGUYA:
		if(pc == CLASS_KAGUYA) return TRUE;
		if(ironman_no_fixed_art) return TRUE;
		break;

	case QUEST_OKINA:
		//v1.1.32 �Ƃ肠�������ƃI�L�i�͕s���֌W���ۂ��̂Ńp�X
		if(pc == CLASS_YUKARI) return TRUE;
		if(pc == CLASS_RAN) return TRUE;
		//v1.1.41 ���Ɨ��T���p�X�@
		if (pc == CLASS_MAI) return TRUE;
		if (pc == CLASS_SATONO) return TRUE;
		//v1.1.57 �B���
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
		//v1.1.52 �������͜߈ˉ؂Łu�S���X�J�X�J�Ŗ������Ȃ��H�v�Ƃ������Ă��̂Ŗ����ɂ��Ă����镕�i�C�g���A�_�C�A���[�Ŗ��̐��E�̂��������o�Ă����̂ŗL���ɂ���B
		//�������v���C���ɃZ�[�u�f�[�^���A�b�v�f�[�g����ƃN�G�X�g���s�����ɂȂ��Ă���̂�rd_savefile_new_aux()�ɂ��׍H�����B
		//	if (pc == CLASS_KOISHI) return TRUE;
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) return TRUE;
		//v1.1.57 �B��ނ����̒��ɕ��ʂɏo�ė��Ă����ۂ��̂Ńp�X
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//���ۍ��Ĕ��߁@������͎󂯂��Ȃ�
	case QUEST_SHORELINE2:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;

		//v2.0.14 �Y��Ă��̂Œǉ�
		if (pc == CLASS_YUMA) return TRUE;
		if (pc == CLASS_ENOKO) return TRUE;
		if (pc == CLASS_BITEN) return TRUE;
		if (pc == CLASS_CHIYARI) return TRUE;

		break;

		//�j�ł̃N�G�X�g3�@������͎󂯂��Ȃ�
	case QUEST_DOOM3:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;
		//�ݕP���t���@���łɕ�V�A�C�e���������Ă���̂�
		if (pc == CLASS_KEIKI) return TRUE;

		break;

	case QUEST_MIYOI: //�h���[���C�[�^�[�̐��Ё@�����A���A�}�~�]�E�͎�̕s��

		if (pc == CLASS_MIYOI || pc == CLASS_SUIKA || pc == CLASS_MAMIZOU) return TRUE;
		//�N���s���Q�[���I�[�o�[�ɂȂ����Ƃ������ł��Ȃ��̂Œe���Ƃ�
		if (pc == CLASS_CLOWNPIECE) return TRUE;
		break;

	case QUEST_HANGOKU1: //v1.1.84 �A�����������T�@�������E���E�����o�Ȃ����[�h�̂Ƃ���̕s��
		if (pc == CLASS_MARISA || pc == CLASS_YUKARI) return TRUE;
		if (ironman_no_fixed_art) return TRUE;

		break;

	case QUEST_MORIYA_2:
		if (pc == CLASS_CHIMATA) return TRUE;//�疒�̓A�r���e�B�J�[�h����V�̃N�G�X�g���󂯂��Ȃ�
		break;

	case QUEST_REIMU_ATTACK:
		if (pc == CLASS_REIMU) return TRUE;

		break;

	case QUEST_YAKUZA_1:
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//v2.0.13 �܋���N�G�X�g�@�e���͑g���Ƃǂ��ɂ������ꂵ�Ȃ��l�͎�̕s��
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
/*:::���폠�̕����r�T�u���[�`���@���햼�A�U���񐔁A�������A�ʏ�U���͂�\��*/
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
sprintf(tmp_str, "�U����: %d", p_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", p_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
sprintf(tmp_str, "������:  0  50 100 150 200 (�G��AC)");
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
c_put_str(TERM_YELLOW, "�\�ȃ_���[�W:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	mindam = MAX((eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);
	maxdam = MAX((eff_ds * eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);

	/* Damage for one blow (if it hits) */
sprintf(tmp_str, _("�U�����ɂ� %d-%d", "1 blow: %d-%d"), mindam, maxdam);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
sprintf(tmp_str, _("�P�^�[���ɂ� %d-%d", "1 turn: %d-%d"), p_ptr->num_blow[0] * mindam, p_ptr->num_blow[0] * maxdam);
	put_str(tmp_str, row+7, col+1);
}


//���폠�ł̔�r�p object_is_melee_weapon����Őj������������
//v1.1.93 �S�ʏ������� TV_MAGICWEAPON��ǉ��Ŏw��ł���悤�ɂȂ���
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
/*:::���폠�ɂ�镐���r*/

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
		msg_print(_("���Ȃ��̐퓬�X�^�C���͕��폠�̗����𒴂��Ă���炵���B",
                    "Your fighting style is beyond the blacksmith's comprehension."));
		return 0;
	}

	/* Save the screen */
	screen_save();

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	/*:::�ꎞ�I�ɕ����ޔ�*/
	i_ptr = &inventory[INVEN_RARM];
	object_copy(&orig_weapon, i_ptr);

	/* Only compare melee weapons */
	item_tester_no_ryoute = TRUE;
	item_tester_hook = item_tester_hook_compare_weapon;

	/* Get the first weapon */
	q = _("���̕���́H", "What is your first weapon? ");
	s = _("��ׂ���̂�����܂���B", "You have nothing to compare.");

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
		//v1.1.93 melee_weapon��compare_weapon�ɏC��
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
		put_str(format("[ ��r�Ώ�: 's'�ŕύX ($%d) ]", cost), 1, (wid + mgn));
		put_str("(��ԍ����_���[�W���K�p����܂��B�����̔{�Ō��ʂ͑����Z����܂���B)", row + 4, 0);
		prt("���݂̋Z�ʂ��画�f����ƁA���Ȃ��̕���͈ȉ��̂悤�ȈЗ͂𔭊����܂�:", 0, 0);
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
				msg_print(_("����������܂���I", "You don't have enough money!"));
				msg_print(NULL);
				continue;
			}

			q = _("���̕���́H", "What is your second weapon? ");
			s = _("��ׂ���̂�����܂���B", "You have nothing to compare.");

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
			///mod140430 hook���N���A
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

/*:::���폠�́u�h��ɂ��Ē��ׂ�v�R�}���h*/
/*
 * Evaluate AC
 *
 * AC�����𗦁A�_���[�W���������v�Z���\������
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"�_���[�W�y�����Ƃ́A�G�̍U�����������������̃_���[�W��\n"
		"���p�[�Z���g�y�����邩�������܂��B\n"
		"�_���[�W�y���͒ʏ�̒��ڍU���ɑ΂��Ă̂݌��ʂ�����܂��B\n"
		"(��ނ��u�U������v�u����𕉂킹��v�u���ӂ���v�̕�)\n \n"
		"�G�̃��x���Ƃ́A���̓G���ʏ퉽�K�Ɍ���邩�������܂��B\n \n"
		"��𗦂͓G�̒��ڍU�������p�[�Z���g�̊m���Ŕ����邩�������A\n"
		"�G�̃��x���Ƃ��Ȃ���AC�ɂ���Č��肳��܂��B\n \n"
		"�_���[�W���Ғl�Ƃ́A�G�̂P�O�O�|�C���g�̒ʏ�U���ɑ΂��A\n"
		"��𗦂ƃ_���[�W�y�������l�������_���[�W�̊��Ғl�������܂��B\n";
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

	/* �_���[�W�y�������v�Z */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("���Ȃ��̌��݂�AC: %3d", iAC), row++, 0);
	put_str(format("�_���[�W�y����  : %3d%%", protection), row++, 0);
	row++;

	put_str("�G�̃��x��      :", row + 0, 0);
	put_str("���          :", row + 1, 0);
	put_str("�_���[�W���Ғl  :", row + 2, 0);
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
		int dodge;   /* ���(%) */
		int average; /* �_���[�W���Ғl */

		put_str(format("%3d", lvl), row + 0, col);

		/* ��𗦂��v�Z */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;
		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100�_�̍U���ɑ΂��Ẵ_���[�W���Ғl���v�Z */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("���݂̂��Ȃ��̑������炷��ƁA���Ȃ��̖h��͂�"
		   "���ꂭ�炢�ł�:", 0, 0);
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
///mod131224  ��ꂽ����
static bool item_tester_hook_broken_weapon(object_type *o_ptr)
{
 	if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_WEAPON_BROKEN_SWORD) return TRUE;
 	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_BROKEN_DAGGER) return TRUE;
 	if (o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_BROKEN_KATANA) return TRUE;
 	if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_BROKEN_STICK) return TRUE;


	return FALSE;
}

/*:::�܂ꂽ����C�� from_ptr����\�͂����to_ptr�փR�s�[*/
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

			//v1.1.73 �؂ꖡ��n�k�������p���̂ɕ���̎�ނ��l������悤�C��
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

///mod140909 ����C���f��
bool item_tester_hook_repair_material(object_type *o_ptr)
{

	//v1.1.64 EXTRA�����̗쒷���ł��g�����A���̂Ƃ��͕����I���ł��Ȃ�
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


/*:::����C���f�ޗp�e�[�u�� TV,SV,�f�ޗppower,���̃A�C�e���ɂȂ邽�߂̕K�vpower(-1�͍쐬�s��)*/
//�C�����ʌ��莞�͏����𖞂�����ԉ��̂��̂��I�肳���̂ŁA��tval�̕���͕K�vpower�l���Ⴂ���ɕ���ł��Ȃ��Ƃ����Ȃ�
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
	//�Őj�͑I��ΏۊO

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
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON,0,-1," "}, //sv�l�������_����1-18
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

	{0,0,0,0,_("�I�[�p�_�~�[", "dummy terminator")}
};





/*:::����ɃX���C�═��\�͂������_���ɕt�^����Bo_ptr�͋ߐڕ���ł��邱�ƁB*/
//������Ă悭�l������G�S����C�����Ă��X���C���p���[�A�b�v���Ȃ��B�܂��������B
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


/*:::����C�����[�`����������*/
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
	prt(_("����̏C���ɂ͍ޗ��Ƃ��čz�΂��ʂ̕�����g���B",
        "Weapon repair uses ore or another weapon as materal."), row, 2);

	/* Get an item */
	q = _("�ǂ̐܂ꂽ������C���������H", "Repair which broken weapon?");
	s = _("�C���̕K�v�ȕ��������Ă��Ȃ��悤���ȁB", "You don't have anything that needs repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	if(!(wield_check(item,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	/* �z�΂��ޗ��ɂ���Ƃ��ɂ͖��ʂƂ������Ȃ��̂ŃG�S�ȏ�łȂ��Ă��g����
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("����͒����Ă����傤���Ȃ����B", "It is worthless to repair."));
		return (0);
	}
	*/
	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format(_("�����Ă���͈̂�x�Ɉ�ɂ��Ă��炨���B", "One at a time, please."));
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format(_("�C�����镐��@�F %s", "Repairing weapon : %s"), basenm), row+3, 2);

	/* Get an item */
	q = _("�ǂ���C���̍ޗ��Ɏg���H", "Use what as repair material?");
	s = _("�C���̍ޗ��������Ă��Ȃ��悤���ȁB", "You don't have suitables materials for repairing.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_repair_material;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print(_("���̘b�𕷂��Ă��Ȃ������̂��H", "Are you listening to me?"));
		return (0);
	}
	if(!(wield_check(mater,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format(_("�ޗ��Ƃ��镐��F %s", "Used as material: %s"), basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + (object_value_real(o_ptr) + object_value_real(mo_ptr)) ;

#ifdef JP
	if (!get_check(format("��%d�\���󂯂�B�悢���H ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		msg_format(_("�����Ă��Ȃ��ł͂Ȃ����B�o�����ė����B",
                    "Come back when you have the money."));
		return (0);
	}


	use_weapon = object_is_melee_weapon(mo_ptr);
	old_weight = o_ptr->weight;

	//��������Tval����
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

		msg_print(_("���̕����ǂ�ȕ���ɂ��ė~�����̂��H", "What kind of weapon do you want?"));
		put_str(_("a) �݊킪����", "a) Blunt weapon"), 2 , 40);
		put_str(_("b) �_������", "b) Staff"), 3 , 40);
		put_str(_("c) ��������", "c) Axe"), 4 , 40);
		put_str(_("d) ��������", "d) Spear"), 5 , 40);
		put_str(_("e) �������킪����", "e) Polearm"), 6 , 40);

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
		msg_format(_("ERROR:����C����Tval(%d)�̏��������o�^",
                    "ERROR: Unkown tval (%d) in weapon repair logic"),o_ptr->tval);
		return (0);

	}
	if(!r_tv){ msg_format(_("ERROR:����C���ŏC����tval��������",
                            "ERROR: Undefined tval after weapon repair")); return (0);}


	//��������Sval����
	//���ꏈ���@�Z���C�����ɗ��̗؂��g���Ɨ��؂̒Z���ɂȂ�
	if(r_tv == TV_KNIFE && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		 r_sv = SV_WEAPON_DRAGON_DAGGER;
	}
	//���ꏈ���@���C�����Ƀq�q�C���J�l���g���ƃq�q�C���J�l�̌��ɂȂ�
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_SWORD && mo_ptr->sval == SV_WEAPON_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}

	//�ʏ폈���@sv_power��f�ނ��猈��A�K���ɑ��������A�l�ɉ�����Sval����
	else
	{
		sv_power = -1;
		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == mo_ptr->tval && repair_weapon_power_table[i].sval == mo_ptr->sval )
				sv_power = repair_weapon_power_table[i].type;
		if(sv_power == -1){ msg_format(_("ERROR:����C���ł��̏C���f�ނ�sv_power���o�^����Ă��Ȃ�",
                                        "ERROR: Unkown sv_power for weapon repair material")); return (0);}

		//�S���̃p���[�̓����_��
		if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_SCRAP_IRON) sv_power = randint1(18);
		//�A�[�e�B�t�@�N�g�͗ǂ����ɂȂ�₷��
		else if(object_is_artifact(mo_ptr)) sv_power += 3 + randint0(3);

		//+3:42% +6:18% +10:6%
		///mod160206 4��6�ɕύX +3:57% +6:33% +10:15%
		while(!one_in_(6)) sv_power++;

		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == r_tv && sv_power >= repair_weapon_power_table[i].num &&  repair_weapon_power_table[i].num >= 0)
				r_sv = repair_weapon_power_table[i].sval;

	}
	if(!r_sv){ msg_format(_("ERROR:����C���ŏC����sval��������",
                            "ERROR: Undefined sval after weapon repair")); return (0);}
	k_idx = lookup_kind(r_tv,r_sv);
	if(!k_idx){ msg_format(_("ERROR:����C�������ik_idx����������",
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
	///mod150403 MAX()�̒���randint1�����Ă��̂ŏC��
	if (k_ptr->pval)
	{
		int tester = randint1(k_ptr->pval);
		if(o_ptr->pval < tester) o_ptr->pval = tester;
	}
	if (have_flag(k_ptr->flags, TR_ACTIVATE)) o_ptr->xtra2 = k_ptr->act_idx;

	add_art_flags(o_ptr); //���Ɋm���Ő؂ꖡ�t�^����Ȃǂ̏���


	//�C���f�ނ�����̂Ƃ��A�f�ޕ���̔\�͂��ꕔ�����p��
	if(object_is_melee_weapon(mo_ptr))
	{
		/* Add one random ability from material weapon */
		give_one_ability_of_object(o_ptr, mo_ptr);

		/* Add to-dam, to-hit and to-ac from material weapon */
		o_ptr->to_d += MAX(0, (mo_ptr->to_d / 3));
		o_ptr->to_h += MAX(0, (mo_ptr->to_h / 3));
		o_ptr->to_a += MAX(0, (mo_ptr->to_a));
	}
	//�C���f�ނ�����ȊO�̃A�C�e���̂Ƃ��A�Ǝ�����
	//���ۉ��O�@�����_���\�͕t�^�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HEMATITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		one_ability(o_ptr);
	}
	//�}�O�l�^�C�g�@�����_�������A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MAGNETITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

	}
	//��f�z�΁@�ŎE�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ARSENIC)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_BRAND_POIS);
	}
	//����z�΁@�����_���X���C�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MERCURY)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		gain_random_slay_2(o_ptr);
	}
	//�~�X�����@�C���l�������A�d��30%�����A�����_���ϐ�
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MITHRIL)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		o_ptr->weight -= o_ptr->weight * 3 / 10;
		one_resistance(o_ptr);
	}
	//�A�_�}���^�C�g�@�_���[�W�啝�����A���������A�d��50%�����A�_�C�X�{�[�i�X
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ADAMANTITE)
	{
		o_ptr->to_d += 10;
		o_ptr->to_h -= 10;
		o_ptr->weight += o_ptr->weight / 2;
		dd_bonus += 1;
		ds_bonus += 1;
	}
	//���̗� ��ʑϐ�+�C���l������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		one_high_resistance(o_ptr);
	}
	//覓S�@�C���l�啝�����A�؂ꖡ�������͒n�k�A�����_�������ƃ����_���X���C
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
	//���̒܁@�C���l�啝�����A�_�C�X�u�[�X�g�A�����_���X���C�Apval�u�[�X�g
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONNAIL)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		gain_random_slay_2(o_ptr);
		dd_bonus++;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�q�q�C���J�l�@���ŋ�����ȏ�ɂȂ�@���łȂ��Ƃ��_�C�X�u�[�X�g�Ɛ؂ꖡ�@�C���l�啝�����@�����_���X���C2�A�����_���ϐ��A�����_�������@
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
	//�X�̗� ��C�U��+��C�ϐ�+���ϐ�+�C���l�������@�G�S�łȂ��ꍇ�����G�S�ɂȂ�
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
	//�����̂�����@�C���l�����A�l�ԃX���C
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_NIGHTMARE_FRAGMENT)
	{
		o_ptr->to_d += 5 + randint1(3);
		o_ptr->to_h += 5 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
		else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);

	}
	//�΍� �n���ϐ��A���܂ɋz����l�ԃX���C
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
	//�܂ꂽ�j�@�����A�����_���X���C2�A�C���l����
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
	{
		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_THROW);
		gain_random_slay_2(o_ptr);
		gain_random_slay_2(o_ptr);
	}
	//��]�̌����@�C���l�啝�����A�Ύ׃X���C�A�j���A����
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL)) add_flag(o_ptr->art_flags, TR_KILL_EVIL);
		else add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		add_flag(o_ptr->art_flags, TR_BLESSED);
		add_flag(o_ptr->art_flags, TR_LITE);

	}

	//�S�� �S�������_��
	//�~�X�e���E���@�S�������_��������A�d�ʓ�{
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
	//���̃C�����i�C�g�@�C���l�������A�_�C�X�u�[�X�g�Apval�u�[�X�g
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_ILMENITE)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		dd_bonus += 2;
		ds_bonus += 2;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�s�����̉H�� |��*��r�M+�� �}�H
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
	//�G�C�W���̐Ԑ� /X���� /�_ r���M pval�u�[�X�g
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
	//�ɜQ������ /�_ r�� �� pval�u�[�X�g
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_IZANAGIOBJECT)
	{

		o_ptr->to_d += 5+randint1(5);
		o_ptr->to_h += 5+randint1(5);
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		add_flag(o_ptr->art_flags, TR_SEE_INVIS);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(o_ptr->pval) o_ptr->pval++;
	}
	//���� �C���l�����A/�� ~�� �����_������ �_�C�X�u�[�X�g
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

	//��̌� /X�f�_ r�� ���@��x���� �_�C�X�u�[�X�g
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
	//���̕�� /�����d
	else if (mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_MOON_ORB)
	{

		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
		add_flag(o_ptr->art_flags, TR_BLESSED);
	}

	else { msg_format(_("ERROR:����C���ł��̑f�ނ̏�����������Ă��Ȃ�",
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

	//���_�C�X����̒ǉ��U����pval������
	//�b��t���ǉ��U���t�������Ƃ������ɓ���Ȃ��B���������悤�ɂ��Ƃ��ׂ����B
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
	//v1.1.31b �x�b�s�������̕����ɓ��Ă͂܂�悤�ɂ���
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
			msg_print(_("�܂������̓����Ăь���������悤�Ƃ͂ȁB", "Finally, the day I see this sword once again has came."));
		else
			msg_print(_("����͂��Ȃ�̋ƕ��������悤���B", "This weapon seems to be exceptionally well made."));
	}

	//�߂蕐��ɂ͓����K�����t��
	if (have_flag(o_ptr->art_flags, TR_BOOMERANG)) add_flag(o_ptr->art_flags, TR_THROW);

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("�悵�B�C�����ς񂾂��B", cost, basenm);
#else
	msg_format("Alright, repair complete.", basenm, cost);
#endif
	msg_print(NULL);
	//�b��A�C�e�������ɂ���
	//�b��ς݃A�C�e����t�r�_�g���̗d��(xtra3=SPECIAL_ESSENCE_OTHER)�̏ꍇxtra3���ێ������B
	if(!object_is_smith(o_ptr)) o_ptr->xtra3 = SPECIAL_ESSENCE_ONI;

	///mod150311 �C������G�b�Z���X�����o�O�ɑΉ�
	/*:::Hack - �b��t�ƕ���t�̂Ƃ��̂ݓ���t���OON ����C���ƃG�b�Z���X�t���𗼕��s�������ƁA�G�b�Z���X�����������Ƃ��ɂ�����C�����ꂽ�������킩��悤��*/
	if(p_ptr->pclass == CLASS_JEWELER || p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_MISUMARU) o_ptr->xtra1 = 1;

	/* Remove BROKEN flag */
	o_ptr->ident &= ~(IDENT_BROKEN);

	//v1.1.25 ���莞���x���ƃ^�[�����L�^
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;

	/* Add repaired flag */
	o_ptr->discount = 99;

	///mod141129 �d�ʍČv�Z
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
///sys ����C���@���΂炭�͖���
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
	prt(_("�C���ɂ͍ޗ��ƂȂ����1�̕��킪�K�v�ł��B", "Hand one material weapon to repair a broken weapon."), row, 2);
	prt(_("�ޗ��Ɏg�p��������͂Ȃ��Ȃ�܂��I", "The material weapon will disappear after repairing!!"), row+1, 2);

	/* Get an item */
	q = _("�ǂ̐܂ꂽ������C�����܂����H", "Repair which broken weapon? ");
	s = _("�C���ł���܂ꂽ���킪����܂���B", "You have no broken weapon to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("����͒����Ă����傤���Ȃ����B", "It is worthless to repair."));
		return (0);
	}

	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format(_("��x�ɕ������C�����邱�Ƃ͂ł��܂���I", "They are too many to repair at once!"));
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format(_("�C�����镐��@�F %s", "Repairing: %s"), basenm), row+3, 2);

	/* Get an item */
	q = _("�ޗ��ƂȂ镐��́H", "Which weapon for material? ");
	s = _("�ޗ��ƂȂ镐�킪����܂���B", "You have no material to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_melee_weapon;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print(_("�N���C���̒ق���Ȃ��I", "This is not a klein bottle!"));
		return (0);
	}

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format(_("�ޗ��Ƃ��镐��F %s", "Material : %s"), basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + object_value_real(o_ptr) * 2;

#ifdef JP
	if (!get_check(format("��%d������܂�����낵���ł����H ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		object_desc(basenm, o_ptr, OD_NAME_ONLY);
		msg_format(_("%s���C�����邾���̃S�[���h������܂���I",
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

		msg_print(_("����͂��Ȃ�̋ƕ��������悤���B", "This blade seems to be exceptionally."));
	}

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("��%d��%s�ɏC�����܂����B", cost, basenm);
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
/*:::�X�ɂ�镐��h��C���l�㏸(�|��܂�)
 *:::������ĂԑO��item_tester_hook��ݒ肵�Ă���
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
	prt(format("���݂̂��Ȃ��̋Z�ʂ��ƁA+%d �܂ŉ��ǂł��܂��B", maxenchant), 5, 0);
	prt(format(" ���ǂ̗����͈�ɂ���%d �ł��B", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�������ǂ��܂����H";
	s = "���ǂł�����̂�����܂���B";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif
	/*:::����h��̃C���x���g���ԍ��𓾂�Bitem_tester_hook�g�p*/
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* Check if the player has enough money */
	if (p_ptr->au < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%s�����ǂ��邾���̃S�[���h������܂���I", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	/*:::�����Ŏw�肵���񐔕��A�C�e���̏C���l�𑝉�
�@  /*:::�i�����o�O�������Bbreak�̂�����to_???��2�ȏ゠���Ă�1�����オ��Ȃ��Ǝv�����j*/
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
		msg_print("���ǂɎ��s�����B");
#else
		msg_print("The improvement failed.");
#endif

		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("��%d��%s�ɉ��ǂ��܂����B", cost * o_ptr->number, tmp_str);
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
/*:::�A�C�e���Ə[�U�����w��@���s���Ȃ��@*/
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
prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif


	/* Only accept legal items */
	//item_tester_hook = item_tester_hook_recharge;
	//v1.1.86 �A�r���e�B�J�[�h�[�U�p�̃��[�`����g�񂾂��A�[�U�L��ɂ���Ƌ�������̂ŋ֎~�ɂ���
	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e���ɖ��͂��[�U���܂���? ";
s = "���͂��[�U���ׂ��A�C�e�����Ȃ��B";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif
	/*:::�A�C�e�����w�肷��B ����̃A�C�e�����w��ł��邪����������Ȃ̂Ŏ��������@���������Ȃ��Ƃ܂������R�ł�����̂��H*/
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
	/*�Ӓ肳��ĂȂ����50$�ŊӒ�@�S�[�U�̂Ƃ��͊Ӓ藿�������Ă�*/
	if (!object_is_known(o_ptr))
	{
#ifdef JP
msg_format("�[�U����O�ɊӒ肳��Ă���K�v������܂��I");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((p_ptr->au >= 50) &&
#ifdef JP
get_check("��50�ŊӒ肵�܂����H "))
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
msg_format("%s �ł��B", tmp_str);
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


	/*:::�[�U���i�v�Z*/
	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		int lev = k_info[o_ptr->k_idx].level;
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			/*:::���b�h�̏[�U���i�v�Z�@���b�h��pval�͏[�U�^�[�����Atimeout�͎c��[�U�^�[�����Ǝv����*/
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("����͍ď[�U����K�v�͂���܂���B");
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
			//���ʂ���Ȏ��N����񂪃Q�[���̃o�[�W�����A�b�v�Ń`���[�W���Ԑݒ��h��ɘM������N���邩�������
			if (base_charge < 1)
			{
				msg_format(_("ERROR:���̃A�C�e���̃^�C���A�E�g�l�ݒ肪��������",
                            "ERROR: Weird timeout setting for this item"));
				o_ptr->timeout = 0;
				return;
			}

			price = (rank * rank * 500 * o_ptr->timeout) / base_charge;
		}
		else
		{
			price = 0;
			msg_format(_("����͍ď[�U����K�v�͂���܂���B",
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
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
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
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
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
msg_format("%s���ď[�U����ɂ́�%d �K�v�ł��I", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	/*:::���b�h�Ȃ�[�U���邩�ǂ����m�F*/
	if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
	{
		if (get_check(format(_("��%d �ōď[�U���܂����H", "Recharge for $%d?"), price)))
		{
			/* Recharge fully */
			/*:::���b�h�̏[�U�����@�^�C���A�E�g0��*/
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	/*:::��A�_�͉񐔂��m�F*/
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("��񕪁�%d �ŉ��񕪏[�U���܂����H",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(p_ptr->au / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		/*:::��̏[�U�����Ɓi��j�̖����O��*/
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, 0);
#ifdef JP
msg_format("%s����%d �ōď[�U���܂����B", tmp_str, price);
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
/*:::���ɂ��S�Ă̖�����[�U�@�C���x���g���S�Ă��ΏۂɂȂ�ȊO�͒P�̂̂Ƃ��Ɠ���*/
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
	prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* skip non magic device */
		//v1.1.86 �A�r���e�B�J�[�h�����ŏ��������@����Ŏ����������炱�̔��莮�͍����ĂȂ���������̕���ŕςȂ̂͒e�����̂Ŗ��Ȃ������炵��
		//if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		//��͂�A�r���e�B�J�[�h�̏[�U�͒��~
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

			//v1.1.86 �A�r���e�B�J�[�h�̏[�U...�������~����
			/*
		case TV_ABILITY_CARD:
			{

				int rank = ability_card_list[o_ptr->pval].rarity_rank;
				int base_charge = ability_card_list[o_ptr->pval].charge_turn;

				if (o_ptr->timeout > 0)
				{
					//���ʂ���Ȏ��N����񂪃Q�[���̃o�[�W�����A�b�v�Ŕ����^�A�C�e���������^�A�C�e���ɕς����炱���ɗ���
					if (base_charge < 1)
					{
						msg_format("ERROR:���̃A�C�e���̃^�C���A�E�g�l�ݒ肪��������");
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
		msg_print("�[�U����K�v�͂���܂���B");
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
		msg_format("���ׂẴA�C�e�����ď[�U����ɂ́�%d �K�v�ł��I", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("���ׂẴA�C�e���� ��%d �ōď[�U���܂����H",  total_cost))) return;
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
	msg_format("��%d �ōď[�U���܂����B", total_cost);
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

/*:::�X�ړ��@�h���Ɛ�p���@*/
///mod140403 �s���悤�܂��w��ł��Ȃ������̂ŏC���@���łɐ�p���@�̊X�ړ��ł͂ǂ̊X�ɂ��s����悤�ɂ����B
//v1.1.87 �_��_�o��������g���ꍇ�ǉ������𕥂��Ζ��o�^�̏ꏊ�ɍs���邱�Ƃɂ���
//is_magic:���@�̂Ƃ�TRUE,�����R�}���h�̂Ƃ�FALSE ���@�̂Ƃ��͍s�����ꏊ�̂���X�S�Ăɍs����
bool tele_town(bool is_magic)
{
	int i, x, y;
	int new_town_idx = 0;
	int num = 0;
	bool town_ok[TOWN_MAX];

	//v1.1.87 ���o�^�̊X�ɍs���Ƃ��̒ǉ��R�X�g 0�͕s��
	//�C���f�b�N�X��TOWN_***�Ɉ�v
	//v2.0.12 ���n���X���͖��@�̐X����s����悤�ɂȂ�����30000����10000�ɒl��������
	int town_cost[TOWN_MAX] = { 0,0,5000,10000,0,3000,0,0,1000,0,0,0,0,0,0 };

	for(i=0;i<TOWN_MAX;i++) town_ok[i] = FALSE;

	if (dun_level)
	{
#ifdef JP
		msg_print("���̖��@�͒n��ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if(p_ptr->word_recall)
	{
		msg_print(_("���͋A�ґ҂������B", "You are waiting to be recalled."));
		return FALSE;
	}

	if (p_ptr->inside_arena || p_ptr->inside_battle)
	{
#ifdef JP
		msg_print("���̖��@�͊O�ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	//�s����ꏊ�̔���ƊX�ꗗ�̕\��
	for (i=1;i<TOWN_MAX;i++)
	{
		char buf[80];
		///sys ���ړ��ňړ��ł��Ȃ��X�𔻒肵�Ă��镔��
		///mod140329 �n�[�h�R�[�f�B���O�ɂ����B����ɐ�p�̊X�ړ��ł͂ǂ̊X�ɂ��s����悤�ɁB
		//���̂���town_cost[]�Ƃ��Ɠ������ăt���O���܂Ƃ߂���
		//if ((i == NO_TOWN) || (i == SECRET_TOWN) || (i == p_ptr->town_num) || !(p_ptr->visit & (1L << (i-1)))) continue;
		if( (!is_magic && i != TOWN_HITOZATO && i != TOWN_TENGU && i != TOWN_CHITEI && i != TOWN_HAKUREI  && i != TOWN_KOURIN )
			&& !(p_ptr->pclass == CLASS_MARISA && i == TOWN_KIRISAME)
			&& !(CLASS_IS_PRISM_SISTERS && i == TOWN_UG)
			) continue;
		//�����̂���X�̓p�X
		if (i == p_ptr->town_num) continue;

		//v1.1.87 �s�������Ƃ̂Ȃ��ꏊ��e���B�������{�ݗ��p�̏ꍇ�ǉ������𕥂��΍s����ꏊ�̓��X�g�ɓ����
		if (!(p_ptr->visit & (1L << (i - 1))))
		{
			if (is_magic) continue;

			if (!town_cost[i]) continue;
			sprintf(buf, _("%c) %-20s (�ǉ�����$%d)", "%c) %-20s (Extra fee $%d)"), I2A(i - 1), town[i].name,town_cost[i]);
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
		msg_print("�܂��s����Ƃ��낪�Ȃ��B");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

	//�ړ��\�t���O�̗������X��I������
	while(1)
	{
		prt(_("�ǂ��ɍs���܂���:", "Where do you want to go:"), 0, 0);

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
			prt(_("���̊X�ɂ͍s�������Ƃ��Ȃ��B�ǉ������𕥂��ΘA��čs���Ă����炵���B",
                "You haven't visited this location yet, but you can be transported there for an extra fee."), 0, 0);
			inkey();
			if (get_check_strict(_("�x�����܂����H", "Pay?"), CHECK_DEFAULT_Y))
			{
				if (p_ptr->au < 300 + town_cost[new_town_idx])//�{���̗��p��$300���n�[�h�R�[�f�B���O
				{
					msg_print(_("����������Ȃ��B", "You don't have enough money."));
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


//v1.1.32 �����ɏ��R�}���h �l���̎��_��
bool riding_ropeway(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if(p_ptr->word_recall)
	{
		msg_print(_("���͋A�ґ҂������B", "You are waiting to be recalled."));
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if(p_ptr->town_num == TOWN_HITOZATO)
	{
		prt(_("�u���_�Ђ܂ł̗D��ȋ�̗��͔@���ł����H",
                "'How about an elegant trip through the sky to Moriya Shrine?") , 4, 0);
		prt(_("�@��q���t���Ĉ��S���S�I����͉����킸��$500�ł��I�v",
                "It's safe and secure! Only $500 for a round trip!'"), 5, 0);
		new_town = TOWN_MORIYA;
	}
	else
	{
		prt(_("�u�l���ɂ��A��ł����H",
                "'Do you want to return to Human Village?"), 4, 0);
		prt(_("�@�A�H�͖����ƂȂ��Ă���܂��B�v",
                "The return trip is free of charge.'"), 5, 0);
		new_town = TOWN_HITOZATO;
	}

	if (!get_check_strict(_("���܂����H", "Ride?"), CHECK_DEFAULT_Y))
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

	//�t���[�o�[���b�Z�[�W
	switch(randint1(7))
	{
	case 1:
		if(p_ptr->lev > 30)
		{
			prt(_("��q�̔��T�V�炪�A", "The white wolf tengu escorting you"), 10, 10);
			prt(_("�u���̐l��q�v��Ȃ���ˁH�v�ƌ��������ɂ��Ȃ������Ă���...",
                "looks at you as if saying 'Does this person even need an escort?'"), 11, 10);
			break;
		}
		//lev30�ȉ���2��
	case 2: case 3:
		prt(_("���Ȃ��͑��̊Ԃ̋�̗����y���񂾁B", "You enjoyed your air travel."), 10, 10);
		break;
	case 4:
		prt(_("�V�畗�������������N���������A", "A lot of tengu winds happened,"), 10, 10);
		prt(_("���Ȃ��̃S���h���ɓ͂��O�ɑj�܂�ĎU�����B", "but they were stopped before reaching the gondola."), 11, 10);
		break;
	default:
		prt(_("���Ȃ��̓S���h������]�ޔ������i�F���y���񂾁B",
            "You enjoyed the beautiful view from the gondola."), 10, 10);
		break;
	}


	prt(_("�����L�[�������Ă�������", "Press any key"), 0, 0);
	(void)inkey();
	prt("", 0, 0);


	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	teleport_town_mode = TELE_TOWN_MODE_ROPEWAY;
	screen_load();
	return TRUE;
}



//�n�����
//v1.1.91 ���@�̐X�̋��n���s�X
bool pass_through_chirei_koudou(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if (p_ptr->word_recall)
	{
		msg_print(_("���͋A�ґ҂������B", "You are waiting to be recalled."));
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if (p_ptr->town_num == TOWN_KIRISAME)
	{
		prt(_("�L�����A�����ւƑ����Ă���B", "There's a path leading deep into a cave."), 4, 0);
		prt(_("�@������ʂ�Βn��ɍs����͂���...", "  You should be able to reach underground if you go through here..."), 5, 0);
		new_town = TOWN_CHITEI;
	}
	else
	{
		prt(_("�n��̖��@�̐X�ւƒʂ��Ă��铴�A���B", "This cave connects to Forest of Magic on surface."), 4, 0);
		new_town = TOWN_KIRISAME;
	}

	if (!get_check_strict(_("���A�ɓ���܂����H", "Enter the cave? "), CHECK_DEFAULT_Y))
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

	//�t���[�o�[���b�Z�[�W
	switch (randint1(5))
	{
	case 1:
		if (p_ptr->pclass != CLASS_MARISA)
		{
			prt(_("������ו�������ē��S���̖��@�g���Ƃ�������..",
                "You meet a fleeing magician carrying a lot of baggage..."), 10, 10);
			break;
		}
		//go through
	case 2:
		if (p_ptr->pclass != CLASS_YAMAME)
		{
			prt(_("�y�w偂����Q�����Ă���...", "The tsuchigumo is sleeping..."), 10, 10);
			break;
		}
		//go through
	default:
		prt(_("���Ȃ��͔��Â����A�̒�������n�߂�...", "You start walking through the dark cave..."), 10, 10);
		break;
	}


	prt(_("�����L�[�������Ă�������", "Press any key"), 0, 0);
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
/*:::�����X�^�[��l */
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
	/*:::�ꕶ�����͂��󂯂�*/
#ifdef JP
if (!get_com("�����X�^�[�̕�������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^M���O):", &sym, FALSE))
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	/*:::����for����i���ő�ȍ~�܂ő�����Ƃ��ƂŃ}�b�`�Ȃ��Ƃ��Ēe�����B^�����͕ʁB*/
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
		/*:::����ȕ����w��̂Ƃ��̓t���O�𗧂Ă�*/
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "�S�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�E�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�O�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	/*:::���O����*/
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("���O(�p��̏ꍇ�������ŉ�)",temp, 70))
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
		sprintf(buf, "���O:%s�Ƀ}�b�`",temp);
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
sprintf(buf, "%c - %s", sym, "�����ȕ���");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	/*:::*who�̗̈�������X�^�[�̐��ɂ��킹�Ċm�ہA���������Ă���炵���H*/
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	/*:::r_info[]����w�肵���t���O�������ɍ��������X�^�[��I�肵�Awho[]�Ƀ����X�^�[�C���f�b�N�X�����X�g�A�b�v���Ă���*/
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];


		///mod140712 �σp�����[�^�����X�^�[�̓����X�^�[��l����Q�Ƃł��Ȃ��悤�ɂ���
		if(r_ptr->flags7 & RF7_VARIABLE) continue;
		//�e�X�g�d���Ɠ���t�B�[���h�͏o���Ȃ�
		if(i == MON_TEST || i == MON_EXTRA_FIELD) continue;

		/* Empty monster */
		if (!r_ptr->name) continue;

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* ���O���� */
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
	/*:::�����X�^�[�����x�����Ƀ\�[�g�H����if�ɂ͕K������Ǝv�������̈Ӗ�������̂�*/
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
	/*:::�����X�^�[����\�����A-�ƃX�y�[�X�Ő؂�ւ��Ar���������炻�̃����X�^�[�̏���S�ĕ\�����L������B*/
	while (notpicked)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
Term_addstr(-1, TERM_WHITE, " ['r'�v���o, ' '�ő��s, ESC]");
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
				/*:::�w�肵�������X�^�[�̑S�Ă̏����L������*/
				lore_do_probe(r_idx);

				/* Save this monster ID */
				/*:::�v���o�E�B���h�E�ɕ\��*/
				monster_race_track(r_idx);

				/* Hack -- Handle stuff */
				handle_stuff();

				/* know every thing mode */
				/*:::���C���E�B���h�E�㕔�Ƀ����X�^�[�̎v���o��\������*/
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

///mod140112 �\�������Ȃ����̂ŕ����s�\���̂��߂ɍ���Ă݂��B����ݔ��̐����\���ɂ��g���\��B
/*:::rumors_new.txt�����s�ǂݍ����\�ŉ��s�������s�ɕ����ĕ\������B*/
void display_rumor_new(int town_num)
{
	cptr rumor_ptr;
	char buf[1024];
	//char buf2[1024];
	bool err;
	int i;

	//v1.1.92 ������ς���ex_rumor_new()�Ő��l�ݒ肷�邱�Ƃɂ���
		//bool extra_grassroot_rumor = FALSE;
		//v1.1.26 EXTRA�p�\�̔����character_icky��ǉ��B�ʓe�̃��C�V�����ł��\�𕷂���̖Y��Ă�
		//v1.1.27 icky>1�ɂ�����S�����ʓe�p�\�ɂȂ����̂ŏC���B
		//if(EXTRA_MODE && character_icky)
		//	extra_grassroot_rumor = TRUE;

	//v1.1.18 EXTRA�p�\
	if(town_num == 23 && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print(_("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B",
                "The Grassroots Network youkai have left."));
		return;
	}

	//LUNATIC�ł͕ςȉ\�ɂȂ�
	if (difficulty == DIFFICULTY_LUNATIC && (town_num < 7)) town_num = 99;

	screen_save();

	//v1.1.18 EXTRA�\�ǉ�
	//v1.1.92 ������ς���ex_rumor_new()�Ő��l�ݒ肷�邱�Ƃɂ���
	//else if(extra_grassroot_rumor) town_num = 23;


	err = get_rnd_line(_("rumors_new.txt", "rumors_new_e.txt"), town_num, buf);
	if (err) strcpy(buf, _("ERROR:���̔ԍ��̉\�͒�`����Ă��Ȃ��B",
                            "ERROR: Undefined rumor number."));

	//roff_to_buf(buf, 74, buf2, sizeof(buf2));
	//rumor_ptr = buf2;
	rumor_ptr = buf;

	/*:::Mega Hack ����x�����Ă݂�������:-) */
	/*:::���̕��͂�'\'�������Ă��'\0'�ɕϊ����A�s����\0\0�ɂ���B*/
	/*:::�蓮���s��roff_to_buf()�݂����Ȃ���B*/
	for(i=0;i < sizeof(buf)-2;i++)
	{
		if(buf[i] == '\0')
			break;
		/*:::�ꕔ���������s��������Ă��܂��̂�}�~�B����Ő������̂��ǂ����܂��������M���Ȃ��B*/
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
	/*:::15�s�܂ŕ\������*/
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

	prt(_("�����L�[�������Ă�������", "Press any key"), 0, 0);
	(void)inkey();
	prt("", 0, 0);

	screen_load();

}

/*:::�P��Ƙb������@������A�T�[�y���g�N�G��̐�p*/
static void kaguya_quest(void)
{
		int tmp,i;
		quest_type      *q_ptr;

		//v2.0.19 ��ՓxNORMAL�Ŏ��œ|��ɗ̈�ɍs���Ă��܂��̂Ńt���O������������
		bool flag_enter_serpen = FALSE;

		if (quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED)
			flag_enter_serpen = TRUE;

		q_ptr = &quest[QUEST_SERPENT];

		if(q_ptr->status > QUEST_STATUS_UNTAKEN) return;

		if(!flag_enter_serpen)
		{
			if(p_ptr->pclass == CLASS_EIRIN)
				msg_format(_("�P��u�y���݂ˉi�ԁI���������̂���R����ꂻ������B�v",
                            "Kaguya - 'It's fun, isn't it, Eirin? We can find so many rare items.'"));
			else if(p_ptr->pclass == CLASS_KAGUYA)
				msg_format(_("�~�͂͂܂��������Ă��Ȃ��B",
                            "Bonsai hasn't bear fruit yet. "));
			else
				msg_format(_("�P��u���l�����Ƃ����Ă���́B�����������ƂŖK�˂Ă��āB�v",
                            "Kaguya - 'I'm thinking now. Come visit us later.'"));
			return;
		}

		clear_bldg(4, 18);
		//�i�ԂƋP��̂Ƃ��Z���t��ς���
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
		//v1.1.18 ���ϐ��͂̓Z���t�ύX
		else if(p_ptr->pclass == CLASS_CLOWNPIECE || p_ptr->pclass == CLASS_VFS_CLOWNPIECE || p_ptr->pclass == CLASS_JUNKO || p_ptr->pclass == CLASS_HECATIA)
			display_rumor_new(94);
		//v1.1.13 �푰�����̖��̂Ƃ��䎌�ύX
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



/*:::�e�T�Ƃ̉�b�B@�����̍��l�b�g���[�N�ɉ����ł��邩�ǂ��������肷��*/
static void talk_kagerou()
{
	bool flag_ok;
	int count;

	/*
	if(p_ptr->pclass == CLASS_BANKI)
	{
		msg_print("���Ȃ��͎����̓��̕��g���o���Ęb�������Ă݂��B�������ڐV���������͕Ԃ��Ă��Ȃ������B");
		return;
	}
	*/
	if(EXTRA_MODE && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print(_("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B",
                "The Grassroots Network youkai have left."));
		return;
	}

	/*:::�����ς݁@��x����������Q�����i�������Ă����������ςȂ��ɂ��Ă���*/
	if(p_ptr->grassroots)
	{
		if(p_ptr->pclass == CLASS_KAGEROU)
			msg_print(_("�킩�����P�Ɛ��Ԙb�������B", "You make small talk with Wakasagihime."));
		else if(EXTRA_MODE)
			msg_print(_("�e�T�u�݂�ȂŊ撣���ĉ���ڎw���܂��傤�B������ł��Ȃ�ׂ��ǂ����o����B�v",
                        "Kagerou - 'Let's do our best and aim for the bottom. I'll give some good items in exchange."));
		else
			msg_print(_("���Ȃ��Ɖe�T�͐_�ЂɎ����킵���Ȃ����Ԙb�������B", "You make small talk unsuitable for the shrine with Kagerou."));
		return;
	}

	/*:::�����\����*/

	//���̍��Ǝ��̒ǉ�����
	switch(p_ptr->prace)
	{
	/*:::�l�Ԃ͉����s�@�������ψق�10�ȏ゠���OK���ꂽ��*/
	//v1.1.85 �����������ύX�B�ψق̐��łȂ��Ǔے��Ǝ���������g�����Ƃɂ���
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

		//v1.1.70 ������́u�������v�̂ݎQ���\
	case RACE_ANIMAL_GHOST:
		if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_NONE)
			flag_ok = TRUE;
		else
			flag_ok = FALSE;
		break;

	/*:::�l�O�͊�{������*/
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

	//�l�ԑ��͊�{�����s�����A�ϐg�Ƃ�����������ł���悤�ɂ���B
	case RACE_HUMAN:
	case RACE_KOBITO:
	case RACE_GREATER_LILLIPUT:
	case RACE_JK:
	case RACE_DEMIGOD:
	case RACE_HALF_YOUKAI:
	case RACE_HALF_GHOST:
	case RACE_ZASHIKIWARASHI://���~��炵�͗d�����͂���̃X�p�C������ʗd������͐l�ԑ��ƔF������Ă���͂��H
	case RACE_MAGIC_JIZO:
		if (!player_looks_human_side()) flag_ok = TRUE;
		else flag_ok = FALSE;

	break;

	//�ق��A����ȑ��݂Ƃ��Ė����m��Ă�푰�╁�ʗd�����ɂȂ肻���ɂȂ��푰��ʏ���ɉ��炩�̐��͂ɏ������Ă�푰�͉����s�B
	//TODO:���i�u�͂��ꕨ�v��ǉ����A���̂ւ�̐�������������Ă�������������Ȃ�
	default:
		flag_ok = FALSE;
		break;

	}
	//���j�[�N�N���X�͈ꕔ�����������s��
	if(!(cp_ptr->flag_join_grassroots)) flag_ok = FALSE;

	if(flag_ok)
	{
		msg_print(_("���Ȃ��ɂ͑��̍��d���l�b�g���[�N�ւ̎Q�����i������炵���B",
                    "You can enter the Grassroots Youkai Network."));
		if (get_check_strict(_("�Q�����܂����H", "Become a member?"), CHECK_DEFAULT_Y))
		{
			if(EXTRA_MODE)
			{
				int ex_bldg_num = f_info[cave[py][px].feat].subtype;
				msg_print(_("�e�T�u�݂�ȐV�������Ԃ�I���ꂩ�����낵���ˁB�v",
                            "Kagerou - 'Let's welcome our new companion, everyone!"));

				//-Hack- �����ɉ\�R�}���h��ǉ����čĕ`�悷��
				sprintf(building[ex_bldg_num].act_names[6], _("�\�𕷂�", "Listen to rumors"));
				building[ex_bldg_num].letters[6] = 'r';
				building[ex_bldg_num].actions[6] = BACT_RUMORS_NEW;

				show_building(&building[ex_bldg_num]);
			}
			else
			{
				msg_print(_("�e�T�u�F�ɂ͘b��ʂ��Ă�����B�܂����傭���傭����o���ĂˁB�v",
                            "Kagerou - 'I'll inform everyone. Drop by here often.'"));
			}
			p_ptr->grassroots = 1;
			//�얲�̐肢���Z�b�g
			p_ptr->today_mon = 0;
		}
	}
	else
	{
		if(EXTRA_MODE)
			msg_print(_("�e�T�u�v��Ȃ��A�C�e������������������Ă����Ȃ��H�v",
                        "Kagerou - 'Would you like to trade items you don't need?"));
		else if(p_ptr->pclass == CLASS_EIKI)
			msg_print(_("���Ȃ��͉e�T�ɐ����������B", "You give a lecture to Kagerou."));
		else
			msg_print(_("���Ȃ��Ɖe�T�͂ǂ�����X�������Ԙb�������B", "You make some small talk with Kagerou."));
	}
	return;
}


/*:::��ǐ_�l�̐_���^�C�v���m�F����*/
static void reimu_deity_check()
{
#ifdef JP
	cptr comment_cosmos[7] = {"���ׂ̋ɒv","����","���e","����","����","�Ύ��I","�����̌���"};
	cptr comment_warlike[7] = {"�����̎���","�F��","����","����","���\","����","�j��̉��g"};
	cptr comment_noble[7] = {"���|�̌���","����","����","����","�D�]","�l�C��","��]�̏ے�"};
#else
    cptr comment_cosmos[7] = {"Extremely chaotic","Self-serving","Tolerant","Ordinary","Rigid","Devout","Avatar of Law"};
	cptr comment_warlike[7] = {"Generous","Fraternal","Kind","Ordinary","Rough","Violent","Avatar of Destruction"};
	cptr comment_noble[7] = {"Fear Itself","Troublesome","Nameless","Ordinary","Well-liked","Popular","Symbol of Hope"};
#endif

	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_print(_("ERROR:��ǐ_�l�ȊO��deity_check()���Ă΂ꂽ",
                    "ERROR: deity_check() called for someone not a stray god"));
		return;
	}
	msg_print(_("�얲�u�����ƁA�M���̕]���͂ˁE�E�v",
                "Reimu - 'Hmm, your reputation is...'"));

	c_put_str(TERM_WHITE,format(_("�����x�F%s", "Alignment: %s"),comment_cosmos[p_ptr->race_multipur_val[DBIAS_COSMOS] / 37]),8 , 6);
	c_put_str(TERM_WHITE,format(_("�D��x�F%s", "Love of combat: %s"),comment_warlike[p_ptr->race_multipur_val[DBIAS_WARLIKE] / 37]),9 , 6);
	c_put_str(TERM_WHITE,format(_("�l�C�x�F%s", "Popularity: %s"),comment_noble[p_ptr->race_multipur_val[DBIAS_REPUTATION] / 37]),10 , 6);

	return;
}


/*:::�얲�̐肢�@���̍��l�b�g���[�N������͗��p�s��*/
/*�g�p�ϐ��F
 *p_ptr->today_mon �肢�̌��ʂ̔ԍ����i�[�@����0:00��0�Ƀ��Z�b�g
 *p_ptr->barter_value �^�[�Q�b�g�_���W�����A�����X�^�[�A�K���Ȃǂ��i�[�@����͈�x�������ꂽ��0�ɂȂ�
 */
static bool reimu_fortune_teller()
{
	/*:::���̍��l�b�g���[�N������͗��p�s��*/
	if(p_ptr->grassroots)
	{
		msg_print(_("�얲�͐���Ă���Ȃ������B�C�����Ȃ��炵���B",
                    "Reimu won't tell you your fortune. Looks like she doesn't feel like doing it."));
		return FALSE;
	}
	/*:::�����x�̂�*/
	if(p_ptr->today_mon)
	{
		msg_print(_("�얲�u���������������Ȃ��B�����͂����X�d������B�v",
                    "Reimu - 'I've already told you your fortune. My services are done for today.'"));
		return FALSE;
	}
	msg_print(_("�얲�̓P�[�v�Ɛ���������������o���Ă����B",
                "Reimu takes out a cape and a crystal ball."));

	if(one_in_(25)) c_put_str(TERM_WHITE,_("�ʓ|�L����ˁE�E", "So troublesome..."),8 , 6);
	else if(one_in_(4)) c_put_str(TERM_WHITE,_("�ȂɂȂɁE�E", "What is it..."),8 , 6);
	else if(one_in_(3)) c_put_str(TERM_WHITE,_("�ق��ق��E�E", "Let's see..."),8 , 6);
	else if(one_in_(2)) c_put_str(TERM_WHITE,_("���炠��E�E", "Oh, my..."),8 , 6);
	else				c_put_str(TERM_WHITE,_("�ӂނӂށE�E", "Hmmm..."),8 , 6);

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
		//����_���W�����̓���K�ɂȂɂ��ǂ�������
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

					//v1.1.55 �^�[�Q�b�g���S���ȊO�̂Ƃ��A1/2�̊m���Ńt���A�����𖳎�����悤�ɂ���

				}while((target_dungeon == DUNGEON_ANGBAND || one_in_(2)) && (target_level > lev_check * 12 / 10 || target_level < lev_check * 4 / 5));
				if(!fail)
				{
					p_ptr->barter_value = target_dungeon * 1000 + target_level;
#ifdef JP
					c_put_str(TERM_WHITE,format("%s��%d�K�ɂ��󂠂�A�Əo����I",(d_name + d_info[target_dungeon].name),target_level),9 , 6);
#else
					c_put_str(TERM_WHITE,format("You'll find treasure on dungeon level %d of %s!", target_level, (d_name + d_info[target_dungeon].name)),9 , 6);
#endif
				}
			}
			break;
		//���̓��̂����ɓ��胆�j�[�N��|���ƍ����i���h���b�v
		//m_idx��p_ptr->barter_value�֊i�[
		//���̃��x���`���x��*2�̖��œ|�œ���t���O�̂Ȃ����j�[�N�����X�^�[
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
					if (r_ptr->rarity > 100) continue; //v2.0.11�ǉ� ���ꃂ���X�^�[��o��֎~�����X�^�[
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
					c_put_str(TERM_WHITE,format(_("%s��|���ƍK�����K���A�Əo����I",
                                                "Defeating %s will bring you good fortune!"),m_name),9 , 6);
				}
			}
			break;
		//�����I�[����肿����Ɨǂ��A�C�e�������A���A�C�e���̋��z��������
		case FORTUNETELLER_GOODLUCK:
			c_put_str(TERM_WHITE,_("�����̋M���̉^�C�͐�D����I", "Today is your lucky day!"),9 , 6);
			break;
		//�����I�[���Ɠ����̃A�C�e�������A�G�������x�����X�u�[�X�g�AHORDE�������ܔ{
		case FORTUNETELLER_BADLUCK:
			c_put_str(TERM_WHITE,_("�E�E�����͉Ƃő�l�������Ă������ق������������ˁB",
                                "...You might want to stay at home for today."),9 , 6);
			break;
		default:
			msg_print(_("ERROR:����`�̐肢����", "ERROR: Undefined fortune telling result"));
			return FALSE;
		}
		if(!fail) break;
	}
	return TRUE;
}



//v1.1.86
//�����R�}���h:�A�r���e�B�J�[�h�̌�����
//������grassroots_trading_cards()�Ɠ�������H
void trading_ability_cards(void)
{

	int ref_pval, ref_num, ref_cost, ref_totalcost; //�v���C���[���n�����J�[�h�̏��
	int cs;
	char c;
	cptr q, s;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
	cptr msg_reject; //�񎦂��ꂽ�J�[�h�ɍ����J�[�h��������Ȃ������Ƃ��̃��b�Z�[�W
	cptr msg_finish; //�����I�������b�Z�[�W
	char o_name[MAX_NLEN];
	int item;

	int i, j, k;

	int trade_chance, trade_num; //�I�肳�������J�[�h��␔,���ۂɑI�肳�ꂽ�J�[�h��
	object_type barter_list[10];

	//v2.0.7 �疒�v���C���ɂ̓J�[�h�����N�����\������
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		chimata_comment_card_rank();
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print(_("���͉ו�����t���B", "Your inventory is full."));
		return;
	}

	if (!(EXTRA_MODE) && is_daytime())
	{
		if (one_in_(3))
			c_put_str(TERM_WHITE, _("�u�ِ��E����̐N�����Ȃ�āI�����s�ꂪ���Ӑ��������ςȂ���B�v",
                                    "'It came from the outside world! The marketplace is still open every night.'"), 10, 20);
		else if (one_in_(2))
			c_put_str(TERM_WHITE, _("�u�U�V�I�̃C�x���g�ɂ͎Q�����Ă��ꂽ������H�v",
                                    "'Have you participated in the event at False Heaven Shelf?'"), 10, 20);
		else
			c_put_str(TERM_WHITE, _("�u�s�ꂪ�J���͖̂�ɂȂ��Ă����B�v",
                                    "'The marketplace will open at night.'"), 10, 20);

		return;
	}
	else if (ability_card_trade_count > 2 && !p_ptr->wizard)
	{
		if(EXTRA_MODE)
			c_put_str(TERM_WHITE, _("�u����̌����͏I����B�܂��ʂ̃t���A�ŉ�܂��傤�B�v",
                                    "'Exchanging is over for now. Let's meet again on another floor.'"), 10, 20);
		else
			c_put_str(TERM_WHITE, _("�u�����̎s��͏I����B�܂������̔ӂɗ��ĂˁB�v",
                                    "'The marketplace is closed. Come back next night.'"), 10, 20);
		return;

	}

	//�����ɏo���J�[�h��I�я��擾

	q = _("�u�����A�M���������ɏo���J�[�h��I�Ԃ̂ł��B�v",
        "'Now, choose the card you want to exchange.'");
	s = _("�u����A�A�r���e�B�J�[�h�������Ă��Ȃ��̂ˁB�v",
        "'Oh, you don't have ability cards.'");
	msg_select = _("�Q���҂����̓J�[�h��񎦂����B",
                    "Participants presented their cards.");
	msg_reject = _("�u�c�O�����ǁA�N���M���̏o�����J�[�h�Ɍ������J�[�h�������Ă��Ȃ��悤�ˁB�v",
                    "'Unfortunately, no one else has a card to match yours.'");

	if(EXTRA_MODE)
		msg_finish = _("���F�ɓh�肽����ꂽ�����ŃJ�[�h�̌������s��ꂽ�B",
                        "You exchanged cards in a rainbow-colored room.");
	else
		msg_finish = _("�����̉��ŃJ�[�h�̌������s��ꂽ�B",
                        "You exchanged cards in the moonlight.");


	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	ref_num = 1; //�A�r���e�B�J�[�h�����͏�Ɉꖇ����
	ref_pval = o_ptr->pval;

	if (ref_pval < 0 || ref_pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format(_("ERROR:trading_ability_cards()�ɕs���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���n���ꂽ",
                    "ERROR: Ability card with incorrect card idx (%d) was passed to trading_ability_cards()"), ref_pval);
		return;
	}

	for (i = 0; i<10; i++) object_wipe(&barter_list[i]);

	//�������̃J�[�h�𐶐�����
	//�������N�̃J�[�h�͌������肪���Ȃ�
	trade_chance = 5 + (p_ptr->lev / 2 + randint1(p_ptr->lev / 2)) / 9;
	if (ability_card_list[ref_pval].rarity_rank == 4) trade_chance = trade_chance * 3 / 4;
	else if (ability_card_list[ref_pval].rarity_rank >= 3) trade_chance = trade_chance * 4 / 5;


	trade_num = 0;

	//�J�[�h�������J��Ԃ�
	for (i = 0; i<trade_chance && i<10; i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//�J�[�h���X�g��S���`�F�b�N
		for (j = 0; j<ABILITY_CARD_LIST_LEN; j++)
		{
			int k;
			bool flag_ok = TRUE;
			//�ʏ퐶������Ȃ��J�[�h�͒e��
			if (ability_card_list[j].rarity_rank<1 || ability_card_list[j].rarity_rank>4) continue;
			//�����񎦂����J�[�h�Ɠ����̂͒e��
			if (j == ref_pval) continue;
			//���łɑI�肳�ꂽ�J�[�h�Ɠ����̂͒e��
			for (k = 0; k<trade_num; k++) if (j == barter_list[k].pval) flag_ok = FALSE;
			if (!flag_ok) continue;

			//�o�����J�[�h�̔����ȉ��̌������l�̃J�[�h�͒e��
			if (ability_card_list[j].trade_value < ability_card_list[ref_pval].trade_value / 2) continue;
			//�o�����J�[�h����̌������l�̃J�[�h�͊m���Œe��
			if (randint1(ability_card_list[ref_pval].trade_value / 2) < (ability_card_list[j].trade_value - ability_card_list[ref_pval].trade_value)) continue;

			//�o�����J�[�h����̃����N�̃J�[�h��*����*�o�ɂ�������
			//�����N3��4��_���ꍇ�A�������l����Ŗ�80%,���̔����0.8%,�ʂ����J�[�h���烉���_���ɑI�ԂƂ����7%���炢�̊m����ʂ�K�v������͂��B6����₪����Ƃ��Ă��o����0.3%�B���3��g���[�h���Ă�1%��B
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

			//�����܂Œʂ����J�[�h���烉���_���ɑI��
			cnt++;
			if (!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//�I�����ꂽ�J�[�h������ꍇbarter_list�ɓo�^
		if (cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
			tmp_o_ptr->pval = tmp_pv;

			//*�Ӓ�*�ςɂ���
			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			tmp_o_ptr->ident |= (IDENT_MENTAL);


			trade_num++;
		}
	}

	//2�ȉ��̏ꍇ�ꍇ�I��
	if (trade_num < 2)
	{
		c_put_str(TERM_WHITE, msg_reject, 4, 6);
		inkey();
		ability_card_trade_count++;
		return;
	}

	//�J�[�h��\�����I��
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, _("(�������Ȃ��FESC)", "(Don't exchange: ESC)"), 5, 6);
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
			//if (get_check_strict("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H", CHECK_OKAY_CANCEL))
				break;
		}
		else if (c == '2' && cs < (trade_num - 1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c = cs + 'a';
		else if (c >= 'a' && c <= 'a' + trade_num - 1)
		{
			object_desc(o_name, &barter_list[c - 'a'], 0);

			if (ability_card_list[barter_list[c - 'a'].pval].rarity_rank < ability_card_list[ref_pval].rarity_rank)
				msg_format(_("%s�͂��Ȃ����o�����J�[�h��胉���N���Ⴂ�B",
                            "%s has lower rank than the card you're trading away."),o_name);
			else
				msg_format(_("%s���󂯎��B", "You will receive %s."), o_name);

			if (!get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

			//�I���������������
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c - 'a']);
			msg_format(_("%s���󂯎�����B", "You have received %s."), o_name);

			break;
		}
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, msg_finish, 10, 10);
	ability_card_trade_count++;
	inkey();
	clear_bldg(4, 18);


	//���s�ς݃t���O���Ă�
	//ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}



//v1.1.87
//�����R�}���h:�A�r���e�B�J�[�h�̔̔���
//examine:�J�[�h�̐�������ǂ�
void buy_ability_card(bool examine)
{
	int trade_num = 10;
	int cs, item_new;
	char c;
	object_type *o_ptr;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
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
			c_put_str(TERM_WHITE, _("�u������͂��łɏ������[�ł���B", "'I'm ready as well."), 10, 20);
			c_put_str(TERM_WHITE, _("���i�̊m�ۂ͋M���Ɍ������Ă����ł�����撣���Ă��������ˁH�v",
                                "It's up to you to secure the goods, so good luck with that!'"), 11, 20);
		}
		else if (card_rank < 5)
		{
			c_put_str(TERM_WHITE, _("�u����n�߂܂�����I", "'The sales are starting!"), 10, 20);
			c_put_str(TERM_WHITE, _("�ʔ����Ȃ��Ă�����[�I�v", "'Now it's getting interesting!'"), 11, 20);
		}
		else if (card_rank < 8)
		{
			c_put_str(TERM_WHITE, _("�u��ɐ��ł���I", "'We're making a great profit!"), 10, 20);
			c_put_str(TERM_WHITE, _("��͂�эj�ۗl�̖ڂɋ����͂���܂���ł����ˁB�v",
                                    "As I knew, Miss Iizunamaru was right on point.'"), 11, 20);
		}
		else
		{
			c_put_str(TERM_WHITE, _("�u���A�Z����...", "'S-so busy..."), 10, 20);
			c_put_str(TERM_WHITE, _("���낻�뗘�v�m�肵�Ď�d�����ɂ��܂���...�H�v",
                                    "Isn't it time to lock in the profits we got and close the business?'"), 11, 20);
		}

		return;
	}
	if (p_ptr->pclass == CLASS_TSUKASA)
	{
		msg_print(_("��̓J�[�h�ŏ\���ɉ҂����B���������ɗp�͂Ȃ��B",
                    "Your master already has plenty of cards. There's nothing for you to do here."));
		return;
	}


	if (p_ptr->pclass == CLASS_MIKE)
	{
		msg_print(_("�J�[�h�̔����ɂ͏����L�������Q��Ă��ċߊ�肽���Ȃ��B",
                    "You don't want to approach the card trading office swarming with maneki-neko."));
		return;
	}

	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		c_put_str(TERM_WHITE, _("�u�ς݂܂���˂��A�����̓J�[�h�̔��l��p�̎s��Ȃ�ł���B�v",
                                "'Sorry, but this market is for card traders only.'"), 10, 20);
		return;
	}


	if (inventory[INVEN_PACK - 1].k_idx && !examine)
	{
		msg_print(_("���͉ו�����t���B", "Your inventory is full."));
		return;
	}

	if(examine)
		msg_select = _("�u�ӂނӂށA�ǂ�����m�F�Ȃ����܂����H�v",
                        "'Hmmm, which one would you like to examine?'");
	else
		msg_select = _("�u���Ă��āA�ǂ���������グ�ɂȂ�܂����H�v",
                        "'Well, which one would you like to purchase?'");

	//make_ability_card_store_list()�Ō��܂����J�[�hidx���X�g���g���č݌Ƀ��X�g�����
	for (i = 0; i<10; i++)
	{
		int card_idx = p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT];

		if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
		{
			msg_format(_("ERROR:buy_ability_cards()�ɓn���ꂽcard_idx(%d)���͈͊O",
                        "ERROR: card_idx (%d) passed to buy_ability_cards() is out of range"), card_idx);
			return;
		}

		o_ptr = &barter_list[i];
		object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(o_ptr, card_idx, 0, 0);
		flag_no_item = FALSE;
	}

	//���Ԍo�߂ō݌ɂ̈ꕔ�𔄂�؂�ɂ���H

	//if (flag_no_item)
	//{
	//	c_put_str(TERM_WHITE, "�u���A�l�ō݌ɂ��S�ĎJ�����Ⴂ�܂����B�܂����x���Ă��������B�v", 10, 20);
	//	return;
	//}
	//���t���O�̏������~�X���ĂđS���݌ɔ�������Ă����̃��b�Z�[�W�͏o�Ȃ��B�܂��債�����ł��Ȃ��̂ŏ�����


	//�J�[�h��\�����I��
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, _("(�L�����Z���FESC)", "(Cancel: ESC)"), 5, 6);
		for (i = 0; i<trade_num; i++)
		{
			int value;
			int color;
			cptr desc;
			int price = 0;
			o_ptr = &barter_list[i];


			//pval=0(�󔒂̃J�[�h)�͔���؂ꈵ��
			if (!o_ptr->pval)
			{
				color = TERM_L_DARK;
				desc = format(_("(����؂�)", "(Sold out)"));

			}
			else
			{
				color = TERM_WHITE;
				price = calc_ability_card_price(o_ptr->pval);

				//�u���{��`�̃W�����}�v�̃J�[�h�ɂ�鉿�i������
				//p_ptr->magic_num2[10-19)��50�����l�Ƃ���1���Ƃ�2%�㉺����
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
			//if (get_check_strict("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H", CHECK_OKAY_CANCEL))
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
				msg_print(_("����͔���؂ꂾ�B", "It's sold out."));
				continue;
			}

			if (examine)//����
			{

				hack_flag_store_examine = TRUE;
				screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
				hack_flag_store_examine = FALSE;

			}
			else //�w��
			{
				price = calc_ability_card_price(o_ptr->pval);

				//�u���{��`�̃W�����}�v�̃J�[�h�ɂ�鉿�i������
				//p_ptr->magic_num2[10-19)��50�����l�Ƃ���1����邲�Ƃ�2%�㉺����
				if (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT])
				{
					price += price * (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT] - 50) * 2 / 100;
				}


				if (price > p_ptr->au)
				{
					msg_print(_("����������Ȃ��B", "You don't have enough money."));
					continue;
				}

				object_desc(o_name, o_ptr, 0);
				msg_format(_("%s���w������B", "Purchasing %s."), o_name);
				if (!get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

				inven_carry(o_ptr);
				msg_format(_("%s���w�������B", "You have purchased %s."), o_name);
				p_ptr->au -= price;
				building_prt_gold();

				//���v�x���z�ɉ��Z
				//if (total_pay_cardshop < 100000000) total_pay_cardshop += price;
				add_cardshop_profit(price);

				//�J�[�h�݌Ƀ��X�g�̊Y���ӏ����w���ς݂�0�ɂ���
				p_ptr->magic_num2[c - 'a' + ABLCARD_MAGICNUM_SHIFT] = 0;

				break;
			}

		}
	}

	clear_bldg(4, 18);



}

//v1.1.87
//�A�r���e�B�J�[�h�𔄋p����
void	sell_ability_card(void)
{
	int item;
	cptr q, s;
	object_type *o_ptr;
	int amt;
	int price = 0;




	q = _("�u�A�r���e�B�J�[�h����������̂ˁH�v", "'You're selling ability cards?'");
	s = _("�A�r���e�B�J�[�h�������Ă��Ȃ��B", "You don't have ability cards.");

	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	if (o_ptr->number > 1)
	{
		amt = get_quantity(_("�����ł����H:", "How many?:"), o_ptr->number);
		if (amt <= 0) return;
	}
	else amt = 1;

	//���i�v�Z�@�̔�����1/3
	price = (calc_ability_card_price(o_ptr->pval) + 2) / 3 * amt;


#ifdef JP
	msg_format("�u%s�v�̃J�[�h%d������%d�Ŕ�������Ă����炵���B", ability_card_list[o_ptr->pval].card_name, amt, price);
#else
    msg_format("%d card(s) '%s' will sell for $%d.", amt, ability_card_list[o_ptr->pval].card_name, price);
#endif

	if (!get_check_strict(_("����܂����H", "Sell?"), CHECK_DEFAULT_Y))
	{
		return;
	}

	p_ptr->au += price;

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
	building_prt_gold();

}


/*:::�A�C�e��������*/
///mod160307 EXTRA���[�h�Ŏg����悤����������
void grassroots_barter()
{
	int cs, item_new;
	char c;
	bool flag_1=FALSE,flag_2 = FALSE, flag_3 = FALSE;//�܂��A�C�e����n���ĂȂ��t���O�A�܂����Ԃ������Ă��Ȃ��t���O�A�A�C�e���󂯎��J�n�t���O
	//���������@�����ɂ͗��Ȃ��͂�
	if(!p_ptr->grassroots && !EXTRA_MODE)
	{
		 msg_print(_("ERROR:grassroots��������barter()���Ă΂�Ă���",
                    "ERROR: barter() was called for a non grassroots member"));
		 return;
	}

	if(EXTRA_MODE)
	{
		if(ex_buildings_param[f_info[cave[py][px].feat].subtype]==255)
		{
			msg_print(_("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B",
                "The Grassroots Network youkai have left."));
			return;
		}
		flag_1 = TRUE;
		flag_3 = TRUE;
	}
	else
	{
		if(p_ptr->grassroots == 1) flag_1 = TRUE;
		else if((s32b)(p_ptr->grassroots + 50000) > turn) flag_2 = TRUE; //50000:����
		else flag_3 = TRUE;
	}

	//�A�C�e�����n�����ɉו��������ς���������I��
	if(flag_3 && inventory[INVEN_PACK-1].k_idx)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("�킩�����P�u���́A�ו�����t�̂悤�ł����E�E�v",
                                                        "Wakasagihime - 'Um, looks like your inventory is already full...'"));
		else msg_print(_("�e�T�u�M���ו�����t��B�v", "Kagerou - 'Your inventory is full.'"));
		return;
	}



	/*:::�܂��A�C�e����n���Ă��Ȃ��Ƃ�*/
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
			q = _("���������ɏo���܂����H", "What do you want to offer for exchange?");
			s = _("�������������ł������Ȃ��̂����肢���܂��E�E",
                "I'd like something more suitable for exchange...");
		}
		else
		{
			q = _("�ǂ̃A�C�e�����o���́H", "Trade which item?");
			s = _("�M����ȕ������ĂȂ�����Ȃ��B", "You don't have valuable items.");
		}
		item_tester_hook = object_is_not_worthless;//���i��1�ȏ�̃A�C�e���𔻒肷��hook�����

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
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("�킩�����P�u���́A�������Ă܂���E�E�v",
                                                            "Wakasagihime - 'Uh, it's cursed..."));
			else msg_print(_("�e�T�u������ƁB�������Ă��B�v",
                            "Kagerou - 'Hold on. It's cursed.'"));
			return;
		}

		object_copy(q_ptr, o_ptr);
		q_ptr->number = 1;
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
		object_desc(o_name, q_ptr, 0);

		msg_format(_("%s(%c)��I�������B", "%s(%c) was selected."), o_name, index_to_label(item));
		msg_print("");
		value = object_value_real(q_ptr);

		if(value < 100)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("�킩�����P�u���܂�ǂ������͊��҂ł��Ȃ���������܂���ˁB�v",
                                                            "Wakasagihime - 'You probably won't get something good in exchange.'"));
			else msg_print(_("�e�T�u���̒��x�̂��̂��Ƃ��܂���҂ł��Ȃ����H�v",
                            "Kagerou - 'Don't expect too much in exchange.'"));
		}
		else if(value < 1000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("�킩�����P�u����������ɏo���܂���H�v",
                                                            "Wakasagihime - 'Do you want to trade this?'"));
			else msg_print(_("�e�T�u����������ɏo���̂ˁH�v",
                            "Kagerou - 'Do you want to trade this?'"));
		}
		else if(value < 10000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("�킩�����P�u����͖ʔ����i�ł��ˁB�F���񒣂�؂肻���ł��B�v",
                                                            "Wakasagihime - 'Looks interesting. Everyone's enthusiastic.'"));
			else msg_print(_("�e�T�u���X�̕i�ˁB�F����؂��B�v",
                            "Kagerou - 'Not bad. Everyone's enthusiastic.'"));
		}
		else if(value < 100000)
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("�킩�����P�u�f���炵���i�ł��ˁB�����撣��܂��I�v",
                                                            "Wakasagihime - 'That's an amazing item. I won't fall behind!'"));
			else msg_print(_("�e�T�u����͂��Ȃ�̈�i�ˁB�F����قǂ̕����o���邩����B�v",
                            "Kagerou - 'Looks very valuable. I wonder if everyone else can bring out something like that.'"));
		}
		else
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print(_("�킩�����P�u�F���񂱂�Ȃ������������Ă���Ȃ������E�E�v",
                                                            "Wakasagihime - 'Everyone else might not have something that amazing...'"));
			else msg_print(_("�e�T�u�E�E�{���ɂ����́H�M���呹���邩������Ȃ����ǁB�v",
                            "Kagerou - '...Are you certain? You might take a big loss in this exchange.'"));
		}

		if (!get_check_strict(_("�o�i�����A�C�e���͖߂��Ă��܂���B��낵���ł����H",
                                "The item you're trading cannot be brought back. Are you sure?"), CHECK_OKAY_CANCEL))
		{
			return;
		}

		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);

		if(!EXTRA_MODE)
		{
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("�킩�����P�u�܂�������ɏW�܂�܂��傤�B�撣���Ă������T���ė��܂��I�v",
                                                            "Wakasagihime - 'We'll get back together in half a day. I'll do my best to find something good!'"));
			else msg_print(_("�e�T�u���ꂶ��A�F�ɒm�点�Č�����邩�甼�����x�o������܂����ĂˁB�v",
                            "Kagerou - 'I'll inform everyone and we'll hold an exchange meeting; come back in half a day.'"));

		}
		msg_print(NULL);
		p_ptr->barter_value = value;
		if(item > INVEN_PACK) calc_android_exp();
		handle_stuff();

		//���݃^�[�����L�^
		//Ex���[�h�ł͑҂����Ԃ��Ȃ��l�b�g���[�N�������ł������ł���̂Ńt���O�ێ��̂��߂ɋL�^���Ȃ�
		if(!EXTRA_MODE) p_ptr->grassroots = turn;
	}

	/*:::�܂������o���ĂȂ��Ƃ�*/
	if(flag_2)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print(_("�킩�����P�u���݂܂���B�܂��F���񂩂�̕i���������ĂȂ���ł��B�v",
                                                        "Wakasagihime - 'Sorry, I haven't gathered the goods from everyone yet.'"));
		else msg_print(_("�e�T�u�܂��F����̕i�����o�����Ă��Ȃ��́B���������҂��ĂāB�v",
                        "Kagerou - 'I haven't received goods from everyone yet. Wait a bit longer.'"));
		return;
	}
	/*:::�A�C�e���𐶐����ꗗ�\�����Ăǂꂩ�𓾂�*/
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
					msg_print(_("ERROR:barter�A�C�e����������k_idx�������Ȓl",
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
				/*:::��炵�זh�~��20%����*/
				//140812 20%��30%
				///mod141129 LUNATIC�ł͂����ƈ�������
				if(difficulty == DIFFICULTY_LUNATIC)
					p_value = p_ptr->barter_value * 5 / 10;
				else if(EXTRA_MODE)
				{
					//�l�b�g���[�N�������ł͖��͂ŏo�����ς��悤�ɂ��Ă݂�
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
			c_put_str(TERM_WHITE,_("���̍��̗d����������̏o�����������悤���B�~�������̂�I�ڂ��B",
                                    "The goods from Grassroots Network youkai have been collected. Pick the one you like."),4 , 6);
		else if(p_ptr->pclass == CLASS_KAGEROU)
			c_put_str(TERM_WHITE,_("�킩�����P�u����ł͌�����̊J�n�ł��I�܂��e�T���񂩂�I��ł��������ˁB�v",
                                    "Wakasagihime - 'With that, let's begin exchange meeting! Kagerou, you take the first pick.'"),4 , 6);
		else
			c_put_str(TERM_WHITE,_("�e�T�u�F���҂����[�I�����M������I��ŁB�ǂꂪ�����H�v",
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
				msg_format(_("%s���󂯎��B", "You will receive %s."),o_name);
				if (get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_DEFAULT_Y)) break;
			}
		}
		clear_bldg(4,18);
		item_new = inven_carry(&barter_list[c-'a']);
		autopick_alter_item(item_new, FALSE);
		handle_stuff();
		if(EXTRA_MODE)
		{
			msg_format(_("������I�������B�d�������͉ו����܂Ƃ߂Ď��̃t���A�ւƌ��������B",
                        "Transaction complete. The youkai pack their belongings and head to lower floors."));
			//�����g�p�ς݃t���O
			ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;
			return;
		}
		else
			msg_format(_("�A�C�e���������I�������B�F�͌��������A�C�e���������ċA���Ă������B",
                        "Item exchange complete. Everyone left with items they've exchanged."));

		/*:::grassroots���f�t�H��1�֖߂��B0���ƃl�b�g���[�N�������B*/
		p_ptr->grassroots = 1;

	}
	return;

}









/*:::�J�[�h������*/
void grassroots_trading_cards(void)
{
	int i;
	int ref_pval=0,ref_num,ref_cost,ref_totalcost; //�v���C���[���n�����J�[�h�̏��
	char c;
	int cs;
	cptr q, s;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
	cptr msg_reject; //�񎦂��ꂽ�J�[�h�ɍ����J�[�h��������Ȃ������Ƃ��̃��b�Z�[�W
	cptr msg_finish; //�����I�������b�Z�[�W
	char o_name[MAX_NLEN];
	int item;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int mult; //�J�[�h�̉��l�I�莞�̕␳
	int trade_chance, trade_num; //�I�肳�������J�[�h��␔,���ۂɑI�肳�ꂽ�J�[�h��
	object_type barter_list[10];
	int base_rarity; //�J�[�h�I��Ń��A�J�[�h�̏o�₷��

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

//���s�ς݃t���O�����Ă�ꍇ�͏I��
	if(ex_buildings_param[ex_bldg_num] == 255)
	{
		msg_print(_("����͏I�������B", "Trading is over."));
		return;
	}

	if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("���͉ו�����t���B", "Your inventory is full"));
		return;
	}

	//���̍��l�b�g���[�N�ł̌����̏�����
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
			q = _("�u�ǂ�ȃJ�[�h���o���Ă�����ł����H�v", "'Which card do you want to trade?'");
			s = _("�u����A�J�[�h�������Ă��܂���ˁB�v", "'Oh, you don't have cards.'");
		}
		else
		{
			q = _("�u�����A�M�����o���J�[�h�͉�������H�v", "'So, which card are you going to trade?'");
			s = _("�u������ƁA�M���J�[�h�����ĂȂ�����Ȃ��B�v", "'Hold on, you don't have cards.'");
		}
		msg_select = _("���̍��d���l�b�g���[�N�̗d�������̓J�[�h��񎦂����B",
                        "The Grassroots Network youkai present their cards.");
		msg_reject = _("�N�����Ȃ��̏o�����J�[�h�Ɍ������J�[�h�������Ă��Ȃ������E�E",
                        "No one had a card to match yours...");
		msg_finish = _("�g���[�h�͏I�������B�d�������͉ו����܂Ƃ߂Ď��̃t���A�֌��������B",
                        "Trade complete. The youkai pack their belongings and head to lower floors.");
	}
	else if(ex_bldg_idx == BLDG_EX_REIMU)
	{
		//�p�����[�^(0-100)
		int rank = ex_buildings_param[ex_bldg_num];
		//msg_format("rank:%d",rank);
		if(!rank)
		{
			msg_print(_("�u�g���[�h�H�������m��Ȃ�����Ǒf�G�Ȃ��ΑK���͂�����B�v",
                        "'Trading? No idea what you're talking about, but here's a nice donation box.'"));
			return;
		}

		mult = 70 + rank/2;
		base_rarity = 50 + rank;
		trade_chance = 3 + rank / 15;
		q = _("�ǂ̃J�[�h���o���܂����H", "Which card do you want to trade?");
		s = _("�J�[�h�������Ă��Ȃ��B", "You don't have cards.");
		msg_select = _("�u�J�[�h���Ă���̂��ƁH���������炵�߂��d���������Ă����񂾂��ǁv",
                        "'So, that's a card? The youkai I've disciplined earlier had those.'");
		msg_reject = _("�u���[��A����Ȑ������Ȃ͎̂����ĂȂ���B�v",
                        "'Well, I don't have anything that amazing.'");
		msg_finish = _("�u����Ŏ���I���ˁB�Ƃ���őf�G�Ȃ��ΑK���͂�����B�v",
                        "'It's a deal, then. By the way, here's a nice donation box.'");

	}
	else if(ex_bldg_idx == BLDG_EX_MARISA)
	{

		mult = 100;
		base_rarity = 70;
		trade_chance = 5 ;
		q = _("�u���̃J�[�h���o���񂾁H��΂▂�@���ł��������B�v",
                "'What card do you want to trade? Gems or magic books are fine too.'");
		s = _("�����ł�����̂������Ă��Ȃ��B", "You don't have anything to exchange.");
		msg_select = _("�������̓J�[�h��񎦂����B", "Marisa shows you cards.");
		msg_reject = _("�u�����ȁB����Ɍ������J�[�h�������ĂȂ����B�v",
                        "'Sorry, I don't have any cards to match that.'");
		msg_finish = _("�u���āA�܂��ǂ����ŏ��i���d����Ă��Ȃ���ȁB�v",
                        "'Well, I'll have to obtain some products again.'");

	}
	else if (ex_bldg_idx == BLDG_EX_YUMA)
	{

		mult = 80;
		base_rarity = 60;
		trade_chance = 4;
		q = _("�u�����H�������悱���B�v", "'Give me sake or food.'");
		s = _("�����ł�����̂������Ă��Ȃ��B", "You don't have anything to exchange.");
		//msg_select = ""; ���ƂŐݒ肷��
		msg_reject = _("�u�����ȁB����Ɍ������J�[�h�������ĂȂ����B�v",
                        "'Sorry, I don't have any cards to match that.'");
		msg_finish = _("�u���͂����Ǝ|�����̂������Ă����B�v",
                        "'Bring something better next time.'");

	}
	else
	{
		msg_print(_("ERROR:���̌����ŃJ�[�h��������Ƃ��̊�{�p�����[�^���ݒ肳��Ă��Ȃ�",
                    "ERROR: Base parameter for card trading not set for this building."));
		return;
	}

	//�J�[�h�I��
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
		if(o_ptr->tval == TV_ITEMCARD) ref_num = get_quantity(_("�����ł����H:", "How many?:"), o_ptr->number);
		else ref_num = get_quantity(NULL, o_ptr->number);
		if (ref_num <= 0) return;
	}
	else ref_num = 1;

	//�J�[�h�I��O�̏������Ȃ�
	if(o_ptr->tval == TV_ITEMCARD)
	{
		ref_pval = o_ptr->pval;
		ref_cost = support_item_list[ref_pval].cost;

		ref_totalcost = ref_cost * ref_num * mult / 100;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA && o_ptr->name1 == ART_IBUKI) //v2.0.13 �ړ����@�X�ňɐ��Z��n�����Ƃ�
	{
		ref_cost = 300;
		ref_totalcost = 300;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA) //�ړ����@�X�ł���ȊO�̃A�C�e����n�����Ƃ�
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

		//����ȗ�
		if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG)
		{
			msg_select = _("�N�b�N�b�N...�����͒��������̂������Ă����ȁB",
                            "Hehehe... You've brought in something rare.");
			ref_cost = 300;
		}
		//�ɐ��Z
		else if (o_ptr->name1 == ART_IBUKI)
		{
			msg_select = _("�ق��A������ł������o��̂��H���̎��ɑł��ĕt������Ȃ����B",
                            "Oh, it produces as much sake as you want? It's perfect for me!");
			ref_cost = 250;
		}
		//�����̚�
		else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
		{
			msg_select = _("�ق��A������ł������o��̂��H���̎��ɑł��ĕt������Ȃ����B",
                            "Oh, it produces as much oil as you want? It's perfect for me!");
			ref_cost = 200;
		}
		else if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI)
		{
			msg_select = _("����͗��̍ޗ����B�����Ȃ��B",
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
				msg_select = _("�܂����̑����ɂ͂Ȃ邩�B",
                                "I guess that'll do.");
				ref_cost = 5;
				break;

			case SV_FOOD_EEL:
			case SV_FOOD_TENTAN:
			case SV_FOOD_STRANGE_BEAN:
				msg_select = _("���X���{�����肻������Ȃ����B",
                                "That looks quite nourishing!");
				ref_cost = 20;
				break;

			default:
				msg_select = _("�܂��ȁB���ɉ��������ĂȂ��̂��H",
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
				msg_select = _("�܂����̑����ɂ͂Ȃ邩�B",
                                "I guess that'll do.");
				ref_cost = 5;
				break;

				//����EXTRA�ł͎�ɓ���Ȃ������̂��������Ŏ�ɓ���悤�ɂȂ邩�������̂Őݒ肵�Ă���
			case SV_MUSHROOM_MON_SUPER:
				msg_select = _("�����͖��@�̐X�̎傩�H�Ȃ�����ȂƂ���ɁH",
                                "Isn't that the Lord of Forest of Magic? What is it doing here?");
				ref_cost = 300;
				break;

			default:
				msg_select = _("���̑����ɂ��Ȃ��ȁB", "That doesn't satisfy me.");
				ref_cost = 1;
				break;
			}
		}
		else if (o_ptr->tval == TV_SWEETS)
		{
			switch (o_ptr->sval)
			{
			case SV_SWEET_POTATO:
				msg_select = _("���������ȏĂ������ȁB�����悱���B",
                                "That's a delicious sweet potato. Hand it over!");
				ref_cost = 10;
				break;

			case SV_SWEETS_MIRACLE_FRUIT:
				msg_select = _("���X���{�����肻������Ȃ����B",
                                "That looks quite nourishing!");
				ref_cost = 20;
				break;

			case SV_SWEETS_PEACH:
				msg_select = _("����͐响����Ȃ����B���������ȁB",
                                "Isn't that a hermit peach? I miss those.");
				ref_cost = 50;
				break;

			default:
				msg_select = _("�܂��ȁB���ɉ��������ĂȂ��̂��H",
                                "Meh. Do you have anything else?");
				ref_cost = 2;
				break;
			}
		}
		else if (o_ptr->tval == TV_FLASK)
		{
			switch (o_ptr->sval)
			{
				//���P�b�g�R������ԃR�X�p���������g�����Əd�ʂ��l����Α�ʂɔ����Ď��������̂͌����I�ł͂Ȃ����낤
			case SV_FLASK_ROCKET:
			case SV_FLASK_ENERGY_RED:
			case SV_FLASK_ENERGY_BLUE:
			case SV_FLASK_ENERGY_WHITE:
			case SV_FLASK_ENERGY_BLACK:
				msg_select = _("�܂����̑����ɂ͂Ȃ邩�B",
                                "I guess that'll do.");
				ref_cost = 5;
				break;
			case SV_FLASK_ENERGY_PARTICLE:
				msg_select = _("��������́H�������Ƃ��Ȃ��͂������邼�B",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 50;
				break;
			case SV_FLASK_ENERGY_WAVEMOTION:
				msg_select = _("��������́H�������Ƃ��Ȃ��͂������邼�B",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 100;
				break;
			case SV_FLASK_ENERGY_HYPERSPACE:
				msg_select = _("��������́H�������Ƃ��Ȃ��͂������邼�B",
                                "What's this? I'm feeling some unknown power.");
				ref_cost = 200;
				break;

			default:
				msg_select = _("�܂��ȁB���ɉ��������ĂȂ��̂��H",
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
				msg_select = _("�Ȃ񂩖�L�������ȁB", "What's that medicinal liquor?");
				ref_cost = 10;
				break;

			case SV_ALCOHOL_STRONGWINE:
			case SV_ALCOHOL_MAGGOT_BEER:
			case SV_ALCOHOL_1420:
			case SV_ALCOHOL_SCARLET:
			case SV_ALCOHOL_MAMUSHI:
				msg_select = _("�Ȃ��Ȃ������Ȃ������B�����Ǝ����Ă����B", "That wasn't bad. Bring some more.");
				ref_cost = 30;
				break;

			case SV_ALCOHOL_MARISA:
				msg_select = _("�����H����������炽���̈���������Ȏ��ꂽ�㕨�ɂȂ�񂾁H",
                                "What? Just what did you do to turn cheap sake into a cursed item like this?");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_90:
			case SV_ALCOHOL_SYUTYUU:
			case SV_ALCOHOL_TRUE_ONIKOROSHI:
				msg_select = _("�ق��A�����͔����������B", "Ohh, that's delicious!");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KOKO:
			case SV_ALCOHOL_EIRIN:
				msg_select = _("�����Â����������ɖ��C�Ȃ������ȁB",
                                "Looks very old, but it's strangely tasteless.");
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KUSHINADA:
				msg_select = _("�N�b�N�b�N...���̒��x�̓Ŏ��ł��̎����ׂ��Ǝv�������H",
                                "Hehehe... Did you think you could knock me out with that much poisoned sake?");
				ref_cost = 100;
				break;

			case SV_ALCOHOL_GOLDEN_MEAD:
				msg_select = _("���̒m��Ȃ��Â������͂�������ȁB�Ȃ��Ȃ������[�����B",
                                "I'm feeling some unknown ancient power. That's very interesting.");
				ref_cost = 100;
				break;

			case SV_ALCOHOL_NECTAR:
				msg_select = _("�ق��A�ٍ��̐_�����I�����͉h�{���_���I",
                                "Ohh, an exotic divine liquor! It's so fullfilling!");
				ref_cost = 200;
				break;

			default:
				msg_select = _("�����Ƌ������͂Ȃ��̂��H", "Don't you have anything stronger?");
				ref_cost = 5;
				break;
			}
		}
		else
		{
			msg_print(_("ERROR:���̃A�C�e����ޖ��ɓn�����Ƃ��̕]���l���ݒ肳��Ă��Ȃ�",
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

		//�H������99�Ƃ��ō����ȃJ�[�h����肷�邱�Ƃ�h�����߂ɃA�C�e���̐������������totalcost�����炵�Ă���

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
		msg_print(_("ERROR:���̌����ŃJ�[�h��������Ƃ��̃R�X�g�v�Z���ݒ肳��Ă��Ȃ�",
                    "ERROR: Undefined cost calculation for card trading in this building"));
		return;
	}

	for(i=0;i<10;i++) object_wipe(&barter_list[i]);
	trade_num = 0;

	//�J�[�h�������J��Ԃ�
	for(i=0;i<trade_chance && i<10;i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//�J�[�h���X�g�S���`�F�b�N
		for(j=0;support_item_list[j].rarity;j++)
		{
			int k;
			bool flag_ok = TRUE;
			//�����񎦂����J�[�h�Ɠ����̂͒e��
			if(j == ref_pval) continue;
			//���łɑI�肳�ꂽ�J�[�h�Ɠ����̂͒e��
			for(k=0;k<trade_num;k++) if(j == barter_list[k].pval) flag_ok = FALSE;
			if(!flag_ok) continue;
			//�ꖇ�̃R�X�g�����̒񎦂����J�[�h�S���̃R�X�g��荂���̂͒e��(�R�X�g1�̃J�[�h�͒e���Ȃ�)
			if(support_item_list[j].cost > 1 && support_item_list[j].cost > ref_totalcost) continue;

			//50���o���Ă����̒񎦂����J�[�h�S���̃R�X�g�ɑ���Ȃ��̂͒e��
			if( support_item_list[j].cost * 50 < ref_totalcost) continue;

			//�Œᐶ���K��������K���[���J�[�h�͍��m���Œe��
			if(dun_level < support_item_list[j].min_lev && randint1(support_item_list[j].min_lev - dun_level) < 5 && one_in_(2)) continue;
			//�ō������K��������K���󂢃J�[�h�͒��m���Œe��
			if(dun_level > support_item_list[j].max_lev && randint1(dun_level - support_item_list[j].max_lev) < 10 ) continue;
			//�������J�[�h�͊m���Œe��
			if(randint1(support_item_list[j].rarity) > base_rarity) continue;

			//�����܂Œʂ����J�[�h���烉���_���ɑI��
			cnt++;
			if(!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//�I�����ꂽ�J�[�h������ꍇ���������߂�barter_list�ɓo�^
		if(cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr,lookup_kind(TV_ITEMCARD,SV_ITEMCARD));
			tmp_o_ptr->pval = tmp_pv;

			//��{�����@������Ɨ����K�p
			tmp_num = (ref_totalcost*2/3 + randint1(ref_totalcost*2/3) + 1) / support_item_list[tmp_pv].cost;
			//�ʏ�1�������o�Ȃ��J�[�h�͐��������ɂ���
			if(support_item_list[tmp_pv].prod_num == 1)
			{
				if(tmp_num>10) tmp_num -= (tmp_num-9) / 2;
				tmp_num /= 2;
			}
			//���܂��R�o��J�[�h�͌��炷
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

	//�ЂƂ��I�肳��Ȃ������ꍇ�I��
	if(!trade_num)
	{
		c_put_str(TERM_WHITE,msg_reject,4 , 6);
		inkey();
		return;
	}

	//�J�[�h��\�����I��
	cs = 0;
	while(1)
	{
		c_put_str(TERM_WHITE,msg_select,4 , 6);
		c_put_str(TERM_WHITE,_("(�g���[�h����߂�FESC)", "(Stop trading: ESC)"),5 , 6);
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
			if (get_check_strict(_("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H",
                                    "You won't be able to trade on this floor anymore. Are you sure?"), CHECK_OKAY_CANCEL)) break;
		}
		else if (c == '2' && cs < (trade_num-1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c=cs+'a';
		else if(c >= 'a' && c <= 'a'+trade_num-1)
		{
			object_desc(o_name, &barter_list[c-'a'], 0);
			msg_format(_("%s���󂯎��B", "You will receive %s."),o_name);
			if (!get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_DEFAULT_Y)) continue;

			//�I���������������
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c-'a']);
			msg_format(_("%s���󂯎�����B", "You have received %s."),o_name);

			break;
		}
	}

	clear_bldg(4,18);
	c_put_str(TERM_WHITE,msg_finish,4 , 6);
	inkey();
	clear_bldg(4,18);

	//���s�ς݃t���O���Ă�
	ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}




/*:::��ʃG�S�t���p�f�ރe�[�u��*/
const struct guild_arts_type archer_guild_table[] =
{
	{TV_MUSHROOM,SV_MUSHROOM_MON_RED,EGO_ARROW_FIRE,5,_("�Ή��̖�", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_RUBY,EGO_ARROW_FIRE,99,_("�Ή��̖�", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_GARNET ,EGO_ARROW_FIRE,30,_("�Ή��̖�", "Flame Arrows")},
	{TV_MATERIAL,SV_MATERIAL_HEMATITE,EGO_ARROW_FIRE,20,_("�Ή��̖�", "Flame Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_BLUE,EGO_ARROW_ELEC,5,_("�d���̖�", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_SAPPHIRE,EGO_ARROW_ELEC,99,_("�d���̖�", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_AMETHYST ,EGO_ARROW_ELEC,30,_("�d���̖�", "Lightning Arrows")},
	{TV_MATERIAL,SV_MATERIAL_LAPISLAZULI,EGO_ARROW_ELEC,20,_("�d���̖�", "Lightning Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_WHITE, EGO_ARROW_COLD,5,_("��C�̖�", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_EMERALD , EGO_ARROW_COLD,99,_("��C�̖�", "Frost Arrows")},
	{TV_MATERIAL, SV_MATERIAL_OPAL , EGO_ARROW_COLD,30,_("��C�̖�", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_AQUAMARINE, EGO_ARROW_COLD,20,_("��C�̖�", "Frost Arrows")},
	{TV_MATERIAL,SV_MATERIAL_ICESCALE, EGO_ARROW_COLD,50,_("��C�̖�", "Frost Arrows")},

	{TV_MUSHROOM,SV_MUSHROOM_MON_GREEN, EGO_ARROW_POIS,5,_("�ł̖�", "Poison Arrows")},
	{TV_MATERIAL,SV_MATERIAL_GELSEMIUM , EGO_ARROW_POIS,99,_("�ł̖�", "Poison Arrows")},
	{TV_MATERIAL,SV_MATERIAL_ARSENIC, EGO_ARROW_POIS,30,_("�ł̖�", "Poison Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_MERCURY, EGO_ARROW_POIS,50,_("�ł̖�", "Poison Arrows")},


	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER  , EGO_ARROW_HOLY,99,_("���Ȃ��", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_DIAMOND, EGO_ARROW_HOLY,50,_("���Ȃ��", "Holy Arrows")},
	{TV_MATERIAL,SV_MATERIAL_MITHRIL , EGO_ARROW_HOLY,5,_("���Ȃ��", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_HOPE_FRAGMENT, EGO_ARROW_HOLY,50,_("���Ȃ��", "Holy Arrows")},
	{TV_MATERIAL, SV_MATERIAL_BROKEN_NEEDLE, EGO_ARROW_HOLY,10,_("���Ȃ��", "Holy Arrows")},

	{TV_MATERIAL,SV_MATERIAL_TOPAZ , EGO_ARROW_HUNTER,30,_("��l�̖�", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_PERIDOT , EGO_ARROW_HUNTER,30,_("��l�̖�", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_MOONSTONE , EGO_ARROW_HUNTER,30,_("��l�̖�", "Hunter Arrows")},
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI , EGO_ARROW_HUNTER,50,_("��l�̖�", "Hunter Arrows")},

	{TV_MATERIAL,SV_MATERIAL_HIKARIGOKE , EGO_ARROW_DRAGONSLAY,5,_("���E���̖�", "Dragonslayer Arrows")},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE , EGO_ARROW_DRAGONSLAY,20,_("���E���̖�", "Dragonslayer Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU , EGO_ARROW_DRAGONSLAY,30,_("���E���̖�", "Dragonslayer Arrows") },
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL , EGO_ARROW_DRAGONSLAY,50,_("���E���̖�", "Dragonslayer Arrows")},

	//v1.1.56 �ǉ�
	{ TV_MUSHROOM,SV_MUSHROOM_MON_BLACK, EGO_ARROW_ACID,5,_("���I�̖�", "Corrosion Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT , EGO_ARROW_ACID,50,_("���I�̖�", "Corrosion Arrows")},
	{ TV_MATERIAL,SV_MATERIAL_ISHIZAKURA, EGO_ARROW_ACID,30,_("���I�̖�", "Corrosion Arrows")},

	{ TV_MATERIAL, SV_MATERIAL_IZANAGIOBJECT, EGO_ARROW_SLAY_DEITY,50,_("�V�T���̖�", "Arrows of Ame-no-Sugume")},

	{ TV_MATERIAL, SV_MATERIAL_MYSTERIUM, EGO_ARROW_AMANOZYAKU,99,_("�V�׋S�̖�", "Arrows of Amanojaku")},


	{0,0,0,0,_("�I�[�p�_�~�[", "dummy terminator")}

};

/*:::��ʋ����p�f�ރe�[�u��*/
const struct guild_arts_type archer_guild_table2[] =
{
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON ,3,99," "},
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE,7,99," "},
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE,10,99," "},
	{TV_MATERIAL, SV_MATERIAL_METEORICIRON ,15,99," "},
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,25,99," "},
	{0,0,0,0,_("�I�[�p�_�~�[", "dummy terminator")}
};



/*:::�A�[�`���[�M���h�ɂĖ�ʋ����̑f�ރA�C�e���𔻒�*/
static bool archer_guild_hook2(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table2[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table2[i].tval && o_ptr->sval == archer_guild_table2[i].sval) return TRUE;
	}
	return FALSE;
}

/*:::�A�[�`���[�M���h�ɂăG�S�t���̑f�ރA�C�e���𔻒�*/
static bool archer_guild_hook(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table[i].tval && o_ptr->sval == archer_guild_table[i].sval) return TRUE;
	}
	return FALSE;
}



/*:::�f�ނ��g���Ė����������*/
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
		msg_print(_("���Ȃ��͍H�[�ɓ���Ă��炦�Ȃ������B",
                    "You weren't let inside the workshop."));
		return;
	}

	item_tester_hook = item_tester_hook_ammo;
	q = _("�ǂ̖���������܂����H", "Improve which arrows?");
	s = _("���Ȃ��͋����ł����������Ă��Ȃ��B", "You don't have arrows to improve.");
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook2;
	q = _("�ǂ̑f�ނ��g���ċ������܂����H", "Use which material to improve?");
	s = _("���������Ȃ��͋����Ɏg���f�ނ������Ă��Ȃ������B",
            "You don't have suitable materials to use.");
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table2[i].tval;i++)
		if(archer_guild_table2[i].tval == om_ptr->tval && archer_guild_table2[i].sval == om_ptr->sval)
		{ thtd=archer_guild_table2[i].type; break;}

	if(!thtd) {msg_format(_("ERROR:archer_guild_arts2()�őf�ނ��K�؂ɓo�^����Ă��Ȃ��H",
                            "ERROR: material not properly defined in archer_guild_arts2()?")); return;}

	object_desc(o_name, o_ptr, 0);

	if(o_ptr->to_d >= thtd && o_ptr->to_h >= thtd)
	{
		msg_format(_("���̑f�ނł�%s�������ł��Ȃ��B", "You cannot improve %s with this material."),o_name);
		return;
	}

	msg_format(_("%s��+%d�܂ŋ����ł���B", "%s can be enchanted up to +%d."),o_name,thtd);

	if(!get_check(_("�������܂����H", "Enchant?"))) return;

	if(o_ptr->to_d < thtd) o_ptr->to_d = thtd;
	if(o_ptr->to_h < thtd) o_ptr->to_h = thtd;
	msg_format(_("���������������B", "Complete."));

	if (object_is_cursed(o_ptr))
	{
		msg_format(_("��̎􂢂��������B", "Your arrows are no longer cursed."));
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



/*:::�f�ނ��g���Ė�ɑ����t�^����*/
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
		msg_print(_("���Ȃ��͍H�[�ɓ���Ă��炦�Ȃ������B",
                    "You weren't let inside the workshop."));
		return;
	}

	item_tester_hook = item_tester_hook_ammo_make_ego;
	q = _("�ǂ̖�ɑ����t�^���܂����H", "Brand which arrows?");
	s = _("���Ȃ��͑����t�^�ł����������Ă��Ȃ��B", "You don't have arrows to brand.");
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook;
	q = _("�ǂ̑f�ނ��g���ċ������܂����H", "Use which material to improve?");
	s = _("���������Ȃ��͋����Ɏg���f�ނ������Ă��Ȃ������B",
            "You don't have suitable materials to use.");
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table[i].tval;i++)
	{
		if(archer_guild_table[i].tval == om_ptr->tval && archer_guild_table[i].sval == om_ptr->sval) break;
	}
	if(!archer_guild_table[i].tval) {msg_format(_("ERROR:archer_guild_arts()�őf�ނ��K�؂ɓo�^����Ă��Ȃ��H",
                                                "ERROR: material not properly registered in archer_guild_arts()?")); return;}


	object_desc(o_name, o_ptr, 0);

	num = archer_guild_table[i].num;

	if(inventory[INVEN_PACK-1].k_idx && om_ptr->number != 1 && o_ptr->number > num)
	{
		msg_print(_("�����t�^����������Ă�]�T���Ȃ��悤���B", "You don't have free space for branded arrows."));
		return;
	}


	if(num < o_ptr->number)
#ifdef JP
		msg_format("%s�̂���%d�{��%s�ɂ��邱�Ƃ��ł���B",o_name,num, archer_guild_table[i].name);
#else
        msg_format("You can make %d %s from %s.",num, archer_guild_table[i].name, o_name);
#endif
	else
		msg_format(_("%s��%s�ɂ��邱�Ƃ��ł���B",
                    "You can turn %s into %s."),o_name, archer_guild_table[i].name);

	if(!get_check(_("�����t�^���܂����H", "Perform branding?"))) return;

	object_prep(new_o_ptr,o_ptr->k_idx);

	new_o_ptr->number = MIN(num,o_ptr->number);
	new_o_ptr->to_d = (o_ptr->to_d<0)?0:o_ptr->to_d;
	new_o_ptr->to_h = (o_ptr->to_h<0)?0:o_ptr->to_h;
	new_o_ptr->name2 = archer_guild_table[i].type;
	object_aware(new_o_ptr);
	object_known(new_o_ptr);

	if (object_is_cursed(new_o_ptr))
	{
		msg_format(_("��̎􂢂��������B", "Your arrows are no longer cursed."));
		new_o_ptr->curse_flags = 0L;
	}

	msg_format(_("�����t�^�����������B",
              "Branding complete."));

	/*:::�ŏ��̃A�C�e�����������ŃC���x���g��������Ď��̃A�C�e�����������Ώۂ��ς�鋰�ꂪ����̂ŋ���̍�ŕ���B�����Ƃ܂Ƃ��ȕ��@�����肻���Ȃ��̂����v�����Ȃ������B*/
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






/*:::�A�[�`���[�M���h�̐����𕷂�*/
static void archer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print(_("���Ȃ��͎ˎ�ł͂Ȃ��̂ł����łł��邱�Ƃ͂Ȃ��悤���B",
                    "You can't do anything here since you're not an archer."));
		return;
	}
	display_rumor_new(20);
}

/*:::�O���l��p�@����_�Ђ���O�E�֋A�҂���*/
static bool escape_from_gensoukyou(void)
{
	int i;
	if(p_ptr->pclass != CLASS_OUTSIDER)
	{
		msg_print(_("���Ȃ��͌��z���o�g���B", "You are a resident of Gensoukyou."));
		return FALSE;
	}

	if( !get_check_strict(_("�O�E�֋A��܂���? ", "Return to the outside world?"), CHECK_NO_HISTORY)) return FALSE;
	if (!p_ptr->noscore)
	{
	prt(_("�m�F�̂��� '@' �������ĉ������B", "Press '@' to confirm."), 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return FALSE;

		if(p_ptr->total_winner) finish_the_game = TRUE;
	}

	/* Initialize "last message" buffer */
	if (p_ptr->last_message) string_free(p_ptr->last_message);
	p_ptr->last_message = NULL;

	/*:::last_words:�⌾�I�v�V����*/
	if (last_words)
	{
		char buf[1024] = "";
		cptr tmp_msg;

		if(p_ptr->total_winner)
			tmp_msg = _("*����*���b�Z�[�W: ", "***WINNER*** message: ");
		else
			tmp_msg = _("�����ꌾ: ", "Say something: ");

		do
		{
			while (!get_string(tmp_msg, buf, sizeof buf)) ;
		}
		while (!get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_NO_HISTORY));

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
//�񕜎{�݂ŋ߂��̔z�����񕜂��邱�Ƃɂ���
//ratio:HP�̉񕜗�
static void bldg_heal_all_pets(int ratio)
{
	int i;

	//�t���A�̃����X�^�[�����[�v
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

		if(flag_done) msg_format(_("%s�͉񕜂����B", "%s is healed."), m_name);
	}



	return;

}






/*:::����_�Ё@����*/
/*:::*����*�Ƃ͈Ⴂ�i���̎􂢕t�������̉���͑S���s���Ȃ��������ʓ|�Ȃ̂ŕ��u*/
//v1.1.32 ���_�Ђ��ǉ������̂Ń��b�Z�[�W�ǉ�
bool bldg_remove_curse( void)
{
	int i;
	u32b flag_curse=0L;
	u32b flag_perma=0L;

	//1:�얲 2:��� 3:���~��炵
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
		msg_print(_("���Ȃ��͎���Ă��Ȃ��B", "You're not cursed."));
		return FALSE;
	}
	else if(flag_curse & ~flag_perma)
	{
		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print(_("���Ђ̌˂��J���z�K�q���o�Ă��Ď􂢂��z������Ă��ꂽ�B",
                            "Suwako comes out of the branch shrine and absorbs the curse."));
			else
				msg_print(_("�얲�͂��Ȃ��̖ڂ̑O�Ɏ���������Ė��Ȏ���Ŏw�𓮂������B",
                            "Reimu holds her hand in front of you, making strange movements."));
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print(_("�z�K�q���j���j�����Ȃ���o�Ă��Ď􂢂��z��������B",
                            "Suwako comes out and absorbs the curse with a grin."));
			else
				msg_print(_("�_�ގq���o�Ă��āu�n�@�[�[�I�I�v�ƋC����������I",
                            "Kanako comes out and yells out 'Haaaah!!'"));
			break;

		case 3:
			msg_print(_("���~��炵��������ۂ悭���􂵂Ă��ꂽ�B",
                        "The zashiki-warashi skillfully remove your curses."));
			break;

		default:
			msg_print(_("ERROR:���̌����̉����������b�Z�[�W���ݒ肳��Ă��Ȃ�",
                        "ERROR: Undefined messages for removing curses in this building"));
		}

		if(remove_all_curse())
		{
			msg_print(_("�����i�̎􂢂��������I", "Your equipment is uncursed!"));

		}
		return TRUE;
	}
	else
	{

		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print(_("���Ђ̌˂��J���z�K�q���o�Ă��āA�������킸�Ɉ������񂾁B",
                            "Suwako comes out of the branch shrine and goes back in without saying a word."));
			else
				msg_print(_("�얲�u���߂�Ȃ����B���̎􂢂͎��̎�ɂ������Ȃ���B�v",
                            "Reimu - 'I'm sorry, but removing this curse is beyond my abilities.'"));
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print(_("�z�K�q�u�����܂��A�Ђǂ��M��Ɍ����߂�ꂽ���񂾂˂��B�܂����ǂ��t�������񂾂ˁB�v",
                            "Suwako - 'Wow, that's the first time I'm seeing a curse like this. Well, let's get along.'"));
			else
				msg_print(_("�_�ގq�u�ς܂ʂ����̎􂢂͐_�̎�ɂ������ʁB�w�}�f�̊����x��T���B�v",
                            "Kanako - 'I'm sorry, but this curse is beyond my powers. Look for a Scroll of Mundanity."));
			break;
		case 3:
			msg_print(_("���~��炵�������q�\�q�\�Ƒ��k�������Ă���B�䂪�Ƃ���̈����z�����������Ă���悤��...",
                        "The zashiki-warashi talk with each other in whispers. Looks like they're considering leaving your house..."));
			break;
		default:
			msg_print(_("ERROR:���̌����̉��􎸔s���b�Z�[�W���ݒ肳��Ă��Ȃ�",
                        "ERROR: Undefined message for failing to remove curses in this building"));
		}

		return FALSE;
	}


}


/*�G���W�j�A�M���h�F�@�B�����/���Օi�𒲒B����*/
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
		msg_print(_("���Ȃ��̓G���W�j�A�ł͂Ȃ��̂ŉ��ɓ���Ă��炦�Ȃ������B",
                    "You weren't let inside since you're not an engineer."));
		return;
	}

	if(make_machine && inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("�ו�����t���B���炵�Ă��痈�悤�B",
                    "Your inventory is full. Come back when you've freed up some space."));
		return;
	}
	//���i�ϓ����l���������Օi�l�i�e�[�u�������
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
			c_put_str(TERM_WHITE,_("�u�������́H�v", "'What do you want to make?'"),5 , 6);
			c_put_str(TERM_WHITE,_("(a�`j:�i�ڂ�I�� /x:���������� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",
                                    "(a-j: Pick item / x: Explanation / n, Space: Next page / ESC: Cancel"),6 , 6);
		}
		else
		{
			 c_put_str(TERM_WHITE,_("�u�ǂ���������H�v", "'What do you want to order?'"),5 , 6);
			c_put_str(TERM_WHITE,_("(a�`j:�i�ڂ�I�� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",
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
					c_put_str(color,format(_("%c) [ �ݔ��s�� ] ", "%c) [ not in this workshop ]"),'a'+i),7 + i, 10);
				}
				else if(machine_table[table_num].difficulty > power)
				{
					c_put_str(color,format(_("%c) [ �쐬�s�� ] ", "%c) [ cannot create ]"),'a'+i),7 + i, 10);
				}
				else
					c_put_str(color,format("%c) [$ %8d] %s",'a'+i, k_info[q_ptr->k_idx].cost,o_name),7 + i, 10);

			}
			else //���Օi���B�e�[�u�� �Ƃ肠�����S�Ă�\��
			{
				if(table_num >= MAX_MACHINE_SUPPLY) break;
				object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[table_num][0], engineer_guild_supply_table[table_num][1]));

				if(cs==i) color = TERM_YELLOW;
				else if(p_ptr->au < supply_price_list[table_num]) color = TERM_L_DARK;
				else color = TERM_WHITE;
				object_desc(o_name, q_ptr, 0);
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					c_put_str(color,format(_("%c) [ �݌ɖ��� ] ", "%c) [ out of stock ]"),'a'+i),7 + i, 10);
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
				msg_print(_("�u�����̐ݔ����ᖳ�����ˁB�������ǎ������̃o�U�[�܂ŗ��Ă�B�v",
                            "'Sorry, but you can't do it in this workshop. Come to our bazaar.'"));
				msg_print(NULL);
				continue;
			}
			else if(machine_table[choice].difficulty > power)
			{
				msg_print(_("���̂��Ȃ��ɂ͂�������͓̂���悤���B",
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
			if(!make_machine && choice >= MAX_MACHINE_SUPPLY) continue; //���Օi���B�ŋ󗓂�I�񂾏ꍇ

			if(make_machine)
			{
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					msg_print(_("�u�����̐ݔ����ᖳ�����ˁB�������ǎ������̃o�U�[�܂ŗ��Ă�B�v",
                                "'Sorry, but you can't do it in this workshop. Come to our bazaar.'"));
					msg_print(NULL);
					continue;
				}
				else if(machine_table[choice].difficulty > power)
				{
					msg_print(_("���̂��Ȃ��ɂ͂�������͓̂���悤���B",
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
					msg_print(_("�u���̎x���ɂ͒u���ĂȂ���B�������ǎ������̃o�U�[�܂ŗ��Ă�B�v",
                                "'Sorry, not available at this branch. Come to our bazaar.'"));
					msg_print(NULL);
					continue;
				}
				else
				{
					object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[choice][0], engineer_guild_supply_table[choice][1]));
					number = get_quantity(_("�����������܂����H", "Order how many?"),99);
					if(!number) continue;
					q_ptr->number = number;
					if(!inven_carry_okay(q_ptr))
					{
						msg_print(_("����ȂɎ�������Ȃ��B", "You can't carry that much."));
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
		msg_format(_("����������Ȃ��B", "You don't have enough money."));
		return;
	}

	if(make_machine) msg_format(_("$%d�g����%s�����B", "You'll spend $%d to build %s."),price,o_name);
	else	msg_format(_("$%d��%s�𒲒B����B", "You'll spend $%d to obtain %s."),price,o_name);
	if (!get_check_strict(_("��낵���ł����H", "Are you sure?"), CHECK_DEFAULT_Y)) return;

	//���̂Ƃ������p
	apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
	//�A�C�e����*�Ӓ�*��Ԃɂ���
	identify_item(q_ptr);
	q_ptr->ident |= (IDENT_MENTAL);

	p_ptr->au -= price;
	building_prt_gold();

	if(make_machine)
	{
		msg_print(_("���Ȃ��͋@�B�̐���Ɏ��|�������E�E", "You started building the machine..."));
		msg_print(NULL);
		msg_format(_("%s�����������I", "%s is complete!"),o_name);
	}
	else  msg_format(_("%s���󂯎�����B", "You receive %s."),o_name);

	slot_new = inven_carry(q_ptr);
	autopick_alter_item(slot_new, FALSE);

	handle_stuff();
	clear_bldg(5, 18);

}


/*:::�G���W�j�A�M���h�̐����𕷂�*/
static void engineer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI)
	{
		msg_print(_("���Ȃ��̓G���W�j�A�ł͂Ȃ��̂ł����łł��邱�Ƃ͂Ȃ��悤���B",
                    "You can't do anything here since you're not an engineer."));
		return;
	}
	display_rumor_new(21);
}


//v1.1.92 EX�_���W���������p�̉\�R�}���h ���������Ƃ�
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
		msg_print(_("ERROR:���̌�����ex_rumor_new()�������o�^����Ă��Ȃ�",
                    "ERROR: ex_rumor_new() logic undefined for this building"));


	}
}


//v1.1.92 �ޖ������ꂽ�A�C�e����H�ׂď��ł�����
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

	q = _("�u�ǂ������Ăق����H�v", "'What do you want me to eat?'");
	s = _("�u�􂢂̓����͂��Ȃ��ȁB�܂��B�v", "'Meh, I don't smell any curses.'");

	item_tester_hook = item_tester_hook_cursed;
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return;
	o_ptr = &inventory[item];

	if (!(wield_check(item, INVEN_PACK,0)))
	{
		return;
	}

	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if (!get_check(format(_("�u%s���򂤂��H�����񂾂ȁH�v", "'I'm going to eat %s; are you sure?' "), o_name))) return;

	if (o_ptr->curse_flags & TRC_PERMA_CURSE)
	{
		prt(_("�u�����͐�N���̂̎􂢂��ȁI�h�{���_���I�v",
            "'That's a thousand-year curse! It's so fullfilling!'"),10,20);
	}
	else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
	{
		prt(_("�u�Ȃ��Ȃ������Ȃ����̎􂢂��ȁB�v",
            "'This curse tasted pretty well.'"), 10, 20);
	}
	else
	{
		prt(_("�u�n���Ȏ􂢂��ȁB�򂢑����B�v",
            "'What a lousy curse. I need more food.'"), 10, 20);
	}


	inven_item_increase(item, (0-o_ptr->number));
	inven_item_describe(item);
	inven_item_optimize(item);

}



///mod150811 ���������@�쐬���[����ǂ�
static void marisa_read_memo(void)
{
	if(p_ptr->pclass != CLASS_MARISA)
	{
		msg_print(_("ERROR:�������ȊO��marisa_read_memo()���Ă΂ꂽ",
                    "ERROR: marisa_read_memo() called for non-Marisa"));
		return;
	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return;
	}

	display_rumor_new(22);
}


/*
 * Execute a building command
 */
/*:::�������ł̃R�}���h����*/
///sys �������R�}���h����
static void bldg_process_command(building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);



	//v2.0.8 �����炩�m���EXTRA�����ŉ��i��member_costs�ɐݒ肵�Ă�̂ɂ�����other_costs�ŏ������Ă邩��S���̉��i��0�ɂȂ��Ă�
	//���̏����ɖ߂��Bis_owner()�͌�����TRUE�Ȃ̂�member_costs[]�݂̂��Q�Ƃ����
	if (is_owner(bldg))
		bcost = bldg->member_costs[i];
	else

		bcost = bldg->other_costs[i];

	if(cheat_room) msg_format("cost:%d", bcost);

	/* action restrictions */
	///building �����ύX���Ȃ��Ƃ����Ȃ�
	if (((bldg->action_restr[i] == 1) && !is_member(bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(bldg)))
	{
#ifdef JP
msg_print("�����I�����錠���͂���܂���I");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	/*:::����������Ȃ��ꍇreturn ������������[�U�͕ʏ���*/
	//v1.1.48 �������}�C�i�X���Ɩ����̍s�ׂ��ł��Ȃ������̂ŏ������ǉ�
	if ((bact != BACT_RECHARGE) &&
	    ((bldg->member_costs[i] > 0 && (bldg->member_costs[i] > p_ptr->au) && is_owner(bldg)) ||
	     (bldg->other_costs[i] > 0 && (bldg->other_costs[i] > p_ptr->au) && !is_owner(bldg))))
	{
#ifdef JP
msg_print("����������܂���I");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	clear_bldg(4, 18);

	///sys building �����̊e�R�}���h���� BACT_???�Ɉ�v
	switch (bact)
	{
	case BACT_NOTHING:
		/* Do nothing */
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(FALSE);
		break;
		///sysdel building �폜�ݔ�
	case BACT_TOWN_HISTORY:
		town_history();
		break;
		///sysdel building �폜�ݔ�
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
		msg_print(_("ERROR:arena_comm()�͂����R�����g�A�E�g����",
                    "ERROR: arena_comm() already commented out"));
		//arena_comm(bact);
		break;
	///sysdel �J�W�m
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
		if (!get_check("��������S�ĊӒ肵�Ă�낵���ł����H")) break;
		identify_pack();
		msg_print(" �������S�Ă��Ӓ肳��܂����B");
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
		/*:::�������w�K����B�������݂��̐ݔ��������͂Ȃ��炵���B*/
	//	do_cmd_study();
	//	break;
	case BACT_HEALING: /* needs work */
		if (dun_level)
			msg_print(_("���Â��󂯂��B", "You are healed."));
		else
			msg_print(_("���Ȃ��͉���ő̂���₵��...", "You heal your body in the hot springs..."));

		//v2.0.3 ���ւ�l�`�Ȃǂ̔j�������邱�Ƃɂ���
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

		//v1.1.92 �z�����񕜂���
		bldg_heal_all_pets(30);

		paid = TRUE;
		break;

		///mod140113 �i�����p�ɋ�������
	case BACT_HEALING_PLUS:
		msg_print(_("���Â��󂯂��B", "You are healed."));

		//v2.0.3 ���ւ�l�`�Ȃǂ̔j�������邱�Ƃɂ���
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

		//v1.1.92 �z�����񕜂���
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
		if(!paid) msg_print(_("�\�͉񕜂̕K�v�͂Ȃ��悤���B", "Your abilities don't need restoring."));
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
	///sysdel �g�����v���̊K�w��A�� �Ƃ肠���������ɂ��Ă����\��
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int max_depth;

		clear_bldg(4, 20);
#ifdef JP
		select_dungeon = choose_dungeon("�Ƀe���|�[�g", 4, 0);
#else
		select_dungeon = choose_dungeon("teleport", 4, 0);
#endif
		show_building(bldg);
		if (!select_dungeon) return;

		max_depth = d_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		///quest �g�����v���̃e���|�ŃI�x�������΂��Ȃ��悤�ɂ��镔��
		if (select_dungeon == DUNGEON_ANGBAND)
		{
			///mod140118 �ŏI�{�X���I�x�����ɂ���
			if (quest[QUEST_YUKARI].status != QUEST_STATUS_FINISHED) max_depth = 50;
			//easy�ł͎���|���Ă�50�K�܂ł����s���Ȃ�
			if (difficulty == DIFFICULTY_EASY ) max_depth = 50;

			//if (quest[QUEST_TAISAI].status != QUEST_STATUS_FINISHED) max_depth = 98;
			//else if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%s�̉��K�Ƀe���|�[�g���܂����H", d_name + d_info[select_dungeon].name), max_depth);
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
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "�g�����v�^���[��");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("���̑�C������߂Ă���...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}

	case BACT_LOSE_MUTATION:
	/*:::�ˑR�ψَ��Á@���b�L�[�}���̔��I�[������*/
		///mod140318 �i���^�ψفA�����^�ψق̔���������ď��������������B
		{
			///mod160225 �o�O�Ή��̂��߉i���ψق�t������
			p_ptr->muta1 |= p_ptr->muta1_perma;
			p_ptr->muta2 |= p_ptr->muta2_perma;
			p_ptr->muta3 |= p_ptr->muta3_perma;
			p_ptr->muta4 |= p_ptr->muta4_perma;

			if(muta_erasable_count())
			{
				/*:::�ˑR�ψق������_���Ɏ����B20�񎎍s���č������Ă�ψقɓ�����Ȃ���FALSE�ɂȂ邪�����ɓ�����܂ł����ŌJ��Ԃ��Ă���*/
				while(!lose_mutation(0));
				paid = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("�����ׂ��ˑR�ψق������B");
#else
				msg_print("You have no mutations.");
#endif
				msg_print(NULL);
			}
			p_ptr->update |= PU_BONUS;
		}
		break;
	///sysdel ���Z��
	case BACT_BATTLE:
		msg_print(_("���Z��͐����悭������Ȃ��B", "You don't really understand the arena."));
		//kakutoujou();
		break;
	//���_��������
	case BACT_RYUUJIN:

		if(difficulty == DIFFICULTY_LUNATIC)
		{
			msg_print(_("����ȑ����������Ă���B�^�R�Ɏ��������A�����̐G�r�A�ЁX������܁E�E�H",
                        "A giant statue stand here. It has an octopus-like head, countless tentacles, terrifying claws...?"));
			if(!p_ptr->resist_chaos && !p_ptr->resist_insanity)set_image(p_ptr->image + 100);
		}
		else if(difficulty == DIFFICULTY_HARD)
			msg_print(_("���_���̖ڂ͐Ԃ������Ă���E�E",
                        "Dragon God Statue's eyes are glowing red..."));
		else
			msg_print(_("���_���̖ڂ͐������Ă���B�����͂����V�C�炵���B",
                        "Dragon God Statue's eyes are glowing blue. Looks like weather is going to be nice today."));
		//tsuchinoko();
		break;
	//case BACT_KUBI:
		///del131221 ���̍폜
		//msg_print("�������͕X�����B");
		//shoukinkubi();
		break;
	//case BACT_TARGET:
		///del131221 ���̍폜
		//msg_print(_("�������͕X�����B", "This office has closed."));
		//today_target();
		break;
	//case BACT_KANKIN:
		///del131221 ���̍폜
		//kankin();
		//break;
		///sysdel virtue
	case BACT_GACHA:
		buy_gacha_box();

		break;
	case BACT_GLASSROOTS_RUMOR:
		msg_print(_("���������B", "Unimplemented."));
		//kankin();
		break;
		/*:::���t��*/
	case BACT_HEIKOUKA:
	msg_print(_("���Ȃǒm��ʁB", "You know nothing about those 'virtues'."));
#if 0
#ifdef JP
		msg_print("���t���̋V�����s�Ȃ����B");
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

		/*:::�X�ړ�*/
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


	/*:::�h��ɂ��Ē��ׂ�i���폠�j*/
	case BACT_EVAL_AC:
		paid = eval_ac(p_ptr->dis_ac + p_ptr->dis_to_a);
		break;
	/*:::�܂ꂽ����C��*/
	case BACT_BROKEN_WEAPON:
		paid = TRUE;
		bcost = repair_broken_weapon(bcost);
		break;
	///mod140112 �\�@�����s�ɏC��
	case BACT_RUMORS_NEW:
		display_rumor_new(p_ptr->town_num);
		paid = TRUE;
		break;
	///mod140113 ���J���@�X�@�ڍז�����
	case BACT_MARIPO_GAIN:
		material_2_maripo();
		break;
	case BACT_MARIPO_CHANGE:
		maripo_2_present();
		break;
	case BACT_KORIN_MAKEOVER:
		msg_print(_("�ς܂Ȃ����A�܂��������ł��Ă��Ȃ��񂾂�B",
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
	///mod150129 ���ΑK��
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
		//Ex���[�h����@�A�C�e��������ɉ^��ł��炤
	case BACT_ITEM_TRANSFER:
		{
			int ex_bldg_num = f_info[cave[py][px].feat].subtype;
			int ex_bldg_idx = building_ex_idx[ex_bldg_num];
			int time_div = 8;
			//Hack - Ex�_���W�����̂��ӂɃA�C�e�����^��ł��炤�����́A�A�C�e��������玝���Ă��Ă�����Ă���Œ��ɂ͖����B
			if(ex_bldg_idx == BLDG_EX_ORIN &&
				(town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
			{
				msg_print(_("���͒N�����Ȃ��B", "There's nobody here."));
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
		geidontei_cooking(p_ptr->pclass == CLASS_MIYOI);//�����̂Ƃ��̂݁u�����ō��t���O�vON
		break;

	default:
		msg_format(_("ERROR:BACT(%d)������`", "ERROR: unhandled BACT_ constant, %d"), bact);

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
msg_print("�����ɂ̓N�G�X�g�̓����͂Ȃ��B");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
		int quest_num = cave[py][px].special;


#ifdef JP
		///v1.1.12 �N�G�X�g���\�L
		if(quest_num)
		{
			//�N�����_�ł�quest[]�ɖ��O�������Ă��Ȃ��̂�p_ptr->inside_quest���ꎞ�I�ɏ��������ăN�G�X�g���������s���K�v������
			int old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = quest_num;
			init_flags = INIT_NAME_ONLY;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			msg_format("�����ɂ̓N�G�X�g�u%s�v(���x��%d)�ւ̓���������܂��B",quest[quest_num].name,quest[quest_num].level);
			p_ptr->inside_quest = old_quest;
		}
		else
			msg_print("�����ɂ̓N�G�X�g�ւ̓���������܂��B");

		if (!get_check("�N�G�X�g�ɓ���܂����H")) return;
		///del131213  msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("�w�Ƃɂ��������Ă݂悤�����B�x");
#else
		if(quest_num)
		{
			//�N�����_�ł�quest[]�ɖ��O�������Ă��Ȃ��̂�p_ptr->inside_quest���ꎞ�I�ɏ��������ăN�G�X�g���������s���K�v������
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
		/*:::�N�G�X�g�̏I����Ԃ��`�F�b�N�H�Ȃ����H*/
		leave_quest_check();

		if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		p_ptr->inside_quest = cave[py][px].special;

		p_ptr->leaving = TRUE;

		///mod140629 �ϒK�푈
		/*:::-Hack- �ϒK�푈�N�G�ɓ���Ƃ��A�ǂ���̐��͂ɂ����I�����ăt���O��ۑ�����*/
		if(cave[py][px].special == QUEST_KORI)
		{
			bool enable_a = TRUE;
			bool enable_b = TRUE;
			bool enable_c = TRUE;
			int i;
			char c;

			screen_save();
			//���͌ϐw�c�̂�
			if(p_ptr->pclass == CLASS_RAN)
			{
				 enable_b = FALSE;
				 enable_c = FALSE;
			}
			//�}�~�]�E�͒K�w�c�̂�
			else if(p_ptr->pclass == CLASS_MAMIZOU)
			{
				 enable_a = FALSE;
				 enable_c = FALSE;
			}
			else if(p_ptr->prace == RACE_YOUKO) enable_b = FALSE;
			else if(p_ptr->prace == RACE_BAKEDANUKI) enable_a = FALSE;
			///mod150110 ��͒K�w�c�ɕt���Ȃ�
			else if(p_ptr->pclass == CLASS_CHEN) enable_b = FALSE;
			///mod150328 �ʂ��͒K�w�c�̂�
			else if(p_ptr->pclass == CLASS_NUE) enable_a = FALSE;
			//�얲�Ɖf�P�͟r�ł̂�
			//v2.0.17 �c�����S�ł݂̂ɂ��Ƃ�
			else if(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_ZANMU)
			{
				 enable_a = FALSE;
				 enable_b = FALSE;
			}

			if(!enable_a && !enable_b && !enable_c)
			{
				msg_format(_("ERROR:�ϒK�푈�N�G�X�g�̑I�������S�Ė���",
                            "ERROR: No valid options for Fox-Tanuki War quest"));
				return;
			}

			msg_print(_("�ǂ���̐��͂ɂ��܂����H", "Which side are you on?"));
			for(i=1;i<7;i++)Term_erase(12, i, 255);
			if(enable_a) put_str(_("a) �ϐw�c�ɖ�������", "a) Ally with the foxes"), 3 , 40);
			if(enable_b) put_str(_("b) �K�w�c�ɖ�������", "b) Ally with the tanuki"), 4 , 40);
			if(enable_c) put_str(_("c) �S���|��", "c) Beat everyone up"), 5 , 40);

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

		//v1.1.91 �R���N�G�X�g1
        //�R���N�G�ɓ���Ƃ��A�ǂ���̐��͂ɂ����I�����ăt���O��p_ptr->animal_ghost_align_flag�ɋL�^����
		else if (cave[py][px].special == QUEST_YAKUZA_1)
			{
				int i;
				char c;
				u32b selectable_flags = 0L;

				//�I���\�Ȑ��͂��t���O�Őݒ�
				//�v���e�E�X�����O�Ƃ��ŕϐg���̂Ƃ����l������H

				//������ɜ߈˂���Ă���ꍇ�ŗD��ł��̏����ɌŒ�
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
			//���S
				else if (p_ptr->pclass == CLASS_SAKI || p_ptr->pclass == CLASS_ENOKO)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
				}
			//����d
				else if (p_ptr->pclass == CLASS_YACHIE || p_ptr->pclass == CLASS_BITEN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
				}
			//�ޖ�
				else if (p_ptr->pclass == CLASS_YUMA || p_ptr->pclass == CLASS_CHIYARI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
				}
			//���ւ��ݕP
				else if (p_ptr->prace == RACE_HANIWA || p_ptr->pclass == CLASS_KEIKI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_HANIWA);
				}
			//�ق��푰������͏������͂Ō��܂�B������(���邢�͍����������邩������Ȃ����̕s���̏W�c)�͏��ֈȊO�S�đI���\
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
			//�_�ގq�ƃt�����͍��~���S��
				else if (p_ptr->pclass == CLASS_KANAKO || p_ptr->pclass == CLASS_FLAN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
			//���Ƃ܂��́H���ւ��S�ŁH
			//�얲�E�f�P�E�v�̉́E�푰���_�͑S�ŃI�����[
				else if (p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_KUTAKA || p_ptr->prace == RACE_DEATH)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
				else //��ɋ����������ɓ��Ă͂܂�Ȃ��Ƃ��S�đI���\
				{
					selectable_flags |= (ANIMAL_GHOST_ALIGN_KEIGA | ANIMAL_GHOST_ALIGN_KIKETSU | ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_HANIWA | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}

				//v2.0.13 ���N�U�N�G�X�g2�œ|�����܋���̑�����g�D�ɂ͖����ł��Ȃ��B���������ł���g�D�����Ȃ��Ȃ�����S���|���̑I���������o�Ȃ�
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KEIGA) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KEIGA);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KIKETSU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KIKETSU);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_GOUYOKU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_GOUYOKU);
				if (!selectable_flags) selectable_flags = ANIMAL_GHOST_ALIGN_KILLTHEMALL;

				screen_save();
				for (i = 1; i<9; i++)Term_erase(12, i, 255);

				put_str(_("�ǂ̐��͂ɂ��܂����H", "Join which forces?"), 1, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA) put_str(_("a) ����g", "a) Keiga Family"), 3, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU) put_str(_("b) �S���g", "b) Kiketsu Family"), 4, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU) put_str(_("c) ���~����", "c) Gouyouku Alliance"), 5, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA) put_str(_("d) ���֕��c", "d) Haniwa Corps"), 6, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL) put_str(_("e) �S���|��", "e) Defeat them all"), 7, 40);

				while (1)
				{
					c = inkey();
					//v1.1.92 �ԈႦ��&&�łȂ��ł��̂ŏC��
					if (c < 'a' || c > 'e') continue;

					if (c == 'a' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA)) continue;
					if (c == 'b' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU)) continue;
					if (c == 'c' && !(selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU)) continue;
					if (c == 'd' && !(selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA)) continue;
					if (c == 'e' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL)) continue;

					break;
				}
				screen_load();

				//�t���O��p_ptr->animal_ghost_align_flag�ɋL�^
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
					msg_print(_("ERROR:�s���ȑI�����s��ꂽ", "ERROR: Incorrect choice"));

			}


		//v1.1.24 �N�G�X�g�u�}������v�̊J�n���^�[�����L�^
		if(QRKDR)
		{
			qrkdr_rec_turn = turn;

		}

	}
}


/*
 * Do building commands
 */
/*:::�X�łȂ������ł̏����Ȃ�*/
void do_cmd_bldg(void)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;


	energy_use = 100;
#if 0
	//����ꎞ���� ���Z��Ńe���|�[�g���ăt���[�Y�����Z�[�u�f�[�^�~�o�p
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
		msg_print(_("ERROR:�����ł͂Ȃ��ꏊ��do_cmd_bldg()���Ă΂�Ă���",
                    "ERROR: do_cmd_bldg() called outside of building"));

		return;
	}

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
	{
		msg_print(_("�����ɓ���Ȃ��B", "You can't enter buildings."));
		return ;
	}

	if(SUPER_SHION)
	{
		msg_print(_("�������ɓ��������ς��B", "You really shouldn't enter buildings right now."));
		return;
	}


	if(world_player || SAKUYA_WORLD)
	{
		msg_print(_("�������Ă��Ӗ����Ȃ��B", "No point in entering right now."));
		return ;
	}

	which = f_info[cave[py][px].feat].subtype;

	bldg = &building[which];

	//���͖��������ƌ����ɓ���Ȃ�(���l�̔����̂݉�)
	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > (HINA_YAKU_LIMIT2+5000) && (p_ptr->town_num != TOWN_HITOZATO || which != 9))
	{
		msg_print(_("���낤�Ƃ������ǂ������Ă��܂����c",
                    "You tried to enter, but were chased out..."));
		return ;
	}



	//v1.1.85 �Ǔے�
	if (p_ptr->town_num == TOWN_HITOZATO && which == 12)
	{
		if (is_daytime()) //�����͕X
		{
			if(p_ptr->pclass == CLASS_MIYOI)
				msg_print(_("�܂��J�X�O���B",
                        "It's before opening time."));
			else
				msg_print(_("�u���߂�Ȃ����B�܂��J�X�O�Ȃ�ł���[�B�v",
                        "'Sorry, we're not open yet.'"));
			return;
		}
		else if (is_midnight() && p_ptr->pclass != CLASS_MIYOI) //�[��͗d����p
		{
			if (player_looks_human_side())
			{
				if (one_in_(2))msg_print(_("�X�͕܂��Ă���B������͓��₩�ȋC�z���`����Ă���̂���...",
                                        "The store is closed. But you sense lively presence from inside..."));
				else msg_print(_("�X�͕܂��Ă���B�ŔɁu�\��v�Ə����ꂽ���ȎD���|�����Ă���...?",
                                "The store is closed. There's a scrap of paper on the sign that says 'Silkmoth'...?"));
				return;
			}
		}
		else //�[��ȊO�̖�Ԃ͐l�Ԑ�p
		{
			if (!player_looks_human_side() && p_ptr->pclass != CLASS_MIYOI)
			{
				if (one_in_(2))msg_print(_("�����u������I�M���͂܂��ʖڂł���I�v",
                                        "Miyou - 'Wait! You can't go in here!'"));
				else msg_print(_("�l�Ԃ������k�΂��Ă���B�����ē���̂͂�߂Ă������B",
                                "The humans are chatting lively. Better not break in."));
				return;
			}
		}
	}





	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;
	///sys store �A���[�i��B:2�ŋL�q����Ă��邱��
#if 0
	//�{�ƃA���[�i�Ŕs�k��ɃA���[�i���p�s�ɂȂ鏈��
	if ((which == 2) && (p_ptr->arena_number < 0))
	{
#ifdef JP
		msg_print("�u�s�҂ɗp�͂Ȃ��B�v");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	//�{�ƃA���[�i�Ń����X�^�[��|���܂ŃA���[�i����o���Ȃ��Ȃ鏈��
	else if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
#ifdef JP
			prt("�Q�[�g�͕܂��Ă���B�����X�^�[�����Ȃ���҂��Ă���I", 0, 0);
#else
			prt("The gates are closed.  The monster awaits!", 0, 0);
#endif
		}
		/*:::�A���[�i�Ń����X�^�[�ɏ����ďo���ɓ��B�����Ƃ�*/
		else
		{
			/* Don't save the arena as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

			p_ptr->inside_arena = FALSE;
			p_ptr->leaving = TRUE;

			/* Re-enter the arena */
			/*:::�A���[�i�̌�����ʂ֖߂�*/
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			energy_use = 0;
		}

		return;
	}
	else
#endif

	//v1.1.51�@�V�A���[�i�Ń����X�^�[���œ|�ł��o����悤�ɂ���
	if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
			if (!get_check_strict(_("�ڂ��o�܂��܂����H", "Wake up?"), CHECK_OKAY_CANCEL)) 	return;
		}

		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->inside_arena = FALSE;
		p_ptr->leaving = TRUE;
		//v1.1.52 �o��Ƃ����t���O���Z�b�g����B�����Ŝ߈ː���E���Ă����̂͐F�X�܂���
		reset_tim_flags();

		//v1.1.54
		if (p_ptr->food < PY_FOOD_ALERT) p_ptr->food = PY_FOOD_ALERT - 1;

		//v1.1.55 �A���[�i����o��Ƃ��A�A���[�i�ɓ������Ƃ���HP�ɖ߂�
		p_ptr->chp = MIN(nightmare_record_hp, p_ptr->mhp);

		if (p_ptr->chp < 0) p_ptr->chp = 0; //paranoia

		/* Re-enter the arena */
		/*:::�A���[�i�̌�����ʂ֖߂�*/
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	/*:::���Z��Ō������������炱���ɗ���*/
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
	/*:::��������o��Ƃ��̂��߂Ɍ��́��̈ʒu��ۑ����Ă���*/
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

	/*:::��������o��܂Ń��[�v*/
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


		//EX���[�h�̌������œ���̍s�����Ƃ����������I�Ɍ�������o�����Bprocess_command()��ɓG����
		if(hack_ex_bldg_summon_idx || hack_ex_bldg_summon_type ) leave_bldg = TRUE;

		if(p_ptr->inside_battle && p_ptr->leaving) leave_bldg = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/*:::�O���l���ޗp*/
		if(p_ptr->is_dead) break;
	}

	select_floor_music(); //v1.1.58


	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	/*:::�N�G�X�g��V�����ȂǁA�X���ĕ`�悷�鏈���炵��*/
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
"���Ƀ��b�Z�[�W�����܂�Ă���:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"�ǂɃ��b�Z�[�W�����܂�Ă���:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"���b�Z�[�W��������:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"�������K�i�̏�ɏ����Ă���:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"�������������B���b�Z�[�W�������Ă���:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
/*:::�����N�G�K�ɍ~��Ă����Ƃ��̃��b�Z�[�W*/
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	monster_race    *r_ptr = &r_info[q_ptr->r_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	///mod160223 Extra�p�N�G�X�g���ꏈ��
	if(EXTRA_MODE)
	{
//		msg_print("�G�L�X�g���N�G�X�g���B���Ȃ��͐���Ă������������Ă������B");
		return;
	}


	strcpy(name, (r_name + r_ptr->name));
	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		//v1.1.92 place_quest_monsters()�̎d�l�ύX�ɂ�肱���ɂ͗��Ȃ��Ȃ�͂�
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("���̊K�͈ȑO�͒N���ɂ���Ď���Ă����悤���c�B");
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
			msg_format("���ӂ���I���̊K��%s�ɂ���Ď���Ă���I", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
msg_format("���ӂ���I���̊K��%d�̂�%s�ɂ���Ď���Ă���I", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
/*:::���݃N�G�X�g�_���W�����ɋ��邩���݂̊K�ŃN�G�X�g���s���Ă���Ƃ��N�G�X�gID��Ԃ��B*/
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
///quest �����N�G
///mod160223 �������ǉ�
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

