#include "angband.h"

/*:::�푰�u��ǐ_�l�v�p�̊֐���f�[�^�Ȃ�*/
//���łɗd���p�̒ǉ�������������

/*:::��ǐ_�l�p�̓����ύX���[�`��*/
/*:::p_ptr->race_multipur_val[0]�`[2]���g���B[0]�������x�A[1]���D��x�A[2]�������x�@�͈͂�0-255*/
void set_deity_bias(int type,int num)
{

	if(p_ptr->prace != RACE_STRAYGOD) return;
	if (p_ptr->inside_arena) return; //v1.1.51 �V�A���[�i���Ŗ�ǐ_�l�̓����l�͕ϓ����Ȃ�
	if(!num) return;
	if(p_ptr->max_plv > 44) return; //���x��45�ȍ~�͂����ω����Ȃ�

	if(type < DBIAS_COSMOS || type > DBIAS_REPUTATION)
	{
		msg_format(_("ERROR:set_deity_bias()�ɒ�`����Ă��Ȃ������l��������(%d)",
                    "ERROR: Undefined bias value (%d) in set_deity_bias()"),type);
		return;
	}

	/*:::���܂�[�����̐��l�ɂ͂Ȃ�ɂ����悤�ɕ␳����*/
	if(p_ptr->race_multipur_val[type] > 192 && num > 0 || p_ptr->race_multipur_val[type] < 64 && num < 0)
	{
		//v1.1.28 �������C��
		if( num==1 || num==-1)
		{
			if( one_in_(3)) return;
		}
		else
		{
			num = (num * 2 + randint0(3))/ 3;
		}
	}

	p_ptr->race_multipur_val[type] += num;
	if(p_ptr->race_multipur_val[type] < 0) p_ptr->race_multipur_val[type] = 0;
	else if(p_ptr->race_multipur_val[type] > 255) p_ptr->race_multipur_val[type] = 255;

	return;

}

/*:::��ǐ_�l�p�@���݂̓����l�ɉ����ĐV���Ȑ_���^�C�v�𓾂�*/
//p_ptr->race_multipur_val[0-2]:���݂̓����l
//p_ptr->race_multipur_val[3]:���݂̐_�i�^�C�v
//p_ptr->race_multipur_val[4]:�����N1�̂Ƃ��̃^�C�v + �����N2�̂Ƃ��̃^�C�v * 128
void change_deity_type(int rank)
{
	int i;
	int new_type = -1;
	int cosmos = 999;
	int noble = 999;
	int reputation = 999;

	if(p_ptr->prace != RACE_STRAYGOD) return;

	if(rank < 1 || rank > 3)
	{	msg_format(_("ERROR:change_deity_type()�ɒ�`����Ă��Ȃ�rank�l��������(%d)",
                    "ERROR: Undefined rank (%d) in change_deity_type()"),rank);	return;	}

	for(i=0;deity_table[i].rank != 0;i++)
	{
		if(rank != deity_table[i].rank) continue;

		if(new_type == -1) new_type = i;
		else
		{
			int dis_c1,dis_c2,dis_w1,dis_w2,dis_n1,dis_n2;

			dis_c1 = deity_table[new_type].dbias_cosmos - p_ptr->race_multipur_val[DBIAS_COSMOS];
			if(dis_c1 < 0) dis_c1 *= -1;
			dis_c2 = deity_table[i].dbias_cosmos - p_ptr->race_multipur_val[DBIAS_COSMOS];
			if(dis_c2 < 0) dis_c2 *= -1;

			dis_w1 = deity_table[new_type].dbias_warlike - p_ptr->race_multipur_val[DBIAS_WARLIKE];
			if(dis_w1 < 0) dis_w1 *= -1;
			dis_w2 = deity_table[i].dbias_warlike - p_ptr->race_multipur_val[DBIAS_WARLIKE];
			if(dis_w2 < 0) dis_w2 *= -1;

			dis_n1 = deity_table[new_type].dbias_reputation - p_ptr->race_multipur_val[DBIAS_REPUTATION];
			if(dis_n1 < 0) dis_n1 *= -1;
			dis_n2 = deity_table[i].dbias_reputation - p_ptr->race_multipur_val[DBIAS_REPUTATION];
			if(dis_n2 < 0) dis_n2 *= -1;

			//��ԋ߂������l�̐_�i�^�C�v�ɕω��@�����l�̉e���͑��̓�{
			if( (dis_c1 + dis_w1 + dis_n1 * 2) > (dis_c2 + dis_w2 + dis_n2 * 2)) new_type = i;
		}

	}
	if(rank == 2) p_ptr->race_multipur_val[4] = p_ptr->race_multipur_val[3];
	else if(rank == 3) p_ptr->race_multipur_val[4] += p_ptr->race_multipur_val[3] * 128;
	p_ptr->race_multipur_val[3] = new_type;

	//if(rank != 1)
	{
		msg_print(_("�V���ȐM���W�܂��Ă����̂�������E�E", "You feel you've gathered more faith..."));
		msg_format(_("���Ȃ��́u%s�v�ƂȂ����I", "You have became a %s!"),deity_table[new_type].deity_name);
	}
	p_ptr->update |= (PU_BONUS);

	return;
}


/*:::��ǐ_�l�̑ϐ��t�^���[�`���@calc_bonuses()����Ăԁ@apply_deity_ability_flags()�ƈ�v����悤����*/
void apply_deity_ability(int deity_type, int *new_speed)
{
	if(p_ptr->prace != RACE_STRAYGOD) return;
	if(deity_type < 0 || deity_type >= MAX_DEITY_TABLE_ENTRY)
	{
		msg_format(_("ERROR:apply_deity_ability()���e�[�u���O�̒l(%d)�ŌĂ΂ꂽ",
                    "ERROR: apply_deity_ability() called with value (%d) out of table bounds"),deity_type);
		return;
	}

	switch(deity_type)
	{
		case 0: //p_ptr->race_multipur_val[4]�̋󔒕�
		break;

		case 1: //����̐_�l
		p_ptr->slow_digest = TRUE;
		break;
		case 2: //�g�t�̐_�l
		p_ptr->sustain_chr = TRUE;
		break;
		case 3: //�n��
		p_ptr->free_act = TRUE;
		break;
		case 4: //�Y���ꂽ���c�_
		p_ptr->see_inv = TRUE;
		break;
		case 5: //�Â��~�̖؂̐_�l
		p_ptr->sustain_wis = TRUE;
		break;
		case 6: //��B�_
		p_ptr->resist_disen = TRUE;
		break;
		case 7: //�ςȊ�
		p_ptr->resist_fire = TRUE;
		break;
		case 8: //�Ջ�
		p_ptr->resist_pois = TRUE;
		break;
		case 9: //��
		p_ptr->resist_water = TRUE;
		break;
		case 10: //��h
		p_ptr->levitation = TRUE;
		break;
		case 11: //�Y�y�_
		p_ptr->esp_human = TRUE;
		break;
		case 12: //���_(�����Ȃ�)
		break;
		case 13: //�_��
		p_ptr->resist_holy = TRUE;
		break;
		case 14: //��b
		p_ptr->esp_animal = TRUE;
		break;

		//rank2
		case 15: //�R�m
		p_ptr->resist_fear = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_SWORD) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_SWORD) p_ptr->to_ds[1]+=2;
		break;

		case 16: //��
		p_ptr->resist_disen = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_KATANA) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_KATANA) p_ptr->to_ds[1]+=2;
		break;

		case 17: //�ΎR
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_fire = TRUE;
		break;

		case 18: //����
		p_ptr->dec_mana = TRUE;
		break;
		case 19: //���a����
		p_ptr->resist_pois = TRUE;
		p_ptr->free_act = TRUE;
		break;
		case 20: //���b�N
		p_ptr->resist_sound = TRUE;
		p_ptr->resist_chaos = TRUE;
		break;
		case 21: //������
		p_ptr->esp_human = TRUE;
		p_ptr->esp_unique = TRUE;
		break;
		case 22: //�w�Ɛ��A
		p_ptr->resist_conf = TRUE;
		p_ptr->sustain_int = TRUE;
		break;
		case 23: //����
		p_ptr->free_act = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;

		case 24: //����
		if(p_ptr->do_martialarts)
		{
			p_ptr->to_dd[0] +=1;
		}
		break;
		case 25: //�؂���
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_AXE) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_AXE) p_ptr->to_ds[1]+=2;
		case 26: //�W�����O��
		p_ptr->resist_pois = TRUE;
		p_ptr->regenerate = TRUE;
		break;
		case 27: //�Ă���
		p_ptr->resist_fire = TRUE;
		break;
		case 28: //��l
		p_ptr->xtra_might = TRUE;
		break;
		case 29: //����
		p_ptr->easy_spell = TRUE;
		break;
		case 30: //�@�D��
		p_ptr->sustain_dex = TRUE;
		p_ptr->resist_disen = TRUE;
		break;
		case 31: //���l
		p_ptr->slow_digest = TRUE;
		break;
		case 32: //�哹�|
		p_ptr->mighty_throw = TRUE;
		break;
		case 33: //���
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_water = TRUE;
		break;
		case 34: //����
		p_ptr->resist_cold = TRUE;
		p_ptr->sh_cold = TRUE;
		break;
		case 35: //�앪
		*new_speed += 2;
		p_ptr->levitation = TRUE;
		break;
		case 36: //�ÎE��
		p_ptr->resist_fear = TRUE;
		if(buki_motteruka(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_KNIFE) p_ptr->to_ds[0]+=2;
		if(buki_motteruka(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_KNIFE) p_ptr->to_ds[1]+=2;
		break;
		case 37: //�΍�
		p_ptr->resist_fire = TRUE;
		p_ptr->sh_fire = TRUE;
		break;
		case 38: //�D�_
		p_ptr->warning = TRUE;
		break;
		case 39: //�m�l�`
		p_ptr->resist_neth = TRUE;
		break;
		case 40: //�L�m�R
		p_ptr->regenerate = TRUE;
		p_ptr->resist_pois = TRUE;
		break;
		case 41: //�u�a
		p_ptr->resist_pois = TRUE;
		break;

		//rank3
		case 42: //�j��_
			p_ptr->resist_conf = TRUE; //v1.1.50 �Y��Ă��̂Œǉ�
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->kill_wall = TRUE;
		break;
		case 43: //��n�_
		p_ptr->pass_wall = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_sound = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->slow_digest = TRUE;
		break;

		case 44: //���z�_
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_lite = TRUE;//�Ɖu
		p_ptr->resist_blind = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->levitation = TRUE;
		break;

		case 45: //�V��
		p_ptr->resist_holy = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->levitation = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		case 46: //���̐_
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->warning = TRUE;
		break;

		case 47: //�C�_
		p_ptr->resist_water = TRUE; //�Ɖu�����ǉ�
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_pois = TRUE;
		break;

		case 48: //�S�_
		p_ptr->resist_fear = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_elec = TRUE;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_con = TRUE;
		break;

		case 49: //�L���_
		p_ptr->resist_water = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->resist_pois = TRUE;
		break;


		case 50: //�b�q�̐_
		p_ptr->telepathy = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		break;

		case 51: //�R�_
		p_ptr->resist_fear = TRUE;
		p_ptr->warning = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->reflect = TRUE;
		break;

		case 52: //���~�̐_
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_disen = TRUE;
		p_ptr->sustain_chr = TRUE;
		break;

		case 53: //��_
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 54: //���_
		p_ptr->speedster = TRUE;
		p_ptr->levitation = TRUE;
		*new_speed += 5;
		break;

		case 55: //���_
		p_ptr->resist_elec = TRUE;
		p_ptr->immune_elec = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->resist_sound = TRUE;
		break;

		case 56: //腖�
		p_ptr->free_act = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		case 57: //�M��_
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;
		case 58: //���ׂ̐_
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->esp_evil = TRUE;
		break;
		case 59: //���_
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->sh_fire = TRUE;
		break;

		case 60: //���|�̑剤
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 61: //���҂̐_
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->esp_undead = TRUE;
		break;

		case 62: //�א_
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->esp_demon = TRUE;
		p_ptr->sh_cold = TRUE;
		break;

		case 63: //�O�Ȃ�_
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_time = TRUE;
		p_ptr->resist_insanity = TRUE;
		p_ptr->resist_fear = TRUE;
		break;

		case 64: //�R�f
		p_ptr->sustain_dex = TRUE;
		break;
		case 65: //���Y
		p_ptr->resist_fear = TRUE;
		p_ptr->resist_neth = TRUE;
		break;
		case 66: //�ԉ�
		p_ptr->resist_fire = TRUE;
		break;
		case 67: //�V�F�t
		p_ptr->regenerate = TRUE;
		p_ptr->slow_digest = TRUE;
		break;
		case 68: //���܂�
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_blind = TRUE;
		break;
		case 69: //�؃g��
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_con = TRUE;
		break;
		case 70: //�T �����Ȃ�
		break;
		case 71: //���
		p_ptr->resist_water = TRUE;
		break;
		case 72: //����
		p_ptr->resist_water = TRUE;
		p_ptr->resist_disen = TRUE;
		break;
		case 73: //�[�Ă�
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		break;

		case 74: //��_
		p_ptr->see_inv = TRUE;
		break;

		case 75: //��
		p_ptr->see_inv = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_insanity = TRUE;
		break;

		case 76: //��
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_time = TRUE;
		break;

		case 77: //�b��_
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_lite = TRUE;
		break;

		case 78: //�z�R
		p_ptr->resist_dark = TRUE;
		break;

		case 79: //���q��(���ԗp)
		p_ptr->resist_water = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->see_inv = TRUE;
		break;

		default:
		msg_format(_("ERROR:apply_deity_ability()�Ŗ��o�^�̐_�i�^�C�v(%d)���Ă΂ꂽ",
                    "ERROR: apply_deity_ability() called for an unlisted deity type (%d)"),deity_type);
		return;
	}

}


/*:::��ǐ_�l�̑ϐ��L�q���[�`���@player_flags()����Ă�*/
void apply_deity_ability_flags(int deity_type, u32b flgs[TR_FLAG_SIZE])
{
	if(p_ptr->prace != RACE_STRAYGOD) return;
	if(deity_type < 0 || deity_type >= (MAX_DEITY_TABLE_ENTRY))
	{
		msg_format(_("ERROR:apply_deity_ability_flags()���e�[�u���O�̒l(%d)�ŌĂ΂ꂽ",
                    "ERROR: apply_deity_ability_flags() called with value (%d) out of table bounds"),deity_type);
		return;
	}

	switch(deity_type)
	{
		case 0: break;

		case 1: //����̐_�l
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 2: //�g�t�̐_�l
		add_flag(flgs, TR_SUST_CHR);
		break;
		case 3: //�n��
		add_flag(flgs, TR_FREE_ACT);
		break;
		case 4: //�Y���ꂽ���c�_
		add_flag(flgs, TR_SEE_INVIS);
		break;
		case 5: //�Â��~�̖؂̐_�l
		add_flag(flgs, TR_SUST_WIS);
		break;
		case 6: //��B�_
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 7: //�ςȊ�
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 8: //�Ջ�
		add_flag(flgs, TR_RES_POIS);
		break;
		case 9: //��
		add_flag(flgs, TR_RES_WATER);
		break;
		case 10: //�h
		add_flag(flgs, TR_LEVITATION);
		break;
		case 11: //�Y�y�_
		add_flag(flgs, TR_ESP_HUMAN);
		break;
		case 12: //���_(�����Ȃ�)
		break;
		case 13: //�_��
		add_flag(flgs, TR_RES_HOLY);
		break;
		case 14: //��b
		add_flag(flgs, TR_ESP_ANIMAL);
		break;

		case 15: //�R�m
		add_flag(flgs, TR_RES_FEAR);
		break;
		case 16: //��
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 17: //�ΎR
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_SHARDS);
		break;
		case 18: //����
		add_flag(flgs, TR_DEC_MANA);
		break;
		case 19: //���a����
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		break;
		case 20: //���b�N
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_CHAOS);
		break;
		case 21: //������
		add_flag(flgs, TR_ESP_HUMAN);
		add_flag(flgs, TR_ESP_UNIQUE);
		break;
		case 22: //�w�Ɛ��A
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_SUST_INT);
		break;
		case 23: //����
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SUST_CHR);
		break;
		case 24: //����(�����Ȃ�)
		case 25: //�؂���(�����Ȃ�)
		break;
		case 26: //�W�����O��
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_REGEN);
		break;
		case 27: //�Ă���
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 28: //��l
		add_flag(flgs, TR_XTRA_MIGHT);
		break;
		case 29: //����
		add_flag(flgs, TR_EASY_SPELL);
		break;
		case 30: //�@�D��
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 31: //���l
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 32: //�哹�|
		add_flag(flgs, TR_THROW);
		break;
		case 33: //���
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_WATER);
		break;
		case 34: //����
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_SH_COLD);
		break;
		case 35: //�앪
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_LEVITATION);
		break;
		case 36: //�ÎE��
		add_flag(flgs, TR_RES_FEAR);
		break;
		case 37: //�΍�
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_SH_FIRE);
		break;
		case 38: //�D�_
		add_flag(flgs, TR_WARNING);
		break;
		case 39: //�m�l�`
		add_flag(flgs, TR_RES_NETHER);
		break;
		case 40: //�L�m�R
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 41: //�u�a
		add_flag(flgs, TR_RES_POIS);
		break;

		//rank3
		case 42: //�j��_
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_HOLY);
		break;
		case 43: //��n�_
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 44: //���z�_
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		break;
		case 45: //�V��
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SEE_INVIS);
		break;
		case 46: //���̐_
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_WARNING);
		break;
		case 47: //�C�_
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 48: //�S�_
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_CON);
		break;

		case 49: //�L���_
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_POIS);
		break;

		case 50: //�b�q�̐_
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		break;

		case 51: //�R�_
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_REFLECT);
		break;

		case 52: //���~
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_SUST_CHR);
		break;

		case 53: //��
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 54: //���_
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SPEEDSTER);
		add_flag(flgs, TR_LEVITATION);
		break;

		case 55: //���_
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_SOUND);
		break;

		case 56: //腖�
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		break;

		case 57: //�M��_
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FEAR);
		break;

		case 58: //���ׂ̐_
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_ESP_EVIL);
		break;

		case 59: //���_
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SH_FIRE);
		break;

		case 60: //���|�̑剤
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 61: //���҂̐_
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_ESP_UNDEAD);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 62: //�א_
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_ESP_DEMON);
		add_flag(flgs, TR_SH_COLD);
		break;

		case 63: //�O�Ȃ�_
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FEAR);
		break;

		case 64: //�R�f
		add_flag(flgs, TR_SUST_DEX);
		break;
		case 65: //���Y
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_NETHER);
		break;
		case 66: //�ԉ�
		add_flag(flgs, TR_RES_FIRE);
		break;
		case 67: //�V�F�t
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		break;
		case 68: //���܂�
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_BLIND);
		break;
		case 69: //�؃g��
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_CON);
		break;
		case 70: //�T �����Ȃ�
		break;
		case 71: //���
		add_flag(flgs, TR_RES_WATER);
		break;
		case 72: //����
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_DISEN);
		break;
		case 73: //�[�Ă�
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		break;

		case 74: //��_
		add_flag(flgs, TR_SEE_INVIS);
		break;

		case 75: //��
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		break;

		case 76: //��
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_TIME);
		break;

		case 77: //�b��_
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_LITE);
		break;

		case 78: //�z�R
		add_flag(flgs, TR_RES_DARK);
		break;

		case 79: //����
			add_flag(flgs, TR_RES_WATER);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_NETHER);
		break;

		default:
		msg_format(_("ERROR:apply_deity_ability_flags()�ɖ��o�^�̒l(%d)�����͂��ꂽ",
                    "ERROR: Unlisted value (%d) entered in apply_deity_ability_flags()"),deity_type);
		break;
	}

}

//�����ψق𓾂�num�������Ə�������Ă��邩�`�F�b�N���邱��
/*:::��ǐ_�l�p���C�V�����ݒ胋�[�`�� do_cmd_racial_power()����̂݌Ă΂��*/
void deity_racial_power(power_desc_type *power_desc, int *num_return)
{

	int rank1,rank2,rank3;
	int num = *num_return;

	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_format(_("ERROR:deity_racial_power()����ǐ_�l�ȊO�̂Ƃ��Ă΂ꂽ",
                    "ERROR: deity_racial_power() called for non-stray-god"));
		return;
	}

	//����܂ł̐_�i����S�ēK�p�@����rank�̔ԍ��̓��x���A�b�v���ԂƂ͊֌W�Ȃ�
	rank1 = p_ptr->race_multipur_val[3];
	rank2 = p_ptr->race_multipur_val[4] / 128;
	rank3 = p_ptr->race_multipur_val[4] % 128;

			if(rank1 == 17 || rank2 == 17 || rank3 == 17 ) //�ΎR�̐_�l
			{
				strcpy(power_desc[num].name, _("����m��", "Berserk Rage"));
				power_desc[num].level = 25;
				power_desc[num].cost = 15;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 15;
				power_desc[num].info = _("�ꎞ�I�ɋ���m������B", "Grants temporary berserk rage.");
				power_desc[num++].number = -1;
			}
			if(rank1 == 18 || rank2 == 18 || rank3 == 18 ) //�����̐_�l
			{
				strcpy(power_desc[num].name, _("�ڗ���", "Appraise"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("�A�C�e�����Ӓ肷��B", "Identifies an item.");
				power_desc[num++].number = -2;
			}
			if(rank1 == 19 || rank2 == 19 || rank3 == 19 ) //���a���Ђ̐_�l
			{
				strcpy(power_desc[num].name, _("����", "Curing"));

				power_desc[num].level = 25;
				power_desc[num].cost = 24;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�ŁE�؂菝�E�N�O�E�ӖځE���o�����Â�HP��30�񕜂���B",
                                        "Heals 30 HP and cures poison, cuts, stunning, blindness and hallucination.");
				power_desc[num++].number = -3;
			}
			if(rank1 == 20 || rank2 == 20 || rank3 == 20 ) //���b�N�̐_�l
			{
				strcpy(power_desc[num].name, _("�V���E�g", "Shout"));

				power_desc[num].level = 20;
				power_desc[num].cost = 18;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�����̎��͂ɍ��������U�����s���B",
                                        "Performs a sonic attack around you.");
				power_desc[num++].number = -4;
			}
			if(rank1 == 21 || rank2 == 21 || rank3 == 21 ) //�����т̐_�l
			{
				strcpy(power_desc[num].name, _("�`���[�������X�^�[", "Charm Monster"));

				power_desc[num].level = 20;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�G��̂����_���z���ɂ��悤�Ǝ��݂�B",
                                        "Attempts to charm an enemy into being your follower.");
				power_desc[num++].number = -5;

			}

			if(rank1 == 22 || rank2 == 22 || rank3 == 22 ) //�w�Ɛ��A�̐_�l
			{
				strcpy(power_desc[num].name, _("���_�W��", "Concentration"));

				power_desc[num].level = 20;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�ꎞ�I�ɒm�\���㏸������B", "Temporarily raises intelligence.");
				power_desc[num++].number = -6;

			}
			if(rank1 == 23 || rank2 == 23 || rank3 == 23 ) //�����̐_�l
			{
				strcpy(power_desc[num].name, _("���f�̕�", "Alluring Dance"));

				power_desc[num].level = 20;
				power_desc[num].cost = 24;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�ꎞ�I�ɖ��͂��㏸������B", "Temporarily raises charisma.");
				power_desc[num++].number = -7;
			}
			if(rank1 == 24 || rank2 == 24 || rank3 == 24 ) //�����̐_�l
			{
				strcpy(power_desc[num].name, _("�m�C���g", "Raise Morale"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 10;
				power_desc[num].info = _("�ꎞ�I�Ƀq�[���[��ԂɂȂ�B", "Grants temporary heroism.");
				power_desc[num++].number = -8;

			}
			if(rank1 == 26 || rank2 == 26 || rank3 == 26 ) //���т̐_�l
			{
				strcpy(power_desc[num].name, _("��������", "Summon Animal"));

				power_desc[num].level = 20;
				power_desc[num].cost = 15;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�b�⎩�R�E�̐�����z���Ƃ��Ĉ�̏�������B",
                                        "Summons a beast or a natural being as a follower.");
				power_desc[num++].number = -9;

			}
			if(rank1 == 27 || rank2 == 27 || rank3 == 27 ) //�Ă����̐_�l
			{
				strcpy(power_desc[num].name, _("�Ă���", "Roast Sweet Potato"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("���������u�Ă����v�𐶐�����B���͂̃����X�^�[���N����B",
                                        "Creates a delicious roast sweet potato. Wakes up nearby monsters.");
				power_desc[num++].number = -10;
			}

			if(rank1 == 34 || rank2 == 34 || rank3 == 34 ) //����̐_�l
			{
				strcpy(power_desc[num].name, _("��C�̃u���X", "Frost Breath"));

				power_desc[num].level = 20;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HP��1/5�̈З̗͂�C�̃u���X��f���B",
                                        "Breathes frost. (damage: 1/5 of current HP)");
				power_desc[num++].number = -11;

			}


			if(rank1 == 37 || rank2 == 37 || rank3 == 37 ) //�΍Ђ̐_�l
			{
				strcpy(power_desc[num].name, _("�Ή��̃u���X", "Fire Breath"));

				power_desc[num].level = 20;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HP��1/5�̈З͂̉Ή��̃u���X��f���B",
                                        "Breathes fire. (damage: 1/5 of current HP)");
				power_desc[num++].number = -13;

			}

			if(rank1 == 38 || rank2 == 38 || rank3 == 38 ) //�D�_�̐_�l
			{
				strcpy(power_desc[num].name, _("�q�b�g���A�E�F�C", "Hit and Away"));

				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 15;
				power_desc[num].info = _("�U����u���ɗ��E����B���s���邱�Ƃ�����B",
                                        "Teleport away after attacking. Has a chance to fail.");
				power_desc[num++].number = -14;

			}
			if(rank1 == 39 || rank2 == 39 || rank3 == 39 ) //�m�l�`�̐_�l
			{
				strcpy(power_desc[num].name, _("��", "Curse"));

				power_desc[num].level = 20;
				power_desc[num].cost = 16;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 25;
				power_desc[num].info = _("�G��̂��􂢂ōU����15d15�̃_���[�W��^����B��R�����Ɩ����B",
                                        "Curses a single enemy, dealing 15d15 damage. Does nothing if resisted.");
				power_desc[num++].number = -15;

			}
			if(rank1 == 40 || rank2 == 40 || rank3 == 40 ) //�L�m�R�̐_�l
			{
				strcpy(power_desc[num].name, _("�L�m�R����", "Summon Mushrooms"));

				power_desc[num].level = 20;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 5;
				power_desc[num].info = _("���͂ɃL�m�R�^�����X�^�[����������B", "Summons mushroom monsters.");
				power_desc[num++].number = -16;

			}

			if(rank1 == 42 || rank2 == 42 || rank3 == 42 ) //�j��_
			{
				strcpy(power_desc[num].name, _("*�j��*", "*Destruction*"));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("���͂̃_���W������j�󂷂�B", "Destroys the dungeon in your vicinity.");
				power_desc[num++].number = -17;

				strcpy(power_desc[num].name, _("���̌�", "Psycho-Spear"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 50;
				power_desc[num].info = _("(���x��*3)+1d(���x��*3)�̈З͂́A�G�̃o���A�Ȃǂ��ђʂ��閜�\�����̃r�[������B",
                                        "Fires an almighty beam that pierces any enemy barriers. (damage: (level * 3) + 1d(level * 3))");
				power_desc[num++].number = -18;

			}

			if(rank1 == 43 || rank2 == 43 || rank3 == 43 ) //��n�_
			{

				strcpy(power_desc[num].name, _("�n�k", "Earthquake"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�n�k���N�����B", "Causes an earthquake.");
				power_desc[num++].number = -19;
			}

			if(rank1 == 44 || rank2 == 44 || rank3 == 44 ) //���z�_
			{
				strcpy(power_desc[num].name, _("���僌�[�U�[", "Huge Laser"));
				power_desc[num].level = 35;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�j�M�����̋���ȃ��[�U�[����B�З͂�HP��1/3�ɂȂ�B",
                                        "Fires a huge laser of nuclear energy. (damage: 1/3 of current HP)");
				power_desc[num++].number = -20;

				strcpy(power_desc[num].name, _("�z�[���[�t�@�C�A", "Holy Fire"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 50;
				power_desc[num].info = _("�j�ב����̋���ȋ�����B�З͂�(���x��*5)�ɂȂ�B",
                                        "Fires a huge ball of holy energy. (damage: level * 5)");
				power_desc[num++].number = -21;
			}
			if(rank1 == 45 || rank2 == 45 || rank3 == 45 ) //�V��
			{
				strcpy(power_desc[num].name, _("���͉�", "Restore Mana"));
				power_desc[num].level = 30;
				power_desc[num].cost = 1;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�͂���MP���񕜂���B", "Slightly restores MP.");
				power_desc[num++].number = -22;

				strcpy(power_desc[num].name, _("�����̔�", "Dimension Door."));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 50;
				power_desc[num].info = _("�w�肵���ꏊ�փe���|�[�g����B���s���邱�Ƃ�����B",
                                        "Teleports to a specified location. Might fail.");
				power_desc[num++].number = -23;
			}
			if(rank1 == 46 || rank2 == 46 || rank3 == 46 ) //���̐_
			{

				strcpy(power_desc[num].name, _("��������", "Invite Fortune"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�ꎞ�I�ɍK�^���g�ɕt���B�t���A���ړ�����Ɖ��������B",
                                        "Temporarily increases luck. Effect disappear if you move to another level.");
				power_desc[num++].number = -24;
			}
			if(rank1 == 47 || rank2 == 47 || rank3 == 47 ) //�C�_
			{

				strcpy(power_desc[num].name, _("�A�N�A�u���X", "Water Breath"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("HP��1/4�̈З͂̐������̃u���X��f���B",
                                        "Breathes water. (damage: 1/4 of your current HP)");
				power_desc[num++].number = -25;

				strcpy(power_desc[num].name, _("���C���V���g����", "Maelstrom"));
				power_desc[num].level = 40;
				power_desc[num].cost = 45;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 50;
				power_desc[num].info = _("��Q���N�����Ď��͂̓G�ɍő�(���x��*4)�_���[�W�̐������U�������A����Ɏ��͂̒n�`�𐅂ɂ���B",
                                        "Creates a huge whirpool, dealing water damage (up to level * 4) to nearby enemies; also turns terrain into water.");
				power_desc[num++].number = -26;
			}
			if(rank1 == 48 || rank2 == 48 || rank3 == 48 ) //�S�_
			{

				strcpy(power_desc[num].name, _("����", "Madness"));
				power_desc[num].level = 30;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 25;
				power_desc[num].info = _("��莞�ԉ���������m������B", "Temporarily hastes you and makes you berserk.");
				power_desc[num++].number = -27;

				strcpy(power_desc[num].name, _("���o", "Peerless"));
				power_desc[num].level = 40;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("���͂�8�}�X�S�Ăɒʏ�U�����s���B", "Attacks all 8 adjacent tiles.");
				power_desc[num++].number = -28;
			}

			if(rank1 == 49 || rank2 == 49 || rank3 == 49 ) //�L���_
			{

				strcpy(power_desc[num].name, _("�󕠏[��", "Satisfy Hunger"));
				power_desc[num].level = 20;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 10;
				power_desc[num].info = _("������ԂɂȂ�B", "Makes you full.");
				power_desc[num++].number = -29;

				strcpy(power_desc[num].name, _("�������i", "Rapid Growth"));
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("���͂̒n�ʂ�X��Ԕ��ɕω�������B",
                                        "Change nearby terrain into forest/flowers.");
				power_desc[num++].number = -30;
			}


			if(rank1 == 50 || rank2 == 50 || rank3 == 50 ) //�b�q�̐_
			{
				strcpy(power_desc[num].name, _("�����~��", "Clear Mind"));
				power_desc[num].level = 30;
				power_desc[num].cost = 1;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�͂���MP���񕜂���B", "Slightly restores MP.");
				power_desc[num++].number = -22; //�V�_�̂Əd��
			}


			if(rank1 == 51 || rank2 == 51 || rank3 == 51 ) //�R�_
			{
				strcpy(power_desc[num].name, _("���R����", "Summon Reinforcements"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�~������������B", "Summons aid.");
				power_desc[num++].number = -31;
			}

			if(rank1 == 52 || rank2 == 52 || rank3 == 52 ) //���~
			{
				strcpy(power_desc[num].name, _("����", "Allure"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 40;
				power_desc[num].info = _("���E���̓G�S�Ă�z���ɂ��悤�Ǝ��݂�B",
                                        "Attempts to make all enemies in sight your followers.");
				power_desc[num++].number = -32;

				strcpy(power_desc[num].name, _("����", "Healing"));
				power_desc[num].level = 40;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_WIS;
				power_desc[num].fail = 45;
				power_desc[num].info = _("���x��*6��HP���񕜂���B", "Recovers (level * 6) HP.");
				power_desc[num++].number = -33;
			}


			if(rank1 == 53 || rank2 == 53 || rank3 == 53 ) //��_
			{
				strcpy(power_desc[num].name, _("��𐶐�", "Create Spirit Sake"));
				power_desc[num].level = 30;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_CHR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�u����v�𐶐�����B�Ȃ��A��_�͂������������ł��|��Ȃ��B",
                                        "Creates Spirit sake. A Sake God won't collapse from drinking.");
				power_desc[num++].number = -34;
			}


			if(rank1 == 54 || rank2 == 54 || rank3 == 54 ) //���_
			{
				strcpy(power_desc[num].name, _("����", "Tornado"));
				power_desc[num].level = 30;
				power_desc[num].cost = 27;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�^�[�Q�b�g�̈ʒu��(���x��*4)�̈З̗͂������N�����čU������B",
                                        "Creates a tornado in target location. (damage: level * 4)");
				power_desc[num++].number = -35;
			}

			if(rank1 == 55 || rank2 == 55 || rank3 == 55 ) //���_
			{
				strcpy(power_desc[num].name, _("����", "Call Lightning"));
				power_desc[num].level = 30;
				power_desc[num].cost = 27;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�^�[�Q�b�g�̈ʒu��(���x��*4)�̈З̗͂����N�����čU������B",
                                        "Makes lightning strike down at target position. (damage: level * 4)");
				power_desc[num++].number = -36;
			}

			if(rank1 == 56 || rank2 == 56 || rank3 == 56 ) //腖�
			{
				strcpy(power_desc[num].name, _("�ދ�����", "Dismissal Order"));
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�w��V���{���̃����X�^�[�����݂̃t���A����Ǖ�����B���s���邱�Ƃ�����B",
                                        "Banishes all monsters of chosen symbol from current level. Might fail.");
				power_desc[num++].number = -37;
			}

			if(rank1 == 57 || rank2 == 57 || rank3 == 57 ) //�M��_
			{
				strcpy(power_desc[num].name, _("���Ӗ��E", "Massacre Nearby"));
				power_desc[num].level = 35;
				power_desc[num].cost = 60;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 40;
				power_desc[num].info = _("���͂̃����X�^�[�����݂̃t���A����Ǖ�����B���s���邱�Ƃ�����B",
                                        "Banishes all nearby monsters from current level. Might fail.");
				power_desc[num++].number = -38;

				strcpy(power_desc[num].name, _("�M��", "Curse of Retribution"));
				power_desc[num].level = 40;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = _("��莞�ԁA�������U�����󂯂��Ƃ������_���[�W�𑊎�ɕԂ��B",
                                        "Temporarily, makes you respond to received attacks by dealing the same damage to the enemy.");
				power_desc[num++].number = -39;

			}
			if(rank1 == 58 || rank2 == 58 || rank3 == 58 ) //���ׂ̐_
			{
				strcpy(power_desc[num].name, _("�����O���X", "Raw Logrus"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("���x��*5�̈З͂̃J�I�X�����̋�������čU������B",
                                        "Fires a ball of chaos. (damage: level * 5)");
				power_desc[num++].number = -40;

				strcpy(power_desc[num].name, _("�����ϗe", "Alter Reality"));
				power_desc[num].level = 32;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("��莞�Ԍo�ߌ�A���݂̃t���A���Đ��������B�n���N�G�X�g�_���W�����ł͖����B",
                                        "Recreates current level after some delay. Does not work on surface or on quest levels.");
				power_desc[num++].number = -41;

			}

			if(rank1 == 59 || rank2 == 59 || rank3 == 59 ) //���_
			{
				strcpy(power_desc[num].name, _("����", "Demonblade"));
				power_desc[num].level = 30;
				power_desc[num].cost = 40;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�������Ă��镐�킪�����̐��͂ɑ�_���[�W��^���A����ɐ�������HP���z������悤�ɂȂ�B",
                                        "Makes currently wielded weapon deal large damage to lawfully aligned enemies, and also drain HP from living beings.");
				power_desc[num++].number = -42;

				strcpy(power_desc[num].name, _("�w���t�@�C�A", "Hellfire"));
				power_desc[num].level = 40;
				power_desc[num].cost = 60;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 60;
				power_desc[num].info = _("���x��*5�̈З͂̒n���̍��΂�����čU������B�V�g�Ȃǂ̐��Ȃ鑶�݂ɂ͑�_���[�W��^����B",
                                        "Fires a ball of hellfire that deals large damage to angels or other holy beings. (damage: level * 5)");
				power_desc[num++].number = -43;

			}
			if(rank1 == 60 || rank2 == 60 || rank3 == 60 ) //���|�̑剤
			{
				strcpy(power_desc[num].name, _("���e�I�X�g���C�N", "Meteor Strike"));
				power_desc[num].level = 40;
				power_desc[num].cost = 64;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�^�[�Q�b�g�Ƃ��̎��ӂɋ��͂�覐Α����U�����s���B",
                                        "Launches powerful meteor attacks near the target.");
				power_desc[num++].number = -44;
			}
			if(rank1 == 61 || rank2 == 61 || rank3 == 61 ) //���҂̐_
			{
				strcpy(power_desc[num].name, _("���ҏ���", "Summon Dead"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�A���f�b�h�̔z������́`������������B", "Summons one or more undead as your followers.");
				power_desc[num++].number = -45;
			}
			if(rank1 == 62 || rank2 == 62 || rank3 == 62 ) //�א_
			{
				strcpy(power_desc[num].name, _("���f", "Bedazzle"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("���E���̓G��N�O�E���|�E����������B", "Stuns, terrifies and confuses all enemies in sight.");
				power_desc[num++].number = -46;

				strcpy(power_desc[num].name, _("���͋z��", "Drain Mana"));
				power_desc[num].level = 30;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�G��̂ɐ��_�����_���[�W��^���A������MP���񕜂���B",
                                        "Deals mental damage to a single enemy and restores your MP.");
				power_desc[num++].number = -47;
			}
			if(rank1 == 63 || rank2 == 63 || rank3 == 63 ) //�O�Ȃ�_
			{
				strcpy(power_desc[num].name, _("�R�Y�~�b�N�E�z���[", "Cosmic Horror"));
				power_desc[num].level = 30;
				power_desc[num].cost = 32;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�G��̂ɋ���Ȑ��_�U�����s���B�A���f�b�h�∫���ɂ͌����ɂ����B",
                                        "Performs an intense mental attack against a single enemy. Has little effect on undead and demons.");
				power_desc[num++].number = -48;
			}
			if(rank1 == 66 || rank2 == 66 || rank3 == 66 ) //�ԉΐE�l
			{
				strcpy(power_desc[num].name, _("�t�@�C�A�E�{�[��", "Fire Ball"));
				power_desc[num].level = 20;
				power_desc[num].cost = 12;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 10;
				power_desc[num].info = _("�З�(���x��*2)�̉Ή������̋�����B",
                                        "Fires a ball of fire. (damage: level * 2)");
				power_desc[num++].number = -49;
			}
			if(rank1 == 67 || rank2 == 67 || rank3 == 67 ) //�V�F�t
			{
				strcpy(power_desc[num].name, _("�H������", "Create Food"));
				power_desc[num].level = 20;
				power_desc[num].cost = 12;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 20;
				power_desc[num].info = _("�H���𐶐�����B", "Creates a food ration.");
				power_desc[num++].number = -50;
			}
			if(rank1 == 70 || rank2 == 70 || rank3 == 70 ) //�T
			{
				strcpy(power_desc[num].name, _("�����X�^�[���m", "Detect Monsters"));
				power_desc[num].level = 10;
				power_desc[num].cost = 5;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 10;
				power_desc[num].info = _("�߂��̃����X�^�[�����m����B", "Detects nearby monsters.");
				power_desc[num++].number = -51;
			}
			if(rank1 == 75 || rank2 == 75 || rank3 == 75 ) //���̐_
			{
				strcpy(power_desc[num].name, _("���f", "Bedazzle"));
				power_desc[num].level = 30;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = _("���E���̓G��N�O�E���|�E����������B", "Stuns, terrifies and confuses all enemies in sight.");
				power_desc[num++].number = -46;
			}

			if(rank1 == 76 || rank2 == 76 || rank3 == 76 ) //���̐_
			{
				strcpy(power_desc[num].name, _("�����̔�", "Dimension Door"));
				power_desc[num].level = 35;
				power_desc[num].cost = 26;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 40;
				power_desc[num].info = _("�w�肵���ꏊ�փe���|�[�g����B���s���邱�Ƃ�����B",
                                        "Teleports to specified location. Might fail.");
				power_desc[num++].number = -23;
			}
			if(rank1 == 77 || rank2 == 77 || rank3 == 77 ) //�b��_
			{
				strcpy(power_desc[num].name, _("���틭��", "Enchant Weapon"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 50;
				power_desc[num].info = _("�w�肵������̏C���l����������B16�𒴂��Ȃ��B",
                                        "Increases bonuses on specified weapon. Cannot exceed 16.");
				power_desc[num++].number = -52;

				strcpy(power_desc[num].name, _("�h���", "Enchant Armor"));
				power_desc[num].level = 30;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 50;
				power_desc[num].info = _("�w�肵���h��̏C���l����������B16�𒴂��Ȃ��B",
                                        "Increases AC bonus on specified armor. Cannot exceed 16.");
				power_desc[num++].number = -53;
			}
			if(rank1 == 78 || rank2 == 78 || rank3 == 78 ) //�z�R
			{
				strcpy(power_desc[num].name, _("�f�ފ��m", "Detect Materials"));
				power_desc[num].level = 20;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 30;
				power_desc[num].info = _("�t���A�ɑf�ރJ�e�S���̃A�C�e��������΂�����@�m����B",
                                        "Detects material items on this level.");
				power_desc[num++].number = -54;
			}



	*num_return = num;
}



/*:::��ǐ_�l�p���C�V�����p���[���s���@�s���������Ƃ�TRUE*/
bool deity_racial_power_aux(s32b command)
{
	int dir;
	int plev = p_ptr->lev;
	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_format(_("deity_racial_power_aux()����ǐ_�l�ȊO����Ă΂ꂽ",
                    "deity_racial_power_aux() called for a non-stray-god."),command);
		 return FALSE;
	}

	switch(command)
	{
		case -1: //����m��
		{
			int base = 20;
			msg_print(_("�{��̗͂��Ăъo�܂����I", "You awaken the power of anger!"));
			set_shero(randint1(base) + base, FALSE);
			hp_player(30);
			set_afraid(0);

		}
		break;
		case -2: //�ڗ���
		{
			if (!ident_spell(FALSE)) return FALSE;
		}
		break;
		case -3: //����
		{
			msg_print(_("�����̗͂��g�����E�E", "You use the power of curing..."));
			set_poisoned(0);
			set_stun(0);
			set_cut(0);
			set_image(0);
			set_blind(0);
			hp_player(30);
		}
		break;
		case -4: //�V���E�g
		{
			msg_print(_("�n�̉ʂĂ܂œ͂��V���E�g��������I",
                        "You shout loud enough to be heard on ends of the earth!"));
			project(0, 7, py, px, plev * 3 + 50, GF_SOUND, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			aggravate_monsters(0,FALSE);
		}
		break;
		case -5: //�`���[�������X�^�[
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���Ȃ��͉��_�����݂��E�E", "You show your soft side..."));
			charm_monster(dir, plev * 2);
		}
		break;
		case -6: //���_�W��
		{
			int base = 10;
			msg_print(_("���_���W������ƁA�����Ⴆ�Ă����I", "You concentrate your mind, and your head clears!"));
			set_tim_addstat(A_INT,105,base + randint1(base),FALSE);
		}
		break;
		case -7: //�_���X
		{
			int base = 10;
			msg_format(_("%s�́A���邮��Ƃ��ǂ����B", "%s dances round and round!"),player_name);
			set_tim_addstat(A_CHR,105,base + randint1(base),FALSE);
		}
		break;
		case -8: //�m�C���g
		{
			int base = 25;
			msg_print(_("�C���̑����𔭂����I", "You take a deep breath and feel full of energy!"));
			set_hero(randint1(base) + base, FALSE);
			hp_player(10);
			set_afraid(0);
		}
		break;
		case -9: //��������
		{
			if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				msg_print(_("�����͌���Ȃ������B", "No animals appear."));
			else
				msg_print(_("�������Ăъ񂹂��B", "You call forth animals."));

		}
		break;
		case -10: //�Ă���
		{
			object_type forge;
			object_type *q_ptr = &forge;

			msg_print(_("���Ȃ��͈����Ă��n�߂��B�F�������肪�������߂��E�E",
                        "You start roasting sweet potatoes. There is an appetizing scent..."));
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			drop_near(q_ptr, -1, py, px);
			aggravate_monsters(0,FALSE);
		}
		break;
		case -11: //��C�̃u���X
		{
			int dam = p_ptr->chp / 5;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���Ȃ��͐���������N�������I", "You summon a blizzard!"));
			fire_ball(GF_COLD, dir, dam, -2);
		}
		break;
		case -13: //�Ή��̃u���X
		{
			int dam = p_ptr->chp / 5;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���Ȃ��͉΂𐁂����I", "You breathe fire!"));
			fire_ball(GF_FIRE, dir, dam, -2);
		}
		break;
		case -14: //�q�b�g���A�E�F�C
		{
			if(!hit_and_away()) return FALSE;
		}
		break;

		case -15: //��
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball_hide(GF_CAUSE_4, dir, damroll(15, 15), 0);
		}
		break;

		case -16: //�L�m�R����
		{
			int num = randint1(3) + plev / 7;
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, plev, SUMMON_MUSHROOM, (PM_FORCE_PET)))	flag = TRUE;
			}
			if(flag) msg_print(_("��Ȕz���B�����ꂽ�B", "Some weird followers have appeared."));
			else msg_print(_("�z���͌���Ȃ������B", "No followers appears."));
		}
		break;
		case -17: //*�j��*
		{
			msg_print(_("�j��̗͂������������I", "You unleash destructive power!"));
			destroy_area(py, px, 12 + randint1(4), FALSE,FALSE,FALSE);
		}
		break;
		case -18: //���̌�
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���̌��𓊂��������I", "You throw a Psycho-Spear!"));
			fire_beam(GF_PSY_SPEAR, dir, randint1(plev*3)+plev*3);
		}
		break;
		case -19: //�n�k
		{

			msg_print(_("�n�ʂ��������h��n�߂��E�E", "The earth starts to rumble violently..."));
			earthquake(py, px, 8);
		}
		break;
		case -20: //���僌�[�U�[
		{
			int damage = p_ptr->chp / 3;
			if(!damage) damage = 1;
			if (!get_aim_dir(&dir)) return FALSE;

			msg_print(_("���Ȃ��͌���������I", "You unleash light!"));
			fire_spark(GF_NUKE,dir,damage,2);
		}
		break;

		case -21: //�z�[���[�t�@�C�A
		{
			int damage = plev * 5;
			if (!get_aim_dir(&dir)) return FALSE;

			if(!fire_ball_jump(GF_HOLY_FIRE, dir, damage, 4,_("ῂ��������o�������I", "A sphere of dazzling light appears!"))) return FALSE;
		}
		break;
		case -22: //�����~��
		{
			msg_print(_("��C�̖��͂���荞�񂾁B", "You absorb ambient mana."));
			p_ptr->csp += (3 + plev/10);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
		case -23: //�����̔�
		{
				msg_print(_("�����̔����J�����B�ړI�n��I��ŉ������B",
                            "You open a dimensional door. Choose your destination."));
				if (!dimension_door(D_DOOR_NORMAL)) return FALSE;
		}
		break;
		case -24: //��������
		{
			msg_print(_("�����珬�Ƃ���o���A�傫���U�����I", "You pull out a small mallet and swing it in a wide arc!"));
			set_tim_lucky(20,FALSE);
		}
		break;
		case -25: //�A�N�A�u���X
		{
			int dam = p_ptr->chp / 4;
			if(dam<1) dam=1;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���@�̖A��f���������B", "You blow magical bubbles."));
			fire_ball(GF_WATER, dir, dam, -2);
		}
		break;
		case -26: //���C���V���g����
		{
			int rad = 6;
			msg_print(_("��Q�������N�������I", "You create a maelstrom!"));
			project(0, rad, py, px, rad, GF_WATER_FLOW, PROJECT_GRID, -1);
			project(0, rad, py, px, plev * 8, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);

		}
		break;
		case -27: //����
		{
			int v = 20 + randint1(20);
			msg_print(_("���Ȃ��̓{�肪���������I", "You are bursting with rage!"));
			set_shero(v, FALSE);
			hp_player(30);
			set_afraid(0);
			set_fast(v, FALSE);
		}
		break;
		case -28: //���o
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
		break;
		case -29: //�󕠏[��
		{
			msg_print(_("�L���̗͂����̐g�ɏW�߂��E�E", "You gather power of fertility in your body..."));
			set_food(PY_FOOD_MAX - 1);
		}
		break;
		case -30: //�X��&�Ԑ���
		{
			msg_print(_("�L���̗͂���n�ɖ������I", "The power of fertility fills the earth!"));
			project(0, 7, py, px, 0, GF_MAKE_FLOWER, (PROJECT_GRID | PROJECT_HIDE), -1);
		}
		break;
		case -31: //���R����
		{
			if (summon_kin_player(p_ptr->lev, py, px, (PM_FORCE_PET | PM_ALLOW_GROUP)))
				msg_print(_("���Ȃ��͉��R�����W�����I", "You call for reinforcements!"));
			else
				msg_print(_("���R�͗��Ȃ������B", "No reinforcements arrive."));
		}
		break;
		case -32: //����
		{
			msg_print(_("���Ȃ��͌����ȓ��̂����������I", "You show off your beautiful body!"));
			charm_monsters(p_ptr->lev * 2);
		}
		break;
		case -33: //����
		{
			msg_print(_("���Ȃ��͎����̏��ɏW�������B", "You concentrate on your wounds."));
			hp_player(plev * 6);
		}
		break;
		case -34: //��𐶐�
		{
			object_type forge;
			object_type *q_ptr = &forge;
			msg_print(_("���Ȃ��̎�̒��ɂ������o�������B", "Sake appears in your hand."));
			object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_REISYU));
			apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
			(void)drop_near(q_ptr, -1, py, px);
		}
		break;
		case -35: //����
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_TORNADO, dir, plev * 4, 4,_("�������N�������I", "A tornado appears!"))) return FALSE;
		}
		break;
		case -36: //����
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_ELEC, dir, plev * 4, 4,_("���𗎂Ƃ����I", "Lightning strikes!"))) return FALSE;
		}
		break;
		case -37: //���E
		{
			symbol_genocide(plev * 3, TRUE,0);
		}
		break;

		case -38: //���Ӗ��E
		{
			mass_genocide(plev * 2, TRUE);
		}
		break;
		case -39: //�M��
		{
			int base = 20;
			set_tim_eyeeye(randint1(base) + base, FALSE);
		}
		break;
		case -40: //�����O���X
		{

			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("�������̔g����������I", "You release a wave of anarchy!"));
			fire_ball(GF_CHAOS, dir, plev * 5, 4);
		}
		break;
		case -41: //�����ϗe
		{

			alter_reality();
		}
		break;
		case -42: //����
		{
			int base = 10;
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			{
				msg_format(_("����������Ă��Ȃ��B", "You're not wielding a weapon."));
				return FALSE;
			}
			set_ele_attack(ATTACK_DARK, base + randint1(base));
		}
		break;
		case -43: //�w���t�@�C�A
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_HELL_FIRE, dir, plev * 5, 4,_("�n���̉����Ăяo�����I", "Fires of hell come forth!"))) return FALSE;
		}
		break;
		case -44: //���e�I�X�g���C�N
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_METEOR, dir, plev * 6 + randint1(plev * 6), 6,_("覐΂��~���Ă����I", "A meteor crashes down!"))) return FALSE;
		}
		break;
		case -45: //���ҏ���
		{
			int num = randint1(3);
			bool flag = FALSE;

			for(;num>0;num--)
			{
				if (summon_specific(0, py, px, plev, SUMMON_UNDEAD, (PM_FORCE_PET)))	flag = TRUE;
			}
			if(flag) msg_print(_("���҂������n�̒ꂩ�甇���o���Ă����B", "The dead rise out of the depths of the earth."));
			else msg_print(_("�z���͌���Ȃ������B", "No followers appear."));
		}
		break;
		case -46: //���f
		{
			int power = plev * 4;
			msg_format(_("���_�g��������B", "You release a mental wave."));
			stun_monsters(power);
			confuse_monsters(power);
			turn_monsters(power);
		}
		break;
		case -47: //���͋z��
		{
			int dice= 1;
			int sides= plev * 2;
			int base = plev;
			if (!get_aim_dir(&dir)) return FALSE;

			fire_ball_hide(GF_PSI_DRAIN, dir, damroll(dice,sides)+base , 0);
		}
		break;
		case -48: //�R�Y�~�b�N�E�z���[
		{
			int dam = damroll(10,10) + plev * 7;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format(_("���Ȃ��͖��󂵂������F���I���|�𔭌��������I",
                        "You reveal unspeakable cosmic horrors!"));
			fire_ball_hide(GF_COSMIC_HORROR, dir, dam, 0);
		}
		break;
		case -49: //�t�@�C�A�E�{�[��
		{
			int dam = plev * 2;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format(_("�ԉ΂𓊂����I", "You throw fireworks!"));
			fire_ball(GF_FIRE, dir, dam, 3);
		}
		break;
		case -50: //�H������
		{
			object_type forge, *q_ptr = &forge;
			msg_print(_("�߂��̐H�ׂ�ꂻ���Ȃ��̂�H���Ɏd���Ă��B",
                        "You prepare some food out of nearby edible stuff."));
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, py, px);
			break;
		}
		break;
		case -51: //�����X�^�[���m
		{
			msg_print(_("�����Ǝ��𐟂܂����E�E", "You listen carefully..."));
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			detect_monsters_normal(DETECT_RAD_DEFAULT);
		}
		break;
		case -52: //���틭��
		{
			if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return FALSE;
		}
		break;
		case -53: //�h���
		{
			if (!enchant_spell(0, 0, randint0(3) + 2)) return FALSE;
		}
		break;
		case -54: //�f�ފ��l
		{
			msg_print(_("���Ȃ��̓t���A�̒n���ɏW�������E�E",
                        "You concentrate on local geology..."));
			search_specific_object(3);
			break;
		}
		break;

		default:
		msg_format(_("deity_racial_power_aux()�ɕs���ȃR�}���h�l(%d)���n���ꂽ",
                    "Incorrect command (%d) passed to deity_racial_power_aux()"),command);
		return FALSE;
	}

	return TRUE;
}



const deity_type deity_table[MAX_DEITY_TABLE_ENTRY]={
	{99,_("�n�_�p�_�~�[", "starting dummy"),0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0,0},

//���������N�@��Q�͏H�o���p  �����łȂ��Ȃ�L�������C�N����[3]�`[14]�̃����_���Ɍ��܂�
	//�x����
	{1,_("����̐_�l", "God of Fruit"),
	128,80,80,
	{-1,-2,0,-1,0,1},-3,0,2,2,2,0,0,0},
	//���͈ێ�
	{1,_("�g�t�̐_�l", "Autumn God"),
	80,140,80,
	{1,-1,-1,1,0,-1},7,2,8,3,2,5,7,-4},
	//r��
	{1,_("�ۂނ������n���l", "Mossy Jizo"),
	180, 60,80,
	{-2,-1,1,-1,1,0},-3,4,4,3,-3,0,-5,-5},
	//�������F
	{1,_("�Y���ꂽ���c�_", "Forgotten God"),
	100,60, 80,
	{-1,0,1,-1,-1,-1},0,0,3,2,3,0,0,-10},
	//(��
	{1,_("�Â��~�̖؂̐_�l", "Plum Tree God"),
	128,20,80,
	{0,-1,1,-3,0,1},-2,4,5,4,3,0,0,0},
	//r��
	{1,_("����p���̐�B�_", "Lavatory God"),
	160,60,80,
	{0,-1,1,-2,1,-3},-3,-2,0,2,-2,-5,0,0},
	//r��
	{1,_("�ςȌ`�̊�̐_�l", "Odd Stone God"),
	30,180,80,
	{1,-2,-1,-2,2,-2},-5,-2,3,2,0,0,10,-5},
	//r��
	{1,_("�Â��Ջ�̕t�r�_", "Ceremony Tsukumo"),
	160,140,80,
	{-1,-1,-1,-1,-1,-1},-2,8,-2,0,0,-5,-5,-5},
	//r��
	{1,_("�l�����ꂽ���̎�", "Lord of Swamp"),
	100,120,80,
	{1,-2,0,-1,0,-2},3,-3,0,2,2,5,2,2},
	//��
	{1,_("�_�i�����ꂽ��h", "Deified Eagle"),
	50,180,80,
	{0,-2,-1,2,-1,-1},-3,-2,3,2,4,0,10,-10},
	//~�l
	{1,_("����p���̎Y�y�_", "Ubusunagami"),
	120,70,80,
	{-1,-1,1,-1,-1,1},0,0,0,2,2,0,-3,-3},
	//�Ȃ�
	{1,_("���������Ƃ̎��_", "Old Family God"),
	128,128,80,
	{0,0,0,0,0,0},0,0,0,0,0,0,0,0},
	//r��
	{1,_("�_�쉻�������l��", "Divine Spirit"),
	200,120,80,
	{-2,-1,0,-1,-2,-1},-2,3,-3,0,0,0,0,0},
	//~��
	{1,_("�N�o����b�̉��g", "Beast Avatar"),
	60,150,80,
	{0,-2,-1,0,1,-2},-2,-3,3,2,3,6,9,-6},


//��񃉃��N�@3*3*3�������Ă���
//[15]
	//�������@���D��@������
	//r��
	//���������_�C�X�{�[�i�X
	{2,_("�R�m�̐_�l", "Knight God"),
	230,230,200,
	{3,0,3,0,2,2},0,2,6,0,0,10,25,10},

	//�������@���D��@������
	//r�� ���������_�C�X�{�[�i�X
	{2,_("���̐_�l", "Blade God"),
	140,255,200,
	{3,-2,0,3,2,2},0,5,3,1,0,5,30,0},

	//�ᒁ���@���D��@������(����m��)
	//r�Δj
	{2,_("�ΎR�̐_�l", "Volcano God"),
	40,230,200,
	{5,-1,-2,0,6,0},0,5,5,-1,3,10,25,25},

	//��⍂�����@���D��@������(�ڗ���)
	//����͌���
	{2,_("�����̐_�l", "Trade God"),
	200,80,200,
	{0,2,1,1,0,7},3,8,0,0,5,0,-5,-5},

	//�������@���D��@������(����)
	//r�Ŗ�
	{2,_("���a���Ђ̐_�l", "Health God"),
	135,100,200,
	{1,0,0,0,4,2},0,3,8,2,4,0,5,5},

	//�ᒁ���@���D��@������(�����U��)
	//r����
	{2,_("���b�N�̐_�l", "Rock God"),
	30,140,180,
	{1,0,0,1,1,2},-5,3,2,-2,0,6,10,5},

	//�������@��D��@������(�`���[�������X�^�[)
	//~�l��
	{2,_("�����т̐_�l", "Marriage God"),
	160,50,180,
	{-2,1,2,0,2,4},0,5,5,1,5,0,5,5},

	//�������@��D��@������(�m�\�ꎞ����)
	//r�� (�m
	{2,_("�w�Ɛ��A�̐_�l", "Education God"),
	200,10,180,
	{-1,4,3,1,0,3},5,8,6,3,6,0,0,0},

	//�ᒁ���@��D��@������(���͈ꎞ�㏸)
	//r��(��
	{2,_("���x�̐_�l", "Dance God"),
	40,40,180,
	{1,0,1,3,1,4},0,5,6,1,3,3,10,0},

	//�������@���D��@������(�m�C���g)
	//�i�����_�C�X�����A�U���񐔏㏸
	{2,_("�����̐_�l", "Bushido God"),
	200,230,130,
	{2,0,0,2,2,2},0,-5,-5,0,0,10,30,10},

	//�������@���D��@������
	//���������_�C�X�{�[�i�X
	{2,_("�؂���̐_�l", "Woodcutter God"),
	120,200,120,
	{0,0,2,0,2,0},1,5,5,4,5,0,0,10},

	//�ᒁ���@���D��@������(��������)
	//r�� �}��
	{2,_("���т̐_�l", "Deep Forest God"),
	50,230,100,
	{2,-2,-1,2,3,-1},-3,-3,0,3,0,10,15,5},


	//�������@���D��@������
	//r��
	//�Ă���(�Ă��𐶐��@���َq�����@�����x�啝�񕜁AMP���񕜁A���������͂̓G���N����)
	{2,_("�Ă����̐_�l", "Sweet Potato God"),
	175,100,120,
	{1,0,2,0,1,2},2,3,10,2,5,2,6,6},

	//�������@���D��@������
	//�|���������͎�
	{2,_("��l�̐_�l", "Huntsman God"),
	115,150,110,
	{0,1,1,3,0,1},7,7,3,3,6,5,10,30},

	//�ᒁ���@���D��@������
	//���@��x�ቺ
	{2,_("�����̐_�l", "Randomness God"),
	0,128,128,
	{0,0,0,0,0,0},10,10,20,0,10,0,0,0},


	//�������@��D��@������
	//(�� r��
	{2,_("�@�D��̐_�l", "Weaver God"),
	240,30,110,
	{0,2,0,2,0,2},9,8,7,3,4,0,12,0},

	//�������@��D��@������
	//�x����
	{2,_("���l�̐_�l", "Traveller God"),
	130,60,90,
	{0,0,2,1,2,1},2,3,4,3,8,0,5,10},

	//�ᒁ���@��D��@������
	//���͓���
	{2,_("�哹�|�̐_�l", "Performer God"),
	50,50,110,
	{0,2,0,3,-1,2},15,15,0,-2,6,20,20,20},


	//�������@���D��@�ᖼ��
	//r�ΐ�
	{2,_("��鯂̐_�l", "Drought God"),
	210,230,30,
	{2,1,-1,1,3,-3},0,3,4,1,2,10,15,5},

	//�������@���D��@�ᖼ��(��C�̃u���X)
	//r��[��
	{2,_("����̐_�l", "Blizzard God"),
	150,230,65,
	{1,1,-2,1,3,-2},0,3,4,1,-2,10,5,5},

	//�ᒁ���@���D��@�ᖼ��
	//�� ����+2
	{2,_("�앪�̐_�l", "Nowaki God"),
	20,240,60,
	{2,-2,-2,2,2,-2},-2,-4,-2,3,4,10,20,10},


	//�������@���D��@�ᖼ��
	//�Z���Ƀ_�C�X�{�[�i�X
	//r��
	{2,_("�ÎE�҂̐_�l", "Assassin God"),
	190,160,30,
	{1,-2,-2,4,1,-2},3,5,-2,5,8,5,20,5},

	//�������@���D��@�ᖼ��(�Ή��̃u���X)
	//r��[��
	{2,_("�΍Ђ̐_�l", "Flame God"),
	120,150,30,
	{1,-1,-2,1,2,-3},-5,3,6,2,2,10,15,0},

	//�ᒁ���@���D��@�ᖼ��(H&A)
	//�x
	{2,_("�D�_�̐_�l", "Thief God"),
	20,128,20,
	{1,1,-2,3,0,-3},10,3,-3,5,8,0,4,-4},

	//�������@��D��@�ᖼ��(���̌���)
	//r��
	{2,_("�m�l�`�̐_�l", "Straw Doll God"),
	220,10,30,
	{-1,3,0,1,1,-2},3,12,7,4,2,0,5,-5},

	//�������@��D��@�ᖼ��(������)
	//r�� �}��
	{2,_("�L�m�R�̐_�l", "Mushroom God"),
	100,30,60,
	{-1,-1,-1,-1,4,-2},3,3,3,4,0,0,20,0},

	//�ᒁ���@��D��@�ᖼ��
	//r��
	{2,_("�u�a�̐_�l", "Pestilence God"),
	10,30,45,
	{0,0,1,0,2,-3},3,3,3,2,6,0,7,7},


//��O�����N
	//[42]
	//�ᒁ���@���D��@�������i*�j��*�A���̌��A�ǖx�ړ��j
	//r�׋�����
	{3,_("�j��_", "Destroyer God"),
	0,255,255,
	{9,2,2,3,4,-3},-8,-3,10,-3,0,10,50,25},

	//�������@����D��@�������i�n�k�A�펞�ǔ����j
	//r���j�����x�}
	{3,_("��n�_", "Earth God"),
	128,80,255,
	{3,1,3,-2,5,3},5,10,15,3,10,5,20,10},

	//�������@��⍂�D��@�������i���僌�[�U�[�A�z�[���[�t�@�C�A�j
	//*��r�M�Ӑ����� [�� ���O����
	{3,_("���z�_", "Sun God"),
	225,200,255,
	{2,2,2,2,3,5},5,12,10,-2,10,0,10,20},

	//�������@���D��@�������i�����~���A�����̔�)
	//r����������
	{3,_("�V��", "Heavenly King"),
	255,90,255,
	{2,5,4,3,3,5},10,10,10,3,10,10,25,25},


	//��⍂�����@��D��@�������i��������(�ꎞ�I�ɔ��I�[���ȏ�̍K�^)�j
	//r�����Ӌ����x
	{3,_("���̐_", "Fortune God"),
	180,50,225,
	{1,2,3,2,2,6},5,8,12,2,20,5,5,5},

	//���ᒁ���@���D��@�������i�A�N�A�u���X�A���C���V���g�����j
	//*�� r�_�Η�Ő�
	{3,_("�C�_", "Sea God"),
	70,145,225,
	{3,2,3,2,3,2},0,6,12,3,7,5,20,15},


	//�������@���D��@�������i�����A�S���͍U���j
	//r���}�Γd(�r��
	{3,_("�S�_", "Kishin"),
	100,255,215,
	{7,-3,2,1,5,1},0,0,10,-1,5,10,25,25},

	//�������@���D��@��⍂�����i�󕠏[���A�X�ѐ����j
	//r���ŋ}
	{3,_("�L���_", "Fertility God"),
	140,90,160,
	{1,1,3,1,3,3},0,4,8,2,12,0,5,5},


	//�������@��D��@�������i�����~���j
	//~�� (�m��r��
	{3,_("�b�q�̐_", "Wisdom God"),
	225,25,160,
	{-2,5,5,1,0,3},5,10,10,2,10,5,-5,-5},


	//��⍂�����@���D��@�������@�~������
	//r���x�j�@����
	{3,_("�R�_", "War God"),
	180,240,140,
	{4,2,2,2,3,2},0,0,10,-1,5,10,25,25},


	//�ᒁ���@��D��@������(���З̓`���[�������X�^�[�A����)
	//r�����ח�(��
	{3,_("���~�̐_", "Love God"),
	60,0,140,
	{0,0,3,1,3,7},3,7,10,0,12,5,10,10},

	//���ᒁ���@���D��@������
	//r������
	//�������(���s�v)
	{3,_("��_", "Sake God"),
	55,120,120,
	{2,-2,2,1,2,3},-2,3,10,0,3,0,10,0},

	//�������@���D��@�������i����+5�A���A�����ړ��A�����j
	{3,_("���_", "Wind God"),
	100,200,120,
	{1,1,0,5,0,2},3,5,8,1,5,10,15,10},

	//�������@���D��@�������i�����j
	//*�dr����[�d
	{3,_("���_", "Thunder God"),
	95,205,120,
	{4,0,1,2,2,2},3,5,8,-2,10,10,30,10},


	//�������@��D��@������(���E)
	//�������Ӗ�����
	{3,_("腖�", "Yama"),
	255,80,120,
	{1,3,4,-1,2,1},5,10,12,1,10,10,0,0},


	//�������@���D��@���ᖼ���i���Ӗ��E�A�ڂɂ͖ځj
	//r�Í�����
	{3,_("�M��_", "Curse God"),
	120,180,50,
	{2,-1,1,1,4,-3},0,3,20,1,10,5,15,15},

	//�ᒁ���@���D��@�ᖼ�� (�����O���X�A�����ϗe)
	//r���׋� ~��
	{3,_("���ׂ̐_", "Chaos God"),
	0,120,50,
	{3,2,-2,2,3,-5},-5,-3,15,4,10,5,5,5},

	//�ᒁ���@���D��@�ᖼ�� (�����A�w���t�@�C�A�j
	//r�ΈÍ����� v��[��
	{3,_("���_", "Demon God"),
	0,255,50,
	{4,2,0,3,3,-8},0,7,12,-2,0,5,25,0},


	//��⍂�����@���D��@�ᖼ�� (���e�I�X�g���C�N�A���E�����Q)
	//r�M�Ë���
	{3,_("���|�̑剤", "Lord of Terror"),
	160,225,30,
	{3,-2,-2,0,5,-5},-5,3,10,-3,10,5,15,35},

	//�������@��D��@�ᖼ�� (�n���Ɖu�A�A���f�b�h�����j
	//*�� r���� ~�� v��
	{3,_("���҂̐_", "God of the Dead"),
	200,20,30,
	{1,2,2,-2,2,-5},0,10,16,4,10,5,15,10},

	//�������@��D��@�ᖼ�� (���E�����f�A���͋z��)
	//r��Í����� v�� ~��[��
	{3,_("�א_", "Evil God"),
	130,120,30,
	{2,2,3,0,2,-6},5,12,10,0,10,5,5,5},

	//63
	//�������@���D��@���ᖼ�� (�R�Y�~�b�N�z���[)
	//*�� r�_�Ë��� v��
	{3,_("�O�Ȃ�_", "Outer God"),
	50,120,0,
	{5,5,-5,0,5,-10},0,0,20,4,5,10,25,15},


	///mod150228 �ǉ�

	//(��
	{2,_("�R�f�̐_�l", "Kemari God"),
	200,95,70,
	{0,1,1,2,-1,2},3,2,3,0,2,10,12,10},

	//r����
	{2,_("���Y�F��̐_�l", "Delivery God"),
	120,30,145,
	{-2,1,3,-2,3,2},3,4,9,1,3,10,0,0},

	//r�΁@�t�@�C�A�{�[��
	{2,_("�ԉΐE�l�̐_�l", "Fireworks God"),
	70,150,120,
	{1,1,0,1,1,0},3,10,0,0,0,10,0,20},

	//�}�x�@�H������
	{2,_("�V�F�t�̐_�l", "Chef God"),
	120, 80,150,
	{2,2,-2,2,-1,1},10,5,-2,1,5,10,12,0},

	//r�Ζ�
	{2,_("���܂ǂ̐_�l", "Kamado God"),
	90, 100,80,
	{1,1,0,0,2,1},2,5,2,2,3,10,5,0},

	//(�r��
	{2,_("�؃g���̐_�l", "Training God"),
	160, 150,80,
	{5,0,-2,0,2,-1},0,0,-2,1,0,10,20,0},

	//�����X�^�[���m
	{2,_("�T�̐_�l", "Wolf God"),
	100, 140,60,
	{2,-2,-1,2,1,-2},0,-2,-2,3,4,10,10,0},

	//r��
	{2,_("���̐_�l", "Waterfall God"),
	80, 130,90,
	{2,-1,0,0,1,0},0,2,4,3,1,2,10,10},

	//r����
	{2,_("�����̐_�l", "Flood God"),
	140, 80,150,
	{1,1,2,1,3,1},0,2,7,3,1,2,10,10},

	//r�M��
	{2,_("�[�Ă��̐_�l", "Sunset God"),
	70, 100,85,
	{0,0,1,1,1,1},0,2,7,3,2,2,3,3},


	//�� ���SESP
	{3,_("��_", "Pursuit God"),
	80, 180,110,
	{3,1,0,3,2,1},7,2,2,4,10,10,10,20},

	//r�Ë���
	//���f
	{3,_("���̐_", "Moon God"),
	80, 90,120,
	{1,2,1,2,3,3},3,12,12,3,0,3,5,8},

	//r�����׎�
	//�����̔�
	{3,_("���̐_", "Dream God"),
	30, 60,150,
	{-1,2,4,1,2,3},3,5,10,2,0,3,3,3},

	//r�ΑM
	//���틭���A�h���
	{3,_("�b��_", "Blacksmith God"),
	150, 140,80,
	{3,2,-1,3,2,1},6,12,3,1,0,7,22,3},

	//78
	//r��
	//�f�ފ��m
	{2,_("�z�R�̐_", "Mining God"),
	100, 120,100,
	{2,0,-1,1,2,0},3,5,5,2,6,3,8,2},

	//79 ���ԗp
	//r����;��
	{ 1,_("���q��̃��[�_�[", "Mizuko Spirit"),
		180,50,200,
		{ -1,-1,1,1,-1,1 },4,0,-3,2,6,0,-10,5 },

//���㑝�₷�ɂ��Ă�125��ނ܂łɂ��Ă���

	{0,_("�I�[�p�_�~�[", "terminator dummy"),0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0,0}
};

///mod150315 �d���p���Z����
/*:::�d���\�͕\�@�z��Y����p_ptr->race_multipur_val[0]�ɕۑ�����*/
const fairy_type fairy_table[FAIRY_TYPE_MAX+2] =
{
	{100,_("�n�[�_�~�[���\�͕t�^�Ȃ�", "starting dummy - no ability")}, //���j�[�N�N���X�Ȃǂ͂��̒l�ɂȂ�
//1
	{20,_("�h�A���J���Ȃ�������x�̔\��", "Capable of making doors unopenable")},
	{40,_("�Ƃ��ǂ����b�L�[����������x�̔\��", "Capable of occasional lucky dodges")}, //�{���g�𒆊m���Ŗ�����
	{30,_("�L�m�R������������x�̔\��", "Capable of telling mushrooms apart")},
	{90,_("�L�ɍD�������x�̔\��", "Capable of being loved by cats")}, //f�V���{�����F�D�I
	{65,_("�������Ō��C���o����x�̔\��", "Capable of being energetic in warm places")},

	{80,_("�ǂ��h�A�ɕς�����x�̔\��", "Capable of turning walls into doors")},
	{90,_("����ȓG���猩����ɂ������x�̔\��", "Capable of hiding from gigantic enemies")},
	{55,_("�Ԃ������Ă���Ƌ����Ȃ���x�̔\��", "Capable of being stronger while holding a flower")}, //����u�ԁv��sides+3
	{50,_("���z�̕R���ł����x�̔\��", "Capable of having a strong purse string")},//���𓐂܂�Ȃ�
	{80,_("������Ŏq��̂��̂����x�̔\��", "Capable of singing lullabies in dragon tongue")},//����Ȃ��G�ɂ��L���H
//11
	{30,_("���������������x�̔\��", "Capable of running away quickly")},//HP��1/2�ȉ��̂Ƃ������ړ�
	{80,_("����ƋL�����������x�̔\��", "Capable of losing memories when falling asleep")},
	{70,_("�Q�V��U�����x�̔\��", "Capable of inducing oversleeping")},
	{50,_("�S�[�E�F�X�g������x�̔\��", "Capable of going west")},
	{40,_("�אڂ����G�ƈʒu����������x�̔\��", "Capable of swapping positions with adjacent enemies")},

	{50,_("���܂��Ȃ������ӂȒ��x�̔\��", "Capable at charms and wards")},//���@�h��+
	{50,_("�������ڂ����ӂȒ��x�̔\��", "Capable at playing hide-and-seek")},//�B��+
	{40,_("��T�������ӂȒ��x�̔\��", "Capable at searching for treasure")}, //�T��+
	{30,_("�_�[�c�����ӂȒ��x�̔\��", "Capable at playing darts")}, //�ˌ�+
	{20,_("�p�Y�������ӂȒ��x�̔\��", "Capable at solving puzzles")}, //����+
//21
	{50,_("���E�̋C�z�����m������x�̔\��", "Capable of detecting fellow fairies")},//�d�����m
	{60,_("���O�ő�����ׂ���x�̔\��", "Capable of quickly flying while outdoors")},//���[���h�}�b�v�ňړ����x������
	{70,_("���S�C�����ӂȒ��x�̔\��", "Capable at using water guns")},//��З̓E�H�[�^�[�{���g�@���n�`�̂�
	{95,_("3�O���b�h�ȓ��̑_�����ꏊ�Ƀe���|�[�g������x�̔\��", "Capable of teleporting to a specified location within 3 grids")},
	{30,_("������x�̔\��", "Capable of producing light")}, //���C�g�G���A

	{60,_("�߂��̃c�^��؂̎}�𑀂���x�̔\��", "Capable at manipulating nearby vines and branches")},//�אڑ������
	{30,_("�A���Ɉ͂܂�Ă���Ƃ�����ƌ��C���o����x�̔\��", "Capable of being more energetic while surrounded by plants")},
	{45,_("���d������x�̔\��", "Capable of discharging electricity")}, //�Z�˒��d���r�[��
	{30,_("�Ö��p�����ӂȒ��x�̔\��", "Capable at hypnotism")},//�X���[�v�����X�^�[
	{50,_("���̌��𗁂т�ƌ��C���o����x�̔\��", "Capable of being energetic while in moonlight")},//���O��ԂŃp���[�A�b�v
//31
	{90,_("�d���Љ�Ŋ炪�L�����x�̔\��", "Capable of being popular in fairy society")}, //�d�����F�D�I
	{40,_("�j�������ӂȒ��x�̔\��", "Capable at swimming")}, //���㍂���ړ�
	{80,_("�߂��ɃJ�r�𐶂₷���x�̔\��", "Capable of making moss grow nearby")},//�F�D�Im�V���{������
	{60,_("�Z�����e���|�[�g��������x�̔\��", "Capable of teleporting short distances")}, //�V���[�g�e���|
	{35,_("�������������x�̔\��", "Capable of taming dogs")}, //C�V���{���ɂ̂ݗL���ȋ��͂ȃ`���[�������X�^�[

	{30,_("�ւ𑀂���x�̔\��", "Capable of manipulating snakes")}, //J�V���{���ɂ̂ݗL���ȋ��͂ȃ`���[�������X�^�[
	{65,_("��������������x�̔\��", "Capable of photosynthesis")},//���Ԃ̉��O�Ŗ����x��������B�M���u���X�ł�������B(��������v)
	{60,_("���͂̐�������m������x�̔\��", "Capable of detecting bodies of water")},
	{70,_("����Ō��C���o����x�̔\��", "Capable of being energetic while in water")},
	{80,_("�d�͍U���𒆘a������x�̔\��", "Capable of neutralizing gravity attacks")},
//41
	{95,_("��΂̌��Ԃ���蔲������x�̔\��", "Capable of slipping through gaps in stone")},
	{45,_("���e���o�����x�̔\��", "Capable of conjuring illusions")},//�^�[�������X�^�[
	{75,_("�؂̏�Ō��C���o����x�̔\��", "Capable of being energetic in forest")},
	{50,_("�e���|�[�g�E�o�b�N���炷�蔲������x�̔\��", "Capable of slipping through 'teleport to' spells")},
	{60,_("���ɋ������x�̔\��", "Capable at handling alcohol")},//�D���x�����ȏ�ɐi�݂ɂ����H

	{70,_("���o��ԂɂȂ�Ƒ啝�Ƀp���[�A�b�v������x�̔\��", "Capable of greatly powering up while hallucinating")},
	{20,_("���������e��������x�̔\��", "Capable of shooting weak danmaku")},//�_���[�W=plev/2 ����1
	{70,_("����Ȃ�ɒe��������x�̔\��", "Capable of shooting danmaku")}, //�_���[�W=plev ����2
	{80,_("���񂹂����ӂȒ��x�̔\��", "Capable of taming birds")},//B�V���{���Ɍ����₷���`���[�������X�^�[
	{100,_("�_�~�[", "Dummy")},// ���o�[�W�����̃L����(�N�C�b�N�X�^�[�g�܂�)��p_ptr->sc��50�̂͂��Ȃ̂Ŗ��K���̔���ɂ���
//51
	{85,_("�n�ʂɌ����J������x�̔\��","Capable of opening chasms in the ground")},//�����̒n�ʂ�[�����ɂ���B�A�C�e���Ȃǂ�����Ƃł��Ȃ��B�R�撆�ł��ł��Ȃ��B
	{60,_("�A���𐬒���������x�̔\��", "Capable of making plants grow")},//�Ԃ⑐�n�`���؂ɂȂ�
	{70,_("��������炷���x�̔\��", "Capable of deflecting beams of light")}, //���[�U�[�������Ȃ��B���僌�[�U�[��M���u���X�͌����B
	{90,_("覐΂��O���C�Y������x�̔\��", "Capable of grazing meteors")},//GF_METEOR�������Ȃ�
	{90,_("�ނ������N�������x�̔\��", "Capable of causing whirlwinds")},

//	{0,"���x�̔\��"},
//	{0,"���x�̔\��"},

//���ɔ�΂���Đ؂Ȃ����ƂɂȂ���x�̔\�́@�������󂯂�Ƒ傫���e���|�[�g

//�F�B�̏ꏊ��m��Ffriendly���m�H
//	{0,"�Ԃ̖����W�߂���x�̔\��"},//�Ԓn�`��Łu�L�����f�B�v�����HMP���񕜁H
	{100,_("�I�[�_�~�[", "terminator dummy")},
	{100,_("�I�[�_�~�[", "terminator dummy")},
};


/*:::�d����p���C�V�����p���[���*/
void fairy_racial_power(power_desc_type *power_desc, int *num_return)
{

	int num = *num_return;

	if(p_ptr->prace != RACE_FAIRY)
	{
		msg_format(_("ERROR:fairy_racial_power()���d���ȊO�̂Ƃ��Ă΂ꂽ",
                    "ERROR: fairy_racial_power() called for a non-fairy"));
		return;
	}

	switch(CHECK_FAIRY_TYPE)
	{
	case 1:
		strcpy(power_desc[num].name, _("�{��", "Lock Doors"));
		power_desc[num].level = 1;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 10;
		power_desc[num].info = _("����ꂽ�h�A����Œ肷��r�[������B�Œ肳�ꂽ�h�A�͊J���ł��Ȃ����@���󂷂��Ƃ͂ł���B",
                                "Fires a beam that locks closed doors. Locked door cannot be unlocked, but can be bashed down.");
		power_desc[num++].number = -1;
		break;

	case 3:
		strcpy(power_desc[num].name, _("�L�m�R�Ӓ�", "Identify Mushrooms"));
		power_desc[num].level = 1;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num].info = _("�L�m�R�̎�ނ𔻕ʂ���B", "Tells different mushrooms apart.");
		power_desc[num++].number = -2;
		break;

	case 6:
		strcpy(power_desc[num].name, _("�h�A�ω�", "Change to Door"));
		power_desc[num].level = 12;
		power_desc[num].cost = 8;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 25;
		power_desc[num].info = _("�ǂ��h�A�ɕω�������B", "Changes a wall into a door.");
		power_desc[num++].number = -3;
		break;

	case 10:
		strcpy(power_desc[num].name, _("�q���(�h���S����)", "Lullaby (dragon tongue)"));
		power_desc[num].level = 20;
		power_desc[num].cost = 12;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 30;
		power_desc[num].info = _("���͂̃h���S���𖰂点��B�ʏ햰�点���Ȃ�����ɂ����������j�[�N�����X�^�[�ɂ͌����Ȃ��B",
                                "Sleeps nearby dragons. Works against opponents that normally resist sleep, but doesn't work on unique monsters.");
		power_desc[num++].number = -4;
		break;


	case 13:
		strcpy(power_desc[num].name, _("�����P�A", "Good Sleep"));
		power_desc[num].level = 10;
		power_desc[num].cost = 7;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 20;
		power_desc[num].info = _("�אڂ��������Ă��郂���X�^�[��̂����[�����点��B",
                                "Makes one adjacent sleeping monster sleep for longer.");
		power_desc[num++].number = -6;
		break;

	case 14:
		strcpy(power_desc[num].name, "Go West!");
		power_desc[num].level = 3;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�}�b�v�������֌����Đ�����ԁB��Q���ɓ�����Ǝ~�܂�B",
                                "Charges to the left side of the map. You stop if you hit an obstacle.");
		power_desc[num++].number = -7;
		break;
	case 15:
		strcpy(power_desc[num].name, _("�אڈʒu����", "Adjacent Swap Positions"));
		power_desc[num].level = 3;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 20;
		power_desc[num].info = _("�אڂ����G�ƈʒu����������B", "Swaps positions with an adjacent enemy.");
		power_desc[num++].number = -8;
		break;

	case 23:
		strcpy(power_desc[num].name, _("���S�C", "Water Gun"));
		power_desc[num].level = 3;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�З�1d(���x��)�̐������{���g����B�߂��ɐ����Ȃ��Ǝg���Ȃ��B",
                                "Fires a bolt of water (damage 1d(level)). Cannot be used unless there's water nearby.");
		power_desc[num++].number = -9;
		break;

	case 24:
		strcpy(power_desc[num].name, _("�V���[�g�E�����̔�", "Short Dimension Door"));
		power_desc[num].level = 20;
		power_desc[num].cost = 9;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 30;
		power_desc[num].info = _("����3�ȓ��̑_�����Ƃ���Ƀe���|�[�g����B���s����ƑS���e���|�[�g���Ȃ��B",
                                "Teleports to specified location within 3 squares. Does not teleport at all if it fails.");
		power_desc[num++].number = -10;
		break;
	case 25:
		strcpy(power_desc[num].name, _("����", "Emit Light"));
		power_desc[num].level = 1;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CON;
		power_desc[num].fail = 5;
		power_desc[num].info = _("���邭�����Ď����̋��镔�����Ƃ炷�B",
                                "Illuminates nearby area.");
		power_desc[num++].number = -11;
		break;
	case 26:
		strcpy(power_desc[num].name, _("�A������", "Control Vegetation"));
		power_desc[num].level = 1;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 15;
		power_desc[num].info = _("�אڂ���G������������B����ԓG�A�͋����G�A���j�[�N�ɂ͌��ʂ������B�������؂̏�ɋ��Ȃ��Ǝg���Ȃ�",
                                "Slows an adjacent enemy. Less effective against flying, powerful or unique enemies. Cannot be used unless you are in woods.");
		power_desc[num++].number = -12;
		break;
	case 28:
		strcpy(power_desc[num].name, _("���d", "Discharge"));
		power_desc[num].level = 3;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_CON;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�З�1d(���x��*2)�̓d�������{���g����B",
                                "Fires a bolt of lightning. (damage 1d(level * 2))");
		power_desc[num++].number = -13;
		break;
	case 29:
		strcpy(power_desc[num].name, _("�Ö��p", "Hypnotism"));
		power_desc[num].level = 2;
		power_desc[num].cost = 4;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�G��̂𖰂点��B", "Tries to put an enemy to sleep.");
		power_desc[num++].number = -22;
		break;

	case 33:
		strcpy(power_desc[num].name, _("�J�r���B", "Grow Mold"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 15;
		power_desc[num].info = _("�F�D�I��m�V���{���̃����X�^�[����������B", "Summon friendly 'm' monsters.");
		power_desc[num++].number = -14;
		break;

	case 34:
		strcpy(power_desc[num].name, _("�u���ړ�", "Blink Movement"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num].info = _("����10���x�̃����_���Ȉʒu�Ƀe���|�[�g����B", "Teleports to a random position within 10 squares.");
		power_desc[num++].number = -15;
		break;

	case 35:
		strcpy(power_desc[num].name, _("����炵", "Tame Dogs"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("�אڂ���C�V���{���̃����X�^�[��̂𖣗����z���ɂ���B",
                                "Charms an adjacent 'C' monster into being your follower.");
		power_desc[num++].number = -16;
		break;
	case 36:
		strcpy(power_desc[num].name, _("�֎g��", "Snake Whisperer"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("�אڂ���J�V���{���̃����X�^�[��̂𖣗����z���ɂ���B",
                                "Charms an adjacent 'J' monster into being your follower.");
		power_desc[num++].number = -17;
		break;
	case 42:
		strcpy(power_desc[num].name, _("���e", "Illusion"));
		power_desc[num].level = 1;
		power_desc[num].cost = 3;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�G��̂����|������B", "Terrifies an enemy.");
		power_desc[num++].number = -18;
		break;
	case 47:
		strcpy(power_desc[num].name, _("���e��", "Weak Danmaku"));
		power_desc[num].level = 1;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num].info = _("�З�1d(���x��)�̖������{���g����B", "Fires a non-elemental bolt. (damage: 1d(level))");
		power_desc[num++].number = -19;
		break;
	case 48:
		strcpy(power_desc[num].name, _("�e��", "Danmaku"));
		power_desc[num].level = 5;
		power_desc[num].cost = 2;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 15;
		power_desc[num].info = _("�З�=���x���̖������{���g����B", "Fires a non-elemental bolt. (damage: level)");
		power_desc[num++].number = -20;
		break;
	case 49:
		strcpy(power_desc[num].name, _("����", "Tame Birds"));
		power_desc[num].level = 5;
		power_desc[num].cost = 5;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 20;
		power_desc[num].info = _("B�V���{���̃����X�^�[�𖣗����z���ɂ���B",
                                "Charms a 'B' monster into being your follower.");
		power_desc[num++].number = -21;
		break;
	case 51:
		strcpy(power_desc[num].name, _("�זv", "Create Chasm"));
		power_desc[num].level = 10;
		power_desc[num].cost = 7;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 25;
		power_desc[num].info = _("�����̋���ꏊ��[�����ɂ���B�A�C�e�������n�`��ł͎g���Ȃ��B",
                                "Creates a deep chasm on your position. Cannot be used on items or special terrain.");
		power_desc[num++].number = -23;
		break;
	case 52:
		strcpy(power_desc[num].name, _("�������i", "Rapid Growth"));
		power_desc[num].level = 16;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 35;
		power_desc[num].info = _("������n�`�����n���Ԃ̂Ƃ��n�`��؂ɕς���B",
                                "If current terrain has flowers or grass, turn it into a tree.");
		power_desc[num++].number = -24;
		break;
	case 55:
		strcpy(power_desc[num].name, _("�ނ���", "Whirlwind"));
		power_desc[num].level = 15;
		power_desc[num].cost = 16;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 25;
		power_desc[num].info = _("���͂̓G�Ƀ��x��/2�̃_���[�W��^��������΂��B����ȓG�ɂ͌��ʂ������B",
                                "Deals damage (level / 2) to nearby enemies and blows them away. Less effective against gigantic enemies.");
		power_desc[num++].number = -26;
		break;

	default:
		break;

	}

	*num_return = num;

}

/*:::�d���p���C�V�����p���[���s���@�s���������Ƃ�TRUE*/
bool fairy_racial_power_aux(s32b command)
{
	int dir;
	int plev = p_ptr->lev;
	if(p_ptr->prace != RACE_FAIRY)
	{
		msg_format(_("fairy_racial_power_aux()���d���ȊO����Ă΂ꂽ", "fairy_racial_power_aux() was called for someone other than a fairy"));
		 return FALSE;
	}

	switch(command)
	{
		case -1: //�{��
		{
			if (!get_aim_dir(&dir)) return FALSE;
			wizard_lock(dir);
		}
		break;
		case -2: //�L�m�R�Ӓ�
		{
			if (!ident_spell_2(1)) return FALSE;
		}
		break;
		case -3: //�h�A�ω�
		{
			int x,y;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 0, GF_WALL_TO_DOOR, PROJECT_GRID, -1);
		}
		break;
		case -4: //�q���
		{
			msg_print(_("���Ȃ��̓h���S���̋C�����ɂȂ��Ďq��S���̂����E�E",
                        "You think of yourself as a dragon and sing a lullaby..."));
			project_hack(GF_STASIS_DRAGON, plev);
		}
		break;

		case -6:  //���͂̓G�̐����l�㏸
		{
			int x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if(!MON_CSLEEP(m_ptr))
			{
				msg_format(_("���Ȃ��̓��Z�͖����Ă���҂ɂ������ʂ��Ȃ��B",
                            "Your special ability works only on those who are already asleep."));
			}
			else if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				char m_name[80];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				msg_format(_("%s�͐[���Q���𗧂Ďn�߂��E�E", "%s starts snoring loudly..."),m_name);
				if(m_ptr->mtimed[MTIMED_CSLEEP]<5000) m_ptr->mtimed[MTIMED_CSLEEP] += 200;
			}
			else
			{
				msg_format(_("�����ɂ͉������Ȃ��B", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -7: //���֐������
		{
			go_west();
		}
		break;
		case -8: //�אڈʒu����
		{
			int x,y;
			monster_type *m_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				teleport_swap(dir);
			}
			else
			{
				msg_format(_("�����ɂ͉������Ȃ��B", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -9: //���S�C
		{
			if(!cave_have_flag_bold(py,px,FF_WATER))
			{
				msg_print(_("�����ɂ͐����Ȃ��B", "There's no water here."));
				return FALSE;
			}

			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���S�C��������B", "You fire a water gun."));
			fire_bolt(GF_WATER, dir, randint1(plev));
		}
		break;
		case -10: //�~�j�����̔�
		{
			msg_print(_("����3�ȓ��̖ړI�n��I��ŉ������B", "Select destination within 3 squares."));
			if (!dimension_door(D_DOOR_MINI)) return FALSE;
			break;
		}
		break;
		case -11: //����
		{
			msg_print(_("���Ȃ���ῂ����������B", "You shine brightly."));
			lite_area(0, 0);
		}
		break;
		case -12: //�������
		{
			if(!cave_have_flag_bold(py,px,FF_TREE))
			{
				msg_print(_("�����ɂ͖؂��Ȃ��B", "There's no trees here."));
				return FALSE;
			}
				msg_print(_("���Ȃ��͒ӂ�؂̎}�𑀂����I", "You manipulate vines and branches!"));
				project(0,1,py,px,plev * 2+20,GF_SLOW_TWINE, PROJECT_KILL | PROJECT_HIDE,-1);
		}
		break;
		case -13: //���d
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("�d����������I", "You discharge electricity!"));
			fire_bolt(GF_ELEC, dir, randint1(plev*2));
		}
		break;
		case -14: //�J�r���B
		{
			int i;
			for (i = randint1(3); i > 0; i--)
			{
				summon_specific(-1, py, px, plev, SUMMON_BIZARRE1, PM_FORCE_FRIENDLY);
			}
		}
		break;

		case -15: //�V���[�g�e���|
		{
			teleport_player(10L, 0);
		}
		break;

		case -16: //����炵
		case -17: //�֎g��
		case -21: //����
		{
			int x,y;
			int power = plev + 25;
			monster_type *m_ptr;
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir==5) return FALSE;

			y = py + ddy[dir];
			x = px + ddx[dir];
			m_ptr = &m_list[cave[y][x].m_idx];

			if (cave[y][x].m_idx && (m_ptr->ml) )
			{
				if(command == -16) project(0,0,y,x,power,GF_CHARM_C,PROJECT_HIDE | PROJECT_KILL,-1);
				else if(command == -17) project(0,0,y,x,power,GF_CHARM_J,PROJECT_HIDE | PROJECT_KILL,-1);
				else  project(0,0,y,x,power,GF_CHARM_B,PROJECT_HIDE | PROJECT_KILL,-1);
			}
			else
			{
				msg_format(_("�����ɂ͉������Ȃ��B", "There's nobody here."));
				return FALSE;
			}
		}
		break;
		case -18: //���e
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("���낵���Ȍ��e�����o�����E�E", "You create a frightening illusion..."));
			fear_monster(dir, plev+20);
		}
		break;
		case -19: //�e��
		case -20:
		{
			int dam;
			if(command == -19) dam = randint1(plev);
			else dam = plev;
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("�e����������I", "You shoot danmaku!"));
			fire_bolt(GF_MISSILE, dir, dam);
		}
		break;
		case -22: //�Ö��p
		{
			if (!get_aim_dir(&dir)) return FALSE;
			msg_print(_("�w�����邮��񂵂��E�E", "You spin your finger in circles..."));
			sleep_monster(dir, plev+10);
		}
		break;
		case -23: //�זv
		{
			if(!cave_clean_bold(py, px))
			{
				msg_format(_("�����Ɍ����J���邱�Ƃ͂ł��Ȃ��B", "You can't open a chasm here."));
				return FALSE;
			}
			msg_format(_("�����ɑ匊���J�����I", "A large chasm opens up beneath you!"));
			cave_set_feat(py, px, feat_dark_pit);
			p_ptr->redraw |= (PR_MAP);

		}
		break;
		case -24: //�������i
		{
			if(!cave_have_flag_bold(py, px, FF_PLANT))
			{
				msg_format(_("�����ɂ͐A�����Ȃ��B", "There's no plants here."));
				return FALSE;
			}
			else if(cave_have_flag_bold(py, px, FF_TREE))
			{
				msg_format(_("���łɖ؂̏ゾ�B", "There's already a tree here."));
				return FALSE;
			}
			msg_format(_("�����̐A�����݂�݂邤���Ɉ���Ă����I","The plants beneath you start growing rapidly!"));
			cave_set_feat(py, px, feat_tree);
			p_ptr->redraw |= (PR_MAP);


		}
		break;
		case -25: //���N
		{


		}
		break;
		case -26: //�ނ���
		{
			msg_print(_("�������N�������I", "You summon a whirlwind!"));
			project(0, 1, py, px, plev, GF_TORNADO, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		}
		break;

		default:
		msg_format(_("fairy_racial_power_aux()�ɕs���ȃR�}���h�l(%d)���n���ꂽ",
                    "Incorrent command value (%d) passed to fairy_racial_power_aux()"),command);
		return FALSE;
	}

	return TRUE;
}

///mod150315 �d���p�����𓾂�
/*:::memo
 *�d�����Z��fairy_table[]�̂ǂꂩ���I�΂�A�e�[�u���̃C���f�b�N�X�l��p_ptr->race_multipur_val[0]�Ɋi�[�����B
 *�܂��A�N�C�b�N�X�^�[�g���ɂ��̔\�͂��ێ������悤��p_ptr->sc��previous_char.sc�ɂ������l��ۑ����Ă����B
 *�N�C�b�N�X�^�[�g��ɂ�������l��ǂ�ł���p_ptr->race_multipur_val[0]�Ɋi�[�������B
 */
void get_fairy_type(bool birth)
{
	int new_type;

	//�񃆃j�[�N�N���X�d���̂ݐݒ肷��
	if(p_ptr->prace != RACE_FAIRY) return;
	if(cp_ptr->flag_only_unique) return;

	//���A���e�B�ɉ����ĕ\����I��
	do
	{
		new_type = randint1(FAIRY_TYPE_MAX);
	}while(fairy_table[new_type].rarity >= randint1(100));
		//v1.1.31c��̎��� >��>=�ɂ����B1%�̊m���Ń_�~�[������𔲂��Ă��܂�

	p_ptr->race_multipur_val[0] = new_type;


	//�d���ŃL�������C�N���A�������͗d���Ńv���C���Ƀo�[�W�����A�b�v�����ꍇ
	//if(birth && previous_char.prace == RACE_FAIRY)
	//v1.1.31c�C�� ���܂���łȂ��푰�ϗe�ŗd���ɂȂ����Ƃ��ɗd�����Zidx���ۑ����ꂽsc(�Љ�I�n��)�l���������Ȃ��悤�ɔ���t���Ă����s��̂��ƂɂȂ����̂ŏ������B���������Ƃ��ɂ͏������ɂ�肱���ɗ��Ȃ��̂ő��v�Ȃ͂��H
	if(birth)
	{
		p_ptr->sc = new_type;
		previous_char.sc = new_type; //�N�C�b�N�X�^�[�g���ɓ����\�͂ɂȂ�悤�ۑ�
	}

}

//v1.1.32 EX���[�h��p�@��ǐ_�l�Ƀ����_���ȓ����l��ݒ肷��B
bool straygod_job_adviser(void)
{
	int rank;
	int new_idx;
	int dummy_idx=-1;
	int i;
	int cnt;
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;

	if (p_ptr->prace != RACE_STRAYGOD)
	{
		msg_print(_("���Ȃ��͖�ǐ_�l�ł͂Ȃ��̂Ŋ֌W�Ȃ��悤���B",
                    "You're not a stray god, so this has nothing to do with you."));
		return FALSE;
	}
	else if (p_ptr->max_plv > 44)
	{
		msg_print(_("���Ȃ��݂̍���͂����h�炮���Ƃ͂Ȃ��B",
                    "You're not going to change the way you are anymore."));
		return FALSE;
	}
	else if (ex_buildings_param[ex_bldg_num] == 255)
	{
		msg_print(_("���Ђ͂����₢�����ɓ����Ȃ��B", "You call out to the branch shrine, but nobody answers."));
		return FALSE;
	}

	if (p_ptr->max_plv > 29) rank = 3;
	else rank = 2;

	clear_bldg(4, 18);

	prt(_("����̐_�����Ȃ��̐g�̐U����ɂ��ď������Ă����炵���B",
        "Goddess Yasaka is willing to give you advice on how to develop further."), 5, 20);

	if (!get_check_strict(_("��������܂����H", "Do you listen? "), CHECK_OKAY_CANCEL))
	{
		clear_bldg(4, 18);
		return FALSE;
	}

	clear_bldg(4, 18);
	prt(_("���Ђ̔��̒����琺�������Ă���...",
            "A voice echoes from beyond the door of branch shrine..."), 6, 20);
	inkey();

	cnt = 0;

	for (i = 0; deity_table[i].rank != 0; i++)
	{
		if (rank != deity_table[i].rank) continue;
		cnt++;
		if (one_in_(cnt)) new_idx = i;
	}
	cnt = 0;
	for (i = 0; deity_table[i].rank != 0; i++)
	{
		if (rank != deity_table[i].rank) continue;
		if (new_idx == dummy_idx) continue;
		cnt++;
		if (one_in_(cnt)) dummy_idx = i;
	}

	switch (randint1(10))
	{
	case 1: case 2: case 3: case 4: case 5:
    #ifdef JP
		prt(format("�u���Ȃ���%s��ڎw���̂ł��B", deity_table[new_idx].deity_name), 7, 22);
		prt(format("������ΐl�X�̐M�Ă��A���i���ɉh�𓾂��邱�Ƃł��傤�B�v"), 8, 22);
    #else
        prt(format("'You should aim to become a %s.", deity_table[new_idx].deity_name), 7, 22);
		prt(format("If you do so, your amount of worshippers will increase, "), 8, 22);
		prt(format("and you shall be prosperous for aeons to come.'"), 9, 22);
    #endif
		break;
	case 6: case 7:
    #ifdef JP
		prt(format("�u�ӂނӂށA������Ȃ����%s�͂ǂ�������H", deity_table[new_idx].deity_name), 7, 22);
		prt(format("���̕ӂ���Ď��͈ĊO�u���[�I�[�V�������Ǝv���̂�ˁB�v"), 8, 22);
    #else
        prt(format("'Hmm, hmm, how does being a %s sound?", deity_table[new_idx].deity_name), 7, 22);
		prt(format("I think that's a surprisingly promising position.'"), 8, 22);
    #endif
		break;
	case 8: case 9:
    #ifdef JP
		prt(format("�u���[��c�c"), 7, 22);
		prt(format("������%s�ɂȂ����ق����M���W�܂�񂶂�Ȃ��H�v", deity_table[new_idx].deity_name), 8, 22);
    #else
        prt(format("'Umm......"), 7, 22);
		prt(format("Wouldn't you be able to gather more faith if you become a %s?'", deity_table[new_idx].deity_name), 8, 22);
    #endif
		break;
	default:
    #ifdef JP
		prt(format("�u���Ȃ���%s��ڎw���̂ł��B������΁E�E", deity_table[new_idx].deity_name), 7, 22);
		prt(format("�u����ꕪ�����ĂȂ��˂��B���%s�̂ق����������āI�v", deity_table[dummy_idx].deity_name), 8, 22);
		prt(format("�u���ł����āH���̎d���ɉ���������o���񂶂�Ȃ����B�v"), 9, 22);
		prt(format("�u��[���A�v�X�ɂ�邩���H�v"), 10, 22);
		prt("", 11, 22);
		if (one_in_(2)) new_idx = dummy_idx;
		prt(format("�e�������̖��A%s��ڎw���̂��悢���낤�Ƃ������ƂɂȂ����B", deity_table[new_idx].deity_name), 12, 22);
    #else
        prt(format("'You should aim to become a %s. If you do so...'", deity_table[new_idx].deity_name), 7, 22);
		prt(format("'You just don't get it, do you? Being a %s is way better!'", deity_table[dummy_idx].deity_name), 8, 22);
		prt(format("'What? Are you telling me how to do my job?'"), 9, 22);
		prt(format("'Well.. We haven't fought for a long time, did we?'"), 10, 22);
		prt("", 11, 22);
		if (one_in_(2)) new_idx = dummy_idx;
		prt(format("As the result of the danmaku battle, you were told to become a %s.", deity_table[new_idx].deity_name), 12, 22);
    #endif
		break;

	}

	//�_�i�l���Z�b�g
	p_ptr->race_multipur_val[DBIAS_COSMOS] = deity_table[new_idx].dbias_cosmos;
	p_ptr->race_multipur_val[DBIAS_WARLIKE] = deity_table[new_idx].dbias_warlike;
	p_ptr->race_multipur_val[DBIAS_REPUTATION] = deity_table[new_idx].dbias_reputation;

	inkey();
	clear_bldg(4, 18);

	ex_buildings_param[ex_bldg_num] = 255;
	return TRUE;

}

//v1.1.70 �푰�u������v�̏������͂����肷��
//�L�������C�N���ƃN�C�b�N�X�^�[�g���ɌĂ΂��
//�N�C�b�N�X�^�[�g���ɃL�������C�N���Ɠ����l�ɂ��邽�߁A�d���̓��Z���l��p_ptr->sc(�Љ�I�n��)���g���Ă��̂�
void get_animal_ghost_family(bool birth)
{

	int tmp;

	if (p_ptr->prace != RACE_ANIMAL_GHOST) return;

	//�L�������C�N���̓����_���ɐݒ肵����l��p_ptr->sc�ɕۑ�
	if (birth)
	{
		//���j�[�N�N���X�͌Œ�
		if (p_ptr->pclass == CLASS_SAKI)
			tmp = ANIMAL_GHOST_STRIFE_KEIGA;
		else 	if (p_ptr->pclass == CLASS_YACHIE)
			tmp = ANIMAL_GHOST_STRIFE_KIKETSU;
		else
		{
			switch (randint1(10))
			{
			case 1: case 2: case 3:
				tmp = ANIMAL_GHOST_STRIFE_KEIGA;
				break;
			case 4: case 5: case 6:
				tmp = ANIMAL_GHOST_STRIFE_KIKETSU;
				break;
			case 7: case 8: case 9:
				tmp = ANIMAL_GHOST_STRIFE_GOUYOKU;
				break;

			default:
				tmp = ANIMAL_GHOST_STRIFE_NONE;
				break;
			}
		}
		p_ptr->sc = tmp;
		previous_char.sc = tmp;
	}
	//�N�C�b�N�X�^�[�g����p_ptr->sc�ɕۑ�����Ă���l�����[���o�b�N
	else
	{
		tmp = p_ptr->sc;
	}

	//�푰�p�ϐ��ɋL�^
	p_ptr->race_multipur_val[0] = tmp;

}



