#include "angband.h"

/*:::ランダムユニークモンスターのパラメータ設定をするルーチン*/


/*:::スキルやパラメータ付与に関わる傾向　最初に設定され場合によっては上書きされる(予定)*/
#define RUBIAS_NONE	0	//バイアスなし
#define RUBIAS_TOUGHNESS	1	//HP、AC、巨大など
#define RUBIAS_POWER	2	//攻撃ダイス増加など
#define RUBIAS_SPEED	3	//加速増加、竜巻など
#define RUBIAS_BILE		4	//酸、劣化など
#define RUBIAS_ELEC	5	//電撃、プラズマなど
#define RUBIAS_COLD	6	//冷気、減速、破片など
#define RUBIAS_FIRE	7	//火炎、核熱、プラズマなど
#define RUBIAS_POIS	8	//毒、劣化、病気など
#define RUBIAS_DEMONIC	9	//暗黒、悪魔召喚、吸収打撃など
#define RUBIAS_HOLY	10	//破邪、閃光、治癒など
#define RUBIAS_WATE	11	//水など
#define RUBIAS_NETH	12	//地獄、アンデッド召喚、能力低下打撃など
#define RUBIAS_CURSE	13	//呪い、精神攻撃など
#define RUBIAS_PHYSICAL	14	//射撃、破片、轟音、ロケットなど
#define RUBIAS_SOUND	15	//轟音、悲鳴、歌攻撃など
#define RUBIAS_CHAOS	16	//カオス、劣化、テレポ系など
#define RUBIAS_SPACE	17	//時空系、テレポ系など
#define RUBIAS_SUMMON	18	//召喚全般、テレポ系など
#define RUBIAS_INSANITY	19	//精神攻撃、エルドリッチホラー、悪魔召喚など
#define RUBIAS_MANA	20	//魔力属性、光の剣など　
#define RUBIAS_MULTI_MAGIC	21	//複合属性魔法
#define RUBIAS_MULTI_BREATH 22	//複合属性ブレス
#define RUBIAS_MAX		22

/*:::ランダムユニーク生成　スキルの付与されやすさを決定する表*/

struct random_unique_skilltype
{
	s16b	chance;	//基礎付与確率 全てのスキルのchance値を合計して合計値のランダム値から全てのスキルのchance値を順に引いていって負になったところで決める
	s16b	minlev;	//このスキルが付与されるための基準レベル。シンボルとの相性で増減。基本レベルを下回ると付与されにくくなり半分で絶対付与されない。
	s16b	maxlev;	//このスキルが付与される基準上限レベル。このレベルを超えると付与されにくくなり倍になると絶対付与されない。

	s16b	bias1;	//今生成しようとしているモンスターの暫定バイアスがここに一致するとスキルの付与確率が増減する 対象なしは0
	s16b	bias1_mult;//暫定バイアスがbias1に該当したときの基礎付与確率への修正 %単位で100が現状維持
	s16b	bias2;
	s16b	bias2_mult;
	s16b	bias3;
	s16b	bias3_mult;
	s16b	bias4;
	s16b	bias4_mult;
	s16b	bias5;
	s16b	bias5_mult;
	cptr	char_suitable;	//このスキルを得やすい敵シンボル 基礎付与確率が3倍され付与最低レベルが-10
	cptr	char_unsuitable;	//このスキルを得にくい敵シンボル 基礎付与確率が1/3され付与最低レベルが+10

};
typedef struct random_unique_skilltype random_unique_skilltype;



//v1.1.42
//クエストにおいて夢の世界の＠となるランダムユニークを生成するときの生成バイアス値を決める。
//基本的に職業と第一魔法領域で決まる。
static int	set_random_unique_bias_of_your_dream(void)
{
	int bias_num = RUBIAS_NONE;

	bool flag_refer_realm = FALSE;

	switch (p_ptr->pclass)
	{
	case	CLASS_WARRIOR:
		if (one_in_(3))	bias_num = RUBIAS_TOUGHNESS;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_SAMURAI:
	case	CLASS_CAVALRY:
		if (one_in_(3))	bias_num = RUBIAS_SPEED;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_TEACHER:
		if (one_in_(2))	flag_refer_realm = TRUE;
		else 	bias_num = RUBIAS_POWER;
		break;

	case	CLASS_MAGIC_KNIGHT:
	case	CLASS_MAID:
	case	CLASS_SYUGEN:
		if (one_in_(3))	bias_num = RUBIAS_SPEED;
		else if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		flag_refer_realm = TRUE;
		break;

	case	CLASS_CIVILIAN:
		if (one_in_(4))	flag_refer_realm = TRUE;
		break;

	case	CLASS_MAGE:
	case	CLASS_HIGH_MAGE:
	case	CLASS_PRIEST:
	case	CLASS_JEWELER:
		flag_refer_realm = TRUE;
		break;

	case	CLASS_ROGUE:
		if (one_in_(3))	bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_RANGER:
		if (one_in_(3))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_PALADIN:
		if (one_in_(4))	bias_num = RUBIAS_HOLY;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_MARTIAL_ARTIST:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_MINDCRAFTER:
		if (one_in_(2))	bias_num = RUBIAS_MANA;
		else		bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_ARCHER:
	case	CLASS_ENGINEER:
	case	CLASS_SOLDIER:
		if (one_in_(3))	bias_num = RUBIAS_SPEED;
		else bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_MAGIC_EATER:
	case	CLASS_LIBRARIAN:
		bias_num = RUBIAS_MULTI_MAGIC;
		break;

	case	CLASS_CHEMIST:
		bias_num = RUBIAS_POIS;
		break;

	case	CLASS_TSUKUMO_MASTER:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_CURSE;
		break;

	case	CLASS_NINJA:
		if (one_in_(3))	bias_num = RUBIAS_POWER;
		bias_num = RUBIAS_SPEED;
		break;


	case	CLASS_RUMIA:
		if (one_in_(3))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_DEMONIC;
		break;

	case	CLASS_YOUMU:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_TEWI:
		if (one_in_(3))	bias_num = RUBIAS_SPEED;
		else bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_UDONGE:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_INSANITY;
		break;

	case	CLASS_IKU:
		bias_num = RUBIAS_ELEC;
		break;

	case	CLASS_KOMACHI:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else if (one_in_(2))	bias_num = RUBIAS_NETH;
		else		bias_num = RUBIAS_WATE;
		break;

	case	CLASS_KOGASA:
		if (one_in_(2))	bias_num = RUBIAS_CURSE;
		else		bias_num = RUBIAS_WATE;
		break;

	case	CLASS_KASEN:
		if (one_in_(2))	bias_num = RUBIAS_TOUGHNESS;
		else if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_SUIKA:
		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_MARISA:
		if (one_in_(4))	flag_refer_realm = TRUE;
		else if (one_in_(3)) bias_num = RUBIAS_MANA;
		else bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_WAKASAGI:
		bias_num = RUBIAS_WATE;
		break;

	case	CLASS_KOISHI:
		if (one_in_(2))	bias_num = RUBIAS_INSANITY;
		else		bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_MOMIZI:
		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_SEIGA:
		if (one_in_(3))	flag_refer_realm = TRUE;
		else bias_num = RUBIAS_NETH;
		break;

	case	CLASS_CIRNO:
		if(one_in_(2))bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_COLD;
		break;

	case	CLASS_ORIN:
		if (one_in_(2))bias_num = RUBIAS_SPEED;
		else bias_num = RUBIAS_NETH;
		break;

	case	CLASS_SHINMYOUMARU:
		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_NAZRIN:
		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_LETTY:
		bias_num = RUBIAS_COLD;
		break;

	case	CLASS_YOSHIKA:
		if (one_in_(4))	bias_num = RUBIAS_NETH;
		else bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_PATCHOULI:
		if (one_in_(2))	flag_refer_realm = TRUE;
		else	bias_num = RUBIAS_MULTI_MAGIC;
		break;

	case	CLASS_AYA:
		if (one_in_(5))	flag_refer_realm = TRUE;
		else if(one_in_(2)) bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_BANKI:
		bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_MYSTIA:
		bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_FLAN:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else if (one_in_(2))	bias_num = RUBIAS_MANA;
		else		bias_num = RUBIAS_CHAOS;
		break;

	case	CLASS_SHOU:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_HOLY;
		break;

	case	CLASS_MEDICINE:
		bias_num = RUBIAS_POIS;
		break;

	case	CLASS_YUYUKO:
		bias_num = RUBIAS_NETH;
		break;

	case	CLASS_SATORI:
		if (one_in_(2))	bias_num = RUBIAS_MULTI_MAGIC;
		else		bias_num = RUBIAS_INSANITY;
		break;

	case	CLASS_KYOUKO:
		bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_TOZIKO:
		if (one_in_(3))	bias_num = RUBIAS_CURSE;
		else bias_num = RUBIAS_ELEC;
		break;

	case	CLASS_LILY_WHITE:
		if (one_in_(2))	bias_num = RUBIAS_SPEED;
		else bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_KISUME:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_HATATE:
		if (one_in_(2))	flag_refer_realm = TRUE;
		else	bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_MIKO:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		bias_num = RUBIAS_MANA;
		break;

	case	CLASS_KOKORO:
		bias_num = RUBIAS_MULTI_MAGIC;
		break;

	case	CLASS_WRIGGLE:
		if (one_in_(2))
			bias_num = RUBIAS_FIRE;
		else
			bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_YUUKA:
		if (one_in_(4))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_CHEN:
		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_MURASA:
		if (one_in_(4))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_WATE;
		break;

	case	CLASS_KEINE:
		if (one_in_(2))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_MANA;
		break;

	case	CLASS_REIMU:
		if (one_in_(3))	bias_num = RUBIAS_TOUGHNESS;
		else if (one_in_(2))	bias_num = RUBIAS_POWER;
		else bias_num = RUBIAS_HOLY;
		break;

	case	CLASS_YUGI:
		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_KAGEROU:
		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_SHIZUHA:
		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_SANAE:
		bias_num = RUBIAS_MULTI_MAGIC;
		break;

	case	CLASS_MINORIKO:
		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_REMY:
		if (one_in_(6))	bias_num = RUBIAS_MANA;
		else if (one_in_(2))	bias_num = RUBIAS_POWER;
		else	flag_refer_realm = TRUE;
		break;

	case	CLASS_NITORI:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_BYAKUREN:
		if (one_in_(2))	bias_num = RUBIAS_HOLY;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_SUWAKO:
		if (one_in_(2))	bias_num = RUBIAS_CURSE;
		else		bias_num = RUBIAS_BILE;
		break;

	case	CLASS_NUE:
		if (one_in_(3))	flag_refer_realm = TRUE;
		else if (one_in_(2))	bias_num = RUBIAS_CHAOS;
		else		bias_num = RUBIAS_MULTI_BREATH;
		break;

	case	CLASS_SEIJA:
		bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_UTSUHO:
		if (one_in_(3))	bias_num = RUBIAS_POWER;
		else if (one_in_(2))	bias_num = RUBIAS_TOUGHNESS;
		bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_YAMAME:
		bias_num = RUBIAS_POIS;
		break;

	case	CLASS_RAN:
		if (one_in_(3))	flag_refer_realm = TRUE;
		else if (one_in_(2))	bias_num = RUBIAS_TOUGHNESS;
		else bias_num = RUBIAS_MANA;
		break;

	case	CLASS_EIKI:
		if (one_in_(3))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_MANA;
		break;

	case	CLASS_TENSHI:
		if (one_in_(5))	flag_refer_realm = TRUE;
		else if (one_in_(4))	bias_num = RUBIAS_POWER;
		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_MEIRIN:
		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_PARSEE:
		bias_num = RUBIAS_CURSE;
		break;

	case	CLASS_SHINMYOU_2:
		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_SUMIREKO:
		if (one_in_(2))	bias_num = RUBIAS_MANA;
		else bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_ICHIRIN:
		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_MOKOU:
		if (one_in_(3)) bias_num = RUBIAS_FIRE;
		else bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_KANAKO:
		if (one_in_(4))	flag_refer_realm = TRUE;
		else if (one_in_(3))	bias_num = RUBIAS_POWER;
		else if (one_in_(2))	bias_num = RUBIAS_TOUGHNESS;
		bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_FUTO:
		if (one_in_(2))	flag_refer_realm = TRUE;
		else bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_SUNNY:
		bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_LUNAR:
		if (one_in_(2))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_DEMONIC;
		break;

	case	CLASS_STAR:
		bias_num = RUBIAS_MANA;
		break;

	case	CLASS_ALICE:
		if (one_in_(2))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_PHYSICAL;
		break;

	case	CLASS_LUNASA:
	case	CLASS_MERLIN:
	case	CLASS_LYRICA:
		bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_CLOWNPIECE:
	case CLASS_VFS_CLOWNPIECE:
		if (one_in_(6))	bias_num = RUBIAS_NETH;
		else if (one_in_(2))	bias_num = RUBIAS_INSANITY;
		else		bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_BENBEN:
	case	CLASS_YATSUHASHI:
		if (one_in_(2))	flag_refer_realm = TRUE;
		bias_num = RUBIAS_SOUND;
		break;

	case	CLASS_HINA:
		bias_num = RUBIAS_CURSE;
		break;

	case	CLASS_SAKUYA:
		if (one_in_(2))	bias_num = RUBIAS_SPEED;
		else		bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_3_FAIRIES:
		if (one_in_(3))	bias_num = RUBIAS_FIRE;
		else if (one_in_(2))	bias_num = RUBIAS_DEMONIC;
		else		bias_num = RUBIAS_MANA;
		break;

	case	CLASS_RAIKO:
		if (one_in_(3))	bias_num = RUBIAS_ELEC;
		else if (one_in_(2))	bias_num = RUBIAS_SOUND;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_MAMIZOU:
		if (one_in_(3))	bias_num = RUBIAS_CHAOS;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_YUKARI:
		if (one_in_(4))	bias_num = RUBIAS_CHAOS;
		else if (one_in_(3))	bias_num = RUBIAS_SPACE;
		else if (one_in_(2))	bias_num = RUBIAS_MULTI_BREATH;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_RINGO:
		if (one_in_(2))	bias_num = RUBIAS_PHYSICAL;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_SEIRAN:
		if (one_in_(2))	bias_num = RUBIAS_PHYSICAL;
		else		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_EIRIN:
		if (one_in_(4))	bias_num = RUBIAS_SPACE;
		else if (one_in_(3))	bias_num = RUBIAS_MANA;
		else if (one_in_(2))	bias_num = RUBIAS_POIS;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_KAGUYA:
		if (one_in_(2))	bias_num = RUBIAS_MULTI_MAGIC;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_SAGUME:
		if (one_in_(3))	bias_num = RUBIAS_MULTI_MAGIC;
		else if (one_in_(2))	bias_num = RUBIAS_SPACE;
		else		bias_num = RUBIAS_CURSE;
		break;

	case	CLASS_REISEN2:
		if (one_in_(2))	bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_TOYOHIME:
		bias_num = RUBIAS_SPACE;
		break;

	case	CLASS_YORIHIME:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_MULTI_MAGIC;
		break;

	case	CLASS_JUNKO:
		if (one_in_(2))	bias_num = RUBIAS_MANA;
		else		bias_num = RUBIAS_INSANITY;
		break;

	case	CLASS_HECATIA:
		if (one_in_(6))	bias_num = RUBIAS_NETH;
		else if (one_in_(2))	bias_num = RUBIAS_MULTI_MAGIC;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_NEMUNO:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_AUNN:
		if (one_in_(2))	bias_num = RUBIAS_HOLY;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case	CLASS_NARUMI:
		if (one_in_(3))	flag_refer_realm = TRUE;
		else if (one_in_(2))	bias_num = RUBIAS_HOLY;
		else		bias_num = RUBIAS_FIRE;
		break;

	case	CLASS_KOSUZU:
		if (one_in_(2))	bias_num = RUBIAS_DEMONIC;
		else		bias_num = RUBIAS_POWER;
		break;

	case	CLASS_MAI:
	case CLASS_SATONO:
		if (one_in_(3))	bias_num = RUBIAS_SPACE;
		else		bias_num = RUBIAS_INSANITY;
		break;

	case	CLASS_JYOON:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_SPEED;
		break;

	case	CLASS_SHION:
		if (one_in_(2))	bias_num = RUBIAS_CURSE;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case CLASS_MAYUMI:
		if (one_in_(2))	bias_num = RUBIAS_POWER;
		else		bias_num = RUBIAS_TOUGHNESS;
		break;

	case CLASS_EIKA:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_NETH;
		break;

	case CLASS_KUTAKA:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_HOLY;
		break;

	case CLASS_URUMI:
		if (one_in_(2))	bias_num = RUBIAS_WATE;
		else		bias_num = RUBIAS_POWER;
		break;

	case CLASS_SAKI:
		if (one_in_(4))	bias_num = RUBIAS_PHYSICAL;
		else		bias_num = RUBIAS_POWER;
		break;

	case CLASS_YACHIE:
		if (one_in_(3))	bias_num = RUBIAS_TOUGHNESS;
		else		bias_num = RUBIAS_CURSE;
		break;

	case CLASS_MOMOYO:
		bias_num = RUBIAS_POWER;

		break;

	}


	if (flag_refer_realm)
	{
		switch (p_ptr->realm1)
		{
		case	TV_BOOK_ELEMENT:
			if (one_in_(3))	bias_num = RUBIAS_COLD;
			else if (one_in_(2))	bias_num = RUBIAS_ELEC;
			else		bias_num = RUBIAS_FIRE;
			break;

		case	TV_BOOK_FORESEE:
			if (one_in_(2))	bias_num = RUBIAS_CURSE;
			else		bias_num = RUBIAS_SPACE;
			break;

		case	TV_BOOK_ENCHANT:
			bias_num = RUBIAS_POWER;
			break;

		case	TV_BOOK_SUMMONS:
		case	TV_BOOK_MYSTIC:
			bias_num = RUBIAS_SPACE;
			break;

		case	TV_BOOK_LIFE:
		case	TV_BOOK_PUNISH:
			bias_num = RUBIAS_HOLY;
			break;

		case	TV_BOOK_NATURE:
			if (one_in_(3))	bias_num = RUBIAS_WATE;
			else if (one_in_(2))	bias_num = RUBIAS_ELEC;
			else		bias_num = RUBIAS_TOUGHNESS;
			break;

		case	TV_BOOK_TRANSFORM:
			if (one_in_(3))	bias_num = RUBIAS_POWER;
			else if (one_in_(2))	bias_num = RUBIAS_BILE;
			else		bias_num = RUBIAS_POIS;
			break;

		case	TV_BOOK_DARKNESS:
			if (one_in_(3))	bias_num = RUBIAS_POIS;
			else if (one_in_(2))	bias_num = RUBIAS_INSANITY;
			else		bias_num = RUBIAS_DEMONIC;
			break;

		case	TV_BOOK_NECROMANCY:
			bias_num = RUBIAS_NETH;
			break;

		case	TV_BOOK_CHAOS:
			bias_num = RUBIAS_CHAOS;
			break;



		default:
			bias_num = RUBIAS_MULTI_MAGIC;
			break;

		}
	}


	return bias_num;

}



#define RU_SKILL_TABLE_MAX	150

random_unique_skilltype	r_unique_skill_table[RU_SKILL_TABLE_MAX] ={

	/*:::HPなどパラメータ関係*/

	//0:HP増加
	//v1.1.42 3000→5000
	{5000,1,127,
	RUBIAS_TOUGHNESS,200,RUBIAS_POWER,150,RUBIAS_INSANITY,150,RUBIAS_PHYSICAL,125,RUBIAS_SUMMON,50,
	"glzDHMOPTUVY#","abijrvFGZ*,"},
	//1:攻撃ダイス増加
	//v1.1.42 1000→1500
	{1500,1,127,
	RUBIAS_POWER,250,RUBIAS_TOUGHNESS,150,RUBIAS_SPEED,150,RUBIAS_DEMONIC,150,RUBIAS_SUMMON,70,
	"afghpCDHJMOPRTUV","beiwyFLQ,"},
	//2:AC増加
	{500,1,127,
	RUBIAS_TOUGHNESS,200,RUBIAS_HOLY,150,RUBIAS_SPEED,150,0,0,0,0,
	"acghpBDISV","iwzFQZ,"},
	//3:視界範囲増加
	{300,1,127,
	RUBIAS_TOUGHNESS,50,RUBIAS_POWER,50,0,0,0,0,0,0,
	"entABCZ","ijmzQ"},
	//4:加速増加
	{300,30,127,
	RUBIAS_SPEED,300,RUBIAS_ELEC,150,RUBIAS_SPACE,150,RUBIAS_TOUGHNESS,75,0,0,
	"bfpqrtBCEFV","gim"},
	//5:魔法使用頻度増加
	{200,40,127,
	RUBIAS_MULTI_MAGIC,400,RUBIAS_MULTI_BREATH,250,RUBIAS_POWER,20,RUBIAS_TOUGHNESS,20,RUBIAS_SPEED,50,
	"nptADELQSUVYZ*,","acoCO"},
	//6:護衛付き
	{10,50,127,
	RUBIAS_SUMMON,300,0,0,0,0,0,0,0,0,
	"aikorvEFZ,","gY"},
	/*:::RF2フラグ*/
	//7:RF2_STUPID (7,8は重複しない)
	{30,1,127,
	RUBIAS_TOUGHNESS,300,RUBIAS_POWER,300,RUBIAS_INSANITY,300,RUBIAS_MULTI_MAGIC,0,0,0,
	"acgovyzQ,","efnptADELSUVW"},
	//8:RF2_SMART
	{50,50,127,
	RUBIAS_MULTI_MAGIC,300,RUBIAS_MANA,300,RUBIAS_POWER,50,RUBIAS_TOUGHNESS,50,0,0,
	"ehptuADEFGKLPSUVWY","efnptADELSUVW"},
	//9:RF2_REFLECTING
	{20,70,127,
	RUBIAS_SPACE,300,RUBIAS_MANA,200,RUBIAS_PHYSICAL,200,RUBIAS_TOUGHNESS,200,RUBIAS_POWER,70,
	"ejpstAESUW*","imw"},

	//10:RF2_INSVISIBLE
	{50,15,127,
	RUBIAS_CURSE,300,RUBIAS_NETH,250,RUBIAS_SPEED,150,RUBIAS_TOUGHNESS,50,RUBIAS_POWER,50,
	"ijtuvEFGLQVW#","dkoqADHMOPT"},

	//11:RF2_REGENERATE
	{30,30,127,
	RUBIAS_TOUGHNESS,400,RUBIAS_HOLY,200,RUBIAS_SPACE,150,0,0,0,0,
	"ijmwzADEFLMOSTUVW*","g&"},

	//12:RF2_POWERFUL
	{10,70,127,
	RUBIAS_MULTI_MAGIC,300,RUBIAS_MANA,200,RUBIAS_POWER,150,RUBIAS_PHYSICAL,150,0,0,
	"hptADELOPSUVWY","iovQ"},

	//13:RF2_ELDRITCH_HORROR
	{30,60,127,
	RUBIAS_INSANITY,1000,RUBIAS_DEMONIC,300,RUBIAS_NETH,200,RUBIAS_CURSE,200,RUBIAS_HOLY,10,
	"euszGLSUW","dfopAD"},

	//14:RF2_AURA_FIRE
	{45,30,127,
	RUBIAS_FIRE,500,RUBIAS_DEMONIC,300,RUBIAS_COLD,20,0,0,0,0,
	"gvDEFMSU*,","abcijl"},

	//15:RF2_AURA_ELEC
	{45,30,127,
	RUBIAS_ELEC,500,RUBIAS_FIRE,150,RUBIAS_WATE,20,0,0,0,0,
	"gvDEFMSU*,","abcijl"},

	//16:RF2_PASS_WALL (16,17は重複しない)
	{30,30,127,
	RUBIAS_SPACE,500,RUBIAS_NETH,300,RUBIAS_DEMONIC,200,RUBIAS_MANA,200,RUBIAS_TOUGHNESS,20,
	"ijEGLSUVX","abcdfklmnorsvwzBCDHIJKMOPQT"},

	//17:RF2_KILL_WALL
	{50,40,127,
	RUBIAS_POWER,300,RUBIAS_BILE,300,RUBIAS_MANA,200,RUBIAS_TOUGHNESS,150,0,0,
	"gijwDIKOPUX","eFGQ"},

	//18:RF2_MOVE_BODY (18,22は重複しない)
	{200,10,127,
	RUBIAS_POWER,300,RUBIAS_TOUGHNESS,300,RUBIAS_SUMMON,50,0,0,0,0,
	"dghknopqstuwACDHIKMOPTUW","Q*&"},

	//19:RF2_AURA_COLD
	{45,30,127,
	RUBIAS_COLD,500,RUBIAS_NETH,300,RUBIAS_FIRE,20,0,0,0,0,
	"gvDEFMSU*,","abcijl"},

	//20:RF2_TELE_AFTER
	{20,70,127,
	RUBIAS_SPACE,400,RUBIAS_CHAOS,300,RUBIAS_SPEED,200,RUBIAS_TOUGHNESS,50,RUBIAS_POWER,30,
	"ehptEFGL","ijm"},

	//21:RF2_GIGANTIC
	{100,40,127,
	RUBIAS_TOUGHNESS,600,RUBIAS_POWER,500,RUBIAS_MULTI_BREATH,300,0,0,0,0,
	"gDEHMPU,","fhkopFKLV"},

	//22:RF2_KILL_BODY
	{40,50,127,
	RUBIAS_POWER,300,RUBIAS_TOUGHNESS,300,RUBIAS_SUMMON,50,0,0,0,0,
	"goqstuwCDHIKMOPTUW","Q*&"},

	/*:::RF3フラグ*/
	//23:RF3_NO_FEAR
	{100,60,127,
	RUBIAS_POWER,300,RUBIAS_TOUGHNESS,200,RUBIAS_DEMONIC,200,RUBIAS_CURSE,150,RUBIAS_SUMMON,30,
	"gijmszvADEOPSQUVW*&",""},

	//24:RF3_NO_STUN
	{100,50,127,
	RUBIAS_SOUND,300,RUBIAS_ELEC,200,RUBIAS_WATE,150,RUBIAS_CURSE,150,0,0,
	"acgijmvwGIQW*",""},

	/*:::NO_CONFとNO_SLEEPは別処理で付与*/

	/*:::RF4フラグ*/
	//25:RF4_SHRIEK
	{10,0,50,
	RUBIAS_SOUND,500,RUBIAS_INSANITY,200,RUBIAS_TOUGHNESS,30,0,0,0,0,
	"bdhoquyBCG,","aeijm"},

	//26:RF4_DANMAKU
	{30,0,50,
	0,0,0,0,0,0,0,0,0,0,
	"",""},

	//27:RF4_DISPEL
	{20,50,127,
	RUBIAS_MANA,300,RUBIAS_INSANITY,200,RUBIAS_HOLY,200,RUBIAS_CHAOS,150,0,0,
	"ehpvALSU","ac"},

	//28:RF4_ROCKET
	{10,90,127,
	RUBIAS_PHYSICAL,300,0,0,0,0,0,0,0,0,
	"ghpK","abcdijlmrvwBCDEGHIJQZ"},

	//29:RF4_SHOOT
	{20,0,30,
	RUBIAS_PHYSICAL,300,RUBIAS_SPEED,250,0,0,0,0,0,0,
	"ghnopstK","abcdijlmrvwBCDEGHIJQZ"},

	//30:RF4_BR_HOLY
	{20,70,127,
	RUBIAS_HOLY,300,RUBIAS_MULTI_BREATH,250,RUBIAS_DEMONIC,10,RUBIAS_NETH,10,RUBIAS_CURSE,10,
	"dgqvACDEHMOPSZ*&","opsuyzGLQUVWY"},

	//31:RF4_BR_HELL
	{5,100,127,
	RUBIAS_DEMONIC,400,RUBIAS_NETH,300,RUBIAS_MULTI_BREATH,200,RUBIAS_HOLY,10,0,0,
	"dvDEGLMSUVWZ*&","opsuyzAQ"},

	//32:RF4_BR_AQUA
	{30,40,127,
	RUBIAS_WATE,500,RUBIAS_MULTI_BREATH,300,RUBIAS_FIRE,30,RUBIAS_ELEC,30,0,0,
	"dgijlvDHKMRSZ*&","pBLQV"},

	//33:RF4_BR_ACID
	{40,50,127,
	RUBIAS_BILE,500,RUBIAS_MULTI_BREATH,300,RUBIAS_POIS,200,RUBIAS_WATE,200,0,0,
	"acdgijmvwDEHIJKMOZ*&,","pGQ"},

	//34:RF4_BR_ELEC
	{40,50,127,
	RUBIAS_ELEC,500,RUBIAS_MULTI_BREATH,300,RUBIAS_SPEED,200,RUBIAS_HOLY,200,RUBIAS_WATE,30,
	"gtuvwABDEFHMPSUZ*&","lpGKQ"},

	//35:RF4_BR_FIRE
	{40,50,127,
	RUBIAS_FIRE,500,RUBIAS_MULTI_BREATH,300,RUBIAS_DEMONIC,200,RUBIAS_COLD,30,RUBIAS_WATE,30,
	"dgtCDEFHMOPSUZ*&","lpGKLQVW"},

	//36:RF4_BR_COLD
	{40,50,127,
	RUBIAS_COLD,500,RUBIAS_MULTI_BREATH,300,RUBIAS_NETH,200,RUBIAS_FIRE,30,0,0,
	"dgjuvDEFGHMOPSUVWZ*&","pQ"},

	//37:RF4_BR_POIS
	{30,50,127,
	RUBIAS_BILE,400,RUBIAS_MULTI_BREATH,300,RUBIAS_NETH,200,RUBIAS_DEMONIC,200,RUBIAS_HOLY,10,
	"cdgijmnqruvzCDEGHJMOPRSUVWZ#*&,","pQ"},

	//38:RF4_BR_NETH
	{25,60,127,
	RUBIAS_NETH,500,RUBIAS_MULTI_BREATH,300,RUBIAS_DEMONIC,300,RUBIAS_CURSE,200,RUBIAS_HOLY,10,
	"dsuvzDGLMSUVWZ*&","pAQ"},

	//39:RF4_BR_LITE
	{30,60,127,
	RUBIAS_HOLY,300,RUBIAS_MULTI_BREATH,300,RUBIAS_FIRE,200,RUBIAS_NETH,30,RUBIAS_DEMONIC,30,
	"degvADEFMZ*&","impsuGLQVW"},

	//40:RF4_BR_DARK
	{30,60,127,
	RUBIAS_DEMONIC,500,RUBIAS_MULTI_BREATH,300,RUBIAS_INSANITY,300,RUBIAS_FIRE,50,RUBIAS_HOLY,30,
	"douvDMOSUVWZ*&,","pAQ"},

	//41:RF4_WAVEMOTION
	{5,125,127,
	RUBIAS_PHYSICAL,300,RUBIAS_SPACE,250,RUBIAS_MANA,150,0,0,0,0,
	"g&*","Q"},

	//42:RF4_BR_SOUN
	{25,50,127,
	RUBIAS_SOUND,600,RUBIAS_MULTI_BREATH,300,RUBIAS_PHYSICAL,200,RUBIAS_SPACE,200,0,0,
	"bdgtvBDEHIVZ*&","ijpQ"},

	//43:RF4_BR_CHAO
	{30,60,127,
	RUBIAS_CHAOS,500,RUBIAS_INSANITY,400,RUBIAS_MULTI_BREATH,300,RUBIAS_DEMONIC,200,0,0,
	"uvDGHMSUZ*&","pAQ"},

	//44:RF4_BR_DISE
	{30,60,127,
	RUBIAS_BILE,300,RUBIAS_POIS,300,RUBIAS_CHAOS,200,RUBIAS_MULTI_BREATH,200,0,0,
	"ijmvDHMUZ*&","pQ"},

	//45:RF4_BR_NEXU
	{10,120,127,
	RUBIAS_SPACE,400,RUBIAS_CHAOS,200,RUBIAS_INSANITY,200,RUBIAS_MULTI_BREATH,200,0,0,
	"evDEXZ*&","pQ"},

	//46:RF4_BR_TIME
	{25,70,127,
	RUBIAS_SPACE,500,RUBIAS_CHAOS,200,RUBIAS_MULTI_BREATH,200,0,0,0,0,
	"evDEXZ*&","pQ"},

	//47:RF4_BR_INER
	{30,40,127,
	RUBIAS_BILE,300,RUBIAS_SPACE,200,RUBIAS_POIS,200,RUBIAS_MULTI_BREATH,200,0,0,
	"abcdgijmvwDEHIZ*&","pQ"},

	//48:RF4_BR_GRAV
	{30,40,127,
	RUBIAS_SPACE,300,RUBIAS_PHYSICAL,200,RUBIAS_MULTI_BREATH,200,0,0,0,0,
	"egvBDHMSZ*&","pQ"},

	//49:RF4_BR_SHAR
	{30,60,127,
	RUBIAS_PHYSICAL,400,RUBIAS_COLD,300,RUBIAS_MULTI_BREATH,200,0,0,0,0,
	"cDHIMZ*&","pQ"},

	//50:RF4_BR_PLAS
	{30,60,127,
	RUBIAS_FIRE,300,RUBIAS_ELEC,300,RUBIAS_MULTI_BREATH,300,RUBIAS_SPACE,200,0,0,
	"dvCDEFHMUZ*&","ijlpQ"},

	//51:RF4_BA_FORCE
	{20,70,127,
	RUBIAS_POWER,500,RUBIAS_TOUGHNESS,200,0,0,0,0,0,0,
	"hoptOP","ijmwGQVW"},

	//52:RF4_BR_MANA
	{20,70,127,
	RUBIAS_MANA,300,RUBIAS_MULTI_BREATH,200,0,0,0,0,0,0,
	"dgCDMUZ*&","pQ"},

	//53:RF4_BR_NUKE
	{30,70,127,
	RUBIAS_FIRE,300,RUBIAS_SPACE,300,RUBIAS_MULTI_BREATH,300,0,0,0,0,
	"dvCDEFHMUZ*&","ijlpQ"},

	//54:RF4_BA_CHAO
	{40,50,70,
	RUBIAS_CHAOS,400,RUBIAS_MULTI_MAGIC,300,RUBIAS_BILE,200,RUBIAS_INSANITY,200,0,0,
	"hoptOP","ijmwGQVWZ"},

	//55:RF4_BR_DISI
	{10,70,127,
	RUBIAS_PHYSICAL,300,RUBIAS_MANA,200,RUBIAS_MULTI_BREATH,200,0,0,0,0,
	"gDMUXZ*&,","pQ"},

	/*:::RF5フラグ*/

	//56:RF5_BA_ACID
	{50,30,40,
	RUBIAS_BILE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_POIS,200,RUBIAS_INSANITY,200,0,0,
	"ehijknoptuAEFGKLOPSUVW,","QZ"},

	//57:RF5_BA_ELEC
	{50,27,40,
	RUBIAS_ELEC,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_PHYSICAL,200,0,0,0,0,
	"ghopEFGLOPSUVW","QZ"},

	//58:RF5_BA_FIRE
	{50,25,40,
	RUBIAS_FIRE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,300,0,0,0,0,
	"ghopEFGOPSU","QZ"},

	//59:RF5_BA_COLD
	{50,22,40,
	RUBIAS_COLD,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_NETH,200,0,0,0,0,
	"ghopzEFGLOPSUVW","QZ"},

	//60:RF5_BA_POIS
	{50,18,25,
	RUBIAS_POIS,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,200,RUBIAS_BILE,200,0,0,
	"abchijkmnpsuyzEGIJLORSUVWY,","QZ"},

	//61:RF5_BA_NETH
	{30,50,80,
	RUBIAS_NETH,400,RUBIAS_CURSE,300,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,300,RUBIAS_HOLY,30,
	"ehoptuyzLOPSUVWY","AQZ"},

	//62:RF5_BA_WATE
	{35,40,80,
	RUBIAS_WATE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_INSANITY,200,0,0,0,0,
	"ijloptEFLOPSUY","AQZ"},

	//63:RF5_BA_MANA
	{15,100,127,
	RUBIAS_MANA,300,RUBIAS_MULTI_MAGIC,200,0,0,0,0,0,0,
	"ptAELSUVWY","QZ"},

	//64:RF5_BA_DARK
	{25,70,127,
	RUBIAS_DEMONIC,300,RUBIAS_MULTI_MAGIC,200,RUBIAS_CURSE,200,RUBIAS_INSANITY,200,RUBIAS_HOLY,50,
	"hnoptGLSUVWY","AQZ"},

	//65:RF5_DRAIN_MANA
	{35,20,40,
	RUBIAS_CURSE,400,RUBIAS_DEMONIC,300,RUBIAS_NETH,300,RUBIAS_INSANITY,200,0,0,
	"hnpGLSUVWY","vAQZ"},

	//66:RF5_MIND_BLAST
	{45,30,35,
	RUBIAS_CURSE,500,RUBIAS_INSANITY,400,RUBIAS_DEMONIC,300,RUBIAS_NETH,200,0,0,
	"ehpstuGLSUVWY","vZ"},

	//67:RF5_BRAIN_SMASH
	{30,50,127,
	RUBIAS_CURSE,500,RUBIAS_INSANITY,400,RUBIAS_DEMONIC,300,RUBIAS_NETH,200,0,0,
	"ehpstuGLSUVWY","vZ"},

	//68:RF5_CAUSE_1
	{50,3,20,
	RUBIAS_CURSE,500,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_NETH,200,RUBIAS_HOLY,50,
	"ehkopstuyzGLOPSUVWY","vAQZ"},

	//69:RF5_CAUSE_2
	{50,15,30,
	RUBIAS_CURSE,500,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_NETH,200,RUBIAS_HOLY,50,
	"ehkopstuyzGLOPSUVWY","vAQZ"},

	//70:RF5_CAUSE_3
	{50,25,50,
	RUBIAS_CURSE,500,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_NETH,200,RUBIAS_HOLY,50,
	"ehkopstuyzGLOPSUVWY","vAQZ"},

	//71:RF5_CAUSE_4
	{50,40,60,
	RUBIAS_CURSE,500,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_NETH,200,RUBIAS_HOLY,50,
	"ehkopstuyzGLOPSUVWY","vAQZ"},

	//72:RF5_BO_ACID
	{70,15,25,
	RUBIAS_BILE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_POIS,200,RUBIAS_INSANITY,200,0,0,
	"ehijknoptuAEFGKLOPSUVW","QZ"},

	//73:RF5_BO_ELEC
	{70,12,25,
	RUBIAS_ELEC,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_PHYSICAL,200,0,0,0,0,
	"ghopEFGLOPSUVW","QZ"},

	//74:RF5_BO_FIRE
	{70,10,25,
	RUBIAS_FIRE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,300,0,0,0,0,
	"ghopEFGOPSU","QZ"},

	//75:RF5_BO_COLD
	{70,10,25,
	RUBIAS_COLD,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_NETH,200,0,0,0,0,
	"ghopszEFGLOPSUVW","QZ"},

	//76:RF5_BA_LITE
	{25,70,127,
	RUBIAS_HOLY,300,RUBIAS_MULTI_MAGIC,200,RUBIAS_FIRE,200,RUBIAS_DEMONIC,10,RUBIAS_NETH,10,
	"hpAFS","mosuyGQTUVWZ"},

	//77:RF5_BO_NETH
	{40,35,60,
	RUBIAS_NETH,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,300,RUBIAS_INSANITY,200,RUBIAS_HOLY,30,
	"pstuyLSUVWY","vAQZ"},

	//78:RF5_BO_WATE
	{45,30,60,
	RUBIAS_WATE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_INSANITY,200,0,0,0,0,
	"ijloptEFLOPSUY","AQZ"},

	//79:RF5_BO_MANA
	{25,45,127,
	RUBIAS_MANA,300,RUBIAS_MULTI_MAGIC,200,0,0,0,0,0,0,
	"ptAELSUVWY","QZ"},

	//80:RF5_BO_PLAS
	{35,40,100,
	RUBIAS_FIRE,300,RUBIAS_MULTI_MAGIC,300,RUBIAS_ELEC,300,RUBIAS_WATE,30,0,0,
	"ghptuAEFLSU","ijlmKQZ"},

	//81:RF5_BO_ICEE
	{35,40,60,
	RUBIAS_COLD,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_NETH,200,RUBIAS_FIRE,30,0,0,
	"ghptuEFLSU","QZ"},

	//82:RF5_MISSILE
	{100,0,20,
	0,0,0,0,0,0,0,0,0,0,
	"p","QZ"},

	//83:RF5_SCARE
	{100,0,50,
	RUBIAS_CURSE,300,RUBIAS_NETH,200,RUBIAS_INSANITY,200,0,0,0,0,
	"epuyGLSUVWY","QZ"},

	//84:RF5_BLIND
	{100,10,60,
	RUBIAS_CURSE,300,RUBIAS_NETH,200,RUBIAS_INSANITY,200,0,0,0,0,
	"epuyGLSUVWY","QZ"},

	//85:RF5_CONF
	{100,15,70,
	RUBIAS_CURSE,300,RUBIAS_NETH,200,RUBIAS_INSANITY,200,0,0,0,0,
	"epuyGLSUVWY","QZ"},

	//86:RF5_SLOW
	{100,20,70,
	RUBIAS_CURSE,300,RUBIAS_NETH,200,RUBIAS_INSANITY,200,0,0,0,0,
	"epuyGLSUVWY","QZ"},

	//87:RF5_HOLD
	{100,30,70,
	RUBIAS_CURSE,300,RUBIAS_NETH,200,RUBIAS_INSANITY,200,0,0,0,0,
	"epuyGLSUVWY","QZ"},

	/*:::RF6フラグ*/

	//88:RF6_HASTE
	{30,50,127,
	RUBIAS_SPEED,500,RUBIAS_SPACE,400,RUBIAS_ELEC,200,0,0,0,0,
	"fptBLSUV","imQ"},

	//89:RF6_HAND_DOOM
	{25,80,127,
	RUBIAS_CURSE,500,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_NETH,200,0,0,
	"ehptGLSUVWY","vQZ"},

	//90:RF6_HEAL
	{40,30,127,
	RUBIAS_HOLY,500,RUBIAS_TOUGHNESS,200,RUBIAS_DEMONIC,30,RUBIAS_INSANITY,30,RUBIAS_NETH,30,
	"hptAEFSTVY","suvzQWZ"},

	//91:RF6_INVULNER
	{20,90,127,
	RUBIAS_MANA,300,RUBIAS_HOLY,300,RUBIAS_TOUGHNESS,200,RUBIAS_SPACE,200,RUBIAS_MULTI_MAGIC,200,
	"hptAELSU*","vQZ"},

	//92:RF6_BLINK
	{150,0,127,
	RUBIAS_SPACE,400,RUBIAS_CHAOS,300,RUBIAS_SUMMON,300,RUBIAS_MULTI_MAGIC,200,RUBIAS_TOUGHNESS,50,
	"hopqrtuyEFLQSUVWY","gm"},

	//93:RF6_TPORT
	{150,10,127,
	RUBIAS_SPACE,400,RUBIAS_CHAOS,300,RUBIAS_SUMMON,300,RUBIAS_MULTI_MAGIC,200,RUBIAS_TOUGHNESS,30,
	"hopqrtuyEFLQSUVWY","gm"},

	//94:RF6_WORLD
	{2,110,127,
	RUBIAS_SPACE,1000,0,0,0,0,0,0,0,0,
	"",""},

	//95:RF6_TELE_TO
	{30,40,127,
	RUBIAS_SPACE,300,RUBIAS_CHAOS,300,RUBIAS_POWER,200,0,0,0,0,
	"ghptuyFKOPQSUY",""},

	//96:RF6_TELE_AWAY
	{40,50,127,
	RUBIAS_SPACE,300,RUBIAS_CHAOS,300,RUBIAS_SUMMON,200,0,0,0,0,
	"hptuyFKPQSUY",""},

	//97:RF6_PSY_SPEAR
	{20,60,127,
	RUBIAS_MANA,300,RUBIAS_HOLY,250,RUBIAS_POWER,200,RUBIAS_MULTI_MAGIC,200,0,0,
	"hptALSUY*","vQZ"},
/* 番号付け間違えて面倒なので消してしまう
	//98:RF6_DARKNESS
	{50,0,50,
	RUBIAS_DEMONIC,300,RUBIAS_NETH,250,RUBIAS_CURSE,250,RUBIAS_INSANITY,200,0,0,
	"behijknopstuyzGLTUVWY","vQZ"},
*/
	//98:RF6_TRAPS
	{50,15,50,
	RUBIAS_SPEED,300,RUBIAS_CURSE,300,RUBIAS_PHYSICAL,200,0,0,0,0,
	"hknoptuyWY","vQZ"},

	//99:RF6_FORGET
	{50,15,50,
	RUBIAS_DEMONIC,300,RUBIAS_CURSE,300,RUBIAS_INSANITY,200,0,0,0,0,
	"ehknoptuyLWUY","vQZ"},

	//100:RF6_COSMIC_HORROR
	{12,90,127,
	RUBIAS_INSANITY,500,RUBIAS_DEMONIC,300,RUBIAS_CURSE,200,RUBIAS_NETH,200,0,0,
	"eGLSU","vQZ"},

	//101:RF6_S_KIN
	{25,60,127,
	RUBIAS_SUMMON,400,RUBIAS_DEMONIC,300,0,0,0,0,0,0,
	"ptQU",""},

	//102:RF6_S_CYBER
	{10,90,127,
	RUBIAS_SUMMON,400,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,0,0,0,0,
	"QU&",""},

	//103:RF6_S_MONSTER
	{60,15,50,
	RUBIAS_SUMMON,500,0,0,0,0,0,0,0,0,
	"Q&",""},

	//104:RF6_S_MONSTERS
	{30,50,127,
	RUBIAS_SUMMON,500,0,0,0,0,0,0,0,0,
	"Q&",""},

	//105:RF6_S_ANT
	{40,20,30,
	RUBIAS_SUMMON,200,0,0,0,0,0,0,0,0,
	"acIQ&",""},

	//106:RF6_S_SPIDER
	{40,20,50,
	RUBIAS_SUMMON,200,0,0,0,0,0,0,0,0,
	"acIQ&",""},

	//107:RF6_S_HOUND
	{30,50,127,
	RUBIAS_SUMMON,200,0,0,0,0,0,0,0,0,
	"CQZ&",""},

	//108:RF6_S_HYDRA
	{20,50,127,
	RUBIAS_SUMMON,200,RUBIAS_BILE,200,RUBIAS_FIRE,200,0,0,0,0,
	"nMQRZ&",""},

	//109:RF6_S_ANGEL
	{20,50,127,
	RUBIAS_HOLY,500,RUBIAS_SUMMON,200,RUBIAS_DEMONIC,10,RUBIAS_NETH,10,RUBIAS_INSANITY,10,
	"pA","ostuyzGLOTUVWY"},

	//110:RF6_S_DEMON
	{30,30,50,
	RUBIAS_SUMMON,400,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_HOLY,20,0,0,
	"opuQSU&","A"},

	//111:RF6_S_UNDEAD
	{40,20,50,
	RUBIAS_SUMMON,400,RUBIAS_NETH,400,RUBIAS_INSANITY,300,RUBIAS_DEMONIC,250,RUBIAS_HOLY,10,
	"hkopstuyzGLQSUVWY&","A"},

	//112:RF6_S_DRAGON
	{35,30,60,
	RUBIAS_SUMMON,400,RUBIAS_MULTI_MAGIC,250,0,0,0,0,0,0,
	"dpDMQS&",""},

	//113:RF6_S_HI_UNDEAD
	{20,70,127,
	RUBIAS_SUMMON,400,RUBIAS_NETH,400,RUBIAS_INSANITY,300,RUBIAS_DEMONIC,250,RUBIAS_HOLY,10,
	"hkopstuyzGLQSUVWY&","A"},

	//114:RF6_S_HI_DRAGON
	{15,80,127,
	RUBIAS_SUMMON,400,RUBIAS_MULTI_MAGIC,250,0,0,0,0,0,0,
	"pDMQS&",""},

	//115:RF6_S_AMBERITES
	{10,90,127,
	RUBIAS_SUMMON,300,0,0,0,0,0,0,0,0,
	"p",""},

	//116:RF6_S_UNIQUE
	{10,85,127,
	RUBIAS_SUMMON,300,0,0,0,0,0,0,0,0,
	"",""},

	/*:::RF9フラグ*/


	//117:RF9_FIRE_STORM
	{30,50,127,
	RUBIAS_FIRE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_DEMONIC,300,RUBIAS_COLD,50,0,0,
	"ghptuAEFLSUY*","vQZ"},

	//118:RF9_ICE_STORM
	{30,50,127,
	RUBIAS_COLD,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_NETH,300,RUBIAS_FIRE,50,0,0,
	"hinoptuEFGLSUVWY*","vQZ"},

	//119:RF9_THUNDER_STORM
	{30,50,127,
	RUBIAS_ELEC,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_SPEED,300,RUBIAS_PHYSICAL,250,RUBIAS_WATE,50,
	"ghptAEFLSUWY*","lvQZ"},

	//120:RF9_ACID_STORM
	{30,50,127,
	RUBIAS_BILE,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_INSANITY,300,RUBIAS_WATE,200,0,0,
	"ghijknoptuyELSUWY*","vQZ"},

	//121:RF9_TOXIC_STORM
	{30,50,127,
	RUBIAS_POIS,500,RUBIAS_MULTI_MAGIC,300,RUBIAS_INSANITY,300,RUBIAS_DEMONIC,300,0,0,
	"ghijknoptuyELRSUVWY*,","vQZ"},

	//122:RF9_BA_POLLUTE
	{30,60,127,
	RUBIAS_BILE,400,RUBIAS_MULTI_MAGIC,300,RUBIAS_INSANITY,300,RUBIAS_POIS,300,0,0,
	"ghijknoptuyELRSUVWY*","vQZ"},

	//123:RF9_BA_DISI
	{20,80,127,
	RUBIAS_PHYSICAL,300,RUBIAS_MULTI_MAGIC,250,RUBIAS_SPACE,200,0,0,0,0,
	"gptFSUY*","vQZ"},

	//124:RF9_BA_HOLY
	{25,60,80,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF*","ostuvyzKLOTUVWYZ"},

	//125:RF9_BA_METEOR
	{10,90,127,
	RUBIAS_SPACE,400,RUBIAS_MULTI_MAGIC,200,RUBIAS_CHAOS,200,0,0,0,0,
	"pASUY","vQZ"},

	//126:RF9_BA_DISTORTION
	{20,80,127,
	RUBIAS_SPACE,400,RUBIAS_CHAOS,300,RUBIAS_MULTI_MAGIC,200,RUBIAS_INSANITY,200,0,0,
	"pLSUY*","vQZ"},

	//127:RF9_PUNISH_1
	{75,0,20,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF","ostuvyzKLOTUVWYZ"},

	//128:RF9_PUNISH_2
	{60,15,30,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF","ostuvyzKLOTUVWYZ"},

	//129:RF9_PUNISH_3
	{50,30,50,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF","ostuvyzKLOTUVWYZ"},

	//130:RF9_PUNISH_4
	{35,50,80,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF","ostuvyzKLOTUVWYZ"},

	//131:RF9_BO_HOLY
	{40,40,80,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,20,RUBIAS_INSANITY,20,RUBIAS_NETH,20,RUBIAS_CURSE,20,
	"pAF","ostuvyzKLOTUVWYZ"},

	//132:RF9_GIGANTIC_LASER
	{20,90,127,
	RUBIAS_FIRE,300,RUBIAS_ELEC,250,RUBIAS_MULTI_MAGIC,300,RUBIAS_COLD,50,RUBIAS_NETH,30,
	"ghptADESUY","ijosvzVQZ"},

	//133:RF9_BO_SOUND
	{50,30,50,
	RUBIAS_SOUND,500,RUBIAS_PHYSICAL,300,0,0,0,0,0,0,
	"bpBEFKSUVW","vQZ"},

	//134:RF9_S_ANIMAL
	{30,50,80,
	RUBIAS_SUMMON,400,0,0,0,0,0,0,0,0,
	"fhkrtBFEHMOPS&",""},

	//135:RF9_BEAM_LITE
	{50,30,90,
	RUBIAS_FIRE,300,RUBIAS_MULTI_MAGIC,300,RUBIAS_HOLY,200,RUBIAS_COLD,50,RUBIAS_NETH,30,
	"ghptADESUY","ijosvzVQZ"},

	//136:RF9_HELLFIRE
	{5,100,127,
	RUBIAS_DEMONIC,500,RUBIAS_NETH,300,RUBIAS_HOLY,0,0,0,0,0,
	"pLUY","AvQZ"},

	//137:RF9_HOLYFIRE
	{5,100,127,
	RUBIAS_HOLY,500,RUBIAS_DEMONIC,0,RUBIAS_INSANITY,0,RUBIAS_NETH,0,RUBIAS_CURSE,0,
	"pAF*","ostuvyzKLOTUVWYZ"},

	//138:RF9_TORNADO
	{50,30,70,
	RUBIAS_SPACE,200,RUBIAS_MULTI_MAGIC,200,0,0,0,0,0,0,
	"pbtvABEFSY","QZ"},

	//139:RF9_S_DEITY
	{20,60,127,
	RUBIAS_SUMMON,400,RUBIAS_INSANITY,300,0,0,0,0,0,0,
	"pS","Z"},

	//140:RF9_S_HI_DEMON
	{20,90,127,
	RUBIAS_SUMMON,400,RUBIAS_DEMONIC,400,RUBIAS_INSANITY,300,RUBIAS_HOLY,20,0,0,
	"pQSU&","A"},

	//141:RF9_S_KWAI
	{40,40,50,
	RUBIAS_SUMMON,400,0,0,0,0,RUBIAS_HOLY,40,0,0,
	"pyQSY&","A"},

	//142:RF9_TELE_APPROACH
	{50,30,127,
	RUBIAS_SPEED,500,RUBIAS_CHAOS,300,RUBIAS_SPACE,300,0,0,0,0,
	"hprtBCEV","ijmQ&"},

	//143:RF9_TELE_HI_APPROACH
	{20,70,127,
	RUBIAS_SPACE,500,RUBIAS_CHAOS,300,RUBIAS_SPEED,200,0,0,0,0,
	"ehprtCEUVY","mZQ&"},

	//144:RF9_MAELSTROM
	{30,70,127,
	RUBIAS_WATE,500,RUBIAS_FIRE,50,0,0,0,0,0,0,
	"lnvEFKS","QZ&"},

	//145:RF9_ALARM
	{10,50,127,
	RUBIAS_SOUND,500,RUBIAS_INSANITY,200,RUBIAS_TOUGHNESS,30,0,0,0,0,
	"bdhoqtuyBCG,","aeijm"},


	/*:::RFR耐性は能力付加処理のあとシンボルとバイアスとレベルに応じてある程度付与される*/

	/*:::特殊魔法セット*/
	//146:元素ボール全て
	{20,30,60,
	RUBIAS_MULTI_MAGIC,500,RUBIAS_MANA,300,0,0,0,0,0,0,
	"ghptuAEFLSUY*","vQZ"},

	//147:元素嵐全て
	{4,75,127,
	RUBIAS_MULTI_MAGIC,800,RUBIAS_MANA,200,0,0,0,0,0,0,
	"ghptuAEFLSUY*","vQZ"},

	//148:元素ブレス全て
	{5,90,127,
	RUBIAS_MULTI_BREATH,600,0,0,0,0,0,0,0,0,
	"vDHMZ",""},

	//149:ショートテレポ・テレポ・テレポアウェイ
	{30,30,127,
	RUBIAS_SPACE,500,RUBIAS_SUMMON,300,RUBIAS_MULTI_MAGIC,300,RUBIAS_TOUGHNESS,30,0,0,
	"hptuEFLQSUVY",""},


	//攻撃属性、効果変更
	//COLD_BLOOD,EMPTY_MIND,WEIRD_MIND,RF3種族フラグは別処理でシンボルごとに付与

};



/*:::ランダムユニーク生成用バイアステーブル 配列添字がRUBIAS_???の値に一致する*/
struct random_unique_biastype
{
	int	freq;		//基本出現率 デフォ100
	byte	base_attr;		//このバイアスをもつモンスターの基本色　万色やシェイプチェンジャーは別処理でフラグ付与
	cptr	common_symbol;	//該当する文字のモンスターはこのバイアスを持ちやすい
	cptr	uncommon_symbol;	//やや持ちにくい
	cptr	rare_symbol;	//滅多に持たない
};
typedef struct random_unique_biastype random_unique_biastype;

/*:::ランダムユニーク生成用バイアステーブル 配列添字がRUBIAS_???に対応するので終端用ダミー不要*/
random_unique_biastype 	r_unique_bias_table[] ={
	{200,TERM_WHITE,	"fhkpyI","EFS","dyDYgQ"}, //NONE
	{200,TERM_L_UMBER,	"dghjklmostwzACDHMOPRTUVW#","aceuBGLX","birvFZ*"}, //TOUGHNESS
	{200,TERM_RED,	"dfghkloqtzACDHKMOPRTUV#","bcmsuGXZ","eijrvFLQ*,"}, //POWER
	{150,TERM_YELLOW,	"abfhkqrtuBC","cAGLPRTVX","gijmswzMQ#*,"}, //SPEED
	{80,TERM_L_DARK,	"acdijlmnpruvwDHIJLMRTUVZ#*,","bgstBCKOX","fkqAFQ"}, //BILE
	{80, TERM_BLUE,	"dfgptuvABDZ*","abcmoqrswzCKLORX","ijlnQTV,"}, //ELEC
	{90,TERM_SLATE,	"dipsuvDLVZ*","abcgjlotwzBCOX","fmnqrJKQRT,"}, //COLD
	{100,TERM_L_RED,	"dptuvADUZ*","abcfgowBCJLOX,","ijlmnqrszGQTV#"}, //FIRE
	{80, TERM_GREEN,	"abcdijlmnruvwzDHIJLMRTVZ#*,","gsBCKX","qtAFQ"}, //POIS
	{90,TERM_VIOLET,	"befhknpsuzCDGHJLMUVZ#*","cilmqwBIKRX","adgjtvADFQ"}, //DEMONIC
	{70, TERM_WHITE,	"hpqAF","cdfijmrwBCDJMRZ*","abeknostuyzGHIKLOQTUVWXY,"}, //HOLY
	{50, TERM_L_BLUE,	"hijlnvwDKMRZ*","acdfquzACX,","bkrsBLOQV"}, //WATE
	{90, TERM_UMBER,	"befmstuzDGLUVWZ*","cdijlwBCIKORX","aqvAFQ"}, //NETH
	{60, TERM_SLATE,	"bcefhlnpstuzGLUW*,","aijmrwCDHIKMORTX","dgqvAFQZ"}, //CURSE
	{50, TERM_ORANGE,	"cghpsIKU*","bdejknqwyzABDJLOVX","filmrtuvCEFGMQRTYZ#"}, //PHYSICAL
	{50, TERM_YELLOW,	"bgktvABCDIVZ*","cdfjqswzJLMORX","imnruQT"}, //SOUND
	{50, TERM_VIOLET,	"cehinpsvwDHMUXZ*,","bdfjltBCKLOQV","gqrAF"}, //CHAOS
	{30, TERM_L_GREEN,	"eghptvDHLUX*","bcfjklnoqsuwyCGIJKMRV","adimrzFOT"}, //SPACE
	{50, TERM_L_UMBER,	"efnpuLQUV","chjloqszBDKR","dgimrvwJOTXZ"}, //SUMMON
	{30, TERM_VIOLET,	"cefilmnsuwzGHLUWX","abhovCJMTV*,","dkqrtADFKOZ"}, //INSANITY
	{30, TERM_WHITE,	"ehnptALUVW*","fijklmoqsyzBDJMX","abcdgrwCHIKOQRTZ#"}, //MANA
	{35, TERM_VIOLET,	"efhnptALUVW*","dijkosvDJKOX","abcglmqrwzBCHIQRTZ#,"}, //MULTI_MAGIC
	{25, TERM_VIOLET,	"jvwDHMZ","acfkmnqrtAFIKLWX,","ehlopszGOQTV*"}, //MULTI_BREATH
};

/*:::ランダムユニーク生成用文字テーブル */
struct random_unique_symboltype
{
	char	mon_char;		//モンスターの文字
	int	freq;		//基本出現率 デフォ100
	int 	base_level_min;	//出現開始階層 この階層を下回ると出にくくなりこの階層の半分になると全く出ない
	int	base_level_max;	//出現終了階層 この階層を上回ると出にくくなり倍になると全く出ない
	int	hd_sides;		//monster_race.hsideになる基本値
	int	power_rate;	//ランダムユニーク生成パワーを決めるときの係数
	int rbm_60;		//60%の確率で出る物理攻撃
	int rbm_30;		//30%の確率で出る物理攻撃
	int rbm_10;		//10%の確率で出る物理攻撃
};
typedef struct random_unique_symboltype random_unique_symboltype;

#define RU_SYMBOL_NUM	54 //ランダムユニークに使われるシンボルの種類

random_unique_symboltype 	r_unique_symbol_table[RU_SYMBOL_NUM] ={
	{'a',20,0,40,43,75,RBM_BITE,RBM_CLAW,RBM_SPIT},
	{'b',20,0,40,49,75,RBM_CLAW,RBM_BITE,RBM_HOWL},
	{'c',40,0,50,42,85,RBM_BITE,RBM_STING,RBM_CRAWL},
	{'d',70,20,50,71,100,RBM_BITE,RBM_CLAW,RBM_CRUSH},
	{'e',30,10,80,55,90,RBM_GAZE,RBM_GAZE,RBM_CRUSH},
	{'f',60,0,70,61,90,RBM_CLAW,RBM_BITE,RBM_KICK},
	{'g',70,20,80,113,80,RBM_PUNCH,RBM_CRUSH,RBM_PRESS},
	{'h',120,5,90,61,100,RBM_PUNCH,RBM_SLASH,RBM_KICK},
	{'i',20,0,25,41,50,RBM_CRAWL,RBM_SPIT,RBM_HIT},
	{'j',40,0,90,81,90,RBM_ENGULF,RBM_DROOL,RBM_HIT},
	{'k',50,0,70,77,85,RBM_CLAW,RBM_KICK,RBM_HOWL},
	{'l',40,16,60,73,90,RBM_CRUSH,RBM_SQUEEZE,RBM_HIT},
	{'m',20,0,40,55,75,RBM_SPORE,RBM_ENGULF,RBM_HIT},
	{'n',30,15,60,61,95,RBM_SPEAR,RBM_SQUEEZE,RBM_PUNCH},
	{'o',40,0,35,73,75,RBM_PUNCH,RBM_CRUSH,RBM_SLASH},
	{'p',150,0,110,75,100,RBM_SLASH,RBM_PUNCH,RBM_SPEAR},
	{'q',20,0,90,81,90,RBM_KICK,RBM_CRUSH,RBM_HOWL},
	{'r',20,0,40,39,75,RBM_BITE,RBM_BITE,RBM_CRUSH},
	{'s',40,5,60,53,95,RBM_PUNCH,RBM_SLASH,RBM_WAIL},
	{'t',25,40,80,84,110,RBM_KICK,RBM_SLASH,RBM_CRUSH},
	{'u',30,20,50,52,85,RBM_CLAW,RBM_STING,RBM_BITE},
	{'v',25,10,80,33,65,RBM_ENGULF,RBM_TOUCH,RBM_FLASH},
	{'w',20,0,70,70,75,RBM_CRUSH, RBM_BITE, RBM_ENGULF},
	{'y',70,0,50,52,80,RBM_CLAW,RBM_PUNCH,RBM_HIT},
	{'z',30,10,50,87,85,RBM_BITE,RBM_CLAW,RBM_WAIL},
	{'A',25,40,120,91,120, RBM_SLASH, RBM_FLASH, RBM_WHISPER},
	{'B',30,0,90,56,85,RBM_CLAW,RBM_CRUSH,RBM_CLAW},
	{'C',40,10,80,90,85,RBM_CRUSH,RBM_BITE,RBM_BITE},
	{'D',60,50,120,100,135,RBM_STRIKE,RBM_BITE,RBM_CRUSH},
	{'E',50,20,90,61,100,RBM_TOUCH,RBM_HIT,RBM_HIT},
	{'F',30,0,40,43,70,RBM_STRIKE,RBM_KICK,RBM_WHISPER},
	{'G',50,20,80,57,95,RBM_TOUCH,RBM_WAIL,RBM_GAZE},
	{'H',20,40,90,93,85,RBM_CRUSH,RBM_CLAW,RBM_BITE},
	{'I',30,20,70,61,75,RBM_BITE,RBM_STING,RBM_SLASH},
	{'J',25,10,50,64,80,RBM_BITE,RBM_SQUEEZE,RBM_SQUEEZE},
	{'K',25,30,70,66,90,RBM_STRIKE,RBM_KICK,RBM_PUNCH},
	{'L',30,60,120,70,110,RBM_TOUCH,RBM_GAZE,RBM_SMILE},
	{'M',15,50,100,97,80,RBM_BITE,RBM_SQUEEZE,RBM_CRUSH},
	{'O',30,30,100,85,80,RBM_PUNCH,RBM_STRIKE,RBM_KICK},
	{'P',25,50,100,100,110,RBM_PUNCH,RBM_PRESS,RBM_CRUSH},
	{'Q',20,40,100,49,75,RBM_CRAWL,RBM_ENGULF,RBM_GAZE},
	{'R',25,10,90,73,85,RBM_BITE,RBM_CLAW,RBM_ENGULF},
	{'S',20,60,120,96,150,RBM_HIT,RBM_HIT,RBM_HIT},
	{'T',30,30,70,94,75,RBM_PUNCH,RBM_CRUSH,RBM_SPIT},
	{'U',35,60,120,93,120,RBM_SLASH,RBM_BUTT,RBM_PRESS},
	{'V',25,50,100,96,110,RBM_SLASH,RBM_BITE,RBM_BITE},
	{'W',20,40,100,63,100,RBM_TOUCH,RBM_SLASH,RBM_SPEAR},
	{'X',5,50,100,66,100,RBM_CRUSH,RBM_CRUSH,RBM_CRUSH},
	{'Y',50,40,100,83,120,RBM_SLASH,RBM_TOUCH,RBM_SMILE},
	{'Z',15,30,70,43,75,RBM_CLAW,RBM_BITE,RBM_CRUSH},
	{'#',10,20,80,73,85,RBM_ENGULF,RBM_SQUEEZE,RBM_CRUSH},
	{'&',5,30,70,80,80,RBM_ENGULF,RBM_BITE,RBM_DRAG},
	{'*',15,20,100,47,90,RBM_CRUSH,RBM_FLASH,RBM_ENGULF},
	{',',20,10,50,41,60,RBM_CRUSH,RBM_SPORE,RBM_SPORE},

};





/*:::ランダムユニークのレベル決定後、シンボルを決定する。ダンジョンによるバイアスを「ある程度」考慮する。*/
/*:::戻り値はr_unique_symbol_table[]の添え字*/
int get_random_unique_symbol(int r_idx, int lev)
{
	int i;
	int chance[RU_SYMBOL_NUM];
	int totalchance = 0;

	//v1.1.86 リリーホワイト生成時は強制的に'F'
	if (flag_generate_lilywhite)
	{
		return 30;
	}


	//v1.1.42 夢の世界の＠はplayer_race[]の情報を参考に決める
	if (p_ptr->inside_quest == QUEST_DREAMDWELLER)
	{
		for (i = 0; i < RU_SYMBOL_NUM; i++)
		{
			if (r_unique_symbol_table[i].mon_char == rp_ptr->random_unique_symbol)
				return i;


		}
		msg_format(_("WARNING:r_unique_symbol_table[].mon_charとrp_ptr->random_unique_symbolが一致しない", "WARNING:r_unique_synbol_table[].mon_char and rp_ptr->random_unique_symbol do not match"));
	}


	for(i=0;i<RU_SYMBOL_NUM;i++)
	{
		int tmp;
		int minlev = r_unique_symbol_table[i].base_level_min;
		int maxlev = r_unique_symbol_table[i].base_level_max;

		tmp = r_unique_symbol_table[i].freq;

		if( lev < minlev) tmp = tmp * MAX(0,(lev - minlev/2)) / MAX(1,(minlev/2)); //シンボル出現最低レベルより低いと出現率減少 最低レベルの半分以下だと0になる
		if( lev > maxlev) tmp = tmp * MAX(0,(maxlev - lev / 2)) / MAX(1,(lev/2)); //シンボル出現高レベルより高いと出現率減少 最高レベルの倍以上だと0になる

		//水棲ランダムユニークの特殊補正
		if(r_idx == MON_RANDOM_UNIQUE_3)
		{
			if(my_strchr("ijlnKM",r_unique_symbol_table[i].mon_char)) tmp *= 5;
			if(my_strchr("adfkoqtCDFOW",r_unique_symbol_table[i].mon_char)) tmp /= 5;
			if(my_strchr("bpABV",r_unique_symbol_table[i].mon_char)) tmp = 0;
		}

		//ダンジョンで出やすい文字のモンスターは一律確率20倍　この勝手版では「出やすい文字の指定」しかしていないはず？
		//一通り実装済むまで処理しないでおく
		//for(j=0;j<5;j++) if( d_info[dungeon_type].r_char[j] && d_info[dungeon_type].r_char[j] == r_unique_symbol_table[i].mon_char) tmp *= 20;

		chance[i] = tmp;
		totalchance += tmp;
	}

	totalchance = randint1(totalchance);

	for(i=0;i<RU_SYMBOL_NUM;i++)
	{
		totalchance -= chance[i];
		if(totalchance <=0) break;
	}

	return i;
}


/*:::ランダムユニークのシンボル決定後、スキル付与バイアスを決定する。スキル付与時にもしばしば上書きされるようにする予定。*/
int get_random_unique_bias(int r_idx, char c)
{
	int i;
	int chance[RUBIAS_MAX];
	int totalchance = 0;

	//v1.1.86
	if (flag_generate_lilywhite)
	{
		if (one_in_(5)) return RUBIAS_SOUND;
		else if (one_in_(4)) return RUBIAS_NONE;
	}

	//v1.1.74 袿姫偶像生成特殊処理
	if (special_idol_generate_type)
	{
		switch (special_idol_generate_type)
		{
		case TR_RES_ACID:
		case TR_RES_DISEN:
			return RUBIAS_BILE;
		case TR_RES_ELEC:
			return RUBIAS_ELEC;
		case TR_RES_FIRE:
			return RUBIAS_FIRE;
		case TR_RES_COLD:
			return RUBIAS_COLD;
		case TR_RES_POIS:
			return RUBIAS_POIS;
		case TR_RES_LITE:
		case TR_RES_HOLY:
			return RUBIAS_HOLY;
		case TR_RES_DARK:
			return RUBIAS_DEMONIC;
		case TR_RES_SHARDS:
			return RUBIAS_PHYSICAL;
		case TR_RES_SOUND:
			return RUBIAS_SOUND;
		case TR_RES_NETHER:
			return RUBIAS_NETH;
		case TR_RES_WATER:
			return RUBIAS_WATE;
		case TR_RES_CHAOS:
			return RUBIAS_CHAOS;
		default:
			return RUBIAS_NONE;
		}


		return i;
	}


	for(i=0;i<RUBIAS_MAX;i++)
	{
		int tmp;
		tmp = r_unique_bias_table[i].freq;
		if(my_strchr(r_unique_bias_table[i].common_symbol,c)) tmp += 200;
		else if(my_strchr(r_unique_bias_table[i].uncommon_symbol,c)) tmp /= 2;
		else if(my_strchr(r_unique_bias_table[i].rare_symbol,c)) tmp /= 10;

		//水棲ランダムユニークの特殊補正
		if(r_idx == MON_RANDOM_UNIQUE_3)
		{
			if(i == RUBIAS_FIRE) tmp /= 20;
			if(i == RUBIAS_ELEC) tmp /= 20;
			if(i == RUBIAS_WATE) tmp *= 10;
			if(i == RUBIAS_INSANITY) tmp *= 3;
		}

		chance[i] = tmp;
		totalchance += tmp;

	}
	totalchance = randint1(totalchance);

	for(i=0;i<RUBIAS_MAX;i++)
	{
		totalchance -= chance[i];
		if(totalchance <=0) break;
	}
	return i;

}


/*:::ランダムユニークにスキルを付与する前に基礎パラメータを付与する。レベルとシンボルとバイアスが影響する。*/
void apply_random_unique_base_param(monster_race *r_ptr,int r_idx, int lev, int symbol_num, int bias, int power)
{
	int atkdicebase;
	byte d_char = r_unique_symbol_table[symbol_num].mon_char;

	/*:::HP,AC,加速など基礎値付与*/
	r_ptr->hdice = randint1(5) + lev / 5;
	r_ptr->hside = r_unique_symbol_table[symbol_num].hd_sides;
	if(bias == RUBIAS_TOUGHNESS) r_ptr->hside += 30;

	r_ptr->ac = 10 + lev/10 * 10;
	if(bias == RUBIAS_TOUGHNESS) r_ptr->ac += 20;

	if(my_strchr("jmvQZ&*", d_char )) r_ptr->sleep =0;
	else if(my_strchr("dgsyzCDEGJKMOPRSTVY",d_char)) r_ptr->sleep = 70 + randint1(30);
	else  r_ptr->sleep = 30 + randint1(70);

	r_ptr->aaf = 20 + randint0(lev /2);

	if(bias == RUBIAS_TOUGHNESS) r_ptr->speed = 107 + MAX(0,(lev-20)/6);
	else if(bias == RUBIAS_SPEED) r_ptr->speed = 115 + MAX(0,(lev-15)/4);
	else r_ptr->speed = 110 + MAX(0,(lev-20)/5);

	r_ptr->mexp = (power+randint1(3)) * lev * (250+randint1(250)) / 100;

	if(bias == RUBIAS_MULTI_MAGIC || bias == RUBIAS_MANA ||  bias == RUBIAS_HOLY  || bias == RUBIAS_CURSE ) r_ptr->freq_spell = 25;
	else r_ptr->freq_spell = 16;

	/*:::フラグリセット*/
	r_ptr->flags1 = (RF1_FORCE_MAXHP | RF1_FORCE_SLEEP | RF1_ONLY_ITEM | RF1_DROP_90 );
	r_ptr->flags2 = (RF2_OPEN_DOOR | RF2_BASH_DOOR);
	r_ptr->flags3 = (RF3_NO_SLEEP);
	r_ptr->flags4 = 0L;
	r_ptr->flags5 = 0L;
	r_ptr->flags6 = 0L;
	r_ptr->flags7 = (RF7_UNIQUE2 | RF7_VARIABLE);
	r_ptr->flags8 = 0L;
	r_ptr->flags9 = 0L;
	r_ptr->flagsr = 0L;

	//ランダムユニーク3は水棲 パラメータ再計算して再出現するとき少し処理が不審な気がするがまあ問題はないだろう？
	///mod150924 闘技場では水棲にならないように修正
	if(r_idx == MON_RANDOM_UNIQUE_3 && !p_ptr->inside_battle)
	{
		if(my_strchr("ijlmnQ",d_char) && !one_in_(4) || one_in_(4)) r_ptr->flags7 |= (RF7_AQUATIC);
		r_ptr->flags7 |= (RF7_CAN_SWIM);
	}

	/*:::命名時の都合により今のところ男は無し。*/
	if(my_strchr("hknptAFKOPV",d_char)) r_ptr->flags1 |= RF1_FEMALE;
	else if(my_strchr("fgoquyzEGLSTUWY",d_char) && one_in_(2) ) r_ptr->flags1 |= RF1_FEMALE;
	else if(one_in_(5))  r_ptr->flags1 |= RF1_FEMALE;

	atkdicebase = 1 + lev / 20;
	if(bias == RUBIAS_POWER) atkdicebase += 2 + lev / 20;

	/*:::攻撃ダイス基礎値*/
	r_ptr->blow[0].method = RBM_HIT;
	r_ptr->blow[0].effect = RBE_HURT;
	r_ptr->blow[0].d_dice = atkdicebase  + randint0(atkdicebase/2);
	r_ptr->blow[0].d_side = atkdicebase  + randint1(atkdicebase);

	r_ptr->blow[1].method = RBM_HIT;
	r_ptr->blow[1].effect = RBE_HURT;
	r_ptr->blow[1].d_dice = atkdicebase  + randint0(atkdicebase/2);
	r_ptr->blow[1].d_side = atkdicebase  + randint1(atkdicebase);

	r_ptr->blow[2].method = RBM_HIT;
	r_ptr->blow[2].effect = RBE_HURT;
	r_ptr->blow[2].d_dice = atkdicebase  + randint0(atkdicebase/2);
	r_ptr->blow[2].d_side = atkdicebase  + randint1(atkdicebase);

	if(lev < (10 + randint1((bias==RUBIAS_POWER)?5:20))) r_ptr->blow[2].method = 0;

	r_ptr->blow[3].method = RBM_HIT;
	r_ptr->blow[3].effect = RBE_HURT;
	r_ptr->blow[3].d_dice = atkdicebase  + randint0(atkdicebase/2);
	r_ptr->blow[3].d_side = atkdicebase  + randint1(atkdicebase);

	if(lev < (30 + randint1((bias==RUBIAS_POWER)?5:20))) r_ptr->blow[3].method = 0;

	r_ptr->next_r_idx = 0;//無意味だが一応
	r_ptr->next_exp = 0;//無意味だが一応

	r_ptr->level = lev;
	r_ptr->rarity = 1; //出現は別処理なので無意味だが一応

	//シンボル適用　色はバイアス上書きに備えてスキル付与後に適用する
	r_ptr->d_char = d_char;
	r_ptr->x_char = d_char;

	//カウンタリセットなど
	r_ptr->r_sights = 0;
	r_ptr->r_deaths = 0;
	r_ptr->r_pkills = 0;
	r_ptr->r_akills = 0;
	r_ptr->r_tkills = 0;

	r_ptr->r_wake = 0;
	r_ptr->r_ignore = 0;

	r_ptr->r_xtra1 = 0;
	r_ptr->r_xtra2 = 0;

	r_ptr->r_drop_gold = 0;
	r_ptr->r_drop_item = 0;
	r_ptr->r_cast_spell = 0;
	r_ptr->r_blows[0] = r_ptr->r_blows[1] = r_ptr->r_blows[2] = r_ptr->r_blows[3] = 0;

	r_ptr->r_flags1 = 0L;
	r_ptr->r_flags2 = 0L;
	r_ptr->r_flags3 = 0L;
	r_ptr->r_flags4 = 0L;
	r_ptr->r_flags5 = 0L;
	r_ptr->r_flags6 = 0L;
	r_ptr->r_flags9 = 0L;
	r_ptr->r_flagsr = 0L;

	/*:::勢力設定*/
	if (flag_generate_lilywhite)
	{
		r_ptr->flags3 |= RF3_GEN_NEUTRAL;
		r_ptr->flags4 |= RF4_DANMAKU;
	}
	else if(my_strchr("A",d_char))
	{
		if(one_in_(100) && bias != RUBIAS_HOLY) r_ptr->flags3 |= RF3_ANG_CHAOS;
		else if(!one_in_(10) && bias != RUBIAS_NETH && bias != RUBIAS_DEMONIC ) r_ptr->flags3 |= RF3_ANG_COSMOS;
	}
	else if(my_strchr("suzGUW",d_char))
	{
		if(!one_in_(10) && bias != RUBIAS_HOLY) r_ptr->flags3 |= RF3_ANG_CHAOS;
		else if(one_in_(10) && bias != RUBIAS_NETH && bias != RUBIAS_DEMONIC ) r_ptr->flags3 |= RF3_ANG_COSMOS;
	}
	else if(my_strchr("eokLOTV",d_char))
	{
		if(!one_in_(4) && bias != RUBIAS_HOLY) r_ptr->flags3 |= RF3_ANG_CHAOS;
		else if(one_in_(4) && bias != RUBIAS_NETH && bias != RUBIAS_DEMONIC ) r_ptr->flags3 |= RF3_ANG_COSMOS;
	}
	else if(my_strchr("dfhnpDEPS",d_char))
	{
		if(one_in_(3) && bias != RUBIAS_HOLY) r_ptr->flags3 |= RF3_ANG_CHAOS;
		else if(one_in_(2) && bias != RUBIAS_NETH && bias != RUBIAS_DEMONIC ) r_ptr->flags3 |= RF3_ANG_COSMOS;
	}
	else
	{
		if(one_in_(10) && bias != RUBIAS_HOLY) r_ptr->flags3 |= RF3_ANG_CHAOS;
		else if(one_in_(9) && bias != RUBIAS_NETH && bias != RUBIAS_DEMONIC ) r_ptr->flags3 |= RF3_ANG_COSMOS;

	}

}

/*:::どうもブレス系が付与されやすすぎる気がするので少し制限　とくに人間型モンスターはブレスを使いにくくする*/
bool check_multi_breath(monster_race *r_ptr, int bias)
{
	int breath_num=0, i;

	/*:::MULTIBREATHは無制限*/
	if(bias == RUBIAS_MULTI_BREATH) return TRUE;

	for(i=0;i<32;i++)
	{
		if((r_ptr->flags4 & 1L<<i) & RF4_BREATH_MASK) breath_num++;
	}

	/*:::沢山ブレス持っててもそう不思議でないシンボル*/
	if(my_strchr("vDEMZ&",r_ptr->d_char))
	{
		if((r_ptr->level / 20 + 1) < breath_num && one_in_(2)) return FALSE;

	}
	/*:::精々2,3のブレスしか吐きそうにないシンボル*/
	else if(my_strchr("abcdfgjlnqruwyBCFHIJOPRSUWXY*,",r_ptr->d_char))
	{
		if(breath_num > (1+r_ptr->level / 30) && !one_in_(3)) return FALSE;

	}
	/*:::ブレスとか吐きそうにないシンボル*/
	else
	{
		if(!one_in_(breath_num * 3 + 5)) return FALSE;
	}
	return TRUE;
}


/*:::ランダムユニークへpowerの回数分スキルを付与する。*/
void apply_random_unique_skill(monster_race *r_ptr, int power, int *bias)
{

	int i,j,powercnt;
	int lev = r_ptr->level;
	int chance[RU_SKILL_TABLE_MAX];
	int totalchance = 0;
	int randomchance;
	int skillnum;

	bool flag_recalc_chance = TRUE; //スキル付与確率再計算フラグ 初回とbias変更時にTRUEになるが今のところbiasは途中で変わらない


	for(powercnt=0;powercnt<power;powercnt++)
	{

		//v1.1.42 夢の世界の＠は1/7の確率でバイアス値が切り替わる
		if ((p_ptr->inside_quest == QUEST_DREAMDWELLER || p_ptr->inside_arena) && one_in_(7))
		{
			*bias = set_random_unique_bias_of_your_dream();
			flag_recalc_chance = TRUE;
			totalchance = 0;
		}

		if(flag_recalc_chance) for(i=0;i<RU_SKILL_TABLE_MAX;i++)
		{
			int tmp;
			int minlev = r_unique_skill_table[i].minlev;
			int maxlev = r_unique_skill_table[i].maxlev;

			tmp = r_unique_skill_table[i].chance;

			/*:::このスキルに向いている/向いていないシンボルの敵へのスキル付与確率とレベル範囲を調整*/
			if(my_strchr(r_unique_skill_table[i].char_suitable ,r_ptr->d_char))
			{
				minlev = MAX(0,minlev-10);
				tmp *= 3;
			}
			else if(my_strchr(r_unique_skill_table[i].char_unsuitable ,r_ptr->d_char))
			{
				minlev +=10;
				maxlev +=10;
				tmp /= 3;
			}

			/*:::レベルによる付与確率調整*/
			if( lev < minlev) tmp = tmp * MAX(0,(lev - minlev/2)) / MAX(1,(minlev/2)); //シンボル出現開始レベルより低いと出現率減少 半分以下だと0になる
			if( lev > maxlev) tmp = tmp * MAX(0,(maxlev - lev / 2)) / MAX(1,(lev/2)); //シンボル出現終了レベルより高いと出現率減少 倍以上だと0になる

			/*:::バイアスによる付与確率調整*/
			if(r_unique_skill_table[i].bias1 == *bias) tmp = tmp * r_unique_skill_table[i].bias1_mult / 100;
			else if(r_unique_skill_table[i].bias2 == *bias) tmp = tmp * r_unique_skill_table[i].bias2_mult / 100;
			else if(r_unique_skill_table[i].bias3 == *bias) tmp = tmp * r_unique_skill_table[i].bias3_mult / 100;
			else if(r_unique_skill_table[i].bias4 == *bias) tmp = tmp * r_unique_skill_table[i].bias4_mult / 100;
			else if(r_unique_skill_table[i].bias5 == *bias) tmp = tmp * r_unique_skill_table[i].bias5_mult / 100;

			chance[i] = tmp;
			totalchance += tmp;
			flag_recalc_chance = FALSE;
		}

		/*:::各スキルの付与確率が反映されたtotalchanceとchance[]が作成されたので付与作業に入る*/
		randomchance = randint1(totalchance);
		for(i=0;i<RU_SKILL_TABLE_MAX;i++)
		{
			randomchance -= chance[i];
			if(randomchance <=0) break;
		}

		skillnum = i;

		switch(skillnum)
		{
			case 0: //HP増加
				r_ptr->hdice = MIN(200,r_ptr->hdice + 1 + lev/30);
				break;
			case 1: //攻撃ダイス増加
				if(one_in_(3)) r_ptr->blow[0].d_dice = MIN( r_ptr->blow[0].d_dice+1, 2 + lev / 6);
				if(one_in_(3)) r_ptr->blow[0].d_side = MIN( r_ptr->blow[0].d_side+1, 4 + lev / 6);
				if(one_in_(3)) r_ptr->blow[1].d_dice = MIN( r_ptr->blow[1].d_dice+1, 2 + lev / 6);
				if(one_in_(3)) r_ptr->blow[1].d_side = MIN( r_ptr->blow[1].d_side+1, 4 + lev / 6);
				if(one_in_(3)) r_ptr->blow[2].d_dice = MIN( r_ptr->blow[2].d_dice+1, 2 + lev / 6);
				if(one_in_(3)) r_ptr->blow[2].d_side = MIN( r_ptr->blow[2].d_side+1, 5 + lev / 6);
				if(one_in_(3)) r_ptr->blow[3].d_dice = MIN( r_ptr->blow[3].d_dice+1, 2 + lev / 6);
				if(one_in_(3)) r_ptr->blow[3].d_side = MIN( r_ptr->blow[3].d_side+1, 5 + lev / 6);
				break;
			case 2: //AC増加
				{
					int ac_plus = randint1(5) + lev / 10;

					if (r_ptr->ac > 300) ac_plus /= 4;
					else if (r_ptr->ac > 150) ac_plus /= 2;

					r_ptr->ac += ac_plus;

					if (r_ptr->ac > 500) ac_plus = 500;

					break;
				}
			case 3: //感知範囲増加
				r_ptr->aaf = MIN(100,r_ptr->aaf + 20);
				break;
			case 4: //加速増加
				if((r_ptr->speed - 110) < lev / 4 || *bias == RUBIAS_SPEED || one_in_(3)) r_ptr->speed = MIN(180, r_ptr->speed + randint1(3) + lev / 30);
				break;
			case 5: //魔法使用頻度増加
				r_ptr->freq_spell = MIN(50,r_ptr->freq_spell + 5);
				break;
			case 6: //護衛付き
				if(r_ptr->flags1 & RF1_ESCORT) r_ptr->flags1 |= RF1_ESCORTS;
				else  r_ptr->flags1 |= RF1_ESCORT;
				break;
			case 7: //RF2_STUPID
				if(!(r_ptr->flags2 & RF2_SMART))   r_ptr->flags2 |= RF2_STUPID;
				break;
			case 8: //RF2_SMART
				if(!(r_ptr->flags2 & RF2_STUPID)) r_ptr->flags2 |= RF2_SMART;
				break;
			case 9: //RF2_REFLECTING
				r_ptr->flags2 |= RF2_REFLECTING;
				break;
			case 10: //RF2_INVISIBLE
				r_ptr->flags2 |= RF2_INVISIBLE;
				break;
			case 11: //RF2_REGENERATE
				r_ptr->flags2 |= RF2_REGENERATE;
				break;
			case 12: //RF2_POWERFUL
				r_ptr->flags2 |= RF2_POWERFUL;
				break;
			case 13: //RF2_ELDRITCH_HORROR
				r_ptr->flags2 |= RF2_ELDRITCH_HORROR;
				break;
			case 14: //RF2_AURA_FIRE
				r_ptr->flags2 |= RF2_AURA_FIRE;
				break;
			case 15: //RF2_AURA_ELEC
				r_ptr->flags2 |= RF2_AURA_ELEC;
				break;
			case 16: //RF2_PASS_WALL
				if(!(r_ptr->flags2 & RF2_KILL_WALL)) r_ptr->flags2 |= RF2_PASS_WALL;
				break;
			case 17: //RF2_KILL_WALL
				if(!(r_ptr->flags2 & RF2_PASS_WALL)) r_ptr->flags2 |= RF2_KILL_WALL;
				break;
			case 18: //RF2_MOVE_BODY
				if(!(r_ptr->flags2 & RF2_KILL_BODY)) r_ptr->flags2 |= RF2_MOVE_BODY;
				break;
			case 19: //RF2_AURA_COLD
				r_ptr->flags2 |= RF2_AURA_COLD;
				break;
			case 20: //RF2_TELE_AFTER
				r_ptr->flags2 |= RF2_TELE_AFTER;
				break;
			case 21: //RF2_GIGANTIC
				if(!my_strchr("p",r_ptr->d_char)) r_ptr->flags2 |= RF2_GIGANTIC;
				break;
			case 22: //RF2_KILL_BODY
				if(!(r_ptr->flags2 & RF2_MOVE_BODY)) r_ptr->flags2 |= RF2_KILL_BODY;
				break;
			case 23: //RF3_NO_FEAR
				r_ptr->flags3 |= RF3_NO_FEAR;
				break;
			case 24: //RF3_NO_STUN
				r_ptr->flags3 |= RF3_NO_STUN;
				break;

			case 25: //RF4_SHRIEK
				r_ptr->flags4 |= RF4_SHRIEK;
				break;
			case 26: //RF4_DANMAKU
				r_ptr->flags4 |= RF4_DANMAKU;
				break;
			case 27: //RF4_DISPEL
				r_ptr->flags4 |= RF4_DISPEL;
				break;

			case 28: //RF4_ROCKET
				r_ptr->flags4 |= RF4_ROCKET;
				break;

			case 29: //RF4_SHOOT
				r_ptr->flags4 |= RF4_SHOOT;
				break;

			case 30: //RF4_BR_HOLY
				if( check_multi_breath(r_ptr, *bias) && !(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags4 |= RF4_BR_HOLY;
				break;
			case 31: //RF4_BR_HELL
				if(check_multi_breath(r_ptr, *bias) && !(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(6)) r_ptr->flags4 |= RF4_BR_HELL;
				break;
			case 32: //RF4_BR_AQUA
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_AQUA;
				break;
			case 33: //RF4_BR_ACID
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_ACID;
				break;
			case 34: //RF4_BR_ELEC
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_ELEC;
				break;
			case 35: //RF4_BR_FIRE
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_FIRE;
				break;
			case 36: //RF4_BR_COLD
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_COLD;
				break;
			case 37: //RF4_BR_POIS
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_POIS;
				break;
			case 38: //RF4_BR_NETH
				if(check_multi_breath(r_ptr, *bias) && !(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags4 |= RF4_BR_NETH;
				break;
			case 39: //RF4_BR_LITE
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_LITE;
				break;
			case 40: //RF4_BR_DARK
				if(check_multi_breath(r_ptr, *bias) && !(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags4 |= RF4_BR_DARK;
				break;
			case 41: //RF4_WAVEMOTION
				r_ptr->flags4 |= RF4_WAVEMOTION;
				break;
			case 42: //RF4_BR_SOUN
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_SOUN;
				break;
			case 43: //RF4_BR_CHAO
				if(check_multi_breath(r_ptr, *bias) && !(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(4)) r_ptr->flags4 |= RF4_BR_CHAO;
				break;
			case 44: //RF4_BR_DISE
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_DISE;
				break;
			case 45: //RF4_BR_NEXU
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_NEXU;
				break;
			case 46: //RF4_BR_TIME
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_TIME;
				break;
			case 47: //RF4_BR_INER
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_INER;
				break;
			case 48: //RF4_BR_GRAV
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_GRAV;
				break;
			case 49: //RF4_BR_SHAR
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_SHAR;
				break;
			case 50: //RF4_BR_PLAS
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_PLAS;
				break;
			case 51: //RF4_BA_FORCE
				r_ptr->flags4 |= RF4_BA_FORCE;
				break;
			case 52: //RF4_BR_MANA
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_MANA;
				break;
			case 53: //RF4_BR_NUKE
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_NUKE;
				break;
			case 54: //RF4_BA_CHAO
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(4)) r_ptr->flags4 |= RF4_BA_CHAO;
				break;
			case 55: //RF4_BR_DISI
				if(check_multi_breath(r_ptr, *bias)) r_ptr->flags4 |= RF4_BR_DISI;
				break;
			case 56: //RF5_BA_ACID
				r_ptr->flags5 |= RF5_BA_ACID;
				break;
			case 57: //RF5_BA_ELEC
				r_ptr->flags5 |= RF5_BA_ELEC;
				break;
			case 58: //RF5_BA_FIRE
				r_ptr->flags5 |= RF5_BA_FIRE;
				break;
			case 59: //RF5_BA_COLD
				r_ptr->flags5 |= RF5_BA_COLD;
				break;
			case 60: //RF5_BA_POIS
				r_ptr->flags5 |= RF5_BA_POIS;
				break;
			case 61: //RF5_BA_NETH
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_BA_NETH;
				break;
			case 62: //RF5_BA_WATE
				r_ptr->flags5 |= RF5_BA_WATE;
				break;
			case 63: //RF5_BA_MANA
				r_ptr->flags5 |= RF5_BA_MANA;
				break;
			case 64: //RF5_BA_DARK
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_BA_DARK;
				break;
			case 65: //RF5_DRAIN_MANA
				r_ptr->flags5 |= RF5_DRAIN_MANA;
				break;
			case 66: //RF5_MIND_BLAST
				r_ptr->flags5 |= RF5_MIND_BLAST;
				break;
			case 67: //RF5_BRAIN_SMASH
				r_ptr->flags5 |= RF5_BRAIN_SMASH;
				break;
			case 68: //RF5_CAUSE_1
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_CAUSE_1;
				break;
			case 69: //RF5_CAUSE_2
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_CAUSE_2;
				break;
			case 70: //RF5_CAUSE_3
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_CAUSE_3;
				break;
			case 71: //RF5_CAUSE_4
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_CAUSE_4;
				break;
			case 72: //RF5_BO_ACID
				r_ptr->flags5 |= RF5_BO_ACID;
				break;
			case 73: //RF5_BO_ELEC
				r_ptr->flags5 |= RF5_BO_ELEC;
				break;
			case 74: //RF5_BO_FIRE
				r_ptr->flags5 |= RF5_BO_FIRE;
				break;
			case 75: //RF5_BO_COLD
				r_ptr->flags5 |= RF5_BO_COLD;
				break;
			case 76: //RF5_BA_LITE
				r_ptr->flags5 |= RF5_BA_LITE;
				break;
			case 77: //RF5_BO_NETH
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(3)) r_ptr->flags5 |= RF5_BO_NETH;
				break;
			case 78: //RF5_BO_WATE
				r_ptr->flags5 |= RF5_BO_WATE;
				break;
			case 79: //RF5_BO_MANA
				r_ptr->flags5 |= RF5_BO_MANA;
				break;
			case 80: //RF5_BO_PLAS
				r_ptr->flags5 |= RF5_BO_PLAS;
				break;
			case 81: //RF5_BO_ICEE
				r_ptr->flags5 |= RF5_BO_ICEE;
				break;
			case 82: //RF5_BO_MISSILE
				r_ptr->flags5 |= RF5_MISSILE;
				break;
			case 83: //RF5_SCARE
				r_ptr->flags5 |= RF5_SCARE;
				break;
			case 84: //RF5_BLIND
				r_ptr->flags5 |= RF5_BLIND;
				break;
			case 85: //RF5_CONF
				r_ptr->flags5 |= RF5_CONF;
				break;
			case 86: //RF5_SLOW
				r_ptr->flags5 |= RF5_SLOW;
				break;
			case 87: //RF5_HOLD
				r_ptr->flags5 |= RF5_HOLD;
				break;
			case 88: //RF6_HASTE
				r_ptr->flags6 |= RF6_HASTE;
				break;
			case 89: //RF6_HAND_DOOM
				r_ptr->flags6 |= RF6_HAND_DOOM;
				break;
			case 90: //RF6_HEAL
				r_ptr->flags6 |= RF6_HEAL;
				break;

			case 91: //RF6_INVULNER
				r_ptr->flags6 |= RF6_INVULNER;
				break;
			case 92: //RF6_BLINK
				r_ptr->flags6 |= RF6_BLINK;
				break;
			case 93: //RF6_TPORT
				r_ptr->flags6 |= RF6_TPORT;
				break;
			case 94: //RF6_WORLD
				r_ptr->flags6 |= RF6_WORLD;
				break;
			case 95: //RF6_TELE_TO
				r_ptr->flags6 |= RF6_TELE_TO;
				break;
			case 96: //RF6_TELE_AWAY
				r_ptr->flags6 |= RF6_TELE_AWAY;
				break;
			case 97: //RF6_PSY_SPEAR
				r_ptr->flags6 |= RF6_PSY_SPEAR;
				break;
			case 98: //RF6_TRAPS
				r_ptr->flags6 |= RF6_TRAPS;
				break;
			case 99: //RF6_FORGET
				r_ptr->flags6 |= RF6_FORGET;
				break;
			case 100: //RF6_COSMIC_HORROR エルドリッチホラーと神格も付加される
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(6))
				{
					r_ptr->flags6 |= RF6_COSMIC_HORROR;
					r_ptr->flags2 |= RF2_ELDRITCH_HORROR;
					r_ptr->flags3 |= RF3_DEITY;
				}
				break;

			case 101: //RF6_S_KIN
				r_ptr->flags6 |= RF6_S_KIN;
				break;
			case 102: //RF6_S_CYBER
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS)) r_ptr->flags6 |= RF6_S_CYBER;
				break;
			case 103: //RF6_S_MONSTER
				r_ptr->flags6 |= RF6_S_MONSTER;
				break;
			case 104: //RF6_S_MONSTERS
				r_ptr->flags6 |= RF6_S_MONSTERS;
				break;
			case 105: //RF6_S_ANT
				r_ptr->flags6 |= RF6_S_ANT;
				break;
			case 106: //RF6_S_SPIDER
				r_ptr->flags6 |= RF6_S_SPIDER;
				break;
			case 107: //RF6_S_HOUND
				r_ptr->flags6 |= RF6_S_HOUND;
				break;
			case 108: //RF6_S_HYDRA
				r_ptr->flags6 |= RF6_S_HYDRA;
				break;
			case 109: //RF6_S_ANGEL
				r_ptr->flags6 |= RF6_S_ANGEL;
				break;
			case 110: //RF6_S_DEMON
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) ) r_ptr->flags6 |= RF6_S_DEMON;
				break;
			case 111: //RF6_S_UNDEAD
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) ) r_ptr->flags6 |= RF6_S_UNDEAD;
				break;
			case 112: //RF6_S_DRAGON
				r_ptr->flags6 |= RF6_S_DRAGON;
				break;
			case 113: //RF6_S_HI_UNDEAD
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) ) r_ptr->flags6 |= RF6_S_HI_UNDEAD;
				break;
			case 114: //RF6_S_HI_DRAGON
				r_ptr->flags6 |= RF6_S_HI_DRAGON;
				break;
			case 115: //RF6_S_AMBERITES
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) ) r_ptr->flags6 |= RF6_S_AMBERITES;
				break;
			case 116: //RF6_S_UNIQUE
				r_ptr->flags6 |= RF6_S_UNIQUE;
				break;
			case 117: //RF9_FIRE_STORM
				r_ptr->flags9 |= RF9_FIRE_STORM;
				break;
			case 118: //RF9_ICE_STORM
				r_ptr->flags9 |= RF9_ICE_STORM;
				break;
			case 119: //RF9_THUNDER_STORM
				r_ptr->flags9 |= RF9_THUNDER_STORM;
				break;
			case 120: //RF9_ACID_STORM
				r_ptr->flags9 |= RF9_ACID_STORM;
				break;
			case 121: //RF9_TOXIC_STORM
				r_ptr->flags9 |= RF9_TOXIC_STORM;
				break;
			case 122: //RF9_BA_POLLUTE
				r_ptr->flags9 |= RF9_BA_POLLUTE;
				break;
			case 123: //RF9_BA_DISI
				r_ptr->flags9 |= RF9_BA_DISI;
				break;
			case 124: //RF9_BA_HOLY
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_BA_DISI;
				break;
			case 125: //RF9_BA_METEOR
				r_ptr->flags9 |= RF9_BA_METEOR;
				break;
			case 126: //RF9_BA_DISTORTION
				r_ptr->flags9 |= RF9_BA_DISTORTION;
				break;
			case 127: //RF9_PUNISH_1
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_PUNISH_1;
				break;
			case 128: //RF9_PUNISH_2
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_PUNISH_2;
				break;
			case 129: //RF9_PUNISH_3
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_PUNISH_3;
				break;
			case 130: //RF9_PUNISH_4
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_PUNISH_4;
				break;
			case 131: //RF9_BO_HOLY
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_BO_HOLY;
				break;
			case 132: //RF9_GIGANTIC_LASER
				r_ptr->flags9 |= RF9_GIGANTIC_LASER;
				break;
			case 133: //RF9_BO_SOUND
				r_ptr->flags9 |= RF9_BO_SOUND;
				break;
			case 134: //RF9_S_ANIMAL
				r_ptr->flags9 |= RF9_S_ANIMAL;
				break;
			case 135: //RF9_BEAM_LITE
				r_ptr->flags9 |= RF9_BEAM_LITE;
				break;
			case 136: //RF9_HELLFIRE
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) || one_in_(6)) r_ptr->flags9 |= RF9_HELLFIRE;
				break;
			case 137: //RF9_HOLYFIRE
				if(!(r_ptr->flags3 & RF3_ANG_CHAOS) || one_in_(6)) r_ptr->flags9 |= RF9_HOLYFIRE;
				break;
			case 138: //RF9_TORNADO
				r_ptr->flags9 |= RF9_TORNADO;
				break;
			case 139: //RF9_S_DEITY
				r_ptr->flags9 |= RF9_S_DEITY;
				break;
			case 140: //RF9_S_HI_DEMON
				if(!(r_ptr->flags3 & RF3_ANG_COSMOS) ) r_ptr->flags9 |= RF9_S_HI_DEMON;
				break;
			case 141: //RF9_S_KWAI
				r_ptr->flags9 |= RF9_S_KWAI;
				break;
			case 142: //RF9_TELE_APPROACH
				r_ptr->flags9 |= RF9_TELE_APPROACH;
				break;
			case 143: //RF9_TELE_HI_APPROACH
				r_ptr->flags9 |= RF9_TELE_HI_APPROACH;
				break;
			case 144: //RF9_MAELSTROM
				r_ptr->flags9 |= RF9_MAELSTROM;
				break;
			case 145: //RF9_ALARM
				r_ptr->flags9 |= RF9_ALARM;
				break;

			case 146: //元素ボール全て
				r_ptr->flags5 |= RF5_BA_ACID;
				r_ptr->flags5 |= RF5_BA_ELEC;
				r_ptr->flags5 |= RF5_BA_FIRE;
				r_ptr->flags5 |= RF5_BA_COLD;
				break;

			case 147: //元素嵐全て
				r_ptr->flags9 |= RF9_FIRE_STORM;
				r_ptr->flags9 |= RF9_ICE_STORM;
				r_ptr->flags9 |= RF9_THUNDER_STORM;
				r_ptr->flags9 |= RF9_ACID_STORM;
				r_ptr->flags9 |= RF9_TOXIC_STORM;
				break;
			case 148: //元素ブレス全て
				r_ptr->flags4 |= RF4_BR_ACID;
				r_ptr->flags4 |= RF4_BR_ELEC;
				r_ptr->flags4 |= RF4_BR_FIRE;
				r_ptr->flags4 |= RF4_BR_COLD;
				r_ptr->flags4 |= RF4_BR_POIS;
				break;
			case 149: //ショートテレポ・テレポ・テレポアウェイ
				r_ptr->flags6 |= RF6_BLINK;
				r_ptr->flags6 |= RF6_TPORT;
				r_ptr->flags6 |= RF6_TELE_AWAY;
				break;

			default:
				msg_format(_("ERROR:ランダムユニークのスキル付与乱数設定がおかしい(%d)",
                            "ERROR: Weird random number for picking skills for random unique (%d)"),skillnum);

		}
	}

}


/*:::ランダムユニークにスキルを付与した後にバイアスやシンボルを参照し相応しいフラグや攻撃属性や耐性を付与する。スキル付与前にしないのはバイアス途中上書きに対応したり得たスキルにより耐性の傾向を決めるため。*/
void apply_random_unique_xtra_param(monster_race *r_ptr, int symbol_num, int bias, int power)
{
	byte d_c = r_ptr->d_char;
	bool flag_onlymagic_ok = FALSE;
	int drop_num;
	int i,restry;
	int lev = r_ptr->level;
	u32b f2,f3,f4,f5,f6,f7,f9;

	//バイアスにより色を決める
	r_ptr->d_attr = r_unique_bias_table[bias].base_attr;
	r_ptr->x_attr = r_unique_bias_table[bias].base_attr;

	/*:::文字・色*/
	if((bias == RUBIAS_CURSE || bias == RUBIAS_PHYSICAL || bias == RUBIAS_INSANITY || bias == RUBIAS_MULTI_MAGIC || bias == RUBIAS_MANA) && one_in_(50)) r_ptr->flags1 |= RF1_CHAR_CLEAR;
	else if(my_strchr("ijtvKSUVX",d_c) && one_in_(20)) r_ptr->flags1 |= RF1_CHAR_CLEAR;
	else if((bias == RUBIAS_CHAOS || bias == RUBIAS_INSANITY || bias == RUBIAS_SPACE) && one_in_(10)) r_ptr->flags1 |= RF1_SHAPECHANGER;
	if(bias == RUBIAS_SPACE && one_in_(2)) r_ptr->flags1 |= RF1_ATTR_SEMIRAND;
	if(my_strchr("ijG#",d_c) && one_in_(20)) r_ptr->flags1 |= RF1_ATTR_CLEAR;
	else if((bias == RUBIAS_CHAOS || bias == RUBIAS_MULTI_BREATH) )
	{
		r_ptr->flags1 |= RF1_ATTR_MULTI;
		r_ptr->flags2 |= RF2_ATTR_ANY;
	}
	else if((bias == RUBIAS_MULTI_MAGIC) )
	{
		r_ptr->flags1 |= RF1_ATTR_MULTI;
	}


	/*:::移動関係*/
	if(my_strchr("egjlvS",d_c) && (randint1(100) > 90) || my_strchr("*",d_c) && (randint1(100) > 50) || my_strchr("Qm&,",d_c) && (randint1(100) > 5) || bias == RUBIAS_TOUGHNESS && one_in_(10))
	{
		r_ptr->flags1 |= RF1_NEVER_MOVE;
		r_ptr->flags2 &= ~(RF2_OPEN_DOOR | RF2_BASH_DOOR | RF2_MOVE_BODY | RF2_KILL_BODY);
	}
	else if(my_strchr("bfqrtvzCEFSXZ",d_c) && (randint1(100) > 80)) r_ptr->flags1 |= RF1_RAND_25;
	else if(my_strchr("v*",d_c) && (randint1(100) > 20)) r_ptr->flags1 |= RF1_RAND_50;

	/*:::攻撃関係*/

	if((r_ptr->flags4 & ( RF4_ATTACK_MASK | RF4_BREATH_MASK | RF4_INDIRECT_MASK ))
	 ||(r_ptr->flags5 & ( RF5_ATTACK_MASK | RF5_BREATH_MASK | RF5_INDIRECT_MASK ))
	 ||(r_ptr->flags6 & ( RF6_ATTACK_MASK | RF6_BREATH_MASK | RF6_INDIRECT_MASK ))
	 ||(r_ptr->flags9 & ( RF9_ATTACK_MASK | RF9_BREATH_MASK | RF9_INDIRECT_MASK ))) flag_onlymagic_ok = TRUE;

	/*:::確率で「物理的な攻撃方法を持たない」にし、魔法使用率100%にし、速度減少*/
	if(flag_onlymagic_ok &&
		(my_strchr("Q&",d_c) && (randint1(100) > 20) || my_strchr("*",d_c) && (randint1(100) > 50)
		|| power > 50 && bias != RUBIAS_TOUGHNESS && bias !=  RUBIAS_POWER && bias != RUBIAS_SPEED && one_in_(50) ))
	{
		r_ptr->blow[0].method = 0;
		r_ptr->blow[1].method = 0;
		r_ptr->blow[2].method = 0;
		r_ptr->blow[3].method = 0;
		r_ptr->flags1 |= RF1_NEVER_BLOW;
		r_ptr->freq_spell = 100;
		if(r_ptr->speed > 110) r_ptr->speed -= (r_ptr->speed - 110) / 2;
	}
	/*:::物理攻撃方法と効果を設定し、場合によってはダイス値を修正*/
	else
	{
		for(i=0;i<4;i++)
		{
			int chance = randint0(10);
			if(!r_ptr->blow[i].method) break;

			/*:::まずシンボルで分岐しそれっぽい攻撃方法を設定*/
			if(chance < 6) r_ptr->blow[i].method = r_unique_symbol_table[symbol_num].rbm_60;
			else if(chance < 9) r_ptr->blow[i].method = r_unique_symbol_table[symbol_num].rbm_30;
			else r_ptr->blow[i].method = r_unique_symbol_table[symbol_num].rbm_10;

			/*:::バイアスにより中確率で特殊な攻撃効果を設定。場合によっては攻撃方法も修正する*/

			//色々調整したがどうも特殊効果が増え易すぎるので力技で通常攻撃にする
			if(i==0 && one_in_(2)) continue;
			if(i==2 && one_in_(2)) continue;

			if(bias == RUBIAS_TOUGHNESS && randint0(power) > randint0(120) )
			{
				if(r_ptr->blow[i].method != RBM_PUNCH && r_ptr->blow[i].method != RBM_KICK && r_ptr->blow[i].method != RBM_PRESS) r_ptr->blow[i].method = RBM_CRUSH;
				r_ptr->blow[i].effect = RBE_SHATTER;
			}
			else if(bias == RUBIAS_POWER && randint0(power) > randint0(100))
			{
				if(one_in_(2))r_ptr->blow[i].effect = RBE_SUPERHURT;
				else r_ptr->blow[i].effect = RBE_SUPERHURT;
			}
			else if(bias == RUBIAS_SPEED && randint0(power) > randint0(120))
			{
				r_ptr->blow[i].effect = RBE_BLEED;
			}
			else if(bias == RUBIAS_SPEED && i==1 && one_in_(3))
			{
				r_ptr->blow[i].effect = RBE_EAT_GOLD;
			}
			else if(bias == RUBIAS_BILE)
			{
				/*v2.0.3 未実装の吸収攻撃削除
				if(lev > 50 && randint0(power) > randint0(120)) r_ptr->blow[i].effect = RBE_MELT;
				else
				*/
					if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_UN_BONUS;
				else if(randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_ACID;
			}
			else if(bias == RUBIAS_ELEC)
			{
				if(lev > 24 && randint0(power) > randint0(110)) r_ptr->blow[i].effect = RBE_PARALYZE;
				else if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_LOSE_DEX;
				else if(randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_ELEC;
			}
			else if(bias == RUBIAS_COLD)
			{
				if(randint0(power) > randint0(120)) r_ptr->blow[i].effect = RBE_BLEED;
				else if(randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_INERTIA;
				else if(randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_COLD;
			}
			else if(bias == RUBIAS_FIRE)
			{
				if(randint0(power) > randint0(120)) r_ptr->blow[i].effect = RBE_STUN;
				else if(randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_FIRE;
			}
			else if(bias == RUBIAS_POIS)
			{
				if(randint0(power) > randint0(150)) r_ptr->blow[i].effect = RBE_LOSE_ALL;
				else if(randint0(power) > randint0(120)) r_ptr->blow[i].effect = RBE_DISEASE;
				else if(randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_LOSE_STR;
				else if(randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_LOSE_CON;
				else if(randint0(power) > randint0(40)) r_ptr->blow[i].effect = RBE_POISON;
			}
			else if(bias == RUBIAS_DEMONIC)
			{
				if(randint0(power) > randint0(130)) r_ptr->blow[i].effect = RBE_UN_POWER;
				else if(randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_DISEASE;
				else if(randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_UN_BONUS;
				else if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_TERRIFY;
				else if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_EXP_VAMP;
				else if(lev > 60 && randint0(power) > randint0(70)) r_ptr->blow[i].effect = RBE_EXP_40;
				else if(lev > 30 && randint0(power) > randint0(60)) r_ptr->blow[i].effect = RBE_EXP_10;
			}
			else if(bias == RUBIAS_HOLY)
			{
				if(randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_SMITE;
			}
			else if(bias == RUBIAS_WATE)
			{
				if(randint0(power) > randint0(100))
				{
					if(r_ptr->blow[i].method != RBM_DRAG && r_ptr->blow[i].method != RBM_ENGULF && r_ptr->blow[i].method != RBM_SPIT) r_ptr->blow[i].method = RBM_TOUCH;
					r_ptr->blow[i].effect = RBE_DROWN;
				}
			}
			else if(bias == RUBIAS_NETH)
			{
				if(lev > 50 && randint0(power) > randint0(150))  r_ptr->blow[i].effect = RBE_KILL;
				else if(randint0(power) > randint0(80))  r_ptr->blow[i].effect = RBE_TERRIFY;
				else if(randint0(power) > randint0(70))  r_ptr->blow[i].effect = RBE_CURSE;
				else if(lev > 60 && randint0(power) > randint0(70)) r_ptr->blow[i].effect = RBE_EXP_80;
				else if(lev > 50 && randint0(power) > randint0(60)) r_ptr->blow[i].effect = RBE_EXP_40;
				else if(lev > 30 && randint0(power) > randint0(50)) r_ptr->blow[i].effect = RBE_EXP_20;
				else if(lev > 10 && randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_EXP_10;

			}
			else if(bias == RUBIAS_CURSE)
			{
				if(lev > 30 && randint0(power) > randint0(130))  r_ptr->blow[i].effect = RBE_LOSE_ALL;
				else if(randint0(power) > randint0(90))  r_ptr->blow[i].effect = RBE_STUN;
				else if(randint0(power) > randint0(80))  r_ptr->blow[i].effect = RBE_INERTIA;
				else if(randint0(power) > randint0(70))  r_ptr->blow[i].effect = RBE_TERRIFY;
				else if(randint0(power) > randint0(50))  r_ptr->blow[i].effect = RBE_CURSE;
				else if(i == 3 && randint0(power) > randint0(40))  r_ptr->blow[i].effect = RBE_HUNGER;
			}
			else if(bias == RUBIAS_PHYSICAL)
			{
				if(lev > 30 && randint0(power) > randint0(120))  r_ptr->blow[i].effect = RBE_STUN;
				else if(randint0(power) > randint0(110))  r_ptr->blow[i].effect = RBE_BLEED;
			}
			else if(bias == RUBIAS_SOUND)
			{
				if(lev > 29 && randint0(power) > randint0(120))
				{
					r_ptr->blow[i].effect = RBE_CONFUSE;
					r_ptr->blow[i].method = RBM_SHOW;
				}
				else if(randint0(power) > randint0(100))
				{
					r_ptr->blow[i].effect = RBE_STUN;
					r_ptr->blow[i].method = RBM_SHOW;
				}
			}
			else if(bias == RUBIAS_CHAOS)
			{
				if(lev > 29 && randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_CHAOS;
				else if(lev > 29 && randint0(power) > randint0(70)) r_ptr->blow[i].effect = RBE_CONFUSE;

				if(one_in_(20)) r_ptr->blow[i].method = RBM_DANCE;
			}
			else if(bias == RUBIAS_SPACE)
			{
				if(lev > 39 && randint0(power) > randint0(120)) r_ptr->blow[i].effect = RBE_TIME;
				else if(randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_INERTIA;
				else if(randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_BLEED;
			}
			else if(bias == RUBIAS_INSANITY)
			{
				if(randint0(power) > randint0(100))
				{
					if(r_ptr->blow[i].method != RBM_WAIL && r_ptr->blow[i].method != RBM_ENGULF && r_ptr->blow[i].method != RBM_CRAWL && r_ptr->blow[i].method != RBM_TOUCH) r_ptr->blow[i].method = RBM_GAZE;
					r_ptr->blow[i].effect = RBE_INSANITY;
				}
				else if(lev > 29 && randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_CONFUSE;
				else if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_TERRIFY;

				if(one_in_(20)) r_ptr->blow[i].method = RBM_DANCE;
			}
			else if(bias == RUBIAS_MANA)
			{
				if(lev > 39 && randint0(power) > randint0(130)) r_ptr->blow[i].effect = RBE_UN_POWER;
				else if(randint0(power) > randint0(90)) r_ptr->blow[i].effect = RBE_SUPERHURT;
				else if(randint0(power) > randint0(80)) r_ptr->blow[i].effect = RBE_SHATTER;
				else if(randint0(power) > randint0(70)) r_ptr->blow[i].effect = RBE_DR_MANA;
			}
			else if(bias == RUBIAS_MULTI_MAGIC)
			{
				if(lev > 39 && randint0(power) > randint0(130)) r_ptr->blow[i].effect = RBE_UN_POWER;
				if(lev > 29 && randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_CHAOS;
				else if(randint0(power) > randint0(30)) r_ptr->blow[i].effect = RBE_ELEMENT;
			}
			else if(bias == RUBIAS_MULTI_BREATH)
			{
				if(lev > 29 && randint0(power) > randint0(100)) r_ptr->blow[i].effect = RBE_CHAOS;
				else if(randint0(power) > randint0(50)) r_ptr->blow[i].effect = RBE_ELEMENT;
			}
		}

		/*:::シンボルなどに応じてさらに攻撃方法を上書きする*/

		//人間型のファイター系はたまに全て攻撃が格闘になる
		if(my_strchr("fhkopstAOPTU",d_c) && (bias == RUBIAS_POWER || bias == RUBIAS_SPEED) || one_in_(7))
		{
			for(i=0;i<4;i++) if(r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(2) ? RBM_PUNCH : RBM_KICK);
		}
		//人型のキャスター系は攻撃方法を変更する
		if(my_strchr("hpt",d_c) && bias != RUBIAS_NONE && bias != RUBIAS_POWER && bias != RUBIAS_SPEED && bias != RUBIAS_TOUGHNESS && bias != RUBIAS_WATE)
		{
			for(i=0;i<4;i++)
			{
				if(!r_ptr->blow[i].method)break;

				if(one_in_(3)) r_ptr->blow[i].method = RBM_BOOK;
				else if(one_in_(3)) r_ptr->blow[i].method = RBM_STING;
				else if(one_in_(2)) r_ptr->blow[i].method = RBM_STRIKE;
				else r_ptr->blow[i].method = RBM_TOUCH;
			}
		}
		//スピード型の天狗はパパラッチになる
		if(my_strchr("t",d_c) &&  bias == RUBIAS_SPEED )
		{
			r_ptr->blow[0].method = RBM_HIT;
			r_ptr->blow[0].effect = RBE_PHOTO;
			r_ptr->blow[0].d_side = 0;
			r_ptr->blow[0].d_dice = 0;
		}
		//一部シンボルはしばしば毒攻撃になる
		if(my_strchr("cijwzIJ",d_c) )
		{
			if(one_in_(2))r_ptr->blow[0].effect = RBE_POISON;
			if(one_in_(3))r_ptr->blow[3].effect = RBE_POISON;
		}
		//一部シンボルはしばしば魔力を吸い取る
		if(my_strchr("LUV",d_c))
		{
			if(one_in_(3))r_ptr->blow[3].effect = RBE_UN_POWER;
		}
		//一部シンボルはしばしば吸血攻撃をする
		if(my_strchr("jV",d_c))
		{
			if(!one_in_(3))r_ptr->blow[3].effect = RBE_EXP_VAMP;
		}
		//天使は破邪攻撃をする
		if(my_strchr("A",d_c))
		{
			r_ptr->blow[1].effect = RBE_SMITE;
			if(one_in_(2))r_ptr->blow[0].effect = RBE_SMITE;
			if(one_in_(3))r_ptr->blow[3].effect = RBE_SMITE;
		}
		//巨人はしばしば混乱攻撃をする
		if(my_strchr("P",d_c))
		{
			if(one_in_(2))r_ptr->blow[1].effect = RBE_CONFUSE;
			if(one_in_(2))r_ptr->blow[3].effect = RBE_CONFUSE;
		}
		//水棲系モンスターはしばしば引きずり込み攻撃をする
		if(r_ptr->flags7 & RF7_AQUATIC)
		{
			if(one_in_(2))
			{
				r_ptr->blow[0].method = RBM_DRAG;
				r_ptr->blow[0].effect = RBE_DROWN;
			}
			if(one_in_(3))
			{
				r_ptr->blow[3].method = RBM_DRAG;
				r_ptr->blow[3].effect = RBE_DROWN;
			}
		}

		/*:::最後にダイスを補正する。元素は強く、軽減付加打撃は弱く、魔力吸収は高確率で0*/
		for(i=0;i<4;i++)
		{
			int ef = r_ptr->blow[i].effect;

			//元素攻撃はダイス増やす
			if(ef == RBE_ACID || ef == RBE_ELEC || ef == RBE_FIRE || ef == RBE_COLD || ef == RBE_ELEMENT)
				r_ptr->blow[i].d_side = MIN(30,r_ptr->blow[i].d_side * 2);
			else if(ef == RBE_UN_POWER && randint0(power) < 80)
			{
				r_ptr->blow[i].d_side = 0;
				r_ptr->blow[i].d_dice = 0;
			}
			else if(ef == RBE_HURT || ef == RBE_SHATTER || ef == RBE_SUPERHURT || ef == RBE_BLEED); //AC軽減可能な攻撃のダイスはいじらない
			else if(ef == RBE_PHOTO); //写真撮影のダイスはいじらない
			else if(ef == RBE_UN_BONUS || ef == RBE_CHAOS || ef == RBE_SMITE || ef == RBE_DROWN ) //上位耐性で軽減可能な攻撃　少しダイス減らす
			{
				int tmp = 6 + randint1(3);
				r_ptr->blow[i].d_side = MAX(1,(r_ptr->blow[i].d_side * (tmp) / 10));

			}
			else if(ef == RBE_DR_MANA ) //魔力吸収攻撃　少しダイス増やす
			{
				int tmp = (r_ptr->blow[i].d_dice * (5 + randint1(3)) / 5);
				r_ptr->blow[i].d_dice = MIN(30,tmp);

			}
			else //軽減不可能な攻撃　ダイス減らす
			{
				int tmp = 5 + randint1(2);
				r_ptr->blow[i].d_side = MAX(1,(r_ptr->blow[i].d_side * (tmp) / 10));

			}
		}

	}

	/*:::その他フラグ処理*/

	/*:::RF2_COLD_BLOOD*/
	if(my_strchr("gmszGVW#&",d_c) && (randint1(100) > 1)  || my_strchr("acijluvwIJLQUXY*",d_c) && (randint1(100) > 10)  || my_strchr("neyEKMS",d_c) && (randint1(100) > 30)  || my_strchr("dbfhkopqrtABCDFHOPTZ",d_c) && (randint1(100) > 95))
	{
		r_ptr->flags2 |= RF2_COLD_BLOOD;
	}
	/*:::RF2_PASS_WALL*/
	if(my_strchr("G",d_c) && (randint1(100) > 10)  || my_strchr("V#*",d_c) && (randint1(100) > 80))
	{
		r_ptr->flags2 |= RF2_PASS_WALL;
	}

	//v1.1.64 gシンボルのランダムユニークを時々埴輪にする
	if (my_strchr("g", d_c) && one_in_(3))
	{
		r_ptr->flags2 |= RF2_COLD_BLOOD;
		r_ptr->flags2 |= RF2_EMPTY_MIND;
		r_ptr->flags3 |= RF3_HANIWA;
		r_ptr->flags3 |= RF3_NONLIVING;
	}

	//EMPTY_MIND
	if(!(r_ptr->flags2 & RF2_SMART) && ( bias == RUBIAS_INSANITY && one_in_(6) || bias == RUBIAS_DEMONIC && one_in_(13) ||  my_strchr("gjmvQX,&*",d_c) && (randint1(100) > 10) ||  my_strchr("acirswz#",d_c) && (randint1(100) > 50) ))
	{
		if(one_in_(2)) r_ptr->flags2 |= RF2_EMPTY_MIND;
		else  r_ptr->flags2 |= RF2_WEIRD_MIND;
	}
	//EMPTY_MINDの敵は恐怖、混乱しない
	if(r_ptr->flags2 & RF2_EMPTY_MIND) r_ptr->flags3 |= (RF3_NO_FEAR | RF3_NO_CONF);

	//RF3_KWAI
	if(my_strchr("tyKY",d_c) || my_strchr("abcefhijklmnqrsvwBCHIJLMOPQRTXZ",d_c) && (one_in_(10))  || one_in_(50))
	{
		r_ptr->flags3 |= RF3_KWAI;
	}
	//RF3_DEITY
	if(my_strchr("S",d_c) || my_strchr("EU",d_c) && (one_in_(20)) || my_strchr("eqBDEHJLPRUWY",d_c) && (one_in_(50))  || one_in_(200))
	{
		r_ptr->flags3 |= RF3_DEITY;
	}
	//RF3_DEMIHUMAN
	if(my_strchr("hnoOPT",d_c) || !my_strchr("svyzpAEGSUVW",d_c) && (one_in_(50)) )
	{
		r_ptr->flags3 |= RF3_DEMIHUMAN;
	}
	//RF3_DRAGON
	if(my_strchr("dD",d_c) )
	{
		r_ptr->flags3 |= RF3_DRAGON;
	}
	//RF3_DEMON
	if(my_strchr("uU",d_c) || bias == RUBIAS_DEMONIC && one_in_(4) || one_in_(40))
	{
		r_ptr->flags3 |= RF3_DEMON;
	}
	//RF3_UNDEAD
	if(my_strchr("szGLVW",d_c) || bias == RUBIAS_NETH && one_in_(4))
	{
		r_ptr->flags3 |= RF3_UNDEAD;
	}

	//RF3_NONLIVING
	if(my_strchr("gv&*",d_c) || my_strchr("jS#",d_c) && one_in_(3))
	{
		r_ptr->flags3 |= RF3_NONLIVING;
	}
	//RF3_ANIMAL
	if(my_strchr("abcfrCI",d_c) || my_strchr("kmnqHJ",d_c) && one_in_(3))
	{
		r_ptr->flags3 |= RF3_ANIMAL;
	}
	//RF3_HUMAN
	if(my_strchr("p",d_c))
	{
		r_ptr->flags3 |= RF3_HUMAN;
	}
	//RF3_FAIRY
	if(my_strchr("F",d_c))
	{
		r_ptr->flags3 |= RF3_FAIRY;
	}

	//RF3_NO_CONF
	if((r_ptr->flags2 & RF2_EMPTY_MIND) || (r_ptr->flags2 & RF2_STUPID) || (r_ptr->flags2 & RF2_SMART) && !one_in_(7) || randint1(r_ptr->level) > 15)
	{
		r_ptr->flags3 |= RF3_NO_CONF;
	}

	//RF7_AQUATICはランダムユニーク3が最初から持っている以外には付与しない。
	if(my_strchr("l",d_c) && !(r_ptr->flags7 & RF7_AQUATIC)) r_ptr->flags7 |= RF7_CAN_FLY;
	else if(my_strchr("btvABFG",d_c) || my_strchr("deDESV",d_c) && randint1(100) > 10 || my_strchr("uHIKLUY",d_c) && randint1(100) > 50 || r_ptr->level > 29 && randint1(r_ptr->level) > 30 || bias == RUBIAS_SPEED && one_in_(4) || bias == RUBIAS_MULTI_MAGIC || bias == RUBIAS_MANA)
	{
		r_ptr->flags7 |= RF7_CAN_FLY;
	}
	else if(my_strchr("ijnCDHKMP",d_c) && randint1(100) > 5 || my_strchr("acdefhkopqrsuwyzIJOR",d_c) && randint1(100) > 30 || randint1(100) > 70 || r_ptr->flags9 & RF9_MAELSTROM)
	{
		if(!my_strchr("V",d_c)) r_ptr->flags7 |= RF7_CAN_SWIM;
	}
	if(my_strchr("vA",d_c) && randint1(100) > 10 || my_strchr("eDEFS",d_c) && randint1(100) > 50 || r_ptr->level > 29 && randint1(r_ptr->level) > 30 || bias == RUBIAS_FIRE || bias == RUBIAS_ELEC && one_in_(2)  || bias == RUBIAS_HOLY && one_in_(3) || bias == RUBIAS_MULTI_MAGIC && one_in_(3) || bias == RUBIAS_MANA)
	{
		if(r_ptr->level > 49 || one_in_(3)) r_ptr->flags7 |= RF7_SELF_LITE_2;
		else r_ptr->flags7 |= RF7_SELF_LITE_1;
	}



	//v1.1.86 リリーホワイト生成のとき一部フラグを削除。またドロップ設定を渋る
	if (flag_generate_lilywhite)
	{
		int	tmp = randint1(r_ptr->level);
		drop_num = 0;

		if (r_ptr->level < 50) r_ptr->flags1 |= RF1_DROP_1D2;
		else r_ptr->flags1 |= RF1_DROP_2D2;

		if(tmp > 30) r_ptr->flags1 |= RF1_DROP_GOOD;
		if(tmp > 80) r_ptr->flags1 |= RF1_DROP_GREAT;

		r_ptr->mexp /= 2;

		r_ptr->flags2 &= ~(RF2_INVISIBLE | RF2_COLD_BLOOD | RF2_ELDRITCH_HORROR | RF2_GIGANTIC);

	}
	else
	{


		/*:::アイテムドロップ　*/
		if(power > dun_level / 3)
		{
			r_ptr->flags1 |= RF1_DROP_GOOD;
			if(lev > 19 &&( randint0(power * 3 / 2) > randint0(dun_level))) r_ptr->flags1 |= RF1_DROP_GREAT;
		}
		if(r_ptr->flags1 & RF1_DROP_GREAT) drop_num = MIN(20,power/12);
		else drop_num = MIN(20,power/6);


	}

	if(drop_num > 7) {r_ptr->flags1 |= RF1_DROP_4D2; drop_num -= 8;}
	if(drop_num > 5) {r_ptr->flags1 |= RF1_DROP_3D2; drop_num -= 6;}
	if(drop_num > 3) {r_ptr->flags1 |= RF1_DROP_2D2; drop_num -= 4;}
	if(drop_num > 1) {r_ptr->flags1 |= RF1_DROP_1D2;}


	/*:::以下ずっと耐性付与*/
	f2 = r_ptr->flags2;
	f3 = r_ptr->flags3;
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;
	f7 = r_ptr->flags7;
	f9 = r_ptr->flags9;

	restry = 0;
	if(bias == RUBIAS_BILE ) restry += 90;
	else if(bias == RUBIAS_TOUGHNESS ) restry += lev / 2;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 30 + lev / 2;
	else restry += lev / 3;
	if(f4 & RF4_BR_ACID) restry += 80;
	if(f5 & RF5_BA_ACID) restry += 30;
	if(f6 & RF5_BO_ACID) restry += 20;
	if(f9 & RF9_ACID_STORM) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_IM_ACID;

	restry = 0;
	if(bias == RUBIAS_ELEC ) restry += 90;
	else if(bias == RUBIAS_TOUGHNESS ) restry += lev / 2;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 30 + lev / 2;
	else restry += lev / 3;
	if(f4 & RF4_BR_ELEC) restry += 80;
	if(f4 & RF4_BR_PLAS) restry += 70;
	if(f2 & RF2_AURA_ELEC) restry += 100;
	if(f5 & RF5_BA_ELEC) restry += 30;
	if(f6 & RF5_BO_ELEC) restry += 20;
	if(f9 & RF9_THUNDER_STORM) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_IM_ELEC;

	restry = 0;
	if(bias == RUBIAS_FIRE ) restry += 90;
	else if(bias == RUBIAS_TOUGHNESS ) restry += lev / 2;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 30 + lev / 2;
	else restry += lev / 3;
	if(f4 & RF4_BR_FIRE) restry += 80;
	if(f4 & RF4_BR_PLAS) restry += 70;
	if(f4 & RF4_BR_NUKE) restry += 80;
	if(f2 & RF2_AURA_FIRE) restry += 100;
	if(f5 & RF5_BA_FIRE) restry += 30;
	if(f6 & RF5_BO_FIRE) restry += 20;
	if(f9 & RF9_FIRE_STORM) restry += 50;
	if(f9 & RF9_GIGANTIC_LASER) restry += 70;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_IM_FIRE;

	restry = 0;
	if(bias == RUBIAS_COLD ) restry += 90;
	else if(bias == RUBIAS_TOUGHNESS ) restry += lev / 2;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 30 + lev / 2;
	else restry += lev / 3;
	if(f4 & RF4_BR_COLD) restry += 80;
	if(f2 & RF2_AURA_COLD) restry += 100;
	if(f5 & RF5_BA_COLD) restry += 30;
	if(f5 & RF5_BO_ICEE) restry += 60;
	if(f6 & RF5_BO_COLD) restry += 20;
	if(f9 & RF9_ICE_STORM) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_IM_COLD;

	restry = 0;
	if(bias == RUBIAS_POIS ) restry += 90;
	else if(bias == RUBIAS_TOUGHNESS ) restry += lev / 2;
	else if(bias == RUBIAS_DEMONIC ) restry += 40;
	else if(bias == RUBIAS_NETH ) restry += 40;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 30 + lev / 2;
	else restry += lev / 3;
	if(my_strchr("gjmsuvzGLUVW",d_c)) restry += 50;
	if(f4 & RF4_BR_POIS) restry += 80;
	if(f5 & RF5_BA_POIS) restry += 20;
	if(f9 & RF9_TOXIC_STORM) restry += 60;
	if(f9 & RF9_BA_POLLUTE) restry += 40;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_IM_POIS;

	restry = 0;
	if(bias == RUBIAS_HOLY ) restry += 40;
	else if(bias == RUBIAS_DEMONIC ) restry -= 30;
	else if(bias == RUBIAS_NETH ) restry -= 30;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 20 + lev / 4;
	else restry += lev / 7;
	if(my_strchr("jmuGLTUVW",d_c)) restry -= 50;
	if(my_strchr("A",d_c)) restry += 50;
	if(f4 & RF4_BR_LITE) restry += 70;
	if(f4 & RF4_BR_NUKE) restry += 50;
	if(f5 & RF5_BA_LITE) restry += 50;
	if(f9 & RF9_BEAM_LITE) restry += 20;
	if(f9 & RF9_GIGANTIC_LASER) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_LITE;

	restry = 0;
	if(bias == RUBIAS_DEMONIC ) restry += 60;
	else if(bias == RUBIAS_HOLY ) restry -= 30;
	else if(bias == RUBIAS_NETH ) restry += 20;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 20 + lev / 4;
	else restry += lev / 7;
	if(f3 & RF3_DEMON) restry += 50;
	if(my_strchr("A",d_c)) restry -= 50;
	if(my_strchr("V",d_c)) restry += 200;
	if(my_strchr("ouUWY",d_c)) restry += 50;
	if(f4 & RF4_BR_DARK) restry += 70;
	if(f5 & RF5_BA_DARK) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_DARK;
	if(r_ptr->flagsr & ~(RFR_RES_LITE) && restry >=150) r_ptr->flagsr |= RFR_HURT_LITE;

	restry = 0;
	if(bias == RUBIAS_NETH ) restry += 60;
	else if(bias == RUBIAS_HOLY ) restry -= 30;
	else if(bias == RUBIAS_DEMONIC ) restry += 40;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 20 + lev / 4;
	else restry += lev / 7;
	if(f3 & RF3_UNDEAD) restry += 100;
	if(my_strchr("A",d_c)) restry -= 50;
	if(my_strchr("suLUVW",d_c)) restry += 50;
	if(f4 & RF4_BR_NETH) restry += 70;
	if(f5 & RF5_BA_NETH) restry += 50;
	if(f5 & RF5_BO_NETH) restry += 30;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_NETH;

	restry = 0;
	if(bias == RUBIAS_WATE ) restry += 90;
	else if(bias == RUBIAS_FIRE ) restry -= 50;
	else if(bias == RUBIAS_ELEC ) restry -= 50;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 20 + lev / 4;
	else restry += lev / 7;
	if(f7 & RF7_AQUATIC) restry += 100;
	else if(f7 & RF7_CAN_SWIM) restry += 30;
	if(my_strchr("lK",d_c)) restry += 100;
	if(my_strchr("n",d_c)) restry += 30;
	if(f4 & RF4_BR_AQUA) restry += 50;
	if(f5 & RF5_BA_WATE) restry += 40;
	if(f5 & RF5_BO_WATE) restry += 30;
	if(f9 & RF9_MAELSTROM) restry += 80;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_WATE;

	restry = 0;
	if(bias == RUBIAS_PHYSICAL ) restry += 50;
	else if(bias == RUBIAS_COLD ) restry += 20;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 10 + lev / 5;
	else restry += lev / 16;
	if(my_strchr("G",d_c)) restry += 20;
	if(f4 & RF4_BR_SHAR) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_SHAR;

	restry = 0;
	if(bias == RUBIAS_SOUND ) restry += 70;
	else if(bias == RUBIAS_PHYSICAL ) restry += 30;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 10 + lev / 5;
	else restry += lev / 16;
	if(f4 & RF4_BR_SOUN) restry += 60;
	if(f9 & RF9_BO_SOUND) restry += 30;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_SOUN;

	restry = 0;
	if(bias == RUBIAS_CHAOS ) restry += 80;
	else if(bias == RUBIAS_HOLY ) restry -= 20;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 20 + lev / 4;
	else restry += lev / 7;
	if(f3 & RF3_DEMON) restry += 30;
	if(f3 & RF3_ANG_CHAOS) restry += 30;
	if(f3 & RF3_ANG_COSMOS) restry -= 60;
	if(f4 & RF4_BR_CHAO) restry += 70;
	if(f4 & RF4_BA_CHAO) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_CHAO;

	restry = 0;
	if(bias == RUBIAS_HOLY ) restry += 90;
	else if(bias == RUBIAS_NETH ) restry -= 70;
	else if(bias == RUBIAS_DEMONIC ) restry -= 70;
	else if(bias == RUBIAS_CURSE ) restry -= 50;
	else restry += lev / 10;
	if(f3 & RF3_UNDEAD) restry -= 100;
	if(f3 & RF3_DEMON) restry -= 100;
	if(f3 & RF3_KWAI) restry -= 50;
	if(f3 & RF3_ANG_CHAOS) restry -= 50;
	if(f3 & RF3_ANG_COSMOS) restry += 50;
	if(my_strchr("A",d_c)) restry += 100;
	if(my_strchr("ostuyLTUVWY",d_c)) restry -= 50;
	if(f4 & RF4_BR_HOLY) restry += 70;
	if(f9 & RF9_BA_HOLY) restry += 50;
	if(f9 & RF9_BO_HOLY) restry += 30;
	if(f9 & RF9_PUNISH_3) restry += 20;
	if(f9 & RF9_PUNISH_4) restry += 40;
	if(f9 & RF9_HOLYFIRE) restry += 100;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_HOLY;

	if(restry < -50) r_ptr->flagsr |= RFR_HURT_HOLY;
	else if(restry > 150) r_ptr->flagsr |= RFR_HURT_HELL;

	restry = 0;
	if(bias == RUBIAS_BILE ) restry += 30;
	else if(bias == RUBIAS_POIS ) restry += 20;
	else if(bias == RUBIAS_CHAOS ) restry += 20;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 10 + lev / 5;
	else restry += lev / 10;
	if(f4 & RF4_BR_DISE) restry += 70;
	if(f9 & RF9_BA_POLLUTE) restry += 50;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_DISE;

	if(f4 & RF4_BR_INER) r_ptr->flagsr |= RFR_RES_INER;

	restry = 0;
	if(bias == RUBIAS_SPACE ) restry += 90;
	else if(bias == RUBIAS_MANA ) restry += 50;
	if(my_strchr("FS",d_c)) restry += 20;
	if(f4 & RF4_BR_TIME) restry += 70;
	if(f4 & RF4_BR_NEXU) restry += 80;
	if(f9 & RF9_BA_DISTORTION) restry += 60;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_TIME;

	restry = 0;
	if(bias == RUBIAS_SPACE ) restry += 90;
	else if(bias == RUBIAS_MANA ) restry += 50;
	else if(bias == RUBIAS_MULTI_MAGIC ) restry += 50;
	else if(bias == RUBIAS_TOUGHNESS ) restry += 50;
	else restry += lev / 4;
	if(f2 & RF2_GIGANTIC) restry += 30;
	if(f2 & RF2_POWERFUL) restry += 30;
	if(f4 & RF4_BR_TIME) restry += 70;
	if(f4 & RF4_BR_NEXU) restry += 80;
	if(f9 & RF9_BA_DISTORTION) restry += 60;
	if(f6 & RF6_TPORT) restry += 20;
	if(f6 & RF6_TELE_AWAY) restry += 20;
	if(randint1(100) < restry) r_ptr->flagsr |= RFR_RES_TELE;

	if(!(r_ptr->flagsr & RFR_IM_COLD))
	{
		restry = 0;
		if(bias == RUBIAS_FIRE ) restry += 30;
		if(f4 & RF4_BR_FIRE) restry += 20;
		if(f4 & RF4_BR_PLAS) restry += 10;
		if(f2 & RF2_AURA_FIRE) restry += 30;
		restry -= randint0(lev);
		if(randint1(100) < restry) r_ptr->flagsr |= RFR_HURT_COLD;
	}
	if(!(r_ptr->flagsr & RFR_IM_FIRE))
	{
		restry = 0;
		if(bias == RUBIAS_COLD ) restry += 30;
		if(f4 & RF4_BR_COLD) restry += 20;
		if(f2 & RF2_AURA_COLD) restry += 30;
		restry -= randint0(lev);
		if(randint1(100) < restry) r_ptr->flagsr |= RFR_HURT_FIRE;
	}

}



//v1.1.42
//クエスト用の「夢の世界の＠」を生成するときの調整処理
//apply_random_unique_xtra_param()の後に行う。
void apply_dream_dweller_xtra_param(monster_race *r_ptr)
{

	int i;

	//基本情報
	r_ptr->d_attr = TERM_L_BLUE;
	r_ptr->x_attr = TERM_L_BLUE;
	r_ptr->d_char = '@';
	r_ptr->x_char = '@';
	r_ptr->mexp = 100000;

	r_ptr->flags1 |= (RF1_QUESTOR | RF1_FORCE_MAXHP | RF1_DROP_GREAT);

	if (p_ptr->psex == SEX_MALE)
		r_ptr->flags1 |= (RF1_MALE);
	else
		r_ptr->flags1 |= (RF1_FEMALE);

	//不要フラグ削除
	r_ptr->flags1 &= ~(RF1_ESCORT | RF1_ESCORTS | RF1_CHAR_CLEAR | RF1_SHAPECHANGER| RF1_ATTR_CLEAR | RF1_ATTR_MULTI | RF1_ATTR_SEMIRAND);
	r_ptr->flags2 &= ~(RF2_GIGANTIC);

	//RF3の勢力や種族情報を消去し再設定
	r_ptr->flags3 &= (RF3_NO_CONF | RF3_NO_FEAR | RF3_NO_STUN);
	r_ptr->flags3 |= (RF3_GEN_WARLIKE | RF3_NO_SLEEP);
	//プレイヤー種族情報に記録されている種族フラグを適用
	r_ptr->flags3 |= (rp_ptr->random_unique_flags3);

	if(cp_ptr->flag_only_unique)
		r_ptr->flags7 |= RF7_CAN_FLY;


	//召喚はやめておく。流石にクエストダンジョンで高レベルランダムユニークに召喚を持たせると無理ゲーになる。
	r_ptr->flags6 &= ~(RF6_SUMMON_MASK);
	r_ptr->flags9 &= ~(RF9_SUMMON_MASK);
	r_ptr->flags7 &= ~(RF7_AQUATIC | RF7_RIDING);


	//この先クラス毎に調整
	switch (p_ptr->pclass)
	{
	case	CLASS_WARRIOR:
		r_ptr->blow[3].effect = RBE_SUPERHURT;

		break;
	case	CLASS_MAGE:
		r_ptr->flags6 |= RF6_TPORT;

		break;
	case	CLASS_RANGER:
		//特技「射撃」を付与。
		r_ptr->blow[0].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 30);
		r_ptr->blow[0].d_side = MIN(r_ptr->blow[0].d_side, 20);
		r_ptr->blow[0].method = RBM_SHOOT;
		r_ptr->flags4 |= RF4_SHOOT;
		break;
	case	CLASS_ARCHER:
		//特技「射撃」を付与。
		r_ptr->blow[0].d_dice = MIN(r_ptr->blow[0].d_dice * 3, 40);
		r_ptr->blow[0].d_side = MIN(r_ptr->blow[0].d_side, 30);
		r_ptr->blow[0].method = RBM_SHOOT;
		r_ptr->flags4 |= RF4_SHOOT;


		break;
	case	CLASS_MARTIAL_ARTIST:
	case	CLASS_SYUGEN:
		//隣接攻撃を全てパンチかキックにする
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(2) ? RBM_PUNCH : RBM_KICK);

		break;
	case	CLASS_MINDCRAFTER:
		r_ptr->flags6 |= (RF6_BLINK | RF6_TPORT | RF6_PSY_SPEAR);
		if (one_in_(4)) r_ptr->flags6 |= (RF6_WORLD);

		break;
	case	CLASS_MAGIC_EATER:
		r_ptr->blow[3].method = RBM_TOUCH;
		r_ptr->blow[3].effect = RBE_UN_POWER;

		break;
	case	CLASS_ENGINEER:
		r_ptr->flags4 |= RF4_ROCKET;
		if (one_in_(4)) r_ptr->flags4 |= (RF4_WAVEMOTION);

		break;
	case	CLASS_LIBRARIAN:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_BOOK;

		break;
	case	CLASS_SAMURAI:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_SLASH;

		break;
	case	CLASS_SOLDIER:
		r_ptr->flags4 |= RF4_ROCKET;

		break;
	case	CLASS_CHEMIST:
		r_ptr->flags9 |= (RF9_BA_POLLUTE | RF9_TOXIC_STORM);
		r_ptr->flags6 |= RF6_HEAL;

		break;
	case	CLASS_CAVALRY:
		r_ptr->speed += 5;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(3) ? RBM_CRUSH : RBM_SPEAR);


		break;
	case	CLASS_NINJA:
		r_ptr->flags6 |= RF6_BLINK;
		r_ptr->flags9 |= (RF9_TELE_APPROACH);


		break;
	case	CLASS_RUMIA:
		r_ptr->flags5 |= RF5_BA_DARK;
		r_ptr->blow[3].method = RBM_ENGULF;
		r_ptr->blow[3].effect = RBE_BLIND;

		break;
	case	CLASS_YOUMU:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_SLASH;
		r_ptr->flags9 |= (RF9_TELE_APPROACH);

		break;
	case	CLASS_TEWI:
		r_ptr->flags6 |= RF6_TRAPS;
		r_ptr->blow[3].method = RBM_CHARGE;
		r_ptr->blow[3].effect = RBE_EAT_GOLD;

		break;

	case	CLASS_UDONGE:
		r_ptr->flags4 |= RF4_BR_SOUN;
		r_ptr->flags6 |= RF6_COSMIC_HORROR;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 30);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 20);
		r_ptr->blow[3].method = RBM_GAZE;
		r_ptr->blow[3].effect = RBE_INSANITY;
		break;

	case	CLASS_IKU:
		r_ptr->flags9 |= (RF9_THUNDER_STORM);
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 3, 48);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 20);
		r_ptr->blow[3].method = RBM_SPEAR;
		r_ptr->blow[3].effect = RBE_ELEC;

		break;

	case	CLASS_KOMACHI:
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 32);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 16);
		r_ptr->blow[3].method = RBM_SLASH;
		r_ptr->blow[3].effect = RBE_KILL;
		r_ptr->flags6 |= RF6_TPORT;
		r_ptr->flags9 |= (RF9_TELE_APPROACH);
		break;

	case	CLASS_KOGASA:
		r_ptr->flags5 |= RF5_BA_LITE;
		r_ptr->blow[3].effect = RBE_TERRIFY;
		break;

	case	CLASS_KASEN:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_PUNCH;
		r_ptr->flags4 |= RF4_BR_SOUN;
		break;

	case	CLASS_SUIKA:
		if (r_ptr->hdice * r_ptr->hside < 10000)
		{
			r_ptr->hdice = 100;
			r_ptr->hside = 100;
		}
		for (i = 0; i<4; i++)
		{
			r_ptr->blow[i].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 24);
			r_ptr->blow[i].effect = RBE_SHATTER;
		}
		r_ptr->flags4 |= RF4_BR_FIRE;

		break;

	case	CLASS_MARISA:
		r_ptr->flags9 |= RF9_FINALSPARK;

		break;

	case	CLASS_WAKASAGI:
		r_ptr->flags9 |= RF9_MAELSTROM;
		r_ptr->flags4 |= RF4_BR_AQUA;
		r_ptr->flags7 |= RF7_CAN_SWIM;
		break;

	case	CLASS_KOISHI: //クエスト受領不可の予定だが一応
		r_ptr->flags6 |= RF6_COSMIC_HORROR;
		r_ptr->flags2 |= RF2_EMPTY_MIND;
		r_ptr->flags6 |= RF6_TPORT;

		break;

	case	CLASS_MOMIZI:
		r_ptr->ac += 30;
		for (i = 0; i<4; i++) if (one_in_(2)) r_ptr->blow[i].method = RBM_SLASH;

		break;

	case	CLASS_SEIGA:
		r_ptr->flags2 |= RF2_PASS_WALL;
		r_ptr->flags5 |= RF5_BA_DARK;

		break;

	case	CLASS_CIRNO:
		//＠が冷気免疫のチルノなのでこんな特技ないほうが強いがまあフレーバーとして
		r_ptr->flags9 |= RF9_ICE_STORM;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 3, 48);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 20);
		r_ptr->blow[3].method = RBM_TOUCH;
		r_ptr->blow[3].effect = RBE_COLD;

		break;

	case	CLASS_ORIN:
		r_ptr->flags9 |= RF9_FIRE_STORM;
		r_ptr->flags5 |= RF5_BA_NETH;

		break;

	case	CLASS_SHINMYOUMARU:
	case	CLASS_SHINMYOU_2:
		for (i = 0; i<4; i++) r_ptr->blow[i].method = RBM_SPEAR;
		for (i = 0; i<4; i++) if (one_in_(2)) r_ptr->blow[i].effect = RBE_BLIND;
		r_ptr->flags6 |= (RF6_PSY_SPEAR);

		break;

	case	CLASS_NAZRIN:
		r_ptr->flags5 |= RF5_BA_LITE;
		break;

	case	CLASS_LETTY:
		for (i = 0; i<4; i++) if (one_in_(2)) r_ptr->blow[i].effect = RBE_INERTIA;
		r_ptr->flags9 |= RF9_ICE_STORM;


		break;

	case	CLASS_YOSHIKA:
		for (i = 0; i<4; i++) r_ptr->blow[i].method = (one_in_(2) ? RBM_CLAW : RBM_BITE);
		for (i = 0; i<4; i++) r_ptr->blow[i].effect = (one_in_(2) ? RBE_POISON : RBE_CURSE);
		r_ptr->flags6 |= RF6_HEAL;

		break;

	case	CLASS_PATCHOULI:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= RF5_BA_MANA;
		r_ptr->flags9 |= (RF9_FIRE_STORM | RF9_ICE_STORM | RF9_THUNDER_STORM);
		if (r_ptr->freq_spell < 50) r_ptr->freq_spell = 50;

		break;

	case	CLASS_AYA:
		r_ptr->speed += 10;
		r_ptr->flags9 |= (RF9_TORNADO | RF9_TELE_APPROACH);
		r_ptr->blow[3].method = RBM_HIT;
		r_ptr->blow[3].effect = RBE_PHOTO;

		break;

	case	CLASS_BANKI:
		r_ptr->flags5 |= RF5_BA_LITE;
		r_ptr->flags9 |= RF9_BEAM_LITE;

		break;

	case	CLASS_MYSTIA:
		r_ptr->flags4 |= RF4_BR_SOUN;
		for (i = 0; i<4; i++) if (one_in_(2))
		{
			r_ptr->blow[i].method = RBM_SHOW;
			r_ptr->blow[i].effect = RBE_BLIND;
		}

		break;

	case	CLASS_FLAN:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= (RF9_FIRE_STORM | RF9_HELLFIRE);
		r_ptr->flags6 |= (RF6_PSY_SPEAR);
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 4, 64);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 30);
		r_ptr->blow[3].method = RBM_SLASH;
		r_ptr->blow[3].effect = RBE_FIRE;

		break;

	case	CLASS_SHOU:
		r_ptr->flags5 |= RF5_BA_LITE;
		r_ptr->flags9 |= (RF9_HOLYFIRE);

		break;

	case	CLASS_MEDICINE:
		r_ptr->flags9 |= (RF9_TOXIC_STORM);
		r_ptr->flags6 |= RF6_HEAL;


		break;

	case	CLASS_YUYUKO:
		r_ptr->flags2 |= RF2_PASS_WALL;
		r_ptr->flags5 |= RF5_BA_DARK | RF5_BA_MANA;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 32);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 16);
		r_ptr->blow[3].method = RBM_POINT;
		r_ptr->blow[3].effect = RBE_KILL;

		break;

	case	CLASS_SATORI:
		r_ptr->flags2 |= RF2_SMART_EX;
		r_ptr->flags5 |= RF5_BRAIN_SMASH;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[0].d_dice * 2, 30);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[0].d_side, 20);
		r_ptr->blow[3].method = RBM_GAZE;
		r_ptr->blow[3].effect = RBE_INSANITY;

		break;

	case	CLASS_KYOUKO:
		r_ptr->flags2 |= RF2_REFLECTING;
		r_ptr->flags4 |= RF4_BR_SOUN;
		for (i = 0; i<4; i++) if (one_in_(2))
		{
			r_ptr->blow[i].method = RBM_SHOW;
			r_ptr->blow[i].effect = RBE_STUN;
		}

		break;

	case	CLASS_TOZIKO:
		r_ptr->flags2 |= RF2_PASS_WALL;
		r_ptr->flags9 |= RF9_THUNDER_STORM;
		r_ptr->flags5 |= RF5_BO_PLAS;
		break;

	case CLASS_KISUME:
		r_ptr->flags9 |= RF9_FIRE_STORM;
		r_ptr->flags5 |= RF5_BA_NETH;
		break;

	case CLASS_HATATE:
		r_ptr->flags9 |= RF9_TORNADO;
		r_ptr->flags5 |= RF5_BA_LITE;
		break;

	case CLASS_MIKO:
		r_ptr->flags5 |= (RF5_BA_LITE | RF5_BA_MANA);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[3].d_dice * 3, 30);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[3].d_side, 20);
		r_ptr->blow[3].method = RBM_SLASH;
		r_ptr->blow[3].effect = RBE_SMITE;
		break;

	case CLASS_KOKORO:
		r_ptr->flags5 |= (RF5_BA_LITE | RF5_BA_DARK);
		r_ptr->flags6 |= RF6_COSMIC_HORROR;
		r_ptr->blow[3].d_dice = MIN(r_ptr->blow[3].d_dice * 2, 30);
		r_ptr->blow[3].d_side = MIN(r_ptr->blow[3].d_side, 20);
		r_ptr->blow[3].method = RBM_SLASH;
		r_ptr->blow[3].effect = RBE_TERRIFY;

		break;

	case CLASS_WRIGGLE:
		r_ptr->flags5 |= (RF5_BA_LITE);

		break;

	case CLASS_YUUKA:
		r_ptr->flags4 |= (RF4_DISPEL);
		for (i = 0; i<4; i++) r_ptr->blow[i].method = RBM_STRIKE;
		break;

	case CLASS_CHEN:
		for (i = 0; i<4; i++) r_ptr->blow[i].method = RBM_CLAW;
		break;

	case CLASS_MURASA:
		r_ptr->flags9 |= RF9_MAELSTROM;
		for (i = 0; i<4; i++) if (one_in_(2))
		{
			r_ptr->blow[i].d_dice = MIN(r_ptr->blow[i].d_dice * 2, 30);
			r_ptr->blow[i].method = RBM_DRAG;
			r_ptr->blow[i].effect = RBE_DROWN;
		}

		break;

	case CLASS_KEINE:
		r_ptr->flags5 |= (RF5_BA_LITE);
		for (i = 0; i<4; i++) r_ptr->blow[i].method = (one_in_(3) ? RBM_HEADBUTT : RBM_SLASH);

		break;

	case CLASS_REIMU:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags6 |= (RF6_TPORT | RF6_BLINK);
		r_ptr->flags9 |= RF9_HOLYFIRE;
		r_ptr->flags6 |= RF6_INVULNER;

		break;

	case CLASS_YUGI:
		for (i = 0; i<4; i++)
		{
			r_ptr->blow[i].method = RBM_PUNCH;
			r_ptr->blow[i].effect = RBE_SUPERHURT;
		}
		break;

	case CLASS_KAGEROU:
		for (i = 0; i<4; i++) if(one_in_(2))
		{
			r_ptr->blow[i].method = RBM_BITE;
			r_ptr->blow[i].effect = RBE_SUPERHURT;
		}
		break;

	case CLASS_SHIZUHA:
		for (i = 0; i<4; i++) if (one_in_(2))
		{
			r_ptr->blow[i].method = RBM_KICK;
			r_ptr->blow[i].effect = RBE_SUPERHURT;
		}
		break;

	case CLASS_SANAE:
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags9 |= (RF9_HOLYFIRE | RF9_TORNADO | RF9_MAELSTROM);
		break;
	case CLASS_MINORIKO:
		r_ptr->flags4 |= (RF4_BA_FORCE);
		r_ptr->flags9 |= (RF9_TORNADO);
		break;
	case CLASS_REMY:
		r_ptr->flags6 |= RF6_PSY_SPEAR;
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags9 |= RF9_BA_METEOR;

		break;

	case CLASS_NITORI:
		r_ptr->flags4 |= (RF4_ROCKET);
		r_ptr->flags9 |= (RF9_MAELSTROM);
		break;
	case CLASS_BYAKUREN:
		r_ptr->flags4 |= (RF4_DISPEL);
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(2) ? RBM_PUNCH : RBM_KICK);
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags6 |= RF6_PSY_SPEAR;
		r_ptr->flags6 |= RF6_HASTE;

		break;
	case CLASS_SUWAKO:
		r_ptr->flags4 |= (RF4_BR_HELL | RF4_BR_AQUA);

		break;
	case CLASS_NUE:
		r_ptr->flags4 |= RF4_BR_NETH;
		r_ptr->flags5 |= (RF5_BA_DARK);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags6 |= RF6_COSMIC_HORROR;

		break;
	case CLASS_UTSUHO:
		r_ptr->flags5 |= RF5_BA_LITE;
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_BA_METEOR);
		break;
	case CLASS_YAMAME:
		r_ptr->flags4 |= RF4_BR_POIS;
		r_ptr->flags4 |= RF4_BR_NETH;

		break;
	case CLASS_RAN:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= RF5_BA_MANA;
		break;
	case CLASS_EIKI:
		r_ptr->flags9 |= (RF9_HELLFIRE | RF9_HOLYFIRE);

		break;
	case CLASS_TENSHI:
		for (i = 0; i<4; i++) if (one_in_(2)) r_ptr->blow[i].method = (RBM_SLASH);
		r_ptr->flags9 |= (RF9_BA_METEOR | RF9_GIGANTIC_LASER);
		r_ptr->flags6 |= RF6_PSY_SPEAR;

		break;
	case CLASS_MEIRIN:
		r_ptr->flags4 |= RF4_BA_FORCE;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(2) ? RBM_PUNCH : RBM_KICK);

		break;
	case CLASS_PARSEE:
		r_ptr->flags5 |= (RF5_BA_DARK);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		break;

	case CLASS_SUMIREKO:
		r_ptr->flags6 |= RF6_TPORT;
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_BA_DISTORTION);
		r_ptr->flags9 |= (RF9_BA_METEOR);

		break;
	case CLASS_ICHIRIN:
		for (i = 0; i<4; i++)
		{
			r_ptr->blow[i].method = RBM_PUNCH;
		}
		break;
	case CLASS_MOKOU:
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_HOLYFIRE);
		r_ptr->flags6 |= RF6_INVULNER;

		break;
	case CLASS_KANAKO:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= (RF9_BA_METEOR);
		for (i = 0; i<4; i++)
		{
			r_ptr->blow[i].method = RBM_PRESS;
		}
		break;
	case CLASS_FUTO:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_ICE_STORM);
		r_ptr->flags9 |= (RF9_THUNDER_STORM);
		r_ptr->flags9 |= (RF9_TORNADO);
		r_ptr->flags9 |= (RF9_MAELSTROM);

		break;
	case CLASS_SUNNY:
		r_ptr->flags5 |= (RF5_BA_LITE);
		break;
	case CLASS_LUNAR:
		r_ptr->flags5 |= (RF5_BA_DARK);
		break;
	case CLASS_STAR:
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		break ;
	case CLASS_ALICE:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_ICE_STORM);
		r_ptr->flags9 |= (RF9_THUNDER_STORM);
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags5 |= (RF5_BA_DARK);
		if (r_ptr->freq_spell < 50) r_ptr->freq_spell = 50;

		break;
	case CLASS_LUNASA:
	case CLASS_MERLIN:
	case CLASS_LYRICA:
		r_ptr->flags4 |= RF4_BR_SOUN;
		r_ptr->flags9 |= RF9_BO_SOUND;

		break;
	case CLASS_CLOWNPIECE:
	case CLASS_VFS_CLOWNPIECE:
		r_ptr->flags9 |= RF9_FIRE_STORM;
		r_ptr->flags9 |= RF9_HELLFIRE;
		r_ptr->flags6 |= RF6_HASTE;

		break;
	case CLASS_BENBEN:
	case CLASS_YATSUHASHI:
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags9 |= RF9_BO_SOUND;
		break;
	case CLASS_HINA:
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags5 |= RF5_BA_DARK;
		break;
	case CLASS_SAKUYA:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(4) ? RBM_KICK : RBM_SLASH);
		r_ptr->flags9 |= RF9_BA_DISTORTION;
		r_ptr->flags6 |= RF6_WORLD;
		break;

	case	CLASS_3_FAIRIES:
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags5 |= (RF5_BA_DARK);
		break;

	case CLASS_RAIKO:
		r_ptr->flags9 |= RF9_THUNDER_STORM;
		r_ptr->flags4 |= RF4_BR_SOUN;
		r_ptr->flags4 |= RF4_ROCKET;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (one_in_(2) ? RBM_STRIKE : RBM_PLAY);
		break;

	case CLASS_MAMIZOU:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= RF9_BA_METEOR;
		break;
	case CLASS_YUKARI:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= RF9_BA_METEOR;
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags5 |= (RF5_BA_DARK);
		r_ptr->flags5 |= (RF5_BA_MANA);
		r_ptr->flags6 |= (RF6_HAND_DOOM);
		break;
	case CLASS_RINGO:
		r_ptr->flags4 |= RF4_ROCKET;
		r_ptr->flags4 |= RF4_BA_FORCE;
		r_ptr->flags6 |= RF6_HEAL;
		break;

	case CLASS_SEIRAN:
		r_ptr->flags4 |= RF4_ROCKET;
		r_ptr->flags9 |= RF9_GIGANTIC_LASER;

		break;
	case CLASS_EIRIN:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags5 |= (RF5_BA_MANA);
		r_ptr->flags6 |= RF6_HEAL;
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags6 |= RF6_INVULNER;
		r_ptr->flags9 |= (RF9_TOXIC_STORM);

		break;
	case CLASS_KAGUYA:
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_ICE_STORM);
		r_ptr->flags9 |= (RF9_THUNDER_STORM);
		r_ptr->flags9 |= (RF9_ACID_STORM);
		r_ptr->flags5 |= (RF5_BA_LITE);
		r_ptr->flags5 |= (RF5_BA_DARK);
		r_ptr->flags5 |= (RF5_BA_MANA);
		break;
	case CLASS_SAGUME:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags6 |= RF6_INVULNER;

		break;
	case CLASS_REISEN2:
		r_ptr->flags4 |= RF4_ROCKET;
		break;
	case CLASS_TOYOHIME:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags4 |= RF4_WAVEMOTION;
		r_ptr->flags9 |= RF9_MAELSTROM;

		break;
	case CLASS_YORIHIME:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags9 |= (RF9_FIRE_STORM);
		r_ptr->flags9 |= (RF9_THUNDER_STORM);
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (RBM_SLASH);
		break;
	case CLASS_JUNKO:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= (RF5_BA_MANA);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags6 |= RF6_COSMIC_HORROR;
		r_ptr->flags6 |= RF6_INVULNER;
		break;

	case CLASS_HECATIA:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= (RF5_BA_MANA);
		r_ptr->flags6 |= RF6_HAND_DOOM;
		r_ptr->flags6 |= RF6_INVULNER;
		r_ptr->flags9 |= RF9_BA_METEOR;
		break;

	case CLASS_NEMUNO:
		r_ptr->flags6 |= RF6_INVULNER;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = (RBM_SLASH);

		break;
	case CLASS_AUNN:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags4 |= (RF4_BR_SOUN);

		break;
	case CLASS_NARUMI:
		r_ptr->flags4 |= (RF4_DISPEL);
		r_ptr->flags5 |= (RF5_BA_MANA);
		r_ptr->flags9 |= (RF9_HOLYFIRE);
		break;
	case CLASS_LARVA:
		r_ptr->flags4 |= (RF4_BR_CHAO);
		break;

	case CLASS_MAI:
		r_ptr->flags6 |= RF6_PSY_SPEAR;
		break;

	case CLASS_SATONO:
		r_ptr->flags6 |= RF6_FORGET;
		break;

	case CLASS_JYOON:
		r_ptr->flags4 |= RF4_DISPEL;
		r_ptr->flags5 |= RF5_BA_LITE;
		r_ptr->flags9 |= RF9_TELE_APPROACH;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_PUNCH;
		break;

	case CLASS_MAYUMI:
		r_ptr->flags3 |= RF3_HANIWA;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_SLASH;
		break;

	case CLASS_EIKA:
		r_ptr->flagsr |= RFR_RES_WATE;
		r_ptr->flags9 |= RF9_MAELSTROM;
		break;

	case CLASS_KUTAKA:
		r_ptr->flags5 |= RF5_BA_LITE;
		break;

	case CLASS_URUMI:
		r_ptr->flagsr |= RFR_RES_WATE;
		r_ptr->flags9 |= RF9_MAELSTROM;
		break;

	case CLASS_SAKI:
		r_ptr->flags9 |= RF9_TELE_APPROACH;
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_KICK;
		break;

	case CLASS_YACHIE:
		r_ptr->ac += 50;
		r_ptr->flags9 |= RF6_COSMIC_HORROR;

		break;

	case CLASS_KEIKI:
		for (i = 0; i<4; i++) if (r_ptr->blow[i].method) r_ptr->blow[i].method = RBM_STING;
		r_ptr->flagsr |= RFR_RES_WATE;
		r_ptr->flags9 |= RF9_MAELSTROM;
		break;

	case CLASS_MOMOYO:
		r_ptr->flagsr |= RFR_IM_POIS;
		r_ptr->flags4 |= RF4_BR_POIS;

		break;


	}

	if (cp_ptr->magicmaster)
	{
		if (r_ptr->freq_spell < 33) r_ptr->freq_spell = 33;
		r_ptr->flags2 &= ~(RF2_STUPID);
		r_ptr->flags2 |= RF2_SMART;
	}

	//耐性入れ忘れてたがもういいか。大抵ランダム付与で入っているだろう。

}




#define RU_NAMEPOWER_LOW	1
#define RU_NAMEPOWER_MED	2
#define RU_NAMEPOWER_HIGH	3
#define RU_NAMEPOWER_MAX	4

/*:::ランダムユニークへ命名し、random_unique_name[][]へ格納する。*/
/*:::鈴奈庵に倣って「○○(日本語)の□□(カタカナ)《△△(ランダム名)》という形式にする。*/
void get_random_unique_name(int r_idx, monster_race *r_ptr, int power, int bias)
{
	int namepower;
	char name_total[200];
	char name_biasclass[50];
	char name_prefix[50];
	char name_rnd[100];

	int biasname_num;
	int biasname_max;

#ifdef JP
	cptr biasname_toughness[] = {"ディフェンダー","ゲートキーパー","アイアンメイデン","ガーディアン","クロムハート","ダイハード","フォートレス","コロッサス","イモータル"};
	cptr biasname_power[] = {"ファイター","クラッシャー","ブレイカー","スライサー","アイアンフィスト","チャリオット","ウォーロード","ジェノサイダー","デストロイヤー"};
	cptr biasname_speed[] = {"トロッター","ハイウェイランナー","スピードキング","アローヘッド","ファルコン","スピードスター","ルナシャドウ","ハイマスター"};
	cptr biasname_bile[] = {"スライムマニア","プロトプラズム","メルター","デコンポーザー","ディスエンチャンター","ディソルバー","コラプター","ユータナシア"};
	cptr biasname_elec[] = {"スタナー","ディスチャージャー","レールガン","ライトニング","ストームコーザー","オーバーロード","ガンマバースト"};
	cptr biasname_cold[] = {"アイスディーラー","フリーザー","スノークラッド","コールドハート","スパインチラー","アイスエイジ","エントロピー","アブソリュートゼロ"};
	cptr biasname_fire[] = {"トーチャー","ファイアイーター","アーソニスト","バックドラフト","ブロイラー","フレイムスロワー","デトネイター","エクスプロージョナー","メルトダウナー","フレアスター"};
	cptr biasname_pois[] = {"ポイズンスピッター","ヴェノムハンター","アルケミスト","ポイズンマーダー","トキシックレディ","ウィッチドクター","ヴェネフィック","コンタミネーター","アハルメス"};
	cptr biasname_demonic[] = {"ヘマトフィリア","ファナティック","サタニスト","フィーンド","デビルサマナー","マーダラスト","ブラッドサッカー","デーモンロード","アンリ・マンユ"};
	cptr biasname_holy[] = {"ヒーラー","エクソシスト","シーアルジスト","シャーマン","エクスキューショナー","インクイジター","パニッシャー","セイント","アポストル","ゴッドハンド","ディヴィニティ","ネメシス"};
	cptr biasname_wate[] = {"フィッシャー","ベントス","パイレート","レインメーカー","マリナー","ネレイド","オーシャンワンダー","アビサルマローダ","ディープワン","インコグニタ"};
	cptr biasname_neth[] = {"グレイブディッガー","スカベンジャー","キャリオンフィーダー","グレイブダンサー","レヴナント","ネクロマンサー","フロムヘル","グリムリーパー","ウィッチキング","タナトス"};
	cptr biasname_curse[] = {"オカルティスト","タナトフィリア","サイコパス","ウィッチ","マインドシーカー","カースメーカー","サイオニック","ルイナスオーメン","ドゥーム","ラグナロク"};
	cptr biasname_physical[] = {"ギーク","エンジニア","オートマトン","ボマー","コッペリア","ウォーモンガー","ワンマンアーミー","リーサルウェポン","デウスエクスマキナ"};
	cptr biasname_sound[] = {"シンガー","グラスホッパー","ロアリングビースト","キャロル","バンシー","スクリーマー","コーラスマスター","プリマドンナ","レゾネーター","ディーヴァ","トランペッター"};
	cptr biasname_chaos[] = {"フェイカー","ディスガイザー","トリックスター","シェイプチェンジャー","カオスビジター","ディスオーダー","ポリモーファー","ログルスマスター","ワンダー","アーキタイプ"};
	cptr biasname_space[] = {"マジシャン","フォーチュンテラー","スターゲイザー","タイムトラベラー","インベーダー","ディストーター","コメットサマナー","ムーンキャンサー","マントルバンカー","アナザーゲート","シューティングスター"};
	cptr biasname_summon[] = {"コンダクター","コンジャラー","コントラクター","サマナー","テウルギスト","クイーン","ドミネーター","クロスボーダー","ゲートオープナー","ファンタズマゴリア"};
	cptr biasname_insanity[] = {"ラヴクラフティアン","マッドネス","インスマスルック","ブレインイーター","オールドワン","インサニティ","グレートオールドワン","クロウリングケイオス","アウターゴッド"};
	cptr biasname_mana[] = {"ミスティック","ウォーメイジ","ワーロック","ロードマンサー","ソーサラー","ワイズマン","アークウィザード","アルティメットメイガス","アルマゲスト"};
	cptr biasname_multi_magic[] = {"ライブラリアン","ドルイド","セージ","マルチキャスター","ジオマスター","エーテルマスター","ガイアマンサー","ナインテール","オムニポーテント"};
	cptr biasname_multi_breath[] = {"ヒュージマウス","コンパウンド","ヒドラ","トライヘッド","マルチフェイス","ギドラ","ナインヘッド","キングギドラ","パンデモニウム","ラストダンサー"};
	cptr biasname_none[] = {"ニート","インファント","シビリアン","メイド","トリックスター","ワンダラスト","アウトサイダー","プロディジー","インフィニティア"};
#else
    cptr biasname_toughness[] = {"Defender","Gatekeeper","Iron Maiden","Guardian","Chrome Heart","Diehard","Fortress","Colossus","Immortal"};
	cptr biasname_power[] = {"Fighter","Crasher","Breaker","Slicer","Iron Fist","Chariot","Warlord","Genocider","Destroyer"};
	cptr biasname_speed[] = {"Trotter","Highway Runner","Speed King","Arrowhead","Falcon","Speedster","Luna Shadow","High Master"};
	cptr biasname_bile[] = {"Slime Mania","Protoplasm","Melter","Decomposer","Disenchanter","Dissolver","Corruptor","Euthanasia"};
	cptr biasname_elec[] = {"Stunner","Discharger","Railgun","Lightning","Storm Causer","Overload","Gamma Burst"};
	cptr biasname_cold[] = {"Ice Dealer","Freezer","Snow Cloud","Cold Heart","Spine Chiller","Ice Edge","Entropy","Absolute Zero"};
	cptr biasname_fire[] = {"Torcher","Fire Eater","Arsonist","Backdraft","Broiler","Flamethrower","Detonator","Explosioner","Meltdowner","Flare Star"};
	cptr biasname_pois[] = {"Poison Spitter","Venom Hunter","Alchemist","Poison Murder","Toxic Lady","Witch Doctor","Venefic","Contaminator","Aharmes"};
	cptr biasname_demonic[] = {"Hematophiliac","Fanatic","Satanist","Fiend","Devil Summoner","Murder Last","Bloodsucker","Demon Lord","Ahri Man'yu"};
	cptr biasname_holy[] = {"Healer","Exorcist","Theologist","Shaman","Executioner","Inquisitor","Punisher","Saint","Apostle","God Hand","Divinity","Nemesis"};
	cptr biasname_wate[] = {"Fisher","Benthos","Pirate","Rain Maker","Mariner","Nereid","Ocean Shower","Abyssal Marauder","Deep One","Incognita"};
	cptr biasname_neth[] = {"Gravedigger","Scavenger","Carrion Feeder","Gravedancer","Revenant","Necromancer","One From Hell","Grim Reaper","Witch-King","Thanatos"};
	cptr biasname_curse[] = {"Occultist","Thanatophiliac","Psychopath","Witch","Mind Seeker","Curse Maker","Psionic","Ruinous Omen","Doom","Ragnarok"};
	cptr biasname_physical[] = {"Geek","Engineer","Automaton","Bomber","Coppelia","War Monger","Army of One","Lethal Weapon","Deus Ex Machina"};
	cptr biasname_sound[] = {"Singer","Grasshopper","Roaring Beast","Carol","Banshee","Screamer","Chorus Master","Prima Donna","Resonator","Diva","Trumpeter"};
	cptr biasname_chaos[] = {"Faker","Disguiser","Trickster","Shapeshifter","Chaos Visitor","Disorder","Polymorpger","Logrus Master","Wonder","Archetype"};
	cptr biasname_space[] = {"Magician","Fortune Teller","Stargazer","Time Traveller","Invader","Distorter","Comet Summoner","Moon Cnacer","Mantle Bunker","Another Gate","Shooting Star"};
	cptr biasname_summon[] = {"Conductor","Conjurer","Contractor","Summoner","Theurgist","Queen","Dominator","Cross Border","Gate Opener","Phantasmagoria"};
	cptr biasname_insanity[] = {"Lovecraftian","Madness","Innsmouth","Brain Eater","Old One","Insanity","Great Old One","Crawling Chaos","Outer God"};
	cptr biasname_mana[] = {"Mystic","War Mage","Warlock","Lord Mancer","Sorcerer","Wiseman","Archwizard","Ultimate Magus","Almagest"};
	cptr biasname_multi_magic[] = {"Librarian","Druid","Sage","Multicaster","Geomaster","Ether Master","Gaiamancer","Nine Tales","Omnipotent"};
	cptr biasname_multi_breath[] = {"Huge Mouth","Compound","Hydra","Trihead","Multi-Face","Ghidorah","Nine-Head","King Ghidorah","Pandemonium","Last Dancer"};
	cptr biasname_none[] = {"Neet","Infant","Civilian","Maid","Trickster","Wonder Last","Outsider","Prodigy","Infinity"};
#endif

	//ドッペルゲンガー生成特殊処理
	if(flag_generate_doppelganger)
	{
		sprintf(name_total,_("《%s》", "<%s>"),player_name);
		my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1],name_total,sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1])-2);
		return;
	}

	if (flag_generate_lilywhite)
	{

		sprintf(name_total, _("『リリーホワイト』", "Lily White"));
		my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1], name_total, sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]) - 2);
		return;

	}

	//夢の世界の＠生成特殊処理
	if (p_ptr->inside_quest == QUEST_DREAMDWELLER || p_ptr->inside_arena)
	{
		if(is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
			sprintf(name_total, _("本物の『%s』", "True %s"), player_name);
		else
			sprintf(name_total, _("夢の世界の『%s』", "%s of the Dream World"), player_name);

		my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1], name_total, sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]) - 2);
		return;
	}

	if((power + r_ptr->level / 3) < 30) namepower = RU_NAMEPOWER_LOW;
	else if((power + r_ptr->level / 3) < 60) namepower = RU_NAMEPOWER_MED;
	else if((power + r_ptr->level / 3) < 120) namepower = RU_NAMEPOWER_HIGH;
	else namepower = RU_NAMEPOWER_MAX;

	//msg_format("test:%d",sizeof(biasname_none) / sizeof(cptr));

	while(1)
	{
		/*:::ランダムな接頭語を得る。レベルとパワーで4段階*/
#ifdef JP
		if(get_rnd_line(_("ru_name_prefix.txt", "ru_name_prefix_e.txt"),namepower,name_prefix))
            msg_print(_("ERROR:ランダムユニーク名接頭語を得られない", "ERROR: Failed to get random unique name prefixes"));
#endif // JP

		/*:::バイアスに応じて職業っぽい中間名を得る。パワーが高いほど配列の後のものが出やすいようにしておく*/
		switch(bias)
		{
		case RUBIAS_TOUGHNESS:
			biasname_max = sizeof(biasname_toughness) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
			//biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			//↑なんか知らんがこのルーチンだとMINが働かない。
			//↑よく考えたら条件判断と代入の所でそれぞれ別のランダム値が適用される。働かなくて当然だ。
			sprintf(name_biasclass,"%s",biasname_toughness[biasname_num]);
			break;
		case RUBIAS_POWER:
			biasname_max = sizeof(biasname_power) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_power[biasname_num]);
			break;
		case RUBIAS_SPEED:
			biasname_max = sizeof(biasname_speed) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_speed[biasname_num]);
			break;
		case RUBIAS_BILE:
			biasname_max = sizeof(biasname_bile) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_bile[biasname_num]);
			break;
		case RUBIAS_ELEC:
			biasname_max = sizeof(biasname_elec) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_elec[biasname_num]);
			break;
		case RUBIAS_COLD:
			biasname_max = sizeof(biasname_cold) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_cold[biasname_num]);
			break;
		case RUBIAS_FIRE:
			biasname_max = sizeof(biasname_fire) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_fire[biasname_num]);
			break;
		case RUBIAS_POIS:
			biasname_max = sizeof(biasname_pois) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_pois[biasname_num]);
			break;
		case RUBIAS_DEMONIC:
			biasname_max = sizeof(biasname_demonic) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_demonic[biasname_num]);
			break;
		case RUBIAS_HOLY:
			biasname_max = sizeof(biasname_holy) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_holy[biasname_num]);
			break;
		case RUBIAS_WATE:
			biasname_max = sizeof(biasname_wate) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_wate[biasname_num]);
			break;
		case RUBIAS_NETH:
			biasname_max = sizeof(biasname_neth) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_neth[biasname_num]);
			break;
		case RUBIAS_CURSE:
			biasname_max = sizeof(biasname_curse) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_curse[biasname_num]);
			break;
		case RUBIAS_PHYSICAL:
			biasname_max = sizeof(biasname_physical) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_physical[biasname_num]);
			break;
		case RUBIAS_SOUND:
			biasname_max = sizeof(biasname_sound) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_sound[biasname_num]);
			break;
		case RUBIAS_CHAOS:
			biasname_max = sizeof(biasname_chaos) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_chaos[biasname_num]);
			break;
		case RUBIAS_SPACE:
			biasname_max = sizeof(biasname_space) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_space[biasname_num]);
			break;
		case RUBIAS_SUMMON:
			biasname_max = sizeof(biasname_summon) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_summon[biasname_num]);
			break;
		case RUBIAS_INSANITY:
			biasname_max = sizeof(biasname_insanity) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_insanity[biasname_num]);
			break;
		case RUBIAS_MANA:
			biasname_max = sizeof(biasname_mana) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_mana[biasname_num]);
			break;
		case RUBIAS_MULTI_MAGIC:
			biasname_max = sizeof(biasname_multi_magic) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_multi_magic[biasname_num]);
			break;

		case RUBIAS_MULTI_BREATH:
			biasname_max = sizeof(biasname_multi_breath) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_multi_breath[biasname_num]);
			break;
		default:
			biasname_max = sizeof(biasname_none) / sizeof(cptr)-1;
			biasname_num = (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max);
			if(biasname_num > biasname_max) biasname_num = biasname_max;
			if(biasname_num < 0) biasname_num = 0;
//			biasname_num = MAX(0, MIN(biasname_max,( (power + r_ptr->level / 3 + randint0(50) - 25) / (120/biasname_max) )));
			sprintf(name_biasclass,"%s",biasname_none[biasname_num]);
			break;
		}

		//msg_format("Bias:%d biasname_max:%d biasnum:%d",bias,biasname_max,biasname_num);

		/*:::完全にランダムな名前を得る。女性かそうでないかで分岐、女性以外はさらにリストかシンダリン名から選ぶ*/
		if(r_ptr->flags1 & RF1_FEMALE)
		{
			if(get_rnd_line(_("ru_namelist_female.txt", "ru_namelist_female_e.txt"),0,name_rnd))
                msg_print(_("ERROR:ランダムユニーク名女性名を得られない", "ERROR: Failed to get random unique female names"));
		}
		else if(one_in_(3))
		{
			if(get_rnd_line(_("ru_namelist_monster.txt", "ru_namelist_monster_e.txt"),0,name_rnd))
                msg_print(_("ERROR:ランダムユニーク名モンスター名を得られない", "ERROR: Failed to get random unique male names"));
		}
		else
		{
			get_table_sindarin(name_rnd,FALSE);
		}

#ifdef JP
		sprintf(name_total,_("%s%s《%s》", "%s%s <%s>"),name_prefix,name_biasclass,name_rnd);
#else
        sprintf(name_total,"%s, the %s", name_rnd, name_biasclass);
#endif
		//msg_format("Len:%d",strlen(name_total));
		if(strlen(name_total) < 65) break; //32文字を超えるならつけ直す

	}
	my_strcpy(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1],name_total,sizeof(random_unique_name[r_idx - MON_RANDOM_UNIQUE_1])-2);

	if(cheat_hear) msg_format("%s(Lev:%d Power:%d)",random_unique_name[r_idx - MON_RANDOM_UNIQUE_1],r_ptr->level,power);
}

/*:::ランダムユニークモンスターのパラメータ設定をする(ここが一番上)*/
bool apply_random_unique(int r_idx)
{

	int bias=0;
	int power=0;
	int lev;
	byte d_char;
	int i;
	int symbol_num;
	int lev2,lev3;
	monster_race *r_ptr = &r_info[r_idx];

	if(!IS_RANDOM_UNIQUE_IDX(r_idx))
	{
		msg_format(_("ERROR:apply_random_unique()がランダムユニーク以外のr_idx(%d)で呼ばれた",
                    "ERROR: apply_random_unique() called for a non-random unique r_idx (%d)"),r_idx);
		return FALSE;
	}

	//新闘技場
	if(p_ptr->inside_battle)
	{
		lev = battle_mon_base_level / 2 + randint1(battle_mon_base_level / 2) ;
	}
	//v1.1.42 夢の世界の＠
	else if (p_ptr->inside_quest == QUEST_DREAMDWELLER)
	{
		lev = dun_level;
	}
	//v1.1.51 新アリーナに出る夢の世界の＠
	else if (p_ptr->inside_arena)
	{
		lev = nightmare_mon_base_level;
	}
	//v1.1.86アビリティカード「アイテムの季節」でリリーホワイトを呼ぶとき。生成フラグ兼呼び出しレベル記録変数。
	else if (flag_generate_lilywhite)
	{
		lev = flag_generate_lilywhite;

	}
	else
	{
		if(dun_level < 10) return FALSE; //10階未満では出ない　注：10レベル未満にならないわけではない
		/*:::レベルを決める　生成レベルに偏る*/
		///mod150403 乱数ミスってた。修正
		lev = randint1(monster_level);
		lev2 = randint1(monster_level);
		lev3 = randint1(monster_level);
		lev = MAX(lev,lev2);
		lev = MAX(lev,lev3);
		if(lev > 127) lev = 127;

	}
	if (cheat_hear) msg_format(_("ランダムユニーク生成レベル:%d", "Random unique generation level: %d"), lev);

	/*:::シンボルを決める*/
	symbol_num = get_random_unique_symbol(r_idx, lev);

	/*:::パワーを決める 10階で5-15、 30階で15-45、50階で35-100、100階で65-150、深層で極めつけに吹っ飛んで500といったところ　100超えれば大抵の深層ユニークより強い*/
	//v1.1.42 夢の世界の＠はスコア倍率で決まるようにしてみる。
	if (p_ptr->inside_quest == QUEST_DREAMDWELLER)
	{
		//200%で75、100%で100、50%で150、33%以下で最大値の200。
		//難易度LUNATICのときは元悪夢モードということで常に最大値にしてしまおう。
		power = 5000 / calc_player_score_mult() + 50;
		if (power > 200 || difficulty == DIFFICULTY_LUNATIC)
			power = 200;

	}
	else
	{
		power = r_unique_symbol_table[symbol_num].power_rate / 10 + randint1(5); //5-20
		while (!one_in_(7)) power++; //+10(20%) +30(1%)
		if (lev >= 40 && one_in_(13)) power *= 2;
		if (lev >= 70 && one_in_(666)) power *= 5;
		power = power * lev / 30;
		power = power * r_unique_symbol_table[symbol_num].power_rate / 100;
		power += randint0(lev / 5);
	}

	//v1.1.86
	if (flag_generate_lilywhite && power < lev) power = lev;

	if(power > 1000) power = 1000;
	if (cheat_hear) msg_format(_("ランダムユニーク生成パワー:%d", "Random unique generation power: %d"), power);

	if (special_idol_generate_type)
	{
		if (power < 30)
			msg_print(_("あまり出来が良くない。", "It didn't turn out very good."));
		else if (power < 150)
			msg_print(_("まずまずの出来だ。", "It's pretty decent."));
		else if (power < 300)
			msg_print(_("会心の出来だ！", "It's a great success!"));
		else
			msg_print(_("比類なき会心の出来だ！", "It's an unparalleled success!"));

	}

	/*:::スキル付与と命名のバイアスを決定する*/
	//v1.1.42 夢の世界の＠は別関数で処理するよう変更
	//v1.1.51 新アリーナも同じにする
	if(p_ptr->inside_quest == QUEST_DREAMDWELLER || p_ptr->inside_arena)
		bias = set_random_unique_bias_of_your_dream();
	else
		bias = get_random_unique_bias( r_idx, r_unique_symbol_table[symbol_num].mon_char);


	if(cheat_hear) msg_format(_("ランダムユニークバイアス:%d", "Random unique bias: %d"),bias);

	/*:::基礎パラメータを付与する*/
	apply_random_unique_base_param(r_ptr,r_idx,lev,symbol_num,bias, power);

	/*:::パワーの回数分スキルを付与する　上限は考慮するが重複は考慮しない。その分パワーは多め。*/
	apply_random_unique_skill(r_ptr,power,&bias);

	/*:::シンボルとバイアスに合わせてパラメータを調整する*/
	apply_random_unique_xtra_param(r_ptr,symbol_num, bias, power);

	//v1.1.42 夢の世界の＠はさらに調整を行う
	if (p_ptr->inside_quest == QUEST_DREAMDWELLER || p_ptr->inside_arena)
		apply_dream_dweller_xtra_param(r_ptr);

	/*:::ランダムユニークに命名する*/
	get_random_unique_name(r_idx, r_ptr, power, bias);

	//ドッペルゲンガー特殊処理(名前は上の関数内で処理した)
	if(flag_generate_doppelganger)
	{
		r_ptr->d_attr = TERM_L_DARK;
		r_ptr->x_attr = TERM_L_DARK;
		r_ptr->d_char = '@';
		r_ptr->x_char = '@';
		r_ptr->flags1 &= (RF1_FORCE_MAXHP | RF1_NEVER_BLOW | RF1_RAND_25 | RF1_RAND_50);
		r_ptr->flags3 |= (RF3_GEN_WARLIKE);
		r_ptr->flags3 &= (RF3_GEN_WARLIKE | RF3_NO_FEAR | RF3_NO_STUN | RF3_NO_CONF | RF3_NO_SLEEP);
		r_ptr->flags7 |= RF7_DOPPELGANGER;
		r_ptr->flags7 &= ~(RF7_AQUATIC | RF7_RIDING);
		r_ptr->mexp = 1;
	}
	//偶像特殊処理(名前は召喚後に処理する)
	if (special_idol_generate_type)
	{

		r_ptr->d_char = 'g';
		r_ptr->x_char = 'g';
		r_ptr->flags1 &= ~(RF1_MALE | RF1_FEMALE);
		r_ptr->flags3 = (RF3_NO_FEAR | RF3_NO_STUN | RF3_NO_CONF | RF3_NO_SLEEP | RF3_GEN_HUMAN | RF3_HANIWA);
		r_ptr->flags7 &= ~(RF7_AQUATIC | RF7_UNIQUE2); //モンスターが倒せるようにユニーク2フラグ外す
		r_ptr->mexp = 1;
	}



	return TRUE;

}















