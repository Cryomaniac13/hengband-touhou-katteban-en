/***この下、モンスター魔法関連****************************************/


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
	if(xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO) //アイテムカードのヤマビコ用
#ifdef JP
		msg_format("あなたは「%s」を%s",monspell_list2[num].name,msg);
#else
        msg_format("You %s '%s'!",msg, monspell_list2[num].name);
#endif
	else if(p_ptr->pclass == CLASS_YUKARI && monspell_list2[num].attack_spell)
		msg_format(_("スキマから%sが飛び出した！", "%s flies out of your gap!"),monspell_list2[num].name,msg);
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		cptr mname;
		int r_idx = p_ptr->magic_num1[20];

		//v1.1.30 ランダムユニークの名前処理追加
		if(IS_RANDOM_UNIQUE_IDX(r_idx))
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
cptr cast_monspell_new_aux(int num, bool only_info, bool fail, int xtra)
{
	int cast_lev;
	int cast_hp;
	int plev = p_ptr->lev;
	bool powerful=FALSE;
	cptr msg;
	int dir;
	int dam,base,dice,sides;

	if(xtra == CAST_MONSPELL_EXTRA_KYOUKO_YAMABIKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = _("オウム返しした！", "reverberate");
	}

	else if(p_ptr->pclass == CLASS_SATORI)
	{
		cast_lev = p_ptr->lev * 3 / 2;
		cast_hp = p_ptr->chp;
		if(p_ptr->lev > 44) powerful = TRUE;
		msg = _("想起した！", "recollect");
	}
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		cast_lev = r_ptr->level;
		cast_hp = m_ptr->hp;
		if (r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = _("使わせた！", "use");
	}
	else if(p_ptr->pclass == CLASS_KYOUKO)
	{
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp;
		msg = _("オウム返しした！", "reverberate");
	}
	else if(p_ptr->pclass == CLASS_KOKORO)
	{
		object_type *o_ptr = &inventory[INVEN_HEAD];

		if(o_ptr->tval != TV_MASK)
		{
			msg_print(_("ERROR:面を装備していない状態でcast_monspell_new_aux()が呼ばれている",
                        "ERROR: cast_monspell_new_aux() called while not wearing a mask"));
			return NULL;
		}

		cast_lev = o_ptr->discount;
		cast_hp = MAX(10,(o_ptr->xtra5 / 6));
		if(have_flag(o_ptr->art_flags,TR_SPECIAL_KOKORO)) powerful = TRUE;
		msg = _("再演した！", "reproduce");
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		//xtra値に判定レベルが入る。POWERFULのとき+100されている。
		if(xtra > 99) powerful = TRUE;
		cast_lev = xtra % 100;
		cast_hp = p_ptr->chp;
		msg = _("発動した！", "activate");
	}
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		if(p_ptr->lev > 44) powerful = TRUE;
		cast_lev = p_ptr->lev;
		cast_hp = p_ptr->chp * 2;
		msg = _("行使した！", "use"); //攻撃魔法のときメッセージ例外処理あり
	}
	else if(IS_METAMORPHOSIS)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		cast_lev = r_ptr->level;
		cast_hp = p_ptr->chp;
		if(r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
		msg = _("使った！", "use");

	}
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		int r_idx = p_ptr->magic_num1[20];
		int mon_hp;
		monster_race *r_ptr;

		if(!r_idx)
		{
			msg_print(_("ERROR:依姫cast_monspell_new_auxでr_idxが設定されていない",
                        "ERROR: r_idx not set during Yorihime's cast_monspell_aux"));
			if(only_info) return format("");
			return NULL;
		}
		r_ptr = &r_info[r_idx];
		if(r_ptr->flags1 & RF1_FORCE_MAXHP)
			cast_hp = r_ptr->hdice * r_ptr->hside;
		else
			cast_hp = r_ptr->hdice * (r_ptr->hside+1)/2;

		cast_lev = r_ptr->level;
		if(r_ptr->flags2 & RF2_POWERFUL) powerful = TRUE;
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
		cast_lev = p_ptr->lev *2;
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
		if(only_info) return format("");
		return NULL;
	}



	switch(num)
	{

	case 1://RF4_SHRIEK
	case 128: //RF9_ALARM
		{
			if(only_info) return format("");
			if(fail) return "";

			//msg_format("あなたは「%s」を%s",monspell_list2[num].name,msg);
			msg_cast_monspell_new(num, msg, xtra);

			if( num==1) aggravate_monsters(0,FALSE);
			else aggravate_monsters(0,TRUE);
		}
		break;
	case 2://RF4_DANMAKU
		{
			dam = cast_lev;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
	case 3: //RF4_DISPEL
		{
			int m_idx;

			if(only_info) return format("");
			if(fail) return "";

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
			if(dam>800) dam=800;

			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";
			if (!get_aim_dir(&dir)) return NULL;

			msg_cast_monspell_new(num, msg, xtra);
			fire_rocket(GF_ROCKET, dir, dam, 2);
		}
		break;
	case 5://RF4_SHOOT
		{
			if(IS_METAMORPHOSIS)
			{
				dice = r_info[MON_EXTRA_FIELD].blow[0].d_dice;
				sides = r_info[MON_EXTRA_FIELD].blow[0].d_side;
				if(only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);

				dam = damroll(dice,sides);
			}
			else
			{
				dam = cast_lev * 3 / 2;
				if(only_info) return format(_("損傷：%d", "dam: %d"),dam);

			}
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
	case 6: //RF4_BR_HOLY
	case 7: //RF4_BR_HELL
		{
			int typ;
			if(num==6) typ = GF_HOLY_FIRE;
			if(num==7) typ = GF_HELL_FIRE;
			dam = cast_hp / 6;
			if(dam>500) dam=500;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 8: //RF4_BR_AQUA
		{
			dam = cast_hp / 8;
			if(dam>350) dam=350;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

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
			if(num==9) typ = GF_ACID;
			if(num==10) typ = GF_ELEC;
			if(num==11) typ = GF_FIRE;
			if(num==12) typ = GF_COLD;

			dam = cast_hp / 3;
			if(dam>1600) dam=1600;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 13: //RF4_BR_POIS
		{
			dam = cast_hp / 3;
			if(dam>800) dam=800;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 14: //RF4_BR_NETH
		{
			int typ;
			if(num==14) typ = GF_NETHER;

			dam = cast_hp / 6;
			if(dam>550) dam=550;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;


	case 15: //RF4_BR_LITE
	case 16: //RF4_BR_DARK
		{
			int typ;
			if(num==15) typ = GF_LITE;
			if(num==16) typ = GF_DARK;

			dam = cast_hp / 6;
			if(dam>400) dam=400;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;

	case 17: //RF4_WAVEMOTION
		{
			dam = cast_hp / 6;
			if(dam>555) dam=555;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_DISINTEGRATE,dir,dam,2);
		}
		break;
	case 18: //RF4_BR_SOUN
		{
			int typ;
			if(num==18) typ = GF_SOUND;

			dam = cast_hp / 6;
			if(dam>450) dam=450;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 19: //RF4_BR_CHAO
		{
			int typ;
			if(num==19) typ = GF_CHAOS;

			dam = cast_hp / 6;
			if(dam>600) dam=600;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 20: //RF4_BR_DISE
	case 25: //RF4_BR_SHAR
		{
			int typ;
			if(num==20) typ = GF_DISENCHANT;
			if(num==25) typ = GF_SHARDS;

			dam = cast_hp / 6;
			if(dam>500) dam=500;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 21: //RF4_BR_NEXU
		{
			int typ;
			if(num==21) typ = GF_NEXUS;

			dam = cast_hp / 7;
			if(dam>600) dam=600;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 22: //RF4_BR_TIME
		{
			int typ;
			if(num==22) typ = GF_TIME;

			dam = cast_hp / 3;
			if(dam>150) dam=150;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 23: //RF4_BR_INACT
		{
			int typ;
			if(num==23) typ = GF_INACT;

			dam = cast_hp / 6;
			if(dam>200) dam=200;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 24: //RF4_BR_GRAV
		{
			int typ;
			if(num==24) typ = GF_GRAVITY;

			dam = cast_hp / 3;
			if(dam>200) dam=200;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 26: //RF4_BR_PLAS
		{
			int typ;
			if(num==26) typ = GF_PLASMA;

			dam = cast_hp / 4;
			if(dam>1200) dam=1200;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 27: //RF4_BA_FORCE
		{
			if(powerful)
			{
				dam = cast_hp / 4;
				if(dam>350) dam=350;
			}
			else
			{
				dam = cast_hp / 6;
				if(dam>250) dam=250;
			}

			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_FORCE, dir, dam, 2);
		}
		break;
	case 28: //RF4_BR_MANA
		{
			int typ;
			if(num==28) typ = GF_MANA;

			dam = cast_hp / 3;
			if(dam>250) dam=250;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

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
			if(num==30) typ = GF_NUKE;

			dam = cast_hp / 4;
			if(dam>1000) dam=1000;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 31: //RF4_BA_CHAO
		{
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_CHAOS, dir, dam, 4);
		}
		break;
	case 32: //RF4_BR_DISI
		{
			int typ = GF_DISINTEGRATE;

			dam = cast_hp / 6;
			if(dam>150) dam=150;
			if(only_info) return format(_("損傷：%d", "dam: %d"),dam);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, (plev > 40 ? -3 : -2));
		}
		break;
	case 33: //RF5_BA_ACID
		{
			int rad;
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_ACID, dir, dam, 4);
		}
		break;

	case 34: //RF5_BA_ELEC
		{
			int rad;
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_ELEC, dir, dam, 4);
		}
		break;

	case 35: //RF5_BA_FIRE
		{
			int rad;
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_FIRE, dir, dam, 4);
		}
		break;
	case 36: //RF5_BA_COLD
		{
			int rad;
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = base + damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_COLD, dir, dam, 4);
		}
		break;
	case 37: //RF5_BA_POIS
		{
			int rad = 2;
			if(powerful)
			{
				dice = 24;
				sides = 2;
			}
			else
			{
				dice = 12;
				sides = 2;
			}

			if(only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides);

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, 4);
		}
		break;
	case 38: //RF5_BA_NETH
		{
			int rad = 2;
			if(powerful)
			{
				base = 50 + cast_lev * 2;
				dice = 10;
				sides = 10;
			}
			else
			{
				base = 50 + cast_lev ;
				dice = 10;
				sides = 10;
			}

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_NETHER, dir, dam, 4);
		}
		break;
	case 39: //RF5_BA_WATE
		{
			int rad = 4;
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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

			if(num==40) typ = GF_MANA;
			else if(num==41) typ = GF_DARK;
			else if(num==53) typ = GF_LITE;
			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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

			base = MAX(1,cast_lev/2);
			dice = 1;
			sides = MAX(2,cast_lev/2);

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);

			dam = damroll(dice, sides) + base;
			//fire_ball_hide(GF_DRAIN_MANA, dir, dam, 0);
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
			if(num==43)
			{
				dice = 7;
				sides = 7;
				typ = GF_MIND_BLAST;
			}
			else if(num==44)
			{
				dice = 12;
				sides = 12;
				typ = GF_BRAIN_SMASH;
			}
			else if(num==45)
			{
				dice = 3;
				sides = 8;
				typ = GF_CAUSE_1;
			}
			else if(num==46)
			{
				dice = 8;
				sides = 8;
				typ = GF_CAUSE_2;
			}
			else if(num==47)
			{
				dice = 10;
				sides = 15;
				typ = GF_CAUSE_3;
			}
			else if(num==48)
			{
				dice = 15;
				sides = 15;
				typ = GF_CAUSE_4;
			}
			else if(num==107)
			{
				dice = 3;
				sides = 8;
				typ = GF_PUNISH_1;
			}
			else if(num==108)
			{
				dice = 8;
				sides = 8;
				typ = GF_PUNISH_2;
			}
			else if(num==109)
			{
				dice = 10;
				sides = 15;
				typ = GF_PUNISH_3;
			}
			else if(num==110)
			{
				dice = 15;
				sides = 15;
				typ = GF_PUNISH_4;
			}
			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format(_("損傷：%dd%d", "dam: %dd%d"),dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides);

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

			if(num==49)
			{
				dice = 7;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_ACID;
			}
			else if(num==50)
			{
				dice = 4;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_ELEC;
			}
			else if(num==51)
			{
				dice = 9;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_FIRE;
			}
			else if(num==52)
			{
				dice = 6;
				sides = 8;
				base = cast_lev / 3;
				if(powerful) base *= 2;
				typ = GF_COLD;
			}
			else if(num==54)
			{
				dice = 5;
				sides = 5;
				base = cast_lev * 4;
				if(powerful) base /= 2;
				else base /= 3;
				typ = GF_NETHER;
			}
			else if(num==55)
			{
				dice = 10;
				sides = 10;
				base = cast_lev * 3;
				if(powerful) base /= 2;
				else base /= 3;
				typ = GF_WATER;
			}
			else if(num==56)
			{
				dice = 1;
				sides = cast_lev * 7 / 2;
				base = 50;
				typ = GF_MANA;
			}
			else if(num==57)
			{
				dice = 1;
				sides = 200;
				if(powerful) base = 100 + cast_lev * 6;
				else  base = 100 + cast_lev * 3;
				typ = GF_PLASMA;
			}
			else if(num==58)
			{
				dice = 6;
				sides = 6;
				base = cast_lev * 3;
				if(powerful) base /= 2;
				else  base /= 3;
				typ = GF_ICE;
			}
			else if(num==59)
			{
				dice = 2;
				sides = 6;
				base = cast_lev / 3 + 1;
				typ = GF_MISSILE;
			}
			else if(num==111)
			{
				dice = 10;
				sides = 10;
				if(powerful) base = cast_lev * 3 / 2;
				else base = cast_lev;
				typ = GF_HOLY_FIRE;
			}
			else if(num==113)
			{
				dice = 3;
				if(powerful) dice = 10;
				sides = 10;
				if(powerful) base = cast_lev * 3 / 2;
				else base = cast_lev;
				typ = GF_SOUND;
			}


			else {msg_print("ERROR:typ"); return NULL;}

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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

			if(num == 60 || num == 63 || num == 64) power = cast_lev;
			else power = cast_lev * 2;

			if(only_info) return format(_("効力：%d", "pow: %d"),power);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);

			if(num==60) fear_monster(dir, power);
			else if(num==61) confuse_monster(dir, power);
			else if(num==62) confuse_monster(dir, power);
			else if(num==63) slow_monster(dir, power);
			else if(num==64) sleep_monster(dir, power);
			else fire_ball_hide(GF_GENOCIDE, dir, power, 0);

		}
		break;
	case 65: //RF6_HASTE
		{
			int time = 100;

			if(only_info) return format(_("期間：%d", "dur: %d"),time);
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);

			(void)set_fast(time, FALSE);
		}
		break;
	case 66: //RF6_HAND_DOOM
		{
			int power = cast_lev * 3;

			if(only_info) return format(_("効力：%d", "pow: %d"),power);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_hide(GF_HAND_DOOM, dir, power, 0);


		}
		break;
	case 67: //RF6_HEAL
		{
			int power = cast_lev * 6;

			if(only_info) return format(_("回復：%d", "heal: %d"),power);
			if(fail) return "";

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
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);

			(void)set_invuln(randint1(base) + base, FALSE);
		}
		break;
	case 69: //RF6_BLINK
	case 70: //RF6_TPORT
		{
			int dist;

			if(num==69) dist = 10;
			else dist = MAX_SIGHT * 2 + 5;

			if(only_info) return format(_("距離：%d", "dist: %d"),dist);
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			teleport_player(dist, 0L);

		}
		break;
	case 71: //RF6_WORLD
		{

			if(only_info) return format(" ");
			if(fail) return "";
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
				p_ptr->energy_need -= 1000 + (400)*TURNS_PER_TICK/10;
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
			if(only_info) return format(" ");
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			if(!teleport_back(&idx_dummy, 0L)) return NULL;

		}
		break;
	case 74: //RF6_TELE_AWAY
		{
			int dist = 100;
			if(only_info) return format(_("距離：%d", "dist: %d"),dist);
			if(fail) return "";

			if (!get_aim_dir(&dir)) return FALSE;
			msg_cast_monspell_new(num, msg, xtra);
			(void)fire_beam(GF_AWAY_ALL, dir, dist);


		}
		break;
	case 75: //RF6_TELE_LEVEL?
		{

			if(only_info) return format(_("未実装", "unimplemented"));
			if(fail) return "";
			msg_format(_("ERROR:この技は未実装のはず：RF6_TELE_LEVEL",
                        "ERROR: This ability is unimplemented: RF6_TELE_LEVEL"));
			return NULL;
		}
		break;
	case 76: //RF5_PSY_SPEAR
		{
			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			(void)fire_beam(GF_PSY_SPEAR, dir, dam);
		}
		break;
	case 77: //RF6_DARKNESS　←ライト・エリアにする
		{
			if(only_info) return format(_("効果：照明", "eff: illuminate"));
			if(fail) return "";
			msg_cast_monspell_new(num, msg, xtra);
			(void)lite_area(0, 3);
		}
		break;
	case 78: //RF6_TRAP
		{
			if(only_info) return format("");
			if(fail) return "";
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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
			if(only_info) return format("");

			if(fail) mode = (PM_NO_PET | PM_ALLOW_GROUP);
			else mode = PM_FORCE_PET;

			if(!fail) msg_cast_monspell_new(num, msg, xtra);

			if(num == 81) summon = summon_kin_player(cast_lev, py, px, mode);
			if(num == 82) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_CYBER, mode);
			if(num == 83) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode);
			if(num == 84) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, 0, mode)) summon = TRUE;
			if(num == 85) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANT, (mode | PM_ALLOW_GROUP));
			if(num == 86)  for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_SPIDER, mode | PM_ALLOW_GROUP)) summon = TRUE;
			if(num == 87) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HOUND, (mode | PM_ALLOW_GROUP));
			if(num == 88) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HYDRA, (mode));
			if(num == 89) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANGEL, (mode));
			if(num == 90) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEMON, (mode));
			if(num == 91) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNDEAD, (mode));
			if(num == 92) summon = summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DRAGON, (mode));
			if(num == 93) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_UNDEAD, mode)) summon = TRUE;
			if(num == 94) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DRAGON, mode)) summon = TRUE;
			if(num == 95) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE))) summon = TRUE;
			if(num == 96) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 114) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_ANIMAL, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 122) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_DEITY, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 123) for(cnt=0;cnt<4;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_HI_DEMON, (mode | PM_ALLOW_GROUP))) summon = TRUE;
			if(num == 124) for(cnt=0;cnt<6;cnt++) if(summon_specific((fail ? 0 : -1), py, px, cast_lev, SUMMON_KWAI, (mode | PM_ALLOW_GROUP))) summon = TRUE;



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

			if(num==97) typ = GF_FIRE;
			if(num==98) typ = GF_COLD;
			if(num==99) typ = GF_ELEC;
			if(num==100) typ = GF_ACID;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(typ, dir, dam, rad);
		}
		break;
	case 101: //RF9_TOXIC_STORM
		{
			int rad = 4;
			int typ;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POIS, dir, dam, rad);
		}
		break;
	case 102: //RF9_BA_POLLUTE
		{
			int rad = 4;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_POLLUTE, dir, dam, rad);
		}
		break;
	case 103: //RF9_BA_DISI
		{
			int rad = 5;

			if(powerful)
			{
				base = 50 + cast_lev * 2;
				dice = 1;
				sides = 50;
			}
			else
			{
				base = 25 + cast_lev ;
				dice = 1;
				sides = 25;
			}

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_DISINTEGRATE, dir, dam, rad);
		}
		break;
	case 104: //RF9_BA_HOLY
		{
			int rad = 2;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball(GF_HOLY_FIRE, dir, dam, rad);
		}
		break;
	case 105: //RF9_METEOR
		{
			int rad;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_METEOR, dir, dam, rad,NULL);
		}
		break;
	case 106: //RF9_DISTORTION
		{
			int rad = 4;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_DISTORTION, dir, dam, rad,NULL);
		}
		break;
	case 112: //RF9_GIGANTIC_LASER
		{
			int rad;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_NUKE, dir, dam, rad);
		}
		break;

	case 115: //RF9_BEAM_LITE
		{
			if(powerful)
				base = cast_lev * 4;
			else
				base = cast_lev * 2;

			if(only_info) return format(_("損傷：%d", "dam: %d"),base);
			if(fail) return "";
			dam =  base;

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
			if(num == 117) typ = GF_HELL_FIRE;
			else typ = GF_HOLY_FIRE;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_spark(GF_DISINTEGRATE, dir, dam, 2);
		}
		break;
	case 120: //RF9_TORNADO
		{
			int rad = 5;

			if(powerful)
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

			if(only_info) return format(_("損傷：%d+%dd%d", "dam: %d+%dd%d"),base,dice,sides);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

			if (!get_aim_dir(&dir)) return NULL;
			msg_cast_monspell_new(num, msg, xtra);
			fire_ball_jump(GF_TORNADO, dir, dam, rad,NULL);
		}
		break;
	case 121: //RF9_DESTRUCTION
		{
			base = 7;
			sides = 5;
			if(only_info) return format(_("範囲：%d+1d%d", "rad: %d+1d%d"),base,sides);
			if(fail) return "";

			msg_cast_monspell_new(num, msg, xtra);
			destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);

		}
		break;
	case 125: //RF9_TELE_APPROACH
		{
			if(only_info) return format("");
			if(fail) return "";
			if (!target_set(TARGET_KILL)) return NULL;
			if (!target_okay()|| !projectable(target_row,target_col,py,px))
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
			if(only_info) return format("");
			if(fail) return "";

			if(target_who > 0 && m_list[target_who].r_idx)
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

			if(powerful)
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

			if(only_info) return format(_("損傷：～%d+%dd%d", "dam: ~%d+%dd%d"),base/2,dice,sides/2);
			if(fail) return "";
			dam = damroll(dice,sides) + base;

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
int calc_monspell_cast_mem(void)
{
	int num=0;
	//モンスター変身系
	if (IS_METAMORPHOSIS) num = 20;
	//さとり
	else if(p_ptr->pclass == CLASS_SATORI) num = 3 + p_ptr->lev / 7;
	//こころ
	else if(p_ptr->pclass == CLASS_KOKORO) num = 2 + (p_ptr->lev-1) / 13;
	//雷鼓
	else if(p_ptr->pclass == CLASS_RAIKO) num = 20;
	//隠岐奈
	else if (p_ptr->pclass == CLASS_OKINA) num = NEW_MSPELL_LIST_MAX;
	//デフォ20
	else num = 20;

	return num;
}

/*:::さとり用ものまね準備ルーチン 視界内の敵を走査し、monspell_list2[]用の添え字を最大10までp_ptr->magic_num1[]に格納する*/
void make_magic_list_satori(bool del)
{
	int i,shift;
	int mlist_num = 0;
	int memory;

	if(p_ptr->pclass != CLASS_SATORI) return;

	memory = calc_monspell_cast_mem();

	for(i=0;i<10;i++) p_ptr->magic_num1[i] = 0; //常に最大の10個分クリア
	if(del) return;

	//全モンスター走査
	for (i = 1; i < m_max; i++)
	{
		u32b rflags;
		int j;
		int new_num;
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		if (!m_ptr->r_idx) continue;
		//視界内の精神のある敵を抽出

		if(!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;
		if(!m_ptr->ml) continue;
		if(r_ptr->flags2 & RF2_EMPTY_MIND) continue;

		//WEIRD_MINDの敵は1/2の確率でしか読めない
		if((r_ptr->flags2 & RF2_WEIRD_MIND) && (turn % 2)) continue;

		//敵の持っている魔法を抽出しmonspell_list2[]の添字番号に変換してmagic_num1[]に追加　記憶可能数を超えたら適当に入れ替える
		for(j=0;j<4;j++)
		{
			int spell;
			if(j==0) rflags = r_ptr->flags4;
			else if(j==1) rflags = r_ptr->flags5;
			else if(j==2) rflags = r_ptr->flags6;
			else rflags = r_ptr->flags9;

			for(shift=0;shift<32;shift++)
			{
				bool exist = FALSE;
				int k;
				if((rflags >> shift) % 2 == 0) continue;
				spell = shift+1 + j * 32;

				if(spell < 1 || spell > 128)
				{
					msg_format(_("ERROR:make_magic_satori()で不正なspell値(%d)が出た",
                                "ERROR: Incorrect spell value (%d) in make_magic_satori()"),spell);
					return;
				}
				if(!monspell_list2[spell].level) continue; //特別な行動や未実装の技は非対象


				//既にある魔法はパス
				for(k=0;k<10;k++) if(spell == p_ptr->magic_num1[k]) exist = TRUE;
				if(exist) continue;


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
	monster_type *m_ptr;
	monster_race *r_ptr;
	u32b rlags;

	if (p_ptr->pclass != CLASS_SATONO) return;

	memory = calc_monspell_cast_mem();
	for (i = 0; i<memory; i++) p_ptr->magic_num1[i] = 0; //リストのクリア

	if (!p_ptr->riding) return;
	m_ptr = &m_list[p_ptr->riding];
	if (!m_ptr->r_idx) { msg_format(_("ERROR:ridingの値がおかしい(%d)",
                                        "ERROR: Weird riding value (%d)"), p_ptr->riding); return; }
	r_ptr = &r_info[m_ptr->r_idx];

	//モンスターの持っている魔法を抽出しmonspell_list2[]の添字番号に変換してmagic_num1[]に追加する。記憶可能数を超えたら適当に入れ替える
	for (j = 0; j<4; j++)
	{
		u32b rflags;
		int spell, shift;
		//魔法のみを対象にする
		if (j == 0) rflags = r_ptr->flags4 & ~(RF4_NOMAGIC_MASK);
		else if (j == 1) rflags = r_ptr->flags5 & ~(RF5_NOMAGIC_MASK);
		else if (j == 2) rflags = r_ptr->flags6 & ~(RF6_NOMAGIC_MASK);
		else rflags = r_ptr->flags9 & ~(RF9_NOMAGIC_MASK);

		for (shift = 0; shift<32; shift++)
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
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int hand;
	int power, rank;
	int magic_num = 10;
	int i;

	for(i=10;i<30;i++) p_ptr->magic_num1[i] = 0; //魔法リストを20個クリア
	p_ptr->magic_num1[30] = 0;

	//両手ループ
	for(hand = 0;hand < 2;hand++)
	{
		int magic_num_one = 0; //今の武器から得た魔法の数 10になったら終了
		if(hand == 1) p_ptr->magic_num1[30] = (magic_num-10);

		o_ptr = &inventory[INVEN_RARM+hand];

		//通常武器かランダムアーティファクトのみ
		if(!o_ptr->k_idx || !object_is_melee_weapon(o_ptr) || object_is_fixed_artifact(o_ptr)) continue;
		if(o_ptr->xtra3 < 50) continue;
		object_flags(o_ptr, flgs);

		//武器のランクを五段階に決める
		power = k_info[o_ptr->k_idx].level;
		if(object_is_artifact(o_ptr)) power += 50;
		else if(object_is_ego(o_ptr)) power += e_info[o_ptr->name2].rarity;//e_info.levelは機能していないのでrarityを使う
		power += flag_cost(o_ptr,o_ptr->pval,FALSE) / 1000;

		if(power < 50) rank=0;
		else if(power < 100) rank=1;
		else if(power < 150) rank = 2;
		else rank = 3;

		if(o_ptr->xtra1 == 100) rank += 1;//「完全に使いこなせる」ものは1ランクアップ


		//以後、武器のフラグとランクによって使用可能な魔法を判定しp_ptr->magic_num1[11-29]に格納　武器片方ごとに最大10　
		//格納される値はmonspell_list2[]の添字

		//弾幕はなし？
		//波動砲
		if((have_flag(flgs, TR_VORPAL) || have_flag(flgs, TR_EX_VORPAL)) && have_flag(flgs, TR_FORCE_WEAPON) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 17;
			if(++magic_num_one == 10) continue;
		}

		//魔力の嵐
		if((o_ptr->name2 == EGO_WEAPON_HERO || have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 3
		|| (o_ptr->name2 == EGO_WEAPON_FORCE && rank > 2))
		{
			p_ptr->magic_num1[magic_num++] = 40;
			if(++magic_num_one == 10) continue;
		}
		//暗黒の嵐
		if((o_ptr->name2 == EGO_WEAPON_VAMP || have_flag(flgs, TR_RES_DARK) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) ) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 41;
			if(++magic_num_one == 10) continue;
		}
		//閃光の嵐
		if((o_ptr->name2 == EGO_WEAPON_HAKUREI || have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) ) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 53;
			if(++magic_num_one == 10) continue;
		}
		//ヘルファイア
		if(have_flag(flgs, TR_KILL_GOOD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 117;
			if(++magic_num_one == 10) continue;
		}
		//ホーリーファイア
		if(have_flag(flgs, TR_KILL_EVIL) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 118;
			if(++magic_num_one == 10) continue;
		}

		//汚染の球
		if(have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 102;
			if(++magic_num_one == 10) continue;
		}

		//原子分解
		if((have_flag(flgs, TR_VORPAL) && rank > 3 || have_flag(flgs, TR_EX_VORPAL))&& (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) )
		{
			p_ptr->magic_num1[magic_num++] = 103;
			if(++magic_num_one == 10) continue;
		}
		//破邪の光球
		if((have_flag(flgs, TR_SLAY_EVIL) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 104;
			if(++magic_num_one == 10) continue;
		}
		//メテオストライク
		if(have_flag(flgs, TR_IMPACT) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 105;
			if(++magic_num_one == 10) continue;
		}
		//空間歪曲
		if((have_flag(flgs, TR_TELEPORT) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| (have_flag(flgs, TR_RES_TIME) &&  (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 ))
		{
			p_ptr->magic_num1[magic_num++] = 106;
			if(++magic_num_one == 10) continue;
		}
		//メイルシュトロム
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 ||
		have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 127;
			if(++magic_num_one == 10) continue;
		}

		//レーザー
		if(have_flag(flgs, TR_RES_LITE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1
		|| have_flag(flgs, TR_VORPAL) && rank > 2 ||  have_flag(flgs, TR_EX_VORPAL))
		{
			p_ptr->magic_num1[magic_num++] = 115;
			if(++magic_num_one == 10) continue;
		}
		//大型レーザー
		if((have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 2 )
		|| have_flag(flgs, TR_IM_FIRE) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 112;
			if(++magic_num_one == 10) continue;
		}
		//光の剣
		if(have_flag(flgs, TR_FORCE_WEAPON)  && rank > 2
		 ||have_flag(flgs, TR_EX_VORPAL)  && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 76;
			if(++magic_num_one == 10) continue;
		}

		//気弾
		if((have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_STR) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
		  ||(o_ptr->name2 == EGO_WEAPON_ONI && rank > 2 ))
		{
			p_ptr->magic_num1[magic_num++] = 27;
			if(++magic_num_one == 10) continue;
		}


		//魔力消去
		if(have_flag(flgs, TR_EX_VORPAL) || have_flag(flgs, TR_VORPAL) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 3;
			if(++magic_num_one == 10) continue;
		}
		//魔力吸収
		if(have_flag(flgs, TR_VAMPIRIC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_CHR)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 42;
			if(++magic_num_one == 10) continue;
		}
		//加速
		if(o_ptr->name2 == EGO_WEAPON_EXATTACK && rank > 1
		|| o_ptr->name2 == EGO_WEAPON_HUNTER && rank > 2
		|| have_flag(flgs, TR_SPEEDSTER) && rank > 1
		|| have_flag(flgs, TR_SPEED) && o_ptr->pval > 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 65;
			if(++magic_num_one == 10) continue;
		}
		//破滅の手
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0
		|| (o_ptr->curse_flags & TRC_PERMA_CURSE)
		|| (o_ptr->curse_flags & TRC_ADD_H_CURSE ) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 66;
			if(++magic_num_one == 10) continue;
		}
		//治癒
		if((have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_CON) && rank > 2)
		  ||(have_flag(flgs, TR_CON) && have_flag(flgs, TR_WIS) && rank > 3)
		  ||(have_flag(flgs, TR_REGEN) && have_flag(flgs, TR_WIS) && rank > 3)
		  ||(o_ptr->name2 == EGO_WEAPON_DEFENDER && rank > 2)
		  ||(o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1))
		{
			p_ptr->magic_num1[magic_num++] = 67;
			if(++magic_num_one == 10) continue;
		}
		//無敵化
		if((o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 3)
		|| have_flag(flgs, TR_BLESSED) && o_ptr->to_a > 5 && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 68;
			if(++magic_num_one == 10) continue;
		}
		//ショート・テレポート
		if(have_flag(flgs, TR_TELEPORT) || have_flag(flgs, TR_LEVITATION) || have_flag(flgs, TR_SPEEDSTER))
		{
			p_ptr->magic_num1[magic_num++] = 69;
			if(++magic_num_one == 10) continue;
		}
		//テレポート
		if(have_flag(flgs, TR_TELEPORT) && rank > 0
		|| have_flag(flgs, TR_RES_CHAOS)
		|| have_flag(flgs, TR_RES_TIME) )
		{
			p_ptr->magic_num1[magic_num++] = 70;
			if(++magic_num_one == 10) continue;
		}
		//ザ・ワールド
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_SPEEDSTER) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 71;
			if(++magic_num_one == 10) continue;
		}
		//テレポバック
		if(have_flag(flgs, TR_NO_TELE) && rank > 0 || have_flag(flgs, TR_RIDING) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 73;
			if(++magic_num_one == 10) continue;
		}
		//テレポアウェイ
		if(have_flag(flgs, TR_NO_TELE) && rank > 1 || have_flag(flgs, TR_BLESSED) && rank > 3 || o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 74;
			if(++magic_num_one == 10) continue;
		}
		//隣接テレポ
		if(o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 1 || have_flag(flgs, TR_SPEEDSTER) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 125;
			if(++magic_num_one == 10) continue;
		}
		//視界外隣接テレポ
		if(have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 126;
			if(++magic_num_one == 10) continue;
		}
		//ライトエリア
		if(have_flag(flgs, TR_LITE) )
		{
			p_ptr->magic_num1[magic_num++] = 77;
			if(++magic_num_one == 10) continue;
		}

		//*破壊*
		if(have_flag(flgs, TR_CHAOTIC) && rank > 3 ||  have_flag(flgs, TR_IMPACT) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 121;
			if(++magic_num_one == 10) continue;
		}
		//炎の嵐
		if(have_flag(flgs, TR_BRAND_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
		|| have_flag(flgs, TR_IM_FIRE) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 97;
			if(++magic_num_one == 10) continue;
		}
		//氷の嵐
		if(have_flag(flgs, TR_BRAND_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
		|| have_flag(flgs, TR_IM_COLD) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 98;
			if(++magic_num_one == 10) continue;
		}
		//雷の嵐
		if(have_flag(flgs, TR_BRAND_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
		|| have_flag(flgs, TR_IM_ELEC) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 99;
			if(++magic_num_one == 10) continue;
		}
		//酸の嵐
		if(have_flag(flgs, TR_BRAND_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3
		|| have_flag(flgs, TR_IM_ACID) &&  rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 100;
			if(++magic_num_one == 10) continue;
		}
		//毒素の嵐
		if(have_flag(flgs, TR_BRAND_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 101;
			if(++magic_num_one == 10) continue;
		}

		//地獄球
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 1 ||
		have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 38;
			if(++magic_num_one == 10) continue;
		}
		//ウォーター・ボール
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank == 3 ||  o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank >= 2
		|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 39;
			if(++magic_num_one == 10) continue;
		}
		//純ログルス
		if(o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 1 ||
		have_flag(flgs, TR_RES_CHAOS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 31;
			if(++magic_num_one == 10) continue;
		}
		//竜巻
		if(o_ptr->name2 == EGO_WEAPON_FUJIN && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 120;
			if(++magic_num_one == 10) continue;
		}

		//地獄の矢
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON ||
		have_flag(flgs, TR_RES_NETHER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 54;
			if(++magic_num_one == 10) continue;
		}
		//ウォーター・ボルト
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank >= 1 ||  o_ptr->name2 == EGO_WEAPON_RYU_JIN && rank == 1
		|| have_flag(flgs, TR_RES_WATER) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 55;
			if(++magic_num_one == 10) continue;
		}
		//魔力の矢
		if((o_ptr->name2 == EGO_WEAPON_HERO || o_ptr->name2 == EGO_WEAPON_FORCE ||
		have_flag(flgs, TR_INT) && have_flag(flgs, TR_WIS) || have_flag(flgs, TR_EASY_SPELL)) && rank > 1 ||
		have_flag(flgs, TR_THROW) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 56;
			if(++magic_num_one == 10) continue;
		}
		//プラズマボルト
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEM || have_flag(flgs, TR_IM_FIRE) || have_flag(flgs, TR_IM_ELEC) ||
		have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 1 )
		{
			p_ptr->magic_num1[magic_num++] = 57;
			if(++magic_num_one == 10) continue;
		}
		//極寒の矢
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 1 || have_flag(flgs, TR_IM_COLD) ||
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS)) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 58;
			if(++magic_num_one == 10) continue;
		}

		//聖なる矢
		if(o_ptr->name2 == EGO_WEAPON_HAKUREI  || have_flag(flgs, TR_RES_HOLY) && rank > 2 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) )
		{
			p_ptr->magic_num1[magic_num++] = 111;
			if(++magic_num_one == 10) continue;
		}

		//♪のボルト
		if(have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank > 1
		|| have_flag(flgs, TR_AGGRAVATE) && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 113;
			if(++magic_num_one == 10) continue;
		}
		//精神攻撃
		if(have_flag(flgs, TR_CHAOTIC) && rank > 0 && rank < 3 )
		{
			p_ptr->magic_num1[magic_num++] = 43;
			if(++magic_num_one == 10) continue;
		}
		//脳攻撃
		if(have_flag(flgs, TR_CHAOTIC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 44;
			if(++magic_num_one == 10) continue;
		}
		//軽傷の呪い
		if(o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 45;
			if(++magic_num_one == 10) continue;
		}
		//重傷の呪い
		if(o_ptr->name2 == EGO_WEAPON_GHOST)
		{
			p_ptr->magic_num1[magic_num++] = 46;
			if(++magic_num_one == 10) continue;
		}
		//致命傷の呪い
		if((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 47;
			if(++magic_num_one == 10) continue;
		}
		//死の言霊
		if((o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON || o_ptr->name2 == EGO_WEAPON_MURDERER) && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 48;
			if(++magic_num_one == 10) continue;
		}
		//コズミック・ホラー
		if(have_flag(flgs, TR_RES_INSANITY) && rank > 3 || have_flag(flgs, TR_CHR) && o_ptr->pval < -4 )
		{
			p_ptr->magic_num1[magic_num++] = 80;
			if(++magic_num_one == 10) continue;
		}


		//魔除けのまじない
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 1)
		{
			p_ptr->magic_num1[magic_num++] = 107;
			if(++magic_num_one == 10) continue;
		}
		//聖なる言葉
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 108;
			if(++magic_num_one == 10) continue;
		}
		//退魔の呪文
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank == 3)
		{
			p_ptr->magic_num1[magic_num++] = 109;
			if(++magic_num_one == 10) continue;
		}
		//破邪の印
		if((o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER || o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI || o_ptr->name2 == EGO_WEAPON_DEMONSLAY) && rank > 3
		|| o_ptr->name2 == EGO_WEAPON_HAKUREI && rank < 3 || have_flag(flgs, TR_RES_HOLY) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 110;
			if(++magic_num_one == 10) continue;
		}

		//アシッド・ボール
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank >= 2  ||
		have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 33;
			if(++magic_num_one == 10) continue;
		}
		//サンダー・ボール
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank >= 2  ||
		have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 34;
			if(++magic_num_one == 10) continue;
		}
		//ファイア・ボール
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank >= 2  ||
		have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 35;
			if(++magic_num_one == 10) continue;
		}
		//アイス・ボール
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank >= 2  ||
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 2 )
		{
			p_ptr->magic_num1[magic_num++] = 36;
			if(++magic_num_one == 10) continue;
		}
		//アシッド・ボルト
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank == 1  ||
		have_flag(flgs, TR_RES_ACID) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 49;
			if(++magic_num_one == 10) continue;
		}
		//サンダー・ボルト
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank == 1  ||
		have_flag(flgs, TR_RES_ELEC) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 50;
			if(++magic_num_one == 10) continue;
		}
		//ファイア・ボルト
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank == 1 ||
		have_flag(flgs, TR_RES_FIRE) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 51;
			if(++magic_num_one == 10) continue;
		}
		//アイス・ボルト
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank == 1  ||
		have_flag(flgs, TR_RES_COLD) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank == 1 )
		{
			p_ptr->magic_num1[magic_num++] = 52;
			if(++magic_num_one == 10) continue;
		}
		//悪臭雲
		if(o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank <= 2  ||
		have_flag(flgs, TR_RES_POIS) && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_DEX)) && rank <= 2 )
		{
			p_ptr->magic_num1[magic_num++] = 37;
			if(++magic_num_one == 10) continue;
		}


		//救援召喚
		if(have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 3 || o_ptr->name2 == EGO_WEAPON_HERO && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 81;
			if(++magic_num_one == 10) continue;
		}
		//サイバーU
		if(have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_SLAY_DEMON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 82;
			if(++magic_num_one == 10) continue;
		}
		//モンスター一体召喚
		if(have_flag(flgs, TR_CHR) && o_ptr->pval > 3 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 83;
			if(++magic_num_one == 10) continue;
		}
		//モンスター複数召喚
		if(have_flag(flgs, TR_TELEPATHY) && have_flag(flgs, TR_TELEPORT))
		{
			p_ptr->magic_num1[magic_num++] = 84;
			if(++magic_num_one == 10) continue;
		}
		//アリ召喚
		if(o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank < 2)
		{
			p_ptr->magic_num1[magic_num++] = 85;
			if(++magic_num_one == 10) continue;
		}
		//虫召喚
		if(o_ptr->name2 == EGO_WEAPON_HUNTER || have_flag(flgs, TR_SLAY_ANIMAL) && rank == 2)
		{
			p_ptr->magic_num1[magic_num++] = 86;
			if(++magic_num_one == 10) continue;
		}
		//ハウンド召喚
		if((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL) )&& rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 87;
			if(++magic_num_one == 10) continue;
		}
		//ヒドラ召喚
		if(have_flag(flgs, TR_KILL_ANIMAL) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 88;
			if(++magic_num_one == 10) continue;
		}
		//天使召喚
		if(have_flag(flgs, TR_KILL_GOOD) && rank > 2 || have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_GOOD) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 89;
			if(++magic_num_one == 10) continue;
		}
		//悪魔召喚
		if(have_flag(flgs, TR_SLAY_DEMON) && rank > 2 ||
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 90;
			if(++magic_num_one == 10) continue;
		}
		//アンデッド召喚
		if(have_flag(flgs, TR_SLAY_UNDEAD) && rank > 2 ||
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_UNDEAD) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 91;
			if(++magic_num_one == 10) continue;
		}
		//ドラゴン召喚
		if(have_flag(flgs, TR_SLAY_DRAGON) && rank > 2 ||
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_DRAGON) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 92;
			if(++magic_num_one == 10) continue;
		}
		//上級アンデッド召喚
		if(have_flag(flgs, TR_KILL_UNDEAD) && rank > 3 ||
		have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_UNDEAD) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 93;
			if(++magic_num_one == 10) continue;
		}
		//古代ドラゴン召喚
		if(have_flag(flgs, TR_KILL_DRAGON) && rank > 3 ||
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DRAGON) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 94;
			if(++magic_num_one == 10) continue;
		}
		//アンバライト召喚
		if((have_flag(flgs, TR_KILL_HUMAN) && rank > 3 ||
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_HUMAN) && rank > 3)
		&& have_flag(flgs, TR_RES_TIME) )
		{
			p_ptr->magic_num1[magic_num++] = 95;
			if(++magic_num_one == 10) continue;
		}
		//ユニークモンスター召喚
		if(have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_UNIQUE) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 96;
			if(++magic_num_one == 10) continue;
		}

		//動物召喚
		if((have_flag(flgs, TR_SLAY_ANIMAL) || have_flag(flgs, TR_ESP_ANIMAL) )&& rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 114;
			if(++magic_num_one == 10) continue;
		}
		//神格召喚
		if(have_flag(flgs, TR_KILL_DEITY) && rank > 1 ||
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEITY) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 122;
			if(++magic_num_one == 10) continue;
		}
		//上級悪魔召喚
		if(have_flag(flgs, TR_KILL_DEMON) && rank > 2 ||
		have_flag(flgs, TR_CHR) && have_flag(flgs, TR_ESP_DEMON) && rank > 3
		&& o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 0 )
		{
			p_ptr->magic_num1[magic_num++] = 123;
			if(++magic_num_one == 10) continue;
		}
		//妖怪召喚
		if(have_flag(flgs, TR_KILL_KWAI) && rank > 2 || have_flag(flgs, TR_CHR) &&have_flag(flgs, TR_ESP_KWAI) && rank > 1)
		{
			p_ptr->magic_num1[magic_num++] = 124;
			if(++magic_num_one == 10) continue;
		}

		//聖なるブレス
		if(have_flag(flgs, TR_BLESSED)  && rank > 3 || have_flag(flgs, TR_RES_HOLY) && have_flag(flgs, TR_WIS) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 6;
			if(++magic_num_one == 10) continue;
		}
		//地獄の劫火のブレス
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD && rank > 3 || have_flag(flgs, TR_KILL_GOOD) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 7;
			if(++magic_num_one == 10) continue;
		}

		//アクアブレス
		if(o_ptr->name2 == EGO_WEAPON_SUI_RYU && rank > 2 || have_flag(flgs, TR_RES_WATER) && have_flag(flgs, TR_CON) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 8;
			if(++magic_num_one == 10) continue;
		}
		//酸ブレス
		if(o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 2 || have_flag(flgs, TR_IM_ACID) ||
		have_flag(flgs, TR_RES_ACID) && have_flag(flgs, TR_CON) && rank > 3 || have_flag(flgs, TR_BRAND_ACID) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 9;
			if(++magic_num_one == 10) continue;
		}
		//電撃ブレス ちょっと得やすくする
		if(o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 1 || have_flag(flgs, TR_IM_ELEC) ||
		have_flag(flgs, TR_RES_ELEC) && have_flag(flgs, TR_DEX) && rank > 2 || have_flag(flgs, TR_BRAND_ELEC) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 10;
			if(++magic_num_one == 10) continue;
		}
		//火炎ブレス
		if(o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 2 || have_flag(flgs, TR_IM_FIRE) ||
		have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_STR) && rank > 3 || have_flag(flgs, TR_BRAND_FIRE) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 11;
			if(++magic_num_one == 10) continue;
		}
		//冷気ブレス
		if(o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 2 || have_flag(flgs, TR_IM_COLD) ||
		have_flag(flgs, TR_RES_COLD) && have_flag(flgs, TR_WIS) && rank > 3 || have_flag(flgs, TR_BRAND_COLD) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 12;
			if(++magic_num_one == 10) continue;
		}

		//毒ブレス
		if(o_ptr->name2 == EGO_WEAPON_BRANDPOIS && rank > 2 ||
		have_flag(flgs, TR_RES_POIS) && have_flag(flgs, TR_INT) && rank > 3 || have_flag(flgs, TR_BRAND_POIS) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 13;
			if(++magic_num_one == 10) continue;
		}

		//地獄ブレス
		if(o_ptr->name2 == EGO_WEAPON_DEMONLORD  || o_ptr->name2 == EGO_WEAPON_DEMON && rank > 2 ||
		have_flag(flgs, TR_RES_NETHER) && have_flag(flgs, TR_VAMPIRIC) && rank > 2 || have_flag(flgs, TR_KILL_HUMAN) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 14;
			if(++magic_num_one == 10) continue;
		}

		//閃光ブレス
		if(o_ptr->name2 == EGO_WEAPON_HAKUREI && rank > 1  || o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER && rank > 2 ||
		have_flag(flgs, TR_RES_LITE) && have_flag(flgs, TR_WIS) && rank > 2 || have_flag(flgs, TR_KILL_UNDEAD) && rank > 2 )
		{
			p_ptr->magic_num1[magic_num++] = 15;
			if(++magic_num_one == 10) continue;
		}
		//暗黒ブレス
		if(o_ptr->name2 == EGO_WEAPON_MURDERER && rank > 2 ||
		have_flag(flgs, TR_RES_DARK) && have_flag(flgs, TR_CHR) && rank > 2 || have_flag(flgs, TR_SLAY_GOOD) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 16;
			if(++magic_num_one == 10) continue;
		}
		//轟音ブレス
		if(o_ptr->name2 == EGO_WEAPON_DRAGONSLAY && rank > 2 ||
		have_flag(flgs, TR_RES_SOUND) && have_flag(flgs, TR_STR) && rank > 2 || have_flag(flgs, TR_SLAY_DRAGON) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 18;
			if(++magic_num_one == 10) continue;
		}
		//カオスブレス
		if(o_ptr->name2 == EGO_WEAPON_CHAOS && rank > 2 ||
		have_flag(flgs, TR_RES_CHAOS) && have_flag(flgs, TR_CON) && rank > 2 || have_flag(flgs, TR_CHAOTIC) && rank > 3 )
		{
			p_ptr->magic_num1[magic_num++] = 19;
			if(++magic_num_one == 10) continue;
		}
		//劣化ブレス
		if(o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 ||
		have_flag(flgs, TR_RES_DISEN) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 20;
			if(++magic_num_one == 10) continue;
		}
		//因果混乱ブレス
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_TELEPORT) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 21;
			if(++magic_num_one == 10) continue;
		}
		//時間逆転ブレス
		if(have_flag(flgs, TR_RES_TIME) && have_flag(flgs, TR_CHR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 22;
			if(++magic_num_one == 10) continue;
		}
		//遅鈍ブレス
		if(have_flag(flgs,TR_SPEED) && o_ptr->pval < 0 && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 23;
			if(++magic_num_one == 10) continue;
		}
		//重力ブレス
		if(o_ptr->name2 == EGO_WEAPON_QUAKE && rank > 1 || have_flag(flgs,TR_IMPACT) && have_flag(flgs, TR_STR) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 24;
			if(++magic_num_one == 10) continue;
		}
		//破片ブレス
		if(o_ptr->name2 == EGO_WEAPON_GODEATER && rank > 2 ||
		have_flag(flgs, TR_RES_SHARDS) && have_flag(flgs, TR_DEX) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 25;
			if(++magic_num_one == 10) continue;
		}
		//プラズマブレス
		if(have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_BRAND_ELEC) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 26;
			if(++magic_num_one == 10) continue;
		}
		//魔力ブレス
		if(have_flag(flgs, TR_FORCE_WEAPON) && have_flag(flgs, TR_CON) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 28;
			if(++magic_num_one == 10) continue;
		}
		//核熱ブレス
		if(have_flag(flgs, TR_BRAND_FIRE) && have_flag(flgs, TR_RES_LITE) && rank > 2)
		{
			p_ptr->magic_num1[magic_num++] = 30;
			if(++magic_num_one == 10) continue;
		}
		//分解ブレス
		if(have_flag(flgs, TR_EX_VORPAL) && (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON) ) && rank > 2
		|| have_flag(flgs, TR_TUNNEL) && o_ptr->pval > 2 &&  (have_flag(flgs, TR_STR) || have_flag(flgs, TR_CON) ) && rank > 3)
		{
			p_ptr->magic_num1[magic_num++] = 32;
			if(++magic_num_one == 10) continue;
		}


		//マジックミサイル
		if( rank >0 && (have_flag(flgs, TR_INT) || have_flag(flgs, TR_WIS) || have_flag(flgs, TR_SUST_INT) || have_flag(flgs, TR_SUST_WIS))
		|| have_flag(flgs, TR_THROW) )
		{
			p_ptr->magic_num1[magic_num++] = 59;
			if(++magic_num_one == 10) continue;
		}
		//恐慌
		if(o_ptr->name2 == EGO_WEAPON_GHOST || o_ptr->name2 == EGO_WEAPON_DEMON)
		{
			p_ptr->magic_num1[magic_num++] = 60;
			if(++magic_num_one == 10) continue;
		}
		//盲目
		if(o_ptr->name2 == EGO_WEAPON_MURDERER || o_ptr->name2 == EGO_WEAPON_BRANDACID && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 61;
			if(++magic_num_one == 10) continue;
		}
		//混乱
		if(o_ptr->name2 == EGO_WEAPON_CHAOS || o_ptr->name2 == EGO_WEAPON_BRANDFIRE && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 62;
			if(++magic_num_one == 10) continue;
		}
		//減速
		if(o_ptr->name2 == EGO_WEAPON_QUAKE || o_ptr->name2 == EGO_WEAPON_BRANDCOLD && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 63;
			if(++magic_num_one == 10) continue;
		}
		//麻痺
		if(o_ptr->name2 == EGO_WEAPON_VAMP || o_ptr->name2 == EGO_WEAPON_BRANDELEC && rank > 0)
		{
			p_ptr->magic_num1[magic_num++] = 64;
			if(++magic_num_one == 10) continue;
		}
		//トラップ
		if(have_flag(flgs, TR_AGGRAVATE) || o_ptr->curse_flags & TRC_ADD_H_CURSE || o_ptr->curse_flags & TRC_ADD_L_CURSE)
		{
			p_ptr->magic_num1[magic_num++] = 78;
			if(++magic_num_one == 10) continue;
		}
		//記憶消去
		if(have_flag(flgs, TR_STEALTH) && o_ptr->pval > 0)
		{
			p_ptr->magic_num1[magic_num++] = 79;
			if(++magic_num_one == 10) continue;
		}

		//叫び、警報
		if(have_flag(flgs, TR_AGGRAVATE) || have_flag(flgs, TR_STEALTH) && o_ptr->pval < 0)
		{
			if(rank > 1) p_ptr->magic_num1[magic_num++] = 128; //警報
			else p_ptr->magic_num1[magic_num++] = 1; //叫び
			if(++magic_num_one == 10) continue;
		}


	}
	return (magic_num-10);

}

//雷鼓技レベルを計算する。iはspell_list[]添字でp_ptr->magic_num1[30]には左手武器開始部のi値が入っているはず
//☆はPOWERFUL扱いのため+100
static int calc_raiko_spell_lev(int i)
{
	int skilllev;
	object_type *o_ptr;
	if(i < p_ptr->magic_num1[30]) o_ptr = &inventory[INVEN_RARM];
	else  o_ptr = &inventory[INVEN_LARM];

	skilllev = k_info[o_ptr->k_idx].level / 2 + p_ptr->lev;
	if(skilllev > 99) skilllev = 99;
	if(object_is_artifact(o_ptr)) skilllev += 100;

	return skilllev;

}


//里乃は独自に最低失敗率計算をする
static int satono_spell_minfail(void)
{
	monster_type *m_ptr = &m_list[p_ptr->riding];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

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

/*:::モンスターの魔法を詠唱するためのルーチン 行動順消費したときTRUEを返す*/
bool cast_monspell_new(void)
{

	int spell_list[NEW_MSPELL_LIST_MAX]; //使用可能魔法リスト monspell_list2[]の添字
	int memory=0; //モンスター魔法の記憶可能数
    int cnt=0; //実際のリストの長さ
	int spell_lev;
	cptr power_desc = "";

	int num; //技番号 0から開始
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

	/*:::各種変数やリストを設定*/
	for(i=0;i<NEW_MSPELL_LIST_MAX;i++) spell_list[i]=0; //とりあえず25箇所初期化
	memory = calc_monspell_cast_mem();
	if(!memory)
	{
		msg_print(_("ERROR:モンスター魔法の記憶可能数が0",
                    "ERROR: Monster spell memorization is 0"));
		return FALSE;
	}
	//モンスター変身時
	if(IS_METAMORPHOSIS)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		power_desc = _("使用", "use");

		for(i=0;i<=MAX_MONSPELLS2;i++)
		{
			if(monspell_list2[i].priority == 0) continue;

			if(i <= 32 && !(r_ptr->flags4 >> (i-1) & 1L)) continue;
			if(i > 32 && i <= 64 && !(r_ptr->flags5 >> (i-33) & 1L)) continue;
			if(i > 64 && i <= 96 && !(r_ptr->flags6 >> (i-65) & 1L)) continue;
			if(i > 96 && !(r_ptr->flags9 >> (i-97) & 1L)) continue;


			if(cnt < memory)
			{
				spell_list[cnt] = i;
				cnt++;
			}
			else //敵が20以上の特技を持っているとき、priority値を参考に一番優先度が低いのを消す
			{
				int change_num = 0;
				int j;

				for(j=memory-1;j>0;j--)
				{
					if(monspell_list2[spell_list[j]].priority > monspell_list2[spell_list[change_num]].priority) change_num = j;
				}
				if(monspell_list2[spell_list[change_num]].priority > monspell_list2[i].priority) spell_list[change_num] = i;

			}
		}
	}
	//さとり用初期化
	else if(p_ptr->pclass == CLASS_SATORI)
	{
		power_desc = _("想起", "recollect");
		for(i=0;i<memory;i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if(spell_list[i]) cnt++;
		}
	}
	//里乃用初期化
	else if (p_ptr->pclass == CLASS_SATONO)
	{
		power_desc = _("応援", "support");
		for (i = 0; i<memory; i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if (spell_list[i]) cnt++;
		}
	}
	//こころ用初期化
	else if(p_ptr->pclass == CLASS_KOKORO)
	{
		object_type *o_ptr = &inventory[INVEN_HEAD];

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
		for(i=0;i<memory;i++) if(spell_list[i]) cnt++;
	}
	//雷鼓用初期化
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		power_desc = _("発動", "activate");
		cnt = make_magic_list_aux_raiko();
		for(i=0;i<memory;i++)spell_list[i] = p_ptr->magic_num1[10+i];

	}
	//紫弾幕結界
	else if(p_ptr->pclass == CLASS_YUKARI)
	{
		const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/
		power_desc = _("使用", "use");

		for(i=1;i<=MAX_MONSPELLS2;i++)
		{
			if(i % 8 != turn / (A_DAY / 24) % 8 ) continue;
			if(!monspell_list2[i].level) continue;
			spell_list[cnt++] = i;
		}
	}
	//依姫降神
	else if(p_ptr->pclass == CLASS_YORIHIME)
	{
		power_desc = _("使用", "use");
		for(i=0;i<memory;i++)
		{
			spell_list[i] = p_ptr->magic_num1[i];
			if(spell_list[i]) cnt++;
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

	if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		msg_print(_("ダンジョンが魔力を吸収した！", "The dungeon absorbs magical energy!"));
		return FALSE;
	}
	if(p_ptr->anti_magic)
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
		//レベルが足りていない(直前に経験値減少攻撃受けてレベルが下がったなど)ときはリピートしても選択済みフラグが立たず選択し直し。
		//ただしドレミーと里乃はレベル制限無視
		//v1.1.94 モンスター変身中にもレベル制限無視
		if ( monspell_list2[num].level <=  p_ptr->lev || (IS_METAMORPHOSIS) || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_SATONO) flag_choice = TRUE;

		//依姫神降ろしは使う度に効果が変わるのでリピート不可
		if(p_ptr->pclass == CLASS_YORIHIME) flag_choice = FALSE;
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

	choice= (always_show_list ) ? ESCAPE:1;

	while (!flag_choice)
	{
		if(choice==ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice, TRUE) )break;
		/*:::メニュー表示用*/

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') )
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
					if(p_ptr->pclass == CLASS_SATORI) use_stat = A_INT;
					//里乃は常に魅力で技を使う。
					if (p_ptr->pclass == CLASS_SATONO) use_stat = A_CHR;

					//雷鼓はどちらの武器の技かでレベル判定が違う
					if(p_ptr->pclass == CLASS_RAIKO)
						xtra = calc_raiko_spell_lev(i);

					cost = monspell_list2[spell_list[i]].smana;
					if(cost > 0)
					{
						/*:::消費魔力減少 mod_need_manaに定義されたMANA_DIVとDEC_MANA_DIVの値を使用*/
						if (p_ptr->dec_mana) cost = cost * 3 / 4;

						if(cost < 1) cost = 1;
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
							//レベルが＠のレベルより高いと使えない。モンスター変身と里乃は例外
							if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS) && p_ptr->pclass != CLASS_SATONO)
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
					sprintf(comment, "%s", cast_monspell_new_aux(spell_list[i],TRUE, FALSE, xtra));

					/*:::カーソルまたはアルファベットの表示*/
					sprintf(psi_desc, "  %c) ",I2A(i));

					//v1.1.82b 弾幕研究家　未習得の特技は隠す
					if (p_ptr->pclass == CLASS_RESEARCHER && !check_monspell_learned(spell_list[i]))
					{
						flag_usable = FALSE;
						strcat(psi_desc, _("(未習得)", "(unknown)"));

					}
					//v2.0.6 尤魔　未所持のパワーは隠す
					else if(p_ptr->pclass == CLASS_YUMA && !p_ptr->magic_num1[spell_list[i]])
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
					if (p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO)
						flag_usable = FALSE;

					if(!flag_usable)
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
		/* Stop the loop */
		flag_choice = TRUE;
#ifdef ALLOW_REPEAT /* TNB */
		repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */

	}

	/* Restore the screen */
	if (flag_redraw ) screen_load();
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

	if(p_ptr->pclass == CLASS_RAIKO)
		xtra = calc_raiko_spell_lev(i);

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
	if(cost > 0)
	{
		if(p_ptr->dec_mana) cost = cost * 3 / 4;
		if(cost < 1) cost = 1;
	}

	//里乃は独自に失敗率計算をする
	if (p_ptr->pclass == CLASS_SATONO)
	{
		int s_minfail = satono_spell_minfail();
		if (chance < s_minfail) chance = s_minfail;
		cost = 0;
	}


	/*:::MPが足りない時は強制中断　どうせオプション使わないし処理めんどい*/
	if ( cost > p_ptr->csp)
	{
		msg_print(_("ＭＰが足りません。", "You don't have enough MP."));
		return FALSE;
	}
	///mod151001 ドレミー変身、里乃ダンス、紫苑憑依はレベルに関わらず技を使える
	if(p_ptr->lev < spell_lev && !(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME)) && p_ptr->pclass != CLASS_SATONO)
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
		cast_monspell_new_aux(num,FALSE,TRUE,xtra);
	}

	/*:::特技成功処理*/
	else
	{
		/*:::成功判定後にターゲット選択でキャンセルしたときなどにはcptrにNULLが返り、そのまま行動順消費せず終了する*/
		if(!cast_monspell_new_aux(num,FALSE,FALSE,xtra)) return FALSE;
	}

	p_ptr->csp -= cost;
	if (p_ptr->csp < 0)
	{
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;
	}

	//里乃は騎乗中モンスターが技を使ったという扱いなのでモンスターに一行動分行動遅延させる。
	//もしかしたら対象の爆発などで騎乗中モンスターが死んでるかもしれんのでp_ptr->ridingを条件式に入れておく。
	if (p_ptr->pclass == CLASS_SATONO && p_ptr->riding)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
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

	if(del)
	{
//		p_ptr->magic_num1[5]=0;
		monspell_yamabiko = 0;
		return;
	}
	if(num < 0 || num > 128)
	{
		msg_print(_("ERROR:kyouko_echo()で不正なthrown_spell値が渡された",
                    "ERROR: Incorrect thrown_spell value passed to kyouko_echo()"));
        return;
	}

	if(!monspell_list2[num].level || !monspell_list2[num].use_words) return; //使用不可の魔法、「言葉を使う」フラグのない魔法は非対象

	//p_ptr->magic_num1[5]=num;
	monspell_yamabiko = num;
	return;
}
