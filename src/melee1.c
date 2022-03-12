/* File: melee1.c */

/* Purpose: Monster attacks */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
/*:::�����X�^�[�̑Ō����N���e�B�J���q�b�g���邩�ǂ�������*/
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20) return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (randint0(100) >= dam)) return (0);

	/* Perfect damage */
	if ((dam >= total) && (dam >= 40)) max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (randint0(100) < 2) max++;
	}

	/* Critical damage */
	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match monster power against player armor.
 */
/*:::�����X�^�[�����ɒ��ڍU������Ƃ��̖�������*/
///system �����̊֐��������Ă�₱�����̂Ŗ��O�ς��悤

///mod141101 �t���O"nomind"��ǉ��@���Ƃ��p����
static int check_hit(int power, int level, int stun, bool nomind)
{
	int i, k, ac;

	/* Percentile dice */
	k = randint0(100);

	if (stun && one_in_(2)) return FALSE;

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;
	if(p_ptr->pclass == CLASS_SATORI && !nomind) ac += 50 + p_ptr->stat_ind[A_INT]+3; //���Ƃ����AC�{�[�i�X

	//v1.1.78 ������AC�{�[�i�X �D���x/100 +30�`+150
	//calc_bonuses()�ɓ���ĕ\�L�ɔ��f���ׂ����Ǝv�����D���x���ς�邽�т�calc_bonuses()��ʂ��Əd���Ȃ邩������Ȃ��̂ł����ŉB�������ɂ���
	if (p_ptr->special_attack & ATTACK_SUIKEN) ac += (p_ptr->alcohol / 100);

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint1(i) > ((ac * 3) / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Hack -- possible "insult" messages
 */
///msg ���J�U���@�ő�7�ڂ܂Ŏg����@�����ς��悤���H
static cptr desc_insult[] =
{
#ifdef JP
	"�����Ȃ��𕎐J�����I",
	"�����Ȃ��̕�𕎐J�����I",
	"�����Ȃ����y�̂����I",
	"�����Ȃ���J�߂��I",
	"�����Ȃ����������I",
	"�����Ȃ��̉��ŗx�����I",
	"�������Ȑg�Ԃ�������I",
	"�����Ȃ����ڂ���ƌ����I�I�I",
	"�����Ȃ����p���T�C�g�Ă΂�肵���I",
	"�����Ȃ����T�C�{�[�O���������I"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
	"calls you a parasite!",
	"calls you a cyborg!"
#endif

};



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan_maggot[] =
{
#ifdef JP
	"�͉�����߂���ł���悤���B",
	"���ނ̎����������Ȃ��������Ɛq�˂Ă���B",
	"���꒣�肩��o�čs���ƌ����Ă���B",
	"�̓L�m�R���ǂ��Ƃ��ꂢ�Ă���B"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif

};
static cptr desc_moan[] =
{
#ifdef JP
	"�͔߂����Ȃ��߂������グ���B",
	"�͕����ڂ����ɂ�����������B",
	"�͉�������s���Ă���B",
	"�͂��߂����B"
#else
    "lets out a sad moan.",
    "lowers her head, staring downwards.",
    "complains about something.",
    "moans."
#endif
};


//��肪���łɂ��̑����̖Ɖu�������Ă���ꍇTRUE��Ԃ��B�܂����̑������L�^����
static bool process_ringo_im(int typ)
{
	if(p_ptr->pclass != CLASS_RINGO) return FALSE;
	p_ptr->magic_num1[2] = typ;

	return CHECK_RINGO_IMMUNE(typ);
}

/*
 * Attack the player via physical attacks.
 */
/*:::�����X�^�[���灗�ւ̒ʏ�U��*/
bool make_attack_normal(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	int ap_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char m_name[80];

	char ddesc[80];

	bool blinked;
	bool blinked_photo;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;
	bool conf=FALSE, stun=FALSE;
	bool do_silly_attack = (one_in_(2) && p_ptr->image);
	int get_damage = 0;
	int abbreviate = 0;

	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (FALSE);

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE) return (FALSE);

	/* ...nor if friendly */
	if (!is_hostile(m_ptr)) return FALSE;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	//�h�b�y���Q���K�[���ꏈ��
	if(r_ptr->flags7 & RF7_DOPPELGANGER)
	{
		delete_monster_idx(m_idx);
		msg_print(_("���Ȃ��͎����̃h�b�y���Q���K�[�ɐG��Ă��܂����I",
                    "Oh no! You've touched your doppelganger!"));
		take_hit(DAMAGE_LOSELIFE,p_ptr->chp,_("�h�b�y���Q���K�[�Ƃ̐ڐG", "contact with doppelganger"),-1);
		set_stun(100+randint0(3));

		return TRUE;
	}


	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == m_idx)
	{
		msg_format(_("%^s�͍U�����ł��Ȃ��I",
                    "%^s cannot attack!"), m_name);
		return TRUE;
	}

	if (p_ptr->special_defense & KATA_IAI)
	{
#ifdef JP
		msg_print("���肪�P��������O�ɑf���������U������B");
#else
		msg_format("You quickly draw out your weapon and strike before %s can attack.", m_name);
#endif
		if (py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_IAI)) return TRUE;
	}



	if (p_ptr->pclass == CLASS_SEIJA && p_ptr->special_defense & NINJA_KAWARIMI)
	{
		if (kawarimi(TRUE)) return TRUE;
	}
	else if (p_ptr->pclass == CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (p_ptr->lev+20)))
	{
		if (kawarimi(TRUE)) return TRUE;
	}
	else if (p_ptr->pclass != CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (p_ptr->lev*3/5+20)))
	{
		if (kawarimi(TRUE)) return TRUE;
	}
	else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		msg_format(_("%^s�̍U���͓͂��Ȃ������B", "%^s failed to reach you."), m_name);
		return TRUE;
	}

	//v1.1.77 ����(��p���i)�̒ǐՁ@
	if (orin_escape(m_idx)) return TRUE;


	//v1.1.66 �n�j���ɂ�G�V���{���̍U���������Ȃ����Ƃɂ���
	if (r_ptr->d_char == 'G' && prace_is_(RACE_HANIWA))
	{
		msg_format(_("%^s�̍U���͂��Ȃ��ɂ͌����Ă��Ȃ��I",
                    "You are unaffected by the attack of %^s!"), m_name);
		return TRUE;
	}


	/* Assume no blink */
	blinked = FALSE;
	blinked_photo = FALSE;

	/*:::�ő�4��̍U��������*/
	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;
		///mod140626 ��{damage�Ɠ���������_������damage�������Ă�������͑����Ȃ��B
		int damage_stun = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;


		if (!m_ptr->r_idx) break;

		/* Hack -- no more attacks */
		if (!method) break;

		/*:::�L�X��������̍U����������鏈��*/
		///mod141118 �L�X��
		if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent )
		{
			int chance;
			switch(method)
			{
				// 1/concent+1�Ŗ���
				case RBM_HIT:	case RBM_SPIT:
				case RBM_SPORE:	case RBM_INSULT:
				case RBM_MOAN:	case RBM_SQUEEZE:
				case RBM_DRAG:	case RBM_SMILE:
				case RBM_FLASH:
				case RBM_SMOKE:
				chance = 100 / (p_ptr->concent+1);
				break;

				// ��ɉ��
				case RBM_TOUCH:	case RBM_PUNCH:
				case RBM_KICK:	case RBM_CLAW:
				case RBM_BITE:	case RBM_STING:
				case RBM_SLASH:	case RBM_BUTT:
				case RBM_CRUSH:	case RBM_ENGULF:
				case RBM_CRAWL:	case RBM_DROOL:
				case RBM_BEG:	case RBM_SPEAR:
				case RBM_BOOK:	case RBM_MOFUMOFU:
				case RBM_INJECT:	case RBM_HEADBUTT:
				case RBM_WHISPER:	case RBM_PRESS:
				case RBM_STRIKE:
				chance = 0;
				break;

				//��ɖ���
				default:
				chance = 100;
				break;

			}
			//�����G�A�͋����G�A����ȓG�A����ԓG�̍U���͓�����₷��
			if(r_ptr->flags2 & RF2_SMART) chance += 20;
			if(r_ptr->flags2 & RF2_POWERFUL) chance += 30;
			if(r_ptr->flags2 & RF2_GIGANTIC) chance += 40;
			if(r_ptr->flags7 & RF7_CAN_FLY) chance += 50;
			if(randint1(100) > chance)
			{
				msg_format(_("%^s�̍U���͓͂��Ȃ������B",
                            "%^s failed to reach out."), m_name);
				continue; //�U�����
			}
		}

		/*:::�V�A�[�n�[�g�A�^�b�N�H*/
		if (is_pet(m_ptr) && (r_ptr->flags1 & RF1_UNIQUE) && (method == RBM_EXPLODE))
		{
			method = RBM_HIT;
			d_dice /= 10;
		}

		/* Stop if player is dead or gone */
		if (!p_ptr->playing || p_ptr->is_dead) break;
		if (distance(py, px, m_ptr->fy, m_ptr->fx) > 1) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		if (method == RBM_SHOOT) continue;

		///mod140323 ���������Ɍo���l�𓾂�B�����ɒu������ő�4���x�ɓ��邱�ƂɂȂ邪�܂��������낤
		if(!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_SHIELD || !check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_LARM].tval == TV_SHIELD)
			gain_skill_exp(SKILL_SHIELD,m_ptr);

		/* Extract the attack "power" */
		power = mbe_info[effect].power;

		/* Total armor */
		ac = p_ptr->ac + p_ptr->to_a;

		/* Monster hits player */
		/*:::��������*/
		if (!effect || check_hit(power, rlev, MON_STUNNED(m_ptr),(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))))
		{
			/* Always disturbing */
			disturb(1, 1);


			/* Hack -- Apply "protection from evil" */
			/*:::�Ύ׈����E�̌��ޔ���*/
			if ((p_ptr->protevil > 0) &&
			    (r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_DEMON | RF3_KWAI | RF3_UNDEAD)) &&
			    (p_ptr->lev >= rlev) &&
			    ((randint0(100) + p_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ANG_CHAOS;

				/* Message */
#ifdef JP
				if (abbreviate)
				    msg_format("���ނ����B");
				else
				    msg_format("%^s�͌��ނ��ꂽ�B", m_name);
				abbreviate = 1;/*�Q��ڈȍ~�͏ȗ� */
#else
				msg_format("%^s is repelled.", m_name);
#endif


				/* Hack -- Next attack */
				continue;
			}

			if((m_ptr->mflag & MFLAG_NO_SPELL) ||
			p_ptr->silent_floor)
			{
				if(method == RBM_MOAN || method == RBM_SHOW || method == RBM_WHISPER ||method == RBM_BEG || method == RBM_INSULT)
				{
				    msg_format(_("%^s�͉����𒝂낤�Ƃ����悤���B",
                                "%^s tries to say something."), m_name);
					continue;

				}
			}
			if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1]))
			{
				if(method == RBM_MOAN || method == RBM_SHOW || method == RBM_WHISPER ||method == RBM_BEG || method == RBM_INSULT || method == RBM_HOWL)
				{
				    msg_format(_("%^s�͐����o���Ȃ������B",
                                "%^s couldn't speak."), m_name);
					continue;
				}
				else if(method == RBM_PLAY)
				{
				    msg_format(_("%^s�͉��t�Ɏ��s�����B",
                                "%^s fails to perform."), m_name);
					continue;
				}
			}
			//���R�F���̐_�~�낵
			//SLASH,SPEAR�̍U���S�Ă�STING�̍U���̂���HUMAN,DEMIHUMAN,is_gen_unique()�̃����X�^�[�ɂ��U���𖳌���
			if((p_ptr->kamioroshi & KAMIOROSHI_KANAYAMAHIKO)
				&& (method == RBM_SLASH || method == RBM_SPEAR || method == RBM_STING && ( r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN) || is_gen_unique(m_ptr->r_idx))) )
			{
				if (!abbreviate)
				{
				    msg_format(_("���R�F����%^s�̕���𖳗͉������I",
                                "The weapon of %^s is negated by Kanayamahiko-no-Mikoto!"), m_name);
					abbreviate = 1;/*�Q��ڈȍ~�͏ȗ� */
				}
				continue;
			}

			//v1.1.89 �S�X���X�l�[�N�C�[�^�[�ɂ�����U���̖�����
			if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[1])
			{
				if (method == RBM_BITE)
				{
					msg_print(_("���݂��U�������킵���I",
                                "You evade the biting attack!"));
					continue;
				}
				if (method == RBM_SQUEEZE)
				{
					msg_print(_("���ߕt���U�������킵���I",
                                "You evade the squeezing attack!"));
					continue;
				}
			}

			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Describe the attack method */
			///sys mon �����X�^�[�ʏ�U�����s��
			switch (method)
			{
				case RBM_HIT:
				{
#ifdef JP
		///mod131231 �����X�^�[�U����HIT�����邩��u�U������v�֕\�L�ύX
					act = "�U�����ꂽ�B";
#else
					act = "hits you.";
#endif

					do_cut = do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_TOUCH:
				{
#ifdef JP
					act = "�G��ꂽ�B";
#else
					act = "touches you.";
#endif

					touched = TRUE;
					sound(SOUND_TOUCH);
					break;
				}

				case RBM_PUNCH:
				{
#ifdef JP
					act = "����ꂽ�B";
#else
					act = "punches you.";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_KICK:
				{
#ifdef JP
					act = "�R��ꂽ�B";
#else
					act = "kicks you.";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CLAW:
				{
#ifdef JP
					act = "�Ђ������ꂽ�B";
#else
					act = "claws you.";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BITE:
				{
#ifdef JP
					act = "���܂ꂽ�B";
#else
					act = "bites you.";
#endif

					do_cut = 1;
					touched = TRUE;
					sound(SOUND_BITE);
					break;
				}

				case RBM_STING:
				{
#ifdef JP
					act = "�h���ꂽ�B";
#else
					act = "stabs you.";
#endif

					touched = TRUE;
					sound(SOUND_STING);
					break;
				}

				case RBM_SLASH:
				{
#ifdef JP
					act = "�a��ꂽ�B";
#else
					act = "slashes you.";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BUTT:
				{
#ifdef JP
					act = "�p�œ˂��ꂽ�B";
#else
					act = "butts you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CRUSH:
				{
#ifdef JP
					act = "�̓����肳�ꂽ�B";
#else
					act = "crushes you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_ENGULF:
				{
#ifdef JP
					act = "���ݍ��܂ꂽ�B";
#else
					act = "engulfs you.";
#endif

					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_CHARGE:
				{
#ifdef JP
					abbreviate = -1;
					act = "�͐��������悱�����B";
#else
					act = "gives you an invoice.";
#endif

					touched = TRUE;
					sound(SOUND_BUY); /* Note! This is "charges", not "charges at". */
					break;
				}

				case RBM_CRAWL:
				{
#ifdef JP
					abbreviate = -1;
					act = "���̂̏�𔇂�������B";
#else
					act = "crawls on you.";
#endif

					touched = TRUE;
					sound(SOUND_SLIME);
					break;
				}

				case RBM_DROOL:
				{
#ifdef JP
					act = "�悾������炳�ꂽ�B";
#else
					act = "drools on you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_SPIT:
				{
#ifdef JP
					act = "����f���ꂽ�B";
#else
					act = "spits on you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_EXPLODE:
				{
#ifdef JP
					abbreviate = -1;
					act = "�͔��������B";
#else
					act = "explodes.";
#endif

					explode = TRUE;
					break;
				}

				case RBM_GAZE:
				{
#ifdef JP
					act = "�ɂ�܂ꂽ�B";
#else
					act = "gazes at you.";
#endif

					break;
				}

				case RBM_WAIL:
				{
#ifdef JP
					act = "�������΂ꂽ�B";
#else
					act = "wails at you.";
#endif

					sound(SOUND_WAIL);
					break;
				}

				case RBM_SPORE:
				{
#ifdef JP
					act = "�E�q���΂��ꂽ�B";
#else
					act = "releases spores at you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_DANCE:
				{
#ifdef JP
					abbreviate = -1;
					act = "�����Ȃ��̉��ŗx�����I";
#else
					act = "dances around you!";
#endif

					break;
				}

				case RBM_BEG:
				{
#ifdef JP
					act = "���������܂ꂽ�B";
#else
					act = "begs you for money.";
#endif

					sound(SOUND_MOAN);
					break;
				}

				case RBM_INSULT:
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = desc_insult[randint0(8)];
					//act = desc_insult[randint0(m_ptr->r_idx == MON_DEBBY ? 10 : 8)];
					sound(SOUND_MOAN);
					break;
				}

				case RBM_MOAN:
				{
#ifdef JP
					abbreviate = -1;
#endif
					if(m_ptr->r_idx == MON_MAGGOT)
						act = desc_moan_maggot[randint0(4)];
					else
						act = desc_moan[randint0(4)];

					sound(SOUND_MOAN);
					break;
				}

				case RBM_SHOW:
				{
#ifdef JP
					abbreviate = -1;
#endif
					if (m_ptr->r_idx == MON_JAIAN)
					{
#ifdef JP
						switch(randint1(15))
						{
						  case 1:
						  case 6:
						  case 11:
							act = "�u�􂨁`��̓W���C�A���`�`�K�`�L�������傤�`�v";
							break;
						  case 2:
							act = "�u��ā`�񂩂ށ`�Ă��̂��`�Ƃ������`�`�v";
							break;
						  case 3:
							act = "�u��́`�ё��X�l�v�̓�����Ȃ���`�`�v";
							break;
						  case 4:
							act = "�u�􂯁`�񂩃X�|�`�c�`�ǂ�Ƃ����`�v";
							break;
						  case 5:
							act = "�u�􂤂��`���`�`���`�܂����`�܂����Ƃ��`�v";
							break;
						  case 7:
							act = "�u��܁`�������΁`��̂Ɂ`�񂫂��́`�`�v";
							break;
						  case 8:
							act = "�u��ׂ񂫂傤���キ����������Ȃ���`�`�v";
							break;
						  case 9:
							act = "�u�􂫂͂₳���`���ā`���`��������`�v";
							break;
						  case 10:
							act = "�u�􂩂��`���`�`�X�^�C�����`�o�c�O�����`�v";
							break;
						  case 12:
							act = "�u�􂪂��������`���́`���`�΂��ڂ��`�`�v";
							break;
						  case 13:
							act = "�u��h�`�����h���~��������Ȃ���`�`�v";
							break;
						  case 14:
							act = "�u��悶����ۂ����Ɓ`�ȁ`�������ā`�v";
							break;
						  case 15:
							act = "�u�􂠂��`�́`�`�Ȃ����`�́`�܂��Ȃ����`�v";
							break;
						}
#else
						act = "horribly sings 'I AM GIAAAAAN. THE BOOOSS OF THE KIIIIDS.'";
#endif
					}
					else if(m_ptr->r_idx == MON_GACHAPIN)
					{
						if (one_in_(3))
#ifdef JP
							act = "�́��l��͊y�����Ƒ���Ɖ̂��Ă���B";
						else
							act = "�́�A�C ���u ���[�A���[ ���u �~�[��Ɖ̂��Ă���B";
#else
							act = "sings 'We are a happy family.'";
						else
							act = "sings 'I love you, you love me.'";
#endif
					}
					else
					{
							act = _("�̉̐������Ȃ����񂾁B", "sings in your direction.");
					}

					sound(SOUND_SHOW);
					break;
				}

				case RBM_SPEAR:
				{
#ifdef JP
					act = "�˂��ꂽ�B";
#else
					act = "thrusts at you.";
#endif

					do_cut = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_BOOK:
				{
#ifdef JP
					act = "�������{�ŉ���ꂽ�B";
#else
					act = "bludgeons you with a thick book.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_MOFUMOFU:
				{
#ifdef JP
					act = "�K���Ń��t���t���ꂽ�B";
#else
					act = "envelops you with fluffy tail.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_SQUEEZE:
				{
#ifdef JP
					act = "���ߕt����ꂽ�B";
#else
					act = "squeezes you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_DRAG:
				{
#ifdef JP
					act = "�������荞�܂ꂽ�B";
#else
					act = "drags you in.";
#endif

					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_INJECT:
				{
#ifdef JP
					act = "�����𒍎˂��ꂽ�B";
#else
					act = "injects you with something.";
#endif
					sound(SOUND_HIT);
					break;
				}
				case RBM_SMILE:
				{
#ifdef JP
					act = "���΂݂�����ꂽ�B";
#else
					act = "smiles at you.";
#endif

					sound(SOUND_HIT);
					break;
				}
				case RBM_HEADBUTT:
				{
#ifdef JP
					act = "���˂����ꂽ�B";
#else
					act = "headbutts you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_FLASH:
				{
#ifdef JP
					abbreviate = -1;
					act = "���������I";
#else
					act = "lets out a bright flash!";
#endif

					sound(SOUND_HIT);
					break;
				}
				case RBM_HOWL:
				{
#ifdef JP
					abbreviate = -1;
					act = "���i�����I";
#else
					act = "howls at you.";
#endif

					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}
				case RBM_WHISPER:
				{
#ifdef JP
					abbreviate = -1;
					act = "�������𚑂����B";
#else
					act = "whispers something.";
#endif

					sound(SOUND_HIT);
					break;
				}
				case RBM_PRESS:
				{
#ifdef JP
					act = "�����ׂ��ꂽ�B";
#else
					act = "presses you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_POINT:
				{
#ifdef JP
					abbreviate = -1;
					act = "�����Ȃ����w�������B";
#else
					act = "points at you.";
#endif

					sound(SOUND_HIT);
					break;
				}
				case RBM_STRIKE:
				{
#ifdef JP
					abbreviate = -1;
					act = "�����Ȃ���ł��������B";
#else
					act = "strikes you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}
				case RBM_PLAY:
				{
#ifdef JP
					abbreviate = -1;
					if(effect == RBE_STUN)
						act = "�̊y�킪������������I";
					else
						act = "�̊y�킪��ȉ��F��t�ł��B";
#else
					if(effect == RBE_STUN)
						act = "makes out a deafening sound!";
					else
						act = "plays a strange tune.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}


				case RBM_SMOKE: //v1.1.86
				{
					abbreviate = -1;
					act = _("�����Ȃ��ɉ��𐁂��t�����B", "blows smoke at you.");
					sound(SOUND_HIT);
					break;
				}


			}

			/* Message */
			if (act)
			{
				/*:::���o���A���b�Z�[�W��silly_attacks[]�̓��e�ɕύX�����*/
				if (do_silly_attack)
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = silly_attacks[randint0(MAX_SILLY_ATTACK)];
				}
#ifdef JP
				if (abbreviate == 0)
				    msg_format("%^s��%s", m_name, act);
				else if (abbreviate == 1)
				    msg_format("%s", act);
				else /* if (abbreviate == -1) */
				    msg_format("%^s%s", m_name, act);
				abbreviate = 1;/*�Q��ڈȍ~�͏ȗ� */
#else
				msg_format("%^s %s%s", m_name, act, do_silly_attack ? " you." : "");
#endif
			}

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);
			damage_stun = damage;

			/*
			 * Skip the effect when exploding, since the explosion
			 * already causes the effect.
			 */
			if (explode)
			{
				damage = 0;
				damage_stun = 0;
			}
			/* Apply appropriate damage */
			///sys mon �����X�^�[�ʏ�U�����s��
			switch (effect)
			{
				case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;
					damage_stun = 0;

					break;
				}

				case RBE_SUPERHURT:
				{
					/*:::�ɍ��̈ꌂ�łȂ��ꍇbreak����HURT�Ɠ�������*/
					if (((randint1(rlev*2+300) > (ac+200)) || one_in_(13)) && !CHECK_MULTISHADOW())
					{
						int tmp_damage = damage - (damage * ((ac < 150) ? ac : 150) / 250);
#ifdef JP
						msg_print("�ɍ��̈ꌂ�I");
#else
						msg_print("It was a critical hit!");
#endif

						tmp_damage = MAX(damage, tmp_damage*2);

						/* Take damage */
						get_damage += take_hit(DAMAGE_ATTACK, tmp_damage, ddesc, -1);
						break;
					}
				}
				case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);
					damage_stun = damage;

					///mod140530 ���ω����͕����U���_���[�W����
					if(p_ptr->mimic_form == MIMIC_MIST) damage /= 3;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					break;
				}

				/*:::�łɖ`���U����AC��ϐ��ɂ��З͌������Ȃ��炵��*/
				//v1.1.83 �ϐ��ɂ��З͌����K�p
				case RBE_POISON:
				{
					if(process_ringo_im(GF_POIS)) break;

					if (explode) break;
					if(p_ptr->pclass == CLASS_MEDICINE)
					{
						msg_print(_("�̂��łŖ��������B", "Your body gets filled with poison."));
						hp_player(damage / 3);
						set_food(p_ptr->food + damage*10);
						break;
					}


					//v1.1.83 �ϐ��ɂ��З͌����K�p
					//�i�Ԃ͓ŖƉu�����A�u�h���ēł�H��킷�v�݂����ȍU�����m�[�_���[�W�ɂȂ�͕̂ςȋC������̂ŏ����H�炤�悤�ɂ��Ƃ�
					if (p_ptr->pclass == CLASS_EIRIN)
					{
						damage /= 9;
						break;
					}
					else
					{
						if (p_ptr->resist_pois)
							damage = (damage * 2 + 2) / 5;
						if (IS_OPPOSE_POIS())
							damage = (damage * 2 + 2) / 5;

					}


					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()) && !CHECK_MULTISHADOW())
					{
						if (set_poisoned(p_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}
					if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + damage * 20);



					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_POIS);

					break;
				}

				case RBE_UN_BONUS:
				{
					if (explode) break;
					if(process_ringo_im(GF_DISENCHANT )) break;

					/* Allow complete resist */
					if (!p_ptr->resist_disen && !CHECK_MULTISHADOW())
					{
						/* Apply disenchantment */
						if (apply_disenchant(0,0))
						{
							/* Hack -- Update AC */
							update_stuff();
							obvious = TRUE;
						}
					}
					///mod140119 �򉻑Ō��͑ϐ��Ń_���[�W�������邱�Ƃɂ���
					damage = mod_disen_dam(damage);
					if(damage == 0) damage_stun = 0;
					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_DISEN);

					break;
				}
				/*:::���͂��z�����*/
				///system ���͂��z����鏈���@
				//v1.1.81 ���͏[�U�z���Ō��̎d�l�ύX
				case RBE_UN_POWER:
				{


					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Drain charged wands/staffs */
						//������\�͂�magicmaster�t���O�Œ�R�ł���悤�ɂ���
						if (((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_WAND)) &&(o_ptr->pval))
						{
							bool flag_resist = FALSE;
							int resist_pow;
							int drain_rasio;
							int drain_max;
							int drain_value;
							int mon_heal_hp;

							//��R�͌v�Z�ƒ�R�۔���
							if (cp_ptr->magicmaster)
								resist_pow = p_ptr->skill_dev - 50;
							else
								resist_pow = p_ptr->skill_dev - 100;

							if (resist_pow > randint1(r_ptr->level)) flag_resist = TRUE;

							//�z����񐔂̊���(%)
							if (flag_resist)
								drain_rasio = (100 + r_ptr->level + randint1(r_ptr->level)) / 5;
							else
								drain_rasio = 100;

							//���̃A�C�e���̊�{�ő�[�U��(�z����ő�l)
							drain_max = k_info[o_ptr->k_idx].pval;
							if (o_ptr->tval == TV_WAND) drain_max *= o_ptr->number;//���@�_�͕����{pval���L
							//�ߏ[�U�l��
							if (o_ptr->pval > drain_max) drain_max = o_ptr->pval;

							//�z�����(�Œ�1)
							drain_value = drain_max * drain_rasio / 100;
							if (drain_value > o_ptr->pval) drain_value = o_ptr->pval;
							if (drain_value < 1) drain_value = 1;

							//�A�C�e���̎g�p�񐔌�������
							msg_format(_("�U�b�N����G�l���M�[��%s�z�����ꂽ�I",
                                        "Energy %sdrains from your pack!"),flag_resist? _("����", "slightly "):"");
							o_ptr->pval -= drain_value;
							if (o_ptr->pval < 0) o_ptr->pval=0; //paranoia
							obvious = TRUE;
							p_ptr->notice |= (PN_COMBINE | PN_REORDER);
							p_ptr->window |= (PW_INVEN);

							//�����X�^�[��HP�񕜏���
							//m_ptr->hp�𒼐ڑ��₵����I�[�o�[�t���[����̂�mon_heal_hp�o�R
							mon_heal_hp = drain_value * r_ptr->level;
							if (o_ptr->tval == TV_STAFF) mon_heal_hp *= o_ptr->number;//��͕����{pval�񋤗L
							if (cheat_xtra)	msg_format("pow:%d rasio:%d max:%d value:%d heal:%d", resist_pow,drain_rasio, drain_max, drain_value, mon_heal_hp);
							if (mon_heal_hp > (m_ptr->maxhp - m_ptr->hp)) mon_heal_hp = m_ptr->maxhp - m_ptr->hp;
							if (cheat_xtra) msg_format("heal2:%d", mon_heal_hp);
							m_ptr->hp += mon_heal_hp;
							if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);


							break;
						}

#if 0
						/* Drain charged wands/staffs */
						if (((o_ptr->tval == TV_STAFF) ||
						     (o_ptr->tval == TV_WAND)) &&
						    (o_ptr->pval))
						{
							/* Calculate healed hitpoints */
							int heal=rlev * o_ptr->pval;
							if( o_ptr->tval == TV_STAFF)
							    heal *=  o_ptr->number;

							/* Don't heal more than max hp */
							heal = MIN(heal, m_ptr->maxhp - m_ptr->hp);

							msg_print("�U�b�N����G�l���M�[���z�����ꂽ�I");

							/* Obvious */
							obvious = TRUE;

							/* Heal the monster */
							m_ptr->hp += heal;

							/* Redraw (later) if needed */
							if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

							/* Uncharge */
							o_ptr->pval = 0;

							/* Combine / Reorder the pack */
							p_ptr->notice |= (PN_COMBINE | PN_REORDER);

							/* Window stuff */
							p_ptr->window |= (PW_INVEN);

							/* Done */
							break;
						}
#endif
					}

					break;
				}

				case RBE_EAT_GOLD:
				{
					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (MON_CONFUSED(m_ptr)) break;

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Obvious */
					obvious = TRUE;

					//v1.1.48
					if (p_ptr->au < 1)
					{
						msg_print(_("�D�_�͂��Ȃ��̉��ɋ����������Ȃ������B",
                                    "The thief has no interest in your possessions."));
						break;
					}

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] + p_ptr->lev))
						|| CHECK_FAIRY_TYPE == 9
						|| count_ability_card(ABL_CARD_SYUSENDO)		)
					{
						/* Saving throw message */
#ifdef JP
						msg_print("�������f�������z��������I");
#else
						msg_print("You quickly protect your money pouch!");
#endif


						/* Occasional blink anyway */
						if (randint0(3)) blinked = TRUE;
					}

					/* Eat gold */
					else
					{

						gold = (p_ptr->au / 10) + randint1(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (p_ptr->au / 20) + randint1(3000);
						if (gold > p_ptr->au) gold = p_ptr->au;
						p_ptr->au -= gold;
						if (gold <= 0)
						{
#ifdef JP
							msg_print("�������������܂�Ȃ������B");
#else
							msg_print("Nothing was stolen.");
#endif

						}
						else if (p_ptr->au)
						{
#ifdef JP
							msg_print("���z���y���Ȃ����C������B");
							msg_format("$%ld �̂��������܂ꂽ�I", (long)gold);
#else
							msg_print("Your purse feels lighter.");
							msg_format("%ld coins were stolen!", (long)gold);
#endif
							chg_virtue(V_SACRIFICE, 1);
						}
						else
						{
#ifdef JP
							msg_print("���z���y���Ȃ����C������B");
							msg_print("�������S�����܂ꂽ�I");
#else
							msg_print("Your purse feels lighter.");
							msg_print("All of your coins were stolen!");
#endif

							chg_virtue(V_SACRIFICE, 2);
						}

						/* Redraw gold */
						p_ptr->redraw |= (PR_GOLD);

						/* Window stuff */
						p_ptr->window |= (PW_PLAYER);

						/* Blink away */
						blinked = TRUE;
					}

					break;
				}

				case RBE_EAT_ITEM:
				{
					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (MON_CONFUSED(m_ptr)) break;

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
							      p_ptr->lev))
						|| count_ability_card(ABL_CARD_SYUSENDO))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("����������ĂăU�b�N�����Ԃ����I");
#else
						msg_print("You grab hold of your backpack!");
#endif


						/* Occasional "blink" anyway */
						blinked = TRUE;

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						s16b o_idx;

						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (object_is_artifact(o_ptr)) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

						/* Message */
#ifdef JP
						msg_format("%s(%c)��%s���܂ꂽ�I",
							   o_name, index_to_label(i),
							   ((o_ptr->number > 1) ? "���" : ""));
#else
						msg_format("%sour %s (%c) was stolen!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(i));
#endif

						chg_virtue(V_SACRIFICE, 1);


						/* Make an object */
						o_idx = o_pop();

						/* Success */
						if (o_idx)
						{
							object_type *j_ptr;

							/* Get new object */
							j_ptr = &o_list[o_idx];

							/* Copy object */
							object_copy(j_ptr, o_ptr);

							/* Modify number */
							j_ptr->number = 1;

							/* Hack -- If a rod or wand, allocate total
							 * maximum timeouts or charges between those
							 * stolen and those missed. -LM-
							 */
							if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
							{
								j_ptr->pval = o_ptr->pval / o_ptr->number;
								o_ptr->pval -= j_ptr->pval;
							}

							/* Forget mark */
							j_ptr->marked = OM_TOUCHED;

							/* Memorize monster */
							j_ptr->held_m_idx = m_idx;

							/* Build stack */
							j_ptr->next_o_idx = m_ptr->hold_o_idx;

							/* Build stack */
							m_ptr->hold_o_idx = o_idx;
						}

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Blink away */
						blinked = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_FOOD:
				{
					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = randint0(INVEN_PACK);

						/* Get the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						///del131221 ����
						//if ((o_ptr->tval != TV_FOOD) && !((o_ptr->tval == TV_CORPSE) && (o_ptr->sval))) continue;
						///item �H�ו��i�G���H�ׂ�j����
						///mod131223
						if (o_ptr->tval != TV_FOOD && o_ptr->tval != TV_SWEETS && o_ptr->tval != TV_MUSHROOM) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

						/* Message */
#ifdef JP
						msg_format("%s(%c)��%s�H�ׂ��Ă��܂����I",
							  o_name, index_to_label(i),
							  ((o_ptr->number > 1) ? "���" : ""));
#else
						msg_format("%sour %s (%c) was eaten!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(i));
#endif


						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_LITE:
				{
					/* Access the lite */
					o_ptr = &inventory[INVEN_LITE];

					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Drain fuel */
					if ((o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(o_ptr)))
					{
						/* Reduce fuel */
						o_ptr->xtra4 -= (s16b)(250 + randint1(250));
						if (o_ptr->xtra4 < 1) o_ptr->xtra4 = 1;

						/* Notice */
						if (!p_ptr->blind)
						{
#ifdef JP
							msg_print("�����肪�Â��Ȃ��Ă��܂����B");
#else
							msg_print("Your light dims.");
#endif

							obvious = TRUE;
						}

						/* Window stuff */
						p_ptr->window |= (PW_EQUIP);
					}

					break;
				}

				case RBE_ACID:
				{
					if (explode) break;
					if(process_ringo_im(GF_ACID)) break;

					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("�_�𗁂т���ꂽ�I");
#else
					msg_print("You are covered in acid!");
#endif


					/* Special damage */
					get_damage += acid_dam(damage, ddesc, -1);

					/* Hack -- Update AC */
					update_stuff();

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ACID);

					break;
				}

				case RBE_ELEC:
				{
					if (explode) break;
					if(process_ringo_im(GF_ELEC )) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("�d���𗁂т���ꂽ�I");
#else
					msg_print("You are struck by electricity!");
#endif


					/* Special damage */
					get_damage += elec_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ELEC);

					break;
				}

				case RBE_FIRE:
				{
					if (explode) break;
					if(process_ringo_im(GF_FIRE)) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("�S�g�����ɕ�܂ꂽ�I");
#else
					msg_print("You are enveloped in flames!");
#endif


					/* Special damage */
					get_damage += fire_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FIRE);

					break;
				}

				case RBE_COLD:
				{
					if (explode) break;
					if(process_ringo_im(GF_COLD )) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("�S�g����C�ŕ���ꂽ�I");
#else
					msg_print("You are covered with frost!");
#endif


					/* Special damage */
					get_damage += cold_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_COLD);

					break;
				}

				case RBE_BLIND:
				{

					//v1.1.83 �Ӗڑϐ��ňЗ͌���
					if (p_ptr->resist_blind)
						damage = damage * (randint1(4) + 3) / 8;


					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "blind" */
					if (!p_ptr->resist_blind && !CHECK_MULTISHADOW())
					{
						if (set_blind(p_ptr->blind + 10 + randint1(rlev)))
						{
#ifdef JP
							if (m_ptr->r_idx == MON_DIO) msg_print("�ǂ����b�I���̌��̖ڒׂ��̓b�I");
#else
							/* nanka */
#endif
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_BLIND);

					break;
				}

				case RBE_CONFUSE:
				{
					if (explode) break;
					/* Take damage */

					//v1.1.83 �����ϐ��ňЗ͌���
					if (p_ptr->resist_conf)
						damage = damage * (randint1(4) + 3) / 8;

					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "confused" */
					if (!p_ptr->resist_conf && !CHECK_MULTISHADOW())
					{
						if (set_confused(p_ptr->confused + 3 + randint1(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_CONF);

					break;
				}

				case RBE_TERRIFY:
				{

					//v1.1.83 ���|�ϐ��ňЗ͌���
					if (p_ptr->resist_fear)
						damage = damage * (randint1(4) + 3) / 8;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "afraid" */
					if (CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					//v1.1.37 ���鋰�|��_����
					else if (p_ptr->resist_fear && !IS_VULN_FEAR)
					{
#ifdef JP
						msg_print("���������|�ɐN����Ȃ������I");
#else
						msg_print("You stand your ground!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < (p_ptr->skill_sav * (p_ptr->resist_fear ? 2 : 1 )))
					{
#ifdef JP
						msg_print("���������|�ɐN����Ȃ������I");
#else
						msg_print("You stand your ground!");
#endif

						obvious = TRUE;
					}
					else
					{
						if (set_afraid(p_ptr->afraid + 3 + randint1(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FEAR);

					break;
				}

				case RBE_PARALYZE:
				{
					if (explode) break;

					//v1.1.83 ��ბϐ��ňЗ͌���
					if (p_ptr->free_act)
						damage = damage * (randint1(4) + 3) / 8;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "paralyzed" */
					if (CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					else if (p_ptr->free_act)
					{
#ifdef JP
						msg_print("���������ʂ��Ȃ������I");
#else
						msg_print("You are unaffected!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < p_ptr->skill_sav)
					{
#ifdef JP
						msg_print("���������͂𒵂˕Ԃ����I");
#else
						msg_print("You resist the effects!");
#endif

						obvious = TRUE;
					}
					else
					{
						if (!p_ptr->paralyzed)
						{
							if (set_paralyzed(3 + randint1(rlev)))
							{
								obvious = TRUE;
							}
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FREE);

					break;
				}

				case RBE_LOSE_STR:
				{

					//v1.1.83 �ێ��ňЗ͌���
					if(p_ptr->sustain_str)
						damage = damage * (randint1(4) + 4) / 9;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_STR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_INT:
				{
					//v1.1.83 �ێ��ňЗ͌���
					if (p_ptr->sustain_int)
						damage = damage * (randint1(4) + 4) / 9;


					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_INT)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_WIS:
				{
					//v1.1.83 �ێ��ňЗ͌���
					if (p_ptr->sustain_wis)
						damage = damage * (randint1(4) + 4) / 9;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_WIS)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_DEX:
				{
					//v1.1.83 �ێ��ňЗ͌���
					if (p_ptr->sustain_dex)
						damage = damage * (randint1(4) + 4) / 9;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_DEX)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CON:
				{
					//v1.1.83 �ێ��ňЗ͌���
					if (p_ptr->sustain_con)
						damage = damage * (randint1(4) + 4) / 9;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_CON)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CHR:
				{
					//v1.1.83 �ێ��ňЗ͌���
					if (p_ptr->sustain_chr)
						damage = damage * (randint1(4) + 4) / 9;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_ALL:
				{
					//v1.1.83 �ێ��ЂƂɂ�(1d4+3)%�y��
					int dam_dec_count =0;

					if (p_ptr->sustain_str) dam_dec_count++;
					if (p_ptr->sustain_int) dam_dec_count++;
					if (p_ptr->sustain_wis) dam_dec_count++;
					if (p_ptr->sustain_dex) dam_dec_count++;
					if (p_ptr->sustain_con) dam_dec_count++;
					if (p_ptr->sustain_chr) dam_dec_count++;

					if (dam_dec_count)
					{
						damage = damage * (100 - (randint1(4)+3)*dam_dec_count) / 100;

					}



					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stats) */
					if (do_dec_stat(A_STR)) obvious = TRUE;
					if (do_dec_stat(A_DEX)) obvious = TRUE;
					if (do_dec_stat(A_CON)) obvious = TRUE;
					if (do_dec_stat(A_INT)) obvious = TRUE;
					if (do_dec_stat(A_WIS)) obvious = TRUE;
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);
					damage_stun = damage;

					//���ω����Ɍy��
					if(p_ptr->mimic_form == MIMIC_MIST) damage /= 3;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23 || explode)
					{
						earthquake_aux(m_ptr->fy, m_ptr->fx, 8, m_idx);
					}

					break;
				}

				case RBE_EXP_10:
				{
					s32b d = damroll(10, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					//v1.1.83 �n���ϐ��ňЗ͌���
					if (p_ptr->prace == RACE_ANDROID)
						damage = damage * 2 / 3;
					else if (p_ptr->resist_neth)
						damage = damage * (randint1(4) + 10) / 16;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 95);
					break;
				}

				case RBE_EXP_20:
				{
					s32b d = damroll(20, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					//v1.1.83 �n���ϐ��ňЗ͌���
					if (p_ptr->prace == RACE_ANDROID)
						damage = damage * 2 / 3;
					else if (p_ptr->resist_neth)
						damage = damage * (randint1(4) + 10) / 16;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 90);
					break;
				}

				case RBE_EXP_40:
				{
					s32b d = damroll(40, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
					//v1.1.83 �n���ϐ��ňЗ͌���
					if (p_ptr->prace == RACE_ANDROID)
						damage = damage * 2 / 3;
					else if (p_ptr->resist_neth)
						damage = damage * (randint1(4) + 10) / 16;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 75);
					break;
				}

				case RBE_EXP_80:
				{
					s32b d = damroll(80, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
					//v1.1.83 �n���ϐ��ňЗ͌���
					if (p_ptr->prace == RACE_ANDROID)
						damage = damage * 2 / 3;
					else if (p_ptr->resist_neth)
						damage = damage * (randint1(4) + 10) / 16;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 50);
					break;
				}

				case RBE_DISEASE:
				{

					//v1.1.83 �őϐ��ňЗ͌���
					if (p_ptr->pclass == CLASS_MEDICINE || p_ptr->pclass == CLASS_EIRIN || p_ptr->prace == RACE_HOURAI)
						damage = damage / 9;
					else
					{
						if (p_ptr->resist_pois)
							damage = damage * (randint1(4) + 4) / 9;

						if (IS_OPPOSE_POIS())
							damage = damage * (randint1(4) + 4) / 9;
					}


					/* Take some damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
					{
						if (set_poisoned(p_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}
					if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + damage * 20);

					/* Damage CON (10% chance)*/
					if ((randint1(100) < 11) && (p_ptr->prace != RACE_ANDROID) && (p_ptr->prace != RACE_HANIWA) && (p_ptr->prace != RACE_TSUKUMO) && (p_ptr->prace != RACE_HOURAI))
					{
						/* 1% chance for perm. damage */
						bool perm = one_in_(10);
						if (dec_stat(A_CON, randint1(10), perm))
						{
#ifdef JP
							msg_print("�a�����Ȃ���I��ł���C������B");
#else
							msg_print("You feel strange sickness.");
#endif

							obvious = TRUE;
						}
					}

					break;
				}
				case RBE_TIME:
				{
					if (explode) break;
					if(process_ringo_im(GF_TIME)) break;

					//v1.1.83 ����ϐ��ňЗ͌���
					if (p_ptr->resist_time)
						damage = damage * (randint1(4) + 4) / 9;


					if (!p_ptr->resist_time && !CHECK_MULTISHADOW())
					{
						switch (randint1(10))
						{
							case 1: case 2: case 3: case 4: case 5:
							{
								if (p_ptr->prace == RACE_ANDROID) break;
#ifdef JP
								msg_print("�l�����t�߂肵���C������B");
#else
								msg_print("You feel life has clocked back.");
#endif

								lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

							case 6: case 7: case 8: case 9:
							{
								int stat = randint0(6);

								switch (stat)
								{
#ifdef JP
									case A_STR: act = "����"; break;
									case A_INT: act = "������"; break;
									case A_WIS: act = "������"; break;
									case A_DEX: act = "��p��"; break;
									case A_CON: act = "���N��"; break;
									case A_CHR: act = "������"; break;
#else
									case A_STR: act = "strong"; break;
									case A_INT: act = "bright"; break;
									case A_WIS: act = "wise"; break;
									case A_DEX: act = "agile"; break;
									case A_CON: act = "hale"; break;
									case A_CHR: act = "beautiful"; break;
#endif

								}

#ifdef JP
								msg_format("���Ȃ��͈ȑO�ق�%s�Ȃ��Ȃ��Ă��܂���...�B", act);
#else
								msg_format("You're not as %s as you used to be...", act);
#endif


								p_ptr->stat_cur[stat] = (p_ptr->stat_cur[stat] * 3) / 4;
								if (p_ptr->stat_cur[stat] < 3) p_ptr->stat_cur[stat] = 3;
								p_ptr->update |= (PU_BONUS);
								break;
							}

							case 10:
							{
#ifdef JP
						msg_print("���Ȃ��͈ȑO�قǗ͋����Ȃ��Ȃ��Ă��܂���...�B");
#else
								msg_print("You're not as powerful as you used to be...");
#endif


								for (k = 0; k < 6; k++)
								{
									p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 7) / 8;
									if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
								}
								p_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

				update_smart_learn(m_idx, DRS_TIME);
				break;
				}
				///sys mon �z���U��
				case RBE_EXP_VAMP:
				{
					s32b d = damroll(60, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
					bool flag_no_heal_enemy;

					/* Obvious */
					obvious = TRUE;

					flag_no_heal_enemy = !drain_exp(d, d / 10, 50);

					//v1.1.83 �n���ϐ���z�������푰�Ōy��
					if (p_ptr->mimic_form)
					{
						if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)
						{
							flag_no_heal_enemy = TRUE;
							damage = damage * 2 / 3;
						}
					}
					else if(RACE_NO_DRAIN)
					{
						flag_no_heal_enemy = TRUE;
						damage = damage * 2 / 3;
					}
					else if (p_ptr->resist_neth)
					{
						damage = damage * (randint1(4) + 10) / 16;
					}

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;



					if ((damage > 5) && !flag_no_heal_enemy)
					{
						bool did_heal = FALSE;

						if (m_ptr->hp < m_ptr->maxhp) did_heal = TRUE;

						/* Heal */
						m_ptr->hp += damroll(4, damage / 6);
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (m_ptr->ml && did_heal)
						{
#ifdef JP
msg_format("%s�̗͑͂��񕜂����悤���B", m_name);
#else
							msg_format("%^s appears healthier.", m_name);
#endif

						}
					}

					break;
				}
				case RBE_DR_MANA:
				{
					/* Obvious */
					obvious = TRUE;

					if (CHECK_MULTISHADOW())
					{
#ifdef JP
						msg_print("�U���͌��e�ɖ������A���Ȃ��ɂ͓͂��Ȃ������B");
#else
						msg_print("The attack hits Shadow, you are unharmed!");
#endif
					}
					else
					{
						do_cut = 0;

						if(!IS_INVULN())
						{
							int kaguya_card_num = count_ability_card(ABL_CARD_KAGUYA);

							if (kaguya_card_num)
							{
								int percen = calc_ability_card_prob(ABL_CARD_KAGUYA, kaguya_card_num);
								if (percen >= 100 )break;

								damage -= damage * percen / 100;

							}


							/* Take damage */
							p_ptr->csp -= damage;
							if (p_ptr->csp < 0)
							{
								p_ptr->csp = 0;
								p_ptr->csp_frac = 0;
							}
							p_ptr->redraw |= (PR_MANA);
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_MANA);

					break;
				}
				case RBE_INERTIA:
				{
					if(process_ringo_im(GF_INACT)) break;

					if (p_ptr->pclass == CLASS_SAKI) damage = (damage + 1) / 2;

					//v1.1.83 �ꎞ�����������͉���+20�ȏ�Ōy��
					else if(p_ptr->fast || p_ptr->pspeed >= 130)
						damage = damage * (randint1(4) + 4) / 9;

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Decrease speed */
					if (CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					else
					{
						if (set_slow((p_ptr->slow + 4 + randint0(rlev / 10)), FALSE))
						{
							obvious = TRUE;
						}
					}

					break;
				}
				case RBE_STUN:
				{
					//v1.1.83 �����ϐ��Ōy��
					if (p_ptr->resist_sound)
					{
						damage = damage * (randint1(4) + 10) / 16;
					}

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if(process_ringo_im(GF_SOUND)) break;

					if (p_ptr->is_dead) break;

					/* Decrease speed */
					if (p_ptr->resist_sound || CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					else
					{
						if (set_stun(p_ptr->stun + 10 + randint1(r_ptr->level / 4)))
						{
							obvious = TRUE;
						}
					}
					update_smart_learn(m_idx, DRS_SOUND);

					break;
				}
				///mod131230 �����X�^�[�V�U���ǉ�

				///mod131230 �J�I�X�����ʏ�U�� ����J�I�X�ϐ�������ƃ_���[�W�ȊO�����N����Ȃ�
				case RBE_CHAOS:
				{
					if(process_ringo_im(GF_CHAOS)) break;

					if (p_ptr->resist_chaos) damage = damage * 2 / 3;
					if (damage == 0) break;

					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					if (p_ptr->resist_chaos) break;
					obvious = TRUE;

					if (!p_ptr->resist_conf)
						(void)set_confused(p_ptr->confused + randint0(20) + 10);
					(void)set_image(p_ptr->image + randint1(10));
					if (one_in_(3))
					{
#ifdef JP
						msg_print("���Ȃ��̐g�̂̓J�I�X�̗͂ŔP���Ȃ���ꂽ�I");
#else
						msg_print("Your body is twisted by chaos!");
#endif
						(void)gain_random_mutation(0);
					}
					if (!p_ptr->resist_neth)
						drain_exp(5000 + (p_ptr->exp / 100), 500 + (p_ptr->exp / 1000), 75);
					inven_damage(set_elec_destroy, 2);
					inven_damage(set_fire_destroy, 2);
					update_smart_learn(m_idx, DRS_CONF);
					update_smart_learn(m_idx, DRS_CHAOS);

					break;
				}
				///mod131230 �G�������g�Ō�
				case RBE_ELEMENT:
				{
					obvious = TRUE;

					if(one_in_(4))
					{
						if(process_ringo_im(GF_ACID)) break;

#ifdef JP
						msg_print("�_�𗁂т���ꂽ�I");
#else
						msg_print("You are covered in acid!");
#endif
						get_damage += acid_dam(damage, ddesc, -1);
						update_stuff();
						update_smart_learn(m_idx, DRS_ACID);
					}
					else if(one_in_(3))
					{
						if(process_ringo_im(GF_ELEC )) break;
#ifdef JP
						msg_print("�d���𗁂т���ꂽ�I");
#else
						msg_print("You are struck by electricity!");
#endif
						get_damage += elec_dam(damage, ddesc, -1);
						update_smart_learn(m_idx, DRS_ELEC);
					}
					else if(one_in_(2))
					{
						if(process_ringo_im(GF_FIRE )) break;
#ifdef JP
						msg_print("�S�g�����ɕ�܂ꂽ�I");
#else
						msg_print("You are enveloped in flames!");
#endif
						get_damage += fire_dam(damage, ddesc, -1);
						update_smart_learn(m_idx, DRS_FIRE);
					}
					else
					{
						if(process_ringo_im(GF_COLD)) break;

#ifdef JP
						msg_print("�S�g����C�ŕ���ꂽ�I");
#else
						msg_print("You are covered with frost!");
#endif
						get_damage += cold_dam(damage, ddesc, -1);
						update_smart_learn(m_idx, DRS_COLD);
					}
					break;
				}
				///mod131230 �ˑR�ψٗU���Ō� �ŁA�J�I�X�ϐ��ł��ꂼ��90%������
				case RBE_MUTATE:
				{
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;
					if(m_ptr->r_idx != MON_ASHIURI && p_ptr->resist_pois && !one_in_(10))break;
					if(p_ptr->resist_chaos && !one_in_(10))break;
					obvious = TRUE;

					///mod160202 ������k�́u���O�ꂽ�r�́v�u�悭�]�ԁv�̂ǂ��炩�̕ψق�t������B�ǂ��炩�����Ɏ����Ă���A���邢�͐l���̔����j���Â��Ɖ������Ȃ��B
					if(m_ptr->r_idx == MON_ASHIURI)
					{
						if(!(p_ptr->muta2 & MUT2_DISARM) && !(p_ptr->muta3 & MUT3_SPEEDSTER) && !(p_ptr->muta3 & MUT3_FISH_TAIL) && p_ptr->pclass != CLASS_TOZIKO)
						{
							if(one_in_(3)) gain_random_mutation(138);
							else gain_random_mutation(119);
						}
					}
					else
					{
						(void)gain_random_mutation(0);
					}

					break;
				}
				///mod140103 �򉻑Ō� �j�בϐ��̏�ԂŃ_���[�W����������悤�ɂ���
				case RBE_SMITE:
				{
					if(process_ringo_im(GF_HOLY_FIRE)) break;
					damage = mod_holy_dam(damage);
					if(damage == 0) damage_stun = 0;
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					update_smart_learn(m_idx, DRS_HOLY);
					break;

				}
				//mod131230 �M�ꂳ����Ō�
				case RBE_DROWN:
				{
					if(process_ringo_im(GF_WATER)) break;
					//if(p_ptr->resist_water) damage /= 2;
					damage = mod_water_dam(damage);
					if(damage==0)
					{
						damage_stun = 0;
						break;
					}
					engineer_malfunction(GF_WATER, damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					if ( p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0])
					{
						msg_print(_("�̂��G��Ď���������Ă��܂����I", "You lose your shikigami power as your body gets wet!"));
						p_ptr->magic_num1[0] = 1;
					}

					if(p_ptr->resist_water) break;
					obvious = TRUE;
					msg_print(_("�M�ꂳ����ꂽ�I", "You are being drowned!"));
					set_stun(p_ptr->stun + 10 + randint1(r_ptr->level / 4));
					update_smart_learn(m_idx, DRS_WATER);

					break;
				}
				//mod131230 ���ɗU���Ō� �����^�[���ɓ�񂱂̍U��������G�����ƘA���s���ő������鋭���Ȃ̂ł��������������悤���H
				case RBE_KILL:
				{
					if(p_ptr->pclass == CLASS_HINA)
					{
						msg_print(_("���Ȃ��͍U�����Ƃ��ċz�������B", "You absorb the attack as misfortune."));
						hina_gain_yaku(damage);
						break;
					}
					if(process_ringo_im(GF_NETHER)) break;

					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;
					if(p_ptr->resist_neth) break;
					obvious = TRUE;
					msg_print(_("�ӎ������̂��Ă����c", "Your consciousness starts to fade..."));
					set_stun(p_ptr->stun + 20 + randint1(r_ptr->level / 3));
					update_smart_learn(m_idx, DRS_NETH);

					break;
				}
				///mod131231 �􂢑Ō� �Í��ϐ��A�n���ϐ��A�Z�[�r���O�X���[�̃`�F�b�N�㑕�����G���x���ɉ������m���Ŏ����
				case RBE_CURSE:
				{
					if(p_ptr->pclass == CLASS_HINA)
					{
						msg_print(_("���Ȃ��͍U�����Ƃ��ċz�������B", "You absorb the attack as misfortune."));
						hina_gain_yaku(damage);
						break;
					}
					if (p_ptr->pclass == CLASS_SHION)
					{
						msg_print(_("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B",
                                    "You absorb the attack as misfortune power."));
						p_ptr->magic_num1[1] += damage * 10;
						break;
					}

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;
					if(p_ptr->resist_dark && one_in_(2))break;
					if(p_ptr->resist_neth && one_in_(2))break;
					if (saving_throw(p_ptr->skill_sav)) break;
					obvious = TRUE;
					curse_equipment(MIN(99,rlev), MAX(0,(rlev-50)));
					break;
				}
				///pend ���j�[�N�N���X�̂Ƃ��B��ʐ^���Y�������X�^�[�ɂ��悤���H�����������ʓ|�B
				//monster_is_you���g���ɂ��Ă��v���Y�����o�[�o���Ȃǂ�����ƈ�H�v�v��B
				case RBE_PHOTO:
				{

					object_type *q_ptr;
					object_type forge;
					s16b o_idx=0;
					s16b tmp_idx;
					int cnt=0;
					q_ptr = &forge;

					/*:::�܂��_���[�W����*/
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					/*:::�p�p���b�`��������t���O*/
					if (randint0(3)) blinked_photo = TRUE;

					/*:::99���ȏ�B��Ȃ�*/
					for (tmp_idx = m_ptr->hold_o_idx; tmp_idx; tmp_idx = o_list[tmp_idx].next_o_idx)cnt++;
					if(cnt>=99) break;

					/*:::�ʐ^�𐶐��������X�^�[�̏����i�ɉ�����*/
					msg_print(_("�ʐ^���B��ꂽ�I", "Your photo was taken!"));
					object_prep(q_ptr, lookup_kind(TV_PHOTO, 0));
					q_ptr->pval = 0;



					q_ptr->ident |= (IDENT_MENTAL);
					//(void)drop_near(q_ptr, -1, py, px);
					o_idx = o_pop();
					q_ptr->held_m_idx = m_idx;
					q_ptr->next_o_idx = m_ptr->hold_o_idx;
					m_ptr->hold_o_idx = o_idx;
					object_copy(&o_list[o_idx], q_ptr);
					//excise_object_idx(o_idx);

					/* Damage (physical) */
					//if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;
					break;
				}
				///pend �z���Ō��@�ڍז��쐬
				case RBE_MELT:
				{
					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					break;
				}
				///mod131231 ���Ō� �f���Ƀ_���[�W�����ɉ��Z���Ă݂�
				case RBE_BLEED:
				{

					///mod140530 ���Ō���HURT�Ɠ��l��AC�ňЗ͌y���B�܂����ω����͕����U���_���[�W����
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);
					if(p_ptr->mimic_form == MIMIC_MIST) damage /= 3;

					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)set_cut(p_ptr->cut + damage);
					break;
				}
				///mod131231 ���C�Ō� �ϐ��Ȃ���sanityblast
				case RBE_INSANITY:
				{
					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;
					if(p_ptr->resist_insanity) break;
					obvious = TRUE;
					sanity_blast(0, TRUE);
				//	update_smart_learn(m_idx, DRS_INSANITY);
					break;
				}
				//v1.1.32 �󕠑Ō�
				case RBE_HUNGER:
				{
					/* Damage (physical) */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);
					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					msg_print(_("�ˑR�󕠊��ɏP��ꂽ�I", "You suddenly feel hungry!"));
					set_food(p_ptr->food - MIN(p_ptr->food/3,3000));

					obvious = TRUE;
					break;
				}

			}

			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (randint0(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 5; break;
					case 3: k = randint1(20) + 20; break;
					case 4: k = randint1(50) + 50; break;
					case 5: k = randint1(100) + 100; break;
					case 6: k = 300; break;
					default: k = 500; break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(p_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				//tmp = monster_critical(d_dice, d_side, damage);

				///mod140626 �_�C�X��U������_���[�W���������鏈���̏ꍇ�i�j�׎�_�Łu�����ď򉻂���v���󂯂�Ȃǁj�ꍇ�ɂ���Ă͈ꔭ�ňӎ��s�����ɂȂ肩�˂Ȃ��̂ŕ␳�B
				///�s�{�ӂȂ���X�^������p��damage�l��������B�ϐ���y���Ō����͂��Ă������͂��Ȃ��B
				tmp = monster_critical(d_dice, d_side, damage_stun);


				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 10; break;
					case 3: k = randint1(10) + 20; break;
					case 4: k = randint1(15) + 30; break;
					case 5: k = randint1(20) + 40; break;
					case 6: k = 80; break;
					default: k = 150; break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(p_ptr->stun + k);
			}
			/*:::�������������X�^�[�����S����*/
			if (explode)
			{
				sound(SOUND_EXPLODE);

				if (mon_take_hit(m_idx, m_ptr->hp + 1, &fear, NULL))
				{
					blinked = FALSE;
					blinked_photo = FALSE;
						alive = FALSE;
				}
			}
			/*:::�����X�^�[���I�[���_���[�W���󂯂�*/
			if (touched)
			{
				//�����l
				if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO && alive && !p_ptr->is_dead && !(r_ptr->flagsr & RFR_RES_ALL))
				{
						int dam = damroll(6, 6);
						if(!(r_ptr->flagsr & RFR_IM_FIRE)) dam *= 3;
						if(r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
						dam = mon_damage_mod(m_ptr, dam, FALSE);
						msg_format(_("%^s�͐_�̉΂ɏĂ��ꂽ�I",
                                    "%^s is scorched by the divine flame!"), m_name);
						if (mon_take_hit(m_idx, dam, &fear,_("�͓|�ꂽ�B", " is defeated.")))

						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
				}
				///mod140907 ���f�B�X����Lev25�œŃI�[��
				else if (p_ptr->pclass == CLASS_MEDICINE && p_ptr->lev > 24 && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS)))
					{
						int dam = damroll(3, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);
						msg_format(_("%^s�͓ł��󂯂��I", "%^s is hit by poison!"), m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    _("�͓|�ꂽ�B", " is defeated.")))
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
				}
				else if(p_ptr->pclass == CLASS_HINA && alive && !p_ptr->is_dead)
				{
					int yaku = p_ptr->magic_num1[0];
					int sides =  yaku / 100;

					if(sides && !(r_ptr->flagsr & RFR_RES_ALL))
					{
						int dam = randint1(sides);

						msg_format(_("%^s�͖���󂯂��I", "%^s is afflicted by misfortune!"), m_name);
						if (mon_take_hit(m_idx, dam, &fear,_("�͓|�ꂽ�B", " is defeated.")))
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
						else
						{
							if (yaku > 10000 && (randint1(yaku / 100) > r_ptr->level) && !(r_ptr->flags3 & RF3_NO_CONF))
							{
								msg_format(_("%^s�͂ӂ���Ă���c", "%^s staggers..."), m_name);
								(void)set_monster_confused(m_idx,  MON_CONFUSED(m_ptr) + randint1(yaku / 1000));
							}
							if (yaku > 20000 && randint1(yaku / 100) > r_ptr->level && !(r_ptr->flags3 & RF3_NO_STUN))
							{
								msg_format(_("%^s�͞N�O�Ƃ��Ă���B", "%^s is stunned."), m_name);
								(void)set_monster_stunned(m_idx,  MON_STUNNED(m_ptr) + randint1(yaku / 1500));
							}
						}
						hina_gain_yaku(-(sides / 50 + 1));//�I�[�������ł͂��܂�������Ȃ�
					}
				}

				//俎q�A�[�o���T�C�R�L�l�V�X
				else if(p_ptr->pclass == CLASS_SUMIREKO && alive && !p_ptr->is_dead)
				{
					int sides = 0;
					int j;
					for(j=0;j<7;j++)
					{
						if(!inven_add[j].k_idx) continue;
						sides += inven_add[j].weight / 5;
						sides += inven_add[j].ac / 10;
						//v1.1.27 �ߐڕ���݂̂ɂ���
						if(object_is_melee_weapon(&inven_add[j]))
							sides += damroll(inven_add[j].dd + 1, inven_add[j].ds + 1);
					}
					if(sides)
					{
						int dam = randint1(sides);
						dam = mon_damage_mod(m_ptr, dam, FALSE);
						msg_format(_("���Ȃ��̎������]����A�C�e����%^s�ɓ��������I",
                                    "%^s is hit by items flying around you!"), m_name);

						if (mon_take_hit(m_idx, dam, &fear,_("�͓|�ꂽ�B", " is defeated.")))
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}

					}
				}

				if (p_ptr->sh_fire && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK))
					{
						int dam = damroll(2, 6);

						if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 39) dam = damroll(10,6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

						///msg131221
						if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 39)
							msg_format(_("����ȔM�ƌ���%^s���Ă����I",
                                        "%^s is scorched by intense heat and light!"), m_name);
						else
							msg_format(_("%^s�̑̂��R���オ�����I",
                                        "%^s is enveloped in flames!"), m_name);

						if (mon_take_hit(m_idx, dam, &fear, _("�͓|�ꂽ�B", " is defeated.")))

						{
					blinked_photo = FALSE;
					blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
					}
				}

				if (p_ptr->sh_elec && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						///msg131221
						msg_format("%^s�͓d������������I", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�͊��d���ē|�ꂽ�B"))
#else
						msg_format("%^s gets zapped!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " is defeated by the electric shock."))
#endif

						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
					}
				}

				if (p_ptr->sh_cold && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_COLD_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						///msg131221
						msg_format("%^s�͗�C����������I", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�͓�����ē|�ꂽ�B"))
#else
						msg_format("%^s is very cold!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " is defeated by being frozen."))
#endif

						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
					}
				}


				/* by henkma */
				/*:::�j�ЃI�[���@���_�����A�G�������^���n�[�x�X�^�[�A�㎚�h���A�N���~�i���M�A�Ƃ��ė��p*/
				if (p_ptr->dustrobe && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_RES_SHAR_MASK))
					{
						int dam;
						if(p_ptr->pclass == CLASS_IKU || p_ptr->pclass == CLASS_NITORI)
							dam = damroll(2,(p_ptr->lev / 3));
						else if(p_ptr->pclass == CLASS_PATCHOULI || p_ptr->pclass == CLASS_SANAE || p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_JUNKO)
							dam = damroll(2,(p_ptr->lev / 2));
						else
							dam = damroll(2, 6);
						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

						///msg131221
						///mod140217 �ߋ�̃I�[���Ƌ��L
						if(p_ptr->pclass == CLASS_IKU)
							msg_format(_("%^s�ɞ����h�������I", "%^s is stabbes by thorns!"), m_name);
						else
							msg_format(_("%^s�͐؂�􂩂ꂽ�I", "%^s gets sliced!"), m_name);

						if (mon_take_hit(m_idx, dam, &fear,_("�͓|�ꂽ�B", " is defeated.")))
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_RES_SHAR_MASK);
					}

					if (is_mirror_grid(&cave[py][px]))
					{
						teleport_player(10, 0L);
					}
				}


				///mod140321 ����̃I�[���@���|�A�N�O�A�����t�^������ǉ�
				/*::: ����̃I�[�� */
				if (p_ptr->tim_sh_death && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_RES_NETH))
					{
						int power = p_ptr->lev * 2;
						if(r_ptr->flags1 & RF1_UNIQUE ) power /= 2;
						if(!(r_ptr->flags3 & RF3_NO_FEAR) && randint0(power) > r_ptr->level && !MON_MONFEAR(m_ptr))
						{
							(void)set_monster_monfear(m_idx, 5 + randint0(power/2));
							fear = TRUE;
						}
						if(!(r_ptr->flags3 & RF3_NO_STUN) && randint0(power) > r_ptr->level)
						{
							(void)set_monster_stunned(m_idx,MON_CONFUSED(m_ptr) + 5 + randint0(power/2));
							stun = TRUE;
						}
						if(!(r_ptr->flags3 & RF3_NO_CONF) && randint0(power) > r_ptr->level)
						{
							(void)set_monster_confused(m_idx, MON_STUNNED(m_ptr) + 5 + randint0(power/2));
							conf = TRUE;
						}

					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_RES_NETH);
					}

				}

				//v1.1.30 �u���Ȃ�I�[���v��j�׎�_�̓G�Ɍ����悤�ύX
				if (p_ptr->tim_sh_holy && alive && !p_ptr->is_dead)
				{
					if (r_ptr->flagsr & RFR_RES_ALL)
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= RFR_RES_ALL;
					}
					else if(r_ptr->flagsr & RFR_HURT_HOLY)
					{
						int dam = damroll(4, 6);

						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= RFR_HURT_HOLY;

						if(r_ptr->flags3 & (RF3_KWAI | RF3_DEMON | RF3_UNDEAD))
						{
							msg_format(_("%^s�͐��Ȃ�I�[�����󂯂ĔR���オ�����I",
                                        "%^s is set ablaze by the holy aura!"), m_name);
							dam *= 2;
						}
						else
						{
							msg_format(_("���Ȃ�I�[����%^s���ł������I",
                                        "%^s is scorched by the holy aura!"), m_name);
						}
						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

						if (mon_take_hit(m_idx, dam, &fear,_("�͓|�ꂽ�B", " is defeated.")))
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
				}

				if (p_ptr->tim_sh_touki && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_RES_ALL))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s���s�����C�̃I�[���ŏ������I", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�͓|�ꂽ�B"))
#else
						msg_format("%^s is injured by the fighting spirit aura!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " is defeated."))
#endif

						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= RFR_RES_ALL;
					}
				}

				//v1.1.86 �A�r���e�B�J�[�h�u����̔����v
				if (alive && !p_ptr->is_dead)
				{
					int card_count;

					card_count = count_ability_card(ABL_CARD_QUARTER_GHOST);
					if (card_count)
					{
						if (!(r_ptr->flagsr & RFR_EFF_RES_NETH_MASK))
						{
							int dam;

							int dice = card_count+1;
							int sides = 9;
							if (card_count >= 9) sides = 10;

							dam = damroll(dice, sides);

							dam = mon_damage_mod(m_ptr, dam, FALSE);

							msg_format(_("����̔�����%^s�ɔ��������I",
                                        "Your half-half-ghost counterattacks %^s!"), m_name);

							if (mon_take_hit(m_idx, dam, &fear, _("�͓|�ꂽ�B", " is defeated.")))
							{
								blinked_photo = FALSE;
								blinked = FALSE;
								alive = FALSE;
							}
						}

					}
				}

				/*:::��p�F�e�̃N���[�N*/
				if (hex_spelling(HEX_SHADOW_CLOAK) && alive && !p_ptr->is_dead)
				{
					int dam = 1;
					object_type *o_ptr = &inventory[INVEN_RARM];

					if (!(r_ptr->flagsr & RFR_RES_ALL || r_ptr->flagsr & RFR_RES_DARK))
					{
						if (o_ptr->k_idx)
						{
							int basedam = ((o_ptr->dd + p_ptr->to_dd[0]) * (o_ptr->ds + p_ptr->to_ds[0] + 1));
							dam = basedam / 2 + o_ptr->to_d + p_ptr->to_d[0];
						}

						/* Cursed armor makes damages doubled */
						o_ptr = &inventory[INVEN_BODY];
						if ((o_ptr->k_idx) && object_is_cursed(o_ptr)) dam *= 2;

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("�e�̃I�[����%^s�ɔ��������I", m_name);
						if (mon_take_hit(m_idx, dam, &fear, "�͓|�ꂽ�B"))
#else
						msg_format("Your shadow aura counterattacks %^s!", m_name);

						if (mon_take_hit(m_idx, dam, &fear, " is defeated."))
#endif
						{
							blinked_photo = FALSE;
							blinked = FALSE;
							alive = FALSE;
						}
						else /* monster does not dead */
						{
							int j;
							int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
							int typ[4][2] = {
								{ INVEN_HEAD, GF_OLD_CONF },
								{ INVEN_LARM,  GF_OLD_SLEEP },
								{ INVEN_HANDS, GF_TURN_ALL },
								{ INVEN_FEET, GF_OLD_SLOW }
							};

							/* Some cursed armours gives an extra effect */
							for (j = 0; j < 4; j++)
							{
								o_ptr = &inventory[typ[j][0]];
								if ((o_ptr->k_idx) && object_is_cursed(o_ptr) && object_is_armour(o_ptr))
									project(0, 0, m_ptr->fy, m_ptr->fx, (p_ptr->lev * 2), typ[j][1], flg, -1);
							}
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (RFR_RES_ALL | RFR_RES_DARK);
					}
				}
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_SLASH:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_CHARGE:
				case RBM_SPEAR:
				case RBM_BOOK:
				case RBM_INJECT:
				case RBM_HEADBUTT:
				case RBM_PRESS:
				case RBM_STRIKE:

				/* Visible monsters */
				if (m_ptr->ml)
				{
					/* Disturbing */
					disturb(1, 1);

					/* Message */
#ifdef JP
					if (abbreviate)
					    msg_format("%s���킵���B", (p_ptr->special_attack & ATTACK_SUIKEN) ? "��ȓ�����" : "");
					else
					    msg_format("%s%^s�̍U�������킵���B", (p_ptr->special_attack & ATTACK_SUIKEN) ? "��ȓ�����" : "", m_name);
					abbreviate = 1;/*�Q��ڈȍ~�͏ȗ� */
#else
					msg_format("%^s misses you.", m_name);
#endif

				}
				damage = 0;

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (is_original_ap_and_seen(m_ptr) && !do_silly_attack)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}

		if (p_ptr->riding && damage)
		{
			char m_name[80];
			monster_desc(m_name, &m_list[p_ptr->riding], 0);
			if (rakuba((damage > 200) ? 200 : damage, FALSE))
			{
#ifdef JP
msg_format("%^s���痎���Ă��܂����I", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}

		if (p_ptr->special_defense & NINJA_KAWARIMI)
		{
			if (kawarimi(FALSE)) return TRUE;
		}

		//v1.1.77 ����(��p���i)�̒ǐՁ@
		if (orin_escape(m_idx)) return TRUE;

	}

	/* Hex - revenge damage stored */
	revenge_store(get_damage);

	if ((p_ptr->tim_eyeeye || hex_spelling(HEX_EYE_FOR_EYE))
		&& get_damage > 0 && !p_ptr->is_dead)
	{
#ifdef JP
		msg_format("�U����%s���g���������I", m_name);
#else
		char m_name_self[80];

		/* hisself */
		monster_desc(m_name_self, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
#endif
		project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
		if(p_ptr->pclass == CLASS_HINA)
		{
			hina_gain_yaku(-(get_damage/5));
			if(!p_ptr->magic_num1[0]) set_tim_eyeeye(0,TRUE);
		}
		else if (p_ptr->tim_eyeeye) set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);


		///mod151101�ǉ�
		if(!m_ptr->r_idx) alive = FALSE;
	}

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_JINKIN)
		&& alive && get_damage > 0 && !p_ptr->is_dead)
	{
		msg_format(_("%s�͋Ղ̉��ɕ�܂ꂽ�E�E", "The sounds of koto envelop %s..."), m_name);
		project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
		///mod151101�ǉ�
		if(!m_ptr->r_idx) alive = FALSE;
	}

	//���z�̌^�ƗH�X�q���Z�ɂ�锽������
	if ((p_ptr->counter || (p_ptr->special_defense & KATA_MUSOU)) && alive && !p_ptr->is_dead && m_ptr->ml && (p_ptr->csp > 7))
	{


		if (p_ptr->pclass == CLASS_YUYUKO)
		{
			int tmp_dam = damroll(p_ptr->lev,(20+ adj_general[p_ptr->stat_ind[A_CHR]]));
			msg_format(_("���̗͂�%s��I��ł����I", "The power of death consumes %s!"), m_name);
			project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_NETHER, PROJECT_KILL, -1);

		}
		else
		{

			//char m_name[80];
			//monster_desc(m_name, m_ptr, 0);

			p_ptr->csp -= 7;
			msg_format(_("%^s�ɔ��������I", "You counterattack %^s!"), m_name);
			py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_COUNTER);
			fear = FALSE;
		}

		///mod151101�ǉ�
		if(!m_ptr->r_idx) alive = FALSE;

		/* Redraw mana */
		p_ptr->redraw |= (PR_MANA);
	}

	if(p_ptr->pclass == CLASS_BENBEN && music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE) && alive && !p_ptr->is_dead)
	{
		msg_format(_("%^s�ɔ��������I", "You counterattack %^s!"), m_name);
		py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_DOUBLESCORE);
		fear = FALSE;
		///mod151101�ǉ�
		if(!m_ptr->r_idx) alive = FALSE;
	}
	if(p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_NORIGOTO) && alive && !p_ptr->is_dead)
	{
		int tmp_dice = 10 + p_ptr->lev / 5;
		int tmp_sides = 10 + adj_general[p_ptr->stat_ind[A_CHR]] / 3;
		msg_format(_("�b�����Ղ̉���%^s�ɓ˂��h�������I", "Shrill sounds of koto pierce %^s!"), m_name);
		project_m(0,0,m_ptr->fy,m_ptr->fx,damroll(tmp_dice,tmp_sides),GF_SOUND,PROJECT_KILL,TRUE);
		fear = FALSE;
		///mod151101�ǉ�
		if(!m_ptr->r_idx) alive = FALSE;
	}

	/* Blink away */
	if (blinked && alive && !p_ptr->is_dead)
	{
		//v1.1.86�@��K�z�̋��P�J�[�h�ɂ�铦�S�j�~
		int kappa_card_num = count_ability_card(ABL_CARD_SYUSENDO);
		int kappa_prob = 0;

		if (kappa_card_num>1)
		{
			kappa_prob = (kappa_card_num - 1) * 10;
			if (kappa_card_num >= 9) kappa_prob = 100;

		}

		if (conf)
		{
			msg_print(_("�D�_�͏΂��ē���...�悤�Ƃ������������ē������Ȃ������B",
                        "The thief flees laughing...? But fails due to being confused."));
		}

		else if (cave_have_flag_bold(m_ptr->fy, m_ptr->fx, FF_SPIDER_NEST_1)
			&& !(r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)))
		{
			msg_print(_("�D�_�͏΂��ē���...�悤�Ƃ������w偂̑��Ɉ������������B",
                        "The thief flees laughing...? But fails due to being stuck in the web."));
		}

		else if (teleport_barrier(m_idx))
		{
			msg_print(_("�D�_�͏΂��ē���...�悤�Ƃ����������߂��ꂽ�B",
                        "The thief flees laughing...? But magic barrier obstructs it."));
		}
		else if (randint1(100) <= kappa_prob)
		{
			msg_print(_("�D�_�͏΂��ē���...�悤�Ƃ������͓��̘r�Ɉ����͂܂ꂽ�I",
                        "The thief flees laughing...? But gets dragged back by your kappa arm!"));
		}
		else
		{
#ifdef JP
			msg_print("�D�_�͏΂��ē������I");
#else
			msg_print("The thief flees laughing!");
#endif
			teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);
		}
	}
	///mod140111 �ʐ^�B�e�̂Ƃ��ɂ������X�^�[�͏΂��ē�����悤�ɂ���
	else if (blinked_photo && alive && !p_ptr->is_dead)
	{
		if(conf)
			msg_print(_("�p�p���b�`�͏΂��ē���...�悤�Ƃ������������ē������Ȃ������B",
                        "The paparazzi flees laughing...? But fails due to being confused."));
		else if(cave_have_flag_bold(m_ptr->fy,m_ptr->fx,FF_SPIDER_NEST_1)
			&& !(r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL)))
			msg_print(_("�p�p���b�`�͏΂��ē���...�悤�Ƃ������w偂̑��Ɉ������������B",
                        "The paparazzi flees laughing...? But fails due to being stuck in the web."));

		else if (teleport_barrier(m_idx))
		{
#ifdef JP
			msg_print("�p�p���b�`�͏΂��ē���...�悤�Ƃ����������߂��ꂽ�B");
#else
			msg_print("The paparazzi flees laughing...? But magic barrier obstructs it.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("�p�p���b�`�͏΂��ē������I");
#else
			msg_print("The paparazzi flees laughing!");
#endif
			teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);
		}
	}


	/* Always notice cause of death */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++;
	}

	if (m_ptr->ml && conf && alive && !p_ptr->is_dead)	msg_format(_("%^s�͍��������悤���B",
                                                                    "%^s is confused."), m_name);
	if (m_ptr->ml && stun && alive && !p_ptr->is_dead)	msg_format(_("%^s�͞N�O�Ƃ����B",
                                                                    "%^s is stunned."), m_name);

	if (m_ptr->ml && fear && alive && !p_ptr->is_dead)
	{
		sound(SOUND_FLEE);
#ifdef JP
		msg_format("%^s�͋��|�œ����o�����I", m_name);
#else
		msg_format("%^s flees in terror!", m_name);
#endif

	}




	if (p_ptr->special_defense & KATA_IAI)
	{
		set_action(ACTION_NONE);
	}

	/* Assume we attacked */
	return (TRUE);
}
