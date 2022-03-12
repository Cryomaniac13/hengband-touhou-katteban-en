/* File: xtra2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */

#include "angband.h"

#define REWARD_CHANCE 10


/*:::モンスターが＠のキャラクターかどうか判定　*/
/*:::同じキャラクターの場合モンスター名表示に(2P)とつけておくための処理*/
//v1.1.51 monster_hookから使うことになったので効率化のためにswitch文で書き直した
bool monster_is_you(s16b r_idx)
{
	int p = p_ptr->pclass;

	if(!cp_ptr->flag_only_unique) return FALSE;

	switch(p_ptr->pclass)
	{
		case CLASS_RUMIA:
			if (r_idx == MON_RUMIA) return TRUE;
			else return FALSE;

		case CLASS_YOUMU:
			if (r_idx == MON_YOUMU) return TRUE;
			else return FALSE;
		case CLASS_TEWI:
			if (r_idx == MON_TEWI) return TRUE;
			else return FALSE;
		case CLASS_UDONGE:
			if (r_idx == MON_UDONGE) return TRUE;
			else return FALSE;
		case CLASS_IKU:
			if (r_idx == MON_IKU) return TRUE;
			else return FALSE;

		case CLASS_KOMACHI:
			if (r_idx == MON_KOMACHI) return TRUE;
			else return FALSE;
		case CLASS_KOGASA:
			if (r_idx == MON_KOGASA) return TRUE;
			else return FALSE;
		case CLASS_KASEN:
			if (r_idx == MON_KASEN) return TRUE;
			else return FALSE;
		case CLASS_SUIKA:
			if (r_idx == MON_SUIKA) return TRUE;
			else return FALSE;
		case CLASS_WAKASAGI:
			if (r_idx == MON_WAKASAGI) return TRUE;
			else return FALSE;

		case CLASS_KOISHI:
			if (r_idx == MON_KOISHI) return TRUE;
			else return FALSE;
		case CLASS_MOMIZI:
			if (r_idx == MON_MOMIZI) return TRUE;
			else return FALSE;
		case CLASS_SEIGA:
			if (r_idx == MON_SEIGA) return TRUE;
			else return FALSE;
		case CLASS_CIRNO:
			if (r_idx == MON_CIRNO) return TRUE;
			else return FALSE;
		case CLASS_ORIN:
			if (r_idx == MON_ORIN) return TRUE;
			else return FALSE;
		case CLASS_SHINMYOUMARU:
		case CLASS_SHINMYOU_2:
			if (r_idx == MON_SHINMYOUMARU) return TRUE;
			else return FALSE;
		case CLASS_NAZRIN:
			if (r_idx == MON_NAZRIN) return TRUE;
			else return FALSE;
		case CLASS_LETTY:
			if (r_idx == MON_LETTY) return TRUE;
			else return FALSE;
		case CLASS_YOSHIKA:
			if (r_idx == MON_YOSHIKA) return TRUE;
			else if (r_idx == MON_SEIGA) return TRUE; //青娥が名前だけとはいえ出てくる特技があるので青娥も2P扱いにしておく
			else return FALSE;
		case CLASS_PATCHOULI:
			if (r_idx == MON_PATCHOULI) return TRUE;
			else return FALSE;

		case CLASS_AYA:
			if (r_idx == MON_AYA) return TRUE;
			else return FALSE;
		case CLASS_BANKI:
			if (r_idx == MON_SEKIBANKI) return TRUE;
			else return FALSE;
		case CLASS_MYSTIA:
			if (r_idx == MON_MYSTIA) return TRUE;
			else return FALSE;
		case CLASS_FLAN:
			if (r_idx == MON_F_SCARLET) return TRUE;
			else return FALSE;
		case CLASS_SHOU:
			if (r_idx == MON_SHOU) return TRUE;
			else return FALSE;

		case CLASS_MEDICINE:
			if (r_idx == MON_MEDICINE) return TRUE;
			else return FALSE;
		case CLASS_YUYUKO:
			if (r_idx == MON_YUYUKO) return TRUE;
			else return FALSE;
		case CLASS_SATORI:
			if (r_idx == MON_SATORI) return TRUE;
			else return FALSE;
		case CLASS_KYOUKO:
			if (r_idx == MON_KYOUKO) return TRUE;
			else return FALSE;
		case CLASS_TOZIKO:
			if (r_idx == MON_TOZIKO) return TRUE;
			else return FALSE;

		case CLASS_LILY_WHITE:
			if (r_idx == MON_LILY_WHITE) return TRUE;
			else return FALSE;
		case CLASS_KISUME:
			if (r_idx == MON_KISUME) return TRUE;
			else return FALSE;
		case CLASS_HATATE:
			if (r_idx == MON_HATATE) return TRUE;
			else return FALSE;
		case CLASS_MIKO:
			if (r_idx == MON_MIKO) return TRUE;
			else return FALSE;
		case CLASS_KOKORO:
			if (r_idx == MON_KOKORO) return TRUE;
			else return FALSE;

		case CLASS_WRIGGLE:
			if (r_idx == MON_WRIGGLE) return TRUE;
			else return FALSE;
		case CLASS_YUUKA:
			if (r_idx == MON_YUUKA) return TRUE;
			else return FALSE;
		case CLASS_CHEN:
			if (r_idx == MON_CHEN) return TRUE;
			else return FALSE;
		case CLASS_MURASA:
			if (r_idx == MON_MURASA) return TRUE;
			else return FALSE;
		case CLASS_KEINE:
			if (r_idx == MON_KEINE) return TRUE;
			else return FALSE;

		case CLASS_YUGI:
			if (r_idx == MON_YUGI) return TRUE;
			else return FALSE;
		case CLASS_REIMU:
			if (r_idx == MON_REIMU) return TRUE;
			else return FALSE;
		case CLASS_KAGEROU:
			if (r_idx == MON_KAGEROU) return TRUE;
			else return FALSE;
		case CLASS_SHIZUHA:
			if (r_idx == MON_SHIZUHA) return TRUE;
			else return FALSE;
		case CLASS_SANAE:
			if (r_idx == MON_SANAE) return TRUE;
			else return FALSE;

		case CLASS_MINORIKO:
			if (r_idx == MON_MINORIKO) return TRUE;
			else return FALSE;
		case CLASS_REMY:
			if (r_idx == MON_REMY) return TRUE;
			else return FALSE;
		case CLASS_NITORI:
			if (r_idx == MON_NITORI) return TRUE;
			else return FALSE;
		case CLASS_BYAKUREN:
			if (r_idx == MON_BYAKUREN) return TRUE;
			else return FALSE;
		case CLASS_NUE:
			if (r_idx == MON_NUE) return TRUE;
			else return FALSE;

		case CLASS_UTSUHO:
			if (r_idx == MON_UTSUHO) return TRUE;
			else return FALSE;
		case CLASS_YAMAME:
			if (r_idx == MON_YAMAME) return TRUE;
			else return FALSE;
		case CLASS_SUWAKO:
			if (r_idx == MON_SUWAKO) return TRUE;
			else return FALSE;
		case CLASS_RAN:
			if (r_idx == MON_RAN) return TRUE;
			else return FALSE;
		case CLASS_TENSHI:
			if (r_idx == MON_TENSHI) return TRUE;
			else return FALSE;

		case CLASS_MEIRIN:
			if (r_idx == MON_MEIRIN) return TRUE;
			else return FALSE;
		case CLASS_EIKI:
			if (r_idx == MON_EIKI) return TRUE;
			else return FALSE;
		case CLASS_PARSEE:
			if (r_idx == MON_PARSEE) return TRUE;
			else return FALSE;
		case CLASS_ICHIRIN:
			if (r_idx == MON_ICHIRIN) return TRUE;
			else return FALSE;

		case CLASS_KANAKO:
			if (r_idx == MON_KANAKO) return TRUE;
			else return FALSE;
		case CLASS_FUTO:
			if (r_idx == MON_FUTO) return TRUE;
			else return FALSE;
		case CLASS_SUNNY:
		case CLASS_LUNAR:
		case CLASS_STAR:
		case CLASS_3_FAIRIES:
			if (r_idx == MON_3FAIRIES) return TRUE;
			//v1.1.68 三妖精専用性格はチルノ、ラルバ、ピースも自分扱い
			else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) && (r_idx == MON_CIRNO || r_idx == MON_CLOWNPIECE || r_idx == MON_LARVA))
				return TRUE;
			else return FALSE;

		case CLASS_ALICE:
			if (r_idx == MON_ALICE) return TRUE;
			else return FALSE;
		case CLASS_LUNASA:
		case CLASS_MERLIN:
		case CLASS_LYRICA:
			if (r_idx == MON_LUNASA || r_idx == MON_MERLIN || r_idx == MON_LYRICA) return TRUE;
			else return FALSE;

		case CLASS_MARISA:
			if (r_idx == MON_MARISA) return TRUE;
			else return FALSE;
		case CLASS_CLOWNPIECE:
		case CLASS_VFS_CLOWNPIECE:
			if (r_idx == MON_CLOWNPIECE) return TRUE;
			else return FALSE;
		case CLASS_DOREMY:
			if (r_idx == MON_DOREMY) return TRUE;
			else return FALSE;
		case CLASS_SAKUYA:
			if (r_idx == MON_SAKUYA) return TRUE;
			else return FALSE;
		case CLASS_HINA:
			if (r_idx == MON_HINA) return TRUE;
			else return FALSE;

		case CLASS_BENBEN:
		case CLASS_YATSUHASHI:
			if (r_idx == MON_BENBEN || r_idx == MON_YATSUHASHI) return TRUE;
			else return FALSE;

		case CLASS_RAIKO:
			if (r_idx == MON_RAIKO) return TRUE;
			else return FALSE;
		case CLASS_MAMIZOU:
			if (r_idx == MON_MAMIZOU) return TRUE;
			else return FALSE;
		case CLASS_YUKARI:
			if (r_idx == MON_YUKARI) return TRUE;
			else return FALSE;
		case CLASS_RINGO:
			if (r_idx == MON_RINGO) return TRUE;
			else return FALSE;
		case CLASS_SEIRAN:
			if (r_idx == MON_SEIRAN) return TRUE;
			else return FALSE;

		case CLASS_EIRIN:
		case CLASS_KAGUYA:
			if (r_idx == MON_EIRIN || r_idx == MON_KAGUYA) return TRUE;
			else return FALSE;

		case CLASS_SAGUME:
			if (r_idx == MON_SAGUME) return TRUE;
			else return FALSE;
		case CLASS_REISEN2:
			if (r_idx == MON_REISEN2) return TRUE;
			else return FALSE;
		case CLASS_TOYOHIME:
			if (r_idx == MON_TOYOHIME) return TRUE;
			else return FALSE;
		case CLASS_YORIHIME:
			if (r_idx == MON_YORIHIME) return TRUE;
			else return FALSE;
		case CLASS_JUNKO:
			if (r_idx == MON_JUNKO) return TRUE;
			else return FALSE;

		case CLASS_HECATIA:
			if (r_idx >= MON_HECATIA1 && r_idx <= MON_HECATIA3) return TRUE;
			else return FALSE;

		case CLASS_NEMUNO:
			if (r_idx == MON_NEMUNO) return TRUE;
			else return FALSE;
		case CLASS_AUNN:
			if (r_idx == MON_AUNN) return TRUE;
			else return FALSE;
		case CLASS_NARUMI:
			if (r_idx == MON_NARUMI) return TRUE;
			else return FALSE;
		case CLASS_LARVA:
			if (r_idx == MON_LARVA) return TRUE;
			else return FALSE;
		case CLASS_MAI:
			if (r_idx == MON_MAI) return TRUE;
			else return FALSE;

		case CLASS_SATONO:
			if (r_idx == MON_SATONO) return TRUE;
			else return FALSE;
		case CLASS_JYOON:
			if (r_idx == MON_JYOON) return TRUE;
			else return FALSE;
		case CLASS_SHION:
			if (r_idx == MON_SHION_1 || r_idx == MON_SHION_2) return TRUE;
			else return FALSE;

		case CLASS_OKINA:
			if (r_idx == MON_OKINA) return TRUE;
			else return FALSE;
		case CLASS_EIKA:
			if (r_idx == MON_EIKA) return TRUE;
			else return FALSE;
		case CLASS_MAYUMI:
			if (r_idx == MON_MAYUMI) return TRUE;
			else return FALSE;

		case CLASS_KUTAKA:
			if (r_idx == MON_KUTAKA) return TRUE;
			else return FALSE;

		case CLASS_URUMI:
			if (r_idx == MON_URUMI) return TRUE;
			else return FALSE;

		case CLASS_SAKI:
			if (r_idx == MON_SAKI) return TRUE;
			else return FALSE;

		case CLASS_KEIKI:
			if (r_idx == MON_KEIKI) return TRUE;
			else return FALSE;

		case CLASS_TAKANE:
			if (r_idx == MON_TAKANE) return TRUE;
			else return FALSE;

		case CLASS_MOMOYO:
			if (r_idx == MON_MOMOYO) return TRUE;
			else return FALSE;

		case CLASS_SANNYO:
			if (r_idx == MON_SANNYO) return TRUE;
			else return FALSE;



	}


	return FALSE;
}




/*:::一切の精神攻撃が効かない*/
bool diehardmind()
{
	if(p_ptr->pclass == CLASS_KOISHI) return TRUE;
	if(p_ptr->pclass == CLASS_FLAN) return TRUE;
	if(p_ptr->pclass == CLASS_EIKI) return TRUE;
	if(p_ptr->pclass == CLASS_JUNKO) return TRUE;
	if(prace_is_(RACE_HANIWA))return TRUE;


	return FALSE;
}

//v1.1.15 ヘカーティアの地球と月の体に新しいTシャツを作る
static void make_heca_t(int body)
{
	object_type forge;
	object_type *o_ptr = &forge;
	object_type *to_ptr;

	//変なTシャツリスト
	cptr engrish_list[] =
	{
		"All your base are belong to us",
		"Today is under construction",
		"Crap Your Hands",
		"flog",
		"HELS",
		"EXPREMAY DENGERLUS",
		"JUST IT DO",
		"GET KILLED, GET NOTICED",
		"THIS IS NOT HERE",
		"FIGHT PEACEFUL",
		"I rub you",
		"Your Next",
		_("地獄お", "Welcome <3 Hell"),
		"ajides",
		"ocosite",
		"KIBIN",
		_("GHBを倒したけどこんな嫌なTシャツしか手に入らなかったよ!",
        "I defeated the GHB, and all I got was this ugly T-shirt!"),

	};

	int old_object_level = object_level;

	//inven_add[]の前半部が地球の体と決めている。キャラメイク時のデフォ設定
	if(body == HECATE_BODY_EARTH)
		to_ptr = &inven_add[INVEN_BODY - INVEN_RARM];
	else
		to_ptr = &inven_add[INVEN_BODY - INVEN_RARM + 12];

	if(to_ptr->k_idx)
	{
		msg_print(_("ERROR:make_heca_tの動作がおかしい", "ERROR: Weirdness in make_heca_t"));
		return;
	}
	object_level = 70;

	do
	{
		object_prep(o_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_T_SHIRT));
		create_artifact(o_ptr, FALSE);
	}while(object_is_cursed(o_ptr));

	remove_flag(o_ptr->art_flags, TR_NO_TELE);
	remove_flag(o_ptr->art_flags, TR_NO_MAGIC);
	o_ptr->to_a = 18 + randint0(6);
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	o_ptr->art_name = quark_add(format(_("【%s】と書かれた",
                                        " with [%s] written on it"),engrish_list[randint0(sizeof(engrish_list) / sizeof(cptr))]));

	object_copy(to_ptr,o_ptr);

	object_level = old_object_level;

}

/*
 * Advance experience levels and print experience
 */
/*:::レベルアップチェックとレベルダウン処理*/
/*:::レベル10ごとの増加能力値選択、獣人の突然変異、カオスパトロンもここ*/
///sys race レベルアップ処理とカオスパトロン処理
void check_experience(void)
{
	bool level_reward = FALSE;
	bool level_mutation = FALSE;
	bool level_inc_stat = FALSE;
	bool android = (p_ptr->prace == RACE_ANDROID ? TRUE : FALSE);
	int  old_lev = p_ptr->lev;


	/* Hack -- lower limit */
	if (p_ptr->exp < 0) p_ptr->exp = 0;
	if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;
	if (p_ptr->max_max_exp < 0) p_ptr->max_max_exp = 0;

	/* Hack -- upper limit */
	if (p_ptr->exp > PY_MAX_EXP) p_ptr->exp = PY_MAX_EXP;
	if (p_ptr->max_exp > PY_MAX_EXP) p_ptr->max_exp = PY_MAX_EXP;
	if (p_ptr->max_max_exp > PY_MAX_EXP) p_ptr->max_max_exp = PY_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if (p_ptr->exp > p_ptr->max_exp) p_ptr->max_exp = p_ptr->exp;

	/* Hack -- maintain "max max" experience */
	if (p_ptr->max_exp > p_ptr->max_max_exp) p_ptr->max_max_exp = p_ptr->max_exp;

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();


	/* Lose levels while possible */
	while ((p_ptr->lev > 1) &&
	       (p_ptr->exp < ((android ? player_exp_a : player_exp)[p_ptr->lev - 2] * p_ptr->expfact / 100L)))
	{
		/* Lose a level */
		p_ptr->lev--;

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((p_ptr->lev < PY_MAX_LEVEL) &&
	       (p_ptr->exp >= ((android ? player_exp_a : player_exp)[p_ptr->lev-1] * p_ptr->expfact / 100L)))
	{
		int new_deity_rank = 0;//野良神様用
		/* Gain a level */
		p_ptr->lev++;

		/* Save the highest level */
		//最大レベルを更新したときの処理(経験値ダウンなどで下がったレベルが回復するときにはここには来ない)
		if (p_ptr->lev > p_ptr->max_plv)
		{
			p_ptr->max_plv = p_ptr->lev;

			/*:::Hack 小傘の武器がパワーアップする*/
			if(p_ptr->pclass == CLASS_KOGASA)
				apply_kogasa_magic(&inventory[INVEN_RARM],p_ptr->max_plv,FALSE);

			//v1.1.21 兵士の技能習得
			if(p_ptr->pclass == CLASS_SOLDIER)
			{
				get_soldier_skillpoint();
			}

			//v1.1.52 菫子新性格での新技習得 7レベルごと
			if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
			{
				if ((p_ptr->lev - 1) % 7 == 0)
				{
					okina_make_nameless_arts(p_ptr->lev / 7);//1-7 0はゲーム開始時に処理済み
				}
			}

			///mod150103 野良神様名声プラス　特定レベルで新しい神格になる
			set_deity_bias(DBIAS_REPUTATION, 1);
			if(p_ptr->prace == RACE_STRAYGOD && p_ptr->max_plv == 30) new_deity_rank = 2;
			else if(p_ptr->prace == RACE_STRAYGOD && p_ptr->max_plv == 45) new_deity_rank = 3;

			//v1.1.15 ヘカーティアがレベル20になったら地球と月の体が使える。Tシャツ生成
			if(p_ptr->pclass == CLASS_HECATIA && p_ptr->max_plv == 20)
			{
				make_heca_t(HECATE_BODY_EARTH);
				make_heca_t(HECATE_BODY_MOON);
			}

			//if ((p_ptr->pclass == CLASS_CHAOS_WARRIOR) ||
			if ((p_ptr->pclass == CLASS_MAID) ||
			    (p_ptr->muta2 & MUT2_CHAOS_GIFT))
			{
				level_reward = TRUE;
			}
			/*
			if (p_ptr->prace == RACE_BEASTMAN)
			{
				if (one_in_(5)) level_mutation = TRUE;
			}
			*/
			if (p_ptr->pclass == CLASS_LARVA)
			{
				if (one_in_(4)) level_mutation = TRUE;
			}
			level_inc_stat = TRUE;

			do_cmd_write_nikki(NIKKI_LEVELUP, p_ptr->lev, NULL);
		}

		/* Sound */
		sound(SOUND_LEVEL);

		/* Message */
#ifdef JP
msg_format("レベル %d にようこそ。", p_ptr->lev);
#else
		msg_format("Welcome to level %d.", p_ptr->lev);

#endif

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL | PW_INVEN);

		/* HPとMPの上昇量を表示 */
		level_up = 1;

		/* Handle stuff */
		handle_stuff();

		level_up = 0;

		if (level_inc_stat)
		{
			if(!(p_ptr->max_plv % 10))
			{
				int choice;
				screen_save();
				while(1)
				{
					int n;
					char tmp[32];

#ifdef JP
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) 腕力 (現在値 %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) 知能 (現在値 %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) 賢さ (現在値 %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) 器用 (現在値 %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) 耐久 (現在値 %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) 魅力 (現在値 %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        どの能力値を上げますか？", 1, 14);
#else
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) Str (cur %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) Int (cur %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) Wis (cur %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) Dex (cur %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) Con (cur %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) Chr (cur %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        Which stat do you want to raise?", 1, 14);
#endif
					while(1)
					{
						choice = inkey();
						if ((choice >= 'a') && (choice <= 'f')) break;
					}
					for(n = 0; n < 6; n++)
						if (n != choice - 'a')
							prt("",n+2,14);
#ifdef JP
					if (get_check("よろしいですか？")) break;
#else
					if (get_check("Are you sure? ")) break;
#endif
				}
				do_inc_stat(choice - 'a');
				screen_load();
			}
			else if(!(p_ptr->max_plv % 2))
				do_inc_stat(randint0(6));
		}

		if (level_mutation)
		{
#ifdef JP
msg_print("あなたは変わった気がする...");
#else
			msg_print("You feel different...");
#endif

			(void)gain_random_mutation(0);
			level_mutation = FALSE;
		}
		//野良神様の神格変更
		if(new_deity_rank)	change_deity_type(new_deity_rank);

		/*
		 * 報酬でレベルが上ると再帰的に check_experience() が
		 * 呼ばれるので順番を最後にする。
		 */
		/*:::カオスパトロン報酬*/
		if (level_reward)
		{
			if(p_ptr->muta2 & MUT2_CHAOS_GIFT) gain_level_reward(0);
			else gain_maid_reward();
			level_reward = FALSE;
		}

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL);

		/* Handle stuff */
		handle_stuff();
	}
	///mod140913 時間逆転食らったとき次レベルまでの必要経験値表示の再描画が遅れるので追加
	p_ptr->redraw |= (PR_EXP);
	redraw_stuff();

	/* Load an autopick preference file */
	if (old_lev != p_ptr->lev) autopick_load_pref(FALSE);
}


/*
 * Hack -- Return the "automatic coin type" of a monster race
 * Used to allocate proper treasure when "Creeping coins" die
 *
 * XXX XXX XXX Note the use of actual "monster names"
 */
/*:::クリーピングコインが特定の種類のコインを落とすための処理*/
///item クリーピングコインなどが落とすコインを指定する
static int get_coin_type(int r_idx)
{
	/* Analyze monsters */
	switch (r_idx)
	{
	case MON_COPPER_COINS: return 2;
	case MON_SILVER_COINS: return 5;
	case MON_GOLD_COINS: return 10;
//	case MON_MITHRIL_COINS:
//	case MON_MITHRIL_GOLEM: return 16;
//	case MON_ADAMANT_COINS: return 17;
	}
	/* Assume nothing */
	return 0;
}


/*
 * Hack -- determine if a template is Cloak
 */
///item クロークを落とすhook
static bool kind_is_cloak(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_CLOAK)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Polearm
 */

///mod131224 item TVAL ポールアームを落とすhook 槍か長柄か斧のランダムになるようにした
static bool kind_is_polearm(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_POLEARM || k_ptr->tval == TV_AXE || k_ptr->tval == TV_SPEAR  )
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Sword
 */
///item デスソードとヘルブレードが剣を落とすためのhook
///mod とりあえず折れた剣以外の剣に設定
static bool kind_is_sword(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval == TV_SWORD) && (k_ptr->sval > SV_WEAPON_BROKEN_SWORD))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Book
 */
///item realm 魔法書
/*:::ラアルが落とす本に使われる*/
static bool kind_is_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_BOOK_ELEMENT) && (k_ptr->tval <= TV_BOOK_CHAOS))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Good book
 */
/*:::上質な本を選定するhook*/
/*:::ラアルが落とす本に使われる*/
///item realm 魔法書
static bool kind_is_good_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_BOOK_ELEMENT) && (k_ptr->tval <= TV_BOOK_CHAOS)  && (k_ptr->sval > 1))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Armor
 */
///item 鎧を落とすhook
///mod131223
static bool kind_is_armor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_ARMOR)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is hafted weapon
 */
///item 鈍器を落とすhook
///mod131223 とりあえずハンマーで固定
static bool kind_is_hafted(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_HAMMER)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}

/*:::クエスト達成時のフラグ処理とメッセージ*/
void complete_quest(int quest_num)
{
	switch (quest[quest_num].type)
	{
	case QUEST_TYPE_RANDOM:
		if (record_rand_quest) do_cmd_write_nikki(NIKKI_RAND_QUEST_C, quest_num, NULL);
		break;
	default:
		if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, quest_num, NULL);
		break;
	}

	//v1.1.40 クエスト達成時刻を正確にするため追加
	update_playtime();

	//v1.1.25 クエスト達成時刻追加
	quest[quest_num].comptime = playtime;

	quest[quest_num].status = QUEST_STATUS_COMPLETED;
	quest[quest_num].complev = (byte)p_ptr->lev;

	///mod140610
	if (quest[quest_num].type == QUEST_TYPE_BOUNTY_HUNT)
	{
		if(quest_num == QUEST_WANTED_SEIJA) msg_print(_("お尋ね者を捕まえた！", "You just captured the wanted person!"));
		else msg_print(_("クエストを達成した！", "You just completed your quest!"));
		msg_print(NULL);
	}
	else if(quest[quest_num].type == QUEST_TYPE_FIND_EXIT)
	{
		msg_print(_("あなたは目的地点に辿り着いた！", "You reached your destination!"));
		if(QRKDR)
		{
			//開始ターンを経過ターンに切り替えて保存
			qrkdr_rec_turn = turn - qrkdr_rec_turn;
			if(cheat_xtra) msg_format("take_turn:%d",qrkdr_rec_turn);
		}
	}
	else if (!(quest[quest_num].flags & QUEST_FLAG_SILENT))
	{
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_QUEST_CLEAR);//v1.1.58
		msg_print(_("クエストを達成した！", "You just completed your quest!"));
		msg_print(NULL);
	}
}

static int count_all_hostile_monsters(void)
{
	int x, y;
	int number_mon = 0;

	for (x = 0; x < cur_wid; ++ x)
	{
		for (y = 0; y < cur_hgt; ++ y)
		{
			int m_idx = cave[y][x].m_idx;

			if (m_idx > 0 && is_hostile(&m_list[m_idx]))
			{
				++ number_mon;
			}
		}
	}

	return number_mon;
}

/*:::Exダンジョン専用のクエスト報酬生成*/
void create_extra_quest_rewards(void)
{
	int k_idx, a_idx,i;
	object_type forge;
	object_type *q_ptr = &forge;
	int lev = dun_level;
	int x,y,tx, ty;
	bool msg_done_flag = FALSE;
	bool flag_ok = FALSE;
	int sdia;

	if(!EXTRA_QUEST_FLOOR)
	{
		msg_print(_("ERROR:EXクエストフロアでない階でgain_extra_quest_rewardsが呼ばれた",
                    "ERROR: gain_extra_quest_rewards called on non-extra quest level"));
		return;
	}
	//階段の場所を得る
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave_have_flag_bold(y,x,FF_STAIRS)) break;
		}
		if(cave_have_flag_bold(y,x,FF_STAIRS)) break;
	}
	for(sdia=1;sdia<10;sdia++)
	{
		int k;
		for(k=sdia*10;k>0;k--)
		{
			//階段の近くでアイテムを落とせる座標を得る　壁を挟んでもいい
			ty = y-sdia+randint0(sdia*2+1);
			tx = x-sdia+randint0(sdia*2+1);

			if(!in_bounds(ty,tx)) continue;
			if(!cave_drop_bold(ty,tx)) continue;
			flag_ok = TRUE;
			break;
		}
		if(flag_ok) break;
	}
	if(!flag_ok)
	{
		msg_print(_("WARNING:クエストアイテムを生成可能な地形が階段の近くに見つからなかった",
                    "WARNING: No terrain suitable for quest item generation found close to staircase"));
		return;
	}

	for(i=0;ex_quest_reward_table[i].level;i++)
	{
		if(ex_quest_reward_table[i].level != lev) continue;
		object_wipe(q_ptr);

		a_idx = ex_quest_reward_table[i].a_idx;

		if(a_idx)	//★の生成
		{
			//既出
			if(a_info[a_idx].cur_num)
			{
				if(dun_level > 40) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
				else	k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);
				object_prep(q_ptr, k_idx);
				//巻物なのでapply_magic不要のはず
			}
			else
			{
				artifact_type *a_ptr = &a_info[a_idx];
				if (create_named_art(a_idx, ty, tx))
				{
					a_ptr->cur_num = 1;
					if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
				}
				else if (!preserve_mode) a_ptr->cur_num = 1;
				continue;
			}


		}
		else//★以外
		{
			k_idx = lookup_kind(ex_quest_reward_table[i].tval, ex_quest_reward_table[i].sval);
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, lev, ex_quest_reward_table[i].am_mode);
			q_ptr->number = ex_quest_reward_table[i].num;
		}

		(void)drop_near(q_ptr, -1, ty, tx);
		if(!msg_done_flag)
		{
			msg_print(_("階段のほうで何かゴトゴトと音がした。",
                        "You hear something roll on the staricase."));
			msg_done_flag = TRUE;
		}
	}

}





/*
 * Check for "Quest" completion when a quest monster is killed or charmed.
 */
///sys モンスターが倒れたときや消えたとき、クエスト完了状態確認
void check_quest_completion(monster_type *m_ptr)
{
	int i, j, y, x, ny, nx;

	int quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;
	bool extra_reward = FALSE;

	object_type forge;
	object_type *q_ptr;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	//一時的存在は関係ない
	if(m_ptr->mflag & MFLAG_EPHEMERA) return;

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Inside a quest */
	quest_num = p_ptr->inside_quest;

	///mod141231 霊夢占いの日替わり賞金首処理
	if(p_ptr->today_mon == FORTUNETELLER_TROPHY && m_ptr->r_idx == p_ptr->barter_value && r_ptr->r_akills)
	{
		reward = TRUE;
	}

	///mod140610 お尋ね者クエスト
	/*:::賞金首クエスト処理*/
	if(r_ptr->flags3 & RF3_WANTED)
	{
		int cnt;
		bool ok = FALSE;
		for (cnt = max_quests - 1; cnt > 0; cnt--)
		{
			if (quest[cnt].status != QUEST_STATUS_TAKEN || quest[cnt].type != QUEST_TYPE_BOUNTY_HUNT)continue;
			if(quest[cnt].r_idx != m_ptr->r_idx) continue;

			ok = TRUE;
			break;
		}
		if(!ok) msg_format(_("ERROR:クエストを受領していない賞金首が倒された",
                            "ERROR: Wanted person defeated without quest being received"));
		else complete_quest(cnt);
	}

	/* Search for an active quest on this dungeon level */
	if (!quest_num)
	{
		for (i = max_quests - 1; i > 0; i--)
		{
			/* Quest is not active */
			if (quest[i].status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if (quest[i].flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if ((quest[i].level != dun_level) &&
			    (quest[i].type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill monster" quest */
			if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) ||
				(quest[i].type == QUEST_TYPE_FIND_OBJECT) ||
				(quest[i].type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if ((quest[i].type == QUEST_TYPE_KILL_NUMBER) ||
			    (quest[i].type == QUEST_TYPE_TOWER) ||
			    (quest[i].type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if (((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_RANDOM)) &&
			     (quest[i].r_idx == m_ptr->r_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if (quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		i = quest_num;

		switch (quest[i].type)
		{
			case QUEST_TYPE_KILL_NUMBER:
			{
				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].num_mon)
				{
					complete_quest(i);

					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_KILL_ALL:
			{
				if (!is_hostile(m_ptr)) break;

				//「いま倒されている途中なのが最後の敵対モンスター」のとき
				if (count_all_hostile_monsters() == 1)
				{
					if (quest[i].flags & QUEST_FLAG_SILENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 2);
						set_deity_bias(DBIAS_COSMOS, 1);
					}
					else
					{
						complete_quest(i);
					}

					//エクストラクエスト達成時、ステータスをFINISHEDにしinside_questをクリアし報酬フラグ設定
					if(INSIDE_EXTRA_QUEST)
					{
						extra_reward = TRUE;
						p_ptr->inside_quest = 0;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_LEVEL:
			case QUEST_TYPE_RANDOM:
			{
				/* Only count valid monsters */
				if (quest[i].r_idx != m_ptr->r_idx)
					break;

				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].max_num)
				{
					complete_quest(i);

					//エクストラクエスト達成時、ステータスをFINISHEDにしinside_questをクリアし報酬フラグ設定
					if(INSIDE_EXTRA_QUEST)
					{
						create_stairs = FALSE;
						extra_reward = TRUE;
						p_ptr->inside_quest = 0;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
					else if (!(quest[i].flags & QUEST_FLAG_PRESET))
					{
						create_stairs = TRUE;
						p_ptr->inside_quest = 0;
					}


					/* Finish the two main quests without rewarding */
				//	if ((i == QUEST_OBERON) || (i == QUEST_SERPENT))
					///mod140305 紫を倒しても報酬は出ない
					if ((i == QUEST_YUKARI) )
					{
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 10);
						set_deity_bias(DBIAS_COSMOS, 3);
					}
					/*:::Hack Easyでは紫を倒しても50階から下にはいけない*/
					if ((i == QUEST_YUKARI) && difficulty == DIFFICULTY_EASY) create_stairs = FALSE;

					if ((i == QUEST_OBERON) ||(i == QUEST_SERPENT) )
					{
						///mod140118 オベロンクエを達成しても階段が出ないようにする
						create_stairs = FALSE;
						quest[i].status = QUEST_STATUS_FINISHED;
						set_deity_bias(DBIAS_REPUTATION, 10); //どうせこの時にはレベル45以上になってると思うが一応
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
					}

					//Hack - Extraモードではオベロンを倒したとき階段が出る(混沌の領域へ飛ぶ)
					if(EXTRA_MODE && i == QUEST_OBERON)
					{
						create_stairs = TRUE;
					}

						///mod140118 火竜討伐クエを達成しても階段が出ないようにする
						///mod150919 ヘカーティアクエも追加
					if (i == QUEST_SHOOTINGSTAR || i == QUEST_HECATIA)
					{
						create_stairs = FALSE;
					}

					if (quest[i].type == QUEST_TYPE_RANDOM)
					{
						reward = TRUE;
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 2);
						set_deity_bias(DBIAS_COSMOS, 1);
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_ANY_LEVEL:
			{
				quest[i].cur_num++;
				if (quest[i].cur_num >= quest[i].max_num)
				{
					complete_quest(i);
					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_TOWER:
			{
				if (!is_hostile(m_ptr)) break;

				if (count_all_hostile_monsters() == 1)
				{
					quest[i].status = QUEST_STATUS_STAGE_COMPLETED;

					if((quest[QUEST_TOWER1].status == QUEST_STATUS_STAGE_COMPLETED) &&
					   (quest[QUEST_TOWER2].status == QUEST_STATUS_STAGE_COMPLETED) &&
					   (quest[QUEST_TOWER3].status == QUEST_STATUS_STAGE_COMPLETED))
					{

						complete_quest(QUEST_TOWER1);
					}
				}
				break;
			}
		}
	}

	/* Create a magical staircase */
	if (create_stairs)
	{
		/* Stagger around */
		while (cave_perma_bold(y, x) || cave[y][x].o_idx || (cave[y][x].info & CAVE_OBJECT) )
		{
			/* Pick a location */
			scatter(&ny, &nx, y, x, 1, 0);

			/* Stagger */
			y = ny; x = nx;
		}

		if(EXTRA_MODE && dun_level == 100)
			msg_print(_("混沌の領域への通路が開いた...", "A passage opens to the Realm of Chaos..."));
		else
			msg_print(_("魔法の階段が現れた...", "A magical staircase appears..."));


		/* Create stairs down */
		cave_set_feat(y, x, feat_down_stair);

		/* Remember to update everything */
		p_ptr->update |= (PU_FLOW);
	}

	/*
	 * Drop quest reward
	 */
	if (reward)
	{
		//v1.1.55 ランクエと霊夢賞金首の報酬増加
//		for (j = 0; j < (dun_level / 15)+1; j++)
		for (j = 0; j < (dun_level / 10)+1; j++)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Make a great object */
			make_object(q_ptr, AM_GOOD | AM_GREAT);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
	}

	if(extra_reward)
	{
		create_extra_quest_rewards();

	}

}

/*:::アイテム入手型クエストでアイテムを入手したかどうかチェック*/

void check_find_art_quest_completion(object_type *o_ptr)
{
	int i;
	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->name1))
		{
			complete_quest(i);
		}
		///mod151029 宝物庫の★なし時の特殊処理
		else if ((quest[i].type == QUEST_TYPE_FIND_RANDOM_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (o_ptr->marked & OM_SPECIAL_FLAG))
		{
			complete_quest(i);
		}

		else if ((quest[i].type == QUEST_TYPE_FIND_OBJECT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->k_idx))
		{
			complete_quest(i);
		}


	}
}

/*
 * Return monster death string
 */
/*:::デフォルトのモンスター死亡時セリフ*/
///mon デフォのモンスター死亡時セリフ
cptr extract_note_dies(monster_race *r_ptr)
{
	/* Some monsters get "destroyed" */
	if (!monster_living(r_ptr))
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (r_ptr->blow[i].method == RBM_EXPLODE)
			{
#ifdef JP
//				return "は爆発して粉々になった。";
				return "は大爆発した！";
#else
				return " explodes!";
#endif
			}
		}

#ifdef JP
		return "を倒した。";
#else
		return " is defeated.";
#endif
	}

	/* Assume a default death */
#ifdef JP
	///msg131214 death
	//return "は死んだ。";
	return "を倒した。";

#else
	return " is defeated.";
#endif
}





//v1.1.51 新アリーナ用商品生成
//nightmare_treasure_table[]から今回のステージレベルに合わせた報酬アイテムをレアリティ考慮して選び生成して地面に落とす
static void make_nightmare_treasure(byte x, byte y)
{
	object_type forge;
	object_type *o_ptr = &forge;
	int i;
	int treasure_idx = -1;
	int stage_level;

	int chance_total;

	//グローバル変数に保存しておいたモンスターレベルからステージレベルを逆算 0-7
	for (i = 0; i < NIGHTMARE_DIARY_STAGE_LEV_MAX; i++)
	{
		stage_level = i;
		if (nightmare_mon_base_level <= nightmare_stage_table[i].level) break;
	}


	//rarity値を全部足し、合計値以内のランダムな値を取得し、そこから同じ順でrarity値を引いていき、マイナスになったところで止めてそこのインデックスを決定値とする
	chance_total = 0;
	for (i = 0; nightmare_treasure_table[i].rarity; i++)//rarity=0が配列の終端ダミー
	{
		if (nightmare_treasure_table[i].nightmare_level != stage_level) continue;
		chance_total += nightmare_treasure_table[i].rarity;
	}
	chance_total = randint0(chance_total);
	for (i = 0; nightmare_treasure_table[i].rarity; i++)
	{
		if (nightmare_treasure_table[i].nightmare_level != stage_level) continue;
		chance_total -= nightmare_treasure_table[i].rarity;

		if (chance_total < 0) break;
	}
	if (!nightmare_treasure_table[i].rarity) { msg_format(_("ERROR:make_nightmare_treasure()で指定されたレベル(%d)の報酬アイテムが登録されていない",
                                                            "ERROR: Reward item not list for given level (%d) in make_nightmare_treasure()"), stage_level); return; }
	treasure_idx = i;

	//決定したidxのアイテムを生成
	object_prep(o_ptr, lookup_kind(nightmare_treasure_table[treasure_idx].tval, nightmare_treasure_table[treasure_idx].sval));
	o_ptr->number = nightmare_treasure_table[treasure_idx].num;
	apply_magic(o_ptr, nightmare_mon_base_level, AM_FORCE_NORMAL);

	(void)drop_near(o_ptr, 0, y, x);

}


//v1.1.51　新アリーナ勝利判定用ルーチン
//monster_death()内でモンスターが倒れたときに呼ばれる。全部のモンスターを倒したかどうか確認し、達成したらメッセージを出し賞品生成ルーチンを呼ぶ
static void	check_nightmare_diary_completion(monster_type *m_ptr)
{

	if (!p_ptr->inside_arena)	return;
	if (!m_ptr->r_idx) return;
	if (!is_hostile(m_ptr)) return;

	//「いま打倒処理途中のモンスターが最後の敵対モンスターである」とき達成と判定する
	if (count_all_hostile_monsters() == 1)
	{
		p_ptr->exit_bldg = TRUE;

		//賞品生成が既に済んでいる場合終了　
		//アリーナ内ではモンスター召喚系が全て無効化されているはずなので同じアリーナでここに二度来ることはないが念の為
		if (!nightmare_mon_base_level) return;

		//新アリーナの達成回数と合計ポイント数に加算
		nightmare_diary_win++;
		nightmare_total_point += tmp_nightmare_point;

		if (nightmare_total_point > 999999999)
			nightmare_total_point = 999999999;

		if (p_ptr->pclass == CLASS_DOREMY)
		{
			msg_print(_("あなたは皆の悪夢を美味しく頂いた。", "All those nightmares were delicious."));
			(void)set_food(PY_FOOD_MAX - 1);

		}

		//挑戦可能悪夢レベルが上がったら報告？

		//写真生成
		make_nightmare_diary_photo(m_ptr->fx, m_ptr->fy);
		//賞品生成
		make_nightmare_treasure(m_ptr->fx, m_ptr->fy);

		//v1.1.57
		if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, 1, 0);

		//賞品生成が住んだフラグとして、グローバル変数のアリーナモンスターレベルを0にする
		nightmare_mon_base_level = 0;

	}



}


/*:::モンスターからのアイテムドロップ処理や一部モンスターが倒れた時の特殊処理　拾ったアイテムとクエストモンスター、アリーナ考慮*/
/*:::ユニークに対しては＠のみがこの関数を処理する*/
/*
 * Handle the "death" of a monster.
 *
 * Disperse treasures centered at the monster location based on the
 * various flags contained in the monster flags fields.
 *
 * Check for "Quest" completion when a quest monster is killed.
 *
 * Note that only the player can induce "monster_death()" on Uniques.
 * Thus (for now) all Quest monsters should be Uniques.
 *
 * Note that monsters can now carry objects, and when a monster dies,
 * it drops all of its objects, which may disappear in crowded rooms.
 */
///item mon モンスターが倒れた時の処理　アイテムドロップなど
///mod131224 TVAL
void monster_death(int m_idx, bool drop_item)
{
	int i, j, y, x;

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;

	int yachie_card_num = 0;

	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool visible = ((m_ptr->ml && !p_ptr->image) || (r_ptr->flags1 & RF1_UNIQUE));

	u32b mo_mode = 0L;

	bool do_gold = (!(r_ptr->flags1 & RF1_ONLY_ITEM));
	bool do_item = (!(r_ptr->flags1 & RF1_ONLY_GOLD));
	bool cloned = (m_ptr->smart & SM_CLONED) ? TRUE : FALSE;
	int force_coin = get_coin_type(m_ptr->r_idx);

	object_type forge;
	object_type *q_ptr;

	/*:::モンスターが指定したアイテムを落とすことを可能にするフラグ*/
	bool drop_chosen_item = drop_item && !cloned && !p_ptr->inside_arena
		&& !p_ptr->inside_battle && !is_pet(m_ptr);

	//配下を倒した時　野良神様処理
	if(drop_item && is_pet(m_ptr))
	{
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		set_deity_bias(DBIAS_REPUTATION, -1);
	}
	//神奈子の信仰ポイント取得
	if(p_ptr->pclass == CLASS_KANAKO && drop_chosen_item ) kanako_get_point(m_ptr);

	//雛の人形販売
	if(p_ptr->pclass == CLASS_HINA && drop_chosen_item )
	{
		int get_au;
		get_au = r_ptr->level * r_ptr->level * r_ptr->level / (200-p_ptr->lev-p_ptr->stat_ind[A_CHR]);
		if(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
			get_au = get_au * 5 + 100;
		p_ptr->magic_num1[1] += get_au;
		if(p_ptr->magic_num1[1] > 100000) p_ptr->magic_num1[1] = 100000;
	}
	///mod160325 UNIQUE2を倒した時にスコア加算値を計算
	if(r_ptr->flags7 & RF7_UNIQUE2 && drop_chosen_item)
	{
		score_bonus_kill_unique2 += r_ptr->level * r_ptr->level;
		if(score_bonus_kill_unique2 > 1000000L) score_bonus_kill_unique2 = 1000000L;
	}

	//一時モンスターのときそれっぽいフラグON
	if(m_ptr->mflag & MFLAG_EPHEMERA)
	{
		cloned = TRUE;
		drop_chosen_item = FALSE;
		drop_item = FALSE;
	}
	/* The caster is dead? */
	/*:::時間停止中のモンスターが死亡した場合時間停止解除*/
	if (world_monster && world_monster == m_idx) world_monster = 0;

	/* Notice changes in view */
	if (r_ptr->flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
	{
		/* Update some things */
		p_ptr->update |= (PU_MON_LITE);
	}

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
		do_cmd_write_nikki(NIKKI_NAMED_PET, 3, m_name);
	}




	/* Let monsters explode! */
	/*:::爆発モンスターの場合爆発する */
	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE)
		{
			int rad = 3;
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int typ = mbe_info[r_ptr->blow[i].effect].explode_type;
			int d_dice = r_ptr->blow[i].d_dice;
			int d_side = r_ptr->blow[i].d_side;
			int damage = damroll(d_dice, d_side);

			if(m_ptr->r_idx == MON_SAGUME_MINE) rad = 4;

			if(!world_monster) project(m_idx, 3, y, x, damage, typ, flg, -1);
			break;
		}
	}

	//主の輝夜が倒れたらゲームオーバー
	//大丈夫だと思うが闘技場やアリーナでは発動しないようにしとく
	if(m_ptr->r_idx == MON_MASTER_KAGUYA && p_ptr->pclass == CLASS_EIRIN && !p_ptr->inside_battle && !p_ptr->inside_arena)
	{
		msg_print(_("何ということだ！輝夜を守りきれなかった！",
                    "No way! You failed to protected Kaguya!"));
		if (last_words)
		{
			char death_message[1024];
			get_rnd_line(_("death_j.txt", "death.txt"), 0, death_message);
			while (!get_string(_("負けセリフ: ", "Losing line: "), death_message, 1024)) ;
			p_ptr->last_message = string_make(death_message);
			msg_print(death_message);
		}
		p_ptr->playing = FALSE;
		p_ptr->is_dead = TRUE;
		p_ptr->leaving = TRUE;
		strcpy(p_ptr->died_from, "mission_failed");
		return;
	}

	/*:::カメレオンは正体を現す？*/
	if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
	{
		choose_new_monster(m_idx, TRUE, MON_CHAMELEON);
		r_ptr = &r_info[m_ptr->r_idx];
	}

	/* Check for quest completion */
	/*:::check_quest_completion() クエスト達成状況確認？*/
	check_quest_completion(m_ptr);

	/*:::アリーナでペット以外が死んだ場合（アリーナ勝利）*/
	/* Handle the possibility of player vanquishing arena combatant -KMW- */
	if (p_ptr->inside_arena && !is_pet(m_ptr))
	{
		check_nightmare_diary_completion(m_ptr);

#if 0
		p_ptr->exit_bldg = TRUE;

		if (p_ptr->arena_number > MAX_ARENA_MONS)
		{
#ifdef JP
msg_print("素晴らしい！君こそ真の勝利者だ。");
#else
			msg_print("You are a Genuine Champion!");
#endif
		}
		else
		{
#ifdef JP
msg_print("勝利！チャンピオンへの道を進んでいる。");
#else
			msg_print("Victorious! You're on your way to becoming Champion.");
#endif
		}

	/*:::指定したアイテムをドロップ*/
		if (arena_info[p_ptr->arena_number].tval)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a prize */
			object_prep(q_ptr, lookup_kind(arena_info[p_ptr->arena_number].tval, arena_info[p_ptr->arena_number].sval));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);


			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}

		if (p_ptr->arena_number > MAX_ARENA_MONS) p_ptr->arena_number++;
		p_ptr->arena_number++;
		if (record_arena)
		{
			char m_name[80];

			/* Extract monster name */
			monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

			do_cmd_write_nikki(NIKKI_ARENA, p_ptr->arena_number, m_name);
		}

#endif
	}

/*:::乗馬中のモンスターが死んだなら落馬処理 */
	if (m_idx == p_ptr->riding)
	{
		if (rakuba(-1, FALSE))
		{
#ifdef JP
			msg_print("地面に落とされた。");
#else
			msg_print("You have fallen from your mount.");
#endif
		}
	}

#if 0
	///del131221 モンスターの死体ドロップを無くした
	/*:::死体か骨を落とす処理 */
	/* Drop a dead corpse? */
	if (one_in_(r_ptr->flags1 & RF1_UNIQUE ? 1 : 4) &&
	    (r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle || cloned || ((m_ptr->r_idx == today_mon) && is_pet(m_ptr))))
	{
		/* Assume skeleton */
		bool corpse = FALSE;

		/*
		 * We cannot drop a skeleton? Note, if we are in this check,
		 * we *know* we can drop at least a corpse or a skeleton
		 */

		if (!(r_ptr->flags9 & RF9_DROP_SKELETON))
			corpse = TRUE;
		else if ((r_ptr->flags9 & RF9_DROP_CORPSE) && (r_ptr->flags1 & RF1_UNIQUE))
			corpse = TRUE;

		/* Else, a corpse is more likely unless we did a "lot" of damage */
		else if (r_ptr->flags9 & RF9_DROP_CORPSE)
		{
			/*:::オーバーキルしすぎると骨が落ちやすい処理 （今モンスターのHPはマイナスである）*/
			/* Lots of damage in one blow */
			if ((0 - ((m_ptr->maxhp) / 4)) > m_ptr->hp)
			{
				if (one_in_(5)) corpse = TRUE;
			}
			else
			{
				if (!one_in_(5)) corpse = TRUE;
			}
		}

		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make an object */
		object_prep(q_ptr, lookup_kind(TV_CORPSE, (corpse ? SV_CORPSE : SV_SKELETON)));

		apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);
		/*:::死体のpvalはモンスターidxが入る*/
		q_ptr->pval = m_ptr->r_idx;

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}
#endif

	/*:::付喪神が倒されると元アイテムが劣化することがある*/
	if (m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
	{
		object_type *o_ptr = &o_list[m_ptr->hold_o_idx];

		if(object_is_artifact(o_ptr))
		{
			if (o_ptr->to_h > -99 && one_in_(2)) o_ptr->to_h--;
			if (o_ptr->to_d > -99 && one_in_(2)) o_ptr->to_d--;
			if (o_ptr->to_a > 0 && one_in_(2)) o_ptr->to_a--;
			if ((o_ptr->pval > 1) && one_in_(13) ) o_ptr->pval--;
		}
		else
		{
			int tmp1 = o_ptr->to_h - randint1(2) - m_bonus(5,dun_level);
			int tmp2 = o_ptr->to_d - randint1(2) - m_bonus(5,dun_level);
			int tmp3 = o_ptr->to_a - randint1(3);
			o_ptr->to_h = MAX(-99,(tmp1));
			o_ptr->to_d = MAX(-99,(tmp2));
			o_ptr->to_a = MAX(0,tmp3);
			if ((o_ptr->pval > 1) && one_in_(6) ) o_ptr->pval--;
		}


	}

	//敵打倒時のHP吸収とか
	//v1.1.83 @がモンスターを倒したときのみに限定。
	if (drop_item && !p_ptr->is_dead)
	{

		///mod150502 諏訪子特殊処理　人間を倒せば倒すほど最大MP上昇
		if (p_ptr->pclass == CLASS_SUWAKO && (r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) && p_ptr->magic_num1[0] < SUWAKO_POINT_MAX)
		{
			int point = 0;
			int point_old = p_ptr->magic_num1[0] / 100;

			point = r_ptr->level + randint1(10);
			if (r_ptr->flags3 & (RF3_DEMIHUMAN)) point /= 5;
			if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || (r_ptr->flags7 & RF7_UNIQUE2)) point *= 10;

			if (point_old > 500) point /= 2;

			p_ptr->magic_num1[0] += point;
			if (p_ptr->magic_num1[0] > SUWAKO_POINT_MAX) p_ptr->magic_num1[0] = SUWAKO_POINT_MAX;
			if (p_ptr->magic_num1[0] / 100 > point_old) p_ptr->update |= PU_MANA;
		}
		//v1.1.21 兵士
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT, SS_C_SCAVENGER))
		{
			int get_mana = r_ptr->level;
			if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) get_mana *= 2;
			if (get_mana <1) get_mana = 1;
			if (player_gain_mana(get_mana))
				msg_print(_("MPが回復した！", "You recover MP!"));
		}
		//v1.1.30 妖夢　修羅の血発動中
		if (p_ptr->pclass == CLASS_YOUMU && p_ptr->tim_general[0])
		{
			int gain = MAX(1, r_ptr->level / 2);
			if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) gain *= 2;
			player_gain_mana(gain);

		}
		//v1.1.89 百々世蠱毒のグルメ HPと満腹度大幅吸収
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[3])
		{
			hp_player(m_ptr->maxhp);

			if (p_ptr->food < PY_FOOD_FULL)
			{
				set_food(MIN(p_ptr->food + m_ptr->maxhp, PY_FOOD_FULL));

			}

		}

		//v1.1.65 華扇特殊性格
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			{
				int gain = MAX(1, r_ptr->level * 2);
				if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) gain *= 2;
				hp_player(gain);
			}
		}

		//v1.1.70 種族動物霊は生物かアンデッドを倒すことで満腹度上昇
		//v1.1.87 計算式ミスっててしょっちゅう食べすぎになってたので修正
		if (prace_is_(RACE_ANIMAL_GHOST) && (monster_living(r_ptr) || r_ptr->flags3 & RF3_UNDEAD))
		{
			int feed = 1000 + r_ptr->level * 50 + r_ptr->level * r_ptr->level;
			if(p_ptr->food < PY_FOOD_ALERT) msg_print(_("あなたは敵から活力を奪った。", "You steal energy from the enemy."));
			if (p_ptr->food < PY_FOOD_FULL)	set_food(MIN(PY_FOOD_FULL, (p_ptr->food + feed)));

		}

	}


	/* Drop objects being carried */
	/*:::モンスターが拾っていたアイテムを落とす*/
	monster_drop_carried_objects(m_ptr);

	if (r_ptr->flags1 & RF1_DROP_GOOD) mo_mode |= AM_GOOD;
	if (r_ptr->flags1 & RF1_DROP_GREAT) mo_mode |= AM_GREAT;

	/*:::モンスター種族で分岐し、モンスター死亡時の特殊処理*/
	///mon 一部モンスター死亡時の特殊処理
	switch (m_ptr->r_idx)
	{
	/*:::ピンクホラーの分裂処理 summon_specific()二回*/
	case MON_PINK_HORROR:
		/* Pink horrors are replaced with 2 Blue horrors */
		if (!(p_ptr->inside_arena || p_ptr->inside_battle))
		{
			bool notice = FALSE;

			for (i = 0; i < 2; i++)
			{
				int wy = y, wx = x;
				bool pet = is_pet(m_ptr);
				u32b mode = 0L;

				if (pet) mode |= PM_FORCE_PET;

				if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_BLUE_HORROR, mode))
				{
					if (player_can_see_bold(wy, wx))
						notice = TRUE;
				}
			}

			if (notice)
#ifdef JP
				msg_print("ピンク・ホラーは分裂した！");
#else
				msg_print("The Pink horror divides!");
#endif
		}
		break;
	/*:::血戮は15%の確率でカオスブレードを落とす*/
	///item 血戮悪魔ドロップ　カオブレでなく混沌武器に変えよう
	///mon 血戮に変わって黒の剣がカオブレを落とす
#if 0
	case MON_BLOODLETTER:
		/* Bloodletters of Khorne may drop a blade of chaos */
		if (drop_chosen_item && (randint1(100) < 15))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Blade of Chaos */
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | mo_mode);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
#endif
	case MON_RAAL:
		/*:::ラアルは階層に応じた確率で本を落とす*/
		if (drop_chosen_item && (dun_level > 9))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Activate restriction */
			if ((dun_level > 49) && one_in_(5))
				get_obj_num_hook = kind_is_good_book;
			else
				get_obj_num_hook = kind_is_book;

			/* Make a book */
			make_object(q_ptr, mo_mode);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;



	/*:::暁の戦士の援軍処理*/
	case MON_DAWN:
		/*
		 * Mega^3-hack: killing a 'Warrior of the Dawn' is likely to
		 * spawn another in the fallen one's place!
		 */
		if (!p_ptr->inside_arena && !p_ptr->inside_battle)
		{
			if (!one_in_(7))
			{
				int wy = y, wx = x;
				int attempts = 100;
				bool pet = is_pet(m_ptr);

				do
				{
					scatter(&wy, &wx, y, x, 20, 0);
				}
				while (!(in_bounds(wy, wx) && cave_empty_bold2(wy, wx)) && --attempts);

				if (attempts > 0)
				{
					u32b mode = 0L;
					if (pet) mode |= PM_FORCE_PET;

					if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_DAWN, mode))
					{
						if (player_can_see_bold(wy, wx))
#ifdef JP
							msg_print("新たな戦士が現れた！");
#else
							msg_print("A new warrior steps forth!");
#endif

					}
				}
			}
		}
		break;
	/*:::混沌は巨大なカオスの爆発を起こす*/
	case MON_UNMAKER:
		/* One more ultra-hack: An Unmaker goes out with a big bang! */
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 6, y, x, 100, GF_CHAOS, flg, -1);
		}
		break;
///del131224 item なまけ神器
#if 0
	/*:::なまけ神器生成判定*/
	case MON_UNICORN_ORD:
	case MON_MORGOTH:
	case MON_ONE_RING:
		/* Reward for "lazy" player */
		if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		{
			int a_idx = 0;
			artifact_type *a_ptr = NULL;

			if (!drop_chosen_item) break;

			do
			{
				switch (randint0(3))
				{
				case 0:
					a_idx = ART_NAMAKE_HAMMER;
					break;
				case 1:
					a_idx = ART_NAMAKE_BOW;
					break;
				case 2:
					a_idx = ART_NAMAKE_ARMOR;
					break;
				}

				a_ptr = &a_info[a_idx];
			}
			while (a_ptr->cur_num);

			/* Create the artifact */
			if (create_named_art(a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				/* Hack -- Memorize location of artifact in saved floors */
				if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
			}
			else if (!preserve_mode) a_ptr->cur_num = 1;
		}
		break;
#endif
	/*:::Jはグロンドと鉄冠生成*/
	///item mon Jのドロップアイテム
		///mod140304 グロンドはモルゴスが落とすようにした
	case MON_MORGOTH:
		if (!drop_chosen_item) break;

		/* Get local object */
		q_ptr = &forge;


		//v1.1.23 ★なしオプションのときはグロンドの☆にするようにした。3d9じゃ寂しいのでちょっと強くしとく。
		if(ironman_no_fixed_art)
		{
			do
			{
				object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_GROND));
				create_artifact(q_ptr, FALSE);
				q_ptr->to_d += 5+randint1(5);
				q_ptr->to_h += 5+randint1(5);
				q_ptr->dd += 3;

			}while(object_is_cursed(q_ptr));
			//v1.1.24 もう少し補正
			if(q_ptr->to_d < 20) q_ptr->to_d = 20;

		}
		else
		{
			/* Mega-Hack -- Prepare to make "Grond" */
			object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_GROND));
			/* Mega-Hack -- Mark this item as "Grond" */
			q_ptr->name1 = ART_GROND;
			/* Mega-Hack -- Actually create "Grond" */
			apply_magic(q_ptr, -1, AM_GOOD | AM_GREAT);
		}


		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
		break;
	case MON_SERPENT:
		if (!drop_chosen_item) break;

		/* Get local object */
		q_ptr = &forge;

		/* Mega-Hack -- Prepare to make "Chaos" */
		object_prep(q_ptr, lookup_kind(TV_HEAD, SV_HEAD_IRONCROWN));

		/* Mega-Hack -- Mark this item as "Chaos" */
		q_ptr->name1 = ART_CHAOS;

		/* Mega-Hack -- Actually create "Chaos" */
		apply_magic(q_ptr, -1, AM_GOOD | AM_GREAT);

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
		break;
	/*:::折れたデスソードは折れた剣か折れたダガーを生成*/
	case MON_B_DEATH_SWORD:
		if (drop_chosen_item)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a broken sword */
			//v1.1.29 分岐がおかしかったので直した
			if(one_in_(2))
				object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_BROKEN_SWORD));
			else
				object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_BROKEN_DAGGER));

			//v1.1.29
			//apply_magic()がされてなかったが本家からだった。スカム防止だろうか？
			//でもしといたほうが面白そうなので追加しておく
			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);
			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	/*:::金銀エンゼルの缶詰生成*/
	case MON_A_GOLD:
	case MON_A_SILVER:
		if (drop_chosen_item && ((m_ptr->r_idx == MON_A_GOLD) ||
		     ((m_ptr->r_idx == MON_A_SILVER) && (r_ptr->r_akills % 5 == 0))))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Can of Toys */
			object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_KANDUME));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	/*:::ロレントは火の爆発*/
	///mon ロレント死亡処理
	case MON_ROLENTO:
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 3, y, x, damroll(20, 10), GF_FIRE, flg, -1);
		}
		break;
		///mod140424

	/*:::Mushroom hunting */
	case MON_MAGIC_MUSHROOM_RED:
	case MON_MAGIC_MUSHROOM_WHITE:
	case MON_MAGIC_MUSHROOM_BLUE:
	case MON_MAGIC_MUSHROOM_BLACK:
	case MON_MAGIC_MUSHROOM_GREEN:
	case MON_MAGIC_MUSHROOM_GAUDY:
	case MON_MAGIC_MUSHROOM_GRAY:
	case MON_MAGIC_MUSHROOM_CLEAR:
	case MON_MAGIC_MUSHROOM:
		if(drop_chosen_item && one_in_(2))
		{
			int sval;
			q_ptr = &forge;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_RED) sval = SV_MUSHROOM_MON_RED;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_WHITE) sval = SV_MUSHROOM_MON_WHITE;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_BLUE) sval = SV_MUSHROOM_MON_BLUE;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_BLACK) sval = SV_MUSHROOM_MON_BLACK;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GREEN) sval = SV_MUSHROOM_MON_GREEN;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GAUDY) sval = SV_MUSHROOM_MON_GAUDY;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GRAY) sval = SV_MUSHROOM_MON_GRAY;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_CLEAR) sval = SV_MUSHROOM_MON_CLEAR;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM) sval = SV_MUSHROOM_MON_L_BLUE;
			object_prep(q_ptr, lookup_kind(TV_MUSHROOM, sval));
			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | mo_mode);
			(void)drop_near(q_ptr, -1, y, x);

		}
		break;
	case  MON_SANAE:
		/*:::フルーツ*/
		if(drop_chosen_item )
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT ));
			q_ptr->number = randint1(5);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.49 ミスティアを倒したら鰻を落とすことにした。鰻違いだが
	case MON_MYSTIA:
		if (drop_chosen_item)
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_EEL));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_EIRIN:
		/*:::月都万象展で倒したときのみ玉手箱を落とす*/
		if(drop_chosen_item && p_ptr->inside_quest == QUEST_MOON_VAULT )
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_MARIPO_24 ));
			q_ptr->pval = -6;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_MITHRIL_GOLEM :
		/*:::ミスリルゴーレムはミスリル鉱石を1～3落とすようにする*/
		if(drop_chosen_item )
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_MITHRIL ));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_BLOODLETTER:
		/*:::カオスブレードでなく混沌武器を落とすように変更*/
		if (drop_chosen_item && (randint1(100) < 20))
		{
			/*::: Hack - カオスブレードまでの全武器がランダムで出る */
			int sval = randint1(SV_WEAPON_BLADE_OF_CHAOS);
			q_ptr = &forge;

			object_prep(q_ptr, lookup_kind(TV_SWORD, sval));

			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_WEAPON_CHAOS;

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		/*:::黒の剣はカオスブレードを落とす。1/2で高級品*/
	case MON_CHAOS_BLADE:
		if (drop_chosen_item )
		{
			int am_mode = 0L;
			if(one_in_(2)) am_mode |= (AM_GOOD | AM_GREAT);
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_BLADE_OF_CHAOS));
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		/*:::小町は死神の大鎌を落とす。1/2で高級品*/
	case MON_KOMACHI:
		if (drop_chosen_item)
		{
			int am_mode = AM_GOOD;
			if(one_in_(2)) am_mode |= AM_GREAT;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_POLEARM, SV_WEAPON_SCYTHE_OF_SLICING));
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_REIMU:
		///mod160206 霊夢は折れた針を落とす
		if(drop_chosen_item && randint1(r_ptr->level) > 20)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_BROKEN_NEEDLE ));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		/*:::清蘭は杵を落とす*/
		//ルナティックガンも落とす？
	case MON_SEIRAN:
		if (drop_chosen_item)
		{
			int am_mode = AM_GOOD;
			int k_idx;

			if(one_in_(3)) am_mode |= AM_GREAT;

			k_idx = lookup_kind(TV_HAMMER, SV_WEAPON_KINE);

			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_ORIN:
		if (drop_chosen_item)
		{
			int num,wy,wx;
			q_ptr = &forge;
			for(num = 2 + randint1(3);num > 0; num--)
			{
				scatter(&wy, &wx, y, x, 3, 0);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_ISHIZAKURA));
				(void)drop_near(q_ptr, -1, wy, wx);
			}
		}
		break;
	case MON_RINGO:
		if (drop_chosen_item)
		{
			int num,wy,wx;
			q_ptr = &forge;
			for(num = 20 + randint1(20);num > 0; num--)
			{
				scatter(&wy, &wx, y, x, 3, 0);
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
				(void)drop_near(q_ptr, -1, wy, wx);
			}
		}
		break;
		//てゐに高草郡の光る竹追加
	case MON_TEWI:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_TAKAKUSAGORI));
			q_ptr->number = 1+randint1(2);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_SUMIREKO:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_NIGHTMARE_F:
		if (drop_chosen_item && one_in_(3))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_NIGHTMARE_FRAGMENT));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;


	case MON_3FAIRIES:
		if (drop_chosen_item)
		{
			int power = randint1(100);
			q_ptr = &forge;

			if(power < 94) object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_GINZYOU));
			else if(power < 97) object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_SYUTYUU));
			else if(power < 99) object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STARS_AND_STRIPES));
			else if(power < 100) object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_BIG_EGG));
			else object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_METEORICIRON));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		//こいしはスマートフォン（菫子が落としたもの）を持っている。菫子のときのみ落とす。
	case MON_KOISHI:
		if (drop_chosen_item && (p_ptr->pclass == CLASS_SUMIREKO) && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_UDONGE:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_ULTRASONIC));
			(void)drop_near(q_ptr, -1, y, x);

		}
		break;

		//魔理沙は低確率で色々落とす
	case MON_MARISA:
		if (drop_chosen_item && randint1(dun_level) > 50 )
		{
			q_ptr = &forge;
			if(one_in_(3))
				object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MARISA_TUBO));
			else
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_MAMUSHI));

			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_KASEN:
		if (drop_chosen_item && one_in_(3))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_DOUJU));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_URUMI:
		//v1.1.69 潤美は石の赤子を落とす
		if (drop_chosen_item)
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));
			q_ptr->number =1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 針妙丸ハウス
	case	MON_SHINMYOUMARU:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SHINMYOU_HOUSE));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 吸血鬼ハウス
	case	MON_REMY:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_VAMPIRE_COFFIN));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 マイクロンフト・ボブ
	case	MON_BULLGATES:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_BOB));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 ロック　ラフノールの鏡
	case	MON_LOCKE:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MIRROR_OF_RUFFNOR));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		//v1.1.79 久侘歌　鳥の巣風エクステ
	case	MON_KUTAKA:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_KUTAKA));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.79 ロキ　スキーズブラズニル
	case	MON_LOKI:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SKIDBLADNIR));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;

	//v1.1.79 依姫　古書型コンピュータ
	case	MON_YORIHIME:
		if (drop_chosen_item)
			{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MOON_COMPUTER));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.79 ノーデンス　貝殻の戦車
	case	MON_NODENS:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_NODENS_CHARIOT));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;

	//v1.1.80 古のもの　5%で水晶
	case MON_ELDER_THING:
		if (drop_chosen_item && one_in_(20))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_ELDER_THINGS_CRYSTAL));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;


	default:
		if (!drop_chosen_item) break;

		/*:::(, /, [, \, | はそれぞれに応じたシンボルのアイテムを落とす*/
		switch (r_ptr->d_char)
		{
		case '(':
			if (dun_level > 0)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_cloak;

				/* Make a cloak */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '/':
			if (dun_level > 4)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_polearm;

				/* Make a poleweapon */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '[':
			if (dun_level > 19)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_armor;

				/* Make a hard armor */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '\\':
			//v1.1.86 丸太を倒してもアイテムを落とさないようにした
			if (dun_level > 4 && m_ptr->r_idx != MON_MARUTA)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_hafted;

				/* Make a hafted weapon */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '|':
			///mon ストームブリンガー→エルリックにする予定
			//if (m_ptr->r_idx != MON_STORMBRINGER)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_sword;

				/* Make a sword */
				make_object(q_ptr, mo_mode);

				///mod141031 浅すぎる階でデスソード倒したら空欄アイテムが落ちるのを修正
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;
	///mod140424 ドラゴン剥ぎ取り
		case 'D':
			if (randint1(r_ptr->level) > 50 && weird_luck() )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONNAIL));
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if (randint1(r_ptr->level) > 30 && weird_luck() )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONSCALE));
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

			//v1.1.47 ミスティアが水棲lシンボルを倒したら食料を得るようにしてみる
		case 'l':
			if (p_ptr->pclass == CLASS_MYSTIA && (r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				if(m_ptr->r_idx == MON_EEL)
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_EEL));
				else
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
				(void)drop_near(q_ptr, -1, y, x);

			}

		}
		break;
	}

	//エクストラモードでユニークを倒すとアイテムカードを落とす
	///mod160719 配下を倒してもカード落としたのを修正 クローンでも落ちないようになった
	if(EXTRA_MODE && (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) && drop_chosen_item)
	{
		int drop_chance;

		if(is_gen_unique(m_ptr->r_idx)) drop_chance = 100;
		else drop_chance = 50 + r_ptr->level / 2;
		if(randint0(100) < drop_chance)
		{

			int idx = m_ptr->r_idx;
			if(one_in_(4)) idx = 0;//3/4の確率で落としたキャラクターにちなんだアイテムカードになる
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
			apply_magic_itemcard(q_ptr, (dun_level+r_ptr->level)/2, idx);
			(void)drop_near(q_ptr, -1, y, x);
		}
	}

	//v1.1.22 特定の敵を倒したとき銃をドロップすることにした
	if (drop_chosen_item && !gun_free_world)
	{
		int am_mode = AM_GOOD;
		int sv=0;
		int temp;

		switch(m_ptr->r_idx)
		{
		case MON_SEIRAN:
		case MON_RINGO:
		case MON_UDONGE:
			am_mode |= AM_GREAT;
			sv = SV_FIRE_GUN_LUNATIC;
			break;
		case MON_EAGLE_RABBIT:
			if(one_in_(2))sv = SV_FIRE_GUN_LUNATIC;
			else sv = SV_FIRE_GUN_HANDGUN;
			break;

		case MON_HELL_KAPPA:
			if(one_in_(10)) sv = SV_FIRE_GUN_LASER;
			else if(one_in_(5)) sv = SV_FIRE_GUN_MAGNUM;
			else if(one_in_(3)) sv = SV_FIRE_GUN_SHOTGUN;
			else sv = SV_FIRE_GUN_MUSKET;
			break;
		case MON_GYOKUTO:
		case MON_REISEN2:
			if(one_in_(4)) sv = SV_FIRE_GUN_MUSKET;
			else  sv = SV_FIRE_GUN_INFANTRY;
			break;
		case MON_BAZOOKER:
			if(one_in_(4)) sv = SV_FIRE_GUN_ROCKET;
			else sv = SV_FIRE_GUN_HANDGUN;
			break;
		case MON_GUNNER_OF_CORWIN:
			if(one_in_(5)) sv = SV_FIRE_GUN_MUSKET;
			else if(one_in_(2)) sv = SV_FIRE_GUN_SHOTGUN;
			else sv = SV_FIRE_GUN_INFANTRY;
			break;

		case MON_CLEANING_BOT:
			sv = SV_FIRE_GUN_LASER;
			break;
		case MON_CYBER:
			sv = SV_FIRE_GUN_ROCKET;
			break;
		case MON_CYBER_KING:
			sv = SV_FIRE_GUN_ROCKET;
			am_mode |= AM_GREAT;
			break;

		case MON_ITEM_CARRIER:
			temp = randint1(dun_level) + randint1(30);
			if(weird_luck()) temp += 50;
			if(one_in_(3)) am_mode |= AM_GREAT;

			if(temp < 10) sv = SV_FIRE_GUN_HANDGUN;
			else if(temp < 25) sv = SV_FIRE_GUN_INFANTRY;
			else if(temp < 40) sv = SV_FIRE_GUN_SHOTGUN;
			else if(temp < 60) sv = SV_FIRE_GUN_MUSKET;
			else if(temp < 75) sv = SV_FIRE_GUN_MAGNUM;
			else if(temp < 90) sv = SV_FIRE_GUN_LASER;
			else sv = SV_FIRE_GUN_ROCKET;

		}

		if(sv)
		{
			int k_idx = lookup_kind(TV_GUN, sv);
			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}

	}


	//輝夜神宝ドロップ
	if(p_ptr->pclass == CLASS_KAGUYA && drop_chosen_item && !ironman_no_fixed_art)
	{
		artifact_type *a_ptr;
		int a_idx;

		for(i=0;i<KAGUYA_QUEST_MAX;i++)
		{

			//p_ptr->magic_num[0-4]がシャッフル済みモンスターリストになっており、
			//リストにあるモンスターを倒した場合kaguya_quest_art_list[]の同じ添字の神宝生成処理
			if(m_ptr->r_idx != p_ptr->magic_num1[i]) continue;
			a_idx = kaguya_quest_art_list[i];
			a_ptr = &a_info[a_idx];

			if(a_ptr->cur_num) break;

			if (create_named_art(a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
			}
			else if (!preserve_mode) a_ptr->cur_num = 1;

			break;
		}

	}



/*:::さらに別の分岐でアイテムドロップ処理　*/
/*:::chance値とa_idxで特定の敵が特定確率で特定アーティファクトを落とす処理*/
/*:::chanceがrand(0-99)を上回れば該当アーティファクト生成。wizardは必ずチェックを通る。*/
	///mon item 一定確率のアーティファクトドロップ処理
	/* Mega-Hack -- drop fixed items */
	if (drop_chosen_item)
	{
		int a_idx = 0;
		int chance = 0;
		int chance_bonus = 0;

		int kamiyama_card_num = count_ability_card(ABL_CARD_KAMIYAMA);

		switch (m_ptr->r_idx)
		{
		case MON_OBERON:
			if (one_in_(3))
			{
				a_idx = ART_JUDGE;
				chance = 33;
			}
			else
			{
				a_idx = ART_AMBER;
				chance = 50;
			}
			break;
		///del131224 Tシャツ廃止
	/*
		case MON_GHB:
			a_idx = ART_GHB;
			chance = 100;
			break;
	*/
		///mod ストブリ→エルリック
		case MON_STORMBRINGER:
			a_idx = ART_STORMBRINGER;
			chance = 100;
			break;

		case MON_YORIHIME:
			a_idx = ART_YORIHIME;
			chance = 100;
			break;

		case MON_TOYOHIME:
			a_idx = ART_SHIOMITSUTAMA;
			chance = 66;
			break;

		case MON_NITORI:
			a_idx = ART_CROWBAR;
			chance = 80;
			break;

		case MON_YUUKA:
			a_idx = ART_YUUKA;
			chance = 75;
			break;

		case MON_YUGI:
			a_idx = ART_HOSHIGUMA;
			chance = 100;
			break;

		case MON_KASEN:
			a_idx = ART_ONIKIRIMARU;
			chance = 25;
			break;


		case MON_MAYUMI:
			a_idx = ART_MAYUMI;
			chance = 100;
			break;


		case MON_ECHIZEN:
			if(!gun_free_world)
			{
				a_idx = ART_CRIMSON;
				chance = 50;
			}
			break;

		case MON_NINJA_SLAYER:
			a_idx = ART_MENPO;
			chance = 10;
			break;

		case MON_GANDALF:
			a_idx = ART_ICANUS;
			chance = 20;
			break;

		///mon 草薙の剣は強化したので5%程度に下げる　天の羽々切を装備してれば50%にしてみる？
		///mod131224 草薙の剣ドロップはとりあえず3%に
		case MON_OROCHI:
			a_idx = ART_KUSANAGI;
			chance = 3;
			break;
			///mod150308　避来矢追加
		case MON_G_MUKADE:
		case MON_MOMOYO:
			a_idx = ART_HIRAISHI;
			chance = 50;
			break;

		case MON_DWORKIN:
		case MON_ERIC:
			a_idx = ART_JUDGE;
			chance = 20;
			break;
		case MON_MARIO:
			a_idx = ART_MARIO;
			chance = 10;
			break;

		case MON_SAURON:
			if (one_in_(10))
			{
				a_idx = ART_POWER;
				chance = 100;
			}
			else
			{
				a_idx = ART_AHO;
				chance = 100;
			}
			break;

		case MON_BRAND:
			if (!one_in_(3))
			{
				a_idx = ART_BRAND;
				chance = 25;
			}
			else
			{
				a_idx = ART_WEREWINDLE;
				chance = 33;
			}
			break;


		case MON_CORWIN:

			if(one_in_(3))
			{
				a_idx = ART_JUDGE;
				chance = 20;

			}
			else if (one_in_(2))
			{
				a_idx = ART_GRAYSWANDIR;
				chance = 33;
			}
			else
			{
				a_idx = ART_CORWIN;
				chance = 33;
			}
			break;
		case MON_BENEDICT:
			a_idx = ART_METALHAND;
			chance = 25;
			break;


		case MON_SURTUR:
			a_idx = ART_TWILIGHT;
			chance = 66;
			break;

		case MON_KRONOS:
			a_idx = ART_KRONOS;
			chance = 33;
			break;

		case MON_SARUMAN:
			a_idx = ART_ELENDIL;
			chance = 33;
			break;

		case MON_FIONA:
			a_idx = ART_FIONA;
			chance = 50;
			break;

		case MON_JULIAN:
			a_idx = ART_JULIAN;
			chance = 45;
			break;

		case MON_KLING:
			a_idx = ART_DESTINY;
			chance = 40;
			break;
		///mod131224 ゴエモンが斬鉄剣落とす確率を20%にした
		case MON_GOEMON:
			a_idx = ART_ZANTETSU;
			chance = 20;
			break;

		case MON_HAGEN:
			a_idx = ART_HAGEN;
			chance = 66;
			break;

		case MON_CAINE:
			a_idx = ART_CAINE;
			chance = 50;
			break;

		case MON_BULLGATES:
			a_idx = ART_WINBLOWS;
			chance = 66;
			break;

		case MON_LUNGORTHIN:
			a_idx = ART_CALRIS;
			chance = 50;
			break;

		case MON_JACK_SHADOWS:
			a_idx = ART_JACK;
			chance = 15;
			break;

		case MON_DIO:
			a_idx = ART_STONEMASK;
			chance = 20;
			break;

		case MON_BELD:
			a_idx = ART_SOULCRUSH;
			chance = 10;
			break;

		case MON_PIP:
			a_idx = ART_EXCALIBUR_J;
			chance = 50;
			break;

			///item mon 変更予定？
		///del131224 酒呑童子は削除するかもしれないのでドロップ処理も消しとく
/*
		case MON_SHUTEN:
			a_idx = ART_SHUTEN_DOJI;
			chance = 33;
			break;
*/
		case MON_GOTHMOG:
			a_idx = ART_GOTHMOG;
			chance = 33;
			break;

		case MON_FUNDIN:
			a_idx = ART_FUNDIN;
			chance = 5;
			break;

		case MON_ROBIN_HOOD:
			a_idx = ART_ROBIN_HOOD;
			chance = 20;///5→20
			break;

		///mod140424 色々ドロップ追加
		case MON_YOUMU:
			if (one_in_(2))
			{
				a_idx = ART_ROUKANKEN;
				chance = 100;
			}
			else
			{
				a_idx = ART_HAKUROUKEN;
				chance = 100;
			}
			break;

		case MON_IKU:
			a_idx = ART_HEAVENLY_MAIDEN;
			chance = 20;
			break;

		case MON_TENSHI:
			a_idx = ART_HISOU;
			chance = 20;
			break;
		case MON_SUIKA:
			a_idx = ART_IBUKI;
			chance = 30;
			break;
		case MON_SHOU:
			a_idx = ART_HOUTOU;
			chance = 75;
			break;
		case MON_NAZRIN:
			a_idx = ART_NAZRIN;
			chance = 50;
			break;
		case MON_SHINMYOUMARU:
			a_idx = ART_SHINMYOUMARU;
			chance = 50;
			break;
		case  MON_G_CTHULHU:
			a_idx = ART_WRIST;
			chance = 100;
			break;
		case MON_CLOWNPIECE:
			a_idx = ART_CLOWNPIECE;
			chance = 80;
			break;
		case MON_HECATIA1:
			a_idx = ART_HECATIA;
			chance = 100;
			break;
		case MON_YUKARI:
			a_idx = ART_YUKARI;
			chance = 70;
			break;
		case MON_MARISA:
			if (EXTRA_MODE)
			{
				a_idx = ART_GRIMOIRE_OF_MARISA;
				chance = (r_ptr->level - 20);
				if (chance < 0) chance = 0;
			}
			else if (r_ptr->flags2 & RF2_TROPHY)
			{
				a_idx = ART_HANGOKU_SAKE;
				chance = 100;
			}
			break;
		case MON_SUMIREKO:
			if(!gun_free_world)
			{
				a_idx = ART_3DPRINTER_GUN;
				chance = 70;
			}
			break;
		case MON_3FAIRIES:
			a_idx = ART_KODUCHI_TENGU;
			chance = 20;
			break;

		case MON_KOSUZU:
			a_idx = ART_HYAKKI;
			chance = 100;
			break;

		//v1.1.48追加 ファフナーが守っていた財宝のひとつ
		case MON_FAFNER:
			a_idx = ART_ANDVARI;
			chance = 50;
			break;



		}

		//v1.1.86 「神山への供物」カードによるドロップ率ボーナス EXTRAのボーナスと倍々にはならないよう書き直し
		if (EXTRA_MODE) chance_bonus = chance;

		if (kamiyama_card_num)
		{
			int prob_add = calc_ability_card_prob(ABL_CARD_KAMIYAMA, kamiyama_card_num);
			chance_bonus += chance * prob_add / 100;
		}

		chance += chance_bonus;

		if (p_ptr->wizard) msg_format("chance:%d (bonus:%d)", chance, chance_bonus);


		if ((a_idx > 0) && ((randint0(100) < chance) || p_ptr->wizard))
		{
			artifact_type *a_ptr = &a_info[a_idx];

			if (!a_ptr->cur_num)
			{
				/* Create the artifact */
				if (create_named_art(a_idx, y, x))
				{
					a_ptr->cur_num = 1;

					/* Hack -- Memorize location of artifact in saved floors */
					if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
				}
				else if (!preserve_mode) a_ptr->cur_num = 1;
			}
		}

		/*:::ダンジョンの主によるアーティファクトドロップ*/
		if ((r_ptr->flags7 & RF7_GUARDIAN) && (d_info[dungeon_type].final_guardian == m_ptr->r_idx))
		{
			/*:::final_objectは新生の薬など一般アイテム報酬のダンジョン　それがないときは★が報酬なので★が生成されなかったとき用に獲得の巻物の生成準備をする*/
			int k_idx = d_info[dungeon_type].final_object ? d_info[dungeon_type].final_object
				: lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);

			if (d_info[dungeon_type].final_artifact)
			{
				int a_idx = d_info[dungeon_type].final_artifact;
				artifact_type *a_ptr = &a_info[a_idx];

				if (!a_ptr->cur_num)
				{
					/* Create the artifact */
					if (create_named_art(a_idx, y, x))
					{
						a_ptr->cur_num = 1;

						/* Hack -- Memorize location of artifact in saved floors */
						if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
					}
					else if (!preserve_mode) a_ptr->cur_num = 1;

					/* Prevent rewarding both artifact and "default" object */
					/*:::★が生成されたので獲得の巻物がドロップしないようにする*/
					if (!d_info[dungeon_type].final_object) k_idx = 0;
				}
			}
			/*:::一般アイテムのダンジョン制覇報酬　もしくは報酬の★をすでに持っているとき*/
			if (k_idx)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Prepare to make a reward */
				object_prep(q_ptr, k_idx);

				apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | AM_GOOD);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
#ifdef JP
			msg_format("あなたは%sを制覇した！",d_name+d_info[dungeon_type].name);
#else
			msg_format("You have conquered %s!",d_name+d_info[dungeon_type].name);
#endif
			set_deity_bias(DBIAS_WARLIKE, 5);
			set_deity_bias(DBIAS_REPUTATION, 3);

			//v1.1.25 フラグ処理
			flag_dungeon_complete[dungeon_type] = 1;

		}
	}

	//v1.1.86 鬼傑組長の脅嚇カードの効果　60%ドロップや90%ドロップを必ず落とすことにし、さらにドロップ量増加
	yachie_card_num = count_ability_card(ABL_CARD_YACHIE);

	/*:::通常の敵のアイテムドロップ*/
	/*:::例えば20個までのアイテムを落とす敵なら、DROP_1D2～DROP_4D2のフラグが全て立っている*/
	/* Determine how much we can drop */
	if ((r_ptr->flags1 & RF1_DROP_60) && (randint0(100) < 60 || yachie_card_num)) number++;
	if ((r_ptr->flags1 & RF1_DROP_90) && (randint0(100) < 90 || yachie_card_num)) number++;
	if  (r_ptr->flags1 & RF1_DROP_1D2) number += damroll(1, 2);
	if  (r_ptr->flags1 & RF1_DROP_2D2) number += damroll(2, 2);
	if  (r_ptr->flags1 & RF1_DROP_3D2) number += damroll(3, 2);
	if  (r_ptr->flags1 & RF1_DROP_4D2) number += damroll(4, 2);


	//v1.1.86 「鬼傑組長の脅嚇」カードの効果
	if(number && yachie_card_num)
	{
		int add = 0;
		int mult = 10;

		if (yachie_card_num>1) mult += (yachie_card_num - 1) * 2;
		if (yachie_card_num >= 9) mult = 30;


		add += number * mult / 100;
		if (randint1(100) <= (number*mult % 100)) add++;

		if (p_ptr->wizard) msg_format("number:%d add:%d", number, add);

		number += add;

	}



	if (cloned && !(r_ptr->flags1 & RF1_UNIQUE))
		number = 0; /* Clones drop no stuff unless Cloning Pits */

	//v1.1.48 スーパー貧乏神のときはモンスターがアイテムを落とさない(特殊処理のドロップはする)
	if (SUPER_SHION)
		number = 0;

	if (is_pet(m_ptr) || p_ptr->inside_battle || p_ptr->inside_arena)
		number = 0; /* Pets drop no stuff */
	if (!drop_item && (r_ptr->d_char != '$')) number = 0;

	if((r_ptr->flags2 & (RF2_MULTIPLY)) && (r_ptr->r_akills > 1024))
		number = 0; /* Limit of Multiply monster drop */

	//Extraモードでダンジョン生成後に生成された非ユニークはドロップが減る
	if(EXTRA_MODE && number && (m_ptr->mflag & MFLAG_EX_RESTRICT))
	{
		number = randint1(number+1) / 2;
		if(r_ptr->r_akills > 32) number = number * 32 / (r_ptr->r_akills);
	}

	//霊夢は敵を倒すとお賽銭を得る
	if(p_ptr->pclass == CLASS_REIMU && !is_pet(m_ptr))
	{
		int amount = r_ptr->level * r_ptr->level * (50 + p_ptr->stat_ind[A_CHR]) / 200;
		if(!amount) amount = 1;
		if(r_ptr->flags1 & RF1_UNIQUE) amount *= 10;
		if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) amount *= 3;
		else if(r_ptr->flags3 & RF3_ANG_CHAOS) amount *= 2;
		else amount /= 2;

		gain_osaisen(amount);
	}

	/* Hack -- handle creeping coins */
	/*:::クリーピングコイン用の財宝の種類指定*/
	coin_type = force_coin;

	/* Average dungeon and monster levels */
	object_level = (dun_level + r_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Make Gold */
		if (do_gold && (!do_item || (randint0(100) < 50)))
		{
			/* Make some gold */
			if (!make_gold(q_ptr)) continue;

			/* XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if (!make_object(q_ptr, mo_mode)) continue;

			/* XXX XXX XXX */
			dump_item++;
		}

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* Reset "coin" type */
	coin_type = 0;


	/* Take note of any dropped treasure */
	/*:::モンスターが落とすアイテムについての思い出更新？*/
	if (visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(m_idx, dump_item, dump_gold);
	}

	//v1.1.17 純狐が月面勢力を倒した時メッセージ
	if(p_ptr->pclass == CLASS_JUNKO && (r_ptr->flags3 & RF3_GEN_MOON) && !is_pet(m_ptr))
	{
		junko_msg();
	}


	/* Only process "Quest Monsters" */
	if (!(r_ptr->flags1 & RF1_QUESTOR)) return;
	if (p_ptr->inside_battle) return;
	if (m_ptr->mflag & MFLAG_EPHEMERA) return;


	if ((m_ptr->r_idx == MON_YUKARI)  && !cloned && !p_ptr->inside_arena)
	{
		if(difficulty == DIFFICULTY_EASY)
		{
			msg_print(_("「おめでとう。初心者モード卒業ですわ。」", "'Congratulations. Beginner mode clear.'"));
			msg_print(_("「次はもっと高い難易度で会いましょう。」", "'Let's meet again next time on a higher difficulty level.'"));
			p_ptr->total_winner = TRUE;
		}
		else if(!EXTRA_MODE)
		{
			int tmp;
			if (difficulty >= DIFFICULTY_HARD && one_in_(2) )
			{
				msg_print(_("「あとは任せた！おやすみー！」", "'I'm leaving the rest to you! Off to sleep now!'"));

			}
			else
			{
				msg_print(_("「いつの間にこれほどの力を・・」", "'Such power... Since when...'"));
				msg_print(_("「しょうがないわね。この先に進みたければ勝手になさい。」", "'I guess I have no choice. Feel free to proceed further.'"));
				msg_print(_("「出来れば一番下にいる迷惑な余所者を追い払ってくれたら助かるわ。」",
                            "'It'd be appreciated if you'd dispose of a troublesome stranger on the lowest floor.'"));

			}
			//ノーマルは紫を倒せば引退可能
			if(difficulty == DIFFICULTY_NORMAL) p_ptr->total_winner = TRUE;

			/*:::Mega Hack - 紫を倒した時オベロンクエスト受領*/
			tmp = p_ptr->inside_quest;
			p_ptr->inside_quest = QUEST_OBERON;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);

			quest[QUEST_OBERON].status = QUEST_STATUS_TAKEN;
			p_ptr->inside_quest = tmp;
		}
		//Extraモードでは何もしない

		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(500000L);

	}


	if ((m_ptr->r_idx == MON_SERPENT) && !cloned)
	{
		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("あなたは変愚蛮怒(勝手版)の*真の勝利者*となった！",
                                                "You become *TRUE WINNER* of Hengband (Touhou Katteban)!"));

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_FINAL_QUEST_CLEAR);//v1.1.58

		msg_print("*** CONGRATULATIONS ***");
		msg_print(_("あなたはゲームをコンプリートしました。", "You have completed the game."));
		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(3000000L);

		//v1.1.25 フラグ処理
		flag_dungeon_complete[DUNGEON_CHAOS] = 1;

		//紫苑がスーパー貧乏神の状態でJを倒したときのフラグ処理
		if (SUPER_SHION)
		{
			p_ptr->magic_num2[0]++;
		}

	}

	/* Winner? */
	///sys サーペント倒したときのメッセージとフラグ処理　オベロンや紫に変更しよう
	if ((m_ptr->r_idx == MON_OBERON) && !cloned)
	{
		/* Total winner */
		p_ptr->total_winner = TRUE;

		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, _("見事に変愚蛮怒(勝手版)の勝利者となった！",
                                                "Became a Winner of Hengband (Touhou Katteban)!"));

		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(2000000L);
		if (p_ptr->pclass == CLASS_MAID)
			msg_print(_("主からのメッセージが届いた。あなたのために祝宴を開いてくれるそうだ。",
                        "A message arrives from your master. A feast is being held in your honor."));

		if ( /* (p_ptr->pclass == CLASS_CHAOS_WARRIOR) || */  (p_ptr->muta2 & MUT2_CHAOS_GIFT))
		{
			msg_format(_("%sからの声が響いた。", "The voice of %s echoes..."), chaos_patrons[p_ptr->chaos_patron]);
			msg_print(_("『よくやった、定命の者よ！』", "'Well done, Chosen One!'"));
		}

		/* Congratulations */
		msg_print("*** GAME CLEAR ***");

		///msg140304
		msg_print(_("Qコマンドでゲームを終了してスコアを残すことができます。",
                    "You can finish the game and record your score with the 'Q' command."));

		//v1.1.25 フラグ処理
		flag_dungeon_complete[DUNGEON_ANGBAND] = 1;


	}
}

/*
 * Modify the physical damage done to the monster.
 * (for example when it's invulnerable or shielded)
 *
 * ToDo: Accept a damage-type to calculate the modified damage from
 * things like fire, frost, lightning, poison, ... attacks.
 *
 * "type" is not yet used and should be 0.
 */
/*:::無傷球とはぐれメタルのダメージ処理*/
int mon_damage_mod(monster_type *m_ptr, int dam, bool is_psy_spear)
{
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	if ((r_ptr->flagsr & RFR_RES_ALL) && dam > 0)
	{
		dam /= 100;
		if ((dam == 0) && one_in_(3)) dam = 1;
	}

	if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
	{
		if (is_psy_spear)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print(_("輝く刃がひらり布を切り裂いた！",
                                                            "The Psycho-Spear cuts through the Nimble Fabric!"));
		}
		//物理攻撃だけのつもりだったが何しようが貫通する。まあいいか
		else if (p_ptr->pclass == CLASS_FLAN)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print(_("破壊の力がひらり布を貫通した！",
                                                            "The power of destruction pierces the Nimble Fabric!"));
		}
		else if (p_ptr->pclass == CLASS_YUGI)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print(_("あなたの豪腕はひらり布を物ともしなかった！",
                                                            "The Nimble Fabric didn't stop you!"));
		}
		else if(!one_in_(7))
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print(_("妙な布で攻撃を逸らされた。",
                                                            "A weird cloth diverts the attack."));
			return (0);
		}

	}

	if (MON_INVULNER(m_ptr))
	{
		if (is_psy_spear || p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI)
		{
			if (!p_ptr->blind && is_seen(m_ptr))
			{
#ifdef JP

				if( p_ptr->pclass == CLASS_YUGI)
					msg_print("バリアを引き裂いた！");
				else
					msg_print("バリアを切り裂いた！");
#else
				if( p_ptr->pclass == CLASS_YUGI)
					msg_print("The barrier is torn down!");
				else
					msg_print("The barrier is penetrated!");
#endif
			}
		}
		else if (!one_in_(PENETRATE_INVULNERABILITY))
		{
			return (0);
		}
	}
	return (dam);
}


/*
 * Calculate experience point to be get
 *
 * Even the 64 bit operation is not big enough to avoid overflaw
 * unless we carefully choose orders of multiplication and division.
 *
 * Get the coefficient first, and multiply (potentially huge) base
 * experience point of a monster later.
 */
/*:::ダメージ量から経験値を計算する。*/
/*:::加速の高いモンスターは経験値が高く、増殖モンスターは経験値が下がる*/
static void get_exp_from_mon(int dam, monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	s32b new_exp;
	u32b new_exp_frac;
	s32b div_h;
	u32b div_l;

	if (!m_ptr->r_idx) return;
	if (is_pet(m_ptr) || p_ptr->inside_battle) return;

	/*
	 * - Ratio of monster's level to player's level effects
	 * - Varying speed effects
	 * - Get a fraction in proportion of damage point
	 */
	new_exp = r_ptr->level * SPEED_TO_ENERGY(m_ptr->mspeed) * dam;
	new_exp_frac = 0;
	div_h = 0L;
	div_l = (p_ptr->max_plv+2) * SPEED_TO_ENERGY(r_ptr->speed);

	//v1.1.89 百々世蠱毒のグルメ
	if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[3])
		new_exp *= 2;

	/* Use (average maxhp * 2) as a denominator */
	if (!(r_ptr->flags1 & RF1_FORCE_MAXHP))
		s64b_mul(&div_h, &div_l, 0, r_ptr->hdice * (ironman_nightmare ? 2 : 1) * (r_ptr->hside + 1));
	else
		s64b_mul(&div_h, &div_l, 0, r_ptr->hdice * (ironman_nightmare ? 2 : 1) * r_ptr->hside * 2);

	//地上では経験値が1/5になるらしい。スカム防止か？
	/* Special penalty in the wilderness */
	if (!dun_level && (!(r_ptr->flags8 & RF8_WILD_ONLY) || !(r_ptr->flags1 & RF1_UNIQUE)))
		s64b_mul(&div_h, &div_l, 0, 5);


	/* Do division first to prevent overflaw */
	s64b_div(&new_exp, &new_exp_frac, div_h, div_l);

	/* Special penalty for mutiply-monster */
	if ((r_ptr->flags2 & RF2_MULTIPLY) || (m_ptr->r_idx == MON_DAWN))
	{
		int monnum_penarty = r_ptr->r_akills / 400;
		if (monnum_penarty > 8) monnum_penarty = 8;

		while (monnum_penarty--)
		{
			/* Divide by 4 */
			s64b_RSHIFT(new_exp, new_exp_frac, 2);
		}
	}
	/*:::Extraモードでダンジョン生成後に生成された非ユニークを倒した時経験値1/4～1/32*/
	if (m_ptr->mflag & MFLAG_EX_RESTRICT)
	{
		int dev = 2;
		if(r_ptr->r_akills > 32) dev++;
		if(r_ptr->r_akills > 64) dev++;
		if(r_ptr->r_akills > 128) dev++;
		s64b_RSHIFT(new_exp, new_exp_frac, dev);
	}

	/* Finally multiply base experience point of the monster */
	s64b_mul(&new_exp, &new_exp_frac, 0, r_ptr->mexp);

	/* Gain experience */
	gain_exp_64(new_exp, new_exp_frac);
}


/*
 * Decreases monsters hit points, handling monster death.
 *
 * We return TRUE if the monster has been killed (and deleted).
 *
 *:::モンスターへのダメージと死亡処理を行う。モンスターが死んだらTRUEを返す。
 * We announce monster death (using an optional "death message"
 * if given, and a otherwise a generic killed/destroyed message).
 *:::属性による死亡メッセージはnoteで指定される。
 *
 * Only "physical attacks" can induce the "You have slain" message.
 * Missile and Spell attacks will induce the "dies" message, or
 * various "specialized" messages.  Note that "You have destroyed"
 * and "is destroyed" are synonyms for "You have slain" and "dies".
 *
 * Hack -- unseen monsters yield "You have killed it." message.
 *
 * Added fear (DGK) and check whether to print fear messages -CWS
 *
 * Made name, sex, and capitalization generic -BEN-
 *
 * As always, the "ghost" processing is a total hack.
 *
 * Hack -- we "delay" fear messages by passing around a "fear" flag.
 *
 * XXX XXX XXX Consider decreasing monster experience over time, say,
 * by using "(m_exp * m_lev * (m_lev)) / (p_lev * (m_lev + n_killed))"
 * instead of simply "(m_exp * m_lev) / (p_lev)", to make the first
 * monster worth more than subsequent monsters.  This would also need
 * to induce changes in the monster recall code.
 */
///sys mon ＠によるモンスターへのダメージ処理
//モンスターからモンスターへのダメージ処理はmon_take_hit_mon()かproject_m()内で行われる。
bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note)
{
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	monster_type    exp_mon;

	/* Innocent until proven otherwise */
	bool        innocent = TRUE, thief = FALSE;
	//int         i;
	int         expdam;

	if(m_idx < 1 || m_idx >=m_max)
	{
		msg_format(_("ERROR:mon_take_hit()のm_idxがおかしい(%d)",
                    "ERROR: Weird m_idx (%d) in mon_take_hit()"),m_idx);
		return FALSE;
	}

	if(m_ptr->r_idx < 1 || m_ptr->r_idx >= max_r_idx)
	{
		msg_format(_("ERROR:mon_take_hit()で指定グリッドのモンスターのr_idxがおかしい(%d)",
                    "Weird r_idx (%d) for monster on specified grid in mon_take_hit()"),m_ptr->r_idx);
		delete_monster_idx(m_idx);
		return FALSE;
	}

	if(SAKUYA_WORLD)
	{
		msg_format(_("ERROR:「咲夜の世界」中にモンスターがダメージを受ける行動があった",
                    "ERROR: Monster was damaged during 'Sakuya's World'"));
		return FALSE;
	}


	(void)COPY(&exp_mon, m_ptr, monster_type);
/*:::RF7_KILL_EXP(死んだときしかEXPが入らない敵）でないとき、敵HP減少量によりダメージを得る*/
	if (!(r_ptr->flags7 & RF7_KILL_EXP))
	{
			/*:::expdam(経験値算出に使うダメージ値?)を計算 基本はダメージ値そのままでモンスターが死んだらHP0までの値　*/
		expdam = (m_ptr->hp > dam) ? dam : m_ptr->hp;
		if (r_ptr->flags6 & RF6_HEAL) expdam = (expdam+1) * 2 / 3;

		get_exp_from_mon(expdam, &exp_mon);

		/* Genocided by chaos patron */
		/*:::ここよく分からない。カオスパトロンに抹殺されたときのための処理らしいがなぜこのif文の中に?*/
		if (!m_ptr->r_idx) m_idx = 0;
	}

	/* Redraw (later) if needed */
	if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
	if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

	if(MON_CSLEEP(m_ptr))
	{
		if(one_in_(100)) set_deity_bias(DBIAS_REPUTATION, -1);
		if(one_in_(100)) set_deity_bias(DBIAS_COSMOS, -1);
	}

	/* Wake it up */
	(void)set_monster_csleep(m_idx, 0);

	///class 忍者ステルス
	/* Hack - Cancel any special player stealth magics. -LM- */
	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		set_superstealth(FALSE);
	}

	/* Genocided by chaos patron */
	if (!m_idx) return TRUE;

	/* Hurt it */
	/*:::実際にダメージ*/
	m_ptr->hp -= dam;

	/* It is dead now */
	/*:::モンスターが死んだら*/
	if (m_ptr->hp < 0)
	{
		char m_name[80];



		/* Extract monster name */
		monster_desc(m_name, m_ptr, MD_TRUE_NAME);

		//妹紅は6/7で復活する
		///mod150502 諏訪子特殊処理 「赤口（ミシャグチ）さま」発動中はworld_monster=-1になって敵の呪いなどを受けない
		//v1.1.52 新アリーナ内では6/7から3/4へ　下の正邪は新アリーナに出ないので変えない
		if (m_ptr->r_idx == MON_MOKOU && !one_in_(7) && !world_monster)
		{
			int rez_chance = p_ptr->inside_arena ? 4 : 7;

			if (!one_in_(rez_chance))
			{
				project(m_idx, 7, m_ptr->fy, m_ptr->fx, 300, GF_NUKE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
				m_ptr->hp = m_ptr->maxhp;
				msg_format(_("『 -リザレクション- 』", "[ -Resurrection- ]"));
				return FALSE;
			}
		}
		//正邪も6/7で復活する
		if (m_ptr->r_idx == MON_SEIJA && !one_in_(7))
		{
			m_ptr->maxhp = m_ptr->max_maxhp;
			m_ptr->hp = m_ptr->maxhp;
			msg_format(_("%sを倒した。", "%s is defeated."), m_name);
			msg_format(_("・・と思ったが地面に転がったのは狸のような地蔵だった。", "...But wait! There's only a tanuki-like jizo statue on the ground!"));
			return FALSE;
		}
		// クエスト「月都万象展」中は輝夜がいるかぎり永琳が復活する
		if(m_ptr->r_idx == MON_EIRIN && p_ptr->inside_quest == QUEST_MOON_VAULT)
		{
			bool flag_kaguya = FALSE;
			int i;
			for (i = 1; i < m_max; i++)
			{
				monster_type *m2_ptr = &m_list[i];
				if(m2_ptr->r_idx == MON_KAGUYA && !(m2_ptr->smart & SM_CLONED))
				{
					flag_kaguya = TRUE;
					monster_desc(m_name, m2_ptr, 0);
					break;
				}
			}
			if(flag_kaguya)
			{
				msg_format(_("「何を遊んでいるのよ永琳！」", "'Stop playing around, Eirin!'"));
				msg_format(_("輝夜は永琳を復活させた！", "Kaguya resurrects Eirin!"));
				m_ptr->maxhp = m_ptr->max_maxhp;
				m_ptr->hp = m_ptr->maxhp;
				return FALSE;
			}

		}

		//正邪（偽）は倒したとき名前が変わる
		if(m_ptr->r_idx == MON_SEIJA_D)
		{
			sprintf(m_name,_("デコイ人形", "Decoy Doll"));
		}

		/*:::野良神様の徳性変更*/
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			if(r_ptr->flags1 & RF1_UNIQUE)
			{

				if(r_ptr->flags3 & (RF3_WANTED))//お尋ね者
				{
					set_deity_bias(DBIAS_COSMOS, 5);
					set_deity_bias(DBIAS_REPUTATION, 5);
				}
				else if(r_ptr->flags3 & (RF3_GEN_FRIENDLY | RF3_ANG_FRIENDLY))//友好ユニーク
				{
					set_deity_bias(DBIAS_COSMOS, -3);
					set_deity_bias(DBIAS_WARLIKE, 2);
					set_deity_bias(DBIAS_REPUTATION, -2);
				}
				else if(r_ptr->flags3 & RF3_ANG_AMBER)
				{
					set_deity_bias(DBIAS_COSMOS, -5);//アンバーの王族
					set_deity_bias(DBIAS_REPUTATION, 3);
				}
				else if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					set_deity_bias(DBIAS_COSMOS, 1);//混沌勢力ユニーク
					if(r_ptr->level > randint1(100)) set_deity_bias(DBIAS_REPUTATION, 1);
				}
				else if(r_ptr->flags3 & RF3_ANG_COSMOS)
				{
					set_deity_bias(DBIAS_COSMOS, -3);//秩序勢力ユニーク
				}
				else if(r_ptr->flags3 & RF3_GEN_HUMAN)
				{
					set_deity_bias(DBIAS_COSMOS, -1);//幻想郷人間勢力ユニーク
					set_deity_bias(DBIAS_WARLIKE, 1);
					set_deity_bias(DBIAS_REPUTATION, -1);
				}
				else if(r_ptr->flags3 & RF3_HUMAN)
				{
					set_deity_bias(DBIAS_COSMOS, -1);//無所属人間ユニーク
				}

				else if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
				{
					if(one_in_(2))set_deity_bias(DBIAS_COSMOS, 1);//アンデッド・悪魔・妖怪ユニーク
					if(r_ptr->level > randint1(50)) set_deity_bias(DBIAS_REPUTATION, 1);
				}

				if(r_ptr->level > p_ptr->lev * 3 / 2 )//高レベルユニーク
				{
					set_deity_bias(DBIAS_REPUTATION, 1);
					set_deity_bias(DBIAS_WARLIKE, 1);
				}
				else if(r_ptr->level > p_ptr->lev )
				{
					if((r_ptr->level - p_ptr->lev) > randint1(20)) set_deity_bias(DBIAS_REPUTATION, 1);
				}


			}
			else //非ユニーク
			{
				if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					if(r_ptr->level > randint1(1000)) set_deity_bias(DBIAS_COSMOS, 1);//混沌勢力
				}
				else if(r_ptr->flags3 & RF3_ANG_COSMOS)
				{
					if(r_ptr->level > randint1(200)) set_deity_bias(DBIAS_COSMOS, -1);//秩序勢力
				}
				else if(r_ptr->flags3 & RF3_GEN_HUMAN)//幻想郷人間勢力
				{
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_COSMOS, -1);
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_WARLIKE, 1);
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_REPUTATION, -1);
				}
				else if(r_ptr->flags3 & (RF3_GEN_FRIENDLY | RF3_ANG_FRIENDLY))//友好モンスター
				{
					set_deity_bias(DBIAS_COSMOS, -1);
					set_deity_bias(DBIAS_WARLIKE, 1);
					set_deity_bias(DBIAS_REPUTATION, -1);
				}
			}

			//泥棒打撃モンスターを倒したとき(分裂する敵は除く)
			if (((r_ptr->blow[0].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[0].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[1].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[1].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[2].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[2].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[3].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[3].effect == RBE_EAT_GOLD))
				&& !(r_ptr->flags2 & RF2_MULTIPLY))
			{
					if(one_in_(5)) set_deity_bias(DBIAS_COSMOS, 1);
					if(one_in_(10)) set_deity_bias(DBIAS_REPUTATION, 1);
			}
			//隣接した敵を倒したとき
			if(m_ptr->cdis == 1)
			{
				if(randint1(p_ptr->lev * 5) < randint1(r_ptr->level)) set_deity_bias(DBIAS_WARLIKE, 1);
			}
			//混乱・盲目・狂戦士状態で敵を倒した時
			if(p_ptr->confused || p_ptr->blind || p_ptr->shero)
			{
				if(randint1(p_ptr->lev * 3) < randint1(r_ptr->level))set_deity_bias(DBIAS_WARLIKE, 1);
				if(randint1(p_ptr->lev * 2) < randint1(r_ptr->level))set_deity_bias(DBIAS_COSMOS, -1);
			}
			//HP半分以下で敵を倒したとき
			if(p_ptr->chp < p_ptr->mhp / 2)
			{
				if(randint1(p_ptr->lev) < randint1(r_ptr->level)) set_deity_bias(DBIAS_WARLIKE, 1);
			}
		}



		/*:::サンタを倒した時の特殊メッセージ*/
		if ((m_ptr->r_idx == MON_SANTACLAUS))
			msg_print("≪クリスマス中止のお知らせ≫");

		/*:::こころを倒した時の特殊メッセージ*/
		//v1.1.48 闘技場ではメッセージ出なくする。ここ通らないかもしれないが
		if ((m_ptr->r_idx == MON_KOKORO) && !p_ptr->inside_battle && !p_ptr->inside_arena)
			msg_format(_("【号外】%s氏、ダンジョンから大量のお面を持ち帰る",
                        "[Extra] %s returns from the dungeon with a large quantity of masks"),player_name);

		/*:::たぬきとカメレオンは正体を現す　それぞれ少し処理が違う*/
		if (r_info[m_ptr->r_idx].flags7 & RF7_TANUKI)
		{
			/* You might have unmasked Tanuki first time */
			r_ptr = &r_info[m_ptr->r_idx];
			m_ptr->ap_r_idx = m_ptr->r_idx;
			if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
		}

		if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
		{
			/* You might have unmasked Chameleon first time */
			r_ptr = real_r_ptr(m_ptr);
			if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
		}

		//v1.1.51 新アリーナ内ではモンスターの打倒数が増えないことにする
		if (!p_ptr->inside_arena)
		{
			if (!(m_ptr->smart & SM_CLONED) && !(m_ptr->mflag & MFLAG_EPHEMERA))
			{
				/* When the player kills a Unique, it stays dead */
				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					r_ptr->max_num = 0;

					/* Mega-Hack -- Banor & Lupart */
					/*:::バーノール＝ルパート用の処理　一人倒したら残りも新たに出現しないようにする*/
					if ((m_ptr->r_idx == MON_BANOR) || (m_ptr->r_idx == MON_LUPART))
					{
						r_info[MON_BANORLUPART].max_num = 0;
						r_info[MON_BANORLUPART].r_pkills++;
						r_info[MON_BANORLUPART].r_akills++;
						if (r_info[MON_BANORLUPART].r_tkills < MAX_SHORT) r_info[MON_BANORLUPART].r_tkills++;
					}
					else if (m_ptr->r_idx == MON_BANORLUPART)
					{
						r_info[MON_BANOR].max_num = 0;
						r_info[MON_BANOR].r_pkills++;
						r_info[MON_BANOR].r_akills++;
						if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
						r_info[MON_LUPART].max_num = 0;
						r_info[MON_LUPART].r_pkills++;
						r_info[MON_LUPART].r_akills++;
						if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
					}
				}

				/* When the player kills a Nazgul, it stays dead */
				/*:::ナズグルの場合出現可能数を減らす*/
				else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num--;
			}

			/* Count all monsters killed */
			/*:::r_akills加算　この＠によるこの種族のモンスター打倒数　知らずに倒したぶん含む*/
			if (r_ptr->r_akills < MAX_SHORT) r_ptr->r_akills++;

			/* Recall even invisible uniques or winners */
			if ((m_ptr->ml && !p_ptr->image) || (r_ptr->flags1 & RF1_UNIQUE))
			{
				/*::: Hack 既知カウントが0の場合足しとく*/
				if (!r_ptr->r_sights) r_ptr->r_sights++;



				/*:::r_pkills加算 今の＠によるこの種族のモンスター打倒数 不可視のとき非ユニークを倒した場合加算されない*/
				/* Count kills this life */
				if ((m_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_pkills < MAX_SHORT)) r_info[MON_KAGE].r_pkills++;
				else if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;

				/* Count kills in all lives */
				/*:::r_tkills加算 全ての＠のこの種族のモンスター打倒数 不可視のとき非ユニークを倒した場合加算されない*/
				if ((m_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_tkills < MAX_SHORT)) r_info[MON_KAGE].r_tkills++;
				else if (r_ptr->r_tkills < MAX_SHORT) r_ptr->r_tkills++;

				/* Hack -- Auto-recall */
				monster_race_track(m_ptr->ap_r_idx);
			}
		//特定モンスターを打倒したときスクリーンショット獲得。保存しておいてスコア送信のときに一緒に送る。
		//v1.1.25 スコア送信復活。場所を少し移動し、オベロンと紫のときも撮ることにした。後のボスを倒したりゲームオーバーになればその都度上書きされるはず
#ifdef WORLD_SCORE
			if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_OBERON || m_ptr->r_idx == MON_YUKARI && difficulty != DIFFICULTY_EXTRA)
			{
				/* Make screen dump */
				screen_dump = make_screen_dump();
			}
#endif

		}

		/* Don't kill Amberites */
		/*:::アンバーだったら1/2で血の呪い*/
		///sys mon アンバーの血の呪い オベロンは100%にしようか
		///mod150502 諏訪子特殊処理 「赤口（ミシャグチ）さま」発動中はworld_monster=-1になって敵の呪いなどを受けない
		///mod150711 妹紅パゼストバイフェニックスのときも血の呪いを受けない
		if ((m_ptr->r_idx == MON_OBERON || (r_ptr->flags3 & RF3_ANG_AMBER) && one_in_(2)) && !world_monster && !flag_mokou_possess && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int curses = 1 + randint1(3);
			bool stop_ty = FALSE;
			int count = 0;

#ifdef JP
msg_format("%^sは恐ろしい血の呪いをあなたにかけた！", m_name);
#else
			msg_format("%^s puts a terrible blood curse on you!", m_name);
#endif

			curse_equipment(100, 50);

			do
			{
				stop_ty = activate_ty_curse(stop_ty, &count);
			}
			while (--curses);
		}


		/*:::死にセリフ*/
		if ((r_ptr->flags2 & RF2_CAN_SPEAK) &&
			!(m_ptr->mflag & MFLAG_NO_SPELL) &&
			!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1])) &&
			!p_ptr->silent_floor
			)
		{
			char line_got[1024];

			/* Dump a message */
#ifdef JP
			if (!get_rnd_line("mondeath_j.txt", m_ptr->r_idx, line_got))
#else
			if (!get_rnd_line("mondeath.txt", m_ptr->r_idx, line_got))
#endif

				msg_format("%^s %s", m_name, line_got);


		}
		///mod140505 幻想ユニーク打倒セリフは別ファイル　CAN_SPEAKフラグ不要
		//v1.1.84 TROPHYフラグ持ち(反獄王に取り憑かれている)は喋らない
		else if(is_gen_unique(m_ptr->ap_r_idx) && !(r_ptr->flags2 & RF2_TROPHY) &&
			!(m_ptr->mflag & MFLAG_NO_SPELL) &&
			!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1])) &&
			!p_ptr->silent_floor)
		{
			char line_got[1024];
			if (!get_rnd_line(_("mondeath_gen.txt", "mondeath_gen_e.txt"), m_ptr->r_idx, line_got))
			msg_format("%^s %s", m_name, line_got);
		}


///del131221 virtue
#if 0
		///sysdel virtue mon ここからしばらく徳処理など
		if (!(d_info[dungeon_type].flags1 & DF1_BEGINNER))
		{
			if (!dun_level && !ambush_flag && !p_ptr->inside_arena)
			{
				chg_virtue(V_VALOUR, -1);
			}
			else if (r_ptr->level > dun_level)
			{
				if (randint1(10) <= (r_ptr->level - dun_level))
					chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level > 60)
			{
				chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level >= 2 * (p_ptr->lev+1))
				chg_virtue(V_VALOUR, 2);
		}

		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			if (r_ptr->flags3 & (RF3_EVIL | RF3_GOOD)) chg_virtue(V_HARMONY, 2);

			if (r_ptr->flags3 & RF3_GOOD)
			{
				chg_virtue(V_UNLIFE, 2);
				chg_virtue(V_VITALITY, -2);
			}

			if (one_in_(3)) chg_virtue(V_INDIVIDUALISM, -1);
		}

		if (m_ptr->r_idx == MON_BEGGAR || m_ptr->r_idx == MON_LEPER)
		{
			chg_virtue(V_COMPASSION, -1);
		}

		if ((r_ptr->flags3 & RF3_GOOD) &&
			((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100)))
			chg_virtue(V_UNLIFE, 1);

		if (r_ptr->d_char == 'A')
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, -2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
			{
				if (r_ptr->flags3 & RF3_GOOD) chg_virtue(V_FAITH, -1);
				else chg_virtue(V_FAITH, 1);
			}
		}
		else if (r_ptr->flags3 & RF3_DEMON)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, 2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
				chg_virtue(V_FAITH, 1);
		}

		if ((r_ptr->flags3 & RF3_UNDEAD) && (r_ptr->flags1 & RF1_UNIQUE))
			chg_virtue(V_VITALITY, 2);

		if (r_ptr->r_deaths)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				chg_virtue(V_HONOUR, 10);
			}
			else if ((r_ptr->level) / 10 + (2 * dun_level) >= randint1(100))
			{
				chg_virtue(V_HONOUR, 1);
			}
		}
		if ((r_ptr->flags2 & RF2_MULTIPLY) && (r_ptr->r_akills > 1000) && one_in_(10))
		{
			chg_virtue(V_VALOUR, -1);
		}

		for (i = 0; i < 4; i++)
		{
			if (r_ptr->blow[i].d_dice != 0) innocent = FALSE; /* Murderer! */

			if ((r_ptr->blow[i].effect == RBE_EAT_ITEM)
				|| (r_ptr->blow[i].effect == RBE_EAT_GOLD))

				thief = TRUE; /* Thief! */
		}

		/* The new law says it is illegal to live in the dungeon */
		if (r_ptr->level != 0) innocent = FALSE;

		if (thief)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_JUSTICE, 3);
			else if (1+((r_ptr->level) / 10 + (2 * dun_level))
				>= randint1(100))
				chg_virtue(V_JUSTICE, 1);
		}
		else if (innocent)
		{
			chg_virtue (V_JUSTICE, -1);
		}

		if ((r_ptr->flags3 & RF3_ANIMAL) && !(r_ptr->flags3 & RF3_EVIL) && !(r_ptr->flags4 & ~(RF4_NOMAGIC_MASK))  && !(r_ptr->flags5 & ~(RF5_NOMAGIC_MASK)) && !(r_ptr->flags6 & ~(RF6_NOMAGIC_MASK)))
		{
			if (one_in_(4)) chg_virtue(V_NATURE, -1);
		}
		///virtueここまで
#endif
		//v1.1.51 新アリーナ内ではユニーク打倒が記録されない
		if ((r_ptr->flags1 & RF1_UNIQUE) && record_destroy_uniq && !p_ptr->inside_arena)
		{
			char note_buf[160];
#ifdef JP
			sprintf(note_buf, "%s%s", r_name + r_ptr->name, (m_ptr->smart & SM_CLONED) ? "(クローン)" : "");
#else
			sprintf(note_buf, "%s%s", r_name + r_ptr->name, (m_ptr->smart & SM_CLONED) ? "(Clone)" : "");
#endif
			do_cmd_write_nikki(NIKKI_UNIQUE, 0, note_buf);
		}

		/* Make a sound */
		sound(SOUND_KILL);

		//v1.1.24
		if(m_ptr->r_idx == MON_YOUR_BOAT || m_ptr->r_idx == MON_ANCIENT_SHIP)
		{
			msg_format(_("%^sは沈んだ。", "%^s sinks."), m_name);
		}

		/* Death by Missile/Spell attack */
		else if (note)
		{
			msg_format("%^s%s", m_name, note);
		}

		/* Death by physical attack -- invisible monster */
		///sysdel dead 敵を殺すメッセージ　「○○を倒した」「○○は倒れた」だけにしておこう
		else if (!m_ptr->ml)
		{
#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_format("せっかくだから%sを殺した。", m_name);
			//else
			msg_format("%sを倒した。", m_name);
#else
				msg_format("You have defeated %s.", m_name);
#endif

		}

		/* Death by Physical attack -- non-living monster */
		else if (!monster_living(r_ptr))
		{
			int i;
			bool explode = FALSE;

			for (i = 0; i < 4; i++)
			{
				if (r_ptr->blow[i].method == RBM_EXPLODE) explode = TRUE;
			}

			/* Special note at death */
			if (explode)
#ifdef JP
				msg_format("%sは爆発して粉々になった。", m_name);
#else
				msg_format("%^s explodes into tiny shreds.", m_name);
#endif
			else
			{
#ifdef JP
			//	if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//		msg_format("せっかくだから%sを倒した。", m_name);
			//	else
msg_format("%sを倒した。", m_name);
#else
				msg_format("You have defeated %s.", m_name);
#endif
			}
		}

		/* Death by Physical attack -- living monster */
		else
		{
#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_format("せっかくだから%sを葬り去った。", m_name);
			//else
			msg_format("%sを倒した。", m_name);
#else
				msg_format("You have defeated %s.", m_name);
#endif

		}
///msg131221 del 賞金首
#if 0
		if ((r_ptr->flags1 & RF1_UNIQUE) && !(m_ptr->smart & SM_CLONED) && !vanilla_town)
		{
			for (i = 0; i < MAX_KUBI; i++)
			{
				if ((kubi_r_idx[i] == m_ptr->r_idx) && !(m_ptr->mflag2 & MFLAG2_CHAMELEON))
				{
#ifdef JP
msg_format("%sの首には賞金がかかっている。", m_name);
#else
					msg_format("There is a price on %s's head.", m_name);
#endif
					break;
				}
			}
		}
#endif


		///mod140118 アリスが倒れたら人形が全て爆発する
		if (m_ptr->r_idx == MON_ALICE && !world_monster)
		{
			int i;
			bool flag_msg = TRUE;
			monster_type *tmp_m_ptr;

			for (i = m_max - 1; i >= 1; i--)
			{
				/* Access the monster */
				tmp_m_ptr = &m_list[i];

				/* Ignore "dead" monsters */
				if (!tmp_m_ptr->r_idx) continue;
				if (tmp_m_ptr->r_idx != MON_ALICE_DOLL) continue;
				if (!is_hostile(tmp_m_ptr)) continue; //v1.1.51　条件式追加
				if(tmp_m_ptr->ml && flag_msg){
					flag_msg = FALSE;
					msg_print(_("アリスの人形が爆発した！", "Alice's dolls explode!"));
				}
				project(i, 3, tmp_m_ptr->fy, tmp_m_ptr->fx, 100, GF_FIRE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
				delete_monster_idx(i);
				if(p_ptr->chp < 0 ) return (TRUE);
			}
		}

		/*:::神奈子を倒すとクエストクリア前に諏訪子が出てくる*/
		if ((m_ptr->r_idx == MON_KANAKO) &&
		    p_ptr->inside_quest == QUEST_KANASUWA)
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			if (summon_named_creature(0, dummy_y, dummy_x, MON_SUWAKO, PM_NO_ENERGY))
			{
				msg_print(_("「やるわね！次は私と遊びなさい！」", "'Not bad! Now play with me!'"));
			}
		}

		/* Generate treasure */
		/*:::モンスター死亡処理　アイテムなども落とす*/
		monster_death(m_idx, TRUE);

		/*:::イケタ処理*/
		/* Mega hack : replace IKETA to BIKETAL */
		if ((m_ptr->r_idx == MON_IKETA) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;

			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (summon_named_creature(0, dummy_y, dummy_x, MON_BIKETAL, mode))
			{
#ifdef JP
				msg_print("「ハァッハッハッハ！！私がバイケタルだ！！」");
#else
				msg_print("Uwa-hahaha!  *I* am Biketal!");
#endif
			}
		}
		//ヘカーティアの地球か月が倒されたら元の異界の体が出る
		else if ((m_ptr->r_idx == MON_HECATIA2 || m_ptr->r_idx == MON_HECATIA3) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;

			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (summon_named_creature(0, dummy_y, dummy_x, MON_HECATIA1, mode))
			{
#ifdef JP
				msg_format("%^sは体を交代した！", m_name);
#else
                msg_format("%^s changes bodies!", m_name);
#endif
			}
		}
		/*:::モズグス様変身処理追加*/
		else if ((m_ptr->r_idx == MON_MOZGUS) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;
			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
			delete_monster_idx(m_idx);
			if (summon_named_creature(0, dummy_y, dummy_x, MON_MOZGUS2, mode))
			{
#ifdef JP
				msg_print("「我　奇跡を見たり！！」");
#else
                msg_print("'BEHOLD MY MIRACLE!!'");
#endif
			}
		}
		//v1.1.42 紫苑パワーアップ復活
		else if ((m_ptr->r_idx == MON_SHION_1) &&
			!(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;
			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
			delete_monster_idx(m_idx);
			if (summon_named_creature(0, dummy_y, dummy_x, MON_SHION_2, mode))
			{
				msg_format(_("%sはスーパー貧乏神へと変化した！",
                            "%s transforms into a super poverty god!"),m_name);
			}
		}
		else
		{
			/* Delete the monster */
			delete_monster_idx(m_idx);
		}

		/* Prevent bug of chaos patron's reward */
		if (r_ptr->flags7 & RF7_KILL_EXP)
			get_exp_from_mon((long)exp_mon.max_maxhp*2, &exp_mon);
		else
			get_exp_from_mon(((long)exp_mon.max_maxhp+1L) * 9L / 10L, &exp_mon);

		//v1.1.48 紫苑はフロアでモンスターが倒れたときその経験値相当の値を「不運パワー」として蓄積する
		if (p_ptr->pclass == CLASS_SHION)
		{
			p_ptr->magic_num1[1] += r_info[m_ptr->r_idx].mexp;

		}

		/* Not afraid */
		(*fear) = FALSE;

		/* Monster is dead */
		return (TRUE);
	}

/*:::ここから先、モンスターが死んでないとき*/
#ifdef ALLOW_FEAR
/*:::モンスターが恐怖しているときダメージを与えると恐怖が消えることがある*/
	/* Mega-Hack -- Pain cancels fear */
	if (MON_MONFEAR(m_ptr) && (dam > 0))
	{
		/* Cure fear */
		if (set_monster_monfear(m_idx, MON_MONFEAR(m_ptr) - randint1(dam)))
		{
			/* No more fear */
			(*fear) = FALSE;
		}
	}
/*:::モンスターはダメージで恐怖することがあるset_monster_monfear()*/
	/* Sometimes a monster gets scared by damage */
	if (!MON_MONFEAR(m_ptr) && !(r_ptr->flags3 & (RF3_NO_FEAR)))
	{
		/* Percentage of fully healthy */
		int percentage = (100L * m_ptr->hp) / m_ptr->maxhp;

		/*
		 * Run (sometimes) if at 10% or less of max hit points,
		 * or (usually) when hit for half its current hit points
		 */
		if ((randint1(10) >= percentage) ||
		    ((dam >= m_ptr->hp) && (randint0(100) < 80)))
		{
			/* Hack -- note fear */
			(*fear) = TRUE;

			/* XXX XXX XXX Hack -- Add some timed fear */
			(void)set_monster_monfear(m_idx, (randint1(10) +
					  (((dam >= m_ptr->hp) && (percentage > 7)) ?
					   20 : ((11 - percentage) * 5))));
		}
	}

#endif
/*:::落馬判定らしいがここでは実行されない*/
#if 0
	if (p_ptr->riding && (p_ptr->riding == m_idx) && (dam > 0))
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0);

		if (m_ptr->hp > m_ptr->maxhp/3) dam = (dam + 1) / 2;
		if (rakuba((dam > 200) ? 200 : dam, FALSE))
		{
#ifdef JP
msg_format("%^sに振り落とされた！", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
		}
	}
#endif

	/* Not dead yet */
	return (FALSE);
}


/*
 * Get term size and calculate screen size
 */
void get_screen_size(int *wid_p, int *hgt_p)
{
	Term_get_size(wid_p, hgt_p);
	*hgt_p -= ROW_MAP + 2;
	*wid_p -= COL_MAP + 2;
	if (use_bigtile) *wid_p /= 2;
}


/*
 * Calculates current boundaries
 * Called below and from "do_cmd_locate()".
 */
/*:::ウィンドウの大きさを検出し、その升目数を計算する？*/
void panel_bounds_center(void)
{
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	panel_row_max = panel_row_min + hgt - 1;
	panel_row_prt = panel_row_min - 1;
	panel_col_max = panel_col_min + wid - 1;
	panel_col_prt = panel_col_min - 13;
}


/*
 * Map resizing whenever the main term changes size
 */
void resize_map(void)
{
	/* Only if the dungeon exists */
	if (!character_dungeon) return;

	/* Mega-Hack -- no panel yet */
	panel_row_max = 0;
	panel_col_max = 0;

	/* Reset the panels */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_TORCH | PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Forget lite/view */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Hack -- update */
	handle_stuff();

	/* Redraw */
	Term_redraw();

	/*
	 * Waiting command;
	 * Place the cursor on the player
	 */
	if (can_save) move_cursor_relative(py, px);

	/* Refresh */
	Term_fresh();
}

/*
 * Redraw a term when it is resized
 */
void redraw_window(void)
{
	/* Only if the dungeon exists */
	if (!character_dungeon) return;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Hack -- update */
	handle_stuff();

	/* Redraw */
	Term_redraw();
}


/*
 * Handle a request to change the current panel
 *
 * Return TRUE if the panel was changed.
 *
 * Also used in do_cmd_locate
 */
bool change_panel(int dy, int dx)
{
	int y, x;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Apply the motion */
	y = panel_row_min + dy * hgt / 2;
	x = panel_col_min + dx * wid / 2;

	/* Verify the row */
	if (y > cur_hgt - hgt) y = cur_hgt - hgt;
	if (y < 0) y = 0;

	/* Verify the col */
	if (x > cur_wid - wid) x = cur_wid - wid;
	if (x < 0) x = 0;

	/* Handle "changes" */
	if ((y != panel_row_min) || (x != panel_col_min))
	{
		/* Save the new panel info */
		panel_row_min = y;
		panel_col_min = x;

		/* Recalculate the boundaries */
		panel_bounds_center();

		/* Update stuff */
		p_ptr->update |= (PU_MONSTERS);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Handle stuff */
		handle_stuff();

		/* Success */
		return (TRUE);
	}

	/* No change */
	return (FALSE);
}

bool change_panel_xy(int y, int x)
{
	int dy = 0, dx = 0;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	if (y < panel_row_min) dy = -1;
	if (y > panel_row_max) dy = 1;
	if (x < panel_col_min) dx = -1;
	if (x > panel_col_max) dx = 1;

	if (!dy && !dx) return (FALSE);

	return change_panel(dy, dx);
}


/*
 * Given an row (y) and col (x), this routine detects when a move
 * off the screen has occurred and figures new borders. -RAK-
 *
 * "Update" forces a "full update" to take place.
 *
 * The map is reprinted if necessary, and "TRUE" is returned.
 */
/*:::＠が画面外に消えないようにする処理？*/
void verify_panel(void)
{
	int y = py;
	int x = px;
	int wid, hgt;

	int prow_min;
	int pcol_min;
	int max_prow_min;
	int max_pcol_min;

	/* Get size */
	get_screen_size(&wid, &hgt);

	max_prow_min = cur_hgt - hgt;
	max_pcol_min = cur_wid - wid;

	/* Bounds checking */
	if (max_prow_min < 0) max_prow_min = 0;
	if (max_pcol_min < 0) max_pcol_min = 0;

		/* Center on player */
	if (center_player && (center_running || !running))
	{
		/* Center vertically */
		prow_min = y - hgt / 2;
		if (prow_min < 0) prow_min = 0;
		else if (prow_min > max_prow_min) prow_min = max_prow_min;

		/* Center horizontally */
		pcol_min = x - wid / 2;
		if (pcol_min < 0) pcol_min = 0;
		else if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
	}
	else
	{
		prow_min = panel_row_min;
		pcol_min = panel_col_min;

		/* Scroll screen when 2 grids from top/bottom edge */
		if (y > panel_row_max - 2)
		{
			while (y > prow_min + hgt-1 - 2)
			{
				prow_min += (hgt / 2);
			}
		}

		if (y < panel_row_min + 2)
		{
			while (y < prow_min + 2)
			{
				prow_min -= (hgt / 2);
			}
		}

		if (prow_min > max_prow_min) prow_min = max_prow_min;
		if (prow_min < 0) prow_min = 0;

		/* Scroll screen when 4 grids from left/right edge */
		if (x > panel_col_max - 4)
		{
			while (x > pcol_min + wid-1 - 4)
			{
				pcol_min += (wid / 2);
			}
		}

		if (x < panel_col_min + 4)
		{
			while (x < pcol_min + 4)
			{
				pcol_min -= (wid / 2);
			}
		}

		if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
		if (pcol_min < 0) pcol_min = 0;
	}

	/* Check for "no change" */
	if ((prow_min == panel_row_min) && (pcol_min == panel_col_min)) return;

	/* Save the new panel info */
	panel_row_min = prow_min;
	panel_col_min = pcol_min;

	/* Hack -- optional disturb on "panel change" */
	if (disturb_panel && !center_player) disturb(0, 0);

	/* Recalculate the boundaries */
	panel_bounds_center();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Monster health description
 */
cptr look_mon_desc(monster_type *m_ptr, u32b mode)
{
	monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
	bool         living;
	int          perc;
	cptr desc;
	cptr attitude;
	cptr clone;

	/* Determine if the monster is "living" */
	living = monster_living(ap_r_ptr);

	/* Calculate a health "percentage" */
	perc = 100L * m_ptr->hp / m_ptr->maxhp;

	/* Healthy monsters */
	if (m_ptr->hp >= m_ptr->maxhp)
	{
		/* No damage */
#ifdef JP
		desc = living ? "無傷" : "無ダメージ";
#else
		desc = living ? "unhurt" : "undamaged";
#endif

	}

	else if (perc >= 60)
	{
#ifdef JP
		desc = living ? "軽傷" : "小ダメージ";
#else
		desc = living ? "somewhat wounded" : "somewhat damaged";
#endif

	}

	else if (perc >= 25)
	{
#ifdef JP
		desc = living ? "負傷" : "中ダメージ";
#else
		desc = living ? "wounded" : "damaged";
#endif

	}

	else if (perc >= 10)
	{
#ifdef JP
	//	desc = living ? "重傷" : "大ダメージ";
		desc = "大ダメージ";

#else
		desc = "badly damaged";
#endif

	}

	else
	{
#ifdef JP
		//desc = living ? "半死半生" : "倒れかけ";
		desc = "倒れかけ";
#else
		desc = "almost defeated";
#endif
	}


	/* Need attitude information? */
	if (!(mode & 0x01))
	{
		/* Full information is not needed */
		attitude = "";
	}
	else if(m_ptr->r_idx == MON_MASTER_KAGUYA)
	{
		attitude = _(", 主人", ", mistress");
	}
	else if (is_pet(m_ptr))
	{
#ifdef JP
		attitude = ", 配下";
#else
		attitude = ", follower";
#endif
	}
	else if (is_friendly(m_ptr))
	{
#ifdef JP
		attitude = ", 友好的";
#else
		attitude = ", friendly";
#endif
	}
	else if (p_ptr->pclass == CLASS_PARSEE && (m_ptr->mflag & MFLAG_SPECIAL))
	{
		attitude = _(", 嫉妬", ", jealous");
	}
	else
	{
#ifdef JP
		attitude = "";
#else
		attitude = "";
#endif
	}


	/* Clone monster? */
	if (m_ptr->smart & SM_CLONED)
	{
		clone = ", clone";
	}
	else if(m_ptr->mflag & MFLAG_EPHEMERA)
	{
		if(m_ptr->r_idx == MON_YOUMU || m_ptr->r_idx == MON_F_SCARLET_4 ) clone = "(分身)";
		else clone = "";
	}
	else if(monster_is_you(m_ptr->r_idx))
	{
		clone = "(2P Color)";
	}
	else
	{
		clone = "";
	}

	/* Display monster's level --- idea borrowed from ToME */
	if (ap_r_ptr->r_tkills && !(m_ptr->mflag2 & MFLAG2_KAGE))
	{
#ifdef JP
		return format("レベル%d, %s%s%s", ap_r_ptr->level, desc, attitude, clone);
#else
		return format("Level %d, %s%s%s", ap_r_ptr->level, desc, attitude, clone);
#endif
	}
	else
	{
#ifdef JP
		return format("レベル???, %s%s%s", desc, attitude, clone);
#else
		return format("Level ???, %s%s%s", desc, attitude, clone);
#endif
	}
}



/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
/*:::クイックソート　実行前にcomp_hookとswap_hookを指定する　詳細未読*/
void ang_sort_aux(vptr u, vptr v, int p, int q)
{
	int z, a, b;

	/* Done sort */
	if (p >= q) return;

	/* Pivot */
	z = p;

	/* Begin */
	a = p;
	b = q;

	/* Partition */
	while (TRUE)
	{
		/* Slide i2 */
		while (!(*ang_sort_comp)(u, v, b, z)) b--;

		/* Slide i1 */
		while (!(*ang_sort_comp)(u, v, z, a)) a++;

		/* Done partition */
		if (a >= b) break;

		/* Swap */
		(*ang_sort_swap)(u, v, a, b);

		/* Advance */
		a++, b--;
	}

	/* Recurse left side */
	ang_sort_aux(u, v, p, b);

	/* Recurse right side */
	ang_sort_aux(u, v, b+1, q);
}


/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort(vptr u, vptr v, int n)
{
	/* Sort the array */
	ang_sort_aux(u, v, 0, n-1);
}



/*** Targeting Code ***/


/*
 * Determine is a monster makes a reasonable target
 *
 * The concept of "targeting" was stolen from "Morgul" (?)
 *
 * The player can target any location, or any "target-able" monster.
 *
 * Currently, a monster is "target_able" if it is visible, and if
 * the player can hit it with a projection, and the player is not
 * hallucinating.  This allows use of "use closest target" macros.
 *
 * Future versions may restrict the ability to target "trappers"
 * and "mimics", but the semantics is a little bit weird.
 */
/*:::指定したモンスターがターゲットして適切か判定する*/
bool target_able(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	/* Monster must be alive */
	if (!m_ptr->r_idx) return (FALSE);

	/* Hack -- no targeting hallucinations */
	if (p_ptr->image) return (FALSE);

	/* Monster must be visible */
	if (!m_ptr->ml) return (FALSE);

	if (p_ptr->riding && (p_ptr->riding == m_idx)) return (TRUE);

	/* Monster must be projectable */
	if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) return (FALSE);

	/* XXX XXX XXX Hack -- Never target trappers */
	/* if (CLEAR_ATTR && (CLEAR_CHAR)) return (FALSE); */

	/* Assume okay */
	return (TRUE);
}




/*
 * Update (if necessary) and verify (if possible) the target.
 *
 * We return TRUE if the target is "okay" and FALSE otherwise.
 */
/*:::target_whoをチェックし、現在もターゲットとして問題なければその座標をtarget_row/colに格納する*/
bool target_okay(void)
{
	/* Accept stationary targets */
	if (target_who < 0) return (TRUE);

	/* Check moving targets */
	if (target_who > 0)
	{
		/* Accept reasonable targets */
		if (target_able(target_who))
		{
			monster_type *m_ptr = &m_list[target_who];

			/* Acquire monster location */
			target_row = m_ptr->fy;
			target_col = m_ptr->fx;

			/* Good target */
			return (TRUE);
		}
	}

	/* Assume no target */
	return (FALSE);
}


/*
 * Sorting hook -- comp function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by double-distance to the player.
 */
static bool ang_sort_comp_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	int da, db, kx, ky;

	/* Absolute distance components */
	kx = x[a]; kx -= px; kx = ABS(kx);
	ky = y[a]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= px; kx = ABS(kx);
	ky = y[b]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
 * Sorting hook -- comp function -- by importance level of grids
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by level of monster
 */
static bool ang_sort_comp_importance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);
	cave_type *ca_ptr = &cave[y[a]][x[a]];
	cave_type *cb_ptr = &cave[y[b]][x[b]];
	monster_type *ma_ptr = &m_list[ca_ptr->m_idx];
	monster_type *mb_ptr = &m_list[cb_ptr->m_idx];
	monster_race *ap_ra_ptr, *ap_rb_ptr;

	/* The player grid */
	if (y[a] == py && x[a] == px) return TRUE;
	if (y[b] == py && x[b] == px) return FALSE;

	/* Extract monster race */
	if (ca_ptr->m_idx && ma_ptr->ml) ap_ra_ptr = &r_info[ma_ptr->ap_r_idx];
	else ap_ra_ptr = NULL;
	if (cb_ptr->m_idx && mb_ptr->ml) ap_rb_ptr = &r_info[mb_ptr->ap_r_idx];
	else ap_rb_ptr = NULL;

	if (ap_ra_ptr && !ap_rb_ptr) return TRUE;
	if (!ap_ra_ptr && ap_rb_ptr) return FALSE;

	/* Compare two monsters */
	if (ap_ra_ptr && ap_rb_ptr)
	{
		/* Unique monsters first */
		//if ((ap_ra_ptr->flags1 & RF1_UNIQUE) && !(ap_rb_ptr->flags1 & RF1_UNIQUE)) return TRUE;
		//if (!(ap_ra_ptr->flags1 & RF1_UNIQUE) && (ap_rb_ptr->flags1 & RF1_UNIQUE)) return FALSE;

		/* 可変パラメータユニークにも対応させた */
		if ((ap_ra_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) ) && !(ap_rb_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) )) return TRUE;
		if (!(ap_ra_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) ) && (ap_rb_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) )) return FALSE;

		/* Shadowers first (あやしい影) */
		if ((ma_ptr->mflag2 & MFLAG2_KAGE) && !(mb_ptr->mflag2 & MFLAG2_KAGE)) return TRUE;
		if (!(ma_ptr->mflag2 & MFLAG2_KAGE) && (mb_ptr->mflag2 & MFLAG2_KAGE)) return FALSE;

 		/* Unknown monsters first */
		if (!ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills) return TRUE;
		if (ap_ra_ptr->r_tkills && !ap_rb_ptr->r_tkills) return FALSE;

		/* Higher level monsters first (if known) */
		if (ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills)
		{
			if (ap_ra_ptr->level > ap_rb_ptr->level) return TRUE;
			if (ap_ra_ptr->level < ap_rb_ptr->level) return FALSE;
		}

		/* Sort by index if all conditions are same */
		if (ma_ptr->ap_r_idx > mb_ptr->ap_r_idx) return TRUE;
		if (ma_ptr->ap_r_idx < mb_ptr->ap_r_idx) return FALSE;
	}

	/* An object get higher priority */
	if (cave[y[a]][x[a]].o_idx && !cave[y[b]][x[b]].o_idx) return TRUE;
	if (!cave[y[a]][x[a]].o_idx && cave[y[b]][x[b]].o_idx) return FALSE;

	/* Priority from the terrain */
	if (f_info[ca_ptr->feat].priority > f_info[cb_ptr->feat].priority) return TRUE;
	if (f_info[ca_ptr->feat].priority < f_info[cb_ptr->feat].priority) return FALSE;

	/* If all conditions are same, compare distance */
	return ang_sort_comp_distance(u, v, a, b);
}


/*
 * Sorting hook -- swap function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by distance to the player.
 */
static void ang_sort_swap_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	byte temp;

	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;

	/* Swap "y" */
	temp = y[a];
	y[a] = y[b];
	y[b] = temp;
}



/*
 * Hack -- help "select" a location (see below)
 */
static s16b target_pick(int y1, int x1, int dy, int dx)
{
	int i, v;

	int x2, y2, x3, y3, x4, y4;

	int b_i = -1, b_v = 9999;


	/* Scan the locations */
	for (i = 0; i < temp_n; i++)
	{
		/* Point 2 */
		x2 = temp_x[i];
		y2 = temp_y[i];

		/* Directed distance */
		x3 = (x2 - x1);
		y3 = (y2 - y1);

		/* Verify quadrant */
		if (dx && (x3 * dx <= 0)) continue;
		if (dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if (dy && !dx && (x4 > y4)) continue;
		if (dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* XXX XXX XXX Penalize location */

		/* Track best */
		if ((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


/*
 * Hack -- determine if a given location is "interesting"
 */
static bool target_set_accept(int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);

	/* Player grid is always interesting */
	if (player_bold(y, x)) return (TRUE);


	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);


	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Visible monsters */
	if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized object */
		if (o_ptr->marked & OM_FOUND) return (TRUE);
	}

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice object features */
		if (c_ptr->info & CAVE_OBJECT) return (TRUE);

		/* Feature code (applying "mimic" field) */
		if (have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_NOTICE)) return TRUE;
	}

	/* Nope */
	return (FALSE);
}


/*
 * Prepare the "temp" array for "target_set"
 *
 * Return the number of target_able monsters in the set.
 */
/*:::modeによってモンスターやアイテムのあるマスの一覧を作成するらしい*/
/*:::TARGET_KILL:射程内モンスターを距離順にソート*/
/*:::TARGET_LOOK:感知済みモンスターをレベル順にソート？*/
//ソートされた結果は、temp_x[],temp_y[],temp_n(個数)に格納される
static void target_set_prepare(int mode)
{
	int y, x;
	int min_hgt, max_hgt, min_wid, max_wid;

	if (mode & TARGET_KILL)
	{
		/* Inner range */
		min_hgt = MAX((py - MAX_RANGE), 0);
		max_hgt = MIN((py + MAX_RANGE), cur_hgt - 1);
		min_wid = MAX((px - MAX_RANGE), 0);
		max_wid = MIN((px + MAX_RANGE), cur_wid - 1);
	}
	else if(mode & TARGET_FULL_RANGE)
	{
		/* Inner panel */
		min_hgt = 1;
		max_hgt = cur_hgt - 1;
		min_wid = 1;
		max_wid = cur_wid - 1;
	}
	else /* not targetting */
	{
		/* Inner panel */
		min_hgt = panel_row_min;
		max_hgt = panel_row_max;
		min_wid = panel_col_min;
		max_wid = panel_col_max;
	}

	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = min_hgt; y <= max_hgt; y++)
	{
		for (x = min_wid; x <= max_wid; x++)
		{
			cave_type *c_ptr;

			/* Require "interesting" contents */
			/*:::敵、アイテム、特殊物体などあればTRUE*/
			if (!target_set_accept(y, x)) continue;

			c_ptr = &cave[y][x];

			/* Require target_able monsters for "TARGET_KILL" */
			if ((mode & (TARGET_KILL)) && !target_able(c_ptr->m_idx)) continue;

			if ((mode & (TARGET_KILL)) && !target_pet && is_pet(&m_list[c_ptr->m_idx])) continue;

			if ((mode & (TARGET_KILL)) && m_list[c_ptr->m_idx].r_idx == MON_MASTER_KAGUYA) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Set the sort hooks */
	/*:::modeによってang_sort以下で使う関数を変更しているらしい*/
	if (mode & (TARGET_KILL))
	{
		/* Target the nearest monster for shooting */
		/*:::距離でソート*/
		ang_sort_comp = ang_sort_comp_distance;
		ang_sort_swap = ang_sort_swap_distance;
	}
	else
	{
		/*:::種別、レベル別でソート？*/
		/* Look important grids first in Look command */
		ang_sort_comp = ang_sort_comp_importance;
		ang_sort_swap = ang_sort_swap_distance;
	}

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);

	if (p_ptr->riding && target_pet && (temp_n > 1) && (mode & (TARGET_KILL)))
	{
		byte tmp;

		tmp = temp_y[0];
		temp_y[0] = temp_y[1];
		temp_y[1] = tmp;
		tmp = temp_x[0];
		temp_x[0] = temp_x[1];
		temp_x[1] = tmp;
	}
}
void target_set_prepare_look(){
	target_set_prepare(TARGET_LOOK);
}
void target_set_prepare_full() {
	target_set_prepare(TARGET_FULL_RANGE);
}


/*
 * Evaluate number of kill needed to gain level
 */
static void evaluate_monster_exp(char *buf, monster_type *m_ptr)
{
	monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
	u32b num;
	s32b exp_mon, exp_adv;
	u32b exp_mon_frac, exp_adv_frac;

	if ((p_ptr->lev >= PY_MAX_LEVEL) || (p_ptr->prace == RACE_ANDROID))
	{
		sprintf(buf,"**");
		return;
	}
	else if (!ap_r_ptr->r_tkills || (m_ptr->mflag2 & MFLAG2_KAGE))
	{
		if (!p_ptr->wizard)
		{
			sprintf(buf,"??");
			return;
		}
	}


	/* The monster's experience point (assuming average monster speed) */
	exp_mon = ap_r_ptr->mexp * ap_r_ptr->level;
	exp_mon_frac = 0;
	s64b_div(&exp_mon, &exp_mon_frac, 0, (p_ptr->max_plv + 2));


	/* Total experience value for next level */
	exp_adv = player_exp[p_ptr->lev -1] * p_ptr->expfact;
	exp_adv_frac = 0;
	s64b_div(&exp_adv, &exp_adv_frac, 0, 100);

	/* Experience value need to get */
	s64b_sub(&exp_adv, &exp_adv_frac, p_ptr->exp, p_ptr->exp_frac);


	/* You need to kill at least one monster to get any experience */
	s64b_add(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);
	s64b_sub(&exp_adv, &exp_adv_frac, 0, 1);

	/* Extract number of monsters needed */
	s64b_div(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);

	/* If 999 or more monsters needed, only display "999". */
	num = MIN(999, exp_adv_frac);

	/* Display the number */
	sprintf(buf,"%03ld", (long int)num);
}


bool show_gold_on_floor = FALSE;

/*
 * Examine a grid, return a keypress.
 *
 * The "mode" argument contains the "TARGET_LOOK" bit flag, which
 * indicates that the "space" key should scan through the contents
 * of the grid, instead of simply returning immediately.  This lets
 * the "look" command get complete information, without making the
 * "target" command annoying.
 *
 * The "info" argument contains the "commands" which should be shown
 * inside the "[xxx]" text.  This string must never be empty, or grids
 * containing monsters will be displayed with an extra comma.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * Eventually, we may allow multiple objects per grid, or objects
 * and terrain features in the same grid. XXX XXX XXX
 *
 * This function must handle blindness/hallucination.
 */
static int target_set_aux(int y, int x, int mode, cptr info)
{
	cave_type *c_ptr = &cave[y][x];
	s16b this_o_idx, next_o_idx = 0;
	cptr s1 = "", s2 = "", s3 = "", x_info = "";
	bool boring = TRUE;
	s16b feat;
	feature_type *f_ptr;
	int query = '\001';
	char out_val[MAX_NLEN+80];

#ifdef ALLOW_EASY_FLOOR
	int floor_list[23], floor_num = 0;

	/* Scan all objects in the grid */
	if (easy_floor)
	{
		floor_num = scan_floor(floor_list, y, x, 0x02);

		if (floor_num)
		{
#ifdef JP
			x_info = "x物 ";
#else
			x_info = "x,";
#endif
		}
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Hack -- under the player */
	if (player_bold(y, x))
	{
		/* Description */
#ifdef JP
		s1 = "あなたは";
		s2 = "の上";
		s3 = "にいる";
#else
		s1 = "You are ";

		/* Preposition */
		s2 = "on ";
#endif
	}
	else
	{
#ifdef JP
		s1 = "ターゲット:";
#else
		s1 = "Target:";
#endif
	}

	/* Hack -- hallucination */
	if (p_ptr->image)
	{
		cptr name;
		name = _("何か奇妙な物", "something strange");


		/* Display a message */
#ifdef JP
		sprintf(out_val, "%s%s%s%s [%s]", s1, name, s2, s3, info);
#else
		sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Stop on everything but "return" */
		if ((query != '\r') && (query != '\n')) return query;

		/* Repeat forever */
		return 0;
	}


	/* Actual monsters */
	if (c_ptr->m_idx && m_list[c_ptr->m_idx].ml)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];
		monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
		char m_name[80];
		bool recall = FALSE;

		/* Not boring */
		boring = FALSE;

		/* Get the monster name ("a kobold") */
		monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);

		/* Hack -- track this monster race */
		monster_race_track(m_ptr->ap_r_idx);

		/* Hack -- health bar for this monster */
		health_track(c_ptr->m_idx);

		/* Hack -- handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			char acount[10];

			/* Recall */
			if (recall)
			{
				/* Save */
				screen_save();

				/* Recall on screen */
				screen_roff(m_ptr->ap_r_idx, 0);

				/* Hack -- Complete the prompt (again) */
#ifdef JP
				Term_addstr(-1, TERM_WHITE, format("  [r思 %s%s]", x_info, info));
#else
				Term_addstr(-1, TERM_WHITE, format("  [r,%s%s]", x_info, info));
#endif

				/* Command */
				query = inkey();

				/* Restore */
				screen_load();

				/* Normal commands */
				if (query != 'r') break;

				/* Toggle recall */
				recall = FALSE;

				/* Cleare recall text and repeat */
				continue;
			}

			/*** Normal ***/

			/* Describe, and prompt for recall */
			evaluate_monster_exp(acount, m_ptr);

			//舞と里乃のメッセージ変更
			if (CLASS_RIDING_BACKDANCE && player_bold(y, x))
				s2 = "の背後";

#ifdef JP
			sprintf(out_val, "[%s]%s%s(%s)%s%s [r思 %s%s]", acount, s1, m_name, look_mon_desc(m_ptr, 0x01), s2, s3, x_info, info);
#else
			sprintf(out_val, "[%s]%s%s%s%s(%s) [r, %s%s]", acount, s1, s2, s3, m_name, look_mon_desc(m_ptr, 0x01), x_info, info);
#endif

			prt(out_val, 0, 0);

			/* Place cursor */
			move_cursor_relative(y, x);

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = TRUE;
		}

		/* Always stop at "normal" keys */
		if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

		/* Sometimes stop at "space" key */
		if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

		/* Change the intro */
#ifdef JP
		s1 = "それは";
#else
		s1 = "It is ";
#endif


		/* Hack -- take account of gender */
#ifdef JP
		if (ap_r_ptr->flags1 & (RF1_FEMALE)) s1 = "彼女は";
#else
		if (ap_r_ptr->flags1 & (RF1_FEMALE)) s1 = "She is ";
#endif

#ifdef JP
		else if (ap_r_ptr->flags1 & (RF1_MALE)) s1 = "彼は";
#else
		else if (ap_r_ptr->flags1 & (RF1_MALE)) s1 = "He is ";
#endif


		/* Use a preposition */
#ifdef JP
		s2 = "を";
		s3 = "持っている";
#else
		s2 = "carrying ";
#endif
		///mod140710 付喪神使い
		if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)
			{
				s2 = "の";
				s3 = "付喪神だ";
			}
			else
			{
				s2 = "に";
				s3 = "魔力を込めたものだ";
			}
#else
            if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)
			{
				s2 = "a tsukumogami born of";
			}
			else
			{
				s2 = "an object imbued with magical power -";
			}
#endif
		}

		/* Scan all objects being carried */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			char o_name[MAX_NLEN];

			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

			/* Change the intro */
#ifdef JP
			s2 = "をまた";
#else
			s2 = "also carrying ";
#endif
		}

		/* Use a preposition */
#ifdef JP
		s2 = "の上";
		s3 = "にいる";
#else
		s2 = "on ";
#endif
	}


#ifdef ALLOW_EASY_FLOOR
	if (floor_num)
	{
		int min_width = 0;

		while (1)
		{
			if (floor_num == 1)
			{
				char o_name[MAX_NLEN];

				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[floor_list[0]];

				/* Describe the object */
				object_desc(o_name, o_ptr, 0);

				/* Message */
#ifdef JP
				sprintf(out_val, "%s%s%s%s[%s]",
					s1, o_name, s2, s3, info);
#else
				sprintf(out_val, "%s%s%s%s [%s]",
					s1, s2, s3, o_name, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* End this grid */
				return query;
			}

			/* Provide one cushion before item listing  */
			if (boring)
			{
				/* Display rough information about items */
#ifdef JP
				sprintf(out_val, "%s %d個のアイテム%s%s ['x'で一覧, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [x,%s]",
					s1, s2, s3, floor_num, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* No request for listing */
				if (query != 'x' && query != ' ') return query;
			}


			/** Display list of items **/

			/* Continue scrolling list if requested */
			while (1)
			{
				int i, o_idx;

				/* Save screen */
				screen_save();

				/* Display */
				show_gold_on_floor = TRUE;
				(void)show_floor(0, y, x, &min_width);
				show_gold_on_floor = FALSE;

				/* Prompt */
#ifdef JP
				sprintf(out_val, "%s %d個のアイテム%s%s [Enterで次へ, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [Enter,%s]",
					s1, s2, s3, floor_num, info);
#endif
				prt(out_val, 0, 0);


				/* Wait */
				query = inkey();

				/* Load screen */
				screen_load();

				/* Exit unless 'Enter' */
				if (query != '\n' && query != '\r')
				{
					return query;
				}

				/* Get the object being moved. */
				o_idx = c_ptr->o_idx;

				/* Only rotate a pile of two or more objects. */
				if (!(o_idx && o_list[o_idx].next_o_idx)) continue;

				/* Remove the first object from the list. */
				excise_object_idx(o_idx);

				/* Find end of the list. */
				i = c_ptr->o_idx;
				while (o_list[i].next_o_idx)
					i = o_list[i].next_o_idx;

				/* Add after the last object. */
				o_list[i].next_o_idx = o_idx;

				/* Loop and re-display the list */
			}
		}

		/* NOTREACHED */
	}
#endif /* ALLOW_EASY_FLOOR */


	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Describe it */
		if (o_ptr->marked & OM_FOUND)
		{
			char o_name[MAX_NLEN];

			/* Not boring */
			boring = FALSE;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & TARGET_LOOK)) return query;

			/* Change the intro */
#ifdef JP
			s1 = "それは";
#else
			s1 = "It is ";
#endif


			/* Plurals */
#ifdef JP
			if (o_ptr->number != 1) s1 = "それらは";
#else
			if (o_ptr->number != 1) s1 = "They are ";
#endif


			/* Preposition */
#ifdef JP
			///mod150719
			if (is_plate_grid(&cave[o_ptr->iy][o_ptr->ix]))
				s2 = "の下";
			else
				s2 = "の上";

			s3 = "に見える";
#else
			s2 = "on ";
#endif

		}
	}


	/* Feature code (applying "mimic" field) */
	feat = get_feat_mimic(c_ptr);

	/* Require knowledge about grid, or ability to see grid */
	if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
	{
		/* Forget feature */
		feat = feat_none;
	}

	f_ptr = &f_info[feat];

	/* Terrain feature if needed */
	if (boring || have_flag(f_ptr->flags, FF_REMEMBER))
	{
		cptr name;

		/* Hack -- special handling for quest entrances */
		if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
		{
			/* Set the quest number temporary */
			int old_quest = p_ptr->inside_quest;
			int j;

			/* Clear the text */
			for (j = 0; j < 10; j++) quest_text[j][0] = '\0';
			quest_text_line = 0;

			p_ptr->inside_quest = c_ptr->special;

			/* Get the quest text */
			init_flags = INIT_NAME_ONLY;

			process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
			name = format("クエスト「%s」(%d階相当)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#else
			name = format("the entrance to the quest '%s'(level %d)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#endif

			/* Reset the old quest number */
			p_ptr->inside_quest = old_quest;
		}

		/* Hack -- special handling for building doors */
		else if (have_flag(f_ptr->flags, FF_BLDG) && !p_ptr->inside_arena)
		{
			name = building[f_ptr->subtype].name;
		}
		else if (have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			name = format("%s(%d階相当)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#else
			name = format("%s(level %d)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#endif
		}
		else if (have_flag(f_ptr->flags, FF_TOWN))
		{
			name = town[c_ptr->special].name;
		}
		else if (p_ptr->wild_mode && (feat == feat_floor))
		{
#ifdef JP
			name = "道";
#else
			name = "road";
#endif
		}
		else
		{
			name = f_name + f_ptr->name;
		}


		/* Pick a prefix */
		if (*s2 &&
		    ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
		     (!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE)) ||
		     have_flag(f_ptr->flags, FF_TOWN)))
		{
#ifdef JP
			s2 = "の中";
#else
			s2 = "in ";
#endif
		}

		/* Hack -- special introduction for store & building doors -KMW- */
		if (have_flag(f_ptr->flags, FF_STORE) ||
		    have_flag(f_ptr->flags, FF_QUEST_ENTER) ||
		    (have_flag(f_ptr->flags, FF_BLDG) && !p_ptr->inside_arena) ||
		    have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			s2 = "の入口";
#else
			s3 = "";
#endif
		}




#ifndef JP
		else if (have_flag(f_ptr->flags, FF_FLOOR) ||
			 have_flag(f_ptr->flags, FF_TOWN) ||
			 have_flag(f_ptr->flags, FF_SHALLOW) ||
			 have_flag(f_ptr->flags, FF_DEEP))
		{
			s3 ="";
		}
		else
		{
			/* Pick proper indefinite article */
			s3 = (is_a_vowel(name[0])) ? "an " : "a ";
		}
#endif

		/* Display a message */
		if (p_ptr->wizard)
		{
			char f_idx_str[32];
			if (c_ptr->mimic) sprintf(f_idx_str, "%d/%d", c_ptr->feat, c_ptr->mimic);
			else sprintf(f_idx_str, "%d", c_ptr->feat);
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s] %x %s %d %d %d (%d,%d) %d", s1, name, s2, s3, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, y, x, travel.cost[y][x]);
#else
			sprintf(out_val, "%s%s%s%s [%s] %x %s %d %d %d (%d,%d)", s1, s2, s3, name, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, y, x);
#endif
		}
		else
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Always stop at "normal" keys */
		if ((query != '\r') && (query != '\n') && (query != ' ')) return query;
	}

	/* Stop on everything but "return" */
	if ((query != '\r') && (query != '\n')) return query;

	/* Repeat forever */
	return 0;
}


/*
 * Handle "target" and "look".
 *
 * Note that this code can be called from "get_aim_dir()".
 *
 * All locations must be on the current panel.  Consider the use of
 * "panel_bounds()" to allow "off-panel" targets, perhaps by using
 * some form of "scrolling" the map around the cursor.  XXX XXX XXX
 * That is, consider the possibility of "auto-scrolling" the screen
 * while the cursor moves around.  This may require changes in the
 * "update_mon()" code to allow "visibility" even if off panel, and
 * may require dynamic recalculation of the "temp" grid set.
 *
 * Hack -- targeting/observing an "outer border grid" may induce
 * problems, so this is not currently allowed.
 *
 * The player can use the direction keys to move among "interesting"
 * grids in a heuristic manner, or the "space", "+", and "-" keys to
 * move through the "interesting" grids in a sequential manner, or
 * can enter "location" mode, and use the direction keys to move one
 * grid at a time in any direction.  The "t" (set target) command will
 * only target a monster (as opposed to a location) if the monster is
 * target_able and the "interesting" mode is being used.
 *
 * The current grid is described using the "look" method above, and
 * a new command may be entered at any time, but note that if the
 * "TARGET_LOOK" bit flag is set (or if we are in "location" mode,
 * where "space" has no obvious meaning) then "space" will scan
 * through the description of the current grid until done, instead
 * of immediately jumping to the next "interesting" grid.  This
 * allows the "target" command to retain its old semantics.
 *
 * The "*", "+", and "-" keys may always be used to jump immediately
 * to the next (or previous) interesting grid, in the proper mode.
 *
 * The "return" key may always be used to scan through a complete
 * grid description (forever).
 *
 * This command will cancel any old target, even if used from
 * inside the "look" command.
 */
/*:::マップからモンスターやグリッドを選択しtarget_who/row/colに格納？詳細未読*/
bool target_set(int mode)
{
	int		i, d, m, t, bd;
	int		y = py;
	int		x = px;

	bool	done = FALSE;

	bool	flag = TRUE;

	char	query;

	char	info[80];

	cave_type		*c_ptr;

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Cancel target */
	target_who = 0;


	/* Cancel tracking */
	/* health_track(0); */


	/* Prepare the "temp" array */
	target_set_prepare(mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{


		/* Interesting grids */
		if (flag && temp_n)
		{
			y = temp_y[m];
			x = temp_x[m];

			/* Set forcus */
			change_panel_xy(y, x);

			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			///mod140428
			fix_floor_item_list(y,x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Allow target */
			if (target_able(c_ptr->m_idx))
			{
#ifdef JP
strcpy(info, "q止 t決 p自 o現 +次 -前");
#else
				strcpy(info, "q,t,p,o,+,-,<dir>");
#endif

			}

			/* Dis-allow target */
			else
			{
#ifdef JP
strcpy(info, "q止 p自 o現 +次 -前");
#else
				strcpy(info, "q,p,o,+,-,<dir>");
#endif

			}

			/* Describe and Prompt */
			while (TRUE){
				query = target_set_aux(y, x, mode, info);
				if(query)break;
			}

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no "direction" */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}

			/* Analyze */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					if (target_able(c_ptr->m_idx))
					{
						health_track(c_ptr->m_idx);
						target_who = c_ptr->m_idx;
						target_row = y;
						target_col = x;
						done = TRUE;
					}
					else
					{
						bell();
					}
					break;
				}

				case ' ':
				case '*':
				case '+':
				{
					if (++m == temp_n)
					{
						m = 0;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case '-':
				{
					if (m-- == 0)
					{
						m = temp_n - 1;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					flag = FALSE;
					break;
				}

				case 'm':
				{
					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					if (!d) bell();
					break;
				}
			}

			/* Hack -- move around */
			if (d)
			{
				/* Modified to scroll to monster */
				int y2 = panel_row_min;
				int x2 = panel_col_min;

				/* Find a new monster */
				i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

				/* Request to target past last interesting grid */
				while (flag && (i < 0))
				{
					/* Note the change */
					if (change_panel(ddy[d], ddx[d]))
					{
						int v = temp_y[m];
						int u = temp_x[m];

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at interesting grids */
						flag = TRUE;

						/* Find a new monster */
						i = target_pick(v, u, ddy[d], ddx[d]);

						/* Use that grid */
						if (i >= 0) m = i;
					}

					/* Nothing interesting */
					else
					{
						int dx = ddx[d];
						int dy = ddy[d];

						/* Restore previous position */
						panel_row_min = y2;
						panel_col_min = x2;
						panel_bounds_center();

						/* Update stuff */
						p_ptr->update |= (PU_MONSTERS);

						/* Redraw map */
						p_ptr->redraw |= (PR_MAP);

						/* Window stuff */
						p_ptr->window |= (PW_OVERHEAD);

						/* Handle stuff */
						handle_stuff();

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at boring grids */
						flag = FALSE;

						/* Move */
						x += dx;
						y += dy;

						/* Do not move horizontally if unnecessary */
						if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
							 ((x > panel_col_min + wid / 2) && (dx < 0)))
						{
							dx = 0;
						}

						/* Do not move vertically if unnecessary */
						if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
							 ((y > panel_row_min + hgt / 2) && (dy < 0)))
						{
							dy = 0;
						}

						/* Apply the motion */
						if ((y >= panel_row_min+hgt) || (y < panel_row_min) ||
						    (x >= panel_col_min+wid) || (x < panel_col_min))
						{
							if (change_panel(dy, dx)) target_set_prepare(mode);
						}

						/* Slide into legality */
						if (x >= cur_wid-1) x = cur_wid - 2;
						else if (x <= 0) x = 1;

						/* Slide into legality */
						if (y >= cur_hgt-1) y = cur_hgt- 2;
						else if (y <= 0) y = 1;
					}
				}

				/* Use that grid */
				m = i;
			}
		}

		/* Arbitrary grids */
		else
		{
			bool move_fast = FALSE;

			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			/* Access */
			c_ptr = &cave[y][x];

			///mod140428
			fix_floor_item_list(y,x);

			/* Default prompt */
#ifdef JP
strcpy(info, "q止 t決 p自 m近 +次 -前");
#else
			strcpy(info, "q,t,p,m,+,-,<dir>");
#endif


			/* Describe and Prompt (enable "TARGET_LOOK") */
			while (!(query = target_set_aux(y, x, mode | TARGET_LOOK, info)));

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no direction */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}

			/* Analyze the keypress */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					target_who = -1;
					target_row = y;
					target_col = x;
					done = TRUE;
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '-':
				case 'm':
				{
					flag = TRUE;

					m = 0;
					bd = 999;

					/* Pick a nearby monster */
					for (i = 0; i < temp_n; i++)
					{
						t = distance(y, x, temp_y[i], temp_x[i]);

						/* Pick closest */
						if (t < bd)
						{
							m = i;
							bd = t;
						}
					}

					/* Nothing interesting */
					if (bd == 999) flag = FALSE;

					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					/* XTRA HACK MOVEFAST */
					if (isupper(query)) move_fast = TRUE;

					if (!d) bell();
					break;
				}
			}

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					if (change_panel(dy, dx)) target_set_prepare(mode);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;
			}





		}



	}

	/* Forget */
	temp_n = 0;

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Failure to set target */
	if (!target_who) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Get an "aiming direction" from the user.
 *
 * The "dir" is loaded with 1,2,3,4,6,7,8,9 for "actual direction", and
 * "0" for "current target", and "-1" for "entry aborted".
 *
 * Note that "Force Target", if set, will pre-empt user interaction,
 * if there is a usable target already set.
 *
 * Note that confusion over-rides any (explicit?) user choice.
 */
/*:::ターゲットを指定する？キャンセルされたらFALSEを返す*/
bool get_aim_dir(int *dp)
{
	int		dir;

	char	command;

	cptr	p;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	/* Hack -- auto-target if requested */
	if (use_old_target && target_okay()) dir = 5;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		/* Confusion? */

		/* Verify */
		if (!(*dp == 5 && !target_okay()))
		{
/*			return (TRUE); */
			dir = *dp;
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "方向 ('*'でターゲット選択, ESCで中断)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "方向 ('5'でターゲットへ, '*'でターゲット再選択, ESCで中断)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Extract the action (if any) */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir)
	{
		project_length = 0; /* reset to default */
		return (FALSE);
	}

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("あなたは混乱している。");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* A "valid" direction was entered */
	return (TRUE);
}



/*
 * Request a "movement" direction (1,2,3,4,6,7,8,9) from the user,
 * and place it into "command_dir", unless we already have one.
 *
 * This function should be used for all "repeatable" commands, such as
 * run, walk, open, close, bash, disarm, spike, tunnel, etc, as well
 * as all commands which must reference a grid adjacent to the player,
 * and which may not reference the grid under the player.  Note that,
 * for example, it is no longer possible to "disarm" or "open" chests
 * in the same grid as the player.
 *
 * Direction "5" is illegal and will (cleanly) abort the command.
 *
 * This function tracks and uses the "global direction", and uses
 * that as the "desired direction", to which "confusion" is applied.
 */
/*:::プロンプトを出して方向を聞く
 *:::dp:方向1-9を返すポインタ
 *:::under:足元を指定可能にするかどうか
 *:::＠が混乱などしているとランダムに方向を替える
 */
bool get_rep_dir(int *dp, bool under)
{
	int dir;
	cptr prompt;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */
	/*:::リピート中ならそのdir値を返す*/
	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	if (under)
	{
		prompt = _("方向 ('.'足元, ESCで中断)? ", "Direction ('.' at feet, Escape to cancel)? ");
	}
	else
	{
		prompt = _("方向 (ESCで中断)? ", "Direction (Escape to cancel)? ");
	}

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
		/*:::promptの文字列を表示し入力を受ける*/
		if (!get_com(prompt, &ch, TRUE)) break;

		/* Look down */
		if ((under) && ((ch == '5') || (ch == '-') || (ch == '.')))
		{
			dir = 5;
		}
		else
		{
			/* Look up the direction */
			/*:::入力キーをdirの数値に変換　ローグライクキー対応*/
			dir = get_keymap_dir(ch);

			/* Oops */
			if (!dir) bell();
		}
	}

	/* Prevent weirdness */
	if ((dir == 5) && (!under)) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	/*:::混乱中や乗馬の混乱のとき方向をランダムに変更*/
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}
	else if (p_ptr->riding)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (MON_CONFUSED(m_ptr))
		{
			/* Standard confusion */
			if (randint0(100) < 75)
			{
				/* Random direction */
				dir = ddd[randint0(8)];
			}
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (r_ptr->flags1 & RF1_RAND_25) && (randint0(100) < 50))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (randint0(100) < 25))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		if (p_ptr->confused)
		{
			/* Warn the user */
#ifdef JP
msg_print("あなたは混乱している。");
#else
			msg_print("You are confused.");
#endif
		}
		else
		{
			char m_name[80];
			monster_type *m_ptr = &m_list[p_ptr->riding];

			monster_desc(m_name, m_ptr, 0);
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
msg_format("%sは混乱している。", m_name);
#else
 msg_format("%^s is confused.", m_name);

#endif
			}
			else
			{
#ifdef JP
msg_format("%sは思い通りに動いてくれない。", m_name);
#else
msg_format("You cannot control %s.", m_name);
#endif
			}
		}
	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	/*:::リピートコマンドに方向を登録？*/
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


bool get_rep_dir2(int *dp)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("方向 (ESCで中断)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if (dir == 5) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("あなたは混乱している。");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}
/*:::カオスパトロンからの報酬　詳細未読*/
void gain_level_reward(int chosen_reward)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	int         dummy = 0, dummy2 = 0;
	int         type, effect;
	cptr        reward = NULL;
	char o_name[MAX_NLEN];

	int count = 0;

	if (!chosen_reward)
	{
		if (multi_rew) return;
		else multi_rew = TRUE;
	}


	if (p_ptr->lev == 13) nasty_chance = 2;
	else if (!(p_ptr->lev % 13)) nasty_chance = 3;
	else if (!(p_ptr->lev % 14)) nasty_chance = 12;

	if (one_in_(nasty_chance))
		type = randint1(20); /* Allow the 'nasty' effects */
	else
		type = randint1(15) + 5; /* Or disallow them */

	if (type < 1) type = 1;
	if (type > 20) type = 20;
	type--;


#ifdef JP
sprintf(wrath_reason, "%sの怒り",
		chaos_patrons[p_ptr->chaos_patron]);
#else
	sprintf(wrath_reason, "the Wrath of %s",
		chaos_patrons[p_ptr->chaos_patron]);
#endif


	effect = chaos_rewards[p_ptr->chaos_patron][type];

	if (one_in_(6) && !chosen_reward)
	{
#ifdef JP
msg_format("%^sは褒美としてあなたを突然変異させた。",
			chaos_patrons[p_ptr->chaos_patron]);
#else
		msg_format("%^s rewards you with a mutation!",
			chaos_patrons[p_ptr->chaos_patron]);
#endif

		(void)gain_random_mutation(0);
#ifdef JP
		reward = "変異した。";
#else
		reward = "mutation";
#endif
	}
	else
	{
	switch (chosen_reward ? chosen_reward : effect)
	{
		///sysdel カオスパトロン自己変容　削除予定
		case REW_POLY_SLF:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝、新たなる姿を必要とせり！」");
#else
			msg_print("'Thou needst a new form, mortal!'");
#endif

			do_poly_self();
#ifdef JP
			reward = "変異した。";
#else
			reward = "polymorphing";
#endif
			break;
		case REW_GAIN_EXP:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝は良く行いたり！続けよ！」");
#else
			msg_print("'Well done, mortal! Lead on!'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
				msg_print("しかし何も起こらなかった。");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
msg_print("更に経験を積んだような気がする。");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(ee);
#ifdef JP
				reward = "経験値を得た";
#else
				reward = "experience";
#endif
			}
			break;
		case REW_LOSE_EXP:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「下僕よ、汝それに値せず。」");
#else
			msg_print("'Thou didst not deserve that, slave.'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
				msg_print("しかし何も起こらなかった。");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else
			{
				lose_exp(p_ptr->exp / 6);
#ifdef JP
				reward = "経験値を失った。";
#else
				reward = "losing experience";
#endif
			}
			break;
		case REW_GOOD_OBJ:
#ifdef JP
msg_format("%sの声がささやいた:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我が与えし物を賢明に使うべし。」");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, FALSE, FALSE);
#ifdef JP
			reward = "上質なアイテムを手に入れた。";
#else
			reward = "a good item";
#endif
			break;
		case REW_GREA_OBJ:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我が与えし物を賢明に使うべし。」");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, TRUE, FALSE);
#ifdef JP
			reward = "高級品のアイテムを手に入れた。";
#else
			reward = "an excellent item";
#endif
			break;
		case REW_CHAOS_WP:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝の行いは貴き剣に値せり。」");
#else
			msg_print("'Thy deed hath earned thee a worthy blade.'");
#endif

			/* Get local object */
			///item TVAL カオスパトロンの混沌武器生成
			///pend 混沌武器生成　ロングソードで固定した
			q_ptr = &forge;
			dummy = TV_SWORD;
			dummy2 = SV_WEAPON_LONG_SWORD;
			/*
			switch (randint1(p_ptr->lev))
			{
				case 0: case 1:
					dummy2 = SV_DAGGER;
					break;
				case 2: case 3:
					dummy2 = SV_MAIN_GAUCHE;
					break;
				case 4:
					dummy2 = SV_TANTO;
					break;
				case 5: case 6:
					dummy2 = SV_RAPIER;
					break;
				case 7: case 8:
					dummy2 = SV_SMALL_SWORD;
					break;
				case 9: case 10:
					dummy2 = SV_BASILLARD;
					break;
				case 11: case 12: case 13:
					dummy2 = SV_SHORT_SWORD;
					break;
				case 14: case 15:
					dummy2 = SV_SABRE;
					break;
				case 16: case 17:
					dummy2 = SV_CUTLASS;
					break;
				case 18:
					dummy2 = SV_WAKIZASHI;
					break;
				case 19:
					dummy2 = SV_KHOPESH;
					break;
				case 20:
					dummy2 = SV_TULWAR;
					break;
				case 21:
					dummy2 = SV_BROAD_SWORD;
					break;
				case 22: case 23:
					dummy2 = SV_LONG_SWORD;
					break;
				case 24: case 25:
					dummy2 = SV_SCIMITAR;
					break;
				case 26:
					dummy2 = SV_NINJATO;
					break;
				case 27:
					dummy2 = SV_KATANA;
					break;
				case 28: case 29:
					dummy2 = SV_BASTARD_SWORD;
					break;
				case 30:
					dummy2 = SV_GREAT_SCIMITAR;
					break;
				case 31:
					dummy2 = SV_CLAYMORE;
					break;
				case 32:
					dummy2 = SV_ESPADON;
					break;
				case 33:
					dummy2 = SV_TWO_HANDED_SWORD;
					break;
				case 34:
					dummy2 = SV_FLAMBERGE;
					break;
				case 35:
					dummy2 = SV_NO_DACHI;
					break;
				case 36:
					dummy2 = SV_EXECUTIONERS_SWORD;
					break;
				case 37:
					dummy2 = SV_ZWEIHANDER;
					break;
				case 38:
					dummy2 = SV_HAYABUSA;
					break;
				default:
					dummy2 = SV_BLADE_OF_CHAOS;
			}
			*/
			object_prep(q_ptr, lookup_kind(dummy, dummy2));
			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_WEAPON_CHAOS;

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, py, px);
#ifdef JP
			reward = "(混沌)の武器を手に入れた。";
#else
			reward = "chaos weapon";
#endif
			break;
		case REW_GOOD_OBS:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝の行いは貴き報いに値せり。」");
#else
			msg_print("'Thy deed hath earned thee a worthy reward.'");
#endif

			acquirement(py, px, randint1(2) + 1, FALSE, FALSE);
#ifdef JP
			reward = "上質なアイテムを手に入れた。";
#else
			reward = "good items";
#endif
			break;
		case REW_GREA_OBS:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「下僕よ、汝の献身への我が惜しみ無き報いを見るがよい。」");
#else
			msg_print("'Behold, mortal, how generously I reward thy loyalty.'");
#endif

			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);
#ifdef JP
			reward = "高級品のアイテムを手に入れた。";
#else
			reward = "excellent items";
#endif
			break;
		case REW_TY_CURSE:
#ifdef JP
msg_format("%sの声が轟き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「下僕よ、汝傲慢なり。」");
#else
			msg_print("'Thou art growing arrogant, mortal.'");
#endif

			(void)activate_ty_curse(FALSE, &count);
#ifdef JP
			reward = "禍々しい呪いをかけられた。";
#else
			reward = "cursing";
#endif
			break;
		case REW_SUMMON_M:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我が下僕たちよ、かの傲慢なる者を倒すべし！」");
#else
			msg_print("'My pets, destroy the arrogant mortal!'");
#endif

			for (dummy = 0; dummy < randint1(5) + 1; dummy++)
			{
				(void)summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
#ifdef JP
			reward = "モンスターを召喚された。";
#else
			reward = "summoning hostile monsters";
#endif
			break;
		case REW_H_SUMMON:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝、より強き敵を必要とせり！」");
#else
			msg_print("'Thou needst worthier opponents!'");
#endif

			activate_hi_summon(py, px, FALSE);
#ifdef JP
			reward = "モンスターを召喚された。";
#else
			reward = "summoning many hostile monsters";
#endif
			break;
		case REW_DO_HAVOC:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「死と破壊こそ我が喜びなり！」");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			call_chaos();
#ifdef JP
			reward = "カオスの力が渦巻いた。";
#else
			reward = "calling chaos";
#endif
			break;
		case REW_GAIN_ABL:
#ifdef JP
msg_format("%sの声が鳴り響いた:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s rings out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「留まるのだ、下僕よ。余が汝の肉体を鍛えん。」");
#else
			msg_print("'Stay, mortal, and let me mold thee.'");
#endif

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_inc_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				do_inc_stat(randint0(6));
#ifdef JP
			reward = "能力値が上がった。";
#else
			reward = "increasing a stat";
#endif
			break;
		case REW_LOSE_ABL:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「下僕よ、余は汝に飽みたり。」");
#else
			msg_print("'I grow tired of thee, mortal.'");
#endif

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_dec_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				(void)do_dec_stat(randint0(6));
#ifdef JP
			reward = "能力値が下がった。";
#else
			reward = "decreasing a stat";
#endif
			break;
		case REW_RUIN_ABL:
#ifdef JP
msg_format("%sの声が轟き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝、謙虚たることを学ぶべし！」");
msg_print("あなたは以前より弱くなった！");
#else
			msg_print("'Thou needst a lesson in humility, mortal!'");
			msg_print("You feel less powerful!");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), TRUE);
			}
#ifdef JP
			reward = "全能力値が下がった。";
#else
			reward = "decreasing all stats";
#endif
			break;
		case REW_POLY_WND:
#ifdef JP
msg_format("%sの力が触れるのを感じた。",
#else
			msg_format("You feel the power of %s touch you.",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
			do_poly_wounds();
#ifdef JP
			reward = "傷が変化した。";
#else
			reward = "polymorphing wounds";
#endif
			break;
		case REW_AUGM_ABL:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我がささやかなる賜物を受けとるがよい！」");
#else
			msg_print("'Receive this modest gift from me!'");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_inc_stat(dummy);
			}
#ifdef JP
			reward = "全能力値が上がった。";
#else
			reward = "increasing all stats";
#endif
			break;
		case REW_HURT_LOT:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「苦しむがよい、無能な愚か者よ！」");
#else
			msg_print("'Suffer, pathetic fool!'");
#endif

			fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
			take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
#ifdef JP
			reward = "分解の球が発生した。";
#else
			reward = "generating disintegration ball";
#endif
			break;
	   case REW_HEAL_FUL:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「甦るがよい、我が下僕よ！」");
#else
			msg_print("'Rise, my servant!'");
#endif

			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			hp_player(5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(dummy);
			}
#ifdef JP
			reward = "体力が回復した。";
#else
			reward = "healing";
#endif
			break;
		case REW_CURSE_WP:
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝、武器に頼ることなかれ。」");
#else
			msg_print("'Thou reliest too much on thy weapon.'");
#endif

			dummy = INVEN_RARM;
			if (buki_motteruka(INVEN_LARM))
			{
				dummy = INVEN_LARM;
				if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
			}
			object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
			(void)curse_weapon(FALSE, dummy);
#ifdef JP
			reward = format("%sが破壊された。", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_CURSE_AR:
			if (!inventory[INVEN_BODY].k_idx) break;
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「汝、防具に頼ることなかれ。」");
#else
			msg_print("'Thou reliest too much on thine equipment.'");
#endif

			object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
			(void)curse_armor();
#ifdef JP
			reward = format("%sが破壊された。", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_PISS_OFF:
#ifdef JP
msg_format("%sの声がささやいた:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我を怒りしめた罪を償うべし。」");
#else
			msg_print("'Now thou shalt pay for annoying me.'");
#endif

			switch (randint1(4))
			{
				case 1:
					(void)activate_ty_curse(FALSE, &count);
#ifdef JP
					reward = "禍々しい呪いをかけられた。";
#else
					reward = "cursing";
#endif
					break;
				case 2:
					activate_hi_summon(py, px, FALSE);
#ifdef JP
					reward = "モンスターを召喚された。";
#else
					reward = "summoning hostile monsters";
#endif
					break;
				case 3:
					if (one_in_(2))
					{
						if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
						dummy = INVEN_RARM;
						if (buki_motteruka(INVEN_LARM))
						{
							dummy = INVEN_LARM;
							if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
						}
						object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
						(void)curse_weapon(FALSE, dummy);
#ifdef JP
						reward = format("%sが破壊された。", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					else
					{
						if (!inventory[INVEN_BODY].k_idx) break;
						object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
						(void)curse_armor();
#ifdef JP
						reward = format("%sが破壊された。", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					break;
				default:
					for (dummy = 0; dummy < 6; dummy++)
					{
						(void)dec_stat(dummy, 10 + randint1(15), TRUE);
					}
#ifdef JP
					reward = "全能力値が下がった。";
#else
					reward = "decreasing all stats";
#endif
					break;
			}
			break;
		case REW_WRATH:
#ifdef JP
msg_format("%sの声が轟き渡った:",
#else
			msg_format("The voice of %s thunders:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_print("「死ぬがよい、下僕よ！」");
#else
			msg_print("'Die, mortal!'");
#endif

			take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), FALSE);
			}
			activate_hi_summon(py, px, FALSE);
			(void)activate_ty_curse(FALSE, &count);
			if (one_in_(2))
			{
				dummy = 0;

				if (buki_motteruka(INVEN_RARM))
				{
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM) && one_in_(2)) dummy = INVEN_LARM;
				}
				else if (buki_motteruka(INVEN_LARM)) dummy = INVEN_LARM;

				if (dummy) (void)curse_weapon(FALSE, dummy);
			}
			if (one_in_(2)) (void)curse_armor();
			break;
		case REW_DESTRUCT:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「死と破壊こそ我が喜びなり！」");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			(void)destroy_area(py, px, 25, FALSE,FALSE,FALSE);
#ifdef JP
			reward = "ダンジョンが*破壊*された。";
#else
			reward = "*destruct*ing dungeon";
#endif
			break;
		case REW_GENOCIDE:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我、汝の敵を抹殺せん！」");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)symbol_genocide(0, FALSE,0);
#ifdef JP
			reward = "モンスターが抹殺された。";
#else
			reward = "genociding monsters";
#endif
			break;
		case REW_MASS_GEN:
#ifdef JP
msg_format("%sの声が響き渡った:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("「我、汝の敵を抹殺せん！」");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)mass_genocide(0, FALSE);
#ifdef JP
			reward = "モンスターが抹殺された。";
#else
			reward = "genociding nearby monsters";
#endif
			break;
		case REW_DISPEL_C:
#ifdef JP
msg_format("%sの力が敵を攻撃するのを感じた！",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("You can feel the power of %s assault your enemies!",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			(void)dispel_monsters(p_ptr->lev * 4);
			break;
		case REW_IGNORE:
#ifdef JP
msg_format("%sはあなたを無視した。",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s ignores you.",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			break;
		case REW_SER_DEMO:
#ifdef JP
msg_format("%sは褒美として悪魔の使いをよこした！",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a demonic servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_DEMON, PM_FORCE_PET))
#ifdef JP
msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "悪魔が配下になった。";
#else
				reward = "a demonic servant";
#endif

			break;
		case REW_SER_MONS:
#ifdef JP
msg_format("%sは褒美として使いをよこした！",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, 0, PM_FORCE_PET))
#ifdef JP
msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "モンスターが配下になった。";
#else
				reward = "a servant";
#endif

			break;
		case REW_SER_UNDE:
#ifdef JP
msg_format("%sは褒美としてアンデッドの使いをよこした。",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with an undead servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_UNDEAD, PM_FORCE_PET))
#ifdef JP
msg_print("何も現れなかった...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "アンデッドが配下になった。";
#else
				reward = "an undead servant";
#endif

			break;
		default:
#ifdef JP
msg_format("%sの声がどもった:",
#else
			msg_format("The voice of %s stammers:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_format("「あー、あー、答えは %d/%d。質問は何？」", type, effect);
#else
			msg_format("'Uh... uh... the answer's %d/%d, what's the question?'", type, effect);
#endif

	}
	}
	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("パトロンの報酬で%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}

/*:::メイド用　主からの報酬*/
void gain_maid_reward(void)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         type,i,dummy;
	cptr        reward = NULL;

	if (multi_rew) return;
	else multi_rew = TRUE;
	q_ptr = &forge;

	sprintf(wrath_reason, _("主からのお仕置き", "punshiment from your master"));

	type = randint0(100);
	msg_print(_("主からの封書が飛んできた。", "An envelope from your master flies in."));
	msg_print("");


	if(type < 5)
	{
		msg_print(_("封書を開けた。", "You open the envelope."));
		msg_print(_("主の熱烈な激励が書かれている。力がみなぎる！",
                    "Your master sends you words of encouragment. You feel full of power!"));
		reward = _("全能力値が上がった。", "raising all your stats.");
		for (dummy = 0; dummy < 6; dummy++)
		{
			(void)do_inc_stat(dummy);
		}
	}
	else if(type < 14)
	{
		bool flag = FALSE;
		int mcount = 0;
		int mcount2 = 0;
		msg_print(_("封書を開けた。", "You open the envelope."));

		for(i=0;i<6;i++) if(p_ptr->stat_cur[i] < p_ptr->stat_max[i] - 2)flag = TRUE;
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!m_ptr->ml) continue;
			if(projectable(py,px,m_ptr->fy,m_ptr->fx)) mcount++;
			if(m_ptr->cdis < MAX_SIGHT) mcount2++;
		}

		if(p_ptr->chp < p_ptr->mhp / 2 || p_ptr->lev < p_ptr->max_plv || flag)
		{
			msg_print(_("ピンチのあなたを主が励ましてくれた！",
                        "Your master encourages you in your current situation!"));
			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			hp_player(5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(dummy);
			}
			reward = _("回復した。", "healing.");
		}
		else if(mcount > 4)
		{
			msg_print(_("主の大音声が響き渡った！", "Your master's voice booms out loudly!"));
			(void)dispel_monsters(p_ptr->lev * 6);
			(void)turn_monsters(p_ptr->lev * 6);

		}
		else if(mcount2 > 9)
		{
			msg_print(_("主の大音声が響き渡った！", "Your master's voice booms out loudly!"));
			(void)mass_genocide(0, FALSE);
		}
		else if(p_ptr->lev < 50)
		{
			msg_print(_("主からの温かな心遣いを受け取った。",
                        "You receive warm praise from your master."));
			if (p_ptr->prace == RACE_ANDROID)
			{
				acquirement(py, px, 1+randint1(2), TRUE, FALSE);
				reward = _("高級なアイテムを与えられた。", "valuable item.");
			}
			else
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
				reward = _("経験値を得た。", "experience.");
			}
		}
		else
		{
			msg_print(_("主からのお祝いを受け取った。",
                        "You receive congratulations from your master."));
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT));
			(void)drop_near(q_ptr, -1, py, px);
				reward = _("アーティファクト生成の巻物を受け取った。",
                            "a scroll of Artifact Creation.");
		}
	}
	else if(type < 23)
	{
		msg_print(_("封書を開けた。", "You open the envelope."));
		msg_print(_("主から褒賞の言葉を受け取った。さらなる力が湧いた。",
                    "Your master has rewarded you. You feel more powerful."));
		reward = _("能力値が上がった。", "raising a stat.");
			(void)do_inc_stat(randint0(6));
	}
	else if(type < 31)
	{
		msg_print(_("臨時ボーナスが出た！", "You got a bonus!"));
		p_ptr->au += p_ptr->lev * p_ptr->lev * 100;
		reward = _("ボーナスを受け取った。", "bonus salary.");
	}
	else if(type < 36)
	{
		msg_print("主はあなたの奉仕に深く満足している。褒美の品が送られてきた。");
		reward = _("高級なアイテムを与えられた。", "valuable item.");
		if(p_ptr->lev > 24 || one_in_(3)) acquirement(py, px, 1+randint1(2), TRUE, FALSE);
		else acquirement(py, px, 1, TRUE, FALSE);
	}
	else if(type < 43)
	{
		msg_print(_("新しい制服が送られてきた！", "You have received a new uniform!"));
		reward = _("制服を支給された。", "providing a uniform.");
		if(p_ptr->psex == SEX_MALE)	object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_SUIT));
		else						object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_MAID));
		if(p_ptr->lev > 24 || one_in_(3)) apply_magic(q_ptr, object_level, AM_GOOD | AM_GREAT);
		else  apply_magic(q_ptr, object_level, AM_GOOD);
		(void)drop_near(q_ptr, -1, py, px);
	}
	else if(type < 48)
	{
		msg_print(_("新しい本が送られてきた！", "You were sent a new book!"));
		reward = _("魔法書を支給された。", "providing a spellbook.");
		//sval0:一冊目からsval3:四冊目まで
		object_prep(q_ptr, lookup_kind(p_ptr->realm1, p_ptr->lev / 14));
		(void)drop_near(q_ptr, -1, py, px);
	}
	else if(type < 55)
	{
		msg_print(_("封書を開けた。", "You open the envelope."));
		msg_print(_("主はあなたの働きに満足している。物資が同封されている。",
                    "Your master is satisfied with your work. There are supplies enclosed."));
		reward = _("物資を支給された。", "providing supplies.");
		//SVAL11-18の箱
		object_prep(q_ptr, lookup_kind(TV_CHEST, randint1(8)+10));
		q_ptr->pval = -6; //非施錠
		(void)drop_near(q_ptr, -1, py, px);
	}

	else if(type < 62)
	{
		bool anger=FALSE;

		msg_print(_("封書を開けた。", "You open the envelope."));
		for(i=INVEN_OUTER;i<=INVEN_FEET;i++)
		{
			u32b flgs[TR_FLAG_SIZE];
			int anger_rate = 0;
			object_type *o_ptr;

			if(inventory[i].k_idx==0) continue;
			/*:::天狗の頭襟は破壊されない*/
			if(i==INVEN_HEAD && (p_ptr->prace == RACE_KARASU_TENGU ||  p_ptr->prace == RACE_HAKUROU_TENGU )) continue;
			o_ptr= &inventory[i];
			object_flags(o_ptr, flgs);

			if(i==INVEN_HEAD && o_ptr->weight > 50 || i==INVEN_HANDS && o_ptr->weight > 25 || i==INVEN_FEET && o_ptr->weight > 30 ) anger_rate += 30;
			if(object_is_nameless(o_ptr))anger_rate += 20;
			if(object_is_cursed(o_ptr)) anger_rate += 50;
			if(have_flag(flgs, TR_AGGRAVATE)) anger_rate += 70;
			if(object_is_artifact(o_ptr)) anger_rate /= 4;
			if(o_ptr->pval > 0 && have_flag(flgs, TR_CHR) || have_flag(flgs,TR_SUST_CHR)) anger_rate = 0;
			if(randint1(100) < anger_rate)
			{
				anger = TRUE;
				break;
			}
		}

		if(anger)
		{
			object_type *o_ptr = &inventory[i];
			char		o_name[MAX_NLEN];
			object_desc(o_name, o_ptr, OD_OMIT_PREFIX);
			msg_print(_("もっとエレガントな装いをしろと叱責された！",
                        "You were reprimanded for not dressing elegantly enough!"));
			msg_format(_("%sは粉々に砕けて消えた。", "%s shatters into pieces and vanishes."),o_name);
			reward = format(_("%sを破壊された。", "destroying %s."),o_name);
			inven_item_increase(i, -1);
			inven_item_describe(i);
			inven_item_optimize(i);
			calc_android_exp();

		}
		else
		{
			if(one_in_(36))     object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_SPEED));
			else if(one_in_(36))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_ATTACKS));
			else if(one_in_(36))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_LORDLY));
			else if(one_in_(24))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_RES_HOLY));
			else if(one_in_(16))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_TELEPATHY));
			else if(one_in_(8))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_THE_MAGI));
			else if(one_in_(7))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RESIST_POIS));
			else if(one_in_(6))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_NETHER));
			else if(one_in_(5))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_DISENCHANT));
			else if(one_in_(4))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_CHAOS));
			else if(one_in_(3))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_MUSCLE));
			else if(one_in_(2))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_CHARISMA));
			else object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_RESISTANCE));
			apply_magic(q_ptr,p_ptr->lev*2,AM_NO_FIXED_ART | AM_GOOD);
			(void)drop_near(q_ptr, -1, py, px);
			msg_print(_("日頃の献身の褒美に高価な装飾品を賜った！", "You have received a valuable ornament for your devotion!"));
			reward = format(_("高価な装飾品を受け取った。", "a valuable ornament."));
		}
	}
	else if(type < 67)
	{
		msg_print(_("封書から紫の煙が噴き出しあなたを包んだ・・", "Purple smoke comes out of the envelope and surrounds you..."));
		reward = _("突然変異させられた。", "mutation.");
		gain_random_mutation(0);

	}
	else if(type < 70)
	{
		msg_print(_("主が誰かに宛てたらしい恋文が誤配されてきた。",
                    "Looks like it's a misdelivering love letter from your master addressed to someone else."));
		msg_print(_("あなたは忠誠心を発揮して見なかったことにした。",
                    "You show your loyalty by pretending not to see it."));
	}
	else if(type < 73)
	{
		msg_print(_("酔っ払っているとしか思えない筆跡で前衛的なポエムが書かれている。",
                    "It's some avant-gard poem in clearly drunk handwriting."));
		msg_print(_("あなたは忠誠心を発揮して見なかったことにした。",
                    "You show your loyalty by pretending not to see it."));
	}
	else if(type < 76)
	{
		int num;
		msg_print(_("封書を開けたら水が噴き出した！", "As you open the envelope, water pours out!"));
		fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);
		num = 1 + dun_level/20;
		for (i = 0; i < num; i++) (void)summon_specific(0, py, px, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
	}
	else if(type < 81)
	{
		msg_print(_("封書を開けたら大爆発した！", "As you open the envelope, it explodes!"));
		fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
		take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
		reward = _("仕置を受けた。", "dealing punishment.");
	}
	else if(type < 87)
	{
		msg_print(_("封書を開けた。", "You open the envelope."));
		msg_print(_("覚えのない調度品損壊の叱責を受けた。給料から差っ引かれた・・",
                    "You were reprimanded for damaging furniture (though you don't remember that). It was deduced from your salary..."));
		p_ptr->au = p_ptr->au / 10 * (5 + randint0(5));
		reward = _("所持金を減らされた。", "confiscating money.");
	}
	else if(type < 92)
	{
		msg_print(_("そしてそのままあなたの額に刺さった。", "It gets embedded in your forehead."));
		reward = _("仕置を受けた。", "dealing punishment.");
		take_hit(DAMAGE_NOESCAPE, damroll(7,7), wrath_reason, -1);
		set_stun(105);
	}
	else if(type < 99)
	{
		int count = 0;
		char		o_name[MAX_NLEN];

		msg_print(_("封書を開けた。", "You open the envelope."));
		msg_print(_("主は相当機嫌が悪いらしい・・", "Your master seems to be in a bad mood..."));
		switch (randint1(4))
		{
			case 1:
				(void)activate_ty_curse(FALSE, &count);
				reward = _("禍々しい呪いをかけられた。", "ancient and foul curse.");
				break;
			case 2:
				activate_hi_summon(py, px, FALSE);
				reward = _("モンスターを召喚された。", "summoning monsters.");
				break;
			case 3:
				if (one_in_(2))
				{
					if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM))
					{
						dummy = INVEN_LARM;
						if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
					}
					object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
					(void)curse_weapon(FALSE, dummy);
					reward = format(_("%sが破壊された。", "%s being destroyed."), o_name);
				}
				break;
			default:
				for (dummy = 0; dummy < 6; dummy++)
				{
					(void)dec_stat(dummy, 10 + randint1(15), TRUE);
				}
				reward = _("全能力値を下げられた。", "lowering all stats.");
				break;
		}
	}

	else
	{
		int num;
		int count = 0;

		msg_print(_("封書を開けた。", "You open the envelope."));
		screen_save();
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);
		Term_erase(12, 15, 255);
		put_str(_("死ぬがよい。", "Now die."), 17 , 40);
		(void)inkey();
		screen_load();
		take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
		for (dummy = 0; dummy < 6; dummy++)
		{
			if(one_in_(3)) (void)dec_stat(dummy, 10 + randint1(15), TRUE);
			else (void)dec_stat(dummy, 10 + randint1(15), FALSE);
		}
		activate_hi_summon(py, px, FALSE);
		(void)activate_ty_curse(FALSE, &count);
		if (one_in_(2))
		{
			dummy = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				dummy = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) dummy = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) dummy = INVEN_LARM;

			if (dummy) (void)curse_weapon(FALSE, dummy);
		}
		if (one_in_(2)) (void)curse_armor();
		reward = _("クビにされかけた。", "not getting fired.");
	}

	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("主から%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("Your master rewards you with %s.", reward));
#endif
	}
}

/*
 * XAngband: determine if a given location is "interesting"
 * based on target_set_accept function.
 */
static bool tgt_pt_accept(int y, int x)
{
	cave_type *c_ptr;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);

	/* Player grid is always interesting */
	if ((y == py) && (x == px)) return (TRUE);

	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);

	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice stairs */
		if (cave_have_flag_grid(c_ptr, FF_LESS))
			return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_MORE))
		{
			//v1.1.70 mimicフラグつきの下り階段(未発見の落とし戸)はターゲットの対象にしない
			if (c_ptr->mimic)
				return FALSE;
			else
				return (TRUE);
		}

		/* Notice quest features */
		if (cave_have_flag_grid(c_ptr, FF_QUEST_ENTER)) return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_QUEST_EXIT)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * XAngband: Prepare the "temp" array for "tget_pt"
 * based on target_set_prepare funciton.
 */
static void tgt_pt_prepare(void)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	if (!expand_list) return;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt; y++)
	{
		for (x = 1; x < cur_wid; x++)
		{
			/* Require "interesting" contents */
			if (!tgt_pt_accept(y, x)) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Target the nearest monster for shooting */
	ang_sort_comp = ang_sort_comp_distance;
	ang_sort_swap = ang_sort_swap_distance;

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);
}

/*
 * old -- from PsiAngband.
 */
/*:::場所を選んでスペースキーで決定　詳細未読*/
bool tgt_pt(int *x_ptr, int *y_ptr)
{
	char ch = 0;
	int d, x, y, n;
	bool success = FALSE;

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	x = px;
	y = py;

	//v1.1.66 特定条件のグリッドをカウントするtemp_n,temp_x[],temp_y[]をリセットしておく
	temp_n = 0;

	if (expand_list)
	{
		tgt_pt_prepare();
		n = 0;
	}

#ifdef JP
	msg_print("場所を選んでスペースキーを押して下さい。");
#else
	msg_print("Select a point and press space.");
#endif
	msg_flag = FALSE; /* prevents "-more-" message. */

	while ((ch != ESCAPE) && !success)
	{
		bool move_fast = FALSE;

		move_cursor_relative(y, x);
		ch = inkey();
//		msg_format("(%c:", ch);//chk
		switch (ch)
		{
		case ESCAPE:
			break;
		case ' ':
		case 't':
		case '.':
		case '5':
		case '0':
			/* illegal place */
			if (player_bold(y, x)) ch = 0;

			/* okay place */
			else success = TRUE;

			break;

		/* XAngband: Move cursor to stairs */
		case '>':
		case '<':
			if (expand_list && temp_n)
			{
				int dx, dy;
				int cx = (panel_col_min + panel_col_max) / 2;
				int cy = (panel_row_min + panel_row_max) / 2;

				n++;

				while(n < temp_n)	/* Skip stairs which have defferent distance */
				{
					cave_type *c_ptr = &cave[temp_y[n]][temp_x[n]];

					if (ch == '>')
					{
						if (cave_have_flag_grid(c_ptr, FF_LESS) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_ENTER))
						{
							n++;
							//koko
						}
						else
							break;
					}
					else /* if (ch == '<') */
					{
						if (cave_have_flag_grid(c_ptr, FF_MORE) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_EXIT))
							n++;
						else
							break;
					}
				}

				if (n == temp_n)	/* Loop out taget list */
				{
					n = 0;
					y = py;
					x = px;
					verify_panel();	/* Move cursor to player */

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();
				}
				else	/* move cursor to next stair and change panel */
				{
					y = temp_y[n];
					x = temp_x[n];

					dy = 2 * (y - cy) / hgt;
					dx = 2 * (x - cx) / wid;
					if (dy || dx) change_panel(dy, dx);
				}
			}
			break;

		default:
			/* Look up the direction */
			d = get_keymap_dir(ch);

			/* XTRA HACK MOVEFAST */
			if (isupper(ch)) move_fast = TRUE;
//	msg_format("%d)", d);//chk
			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					/* if (change_panel(dy, dx)) target_set_prepare(mode); */
					change_panel(dy, dx);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;

			}
			break;
		}
	}

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	//v1.1.66 特定条件のグリッドをカウントするtemp_n,temp_x[],temp_y[]をリセットしておく
	temp_n = 0;


	*x_ptr = x;
	*y_ptr = y;
	return success;
}

/*:::方向を決定する。キャンセル不可？*/
bool get_hack_dir(int *dp)
{
	int		dir;
	cptr    p;
	char    command;


	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = 0;

	/* (No auto-targeting) */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "方向 ('*'でターゲット選択, ESCで中断)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "方向 ('5'でターゲットへ, '*'でターゲット再選択, ESCで中断)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Look up the direction */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("あなたは混乱している。");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}


/*
 * エネルギーの増加量10d5を速く計算するための関数
 //使われてないので消す

#define Go_no_JuuJou 5*5*5*5*5*5*5*5*5*5

s16b gain_energy(void)
{
	int i;
	s32b energy_result = 10;
	s32b tmp;

	tmp = randint0(Go_no_JuuJou);

	for (i = 0; i < 9; i ++){
		energy_result += tmp % 5;
		tmp /= 5;
	}

	return (s16b)(energy_result + tmp);
}
*/


/*
 * Return bow energy
 */
/*:::弓の基本射撃速度を計算　なまけ弓はソースレベルで早い*/
///item sys svalによる射撃速度設定 tvalも参照するように変えないといけない
///mod131229 弓の基本射撃速度 無理矢理固定
s16b bow_energy(int tval, int sval)
{
	int energy = 100;

	if(tval == TV_BOW && sval == SV_FIRE_SHORT_BOW)		energy = 10000;
	else if(tval == TV_BOW && sval == SV_FIRE_LONG_BOW)	energy = 10000;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_LIGHT_CROSSBOW)	energy = 12000;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_HEAVY_CROSSBOW)	energy = 13333;

	else if(tval == TV_GUN)	energy = 10000;

	else msg_print(_("ERROR:設定されていないtvalとsvalでbow_energy()が呼ばれた",
                    "ERROR: bow_energy() called without set tval and sval"));

	return (energy);
}


/*
 * Return bow tmul
 */
/*:::弓の基本倍率*/
///item sys 弓の基本倍率　弓とクロスボウでTVALを分けるのでここ変更する必要あり
///mod131229 弓の基本倍率
int bow_tmul(int tval, int sval)
{
	int tmul = 0;
	/* Analyze the launcher */

	if(tval == TV_BOW && sval == SV_FIRE_SHORT_BOW)		tmul = 2;
	else if(tval == TV_BOW && sval == SV_FIRE_LONG_BOW)	tmul = 3;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_LIGHT_CROSSBOW)	tmul = 3;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_HEAVY_CROSSBOW)	tmul = 4;
//	else if(tval == TV_GUN && sval == SV_FIRE_GUN_CRIMSON)	tmul = 0;
//	else if(tval == TV_GUN && sval == SV_FIRE_GUN_MUSKET)	tmul = 0;
	else msg_print(_("ERROR:設定されていないtvalとsvalでbow_tmul()が呼ばれた",
                    "ERROR: bow_tmul() called without set tval and sval"));

	return (tmul);
}

/*
 * Return alignment title
 */
cptr your_alignment(void)
{
#ifdef JP
	if (p_ptr->align > 150) return "大善";
	else if (p_ptr->align > 50) return "中善";
	else if (p_ptr->align > 10) return "小善";
	else if (p_ptr->align > -11) return "中立";
	else if (p_ptr->align > -51) return "小悪";
	else if (p_ptr->align > -151) return "中悪";
	else return "大悪";
#else
	if (p_ptr->align > 150) return "Lawful";
	else if (p_ptr->align > 50) return "Good";
	else if (p_ptr->align > 10) return "Neutral Good";
	else if (p_ptr->align > -11) return "Neutral";
	else if (p_ptr->align > -51) return "Neutral Evil";
	else if (p_ptr->align > -151) return "Evil";
	else return "Chaotic";
#endif
}


/*
 * Return proficiency level of weapons and misc. skills (except riding)
 */
///skill 武器熟練度関係
int weapon_exp_level(int weapon_exp)
{
	if (weapon_exp < WEAPON_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (weapon_exp < WEAPON_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (weapon_exp < WEAPON_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (weapon_exp < WEAPON_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of riding
 */
int riding_exp_level(int riding_exp)
{
	if (riding_exp < RIDING_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (riding_exp < RIDING_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (riding_exp < RIDING_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (riding_exp < RIDING_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of spells
 */
/*:::魔法の技能値を熟練レベル表記用の値に直しているらしい*/
int spell_exp_level(int spell_exp)
{
	if (spell_exp < SPELL_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (spell_exp < SPELL_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (spell_exp < SPELL_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (spell_exp < SPELL_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Display a rumor and apply its effects
 */

int rumor_num(char *zz, int max_idx)
{
	if (strcmp(zz, "*") == 0) return randint1(max_idx - 1);
	return atoi(zz);
}

cptr rumor_bind_name(char *base, cptr fullname)
{
	char *s, *v;

	s = strstr(base, "{Name}");
	if (s)
	{
		s[0] = '\0';
		v = format("%s%s%s", base, fullname, (s + 6));
	}
	else
	{
		v = base;
	}

	return v;
}

/*:::噂の巻物、宿の「噂を聞く」コマンド*/
void display_rumor(bool ex)
{
	bool err;
	int section = 0;
	char Rumor[1024];

	///sys131216 噂コマンドで町の情報を聞けなくしたので確率やや下げる
	if (ex)
	{
		//if (randint0(3) == 0) section = 1;
		if (randint0(7) == 0) section = 1;

	}

#ifdef JP
	err = get_rnd_line_jonly("rumors_j.txt", section, Rumor, 10);
	if (err) strcpy(Rumor, "嘘の噂もある。");
#else
	err = get_rnd_line("rumors.txt", section, Rumor);
	if (err) strcpy(Rumor, "Some rumors are wrong.");
#endif

	err = TRUE;

	/*:::rumors_j.txtのN:1のエントリ(section 1)に対応　★や町などの情報を得る*/
	///sys 古い街についての記述は削除予定
	if (strncmp(Rumor, "R:", 2) == 0)
	{
		char *zz[4];
		cptr rumor_msg = NULL;
		cptr rumor_eff_format = NULL;
		char fullname[1024] = "";

		if (tokenize(Rumor + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			if (strcmp(zz[0], "ARTIFACT") == 0)
			{
				int a_idx, k_idx;
				object_type forge;
				object_type *q_ptr = &forge;
				artifact_type *a_ptr;

				while (1)
				{
					a_idx = rumor_num(zz[1], max_a_idx);

					a_ptr = &a_info[a_idx];
					if (a_ptr->name) break;
				}

				k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
				object_prep(q_ptr, k_idx);
				q_ptr->name1 = a_idx;
				q_ptr->ident = IDENT_STORE;
				object_desc(fullname, q_ptr, OD_NAME_ONLY);
			}
			else if  (strcmp(zz[0], "MONSTER") == 0)
			{
				int r_idx;
				monster_race *r_ptr;

				while(1)
				{
					r_idx = rumor_num(zz[1], max_r_idx);
					r_ptr = &r_info[r_idx];
					if (r_ptr->name) break;
				}

				strcpy(fullname, r_name + r_ptr->name);

				/* Remember this monster */
				if (!r_ptr->r_sights)
				{
					r_ptr->r_sights++;
				}
			}
			///del131216 ダンジョンと町のうわさを聞けなくした
#if 0
			else if  (strcmp(zz[0], "DUNGEON") == 0)
			{
				int d_idx;
				dungeon_info_type *d_ptr;

				while (1)
				{
					d_idx = rumor_num(zz[1], max_d_idx);
					d_ptr = &d_info[d_idx];
					if (d_ptr->name) break;
				}

				strcpy(fullname, d_name + d_ptr->name);

				if (!max_dlv[d_idx])
				{
					max_dlv[d_idx] = d_ptr->mindepth;
					rumor_eff_format = _("%sに帰還できるようになった。", "You can recall to %s.");
				}
			}
			else if  (strcmp(zz[0], "TOWN") == 0)
			{
				int t_idx;
				s32b visit;

				while(1)
				{
					t_idx = rumor_num(zz[1], NO_TOWN);
					if (town[t_idx].name) break;
				}

				strcpy(fullname, town[t_idx].name);

				visit = (1L << (t_idx - 1));
				if ((t_idx != SECRET_TOWN) && !(p_ptr->visit & visit))
				{
					p_ptr->visit |= visit;
					rumor_eff_format = _("%sに行ったことがある気がする。", "You feel you have been to %s.");
				}
			}
#endif
			rumor_msg = rumor_bind_name(zz[2], fullname);
			msg_print(rumor_msg);
			if (rumor_eff_format)
			{
				msg_print(NULL);
				msg_format(rumor_eff_format, fullname);
			}
			err = FALSE;
		}
	/* error */
#ifdef JP
		if (err) msg_print("この情報は間違っている。");
#else
		if (err) msg_print("This information is wrong.");
#endif
	}
			else
	{
		msg_format("%s", Rumor);
	}
}



//永琳の主配置
void	place_master_kaguya(void)
{
	int tx,ty;
	int dir;

	if(p_ptr->pclass != CLASS_EIRIN) return;
	if(p_ptr->wild_mode) return;
	if(p_ptr->inside_battle) return;
	if(p_ptr->inside_arena) return;

	//永琳の隣に配置しそこねた場合、フロアの適当なところにもう一度配置を試みる。それでも無理なら出ないまま。
	if(!summon_named_creature(0,py,px,MON_MASTER_KAGUYA,PM_FORCE_FRIENDLY))
	{
		int i;
		for(i=100;i>0;i--)
		{
			tx = randint1(cur_wid);
			ty = randint1(cur_hgt);

			if(cave_empty_bold(ty,tx))
			{
				summon_named_creature(0,ty,tx,MON_MASTER_KAGUYA,PM_FORCE_FRIENDLY);
				break;
			}
		}
		msg_print(_("・・？輝夜とはぐれてしまったようだ。", "...? Looks like Kaguya has snuck away."));

	}

}

//v1.1.53 クラウンピースは火を見ると楽しくなるらしい
void clownpiece_likes_fire(int typ)
{
	if (p_ptr->pclass != CLASS_CLOWNPIECE && p_ptr->pclass != CLASS_VFS_CLOWNPIECE) return;

	if (typ != GF_FIRE && typ != GF_NUKE) return;

	if (p_ptr->hero || p_ptr->is_dead) return;

#ifdef JP
	if (one_in_(3))
		msg_print("あなたは火を見て気分が高揚した！");
	else if (one_in_(2))
		msg_print("「燃えてきたぜー！」");
	else
		msg_print("「この炎...ロックだぜ！」");
#else
    msg_print("Your spirits soar as you look at the flame!");
#endif

	set_hero(5 + randint1(5), FALSE);

}
