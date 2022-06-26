/* File: do-spell.c */

/* Purpose: Do everything for each spell */

#include "angband.h"


/*
 * Generate dice info string such as "foo 2d10"
 */
static cptr info_string_dice(cptr str, int dice, int sides, int base)
{
	/* Fix value */
	if (!dice)
		return format("%s%d", str, base);

	/* Dice only */
	else if (!base)
		return format("%s%dd%d", str, dice, sides);

	/* Dice plus base value */
	else
		return format("%s%dd%d%+d", str, dice, sides, base);
}


/*
 * Generate damage-dice info string such as "dam 2d10"
 */
static cptr info_damage(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("損傷:", dice, sides, base);
#else
	return info_string_dice("dam ", dice, sides, base);
#endif
}


/*
 * Generate duration info string such as "dur 20+1d20"
 */
static cptr info_duration(int base, int sides)
{
#ifdef JP
	return format("期間:%d+1d%d", base, sides);
#else
	return format("dur %d+1d%d", base, sides);
#endif
}


/*
 * Generate range info string such as "range 5"
 */
static cptr info_range(int range)
{
#ifdef JP
	return format("範囲:%d", range);
#else
	return format("range %d", range);
#endif
}


/*
 * Generate heal info string such as "heal 2d8"
 */
static cptr info_heal(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("回復:", dice, sides, base);
#else
	return info_string_dice("heal ", dice, sides, base);
#endif
}


/*
 * Generate delay info string such as "delay 15+1d15"
 */
static cptr info_delay(int base, int sides)
{
#ifdef JP
	return format("遅延:%d+1d%d", base, sides);
#else
	return format("delay %d+1d%d", base, sides);
#endif
}


/*
 * Generate multiple-damage info string such as "dam 25 each"
 */
static cptr info_multi_damage(int dam)
{
#ifdef JP
	return format("損傷:各%d", dam);
#else
	return format("dam %d each", dam);
#endif
}


/*
 * Generate multiple-damage-dice info string such as "dam 5d2 each"
 */
static cptr info_multi_damage_dice(int dice, int sides)
{
#ifdef JP
	return format("損傷:各%dd%d", dice, sides);
#else
	return format("dam %dd%d each", dice, sides);
#endif
}


/*
 * Generate power info string such as "power 100"
 */
static cptr info_power(int power)
{
#ifdef JP
	return format("効力:%d", power);
#else
	return format("power %d", power);
#endif
}


/*
 * Generate power info string such as "power 1d100"
 */
static cptr info_power_dice(int dice, int sides)
{
#ifdef JP
	return format("効力:%dd%d", dice, sides);
#else
	return format("power %dd%d", dice, sides);
#endif
}


/*
 * Generate radius info string such as "rad 100"
 */
static cptr info_radius(int rad)
{
#ifdef JP
	return format("半径:%d", rad);
#else
	return format("rad %d", rad);
#endif
}


/*
 * Generate weight info string such as "max wgt 15"
 */
static cptr info_weight(int weight)
{
#ifdef JP
//	return format("最大重量:%d.%dkg", lbtokg1(weight/10), lbtokg2(weight/10));
	///mod151024 よく分からんが/10を取った
	return format("最大重量:%d.%dkg", lbtokg1(weight), lbtokg2(weight));
#else
	return format("max wgt %d.%dkg", lbtokg1(weight), lbtokg2(weight));
#endif
}


/*
 * Prepare standard probability to become beam for fire_bolt_or_beam()
 */
static int beam_chance(void)
{

	if (p_ptr->pclass == CLASS_MAGE)
		return p_ptr->lev;
	if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER)
		return p_ptr->lev + 10;

	if(cp_ptr->magicmaster) return p_ptr->lev + 5;

	return p_ptr->lev / 2;
}


/*
 * Handle summoning and failure of trump spells
 */
static bool trump_summoning(int num, bool pet, int y, int x, int lev, int type, u32b mode)
{
	int plev = p_ptr->lev;

	int who;
	int i;
	bool success = FALSE;

	/* Default level */
	if (!lev) lev = plev * 2 / 3 + randint1(plev / 2);

	if (pet)
	{
		/* Become pet */
		mode |= PM_FORCE_PET;

		/* Only sometimes allow unique monster */
		if (mode & PM_ALLOW_UNIQUE)
		{
			/* Forbid often */
			if (randint1(50 + plev) >= plev / 10)
				mode &= ~PM_ALLOW_UNIQUE;
		}

		/* Player is who summons */
		who = -1;
	}
	else
	{
		/* Prevent taming, allow unique monster */
		mode |= PM_NO_PET;

		/* Behave as if they appear by themselfs */
		who = 0;
	}

	for (i = 0; i < num; i++)
	{
		if (summon_specific(who, y, x, lev, type, mode))
			success = TRUE;
	}

	if (!success)
	{
#ifdef JP
		msg_print("誰もあなたのカードの呼び声に答えない。");
#else
		msg_print("Nobody answers to your Trump call.");
#endif
	}

	return success;
}


/*
 * This spell should become more useful (more controlled) as the
 * player gains experience levels.  Thus, add 1/5 of the player's
 * level to the die roll.  This eliminates the worst effects later on,
 * while keeping the results quite random.  It also allows some potent
 * effects only at high level.
 */
static void cast_wonder(int dir)
{
	int plev = p_ptr->lev;
	int die = randint1(100) + plev / 5;
	//int vir = virtue_number(V_CHANCE);
	/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
	*/
	//if (die < 26)
	//	chg_virtue(V_CHANCE, 1);

	if (die > 100)
	{
#ifdef JP
		msg_print("あなたは力がみなぎるのを感じた！");
#else
		msg_print("You feel a surge of power!");
#endif
	}

	if (die < 8) clone_monster(dir);
	else if (die < 14) speed_monster(dir, plev);
	else if (die < 26) heal_monster(dir, damroll(4, 6));
	else if (die < 31) poly_monster(dir, plev);
	else if (die < 36)
		fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir,
				  damroll(3 + ((plev - 1) / 5), 4));
	else if (die < 41) confuse_monster(dir, plev);
	else if (die < 46) fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
	else if (die < 51) (void)lite_line(dir, damroll(6, 8));
	else if (die < 56)
		fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir,
				  damroll(3 + ((plev - 5) / 4), 8));
	else if (die < 61)
		fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir,
				  damroll(5 + ((plev - 5) / 4), 8));
	else if (die < 66)
		fire_bolt_or_beam(beam_chance(), GF_ACID, dir,
				  damroll(6 + ((plev - 5) / 4), 8));
	else if (die < 71)
		fire_bolt_or_beam(beam_chance(), GF_FIRE, dir,
				  damroll(8 + ((plev - 5) / 4), 8));
	else if (die < 76) drain_life(dir, 75);
	else if (die < 81) fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
	else if (die < 86) fire_ball(GF_ACID, dir, 40 + plev, 2);
	else if (die < 91) fire_ball(GF_ICE, dir, 70 + plev, 3);
	else if (die < 96) fire_ball(GF_FIRE, dir, 80 + plev, 3);
	else if (die < 101) drain_life(dir, 100 + plev);
	else if (die < 104)
	{
		earthquake(py, px, 12);
	}
	else if (die < 106)
	{
		(void)destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE);
	}
	else if (die < 108)
	{
		symbol_genocide(plev+50, TRUE,0);
	}
	else if (die < 110) dispel_monsters(120);
	else /* RARE */
	{
		dispel_monsters(150);
		slow_monsters(plev);
		sleep_monsters(plev);
		hp_player(300);
	}
}


//v1.1.14 天照大御神降臨の実行部　
//依姫特技と魔法で二回使うし天宇受売命で分岐するのでルーチン分割して共有化した
void call_amaterasu(void)
{
	int plev = p_ptr->lev;

	if(p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];
		int i;
		int delay;
		set_kamioroshi(KAMIOROSHI_AMATERASU, 0);
		msg_print(_("突如、ダンジョンが太陽のような眩い光に覆われた！",
                    "Suddenly, light as bright as the sun fills the dungeon!"));
		project(0, 3, py, px, plev*24, GF_HOLY_FIRE, PROJECT_KILL, -1);
		project(0, 8, py, px, plev*20, GF_DISP_ALL, PROJECT_KILL, -1);
		project_hack2(GF_LITE, 0,0,plev * 10);
		stun_monsters(plev * 6);
		for (i = 1; (i < m_max); i++)
		{
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];
			if(p_ptr->riding == i) continue;
			if(r_ptr->flagsr & RFR_RES_LITE) continue;
			if(m_ptr->cdis > 3) continue;
			monster_desc(m_name, m_ptr, 0);
			delay = 200 / (m_ptr->cdis+1);
			if(r_ptr->flagsr & RFR_HURT_LITE) delay *= 2;
			m_ptr->energy_need += delay;
			msg_format(_("%sはあまりの光に怯んでいる！", "%s falters from the light!"),m_name);

		}
		wiz_lite(FALSE);
	}
	else
	{
		set_kamioroshi(KAMIOROSHI_AMATERASU, 0);
		msg_print(_("天照大御神が現れ、眩い光を放った！",
                    "Amaterasu Oomikami appears, radiating dazzling light!"));
		project(0, 1, py, px, plev*12, GF_HOLY_FIRE, PROJECT_KILL, -1);
		project(0, 7, py, px, plev*10, GF_DISP_ALL, PROJECT_KILL, -1);
		project_hack2(GF_LITE, 0,0,plev * 5);
		stun_monsters(plev * 6);
	}

}


/*:::暗黒領域の悪霊召喚*/
static void cast_invoke_spirits(int dir)
{
	int plev = p_ptr->lev;
	int die = randint1(100) + plev / 5;
	///del131214 virtue
	//int vir = virtue_number(V_CHANCE);
	/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
	*/
#ifdef JP
	msg_print("あなたは死者たちの力を招集した...");
#else
	msg_print("You call on the power of the dead...");
#endif
	///del131214 virtue
	//if (die < 26)
	//chg_virtue(V_CHANCE, 1);

	if (die > 100)
	{
#ifdef JP
		msg_print("あなたはおどろおどろしい力のうねりを感じた！");
#else
		msg_print("You feel a surge of eldritch force!");
#endif
	}


	if (die < 8)
	{
#ifdef JP
		msg_print("なんてこった！あなたの周りの地面から朽ちた人影が立ち上がってきた！");
#else
		msg_print("Oh no! Mouldering forms rise from the earth around you!");
#endif

		(void)summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			///del131214 virtue
		//chg_virtue(V_UNLIFE, 1);
	}
	else if (die < 14)
	{
#ifdef JP
		msg_print("名状し難い邪悪な存在があなたの心を通り過ぎて行った...");
#else
		msg_print("An unnamable evil brushes against your mind...");
#endif

		set_afraid(p_ptr->afraid + randint1(4) + 4);
	}
	else if (die < 26)
	{
#ifdef JP
		msg_print("あなたの頭に大量の幽霊たちの騒々しい声が押し寄せてきた...");
#else
		msg_print("Your head is invaded by a horde of gibbering spectral voices...");
#endif

		set_confused(p_ptr->confused + randint1(4) + 4);
	}
	else if (die < 31)
	{
		poly_monster(dir, plev);
	}
	else if (die < 36)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir,
				  damroll(3 + ((plev - 1) / 5), 4));
	}
	else if (die < 41)
	{
		confuse_monster (dir, plev);
	}
	else if (die < 46)
	{
		fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
	}
	else if (die < 51)
	{
		(void)lite_line(dir, damroll(6, 8));
	}
	else if (die < 56)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir,
				  damroll(3+((plev-5)/4),8));
	}
	else if (die < 61)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir,
				  damroll(5+((plev-5)/4),8));
	}
	else if (die < 66)
	{
		fire_bolt_or_beam(beam_chance(), GF_ACID, dir,
				  damroll(6+((plev-5)/4),8));
	}
	else if (die < 71)
	{
		fire_bolt_or_beam(beam_chance(), GF_FIRE, dir,
				  damroll(8+((plev-5)/4),8));
	}
	else if (die < 76)
	{
		drain_life(dir, 75);
	}
	else if (die < 81)
	{
		fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
	}
	else if (die < 86)
	{
		fire_ball(GF_ACID, dir, 40 + plev, 2);
	}
	else if (die < 91)
	{
		fire_ball(GF_ICE, dir, 70 + plev, 3);
	}
	else if (die < 96)
	{
		fire_ball(GF_FIRE, dir, 80 + plev, 3);
	}
	else if (die < 101)
	{
		drain_life(dir, 100 + plev);
	}
	else if (die < 104)
	{
		earthquake(py, px, 12);
	}
	else if (die < 106)
	{
		(void)destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE);
	}
	else if (die < 108)
	{
		symbol_genocide(plev+50, TRUE,0);
	}
	else if (die < 110)
	{
		dispel_monsters(120);
	}
	else
	{ /* RARE */
		dispel_monsters(150);
		slow_monsters(plev);
		sleep_monsters(plev);
		hp_player(300);
	}

	if (die < 31)
	{
#ifdef JP
		msg_print("陰欝な声がクスクス笑う。「もうすぐおまえは我々の仲間になるだろう。弱き者よ。」");
#else
		msg_print("Sepulchral voices chuckle. 'Soon you will join us, mortal.'");
#endif
	}
}

/*:::カオス領域失敗ペナルティ、トランプ領域シャッフル効果など・・というかcmd5.cとdo-spell.cの両方にある？*/
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
}

/*:::トランプ魔法のシャッフル*/
static void cast_shuffle(void)
{
	int plev = p_ptr->lev;
	int dir;
	int die;
	///del131214 virtue
	//int vir = virtue_number(V_CHANCE);
	int i;

	/* Card sharks and high mages get a level bonus */
	///class シャッフル魔法へのボーナス
	if (cp_ptr->magicmaster)
		die = (randint1(110)) + plev / 5;
	else
		die = randint1(120);

	set_deity_bias(DBIAS_COSMOS, -1);


			///del131214 virtue
/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
*/
#ifdef JP
	msg_print("あなたはカードを切って一枚引いた...");
#else
	msg_print("You shuffle the deck and draw a card...");
#endif
	///del131214 virtue
	//if (die < 30)
	//	chg_virtue(V_CHANCE, 1);

	if (die < 7)
	{
#ifdef JP
		msg_print("なんてこった！《死》だ！");
#else
		msg_print("Oh no! It's Death!");
#endif

		for (i = 0; i < randint1(3); i++)
			activate_hi_summon(py, px, FALSE);
	}
	else if (die < 14)
	{
#ifdef JP
		msg_print("なんてこった！《悪魔》だ！");
#else
		msg_print("Oh no! It's the Devil!");
#endif

		summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
	}
	else if (die < 18)
	{
		int count = 0;
#ifdef JP
		msg_print("なんてこった！《吊られた男》だ！");
#else
		msg_print("Oh no! It's the Hanged Man.");
#endif

		activate_ty_curse(FALSE, &count);
	}
	else if (die < 22)
	{
#ifdef JP
		msg_print("《不調和の剣》だ。");
#else
		msg_print("It's the swords of discord.");
#endif

		aggravate_monsters(0,TRUE);
	}
	else if (die < 26)
	{
#ifdef JP
		msg_print("《愚者》だ。");
#else
		msg_print("It's the Fool.");
#endif

		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
	}
	else if (die < 30)
	{
#ifdef JP
		msg_print("奇妙なモンスターの絵だ。");
#else
		msg_print("It's the picture of a strange monster.");
#endif

		trump_summoning(1, FALSE, py, px, (dun_level * 3 / 2), (32 + randint1(6)), PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
	}
	else if (die < 33)
	{
#ifdef JP
		msg_print("《月》だ。");
#else
		msg_print("It's the Moon.");
#endif

		unlite_area(10, 3);
	}
	else if (die < 38)
	{
#ifdef JP
		msg_print("《運命の輪》だ。");
#else
		msg_print("It's the Wheel of Fortune.");
#endif

		wild_magic(randint0(32));
	}
	else if (die < 40)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(10, TELEPORT_PASSIVE);
	}
	else if (die < 42)
	{
#ifdef JP
		msg_print("《正義》だ。");
#else
		msg_print("It's Justice.");
#endif

		set_blessed(p_ptr->lev, FALSE);
	}
	else if (die < 47)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(100, TELEPORT_PASSIVE);
	}
	else if (die < 52)
	{
#ifdef JP
		msg_print("テレポート・カードだ。");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(200, TELEPORT_PASSIVE);
	}
	else if (die < 60)
	{
#ifdef JP
		msg_print("《塔》だ。");
#else
		msg_print("It's the Tower.");
#endif

		wall_breaker();
	}
	else if (die < 72)
	{
#ifdef JP
		msg_print("《節制》だ。");
#else
		msg_print("It's Temperance.");
#endif

		sleep_monsters_touch();
	}
	else if (die < 80)
	{
#ifdef JP
		msg_print("《塔》だ。");
#else
		msg_print("It's the Tower.");
#endif

		earthquake(py, px, 5);
	}
	else if (die < 82)
	{
#ifdef JP
		msg_print("友好的なモンスターの絵だ。");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE1, 0L);
	}
	else if (die < 84)
	{
#ifdef JP
		msg_print("友好的なモンスターの絵だ。");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE2, 0L);
	}
	else if (die < 86)
	{
#ifdef JP
		msg_print("友好的なモンスターの絵だ。");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE4, 0L);
	}
	else if (die < 88)
	{
#ifdef JP
		msg_print("友好的なモンスターの絵だ。");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE5, 0L);
	}
	else if (die < 96)
	{
#ifdef JP
		msg_print("《恋人》だ。");
#else
		msg_print("It's the Lovers.");
#endif

		if (get_aim_dir(&dir))
			charm_monster(dir, MIN(p_ptr->lev, 20));
	}
	else if (die < 101)
	{
#ifdef JP
		msg_print("《隠者》だ。");
#else
		msg_print("It's the Hermit.");
#endif

		wall_stone();
	}
	else if (die < 111)
	{
#ifdef JP
		msg_print("《審判》だ。");
#else
		msg_print("It's the Judgement.");
#endif

		do_cmd_rerate(FALSE);
		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
		{
#ifdef JP
			if(muta_erasable_count()) msg_print("全ての突然変異が治った。");
#else
			msg_print("You are cured of all mutations.");
#endif

			p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
			///mod141204 永続変異適用
			p_ptr->muta1 = p_ptr->muta1_perma;
			p_ptr->muta2 = p_ptr->muta2_perma;
			p_ptr->muta3 = p_ptr->muta3_perma;
			p_ptr->muta4 = p_ptr->muta4_perma;
			p_ptr->update |= PU_BONUS;
			handle_stuff();
		}
	}
	else if (die < 120)
	{
#ifdef JP
		msg_print("《太陽》だ。");
#else
		msg_print("It's the Sun.");
#endif
		///del131214 virtue
		//chg_virtue(V_KNOWLEDGE, 1);
		//chg_virtue(V_ENLIGHTEN, 1);
		wiz_lite(FALSE);
	}
	else
	{
#ifdef JP
		msg_print("《世界》だ。");
#else
		msg_print("It's the World.");
#endif

		if (p_ptr->exp < PY_MAX_EXP)
		{
			s32b ee = (p_ptr->exp / 25) + 1;
			if (ee > 5000) ee = 5000;
#ifdef JP
			msg_print("更に経験を積んだような気がする。");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(ee);
		}
	}
}


/*:::スプリングフロア　行動順消費する場合TRUEが返る*/
bool cast_spring_floor(int dist)
{
	s16b this_o_idx, next_o_idx = 0;
	int dir,target_dir;
	int tx = 0, ty = 0;
	msg_print(_("どこの床を跳ね上げますか？(方角指定した場合プレイヤーの隣になります)",
                "Catapult floor at which point? (direction key targets adjacent tile)"));
	if (!get_aim_dir(&target_dir)) return FALSE;

	tx = px + ddx[target_dir];
	ty = py + ddy[target_dir];

	if ((target_dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}
	if (!in_bounds(ty, tx)) return FALSE;
	if( !player_has_los_bold(ty, tx))
	{
		msg_print(_("そこはあなたからは見えない。", "You can't see there."));
		return FALSE;
	}
	if(!cave_have_flag_bold(ty, tx, FF_FLOOR))
	{
		msg_print(_("そこは床ではない。", "There's no floor here"));
		return FALSE;
	}
	while(1)
	{
		char c;

		if(!get_com(_("どちらに向けて跳ね上げますか？(1-4,5-9)",
                        "Catapult it in which direction? (1-4, 5-9)"),&c,(TRUE))) return FALSE;
		dir = get_keymap_dir(c);
		if( dir < 1 || dir == 5 || dir > 9) continue;
		break;

	}
	if(cave[ty][tx].info & CAVE_ICKY)
	{
		msg_print(_("そこの床は動かなかった。", "The floor doesn't move."));
		return TRUE;
	}
	else
	{
		msg_print(_("突如、床が発条仕掛けで跳ね上がった！", "The floor suddenly springs forth!"));
	}
	/*:::対象グリッドのモンスターに対する処理*/
	if(cave[ty][tx].m_idx)
	{
		int m_idx = cave[ty][tx].m_idx;
		int i;
		monster_type *m_ptr = &m_list[m_idx];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		char m_name[80];
		monster_desc(m_name, m_ptr, 0);
		(void)set_monster_csleep(m_idx, 0);
		if( (r_ptr->flags7 & RF7_CAN_FLY) )
		{
			msg_format(_("%sは跳ね上がった床の上を悠々と飛んでいる・・",
                        "%s is flying above the floor..."),m_name);
		}
		else if( (r_ptr->flags2 & RF2_GIGANTIC) || (r_ptr->flags2 & RF2_POWERFUL)&&r_ptr->level > randint1(p_ptr->lev) )
		{
			msg_format(_("%sは跳ね上がりかけた床を踏み潰した！",
                        "%s tramples the spring-loaded floor!"),m_name);
			return TRUE;
		}
		else
		{
			int dx=tx,dy=ty;
			bool move = FALSE;
			msg_format(_("%sは飛び出した床に跳ね飛ばされた！",
                        "%s is catapulted away by the floor!"),m_name);
			for (i = 0; i < dist; i++)
			{
				if (cave_empty_bold(dy+ddy[dir], dx+ddx[dir]))
				{
					dy += ddy[dir];
					dx += ddx[dir];
					move = TRUE;
				}
				else break;
			}
			if(move)
			{
				cave[ty][tx].m_idx=0;
				cave[dy][dx].m_idx=m_idx;
				m_ptr->fy = dy;
				m_ptr->fx = dx;
				update_mon(m_idx, TRUE);
				lite_spot(dy, dx);
				lite_spot(ty, tx);
				if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
				p_ptr->update |= (PU_MON_LITE);
			}
			project(0, 0, dy, dx, damroll(10,10), GF_MISSILE,PROJECT_KILL, -1);
		}
	}
	/*:::@に対する処理*/
	if(player_bold(ty,tx))
	{
		int i;
		int dx=tx,dy=ty;
		bool move = FALSE;
		msg_format(_("あなたは飛び出した床に跳ね飛ばされた！",
                    "You are catapulted away by the floor!"));
		for (i = 0; i < dist; i++)
		{
			if (cave_empty_bold(dy+ddy[dir], dx+ddx[dir]))
			{
				dy += ddy[dir];
				dx += ddx[dir];
				move = TRUE;
			}
			else break;
		}
		if(move)
		{
			teleport_player_to(dy,dx,TELEPORT_NONMAGICAL);
		}
		take_hit(DAMAGE_ATTACK,damroll(10,10),_("自分が跳ねあげた床", "catapulting floor"),-1);
	}
	//アイテム未処理　ターゲットグリッド近辺へまとめて落とす。消滅確率あり。
	if(cave[ty][tx].o_idx)
	{
		int i;
		int dx=tx,dy=ty;
		msg_format(_("床の上にあったアイテムが跳ね飛んだ！", "The items on the floor get thrown away!"));
		for (i = 0; i < dist; i++)
		{
			if ( cave_have_flag_bold(dy+ddy[dir], dx+ddx[dir],FF_PROJECT))
			{
				dy += ddy[dir];
				dx += ddx[dir];
			}
			else break;
		}


		for (this_o_idx = cave[ty][tx].o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_type *o_ptr;
			o_ptr = &o_list[this_o_idx];
			next_o_idx = o_ptr->next_o_idx;

			object_copy(q_ptr, o_ptr);
			(void)drop_near(q_ptr, 10, dy, dx);

			floor_item_increase(this_o_idx,-255);
			floor_item_optimize(this_o_idx);
		}
	}
	return TRUE;
}


/*
 * Drop 10+1d10 meteor ball at random places near the player
 */
///mod140512 属性変更可能にした。
void cast_meteor(int dam, int rad, int typ)
{
	int i;
	int b = 10 + randint1(10);

	int typ_use;


	for (i = 0; i < b; i++)
	{
		int y, x;
		int count;

		for (count = 0; count <= 20; count++)
		{
			int dy, dx, d;

			x = px - 8 + randint0(17);
			y = py - 8 + randint0(17);

			dx = (px > x) ? (px - x) : (x - px);
			dy = (py > y) ? (py - y) : (y - py);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

			if (d >= 9) continue;

			if (!in_bounds(y, x) || !projectable(py, px, y, x)
			    || !cave_have_flag_bold(y, x, FF_PROJECT)) continue;

			/* Valid position */
			break;
		}

		if (count > 20) continue;

		if(!typ) //typが0なら火冷電のランダムに
		{
			if(one_in_(3)) typ_use = GF_FIRE;
			else if(one_in_(2)) typ_use = GF_COLD;
			else typ_use = GF_ELEC;
		}
		else typ_use = typ;


		project(0, rad, y, x, dam, typ_use, PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM, -1);
	}
}


/*
 * Drop 10+1d10 disintegration ball at random places near the target
 */
/*:::aim=FALSEで呼ぶと自分中心に発生するようにした*/
bool cast_wrath_of_the_god(int dam, int rad, bool aim)
{
	int x, y, tx, ty;
	int nx, ny;
	int dir, i;
	int b = 10 + randint1(10);

	if(aim)
	{
		if (!get_aim_dir(&dir)) return FALSE;
		/* Use the given direction */
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		/* Hack -- Use an actual "target" */
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
	}
	else
	{
		tx = px;
		ty = py;
	}

	x = px;
	y = py;

	while (1)
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		ny = y;
		nx = x;
		mmove2(&ny, &nx, py, px, ty, tx);

		/* Stop at maximum range */
		if (MAX_RANGE <= distance(py, px, ny, nx)) break;

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny, nx, FF_PROJECT)) break;

		/* Stopped by monsters */
		if ((dir != 5) && cave[ny][nx].m_idx != 0) break;

		/* Save the new location */
		x = nx;
		y = ny;
	}
	tx = x;
	ty = y;

	for (i = 0; i < b; i++)
	{
		int count = 20, d = 0;

		while (count--)
		{
			int dx, dy;

			x = tx - 5 + randint0(11);
			y = ty - 5 + randint0(11);

			dx = (tx > x) ? (tx - x) : (x - tx);
			dy = (ty > y) ? (ty - y) : (y - ty);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
			/* Within the radius */
			if (d < 5) break;
		}

		if (count < 0) continue;

		/* Cannot penetrate perm walls */
		if (!in_bounds(y,x) ||
		    cave_stop_disintegration(y,x) ||
		    !in_disintegration_range(ty, tx, y, x))
			continue;

		project(0, rad, y, x, dam, GF_DISINTEGRATE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
	}

	return TRUE;
}


/*
 * An "item_tester_hook" for offer
 */
///del131221 item death グレーターデーモン召喚の生贄の死体判定
#if 0
static bool item_tester_offer(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval != TV_CORPSE) return (FALSE);

	if (o_ptr->sval != SV_CORPSE) return (FALSE);

	if (my_strchr("pht", r_info[o_ptr->pval].d_char)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}
#endif

/*
 * Daemon spell Summon Greater Demon
 */
///sys item グレーターデーモン召喚
static bool cast_summon_greater_demon(void)
{
	int plev = p_ptr->lev;
	int item;
	cptr q, s;
	int summon_lev;
	object_type *o_ptr;
	///del131221 死体廃止と共にグレーターデーモン召喚一時無効化
	msg_format(_("死体は廃止された。", "Corpses got deprecated."));
	return FALSE;


	//item_tester_hook = item_tester_offer;
#ifdef JP
	q = "どの死体を捧げますか? ";
	s = "捧げられる死体を持っていない。";
#else
	q = "Sacrifice which corpse? ";
	s = "You have nothing to scrifice.";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

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

	summon_lev = plev * 2 / 3 + r_info[o_ptr->pval].level;

	if (summon_specific(-1, py, px, summon_lev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP | PM_FORCE_PET)))
	{
#ifdef JP
		msg_print("硫黄の悪臭が充満した。");
#else
		msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


#ifdef JP
		msg_print("「ご用でございますか、ご主人様」");
#else
		msg_print("'What is thy bidding... Master?'");
#endif

		/* Decrease the item (from the pack) */
		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}

		/* Decrease the item (from the floor) */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}
	}
	else
	{
#ifdef JP
		msg_print("悪魔は現れなかった。");
#else
		msg_print("No Greater Demon arrive.");
#endif
	}

	return TRUE;
}


/*
 * Start singing if the player is a Bard
 */
/*:::吟遊詩人の歌の開始処理*/
static void start_singing(int spell, int song)
{
	/* Remember the song index */
	p_ptr->magic_num1[0] = song;

	/* Remember the index of the spell which activated the song */
	p_ptr->magic_num2[0] = spell;


	/* Now the player is singing */
	set_action(ACTION_SING);


	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


/*
 * Stop singing if the player is a Bard
 */

///mod140817 歌関係に吟遊詩人以外の職も便乗
void stop_singing(void)
{

	//if (p_ptr->pclass != CLASS_BARD) return;
	if (!(CHECK_MUSIC_CLASS)) return;

	//弁々と八橋はこの条件では音楽終了しない
	//雷鼓も
	if (p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI || p_ptr->pclass == CLASS_RAIKO) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}

	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;

	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);

	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}

/*:::九十九姉妹専用のstop_singing()*/
void stop_tsukumo_music(void)
{

	if (p_ptr->pclass != CLASS_BENBEN && p_ptr->pclass != CLASS_YATSUHASHI) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}

	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;

	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);

	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


/*:::雷鼓専用のstop_singing() 音楽停止条件を分けるため関数分割　stop_singingにmodeとか引数つけてそれで分けたほうが良かった気がするが今更だ。*/
void stop_raiko_music(void)
{

	if (p_ptr->pclass != CLASS_RAIKO) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}
	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;
	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);
	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}




/*:::召喚用の報酬選定ルーチン 呼ぶ前にitem_tester_hookを設定要　適したアイテムがないかキャンセルしたときFALSEを返す。アイテムを選択したら一つ減らす。*/
bool select_pay(int *cost)
{
	cptr q,s;
	int item;
	object_type *o_ptr;

	if(!item_tester_hook && !item_tester_tval)
	{
		msg_print(_("ERROR:召喚代償選定hook/tval未設定",
                    "ERROR: Undefined hook/tval for summoning payment"));
		return FALSE;
	}
	q = _("召喚の代償として何を捧げますか? ", "Offer what item as payment?");
	s = _("あなたは召喚の代償となるものを持っていない。", "You don't have anything to offer.");
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
	if (item >= 0)
	{
		o_ptr = &inventory[item];
		*cost = (int)object_value_real(o_ptr);
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		o_ptr = &o_list[0 - item];
		*cost = (int)object_value_real(o_ptr);
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	return TRUE;
}


/*
 * Handle summoning and failure of trump spells
 */
/*:::Trump_summoning()を少し改変
 *::: 引数petを無くしFORCE_PETとALLOW_UNIQUEは外で指定
 *::: 召喚modeとしてFORCE_FRIENDLYも追加する
 *::: */
static bool new_summoning(int num, int y, int x, int lev, int type, u32b mode)
{
	int plev = p_ptr->lev;

	int who;
	int i;
	bool success = FALSE;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev *= 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	/* Default level */
	if (!lev) lev = plev * 2 / 3 + randint1(plev / 2);

	/*:::失敗時はハイレベルな敵が出る*/
	if(mode & PM_NO_PET)
	{
		mode |= PM_ALLOW_UNIQUE;
		lev += randint1(lev / 2);
	}
	if(lev > 120) lev = 120;

	if (mode & PM_FORCE_PET) who = -1;
	else who = 0;

	if(cheat_hear) msg_format("Lev:%d mode:%d type:%d num:%d PET:%s NOPET:%s UNIQUE:%s",lev,mode,type,num,(mode&PM_FORCE_PET)?"TRUE":"FALSE",(mode&PM_NO_PET)?"TRUE":"FALSE",(mode&PM_ALLOW_UNIQUE)?"TRUE":"FALSE");

	for (i = 0; i < num; i++)
	{
		if (summon_specific(who, y, x, lev, type, mode)) success = TRUE;
	}

	if (!success)
	{
#ifdef JP
		msg_print("あなたの呼び声に答える者はいなかった。");
#else
		msg_print("Nobody answers to your Trump call.");
#endif
	}

	return success;
}


/*:::「石桜」を選択するためのhook*/
static bool item_tester_hook_ishizakura(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_ISHIZAKURA) return (FALSE);
	return (TRUE);
}

/*:::「黄金の蜂蜜酒」の材料を選択するためのhook*/
static bool item_tester_hook_make_space_mead(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION && o_ptr->sval == SV_POTION_STAR_ENLIGHTENMENT ) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONNAIL ) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI ) return (TRUE);
	if (o_ptr->tval == TV_ALCOHOL  && o_ptr->sval == SV_ALCOHOL_KUSHINADA ) return (TRUE);
	if (o_ptr->tval == TV_SWEETS   && o_ptr->sval == SV_SWEETS_MIRACLE_FRUIT ) return (TRUE);

	return (FALSE);
}
/*:::「伊弉諾物質」を選択するためのhook*/
static bool item_tester_hook_izanagi_object(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval == SV_MATERIAL_IZANAGIOBJECT) return (TRUE);
	if (o_ptr->sval == SV_MATERIAL_RYUUZYU) return (TRUE);
	return (FALSE);
}



/*:::ドラゴン関係の物品*/
static bool item_tester_hook_dragonkind(object_type *o_ptr)
{
	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DRAGON_DAGGER)return (TRUE);
	if (o_ptr->tval == TV_SHIELD && o_ptr->sval == SV_DRAGON_SHIELD)return (TRUE);
	if (o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_DRAGONHELM)return (TRUE);
	if (o_ptr->tval == TV_GLOVES && o_ptr->sval == SV_HAND_DRAGONGLOVES)return (TRUE);
	if (o_ptr->tval == TV_BOOTS && o_ptr->sval == SV_LEG_DRAGONBOOTS)return (TRUE);
	if (o_ptr->tval == TV_DRAG_ARMOR)return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONNAIL)return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONSCALE)return (TRUE);


	/* Assume not wearable */
	return (FALSE);
}



static cptr do_life_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "軽傷の治癒";
		if (desc) return "怪我と体力を少し回復させる。";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif

		{
			int dice = 2;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "祝福";
		if (desc) return "一定時間、命中率とACにボーナスを得る。";
#else
		if (name) return "Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif

		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "軽傷";
		if (desc) return "1体のモンスターに小ダメージを与える。抵抗されると無効。";
#else
		if (name) return "Cause Light Wounds";
		if (desc) return "Wounds a monster a little unless resisted.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "光の召喚";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Call Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "罠 & 隠し扉感知";
		if (desc) return "近くの全ての罠と扉と階段を感知する。";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "重傷の治癒";
		if (desc) return "怪我と体力を中程度回復させる。";
#else
		if (name) return "Cure Medium Wounds";
		if (desc) return "Heals cut and HP more.";
#endif

		{
			int dice = 4;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut((p_ptr->cut / 2) - 20);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "解毒";
		if (desc) return "体内の毒を取り除く。";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cure poison status.";
#endif

		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "空腹充足";
		if (desc) return "満腹にする。";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif

		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "解呪";
		if (desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "重傷";
		if (desc) return "1体のモンスターに中ダメージを与える。抵抗されると無効。";
#else
		if (name) return "Cause Medium Wounds";
		if (desc) return "Wounds a monster unless resisted.";
#endif

		{
			int sides = 8 + (plev - 5) / 4;
			int dice = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(sides, dice), 0);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "致命傷の治癒";
		if (desc) return "体力を大幅に回復させ、負傷と朦朧状態も全快する。";
#else
		if (name) return "Cure Critical Wounds";
		if (desc) return "Heals cut, stun and HP greatly.";
#endif

		{
			int dice = 8;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "耐熱耐寒";
		if (desc) return "一定時間、火炎と冷気に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Heat and Cold";
		if (desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "周辺感知";
		if (desc) return "周辺の地形を感知する。";
#else
		if (name) return "Sense Surroundings";
		if (desc) return "Maps nearby area.";
#endif

		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "パニック・アンデッド";
		if (desc) return "視界内のアンデッドを恐怖させる。抵抗されると無効。";
#else
		if (name) return "Turn Undead";
		if (desc) return "Attempts to scare undead monsters in sight.";
#endif

		{
			if (cast)
			{
				turn_undead();
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "体力回復";
		if (desc) return "極めて強力な回復呪文で、負傷と朦朧状態も全快する。";
#else
		if (name) return "Healing";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif

		{
			int heal = 300;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "結界の紋章";
		if (desc) return "自分のいる床の上に、モンスターが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if (name) return "Glyph of Warding";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif

		{
			if (cast)
			{
				warding_glyph();
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "*解呪*";
		if (desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "鑑識";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Perception";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "アンデッド退散";
		if (desc) return "視界内の全てのアンデッドにダメージを与える。";
#else
		if (name) return "Dispel Undead";
		if (desc) return "Damages all undead monsters in sight.";
#endif

		{
			int dice = 1;
			int sides = plev * 5;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				dispel_undead(damroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "凪の刻";
		if (desc) return "視界内の全てのモンスターを魅了する。抵抗されると無効。";
#else
		if (name) return "Day of the Dove";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_monsters(power);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "致命傷";
		if (desc) return "1体のモンスターに大ダメージを与える。抵抗されると無効。";
#else
		if (name) return "Cause Critical Wounds";
		if (desc) return "Wounds a monster critically unless resisted.";
#endif

		{
			int dice = 5 + (plev - 5) / 3;
			int sides = 15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "帰還の詔";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "真実の祭壇";
		if (desc) return "現在の階を再構成する。";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "真・結界";
		if (desc) return "自分のいる床と周囲8マスの床の上に、モンスターが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if (name) return "Warding True";
		if (desc) return "Creates glyphs in all adjacent squares and under you.";
#endif

		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				warding_glyph();
				glyph_creation();
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "不毛化";
		if (desc) return "この階の増殖するモンスターが増殖できなくなる。";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif

		{
			if (cast)
			{
				num_repro += MAX_REPRO;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "全感知";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "アンデッド消滅";
		if (desc) return "自分の周囲にいるアンデッドを現在の階から消し去る。抵抗されると無効。";
#else
		if (name) return "Annihilate Undead";
		if (desc) return "Eliminates all nearby undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif

		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide_undead(power, TRUE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "千里眼";
		if (desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif

		{
			if (cast)
			{
				wiz_lite(FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "全復活";
		if (desc) return "すべてのステータスと経験値を回復する。";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif

		{
			if (cast)
			{
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				restore_level();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "*体力回復*";
		if (desc) return "最強の治癒の魔法で、負傷と朦朧状態も全快する。";
#else
		if (name) return "Healing True";
		if (desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif

		{
			int heal = 2000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "聖なるビジョン";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Holy Vision";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "究極の耐性";
		if (desc) return "一定時間、あらゆる耐性を付け、ACと魔法防御能力を上昇させる。";
#else
		if (name) return "Ultimate Resistance";
		if (desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int v = randint1(base) + base;
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_sorcery_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全ての見えるモンスターを感知する。";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "ショート・テレポート";
		if (desc) return "近距離のテレポートをする。";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif

		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "罠と扉感知";
		if (desc) return "近くの全ての扉と罠を感知する。";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "ライト・エリア";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "パニック・モンスター";
		if (desc) return "モンスター1体を混乱させる。抵抗されると無効。";
#else
		if (name) return "Confuse Monster";
		if (desc) return "Attempts to confuse a monster.";
#endif

		{
			int power = (plev * 3) / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				confuse_monster(dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "スリープ・モンスター";
		if (desc) return "モンスター1体を眠らせる。抵抗されると無効。";
#else
		if (name) return "Sleep Monster";
		if (desc) return "Attempts to sleep a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				sleep_monster(dir, plev);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "魔力充填";
		if (desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "魔法の地図";
		if (desc) return "周辺の地形を感知する。";
#else
		if (name) return "Magic Mapping";
		if (desc) return "Maps nearby area.";
#endif

		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "鑑定";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "スロウ・モンスター";
		if (desc) return "モンスター1体を減速さる。抵抗されると無効。";
#else
		if (name) return "Slow Monster";
		if (desc) return "Attempts to slow a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				slow_monster(dir, plev);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "周辺スリープ";
		if (desc) return "視界内の全てのモンスターを眠らせる。抵抗されると無効。";
#else
		if (name) return "Mass Sleep";
		if (desc) return "Attempts to sleep all monsters in sight.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				sleep_monsters(plev);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "テレポート・モンスター";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "スピード";
		if (desc) return "一定時間、加速する。";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif

		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "真・感知";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Detection True";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "真・鑑定";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Identify True";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "物体と財宝感知";
		if (desc) return "近くの全てのアイテムと財宝を感知する。";
#else
		if (name) return "Detect items and Treasure";
		if (desc) return "Detects all treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "チャーム・モンスター";
		if (desc) return "モンスター1体を魅了する。抵抗されると無効。";
#else
		if (name) return "Charm Monster";
		if (desc) return "Attempts to charm a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_monster(dir, power);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "精神感知";
		if (desc) return "一定時間、テレパシー能力を得る。";
#else
		if (name) return "Sense Minds";
		if (desc) return "Gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "街移動";
		if (desc) return "街へ移動する。地上にいるときしか使えない。";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif

		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "自己分析";
		if (desc) return "現在の自分の状態を完全に知る。";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif

		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "テレポート・レベル";
		if (desc) return "瞬時に上か下の階にテレポートする。";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif

		{
			if (cast)
			{
#ifdef JP
				if (!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "帰還の呪文";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "次元の扉";
		if (desc) return "短距離内の指定した場所にテレポートする。";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif

		{
			int range = plev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "調査";
		if (desc) return "モンスターの属性、残り体力、最大体力、スピード、正体を知る。";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif

		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "爆発のルーン";
		if (desc) return "自分のいる床の上に、モンスターが通ると爆発してダメージを与えるルーンを描く。";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif

		{
			int dice = 7;
			int sides = 7;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				explosive_rune();
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "念動力";
		if (desc) return "アイテムを自分の足元へ移動させる。";
#else
		if (name) return "Telekinesis";
		if (desc) return "Pulls a distant item close to you.";
#endif

		{
			int weight = plev * 15;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "千里眼";
		if (desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。さらに、一定時間テレパシー能力を得る。";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				chg_virtue(V_KNOWLEDGE, 1);
				chg_virtue(V_ENLIGHTEN, 1);

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "魅了の視線";
		if (desc) return "視界内の全てのモンスターを魅了する。抵抗されると無効。";
#else
		if (name) return "Charm monsters";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_monsters(power);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "錬金術";
		if (desc) return "アイテム1つをお金に変える。";
#else
		if (name) return "Alchemy";
		if (desc) return "Turns an item into 1/3 of its value in gold.";
#endif

		{
			if (cast)
			{
				if (!alchemy()) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "怪物追放";
		if (desc) return "視界内の全てのモンスターをテレポートさせる。抵抗されると無効。";
#else
		if (name) return "Banishment";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "無傷の球";
		if (desc) return "一定時間、ダメージを受けなくなるバリアを張る。切れた瞬間に少しターンを消費するので注意。";
#else
		if (name) return "Globe of Invulnerability";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif

		{
			int base = 4;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_invuln(randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_nature_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_rng[] = "射程";
#else
	static const char s_dam[] = "dam ";
	static const char s_rng[] = "rng ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全ての見えるモンスターを感知する。";
#else
		if (name) return "Detect Creatures";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "稲妻";
		if (desc) return "電撃の短いビームを放つ。";
#else
		if (name) return "Lightning";
		if (desc) return "Fires a short beam of lightning.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int range = plev / 6 + 2;

			if (info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);

			if (cast)
			{
				project_length = range;

				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "罠と扉感知";
		if (desc) return "近くの全ての罠と扉を感知する。";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "食糧生成";
		if (desc) return "食料を一つ作り出す。";
#else
		if (name) return "Produce Food";
		if (desc) return "Produces a Ration of Food.";
#endif

		{
			if (cast)
			{
				object_type forge, *q_ptr = &forge;

#ifdef JP
				msg_print("食料を生成した。");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				///item 食糧生成
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "日の光";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Daylight";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("日の光があなたの肉体を焦がした！");
#else
					msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "日の光", -1);
#else
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "daylight", -1);
#endif
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "動物習し";
		if (desc) return "動物1体を魅了する。抵抗されると無効。";
#else
		if (name) return "Animal Taming";
		if (desc) return "Attempts to charm an animal.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_animal(dir, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "環境への耐性";
		if (desc) return "一定時間、冷気、炎、電撃に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Environment";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "傷と毒治療";
		if (desc) return "怪我を全快させ、毒を体から完全に取り除き、体力を少し回復させる。";
#else
		if (name) return "Cure Wounds & Poison";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif

		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "岩石溶解";
		if (desc) return "壁を溶かして床にする。";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif

		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "アイス・ボルト";
		if (desc) return "冷気のボルトもしくはビームを放つ。";
#else
		if (name) return "Frost Bolt";
		if (desc) return "Fires a bolt or beam of cold.";
#endif

		{
			int dice = 3 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "自然の覚醒";
		if (desc) return "周辺の地形を感知し、近くの罠、扉、階段、全てのモンスターを感知する。";
#else
		if (name) return "Nature Awareness";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif

		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_traps(rad2, TRUE);
				detect_doors(rad2);
				detect_stairs(rad2);
				detect_monsters_normal(rad2);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "ファイア・ボルト";
		if (desc) return "火炎のボルトもしくはビームを放つ。";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif

		{
			int dice = 5 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "太陽光線";
		if (desc) return "光線を放つ。光りを嫌うモンスターに効果がある。";
#else
		if (name) return "Ray of Sunlight";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif

		{
			int dice = 6;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				msg_print("太陽光線が現れた。");
#else
				msg_print("A line of sunlight appears.");
#endif

				lite_line(dir, damroll(6, 8));
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "足かせ";
		if (desc) return "視界内の全てのモンスターを減速させる。抵抗されると無効。";
#else
		if (name) return "Entangle";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				slow_monsters(plev);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "動物召喚";
		if (desc) return "動物を1体召喚する。";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons an animal.";
#endif

		{
			if (cast)
			{
				if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				{
#ifdef JP
					msg_print("動物は現れなかった。");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "薬草治療";
		if (desc) return "体力を大幅に回復させ、負傷、朦朧状態、毒から全快する。";
#else
		if (name) return "Herbal Healing";
		if (desc) return "Heals HP greatly. And heals cut, stun and poison completely.";
#endif

		{
			int heal = 500;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "階段生成";
		if (desc) return "自分のいる位置に階段を作る。";
#else
		if (name) return "Stair Building";
		if (desc) return "Creates a stair which goes down or up.";
#endif

		{
			if (cast)
			{
				stair_creation();
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "肌石化";
		if (desc) return "一定時間、ACを上昇させる。";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif

		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "真・耐性";
		if (desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resistance True";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "森林創造";
		if (desc) return "周囲に木を作り出す。";
#else
		if (name) return "Forest Creation";
		if (desc) return "Creates trees in all adjacent squares.";
#endif

		{
			if (cast)
			{
				tree_creation();
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "動物友和";
		if (desc) return "視界内の全ての動物を魅了する。抵抗されると無効。";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_animals(power);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "試金石";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Stone Tell";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "石の壁";
		if (desc) return "自分の周囲に花崗岩の壁を作る。";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif

		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "腐食防止";
		if (desc) return "アイテムを酸で傷つかないよう加工する。";
#else
		if (name) return "Protect from Corrosion";
		if (desc) return "Makes an equipment acid-proof.";
#endif

		{
			if (cast)
			{
				if (!rustproof()) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "地震";
		if (desc) return "周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。";
#else
		if (name) return "Earthquake";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cast)
			{
				earthquake(py, px, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "カマイタチ";
		if (desc) return "全方向に向かって攻撃する。";
#else
		if (name) return "Cyclone";
		if (desc) return "Attacks all adjacent monsters.";
#endif

		{
			///sys カマイタチの魔法
			if (cast)
			{
				int y = 0, x = 0;
				cave_type       *c_ptr;
				monster_type    *m_ptr;

				for (dir = 0; dir < 8; dir++)
				{
					y = py + ddy_ddd[dir];
					x = px + ddx_ddd[dir];
					c_ptr = &cave[y][x];

					/* Get the monster */
					m_ptr = &m_list[c_ptr->m_idx];

					/* Hack -- attack monsters */
					if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
						py_attack(y, x, 0);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "ブリザード";
		if (desc) return "巨大な冷気の球を放つ。";
#else
		if (name) return "Blizzard";
		if (desc) return "Fires a huge ball of cold.";
#endif

		{
			int dam = 70 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_COLD, dir, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "稲妻嵐";
		if (desc) return "巨大な電撃の球を放つ。";
#else
		if (name) return "Lightning Storm";
		if (desc) return "Fires a huge electric ball.";
#endif

		{
			int dam = 90 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_ELEC, dir, dam, rad);
				break;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "渦潮";
		if (desc) return "巨大な水の球を放つ。";
#else
		if (name) return "Whirlpool";
		if (desc) return "Fires a huge ball of water.";
#endif

		{
			int dam = 100 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_WATER, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "陽光召喚";
		if (desc) return "自分を中心とした光の球を発生させる。さらに、その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。";
#else
		if (name) return "Call Sunlight";
		if (desc) return "Generates ball of light centered on you. Maps and lights whole dungeon level. Knows all objects location.";
#endif

		{
			int dam = 150;
			int rad = 8;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_LITE, 0, dam, rad);
				chg_virtue(V_KNOWLEDGE, 1);
				chg_virtue(V_ENLIGHTEN, 1);
				wiz_lite(FALSE);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("日光があなたの肉体を焦がした！");
#else
					msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, 50, "日光", -1);
#else
					take_hit(DAMAGE_NOESCAPE, 50, "sunlight", -1);
#endif
				}
			}
		}
		break;


#ifdef JP
		if (name) return "自然の脅威";
		if (desc) return "近くの全てのモンスターにダメージを与え、地震を起こし、自分を中心とした分解の球を発生させる。";
#else
		if (name) return "Nature's Wrath";
		if (desc) return "Damages all monsters in sight. Makes quake. Generates disintegration ball centered on you.";
#endif

		{
			int d_dam = 4 * plev;
			int b_dam = (100 + plev) * 2;
			int b_rad = 1 + plev / 12;
			int q_rad = 20 + plev / 2;

			if (info) return format("%s%d+%d", s_dam, d_dam, b_dam/2);

			if (cast)
			{
				dispel_monsters(d_dam);
				earthquake(py, px, q_rad);
				project(0, b_rad, py, px, b_dam, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;
	}

	return "";
}


static cptr do_chaos_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "マジック・ミサイル";
		if (desc) return "弱い魔法の矢を放つ。";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif

		{
			int dice = 3 + ((plev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "トラップ/ドア破壊";
		if (desc) return "隣接する罠と扉を破壊する。";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif

		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "閃光";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Flash of Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "混乱の手";
		if (desc) return "相手を混乱させる攻撃をできるようにする。";
#else
		if (name) return "Touch of Confusion";
		if (desc) return "Attempts to confuse the next monster that you hit.";
#endif

		{
			if (cast)
			{
				if (!(p_ptr->special_attack & ATTACK_CONFUSE))
				{
#ifdef JP
					msg_print("あなたの手は光り始めた。");
#else
					msg_print("Your hands start glowing.");
#endif

					p_ptr->special_attack |= ATTACK_CONFUSE;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "魔力炸裂";
		if (desc) return "魔法の球を放つ。";
#else
		if (name) return "Mana Burst";
		if (desc) return "Fires a ball of magic.";
#endif

		{
			int dice = 3;
			int sides = 5;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class メイジ系は威力が上昇する魔法
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MISSILE, dir, damroll(dice, sides) + base, rad);

				/*
				 * Shouldn't actually use GF_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "ファイア・ボルト";
		if (desc) return "炎のボルトもしくはビームを放つ。";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif

		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "力の拳";
		if (desc) return "ごく小さな分解の球を放つ。";
#else
		if (name) return "Fist of Force";
		if (desc) return "Fires a tiny ball of disintegration.";
#endif

		{
			int dice = 8 + ((plev - 5) / 4);
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir,
					damroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport Self";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "ワンダー";
		if (desc) return "モンスターにランダムな効果を与える。";
#else
		if (name) return "Wonder";
		if (desc) return "Fires something with random effects.";
#endif

		{
			if (info) return s_random;

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				cast_wonder(dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "カオス・ボルト";
		if (desc) return "カオスのボルトもしくはビームを放つ。";
#else
		if (name) return "Chaos Bolt";
		if (desc) return "Fires a bolt or ball of chaos.";
#endif

		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_CHAOS, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "ソニック・ブーム";
		if (desc) return "自分を中心とした轟音の球を発生させる。";
#else
		if (name) return "Sonic Boom";
		if (desc) return "Generates a ball of sound centered on you.";
#endif

		{
			int dam = 60 + plev;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
#ifdef JP
				msg_print("ドーン！部屋が揺れた！");
#else
				msg_print("BOOM! Shake the room!");
#endif

				project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "破滅の矢";
		if (desc) return "純粋な魔力のビームを放つ。";
#else
		if (name) return "Doom Bolt";
		if (desc) return "Fires a beam of pure mana.";
#endif

		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_MANA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "ファイア・ボール";
		if (desc) return "炎の球を放つ。";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif

		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "テレポート・アウェイ";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Other";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "破壊の言葉";
		if (desc) return "周辺のアイテム、モンスター、地形を破壊する。";
#else
		if (name) return "Word of Destruction";
		if (desc) return "Destroy everything in nearby area.";
#endif

		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "ログルス発動";
		if (desc) return "巨大なカオスの球を放つ。";
#else
		if (name) return "Invoke Logrus";
		if (desc) return "Fires a huge ball of chaos.";
#endif

		{
			int dam = plev * 2 + 99;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "他者変容";
		if (desc) return "モンスター1体を変身させる。抵抗されると無効。";
#else
		if (name) return "Polymorph Other";
		if (desc) return "Attempts to polymorph a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				poly_monster(dir, plev);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "連鎖稲妻";
		if (desc) return "全方向に対して電撃のビームを放つ。";
#else
		if (name) return "Chain Lightning";
		if (desc) return "Fires lightning beams in all directions.";
#endif

		{
			int dice = 5 + plev / 10;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				for (dir = 0; dir <= 9; dir++)
					fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "魔力封入";
		if (desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if (name) return "Arcane Binding";
		if (desc) return "Recharges staffs, wands or rods.";
#endif

		{
			int power = 90;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "原子分解";
		if (desc) return "巨大な分解の球を放つ。";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif

		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "現実変容";
		if (desc) return "現在の階を再構成する。";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "マジック・ロケット";
		if (desc) return "ロケットを発射する。";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif

		{
			int dam = 120 + plev * 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("ロケット発射！");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "混沌の刃";
		if (desc) return "武器にカオスの属性をつける。";
#else
		if (name) return "Chaos Branding";
		if (desc) return "Makes current weapon a Chaotic weapon.";
#endif

		{
			if (cast)
			{
				brand_weapon(2);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "悪魔召喚";
		if (desc) return "悪魔を1体召喚する。";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif

		{
			if (cast)
			{
				u32b summon_flags = 0L;
				bool pet = !one_in_(3);

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, SUMMON_DEMON, summon_flags))
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif

					if (pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「卑しき者よ、我は汝の下僕にあらず！ お前の魂を頂くぞ！」");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "重力光線";
		if (desc) return "重力のビームを放つ。";
#else
		if (name) return "Beam of Gravity";
		if (desc) return "Fires a beam of gravity.";
#endif

		{
			int dice = 9 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_GRAVITY, dir, damroll(dice, sides));
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "流星群";
		if (desc) return "自分の周辺に隕石を落とす。";
#else
		if (name) return "Meteor Swarm";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif

		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "焔の一撃";
		if (desc) return "自分を中心とした超巨大な炎の球を発生させる。";
#else
		if (name) return "Flame Strike";
		if (desc) return "Generate a huge ball of fire centered on you.";
#endif

		{
			int dam = 300 + 3 * plev;
			int rad = 8;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_FIRE, 0, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "混沌召来";
		if (desc) return "ランダムな属性の球やビームを発生させる。";
#else
		if (name) return "Call Chaos";
		if (desc) return "Generate random kind of balls or beams.";
#endif

		{
			if (info) return format("%s150 / 250", s_dam);

			if (cast)
			{
				call_chaos();
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "自己変容";
		if (desc) return "自分を変身させようとする。";
#else
		if (name) return "Polymorph Self";
		if (desc) return "Polymorphs yourself.";
#endif

		{
			if (cast)
			{
#ifdef JP
				if (!get_check("変身します。よろしいですか？")) return NULL;
#else
				if (!get_check("You will polymorph yourself. Are you sure? ")) return NULL;
#endif
				do_poly_self();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "魔力の嵐";
		if (desc) return "非常に強力で巨大な純粋な魔力の球を放つ。";
#else
		if (name) return "Mana Storm";
		if (desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif

		{
			int dam = 300 + plev * 4;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MANA, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "ログルスのブレス";
		if (desc) return "非常に強力なカオスの球を放つ。";
#else
		if (name) return "Breathe Logrus";
		if (desc) return "Fires an extremely powerful ball of chaos.";
#endif

		{
			int dam = p_ptr->chp;
			int rad = 2;


			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "虚無召来";
		if (desc) return "自分に周囲に向かって、ロケット、純粋な魔力の球、放射性廃棄物の球を放つ。ただし、壁に隣接して使用すると広範囲を破壊する。";
#else
		if (name) return "Call the Void";
		if (desc) return "Fires rockets, mana balls and nuclear waste balls in all directions each unless you are not adjacent to any walls. Otherwise *destroys* huge area.";
#endif

		{
			if (info) return format("%s3 * 175", s_dam);

			if (cast)
			{
				call_the_();
			}
		}
		break;
	}

	return "";
}


static cptr do_death_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "無生命感知";
		if (desc) return "近くの生命のないモンスターを感知する。";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "呪殺弾";
		if (desc) return "ごく小さな邪悪な力を持つボールを放つ。善良なモンスターには大きなダメージを与える。";
#else
		if (name) return "Malediction";
		if (desc) return "Fires a tiny ball of evil power which hurts good monsters greatly.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				/*
				 * A radius-0 ball may (1) be aimed at
				 * objects etc., and will affect them;
				 * (2) may be aimed at ANY visible
				 * monster, unlike a 'bolt' which must
				 * travel to the monster.
				 */

				fire_ball(GF_HELL_FIRE, dir, damroll(dice, sides), rad);

				if (one_in_(5))
				{
					/* Special effect first */
					int effect = randint1(1000);

					if (effect == 666)
						fire_ball_hide(GF_DEATH_RAY, dir, plev * 200, 0);
					else if (effect < 500)
						fire_ball_hide(GF_TURN_ALL, dir, plev, 0);
					else if (effect < 800)
						fire_ball_hide(GF_OLD_CONF, dir, plev, 0);
					else
						fire_ball_hide(GF_STUN, dir, plev, 0);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "邪悪感知";
		if (desc) return "近くの邪悪なモンスターを感知する。";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "悪臭雲";
		if (desc) return "毒の球を放つ。";
#else
		if (name) return "Stinking Cloud";
		if (desc) return "Fires a ball of poison.";
#endif

		{
			int dam = 10 + plev / 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "黒い眠り";
		if (desc) return "1体のモンスターを眠らせる。抵抗されると無効。";
#else
		if (name) return "Black Sleep";
		if (desc) return "Attempts to sleep a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				sleep_monster(dir, plev);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "耐毒";
		if (desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "恐慌";
		if (desc) return "モンスター1体を恐怖させ、朦朧させる。抵抗されると無効。";
#else
		if (name) return "Horrify";
		if (desc) return "Attempts to scare and stun a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
				stun_monster(dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "アンデッド従属";
		if (desc) return "アンデッド1体を魅了する。抵抗されると無効。";
#else
		if (name) return "Enslave Undead";
		if (desc) return "Attempts to charm an undead monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_undead(dir, power);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "エントロピーの球";
		if (desc) return "生命のある者に効果のある球を放つ。";
#else
		if (name) return "Orb of Entropy";
		if (desc) return "Fires a ball which damages living monsters.";
#endif

		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
///class メイジ系は威力が上がる魔法
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_OLD_DRAIN, dir, damroll(dice, dice) + base, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "地獄の矢";
		if (desc) return "地獄のボルトもしくはビームを放つ。";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether.";
#endif

		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "殺戮雲";
		if (desc) return "自分を中心とした毒の球を発生させる。";
#else
		if (name) return "Cloud kill";
		if (desc) return "Generate a ball of poison centered on you.";
#endif

		{
			int dam = (30 + plev) * 2;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				project(0, rad, py, px, dam, GF_POIS, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "モンスター消滅";
		if (desc) return "モンスター1体を消し去る。経験値やアイテムは手に入らない。抵抗されると無効。";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif

		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "毒の刃";
		if (desc) return "武器に毒の属性をつける。";
#else
		if (name) return "Poison Branding";
		if (desc) return "Makes current weapon poison branded.";
#endif

		{
			if (cast)
			{
				brand_weapon(3);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "吸血ドレイン";
		if (desc) return "モンスター1体から生命力を吸いとる。吸いとった生命力によって満腹度が上がる。";
#else
		if (name) return "Vampiric Drain";
		if (desc) return "Absorbs some HP from a monster and gives them to you. You will also gain nutritional sustenance from this.";
#endif

		{
			int dice = 1;
			int sides = plev * 2;
			int base = plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice, sides);

				if (!get_aim_dir(&dir)) return NULL;

				if (drain_life(dir, dam))
				{
	///del131214 virtue
					//chg_virtue(V_SACRIFICE, -1);
					//chg_virtue(V_VITALITY, -1);

					hp_player(dam);

					/*
					 * Gain nutritional sustenance:
					 * 150/hp drained
					 *
					 * A Food ration gives 5000
					 * food points (by contrast)
					 * Don't ever get more than
					 * "Full" this way But if we
					 * ARE Gorged, it won't cure
					 * us
					 */
					dam = p_ptr->food + MIN(5000, 100 * dam);

					/* Not gorged already */
					if (p_ptr->food < PY_FOOD_MAX)
						set_food(dam >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dam);
				}
			}
		}
		break;

		///sysdel 死人返し
	case 14:
#ifdef JP
		if (name) return "反魂の術";
		if (desc) return "周囲の死体や骨を生き返す。";
#else
		if (name) return "Animate dead";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif

		{
			if (cast)
			{
				///del131214 dead
				//animate_dead(0, py, px);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "抹殺";
		if (desc) return "指定した文字のモンスターを現在の階から消し去る。抵抗されると無効。";
#else
		if (name) return "Genocide";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif

		{
			int power = plev+50;

			if (info) return info_power(power);

			if (cast)
			{
				symbol_genocide(power, TRUE,0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "狂戦士化";
		if (desc) return "狂戦士化し、恐怖を除去する。";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "悪霊召喚";
		if (desc) return "ランダムで様々な効果が起こる。";
#else
		if (name) return "Invoke Spirits";
		if (desc) return "Causes random effects.";
#endif

		{
			if (info) return s_random;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				cast_invoke_spirits(dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "暗黒の矢";
		if (desc) return "暗黒のボルトもしくはビームを放つ。";
#else
		if (name) return "Dark Bolt";
		if (desc) return "Fires a bolt or beam of darkness.";
#endif

		{
			int dice = 4 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "狂乱戦士";
		if (desc) return "狂戦士化し、恐怖を除去し、加速する。";
#else
		if (name) return "Battle Frenzy";
		if (desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif

		{
			int b_base = 25;
			int sp_base = plev / 2;
			int sp_sides = 20 + plev / 2;

			if (info) return info_duration(b_base, b_base);

			if (cast)
			{
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "吸血の刃";
		if (desc) return "武器に吸血の属性をつける。";
#else
		if (name) return "Vampiric Branding";
		if (desc) return "Makes current weapon Vampiric.";
#endif

		{
			if (cast)
			{
				brand_weapon(4);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "真・吸血";
		if (desc) return "モンスター1体から生命力を吸いとる。吸いとった生命力によって体力が回復する。";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif

		{
			int dam = 100;

			if (info) return format("%s3*%d", s_dam, dam);

			if (cast)
			{
				int i;

				if (!get_aim_dir(&dir)) return NULL;
				///del131214 virtue
				//chg_virtue(V_SACRIFICE, -1);
				//chg_virtue(V_VITALITY, -1);

				for (i = 0; i < 3; i++)
				{
					if (drain_life(dir, dam))
						hp_player(dam);
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "死の言魂";
		if (desc) return "視界内の生命のあるモンスターにダメージを与える。";
#else
		if (name) return "Nether Wave";
		if (desc) return "Damages all living monsters in sight.";
#endif

		{
			int sides = plev * 3;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_living(randint1(sides));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "暗黒の嵐";
		if (desc) return "巨大な暗黒の球を放つ。";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif

		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DARK, dir, dam, rad);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "死の光線";
		if (desc) return "死の光線を放つ。";
#else
		if (name) return "Death Ray";
		if (desc) return "Fires a beam of death.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				death_ray(dir, plev);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "死者召喚";
		if (desc) return "1体のアンデッドを召喚する。";
#else
		if (name) return "Raise the Dead";
		if (desc) return "Summons an undead monster.";
#endif

		{
			if (cast)
			{
				int type;
				bool pet = one_in_(3);
				u32b summon_flags = 0L;

				type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

				if (!pet || (pet && (plev > 24) && one_in_(3)))
					summon_flags |= PM_ALLOW_GROUP;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= (PM_ALLOW_UNIQUE | PM_NO_PET);

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, type, summon_flags))
				{
#ifdef JP
					msg_print("冷たい風があなたの周りに吹き始めた。それは腐敗臭を運んでいる...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("古えの死せる者共があなたに仕えるため土から甦った！");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("死者が甦った。眠りを妨げるあなたを罰するために！");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
					///del131214 virtue
					//chg_virtue(V_UNLIFE, 1);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "死者の秘伝";
		if (desc) return "アイテムを1つ識別する。レベルが高いとアイテムの能力を完全に知ることができる。";
#else
		if (name) return "Esoteria";
		if (desc) return "Identifies an item. Or *identifies* an item at higher level.";
#endif

		{
			if (cast)
			{
				if (randint1(50) > plev)
				{
					if (!ident_spell(FALSE)) return NULL;
				}
				else
				{
					if (!identify_fully(FALSE)) return NULL;
				}
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "吸血鬼変化";
		if (desc) return "一定時間、吸血鬼に変化する。変化している間は本来の種族の能力を失い、代わりに吸血鬼としての能力を得る。";
#else
		if (name) return "Polymorph Vampire";
		if (desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif

		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_VAMPIRE, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "生命力復活";
		if (desc) return "失った経験値を回復する。";
#else
		if (name) return "Restore Life";
		if (desc) return "Restore lost experience.";
#endif

		{
			if (cast)
			{
				restore_level();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "周辺抹殺";
		if (desc) return "自分の周囲にいるモンスターを現在の階から消し去る。抵抗されると無効。";
#else
		if (name) return "Mass Genocide";
		if (desc) return "Eliminates all nearby monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif

		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide(power, TRUE);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "地獄の劫火";
		if (desc) return "邪悪な力を持つ宝珠を放つ。善良なモンスターには大きなダメージを与える。";
#else
		if (name) return "Hellfire";
		if (desc) return "Fires a powerful ball of evil power. Hurts good monsters greatly.";
#endif

		{
			int dam = 666;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "地獄の劫火の呪文を唱えた疲労", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "幽体化";
		if (desc) return "一定時間、壁を通り抜けることができ受けるダメージが軽減される幽体の状態に変身する。";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_wraith_form(randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_trump_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

#ifdef JP
	static const char s_random[] = "ランダム";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "ショート・テレポート";
		if (desc) return "近距離のテレポートをする。";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif

		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "蜘蛛のカード";
		if (desc) return "蜘蛛を召喚する。";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたは蜘蛛のカードに集中する...");
#else
				msg_print("You concentrate on the trump of an spider...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_SPIDER, PM_ALLOW_GROUP))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚された蜘蛛は怒っている！");
#else
						msg_print("The summoned spiders get angry!");
#endif
					}
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "シャッフル";
		if (desc) return "カードの占いをする。";
#else
		if (name) return "Shuffle";
		if (desc) return "Causes random effects.";
#endif

		{
			if (info) return s_random;

			if (cast)
			{
				cast_shuffle();
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "フロア・リセット";
		if (desc) return "最深階を変更する。";
#else
		if (name) return "Reset Recall";
		if (desc) return "Resets the 'deepest' level for recall spell.";
#endif

		{
			if (cast)
			{
				if (!reset_recall()) return NULL;
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 4;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "感知のカード";
		if (desc) return "一定時間、テレパシー能力を得る。";
#else
		if (name) return "Trump Spying";
		if (desc) return "Gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "テレポート・モンスター";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "動物のカード";
		if (desc) return "1体の動物を召喚する。";
#else
		if (name) return "Trump Animals";
		if (desc) return "Summons an animal.";
#endif

		{
			if (cast || fail)
			{
				int type = (!fail ? SUMMON_ANIMAL_RANGER : SUMMON_ANIMAL);

#ifdef JP
				msg_print("あなたは動物のカードに集中する...");
#else
				msg_print("You concentrate on the trump of an animal...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚された動物は怒っている！");
#else
						msg_print("The summoned animal gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "移動のカード";
		if (desc) return "アイテムを自分の足元へ移動させる。";
#else
		if (name) return "Trump Reach";
		if (desc) return "Pulls a distant item close to you.";
#endif

		{
			int weight = plev * 15;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;
///sysdel death カミカゼのカード
	case 9:
#ifdef JP
		if (name) return "カミカゼのカード";
		if (desc) return "複数の爆発するモンスターを召喚する。";
#else
		if (name) return "Trump Kamikaze";
		if (desc) return "Summons monsters which explode by itself.";
#endif

		{
			if (cast || fail)
			{
				int x, y;
				int type;

				if (cast)
				{
					if (!target_set(TARGET_KILL)) return NULL;
					x = target_col;
					y = target_row;
				}
				else
				{
					/* Summons near player when failed */
					x = px;
					y = py;
				}

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_KAMIKAZE_LIVING;
				else
					type = SUMMON_KAMIKAZE;

#ifdef JP
				msg_print("あなたはカミカゼのカードに集中する...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if (trump_summoning(2 + randint0(plev / 7), !fail, y, x, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたモンスターは怒っている！");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "幻霊召喚";
		if (desc) return "1体の幽霊を召喚する。";
#else
		if (name) return "Phantasmal Servant";
		if (desc) return "Summons a ghost.";
#endif

		{
			/* Phantasmal Servant is not summoned as enemy when failed */
			if (cast)
			{
				int summon_lev = plev * 2 / 3 + randint1(plev / 2);

				if (trump_summoning(1, !fail, py, px, (summon_lev * 3 / 2), SUMMON_PHANTOM, 0L))
				{
#ifdef JP
					msg_print("御用でございますか、御主人様？");
#else
					msg_print("'Your wish, master?'");
#endif
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "スピード・モンスター";
		if (desc) return "モンスター1体を加速させる。";
#else
		if (name) return "Haste Monster";
		if (desc) return "Hastes a monster.";
#endif

		{
			if (cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(&dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if (!result) return NULL;

				speed_monster(dir, plev);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "テレポート・レベル";
		if (desc) return "瞬時に上か下の階にテレポートする。";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif

		{
			if (cast)
			{
#ifdef JP
				if (!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "次元の扉";
		if (desc) return "短距離内の指定した場所にテレポートする。";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif

		{
			int range = plev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(FALSE)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "帰還の呪文";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "怪物追放";
		if (desc) return "視界内の全てのモンスターをテレポートさせる。抵抗されると無効。";
#else
		if (name) return "Banish";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "位置交換のカード";
		if (desc) return "1体のモンスターと位置を交換する。";
#else
		if (name) return "Swap Position";
		if (desc) return "Swap positions of you and a monster.";
#endif

		{
			if (cast)
			{
				bool result;

				/* HACK -- No range limit */
				project_length = -1;

				result = get_aim_dir(&dir);

				/* Restore range to default */
				project_length = 0;

				if (!result) return NULL;

				teleport_swap(dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "アンデッドのカード";
		if (desc) return "1体のアンデッドを召喚する。";
#else
		if (name) return "Trump Undead";
		if (desc) return "Summons an undead monster.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたはアンデッドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of an undead creature...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_UNDEAD, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたアンデッドは怒っている！");
#else
						msg_print("The summoned undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "爬虫類のカード";
		if (desc) return "1体のヒドラを召喚する。";
#else
		if (name) return "Trump Reptiles";
		if (desc) return "Summons a hydra.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたは爬虫類のカードに集中する...");
#else
				msg_print("You concentrate on the trump of a reptile...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HYDRA, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚された爬虫類は怒っている！");
#else
						msg_print("The summoned reptile gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "モンスターのカード";
		if (desc) return "複数のモンスターを召喚する。";
#else
		if (name) return "Trump Monsters";
		if (desc) return "Summons some monsters.";
#endif

		{
			if (cast || fail)
			{
				int type;

#ifdef JP
				msg_print("あなたはモンスターのカードに集中する...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_LIVING;
				else
					type = 0;

				if (trump_summoning((1 + (plev - 15)/ 10), !fail, py, px, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたモンスターは怒っている！");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}

			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "ハウンドのカード";
		if (desc) return "1グループのハウンドを召喚する。";
#else
		if (name) return "Trump Hounds";
		if (desc) return "Summons a group of hounds.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたはハウンドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a hound...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HOUND, PM_ALLOW_GROUP))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたハウンドは怒っている！");
#else
						msg_print("The summoned hounds get angry!");
#endif
					}
				}
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "トランプの刃";
		if (desc) return "武器にトランプの属性をつける。";
#else
		if (name) return "Trump Branding";
		if (desc) return "Makes current weapon a Trump weapon.";
#endif

		{
			if (cast)
			{
				brand_weapon(5);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "人間トランプ";
		if (desc) return "ランダムにテレポートする突然変異か、自分の意思でテレポートする突然変異が身につく。";
#else
		if (name) return "Living Trump";
		if (desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif

		{
			if (cast)
			{
				///sys muta 人間トランプ　ハードコード
				int mutation;

				if (one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if (gain_random_mutation(mutation))
				{
#ifdef JP
					msg_print("あなたは生きているカードに変わった。");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "サイバーデーモンのカード";
		if (desc) return "1体のサイバーデーモンを召喚する。";
#else
		if (name) return "Trump Cyberdemon";
		if (desc) return "Summons a cyber demon.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたはサイバーデーモンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a Cyberdemon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_CYBER, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたサイバーデーモンは怒っている！");
#else
						msg_print("The summoned Cyberdemon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "予見のカード";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Trump Divination";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "知識のカード";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Trump Lore";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "回復モンスター";
		if (desc) return "モンスター1体の体力を回復させる。";
#else
		if (name) return "Heal Monster";
		if (desc) return "Heal a monster.";
#endif

		{
			int heal = plev * 10 + 200;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(&dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if (!result) return NULL;

				heal_monster(dir, heal);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "ドラゴンのカード";
		if (desc) return "1体のドラゴンを召喚する。";
#else
		if (name) return "Trump Dragon";
		if (desc) return "Summons a dragon.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたはドラゴンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a dragon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_DRAGON, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたドラゴンは怒っている！");
#else
						msg_print("The summoned dragon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "隕石のカード";
		if (desc) return "自分の周辺に隕石を落とす。";
#else
		if (name) return "Trump Meteor";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif

		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "デーモンのカード";
		if (desc) return "1体の悪魔を召喚する。";
#else
		if (name) return "Trump Demon";
		if (desc) return "Summons a demon.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたはデーモンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a demon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_DEMON, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚されたデーモンは怒っている！");
#else
						msg_print("The summoned demon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "地獄のカード";
		if (desc) return "1体の上級アンデッドを召喚する。";
#else
		if (name) return "Trump Greater Undead";
		if (desc) return "Summons a greater undead.";
#endif

		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("あなたは強力なアンデッドのカードに集中する...");
#else
				msg_print("You concentrate on the trump of a greater undead being...");
#endif
				/* May allow unique depend on level and dice roll */
				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HI_UNDEAD, PM_ALLOW_UNIQUE))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚された上級アンデッドは怒っている！");
#else
						msg_print("The summoned greater undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "古代ドラゴンのカード";
		if (desc) return "1体の古代ドラゴンを召喚する。";
#else
		if (name) return "Trump Ancient Dragon";
		if (desc) return "Summons an ancient dragon.";
#endif

		{
			if (cast)
			{
				int type;

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_HI_DRAGON_LIVING;
				else
					type = SUMMON_HI_DRAGON;

#ifdef JP
				msg_print("あなたは古代ドラゴンのカードに集中する...");
#else
				msg_print("You concentrate on the trump of an ancient dragon...");
#endif

				/* May allow unique depend on level and dice roll */
				if (trump_summoning(1, !fail, py, px, 0, type, PM_ALLOW_UNIQUE))
				{
					if (fail)
					{
#ifdef JP
						msg_print("召喚された古代ドラゴンは怒っている！");
#else
						msg_print("The summoned ancient dragon gets angry!");
#endif
					}
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_arcane_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "電撃";
		if (desc) return "電撃のボルトもしくはビームを放つ。";
#else
		if (name) return "Zap";
		if (desc) return "Fires a bolt or beam of lightning.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 3;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "魔法の施錠";
		if (desc) return "扉に鍵をかける。";
#else
		if (name) return "Wizard Lock";
		if (desc) return "Locks a door.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wizard_lock(dir);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "透明体感知";
		if (desc) return "近くの透明なモンスターを感知する。";
#else
		if (name) return "Detect Invisibility";
		if (desc) return "Detects all invisible monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_invis(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全ての見えるモンスターを感知する。";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "ショート・テレポート";
		if (desc) return "近距離のテレポートをする。";
#else
		if (name) return "Blink";
		if (desc) return "Teleport short distance.";
#endif

		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "ライト・エリア";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "罠と扉 破壊";
		if (desc) return "一直線上の全ての罠と扉を破壊する。";
#else
		if (name) return "Trap & Door Destruction";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "軽傷の治癒";
		if (desc) return "怪我と体力を少し回復させる。";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif

		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "罠と扉 感知";
		if (desc) return "近くの全ての罠と扉と階段を感知する。";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "燃素";
		if (desc) return "光源に燃料を補給する。";
#else
		if (name) return "Phlogiston";
		if (desc) return "Adds more turns of light to a lantern or torch.";
#endif

		{
			if (cast)
			{
				///del131214 燃素の魔法
				//phlogiston();
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "財宝感知";
		if (desc) return "近くの財宝を感知する。";
#else
		if (name) return "Detect Treasure";
		if (desc) return "Detects all treasures in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "魔法 感知";
		if (desc) return "近くの魔法がかかったアイテムを感知する。";
#else
		if (name) return "Detect Enchantment";
		if (desc) return "Detects all magical items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_magic(rad);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "アイテム感知";
		if (desc) return "近くの全てのアイテムを感知する。";
#else
		if (name) return "Detect Objects";
		if (desc) return "Detects all items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "解毒";
		if (desc) return "毒を体内から完全に取り除く。";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cures poison status.";
#endif

		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "耐冷";
		if (desc) return "一定時間、冷気への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "耐火";
		if (desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "耐電";
		if (desc) return "一定時間、電撃への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "耐酸";
		if (desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "重傷の治癒";
		if (desc) return "怪我と体力を中程度回復させる。";
#else
		if (name) return "Cure Medium Wounds";
		if (desc) return "Heals cut and HP more.";
#endif

		{
			int dice = 4;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut((p_ptr->cut / 2) - 50);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "鑑定";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "岩石溶解";
		if (desc) return "壁を溶かして床にする。";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif

		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "閃光";
		if (desc) return "光線を放つ。光りを嫌うモンスターに効果がある。";
#else
		if (name) return "Ray of Light";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif

		{
			int dice = 6;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("光線が放たれた。");
#else
				msg_print("A line of light appears.");
#endif

				lite_line(dir, damroll(6, 8));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "空腹充足";
		if (desc) return "満腹にする。";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif

		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "透明視認";
		if (desc) return "一定時間、透明なものが見えるようになる。";
#else
		if (name) return "See Invisible";
		if (desc) return "Gives see invisible for a while.";
#endif

		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "エレメンタル召喚";
		if (desc) return "1体のエレメンタルを召喚する。";
#else
		if (name) return "Conjure Elemental";
		if (desc) return "Summons an elemental.";
#endif

		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, plev, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("エレメンタルは現れなかった。");
#else
					msg_print("No Elementals arrive.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "テレポート・レベル";
		if (desc) return "瞬時に上か下の階にテレポートする。";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif

		{
			if (cast)
			{
#ifdef JP
				if (!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "テレポート・モンスター";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "元素の球";
		if (desc) return "炎、電撃、冷気、酸のどれかの球を放つ。";
#else
		if (name) return "Elemental Ball";
		if (desc) return "Fires a ball of some elements.";
#endif

		{
			int dam = 75 + plev;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				int type;

				if (!get_aim_dir(&dir)) return NULL;

				switch (randint1(4))
				{
					case 1:  type = GF_FIRE;break;
					case 2:  type = GF_ELEC;break;
					case 3:  type = GF_COLD;break;
					default: type = GF_ACID;break;
				}

				fire_ball(type, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "全感知";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "帰還の呪文";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "千里眼";
		if (desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。さらに、一定時間テレパシー能力を得る。";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
	///del131214 virtue
				//chg_virtue(V_KNOWLEDGE, 1);
				//chg_virtue(V_ENLIGHTEN, 1);

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_craft_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "赤外線視力";
		if (desc) return "一定時間、赤外線視力が増強される。";
#else
		if (name) return "Infravision";
		if (desc) return "Gives infravision for a while.";
#endif

		{
			int base = 100;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_infra(base + randint1(base), FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "回復力強化";
		if (desc) return "一定時間、回復力が増強される。";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif

		{
			int base = 80;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "空腹充足";
		if (desc) return "満腹になる。";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif

		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "耐冷気";
		if (desc) return "一定時間、冷気への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "耐火炎";
		if (desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "士気高揚";
		if (desc) return "一定時間、ヒーロー気分になる。";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "耐電撃";
		if (desc) return "一定時間、電撃への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "耐酸";
		if (desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "透明視認";
		if (desc) return "一定時間、透明なものが見えるようになる。";
#else
		if (name) return "See Invisibility";
		if (desc) return "Gives see invisible for a while.";
#endif

		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "解呪";
		if (desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "耐毒";
		if (desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "狂戦士化";
		if (desc) return "狂戦士化し、恐怖を除去する。";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "自己分析";
		if (desc) return "現在の自分の状態を完全に知る。";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif

		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "対邪悪結界";
		if (desc) return "邪悪なモンスターの攻撃を防ぐバリアを張る。";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif

		{
			int base = 3 * plev;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "癒し";
		if (desc) return "毒、朦朧状態、負傷を全快させ、幻覚を直す。";
#else
		if (name) return "Cure";
		if (desc) return "Heals poison, stun, cut and hallucination completely.";
#endif

		{
			if (cast)
			{
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "魔法剣";
		if (desc) return "一定時間、武器に冷気、炎、電撃、酸、毒のいずれかの属性をつける。武器を持たないと使えない。";
#else
		if (name) return "Mana Branding";
		if (desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_attack()) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "テレパシー";
		if (desc) return "一定時間、テレパシー能力を得る。";
#else
		if (name) return "Telepathy";
		if (desc) return "Gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "肌石化";
		if (desc) return "一定時間、ACを上昇させる。";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif

		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "全耐性";
		if (desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "スピード";
		if (desc) return "一定時間、加速する。";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif

		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "壁抜け";
		if (desc) return "一定時間、半物質化し壁を通り抜けられるようになる。";
#else
		if (name) return "Walk through Wall";
		if (desc) return "Gives ability to pass walls for a while.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "盾磨き";
		if (desc) return "盾に反射の属性をつける。";
#else
		if (name) return "Polish Shield";
		if (desc) return "Makes a shield a shield of reflection.";
#endif

		{
			if (cast)
			{
				// pulish_shield();
			msg_print(_("盾磨きは無効化中。", "Shield polishing is disabled."));
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "ゴーレム製造";
		if (desc) return "1体のゴーレムを製造する。";
#else
		if (name) return "Create Golem";
		if (desc) return "Creates a golem.";
#endif

		{
			if (cast)
			{
				if (summon_specific(-1, py, px, plev, SUMMON_GOLEM, PM_FORCE_PET))
				{
#ifdef JP
					msg_print("ゴーレムを作った。");
#else
					msg_print("You make a golem.");
#endif
				}
				else
				{
#ifdef JP
					msg_print("うまくゴーレムを作れなかった。");
#else
					msg_print("No Golems arrive.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "魔法の鎧";
		if (desc) return "一定時間、魔法防御力とACが上がり、混乱と盲目の耐性、反射能力、麻痺知らず、浮遊を得る。";
#else
		if (name) return "Magical armor";
		if (desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_magicdef(randint1(base) + base, FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "装備無力化";
		if (desc) return "武器・防具にかけられたあらゆる魔力を完全に解除する。";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif

		{
			if (cast)
			{
				if (!mundane_spell(TRUE)) return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "呪い粉砕";
		if (desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if (name) return "Remove All Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "完全なる知識";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Knowledge True";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "武器強化";
		if (desc) return "武器の命中率修正とダメージ修正を強化する。";
#else
		if (name) return "Enchant Weapon";
		if (desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif

		{
			if (cast)
			{
				if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "防具強化";
		if (desc) return "鎧の防御修正を強化する。";
#else
		if (name) return "Enchant Armor";
		if (desc) return "Attempts to increase +AC of an armor.";
#endif

		{
			if (cast)
			{
				if (!enchant_spell(0, 0, randint0(3) + 2)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "武器属性付与";
		if (desc) return "武器にランダムに属性をつける。";
#else
		if (name) return "Brand Weapon";
		if (desc) return "Makes current weapon a random ego weapon.";
#endif

		{
			if (cast)
			{
				brand_weapon(randint0(18));
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "人間トランプ";
		if (desc) return "ランダムにテレポートする突然変異か、自分の意思でテレポートする突然変異が身につく。";
#else
		if (name) return "Living Trump";
		if (desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif

		{
			if (cast)
			{
				///sys muta 人間トランプ　変異番号ハードコーディング
				int mutation;

				if (one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if (gain_random_mutation(mutation))
				{
#ifdef JP
					msg_print("あなたは生きているカードに変わった。");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "属性への免疫";
		if (desc) return "一定時間、冷気、炎、電撃、酸のいずれかに対する免疫を得る。";
#else
		if (name) return "Immunity";
		if (desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif

		{
			int base = 13;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_immune(base + randint1(base))) return NULL;
			}
		}
		break;
	}

	return "";
}


static cptr do_daemon_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "マジック・ミサイル";
		if (desc) return "弱い魔法の矢を放つ。";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "無生命感知";
		if (desc) return "近くの生命のないモンスターを感知する。";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "邪なる祝福";
		if (desc) return "一定時間、命中率とACにボーナスを得る。";
#else
		if (name) return "Evil Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif

		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "耐火炎";
		if (desc) return "一定時間、炎への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "恐慌";
		if (desc) return "モンスター1体を恐怖させ、朦朧させる。抵抗されると無効。";
#else
		if (name) return "Horrify";
		if (desc) return "Attempts to scare and stun a monster.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
				stun_monster(dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "地獄の矢";
		if (desc) return "地獄のボルトもしくはビームを放つ。";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether.";
#endif

		{
			int dice = 6 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "古代の死霊召喚";
		if (desc) return "古代の死霊を召喚する。";
#else
		if (name) return "Summon Manes";
		if (desc) return "Summons a manes.";
#endif

		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, (plev * 3) / 2, SUMMON_MANES, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("古代の死霊は現れなかった。");
#else
					msg_print("No Manes arrive.");
#endif
				}
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "地獄の焔";
		if (desc) return "邪悪な力を持つボールを放つ。善良なモンスターには大きなダメージを与える。";
#else
		if (name) return "Hellish Flame";
		if (desc) return "Fires a ball of evil power. Hurts good monsters greatly.";
#endif

		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class メイジ系は威力が上がる魔法
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "デーモン支配";
		if (desc) return "悪魔1体を魅了する。抵抗されると無効";
#else
		if (name) return "Dominate Demon";
		if (desc) return "Attempts to charm a demon.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_demon(dir, power);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "ビジョン";
		if (desc) return "周辺の地形を感知する。";
#else
		if (name) return "Vision";
		if (desc) return "Maps nearby area.";
#endif

		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "耐地獄";
		if (desc) return "一定時間、地獄への耐性を得る。";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "プラズマ・ボルト";
		if (desc) return "プラズマのボルトもしくはビームを放つ。";
#else
		if (name) return "Plasma bolt";
		if (desc) return "Fires a bolt or beam of plasma.";
#endif

		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_PLASMA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "ファイア・ボール";
		if (desc) return "炎の球を放つ。";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif

		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "炎の刃";
		if (desc) return "武器に炎の属性をつける。";
#else
		if (name) return "Fire Branding";
		if (desc) return "Makes current weapon fire branded.";
#endif

		{
			if (cast)
			{
				brand_weapon(1);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "地獄球";
		if (desc) return "大きな地獄の球を放つ。";
#else
		if (name) return "Nether Ball";
		if (desc) return "Fires a huge ball of nether.";
#endif

		{
			int dam = plev * 3 / 2 + 100;
			int rad = plev / 20 + 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "デーモン召喚";
		if (desc) return "悪魔1体を召喚する。";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif

		{
			if (cast)
			{
				bool pet = !one_in_(3);
				u32b summon_flags = 0L;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, plev*2/3+randint1(plev/2), SUMMON_DEMON, summon_flags))
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「卑しき者よ、我は汝の下僕にあらず！ お前の魂を頂くぞ！」");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("悪魔は現れなかった。");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "悪魔の目";
		if (desc) return "一定時間、テレパシー能力を得る。";
#else
		if (name) return "Devilish Eye";
		if (desc) return "Gives telepathy for a while.";
#endif

		{
			int base = 30;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(base) + sides, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "悪魔のクローク";
		if (desc) return "恐怖を取り除き、一定時間、炎と冷気の耐性、炎のオーラを得る。耐性は装備による耐性に累積する。";
#else
		if (name) return "Devil Cloak";
		if (desc) return "Removes fear. Gives resistance to fire and cold, and aura of fire. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int dur = randint1(base) + base;

				set_oppose_fire(dur, FALSE);
				set_oppose_cold(dur, FALSE);
				set_tim_sh_fire(dur, FALSE);
				set_afraid(0);
				break;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "溶岩流";
		if (desc) return "自分を中心とした炎の球を作り出し、床を溶岩に変える。";
#else
		if (name) return "The Flow of Lava";
		if (desc) return "Generates a ball of fire centered on you which transforms floors to magma.";
#endif

		{
			int dam = (55 + plev) * 2;
			int rad = 3;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_FIRE, 0, dam, rad);
				fire_ball_hide(GF_LAVA_FLOW, 0, 2 + randint1(2), rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "プラズマ球";
		if (desc) return "プラズマの球を放つ。";
#else
		if (name) return "Plasma Ball";
		if (desc) return "Fires a ball of plasma.";
#endif

		{
			int dam = plev * 3 / 2 + 80;
			int rad = 2 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_PLASMA, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "悪魔変化";
		if (desc) return "一定時間、悪魔に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔としての能力を得る。";
#else
		if (name) return "Polymorph Demon";
		if (desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif

		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "地獄の波動";
		if (desc) return "視界内の全てのモンスターにダメージを与える。善良なモンスターに特に大きなダメージを与える。";
#else
		if (name) return "Nather Wave";
		if (desc) return "Damages all monsters in sight. Hurts good monsters greatly.";
#endif

		{
			int sides1 = plev * 2;
			int sides2 = plev * 2;

			if (info) return format("%sd%d+d%d", s_dam, sides1, sides2);

			if (cast)
			{
				dispel_monsters(randint1(sides1));
				dispel_good(randint1(sides2));
			}
		}
		break;
///sysdel NEXUS
	case 22:
#ifdef JP
		if (name) return "サキュバスの接吻";
		if (desc) return "因果混乱の球を放つ。";
#else
		if (name) return "Kiss of Succubus";
		if (desc) return "Fires a ball of nexus.";
#endif

		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_NEXUS, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "破滅の手";
		if (desc) return "破滅の手を放つ。食らったモンスターはそのときのHPの半分前後のダメージを受ける。";
#else
		if (name) return "Doom Hand";
		if (desc) return "Attempts to make a monster's HP almost half.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				else msg_print("<破滅の手>を放った！");
#else
				else msg_print("You invoke the Hand of Doom!");
#endif

				fire_ball_hide(GF_HAND_DOOM, dir, plev * 2, 0);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "士気高揚";
		if (desc) return "一定時間、ヒーロー気分になる。";
#else
		if (name) return "Raise the Morale";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "不滅の肉体";
		if (desc) return "一定時間、時間逆転への耐性を得る。";
#else
		if (name) return "Immortal Body";
		if (desc) return "Gives resistance to time for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_time(randint1(base)+base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "狂気の円環";
		if (desc) return "自分を中心としたカオスの球、混乱の球を発生させ、近くのモンスターを魅了する。";
#else
		if (name) return "Insanity Circle";
		if (desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif

		{
			int dam = 50 + plev;
			int power = 20 + plev;
			int rad = 3 + plev / 20;

			if (info) return format("%s%d+%d", s_dam, dam/2, dam/2);

			if (cast)
			{
				fire_ball(GF_CHAOS, 0, dam, rad);
				fire_ball(GF_CONFUSION, 0, dam, rad);
				fire_ball(GF_CHARM, 0, power, rad);
			}
		}
		break;
		///sysdel death
	case 27:
#ifdef JP
		if (name) return "ペット爆破";
		if (desc) return "全てのペットを強制的に爆破させる。";
#else
		if (name) return "Explode Pets";
		if (desc) return "Makes all pets explode.";
#endif

		{
			if (cast)
			{
				discharge_minion();
			}
		}
		break;

		///sys item グレーターデーモン召喚　死体関係
	case 28:
#ifdef JP
		if (name) return "グレーターデーモン召喚";
		if (desc) return "上級デーモンを召喚する。召喚するには人間('p','h','t'で表されるモンスター)の死体を捧げなければならない。";
#else
		if (name) return "Summon Greater Demon";
		if (desc) return "Summons greater demon. It need to sacrifice a corpse of human ('p','h' or 't').";
#endif

		{
			if (cast)
			{
				if (!cast_summon_greater_demon()) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "地獄嵐";
		if (desc) return "超巨大な地獄の球を放つ。";
#else
		if (name) return "Nether Storm";
		if (desc) return "Generate a huge ball of nether.";
#endif

		{
			int dam = plev * 15;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "血の呪い";
		if (desc) return "自分がダメージを受けることによって対象に呪いをかけ、ダメージを与え様々な効果を引き起こす。";
#else
		if (name) return "Bloody Curse";
		if (desc) return "Puts blood curse which damages and causes various effects on a monster. You also take damage.";
#endif

		{
			int dam = 600;
			int rad = 0;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_BLOOD_CURSE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "血の呪い", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "魔王変化";
		if (desc) return "悪魔の王に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔の王としての能力を得、壁を破壊しながら歩く。";
#else
		if (name) return "Polymorph Demonlord";
		if (desc) return "Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.";
#endif

		{
			int base = 15;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON_LORD, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_crusade_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "懲罰";
		if (desc) return "電撃のボルトもしくはビームを放つ。";
#else
		if (name) return "Punishment";
		if (desc) return "Fires a bolt or beam of lightning.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "邪悪存在感知";
		if (desc) return "近くの邪悪なモンスターを感知する。";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "恐怖除去";
		if (desc) return "恐怖を取り除く。";
#else
		if (name) return "Remove Fear";
		if (desc) return "Removes fear.";
#endif

		{
			if (cast)
			{
				set_afraid(0);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "威圧";
		if (desc) return "モンスター1体を恐怖させる。抵抗されると無効。";
#else
		if (name) return "Scare Monster";
		if (desc) return "Attempts to scare a monster.";
#endif

		{
			//v1.1.90 強化
			int power = plev*2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "聖域";
		if (desc) return "隣接した全てのモンスターを眠らせる。抵抗されると無効。";
#else
		if (name) return "Sanctuary";
		if (desc) return "Attempts to sleep monsters in the adjacent squares.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				sleep_monsters_touch();
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "入口";
		if (desc) return "中距離のテレポートをする。";
#else
		if (name) return "Portal";
		if (desc) return "Teleport medium distance.";
#endif

		{
			int range = 25 + plev / 2;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "スターダスト";
		if (desc) return "ターゲット付近に閃光のボルトを連射する。";
#else
		if (name) return "Star Dust";
		if (desc) return "Fires many bolts of light near the target.";
#endif

		{
			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_LITE, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "身体浄化";
		if (desc) return "傷、毒、朦朧から全快する。";
#else
		if (name) return "Purify";
		if (desc) return "Heals all cut, stun and poison status.";
#endif

		{
			if (cast)
			{
				set_cut(0);
				set_poisoned(0);
				set_stun(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "邪悪飛ばし";
		if (desc) return "邪悪なモンスター1体をテレポートさせる。抵抗されると無効。";
#else
		if (name) return "Scatter Evil";
		if (desc) return "Attempts to teleport an evil monster away.";
#endif

		{
			int power = MAX_SIGHT * 5;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_AWAY_EVIL, dir, power, 0);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "聖なる光球";
		if (desc) return "聖なる力をもつ宝珠を放つ。邪悪なモンスターに対して大きなダメージを与えるが、善良なモンスターには効果がない。";
#else
		if (name) return "Holy Orb";
		if (desc) return "Fires a ball with holy power. Hurts evil monsters greatly, but don't effect good monsters.";
#endif

		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class メイジ系は威力が上がる魔法
			if (p_ptr->pclass == CLASS_PRIEST ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HOLY_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "退魔の呪文";
		if (desc) return "視界内のアンデッド・悪魔・妖怪にダメージを与え朦朧とさせる。";
#else
		if (name) return "Exorcism";
		if (desc) return "Damages all undead and demons in sight, and scares all evil monsters in sight.";
#endif

		{
			int dam = plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				dispel_undead(dam);
				dispel_demons(dam);
				dispel_kwai(dam);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "解呪";
		if (desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "透明視認";
		if (desc) return "一定時間、透明なものが見えるようになる。";
#else
		if (name) return "Sense Unseen";
		if (desc) return "Gives see invisible for a while.";
#endif

		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "対邪悪結界";
		if (desc) return "邪悪なモンスターの攻撃を防ぐバリアを張る。";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif

		{
			int base = 25;
			int sides = 3 * plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + sides, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "裁きの雷";
		if (desc) return "強力な電撃のボルトを放つ。";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif

		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt(GF_ELEC, dir, dam);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "聖なる御言葉";
		if (desc) return "視界内の邪悪な存在に大きなダメージを与え、体力を回復し、毒、恐怖、朦朧状態、負傷から全快する。";
#else
		if (name) return "Holy Word";
		if (desc) return "Damages all evil monsters in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif

		{
			int dam_sides = plev * 6;
			int heal = 100;

#ifdef JP
			if (info) return format("損:1d%d/回%d", dam_sides, heal);
#else
			if (info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if (cast)
			{
				dispel_evil(randint1(dam_sides));
				hp_player(heal);
				set_afraid(0);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "開かれた道";
		if (desc) return "一直線上の全ての罠と扉を破壊する。";
#else
		if (name) return "Unbarring Ways";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "封魔";
		if (desc) return "邪悪なモンスターの動きを止める。";
#else
		if (name) return "Arrest";
		if (desc) return "Attempts to paralyze an evil monster.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				stasis_evil(dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "聖なるオーラ";
		if (desc) return "一定時間、邪悪なモンスターを傷つける聖なるオーラを得る。";
#else
		if (name) return "Holy Aura";
		if (desc) return "Gives aura of holy power which injures evil monsters which attacked you for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_sh_holy(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "アンデッド&悪魔退散";
		if (desc) return "視界内の全てのアンデッド及び悪魔にダメージを与える。";
#else
		if (name) return "Dispel Undead & Demons";
		if (desc) return "Damages all undead and demons in sight.";
#endif

		{
			int sides = plev * 4;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_undead(randint1(sides));
				dispel_demons(randint1(sides));
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "邪悪退散";
		if (desc) return "視界内の全ての邪悪なモンスターにダメージを与える。";
#else
		if (name) return "Dispel Evil";
		if (desc) return "Damages all evil monsters in sight.";
#endif

		{
			int sides = plev * 4;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_evil(randint1(sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "聖なる刃";
		if (desc) return "通常の武器に滅邪の属性をつける。";
#else
		if (name) return "Holy Blade";
		if (desc) return "Makes current weapon especially deadly against evil monsters.";
#endif

		{
			if (cast)
			{
				brand_weapon(13);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "スターバースト";
		if (desc) return "巨大な閃光の球を放つ。";
#else
		if (name) return "Star Burst";
		if (desc) return "Fires a huge ball of powerful light.";
#endif

		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_LITE, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "天使召喚";
		if (desc) return "天使を1体召喚する。";
#else
		if (name) return "Summon Angel";
		if (desc) return "Summons an angel.";
#endif

		{
			if (cast)
			{
				bool pet = !one_in_(3);
				u32b summon_flags = 0L;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, SUMMON_ANGEL, summon_flags))
				{
					if (pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「我は汝の下僕にあらず！ 悪行者よ、悔い改めよ！」");
#else
						msg_print("Mortal! Repent of thy impiousness.");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "士気高揚";
		if (desc) return "一定時間、ヒーロー気分になる。";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "呪い退散";
		if (desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "邪悪追放";
		if (desc) return "視界内の全ての邪悪なモンスターをテレポートさせる。抵抗されると無効。";
#else
		if (name) return "Banish Evil";
		if (desc) return "Teleports all evil monsters in sight away unless resisted.";
#endif

		{
			int power = 100;

			if (info) return info_power(power);

			if (cast)
			{
				if (banish_evil(power))
				{
#ifdef JP
					msg_print("神聖な力が邪悪を打ち払った！");
#else
					msg_print("The holy power banishes evil!");
#endif

				}
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "ハルマゲドン";
		if (desc) return "周辺のアイテム、モンスター、地形を破壊する。";
#else
		if (name) return "Armageddon";
		if (desc) return "Destroy everything in nearby area.";
#endif

		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "目には目を";
		if (desc) return "一定時間、自分がダメージを受けたときに攻撃を行ったモンスターに対して同等のダメージを与える。";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif

		{
			int base = 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_eyeeye(randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "神の怒り";
		if (desc) return "ターゲットの周囲に分解の球を多数落とす。";
#else
		if (name) return "Wrath of the God";
		if (desc) return "Drops many balls of disintegration near the target.";
#endif

		{
			int dam = plev * 3 + 25;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "神威";
		if (desc) return "隣接するモンスターに聖なるダメージを与え、視界内のモンスターにダメージ、減速、朦朧、混乱、恐怖、眠りを与える。さらに体力を回復する。";
#else
		if (name) return "Divine Intervention";
		if (desc) return "Damages all adjacent monsters with holy power. Damages and attempt to slow, stun, confuse, scare and freeze all monsters in sight. And heals HP.";
#endif

		{
			int b_dam = plev * 11;
			int d_dam = plev * 4;
			int heal = 100;
			int power = plev * 4;

#ifdef JP
			if (info) return format("回%d/損%d+%d", heal, d_dam, b_dam/2);
#else
			if (info) return format("h%d/dm%d+%d", heal, d_dam, b_dam/2);
#endif

			if (cast)
			{
				project(0, 1, py, px, b_dam, GF_HOLY_FIRE, PROJECT_KILL, -1);
				dispel_monsters(d_dam);
				slow_monsters(plev);
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
				stasis_monsters(power);
				hp_player(heal);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "聖戦";
		if (desc) return "視界内の善良なモンスターをペットにしようとし、ならなかった場合及び善良でないモンスターを恐怖させる。さらに多数の加速された騎士を召喚し、ヒーロー、祝福、加速、対邪悪結界を得る。";
#else
		if (name) return "Crusade";
		if (desc) return "Attempts to charm all good monsters in sight, and scare all non-charmed monsters, and summons great number of knights, and gives heroism, bless, speed and protection from evil.";
#endif

		{
			if (cast)
			{
				int base = 25;
				int sp_sides = 20 + plev;
				int sp_base = plev;

				int i;
				crusade();
				for (i = 0; i < 12; i++)
				{
					int attempt = 10;
					int my, mx;

					while (attempt--)
					{
						scatter(&my, &mx, py, px, 4, 0);

						/* Require empty grids */
						if (cave_empty_bold2(my, mx)) break;
					}
					if (attempt < 0) continue;
					summon_specific(-1, my, mx, plev, SUMMON_KNIGHTS, (PM_ALLOW_GROUP | PM_FORCE_PET | PM_HASTE));
				}
				set_hero(randint1(base) + base, FALSE);
				set_blessed(randint1(base) + base, FALSE);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
				set_protevil(randint1(base) + base, FALSE);
				set_afraid(0);
			}
		}
		break;
	}

	return "";
}

///sys　realm 歌　しばらくは無効にしておく
static cptr do_music_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "遅鈍の歌";
		if (desc) return "視界内の全てのモンスターを減速させる。抵抗されると無効。";
#else
		if (name) return "Song of Holding";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("ゆっくりとしたメロディを口ずさみ始めた．．．");
#else
			msg_print("You start humming a slow, steady melody...");
#endif
			start_singing(spell, MUSIC_SLOW);
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				slow_monsters(plev);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "祝福の歌";
		if (desc) return "命中率とACのボーナスを得る。";
#else
		if (name) return "Song of Blessing";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("厳かなメロディを奏で始めた．．．");
#else
			msg_print("The holy power of the Music of the Ainur enters you...");
#endif
			start_singing(spell, MUSIC_BLESS);
		}

		if (stop)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("高潔な気分が消え失せた。");
#else
				msg_print("The prayer has expired.");
#endif
			}
		}

		break;

	case 2:
#ifdef JP
		if (name) return "崩壊の音色";
		if (desc) return "轟音のボルトを放つ。";
#else
		if (name) return "Wrecking Note";
		if (desc) return "Fires a bolt of sound.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		{
			int dice = 4 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_SOUND, dir, damroll(dice, sides));
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "朦朧の旋律";
		if (desc) return "視界内の全てのモンスターを朦朧させる。抵抗されると無効。";
#else
		if (name) return "Stun Pattern";
		if (desc) return "Attempts to stun all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("眩惑させるメロディを奏で始めた．．．");
#else
			msg_print("You weave a pattern of sounds to bewilder and daze...");
#endif
			start_singing(spell, MUSIC_STUN);
		}

		{
			int dice = plev / 10;
			int sides = 2;

			if (info) return info_power_dice(dice, sides);

			if (cont)
			{
				stun_monsters(damroll(dice, sides));
			}
		}

		break;

	case 4:
#ifdef JP
		if (name) return "生命の流れ";
		if (desc) return "体力を少し回復させる。";
#else
		if (name) return "Flow of Life";
		if (desc) return "Heals HP a little.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("歌を通して体に活気が戻ってきた．．．");
#else
			msg_print("Life flows through you as you sing a song of healing...");
#endif
			start_singing(spell, MUSIC_L_LIFE);
		}

		{
			int dice = 2;
			int sides = 6;

			if (info) return info_heal(dice, sides, 0);

			if (cont)
			{
				hp_player(damroll(dice, sides));
			}
		}

		break;

	case 5:
#ifdef JP
		if (name) return "太陽の歌";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Song of the Sun";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
#ifdef JP
				msg_print("光り輝く歌が辺りを照らした。");
#else
				msg_print("Your uplifting song brings brightness to dark places...");
#endif

				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "恐怖の歌";
		if (desc) return "視界内の全てのモンスターを恐怖させる。抵抗されると無効。";
#else
		if (name) return "Song of Fear";
		if (desc) return "Attempts to scare all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("おどろおどろしいメロディを奏で始めた．．．");
#else
			msg_print("You start weaving a fearful pattern...");
#endif
			start_singing(spell, MUSIC_FEAR);
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				project_hack(GF_TURN_ALL, power);
			}
		}

		break;

	case 7:
#ifdef JP
		if (name) return "戦いの歌";
		if (desc) return "ヒーロー気分になる。";
#else
		if (name) return "Heroic Ballad";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("激しい戦いの歌を歌った．．．");
#else
			msg_print("You start singing a song of intense fighting...");
#endif

			(void)hp_player(10);
			(void)set_afraid(0);

			/* Recalculate hitpoints */
			p_ptr->update |= (PU_HP);

			start_singing(spell, MUSIC_HERO);
		}

		if (stop)
		{
			if (!p_ptr->hero)
			{
#ifdef JP
				msg_print("ヒーローの気分が消え失せた。");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}
		}

		break;

	case 8:
#ifdef JP
		if (name) return "霊的知覚";
		if (desc) return "近くの罠/扉/階段を感知する。レベル15で全てのモンスター、20で財宝とアイテムを感知できるようになる。レベル25で周辺の地形を感知し、40でその階全体を永久に照らし、ダンジョン内のすべてのアイテムを感知する。この効果は歌い続けることで順に起こる。";
#else
		if (name) return "Clairaudience";
		if (desc) return "Detects traps, doors and stairs in your vicinity. And detects all monsters at level 15, treasures and items at level 20. Maps nearby area at level 25. Lights and know the whole level at level 40. These effects occurs by turns while this song continues.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("静かな音楽が感覚を研ぎ澄まさせた．．．");
#else
			msg_print("Your quiet music sharpens your sense of hearing...");
#endif

			/* Hack -- Initialize the turn count */
			p_ptr->magic_num1[2] = 0;

			start_singing(spell, MUSIC_DETECT);
		}

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cont)
			{
				int count = p_ptr->magic_num1[2];

				if (count >= 19) wiz_lite(FALSE);
				if (count >= 11)
				{
					map_area(rad);
					if (plev > 39 && count < 19)
						p_ptr->magic_num1[2] = count + 1;
				}
				if (count >= 6)
				{
					/* There are too many hidden treasure.  So... */
					/* detect_treasure(rad); */
					detect_objects_gold(rad);
					detect_objects_normal(rad);

					if (plev > 24 && count < 11)
						p_ptr->magic_num1[2] = count + 1;
				}
				if (count >= 3)
				{
					detect_monsters_invis(rad);
					detect_monsters_normal(rad);

					if (plev > 19 && count < 6)
						p_ptr->magic_num1[2] = count + 1;
				}
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);

				if (plev > 14 && count < 3)
					p_ptr->magic_num1[2] = count + 1;
			}
		}

		break;

	case 9:
#ifdef JP
		if (name) return "魂の歌";
		if (desc) return "視界内の全てのモンスターに対して精神攻撃を行う。";
#else
		if (name) return "Soul Shriek";
		if (desc) return "Damages all monsters in sight with PSI damages.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("精神を捻じ曲げる歌を歌った．．．");
#else
			msg_print("You start singing a song of soul in pain...");
#endif
			start_singing(spell, MUSIC_PSI);
		}

		{
			int dice = 1;
			int sides = plev * 3 / 2;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				project_hack(GF_PSI, damroll(dice, sides));
			}
		}

		break;

	case 10:
#ifdef JP
		if (name) return "知識の歌";
		if (desc) return "自分のいるマスと隣りのマスに落ちているアイテムを鑑定する。";
#else
		if (name) return "Song of Lore";
		if (desc) return "Identifies all items which are in the adjacent squares.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("この世界の知識が流れ込んできた．．．");
#else
			msg_print("You recall the rich lore of the world...");
#endif
			start_singing(spell, MUSIC_ID);
		}

		{
			int rad = 1;

			if (info) return info_radius(rad);

			/*
			 * 歌の開始時にも効果発動：
			 * MP不足で鑑定が発動される前に歌が中断してしまうのを防止。
			 */
			if (cont || cast)
			{
				project(0, rad, py, px, 0, GF_IDENTIFY, PROJECT_ITEM, -1);
			}
		}

		break;

	case 11:
#ifdef JP
		if (name) return "隠遁の歌";
		if (desc) return "隠密行動能力を上昇させる。";
#else
		if (name) return "Hiding Tune";
		if (desc) return "Gives improved stealth.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("あなたの姿が景色にとけこんでいった．．．");
#else
			msg_print("Your song carries you beyond the sight of mortal eyes...");
#endif
			start_singing(spell, MUSIC_STEALTH);
		}

		if (stop)
		{
			if (!p_ptr->tim_stealth)
			{
#ifdef JP
				msg_print("姿がはっきりと見えるようになった。");
#else
				msg_print("You are no longer hided.");
#endif
			}
		}

		break;

	case 12:
#ifdef JP
		if (name) return "幻影の旋律";
		if (desc) return "視界内の全てのモンスターを混乱させる。抵抗されると無効。";
#else
		if (name) return "Illusion Pattern";
		if (desc) return "Attempts to confuse all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("辺り一面に幻影が現れた．．．");
#else
			msg_print("You weave a pattern of sounds to beguile and confuse...");
#endif
			start_singing(spell, MUSIC_CONF);
		}

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cont)
			{
				confuse_monsters(power);
			}
		}

		break;

	case 13:
#ifdef JP
		if (name) return "破滅の叫び";
		if (desc) return "視界内の全てのモンスターに対して轟音攻撃を行う。";
#else
		if (name) return "Doomcall";
		if (desc) return "Damages all monsters in sight with booming sound.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("轟音が響いた．．．");
#else
			msg_print("The fury of the Downfall of Numenor lashes out...");
#endif
			start_singing(spell, MUSIC_SOUND);
		}

		{
			int dice = 10 + plev / 5;
			int sides = 7;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				project_hack(GF_SOUND, damroll(dice, sides));
			}
		}

		break;

	case 14:
#ifdef JP
		if (name) return "フィリエルの歌";
		if (desc) return "周囲の死体や骨を生き返す。";
#else
		if (name) return "Firiel's Song";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif

		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("生命と復活のテーマを奏で始めた．．．");
#else
				msg_print("The themes of life and revival are woven into your song...");
#endif

				animate_dead(0, py, px);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "旅の仲間";
		if (desc) return "視界内の全てのモンスターを魅了する。抵抗されると無効。";
#else
		if (name) return "Fellowship Chant";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("安らかなメロディを奏で始めた．．．");
#else
			msg_print("You weave a slow, soothing melody of imploration...");
#endif
			start_singing(spell, MUSIC_CHARM);
		}

		{
			int dice = 10 + plev / 15;
			int sides = 6;

			if (info) return info_power_dice(dice, sides);

			if (cont)
			{
				charm_monsters(damroll(dice, sides));
			}
		}

		break;

	case 16:
#ifdef JP
		if (name) return "分解音波";
		if (desc) return "壁を掘り進む。自分の足元のアイテムは蒸発する。";
#else
		if (name) return "Sound of disintegration";
		if (desc) return "Makes you be able to burrow into walls. Objects under your feet evaporate.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("粉砕するメロディを奏で始めた．．．");
#else
			msg_print("You weave a violent pattern of sounds to break wall.");
#endif
			start_singing(spell, MUSIC_WALL);
		}

		{
			/*
			 * 歌の開始時にも効果発動：
			 * MP不足で効果が発動される前に歌が中断してしまうのを防止。
			 */
			if (cont || cast)
			{
				project(0, 0, py, px,
					0, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE, -1);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "元素耐性";
		if (desc) return "酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Finrod's Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("元素の力に対する忍耐の歌を歌った。");
#else
			msg_print("You sing a song of perseverance against powers...");
#endif
			start_singing(spell, MUSIC_RESIST);
		}

		if (stop)
		{
			if (!p_ptr->oppose_acid)
			{
#ifdef JP
				msg_print("酸への耐性が薄れた気がする。");
#else
				msg_print("You feel less resistant to acid.");
#endif
			}

			if (!p_ptr->oppose_elec)
			{
#ifdef JP
				msg_print("電撃への耐性が薄れた気がする。");
#else
				msg_print("You feel less resistant to elec.");
#endif
			}

			if (!p_ptr->oppose_fire)
			{
#ifdef JP
				msg_print("火への耐性が薄れた気がする。");
#else
				msg_print("You feel less resistant to fire.");
#endif
			}

			if (!p_ptr->oppose_cold)
			{
#ifdef JP
				msg_print("冷気への耐性が薄れた気がする。");
#else
				msg_print("You feel less resistant to cold.");
#endif
			}

			if (!p_ptr->oppose_pois)
			{
#ifdef JP
				msg_print("毒への耐性が薄れた気がする。");
#else
				msg_print("You feel less resistant to pois.");
#endif
			}
		}

		break;

	case 18:
#ifdef JP
		if (name) return "ホビットのメロディ";
		if (desc) return "加速する。";
#else
		if (name) return "Hobbit Melodies";
		if (desc) return "Hastes you.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("軽快な歌を口ずさみ始めた．．．");
#else
			msg_print("You start singing joyful pop song...");
#endif
			start_singing(spell, MUSIC_SPEED);
		}

		if (stop)
		{
			if (!p_ptr->fast)
			{
#ifdef JP
				msg_print("動きの素早さがなくなったようだ。");
#else
				msg_print("You feel yourself slow down.");
#endif
			}
		}

		break;

	case 19:
#ifdef JP
		if (name) return "歪んだ世界";
		if (desc) return "近くのモンスターをテレポートさせる。抵抗されると無効。";
#else
		if (name) return "World Contortion";
		if (desc) return "Teleports all nearby monsters away unless resisted.";
#endif

		{
			int rad = plev / 15 + 1;
			int power = plev * 3 + 1;

			if (info) return info_radius(rad);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("歌が空間を歪めた．．．");
#else
				msg_print("Reality whirls wildly as you sing a dizzying melody...");
#endif

				project(0, rad, py, px, power, GF_AWAY_ALL, PROJECT_KILL, -1);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "退散の歌";
		if (desc) return "視界内の全てのモンスターにダメージを与える。邪悪なモンスターに特に大きなダメージを与える。";
#else
		if (name) return "Dispelling chant";
		if (desc) return "Damages all monsters in sight. Hurts evil monsters greatly.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("耐えられない不協和音が敵を責め立てた．．．");
#else
			msg_print("You cry out in an ear-wracking voice...");
#endif
			start_singing(spell, MUSIC_DISPEL);
		}

		{
			int m_sides = plev * 3;
			int e_sides = plev * 3;

			if (info) return format("%s1d%d+1d%d", s_dam, m_sides, e_sides);

			if (cont)
			{
				dispel_monsters(randint1(m_sides));
				dispel_evil(randint1(e_sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "サルマンの甘言";
		if (desc) return "視界内の全てのモンスターを減速させ、眠らせようとする。抵抗されると無効。";
#else
		if (name) return "The Voice of Saruman";
		if (desc) return "Attempts to slow and sleep all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("優しく、魅力的な歌を口ずさみ始めた．．．");
#else
			msg_print("You start humming a gentle and attractive song...");
#endif
			start_singing(spell, MUSIC_SARUMAN);
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				slow_monsters(plev);
				sleep_monsters(plev);
			}
		}

		break;

	case 22:
#ifdef JP
		if (name) return "嵐の音色";
		if (desc) return "轟音のビームを放つ。";
#else
		if (name) return "Song of the Tempest";
		if (desc) return "Fires a beam of sound.";
#endif

		{
			int dice = 15 + (plev - 1) / 2;
			int sides = 10;

			if (info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_SOUND, dir, damroll(dice, sides));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "もう一つの世界";
		if (desc) return "現在の階を再構成する。";
#else
		if (name) return "Ambarkanta";
		if (desc) return "Recreates current dungeon level.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("周囲が変化し始めた．．．");
#else
				msg_print("Your surroundings start to change...");
#endif

				alter_reality();
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "破壊の旋律";
		if (desc) return "周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。";
#else
		if (name) return "Wrecking Pattern";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("破壊的な歌が響きわたった．．．");
#else
			msg_print("You weave a pattern of sounds to contort and shatter...");
#endif
			start_singing(spell, MUSIC_QUAKE);
		}

		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cont)
			{
				earthquake(py, px, 10);
			}
		}

		break;


	case 25:
#ifdef JP
		if (name) return "停滞の歌";
		if (desc) return "視界内の全てのモンスターを麻痺させようとする。抵抗されると無効。";
#else
		if (name) return "Stationary Shriek";
		if (desc) return "Attempts to freeze all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("ゆっくりとしたメロディを奏で始めた．．．");
#else
			msg_print("You weave a very slow pattern which is almost likely to stop...");
#endif
			start_singing(spell, MUSIC_STASIS);
		}

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cont)
			{
				stasis_monsters(power);
			}
		}

		break;

	case 26:
#ifdef JP
		if (name) return "守りの歌";
		if (desc) return "自分のいる床の上に、モンスターが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if (name) return "Endurance";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif

		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("歌が神聖な場を作り出した．．．");
#else
				msg_print("The holy power of the Music is creating sacred field...");
#endif

				warding_glyph();
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "英雄の詩";
		if (desc) return "加速し、ヒーロー気分になり、視界内の全てのモンスターにダメージを与える。";
#else
		if (name) return "The Hero's Poem";
		if (desc) return "Hastes you. Gives heroism. Damages all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("英雄の歌を口ずさんだ．．．");
#else
			msg_print("You chant a powerful, heroic call to arms...");
#endif
			(void)hp_player(10);
			(void)set_afraid(0);

			/* Recalculate hitpoints */
			p_ptr->update |= (PU_HP);

			start_singing(spell, MUSIC_SHERO);
		}

		if (stop)
		{
			if (!p_ptr->hero)
			{
#ifdef JP
				msg_print("ヒーローの気分が消え失せた。");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}

			if (!p_ptr->fast)
			{
#ifdef JP
				msg_print("動きの素早さがなくなったようだ。");
#else
				msg_print("You feel yourself slow down.");
#endif
			}
		}

		{
			int dice = 1;
			int sides = plev * 3;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				dispel_monsters(damroll(dice, sides));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "ヤヴァンナの助け";
		if (desc) return "強力な回復の歌で、負傷と朦朧状態も全快する。";
#else
		if (name) return "Relief of Yavanna";
		if (desc) return "Powerful healing song. Also heals cut and stun completely.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("歌を通して体に活気が戻ってきた．．．");
#else
			msg_print("Life flows through you as you sing the song...");
#endif
			start_singing(spell, MUSIC_H_LIFE);
		}

		{
			int dice = 15;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cont)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}

		break;

	case 29:
#ifdef JP
		if (name) return "再生の歌";
		if (desc) return "すべてのステータスと経験値を回復する。";
#else
		if (name) return "Goddess' rebirth";
		if (desc) return "Restores all stats and experience.";
#endif

		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("暗黒の中に光と美をふりまいた。体が元の活力を取り戻した。");
#else
				msg_print("You strewed light and beauty in the dark as you sing. You feel refreshed.");
#endif
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_CHR);
				(void)restore_level();
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "サウロンの魔術";
		if (desc) return "非常に強力でごく小さい轟音の球を放つ。";
#else
		if (name) return "Wizardry of Sauron";
		if (desc) return "Fires an extremely powerful tiny ball of sound.";
#endif

		{
			int dice = 50 + plev;
			int sides = 10;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_SOUND, dir, damroll(dice, sides), rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "フィンゴルフィンの挑戦";
		if (desc) return "ダメージを受けなくなるバリアを張る。";
#else
		if (name) return "Fingolfin's Challenge";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
				msg_print("フィンゴルフィンの冥王への挑戦を歌った．．．");
#else
				msg_print("You recall the valor of Fingolfin's challenge to the Dark Lord...");
#endif

				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				start_singing(spell, MUSIC_INVULN);
		}

		if (stop)
		{
			if (!p_ptr->invuln)
			{
#ifdef JP
				msg_print("無敵ではなくなった。");
#else
				msg_print("The invulnerability wears off.");
#endif
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
		}

		break;
	}

	return "";
}


///mod140817 歌集でなく職業ごと特技から直接使う音楽専用　ミスティアなどが使う
//cast:使用時　cont:継続時一定ゲームターンごと効果 stop:中止時メッセージ
//吟遊詩人とは違い継続系の歌だけなのでstart_singingのsong_indexとspell_indexは同じにする
//継続時コスト計算のためにcheck_music()に別処理の記述が必要
static cptr do_new_spell_music2(int spell, int mode)
{
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	int dir;
	int plev = p_ptr->lev;

	int cont_count = 0;

	if(!cast && !cont && !stop)
	{
		msg_format(_("ERROR:do_new_spell_music2()が未定義のmode(%d)で呼ばれた",
                    "ERROR: do_new_spell_music2() called with undefined mode (%d)"),mode);
		return NULL;
	}

	//p_ptr->magic_num1[2](開始してからの経過ターン)の処理;
	if (cont)
	{
		if(p_ptr->magic_num1[2] < 1000)p_ptr->magic_num1[2]++;
		cont_count = p_ptr->magic_num1[2];
	}
	else
	{
		p_ptr->magic_num1[2] = 0;

	}


	switch (spell)
	{
	case MUSIC_NONE: //歌うのをやめたとき
		break;

	case MUSIC_NEW_MYSTIA_YAZYAKU: //ミスティア　夜雀の歌
   		if (cast)
		{
			stop_singing();
			msg_print(_("朧な歌声が響き渡った・・", "You start singing..."));
			start_singing(MUSIC_NEW_MYSTIA_YAZYAKU, MUSIC_NEW_MYSTIA_YAZYAKU);
		}

		if (cont)
		{
			//何もしない　夜盲処理は別に記述
		}
		break;
	case MUSIC_NEW_MYSTIA_MYSTERIOUS: //ミスティア　ミステリアスソング
   		if (cast)
		{

			stop_singing();
			msg_print(_("不思議な歌声が響き渡った・・", "You start singing a mysterious song..."));
			start_singing(MUSIC_NEW_MYSTIA_MYSTERIOUS, MUSIC_NEW_MYSTIA_MYSTERIOUS);
		}

		if (cont)
		{
			int power = plev * 2+ chr_adj * 2;
			project_hack(GF_REDEYE, power);
			project_hack(GF_DEC_MAG, power );
		}
		break;
	case MUSIC_NEW_MYSTIA_CHORUSMASTER: //ミスティア　真夜中のコーラスマスター
   		if (cast)
		{
			stop_singing();
			msg_print(_("妖しい歌声が響き渡った・・", "You start singing a bewitching song..."));
			start_singing(MUSIC_NEW_MYSTIA_CHORUSMASTER, MUSIC_NEW_MYSTIA_CHORUSMASTER);
		}

		if (cont)
		{
				charm_monsters(randint1(chr_adj * 5) );
		}
		break;
	case MUSIC_NEW_KYOUKO_NENBUTSU: //響子　無限念仏
   		if (cast)
		{
			stop_singing();
			msg_print(_("大声で念仏を唱え始めた。", "You started chanting a prayer in a loud voice."));
			start_singing(MUSIC_NEW_KYOUKO_NENBUTSU, MUSIC_NEW_KYOUKO_NENBUTSU);
		}

		if (cont)
		{
			int dice = 5 + plev/5;
			if(one_in_(2)) msg_print(_("「ぎゃ～て～ぎゃ～て～・・」", "'Gya~te~gya~te~...'"));
			else msg_print(_("「ぜ～む～と～ど～しゅ～・・」", "'Ze~mu~to~do~shu~...'"));
			project_hack(GF_PUNISH_1, damroll(dice,dice));
		}
		break;
	case MUSIC_NEW_KYOUKO_SCREAM: //響子　プライマルスクリーム
   		if (cast)
		{
			stop_singing();
			msg_print(_("あなたは大きく息を吸い込んだ・・", "You take a deep breath..."));
			start_singing(MUSIC_NEW_KYOUKO_SCREAM, MUSIC_NEW_KYOUKO_SCREAM);
		}

		if (cont)
		{

			if(one_in_(5))msg_print(_("髪を振り乱し熱唱した！", "You sing enthusiastically, shaking your hair!"));
			else if(one_in_(4))msg_print(_("魂のシャウトを放った！", "You release the shout of your soul!"));
			else if(one_in_(3))msg_print(_("爆音がダンジョンを揺らした！", "The dungeon shakes from the loud noise!"));
			else if(one_in_(2))msg_print(_("抑圧された不満を叫んだ！", "You shout out in protest of your oppression!"));
			else msg_print(_("お寺の修行の辛さを大声で愚痴った！", "You loudly complain about the pain of training at the temple!"));

			project_hack(GF_SOUND, randint1(plev * 2));
			aggravate_monsters(0,FALSE);

		}
		break;
	case MUSIC_NEW_LUNASA_SOLO:
		{
			if(cast)
			{
				start_singing(MUSIC_NEW_LUNASA_SOLO, MUSIC_NEW_LUNASA_SOLO);
			}
			else if(cont)
			{
				monster_type *m_ptr;
				monster_race *r_ptr;
				char m_name[80];
				int i;
				int delay;

				msg_print(_("ヴァイオリンが陰鬱な音を奏でた・・", "You make the violin play a gloomy melody..."));
				for (i = 1; (i < m_max); i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(p_ptr->riding == i) continue;
					if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
					monster_desc(m_name, m_ptr, 0);
					delay = plev + randint1(plev);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) delay /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) delay -= 30;
					if(r_ptr->flags2 & RF2_POWERFUL) delay -= 20;
					if(r_ptr->flags2 & RF2_EMPTY_MIND) delay = 0;
					if(delay < 0) delay = 0;
					m_ptr->energy_need += delay;

					if(!delay)
						msg_format(_("%sは全く怯んでいない。", "%s does not falter."),m_name);
					else if(delay < 30)
						msg_format(_("%sは少し怯んでいる。", "%s falters slightly."),m_name);
					else
						msg_format(_("%sは気が滅入っているようだ。", "%s looks depressed."),m_name);

				}
			}
			break;
		}
	case MUSIC_NEW_MERLIN_SOLO:
			if(cast)
			{
				msg_format(_("あなたは底抜けにハイテンションな曲を演奏し始めた！",
                            "You start playing a high-tension song!"));
				start_singing(MUSIC_NEW_MERLIN_SOLO, MUSIC_NEW_MERLIN_SOLO);
			}

		break;
	case MUSIC_NEW_LYRICA_SOLO:
			if(cast)
			{
				msg_format(_("あなたは幻想の音を奏で始めた・・",
                            "You started playing a phantasmal melody..."));
				start_singing(MUSIC_NEW_LYRICA_SOLO, MUSIC_NEW_LYRICA_SOLO);
			}
		break;
	case MUSIC_NEW_TSUKUMO_HEIKE: //弁々　平家の大怨霊
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("物悲しい琵琶の音が響いた・・。", "A sad biwa melody echoes..."));
			start_singing(MUSIC_NEW_TSUKUMO_HEIKE, MUSIC_NEW_TSUKUMO_HEIKE);
		}

		if (cont)
		{
			int dice = 7 + plev/6;
			fire_random_target(GF_CAUSE_1,damroll(dice,dice),4,0,0);
		}
		break;
	case MUSIC_NEW_TSUKUMO_STORM: //弁々＆八橋　嵐のアンサンブル
   		if (cast)
		{
			stop_tsukumo_music();
			if(p_ptr->pclass == CLASS_BENBEN) msg_print(_("八橋と共に激しい音楽を奏ではじめた。",
                                                            "You started an intense performance along with Yatsuhashi."));
			else msg_print(_("弁々と共に激しい音楽を奏ではじめた。",
                                "You started an intense performance along with Yatsuhashi."));
			start_singing(MUSIC_NEW_TSUKUMO_STORM, MUSIC_NEW_TSUKUMO_STORM);
		}

		if (cont)
		{
			int base = plev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 2;

			msg_print(_("魔力を伴う大嵐を巻き起こした！", "You cause a great storm of magical power!"));
			project_hack2(GF_TSUKUMO_STORM,1,base,0);

		}
		break;

	case MUSIC_NEW_TSUKUMO_SCOREWEB: //弁々スコアウェブ
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("あなたは激しく琵琶をかき鳴らし始めた。", "Your biwa starts strumming violently..."));
			start_singing(MUSIC_NEW_TSUKUMO_SCOREWEB, MUSIC_NEW_TSUKUMO_SCOREWEB);
		}

		if (cont)
		{
			int dam = plev/2 + chr_adj / 2;
			int num = 5;
			for(;num>0;num--) fire_random_target(GF_LITE,dam,2,0,0);


		}
		break;

	case MUSIC_NEW_TSUKUMO_DOUBLESCORE: //弁々ダブルスコア
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("無数の光があなたの周りに渦巻いた・・", "Countless lights start swirling around you..."));
			start_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE, MUSIC_NEW_TSUKUMO_DOUBLESCORE);
		}
		break;
	case MUSIC_NEW_TSUKUMO_NORIGOTO: //八橋　天の詔琴
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("あなたは攻撃を待ち構えている。", "You prepare for incoming attacks."));
			start_singing(MUSIC_NEW_TSUKUMO_NORIGOTO, MUSIC_NEW_TSUKUMO_NORIGOTO);
		}
		break;
	case MUSIC_NEW_TSUKUMO_JINKIN: //八橋　人琴ともに亡ぶ
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("あなたは胸を締め付けるような旋律を奏で始めた。",
                        "You started playing a chest-tightening tune."));
			start_singing(MUSIC_NEW_TSUKUMO_JINKIN, MUSIC_NEW_TSUKUMO_JINKIN);
		}

		break;
	case MUSIC_NEW_TSUKUMO_ECHO: //八橋　エコーチェンバー
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("あなたの周りに魔力が渦巻いた・・", "Magical power swirls around you..."));
			start_singing(MUSIC_NEW_TSUKUMO_ECHO, MUSIC_NEW_TSUKUMO_ECHO);
		}
		break;
	case MUSIC_NEW_TSUKUMO_ECHO2: //八橋　平安の残響
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print(_("あなたの音は奇妙な反響をし始めた。",
                        "Your melody start echoing strangely."));
			start_singing(MUSIC_NEW_TSUKUMO_ECHO2, MUSIC_NEW_TSUKUMO_ECHO2);
		}

		break;

	case MUSIC_NEW_RAIKO_AYANOTUDUMI:
		//アヤノツヅミ　未実装
		if (cast)
		{
			stop_raiko_music();
			msg_print(_("幽かな響きがダンジョンに染み入った．．．",
                        "Mysterious sounds permeate the dungeon..."));

			start_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI, MUSIC_NEW_RAIKO_AYANOTUDUMI);
		}

		{//castでもcontでもここに入る
			int rad = DETECT_RAD_DEFAULT;


			if (cont)
			{
				if (plev > 24 )
				{
					map_area(rad);
				}
				if (plev > 29 )
				{
					detect_objects_gold(rad);
					detect_objects_normal(rad);
				}
				if (plev > 34)
				{
					detect_traps(rad, TRUE);
				}
				detect_monsters_invis(rad);
				detect_monsters_normal(rad);


			}
		}

		break;
	case MUSIC_NEW_RAIKO_DENDEN: //怒りのデンデン太鼓
   		if (cast)
		{
			stop_raiko_music();
			msg_print(_("あなたは激しくドラムを打ち鳴らし始めた！",
                        "You start violently beating your drum!"));
			start_singing(MUSIC_NEW_RAIKO_DENDEN, MUSIC_NEW_RAIKO_DENDEN);
		}
		if (cont)
		{
			int dam = plev*3 + randint1(adj_general[p_ptr->stat_ind[A_CHR]]*5);
			int typ;
			if(one_in_(2)) typ = GF_ELEC;
			else typ = GF_SOUND;
			fire_random_target(typ,dam,2,0,0);

		}
		break;
	case MUSIC_NEW_RAIKO_PRISTINE: //プリスティンビート
   		if (cast)
		{
			stop_raiko_music();
			msg_print(_("始原の魂を呼び覚ました！", "You awaken your pristine soul!"));
			start_singing(MUSIC_NEW_RAIKO_PRISTINE, MUSIC_NEW_RAIKO_PRISTINE);
		}
		break;
	case MUSIC_NEW_MYSTIA_HUMAN_CAGE: //ミスティア　ヒューマンケージ
		if (cast)
		{
			stop_singing();
			msg_print(_("胸を締め付けるような歌声が響き渡った・・",
                        "You start singing a chest-tightening song..."));
			start_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE, MUSIC_NEW_MYSTIA_HUMAN_CAGE);
		}
		if (cont)
		{
			//何もしない　処理は別に記述
		}
		break;

	case MUSIC_NEW_WAKASAGI_SCHOOLOFFISH: //わかさぎ姫　スクールオブフィッシュ
		if (cast)
		{
			stop_singing();
			msg_print(_("あなたは魚たちに向けて楽しげに歌い始めた。",
                        "You start singing happily to the fish."));
			start_singing(MUSIC_NEW_WAKASAGI_SCHOOLOFFISH, MUSIC_NEW_WAKASAGI_SCHOOLOFFISH);
		}

		if (cont)
		{
			project_hack(GF_CONTROL_FISH, plev + chr_adj * 3);
		}
		break;

	case MUSIC_NEW_SANNYO_SMOKE:
		if (cast)
		{
			msg_print(_("あなたはお気に入りの煙草を吸い始めた。",
                        "You start smoking your favorite tobacco."));
			start_singing(MUSIC_NEW_SANNYO_SMOKE, MUSIC_NEW_SANNYO_SMOKE);
		}

		if (cont)
		{
			player_gain_mana( 1 + plev / 15 + randint1((plev+10) / 15));
		}
		break;

	case MUSIC_NEW_SANNYO_FEAR:
		if (cast)
		{
			msg_print(_("あなたは苦み走る煙草を吸い始めた。",
                        "You start smoking bitter tobacco."));
			start_singing(MUSIC_NEW_SANNYO_FEAR, MUSIC_NEW_SANNYO_FEAR);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);

			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_TURN_ALL,(PROJECT_KILL|PROJECT_JUMP|PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_CONF:
		if (cast)
		{
			msg_print(_("あなたは甘ったるい煙草を吸い始めた。",
                        "You start smoking sweet tobacco."));
			start_singing(MUSIC_NEW_SANNYO_CONF, MUSIC_NEW_SANNYO_CONF);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_OLD_CONF, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;

	case MUSIC_NEW_SANNYO_STUN:
		if (cast)
		{
			msg_print(_("あなたは突き抜けるような煙草を吸い始めた。",
                        "You start smoking stunning tobacco."));
			start_singing(MUSIC_NEW_SANNYO_STUN, MUSIC_NEW_SANNYO_STUN);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_STUN, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;

	case MUSIC_NEW_SANNYO_SLOW:
		if (cast)
		{
			msg_print(_("あなたはずっしり重い煙草を吸い始めた。",
                        "You start smoking heavy tobacco."));
			start_singing(MUSIC_NEW_SANNYO_SLOW, MUSIC_NEW_SANNYO_SLOW);
		}

		if (cont)
		{
			int power = 20 + plev * 2+ cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_OLD_SLOW, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_MINDBLAST:
		if (cast)
		{
			msg_print(_("あなたは激烈に濃厚な煙草を吸い始めた。",
                        "You start smoking intensely rich tobacco."));
			start_singing(MUSIC_NEW_SANNYO_MINDBLAST, MUSIC_NEW_SANNYO_MINDBLAST);
		}

		if (cont)
		{
			int power = 50 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_REDEYE, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_BERSERK:
		if (cast)
		{
			msg_print(_("とっておきの煙草葉に火を点けた！",
                        "You light up your special tobacco!"));
			start_singing(MUSIC_NEW_SANNYO_BERSERK, MUSIC_NEW_SANNYO_BERSERK);
			set_shero(500, FALSE);
			p_ptr->update |= PU_BONUS;
			update_stuff();
			hp_player(1000);

		}
		if (stop)
		{
			set_shero(0, TRUE);
			p_ptr->update |= PU_BONUS;
			update_stuff();
		}
		break;


		//継続時コスト計算のためにcheck_music()にも処理の記述が必要

		//13辺りから元の吟遊詩人効果が影響してくる。注意。
		//↑MUSIC_XXXXに+100して使えなくしといた

	default:
		msg_format(_("ERROR:この音楽IDX(%d)がdo_new_spell_music2()に定義されていない",
                    "ERROR: Undefined idx (%d) for this music in do_new_spell_music2()"),spell);
		return NULL;
	}

	return "";
}

/*:::剣術家の必殺剣*/
///mod140913
/*:::モード"check"を導入。武器の種類などを検証し技を使用可能なら"",不可能ならNULLを返す。*/
static cptr do_hissatsu_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool check =  (mode == SPELL_CHECK) ? TRUE : FALSE;

	bool have_weapon = buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM);
	bool have_edge = (object_has_a_blade(&inventory[INVEN_RARM]) || object_has_a_blade(&inventory[INVEN_LARM]));
	bool have_sword = (inventory[INVEN_RARM].tval == TV_SWORD || inventory[INVEN_LARM].tval == TV_SWORD ||inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA );



	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "飛飯綱";
		if (desc) return "(2+レベル/16)マス離れたところにいるモンスターを攻撃する。刀を持っていないと使えない。";
#else
        if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "Tobi-Izuna";
		if (desc) return "Attacks a two squares distant monster. Requires wielding a katana.";
#endif

		if (cast)
		{
			project_length = 2 + plev / 16;
			if (!get_aim_dir(&dir)) return NULL;

			project_hook(GF_ATTACK, dir, HISSATSU_2, PROJECT_STOP | PROJECT_KILL);
		}
		break;

	case 1:
		if(check)
		{
			if(have_edge) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "五月雨斬り";
		if (desc) return "3方向に対して攻撃する。刃のある武器を持っていないと使えない。";
#else
		if (name) return "3-Way Attack";
		if (desc) return "Attacks in 3 directions in one time. Requires wielding an edged weapon.";
#endif

		if (cast)
		{
			int cdir;
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			for (cdir = 0;cdir < 8; cdir++)
			{
				if (cdd[cdir] == dir) break;
			}

			if (cdir == 8) return NULL;

			y = py + ddy_cdd[cdir];
			x = px + ddx_cdd[cdir];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
			y = py + ddy_cdd[(cdir + 7) % 8];
			x = px + ddx_cdd[(cdir + 7) % 8];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
			y = py + ddy_cdd[(cdir + 1) % 8];
			x = px + ddx_cdd[(cdir + 1) % 8];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("攻撃は空を切った。");
#else
				msg_print("You attack the empty air.");
#endif
		}
		break;

	case 2:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "ブーメラン";
		if (desc) return "武器を手元に戻ってくるように投げる。戻ってこないことがあり、受け止めそこねることもある。元々投げた時に戻ってくる武器の場合、さらに戻りやすく受けやすくなる。何か武器を持っていないと使えない。";
#else
		if (name) return "Boomerang";
		if (desc) return "Throws current weapon. And it'll return to your hand unless failed.";
#endif

		if (cast)
		{
			if (!do_cmd_throw_aux(1, TRUE, -1)) return NULL;
		}
		break;

	case 3:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "焔霊";
		if (desc) return "火炎耐性のないモンスターに大ダメージを与える。何か武器を持っていないと使えない。";
#else
		if (name) return "Burning Strike";
		if (desc) return "Attacks a monster with more damage unless it has resistance to fire. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_FIRE);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 4:
		if(check) return "";
#ifdef JP
		if (name) return "殺気感知";
		if (desc) return "近くの思考することができるモンスターを感知する。";
#else
		if (name) return "Detect Ferocity";
		if (desc) return "Detects all monsters except mindless in your vicinity.";
#endif

		if (cast)
		{
			detect_monsters_mind(DETECT_RAD_DEFAULT);
		}
		break;

	case 5:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "みね打ち";
		if (desc) return "相手にダメージを与えないが、朦朧とさせる。何か武器を持っていないと使えない。";
#else
		if (name) return "Strike to Stun";
		if (desc) return "Attempts to stun a monster in the adjacent. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_MINEUCHI);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 6:
#ifdef JP
		if(check) return "";
		if (name) return "カウンター";
		if (desc) return "相手に攻撃されたときに反撃する。反撃するたびにMPを消費する。";
#else
        if(check) return "";
		if (name) return "Counter";
		if (desc) return "Prepares to counterattack. When attack by a monster, strikes back using SP each time.";
#endif

		if (cast)
		{
			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("乗馬中には無理だ。");
#else
				msg_print("You cannot do it when riding.");
#endif
				return NULL;
			}
#ifdef JP
			msg_print("相手の攻撃に対して身構えた。");
#else
			msg_print("You prepare to counter blow.");
#endif
			p_ptr->counter = TRUE;
		}
		break;

	case 7:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "払い抜け";
		if (desc) return "攻撃した後、反対側に抜ける。何か武器を持っていないと使えない。";
#else
		if (name) return "Harainuke";
		if (desc) return "Attacks monster with your weapons normally, then move through counter side of the monster. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("乗馬中には無理だ。");
#else
				msg_print("You cannot do it when riding.");
#endif
				return NULL;
			}

			if (!get_rep_dir2(&dir)) return NULL;

			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!cave[y][x].m_idx)
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}

			py_attack(y, x, 0);

			if (!player_can_enter(cave[y][x].feat, 0) || is_trap(cave[y][x].feat))
				break;

			y += ddy[dir];
			x += ddx[dir];

			if (player_can_enter(cave[y][x].feat, 0) && !is_trap(cave[y][x].feat) && !cave[y][x].m_idx)
			{
				msg_print(NULL);

				/* Move the player */
				(void)move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
			}
		}
		break;

	case 8:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "サーペンツタン";
		if (desc) return "毒耐性のないモンスターに大ダメージを与える。何か武器を持っていないと使えない。";
#else
		if (name) return "Serpent's Tongue";
		if (desc) return "Attacks a monster with more damage unless it has resistance to poison. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_POISON);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 9:
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "斬魔剣弐の太刀";
		if (desc) return "混沌の勢力に属するアンデッドや悪魔に大ダメージを与えるが、他のモンスターには全く効果がない。剣か刀を装備していないと使えない。";
#else
		if (name) return "Zammaken";
		if (desc) return "Attacks an evil unliving monster with great damage. No effect to other  monsters. Requires wielding a sword or a katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_ZANMA);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "裂風剣";
		if (desc) return "攻撃した相手を後方へ吹き飛ばす。剣か刀を装備していないと使えない。";
#else
        if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "Wind Blast";
		if (desc) return "Attacks an adjacent monster, and blow it away. Requires wielding a sword or a katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_FUKI);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				return "";
			}
			if (cave[y][x].m_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				char m_name[80];

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
#ifdef JP
					msg_format("%sを吹き飛ばした！", m_name);
#else
					msg_format("You blow %s away!", m_name);
#endif
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
		break;

	case 11:
#ifdef JP
		if(check) return "";
		if (name) return "刀匠の目利き";
		if (desc) return "武器・防具を1つ識別する。レベル45以上で武器・防具の能力を完全に知ることができる。";
#else
        if(check) return "";
		if (name) return "Judge";
		if (desc) return "Identifies a weapon or armor. Or *identifies* these at level 45.";
#endif

		if (cast)
		{
			if (plev > 44)
			{
				if (!identify_fully(TRUE)) return NULL;
			}
			else
			{
				if (!ident_spell(TRUE)) return NULL;
			}
		}
		break;

	case 12:
#ifdef JP
		if(check)
		{
			if(have_edge) return "";
			else return NULL;
		}
		if (name) return "破岩斬";
		if (desc) return "岩を壊し、岩石系のモンスターに大ダメージを与える。刃のある武器を装備していないと使えない。";
#else
        if(check)
		{
			if(have_edge) return "";
			else return NULL;
		}
		if (name) return "Rock Smash";
		if (desc) return "Breaks rock. Or greatly damage a monster made by rocks. Requires wielding an edged weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_HAGAN);

			if (!cave_have_flag_bold(y, x, FF_HURT_ROCK)) break;

			/* Destroy the feature */
			cave_alter_feat(y, x, FF_HURT_ROCK);

			/* Update some things */
			p_ptr->update |= (PU_FLOW);
		}
		break;

	case 13:
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "乱れ雪月花";
		if (desc) return "攻撃回数が増え、冷気耐性のないモンスターに大ダメージを与える。剣か刀を装備していないと使えない。";
#else
		if (name) return "Midare-Setsugekka";
		if (desc) return "Attacks a monster with increased number of attacks and more damage unless it has resistance to cold. Requires wielding a sword or a katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_COLD);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 14:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "急所突き";
		if (desc) return "モンスターを一撃で倒す攻撃を繰り出す。失敗すると1点しかダメージを与えられない。武器を装備していないと使えない。";
#else
		if (name) return "Spot Aiming";
		if (desc) return "Attempts to kill a monster instantly. If failed cause only 1HP of damage. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_KYUSHO);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_AXE || inventory[INVEN_LARM].tval == TV_AXE) return "";
			else return NULL;
		}
		if (name) return "魔神斬り";
		if (desc) return "攻撃が当たれば必ず会心の一撃になる。斧を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_AXE || inventory[INVEN_LARM].tval == TV_AXE) return "";
			else return NULL;
		}
		if (name) return "Majingiri";
		if (desc) return "Attempts to attack with critical hit. Requires wielding an axe.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_MAJIN);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(check) return "";
		if (name) return "捨て身";
		if (desc) return "強力な攻撃を繰り出す。次のターンまでの間、食らうダメージが増える。";
#else
		if(check) return "";
		if (name) return "Desperate Attack";
		if (desc) return "Attacks with all of your power. But all damages you take will be doubled for one turn.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_SUTEMI);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
			p_ptr->sutemi = TRUE;
		}
		break;

	case 17:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}

		if (name) return "雷撃鷲爪斬";
		if (desc) return "電撃耐性のないモンスターに非常に大きいダメージを与える。剣か刀を装備していないと使えない。";
#else
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}

		if (name) return "Lightning Eagle";
		if (desc) return "Attacks a monster with more damage unless it has resistance to electricity. Requires wielding a sword or katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_ELEC);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(check) return "";
		if (name) return "入身";
		if (desc) return "素早く相手に近寄り攻撃する。";
#else
		if(check) return "";
		if (name) return "Rush Attack";
		if (desc) return "Steps close to a monster and attacks at a time.";
#endif

		if (cast)
		{
			if (!rush_attack(NULL,0,0)) return NULL;
		}
		break;

	case 19:
#ifdef JP
		if(check) return "";
		if (name) return "赤流渦";
		if (desc) return "自分自身も傷を作りつつ、その傷が深いほど大きい威力で全方向の敵を攻撃できる。生きていないモンスターには効果がない。";
#else
		if(check) return "";
		if (name) return "Bloody Maelstrom";
		if (desc) return "Attacks all adjacent monsters with power corresponding to your cut status. Then increases your cut status. No effect to unliving monsters.";
#endif

		if (cast)
		{
			int y = 0, x = 0;

			cave_type       *c_ptr;
			monster_type    *m_ptr;

			if (p_ptr->cut < 300)
				set_cut(p_ptr->cut + 300);
			else
				set_cut(p_ptr->cut * 2);

			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				{
					if (!monster_living(&r_info[m_ptr->r_idx]))
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, 0);
#ifdef JP
						msg_format("%sには効果がない！", m_name);
#else
						msg_format("%s is unharmed!", m_name);
#endif
					}
					else py_attack(y, x, HISSATSU_SEKIRYUKA);
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].weight >= 100 || inventory[INVEN_LARM].weight >= 100) return "";
			else return NULL;
		}

		if (name) return "激震撃";
		if (desc) return "地震を起こす。5kg以上の武器を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].weight >= 100 || inventory[INVEN_LARM].weight >= 100) return "";
			else return NULL;
		}

		if (name) return "Earthquake Blow";
		if (desc) return "Causes an earthquake. Requires wield a 5 kg or heavier weapon.";
#endif

		if (cast)
		{
			int y,x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_QUAKE);
			else
				earthquake(py, px, 10);
		}
		break;

	case 21:
#ifdef JP
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
		if (name) return "地走り";
		if (desc) return "衝撃波のビームを放つ。武器を装備していないと使えない。";
#else
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
		if (name) return "Crack";
		if (desc) return "Fires a beam of shock wave. Requires wielding a weapon.";
#endif

		if (cast)
		{
			int total_damage = 0, basedam, i;
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
			msg_print("武器を大きく振り下ろした。");
#else
			msg_print("You swing your weapon downward.");
#endif
			///item sys 剣術の地走り　ダメージ計算にヴォーパル処理使用
			for (i = 0; i < 2; i++)
			{
				int damage;

				if (!buki_motteruka(INVEN_RARM+i)) break;
				o_ptr = &inventory[INVEN_RARM+i];
				basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
				damage = o_ptr->to_d * 100;
				object_flags(o_ptr, flgs);
			//	if ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD))
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				damage *= p_ptr->num_blow[i];
				total_damage += damage / 200;
				if (i) total_damage = total_damage*7/10;
			}
			fire_beam(GF_FORCE, dir, total_damage);
		}
		break;

	case 22:
#ifdef JP
		if(check) return "";
		if (name) return "気迫の雄叫び";
		if (desc) return "視界内の全モンスターに対して轟音の攻撃を行う。ただし近くにいるモンスターが起きる。";
#else
		if(check) return "";
		if (name) return "War Cry";
		if (desc) return "Damages all monsters in sight with sound. Aggravate nearby monsters.";
#endif

		if (cast)
		{
#ifdef JP
			msg_print("雄叫びをあげた！");
#else
			msg_print("You roar out!");
#endif
			project_hack(GF_SOUND, randint1(plev * 3));
			aggravate_monsters(0,FALSE);
		}
		break;

	case 23:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_SPEAR || inventory[INVEN_LARM].tval == TV_SPEAR) return "";
			else if(inventory[INVEN_RARM].tval == TV_POLEARM || inventory[INVEN_LARM].tval == TV_POLEARM) return "";
			else return NULL;
		}

		if (name) return "無双三段";
		if (desc) return "強力な3段攻撃を繰り出す。槍か長柄武器を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_SPEAR || inventory[INVEN_LARM].tval == TV_SPEAR) return "";
			else if(inventory[INVEN_RARM].tval == TV_POLEARM || inventory[INVEN_LARM].tval == TV_POLEARM) return "";
			else return NULL;
		}

		if (name) return "Musou-Sandan";
		if (desc) return "Attacks with powerful 3 strikes. Requires wielding a spear or a polearm.";
#endif

		if (cast)
		{
			int i;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			for (i = 0; i < 3; i++)
			{
				int y, x;
				int ny, nx;
				int m_idx;
				cave_type *c_ptr;
				monster_type *m_ptr;

				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (c_ptr->m_idx)
					py_attack(y, x, HISSATSU_3DAN);
				else
				{
#ifdef JP
					msg_print("その方向にはモンスターはいません。");
#else
					msg_print("There is no monster.");
#endif
					return NULL;
				}

				if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
				{
					return "";
				}

				/* Monster is dead? */
				if (!c_ptr->m_idx) break;

				ny = y + ddy[dir];
				nx = x + ddx[dir];
				m_idx = c_ptr->m_idx;
				m_ptr = &m_list[m_idx];

				/* Monster cannot move back? */
				if (!monster_can_enter(ny, nx, &r_info[m_ptr->r_idx], 0))
				{
					/* -more- */
					if (i < 2) msg_print(NULL);
					continue;
				}

				c_ptr->m_idx = 0;
				cave[ny][nx].m_idx = m_idx;
				m_ptr->fy = ny;
				m_ptr->fx = nx;

				update_mon(m_idx, TRUE);

				/* Redraw the old spot */
				lite_spot(y, x);

				/* Redraw the new spot */
				lite_spot(ny, nx);

				/* Player can move forward? */
				if (player_can_enter(c_ptr->feat, 0))
				{
					/* Move the player */
					if (!move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP)) break;
				}
				else
				{
					break;
				}

				/* -more- */
				if (i < 2) msg_print(NULL);
			}
		}
		break;

	case 24:
#ifdef JP
		if(check) return "";
		if (name) return "吸血鬼の牙";
		if (desc) return "攻撃した相手の体力を吸いとり、自分の体力を回復させる。生命を持たないモンスターには通じない。";
#else
		if(check) return "";
		if (name) return "Vampire's Fang";
		if (desc) return "Attacks with vampiric strikes which absorbs HP from a monster and gives them to you. No effect to unliving monsters.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_DRAIN);
			else
			{
#ifdef JP
					msg_print("その方向にはモンスターはいません。");
#else
					msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if(check) return "";
		if (name) return "幻惑";
		if (desc) return "視界内の起きている全モンスターに朦朧、混乱、眠りを与えようとする。";
#else
		if(check) return "";
		if (name) return "Moon Dazzling";
		if (desc) return "Attempts to stun, confuse and sleep all waking monsters.";
#endif

		if (cast)
		{
#ifdef JP

			if(have_weapon) msg_print("武器を不規則に揺らした．．．");
			else msg_print("不思議な踊りを踊った。");
#else
			msg_print("You irregularly wave your weapon...");
#endif
			project_hack(GF_ENGETSU, plev * 4);
			project_hack(GF_ENGETSU, plev * 4);
			project_hack(GF_ENGETSU, plev * 4);
		}
		break;

	case 26:
#ifdef JP
		if(check) return "";
		if (name) return "百人斬り";
		if (desc) return "連続して入身でモンスターを攻撃する。攻撃するたびにMPを消費。MPがなくなるか、モンスターを倒せなかったら百人斬りは終了する。";
#else
		if(check) return "";
		if (name) return "Hundred Slaughter";
		if (desc) return "Performs a series of rush attacks. The series continues while killing each monster in a time and SP remains.";
#endif

		if (cast)
		{
			const int mana_cost_per_monster = 8;
			bool flag_new = TRUE;
			bool mdeath;

			do
			{
				if (!rush_attack(&mdeath,0,0)) break;
				if (flag_new)
				{
					/* Reserve needed mana point */
				//	p_ptr->csp -= technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
					p_ptr->csp -= hissatsu_info[26].smana;
					flag_new = FALSE;
				}
				else
					p_ptr->csp -= mana_cost_per_monster;

				if (!mdeath) break;
				command_dir = 0;

				p_ptr->redraw |= PR_MANA;
				handle_stuff();
			}
			while (p_ptr->csp > mana_cost_per_monster);

			if (flag_new) return NULL;

			/* Restore reserved mana */
			p_ptr->csp += hissatsu_info[26].smana;
		}
		break;

	case 27:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;

		}
		if (name) return "天翔龍閃";
		if (desc) return "視界内の場所を指定して、その場所と自分の間にいる全モンスターを攻撃し、その場所に移動する。刀を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;

		}
		if (name) return "Dragonic Flash";
		if (desc) return "Runs toward given location while attacking all monsters on the path. Requires wielding a katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!tgt_pt(&x, &y)) return NULL;

			if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT / 2) ||
			    !projectable(py, px, y, x))
			{
#ifdef JP
				msg_print("失敗！");
#else
				msg_print("You cannot move to that place!");
#endif
				break;
			}
			if (p_ptr->anti_tele)
			{
#ifdef JP
				msg_print("不思議な力がテレポートを防いだ！");
#else
				msg_print("A mysterious force prevents you from teleporting!");
#endif

				break;
			}
			project(0, 0, y, x, HISSATSU_ISSEN, GF_ATTACK, PROJECT_BEAM | PROJECT_KILL, -1);
			teleport_player_to(y, x, 0L);
		}
		break;

	case 28:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "二重の剣撃";
		if (desc) return "1ターンで2度攻撃を行う。剣か刀を装備していないと使えない。";
#else
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "Twin Slash";
		if (desc) return "double attacks at a time. Requires wield a sword or a katana.";
#endif

		if (cast)
		{
			int x, y;

			if (!get_rep_dir(&dir, FALSE)) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
			{
				py_attack(y, x, HISSATSU_DOUBLE);
				if (cave[y][x].m_idx)
				{
					handle_stuff();
					py_attack(y, x, HISSATSU_DOUBLE);
				}
			}
			else
			{
#ifdef JP
	msg_print("その方向にはモンスターはいません。");
#else
				msg_print("You don't see any monster in this direction");
#endif
				return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}

		if (name) return "虎伏絶刀勢";
		if (desc) return "強力な攻撃を行い、近くの場所にも効果が及ぶ。刀を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}

		if (name) return "Kofuku-Zettousei";
		if (desc) return "Performs a powerful attack which even effect nearby monsters. Requires wielding a katana.";
#endif

		if (cast)
		{
			int total_damage = 0, basedam, i;
			int y, x;
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("なぜか攻撃することができない。");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return "";
			}
#ifdef JP
			msg_print("武器を大きく振り下ろした。");
#else
			msg_print("You swing your weapon downward.");
#endif
			for (i = 0; i < 2; i++)
			{
				///item sys 剣術　ダメージ計算にヴォーパル処理使用
				int damage;
				if (!buki_motteruka(INVEN_RARM+i)) break;
				if(inventory[INVEN_RARM+i].tval != TV_KATANA) continue;

				o_ptr = &inventory[INVEN_RARM+i];
				basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
				damage = o_ptr->to_d * 100;
				object_flags(o_ptr, flgs);
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				damage += p_ptr->to_d[i] * 100;
				damage *= p_ptr->num_blow[i];
				total_damage += (damage / 100);
			}
			project(0, (cave_have_flag_bold(y, x, FF_PROJECT) ? 5 : 0), y, x, total_damage * 2, GF_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
		}
		break;

	case 30:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "慶雲鬼忍剣";
		if (desc) return "自分もダメージをくらうが、相手に非常に大きなダメージを与える。アンデッドには特に効果がある。刀を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "Keiun-Kininken";
		if (desc) return "Attacks a monster with extremely powerful damage. But you also takes some damages. Hurts a undead monster greatly. Requires wielding a katana.";
#endif

		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_UNDEAD);
			else
			{
#ifdef JP
				msg_print("その方向にはモンスターはいません。");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "慶雲鬼忍剣を使った衝撃", -1);
#else
			take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "exhaustion on using Keiun-Kininken", -1);
#endif
		}
		break;

	case 31:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "時を斬る";
		if (desc) return "「時を斬れる様になるには二百年は掛かると言う。」　　　　　　　　　　　　　　　　　　　　　　　　　　(東方萃夢想)　　　　視界内の全てに対し因果混乱属性の攻撃を仕掛ける。この攻撃では行動時間をほとんど消費しない。刀を装備していないと使えない。";
#else
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "Cut Through Time";
		if (desc) return "'It'll take two hundred years to be able to cut through time.'";
#endif

		if (cast)
		{
			int dam = 0;
			if(inventory[INVEN_RARM].tval == TV_KATANA ) dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA ) dam += calc_weapon_dam(1);
			msg_print(_("あなたの剣は因果を切り裂いた！", "Your sword cuts through causality!"));
			project_hack2(GF_NEXUS,0,0,dam);

		}
		break;
	}

	return "";
}


/* Hex */
static bool item_tester_hook_weapon_except_bow(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
	case TV_OTHERWEAPON:

		{
			return (TRUE);
		}
	}

	return (FALSE);
}

///sysdel realm 呪術
#if 0
static cptr do_hex_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

	bool add = TRUE;

	int plev = p_ptr->lev;
	int power;

	switch (spell)
	{
	/*** 1st book (0-7) ***/
	case 0:
#ifdef JP
		if (name) return "邪なる祝福";
		if (desc) return "祝福により攻撃精度と防御力が上がる。";
#else
		if (name) return "Evily blessing";
		if (desc) return "Attempts to increase +to_hit of a weapon and AC";
#endif
		if (cast)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("高潔な気分になった！");
#else
				msg_print("You feel righteous!");
#endif
			}
		}
		if (stop)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("高潔な気分が消え失せた。");
#else
				msg_print("The prayer has expired.");
#endif
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "軽傷の治癒";
		if (desc) return "HPや傷を少し回復させる。";
#else
		if (name) return "Cure light wounds";
		if (desc) return "Heals cut and HP a little.";
#endif
		if (info) return info_heal(1, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(1, 10));
			set_cut(p_ptr->cut - 10);
		}
		break;

	case 2:
#ifdef JP
		if (name) return "悪魔のオーラ";
		if (desc) return "炎のオーラを身にまとい、回復速度が速くなる。";
#else
		if (name) return "Demonic aura";
		if (desc) return "Gives fire aura and regeneration.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("体が炎のオーラで覆われた。");
#else
			msg_print("You have enveloped by fiery aura!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("炎のオーラが消え去った。");
#else
			msg_print("Fiery aura disappeared.");
#endif
		}
		break;

	case 3:
#ifdef JP
		if (name) return "悪臭霧";
		if (desc) return "視界内のモンスターに微弱量の毒のダメージを与える。";
#else
		if (name) return "Stinking mist";
		if (desc) return "Deals few damages of poison to all monsters in your sight.";
#endif
		power = plev / 2 + 5;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_POIS, randint1(power));
		}
		break;

	case 4:
#ifdef JP
		if (name) return "腕力強化";
		if (desc) return "術者の腕力を上昇させる。";
#else
		if (name) return "Extra might";
		if (desc) return "Attempts to increase your strength.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("何だか力が湧いて来る。");
#else
			msg_print("You feel you get stronger.");
#endif
		}
		break;

	case 5:
#ifdef JP
		if (name) return "武器呪縛";
		if (desc) return "装備している武器を呪う。";
#else
		if (name) return "Curse weapon";
		if (desc) return "Curses your weapon.";
#endif
		if (cast)
		{
			int item;
			char *q, *s;
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			u32b f[TR_FLAG_SIZE];

			item_tester_hook = item_tester_hook_weapon_except_bow;
#ifdef JP
			q = "どれを呪いますか？";
			s = "武器を装備していない。";
#else
			q = "Which weapon do you curse?";
			s = "You wield no weapons.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			object_flags(o_ptr, f);

#ifdef JP
			if (!get_check(format("本当に %s を呪いますか？", o_name))) return FALSE;
#else
			if (!get_check(format("Do you curse %s, really？", o_name))) return FALSE;
#endif

			if (!one_in_(3) &&
				(object_is_artifact(o_ptr) || have_flag(f, TR_BLESSED)))
			{
#ifdef JP
				msg_format("%s は呪いを跳ね返した。", o_name);
#else
				msg_format("%s resists the effect.", o_name);
#endif
				if (one_in_(3))
				{
					if (o_ptr->to_d > 0)
					{
						o_ptr->to_d -= randint1(3) % 2;
						if (o_ptr->to_d < 0) o_ptr->to_d = 0;
					}
					if (o_ptr->to_h > 0)
					{
						o_ptr->to_h -= randint1(3) % 2;
						if (o_ptr->to_h < 0) o_ptr->to_h = 0;
					}
					if (o_ptr->to_a > 0)
					{
						o_ptr->to_a -= randint1(3) % 2;
						if (o_ptr->to_a < 0) o_ptr->to_a = 0;
					}
#ifdef JP
					msg_format("%s は劣化してしまった。", o_name);
#else
					msg_format("Your %s was disenchanted!", o_name);
#endif
				}
			}
			else
			{
				int power = 0;
#ifdef JP
				msg_format("恐怖の暗黒オーラがあなたの%sを包み込んだ！", o_name);
#else
				msg_format("A terrible black aura blasts your %s!", o_name);
#endif
				o_ptr->curse_flags |= (TRC_CURSED);

				if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
				{

					if (one_in_(3)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
					if (one_in_(666))
					{
						o_ptr->curse_flags |= (TRC_TY_CURSE);
						if (one_in_(666)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);

						add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						add_flag(o_ptr->art_flags, TR_VORPAL);
						add_flag(o_ptr->art_flags, TR_VAMPIRIC);
#ifdef JP
						msg_print("血だ！血だ！血だ！");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				o_ptr->curse_flags |= get_curse(power, o_ptr);
			}

			p_ptr->update |= (PU_BONUS);
			add = FALSE;
		}
		break;

	case 6:
#ifdef JP
		if (name) return "邪悪感知";
		if (desc) return "周囲の邪悪なモンスターを感知する。";
#else
		if (name) return "Evil detection";
		if (desc) return "Detects evil monsters.";
#endif
		if (info) return info_range(MAX_SIGHT);
		if (cast)
		{
#ifdef JP
			msg_print("邪悪な生物の存在を感じ取ろうとした。");
#else
			msg_print("You attend to the presence of evil creatures.");
#endif
		}
		break;

	case 7:
#ifdef JP
		if (name) return "我慢";
		if (desc) return "数ターン攻撃を耐えた後、受けたダメージを地獄の業火として周囲に放出する。";
#else
		if (name) return "Patience";
		if (desc) return "Bursts hell fire strongly after patients any damage while few turns.";
#endif
		power = MIN(200, (p_ptr->magic_num1[2] * 2));
		if (info) return info_damage(0, 0, power);
		if (cast)
		{
			int a = 3 - (p_ptr->pspeed - 100) / 10;
			int r = 3 + randint1(3) + MAX(0, MIN(3, a));

			if (p_ptr->magic_num2[2] > 0)
			{
#ifdef JP
				msg_print("すでに我慢をしている。");
#else
				msg_print("You are already patienting.");
#endif
				return NULL;
			}

			p_ptr->magic_num2[1] = 1;
			p_ptr->magic_num2[2] = r;
			p_ptr->magic_num1[2] = 0;
#ifdef JP
			msg_print("じっと耐えることにした。");
#else
			msg_print("You decide to patient all damages.");
#endif
			add = FALSE;
		}
		if (cont)
		{
			int rad = 2 + (power / 50);

			p_ptr->magic_num2[2]--;

			if ((p_ptr->magic_num2[2] <= 0) || (power >= 200))
			{
#ifdef JP
				msg_print("我慢が解かれた！");
#else
				msg_print("Time for end of patioence!");
#endif
				if (power)
				{
					project(0, rad, py, px, power, GF_HELL_FIRE,
						(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				}
				if (p_ptr->wizard)
				{
#ifdef JP
					msg_format("%d点のダメージを返した。", power);
#else
					msg_format("You return %d damages.", power);
#endif
				}

				/* Reset */
				p_ptr->magic_num2[1] = 0;
				p_ptr->magic_num2[2] = 0;
				p_ptr->magic_num1[2] = 0;
			}
		}
		break;

	/*** 2nd book (8-15) ***/
	case 8:
#ifdef JP
		if (name) return "氷の鎧";
		if (desc) return "氷のオーラを身にまとい、防御力が上昇する。";
#else
		if (name) return "Ice armor";
		if (desc) return "Gives fire aura and bonus to AC.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("体が氷の鎧で覆われた。");
#else
			msg_print("You have enveloped by ice armor!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("氷の鎧が消え去った。");
#else
			msg_print("Ice armor disappeared.");
#endif
		}
		break;

	case 9:
#ifdef JP
		if (name) return "重傷の治癒";
		if (desc) return "体力や傷を多少回復させる。";
#else
		if (name) return "Cure serious wounds";
		if (desc) return "Heals cut and HP more.";
#endif
		if (info) return info_heal(2, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(2, 10));
			set_cut((p_ptr->cut / 2) - 10);
		}
		break;

	case 10:
#ifdef JP
		if (name) return "薬品吸入";
		if (desc) return "呪文詠唱を中止することなく、薬の効果を得ることができる。";
#else
		if (name) return "Inhail potion";
		if (desc) return "Quaffs a potion without canceling of casting a spell.";
#endif
		if (cast)
		{
			p_ptr->magic_num1[0] |= (1L << HEX_INHAIL);
			do_cmd_quaff_potion();
			p_ptr->magic_num1[0] &= ~(1L << HEX_INHAIL);
			add = FALSE;
		}
		break;

	case 11:
#ifdef JP
		if (name) return "吸血霧";
		if (desc) return "視界内のモンスターに微弱量の生命力吸収のダメージを与える。与えたダメージの分、体力が回復する。";
#else
		if (name) return "Vampiric mist";
		if (desc) return "Deals few dameges of drain life to all monsters in your sight.";
#endif
		power = (plev / 2) + 5;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_OLD_DRAIN, randint1(power));
		}
		break;

	case 12:
#ifdef JP
		if (name) return "魔剣化";
		if (desc) return "武器の攻撃力を上げる。切れ味を得、呪いに応じて与えるダメージが上昇し、善良なモンスターに対するダメージが2倍になる。";
#else
		if (name) return "Swords to runeswords";
		if (desc) return "Gives vorpal ability to your weapon. Increases damages by your weapon acccording to curse of your weapon.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("あなたの武器が黒く輝いた。");
#else
			if (!empty_hands(FALSE))
				msg_print("Your weapons glow bright black.");
			else
				msg_print("Your weapon glows bright black.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("武器の輝きが消え去った。");
#else
			msg_format("Brightness of weapon%s disappeared.", (empty_hands(FALSE)) ? "" : "s");
#endif
		}
		break;

	case 13:
#ifdef JP
		if (name) return "混乱の手";
		if (desc) return "攻撃した際モンスターを混乱させる。";
#else
		if (name) return "Touch of confusion";
		if (desc) return "Confuses a monster when you attack.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("あなたの手が赤く輝き始めた。");
#else
			msg_print("Your hands glow bright red.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("手の輝きがなくなった。");
#else
			msg_print("Brightness on your hands disappeard.");
#endif
		}
		break;

	case 14:
#ifdef JP
		if (name) return "肉体強化";
		if (desc) return "術者の腕力、器用さ、耐久力を上昇させる。攻撃回数の上限を 1 増加させる。";
#else
		if (name) return "Building up";
		if (desc) return "Attempts to increases your strength, dexterity and constitusion.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("身体が強くなった気がした。");
#else
			msg_print("You feel your body is developed more now.");
#endif
		}
		break;

	case 15:
#ifdef JP
		if (name) return "反テレポート結界";
		if (desc) return "視界内のモンスターのテレポートを阻害するバリアを張る。";
#else
		if (name) return "Anti teleport barrier";
		if (desc) return "Obstructs all teleportations by monsters in your sight.";
#endif
		power = plev * 3 / 2;
		if (info) return info_power(power);
		if (cast)
		{
#ifdef JP
			msg_print("テレポートを防ぐ呪いをかけた。");
#else
			msg_print("You feel anyone can not teleport except you.");
#endif
		}
		break;

	/*** 3rd book (16-23) ***/
	case 16:
#ifdef JP
		if (name) return "衝撃のクローク";
		if (desc) return "電気のオーラを身にまとい、動きが速くなる。";
#else
		if (name) return "Cloak of shock";
		if (desc) return "Gives lightning aura and a bonus to speed.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("体が稲妻のオーラで覆われた。");
#else
			msg_print("You have enveloped by electrical aura!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("稲妻のオーラが消え去った。");
#else
			msg_print("Electrical aura disappeared.");
#endif
		}
		break;

	case 17:
#ifdef JP
		if (name) return "致命傷の治癒";
		if (desc) return "体力や傷を回復させる。";
#else
		if (name) return "Cure critical wounds";
		if (desc) return "Heals cut and HP greatry.";
#endif
		if (info) return info_heal(4, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("気分が良くなってくる。");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(4, 10));
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
		}
		break;

	case 18:
#ifdef JP
		if (name) return "呪力封入";
		if (desc) return "魔法の道具に魔力を再充填する。";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges a magic device.";
#endif
		power = plev * 2;
		if (info) return info_power(power);
		if (cast)
		{
			if (!recharge(power)) return NULL;
			add = FALSE;
		}
		break;

	case 19:
#ifdef JP
		if (name) return "死者復活";
		if (desc) return "死体を蘇らせてペットにする。";
#else
		if (name) return "Animate Dead";
		if (desc) return "Raises corpses and skeletons from dead.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("死者への呼びかけを始めた。");
#else
			msg_print("You start to call deads.!");
#endif
		}
		if (cast || cont)
		{
			animate_dead(0, py, px);
		}
		break;

	case 20:
#ifdef JP
		if (name) return "防具呪縛";
		if (desc) return "装備している防具に呪いをかける。";
#else
		if (name) return "Curse armor";
		if (desc) return "Curse a piece of armour that you wielding.";
#endif
		if (cast)
		{
			int item;
			char *q, *s;
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			u32b f[TR_FLAG_SIZE];

			item_tester_hook = object_is_armour;
#ifdef JP
			q = "どれを呪いますか？";
			s = "防具を装備していない。";
#else
			q = "Which piece of armour do you curse?";
			s = "You wield no piece of armours.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			object_flags(o_ptr, f);

#ifdef JP
			if (!get_check(format("本当に %s を呪いますか？", o_name))) return FALSE;
#else
			if (!get_check(format("Do you curse %s, really？", o_name))) return FALSE;
#endif

			if (!one_in_(3) &&
				(object_is_artifact(o_ptr) || have_flag(f, TR_BLESSED)))
			{
#ifdef JP
				msg_format("%s は呪いを跳ね返した。", o_name);
#else
				msg_format("%s resists the effect.", o_name);
#endif
				if (one_in_(3))
				{
					if (o_ptr->to_d > 0)
					{
						o_ptr->to_d -= randint1(3) % 2;
						if (o_ptr->to_d < 0) o_ptr->to_d = 0;
					}
					if (o_ptr->to_h > 0)
					{
						o_ptr->to_h -= randint1(3) % 2;
						if (o_ptr->to_h < 0) o_ptr->to_h = 0;
					}
					if (o_ptr->to_a > 0)
					{
						o_ptr->to_a -= randint1(3) % 2;
						if (o_ptr->to_a < 0) o_ptr->to_a = 0;
					}
#ifdef JP
					msg_format("%s は劣化してしまった。", o_name);
#else
					msg_format("Your %s was disenchanted!", o_name);
#endif
				}
			}
			else
			{
				int power = 0;
#ifdef JP
				msg_format("恐怖の暗黒オーラがあなたの%sを包み込んだ！", o_name);
#else
				msg_format("A terrible black aura blasts your %s!", o_name);
#endif
				o_ptr->curse_flags |= (TRC_CURSED);

				if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
				{

					if (one_in_(3)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
					if (one_in_(666))
					{
						o_ptr->curse_flags |= (TRC_TY_CURSE);
						if (one_in_(666)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);

						add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						add_flag(o_ptr->art_flags, TR_RES_POIS);
						add_flag(o_ptr->art_flags, TR_RES_DARK);
						add_flag(o_ptr->art_flags, TR_RES_NETHER);
#ifdef JP
						msg_print("血だ！血だ！血だ！");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				o_ptr->curse_flags |= get_curse(power, o_ptr);
			}

			p_ptr->update |= (PU_BONUS);
			add = FALSE;
		}
		break;

	case 21:
#ifdef JP
		if (name) return "影のクローク";
		if (desc) return "影のオーラを身にまとい、敵に影のダメージを与える。";
#else
		if (name) return "Cloak of shadow";
		if (desc) return "Gives aura of shadow.";
#endif
		if (cast)
		{
			object_type *o_ptr = &inventory[INVEN_OUTER];

			if (!o_ptr->k_idx)
			{
#ifdef JP
				msg_print("クロークを身につけていない！");
#else
				msg_print("You don't ware any cloak.");
#endif
				return NULL;
			}
			else if (!object_is_cursed(o_ptr))
			{
#ifdef JP
				msg_print("クロークは呪われていない！");
#else
				msg_print("Your cloak is not cursed.");
#endif
				return NULL;
			}
			else
			{
#ifdef JP
				msg_print("影のオーラを身にまとった。");
#else
				msg_print("You have enveloped by shadow aura!");
#endif
			}
		}
		if (cont)
		{
			object_type *o_ptr = &inventory[INVEN_OUTER];

			if ((!o_ptr->k_idx) || (!object_is_cursed(o_ptr)))
			{
				do_spell(REALM_HEX, spell, SPELL_STOP);
				p_ptr->magic_num1[0] &= ~(1L << spell);
				p_ptr->magic_num2[0]--;
				if (!p_ptr->magic_num2[0]) set_action(ACTION_NONE);
			}
		}
		if (stop)
		{
#ifdef JP
			msg_print("影のオーラが消え去った。");
#else
			msg_print("Shadow aura disappeared.");
#endif
		}
		break;

	case 22:
#ifdef JP
		if (name) return "苦痛を魔力に";
		if (desc) return "視界内のモンスターに精神ダメージ与え、魔力を吸い取る。";
#else
		if (name) return "Pains to mana";
		if (desc) return "Deals psychic damages to all monsters in sight, and drains some mana.";
#endif
		power = plev * 3 / 2;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_PSI_DRAIN, randint1(power));
		}
		break;

	case 23:
#ifdef JP
		if (name) return "目には目を";
		if (desc) return "打撃や魔法で受けたダメージを、攻撃元のモンスターにも与える。";
#else
		if (name) return "Eye for an eye";
		if (desc) return "Returns same damage which you got to the monster which damaged you.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("復讐したい欲望にかられた。");
#else
			msg_print("You wish strongly you want to revenge anything.");
#endif
		}
		break;

	/*** 4th book (24-31) ***/
	case 24:
#ifdef JP
		if (name) return "反増殖結界";
		if (desc) return "その階の増殖するモンスターの増殖を阻止する。";
#else
		if (name) return "Anti multiply barrier";
		if (desc) return "Obstructs all multiplying by monsters in entire floor.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("増殖を阻止する呪いをかけた。");
#else
			msg_print("You feel anyone can not already multiply.");
#endif
		}
		break;

	case 25:
#ifdef JP
		if (name) return "生命力復活";
		if (desc) return "経験値を徐々に復活し、減少した能力値を回復させる。";
#else
		if (name) return "Restore life";
		if (desc) return "Restores life energy and status.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("生命力が戻り始めた。");
#else
			msg_print("You feel your life energy starting to return.");
#endif
		}
		if (cast || cont)
		{
			bool flag = FALSE;
			int d = (p_ptr->max_exp - p_ptr->exp);
			int r = (p_ptr->exp / 20);
			int i;

			if (d > 0)
			{
				if (d < r)
					p_ptr->exp = p_ptr->max_exp;
				else
					p_ptr->exp += r;

				/* Check the experience */
				check_experience();

				flag = TRUE;
			}
///mod140104 パラメータ上限と表記を変更 呪術はどうせ消す予定なのでパラメータ復活部分を丸ごと削除した
#if 0
			for (i = A_STR; i < 6; i ++)
			{
				if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
				{
					if (p_ptr->stat_cur[i] < 18)
						p_ptr->stat_cur[i]++;
					else
						p_ptr->stat_cur[i] += 10;

					if (p_ptr->stat_cur[i] > p_ptr->stat_max[i])
						p_ptr->stat_cur[i] = p_ptr->stat_max[i];

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					flag = TRUE;
				}
			}
#endif
			if (!flag)
			{
#ifdef JP
				msg_format("%sの呪文の詠唱をやめた。", do_spell(REALM_HEX, HEX_RESTORE, SPELL_NAME));
#else
				msg_format("Finish casting '%^s'.", do_spell(REALM_HEX, HEX_RESTORE, SPELL_NAME));
#endif
				p_ptr->magic_num1[0] &= ~(1L << HEX_RESTORE);
				if (cont) p_ptr->magic_num2[0]--;
				if (p_ptr->magic_num2) p_ptr->action = ACTION_NONE;

				/* Redraw status */
				p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
				p_ptr->redraw |= (PR_EXTRA);

				return "";
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "呪力吸収";
		if (desc) return "呪われた武器の呪いを吸収して魔力を回復する。";
#else
		if (name) return "Drain curse power";
		if (desc) return "Drains curse on your weapon and heals SP a little.";
#endif
		if (cast)
		{
			int item;
			char *s, *q;
			u32b f[TR_FLAG_SIZE];
			object_type *o_ptr;

			item_tester_hook = item_tester_hook_cursed;
#ifdef JP
			q = "どの装備品から吸収しますか？";
			s = "呪われたアイテムを装備していない。";
#else
			q = "Which cursed equipment do you drain mana from?";
			s = "You have no cursed equipment.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_flags(o_ptr, f);

			p_ptr->csp += (p_ptr->lev / 5) + randint1(p_ptr->lev / 5);
			if (have_flag(f, TR_TY_CURSE) || (o_ptr->curse_flags & TRC_TY_CURSE)) p_ptr->csp += randint1(5);
			if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

			if (o_ptr->curse_flags & TRC_PERMA_CURSE)
			{
				/* Nothing */
			}
			else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				if (one_in_(7))
				{
#ifdef JP
					msg_print("呪いを全て吸い取った。");
#else
					msg_print("Heavy curse vanished away.");
#endif
					o_ptr->curse_flags = 0L;
				}
			}
			else if ((o_ptr->curse_flags & (TRC_CURSED)) && one_in_(3))
			{
#ifdef JP
				msg_print("呪いを全て吸い取った。");
#else
				msg_print("Curse vanished away.");
#endif
				o_ptr->curse_flags = 0L;
			}

			add = FALSE;
		}
		break;

	case 27:
#ifdef JP
		if (name) return "吸血の刃";
		if (desc) return "吸血属性で攻撃する。";
#else
		if (name) return "Swords to vampires";
		if (desc) return "Gives vampiric ability to your weapon.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("あなたの武器が血を欲している。");
#else
			if (!empty_hands(FALSE))
				msg_print("Your weapons want more blood now.");
			else
				msg_print("Your weapon wants more blood now.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("武器の渇望が消え去った。");
#else
			msg_format("Thirsty of weapon%s disappeared.", (empty_hands(FALSE)) ? "" : "s");
#endif
		}
		break;

	case 28:
#ifdef JP
		if (name) return "朦朧の言葉";
		if (desc) return "視界内のモンスターを朦朧とさせる。";
#else
		if (name) return "Word of stun";
		if (desc) return "Stuns all monsters in your sight.";
#endif
		power = plev * 4;
		if (info) return info_power(power);
		if (cast || cont)
		{
			stun_monsters(power);
		}
		break;

	case 29:
#ifdef JP
		if (name) return "影移動";
		if (desc) return "モンスターの隣のマスに瞬間移動する。";
#else
		if (name) return "Moving into shadow";
		if (desc) return "Teleports you close to a monster.";
#endif
		if (cast)
		{
			int i, y, x, dir;
			bool flag;

			for (i = 0; i < 3; i++)
			{
				if (!tgt_pt(&x, &y)) return FALSE;

				flag = FALSE;

				for (dir = 0; dir < 8; dir++)
				{
					int dy = y + ddy_ddd[dir];
					int dx = x + ddx_ddd[dir];
					if (dir == 5) continue;
					if(cave[dy][dx].m_idx) flag = TRUE;
				}

				if (!cave_empty_bold(y, x) || (cave[y][x].info & CAVE_ICKY) ||
					(distance(y, x, py, px) > plev + 2))
				{
#ifdef JP
					msg_print("そこには移動できない。");
#else
					msg_print("Can not teleport to there.");
#endif
					continue;
				}
				break;
			}

			if (flag && randint0(plev * plev / 2))
			{
				teleport_player_to(y, x, 0L);
			}
			else
			{
#ifdef JP
				msg_print("おっと！");
#else
				msg_print("Oops!");
#endif
				teleport_player(30, 0L);
			}

			add = FALSE;
		}
		break;

	case 30:
#ifdef JP
		if (name) return "反魔法結界";
		if (desc) return "視界内のモンスターの魔法を阻害するバリアを張る。";
#else
		if (name) return "Anti magic barrier";
		if (desc) return "Obstructs all magic spell of monsters in your sight.";
#endif
		power = plev * 3 / 2;
		if (info) return info_power(power);
		if (cast)
		{
#ifdef JP
			msg_print("魔法を防ぐ呪いをかけた。");
#else
			msg_print("You feel anyone can not cast spells except you.");
#endif
		}
		break;

	case 31:
#ifdef JP
		if (name) return "復讐の宣告";
		if (desc) return "数ターン後にそれまで受けたダメージに応じた威力の地獄の劫火の弾を放つ。";
#else
		if (name) return "Revenge sentence";
		if (desc) return "Fires  a ball of hell fire to try revenging after few turns.";
#endif
		power = p_ptr->magic_num1[2];
		if (info) return info_damage(0, 0, power);
		if (cast)
		{
			int r;
			int a = 3 - (p_ptr->pspeed - 100) / 10;
			r = 1 + randint1(2) + MAX(0, MIN(3, a));

			if (p_ptr->magic_num2[2] > 0)
			{
#ifdef JP
				msg_print("すでに復讐は宣告済みだ。");
#else
				msg_print("You already pronounced your revenge.");
#endif
				return NULL;
			}

			p_ptr->magic_num2[1] = 2;
			p_ptr->magic_num2[2] = r;
#ifdef JP
			msg_format("あなたは復讐を宣告した。あと %d ターン。", r);
#else
			msg_format("You pronounce your revenge. %d turns left.", r);
#endif
			add = FALSE;
		}
		if (cont)
		{
			p_ptr->magic_num2[2]--;

			if (p_ptr->magic_num2[2] <= 0)
			{
				int dir;

				if (power)
				{
					command_dir = 0;

					do
					{
#ifdef JP
						msg_print("復讐の時だ！");
#else
						msg_print("Time to revenge!");
#endif
					}
					while (!get_aim_dir(&dir));

					fire_ball(GF_HELL_FIRE, dir, power, 1);

					if (p_ptr->wizard)
					{
#ifdef JP
						msg_format("%d点のダメージを返した。", power);
#else
						msg_format("You return %d damages.", power);
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("復讐する気が失せた。");
#else
					msg_print("You are not a mood to revenge.");
#endif
				}
				p_ptr->magic_num1[2] = 0;
			}
		}
		break;
	}

	/* start casting */
	if ((cast) && (add))
	{
		/* add spell */
		p_ptr->magic_num1[0] |= 1L << (spell);
		p_ptr->magic_num2[0]++;

		if (p_ptr->action != ACTION_SPELL) set_action(ACTION_SPELL);
	}

	/* Redraw status */
	if (!info)
	{
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
		p_ptr->redraw |= (PR_EXTRA | PR_HP | PR_MANA);
	}

	return "";
}
#endif


/*:::「ゲルセミウム・エレガンス」を選択するためのhook*/
static bool item_tester_hook_gelsemium(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_GELSEMIUM) return (FALSE);
	return (TRUE);
}
/*:::霊酒の材料を選択するためのhook 塩水以外のSOFTDRINK*/
static bool item_tester_hook_make_reisyu(object_type *o_ptr)
{
	if (o_ptr->tval != TV_SOFTDRINK) return (FALSE);
	if (o_ptr->sval == SV_DRINK_SALT_WATER) return (FALSE);
	return (TRUE);
}

/*:::「水銀鉱石」を選択するためのhook*/
static bool item_tester_hook_mercury(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_MERCURY) return (FALSE);
	return (TRUE);
}


static cptr do_new_spell_element(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
    static const char s_dam[] = "dam: ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
		if (name) return _("マジック・ミサイル", "Magic Missile");
		if (desc) return _("弱い魔法の矢を放つ。", "Fires a weak bolt of magic.");

		{
			int dice = 3 + ((plev - 1) / 6);
			int sides = 3;
			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
		if (name) return _("閃光", "Flash of Light");
		if (desc) return _("光源が照らしている範囲か部屋全体を永久に明るくする。",
                            "Lights up nearby area and the inside of a room permanently.");
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if(rad > 8) rad = 8;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 2:
		if (name) return _("サンダー・ボルト", "Thunderbolt");
		if (desc) return _("電撃のボルトもしくはビームを放つ。", "Fires a bolt/beam of lightning.");
		{
			int dice = 6 + (plev - 5) / 4;
			int sides = 5;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;
	case 3:
		if (name) return _("アイス・ボルト", "Ice Bolt");
		if (desc) return _("冷気のボルトもしくはビームを放つ。", "Fires a bolt/beam of cold.");
		{
			int dice = 6 + (plev - 5) / 4;
			int sides = 6;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir, damroll(dice, sides));
			}
		}
		break;

	case 4:
		if (name) return _("アシッド・ボルト", "Acid Bolt");
		if (desc) return _("酸のボルトもしくはビームを放つ。", "Fires a bolt/beam of acid.");
		{
			int dice = 7 + (plev - 5) / 4;
			int sides = 6;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ACID, dir, damroll(dice, sides));
			}
		}
		break;
	case 5:
#ifdef JP
		if (name) return "ファイア・ボルト";
		if (desc) return "炎のボルトもしくはビームを放つ。";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif

		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
		if (name) return _("浮遊", "Levitation");
		if (desc) return _("一定時間浮遊状態になる。", "Grants temporary levitation.");

		{
			int base = plev;
			int sides = 20 + plev;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				msg_print(_("あなたの周りに風が渦巻いた。", "Air starts swirling around you."));
				set_tim_levitation(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 7:
		if (name) return _("温度遮断", "Thermal Insulation");
		if (desc) return _("一定時間、火炎と冷気に対する耐性を得る。装備による耐性に累積する。",
                            "Grants temporary resistance to fire and cold. Stacks with resistance from equipment.");

		{
			int base = 20;
			if (info) return info_duration(base, base);
			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 8:
		if (name) return _("耐電撃", "Resist Electricity");
		if (desc) return _("一定時間、電撃への耐性を得る。装備による耐性に累積する。",
                            "Grants temporary resistance to lightning. Stacks with resistance from equipment.");

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "耐酸";
		if (desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Acid";
		if (desc) return "Grants temporary resistance to acid. Stacks with resistance from equipment.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
		if (name) return _("ウィンドカッター", "Wind Cutter");
		if (desc) return _("風の刃を連続で飛ばして攻撃する。レベルが上がると刃の数が増える。ACの高い敵には効果が薄い。",
                            "Attacks by launching blades of wind. Amount of blades increases with level. Less effective against enemies with high AC.");

		{
			int count = 2 + (plev - 10) / 5;
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int i;
			if(cp_ptr->magicmaster) count+=2;
			if(count<1) break; //paranoia
			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				for (i = 0; i < count; i++)
				{
					fire_bolt(GF_WINDCUTTER, dir, damroll(dice, sides));
				}
			}
		}
		break;

	case 11:
		if (name) return _("トンネル生成", "Create Tunnel");
		if (desc) return _("壁を溶かす短射程のビームを放つ。",
                            "Fires a short stone-melting beam.");

		{
			int dice = 1;
			int sides = 1;
			int range = plev / 10 + 2;


			if (info) return format(_("射程：%d", "rng: %d"), range);

			if (cast)
			{
				int flg = PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI;
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;
				project_hook(GF_KILL_WALL, dir, damroll(dice,sides), flg);
			}
		}
		break;

	case 12:
		if (name) return _("サンダー・ボール", "Thunder Ball");
		if (desc) return _("雷の球を放つ。", "Fires a ball of lightning.");

		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ELEC, dir, dam, rad);
			}
		}
		break;

	case 13:
		if (name) return _("アイス・ボール", "Ice Ball");
		if (desc) return _("冷気の球を放つ。", "Fires a ball of cold.");

		{
			int dam = plev + 75;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_COLD, dir, dam, rad);
			}
		}
		break;
	case 14:
		if (name) return _("アシッド・ボール", "Acid Ball");
		if (desc) return _("酸の球を放つ。", "Fires a ball of acid.");

		{
			int dam = plev + 95;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ACID, dir, dam, rad);
			}
		}
		break;
	case 15:
		if (name) return _("ファイア・ボール", "Fire Ball");
		if (desc) return _("炎の球を放つ。", "Fires a ball of fire.");

		{
			int dam = plev*2 + 100;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;
	case 16:
		if (name) return _("水の鎧", "Armor of Water");
		if (desc) return _("一定時間、AC上昇と水耐性を得る。",
                            "Temporarily increases AC and grants water resistance.");

		{
			int base = 20;
			int sides = 30;
			int tmp;

			if (info) return info_duration(base, base);

			if (cast)
			{
				tmp = randint1(sides);
				msg_print(_("水が渦巻いてあなたを包んだ。", "A whirlpool of water wraps around you."));
				set_shield(tmp + base, FALSE);
				set_tim_res_water(tmp + base, FALSE);
			}
		}
		break;
	case 17:
		if (name) return _("深い穴生成", "Create Chasm");
		if (desc) return _("隣接した地面に大穴を開ける。浮遊していない敵がいると落ちてダメージを受ける。アイテムなどのある地面には使えない。",
                            "Opens up a chasm on adjacent tile. Non-flying enemies will take damage from falling. Can't be used on tiles with items.");

		{
			int dice = 1;
			int sides = plev * 2;
			int base = 50 + plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int y, x;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];

				//何もない床にしか開けられない。でもトラップ床には開けられる
				if(!in_bounds(y,x) || (!cave_clean_bold(y, x) && !is_trap(cave[y][x].feat))|| cave_have_flag_bold(y, x, FF_SHALLOW))
				{
					msg_print(_("そこには穴を開けられない。", "You can't open up a chasm here."));

					return NULL;
				}
				cave_set_feat(y, x, feat_dark_pit);

				//v1.1.24 FLOWアップデート追加
				p_ptr->update |= (PU_FLOW);
				msg_print(_("地面に大穴が開いた！", "A large chasm opens up on the floor!"));
				/*:::罠があれば消す。水と溶岩以外のアイテムを置ける地形なら深い穴にする。そのとき浮いていないモンスターがいたらダメージを与える*/
				if (cave[y][x].m_idx)
				{
					int dam = damroll(dice,sides) + base;
					int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_KILL | PROJECT_HIDE;
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					char m_name[80];

					monster_desc(m_name, m_ptr, 0);

					if(r_info[m_ptr->r_idx].flags7 & RF7_CAN_FLY)
					{
						if (m_ptr->ml) msg_format(_("%sは悠々と穴の上を飛んでいる・・", "%s is flying above the chasm..."),m_name);
					}
					else
					{
						if(r_info[m_ptr->r_idx].flags2 & RF2_GIGANTIC)
						{
							if (m_ptr->ml) msg_format(_("%sの巨体が地響きを立てて穴に落ちた！",
                                                        "%s crashes down with a deafening noise!"),m_name);
							dam *= 2;
						}
						else
						{
							if (m_ptr->ml) msg_format(_("%sが穴に落ちた！", "%s falls into the chasm!"),m_name);
						}
						project(0, 0, y, x, dam, GF_MISSILE, flg, -1);
						if(!(r_info[m_ptr->r_idx].flags3 & RF3_NO_STUN))
						{
							if (m_ptr->ml) msg_format(_("%sはもうろうとした。", "%s is stunned."),m_name);
							(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + dam / 20);
						}
					}
				}
				p_ptr->redraw |= (PR_MAP);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "エレメンタル召喚";
		if (desc) return "1体のエレメンタルを召喚する。";
#else
		if (name) return "Conjure Elemental";
		if (desc) return "Summons an elemental.";
#endif

		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, plev, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("エレメンタルは現れなかった。");
#else
					msg_print("No Elementals arrive.");
#endif
				}
			}
		}
		break;
	case 19:
		if (name) return _("元素耐性", "Resist Elements");
		if (desc) return _("一定時間、酸、電撃、炎、冷気に対する耐性を得る。装備による耐性に累積する。",
                            "Grants temporary resistance to acid, lightning, fire and cold. Stacks with resistance from equipment.");

		{
			int base = plev;
			int dice = 20;

			if (info) return info_duration(base, dice);

			if (cast)
			{
			int time = randint1(dice) + base;
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
			}
		}
		break;

	case 20:
		if (name) return _("氷の槍", "Ice Spear");
		if (desc) return _("強力な冷気のビームを放つ。", "Fires a powerful beam of cold.");

		{
			int dice = plev / 3;
			int sides = 10;
			int base = 100 + plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_beam(GF_COLD, dir, damroll(dice, sides) + base);
			}
		}
		break;
	case 21:
		if (name) return _("酸の雨", "Acid Rain");
		if (desc) return _("視界内全てに対して酸で攻撃する。", "Hits everything in sight with acid.");

		{
			int dam = 50 + plev * 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print(_("部屋中が強酸に覆われた！", "The room is showered with strong acid!"));
				project_hack2(GF_ACID, 0,0,dam);

			}
		}
		break;

	case 22:
		if (name) return _("炎の嵐", "Fire Storm");
		if (desc) return _("巨大な炎の球を放つ。", "Fires a giant ball of fire.");

		{
			int dam = plev*5 + 100;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}

		break;

	case 23:
		if (name) return _("雷の拳", "Fist of Thunder");
		if (desc) return _("隣接した敵一体に触れ、強力な電撃を放つ。",
                            "Touches an adjacent enemy, delivering a powerful electric shock.");

		{
			int dice = 1;
			int sides = plev * 3;
			int base = 150 + plev * 4;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int y, x;
				monster_type *m_ptr;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int dam = damroll(dice,sides) + base;
					int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					msg_format(_("あなたは%sに触れ、強烈な電撃を食らわせた！",
                                "You touch %s, delivering a violent electric shock!"),m_name);
					project(0, 0, y, x, dam, GF_ELEC, flg, -1);
					touch_zap_player(m_ptr);
				}
				else
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
			}
		}
		break;

	case 24:
		if (name) return _("魔力消去", "Dispel Magic");
		if (desc) return _("モンスター一体の加速、減速、無敵化を解除する。",
                            "Dispels haste, slowness and invulnerability from a single monster.");

		{

			if (info) return _("解除：1体", "dispel: 1");

			if (cast)
			{

				int m_idx;

				if (!target_set(TARGET_KILL)) return NULL;
				m_idx = cave[target_row][target_col].m_idx;
				if (!m_idx) break;
				if (!player_has_los_bold(target_row, target_col)) break;
				if (!projectable(py, px, target_row, target_col)) break;
				dispel_monster_status(m_idx);
				break;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "原子分解";
		if (desc) return "巨大な分解の球を放つ。";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif

		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "石壁生成";
		if (desc) return "自分の周囲に花崗岩の壁を作る。";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif

		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;


	case 27:
		if (name) return _("絶対零度", "Absolute Zero");
		if (desc) return _("視界内全てに対して極寒の攻撃を行う。",
                            "Hits everything in sight with extreme cold.");

		{

			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print(_("一瞬、部屋の全ての物質が活動を停止した・・",
                            "For a moment, everything in the room stops moving..."));
				project_hack2(GF_ICE, 0,0,dam);
			}

		}
		break;

	case 28:
		if (name) return _("アトミックレイ", "Atomic Ray");
		if (desc) return _("非常に強力な火炎のビームを放つ。", "Fires an extremely powerful beam of fire.");

		{
			int dam = plev * 20;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print(_("あなたは超高温の熱線を放った！", "You fire an extremely hot heat ray!"));
				fire_spark(GF_FIRE, dir, dam ,2);
			}
		}
		break;

	case 29:
		if (name) return _("エリクサー", "Elixir");
		if (desc) return _("万能の霊薬を精製して服用する。体力とバッドステータスを回復する。触媒として「水銀鉱石」が必要になる。",
                            "You create an elixir and drink it. Heals and cures bad status effects. Requires 'Mercury Ore' as catalyst.");

		{
			int heal = 500 + plev * 10;
			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				cptr q,s;
				int item;

				item_tester_hook = item_tester_hook_mercury;
				q = _("どの触媒を使いますか? ", "Use which catalyst?");
				s = _("あなたは触媒となる水銀を持っていない。", "You don't have mercury to use as a catalyst.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

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

				msg_print(_("あなたは霊薬を精製して飲んだ。", "You create an elixir and drink it."));
				hp_player(heal);
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
				set_asthma(0);

				set_broken(-(BROKEN_MAX));

			}
		}
		break;

	case 30:
		if (name) return _("四元素免疫", "Elemental Immunity");
		if (desc) return _("一定時間、酸、電撃、火炎、冷気の攻撃を全く受け付けなくなる。",
                            "Grants temporary immunity to acid, lightning, fire and cold.");

		{
			int base = plev / 4;
			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;

				set_ele_immune(DEFENSE_4ELEM, time);
			}
		}
		break;

	case 31:
		if (name) return _("エーテルストライク", "Aether Strike");
		if (desc) return _("天上の第五元素による無属性攻撃を行う。",
                            "Performs a non-elemental attack using the ethereal fifth element.");

		{
			int dam = plev * 10;
			int rad = 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISP_ALL, dir, dam, rad);
			}
		}
		break;
	}

	return "";
}



/*:::カオス領域新実装*/
static cptr do_new_spell_chaos(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam: ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;


	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "マジック・ミサイル";
		if (desc) return "弱い魔法の矢を放つ。";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif

		{
			int dice = 3 + ((plev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "トラップ/ドア破壊";
		if (desc) return "隣接する罠と扉を破壊する。";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif

		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "閃光";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Flash of Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "混乱の手";
		if (desc) return "相手を混乱させる攻撃をできるようにする。";
#else
		if (name) return "Touch of Confusion";
		if (desc) return "Attempts to confuse the next monster that you hit.";
#endif

		{
			if (cast)
			{
				if (!(p_ptr->special_attack & ATTACK_CONFUSE))
				{
#ifdef JP
					msg_print("あなたの手は光り始めた。");
#else
					msg_print("Your hands start glowing.");
#endif

					p_ptr->special_attack |= ATTACK_CONFUSE;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "魔力炸裂";
		if (desc) return "魔法の球を放つ。";
#else
		if (name) return "Mana Burst";
		if (desc) return "Fires a ball of magic.";
#endif

		{
			int dice = 3;
			int sides = 5;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class メイジ系は威力が上昇する魔法
			if (cp_ptr->magicmaster)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MISSILE, dir, damroll(dice, sides) + base, rad);

				/*
				 * Shouldn't actually use GF_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "ファイア・ボルト";
		if (desc) return "炎のボルトもしくはビームを放つ。";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif

		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "力の拳";
		if (desc) return "ごく小さな分解の球を放つ。";
#else
		if (name) return "Fist of Force";
		if (desc) return "Fires a tiny ball of disintegration.";
#endif

		{
			int dice = 8 + ((plev - 5) / 4);
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir,
					damroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport Self";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "ワンダー";
		if (desc) return "モンスターにランダムな効果を与える。";
#else
		if (name) return "Wonder";
		if (desc) return "Fires something with random effects.";
#endif

		{
			if (info) return s_random;

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				cast_wonder(dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "カオス・ボルト";
		if (desc) return "カオスのボルトもしくはビームを放つ。";
#else
		if (name) return "Chaos Bolt";
		if (desc) return "Fires a bolt or ball of chaos.";
#endif

		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_CHAOS, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "ソニック・ブーム";
		if (desc) return "自分を中心とした轟音の球を発生させる。";
#else
		if (name) return "Sonic Boom";
		if (desc) return "Generates a ball of sound centered on you.";
#endif

		{
			int dam = 60 + plev;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
#ifdef JP
				msg_print("ドーン！部屋が揺れた！");
#else
				msg_print("BOOM! Shake the room!");
#endif

				project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "破滅の矢";
		if (desc) return "純粋な魔力のビームを放つ。";
#else
		if (name) return "Doom Bolt";
		if (desc) return "Fires a beam of pure mana.";
#endif

		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_MANA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "ファイア・ボール";
		if (desc) return "炎の球を放つ。";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif

		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "テレポート・アウェイ";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Other";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "破壊の言葉";
		if (desc) return "周辺のアイテム、モンスター、地形を破壊する。";
#else
		if (name) return "Word of Destruction";
		if (desc) return "Destroy everything in nearby area.";
#endif

		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "ログルス発動";
		if (desc) return "巨大なカオスの球を放つ。";
#else
		if (name) return "Invoke Logrus";
		if (desc) return "Fires a huge ball of chaos.";
#endif

		{
			int dam = plev * 2 + 99;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

		///mod140211 カオス領域の他者変容　カオス耐性持ちには効かない代わりにパワー三倍にした
	case 16:
#ifdef JP
		if (name) return "他者変容";
		if (desc) return "モンスター1体を変身させる。抵抗されると無効。";
#else
		if (name) return "Polymorph Other";
		if (desc) return "Attempts to polymorph a monster.";
#endif

		{
			int power = plev * 3;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				poly_monster(dir, plev);
			}
		}
		break;

		//v1.1.85 カロジアンの効果を変更...しようかと思ったがやめた
#if 0
	case 17:
		if (name) return "カロジアン";
		if (desc) return "自分を中心に酸属性の巨大なボールを発生させ、さらに地形を酸の沼に変化させる。";

		{

			int dam;

			if (cp_ptr->magicmaster) dam = 225 + plev * 9;
			else dam = 150 + plev * 6;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				msg_print("周囲の物が急速に溶け崩れた！");
				project(0, 5, py, px, dam, GF_ACID, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
				project(0, 4, py, px, 5, GF_MAKE_ACID_PUDDLE, (PROJECT_GRID|PROJECT_HIDE), -1);

			}
		}
		break;
#else
	case 17:
		if (name) return _("カロジアン", "Corrosion");
		if (desc) return _("視界内の全てを急速に劣化させて攻撃する。",
                            "Hits everything in sight with disenchantment.");

		{
			int dam = plev * 2;
			int dice = 1;
			int sides = plev;

			if (info) return info_damage(dice, sides, dam);

			if (cast)
			{
				dam += damroll(dice,sides);
				msg_print(_("部屋中の物が溶け崩れた！",
                            "Everything in the room melts down and breaks!"));
				project_hack2(GF_DISENCHANT, dice,sides,dam);

			}
		}
		break;
#endif

	case 18:
#ifdef JP
		if (name) return "原子分解";
		if (desc) return "巨大な分解の球を放つ。";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif

		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "現実変容";
		if (desc) return "現在の階を再構成する。";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;
	case 20:
#ifdef JP
		if (name) return "プラズマ球";
		if (desc) return "プラズマの球を放つ。";
#else
		if (name) return "Plasma Ball";
		if (desc) return "Fires a ball of plasma.";
#endif

		{
			int dice = 1;
			int sides = plev*3;
			int base = plev*2 + 100;
			int rad = 2 + plev / 40;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_PLASMA, dir, (damroll(dice,sides)+base), rad);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "混沌の刃";
		if (desc) return "装備中の並あるいは上質の武器に対し、永続的に対秩序の特性を付与する。";
#else
		if (name) return "Chaos Branding";
		if (desc) return "Makes current weapon a Chaotic weapon.";
#endif

		{
			if (cast)
			{
				brand_weapon(7);
			}
		}
		break;
	case 22:
#ifdef JP
		if (name) return "マジック・ロケット";
		if (desc) return "ロケットを発射する。";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif

		{
			int dam = 150 + plev * 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("ロケット発射！");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
			}
		}
		break;


	case 23:
		if (name) return _("力場の球", "Gravity Ball");
		if (desc) return _("巨大な重力属性の球を放つ。巨大な敵に効きやすく飛んでいる敵には効果が薄い。",
                            "Fires a huge gravity ball. Effective against gigantic enemies, but not that effective against flying ones.");

		{
			int dice = 1;
			int sides = plev * 4;
			int base = plev*2;
			int rad = 5;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice,sides);
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_GRAVITY, dir, dam, rad);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "自己変容";
		if (desc) return "自分を変身させようとする。";
#else
		if (name) return "Polymorph Self";
		if (desc) return "Polymorphs yourself.";
#endif

		{
			if (cast)
			{
#ifdef JP

				if(cp_ptr->flag_only_unique)
				{
					msg_print("……嫌な予感がする。やっぱりやめておこう。");
					return NULL;
				}
				if (!get_check("変身します。よろしいですか？")) return NULL;
#else
				if (!get_check("You will polymorph yourself. Are you sure? ")) return NULL;
#endif
				set_deity_bias(DBIAS_COSMOS, -5);
				do_poly_self();
			}
		}
		break;
	case 25:
		if (name) return _("スキマ伝送", "Gap Transfer");
		if (desc) return _("アイテムを自宅に転送する。ただし1/13の確率でアイテムがロストする。",
                            "Transports an item to your house, but with a 1/13 chance of item getting lost.");
		{

			if (info) return "";

			if (cast)
			{
				msg_print(_("自宅への境界を繋いだ...", "You link a boundary to your house..."));
				item_recall(3);
			}
		}
		break;
/*
	case 25:
#ifdef JP
		if (name) return "流星群";
		if (desc) return "自分の周辺に隕石を落とす。";
#else
		if (name) return "Meteor Swarm";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif

		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;
*/
	case 26:
		if (name) return _("空間歪曲", "Spatial Distortion");
		if (desc) return _("空間を歪めて視界内全てに攻撃する。時空耐性がない場合自分も少し影響を受ける。",
                            "Hits everything in sight with distortion. You take some damage as well unless you resist time.");

		{
			int dice = 1;
			int sides = plev * 10;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				msg_print(_("周囲の空間が歪んだ！", "Space warps around you!"));
				project_hack2(GF_DISTORTION, dice, sides, 0);
				if(!p_ptr->resist_time)
				{
					take_hit(DAMAGE_NOESCAPE, randint1(100), _("空間歪曲", "spatial distortion"), -1);
					teleport_player(randint1(50), TELEPORT_PASSIVE);
				}
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "ログルスのブレス";
		if (desc) return "非常に強力なカオスの球を放つ。";
#else
		if (name) return "Breathe Logrus";
		if (desc) return "Fires an extremely powerful ball of chaos.";
#endif

		{
			int dam = p_ptr->chp;
			int rad = 2;
			if(dam > 1600) dam = 1600;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;
	case 28:
#ifdef JP
		if (name) return "魔力の嵐";
		if (desc) return "非常に強力で巨大な純粋な魔力の球を放つ。";
#else
		if (name) return "Mana Storm";
		if (desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif

		{
			int dam = 300 + plev * 4;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MANA, dir, dam, rad);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "混沌召来";
		if (desc) return "ランダムな属性の視界内攻撃を発生させる。";
#else
		if (name) return "Call Chaos";
		if (desc) return "Generate random kind of balls or beams.";
#endif

		{
			if (info) return format("%s1d777", s_dam);

			if (cast)
			{

				call_chaos();
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "虚無召来";
		if (desc) return "自分の周囲に向かって、ロケット、純粋な魔力の球、核熱の球を放つ。ただし、壁に隣接して使用すると広範囲を破壊する。";
#else
		if (name) return "Call the Void";
		if (desc) return "Fires rockets, mana balls and nuclear waste balls in all directions each unless you are not adjacent to any walls. Otherwise *destroys* huge area.";
#endif

		{
			if (info) return format("%s3 * 175", s_dam);

			if (cast)
			{
				call_the_();
			}
		}
		break;



	case 31:
		if (name) return _("オープンパンドラ", "Open Pandora");
		if (desc) return _("極めて特殊なことが起こる。いくつかの候補の中から効果を選べるがキャンセルはできない。代償として経験値の10%が恒久的に失われる。妖怪人形が使った場合経験値は減少しないが25%の確率で装備品のどれかが破壊される。",
                            "Causes something very special to happen. You choose one effect out of several, but you cannot cancel. As a compensation, you permanently lose 10% of your total experience. If used by a youkai doll, no experience loss occurs, but there's a 25% chance of destroying a piece of equipment.");

		{

			if (info) return _("不定", "unknown");

			if (cast)
			{
				int i,c,effect_mode;
#ifdef JP
				cptr effect_desc[10] = {"床を全て水にする",
                                    "壁を全て森にする",
                                    "床を全て溶岩にする",
                                    "時を止める(約10行動分)",
                                    "全回復+究極の耐性+加速(100ターン)",
                                    "フロアのモンスターを追い払う",
                                    "一時的な能力大幅増強(100ターン)",
                                    "永久的な能力増強",
                                    "敵を黙らせる",
                                    "視界内の存在に1d7777ダメージ"};
#else
                cptr effect_desc[10] = { "Turn all floor into water",
                                    "Turn all walls into trees",
                                    "Turn all floor into lava",
                                    "Stop time (around 10 moves)",
                                    "Full heal + Ultimate Resistance + Haste (100 turns)",
                                    "Banish monsters from this floor",
                                    "Temporary major boost to all stats (100 turns)",
                                    "Augmentation",
                                    "Silence enemies",
                                    "1d7777 damage to everything in sight" };
#endif

				int effect_num[3];
				for(i=0;i<3;i++) effect_num[i] = -1;
				effect_num[0] = randint0(10);
				while(1)
				{
					effect_num[1] = randint0(10);
					if(effect_num[0] != effect_num[1]) break;
				}
				while(1)
				{
					effect_num[2] = randint0(10);
					if(effect_num[0] != effect_num[2] && effect_num[1] != effect_num[2]) break;
				}
				screen_save();
				msg_print(_("起こしたい効果を記号で選択してください。", "Choose the effect you want to happen."));
				for(i=1;i<7;i++)Term_erase(12, i, 255);
				for(i=0;i<3;i++) put_str(format("%c) %s",('a'+i),effect_desc[effect_num[i]]), 3+i , 40);

				while(1)
				{
					c = inkey();
					if(c >= 'a' && c <= 'c') break;
				}
				screen_load();

				effect_mode = effect_num[c - 'a'];

				switch(effect_mode)
				{
				case 0:
				case 1:
				case 2:
				{
					int y,x,feat;
					feature_type *f_ptr;

					if(effect_mode == 0)msg_print(_("轟音と共にダンジョンが洪水に襲われた！",
                                                    "There is a roaring sound as the dungeon floods with water!)"));
					if(effect_mode == 1)msg_print(_("辺り中から岩が砕けるような音が響いた。",
                                                    "Sounds of cracking rock reverberate around you."));
					if(effect_mode == 2)msg_print(_("地響きと共にダンジョンが溶岩に覆われた！",
                                                    "The floor cracks as the dungeon floods with lava!"));

					for (y = 1; y < cur_hgt - 1; y++)
					{
						for (x = 1; x < cur_wid - 1; x++)
						{
							cave_type *c_ptr = &cave[y][x];
							feat = get_feat_mimic(c_ptr);
							f_ptr = &f_info[feat];

							if (effect_mode == 0 && have_flag(f_ptr->flags, FF_FLOOR))
								cave_set_feat(y, x, feat_deep_water);
							if (effect_mode == 1 && have_flag(f_ptr->flags, FF_HURT_ROCK))
								cave_set_feat(y, x, feat_tree);
							if (effect_mode == 2 && have_flag(f_ptr->flags, FF_FLOOR))
								 cave_set_feat(y, x, feat_deep_lava);
						}
					}

					p_ptr->redraw |= (PR_MAP);
					p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
					handle_stuff();
					break;
				}
				case 3:
					if (world_player)
					{
						msg_print(_("既に時は止まっている。", "Time is already stopped."));
						return NULL;
					}
					world_player = TRUE;
					msg_print(_("時が止まった。", "The time has stopped."));
					//v1.1.58
					flag_update_floor_music = TRUE;
					msg_print(NULL);
					p_ptr->energy_need -= 2000 ;
					p_ptr->redraw |= (PR_MAP);
					p_ptr->update |= (PU_MONSTERS);
					p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
					handle_stuff();
					break;
				case 4:
				{
					int v = 100;
					hp_player(5000);
					set_stun(0);
					set_cut(0);
					set_image(0);
					(void)set_food(PY_FOOD_MAX-1);
					do_res_stat(A_STR);
					do_res_stat(A_INT);
					do_res_stat(A_WIS);
					do_res_stat(A_DEX);
					do_res_stat(A_CON);
					do_res_stat(A_CHR);
					restore_level();
					set_fast(v, FALSE);
					set_oppose_acid(v, FALSE);
					set_oppose_elec(v, FALSE);
					set_oppose_fire(v, FALSE);
					set_oppose_cold(v, FALSE);
					set_oppose_pois(v, FALSE);
					set_ultimate_res(v, FALSE);
					set_broken(-(BROKEN_MAX));//v1.1.69
					break;
				}
				case 5:
				{
					msg_print(_("ひどく孤独になった気がする。", "You feel incredibly lonely."));

					//v1.1.32 別関数に置き換えた
					mass_genocide_3(0,TRUE,TRUE);
#if 0
					for (i = 1; i < m_max; i++)
					{
						monster_type *m_ptr = &m_list[i];
						monster_race *r_ptr = &r_info[m_ptr->r_idx];

						if (!m_ptr->r_idx) continue;
						if(m_ptr->r_idx == MON_MASTER_KAGUYA) continue;
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
#endif
					break;
				}
				case 6:
					msg_print(_("スーパーマンになった気がする！", "You feel like a superhuman!"));
					for(i=0;i<6;i++)set_tim_addstat(i,130,100,FALSE);
					break;
				case 7:
					msg_print(_("体から力があふれ出る！", "Power flows through your body!"));
					for(i=0;i<6;i++)
					{
						do_inc_stat(i);
						do_inc_stat(i);
					}
					break;
				case 8:
					msg_print(_("ダンジョンに異質な魔力が満ちた。", "The dungeon is filled with extraordinary magical power."));
					p_ptr->silent_floor = 1;//フロアから出たとき0に直す。これが1のときmagic_barrierで無条件にTRUEを返す。stupidでない敵は魔法を使わなくなる。

					break;
				case 9:
					msg_print(_("周囲が白い光に満たされた。", "White light fills the area around you."));
					project_hack2(GF_DISP_ALL, 1,7777,0);
					break;


				default:
					msg_print(_("ERROR:MAHMANで定義されていないeffect値が使われた",
                                "ERROR: Undefined effect value used in MAHMAN"));
					return NULL;
				}

				if(p_ptr->prace == RACE_ANDROID )
				{
					int slot = INVEN_RARM + randint0(12);

					if(!one_in_(4) || !(inventory[slot].k_idx) || check_invalidate_inventory(slot))
					{
						msg_print(_("一瞬体が震えたが何も起こらなかった。",
                                    "Your body shudders for a moment, but nothing happens."));

					}
					else
					{
						object_type *o_ptr = &inventory[slot];
						char		o_name[MAX_NLEN];
						object_desc(o_name, o_ptr, OD_OMIT_PREFIX);
						msg_format(_("%sは粉々に砕けて消えた！", "%s shatters into pieces and disappears!"),o_name);
						inven_item_increase(slot, -1);
						inven_item_describe(slot);
						inven_item_optimize(slot);
						calc_android_exp();
					}
				}
				else
				{
					msg_print(_("生命力を大幅に喪失した。", "You lose a lot of life force."));
					/* Lose some experience (permanently) */
					p_ptr->exp -= (p_ptr->exp / 10);
					p_ptr->max_exp -= (p_ptr->max_exp / 10);
					check_experience();
				}
			}
		}
		break;






	}

	return "";
}


/*:::新領域：予見*/
static cptr do_new_spell_foresee(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int rad= DETECT_RAD_DEFAULT;
	int plev = p_ptr->lev;
	int damage;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "鉱物探知";
		if (desc) return "近くにある財宝や鉱物を探知する。";
#else
		if (name) return "Detect Treasure";
		if (desc) return "Detects nearby treasure and minerals.";
#endif

		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "トラップ感知";
		if (desc) return "近くにある罠を感知する。";
#else
		if (name) return "Detect Traps";
		if (desc) return "Detects nearby traps.";
#endif

		{
			if (info) return info_radius(rad);
			if (cast)
			{
				detect_traps(rad, TRUE);

			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全てのモンスターを感知する。";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
				detect_monsters_invis(rad);
			}
		}
		break;
	case 3:
		if (name) return _("恐怖治癒", "Cure Fear");
		if (desc) return _("恐怖状態を治療する。", "Cures fear.");

		{
			if (cast)
			{
				set_afraid(0);
			}
		}
		break;


	case 4:
		if (name) return _("ドア・階段感知", "Detect Door/Stairs");
		if (desc) return _("近くにあるドアと階段を感知する。", "Detects nearby doors and stairs.");

		{

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 5:
		if (name) return _("アイテム感知", "Detect Items");
		if (desc) return _("近くの全てのアイテムを感知する。", "Detect nearby items.");
		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
			}
		}
		break;

	case 6:
		if (name) return _("恐怖のビジョン", "Vision of Terror");
		if (desc) return _("指定した一体のターゲットに恐ろしい幻覚を見せて恐怖させる。ユニークモンスターには効かず、高レベルな敵には効きにくい。",
                            "Scares target by showing them terrifying visions. Doesn't affect unique monsters and is less effective against high level enemies.");
		{
			int power = plev + 25;
			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fear_monster(dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "鑑定";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

		break;

	case 8:
		if (name) return _("周辺感知", "Sense Surroundings");
		if (desc) return _("周辺の地形を感知する。", "Creates a map of nearby area.");
    	{
			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "調査";
		if (desc) return "モンスターの属性、残り体力、最大体力、スピード、正体を知る。";
#else
		if (name) return "Probing";
		if (desc) return "Learns all monsters' alignment, HP, speed and their true character.";
#endif

		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "匠の輝き発見";
		if (desc) return "現在のフロアにある未入手の「名のあるアイテム」を感知し、大まかな距離を知る。アーティファクトは感知できない。";
#else
		if (name) return "Discover Craftsmanship";
		if (desc) return "Searches for unobtained ego-items on this floor and tells you how far away are you from them. Doesn't detect artifacts.";
#endif

		{
			if (cast)
			{
				search_specific_object(1);
			}
		}
		break;

	case 11:
		if (name) return _("精神感応", "Telepathy");
		if (desc) return _("一定時間、テレパシー能力を得る。", "Grants temporary telepathy.");

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 12:
		if (name) return _("幻覚治癒", "Cure Hallucinations");
		if (desc) return _("幻覚状態を治療する。", "Cures hallucinations.");

		{

			if (cast)
			{
				set_image(0);
			}
		}
		break;

	case 13:
		if (name) return _("空気感知", "Sense Atmosphere");
		if (desc) return _("現在いるフロアの雰囲気を察知する。", "Learns the feeling of current level.");

		{
			if (cast)
			{
				if(!dun_level)
				{
					msg_print(_("今使っても意味はない。", "No point in using that now."));
					return NULL;
				}
				p_ptr->feeling_turn = 0;
			}
		}
		break;

	case 14:
		if (name) return _("破滅のビジョン", "Vision of Doom");
		if (desc) return _("ターゲットの精神に破滅的なイメージを植え付けて攻撃する。抵抗されると無効。",
                            "Attacks by planting visions of devastation in target's mind. Does nothing if resisted.");

		{
			int base = plev * 2;
			int dice = 5 + (plev - 5) / 3;
			int sides = 9;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				damage = damroll(dice,sides) + base;
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball_hide(GF_BRAIN_SMASH, dir, damage, 0);
				break;
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "全感知";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "シャッフル";
		if (desc) return "カードの占いをする。";
#else
		if (name) return "Shuffle";
		if (desc) return "Causes random effects.";
#endif

		{
			if (info) return _("ランダム", "Random");

			if (cast)
			{
				cast_shuffle();
			}
		}
		break;

	case 17:
		if (name) return _("ユニーク・モンスター感知", "Detect Unique Monsters");
		if (desc) return _("現在のフロアにいる全ての「特別な強敵」の名前を知る。",
                            "Learn the names of all unique enemies present on this floor.");

		{
			int base = 20;

/*:::パランティア発動からコピー*/
			if (cast)
			{
			int i;
			monster_type *m_ptr;
			monster_race *r_ptr;
#ifdef JP
			msg_print("奇妙な場所が頭の中に浮かんだ．．．");
#else
			msg_print("Some strange places show up in your mind. And you see ...");
#endif
			/* Process the monsters (backwards) */
				for (i = m_max - 1; i >= 1; i--)
				{
					/* Access the monster */
					m_ptr = &m_list[i];
					/* Ignore "dead" monsters */
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(r_ptr->flags1 & RF1_UNIQUE)
					{
#ifdef JP
						msg_format("%s． ",r_name + r_ptr->name);
#else
						msg_format("%s. ",r_name + r_ptr->name);
#endif
					}
				}
				break;
			}
		}
		break;

	case 18:
		if (name) return _("*鑑定*", "*Identify*");
		if (desc) return _("アイテムの持つ能力を完全に知る。", "Fully learns all abilites of an item.");

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "自己分析";
		if (desc) return "現在の自分の状態を完全に知る。";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif

		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "耐混沌";
		if (desc) return "一定時間、混沌属性攻撃に対する耐性を得る。";
#else
		if (name) return "Resist Chaos";
		if (desc) return "Gives temporary resistance to chaos.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "*モンスター感知*";
		if (desc) return "現在のフロアにいる全てのモンスターを感知する。";
#else
		if (name) return "*Detect Monsters*";
		if (desc) return "Detects all monsters on current floor.";
#endif
		{
			if (cast)
			{
				(void)detect_monsters_invis(255);
				(void)detect_monsters_normal(255);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "未来予知";
		if (desc) return "ごく近い未来を見る。一時的にAC大幅上昇と命中率上昇と警告を得る。";
#else
		if (name) return "Future Sight";
		if (desc) return "Grants ability to see immediate future. Temporarily significantly raises AC and to-hit and grants warning.";
#endif

		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_foresight(randint1(base) + base, FALSE);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "伝説の品感知";
		if (desc) return "現在のフロアにあるアーティファクトを感知し、大まかな距離を知る。";
#else
		if (name) return "Detect Legendary Objects";
		if (desc) return "Searches for artifact on current floor and tells you how far away are you from them.";
#endif

		{
			if (cast)
			{
				search_specific_object(2);

			}
		}
		break;

	case 24:
		if (name) return _("ヒロインの特権", "Heroine's Privilege");
		if (desc) return _("一定時間、寝ている敵が極めて起きにくくなる。", "Temporarily makes it extremely difficult for sleeping enemies to wake up.");

		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_stealth(randint1(base) + base, FALSE);
			}
		}
		break;

	case 25:
		if (name) return _("*千里眼*", "*Clairvoyance*");
		if (desc) return _("フロア全ての地形、アイテム、罠を感知する。",
                            "Maps the entire floor and detects items and traps.");

		{
			if (cast)
			{
				wiz_lite(FALSE);
				msg_print(_("フロアの地形とアイテムを感じとった！", "You sense the layout of current floor and presence of items!"));
				(void)detect_traps(255,TRUE);

			}
		}
		break;


	case 26:
		if (name) return _("因果混乱", "Twist Causality");
		if (desc) return _("自分の周囲のダンジョンを破壊する。範囲内のアイテムとモンスターは全て消失する。",
                            "Destroys the dungeon around you. All items and monsters in range will disappear.");

		{
			int base = 12;
			int sides = 4;
			if (info) return format(_("範囲：%d+1d%d", "rad: %d+1d%d"),base,sides);

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "シャドウ・シフト";
		if (desc) return "平行世界に渡ることで今いるフロアを作り替える。フロアの再構成は、「ほとんど」瞬時に行われる。";
#else
		if (name) return "Shadow Shift";
		if (desc) return "Recreates current floor by travelling to a parallel floor. The floor is recreated almost instantly.";
#endif

		{
			if (cast)
			{
				if (!get_check(_("本当に別のフロアに移動しますか？", "Really move to a different floor?"))) return NULL;

				if (p_ptr->inside_arena || ironman_downward)
				{
					msg_print(_("何も起こらなかった。", "Nothing happens."));
				}
				else
				{
					/*:::0ではまずいはず*/
					p_ptr->alter_reality = 1;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 28:
		if (name) return _("破滅の宣告", "Proclamation of Doom");
		if (desc) return _("1体のモンスターに対し破滅的な災禍をもたらす。抵抗されると無効。ユニークモンスターには効かない。",
                            "Brings devastating calamity upon single monster. Does nothing if resisted. Doesn't affect unique monsters.");

		{
			if (info) return format(_("損傷：対象HPの40%-60%", "dam: 40%-60% of cur HP"));

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print(_("あなたは破滅的な運命を紡ぎ出した。",
                            "You weave forth calamitous fate."));
				/*:::45lev@→55levモンスターで1/4程度刺さった*/
				fire_ball_hide(GF_HAND_DOOM, dir, plev * 3, 0);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "幸運のメカニズム";
		if (desc) return "現在のフロアでごく短時間幸運になる。主に敵が落とすアイテムの質に影響する。";
#else
		if (name) return "Mechanism of Good Fortune";
		if (desc) return "Grants you good fortune on current floor for a short time. Mainly affects quality of items dropped by enemies.";
#endif

		{

			int base = 1 + plev / 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_lucky(10,FALSE);
			}
		}
		break;

	case 30:
		if (name) return _("因果修復", "Repair Causality");
		if (desc) return _("自分の体の全てを常態に戻す。",
                            "Fully heals and restores you and removes bad status effects.");

		{
			if (cast)
			{
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
				set_broken(-(BROKEN_MAX));//v1.1.69

				if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
				{
					if(muta_erasable_count()) msg_print(_("全ての突然変異が治った。",
                                                            "All of your mutations have been cured."));
					p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
					p_ptr->muta1 = p_ptr->muta1_perma;
					p_ptr->muta2 = p_ptr->muta2_perma;
					p_ptr->muta3 = p_ptr->muta3_perma;
//					p_ptr->muta3 = p_ptr->muta4_perma;//v1.1.80 ミス修正
					p_ptr->muta4 = p_ptr->muta4_perma;
					p_ptr->update |= PU_BONUS;
					p_ptr->bydo = 0;
					handle_stuff();
					mutant_regenerate_mod = calc_mutant_regenerate_mod();
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "アカシックレコード直結";
		if (desc) return "一時的に知能と賢さが限界を超えて上昇する。";
#else
		if (name) return "Akashic Record";
		if (desc) return "Temporarily raises intelligence and wisdom beyond usual limit.";
#endif

		{

			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_tim_addstat(A_INT,199,time, FALSE);
				set_tim_addstat(A_WIS,199,time, FALSE);
				msg_print(_("あなたの頭に全ての時空の全ての知識が流れこんできた！",
                            "All of knowledge in time and space flows through your mind!"));
			}


		}
		break;
	}

	return "";
}



static cptr do_new_spell_enchant(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	int dice,sides,base,damage;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
		if (name) return _("魔法の施錠", "Wizard Lock");
		if (desc) return _("閉じた扉を固く施錠する。施錠された扉は開錠できないが叩き壊したり魔法で開けることはできる。",
                            "Locks a closed door. It can't be unlocked, but it can be broken down or opened by magic.");

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wizard_lock(dir);
			}
		}
		break;

	case 1:
		if (name) return _("魔法の弩", "Magic Crossbow");
		if (desc) return _("魔力の込められた矢を放つ。クロスボウの矢を一本消費する。威力は矢の質で変わる。",
                            "Fires a mana-imbued bolt. Uses up a single crossbow bolt. Power depends on bolt quality.");

		{
			int mul = 2;
			cptr q,s;
			object_type *o_ptr;
			int type;
			int item;

			base = plev / 2 + 5;
			dice = 1;
			sides = 5;

			if (info) return format(_("損傷：%d+%dd%d～", "dam: %d+%dd%d~"),base,dice,sides);

			if (cast)
			{
				item_tester_tval = TV_BOLT;
				q = _("どの矢を発射しますか? ", "Fire which bolt?");
				s = _("あなたはクロスボウの矢を持っていない。", "You don't have crossbow bolts.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
				if (!get_aim_dir(&dir)) return NULL;
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
				if(o_ptr->sval == SV_AMMO_NORMAL)
				{
					dice += 2;
					mul = 3;
				}
				if(o_ptr->sval == SV_AMMO_HEAVY)
				{
					dice += 5;
					mul = 4;
				}
				damage = (base + damroll(dice,sides) + o_ptr->to_d) * mul;
				if(o_ptr->name2 == EGO_ARROW_HOLY) type = GF_HOLY_FIRE;
				else if(o_ptr->name2 == EGO_ARROW_ELEC) type = GF_ELEC;
				else if(o_ptr->name2 == EGO_ARROW_FIRE) type = GF_FIRE;
				else if(o_ptr->name2 == EGO_ARROW_COLD) type = GF_COLD;
				else type = GF_ARROW;

				fire_bolt(type, dir, damage);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "回復力強化";
		if (desc) return "一定時間、回復力が増強される。";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif

		{
			int base = MIN(25,plev);

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "トラップ/ドア破壊";
		if (desc) return "隣接する罠と扉を破壊する。";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif

		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;


	case 4:
#ifdef JP
		if (name) return "視力強化";
		if (desc) return "一定時間、赤外線視力と透明体視認を得る。";
#else
		if (name) return "Augment Eyesight";
		if (desc) return "Grants temporary infravision and ability to see invisible.";
#endif

		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
				set_tim_infra(base + randint1(base), FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "空腹充足";
		if (desc) return "魔力を栄養に変換する。";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger with mana.";
#endif

		{
			int base = 25;

			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "耐熱耐寒";
		if (desc) return "一定時間、火炎と冷気に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Heat and Cold";
		if (desc) return "Gives resistance to fire and cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "筋力増幅";
		if (desc) return "一定時間、腕力を増加させる。増加後の腕力は40を超えない。";
#else
		if (name) return "Augment Muscle";
		if (desc) return "Temporarily raises your strength. Will not go past 40.";
#endif

		{
			int base = 15 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format(_("あなたは腕力を強化した。", "Your strength has increased."));
				set_tim_addstat(A_STR,1+plev/10,base + randint1(base),FALSE);
			}
		}
		break;

	case 8:

		if (name) return _("魔法陣設置", "Place Magical Circle");
		if (desc) return _("迎撃用魔法陣を設置する。魔法陣は配下として扱われ、移動せずに魔法攻撃のみを行う。階移動すると消える。レベル40以降では一度に複数作れる。",
                            "Sets up a magical circle for interception. It is treated as a follower and performs magical attacks without moving. It disappears if you leave the level. You can place several at once at level 40.");

		{
			if (cast)
			{
				bool flag = FALSE;
				u32b mode = PM_FORCE_PET | PM_EPHEMERA;
				if(p_ptr->lev > 39) mode |= PM_ALLOW_GROUP;
				if (summon_specific(-1, py, px, p_ptr->lev, SUMMON_MAGICSIGN, mode)) flag = TRUE;

				if(flag) msg_print(_("あなたは宙に魔法陣を描いた。", "You draw a magical circle in the air."));
				else msg_print(_("魔法陣を描くのに失敗した。", "You fail to draw a magical circle."));

			}
		}
		break;




	case 9:
#ifdef JP
		if (name) return "耐電";
		if (desc) return "一定時間、電撃への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "耐酸";
		if (desc) return "一定時間、酸への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;
	case 11:
#ifdef JP
		if (name) return "狂戦士化";
		if (desc) return "狂戦士化し、恐怖を除去する。";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "魔力充填";
		if (desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "代謝急加速";
		if (desc) return "一定時間回復力が強化され、気分が高揚し、さらに毒に対する耐性を得る。";
#else
		if (name) return "Speed Metabolism";
		if (desc) return "Grants temporary regeneration, heroism and resistance to poison.";
#endif

		{
			int base = 15 + plev / 3;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "武器強化";
		if (desc) return "武器の命中率修正とダメージ修正を強化する。";
#else
		if (name) return "Enchant Weapon";
		if (desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif

		{
			if (cast)
			{
				if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "防具強化";
		if (desc) return "鎧の防御修正を強化する。";
#else
		if (name) return "Enchant Armor";
		if (desc) return "Attempts to increase +AC of an armor.";
#endif

		{
			if (cast)
			{
				if (!enchant_spell(0, 0, randint0(3) + 2)) return NULL;
			}
		}
		break;
	case 16:
		if (name) return _("ゴーレム製造", "Create Golem");
		if (desc) return _("ゴーレムを一体～複数生成する。触媒として金を消費する。",
                            "Creates one or more golems. Requires monetary payment.");

		{
			int lev = p_ptr->lev;
			int charge = lev * lev * 4;
			int i;
			bool flag = FALSE;
			if (info) return format(_("召喚レベル：%d 触媒：＄%d", "level: %d cost: $%d"),lev,charge);
			if (cast)
			{
				int num = randint1(2) + plev / 15;

				if(p_ptr->au < charge)
				{
					msg_print(_("あなたは充分なお金を持っていない。",
                                "You don't have enough money."));
					return NULL;
				}
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;

				for(i=0;i<num;i++) if (summon_specific(-1, py, px, lev, SUMMON_GOLEM, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;

				if(flag) msg_print(_("辺りの地面から人型の塊が起き上がった。",
                                    "Something human-shaped rises up from the ground."));
				else msg_print(_("ゴーレムを作るのに失敗した。", "Golem creation failed."));
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "皮膚硬化";
		if (desc) return "一定時間、ACを上昇させる。";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif

		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "全耐性";
		if (desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "身体加速";
		if (desc) return "一定時間、加速する。";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif

		{
			int base = plev/2;
			int sides = plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "爆発のルーン";
		if (desc) return "自分のいる床の上に、モンスターが通ると爆発してダメージを与えるルーンを描く。";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif

		{
			int dice = 7;
			int sides = 7;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				explosive_rune();
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "アイテム詳細分析";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Knowledge True";
		if (desc) return "*Identifies* an item.";
#endif
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "魔法剣";
		if (desc) return "一定時間、武器に冷気、炎、電撃、酸、毒のいずれかの属性をつける。武器を持たないと使えない。";
#else
		if (name) return "Mana Branding";
		if (desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_attack()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "魔法の鎧";
		if (desc) return "一定時間物理防御と魔法防御を上昇させ、さらにプレーヤーが受けるほぼ全てのダメージを2/3に減少させる。";
#else
		if (name) return "Magical armor";
		if (desc) return "Temporarily raises physical and magical defenses, also reduces almost all damage received to 2/3.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_magicdef(randint1(base) + base, FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "呪縛消去";
		if (desc) return "アイテムにかかった弱い呪いを解除する。レベル40以降では強い呪いも解除できる。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{

				if (plev < 40 && remove_curse() || plev > 39 && remove_all_curse())
				{
#ifdef JP
					msg_print("装備品の呪縛が解けた。");
#else
					msg_print("The curse on your equipment is lifted.");
#endif
				}
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "*魔力付与*";
		if (desc) return "指定した武器に魔力を込めて配下として戦わせる。能力は武器の強さにより変化する。倒されたり階移動したり解放されると効果が消えて武器はその場に落ちる。倒されたとき元の武器が劣化する。";
#else
		if (name) return "*Imbue with Mana*";
		if (desc) return "Imbues chosen weapon with magic, making it fight on your side as a follower. Its abilities depend on the weapon's strength. If you go to another level, dismiss it or if it gets defeated, the effect wears off and the weapon drops on the floor. The item will get disenchanted if it is defeated.";
#endif

		{
			if (cast)
			{
				int         item;
				object_type *o_ptr;
				cptr        q, s;
				int	new_o_idx;
				int i;
				int cnt=0,max;


				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					if (m_ptr->r_idx != MON_TSUKUMO_WEAPON1 ) continue;
					msg_print(_("既に作成済みだ。", "You already have imbued a weapon."));
					return NULL;
				}

				//v1.1.93 名称変更
				item_tester_hook = object_is_melee_weapon_except_strange_kind;

				q = _("どの武器に魔力を込めますか? ", "Imbue which weapon with mana?");
				s = _("使えそうな武器がない。", "You don't have suitable weapons.");

				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0) o_ptr = &inventory[item];
				else o_ptr = &o_list[0 - item];
				apply_mon_race_tsukumoweapon(o_ptr,MON_TSUKUMO_WEAPON1);

				new_o_idx = o_pop();

				if (!new_o_idx || !summon_named_creature(0, py, px, MON_TSUKUMO_WEAPON1, PM_EPHEMERA))
				{
					msg_print(_("失敗した。", "Spell failed."));
				}

				else //ここに入る時summon_named_creature()がTRUEで終わり、付喪神のパラメータ処理と生成が済んでいる
				{
					char m_name[80];
					monster_race *r_ptr = &r_info[MON_TSUKUMO_WEAPON1];
					object_copy(&o_list[new_o_idx], o_ptr);
					for (i = 1; i < m_max; i++)
					{
						monster_type *m_ptr = &m_list[i];
						if (m_ptr->r_idx == MON_TSUKUMO_WEAPON1 )
						{
							m_ptr->hold_o_idx = new_o_idx;
							break;
						}
					}
					if(i == m_max) msg_print(_("ERROR:*魔力付与*の動作がおかしい",
                                                "ERROR: *Imbue with Magic* acts weird"));
					/*:::新たな付喪神が元アイテムを持っている扱いにする*/
					o_list[new_o_idx].held_m_idx = i;
					o_list[new_o_idx].ix = 0;
					o_list[new_o_idx].iy = 0;
					o_list[new_o_idx].number = 1;

					/*:::思い出情報を全て得て打倒数カウントをリセット*/
					lore_do_probe(MON_TSUKUMO_WEAPON1);
					r_ptr->r_sights = 0;
					r_ptr->r_deaths = 0;
					r_ptr->r_pkills = 0;
					r_ptr->r_akills = 0;
					r_ptr->r_tkills = 0;

					object_desc(m_name,&o_list[new_o_idx],OD_NAME_ONLY);

					msg_format(_("%sは宙に浮き、あなたの意のままに動いた！", "%s floats in the air, moving according to your will!"),m_name);

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
			}

		}
		break;

	case 26:
#ifdef JP
		if (name) return "装備無力化";
		if (desc) return "武器・防具にかけられたあらゆる魔力を完全に解除する。";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif

		{
			if (cast)
			{
				if (!mundane_spell(TRUE)) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "匠の技";
		if (desc) return "装備中の並あるいは上質の武器に対し、永続的に何らかの属性や特性を付加する。";
#else
		if (name) return "Craftsman's Technique";
		if (desc) return "Makes current weapon a random ego weapon.";
#endif

		{
			if (cast)
			{
				brand_weapon(randint0(16));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "属性への免疫";
		if (desc) return "一定時間、冷気、炎、電撃、酸のいずれかに対する免疫を得る。";
#else
		if (name) return "Immunity";
		if (desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif

		{
			int base = 13;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_immune(base + randint1(base))) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "無生物破壊";
		if (desc) return "隣接した無生物に対し極めて強力な攻撃を行う。アンデッドやデーモンは対象に入らない。";
#else
		if (name) return "Destroy Nonliving";
		if (desc) return "Performs extremely powerful attack against an adjacent nonliving being, except undead or demons.";
#endif

		{
			int dice = plev;
			int sides = 50;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int y, x;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int dam = damroll(dice,sides);
					int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
					char m_name[80];
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					if(r_ptr->flags3 & RF3_NONLIVING)
					{
						msg_format(_("あなたは%sに触れ、分解した！", "You disintegrate %s with your touch!"),m_name);
						project(0, 0, y, x, dam, GF_DISINTEGRATE, flg, -1);
					}
					else
					{
						msg_format(_("あなたは%sに触れたが、どうやら無生物ではないようだ・・",
                                    "You touch %s, but it has no effect..."),m_name);
					}
					touch_zap_player(m_ptr);
				}
				else
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "*魔力充填*";
		if (desc) return "現在装備している全ての装備品の充填を早める。ただし銃の充填の効果は1/10になる。";
#else
		if (name) return "*Recharging*";
		if (desc) return "Reduces charge timeout for your current equipment. Only 1/10 as effective for guns.";
#endif

		{
			int base = plev * 2;
			if (info) return format(_("充填:%dターン分", "recharge: %d turns"),base);
			if (cast)
			{
				int inven;
				bool flag = FALSE;
				for(inven = INVEN_RARM;inven < INVEN_TOTAL;inven++)
				{
					object_type *o_ptr;
					char desc[80];

					o_ptr = &inventory[inven];
					if(o_ptr->timeout)
					{
						flag = TRUE;
						if(o_ptr->tval == TV_GUN) base *= 100;
						o_ptr->timeout -= base;
						if(o_ptr->timeout < 1)
						{
							o_ptr->timeout = 0;
							object_desc(desc,o_ptr,OD_NAME_ONLY);
							msg_format(_("%sは再充填された！", "%s is recharged!"),desc);
							p_ptr->window |= (PW_EQUIP);
						}
					}
				}
				if(!flag)
				{
					msg_format(_("充填が必要なものを身につけていない。", "You don't have equipment that requires recharging."));
					return NULL;
				}
			}
		}
		break;
	case 31:
		if (name) return _("超人化", "Superhuman");
		if (desc) return _("ごく短時間超高速で動き、身体能力が飛躍的に上昇する。",
                            "*Greatly* increases physical abilities and speed for a short time.");

		{
			int base = 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				int time = base + randint1(base);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_tim_addstat(A_STR,110,time,FALSE);
				set_tim_addstat(A_DEX,110,time,FALSE);
				set_tim_addstat(A_CON,110,time,FALSE);
				set_lightspeed(time, FALSE);
				set_tim_speedster(time, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();
				msg_print(_("スーパーマンになった気がする！", "You feel like a superhuman!"));
			}
		}
		break;
	}

	return "";
}





static cptr do_new_spell_nature(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_rng[] = "射程";
#else
	static const char s_dam[] = "dam: ";
	static const char s_rng[] = "rng ";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全ての見えるモンスターを感知する。";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "放電";
		if (desc) return "電撃の短いビームを放つ。";
#else
		if (name) return "Lightning";
		if (desc) return "Fires a short beam of lightning.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int range = plev / 6 + 2;

			if (info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);

			if (cast)
			{
				project_length = range;

				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "罠と扉感知";
		if (desc) return "近くの全ての罠と扉を感知する。";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "食糧生成";
		if (desc) return "食料を一つ作り出す。";
#else
		if (name) return "Produce Food";
		if (desc) return "Produces a Ration of Food.";
#endif

		{
			if (cast)
			{
				object_type forge, *q_ptr = &forge;

#ifdef JP
				msg_print("食料を生成した。");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				///item 食糧生成
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "日の光";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Daylight";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("日の光があなたの肉体を焦がした！");
#else
					msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "日の光", -1);
#else
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "daylight", -1);
#endif
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "動物習し";
		if (desc) return "動物1体を魅了する。抵抗されると無効。";
#else
		if (name) return "Animal Taming";
		if (desc) return "Attempts to charm an animal.";
#endif

		{
			//v1.1.90 強化
			int power = plev + 25;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_animal(dir, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "環境への耐性";
		if (desc) return "一定時間、冷気、炎、電撃に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Environment";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "傷と毒治療";
		if (desc) return "怪我を全快させ、毒を体から完全に取り除き、体力を少し回復させる。";
#else
		if (name) return "Cure Wounds & Poison";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif

		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "岩石溶解";
		if (desc) return "壁を溶かして床にする。";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif

		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "耐水";
		if (desc) return "一時的に水耐性を得る。";
#else
		if (name) return "Resist Water";
		if (desc) return "Grants temporary resistance to water.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_water(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "太陽光線";
		if (desc) return "光線を放つ。光りを嫌うモンスターに効果がある。";
#else
		if (name) return "Ray of Sunlight";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif

		{
			int dice = 10;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				msg_print("太陽光線が現れた。");
#else
				msg_print("A line of sunlight appears.");
#endif

				lite_line(dir, damroll(10, 8));
			}
		}
		break;

		/*:::足がらめ仕様変更　パワー上昇しユニークにも効くが飛行や壁抜けなど一部モンスターに効きにくい*/
	case 11:
#ifdef JP
		if (name) return "足がらめ";
		if (desc) return "視界内の全てのモンスターを減速させる。抵抗されると無効。";
#else
		if (name) return "Entangle";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif

		{
			int power = plev * 5 / 2 ;

			if (info) return info_power(power);

			if (cast)
			{
				msg_print(_("あなたは蔦や木の根を操った！", "You manipulated the vines and roots of the tree."));
				project_hack(GF_SLOW_TWINE, power);
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "自然の覚醒";
		if (desc) return "瞬間的に自然と一体化し、近くの地形、罠、モンスターを感知する。";
#else
		if (name) return "Nature Awareness";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif

		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_traps(rad2, TRUE);
				detect_doors(rad2);
				detect_stairs(rad2);
				detect_monsters_normal(rad2);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "鎌鼬";
		if (desc) return "指定した位置に真空の刃を巻き起こす。防御力の高い敵には効きにくい。";
#else
		if (name) return "Kamaitachi";
		if (desc) return "Creates a vacuum blade at target location. Less effective against enemies with high AC.";
#endif

		{
			int dice = 1 + plev / 7;
			int sides = plev;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(!fire_ball_jump(GF_WINDCUTTER, dir, damroll(dice, sides),3,NULL)) return NULL;
			}
		}
		break;
	case 14:
#ifdef JP
		if (name) return "薬草治療";
		if (desc) return "秘伝の薬草による治療を行う。HPを中程度回復させて毒と切り傷を治す。";
#else
		if (name) return "Herbal Healing";
		if (desc) return "Heals HP greatly. And heals cut, stun and poison completely.";
#endif

		{
			int heal = plev * 5;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "動物召喚";
		if (desc) return "動物を1体召喚する。";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons an animal.";
#endif

		{
			if (cast)
			{
				if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				{
#ifdef JP
					msg_print("動物は現れなかった。");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "肌石化";
		if (desc) return "一定時間、ACを上昇させる。";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif

		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "天狗礫";
		if (desc) return "敵一体に対し石塊を降らせて攻撃する。森の近くをターゲットとして明示的に指定した場合は木が倒れてくる「天狗倒し」が起こり威力が上がる。";
#else
		if (name) return "Tengu-Tsubute";
		if (desc) return "Drops a rock on a single enemy. If target is standing close to forest terrain, a tree will fall down causing extra damage.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 12;
			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				bool wood = FALSE;
				int i,xx,yy;
				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay() && projectable(py,px,target_row,target_col))
				{
					if(cave_have_flag_bold((target_row), (target_col), FF_TREE)) wood = TRUE;
					for(i=0;i<8;i++)
					{
						yy = target_row + ddy_cdd[i];
						xx = target_col + ddx_cdd[i];
						if(!in_bounds(yy,xx)) continue;
						if(cave_have_flag_bold((yy), (xx), FF_TREE)) wood = TRUE;
					}
				}
				if(wood)
				{
					sides *= 2;
					if(!fire_ball_jump(GF_ARROW,dir,damroll(dice,sides),0,_("地響きとともに木が一本倒れた！",
                                                                            "There is a loud sound as a tree falls down!"))) return NULL;
				}
				else
				{
					msg_print(_("石礫が飛んだ！", "A rock falls down from above!"));
					fire_bolt(GF_ARROW,dir,damroll(dice,sides));
				}

			}
		}
		break;


	case 18:
#ifdef JP
		if (name) return "真・耐性";
		if (desc) return "一定時間、酸、電撃、炎、冷気、毒に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resistance True";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "森林創造";
		if (desc) return "周囲に木を作り出す。";
#else
		if (name) return "Forest Creation";
		if (desc) return "Creates trees in all adjacent squares.";
#endif

		{
			if (cast)
			{
				tree_creation();
			}
		}
		break;
///mod151024 動物融和→鷹使いに変更
/*
	case 20:
#ifdef JP
		if (name) return "動物友和";
		if (desc) return "視界内の全ての動物を魅了する。抵抗されると無効。";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_animals(power);
			}
		}
		break;
*/
	case 20:
#ifdef JP
		if (name) return "鷲使い";
		if (desc) return "野生の大鷲にアイテムを自宅まで運んでもらう。ただしあまり重いものを運ばせることはできない。";
#else
		if (name) return "Eagle Familiar";
		if (desc) return "Summons a wild eagle to carry an item to your house. Cannot carry too heavy items.";
#endif

		{
			int weight = plev * 2;

			if (info) return info_weight(weight);

			if (cast)
			{
				msg_print(_("大鷲がどこからともなく飛んできた。", "An eagle flies out of nowhere."));
				item_recall(1);
			}
		}
		break;

	case 21:
		if (name) return _("吹雪", "Blizzard");
		if (desc) return _("視界内全てに対し冷気攻撃を行う。",
                            "Hits everything in sight with cold.");

		{
			int dam = 50 + plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print(_("部屋中が吹雪に覆われた！", "Strong blizzard blows through the room!"));
				project_hack2(GF_COLD, 0,0,dam);

			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "石の壁";
		if (desc) return "自分の周囲に花崗岩の壁を作る。";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif

		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "召雷";
		if (desc) return "視界内のランダムな敵に対し強力な雷を落とす。";
#else
		if (name) return "Summon Lightning";
		if (desc) return "Strikes a random enemy in sight with a powerful lightning bolt.";
#endif

		{
			int dam = plev*4 + 100;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{

				if(!fire_random_target(GF_ELEC, dam,4, rad,0))
				{
					msg_print(_("雷が届く範囲には何もいないようだ。",
                                "There are no suitable targets for the lightning to strike."));
					return NULL;
				}
			}
		}
		break;
	case 24:
		if (name) return _("妖精召喚", "Summon Fairies");
		if (desc) return _("友好的な妖精を大量に召喚する。", "Summons a lot of friendly fairies.");

		{
			int num;
			if (info) return "";
			if (cast)
			{
				for(num = plev / 15 + 1;num > 0;num--)
				if (!(summon_specific(-1, py, px, plev, SUMMON_FAIRY, (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))))
				{
#ifdef JP
					msg_print("妖精は現れなかった。");
#else
					msg_print("No fairies arrive.");
#endif
				}
				break;
			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "地震";
		if (desc) return "周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。";
#else
		if (name) return "Earthquake";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cast)
			{
				earthquake(py, px, rad);
			}
		}
		break;


	case 26:
		if (name) return _("竜巻", "Tornado");
		if (desc) return _("巨大な竜巻を発生させる。巨大な敵や飛んでいる敵には効果が薄い。",
                            "Creates a huge tornado. Less effective against gigantic or flying enemies.");

		{
			int dam = 100 + plev * 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(!fire_ball_jump(GF_TORNADO, dir, dam, rad,_("竜巻を起こした！", "You create a tornado!"))) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "精霊の刃";
		if (desc) return "装備中の並あるいは上質の武器に対し、永続的に何らかの元素属性を付加する。";
#else
		if (name) return "Elemental Branding";
		if (desc) return "Makes current weapon branded with an element.";
#endif

		{
			if (cast)
			{
				brand_weapon(randint0(5));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "陽光召喚";
		if (desc) return "視界内を閃光で攻撃し、さらに広範囲の地形とアイテムとトラップとモンスターを感知する。";
#else
		if (name) return "Call Sunlight";
		if (desc) return "Hits everything in sight with light; also maps the surrounding area and detects items, traps and monsters.";
#endif

		{
			int dam = 150;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				project_hack2(GF_LITE, 0, 0, dam);

				map_area(plev);
				detect_all(MIN(plev,50));

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("日光があなたの肉体を焦がした！");
#else
					msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, 50, "日光", -1);
#else
					take_hit(DAMAGE_NOESCAPE, 50, "sunlight", -1);
#endif
				}
			}
		}
		break;

	case 29:
		if (name) return _("水脈収束", "Converge Water Veins");
		if (desc) return _("水脈を収束させ高圧の水柱で攻撃する。",
                            "Converges water veins, launching a high pressure water pillar.");

		{
			int dam = 200 + plev * 4;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				cptr msg;
				if (!get_aim_dir(&dir)) return NULL;

				if(dun_level == 0) msg = _("幻想郷の雨を収束させた！", "You converge the rains of Gensoukyou!");
				else msg = _("地下水脈を収束させた！", "You converge the underground water veins!");

				if(!fire_ball_jump(GF_WATER_FLOW, dir, 3, rad,msg)) return NULL;
				fire_ball_jump(GF_WATER, dir, dam, rad,NULL);
			}
		}
		break;

	case 30:
		if (name) return _("聖獣召喚", "Summon Sacred Beast");
		if (desc) return _("麒麟・鳳凰・応龍・霊亀のいずれかを召喚する。",
                            "Summons a Kirin, Fenghuang, Yinglong or Linggui.");

		{
			if (info) return "";
			if (cast)
			{
				if (summon_specific(-1, py, px, plev*2, SUMMON_HOLY_ANIMAL, (PM_FORCE_PET)))
				{
					msg_print(_("あなたの声に応えて伝説の聖獣が現れた！", "A legendary sacred beast responds to your call!"));
				}
				else
				{
					msg_print(_("聖獣は現れなかった。", "No sacred beasts arrive."));
				}
				break;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "天変地異";
		if (desc) return "自分を中心に魔力、轟音、水、電撃、火炎の巨大ボールを順に発生させ、さらに地震を起こし床を溶岩にする。";
#else
		if (name) return "Nature's Wrath";
		if (desc) return "Generates huge balls of mana, sound, water, lightning and fire centered on you; also causes an earthquake and turns floor into lava.";
#endif

		{
			int rad = 6;

			if (cast)
			{

				project(0, rad, py, px, 450, GF_MANA, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 350, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 250, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 200, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 150, GF_FIRE, PROJECT_KILL | PROJECT_ITEM, -1);
				earthquake(py, px, rad);
				project(0, rad, py, px, 8, GF_LAVA_FLOW, PROJECT_GRID, -1);

			}
		}
		break;
	}

	return "";
}

/*:::死霊領域新スペル*/

static cptr do_new_spell_necromancy(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
	static const char s_random[] = "ランダム";
#else
	static const char s_dam[] = "dam: ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "呪符";
		if (desc) return "呪いの札を飛ばして攻撃する。命を持たない者には効果がない。";
#else
		if (name) return "Malediction";
		if (desc) return "Throws a cursing charm. Does not effect nonliving beings.";
#endif

		{
			int dice = 4 + plev / 5;
			int sides = 4;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISP_LIVING, dir, damroll(dice, sides), rad);

			}
		}
		break;
	case 1:
#ifdef JP
		if (name) return "無生命感知";
		if (desc) return "アンデッド・デーモン・無生物など命を持たない者を感知する。";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "アンデッド退散";
		if (desc) return "アンデッド一体を恐怖させる。";
#else
		if (name) return "Turn Undead";
		if (desc) return "Scares a single undead monster.";
#endif

		{
			int power = 25 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_TURN_UNDEAD, dir, power, 0);
			}
		}
		break;

	case 3:
#ifdef JP
		//v1.1.90 催眠→金縛りにしてsleep→stasis系にした
		if (name) return "金縛り";
		if (desc) return "1体のモンスターの動きを止める。抵抗されると無効。";
#else
		if (name) return "Paralysis";
		if (desc) return "Stops a single monster from moving. Does nothing if resisted.";
#endif

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				stasis_monster(dir);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "鬼火";
		if (desc) return "火炎属性の小さな玉を放つ。";
#else
		if (name) return "Oni Fire";
		if (desc) return "Fires a small ball of fire.";
#endif

		{
			int dam = 10 + plev ;
			int rad = 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "耐地獄";
		if (desc) return "一定時間、地獄属性への耐性を得る。";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "スケルトン生成";
		if (desc) return "骨のモンスターを1体召喚する。";
#else
		if (name) return "Create Skeleton";
		if (desc) return "Summons a skeletal monster.";
#endif

		{
			int power = plev;


			if (cast)
			{
				int type = SUMMON_SKELETON;
				u32b mode = PM_FORCE_PET;
				if(plev > 34) mode |= PM_ALLOW_GROUP;
				if (summon_specific(-1, py, px, plev , type, mode))
				{
					msg_print(_("地面から骸骨が這い出し、あなたに従った。", "Bones crawl out of the ground to serve you."));
				}
				else
				{
					msg_print(_("地面が少し動いた気がするが、何も起こらなかった。", "You feel the ground move slightly, but nothing happens."));

				}
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "アンデッド従属";
		if (desc) return "アンデッド1体を配下にする。失敗することもある。";
#else
		if (name) return "Enslave Undead";
		if (desc) return "Attempts to charm an undead monster.";
#endif

		{
			int power = plev + 20;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_undead(dir, power);
			}
		}
		break;
	case 8:
#ifdef JP
		if (name) return "地獄の矢";
		if (desc) return "地獄のボルトもしくはビームを放つ。アンデッドにはほぼ効かずデーモンには効果が薄い。";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether. Less effective against demons and has particularly no effect on undead.";
#endif

		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "悪霊召喚";
		if (desc) return "複数の幽霊系モンスターを召喚し配下にする。";
#else
		if (name) return "Summon Evil Spirits";
		if (desc) return "Summons multiple ghost monsters as your followers.";
#endif

		{
			int power = plev * 2 / 3 + 1;
			int num = randint1(3) + plev / 10;
			bool msgflag = FALSE;
			int i;


			if (cast)
			{
				int type = SUMMON_GHOST;
				u32b mode = PM_FORCE_PET;
				if(plev > 34) mode |= PM_ALLOW_GROUP;
				for(i=0;i<num;i++) if (summon_specific(-1, py, px, plev , type, mode)) msgflag=TRUE;

				if(msgflag)	msg_print(_("幽霊たちが現れ、あなたに従った。", "Ghosts appear to serve you."));
				else msg_print(_("生温い風が吹いた気がするが、何も起こらなかった。", "A chilling wind blows, but nothing happens."));
			}
		}
		break;


	case 10:
#ifdef JP
		if (name) return "念縛";
		if (desc) return "一定時間、自分自身と周囲の敵のテレポートを妨害する。";
#else
		if (name) return "Binding";
		if (desc) return "Temporarily prevents you or nearby enemies from teleporting.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_nennbaku(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "残留思念解読";
		if (desc) return "アイテムの大まかな価値を判定する。レベル30以上で通常の鑑定と同じ効果になる。";
#else
		if (name) return "Read Lingering Thoughts";
		if (desc) return "Gives a rough estimate of an item's value. At level 30 identifies the item instead.";
#endif

		{
			if (cast)
			{
				if (plev < 30)
				{
					if(!psychometry()) return NULL;
				}
				else
				{
					if (!ident_spell(FALSE)) return NULL;
				}
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "怨霊憑依";
		if (desc) return "モンスター一体に怨霊を憑依させ、高確率で狂戦士化させる。抵抗されると無効。妖怪に効きやすいが通常の精神を持たないモンスターには効かない。";
#else
		if (name) return "Vengeful Spirit Possession";
		if (desc) return "Makes a single monster possessed a vengeful spirit, making it berserk with a high probability rate. Does nothing if resisted. Works well against youkai, but does not affect monsters with unusual mind.";
#endif

		{
			int power = plev * 2;
			int rad = 0;

			if (cp_ptr->magicmaster) power *= 2;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POSSESSION, dir, power, rad);
			}
		}
		break;
	case 13:
#ifdef JP
		if (name) return "死者召喚";
		if (desc) return "1体のアンデッドを召喚し配下にする。召喚されたアンデッドが敵対することもある。";
#else
		if (name) return "Raise the Dead";
		if (desc) return "Summons an undead monster. Summoned undead might be hostile.";
#endif

		{
			if (cast)
			{
				int type;
				bool pet = TRUE;
				u32b mode = 0L;

				if(plev + adj_general[p_ptr->stat_ind[A_WIS]] < randint1(80)) pet = FALSE;

				type = SUMMON_UNDEAD;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

				if (summon_specific((pet ? -1 : 0), py, px, plev*5/4, type, mode))
				{
					if (pet)
					{
#ifdef JP
						msg_print("地面から死者が這い出し、あなたに従った。");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("地面から死者が這い出し、あなたに襲い掛かってきた！");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
				}
			}
		}
		break;
	case 15:
#ifdef JP
		if (name) return "使嗾";
		if (desc) return "フロアにいるアンデッドの配下をターゲット周辺に呼び寄せる。";
#else
		if (name) return "Instigate";
		if (desc) return "Transports your undead followers on this floor around the target.";
#endif

		{
			int num = 1 + plev / 7;

			if(num > 8) num = 8;

			if (info) return format(_("最大：%d体", "max: %d"),num);

			if (cast)
			{
				int i;
				int cnt=0;
				int idx[8]; //最大8体
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					if (!m_ptr->r_idx) continue;
					if (!is_pet(m_ptr) || !(r_ptr->flags3 & RF3_UNDEAD)) continue;
					/*:::アンデッドのペットのidxを配列に格納 呼び出せる以上の数の配下がいたら候補から適当に入れ替える*/
					///このやり方だと出てくる配下がある程度固定化されてしまうかもしれないがまあ実用上大した影響はないだろう。
					if(cnt<num)idx[cnt++] = i;
					else idx[randint0(num)] = i;
				}
				if(cnt==0)
				{
					msg_format(_("あなたにはアンデッドの配下がいない。", "You have no undead followers."));
					return NULL;
				}
				if (!get_aim_dir(&dir)) return NULL;

				msg_format(_("配下のアンデッドを呼び出した！", "You call forth your undead followers!"));
				for(i=0;i<cnt&&i<num;i++)
				{
					teleport_monster_to(idx[i], target_row, target_col, 100, TELEPORT_PASSIVE);
				}

			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "魔力吸収";
		if (desc) return "敵一体に精神攻撃を行いMPを吸収する。通常の精神を持たない敵には効果が薄い。";
#else
		if (name) return "Drain Mana";
		if (desc) return "Drains MP from a single enemy with a mental attack. Less effective against enemies with an unusual mind.";
#endif

		{
			int dice= 1;
			int sides= plev * 3;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_PSI_DRAIN, dir, damroll(dice,sides)+base , 0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "死の言霊";
		if (desc) return "敵一体を強力な呪いで攻撃する。抵抗されると無効。命を持たない敵には効果がない。";
#else
		if (name) return "Word of Death";
		if (desc) return "Attacks a single enemy with a powerful curse. Does nothing if resisted. Does not effect nonliving enemies.";
#endif

		{
			int dice=15,sides=15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_CAUSE_4, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "精神感応";
		if (desc) return "一定時間、テレパシーを得る。";
#else
		if (name) return "Telepathy";
		if (desc) return "Grants temporary telepathy.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "アンデッド支配";
		if (desc) return "視界内のアンデッドを配下にしようと試みる。抵抗されると無効。";
#else
		if (name) return "Control Undead";
		if (desc) return "Tries to make all undead monsters in sight your followers. Does nothing if resisted.";
#endif

		{
			int power = plev + 20;

			if (info) return info_power(power);

			if (cast)
			{
				charm_undead(power);
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "死の光線";
		if (desc) return "死の光線を放つ。";
#else
		if (name) return "Death Ray";
		if (desc) return "Fires a beam of death.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				death_ray(dir, plev);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "死霊のオーラ";
		if (desc) return "大量の死霊を身にまとう。一定時間AC上昇と地獄耐性と元素耐性を得て、さらに攻撃してきた敵を朦朧・混乱・恐怖させる。";
#else
		if (name) return "Aura of Ghosts";
		if (desc) return "Surrounds your body with multitude of ghosts. Temporarily increases AC, grants resistance to nether and basic elements; also stuns, confuses and terrifies enemies attacking you.";
#endif

		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				int time = randint1(sides) + base;
				set_tim_sh_death(time,FALSE);
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "呪殺法陣";
		if (desc) return "強力な呪いのエネルギーを炸裂させる。命を持たない者には効果がない。";
#else
		if (name) return "Malediction Blast";
		if (desc) return "Creates a powerful explosion of curse energy. Does not effect nonliving beings.";
#endif

		{
			int dam = plev * 2 + 100;
			int sides = 100;
			int rad = plev / 20 + 2;

			if (info) return info_damage(1, sides, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(!fire_ball_jump(GF_DISP_LIVING, dir, dam + randint1(sides), rad,NULL)) return NULL;
			}
		}
		break;


	case 22:
#ifdef JP
		if (name) return "吸血鬼変化";
		if (desc) return "一定時間、吸血鬼に変化する。変化している間は本来の種族の能力を失い、代わりに吸血鬼としての能力を得る。";
#else
		if (name) return "Vampire Form";
		if (desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif

		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_VAMPIRE, FALSE);
			}
		}
		break;
	case 23:
#ifdef JP
		if (name) return "上位アンデッド召喚";
		if (desc) return "強力なアンデッドを複数召喚し配下にする。召喚されたアンデッドが敵対することもある。";
#else
		if (name) return "Summon Greater Undead";
		if (desc) return "Summons multiple powerful undead monsters as your followers. Summoned monsters might be hostile.";
#endif

		{
			if (cast)
			{
				int type;
				bool pet =TRUE;
				u32b mode = 0L;
				int num,i;
				bool msgflag=FALSE;

				if(plev + adj_general[p_ptr->stat_ind[A_WIS]] < randint1(120)) pet = FALSE;

				type = SUMMON_HI_UNDEAD;

				mode |= PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);
				num = 1 + plev / 45 + randint0(1);

				for(i=0;i<num;i++) if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, type, mode) && !msgflag)
				{
					msgflag = TRUE;
#ifdef JP
					msg_print("冷たい風があなたの周りに吹き始めた。それは腐敗臭を運んでいる...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("古えの死せる者共があなたに仕えるため土から甦った！");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("死者が甦った。眠りを妨げるあなたを罰するために！");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "屍竜召喚";
		if (desc) return "ドラゴンのアンデッドを召喚する。触媒として竜の鱗などの物品を消費する。";
#else
		if (name) return "Summon Undead Dragon";
		if (desc) return "Summons an undead dragon. Consumes a dragon-related item as extra cost.";
#endif

		{
			u32b summon_mode = 0L;
			int lev = plev ;
			if (info) return format("");
			if (cast)
			{
				int num = 1;
				int cost;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					summon_mode |= PM_FORCE_PET;
					if(!select_pay(&cost)) return NULL;

					if(cost <= 0) //無価値なアイテムを使うと失敗する
					{
						summon_mode &= ~(PM_FORCE_PET);
						summon_mode |= PM_NO_PET;
					}
					lev += cost / 2000;
					if(lev > 100) lev = 100;
				}

				if (new_summoning(num, py, px, lev, SUMMON_UNDEAD_DRAGON, summon_mode))
				{
					if(summon_mode & PM_NO_PET) msg_print(_("触媒が悪かったらしい。竜が襲いかかってきた！",
                                                            "Your offering was worthless. The dragon attacks you!"));
					else     msg_print(_("地の底から響くような唸り声が聞こえた・・",
                                        "You hear a growling sound coming from depths of the earth..."));
				}
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "破滅の手";
		if (desc) return "破滅の手を放つ。食らったモンスターはそのときのHPの半分前後のダメージを受ける。";
#else
		if (name) return "Hand of Doom";
		if (desc) return "Attempts to make a monster's HP almost half.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				else msg_print("<破滅の手>を放った！");
#else
				else msg_print("You invoke the Hand of Doom!");
#endif

				fire_ball_hide(GF_HAND_DOOM, dir, plev * 2, 0);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "地獄の嵐";
		if (desc) return "巨大な地獄属性の球を放つ。";
#else
		if (name) return "Nether Storm";
		if (desc) return "Generate a huge ball of nether.";
#endif

		{
			int dam = plev * 7;
			int rad = plev / 7;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "復讐の契約";
		if (desc) return "一定時間、自分がダメージを受けたときに攻撃を行ったモンスターに対して同等のダメージを与える。";
#else
		if (name) return "Oath of Vengeance";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif

		{
			int base = 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_eyeeye(randint1(base) + base, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "ソウルスティール";
		if (desc) return "生命のある敵を一撃で倒し、そのHPを吸収する。抵抗されると無効。力強い敵や賢い敵には効きにくい。";
#else
		if (name) return "Soul Steal";
		if (desc) return "Defeats living enemy in a single blow and drains their HP. Does nothing if resisted. Less effective against powerful or smart enemies.";
#endif

		{
			int power = plev * 7 / 2;
			int rad = 0;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_SOULSTEAL, dir, power, rad);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "幽体化";
		if (desc) return "一定時間、壁を通り抜けることができ受けるダメージが軽減される幽体の状態に変身する。";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_wraith_form(randint1(base) + base, FALSE);
			}
		}
		break;



	case 30:
#ifdef JP
		if (name) return "血の呪い";
		if (desc) return "極めて強力な呪いにより敵1体に大ダメージを与え、時々何らかの追加効果を引き起こす。生命のない敵にも効果がある。30～100のHPを消費し、体力が減っているほど威力が高まる。";
#else
		if (name) return "Bloody Curse";
		if (desc) return "Puts blood curse which damages and causes various effects on a monster. Effective against nonliving enemies as well. Drain 30-100 HP, and is more effective the less HP you have.";
#endif

		{
			int base = plev * 10;
			int sides = plev * 10;
			int rad = 0;
			int mult = 50 + 300 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;
			base = base * mult / 100;
			sides = sides * mult / 100;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				int dam_add = randint1(sides);
				int uselife = 30 + 70 * dam_add / sides;

				if(uselife > 100) uselife = 100;

				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_BLOOD_CURSE, dir, base + dam_add , rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, uselife, "血の呪い", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", -1);
#endif
			}
		}
		break;




	case 31:
#ifdef JP
		if (name) return "幽冥の鬼神";
		if (desc) return "強力なアンデッドであるリッチに転生する。この変化は永続的なものである。一部の種族やクラスでは使用できない。大抵の場合スコアが大幅に下がる。人里にいないと使えない。";
#else
		if (name) return "Demon God of Netherworld";
		if (desc) return "You permanently become a lich. Cannot be used by several races and classes. Significantly reduces your score in most of the cases. Cannot be used outside of Human Village.";
#endif

		{

			if (cast)
			{
				if(p_ptr->prace == RACE_LICH)
				{
					msg_print(_("あなたはすでに死の権化だ。", "You are already death incarnate."));
					return NULL;
				}
				if(RACE_NEVER_CHANGE)
				{
					msg_print(_("あなたの体は転生という行為に適さないようだ。", "You cannot change your race."));
					return NULL;
				}
				if(cp_ptr->flag_only_unique)
				{
					msg_print(_("……嫌な予感がする。やっぱりやめておこう。", "...You have a bad feeling about this and decide against it."));
					return NULL;
				}
				if(dun_level || p_ptr->town_num != TOWN_HITOZATO)
				{
					msg_print(_("ここで秘術を執り行うのは危険だ。", "It's dangerous to perform the ritual in this place."));
					return NULL;
				}
				if (!get_check(_("転生を行います。よろしいですか？", "You're going to change your race. Are you sure?"))) return NULL;

				msg_print(_("あなたは書物を書き綴り、こっそり鈴奈庵の在庫に紛れ込ませた・・",
                            "You write a book, and sneak it into Suzunaan's inventory..."));
				world_monster = -1;
				do_cmd_redraw();
				Term_xtra(TERM_XTRA_DELAY, 3000);
				teleport_player_to(34,81,TELEPORT_NONMAGICAL);
				world_monster = 0;
				p_ptr->redraw |= PR_MAP;
				redraw_stuff();
				change_race(RACE_LICH,"");
			}
		}
		break;
	}

	return "";
}




static cptr do_new_spell_life(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "軽傷の治癒";
		if (desc) return "怪我と体力を少し回復させる。";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif

		{
			int dice = 2;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "祝福";
		if (desc) return "一定時間、命中率とACと魔法防御力にボーナスを得る。";
#else
		if (name) return "Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif

		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "解毒";
		if (desc) return "体内の毒を取り除く。";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cure poison status.";
#endif

		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;


	case 3:
#ifdef JP
		if (name) return "光の召喚";
		if (desc) return "光源が照らしている範囲か部屋全体を永久に明るくする。";
#else
		if (name) return "Call Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "生命体感知";
		if (desc) return "近くの命を持つモンスターを感知する。";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_living(rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "回復力強化";
		if (desc) return "一定時間、回復力が増強される。";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif

		{
			int base = 80;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;


	case 6:
#ifdef JP
		if (name) return "空腹充足";
		if (desc) return "満腹にする。";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif

		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "治癒";
		if (desc) return "体力を大幅に回復させ、負傷と朦朧状態も全快する。";
#else
		if (name) return "Curing";
		if (desc) return "Greatly recovers HP and cures cuts and stunning.";
#endif

		{
			int dice = 3 + plev / 3;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "解呪";
		if (desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "耐熱耐寒";
		if (desc) return "一定時間、火炎と冷気に対する耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Heat and Cold";
		if (desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "ターンアンデッド";
		if (desc) return "視界内のアンデッドを恐怖させる。抵抗されると無効。";
#else
		if (name) return "Turn Undead";
		if (desc) return "Attempts to scare undead monsters in sight.";
#endif

		{
			if (cast)
			{
				turn_undead();
			}
		}
		break;


	case 11:
#ifdef JP
		if (name) return "耐地獄";
		if (desc) return "一定時間、地獄への耐性を得る。";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "耐毒";
		if (desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "鑑識";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Perception";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "体力回復";
		if (desc) return "極めて強力な回復呪文で、負傷と朦朧状態も全快する。";
#else
		if (name) return "Healing";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif

		{
			int heal = 300;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "結界の紋章";
		if (desc) return "自分のいる床の上に、モンスターが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if (name) return "Glyph of Warding";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif

		{
			if (cast)
			{
				warding_glyph();
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "精神感応";
		if (desc) return "一定時間、テレパシーを得る。";
#else
		if (name) return "Telepathy";
		if (desc) return "Grants temporary telepathy.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "肉体強化";
		if (desc) return "一定時間、腕力・器用さ・耐久力を上昇させる。通常の限界値を超える。";
#else
		if (name) return "Physical Augmentation";
		if (desc) return "Temporarily increases strength, dexterity and constitution. Can exceed ordinary limits.";
#endif

		{
			int base = 10 + p_ptr->lev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = base + randint1(base);
				set_tim_addstat(A_STR,100 + 1 + p_ptr->lev / 15,time,FALSE);
				set_tim_addstat(A_DEX,100 + 1 + p_ptr->lev / 15,time,FALSE);
				set_tim_addstat(A_CON,100 + 1 + p_ptr->lev / 15,time,FALSE);
				msg_print(_("肉体を急激に活性化させた！", "Your physical abilities get rapidly augmented!"));

			}
		}
		break;



	case 18:
#ifdef JP
		if (name) return "*解呪*";
		if (desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("誰かに見守られているような気がする。");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;



	case 19:
#ifdef JP
		if (name) return "ディスペル";
		if (desc) return "アンデッド一体を中確率で一撃で倒す。抵抗されると無効。ターゲットが近いほど効きやすい。";
#else
		if (name) return "Dispel";
		if (desc) return "Has a medium chance of defeating an undead monster in a single strike. Does nothing if resisted. More effective against nearby targets.";
#endif

		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;

				y = target_row;
				x = target_col;
				m_ptr = &m_list[target_who];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int power;
					char m_name[80];
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					power = p_ptr->lev * 3 / 2 - m_ptr->cdis * 4;
					if(power<1) power=1;

					///mod140324 本家に倣って5%の成功保証を適用する。これほど適当解釈しといて何を今更という気もするが。
					msg_format(_("天から光が降り注いだ！", "Light shines down from heavens!"));
					if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && ( power / 2 + randint1( power / 2 ) > r_ptr->level || one_in_(20)))
					{
						msg_format(_("%sは光の中に消えた。", "%s fades away in light."),m_name);
						damage = m_ptr->hp + 1;
						project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					}
					else if(r_ptr->flags3 & RF3_UNDEAD)
					{
						msg_format(_("%sは解呪に抵抗した！", "%s resists being dispelled!"),m_name);
					}
					else
					{
						msg_format(_("%sには全く効いていないようだ。", "%s isn't affected at all."),m_name);
					}
				}
				else
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "他者治療";
		if (desc) return "生命のあるターゲット1体を治療する。ターゲットの最大HPの10%か200の多いほうの数値を回復させ朦朧を治す。";
#else
		if (name) return "Cure Other";
		if (desc) return "Cures a living being. Recovers 200 HP or 10% of target's maximum HP (whichever is greater) and cures stunning.";
#endif

		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				int target_m_idx;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay())
				{
					y = target_row;
					x = target_col;
					target_m_idx = cave[y][x].m_idx;
				}
				else
				{
					msg_format(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}
				//m_ptr = &m_list[target_who];
				m_ptr = &m_list[target_m_idx];

				if (!los(py,px,m_ptr->fy,m_ptr->fx) )
				{
					msg_format(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}

				else if (m_ptr->r_idx && (m_ptr->ml))
				{
					int heal;
					char m_name[80];
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					heal = m_ptr->maxhp / 10;
					if(heal < 200) heal = 200;
					if(monster_living(r_ptr))
					{
						m_ptr->hp += heal;
						if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
						set_monster_stunned(cave[y][x].m_idx,0);
						msg_format(_("%^sの傷を治療した。", "You cure %s."), m_name);
						if (p_ptr->health_who == target_m_idx) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == target_m_idx) p_ptr->redraw |= (PR_UHEALTH);
					}
					else
						msg_format(_("%^sの傷を治療することはできなかった。", "You are unable to cure %s."), m_name);

				}
				else
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}


			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "耐混沌";
		if (desc) return "一定時間、混沌属性攻撃に対する耐性を得る。";
#else
		if (name) return "Resist Chaos";
		if (desc) return "Grants temporary resistance to chaos.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
		}
		break;





	case 22:
#ifdef JP
		if (name) return "*鑑識*";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Holy Vision";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;


	case 23:
#ifdef JP
		if (name) return "精神防壁";
		if (desc) return "一定時間、恐怖と狂気への耐性、魔法防御上昇を得る。";
#else
		if (name) return "Mental Barrier";
		if (desc) return "Granys temporary resistance to fear and insanity and raises protection from magic.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_afraid(0);
				set_stun(0);
				set_hero(time,FALSE);
				set_resist_magic(time, FALSE);
				set_tim_res_insanity(time, FALSE);

			}
		}
		break;





	case 24:
#ifdef JP
		if (name) return "リアクティブヒール";
		if (desc) return "一定時間、ダメージを受けたとき自動的に少量のHPが回復する。回復時に効果時間が少し減る。";
#else
		if (name) return "Reactive Healing";
		if (desc) return "For a while, you while automatically recover a bit of HP upon receiving damage. Healing in this manner decreases this effect's duration a bit.";
#endif

		{
			int base = 25 + plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_reactive_heal(randint1(base) + base, FALSE);
			}
		}
		break;


	case 25:
#ifdef JP
		if (name) return "アンデッド消滅";
		if (desc) return "自分の周囲にいるアンデッドを現在の階から消し去る。抵抗されると無効。";
#else
		if (name) return "Annihilate Undead";
		if (desc) return "Eliminates all nearby undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif

		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide_undead(power, TRUE);
			}
		}
		break;





	case 26:
#ifdef JP
		if (name) return "全復活";
		if (desc) return "すべてのステータスと経験値を回復する。";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif

		{
			if (cast)
			{
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				restore_level();
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "ライフストリーム";
		if (desc) return "視界内の配下モンスターのHPを大幅に回復させる。";
#else
		if (name) return "Lifestream";
		if (desc) return "Greatly heals all your followers in sight.";
#endif

		{
			if (cast)
			{
				int i,heal,x,y;
				bool count = FALSE;
				msg_format(_("生命のエネルギーが辺りに満ち溢れた！", "Life energy surges around you!"));
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr;
					char m_name[80];
					r_ptr = &r_info[m_ptr->r_idx];
					if (!m_ptr->r_idx) continue;
					y = m_ptr->fy;
					x = m_ptr->fx;
					if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
					if(!is_pet(m_ptr)) continue;
					if(!monster_living(r_ptr))continue;
					monster_desc(m_name, m_ptr, 0);
					r_ptr = &r_info[m_ptr->r_idx];
					heal = m_ptr->maxhp / 5;
					if(heal < 500) heal = 500;
					count = TRUE;
					m_ptr->hp += heal;
					if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
					set_monster_stunned(cave[y][x].m_idx,0);
					msg_format(_("%sは体力を回復した！", "%s is healed!"),m_name);
				}
				if(!count) msg_format(_("しかし傷を治すべき配下が近くにいなかった・・",
                                        "However, there were no nearby followers in need of healing..."));

			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "真・結界";
		if (desc) return "自分のいる床と周囲8マスの床の上に、モンスターが通り抜けたり召喚されたりすることができなくなるルーンを描く。";
#else
		if (name) return "Warding True";
		if (desc) return "Creates glyphs in all adjacent squares and under you.";
#endif

		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				warding_glyph();
				glyph_creation();
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "*体力回復*";
		if (desc) return "最強の治癒の魔法で、負傷と朦朧状態も全快する。";
#else
		if (name) return "Healing True";
		if (desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif

		{
			int heal = 2000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;


	case 30:
#ifdef JP
		if (name) return "新生";
		if (desc) return "全ての突然変異を治し、HP最大値を再計算する。";
#else
		if (name) return "New Life";
		if (desc) return "Cures all mutation and rerolls your maximum HP.";
#endif

		{
			if (cast)
			{
			do_cmd_rerate(FALSE);
			get_max_stats();
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			set_alcohol(0);
			set_asthma(0);

			if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print(_("全ての突然変異が治った。", "You are cured of all mutations."));

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
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "究極の耐性";
		if (desc) return "一定時間、あらゆる耐性を付け、ACと魔法防御能力を上昇させる。";
#else
		if (name) return "Ultimate Resistance";
		if (desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int v = randint1(base) + base;
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
		}
		break;
	}

	return "";
}







/*:::新領域　変容*/
static cptr do_new_spell_transform(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	int dice,sides,base,damage;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
		if (name) return _("発光", "Radiate Light");
		if (desc) return _("今いる部屋を明るく照らす。", "Lights up nearby area.");

		{
			if (cast)
			{
				switch(randint1(100))
				{
				case 1:	msg_print(_("あなたの両目は七色に輝いた！", "Your eyes glow seven colors!"));break;
				case 2:	msg_print(_("あなたの額はまばゆく光った！", "Your forehead shines brightly!"));break;
				case 3:	msg_print(_("あなたのうなじは眩しくぎらついた！", "You shine with dazzling light!"));break;
				case 4:	msg_print(_("あなたのこの手が光って唸る！", "Your hands shine!"));break;
				case 5:	msg_print(_("あなたの髪は妖しくイルミネーションした！", "Your hair gives off bewitching illumination!"));break;
				case 6:	msg_print(_("あなたのお尻が蛍のように光った！", "Your backside glows like that of a firefly!"));break;
				case 7:	msg_print(_("あなたの右鎖骨が煌めいた！", "Your right collarbone glitters!"));break;
				case 8:	msg_print(_("あなたの鼻が真っ赤にピカピカ光って暗い夜道を照らした！", "Your nose glows bright red!"));break;
				default:	msg_print(_("あなたの体は光り始めた。", "You body starts to glow."));break;
				}
				lite_area(0, 0);


			}
		}
		break;
	case 1:
#ifdef JP
		if (name) return "筋肉増強";
		if (desc) return "一定時間、腕力を4ポイント増加させる。増加後の値は40を超えない。";
#else
		if (name) return "Grow Muscles";
		if (desc) return "Temporarily raises strength by 4 points. Cannot exceed the limit of 40.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format(_("筋肉が膨張した。", "Your muscles grow in size."));
				set_tim_addstat(A_STR,4,base + randint1(base),FALSE);
			}
		}
		break;



	case 2:
#ifdef JP
		if (name) return "獣の耳";
		if (desc) return "近くのモンスターを感知する。レベルが上がると感知範囲が広がる。";
#else
		if (name) return "Beast Ears";
		if (desc) return "Detects nearby monsters. Detection radius increases with level.";
#endif

		{
			int rad = plev / 2 + 10;

			if (info) return format(_("範囲：%d", "rad: %d"),rad);

			if (cast)
			{
				if(one_in_(10))
					msg_print(_("あなたの頭に猫耳が生えてピクピク動いた。", "Cat ears pop out of your head and make jerky movements."));
				else
					msg_print(_("あなたは聴覚を強化し耳を澄ませた・・", "You augment your hearing and listen..."));
				(void)detect_monsters_normal(rad);
				(void)detect_monsters_invis(rad);
			}
		}
		break;


	case 3:
		if (name) return _("強力投擲", "Powerful Throw");
		if (desc) return _("アイテムを強力に投げつける。命中率は通常の投擲と同様に計算される。",
                            "Throws an item powerfully. Accuracy is calculated just like ordinary throwing.");

		{
			int mult = 2 + plev / 15;

			if (info) return format(_("倍率：%d", "mult: %d"),mult);

			if (cast)
			{

				msg_print(_("腕と肩の筋肉が盛り上がった！", "You arm and shoulder muscles grow in power!"));
				if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;

			}
		}
		break;
	case 4:
#ifdef JP
		if (name) return "皮膚変容";
		if (desc) return "一時的に火に対する耐性を得る。レベルが上昇すると酸と冷気に対する耐性も得られる。";
#else
		if (name) return "Transform Skin";
		if (desc) return "Grants temporary resistance to fire. On higher levels, grants resistance to acid and cold as well.";
#endif

		{
			int base = 15 + plev / 3;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				msg_print(_("皮膚組織を変性させた・・", "You change the composition of your skin tissue..."));
				set_oppose_fire(randint1(base) + base, FALSE);
				if(plev > 19) set_oppose_acid(randint1(base) + base, FALSE);
				if(plev > 29) set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "髪の毛針";
		if (desc) return "ターゲット周辺に向け、多数の低威力なボルトを放つ。";
#else
		if (name) return "Needle Hair";
		if (desc) return "Fires multiple low power bolts around the target.";
#endif

		{

			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_ARROW, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "翼生成";
		if (desc) return "背から翼を生やし、一定時間飛行する。";
#else
		if (name) return "Grow Wings";
		if (desc) return "Grows wings from your back, giving you temporary levitation.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("背から翼が生えた！", "Wings sprout from your back!"));
				set_tim_levitation(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
		if (name) return _("石油噴出", "Oil Eruption");
		if (desc) return _("ターゲット周辺の地形を「石油」にする。また範囲内のモンスターの攻撃力を低下させる。石油地形上では火炎系統のダメージが上昇する。",
                            "Turns terrain around the target into oil, and lowers attack power of monsters in that area. Fire attacks deal more damage to targets on oil terrain.");
		{
			int dam = plev * 3;
			int rad = 1 + plev / 20;

			if (cp_ptr->magicmaster) dam *= 2;
			if (info) return info_power(dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if (!fire_ball_jump(GF_DIG_OIL, dir, dam, rad, _("石油が噴き出した！", "Oil erupts from the ground!"))) return NULL;
			}
		}
		break;
		//v1.1.94 足かせ→石油噴出

	    /*
#ifdef JP
		if (name) return "足かせ";
		if (desc) return "指定ターゲットを減速させる。抵抗されると無効。飛んでいる敵など一部の敵には効果が薄い。";
#else
		if (name) return "Entangle";
		if (desc) return "Slows target monster. Does nothing if resisted. Less effective against some enemies, like flying ones.";
#endif

		{
			int power = plev * 3 ;
			int rad = 0 + plev / 20;

			if (info) return info_power(power);

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				msg_print(_("地面が揺れ動きうねった。", "The ground starts the shake and swell"));
				fire_ball_hide(GF_SLOW_TWINE, dir, power, rad);
			}

		}
		break;
		*/

	case 8:
#ifdef JP
		if (name) return "循環器変容";

		if (desc) return "一定時間回復力が強化され、さらに毒に対する耐性を得る。";
#else
		if (name) return "Transform Circulatory Organs";
		if (desc) return "Grants temporary regeneration and resistance to poison.";
#endif

		{
			int base = 10 + plev / 2;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				int time = base + randint1(base);
				set_tim_regen(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
		}
		break;
	case 9:
#ifdef JP
		if (name) return "目からビーム";
		if (desc) return "目からビームを放つ。ビームの属性は種族により変わる。";
#else
		if (name) return "Eye Beam";
		if (desc) return "Fires a beam from your eyes. Beam element depends on your race.";
#endif

		{

			int dice = 10 + plev  / 5;
			int sides = 7;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int typ;
				switch(p_ptr->prace)
				{
				case RACE_VAMPIRE:
				case RACE_KAPPA:
				case RACE_NINGYO:
					typ = GF_WATER;break;
				case RACE_YOUKAI:
				case RACE_DAIYOUKAI:
				case RACE_IMP:
					typ = GF_DARK;break;
				case RACE_MAGICIAN:
				case RACE_DEITY:
					typ = GF_MANA;break;
				case RACE_DEATH:
				case RACE_ZOMBIE:
				case RACE_SPECTRE:
				case RACE_LICH:
				case RACE_ANIMAL_GHOST:
					typ = GF_NETHER;break;
				case RACE_ULTIMATE:
				case RACE_FAIRY:
					typ = GF_HOLY_FIRE;break;
				case RACE_NINJA:
				case RACE_ONI:
					typ = GF_FORCE;break;
				case RACE_HOURAI:
					typ = GF_NUKE;break;
				default:
					typ = GF_LITE;break;

				}

				if (!get_aim_dir(&dir)) return NULL;
				 msg_print(_("目からビームを放った！", "You fire a beam from your eyes!"));
				fire_beam(typ, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "ドア生成";
		if (desc) return "自分の周りの床を閉じたドアに変える。";
#else
		if (name) return "Door Creation";
		if (desc) return "Creates closed doors around you.";
#endif

		{
			int base = 20;
			//レベルで半径増えるようにしようとしたがちょっと便利すぎるので自重
			//int rad = 1 + plev / 20;

			if (cast)
			{
				door_creation(1);
			}
		}
		break;
	case 11:
#ifdef JP
		if (name) return "結合再生";
		if (desc) return "傷を無理矢理塞いで治療する。HPを大幅に回復し切り傷を治すが満腹度が減少する。";
#else
		if (name) return "Forceful Regeneration";
		if (desc) return "Heals by forcefully closing wounds. Greatly recovers HP and cures cuts, but reduces satiation.";
#endif

		{
			int heal = plev * 4;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				if(p_ptr->chp < p_ptr->mhp) msg_print(_("体の傷を強引に塞いでくっつけた！", "You forcefully close the wounds on your body!"));
				hp_player(heal);
				set_cut(0);
				set_food(p_ptr->food - 1000);
			}
		}
		break;



	case 12:
#ifdef JP
		if (name) return "爪強化";
		if (desc) return "両手の爪を巨大な刃に変化させる。ACが30上昇し強力な格闘攻撃ができるようになるが魔法の失敗率が上昇する。また右腕装備、左腕装備、グローブは一時的に無効化される。";
#else
		if (name) return "Augment Nails";
		if (desc) return "Turns nails on both of your hands into huge blades. Raises AC by 30 and lets you perform strong barehanded attacks, but raises spell failure rate. Temporarily disables gloves and items you're holding in your arms.";
#endif

		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				if(p_ptr->riding && !CLASS_RIDING_BACKDANCE)
				{
					msg_print(_("馬上ではこの魔法を使えない。", "You cannot use this spell while mounted."));
					return NULL;
				}
				set_clawextend(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "ブレス";
		if (desc) return "現在HPの1/3の威力の元素ブレスを吐く。ブレスの属性は性格により変わる。";
#else
		if (name) return "Breath";
		if (desc) return "Breathes a basic element with power equal to 1/3 of your current HP. Element depends on your personality.";
#endif

		{
			//念のため実際の性格の数より多くしておく
			int gf_table[] = {GF_FIRE,GF_FIRE,GF_COLD,GF_ACID, GF_ELEC,GF_ACID,GF_FIRE,GF_POIS,GF_ELEC,GF_ACID, GF_COLD, GF_ACID, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE};
			int dam = p_ptr->chp / 3;
			int rad = -2 - plev / 30;

			if(rad < -3) rad = -3;
			if(dam > 1600) dam = 1600;

			if(dam<1) dam=1;
			if (info) return info_damage(0, 0, dam);


			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP


				if (gf_table[p_ptr->pseikaku] == GF_FIRE) msg_print("あなたは火炎のブレスを吐いた！");
				if (gf_table[p_ptr->pseikaku] == GF_COLD) msg_print("あなたは冷気のブレスを吐いた！");
				if (gf_table[p_ptr->pseikaku] == GF_ELEC) msg_print("あなたは電撃のブレスを吐いた！");
				if (gf_table[p_ptr->pseikaku] == GF_ACID) msg_print("あなたは酸のブレスを吐いた！");
				if (gf_table[p_ptr->pseikaku] == GF_POIS) msg_print("あなたはガスのブレスを吐いた！");
#else
				if (gf_table[p_ptr->pseikaku] == GF_FIRE) msg_print("You breathe fire!");
				if (gf_table[p_ptr->pseikaku] == GF_COLD) msg_print("You breathe cold!");
				if (gf_table[p_ptr->pseikaku] == GF_ELEC) msg_print("You breathe lightning!");
				if (gf_table[p_ptr->pseikaku] == GF_ACID) msg_print("You breathe acid!");
				if (gf_table[p_ptr->pseikaku] == GF_POIS) msg_print("You breathe poison!");
#endif

				fire_ball(gf_table[p_ptr->pseikaku], dir, dam, rad);
			break;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "階段生成";
		if (desc) return "今いる場所に階段を作る。";
#else
		if (name) return "Stair Creation";
		if (desc) return "Creates a staircase.";
#endif

		{
			if (cast)
			{
				stair_creation();
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "伸腕";
		if (desc) return "腕を伸ばして離れた場所の敵に攻撃する。攻撃回数は距離に応じ減少する。 河童と山童は射程距離にボーナスを得る。";
#else
		if (name) return "Extending Arm";
		if (desc) return "Extends your arm, attacking distant enemies. Amount of blows reduces with distance. Kappa and yamawaro can extend further.";
#endif

		{
			int length = 1 + p_ptr->lev / 11;

			if(p_ptr->prace == RACE_KAPPA || p_ptr->prace == RACE_YAMAWARO) length = 2 + p_ptr->lev / 7;

			if (info) return format(_("射程：%d", "len: %d"),length);
			if (cast)
			{
				project_length = length;
				if (!get_aim_dir(&dir)) return NULL;
				project_hook(GF_ATTACK, dir, HISSATSU_LONGARM, PROJECT_STOP | PROJECT_KILL);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "スプリングフロア";
		if (desc) return "指定した場所の地面を跳ね上げ、そこに乗っている敵を吹き飛ばし少しダメージを与える。一部の敵には効果がない。自分を吹き飛ばすこともできる。アイテムも吹き飛ぶが壊れることがある。";
#else
		if (name) return "Catapult Floor";
		if (desc) return "Makes floor at target location spring forth, catapulting any enemies standing there. Does not effect some enemies. You can catapult yourself as well. Items also can be launched in this manner, but they might break.";
#endif

		{
			int dist = 1 + plev / 7;

			if (info) return format(_("距離：%d", "dist: %d"),dist);
			if (cast)
			{
				if(!cast_spring_floor(dist)) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "電光石火";
		if (desc) return "一時的に加速する。";
#else
		if (name) return "Lightning Speed";
		if (desc) return "Temporarily increases speed.";
#endif

		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;
	case 18:
#ifdef JP
		if (name) return "スライム変化";
		if (desc) return "一時的にスライムに変身する。それまでの一時効果は全て解除される。酸免疫と多くの耐性を得て吸収攻撃が可能になるが、移動速度が低下し全ての装備が無効化され巻物、魔道具、魔法、ほとんどの特殊攻撃が使用不可能になる。Uコマンドから元に戻れる。";
#else
		if (name) return "Slime Form";
		if (desc) return "Temporarily transforms into a slime. Removes other temporary effects. Grants acid immunity, multiple resistances and draining attacks, but reduces movement speed, disables all equipment, and makes it impossible to use scrolls, magic devices, spell and most special attacks. Revert with 'U'.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base + randint1(base), MIMIC_SLIME, FALSE)) return NULL;
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "*筋肉増強*";
		if (desc) return "一定時間、腕力を大幅に増加させる。通常の最大値を超える。";
#else
		if (name) return "*Grow Muscles*";
		if (desc) return "Temporarily greatly increases your strength. Can exceed ordinary limits.";
#endif

		{
			int base = plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format(_("強大な力が体内を荒れ狂う気がする！", "You feel your body raging with great power!"));
				set_tim_addstat(A_STR,100 + plev/4 ,base + randint1(base),FALSE);

			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "肉体再構成";
		if (desc) return "HPと傷を完全に回復するがランダムな能力が恒久的に減少することがある。減少する確率はHP回復量と最大HPの比率により最高1/2まで上がる。";
#else
		if (name) return "Reshape Body";
		if (desc) return "Fully heals and cures cuts, but might permanently reduce a random stat. Chance of stat reduction is proportional to HP recovered and maxes out at 50%.";
#endif

		{

			if (cast)
			{
				int chance = 50 - p_ptr->chp * 50 / p_ptr->mhp ;
				if(p_ptr->chp == p_ptr->mhp)
				{
					msg_format(_("体を再構成する必要はない。", "You don't have to reshape your body."));
					return NULL;
				}

				msg_format(_("あなたは肉体を再構成し始めた・・", "You start reshaping your body..."));
				hp_player(5000);
				set_cut(0);
				/*:::oops */
				if(randint0(100) < chance)
				{
					int stat = randint0(6);
					switch(stat)
					{
					case A_STR:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少し足腰が弱った気がする・・", "You feel your legs weaken a bit..."));
						break;
					case A_INT:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少し脳の皺が減った気がする・・", "You feel your brain getting a bit smoother..."));
						break;
					case A_WIS:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少し心臓が小さくなった気がする・・", "You feel your heart shrinking a bit..."));
						break;
					case A_DEX:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少し関節が痛む気がする・・", "You feel your joints hurt a bit..."));
						break;
					case A_CON:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少し胃腸が弱くなった気がする・・", "You feel your stomach hurts a bit..."));
						break;
					case A_CHR:
						if(dec_stat(stat,20,TRUE)) msg_format(_("少しお肌が荒れた気がする・・", "You feel your skin grow a bit rougher..."));
						break;

					}

				}
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "*ブレス*";
		if (desc) return "現在HPの1/3の威力の上位属性ブレスを吐く。属性は種族により変わる。";
#else
		if (name) return "*Breath*";
		if (desc) return "Breathes a higher element with power equal to 1/3 of your current HP. Element depends on your race.";
#endif
		{
			int dam = p_ptr->chp / 3;
			int rad = -3;
			int typ;
			cptr name;
			if(dam > 500) dam = 500;//v1.1.49 1600から500に制限　モンスター変身してからこれ使うと上限に届くため

			if(dam<1) dam=1;
			if (info) return info_damage(0, 0, dam);


			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP

				switch(p_ptr->prace)
				{
				case RACE_HUMAN:
				case RACE_SENNIN:
				case RACE_KOBITO:
				case RACE_NINJA:
					typ = GF_FORCE;
					name = "フォースの";
					break;
				case RACE_YOUKAI:
				case RACE_HOFGOBLIN:
				case RACE_HALF_YOUKAI:
				case RACE_IMP:
				case RACE_VAMPIRE:
					typ = GF_DARK;
					name = "暗黒の";
					break;
				case RACE_FAIRY:
					typ = GF_HOLY_FIRE;
					name = "聖なる";
					break;
				case RACE_KAPPA:
				case RACE_NINGYO:
					typ = GF_WATER;
					name = "アクア";
					break;
				case RACE_KARASU_TENGU:
				case RACE_DEMIGOD:
					typ = GF_TORNADO;
					name = "竜巻の";
					break;
				case RACE_HAKUROU_TENGU:
				case RACE_ONI:
				case RACE_WARBEAST:
				case RACE_NYUDOU:
					typ = GF_SOUND;
					name = "衝撃波の";
					break;
				case RACE_DEATH:
				case RACE_ZOMBIE:
				case RACE_SPECTRE:
				case RACE_HALF_GHOST:
				case RACE_ANIMAL_GHOST:
					typ = GF_NETHER;
					name = "地獄の";
					break;
				case RACE_TSUKUMO:
				case RACE_YAMAWARO:
				case RACE_ULTIMATE:
					typ = GF_SHARDS;
					name = "破片の";
					break;

				case RACE_YOUKO:
				case RACE_BAKEDANUKI:
					typ = GF_CHAOS;
					name = "カオスの";
					break;
				case RACE_GOLEM:
					typ = GF_LITE;
					name = "閃光の";
					break;

				case RACE_DEITY:
				case RACE_DAIYOUKAI:
					typ = GF_DISINTEGRATE;
					name = "分解の";
					break;
				case RACE_ZASHIKIWARASHI:
					typ = GF_GRAVITY;
					name = "重力の";
					break;

				default:
					typ = GF_MANA;
					name = "魔力の";
					break;
				}

				msg_format("あなたは%sブレスを吐いた！",name);
#else
                switch(p_ptr->prace)
				{
				case RACE_HUMAN:
				case RACE_SENNIN:
				case RACE_KOBITO:
				case RACE_NINJA:
					typ = GF_FORCE;
					name = "force";
					break;
				case RACE_YOUKAI:
				case RACE_HOFGOBLIN:
				case RACE_HALF_YOUKAI:
				case RACE_IMP:
				case RACE_VAMPIRE:
					typ = GF_DARK;
					name = "darkness";
					break;
				case RACE_FAIRY:
					typ = GF_HOLY_FIRE;
					name = "holy energy";
					break;
				case RACE_KAPPA:
				case RACE_NINGYO:
					typ = GF_WATER;
					name = "water";
					break;
				case RACE_KARASU_TENGU:
				case RACE_DEMIGOD:
					typ = GF_TORNADO;
					name = "a tornado";
					break;
				case RACE_HAKUROU_TENGU:
				case RACE_ONI:
				case RACE_WARBEAST:
				case RACE_NYUDOU:
					typ = GF_SOUND;
					name = "a shockwave";
					break;
				case RACE_DEATH:
				case RACE_ZOMBIE:
				case RACE_SPECTRE:
				case RACE_HALF_GHOST:
				case RACE_ANIMAL_GHOST:
					typ = GF_NETHER;
					name = "nether";
					break;
				case RACE_TSUKUMO:
				case RACE_YAMAWARO:
				case RACE_ULTIMATE:
					typ = GF_SHARDS;
					name = "shards";
					break;

				case RACE_YOUKO:
				case RACE_BAKEDANUKI:
					typ = GF_CHAOS;
					name = "chaos";
					break;
				case RACE_GOLEM:
					typ = GF_LITE;
					name = "light";
					break;

				case RACE_DEITY:
				case RACE_DAIYOUKAI:
					typ = GF_DISINTEGRATE;
					name = "disintegration";
					break;
				case RACE_ZASHIKIWARASHI:
					typ = GF_GRAVITY;
					name = "gravity";
					break;

				default:
					typ = GF_MANA;
					name = "mana";
					break;
				}

				msg_format("You breathe %s!",name);
#endif

				fire_ball(typ, dir, dam, rad);
			break;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "獣変化";
		if (desc) return "一定時間、大型の獣に変身する。それまでの一時効果は全て解除される。変身中は加速と身体能力が大幅に上昇するが、外套とアクセサリ以外の武器防具が一時的に無効化され魔法・特技・魔道具・巻物が使えなくなる。";
#else
		if (name) return "Beast Form";
		if (desc) return "Temporarily transforms into a large beast. Removes all other temporary effects. Greatly increases speed and physical abilities, but disables equipment other than cloaks and accessories and makes you unable to use spells, special abilities or magic devices.";
#endif

		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base + randint1(base), MIMIC_BEAST, FALSE)) return NULL;
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();

			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "マグマ活性";
		if (desc) return "ターゲット周辺へ巨大な火炎のボールを発生させ、さらに床を溶岩にする。";
#else
		if (name) return "Magma Burst";
		if (desc) return "Fires a huge ball of fire close to the target and turns floor into lava.";
#endif

		{
			int dam = (100 + plev) * 2;
			int rad = 3 + plev / 45;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				//ここ、もしGF_FIREが先だとそれでターゲットが死んだときターゲットが無効になって＠の足元が溶岩になる
				if(!fire_ball_jump(GF_LAVA_FLOW, dir, 3 + randint1(2), rad,NULL)) return NULL;
				fire_ball_jump(GF_FIRE, dir, dam, rad,NULL);
			}
		}
		break;
	case 24:
		if (name) return _("魔理沙変化", "Marisa Form");
		if (desc) return _("化けやすいと評判の幻想郷の有名人に変身する。変身中は種族特性が人間とほぼ同じになり、移動能力がやや向上し、レイシャルとしていくつかの特殊魔法を使える。特殊魔法のコストはやや高めで威力も本物より落ちる。",
                            "Transforms into that famous easy-to-imitate resident of Gensoukyou. Your race effectively changes to human, your movement improves to some degree and you can use several special spells as your racial ability. Cost of special spells is fairly high, and their power is lower than that of original's.");
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if(p_ptr->pclass == CLASS_MARISA)
				{
					msg_print(_("あなたはすでに魔理沙だ。", "You are already Marisa."));
					return NULL;
				}
				if(!set_mimic(base+randint1(base), MIMIC_MARISA, FALSE)) return NULL;

			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "レーダーセンス";
		if (desc) return "一定時間、周辺の地形・アイテム・トラップ・モンスターを全て感知し続ける。";
#else
		if (name) return "Radar Sense";
		if (desc) return "Temporarily keeps detecting surrounding area layout, items, traps and monsters.";
#endif

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_radar_sense(randint1(base) + base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "地面泳法";
		if (desc) return "一定時間、地面や壁の中を水のように泳ぐことができるようになる。";
#else
		if (name) return "Swim through Terrain";
		if (desc) return "Temporarily lets you swim through the ground and walls like through water.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "霧変化";
		if (desc) return "一定時間、霧に変身する。種族特性は一時的に上書きされる。物理攻撃とボルトが効きにくくなり、切り傷・落石を受け付けなくなり、隠密能力が大幅に上昇する。しかし身体能力が大幅に低下し元素と閃光と劣化の攻撃に弱くなってしまう。装備品は無効化されず、魔法やアイテムは従来通り使用可能。騎乗は不可。";
#else
		if (name) return "Mist Form";
		if (desc) return "Temporarily transforms into mist. Overwrites current racial abilities. Physical attacks and bolts are less effective against you, you can't be cut, and your stealth is greatly increased. However, your physical abilities greatly decrease and you become vulnerable to basic elements, light and disenchantment. Your equipment doesn't get disabled, and you still can use spells and items. You cannot ride in this form.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if(!set_mimic(base+randint1(base), MIMIC_MIST, FALSE)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "肉体変容";
		if (desc) return "ランダムに肉体に関する突然変異を得る。";
#else
		if (name) return "Physical Alteration";
		if (desc) return "Gives you a random body-related mutation.";
#endif

		{
			//gain_random_mutation()で変異決定するための数値のリスト
			int muta_lis[] = {1,5,8,17,19,24,31,35,38,46,62,65,81,83,85,95,98,100,109,120,123,126,129,136,138,141,143,146,154,157,161,163,165,168,171,173,182,185};

			int attempt = 10;

			if (cast)
			{
				while(( attempt-- > 0) && !gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int))-1]));
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "生体ミサイル";
		if (desc) return "肉体の一部を爆発物に再構成して射出し、破片属性の攻撃を行う。強力だがHPを200～300消費する。";
#else
		if (name) return "Organic Missile";
		if (desc) return "Reshapes a part of your body into an explosive missile and fires it. Deals shards damage. Powerful, but costs 200-300 HP.";
#endif

		{
			int dam = 800;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(p_ptr->chp < 300 && !get_check_strict(_("体力が少なすぎる。本当に使いますか？",
                                                            "Your health is too low. Are you really sure?"), CHECK_OKAY_CANCEL)) return NULL;

#ifdef JP
				msg_print("あなたは生体ミサイルを発射した！");
#else
				msg_print("You launch an organic missile!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 200+randint0(101), "無謀な肉体変換", -1);
#else
				take_hit(DAMAGE_USELIFE, 200+randint0(101), "reckless body transformation", -1);
#endif
			}
		}
		break;
	case 30:
#ifdef JP
		if (name) return "ドラゴン変化";
		if (desc) return "一定時間、巨大なドラゴンに変身する。それまでの一時効果は全て解除される。変身中は身体能力が爆発的に上昇しブレスを吐けるようになるが、リボン以外の装備品が無効化され魔法・特技・魔道具・巻物・薬が一切使えなくなる。変身時のレベル・知能・賢さにより耐性と能力値が変化する。";
#else
		if (name) return "Dragon Form";
		if (desc) return "Temporarily transforms into a giant dragon. Removes all other temporary effects. Massively increases physical abilities and gives you breath attack, but disables all equipment other than ribbons and makes you unable to use spells, special abilities, magic devices, scrolls or potions. Resistances and stat changes depend on your level, intelligence and wisdom at the point of transformation.";
#endif

		{

			int base = 50;

			if (info) return format(_("期間：50ターン", "dur: 50 turns"));

			if (cast)
			{
				int percentage;
				p_ptr->mimic_dragon_rank = p_ptr->lev + p_ptr->stat_ind[A_INT] + 3 + p_ptr->stat_ind[A_WIS] + 3;
				if(p_ptr->pclass == CLASS_HIGH_MAGE) p_ptr->mimic_dragon_rank+= 20;

				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base, MIMIC_DRAGON, FALSE)) return NULL;

				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();

			}
		}
		break;

	case 31:
		if (name) return _("転生", "Rebirth");
		if (desc) return _("三つのランダムな種族候補から一つ選択し、その種族へと変化する。全ての突然変異を治し、HPと能力値と経験値は再計算後に全回復する。現在経験値の1～5%が永久に失われる。一部の種族は使用不可。",
                            "Changes you race into one out of 3 randomly chosen ones. Cures all mutations and fully heals after recalculations. Permanently reduces curren total experience by 1-5%. Cannot be used by some races.");

		{
			if (cast)
			{
				int new_race;
				int choices[3];
				int i;
				char c;
				bool flag_untimate;

				if((RACE_NEVER_CHANGE) || p_ptr->prace == RACE_ULTIMATE)
				{
					msg_format(_("あなたの存在は通常の輪廻から外れているようだ。", "You exist beyond the circle of rebirth."));
					return NULL;
				}
				if (!get_check(_("別の種族へと転生します。よろしいですか？", "You will be reborn as a different race. Are you sure?"))) return NULL;

				/*:::転生先種族をランダムに選定する。*/
				///mod150406 除外設定に野良神様追加
				//v1.1.79 新種族「座敷わらし」になることはできない
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special);
				choices[0] = new_race;
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special || (new_race == choices[0]) );
				choices[1] = new_race;
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special || (new_race == choices[0])  || (new_race == choices[1]));
				choices[2] = new_race;

				/*:::特殊処理　石仮面を装備した吸血鬼は究極生命体になれる（原作は吸血鬼じゃなくて柱の男だが）*/
				//それとも石仮面+赤石で全種族対象にする？
				if(p_ptr->prace == RACE_VAMPIRE && inventory[INVEN_HEAD].name1 == ART_STONEMASK)
				{
					choices[2] = RACE_ULTIMATE;
				}

				screen_save();
				msg_print(_("転生したい種族を記号で選択してください。", "Choose race you want to be reborn as."));
				Term_erase(12, 18, 255);
				Term_erase(12, 17, 255);
				Term_erase(12, 16, 255);
				Term_erase(12, 15, 255);
				Term_erase(12, 14, 255);
				for(i=0;i<3;i++) put_str(format("%c) %s",'a'+i, race_info[choices[i]].title) , 15+i , 40);

				while(1)
				{
					c = inkey();
					if(c >= 'a' && c <= 'c' &&(choices[c-'a']==RACE_TENNIN)
						&& !get_check_strict(_("天人になると二度と変容の妖術が使えません。よろしいですか？",
                                                "If you become a celestial, you won't be able to use Sorcery Arts of Transformation anymore. Are you sure?"), CHECK_OKAY_CANCEL)) continue;

					if(c >= 'a' && c <= 'c') break;
				}
				screen_load();
				//経験値減少
				p_ptr->max_exp -= (p_ptr->max_exp / 100 * randint1(5));
				p_ptr->exp = p_ptr->max_exp;
				//種族変更　変異はすべて消え、永続変異が再設定される
				change_race(choices[c-'a'],"");

				hp_player(5000);
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

			}
		}
		break;
	}

	return "";
}






static cptr do_new_spell_darkness(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "損傷:";
#else
	static const char s_dam[] = "dam: ";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "マジック・ミサイル";
		if (desc) return "弱い魔法の矢を放つ。";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif

		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "殺気感知";
		if (desc) return "近くの思考することができるモンスターを感知する。";
#else
		if (name) return "Detect Ferocity";
		if (desc) return "Detects all monsters except mindless in your vicinity.";
#endif

		if (cast)
		{
			detect_monsters_mind(DETECT_RAD_DEFAULT);
		}
		break;

	case 2:
#ifdef JP
		if (name) return "悪魔の祝福";
		if (desc) return "一定時間邪なる祝福を受け、命中率とACと魔法防御にボーナスを得る。";
#else
		if (name) return "Demon's Blessing";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif

		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "悪臭雲";
		if (desc) return "毒の球を放つ。";
#else
		if (name) return "Stinking Cloud";
		if (desc) return "Fires a ball of poison.";
#endif

		{
			int dam = 10 + plev / 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "耐暗黒";
		if (desc) return "一定時間、暗黒への耐性を得る。";
#else
		if (name) return "Resist Darkness";
		if (desc) return "Gives resistance to darkness for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_dark(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:

#ifdef JP
		if (name) return "暗闇";
		if (desc) return "今いる部屋を暗くする。";
#else
		if (name) return "Darkness";
		if (desc) return "Creates darkness in nearby area.";
#endif

		{

			if(info) return format("");
			if (cast)
			{
				unlite_room(py,px);
			}
		}
		break;


	case 6:
#ifdef JP
		if (name) return "暗黒の矢";
		if (desc) return "暗黒のボルトを放つ。";
#else
		if (name) return "Dark Bolt";
		if (desc) return "Fires a bolt or beam of darkness.";
#endif

		{
			int dice = 4 + plev / 5;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "略式契約";
		if (desc) return "隣接したデーモン1体を配下にしようと試みる。契約の成立不成立に関わらず敵レベル*2のダメージを受ける。ユニークには効かない。";
#else
		if (name) return "Brief Contract";
		if (desc) return "Attempts to make an adjacent demon your follower. Regardless of success, you take (enemy level) * 2 damage. Does not work against unique monsters.";
#endif

		{
			if (cast)
			{
				int y, x;
				monster_race *r_ptr;
				monster_type *m_ptr;
				char m_name[80];
				if (!get_rep_dir2(&dir)) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!cave[y][x].m_idx)
				{
					msg_print(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
				m_ptr = &m_list[cave[y][x].m_idx];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(!(r_ptr->flags3 & RF3_DEMON))
				{
					msg_format(_("%sは契約を理解できないようだ。", "Doesn't look like %s understands the contract."),m_name);
				}
				else
				{
					msg_format(_("あなたは%sに契約を持ちかけた・・", "You present your contract to %s..."),m_name);
					take_hit(DAMAGE_USELIFE,(r_ptr->level)*2,_("無謀な悪魔契約", "reckless demonology"),-1);
					control_one_demon(dir, 50 + plev * 5);
				}
			}
		}
		break;


	case 8:
#ifdef JP
		if (name) return "耐毒";
		if (desc) return "一定時間、毒への耐性を得る。装備による耐性に累積する。";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;


	case 9:
#ifdef JP
		if (name) return "鈴蘭の毒";
		if (desc) return "装備中の武器に対し一時的に毒の属性を付与する。";
#else
		if (name) return "Suzuran Poison";
		if (desc) return "Temporarily brands a wielded weapon with poison.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)&& !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
				{
#ifdef JP
					msg_format("毒を塗れる武器を持っていない。");
#else
		msg_format("You don't have a weapon to coat with poison.");
#endif
					return FALSE;
				}
				set_ele_attack(ATTACK_POIS, base + randint1(base));

			}
		}
		break;

		break;

	case 10:
#ifdef JP
		if (name) return "追放";
		if (desc) return "指定したモンスター一体を現在の階から追放する。抵抗されると無効。";
#else
		if (name) return "Exile";
		if (desc) return "Exiles target monster from current floor. Does nothing if resisted.";
#endif


		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;


	case 11:
#ifdef JP
		if (name) return "暗黒レーザー";
		if (desc) return "暗黒属性のビームを放つ。";
#else
		if (name) return "Darkness Laser";
		if (desc) return "Fires a beam of darkness.";
#endif

		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "悪魔召喚";
		if (desc) return "悪魔1体を召喚して配下にする。召喚された悪魔が敵対することもあり、敵対する確率は賢さと魅力による。";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif

		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;

				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(30)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, plev+randint1(plev/2), SUMMON_DEMON, mode))
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("「ご用でございますか、ご主人様」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「卑しき者よ、我は汝の下僕にあらず！ お前の魂を頂くぞ！」");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("悪魔は現れなかった。");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;

		//v1.1.94 毒針作成→硝子の盾...にしようかと思ったがやっぱやめた
		//防御デバフの「鎧粉砕」とかそんなのにする
    case 13:
		if (name) return _("鎧粉砕", "Smash Armor");
		if (desc) return _("モンスター一体の防御力(AC)を低下させる。抵抗されると無効。",
                            "Lowers a monster's defensive power (AC). Does nothing if resisted.");
		{

			int base = plev * 3;

			if (cp_ptr->magicmaster) base += plev * 2;

			if (info) return info_string_dice(_("効力:", "pow: "),1, base, base);

			if (cast)
			{


				project_length = 1 + p_ptr->lev / 10;
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_DEC_DEF, dir, base+randint1(base), 0);
			}
		}
		break;
		/*
	case 13:
		{
			int dam = plev * 4;

			if (cp_ptr->magicmaster)
				dam = plev * 6;

			if (name) return _("硝子の盾", "Glass Shield");
			if (desc) return _("接触型の隣接攻撃を防ぐ盾を作り出す。一度攻撃を防ぐと盾は破壊され、そのとき強力な破片属性の反撃を行う。",
                            "Creates a shield that protects against contact melee attacks. It breaks upon receiving an attack, countering with a powerful shards attack.");

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print(_("透き通った巨大な盾が現れた。", "A huge transparent shield appears."));
				p_ptr->special_defense |= (SD_GLASS_SHIELD);
				p_ptr->redraw |= (PR_STATUS);
			}

		}
		break;

		*/

        /*
	case 13:
#ifdef JP
		if (name) return "毒針作成";
		if (desc) return "敵を一撃で倒すことがある武器「毒針」を作成する。材料として「ゲルセミウム・エレガンス」を一つ消費する。";
#else
		if (name) return "Create Poison Needle";
		if (desc) return "Creates Poison Needle, a weapon capable of defeating enemies in a single blow. Requires Gelsemium Elegance as material.";
#endif

		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_gelsemium;

				q = _("どの触媒を使いますか? ", "Use which catalyst?");
				s = _("あなたは触媒となる毒を持っていない。", "You don't have the appropriate poison.");

				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

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

				msg_print(_("あなたは猛毒の葉を慎重に毒針に仕上げた・・",
                            "You carefully make a poison needle using the toxic leaves..."));
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_KNIFE,SV_WEAPON_DOKUBARI));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				(void)drop_near(q_ptr, -1, py, px);


			}
		}
		break;

        */

	case 14:
#ifdef JP
		if (name) return "吸血";
		if (desc) return "モンスター1体から生命力を吸い取って栄養にする。";
#else
		if (name) return "Vampiric Drain";
		if (desc) return "Absorbs some HP from a monster and gives them to you. You will also gain nutritional sustenance from this.";
#endif

		{
			int dice = 1;
			int sides = plev * 2;
			int base = plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice, sides);

				if (!get_aim_dir(&dir)) return NULL;

				if (drain_life(dir, dam))
				{
	///del131214 virtue
					//chg_virtue(V_SACRIFICE, -1);
					//chg_virtue(V_VITALITY, -1);

					hp_player(dam);
					if(one_in_(10)) set_deity_bias(DBIAS_COSMOS, -1);
					/*
					 * Gain nutritional sustenance:
					 * 150/hp drained
					 *
					 * A Food ration gives 5000
					 * food points (by contrast)
					 * Don't ever get more than
					 * "Full" this way But if we
					 * ARE Gorged, it won't cure
					 * us
					 */
					dam = p_ptr->food + MIN(5000, 100 * dam);

					/* Not gorged already */
					if (p_ptr->food < PY_FOOD_MAX)
						set_food(dam >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dam);
				}
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "抹殺";
		if (desc) return "フロア全体の指定した文字のモンスターを現在の階から追放する。抵抗されると無効。";
#else
		if (name) return "Massacre";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif

		{
			int power = plev+50;

			if (info) return info_power(power);

			if (cast)
			{
				symbol_genocide(power, TRUE,0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "ナイトメア";
		if (desc) return "近くの敵一体に対し精神攻撃を行う。通常の精神を持たない敵には効果が薄い。眠っている敵には効果が高く、敵を起こしにくい。";
#else
		if (name) return "Nightmare";
		if (desc) return "Performs mental attack against a nearby enemy. Less effective against enemies with unusual mind. Works well against sleeping enemies and isn't likely to wake them up.";
#endif

		{
			int dam = plev * 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				project_length = 1 + p_ptr->lev / 15;
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_NIGHTMARE, dir, dam, 0);
			}
		}
		break;
	case 17:

#ifdef JP
		if (name) return "狂乱";
		if (desc) return "狂戦士化し、恐怖を除去し、加速する。";
#else
		if (name) return "Battle Frenzy";
		if (desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif

		{
			int b_base = 25;
			int sp_base = plev / 2;
			int sp_sides = 20 + plev / 2;

			if (info) return info_duration(b_base, b_base);

			if (cast)
			{
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "毒素の嵐";
		if (desc) return "巨大な毒の球を放つ。";
#else
		if (name) return "Toxic Storm";
		if (desc) return "Fires a huge ball of poison.";
#endif

		{
			int dam = plev * 3 + 100;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}

		break;


	case 19:
#ifdef JP
		if (name) return "ヘル・ランス";
		if (desc) return "ヘルファイア属性のビームを放つ。あらゆる敵に効果があり、天使などにはさらに大きなダメージを与える。";
#else
		if (name) return "Hell Lance";
		if (desc) return "Fires a beam of hellfire. Effective against any enemies, and deals even more damage to beings like angels.";
#endif

		{
			int dam = plev * 3 ;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_beam(GF_HELL_FIRE,dir, dam);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "悪魔変化";
		if (desc) return "一定時間、悪魔に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔としての能力を得る。";
#else
		if (name) return "Polymorph Demon";
		if (desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif

		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "暗黒の嵐";
		if (desc) return "巨大な暗黒の球を放つ。";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif

		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DARK, dir, dam, rad);
			}
		}
		break;
	case 22:
#ifdef JP
		if (name) return "真・吸血";
		if (desc) return "モンスター1体から生命力を吸いとる。吸いとった生命力によって体力が回復する。";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif

		{
			int dam = 100;

			if (info) return format("%s3*%d", s_dam, dam);

			if (cast)
			{
				int i;

				if (!get_aim_dir(&dir)) return NULL;


				for (i = 0; i < 3; i++)
				{
					if (drain_life(dir, dam))
					{
						if(!i && one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);
						hp_player(dam);
					}
				}
			}
		}
		break;


	case 23:
#ifdef JP
		if (name) return "闇との融和";
		if (desc) return "一定時間暗闇と同化する。隠密能力が上昇し、光源が消え、暗闇でも物を見通せ、直接攻撃時に不意打ちで大ダメージを与え、暗黒攻撃に対する免疫を得るようになる。ただし破邪と閃光に極めて弱くなる。";
#else
		if (name) return "At One with Darkness";
		if (desc) return "You temporarily become one with darkness. Your stealth is increased, you do not produce light and are capable of seeing in darkness, you deal major damage to unaware enemies in melee and are completely immune to darkness. However, you become very vulnerable to light and holy attacks.";
#endif

		{
			int base = 15;
			int sides = plev / 3;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_unite_darkness(base + randint1(sides),FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "影のゲート";
		if (desc) return "位置を指定してテレポートする。明るく照らされた場所や障害物のある場所には出られない。指定した位置に出られなかったときテレポートは行われない。";
#else
		if (name) return "Shadow Gate";
		if (desc) return "Teleports to a specified position. Cannot teleport if that spot is illuminated or if there's an obstacle in the way. No teleportation occurs if you're unable to move to the specified position.";
#endif

		{
			int range = plev ;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("影の門が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a shadow gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_SHADOW)) return NULL;
			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "不毛化";
		if (desc) return "この階の増殖するモンスターが増殖できなくなる。";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif

		{
			if (cast)
			{
				msg_print(_("あなたの放った呪いが大地に満ちた・・", "You invoke a curse that spreads through the ground..."));

				num_repro += MAX_REPRO;
			}
		}
		break;


	case 26:
#ifdef JP
		if (name) return "魔剣";
		if (desc) return "一定時間、装備中の武器が秩序勢力の敵に大きなダメージを与えるようになる。レベルが上昇すると吸血効果も追加される。";
#else
		if (name) return "Demonblade";
		if (desc) return "Temporarily makes currently wielded weapon deal extra damage to lawful enemies. At higher levels makes it vampiric as well.";
#endif

		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)&& !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
				{
#ifdef JP
					msg_format("武器を持っていない。");
#else
		msg_format("You're not wielding a weapon.");
#endif
					return FALSE;
				}
				set_ele_attack(ATTACK_DARK, base + randint1(base));
			}
		}
		break;
	case 27:
#ifdef JP
		if (name) return "テンプテーション";
		if (desc) return "一定時間通常の限界を超えて魅力が上昇し、さらに詠唱時に視界内にいる敵全てを配下にしようと試みる。";
#else
		if (name) return "Temptation";
		if (desc) return "Temporarily raises your charisma beyond ordinary limit, and attempts to charm all enemies in sight.";
#endif

		{
			int base = 15 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{

				msg_print(_("あなたは魔性のカリスマに満ち溢れた！", "You are overflowing with bewitching charisma!"));
				set_tim_addstat(A_CHR,120,base + randint1(base),FALSE);
				charm_monsters(plev * 3);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "周辺抹殺";
		if (desc) return "自分の周囲にいるモンスターを現在の階から追放する。抵抗されると無効。";
#else
		if (name) return "Massacre Nearby";
		if (desc) return "Eliminates all nearby monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif

		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide(power, TRUE);
			}
		}
		break;



	case 29:
#ifdef JP
		if (name) return "上位悪魔召喚";
		if (desc) return "強力な悪魔を複数召喚して配下にする。召喚された悪魔が敵対することもあり、敵対する確率は賢さと魅力による。";
#else
		if (name) return "Summon Greater Demons";
		if (desc) return "Summons multiple strong demons as your followers. Summoned demons might be hostile; hostility rate depends on your wisdom and charisma.";
#endif
		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;
				bool summon = FALSE;
				int i;
				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(90)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;

				for(i=3+randint0(3);i>0;i--)
					if (summon_specific((pet ? -1 : 0), py, px, plev+randint1(plev), SUMMON_HI_DEMON, mode))summon = TRUE;

				if(summon)
				{
#ifdef JP
					msg_print("硫黄の悪臭が充満した。");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("「「「ご用でございますか、ご主人様」」」");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("「「「卑しき者よ、我々は汝の下僕にあらず！ お前の魂を頂くぞ！」」」");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("悪魔は現れなかった。");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;



	case 30:
#ifdef JP
		if (name) return "地獄の劫火";
		if (desc) return "ヘルファイア属性の強力なボールを放つ。あらゆる敵に効果があり、天使などにはさらに大きなダメージを与える。詠唱時に疲労で20+1d30のダメージを受ける。";
#else
		if (name) return "Hellfire";
		if (desc) return "Fires a powerful ball of evil power. Hurts good monsters greatly. Casting this spell strains you, dealing 20+1d30 damage.";
#endif

		{
			int dam = 666;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "地獄の劫火の呪文を唱えた疲労", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "魔王変化";
		if (desc) return "悪魔の王に変化する。変化している間は本来の種族の能力を失い、代わりに悪魔の王としての能力を得、壁を破壊しながら歩く。";
#else
		if (name) return "Polymorph Demonlord";
		if (desc) return "Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.";
#endif

		{
			int base = 15;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON_LORD, FALSE);
			}
		}
		break;
	}

	return "";
}





/*:::新領域　召喚*/
static cptr do_new_spell_summon(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

	u32b summon_mode = 0L;

#ifdef JP
	static const char s_random[] = "ランダム";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "蛇召喚";
		if (desc) return "蛇を配下として一匹召喚する。代償は不要。";
#else
		if (name) return "Summon Snake";
		if (desc) return "Summons a snake. Has no extra cost.";
#endif

		{
			int lev = plev;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);
			if (cast || fail)
			{
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;

				if (new_summoning(1, py, px, lev, SUMMON_SNAKE, summon_mode))
				{
					if(cast) msg_print(_("あなたは蛇を召喚した。", "You summon a snake."));
					else     msg_print(_("召喚された蛇があなたに噛みついてきた！", "The snake you summoned lashes out at you!"));
				}
			}
		}
		break;


	case 1:
#ifdef JP
		if (name) return "ショート・テレポート";
		if (desc) return "近距離のテレポートをする。";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif

		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;
	case 2:
#ifdef JP
		if (name) return "虫召喚";
		if (desc) return "虫を配下として一匹～複数召喚する。代償は不要。";
#else
		if (name) return "Summon Insects";
		if (desc) return "Summons one or several insects. Has no extra cost.";
#endif

		{
			int lev = 1 + plev / 2;
			int kind;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);
			if (cast || fail)
			{
				int num = 1 + plev / 10;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				if(plev > 19) summon_mode |= PM_ALLOW_GROUP;

				if(one_in_(3)) kind = SUMMON_ANT;
				else kind = SUMMON_SPIDER;

				if (new_summoning(num, py, px, lev, SUMMON_SPIDER, summon_mode))
				{
					if(cast) msg_print(_("あなたは虫を召喚した。", "You summon insects."));
					else     msg_print(_("召喚された虫があなたに飛びかかってきた！", "The summoned insects are flying at you!"));
				}
			}
		}
		break;


	case 3:
#ifdef JP
		if (name) return "サモンマテリアル";
		if (desc) return "ターゲットの頭上へ何かを落とす。";
#else
		if (name) return "Summon Material";
		if (desc) return "Drops something on the target's head.";
#endif

		{
			if (info) return format(_("損傷：不定", "dam: unkown"));
			if (cast)
			{
				cptr msg;
				int summon = randint1(plev);
				int typ,rad,dam;

				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay())
				{
					if(target_who == p_ptr->riding)
					{
						msg_print(_("それは危ないからやめておこう。", "That's dangerous, you shouldn't be doing it."));
						return NULL;
					}
					else if(target_who < 0)
					{
						msg_print(_("そこには何もいない。", "There's nobody here."));
						return NULL;
					}
					else summon_material(target_who);
					///mod150215 別ルーチンに分離独立した
				}
				else
				{
					msg_format(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}

			}


		}
		break;
	case 4:
#ifdef JP
		if (name) return "妖精召喚";
		if (desc) return "妖精を配下として一体～複数召喚する。代償としてお菓子を消費する。";
#else
		if (name) return "Summon Fairies";
		if (desc) return "Summons one or several fairies. Requires sweets as extra cost.";
#endif

		{
			int lev = plev * 2 / 3;
			if (info) return format(_("基本召喚レベル：%d", "base level: %d"),lev);
			if (cast || fail)
			{
				int num = 1 + plev / 20;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_tval = TV_SWEETS;
					if(!select_pay(&cost)) return NULL;
					if(cost > 100) lev += 20;
					if(cost > 1000) lev += 20;
				}
				if (new_summoning(num, py, px, lev, SUMMON_FAIRY, summon_mode))
				{
					if(cast) msg_print(_("あなたは妖精を召喚した。", "You summon fairies."));
					else     msg_print(_("妖精が現れたが、ひどく興奮しており制御不能だ！","Fairies appear, but they're too excited and you fail to control them!"));
				}
			}
		}
		break;


	case 5:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 4;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "動物召喚";
		if (desc) return "自然界の存在を配下として一体～複数召喚する。代償として食料を消費する。";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons one or more animals. Requires food ration as an extra cost.";
#endif

		{
			int lev = plev ;
			if (info) return format(_("基本召喚レベル：%d", "base level: %d"),lev);
			if (cast || fail)
			{
				int num = 1 + plev / 15;
				int cost;
				int kind;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_tval = TV_FOOD;
					if(!select_pay(&cost)) return NULL;
					if(cost > 20) lev += 10;
					if(cost > 200) lev += 10;
					if(cost > 2000) lev += 10;
				}

				if(plev > 40 && one_in_(4)) kind = SUMMON_HYDRA;
				else kind = SUMMON_ANIMAL;

				if (new_summoning(num, py, px, lev, kind, summon_mode))
				{
					if(cast) msg_print(_("あなたは動物を召喚した。", "You summon animals."));
					else     msg_print(_("動物たちはあなたをメインディッシュに決めたようだ！",
                                        "The animals decide to make you their main dish!"));
				}
			}
		}
		break;



	case 7:
#ifdef JP
		if (name) return "アイテムテレポート";
		if (desc) return "離れた場所にあるアイテムを自分の足元へ移動させる。";
#else
		if (name) return "Teleport Item";
		if (desc) return "Pulls a distant item close to you.";
#endif

		{
			int weight = plev * 30;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;
	case 8:
#ifdef JP
		if (name) return "テレポート・アウェイ";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;
	case 9:
#ifdef JP
		if (name) return "モンスター一体召喚";
		if (desc) return "ランダムなモンスターを配下として一体召喚する。代償は不要。";
#else
		if (name) return "Summon Monster";
		if (desc) return "Summons a random monster. Has no extra cost.";
#endif

		{
			int lev = plev;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);
			if (cast || fail)
			{
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, 0, summon_mode))
				{
					if(cast) msg_print(_("あなたはモンスターを召喚した。", "You summon a monster."));
					else     msg_print(_("召喚された者はあなたに敵対した！", "You summon a hostile monster!"));
				}
			}
		}
		break;


	case 10:
#ifdef JP
		if (name) return "テレポート・レベル";
		if (desc) return "瞬時に上か下の階にテレポートする。";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif

		{
			if (cast)
			{
#ifdef JP
				if (!get_check("本当に他の階にテレポートしますか？")) return NULL;
#else
				if (!get_check("Really teleport to another level?")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "悪魔召喚";
		if (desc) return "デーモンを配下として一体召喚する。代償として召喚レベルと同量のHPを消費する。召喚に失敗したときもこの代償は消費される。この召喚はプレーヤー勢力を無視する。";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon. Requires amount of HP equal to the summon level as an extra cost. HP is reduced even if summoning fails. Ignores player alignment.";
#endif

		{
			int lev = plev * 3 / 2;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);

			if(cast && p_ptr->chp < lev)
			{
				msg_print(_("今のあなたの体力ではこの召喚は危険だ。", "It's dangerous to cast this spell in your current state."));
				return NULL;
			}

			if (cast || fail)
			{
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack 普段混沌勢力を呼べない＠でも悪魔召喚できる*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_DEMON, summon_mode))
				{
					if(cast) msg_print(_("あなたは悪魔を召喚した。", "You summon a demon."));
					else     msg_print(_("召喚された悪魔はあなたの魂を要求した！", "The summoned demon wants your soul!"));

					take_hit(DAMAGE_USELIFE, lev, _("悪魔召喚による代償", "cost of demon summoning"), -1);
				}
				ignore_summon_align = FALSE;

			}
		}
		break;
///mod151024 追放を転送陣に変更
/*
	case 12:
#ifdef JP
		if (name) return "追放";
		if (desc) return "モンスター1体を現在の階から追放する。経験値やアイテムは手に入らない。テレポート耐性を無視するが抵抗されると無効。";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif

		{
			int power = 50 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;
*/
	case 12:
		if (name) return "Transportation Circle";
		if (desc) return "Transports an item to your house.";
		{

			if (info) return "";

			if (cast)
			{
				msg_print(_("あなたは転送の魔法陣を描いた・・", "You draw a transportation circle..."));
				item_recall(2);
			}
		}
		break;
	case 13:
#ifdef JP
		if (name) return "エレメンタル召喚";
		if (desc) return "エレメンタルを配下として複数召喚する。代償は不要だが、杖やロッドなどの魔道具を捧げると強力なエレメンタルが出てきやすい。";
#else
		if (name) return "Summon Elemental";
		if (desc) return "Summons an elemental. Has no extra cost, but you can offer a magic device to summon stronger elementals.";
#endif

		{
			int lev = plev ;
			if (info) return format(_("基本召喚レベル：%d", "base level: %d"),lev);
			if (cast || fail)
			{
				int num = 2 + plev / 15;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_staff_rod_wand;
					if(!select_pay(&cost)) cost=0;
					if(cost) lev += 5 + cost / 200;
					if(lev > 120) lev = 120;
				}

				if (new_summoning(num, py, px, lev, SUMMON_ELEMENTAL, summon_mode))
				{
					if(cast) msg_print(_("あなたはエレメンタルを召喚した。", "You summon an elemental."));
					else     msg_print(_("エレメンタルはあなたに使役されることを拒否した！", "The elemental refuses to serve you!"));
				}
			}
		}
		break;



	case 14:
#ifdef JP
		if (name) return "傭兵雇用";
		if (desc) return "人間、あるいは人型モンスターを配下として複数召喚する。代償として金を支払う。";
#else
		if (name) return "Employ Mercenaries";
		if (desc) return "Summons multiple humans or humanoid monsters. Requires money as an extra cost.";
#endif

		{
			int lev = MAX(dun_level,plev);
			int charge = lev * lev * 2;
			if (info) return format(_("召喚レベル：%d 報酬：＄%d", "level: %d fee: $%d"),lev,charge);
			if (cast || fail)
			{
				int num = 3 + randint1(3) + plev / 10;
				int cost;
				if(cast)	summon_mode |= (PM_FORCE_PET | PM_ALLOW_GROUP);
				else summon_mode |= PM_NO_PET;

				if(cast)
				{
					if(p_ptr->au < charge)
					{
						msg_print(_("あなたは充分なお金を持っていない。", "You don't have enough money."));
						return NULL;
					}
				}

				if (new_summoning(num, py, px, lev, SUMMON_HUMANOID, summon_mode))
				{
					if(cast)
					{
						p_ptr->au -= charge;
						p_ptr->redraw |= PR_GOLD;
						msg_print(_("あなたの元に傭兵たちが馳せ参じた。", "Several mercenaries join you."));
					}
					else     msg_print(_("傭兵たちはたった今強盗に転職したようだ！",
                                        "The mercenaries decided to make living as robbers!"));
				}
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "次元の扉";
		if (desc) return "短距離内の指定した場所にテレポートする。";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif

		{
			int range = p_ptr->lev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "タウン・テレポート";
		if (desc) return "行ったことのある街へ移動する。地上にいるときしか使えない。";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif

		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "ハウンド召喚";
		if (desc) return "狼・ゼファーハウンド系モンスターを配下として一グループ召喚する。代償は不要。";
#else
		if (name) return "Summond Hounds";
		if (desc) return "Summons wolves or zephyr hounds. Has no extra cost.";
#endif

		{
			int lev = plev;
			if (info) return format(_("召喚レベル：%d + 1d%d", "level: %d+1d%d"),lev,lev/2);
			if (cast || fail)
			{
				int num = 1;
				lev += randint1(lev/2);
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, SUMMON_HOUND, summon_mode))
				{
					if(cast) msg_print(_("あなたはハウンドを召喚した。", "You summon hounds."));
					else     msg_print(_("召喚されたハウンドはあなたに牙を剥いた！", "The summoned hounds turn their fangs on you!"));
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "位置交換テレポート";
		if (desc) return "1体のモンスターと位置を交換する。";
#else
		if (name) return "Swap Position";
		if (desc) return "Swap positions of you and a monster.";
#endif

		{
			if (cast)
			{
				bool result;

				/* HACK -- No range limit */
				project_length = -1;

				result = get_aim_dir(&dir);

				/* Restore range to default */
				project_length = 0;

				if (!result) return NULL;

				teleport_swap(dir);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "神格召喚";
		if (desc) return "神格を配下として一体召喚する。代償として武器を一つ消費する。高価な武器であるほど強力な神格が出やすい。";
#else
		if (name) return "Summon Deity";
		if (desc) return "Summons a deity. Requires a weapon as an extra cost. More valuable weapons will attract stronger deities.";
#endif

		{
			int lev = plev ;
			if (info) return format(_("基本召喚レベル：%d", "level: %d"),lev);
			if (cast || fail)
			{
				int num = 1;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_hook = object_allow_enchant_weapon;
					if(!select_pay(&cost)) return NULL;
					if(cost <= 0) //無価値なアイテムを使うと失敗する
					{
						summon_mode &= ~(PM_FORCE_PET);
						summon_mode |= PM_NO_PET;
					}
					lev += cost / 1000;
					if(lev > 120) lev = 120;
				}

				if (new_summoning(num, py, px, lev, SUMMON_DEITY, summon_mode))
				{
					if(summon_mode & PM_FORCE_PET) msg_print(_("あなたは神格を召喚した。", "You summon a deity."));
					else     msg_print(_("あなたは神の怒りに触れてしまったようだ！", "Divine wrath is upon you!"));
				}
			}
		}
		break;



	case 20:
#ifdef JP
		if (name) return "ダンジョンテレポート";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "ドラゴン召喚";
		if (desc) return "ドラゴンを配下として一体召喚する。代償は不要だが、ドラゴンの鱗や爪などの物品を捧げると強い竜が召喚されやすい。";
#else
		if (name) return "Summon Dragon";
		if (desc) return "Summons a dragon. Has no extra cost, but offering a dragon-related item attracts stronger dragons.";
#endif

		{
			int lev = plev * 4 / 5 ;
			if (info) return format(_("基本召喚レベル：%d", "base level: %d"),lev);
			if (cast || fail)
			{
				int num = 1;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					if(!select_pay(&cost)) cost=0;

					if(cost) lev += 5 + cost / 1000;
					if(lev > 80) lev = 80;
				}

				if (new_summoning(num, py, px, lev, SUMMON_DRAGON, summon_mode))
				{
					if(cast) msg_print(_("あなたは竜を召喚した。", "You summon a dragon."));
					else     msg_print(_("召喚された竜はあなたを賞味しようとしている！", "The summoned dragon wants to savor you!"));
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "召集";
		if (desc) return "フロアにいる自分の配下を自分の近くに呼び寄せる。";
#else
		if (name) return "Gathering";
		if (desc) return "Calls all of your followers on this floor to your side.";
#endif

		{
			if (cast)
			{

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
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "モンスター複数召喚";
		if (desc) return "ランダムなモンスターを配下として複数召喚する。代償は不要。";
#else
		if (name) return "Summon Monsters";
		if (desc) return "Summons several random monsters. Has no extra cost.";
#endif

		{
			int lev = plev;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);
			if (cast || fail)
			{
				int num = 3 + lev / 10;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, 0, summon_mode))
				{
					if(cast) msg_print(_("あなたはモンスターの群れを召喚した。", "You summon a horde of monsters."));
					else     msg_print(_("召喚された者たちは全てあなたに敵対した！", "All the monsters you summoned are hostile!"));
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "時空耐性";
		if (desc) return "一定時間、時空攻撃への耐性を得る。";
#else
		if (name) return "Resist Time";
		if (desc) return "Grants temporay resistance to time.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_time(randint1(base)+base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "*テレポート・アウェイ*";
		if (desc) return "視界内の全てのモンスターをテレポートさせる。抵抗されると無効。";
#else
		if (name) return "*Teleport Away*";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "電車召喚";
		if (desc) return "射程が短いがターゲットを吹き飛ばすビーム状の攻撃を仕掛ける。テレポート耐性を無視するが巨大な敵は吹き飛ばない。";
#else
		if (name) return "Summon Train";
		if (desc) return "Fires a short beam that blows the target away. Ignores teleportation resistance, but doesn't blow away gigantic enemies.";
#endif

		{

			int dam = plev * 2 + 100;
			int range = plev /5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;

				msg_print(_("突如、轟音とともに電車が通り過ぎた！", "Suddenly, a train passes by with a deafening sound!"));

				fire_beam(GF_TRAIN, dir, dam);
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "上位悪魔召喚";
		if (desc) return "強力なデーモンを配下として複数体召喚する。代償として召喚レベルの三倍のHPを消費する。召喚に失敗したときもこの代償は消費される。この召喚はプレーヤー勢力を無視する。";
#else
		if (name) return "Summon Greater Demons";
		if (desc) return "Summons several strong demons. Requires amount of HP equal to three times the summoning level as an extra cost. HP is decreased even is summoning fails. Ignores player alignment.";
#endif

		{
			int lev = plev * 3 / 2;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);

			if(cast && p_ptr->chp < lev * 3)
			{
				msg_print(_("今のあなたの体力ではこの召喚は危険だ。", "This summoning is too dangerous to perform in your current state."));
				return NULL;
			}

			if (cast || fail)
			{
				int num = 1 + plev / 20 + randint0(4);
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack 普段混沌勢力を呼べない＠でも悪魔召喚できる*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_HI_DEMON, summon_mode))
				{
					if(cast) msg_print(_("あなたは強力な悪魔たちを召喚した。", "You summon powerful demons."));
					else     msg_print(_("悪魔たちはあなたを連れて地獄に帰ろうとしている！", "The demons are intent on taking you to hell with them!"));

					take_hit(DAMAGE_USELIFE, lev*3, _("悪魔召喚による代償", "cost of demon summoning"), -1);
				}
				ignore_summon_align = FALSE;
			}
		}
		break;


	case 28:
#ifdef JP
		if (name) return "送還術";
		if (desc) return "一定時間、敵の召喚魔法を阻害する。敵のレベルが高いと効果が発揮されないことがある。";
#else
		if (name) return "Unsummoning";
		if (desc) return "Temporarily prevents enemies from casting summoning spells. Might not work against high level enemies.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_deportation(randint1(base)+base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "サイバーデーモン召喚";
		if (desc) return "強力な悪魔兵器を配下として一体召喚する。代償は不要。この召喚はプレーヤー勢力を無視する。";
#else
		if (name) return "Summon Cyberdemon";
		if (desc) return "Summons a powerful demon soldier. Has no extra cost. Ignores player alignment.";
#endif
		{
			if (info) return format(" ");

			if (cast || fail)
			{
				int lev = 77;
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack 普段混沌勢力を呼べない＠でも悪魔召喚できる*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_CYBER, summon_mode))
				{
					if(cast) msg_print(_("あなたはサイバーデーモンを召喚した。", "You summon a cyberdemon."));
					else     msg_print(_("サイバーデーモンは恐ろしい砲口をあなたに向けた！", "The cyberdemon points its terrifying rocket launcher at you!"));
				}
				ignore_summon_align = FALSE;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "古代ドラゴン召喚";
		if (desc) return "強力なドラゴンを配下として1体～複数召喚する。代償は不要だが、ドラゴンの鱗や爪などの物品を捧げると強い竜が召喚されやすい。";
#else
		if (name) return "Summon Ancient Dragons";
		if (desc) return "Summons one or more powerful dragons. Has no extra cost, but offering dragon-related items attracts stronger dragons.";
#endif

		{
			int lev = plev;
			if (info) return format(_("基本召喚レベル：%d", "base level: %d"),lev);
			if (cast || fail)
			{
				int num = randint1(2);
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::失敗時は報酬を消費しない*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					if(!select_pay(&cost)) cost=0;
					if(cost)
					{
						lev += 10 + cost / 1000;
						num += randint1(2) + cost / 10000;
					}
					if(lev > 120)
					{
						lev = 120;
					}
				}

				if (new_summoning(num, py, px, lev, SUMMON_HI_DRAGON, summon_mode))
				{
					if(cast) msg_print(_("あなたは古代竜を召喚した。", "You summon ancient dragons."));
					else     msg_print(_("古代の竜はあなたに向けて恐ろしい咆哮をあげた！", "The dragons turn on you, letting out a terrifying roar!"));
				}
			}
		}
		break;


	case 31:
#ifdef JP
//既に倒したものを強引に呼んだら何かトラブル起こる？
		if (name) return "宴会開催";
		if (desc) return "幻想郷の名のある者達を友好的な状態で召喚する。酒を一本消費し、高価な酒ほど大人数が召喚される。詠唱失敗時には誰も現れない。";
#else
		if (name) return "Fantasy Banquet";
		if (desc) return "Summons famous residents of Gensoukyou in friendly state. Consumes a bottle of alcohol; more valuable items will attract more people. Nobody appears if spell fails.";
#endif

		{
			int lev = 99;
			if (info) return format(_("召喚レベル：%d", "level: %d"),lev);
			if (cast)
			{
				int num = 1;
				int cost;
				summon_mode |= (PM_FORCE_FRIENDLY | PM_ALLOW_UNIQUE);

				item_tester_tval = TV_ALCOHOL;
				if(!select_pay(&cost)) return NULL;

				num += cost / 1000;

				if(num > 50) num = 50;

				msg_print(_("あなたは宴会の開催を宣言した！",
                            "You announce you're holding a banquet!"));
				new_summoning(num, py, px, lev, SUMMON_PHANTASMAGORIA, summon_mode);
			}
		}
		break;
	}

	return "";
}








/*:::新領域　神秘*/
static cptr do_new_spell_mystic(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "浮遊";
		if (desc) return "一定時間浮遊状態になる。";
#else
		if (name) return "Levitation";
		if (desc) return "Grants temporary levitation.";
#endif

		{
			int base = 25 + plev / 2;
			int sides = 25 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				set_tim_levitation(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "モンスター感知";
		if (desc) return "近くの全ての見えるモンスターを感知する。レベル20以上で見えないモンスターも感知する。";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
				if(plev > 19) detect_monsters_invis(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "ショート・テレポート";
		if (desc) return "近距離のテレポートをする。";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif

		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;



	case 3:
#ifdef JP
		if (name) return "光球生成";
		if (desc) return "光る球を生成し、今いる部屋を明るくする。";
#else
		if (name) return "Sphere of Light";
		if (desc) return "Creates a sphere of light, illuminating nearby area.";
#endif

		{

			if (cast)
			{
				msg_print(_("光球を設置した。", "You release a sphere of light."));
				lite_room(py,px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "催眠術";
		if (desc) return "モンスター1体を眠らせる。レベル30以上で視界内全てのモンスターを眠らせる。抵抗されると無効。";
#else
		if (name) return "Hypnotism";
		if (desc) return "Attempts to sleep a monster. At level 30 attempts to sleep all monsters in sight.";
#endif

		{
			//v1.1.90 強化
			int power = plev + 30;

			if (info) return info_power(power);

			if (cast)
			{
				if(plev < 30)
				{

					if (!get_aim_dir(&dir)) return NULL;
					sleep_monster(dir, plev);
				}
				else sleep_monsters(power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "テレポート";
		if (desc) return "遠距離のテレポートをする。";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif

		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "地形感知";
		if (desc) return "周辺の地形を感知する。";
#else
		if (name) return "Magic Mapping";
		if (desc) return "Maps nearby area.";
#endif

		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "魔力充填";
		if (desc) return "杖/魔法棒の充填回数を増やすか、充填中のロッドの充填時間を減らす。";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 8:
		if (name) return _("韋駄天の術", "Art of Idaten");
		if (desc) return _("一定時間移動速度が上昇する。", "Temporarily increases movement speed.");

		{
			int base = 25 + plev / 2;
			int sides =25 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				set_tim_speedster(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 9:
#ifdef JP
		if (name) return "鑑定";
		if (desc) return "アイテムを識別する。";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif

		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "テレポート・モンスター";
		if (desc) return "モンスターをテレポートさせるビームを放つ。抵抗されると無効。";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif

		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 11:
		if (name) return _("心頭滅却", "Clear Mind");
		if (desc) return _("一定時間耐火を得る。装備品の耐性に累積する。", "Grants temporary resistance to fire. Stacks with resistance provided by equipment.");

		{
			int base = 15 + plev / 2;
			int sides = 5 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				msg_print(_("精神を集中した・・", "You gather your mind..."));
				set_oppose_fire(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "身体加速";
		if (desc) return "一定時間、すべての行動の速度が上昇する。";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif

		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "霊酒醸成";
		if (desc) return "水あるいはジュース一つを「霊酒」にする。";
#else
		if (name) return "Brew Spirit Sake";
		if (desc) return "Turns water or juice into Spirit Sake.";
#endif

		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_make_reisyu;
				q = _("どれを酒にしますか? ", "Turn what into sake?");
				s = _("あなたは酒にするのに適当な飲み物を持っていない。", "You don't have appropriate drinks to turn into sake.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

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

				msg_print(_("あなたの手の中で瓶が芳醇な香りを放ち始めた・・",
                            "The bottle in your hand starts to give off a mellow smell..."));
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_REISYU));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				(void)drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "全感知";
		if (desc) return "近くの全てのモンスター、罠、扉、階段、財宝、そしてアイテムを感知する。";
#else
		if (name) return "Detection True";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "神格召喚";
		if (desc) return "神格をもつ存在を配下として1体召喚する。召喚された神格が敵対することもあり、敵対する確率は賢さと魅力による。";
#else
		if (name) return "Summon Deity";
		if (desc) return "Summons a divine being. Summoned deity might be hostile; hostility rate depends on your wisdom and charisma.";
#endif

		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;
				int slev = plev + randint1(plev/2);

				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(50)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!pet) slev += randint1(plev);
				if (!pet) mode |= (PM_ALLOW_UNIQUE);

				if (summon_specific((pet ? -1 : 0), py, px, slev, SUMMON_DEITY, mode))
				{
#ifdef JP
					msg_print("場に異様な気配が満ちた・・。");
#else
					msg_print("The area fills with a strange presence...");
#endif
					if (pet)
					{
#ifdef JP
						msg_print("神性を持つものがあなたを守護するために顕現した。");
#else
						msg_print("A divine being manifests to protect you.");
#endif
					}
					else
					{
#ifdef JP
						msg_print("神性を持つものがあなたの傲慢を罰するために顕現した！");
#else
						msg_print("A divine being manifests to punish you for your arrogance!");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("神格は現れなかった。");
#else
					msg_print("No deities arrive.");
#endif
				}
				break;
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "看破";
		if (desc) return "視界内のモンスターの属性、残り体力、最大体力、スピード、あやしい影の正体を知る。";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif

		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "飛燕の術";
		if (desc) return "行ったことのある街へ移動する。地上にいるときしか使えない。";
#else
		if (name) return "Art of Flying Swallow";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif

		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;


	case 18:
#ifdef JP
		if (name) return "幻術";
		if (desc) return "視界内のモンスターを混乱、恐怖、減速、朦朧させる。";
#else
		if (name) return "Illusionary Technique";
		if (desc) return "Attempts to confuse, terrify, slow and stun all monsters in sight.";
#endif

		{
			int power = plev * 3;
			if (info) return info_power(power);

			if (cast)
			{
				slow_monsters(power);
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
			}
		}
		break;



	case 19:
#ifdef JP
		if (name) return "*鑑定*";
		if (desc) return "アイテムの持つ能力を完全に知る。";
#else
		if (name) return "Identify True";
		if (desc) return "*Identifies* an item.";
#endif

		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;


	case 20:
#ifdef JP
		if (name) return "金丹の術";
		if (desc) return "HPを回復し、満腹度を増加させ、切り傷・毒・朦朧・幻覚を治癒する。";
#else
		if (name) return "Kintanjutsu";
		if (desc) return "Restores HP, satiates hunger and cures cuts, poison, stunning and hallucination.";
#endif

		{
			int heal = plev * 4;
			if (info) return info_heal(0, 0, heal);
			if (cast)
			{

				hp_player(heal);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
				set_food(p_ptr->food + 2000);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "壁抜け";
		if (desc) return "一定時間、半物質化し壁を通り抜けられるようになる。";
#else
		if (name) return "Walk through Wall";
		if (desc) return "Gives ability to pass walls for a while.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "帰還の呪文";
		if (desc) return "地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "次元の扉";
		if (desc) return "短距離内の指定した場所にテレポートする。";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif

		{
			int range = p_ptr->lev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("次元の扉が開いた。目的地を選んで下さい。");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;




	case 24:
#ifdef JP
		if (name) return "ミダス王の手";
		if (desc) return "アイテム1つを金塊に変え、お金として入手する。金額はそのアイテムの価値の1/3程度となる。";
#else
		if (name) return "Hand of Midas";
		if (desc) return "Turns an item into 1/3 of its value in gold.";
#endif

		{
			if (cast)
			{
				if (!alchemy()) return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "気配遮断";
		if (desc) return "一定時間、周囲の眠った敵が起きにくくなる。";
#else
		if (name) return "Conceal Presence";
		if (desc) return "Temporarily makes nearby enemies less likely to wake up.";
#endif

		{
			int base = plev / 2;
			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_stealth(randint1(base) + base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "剛力招来";
		if (desc) return "一定時間腕力を増加させる。通常の限界値を超える。";
#else
		if (name) return "Invoke Strength";
		if (desc) return "Temporarily raises strength. Can surpass ordinary limits.";
#endif

		{
			int base = plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format(_("常識外れの力があなたの腕に宿った気がする。",
                            "You feel strength defying common sense in your arms."));
				set_tim_addstat(A_STR,102 + plev/10 ,base + randint1(base),FALSE);

			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "千里眼";
		if (desc) return "その階全体を永久に照らし、ダンジョン内すべてのアイテムを感知する。さらに、一定時間テレパシー能力を得る。";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif

		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;


	case 28:
#ifdef JP
		if (name) return "矢返し";
		if (desc) return "一定時間、敵が放った矢やボルト系魔法を高確率で跳ね返せるようになる。";
#else
		if (name) return "Reflect Arrows";
		if (desc) return "Temporarily grants high chance to reflect enemy projectiles and bolt spells back at them.";
#endif

		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_reflect(randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "鹿角の術";
		if (desc) return "短時間、装備中の武器に理力の能力が付加される。";
#else
		if (name) return "Art of Deer Antlers";
		if (desc) return "Brands currently wielded weapon with force for a short period.";
#endif

		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
					msg_format(_("武器を持っていない。", "You're not wielding a weapon."));
					return FALSE;
				}
				set_ele_attack(ATTACK_FORCE, base + randint1(base));
			}
		}
		break;
	case 30:
#ifdef JP
		if (name) return "無傷の球";
		if (desc) return "ごく短時間、ダメージを受けなくなるバリアを張る。切れた瞬間に少しターンを消費するので注意。";
#else
		if (name) return "Globe of Invulnerability";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif

		{
			int base = 4;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_invuln(randint1(base) + base, FALSE);
			}
		}
		break;


	case 31:
#ifdef JP
		if (name) return "鬼神変化";
		if (desc) return "短時間巨大化する。それまでにかかっていた一時効果は全て解除される。巨大化中は身体能力が爆発的に上昇するが魔法書と巻物と魔道具が使用できない。Uコマンドで巨大化を解除できる。";
#else
		if (name) return "Kishin Form";
		if (desc) return "Transforms into a giant for a short period of time. Removes all other temporary effects. Incredibly raises physical abilities, but makes you unable to use spellbooks, scrolls and magic devices. Can be dispelled with 'U'.";
#endif

		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;

				reset_tim_flags();
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(base + randint1(base), MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP | PR_STATUS | PR_CUT | PR_STUN );
				redraw_stuff();
				break;
			}
		}
		break;





	}

	return "";
}


/*:::新破邪領域*/
static cptr do_new_spell_punish(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
		if (name) return _("魔除けのまじない", "Warding Charm");
		if (desc) return _("1体のモンスターに破邪属性のダメージを与え恐怖・能力低下させる。抵抗されると無効。破邪弱点でないモンスターには効果がない。",
                            "Deals holy damage to a single monster, terrifies it and lowers its stats. Does nothing if resisted. Does not affect monsters not vulnerable to holy attacks.");

		{
			int dice = 3 + (plev) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_PUNISH_1, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "混沌感知";
		if (desc) return "近くの混沌の勢力に属するモンスターを感知する。";
#else
		if (name) return "Detect Chaos";
		if (desc) return "Detects all chaotically aligned monsters in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 2:
		if (name) return _("祝福", "Blessing");
		if (desc) return _("一定時間、命中率とACにボーナスを得る。", "Temporarily increases AC and to-hit bonus.");

		{
			int base = 15+plev/5;
			if (info) return info_duration(base, base);
			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
		if (name) return _("御札投擲", "Throw Ofuda");
		if (desc) return _("射程のやや短い破邪属性のボルトを放つ。", "Fires a fairly short range holy bolt.");

		{
			int dice = 5 + plev / 10;
			int sides = 8;
			int range = 6 + plev / 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;
				msg_print(_("御札を投げた！", "You throw an ofuda!"));
				fire_bolt(GF_HOLY_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "透明体視認";
		if (desc) return "一定時間、透明な敵が見えるようになる。";
#else
		if (name) return "Sense Unseen";
		if (desc) return "Gives see invisible for a while.";
#endif

		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;
	case 5:
		if (name) return _("士気高揚", "Raise Morale");
		if (desc) return _("一定時間、恐怖を取り除き恐怖耐性を得て白兵能力が少し上昇する。", "Cures fear; temporarily grants resistance to fear and raises your combat abilities.");

		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "スターダスト";
		if (desc) return "ターゲット付近に閃光のボルトを連射する。";
#else
		if (name) return "Star Dust";
		if (desc) return "Fires many bolts of light near the target.";
#endif

		{
			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_LITE, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "解呪";
		if (desc) return "アイテムにかかった弱い呪いを解除する。";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("装備品にかかった呪縛が解けた。");
#else
					msg_print("The curse on your equipment has been lifted.");
#endif
				}
			}
		}
		break;



	case 8:
#ifdef JP
		if (name) return "治癒";
		if (desc) return "体力を中程度回復させ、切り傷を治療する。";
#else
		if (name) return "Curing";
		if (desc) return "Recovers a medium amount of HP and cures cuts.";
#endif

		{
			int dice = 4 + plev / 4;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;





	case 9:
#ifdef JP
		if (name) return "威圧";
		if (desc) return "モンスター1体を恐怖させる。抵抗されると無効。";
#else
		if (name) return "Intimidate";
		if (desc) return "Attempts to scare a monster.";
#endif

		{
			int power = plev * 3 / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "聖なる光球";
		if (desc) return "破邪属性のボールを放って攻撃する。純魔法職が使うと少し威力が高い。";
#else
		if (name) return "Holy Orb";
		if (desc) return "Fires a ball with holy power. Slightly more powerful when used by a pure magic class.";
#endif

		{
			int dice = 3;
			int sides = 4 + plev / 12;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			if (cp_ptr->magicmaster)
				base = plev * 2;
			else
				base = plev * 3 / 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HOLY_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "退魔の呪文";
		if (desc) return "視界内のアンデッド・悪魔・妖怪にダメージを与え朦朧とさせる。";
#else
		if (name) return "Exorcism";
		if (desc) return "Damages all undead, demons and youkai in sight, and also stuns them.";
#endif

		{
			int dam = plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				dispel_undead(dam);
				dispel_demons(dam);
				dispel_kwai(dam);
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "封印解除";
		if (desc) return "一直線上の全ての罠と扉を破壊する。";
#else
		if (name) return "Unbarring Ways";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif

		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "ディスペル";
		if (desc) return "アンデッド一体を中確率で一撃で倒す。抵抗されると無効。ターゲットが近いほど効きやすい。";
#else
		if (name) return "Dispel";
		if (desc) return "Has a medium chance of defeating an undead monster in a single strike. Does nothing if resisted. More effective against nearby targets.";
#endif

		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;

				y = target_row;
				x = target_col;
				m_ptr = &m_list[target_who];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int power;
					char m_name[80];
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					power = p_ptr->lev * 3 / 2 - m_ptr->cdis * 4;
					if(power<1) power=1;

					///mod140324 本家に倣って5%の成功保証を適用する。これほど適当解釈しといて何を今更という気もするが。
					msg_format(_("天から光が降り注いだ！", "Light shines down from heavens!"));
					if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && ( power / 2 + randint1( power / 2 ) > r_ptr->level || one_in_(20)))
					{
						msg_format(_("%sは光の中に消えた。", "%s fades away in light."),m_name);
						damage = m_ptr->hp + 1;
						project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					}
					else if(r_ptr->flags3 & RF3_UNDEAD)
					{
						msg_format(_("%sは解呪に抵抗した！", "%s resists being dispelled!"),m_name);
					}
					else
					{
						msg_format(_("%sには全く効いていないようだ。", "%s isn't affected at all."),m_name);
					}
				}
				else
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
			}
		}
		break;



		//v1.1.85 裁きの雷を「天なる轟き」に名称変更し、雷雲生成+半径1電撃ボール遠隔発生にした。ネタ元としてはDQのデイン系。
	case 14:
#if 0
#ifdef JP
		if (name) return "裁きの雷";
		if (desc) return "強力な電撃のボルトを放つ。";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif

		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt(GF_ELEC, dir, dam);
			}
		}
		break;
#else
		if (name) return _("天なる轟き", "Heavenly Thunder");
		if (desc) return _("指定した位置に雷雲を呼び出し、強力な電撃属性攻撃を行う。", "Calls thunderclouds to specified position, launching a powerful lightning attack.");
		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				if (!fire_ball_jump(GF_MAKE_STORM, dir, 2, 1, _("雷雲を呼び出した！", "You call forth thunderclouds!"))) return NULL;
				fire_ball_jump(GF_ELEC, dir, dam, 1, NULL);


			}
		}
		break;

#endif

	case 15:
#ifdef JP
		if (name) return "聖なる御言葉";
		if (desc) return "視界内の混沌の勢力に属するモンスターにダメージを与え、体力を回復し、毒、恐怖、朦朧状態、負傷から全快する。";
#else
		if (name) return "Holy Word";
		if (desc) return "Damages all evil monsters in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif

		{
			int dam_sides = plev * 6;
			int heal = 100;

#ifdef JP
			if (info) return format("損:1d%d/回%d", dam_sides, heal);
#else
			if (info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if (cast)
			{
				dispel_evil(randint1(dam_sides));
				hp_player(heal);
				set_afraid(0);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;



	case 16:
		if (name) return _("封魔", "Seal Evil");
		if (desc) return _("混沌の勢力に属するモンスターの動きを止める。", "Stops movement of a chaotically aligned monster.");

		{
			int power = plev * 7 / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				stasis_evil(dir);
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "対邪悪結界";
		if (desc) return "混沌の勢力のモンスターの攻撃を防ぐバリアを張る。";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif

		{
			int base = 25;
			int sides = 3 * plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + sides, FALSE);
			}
		}
		break;
	case 18:
		if (name) return _("破邪の印", "Holy Seal");
		if (desc) return _("1体のモンスターに破邪属性の大ダメージを与え恐怖・朦朧・能力低下させる。抵抗されると無効。破邪弱点でないモンスターには効果がない。",
                            "Deals large amount of holy damage to a single monster; also terrifies, stuns, and lowers stats. Does nothing if resisted. Has no effect on monster not vulnerable to holy attacks.");

		{
			int dice = 15 + plev / 5;
			int sides = 15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_PUNISH_4, dir, damroll(dice, sides), 0);
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "*解呪*";
		if (desc) return "アイテムにかかった強力な呪いを解除する。";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("装備品にかかった呪縛が解けた。");
#else
					msg_print("The curse on your equipment is lifted.");
#endif
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "聖なるオーラ";
		if (desc) return "一定時間、破邪属性のオーラを身にまとう。";
#else
		if (name) return "Holy Aura";
		if (desc) return "Gives aura of holy power which injures evil monsters which attacked you for a while.";
#endif

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_sh_holy(randint1(base) + base, FALSE);
			}
		}
		break;
	case 21:
#ifdef JP
		if (name) return "聖なる矢";
		if (desc) return "破邪属性のボルトを放つ。レベル45以降はビームを放つ。";
#else
		if (name) return "Holy Arrow";
		if (desc) return "Fires a bolt of holy energy. Fires a beam instead at level 45.";
#endif

		{
			int dam = plev * 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(plev < 45) fire_bolt(GF_HOLY_FIRE, dir, dam);
				else fire_beam(GF_HOLY_FIRE, dir, dam);
			}
		}
		break;





	case 22:
#ifdef JP
		if (name) return "邪悪追放";
		if (desc) return "視界内の混沌の勢力に属するモンスターを全てテレポートさせる。抵抗されると無効。";
#else
		if (name) return "Banish Evil";
		if (desc) return "Teleports all chaotically aligned monsters in sight away unless resisted.";
#endif

		{
			int power = 50 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (banish_evil(power))
				{
#ifdef JP
					msg_print("神聖な力が邪悪を打ち払った！");
#else
					msg_print("The holy power banishes evil!");
#endif

				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "閃光の嵐";
		if (desc) return "巨大な閃光の球を放つ。";
#else
		if (name) return "Light Storm";
		if (desc) return "Fires a huge ball of powerful light.";
#endif

		{
			int dam = 100 + plev * 3;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_LITE, dir, dam, rad);
			}
		}
		break;


	case 24:
#ifdef JP
		if (name) return "天石門別命降臨";
		if (desc) return "天石門別命(あまのいわとわけのみこと)をその身に降ろし、周囲全ての地形、アイテム、モンスターを消失させる。";
#else
		if (name) return "Descent of Ama-no-Iwato-Wake-no-Mikoto";
		if (desc) return "Become possessed by Ama-no-Iwato-Wake-no-Mikoto; wipes away all terrain, items and monsters in your vicinity.";
#endif
		{
			int rad = 3 + plev / 4;
			if (info) return format(_("範囲：%d", "range %d"), rad);

			if (cast)
			{
				set_kamioroshi(KAMIOROSHI_AMATO, 0);
				destroy_area(py, px, rad, FALSE, TRUE,FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "石凝姥命降臨";
		if (desc) return "石凝姥命(いしこりどめのみこと)をその身に降ろす。一定時間、閃光攻撃に対する完全な免疫と反射能力を得る。";
#else
		if (name) return "Descent of Ishikori-dome-no-Mikoto";
		if (desc) return "Become possessed by Ishikori-dome-no-Mikoto. Grants temporary immunity to light and reflection.";
#endif

		{
			int base = 15 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("石凝姥命が現れ、神鏡を掲げた！", "Ishikori-dome-no-mikoto appears, raising the sacred mirror!"));
				set_kamioroshi(KAMIOROSHI_ISHIKORIDOME, randint1(base) + base);
			}
		}
		break;
	case 26:
#ifdef JP
		if (name) return "飯綱権現降臨";
		if (desc) return "飯綱権現(いづなごんげん)をその身に降ろす。一定時間、直接攻撃によって混沌の勢力のモンスターに大きなダメージを与えられるようになる。";
#else
		if (name) return "Descent of Izuna-Gongen";
		if (desc) return "Become possessed by Izuna-Gongen. Temporarily makes your melee attacks deal extra damage to chaotically aligned monsters.";
#endif

		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("飯縄権現の護法の力が身に宿った！", "The power of Izuna-Gongen resides in your body!"));
				set_kamioroshi(KAMIOROSHI_IZUNA, randint1(base) + base);
			}
		}
		break;

	case 27:
		if (name) return _("伊豆能売降臨", "Descent of Izunome");
		if (desc) return _("伊豆能売(いづのめ)をその身に降ろし、視界内全てに対して強力な破邪属性攻撃を行い、さらに呪われた装備を解呪する。",
                            "Become possessed by Izunome; hits everything in sight with a powerful holy attack, and removes curses from your equipment.");
		{
			int dice = 1;
			int sides = plev * 3;
			int base = plev * 3;
			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				set_kamioroshi(KAMIOROSHI_IZUNOME, 0);
				msg_print(_("鈴の音と共にダンジョンに清浄な空気が満ちた・・",
                            "A bell rings, and the dungeon is filled with holiness..."));
				project_hack2(GF_HOLY_FIRE, dice,sides,base);
				if (remove_all_curse())
				{
					msg_print(_("装備品にかかった呪縛が解けた。", "The curse on your equipment is lifted."));
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "住吉三神降臨";
		if (desc) return "上筒男命（うわつつのおのみこと）、中筒男命（なかつつのおのみこと）、底筒男命（そこつつのおのみこと）をその身に降ろす。一定時間、加速、高速移動能力、水耐性、時空攻撃耐性、警告を得る。";
#else
		if (name) return "Descent of Sumiyoshi Sanjin";
		if (desc) return "Become possessed by Uwatsutsu-no-O-no-Mikoto, Nakatsutsu-no-O-no-Mikoto and Sokotsutsu-no-O-no-Mikoto. Grants temporary haste, swift movement, resistance to water and time, and warning.";
#endif

		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("住吉三神の加護を得た！", "You are protected by the Sumiyoshi Sanjin!"));
				set_kamioroshi(KAMIOROSHI_SUMIYOSHI, randint1(base) + base);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "愛宕権現降臨";
		if (desc) return "愛宕権現（あたごごんげん）をその身に降ろす。一定時間、炎に対する完全な免疫と強力な火炎のオーラを得て火炎属性攻撃ができるようになる。";
#else
		if (name) return "Descent of Atago-Gongen";
		if (desc) return "Become possessed by Atago-Gongen. Grants temporary immunity to fire, strong fire aura and ability to use fire attacks.";
#endif

		{
			int base = 5 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("愛宕様の神火が身に宿った！", "Divine flame of Atago-Gongen resides in your body!"));
				set_kamioroshi(KAMIOROSHI_ATAGO, randint1(base) + base);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "天宇受売命降臨";
		if (desc) return "天宇受売命（あめのうずめのみこと）をその身に降ろす。一定時間、暗黒・地獄・地獄の業火属性の攻撃への耐性、AC上昇、魔法防御上昇、魅力大幅上昇を得る。";
#else
		if (name) return "Descent of Ame-no-Uzume-no-Mikoto";
		if (desc) return "Become possessed by Ame-no-Uzume-no-Mikoto. Grants temporary resistance to darkness, nether and hellfire, raises AC and protection from magic, and greatly raises charisma.";
#endif

		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print(_("あなたは軽やかに舞い始めた！", "You start playfully dancing!"));
				set_kamioroshi(KAMIOROSHI_AMENOUZUME, randint1(base) + base);
			}
		}
		break;


	case 31:
		if (name) return _("天照大御神降臨", "Descent of Amaterasu Oomikami");
		if (desc) return _("天照大御神(あまてらすおおみかみ)をその身に降ろし、周囲の敵に大打撃を与える。自分の近くにいるモンスターほど効果が大きい。天宇受売命を降ろしているときに使うとさらに効果が増す。",
                            "Become possessed by Amaterasu Oomikami. Delivers severe damage to enemies in your vicinity; deals more damage to nearby monsters. Even more effective if used while being possessed by Ame-no-Uzume-no-Mikoto.");
		{

			if (info) return format("");

			if (cast)
			{
				call_amaterasu();
			}
		}
		break;


	}

	return "";
}





/*:::新領域　オカルト*/
static cptr do_new_spell_occult(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

	u32b summon_mode = 0L;

#ifdef JP
	static const char s_random[] = "ランダム";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
		if (name) return _("走る二宮金次郎像", "Running Ninomiya Kinjirou");
		if (desc) return _("二宮金次郎像が駆けて行ってターゲットへ体当たりする。ただし暗い穴や水地形などを越えられない。",
                            "Ninomiya Kinjirou statue charges at target and tackles it. Cannot cross terrain like chasms or water.");

		{
			int wgt = 200 + plev * 8;

			if (info) return format(_("重量:%d.%02d kg", "wgt: %d.%02d kg"),wgt/20,wgt%20);

			if (cast)
			{
				object_type forge;
				object_type *tmp_o_ptr = &forge;
				int ty,tx;

				if (!get_aim_dir(&dir)) return NULL;
				tx = px + 99 * ddx[dir];
				ty = py + 99 * ddy[dir];
				if ((dir == 5) && target_okay())
				{
					tx = target_col;
					ty = target_row;
				}
				//ダミーアイテムを生成し重量を設定し特殊射撃ルーチンへ渡す
				msg_print(_("二宮金次郎像が駈け出した！", "Ninomiya Kinjirou statue runs forward!"));
				object_prep(tmp_o_ptr,lookup_kind(TV_MATERIAL,SV_MATERIAL_NINOMIYA));
				tmp_o_ptr->weight = wgt;
				do_cmd_throw_aux2(py,px,ty,tx,2,tmp_o_ptr,6);

			}
		}
		break;

	case 1:
		if (name) return _("こっくりさん", "Kokkuri-san");
		if (desc) return _("$10消費して近くのモンスターを感知する。レベル15以降は罠も感知し$50消費、レベル25以降はアイテムも感知して$100消費する。この魔法に失敗したとき幽霊系のモンスターに襲われる。",
                            "Pay $10 to detect nearby monsters. At level 15, detects traps as well and costs $50; at level 25, detects items as well and costs $100. You get attacked by hostile ghosts if you fail to properly cast this spell.");
		{
			int rad = DETECT_RAD_DEFAULT;
			int price;
			if (info) return info_radius(rad);

			if (cast)
			{
				if(plev < 15) price = 10;
				else if(plev < 24) price = 50;
				else price = 100;

				if(p_ptr->au < price)
				{
					msg_print(_("お金が足りない。", "You don't have enough money."));
					return NULL;
				}
				msg_print(_("こっくりさんに周囲の状況を教わった...",
                            "Kokkuri-san gives you knowledge of nearby area..."));
				detect_monsters_normal(rad);
				detect_monsters_invis(rad);
				if(plev > 14) detect_traps(rad,TRUE);
				if(plev > 24) detect_objects_normal(rad);
				p_ptr->au -= price;
				p_ptr->redraw |= PR_GOLD;

			}
			else if(fail)
			{
				if (summon_specific(0, py, px, MAX(dun_level,10), SUMMON_GHOST, (PM_ALLOW_UNIQUE | PM_NO_PET)))
					msg_print(_("間違えて敵対的な霊を呼んでしまった！", "You accidentally summon a hostile spirit!"));

			}
		}
		break;


	case 2:
		if (name) return _("消えずの行灯", "Unextinguishable Lantern");
		if (desc) return _("装備中のランタンか松明に燃料を補給する。", "Refills your light source.");

		{
			if (cast)
			{
				int max_flog = 0;
				object_type * o_ptr = &inventory[INVEN_LITE];

				if(o_ptr->name2 == EGO_LITE_DARKNESS)
				{
					msg_print(_("光源は暗いままだ...", "Your light source is still dark..."));
					break;
				}
				if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
				{
					msg_print(_("ランタンは数度またたき、怪しげな光を放ち始めた。",
                                "Your lantern twitches several times and starts emitting a mysterious light."));
					max_flog = FUEL_LAMP;
				}
				else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
				{
					msg_print(_("松明が煌々と光った。", "Your torch shines brightly."));
					max_flog = FUEL_TORCH;
				}
				else
				{
					msg_print(_("燃料が要る光源を装備していない。", "You're not using a light source that needs refueling."));
					return NULL;
				}

				o_ptr->xtra4 += (max_flog / 2);
				if (o_ptr->xtra4 >= max_flog) o_ptr->xtra4 = max_flog;
				p_ptr->update |= (PU_TORCH);
			}
		}
		break;

	case 3:
		if (name) return _("校長先生の肖像画", "Head Teacher's Portrait");
		if (desc) return _("隣接した壁にモンスター「校長先生の肖像画」を配下として設置する。",
                            "Places 'Head Teacher's Portrait' monster on an adjacent wall as a follower.");

		{
			if (cast)
			{
				int y,x;
				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!cave_have_flag_bold(y,x,FF_HURT_ROCK))
				{
					msg_print(_("そこには飾れない。", "You can't hang it here."));
					return NULL;
				}
				if(cave_have_flag_bold(y,x,FF_DOOR))
				{
					msg_print(_("グラグラして飾れない。", "It's wobbling and won't hang properly."));
					return NULL;
				}

				if (cave[y][x].m_idx )
				{
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					if(m_ptr->ml) msg_print(_("そこにはモンスターがいる！", "There's a monster in the way!"));
					else msg_print(_("うまく飾れなかった。何だか壁の様子がおかしい。", "You couldn't hang it. Something's weird about this wall."));
					break;
				}
				if(summon_named_creature(0, y, x, MON_PORTRAIT, PM_FORCE_PET))
					msg_print(_("肖像画を飾った。", "You hang the portrait."));
				else
					msg_print(_("失敗した。", "Spell failed."));

			}
		}
		break;


	case 4:
		if (name) return _("置いてけ掘", "Oiteke-bori");
		if (desc) return _("アイテムを拾って持っている敵が水地形に隣接しているときにしか使えない。敵一体に水属性の大ダメージを与えてアイテムを落とさせ、ユニークモンスターでない場合高確率で一撃で倒す。",
                            "Can be used only on enemies that have picked up an item and are adjacent to a water square. Delivers significant water damage and becomes them drop the item, and has high chance of defeating non-unique monster in a single strike.");

		{
			int i;
			char m_name[80];
			int x,y,tx,ty;
			monster_type *m_ptr;
			monster_race *r_ptr;
			bool flag_water = FALSE;
			int dam = 200 + plev * 4;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(!m_ptr->hold_o_idx)
				{
					msg_format(_("%sは何も持っていない。", "%s isn't holding anything."),m_name);
					return NULL;
				}

				for (i = 0; i < 8; i++)
				{
					ty = y + ddy_ddd[i];
					tx = x + ddx_ddd[i];
					if(!in_bounds(ty,tx)) continue;
					if(cave_have_flag_bold(ty,tx,FF_WATER))
					{
						flag_water = TRUE;
						break;
					}
				}
				if(!flag_water)
				{
					msg_format(_("%sの近くに水場がない。", "There's no bodies of water close to %s."),m_name);
					return NULL;
				}
				msg_print(_("どこからともなく「置・い・て・け～」と声がする..",
                            "A voice sounds out of nowhere... 'LEAVE... IT... BEHIND...'"));

				if(!(r_ptr->flagsr & (RFR_RES_WATE | RFR_RES_ALL)) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & (RF7_AQUATIC | RF7_UNIQUE2))
				&& randint1((r_ptr->flags2 & (RF2_POWERFUL | RF2_GIGANTIC))?(dam/4):(dam/2)) > r_ptr->level )
				{
					msg_format(_("水面から幾多の手が伸び、%sを引きずり込んだ！",
                                "Countless arms extend from the water surface and drag %s in!"),m_name);
					dam = m_ptr->hp + 1;

					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,flg,-1);
				}
				else
				{
					msg_format(_("水面から幾多の手が伸び、%sを引きずり込んだ！",
                                "Countless arms extend from the water surface and drag %s in!"),m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_WATER,flg,-1);
					if(m_ptr->r_idx) monster_drop_carried_objects(m_ptr);

				}

			}
		}
		break;


	case 5:
		if (name) return _("トイレの花子さん", "Hanako-san in the Toilet");
		if (desc) return _("閉じたドアに使っておくと、そのドアを開けようとしたモンスターにダメージを与えて朦朧とさせ高確率で吹き飛ばす。効果が発動したドアは消える。プレイヤーが開けると効果が解除される。",
                            "Used on a closed door. When a monster opens that door, deals damage, stuns and has high chance of throwing it aside. The door disappears when this effect triggers. Effect is dispelled if player opens the door.");
		{
			int dice = 3 + plev / 12;
			int sides = 77;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int y,x;
				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!is_closed_door(cave[y][x].feat))
				{
					msg_print(_("そこには使えない。", "Can't use it here."));
					return NULL;
				}

				if (cave[y][x].m_idx )
				{
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					if(m_ptr->ml) msg_print(_("そこにはモンスターがいる！", "There's a monster in the way!"));
					else 		msg_print(_("失敗した。", "Spell failed."));
					break;
				}

				cave_set_feat(y, x, f_tag_to_index_in_init("HANAKOSAN_DOOR"));
				msg_print(_("ドアに術を施して張り紙をした。", "You use the technique on the door and put a scrap of paper on it."));
			}
		}
		break;


	case 6:
		if (name) return _("赤マント青マントの怪人", "Red Mantle, Blue Mantle");
		if (desc) return _("隣接した人間もしくは人間型生物一体に地獄属性のダメージを与え、HPかMPのどちらかを吸収する。",
                            "Deals nether damage to adjacent human or humanoid being, and drains either HP or MP.");

		{
			char m_name[80];
			int tx,ty,x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			int dam;
			bool flag_water = FALSE;
			int base = plev;
			int heal;

			int dice = 8;
			int sides = 4 + plev / 3;

			if (info) return info_damage(dice, sides, base);
			if (cast)
			{

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];
				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				if(!(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)))
				{
					msg_format(_("効果がなかった。", "There was no effect."));
					break;

				}
				monster_desc(m_name, m_ptr, 0);
				dam = damroll(dice,sides)+base;
				heal = dam;

				if(!(cave[y][x].m_idx % 2)) // 効果がいつも同じになるようにm_idxで判定
				{
					msg_format(_("赤いマントが%sを包み込んだ！", "The red mantle wraps around %s!"),m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_NETHER,flg,-1);
					if(r_ptr->flagsr & RFR_RES_NETH) heal /= 3;
					hp_player(heal);
				}
				else
				{
					msg_format(_("青いマントが%sを包み込んだ！", "The blue mantle wraps around %s!"),m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_NETHER,flg,-1);
					heal /= 2;
					if(r_ptr->flagsr & RFR_RES_NETH) heal /= 3;
					player_gain_mana(heal);

				}

			}
		}
		break;


	case 7:
		if (name) return _("足洗邸","Ashiarai-yashiki");
		if (desc) return _("ターゲットに大ダメージを与え地震を起こす。プレイヤーが地震の範囲内にいると巻き込まれてダメージを受けることがある。ターゲットを中心とした縦横3グリッドに壁などがあると使えない。また屋外では使えない。",
                            "Deals significant damage to target and causes on earthquake. You also receive damage if you are within the earthquake radius. Cannot be used if there are walls or similar terrain within 3 tiles of the target. Cannot be used outdoors.");

		{
			int j;
			char m_name[80];
			int tx,ty,x,y;
			monster_type *m_ptr;
			bool flag_ok = TRUE;
			int i;

			int dam = plev * 6;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{
				if(!dun_level || p_ptr->inside_quest == QUEST_CHAOS_WYRM ||
					p_ptr->inside_quest == QUEST_GRAVEDANCER || p_ptr->inside_quest == QUEST_OOGAME
					|| p_ptr->inside_quest == QUEST_KILL_MOKO || p_ptr->inside_quest == QUEST_LIVE
					|| p_ptr->inside_quest == QUEST_KANASUWA || p_ptr->inside_quest == QUEST_KORI )
				{
					msg_print(_("ここでは使えない。", "You can't use it here."));
					return NULL;
				}

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in your sight."));
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}

				//射撃が通らない地形、マップ端、登り階段、木が近くにあると使えない
				//v1.1.82 条件式を間違ってたので修正
				//v1.1.82a 1.1.11でこの形に調整したのを忘れてた。もとに戻して説明文の方を修正する。
				for (i = -1; i < 1; i++) for (j = -1; j < 1; j++)
				{
					ty = y + i;
					tx = x + j;
					if(!in_bounds2(ty,tx) || !cave_have_flag_bold(ty,tx,FF_PROJECT) || cave_have_flag_bold(ty,tx,FF_LESS))
					{
						flag_ok = FALSE;
						break;
					}
				}
				if(!flag_ok)
				{
					msg_format(_("もっと広い場所でないと使えない。", "You need more open space to use this."));
					return NULL;
				}
				monster_desc(m_name, m_ptr, 0);
				ty = m_ptr->fy;
				tx = m_ptr->fx;
				msg_format(_("巨大な足が現れて%sを踏み潰した！", "A giant foot appears and tramples %s!"),m_name);
				project(0,0,ty,tx,dam,GF_MISSILE,flg,-1);
				earthquake(ty, tx, 5);

			}
		}
		break;


	case 8:
		if (name) return _("人面犬", "Human-faced dog");
		if (desc) return _("友好的なモンスター「人面犬」が現れる。", "Summons a friendly Human-faced dog.");
		{
			if(info) return "";

			if(cast)
			{

				if(summon_named_creature(0, py, px, MON_MANFACE_DOG, PM_FORCE_FRIENDLY))
				{
					if(one_in_(10)) msg_print(_("「もう俺のことなんてほっといてくれよお・・」", "'Please leave me alone...'"));
					msg_print(_("奇妙な風貌の犬が現れた。", "A weird-looking dog appears."));
				}
				else
					msg_print(_("何も現れなかった。", "Nobody arrives."));
			}
			break;
		}
	case 9:
		if (name) return _("隙間女", "Gap Woman");
		if (desc) return _("指定方向の壁などの移動不可地形の向こうへ移動する。通常のテレポートで移動できない場所には出られない。マップ端まで移動先がない場合は移動に失敗する。この魔法は反テレポート状態でも使うことができる。",
                            "Moves to the other side of a wall or other impassable terrain in given direction. Cannot arrive in a location normally inaccessible by teleportation. Movement fails if no suitable location is found up to the map edge. Can be used even under effects of anti-teleportation.");

		{

			if (info) return "";

			if (cast)
			{
				int x,y;
				int attempt = 500;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];

				if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
				{
					msg_format(_("そこには壁がない。", "There's no wall here."));
					return NULL;
				}
				while(1)
				{
					attempt--;
					y += ddy[dir];
					x += ddx[dir];

					if(y >= cur_hgt || y < 1 || x >= cur_wid || x < 1)
					{
						msg_format(_("あなたは壁の隙間に身を捩じ込んだ！", "You squeeze yourself through a gap in the wall!"));
						msg_print(_("..しかし隙間の先は行き止まりだった。", "...However, there's something blocking your movement."));
						break;
					}
					if(!cave_player_teleportable_bold(y, x, 0L)) continue;
					msg_format(_("あなたは壁の隙間に身を捩じ込んだ！", "You squeeze yourself through a gap in the wall!"));
					//テレポ阻害を無視するためにここだけNONMAGICALにする
					teleport_player_to(y,x,TELEPORT_NONMAGICAL);
					break;
				}
			}
		}
		break;


	case 10:
		if (name) return _("口裂け女","Kuchisake-Onna");
		if (desc) return _("周囲の敵を恐怖、混乱させる。魅力が高いほど効果が高いが、使用すると魅力が一時的に低下する。",
                            "Terrifies and confuses nearby enemies. More effective depending on your charisma, but temporarily reduces charisma upon use.");

		{
			int power = plev + (p_ptr->stat_ind[A_CHR] + 3) * 5;
			if(info) return format(_("効力:%d", "pow: %d"),power);
			if(cast)
			{
				msg_print(_("あなたは恐ろしい口元を露わにした...", "You reveal a terrifying mouth..."));
				confuse_monsters(power);
				turn_monsters(power);
				do_dec_stat(A_CHR);
			}
		}
		break;



	case 11:
		if (name) return _("ターボババァ", "Turbo Granny");
		if (desc) return _("一時的に高速移動能力を得る。レベル35以降はさらに加速する。",
                            "Grants temporary swift movement. At level 35 hastes as well.");

		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_tim_speedster(time, FALSE);

				if(plev > 34) set_fast(time, FALSE);
			}
		}
		break;


	case 12:
		if (name) return _("番町皿屋敷", "Banchou Sarayashiki");
		if (desc) return _("自分を中心とした地獄属性の球を発生させる。このときザックの中の一番高価な割れ物がひとつ破壊され、その価格に応じてダメージが上がる。",
                            "Generates a ball of nether centered on you. Breaks the most expensive item in your inventory that can be shattered, and increases damage according to its cost.");
		{
			int dam = 100 + plev * 4;
			int rad;
			int slot;
			int value = 0;
			int i;
			bool flag;
			object_type *o_ptr;
			char o_name[MAX_NLEN];

			if (info) return format(_("損傷:最大%d＋α", "dam: max %d+"),dam/2);

			if (cast)
			{


				for (i = 0; i < INVEN_PACK; i++)
				{
					int tmp_value;
					o_ptr = &inventory[i];


					if(!o_ptr->k_idx) continue;
					if(o_ptr->tval != TV_POTION && o_ptr->tval != TV_FLASK && o_ptr->tval != TV_COMPOUND) continue;
					if(o_ptr->tval == TV_FLASK && o_ptr->sval != SV_FLASK_OIL) continue;
					if(o_ptr->tval == TV_COMPOUND && o_ptr->sval == SV_COMPOUND_HOURAI) continue;
					tmp_value = object_value_real(o_ptr);
					tmp_value -= tmp_value * o_ptr->discount / 100;

					if(!tmp_value) continue;
					if(tmp_value < value) continue;

					slot = i;
					value = tmp_value;
				}

				if(!value)
				{
					msg_print(_("何も起こらなかった。", "Nothing happens."));
					break;
				}

				o_ptr = &inventory[slot];

				if(value < 200) ; //ボーナスなし
				else if(value < 500) dam += 50 + randint1(100);
				else if(value < 3000) dam += dam + 200 + randint1(200);
				else if(value < 10000) dam += dam * 2 + 300 + randint1(300);
				else if(value < 50000) dam += dam * 4 + 400 + randint1(400);
				else  dam += dam * 9 + 1000;

				if(value < 3000) rad = 2;
				else if(value < 10000) rad = 3;
				else if(value < 50000) rad = 4;
				else rad = 6;

				if(o_ptr->number > 1) flag = TRUE;
				else flag = FALSE;

				object_desc(o_name,o_ptr,(OD_OMIT_PREFIX | OD_NAME_ONLY));
				inven_item_increase(slot,-1);
				inven_item_optimize(slot);

				if(flag) msg_format(_("%sが一つ足りない...", "One %s is missing..."),o_name);
				else 	 msg_format(_("%sがない...", "There's no %s..."),o_name);
//portion_smash_effectを入れる？

				project(0,rad,py,px,dam,GF_NETHER,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID),-1);

			}
		}
		break;

	case 13:
		{
			int rad = MAX_SIGHT;

			if (name) return _("メリーさんの電話", "Phone Call from Mary");
			if (desc) return _("周囲のランダムなモンスターの隣にテレポートし、そのまま隣接攻撃を仕掛ける。精神を持たないモンスターは対象にならない。",
                                "Teleports next to a random nearby monster and performs melee attack. Cannot target mindless monsters.");
			if (info) return info_radius(rad);

			if(cast)
			{
				monster_type *m_ptr;
				monster_race *r_ptr;
				int i;

				int num=0;
				int tx,ty;

				msg_print(_("あなたは電話をかけ始めた...", "You start dialing your phone..."));
				for (i = 1; i < m_max; i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if (is_pet(m_ptr)) continue;
					if (is_friendly(m_ptr)) continue;
					if(r_ptr->flags2 & (RF2_WEIRD_MIND | RF2_EMPTY_MIND)) continue;
					if(distance(py,px,m_ptr->fy,m_ptr->fx) > rad) continue;

					num++;
					if(!one_in_(num)) continue;

					ty = m_ptr->fy;
					tx = m_ptr->fx;

				}
				if(!num)
				{
					msg_print(_("誰も電話に出なかった。", "Nobody picks up."));
				}
				else if(p_ptr->anti_tele)
				{
					msg_print(_("テレポートに失敗した。", "Teleport failed."));
				}
				else
				{
					teleport_player_to(ty,tx,0L);
					if(distance(py,px,ty,tx)==1)
					{
						char m_name[80];
						monster_desc(m_name, &m_list[cave[ty][tx].m_idx], 0);
						msg_format(_("あなたは%sの後ろに出現した！", "You appear behind %s!"),m_name);
						py_attack(ty,tx,0);
					}
					else
					{
						msg_print(_("うまく後ろに出られなかった。", "You couldn't properly appear behind."));
					}
				}
			}
		}
		break;

	case 14:
		if (name) return _("八尺さま", "Hasshaku-sama");
		if (desc) return _("このフロア限定でモンスター『八尺さま』を配下として召喚する。八尺さまは強力な攻撃を行うが、近くに人間もしくは人型生物の男がいると命令を無視してそちらを襲いに行く。",
                            "Summons Hasshaku-sama to this floor. She has powerful attacks, but if there are nearby male humans or humanoid beins, she ignores orders and attacks them.");
		if(info) return "";
		if(cast)
		{
			int i;
			monster_type *m_ptr;
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_HASSYAKUSAMA)
				{
					msg_print(_("八尺さまはすでにこのフロアにいる。", "Hasshaku-sama already is on this floor."));
					return NULL;
				}
			}

			if(summon_named_creature(0, py, px, MON_HASSYAKUSAMA, PM_EPHEMERA))
			{

				if(one_in_(7)) msg_print(_("「ぽっぽぽっぽっぽ・・・」", "'Poppopoppoppo...'"));
				msg_print(_("異様なほどに長身の女性が現れた。", "An extraordinarily tall woman appears."));
			}
			else
				msg_print(_("何も現れなかった。", "Nobody arrives."));

		}
		break;

	case 15:
		if (name) return _("くねくね", "Kunekune");
		if (desc) return _("このフロア限定でモンスター「くねくね」を配下として召喚する。くねくねは近くのモンスターに精神攻撃を行い、またモンスターの視界内にいるとき混乱・朦朧させたり行動を阻害することがある。",
                            "Summons a Kunekune to this floor. It performs mental attacks against nearby monsters, and might confuse or stun monsters that have it in sight.");
		if(info) return "";
		if(cast)
		{
			int i;
			monster_type *m_ptr;
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_KUNEKUNE)
				{
					msg_print(_("一度に一体しか出せない。", "You can summon only one at a time."));
					return NULL;
				}
			}

			if(summon_named_creature(0, py, px, MON_KUNEKUNE, PM_EPHEMERA))
			{
				msg_print(_("くねくねと身をよじる奇妙なものが現れた..", "A weird wriggling thing appears..."));
			}
			else
				msg_print(_("何も現れなかった。", "Nobody arrives."));

		}
		break;


	case 16:
		if (name) return "UFO";
		if (desc) return _("地上にいるときはダンジョンの到達済み最深階へ、ダンジョンにいるときは地上へと移動する。",
                            "Transports you to the deepest dungeon level reached if you're on surface, or back to surface if you're in a dungeon.");

		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if(one_in_(3))		msg_print(_("あなたは心の中で宇宙人へ呼びかけた...", "You call out to aliens from the bottom of your heart..."));
				else if(one_in_(2))	msg_print(_("あなたは宇宙船とのチャネリングを試みた...", "You tried channeling with a spaceship..."));
				else				msg_print(_("《ベントラー、ベントラー...》", "< Bentora, bentora... >"));
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 17:
		if (name) return _("人体発火現象", "Spontaneous Human Combustion");
		if (desc) return _("周囲のランダムな生物一体の場所に強力なプラズマ属性の球が発生する。",
                            "Generates a powerful ball of plasma on top of a random nearby living being.");
		{
			int tx,ty;
			int rad = 1 + plev / 24;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int i;
			int dice = 10;
			int sides = 25 + plev/2;
			int num = 0;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				msg_print(_("周囲の空気が乾き、パチパチと音を立てた・・", "The air grows dry, and you hear crackling sounds..."));
				for (i = 1; i < m_max; i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if (is_pet(m_ptr)) continue;
					if (is_friendly(m_ptr)) continue;
					if (!monster_living(r_ptr)) continue;
					if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					num++;
					if(!one_in_(num)) continue;

					ty = m_ptr->fy;
					tx = m_ptr->fx;
				}
				if(!num)
				{
					msg_print(_("しかし何も起こらなかった。", "However, nothing happens."));
				}
				else
				{
					char m_name[80];
					int dam = damroll(dice,sides);
					monster_desc(m_name, &m_list[cave[ty][tx].m_idx], 0);
					msg_format(_("%sは突然眩い炎に包まれて燃え上がった！", "%s suddenly bursts in dazzling flames!"),m_name);
					project(0, rad, ty, tx, dam, GF_PLASMA, PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM, -1);
				}

			}
		}
		break;

	case 18:
		if (name) return _("ネッシー", "Nessie");
		if (desc) return _("水属性のボルトやブレス等を放つ。自分が水地形にいないと使えず、威力は周囲の水地形の数により変わる。使うと水地形が干上がっていく。",
                            "Fires a bolt or breath of water. Cannot be used outside of water terrain, and gets more powerful depending on amount of water in nearby area. The water terrain will dry up upon use.");
		{

			int power=0, dam;
			int xx,yy;

			if (info) return format(_("損傷:不定", "dam: unkown"));

			if (cast)
			{
				int dist = plev / 5;
				if(!cave_have_flag_bold(py,px,FF_WATER))
				{
					msg_print(_("ここには水がない。", "There's no water here."));
					return NULL;
				}
				if (!get_aim_dir(&dir)) return NULL;

				for(yy = py - 10;yy <= py + 10;yy++)
				for(xx = px - 10;xx <= px + 10;xx++)
				{
					if(!in_bounds(yy,xx)) continue;
					if(!cave_have_flag_bold(yy,xx,FF_WATER)) continue;
					if(distance(yy,xx,py,px) > dist) continue;
					if(!projectable(yy,xx,py,px)) continue;

					if(cave_have_flag_bold(yy,xx,FF_DEEP)) power += 3;
					else power++;
				}
				//powerの最大値は1000くらい
				dam = power;
				if(power < 100)
				{
					msg_print(_("水面から何か小さな生き物が首を出して水玉を吹いた。",
                                "Some small creature raises its head from surface and blows water."));
					fire_bolt(GF_WATER,dir,dam);
				}
				else if(power < 500)
				{
					msg_print(_("水面から謎の怪物が現れてブレスを吐いた。",
                                "A mysterious monster appears from water surface and breathes water."));
					fire_ball(GF_WATER,dir,dam,-2);
				}
				else
				{
					msg_print(_("水面を押し上げて巨大な怪獣が現れ激流を吐き出した！",
                                "A massive monster rises out of water and unleashes a violent torrent!"));
					fire_spark(GF_WATER,dir,dam,2);
				}

				for(yy = py - 10;yy <= py + 10;yy++)
				for(xx = px - 10;xx <= px + 10;xx++)
				{
					if(!in_bounds(yy,xx)) continue;
					if(!cave_have_flag_bold(yy,xx,FF_WATER)) continue;
					if(distance(yy,xx,py,px) > dist) continue;
					if(!projectable(yy,xx,py,px)) continue;

					//水が枯れる処理
					if(cave_have_flag_bold(yy,xx,FF_DEEP))
					{
						if(!one_in_(3)) cave_set_feat(yy, xx, feat_shallow_water);
					}
					else if(one_in_(2))
					{
						if(one_in_(3))
							cave_set_feat(yy, xx, feat_swamp);
						else if(one_in_(2))
							cave_set_feat(yy, xx, feat_floor);
						else
							cave_set_feat(yy, xx, feat_dirt);
					}
				}

			}
		}
		break;
	case 19:
		if (name) return _("猿の手", "Monkey's Paw");
		if (desc) return _("ターゲット一体に無属性の大ダメージを与える。ただし三回使うたびに太古の怨念が発生する。詠唱に失敗した時も使ったとみなされる。",
                            "Deals significant amount of nonelemental damage to a single target. However, activates Ancient Foul Curse on every third attempt. Failed attempts count as well.");
		{
			int j;
			char m_name[80];
			int x,y;
			bool flag_ok = TRUE;
			monster_type *m_ptr;

			int dam = 512;

			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}

				monster_desc(m_name, m_ptr, 0);

				msg_format(_("あなたは猿の手に%sを握り潰すようにと願った！",
                            "You make a wish to crush %s!"),m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_ARROW,flg,-1);
			}

			if(cast || fail)
			{
				p_ptr->monkey_count++;

				if(p_ptr->monkey_count > 2)
				{

					bool stop_ty = FALSE;
					int count = 0;

					msg_print(_("猿の手はあなたに災いをもたらした！", "The monkey's paw brings calamity upon you!"));
					do
					{
						stop_ty = activate_ty_curse(stop_ty, &count);
					}
					while (one_in_(6));

					p_ptr->monkey_count = 0;

				}
			}

		}
		break;

	case 20:
		if (name) return _("リトルグリーンマン", "Little Green Men");
		if (desc) return _("周囲の恐怖状態の敵に対し、複数回の無属性攻撃が行われる。視界内でなくてもターゲットになる。",
                            "Performs several nonelemental attacks against nearby terrified enemies. Can attack targets out of your sight as well.");
		{
			int dam = 50+plev;
			int num = 2 + plev / 10;
			int i;
			if (info) return "";

			if (cast)
			{
				bool flag = FALSE;
				msg_print(_("緑色の小人たちが散っていった..", "Little green men scatter about..."));
				for(;num>0;num--)
				{
					for(i=1;i<m_max;i++)
					{
						char m_name[80];
						monster_type *m_ptr = &m_list[i];

						if(!m_ptr->r_idx) continue;
						if(m_ptr->cdis > MAX_SIGHT) continue;
						if(!MON_MONFEAR(m_ptr)) continue;
						if(!is_hostile(m_ptr)) continue;

						monster_desc(m_name, m_ptr, 0);
						msg_format(_("緑色の小人が%sへ襲いかかった！", "The little green men attack %s!"),m_name);
						project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,(PROJECT_JUMP|PROJECT_KILL),-1);
						flag = TRUE;
						if(p_ptr->is_dead) break;
					}
				}
				if(!flag) msg_print(_("しかし小人たちは何もせず消えた。", "They disappear without doing anything."));

			}
		}
		break;

	case 21:
		if (name) return _("ドッペルゲンガー", "Doppelganger");
		if (desc) return _("このフロア限定で友好的なドッペルゲンガーが近くに現れる。ドッペルゲンガーのパラメータは全くランダム。ドッペルゲンガーにプレイヤーが直接触れると大変なことになる。詠唱に失敗した場合敵対的なドッペルゲンガーが現れる。",
                            "Summons a friendly Doppelganger to this floor. Doppelganger's parameters are completely random. Bad things will hapen if you touch the doppelganger. If you fail to cast this, a hostile doppelganger will appear.");
		{

			if (info) return "";

			if (cast | fail)
			{
				u32b mode;
				int r_idx;
				int i;
				int ty,tx;
				bool flag1 = TRUE;
				bool flag2 = TRUE;
				bool flag3 = TRUE;
				int attempt;
				//開いているランダムユニークを使う
				msg_print(_("何か胸がざわつくような気配を感じる...", "You feel a presence that makes your chest tighten..."));
				for(i=1;i<m_max;i++)
				{
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_1) flag1 = FALSE;
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_2) flag2 = FALSE;
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_3) flag3 = FALSE;

					if(r_info[m_list[i].r_idx].flags7 & RF7_DOPPELGANGER)
					{
						flag1 = FALSE;
						flag2 = FALSE;
						flag3 = FALSE;
						break;
					}
				}
				if(flag1)
					r_idx = MON_RANDOM_UNIQUE_1;
				else if(flag2)
					r_idx = MON_RANDOM_UNIQUE_2;
				else if(flag3)
					r_idx = MON_RANDOM_UNIQUE_3;
				else
				{
					msg_print(_("...気のせいだったようだ。", "Looks like that was your imagination."));
					break;
				}
				if(cast) mode = PM_FORCE_FRIENDLY;
				else mode = PM_NO_PET;
				for(attempt = 1000;attempt > 0;attempt--)
				{
					scatter(&ty,&tx,py,px,MAX_RANGE,0);
					if(distance(ty,tx,py,px) >= 10) break;
				}
				if(!attempt)
				{
					msg_print(_("...気のせいだったようだ。", "Looks like that was your imagination."));
					break;
				}

				flag_generate_doppelganger = TRUE;

				if(!summon_named_creature(0,ty,tx,r_idx,mode))
				{
					msg_print(_("...気のせいだったようだ。", "Looks like that was your imagination."));
				}

				flag_generate_doppelganger = FALSE;
			}
		}
		break;

	case 22:
		if (name) return _("黒服の男達", "Men in Black");
		if (desc) return _("周囲の敵モンスターがフロアから消える。ユニークモンスターは消えずにテレポートする。詠唱に失敗すると自分が記憶消去されフロアから追放される。クエストダンジョンでは使えない。",
                            "Removes nearby enemies from the floor. Unique monsters are teleported away instead. If you fail to cast this spell, you will lose your memories and get teleported to another floor. Cannot be used in quests.");
		{
			int i;
			int rad = MAX_SIGHT;
			if (info) return info_range(rad);

			if (cast)
			{

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print(_("ここでは使えない。", "You cannot use this here."));
					return NULL;
				}


				msg_print(_("黒服の男達が慌ただしく散って行った...", "Men in black swiftly scatter about..."));
				//v1.1.32 別ルーチンに置き換えた
				mass_genocide_3(rad, FALSE, FALSE);
#if 0
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];

					if (!m_ptr->r_idx) continue;
					if(is_pet(m_ptr)) continue;
					if(is_friendly(m_ptr)) continue;
					if (i == p_ptr->riding) continue;
					if(m_ptr->cdis > rad) continue;
					if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2) )
					{
						teleport_away(i,100,TELEPORT_PASSIVE);
					}
					else
					{

						if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
						{
							char m_name[80];

							monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
							do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
						}
						delete_monster_idx(i);
					}
				}
#endif
			}
			else if(fail)
			{
				if(p_ptr->blind) msg_print(_("複数の足音と共に異様な気配に包まれた・・", "You hear many footsteps and some extraordinary presence surrounding you..."));
				else msg_print(_("あなたは黒服の男達に取り囲まれた！", "You are surrounded by the men in black!"));

				if(lose_all_info()) msg_print(_("何かの処置をされた。何をされたのか記憶が曖昧だ・・",
                                                "Something has happened. But your memory is very vague..."));
				teleport_level(0);
			}
		}
		break;



	case 23:
		if (name) return _("月面超文明の襲来", "Attack of the Lunarians");
		if (desc) return _("ターゲットの周囲に分解属性の球が多数落ちて来る。夜でないと使用できない。",
                            "Drops multiple disintegration balls near the target. Can be used only at night.");

		{
			int dam = plev * 3 ;
			int rad = 3;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				if(is_daytime())
				{
					msg_print(_("何も起こらなかった。", "Nothing happens."));
					break;
				}
				if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			}
		}
		break;


	case 24:
		if (name) return _("エイボンの霧の車輪", "Wheel of Eibon");
		if (desc) return _("使用すると「幻想郷の外から来たモンスター」から認識されなくなる。ただし待機・休憩・飲食・装備変更などを除く行動をしたり何らかの原因でダメージを受けると効果が切れる。",
                            "Prevents monsters from outside Gensoukyou from perceiving you. The effect stops if you perform any action aside from waiting, resting, eating or swapping equipment, or if you take damage for any reason.");
		{

			if (info) return "";

			if (cast)
			{
				msg_print(_("青い霧の筒が現れ、異界の者の目からあなたの姿を隠した...",
                            "A cylinder of blue mist appears, hiding your presence from beings of other worlds..."));
				p_ptr->special_defense |= (SD_EIBON_WHEEL);
				break_eibon_flag = FALSE;
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		break;

	case 25:
		if (name) return _("スレイマンの塵", "Dust of Suleiman");
		if (desc) return _("隣接する「幻想郷の外から来たモンスター」一体に対し大ダメージを与える。ただしユニーク・モンスターに対しては半分の効果しか発揮されない。アイテム「石桜」をひとつ消費する。",
                            "Deals large damage to an adjacent monster from outside Gensoukyou. Damage is halved against unique monsters. Consumes one 'Ishizakura'.");
		{
			int dam;
			int dice;
			int sides;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			char m_name[80];
			int x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;

			dice = plev;
			sides = 50;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				cptr q,s;
				int item;

				item_tester_hook = item_tester_hook_ishizakura;
				q = _("どの触媒を使いますか? ", "Use what as a catalyst?");
				s = _("あなたは触媒となる石桜を持っていない。", "You don't have ishizakura to use as a catalyst.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];
				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There is nothing there."));
					return NULL;
				}

				//攻撃処理の前に触媒を減らす。
				//そうしないとこの攻撃で「跳ねる火の球」などを倒して爆発でアイテムが壊れた場合減るアイテムがずれるかもしれない
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

				monster_desc(m_name, m_ptr, 0);
				msg_format(_("輝く粉が%sを包み込んだ...", "%s is surrounded by glittering particles..."),m_name);

				r_ptr = &r_info[m_ptr->r_idx];
				if(r_ptr->flags3 & RF3_GEN_MASK)
				{
					msg_format(_("しかし効果がなかった。", "But nothing happens."));
					break;
				}
				dam = damroll(dice,sides);
				if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2) ) dam /= 2;

				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,flg,-1);

			}
		}
		break;


	case 26:

		if (name) return _("異界との接触", "Contact with Other Worlds");
		if (desc) return _("モンスターを一体配下として召喚する。詠唱に失敗したとき敵対的な状態で現れる。召喚されてくるモンスターは階層と地形によって変わる。",
                            "Summons a monster. If you fail to cast this spell, the summoned monster will be hostile. Summoned monster depends on dungeon level and terrain.");

		{
			if (cast || fail)
			{
				int i,j;
				int lev = dun_level;
				int table_idx = 99;
				u32b mode;

				if(cast) msg_print(_("あなたは書物に記された特別な儀式を執り行った。", "You perform the special ritual described in the book."));
				//自分と周囲のグリッドループ
				for(i=1;i<=9;i++)
				{
					int tx = px+ddx[i];
					int ty = py+ddy[i];

					if(!in_bounds(ty,tx)) continue;
					//各グリッドに対し、クトゥルフ系召喚テーブルの各行と条件一致判定
					for(j=0;occult_contact_table[j].r_idx;j++)
					{
						//テーブルの若い行優先
						if(j >= table_idx) break;
						//featflag1,2,3(not),r_info.levelの条件を満たせば召喚idxを記録
						if(occult_contact_table[j].featflag1 && !cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag1)) continue;
						if(occult_contact_table[j].featflag2 && !cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag2)) continue;
						if(occult_contact_table[j].featflag3 &&  cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag3)) continue;
						if(r_info[occult_contact_table[j].r_idx].level > lev+randint1(10)) continue;

						table_idx = j;
						break;
					}
				}
				if(table_idx == 99)
				{
					msg_print(_("しかし何も現れなかった。", "However, nobody appeared."));
					break;
				}
				if(cast)
				{
					mode = PM_FORCE_PET;
				}
				else	mode = PM_NO_PET;

				if(summon_named_creature(0,py,px,occult_contact_table[table_idx].r_idx,mode))
				{
#ifdef JP
					if(cast)
						msg_format("%s%sが現れ、あなたに従った。",occult_contact_table[table_idx].feat_desc,(r_name+r_info[occult_contact_table[table_idx].r_idx].name));
					else
						msg_format("%s%sが現れ、あなたに襲い掛かってきた！",occult_contact_table[table_idx].feat_desc,(r_name+r_info[occult_contact_table[table_idx].r_idx].name));
#else
                    if(cast)
						msg_format("%s comes forth %s to serve you.",(r_name+r_info[occult_contact_table[table_idx].r_idx].name), occult_contact_table[table_idx].feat_desc);
					else
						msg_format("%s comes forth %s and turns on you!",(r_name+r_info[occult_contact_table[table_idx].r_idx].name), occult_contact_table[table_idx].feat_desc);
#endif
				}
				else
				{
					msg_print(_("しかし何も現れなかった。", "However, nobody appeared."));
				}


			}
		}
		break;


	case 27:
		if (name) return _("異界への退散", "Banish to Other Worlds");
		if (desc) return _("「幻想郷の外から来たモンスター」一体をフロアから追放する。モンスターのレベルの倍(ユニークモンスターは四倍)のMPを追加で消費し、MPが足りていれば必ず追放に成功する。クエストダンジョンでは使えず、クエストのターゲットモンスターには効果がない。MPが足りなかった場合追放に失敗したうえ彫像化を受ける。",
                            "Banishes a monster from outside of Gensoukyou from the floor. Consumes extra MP equal to double the level of the monster (or quadruple if it's unique); banishment is guaranteed to succeed if extra cost is paid. Cannot be used in quest and doesn't affect quest target monsters. If you don't have enough MP and banishment fails, you get paralysed regardless of free action.");
		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			int j;
			char m_name[80];
			int x,y;
			bool flag_ok = TRUE;

			if (cast)
			{
				int use_mana;

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print(_("ここでは使えない。", "You can't use this here."));
					return NULL;
				}


				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print(_("視界内のターゲットを明示的に指定しないといけない。",
                                "You have to pick a target in sight."));
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!m_ptr->r_idx || !m_ptr->ml)
				{
					msg_format(_("そこには何もいない。", "There's nobody here."));
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(r_ptr->flags3 & RF3_GEN_MASK)
				{
					msg_format(_("%sには効果がなかった。ここの住人のようだ。",
                                "%s was unaffected. That's a resident of this world."),m_name);
					break;
				}
				if((r_ptr->flags1 & (RF1_QUESTOR)))
				{
					msg_format(_("%sには効果がなかった。", "%s was unaffected."),m_name);
					break;
				}
				use_mana = r_ptr->level * 2;
				if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_UNIQUE2) ) use_mana *= 2;


				if(use_mana <= p_ptr->csp)
				{
					msg_format(_("%sを元いた世界へと送還した。", "%s is sent back to their home world."),m_name);
					delete_monster_idx(cave[y][x].m_idx);

				}
				else
				{
					msg_format(_("%sは思っていたより強大だ！追放する前に精神力を使い果たしてしまった！",
                                "%s is more powerful than you thought! Your mental power gives out before you complete the banishment!"),m_name);
					(void)set_monster_csleep(cave[y][x].m_idx, 0);
					set_paralyzed(p_ptr->paralyzed + (use_mana - p_ptr->csp)/10 + randint1(3));
				}

				p_ptr->csp -= use_mana;
				if(p_ptr->csp < 0) p_ptr->csp = 0;


			}
		}
		break;


	case 28:
		if (name) return _("黄金の蜂蜜酒の製造", "Create Golden Mead");
		if (desc) return _("「黄金の蜂蜜酒」を製造する。アイテム「*啓蒙*の薬」「竜の爪」「高草郡の光る竹」「ミラクルフルーツ」「一夜のクシナダ」のいずれか一つを消費する。",
                            "Creates Golden Mead. Uses either a Potion of *Enlightenment*, Shining bamboo of Takakusa or Overnight Kushinada as material.");
		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_make_space_mead;
				q = _("どれを材料にしますか? ", "Use what item as material?");
				s = _("あなたは書物に記された材料を持っていない。", "You don't have the materials listed in the book.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

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

				msg_print(_("あなたはまるで何かの儀式のような調合作業を始めた・・", "You started a mixing process that looks more like a ritual..."));
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_GOLDEN_MEAD));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				q_ptr->discount = 90;
				(void)drop_near(q_ptr, -1, py, px);
			}
		}
		break;


	case 29:
		if (name) return _("イシスの封印", "Seal of Isis");
		if (desc) return _("周囲の壁が永久壁に変化する。使用時と失敗時に狂気に襲われる。中にモンスターがいる壁には何も起こらない。クエストダンジョンでは使えない。",
                            "Turns walls close to you into permanent walls. Causes insanity (even if spellcasting fails). Does not affect walls with monsters in them. Cannot be used in quests.");
		{
			int xx,yy;
			bool flag = FALSE;
			int i;

			if (info) return "";

			if (cast)
			{

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print(_("ここでは使えない。", "You can't use it here."));
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
				if(flag) msg_print(_("周囲の壁に強力な守護が宿った。", "Powerful protection resides in walls around you."));
			}
			if(cast || fail)
			{
				sanity_blast(0,TRUE);
			}

		}
		break;
	case 30:
		if (name) return _("旧神の印", "Sign of Elder Gods");
		if (desc) return _("アイテム「伊弉諾物質」か「龍珠」をひとつ消費し、床に「旧神の印」を設置する。印は近くの混沌の勢力のモンスターによる召喚魔法を阻害し、混沌の勢力のモンスターが印の場所に踏み込むことを高確率で妨害する。妨害に失敗して印が破壊されることがあるが、そのとき印は破邪属性の爆発を起こす。",
                            "Uses an Izanagi Object or Dragon Orb to place Sign of Elder Gods on the floor. The sign interferes with summoning magic of chaotically aligned monsters close to it, and has a high chance to prevent chaotically aligned monsters from moving on it. If it fails and the sign is broken, it causes an explosion of holy energy.");
		{

			if (info) return "";

			if (cast)
			{
				cptr q,s;
				int item;

				if (!cave_clean_bold(py, px))
				{
					msg_print(_("ここでは使えない。", "You can't use it here."));
					return NULL;
				}

				item_tester_hook = item_tester_hook_izanagi_object;
				q = _("どれを使いますか? ", "Use what item?");
				s = _("あなたは必要な物品を持っていない。", "You don't have required items.");
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

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

				msg_print(_("古き印を発動した。", "You activate an Elder Sign."));
				cave[py][px].info |= CAVE_OBJECT;
				cave[py][px].mimic = feat_elder_sign;
				note_spot(py, px);
				lite_spot(py, px);

			}
		}
		break;

	case 31:
		if (name) return _("ヨグ＝ソトートの一撃", "Strike of Yog-Sothoth");
		if (desc) return _("ターゲットの位置に半径不定の強力な因果混乱の球を発生させる。使用者は呪文の成否にかかわらず50～200ダメージの狂気攻撃を受ける。詠唱に失敗するか敵対的な『ヨグ＝ソトート』がフロアにいるとき、『ヨグ＝ソトート』がプレイヤーに襲い掛かってくる。",
                            "Generates a powerful nexus ball of unkown radius on target location. Regardless of success, you take 50-200 damage and suffer insanity. If the spellcasting fails or if hostile Yog-Sothoth is present, Yog-Sothoth attacks you.");
		{
			int base = 500;
			int sides = 999;
			int rad;

			if (info) return info_damage(1, sides, base);

			if (cast || fail)
			{
				bool tmp_m_idx = 0;
				int i;

				if(cast)
				{
					msg_print(_("あなたは「全てにして一つのもの」との接触を試みた...", "You attempt to contact the All-in-One..."));
					project(PROJECT_WHO_CONTACT_YOG,0,py,px,50+randint1(150),GF_COSMIC_HORROR,PROJECT_KILL,-1);
				}
				if(fail)
				{
					msg_print(_("あなたは「全てにして一つのもの」の侵蝕を受けた！", "You are assaulted by the All-in-One!"));
					project(PROJECT_WHO_CONTACT_YOG,0,py,px,200,GF_COSMIC_HORROR,PROJECT_KILL,-1);
				}

				if(!p_ptr->csp || p_ptr->is_dead || p_ptr->confused || p_ptr->image || p_ptr->paralyzed) fail = TRUE;

				for(i=1;i<m_max;i++) if(m_list[i].r_idx == MON_YOG_SOTHOTH && is_hostile(&m_list[i])) tmp_m_idx = i;

				if(tmp_m_idx)
				{
					teleport_monster_to(tmp_m_idx,py,px,100,0L);
					(void)set_monster_csleep(tmp_m_idx, 0);
					msg_print(_("無数のきらめく球体がこちらへ飛んできた！", "Countless scintillating spheres fly forth!"));
				}
				else if(fail)
				{
					summon_named_creature(0,py,px,MON_YOG_SOTHOTH,PM_NO_PET);
				}
				else
				{
					cptr yog_desc;
					int roll = randint1(sides);
					if (!get_aim_dir(&dir)) return NULL;
					rad = roll / 100;

					if(rad < 2) yog_desc = _("奇妙な球が上から落ちてきた。", "A weird sphere descends from above.");
					else if(rad < 5) yog_desc = _("きらめく巨大な球が降ってきた。", "A giant shining sphere drops down.");
					else yog_desc = _("途方もなく巨大な球体がダンジョンにめり込んだ！",
                                        "A tremendously large sphere sinks into the dungeon!");

					if(!fire_ball_jump(GF_NEXUS, dir, base+roll, rad, yog_desc)) return NULL;

				}
			}
		}
		break;

	}

	return "";
}














/*
 * Do everything for each spell
 */
/*:::戻り値:呪文や技名の文字列へのポインタが返る*/
/*:::魔法を詠唱する。成功か失敗か判定済み。領域ごとに分岐。*/
///realm 魔法領域ごとに効果発生へ分岐
cptr do_spell(int realm, int spell, int mode)
{
	switch (realm)
	{
		/*
	case REALM_LIFE:     return do_life_spell(spell, mode);
	case REALM_SORCERY:  return do_sorcery_spell(spell, mode);
	case REALM_NATURE:   return do_nature_spell(spell, mode);
	case REALM_CHAOS:    return do_chaos_spell(spell, mode);
	case REALM_DEATH:    return do_death_spell(spell, mode);
	case REALM_TRUMP:    return do_trump_spell(spell, mode);
	case REALM_ARCANE:   return do_arcane_spell(spell, mode);
	case REALM_CRAFT:    return do_craft_spell(spell, mode);
	case REALM_DAEMON:   return do_daemon_spell(spell, mode);
	case REALM_CRUSADE:  return do_crusade_spell(spell, mode);
	case REALM_MUSIC:    return do_music_spell(spell, mode);
	case REALM_HISSATSU: return do_hissatsu_spell(spell, mode);
	case REALM_HEX:      return do_hex_spell(spell, mode);
		*/
	case TV_BOOK_ELEMENT:
		return do_new_spell_element(spell,mode);
	case TV_BOOK_CHAOS:
		return do_new_spell_chaos(spell,mode);
	case TV_BOOK_FORESEE:
		return do_new_spell_foresee(spell,mode);
	case TV_BOOK_ENCHANT:
		return do_new_spell_enchant(spell,mode);
	case TV_BOOK_NATURE:
		return do_new_spell_nature(spell,mode);
	case TV_BOOK_NECROMANCY:
		return do_new_spell_necromancy(spell,mode);
	case TV_BOOK_LIFE:
		return do_new_spell_life(spell,mode);
	case TV_BOOK_TRANSFORM:
		return do_new_spell_transform(spell,mode);
	case TV_BOOK_DARKNESS:
		return do_new_spell_darkness(spell,mode);
	case TV_BOOK_SUMMONS:
		return do_new_spell_summon(spell,mode);
	case TV_BOOK_MYSTIC:
		return do_new_spell_mystic(spell,mode);
	case TV_BOOK_PUNISH:
		return do_new_spell_punish(spell,mode);
	case TV_BOOK_MUSIC://歌唱職用の特殊処理
		if(p_ptr->pclass != CLASS_BARD) return do_new_spell_music2(spell,mode);
		else
		{
			msg_format(_("ERROR:この職業の歌特技が登録されていない",
                        "ERROR: Songs are undefined for this class."));
			return NULL;
		}

	case TV_BOOK_HISSATSU:
		return do_hissatsu_spell(spell, mode);
	case TV_BOOK_OCCULT:
		return do_new_spell_occult(spell,mode);

	default:
		msg_print(_("この領域は未実装だ。", "This realm is unimplemented."));
		return NULL;

	}

	return NULL;
}
