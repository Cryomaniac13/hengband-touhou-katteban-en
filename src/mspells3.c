/* File: mspells3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Imitation code */
#include "angband.h"

//モンスターの魔法をプレイヤーが使う処理
//v2.1.6 現在未使用の本家青魔系の関数を無効化し、現行のcast_monspell_new()などをcmd7.cからこちらに持ってきた

#if 0

#define pseudo_plev() (((p_ptr->lev + 40) * (p_ptr->lev + 40) - 1550) / 130)

static void learned_info(char *p, int power)
{
	int plev = pseudo_plev();
	int hp = p_ptr->chp;

#ifdef JP
	cptr s_dam = "損傷:";
	cptr s_dur = "期間:";
	cptr s_range = "範囲:";
	cptr s_heal = "回復:";
#else
	cptr s_dam = "dam ";
	cptr s_dur = "dur ";
	cptr s_range = "range ";
	cptr s_heal = "heal ";
#endif

	strcpy(p, "");

	switch (power)
	{
		case MS_SHRIEK:
		//case MS_XXX1:
		//case MS_XXX2:
		//case MS_XXX3:
		//case MS_XXX4:
		case MS_SCARE:
		case MS_BLIND:
		case MS_CONF:
		case MS_SLOW:
		case MS_SLEEP:
		case MS_HAND_DOOM:
		case MS_WORLD:
		case MS_SPECIAL:
		case MS_TELE_TO:
		case MS_TELE_AWAY:
		case MS_TELE_LEVEL:
		case MS_DARKNESS:
		case MS_MAKE_TRAP:
		case MS_FORGET:
		case MS_S_KIN:
		case MS_S_CYBER:
		case MS_S_MONSTER:
		case MS_S_MONSTERS:
		case MS_S_ANT:
		case MS_S_SPIDER:
		case MS_S_HOUND:
		case MS_S_HYDRA:
		case MS_S_ANGEL:
		case MS_S_DEMON:
		case MS_S_UNDEAD:
		case MS_S_DRAGON:
		case MS_S_HI_UNDEAD:
		case MS_S_HI_DRAGON:
		case MS_S_AMBERITE:
		case MS_S_UNIQUE:
			break;
		case MS_BALL_MANA:
		case MS_BALL_DARK:
		case MS_STARBURST:
			sprintf(p, " %s%d+10d10", s_dam, plev * 8 + 50);
			break;
		case MS_DISPEL:
			break;
		case MS_ROCKET:
			sprintf(p, " %s%d", s_dam, hp/4);
			break;
		case MS_SHOOT:
		{
			object_type *o_ptr = NULL;
			if (buki_motteruka(INVEN_RARM)) o_ptr = &inventory[INVEN_RARM];
			else if (buki_motteruka(INVEN_LARM)) o_ptr = &inventory[INVEN_LARM];
			else
				sprintf(p, " %s1", s_dam);
			if (o_ptr)
				sprintf(p, " %s%dd%d+%d", s_dam, o_ptr->dd, o_ptr->ds, o_ptr->to_d);
			break;
		}
		case MS_BR_ACID:
		case MS_BR_ELEC:
		case MS_BR_FIRE:
		case MS_BR_COLD:
		case MS_BR_POIS:
		case MS_BR_NUKE:
			sprintf(p, " %s%d", s_dam, hp/3);
			break;
		case MS_BR_NEXUS:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 250));
			break;
		case MS_BR_TIME:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 150));
			break;
		case MS_BR_GRAVITY:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 200));
			break;
		case MS_BR_MANA:
			sprintf(p, " %s%d", s_dam, MIN(hp/3, 250));
			break;
		case MS_BR_NETHER:
		case MS_BR_LITE:
		case MS_BR_DARK:
	//	case MS_BR_CONF:
		case MS_BR_SOUND:
		case MS_BR_CHAOS:
		case MS_BR_DISEN:
		case MS_BR_SHARDS:
		case MS_BR_PLASMA:
			sprintf(p, " %s%d", s_dam, hp/6);
			break;
		case MS_BR_INERTIA:
	//	case MS_BR_FORCE:
			sprintf(p, " %s%d", s_dam, MIN(hp/6, 200));
			break;
		case MS_BR_DISI:
			sprintf(p, " %s%d", s_dam, MIN(hp/6, 150));
			break;
	//	case MS_BALL_NUKE:
	//		sprintf(p, " %s%d+10d6", s_dam, plev * 2);
	//		break;
		case MS_BALL_CHAOS:
			sprintf(p, " %s%d+10d10", s_dam, plev * 4);
			break;
		case MS_BALL_ACID:
			sprintf(p, " %s15+d%d", s_dam, plev * 6);
			break;
		case MS_BALL_ELEC:
			sprintf(p, " %s8+d%d", s_dam, plev * 3);
			break;
		case MS_BALL_FIRE:
			sprintf(p, " %s10+d%d", s_dam, plev * 7);
			break;
		case MS_BALL_COLD:
			sprintf(p, " %s10+d%d", s_dam, plev * 3);
			break;
		case MS_BALL_POIS:
			sprintf(p, " %s12d2", s_dam);
			break;
		case MS_BALL_NETHER:
			sprintf(p, " %s%d+10d10", s_dam, plev * 2 + 50);
			break;
		case MS_BALL_WATER:
			sprintf(p, " %s50+d%d", s_dam, plev * 4);
			break;
		case MS_DRAIN_MANA:
			sprintf(p, " %sd%d+%d", s_heal, plev, plev);
			break;
		case MS_MIND_BLAST:
			sprintf(p, " %s8d8", s_dam);
			break;
		case MS_BRAIN_SMASH:
			sprintf(p, " %s12d15", s_dam);
			break;
		case MS_CAUSE_1:
			sprintf(p, " %s3d8", s_dam);
			break;
		case MS_CAUSE_2:
			sprintf(p, " %s8d8", s_dam);
			break;
		case MS_CAUSE_3:
			sprintf(p, " %s10d15", s_dam);
			break;
		case MS_CAUSE_4:
			sprintf(p, " %s15d15", s_dam);
			break;
		case MS_BOLT_ACID:
			sprintf(p, " %s%d+7d8", s_dam, plev * 2 / 3);
			break;
		case MS_BOLT_ELEC:
			sprintf(p, " %s%d+4d8", s_dam, plev * 2 / 3);
			break;
		case MS_BOLT_FIRE:
			sprintf(p, " %s%d+9d8", s_dam, plev * 2 / 3);
			break;
		case MS_BOLT_COLD:
			sprintf(p, " %s%d+6d8", s_dam, plev * 2 / 3);
			break;
		case MS_BOLT_NETHER:
			sprintf(p, " %s%d+5d5", s_dam, 30 + plev * 8 / 3);
			break;
		case MS_BOLT_WATER:
			sprintf(p, " %s%d+10d10", s_dam, plev * 2);
			break;
		case MS_BOLT_MANA:
			sprintf(p, " %s50+d%d", s_dam, plev * 7);
			break;
		case MS_BOLT_PLASMA:
			sprintf(p, " %s%d+8d7", s_dam, plev * 2 + 10);
			break;
		case MS_BOLT_ICE:
			sprintf(p, " %s%d+6d6", s_dam, plev * 2);
			break;
		case MS_MAGIC_MISSILE:
			sprintf(p, " %s%d+2d6", s_dam, plev * 2 / 3);
			break;
		case MS_SPEED:
			sprintf(p, " %sd%d+%d", s_dur, 20+plev, plev);
			break;
		case MS_HEAL:
			sprintf(p, " %s%d", s_heal, plev*4);
			break;
		case MS_INVULNER:
			sprintf(p, " %sd7+7", s_dur);
			break;
		case MS_BLINK:
			sprintf(p, " %s10", s_range);
			break;
		case MS_TELEPORT:
			sprintf(p, " %s%d", s_range, plev * 5);
			break;
		case MS_PSY_SPEAR:
			sprintf(p, " %s100+d%d", s_dam, plev * 3);
			break;
		//case MS_RAISE_DEAD:
		//	sprintf(p, " %s5", s_range);
		//	break;
		default:
			break;
	}
}
#endif

#if 0
/*
 * Allow user to choose a imitation.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
 /*:::青魔の魔法から選択する*/
static int get_learned_power(int *sn)
{
	int             i = 0;
	int             num = 0;
	int             y = 1;
	int             x = 18;
	int             minfail = 0;
	int             plev = p_ptr->lev;
	int             chance = 0;
	int             ask = TRUE, mode = 0;
	int             spellnum[MAX_MONSPELLS];
	char            ch;
	char            choice;
	char            out_val[160];
	char            comment[80];
	s32b            f4 = 0, f5 = 0, f6 = 0;
#ifdef JP
cptr            p = "魔法";
#else
	cptr            p = "magic";
#endif

	monster_power   spell;
	bool            flag, redraw;
	int menu_line = (use_menu ? 1 : 0);

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Success */
		return (TRUE);
	}

#endif /* ALLOW_REPEAT -- TNB */

	if (use_menu)
	{
		screen_save();

		while(!mode)
		{
#ifdef JP
			prt(format(" %s ボルト", (menu_line == 1) ? "》" : "  "), 2, 14);
			prt(format(" %s ボール", (menu_line == 2) ? "》" : "  "), 3, 14);
			prt(format(" %s ブレス", (menu_line == 3) ? "》" : "  "), 4, 14);
			prt(format(" %s 召喚", (menu_line == 4) ? "》" : "  "), 5, 14);
			prt(format(" %s その他", (menu_line == 5) ? "》" : "  "), 6, 14);
			prt("どの種類の魔法を使いますか？", 0, 0);
#else
			prt(format(" %s bolt", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s ball", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s breath", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt(format(" %s sommoning", (menu_line == 4) ? "> " : "  "), 5, 14);
			prt(format(" %s others", (menu_line == 5) ? "> " : "  "), 6, 14);
			prt("use which type of magic? ", 0, 0);
#endif
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return FALSE;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 4;
				break;
			case '\r':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if (menu_line > 5) menu_line -= 5;
		}
		screen_load();
	}
	else
	{
#ifdef JP
	sprintf(comment, "[A]ボルト, [B]ボール, [C]ブレス, [D]召喚, [E]その他:");
#else
	sprintf(comment, "[A] bolt, [B] ball, [C] breath, [D] summoning, [E] others:");
#endif
	while (TRUE)
	{
		if (!get_com(comment, &ch, TRUE))
		{
			return FALSE;
		}
		if (ch == 'A' || ch == 'a')
		{
			mode = 1;
			break;
		}
		if (ch == 'B' || ch == 'b')
		{
			mode = 2;
			break;
		}
		if (ch == 'C' || ch == 'c')
		{
			mode = 3;
			break;
		}
		if (ch == 'D' || ch == 'd')
		{
			mode = 4;
			break;
		}
		if (ch == 'E' || ch == 'e')
		{
			mode = 5;
			break;
		}
	}
	}

	set_rf_masks(&f4, &f5, &f6, mode);

	for (i = 0, num = 0; i < 32; i++)
	{
		if ((0x00000001 << i) & f4) spellnum[num++] = i;
	}
	for (; i < 64; i++)
	{
		if ((0x00000001 << (i - 32)) & f5) spellnum[num++] = i;
	}
	for (; i < 96; i++)
	{
		if ((0x00000001 << (i - 64)) & f6) spellnum[num++] = i;
	}
	for (i = 0; i < num; i++)
	{
		if (p_ptr->magic_num2[spellnum[i]])
		{
			if (use_menu) menu_line = i+1;
			break;
		}
	}
	if (i == num)
	{
#ifdef JP
		msg_print("その種類の魔法は覚えていない！");
#else
		msg_print("You don't know any spell of this type.");
#endif
		return (FALSE);
	}

	/* Build a prompt (accept all spells) */
	(void)strnfmt(out_val, 78,
#ifdef JP
		      "(%c-%c, '*'で一覧, ESC) どの%sを唱えますか？",
#else
		      "(%c-%c, *=List, ESC=exit) Use which %s? ",
#endif
		      I2A(0), I2A(num - 1), p);

	if (use_menu) screen_save();

	/* Get a spell from the user */

	choice= (always_show_list || use_menu) ? ESCAPE:1 ;
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice, TRUE) )break;

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return (FALSE);
				}

				case '8':
				case 'k':
				case 'K':
				{
					do
					{
						menu_line += (num-1);
						if (menu_line > num) menu_line -= num;
					} while(!p_ptr->magic_num2[spellnum[menu_line-1]]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if (menu_line > num) menu_line -= num;
					} while(!p_ptr->magic_num2[spellnum[menu_line-1]]);
					break;
				}

				case '6':
				case 'l':
				case 'L':
				{
					menu_line=num;
					while(!p_ptr->magic_num2[spellnum[menu_line-1]]) menu_line--;
					break;
				}

				case '4':
				case 'h':
				case 'H':
				{
					menu_line=1;
					while(!p_ptr->magic_num2[spellnum[menu_line-1]]) menu_line++;
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
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				char psi_desc[80];

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
#ifdef JP
put_str("名前", y, x + 5);
#else
				put_str("Name", y, x + 5);
#endif

#ifdef JP
put_str("MP 失率 効果", y, x + 33);
#else
				put_str("SP Fail Info", y, x + 32);
#endif


				/* Dump the spells */
				for (i = 0; i < num; i++)
				{
					int need_mana;

					prt("", y + i + 1, x);
					if (!p_ptr->magic_num2[spellnum[i]]) continue;

					/* Access the spell */
					spell = monster_powers[spellnum[i]];

					chance = spell.fail;

					/* Reduce failure rate by "effective" level adjustment */
					if (plev > spell.level) chance -= 3 * (plev - spell.level);
					else chance += (spell.level - plev);

					/* Reduce failure rate by INT/WIS adjustment */
					chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[A_INT]] - 1);

					chance = mod_spell_chance_1(chance);

					need_mana = mod_need_mana(monster_powers[spellnum[i]].smana, 0, REALM_NONE);

					/* Not enough mana to cast */
					if (need_mana > p_ptr->csp)
					{
						chance += 5 * (need_mana - p_ptr->csp);
					}

					/* Extract the minimum failure rate */
					minfail = adj_mag_fail[p_ptr->stat_ind[A_INT]];

					/* Minimum failure rate */
					if (chance < minfail) chance = minfail;

					/* Stunning makes spells harder */
					if (p_ptr->stun > 50) chance += 25;
					else if (p_ptr->stun) chance += 15;

					/* Always a 5 percent chance of working */
					if (chance > 95) chance = 95;

					chance = mod_spell_chance_2(chance);

					/* Get info */
					learned_info(comment, spellnum[i]);

					if (use_menu)
					{
#ifdef JP
						if (i == (menu_line-1)) strcpy(psi_desc, "  》");
#else
						if (i == (menu_line-1)) strcpy(psi_desc, "  > ");
#endif
						else strcpy(psi_desc, "    ");
					}
					else sprintf(psi_desc, "  %c)", I2A(i));

					/* Dump the spell --(-- */
					strcat(psi_desc, format(" %-26s %3d %3d%%%s",
						spell.name, need_mana,
						chance, comment));
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				if (y < 22) prt("", y + i + 1, x);
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
			ask = isupper(choice);

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num) || !p_ptr->magic_num2[spellnum[i]])
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = monster_powers[spellnum[i]];

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
			(void) strnfmt(tmp_val, 78, "%sの魔法を唱えますか？", monster_powers[spellnum[i]].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", monster_powers[spellnum[i]].name);
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
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = spellnum[i];

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}
#endif

#if 0
/*
 * do_cmd_cast calls this function if the player's class
 * is 'imitator'.
 */
static bool cast_learned_spell(int spell, bool success)
{
	int             dir;
	int             plev = pseudo_plev();
	int     summon_lev = p_ptr->lev * 2 / 3 + randint1(p_ptr->lev/2);
	int             hp = p_ptr->chp;
	int             damage = 0;
	bool   pet = success;
	bool   no_trump = FALSE;
	u32b p_mode, u_mode = 0L, g_mode;

	if (pet)
	{
		p_mode = PM_FORCE_PET;
		g_mode = 0;
	}
	else
	{
		p_mode = PM_NO_PET;
		g_mode = PM_ALLOW_GROUP;
	}

	if (!success || (randint1(50+plev) < plev/10)) u_mode = PM_ALLOW_UNIQUE;

	/* spell code */
	switch (spell)
	{
	case MS_SHRIEK:
#ifdef JP
msg_print("かん高い金切り声をあげた。");
#else
		msg_print("You make a high pitched shriek.");
#endif

		aggravate_monsters(0,FALSE);
		break;
	//case MS_XXX1:
	//	break;
	case MS_DISPEL:
	{
		int m_idx;

		if (!target_set(TARGET_KILL)) return FALSE;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		dispel_monster_status(m_idx);
		break;
	}
	case MS_ROCKET:
		if (!get_aim_dir(&dir)) return FALSE;
		else
#ifdef JP
msg_print("ロケットを発射した。");
#else
			msg_print("You fire a rocket.");
#endif
		damage = hp / 4;
			fire_rocket(GF_ROCKET, dir, damage, 2);
		break;
	case MS_SHOOT:
	{
		object_type *o_ptr = NULL;

		if (!get_aim_dir(&dir)) return FALSE;
		else
		{
#ifdef JP
msg_print("矢を放った。");
#else
			msg_print("You fire an arrow.");
#endif
			if (buki_motteruka(INVEN_RARM)) o_ptr = &inventory[INVEN_RARM];
			else if (buki_motteruka(INVEN_LARM)) o_ptr = &inventory[INVEN_LARM];
			else
			damage = 1;
			if (o_ptr)
			{
				damage = damroll(o_ptr->dd, o_ptr->ds)+ o_ptr->to_d;
				if (damage < 1) damage = 1;
			}
			fire_bolt(GF_ARROW, dir, damage);
		}
		break;
	}
	//case MS_XXX2:
	//	break;
	//case MS_XXX3:
		//break;
	//case MS_XXX4:
	//	break;
	case MS_BR_ACID:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("酸のブレスを吐いた。");
#else
			else msg_print("You breathe acid.");
#endif
		damage = hp / 3;
		fire_ball(GF_ACID, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_ELEC:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("稲妻のブレスを吐いた。");
#else
			else msg_print("You breathe lightning.");
#endif
		damage = hp / 3;
		fire_ball(GF_ELEC, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_FIRE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("火炎のブレスを吐いた。");
#else
			else msg_print("You breathe fire.");
#endif
		damage = hp / 3;
		fire_ball(GF_FIRE, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_COLD:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("冷気のブレスを吐いた。");
#else
			else msg_print("You breathe frost.");
#endif
		damage = hp / 3;
		fire_ball(GF_COLD, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_POIS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("ガスのブレスを吐いた。");
#else
			else msg_print("You breathe gas.");
#endif
		damage = hp / 3;
		fire_ball(GF_POIS, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_NETHER:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("地獄のブレスを吐いた。");
#else
			else msg_print("You breathe nether.");
#endif
		damage = hp / 6;
		fire_ball(GF_NETHER, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_LITE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("閃光のブレスを吐いた。");
#else
			else msg_print("You breathe light.");
#endif
		damage = hp / 6;
		fire_ball(GF_LITE, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_DARK:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("暗黒のブレスを吐いた。");
#else
			else msg_print("You breathe darkness.");
#endif
		damage = hp / 6;
		fire_ball(GF_DARK, dir, damage, (plev > 40 ? -3 : -2));
		break;
/*
	case MS_BR_CONF:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("混乱のブレスを吐いた。");
#else
			else msg_print("You breathe confusion.");
#endif
		damage = hp / 6;
		fire_ball(GF_CONFUSION, dir, damage, (plev > 40 ? -3 : -2));
		break;
*/
		case MS_BR_SOUND:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("轟音のブレスを吐いた。");
#else
			else msg_print("You breathe sound.");
#endif
		damage = hp / 6;
		fire_ball(GF_SOUND, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_CHAOS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("カオスのブレスを吐いた。");
#else
			else msg_print("You breathe chaos.");
#endif
		damage = hp / 6;
		fire_ball(GF_CHAOS, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_DISEN:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("劣化のブレスを吐いた。");
#else
			else msg_print("You breathe disenchantment.");
#endif
		damage = hp / 6;
		fire_ball(GF_DISENCHANT, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_NEXUS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("因果混乱のブレスを吐いた。");
#else
			else msg_print("You breathe nexus.");
#endif
		damage = MIN(hp / 3, 250);
		fire_ball(GF_NEXUS, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_TIME:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("時間逆転のブレスを吐いた。");
#else
			else msg_print("You breathe time.");
#endif
		damage = MIN(hp / 3, 150);
		fire_ball(GF_TIME, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_INERTIA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("遅鈍のブレスを吐いた。");
#else
			else msg_print("You breathe inertia.");
#endif
		damage = MIN(hp / 6, 200);
		fire_ball(GF_INACT, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_GRAVITY:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("重力のブレスを吐いた。");
#else
			else msg_print("You breathe gravity.");
#endif
		damage = MIN(hp / 3, 200);
		fire_ball(GF_GRAVITY, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_SHARDS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("破片のブレスを吐いた。");
#else
			else msg_print("You breathe shards.");
#endif
		damage = hp / 6;
		fire_ball(GF_SHARDS, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BR_PLASMA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("プラズマのブレスを吐いた。");
#else
			else msg_print("You breathe plasma.");
#endif
		damage = MIN(hp / 6, 150);
		fire_ball(GF_PLASMA, dir, damage, (plev > 40 ? -3 : -2));
		break;
/*
	case MS_BR_FORCE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("フォースのブレスを吐いた。");
#else
			else msg_print("You breathe force.");
#endif
		damage = MIN(hp / 6, 200);
		fire_ball(GF_FORCE, dir, damage, (plev > 40 ? -3 : -2));
		break;
*/
	case MS_BR_MANA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("魔力のブレスを吐いた。");
#else
			else msg_print("You breathe mana.");
#endif

		damage = MIN(hp / 3, 250);
		fire_ball(GF_MANA, dir, damage, (plev > 40 ? -3 : -2));
		break;
/*
	case MS_BALL_NUKE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("放射能球を放った。");
#else
			else msg_print("You cast a ball of radiation.");
#endif
		damage = plev * 2 + damroll(10, 6);
		fire_ball(GF_NUKE, dir, damage, 2);
		break;
*/
		case MS_BR_NUKE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("放射性廃棄物のブレスを吐いた。");
#else
			else msg_print("You breathe toxic waste.");
#endif
		damage = hp / 3;
		fire_ball(GF_NUKE, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BALL_CHAOS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("純ログルスを放った。");
#else
			else msg_print("You invoke a raw Logrus.");
#endif
		damage = plev * 4 + damroll(10, 10);
		fire_ball(GF_CHAOS, dir, damage, 4);
		break;
	case MS_BR_DISI:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("分解のブレスを吐いた。");
#else
			else msg_print("You breathe disintegration.");
#endif
		damage = MIN(hp / 6, 150);
		fire_ball(GF_DISINTEGRATE, dir, damage, (plev > 40 ? -3 : -2));
		break;
	case MS_BALL_ACID:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("アシッド・ボールの呪文を唱えた。");
#else
			else msg_print("You cast an acid ball.");
#endif
		damage = randint1(plev * 6) + 15;
		fire_ball(GF_ACID, dir, damage, 2);
		break;
	case MS_BALL_ELEC:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("サンダー・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a lightning ball.");
#endif
		damage = randint1(plev * 3) + 8;
		fire_ball(GF_ELEC, dir, damage, 2);
		break;
	case MS_BALL_FIRE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("ファイア・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a fire ball.");
#endif
		damage = randint1(plev * 7) + 10;
		fire_ball(GF_FIRE, dir, damage, 2);
		break;
	case MS_BALL_COLD:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("アイス・ボールの呪文を唱えた。");
#else
			else msg_print("You cast a frost ball.");
#endif
		damage = randint1(plev * 3) + 10;
		fire_ball(GF_COLD, dir, damage, 2);
		break;
	case MS_BALL_POIS:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("悪臭雲の呪文を唱えた。");
#else
			else msg_print("You cast a stinking cloud.");
#endif
		damage = damroll(12,2);
		fire_ball(GF_POIS, dir, damage, 2);
		break;
	case MS_BALL_NETHER:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("地獄球の呪文を唱えた。");
#else
			else msg_print("You cast a nether ball.");
#endif
		damage = plev * 2 + 50 + damroll(10, 10);
		fire_ball(GF_NETHER, dir, damage, 2);
		break;
	case MS_BALL_WATER:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("流れるような身振りをした。");
#else
			else msg_print("You gesture fluidly.");
#endif
		damage = randint1(plev * 4) + 50;
		fire_ball(GF_WATER, dir, damage, 4);
		break;
	case MS_BALL_MANA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("魔力の嵐の呪文を念じた。");
#else
			else msg_print("You invoke a mana storm.");
#endif
		damage = plev * 8 + 50 + damroll(10, 10);
		fire_ball(GF_MANA, dir, damage, 4);
		break;
	case MS_BALL_DARK:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("暗黒の嵐の呪文を念じた。");
#else
			else msg_print("You invoke a darkness storm.");
#endif
		damage = plev * 8 + 50 + damroll(10, 10);
		fire_ball(GF_DARK, dir, damage, 4);
		break;
	case MS_DRAIN_MANA:
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball_hide(GF_DRAIN_MANA, dir, randint1(plev)+plev, 0);
		break;
	case MS_MIND_BLAST:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(7, 7);
		fire_ball_hide(GF_MIND_BLAST, dir, damage, 0);
		break;
	case MS_BRAIN_SMASH:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(12, 12);
		fire_ball_hide(GF_BRAIN_SMASH, dir, damage, 0);
		break;
	case MS_CAUSE_1:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(3, 8);
		fire_ball_hide(GF_CAUSE_1, dir, damage, 0);
		break;
	case MS_CAUSE_2:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(8, 8);
		fire_ball_hide(GF_CAUSE_2, dir, damage, 0);
		break;
	case MS_CAUSE_3:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(10, 15);
		fire_ball_hide(GF_CAUSE_3, dir, damage, 0);
		break;
	case MS_CAUSE_4:
		if (!get_aim_dir(&dir)) return FALSE;
		damage = damroll(15, 15);
		fire_ball_hide(GF_CAUSE_4, dir, damage, 0);
		break;
	case MS_BOLT_ACID:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("アシッド・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast an acid bolt.");
#endif
		damage = damroll(7, 8) + plev * 2 / 3;
		fire_bolt(GF_ACID, dir, damage);
		break;
	case MS_BOLT_ELEC:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("サンダー・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a lightning bolt.");
#endif
		damage = damroll(4, 8) + plev * 2 / 3;
		fire_bolt(GF_ELEC, dir, damage);
		break;
	case MS_BOLT_FIRE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("ファイア・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a fire bolt.");
#endif
		damage = damroll(9, 8) + plev * 2 / 3;
		fire_bolt(GF_FIRE, dir, damage);
		break;
	case MS_BOLT_COLD:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("アイス・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a frost bolt.");
#endif
		damage = damroll(6, 8) + plev * 2 / 3;
		fire_bolt(GF_COLD, dir, damage);
		break;
	case MS_STARBURST:
		if (!get_aim_dir(&dir)) return FALSE;
		else
#ifdef JP
msg_print("スターバーストの呪文を念じた。");
#else
			msg_print("You invoke a starburst.");
#endif
		damage = plev * 8 + 50 + damroll(10, 10);
		fire_ball(GF_LITE, dir, damage, 4);
		break;
	case MS_BOLT_NETHER:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("地獄の矢の呪文を唱えた。");
#else
			else msg_print("You cast a nether bolt.");
#endif
		damage = 30 + damroll(5, 5) + plev * 8 / 3;
		fire_bolt(GF_NETHER, dir, damage);
		break;
	case MS_BOLT_WATER:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("ウォーター・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a water bolt.");
#endif
		damage = damroll(10, 10) + plev * 2;
		fire_bolt(GF_WATER, dir, damage);
		break;
	case MS_BOLT_MANA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("魔力の矢の呪文を唱えた。");
#else
			else msg_print("You cast a mana bolt.");
#endif
		damage = randint1(plev * 7) + 50;
		fire_bolt(GF_MANA, dir, damage);
		break;
	case MS_BOLT_PLASMA:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("プラズマ・ボルトの呪文を唱えた。");
#else
			else msg_print("You cast a plasma bolt.");
#endif
		damage = 10 + damroll(8, 7) + plev * 2;
		fire_bolt(GF_PLASMA, dir, damage);
		break;
	case MS_BOLT_ICE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("極寒の矢の呪文を唱えた。");
#else
			else msg_print("You cast a ice bolt.");
#endif
		damage = damroll(6, 6) + plev * 2;
		fire_bolt(GF_ICE, dir, damage);
		break;
	case MS_MAGIC_MISSILE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("マジック・ミサイルの呪文を唱えた。");
#else
			else msg_print("You cast a magic missile.");
#endif
		damage = damroll(2, 6) + plev * 2 / 3;
		fire_bolt(GF_MISSILE, dir, damage);
		break;
	case MS_SCARE:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("恐ろしげな幻覚を作り出した。");
#else
			else msg_print("You cast a fearful illusion.");
#endif
		fear_monster(dir, plev+10);
		break;
	case MS_BLIND:
		if (!get_aim_dir(&dir)) return FALSE;
		confuse_monster(dir, plev * 2);
		break;
	case MS_CONF:
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("誘惑的な幻覚をつくり出した。");
#else
			else msg_print("You cast a mesmerizing illusion.");
#endif
		confuse_monster(dir, plev * 2);
		break;
	case MS_SLOW:
		if (!get_aim_dir(&dir)) return FALSE;
		slow_monster(dir, plev);
		break;
	case MS_SLEEP:
		if (!get_aim_dir(&dir)) return FALSE;
		sleep_monster(dir, plev);
		break;
	case MS_SPEED:
		(void)set_fast(randint1(20 + plev) + plev, FALSE);
		break;
	case MS_HAND_DOOM:
	{
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("<破滅の手>を放った！");
#else
		else msg_print("You invoke the Hand of Doom!");
#endif

		fire_ball_hide(GF_HAND_DOOM, dir, plev * 3, 0);
		break;
	}
	case MS_HEAL:
#ifdef JP
msg_print("自分の傷に念を集中した。");
#else
			msg_print("You concentrate on your wounds!");
#endif
		(void)hp_player(plev*4);
		(void)set_stun(0);
		(void)set_cut(0);
		break;
	case MS_INVULNER:
#ifdef JP
msg_print("無傷の球の呪文を唱えた。");
#else
			msg_print("You cast a Globe of Invulnerability.");
#endif
		(void)set_invuln(randint1(4) + 4, FALSE);
		break;
	case MS_BLINK:
		teleport_player(10, 0L);
		break;
	case MS_TELEPORT:
		teleport_player(plev * 5, 0L);
		break;
	case MS_WORLD:
		world_player = TRUE;
#ifdef JP
		msg_print("「時よ！」");
#else
		msg_print("'Time!'");
#endif
		msg_print(NULL);

		/* Hack */
		p_ptr->energy_need -= 1000 + (100 + randint1(200)+200)*TURNS_PER_TICK/10;

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		handle_stuff();
		break;
	case MS_SPECIAL:
		break;
	case MS_TELE_TO:
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];

		if (!target_set(TARGET_KILL)) return FALSE;
		if (!cave[target_row][target_col].m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		m_ptr = &m_list[cave[target_row][target_col].m_idx];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);
		if (r_ptr->flagsr & RFR_RES_TELE)
		{
			if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
				msg_format("%sには効果がなかった！", m_name);
#else
				msg_format("%s is unaffected!", m_name);
#endif

				break;
			}
			else if (r_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
				msg_format("%sには耐性がある！", m_name);
#else
				msg_format("%s resists!", m_name);
#endif

				break;
			}
		}
#ifdef JP
msg_format("%sを引き戻した。", m_name);
#else
		msg_format("You command %s to return.", m_name);
#endif

		teleport_monster_to(cave[target_row][target_col].m_idx, py, px, 100, TELEPORT_PASSIVE);
		break;
	}
	case MS_TELE_AWAY:
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fire_beam(GF_AWAY_ALL, dir, 100);
		break;
	case MS_TELE_LEVEL:
	{
		int target_m_idx;
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];

		if (!target_set(TARGET_KILL)) return FALSE;
		target_m_idx = cave[target_row][target_col].m_idx;
		if (!target_m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		m_ptr = &m_list[target_m_idx];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%^sの足を指さした。", m_name);
#else
		msg_format("You gesture at %^s's feet.", m_name);
#endif

		if ((r_ptr->flagsr & (RFR_EFF_RES_NEXU_MASK | RFR_RES_TELE)) ||
			(r_ptr->flags1 & RF1_QUESTOR) || (r_ptr->level + randint1(50) > plev + randint1(60)))
		{
#ifdef JP
			msg_print("しかし効果がなかった！");
#else
			msg_format("%^s is unaffected!", m_name);
#endif
		}
		else teleport_level(target_m_idx);
		break;
	}
	case MS_PSY_SPEAR:
		if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
else msg_print("光の剣を放った。");
#else
			else msg_print("You throw a psycho-spear.");
#endif
		damage = randint1(plev * 3) + 100;
		(void)fire_beam(GF_PSY_SPEAR, dir, damage);
		break;
	case MS_DARKNESS:
#ifdef JP
msg_print("暗闇の中で手を振った。");
#else
			msg_print("You gesture in shadow.");
#endif
		(void)unlite_area(10, 3);
		break;
	case MS_MAKE_TRAP:
		if (!target_set(TARGET_KILL)) return FALSE;
#ifdef JP
msg_print("呪文を唱えて邪悪に微笑んだ。");
#else
			msg_print("You cast a spell and cackle evilly.");
#endif
		trap_creation(target_row, target_col);
		break;
	case MS_FORGET:
#ifdef JP
msg_print("しかし何も起きなかった。");
#else
			msg_print("Nothing happen.");
#endif
		break;
		/*
	case MS_RAISE_DEAD:
#ifdef JP
msg_print("死者復活の呪文を唱えた。");
#else
		msg_print("You cast a animate dead.");
#endif
		(void)animate_dead(0, py, px);
		break;
		*/
		case MS_S_KIN:
	{
		int k;
#ifdef JP
msg_print("援軍を召喚した。");
#else
			msg_print("You summon minions.");
#endif
		for (k = 0;k < 1; k++)
		{
			if (summon_kin_player(summon_lev, py, px, (pet ? PM_FORCE_PET : 0L)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚された仲間は怒っている！");
#else
msg_print("Summoned fellows are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		}
		break;
	}
	case MS_S_CYBER:
	{
		int k;
#ifdef JP
msg_print("サイバーデーモンを召喚した！");
#else
			msg_print("You summon a Cyberdemon!");
#endif
		for (k = 0 ;k < 1 ; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_CYBER, p_mode))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたサイバーデーモンは怒っている！");
#else
msg_print("The summoned Cyberdemon are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_MONSTER:
	{
		int k;
#ifdef JP
msg_print("仲間を召喚した。");
#else
			msg_print("You summon help.");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, 0, p_mode))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたモンスターは怒っている！");
#else
msg_print("The summoned monster is angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_MONSTERS:
	{
		int k;
#ifdef JP
msg_print("モンスターを召喚した！");
#else
			msg_print("You summon monsters!");
#endif
		for (k = 0;k < plev / 15 + 2; k++)
			if(summon_specific((pet ? -1 : 0), py, px, summon_lev, 0, (p_mode | u_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたモンスターは怒っている！");
#else
msg_print("Summoned monsters are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_ANT:
	{
		int k;
#ifdef JP
msg_print("アリを召喚した。");
#else
			msg_print("You summon ants.");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_ANT, (PM_ALLOW_GROUP | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたアリは怒っている！");
#else
msg_print("Summoned ants are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_SPIDER:
	{
		int k;
#ifdef JP
msg_print("蜘蛛を召喚した。");
#else
			msg_print("You summon spiders.");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_SPIDER, (PM_ALLOW_GROUP | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚された蜘蛛は怒っている！");
#else
msg_print("Summoned spiders are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_HOUND:
	{
		int k;
#ifdef JP
msg_print("ハウンドを召喚した。");
#else
			msg_print("You summon hounds.");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HOUND, (PM_ALLOW_GROUP | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたハウンドは怒っている！");
#else
msg_print("Summoned hounds are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_HYDRA:
	{
		int k;
#ifdef JP
msg_print("ヒドラを召喚した。");
#else
			msg_print("You summon a hydras.");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HYDRA, (g_mode | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたヒドラは怒っている！");
#else
msg_print("Summoned hydras are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_ANGEL:
	{
		int k;
#ifdef JP
msg_print("天使を召喚した！");
#else
			msg_print("You summon an angel!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_ANGEL, (g_mode | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚された天使は怒っている！");
#else
msg_print("Summoned angels are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_DEMON:
	{
		int k;
#ifdef JP
msg_print("混沌の宮廷から悪魔を召喚した！");
#else
			msg_print("You summon a demon from the Courts of Chaos!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_DEMON, (g_mode | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたデーモンは怒っている！");
#else
msg_print("Summoned demons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_UNDEAD:
	{
		int k;
#ifdef JP
msg_print("アンデッドの強敵を召喚した！");
#else
			msg_print("You summon an undead adversary!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_UNDEAD, (g_mode | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたアンデッドは怒っている！");
#else
msg_print("Summoned undeads are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_DRAGON:
	{
		int k;
#ifdef JP
msg_print("ドラゴンを召喚した！");
#else
			msg_print("You summon a dragon!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_DRAGON, (g_mode | p_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたドラゴンは怒っている！");
#else
msg_print("Summoned dragons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_HI_UNDEAD:
	{
		int k;
#ifdef JP
msg_print("強力なアンデッドを召喚した！");
#else
			msg_print("You summon a greater undead!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HI_UNDEAD, (g_mode | p_mode | u_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚された上級アンデッドは怒っている！");
#else
msg_print("Summoned greater undeads are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_HI_DRAGON:
	{
		int k;
#ifdef JP
msg_print("古代ドラゴンを召喚した！");
#else
			msg_print("You summon an ancient dragon!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HI_DRAGON, (g_mode | p_mode | u_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚された古代ドラゴンは怒っている！");
#else
msg_print("Summoned ancient dragons are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_AMBERITE:
	{
		int k;
#ifdef JP
msg_print("アンバーの王族を召喚した！");
#else
			msg_print("You summon a Lord of Amber!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_AMBERITES, (g_mode | p_mode | u_mode)))
			{
				if (!pet)
#ifdef JP
msg_print("召喚されたアンバーの王族は怒っている！");
#else
msg_print("Summoned Lords of Amber are angry!");
#endif
			}
			else
			{
				no_trump = TRUE;
			}
		break;
	}
	case MS_S_UNIQUE:
	{
		int k, count = 0;
#ifdef JP
msg_print("特別な強敵を召喚した！");
#else
			msg_print("You summon a special opponent!");
#endif
		for (k = 0;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_UNIQUE, (g_mode | p_mode | PM_ALLOW_UNIQUE)))
			{
				count++;
				if (!pet)
#ifdef JP
msg_print("召喚されたユニーク・モンスターは怒っている！");
#else
msg_print("Summoned special opponents are angry!");
#endif
			}
		for (k = count;k < 1; k++)
			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HI_UNDEAD, (g_mode | p_mode | PM_ALLOW_UNIQUE)))
			{
				count++;
				if (!pet)
#ifdef JP
msg_print("召喚された上級アンデッドは怒っている！");
#else
msg_print("Summoned greater undeads are angry!");
#endif
			}
		if (!count)
		{
			no_trump = TRUE;
		}
		break;
	}
	default:
		msg_print("hoge?");
	}
	if (no_trump)
	{
#ifdef JP
msg_print("何も現れなかった。");
#else
msg_print("No one have appeared.");
#endif
	}

	return TRUE;
}
#endif

#if 0
/*
 * do_cmd_cast calls this function if the player's class
 * is 'Blue-Mage'.
 */
bool do_cmd_cast_learned(void)
{
	int             n = 0;
	int             chance;
	int             minfail = 0;
	int             plev = p_ptr->lev;
	monster_power   spell;
	bool            cast;
	int             need_mana;


	/* not if confused */
	if (p_ptr->confused)
	{
#ifdef JP
msg_print("混乱していて唱えられない！");
#else
		msg_print("You are too confused!");
#endif

		return TRUE;
	}

	/* get power */
	if (!get_learned_power(&n)) return FALSE;

	spell = monster_powers[n];

	need_mana = mod_need_mana(spell.smana, 0, REALM_NONE);

	/* Verify "dangerous" spells */
	if (need_mana > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
msg_print("ＭＰが足りません。");
#else
		msg_print("You do not have enough mana to use this power.");
#endif


		if (!over_exert) return FALSE;

		/* Verify */
#ifdef JP
if (!get_check("それでも挑戦しますか? ")) return FALSE;
#else
		if (!get_check("Attempt it anyway? ")) return FALSE;
#endif

	}

	/* Spell failure chance */
	chance = spell.fail;

	/* Reduce failure rate by "effective" level adjustment */
	if (plev > spell.level) chance -= 3 * (plev - spell.level);
	else chance += (spell.level - plev);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[A_INT]] - 1);

	chance = mod_spell_chance_1(chance);

	/* Not enough mana to cast */
	if (need_mana > p_ptr->csp)
	{
		chance += 5 * (need_mana - p_ptr->csp);
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[A_INT]];

	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	chance = mod_spell_chance_2(chance);

	/* Failed spell */
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
#ifdef JP
msg_print("魔法をうまく唱えられなかった。");
#else
		msg_print("You failed to concentrate hard enough!");
#endif

		sound(SOUND_FAIL);

		if (n >= MS_S_KIN)
			/* Cast the spell */
			cast = cast_learned_spell(n, FALSE);
	}
	else
	{
		sound(SOUND_ZAP);

		/* Cast the spell */
		cast = cast_learned_spell(n, TRUE);

		if (!cast) return FALSE;
	}

	/* Sufficient mana */
	if (need_mana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= need_mana;
	}
	else
	{
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

		chg_virtue(V_KNOWLEDGE, -10);

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
	}

	/* Take a turn */
	energy_use = 100;

	/* Window stuff */
	p_ptr->redraw |= (PR_MANA);
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);

	return TRUE;
}
#endif

#if 0

/*本家青魔の学習処理*/
void learn_spell(int monspell)
{
	if (p_ptr->action != ACTION_LEARN) return;

	if (monspell < 0) return; /* Paranoia */
	if (p_ptr->magic_num2[monspell]) return;
	if (p_ptr->confused || p_ptr->blind || p_ptr->image || p_ptr->stun || p_ptr->paralyzed) return;
	if (randint1(p_ptr->lev + 70) > monster_powers[monspell].level + 40)
	{
		p_ptr->magic_num2[monspell] = 1;
#ifdef JP
		msg_format("%sを学習した！", monster_powers[monspell].name);
#else
		msg_format("You have learned %s!", monster_powers[monspell].name);
#endif
		gain_exp(monster_powers[monspell].level * monster_powers[monspell].smana);

		/* Sound */
		sound(SOUND_STUDY);

		new_mane = TRUE;
		p_ptr->redraw |= (PR_STATE);
	}
}
#endif

#if 0
/*
 * Extract monster spells mask for the given mode
 */
void set_rf_masks(s32b* f4, s32b* f5, s32b* f6, int mode)
{
	switch (mode)
	{
	case MONSPELL_TYPE_BOLT:
		*f4 = ((RF4_BOLT_MASK | RF4_BEAM_MASK) & ~(RF4_ROCKET));
		*f5 = RF5_BOLT_MASK | RF5_BEAM_MASK;
		*f6 = RF6_BOLT_MASK | RF6_BEAM_MASK;
		break;

	case MONSPELL_TYPE_BALL:
		*f4 = (RF4_BALL_MASK & ~(RF4_BREATH_MASK));
		*f5 = (RF5_BALL_MASK & ~(RF5_BREATH_MASK));
		*f6 = (RF6_BALL_MASK & ~(RF6_BREATH_MASK));
		break;

	case MONSPELL_TYPE_BREATH:
		*f4 = RF4_BREATH_MASK;
		*f5 = RF5_BREATH_MASK;
		*f6 = RF6_BREATH_MASK;
		break;

	case MONSPELL_TYPE_SUMMON:
		*f4 = RF4_SUMMON_MASK;
		*f5 = RF5_SUMMON_MASK;
		*f6 = RF6_SUMMON_MASK;
		break;

	case MONSPELL_TYPE_OTHER:
		*f4 = RF4_ATTACK_MASK & ~(RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK | RF4_INDIRECT_MASK);
		*f5 = RF5_ATTACK_MASK & ~(RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_INDIRECT_MASK);
		*f6 = RF6_ATTACK_MASK & ~(RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK | RF6_INDIRECT_MASK);
		break;
	}

	return;
}

#endif


//モンスター魔法の分類を選択する。今のところ隠岐奈専用
//v1.1.82 新職業「弾幕研究家」も使う
static int choose_monspell_kind(void)
{

	int choose_kind = 0;
	char choice;
	int menu_line = 1;


	if (repeat_pull(&choose_kind)) return choose_kind;
	screen_save();
	while (!choose_kind)
	{
		//このmenu_lineの行とNEW_MSPELL_TYPE_***が一致していること。
		//列挙型とか使えばもっとスマートに書けるらしいがよく知らない:(
#ifdef JP
		prt(format(" %s a) ボルト/ビーム/ロケット", (menu_line == 1) ? "》" : "  "), 2, 14);
		prt(format(" %s b) ボール", (menu_line == 2) ? "》" : "  "), 3, 14);
		prt(format(" %s c) ブレス", (menu_line == 3) ? "》" : "  "), 4, 14);
		prt(format(" %s d) その他の攻撃", (menu_line == 4) ? "》" : "  "), 5, 14);
		prt(format(" %s e) 召喚", (menu_line == 5) ? "》" : "  "), 6, 14);
		prt(format(" %s f) その他", (menu_line == 6) ? "》" : "  "), 7, 14);
#else
        prt(format(" %s a) Bolt/Beam/Rocket", (menu_line == 1) ? ">" : "  "), 2, 14);
		prt(format(" %s b) Ball", (menu_line == 2) ? ">" : "  "), 3, 14);
		prt(format(" %s c) Breath", (menu_line == 3) ? ">" : "  "), 4, 14);
		prt(format(" %s d) Other attacks", (menu_line == 4) ? ">" : "  "), 5, 14);
		prt(format(" %s e) Summoning", (menu_line == 5) ? ">" : "  "), 6, 14);
		prt(format(" %s f) Other", (menu_line == 6) ? ">" : "  "), 7, 14);
#endif
		prt(_("どの種類の能力を使いますか？", "Use which kind of ability?"), 0, 0);

		choice = inkey();
		switch (choice)
		{
		case ESCAPE:
			screen_load();
			return 0;
		case '2':
		case 'j':
		case 'J':
			menu_line++;
			break;
		case '8':
		case 'k':
		case 'K':
			menu_line += 5;
			break;
		case '\r':
		case 'x':
		case 'X':
			choose_kind = menu_line;
			break;
		case 'a':
		case 'A':
			choose_kind = NEW_MSPELL_TYPE_BOLT_BEAM_ROCKET;
			break;
		case 'b':
		case 'B':
			choose_kind = NEW_MSPELL_TYPE_BALL;
			break;
		case 'c':
		case 'C':
			choose_kind = NEW_MSPELL_TYPE_BREATH;
			break;
		case 'd':
		case 'D':
			choose_kind = NEW_MSPELL_TYPE_OTHER_ATTACK;
			break;
		case 'e':
		case 'E':
			choose_kind = NEW_MSPELL_TYPE_SUMMMON;
			break;
		case 'f':
		case 'F':
			choose_kind = NEW_MSPELL_TYPE_OTHER;
			break;

		}
		if (menu_line > 6) menu_line -= 6;
	}
	screen_load();
	repeat_push(choose_kind);

	return choose_kind;

}

//モンスター魔法を使ったときのメッセージ　クラスのほか攻撃魔法かそうでないかなどで分岐
static void msg_cast_monspell_new(int num, cptr msg, int xtra)
{
	if (xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO) //アイテムカードのヤマビコ用
#ifdef JP
		msg_format("あなたは「%s」を%s",monspell_list2[num].name,msg);
#else
        msg_format("You %s '%s'!",msg, monspell_list2[num].name);
#endif
	else if (p_ptr->pclass == CLASS_YUKARI && monspell_list2[num].attack_spell)
		msg_format(_("スキマから%sが飛び出した！", "%s flies out of your gap!"),monspell_list2[num].name,msg);
	else if (p_ptr->pclass == CLASS_YORIHIME)
	{
		cptr mname;
		int r_idx = p_ptr->magic_num1[20];

		//v1.1.30 ランダムユニークの名前処理追加
		if (IS_RANDOM_UNIQUE_IDX(r_idx))
			mname = random_unique_name[r_idx - MON_RANDOM_UNIQUE_1];
		else
			mname = r_name + r_info[r_idx].name;

		if(monspell_list2[num].attack_spell)
			msg_format(_("「%sよ、%sを放て！」", "'%s, bring forth %s!'"),mname,monspell_list2[num].name);
		else
			msg_format(_("「%sよ、%sをもたらせ！」", "'%s, deliver %s!'"),mname,monspell_list2[num].name);
	}
	else if(p_ptr->pclass == CLASS_OKINA)
	{
#ifdef JP
		msg_format("あなたは「裏・%s」を%s", monspell_list2[num].name, msg);
#else
        msg_format("You %s 'Hidden - %s'!", msg, monspell_list2[num].name);
#endif
	}
	else
	{
#ifdef JP
		msg_format("あなたは「%s」を%s", monspell_list2[num].name, msg);
#else
        msg_format("You %s '%s'!", msg, monspell_list2[num].name);
#endif
	}

}

/*:::新しいモンスター魔法使用ルーチン 既に成功判定やMP処理は済んでいること*/
//戻り値：only_infoのとき威力などの簡易説明文 行動消費時"" 行動非消費時NULL
//num：monspell_list2[]の添え字
//fail：失敗時TRUE 召喚失敗処理など
//xtra : 特殊な動作でこの関数を使う場合に設定
//mane_dam: ものまねのときのダメージ
cptr cast_monspell_new_aux(int num, bool only_info, bool fail, int xtra, int mane_dam)
{
	int cast_lev;
	int cast_hp;
	int plev = p_ptr->lev;
	bool powerful = FALSE;
	cptr msg;
	int dir;
	int dam, base, dice, sides;
	bool monomane = FALSE;

	if (xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = _("オウム返しした！", "reverberate");
	}
	else if (xtra == CAST_MONSPELL_EXTRA_MONOMANE)
	{
		monomane = TRUE;
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = _("ものまねした！", "imitate");
	}

	else if (p_ptr->pclass == CLASS_SATORI)
	{
		cast_lev = p_ptr->lev * 3 / 2;
		cast_hp = p_ptr->chp;
		if (p_ptr->lev > 44) powerful = TRUE;
		msg = _("想起した！", "recollect");
	}
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		monster_type* m_ptr = &m_list[p_ptr->riding];
		monster_race* r_ptr = &r_info[m_ptr->r_idx];

		cast_lev = r_ptr->level;
		cast_hp = m_ptr->hp;
		if (r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = _("使わせた！", "use");
	}
	else if (p_ptr->pclass == CLASS_KYOUKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = _("オウム返しした！", "reverberate");
	}
	else if (p_ptr->pclass == CLASS_KOKORO)
	{
		object_type* o_ptr = &inventory[INVEN_HEAD];

		if (o_ptr->tval != TV_MASK)
		{
			msg_print(_("ERROR:面を装備していない状態でcast_monspell_new_aux()が呼ばれている",
                        "ERROR: cast_monspell_new_aux() called while not wearing a mask"));
			return NULL;
		}

		cast_lev = o_ptr->discount;
		cast_hp = MAX(10, (o_ptr->xtra5 / 6));
		if (have_flag(o_ptr->art_flags, TR_SPECIAL_KOKORO)) powerful = TRUE;
		msg = _("再演した！", "reproduce");
	}
	else if (p_ptr->pclass == CLASS_RAIKO)
	{
		//xtra値に判定レベルが入る。POWERFULのとき+100されている。
		if (xtra > 99) powerful = TRUE;
		cast_lev = xtra % 100;
		cast_hp = p_ptr->chp;
		msg = _("発動した！", "activate");
	}
	else if (p_ptr->pclass == CLASS_YUKARI)
	{
		if (p_ptr->lev > 44) powerful = TRUE;
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp * 2;
		msg = _("行使した！", "use"); //攻撃魔法のときメッセージ例外処理あり
	}
	else if (IS_METAMORPHOSIS)
	{
		monster_race* r_ptr = &r_info[MON_EXTRA_FIELD];
		cast_lev = r_ptr->level;
		cast_hp = p_ptr->chp;
		if (r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = _("使った！", "use");

	}
	else if (p_ptr->pclass == CLASS_YORIHIME)
	{
		int r_idx = p_ptr->magic_num1[20];
		int mon_hp;
		monster_race* r_ptr;

		if (!r_idx)
		{
			msg_print(_("ERROR:依姫cast_monspell_new_auxでr_idxが設定されていない",
                        "ERROR: r_idx not set during Yorihime's cast_monspell_aux"));
			if (only_info) return format("");
			return NULL;
		}
		r_ptr = &r_info[r_idx];
		if (r_ptr->flags1 & RF1_FORCE_MAXHP)
			cast_hp = r_ptr->hdice * r_ptr->hside;
		else
			cast_hp = r_ptr->hdice * (r_ptr->hside + 1) / 2;

		cast_lev = r_ptr->level;
		if (r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = _("使用した！", "use");//例外処理で上書き
	}
	else if (p_ptr->pclass == CLASS_OKINA)
	{
		cast_lev = p_ptr->lev * 2;
		cast_hp = p_ptr->chp;
		if (p_ptr->lev > 44) powerful = TRUE;
		msg = _("行使した！", "use");
	}
	else if (p_ptr->pclass == CLASS_YUMA)
	{
		cast_lev = p_ptr->lev * 2;
		cast_hp = p_ptr->chp * 3 / 2;
		msg = _("再現した！", "reproduce");
	}
	else if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		cast_lev = p_ptr->lev * 2;
		cast_hp = p_ptr->chp;
		if (p_ptr->concent)
		{
			if (p_ptr->magic_num1[0] == CONCENT_KANA)
				cast_hp = cast_hp * 2;
			else
				powerful = TRUE;
		}

		msg = _("再現した！", "reproduce");

	}
	else
	{
		msg_print(_("ERROR:このクラスでの敵魔法実行パラメータが定義されていない",
                    "ERROR: Enemy magic usage parameter not set for this class"));
		if (only_info) return format("");
		return NULL;
	}



	switch (num)
	{

	case 1://RF4_SHRIEK
	case 128: //RF9_ALARM
	{
		if (only_info) return format("");
		if (fail) return "";

		//msg_format("あなたは「%s」を%s",monspell_list2[num].name,msg);
		msg_cast_monspell_new(num, msg, xtra);

		if (num == 1) aggravate_monsters(0, FALSE);
		else aggravate_monsters(0, TRUE);
	}
	break;
	case 2://RF4_DANMAKU
	{
		dam = cast_lev;
		if (monomane) dam = mane_dam;
		if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_bolt(GF_MISSILE, dir, dam);
	}
	break;
	case 3: //RF4_DISPEL
	{
		int m_idx;

		if (only_info) return format("");
		if (fail) return "";

		if (!target_set(TARGET_KILL)) return NULL;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;

		msg_cast_monspell_new(num, msg, xtra);
		dispel_monster_status(m_idx);
	}
	break;

	case 4: //RF4_ROCKET
	{
		dam = cast_hp / 4;
		if (dam > 800) dam = 800;
		if (monomane) dam = mane_dam;
		if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
		if (fail) return "";
		if (!get_aim_dir(&dir)) return NULL;

		msg_cast_monspell_new(num, msg, xtra);
		fire_rocket(GF_ROCKET, dir, dam, 2);
	}
	break;
	case 5://RF4_SHOOT
	{
		if (IS_METAMORPHOSIS)
		{
			dice = r_info[MON_EXTRA_FIELD].blow[0].d_dice;
			sides = r_info[MON_EXTRA_FIELD].blow[0].d_side;
			if(only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);

			dam = damroll(dice, sides);
		}
		else
		{
			dam = cast_lev * 3 / 2;
			if (monomane) dam = mane_dam;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);

		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_bolt(GF_MISSILE, dir, dam);
	}
	break;
	case 6: //RF4_BR_HOLY
	case 7: //RF4_BR_HELL
	{
		int typ;
		if (num == 6) typ = GF_HOLY_FIRE;
		if (num == 7) typ = GF_HELL_FIRE;
		dam = cast_hp / 6;
		if (dam > 500) dam = 500;
		if (monomane) dam = mane_dam;
		if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 8: //RF4_BR_AQUA
	{
		dam = cast_hp / 8;
		if (dam > 350) dam = 350;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_WATER, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 9: //RF4_BR_ACID
	case 10: //RF4_BR_ELEC
	case 11: //RF4_BR_FIRE
	case 12: //RF4_BR_COLD
	{
		int typ;
		if (num == 9) typ = GF_ACID;
		if (num == 10) typ = GF_ELEC;
		if (num == 11) typ = GF_FIRE;
		if (num == 12) typ = GF_COLD;

		dam = cast_hp / 3;
		if (dam > 1600) dam = 1600;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 13: //RF4_BR_POIS
	{
		dam = cast_hp / 3;
		if (dam > 800) dam = 800;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_POIS, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 14: //RF4_BR_NETH
	{
		int typ;
		if (num == 14) typ = GF_NETHER;

		dam = cast_hp / 6;
		if (dam > 550) dam = 550;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;


	case 15: //RF4_BR_LITE
	case 16: //RF4_BR_DARK
	{
		int typ;
		if (num == 15) typ = GF_LITE;
		if (num == 16) typ = GF_DARK;

		dam = cast_hp / 6;
		if (dam > 400) dam = 400;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;

	case 17: //RF4_WAVEMOTION
	{
		dam = cast_hp / 6;
		if (dam > 555) dam = 555;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_spark(GF_DISINTEGRATE, dir, dam, 2);
	}
	break;
	case 18: //RF4_BR_SOUN
	{
		int typ;
		if (num == 18) typ = GF_SOUND;

		dam = cast_hp / 6;
		if (dam > 450) dam = 450;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 19: //RF4_BR_CHAO
	{
		int typ;
		if (num == 19) typ = GF_CHAOS;

		dam = cast_hp / 6;
		if (dam > 600) dam = 600;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 20: //RF4_BR_DISE
	case 25: //RF4_BR_SHAR
	{
		int typ;
		if (num == 20) typ = GF_DISENCHANT;
		if (num == 25) typ = GF_SHARDS;

		dam = cast_hp / 6;
		if (dam > 500) dam = 500;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 21: //RF4_BR_NEXU
	{
		int typ;
		if (num == 21) typ = GF_NEXUS;

		dam = cast_hp / 7;
		if (dam > 600) dam = 600;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 22: //RF4_BR_TIME
	{
		int typ;
		if (num == 22) typ = GF_TIME;

		dam = cast_hp / 3;
		if (dam > 150) dam = 150;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 23: //RF4_BR_INACT
	{
		int typ;
		if (num == 23) typ = GF_INACT;

		dam = cast_hp / 6;
		if (dam > 200) dam = 200;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 24: //RF4_BR_GRAV
	{
		int typ;
		if (num == 24) typ = GF_GRAVITY;

		dam = cast_hp / 3;
		if (dam > 200) dam = 200;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 26: //RF4_BR_PLAS
	{
		int typ;
		if (num == 26) typ = GF_PLASMA;

		dam = cast_hp / 4;
		if (dam > 1200) dam = 1200;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 27: //RF4_BA_FORCE
	{
		if (powerful)
		{
			dam = cast_hp / 4;
			if (dam > 350) dam = 350;
		}
		else
		{
			dam = cast_hp / 6;
			if (dam > 250) dam = 250;
		}

		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_FORCE, dir, dam, 2);
	}
	break;
	case 28: //RF4_BR_MANA
	{
		int typ;
		if (num == 28) typ = GF_MANA;

		dam = cast_hp / 3;
		if (dam > 250) dam = 250;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 29: //RF4_SPECIAL2
	{

		msg_format(_("ERROR:この技は呼ばれないはず：RF4_SPECIAL2",
                        "ERROR: This ability shouldn't be called: RF4_SPECIAL2"));
		return NULL;
	}
	break;

	case 30: //RF4_BR_NUKE
	{
		int typ;
		if (num == 30) typ = GF_NUKE;

		dam = cast_hp / 4;
		if (dam > 1000) dam = 1000;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 31: //RF4_BA_CHAO
	{
		if (powerful)
		{
			base = cast_lev * 3;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = cast_lev * 2;
			dice = 10;
			sides = 10;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_CHAOS, dir, dam, 4);
	}
	break;
	case 32: //RF4_BR_DISI
	{
		int typ = GF_DISINTEGRATE;

		dam = cast_hp / 6;
		if (dam > 150) dam = 150;
		if (monomane) dam = mane_dam;
		if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
	}
	break;
	case 33: //RF5_BA_ACID
	{
		int rad;
		if (powerful)
		{
			rad = 4;
			base = cast_lev * 4 + 50;
			dice = 10;
			sides = 10;
		}
		else
		{
			rad = 2;
			base = 15;
			dice = 1;
			sides = cast_lev * 3;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_ACID, dir, dam, 4);
	}
	break;

	case 34: //RF5_BA_ELEC
	{
		int rad;
		if (powerful)
		{
			rad = 4;
			base = cast_lev * 4 + 50;
			dice = 10;
			sides = 10;
		}
		else
		{
			rad = 2;
			base = 8;
			dice = 1;
			sides = cast_lev * 3 / 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_ELEC, dir, dam, 4);
	}
	break;

	case 35: //RF5_BA_FIRE
	{
		int rad;
		if (powerful)
		{
			rad = 4;
			base = cast_lev * 4 + 50;
			dice = 10;
			sides = 10;
		}
		else
		{
			rad = 2;
			base = 10;
			dice = 1;
			sides = cast_lev * 7 / 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_FIRE, dir, dam, 4);
	}
	break;
	case 36: //RF5_BA_COLD
	{
		int rad;
		if (powerful)
		{
			rad = 4;
			base = cast_lev * 4 + 50;
			dice = 10;
			sides = 10;
		}
		else
		{
			rad = 2;
			base = 10;
			dice = 1;
			sides = cast_lev * 3 / 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_COLD, dir, dam, 4);
	}
	break;
	case 37: //RF5_BA_POIS
	{
		int rad = 2;
		if (powerful)
		{
			dice = 24;
			sides = 2;
		}
		else
		{
			dice = 12;
			sides = 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = damroll(dice, sides);
			if (only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_POIS, dir, dam, 4);
	}
	break;
	case 38: //RF5_BA_NETH
	{
		int rad = 2;
		if (powerful)
		{
			base = 50 + cast_lev * 2;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = 50 + cast_lev;
			dice = 10;
			sides = 10;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_NETHER, dir, dam, 4);
	}
	break;
	case 39: //RF5_BA_WATE
	{
		int rad = 4;
		if (powerful)
		{
			base = 50;
			dice = 1;
			sides = cast_lev * 3;
		}
		else
		{
			base = 50;
			dice = 1;
			sides = cast_lev * 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_WATER, dir, dam, 4);
	}
	break;

	case 40: //RF5_BA_MANA
	case 41: //RF5_BA_DARK
	case 53: //RF5_BA_LITE
	{
		int typ;
		int rad = 4;

		base = 50 + cast_lev * 4;
		dice = 10;
		sides = 10;

		if (num == 40) typ = GF_MANA;
		else if (num == 41) typ = GF_DARK;
		else if (num == 53) typ = GF_LITE;
		else { msg_print("ERROR:typ"); return NULL; }

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, rad);
	}
	break;
	case 42: //RF5_DRAIN_MANA
	{
		//v1.1.82 弾幕研究家実装時、試したら強すぎたんでnerf
		//・威力半減
		//・GF_DRAIN_MANAからGF_PSI_DRAINに
		//PSI_DRAINの抵抗処理も色々整頓

		base = MAX(1, cast_lev / 2);
		dice = 1;
		sides = MAX(2, cast_lev / 2);

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);

		fire_ball_hide(GF_PSI_DRAIN, dir, dam, 0);

	}
	break;
	case 43: //RF5_MIND_BLAST
	case 44: //RF5_BRAIN_SMASH
	case 45: //RF5_CAUSE_1
	case 46: //RF5_CAUSE_2
	case 47: //RF5_CAUSE_3
	case 48: //RF5_CAUSE_4
	case 107: //RF9_PUNISH_1
	case 108: //RF9_PUNISH_2
	case 109: //RF9_PUNISH_3
	case 110: //RF9_PUNISH_4
	{
		int typ;
		if (num == 43)
		{
			dice = 7;
			sides = 7;
			typ = GF_MIND_BLAST;
		}
		else if (num == 44)
		{
			dice = 12;
			sides = 12;
			typ = GF_BRAIN_SMASH;
		}
		else if (num == 45)
		{
			dice = 3;
			sides = 8;
			typ = GF_CAUSE_1;
		}
		else if (num == 46)
		{
			dice = 8;
			sides = 8;
			typ = GF_CAUSE_2;
		}
		else if (num == 47)
		{
			dice = 10;
			sides = 15;
			typ = GF_CAUSE_3;
		}
		else if (num == 48)
		{
			dice = 15;
			sides = 15;
			typ = GF_CAUSE_4;
		}
		else if (num == 107)
		{
			dice = 3;
			sides = 8;
			typ = GF_PUNISH_1;
		}
		else if (num == 108)
		{
			dice = 8;
			sides = 8;
			typ = GF_PUNISH_2;
		}
		else if (num == 109)
		{
			dice = 10;
			sides = 15;
			typ = GF_PUNISH_3;
		}
		else if (num == 110)
		{
			dice = 15;
			sides = 15;
			typ = GF_PUNISH_4;
		}
		else { msg_print("ERROR:typ"); return NULL; }

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = damroll(dice, sides);
			if(only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);

		fire_ball_hide(typ, dir, dam, 0);
	}
	break;
	case 49: //RF5_BO_ACID
	case 50: //RF5_BO_ELEC
	case 51: //RF5_BO_FIRE
	case 52: //RF5_BO_COLD
	case 54: //RF5_BO_NETH
	case 55: //RF5_BO_WATE
	case 56: //RF5_BO_MANA
	case 57: //RF5_BO_PLAS
	case 58: //RF5_BO_ICEE
	case 59: //RF5_MISSILE
	case 111: //RF9_BO_HOLY
	case 113: //RF9_BO_SOUND
	{
		int typ;

		if (num == 49)
		{
			dice = 7;
			sides = 8;
			base = cast_lev / 3;
			if (powerful) base *= 2;
			typ = GF_ACID;
		}
		else if (num == 50)
		{
			dice = 4;
			sides = 8;
			base = cast_lev / 3;
			if (powerful) base *= 2;
			typ = GF_ELEC;
		}
		else if (num == 51)
		{
			dice = 9;
			sides = 8;
			base = cast_lev / 3;
			if (powerful) base *= 2;
			typ = GF_FIRE;
		}
		else if (num == 52)
		{
			dice = 6;
			sides = 8;
			base = cast_lev / 3;
			if (powerful) base *= 2;
			typ = GF_COLD;
		}
		else if (num == 54)
		{
			dice = 5;
			sides = 5;
			base = cast_lev * 4;
			if (powerful) base /= 2;
			else base /= 3;
			typ = GF_NETHER;
		}
		else if (num == 55)
		{
			dice = 10;
			sides = 10;
			base = cast_lev * 3;
			if (powerful) base /= 2;
			else base /= 3;
			typ = GF_WATER;
		}
		else if (num == 56)
		{
			dice = 1;
			sides = cast_lev * 7 / 2;
			base = 50;
			typ = GF_MANA;
		}
		else if (num == 57)
		{
			dice = 1;
			sides = 200;
			if (powerful) base = 100 + cast_lev * 6;
			else  base = 100 + cast_lev * 3;
			typ = GF_PLASMA;
		}
		else if (num == 58)
		{
			dice = 6;
			sides = 6;
			base = cast_lev * 3;
			if (powerful) base /= 2;
			else  base /= 3;
			typ = GF_ICE;
		}
		else if (num == 59)
		{
			dice = 2;
			sides = 6;
			base = cast_lev / 3 + 1;
			typ = GF_MISSILE;
		}
		else if (num == 111)
		{
			dice = 10;
			sides = 10;
			if (powerful) base = cast_lev * 3 / 2;
			else base = cast_lev;
			typ = GF_HOLY_FIRE;
		}
		else if (num == 113)
		{
			dice = 3;
			if (powerful) dice = 10;
			sides = 10;
			if (powerful) base = cast_lev * 3 / 2;
			else base = cast_lev;
			typ = GF_SOUND;
		}
		else { msg_print("ERROR:typ"); return NULL; }

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_bolt(typ, dir, dam);
	}
	break;

	case 60: //RF5_SCARE
	case 61: //RF5_BLIND
	case 62: //RF5_CONF
	case 63: //RF5_SLOW
	case 64: //RF5_HOLD
	case 79: //RF6_FORGET
	{
		int power;

		if (num == 60 || num == 63 || num == 64) power = cast_lev;
		else power = cast_lev * 2;

		if (monomane) power = mane_dam;

		if(only_info) return format(_("効力：%d", "pow: %d"),power);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);

		if (num == 60) fear_monster(dir, power);
		else if (num == 61) confuse_monster(dir, power);
		else if (num == 62) confuse_monster(dir, power);
		else if (num == 63) slow_monster(dir, power);
		else if (num == 64) sleep_monster(dir, power);
		else fire_ball_hide(GF_GENOCIDE, dir, power, 0);

	}
	break;
	case 65: //RF6_HASTE
	{
		int time = 100;

		if(only_info) return format(_("期間：%d", "dur: %d"),time);
		if (fail) return "";
		msg_cast_monspell_new(num, msg, xtra);

		(void)set_fast(time, FALSE);
	}
	break;
	case 66: //RF6_HAND_DOOM
	{
		int power = cast_lev * 3;

		if(only_info) return format(_("効力：%d", "pow: %d"),power);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball_hide(GF_HAND_DOOM, dir, power, 0);


	}
	break;
	case 67: //RF6_HEAL
	{
		int power = cast_lev * 6;

		if(only_info) return format(_("回復：%d", "heal: %d"),power);
		if (fail) return "";

		msg_cast_monspell_new(num, msg, xtra);
		(void)hp_player(power);
		(void)set_stun(0);
		(void)set_cut(0);
	}
	break;
	case 68: //RF6_INVULNER
	{
		base = 4;

		if(only_info) return format(_("期間：%d+1d%d", "dur: %d+1d%d"),base,base);
		if (fail) return "";
		msg_cast_monspell_new(num, msg, xtra);

		(void)set_invuln(randint1(base) + base, FALSE);
	}
	break;
	case 69: //RF6_BLINK
	case 70: //RF6_TPORT
	{
		int dist;

		if (num == 69) dist = 10;
		else dist = MAX_SIGHT * 2 + 5;

		if(only_info) return format(_("距離：%d", "dist: %d"),dist);
		if (fail) return "";
		msg_cast_monspell_new(num, msg, xtra);
		teleport_player(dist, 0L);

	}
	break;
	case 71: //RF6_WORLD
	{

		if (only_info) return format(" ");
		if (fail) return "";
		if (world_player)
		{
			msg_print(_("既に時は止まっている。", "Time is already stopped."));
			return NULL;
		}
		else
		{

			msg_cast_monspell_new(num, msg, xtra);
			//v1.1.58
			flag_update_floor_music = TRUE;
			world_player = TRUE;
			/* Hack */
			p_ptr->energy_need -= 1000 + (400) * TURNS_PER_TICK / 10;
			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);
			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();

		}
	}
	break;
	case 72: //RF6_SPECIAL
	{

		msg_format(_("ERROR:この技は呼ばれないはず：RF6_SPECIAL",
                        "ERROR: This ability should not have been used: RF6_SPECIAL"));
		return NULL;
	}
	break;
	case 73: //RF6_TELE_TO
	{
		int idx_dummy;
		if (only_info) return format(" ");
		if (fail) return "";
		msg_cast_monspell_new(num, msg, xtra);
		if (!teleport_back(&idx_dummy, 0L)) return NULL;

	}
	break;
	case 74: //RF6_TELE_AWAY
	{
		int dist = 100;
		if(only_info) return format(_("距離：%d", "dist: %d"),dist);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return FALSE;
		msg_cast_monspell_new(num, msg, xtra);
		(void)fire_beam(GF_AWAY_ALL, dir, dist);


	}
	break;
	case 75: //RF6_TELE_LEVEL?
	{

		if(only_info) return format(_("未実装", "unimplemented"));
		if (fail) return "";
		msg_format(_("ERROR:この技は未実装のはず：RF6_TELE_LEVEL",
                        "ERROR: This ability is unimplemented: RF6_TELE_LEVEL"));
		return NULL;
	}
	break;
	case 76: //RF5_PSY_SPEAR
	{
		if (powerful)
		{
			base = 150;
			dice = 1;
			sides = cast_lev * 2;
		}
		else
		{
			base = 100;
			dice = 1;
			sides = cast_lev * 3 / 2;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		(void)fire_beam(GF_PSY_SPEAR, dir, dam);
	}
	break;
	case 77: //RF6_DARKNESS　←ライト・エリアにする
	{
		if(only_info) return format(_("効果：照明", "eff: illuminate"));
		if (fail) return "";
		msg_cast_monspell_new(num, msg, xtra);
		(void)lite_area(0, 3);
	}
	break;
	case 78: //RF6_TRAP
	{
		if (only_info) return format("");
		if (fail) return "";
		if (!target_set(TARGET_KILL)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		trap_creation(target_row, target_col);
	}
	break;
	case 80: //RF6_COSMIC_HORROR
	{

		base = 50 + cast_lev * 4;
		dice = 10;
		sides = 10;

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball_hide(GF_COSMIC_HORROR, dir, dam, 0);
	}
	break;
	case 81: //RF6_S_KIN
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:
	case 89:
	case 90: //RF6_S_DEMON
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
	case 96:
	case 114: //RF9_S_ANIMAL
	case 122: //RF9_S_DEITY
	case 123: //RF9_S_HI_DEMON
	case 124: //RF9_S_KWAI
	{
		int mode;
		bool summon = FALSE;
		int cnt;
		if (only_info) return format("");

		if (fail) mode = (PM_NO_PET | PM_ALLOW_GROUP);
		else mode = PM_FORCE_PET;

		if (!fail) msg_cast_monspell_new(num, msg, xtra);

		if (num == 81) summon = summon_kin_player(cast_lev, py, px, mode);
		if (num == 82) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_CYBER, mode);
		if (num == 83) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode);
		if (num == 84) for (cnt = 0; cnt < 6; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode)) summon = TRUE;
		if (num == 85) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANT, (mode | PM_ALLOW_GROUP));
		if (num == 86)  for (cnt = 0; cnt < 6; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_SPIDER, mode | PM_ALLOW_GROUP)) summon = TRUE;
		if (num == 87) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HOUND, (mode | PM_ALLOW_GROUP));
		if (num == 88) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HYDRA, (mode));
		if (num == 89) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANGEL, (mode));
		if (num == 90) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEMON, (mode));
		if (num == 91) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNDEAD, (mode));
		if (num == 92) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DRAGON, (mode));
		if (num == 93) for (cnt = 0; cnt < 6; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_UNDEAD, mode)) summon = TRUE;
		if (num == 94) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DRAGON, mode)) summon = TRUE;
		if (num == 95) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE))) summon = TRUE;
		if (num == 96) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE | PM_ALLOW_GROUP))) summon = TRUE;
		if (num == 114) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANIMAL, (mode | PM_ALLOW_GROUP))) summon = TRUE;
		if (num == 122) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEITY, (mode | PM_ALLOW_GROUP))) summon = TRUE;
		if (num == 123) for (cnt = 0; cnt < 4; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DEMON, (mode | PM_ALLOW_GROUP))) summon = TRUE;
		if (num == 124) for (cnt = 0; cnt < 6; cnt++) if (summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_KWAI, (mode | PM_ALLOW_GROUP))) summon = TRUE;



		if(summon && fail) msg_format(_("召喚したモンスターが襲いかかってきた！",
										"The summoned monsters turn on you!"));
		else if(!summon) msg_format(_("しかし何も現れなかった。", "However, nobody appeared."));

	}
	break;
	case 97: //RF9_FIRE_STORM
	case 98: //RF9_ICE_STORM
	case 99: //RF9_THUNDER_STORM
	case 100: //RF9_ACID_STORM

	{
		int rad = 4;
		int typ;

		if (num == 97) typ = GF_FIRE;
		if (num == 98) typ = GF_COLD;
		if (num == 99) typ = GF_ELEC;
		if (num == 100) typ = GF_ACID;

		if (powerful)
		{
			base = 100 + cast_lev * 10;
			dice = 1;
			sides = 100;
		}
		else
		{
			base = 50 + cast_lev * 7;
			dice = 1;
			sides = 50;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, rad);
	}
	break;
	case 101: //RF9_TOXIC_STORM
	{
		int rad = 4;
		int typ;

		if (powerful)
		{
			base = 50 + cast_lev * 8;
			dice = 1;
			sides = 50;
		}
		else
		{
			base = 25 + cast_lev * 5;
			dice = 1;
			sides = 25;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_POIS, dir, dam, rad);
	}
	break;
	case 102: //RF9_BA_POLLUTE
	{
		int rad = 4;

		if (powerful)
		{
			base = 50 + cast_lev * 4;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = 50 + cast_lev * 3;
			dice = 1;
			sides = 50;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_POLLUTE, dir, dam, rad);
	}
	break;
	case 103: //RF9_BA_DISI
	{
		int rad = 5;

		if (powerful)
		{
			base = 50 + cast_lev * 2;
			dice = 1;
			sides = 50;
		}
		else
		{
			base = 25 + cast_lev;
			dice = 1;
			sides = 25;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_DISINTEGRATE, dir, dam, rad);
	}
	break;
	case 104: //RF9_BA_HOLY
	{
		int rad = 2;

		if (powerful)
		{
			base = 50 + cast_lev * 2;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = 50 + cast_lev;
			dice = 10;
			sides = 10;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(GF_HOLY_FIRE, dir, dam, rad);
	}
	break;
	case 105: //RF9_METEOR
	{
		int rad;

		if (powerful)
		{
			//v2.0 少し弱体化
			rad = 5;
			base = cast_lev * 3;
			dice = 1;
			sides = 250;
		}
		else
		{
			rad = 4;
			base = cast_lev * 2;
			dice = 1;
			sides = 150;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball_jump(GF_METEOR, dir, dam, rad, NULL);
	}
	break;
	case 106: //RF9_DISTORTION
	{
		int rad = 4;

		if (powerful)
		{
			base = 50 + cast_lev * 3;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = 50 + cast_lev * 3 / 2;
			dice = 10;
			sides = 10;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball_jump(GF_DISTORTION, dir, dam, rad, NULL);
	}
	break;
	case 112: //RF9_GIGANTIC_LASER
	{
		int rad;

		if (powerful)
		{
			rad = 2;
			base = cast_lev * 8;
			dice = 10;
			sides = 10;
		}
		else
		{
			rad = 1;
			base = cast_lev * 4;
			dice = 10;
			sides = 10;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_spark(GF_NUKE, dir, dam, rad);
	}
	break;

	case 115: //RF9_BEAM_LITE
	{
		if (powerful)
			base = cast_lev * 4;
		else
			base = cast_lev * 2;

		dam = base;
		if (monomane) dam = mane_dam;

		if(only_info) return format(_("損傷：%d", "dam: %d"),base);
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		(void)fire_beam(GF_LITE, dir, dam);
	}
	break;


	case 116:
	{

		if(only_info) return format(_("未実装", "unimplemented"));
		if(fail) return "";
		msg_format(_("ERROR:この技は未実装のはず：RF9_",
					"ERROR: This ability is unimplemented: RF9_"));
		return NULL;
	}
	break;

	case 117: //RF9_HELL_FIRE
	case 118: //RF9_HOLY_FIRE
	{
		int rad = 5;
		int typ;
		if (num == 117) typ = GF_HELL_FIRE;
		else typ = GF_HOLY_FIRE;

		if (powerful)
		{
			base = 100 + cast_lev * 4;
			dice = 10;
			sides = 10;
		}
		else
		{
			base = 50 + cast_lev * 3;
			dice = 10;
			sides = 5;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball(typ, dir, dam, rad);
	}
	break;
	case 119: //RF9_FINAL_SPARK
	{
		base = cast_lev * 2 + 200;
		dice = 1;
		sides = cast_lev;

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}

		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_spark(GF_DISINTEGRATE, dir, dam, 2);
	}
	break;
	case 120: //RF9_TORNADO
	{
		int rad = 5;

		if (powerful)
		{
			base = 50 + cast_lev * 3;
			dice = 1;
			sides = 50;
		}
		else
		{
			base = 25 + cast_lev * 2;
			dice = 1;
			sides = 25;
		}

		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：%d", "dam: %d"), dam);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
		}
		if (fail) return "";

		if (!get_aim_dir(&dir)) return NULL;
		msg_cast_monspell_new(num, msg, xtra);
		fire_ball_jump(GF_TORNADO, dir, dam, rad, NULL);
	}
	break;
	case 121: //RF9_DESTRUCTION
	{
		base = 7;
		sides = 5;
		if (only_info) return format(_("範囲：%d+1d%d", "rad: %d+1d%d"),base,sides);
		if (fail) return "";

		msg_cast_monspell_new(num, msg, xtra);
		destroy_area(py, px, base + randint1(sides), FALSE, FALSE, FALSE);

	}
	break;
	case 125: //RF9_TELE_APPROACH
	{
		if (only_info) return format("");
		if (fail) return "";
		if (!target_set(TARGET_KILL)) return NULL;
		if (!target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_format(_("視界内のターゲットが指定されていないといけない。",
                            "The target has to be in your line of sight."));
			return NULL;
		}
		msg_cast_monspell_new(num, msg, xtra);

		teleport_player_to(target_row, target_col, 0L);
	}
	break;
	case 126: //RF9_TELE_HI_APPROACH
	{
		if (only_info) return format("");
		if (fail) return "";

		if (target_who > 0 && m_list[target_who].r_idx)
		{
			msg_cast_monspell_new(num, msg, xtra);
			teleport_player_to(m_list[target_who].fy, m_list[target_who].fx, 0L);
		}
		else
		{
			msg_format(_("ターゲットモンスターが指定されていないといけない。",
                            "You have to pick a target monster."));
			return NULL;
		}

	}
	break;

	case 127: //RF9_MAELSTROM
	{
		int rad;

		if (powerful)
		{
			rad = 8;
			base = cast_lev * 8;
			dice = 1;
			sides = cast_lev * 6;
		}
		else
		{
			rad = 6;
			base = cast_lev * 5;
			dice = 1;
			sides = cast_lev * 3;
		}
		if (monomane)
		{
			dam = mane_dam;
			if (only_info) return format(_("損傷：～%d", "dam: ~%d"), dam / 2);
		}
		else
		{
			dam = base + damroll(dice, sides);
			if (only_info) return format(_("損傷：～%d+%dd%d", "dam: ~%d+%dd%d"),base/2,dice,sides/2);
		}

		if (fail) return "";

		msg_cast_monspell_new(num, msg, xtra);

		(void)project(0, rad, py, px, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP), FALSE);
		(void)project(0, rad, py, px, rad, GF_WATER_FLOW, (PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP | PROJECT_HIDE), FALSE);

	}
	break;

	default:
	{
		if(only_info) return format(_("未実装", "unimplemented"));
		msg_format(_("ERROR:実装していない敵魔法が呼ばれた num:%d",
					"ERROR: Unimplemented enemy spell called (num: %d)"),num);
		return NULL;
	}
	}







	return "";
}





/*:::ものまね、青魔系職業の記憶可能な魔法の数*/
//v2.1.6 modeパラメータを追加 意味はcast_monspell_new()と同じ
int calc_monspell_cast_mem(int mode)
{
	int num = 0;

	//ものまね
	if (mode == 1 || mode == 2)
	{
		num = p_ptr->mane_num;
		if (num < 0 || num > MAX_MANE)
		{
			msg_format(_("p_ptr->mane_numがおかしい(%d)", "Weird value for p_ptr->mane_num (%d)"), num);
			return 0;
		}
	}
	//モンスター変身系
	else if (IS_METAMORPHOSIS) num = 20;
	//さとり
	else if (p_ptr->pclass == CLASS_SATORI) num = 3 + p_ptr->lev / 7;
	//こころ
	else if (p_ptr->pclass == CLASS_KOKORO) num = 2 + (p_ptr->lev - 1) / 13;
	//雷鼓
	else if (p_ptr->pclass == CLASS_RAIKO) num = 20;
	//隠岐奈
	else if (p_ptr->pclass == CLASS_OKINA) num = NEW_MSPELL_LIST_MAX;
	//デフォ20
	else num = 20;

	return num;
}

/*:::さとり用ものまね準備ルーチン 視界内の敵を走査し、monspell_list2[]用の添え字を最大10までp_ptr->magic_num1[]に格納する*/
void make_magic_list_satori(bool del)
{
	int i, shift;
	int mlist_num = 0;
	int memory;

	if (p_ptr->pclass != CLASS_SATORI) return;

	memory = calc_monspell_cast_mem(0);

	for (i = 0; i < 10; i++) p_ptr->magic_num1[i] = 0; //常に最大の10個分クリア
	if (del) return;

	//全モンスター走査
	for (i = 1; i < m_max; i++)
	{
		u32b rflags;
		int j;
		int new_num;
		monster_type* m_ptr = &m_list[i];
		monster_race* r_ptr = &r_info[m_ptr->r_idx];
		if (!m_ptr->r_idx) continue;
		//視界内の精神のある敵を抽出

		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;
		if (!m_ptr->ml) continue;
		if (r_ptr->flags2 & RF2_EMPTY_MIND) continue;

		//WEIRD_MINDの敵は1/2の確率でしか読めない
		if ((r_ptr->flags2 & RF2_WEIRD_MIND) && (turn % 2)) continue;

		//敵の持っている魔法を抽出しmonspell_list2[]の添字番号に変換してmagic_num1[]に追加　記憶可能数を超えたら適当に入れ替える
		for (j = 0; j < 4; j++)
		{
			int spell;
			if (j == 0) rflags = r_ptr->flags4;
			else if (j == 1) rflags = r_ptr->flags5;
			else if (j == 2) rflags = r_ptr->flags6;
			else rflags = r_ptr->flags9;

			for (shift = 0; shift < 32; shift++)
			{
				bool exist = FALSE;
				int k;
				if ((rflags >> shift) % 2 == 0) continue;
				spell = shift + 1 + j * 32;

				if (spell < 1 || spell > 128)
				{
					msg_format(_("ERROR:make_magic_satori()で不正なspell値(%d)が出た",
                                "ERROR: Incorrect spell value (%d) in make_magic_satori()"),spell);
					return;
				}
				if (!monspell_list2[spell].level) continue; //特別な行動や未実装の技は非対象


				//既にある魔法はパス
				for (k = 0; k < 10; k++) if (spell == p_ptr->magic_num1[k]) exist = TRUE;
				if (exist) continue;


				if (mlist_num < memory) new_num = mlist_num;
				else
				{
					new_num = randint0(mlist_num + 1);
					if (new_num >= memory) continue;
				}
				p_ptr->magic_num1[new_num] = spell;
				mlist_num++;
			}
		}
	}

}



/*:::
*里乃用ものまね準備ルーチン。騎乗中モンスターの魔法をmonspell_list2[]用の添え字として最大20までp_ptr->magic_num1[]に格納する。
*このルーチンは騎乗処理のたびに呼ばれる。
*/
void make_magic_list_satono(void)
{
	int i, j, new_num;
	int mlist_num = 0;
	int memory;
	monster_type* m_ptr;
	monster_race* r_ptr;
	u32b rlags;

	if (p_ptr->pclass != CLASS_SATONO) return;

	memory = calc_monspell_cast_mem(0);
	for (i = 0; i < memory; i++) p_ptr->magic_num1[i] = 0; //リストのクリア

	if (!p_ptr->riding) return;
	m_ptr = &m_list[p_ptr->riding];
	if (!m_ptr->r_idx) { msg_format(_("ERROR:ridingの値がおかしい(%d)",
                                        "ERROR: Weird riding value (%d)"), p_ptr->riding); return; }
	r_ptr = &r_info[m_ptr->r_idx];

	//モンスターの持っている魔法を抽出しmonspell_list2[]の添字番号に変換してmagic_num1[]に追加する。記憶可能数を超えたら適当に入れ替える
	for (j = 0; j < 4; j++)
	{
		u32b rflags;
		int spell, shift;
		//魔法のみを対象にする
		if (j == 0) rflags = r_ptr->flags4 & ~(RF4_NOMAGIC_MASK);
		else if (j == 1) rflags = r_ptr->flags5 & ~(RF5_NOMAGIC_MASK);
		else if (j == 2) rflags = r_ptr->flags6 & ~(RF6_NOMAGIC_MASK);
		else rflags = r_ptr->flags9 & ~(RF9_NOMAGIC_MASK);

		for (shift = 0; shift < 32; shift++)
		{
			int k;
			if ((rflags >> shift) % 2 == 0) continue;
			spell = shift + 1 + j * 32;

			if (spell < 1 || spell > 128)
			{
				msg_format(_("ERROR:make_magic_satono()で不正なspell値(%d)が出た",
                            "ERROR: Incorrect spell value (%d) in make_magic_satono()"), spell);
				return;
			}
			if (!monspell_list2[spell].level) continue; //特別な行動や未実装の技は非対象

			if (mlist_num < memory) new_num = mlist_num;
			else
			{
				new_num = randint0(mlist_num + 1);
				if (new_num >= memory) continue;
			}
			if (new_num < 0)
			{
				msg_print("ERROR:make_magic_list_satono()");
				return;
			}

			p_ptr->magic_num1[new_num] = spell;
			mlist_num++;
		}
	}

}

/*:::雷鼓魔法リスト作成　武器片方毎に10、合計20まで　魔法数合計を返す*/
//p_ptr->magic_num1[10]-[29]を魔法リストに使う。[30]には「幾つ目の魔法からが2本目の武器か」が記録される。
int make_magic_list_aux_raiko(void)
{
	object_type* o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int hand;
	int power, rank;
	int magic_num = 10;
	int i;

	for (i = 10; i < 30; i++) p_ptr->magic_num1[i] = 0; //魔法リストを20個クリア
	p_ptr->magic_num1[30] = 0;

	//両手ループ
	for (hand = 0; hand < 2; hand++)
	{
		int magic_num_one = 0; //今の武器から得た魔法の数 10になったら終了
		if (hand == 1) p_ptr->magic_num1[30] = (magic_num - 10);

		o_ptr = &inventory[INVEN_RARM + hand];

		//通常武器かランダムアーティファクトのみ
		if (!o_ptr->k_idx || !object_is_melee_weapon(o_ptr) || object_is_fixed_artifact(o_ptr)) continue;
		if (o_ptr->xtra3 < 50) continue;
		object_flags(o_ptr, flgs);

		//武器のランクを五段階に決める
		power = k_info[o_ptr->k_idx].level;
		if (object_is_artifact(o_ptr)) power += 50;
		else if (object_is_ego(o_ptr)) power += e_info[o_ptr->name2].rarity;//e_info.levelは機能していないのでrarityを使う
		power += flag_cost(o_ptr, o_ptr->pval, FALSE) / 1000;

		if (power < 50) rank = 0;
		else if (power < 100) rank = 1;
		else if (power < 150) rank = 2;
		else rank = 3;

		if (o_ptr->xtra1 == 100) rank += 1;//「完全に使いこなせる」ものは1ランクアップ


		//以後、武器のフラグとランクによって使用可能な魔法を判定しp_ptr->magic_num1[11-29]に格納　武器片方ごとに最大10　
		//格納される値はmonspell_list2[]の添字

		//弾幕はなし？
		//波動砲
		if ((have_flag(flgs, TR_VORPAL) || have_flag(flgs, TR_EX_VORPAL)) && have_flag(flgs, TR_FORCE_WEAPON) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 17;
			if (++magic_num_one == 10) continue;
		}

		//魔力の嵐
		if ((o_ptr->name2 == EGO_WEAPON_HERO || have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 3
			|| (o_ptr->name2 == EGO_WEAPON_FORCE && rank > 2))
		{
			p_ptr->magic_num1[magic_num++] = 40;
			if (++magic_num_one == 10) continue;
		}
		//暗黒の嵐
		if ((o_ptr->name2 == EGO_WEAPON_VAMP || have_flag(flgs, TR_RES_DARK) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS))) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 41;
			if (++magic_num_one == 10) continue;
		}
		//閃光の嵐
		if ((o_ptr->name2 == EGO_WEAPON_HAKUREI || have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS))) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 53;
			if (++magic_num_one == 10) continue;
		}
		//ヘルファイア
		if (have_flag(flgs, TR_KILL_GOOD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 117;
			if (++magic_num_one == 10) continue;
		}
		//ホーリーファイア
		if (have_flag(flgs, TR_KILL_EVIL) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 118;
			if (++magic_num_one == 10) continue;
		}

		//汚染の球
		if (have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 102;
			if (++magic_num_one == 10) continue;
		}

		//原子分解
		if ((have_flag(flgs, TR_VORPAL) && rank > 3 || have_flag(flgs, TR_EX_VORPAL)) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)))
		{
			p_ptr->magic_num1[magic_num++] = 103;
			if (++magic_num_one == 10) continue;
		}
		//破邪の光球
		if ((have_flag(flgs, TR_SLAY_EVIL) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2)
			|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 104;
			if (++magic_num_one == 10) continue;
		}
		//メテオストライク
		if (have_flag(flgs, TR_IMPACT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 105;
			if (++magic_num_one == 10) continue;
		}
		//空間歪曲
		if ((have_flag(flgs, TR_TELEPORT) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2)
			|| (have_flag(flgs, TR_RES_TIME) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1))
		{
			p_ptr->magic_num1[magic_num++] = 106;
			if (++magic_num_one == 10) continue;
		}
		//メイルシュトロム
		if (o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 ||
			have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 127;
			if (++magic_num_one == 10) continue;
		}

		//レーザー
		if (have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1
			|| have_flag(flgs, TR_VORPAL) && rank > 2 || have_flag(flgs, TR_EX_VORPAL))
		{
			p_ptr->magic_num1[magic_num++] = 115;
			if (++magic_num_one == 10) continue;
		}
		//大型レーザー
		if ((have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2)
			|| have_flag(flgs, TR_IM_FIRE) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 112;
			if (++magic_num_one == 10) continue;
		}
		//光の剣
		if (have_flag(flgs, TR_FORCE_WEAPON) && rank > 2
			|| have_flag(flgs, TR_EX_VORPAL) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 76;
			if (++magic_num_one == 10) continue;
		}

		//気弾
		if ((have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_CON) && rank > 2)
			|| (have_flag(flgs, TR_STR) && have_flag(flgs, TR_CON) && rank > 2)
			|| (have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
			|| (o_ptr->name2 == EGO_WEAPON_ONI && rank > 2))
		{
			p_ptr->magic_num1[magic_num++] = 27;
			if (++magic_num_one == 10) continue;
		}


		//魔力消去
		if (have_flag(flgs, TR_EX_VORPAL) || have_flag(flgs, TR_VORPAL) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 3;
			if (++magic_num_one == 10) continue;
		}
		//魔力吸収
		if (have_flag(flgs, TR_VAMPIRIC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_CHR)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 42;
			if (++magic_num_one == 10) continue;
		}
		//加速
		if (o_ptr->name2 == EGO_WEAPON_EXATTACK && rank > 1
			|| o_ptr->name2 == EGO_WEAPON_HUNTER && rank > 2
			|| have_flag(flgs, TR_SPEEDSTER) && rank > 1
			|| have_flag(flgs, TR_SPEED) && o_ptr->pval > 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 65;
			if (++magic_num_one == 10) continue;
		}
		//破滅の手
		if (o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0
			|| (o_ptr->curse_flags & TRC_PERMA_CURSE)
			|| (o_ptr->curse_flags & TRC_ADD_H_CURSE) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 66;
			if (++magic_num_one == 10) continue;
		}
		//治癒
		if ((have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_CON) && rank > 2)
			|| (have_flag(flgs, TR_CON) && have_flag(flgs, TR_WIS) && rank > 3)
			|| (have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_WIS) && rank > 3)
			|| (o_ptr->name2 == EGO_WEAPON_DEFENDER && rank > 2)
			|| (o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1))
		{
			p_ptr->magic_num1[magic_num++] = 67;
			if (++magic_num_one == 10) continue;
		}
		//無敵化
		if ((o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 3)
			|| have_flag(flgs, TR_BLESSED) && o_ptr->to_a > 5 && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 68;
			if (++magic_num_one == 10) continue;
		}
		//ショート・テレポート
		if (have_flag(flgs, TR_TELEPORT) || have_flag(flgs, TR_LEVITATION) || have_flag(flgs, TR_SPEEDSTER))
		{
			p_ptr->magic_num1[magic_num++] = 69;
			if (++magic_num_one == 10) continue;
		}
		//テレポート
		if (have_flag(flgs, TR_TELEPORT) && rank > 0
			|| have_flag(flgs, TR_RES_CHAOS)
			|| have_flag(flgs, TR_RES_TIME))
		{
			p_ptr->magic_num1[magic_num++] = 70;
			if (++magic_num_one == 10) continue;
		}
		//ザ・ワールド
		if (have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_SPEEDSTER) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 71;
			if (++magic_num_one == 10) continue;
		}
		//テレポバック
		if (have_flag(flgs, TR_NO_TELE) && rank > 0 || have_flag(flgs, TR_RIDING) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 73;
			if (++magic_num_one == 10) continue;
		}
		//テレポアウェイ
		if (have_flag(flgs, TR_NO_TELE) && rank > 1 || have_flag(flgs, TR_BLESSED) && rank > 3 || o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 74;
			if (++magic_num_one == 10) continue;
		}
		//隣接テレポ
		if (o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 1 || have_flag(flgs, TR_SPEEDSTER) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 125;
			if (++magic_num_one == 10) continue;
		}
		//視界外隣接テレポ
		if (have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 126;
			if (++magic_num_one == 10) continue;
		}
		//ライトエリア
		if (have_flag(flgs, TR_LITE))
		{
			p_ptr->magic_num1[magic_num++] = 77;
			if (++magic_num_one == 10) continue;
		}

		//*破壊*
		if (have_flag(flgs, TR_CHAOTIC) && rank > 3 || have_flag(flgs, TR_IMPACT) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 121;
			if (++magic_num_one == 10) continue;
		}
		//炎の嵐
		if (have_flag(flgs, TR_BRAND_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
			|| have_flag(flgs, TR_IM_FIRE) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 97;
			if (++magic_num_one == 10) continue;
		}
		//氷の嵐
		if (have_flag(flgs, TR_BRAND_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
			|| have_flag(flgs, TR_IM_COLD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 98;
			if (++magic_num_one == 10) continue;
		}
		//雷の嵐
		if (have_flag(flgs, TR_BRAND_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
			|| have_flag(flgs, TR_IM_ELEC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 99;
			if (++magic_num_one == 10) continue;
		}
		//酸の嵐
		if (have_flag(flgs, TR_BRAND_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
			|| have_flag(flgs, TR_IM_ACID) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 100;
			if (++magic_num_one == 10) continue;
		}
		//毒素の嵐
		if (have_flag(flgs, TR_BRAND_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 101;
			if (++magic_num_one == 10) continue;
		}

		//地獄球
		if (o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 1 ||
			have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 38;
			if (++magic_num_one == 10) continue;
		}
		//ウォーター・ボール
		if (o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank == 3 || o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank >= 2
			|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 39;
			if (++magic_num_one == 10) continue;
		}
		//純ログルス
		if (o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 1 ||
			have_flag(flgs, TR_RES_CHAOS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 31;
			if (++magic_num_one == 10) continue;
		}
		//竜巻
		if (o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 120;
			if (++magic_num_one == 10) continue;
		}

		//地獄の矢
		if (o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON ||
			have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 54;
			if (++magic_num_one == 10) continue;
		}
		//ウォーター・ボルト
		if (o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank >= 1 || o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank == 1
			|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 55;
			if (++magic_num_one == 10) continue;
		}
		//魔力の矢
		if ((o_ptr->name2 == EGO_WEAPON_HERO || o_ptr->name2 == EGO_WEAPON_FORCE ||
			have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 1 ||
			have_flag(flgs, TR_THROW) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 56;
			if (++magic_num_one == 10) continue;
		}
		//プラズマボルト
		if (o_ptr->name2 == EGO_WEAPON_BRANDELEM || have_flag(flgs, TR_IM_FIRE) || have_flag(flgs, TR_IM_ELEC) ||
			have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 57;
			if (++magic_num_one == 10) continue;
		}
		//極寒の矢
		if (o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 1 || have_flag(flgs, TR_IM_COLD) ||
			have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 58;
			if (++magic_num_one == 10) continue;
		}

		//聖なる矢
		if (o_ptr->name2 == EGO_WEAPON_HAKUREI || have_flag(flgs, TR_RES_HOLY) && rank > 2 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)))
		{
			p_ptr->magic_num1[magic_num++] = 111;
			if (++magic_num_one == 10) continue;
		}

		//♪のボルト
		if (have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1
			|| have_flag(flgs, TR_AGGRAVATE) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 113;
			if (++magic_num_one == 10) continue;
		}
		//精神攻撃
		if (have_flag(flgs, TR_CHAOTIC) && rank > 0 && rank < 3)
		{
			p_ptr->magic_num1[magic_num++] = 43;
			if (++magic_num_one == 10) continue;
		}
		//脳攻撃
		if (have_flag(flgs, TR_CHAOTIC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 44;
			if (++magic_num_one == 10) continue;
		}
		//軽傷の呪い
		if (o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 45;
			if (++magic_num_one == 10) continue;
		}
		//重傷の呪い
		if (o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 46;
			if (++magic_num_one == 10) continue;
		}
		//致命傷の呪い
		if ((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 47;
			if (++magic_num_one == 10) continue;
		}
		//死の言霊
		if ((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON || o_ptr->name2 == EGO_WEAPON_MURDERER) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 48;
			if (++magic_num_one == 10) continue;
		}
		//コズミック・ホラー
		if (have_flag(flgs, TR_RES_INSANITY) && rank > 3 || have_flag(flgs, TR_CHR) && o_ptr->pval < -4)
		{
			p_ptr->magic_num1[magic_num++] = 80;
			if (++magic_num_one == 10) continue;
		}


		//魔除けのまじない
		if ((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 107;
			if (++magic_num_one == 10) continue;
		}
		//聖なる言葉
		if ((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 108;
			if (++magic_num_one == 10) continue;
		}
		//退魔の呪文
		if ((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 3)
		{
			p_ptr->magic_num1[magic_num++] = 109;
			if (++magic_num_one == 10) continue;
		}
		//破邪の印
		if ((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank > 3
			|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank < 3 || have_flag(flgs, TR_RES_HOLY) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 110;
			if (++magic_num_one == 10) continue;
		}

		//アシッド・ボール
		if (o_ptr->name2 == EGO_WEAPON_BRANDACID && rank >= 2 ||
			have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 33;
			if (++magic_num_one == 10) continue;
		}
		//サンダー・ボール
		if (o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank >= 2 ||
			have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 34;
			if (++magic_num_one == 10) continue;
		}
		//ファイア・ボール
		if (o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank >= 2 ||
			have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 35;
			if (++magic_num_one == 10) continue;
		}
		//アイス・ボール
		if (o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank >= 2 ||
			have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 36;
			if (++magic_num_one == 10) continue;
		}
		//アシッド・ボルト
		if (o_ptr->name2 == EGO_WEAPON_BRANDACID && rank == 1 ||
			have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 49;
			if (++magic_num_one == 10) continue;
		}
		//サンダー・ボルト
		if (o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank == 1 ||
			have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 50;
			if (++magic_num_one == 10) continue;
		}
		//ファイア・ボルト
		if (o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank == 1 ||
			have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 51;
			if (++magic_num_one == 10) continue;
		}
		//アイス・ボルト
		if (o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank == 1 ||
			have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 52;
			if (++magic_num_one == 10) continue;
		}
		//悪臭雲
		if (o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank <= 2 ||
			have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank <= 2)
		{
			p_ptr->magic_num1[magic_num++] = 37;
			if (++magic_num_one == 10) continue;
		}


		//救援召喚
		if (have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 3 || o_ptr->name2 == EGO_WEAPON_HERO && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 81;
			if (++magic_num_one == 10) continue;
		}
		//サイバーU
		if (have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_SLAY_DEMON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 82;
			if (++magic_num_one == 10) continue;
		}
		//モンスター一体召喚
		if (have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 83;
			if (++magic_num_one == 10) continue;
		}
		//モンスター複数召喚
		if (have_flag(flgs, TR_TELEPATHY) && have_flag(flgs, TR_TELEPORT))
		{
			p_ptr->magic_num1[magic_num++] = 84;
			if (++magic_num_one == 10) continue;
		}
		//アリ召喚
		if (o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank < 2)
		{
			p_ptr->magic_num1[magic_num++] = 85;
			if (++magic_num_one == 10) continue;
		}
		//虫召喚
		if (o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 86;
			if (++magic_num_one == 10) continue;
		}
		//ハウンド召喚
		if ((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 87;
			if (++magic_num_one == 10) continue;
		}
		//ヒドラ召喚
		if (have_flag(flgs, TR_KILL_ANIMAL) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 88;
			if (++magic_num_one == 10) continue;
		}
		//天使召喚
		if (have_flag(flgs, TR_KILL_GOOD) && rank > 2 || have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_GOOD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 89;
			if (++magic_num_one == 10) continue;
		}
		//悪魔召喚
		if (have_flag(flgs, TR_SLAY_DEMON) && rank > 2 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 90;
			if (++magic_num_one == 10) continue;
		}
		//アンデッド召喚
		if (have_flag(flgs, TR_SLAY_UNDEAD) && rank > 2 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_UNDEAD) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 91;
			if (++magic_num_one == 10) continue;
		}
		//ドラゴン召喚
		if (have_flag(flgs, TR_SLAY_DRAGON) && rank > 2 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DRAGON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 92;
			if (++magic_num_one == 10) continue;
		}
		//上級アンデッド召喚
		if (have_flag(flgs, TR_KILL_UNDEAD) && rank > 3 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_UNDEAD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 93;
			if (++magic_num_one == 10) continue;
		}
		//古代ドラゴン召喚
		if (have_flag(flgs, TR_KILL_DRAGON) && rank > 3 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DRAGON) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 94;
			if (++magic_num_one == 10) continue;
		}
		//アンバライト召喚
		if ((have_flag(flgs, TR_KILL_HUMAN) && rank > 3 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_HUMAN) && rank > 3)
			&& have_flag(flgs, TR_RES_TIME))
		{
			p_ptr->magic_num1[magic_num++] = 95;
			if (++magic_num_one == 10) continue;
		}
		//ユニークモンスター召喚
		if (have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_UNIQUE) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 96;
			if (++magic_num_one == 10) continue;
		}

		//動物召喚
		if ((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL)) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 114;
			if (++magic_num_one == 10) continue;
		}
		//神格召喚
		if (have_flag(flgs, TR_KILL_DEITY) && rank > 1 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEITY) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 122;
			if (++magic_num_one == 10) continue;
		}
		//上級悪魔召喚
		if (have_flag(flgs, TR_KILL_DEMON) && rank > 2 ||
			have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 3
			&& o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 123;
			if (++magic_num_one == 10) continue;
		}
		//妖怪召喚
		if (have_flag(flgs, TR_KILL_KWAI) && rank > 2 || have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_KWAI) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 124;
			if (++magic_num_one == 10) continue;
		}

		//聖なるブレス
		if (have_flag(flgs, TR_BLESSED) && rank > 3 || have_flag(flgs, TR_RES_HOLY) && have_flag(flgs, TR_WIS) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 6;
			if (++magic_num_one == 10) continue;
		}
		//地獄の劫火のブレス
		if (o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 3 || have_flag(flgs, TR_KILL_GOOD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 7;
			if (++magic_num_one == 10) continue;
		}

		//アクアブレス
		if (o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 || have_flag(flgs, TR_RES_WATER) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 8;
			if (++magic_num_one == 10) continue;
		}
		//酸ブレス
		if (o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 2 || have_flag(flgs, TR_IM_ACID) ||
			have_flag(flgs, TR_RES_ACID) && have_flag(flgs, TR_CON) && rank > 3 || have_flag(flgs, TR_BRAND_ACID) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 9;
			if (++magic_num_one == 10) continue;
		}
		//電撃ブレス ちょっと得やすくする
		if (o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 1 || have_flag(flgs, TR_IM_ELEC) ||
			have_flag(flgs, TR_RES_ELEC) && have_flag(flgs, TR_DEX) && rank > 2 || have_flag(flgs, TR_BRAND_ELEC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 10;
			if (++magic_num_one == 10) continue;
		}
		//火炎ブレス
		if (o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 2 || have_flag(flgs, TR_IM_FIRE) ||
			have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_STR) && rank > 3 || have_flag(flgs, TR_BRAND_FIRE) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 11;
			if (++magic_num_one == 10) continue;
		}
		//冷気ブレス
		if (o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 2 || have_flag(flgs, TR_IM_COLD) ||
			have_flag(flgs, TR_RES_COLD) && have_flag(flgs, TR_WIS) && rank > 3 || have_flag(flgs, TR_BRAND_COLD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 12;
			if (++magic_num_one == 10) continue;
		}

		//毒ブレス
		if (o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank > 2 ||
			have_flag(flgs, TR_RES_POIS) && have_flag(flgs, TR_INT) && rank > 3 || have_flag(flgs, TR_BRAND_POIS) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 13;
			if (++magic_num_one == 10) continue;
		}

		//地獄ブレス
		if (o_ptr->name2 == EGO_WEAPON_DEMONLORD || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 2 ||
			have_flag(flgs, TR_RES_NETHER) && have_flag(flgs, TR_VAMPIRIC) && rank > 2 || have_flag(flgs, TR_KILL_HUMAN) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 14;
			if (++magic_num_one == 10) continue;
		}

		//閃光ブレス
		if (o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1 || o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER && rank > 2 ||
			have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_WIS) && rank > 2 || have_flag(flgs, TR_KILL_UNDEAD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 15;
			if (++magic_num_one == 10) continue;
		}
		//暗黒ブレス
		if (o_ptr->name2 == EGO_WEAPON_MURDERER && rank > 2 ||
			have_flag(flgs, TR_RES_DARK) && have_flag(flgs, TR_CHR) && rank > 2 || have_flag(flgs, TR_SLAY_GOOD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 16;
			if (++magic_num_one == 10) continue;
		}
		//轟音ブレス
		if (o_ptr->name2 == EGO_WEAPON_DRAGONSLAY && rank > 2 ||
			have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_STR) && rank > 2 || have_flag(flgs, TR_SLAY_DRAGON) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 18;
			if (++magic_num_one == 10) continue;
		}
		//カオスブレス
		if (o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 2 ||
			have_flag(flgs, TR_RES_CHAOS) && have_flag(flgs, TR_CON) && rank > 2 || have_flag(flgs, TR_CHAOTIC) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 19;
			if (++magic_num_one == 10) continue;
		}
		//劣化ブレス
		if (o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 ||
			have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 20;
			if (++magic_num_one == 10) continue;
		}
		//因果混乱ブレス
		if (have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 21;
			if (++magic_num_one == 10) continue;
		}
		//時間逆転ブレス
		if (have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_CHR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 22;
			if (++magic_num_one == 10) continue;
		}
		//遅鈍ブレス
		if (have_flag(flgs, TR_SPEED) && o_ptr->pval < 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 23;
			if (++magic_num_one == 10) continue;
		}
		//重力ブレス
		if (o_ptr->name2 == EGO_WEAPON_QUAKE && rank > 1 || have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 24;
			if (++magic_num_one == 10) continue;
		}
		//破片ブレス
		if (o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 ||
			have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_DEX) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 25;
			if (++magic_num_one == 10) continue;
		}
		//プラズマブレス
		if (have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_BRAND_ELEC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 26;
			if (++magic_num_one == 10) continue;
		}
		//魔力ブレス
		if (have_flag(flgs, TR_FORCE_WEAPON) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 28;
			if (++magic_num_one == 10) continue;
		}
		//核熱ブレス
		if (have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_RES_LITE) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 30;
			if (++magic_num_one == 10) continue;
		}
		//分解ブレス
		if (have_flag(flgs, TR_EX_VORPAL) && (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON)) && rank > 2
			|| have_flag(flgs, TR_TUNNEL) && o_ptr->pval > 2 && (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON)) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 32;
			if (++magic_num_one == 10) continue;
		}


		//マジックミサイル
		if (rank > 0 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_SUST_INT) || have_flag(flgs, TR_SUST_WIS))
			|| have_flag(flgs, TR_THROW))
		{
			p_ptr->magic_num1[magic_num++] = 59;
			if (++magic_num_one == 10) continue;
		}
		//恐慌
		if (o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON)
		{
			p_ptr->magic_num1[magic_num++] = 60;
			if (++magic_num_one == 10) continue;
		}
		//盲目
		if (o_ptr->name2 == EGO_WEAPON_MURDERER || o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 61;
			if (++magic_num_one == 10) continue;
		}
		//混乱
		if (o_ptr->name2 == EGO_WEAPON_CHAOS || o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 62;
			if (++magic_num_one == 10) continue;
		}
		//減速
		if (o_ptr->name2 == EGO_WEAPON_QUAKE || o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 63;
			if (++magic_num_one == 10) continue;
		}
		//麻痺
		if (o_ptr->name2 == EGO_WEAPON_VAMP || o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 64;
			if (++magic_num_one == 10) continue;
		}
		//トラップ
		if (have_flag(flgs, TR_AGGRAVATE) || o_ptr->curse_flags & TRC_ADD_H_CURSE || o_ptr->curse_flags & TRC_ADD_L_CURSE)
		{
			p_ptr->magic_num1[magic_num++] = 78;
			if (++magic_num_one == 10) continue;
		}
		//記憶消去
		if (have_flag(flgs, TR_STEALTH) && o_ptr->pval > 0)
		{
			p_ptr->magic_num1[magic_num++] = 79;
			if (++magic_num_one == 10) continue;
		}

		//叫び、警報
		if (have_flag(flgs, TR_AGGRAVATE) || have_flag(flgs, TR_STEALTH) && o_ptr->pval < 0)
		{
			if (rank > 1) p_ptr->magic_num1[magic_num++] = 128; //警報
			else p_ptr->magic_num1[magic_num++] = 1; //叫び
			if (++magic_num_one == 10) continue;
		}


	}
	return (magic_num - 10);

}

//雷鼓技レベルを計算する。iはspell_list[]添字でp_ptr->magic_num1[30]には左手武器開始部のi値が入っているはず
//☆はPOWERFUL扱いのため+100
static int calc_raiko_spell_lev(int i)
{
	int skilllev;
	object_type* o_ptr;
	if (i < p_ptr->magic_num1[30]) o_ptr = &inventory[INVEN_RARM];
	else  o_ptr = &inventory[INVEN_LARM];

	skilllev = k_info[o_ptr->k_idx].level / 2 + p_ptr->lev;
	if (skilllev > 99) skilllev = 99;
	if (object_is_artifact(o_ptr)) skilllev += 100;

	return skilllev;

}


//里乃は独自に最低失敗率計算をする
static int satono_spell_minfail(void)
{
	monster_type* m_ptr = &m_list[p_ptr->riding];
	monster_race* r_ptr = &r_info[m_ptr->r_idx];

	int chance = 25 - r_ptr->level / 4;//モンスターの魔法使用と同じ失敗率

	//int mult = r_ptr->freq_spell + p_ptr->stat_ind[A_CHR] + 3;//魔法使用率+魅力補正

	//if (mult > 100) mult = 100;

	//chance = 100 - (100 - chance) * mult / 100;//モンスター失敗率と魔法使用率からトータル失敗率を計算

	if (MON_STUNNED(m_ptr)) chance += 50;
	if (MON_CONFUSED(m_ptr)) chance += 100;

	if (chance < 0) chance = 0;
	if (chance > 100) chance = 100;

	return chance;
}

//モンスターの魔法を詠唱する青魔系の処理
//行動順消費したときTRUEを返す
//v2.1.6 ものまねに対応するためあちこち変更
//mode 0: 青魔系処理　職業ごとに魔法リストなどを個別処理する必要あり
//mode 1: ものまね
//mode 2: 威力二倍のものまね
bool cast_monspell_new(int mode)
{

	int spell_list[NEW_MSPELL_LIST_MAX]; //使用可能魔法リスト monspell_list2[]の添字
	int mane_dam_list[NEW_MSPELL_LIST_MAX];//ものまね専用　魔法ダメージ
	int memory = 0; //モンスター魔法の記憶可能数
	int cnt = 0; //実際に使用可能な魔法のリストの長さ
	int spell_lev;
	cptr power_desc = "";

	int num; //技番号 0から開始
	int mane_dam = 0; //選択された技がものまねだった場合のダメージ値
	bool  flag_choice, flag_redraw;
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
	int xtra = 0; //aux()に渡す特殊パラメータ
	int choose_num = 0;//表示されたspell_list[]から選択された特技や魔法の位置

	bool monomane = FALSE;

	if (mode == 1 || mode == 2) monomane = TRUE;

	for (i = 0; i < NEW_MSPELL_LIST_MAX; i++)
	{
		spell_list[i] = 0;
		mane_dam_list[i] = 0;
	}

	//特技リスト長を取得
	memory = calc_monspell_cast_mem(mode);
	if (!memory)
	{
		msg_print(_("使える魔法や特技がない！", "You don't have spells or abilities you can use!"));
		return FALSE;
	}

	/*:::各種変数やリストを設定*/

	//ものまね
	if (monomane)
	{
		power_desc = _("まね", "imitate");
		xtra = CAST_MONSPELL_EXTRA_MONOMANE;

		for (i = 0; i < memory; i++)
		{
			spell_list[i] = p_ptr->mane_spell[i];
			mane_dam_list[i] = p_ptr->mane_dam[i] * (mode == 1 ? 1 : 2);
			if (spell_list[i]) cnt++;
		}
	}

	//モンスター変身時
	else if (IS_METAMORPHOSIS)
	{
		monster_race* r_ptr = &r_info[MON_EXTRA_FIELD];
		power_desc = _("使用", "use");

		for (i = 0; i <= MAX_MONSPELLS2; i++)
		{
			if (monspell_list2[i].priority == 0) continue;

			if (i <= 32 && !(r_ptr->flags4 >> (i - 1) & 1L)) continue;
			if (i > 32 && i <= 64 && !(r_ptr->flags5 >> (i - 33) & 1L)) continue;
			if (i > 64 && i <= 96 && !(r_ptr->flags6 >> (i - 65) & 1L)) continue;
			if (i > 96 && !(r_ptr->flags9 >> (i - 97) & 1L)) continue;


			if (cnt < memory)
			{
				spell_list[cnt] = i;
				cnt++;
			}
			else //敵が20以上の特技を持っているとき、priority値を参考に一番優先度が低いのを消す
			{
				int change_num = 0;
				int j;

				for (j = memory - 1; j > 0; j--)
				{
					if (monspell_list2[spell_list[j]].priority > monspell_list2[spell_list[change_num]].priority) change_num = j;
				}
				if (monspell_list2[spell_list[change_num]].priority > monspell_list2[i].priority) spell_list[change_num] = i;

			}
		}
	}
	//さとり用初期化
	else if (p_ptr->pclass == CLASS_SATORI)
	{
		power_desc = _("想起", "recollect");
		for (i = 0; i < memory; i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if (spell_list[i]) cnt++;
		}
	}
	//里乃用初期化
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		power_desc = _("応援", "support");
		for (i = 0; i < memory; i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if (spell_list[i]) cnt++;
		}
	}
	//こころ用初期化
	else if (p_ptr->pclass == CLASS_KOKORO)
	{
		object_type* o_ptr = &inventory[INVEN_HEAD];

		power_desc = _("再演", "reproduce");
		if(o_ptr->tval != TV_MASK)
		{
			msg_print(_("面を装備していない。", "You're not wearing a mask."));
			return FALSE;
		}
		else if(object_is_cursed(o_ptr))
		{
			msg_print(_("面が呪われていて使えない。", "You can't use a cursed mask."));
			return FALSE;
		}

		spell_list[0] = o_ptr->xtra1;
		spell_list[1] = o_ptr->xtra2;
		spell_list[2] = o_ptr->xtra3;
		spell_list[3] = o_ptr->xtra4 % 200;
		spell_list[4] = o_ptr->xtra4 / 200;
		for (i = 0; i < memory; i++) if (spell_list[i]) cnt++;
	}
	//雷鼓用初期化
	else if (p_ptr->pclass == CLASS_RAIKO)
	{
		power_desc = _("発動", "activate");
		cnt = make_magic_list_aux_raiko();
		for (i = 0; i < memory; i++)spell_list[i] = p_ptr->magic_num1[10 + i];

	}
	//紫弾幕結界
	else if (p_ptr->pclass == CLASS_YUKARI)
	{
		const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/
		power_desc = _("使用", "use");

		for (i = 1; i <= MAX_MONSPELLS2; i++)
		{
			if (i % 8 != turn / (A_DAY / 24) % 8) continue;
			if (!monspell_list2[i].level) continue;
			spell_list[cnt++] = i;
		}
	}
	//依姫降神
	else if (p_ptr->pclass == CLASS_YORIHIME)
	{
		power_desc = _("使用", "use");
		for (i = 0; i < memory; i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if (spell_list[i]) cnt++;
		}
	}
	//隠岐奈　後戸の力
	//v1.1.82b 弾幕研究家
	//v2.0.6 尤魔もこの方式で選択する
	else if (p_ptr->pclass == CLASS_OKINA || p_ptr->pclass == CLASS_RESEARCHER || p_ptr->pclass == CLASS_YUMA)
	{
		int monspell_kind;
		monspell_kind = choose_monspell_kind();//モンスター魔法分類(ボルト・ボール・ブレスなど)を選択。リピートなら前のを使用

		if (!monspell_kind) return FALSE;

		if(p_ptr->pclass == CLASS_OKINA)
			power_desc = _("行使", "use");
		else
			power_desc = _("再現", "reproduce");


		//指定した分類のモンスター魔法をすべて登録
		for (i = 1; i <= MAX_MONSPELLS2; i++)
		{
			if (monspell_list2[i].monspell_type != monspell_kind) continue;

			if (cnt == NEW_MSPELL_LIST_MAX)
			{
				msg_print(_("ERROR:モンスター魔法リストの登録可能数を超過した",
                            "ERROR: Exceed max amount in monster spell list"));
				break;
			}
			spell_list[cnt++] = i;
		}
	}

	else
	{
		msg_print(_("ERROR:この職業でのモンスター魔法詠唱処理が実装されていない",
                    "ERROR: Unimplemented logic for castic monster spells for this class"));
		return FALSE;
	}

	if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		msg_print(_("ダンジョンが魔力を吸収した！", "The dungeon absorbs magical energy!"));
		return FALSE;
	}
	if (p_ptr->anti_magic)
	{
		msg_print(_("反魔法バリアに妨害された！", "Your anti-magic barrier prevents it!"));
		return FALSE;
	}

	screen_save();

	flag_choice = FALSE;
	flag_redraw = FALSE;

	num = (-1);
#ifdef ALLOW_REPEAT
	//リピートコマンドのとき、前回使用時の特技番号を取得する
	if (repeat_pull(&num))
	{
		//レベルが足りていない(直前に経験値減少攻撃を受けてレベルが下がったなど)ときはリピートしても選択済みフラグが立たず選択し直し。
		//ただしドレミーと里乃はレベル制限無視
		//v1.1.94 モンスター変身中にもレベル制限無視
		if (monspell_list2[num].level <= p_ptr->lev || (IS_METAMORPHOSIS) || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_SATONO) flag_choice = TRUE;

		//依姫神降ろしは使う度に効果が変わるのでリピート不可
		if (p_ptr->pclass == CLASS_YORIHIME) flag_choice = FALSE;

		//ものまねは使った特技が消えるのでリピート不可
		if (monomane) flag_choice = FALSE;
	}
#endif

	/*:::ここから、技の一覧を表示して選択を受け取り技番号を得る処理*/

	if(cnt == 0)
	{
		msg_format(_("%sできる特技がない。", "You don't have abilities you can %s."),power_desc);
		screen_load();
		return FALSE;
	}

	(void) strnfmt(out_val, 78, _("(%c-%c, '*'で一覧, ESC) どれを%sしますか？",
                                    "(%c-%c, '*' - List, ESC) Which one do you want to %s?"),	I2A(0), I2A(cnt - 1),power_desc);

	choice = (always_show_list) ? ESCAPE : 1;

	while (!flag_choice)
	{
		if (choice == ESCAPE) choice = ' ';
		else if (!get_com(out_val, &choice, TRUE))break;
		/*:::メニュー表示用*/

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!flag_redraw)
			{
				char psi_desc[80];

				flag_redraw = TRUE;
				screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(_("名前", "Name"), y, x + 5);

				put_str(format(_("Lv 消費MP　 関連　失率 効果",
                                 "Lv Cost     Stat  Fail Effect")), y, x + 35);

				/* Dump the spells */
				/*:::技などの一覧を表示*/
				for (i = 0; i < cnt; i++)
				{
					bool	flag_usable = TRUE;

					int use_stat = monspell_list2[spell_list[i]].use_stat;
					//さとりは常に知能で技を使う
					if (p_ptr->pclass == CLASS_SATORI) use_stat = A_INT;
					//里乃は常に魅力で技を使う。
					if (p_ptr->pclass == CLASS_SATONO) use_stat = A_CHR;

					//雷鼓はどちらの武器の技かでレベル判定が違う
					if (p_ptr->pclass == CLASS_RAIKO)
						xtra = calc_raiko_spell_lev(i);

					cost = monspell_list2[spell_list[i]].smana;

					//ものまねは追加コストなし
					if (monomane) cost = 0;

					if (cost > 0)
					{
						/*:::消費魔力減少 mod_need_manaに定義されたMANA_DIVとDEC_MANA_DIVの値を使用*/
						if (p_ptr->dec_mana) cost = cost * 3 / 4;

						if (cost < 1) cost = 1;
					}

					spell_lev = monspell_list2[spell_list[i]].level;
					chance = monspell_list2[spell_list[i]].fail;


					/*:::失敗率計算　失敗率設定0の技はパス*/
					if (chance)
					{
						//v1.1.48 ドレミーの変身と紫苑の憑依
						if (IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME))
						{
							//モンスターの魔法使用時と同じ失敗率
							chance = 25 - (r_info[MON_EXTRA_FIELD].level + 3) / 4;
							if (chance < 0) chance = 0;

							if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
							else if (p_ptr->stun) chance += 15;
							/*:::十分なMPがないとき　どのみち実行できなくするが*/
							if (cost > p_ptr->csp) chance = 100;

						}
						else
						{
							/*:::レベル差による失敗率減少*/
							chance -= 3 * (p_ptr->lev - spell_lev);
							/*:::パラメータによる失敗率減少*/
							chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[use_stat]] - 1);

							/*:::性格や呪いによる成功率増減*/
							chance += p_ptr->to_m_chance;
							/*:::消費魔力減少などによる失敗率減少*/
							chance = mod_spell_chance_1(chance);
							/*:::最低失敗率処理*/
							minfail = adj_mag_fail[p_ptr->stat_ind[use_stat]];
							if (chance < minfail) chance = minfail;

							/*:::朦朧時失敗率増加 */
							if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
							else if (p_ptr->stun) chance += 15;

							/* Always a 5 percent chance of working */
							if (chance > 95) chance = 95;
							/*:::十分なMPがないとき　どのみち実行できなくするが*/
							//v1.1.66 里乃は関係なく使える
							if (cost > p_ptr->csp && p_ptr->pclass != CLASS_SATONO)
							{
								flag_usable = FALSE;
								chance = 100;
							}
							//レベルが＠のレベルより高いと使えない。モンスター変身と里乃とものまねは例外
							if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS) && p_ptr->pclass != CLASS_SATONO && !monomane)
							{
								flag_usable = FALSE;
								chance = 100;
							}
						}
					}


					//里乃はコストと最低失敗率を別に計算する
					if (p_ptr->pclass == CLASS_SATONO)
					{
						int s_minfail = satono_spell_minfail();
						if (chance < s_minfail) chance = s_minfail;
						cost = 0;
					}


					/*:::効果欄のコメントを得る*/
					sprintf(comment, "%s", cast_monspell_new_aux(spell_list[i], TRUE, FALSE, xtra, mane_dam_list[i]));

					/*:::カーソルまたはアルファベットの表示*/
					sprintf(psi_desc, "  %c) ", I2A(i));

					//v1.1.82b 弾幕研究家　未習得の特技は隠す
					if (p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(spell_list[i]))
					{
						flag_usable = FALSE;
						strcat(psi_desc, _("(未習得)", "(unknown)"));
					}
					//v2.0.6 尤魔　未所持のパワーは隠す
					else if (p_ptr->pclass == CLASS_YUMA && !p_ptr->magic_num1[spell_list[i]])
					{
						flag_usable = FALSE;
						strcat(psi_desc, _("(未所持)", "(unavailable)"));
					}
					else
					{
						strcat(psi_desc, format("%-30s%2d  %4d    %s %3d%%  %s",
							monspell_list2[spell_list[i]].name, monspell_list2[spell_list[i]].level,
							cost, stat_desc[use_stat], chance, comment));
					}


					if (cost > p_ptr->csp)
						flag_usable = FALSE;

					///mod151001 ドレミーはレベルに関わらず技を使える
					//v1.1.45 里乃も
					//v2.1.6 ものまねも
					if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO && !monomane)
						flag_usable = FALSE;

					if (!flag_usable)
						c_prt(TERM_L_DARK, psi_desc, y + i + 1, x);
					else
						prt(psi_desc, y + i + 1, x);

				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
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

		/* Verify it */
		//v1.1.82b shiftを押しながら特技を選ぶと確認が出るが、そのとき技名が出るので弾幕研究家の未習得特技では確認しないことにする
		if (ask && !(p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(spell_list[i])))
		{
			char tmp_val[160];

			/* Prompt */
			(void) strnfmt(tmp_val, 78, _("%sを使いますか？", "Use %s?"), monspell_list2[spell_list[i]].name);

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}
		num = spell_list[i];
		mane_dam = mane_dam_list[i];
		choose_num = i;
		/* Stop the loop */
		flag_choice = TRUE;
#ifdef ALLOW_REPEAT /* TNB */
		repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */

	}

	/* Restore the screen */
	if (flag_redraw) screen_load();
	/* Show choices */
	p_ptr->window |= (PW_SPELL);
	/* Window stuff */
	window_stuff();
	/* Abort if needed */
	if (!flag_choice)
	{
		screen_load();
		return FALSE;
	}
	/*:::技の選択終了。失敗率判定へ。*/

	screen_load();

	if (p_ptr->pclass == CLASS_RAIKO)
		xtra = calc_raiko_spell_lev(choose_num);

	/*:::選択された技についてコストや失敗率を再び計算する　リピート処理があるので再利用はできない*/
	chance = monspell_list2[num].fail;

	cost = monspell_list2[num].smana;

	//v1.1.82b
	if (p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(num))
	{
		msg_format(_("まだその特技を習得していない。",
                    "You haven't learned this ability yet."));
		return FALSE;
	}
	else if (p_ptr->pclass == CLASS_YUMA && !p_ptr->magic_num1[num])
	{
		msg_format(_("今はその特技を持っていない。",
                    "You don't have this special ability right now."));
		return FALSE;
	}

	if (chance)
	{
		//v1.1.48 ドレミーの変身と紫苑の憑依
		if (IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME))
		{
			//モンスターの魔法使用時と同じ失敗率
			chance = 25 - (r_info[MON_EXTRA_FIELD].level + 3) / 4;

			if (chance < 0) chance = 0;

			if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
			else if (p_ptr->stun) chance += 15;
			/*:::十分なMPがないとき　どのみち実行できなくするが*/
			if (cost > p_ptr->csp) chance = 100;

		}
		else
		{

			int use_stat = monspell_list2[num].use_stat;
			spell_lev = monspell_list2[num].level;
			//さとりは常に知能で技を使う
			if (p_ptr->pclass == CLASS_SATORI) use_stat = A_INT;
			//里乃は常に魅力で技を使う
			if (p_ptr->pclass == CLASS_SATONO) use_stat = A_CHR;


			chance -= 3 * (p_ptr->lev - spell_lev);
			chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[use_stat]] - 1);

			chance += p_ptr->to_m_chance;
			chance = mod_spell_chance_1(chance);
			minfail = adj_mag_fail[p_ptr->stat_ind[use_stat]];
			if (chance < minfail) chance = minfail;
			if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
			else if (p_ptr->stun) chance += 15;
			if (chance > 95) chance = 95;
		}
	}
	if (cost > 0)
	{
		if (p_ptr->dec_mana) cost = cost * 3 / 4;
		if (cost < 1) cost = 1;
	}

	//里乃は独自に失敗率計算をする
	if (p_ptr->pclass == CLASS_SATONO)
	{
		int s_minfail = satono_spell_minfail();
		if (chance < s_minfail) chance = s_minfail;
		cost = 0;
	}


	/*:::MPが足りない時は強制中断　どうせオプション使わないし処理めんどい*/
	if (cost > p_ptr->csp)
	{
		msg_print(_("ＭＰが足りません。", "You don't have enough MP."));
		return FALSE;
	}
	///mod151001 ドレミー変身、里乃ダンス、紫苑憑依はレベルに関わらず技を使える
	if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO && !monomane)
	{
		msg_format(_("あなたのレベルではまだ%sできないようだ。",
                    "You cannot %s this at your current experience level."),power_desc);
		return FALSE;
	}

	stop_raiko_music();

	/*:::特技失敗処理*/
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
		msg_format(_("%sに失敗した！", "You failed to %s it!"),power_desc);
		sound(SOUND_FAIL);

		/*:::特殊な失敗ペナルティがある場合ここに書く*/
		cast_monspell_new_aux(num, FALSE, TRUE, xtra, mane_dam);
	}

	/*:::特技成功処理*/
	else
	{
		/*:::成功判定後にターゲット選択でキャンセルしたときなどにはcptrにNULLが返り、そのまま行動順消費せず終了する*/
		if (!cast_monspell_new_aux(num, FALSE, FALSE, xtra, mane_dam)) return FALSE;
	}

	//ものまねの場合MPコストなしの代わりに使った特技をリストから消去する
	if (monomane)
	{
		int j;
		p_ptr->mane_num--;
		for (j = choose_num; j < p_ptr->mane_num; j++)
		{
			if (choose_num < 0)
			{
				msg_format(_("ERROR:ものまねリスト消去前のchoose_num値がおかしい(%d)",
							"ERROR: Weird choose_num value (%d) before clearing imitator list"), j);
				return FALSE;
			}

			p_ptr->mane_spell[j] = p_ptr->mane_spell[j + 1];
			p_ptr->mane_dam[j] = p_ptr->mane_dam[j + 1];
		}

	}
	//それ以外のときMP減少処理
	else
	{
		p_ptr->csp -= cost;
		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}

	//里乃は騎乗中モンスターが技を使ったという扱いなのでモンスターに一行動分行動遅延させる。
	//もしかしたら対象の爆発などで騎乗中モンスターが死んでるかもしれんのでp_ptr->ridingを条件式に入れておく。
	if (p_ptr->pclass == CLASS_SATONO && p_ptr->riding)
	{
		monster_type* m_ptr = &m_list[p_ptr->riding];
		m_ptr->energy_need += ENERGY_NEED();
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);

	return TRUE;

}


/*:::響子のヤマビコ候補設定ルーチン モンスターのスペル実行ルーチンからthrown_spellを渡す。del:TRUEを渡したらクリア*/
/*:::スペルの番号はp_ptr->magic_num1[5]に格納。(0～2辺りは歌で使うため) 番号はmonspell_list2[1～128]の添え字として使われる。*/
//attack_spell, monst_spell_monst,記憶消去
///mod160305 アイテムカードから使うために独立変数に保存することにした
void kyouko_echo(bool del, int thrown_spell)
{
	int num = thrown_spell - 95;

	//if(p_ptr->pclass != CLASS_KYOUKO) return;

	if (del)
	{
		//		p_ptr->magic_num1[5]=0;
		monspell_yamabiko = 0;
		return;
	}
	if (num < 0 || num > 128)
	{
		msg_print(_("ERROR:kyouko_echo()で不正なthrown_spell値が渡された",
                    "ERROR: Incorrect thrown_spell value passed to kyouko_echo()"));
		return;
	}

	if (!monspell_list2[num].level || !monspell_list2[num].use_words) return; //使用不可の魔法、「言葉を使う」フラグのない魔法は非対象

	//p_ptr->magic_num1[5]=num;
	monspell_yamabiko = num;
	return;
}


//v1.1.82b ＠が特定の青魔法を習得しているかどうか判定
//今のところ「弾幕研究家」専用
//monspell_numはmonspells2_list[]の配列添字 本家青魔のMS_***とは少し違うので注意
bool check_monspell_learned(int monspell_num)
{

	//monspell_list2[]の範囲チェック
	if (monspell_num < 1 || monspell_num > MAX_MONSPELLS2)
	{
		msg_format(_("ERROR:check_monspell_learned()に不正なmonspell_num値(%d)が渡された", "ERROR: invalid monspell_num, %d, passed to check_monspell_learned()."), monspell_num);
		return FALSE;
	}

	switch (p_ptr->pclass)
	{
	case CLASS_RESEARCHER:
		{
		int spell_exp;

		//該当の特技の現在の習得度を取得する
		if (monspell_num <= 96) //1(叫び)から96(ユニークモンスター召喚)まではmagic_num1[]を使う
			spell_exp = p_ptr->magic_num1[monspell_num];
		else //97(炎の嵐)からはmagic_num2[]を使う
			spell_exp = p_ptr->magic_num2[monspell_num - 96];

		//習得度100%ならTRUE
		if (spell_exp >= 100) 	return TRUE;

		}
		break;

	default:
		msg_print(_("ERROR:このクラスでのcheck_monspell_learned()の処理が定義されていない",
                    "ERROR: Undefined logic in check_monspell_learned() for this class"));
		break;
	}



	return FALSE;

}


//v1.1.82 新しい特技習得処理　職業「弾幕研究家」用
//いずれ魔理沙専用性格でも使えるようにする？
//thrown_spell:make_attack_spell()とmonst_spell_monst()内での特技インデックス monspell2_list[]のインデックス値+95
//m_ptr 特技を使用したモンスター
//flag_direct モンスターが＠をターゲットに使ったかどうか 攻撃だけでなく補助も含む make_attack_spell()から呼ばれたときTRUE
//flag_look そのモンスターが特技を使う瞬間に＠がそのモンスターを見ていたときTRUE　感知しているだけでなく直接見えていること
//caster_dist そのモンスターが魔法を使った瞬間の＠からの距離　テレポートなど距離が変動する特技に対応
void learn_monspell_new(int thrown_spell, monster_type *m_ptr, bool flag_direct, bool flag_look, int caster_dist)
{

	int monspell_num = thrown_spell - 95;
	int spell_level, spell_fail;
	cptr spell_name;
	int spell_dist;

	int spell_exp;
	int gain_exp;

	if (p_ptr->pclass != CLASS_RESEARCHER)
	{
		msg_format(_("ERROR:登録されていないクラスでnew_learn_spell()が呼ばれた",
                    "ERROR: new_learn_spell() called for an unexpected class"));
		return;
	}
	//monspell_list2[]の範囲チェック
	if (monspell_num < 1 || monspell_num > MAX_MONSPELLS2)
	{
		msg_format(_("ERROR:new_learn_spell()に不正なthrown_spell値(%d)が渡された",
                    "ERROR: Incorrect thrown_spell value (%d) passed to new_learn_spell()"), thrown_spell);
		return;
	}

	//盲目混乱行動不能時には学習できない
	if (p_ptr->blind || p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
		return;
	if (p_ptr->is_dead) return;

	//特別な行動は対象外
	if (monspell_num == 29 || monspell_num == 72)
	{
		if (p_ptr->wizard) msg_print(_("(特別な行動は習得できない)", "(cannot learn special actions)"));
		return;
	}

	//@が見ていないところで使われた技は習得できない。ただし視界外隣接テレポ(126)が＠に使われたときを除く。しかしこのマジックナンバー連発は何とかならんか
	if (!flag_look && !(monspell_num == 126 && flag_direct))
	{
		return;
	}

	//該当の特技の現在の習得度を取得する
	if (monspell_num <= 96) //1(叫び)から96(ユニークモンスター召喚)まではmagic_num1[]を使う
		spell_exp = p_ptr->magic_num1[monspell_num];
	else //97(炎の嵐)からはmagic_num2[]を使う
		spell_exp = p_ptr->magic_num2[monspell_num - 96];

	//習得度が100(習得済み)なら終了
	if (spell_exp >= 100) return;

	//該当特技のパラメータを取得
	spell_level = monspell_list2[monspell_num].level;
	spell_fail = monspell_list2[monspell_num].fail;
	spell_name = monspell_list2[monspell_num].name;
	spell_dist = caster_dist;

	if (spell_dist < 1) spell_dist = 1; //騎乗中モンスターが使った時は距離1として扱う

	//レベル不足
	if (p_ptr->lev < spell_level)
	{
		msg_print(_("この特技を習得するにはまだ修練が足りないようだ。",
                    "You're not experienced enough to learn this ability."));
		return;
	}

	//獲得経験値=10 + 1d10 + ＠のレベル-技レベル+＠の知能*2-技難易度-距離
	gain_exp = 10 + randint1(10) + p_ptr->lev - spell_level + (p_ptr->stat_ind[A_INT] + 3) * 2 - spell_fail - (spell_dist - 1);

	//隣で見れば+10,かつ最低1は入ることにしておく
	if (spell_dist <= 1)
	{
		gain_exp += 10;
		if (gain_exp < 1) gain_exp = 1;
	}
	//幸運判定
	if (weird_luck()) gain_exp = gain_exp * 2 + randint1(10);

	//性格狂気の場合明らかに強くなりすぎるので経験値1/10
	if (p_ptr->pseikaku == SEIKAKU_BERSERK) gain_exp = (gain_exp + 9) / 5;

	if (gain_exp<1)
	{
		msg_print(_("もっと近くで見ないとよく分からない。",
                    "You have to get closer to understand how it works."));
		return;
	}

	//敵対モンスターが自分と戦っているときに撃った技(具体的にはmake_attack_spell()を通る処理)でないと習熟度が75%を超えないことにする。
	//ペットやガラスvaultで簡単に習得できるのを防ぐため。
	//v1.1.83 新アリーナ(夢日記)も追加。忘れてた。
	if (!flag_direct || p_ptr->inside_arena)
	{
		if (spell_exp >= 75)
		{
			msg_print(_("これ以上は実戦でないと学べない。",
                        "You can't learn more without actual combat."));
			return;
		}
		else if ((spell_exp + gain_exp) > 75)
		{
			gain_exp = 75 - spell_exp;
		}
	}

	if (p_ptr->wizard) msg_format("learn-num:%d exp:%d+%d", monspell_num, spell_exp, gain_exp);

	//習得度加算とメッセージ
	spell_exp += gain_exp;
	if (spell_exp>100) spell_exp = 100;

	if (spell_exp < 25) msg_print(_("まだ習得の取っ掛かりすら掴めない。", "You're not even getting started."));
	else if (spell_exp < 50)msg_print(_("少し特徴が分かってきた。", "You understand it a little."));
	else if (spell_exp < 75)msg_print(_("コツを掴んだ気がする！", "You feel you're getting the hang of it!"));
	else if (spell_exp < 100)msg_print(_("もう少しで習得できそうだ！", "You think you'll learn it soon!"));
	else msg_format(_("「%s」を習得した！", "Learned '%s'!"),spell_name);

	//加算済みの習得度を記録
	if (monspell_num <= 96) p_ptr->magic_num1[monspell_num] = (s16b)spell_exp;
	else p_ptr->magic_num2[monspell_num - 96] = (byte)spell_exp;


}

