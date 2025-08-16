


#include "angband.h"


//�V���Z��Ń����_���`�[���Ґ��̂Ƃ��K���Ɍ��܂�`�[����
const cptr battle_mon_random_team_name_list[BATTLE_MON_RANDOM_TEAM_NAME_NUM+1] =
{
	_("(�_�~�[)", "(dummy)"),
	_("�s���̂��h�t", "<Mushroom School>"),
	_("�s�����̂��h�t", "<Bamboo Shoot School>"),
	_("�s�`�[���E�����Ґ��t", "<Team Improvisation>"),
	_("�s������̉�t", "<Unofficial Match>"),
	_("�s�ǂ����Ă����Ȃ����H�t", "<How Did It Come To This?>"),
	_("�s�V�����J�t", "<New Bonds>"),
	_("�s�œ瓯�D��t", "<Yaminabe Club>"),
	_("�s�閧�̎���ԁt", "<Secret Hobby Friends>"),
	_("�s�����Ȃ钊�I�̌��ʁt", "<Results of Strict Lottery>"),
	_("�s�ϋq����K���Ɉ��������Ă����l�B�t", "<Random Audience Members>"),

	_("�s�������ő��ȂɂȂ������t", "<At the Soba Restaurant>"),
	_("�s���͎��ٕ̈ς̃{�X�t", "<The Boss of Next Incident>"),
	_("�s����ňӋC�����t", "<Banquet Rapport>"),
	_("�s���Ίy�t�@���N���u�t", "<Manzairaku Fan Club>"),
	_("�s���̃J�t�F�e���A�̏�A�t", "<Village Cafeteria Regulars>"),
	_("�s�V�i�W�[�̔������ۑ�t", "<Demonstrating Synergy>"),
	_("�s���߂ẴI�t��t", "<First Off-Party>"),
	_("�s�\�y������t", "<Noh Music Study Group>"),
	_("�s�}���l���Ŕj�̎��݁t", "<Break the Rut>"),
	_("�s�ِF�R���Z�v�g�t", "<Unusual Concepts>"),

	_("�s�C�[�W�[�J���E�C�[�W�[�S�[�t", "<Easy Come, Easy Go>"),
	_("�s�h���[���`�[���t", "<Dream Team>"),
	_("�s�T�C�P�Ńq�b�v�Ńo�b�h�ŃS�[�t", "<Psychedelic, Hip, Bad, Go>"),
	_("�s�e�̃p�C�v���t", "<Shadow Pipe Role>"),
	_("�s�S�Ă��d�g�񂾍����t", "<Mastermind Behind Everythins>"),
	_("�s���z�����Ԉ��t", "<Gensoukyou Style>"),
	_("�s���̎O�d���̎l�l�ڈȍ~�t", "<Beyond the Fourth Youkai of Light>"),
	_("�s�d���̎R�I���G���e�[�����O�`�[���t", "<Youkai Mountain Orienteering>"),
	_("�s�t�@�[�X�g�A���o���ߓ������[�X�I�t", "<First Album Coming Soon!>"),
	_("�s�����X�y���J�[�h�̌������t", "<Spell Card Combination Studies>"),

	_("�s��̒������t", "<Mystery Investigation Team>"),
	_("�s�ς̊E�G��(�����Ⴛ����)�b��t", "<Fox Neighborhood>"),
	_("�s�l�d�S����t", "<Hundred Tales of Humans and Youkai>"),
	_("�s�䕗�̉e���Ńe���V�����������t", "<Typhoons Bring High Tension>"),
	_("�s���̉���̗����S���t", "<Cooking at the Next Banquet>"),
	_("�s�������p�[�e�B�[�Q�[����A�t", "<Kourindou Party Game Regulars>"),
	_("�s�l�O�̃t�H�[�N���A�t", "<Non-Human Folklore>"),
	_("�s���̐��E�Ń^�b�O��g�񂾒��t", "<Dream World Tag Teams>"),
	_("�s�[���e���R���e�X�g�t", "<Awesome Danmaku Contest>"),
	_("�s���퉮�v���[���c�t", "<Hakurei Presents>"),

	_("�s��G�����̊�@�t", "<Immediate Crisis>"),
	_("�s�Ђ܂��s��ٕ�(��)�΍��{���t", "<Sunflower Incident Mitigation Team>"),
	_("�s�_�Ђ�������������Ď��ɗ����t", "<Shrine's Noisy, So We Came to Watch>"),

};

//�V���Z��̌Œ�`�[���Ґ��@�ŏ��̐����̓`�[�����I�肳��邽�߂̍Œᐶ�����x��
const	battle_mon_special_team_type	battle_mon_special_team_list[BATTLE_MON_SPECIAL_TEAM_NUM+1] =
{
	{255,_("(�_�~�[)", "(dummy)"),
		{0, 0, 0, 0, 0, 0, 0, 0}},
	{60,_("�w�g���فx", "Scarlet Devil Mansion"),
		{MON_REMY, MON_FLAN, MON_SAKUYA, MON_PATCHOULI, MON_MEIRIN, MON_KOAKUMA, MON_CHUPACABRA,0}},
	{50,_("�w�A���m�E���x", "TheUnknown"),
		{MON_NUE, MON_RANDOM_UNIQUE_1, MON_RANDOM_UNIQUE_2, 0, 0, 0, 0, 0}},
	{75,_("�w�Ō�̃g���j�e�B�����[�W�����x", "Last Trinity Religion"),
		{MON_REIMU, MON_BYAKUREN, MON_MIKO, 0, 0, 0, 0, 0}},
	{60,_("�w���C�K�X�i�C�g�x", "Magus Night"),
		{MON_MARISA, MON_PATCHOULI, MON_ALICE, MON_BYAKUREN, MON_NARUMI, 0, 0, 0}},
	{35,_("�w�e�p�����x", "Rabbit Alliance"),
		{MON_UDONGE, MON_TEWI, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2}},

	{60,_("�w�t�r�_�A���x", "Tsukumogami Federation"),
		{MON_RAIKO, MON_BENBEN, MON_YATSUHASHI, MON_KOGASA, MON_KOKORO, 0, 0, 0}},
	{55,_("�w�o�A���x", "Big Sisters Union"),
		{MON_REMY, MON_LUNASA, MON_SHIZUHA, MON_SATORI, MON_BENBEN, MON_SHION_1, 0, 0}},
	{60,_("�w���A���x", "Little Sisters Union"),
		{MON_FLAN, MON_LYRICA, MON_MINORIKO, MON_KOISHI, MON_YATSUHASHI, MON_JYOON, 0, 0}},
	{20,_("�w�H��y�c�x", "Phantom Ensemble"),
		{MON_LUNASA, MON_MERLIN, MON_LYRICA, 0, 0, 0, 0, 0}},
	{0,_("�w���b��y�x", "Choujuu Gigaku"),
		{MON_MYSTIA, MON_KYOUKO, 0, 0, 0, 0, 0, 0}},

	{50,_("�w���ʘO�x", "Hakugyoukurou"),
		{MON_YUYUKO, MON_YOUMU, MON_BOU_REI, MON_G_BOU_REI, MON_LOST_SOUL, MON_LOST_SOUL, 0, 0}},
	{70,_("�w���_�Ёx", "Moriya Shrine"),
		{MON_KANAKO, MON_SUWAKO, MON_SANAE, 0, 0, 0, 0, 0}},
	{65,_("�w�i�����x", "Eientei"),
		{MON_KAGUYA, MON_EIRIN, MON_UDONGE, MON_TEWI, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, MON_YOUKAI_RABBIT2, 0}},
	{30,_("�w�_�u���X�|�C���[�x", "Double Spoiler"),
		{MON_AYA, MON_HATATE, 0, 0, 0, 0, 0, 0}},
	{75,_("�w����Ȓ����x", "Ministry of Right and Wrong"),
		{MON_EIKI, MON_KIZINTYOU, MON_KOMACHI, MON_DEATH_3, MON_DEATH_2, MON_DEATH_1, 0, 0}},

	{60,_("�w�n��a�x", "Palace of Earth Spirits"),
		{MON_SATORI, MON_KOISHI, MON_UTSUHO, MON_ORIN, 0, 0, 0, 0}},
	{80,_("�w���@���x", "Myouren Temple"),
		{MON_SHOU, MON_BYAKUREN, MON_ICHIRIN, MON_MURASA, MON_KYOUKO, MON_NAZRIN, MON_NUE, MON_MAMIZOU}},
	{75,_("�w�_��_�x", "Divine Spirit Mausoleum"),
		{MON_MIKO, MON_FUTO, MON_TOZIKO, MON_SEIGA, MON_YOSHIKA, MON_D_SHINREI, MON_SENNIN2, MON_SENNIN1}},
	{15,_("�w�͓��J���g���x", "Kappa Labor Union"),
		{MON_NITORI, MON_KAPPA_ENGINEER, MON_KAPPA_ENGINEER, MON_KAPPA_OKURI, MON_KAPPA_OKURI, MON_KAPPA, MON_KAPPA, MON_KAPPA}},
	{0,_("�w�T�o�C�o���Q�[���x", "Survival Game"),
		{MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, MON_YAMAWARO, 0, 0}},

	{10,_("�w�h�[���Y�E�H�[�x", "Dolls War"),
		{MON_ALICE, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL, MON_ALICE_DOLL}},
	{75,_("�w�v���W�F�N�g�X�~���V�x", "Project Sumiyoshi"),
		{MON_REIMU, MON_MARISA, MON_REMY, MON_SAKUYA, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, 0}},
	{50,_("�w�ؐ�Ƃ䂩���Ȓ��Ԃ����x", "Kasen and her Pleasant Friends"),
		{MON_KASEN, MON_INFANTDRAGON, MON_TIGER, MON_G_EAGLE, MON_RAIZYU, MON_TAMA, 0, 0}},
	{60,_("�w���z�̌��E�`�[���x", "Illusionary Boundary Team"),
		{MON_REIMU, MON_YUKARI, 0, 0, 0, 0, 0, 0}},
	{30,_("�w�֎�̉r���`�[���x", "Aria of Forbidden Magic Team"),
		{MON_MARISA, MON_ALICE, 0, 0, 0, 0, 0, 0}},

	{50,_("�w�����̗d���`�[���x", "Visionary Scarlet Devil Team"),
		{MON_REMY, MON_SAKUYA, 0, 0, 0, 0, 0, 0}},
	{40,_("�w�H���̏Z�l�`�[���x", "Netherworld Dwellers Team"),
		{MON_YUYUKO, MON_YOUMU, 0, 0, 0, 0, 0, 0}},
	{60,_("�w�d���̎R�x", "Youkai Mountain"),
		{MON_AYA, MON_HATATE, MON_MOMIZI, MON_G_TENGU, MON_HANA_TENGU, MON_KARASU_TENGU, MON_HAKUROU_TENGU, 0}},
	{80,_("�w�d�����Ƃ��b�x", "Youkai Tales"),
		{MON_HAKUSEN, MON_YUUTEN, MON_ENENRA, MON_TANUKI, MON_G_ONRYOU, MON_TSUKUMO_1, MON_ZYARYUU, MON_OROCHI}},
	{60,_("�w�S�S��s�̉���x", "Pandemonium Banquet"),
		{MON_MAMIZOU, MON_O_TANUKI, MON_TANUKI, MON_TANUKI, MON_TSUKUMO_1, MON_TSUKUMO_1, MON_TSUKUMO_1, MON_TSUKUMO_1}},

	{80,_("�w�Ƃ��Ă����̖��@�x", "Magic I've Prepared"),
		{MON_PATCHOULI, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, MON_PHILOSOPHER_STONE, 0, 0}},
	{75,_("�w�V�E�x", "Heavenly Realm"),
		{MON_TENSHI, MON_IKU, MON_SUIKA, MON_TENNIN, MON_TENNIN, 0, 0, 0}},
	{10,_("�w�����̉Ƃ̗����x", "Servants of Devil's House"),
		{MON_KOAKUMA, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_HOHGOBLIN, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID}},
	{60,_("�w�a��΂킩��x", "Cut You Down"),
		{MON_SAKUYA, MON_YOUMU, MON_MOMIZI, MON_SHINMYOUMARU, MON_TENSHI, MON_NEMUNO, MON_MAYUMI, 0}},
	{10,_("�w�L�̐��ޗ��x", "Village of Cats"),
		{MON_CHEN, MON_NEKOMATA, MON_YOUKAI_CAT, MON_YOUKAI_CAT, MON_YAMANEKO, MON_YAMANEKO, MON_YAMANEKO, MON_YASE_CAT}},

	{20,_("�w峂̒m�点�T�[�r�X�x", "Insect News Service"),
		{MON_WRIGGLE, MON_TARANTULA, MON_KILLER_STAG, MON_SWARM, MON_G_COCKROACH, MON_TUTUGAMUSHI, MON_G_W_CENTIPEDE, MON_KILLER_BEE}},
	{60,_("�w��̓ۂݒ��ԁx", "Drinking Buddies"),
		{MON_SUIKA, MON_YUGI, MON_KASEN, 0, 0, 0, 0, 0}},
	{20,_("�w�C�[�O�������B�b�c�x", "Eagle Rabbits"),
		{MON_RINGO, MON_SEIRAN, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, MON_EAGLE_RABBIT, 0, 0}},
	{50,_("�w�������̉�x", "My Company"),
		{MON_KOMACHI, MON_ORIN, MON_CIRNO, MON_CLOWNPIECE, 0, 0, 0, 0}},
	{70,_("�w�E�F���J���w���x", "Welcome Hell"),
		{MON_HECATIA1, MON_HECATIA2, MON_HECATIA3, MON_JUNKO, MON_CLOWNPIECE, 0, 0, 0}},

	{40,_("�w�ΉԌ��x", "Ishizakura Viewing"),
		{MON_YUGI, MON_KISUME, MON_YAMAME, MON_PARSEE, MON_ORIN, MON_G_ONRYOU, MON_G_ONRYOU, 0}},
	{20,_("�w�n��a�̕�����ʏZ�l�x", "Unspeaking Residents of Chireiden"),
		{MON_PANTHER, MON_BLACK_PANTHER, MON_KOMODO_DRAGON, MON_HASHIBIROKOU, MON_HELL_KARASU, MON_G_ONRYOU, MON_ONRYOU, 0}},
	{70,_("�w�q�ꖳ�����̖��x", "Pure Lunarians"),
		{MON_SAGUME, MON_YORIHIME, MON_TOYOHIME, 0, 0, 0, 0, 0}},
	{20,_("�w�ǂ����Ă��K�������Ă���x", "That's a Tanuki"),
		{MON_TANUKI, MON_TANUKI, MON_TANUKI, MON_TANUKI, 0, 0, 0, 0}},
	{50,_("�w�z���Y�����o�[�x", "Horismrivers"),
		{MON_RAIKO, MON_LUNASA, MON_MERLIN, MON_LYRICA, 0, 0, 0, 0}},

	{40,_("�w�A�b�g�z�[���ȐE��ł��x", "Homely Workplace"),
		{MON_SAKUYA, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID, MON_FAIRY_MAID}},
	{60,_("�w��_��J���x", "Open Book of the Hidden God"),
		{MON_OKINA, MON_SATONO, MON_MAI, 0, 0, 0, 0, 0}},
	{45,_("�w�F����俎q�͎O�l����I�x", "There's Three Sumireko Usamis!"),
		{ MON_SUMIREKO_2, MON_SUMIREKO_2, MON_SUMIREKO_2, 0, 0, 0, 0, 0 } },
	{ 60,_("�w�Â��m�荇�����m�x", "Old Acquintances"),
		{ MON_KASEN, MON_YUKARI, MON_OKINA, 0, 0, 0, 0, 0 } },

	{ 30,_("�w�͂Ȃ����悤��������x", "Hanasaka Yousei-san"),
		{ MON_CLOWNPIECE, MON_3FAIRIES, MON_MARISA, 0, 0, 0, 0, 0 } },
	{ 45,_("�w���s���c�x", "Inexhaustible Corps"),
		{ MON_MAYUMI, MON_HANIWA_F2, MON_HANIWA_F2, MON_HANIWA_A2, MON_HANIWA_A2, MON_HANIWA_C2, MON_HANIWA_C2, 0 } },
	{ 70,_("�w�����s��x", "Lunar Rainbow Marketplace"),
		{ MON_CHIMATA, MON_MEGUMU, MON_MOMOYO, MON_TSUKASA, 0, 0, 0, 0 } },


};

/*:::battle_mon�p�@�w��G���ʂ̓G�ɖ��@�U�������Ƃ��̊��҃_���[�W*/
int	calc_battle_mon_spell_dam(int r_idx, int enemy_r_idx, int *spell_score_mult, int mode)
{
	int dam;
	int total_damage = 0;
	int spell_cnt = 0;
	int rlev;
	int rhp;
	bool powerful;
	monster_race *r_ptr =  &r_info[r_idx];
	monster_race *enemy_r_ptr =  &r_info[enemy_r_idx];
	int score_mult_plus = 0;

	rlev = r_ptr->level;
	if(rlev < 1) rlev = 1;
	//HP(�u���X�З͌v�Z�p)��8�����炢�Ōv�Z
	rhp = r_ptr->hdice * r_ptr->hside / ((r_ptr->flags1 & RF1_FORCE_MAXHP) ? 1:2) * 4 / 5;
	if(rhp < 1) rhp = 1;
	powerful = (r_ptr->flags2 & RF2_POWERFUL);





	if(r_ptr->flags4 & RF4_DANMAKU)
	{

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else dam = rlev;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_DISPEL)
	{
		if(enemy_r_ptr->flags6 & (RF6_HASTE)) score_mult_plus += 30;
		if(enemy_r_ptr->flags6 & (RF6_INVULNER)) score_mult_plus += 50;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_ROCKET)
	{
		dam = rhp / 4;
		if(dam > 800) dam = 800;
		if(enemy_r_ptr->flagsr & RFR_RES_SHAR) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_SHOOT) //����ˌ��͌v�Z���Ȃ�
	{
		dam = r_ptr->blow[0].d_dice * (r_ptr->blow[0].d_side+1) / 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_HOLY)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_HELL)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_HURT_HELL) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_AQUA)
	{
		dam = rhp / 8;
		if(dam > 350) dam = 350;
		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 10;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_ACID)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_ELEC)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_FIRE)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_COLD)
	{
		dam = rhp / 3;
		if(dam > 1600) dam = 1600;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_POIS)
	{
		dam = rhp / 3;
		if(dam > 800) dam = 800;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NETH)
	{
		dam = rhp / 6;
		if(dam > 550) dam = 550;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_LITE)
	{
		dam = rhp / 6;
		if(dam > 400) dam = 400;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DARK)
	{
		dam = rhp / 6;
		if(dam > 400) dam = 400;
		if (enemy_r_ptr->flagsr & RFR_RES_DARK) dam /= 5;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_WAVEMOTION)
	{
		dam = rhp / 6;
		if(dam > 555) dam = 555;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags4 & RF4_BR_SOUN)
	{
		dam = rhp / 6;
		if(dam > 450) dam = 450;
		if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 10;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_CHAO)
	{
		dam = rhp / 6;
		if(dam > 600) dam = 600;
		if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF))
			score_mult_plus += 50;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DISE)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NEXU)
	{
		dam = rhp / 7;
		if(dam > 600) dam = 600;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_TIME)
	{
		dam = rhp / 3;
		if(dam > 150) dam = 150;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_INER)
	{
		dam = rhp / 6;
		if(dam > 200) dam = 200;
		if (enemy_r_ptr->flagsr & RFR_RES_INER) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_GRAV)
	{
		dam = rhp / 3;
		if(dam > 200) dam = 200;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME || enemy_r_ptr->flags7 & RF7_CAN_FLY) dam /= 3;
		else if (enemy_r_ptr->flags2 & RF2_GIGANTIC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_SHAR)
	{
		dam = rhp / 6;
		if(dam > 500) dam = 500;
		if (enemy_r_ptr->flagsr & RFR_RES_SHAR) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_PLAS)
	{
		dam = rhp / 4;
		if(dam > 1200) dam = 1200;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BA_FORCE)
	{
		if(powerful)
			dam = ((rhp / 4) > 350 ? 350 : (rhp / 4));
		else
			dam = ((rhp / 6) > 250 ? 250 : (rhp / 6));

		if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 20;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_MANA)
	{
		dam = rhp / 3;
		if(dam > 250) dam = 250;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags4 & RF4_SPECIAL2) //�t�������Z�ȊO�͔����}�~����\��Ȃ̂ŃJ�E���g���Ȃ�
	{
		if(r_idx == MON_FLAN)
			dam = 1600; //�t�������Z
		else
			dam = 0;

		total_damage += dam;
		if(dam) spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_NUKE)
	{
		dam = rhp / 4;
		if(dam > 1000) dam = 1000;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BA_CHAO)
	{
		dam = rlev*2 + 50;

		if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF))
			score_mult_plus += 70;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags4 & RF4_BR_DISI)
	{
		dam = rhp / 6;
		if(dam > 150) dam = 150;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_ACID)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 2 + 15;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_ELEC)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 4 + 8;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_FIRE)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 7 / 4 + 10;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_COLD)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3 / 4 + 10;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_POIS)
	{
		if(powerful) dam = 36;
		else dam = 18;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_NETH)
	{
		if(powerful) dam = rlev * 2 + 100;
		else dam = rlev + 100;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_WATE)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev + 50;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 10;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_MANA)
	{
		dam = rlev * 4  + 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_DARK)
	{
		dam = rlev * 4  + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_DARK) dam /= 5;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_DRAIN_MANA)
	{
		dam = rlev / 2 ; //rlev/4���炢�����񕜂�����̂ł�����Ə�悹
		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_MIND_BLAST)
	{
		dam = 25;
		if(enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->flags3 & RF3_NO_CONF ||
			(enemy_r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) dam = 0;
		else
		{
			if(rlev > 10) dam /= rlev / 10;
			if(rlev < 20) score_mult_plus += 10;
		}

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BRAIN_SMASH)
	{
		dam = 78;
		if(enemy_r_ptr->flags1 & RF1_UNIQUE ||  enemy_r_ptr->flags3 & RF3_NO_CONF ||
			(enemy_r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) dam = 0;
		else
		{
			if(rlev > 10) dam /= rlev / 10;
			if(rlev < 20) score_mult_plus += 20;
		}

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_1)
	{
		dam = 13;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120) / 2) / 100;//���G�ɂ͂قƂ�ǎh����Ȃ��̂ō����x���قǌ��炵�Ƃ��B


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_2)
	{
		dam = 36;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_3)
	{
		dam = 80;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CAUSE_4)
	{
		dam = 120;
		if(!monster_living(enemy_r_ptr)) dam = 0;
		else dam = dam * (60 - MIN(rlev,120)  / 2) / 100;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_ACID)
	{
		if(powerful) dam = rlev * 2 / 3 + 31;
		else dam = rlev / 3 + 31;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_ELEC)
	{
		if(powerful) dam = rlev * 2 / 3 + 18;
		else dam = rlev / 3 + 18;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_FIRE)
	{
		if(powerful) dam = rlev * 2 / 3 + 36;
		else dam = rlev / 3 + 36;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_COLD)
	{
		if(powerful) dam = rlev * 2 / 3 + 27;
		else dam = rlev / 3 + 27;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BA_LITE)
	{
		dam = rlev * 4  + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_NETH)
	{
		if(powerful) dam = rlev * 2 + 45;
		else dam = rlev * 4 / 3 + 45;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
		else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_WATE)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev + 50;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 10;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_MANA)
	{
		dam = rlev * 7 / 4 + 50;
		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_BO_PLAS)
	{
		if(powerful) dam = rlev * 3 / 2 + 42;
		else dam = rlev + 42;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags5 & RF5_BO_ICEE)
	{
		if(powerful) dam = rlev * 3 / 2 + 21;
		else dam = rlev + 21;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;
		if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 10;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags5 & RF5_MISSILE)
	{

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else dam = rlev/3 + 7;

		total_damage += dam;
		spell_cnt++;
	}
	//��Ⴢ⍬���Ȃǂ͂قƂ�ǎG���ɂ��������Ȃ��̂Ōv�Z�ɓ���Ȃ�
	if(r_ptr->flags5 & RF5_BLIND)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_CONF)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_SLOW)
	{
		spell_cnt++;
	}
	if(r_ptr->flags5 & RF5_HOLD)
	{
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HASTE)
	{
		score_mult_plus += 40;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HAND_DOOM)
	{
		if (enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->level > rlev + 10) dam = 0;
		else dam = 300; //�K��
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_HEAL)
	{
		score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_INVULNER)
	{
		score_mult_plus += 60;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_BLINK)
	{
		if(mode == 2) //�l�l��ł̓e���|�n���L��
			score_mult_plus += 20;
		else
			score_mult_plus += 10;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_TPORT)
	{
		if(mode == 2) //�l�l��ł̓e���|�n���L��
			score_mult_plus += 40;
		else
			score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_SPECIAL)
	{
		if(r_idx == MON_REIMU)
		{
			dam = rlev / 10 * 10 * rlev/8 / 2 ; //�얲���Z
		}
		else if(r_idx == MON_MOKOU)
		{
			int enemy_rhp = enemy_r_ptr->hdice * enemy_r_ptr->hside / ((enemy_r_ptr->flags1 & RF1_FORCE_MAXHP) ? 1:2) * 4 / 5;

			dam = enemy_rhp / 4; //3�`4��������Ƃ����炱��Ȃ���H
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
		}
		else dam = 0;
		total_damage += dam;
		if(dam) spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_TELE_TO)
	{
		score_mult_plus += 20;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_PSY_SPEAR)
	{
		if(powerful) dam = rlev+180;
		else	dam = rlev * 3 / 4 + 120;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags6 & RF6_COSMIC_HORROR)
	{
		dam = (rlev * 4 + 100) / 2;
		if (enemy_r_ptr->flags1 & RF1_UNIQUE || enemy_r_ptr->flags7 & RF7_UNIQUE2 || enemy_r_ptr->flags2 & RF2_ELDRITCH_HORROR || enemy_r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD)) dam = 0;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags9 & RF9_FIRE_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_ICE_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_THUNDER_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
		else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_ACID_STORM)
	{
		if(powerful) dam = rlev * 10 + 150;
		else dam = rlev * 7  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TOXIC_STORM)
	{
		if(powerful) dam = rlev * 8 + 75;
		else dam = rlev * 5  + 37;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_POLLUTE)
	{
		if(powerful) dam = rlev * 4 + 100;
		else dam = rlev * 3  + 75;
		if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_DISI)
	{
		if(powerful) dam = rlev * 2 + 75;
		else dam = rlev  + 38;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_HOLY)
	{
		if(powerful) dam = rlev * 2 + 100;
		else dam = rlev  + 100;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_METEOR)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 2 + 150;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BA_DISTORTION)
	{
		if(powerful) dam = rlev * 3 + 100;
		else dam = rlev * 3 / 2 + 100;
		if (enemy_r_ptr->flagsr & RFR_RES_TIME ) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_1)
	{
		dam = 13;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		//0�łȂ��Ƃ��Z�[�r���O�X���[�A�j�׎�_�A�N�O�t������𑫂��ăv���}�C�[���Ɨ��\�Ɍv�Z
		else dam = dam * (60 - MIN(rlev,120) / 2) / 50;//���G�ɂ͂قƂ�ǎh����Ȃ��̂ō����x���قǌ��炵�Ƃ��B

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_2)
	{
		dam = 36;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else dam = dam * (60 - MIN(rlev,120) / 2) / 50;//���G�ɂ͂قƂ�ǎh����Ȃ��̂ō����x���قǌ��炵�Ƃ��B
		//0�łȂ��Ƃ��Z�[�r���O�X���[�A�j�׎�_�A�N�O�t������𑫂��ăv���}�C�[���Ɨ��\�Ɍv�Z

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_3)
	{
		dam = 80;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else
		{
			dam = dam * (60 - MIN(rlev,120) / 2) / 50;//���G�ɂ͂قƂ�ǎh����Ȃ��̂ō����x���قǌ��炵�Ƃ��B
			if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
				score_mult_plus += 5;
		}
		//0�łȂ��Ƃ��Z�[�r���O�X���[�A�j�׎�_�A�N�O�t������𑫂��ăv���}�C�[���Ɨ��\�Ɍv�Z

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_PUNISH_4)
	{
		dam = 120;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY) dam = 0;
		else
		{
			dam = dam * (60 - MIN(rlev,120) / 2) / 50;//���G�ɂ͂قƂ�ǎh����Ȃ��̂ō����x���قǌ��炵�Ƃ��B
			if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
				score_mult_plus += 5;
		}
		//0�łȂ��Ƃ��Z�[�r���O�X���[�A�j�׎�_�A�N�O�t������𑫂��ăv���}�C�[���Ɨ��\�Ɍv�Z

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BO_HOLY)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev  + 50;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}

	if(r_ptr->flags9 & RF9_GIGANTIC_LASER)
	{
		if(powerful) dam = rlev * 8 + 50;
		else dam = rlev * 4 + 50;

		if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 3;
		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BO_SOUND)
	{
		if(powerful) dam = rlev * 3 / 2 + 50;
		else dam = rlev  + 16;

		if(enemy_r_ptr->flags2 & RF2_REFLECTING) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
		else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
				score_mult_plus += 20;


		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_BEAM_LITE)
	{
		if(powerful) dam = rlev * 4;
		else dam = rlev * 2;

		if (enemy_r_ptr->flagsr & RFR_RES_LITE) dam /= 5;
		else if (enemy_r_ptr->flagsr & RFR_HURT_LITE) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_HELLFIRE)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 3 + 80;

		if (enemy_r_ptr->flagsr & RFR_HURT_HELL) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_HOLYFIRE)
	{
		if(powerful) dam = rlev * 4 + 150;
		else dam = rlev * 3 + 80;

		if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
		else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
		else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_FINALSPARK)
	{
		dam = rlev * 5 / 2 + 200;

		total_damage += dam;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TORNADO)
	{
		if(powerful) dam = rlev * 3 + 75;
		else dam = rlev * 2 + 37;

		if (enemy_r_ptr->flags2 & RF2_GIGANTIC) dam /= 8;
		else if (enemy_r_ptr->flags7 & RF7_CAN_FLY) dam /= 3;

		total_damage += dam;
		spell_cnt++;
	}
	//���̖��@�������Ă�G�͗אڍU���������͂��H
	if(r_ptr->flags9 & RF9_TELE_APPROACH)
	{
		if(mode != 2)
			score_mult_plus += 15;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_TELE_HI_APPROACH)
	{
		if(mode != 2)
			score_mult_plus += 15;
		spell_cnt++;
	}
	if(r_ptr->flags9 & RF9_MAELSTROM)
	{
		if(powerful) dam = rlev * 3;
		else dam = rlev * 2;

		if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
		else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
		if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN))
			score_mult_plus += 20;

		total_damage += dam;
		spell_cnt++;
	}

	*spell_score_mult += score_mult_plus;
	if(!spell_cnt)
		return 0;
	else
		return (total_damage / spell_cnt);
}



/*:::battle_mon�p�@�w��G���ʂ̓G�ɗאڍU�������Ƃ��̊��҃_���[�W*/
//�ق��t���O�ɂ��X�R�A�{��������
int	calc_battle_mon_attack_dam(int r_idx, int enemy_r_idx, int *attack_score_mult, int mode)
{
	int ap_cnt;
	int dam;
	int total_damage = 0;
	int rlev;
	monster_race *r_ptr =  &r_info[r_idx];
	monster_race *enemy_r_ptr =  &r_info[enemy_r_idx];
	int score_mult_plus = 0;

	int hit_chance;
	int enemy_ac = enemy_r_ptr->ac;

	//v1.1.64 G�V���{���̍U���̓n�j���ɂ͌����Ȃ�
	if (r_ptr->d_char == 'G' && (enemy_r_ptr->flags3 & RF3_HANIWA))
		return 0;

	rlev = r_ptr->level;
	if(rlev < 1) rlev = 1;
	//�t���O��K���ɏ���
	if((r_ptr->flags2 & RF2_AURA_FIRE) && !(enemy_r_ptr->flagsr & RFR_IM_FIRE)) score_mult_plus += 3;
	if((r_ptr->flags2 & RF2_AURA_ELEC) && !(enemy_r_ptr->flagsr & RFR_IM_ELEC)) score_mult_plus += 3;
	if((r_ptr->flags2 & RF2_AURA_COLD) && !(enemy_r_ptr->flagsr & RFR_IM_COLD)) score_mult_plus += 3;

	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;

		if (r_ptr->flags1 & RF1_NEVER_BLOW) break;
		if(!method) break;
		if(!effect) break;
		if(effect == RBE_DR_MANA) continue;
		if(method == RBM_SHOOT) continue;

		dam = d_dice * (d_side + 1) / 2 * 100;

		//��������K���Ɍv�Z
		if(effect == RBE_HURT || effect == RBE_SHATTER || effect == RBE_SUPERHURT || effect == RBE_BLEED)
		{
			hit_chance = ((60 + rlev * 3) - ( enemy_ac * 3 / 4)) * 100 / (60 + rlev * 3);
			if(effect == RBE_SUPERHURT)
				dam -= (dam * ((enemy_ac < 150) ? enemy_ac : 150) / 350); //���\�ȊT�Z
			else
				dam -= (dam * ((enemy_ac < 150) ? enemy_ac : 150) / 250);
		}
		else
			hit_chance = ((10 + rlev * 3) - ( enemy_ac * 3 / 4)) * 100 / (60 + rlev * 3);

		if(hit_chance < 5) hit_chance = 5;
		if(hit_chance > 95) hit_chance = 95;

		switch (effect)
		{
		case RBE_POISON:
		case RBE_DISEASE:
			if (enemy_r_ptr->flagsr & RFR_IM_POIS) dam /= 9;
			break;

		case RBE_ACID:
			if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam /= 9;
			break;

		case RBE_ELEC:
			if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_ELEC) dam *= 2;
			break;

		case RBE_FIRE:
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_FIRE) dam *= 2;
			break;

		case RBE_COLD:
			if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam /= 9;
			else if (enemy_r_ptr->flagsr & RFR_HURT_COLD) dam *= 2;
			break;

		case RBE_UN_BONUS:
		case RBE_UN_POWER:
			if (enemy_r_ptr->flagsr & RFR_RES_DISE) dam /= 3;
			break;

		case RBE_CONFUSE: case RBE_INSANITY:
			if (enemy_r_ptr->flags3 & RF3_NO_CONF) dam /= 3;
			else score_mult_plus += 150 * hit_chance / 100;
			break;
		case RBE_EXP_10:
		case RBE_EXP_20:
		case RBE_EXP_40:
		case RBE_EXP_80:
		case RBE_KILL:
		case RBE_CURSE:
			if (enemy_r_ptr->flags3 & RF3_UNDEAD) dam = 0;
			else if (enemy_r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
			else if (enemy_r_ptr->flags3 & RF3_DEMON) dam /= 2;
			break;

		case RBE_TIME:
			if (enemy_r_ptr->flagsr & RFR_RES_TIME) dam /= 3;
			break;

		case RBE_INERTIA:
			if (enemy_r_ptr->flagsr & RFR_RES_INER) dam /= 3;

			break;

		case RBE_STUN:
			if (enemy_r_ptr->flagsr & RFR_RES_SOUN) dam /= 5;
			else if (!(enemy_r_ptr->flags3 & RF3_NO_STUN))
				score_mult_plus += 70 * hit_chance / 100;

		case RBE_CHAOS: case RBE_MUTATE:
			if (enemy_r_ptr->flagsr & RFR_RES_CHAO) dam /= 3;
			else if (!(enemy_r_ptr->flags3 & RF3_NO_CONF))
				score_mult_plus += 150 * hit_chance / 100;
			break;
		case RBE_ELEMENT: //�T�Z�l
			if (enemy_r_ptr->flagsr & RFR_IM_ACID) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_FIRE) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_ELEC) dam = dam * 4 / 7;
			if (enemy_r_ptr->flagsr & RFR_IM_COLD) dam = dam * 4 / 7;
			break;
		case RBE_SMITE:
			if (enemy_r_ptr->flagsr & RFR_RES_HOLY && enemy_r_ptr->flagsr & RFR_HURT_HELL) dam = 0;
			else if (enemy_r_ptr->flagsr & RFR_RES_HOLY ) dam /= 4;
			else if (enemy_r_ptr->flagsr & RFR_HURT_HOLY ) dam *= 2;
			break;
		case RBE_DROWN:
			if (enemy_r_ptr->flagsr & RFR_HURT_WATER) dam *= 2;
			else if (enemy_r_ptr->flagsr & RFR_RES_WATE) dam /= 3;
			if(!(enemy_r_ptr->flagsr & RFR_RES_WATE) && !(enemy_r_ptr->flags3 & RF3_NO_STUN))
				score_mult_plus += 60 * hit_chance / 100;
			break;
		case RBE_EAT_GOLD:
		case RBE_EAT_ITEM:
		case RBE_PHOTO:
			if(mode == 2) score_mult_plus += 50;
			break;

		default:
			break;
		}


		total_damage += dam * hit_chance / 10000;

	}

	*attack_score_mult += score_mult_plus;
	return (total_damage);
}

/*:::�V���Z���p�̃����X�^�[�I��֐� ���̂Ƃ��댶�z���̑���+�����_�����j�[�N�̂�*/
static bool monster_hook_battle_mon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	if(r_ptr->flagsr & RFR_RES_ALL) return FALSE; //�͂��ꃁ�^���͏o�Ȃ�
	if(r_ptr->flags7 & RF7_CHAMELEON) return FALSE; //�d���ς͏o�Ȃ�
	if(r_ptr->flags3 & RF3_WANTED) return FALSE; //���q�ˎ҂͏o�Ȃ�

	if(r_idx == MON_KOSUZU) return FALSE; //v1.1.37 ����(���߂��ꒆ)�͏o�Ȃ�

	//v1.1.42 ����2�͏o�Ȃ�
	if (r_idx == MON_SHION_2) return FALSE;

	//v1.1.98 ����͏o�Ȃ�
	if (r_idx == MON_MIZUCHI) return FALSE;

	if(r_idx == MON_MASTER_KAGUYA) return FALSE;//�P��(��)�͏o�Ȃ�

	if(r_ptr->flags7 & RF7_AQUATIC) return FALSE; //���̂Ƃ��됅���͕s��
	if((r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE)) return TRUE; //�����_�����j�[�N���o���Ă݂�H

	if (r_idx == MON_MIYOI) return FALSE; //v1.1.78 �����͏o�Ȃ�

   //������{���x���̔����ȉ��̃����X�^�[�͏o�Ȃ�
	if(r_ptr->level < battle_mon_base_level / 2) return FALSE;

	if(r_ptr->flags3 & (RF3_GEN_MASK)) return TRUE;

	return FALSE;
}

//�����`�[���̃t���O�l���牺�̃��[�`���p�̔z������𓾂�
static int calc_team_idx(u32b team)
{
	if(team & MFLAG_BATTLE_MON_TEAM_A) return 0;
	else if(team & MFLAG_BATTLE_MON_TEAM_B) return 1;
	else if(team & MFLAG_BATTLE_MON_TEAM_C) return 2;
	else if(team & MFLAG_BATTLE_MON_TEAM_D) return 3;
	else msg_print(_("ERROR:calc_team_idx�̈�������������",
                    "ERROR: weird argument in calc_team_idx"));
	return 0;
}

/*:::�V���Z��@��ԏ�*/
void battle_mon_gambling(void)
{
	int r1_idx;
	int r2_idx;
	int r3_idx,r4_idx;
	char tmp_val[10] = "";
	monster_race *r1_ptr;
	monster_race *r2_ptr;
	int i,j;

	int tot_dam[4],need_turn[4],all_enemy_hp[4],ave_enemy_hp[4],odds[4];
	int team_idx;
	int row, printingteam, max_bet;

	int r1spd,r2spd;
	int r1_useturn, r2_useturn;
	int r1_dd, r2_dd;

	int dummy;
	int r1_atk, r2_atk;
	int r1_mag,r2_mag;
	int fail_count = 0;
	int min_turn, temp_val, penalty_ef;

	byte mode = randint1(3);
	if(mode==3) mode++;//2��2�͂�����Ɖ҂��₷�����\�����ʓ|�Ȃ̂ł�߂�


	if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU))
	{
		msg_print(_("���̎������M�����u���Ɏg���킯�ɂ͂����Ȃ��B",
                    "You shouldn't spend your funds on gambling."));
		return;
	}
/*
	if(!p_ptr->wizard)
	{
		msg_print("�����͓̉����u�����ˁB�܂��������Ȃ񂾁B�v");
		return;
	}
*/
	/*::: Hack - ����_�Ђ́u�䂪�Ɓv�́u�Ō�ɖK�ꂽ�^�[���v�̒l���g���ē��Z��A���g�p��}�~*/
	if((town[TOWN_HAKUREI].store[STORE_HOME].last_visit > turn - (TURNS_PER_TICK * BATTLE_MON_TICKS)) && !p_ptr->wizard)
	{
		msg_print(_("�����͓̉����u���̎����̑g���܂����܂��ĂȂ��񂾁B���������҂��Ƃ���B�v",
                    "Freckled Kappa Girl - 'Next matchup hasn't been decided yet. Please wait a bit.'"));
		return;
	}
	town[TOWN_HAKUREI].store[STORE_HOME].last_visit = turn;

	//�σp�����[�^�����X�^�[�̃p�����[�^�v�Z�̂��߂��̒i�K��inside_battle��TRUE�ɂ���B
	//�L�����Z����G���[�Ȃǎ������n�߂��o��ꍇ�K��FALSE�ɂ��ďo�邱��
	p_ptr->inside_battle = TRUE;

	if(one_in_(4))
		battle_mon_base_level = randint1(p_ptr->lev * 2);
	else
		battle_mon_base_level = p_ptr->lev + randint1(p_ptr->lev);

	if(battle_mon_base_level < 30) battle_mon_base_level = 30;
	get_mon_num_prep(monster_hook_battle_mon,NULL);


	//�����X�^�[��I�肵�{�����v�Z����
	while(1)
	{
		bool flag_fail = FALSE;

		//�ϐ�������
		for(i=0;i<BATTLE_MON_LIST_MAX;i++) battle_mon_list[i].r_idx=0;
		for(i=0;i<BATTLE_MON_LIST_MAX;i++) battle_mon_list[i].team=0L;
		for(i=0;i<4;i++) battle_mon_team_type[i]=0;
		battle_mon_team_bet = 0L;
		for(i=0;i<4;i++) tot_dam[i]=0;
		for(i=0;i<4;i++) need_turn[i]=0;
		for(i=0;i<4;i++) all_enemy_hp[i]=0;
		for(i=0;i<4;i++) ave_enemy_hp[i]=0;
		for(i=0;i<4;i++) odds[i]=0;
		min_turn = -1;
		temp_val = 0;

		if(fail_count++ > 10000)
		{
			msg_print(_("WARNING:�V���Z��ɂă`�[���I�莎�s�񐔂�10000��𒴂���",
                        "WARNING: Attempted to pick teams for new arena over 10000 times"));
			get_mon_num_prep(NULL, NULL);
			p_ptr->inside_battle = FALSE;
			return;
		}

		//�o�ꃂ���X�^�[��I�肵���X�g�֊i�[
		//�����X�^�[�̓`�[�����Ƀ\�[�g����Ă��Ȃ���΂Ȃ�Ȃ�
		switch(mode)
		{
		case 1: //1��1
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;

			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[1] = 0;
			break;
		case 2: //4�l
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;
			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[1] = 0;
			battle_mon_list[2].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[2].team = MFLAG_BATTLE_MON_TEAM_C;
			battle_mon_team_type[2] = 0;
			battle_mon_list[3].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[3].team = MFLAG_BATTLE_MON_TEAM_D;
			battle_mon_team_type[3] = 0;
			break;
		case 3: //2��2
			battle_mon_list[0].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[0].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[0] = 0;
			battle_mon_list[1].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[1].team = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_team_type[1] = 0;
			battle_mon_list[2].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[2].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[2] = 0;
			battle_mon_list[3].r_idx = get_mon_num(battle_mon_base_level);
			battle_mon_list[3].team = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_team_type[3] = 0;
			break;
		case 4: //�`�[��
			{
				int cnt = 0;
				u32b team_flag;
				for(i=0;i<2;i++)
				{
					if(i==0) team_flag = MFLAG_BATTLE_MON_TEAM_A;
					else	 team_flag = MFLAG_BATTLE_MON_TEAM_B;

					if(one_in_(2)) //�Œ�`�[��
					{
						int special_team_idx;
						do
						{
							special_team_idx = randint1(BATTLE_MON_SPECIAL_TEAM_NUM);
						}while(battle_mon_special_team_list[special_team_idx].level > battle_mon_base_level
							|| i == 1 && (battle_mon_team_type[0] + special_team_idx == 0));

						battle_mon_team_type[i] = 0 - special_team_idx;
						for(j=0;j<8;j++)
						{
							if(!battle_mon_special_team_list[special_team_idx].r_idx_list[j]) break;
							battle_mon_list[cnt].r_idx =battle_mon_special_team_list[special_team_idx].r_idx_list[j] ;
							battle_mon_list[cnt++].team = team_flag;
						}

					}
					else //�����_���`�[��
					{
						int num = 1 + randint1(3);
						int random_team_name;
						do
						{
							random_team_name = randint1(BATTLE_MON_RANDOM_TEAM_NAME_NUM);
						}while(i == 1 && random_team_name == battle_mon_team_type[0]);
						battle_mon_team_type[i] = random_team_name;
						if(one_in_(3)) num += randint1(4);
						for(j=0;j<num;j++)
						{
							battle_mon_list[cnt].r_idx = get_mon_num(battle_mon_base_level);
							battle_mon_list[cnt++].team = team_flag;

						}
					}
				}
			}
			break;

		default:
			msg_format(_("ERROR:�V���Z��`�[���I�胂�[�h�l(%d)����������`",
                        "ERROR: Don't know how to handle chosen mode (%d) in new team arena"),mode);
			get_mon_num_prep(NULL, NULL);
			p_ptr->inside_battle = FALSE;
			return;
		}

		//���j�[�N�����X�^�[�̏d��������Ƃ������X�^�[�I�肩���蒼��
		//mode1,2�̂Ƃ��̓��j�[�N�łȂ��Ă��d���s��
		for(i=0;i<BATTLE_MON_LIST_MAX-1;i++)
		{
			if(flag_fail || !battle_mon_list[i].r_idx) break;


			for(j=i+1;j<BATTLE_MON_LIST_MAX;j++)
			{
				if(!battle_mon_list[j].r_idx) break;
				if(battle_mon_list[i].r_idx == battle_mon_list[j].r_idx)
				{
					if(mode == 1 || mode == 2 || mode == 3)
					{
						flag_fail = TRUE;
						break;
					}
					else if(r_info[battle_mon_list[i].r_idx].flags1 & RF1_UNIQUE || r_info[battle_mon_list[i].r_idx].flags7 & RF7_UNIQUE2)
					{
						//v1.1.42 ���Z��p俎q�����͏d������OK�ɂ���B�Ђ���Ƃ�����`�[����ŘZ�l�ɂȂ邩������Ȃ�������͂���Ŗʔ���
						if (battle_mon_list[i].r_idx != MON_SUMIREKO_2)
						{
							flag_fail = TRUE;
							break;
						}
					}
				}
			}
			if(battle_mon_list[i].r_idx == MON_MARISA)
			{
				//�������͂��̂��h
				if(battle_mon_team_type[calc_team_idx(battle_mon_list[i].team)] == 2) flag_fail = TRUE;
			}
		}
		if(flag_fail) continue;
		//�σp�����[�^�����X�^�[�̃p�����[�^�v�Z������
		for(i=0;i<BATTLE_MON_LIST_MAX;i++)
		{
			if(battle_mon_list[i].r_idx == MON_REIMU) apply_mon_race_reimu();
			else if(battle_mon_list[i].r_idx == MON_MARISA) apply_mon_race_marisa();
			else if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx)) apply_random_unique(battle_mon_list[i].r_idx);
		}
		if(cheat_xtra) msg_format("");
		//��������S�`�[���̐�͕]���Ə����v�Z�����A�{����ݒ肷��
		//�e�`�[���̓G�S�Ă�HP�����v����
		for(i=0;i<4;i++)
		{
			int enemy_cnt = 0;
			for(j=0;j<BATTLE_MON_LIST_MAX;j++)
			{
				int mon_team_idx;

				if(flag_fail || !battle_mon_list[j].r_idx) break;
				mon_team_idx = calc_team_idx(battle_mon_list[j].team);

				if(i == mon_team_idx) continue;
				enemy_cnt++;
				r2_ptr =  &r_info[battle_mon_list[j].r_idx];

				if(r2_ptr->flags1 & RF1_FORCE_MAXHP)
					all_enemy_hp[i] += r2_ptr->hdice * r2_ptr->hside;
				else
					all_enemy_hp[i] += r2_ptr->hdice * (r2_ptr->hside+1)/2;
			}
			if(enemy_cnt) ave_enemy_hp[i] = all_enemy_hp[i] / enemy_cnt;
		}
		if(cheat_xtra) msg_format("EnemHP:%d:%d:%d:%d",all_enemy_hp[0],all_enemy_hp[1],all_enemy_hp[2],all_enemy_hp[3]);
		if(cheat_xtra) msg_format("Ave_HP:%d:%d:%d:%d",ave_enemy_hp[0],ave_enemy_hp[1],ave_enemy_hp[2],ave_enemy_hp[3]);

		//�e�`�[���\�����ɂ��G�ւ̕��ύU���͊T�Z�l���Z�o
		for(i=0;i<BATTLE_MON_LIST_MAX;i++)
		{
			int r1_hp, low_hp_mult;
			if(flag_fail || !battle_mon_list[i].r_idx) break;
			team_idx = calc_team_idx(battle_mon_list[i].team);
			r1_ptr =  &r_info[battle_mon_list[i].r_idx];

			if(r1_ptr->flags1 & RF1_FORCE_MAXHP)
				r1_hp = r1_ptr->hdice * r1_ptr->hside;
			else
				r1_hp = r1_ptr->hdice * (r1_ptr->hside+1) / 2;

			//4�l�킩�`�[����̂Ƃ�HP���Ⴂ�L�����Ƀy�i���e�B
			low_hp_mult = 50 * (r1_hp + ave_enemy_hp[team_idx])/ ave_enemy_hp[team_idx];
			if(low_hp_mult > 100) low_hp_mult = 100;

			//���̃L�����N�^�[�ɂ�邷�ׂĂ̓G�ւ̍U�����σ_���[�W���Z�o
			for(j=0;j<BATTLE_MON_LIST_MAX;j++)
			{
				int enemy_hp;
				int mult = 100, tmp_atk=0, tmp_mag=0, tmp_tot_dam, dist_mult;
				if(flag_fail || !battle_mon_list[j].r_idx) break;
				if(battle_mon_list[i].team & battle_mon_list[j].team) continue; //���`�[���͌v�Z���Ȃ�
				r2_ptr =  &r_info[battle_mon_list[j].r_idx];

				if(r2_ptr->flags1 & RF1_FORCE_MAXHP)
					enemy_hp = r2_ptr->hdice * r2_ptr->hside;
				else
					enemy_hp = r2_ptr->hdice * (r2_ptr->hside+1) / 2;

				//���̓G�ɑ΂���אڍU�����҃_���[�W�T�Z
				tmp_atk += calc_battle_mon_attack_dam(battle_mon_list[i].r_idx, battle_mon_list[j].r_idx, &mult,mode);
				//���̓G�ɑ΂��鉓�u�U�����҃_���[�W�T�Z
				tmp_mag += calc_battle_mon_spell_dam(battle_mon_list[i].r_idx, battle_mon_list[j].r_idx, &mult,mode);
				//�����Ȃ������͂Ȗ��@�������Ă���G�ɕ␳
				///mod160206 ������Ƌ���
				//v1.1.64 �����Ƌ���
				if ((r1_ptr->flags1 & RF1_NEVER_MOVE) && tmp_mag > r1_ptr->level) mult += 200;

				//4�l�킩�`�[����̂Ƃ�HP���Ⴂ�L�����Ƀy�i���e�B
				if(mode == 2 || mode == 4)
				{
					mult = mult * low_hp_mult / 100;
					if(mult < 1) mult = 1;
				}
				//�჌�x���ł���قǋߋ����U�����ア�ݒ�ɂ��Ă���
				dist_mult = MIN(80, (10 + r1_ptr->level * 4 / 3));
				if(((r1_ptr->flags1 & (RF1_RAND_50 | RF1_RAND_25)) == (RF1_RAND_50 | RF1_RAND_25))) dist_mult /= 4;
				else if(r1_ptr->flags1 & (RF1_RAND_50)) dist_mult /= 2;
				else if(r1_ptr->flags1 & (RF1_RAND_25)) dist_mult = dist_mult * 3 / 4;

				//���̓G�ɑ΂�����҃_���[�W�@�אڂ͋߂Â��܂ł̌������镪������ƌ��炷�B�����A���@�g�p���A�⏕�I�{���l��
				tmp_tot_dam = ( tmp_atk * (100 - r1_ptr->freq_spell) * dist_mult / 100 + tmp_mag * (r1_ptr->freq_spell)) / 100 * SPEED_TO_ENERGY(r1_ptr->speed) / 10 * mult / 100;
				//�GHP���v�ɑ΂��邱�̓G��HP�̊����ɂ���ă_���[�W��������
				tmp_tot_dam = tmp_tot_dam * enemy_hp / all_enemy_hp[team_idx];

				if(!tmp_tot_dam) tmp_tot_dam = 1;
				tot_dam[team_idx] += tmp_tot_dam;
			}
		}
		if(cheat_xtra) msg_format("Tot_dam:%d:%d:%d:%d",tot_dam[0],tot_dam[1],tot_dam[2],tot_dam[3]);

		//���̃`�[�����S�Ă̓G��|���܂ł̏��v�^�[������(*100)���Z�o
		//(�S�Ă̓G��HP���v)/(�S�Ă̓G�ւ̊��҃_���[�W���ϒl(�GHP�ɂ����d����)�̃`�[�����v) �S�Ă̓G��|���܂ł̊��҃^�[����(*100)���o��͂�
		for(i=0;i<4;i++)
		{
			if(!tot_dam[i]) break;//���݂��Ȃ��`�[��
			need_turn[i] = all_enemy_hp[i] * 100 / tot_dam[i];

			if(need_turn[i] > 100000)//�G��|���̂�1000�^�[���ȏォ����`�[�����o���烂���X�^�[�I�肩���蒼��
			{
				flag_fail = TRUE;
				break;
			}
			if(min_turn < 0) min_turn = need_turn[i];
			else if(min_turn > need_turn[i]) min_turn = need_turn[i];
		}
		if(flag_fail) continue;

		if(cheat_xtra) msg_format("Need_turn:%d:%d:%d:%d",need_turn[0],need_turn[1],need_turn[2],need_turn[3]);

		//�y�i���e�B�W���@�s���̎��Ԃ��N����ɂ����Q�[���قǃy�i���e�B����
		if(mode == 1) penalty_ef = 35;
		else if(mode == 2) penalty_ef = 3;
		else if(mode == 3) penalty_ef = 20;
		else if(mode == 4) penalty_ef = 7;

		//�\�z���v�^�[���̑����`�[���ɑ΂��ăy�i���e�B�K�p(�\�z���v�^�[���̑����`�[���͍��m���ŏ��v�^�[���O�ɕ�����̂�)
		//���^�[�����Ȃ�^�[���������قǍ����������A���^�[���䗦�Ȃ�^�[���������قǍ����傫���Ȃ�悤�ɂ��Ƃ�
		//�y�i���e�B�K�p��̃^�[�����Ɂ}20%�̃����_���␳
		for(i=0;i<4;i++)
		{
			int delay;
			if(!tot_dam[i]) break;//���݂��Ȃ��`�[��
			if(need_turn[i] > min_turn)
			{
				delay = (need_turn[i] - min_turn) / 100;

				need_turn[i] += delay * delay * 100 * penalty_ef / 10;
				//�y�i���e�B�K�p��5000�^�[���ȏォ����`�[����5�^�[���ȉ�����������Ȃ��`�[�����o���烂���X�^�[�I�肩���蒼��
				if(need_turn[i] > 500000 || need_turn[i] < 500)
				{
					flag_fail = TRUE;
					break;
				}
			}
			need_turn[i] = need_turn[i] * (75 + damroll(5,9)) / 100;

			temp_val += 1000000 / need_turn[i];
		}
		if(flag_fail) continue;
		if(cheat_xtra) msg_format("Needturn2:%d:%d:%d:%d",need_turn[0],need_turn[1],need_turn[2],need_turn[3]);

		//�y�i���e�B�K�p�ς݂̃^�[������K���Ɍv�Z���Ĕ{��(*100)���Z�o����
		for(i=0;i<4;i++)
		{
			int delay;
			if(!tot_dam[i]) break;//���݂��Ȃ��`�[��
			//�I�[�o�[�t���[���邩�������̂ňꉞ��O����
			if(need_turn[i] < 100000)
				odds[i] = need_turn[i] * temp_val / 10000;
			else
				odds[i] = need_turn[i] / 100 * temp_val / 100;

			//���܂�{��������������Ⴗ�����肷��`�[�����o��Ƃ�蒼��
			switch(mode)
			{
			case 1:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 2:
				if(odds[i] >= 100000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 3:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			case 4:
				if(odds[i] >= 10000 || odds[i] < 110 ) flag_fail = TRUE;
				break;
			default:
				break;
			}
		}

		if(cheat_xtra) msg_format("Odds:%d:%d:%d:%d",odds[0],odds[1],odds[2],odds[3]);

		if(flag_fail) continue;
		break;
	}
	get_mon_num_prep(NULL, NULL);

	if(cheat_xtra) msg_format("fail_count:%d",fail_count);

	screen_save();
	clear_bldg(4,22);
	printingteam = -1;
	row = 4;
	for(i=0;i<BATTLE_MON_LIST_MAX;i++)
	{
		char buf[160];
		char tmp_str[200];

		if(!battle_mon_list[i].r_idx) break;

		team_idx = calc_team_idx(battle_mon_list[i].team);

		if(!battle_mon_team_type[team_idx]) //�P�ƃ`�[���@�L�������ƃI�b�Y���Z�b�g�ŕ\�L
		{
			printingteam = team_idx;
			if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx))
				sprintf(buf, " %s ", random_unique_name[battle_mon_list[i].r_idx - MON_RANDOM_UNIQUE_1]);
			else if(battle_mon_list[i].r_idx == MON_REIMU || battle_mon_list[i].r_idx == MON_MARISA)
				sprintf(buf, " %s(Lv%d) ", r_name+r_info[battle_mon_list[i].r_idx].name, r_info[battle_mon_list[i].r_idx].level);
			else
				sprintf(buf, " %s ", r_name+r_info[battle_mon_list[i].r_idx].name);
			sprintf(tmp_str, "[%c](x%3d.%02d) %s",'a'+team_idx, odds[team_idx]/100,odds[team_idx]%100, buf );
			prt(tmp_str, row, 1);

		}
		else //�����`�[��
		{
			if(printingteam != team_idx) //���̃`�[���̏��߂ĕ\������L�����̂Ƃ��A���̑O�Ƀ`�[�����\��
			{
				printingteam = team_idx;
				if(battle_mon_team_type[team_idx] < 0)
					sprintf(buf, " %s ", battle_mon_special_team_list[0-battle_mon_team_type[team_idx]].name);
				else
					sprintf(buf, " %s ", battle_mon_random_team_name_list[battle_mon_team_type[team_idx]]);
				sprintf(tmp_str, "[%c](x%3d.%02d) %s",'a'+team_idx, odds[team_idx]/100,odds[team_idx]%100, buf );
				prt(tmp_str, row, 1);
				row++;
			}
			if(IS_RANDOM_UNIQUE_IDX(battle_mon_list[i].r_idx))
				sprintf(buf, " %s ", random_unique_name[battle_mon_list[i].r_idx - MON_RANDOM_UNIQUE_1]);
			else if(battle_mon_list[i].r_idx == MON_REIMU || battle_mon_list[i].r_idx == MON_MARISA)
				sprintf(buf, " %s(Lv%d) ", r_name+r_info[battle_mon_list[i].r_idx].name, r_info[battle_mon_list[i].r_idx].level);
			else
				sprintf(buf, " %s ", r_name+r_info[battle_mon_list[i].r_idx].name);
			sprintf(tmp_str, "               %s", buf );
			prt(tmp_str, row, 1);
		}
		row++;
	}

	//�{���\���ƃ`�[���I��
	max_bet = p_ptr->lev;
	if(p_ptr->total_winner) max_bet = 100;
	if(max_bet > p_ptr->au / 1000) max_bet = p_ptr->au / 1000;


	if(p_ptr->psex == SEX_MALE) msg_print(_("�����͓̉����u�₠�Z����A�ǂ������H�v", "Freckled Kappa Girl - 'What will it be?'"));
	else msg_print(_("�����͓̉����u�₠������A�ǂ������H�v", "Freckled Kappa Girl - 'What will it be?'"));

	while(1)
	{
		char c;
		battle_mon_team_bet = 0L;

		c = inkey();
		if(c == ESCAPE)
		{
			msg_print(_("�u�����A�q���Ȃ��̂����H�v", "What? You're not betting?"));
			break;
		}
		if(c == 'a' || c == 'A')
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_A;
			battle_mon_odds = odds[0];
		}
		else if(c == 'b' || c == 'B')
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_B;
			battle_mon_odds = odds[1];
		}
		else if((c == 'c' || c == 'C') && odds[2])
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_C;
			battle_mon_odds = odds[2];
		}
		else if((c == 'd' || c == 'D') && odds[3])
		{
			battle_mon_team_bet = MFLAG_BATTLE_MON_TEAM_D;
			battle_mon_odds = odds[3];
		}
		else continue;

		if(!max_bet)
		{
			battle_mon_team_bet = 0L;
			msg_print(_("�u�������A�����I�P������Ȃ����B�v", "'Oh, you don't have anything."));
			break;
		}
		battle_mon_wager = get_quantity(format(_("�u���$1000����B�����q����ˁH�v(MAX:%d)",
                                                "'You wager in increments of $1000. How much is it going to be?' (MAX: %d)"),max_bet),max_bet) * 1000;

		if(!battle_mon_wager)
		{
			msg_print(_("�u�I�ђ����̂����H�v", "'Changed your mind?'"));
			continue;
		}
		break;
	}
	screen_load();

	if(!battle_mon_team_bet)
	{
		p_ptr->inside_battle = FALSE;
		return;
	}

	//�����łȂ������I����ɏ����������������邱�Ƃɂ����B�Ȃ��������J�n����ɓ��Z�ꂪ�I�����邱�Ƃ����邽�߁B
	//p_ptr->au -= battle_mon_wager;
	set_action(ACTION_NONE);
	prepare_change_floor_mode(CFM_SAVE_FLOORS);
	p_ptr->leaving = TRUE;

}



static bool item_tester_hook_marisa_good_book(object_type *o_ptr)
{
	if(o_ptr->tval >= TV_BOOK_ELEMENT && o_ptr->tval <= TV_BOOK_CHAOS && o_ptr->sval > 1) return (TRUE);
	else return (FALSE);
}

//��������p ���@��tval��sval����magic_num[]�Y����Ԃ�
static int marisa_book_to_magic_num(int tval, int sval)
{
	if(tval > MAX_MAGIC || tval < MIN_MAGIC || sval < 2)
	{
		msg_format(_("ERROR:marisa_book_to_magic()��3,4���ږ��@���ȊO�̕����n���ꂽ",
                    "ERROR: marisa_book_to_magic() received item other than Volume 3/4 of magic books"));
		return -1;
	}

	return (((sval)-2) * 12 + (tval) - 1);

}

/*::�������̖��@�K��*/
void marisa_make_magic_recipe(void)
{
	object_type forge;
	object_type *q_ptr = &forge;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int item, magic_num;
	int i;
	char magic_name[80];

	if(p_ptr->pclass != CLASS_MARISA)
    {
        msg_print(_("ERROR:�������ȊO��marisa_make_magic()���Ă΂ꂽ",
            "ERROR: marisa_make_magic() called for non-Marisa")); return;
    }

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return;
	}

	if(p_ptr->magic_num2[27])
	{
		msg_print(_("�������@�̃A�C�f�A�͏\���ɏW�߂��B", "You already have gathered plenty of ideas for spells."));
		return;
	}

	q = _("�ǂ̖��@�����������܂����H", "Study which spellbook?");
	s = _("�ʔ������Ȗ{���Ȃ��B", "You don't have any interesting books.");
	item_tester_hook = item_tester_hook_marisa_good_book;

	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	magic_num = marisa_book_to_magic_num(o_ptr->tval, o_ptr->sval);

	if(magic_num < 0 || magic_num > 23) { msg_format(_("ERROR:������magic_num����������(%d)",
                                                        "ERROR: Weird magic_num for Marisa (%d)"),magic_num); return;}

	if(p_ptr->magic_num2[magic_num])
	{
		msg_print(_("���̖{�͊��ɓǗ��ς݂��B", "You've already finished this book."));
		return;
	}
	if(one_in_(4)) c_put_str(TERM_WHITE,_("���Ȃ��͖{��Ў�ɖ��@���L�m�R�������n�߂��E�E",
                                            "You start mixing mushrooms and potions while holding the book in one hand..."),8 , 6);
	else if(one_in_(3)) c_put_str(TERM_WHITE,_("���Ȃ��͖{��ǂ�ŃC���X�s���[�V�����𓾂��B",
                                                "You gain inspiration from reading the book."),8 , 6);
	else if(one_in_(2))  c_put_str(TERM_WHITE,_("���Ȃ��͖{���q���g�ɐV���ȃ��V�s���l���n�߂��E�E",
                                                "The books gave you some hints to possible new recipes..."),8 , 6);
	else c_put_str(TERM_WHITE,_("���Ȃ��͖{��ǂ݂Ȃ���l�X�Ȏ������n�߂��E�E",
                                "You started doing experiments while reading the book..."),8 , 6);
	(void)inkey();

#ifdef JP
	sprintf(magic_name,"%s%s������悤�ɂȂ����I",marisa_magic_table[magic_num].name,(marisa_magic_table[magic_num].is_drug) ? "" : "�̖��@" );
#else
    sprintf(magic_name,"You now can make %s%s!", (marisa_magic_table[magic_num].is_drug) ? "" : "magic of ", marisa_magic_table[magic_num].name);
#endif

	p_ptr->magic_num2[magic_num] = 1;
	c_put_str(TERM_WHITE,magic_name,9 , 7);

	//���F�}�W�b�N�i���o�[����
	if(magic_num >= 12 && magic_num <= 15 && p_ptr->magic_num2[12]  && p_ptr->magic_num2[13] && p_ptr->magic_num2[14] && p_ptr->magic_num2[15])
	{
		p_ptr->magic_num2[24] = 1;
		c_put_str(TERM_WHITE,format(_("�����%s�̖��@������悤�ɂȂ����I",
                                    "You also can create the spell '%s'!"),marisa_magic_table[24].name),10 , 8);
	}
	else if(magic_num >= 16 && magic_num <= 19 && p_ptr->magic_num2[16]  && p_ptr->magic_num2[17] && p_ptr->magic_num2[18] && p_ptr->magic_num2[19])
	{
		p_ptr->magic_num2[25] = 1;
		c_put_str(TERM_WHITE,format(_("�����%s�̖��@������悤�ɂȂ����I",
                                    "You also can create the spell '%s'!"),marisa_magic_table[25].name),10 , 8);
	}
	else if(magic_num >= 20 && magic_num <= 23 && p_ptr->magic_num2[20]  && p_ptr->magic_num2[21] && p_ptr->magic_num2[22] && p_ptr->magic_num2[23])
	{
		p_ptr->magic_num2[26] = 1;
		c_put_str(TERM_WHITE,format(_("�����%s�̖��@������悤�ɂȂ����I",
                                    "You also can create the spell '%s'!"),marisa_magic_table[26].name),10 , 8);
	}

	for(i=0;i<27;i++) if(!p_ptr->magic_num2[i]) break;
	if(i == 27)
	{
		p_ptr->magic_num2[27] = 1;
		c_put_str(TERM_WHITE,format(_("������%s�̖��@������悤�ɂȂ����I",
                                    "At last, you can create the spell '%s'!"),marisa_magic_table[27].name),11 , 9);
	}

	return;
}


/*:::���������w�肵�����@���������邩�`�F�b�N*/
static int check_marisa_can_make_spell(int spell_num)
{
	int i;
	int num = 99;

	if(p_ptr->pclass != CLASS_MARISA)
    {
        msg_print(_("ERROR:marisa_can_make_spell()���������ȊO�ŌĂ΂ꂽ",
            "ERROR: marisa_can_make_spell() called for non-Marisa"));
        return 0;
    }

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return 0;
	}

	//���J���̖��@(�����ɂ͗��Ȃ��͂�)
	if(!p_ptr->magic_num2[spell_num]) return 0;

	///mod151224 �������f�o�b�O���[�h�ȂǗp
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return 99;

	//�ޗ�������Ă��邩�`�F�b�N����
	for(i=0;i<8;i++)
	{
		int num_tmp;
		if(!marisa_magic_table[spell_num].use_magic_power[i]) continue;

		if(p_ptr->noscore)
			num_tmp = p_ptr->magic_num1[30+i] / marisa_magic_table[spell_num].use_magic_power[i];
		else
			num_tmp = marisa_magic_power[i] / marisa_magic_table[spell_num].use_magic_power[i];

		if(!num_tmp) return 0;
		if(num_tmp < num) num = num_tmp;
	}

	return num;
}

/*:::�������̖��@�I�� �I������marisa_magic_table�Y���A�L�����Z������-1���Ԃ�*/
int choose_marisa_magic(int mode)
{
	int num = -1;
	int i;
	int carry_num = 0;
	bool flag_repeat = FALSE;

	for(i=0;i<MARISA_MAX_MAGIC_KIND;i++) if(p_ptr->magic_num2[i]) break;
	if(i == MARISA_MAX_MAGIC_KIND)
	{
		msg_print(_("�܂�������@�̃A�C�f�A���Ȃ��B", "You don't have any ideas for spells."));
		return -1;
	}
	if(mode == CMM_MODE_CARRY)
	{
		for(i=0;i<MARISA_MAX_MAGIC_KIND;i++) carry_num += p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT];
	}

	screen_save();
	//�������������łȂ����ŉ�ʃN���A����̈��ς��Ă����B���screen_save()����character_icky++�����̂�TRUE/FALSE�ł͔���ł��Ȃ��B
	if(character_icky > 1)
		clear_bldg(4,22);
	else
		for(i=4;i<22;i++) Term_erase(17, i, 255);


	if(mode == CMM_MODE_CHECK)
		prt(_("�u�ǂ̖��@�ɂ��Ċm�F���悤�H�v(ESC:�L�����Z��)",
            "'Browse which spell?' (ESC: Cancel)"),4,18);
	else if(mode == CMM_MODE_CARRY)
	{
		prt(_("�u�ǂ̖��@�������čs�����H�v",
            "'Carry which spell?'"),4,18);
		prt(format(_(" (����%d ESC:�I���I���@'R':�������@��S�ĉƂɖ߂�)",
                    " (%d left ESC: Go back 'R': Return all carried spells to house)"),(MARISA_CARRY_MAGIC_NUM-carry_num)),5,18);
	}
	else if(mode == CMM_MODE_MAKE)
		prt(_("�u�ǂ̖��@����낤�H�v(ESC:�L�����Z��)",
            " 'Create which spell?' (ESC: Cancel)"),4,18);
	else
		prt(_("�u�ǂ̖��@���g�����H�v(ESC:�L�����Z��)",
            " 'Use which spell?' (ESC: Cancel)"),4,18);

	for(i=0;i<MARISA_MAX_MAGIC_KIND;i++)
	{
		char magic_desc[80];
		char sym = (i < 26)?('a'+i):('A'+i-26);

		//���쐬���V�s�͕\�����Ȃ�
		if(!p_ptr->magic_num2[i]) continue;

		if(mode == CMM_MODE_CARRY || mode == CMM_MODE_CHECK)
		{
			sprintf(magic_desc,"%c)%s(%d/%d)",sym,marisa_magic_table[i].name,p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT],p_ptr->magic_num1[i]);
		}
		else if(mode == CMM_MODE_CAST)
		{
			sprintf(magic_desc,"%c)%s(%d)",sym,marisa_magic_table[i].name,p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT]);
		}
		else
		{
			sprintf(magic_desc,"%c)%s%s(%d/%d)",sym,marisa_magic_table[i].name,(marisa_magic_table[i].is_drug)?_("����", ""):"",p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT],p_ptr->magic_num1[i]);
		}

		if(mode == CMM_MODE_CARRY)
		{
			if(carry_num >= MARISA_CARRY_MAGIC_NUM || !p_ptr->magic_num1[i])
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else if(mode == CMM_MODE_CAST)
		{
			if(!p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT])
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else if(mode == CMM_MODE_MAKE)
		{
			//���@�쐬���A�ޗ�������Ȃ����@�͊D��
			if(!check_marisa_can_make_spell(i))
				c_prt(TERM_L_DARK, magic_desc,7+(i%14),18+(i/14) * 36);
			else
				prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}
		else
		{
			prt(magic_desc,7+(i%14),18+(i/14) * 36);
		}

	}

#ifdef ALLOW_REPEAT
	if (mode == CMM_MODE_CAST && repeat_pull(&num))
	{
		flag_repeat = TRUE;
	}
#endif

	while(1)
	{

		char c;

		if(!flag_repeat)
		{
			num = -1;
			c = inkey();
			if(c == ESCAPE) break;
			else if(c >= 'a' && c <= 'z') num = c - 'a';
			else if(c == 'A' || c == 'B') num = c - 'A' + 26;
			else if(c == 'R')
			{
				if(mode == CMM_MODE_CARRY)
				{
					num = 99;//�������@���Z�b�g
					break;
				}
				else continue;
			}
			else continue;
		}

		//���쐬���V�s�͑I������Ȃ�
		if(!p_ptr->magic_num2[num]) continue;
		//���@�쐬���A�ޗ�������Ȃ����@�͑I������Ȃ�
		if(mode == CMM_MODE_MAKE  && !check_marisa_can_make_spell(num))
		{
			msg_print(_("��������ɂ͍ޗ�������Ȃ��B", "You don't have enough materials to create this."));
			flag_repeat = FALSE;
			msg_print(NULL);
			continue;
		}
		else if(mode == CMM_MODE_CAST && !p_ptr->magic_num1[num+MARISA_HOME_CARRY_SHIFT])
		{
			msg_print(_("���̖��@�͎莝�����Ȃ��B", "You're not carrying this spell."));
			num = -1;
			break;
		}
		else if(mode == CMM_MODE_CARRY)
		{
			if(!p_ptr->magic_num1[num])
			{
				msg_print(_("���@�̍��u�����Ȃ��B", "You don't have any created spells in reserve."));
				flag_repeat = FALSE;
				msg_print(NULL);
				continue;
			}
			if(carry_num >= MARISA_CARRY_MAGIC_NUM)
			{
				msg_print(_("�������ĂȂ��B", "You can't hold any more."));
				flag_repeat = FALSE;
				msg_print(NULL);
				continue;
			}
		}
#ifdef ALLOW_REPEAT /* TNB */
		if(!flag_repeat) repeat_push(num);
#endif /* ALLOW_REPEAT -- TNB */


		break;

	}

	screen_load();
	return num;

}

//�������̖��@�쐬
bool make_marisa_magic(void)
{
	int spell_num;
	int amount;
	int max;
	int i;

	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print(_("ERROR:make_marisa_magic()���������ȊO�ŌĂ΂ꂽ",
                "ERROR: make_marisa_magic() called for non-Marisa")); return FALSE;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return FALSE;
	}

	//�쐬���閂�@��I���@�ޗ�������Ă��邱�Ƃ��m�F
	spell_num = choose_marisa_magic(CMM_MODE_MAKE);
	if(spell_num < 0) return FALSE;
	if(p_ptr->magic_num1[spell_num] >= 9999)
	{
		msg_print(_("��������͎g���؂�Ȃ��قǍ��u���Ă���B",
                    "You already have created enough of this."));
		return FALSE;
	}
	max = check_marisa_can_make_spell(spell_num);

	if(p_ptr->magic_num1[spell_num] + max > 9999) max = 9999 - p_ptr->magic_num1[spell_num];

	amount = get_quantity(format(_("%s��������낤�H(1-%d)",
                                    "Create how many %s? (1-%d)"),marisa_magic_table[spell_num].name,max),max);

	if(amount < 1 || amount > 99) return FALSE;

	prt(_("���Ȃ��͒����Ɏ��|�������E�E",
            "You start mixing..."),8,20);//���������ƃ��b�Z�[�W���₻����
	inkey();

#ifdef JP
	prt(format("%s%s�����������I",marisa_magic_table[spell_num].name,(marisa_magic_table[spell_num].is_drug)?"":"�̖��@" ),10,20);
#else
    prt(format("%s%s complete!",(marisa_magic_table[spell_num].is_drug)? "":"spell ", marisa_magic_table[spell_num].name),10,20);
#endif
	p_ptr->magic_num1[spell_num] += amount;


	///mod151224 �������f�o�b�O���[�h�ȂǗp
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return TRUE;
	///mod160103 ���i�������܈ȊO�̃`�[�g�I�v�V�����g�p���͕ʗ̈���g�����Ƃɂ���
	if(p_ptr->noscore)
	{
		for(i=0;i<8;i++)
		{
			if((int)p_ptr->magic_num1[30+i] < amount * marisa_magic_table[spell_num].use_magic_power[i])
			{
				msg_print(_("ERROR:���͔��~������Ȃ��I", "ERROR: Not enough stockpiled power!"));
				return FALSE;
			}
			p_ptr->magic_num1[30+i] -= amount * marisa_magic_table[spell_num].use_magic_power[i];
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			if((int)marisa_magic_power[i] < amount * marisa_magic_table[spell_num].use_magic_power[i])
			{
				msg_print(_("ERROR:���͔��~������Ȃ��I", "ERROR: Not enough stockpiled power!"));
				return FALSE;
			}
			marisa_magic_power[i] -= amount * marisa_magic_table[spell_num].use_magic_power[i];
		}
	}

	return TRUE;
}

//���͂̔��~���m�F
void marisa_check_essence(void)
{
	int i;
	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print(_("ERROR:marisa_check_essence()���������ȊO�ŌĂ΂ꂽ",
                "ERROR: marisa_check_essence() called for non-Marisa")); return ;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return;
	}

	screen_save();
	for(i=4;i<14;i++) Term_erase(17, i, 255);

	prt(_("���ݖ��͂̔��~�͂��ꂾ������B", "You have following kinds of mana prepared right now."),4,18);
	for(i=0;i<8;i++)
	{

		///mod151224 �������f�o�b�O���[�h�ȂǗp
		if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
			prt(format(_("%s�̖���:(*****)", "%s mana: (*****)"),marisa_essence_name[i]),6+i,18);
		else if(p_ptr->noscore)
			prt(format(_("%s�̖���:(%d)", "%s mana: (%d)"),marisa_essence_name[i],p_ptr->magic_num1[30+i]),6+i,18);
		else
			prt(format(_("%s�̖���:(%d)", "%s mana: (%d)"),marisa_essence_name[i],marisa_magic_power[i]),6+i,18);
	}
	inkey();
	screen_load();

}

//�������̍쐬�������@��16�܂Ŏ���

bool carry_marisa_magic(void)
{
	int spell_num;
	int amount;
	int max;
	int i;

	if(p_ptr->pclass != CLASS_MARISA)
	{msg_print(_("ERROR:carry_marisa_magic()���������ȊO�ŌĂ΂ꂽ",
                "ERROR: carry_marisa_magic() called for non-Marisa")); return FALSE;}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print(_("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B",
                    "You're investigating the black market right now. Go and search for ability cards."));
		return FALSE;
	}

	while(1)
	{
		spell_num = choose_marisa_magic(CMM_MODE_CARRY);
		if(spell_num < 0) return TRUE;
		if(spell_num == 99)//�I����ʂŃX�y�[�X�����������������@��S���Ƃɖ߂��đI�����Ȃ���
		{
			for(i=0;i<MARISA_MAX_MAGIC_KIND;i++)
			{
				p_ptr->magic_num1[i] += p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT];
				p_ptr->magic_num1[i+MARISA_HOME_CARRY_SHIFT] = 0;
			}
			continue;
		}

		p_ptr->magic_num1[spell_num] -=1 ;
		p_ptr->magic_num1[spell_num+40] +=1 ;
	}

	return TRUE;
}



/*:::����U���̈З͂��v�Z hand:�E��̎�0,����̎�1*/
int calc_weapon_dam(int hand)
{
	int total_damage = 0;
	int basedam;
	int dd,ds;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	if(hand < 0 || hand > 1)
	{
		msg_print(_("ERROR:calc_weapon_dam()��hand=0,1�ȊO�ŌĂ΂ꂽ",
                    "ERROR: hand isn't 0 or 1 in calc_weapon_dam()"));
		return 0;
	}
	if (!buki_motteruka(INVEN_RARM+hand)) return 0;
	o_ptr = &inventory[INVEN_RARM+hand];
	dd = o_ptr->dd + p_ptr->to_dd[hand];
	ds = o_ptr->ds + p_ptr->to_ds[hand];
	basedam = (dd * (ds + 1)) * 50;
	total_damage = o_ptr->to_d * 100;
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_EX_VORPAL))
	{
		basedam *= 5;
		basedam /= 3;
	}
	else if (have_flag(flgs, TR_VORPAL))
	{
		basedam *= 11;
		basedam /= 9;
	}
	total_damage += basedam;
	total_damage += p_ptr->to_d[hand] * 100;
	total_damage *= p_ptr->num_blow[hand];
	total_damage /= 100;
	return total_damage;
}


/*:::�������l�̔������玑���𓾂�*/
/*p_ptr->magic_num1[1]���҂��Ƃ��Ďg��*/
void hina_at_work(void)
{

	int gain;

	if(p_ptr->pclass != CLASS_HINA)
	{
		msg_print(_("�~�߂Ă������B�����N���邩�����������̂���Ȃ��B",
                    "You probably should refrain. Who knows what could happen."));
		return;
	}

	gain = p_ptr->magic_num1[1];

	screen_save();
	clear_bldg(4,22);

	if(gain < 100)
	{
		prt(format(_("�l�`�͂܂��S������Ă��Ȃ��B",
                    "The dolls aren't selling at all.")),10,20);
	}
	else if(gain < 1000)
	{
		prt(format(_("�l�`�͂قƂ�ǔ���Ă��Ȃ��E�E",
                    "The dolls are hardly selling...")),10,20);
	}
	else if(gain < 5000)
	{
		prt(format(_("�l�`�͏����͔��ꂽ�悤���B",
                    "The dolls are starting to get sold.")),10,20);
	}
	else if(gain < 50000)
	{
		prt(format(_("�l�`�̔���s���͏�X���B",
                    "The dolls are selling well.")),10,20);
	}
	else if(gain < 100000)
	{
		prt(format(_("�l�`���قƂ�ǔ���Ă���I",
                    "The dolls are selling very well!")),10,20);
	}
	else
	{
		gain = 100000;
		prt(format(_("�l�`���S�Ĕ���Ă����B�l���ŉ����N�����Ă���̂��낤���B",
                    "All dolls were bought out. What's going in the Human Village?")),10,20);
	}
	inkey();

	if(gain >= 100)
	{
		gain -= gain % 100;
		msg_format(_("$%d����������B", "You take $%d."),gain);
		p_ptr->au += gain;
		p_ptr->magic_num1[1] -= gain;
		building_prt_gold();
		if(gain == 100000) msg_format(_("���Ȃ��͐V���ȍ݌ɂ����o�����B",
                                        "You take out new inventory."));
		else msg_print(_("���Ȃ��͍݌ɂ��[�����B", "You restock."));

	}
	screen_load();

}



/*:::���܎��ōޗ��Ƃ��Ďg����A�C�e���𔻒�*/
static bool item_tester_hook_drug_material(object_type *o_ptr)
{
	int i;
	if(!object_is_aware(o_ptr)) return FALSE;

	for(i=0;drug_material_table[i].tval;i++)
	{
		if(o_ptr->tval == drug_material_table[i].tval && o_ptr->sval == drug_material_table[i].sval) return TRUE;
	}
	return FALSE;

}



/*:::��t�Ȃǂ������������*/
/*:::p_ptr->magic_num2[100-107]����쐬�ς݃t���O�Ƃ��Ďg����*/
void compound_drug(void)
{
	object_type forge;
	object_type *prod_o_ptr = &forge;//�����ς݃A�C�e��
	object_type *m1_o_ptr,*m2_o_ptr,*m3_o_ptr;//�ޗ�1�`3
	int item1,item2,item3; //�ޗ�1�`3�̃C���x���g�����ԍ�
	int m1,m2,m3; //�ޗ�1�`3��drug_material_table�Y��
	bool flag1,flag2,flag3; //�ޗ�1�`3��I���������ǂ����̃t���O ��������ȑ�R�̕ϐ��g�킸�z�񂩍\���̂ɂ��Đ��ڂ��Ƃ��΂����ƃX�b�L�������R�[�h�ɂȂ����̂ɂƎv���������x��
	bool flag_make_drug_ok = FALSE;
	bool flag_success;
	bool flag_exp; //��������Ƃ̂���A�C�e���̏ꍇTRUE
	bool flag_have_recipe; //�Ή������`��������

	char c;
	char o_name[MAX_NLEN];
	char m1_o_name[MAX_NLEN], m2_o_name[MAX_NLEN], m3_o_name[MAX_NLEN];
	int chance; //������

	cptr q = _("�ǂ̍ޗ��𒲍��Ɏg���܂����H", "Use which material for chemistry?");
	cptr s = _("��̍ޗ��ɂȂ肻���Ȃ��̂������Ă��Ȃ��B", "You don't have medicine materials.");
	int i;
	int elem_power[8];//�ޗ������l�̍��v
	int compound_result_num;//��������i(drug_compound_table[]�̓Y��)
	int amt; //�쐬��

	int cost;
	int total_cost;//�I�������ޗ�*���@���܎��ւ̎x�����z
	int rtv,rsv; //�ŏI�I�Ɋ���������̂�Tval��Sval


	if(!CHECK_MAKEDRUG_CLASS)
	{
		msg_print(_("���Ȃ��͒��܎����؂���Ȃ������B", "You couldn't rent a pharmacy room."));
		return;
	}
	else if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print(_("���Ȃ��̃U�b�N�͂��łɈ�t���B", "Your backpack is already full."));
		return;
	}
	screen_save();

	flag1 = FALSE;
	flag2 = FALSE;
	flag3 = FALSE;

	//�ޗ���3�܂őI�肷��
	while(1)
	{

		clear_bldg(0, 20);
		c_put_str(TERM_WHITE,format(_("�f�ނɂ�����̂�3�܂őI��ŉ�����:",
                                    "Choose up to 3 items to use as materials:")),8,12);
		//�v�����v�g�\��
		if(flag1)
		{
			object_desc(m1_o_name, m1_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format(_("a)�f��1:%s", "a)Material 1: %s"), m1_o_name),10,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format(_("a)�f��1:���I��", "a)Material 1: Not chosen")),10,10);
		}
		if(flag2)
		{
			object_desc(m2_o_name, m2_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format(_("b)�f��2:%s", "b)Material 2: %s"), m2_o_name),11,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format(_("b)�f��2:���I��", "b)Material 2: Not chosen")),11,10);
		}
		if(flag3)
		{
			object_desc(m3_o_name, m3_o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format(_("c)�f��3:%s", "c)Material 3: %s"), m3_o_name),12,10);
		}
		else
		{
			c_put_str(TERM_WHITE,format(_("c)�f��3:���I��", "c)Material 3: Not chosen")),12,10);
		}

		//�ޗ��͓�K�v
		if(flag1 && flag2 || flag1 && flag3 || flag2 && flag3)
		{
			if(p_ptr->pclass == CLASS_EIRIN && p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,format(_("d)���̍ޗ����������(�{�ݎg�p��:����)",
                                            "d)Make medicine with those materials (Fee: Free)")),14,10);
			else
				c_put_str(TERM_WHITE,format(_("d)���̍ޗ����������(�{�ݎg�p��:���$%d)",
                                            "d)Make medicine with those materials (Fee: $%d each)"),cost),14,10);
		}

		c = inkey();

		if(c == ESCAPE)
		{
			break;
		}
		else if(c == 'a' || c == 'A' || c == 'b' || c == 'B' || c == 'c' || c == 'C')
		{
			int item_tmp;

			item_tester_hook = item_tester_hook_drug_material;
			if (get_item(&item_tmp, q, s, (USE_INVEN)))
			{
				if(c == 'a' || c == 'A')
				{
					item1 = item_tmp;
					m1_o_ptr = &inventory[item1];
					flag1 = TRUE;
					//�I�������A�C�e�����ޗ��e�[�u���̉��Ԗڂɗ��邩�����Ă���
					for(m1=0;drug_material_table[m1].tval != m1_o_ptr->tval || drug_material_table[m1].sval != m1_o_ptr->sval;m1++);
				}
				else if(c == 'b' || c == 'B')
				{
					item2 = item_tmp;
					m2_o_ptr = &inventory[item2];
					flag2 = TRUE;
					for(m2=0;drug_material_table[m2].tval != m2_o_ptr->tval || drug_material_table[m2].sval != m2_o_ptr->sval;m2++);
				}
				else
				{
					item3 = item_tmp;
					m3_o_ptr = &inventory[item3];
					flag3 = TRUE;
					for(m3=0;drug_material_table[m3].tval != m3_o_ptr->tval || drug_material_table[m3].sval != m3_o_ptr->sval;m3++);
				}

			}
			else
			{
				if(c == 'a' || c == 'A') flag1 = FALSE;
				else if(c == 'b' || c == 'B') flag2 = FALSE;
				else flag3 = FALSE;
			}
			//��������p�v�Z
			cost = 0;
			if(flag1) cost += drug_material_table[m1].cost;
			if(flag2) cost += drug_material_table[m2].cost;
			if(flag3) cost += drug_material_table[m3].cost;

		}
		else if((c == 'd' || c == 'D') && (flag1 && flag2 || flag1 && flag3 || flag2 && flag3))
		{

			int max = 99;
			//�ő�쐬�\�����v�Z����
			if(flag1) max = MIN(max,m1_o_ptr->number);
			if(flag2) max = MIN(max,m2_o_ptr->number);
			if(flag3) max = MIN(max,m3_o_ptr->number);
			//�����ޗ����ȏ�g�������쐬�\�������̕����炷
			if(flag1 && flag2 && flag3 && item1==item2 && item2==item3) max /= 3;
			else if(flag1 && flag2 && item1==item2) max = MIN(max,m1_o_ptr->number/2);
			else if(flag1 && flag3 && item1==item3) max = MIN(max,m1_o_ptr->number/2);
			else if(flag2 && flag3 && item2==item3) max = MIN(max,m2_o_ptr->number/2);

			if(!max)
			{
				msg_print(_("�ޗ��̐�������Ȃ��B", "You don't have enough materials."));
				inkey();
				continue;
			}
			else if(max > 1)
			{
				amt = get_quantity(format(_("�������܂����H(1-%d)", "Make how many? (1-%d)"),max),max);
				if(!amt) continue;
			}
			else amt = 1;



			total_cost = cost * amt;

			//�i�Ԃ͉i�����̎g�p������
			if(p_ptr->pclass == CLASS_EIRIN && p_ptr->town_num == TOWN_EIENTEI) total_cost = 0;

			if(p_ptr->au < total_cost)
			{
				msg_print(_("���������{�ݎg�p���ɑ���Ȃ��悤���B",
                            "You don't have enough money to pay the fee."));
				inkey();
				continue;
			}

			//�����l�v�Z
//���̒܂͑����l��{�ɂ���H

			for(i=0;i<8;i++) elem_power[i] = 0;
			if(flag1) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m1].elem[i];
			if(flag2) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m2].elem[i];
			if(flag3) for(i=0;i<8;i++) elem_power[i] += drug_material_table[m3].elem[i];

			compound_result_num = 0;

			//�������ʃe�[�u���𑖍�����������A�C�e��������B�����𖞂����Ă���Ή��̂��̂قǗD�悳���
			for(i=1;drug_compound_table[i].tval;i++)
			{
				bool flag_ok1 = TRUE;
				bool flag_ok2 = TRUE;
				int j;

				if(i >= 63)//�ꉞ�I�[�o�[�t���[�h�~
				{
					msg_print(_("ERROR:drug_compound_table[]�̓o�^���������č쐬�ς݃t���O�r�b�g�ɑ���Ȃ�!",
                                "ERROR: too many entries in drug_compound_table[]; not enough flag bits!"));
					return;
				}

				//�����l��������
				for(j=0;j<8;j++)
				{
					if(drug_compound_table[i].elem[j] < 0 && elem_power[j] > drug_compound_table[i].elem[j]) flag_ok1 = FALSE;
					if(drug_compound_table[i].elem[j] > 0 && elem_power[j] < drug_compound_table[i].elem[j]) flag_ok1 = FALSE;
				}
				//�K�v����f�ޔ���
				if(drug_compound_table[i].special_material_tval)
				{
					flag_ok2 = FALSE;

					if(flag1 && m1_o_ptr->tval == drug_compound_table[i].special_material_tval
						&& m1_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;
					if(flag2 && m2_o_ptr->tval == drug_compound_table[i].special_material_tval
						&& m2_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;
					if(flag3 && m3_o_ptr->tval == drug_compound_table[i].special_material_tval
						&& m3_o_ptr->sval == drug_compound_table[i].special_material_sval) flag_ok2 = TRUE;

				}

				if(!flag_ok1 || !flag_ok2) continue;

				compound_result_num = i;

			}
			//���łɍ�������Ƃ̂���i�̃t���O�𓾂�
			flag_exp = ((p_ptr->magic_num2[100+compound_result_num/8] >> compound_result_num % 8) & 1L);
			//���V�s�����邩�`�F�b�N����
			//�i�Ԃ͑S�Ẵ��V�s��m���Ă���
			if(p_ptr->pclass == CLASS_EIRIN)
			{
				flag_have_recipe = TRUE;
				flag_exp = TRUE;
			}
			else
			{
				flag_have_recipe = FALSE;
				for(i=0;i<32;i++)
				{
					if(drug_recipe_table[i].tval == drug_compound_table[compound_result_num].tval
						&& drug_recipe_table[i].sval == drug_compound_table[compound_result_num].sval)
						break;
				}
				i /= 8;
				if(i < 4) //���̃��[�v��i��32�ɂȂ�ƃ��V�s�ɂȂ������i�Ȃ̂Ńp�X
				{
					int j;
					for(j=0;j<INVEN_PACK;j++)
					{
						if(!inventory[j].k_idx) break;
						if(inventory[j].tval == TV_BOOK_MEDICINE && inventory[j].sval == i)
						{
							flag_have_recipe = TRUE;
						}
					}
				}

			}

			//���������v�Z����B���V�s�L��A�쐬�ς݁A�쐬�ꏊ�ɂ���x�ω�
			chance = 60 + p_ptr->lev * 2 + (p_ptr->stat_ind[A_INT]+3) * 2 - drug_compound_table[compound_result_num].difficulty ;
			if(flag_have_recipe) chance += 20;
			if(flag_exp) chance += 10;
			if(p_ptr->town_num == TOWN_EIENTEI) chance += 10;//�i�������܎��ł͐������₷��
			if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))	chance -= 20; //v1.1.44 ���ǂ񂰂����܂ł��邪�������͒Ⴂ
			if(chance < 5) chance = 5;

			object_prep(prod_o_ptr, lookup_kind(drug_compound_table[compound_result_num].tval,drug_compound_table[compound_result_num].sval ));
			apply_magic(prod_o_ptr,p_ptr->lev,0L);//�v���Ǝv�����ꉞ
			prod_o_ptr->number = 1;


			object_desc(o_name, prod_o_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));

			if(flag_have_recipe || flag_exp)
				c_put_str(TERM_WHITE,format(_("�����i:%s", "Finished product: %s"), o_name),16,10);
			else
				c_put_str(TERM_WHITE,format(_("�����i:????", "Finished product: ????")),16,10);
			c_put_str(TERM_WHITE,format(_("������:%d%%", "Success rate: %d%%"), MIN(chance,100)),17,10);

			if(!flag_have_recipe && !flag_exp)
				msg_format(_("������������Ƃ̂Ȃ����̂��o���������I",
                            "You think you can make something new!"));
			else if(!compound_result_num)//�Y
				msg_format(_("����ł�%s�ɂ����Ȃ�Ȃ��B", "Only %s will come out of this."),o_name);
			else if(flag_have_recipe && flag_exp)
				msg_format(_("%s�����B", "You're going to make %s."),o_name);
			else
				msg_format(_("%s���o����͂����B", "You should manage to make %s."),o_name);

			if(!get_check_strict(_("���܂��J�n���܂����H", "Start chemistry?"), CHECK_OKAY_CANCEL)) continue;

			flag_make_drug_ok = TRUE;
			break;
		}

	}
	screen_load();
	if(!flag_make_drug_ok) return;
	msg_print(NULL);

	//�������ƍޗ��A�C�e�������炷
	p_ptr->au -= total_cost;
	building_prt_gold();

	if(flag1)
	{
		inven_item_increase(item1, -amt);
		//inven_item_describe(item1);
		//inven_item_optimize(item1);
	}
	if(flag2)
	{
		inven_item_increase(item2, -amt);
		//inven_item_describe(item2);
		//inven_item_optimize(item2);
	}
	if(flag3)
	{
		inven_item_increase(item3, -amt);
		//inven_item_describe(item3);
		//inven_item_optimize(item3);
	}
	// -Hack- optimize()��1���s���ƁA�ޗ�1���؂�ăA�C�e�������l�߂��ޗ�2�̌��������̃X���b�g�������A�Ƃ������Ƃ��N���邽�߂��Ƃɂ܂Ƃ߂�
	//optimize()���K�v�ȃA�C�e��������ł���Ə�������΂����̂ŏ����ǉ�
	for(i=0;i<INVEN_PACK;i++)
	{
		if(!inventory[i].k_idx) break;
		if(!inventory[i].number)
		{
			inven_item_optimize(i);
			i--;
		}
	}

	screen_save();
	clear_bldg(0, 20);

	//�쐬�����b�Z�[�W
	c_put_str(TERM_WHITE,_("���Ȃ��͒��܂Ɏ��|�������E�E", "You start mixing..."),8 , 6);
	(void)inkey();


	if(randint0(100) < chance)//����
	{
		rtv = drug_compound_table[compound_result_num].tval;
		rsv = drug_compound_table[compound_result_num].sval;

		c_put_str(TERM_WHITE,_("���܂ɐ��������I", "Success!"),9 , 8);

		//�쐬�ς݃t���O�𗧂Ă�
		p_ptr->magic_num2[100+compound_result_num/8] |= (1L << compound_result_num % 8);
		flag_success = TRUE;

	}
	else//���s
	{
		int cost_mod = cost / 2 + randint1(cost);

		if(cost_mod < 50)
		{
			rtv = TV_MATERIAL;
			rsv = SV_MATERIAL_COAL;
			c_put_str(TERM_WHITE,_("�ޗ����ł��Ă��܂����B", "The materials burnt up."),9 , 8);
		}
		else if(cost_mod < 100)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POISON;
			c_put_str(TERM_WHITE,_("�������ςȏL�������Ă����E�E", "There's a strange smell..."),9 , 8);
		}
		else if(cost_mod < 300)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_TSUYOSHI;
			if(one_in_(3))c_put_str(TERM_WHITE,_("�������̖򂩂�ԐF�̖A�������o�����E�E",
                                                "Red bubbles float out of the mixture..."),9 , 8);
			else if(one_in_(2))c_put_str(TERM_WHITE,_("�������̖򂩂�F�̖A�������o�����E�E",
                                                "Blue bubbles float out of the mixture..."),9 , 8);
			else c_put_str(TERM_WHITE,_("�������̖򂩂率�F�̖A�������o�����E�E",
                                        "Violet bubbles float out of the mixture..."),9 , 8);
		}
		else if(cost_mod < 600)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_NEO_TSUYOSHI;
			c_put_str(TERM_WHITE,_("�����痂����_�l�̑�炩�ŃZ�N�V�[�ȏΊ炪�������C�������E�E",
                                    "You feel like you've seen a big and sexy smile of a strong god..."),9 , 8);
		}
		else if(cost_mod < 1000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POISON2;
			if(one_in_(2))
				c_put_str(TERM_WHITE,_("�������̖򂩂�h�N���̂悤�ȍ������������������E�E",
                                        "Black smoke in shape of a skull floats out of the mixture..."),9 , 8);
			else
				c_put_str(TERM_WHITE,_("�����̂�����������l�������o���Ĉ�Ăɓ����o�����B",
                                        "Several rats crawl out of holes and run away out of the room."),9 , 8);
		}

		else if(cost_mod < 1200)
		{
			rtv = TV_COMPOUND;
			rsv = SV_COMPOUND_NECOMIMI;
			c_put_str(TERM_WHITE,_("�˔@�t���X�R�����������I", "The flask suddenly explodes!"),9 , 8);
		}

		else if(cost_mod < 5000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_POLYMORPH;
			if(one_in_(3))c_put_str(TERM_WHITE,_("�������n���ĕ��ꂽ�E�E", "Your mortar melts and crumbles..."),9 , 8);
			else if(one_in_(2))c_put_str(TERM_WHITE,_("�����킪���F�̉t��f���o�����I",
                                                        "Iridescent liquid flows out of the distiller!"),9 , 8);
			else c_put_str(TERM_WHITE,_("�劘���s���R�ɔP���ꂽ�E�E",
                                        "The cauldron twists in an unnatural way..."),9 , 8);
		}
		else if(cost_mod < 10000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_RUINATION;
			if(one_in_(10))
				c_put_str(TERM_WHITE,_("�󂩂�~�X�e�B�A�������Ă����B", "Mystia falls out of the sky."),9 , 8);
			else if(one_in_(2))
				c_put_str(TERM_WHITE,_("�󂩂璹�������Ă����B", "A bird falls out of the sky."),9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,_("���̊O�̒|�т���ĂɌ͂ꂽ�B", "Bamboo outside the window suddenly withers."),9 , 8);
			else
				c_put_str(TERM_WHITE,_("���̊O�̖؁X����ĂɌ͂ꂽ�B", "Trees outside the window suddenly wither."),9 , 8);
		}

		else if(cost_mod < 30000)
		{
			rtv = TV_COMPOUND;
			rsv = SV_COMPOUND_MAGICAL_ECO_BOMB;
			if(one_in_(2))
				c_put_str(TERM_WHITE,_("���܎��̉�����������񂾁I", "The ceiling of the room gets blasted away!"),9 , 8);
			else if(EXTRA_MODE)
				c_put_str(TERM_WHITE,_("�������_���W������h�邪�����I", "The dungeon shakes from the explosion!"),9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
				c_put_str(TERM_WHITE,_("�������|�т�h�邪�����I", "The bamboo forest shakes from the explosion!"),9 , 8);
			else
				c_put_str(TERM_WHITE,_("�������X��h�邪�����I", "The town shakes from the explosion!"),9 , 8);
		}

		else if(cost_mod < 50000)
		{
			rtv = TV_POTION;
			rsv = SV_POTION_DEATH;
			if(EXTRA_MODE)
				c_put_str(TERM_WHITE,_("�}�Ƀ_���W�������Â����ɕ�܂ꂽ�C�������B",
                                        "The dungeon feels extremely tranquil all of the sudden."),9 , 8);
			else if(one_in_(2))
				c_put_str(TERM_WHITE,_("�󂩂疅�g�������Ă����B", "Mokou falls out of the sky."),9 , 8);
			else if(p_ptr->town_num == TOWN_EIENTEI)
			{
				if(p_ptr->pclass == CLASS_EIRIN)
					c_put_str(TERM_WHITE,_("�E�h���Q��������ς��Ĕ��ł����I", "Udonge rushes in!"),9 , 8);
				else
					c_put_str(TERM_WHITE,_("�i�Ԃ�������ς��Ĕ��ł����I", "Eirin rushes in!"),9 , 8);
			}
			else
				c_put_str(TERM_WHITE,_("�얲���u�܂��򉮂��I�v�ƌ����Ȃ�������R�j���Ă����I",
                                        "Reimu kicks the door open! 'It's those chemists again!'"),9 , 8);
		}
		else
		{
			rtv = TV_MATERIAL;
			rsv = SV_MATERIAL_MYSTERIUM;
			if(one_in_(2))
				c_put_str(TERM_WHITE,_("���܎��̕i�X���u���ɋɍʐF�ɐ��܂����I",
                                        "The items in the pharmacy room suddenly become garishly colored!"),9 , 8);
			else
				c_put_str(TERM_WHITE,_("���܎��̕i�X���u���b�N�z�[���ɋz�����܂ꂽ�I",
                                        "The items in the pharmacy room get sucked into a black hole!"),9 , 8);
		}

		c_put_str(TERM_WHITE,_("���܂Ɏ��s�����E�E", "Chemistry failed..."),10 , 8);
		flag_success = FALSE;

	}

	object_prep(prod_o_ptr, lookup_kind(rtv,rsv));
	apply_magic(prod_o_ptr,p_ptr->lev,0L);//�v���Ǝv�����ꉞ
	if(flag_success && flag_exp && drug_compound_table[compound_result_num].max_num > 1
		&& cost > drug_compound_table[compound_result_num].base_cost) //���쐬�����A��x�ƍޗ��i���ɉ����Đ�������������
	{
		int add=0;
		int max_num = drug_compound_table[compound_result_num].max_num;
		int mult = cost * (chance+100) / 2 / drug_compound_table[compound_result_num].base_cost;

		add = randint1(amt * mult)/100;
		if(add > (max_num-1) * amt) add = (max_num-1) * amt;
		amt += add;
		if(p_ptr->wizard) msg_format("cost:%d base_cost:%d max_num:%d mult:%d add:%d",cost,drug_compound_table[compound_result_num].base_cost,max_num,mult,add);
	}
	prod_o_ptr->number = amt;
	if(prod_o_ptr->number > 99) prod_o_ptr->number = 99;

	if(flag_success && flag_have_recipe) //���V�s�ʂ�ɍ�����ꍇ*�Ӓ�*�ς݂ɂ���
	{
		object_aware(prod_o_ptr);
		object_known(prod_o_ptr);
		prod_o_ptr->ident |= (IDENT_MENTAL);
	}
	object_desc(o_name, prod_o_ptr, 0L);

	inven_carry(prod_o_ptr);
	msg_format(_("%s����肵���B", "You have obtained %s."),o_name);

	screen_load();
}


//���P�ɑ΂���b��˗�
bool kogasa_smith(void)
{
	int     item,amt;
	cptr    q, s;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int	price;
	int new_val = p_ptr->lev/5 + 5;
	char	o_name[MAX_NLEN];

	if(p_ptr->pclass == CLASS_KOGASA)
	{msg_print(_("ERROR:���P�Ńv���C����kogasa_smith()���Ă΂ꂽ",
                "ERROR: kogasa_smith() was called while playing as Kogasa")); return FALSE;}


	//���ɗa���Ă�����̂�����ꍇ���Ԃ��`�F�b�N�������o���Ă�Γn��
	if(inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH].k_idx)
	{
		int time_div;
		if(EXTRA_MODE) time_div = 8;
		else time_div = 1;

		// -Hack- ����_�Е��퉮�́u�Ō�ɖK�ꂽ�^�[���v�̒l������J�E���g�Ɏg���Ă���
		if((town[TOWN_HAKUREI].store[STORE_WEAPON].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
		{
			if(EXTRA_MODE)
				msg_print(_("���P�͍�ƒ����B", "Kogasa is busy."));
			else
				msg_print(_("���P�͂��Ȃ��B", "Kogasa isn't here."));

			return FALSE;
		}
		else if(inventory[INVEN_PACK-1].k_idx)
		{
			msg_print(_("�u�o�b�`���o���Ă��܂���I�ł��ו����󂯂ė��Ă��������I�v",
                        "'It came out perfectly! But please free up some space in your inventory!'"));
			return FALSE;
		}
		else
		{
			object_copy(q_ptr, &inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH]);
			object_wipe(&inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH]);
			//���ł�*�Ӓ�*����
			identify_item(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			object_desc(o_name, q_ptr, (OD_OMIT_INSCRIPTION));

			switch(p_ptr->prace)
			{
			case RACE_HUMAN:
			case RACE_SENNIN:
			case RACE_TENNIN:
			case RACE_DEMIGOD:
			case RACE_JK:
				msg_print(_("�u���̗d���ɂ͓����ł���H�v", "'Please don't tell other youkai!"));
				break;
			default:
				msg_print(_("�u�o�b�`���o���Ă��܂���I�v", "'It came out perfectly!'"));
				break;
			}

			msg_format(_("%s���󂯎�����B", "You receive %s."),o_name);

			inven_carry(q_ptr);

		}


	}
	//���P�ɋ��𕥂��ĕi����a����
	else
	{


		if(one_in_(4)) 		q = _("�u�����̎d�����܂���v", "'The results of my work will surprise you.'");
		else if(one_in_(3))	q = _("�u���񎄂ɔC���Ă���܂��񂩁H�v", "'Will you leave it to me?'");
		else if(one_in_(2))	q = _("�u�����Ŏ��̏o�Ԃł���[�I�v", "'That's where I come in!'");
		else			q = _("�u���͒b������ӂȂ�ł���v", "'I'm also good at blacksmithing.'");
		s = _("�b��𗊂߂�悤�ȋ����������Ă��Ȃ��B", "You don't have any suitable items.");

		item_tester_hook = object_is_metal;
		if (!get_item(&item, q, s, (USE_INVEN))) return FALSE;

		if (item >= 0)	o_ptr = &inventory[item];
		else {msg_print(_("ERROR:kogasa_smith()��item���}�C�i�X", "ERROR: negative item index in kogasa_smith()")); return FALSE;}


		//�y���􂢂łȂ��C���l�����E�ȏ�Ȃ�f����
		if((object_is_weapon_ammo(o_ptr) && o_ptr->to_h >= new_val && o_ptr->to_d >= new_val
		|| object_is_armour(o_ptr) && o_ptr->to_a >= new_val)
		 && !(object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) ))
		{
			msg_print(_("�u����͂�����Ƃ���ȏ�b�����Ȃ�����...�v",
                        "'I don't think I can refine it beyond that...'"));
			return FALSE;
		}

		if (o_ptr->number > 1)
		{
			amt = get_quantity(NULL, o_ptr->number);
			if (amt <= 0) return FALSE;
		}
		else amt = 1;

		//��p�v�Z

		screen_save();
		clear_bldg(4,22);

		object_copy(q_ptr, o_ptr);
		q_ptr->number = amt;
		object_desc(o_name, q_ptr, (OD_NAME_ONLY));


		if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
		{
			price = (new_val * new_val * 20 + 500) * amt;
			c_put_str(TERM_WHITE,format(_("%s���C��:(��p$%d)", "Repair %s: (fee $%d)"), o_name,price),10,10);
		}
		else
		{
			int base_value = object_value_real(o_ptr);
			if(base_value < 100) base_value = 100;
			if(object_is_artifact(o_ptr)) base_value += 10000;

			if(base_value > 100000) base_value = 100000;

			price = base_value / 100 * (new_val * 3 + 5);
			if(price < new_val * 100) price = new_val * 100;

			price *= amt;
			c_put_str(TERM_WHITE,format(_("%s��+%d�܂ŋ���:(��p$%d)", "Enchant %s up to +%d: (fee $%d)"), o_name,new_val,price),10,10);

		}
		if(one_in_(2))c_put_str(TERM_WHITE,_("���P�u���ӂȒb��ŉ��Ƃ�����Ă�����Ċ����H�v",
                                            "Kogasa - 'Do I look like a skilled blacksmith?'") ,9,9);
		else c_put_str(TERM_WHITE,_("���P�u����͕׋������Ē����܂��v", "Kogasa - 'It'll cover the cost of my further training.'") ,9,9);

		if(get_check_strict(_("�˗����܂����H", "Make request? "), CHECK_OKAY_CANCEL))
		{
			if(p_ptr->au < price)
			{
				msg_print(_("�u�������ǁA�����O�����ł����󂯂Ȃ����Ƃɂ����́B�v",
                            "'I'm sorry, but you have to pay in advance.'"));
			}
			else
			{
				p_ptr->au -= price;
				msg_format(_("$%d�x�����ď��P�ɃA�C�e����a�����B",
                            "You paid $%d and entrusted the item to Kogasa."),price);

				inven_item_increase(item, -amt);
				inven_item_describe(item);
				inven_item_optimize(item);
				building_prt_gold();
				kogasa_smith_aux(q_ptr);
				if(EXTRA_MODE)
					msg_print(_("�u���C������I�O���Ԍ�ɂ͏o���Ă��܂���I�v",
                                "'Leave it to me! Come back in three hours!'"));
				else
					msg_print(_("�u���C������I�����̍����ɂ͏o���Ă��܂���I�v",
                                "'Leave it to me! Come back this time tomorrow!'"));

				//q_ptr���ꎞ�C���x���g���ɒǉ�
				object_copy(&inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH], q_ptr);

				// -Hack- ���퉮�́u�Ō�ɖK�ꂽ�^�[���v�̒l������J�E���g�Ɏg��
				town[TOWN_HAKUREI].store[STORE_WEAPON].last_visit = turn;
			}

		}
		screen_load();

	}
	return TRUE;

}



/*:::Ex�_���W���������̔ėp���[�`��*/
void exbldg_search_around(void)
{
	object_type forge;
	object_type *o_ptr = &forge;
	cptr msg1,msg1_2,msg2,msg3;
	char o_name[MAX_NLEN];
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];
	int srh = p_ptr->skill_srh;
	int stl = p_ptr->skill_stl;
	bool flag_max_inven = FALSE;
	int price;
	bool houtou = FALSE;


	if(inventory[INVEN_PACK-1].k_idx) flag_max_inven = TRUE;


	switch(ex_bldg_idx)
	{
	case BLDG_EX_FLAN: //�t�����̒n����
		msg1 = _("�n�������B���ȍ�肾�����������ɐ^�V�����q�r�������Ă���E�E",
                "It's a basement. Looks sturdy, but there are some cracks here and there...");
		msg1_2 = "";
		msg2 = _("�ӂ��T���Ă݂܂����H", "Search around?");
		msg3 = _("�����ɂ͂��������Ȃ��B", "Nothing here.");
		break;

	case BLDG_EX_UDONGE: //���ǂ񂰂̍s��
		msg1 = _("�_���W�����Ƀz���C�g�{�[�h���u����A�e�̖򔄂肪�e�ɗ����Ă���B",
                "A whiteboard stands there, with a rabbit selling medicine by the side.");
		msg1_2 = "";
		msg2 = _("�b�������܂����H", "Talk?");
		msg3 = _("�򔄂�͋A���Ă������B", "The medicine seller has gone home.");
		break;
	case BLDG_EX_SUWAKO: //�z�K�q
		msg1 = _("�z�K�q���~�����Ă���B", "Suwako is hibernating here.");
		msg1_2 = "";
		msg2 = _("���Ă݂܂����H", "Poke her?");
		msg3 = _("�z�K�q�͂���ɐ[���ւƐ����čs�����B", "Suwako has gone deeper into the dungeon.");
		break;
	case BLDG_EX_IKUCHI: //�L�m�R
		msg1 = _("��R�̃L�m�R���ւ̂悤�Ȍ`�ɘA�Ȃ��Đ����Ă���B",
                "Many mushrooms are growing in a big circle here.");
		msg1_2 = "";
		msg2 = _("�̎悵�Ă����܂����H", "Collect some?");
		msg3 = _("�L�m�R�̍̎�͍ς񂾁B", "You've already gathered mushrooms.");
		break;

	case BLDG_EX_CHAOS_BLADE: //���ׂ̌�
		msg1 = _("�����̓����̂悤�Ȏ����̕������B",
                "This room almost looks like it's made out of internal organs.");
		msg1_2 = _("�ЁX�����I�[�����������{������ł���E�E",
                "There's a sword floating in the air radiating a terrifying aura...");
		msg2 = _("�G��Ă݂܂����H", "Touch it?");
		msg3 = _("���������Ȃ��B", "There's nothing here.");
		break;
	case BLDG_EX_ABANDONED: //�������ꂽ������
		msg1 = _("���Ȃ�r��ʂĂĂ��邪�A���Ƃ͖��@�g���̌������������悤���B",
                "This looks like an abandoned magician's study.");
		msg1_2 = "";
		msg2 = _("�ƒT�����Ă݂܂����H", "Search the house?");
		msg3 = _("�ڂڂ������̂͑e���T���I�����B", "You don't see anything else remarkable.");
		break;
	case BLDG_EX_AKISISTERS: //�H�o���̉���
		msg1 = _("�u���B�A�Ă�����������n�߂Ă݂��́I�v",
                "'We've opened up a roast sweet potato stand!'");
		msg1_2 = "";
		msg2 = _("$100�炵���B�����܂����H", "It costs $100. Purchase?");
		msg3 = _("�����Ă����͔���؂�̂悤���B", "The roast sweet potatoes are sold out.");
		break;
	case BLDG_EX_SUKIMA: //���E�̂ق����
		msg1 = _("���E�̒]�т�����B", "There's a crack in the barrier.");
		msg1_2 = "";
		if(p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
			msg2 = _("�C�����܂����H", "Repair it?");
		else
			msg2 = _("�G��Ă݂܂����H", "Touch it?");
		msg3 = _("���E�̒]�т͏������B", "The crack in the barrier has vanished.");
		break;
	case BLDG_EX_PRINT_TENGU: //�V��̈����
		price = dun_level * 300;
		msg1 = _("�R���V�炽���͋x�e���̂悤���B", "The yamabushi tengu are on a break.");
		msg1_2 = format(_("$%d�ŃA�C�e���J�[�h�𕡐����Ă����炵���B",
                        "You can replicate an item card for $%d."),price);
		msg2 = _("�˗����܂����H", "Make a request?");
		msg3 = _("�R���V�炽���͍ĂіZ�������Ɏd�����n�߂Ă���B",
                    "The yamabushi tengu are back to being busy with work.");
		break;
	case BLDG_EX_ORC: //�I�[�N
		msg1 = _("�Ђǂ����L�̂��鏬�����E�E", "It's a small hut that gives off an awful smell...");
		msg1_2 = "";
		msg2 = _("�ƒT�����Ă݂܂����H", "Search inside?");
		msg3 = _("�ڂڂ������̂͑e���T���I�����B", "You don't see anything else remarkable.");
		break;
	case BLDG_EX_OKINA: //v1.1.32 �B���
		msg1 = _("�ł̒��ɑ�ʂ̔��������Ă����Ԃ��B�N�����Ȃ�ł���...",
                "You are in a dark space with many doors floating around. Someone stands before you...");
		if(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_MARISA || p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_CIRNO)
			msg1_2 = _("�B��ށu����A�܂������̂��B����ł������Ăق����̂��H�v",
                        "Okina - 'Oh, you again? Do you want me to give you my blessing?'");
		else if(p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO)
			msg1_2 = _("�B��ށu�C���͏����̂悤���ȁB����Ƃ��菕�����~�����̂��H�v",
                        "Okina - 'Your mission seems to be going well. Do you need some help?'");
		else if (p_ptr->pclass == CLASS_SUMIREKO)
			msg1_2 = _("�B��ށu�₠�A�O���l�B���C�ɂ��Ă��H�v",
                        "Okina - 'Hey, outsider. Have you been feeling well?'");
		else
			msg1_2 = _("�@�u�񓶎q�̓z�͉������Ă���񂾁H���ȓz����������ł������B�v",
                        "   'What are those two children doing? A stranger just wandered in.'");
		msg2 = _("���̏�ɗ��܂�܂����H", "Stay here?");
		msg3 = _("��ȋ�Ԃ͂����Ȃ��B", "The strange space is no longer here.");
		break;

	case BLDG_EX_NAZRIN: //�i�Y�[�����̌@�����ď���
		{
			int i;

			for(i=0;i<INVEN_TOTAL;i++) if(inventory[i].name1 == ART_HOUTOU) houtou = TRUE;
			if(p_ptr->pclass == CLASS_SHOU || p_ptr->pclass == CLASS_BYAKUREN) houtou = FALSE;

			if(houtou)
			{
#ifdef JP
				msg1 = "�u�c���͑��k�Ȃ񂾂��B";
				msg1_2 = "�������킸�ɂ��̕󓃂�$1,000,000�Ŕ����Ăق����B�v";
#else
                msg1 = "'...I want to buy that pagoda from you for $1,000,000.";
                msg1_2 = "Just don't tell anyone.'";
#endif
				msg2 = _("����܂����H", "Sell it?");
				msg3 = _("���łɕ󓃂���������B", "You've already handed over the pagoda.");
			}
			else
			{
				price = 100 + dun_level * 20;
				if(p_ptr->pclass == CLASS_SHOU || p_ptr->pclass == CLASS_BYAKUREN) price /= 5;
				msg1 = _("�u�₠�A���̃t���A�̒n�}������񂾂����Ă��������H�v",
                        "'Hey! I have a map of this floor, do you want to take a look?'");
				msg1_2 = "";
				msg2 = format(_("$%d�炵���B�����܂����H", "It costs $%d. Purchase?"),price);
				msg3 = _("���łɒn�}�������Ă�����Ă���B", "You've already examined the map.");
			}
		}
		break;
	case BLDG_EX_PUFFBALL: //�I�j�t�X�x
		msg1 = _("�傫�Ȕ����򂪂���B����̓��A�L�m�R�̃I�j�t�X�x���I",
                "There's a large white lump here. It's a rare mushroom, a giant puffball!");
		msg1_2 = "";
		msg2 = _("�̎悵�Ă����܂����H", "Collect?");
		msg3 = _("�L�m�R�̍̎�͍ς񂾁B", "You've already gathered mushrooms.");
		break;

	case BLDG_EX_LARVA: //�����o�̕ʑ�
		if (p_ptr->pclass == CLASS_LARVA)
		{
			msg1 = _("�����͂��Ȃ��̕ʑ����B", "This is your vacation home.");
			msg1_2 = "";
			msg2 = _("�g�������Ȃ��̂������Ă����܂����H", "Take something you could use?");
			msg3 = _("�����ł̗p���͍ς񂾁B", "You don't have anything to do here.");
		}
		else
		{
			msg1 = _("�����͗d���̏Z���̂悤���B", "Looks a like a fairy's house.");
			msg1_2 = _("�؂̗t��؂芔�̉Ƌ�݂���ꂽ�g�������������B",
                        "It's a warm, small room furnished with leaves and tree stumps.");
			msg2 = _("�ƒT�����Ă����܂����H", "Search the house?");
			msg3 = _("�����g�������Ȃ��̂͂Ȃ��B", "There's nothing else useful here.");

		}
		break;

	case BLDG_EX_KEIKI: //�쒷��
		msg1 = _("�u�V�����Z���J�����Ă���Ƃ����B�v", "'I'm developing new armor.'");
		msg1_2 = _("�u�����ꖡ������f�ނ������Ă݂����񂾂��ǁc�c�v", "'I'd like to add some new materials...'");
		msg2 = _("�f�ނ�񋟂��܂����H", "Provide materials?");
		msg3 = _("���`�_�͋C���ǂ����ɕʂ̂��̂����n�߂Ă���B", "The sculptor god has started working on a new item.");
		break;


	case BLDG_EX_ZASHIKI: //���~��炵�̃e�����[�N

		msg1 = _("�ዾ�����������~��炵���b�������Ă���...", "A glasses-wearing zashiki-warashi starts talking...");
		if (p_ptr->prace == RACE_ZASHIKIWARASHI)
		{
			if(one_in_(3))
				msg1_2 = _("�u���Ȃ�������̊ԁA�����㗝�ŋ��_��S�����܂��ˁv",
                        "'I'll be in charge of the exploration base while you're away'");
			else if (one_in_(2))
				msg1_2 = _("�u��͂莞��̓����[�g�ˁI�ł����K�Ȃ̂ɂ������d���������Ă���C������́v",
                        "'It's truly the era of remote work! However, I have the feeling amount of work has increased'");
			else
				msg1_2 = _("�u�e�����[�N���������ǂ��Ȃ��̂悤�ȃ��o�C�����[�N���y�������ˁv",
                        "'Remote work is fine, but working on the move like you seems to be fun as well'");

		}
		else
		{
			if (one_in_(3))
				msg1_2 = _("�u�������Ȃ��̒T�����_�̒S���ɂȂ�܂����B�����b���܂��B�v",
                        "'I'm in charge of taking care of your exploration base.'");
			else if (one_in_(2))
				msg1_2 = _("�u�����������Ȃ��̒T��������`�����܂��傤�v",
                        "'It might be just a bit, but I'll help you with your exploration.'");
			else
				msg1_2 = _("�u���Ȃ��̉ƂɓD�_������Ȃ��͎̂��������ƌ������Ă��邩��ł���H�v",
                        "'Do robbers avoid your house because I'm watching over it?'");

		}

		msg2 = _("���_���g���܂����H", "Use exploration base?");
		msg3 = _("�u����ł́A�T���撣���Ă��������ˁB�v", "'Good luck in your adventure.'");
		break;

	case BLDG_EX_GHOSTS: //v2.1.0 ����̗��܂��
		msg1 = _("����̌Q�ꂪ�����ł��Ȃ����������Ɍ��Ă���...", "The swarm of vengeful spirits silently watches you from afar...");
		msg1_2 = "";
		msg2 = _("���̏�ɂƂǂ܂�܂����H", "Stay here?");
		msg3 = _("���삽���͂��Ȃ��ɍ\�킸����琷��オ���Ă���...", "The vengeful spirits seem to be excited about something, ignoring you...");
		break;



		default:
		msg_print(_("ERROR:exbldg_search_around()�ɂ���idx�̃��b�Z�[�W���o�^����Ă��Ȃ�",
                    "ERROR: No message found in exbldg_search_around() for the idx"));
		return;
	}

	if(ex_buildings_param[ex_bldg_num] == 255 && !houtou) //�{���ς݃t���O
	{
		prt(msg3,5,20);
		inkey();
		clear_bldg(4,18);
		return;
	}
	clear_bldg(4, 18);

	//msg��\�����������Ȃ��Ȃ�I��
	prt(msg1,10,10);
	prt(msg1_2,11,10);
	if (!get_check_strict(msg2, CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}

	//�e������
	switch(ex_bldg_idx)
	{
	case BLDG_EX_FLAN: //�t�����̒n����
		{
			if(randint1(66) > srh || flag_max_inven)
			{
				msg_print(_("�R�C����������ɓ��ꂽ�B", "You found a coin."));
				p_ptr->au += 1;
			}
			else
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr,dun_level,MON_FLAN);//�t�����̃J�[�h����
				object_desc(o_name,o_ptr,0);
				msg_format(_("%s���������I", "You have discovered %s!"),o_name);
				inven_carry(o_ptr);
			}
		}
		break;


	case BLDG_EX_UDONGE:
		{
			int tv,sv,num;

			//�L�����Z�����ꂽ�Ƃ��ɍđI�肵�Ȃ��悤�p�����[�^��ۑ��@��𔃂�����255�ɂȂ�
			if(!ex_buildings_param[ex_bldg_num]) ex_buildings_param[ex_bldg_num] = (randint1(dun_level)+randint1(127))/24;
			switch(ex_buildings_param[ex_bldg_num]) //0-10
			{
			case 0: //�E���g���\�j�b�N����L
				tv = TV_SOUVENIR;
				sv = SV_SOUVENIR_ULTRASONIC;
				price = 980;
				num = 1;
				break;
			case 1: //�ؓ�������
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_MUSCLE_DRUG;
				price = 100;
				num = 2+dun_level / 20;
				break;
			case 2: //�K����
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_HAPPY;
				price = 150;
				num = 4+dun_level/16;
				break;
			case 3: //�ΐ�����
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_BREATH_OF_FIRE;
				price = 300;
				num = 2+dun_level/20;
				break;
			case 4: //�W�L����
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_JEKYLL;
				price = 1000;
				num = 2+dun_level/30;
				break;
			case 5: //�b����(�ϐg��Hp50%�ۏ؂���H)
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_METAMOR_BEAST;
				price = 5000;
				num = 1+dun_level/40;
				break;
			default: //*����*�̖�
				tv = TV_COMPOUND;
				sv = SV_COMPOUND_STAR_DETORNATION;
				price = 10000;
				num = 2+dun_level / 40;
				break;
			}



			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			price *= num;
			object_desc(o_name,o_ptr,0);

			clear_bldg(4, 18);
			prt(format(_("�s %s �����I $%d  �t",
                        "< %s special! $%d >"),o_name,price),6,21);
			if(one_in_(3))prt(_("�u���������܂����������ł��傤�H�v",
                                "'I'm selling it for cheap, so how about it?'"),7,22);
			else if(one_in_(2))prt(_("�u���ꂱ���i�ԗl�̎���؂肸�Ɏ����J������...�v",
                                    "'I made this without Lady Eirin's help...'"),7,22);
			else prt(_("�u���̔\�͂������Ă���Α�����Ȃ��v",
                        "'It's no big deal for someone with my skills.'"),7,22);

			if (get_check_strict(_("�����܂����H", "Purchase?"), CHECK_DEFAULT_Y))
			{
				clear_bldg(4,18);
				if(p_ptr->au < price)
				{
					msg_print(_("�u�������ǂ����ł̓c�P�͗����Ȃ��́B�v",
                                "'Sorry, but I don't give invoices.'")) ;
					return;
				}
				if(flag_max_inven)
				{
					msg_print(_("�u�ו�����������悤�ˁB�v", "'You're carrying too much.'"));
					return;
				}
				msg_format(_("%s���w�������B", "You purchased %s."),o_name);
				p_ptr->au -= price;
				inven_carry(o_ptr);


			}
			else
			{
				if(p_ptr->prace == RACE_GYOKUTO || p_ptr->prace == RACE_LUNARIAN)
					msg_print(_("�u���̂��Ƃ͉i�ԗl�ɂ͓����ɂ��Ă����ĉ�����...�v",
                                "'Please keep this a secret from Lady Eirin...'"));
				else
					msg_print(_("�u�ł��傤�ˁB�M���Ɍ��̉Ȋw�Ȃ�ė����ł���͂����Ȃ��B�v",
                                "'I guess you just can't comprehend the science of the moon.'"));
				clear_bldg(4,18);
				return;
			}

		}
		break;
	case BLDG_EX_SUWAKO:
		{
			if(one_in_(13) || flag_max_inven)
			{
				msg_print(_("�z�K�q�͂��Ȃ����ڂ���ƌ����B",
                            "Suwako glares at you."));
				gain_random_mutation(188);

			}
			else
			{
				msg_print(_("�u���[���[�N�����Ȃ��ł�[�B���ꂠ���邩��[�B�v",
                            "'Ah- Don't wake me up-. Here, take this-.'"));
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr,dun_level,MON_SUWAKO);//�z�K�q�̃J�[�h����
				object_desc(o_name,o_ptr,0);
				msg_format(_("%s���󂯎�����B", "You received %s."),o_name);
				inven_carry(o_ptr);
			}
		}
		break;
	case BLDG_EX_IKUCHI:
		{
			int tv,sv,num;
			tv = TV_MUSHROOM;
			sv = randint0(22); //POISON�`BERSERK
			num = 10+randint1(10);
			if(weird_luck()) num += 10+randint1(10);
			if(sv == SV_MUSHROOM_RESTORING) num /= 2;

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			object_desc(o_name,o_ptr,0);

			if(!flag_max_inven)
			{
				msg_format(_("%s���̎悵���I", "You picked up %s!"),o_name);
				inven_carry(o_ptr);
			}
			else
			{
				msg_format(_("�L�m�R�̗d�����ɂ܂ꂽ�B��߂Ă������B",
                            "Mushroom fairies are staring at you. Better not do it."));
			}

		}
		break;
	case BLDG_EX_PUFFBALL:
	{
		int tv, sv, num;
		tv = TV_MUSHROOM;
		sv = SV_MUSHROOM_PUFFBALL;
		num = randint1(3) + randint1(7);
		if (weird_luck()) num += 10;

		object_prep(o_ptr, (int)lookup_kind(tv, sv));
		apply_magic(o_ptr, dun_level, 0L);
		o_ptr->number = num;
		object_desc(o_name, o_ptr, 0);

		if (!flag_max_inven)
		{
			msg_format(_("%s���̎悵���I", "You picked up %s!"),o_name);
			inven_carry(o_ptr);
		}
		else
		{
			msg_format(_("�L�m�R�̗d�����ɂ܂ꂽ�B��߂Ă������B",
                            "Mushroom fairies are staring at you. Better not do it."));
		}

	}
	break;


	case BLDG_EX_CHAOS_BLADE:
		{
			if(one_in_(2) && !flag_max_inven)
			{
				object_prep(o_ptr, (int)lookup_kind(TV_SWORD, SV_WEAPON_BLADE_OF_CHAOS));
				apply_magic(o_ptr,dun_level,(AM_GOOD|AM_GREAT|AM_NO_FIXED_ART));
				msg_format(_("���͒��𕑂��A���Ȃ��̎�ɔ[�܂����B", "The sword dances from the air and settles down in your hand."));
				inven_carry(o_ptr);

			}
			else
			{

				c_put_str(TERM_RED,_("���͒��𕑂��A���Ȃ��Ɏa�肩�����Ă����I",
                                    "The sword dances through the air, trying to cut you up!"),12,12);
				hack_ex_bldg_summon_idx = MON_CHAOS_BLADE;
				hack_ex_bldg_summon_mode = (PM_NO_PET | PM_NO_ENERGY);
				inkey();
			}
		}
		break;
		//v1.1.32
	case BLDG_EX_OKINA:
		{
			bool flag_done = FALSE;

			clear_bldg(4, 18);
			prt(_("�u�ǂ����낤�A�C�ɓ������I���O�ɂ�...",
                    "'Very well, I like you! And now..."), 6, 21);
			inkey();
			while(!flag_done)
			{

				switch(randint1(6))
				{
				case 1:

					prt(_("��˂̐_�Ƃ��āA���������Ă�낤�I�v",
                        "As the god of the back door, I shall grant you protection!'"),7,21);
					remove_all_curse();
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
					set_asthma(0);
					set_food(PY_FOOD_MAX-1);
					set_blessed(5000, FALSE);
					set_resist_magic(5000, FALSE);
					set_protevil(5000, FALSE);

					flag_done = TRUE;
					break;

				case 2:
					if (dun_level > 69 && !r_info[MON_OKINA].r_akills)
						hack_ex_bldg_summon_idx = MON_OKINA;
					else if (dun_level > 39 && !r_info[MON_MAI].r_akills)
						hack_ex_bldg_summon_idx = MON_MAI;
					else if (dun_level > 39 && !r_info[MON_SATONO].r_akills)
						hack_ex_bldg_summon_idx = MON_SATONO;
					else
						break;

					c_put_str(TERM_RED, _("��V�̐_�Ƃ��āA����������Ă�낤�I�v",
                                        "As the god of hindrances, I'll present you with a trial!'"), 12, 21);

					hack_ex_bldg_summon_mode = (PM_NO_PET | PM_NO_ENERGY);
					flag_done = TRUE;
					break;
				case 3:
					{
						int stat = randint0(6);

						if (p_ptr->stat_max[stat] == p_ptr->stat_max_max[stat]) break;
						prt(_("�\�y�̐_�Ƃ��āA���ݔ\�͂������o���Ă�낤�I",
                            "As the god of Noh, I'll draw out your latent abilities!'"), 7, 21);
						do_inc_stat(stat);
						flag_done = TRUE;
						break;
					}
				case 4:
					{
						int check_num;
						int skill_index;

						//���x��1�ȏ�50�����̋Z�\�������_���ɑI��
						for (check_num = 50; check_num; check_num--)
						{
							skill_index = randint0(SKILL_EXP_MAX);
							if (p_ptr->skill_exp[skill_index] >= SKILL_LEV_TICK * 50)
								continue;
							if (p_ptr->skill_exp[skill_index] >= SKILL_LEV_TICK)
								break;
						}

						//���̋Z�\���I�΂ꂸ���[�v�I������I��
						if (!check_num)
							break;

						prt(_("�h�_�Ƃ��āA�Z�\�����߂Ă�낤�I",
                            "As the god of outcasts, I shall raise your skills!'"), 7, 21);

						//�Z�\���x����20�㏸
						p_ptr->skill_exp[skill_index] += SKILL_LEV_TICK * 20;
						if (p_ptr->skill_exp[skill_index] > SKILL_LEV_TICK * 50)
							p_ptr->skill_exp[skill_index] = SKILL_LEV_TICK * 50;

						//�E�ƓK���ɂ����E�l�𒴂���悤�ɂ��Ă݂悤
						if (p_ptr->skill_exp[skill_index] > cp_ptr->skill_aptitude[skill_index] * SKILL_LEV_TICK * 10)
							msg_format(_("�Ȃ�ƁA%s�̋Z�\�����E�𒴂��ďオ�����I",
                                        "No way! Your skill of %s has exceeded its limit!"), skill_exp_desc[skill_index]);
						else
							msg_format(_("%s�̋Z�\���オ�����I",
                                        "Your skill of %s has increased!"), skill_exp_desc[skill_index]);

						flag_done = TRUE;
						p_ptr->update |= PU_BONUS;
					}
					break;

				case 5:
					{

						prt(_("���_�Ƃ��āA�s����̈ł𕥂��Ă�낤�I",
                            "As the god of stars, I shall drive away the darkness!'"),7,21);
						mass_genocide_3(0, FALSE, TRUE);
						wiz_lite(FALSE);

						flag_done = TRUE;
						break;
					}

				case 6:
					{
						if (flag_max_inven) break;

						prt(_("���̌��z����n�������҂̈�l�Ƃ��āA�͂�݂��Ă�낤�I",
                            "As one of the sage creators of Gensoukyou, I shall lend you my power!'"), 7, 21);
						object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
						apply_magic_itemcard(o_ptr, dun_level, MON_OKINA);//�B��ނ̃J�[�h����
						object_desc(o_name, o_ptr, 0);
						msg_format(_("%s���󂯎�����B", "You have received %s."), o_name);
						inven_carry(o_ptr);
						flag_done = TRUE;

						break;
					}
					}
			}
			inkey();

		}
		break;

	case BLDG_EX_ABANDONED:
		{
			int power = randint1(srh);

			if(flag_max_inven)
			{
				msg_print(_("�ו�����t���B���Ƃɂ��悤�B", "Your pack is full. Better leave it for later."));
				return;
			}
			clear_bldg(4,18);

			prt(_("���Ȃ��͍r��ʂĂ��������𒲂׎n�߂��E�E",
                    "You look around the abandoned laboratory..."),7,20);
			inkey();

			if(power > 50)
			{
				int book_tv = 1;
				int book_sv;

				if(one_in_(2)) book_tv = randint1(MAX_MAGIC);
				else if(cp_ptr->realm_aptitude[0] == 1 && p_ptr->realm1 != TV_BOOK_HISSATSU
					 || cp_ptr->realm_aptitude[0] == 3) book_tv = p_ptr->realm1;
				else if(p_ptr->pclass == CLASS_CHEMIST) book_tv = TV_BOOK_MEDICINE;
				else if(cp_ptr->realm_aptitude[0] == 2)
				{
					int j,cnt=0;
					for(j=1;j<=MAX_MAGIC;j++)
					{
						if(cp_ptr->realm_aptitude[j] >1 && rp_ptr->realm_aptitude[j])
						{
							cnt++;
							if(one_in_(cnt)) book_tv = j;
						}
					}
				}
				else book_tv = randint1(MAX_MAGIC);
				book_sv = (MIN(dun_level,100)/2 + randint0(100)) / 50+1;

				object_prep(o_ptr, lookup_kind(book_tv,book_sv));
				prt(_("�O�̏Z�l�̎c�������@�����������I",
                        "You found a spellbook left behind by the previous owner!"),8,21);
				inven_carry(o_ptr);

			}
			else if(power > 10 || dun_level < 30)
			{
				int cnt;

				for(cnt=10000;cnt>0;cnt--)
				{
					int value;
					object_wipe(o_ptr);
					make_object(o_ptr, 0L);
					value = object_value_real(o_ptr);
					if((o_ptr->tval == TV_POTION || o_ptr->tval == TV_SCROLL || o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND || o_ptr->tval == TV_ROD )
					&& value > (srh+dun_level)*(p_ptr->lev / 5 + 2) ) break;
				}
				if(!cnt)
				{
					msg_print(_("WARNING:10000�񐶐����Ă��g�������ȓ���o�Ȃ�����",
                                "WARNING: Usable device not found after 10000 attempts"));
					return;
				}
				prt(_("�g�������ȓ���c���Ă���̂��������B",
                        "You found an item that might be of use."),8,21);
				prt(_("�E�E�����Ɍ����Ă���C���������A�C�̂������낤�B",
                        "...You feel as if someone is watching you, but that's probably your imagination."),9,21);
				object_desc(o_name,o_ptr,0);
				msg_format(_("%s����肵���B", "You got %s."),o_name);
				inven_carry(o_ptr);

			}
			else
			{
				c_put_str(TERM_RED,_("�����A���ɁA���ɁI", "Aaah, in the window, in the window!"),12,20);

				hack_ex_bldg_summon_mode = (PM_ALLOW_UNIQUE | PM_NO_PET | PM_NO_ENERGY);
				hack_ex_bldg_summon_type = SUMMON_INSANITY;
			}
			inkey();

		}
		break;

		case BLDG_EX_AKISISTERS:
		{
			int tv,sv,num;

			tv = TV_SWEETS;
			sv = SV_SWEET_POTATO;
			price = 100;
			num = 1+randint1(2);

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr,dun_level,0L);
			o_ptr->number = num;
			object_desc(o_name,o_ptr,0);

			clear_bldg(4, 18);

			if(flag_max_inven)
			{
				msg_print(_("�u����A�ו�����������悤�ˁB�v", "'Oh, looks like you're carrying too much.'"));
				return;
			}
			if(p_ptr->au < price)
			{
				msg_print(_("�u�������󂩂��Ă�l�����̂Ă���_�̖����p���I�������玝���Ă����Ȃ����I�v",
                            "'We can't call ourselves gods if we ignore a hungry person! Very well, take it with you!'"));
				//v1.1.47 ���������}�C�i�X�̂Ƃ��Ɏ؋�0�ɂȂ�Ȃ��悤�����ǉ�
				if(p_ptr->au > 0)	p_ptr->au = 0;
			}
			else
			{
				msg_print(_("�u������ƃI�}�P�����Ă������B�v",
                            "'Here's a little extra.'"));
				msg_format(_("%s���w�������B", "You purchased %s."),o_name);
				p_ptr->au -= price;
			}
			inven_carry(o_ptr);
		}
		break;

		case BLDG_EX_SUKIMA:
		{
			int i;
			int tester;
			for(i=100;i>0;i--)
			{
				tester = randint1(p_ptr->skill_dis);//�E�ɂ�邪30-100���炢
				if(tester > 25)
				{
					msg_print(_("�X�L�}�̒��ɒT�����_�̌��i��������B��x�������_�𗘗p�ł��������I",
                                "You see your exploration base in the gap. Looks like you can use it (though one time only)!"));
					inkey();
					hack_flag_access_home = TRUE;
					do_cmd_store();
					hack_flag_access_home = FALSE;
					//��������ʂ̌����ɓ������̂Ńt���O�Đݒ�ƍĕ`��
					character_icky = TRUE;
					show_building(&building[ex_bldg_num]);
				}
				else if(tester > 10 || p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
				{
					if(flag_max_inven) continue; //�A�C�e���������ς���������Ĕ���

					msg_print(_("�������X�L�}���炱�ڂꗎ�����B", "Something falls out of the gap."));
					object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
					apply_magic_itemcard(o_ptr,dun_level,MON_YUKARI);//���̃J�[�h����
					object_desc(o_name,o_ptr,0);
					msg_format(_("%s���������I", "You have found %s!"),o_name);
					inven_carry(o_ptr);

				}
				else
				{
					msg_print(_("���Ȃ��̓X�L�}�ɓۂݍ��܂ꂽ�I", "You get sucked inside the gap!"));
					gain_random_mutation(0);
				}
				break;
			}
		}
		break;
		case BLDG_EX_PRINT_TENGU:
		{
			int item;
			cptr q, s;
			if(p_ptr->au < price)
			{
				msg_print(_("����������Ȃ��B", "You don't have enough money."));
				return;
			}

			q = _("�ǂ�𕡐����Ă��炢�܂����H", "Replicate which item?");
			s = _("�A�C�e���J�[�h�������Ă��Ȃ��B", "You don't have item cards.");

			item_tester_tval = TV_ITEMCARD;
			if (!get_item(&item, q, s, (USE_INVEN))) return ;

			if(inventory[item].number >= 99)
			{
				msg_print(_("����͂�����������Ȃ��قǂ���B", "You can't hold any more."));
				return;
			}
			o_ptr = &inventory[item];
			o_ptr->number += support_item_list[inventory[item].pval].prod_num;
			if(o_ptr->number > 99) o_ptr->number = 99;
			object_desc(o_name,o_ptr,OD_OMIT_PREFIX);
			msg_format(_("%s�𕡐����Ă�������I", "You got %s replicated!"),o_name);
			p_ptr->au -= price;

		}
		break;
		case BLDG_EX_ORC:
		{
			int power = randint1(stl);

			if(flag_max_inven)
			{
				msg_print(_("�ו�����t���B���Ƃɂ��悤�B", "Your pack is full. Better leave it for later."));
				return;
			}
			clear_bldg(4,18);

			prt(_("���Ȃ��͈��L�Y�������𒲂׎n�߂��E�E", "You start looking around the foul-smelling hut..."),7,20);
			inkey();

			if(power < randint1(3))
			{
				c_put_str(TERM_RED,_("�����̓I�[�N�̊�n�������B�߂��Ă����I�[�N�B�Ɉ͂܂�Ă���I",
                                    "That was an orc camp. The orcs have returned and surrounded you!"),12,20);

				hack_ex_bldg_summon_mode = (PM_ALLOW_UNIQUE | PM_ALLOW_GROUP | PM_NO_PET | PM_NO_ENERGY);
				if(dun_level < 15) hack_ex_bldg_summon_idx = MON_ORC_CAPTAIN;
				else hack_ex_bldg_summon_type = SUMMON_ORCS;
			}
			else if(weird_luck())
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ARMOR, SV_ARMOR_MITHRIL_CHAIN));
				apply_magic(o_ptr,dun_level,AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
				object_desc(o_name,o_ptr,0);
				prt(_("�����ȑ���̍���q���������I",
                    "You discover a chainmail of excellent craftsmanship!"),8,21);
				inven_carry(o_ptr);
			}
			else if(one_in_(2))
			{
				object_prep(o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_VENISON));
				o_ptr->number = 10+randint1(10);
				prt(_("������ƏL���H�����������B", "You found some smelly food."),8,21);
				inven_carry(o_ptr);

			}
			else
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ALCOHOL, SV_ALCOHOL_ORC));
				o_ptr->number = 2+randint1(3);
				prt(_("�����̂������̂悤�ȉt�̂��������B", "You found some strong-smelling liquid that looks alcoholic."),8,21);
				inven_carry(o_ptr);
			}
			inkey();

		}
		break;

		case BLDG_EX_NAZRIN:
		{

			clear_bldg(4, 18);

			if(houtou)
			{
				int item;
				for(item=0;item<INVEN_TOTAL;item++) if(inventory[item].name1 == ART_HOUTOU) break;

				msg_print(_("�󓃂���������B", "You handed over the pagoda."));
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
				clear_bldg(4,18);
				p_ptr->update |= (PU_BONUS);
				p_ptr->window |= (PW_INVEN | PW_EQUIP);

				p_ptr->au += 1000000;
				building_prt_gold();


				return;//�������p�\�t���O�����ĂȂ�
			}
			else
			{
				if(p_ptr->au < price)
				{
					msg_print(_("�u�������������B������������������Ȃ�łˁB�v",
                                "'You don't have money? I'm sorry, but this is a business.'"));
					return;
				}
				else
				{
					msg_format(_("�n�}�������Ă�������B", "You take a look at the map."),o_name);
					wiz_lite(FALSE);
					p_ptr->au -= price;
				}
			}
		}
		break;
		case BLDG_EX_LARVA:
		{
			int power = randint1(4);

			if (p_ptr->pclass == CLASS_LARVA) power += randint1(2);

			if (flag_max_inven)
			{
				msg_print(_("�ו�����t���B���Ƃɂ��悤�B", "Your pack is full. Better leave it for later."));
				return;
			}
			clear_bldg(4, 18);

			prt(_("���Ȃ��͕����𒲂׎n�߂��E�E", "You start searching the room..."), 7, 20);
			inkey();

			switch (power)
			{
			case 1:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_VENISON));
				o_ptr->number = 3 + randint1(3);
				prt(_("�H�����������B", "You found some food."), 8, 21);
				inven_carry(o_ptr);
				break;
			}
			case 2:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_MUSHROOM, SV_MUSHROOM_MANA));
				o_ptr->number = 3 + randint1(3);
				prt(_("�����������ȃL�m�R���������B", "You found a delicious-looking mushroom."), 8, 21);
				inven_carry(o_ptr);
				break;
			}
			case 3:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
				apply_magic_itemcard(o_ptr, dun_level, MON_LARVA);
				object_desc(o_name, o_ptr, 0);
				msg_format(_("%s���������B", "You found %s."), o_name);
				inven_carry(o_ptr);
				break;
			}
			default:
			{
				object_prep(o_ptr, (int)lookup_kind(TV_LITE, SV_LITE_FEANOR));
				apply_magic(o_ptr, dun_level, AM_NO_FIXED_ART | AM_GOOD);
				prt(_("���@�̃����^�����������B", "You found a magic lantern."), 8, 21);
				inven_carry(o_ptr);
				break;
			}
			}
			inkey();

		}
		break;

		case BLDG_EX_KEIKI:
		{
			int tv, sv, num, i , power;
			int item;
			int am_mode;
			cptr q, s;
			object_type *tmp_o_ptr;

			tv = TV_ARMOR;
			sv = SV_ARMOR_FINE_CERAMIC;

			clear_bldg(4, 18);

			if (flag_max_inven)
			{
				msg_print(_("�u�ו�����������悤�˂��B�v", "'You're carrying too much.'"));
				return;
			}

			q = _("�����f�ނ�񋟂��܂����H", "Offer which material?");
			s = _("���x�悢�f�ނ������Ă��Ȃ��B", "You don't have required items.");

			//�Ƃ肠��������C���Ɠ����f�ނ��g���B����������͑I���ł��Ȃ�
			item_tester_hook = item_tester_hook_repair_material;
			if (!get_item(&item, q, s, (USE_INVEN))) return;
			tmp_o_ptr = &inventory[item];

			//����C���f�ރe�[�u��������SV����p�p���[���J���p���[�ɓ]�p
			power = -1;
			for (i = 0; repair_weapon_power_table[i].tval; i++)
				if (repair_weapon_power_table[i].tval == tmp_o_ptr->tval && repair_weapon_power_table[i].sval == tmp_o_ptr->sval)
					power = repair_weapon_power_table[i].type;
			if (power == -1) { msg_format(_("ERROR:����C���ł��̏C���f�ނ�sv_power���o�^����Ă��Ȃ�",
                                            "ERROR: sv_power for this repair material not listed in weapon repair")); return ; }

			//�f�ނ����炷
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);

			//�t�@�C���Z���~�b�N�X�̊Z�̃p�����[�^����
			//�f�ނ���i�����Ǝ���₷���悤�ɂ��Ă݂�(apply_magic()�̏��������̓s�����Ɏ����킯�ł͂Ȃ��炵�������܂茩�ĂȂ�)
			am_mode = AM_GOOD;
			if(randint0(power)<3)am_mode = AM_CURSED;

			object_prep(o_ptr, (int)lookup_kind(tv, sv));
			apply_magic(o_ptr, dun_level, am_mode);
			o_ptr->number = 1;

			//power/4���ʑϐ��t�^�@�m���őS�ϐ�
			for (i=4; i<power; i += 4)
			{
				if (weird_luck())
				{
					add_flag(o_ptr->art_flags, TR_RES_ACID);
					add_flag(o_ptr->art_flags, TR_RES_ELEC);
					add_flag(o_ptr->art_flags, TR_RES_FIRE);
					add_flag(o_ptr->art_flags, TR_RES_COLD);
					if(one_in_(3)) 		add_flag(o_ptr->art_flags, TR_RES_POIS);

				}
				else if (one_in_(4))
				{
					one_ele_resistance(o_ptr);
				}
				else
				{
					one_high_resistance(o_ptr);
				}
			}
			//�b��i�����ɂ���
			o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER;

			clear_bldg(4, 18);

			if(object_is_cursed(o_ptr))
				prt("Oh!", 10, 25);
			else
				prt("Create!", 10, 25);

			inkey();

			inven_carry(o_ptr);

		}
		break;

		case BLDG_EX_ZASHIKI://���~��炵�@���_���p
		{
			hack_flag_access_home = TRUE;
			do_cmd_store();
			hack_flag_access_home = FALSE;
			//��������ʂ̌����ɓ������̂Ńt���O�Đݒ�ƍĕ`��
			character_icky = TRUE;
			show_building(&building[ex_bldg_num]);
			break;
		}


		case BLDG_EX_GHOSTS: //v2.1.0 ����̗��܂��
		{

			clear_bldg(4, 18);
			prt(_("���삽�������Ȃ��ɋߊ���Ă���...", "The vengeful spirits approach you..."), 7, 20);
			inkey();

			if (p_ptr->chp <= p_ptr->mhp / 2)
			{
				prt(_("���삽���͂��Ȃ��Ɍ����č��炩�Ƀ��b�p�𐁂��炵���I",
                    "The vengeful spirits loudly blow trumpets at you!"), 12, 20);
				hp_player(500);
				set_cut(0);
				set_stun(0);
				gain_random_mutation(218);
			}
			else
			{
				c_put_str(TERM_RED, _("���삽���͂��Ȃ��ɎE�����ĝ��݂�����ɂ����I",
                    "The vengeful spirits assault you!"), 12, 20);

				hack_ex_bldg_summon_mode = (PM_ALLOW_GROUP | PM_NO_PET);
				if (randint1(dun_level) < 25) hack_ex_bldg_summon_idx = MON_ONRYOU;
				else
				{
					hack_ex_bldg_summon_idx = MON_G_ONRYOU;
					hack_ex_bldg_summon_num = 2 + dun_level / 16;
				}

			}
			inkey();

		}
		break;

		default:
		msg_print(_("ERROR:exbldg_search_around()�ɂ��̌����̏������o�^����Ă��Ȃ�",
                    "ERROR: Logic for this building not listed in exbldg_search_around()"));
	}

	//�����g�p�����t���O
	ex_buildings_param[ex_bldg_num] = 255;

	p_ptr->redraw |= (PR_GOLD);
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	building_prt_gold();
	clear_bldg(4, 18);


	return;

}

/*:::Ex���[�h����B���ӂ��Ƃ���A�C�e�����^��ł����B�˗�����TRUE(��������)*/
//������ĂԑO��hack_flag_access_home_orin��؂�ւ��邱��
bool orin_deliver_item(void)
{
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];
	object_type *o_ptr = &inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER];

	//���̃t���A�Ŏ��s�ς�
	if(ex_buildings_param[ex_bldg_num] == 255)
	{
		prt(_("�Ή��L�Ӂu�����ɂ͊����̂������̂����肻�����ȁ[�B",
                "Rin Kaenbyou - 'There should be some fresh corpses around here-"),5,10);
		prt(_("��d���ς񂾂����傢�ƒT���ɍs�������B�v", "I'll go look for them after my work's finished.'"),6,10);
		inkey();
		clear_bldg(4,18);
		return FALSE;
	}
	//�˗����Ă��Ȃ�
	else if(!o_ptr->k_idx)
	{
		//hack_flag_access_home_orin��TRUE�ɂȂ��Ă���ΐ�p�̑I�����[�h�ɂȂ�
		do_cmd_store();
		//��������ʂ̌����ɓ������̂Ńt���O�Đݒ�ƍĕ`��
		character_icky = TRUE;
		show_building(&building[ex_bldg_num]);

		if(!o_ptr->k_idx) return FALSE;

		if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
			msg_print(_("�u���ꂶ��s���Ă��܂��B�ꎞ�Ԃ��炢�Ŗ߂�܂���ŁB�v",
                        "'Well, I'm off. I'll be back in around an hour.'"));
		else
			msg_print(_("�u���񂶂����Ă����B�ꎞ�Ԃ��炢�|���邩�ȁH�v",
                        "'Well, I'll be taking this. I wonder if I'll make it in an hour?'"));

		return TRUE;
	}
	else
	{
		int time_div = 24; //�ꎞ��
		//Hack - ����_�ЎG�݉��̃^�C���J�E���^���g��
		if((town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
		{
			prt(_("�N�����Ȃ�...", "There's nobody here..."),5,20);
			inkey();
			clear_bldg(4,18);
		}
		else if (!inven_carry_okay(o_ptr))
		{
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				prt(_("�Ή��L�Ӂu�����Ă��܂�����[�B�����A��������Ȃ���ł����H�v",
                    "Rin Kaenbyou - 'Come on, I brought it for you. Won't you carry it?'"),5,20);
			else
#ifdef JP
				prt("�Ή��L�Ӂu������Ƃ�����ƁA�܊p�͂�΂�^��ł������Ă̂Ɏ󂯎��Ȃ��̂����H�v",5,20);
#else
                prt("Rin Kaenbyou - 'Hey, hold on, I carried it all the way here,",5,20);
                prt("and you're not taking it off my hands?'",6,20);
#endif
			inkey();
			clear_bldg(4,18);
		}
		else
		{
			char o_name[MAX_NLEN];
			object_desc(o_name, o_ptr, 0L);
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI || p_ptr->pclass == CLASS_UTSUHO)
				prt(_("���ӂ��A�C�e���������Ė߂��Ă����B", "Orin came back with the item."),5,20);
			else
				prt(_("�ΎԂ��A�C�e���������Ė߂��Ă����B", "The kasha came back with the item."),5,20);
			inkey();
			msg_format(_("%s���󂯎�����B������ƕ��L������..", "You receive %s. It smells a bit rotten..."),o_name);
			clear_bldg(4,18);
			inven_carry(o_ptr);
			object_wipe(o_ptr);

			//�����g�p�����t���O
			ex_buildings_param[ex_bldg_num] = 255;

		}

		return FALSE;
	}

}

//1.1.51 �V�A���[�i�֘A
//���񌩂鈫�����N���A�����瓾����|�C���g��nightmare_mon_idx[]����v�Z����B�σp�����[�^���j�[�N�̃p�����[�^�v�Z���ς�ł��邱��
static	s32b	calc_nightmare_points(void)
{

	int i;
	s32b total_points = 0;

	for (i = 0; i<NIGHTMARE_DIARY_MONSTER_MAX; i++)
	{
		int lev;
		int r_idx = nightmare_mon_r_idx[i];

		if (r_idx < 0 || r_idx >= max_r_idx)
		{
			msg_format(_("ERROR:nightmare_mon_r_idx[]�ɋL�^���ꂽr_idx�l���s��(%d)",
                        "ERROR: Incorrect r_idx value (%d) recorded in nightmare_mon_r_idx[]"), r_idx);
			return 0;
		}

		lev = r_info[r_idx].level;

		total_points += (lev * lev * lev / 10);
		/*
		lev10:100
		lev20:800
		lev30:2700
		lev40:6400
		lev50:12500
		lev60:21600
		lev70:34300
		lev100:100000
		lev127:200000
		*/

	}

	return total_points;

}



/*:::�V�A���[�i(���̐��E)�p�̃����X�^�[�I��֐�*/
//�����ł�nightmare_spellname_table[]�ɃX�y�J����o�^�����L�����̂ݑI�肷�邱�Ƃɂ���B
//is_gen_unique()�Ŕ��肷��ƃX�y�J�������Ȃ������X�^�[���I�肳��Ă��܂����߁A
//�s�{�ӂȂ���n�[�h�R�[�f�B���O�Őݒ肷�邱�Ƃɂ����B
//����X�y�J�����̓G�L�����𑝂₵���炱����nightmare_spellname_table[]�ɒǋL����B
static bool monster_hook_nightmare_diary(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_idx == MON_MASTER_KAGUYA) return FALSE;//�P��(��)�͏o�Ȃ�
	if (r_idx == MON_SEIJA) return FALSE;//���ׂ͏o�Ȃ�
	if (r_idx == MON_SUMIREKO) return FALSE; //俎q(���q�ˎ҃o�[�W����)�͏o�Ȃ�

	//v1.1.98 ����͏o�Ȃ�
	if (r_idx == MON_MIZUCHI) return FALSE;

	//�����얲�Ɩ������̂Ƃ��͉σp�����[�^��MON_REIMU��MON_MARISA�������Ƃ��ďo���B
	//����ȊO�̂Ƃ��̓����_�����j�[�N1�������Ƃ��ďo���Amonster_is_you()�ɓ��Ă͂܂鎩�������X�^�[�͏o���Ȃ��B
	//�������h���~�[��俎q�V���i�̏ꍇ�͂ǂ�����o�Ȃ��B
	//�Ȃ��Amonster_is_you()�ł͉��o�̓s���㎩���ȊO�ɂ�2P�����ɂȂ郂���X�^�[������(�i�ԃv���C���̋P��Ȃ�)�̂ŁA
	//�����̃����X�^�[���������E�Ŕ����}�~�����B
	if (p_ptr->pclass != CLASS_REIMU && p_ptr->pclass != CLASS_MARISA)
	{
		if (monster_is_you(r_idx)) return FALSE;
		if (p_ptr->pclass != CLASS_DOREMY && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) && r_idx == MON_RANDOM_UNIQUE_1) return TRUE;
	}

	//俎q�͓��Z��p�̂��o��
	if (p_ptr->pclass != CLASS_SUMIREKO)
	{
		if (r_idx == MON_SUMIREKO_2) return TRUE;
	}

	if (r_idx >= MON_RUMIA && r_idx <= MON_SUWAKO) return TRUE;
	//�����[�z���C�g����
	if (r_idx >= MON_KISUME && r_idx <= MON_SUIKA) return TRUE;
	//�~�j������
	if (r_idx >= MON_IKU && r_idx <= MON_EIKI) return TRUE;
	//�A���X�l�`����
	if (r_idx == MON_HATATE) return TRUE;
	if (r_idx == MON_REIMU) return TRUE;
	if (r_idx == MON_MARISA) return TRUE;
	if (r_idx == MON_MAMIZOU) return TRUE;

	if (r_idx == MON_3FAIRIES) return TRUE;

	if (r_idx >= MON_SEIRAN && r_idx <= MON_HECATIA3) return TRUE;
	//�˕P�L�P���C�Z���U�̓X�y�J���Ȃ��̂ŏo���Ȃ�
	if (r_idx >= MON_LARVA && r_idx <= MON_OKINA) return TRUE;
	if (r_idx >= MON_JYOON && r_idx <= MON_SHION_2) return TRUE;

	//v1.1.64 �S�`�b
	if (r_idx >= MON_EIKA && r_idx <= MON_SAKI) return TRUE;

	//�����͏o���Ȃ�

	//v1.1.86 ������
	if (r_idx >= MON_MIKE && r_idx <= MON_MOMOYO) return TRUE;

	//v1.1.91 ���~�ٕ�
	if (r_idx == MON_YUMA) return TRUE;

	return FALSE;
}






//v1.1.51 �V�A���[�i�J�n���[�`��
//���j���[���爫�����x����I�����A���̃��x���ɉ����������X�^�[�ő�5�̂�I�肵��nightmare_mon_r_idx[]�ɋL�^����B
//����ɏ������Ƃ��ɓ�����C�C�b���ˁI�|�C���g(��)���v�Z����nightmare_point�ɋL�^����B
//�L�����Z�������Ƃ���FALSE
bool	nightmare_diary(void)
{
	int i;
	int monster_num;
	int nightmare_lev_limit;
	int nightmare_level;
	char c;
	int loop_count;
	bool	flag_err = FALSE;

	//�N���E���s�[�X(HP30000�̂ق�)�͎Q���s�ɂ��Ƃ�
	if (p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		msg_print(_("���Ȃ��͖{�������Ă��炦�Ȃ������B",
                    "You weren't allowed to read the diary."));
		return FALSE;
	}

	//������Q���s�B����Ƃ��ɜ߈ˌ��ʂ����ł��ďo���Ȃ��Ȃ�̂�
	if (CHECK_MIZUCHI_GHOST)
	{
		msg_print(_("��߂Ă������B���̎x�z�҂ɐS���J���������Ȃ��B",
                "Better not do this. You don't want to open up your heart to the ruler of dreams."));
		return FALSE;
	}


	screen_save();


	//����\�Ȉ������x���𔻒�
	for (i = 0; i<NIGHTMARE_DIARY_STAGE_LEV_MAX; i++)
	{
		if (nightmare_total_point < nightmare_stage_table[i].need_points) break;
	}
	nightmare_lev_limit = i - 1;

	if (p_ptr->wizard) nightmare_lev_limit = NIGHTMARE_DIARY_STAGE_LEV_MAX - 1;

	//�����X�e�[�W�ꗗ�����j���[�\��
	clear_bldg(4, 22);
	if (p_ptr->pclass == CLASS_DOREMY)
		prt(_("�u�ǂ�Ȗ���H�ׂ悤������...�v", "'I wonder what kind of dreams I'll be eating..."), 7, 20);
	else if (nightmare_lev_limit == (NIGHTMARE_DIARY_STAGE_LEV_MAX - 1))
	{
		prt(_("�h���~�[�u���z�ɁB�M���͂����������瓦����Ȃ��c�c�v",
            "Doremy - 'Pitiful. You can't escape from the nightmare anymore......'"), 7, 20);
	}
	else if (nightmare_total_point > 100000)
	{
		prt(_("�h���~�[�u�܂����Ă��܂����̂ł��ˁc�c�o��͏o���Ă܂����H�v",
            "Doremy - 'You're back again....... Are you ready?'"), 7, 20);
	}
	else
	{
		prt(_("�h���~�[�u����A���̖{�ɏ�����Ă��邱�Ƃ�����ȂɋC�ɂȂ�̂ł����H�v",
            "Doremy - 'Oh, so you're interested in what's written in this book?'"), 7, 20);
	}
	for (i = 0; i <= nightmare_lev_limit; i++)
	{
		prt(format("%c) %s", ('a' + i), nightmare_stage_table[i].desc), 9 + i, 22);
	}
	prt(format(_("���:%d��%d��",
                "Battle Record: %d wins, %d attempts"), nightmare_diary_count, nightmare_diary_win), 18, 30);
	prt(format(_("�݌v�]���_:%d",
                "Total Points: %d"), nightmare_total_point), 19, 30);

	//�I�����󂯕t���Ĉ������x��������
	while (1)
	{
		c = inkey();
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		nightmare_level = c - 'a';
		if (nightmare_level < 0 || nightmare_level > nightmare_lev_limit) continue;

		break;
	}

	//get_mon_num_prep()�Ɏg�p����̂ł��̎��_��inside_arena�t���O�𗧂Ă�B�r���I������ꍇFALSE�ɂ��Ă���o��
	p_ptr->inside_arena = TRUE;


	//�������x���ɍ��킹�ă����X�^�[���ő�5�̑I��
	for (loop_count = 100; loop_count; loop_count--)
	{
		int j, k;
		bool flag_ng = FALSE;

		//�ϐ�������
		tmp_nightmare_point = 0;
		for (i = 0; i<NIGHTMARE_DIARY_MONSTER_MAX; i++)nightmare_mon_r_idx[i] = 0;

		//�o�Ă��郂���X�^�[������
		switch (nightmare_level)
		{
		case 0:		case 1:		case 2:
			monster_num = 2;
			break;
		case 3:
			monster_num = 2 + randint0(2);
			break;
		case 4:
			monster_num = 2 + randint0(3);
			break;
		case 5:		case 6:
			monster_num = 3 + randint0(3);
			break;
		default:
			monster_num = 5;
			break;
		}

		//�����X�^�[���x����ݒ�
		nightmare_mon_base_level = nightmare_stage_table[nightmare_level].level;
		if (nightmare_level == (NIGHTMARE_DIARY_STAGE_LEV_MAX - 1)) nightmare_mon_base_level += randint0(30);
		if (nightmare_mon_base_level > 127) nightmare_mon_base_level = 127;

		// -Hack- �擾�|�C���g�v�Z�ƃ����X�^�[�I��̂��߂ɉσp�����[�^�����X�^�[�̃��x���l�����������ɏ���������B���ۂ̃p�����[�^�����nightmare_gen()�ł̃����X�^�[�z�u���ɍs���B
		r_info[MON_REIMU].level = nightmare_mon_base_level;
		r_info[MON_MARISA].level = nightmare_mon_base_level;
		r_info[MON_RANDOM_UNIQUE_1].level = nightmare_mon_base_level;

		//msg_format("mon_lev:%d", nightmare_mon_base_level);

		//�����X�^�[��I��
		get_mon_num_prep(monster_hook_nightmare_diary, NULL);
		for (j = 0; j<monster_num; j++)
		{
			nightmare_mon_r_idx[j] = get_mon_num(nightmare_mon_base_level);
		}

		//for (j = 0; j < monster_num; j++)			msg_format("gmn  r_idx:%d",nightmare_mon_r_idx[j]);

		//���������X�^�[�����Ƃ�蒼�� �����ƃX�[�p�[���������Ȃ��悤�ɂ���
		for (j = 0; j<monster_num - 1; j++)
		{
			for (k = j+1; k<monster_num; k++)
			{
				if (nightmare_mon_r_idx[j] == nightmare_mon_r_idx[k]) flag_ng = TRUE;
				if (nightmare_mon_r_idx[j] == MON_SHION_1 && nightmare_mon_r_idx[k] == MON_SHION_2) flag_ng = TRUE;
				if (nightmare_mon_r_idx[j] == MON_SHION_2 && nightmare_mon_r_idx[k] == MON_SHION_1) flag_ng = TRUE;
			}
		}

		//v1.1.52 �����X�^�[��2�̂������Ȃ��Ƃ��ɖ��̐��E�́��͏o�Ȃ��悤�ɂ���B
		//�����X�y�J�����o�^����ĂȂ��E�Ƃ̂Ƃ������������肸�ɃG���[�ɂȂ邽�߁B
		for (j = 0; j < monster_num; j++)
		{
			if (monster_num < 3 && nightmare_mon_r_idx[j] == MON_RANDOM_UNIQUE_1) flag_ng = TRUE;
		}

		if (flag_ng) continue;

		//���̃X�e�[�W���N���A���ē�����|�C���g���w��͈͊O���Ƃ�蒼��
		tmp_nightmare_point = calc_nightmare_points();

//msg_format("points:%d", tmp_nightmare_point);
		if (tmp_nightmare_point < nightmare_stage_table[nightmare_level].min_points || tmp_nightmare_point > nightmare_stage_table[nightmare_level].max_points) flag_ng = TRUE;
		if (flag_ng) continue;

		break;
	}

	if (!loop_count)
	{
		msg_format(_("ERROR:�������x��%d�ɂ����ă����X�^�[�����������s�񐔂��K��l�𒴂���",
                    "ERROR: Monster generation logic exceed attempt limit for nightmare level %d"), nightmare_level);
		screen_load();
		p_ptr->inside_arena = FALSE;
		return FALSE;
	}

	if (cheat_xtra) msg_format("nightmare_lev:%d", nightmare_level);

	//test
	/*
	nightmare_mon_base_level = 30;
	nightmare_mon_r_idx[0] = MON_REIMU;
	nightmare_mon_r_idx[1] = MON_MARISA;
	nightmare_mon_r_idx[2] = MON_RANDOM_UNIQUE_1;
	nightmare_mon_r_idx[3] = 0;
	nightmare_mon_r_idx[4] = 0;
	*/

	tmp_nightmare_point = calc_nightmare_points();

	//���肪���肵��r_idx�ƃ|�C���g���L�^������A�A���[�i�֌W�̃t���O�𗧂ĂČ�������o��
	screen_load();

	p_ptr->exit_bldg = FALSE;
	reset_tim_flags();
	prepare_change_floor_mode(CFM_SAVE_FLOORS);

	p_ptr->leaving = TRUE;

	//v1.1.55 �A���[�i�ɓ���Ƃ�HP���L�^���Ă����ďo��Ƃ��������邱�Ƃɂ����B
	//�A���[�i���Ŕs�k������HP�S���ŏo�邱�Ƃɂ��Ă������A�A���[�i���ŕm���ɂȂ��ďo���ꍇ�R���̕�΂Ƃ��ŃQ�[���I�[�o�[�ɂȂ�\��������͕̂ςȘb�Ȃ̂ōďC��
	nightmare_record_hp = p_ptr->chp;

	//�V�A���[�i����񐔂ɉ��Z
	nightmare_diary_count++;

	return TRUE;

}



/*:::v1.1.56
*�^�E���������ň�i����̒��i�𔃂��W�߂錚���p�R�}���h��ǉ��B
*�P�ɒ����v���C���[�����ɋ��̎g������񋟂���(������܂ŏo���Â炩������i���̌������i�l�^�𖳗����g��)�ړI�Ȃ̂Œ��i�ɗp�r�͂Ȃ��B
*�ꕔ���j�[�N�N���X�ł͎����������Ă���͂��̕i�����l�Ŕ��킳��邩������Ȃ������e�͊肨���B
*�A�C�e����TV=STRANGE_OBJECT,SV=p_ptr->kourindou_number�̕i��SV=0���珇�ɔ���o�����B
*�A�C�e����ǉ�����ɂ�k_info.txt�ɒǋL���Ă���SV_STRANGE_OBJ_MAX������������B
*/
void	bact_buy_strange_object(void)
{

	char o_name[256];
	object_type	forge;
	object_type 	*o_ptr = &forge;
	int		k_idx, price;

	//�A�C�e���Љ�ɓK���ɕ\�����镶��
	cptr random_phrase[] =
	{
#ifdef JP
		"���̖l�����߂Ă��ڂɂ������i",		"�V���ɓ�Ɩ����Ə̂��ꂽ�i",		"�V���̖��i�Ɨ_�ꍂ���i",
		"���͂⌻�����Ȃ��Ƃ���Ă����i",		"���̏��i�G�v�����ł��Ƃ����i",		"���ɕ��Ԃ��̂Ȃ����i",
		"�Ȃ�ƊO�E�̕i",						"�Ȃ�ƈيE�̕i",
#else
        "something I've never seen before",		"a masterpiece of worldly renown",
		"a supposedly no longer existing item",	"a beloved item of Matsunaga Hisahide",		"unparalleled rare item",
		"some item from Outside World",			"some otherworldly item",
#endif
	};

	if (!PLAYER_IS_TRUE_WINNER && !p_ptr->wizard)
	{
		msg_print(_("ERROR:*�^�E����*���Ă��Ȃ��̂�buy_strange_object()���Ă΂ꂽ",
                    "ERROR: buy_strange_object() called for a non-true winner"));
		return;
	}

	//���ɑS�Ă̒��i�𔃂����I�����ꍇ
	if (p_ptr->kourindou_number > SV_STRANGE_OBJ_MAX)
	{
		prt(_("���V���u�\����Ȃ��������e���̖��i���i�͌N�ɔ����Ă��܂����B",
            "Rinnosuke 'I'm very sorry, but you already've bought all the rare items."), 7, 10);
		prt(_("�����ꉽ���d����Ă�����B�v", "I'll get new ones someday.'"), 8, 20);
		return;
	}

	/*::: Hack - �������́u�䂪�Ɓv�́u�Ō�ɖK�ꂽ�^�[���v�̒l���g���Ĉ����x���������Ȃ����Ƃɂ���*/
	/*..�Ǝv����������ȂƂ���ɗ���v���[���[�̓^�[������ɒB���Ă邩�������̂ł�߂Ƃ�
	if ((town[TOWN_KOURIN].store[STORE_HOME].last_visit > turn - (TURNS_PER_TICK * TOWN_DAWN)) && !p_ptr->wizard )
	{
		prt("���V���u��قǂ̖��i�͂��C�ɏ��������ȁH�܂����Ă��ꂽ�܂��B�v", 7, 10);
		return;
	}
	clear_bldg(4, 18);
	*/

	k_idx = lookup_kind(TV_STRANGE_OBJ, p_ptr->kourindou_number);
	if (!k_idx)
	{
		msg_format(_("ERROR:SV=%d�̃A�C�e�������݂��Ȃ�",
                    "ERROR Item with SV=%d does not exist"), p_ptr->kourindou_number);
		return;
	}

	//���X�g����A�C�e������
	object_prep(o_ptr, k_idx);
	object_desc(o_name, o_ptr, 0);
	price = k_info[k_idx].cost;

	//��ʂɕ\�����������m�F
	prt(format(_("�u�����ƁA����%s�ɖڂ�t����Ƃ͂��ڂ������B",
                "'Ooh, so you have interest in %s."),o_name), 7, 10);
	prt(format(_("�@�����%s�ŁA", "  This is %s"), random_phrase[randint0(sizeof(random_phrase) / sizeof(cptr))] ), 8, 10);
	prt(format(_("�@�l�i��$%d���B�v", "  I'm selling it for $%d.'"), price), 9, 10);
	if (!get_check_strict(_("���̕i�𔃂����܂����H", "Purchase this souvenir?"), CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print(_("�ו�����t���B", "Your pack is full."));
		clear_bldg(4, 18);
		return;
	}

	if (p_ptr->au < price)
	{
		msg_print(_("����������Ȃ��B", "You don't have enough money."));
		clear_bldg(4, 18);
		return;
	}

	p_ptr->au -= price;
	building_prt_gold();
	p_ptr->kourindou_number++;
	inven_carry(o_ptr);
	msg_print(_("���i�𔃂�������I", "Purchased a rare souvenir!"));

	clear_bldg(4, 18);

	/*::: Hack - �������́u�䂪�Ɓv�́u�Ō�ɖK�ꂽ�^�[���v�̒l���g���Ĉ����x���������Ȃ����Ƃɂ���*/
	//town[TOWN_KOURIN].store[STORE_HOME].last_visit = turn;
}




//v2.0.5
//�͂��ẲƂŔO�ʂ��g���Đl�T�������Ă��炤
//�����I�ɂ͎w�肳�ꂽ������Ń��j�[�N�����X�^�[���������A���̋��ꏊ�Ƃ��ă����_���ȃ_���W�����ƃt���A��ݒ肵�A
//���������ɍs�����Ƃ����̃����X�^�[�𐶐�����B
//p_ptr->hatate_mon_search_ridx:�Ώۃ��j�[�N�����X�^�[��r_idx�l
//p_ptr->hatate_mon_search_dungeon: (�_���W����idx*1000) + (�_���W�����K�w)
//���t���ς�����炱�̒l�̓��Z�b�g�����B�������������ł̓��Z�b�g����Ȃ�
//����(��p����)�̂Ƃ�TRUE��Ԃ�
#define HATATE_SEARCH_MON_LIST_MAX 10 //��⃊�X�g�ő咷
bool hatate_search_unique_monster(void)
{

	bool hatate = (p_ptr->pclass == CLASS_HATATE);

	//���łɎ��s�ς݂̂Ƃ������܂ŗ��p�s�@
	//�����͂��Ă̂Ƃ��͉��x�ł��ł���悤�ɂ���
	if (p_ptr->hatate_mon_search_ridx)
	{
		if (hatate)
		{
			prt(_("���łɒǐՒ��̑��肪���邪...", "There's already someone I'm looking for..."), 7, 20);
			prt(_("�@��߂đ��̐l��T�������H", "   Should I stop and look for somebody else?"), 8, 20);
			if (!get_check_strict(_("�ύX���܂����H ", "Change the target? "), CHECK_DEFAULT_Y)) return FALSE;
		}
		else if(!p_ptr->wizard)
		{
			if(EXTRA_MODE)
				prt(_("�u������ꂿ������B�܂����x�ˁB�v", "'I'm already tired. See you next time.'"), 7, 20);
			else
				prt(_("�u�����͂�����ꂿ������B�܂������ˁB�v", "'I'm tired for today. See you tomorrow.'"), 7, 20);

			return FALSE;
		}
	}

	clear_bldg(4, 18);

	if (hatate)
		prt(_("�N��T�����H", "Who are you looking for?"), 7, 20);
	else if(one_in_(4))
		prt(_("�u����`���҂���B�悤�����䂪�A�W�g�ցv", "'Oh, an adventurer. Welcome to my hideout.'"), 7, 20);
	else
		prt(_("�u���ɒN����T���Ăق����̂ˁH�v", "'Do you want me to look for someone?'"), 7, 20);

	//�������L�����Z���܂ŌJ��Ԃ�
	while (TRUE)
	{
		int i, xx;
		char temp[120] = "";//�����L�[���[�h
		char m_name[120];//�����X�^�[���̌��
		char m_ename[120];//�����X�^�[���̌��(�p��)

		monster_race *r_ptr;
		int r_idx_list[HATATE_SEARCH_MON_LIST_MAX];
		int r_idx_list_len = 0;
		int search_r_idx = 0;
		int search_dungeon = 0;
		int search_floor = 0;

		bool flag_too_much = FALSE;

		//�����L�[���[�h����
		if (!get_string(_("���j�[�N�E�����X�^�[�̖��O(������v):", "Unique monster name (partial match): "), temp, 100))
		{
			clear_bldg(4, 18);
			return FALSE;
		}
		//�L�[���[�h�̑啶�����������ɂ��Ă���
		for (xx = 0; temp[xx] && xx < 120; xx++)
		{
			if (iskanji(temp[xx]))
			{
				xx++;
				continue;
			}
			if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
		}

		//�S�����X�^�[���̂��T�[�`���ăL�[���[�h�ɕ�����v������̂��ő�10�����X�g�ɓo�^
		for (i = 1; i<max_r_idx; i++)
		{

			r_ptr = &r_info[i];

			//�񃆃j�[�N�͏��O(���C�}������)
			if (!(r_ptr->flags1 & RF1_UNIQUE) && i != MON_REIMU && i != MON_MARISA) continue;

			//俎q���Z���p�_�~�[�����O
			if (i == MON_SUMIREKO_2) continue;

			//�����ƃ��Y�O�X�͑œ|�󋵂ɂ��ϐg�O���r�����O
			if (i == MON_SHION_2 && r_info[MON_SHION_1].r_akills == 0) continue;
			if (i == MON_SHION_1 && r_info[MON_SHION_1].r_akills  > 0) continue;
			if (i == MON_MOZGUS2 && r_info[MON_MOZGUS].r_akills == 0) continue;
			if (i == MON_MOZGUS && r_info[MON_MOZGUS].r_akills  > 0) continue;

#ifdef JP
			//���{�ꃂ���X�^�[��
			strcpy(m_name, r_name + r_ptr->name);

			//�p�������X�^�[��(�啶���͏������ɂ���)
			strcpy(m_ename, r_name + r_ptr->E_name);
			for (xx = 0; m_ename[xx] && xx < 80; xx++) if (isupper(m_ename[xx])) m_ename[xx] = tolower(m_ename[xx]);
#else
            strcpy(m_name, r_name + r_ptr->name);
            for (xx = 0; m_name[xx] && xx < 100; xx++) if (isupper(m_name[xx])) m_name[xx] = tolower(m_name[xx]);
#endif

			clear_bldg(4, 18);

			//�����L�[���[�h�ƈ�v������r_idx���ő�10���X�g�ɋL�^
#ifdef JP
			if (my_strstr(m_name, temp) || my_strstr(m_ename, temp))
#else
            if (my_strstr(m_name, temp))
#endif
			{
				//���X�g������B
				if (r_idx_list_len == HATATE_SEARCH_MON_LIST_MAX)
				{
					flag_too_much = TRUE;
					break;
				}
				r_idx_list[r_idx_list_len] = i;
				r_idx_list_len++;

			}

		}
		if (flag_too_much)
		{
			if (hatate)
				prt(format(_("��₪��������悤���B�L�[���[�h��ς��Ă�蒼�����B", "Too many potential targets; change your keyword and try again.")), 7, 20);
			else
				prt(format(_("�u��₪�������ˁB���������i���Ă��炦��H�v", "'There's too many potential targets; can you narrow it down?'")), 7, 20);
			continue;

		}

		if (!r_idx_list_len)
		{
			if (hatate)
				prt(format(_("�Y���Ȃ����B�L�[���[�h��ς��Ă�蒼�����B", "No matches found; change your keyword and try again.")), 7, 20);
			else
				prt(format(_("�u�Y���Ȃ��ˁB�L�[���[�h��ς��Ă��炦��H�v", "'No matches found. Can you try another keyword?'")), 7, 20);

			continue;
		}

		clear_bldg(4, 18);

		if (hatate)	prt(format(_("%d���Y�������I", "%d matches found!"), r_idx_list_len), 8, 20);
		else		prt(format(_("�u...%d������������I�v", "'...I've found %d matches!'"), r_idx_list_len), 8, 20);
		//��������r_idx�̃��X�g�̒�����T�������I��
		//���X�g�\������̂��ʓ|�Ȃ̂Ń��X�g�擪�����l����Yes/No�Ŋm�F ���Ԃ񃊃X�g�ɂ��Ă��債�ĈӖ��͖������낤
		for (i = 0; i<r_idx_list_len; i++)
		{
			r_ptr = &r_info[r_idx_list[i]];

			clear_bldg(9, 18);

			//���{�ꃂ���X�^�[���𓾂�
			strcpy(m_name, r_name + r_ptr->name);

			if (hatate)
				prt(format(_("�y%s�z��T�������H", "Search for '%s'?"), m_name), 9, 20);
			else
				prt(format(_("�u�y%s�z�ł���������H�v", "'Should I look for '%s'?'"), m_name), 9, 20);

			if (!get_check_strict(_("�T���܂����H ", "Search? "), CHECK_DEFAULT_Y)) continue;

			search_r_idx = r_idx_list[i];
			break;

		}

		clear_bldg(4, 18);
		if (!search_r_idx) return FALSE;

		if (hatate)
			prt(format(_("%s�̋��ꏊ��...", "%s is..."), m_name), 8, 20);
		else
			prt(format(_("�u%s�̋��ꏊ�͂�...", "'%s is..."), m_name), 8, 20);

		inkey();

		//�ΏۊO���ꏈ��
		//�͂���
		if (search_r_idx == MON_HATATE)
		{
			if (hatate)
				prt(_("������񂱂̕������ʂ��Ă���B", "Of course, the photo is of this very room."), 9, 20);
			else
				prt(_("�@���Ȃ��̖ڂ̑O��H�v", "   right in front of you?'"), 9, 20);
			inkey();
			return FALSE;
		}
		//�����@�����͂��Ă̂Ƃ��͈�O�ɍs���͂�
		else if (monster_is_you(search_r_idx))
		{
			if(p_ptr->pclass == CLASS_MIZUCHI)
				prt(_("�@����ς�ʖڂˁB������O�ʂ���Ɖ摜���j������́B���߂�ˁH�v",
                    "  It's no use. The image gets messed up when I try to take a photo. I'm sorry."), 9, 20);
			else
				prt(_("�@��������I���̖ڂ̑O�ˁI�v", "  it's you! You're in front of me!'"), 9, 20);
			inkey();
			return FALSE;
		}

		//�œ|�ς�
		//v2.1.0 ���C�}������
		if (r_ptr->r_akills && search_r_idx != MON_REIMU && search_r_idx != MON_MARISA)
		{
			if (hatate)
				prt(_("�@�������i�D�ǂ��|�����V�[�����ʂ��Ă����B", "   There's a scene of you beating the target up."), 9, 20);
			else
				prt(_("�@���Ȃ��ɂ���Ă�Ƃ��낪�ʂ��Ă�񂾂���...�v", "   It's just the place where you've beaten them up...'"), 9, 20);
			inkey();
			return FALSE;
		}

		//�ʏ�o�����Ȃ����A���e�B�l �����̃o�[�m�[�����p�[�g�A�v�ɂȂ����僀�J�f���Y��
		if (r_ptr->rarity > 100)
		{
			if (hatate)
				prt(_("�摜�������Ⴎ����ɗ���Ă���B���ʂɏo��鑊��ł͂Ȃ��悤���B",
                    "You can't make anything out in the picture. It's not somebody you can normally encounter."), 9, 20);
			else
			{
				prt(_("�@���[��A�킩��Ȃ���B", "   Uhhh, I don't get it."), 9, 20);
				prt(_("�@�Ȃ��������Ǝʂ�Ȃ��́B�v", "   For some reason, I couldn't get the picture.'"), 10, 20);
			}

			inkey();
			return FALSE;
		}

		//�_���W�����{�X
		if (r_ptr->flags7 & RF7_GUARDIAN)
		{
			for (i = 1; i < DUNGEON_MAX; i++)
			{
				if (d_info[i].final_guardian == search_r_idx)
				{
					if (hatate)
						c_put_str(TERM_WHITE, format(_("%s�̍Ő[�w������Ă���悤���B",
                                                        "guarding the depths of %s."), (d_name + d_info[i].name)), 9, 20);
					else
					{
						c_put_str(TERM_WHITE, format(_("������%s�̍Ő[�w�ő҂��\���Ă����B",
                                                        "waiting in the depths of %s."), (d_name + d_info[i].name)), 9, 20);
						prt(_("�@���̏��͖����ł�����B�v",
                            "   This bit of info is for free.'"), 10, 20);
					}

					inkey();
					return FALSE;

				}
			}
		}

		//QUESTOR �����N�G�ΏہA�N�G�X�g��p�A�V�i���I�{�X�A�Q�[�g�L�[�p�[�Ȃ�
		if (r_ptr->flags1 & RF1_QUESTOR)
		{

			if (hatate)
				prt(_("�@�s�����B�ǂ���������Ă���悤���B",
                    "   It's unclear, but your target is protecting something."), 9, 20);
			else
			{
				prt(_("�@�悭������Ȃ���B", "   I don't understand."), 9, 20);
				prt(_("�@�����Ɠ������ɂǂ���������Ă���݂����B�v",
                    "   Your target is protecting something without moving from the spot.'"), 10, 20);
			}
			inkey();
			return FALSE;

		}
		//���q�ˎҁ@�Y���N�G�X�g��̂܂ŒT���Ȃ�
		if (r_ptr->flags3 & RF3_WANTED)
		{
			int cnt;
			bool flag_can_alloc = FALSE;
			for (cnt = max_quests - 1; cnt > 0; cnt--)
			{
				if (quest[cnt].status != QUEST_STATUS_TAKEN || quest[cnt].type != QUEST_TYPE_BOUNTY_HUNT)continue;
				if (quest[cnt].r_idx != search_r_idx) continue;

				flag_can_alloc = TRUE;
				break;
			}

			if (!flag_can_alloc)
			{
				if (hatate)
					prt(_("�@������Ȃ��B��ʂ͐^���Â��B", "   You can't understand; the picture is all black."), 9, 20);
				else
				{
					prt(_("�@�悭������Ȃ���B", "   I don't understand."), 9, 20);
					prt(_("�@�ǂ����^���ÂȂƂ���ɉB��Ă���̂�����H�v",
                        "   Maybe your target is hiding somewhere in a pitch-black place?'"), 10, 20);
				}
				inkey();
				return FALSE;
			}
		}

		//�o���_���W����������

		if (EXTRA_MODE)
		{
			//EXTRA�ł͓S���ƍ��ׂ̗̈����
			search_dungeon = dungeon_type;

			//����_���W�����ɂ����o�Ȃ������X�^�[�͎��s
			//v2.0.8 FORCE_DEPTH�t���Ń��x��100�𒴂��郂���X�^�[��S���Ŏw�肵���Ƃ������������s
			if ((search_dungeon == DUNGEON_ANGBAND && (r_ptr->flags1 & RF1_FORCE_DEPTH) && r_ptr->level > 100)
				|| ((r_ptr->flags8 & RF8_STAY_AT_HOME3) && r_ptr->extra != search_dungeon))
			{
				if (hatate)
					prt(_("�@���̃_���W�����ɂ��Ȃ��悤���B", "   Not in this dungeon."), 9, 20);
				else
				{
					prt(_("�@���[��A���̃_���W�����ɂ͋��Ȃ���ˁB�v",
                        "   Uhh, your target isn't in this dungeon.'"), 9, 20);
				}
				inkey();
				return FALSE;
			}

		}
		else
		{
			//����_���W�����ɂ����o�Ȃ��Ȃ炻��
			if (r_ptr->flags8 & RF8_STAY_AT_HOME3)
				search_dungeon = r_ptr->extra;
			//v2.0.8 ���x��100�ȏ��FORCE_DEPTH�����X�^�[�͍��ׂ̗̈�
			else if ((r_ptr->flags1 & RF1_FORCE_DEPTH) && r_ptr->level > 100)
				search_dungeon = DUNGEON_CHAOS;
			//����_���W�����ɂ悭�o��Ȃ獂�m���ł��� AT_HOME1�͖���
			else if (r_ptr->flags8 & RF8_STAY_AT_HOME2 && !one_in_(8))
				search_dungeon = r_ptr->extra;
			//���C�}���͓S���̂�(place_monster_one()�ł����ݒ肵�Ă���̂�)
			else if(search_r_idx == MON_REIMU || search_r_idx == MON_MARISA)
				search_dungeon = DUNGEON_ANGBAND;
			//�����Ȃ猺���̑�
			else if (r_ptr->flags7 & RF7_AQUATIC)
				search_dungeon = DUNGEON_GENBU;
			//�����n�Ȃ璆�m���Ŗ��@�̐X
			else if(r_ptr->flags3 & RF3_ANIMAL && one_in_(2))
				search_dungeon = DUNGEON_FOREST;
			//���l�n�Ȃ璆�m���ōg����
			else if (r_ptr->flags3 & RF3_DEMIHUMAN && one_in_(2))
				search_dungeon = DUNGEON_KOUMA;
			//�n���֌W�҂Ȃ璆�m���Œn����������
			else if (r_ptr->flags7 & RF7_FROM_HELL && one_in_(2))
			{
				if(one_in_(2))
					search_dungeon = DUNGEON_MUEN;
				else
					search_dungeon = DUNGEON_HELL;
			}
			//���z�����j�[�N�̓t���A���x���������Ƃ����m���œS���ɂȂ肻��ȊO�̂Ƃ��e�n�̂ǂ���
			else if(is_gen_unique(search_r_idx))
			{
				int ang_dlv = max_dlv[DUNGEON_ANGBAND];

				if ((ang_dlv < r_ptr->level + 20) && (ang_dlv > r_ptr->level - 10) && !one_in_(3))
				{
					search_dungeon = DUNGEON_ANGBAND;
				}
				else
				{
					switch (randint1(6))
					{
					case 1:
						search_dungeon = DUNGEON_TAISHI;
						break;
					case 2:
						search_dungeon = DUNGEON_GENBU;
						break;
					case 3:
						search_dungeon = DUNGEON_KOUMA;
						break;
					case 4:
						search_dungeon = DUNGEON_ZIGOKUDANI;
						break;
					case 5:
						search_dungeon = DUNGEON_MUEN;
						break;
					default:
						search_dungeon = DUNGEON_ANGBAND;
						break;
					}

				}
			}
			//����ȊO�͓S��
			else
			{
				search_dungeon = DUNGEON_ANGBAND;
			}
		}

		//�o���t���A������

		if (EXTRA_MODE)
		{
			//���݃t���A���5�`10�[���ꏊ
			search_floor = dun_level + 4 + randint1(6);

			//�G���x�����10���x���ȏ�󂭂͂Ȃ�Ȃ�
			if (r_ptr->level -10 > search_floor ) search_floor = r_ptr->level - 10;

			//EXTRA�N�G�X�g�t���A�ɂ͏o�Ȃ�
			if ((search_floor != 100) && (search_floor % 10 == 0)) search_floor++;

			//���
			if (search_floor > d_info[search_dungeon].maxdepth) search_floor = d_info[search_dungeon].maxdepth;

		}
		else
		{

			//v2.0.8�@�S�����ΏۂɂȂ�����o���t���A�����̋A�҃t���A�߂��ɂ��邱�Ƃɂ���
			if (search_dungeon == DUNGEON_ANGBAND)
			{
					search_floor = max_dlv[DUNGEON_ANGBAND] + randint1(8) - 6;
			}
			//����ȊO�̃_���W�����ł̓����X�^�[���x���}5
			else
			{
				search_floor = r_ptr->level + randint1(11) - 6;
			}

			//���
			if (search_floor > d_info[search_dungeon].maxdepth)
			{
				search_floor = d_info[search_dungeon].maxdepth - randint0(3);
			}

			//����
			if (search_floor < d_info[search_dungeon].mindepth)
			{
				search_floor = d_info[search_dungeon].mindepth + randint0(3);
			}

			//v2.0.12 40���x���ȏ�󂢃t���A�ɂ͏o�Ȃ����Ƃɂ���
			if (r_ptr->level > search_floor + 40)
			{
#ifdef JP
				if (hatate)
					prt("�@�摜���s�N�����B�������s����ꏊ���牓������̂�������Ȃ��B", 9, 20);
				else
				{
					prt("�@������Ɖ摜���s�N����...", 9, 20);
					prt("�@���������T���͈͂��L���Ă����Ε����邩���B�v", 10, 20);
				}
#else
				if (hatate)
					prt("  The picture is unclear. You might be too far away from the destination.", 9, 20);
				else
				{
					prt("  The picture is a bit unclear...", 9, 20);
					prt("  I might be able to tell more if you explore a bit deeper.'", 10, 20);
				}
#endif
				inkey();
				return FALSE;

			}

		}

#ifdef JP
		if (hatate)
			c_put_str(TERM_WHITE, format("������!%s��%d�K�ɂ���I", (d_name + d_info[search_dungeon].name), search_floor), 9, 20);
		else
			c_put_str(TERM_WHITE, format("��������!%s��%d�K��I�v", (d_name + d_info[search_dungeon].name), search_floor), 9, 20);
#else
		if (hatate)
			c_put_str(TERM_WHITE, format("You've found the target! Level %d of %s!", search_floor, (d_name + d_info[search_dungeon].name)), 9, 20);
		else
			c_put_str(TERM_WHITE, format("I've found the target! Level %d of %s!'", search_floor, (d_name + d_info[search_dungeon].name)), 9, 20);
#endif

		//v2.1.0 �ǉ��W���[�N���b�Z�[�W
		if (search_r_idx == MON_DIO)
		{

			if (hatate)
				c_put_str(TERM_WHITE, format(_("...����������ĉ�������ł���悤�Ɍ�����̂͋C�̂������낤���H",
                                            "...looks like he's looking here and shouting something... but that's just my imagination?")), 10, 20);
			else
					c_put_str(TERM_WHITE, format(_("(�Ђ���Ƃ��Ă��̐l�������ɋC�Â��Ă�H)",
                                            "(By the way, did that person notice that?)")), 10, 20);

		}

		inkey();

		//�_���W�����ƃt���A���p�ϐ��ɋL�^
		p_ptr->hatate_mon_search_ridx = search_r_idx;
		p_ptr->hatate_mon_search_dungeon = search_dungeon*1000 + search_floor;

		break;
	}

	clear_bldg(4, 18);

	return TRUE;

}

#undef HATATE_SEARCH_MON_LIST_MAX

//v2.0.8
//�|�����f�ރ����X�^�[���Ǔے��Ɏ�������ŗ������Ă��炤
//�f�ރ����X�^�[��RF8_FOOD���ݒ肳��Ă���A�|�����Ƃ���p_ptr->cooking_material_flag�ɋL�^����Ă���
//flag_self:�����ŗ�������Ƃ�TRUE ���b�Z�[�W�̂ݕω�
//�s���������Ƃ�TRUE(�������Z�ȊO�ł͊֌W�Ȃ�)
bool geidontei_cooking(bool flag_self )
{

	int material_list[16];
	int material_num = 0;
	int food_list_idx;
	int i;

	//�܂��f�ނ������Ă��Ē񋟉\�ȗ��������X�g����
	for (i = 0; monster_food_list[i].r_idx; i++)
	{

		if (material_num >= 16)
		{
			msg_print(_("ERROR:geidontei_cooking()�ŗ����I����ʂ̃y�[�W���肪������",
                    "ERROR: page selection not implemented for cooking selection screen in geidontei_cooking()"));
			return FALSE;
		}

		//�Y���̑f�ނ������Ă���Ƃ�
		if ((1L << i) & p_ptr->cooking_material_flag)
		{
			material_list[material_num] = i;
			material_num++;
		}
	}


	if (!material_num)
	{
		if(!flag_self)
			msg_print(_("�������ޑf�ނ��Ȃ��B", "You haven't brought any ingredients."));
        else
			msg_print(_("�����̍ޗ����Ȃ��B", "You don't have cooking ingredients."));


		return FALSE;
	}

	screen_save();
	for (i = 3; i<22; i++) Term_erase(17, i, 255);


	//���ނ̂ݍ���Ƃ��@y/n�Ŋm�F
	if (material_num == 1)
	{

		food_list_idx = material_list[0];

        if(!flag_self)
			prt(format(_("�u%s�v������Ă����悤���B", "Looks like I can make '%^s'."), monster_food_list[food_list_idx].desc), 8, 20);
		else
			prt(format(_("�u%s�v����ꂻ�����B", "Looks like you can make '%^s'."), monster_food_list[food_list_idx].desc), 8, 20);

		if (!get_check_strict(flag_self ? _("���܂����H", "Cook it?"): _("�������܂����H", "Shall you order?"), CHECK_DEFAULT_Y))
		{
			screen_load();
			return FALSE;
		}
	}
	//��������Ƃ��͗��������X�g���đI��
	else
	{
		int choose;

		if(!flag_self)
			prt(_("��������Ă��炢�܂����H(ESC:�L�����Z��)", "What do you want to order? (ESC: cancel)"), 4, 20);
		else
			prt(_("�������܂����H(ESC:�L�����Z��)", "What do you want to cook? (ESC: cancel)"), 4, 20);

		for (i = 0; i<material_num; i++)
		{
			prt(format("%c) %s", 'a' + i, monster_food_list[material_list[i]].desc), 5 + i, 20);
		}

		while (TRUE)
		{
			char c;

			c = inkey();

			if (c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}

			choose = c - 'a';

			if (choose < 0 || choose >= material_num) continue;

			break;
		}

		food_list_idx = material_list[choose];

	}

	if(flag_self)
		msg_print(_("���Ȃ��͕@�̂��̂��Ȃ��痿�����n�߂�...", "You start cooking as you hum to yourself..."));
	else if (one_in_(3))
		msg_print(_("�u�������������[�v", "'It's delicious~'"));
	else if(one_in_(3))
		msg_print(_("�u�����̂��ƂȂ炱�̉���c�����ɂ��C������I�v",
                "'Leave cooking to me, Miyoi Okunoda!'"));
	else
		msg_print(_("�u���Ł[�I�v",
                "'With pleasure!'"));


	for (i = 3; i<22; i++) Term_erase(17, i, 255);

	switch (monster_food_list[food_list_idx].r_idx)
	{

	case MON_SUPPON:
		prt(_("�����ӂ��t�̂��ꑧ�ə�����I",
            "You slurp down the wild-tasting liquid in a single gulp!"), 8, 20);
		set_hero(5000, FALSE);
		set_alcohol(p_ptr->alcohol + 2000);
		break;

	case MON_WILD_BOAR:
		prt(_("�����قǏ�i�Ȗ��ɂȂ����X�[�v�����\�����B",
            "You enjoyed the surprisingly elegantly tasting soup."), 8, 20);
		prt(_("�̂̓��삪�@�q�ɂȂ����C������I",
            "You feel more dextrous!"), 9, 20);
		set_tim_addstat(A_DEX, 2, 5000, FALSE);
		break;

	case MON_WILD_BOAR_2:
		if(!dun_level) //�X���_���W������������ʂ���̂�flag_self���Ƃ܂��s���������̂œK���ɔ��肷��
			prt(_("����ȓ�����F�ɐU�镑���č����ɖj�������I",
                "Everybody eagerly feasted upon huge chunks of meat!"), 8, 20);
		else
			prt(_("����ȓ���������ɏĂ��グ���I",
                "Huge chunks of meat were grilled with gusto!"), 8, 20);

		prt(_("�͂����ӂ��C������I",
            "You feel overflowing with power!"), 9, 20);
		set_tim_addstat(A_STR, 4, 5000, FALSE);

		set_deity_bias(DBIAS_REPUTATION, 1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		break;

	case MON_BADGER:
		prt(_("��������Ă��������I���͂��N���o��悤���I",
            "Fatty and delicious! It makes you feel more vigorous!"), 8, 20);
		set_tim_addstat(A_CON, 2, 5000, FALSE);
		break;

	case MON_RIVER_CRAB:
		prt(_("���΂����Ă��������I",
            "Fragrant and delicious!"), 8, 20);
		prt(_("�����ӎ������Ŋ`�̖؂̖��������C������...",
            "You feel slightly disorientated, dreaming of persimmon trees..."), 9, 20);
		set_tim_res_water(5000, FALSE);
		break;


		//�C�����̃X�[�v
	case MON_NEWT:
		prt(_("���C���N���o��C������I", "You feel energized!"), 8, 20);
		set_tim_regen(5000, FALSE);
		break;

		//�e��
	case MON_VORPAL_BUNNY:
		prt(_("�������肵�đ̂ɗǂ������Ȗ����B", "It has a light flavor and is good for your health."), 8, 20);
		set_oppose_pois(5000, FALSE);
		if(one_in_(3)) prt(_("�e�p�������甽���𔃂����C������...", "You feel as if you've aggravated the Rabbit Alliance..."), 9, 20);//�ʂɉ����N����Ȃ�
		break;

		//���d�̃L��
	case MON_BALL_LIGHTNING:
		prt(_("�H�ׂ���̂��r���r�����n�߂��B", "Your body starts to tingle as you finish eating."), 8, 20);
		set_oppose_elec(5000, FALSE);
		break;

		//���c���E�i�M
	case MON_LAMPREY:
		prt(_("�ڂ��X�b�L���悭������C������B", "You feel you can see very clearly now."), 8, 20);
		set_blind(0);
		set_tim_invis(5000, FALSE);

		break;
		default:
			msg_format(_("ERROR:���̗���(idx:%d)��H�ׂ��Ƃ��̏�������`����Ă��Ȃ�",
                    "ERROR: Undefined logic for eating this dish (idx: %d)"), food_list_idx);
			return FALSE;

	}

	inkey();

	//�����x����
	set_food(p_ptr->food + monster_food_list[food_list_idx].food_amount);

	//�f�ދL�^���X�g����g�����f�ނ��폜����
	p_ptr->cooking_material_flag &= ~((u32b)(1L << food_list_idx));

	screen_load();

	return TRUE;



}


