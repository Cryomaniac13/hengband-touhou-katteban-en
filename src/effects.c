/* File: effects.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */
/*:::探索・構え・型などの行動をセットしたり外す*/
#include "angband.h"

void set_action(int typ)
{
	int prev_typ = p_ptr->action;

	if (typ == prev_typ)
	{
		return;
	}
	else
	{
		switch (prev_typ)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				msg_print("探索をやめた。");
#else
				msg_print("You stop searching.");
#endif
				p_ptr->redraw |= (PR_SPEED);
				break;
			}
			case ACTION_REST:
			{
				resting = 0;
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				msg_print("学習をやめた。");
#else
				msg_print("You stop Learning");
#endif
				new_mane = FALSE;
				break;
			}
			case ACTION_KAMAE:
			{
#ifdef JP
				msg_print("構えをといた。");
#else
				msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KAMAE_MASK);
				break;
			}
			case ACTION_KATA:
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_ICHIRIN)
					msg_print("構えを解いた。");
				else if (p_ptr->pclass == CLASS_YUYUKO)
					msg_print("あなたの纏う雰囲気が普段通りに戻った。");
				else
					msg_print("型を崩した。");
#else
				if(p_ptr->pclass == CLASS_ICHIRIN)
					msg_print("You stop assuming the posture.");
				else if (p_ptr->pclass == CLASS_YUYUKO)
					msg_print("Atmosphere around you returns to normal.");
				else
					msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATUS);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP

				if(p_ptr->pclass == CLASS_KYOUKO)
					msg_print("叫ぶのをやめた。");
				else if (p_ptr->pclass == CLASS_SANNYO)
					msg_print("煙草を消した。");
				else if(CLASS_IS_PRISM_SISTERS)
					msg_print("音を止めた。");
				else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI || p_ptr->pclass == CLASS_RAIKO)
					msg_print("演奏を止めた。");
				else
					msg_print("歌うのをやめた。");
#else
				if(p_ptr->pclass == CLASS_KYOUKO)
					msg_print("You stop shouting.");
				else if (p_ptr->pclass == CLASS_SANNYO)
					msg_print("You extinguish your pipe.");
				else if(CLASS_IS_PRISM_SISTERS)
					msg_print("You stop making noise.");
				else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI || p_ptr->pclass == CLASS_RAIKO)
					msg_print("You stop performing.");
				else
					msg_print("You stop singing.");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				msg_print("あなたは轟音を立てて停止した！");
#else
				msg_print("You come to a halt!");
#endif
				energy_use = 100;
				break;
			}
			case ACTION_SPELL:
			{
#ifdef JP
				msg_print("呪文の詠唱を中断した。");
#else
				msg_print("You stopped spelling all spells.");
#endif
				break;
			}

		}
	}

	p_ptr->action = typ;

	/* If we are requested other action, stop singing */
	if (prev_typ == ACTION_SING)
	{
		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	switch (p_ptr->action)
	{
		case ACTION_SEARCH:
		{
#ifdef JP
			msg_print("注意深く歩き始めた。");
#else
			msg_print("You begin to walk carefully.");
#endif
			p_ptr->redraw |= (PR_SPEED);
			break;
		}
		case ACTION_LEARN:
		{
#ifdef JP
			msg_print("学習を始めた。");
#else
			msg_print("You begin Learning");
#endif
			break;
		}
		case ACTION_FISH:
		{
#ifdef JP
			msg_print("水面に糸を垂らした．．．");
#else
			msg_print("You begin fishing...");
#endif
			break;
		}
		case ACTION_HAYAGAKE:
		{
#ifdef JP
			msg_print("あなたは猛然と駆け出した！");
#else
			msg_print("You start running at an intense speed!");
#endif
			break;
		}
		default:
		{
			break;
		}
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

}

/* reset timed flags */
/*:::一時ステータスなどを全て解除する*/
///sys 一時ステータス解除、バルログ火炎二重耐性など特殊なのは保持
void reset_tim_flags(void)
{
	int i;
	p_ptr->fast = 0;            /* Timed -- Fast */
	p_ptr->lightspeed = 0;
	p_ptr->slow = 0;            /* Timed -- Slow */
	p_ptr->blind = 0;           /* Timed -- Blindness */

	//hack - ルーミア闇生成中は盲目保持
	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0]) p_ptr->blind = 1;

	rokuro_head_search_item(0, TRUE);

	p_ptr->tim_hirari_nuno = 0;

	p_ptr->paralyzed = 0;       /* Timed -- Paralysis */
	p_ptr->confused = 0;        /* Timed -- Confusion */
	p_ptr->afraid = 0;          /* Timed -- Fear */
	p_ptr->image = 0;           /* Timed -- Hallucination */
	p_ptr->poisoned = 0;        /* Timed -- Poisoned */
	p_ptr->cut = 0;             /* Timed -- Cut */
	p_ptr->stun = 0;            /* Timed -- Stun */

	p_ptr->alcohol = 0;

	p_ptr->protevil = 0;        /* Timed -- Protection */
	p_ptr->invuln = 0;          /* Timed -- Invulnerable */
	p_ptr->ult_res = 0;
	p_ptr->hero = 0;            /* Timed -- Heroism */
	p_ptr->shero = 0;           /* Timed -- Super Heroism */
	p_ptr->shield = 0;          /* Timed -- Shield Spell */
	p_ptr->blessed = 0;         /* Timed -- Blessed */
	p_ptr->tim_invis = 0;       /* Timed -- Invisibility */
	p_ptr->tim_infra = 0;       /* Timed -- Infra Vision */
	p_ptr->tim_regen = 0;       /* Timed -- Regeneration */
	p_ptr->tim_stealth = 0;     /* Timed -- Stealth */
	p_ptr->tim_esp = 0;
	p_ptr->wraith_form = 0;     /* Timed -- Wraith Form */
	p_ptr->tim_levitation = 0;
	p_ptr->tim_sh_touki = 0;
	p_ptr->tim_sh_fire = 0;
	p_ptr->tim_sh_holy = 0;
	p_ptr->tim_eyeeye = 0;
	p_ptr->magicdef = 0;
	p_ptr->resist_magic = 0;
	p_ptr->tsuyoshi = 0;
	p_ptr->kabenuke = 0;
	p_ptr->tim_res_nether = 0;
	p_ptr->tim_res_water = 0;
	p_ptr->tim_res_chaos = 0;
	p_ptr->tim_superstealth = 0;
	p_ptr->radar_sense = 0;
	p_ptr->clawextend = 0;
	p_ptr->tim_speedster = 0;
	p_ptr->tim_res_insanity = 0;

	for(i=0;i<TIM_GENERAL_MAX;i++)	p_ptr->tim_general[i] = 0;
	for(i=0;i<6;i++)	p_ptr->tim_addstat_count[i] = 0;
	p_ptr->reactive_heal = 0;

	p_ptr->lucky = 0;
	p_ptr->foresight = 0;
	p_ptr->deportation = 0;

	p_ptr->tim_res_time = 0;
	p_ptr->tim_mimic = 0;
	p_ptr->mimic_form = 0;
	p_ptr->tim_reflect = 0;
	p_ptr->tim_sh_death = 0;
	p_ptr->multishadow = 0;
	p_ptr->dustrobe = 0;
	p_ptr->action = ACTION_NONE;
	p_ptr->nennbaku = 0;
	p_ptr->tim_res_dark = 0;
	p_ptr->tim_unite_darkness = 0;

	p_ptr->tim_spellcard_count = 0;
	p_ptr->spellcard_effect_idx = 0;

	p_ptr->oppose_acid = 0;     /* Timed -- oppose acid */
	p_ptr->oppose_elec = 0;     /* Timed -- oppose lightning */
	p_ptr->oppose_fire = 0;     /* Timed -- oppose heat */
	p_ptr->oppose_cold = 0;     /* Timed -- oppose cold */
	p_ptr->oppose_pois = 0;     /* Timed -- oppose poison */

	p_ptr->word_recall = 0;
	p_ptr->alter_reality = 0;
	p_ptr->sutemi = FALSE;
	p_ptr->counter = FALSE;
	p_ptr->ele_attack = 0;
	p_ptr->ele_immune = 0;
	p_ptr->special_attack = 0L;
	p_ptr->special_defense = 0L;
	//v1.1.13
	p_ptr->special_flags = 0L;

	p_ptr->tim_aggravation = 0L;

	p_ptr->tim_no_move = 0L;
	p_ptr->transportation_trap = 0L;

	while(p_ptr->energy_need < 0) p_ptr->energy_need += ENERGY_NEED();
	world_player = FALSE;
	///race clalss 一時効果解除でも一部の職や種族は一部効果が継続する
	//if (prace_is_(RACE_DEMON) && (p_ptr->lev > 44)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_NINJA) && (p_ptr->lev > 44)) p_ptr->oppose_pois = 1;

	if ((p_ptr->pclass == CLASS_IKU ||p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)) p_ptr->oppose_elec = 1;
	if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) p_ptr->oppose_cold = 1;
	if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) p_ptr->oppose_cold = 1;
	if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_CHIYARI) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_ENOKO) && (p_ptr->lev > 39)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;

	if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MOMOYO) && (p_ptr->lev > 19)) p_ptr->oppose_pois = 1;

	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;

	if ((p_ptr->pclass == CLASS_YUMA) && (p_ptr->lev > 19)) p_ptr->oppose_pois = 1;

	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) p_ptr->shero = 1;

	if (p_ptr->riding)
	{
		(void)set_monster_fast(p_ptr->riding, 0);
		(void)set_monster_slow(p_ptr->riding, 0);
		(void)set_monster_invulner(p_ptr->riding, 0, FALSE);
	}

//	if (p_ptr->pclass == CLASS_BARD)
	///mod140817 歌関係に吟遊詩人以外の職も便乗
	if(CHECK_MUSIC_CLASS)
	{
		p_ptr->magic_num1[0] = 0;
		p_ptr->magic_num2[0] = 0;
	}
}

/*:::魔力消去を受けたとき*/
///system 何か魔法や特殊効果を追加したらここに解除用を入れないといけない
void dispel_player(void)
{
	int i;

	(void)set_fast(0, TRUE);
	(void)set_lightspeed(0, TRUE);
	(void)set_slow(0, TRUE);
	(void)set_shield(0, TRUE);
	(void)set_blessed(0, TRUE);
	(void)set_tsuyoshi(0, TRUE);
	(void)set_hero(0, TRUE);
	(void)set_shero(0, TRUE);
	(void)set_protevil(0, TRUE);
	(void)set_invuln(0, TRUE);
	(void)set_wraith_form(0, TRUE);
	(void)set_kabenuke(0, TRUE);
	(void)set_tim_res_nether(0, TRUE);
	(void)set_tim_res_time(0, TRUE);
	/* by henkma */
	(void)set_tim_reflect(0,TRUE);
	(void)set_multishadow(0,TRUE);
	(void)set_dustrobe(0,TRUE);
	for(i=0;i<6;i++)(void)set_tim_addstat(i,0,0,TRUE);

	(void)set_reactive_heal(0, TRUE);
	(void)set_clawextend(0, TRUE);

	(void)set_tim_invis(0, TRUE);
	(void)set_tim_infra(0, TRUE);
	(void)set_tim_esp(0, TRUE);
	(void)set_tim_regen(0, TRUE);
	(void)set_tim_stealth(0, TRUE);
	(void)set_tim_levitation(0, TRUE);
	(void)set_tim_sh_touki(0, TRUE);
	(void)set_tim_sh_fire(0, TRUE);
	(void)set_tim_sh_holy(0, TRUE);
	(void)set_tim_eyeeye(0, TRUE);
	(void)set_magicdef(0, TRUE);
	(void)set_resist_magic(0, TRUE);
	(void)set_oppose_acid(0, TRUE);
	(void)set_oppose_elec(0, TRUE);
	(void)set_oppose_fire(0, TRUE);
	(void)set_oppose_cold(0, TRUE);
	(void)set_oppose_pois(0, TRUE);
	(void)set_ultimate_res(0, TRUE);
	(void)set_mimic(0, 0, TRUE);
	(void)set_ele_attack(0, 0);
	(void)set_ele_immune(0, 0);
	(void)set_kamioroshi(0, 0);

	///mod140209 水一時耐性
	(void)set_tim_speedster(0, TRUE);
	(void)set_tim_res_insanity(0, TRUE);
	(void)set_tim_res_water(0, TRUE);
	(void)set_tim_res_chaos(0, TRUE);
	(void)set_tim_superstealth(0, TRUE,0);
	(void)set_tim_sh_death(0, TRUE);
	(void)set_radar_sense(0, TRUE);
	(void)set_tim_res_dark(0, TRUE);
	(void)set_tim_unite_darkness(0, TRUE);

	(void)set_tim_lucky(0, TRUE);
	(void)set_foresight(0, TRUE);
	(void)set_deportation(0, TRUE);

	//白蓮の身体強化は魔力消去を受ける
	if(p_ptr->pclass == CLASS_BYAKUREN) set_tim_general(0,TRUE,0,0);

	//ヘカーティアのアポロ反射鏡も
	if(p_ptr->pclass == CLASS_HECATIA) set_tim_general(0,TRUE,0,0);

	//サグメ神々の弾冠も
	if(p_ptr->pclass == CLASS_SAGUME) set_tim_general(0,TRUE,0,0);

	//v1.1.27 結界ガード
	set_mana_shield(FALSE,FALSE);

	//v1.1.56
	set_tim_spellcard_effect(0, TRUE, 0);

	//v1.1.69
	set_nennbaku(0, TRUE);
	//v1.1.93
	(void)set_tim_aggravation(0, TRUE);

	set_tim_transportation_trap(0, TRUE);

	//v1.1.17 純狐一時効果も
	//v1.1.52 菫子新性格追加
	if(USE_NAMELESS_ARTS)
	{
		for(i=0;i<TIM_GENERAL_MAX;i++) set_tim_general(0,TRUE,i,0);
	}

	//鈴瑚の団子強化とインフルーエンスも
	if(p_ptr->pclass == CLASS_RINGO)
	{
		set_tim_general(0,TRUE,0,0);
		set_tim_general(0,TRUE,1,0);
	}
	//成美特技「クリミナルサルヴェイション」も
	if(p_ptr->special_defense & (SD_HELLFIRE_BARRIER))
	{
		msg_print(_("あなたはもう地獄の業火から保護されていない。",
                    "You are no longer protected from hellfire."));
		p_ptr->special_defense &= ~(SD_HELLFIRE_BARRIER);
	}

	//v1.1.94 硝子の盾
	if (p_ptr->special_defense & SD_GLASS_SHIELD)
	{
		msg_print(_("硝子の盾が消えた。", "Your glass shield vanishes."));
		p_ptr->special_defense &= ~(SD_GLASS_SHIELD);

	}

	//v2.0.1 生命爆発の薬
	if (p_ptr->special_defense & SD_LIFE_EXPLODE)
	{
		msg_print(_("生命爆発の効果が消えた。", "Your life explosion effect expires."));
		p_ptr->special_defense &= ~(SD_LIFE_EXPLODE);

	}


	//うどんげ「イビルアンジュレーション」消去
	if (p_ptr->special_defense & EVIL_UNDULATION_MASK)
	{
		evil_undulation(FALSE, FALSE);
	}


	/* Cancel glowing hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
		p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
		msg_print("手の輝きがなくなった。");
#else
		msg_print("Your hands stop glowing.");
#endif
	}

	if (music_singing_any() || hex_spelling_any())
	{
		cptr str;
		if (music_singing_any())
		{
			if (p_ptr->pclass == CLASS_SANNYO)
				msg_print(_("煙草の味が濁った。", "The taste of your tobacco is ruined."));
			else
				msg_print(_("歌が途切れた。", "Your song was interrupted."));
		}
		else
		{
			msg_print(_("詠唱が途切れた。", "Your chanting was interrupted."));
		}

		p_ptr->magic_num1[1] = p_ptr->magic_num1[0];
		p_ptr->magic_num1[0] = 0;
		p_ptr->action = ACTION_NONE;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS | PU_HP);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		p_ptr->energy_need += ENERGY_NEED();
	}
}


/*
 * Set "p_ptr->tim_mimic", and "p_ptr->mimic_form",
 * notice observable changes
 */
/*:::変身 変身不可のときFALSE*/
bool set_mimic(int v, int p, bool do_dec)
{
	bool notice = FALSE;
	bool check_nue = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.85 新たに変身するとき、モンスター変身用のidxを記録してあったのをリセット
	if (!do_dec) p_ptr->metamor_r_idx = 0;


	if(p_ptr->riding && mimic_info[p].MIMIC_FLAGS & MIMIC_NO_RIDING && !do_dec)
	{
		if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
			msg_print(_("寄生中は変身できない。", "You cannot transform while parasitising."));
		else if (CLASS_RIDING_BACKDANCE)
			msg_print(_("今は踊るのに忙しく変身できない。", "You are busy dancing right now and cannot transform."));
		else
			msg_print(_("まず馬から降りてからでないとその変身はできない。", "You have to get off your mount first before transforming."));
		return FALSE;
	}
	if(p_ptr->prace == RACE_HOURAI && !do_dec)
	{
		msg_print(_("・・あなたの肉体は変容を受け付けなかった。", "...Transformation doesn't affect your body."));
		return FALSE;
	}

	// -Hack - ぬえは特殊フラグが立っている限り変身が解けない
	if(p_ptr->mimic_form == MIMIC_NUE && (p_ptr->special_defense & NUE_UNDEFINED) && !v)
	{
		p_ptr->tim_mimic = 1;
		return FALSE;
	}
	//上と同じくドレミー変身もフラグが立ってる限り解けない
	//v1.1.47 マミゾウ変身と区別するため条件式を特殊フラグに変更　紫苑も同じ処理で変身する
	if(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME) && !v)
	{
		p_ptr->tim_mimic = 1;
		return FALSE;
	}
	//マミゾウの変化は解ける。解ける前にフラグを落とす
	else if(IS_METAMORPHOSIS && !v)
	{
		p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
	}

	/* Open */
	if (v)
	{
		if (p_ptr->tim_mimic && (p_ptr->mimic_form == p) && !do_dec)
		{
			if (p_ptr->tim_mimic > v) return FALSE;
		}
		else if ((!p_ptr->tim_mimic) || (p_ptr->mimic_form != p))
		{
			set_deity_bias(DBIAS_COSMOS, -1);
#ifdef JP

			if( p == MIMIC_GIGANTIC)
				msg_print("あなたは山のような巨体になった！");
			else if( p == MIMIC_BEAST)
				msg_print("あなたは巨大な獣に変身した！");
			else if( p == MIMIC_SLIME)
				msg_print("あなたの体は溶け崩れて広がった！");
			else if( p == MIMIC_MARISA)
				msg_print("あなたは白黒の魔法使いに変身した！");
			else if( p == MIMIC_MIST)
				msg_print("あなたの体の輪郭がぼやけ始めた・・");
			else if( p == MIMIC_DRAGON)
				msg_print("あなたは巨大な竜へと変身した！");
			else if( p == MIMIC_CAT)
				msg_print("あなたは猫の姿になった。");
			else if( p == MIMIC_NUE)
				msg_print("あなたは正体不明の怪物に変身した！");
			else if( p == MIMIC_SHINMYOU)
				msg_print("伝説の一寸法師のように体が小さくなった！");
			else if( p == MIMIC_KOSUZU_GHOST)
				msg_print("恋文を読んでいると意識が遠のいていった...");
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("地面から黒い影が噴き出し、あなたに取り憑いた！");
			else if (p == MIMIC_GOD_OF_NEW_WORLD)
			{
				if(one_in_(2))
					msg_print("あなたは狂熱に身を任せて空を駆けた！");
				else
					msg_print("今なら新世界の神にでもなれそうな気分だ！");
			}
			//v1.1.47 紫苑　
			else if (p >= MIMIC_METAMORPHOSE_NORMAL && p <= MIMIC_METAMORPHOSE_GIGANTIC &&
				(p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME) && p_ptr->pclass == CLASS_SHION)
			{
				msg_print("あなたは完全憑依を完了させ主導権を奪った！");
			}
			else
				msg_print("自分の体が変わってゆくのを感じた。");
#else
			if( p == MIMIC_GIGANTIC)
				msg_print("You become as large as a mountain!");
			else if( p == MIMIC_BEAST)
				msg_print("You transform into a giant beast!");
			else if( p == MIMIC_SLIME)
				msg_print("Your body melts down and spreads!");
			else if( p == MIMIC_MARISA)
				msg_print("You transform into the black and white magician!");
			else if( p == MIMIC_MIST)
				msg_print("Your body starts becoming blurry.");
			else if( p == MIMIC_DRAGON)
				msg_print("You transform into a massive dragon!");
			else if( p == MIMIC_CAT)
				msg_print("You take on the form of a cat.");
			else if( p == MIMIC_NUE)
				msg_print("You transform into an undefined monster!");
			else if( p == MIMIC_SHINMYOU)
				msg_print("Your body becomes as small as the legendary Issun-Boushi!");
			else if( p == MIMIC_KOSUZU_GHOST)
				msg_print("Your consciousness fades away as you read the red letters.");
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("Black shadows rise from the ground and possess you!");
			else if (p == MIMIC_GOD_OF_NEW_WORLD)
			{
				if(one_in_(2))
					msg_print(_("あなたは狂熱に身を任せて空を駆けた！",
                                "You frenziedly dance in the sky!"));
				else
					msg_print(_("今なら新世界の神にでもなれそうな気分だ！",
                                "You feel like you can became the god of new world!"));
			}
			//v1.1.47 紫苑　
			else if (p >= MIMIC_METAMORPHOSE_NORMAL && p <= MIMIC_METAMORPHOSE_GIGANTIC &&
				(p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME) && p_ptr->pclass == CLASS_SHION)
			{
				msg_print(_("あなたは完全憑依を完了させ主導権を奪った！",
                            "You complete total possession and take control!"));
			}
			else
				msg_print("You feel that your body changes.");
#endif
			p_ptr->mimic_form=p;
			notice = TRUE;

			// Hack - special_defenceフラグを使う特殊な変身をする職が変身中に普通の変身をした場合フラグ消去
			if(p_ptr->pclass == CLASS_NUE && p_ptr->mimic_form != MIMIC_NUE) p_ptr->special_defense &= ~(NUE_UNDEFINED);
			if(IS_METAMORPHOSIS && (p_ptr->mimic_form < MIMIC_METAMORPHOSE_NORMAL || p_ptr->mimic_form > MIMIC_METAMORPHOSE_GIGANTIC))p_ptr->special_defense &= ~(SD_METAMORPHOSIS);

			//九十九姉妹が一部変身をした場合演奏解除
			if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI)
			{
				if( mimic_info[p].MIMIC_FLAGS & (MIMIC_ONLY_MELEE | MIMIC_NO_CAST)) stop_tsukumo_music();
			}


			//v1.1.68b スライムや霧に変化したら切り傷や破損が治ることにする
			if (p == MIMIC_MIST || p == MIMIC_SLIME)
			{
				if (RACE_BREAKABLE)
					set_broken(-(BROKEN_MAX));
				else
					set_cut(0);
			}

		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_mimic)
		{
#ifdef JP
			if (p_ptr->mimic_form == MIMIC_GIGANTIC)
				msg_print("体の大きさが元に戻った。");
			else if (p_ptr->mimic_form == MIMIC_MIST)
				msg_print("あなたの体の輪郭は再びはっきりした。");
			else if (p_ptr->mimic_form == MIMIC_CAT)
				msg_print("「じゃじゃーん！」");
			else if( p_ptr->mimic_form == MIMIC_NUE)
				msg_print("あなたの姿は元に戻った。");
			else if( p == MIMIC_KOSUZU_GHOST)
			{
				msg_print("あなたは正気に戻った。");
				//壁抜けや変わり身状態だったらそれも消える
				set_kabenuke(0,TRUE);
				p_ptr->special_defense &= ~(NINJA_KAWARIMI);

			}
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("鬼の影はあなたから離れた。");
			//v1.1.48 紫苑　この条件式だとエクストラモードで狸の葉っぱで変身したときもこのメッセージになってしまうが仕方ないか
			else if (p_ptr->mimic_form >= MIMIC_METAMORPHOSE_NORMAL && p_ptr->mimic_form <= MIMIC_METAMORPHOSE_GIGANTIC && p_ptr->pclass == CLASS_SHION)
			{
				msg_print("あなたは憑依を解いた。");
			}
			else
				msg_print("変身が解けた。");
#else
			if (p_ptr->mimic_form == MIMIC_GIGANTIC)
				msg_print("You revert to normal size.");
			else if (p_ptr->mimic_form == MIMIC_MIST)
				msg_print("Your bodt becomes less blurred.");
			else if (p_ptr->mimic_form == MIMIC_CAT)
				msg_print("'Tada~'");
			else if( p_ptr->mimic_form == MIMIC_NUE)
				msg_print("You return to your original form.");
			else if( p == MIMIC_KOSUZU_GHOST)
			{
				msg_print("You come back to your senses.");
				//壁抜けや変わり身状態だったらそれも消える
				set_kabenuke(0,TRUE);
				p_ptr->special_defense &= ~(NINJA_KAWARIMI);

			}
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("Shadows of the oni detach from you.");
			//v1.1.48 紫苑　この条件式だとエクストラモードで狸の葉っぱで変身したときもこのメッセージになってしまうが仕方ないか
			else if (p_ptr->mimic_form >= MIMIC_METAMORPHOSE_NORMAL && p_ptr->mimic_form <= MIMIC_METAMORPHOSE_GIGANTIC && p_ptr->pclass == CLASS_SHION)
			{
				msg_print("You end your possession.");
			}
			else
				msg_print("You are no longer transformed.");
#endif
			if(p_ptr->pclass == CLASS_NUE && p_ptr->mimic_form != MIMIC_NUE) check_nue = TRUE;
			if (p_ptr->mimic_form == MIMIC_DEMON) set_oppose_fire(0, TRUE);
			p_ptr->mimic_form=0;
			notice = TRUE;
			p = 0;
		}
	}


	/* Use the value */
	p_ptr->tim_mimic = v;

	/* Nothing to notice */
	if (!notice)
		return (FALSE);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw title */
	p_ptr->redraw |= (PR_BASIC | PR_STATUS | PR_MAP);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_TORCH);
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	p_ptr->window |= (PW_EQUIP | PW_SPELL | PW_PLAYER | PW_INVEN);

	handle_stuff();

	/*::: -Hack- ぬえが[正体不明]以外からの変身解除のとき、[正体不明]に変身可能かチェックして可能なら変身処理
	 *::: check_nue_undefined()から再びset_mimic()が呼ばれる*/
	//v2.0.5 ここの処理もやめる。ぬえの自動変身は行動開始時にのみ確認する
	if(check_nue) flag_nue_check_undefined = TRUE;
	//if(check_nue) check_nue_undefined();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->blind", notice observable changes
 *
 * Note the use of "PU_UN_LITE" and "PU_UN_VIEW", which is needed to
 * memorize any terrain features which suddenly become "visible".
 * Note that blindness is currently the only thing which can affect
 * "player_can_see_bold()".
 */
bool set_blind(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->cut = 0;
		return FALSE;
	}

	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0]) v = 1;

	/* Open */
	if (v)
	{
		if (!p_ptr->blind)
		{
			///msg131221 盲目時アンドロイドでも普通のメッセージ
			msg_print(_("目が見えなくなってしまった！", "You are blind!"));
#if 0
			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
msg_print("センサーをやられた！");
#else
				msg_print("You are blind!");
#endif
			}
			else
			{
#ifdef JP
msg_print("目が見えなくなってしまった！");
#else
				msg_print("You are blind!");
#endif
			}
#endif
			notice = TRUE;
			///del131221 virtue
			//chg_virtue(V_ENLIGHTEN, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blind)
		{
			///msg131221 盲目時アンドロイドでも普通のメッセージ
			msg_print(_("やっと目が見えるようになった。", "You can see again."));
#if 0
			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
msg_print("センサーが復旧した。");
#else
				msg_print("You can see again.");
#endif
			}
			else
			{
#ifdef JP
msg_print("やっと目が見えるようになった。");
#else
				msg_print("You can see again.");
#endif
			}
#endif
			notice = TRUE;

		}
	}

	/* Use the value */
	p_ptr->blind = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Fully update the visuals */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE | PU_VIEW | PU_LITE | PU_MONSTERS | PU_MON_LITE);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->confused", notice observable changes
 */
bool set_confused(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->confused)
		{
#ifdef JP
msg_print("あなたは混乱した！");
#else
			msg_print("You are confused!");
#endif

			if (p_ptr->action == ACTION_LEARN)
			{
#ifdef JP
				msg_print("学習が続けられない！");
#else
				msg_print("You cannot continue Learning!");
#endif
				new_mane = FALSE;

				p_ptr->redraw |= (PR_STATE);
				p_ptr->action = ACTION_NONE;
			}
			if (p_ptr->action == ACTION_KAMAE)
			{
#ifdef JP
				msg_print("構えがとけた。");
#else
				msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KAMAE_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->action = ACTION_NONE;
			}
			else if (p_ptr->action == ACTION_KATA)
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_YUYUKO)
					msg_print("あなたは技を中断した。");
				else

				msg_print("型が崩れた。");
#else
				if(p_ptr->pclass == CLASS_YUYUKO)
					msg_print("Your ability is interrupted.");
				else
                    msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->action = ACTION_NONE;
			}

			//v1.1.27 結界ガード
			set_mana_shield(FALSE,FALSE);

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if (CLASS_USE_CONCENT) reset_concentration(TRUE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			notice = TRUE;
			p_ptr->counter = FALSE;
			chg_virtue(V_HARMONY, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->confused)
		{
#ifdef JP
msg_print("やっと混乱がおさまった。");
#else
			msg_print("You feel less confused now.");
#endif

			//p_ptr->special_attack &= ~(ATTACK_SUIKEN);
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->confused = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->poisoned", notice observable changes
 */
/*:::毒に冒されたり治る処理*/
bool set_poisoned(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->poisoned)
		{
#ifdef JP
msg_print("毒に侵されてしまった！");
#else
			msg_print("You are poisoned!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->poisoned)
		{
#ifdef JP
msg_print("やっと毒の痛みがなくなった。");
#else
			msg_print("You are no longer poisoned.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->poisoned = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->afraid", notice observable changes
 */
bool set_afraid(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->afraid)
		{
#ifdef JP
msg_print("何もかも恐くなってきた！");
#else
			msg_print("You are terrified!");
#endif

			if (p_ptr->special_defense & KATA_MASK)
			{
#ifdef JP
				msg_print("型が崩れた。");
#else
				msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->action = ACTION_NONE;
			}

			notice = TRUE;
			p_ptr->counter = FALSE;
			chg_virtue(V_VALOUR, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->afraid)
		{
#ifdef JP
msg_print("やっと恐怖を振り払った。");
#else
			msg_print("You feel bolder now.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->afraid = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->paralyzed", notice observable changes
 */
bool set_paralyzed(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->paralyzed = 0;
		return FALSE;
	}

	/* Open */
	if (v)
	{
		if (!p_ptr->paralyzed)
		{
#ifdef JP
			///msg131221
msg_print("動けない！");
#else
			msg_print("You cannot move!");
#endif

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if(CLASS_USE_CONCENT) reset_concentration(TRUE);

			//v1.1.27 結界ガード
			set_mana_shield(FALSE,FALSE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->paralyzed)
		{
#ifdef JP
msg_print("やっと動けるようになった。");
#else
			msg_print("You can move again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->paralyzed = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->image", notice observable changes
 *
 * Note that we must redraw the map when hallucination changes.
 */
bool set_image(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	/* Open */
	if (v)
	{
		set_tsuyoshi(0, TRUE);
		if (!p_ptr->image)
		{
#ifdef JP
msg_print("ワーオ！何もかも虹色に見える！");
#else
			msg_print("Oh, wow! Everything looks so cosmic now!");
#endif

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if(CLASS_USE_CONCENT ) reset_concentration(TRUE);
			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->image)
		{
#ifdef JP
msg_print("やっとはっきりと物が見えるようになった。");
#else
			msg_print("You can see clearly again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->image = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	if(CHECK_FAIRY_TYPE == 46) p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->fast", notice observable changes
 */
bool set_fast(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->fast && !do_dec)
		{
			if (p_ptr->fast > v) return FALSE;
		}
		else if (!IS_FAST() && !p_ptr->lightspeed)
		{
#ifdef JP
msg_print("素早く動けるようになった！");
#else
			msg_print("You feel yourself moving much faster!");
#endif

			notice = TRUE;
			///del131216 virtue
			//chg_virtue(V_PATIENCE, -1);
			//chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->fast && !p_ptr->lightspeed && !music_singing(MUSIC_SPEED) && !music_singing(MUSIC_SHERO))
		{
#ifdef JP
msg_print("動きの素早さがなくなったようだ。");
#else
			msg_print("You feel yourself slow down.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->fast = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->lightspeed", notice observable changes
 */
/*:::錬気術師の高速移動。vは残り時間か？*/
/*:::状態に何か変化があればTRUE,なければFALSEを返すらしい*/
bool set_lightspeed(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if (p_ptr->wild_mode) v = 0;

	/* Open */
	if (v)
	{
		if (p_ptr->lightspeed && !do_dec)
		{
			if (p_ptr->lightspeed > v) return FALSE;
		}
		else if (!p_ptr->lightspeed)
		{
#ifdef JP
msg_print("非常に素早く動けるようになった！");
#else
			msg_print("You feel yourself moving extremely fast!");
#endif

			notice = TRUE;
			chg_virtue(V_PATIENCE, -1);
			chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->lightspeed)
		{
#ifdef JP
msg_print("動きの素早さがなくなったようだ。");
#else
			msg_print("You feel yourself slow down.");
#endif
			notice = TRUE;


		}
	}

	/* Use the value */
	p_ptr->lightspeed = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->slow", notice observable changes
 */
bool set_slow(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_INER) v = 0;

	//v1.1.71 早鬼は減速を受けない
	if (p_ptr->pclass == CLASS_SAKI) v = 0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->slow = 0;
		return FALSE;
	}

	/* Open */
	if (v)
	{
		if (p_ptr->slow && !do_dec)
		{
			if (p_ptr->slow > v) return FALSE;
		}
		else if (!p_ptr->slow)
		{
#ifdef JP
msg_print("体の動きが遅くなってしまった！");
#else
			msg_print("You feel yourself moving slower!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->slow)
		{
#ifdef JP
msg_print("動きの遅さがなくなったようだ。");
#else
			msg_print("You feel yourself speed up.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->slow = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shield", notice observable changes
 */
/*:::石肌　解ける効果もここ*/
/*:::v:ターン数
 *:::do_dec:vがp_ptr->shield値より低い場合にもvを適用するフラグ タイムアウトなどの処理　
 */
bool set_shield(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		/*:::do_decがFALSEのときは、vのほうが小さい場合何もしない*/
		if (p_ptr->shield && !do_dec)
		{
			if (p_ptr->shield > v) return FALSE;
		}
		else if (!p_ptr->shield)
		{
#ifdef JP
			///msg131221
msg_print("防御力が上がった。");
#else
			msg_print("Your defensive power increases.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->shield)
		{
#ifdef JP
msg_print("防御の魔法が解けた。");
#else
			msg_print("The protective spell wears off.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shield = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::破邪領域の神降ろし 排他処理のためルーチンまとめる*/
//カウントはp_ptr->kamioroshi_countで行う
bool set_kamioroshi(u32b kamioroshi_type, int v)
{
	bool flag_master = FALSE;

	//神降ろしを２つ同時に降ろせるフラグ
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_PUNISH && p_ptr->lev > 39
		|| p_ptr->pclass == CLASS_REIMU && osaisen_rank() == 9
		|| p_ptr->pclass == CLASS_YORIHIME)
		flag_master = TRUE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	//v1.1.14 天宇受売命の神降ろし中に天照大御神を呼ぶと天宇受売命がキャンセルされるようにした
	if((kamioroshi_type & KAMIOROSHI_AMATERASU) && (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME))
	{
		p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
		msg_print(_("天宇受売命は役目を果たして消えた。",
                    "Ame-no-Uzume-no-Mikoto disappears, having completed her duty."));
	}

	//v1.1.14 依姫が祇園様の神降ろしをしたときconcent処理
	//reset_concertration()にこれに関する例外処理追加
	if(p_ptr->pclass == CLASS_YORIHIME && ((kamioroshi_type | p_ptr->kamioroshi)& KAMIOROSHI_GION))
	{
		reset_concent = FALSE;
	}


	///mod141019 破邪ハイの神降ろし強化　２つ同時に降ろせるようにする
	///mod150201 最大ランクの霊夢も同じにする
	if(flag_master && kamioroshi_type)
	{
		int count = 0;
		int i;

		for(i=0;i<32;i++) if((p_ptr->kamioroshi >> i)&1L) count++;
		//msg_format("count:%d",count);

		if(count>1) for(i=0;i<32;i++)
		{
			if((p_ptr->kamioroshi >> i)%2 == 0 || kamioroshi_type == (1L << i)) continue;

			switch(1L << i)
			{
			case KAMIOROSHI_IZUNA:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_IZUNA);
				msg_print(_("飯綱権現の力が消えた。", "The power of Izuna-Gongen goes away."));
				break;

			case KAMIOROSHI_ISHIKORIDOME:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_ISHIKORIDOME);
				msg_print(_("石凝姥命の力が消えた。", "The power of Ishikori-dome-no-Mikoto goes away."));
				break;

			case KAMIOROSHI_SUMIYOSHI:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_SUMIYOSHI);
				msg_print(_("住吉三神の力が消えた。", "The power of the Sumiyoshi Sanjin goes away."));
				break;

			case KAMIOROSHI_ATAGO:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_ATAGO);
				msg_print(_("愛宕様の力が消えた。", "The power of Atago-Gongen goes away."));
				break;

			case KAMIOROSHI_AMENOUZUME:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
				msg_print(_("天宇受売命の力が消えた。", "The power of Ame-no-Uzume-no-Mikoto goes away."));
				break;

			case KAMIOROSHI_GION:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_GION);
				if(p_ptr->paralyzed || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
					msg_print(_("祇園様の剣が地面から抜けた。", "Sword of Gion leaves the ground."));
				else
					msg_print(_("祇園様の剣を地面から引き抜いた。", "You pull Sword of Gion out of the ground."));

				break;

			case KAMIOROSHI_KANAYAMAHIKO:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_KANAYAMAHIKO);
				msg_print(_("金山彦命の力が消えた。", "The power of Kanayamahiko-no-Mikoto goes away."));
				break;


			}
			break; //消えるのは最初の一つだけ

		}
	}
	else
	{

		if ((p_ptr->kamioroshi & (KAMIOROSHI_IZUNA)) && (kamioroshi_type != KAMIOROSHI_IZUNA))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_IZUNA);
			msg_print(_("飯綱権現の力が消えた。", "The power of Izuna-Gongen goes away."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_ISHIKORIDOME)) && (kamioroshi_type != KAMIOROSHI_ISHIKORIDOME))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_ISHIKORIDOME);
			msg_print(_("石凝姥命の力が消えた。", "The power of Ishikori-dome-no-Mikoto goes away."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_SUMIYOSHI)) && (kamioroshi_type != KAMIOROSHI_SUMIYOSHI))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_SUMIYOSHI);
			msg_print(_("住吉三神の力が消えた。", "The power of the Sumiyoshi Sanjin goes away."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_ATAGO)) && (kamioroshi_type != KAMIOROSHI_ATAGO))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_ATAGO);
			msg_print(_("愛宕様の力が消えた。", "The power of Atago-Gongen goes away."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_AMENOUZUME)) && (kamioroshi_type != KAMIOROSHI_AMENOUZUME))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
			msg_print(_("天宇受売命の力が消えた。", "The power of Ame-no-Uzume-no-Mikoto goes away."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_GION)) && (kamioroshi_type != KAMIOROSHI_GION))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_GION);
			msg_print(_("祇園様の剣を地面から引き抜いた。", "You pull Sword of Gion out of the ground."));
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_KANAYAMAHIKO)) && (kamioroshi_type != KAMIOROSHI_KANAYAMAHIKO))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_KANAYAMAHIKO);
			msg_print(_("金山彦命の力が消えた。", "The power of Kanayamahiko-no-Mikoto goes away."));
		}

	}

	//一瞬だけ発動して常駐しないタイプの神降ろしはフラグ値を0にする
	if(kamioroshi_type == KAMIOROSHI_AMATO || kamioroshi_type == KAMIOROSHI_IZUNOME || kamioroshi_type == KAMIOROSHI_AMATERASU
		||kamioroshi_type == KAMIOROSHI_YAOYOROZU || kamioroshi_type == KAMIOROSHI_HONOIKAZUTI || kamioroshi_type == KAMIOROSHI_AMATSUMIKABOSHI)
		kamioroshi_type = 0L;

	if ((v) && (kamioroshi_type))
	{
		p_ptr->kamioroshi |= (kamioroshi_type);
		p_ptr->kamioroshi_count = v;
	}
	//即時型神降ろしはカウントとフラグをリセット。ただし神降ろし同時可能な職はリセットしない。
	//2つ降ろしているなら片方残るし1つしか降ろしてないなら枠に余裕があるため。
	else if(!v && !kamioroshi_type && !flag_master)
	{
		p_ptr->kamioroshi_count = 0;
		p_ptr->kamioroshi = 0L;
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	///v1.1.14 依姫の祇園様が消えた時concent値があれば0にする。魔力消去やタイムアウトなど
	if(p_ptr->pclass == CLASS_YORIHIME && !(p_ptr->kamioroshi & KAMIOROSHI_GION) && p_ptr->concent)
		p_ptr->concent = 0;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS | PR_STATS | PR_ARMOR);

	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*:::暗黒領域　闇との融和*/
bool set_tim_unite_darkness(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_unite_darkness && !do_dec)
		{
			if (p_ptr->tim_unite_darkness > v) return FALSE;
		}
		else if (!p_ptr->tim_unite_darkness)
		{
#ifdef JP
msg_print("あなたの体は闇へ溶けた。");
#else
			msg_print("Your body melts into darkness.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_unite_darkness)
		{
#ifdef JP
msg_print("あなたは闇の中から姿を現した。");
#else
			msg_print("You reappear from the darkness.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_unite_darkness = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_TORCH | PU_VIEW | PU_LITE | PU_MON_LITE | PU_MONSTERS );
	p_ptr->redraw |= PR_MAP;

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_superstealth(int v, bool do_dec, int stealth_type)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		//超隠密のタイプ(普通、光学迷彩、森隠れなど)を記録。使用中に他のタイプのを使用したらそちらに上書き
		if (!do_dec)
		{
			p_ptr->superstealth_type = stealth_type;

		}

		/*:::do_decがFALSEのときは、vのほうが小さい場合何もしない ただしstealthのタイプは新しいほうに変わる*/
		if (p_ptr->tim_superstealth && !do_dec)
		{
			if (p_ptr->tim_superstealth > v) return FALSE;
		}
		else if (!p_ptr->tim_superstealth)
		{
			if (stealth_type == SUPERSTEALTH_TYPE_FOREST)
			{
				if (cave_have_flag_bold((py), (px), FF_TREE))
					msg_print(_("あなたの姿は森の中に消えた...", "Your figure vanishes into the forest..."));
				else
					msg_print(_("あなたは森の中に身を隠すべく身構えた。", "You prepare to hide in the forest."));

			}
			else if(p_ptr->pclass == CLASS_3_FAIRIES)
			///msg131221
				msg_print(_("あなたたちは敵の目に映らなくなった。", "You conceal yourself from hostile eyes."));
			else
				msg_print(_("あなたは敵の目に映らなくなった。", "You conceal yourself from hostile eyes."));

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		//超隠密のタイプをsuperstealth_typeに記録する前のフラグ
		//v1.1.88 superstealth_typeに置き換えたが、アップデートのときに光学迷彩中のプレイヤーがいるかもしれんのでしばらくフラグを落とす処理を残しとく
		//v2.0.1 もう消していいだろう
		//p_ptr->special_defense &= ~(SD_OPTICAL_STEALTH);


		if (p_ptr->tim_superstealth)
		{
			msg_print(_("再び敵の目に映るようになった。", "You are no longer concealed from hostile eyes."));
			notice = TRUE;
		}

		p_ptr->superstealth_type = 0;
	}

	/* Use the value */
	p_ptr->tim_superstealth = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//check
	if (cheat_xtra) msg_format("stealth type:%d", p_ptr->superstealth_type);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



//v1.1.93 一時反感処理
bool set_tim_aggravation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_aggravation && !do_dec)
		{
			if (p_ptr->tim_aggravation > v) return FALSE;
		}
		else if (!p_ptr->tim_aggravation)
		{
			msg_print(_("急に嫌われ者になった気がする...", "You suddenly feel like everyone hates you..."));

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_aggravation)
		{
			msg_print(_("嫌われている感じがなくなった。", "You no longer feel hated."));

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_aggravation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}




/*
 * Set "p_ptr->tsubureru", notice observable changes
 */
/*:::横に伸びる→死霊のオーラに変更*/
bool set_tim_sh_death(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_death && !do_dec)
		{
			if (p_ptr->tim_sh_death > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_death)
		{
#ifdef JP
msg_print("呻き声を上げる死霊たちがあなたの周りに渦巻いた。");
#else
			msg_print("Moaning ghosts start swirling around you.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_death)
		{
#ifdef JP
msg_print("あなたの周りの死霊たちが消えた。");
#else
			msg_print("The ghosts around you disappear.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_death = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->magicdef", notice observable changes
 */
/*:::魔法の鎧　魔法防御とAC上昇、いくつかのステータス異常耐性　魔力消去の対象*/
bool set_magicdef(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->magicdef && !do_dec)
		{
			if (p_ptr->magicdef > v) return FALSE;
		}
		else if (!p_ptr->magicdef)
		{
#ifdef JP
			msg_print("魔法の防御力が増したような気がする。");
#else
			msg_print("You feel more resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->magicdef)
		{
#ifdef JP
			msg_print("魔法の防御力が元に戻った。");
#else
			msg_print("You feel less resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->magicdef = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->blessed", notice observable changes
 */
bool set_blessed(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->blessed && !do_dec)
		{
			if (p_ptr->blessed > v) return FALSE;
		}
		else if (!IS_BLESSED())
		{
#ifdef JP
			///msg131221
			msg_print("大いなる何かの祝福を受けた感じがする。");
#else
			msg_print("You feel like you've received a great blessing!");
#endif
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blessed && !music_singing(MUSIC_BLESS))
		{
#ifdef JP
msg_print("祝福された感じが消えた。");
#else
			msg_print("You no longer feel blessed.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->blessed = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->hero", notice observable changes
 */
bool set_hero(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->hero && !do_dec)
		{
			if (p_ptr->hero > v) return FALSE;
		}
		else if (!IS_HERO())
		{
#ifdef JP
msg_print("ヒーローになった気がする！");
#else
			msg_print("You feel like a hero!");
#endif

			notice = TRUE;
		}

		//v1.1.38 ヒーロー化するとき恐怖が消える処理を付け忘れやすいので、いっそここに持ってくることにした。
		if(!do_dec)	set_afraid(0);


	}

	/* Shut */
	else
	{
		if (p_ptr->hero && !music_singing(MUSIC_HERO) && !music_singing(MUSIC_SHERO))
		{
#ifdef JP
msg_print("ヒーローの気分が消え失せた。");
#else
			msg_print("The heroism wears off.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->hero = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shero", notice observable changes
 */
bool set_shero(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(p_ptr->pclass == CLASS_EIRIN && p_ptr->pseikaku != SEIKAKU_BERSERK && v && !do_dec)
	{
#ifdef JP
		msg_print("あなたは冷静さを失わなかった。");
#else
		msg_print("You retain your composure.");
#endif
		return FALSE;
	}

	///sys class 狂戦士は常にバーサークになる処理
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) v = 1;
	/* Open */
	if (v)
	{
		if (p_ptr->shero && !do_dec)
		{
			if (p_ptr->shero > v) return FALSE;
		}
		else if (!p_ptr->shero)
		{
#ifdef JP
msg_print("殺戮マシーンになった気がする！");
#else
			msg_print("You feel like a killing machine!");
#endif
			if(one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);

			notice = TRUE;
		}

		//v1.1.38 ヒーロー化するとき恐怖が消える処理を付け忘れやすいので、いっそここに持ってくることにした。
		if(!do_dec)	set_afraid(0);


	}

	/* Shut */
	else
	{
		if (p_ptr->shero)
		{
#ifdef JP
msg_print("野蛮な気持ちが消え失せた。");
#else
			msg_print("You feel less Berserk.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shero = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->protevil", notice observable changes
 */
bool set_protevil(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0])
	{
		msg_print(_("守護の魔法が厄を打ち払った！", "The protective magic dispels your misfortune!"));
		hina_gain_yaku(-v);
		v=0;
	}


	/* Open */
	if (v)
	{
		if (p_ptr->protevil && !do_dec)
		{
			if (p_ptr->protevil > v) return FALSE;
		}
		else if (!p_ptr->protevil)
		{
#ifdef JP
msg_print("邪悪なる存在から守られているような感じがする！");
#else
			msg_print("You feel safe from evil!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->protevil)
		{
#ifdef JP
msg_print("邪悪なる存在から守られている感じがなくなった。");
#else
			msg_print("You no longer feel safe from evil.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->protevil = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->wraith_form", notice observable changes
 */
///mod140213 ルーミアのダークサイドオブザムーンを幽体化と共有処理にした。
bool set_wraith_form(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->wraith_form && !do_dec)
		{
			if (p_ptr->wraith_form > v) return FALSE;
		}
		else if (!p_ptr->wraith_form)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("あなたの体は闇に溶けた。");
			else msg_print("物質界を離れて幽鬼のような存在になった！");
#else
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("Your body melts into darkness.");
            else msg_print("You leave the physical world and turn into a wraith-being!");
#endif

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_UNLIFE, 3);
			//chg_virtue(V_HONOUR, -2);
			//chg_virtue(V_SACRIFICE, -2);
			//chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->wraith_form)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("あなたの体は再びはっきりした形をとった。");
			else msg_print("不透明になった感じがする。");
#else
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("Your body has a solid form once again.");
			else msg_print("You feel opaque.");
#endif

			notice = TRUE;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Use the value */
	p_ptr->wraith_form = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);

}


/*
 * Set "p_ptr->invuln", notice observable changes
 */
bool set_invuln(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->invuln && !do_dec)
		{
			if (p_ptr->invuln > v) return FALSE;
		}
		else if (!IS_INVULN())
		{
#ifdef JP
msg_print("無敵だ！");
#else
			msg_print("Invulnerability!");
#endif

			notice = TRUE;
			///del131221 virtue
			//chg_virtue(V_UNLIFE, -2);
			//chg_virtue(V_HONOUR, -2);
			//chg_virtue(V_SACRIFICE, -3);
			//chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->invuln && !music_singing(MUSIC_INVULN))
		{
#ifdef JP
msg_print("無敵ではなくなった。");
#else
			msg_print("The invulnerability wears off.");
#endif

			notice = TRUE;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			p_ptr->energy_need += ENERGY_NEED();
		}
	}

	/* Use the value */
	p_ptr->invuln = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_esp", notice observable changes
 */
/*:::一時テレパスを得る*/
bool set_tim_esp(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_esp && !do_dec)
		{
			if (p_ptr->tim_esp > v) return FALSE;
		}
		else if (!IS_TIM_ESP())
		{
#ifdef JP
msg_print("意識が広がった気がする！");
#else
			msg_print("You feel your consciousness expand!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_esp && !music_singing(MUSIC_MIND))
		{
#ifdef JP
msg_print("意識は元に戻った。");
#else
			msg_print("Your consciousness contracts again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_esp = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_invis", notice observable changes
 */
bool set_tim_invis(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_invis && !do_dec)
		{
			if (p_ptr->tim_invis > v) return FALSE;
		}
		else if (!p_ptr->tim_invis)
		{
#ifdef JP
msg_print("目が非常に敏感になった気がする！");
#else
			msg_print("Your eyes feel very sensitive!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_invis)
		{
#ifdef JP
msg_print("目の敏感さがなくなったようだ。");
#else
			msg_print("Your eyes feel less sensitive.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_invis = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::召喚領域の「送還術」　一定時間敵の召喚を阻害する */
bool set_deportation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->deportation && !do_dec)
		{
			if (p_ptr->deportation > v) return FALSE;
		}
		else if (!p_ptr->deportation)
		{
#ifdef JP
msg_print("あなたは周囲の空間へ干渉を始めた・・");
#else
			msg_print("You start interfering with surrounding space...");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->deportation)
		{
#ifdef JP
msg_print("周囲の空間への干渉が止まった。");
#else
			msg_print("Your interference wears off.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->deportation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_foresight(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->foresight && !do_dec)
		{
			if (p_ptr->foresight > v) return FALSE;
		}
		else if (!p_ptr->foresight)
		{
#ifdef JP
			msg_print("現在の景色に未来の景色が重なって見える！");
#else
			msg_print("You start seeing the future and the present at once!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->foresight)
		{
#ifdef JP
msg_print("未来を見られなくなった。");
#else
			msg_print("You no longer see the future.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->foresight = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_infra", notice observable changes
 */
bool set_tim_infra(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_infra && !do_dec)
		{
			if (p_ptr->tim_infra > v) return FALSE;
		}
		else if (!p_ptr->tim_infra)
		{
#ifdef JP
msg_print("目がランランと輝き始めた！");
#else
			msg_print("Your eyes begin to shine!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_infra)
		{
#ifdef JP
msg_print("目の輝きがなくなった。");
#else
			msg_print("Your eyes stop shining.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_infra = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_regen", notice observable changes
 */
///sys 急回復累積したい場合ここも修正
bool set_tim_regen(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_regen && !do_dec)
		{
			if (p_ptr->tim_regen > v) return FALSE;
		}
		else if (!p_ptr->tim_regen)
		{
#ifdef JP
msg_print("回復力が上がった！");
#else
			msg_print("You feel yourself regenerating quickly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_regen)
		{
#ifdef JP
msg_print("素早く回復する感じがなくなった。");
#else
			msg_print("You feel yourself regenerating slowly.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_regen = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_stealth", notice observable changes
 */
bool set_tim_stealth(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_stealth && !do_dec)
		{
			if (p_ptr->tim_stealth > v) return FALSE;
		}
		else if (!IS_TIM_STEALTH())
		{
#ifdef JP
			///msg131221
			msg_print("あなたは気配を消した。");
#else
			msg_print("You conceal your presence.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_stealth && !music_singing(MUSIC_STEALTH))
		{
#ifdef JP
			msg_print("気配を消せなくなった。");
#else
			msg_print("You no longer conceal your presence.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_stealth = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

///class 忍者の超隠密
bool set_superstealth(bool set)
{
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (set)
	{
		if (!(p_ptr->special_defense & NINJA_S_STEALTH))
		{
			if (cave[py][px].info & CAVE_MNLT)
			{
#ifdef JP
				if(!p_ptr->tim_superstealth) msg_print("敵の目から薄い影の中に覆い隠された。");
#else
				if(!p_ptr->tim_superstealth) msg_print("You are mantled in weak shadow from ordinary eyes.");
#endif
				p_ptr->monlite = p_ptr->old_monlite = TRUE;
			}
			else
			{
#ifdef JP
				if(!p_ptr->tim_superstealth) msg_print("敵の目から影の中に覆い隠された！");
#else
				if(!p_ptr->tim_superstealth) msg_print("You are mantled in shadow from ordinary eyes!");
#endif
				p_ptr->monlite = p_ptr->old_monlite = FALSE;
			}

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense |= NINJA_S_STEALTH;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->special_defense & NINJA_S_STEALTH)
		{
#ifdef JP
			if(!p_ptr->tim_superstealth) msg_print("再び敵の目にさらされるようになった。");
#else
			if(!p_ptr->tim_superstealth) msg_print("You are exposed to common sight once more.");
#endif

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense &= ~(NINJA_S_STEALTH);
		}
	}

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_levitation", notice observable changes
 */
/*:::一時浮遊　基本的な一時ステータス関数*/
bool set_tim_levitation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_levitation && !do_dec)
		{
			if (p_ptr->tim_levitation > v) return FALSE;
		}
		else if (!p_ptr->tim_levitation)
		{
#ifdef JP
			///msg131221
			msg_print("あなたの体は宙に浮かんだ。");
#else
			msg_print("You begin to fly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_levitation)
		{
#ifdef JP
msg_print("もう宙に浮かべなくなった。");
#else
			msg_print("You stop flying.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_levitation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_touki", notice observable changes
 */
/*:::纏闘気　基本的な一時ステータス処理*/
bool set_tim_sh_touki(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_touki && !do_dec)
		{
			if (p_ptr->tim_sh_touki > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("体が闘気のオーラで覆われた。");
#else
			msg_print("You have enveloped by the aura of the Force!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("闘気が消えた。");
#else
			msg_print("Aura of the Force disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_touki = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_fire", notice observable changes
 */
bool set_tim_sh_fire(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_fire && !do_dec)
		{
			if (p_ptr->tim_sh_fire > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("体が炎のオーラで覆われた。");
#else
			msg_print("You have enveloped by fiery aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("炎のオーラが消えた。");
#else
			msg_print("Fiery aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_fire = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_holy", notice observable changes
 */
bool set_tim_sh_holy(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_holy && !do_dec)
		{
			if (p_ptr->tim_sh_holy > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("体が聖なるオーラで覆われた。");
#else
			msg_print("You have enveloped by holy aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("聖なるオーラが消えた。");
#else
			msg_print("Holy aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_holy = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->tim_eyeeye", notice observable changes
 */
bool set_tim_eyeeye(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_eyeeye && !do_dec)
		{
			if (p_ptr->tim_eyeeye > v) return FALSE;
		}
		else if (!p_ptr->tim_eyeeye)
		{
#ifdef JP
			///msg131221
			//msg_print("法の守り手になった気がした！");
			msg_print("あなたは攻撃を待ち構えている。");

#else
			msg_print("You prepare for an attack.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_eyeeye)
		{
#ifdef JP
			msg_print("攻撃を待ち構えるのをやめた。");
			//msg_print("懲罰を執行することができなくなった。");
#else
			msg_print("You are no longer preparing for an attack.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_eyeeye = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->resist_magic", notice observable changes
 */
/*:::魔法防御上昇　錬気術と鏡魔法*/
bool set_resist_magic(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->resist_magic && !do_dec)
		{
			if (p_ptr->resist_magic > v) return FALSE;
		}
		else if (!p_ptr->resist_magic)
		{
#ifdef JP
msg_print("魔法への耐性がついた。");
#else
			msg_print("You have been protected from magic!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->resist_magic)
		{
#ifdef JP
msg_print("魔法に弱くなった。");
#else
msg_print("You are no longer protected from magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->resist_magic = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_reflect", notice observable changes
 */
bool set_tim_reflect(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_reflect && !do_dec)
		{
			if (p_ptr->tim_reflect > v) return FALSE;
		}
		else if (!p_ptr->tim_reflect)
		{
#ifdef JP
			///msg131221
msg_print("今なら自分を狙った矢を跳ね返せる気がする。");
#else
			msg_print("You fell like arrows are going to bounce off your body.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_reflect)
		{
#ifdef JP
msg_print("矢玉からの防護が消えた。");
#else
			msg_print("You are no longer protected from projectiles.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_reflect = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * 死霊領域の「念縛」用の処理
 */
bool set_nennbaku(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->nennbaku && !do_dec)
		{
			if (p_ptr->nennbaku > v) return FALSE;
		}
		else if (!p_ptr->nennbaku)
		{
			if(p_ptr->pclass == CLASS_YUGI)
				msg_print(_("鎖と枷が威圧的に宙を舞った。", "Chains and shackles intimidatingly dance in the air."));
			else if (p_ptr->pclass == CLASS_KUTAKA); //メッセージなし
			else
				msg_print(_("地面から半透明の無数の腕が生えてきた・・", "Countless translucent arms rise from the ground..."));

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->nennbaku)
		{
			if(p_ptr->pclass == CLASS_YUGI)
				msg_print(_("鎖と枷が消えた。", "The chains and shackles disappear."));
			else if (p_ptr->pclass == CLASS_KUTAKA); //メッセージなし
			else
				msg_print(_("地面からの腕が消えた。", "The arms from the ground disappear."));

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->nennbaku = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->multishadow", notice observable changes
 */
/*:::分身処理*/
bool set_multishadow(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->multishadow && !do_dec)
		{
			if (p_ptr->multishadow > v) return FALSE;
		}
		else if (!p_ptr->multishadow)
		{
			if(p_ptr->pclass == CLASS_NUE || p_ptr->pclass == CLASS_AUNN)
				msg_format(_("あなたの隣に分身が姿を現した。", "Your clone appears by your side."));
			else if (p_ptr->pclass == CLASS_SHINMYOUMARU)
				msg_format(_("あなたは七人に分身した！", "You split into seven people!"));
			else
				msg_print(_("あなたの周りに幻影が生まれた。", "Shadows appear around you."));

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->multishadow)
		{
#ifdef JP
			if (p_ptr->pclass == CLASS_AUNN)
				msg_format("あなたの分身は神社に戻った。");
			else
				msg_print("幻影が消えた。");
#else
            if (p_ptr->pclass == CLASS_AUNN)
                msg_print("Your clone returns to the shrine.");
            else
                msg_print("Your Shadow disappears.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->multishadow = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->dustrobe", notice observable changes
 */
bool set_dustrobe(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->dustrobe && !do_dec)
		{
			if (p_ptr->dustrobe > v) return FALSE;
		}
		else if (!p_ptr->dustrobe)
		{
#ifdef JP

//msg_print("体が鏡のオーラで覆われた。");
#else
//			msg_print("You were enveloped by mirror shards.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->dustrobe)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_IKU) msg_print("棘が消えた。");
			else if(p_ptr->pclass == CLASS_PATCHOULI) msg_print("回転する刃が消えた。");
			else if(p_ptr->pclass == CLASS_SANAE) msg_print("宙に描いた九字が消えた。");
			else if(p_ptr->pclass == CLASS_NITORI) msg_print("ギアの回転が止まった。");
			else if(p_ptr->pclass == CLASS_BYAKUREN) msg_print("護符が消えた。");
			else msg_print("鏡のオーラが消えた。");
#else
            if(p_ptr->pclass == CLASS_IKU) msg_print("The thorns disappear.");
			else if(p_ptr->pclass == CLASS_PATCHOULI) msg_print("The spinning blades disappear.");
			else if(p_ptr->pclass == CLASS_SANAE) msg_print("The nine syllables drawn in the air disappear.");
			else if(p_ptr->pclass == CLASS_NITORI) msg_print("The gears stop rotating.");
			else if(p_ptr->pclass == CLASS_BYAKUREN) msg_print("The protective seal disappears.");
			else msg_print("The mirror shards disappear.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->dustrobe = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_regen", notice observable changes
 */
/*:::壁抜け状態の管理*/
bool set_kabenuke(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->kabenuke && !do_dec)
		{
			if (p_ptr->kabenuke > v) return FALSE;
		}
		else if (!p_ptr->kabenuke)
		{
#ifdef JP
msg_print("体が半物質の状態になった。");
#else
			msg_print("You became ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->kabenuke)
		{
#ifdef JP
msg_print("体が物質化した。");
#else
			msg_print("You are no longer in an ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->kabenuke = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tsuyoshi(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tsuyoshi && !do_dec)
		{
			if (p_ptr->tsuyoshi > v) return FALSE;
		}
		else if (!p_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("「オクレ兄さん！」");
#else
			msg_print("Brother OKURE!");
#endif

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_VITALITY, 2);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("肉体が急速にしぼんでいった。");
#else
			msg_print("Your body had quickly shriveled.");
#endif

			(void)dec_stat(A_CON, 20, TRUE);
			(void)dec_stat(A_STR, 20, TRUE);

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_VITALITY, -3);
		}
	}

	/* Use the value */
	p_ptr->tsuyoshi = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status
 * messages. -LM-
 */
///mod140725 魔法剣士など魔法剣の種類増やした
bool set_ele_attack(u32b attack_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((p_ptr->special_attack & (ATTACK_ACID)) && (attack_type != ATTACK_ACID))
	{
		p_ptr->special_attack &= ~(ATTACK_ACID);
		msg_print(_("酸で攻撃できなくなった。", "You no longer attack with acid."));
	}
	if ((p_ptr->special_attack & (ATTACK_ELEC)) && (attack_type != ATTACK_ELEC))
	{
		p_ptr->special_attack &= ~(ATTACK_ELEC);
		msg_print(_("電撃で攻撃できなくなった。", "You no longer attack with electricity."));
	}
	if ((p_ptr->special_attack & (ATTACK_FIRE)) && (attack_type != ATTACK_FIRE))
	{
		p_ptr->special_attack &= ~(ATTACK_FIRE);
		msg_print(_("火炎で攻撃できなくなった。", "You no longer attack with fire."));
	}
	if ((p_ptr->special_attack & (ATTACK_COLD)) && (attack_type != ATTACK_COLD))
	{
		p_ptr->special_attack &= ~(ATTACK_COLD);
		msg_print(_("冷気で攻撃できなくなった。", "You no longer attack with cold."));
	}
	if ((p_ptr->special_attack & (ATTACK_POIS)) && (attack_type != ATTACK_POIS))
	{
		p_ptr->special_attack &= ~(ATTACK_POIS);
		msg_print(_("毒で攻撃できなくなった。", "You no longer attack with poison."));
	}
	if ((p_ptr->special_attack & (ATTACK_DARK)) && (attack_type != ATTACK_DARK))
	{
		p_ptr->special_attack &= ~(ATTACK_DARK);
		msg_print(_("魔剣の輝きが失せた。", "The demonsword glow fades."));
	}
	if ((p_ptr->special_attack & (ATTACK_FORCE)) && (attack_type != ATTACK_FORCE))
	{
		p_ptr->special_attack &= ~(ATTACK_FORCE);
		msg_print(_("理力の輝きが失せた。", "The force weapon glow fades."));
	}
	if ((p_ptr->special_attack & (ATTACK_FORESIGHT)) && (attack_type != ATTACK_FORESIGHT))
	{
		p_ptr->special_attack &= ~(ATTACK_FORESIGHT);
		msg_print(_("武器が動く感じがなくなった。", "You no longer feel your weapon moving."));
	}
	if ((p_ptr->special_attack & (ATTACK_UNSUMMON)) && (attack_type != ATTACK_UNSUMMON))
	{
		p_ptr->special_attack &= ~(ATTACK_UNSUMMON);
		msg_print(_("武器から歪みが消えた。", "The distortion on your weapon disappears."));
	}
	if ((p_ptr->special_attack & (ATTACK_INC_DICES)) && (attack_type != ATTACK_INC_DICES))
	{
		p_ptr->special_attack &= ~(ATTACK_INC_DICES);
		msg_print(_("武器の強化魔法の効力が消えた。", "The empowering spell on your weapon wears off."));
	}
	if ((p_ptr->special_attack & (ATTACK_CHAOS)) && (attack_type != ATTACK_CHAOS))
	{
		p_ptr->special_attack &= ~(ATTACK_CHAOS);
		msg_print(_("武器から混沌の力が消えた。", "Chaotic powers disappear from your weapon."));
	}

	if ((p_ptr->special_attack & (ATTACK_VORPAL)) && (attack_type != ATTACK_VORPAL))
	{
		p_ptr->special_attack &= ~(ATTACK_VORPAL);
		msg_print(_("刃の切れが鈍った気がする。", "You feel your blade has gotten duller."));
	}

	if ((v) && (attack_type))
	{
		/* Set attack type. */
		p_ptr->special_attack |= (attack_type);

		/* Set duration. */
		p_ptr->ele_attack = v;

		/* Message. */

		if(p_ptr->pclass == CLASS_ALICE)
		{
			if(attack_type == ATTACK_DARK) msg_print(_("人形たちの武器は妖しい光を放った。",
                                                        "The weapons of your dolls glows with a bewitching light."));
			else if(attack_type == ATTACK_ACID) msg_print(_("人形たちの武器から煙が立ち上った。",
                                                            "Smoke rises from the weapons of your dolls."));
			else if(attack_type == ATTACK_ELEC) msg_print(_("人形たちの武器から火花が散った。",
                                                            "Sparks fly from the weapons of your dolls."));
			else if(attack_type == ATTACK_FIRE) msg_print(_("人形たちの武器は赤熱した。",
                                                            "The weapons of your dolls glow red-hot."));
			else if(attack_type == ATTACK_COLD) msg_print(_("人形たちの武器は冷気を放った。",
                                                            "The weapons of your dolls get covered in frost."));
			else if(attack_type == ATTACK_POIS) msg_print(_("人形たちの武器から毒液が滴った。",
                                                            "Poisonous liquid drips from the weapons of your dolls."));
			else if (attack_type == ATTACK_VORPAL) msg_print(_("人形たちの武器は鋭く輝いた。",
                                                                "The weapons of your dolls glow sharply."));
			else msg_print(_("WARNING:アリスset_ele_attack()のメッセージ未定義",
                            "WARNING: Undefined Alice message for set_ele_attack()"));
		}
		else
		{
			if(attack_type == ATTACK_DARK) msg_print(_("あなたの武器は妖しく輝いた・・",
                                                        "Your weapon glows with a bewitching light..."));
			else if(attack_type == ATTACK_FORCE) msg_print(_("武器はあなたの意思に呼応するように光った。",
                                                            "The weapon glows in response to your will."));
			else if(attack_type == ATTACK_FORESIGHT) msg_print(_("武器が勝手に敵の動きを先回りする気がする！",
                                                                "You feel your weapon is reading ahead the enemy movements!"));
			else if(attack_type == ATTACK_UNSUMMON) msg_print(_("武器は奇妙な歪みをまとった・・",
                                                                "Your weapon distorts in a strange way..."));
			else if(attack_type == ATTACK_INC_DICES) msg_print(_("武器に破壊的な魔力が宿った！",
                                                                "Destructive magical power resides in your weapon!"));
			else if(attack_type == ATTACK_CHAOS) msg_print(_("武器が奇妙に捻じれて見える・・",
                                                                "Your weapon looks twisted in a strange way..."));
			else if (attack_type == ATTACK_VORPAL) msg_print(_("武器の刃がギラリと輝いた。",
                                                                "Your weapon's blade shines glaringly."));
			else
				msg_format(_("%sで攻撃できるようになった！", "You start attacking with %s!"),
			     ((attack_type == ATTACK_ACID) ? _("酸", "acid") :
			      ((attack_type == ATTACK_ELEC) ? _("電撃", "electricity") :
			       ((attack_type == ATTACK_FIRE) ? _("火炎", "fire") :
				((attack_type == ATTACK_COLD) ? _("冷気", "cold") :
				 ((attack_type == ATTACK_POIS) ? _("毒", "poison") :
					_("(なし)", "(nothing)")))))));

		}

	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	p_ptr->update |= (PU_BONUS);

	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}



/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status
 * messages. -LM-
 */
/*:::一時元素免疫　属性を記録する必要があるので少し処理がややこしい*/
bool set_ele_immune(u32b immune_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((p_ptr->special_defense & (DEFENSE_ACID)) && (immune_type != DEFENSE_ACID))
	{
		p_ptr->special_defense &= ~(DEFENSE_ACID);
#ifdef JP
		msg_print("酸の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to acid.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_ELEC)) && (immune_type != DEFENSE_ELEC))
	{
		p_ptr->special_defense &= ~(DEFENSE_ELEC);
#ifdef JP
		msg_print("電撃の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to electricity.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_FIRE)) && (immune_type != DEFENSE_FIRE))
	{
		p_ptr->special_defense &= ~(DEFENSE_FIRE);
#ifdef JP
		msg_print("火炎の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to fire.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_COLD)) && (immune_type != DEFENSE_COLD))
	{
		p_ptr->special_defense &= ~(DEFENSE_COLD);
#ifdef JP
		msg_print("冷気の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to cold.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_POIS)) && (immune_type != DEFENSE_POIS))
	{
		p_ptr->special_defense &= ~(DEFENSE_POIS);
#ifdef JP
		msg_print("毒の攻撃で傷つけられるようになった。。");
#else
		msg_print("You are no longer immune to poison.");
#endif
	}

	if ((v) && (immune_type))
	{
		p_ptr->ele_immune = v;

		if(immune_type == DEFENSE_4ELEM)
		{
			p_ptr->special_defense |= (DEFENSE_ACID | DEFENSE_ELEC | DEFENSE_FIRE | DEFENSE_COLD);
			msg_print(_("四元素の攻撃を受け付けなくなった！", "You become immune to the four elements!"));
		}
		else
		{
			/* Set attack type. */
			p_ptr->special_defense |= (immune_type);

		/* Set duration. */

		/* Message. */
#ifdef JP
			msg_format("%sの攻撃を受けつけなくなった！",
			     ((immune_type == DEFENSE_ACID) ? "酸" :
			      ((immune_type == DEFENSE_ELEC) ? "電撃" :
			       ((immune_type == DEFENSE_FIRE) ? "火炎" :
				((immune_type == DEFENSE_COLD) ? "冷気" :
				 ((immune_type == DEFENSE_POIS) ? "毒" :
					"(なし)"))))));
#else
		msg_format("For a while, You are immune to %s",
			     ((immune_type == DEFENSE_ACID) ? "acid!" :
			      ((immune_type == DEFENSE_ELEC) ? "electricity!" :
			       ((immune_type == DEFENSE_FIRE) ? "fire!" :
				((immune_type == DEFENSE_COLD) ? "cold!" :
				 ((immune_type == DEFENSE_POIS) ? "poison!" :
					"do nothing special."))))));
#endif
		}

	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*
 * Set "p_ptr->oppose_acid", notice observable changes
 */
bool set_oppose_acid(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_acid && !do_dec)
		{
			if (p_ptr->oppose_acid > v) return FALSE;
		}
		else if (!IS_OPPOSE_ACID())
		{
#ifdef JP
msg_print("酸への耐性がついた気がする！");
#else
			msg_print("You feel resistant to acid!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_acid && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("酸への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to acid.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_acid = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_elec", notice observable changes
 */
bool set_oppose_elec(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	///mod140216 衣玖はLv30で雷撃二重耐性
	if ((p_ptr->pclass == CLASS_IKU || p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)) v=1;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_elec && !do_dec)
		{
			if (p_ptr->oppose_elec > v) return FALSE;
		}
		else if (!IS_OPPOSE_ELEC())
		{
#ifdef JP
msg_print("電撃への耐性がついた気がする！");
#else
			msg_print("You feel resistant to electricity!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_elec && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("電撃への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to electricity.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_elec = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_fire", notice observable changes
 */
/*:::火炎一時耐性　魔王と高レベルのバルログはここの値が0にならないよう処理されている*/
bool set_oppose_fire(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	///mod140608 お燐はLv35で火炎二重耐性
	if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34)) v=1;
	else if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29)) v=1;
	else if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39)) v=1;
	else if ((p_ptr->pclass == CLASS_CHIYARI) && (p_ptr->lev > 29)) v = 1;
	else if ((p_ptr->pclass == CLASS_ENOKO) && (p_ptr->lev > 39)) v = 1;
	else if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29)) v = 1;
	else if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29)) v = 1;
	else if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29)) v = 1;

	if ( (p_ptr->mimic_form == MIMIC_DEMON)) v = 1;
	//if ((prace_is_(RACE_DEMON) && (p_ptr->lev > 44)) || (p_ptr->mimic_form == MIMIC_DEMON)) v = 1;
	/* Open */
	if (v)
	{
		if (p_ptr->oppose_fire && !do_dec)
		{
			if (p_ptr->oppose_fire > v) return FALSE;
		}
		else if (!IS_OPPOSE_FIRE())
		{
#ifdef JP
msg_print("火への耐性がついた気がする！");
#else
			msg_print("You feel resistant to fire!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_fire && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("火への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to fire.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_fire = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_cold", notice observable changes
 */
bool set_oppose_cold(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	///mod140729 レティはLv20で冷気二重耐性
	if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) v=1;

	if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) v=1;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_cold && !do_dec)
		{
			if (p_ptr->oppose_cold > v) return FALSE;
		}
		else if (!IS_OPPOSE_COLD())
		{
#ifdef JP
msg_print("冷気への耐性がついた気がする！");
#else
			msg_print("You feel resistant to cold!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_cold && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("冷気への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to cold.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_cold = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_pois", notice observable changes
 */
bool set_oppose_pois(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if ((p_ptr->pclass == CLASS_NINJA) && (p_ptr->lev > 44)) v = 1;

	if ((p_ptr->pclass == CLASS_MOMOYO) && (p_ptr->lev > 19)) v = 1;
	if ((p_ptr->pclass == CLASS_YUMA) && (p_ptr->lev > 19)) v = 1;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_pois && !do_dec)
		{
			if (p_ptr->oppose_pois > v) return FALSE;
		}
		else if (!IS_OPPOSE_POIS())
		{
#ifdef JP
msg_print("毒への耐性がついた気がする！");
#else
			msg_print("You feel resistant to poison!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_pois && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("毒への耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to poison.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_pois = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->stun", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_stun(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	//v1.1.66 埴輪などは別関数へ飛ばして破損処理をする
	if (RACE_BREAKABLE) return set_broken(v * 10);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;
	///race class 朦朧への耐性
	if (prace_is_(RACE_GOLEM) || (( p_ptr->pseikaku == SEIKAKU_BERSERK) && (p_ptr->lev > 34))) v = 0;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags3 & RF3_NO_STUN) v=0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->stun = 0;
		return FALSE;
	}


	/* Knocked out */
	if (p_ptr->stun > 100)
	{
		old_aux = 3;
	}

	/* Heavy stun */
	else if (p_ptr->stun > 50)
	{
		old_aux = 2;
	}

	/* Stun */
	else if (p_ptr->stun > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Knocked out */
	if (v > 100)
	{
		new_aux = 3;
	}

	/* Heavy stun */
	else if (v > 50)
	{
		new_aux = 2;
	}

	/* Stun */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		//v1.1.27 結界ガード
		if(v > 50)
		{
			set_mana_shield(FALSE,FALSE);
		}

		/* Describe the state */
		switch (new_aux)
		{
			/* Stun */
			case 1:
#ifdef JP
msg_print("意識がもうろうとしてきた。");
#else
			msg_print("You have been stunned.");
#endif

			break;

			/* Heavy stun */
			case 2:
#ifdef JP
msg_print("意識がひどくもうろうとしてきた。");
#else
			msg_print("You have been heavily stunned.");
#endif

			break;

			/* Knocked out */
			case 3:
#ifdef JP
msg_print("頭がクラクラして意識が遠のいてきた。");
#else
			msg_print("You have been knocked out.");
#endif

			break;
		}

		if (randint1(1000) < v || one_in_(16))
		{
#ifdef JP
msg_print("割れるような頭痛がする。");
#else
			msg_print("A vicious blow hits your head.");
#endif

			if (one_in_(3))
			{
				if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
				if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
			}
			else if (one_in_(2))
			{
				if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
			}
			else
			{
				if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
			}
		}
		if (p_ptr->special_defense & KATA_MASK)
		{
#ifdef JP
			msg_print("型が崩れた。");
#else
			msg_print("Your posture gets loose.");
#endif
			p_ptr->special_defense &= ~(KATA_MASK);
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->redraw |= (PR_STATE);
			p_ptr->redraw |= (PR_STATUS);
			p_ptr->action = ACTION_NONE;
		}

		/* Sniper */
		//if (p_ptr->concent) reset_concentration(TRUE);
		if(CLASS_USE_CONCENT) reset_concentration(TRUE);

		/* Hex */
		if (hex_spelling_any()) stop_hex_spell_all();

		/* Notice */
		notice = TRUE;
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
msg_print("やっと朦朧状態から回復した。");
#else
			msg_print("You are no longer stunned.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->stun = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "stun" */
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->cut", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_cut(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	//v1.1.66 埴輪などは別関数へ飛ばして破損処理をする
	if (RACE_BREAKABLE) return set_broken((v+1)/2);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;
	///race 切り傷への耐性
	if ((RACE_NO_CUT) && !p_ptr->mimic_form)
		v = 0;
	///mod140530 スライムや霧のときは切り傷を受けない
	if(p_ptr->mimic_form == MIMIC_SLIME || p_ptr->mimic_form == MIMIC_MIST) v=0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->cut = 0;
		return FALSE;
	}

	/* Mortal wound */
	if (p_ptr->cut > CUT_7)
	{
		old_aux = 7;
	}

	/* Deep gash */
	else if (p_ptr->cut > CUT_6)
	{
		old_aux = 6;
	}

	/* Severe cut */
	else if (p_ptr->cut > CUT_5)
	{
		old_aux = 5;
	}

	/* Nasty cut */
	else if (p_ptr->cut > CUT_4)
	{
		old_aux = 4;
	}

	/* Bad cut */
	else if (p_ptr->cut > CUT_3)
	{
		old_aux = 3;
	}

	/* Light cut */
	else if (p_ptr->cut > CUT_2)
	{
		old_aux = 2;
	}

	/* Graze */
	else if (p_ptr->cut > CUT_1)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Mortal wound */
	if (v > CUT_7)
	{
		new_aux = 7;
	}

	/* Deep gash */
	else if (v > CUT_6)
	{
		new_aux = 6;
	}

	/* Severe cut */
	else if (v > CUT_5)
	{
		new_aux = 5;
	}

	/* Nasty cut */
	else if (v > CUT_4)
	{
		new_aux = 4;
	}

	/* Bad cut */
	else if (v > CUT_3)
	{
		new_aux = 3;
	}

	/* Light cut */
	else if (v > CUT_2)
	{
		new_aux = 2;
	}

	/* Graze */
	else if (v > CUT_1)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Graze */
			case 1:
#ifdef JP
msg_print("かすり傷を負ってしまった。");
#else
			msg_print("You have been given a graze.");
#endif

			break;

			/* Light cut */
			case 2:
#ifdef JP
msg_print("軽い傷を負ってしまった。");
#else
			msg_print("You have been given a light cut.");
#endif

			break;

			/* Bad cut */
			case 3:
#ifdef JP
msg_print("ひどい傷を負ってしまった。");
#else
			msg_print("You have been given a bad cut.");
#endif

			break;

			/* Nasty cut */
			case 4:
#ifdef JP
msg_print("大変な傷を負ってしまった。");
#else
			msg_print("You have been given a nasty cut.");
#endif

			break;

			/* Severe cut */
			case 5:
#ifdef JP
msg_print("重大な傷を負ってしまった。");
#else
			msg_print("You have been given a severe cut.");
#endif

			break;

			/* Deep gash */
			case 6:
#ifdef JP
msg_print("ひどい深手を負ってしまった。");
#else
			msg_print("You have been given a deep gash.");
#endif

			break;

			/* Mortal wound */
			case 7:
#ifdef JP
msg_print("致命的な傷を負ってしまった。");
#else
			msg_print("You have been given a mortal wound.");
#endif

			break;
		}

		/* Notice */
		notice = TRUE;

		if (randint1(1000) < v || one_in_(16))
		{
			if (!p_ptr->sustain_chr)
			{
#ifdef JP
msg_print("ひどい傷跡が残ってしまった。");
#else
				msg_print("You have been horribly scarred.");
#endif


				do_dec_stat(A_CHR);
			}
		}
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
//msg_format("やっと%s。", p_ptr->prace == RACE_ANDROID ? "怪我が直った" : "出血が止まった");
msg_format("やっと怪我が治った。");

#else
			msg_print("Your wounds have healed.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->cut = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "cut" */
	p_ptr->redraw |= (PR_CUT);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}





//v1.1.66　状態異常「破損」の処理。
//妖怪人形・ゴーレム・埴輪のみこの状態になる。
//タイムカウントとしてそれらの種族のrace_multipur_val[0]を使用する。
//この関数はset_stun()とset_cut()から呼ばれる。
//他の関数とは違い、vの値に設定されるのではなくvの値によって加減算される。
bool set_broken(int v)
{
	int old_val, new_val;
	int old_rank, new_rank;
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	if (!RACE_BREAKABLE)
	{
		return FALSE;
	}


	old_val = p_ptr->race_multipur_val[0];

	new_val = old_val + v;

	if (new_val < 0) new_val = 0;
	if (new_val > BROKEN_MAX) new_val = BROKEN_MAX;

	///mod140530 スライムや霧のときは切り傷を受けない
	if (p_ptr->mimic_form == MIMIC_SLIME || p_ptr->mimic_form == MIMIC_MIST) new_val = 0;


	if (!old_val) old_rank = 0;
	else if (old_val < BROKEN_1) old_rank = 1;
	else if (old_val < BROKEN_2) old_rank = 2;
	else if (old_val < BROKEN_3) old_rank = 3;
	else 			    old_rank = 4;

	if (!new_val) new_rank = 0;
	else if (new_val < BROKEN_1) new_rank = 1;
	else if (new_val < BROKEN_2) new_rank = 2;
	else if (new_val < BROKEN_3) new_rank = 3;
	else 			    new_rank = 4;


	if (old_rank != new_rank)
	{
		switch (new_rank)
		{
		case 0://常態
			msg_print(_("躰の修復が完了した。", "Your body repairs are finished."));
			break;
		case 1://傷
			if (old_rank < new_rank)
				msg_print(_("あなたの肌に大きな傷がついた。", "Your body is getting chipped."));
			else
				msg_print(_("躰の修復が進んでいる。あとはいくらかの傷を残すのみだ。",
                        "Your body repair is in progress. There's only some cracks left."));
			break;

		case 2://亀裂
			if (old_rank < new_rank)
				msg_print(_("あなたの躰に亀裂が走った！", "A large crack runs through your body!"));
			else
				msg_print(_("躰の欠損が塞がった。しかしまだ亀裂だらけだ。",
                            "Your body defects have been fixed. However, it's still cracked."));
			break;

		case 3://破損
			if (old_rank < new_rank)
				msg_print(_("あなたの躰の一部が砕けてしまった！", "Several of your body parts have been shattered!"));
			else
				msg_print(_("胴体の穴が塞がった。しかしまだ四肢がまともに動かない。",
                            "The hole in your torso has been closed. However, your limbs still don't work quite properly."));
			break;

		default://大破
			msg_print(_("あなたの胴体に大穴が開いた！", "There is a large hole on your torso!"));
			break;
		}

		notice = TRUE;

	}

	if (old_rank < new_rank && new_rank >= 2)
	{
		if (p_ptr->special_defense & KATA_MASK)
		{
			msg_print(_("型が崩れた。", "You break your posture."));
			p_ptr->special_defense &= ~(KATA_MASK);
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->redraw |= (PR_STATE);
			p_ptr->redraw |= (PR_STATUS);
			p_ptr->action = ACTION_NONE;
		}
		if (CLASS_USE_CONCENT) reset_concentration(TRUE);



	}


	/* Use the value */
	p_ptr->race_multipur_val[0] = new_val;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	//朦朧ステータスと同じ場所に表示する
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


//v1.1.66 修復
//「破損」状態になった＠が回復しようとする。
bool	self_repair(void)
{
	int repair_val, con_adj;

	if (!RACE_BREAKABLE)
	{
		msg_print(_("ERROR:想定されない種族でself_repair()が呼ばれた",
                    "ERROR: self_repair() called for an unexpected race"));
		return FALSE;
	}
	if (!REF_BROKEN_VAL)
	{
		msg_print(_("修復の必要はない。", "You don't require repair."));
		return FALSE;

	}
	msg_print(_("躰を修復している...", "You start repairing your body..."));

	//耐久10:1 20:3 30:6 40:10 50:16
	con_adj = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

	repair_val = randint1(con_adj * 4) + con_adj * 3;

	set_broken(-(repair_val));

	return TRUE;

}



/*
 * Set "p_ptr->food", notice observable changes
 *
 * The "p_ptr->food" variable can get as large as 20000, allowing the
 * addition of the most "filling" item, Elvish Waybread, which adds
 * 7500 food units, without overflowing the 32767 maximum limit.
 *
 * Perhaps we should disturb the player with various messages,
 * especially messages about hunger status changes.  XXX XXX XXX
 *
 * Digestion of food is handled in "dungeon.c", in which, normally,
 * the player digests about 20 food units per 100 game turns, more
 * when "fast", more when "regenerating", less with "slow digestion",
 * but when the player is "gorged", he digests 100 food units per 10
 * game turns, or a full 1000 food units per 100 game turns.
 *
 * Note that the player's speed is reduced by 10 units while gorged,
 * so if the player eats a single food ration (5000 food units) when
 * full (15000 food units), he will be gorged for (5000/100)*10 = 500
 * game turns, or 500/(100/5) = 25 player turns (if nothing else is
 * affecting the player speed).
 */
/*:::p_ptr->foodの満腹度をvに変更し、空腹状態の遷移でメッセージ表示。
 *:::最大値は20000である。10000で満腹となり15000以上は食べすぎになる。
 *:::2000以下は空腹、1000以下は衰弱、500以下は飢餓(赤文字衰弱)
 *:::100ゲームターンごとに20を消費する。食べすぎなら1000を消費する。
 *:::食べ過ぎのとき加速-10*/
/*:::どの種族がどんな食事でどれくら満腹度が増えるか、などの処理は別箇所*/

bool set_food(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	//v2.0.6
	//尤魔特殊処理
	//食べ過ぎにならない。
	//満腹度を13000*10までストックできる。
	//15000を超えたらストックを1増やして13000マイナスし、
	//2000を下回ったらストックを1減らして13000プラスする。
	//ストックはp_ptr->magic_num2[200]に記録する。
	if ( p_ptr->pclass == CLASS_YUMA)
	{
		//int max_stock = 3 + p_ptr->lev / 7;
		int max_stock = 10;
		int stock = p_ptr->magic_num2[200];

		//宿や特殊効果発動などの「満腹になる」処理が正常に働かなくなるので、
		//この値をセットして呼ばれたときは現在満腹度+10000と見なす。
		if (v == PY_FOOD_MAX - 1)  v = p_ptr->food + 10000;

		if (stock == max_stock && v >= PY_FOOD_MAX)
		{
			v = PY_FOOD_MAX - 1;
		}
		else if (stock < max_stock && v >= PY_FOOD_MAX)
		{
			v -= 13000;
			if (v >= PY_FOOD_MAX) v = PY_FOOD_MAX - 1;
			stock++;
			notice = TRUE;
		}
		else if (stock > 0 && v < PY_FOOD_ALERT)
		{
			v += 13000;
			stock--;
			notice = TRUE;
		}
		else if (stock == 0 && v < 0)
		{
			v = 0;
		}

		p_ptr->magic_num2[200] = stock;
	}
	else
	{

		/* Hack -- Force good values */
		v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	}

	///mod140113 魔法使いは空腹にならないようにした
	if(prace_is_(RACE_MAGICIAN) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
	if(prace_is_(RACE_ULTIMATE) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
	if(prace_is_(RACE_MAGIC_JIZO) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;

	//v1.1.66 埴輪は空腹にも満腹にもならない
	if (prace_is_(RACE_HANIWA))
	{
		if (v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
		if (v >= PY_FOOD_FULL) v = PY_FOOD_FULL - 1;
	}

	//蓬莱人は空腹であまり衰弱しない
	if(p_ptr->prace == RACE_HOURAI && v < PY_FOOD_FAINT) v = PY_FOOD_FAINT;

	//小傘とメディスンは食べ過ぎにならない
	if(p_ptr->pclass == CLASS_KOGASA && v > PY_FOOD_MAX-1) v = PY_FOOD_MAX-1;
	if(p_ptr->pclass == CLASS_MEDICINE && v > PY_FOOD_MAX-1) v = PY_FOOD_MAX-1;
	//幽々子は満腹度3000以上にならない
	if(p_ptr->pclass == CLASS_YUYUKO && v > 3000) v = 3000;

	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		old_aux = 0;
	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
		old_aux = 1;
	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
		old_aux = 2;
	}

	/* Normal */
	//v2.0.6 尤魔は通常の満腹以上にならない
	else if (p_ptr->food < PY_FOOD_FULL || p_ptr->pclass == CLASS_YUMA)
	{
		old_aux = 3;
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
		old_aux = 4;
	}

	/* Gorged */
	else
	{
		old_aux = 5;
	}




	/* Fainting / Starving */
	if (v < PY_FOOD_FAINT)
	{
		new_aux = 0;
	}

	/* Weak */
	else if (v < PY_FOOD_WEAK)
	{
		new_aux = 1;
	}

	/* Hungry */
	else if (v < PY_FOOD_ALERT)
	{
		new_aux = 2;
	}

	/* Normal */
	//v2.0.6 尤魔は通常の満腹以上にならない
	else if (v < PY_FOOD_FULL || p_ptr->pclass == CLASS_YUMA)
	{
		new_aux = 3;
	}

	/* Full */
	else if (v < PY_FOOD_MAX)
	{
		new_aux = 4;
	}

	/* Gorged */
	else
	{
		new_aux = 5;
	}

	///del131221 virtue
	/*
	if (old_aux < 1 && new_aux > 0)
		chg_virtue(V_PATIENCE, 2);
	else if (old_aux < 3 && (old_aux != new_aux))
		chg_virtue(V_PATIENCE, 1);
	if (old_aux == 2)
		chg_virtue(V_TEMPERANCE, 1);
	if (old_aux == 0)
		chg_virtue(V_TEMPERANCE, -1);
	*/
	/* Food increase */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Weak */
			case 1:
#ifdef JP
msg_print("まだ空腹で倒れそうだ。");
#else
			msg_print("You are still weak.");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("まだ空腹だ。");
#else
			msg_print("You are still hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("空腹感がおさまった。");
#else
			msg_print("You are no longer hungry.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("満腹だ！");
#else
			msg_print("You are full!");
#endif

			break;

			/* Bloated */
			case 5:
#ifdef JP
msg_print("食べ過ぎだ！");
#else
			msg_print("You have gorged yourself!");
#endif
			///del131221 virtue
			//chg_virtue(V_HARMONY, -1);
			//chg_virtue(V_PATIENCE, -1);
			//chg_virtue(V_TEMPERANCE, -2);

			break;
		}

		/* Change */
		notice = TRUE;
	}

	/* Food decrease */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Fainting / Starving */
			case 0:
#ifdef JP
msg_print("あまりにも空腹で気を失ってしまった！");
#else
			msg_print("You are getting faint from hunger!");
#endif

			break;

			/* Weak */
			case 1:
#ifdef JP
msg_print("お腹が空いて倒れそうだ。");
#else
			msg_print("You are getting weak from hunger!");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("お腹が空いてきた。");
#else
			msg_print("You are getting hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("満腹感がなくなった。");
#else
			msg_print("You are no longer full.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("やっとお腹がきつくなくなった。");
#else
			msg_print("You are no longer gorged.");
#endif

			break;
		}

		if (p_ptr->wild_mode && (new_aux < 2))
		{
			change_wild_mode();
		}

		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->food = v;


	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw hunger */
	p_ptr->redraw |= (PR_HUNGER);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

///mod140326 泥酔度
/*:::泥酔度のセット*/
bool set_alcohol(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	//勇儀は特別酒に強い
	if(p_ptr->pclass == CLASS_YUGI && v >= DRANK_4) v = DRANK_4 - 1;
	//酒神も酒に強い
	if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[3] == 53 && v >= DRANK_4)		v = DRANK_4 - 1;

	//永琳はさらに酒に強い
	if(p_ptr->pclass == CLASS_EIRIN && v >= DRANK_2) v = DRANK_2 - 1;
	//尤魔も
	if (p_ptr->pclass == CLASS_YUMA && v >= DRANK_2) v = DRANK_2 - 1;

	//性格狂気も
	if(p_ptr->pseikaku == SEIKAKU_BERSERK && v >= DRANK_4) v = DRANK_4 - 1;

	if(CHECK_FAIRY_TYPE == 45 && v >= DRANK_4) v = DRANK_4 - 1;

	if (p_ptr->alcohol == DRANK_0) old_aux = 0;
	else if (p_ptr->alcohol < DRANK_1) old_aux = 1;
	else if (p_ptr->alcohol < DRANK_2) old_aux = 2;
	else if (p_ptr->alcohol < DRANK_3) old_aux = 3;
	else if (p_ptr->alcohol < DRANK_4) old_aux = 4;
	else 	old_aux = 5;

	if (v == DRANK_0) new_aux = 0;
	else if (v < DRANK_1) new_aux = 1;
	else if (v < DRANK_2) new_aux = 2;
	else if (v < DRANK_3) new_aux = 3;
	else if (v < DRANK_4) new_aux = 4;
	else 	new_aux = 5;


	/* Food increase */
	if (new_aux != old_aux)
	{
		if(p_ptr->muta2 & MUT2_ALCOHOL)
		{
			if(new_aux == 0) msg_print(_("酒が切れてしまった。体がいうことを聞かない・・",
                                        "You're sober now. Your body won't listen to you..."));
			else if(new_aux == 1) msg_print(_("なんだか酔い足りない・・",
                                            "You don't feel like you're drunk enough..."));
			else if(new_aux == 2) msg_print(_("いい気持ちだ！", "You feel good!"));
			else if(new_aux == 3) msg_print(_("最高の気分だ！", "You feel great!"));
			else  msg_print(_("比類なき最高の気分だ！", "You feel absolutely amazing!"));
		}
		else
		{
			if(new_aux == 0) msg_print(_("酔いが醒めた。", "You are no longer drunk."));
			else if(new_aux == 1) msg_print(_("少し気分がいい。", "You feel a little better."));
			else if(new_aux == 2) msg_print(_("いい気持ちだ！", "You feel good!"));
			else if(new_aux == 3) msg_print(_("最高の気分だ！", "You feel great!"));
			else if(new_aux == 4) msg_print(_("飲みすぎて気持ちが悪い・・", "You've drunk too much; you don't feel so good..."));
			else
			{
				msg_print(_("あなたはその場に倒れた。", "You collapse on the spot."));
				//v1.1.27 結界ガード
				set_mana_shield(FALSE,FALSE);
			}
		}
		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->alcohol = v;

	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//v1.1.78 酔拳フラグ処理
	check_suiken();

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Redraw hunger */
/*:::Hack 飲み過ぎて倒れたときのみ意識不明瞭を適用する*/
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Increases a stat by one randomized level             -RAK-
 *
 * Note that this function (used by stat potions) now restores
 * the stat BEFORE increasing it.
 */
/*:::基本パラメータ上昇*/
///mod140104 パラメータ上限と表記を変更
bool inc_stat(int stat)
{
	int value, gain;

	/* Then augment the current/max stat */
	value = p_ptr->stat_cur[stat];

	/* Cannot go above 18/100 */
	if (value < p_ptr->stat_max_max[stat])
	{

		if(value > 27) gain = 1;
		else
		{
			if(weird_luck())gain = 3;
			else if(one_in_(4)) gain = 2;
			else gain = 1;
		}
		value += gain;
		if(value > p_ptr->stat_max_max[stat]) value = p_ptr->stat_max_max[stat];

#if 0
		/* Gain one (sometimes two) points */
		if (value < 18)
		{
			gain = ((randint0(100) < 75) ? 1 : 2);
			value += gain;
		}

		/* Gain 1/6 to 1/3 of distance to 18/100 */
		else if (value < (p_ptr->stat_max_max[stat]-2))
		{
			/* Approximate gain value */
			gain = (((p_ptr->stat_max_max[stat]) - value) / 2 + 3) / 2;

			/* Paranoia */
			if (gain < 1) gain = 1;

			/* Apply the bonus */
			value += randint1(gain) + gain / 2;

			/* Maximal value */
			if (value > (p_ptr->stat_max_max[stat]-1)) value = p_ptr->stat_max_max[stat]-1;
		}

		/* Gain one point at a time */
		else
		{
			value++;
		}
#endif
		/* Save the new value */
		p_ptr->stat_cur[stat] = value;

		/* Bring up the maximum too */
		if (value > p_ptr->stat_max[stat])
		{
			p_ptr->stat_max[stat] = value;
		}

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to gain */
	return (FALSE);
}



/*
 * Decreases a stat by an amount indended to vary from 0 to 100 percent.
 *
 * Amount could be a little higher in extreme cases to mangle very high
 * stats from massive assaults.  -CWS
 *
 * Note that "permanent" means that the *given* amount is permanent,
 * not that the new value becomes permanent.  This may not work exactly
 * as expected, due to "weirdness" in the algorithm, but in general,
 * if your stat is already drained, the "max" value will not drop all
 * the way down to the "cur" value.
 */
/*:::能力値を下げる。amountは敵の能力減少打撃で10,ほか様々なことで11～30程度　信じがたいことで50*/
///mod140104 パラメータ上限と表記を変更　18以上のとき少し下がりやすくなっている
bool dec_stat(int stat, int amount, int permanent)
{
	int cur, max, same, res = FALSE;
	bool flag_dec = FALSE;
	/* Acquire current value */
	cur = p_ptr->stat_cur[stat];
	max = p_ptr->stat_max[stat];
	/* Note when the values are identical */
	same = (cur == max);

	if (cur > 3)
	{
		if (amount > 90) cur--;
		if (amount > 50) cur--;
		if (amount > 20) cur--;
		if (amount > 10) cur--;
		else if(one_in_(2))
		{
			cur--;
			flag_dec = TRUE;
		}

		if (cur < 3) cur = 3;
		if (cur != p_ptr->stat_cur[stat]) res = TRUE;
	}
	if (permanent && (max > 3))
	{
		if (amount > 90) max--;
		if (amount > 50) max--;
		if (amount > 20) max--;
		if (amount > 10) max--;
		else if(flag_dec) max--;

		if (same || (max < cur)) max = cur;
		if (max != p_ptr->stat_max[stat]) res = TRUE;
	}
#if 0
	/* Damage "current" value */
	if (cur > 3)
	{
		/* Handle "low" values */
		if (cur <= 18)
		{
			if (amount > 90) cur--;
			if (amount > 50) cur--;
			if (amount > 20) cur--;
			cur--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((cur-18) / 2 + 1) / 2 + 1);

			/* Paranoia */
			if (loss < 1) loss = 1;

			/* Randomize the loss */
			loss = ((randint1(loss) + loss) * amount) / 100;

			/* Maximal loss */
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			cur = cur - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (cur < 18) cur = (amount <= 20) ? 18 : 17;
		}

		/* Prevent illegal values */
		if (cur < 3) cur = 3;

		/* Something happened */
		if (cur != p_ptr->stat_cur[stat]) res = TRUE;
	}

	/* Damage "max" value */
	if (permanent && (max > 3))
	{
		chg_virtue(V_SACRIFICE, 1);
		if (stat == A_WIS || stat == A_INT)
			chg_virtue(V_ENLIGHTEN, -2);

		/* Handle "low" values */
		if (max <= 18)
		{
			if (amount > 90) max--;
			if (amount > 50) max--;
			if (amount > 20) max--;
			max--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((max-18) / 2 + 1) / 2 + 1);
			loss = ((randint1(loss) + loss) * amount) / 100;
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			max = max - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (max < 18) max = (amount <= 20) ? 18 : 17;
		}

		/* Hack -- keep it clean */
		if (same || (max < cur)) max = cur;

		/* Something happened */
		if (max != p_ptr->stat_max[stat]) res = TRUE;
	}
#endif
	/* Apply changes */
	if (res)
	{
		/* Actually set the stat to its new value. */
		p_ptr->stat_cur[stat] = cur;
		p_ptr->stat_max[stat] = max;

		/* Redisplay the stats later */
		p_ptr->redraw |= (PR_STATS);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);
	}

	/* Done */
	return (res);
}


/*
 * Restore a stat.  Return TRUE only if this actually makes a difference.
 */
bool res_stat(int stat)
{
	/* Restore if needed */
	if (p_ptr->stat_cur[stat] != p_ptr->stat_max[stat])
	{
		/* Restore */
		p_ptr->stat_cur[stat] = p_ptr->stat_max[stat];

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Redisplay the stats later */
		p_ptr->redraw |= (PR_STATS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to restore */
	return (FALSE);
}


/*
 * Increase players hit points, notice effects
 */
/*:::HPをnum回復させる。最大は超えない。*/
bool hp_player(int num)
{
	//int vir;
	/*:::「活」の徳によってはHP回復量が上下するらしい*/
	/*
	vir = virtue_number(V_VITALITY);
	if (vir)
	{
		num = num * (p_ptr->virtues[vir - 1] + 1250) / 1250;
	}
	*/
	/* Healing needed */
	if (p_ptr->chp < p_ptr->mhp)
	{
		if ((num > 0) && (p_ptr->chp < (p_ptr->mhp/3)))
			chg_virtue(V_TEMPERANCE, 1);
		/* Gain hitpoints */
		p_ptr->chp += num;

		/* Enforce maximum */
		if (p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
		}

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		//パルスィのHP減少ボーナス
		if(p_ptr->pclass == CLASS_PARSEE) p_ptr->update |= PU_BONUS;

		/* Heal 0-4 */
		if (num < 5)
		{
#ifdef JP
msg_print("少し気分が良くなった。");
#else
			msg_print("You feel a little better.");
#endif

		}

		/* Heal 5-14 */
		else if (num < 15)
		{
#ifdef JP
msg_print("気分が良くなった。");
#else
			msg_print("You feel better.");
#endif

		}

		/* Heal 15-34 */
		else if (num < 35)
		{
#ifdef JP
msg_print("とても気分が良くなった。");
#else
			msg_print("You feel much better.");
#endif

		}

		/* Heal 35+ */
		else
		{
#ifdef JP
msg_print("ひじょうに気分が良くなった。");
#else
			msg_print("You feel very good.");
#endif

		}

		/* Notice */
		return (TRUE);
	}

	/* Ignore */
	return (FALSE);
}


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_pos[] =
{
#ifdef JP
"強く",
#else
	"strong",
#endif

#ifdef JP
"知的に",
#else
	"smart",
#endif

#ifdef JP
"賢く",
#else
	"wise",
#endif

#ifdef JP
"器用に",
#else
	"dextrous",
#endif

#ifdef JP
"健康に",
#else
	"healthy",
#endif

#ifdef JP
"美しく"
#else
	"beautiful"
#endif

};


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_neg[] =
{
#ifdef JP
"弱く",
"無知に",
"愚かに",
"不器用に",
"不健康に",
"醜く"
#else
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
#endif

};


/*
 * Lose a "point"
 */
bool do_dec_stat(int stat)
{
	bool sust = FALSE;

	/* Access the "sustain" */
	switch (stat)
	{
		case A_STR: if (p_ptr->sustain_str) sust = TRUE; break;
		case A_INT: if (p_ptr->sustain_int) sust = TRUE; break;
		case A_WIS: if (p_ptr->sustain_wis) sust = TRUE; break;
		case A_DEX: if (p_ptr->sustain_dex) sust = TRUE; break;
		case A_CON: if (p_ptr->sustain_con) sust = TRUE; break;
		case A_CHR: if (p_ptr->sustain_chr) sust = TRUE; break;
	}

	/* Sustain */
	if (sust && (!ironman_nightmare || randint0(13)))
	{
		/* Message */
#ifdef JP
msg_format("%sなった気がしたが、すぐに元に戻った。",
#else
		msg_format("You feel %s for a moment, but the feeling passes.",
#endif

				desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Attempt to reduce the stat */
	if (dec_stat(stat, 10, (ironman_nightmare && !randint0(13))))
	{
		/* Message */
#ifdef JP
msg_format("ひどく%sなった気がする。", desc_stat_neg[stat]);
#else
		msg_format("You feel very %s.", desc_stat_neg[stat]);
#endif


		/* Notice effect */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restore lost "points" in a stat
 */
/*:::パラメータを回復する*/
bool do_res_stat(int stat)
{
	/* Attempt to increase */
	if (res_stat(stat))
	{
		/* Message */
#ifdef JP
msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Gain a "point" in a stat
 */
/*:::指定したパラメータが上昇する　レベルアップ時など*/
bool do_inc_stat(int stat)
{
	bool res;

	/* Restore strength */
	res = res_stat(stat);

	/* Attempt to increase */
	if (inc_stat(stat))
	{
		///sysdel virtue
		if (stat == A_WIS)
		{
			chg_virtue(V_ENLIGHTEN, 1);
			chg_virtue(V_FAITH, 1);
		}
		else if (stat == A_INT)
		{
			chg_virtue(V_KNOWLEDGE, 1);
			chg_virtue(V_ENLIGHTEN, 1);
		}
		else if (stat == A_CON)
			chg_virtue(V_VITALITY, 1);

		/* Message */
#ifdef JP
msg_format("ワーオ！とても%sなった！", desc_stat_pos[stat]);
#else
		msg_format("Wow!  You feel very %s!", desc_stat_pos[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Restoration worked */
	if (res)
	{
		/* Message */
#ifdef JP
msg_format("元通りに%sなった気がする。", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restores any drained experience
 */
bool restore_level(void)
{
	/* Restore experience */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Message */
#ifdef JP
msg_print("生命力が戻ってきた気がする。");
#else
		msg_print("You feel your life energies returning.");
#endif


		/* Restore the experience */
		p_ptr->exp = p_ptr->max_exp;

		/* Check the experience */
		check_experience();

		/* Did something */
		return (TRUE);
	}

	/* No effect */
	return (FALSE);
}


/*
 * Forget everything
 */
/*:::記憶喪失　詳細未読*/
bool lose_all_info(void)
{
	int i;

	if(p_ptr->pclass == CLASS_KEINE)
	{
		msg_print(_("白沢の血は記憶喪失に抵抗した！",
                    "Your hakutaku blood protects you from memory loss!"));
		return FALSE;
	}

	//chg_virtue(V_KNOWLEDGE, -5);
	//chg_virtue(V_ENLIGHTEN, -5);

	/* Forget info about objects */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Allow "protection" by the MENTAL flag */
		if (o_ptr->ident & (IDENT_MENTAL)) continue;

		/* Remove "default inscriptions" */
		o_ptr->feeling = FEEL_NONE;

		/* Hack -- Clear the "empty" flag */
		o_ptr->ident &= ~(IDENT_EMPTY);

		/* Hack -- Clear the "known" flag */
		o_ptr->ident &= ~(IDENT_KNOWN);

		/* Hack -- Clear the "felt" flag */
		o_ptr->ident &= ~(IDENT_SENSE);
	}
	///mod141101 記憶消去を受けるとさとりと響子は技を忘れる
	if(p_ptr->pclass == CLASS_SATORI) make_magic_list_satori(TRUE);
	kyouko_echo(TRUE,0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Mega-Hack -- Forget the map */
	wiz_dark();

	/* It worked */
	return (TRUE);
}


void do_poly_wounds(void)
{
	/* Changed to always provide at least _some_ healing */
	s16b wounds = p_ptr->cut;
	s16b hit_p = (p_ptr->mhp - p_ptr->chp);
	s16b change = damroll(p_ptr->lev, 5);
	bool Nasty_effect = one_in_(5);

	if (!(wounds || hit_p || Nasty_effect)) return;

#ifdef JP
msg_print("傷がより軽いものに変化した。");
#else
	msg_print("Your wounds are polymorphed into less serious ones.");
#endif

	hp_player(change);
	if (Nasty_effect)
	{
#ifdef JP
msg_print("新たな傷ができた！");
take_hit(DAMAGE_LOSELIFE, change / 2, "変化した傷", -1);
#else
		msg_print("A new wound was created!");
		take_hit(DAMAGE_LOSELIFE, change / 2, "a polymorphed wound", -1);
#endif

		set_cut(change);
	}
	else
	{
		set_cut(p_ptr->cut - (change / 2));
	}
}


/*
 * Change player race
 */
/*:::種族変容*/
void change_race(int new_race, cptr effect_msg)
{
	cptr title = race_info[new_race].title;
	int  old_race = p_ptr->prace;
	int i;

	if((RACE_NEVER_CHANGE) || new_race == RACE_ANDROID || new_race == RACE_STRAYGOD || new_race == RACE_DEITY)
	{
		msg_print(_("ERROR:意図しない種族でchange_raceが呼ばれた",
                    "ERROR: change_race called for an unintended race"));
		return;
	}

	if(new_race == RACE_ZASHIKIWARASHI)
	{
		msg_print(_("ERROR:他種族が座敷わらしに変化することはできない",
                    "ERROR: Other races cannot transform into zashiki-warashi"));
		return;
	}



	//v1.1.69 種族にかかわらず朦朧切り傷破損が治ることにした。v1.1.66での記述を修正
	if (RACE_BREAKABLE)
	{
		set_broken(-(BROKEN_MAX));
	}
	else
	{
		set_cut(0);
		set_stun(0);
	}


#ifdef JP
	if(new_race == RACE_ULTIMATE)
		msg_format("『究極の生命体%sの誕生だッーっ！』", player_name);
	else if(new_race == RACE_LICH)
		msg_format("あなたは本を縁にしてあの世から舞い戻った！");
	else
		msg_format("あなたは%s%sに変化した！", effect_msg, title);
#else
	if(new_race == RACE_ULTIMATE)
		msg_format(_("『究極の生命体%sの誕生だッーっ！』", "'%s, the Ultimate Lifeform is born!'"), player_name);
	else if(new_race == RACE_LICH)
		msg_format(_("あなたは本を縁にしてあの世から舞い戻った！",
                    "You have bound yourself to that book and returned from the netherworld!"));
	else
		msg_format("You turn into %s %s%s!", (!effect_msg[0] && is_a_vowel(title[0]) ? "an" : "a"), effect_msg, title);
#endif

	///sysdel virtue
	//chg_virtue(V_CHANCE, 2);

	if (p_ptr->prace < 32)
	{
		p_ptr->old_race1 |= 1L << p_ptr->prace;
	}
	else
	{
		p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
	}
	p_ptr->prace = new_race;
	rp_ptr = &race_info[p_ptr->prace];

	/* Experience factor */
	p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

	if(EXTRA_MODE) p_ptr->expfact /= 2;

	/*
	 * The speed bonus of Klackons and Sprites are disabled
	 * and the experience penalty is decreased.
	 */
	///race class 種族変容時の加速種族の修行僧EXPボーナス関連
	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && ((p_ptr->prace == RACE_KLACKON) || (p_ptr->prace == RACE_SPRITE)))
	//	p_ptr->expfact -= 15;

	/* Get character's height and weight */
	/*:::身長体重変更*/
	///sysdel 身長体重
	get_height_weight();

	/* Hitdice */
	///sys class 種族変容時のHD計算 スペマス特殊処理
	//v1.1.32 パチュリー特殊処理
	if (p_ptr->pclass == CLASS_SORCERER || is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
		p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
	else
		p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

	do_cmd_rerate(FALSE);
	//生来型変異を再取得　元の変異は基本すべて消える
	gain_perma_mutation();

	//v1.1.66 ゴーレムや埴輪になったとき朦朧と切り傷を治す。
	//これらの種族ではもうset_cutやset_stun経由では値にアクセスしなくなるので直接値を編集する。
	//v1.1.69 　記述を間違えていたので修正して上に移動

	/*:::種族用特殊値をクリア*/
	for(i=0;i<5;i++) p_ptr->race_multipur_val[i] = 0;

	//妖精になったとき特技習得　クイックスタート用保存処理は不要
	if(new_race == RACE_FAIRY)
		get_fairy_type(FALSE);

	//v1.1.70 動物霊になったとき所属勢力「なし」に設定
	if (p_ptr->prace == RACE_ANIMAL_GHOST)
		p_ptr->race_multipur_val[0] = ANIMAL_GHOST_STRIFE_NONE;

	/* The experience level may be modified */
	check_experience();

	///mod140619
	for(i=INVEN_RARM;i<INVEN_TOTAL;i++)
	{
		if(!inventory[i].k_idx) continue;
		if(!wield_check(i,i,0))
		{
			object_type forge, *q_ptr = &forge;
			char obj_name[160];
			object_desc(obj_name, &inventory[i], 0);
			object_copy(q_ptr, &inventory[i]);

			msg_format(_("%sが外れてしまった！", "%s! has come off!"), obj_name);
			inven_item_increase(i, -1);
			inven_item_optimize(i);
			drop_near(q_ptr,0,py,px);
			kamaenaoshi(i);
			calc_android_exp();
			p_ptr->redraw |= (PR_EQUIPPY);
		}
	}


	p_ptr->redraw |= (PR_BASIC);

	p_ptr->update |= (PU_BONUS);

	handle_stuff();

	/* Load an autopick preference file */
	if (old_race != p_ptr->prace) autopick_load_pref(FALSE);

	/* Player's graphic tile may change */
	lite_spot(py, px);
}

///sys race muta 自己変容　性別と種族変更に関して操作する予定
///mod140224 自己変容　種族性別変化はとりあえず無効化
void do_poly_self(void)
{
	int power = p_ptr->lev;

#ifdef JP
msg_print("あなたは変化の訪れを感じた...");
#else
	msg_print("You feel a change coming over you...");
#endif
	///del131214 virtue
	//chg_virtue(V_CHANCE, 1);

	///mod140224 ユニークプレーヤーは種族、性別変容しないようにした
	if ((power > randint0(20)) && one_in_(3) && (p_ptr->prace != RACE_ANDROID) && !(cp_ptr->flag_only_unique))
	{
		char effect_msg[80] = "";
		int new_race;

		/* Some form of racial polymorph... */
		power -= 10;

		if ((power > randint0(5)) && one_in_(4))
		{
			/* sex change */
			power -= 2;

			if (p_ptr->psex == SEX_MALE)
			{
				p_ptr->psex = SEX_FEMALE;
				sp_ptr = &sex_info[p_ptr->psex];
#ifdef JP
sprintf(effect_msg, "女性の");
#else
				sprintf(effect_msg, "female ");
#endif

			}
			else
			{
				p_ptr->psex = SEX_MALE;
				sp_ptr = &sex_info[p_ptr->psex];
#ifdef JP
sprintf(effect_msg, "男性の");
#else
				sprintf(effect_msg, "male ");
#endif

			}
		}

		if ((power > randint0(30)) && one_in_(5))
		{
			int tmp = 0;

			/* Harmful deformity */
			power -= 15;

			while (tmp < 6)
			{
				if (one_in_(2))
				{
					(void)dec_stat(tmp, randint1(6) + 6, one_in_(3));
					power -= 1;
				}
				tmp++;
			}

			/* Deformities are discriminated against! */
			(void)dec_stat(A_CHR, randint1(6), TRUE);

			if (effect_msg[0])
			{
				char tmp_msg[10];
#ifdef JP
				sprintf(tmp_msg,"%s",effect_msg);
				sprintf(effect_msg,"奇形の%s",tmp_msg);
#else
				sprintf(tmp_msg,"%s ",effect_msg);
				sprintf(effect_msg,"deformed %s ",tmp_msg);
#endif

			}
			else
			{
#ifdef JP
				sprintf(effect_msg,"奇形の");
#else
				sprintf(effect_msg,"deformed ");
#endif

			}
		}

		while ((power > randint0(20)) && one_in_(10))
		{
			/* Polymorph into a less mutated form */
			power -= 10;

			if (!lose_mutation(0))
#ifdef JP
msg_print("奇妙なくらい普通になった気がする。");
#else
				msg_print("You feel oddly normal.");
#endif

		}

		//v1.1.79 新種族「座敷わらし」になることはできない
		do
		{
			new_race = randint0(MAX_RACES);
		}
		while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ZASHIKIWARASHI)
			|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special
			|| new_race == RACE_ULTIMATE && !(weird_luck()));

		change_race(new_race, effect_msg);
	}
	if ((power > randint0(30)) && one_in_(6))
	{
		int tmp = 0;

		/* Abomination! */
		power -= 20;

#ifdef JP
//msg_format("%sの構成が変化した！", p_ptr->prace == RACE_ANDROID ? "機械" : "内臓");
msg_format("体内の構成が変化した！");
#else
		msg_print("Your internal organs are rearranged!");
#endif

		while (tmp < 6)
		{
			(void)dec_stat(tmp, randint1(6) + 6, one_in_(3));
			tmp++;
		}
		if (one_in_(6))
		{
#ifdef JP
			msg_print("現在の姿で生きていくのは困難なようだ！");
			take_hit(DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "環境に合わない突然変異", -1);
#else
			msg_print("You find living difficult in your present form!");
			take_hit(DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "a lethal mutation", -1);
#endif

			power -= 10;
		}
	}

	if ((power > randint0(20)) && one_in_(4))
	{
		power -= 10;

		get_max_stats();
		do_cmd_rerate(FALSE);
	}

	while ((power > randint0(15)) && one_in_(3))
	{
		power -= 7;
		(void)gain_random_mutation(0);
	}

	if (power > randint0(5))
	{
		power -= 5;
		do_poly_wounds();
	}

	/* Note: earlier deductions may have left power < 0 already. */
	while (power > 0)
	{
		mutate_player();
		power--;
	}
}


/*
 * Decreases players hit points and sets death flag if necessary
 *
 * XXX XXX XXX Invulnerability needs to be changed into a "shield"
 *
 * XXX XXX XXX Hack -- this function allows the user to save (or quit)
 * the game when he dies, since the "You die." message is shown before
 * setting the player to "dead".
 */
/*:::＠のHPを減少させ、マイナスになったら死亡処理*/
/*:::距離、耐性、弱点によるダメージ増減は既に済んでいる*/
/*:::返り値は目には目などの魔法や落馬処理に使われるらしい*/
//このcptr hit_fromはformat()の結果のアドレスをそのまま渡すとtake_hit()の処理中にformat()の中身が変わって最悪ゲームがクラッシュするそうな。知らなかった。
int take_hit(int damage_type, int damage, cptr hit_from, int monspell)
{
	int old_chp = p_ptr->chp;

	int kaguya_card_num = 0;

	char death_message[1024];
	//v1.1.33b 80→256
	char tmp[256];

	int warning = (p_ptr->mhp * hitpoint_warn / 10);

	/* Paranoia */
	if (p_ptr->is_dead) return 0;

	///mod150610 レーザー系は＠にも通るらしいので無効処理
	if(p_ptr->inside_battle) return 0;

	if (p_ptr->sutemi) damage *= 2;
	if (p_ptr->special_defense & KATA_IAI) damage += (damage + 4) / 5;
	//諏訪子冬眠中　通常無敵だが一部攻撃は通る
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		if(damage_type == DAMAGE_ATTACK) return 0;
		else	set_tim_general(0,TRUE,0,1);
	}
	//一輪特殊
	else if(p_ptr->pclass == CLASS_ICHIRIN && damage_type == DAMAGE_ATTACK && unzan_guard()) return 0;



	/*:::簡単モード？*/
	///sys easymode
	if (easy_band) damage = (damage+1)/2;

	/*:::USELIFE:魔法やレイシャルでHPを消費したとき*/
	if (damage_type != DAMAGE_USELIFE)
	{
		/* Disturb */
		disturb(1, 1);
		if (auto_more)
		{
			now_damaged = TRUE;
		}
	}
	/*:::ラーニング処理*/
	if (monspell >= 0) learn_spell(monspell);

	/* Mega-Hack -- Apply "invulnerability" */
	if ((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		//v1.1.44 うどんげ「イビルアンジュレーション」
		if (evil_undulation(FALSE, TRUE))
		{
			if (damage_type == DAMAGE_FORCE)
			{
				msg_print(_("障壁が切り裂かれた！", "The barrier has been torn!"));
			}
			else
				return 0;
		}
		//v2.0.1 生命爆発の薬
		else if (p_ptr->special_defense & SD_LIFE_EXPLODE)
		{
			if (damage_type == DAMAGE_FORCE)
			{
				msg_print(_("生命力の壁を貫通された！", "Your lifeforce barrier gets penetrated!"));
			}
			else
			{
				//攻撃を受けたあと爆発を起こすフラグ　ここで実行するとバグの原因になりそうなのでprocess_world()で行う
				flag_life_explode = TRUE;

				msg_print(_("生命の力が攻撃を無効化した！", "Lifeforce negates the attack!"));
				p_ptr->special_defense &= ~(SD_LIFE_EXPLODE);
				p_ptr->redraw |= PR_STATUS;
				return 0;
			}
		}

		/*:::切腹は無敵に優先？　無敵貫通処理*/
		if (IS_INVULN() && (damage < 9000))
		{
			/*:::光の剣と死の大鎌？*/
			///mod150201 霊夢の夢想天生は本当に*無敵*にしておく
			if(p_ptr->special_defense & MUSOU_TENSEI)
			{
				return 0;
			}
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				//msg_print("バリアが切り裂かれた！");

				if(YUMA_ULTIMATE_MODE)
					msg_print("無限の胃袋を突き破られた！");
				else
					msg_print("回避不可能の攻撃を受けた！");
#else
				if(YUMA_ULTIMATE_MODE)
					msg_print("Your boundless stomach has been pierced!");
				else
					msg_print("You get hit by an unavoidable attack!");
#endif
			}

			else if (one_in_(PENETRATE_INVULNERABILITY))
			{
#ifdef JP
				if (YUMA_ULTIMATE_MODE)
					msg_print("攻撃の吸収に失敗した！");
				else
					msg_print("攻撃を無効化できなかった！");
#else
				if (YUMA_ULTIMATE_MODE)
					msg_print("You failed to absorb the attack!");
				else
					msg_print("You failed to negate the attack!");
#endif
			}
			/*:::ダメージを受けずに終了*/
			else
			{
				return 0;
			}
		}
		/*:::分身*/
		if (CHECK_MULTISHADOW())
		{
			if (p_ptr->multishadow)
			{
                if (damage_type == DAMAGE_FORCE)
                {
#ifdef JP
                    msg_print("幻影もろとも体が切り裂かれた！");
#else
                    msg_print("The attack hits Shadow together with you!");
#endif
                }
                else if (damage_type == DAMAGE_ATTACK)
                {
#ifdef JP
                    msg_print("攻撃は幻影に命中し、あなたには届かなかった。");
#else
                    msg_print("The attack hits Shadow, you are unharmed!");
#endif
                    return 0;
                }
			}
			//v1.1.92 女苑専用性格 確率で攻撃が紫苑に当たってダメージ回避 multishadowの判定式に寄生
			//受けたダメージをmagic_num1[6]に記録し、process_upkeep_with_speed()で油を撒く処理
			//時々姉がキレる判定が行われ、magic_num1[6]に30000が格納されて大量に油を撒く。そのあと一時的に値がマイナスになって攻撃からかばってくれなくなる
			else if(is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
			{
				if (damage_type == DAMAGE_FORCE)
				{
					msg_print(_("姉もろとも体が切り裂かれた！",
                                "Both your and your sister's bodies get shredded by the attack!"));
				}
				else if(damage_type == DAMAGE_ATTACK && p_ptr->magic_num1[6] >= 0 )
				{
					msg_print(_("攻撃は姉に命中し、あなたには届かなかった。",
                                "The attack hits your sister without reaching you."));
					p_ptr->magic_num1[6] += damage;

					//時々姉がキレる判定
					if (randint1(SHION_OIL_BOMB_LIMIT) < p_ptr->magic_num1[6]) p_ptr->magic_num1[6] = 30000;
					return 0;

				}

			}
			else
			{

				msg_print(_("ERROR:想定外の状況でCHECK_MULTISHADOW()が通っている",
                            "ERROR: CHECK_MULTISHADOW() passed in an unexpected state"));
			}

		}




		/*:::幽体化*/
		if (p_ptr->wraith_form)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("半物質の体が切り裂かれた！");
#else
				msg_print("The attack cuts through your ethereal body!");
#endif
			}
			else
			{
				damage /= 2;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}
		///mod140218 魔法の鎧
		/*:::魔法の鎧の効果変更*/
		if (p_ptr->magicdef)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("魔法の鎧が貫かれた！");
#else
				msg_print("The attack cuts through your magical armor!");
#endif
			}
			else
			{
				damage  = damage * 2 / 3;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}

		//サグメ神々の弾冠
		if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0] && damage_type != DAMAGE_FORCE && damage_type != DAMAGE_NOESCAPE)
		{
			if(damage < p_ptr->csp)
			{
				p_ptr->csp -= damage;
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();
				return 0;
			}
			else
			{
				damage -= p_ptr->csp;
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				set_tim_general(0,TRUE,0,0);
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();

			}

		}


		if(p_ptr->pclass == CLASS_CLOWNPIECE && damage_type != DAMAGE_FORCE)
		{
			int mult = 100 - (p_ptr->stat_ind[A_CON]+3) + 10;

			if(mult < 100) damage = damage * mult / 100;

		}
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			damage /= 2;
			if ((damage == 0) && one_in_(2)) damage = 1;
		}

		//v1.1.27 結界ガード
		check_mana_shield(&damage,damage_type);



	} /* not if LOSELIFE USELIFE */


	/*:::正邪ひらり布*/
	if (p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_HIRARI] && !one_in_(7) &&
		(damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE ))
	{
		p_ptr->magic_num1[SEIJA_ITEM_HIRARI] -= 1;
		if (damage_type == DAMAGE_FORCE)
		{
			msg_print(_("光の剣はひらり布を貫通した！", "The Psycho-Spear cuts through your Nimble Fabric!"));
		}
		else
		{
			msg_print(_("ひらり布で攻撃をかわした！", "You evade the attack with your Nimble Fabric!"));
			return 0;
		}
	}
	//パルスィのダメージカウント アイテムカードに対応するため全クラス対象にした
	if((damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE ))
	{
		 parsee_damage_count += damage;
		 if(parsee_damage_count > 9999) parsee_damage_count = 9999;
	}
	//クラウンピースのダメージカウント
	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		p_ptr->magic_num1[0] += damage;
		p_ptr->update |= PU_HP;
	}

	if(damage) break_eibon_wheel();

	/* Hurt the player */
	p_ptr->chp -= damage;
	/*:::抹殺のときはHP0で止まる？*/
	if(damage_type == DAMAGE_GENO && p_ptr->chp < 0)
	{
		damage += p_ptr->chp;
		p_ptr->chp = 0;
	}

	//v1.1.86 アビリティカード「かぐや姫の隠し箱」によるMP回復
	kaguya_card_num = count_ability_card(ABL_CARD_KAGUYA);
	if (damage_type == DAMAGE_ATTACK && kaguya_card_num && p_ptr->chp>=0)
	{
		int percen = calc_ability_card_prob(ABL_CARD_KAGUYA, kaguya_card_num);
		player_gain_mana(damage * percen / 100);
	}

	/*:::生命領域のリアクティブヒール、変容領域のスライム変化*/
	//v1.1.85 毒沼の中にいるメディスンも追加
	if((damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE || damage_type == DAMAGE_NOESCAPE)	&& damage > 0 && p_ptr->chp >= 0 &&
		((p_ptr->pclass == CLASS_MEDICINE && cave_have_flag_bold(py, px, FF_POISON_PUDDLE) && cave_have_flag_bold(py, px, FF_DEEP))
			|| p_ptr->reactive_heal
//			|| ((CHECK_YUMA_FOOD_STOCK) >= 5)
			|| (p_ptr->mimic_form == MIMIC_SLIME)
			|| (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39)))
	{
		bool regenerator = FALSE;
		int heal = damroll(3,MAX(p_ptr->lev / 4,1));
		if(p_ptr->reactive_heal && (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39)) regenerator = TRUE;
//		if ((CHECK_YUMA_FOOD_STOCK) >= 9) regenerator = TRUE;
		if(regenerator) heal = heal * 3;

		p_ptr->chp +=heal;
		if(regenerator) msg_format(_("受けたばかりの傷がみるみる塞がっていく！",
                                    "The wounds you just received start closingly rapidly!"));
		else msg_format(_("受けたばかりの傷が少し癒えた！",
                            "The wounds you just received have healed a bit!"));
		if(p_ptr->reactive_heal) set_reactive_heal(p_ptr->reactive_heal - randint1(2),TRUE);
		//尤魔リアクティブヒールは満腹度消費
//		if ((CHECK_YUMA_FOOD_STOCK) >= 5)
//		{
//			set_food(p_ptr->food - heal * 10);
//		}
		if(p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;

		}



	}
	if(p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_JIZO] && p_ptr->chp < p_ptr->mhp / 2)
	{
		msg_format(_("身代わり地蔵が砕けた！", "The Substitute Jizo has been shattered!"));
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
		p_ptr->magic_num1[SEIJA_ITEM_JIZO] -= 1;
	}

	/* Display the hitpoints */
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	handle_stuff();

	/*
	if (damage_type != DAMAGE_GENO && p_ptr->chp == 0)
	{
		chg_virtue(V_SACRIFICE, 1);
		chg_virtue(V_CHANCE, 2);
	}
	*/
	/* Dead player */
	if (p_ptr->chp < 0)
	{
		bool android = (p_ptr->prace == RACE_ANDROID ? TRUE : FALSE);

//		if(p_ptr->pclass == CLASS_MOKOU && !p_ptr->inside_arena)
		if (p_ptr->pclass == CLASS_MOKOU)
		{
			if(p_ptr->csp >= 25)
			{
				flag_mokou_resurrection = TRUE;
				return damage;
			}
			else
				msg_print(_("何てこと！体力の限界だ！", "What? Your body is at its limit!"));
		}

		if(p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			msg_print(_("もう力が残っていない…", "You don't have any power left..."));
		}


#ifdef JP       /* 死んだ時に強制終了して死を回避できなくしてみた by Habu */
		/*:::死亡時真っ先に強制セーブ*/
		if (!cheat_save)
			if(!save_player()) msg_print("セーブ失敗！");
#else
        if (!cheat_save)
			if(!save_player()) msg_print("Saving failed!");
#endif

		/* Sound */
		sound(SOUND_DEATH);

		chg_virtue(V_SACRIFICE, 10);

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Note death */
		p_ptr->is_dead = TRUE;

		//アリーナ内で倒れたときの処理　復活処理やフラグ処理はdungeon()内で行う
		if (p_ptr->inside_arena)
		{
			//v1.1.51 新アリーナ実装により書き換え
			if (p_ptr->pclass == CLASS_DOREMY)
			{
				msg_print(_("あなたは危険な夢から緊急避難した。",
                            "You quickly escaped the dangerous dream."));
			}
			else if (one_in_(3))
			{
				if (p_ptr->pclass == CLASS_SAGUME)
					msg_print(_("「.....!!」", "'.....!!'"));
				else
					msg_print(_("はっ！夢だったか", "Aah! That was just a dream."));
			}
			else if (one_in_(2))
			{
				if (p_ptr->pclass == CLASS_SAGUME)
					msg_print(_("あなたは絶叫しかけて慌てて口を抑えつつ目を覚ました。",
                                "You wake up screaming and cover your mouth in a hurry."));
				else
					msg_print(_("あなたは絶叫して目を覚ました。", "You wake up screaming."));
			}
			else
			{
				msg_print(_("ドレミー「貴方の悪夢は私が美味しく頂きました。」",
                            "Doremy 'Thanks, your nightmare was delicious.'"));
			}

			//v1.1.57
			if (record_arena)
			{
				do_cmd_write_nikki(NIKKI_ARENA,-1, hit_from);
			}

#if 0
			cptr m_name = r_name+r_info[arena_info[p_ptr->arena_number].r_idx].name;
#ifdef JP
			msg_format("あなたは%sの前に敗れ去った。", m_name);
#else
			msg_format("You are beaten by %s.", m_name);
#endif
			msg_print(NULL);
			if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, -1 - p_ptr->arena_number, m_name);
#endif
		}
		else if (p_ptr->inside_quest == QUEST_MIYOI) //倒れてもゲームオーバーにならない特殊クエスト
		{

		}
		else
		{
			int q_idx = quest_number(dun_level);
			bool seppuku = streq(hit_from, "Seppuku");
			bool winning_seppuku = p_ptr->total_winner && seppuku;

			//v1.1.58
			play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_GAMEOVER);

#ifdef WORLD_SCORE
			/* Make screen dump */
			screen_dump = make_screen_dump();
#endif

			/* Note cause of death */
			if (seppuku)
			{
				strcpy(p_ptr->died_from, hit_from);
#ifdef JP
				if (!winning_seppuku) strcpy(p_ptr->died_from, "切腹");
#endif
			}
			else
			{
				char dummy[1024];
#ifdef JP
				sprintf(dummy, "%s%s%s", !p_ptr->paralyzed ? "" : p_ptr->free_act ? "彫像状態で" : "麻痺状態で", p_ptr->image ? "幻覚に歪んだ" : "", hit_from);
#else
				sprintf(dummy, "%s%s%s", hit_from, !p_ptr->paralyzed ? "" : p_ptr->free_act ? " while turned into a statue" : " while paralyzed",
                        !p_ptr->image ? "" : p_ptr->paralyzed ? " and hallucinating" : " while hallucinating");
#endif
				my_strcpy(p_ptr->died_from, dummy, sizeof p_ptr->died_from);
			}

			/* No longer a winner */
			p_ptr->total_winner = FALSE;

			if (winning_seppuku)
			{
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "勝利の後切腹した。");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "did Seppuku after the winning.");
#endif
			}
			else
			{
				char buf[20];

				if (p_ptr->inside_arena)
#ifdef JP
					strcpy(buf,"アリーナ");
#else
					strcpy(buf,"in the Arena");
#endif
				else if (!dun_level)
#ifdef JP
					strcpy(buf,"地上");
#else
					strcpy(buf,"on the surface");
#endif
				else if (q_idx && (is_fixed_quest_idx(q_idx) &&
				   //      !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_SERPENT))))
				         !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_YUKARI))))
#ifdef JP
					strcpy(buf,"クエスト");
#else
					strcpy(buf,"in a quest");
#endif
				else
#ifdef JP
					sprintf(buf,"%d階", dun_level);
#else
					sprintf(buf,"level %d", dun_level);
#endif

#ifdef JP
				///sys131209 dead
				//sprintf(tmp, "%sで%sに殺された。", buf, p_ptr->died_from);
				sprintf(tmp, "%sで%sに倒された。", buf, p_ptr->died_from);

#else
				sprintf(tmp, "defeated by %s %s.", p_ptr->died_from, buf);
#endif
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
			}

#ifdef JP
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ゲームオーバー --------");
#else
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");

			flush();

#ifdef JP
			if (get_check_strict("画面を保存しますか？", CHECK_NO_HISTORY))
#else
			if (get_check_strict("Dump the screen? ", CHECK_NO_HISTORY))
#endif
			{
				do_cmd_save_screen();
			}

			flush();

			/* Initialize "last message" buffer */
			if (p_ptr->last_message) string_free(p_ptr->last_message);
			p_ptr->last_message = NULL;

			/* Hack -- Note death */
			if (!last_words)
			{
#ifdef JP
				///msg131209 dead
				//msg_format("あなたは%sました。", android ? "壊れ" : "死に");
				msg_format("  満身創痍！　Game over");

#else
				msg_print("  Grievously wounded!  Game over");
#endif

				msg_print(NULL);
			}
			else
			{
				//死亡時セリフのデフォルトメッセージ
				if (winning_seppuku)
				{
#ifdef JP
					get_rnd_line("seppuku_j.txt", 0, death_message);
#else
					get_rnd_line("seppuku.txt", 0, death_message);
#endif
				}
				else if(p_ptr->prace == RACE_NINJA)
					sprintf(death_message,_("サヨナラ！", "Sayonara!"));
				else
				{
#ifdef JP
					get_rnd_line("death_j.txt", 0, death_message);
#else
					get_rnd_line("death.txt", 0, death_message);
#endif
				}

				do
				{
#ifdef JP
					///sys131209 dead
					//while (!get_string(winning_seppuku ? "辞世の句: " : "断末魔の叫び: ", death_message, 1024)) ;
					while (!get_string(winning_seppuku ? "辞世の句: " : "負けセリフ: ", death_message, 1024)) ;
#else
					while (!get_string("Losing line: ", death_message, 1024)) ;
#endif
				}
#ifdef JP
				while (winning_seppuku && !get_check_strict("よろしいですか？", CHECK_NO_HISTORY));
#else
				while (winning_seppuku && !get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
#endif

				if (death_message[0] == '\0')
				{
#ifdef JP

					///msg131209 dead
					strcpy(death_message, format("  満身創痍！　Game over"));
					//strcpy(death_message, format("あなたは%sました。", android ? "壊れ" : "死に"));
#else
					strcpy(death_message, format("  Grievously wounded!  Game over"));
#endif
				}
				else p_ptr->last_message = string_make(death_message);

#ifdef JP
				if (winning_seppuku)
				{
					int i, len;
					int w = Term->wid;
					int h = Term->hgt;
					int msg_pos_x[9] = {  5,  7,  9, 12,  14,  17,  19,  21, 23};
					int msg_pos_y[9] = {  3,  4,  5,  4,   5,   4,   5,   6,  4};
					cptr str;
					char* str2;

					Term_clear();

					/* 桜散る */
					for (i = 0; i < 40; i++)
						Term_putstr(randint0(w / 2) * 2, randint0(h), 2, TERM_VIOLET, "υ");

					str = death_message;
					if (strncmp(str, "「", 2) == 0) str += 2;

					str2 = my_strstr(str, "」");
					if (str2 != NULL) *str2 = '\0';

					i = 0;
					while (i < 9)
					{
						str2 = my_strstr(str, " ");
						if (str2 == NULL) len = strlen(str);
						else len = str2 - str;

						if (len != 0)
						{
							Term_putstr_v(w * 3 / 4 - 2 - msg_pos_x[i] * 2, msg_pos_y[i], len,
							TERM_WHITE, str);
							if (str2 == NULL) break;
							i++;
						}
						str = str2 + 1;
						if (*str == 0) break;
					}

					/* Hide cursor */
					Term_putstr(w-1, h-1, 1, TERM_WHITE, " ");

					flush();
#ifdef WORLD_SCORE
					/* Make screen dump */
					screen_dump = make_screen_dump();
#endif

					/* Wait a key press */
					(void)inkey();
				}
				else
#endif
					msg_print(death_message);
			}
		}

		/* Dead */
		return damage;
	}

	//パルスィのHP減少ボーナス
	if(p_ptr->pclass == CLASS_PARSEE) p_ptr->update |= PU_BONUS;


	//v1.1.25 ピンチ(50%以下固定)に陥った回数をカウントしてみる
	if(old_chp > p_ptr->mhp / 2 && p_ptr->chp < p_ptr->mhp / 2)
	{
		score_count_pinch++;
	}
	//v1.1.25 大ピンチ(10%以下固定)に陥った回数もカウントしてみる
	if(old_chp > p_ptr->mhp / 10 && p_ptr->chp < p_ptr->mhp / 10)
	{
		score_count_pinch2++;
	}

	/* Hitpoint warning */
	if (p_ptr->chp < warning)
	{
		/* Hack -- bell on first notice */
		if (old_chp > warning) bell();

		sound(SOUND_WARN);


		if (record_danger && (old_chp > warning))
		{
			if (p_ptr->image && damage_type == DAMAGE_ATTACK)
#ifdef JP
				hit_from = "何か";
#else
				hit_from = "something";
#endif

#ifdef JP
			sprintf(tmp,"%sによってピンチに陥った。",hit_from);
#else
			sprintf(tmp,"A critical situation because of %s.",hit_from);
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
		}

		if (auto_more)
		{
			/* stop auto_more even if DAMAGE_USELIFE */
			now_damaged = TRUE;
		}

		/* Message */
#ifdef JP
msg_print("*** 警告:低ヒット・ポイント！ ***");
#else
		msg_print("*** LOW HITPOINT WARNING! ***");
#endif

		msg_print(NULL);
		flush();
	}
	if (p_ptr->wild_mode && !p_ptr->leaving && (p_ptr->chp < MAX(warning, p_ptr->mhp/5)))
	{
		change_wild_mode();
	}
	return damage;
}


/*
 * Gain experience
 */
/*:::経験値増加処理*/
void gain_exp_64(s32b amount, u32b amount_frac)
{
	if (p_ptr->is_dead) return;

	if (p_ptr->prace == RACE_ANDROID) return;

	//v1.1.51 新アリーナでは経験値を得られない
	if (p_ptr->inside_arena) return;

	/*:::雛の厄落とし*/
	if(p_ptr->pclass == CLASS_HINA)
		hina_yakuotoshi((int)amount);

	/* Gain some experience */
	s64b_add(&(p_ptr->exp), &(p_ptr->exp_frac), amount, amount_frac);

	/* Slowly recover from experience drainage */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Gain max experience (20%) (was 10%) */
		p_ptr->max_exp += amount / 5;
	}

	/* Check Experience */
	check_experience();
}


/*
 * Gain experience
 */
void gain_exp(s32b amount)
{
	gain_exp_64(amount, 0L);
}

/*:::アンドロイド経験値計算*/
///item
void calc_android_exp(void)
{
	int i;
	u32b total_exp = 0;
	if (p_ptr->is_dead) return;

	if (p_ptr->prace != RACE_ANDROID) return;

	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];
		object_type forge;
		object_type *q_ptr = &forge;
		u32b value, exp;
		int level = MAX(k_info[o_ptr->k_idx].level - 8, 1);

		if ((i == INVEN_RIGHT) || (i == INVEN_LEFT) || (i == INVEN_NECK) || (i == INVEN_LITE)) continue;
		if (!o_ptr->k_idx) continue;

		/* Wipe the object */
		object_wipe(q_ptr);

		object_copy(q_ptr, o_ptr);
		q_ptr->discount = 0;
		q_ptr->curse_flags = 0L;

		if (object_is_fixed_artifact(o_ptr))
		{
			level = (level + MAX(a_info[o_ptr->name1].level - 8, 5)) / 2;
			level += MIN(20, a_info[o_ptr->name1].rarity/(a_info[o_ptr->name1].gen_flags & TRG_INSTA_ART ? 10 : 3));
		}
		else if (object_is_ego(o_ptr))
		{
			level += MAX(3, (e_info[o_ptr->name2].rating - 5)/2);
		}
		else if (o_ptr->art_name)
		{
			s32b total_flags = flag_cost(o_ptr, o_ptr->pval,FALSE);
			int fake_level;

			if (!object_is_weapon_ammo(o_ptr))
			{
				/* For armors */
				if (total_flags < 15000) fake_level = 10;
				else if (total_flags < 35000) fake_level = 25;
				else fake_level = 40;
			}
			else
			{
				/* For weapons */
				if (total_flags < 20000) fake_level = 10;
				else if (total_flags < 45000) fake_level = 25;
				else fake_level = 40;
			}

			level = MAX(level, (level + MAX(fake_level - 8, 5)) / 2 + 3);
		}

		value = object_value_real(q_ptr);
		///mod131223 アンドロ経験値計算　カード削除など
		if (value <= 0) continue;
		//if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI) && (p_ptr->pseikaku != SEIKAKU_SEXY)) value /= 32;

		if (value > 5000000L) value = 5000000L;
		if (o_ptr->tval == TV_DRAG_ARMOR) level /= 2;
		///item 特定の高級ベースはアンドロ経験値にあまり高く寄与しないようにする
		if (object_is_artifact(o_ptr) || object_is_ego(o_ptr) ||
		    (o_ptr->tval == TV_DRAG_ARMOR) ||
		    ((o_ptr->tval == TV_HEAD) && (o_ptr->sval == SV_HEAD_DRAGONHELM)) ||
		    ((o_ptr->tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD)) ||
		    ((o_ptr->tval == TV_GLOVES) && (o_ptr->sval == SV_HAND_DRAGONGLOVES)) ||
		    ((o_ptr->tval == TV_BOOTS) && (o_ptr->sval == SV_LEG_DRAGONBOOTS))
			|| ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_WEAPON_HIHIIROKANE)))

		{
			if (level > 65) level = 35 + (level - 65) / 5;
			else if (level > 35) level = 25 + (level - 35) / 3;
			else if (level > 15) level = 15 + (level - 15) / 2;
			exp = MIN(100000L, value) * level * level / 2;
			if (value > 100000L)
				exp += (value - 100000L) * level * level / 8;
		}
		else
		{
			exp = MIN(100000L, value) * level;
			if (value > 100000L)
				exp += (value - 100000L) * level / 4;
		}
		//buki_motteruka()だと特殊変身時に判定通らなくて経験値が/16のほうになるので修正
		//if ((((i == INVEN_RARM) || (i == INVEN_LARM)) && (buki_motteruka(i))) || (i == INVEN_RIBBON)) total_exp += exp / 48;
		if ((((i == INVEN_RARM) || (i == INVEN_LARM)) && (object_is_melee_weapon(o_ptr))) || (i == INVEN_RIBBON)) total_exp += exp / 48;
		else total_exp += exp / 16;
		if (i == INVEN_BODY) total_exp += exp / 32;
	}
	p_ptr->exp = p_ptr->max_exp = total_exp;

	/* Check Experience */
	check_experience();
}


/*
 * Lose experience
 */
void lose_exp(s32b amount)
{
	if (p_ptr->prace == RACE_ANDROID) return;

	/* Never drop below zero experience */
	if (amount > p_ptr->exp) amount = p_ptr->exp;

	/* Lose some experience */
	p_ptr->exp -= amount;

	/* Check Experience */
	check_experience();
}


/*
 * Drain experience
 * If resisted to draining, return FALSE
 */
/*:::経験値吸収処理　*/
///mod131228 res 経験値吸収を地獄耐性で判定するようにした
bool drain_exp(s32b drain, s32b slip, int hold_life_prob)
{
	/* Androids and their mimics are never drained */
	if (p_ptr->prace == RACE_ANDROID) return FALSE;

	if (p_ptr->resist_neth && (randint0(100) < hold_life_prob))
	//if (p_ptr->hold_life && (randint0(100) < hold_life_prob))
	{
		/* Hold experience */
#ifdef JP
		msg_print("しかし自己の生命力を守りきった！");
#else
		msg_print("You keep hold of your life force!");
#endif
		return FALSE;
	}

	/* Hold experience failed */
	//if (p_ptr->hold_life)
	if (p_ptr->resist_neth)
	{
#ifdef JP
		msg_print("生命力を少し吸い取られた気がする！");
#else
		msg_print("You feel your life slipping away!");
#endif
		lose_exp(slip);
	}
	else
	{
#ifdef JP
		msg_print("生命力が体から吸い取られた気がする！");
#else
		msg_print("You feel your life draining away!");
#endif
		lose_exp(drain);
	}

	return TRUE;
}


bool set_ultimate_res(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->ult_res && !do_dec)
		{
			if (p_ptr->ult_res > v) return FALSE;
		}
		else if (!p_ptr->ult_res)
		{
#ifdef JP
msg_print("あらゆることに対して耐性がついた気がする！");
#else
			msg_print("You feel resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->ult_res)
		{
#ifdef JP
msg_print("あらゆることに対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->ult_res = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



///mod140420 耐暗黒
bool set_tim_res_dark(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_dark && !do_dec)
		{
			if (p_ptr->tim_res_dark > v) return FALSE;
		}
		else if (!p_ptr->tim_res_dark)
		{
#ifdef JP
msg_print("暗黒の力に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to darkness!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_dark)
		{
#ifdef JP
msg_print("暗黒の力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to darkness.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_dark = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_nether(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_nether && !do_dec)
		{
			if (p_ptr->tim_res_nether > v) return FALSE;
		}
		else if (!p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("地獄の力に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to nether!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("地獄の力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to nether.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_nether = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



bool set_tim_res_insanity(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_insanity && !do_dec)
		{
			if (p_ptr->tim_res_insanity > v) return FALSE;
		}
		else if (!p_ptr->tim_res_insanity)
		{
#ifdef JP
msg_print("異界の悍ましい恐怖に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to otherworldly horrors!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_insanity)
		{
#ifdef JP
msg_print("異界の悍ましい恐怖に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to otherworldly horrors.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_insanity = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_chaos(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_chaos && !do_dec)
		{
			if (p_ptr->tim_res_chaos > v) return FALSE;
		}
		else if (!p_ptr->tim_res_chaos)
		{
#ifdef JP
msg_print("カオスの力に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to chaos!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_chaos)
		{
#ifdef JP
msg_print("カオスの力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to chaos.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_chaos = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::レーダーセンス*/
bool set_radar_sense(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->radar_sense && !do_dec)
		{
			if (p_ptr->radar_sense > v) return FALSE;
		}
		else if (!p_ptr->radar_sense)
		{
#ifdef JP
msg_print("周囲の全てを知覚できるようになった！");
#else
			msg_print("You start sharply perceiving your surroundings!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->radar_sense)
		{
#ifdef JP
msg_print("普段通りの知覚力に戻った気がする。");
#else
			msg_print("You perception returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->radar_sense = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_res_time(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_time && !do_dec)
		{
			if (p_ptr->tim_res_time > v) return FALSE;
		}
		else if (!p_ptr->tim_res_time)
		{
#ifdef JP
			///msg131221
msg_print("時空の力に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to time and space!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_time)
		{
#ifdef JP
msg_print("時空の力に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to time and space.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_time = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_reactive_heal(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->reactive_heal && !do_dec)
		{
			if (p_ptr->reactive_heal > v) return FALSE;
		}
		else if (!p_ptr->reactive_heal)
		{
#ifdef JP
msg_print("あなたは敵からの攻撃に備えた。");
#else
			msg_print("You prepare for enemy attacks.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->reactive_heal)
		{
#ifdef JP
msg_print("反応回復の備えが消えた気がする。");
#else
			msg_print("You are no longer prepared to reactively heal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->reactive_heal = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::変容領域　爪強化*/
bool set_clawextend(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->clawextend && !do_dec)
		{
			if (p_ptr->clawextend > v) return FALSE;
		}
		else if (!p_ptr->clawextend)
		{
#ifdef JP
			msg_print("あなたの爪は長く伸び、鋭く研ぎ澄まされた。");
#else
			msg_print("You grow long, sharp claws!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->clawextend)
		{
#ifdef JP
			msg_print("爪が抜け落ちた。");
#else
			msg_print("Your claws fall out.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->clawextend = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_water(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_water && !do_dec)
		{
			if (p_ptr->tim_res_water > v) return FALSE;
		}
		else if (!p_ptr->tim_res_water)
		{
#ifdef JP
msg_print("水に対して耐性がついた気がする！");
#else
			msg_print("You feel resistant to water!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_water)
		{
#ifdef JP
msg_print("水に対する耐性が薄れた気がする。");
#else
			msg_print("You feel less resistant to water.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_water = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_speedster(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_speedster && !do_dec)
		{
			if (p_ptr->tim_speedster > v) return FALSE;
		}
		else if (!p_ptr->tim_speedster)
		{
#ifdef JP
msg_print("高速で移動できるようになった！");
#else
			msg_print("You start moving swiftly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_speedster)
		{
#ifdef JP
msg_print("足が遅くなった気がする。");
#else
			msg_print("Your movements feel less swift.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_speedster = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_lucky(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->lucky && !do_dec)
		{
			if (p_ptr->lucky > v) return FALSE;
		}
		else if (!p_ptr->lucky)
		{
#ifdef JP
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("幸運がやってくる気がする！");
			else
				msg_print("幸運の仕組みがわかった気がする！");
#else
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("You feel like your luck is turning!");
			else
				msg_print("You gain understanding of how luck works!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->lucky)
		{
#ifdef JP
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("運気が普段通りに戻った。");
			else
				msg_print("幸運とは何だったのかわからなくなった。");
#else
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("Your luck returns to normal.");
			else
				msg_print("You no longer understand how luck works.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->lucky = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Choose a warrior-mage elemental attack. -LM-
 */
bool choose_ele_attack(void)
{
	int num;

	char choice;

	if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
	{
#ifdef JP
		msg_format("武器を持たないと魔法剣は使えない。");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

	num = (p_ptr->lev - 20) / 5;

#ifdef JP
		      c_prt(TERM_RED,    "        a) 焼棄", 2, 14);
#else
		      c_prt(TERM_RED,    "        a) Burning", 2, 14);
#endif

#ifdef JP
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) 凍結", 3, 14);
#else
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) Freezing", 3, 14);
#endif
	else prt("", 3, 14);

#ifdef JP
	if (num >= 3) c_prt(TERM_GREEN,  "        c) 毒殺", 4, 14);
#else
	if (num >= 3) c_prt(TERM_GREEN,  "        c) Venomous", 4, 14);
#endif
	else prt("", 4, 14);

#ifdef JP
	if (num >= 4) c_prt(TERM_L_DARK, "        d) 溶解", 5, 14);
#else
	if (num >= 4) c_prt(TERM_L_DARK, "        d) Melting", 5, 14);
#endif
	else prt("", 5, 14);

#ifdef JP
	if (num >= 5) c_prt(TERM_BLUE,   "        e) 電撃", 6, 14);
#else
	if (num >= 5) c_prt(TERM_BLUE,   "        e) Shocking", 6, 14);
#endif
	else prt("", 6, 14);

	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        どの元素攻撃をしますか？", 1, 14);
#else
	prt("        Choose a temporary elemental brand ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A'))
		set_ele_attack(ATTACK_FIRE, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'b') || (choice == 'B')) && (num >= 2))
		set_ele_attack(ATTACK_COLD, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'c') || (choice == 'C')) && (num >= 3))
		set_ele_attack(ATTACK_POIS, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'd') || (choice == 'D')) && (num >= 4))
		set_ele_attack(ATTACK_ACID, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'e') || (choice == 'E')) && (num >= 5))
		set_ele_attack(ATTACK_ELEC, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else
	{
#ifdef JP
		msg_print("魔法剣を使うのをやめた。");
#else
		msg_print("You cancel the temporary branding.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}


/*
 * Choose a elemental immune. -LM-
 */
bool choose_ele_immune(int v)
{
	char choice;

	/* Save screen */
	screen_save();

#ifdef JP
	c_prt(TERM_RED,    "        a) 火炎", 2, 14);
#else
	c_prt(TERM_RED,    "        a) Immune Fire", 2, 14);
#endif

#ifdef JP
	c_prt(TERM_L_WHITE,"        b) 冷気", 3, 14);
#else
	c_prt(TERM_L_WHITE,"        b) Immune Cold", 3, 14);
#endif

#ifdef JP
	c_prt(TERM_L_DARK, "        c) 酸", 4, 14);
#else
	c_prt(TERM_L_DARK, "        c) Immune Acid", 4, 14);
#endif

#ifdef JP
	c_prt(TERM_BLUE,   "        d) 電撃", 5, 14);
#else
	c_prt(TERM_BLUE,   "        d) Immune Elec", 5, 14);
#endif


	prt("", 6, 14);
	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        どの元素の免疫をつけますか？", 1, 14);
#else
	prt("        Choose a temporary elemental immunity ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A'))
		set_ele_immune(DEFENSE_FIRE, v);
	else if ((choice == 'b') || (choice == 'B'))
		set_ele_immune(DEFENSE_COLD, v);
	else if ((choice == 'c') || (choice == 'C'))
		set_ele_immune(DEFENSE_ACID, v);
	else if ((choice == 'd') || (choice == 'D'))
		set_ele_immune(DEFENSE_ELEC, v);
	else
	{
#ifdef JP
		msg_print("免疫を付けるのをやめた。");
#else
		msg_print("You cancel the temporary immunity.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}


/*:::職業ごと汎用カウント reset_tim_flag()でリセットするが魔力消去では消えない*/
/*:::ind:p_ptr->tim_general[]の添字　num:何か数字を渡したいときに使う*/
/*:::tim_general[0]: 小町の無間の狭間、メイドのティータイム、*/
bool set_tim_general(int v, bool do_dec, int ind, int num)
{
	bool notice = FALSE;

	if (cheat_xtra && !do_dec)
		msg_format("set_tim_general time:%d ind:%d num:%d", v, ind, num);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;
	if(ind >= TIM_GENERAL_MAX || ind < 0)
	{
		msg_format(_("ERROR:tim_general()が範囲外のindex(%d)で呼ばれた", "ERROR: tim_general() called with out-of-range index, %d"), ind);
		return (FALSE);
	}

	if (p_ptr->is_dead) return FALSE;

	/*:::小町,霊夢,レミリア,雛特殊処理　無間の狭間に囚われたモンスターが死んだら効果が消える*/
	//v1.1.95 移動禁止処理をMTIMED2_NO_MOVEに統合したので処理削除
//	if(p_ptr->pclass == CLASS_KOMACHI && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_REIMU && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_REMY && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_HINA && ind == 0 && !m_list[p_ptr->magic_num1[2]].r_idx) v=0;

	//マミゾウが変身させているモンスター　倒したら効果が消える
	if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;


	/* Open */
	if (v)
	{
		if (p_ptr->tim_general[ind] && !do_dec)
		{
			if (p_ptr->tim_general[ind] > v) return FALSE;
		}
		else if (!p_ptr->tim_general[ind])
		{
			if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES)) ;
			//if(p_ptr->pclass == CLASS_KOMACHI && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_HINA && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_REIMU && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_REMY && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MAID && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MARISA && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST ) ;
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU && ind == 0);
			else if(USE_NAMELESS_ARTS)
			{
				//v1.1.56
				msg_print(msg_tim_nameless_arts(num, 0, TRUE));
				if(num == JKF1_SELF_LIGHT) p_ptr->update |= PU_TORCH; //能力「発光」用
			}
			else if(p_ptr->pclass == CLASS_BYAKUREN && ind == 0)
#ifdef JP
				msg_format("体に力があふれてくる！");
#else
                msg_format("Power flows through your body!");
#endif
			else if(p_ptr->pclass == CLASS_YOUMU && ind == 0)
#ifdef JP
				msg_format("戦いの化身になった気がする！");
#else
                msg_format("You feel like an avatar of combat!");
#endif
			else if(p_ptr->pclass == CLASS_SUWAKO && ind == 0)
#ifdef JP
				msg_format("あなたは帽子をその場に残して地面に潜った・・");
#else
                msg_format("You sink into the ground, leaving your hat behind.");
#endif
			else if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
#ifdef JP
				msg_format("あなたは濃密な闇を生み出し身にまとった！");
#else
                msg_format("You envelop yourself in a sphere of dense darkness!");
#endif
			else if(p_ptr->pclass == CLASS_HECATIA && ind == 0)
#ifdef JP
				msg_format("あなたは障壁を展開した。");
#else
                msg_format("You unfold a barrier.");
#endif
			else if (p_ptr->pclass == CLASS_IKU && ind == 0)
				msg_format(_("あなたは敵の攻撃に対して身構えた。", "You prepare for enemy attacks."));
			else if (p_ptr->pclass == CLASS_KOKORO && ind == 0)
				msg_format(_("恐ろしい形相の面があなたの周囲に浮遊した。", "Terrifying masks start floating around you."));
			else if(p_ptr->pclass == CLASS_YUKARI)
			{
				; //メッセージなし
			}
			else if (p_ptr->pclass == CLASS_OKINA)
			{
#ifdef JP
				if(p_ptr->stat_ind[A_CHR] < 3)
					msg_print("あなたは冒涜的に身をくねらせ始めた...");
				else if (p_ptr->stat_ind[A_CHR] < 13)
					msg_print("普段より動きが鈍くなった気がする。");
				else if (p_ptr->stat_ind[A_CHR] < 23)
					msg_print("あなたは効率よくキビキビ動き始めた。");
				else if (p_ptr->stat_ind[A_CHR] < 33)
					msg_print("あなたは舞うように流麗に動き始めた。");
				else
					msg_print("あなたの所作は洗練を極めた！");
#else
                if(p_ptr->stat_ind[A_CHR] < 3)
					msg_print("You sway unsightily...");
				else if (p_ptr->stat_ind[A_CHR] < 13)
					msg_print("You feel like you're moving slower than usual.");
				else if (p_ptr->stat_ind[A_CHR] < 23)
					msg_print("You move briskly and efficiently.");
				else if (p_ptr->stat_ind[A_CHR] < 33)
					msg_print("You move as gracefully as dancing.");
				else
					msg_print("Your movement is extremely refined!");
#endif
			}
			else if(p_ptr->pclass == CLASS_RINGO);
			else if(p_ptr->pclass == CLASS_SAGUME)
#ifdef JP
				msg_format("無数の輝く球体があなたを取り囲んだ！");
#else
                msg_format("You are surrounded by countless glowing spheres!");
#endif
			else if (p_ptr->pclass == CLASS_JYOON)
#ifdef JP
				msg_format("あなたは財を持つものの運気を奪い始めた...");
#else
                msg_format("You begin to steal fortune from the wealthy...");
#endif
			else if (p_ptr->pclass == CLASS_PARSEE)
#ifdef JP
				msg_format("あなたの周囲が嫉妬と不和の雰囲気に満ちた...");
#else
                msg_format("Jealousy and discord permeate your surroundings...");
#endif
			else if (p_ptr->pclass == CLASS_SHION)
			{
				switch (randint1(3))
				{
#ifdef JP
				case 1:
					msg_format("あなたは全人類の不幸の体現と化した！");
					break;
				case 2:
					msg_format("無尽蔵の負のオーラがダンジョンを覆い尽くした！");
					break;
				default:
					msg_format("溜まりに溜まった不運パワーが爆発した！");
					break;
#else
                case 1:
					msg_format("You become the embodiment of misfortune of all mankind!");
					break;
				case 2:
					msg_format("An inexhaustible negative aura fills the dungeon!");
					break;
				default:
					msg_format("Your accumulated power of misfortune bursts forth!");
					break;
#endif
				}
				//マップを再描画する
				p_ptr->redraw |= (PR_MAP);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
			else if (p_ptr->pclass == CLASS_URUMI)
			{
#ifdef JP
				msg_format("あなたは手元に力を込めた。ずっしりとした重みを感じる...");
#else
                msg_format("You gather power in your hands. You feel a heavy weight...");
#endif
			}
			else if (p_ptr->pclass == CLASS_YUYUKO) //v1.1.76
			{
#ifdef JP
				msg_format("周囲の者を自らの方へ差し招くような妖しい光を放った...");
#else
                msg_format("You emit a bewitching light beckoning others towards you...");
#endif
			}
			else if (p_ptr->pclass == CLASS_TAKANE)
			{
#ifdef JP
				msg_format("あなたは静かに動き始めた...");
#else
                msg_format("You start moving silently...");
#endif
			}
			else if (p_ptr->pclass == CLASS_MOMOYO)
			{
#ifdef JP
				if (ind == 0) msg_print("あなたは猛烈な速度で壁を掘り始めた！");
				else if (ind == 1) msg_print("蛇を食べたい気分だ！");
				else if (ind == 2) msg_print("龍を食べたい気分だ！");
				else msg_print("全てを食い尽くして力を得たい気分だ！");
#else
                if (ind == 0) msg_print("You start digging walls at blazing speed!");
				else if (ind == 1) msg_print("You feel like eating a snake!");
				else if (ind == 2) msg_print("You feel like eating a dragon!");
				else msg_print("You feel like eating everything to gain strength!");
#endif
			}
			else if (p_ptr->pclass == CLASS_MIYOI)
			{
				msg_format(_("あなたはそっとその場から去ろうと試みた...",
                            "You attempt to make a quiet escape..."));

			}
			else if (p_ptr->pclass == CLASS_ZANMU)
			{
				msg_format(_("あなたは無いはずのものに対し不思議な力を行使した。",
                            "You use a mysterious power over that which does not exist."));
			}
			else if (p_ptr->pclass == CLASS_BEEKEEPER)
			{
				msg_format(_("あなたは大量の蜂に護衛と警戒を命じた。",
                            "You order a large amount of bees to defend you and keep watch."));
			}

			else
			{
				msg_format(_("ERROR:tim_general[%d]が定義されていない呼ばれ方をした、もしくはメッセージが未定義", "ERROR: tim_general[%d] is undefined"), ind);
			}
			notice = TRUE;
		}
		//鈴瑚の強化 累積する
		if(p_ptr->pclass == CLASS_RINGO && !do_dec && ind == 0)
		{
			p_ptr->magic_num1[1]++;
		}
		if(p_ptr->pclass == CLASS_RINGO && !do_dec && ind == 1)
		{
#ifdef JP
			msg_format("%s属性攻撃への免疫を獲得した！",gf_desc_name[num]);
#else
            msg_format("You acquire immunity to %s attacks!",gf_desc_name[num]);
#endif
			p_ptr->magic_num1[3] = num;
			p_ptr->redraw |= (PR_STATUS);

		}

	}

	/* Shut */
	else
	{
		if (p_ptr->tim_general[ind])
		{
			if (USE_NAMELESS_ARTS)
			{
				//v1.1.56
				msg_print(msg_tim_nameless_arts(0, ind, FALSE));

				p_ptr->update |= PU_TORCH; //能力「発光」用
			}
			/*
			else if(p_ptr->pclass == CLASS_KOMACHI && ind == 0)
			{
#ifdef JP
				msg_format("狭間が消えた。");
#else
                msg_format("The confines disappear.");
#endif
				p_ptr->magic_num1[0] = 0;
			}
			else if(p_ptr->pclass == CLASS_HINA && ind == 0)
			{
#ifdef JP
				msg_format("敵を捕らえていた厄が消えた。");
#else
                msg_format("Misfortune ailing your enemies vanishes.");
#endif
				p_ptr->magic_num1[2] = 0;
			}
			else if(p_ptr->pclass == CLASS_REIMU && ind == 0)
			{
#ifdef JP
				msg_format("陣が消えた。");
#else
                msg_format("Your magical circle fades.");
#endif
				p_ptr->magic_num1[0] = 0;
			}
			else if(p_ptr->pclass == CLASS_REMY && ind == 0)
			{
#ifdef JP
				msg_format("鎖が消えた。");
#else
                msg_format("Your chains disappear.");
#endif
				p_ptr->magic_num1[0] = 0;
			}
			*/
			else if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
			{
#ifdef JP
				msg_format("闇が消えた。");
#else
                msg_format("Your darkness fades.");
#endif
			}
			else if(p_ptr->pclass == CLASS_SAGUME && ind == 0)
			{
#ifdef JP
				msg_format("弾冠が消えた。");
#else
                msg_format("Your bullet crown disappears.");
#endif
			}
			else if(p_ptr->pclass == CLASS_YOUMU && ind == 0)
			{
#ifdef JP
				msg_format("猛る心が静まった。");
#else
                msg_format("Your ferocity fades.");
#endif
			}
			else if(p_ptr->pclass == CLASS_HECATIA && ind == 0)
			{
#ifdef JP
				msg_format("障壁が消えた。");
#else
                msg_format("Your barrier disappears.");
#endif
			}
			else if(p_ptr->pclass == CLASS_MARISA && ind == 0)
			{
#ifdef JP
				msg_format("マジックアブソーバーの効果が消えた。");
#else
                msg_format("Your magic absorber is no longer in effect.");
#endif
			}
			else if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0)
			{
				int xx = m_list[p_ptr->magic_num1[0]].fx;
				int yy = m_list[p_ptr->magic_num1[0]].fy;
#ifdef JP
				msg_format("術の効果が消えた。");
#else
                msg_format("Your technique is no longer in effect.");
#endif
				p_ptr->magic_num1[0] = 0;
				if(cave[yy][xx].m_idx) lite_spot(yy,xx);
			}
			else if(p_ptr->pclass == CLASS_MAID && ind == 0)
#ifdef JP
				msg_format("少し喉が渇いた。");
#else
                msg_format("You feel a bit thirsty.");
#endif
			else if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES))
#ifdef JP
				msg_format("周囲に音が戻った。");
#else
                msg_format("Your surroundings are no longer silenced.");
#endif
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU && ind == 0)
#ifdef JP
				msg_format("小槌の充填が完了した！");
#else
                msg_format("Your Miracle Mallet is fully charged!");
#endif
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && ind == 0)
			{
#ifdef JP
				msg_format("界王拳の効果が切れた。技の反動が体を襲った！");
#else
                msg_format("Kaiou-ken is no longer in effect. Your receive backlash from the technique!");
#endif

				set_stun(p_ptr->stun + 30 + randint1(30));
				set_slow(p_ptr->slow + 30 + randint1(30),FALSE);
			}
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && ind == 1)
			{
#ifdef JP
				msg_format("体が界王拳の反動から回復したようだ。");
#else
                msg_format("Your body has recovered from the backlash of using Kaiou-ken.");
#endif
			}
			else if(p_ptr->pclass == CLASS_BYAKUREN && ind == 0)
#ifdef JP
				msg_format("強化魔法の効果が切れた！");
#else
                msg_format("Your augmentation spell has ran out!");
#endif
			else if (p_ptr->pclass == CLASS_KOKORO && ind == 0)
				msg_format(_("恐ろしい形相の面は消えた。", "The terrifying masks around you vanish."));
			else if (p_ptr->pclass == CLASS_OKINA)
#ifdef JP
				msg_print("あなたは普段の動きに戻った。");
#else
                msg_print("You are moving normally once again.");
#endif
			else if (p_ptr->pclass == CLASS_IKU && ind == 0)
				msg_format(_("あなたは構えを解いた。", "You resume your normal stance."));
			else if(p_ptr->pclass == CLASS_SUWAKO && ind == 0)
			{
#ifdef JP
				if(!num)
					msg_format("あなたは目を覚ました。");
				else
					msg_format("あなたは冬眠から叩き起こされた！");
#else
                if(!num)
					msg_format("You wake up.");
				else
					msg_format("You are roused from your hibernation!");
#endif

				p_ptr->energy_need += ENERGY_NEED();
			}
			else if(p_ptr->pclass == CLASS_YUKARI)
			{
#ifdef JP
				if(ind == 0) msg_format("境界が元に戻った。力場が消えた。");
				if(ind == 1) msg_format("境界が元に戻った。");
				if(ind == 2) msg_format("フロアの気配が元に戻った。");
#else
                if(ind == 0) msg_format("The boundary returns to normal. The power spot vanishes.");
				if(ind == 1) msg_format("The boundary returns to normal.");
				if(ind == 2) msg_format("The atmosphere on this floor returns to normal.");
#endif
			}
			else if(p_ptr->pclass == CLASS_RINGO)
			{
				if(ind == 0)
				{
					p_ptr->magic_num1[1] = 0;
#ifdef JP
					msg_print("あなたの強さは元に戻った。");
#else
                    msg_print("Your strength returns to normal.");
#endif
				}
				if(ind == 1)
				{
#ifdef JP
					msg_format("免疫を失った。");
#else
                    msg_format("You've lost your immunity.");
#endif
					p_ptr->magic_num1[3] = 0;
					p_ptr->redraw |= (PR_STATUS);
				}

			}
			else if (p_ptr->pclass == CLASS_JYOON)
#ifdef JP
				msg_format("不幸をもたらす力を止めた。");
#else
                msg_format("You no longer bestow misfortune.");
#endif
			else if (p_ptr->pclass == CLASS_PARSEE)
#ifdef JP
                msg_format("周囲の雰囲気が通常に戻った。");
#else
                msg_format("Your surroundings return to normal.");
#endif
			else if (p_ptr->pclass == CLASS_SHION)
			{
#ifdef JP
				msg_format("負のオーラの放出が止まった。");
#else
                msg_format("You stop emitting your aura of negativity.");
#endif
				//マップを再描画する
				p_ptr->redraw |= (PR_MAP);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
			else if (p_ptr->pclass == CLASS_URUMI)
#ifdef JP
				msg_format("手に伝わる重みがもとに戻った。");
#else
                msg_format("Your hands no longer feel heavy.");
#endif
			else if (p_ptr->pclass == CLASS_YUYUKO) //v1.1.76
			{
#ifdef JP
				msg_format("灯の効果が消えた。");
#else
                msg_format("Your light trap is no longer in effect.");
#endif
			}
			else if (p_ptr->pclass == CLASS_TAKANE)
			{
#ifdef JP
				msg_format("あなたは普段の動きに戻った。");
#else
                msg_format("You are moving normally once again.");
#endif
			}
			else if (p_ptr->pclass == CLASS_MOMOYO)
			{
#ifdef JP
				if (ind == 0) msg_print("あなたは壁を掘る手を止めた。");
				else if (ind == 1) msg_print("蛇の気分ではなくなった。");
				else if (ind == 2) msg_print("龍の気分ではなくなった。");
				else msg_print("力への渇望が収まった。");
#else
                if (ind == 0) msg_print("You stop digging walls.");
				else if (ind == 1) msg_print("Your taste for snakes subsides.");
				else if (ind == 2) msg_print("Your taste for dragons subsides.");
				else msg_print("Your thirst for power subsides.");
#endif
			}
			else if (p_ptr->pclass == CLASS_MIYOI)
			{
				msg_format(_("あなたは普段の動きに戻った。",
                            "You are moving normally once again."));
			}
			else if (p_ptr->pclass == CLASS_ZANMU)
			{
				msg_format(_("力の影響が止まった。", "Your power is no longer in effect."));
			}
			else if (p_ptr->pclass == CLASS_BEEKEEPER)
			{
				msg_format(_("あなたの身を守る蜂がいなくなった。", "Bees no longer protect you."));
			}

			else
			{
				msg_format(_("ERROR:tim_general[%d]が定義されていない呼ばれ方をした、もしくはメッセージが未定義", "ERROR: tim_general[%d] is undefined"), ind);
			}

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_general[ind] = v;
	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);

	//Hack - ルーミア闇解除による盲目解除
	if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
	{
		if(p_ptr->tim_general[0])	set_blind(1);
		else	set_blind(0);
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);
	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	/* Handle stuff */
	handle_stuff();
	/* Result */
	return (TRUE);
}


///mod140804 喘息度のセット
bool set_asthma(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	if (p_ptr->asthma < ASTHMA_1) old_aux = 1;
	else if (p_ptr->asthma < ASTHMA_2) old_aux = 2;
	else if (p_ptr->asthma < ASTHMA_3) old_aux = 3;
	else if (p_ptr->asthma < ASTHMA_4) old_aux = 4;
	else  old_aux = 5;

	if (v < ASTHMA_1) new_aux = 1;
	else if (v < ASTHMA_2) new_aux = 2;
	else if (v < ASTHMA_3) new_aux = 3;
	else if (v < ASTHMA_4) new_aux = 4;
	else new_aux=5;

	if (new_aux != old_aux)
	{
#ifdef JP
		if(old_aux < new_aux) msg_print("喘息の調子が悪くなった。");
		else msg_print("喘息の調子が良くなった。");

		if(new_aux == 1) msg_print("喉がすっきり爽快だ。");
		else if(new_aux == 2) msg_print("少し咳き込む感じがする。");
		else if(new_aux == 3) msg_print("咳が止まらず静かに行動できない。");
		else if(new_aux == 4) msg_print("咳がひどくてろくに喋れない。");
		else msg_print("呼吸をするたびに喉が締め付けられるようだ！");
#else
        if(old_aux < new_aux) msg_print("Your asthma has gotten worse.");
		else msg_print("Your asthmatic condition has improved.");

		if(new_aux == 1) msg_print("Your throat feels refreshed.");
		else if(new_aux == 2) msg_print("You're coughing a little.");
		else if(new_aux == 3) msg_print("You can't stop coughing and can't stay quiet.");
		else if(new_aux == 4) msg_print("You have a severe cough and have difficulty speaking.");
		else msg_print("Your throat tightens with your every breath!");
#endif
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->asthma = v;

	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*:::一時的能力上昇*/
/*:::stat:A_STR～A_CHR amount:上昇値 limitbreak:限界値40を超えるかどうか v:カウント do_dec:減らすときTRUE*/
/*:::do_decの役割が他の関数とやや違う。現状のカウントは常にvで上書きされ、do_decがTRUEのときamountの値は無視される。*/
//注：do_decで呼ぶときはamountが0であること
bool set_tim_addstat(int stat, int amount, int v, bool do_dec)
{
	bool notice = FALSE;
	bool do_powerup = FALSE;

	if(do_dec && amount)
	{
		msg_print(_("WARNING:set_tim_addstat()のdo_decがTRUEなのにamountが設定されている", "WARNING: set_tim_addstat(): do_dec is true but amount is nonzero"));
		amount = 0;
	}

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		///mod150724 複数種類の肉体強化を混在させたとき、効力の強いものが常に優先されるようにしておく
		//if (!p_ptr->tim_addstat_count[stat])
		if (p_ptr->tim_addstat_num[stat] <= amount)
		{
#ifdef JP
			//上昇させる能力に応じてメッセージ出そうかと思ったが、この関数呼ぶ所で個別に処理したほうが自然だろう
#endif
			do_powerup = TRUE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
#ifdef JP
		if(stat==A_STR && p_ptr->tim_addstat_count[stat]) msg_print("力の強さが消えた気がする。");
		if(stat==A_INT && p_ptr->tim_addstat_count[stat]) msg_print("頭脳の冴えが鈍った気がする。");
		if(stat==A_WIS && p_ptr->tim_addstat_count[stat]) msg_print("精神の働きが鈍った気がする。");
		if(stat==A_DEX && p_ptr->tim_addstat_count[stat]) msg_print("動きの切れが鈍った気がする。");
		if(stat==A_CON && p_ptr->tim_addstat_count[stat]) msg_print("体の頑健さが失せた気がする。");
		if(stat==A_CHR && p_ptr->tim_addstat_count[stat]) msg_print("カリスマが消えた気がする。");
#else
        if(stat==A_STR && p_ptr->tim_addstat_count[stat]) msg_print("You feel your strength fading.");
		if(stat==A_INT && p_ptr->tim_addstat_count[stat]) msg_print("You feel your brain getting duller.");
		if(stat==A_WIS && p_ptr->tim_addstat_count[stat]) msg_print("You feel your mind getting duller.");
		if(stat==A_DEX && p_ptr->tim_addstat_count[stat]) msg_print("You feel your movements getting clumsier.");
		if(stat==A_CON && p_ptr->tim_addstat_count[stat]) msg_print("You feel your endurance fading.");
		if(stat==A_CHR && p_ptr->tim_addstat_count[stat]) msg_print("You feel your charisma fading.");
#endif
			notice = TRUE;
	}

	/* Use the value */
	///mod150724 複数種類の肉体強化を混在させたとき、効力の強いものが常に優先されるようにしておく
	//p_ptr->tim_addstat_count[stat] = v;

	if(do_powerup || do_dec) p_ptr->tim_addstat_count[stat] = v;

	if(do_powerup) p_ptr->tim_addstat_num[stat] = amount;
	///mod150724 上記の変更に伴い、強化が切れたら強化値格納変数をリセットする必要が生じた
	if(!p_ptr->tim_addstat_count[stat]) p_ptr->tim_addstat_num[stat] = 0;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);
	/* Disturb */
	if (disturb_state) disturb(0, 0);
	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	/* Handle stuff */
	handle_stuff();
	/* Result */
	return (TRUE);
}



//v2.0.11 移動禁止のバッドステータス
//トラバサミの罠や敵の特別行動による拘束など
bool set_no_move(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	/* Open */
	if (v)
	{
		set_tsuyoshi(0, TRUE);
		if (!p_ptr->tim_no_move)
		{
#ifdef JP
			msg_print("その場から移動できなくなった！");
#else
			msg_print("You are rooted to the spot!");
#endif
			/* Sniper */
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_no_move)
		{
#ifdef JP
			msg_print("移動できるようになった。");
#else
			msg_print("You can freely move again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_no_move = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw map */
	//p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_BONUS);

	/* Window stuff */
	//p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



///mod140103 神聖耐性のために追加
/*:::＠が破邪攻撃を受けたときの耐性・弱点・免疫考慮のために関数作成。*/
/*:::耐性考慮前ダメージを受け取って考慮後ダメージを返す。*/
int mod_holy_dam(int dam){
	int base = 100;

	base += is_hurt_holy();

	if(p_ptr->resist_holy) base /= 2;

	//警告の時にもメッセージが出てしまうので一時消す
	/*
	if(base > 150) msg_print("あなたの肉体は聖なる力により燃え上がった！");
	else if(base > 100) msg_print("聖なる力があなたの肉体を焦がした！");
	else if(base == 0) msg_print("あなたの清浄な肉体は聖なる力で傷つかない。");
	*/

	dam = dam * base / 100;

	return (dam);
}

///mod140105 神聖弱点の度合い判別
/*:::＠が神聖弱点ならその度合いを%で返す。中立が0で負なら強く正なら弱い。装備耐性とは別。*/
/*:::耐性一覧画面などほかの判定で使いやすいように上の関数から分けた*/
int is_hurt_holy(){
	int mod = 0;


	//種族による設定
	if(p_ptr->prace == RACE_YOUKAI) mod = 33;
	else if(p_ptr->prace == RACE_KAPPA) mod = 33;
	else if(p_ptr->prace == RACE_KARASU_TENGU) mod = 33;
	else if(p_ptr->prace == RACE_HAKUROU_TENGU) mod = 33;
	else if(p_ptr->prace == RACE_ONI) mod = 33;
	else if(p_ptr->prace == RACE_WARBEAST) mod = 33;
	else if(p_ptr->prace == RACE_TSUKUMO) mod = 33;
	else if(p_ptr->prace == RACE_ANDROID) mod = 33;
	else if(p_ptr->prace == RACE_YAMAWARO) mod = 33;
	else if(p_ptr->prace == RACE_BAKEDANUKI) mod = 33;
	else if(p_ptr->prace == RACE_NINGYO) mod = 33;
	else if(p_ptr->prace == RACE_HOFGOBLIN) mod = 33;
	else if(p_ptr->prace == RACE_NYUDOU) mod = 33;
	else if(p_ptr->prace == RACE_ZOMBIE) mod = 66;
	else if(p_ptr->prace == RACE_IMP) mod = 66;
	else if(p_ptr->prace == RACE_VAMPIRE) mod = 66;
	else if(p_ptr->prace == RACE_SPECTRE) mod = 66;
	else if (p_ptr->prace == RACE_ANIMAL_GHOST) mod = 66;
	else if(p_ptr->prace == RACE_DAIYOUKAI) mod = 33;
	else if (p_ptr->prace == RACE_ZASHIKIWARASHI) mod = 33;
	else if (p_ptr->prace == RACE_HALF_GHOST)
	{
		//v1.1.65 半人半霊の破邪弱点が消えるのは妖夢Lv40以降になるよう記述変更
		if(p_ptr->pclass == CLASS_YOUMU && p_ptr->lev > 39) mod = 0;
		else mod = 33;
	}
	else if(p_ptr->prace == RACE_LICH) mod = 66;
	else if(p_ptr->prace == RACE_YOUKO)
	{
		if((p_ptr->realm1 < TV_BOOK_TRANSFORM  || p_ptr->realm1 > TV_BOOK_CHAOS) && p_ptr->lev > 29) mod = 0;
		else mod = 33;
	}
	else if(prace_is_(RACE_STRAYGOD))
	{
		switch(p_ptr->race_multipur_val[3])
		{
			//魔神、死者の神、邪神、外なる神
			case 59: case 61: case 62: case 63:
			mod = 50;
			break;
		}
	}
	else if(p_ptr->prace == RACE_TENNIN) mod = -100;
	else if(p_ptr->prace == RACE_LUNARIAN) mod = -100;

	/*:::一部職による種族の例外判定*/
	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->pseikaku == SEIKAKU_SHIAWASE && p_ptr->lev > 44) mod = 0;
	else if(p_ptr->pclass == CLASS_KASEN) mod=33;
	else if(p_ptr->pclass == CLASS_ORIN) mod=66;
	else if(p_ptr->pclass == CLASS_CHIYARI) mod = 66;
	else if(p_ptr->pclass == CLASS_MURASA) mod=66;
	else if(p_ptr->pclass == CLASS_NAZRIN || p_ptr->pclass == CLASS_SHOU) mod=0;
	else if(p_ptr->pclass == CLASS_SHOU && p_ptr->lev > 44) mod=-100;
	else if(p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0]) mod = 0;
	else if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 44) mod=0;
	else if(p_ptr->pclass == CLASS_MEIRIN && p_ptr->lev > 39) mod=0;
	else if(p_ptr->pclass == CLASS_HINA) mod=50;
	else if(p_ptr->pclass == CLASS_AUNN) mod=0;
	else if(p_ptr->pclass == CLASS_KEINE) mod=0;
	else if(p_ptr->pclass == CLASS_MIKO && p_ptr->lev > 29) mod = -100;
	else if(p_ptr->pclass == CLASS_YUYUKO) mod = 50;
	else if(p_ptr->pclass == CLASS_REIMU && p_ptr->lev > 29 && osaisen_rank() > 4) mod = -100;
	else if(p_ptr->pclass == CLASS_EIKI) mod = -100;
	else if (p_ptr->pclass == CLASS_MIKE) mod = 0;


	//v1.1.37 一部破邪に弱い種族に変身中は種族的な例外判定を上書きすることにした
	if(p_ptr->mimic_form == MIMIC_DEMON || p_ptr->mimic_form == MIMIC_DEMON_LORD || p_ptr->mimic_form == MIMIC_VAMPIRE ) mod = 66;
	else if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_IS_UNDEAD | MIMIC_IS_DEMON)) mod = 66; //v1.1.37追加　↑の条件式とちょっと被るが
	else if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_HOLY) mod = 50;
	//v1.1.84 反獄王に乗っ取られてるときも破邪に弱くする
	if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH) mod = 66;

	if (p_ptr->mimic_form == MIMIC_GOD_OF_NEW_WORLD)
		mod = -100;

	///mod140219 暗黒、死霊領域だと破邪に弱くなる
	///mod150822 メルラン、リリカ除く
	if(p_ptr->pclass != CLASS_MERLIN && p_ptr->pclass != CLASS_LYRICA)
	{
		if(p_ptr->realm1 == TV_BOOK_DARKNESS || p_ptr->realm1 == TV_BOOK_NECROMANCY) mod += 33;
		else if(p_ptr->realm2 == TV_BOOK_DARKNESS || p_ptr->realm2 == TV_BOOK_NECROMANCY) mod += 25;
	}

	//特殊処理
	if(p_ptr->tim_unite_darkness) mod += 50;

	if(mod >= 99) mod = 100;

/*
	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:神聖弱点の判定値がおかしい");
	}
*/
	return (mod);
}

///mod140115 劣化弱点の度合い判別　神聖と同じルーチン　

int mod_disen_dam(int dam){
	int base = 100;

	base += is_hurt_disen();



	if(p_ptr->resist_disen)
	{
	base *= 6;
	base /= (randint1(4) + 7);
	}



	//if(base > 100) msg_print("体から力が消える気がする！");

	dam = dam * base / 100;

	return (dam);
}

int is_hurt_disen(){
	int mod = 0;

	if(prace_is_(RACE_TSUKUMO)) mod = 33;
	if(prace_is_(RACE_ANDROID)) mod = 33;
	if(prace_is_(RACE_GOLEM)) mod = 33;
	if(prace_is_(RACE_MAGIC_JIZO)) mod = 33;
	if(p_ptr->pclass == CLASS_ALICE) mod = 33;
	if (p_ptr->pclass == CLASS_MISUMARU) mod = 33;

	if (p_ptr->pclass == CLASS_TSUKASA) mod = 25;


	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->lev > 39) mod = 0;
	if(p_ptr->mimic_form == MIMIC_MIST) mod = 33;



	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print(_("ERROR:劣化弱点の判定値がおかしい", "ERROR: incorrect disenchantment modifier"));
	}

	return (mod);
}


///mod140122 閃光弱点の度合い判別　　

int mod_lite_dam(int dam){
	int base = 100;

	base += is_hurt_lite();

	if(p_ptr->resist_lite)
	{
	base *= 4;
	base /= (randint1(4) + 7);
	}

	//if(base > 100) msg_print("強い光があなたの肉体を焦がした！");

	dam = dam * base / 100;

	return (dam);
}

int is_hurt_lite(){
	int mod = 0;


	if(p_ptr->pclass ==  CLASS_RUMIA) mod = 33;
	if(p_ptr->pclass ==  CLASS_NUE) mod = 50;
	if(p_ptr->muta3 & MUT3_HURT_LITE) mod = 33;
	if(prace_is_(RACE_VAMPIRE)) mod = 50;
	if (prace_is_(RACE_ANIMAL_GHOST) && p_ptr->pclass != CLASS_SAKI) mod = 50;
	if(prace_is_(RACE_SPECTRE) && !CLASS_IS_PRISM_SISTERS) mod = 50;
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 44) mod = -100;//太陽神

	if (p_ptr->wraith_form) mod = 100;
	else if(p_ptr->mimic_form == MIMIC_MIST) mod = 33;
	else if(p_ptr->mimic_form == MIMIC_VAMPIRE) mod = 50;

	if(p_ptr->tim_unite_darkness) mod += 50;
	if(p_ptr->pclass == CLASS_YUYUKO) mod = 0; //特殊処理

	if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 44) mod = -100; //特殊処理
	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_LITE) mod = 50;

	//v1.1.37
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI) mod = 50;

	/*
	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:閃光弱点の判定値がおかしい");
	}
	*/

	return (mod);
}

///mod140126 水弱点の度合い判別と耐性処理
int mod_water_dam(int dam){
	int base = 100;

	base += is_hurt_water();

	if(p_ptr->resist_water)
	{
	base /= 2;
	}

	//if(base > 100) msg_print("水は苦手だ！");

	dam = dam * base / 100;

	//if(dam<1) dam=1;
	return (dam);
}

int is_hurt_water(){
	int mod = 0;

	if(p_ptr->mimic_form == MIMIC_VAMPIRE) mod = 50;
	if(prace_is_(RACE_VAMPIRE)) mod = 50;
	if(p_ptr->pclass == CLASS_CHEN) mod = 33;

	//水免疫
	if(p_ptr->pclass == CLASS_WAKASAGI && p_ptr->lev > 19) mod = -100;
	if(p_ptr->pclass == CLASS_MURASA) mod = -100;
	//海神
	if(prace_is_(RACE_STRAYGOD) &&  p_ptr->race_multipur_val[3] == 47) mod = -100;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_WATER) mod = 50;

	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print(_("ERROR:水弱点の判定値がおかしい", "ERROR: incorrect water damage modifier"));
	}

	return (mod);
}



///mod140302 火炎弱点の度合い判別と耐性処理 最低1は受けるようにしておく　免疫は別処理
int mod_fire_dam(int dam){
	int base = 100;

	base += is_hurt_fire();
	if (p_ptr->resist_fire) base /= 3;
	if (IS_OPPOSE_FIRE()) base /= 3;
	//if(base > 100) msg_print("あなたの体は燃え上がった！");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_fire(){
	int mod = 0;

	if(p_ptr->pclass == CLASS_KOGASA) mod = 33;
	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->pseikaku == SEIKAKU_GAMAN && p_ptr->lev > 34) mod = 0;
	if(p_ptr->pclass == CLASS_CIRNO)
	{
		//v1.1.32
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mod = 0;
		else if(p_ptr->lev > 29) mod = 25;
		else mod = 50;
	}

	if(p_ptr->pclass == CLASS_LETTY) mod = 33;
	if(p_ptr->pclass == CLASS_KAGEROU) mod = 25;

	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_FIRE) mod = 50;

	//v1.1.37
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI) mod = 50;


	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print(_("ERROR:火炎弱点の判定値がおかしい", "ERROR: incorrect fire damage modifier"));
	}

	return (mod);
}

///mod140302 冷気弱点の度合い判別と耐性処理 最低1は受けるようにしておく　免疫は別処理
int mod_cold_dam(int dam){
	int base = 100;

	base += is_hurt_cold();
	if (p_ptr->resist_cold) base /= 3;
	if (IS_OPPOSE_COLD()) base /= 3;
	//if(base > 100) msg_print("あなたの体はみるみる凍りついた！");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_cold(){
	int mod = 0;

	if (p_ptr->pclass == CLASS_WRIGGLE) mod += 33;
	if (p_ptr->pclass == CLASS_SHIZUHA) mod += 25;
	if (p_ptr->pclass == CLASS_MINORIKO) mod += 25;
	if (p_ptr->pclass == CLASS_LARVA) mod += 33;

	if (IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_COLD) mod += 50;
	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;

	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;


	if (mod > 200) mod = 200;
	if (mod < -100) mod = -100;

	return (mod);
}

///mod140302 電撃弱点の度合い判別と耐性処理 最低1は受けるようにしておく　免疫は別処理
int mod_elec_dam(int dam){
	int base = 100;

	base += is_hurt_elec();
	if (p_ptr->resist_elec) base /= 3;
	if (IS_OPPOSE_ELEC()) base /= 3;
	//if(base > 100) msg_print("あなたの体を電流が駆け抜けた！");
	dam = dam * base / 100;
	if(dam<1) dam=1;

	return (dam);
}

int is_hurt_elec(){
	int mod = 0;

	if(prace_is_(RACE_KAPPA)) mod = 33;



	if(p_ptr->pclass == CLASS_KAGEROU) mod += 25;
	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_ELEC) mod = 50;

	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print(_("ERROR:電撃弱点の判定値がおかしい", "ERROR: incorrect electrical damage modifier"));
	}

	return (mod);
}

///mod140302 酸弱点の度合い判別と耐性処理 最低1は受けるようにしておく　免疫は別処理
int mod_acid_dam(int dam){
	int base = 100;

	base += is_hurt_acid();
	if (p_ptr->resist_acid) base /= 3;
	if (IS_OPPOSE_ACID()) base /= 3;
//	if(base > 100) msg_print("酸があなたの体を焦がした！");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_acid(){
	int mod = 0;

	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->lev < 30) mod = 33;


	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;


	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print(_("ERROR:酸弱点の判定値がおかしい", "ERROR: incorrect acid damage modifier"));
	}

	return (mod);
}






/*:::狂気に対する免疫を判定*/
bool immune_insanity(void)
{
	if( p_ptr->pseikaku == SEIKAKU_BERSERK) return (TRUE);
	if(prace_is_(RACE_IMP)) return (TRUE);
	if(prace_is_(RACE_LICH)) return (TRUE);
	if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON) return (TRUE);

	if(p_ptr->pclass == CLASS_CLOWNPIECE) return (TRUE);

	if(diehardmind()) return (TRUE);
	//外なる神
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 63) return (TRUE);


	 return (FALSE);
}

//「エイボンの霧の車輪」の効果が切れる
void break_eibon_wheel(void)
{

	//v2.0.1 ひらり布のカードを使っている場合同じ条件で解除する
	if (p_ptr->tim_hirari_nuno) set_hirarinuno_card(0, TRUE);

	if(!(p_ptr->special_defense & (SD_EIBON_WHEEL))) return;

	//v1.1.33 あうんの石像化もこのルーチンに統合する
	if(p_ptr->special_defense & (SD_STATUE_FORM))
#ifdef JP
		msg_print("あなたは石像から人型に変わった。");
#else
        msg_print("You revert from a statue to human form.");
#endif
	else
		msg_print("The cylinder of blue mist vanishes!");

	p_ptr->special_defense &= ~(SD_EIBON_WHEEL | SD_STATUE_FORM);
	p_ptr->redraw |= (PR_STATUS);
	redraw_stuff();
}

//v1.1.44 うどんげ「イビルアンジュレーション」の処理。special_defenseを3ビット使う。
//activate
//		TRUEのときビットを全て立てる。
//check
//		TRUEのとき、ビットが一つでも立っていれば一つ落としてTRUEを返す。一つもない場合FALSEを返す。
//
//どちらもFALSEのときビットを全て落とす。
bool evil_undulation(bool activate, bool check)
{

	if (activate)
	{
#ifdef JP
        msg_print("異質な波動があなたを包んだ。");
#else
        msg_print("Evil undulations wrap around you.");
#endif
		p_ptr->special_defense |= EVIL_UNDULATION_MASK;
	}
	else if (check)
	{
		if (p_ptr->special_defense & EVIL_UNDULATION_MASK)
		{
#ifdef JP
			msg_print("障壁波動があなたを守った！");
#else
            msg_print("Your wave barrier protects you!");
#endif

			if (p_ptr->special_defense & SD_EVIL_UNDULATION_1)
				p_ptr->special_defense &= ~(SD_EVIL_UNDULATION_1);
			else if (p_ptr->special_defense & SD_EVIL_UNDULATION_2)
				p_ptr->special_defense &= ~(SD_EVIL_UNDULATION_2);
			else
			{
#ifdef JP
				msg_print("あなたを守る波動が消えた。");
#else
                msg_print("Your barrier fades away.");
#endif
				p_ptr->special_defense &= ~(EVIL_UNDULATION_MASK);
			}

		}
		else
		{
			return FALSE;
		}
	}
	else
	{
#ifdef JP
        msg_print("あなたを守る波動が消えた。");
#else
        msg_print("Your barrier fades away.");
#endif
		p_ptr->special_defense &= ~(EVIL_UNDULATION_MASK);

	}
	p_ptr->redraw |= PR_STATUS;
	return TRUE;

}

//v1.1.56 アイテム「スペルカード」での時限性効果処理
bool set_tim_spellcard_effect(int v, bool do_dec, int art_idx)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->spellcard_effect_idx == art_idx)
		{
			//同じカードのとき効果時間が短くなることはない
			if (p_ptr->tim_spellcard_count >= v) return FALSE;
		}
		else if (!do_dec)
		{
			//古い効果がある場合それが消去されるメッセージ
			if (p_ptr->tim_spellcard_count)
				msg_format("%s", msg_tim_nameless_arts(p_ptr->spellcard_effect_idx,0, FALSE));
			//新しい効果の発動メッセージ
			msg_format("%s", msg_tim_nameless_arts(art_idx, 0,TRUE));
			notice = TRUE;

			//今起こっている効果のidx
			p_ptr->spellcard_effect_idx = art_idx;
		}
		//do_decのときは何のメッセージも出ずカウント値が減るだけ
	}

	/* Shut */
	else
	{
		if (p_ptr->spellcard_effect_idx)
		{
			//古い効果が消去されるメッセージ
			msg_format("%s", msg_tim_nameless_arts(p_ptr->spellcard_effect_idx,0, FALSE));
			p_ptr->spellcard_effect_idx = 0;
			notice = TRUE;
		}
	}


	//カウンタ値
	p_ptr->tim_spellcard_count = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->update |= (PU_TORCH);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

//v1.1.71	アイテム「チョウセンアサガオの葉」の実装に合わせて胃の掃除ルーチンを独立
bool throw_up(void)
{

	int food = p_ptr->food;

	//毒耐性があると効果がないことにする
	if (p_ptr->resist_pois || IS_OPPOSE_POIS()) return FALSE;
	//野良神様は評判が落ちる
	set_deity_bias(DBIAS_REPUTATION, -3);

#ifdef JP
	msg_format("っ！？");
#else
    msg_format("...!?");
#endif
	msg_format("");
	screen_save();
	Term_clear();
#ifdef JP
	put_str("しばらくお待ち下さい。", 15, 30);
#else
    put_str("Please wait for a bit.", 15, 30);
#endif
	(void)inkey();
	//Term_xtra(TERM_XTRA_DELAY, 2000);
	screen_load();
#ifdef JP
	msg_format("胃の中が空になった。");
#else
    msg_format("Your stomach is now empty.");
#endif

	if (food >= PY_FOOD_ALERT) set_food(PY_FOOD_ALERT-2);//葉っぱそのもので満腹度が1増えるので-2にしとく
	else set_food(food / 2);
	set_alcohol(p_ptr->alcohol - 2000);

	set_poisoned(0);

	return TRUE;

}

//v1.1.78 酔拳実装
void	check_suiken(void)
{
	bool flag_ok;

	if (p_ptr->alcohol >= DRANK_2) flag_ok = TRUE;//「最高の気分だ！」以上のとき
	else flag_ok = FALSE;

	//格闘状態のときのみ
	if (!p_ptr->do_martialarts) flag_ok = FALSE;

	if (ref_skill_exp(SKILL_MARTIALARTS) < 3200) flag_ok = FALSE;

	//酔ってノックダウン中は酔拳停止
	if (!(p_ptr->muta2 & MUT2_ALCOHOL) && p_ptr->alcohol >= DRANK_4) flag_ok = FALSE;

	//使えるようになった時
	if (flag_ok && !(p_ptr->special_attack & ATTACK_SUIKEN))
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_REIMU)
			msg_print("あなたは博麗流夢想酔拳の構えをとった！");
		else if(one_in_(2))msg_print("あなたは酔拳の構えをとった！");
		else msg_print("あなたは無規則に体を揺らし始めた...");
#else
        if(p_ptr->pclass == CLASS_REIMU)
			msg_print("You assume the Hakurei Peerless Drunken Fist posture!");
		else if(one_in_(2))msg_print("You assume the Drunken Fist posture!");
		else msg_print("Your body is shaking irregularly...");
#endif

		p_ptr->special_attack |= ATTACK_SUIKEN;
		p_ptr->redraw |= PR_STATUS;

	}
	//使えなくなった時
	else if (!flag_ok && (p_ptr->special_attack & ATTACK_SUIKEN))
	{
#ifdef JP
		msg_print("あなたは酔拳をやめた。");
#else
        msg_print("You are no longer using Drunken Fist.");
#endif
		p_ptr->special_attack &= ~(ATTACK_SUIKEN);
		p_ptr->redraw |= PR_STATUS;

	}

}






//v2.0.1 アビリティカードのひらり布(正邪特技のひらり布ではない)
//カウントダウンのほかエイボンの霧の車輪(break_eibon_wheel())のように移動や攻撃などで解除される
//ダメージでは解除されない
bool set_hirarinuno_card(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_hirari_nuno && !do_dec)
		{
			if (p_ptr->tim_hirari_nuno > v) return FALSE;
		}
		else
		{
			msg_print(_("あなたは布をかぶって姿を隠した。", "You hide yourself in the fabric."));

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_hirari_nuno)
		{
			msg_print(_("あなたは布から出て姿を現した。", "You emerge from the fabric."));

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_hirari_nuno = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



//移送の罠
//＠が移送の罠状態になると隣接攻撃してきたモンスターに判定で抹殺処理
bool set_tim_transportation_trap(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->transportation_trap && !do_dec)
		{
			if (p_ptr->transportation_trap>v) return FALSE;
		}
		else if (!p_ptr->transportation_trap)
		{
			msg_print(_("移送の罠が発動した。", "You activate a transportation trap."));
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->transportation_trap)
		{
			msg_print(_("移送の罠の発動が止まった。", "Your transportation trap is no longer active."));

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->transportation_trap = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

